// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */


#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <net/genetlink.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/socket.h>
#include <linux/ratelimit.h>
#include <linux/ipc_logging.h>
#include "ipa_api.h"
#include "../inc/ipa_stats_genl.h"

#define IPA_STATS_IPC_LOG_PAGES 50
#define IPA_STATS_LOG_RL_INTERVAL (5 * HZ)
#define IPA_STATS_LOG_RL_BURST 10

static void *ipa_stats_logbuf;

/*
 * All stats_mgr logging goes to IPC logs (no printk). Each call site has its
 * own ratelimit state. RATELIMIT_MSG_ON_RELEASE is required: without it
 * __ratelimit() itself printk()s "N callbacks suppressed" when the interval
 * expires. With it, that summary is deferred to ratelimit_state_exit(), which
 * is never called for these static states, so nothing reaches the kernel log.
 */
#define IPA_STATS_LOG(lvl, fmt, args...) \
	do { \
		static struct ratelimit_state _rs = \
			RATELIMIT_STATE_INIT_FLAGS(_rs, \
				IPA_STATS_LOG_RL_INTERVAL, \
				IPA_STATS_LOG_RL_BURST, \
				RATELIMIT_MSG_ON_RELEASE); \
		if (ipa_stats_logbuf && __ratelimit(&_rs)) \
			ipc_log_string(ipa_stats_logbuf, \
				lvl " %s:%d " fmt, __func__, __LINE__, \
				## args); \
	} while (0)

#define IPA_STATS_ERR(fmt, args...)  IPA_STATS_LOG("ERR", fmt, ## args)
#define IPA_STATS_WARN(fmt, args...) IPA_STATS_LOG("WARN", fmt, ## args)
#define IPA_STATS_DBG(fmt, args...)  IPA_STATS_LOG("DBG", fmt, ## args)

/* ipa_debugfs_get_root() is defined in ipa_debugfs.c and exported via EXPORT_SYMBOL */
#ifdef CONFIG_DEBUG_FS
extern struct dentry *ipa_debugfs_get_root(void);
#endif

#define IPA_STATS_GENL_ATTR_MAX  (__IPA_STATS_GENL_ATTR_MAX - 1)

/* Base timer tick; every client timer_ticks must be a positive multiple. */
#define IPA_STATS_BASE_MS  100u

/*
 * Max snapshot entries per IPA_STATS_GENL_ATTR_STATS_SNAPSHOT attribute.
 * A single nlattr payload is bounded by the u16 nla_len field (~64KB), so a
 * large snapshot is split across several attributes in one message; the client
 * concatenates them. 512 * sizeof(entry) stays well under the u16 limit.
 */
#define IPA_STATS_SNAPSHOT_CHUNK  512u

#define IPA_STATS_GENL_OP(_cmd, _func)	\
	{				\
		.cmd    = (_cmd),	\
		.doit   = (_func),	\
		.dumpit = NULL,		\
		.flags  = 0,		\
	}

/*
 * Everything from here to the genl family definition is the tuple-list / timer
 * / multicast engine and the netlink handlers. It compiles ONLY into ipa_stats.ko
 * (which owns the state and the genl family). ipam.ko (-DIPA_STATS_PROC_ONLY)
 * skips it and gets only the flush stub further down.
 */
#ifndef IPA_STATS_PROC_ONLY
static const struct nla_policy
ipa_stats_genl_policy[IPA_STATS_GENL_ATTR_MAX + 1] = {
	[IPA_STATS_GENL_ATTR_TUPLE_INFO] =
		NLA_POLICY_EXACT_LEN(sizeof(struct ipa_stats_tuple_info)),
	[IPA_STATS_GENL_ATTR_STATS_SNAPSHOT] = { .type = NLA_BINARY },
	[IPA_STATS_GENL_ATTR_NOTIFY_ENABLE]  = { .type = NLA_U8 },
};

struct ipa_stats_tuple_entry {
	struct ipa_stats_tuple_info tuple;
	struct list_head            node;
};

/*
 * One bucket per unique timer_ticks value. Allocated on first ADD that
 * requests this period; freed when the last tuple in this bucket is removed.
 */
struct ipa_stats_flow_ctx {
	struct list_head    node;        /* link in ipa_stats_ctx_list */
	struct list_head    tuples;      /* tuples bound to this timer */
	struct mutex        lock;        /* protects tuples */
	u32                 timer_ticks; /* in 1 ms units; multiple of base */
	u32                 divisor;     /* timer_ticks / IPA_STATS_BASE_MS */
	u32                 tick_count;  /* base ticks elapsed since last emit */
};

static LIST_HEAD(ipa_stats_ctx_list);
static DEFINE_MUTEX(ipa_stats_ctx_list_lock);
static bool ipa_stats_notify_enabled = true;
static u32  ipa_stats_v4_count;
static u32  ipa_stats_v6_count;
static struct dentry *ipa_stats_debugfs_root;

/* Shared base timer. ipa_stats_tick_running is guarded by ipa_stats_ctx_list_lock. */
static struct delayed_work ipa_stats_tick_work;
static bool ipa_stats_tick_running;

/*
 * Serializes ipa_stats_process_notify_set() against itself: the disable path
 * drops ipa_stats_ctx_list_lock to call cancel_delayed_work_sync(), and a racing
 * enable could otherwise see a stale ipa_stats_tick_running and skip rescheduling.
 * Nests outside ipa_stats_ctx_list_lock; never taken on the ADD/DEL paths.
 */
static DEFINE_MUTEX(ipa_stats_notify_lock);

static const struct genl_multicast_group ipa_stats_mcgrps[] = {
	{ .name = IPA_STATS_GENL_MCGRP_NAME },
};

/* tentative declaration — full definition follows the ops array */
static struct genl_family ipa_stats_genl_family;

/* forward declaration for the base-timer work function */
static void ipa_stats_tick_work_fn(struct work_struct *work);

/* Caller must hold ipa_stats_ctx_list_lock. */
static void ipa_stats_tick_kick_locked(void)
{
	if (!ipa_stats_tick_running && ipa_stats_notify_enabled &&
	    !list_empty(&ipa_stats_ctx_list)) {
		ipa_stats_tick_running = true;
		IPA_STATS_DBG("tick: starting base timer (%u ms)\n", IPA_STATS_BASE_MS);
		schedule_delayed_work(&ipa_stats_tick_work,
				      msecs_to_jiffies(IPA_STATS_BASE_MS));
	}
}

/* caller must hold ipa_stats_ctx_list_lock */
static struct ipa_stats_flow_ctx *
ipa_stats_ctx_find_locked(u32 timer_ticks)
{
	struct ipa_stats_flow_ctx *ctx;

	list_for_each_entry(ctx, &ipa_stats_ctx_list, node) {
		if (ctx->timer_ticks == timer_ticks)
			return ctx;
	}
	return NULL;
}

/* Copy ctx's tuples into a freshly allocated array (no HW access). Returns the
 * entry count (0 if empty or on alloc failure); *out is NULL when 0.
 */
static int ipa_stats_collect_snapshot(struct ipa_stats_flow_ctx *ctx,
				      struct ipa_stats_snapshot_entry **out)
{
	struct ipa_stats_tuple_entry *entry;
	struct ipa_stats_snapshot_entry *snapshot;
	int count = 0, i = 0;

	*out = NULL;

	mutex_lock(&ctx->lock);
	list_for_each_entry(entry, &ctx->tuples, node)
		count++;

	if (!count) {
		mutex_unlock(&ctx->lock);
		return 0;
	}

	snapshot = kmalloc_array(count, sizeof(*snapshot), GFP_KERNEL);
	if (!snapshot) {
		mutex_unlock(&ctx->lock);
		IPA_STATS_ERR("kmalloc_array failed\n");
		return 0;
	}

	list_for_each_entry(entry, &ctx->tuples, node) {
		snapshot[i].tuple = entry->tuple;
		i++;
	}
	mutex_unlock(&ctx->lock);

	*out = snapshot;
	return count;
}

/* Query HW, build and multicast the snapshot, then free it. Must run with no
 * locks held: ipa_be_query_tuple_stats() takes BE locks and does a DMA read.
 */
static void ipa_stats_send_snapshot(struct ipa_stats_snapshot_entry *snapshot,
				    int count)
{
	struct sk_buff *skb;
	void *hdr;
	size_t skb_size;
	int i, off;

	if (!snapshot || !count)
		return;

	/* Zero on any failure rather than report stale data. */
	for (i = 0; i < count; i++) {
		if (ipa_be_query_tuple_stats(&snapshot[i].tuple,
					     &snapshot[i].stats))
			memset(&snapshot[i].stats, 0, sizeof(snapshot[i].stats));
	}

	/*
	 * Split into multiple snapshot attributes of at most
	 * IPA_STATS_SNAPSHOT_CHUNK entries each: a single nlattr payload is
	 * bounded by the u16 nla_len, so one giant attribute would overflow
	 * for large connection counts. The client concatenates all attrs.
	 */
	skb_size = 0;
	for (off = 0; off < count; off += IPA_STATS_SNAPSHOT_CHUNK) {
		int chunk = min_t(int, IPA_STATS_SNAPSHOT_CHUNK, count - off);

		skb_size += nla_total_size(chunk * sizeof(*snapshot));
	}

	skb = genlmsg_new(skb_size, GFP_KERNEL);
	if (!skb) {
		IPA_STATS_ERR("genlmsg_new failed\n");
		kfree(snapshot);
		return;
	}

	hdr = genlmsg_put(skb, 0, 0, &ipa_stats_genl_family, 0,
			  IPA_STATS_GENL_CMD_STATS_UPDATE);
	if (!hdr) {
		IPA_STATS_ERR("genlmsg_put failed\n");
		nlmsg_free(skb);
		kfree(snapshot);
		return;
	}

	for (off = 0; off < count; off += IPA_STATS_SNAPSHOT_CHUNK) {
		int chunk = min_t(int, IPA_STATS_SNAPSHOT_CHUNK, count - off);

		if (nla_put(skb, IPA_STATS_GENL_ATTR_STATS_SNAPSHOT,
			    (int)(chunk * sizeof(*snapshot)), &snapshot[off])) {
			IPA_STATS_ERR("nla_put failed\n");
			genlmsg_cancel(skb, hdr);
			nlmsg_free(skb);
			kfree(snapshot);
			return;
		}
	}

	kfree(snapshot);
	genlmsg_end(skb, hdr);
	genlmsg_multicast(&ipa_stats_genl_family, skb, 0, 0, GFP_KERNEL);
	IPA_STATS_DBG("snapshot sent: %d entr%s\n", count, count == 1 ? "y" : "ies");
}

/* Build and multicast one snapshot for ctx. Caller must not hold ctx->lock. */
static void ipa_stats_emit_snapshot(struct ipa_stats_flow_ctx *ctx)
{
	struct ipa_stats_snapshot_entry *snapshot;
	int count;

	count = ipa_stats_collect_snapshot(ctx, &snapshot);
	ipa_stats_send_snapshot(snapshot, count);
}

struct ipa_stats_pending_snapshot {
	struct list_head                 node;
	struct ipa_stats_snapshot_entry *snapshot;
	int                              count;
};

/*
 * Single base timer. Advances each bucket's tick_count and, for buckets whose
 * period elapsed, copies their tuples (under ctx->lock) onto a local list, then
 * does the HW query + multicast after dropping all locks. Copying under the
 * lock keeps this safe against a concurrent DEL/FLUSH freeing the bucket.
 */
static void ipa_stats_tick_work_fn(struct work_struct *work)
{
	struct ipa_stats_flow_ctx *ctx;
	struct ipa_stats_pending_snapshot *pending, *tmp;
	LIST_HEAD(pending_list);

	mutex_lock(&ipa_stats_ctx_list_lock);

	list_for_each_entry(ctx, &ipa_stats_ctx_list, node) {
		struct ipa_stats_snapshot_entry *snapshot;
		int count;

		if (++ctx->tick_count < ctx->divisor)
			continue;

		ctx->tick_count = 0;

		count = ipa_stats_collect_snapshot(ctx, &snapshot);
		if (!count) {
			IPA_STATS_DBG("tick: timer_ticks=%u elapsed, bucket empty, skipping\n",
				 ctx->timer_ticks);
			continue;
		}

		IPA_STATS_DBG("tick: timer_ticks=%u elapsed, queuing %d entr%s for send\n",
			 ctx->timer_ticks, count, count == 1 ? "y" : "ies");

		pending = kzalloc(sizeof(*pending), GFP_KERNEL);
		if (!pending) {
			IPA_STATS_ERR("pending kzalloc failed\n");
			kfree(snapshot);
			continue;
		}
		pending->snapshot = snapshot;
		pending->count = count;
		list_add_tail(&pending->node, &pending_list);
	}

	if (!list_empty(&ipa_stats_ctx_list) && ipa_stats_notify_enabled)
		schedule_delayed_work(&ipa_stats_tick_work,
				      msecs_to_jiffies(IPA_STATS_BASE_MS));
	else {
		IPA_STATS_DBG("tick: stopping base timer (ctx_list_empty=%d notify_enabled=%d)\n",
			 list_empty(&ipa_stats_ctx_list), ipa_stats_notify_enabled);
		ipa_stats_tick_running = false;
	}

	mutex_unlock(&ipa_stats_ctx_list_lock);

	list_for_each_entry_safe(pending, tmp, &pending_list, node) {
		list_del(&pending->node);
		ipa_stats_send_snapshot(pending->snapshot, pending->count);
		kfree(pending);
	}
}

static bool ipa_stats_tuple_match(const struct ipa_stats_tuple_info *a,
				   const struct ipa_stats_tuple_info *b)
{
	return a->af       == b->af       &&
	       a->src_port == b->src_port &&
	       a->dst_port == b->dst_port &&
	       a->protocol == b->protocol &&
	       !memcmp(a->src_addr, b->src_addr, sizeof(a->src_addr)) &&
	       !memcmp(a->dst_addr, b->dst_addr, sizeof(a->dst_addr));
}

static int ipa_stats_tuple_add_req_hdlr(struct sk_buff *skb,
					 struct genl_info *info)
{
	struct ipa_stats_tuple_info tuple;

	if (!info->attrs[IPA_STATS_GENL_ATTR_TUPLE_INFO]) {
		IPA_STATS_ERR("missing tuple info attribute\n");
		return -EINVAL;
	}

	nla_memcpy(&tuple, info->attrs[IPA_STATS_GENL_ATTR_TUPLE_INFO],
		   sizeof(tuple));

	if (!tuple.src_port || !tuple.dst_port) {
		IPA_STATS_ERR("zero port: src_port=%u dst_port=%u\n",
		       ntohs(tuple.src_port), ntohs(tuple.dst_port));
		return -EINVAL;
	}

	if (tuple.af == AF_INET) {
		if (!tuple.src_addr[0] || !tuple.dst_addr[0]) {
			IPA_STATS_ERR("zero v4 addr: src=%pI4 dst=%pI4\n",
			       &tuple.src_addr[0], &tuple.dst_addr[0]);
			return -EINVAL;
		}
	} else if (tuple.af == AF_INET6) {
		if ((!tuple.src_addr[0] && !tuple.src_addr[1] &&
		     !tuple.src_addr[2] && !tuple.src_addr[3]) ||
		    (!tuple.dst_addr[0] && !tuple.dst_addr[1] &&
		     !tuple.dst_addr[2] && !tuple.dst_addr[3])) {
			IPA_STATS_ERR("zero v6 addr: src=%pI6c dst=%pI6c\n",
			       (struct in6_addr *)tuple.src_addr,
			       (struct in6_addr *)tuple.dst_addr);
			return -EINVAL;
		}
	} else {
		IPA_STATS_ERR("unsupported af %u\n", tuple.af);
		return -EINVAL;
	}

	if (tuple.protocol != IPPROTO_TCP && tuple.protocol != IPPROTO_UDP) {
		IPA_STATS_ERR("invalid protocol %u\n", tuple.protocol);
		return -EINVAL;
	}

	if (!tuple.timer_ticks || (tuple.timer_ticks % IPA_STATS_BASE_MS)) {
		IPA_STATS_ERR("timer_ticks %u not a positive multiple of %u ms\n",
		       tuple.timer_ticks, IPA_STATS_BASE_MS);
		return -EINVAL;
	}

	if (tuple.af == AF_INET)
		IPA_STATS_DBG("add: src=%pI4:%u dst=%pI4:%u proto=%u timer_ticks=%u\n",
		         &tuple.src_addr[0], ntohs(tuple.src_port),
		         &tuple.dst_addr[0], ntohs(tuple.dst_port),
		         tuple.protocol, tuple.timer_ticks);
	else
		IPA_STATS_DBG("add: src=%pI6c:%u dst=%pI6c:%u proto=%u timer_ticks=%u\n",
		         (struct in6_addr *)tuple.src_addr, ntohs(tuple.src_port),
		         (struct in6_addr *)tuple.dst_addr, ntohs(tuple.dst_port),
		         tuple.protocol, tuple.timer_ticks);

	/*
	 * Return the add result so genl sends an ACK (0) or NLMSG_ERROR
	 * (e.g. -ENOSPC when the max-accel-conn cap is reached) back to the
	 * caller, instead of silently dropping over-cap tuples.
	 */
	return ipa_stats_process_tuple_add(&tuple);
}

static int ipa_stats_tuple_del_req_hdlr(struct sk_buff *skb,
					 struct genl_info *info)
{
	struct ipa_stats_tuple_info tuple;

	if (!info->attrs[IPA_STATS_GENL_ATTR_TUPLE_INFO]) {
		IPA_STATS_ERR("missing tuple info attribute\n");
		return -EINVAL;
	}

	nla_memcpy(&tuple, info->attrs[IPA_STATS_GENL_ATTR_TUPLE_INFO],
		   sizeof(tuple));

	if (!tuple.src_port || !tuple.dst_port) {
		IPA_STATS_ERR("zero port: src_port=%u dst_port=%u\n",
		       ntohs(tuple.src_port), ntohs(tuple.dst_port));
		return -EINVAL;
	}

	if (tuple.af == AF_INET) {
		if (!tuple.src_addr[0] || !tuple.dst_addr[0]) {
			IPA_STATS_ERR("zero v4 addr: src=%pI4 dst=%pI4\n",
			       &tuple.src_addr[0], &tuple.dst_addr[0]);
			return -EINVAL;
		}
	} else if (tuple.af == AF_INET6) {
		if ((!tuple.src_addr[0] && !tuple.src_addr[1] &&
		     !tuple.src_addr[2] && !tuple.src_addr[3]) ||
		    (!tuple.dst_addr[0] && !tuple.dst_addr[1] &&
		     !tuple.dst_addr[2] && !tuple.dst_addr[3])) {
			IPA_STATS_ERR("zero v6 addr: src=%pI6c dst=%pI6c\n",
			       (struct in6_addr *)tuple.src_addr,
			       (struct in6_addr *)tuple.dst_addr);
			return -EINVAL;
		}
	} else {
		IPA_STATS_ERR("unsupported af %u\n", tuple.af);
		return -EINVAL;
	}

	if (tuple.protocol != IPPROTO_TCP && tuple.protocol != IPPROTO_UDP) {
		IPA_STATS_ERR("invalid protocol %u\n", tuple.protocol);
		return -EINVAL;
	}

	if (tuple.af == AF_INET)
		IPA_STATS_DBG("del: src=%pI4:%u dst=%pI4:%u proto=%u\n",
		         &tuple.src_addr[0], ntohs(tuple.src_port),
		         &tuple.dst_addr[0], ntohs(tuple.dst_port),
		         tuple.protocol);
	else
		IPA_STATS_DBG("del: src=%pI6c:%u dst=%pI6c:%u proto=%u\n",
		         (struct in6_addr *)tuple.src_addr, ntohs(tuple.src_port),
		         (struct in6_addr *)tuple.dst_addr, ntohs(tuple.dst_port),
		         tuple.protocol);

	ipa_stats_process_tuple_del(&tuple);

	return 0;
}

static int ipa_stats_tuple_flush_req_hdlr(struct sk_buff *skb,
					   struct genl_info *info)
{
	IPA_STATS_DBG("flush request received\n");
	ipa_stats_process_tuple_flush();
	return 0;
}

static int ipa_stats_notify_set_req_hdlr(struct sk_buff *skb,
					  struct genl_info *info)
{
	u8 enable;

	if (!info->attrs[IPA_STATS_GENL_ATTR_NOTIFY_ENABLE]) {
		IPA_STATS_ERR("missing notify enable attribute\n");
		return -EINVAL;
	}

	enable = nla_get_u8(info->attrs[IPA_STATS_GENL_ATTR_NOTIFY_ENABLE]);
	ipa_stats_process_notify_set(!!enable);
	return 0;
}

static int ipa_stats_query_stats_req_hdlr(struct sk_buff *skb,
					   struct genl_info *info)
{
	struct ipa_stats_flow_ctx *ctx;
	struct ipa_stats_tuple_entry *entry;
	struct ipa_stats_snapshot_entry *snapshot = NULL;
	struct sk_buff *resp;
	void *hdr;
	int total = 0, i = 0, ret = 0, off;
	size_t skb_size;

	IPA_STATS_DBG("query_stats request received\n");

	mutex_lock(&ipa_stats_ctx_list_lock);

	list_for_each_entry(ctx, &ipa_stats_ctx_list, node) {
		mutex_lock(&ctx->lock);
		list_for_each_entry(entry, &ctx->tuples, node)
			total++;
		mutex_unlock(&ctx->lock);
	}

	if (total) {
		snapshot = kmalloc_array(total, sizeof(*snapshot), GFP_KERNEL);
		if (!snapshot) {
			mutex_unlock(&ipa_stats_ctx_list_lock);
			IPA_STATS_ERR("kmalloc_array failed\n");
			return -ENOMEM;
		}

		list_for_each_entry(ctx, &ipa_stats_ctx_list, node) {
			mutex_lock(&ctx->lock);
			list_for_each_entry(entry, &ctx->tuples, node) {
				snapshot[i].tuple = entry->tuple;
				i++;
			}
			mutex_unlock(&ctx->lock);
		}
	}

	mutex_unlock(&ipa_stats_ctx_list_lock);

	/*
	 * Query real HW stats after dropping all genl locks:
	 * ipa_be_query_tuple_stats() takes the BE cache locks and issues a DMA
	 * read, so it must not run under ipa_stats_ctx_list_lock / ctx->lock.
	 * Zero the entry on any failure rather than report stale data.
	 */
	for (i = 0; i < total; i++) {
		if (ipa_be_query_tuple_stats(&snapshot[i].tuple,
					     &snapshot[i].stats))
			memset(&snapshot[i].stats, 0, sizeof(snapshot[i].stats));
	}

	skb_size = 0;
	for (off = 0; off < total; off += IPA_STATS_SNAPSHOT_CHUNK) {
		int chunk = min_t(int, IPA_STATS_SNAPSHOT_CHUNK, total - off);

		skb_size += nla_total_size(chunk * sizeof(*snapshot));
	}
	resp = genlmsg_new(skb_size, GFP_KERNEL);
	if (!resp) {
		kfree(snapshot);
		return -ENOMEM;
	}

	hdr = genlmsg_put_reply(resp, info, &ipa_stats_genl_family, 0,
				IPA_STATS_GENL_CMD_STATS_UPDATE);
	if (!hdr) {
		nlmsg_free(resp);
		kfree(snapshot);
		return -ENOMEM;
	}

	/*
	 * Emit the snapshot as multiple attributes of at most
	 * IPA_STATS_SNAPSHOT_CHUNK entries each (u16 nla_len bound); the client
	 * concatenates them.
	 */
	for (off = 0; off < total; off += IPA_STATS_SNAPSHOT_CHUNK) {
		int chunk = min_t(int, IPA_STATS_SNAPSHOT_CHUNK, total - off);

		if (nla_put(resp, IPA_STATS_GENL_ATTR_STATS_SNAPSHOT,
			    (int)(chunk * sizeof(*snapshot)), &snapshot[off])) {
			genlmsg_cancel(resp, hdr);
			nlmsg_free(resp);
			kfree(snapshot);
			return -EMSGSIZE;
		}
	}

	kfree(snapshot);
	genlmsg_end(resp, hdr);

	ret = genlmsg_reply(resp, info);
	if (ret)
		IPA_STATS_ERR("genlmsg_reply failed: %d\n", ret);
	return ret;
}

static const struct genl_ops ipa_stats_genl_ops[] = {
	IPA_STATS_GENL_OP(IPA_STATS_GENL_CMD_TUPLE_ADD,
			  ipa_stats_tuple_add_req_hdlr),
	IPA_STATS_GENL_OP(IPA_STATS_GENL_CMD_TUPLE_DEL,
			  ipa_stats_tuple_del_req_hdlr),
	IPA_STATS_GENL_OP(IPA_STATS_GENL_CMD_TUPLE_FLUSH,
			  ipa_stats_tuple_flush_req_hdlr),
	IPA_STATS_GENL_OP(IPA_STATS_GENL_CMD_NOTIFY_SET,
			  ipa_stats_notify_set_req_hdlr),
	IPA_STATS_GENL_OP(IPA_STATS_GENL_CMD_QUERY_STATS,
			  ipa_stats_query_stats_req_hdlr),
};

static struct genl_family ipa_stats_genl_family = {
	.name     = IPA_STATS_GENL_FAMILY_NAME,
	.version  = 1,
	.maxattr  = IPA_STATS_GENL_ATTR_MAX,
	.policy   = ipa_stats_genl_policy,
	.ops      = ipa_stats_genl_ops,
	.n_ops    = ARRAY_SIZE(ipa_stats_genl_ops),
	.mcgrps   = ipa_stats_mcgrps,
	.n_mcgrps = ARRAY_SIZE(ipa_stats_mcgrps),
};

/*
 * The ipa_stats_process_* functions below are the real tuple-list / timer /
 * multicast engine, continuing in the same ipa_stats.ko-only region opened
 * above. ipam.ko (-DIPA_STATS_PROC_ONLY) instead gets the flush stub in the
 * #else branch.
 *
 * NOTE (bug fix): these used to be compiled into ipam.ko (via
 * -DIPA_STATS_PROC_ONLY) and exported, with the genl handlers in ipa_stats.ko
 * calling them. But the file-scope statics (ctx_list, tick_work, counts) were
 * then duplicated per module — adds landed in ipam.ko's copy while the query
 * and timer tick read ipa_stats.ko's always-empty copy, so no snapshot was
 * ever emitted. Keeping all state + all readers/writers in one module (this,
 * ipa_stats.ko) fixes it; ipam.ko only forwards flush via a registered cb.
 */
int ipa_stats_process_tuple_add(struct ipa_stats_tuple_info *tuple)
{
	struct ipa_stats_flow_ctx *ctx;
	struct ipa_stats_tuple_entry *entry, *new_entry;

	/* Re-check here: this symbol is exported, so direct callers bypass the
	 * netlink handler's validation and could otherwise truncate via divisor.
	 */
	if (!tuple->timer_ticks || (tuple->timer_ticks % IPA_STATS_BASE_MS)) {
		IPA_STATS_ERR("timer_ticks %u not a positive multiple of %u ms; rejecting ADD\n",
		       tuple->timer_ticks, IPA_STATS_BASE_MS);
		return -EINVAL;
	}

	if (tuple->af == AF_INET)
		IPA_STATS_DBG("add entry received: src=%pI4:%u dst=%pI4:%u proto=%u timer_ticks=%u\n",
		         &tuple->src_addr[0], ntohs(tuple->src_port),
		         &tuple->dst_addr[0], ntohs(tuple->dst_port),
		         tuple->protocol, tuple->timer_ticks);
	else
		IPA_STATS_DBG("add entry received: src=%pI6c:%u dst=%pI6c:%u proto=%u timer_ticks=%u\n",
		         (struct in6_addr *)tuple->src_addr, ntohs(tuple->src_port),
		         (struct in6_addr *)tuple->dst_addr, ntohs(tuple->dst_port),
		         tuple->protocol, tuple->timer_ticks);

	new_entry = kzalloc(sizeof(*new_entry), GFP_KERNEL);
	if (!new_entry) {
		IPA_STATS_ERR("kzalloc failed\n");
		return -ENOMEM;
	}
	new_entry->tuple = *tuple;
	INIT_LIST_HEAD(&new_entry->node);

	mutex_lock(&ipa_stats_ctx_list_lock);

	if (tuple->af == AF_INET) {
		if (ipa_stats_v4_count >= (u32)ipa_be_ipv4_get_max_accel_conn()) {
			mutex_unlock(&ipa_stats_ctx_list_lock);
			IPA_STATS_DBG("max v4 filters reached (%d); rejecting ADD\n",
				 ipa_be_ipv4_get_max_accel_conn());
			kfree(new_entry);
			return -ENOSPC;
		}
	} else {
		if (ipa_stats_v6_count >= (u32)ipa_be_ipv6_get_max_accel_conn()) {
			mutex_unlock(&ipa_stats_ctx_list_lock);
			IPA_STATS_DBG("max v6 filters reached (%d); rejecting ADD\n",
				 ipa_be_ipv6_get_max_accel_conn());
			kfree(new_entry);
			return -ENOSPC;
		}
	}

	ctx = ipa_stats_ctx_find_locked(tuple->timer_ticks);
	if (!ctx) {
		ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
		if (!ctx) {
			mutex_unlock(&ipa_stats_ctx_list_lock);
			IPA_STATS_ERR("ctx kzalloc failed\n");
			kfree(new_entry);
			return -ENOMEM;
		}
		INIT_LIST_HEAD(&ctx->node);
		INIT_LIST_HEAD(&ctx->tuples);
		mutex_init(&ctx->lock);
		ctx->timer_ticks = tuple->timer_ticks;
		ctx->divisor = tuple->timer_ticks / IPA_STATS_BASE_MS;
		ctx->tick_count = 0;
		list_add_tail(&ctx->node, &ipa_stats_ctx_list);
	}

	mutex_lock(&ctx->lock);

	list_for_each_entry(entry, &ctx->tuples, node) {
		if (ipa_stats_tuple_match(&entry->tuple, tuple)) {
			mutex_unlock(&ctx->lock);
			mutex_unlock(&ipa_stats_ctx_list_lock);
			IPA_STATS_WARN("duplicate ADD for existing tuple, ignoring\n");
			kfree(new_entry);
			return -EEXIST;
		}
	}

	list_add_tail(&new_entry->node, &ctx->tuples);
	if (tuple->af == AF_INET)
		ipa_stats_v4_count++;
	else
		ipa_stats_v6_count++;

	mutex_unlock(&ctx->lock);

	ipa_stats_tick_kick_locked();

	mutex_unlock(&ipa_stats_ctx_list_lock);
	return 0;
}

void ipa_stats_process_tuple_del(struct ipa_stats_tuple_info *tuple)
{
	struct ipa_stats_flow_ctx *ctx;
	struct ipa_stats_tuple_entry *entry, *tmp;

	if (tuple->af == AF_INET)
		IPA_STATS_DBG("del entry received: src=%pI4:%u dst=%pI4:%u proto=%u\n",
		         &tuple->src_addr[0], ntohs(tuple->src_port),
		         &tuple->dst_addr[0], ntohs(tuple->dst_port),
		         tuple->protocol);
	else
		IPA_STATS_DBG("del entry received: src=%pI6c:%u dst=%pI6c:%u proto=%u\n",
		         (struct in6_addr *)tuple->src_addr, ntohs(tuple->src_port),
		         (struct in6_addr *)tuple->dst_addr, ntohs(tuple->dst_port),
		         tuple->protocol);

	mutex_lock(&ipa_stats_ctx_list_lock);
	list_for_each_entry(ctx, &ipa_stats_ctx_list, node) {
		mutex_lock(&ctx->lock);
		list_for_each_entry_safe(entry, tmp, &ctx->tuples, node) {
			if (!ipa_stats_tuple_match(&entry->tuple, tuple))
				continue;

			list_del_init(&entry->node);
			if (entry->tuple.af == AF_INET)
				ipa_stats_v4_count--;
			else
				ipa_stats_v6_count--;
			kfree(entry);

			if (list_empty(&ctx->tuples)) {
				/* detach under both locks so no ADD races onto
				 * this ctx, then tear down outside the locks.
				 */
				list_del_init(&ctx->node);
				mutex_unlock(&ctx->lock);
				mutex_unlock(&ipa_stats_ctx_list_lock);
				mutex_destroy(&ctx->lock);
				kfree(ctx);
				return;
			}
			mutex_unlock(&ctx->lock);
			mutex_unlock(&ipa_stats_ctx_list_lock);
			return;
		}
		mutex_unlock(&ctx->lock);
	}
	mutex_unlock(&ipa_stats_ctx_list_lock);

	if (tuple->af == AF_INET)
		IPA_STATS_ERR("tuple not found: src=%pI4:%u dst=%pI4:%u proto=%u\n",
		       &tuple->src_addr[0], ntohs(tuple->src_port),
		       &tuple->dst_addr[0], ntohs(tuple->dst_port),
		       tuple->protocol);
	else
		IPA_STATS_ERR("tuple not found: src=%pI6c:%u dst=%pI6c:%u proto=%u\n",
		       (struct in6_addr *)tuple->src_addr, ntohs(tuple->src_port),
		       (struct in6_addr *)tuple->dst_addr, ntohs(tuple->dst_port),
		       tuple->protocol);
}

void ipa_stats_process_tuple_flush(void)
{
	struct ipa_stats_flow_ctx *ctx;
	struct ipa_stats_tuple_entry *entry, *tmp;

	IPA_STATS_DBG("flush all tuples\n");

	for (;;) {
		unsigned int n4 = 0, n6 = 0;

		mutex_lock(&ipa_stats_ctx_list_lock);
		ctx = list_first_entry_or_null(&ipa_stats_ctx_list,
					       struct ipa_stats_flow_ctx,
					       node);
		if (!ctx) {
			mutex_unlock(&ipa_stats_ctx_list_lock);
			break;
		}
		/* detach so concurrent ADDs for this timer alloc a fresh ctx
		 * instead of racing onto this one as we tear it down.
		 */
		list_del_init(&ctx->node);

		mutex_lock(&ctx->lock);
		list_for_each_entry(entry, &ctx->tuples, node) {
			if (entry->tuple.af == AF_INET)
				n4++;
			else
				n6++;
		}
		mutex_unlock(&ctx->lock);
		ipa_stats_v4_count -= n4;
		ipa_stats_v6_count -= n6;
		mutex_unlock(&ipa_stats_ctx_list_lock);

		ipa_stats_emit_snapshot(ctx);

		mutex_lock(&ctx->lock);
		list_for_each_entry_safe(entry, tmp, &ctx->tuples, node) {
			list_del_init(&entry->node);
			kfree(entry);
		}
		mutex_unlock(&ctx->lock);

		mutex_destroy(&ctx->lock);
		kfree(ctx);
	}
}

void ipa_stats_process_notify_set(bool enable)
{
	IPA_STATS_DBG("notify_set: %s\n", enable ? "enable" : "silence");

	mutex_lock(&ipa_stats_notify_lock);

	mutex_lock(&ipa_stats_ctx_list_lock);

	if (ipa_stats_notify_enabled == enable) {
		mutex_unlock(&ipa_stats_ctx_list_lock);
		mutex_unlock(&ipa_stats_notify_lock);
		return;
	}

	ipa_stats_notify_enabled = enable;

	if (enable) {
		ipa_stats_tick_kick_locked();
		mutex_unlock(&ipa_stats_ctx_list_lock);
		mutex_unlock(&ipa_stats_notify_lock);
		return;
	}

	/* Drop the list lock before cancelling: the work fn takes it. The flag
	 * is already cleared, so a mid-run work fn won't reschedule.
	 */
	mutex_unlock(&ipa_stats_ctx_list_lock);

	cancel_delayed_work_sync(&ipa_stats_tick_work);

	mutex_lock(&ipa_stats_ctx_list_lock);
	ipa_stats_tick_running = false;
	mutex_unlock(&ipa_stats_ctx_list_lock);

	mutex_unlock(&ipa_stats_notify_lock);
}
#else  /* IPA_STATS_PROC_ONLY (ipam.ko) */
/*
 * ipam.ko side: only a flush stub that forwards to the real implementation
 * registered by ipa_stats.ko. ipam.ko's datapath teardown (ipa_be_reset)
 * calls ipa_stats_process_tuple_flush(); before ipa_stats.ko loads and
 * registers, this is a safe no-op.
 */
static void (*ipa_stats_flush_cb)(void);
/*
 * Serialises callback registration against invocation. The registered callback
 * (ipa_stats_process_tuple_flush) takes sleeping locks, so a sleepable lock is
 * required here — rcu_read_lock() is not valid.
 */
static DEFINE_MUTEX(ipa_stats_flush_cb_lock);

void ipa_stats_register_flush_cb(void (*flush_cb)(void))
{
	mutex_lock(&ipa_stats_flush_cb_lock);
	ipa_stats_flush_cb = flush_cb;
	mutex_unlock(&ipa_stats_flush_cb_lock);
}
EXPORT_SYMBOL_GPL(ipa_stats_register_flush_cb);

void ipa_stats_process_tuple_flush(void)
{
	void (*cb)(void);

	mutex_lock(&ipa_stats_flush_cb_lock);
	cb = ipa_stats_flush_cb;
	if (cb)
		cb();
	mutex_unlock(&ipa_stats_flush_cb_lock);
}
EXPORT_SYMBOL_GPL(ipa_stats_process_tuple_flush);
#endif /* IPA_STATS_PROC_ONLY */


#ifndef IPA_STATS_PROC_ONLY
static int __init ipa_stats_genl_init(void)
{
	int ret;

#ifdef CONFIG_DEBUG_FS
	ipa_stats_debugfs_root = debugfs_create_dir("ipa_stats",
						    ipa_debugfs_get_root());
#endif

	INIT_DELAYED_WORK(&ipa_stats_tick_work, ipa_stats_tick_work_fn);

	ipa_stats_logbuf = ipc_log_context_create(IPA_STATS_IPC_LOG_PAGES,
						  "ipa_stats", 0);

	ret = genl_register_family(&ipa_stats_genl_family);
	if (ret) {
		IPA_STATS_ERR("genl_register_family failed: %d\n", ret);
		debugfs_remove_recursive(ipa_stats_debugfs_root);
		ipc_log_context_destroy(ipa_stats_logbuf);
		ipa_stats_logbuf = NULL;
		return ret;
	}

	/*
	 * Hand ipam.ko the real flush implementation so its datapath teardown
	 * (ipa_be_reset -> ipa_stats_process_tuple_flush) reaches this module's
	 * state. ipam.ko loads first, so it starts with a no-op flush stub and
	 * only forwards here once we register.
	 */
	ipa_stats_register_flush_cb(ipa_stats_process_tuple_flush);

	return 0;
}

static void __exit ipa_stats_genl_exit(void)
{
	struct ipa_stats_flow_ctx *ctx, *ctx_tmp;
	struct ipa_stats_tuple_entry *entry, *entry_tmp;
	int ret;

	/*
	 * Clear ipam.ko's flush callback before unloading. Taking
	 * ipa_stats_flush_cb_lock guarantees that any concurrent
	 * ipa_stats_process_tuple_flush() invocation has finished before we
	 * return and the module text is freed.
	 */
	ipa_stats_register_flush_cb(NULL);

	debugfs_remove_recursive(ipa_stats_debugfs_root);

	cancel_delayed_work_sync(&ipa_stats_tick_work);

	mutex_lock(&ipa_stats_ctx_list_lock);
	list_for_each_entry_safe(ctx, ctx_tmp, &ipa_stats_ctx_list, node) {
		list_del_init(&ctx->node);
		mutex_unlock(&ipa_stats_ctx_list_lock);

		mutex_lock(&ctx->lock);
		list_for_each_entry_safe(entry, entry_tmp, &ctx->tuples, node) {
			list_del_init(&entry->node);
			kfree(entry);
		}
		mutex_unlock(&ctx->lock);
		mutex_destroy(&ctx->lock);
		kfree(ctx);

		mutex_lock(&ipa_stats_ctx_list_lock);
	}
	mutex_unlock(&ipa_stats_ctx_list_lock);

	ret = genl_unregister_family(&ipa_stats_genl_family);
	if (ret)
		IPA_STATS_ERR("genl_unregister_family failed: %d\n", ret);

	ipc_log_context_destroy(ipa_stats_logbuf);
	ipa_stats_logbuf = NULL;
}

module_init(ipa_stats_genl_init);
module_exit(ipa_stats_genl_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("IPA Stats Generic Netlink Module");
#endif /* IPA_STATS_PROC_ONLY */
