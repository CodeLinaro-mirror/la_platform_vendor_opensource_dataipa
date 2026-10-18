// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017-2020, The Linux Foundation. All rights reserved.
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include "ipa_ut_framework.h"
#include "ipa_i.h"
#include "ipa_ipsec.h"
#include <linux/completion.h>
#include <linux/if_ether.h>
#include <linux/io.h>
#include <linux/ip.h>
#include <linux/mm.h>
#include <linux/netdevice.h>
#include <linux/udp.h>
#include <net/ip.h>
#include "ipahal_hw_stats_i.h"
#include "ipahal_i.h"

enum ipa_test_hw_stats_counter {
	IPA_TEST_HW_STATS_RT4_USB,
	IPA_TEST_HW_STATS_RT6_USB,
	IPA_TEST_HW_STATS_RT4_CONS,
	IPA_TEST_HW_STATS_RT6_CONS,
	IPA_TEST_HW_STATS_FLT4_USB,
	IPA_TEST_HW_STATS_FLT6_USB,
	IPA_TEST_HW_STATS_FLT4_PROD,
	IPA_TEST_HW_STATS_FLT6_PROD,
	IPA_TEST_HW_STATS_COUNTER_MAX,
};

struct ipa_test_hw_stats_ctx {
	u32 odu_prod_hdl;
	u32 odu_cons_hdl;
	enum ipa_client_type prod_client;
	enum ipa_client_type cons_client;
	u32 rt4_usb_rule_hdl;
	u32 rt6_usb_rule_hdl;
	u32 rt4_cons_rule_hdl;
	u32 rt6_cons_rule_hdl;
	u32 flt4_usb_rule_hdl;
	u32 flt6_usb_rule_hdl;
	u32 flt4_prod_rule_hdl;
	u32 flt6_prod_rule_hdl;
	int fnr_counter_hdl;
	atomic_t odu_pending;
	u8 hw_counter_start_id;
	bool odu_prod_pipe_setup;
	bool odu_cons_pipe_setup;
	bool rt4_usb_rule_added;
	bool rt6_usb_rule_added;
	bool rt4_cons_rule_added;
	bool rt6_cons_rule_added;
	bool flt4_usb_rule_added;
	bool flt6_usb_rule_added;
	bool flt4_prod_rule_added;
	bool flt6_prod_rule_added;
	bool fnr_counters_allocated;
	bool fnr_flt_commit_pending[IPA_IP_MAX];
	bool fnr_rt_commit_pending[IPA_IP_MAX];
};

#define IPA_TEST_HW_STATS_WIDE_CNT_IDX 200
#define IPA_TEST_HW_STATS_RT_CNT_IDX 201
#define IPA_TEST_HW_STATS_INVALID_CNT_IDX \
	(IPA_MAX_FLT_RT_CNT_INDEX_V7 + 1)
#define IPA_TEST_HW_STATS_RT_TBL_NAME "V4_RT_HW_STATS_WIDE"

/* Use the IPsec decap producer and AP WAN consumer test path. */
#define IPA_TEST_HW_STATS_PROD IPA_CLIENT_IPSEC_DECAP_PROD
#define IPA_TEST_HW_STATS_CONS IPA_CLIENT_IPSEC_APPS_WAN_CONS

struct ipa_test_hw_stats_traffic_ctx {
	struct completion rx_done;
};

static struct ipa_test_hw_stats_ctx *ctx;

static int ipa_test_hw_stats_suite_teardown(void *priv);

static u32 ipa_test_hw_stats_counter_id(
	enum ipa_test_hw_stats_counter counter)
{
	return ctx->hw_counter_start_id + counter;
}

static int ipa_test_hw_stats_init_prod_params(
	struct ipa_sys_connect_params *params, ipa_notify_cb notify)
{
	struct net_device *netdev = NULL;

#if defined(CONFIG_IPA_IPSEC)
	if (ipa3_ctx && ipa3_ctx->ipsec)
		netdev = ipa3_ctx->ipsec->dev;
#endif
	if (!netdev)
		return -ENODEV;

	memset(params, 0, sizeof(*params));
	params->client = IPA_TEST_HW_STATS_PROD;
	params->desc_fifo_sz = IPA_SYS_TX_DATA_DESC_FIFO_SZ;
	params->ipa_ep_cfg.mode.mode = IPA_BASIC;
	params->ipa_ep_cfg.mode.dst = IPA_TEST_HW_STATS_PROD;
	params->ipa_ep_cfg.cfg.cs_offload_en = IPA_DISABLE_CS_OFFLOAD;
	params->ipa_ep_cfg.aggr.aggr_en = IPA_BYPASS_AGGR;
	params->priv = netdev;
	params->notify = notify;
	params->ext_ioctl_v2 = true;
	params->int_modt = 16;
	params->int_modc = 20;

	return 0;
}

static void ipa_test_hw_stats_traffic_notify(void *priv,
	enum ipa_dp_evt_type evt, unsigned long data)
{
	struct sk_buff *skb = (struct sk_buff *)data;

	if (evt == IPA_RECEIVE) {
		struct ipa_test_hw_stats_traffic_ctx *traffic = priv;

		complete(&traffic->rx_done);
		dev_kfree_skb_any(skb);
	} else if (evt == IPA_WRITE_DONE) {
		dev_kfree_skb_any(skb);
	} else {
		IPA_UT_ERR("unexpected traffic event %d\n", evt);
		dev_kfree_skb_any(skb);
	}
}

static struct sk_buff *ipa_test_hw_stats_create_udp_skb(u16 dst_port)
{
	struct sk_buff *skb;
	struct iphdr *ip;
	struct udphdr *udp;
	const unsigned int len = sizeof(*ip) + sizeof(*udp);

	skb = alloc_skb(len + NET_IP_ALIGN, GFP_KERNEL);
	if (!skb)
		return NULL;

	skb_reserve(skb, NET_IP_ALIGN);
	ip = (struct iphdr *)skb_put(skb, len);
	udp = (struct udphdr *)(ip + 1);

	memset(ip, 0, len);
	ip->version = 4;
	ip->ihl = 5;
	ip->tot_len = htons(len);
	ip->ttl = 64;
	ip->protocol = IPPROTO_UDP;
	ip->saddr = htonl(0x0a000001);
	ip->daddr = htonl(0x0a000002);
	ip->check = ip_fast_csum((u8 *)ip, ip->ihl);

	udp->source = htons(5002);
	udp->dest = htons(dst_port);
	udp->len = htons(sizeof(*udp));

	skb->protocol = htons(ETH_P_IP);
	skb->ip_summed = CHECKSUM_NONE;
	return skb;
}

static int ipa_test_hw_stats_suite_setup(void **ppriv)
{
	int ret;

	IPA_UT_DBG("Start Setup\n");

	if (!ctx) {
		ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
		if (!ctx)
			return -ENOMEM;
	} else if (ctx->odu_prod_pipe_setup || ctx->odu_cons_pipe_setup ||
		ctx->fnr_counters_allocated ||
		ctx->rt4_usb_rule_added || ctx->rt6_usb_rule_added ||
		ctx->rt4_cons_rule_added || ctx->rt6_cons_rule_added ||
		ctx->flt4_usb_rule_added || ctx->flt6_usb_rule_added ||
		ctx->flt4_prod_rule_added || ctx->flt6_prod_rule_added ||
		ctx->fnr_flt_commit_pending[IPA_IP_v4] ||
		ctx->fnr_flt_commit_pending[IPA_IP_v6] ||
		ctx->fnr_rt_commit_pending[IPA_IP_v4] ||
		ctx->fnr_rt_commit_pending[IPA_IP_v6]) {
		IPA_UT_INFO("retrying cleanup from the previous run\n");
		ret = ipa_test_hw_stats_suite_teardown(NULL);
		if (ret) {
			IPA_UT_ERR("previous cleanup is still incomplete %d\n",
				ret);
			return ret;
		}
	}

	return 0;
}

static int ipa_test_hw_stats_del_flt_rule(enum ipa_ip_type ip, u32 hdl,
	bool *rule_added, bool *commit_pending)
{
	struct ipa_ioc_del_flt_rule *flt_rule;
	int ret;

	if (!*rule_added)
		return 0;

	flt_rule = kzalloc(sizeof(*flt_rule) +
		sizeof(struct ipa_flt_rule_del), GFP_KERNEL);
	if (!flt_rule)
		return -ENOMEM;

	flt_rule->commit = 0;
	flt_rule->ip = ip;
	flt_rule->num_hdls = 1;
	flt_rule->hdl[0].hdl = hdl;
	flt_rule->hdl[0].status = -1;

	ret = ipa3_del_flt_rule(flt_rule);
	if (!ret && !flt_rule->hdl[0].status) {
		*rule_added = false;
		*commit_pending = true;
	}
	if (ret || flt_rule->hdl[0].status) {
		IPA_UT_ERR("failed to delete filter rule 0x%x\n", hdl);
		if (!ret)
			ret = -EFAULT;
	}

	kfree(flt_rule);
	return ret;
}

static int ipa_test_hw_stats_del_rt_rule(enum ipa_ip_type ip, u32 hdl,
	bool *rule_added, bool *commit_pending)
{
	struct ipa_ioc_del_rt_rule *rt_rule;
	int ret;

	if (!*rule_added)
		return 0;

	rt_rule = kzalloc(sizeof(*rt_rule) +
		sizeof(struct ipa_rt_rule_del), GFP_KERNEL);
	if (!rt_rule)
		return -ENOMEM;

	rt_rule->commit = 0;
	rt_rule->ip = ip;
	rt_rule->num_hdls = 1;
	rt_rule->hdl[0].hdl = hdl;
	rt_rule->hdl[0].status = -1;

	ret = ipa3_del_rt_rule(rt_rule);
	if (!ret && !rt_rule->hdl[0].status) {
		*rule_added = false;
		*commit_pending = true;
	}
	if (ret || rt_rule->hdl[0].status) {
		IPA_UT_ERR("failed to delete routing rule 0x%x\n", hdl);
		if (!ret)
			ret = -EFAULT;
	}

	kfree(rt_rule);
	return ret;
}

static int ipa_test_hw_stats_add_rt_rule(
	struct ipa_ioc_add_rt_rule_v2 *rt_rule, u32 *hdl, bool *rule_added)
{
	struct ipa_rt_rule_add_v2 *rule;
	int ret;

	rule = (struct ipa_rt_rule_add_v2 *)rt_rule->rules;
	rule->status = -1;
	ret = ipa3_add_rt_rule_v2(rt_rule);
	if (!rule->status) {
		*hdl = rule->rt_rule_hdl;
		*rule_added = true;
	}
	if (ret || rule->status) {
		IPA_UT_ERR("failed to install routing rule\n");
		if (!ret)
			ret = -EFAULT;
	}

	return ret;
}

static int ipa_test_hw_stats_add_flt_rule(
	struct ipa_ioc_add_flt_rule_v2 *flt_rule, u32 *hdl, bool *rule_added)
{
	struct ipa_flt_rule_add_v2 *rule;
	int ret;

	rule = (struct ipa_flt_rule_add_v2 *)flt_rule->rules;
	rule->status = -1;
	ret = ipa3_add_flt_rule_v2(flt_rule);
	if (!rule->status) {
		*hdl = rule->flt_rule_hdl;
		*rule_added = true;
	}
	if (ret || rule->status) {
		IPA_UT_ERR("failed to install filter rule\n");
		if (!ret)
			ret = -EFAULT;
	}

	return ret;
}

static int ipa_test_hw_stats_commit_rules(enum ipa_ip_type ip,
	bool *commit_pending, bool routing)
{
	int ret;

	if (!*commit_pending)
		return 0;

	ret = routing ? ipa3_commit_rt(ip) : ipa3_commit_flt(ip);
	if (ret) {
		IPA_UT_ERR("failed to commit FNR %s rules for IP %d %d\n",
			routing ? "routing" : "filter", ip, ret);
		return ret;
	}

	*commit_pending = false;
	return 0;
}

static int ipa_test_hw_stats_cleanup_FnR(void)
{
	int ret = 0;
	int res;

	/*
	 * Commit filter-rule removal before modifying routes. This prevents a
	 * hardware filter from referring to a route table that software releases.
	 * Keep failed commits pending so the next cleanup attempt can retry them.
	 */
	/* Stop USB traffic from reaching the test consumer first. */
	res = ipa_test_hw_stats_del_flt_rule(IPA_IP_v4,
		ctx->flt4_usb_rule_hdl, &ctx->flt4_usb_rule_added,
		&ctx->fnr_flt_commit_pending[IPA_IP_v4]);
	if (res)
		ret = res;
	res = ipa_test_hw_stats_del_flt_rule(IPA_IP_v6,
		ctx->flt6_usb_rule_hdl, &ctx->flt6_usb_rule_added,
		&ctx->fnr_flt_commit_pending[IPA_IP_v6]);
	if (res && !ret)
		ret = res;
	res = ipa_test_hw_stats_del_flt_rule(IPA_IP_v4,
		ctx->flt4_prod_rule_hdl, &ctx->flt4_prod_rule_added,
		&ctx->fnr_flt_commit_pending[IPA_IP_v4]);
	if (res && !ret)
		ret = res;
	res = ipa_test_hw_stats_del_flt_rule(IPA_IP_v6,
		ctx->flt6_prod_rule_hdl, &ctx->flt6_prod_rule_added,
		&ctx->fnr_flt_commit_pending[IPA_IP_v6]);
	if (res && !ret)
		ret = res;

	res = ipa_test_hw_stats_commit_rules(IPA_IP_v4,
		&ctx->fnr_flt_commit_pending[IPA_IP_v4], false);
	if (res && !ret)
		ret = res;
	res = ipa_test_hw_stats_commit_rules(IPA_IP_v6,
		&ctx->fnr_flt_commit_pending[IPA_IP_v6], false);
	if (res && !ret)
		ret = res;

	/* Remove test routes only after no filter can reference them. */
	if (!ctx->flt4_usb_rule_added && !ctx->flt4_prod_rule_added &&
		!ctx->fnr_flt_commit_pending[IPA_IP_v4]) {
		res = ipa_test_hw_stats_del_rt_rule(IPA_IP_v4,
			ctx->rt4_usb_rule_hdl, &ctx->rt4_usb_rule_added,
			&ctx->fnr_rt_commit_pending[IPA_IP_v4]);
		if (res && !ret)
			ret = res;
		res = ipa_test_hw_stats_del_rt_rule(IPA_IP_v4,
			ctx->rt4_cons_rule_hdl, &ctx->rt4_cons_rule_added,
			&ctx->fnr_rt_commit_pending[IPA_IP_v4]);
		if (res && !ret)
			ret = res;
	}

	if (!ctx->flt6_usb_rule_added && !ctx->flt6_prod_rule_added &&
		!ctx->fnr_flt_commit_pending[IPA_IP_v6]) {
		res = ipa_test_hw_stats_del_rt_rule(IPA_IP_v6,
			ctx->rt6_usb_rule_hdl, &ctx->rt6_usb_rule_added,
			&ctx->fnr_rt_commit_pending[IPA_IP_v6]);
		if (res && !ret)
			ret = res;
		res = ipa_test_hw_stats_del_rt_rule(IPA_IP_v6,
			ctx->rt6_cons_rule_hdl, &ctx->rt6_cons_rule_added,
			&ctx->fnr_rt_commit_pending[IPA_IP_v6]);
		if (res && !ret)
			ret = res;
	}

	res = ipa_test_hw_stats_commit_rules(IPA_IP_v4,
		&ctx->fnr_rt_commit_pending[IPA_IP_v4], true);
	if (res && !ret)
		ret = res;
	res = ipa_test_hw_stats_commit_rules(IPA_IP_v6,
		&ctx->fnr_rt_commit_pending[IPA_IP_v6], true);
	if (res && !ret)
		ret = res;

	if (ctx->fnr_counters_allocated &&
		!ctx->rt4_usb_rule_added && !ctx->rt6_usb_rule_added &&
		!ctx->rt4_cons_rule_added && !ctx->rt6_cons_rule_added &&
		!ctx->flt4_usb_rule_added && !ctx->flt6_usb_rule_added &&
		!ctx->flt4_prod_rule_added && !ctx->flt6_prod_rule_added &&
		!ctx->fnr_flt_commit_pending[IPA_IP_v4] &&
		!ctx->fnr_flt_commit_pending[IPA_IP_v6] &&
		!ctx->fnr_rt_commit_pending[IPA_IP_v4] &&
		!ctx->fnr_rt_commit_pending[IPA_IP_v6]) {
		res = ipa3_counter_remove_hdl(ctx->fnr_counter_hdl);
		if (res) {
			IPA_UT_ERR("failed to release FNR counters %d\n", res);
			if (!ret)
				ret = res;
		} else {
			ctx->fnr_counters_allocated = false;
		}
	}

	return ret;
}

static int ipa_test_hw_stats_suite_teardown(void *priv)
{
	int ret = 0;
	int res;

	IPA_UT_DBG("Start Teardown\n");

	res = ipa_test_hw_stats_cleanup_FnR();
	if (res) {
		IPA_UT_ERR("fail to clean up FNR rules %d\n", res);
		return res;
	}

	if (ctx->odu_cons_pipe_setup) {
		res = ipa_teardown_sys_pipe(ctx->odu_cons_hdl);
		if (res) {
			IPA_UT_ERR("fail to teardown sys pipe %s %d\n",
				ipa_clients_strings[ctx->cons_client], res);
			ret = res;
		} else {
			ctx->odu_cons_pipe_setup = false;
		}
	}

	if (ctx->odu_prod_pipe_setup) {
		res = ipa_teardown_sys_pipe(ctx->odu_prod_hdl);
		if (res) {
			IPA_UT_ERR("fail to teardown sys pipe %s %d\n",
				ipa_clients_strings[ctx->prod_client], res);
			ret = res;
		} else {
			ctx->odu_prod_pipe_setup = false;
		}
	}

	return ret;
}

static void odu_prod_notify(void *priv, enum ipa_dp_evt_type evt,
	unsigned long data)
{
	struct sk_buff *skb = (struct sk_buff *)data;

	switch (evt) {
	case IPA_RECEIVE:
		dev_kfree_skb_any(skb);
		break;
	case IPA_WRITE_DONE:
		atomic_dec(&ctx->odu_pending);
		dev_kfree_skb_any(skb);
		break;
	default:
		IPA_UT_ERR("unexpected evt %d\n", evt);
	}
}
static void odu_cons_notify(void *priv, enum ipa_dp_evt_type evt,
	unsigned long data)
{
	struct sk_buff *skb = (struct sk_buff *)data;
	int ret;

	switch (evt) {
	case IPA_RECEIVE:
		if (atomic_read(&ctx->odu_pending) >= 64)
			msleep(20);
		atomic_inc(&ctx->odu_pending);
		skb_put(skb, 100);
		ret = ipa_tx_dp(ctx->prod_client, skb, NULL);
		while (ret) {
			msleep(100);
			ret = ipa_tx_dp(ctx->prod_client, skb, NULL);
		}
		break;
	case IPA_WRITE_DONE:
		dev_kfree_skb_any(skb);
		break;
	default:
		IPA_UT_ERR("unexpected evt %d\n", evt);
	}
}

static int ipa_test_hw_stats_configure(void *priv)
{
	struct ipa_sys_connect_params odu_prod_params;
	struct ipa_sys_connect_params odu_emb_cons_params;
	int res;

	/* Retain the ODU endpoints before IPA v7.0 for compatibility. */
	if (ipa3_ctx->ipa_hw_type >= IPA_HW_v7_0) {
		ctx->prod_client = IPA_CLIENT_TEST_PROD;
		ctx->cons_client = IPA_CLIENT_IPSEC_APPS_WAN_CONS;
	} else {
		ctx->prod_client = IPA_CLIENT_ODU_PROD;
		ctx->cons_client = IPA_CLIENT_ODU_EMB_CONS;
	}

	if (ctx->cons_client == IPA_CLIENT_IPSEC_APPS_WAN_CONS &&
		ipa_ipsec_enabled()) {
		IPA_UT_ERR("IPSEC active: IPSEC_APPS_WAN_CONS pipe is in use; cannot run\n");
		return -EBUSY;
	}

	/* first connect all additional pipe */
	memset(&odu_prod_params, 0, sizeof(odu_prod_params));
	memset(&odu_emb_cons_params, 0, sizeof(odu_emb_cons_params));

	odu_prod_params.client = ctx->prod_client;
	odu_prod_params.desc_fifo_sz = 0x1000;
	odu_prod_params.priv = NULL;
	odu_prod_params.notify = odu_prod_notify;
	res = ipa_setup_sys_pipe(&odu_prod_params,
		&ctx->odu_prod_hdl);
	if (res) {
		IPA_UT_ERR("fail to setup sys pipe %s %d\n",
			ipa_clients_strings[ctx->prod_client], res);
		return res;
	}
	ctx->odu_prod_pipe_setup = true;

	odu_emb_cons_params.client = ctx->cons_client;
	odu_emb_cons_params.desc_fifo_sz = 0x1000;
	odu_emb_cons_params.priv = NULL;
	odu_emb_cons_params.notify = odu_cons_notify;
	if (ctx->cons_client == IPA_CLIENT_IPSEC_APPS_WAN_CONS) {
		/*
		 * Match the EP config that ipa_ipsec_ep_init_cons() programs in
		 * production so ipa3_wan_rx_pyld_status_hdlr parses correctly.
		 */
		ipa3_configure_ipsec_apps_wan_cons(&odu_emb_cons_params);
	}
	res = ipa_setup_sys_pipe(&odu_emb_cons_params,
		&ctx->odu_cons_hdl);
	if (res) {
		IPA_UT_ERR("fail to setup sys pipe %s %d\n",
			ipa_clients_strings[ctx->cons_client], res);
		ipa_test_hw_stats_suite_teardown(priv);
		return res;
	}
	ctx->odu_cons_pipe_setup = true;

	IPA_UT_INFO("Configured. Please connect USB RNDIS now\n");

	return 0;
}

static int ipa_test_hw_stats_add_FnR(void *priv)
{
	struct ipa_ioc_add_rt_rule_v2 *rt_rule;
	struct ipa_ioc_add_flt_rule_v2 *flt_rule;
	struct ipa_ioc_get_rt_tbl rt_lookup;
	struct ipa_ioc_flt_rt_counter_alloc *counter = NULL;
	struct ipa_ioc_flt_rt_query *query;
	u32 rt4_usb;
	u32 rt6_usb;
	u32 rt4_cons;
	u32 rt6_cons;
	int pyld_size = 0;
	int ret;

	rt_rule = kzalloc(sizeof(*rt_rule), GFP_KERNEL);
	if (!rt_rule) {
		IPA_UT_DBG("no mem\n");
		return -ENOMEM;
	}
	rt_rule->rules = (uint64_t)kzalloc(1 *
		sizeof(struct ipa_rt_rule_add_v2), GFP_KERNEL);
	if (!rt_rule->rules) {
		IPA_UT_DBG("no mem\n");
		ret = -ENOMEM;
		goto free_rt;
	}
	rt_rule->rule_add_size = sizeof(struct ipa_rt_rule_add_v2);

	flt_rule = kzalloc(sizeof(*flt_rule), GFP_KERNEL);
	if (!flt_rule) {
		IPA_UT_DBG("no mem\n");
		ret = -ENOMEM;
		goto free_rt;
	}
	flt_rule->rules = (uint64_t)kzalloc(1 *
		sizeof(struct ipa_flt_rule_add_v2), GFP_KERNEL);
	if (!flt_rule->rules) {
		ret = -ENOMEM;
		goto free_flt;
	}
	flt_rule->flt_rule_size = sizeof(struct ipa_flt_rule_add_v2);

	counter = kzalloc(sizeof(struct ipa_ioc_flt_rt_counter_alloc),
					  GFP_KERNEL);
	if (!counter) {
		ret = -ENOMEM;
		goto free_flt;
	}
	counter->hw_counter.num_counters = IPA_TEST_HW_STATS_COUNTER_MAX;
	counter->sw_counter.num_counters = 1;

	/* allocate counters */
	ret = ipa3_alloc_counter_id(counter);
	if (ret < 0) {
		IPA_UT_DBG("ipa3_alloc_counter_id fails\n");
		ret = -ENOMEM;
		goto free_counter;
	}
	ctx->hw_counter_start_id = counter->hw_counter.start_id;
	ctx->fnr_counter_hdl = counter->hdl;
	ctx->fnr_counters_allocated = true;

	/* initially clean all allocated counters */
	query = kzalloc(sizeof(struct ipa_ioc_flt_rt_query),
		GFP_KERNEL);
	if (!query) {
		ret = -ENOMEM;
		ipa_test_hw_stats_cleanup_FnR();
		goto free_counter;
	}
	query->start_id = counter->hw_counter.start_id;
	query->end_id = counter->hw_counter.start_id +
		counter->hw_counter.num_counters - 1;
	query->reset = true;
	query->stats_size = sizeof(struct ipa_flt_rt_stats);
	pyld_size = IPA_MAX_FLT_RT_CNT_INDEX *
		sizeof(struct ipa_flt_rt_stats);
	query->stats = (uint64_t)kzalloc(pyld_size, GFP_KERNEL);
	if (!query->stats) {
		ret = -ENOMEM;
		goto free_query;
	}
	ipa_get_flt_rt_stats(query);

	query->start_id = counter->sw_counter.start_id;
	query->end_id = counter->sw_counter.start_id +
		counter->sw_counter.num_counters - 1;
	query->reset = true;
	query->stats_size = sizeof(struct ipa_flt_rt_stats);
	ipa_get_flt_rt_stats(query);

	rt_rule->commit = 1;
	rt_rule->ip = IPA_IP_v4;
	rt_lookup.ip = rt_rule->ip;
	strscpy(rt_rule->rt_tbl_name, "V4_RT_TO_USB_CONS",
		IPA_RESOURCE_NAME_MAX);
	strscpy(rt_lookup.name, rt_rule->rt_tbl_name, IPA_RESOURCE_NAME_MAX);
	rt_rule->num_rules = 1;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.dst = IPA_CLIENT_USB_CONS;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.hashable = true;
	IPA_UT_INFO("rt4_usb_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_USB));
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_USB);
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.enable_stats = true;
	ret = ipa_test_hw_stats_add_rt_rule(rt_rule,
		&ctx->rt4_usb_rule_hdl, &ctx->rt4_usb_rule_added);
	if (ret)
		goto free_query;
	if (ipa3_get_rt_tbl(&rt_lookup)) {
		IPA_UT_ERR("failed to query V4 rules\n");
		ret = -EFAULT;
		goto free_query;
	}
	rt4_usb = rt_lookup.hdl;

	rt_rule->commit = 1;
	rt_rule->ip = IPA_IP_v6;
	rt_lookup.ip = rt_rule->ip;
	strscpy(rt_rule->rt_tbl_name, "V6_RT_TO_USB_CONS",
		IPA_RESOURCE_NAME_MAX);
	strscpy(rt_lookup.name, rt_rule->rt_tbl_name, IPA_RESOURCE_NAME_MAX);
	rt_rule->num_rules = 1;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.dst = IPA_CLIENT_USB_CONS;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.hashable = true;
	IPA_UT_INFO("rt6_usb_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT6_USB));
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT6_USB);
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.enable_stats = true;
	ret = ipa_test_hw_stats_add_rt_rule(rt_rule,
		&ctx->rt6_usb_rule_hdl, &ctx->rt6_usb_rule_added);
	if (ret)
		goto free_query;
	if (ipa3_get_rt_tbl(&rt_lookup)) {
		IPA_UT_ERR("failed to query V4 rules\n");
		ret = -EFAULT;
		goto free_query;
	}
	rt6_usb = rt_lookup.hdl;

	rt_rule->commit = 1;
	rt_rule->ip = IPA_IP_v4;
	rt_lookup.ip = rt_rule->ip;
	if (ctx->cons_client == IPA_CLIENT_IPSEC_APPS_WAN_CONS)
		strscpy(rt_rule->rt_tbl_name, "V4_RT_TO_IPSEC_WAN",
			IPA_RESOURCE_NAME_MAX);
	else
		strscpy(rt_rule->rt_tbl_name, "V4_RT_TO_ODU_CONS",
			IPA_RESOURCE_NAME_MAX);
	strscpy(rt_lookup.name, rt_rule->rt_tbl_name, IPA_RESOURCE_NAME_MAX);
	rt_rule->num_rules = 1;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.dst = ctx->cons_client;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.hashable = true;
	IPA_UT_INFO("rt4_odu_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_CONS));
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_CONS);
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.enable_stats = true;
	ret = ipa_test_hw_stats_add_rt_rule(rt_rule,
		&ctx->rt4_cons_rule_hdl, &ctx->rt4_cons_rule_added);
	if (ret)
		goto free_query;
	if (ipa3_get_rt_tbl(&rt_lookup)) {
		IPA_UT_ERR("failed to query V4 rules\n");
		ret = -EFAULT;
		goto free_query;
	}
	rt4_cons = rt_lookup.hdl;

	rt_rule->commit = 1;
	rt_rule->ip = IPA_IP_v6;
	rt_lookup.ip = rt_rule->ip;
	if (ctx->cons_client == IPA_CLIENT_IPSEC_APPS_WAN_CONS)
		strscpy(rt_rule->rt_tbl_name, "V6_RT_TO_IPSEC_WAN",
			IPA_RESOURCE_NAME_MAX);
	else
		strscpy(rt_rule->rt_tbl_name, "V6_RT_TO_ODU_CONS",
			IPA_RESOURCE_NAME_MAX);
	strscpy(rt_lookup.name, rt_rule->rt_tbl_name, IPA_RESOURCE_NAME_MAX);
	rt_rule->num_rules = 1;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.dst = ctx->cons_client;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.hashable = true;
	IPA_UT_INFO("rt6_odu_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT6_CONS));
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT6_CONS);
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.enable_stats = true;
	ret = ipa_test_hw_stats_add_rt_rule(rt_rule,
		&ctx->rt6_cons_rule_hdl, &ctx->rt6_cons_rule_added);
	if (ret)
		goto free_query;
	if (ipa3_get_rt_tbl(&rt_lookup)) {
		IPA_UT_ERR("failed to query V4 rules\n");
		ret = -EFAULT;
		goto free_query;
	}
	rt6_cons = rt_lookup.hdl;

	flt_rule->commit = 1;
	flt_rule->ip = IPA_IP_v4;
	flt_rule->ep = IPA_CLIENT_USB_PROD;
	flt_rule->num_rules = 1;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].at_rear = 0;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.action = IPA_PASS_TO_ROUTING;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rt_tbl_hdl = rt4_cons;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.hashable = 1;
	IPA_UT_INFO("flt4_usb_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT4_USB));
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT4_USB);
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.enable_stats = true;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
	ret = ipa_test_hw_stats_add_flt_rule(flt_rule,
		&ctx->flt4_usb_rule_hdl, &ctx->flt4_usb_rule_added);
	if (ret)
		goto free_query;

	flt_rule->commit = 1;
	flt_rule->ip = IPA_IP_v6;
	flt_rule->ep = IPA_CLIENT_USB_PROD;
	flt_rule->num_rules = 1;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].at_rear = 0;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.action = IPA_PASS_TO_ROUTING;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rt_tbl_hdl = rt6_cons;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.hashable = 1;
	IPA_UT_INFO("flt6_usb_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_USB));
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_USB);
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.enable_stats = true;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
	ret = ipa_test_hw_stats_add_flt_rule(flt_rule,
		&ctx->flt6_usb_rule_hdl, &ctx->flt6_usb_rule_added);
	if (ret)
		goto free_query;

	flt_rule->commit = 1;
	flt_rule->ip = IPA_IP_v4;
	flt_rule->ep = ctx->prod_client;
	flt_rule->num_rules = 1;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].at_rear = 0;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.action = IPA_PASS_TO_ROUTING;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rt_tbl_hdl = rt4_usb;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.hashable = 1;
	IPA_UT_INFO("flt4_odu_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT4_PROD));
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT4_PROD);
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.enable_stats = true;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
	ret = ipa_test_hw_stats_add_flt_rule(flt_rule,
		&ctx->flt4_prod_rule_hdl, &ctx->flt4_prod_rule_added);
	if (ret)
		goto free_query;

	flt_rule->commit = 1;
	flt_rule->ip = IPA_IP_v6;
	flt_rule->ep = ctx->prod_client;
	flt_rule->num_rules = 1;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].at_rear = 0;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.action = IPA_PASS_TO_ROUTING;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rt_tbl_hdl = rt6_usb;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.hashable = 1;
	IPA_UT_INFO("flt6_odu_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_PROD));
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_PROD);
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.enable_stats = true;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
	ret = ipa_test_hw_stats_add_flt_rule(flt_rule,
		&ctx->flt6_prod_rule_hdl, &ctx->flt6_prod_rule_added);
	if (ret)
		goto free_query;
	IPA_UT_INFO(
		"Rules added. Please start data transfer on ports 5001/5002\n");
	ret = 0;

free_query:
	if (ret)
		ipa_test_hw_stats_cleanup_FnR();
	kfree((void *)(query->stats));
	kfree(query);
free_counter:
	kfree(counter);
free_flt:
	kfree((void *)(flt_rule->rules));
	kfree(flt_rule);
free_rt:
	kfree((void *)(rt_rule->rules));
	kfree(rt_rule);
	return ret;
}

static int ipa_test_hw_stats_query_FnR_one_by_one(void *priv)
{
	int ret;
	struct ipa_ioc_flt_rt_query *query;
	int pyld_size = 0;

	query = kzalloc(sizeof(struct ipa_ioc_flt_rt_query), GFP_KERNEL);
	if (!query)
		return -ENOMEM;
	pyld_size = IPA_MAX_FLT_RT_CNT_INDEX *
		sizeof(struct ipa_flt_rt_stats);
	query->stats = (uint64_t)kzalloc(pyld_size, GFP_KERNEL);
	if (!query->stats) {
		kfree(query);
		return -ENOMEM;
	}
	/* query 1 by 1 */
	IPA_UT_INFO("========query 1 by 1========\n");
	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT4_USB);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT4_USB);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"usb v4 route counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_USB),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT6_USB);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT6_USB);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"usb v6 route counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT6_USB),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT4_CONS);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT4_CONS);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"odu v4 route counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_CONS),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT6_CONS);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT6_CONS);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"odu v6 route counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT6_CONS),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT4_USB);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT4_USB);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"usb v4 filter counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT4_USB),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT6_USB);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT6_USB);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"usb v6 filter counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_USB),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT4_PROD);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT4_PROD);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"odu v4 filter counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT4_PROD),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT6_PROD);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT6_PROD);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"odu v6 filter counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_PROD),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);
	IPA_UT_INFO("================ done ============\n");

	ret = 0;
	kfree(query);
	return ret;
}

static int ipa_test_hw_stats_query_FnR_one_shot(void *priv)
{
	int ret, i, start = 0;
	struct ipa_ioc_flt_rt_query *query;
	int pyld_size = 0;

	query = kzalloc(sizeof(struct ipa_ioc_flt_rt_query), GFP_KERNEL);
	if (!query)
		return -ENOMEM;
	pyld_size = IPA_MAX_FLT_RT_CNT_INDEX *
		sizeof(struct ipa_flt_rt_stats);
	query->stats = (uint64_t)kzalloc(pyld_size, GFP_KERNEL);
	if (!query->stats) {
		kfree(query);
		return -ENOMEM;
	}

	/* query all together */
	IPA_UT_INFO("========query all together========\n");
	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT4_USB);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT6_PROD);
	ipa_get_flt_rt_stats(query);
	start = 0;
	for (i = ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_USB);
		i <= ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_PROD);
		i++) {
		IPA_UT_INFO(
			"counter %u pkt_cnt %u bytes cnt %llu\n",
			i, ((struct ipa_flt_rt_stats *)
			query->stats)[start].num_pkts,
			((struct ipa_flt_rt_stats *)
			query->stats)[start].num_bytes);
		start++;
	}
	IPA_UT_INFO("================ done ============\n");

	ret = 0;
	kfree((void *)(query->stats));
	kfree(query);
	return ret;
}

static int ipa_test_hw_stats_query_FnR_clean(void *priv)
{
	int ret, i, start = 0;
	struct ipa_ioc_flt_rt_query *query;
	int pyld_size = 0;

	query = kzalloc(sizeof(struct ipa_ioc_flt_rt_query), GFP_KERNEL);
	if (!query) {
		IPA_UT_DBG("no mem\n");
		return -ENOMEM;
	}
	pyld_size = IPA_MAX_FLT_RT_CNT_INDEX *
		sizeof(struct ipa_flt_rt_stats);
	query->stats = (uint64_t)kzalloc(pyld_size, GFP_KERNEL);
	if (!query->stats) {
		kfree(query);
		return -ENOMEM;
	}

	/* query and reset */
	IPA_UT_INFO("========query and reset========\n");
	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT4_USB);
	query->reset = true;
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT6_PROD);
	start = 0;
	ipa_get_flt_rt_stats(query);
	for (i = ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_USB);
		i <= ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_PROD);
		i++) {
		IPA_UT_INFO(
			"counter %u pkt_cnt %u bytes cnt %llu\n",
			i, ((struct ipa_flt_rt_stats *)
			query->stats)[start].num_pkts,
			((struct ipa_flt_rt_stats *)
			query->stats)[start].num_bytes);
		start++;
	}
	IPA_UT_INFO("================ done ============\n");

	ret = 0;
	kfree((void *)(query->stats));
	kfree(query);
	return ret;
}


static int ipa_test_hw_stats_query_sw_stats(void *priv)
{
	int ret, i, start = 0;
	struct ipa_ioc_flt_rt_query *query;
	int pyld_size = 0;

	query = kzalloc(sizeof(struct ipa_ioc_flt_rt_query), GFP_KERNEL);
	if (!query)
		return -ENOMEM;
	pyld_size = IPA_MAX_FLT_RT_CNT_INDEX *
		sizeof(struct ipa_flt_rt_stats);
	query->stats = (uint64_t)kzalloc(pyld_size, GFP_KERNEL);
	if (!query->stats) {
		kfree(query);
		return -ENOMEM;
	}

	/* query all together */
	IPA_UT_INFO("========query all SW counters========\n");
	query->start_id = IPA_FLT_RT_HW_COUNTER + 1;
	query->end_id = IPA_MAX_FLT_RT_CNT_INDEX;
	ipa_get_flt_rt_stats(query);
	start = 0;
	for (i = IPA_FLT_RT_HW_COUNTER + 1;
		i <= IPA_MAX_FLT_RT_CNT_INDEX; i++) {
		IPA_UT_INFO(
			"counter %u pkt_cnt %u bytes cnt %llu\n",
			i, ((struct ipa_flt_rt_stats *)
			query->stats)[start].num_pkts,
			((struct ipa_flt_rt_stats *)
			query->stats)[start].num_bytes);
		start++;
	}
	IPA_UT_INFO("================ done ============\n");

	ret = 0;
	kfree((void *)(query->stats));
	kfree(query);
	return ret;
}

static int ipa_test_hw_stats_set_sw_stats(void *priv)
{
	int i, start = 0;
	struct ipa_flt_rt_stats stats;

	/* set sw counters */
	IPA_UT_INFO("========set all SW counters========\n");
	for (i = IPA_FLT_RT_HW_COUNTER + 1;
		i <= IPA_MAX_FLT_RT_CNT_INDEX; i++) {
		stats.num_bytes = start;
		stats.num_pkts_hash = start + 10;
		stats.num_pkts = start + 100;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			i, stats.num_pkts, stats.num_bytes);
		ipa_set_flt_rt_stats(i, stats);
		start++;
	}
	IPA_UT_INFO("================ done ============\n");

	return 0;
}

static int ipa_test_hw_stats_query_drop_stats(void *priv)
{
       int ret, i, ep_idx, reg_idx;
       struct ipa_drop_stats_all *query;

       query = kzalloc(sizeof(struct ipa_drop_stats_all), GFP_KERNEL);
       if (!query)
               return -ENOMEM;

       IPA_UT_INFO("========query all drop stats========\n");

       ret = ipa_get_drop_stats(query);
       if (ret)
               goto fail;

       for (i = 0; i <= IPA_CLIENT_MAX; i++) {
               ep_idx = ipa_get_ep_mapping(i);
               if (ep_idx == -1 || !IPA_CLIENT_IS_CONS(i) || IPA_CLIENT_IS_TEST(i))
                       continue;

               reg_idx = ipahal_get_ep_reg_idx(ep_idx);
               if (!(ipa3_ctx->hw_stats->drop.init.enabled_bitmask[reg_idx] &
                       ipahal_get_ep_bit(ep_idx)))
                       continue;

               IPA_UT_INFO("Client %u pkt_cnt %u bytes cnt %u\n", i,
                       query->client[i].drop_packet_cnt, query->client[i].drop_byte_cnt);
       }

       IPA_UT_INFO("================ done ============\n");

fail:
       kfree(query);
       return ret;
}

static int ipa_test_hw_stats_reset_all_drop_stats(void *priv)
{
       int ret;

       IPA_UT_INFO("========reset all drop stats========\n");

       ret = ipa_reset_all_drop_stats();
       if (ret)
               IPA_UT_ERR("ipa_reset_all_drop_stats failed %d\n", ret);

       IPA_UT_INFO("================ done ============\n");

       return ret;
}

static int ipa_test_hw_stats_verify_max_counters(void *priv)
{
	u32 v4;
	u32 v6;
	u32 nonip;
	u32 drop_v4;
	u32 drop_v6;
	u32 drop_nonip;
	u32 drop_natct;
	u32 v4_filter, v4_router;
	u32 v6_filter, v6_router;
	u32 nonip_filter, nonip_router;
	u32 exp_nonip_filter, exp_nonip_router;
	u32 natct_v4, natct_v6;

	if (!ipa3_ctx->hw_stats || !ipa3_ctx->hw_stats->enabled) {
		IPA_UT_INFO("HW stats not enabled, skipping\n");
		return 0;
	}

	IPA_UT_INFO("========verify max counter registers========\n");

	v4 = ipahal_read_reg(IPA_STAT_IPV4_FILTER_ROUTER_MAX_COUNTERS);
	v6 = ipahal_read_reg(IPA_STAT_IPV6_FILTER_ROUTER_MAX_COUNTERS);
	nonip = ipahal_read_reg(IPA_STAT_NONIP_FILTER_ROUTER_MAX_COUNTERS);
	drop_v4 = ipahal_read_reg(IPA_DROP_STAT_IPV4_FILTER_ROUTER_MAX_COUNTERS);
	drop_v6 = ipahal_read_reg(IPA_DROP_STAT_IPV6_FILTER_ROUTER_MAX_COUNTERS);
	drop_nonip = ipahal_read_reg(IPA_DROP_STAT_NONIP_FILTER_ROUTER_MAX_COUNTERS);
	drop_natct = ipahal_read_reg(IPA_DROP_STAT_NAT_AND_CONN_TRACK_MAX_COUNTERS);

	IPA_UT_INFO("v4=0x%x v6=0x%x nonip=0x%x\n", v4, v6, nonip);
	IPA_UT_INFO("drop_v4=0x%x drop_v6=0x%x drop_nonip=0x%x drop_natct=0x%x\n",
		drop_v4, drop_v6, drop_nonip, drop_natct);

	if (!v4 || !v6 || !nonip || !drop_natct) {
		IPA_UT_ERR(
			"unprogrammed max counter reg: v4=0x%x v6=0x%x nonip=0x%x natct=0x%x\n",
			v4, v6, nonip, drop_natct);
		IPA_UT_TEST_FAIL_REPORT("max counter register left at 0");
		return -EFAULT;
	}

	v4_filter = (v4 & IPAHAL_STAT_FNR_FILTER_MAX_COUNTERS_BMSK) >>
		IPAHAL_STAT_FNR_FILTER_MAX_COUNTERS_SHFT;
	v4_router = (v4 & IPAHAL_STAT_FNR_ROUTER_MAX_COUNTERS_BMSK) >>
		IPAHAL_STAT_FNR_ROUTER_MAX_COUNTERS_SHFT;
	if (v4_filter != v4_router) {
		IPA_UT_ERR("v4 filter/router halves mismatch: 0x%x vs 0x%x\n",
			v4_filter, v4_router);
		IPA_UT_TEST_FAIL_REPORT("v4 filter/router halves mismatch");
		return -EFAULT;
	}

	v6_filter = (v6 & IPAHAL_STAT_FNR_FILTER_MAX_COUNTERS_BMSK) >>
		IPAHAL_STAT_FNR_FILTER_MAX_COUNTERS_SHFT;
	v6_router = (v6 & IPAHAL_STAT_FNR_ROUTER_MAX_COUNTERS_BMSK) >>
		IPAHAL_STAT_FNR_ROUTER_MAX_COUNTERS_SHFT;
	if (v6_filter != v6_router) {
		IPA_UT_ERR("v6 filter/router halves mismatch: 0x%x vs 0x%x\n",
			v6_filter, v6_router);
		IPA_UT_TEST_FAIL_REPORT("v6 filter/router halves mismatch");
		return -EFAULT;
	}

	/*
	 * Unlike v4/v6, the non-IP filter and router counters live in separate
	 * SRAM partitions, so the two halves are NOT required to match each
	 * other - each must match the size of its own partition.
	 */
	nonip_filter = (nonip & IPAHAL_STAT_FNR_FILTER_MAX_COUNTERS_BMSK) >>
		IPAHAL_STAT_FNR_FILTER_MAX_COUNTERS_SHFT;
	nonip_router = (nonip & IPAHAL_STAT_FNR_ROUTER_MAX_COUNTERS_BMSK) >>
		IPAHAL_STAT_FNR_ROUTER_MAX_COUNTERS_SHFT;
	exp_nonip_filter = min_t(u32,
		IPA_MEM_PART(stats_flt_nonip_size) /
			ipahal_stats_get_hw_entry_size(IPAHAL_HW_STATS_FNR),
		IPAHAL_STAT_MAX_COUNTERS_LIMIT);
	exp_nonip_router = min_t(u32,
		IPA_MEM_PART(stats_rt_nonip_size) /
			ipahal_stats_get_hw_entry_size(IPAHAL_HW_STATS_FNR),
		IPAHAL_STAT_MAX_COUNTERS_LIMIT);
	if (nonip_filter != exp_nonip_filter) {
		IPA_UT_ERR("nonip filter half 0x%x, expected 0x%x\n",
			nonip_filter, exp_nonip_filter);
		IPA_UT_TEST_FAIL_REPORT("nonip filter max counters mismatch");
		return -EFAULT;
	}
	if (nonip_router != exp_nonip_router) {
		IPA_UT_ERR("nonip router half 0x%x, expected 0x%x\n",
			nonip_router, exp_nonip_router);
		IPA_UT_TEST_FAIL_REPORT("nonip router max counters mismatch");
		return -EFAULT;
	}

	if (v4 != v6) {
		IPA_UT_ERR("v4/v6 FnR max counters mismatch: 0x%x vs 0x%x\n", v4, v6);
		IPA_UT_TEST_FAIL_REPORT("v4/v6 FnR max counters mismatch");
		return -EFAULT;
	}

	if (drop_v4 != v4) {
		IPA_UT_ERR("drop_v4 does not mirror v4: 0x%x vs 0x%x\n", drop_v4, v4);
		IPA_UT_TEST_FAIL_REPORT("drop_v4 does not mirror v4");
		return -EFAULT;
	}

	if (drop_v6 != v6) {
		IPA_UT_ERR("drop_v6 does not mirror v6: 0x%x vs 0x%x\n", drop_v6, v6);
		IPA_UT_TEST_FAIL_REPORT("drop_v6 does not mirror v6");
		return -EFAULT;
	}

	if (drop_nonip != nonip) {
		IPA_UT_ERR("drop_nonip does not mirror nonip: 0x%x vs 0x%x\n",
			drop_nonip, nonip);
		IPA_UT_TEST_FAIL_REPORT("drop_nonip does not mirror nonip");
		return -EFAULT;
	}

	natct_v4 = (drop_natct & IPAHAL_STAT_IPV4_MAX_COUNTERS_BMSK) >>
		IPAHAL_STAT_IPV4_MAX_COUNTERS_SHFT;
	natct_v6 = (drop_natct & IPAHAL_STAT_IPV6_MAX_COUNTERS_BMSK) >>
		IPAHAL_STAT_IPV6_MAX_COUNTERS_SHFT;
	if (natct_v4 != natct_v6) {
		IPA_UT_ERR("NAT/CT max counters halves mismatch: 0x%x vs 0x%x\n",
			natct_v4, natct_v6);
		IPA_UT_TEST_FAIL_REPORT("NAT/CT max counters halves mismatch");
		return -EFAULT;
	}

	IPA_UT_INFO("================ done (PASS) ============\n");

	return 0;
}

static int ipa_test_hw_stats_verify_wide_counter_index(void *priv)
{
	struct ipa_ioc_add_rt_rule_v2 *rt_rule = NULL;
	struct ipa_ioc_add_flt_rule_v2 *flt_rule = NULL;
	struct ipa_rt_rule_add_v2 *rt_rule_entry;
	struct ipa_flt_rule_add_v2 *flt_rule_entry;
	struct ipa_ioc_del_rt_rule *rt_del = NULL;
	struct ipa_ioc_del_flt_rule *flt_del = NULL;
	struct ipa_ioc_get_rt_tbl rt_lookup;
	struct ipa_ioc_flt_rt_query *query = NULL;
	struct ipa_flt_rt_stats *stats;
	struct ipa_sys_connect_params test_prod_params;
	struct ipa_sys_connect_params test_cons_params;
	struct ipa_test_hw_stats_traffic_ctx traffic_ctx;
	struct sk_buff *skb = NULL;
	int add_ret;
	int cleanup_ret;
	int i;
	int ret = 0;
	u32 rt_tbl_hdl = 0;
	u32 rt_rule_hdl = 0;
	u32 rt_invalid_rule_hdl = 0;
	u32 flt_rule_hdl = 0;
	u32 flt_invalid_rule_hdl = 0;
	u32 test_prod_hdl = 0;
	u32 test_cons_hdl = 0;
	bool rt_tbl_ref = false;
	bool rt_rule_added = false;
	bool rt_invalid_rule_added = false;
	bool flt_rule_added = false;
	bool flt_invalid_rule_added = false;
	bool test_prod_setup = false;
	bool test_cons_setup = false;

	if (!ipa3_ctx->hw_stats || !ipa3_ctx->hw_stats->enabled) {
		IPA_UT_INFO("HW stats not enabled, skipping\n");
		return 0;
	}
	if (ipa_ipsec_enabled()) {
		IPA_UT_ERR("IPSEC active; IPSEC_APPS_WAN_CONS is already in use\n");
		return -EBUSY;
	}
	IPA_UT_INFO("========verify widened FnR counter index========\n");

	rt_rule = kzalloc(sizeof(*rt_rule), GFP_KERNEL);
	flt_rule = kzalloc(sizeof(*flt_rule), GFP_KERNEL);
	rt_del = kzalloc(sizeof(*rt_del) +
		2 * sizeof(struct ipa_rt_rule_del), GFP_KERNEL);
	flt_del = kzalloc(sizeof(*flt_del) +
		2 * sizeof(struct ipa_flt_rule_del), GFP_KERNEL);
	if (!rt_rule || !flt_rule || !rt_del || !flt_del) {
		ret = -ENOMEM;
		goto out;
	}

	rt_rule->rules = (uint64_t)kzalloc(
		sizeof(struct ipa_rt_rule_add_v2), GFP_KERNEL);
	flt_rule->rules = (uint64_t)kzalloc(
		sizeof(struct ipa_flt_rule_add_v2), GFP_KERNEL);
	if (!rt_rule->rules || !flt_rule->rules) {
		ret = -ENOMEM;
		goto out;
	}

	rt_rule_entry = (struct ipa_rt_rule_add_v2 *)
		(uintptr_t)rt_rule->rules;
	flt_rule_entry = (struct ipa_flt_rule_add_v2 *)
		(uintptr_t)flt_rule->rules;

	memset(&rt_lookup, 0, sizeof(rt_lookup));
	rt_lookup.ip = IPA_IP_v4;
	strscpy(rt_lookup.name, IPA_TEST_HW_STATS_RT_TBL_NAME,
		IPA_RESOURCE_NAME_MAX);
	if (!ipa3_get_rt_tbl(&rt_lookup)) {
		cleanup_ret = ipa_put_rt_tbl(rt_lookup.hdl);
		if (cleanup_ret)
			IPA_UT_ERR("failed to release existing test RT table: %d\n",
				cleanup_ret);
		IPA_UT_ERR("test RT table already exists\n");
		ret = -EEXIST;
		goto out;
	}

	rt_rule->commit = 1;
	rt_rule->ip = IPA_IP_v4;
	rt_rule->num_rules = 1;
	rt_rule->rule_add_size = sizeof(*rt_rule_entry);
	strscpy(rt_rule->rt_tbl_name, IPA_TEST_HW_STATS_RT_TBL_NAME,
		IPA_RESOURCE_NAME_MAX);
	rt_rule_entry->rule.dst = IPA_TEST_HW_STATS_CONS;
	rt_rule_entry->rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	rt_rule_entry->rule.attrib.dst_port = 5003;
	rt_rule_entry->rule.hashable = true;
	rt_rule_entry->rule.enable_stats = true;
	rt_rule_entry->rule.cnt_idx = IPA_TEST_HW_STATS_RT_CNT_IDX;
	rt_rule_entry->rule.cnt_idx2 = 0;

	add_ret = ipa3_add_rt_rule_v2(rt_rule);
	if (add_ret || rt_rule_entry->status) {
		IPA_UT_ERR("failed to install widened-index RT rule: %d/%d\n",
			add_ret, rt_rule_entry->status);
		if (!rt_rule_entry->status) {
			rt_rule_added = true;
			rt_rule_hdl = rt_rule_entry->rt_rule_hdl;
		}
		ret = add_ret ? add_ret : -EFAULT;
		goto out;
	}
	rt_rule_added = true;
	rt_rule_hdl = rt_rule_entry->rt_rule_hdl;

	/* cnt_idx2 is the high byte and 256 is outside the supported ABI range. */
	memset(rt_rule_entry, 0, sizeof(*rt_rule_entry));
	rt_rule_entry->rule.dst = IPA_TEST_HW_STATS_CONS;
	rt_rule_entry->rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	rt_rule_entry->rule.attrib.dst_port = 5004;
	rt_rule_entry->rule.hashable = true;
	rt_rule_entry->rule.enable_stats = true;
	rt_rule_entry->rule.cnt_idx = 0;
	rt_rule_entry->rule.cnt_idx2 = 1;

	add_ret = ipa3_add_rt_rule_v2(rt_rule);
	if (add_ret) {
		IPA_UT_ERR("failed to test invalid widened-index RT rule: %d/%d\n",
			add_ret, rt_rule_entry->status);
		ret = add_ret;
		goto out;
	}
	if (!rt_rule_entry->status) {
		IPA_UT_ERR("invalid widened-index RT rule was accepted: %d/%d\n",
			add_ret, rt_rule_entry->status);
		rt_invalid_rule_hdl = rt_rule_entry->rt_rule_hdl;
		rt_invalid_rule_added = true;
		ret = add_ret ? add_ret : -EFAULT;
		goto out;
	}

	memset(&rt_lookup, 0, sizeof(rt_lookup));
	rt_lookup.ip = IPA_IP_v4;
	strscpy(rt_lookup.name, IPA_TEST_HW_STATS_RT_TBL_NAME,
		IPA_RESOURCE_NAME_MAX);
	ret = ipa3_get_rt_tbl(&rt_lookup);
	if (ret) {
		IPA_UT_ERR("failed to query widened-index RT table: %d\n", ret);
		goto out;
	}
	rt_tbl_hdl = rt_lookup.hdl;
	rt_tbl_ref = true;

	memset(&traffic_ctx, 0, sizeof(traffic_ctx));
	init_completion(&traffic_ctx.rx_done);

	ret = ipa_test_hw_stats_init_prod_params(&test_prod_params,
		ipa_test_hw_stats_traffic_notify);
	if (ret) {
		IPA_UT_ERR("IPsec netdevice is unavailable: %d\n", ret);
		goto out;
	}
	ret = ipa_setup_sys_pipe(&test_prod_params, &test_prod_hdl);
	if (ret) {
		IPA_UT_ERR("failed to setup test producer pipe: %d\n", ret);
		goto out;
	}
	test_prod_setup = true;

	memset(&test_cons_params, 0, sizeof(test_cons_params));
	test_cons_params.client = IPA_TEST_HW_STATS_CONS;
	test_cons_params.desc_fifo_sz = 0x1000;
	test_cons_params.priv = &traffic_ctx;
	test_cons_params.notify = ipa_test_hw_stats_traffic_notify;
	test_cons_params.bypass_agg = true;
	test_cons_params.ipa_ep_cfg.hdr.hdr_len = 8;
	test_cons_params.ipa_ep_cfg.hdr.hdr_ofst_metadata_valid = 1;
	test_cons_params.ipa_ep_cfg.hdr.hdr_ofst_metadata = 1;
	test_cons_params.ipa_ep_cfg.hdr.hdr_ofst_pkt_size_valid = 1;
	test_cons_params.ipa_ep_cfg.hdr.hdr_ofst_pkt_size = 2;
	test_cons_params.ipa_ep_cfg.hdr_ext.hdr_total_len_or_pad_valid = true;
	test_cons_params.ipa_ep_cfg.hdr_ext.hdr_payload_len_inc_padding = true;
	test_cons_params.ipa_ep_cfg.metadata_mask.metadata_mask = 0xFF000000;
	ret = ipa_setup_sys_pipe(&test_cons_params, &test_cons_hdl);
	if (ret) {
		IPA_UT_ERR("failed to setup test consumer pipe: %d\n", ret);
		goto out;
	}
	test_cons_setup = true;

	flt_rule->commit = 1;
	flt_rule->ip = IPA_IP_v4;
	flt_rule->ep = IPA_TEST_HW_STATS_PROD;
	flt_rule->num_rules = 1;
	flt_rule->flt_rule_size = sizeof(*flt_rule_entry);
	flt_rule_entry->rule.action = IPA_PASS_TO_ROUTING;
	flt_rule_entry->rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	flt_rule_entry->rule.attrib.dst_port = 5003;
	flt_rule_entry->rule.rt_tbl_hdl = rt_tbl_hdl;
	flt_rule_entry->rule.hashable = true;
	flt_rule_entry->rule.enable_stats = true;
	flt_rule_entry->rule.cnt_idx = 0;
	flt_rule_entry->rule.cnt_idx_v2 = IPA_TEST_HW_STATS_WIDE_CNT_IDX;
	flt_rule_entry->rule.rule_type = IPA_FLT_RULE_TYPE_MAX;

	add_ret = ipa3_add_flt_rule_v2(flt_rule);
	if (add_ret || flt_rule_entry->status) {
		IPA_UT_ERR("failed to install widened-index FLT rule: %d/%d\n",
			add_ret, flt_rule_entry->status);
		if (!flt_rule_entry->status) {
			flt_rule_added = true;
			flt_rule_hdl = flt_rule_entry->flt_rule_hdl;
		}
		ret = add_ret ? add_ret : -EFAULT;
		goto out;
	}
	flt_rule_added = true;
	flt_rule_hdl = flt_rule_entry->flt_rule_hdl;

	query = kzalloc(sizeof(*query), GFP_KERNEL);
	if (!query) {
		ret = -ENOMEM;
		goto out;
	}
	query->stats = (uint64_t)kzalloc(
		(IPA_TEST_HW_STATS_RT_CNT_IDX -
		 IPA_TEST_HW_STATS_WIDE_CNT_IDX + 1) *
		sizeof(struct ipa_flt_rt_stats),
		GFP_KERNEL);
	if (!query->stats) {
		ret = -ENOMEM;
		goto out;
	}
	query->start_id = IPA_TEST_HW_STATS_WIDE_CNT_IDX;
	query->end_id = IPA_TEST_HW_STATS_RT_CNT_IDX;
	query->stats_size = sizeof(struct ipa_flt_rt_stats);
	query->reset = true;
	ret = ipa_get_flt_rt_stats(query);
	if (ret) {
		IPA_UT_ERR("failed to reset widened counters %u-%u: %d\n",
			IPA_TEST_HW_STATS_WIDE_CNT_IDX,
			IPA_TEST_HW_STATS_RT_CNT_IDX, ret);
		goto out;
	}

	skb = ipa_test_hw_stats_create_udp_skb(5003);
	if (!skb) {
		ret = -ENOMEM;
		goto out;
	}
	ret = ipa_tx_dp(IPA_TEST_HW_STATS_PROD, skb, NULL);
	if (ret) {
		IPA_UT_ERR("failed to transmit widened-index test packet: %d\n",
			ret);
		kfree_skb(skb);
		skb = NULL;
		goto out;
	}
	skb = NULL;
	if (!wait_for_completion_timeout(&traffic_ctx.rx_done,
		msecs_to_jiffies(1000))) {
		IPA_UT_ERR("widened-index test packet was not received\n");
		ret = -ETIMEDOUT;
		goto out;
	}
	msleep(20);

	query->reset = false;
	ret = ipa_get_flt_rt_stats(query);
	if (ret) {
		IPA_UT_ERR("failed to query widened counters %u-%u after traffic: %d\n",
			IPA_TEST_HW_STATS_WIDE_CNT_IDX,
			IPA_TEST_HW_STATS_RT_CNT_IDX, ret);
		goto out;
	}
	stats = (struct ipa_flt_rt_stats *)(uintptr_t)query->stats;
	IPA_UT_INFO("filter counter %u after traffic: packets=%u bytes=%llu\n",
		IPA_TEST_HW_STATS_WIDE_CNT_IDX, stats[0].num_pkts,
		stats[0].num_bytes);
	if (!stats[0].num_pkts || !stats[0].num_bytes) {
		IPA_UT_ERR("widened counter %u did not record traffic\n",
			IPA_TEST_HW_STATS_WIDE_CNT_IDX);
		ret = -EFAULT;
		goto out;
	}
	IPA_UT_INFO("route counter %u after traffic: packets=%u bytes=%llu\n",
		IPA_TEST_HW_STATS_RT_CNT_IDX, stats[1].num_pkts,
		stats[1].num_bytes);
	if (!stats[1].num_pkts || !stats[1].num_bytes) {
		IPA_UT_ERR("route counter %u did not record traffic\n",
			IPA_TEST_HW_STATS_RT_CNT_IDX);
		ret = -EFAULT;
		goto out;
	}

	/* cnt_idx_v2 must be honored; 256 must not fall back to cnt_idx == 0. */
	memset(flt_rule_entry, 0, sizeof(*flt_rule_entry));
	flt_rule_entry->at_rear = 1;
	flt_rule_entry->rule.action = IPA_PASS_TO_ROUTING;
	flt_rule_entry->rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	flt_rule_entry->rule.attrib.dst_port = 5004;
	flt_rule_entry->rule.rt_tbl_hdl = rt_tbl_hdl;
	flt_rule_entry->rule.hashable = true;
	flt_rule_entry->rule.enable_stats = true;
	flt_rule_entry->rule.cnt_idx = 0;
	flt_rule_entry->rule.cnt_idx_v2 = IPA_TEST_HW_STATS_INVALID_CNT_IDX;
	flt_rule_entry->rule.rule_type = IPA_FLT_RULE_TYPE_MAX;

	add_ret = ipa3_add_flt_rule_v2(flt_rule);
	if (add_ret) {
		IPA_UT_ERR("failed to test invalid widened-index FLT rule: %d/%d\n",
			add_ret, flt_rule_entry->status);
		ret = add_ret;
		goto out;
	}
	if (!flt_rule_entry->status) {
		IPA_UT_ERR("invalid widened-index FLT rule was accepted: %d/%d\n",
			add_ret, flt_rule_entry->status);
		flt_invalid_rule_hdl = flt_rule_entry->flt_rule_hdl;
		flt_invalid_rule_added = true;
		ret = add_ret ? add_ret : -EFAULT;
		goto out;
	}

	IPA_UT_INFO("================ done (PASS) ============\n");

out:
	if (skb)
		kfree_skb(skb);

	if (test_cons_setup) {
		cleanup_ret = ipa_teardown_sys_pipe(test_cons_hdl);
		if (cleanup_ret && !ret)
			ret = cleanup_ret;
	}
	if (test_prod_setup) {
		cleanup_ret = ipa_teardown_sys_pipe(test_prod_hdl);
		if (cleanup_ret && !ret)
			ret = cleanup_ret;
	}

	if (query) {
		kfree((void *)(uintptr_t)query->stats);
		kfree(query);
	}

	if (flt_rule_added || flt_invalid_rule_added) {
		memset(flt_del, 0, sizeof(*flt_del) +
			2 * sizeof(struct ipa_flt_rule_del));
		flt_del->commit = 1;
		flt_del->ip = IPA_IP_v4;
		flt_del->num_hdls = flt_rule_added + flt_invalid_rule_added;
		if (flt_rule_added)
			flt_del->hdl[0].hdl = flt_rule_hdl;
		if (flt_invalid_rule_added)
			flt_del->hdl[flt_rule_added].hdl = flt_invalid_rule_hdl;
		cleanup_ret = ipa3_del_flt_rule(flt_del);
		if (cleanup_ret && !ret)
			ret = cleanup_ret;
		for (i = 0; i < flt_del->num_hdls; i++) {
			if (flt_del->hdl[i].status) {
				IPA_UT_ERR("failed to delete FLT rule %u: %d\n",
					flt_del->hdl[i].hdl, flt_del->hdl[i].status);
				if (!ret)
					ret = -EFAULT;
			}
		}
	}

	if (rt_tbl_ref) {
		cleanup_ret = ipa_put_rt_tbl(rt_tbl_hdl);
		if (cleanup_ret && !ret)
			ret = cleanup_ret;
	}

	if (rt_rule_added || rt_invalid_rule_added) {
		memset(rt_del, 0, sizeof(*rt_del) +
			2 * sizeof(struct ipa_rt_rule_del));
		rt_del->commit = 1;
		rt_del->ip = IPA_IP_v4;
		rt_del->num_hdls = rt_rule_added + rt_invalid_rule_added;
		if (rt_rule_added)
			rt_del->hdl[0].hdl = rt_rule_hdl;
		if (rt_invalid_rule_added)
			rt_del->hdl[rt_rule_added].hdl = rt_invalid_rule_hdl;
		cleanup_ret = ipa3_del_rt_rule(rt_del);
		if (cleanup_ret && !ret)
			ret = cleanup_ret;
		for (i = 0; i < rt_del->num_hdls; i++) {
			if (rt_del->hdl[i].status) {
				IPA_UT_ERR("failed to delete RT rule %u: %d\n",
					rt_del->hdl[i].hdl, rt_del->hdl[i].status);
				if (!ret)
					ret = -EFAULT;
			}
		}
	}

	kfree((void *)(uintptr_t)(flt_rule ? flt_rule->rules : 0));
	kfree(flt_rule);
	kfree((void *)(uintptr_t)(rt_rule ? rt_rule->rules : 0));
	kfree(rt_rule);
	kfree(flt_del);
	kfree(rt_del);
	return ret;
}

static int ipa_test_hw_stats_query_teth_stats(void *priv)
{
       int i, j, prod_reg, cons_reg;
       int res;
       struct ipa_quota_stats_all *stats;

       stats = kzalloc(sizeof(*stats), GFP_KERNEL);
       if (!stats)
               return -ENOMEM;

       IPA_UT_INFO("========get all tethering stats========\n");
       res = ipa_get_teth_stats();
       if (res) {
               IPA_UT_ERR("ipa_get_teth_stats failed with code %u\n", res);
               goto teardown;
       }

       for (i = 0; i < IPA_CLIENT_MAX; i++) {
               int ep_idx = ipa_get_ep_mapping(i);

               if (ep_idx == IPA_EP_NOT_ALLOCATED)
                       continue;

               if (!IPA_CLIENT_IS_PROD(i))
                       continue;

               if (IPA_CLIENT_IS_TEST(i))
                       continue;

               prod_reg = ipahal_get_ep_reg_idx(ep_idx);
               if (!(ipa3_ctx->hw_stats->teth.init.prod_bitmask[prod_reg] &
                       ipahal_get_ep_bit(ep_idx)))
                       continue;

               res = ipa_query_teth_stats(i, stats, false);
               if (res) {
                       IPA_UT_ERR("ipa_query_teth_stats failed with code %u\n", res);
                       goto teardown;
               }

               for (j = 0; j < IPA_CLIENT_MAX; j++) {
                       int cons_idx = ipa_get_ep_mapping(j);

                       if (cons_idx == IPA_EP_NOT_ALLOCATED)
                               continue;

                       if (IPA_CLIENT_IS_TEST(j))
                               continue;

                       cons_reg = ipahal_get_ep_reg_idx(j);
                       if (!(ipa3_ctx->hw_stats->teth.init.
                               cons_bitmask[ep_idx][cons_reg]
                               & ipahal_get_ep_bit(cons_idx)))
                               continue;

                       IPA_UT_INFO("%s->%s:\n", ipa_clients_strings[i],
                               ipa_clients_strings[j]);
		       if(j < ipa3_get_max_num_pipes()){
                       	IPA_UT_INFO("num_ipv4_bytes=%llu\n",
                                               stats->client[j].num_ipv4_bytes);
                       	IPA_UT_INFO("num_ipv6_bytes=%llu\n",
                                               stats->client[j].num_ipv6_bytes);
                       	IPA_UT_INFO("num_ipv4_pkts=%u\n",
                                               stats->client[j].num_ipv4_pkts);
                       	IPA_UT_INFO("num_ipv6_pkts=%u\n",
                                               stats->client[j].num_ipv6_pkts);
			       /* Non-IP counters are non-zero only when
				* the pair's Non-IP bit is set.
				*/
			       if ((ipa3_ctx->hw_stats->teth.init.nonip_bitmask[
					prod_reg] & ipahal_get_ep_bit(ep_idx)) ||
					(ipa3_ctx->hw_stats->teth.init.nonip_bitmask[
					cons_reg] & ipahal_get_ep_bit(cons_idx))) {
				       IPA_UT_INFO("num_nonip_bytes=%llu\n",
					       stats->client[j].num_nonip_bytes);
				       IPA_UT_INFO("num_nonip_pkts=%u\n",
					       stats->client[j].num_nonip_pkts);
			       }
		       }
               }
       }

       IPA_UT_INFO("================ done ============\n");

teardown:
       kfree(stats);
       return res;
}

/*
 * TETH_ASSERT() - assert helper for the IPA 7.0 tethering tests.
 * On failure, reports the UT failure and jumps to "restore" for cleanup.
 */
#define TETH_ASSERT(cond, msg) \
	do { \
		if (!(cond)) { \
			IPA_UT_ERR("assert failed: %s\n", msg); \
			IPA_UT_TEST_FAIL_REPORT(msg); \
			rc = -EINVAL; \
			goto restore; \
		} \
	} while (0)

/*
 * ipa_test_hw_stats_teth_v7_roundtrip() - HW-independent HAL round-trip test
 * for the IPA 7.0 tethering mixed-stride stats logic.
 *
 * Unlike ipa_test_hw_stats_query_teth_stats() (print-only, requires v7.0
 * silicon), this drives ipahal_stats_generate_init_pyld(),
 * ipahal_stats_get_offset(), ipahal_stats_generate_teth_cfg_pyld() and
 * ipahal_parse_stats() directly against a fabricated 3-pair fixture and
 * asserts exact offsets/sizes/CFG bytes/parsed values, with no HW dependency.
 * The test is restricted to IPA 7.0 hardware because the public wrappers
 * dispatch on the global runtime ipahal_ctx->hw_type.
 *
 * Fixture:
 *   Producers: P0 = 1, P1 = 40
 *   Consumers: C0 = 2, C1 = 3
 *   Non-IP set on C1 only -> pair (P0,C1) is the sole 40B pair.
 *   ETH set on P0 only -> (P0,C0) and (P0,C1) are ETH, (P1,C0) is not.
 *   Pair order (producer asc, then consumer asc):
 *     #1 P0->C0 : 24B, ETH
 *     #2 P0->C1 : 40B (Non-IP), ETH
 *     #3 P1->C0 : 24B, not ETH
 */
static int ipa_test_hw_stats_teth_v7_roundtrip(void *priv)
{
	int rc = 0;
	struct ipahal_stats_init_tethering init;
	struct ipahal_stats_init_pyld *pyld = NULL;
	struct ipahal_stats_init_pyld *cfg = NULL;
	struct ipahal_stats_get_offset_tethering goff;
	struct ipahal_stats_offset off = {0};
	struct ipahal_stats_tethering_hdr_v7_0_hw *hdr;
	u8 *raw = NULL;
	struct ipahal_stats_tethering_all *out = NULL;
	struct ipahal_stats_tethering_hw *raw_hw;
	struct ipahal_stats_tethering_v7_0_hw *raw_v7_hw;
	u32 counters_bytes = 24 + 40 + 24; /* pair #1 + #2 + #3, see above */
	const int P0 = 1, P1 = 40, C0 = 2, C1 = 3;

	memset(&init, 0, sizeof(init));

	init.prod_bitmask[ipahal_get_ep_reg_idx(P0)] |=
		ipahal_get_ep_bit(P0);
	init.prod_bitmask[ipahal_get_ep_reg_idx(P1)] |=
		ipahal_get_ep_bit(P1);

	init.cons_bitmask[P0][ipahal_get_ep_reg_idx(C0)] |=
		ipahal_get_ep_bit(C0);
	init.cons_bitmask[P0][ipahal_get_ep_reg_idx(C1)] |=
		ipahal_get_ep_bit(C1);
	init.cons_bitmask[P1][ipahal_get_ep_reg_idx(C0)] |=
		ipahal_get_ep_bit(C0);

	init.nonip_bitmask[ipahal_get_ep_reg_idx(C1)] |=
		ipahal_get_ep_bit(C1);
	init.eth_bitmask[ipahal_get_ep_reg_idx(P0)] |=
		ipahal_get_ep_bit(P0);

	IPA_UT_INFO("========teth v7.0 HAL mixed-stride round-trip========\n");

	/* --- init payload: header count + mixed-stride counter bytes --- */
	pyld = ipahal_stats_generate_init_pyld(IPAHAL_HW_STATS_TETHERING,
		&init, false);
	TETH_ASSERT(pyld != NULL, "generate_init_pyld returned NULL");
	TETH_ASSERT(pyld->len == 136, "pyld->len != 136");

	hdr = (struct ipahal_stats_tethering_hdr_v7_0_hw *)pyld->data;
	/*
	 * hdr->offset (8-byte units) is relative to the whole payload
	 * (headers + counters): it starts at hdr_entries*sizeof(hdr)/8 = 6
	 * before any counter bytes are added. P0's pairs are 64B, so P1's
	 * offset is 6 + 64/8 = 14.
	 */
	TETH_ASSERT(hdr[0].offset == 6, "P0 header offset != 6");
	TETH_ASSERT(hdr[0].cfg_start_index == 1,
		"P0 header cfg_start_index != 1");
	TETH_ASSERT(hdr[1].offset == 14, "P1 header offset != 14");
	TETH_ASSERT(hdr[1].cfg_start_index == 3,
		"P1 header cfg_start_index != 3");

	ipahal_destroy_stats_init_pyld(pyld);
	pyld = NULL;

	/* --- get_offset: counter-region size must match mixed-stride sum --- */
	memset(&goff, 0, sizeof(goff));
	goff.init = init;
	rc = ipahal_stats_get_offset(IPAHAL_HW_STATS_TETHERING, &goff, &off);
	TETH_ASSERT(rc == 0, "ipahal_stats_get_offset failed");
	TETH_ASSERT(off.size == counters_bytes, "get_offset out.size != 88");

	/* --- CFG payload: 2 bits/counter, 4 counters/byte, 1-based index --- */
	cfg = ipahal_stats_generate_teth_cfg_pyld(&init, false);
	TETH_ASSERT(cfg != NULL, "generate_teth_cfg_pyld returned NULL");
	TETH_ASSERT(cfg->len == 1, "cfg->len != 1");
	TETH_ASSERT(cfg->data[0] == 0x0D, "cfg->data[0] != 0x0D");

	ipahal_destroy_stats_init_pyld(cfg);
	cfg = NULL;

	/* --- round-trip: build raw counter buffer, parse, check matrix --- */
	raw = kzalloc(counters_bytes, GFP_KERNEL);
	TETH_ASSERT(raw != NULL, "failed to alloc raw counter buffer");

	/* pair #1: P0->C0, 24B, not Non-IP */
	raw_hw = (struct ipahal_stats_tethering_hw *)(raw + 0);
	raw_hw->num_ipv4_bytes = 0x1111;
	raw_hw->num_ipv4_pkts = 0x11;
	raw_hw->num_ipv6_pkts = 0x12;
	raw_hw->num_ipv6_bytes = 0x2222;

	/* pair #2: P0->C1, 40B, Non-IP */
	raw_v7_hw = (struct ipahal_stats_tethering_v7_0_hw *)(raw + 24);
	raw_v7_hw->num_ipv4_bytes = 0x3333;
	raw_v7_hw->num_ipv4_pkts = 0x33;
	raw_v7_hw->num_ipv6_pkts = 0x34;
	raw_v7_hw->num_ipv6_bytes = 0x4444;
	raw_v7_hw->num_nonip_pkts = 0x35;
	raw_v7_hw->num_nonip_bytes = 0x5555;

	/* pair #3: P1->C0, 24B, not Non-IP */
	raw_hw = (struct ipahal_stats_tethering_hw *)(raw + 64);
	raw_hw->num_ipv4_bytes = 0x6666;
	raw_hw->num_ipv4_pkts = 0x66;
	raw_hw->num_ipv6_pkts = 0x67;
	raw_hw->num_ipv6_bytes = 0x7777;

	/*
	 * ~270KB output matrix - never on the stack, see
	 * struct ipahal_stats_tethering_all.
	 */
	out = kvzalloc(sizeof(*out), GFP_KERNEL);
	TETH_ASSERT(out != NULL, "failed to alloc parsed stats matrix");

	rc = ipahal_parse_stats(IPAHAL_HW_STATS_TETHERING, &init, raw, out);
	TETH_ASSERT(rc == 0, "ipahal_parse_stats failed");

	TETH_ASSERT(out->stats[P0][C0].num_ipv4_bytes == 0x1111,
		"[P0][C0] num_ipv4_bytes mismatch");
	TETH_ASSERT(out->stats[P0][C0].num_ipv4_pkts == 0x11,
		"[P0][C0] num_ipv4_pkts mismatch");
	TETH_ASSERT(out->stats[P0][C0].num_ipv6_pkts == 0x12,
		"[P0][C0] num_ipv6_pkts mismatch");
	TETH_ASSERT(out->stats[P0][C0].num_ipv6_bytes == 0x2222,
		"[P0][C0] num_ipv6_bytes mismatch");
	TETH_ASSERT(out->stats[P0][C0].num_nonip_pkts == 0,
		"[P0][C0] num_nonip_pkts should be 0 for 24B pair");
	TETH_ASSERT(out->stats[P0][C0].num_nonip_bytes == 0,
		"[P0][C0] num_nonip_bytes should be 0 for 24B pair");

	TETH_ASSERT(out->stats[P0][C1].num_ipv4_bytes == 0x3333,
		"[P0][C1] num_ipv4_bytes mismatch");
	TETH_ASSERT(out->stats[P0][C1].num_ipv4_pkts == 0x33,
		"[P0][C1] num_ipv4_pkts mismatch");
	TETH_ASSERT(out->stats[P0][C1].num_ipv6_pkts == 0x34,
		"[P0][C1] num_ipv6_pkts mismatch");
	TETH_ASSERT(out->stats[P0][C1].num_ipv6_bytes == 0x4444,
		"[P0][C1] num_ipv6_bytes mismatch");
	TETH_ASSERT(out->stats[P0][C1].num_nonip_pkts == 0x35,
		"[P0][C1] num_nonip_pkts mismatch");
	TETH_ASSERT(out->stats[P0][C1].num_nonip_bytes == 0x5555,
		"[P0][C1] num_nonip_bytes mismatch");

	TETH_ASSERT(out->stats[P1][C0].num_ipv4_bytes == 0x6666,
		"[P1][C0] num_ipv4_bytes mismatch");
	TETH_ASSERT(out->stats[P1][C0].num_ipv4_pkts == 0x66,
		"[P1][C0] num_ipv4_pkts mismatch");
	TETH_ASSERT(out->stats[P1][C0].num_ipv6_pkts == 0x67,
		"[P1][C0] num_ipv6_pkts mismatch");
	TETH_ASSERT(out->stats[P1][C0].num_ipv6_bytes == 0x7777,
		"[P1][C0] num_ipv6_bytes mismatch");
	TETH_ASSERT(out->stats[P1][C0].num_nonip_pkts == 0,
		"[P1][C0] num_nonip_pkts should be 0 for 24B pair");
	TETH_ASSERT(out->stats[P1][C0].num_nonip_bytes == 0,
		"[P1][C0] num_nonip_bytes should be 0 for 24B pair");

	/* spot-check untouched entries stay zero */
	TETH_ASSERT(out->stats[P0][4].num_ipv4_bytes == 0,
		"[P0][4] should be untouched/zero");
	TETH_ASSERT(out->stats[2][C0].num_ipv4_bytes == 0,
		"[2][C0] should be untouched/zero");

	IPA_UT_INFO("================ done ============\n");

restore:
	if (pyld)
		ipahal_destroy_stats_init_pyld(pyld);
	if (cfg)
		ipahal_destroy_stats_init_pyld(cfg);
	kfree(raw);
	kvfree(out);
	return rc;
}

struct ipa_test_teth_v7_pair {
	int prod;
	int cons;
	bool nonip;
	u64 num_ipv4_bytes;
	u64 num_ipv4_pkts;
	u64 num_ipv6_pkts;
	u64 num_ipv6_bytes;
	u64 num_nonip_pkts;
	u64 num_nonip_bytes;
};

/*
 * ipa_test_hw_stats_teth_v7_hal_boundary() - exercise the mixed-stride
 * boundary and the two-byte tethering CFG layout on IPA 7.0 hardware.
 *
 * This deliberately uses valid IPA7 producer/consumer ranges: P0/P1 are
 * producer pipes (< 32), while C0..C3 are consumer pipes (>= 32). P0 owns
 * four pairs and P1 owns two, so P1 starts at CFG index five and the fixture
 * crosses the four-counters-per-CFG-byte boundary. It also represents all
 * four two-bit CFG states: ETH only, both, Non-IP only, and neither.
 */
static int ipa_test_hw_stats_teth_v7_hal_boundary(void *priv)
{
	static const struct ipa_test_teth_v7_pair pairs[] = {
		{ 1, 32, false, 0x1001, 0x11, 0x12, 0x2001, 0, 0 },
		{ 1, 33, true,  0x1002, 0x21, 0x22, 0x2002, 0x23, 0x3002 },
		{ 1, 34, true,  0x1003, 0x31, 0x32, 0x2003, 0x33, 0x3003 },
		{ 1, 35, false, 0x1004, 0x41, 0x42, 0x2004, 0, 0 },
		{ 13, 34, true, 0x1005, 0x51, 0x52, 0x2005, 0x53, 0x3005 },
		{ 13, 35, false, 0x1006, 0x61, 0x62, 0x2006, 0, 0 },
	};
	int rc = 0;
	struct ipahal_stats_init_tethering init;
	struct ipahal_stats_init_pyld *pyld = NULL;
	struct ipahal_stats_init_pyld *cfg = NULL;
	struct ipahal_stats_get_offset_tethering goff;
	struct ipahal_stats_offset off = { 0 };
	struct ipahal_stats_tethering_hdr_v7_0_hw *hdr;
	struct ipahal_stats_tethering_all *out = NULL;
	u8 *raw = NULL;
	u32 raw_offset = 0;
	int i;

	memset(&init, 0, sizeof(init));

	/* P0 has four pairs; P1 has two pairs. */
	init.prod_bitmask[ipahal_get_ep_reg_idx(1)] |= ipahal_get_ep_bit(1);
	init.prod_bitmask[ipahal_get_ep_reg_idx(13)] |= ipahal_get_ep_bit(13);
	init.cons_bitmask[1][ipahal_get_ep_reg_idx(32)] |=
		ipahal_get_ep_bit(32);
	init.cons_bitmask[1][ipahal_get_ep_reg_idx(33)] |=
		ipahal_get_ep_bit(33);
	init.cons_bitmask[1][ipahal_get_ep_reg_idx(34)] |=
		ipahal_get_ep_bit(34);
	init.cons_bitmask[1][ipahal_get_ep_reg_idx(35)] |=
		ipahal_get_ep_bit(35);
	init.cons_bitmask[13][ipahal_get_ep_reg_idx(34)] |=
		ipahal_get_ep_bit(34);
	init.cons_bitmask[13][ipahal_get_ep_reg_idx(35)] |=
		ipahal_get_ep_bit(35);

	/* P0 adds ETH; C1/C2 add Non-IP. */
	init.eth_bitmask[ipahal_get_ep_reg_idx(1)] |= ipahal_get_ep_bit(1);
	init.nonip_bitmask[ipahal_get_ep_reg_idx(33)] |=
		ipahal_get_ep_bit(33);
	init.nonip_bitmask[ipahal_get_ep_reg_idx(34)] |=
		ipahal_get_ep_bit(34);

	IPA_UT_INFO("========teth v7.0 HAL boundary coverage========\n");

	pyld = ipahal_stats_generate_init_pyld(IPAHAL_HW_STATS_TETHERING,
		&init, false);
	TETH_ASSERT(pyld != NULL, "boundary generate_init_pyld returned NULL");
	TETH_ASSERT(pyld->len == 240, "boundary pyld->len != 240");

	hdr = (struct ipahal_stats_tethering_hdr_v7_0_hw *)pyld->data;
	TETH_ASSERT(hdr[0].dst_mask_31_0 == 0x0f,
		"P0 destination mask mismatch");
	TETH_ASSERT(hdr[1].dst_mask_31_0 == 0x0c,
		"P1 destination mask mismatch");
	TETH_ASSERT(hdr[0].offset == 6, "P0 header offset != 6");
	TETH_ASSERT(hdr[1].offset == 22, "P1 header offset != 22");
	TETH_ASSERT(hdr[0].cfg_start_index == 1,
		"P0 cfg_start_index != 1");
	TETH_ASSERT(hdr[1].cfg_start_index == 5,
		"P1 cfg_start_index != 5");

	ipahal_destroy_stats_init_pyld(pyld);
	pyld = NULL;

	memset(&goff, 0, sizeof(goff));
	goff.init = init;
	rc = ipahal_stats_get_offset(IPAHAL_HW_STATS_TETHERING, &goff, &off);
	TETH_ASSERT(rc == 0, "boundary get_offset failed");
	TETH_ASSERT(off.offset == 48, "boundary get_offset out.offset != 48");
	TETH_ASSERT(off.size == 192, "boundary get_offset out.size != 192");

	cfg = ipahal_stats_generate_teth_cfg_pyld(&init, false);
	TETH_ASSERT(cfg != NULL, "boundary generate_teth_cfg_pyld returned NULL");
	TETH_ASSERT(cfg->len == 2, "boundary cfg->len != 2");
	TETH_ASSERT(cfg->data[0] == 0x7d, "boundary cfg byte 0 != 0x7d");
	TETH_ASSERT(cfg->data[1] == 0x02, "boundary cfg byte 1 != 0x02");

	ipahal_destroy_stats_init_pyld(cfg);
	cfg = NULL;

	raw = kzalloc(192, GFP_KERNEL);
	TETH_ASSERT(raw != NULL, "failed to alloc boundary raw buffer");

	for (i = 0; i < ARRAY_SIZE(pairs); i++) {
		if (pairs[i].nonip) {
			struct ipahal_stats_tethering_v7_0_hw *raw_v7_hw =
				(struct ipahal_stats_tethering_v7_0_hw *)(raw + raw_offset);

			raw_v7_hw->num_ipv4_bytes = pairs[i].num_ipv4_bytes;
			raw_v7_hw->num_ipv4_pkts = pairs[i].num_ipv4_pkts;
			raw_v7_hw->num_ipv6_pkts = pairs[i].num_ipv6_pkts;
			raw_v7_hw->num_ipv6_bytes = pairs[i].num_ipv6_bytes;
			raw_v7_hw->num_nonip_pkts = pairs[i].num_nonip_pkts;
			raw_v7_hw->num_nonip_bytes = pairs[i].num_nonip_bytes;
			raw_offset += sizeof(*raw_v7_hw);
		} else {
			struct ipahal_stats_tethering_hw *raw_hw =
				(struct ipahal_stats_tethering_hw *)(raw + raw_offset);

			raw_hw->num_ipv4_bytes = pairs[i].num_ipv4_bytes;
			raw_hw->num_ipv4_pkts = pairs[i].num_ipv4_pkts;
			raw_hw->num_ipv6_pkts = pairs[i].num_ipv6_pkts;
			raw_hw->num_ipv6_bytes = pairs[i].num_ipv6_bytes;
			raw_offset += sizeof(*raw_hw);
		}
	}
	TETH_ASSERT(raw_offset == 192, "boundary raw layout size mismatch");

	out = kvzalloc(sizeof(*out), GFP_KERNEL);
	TETH_ASSERT(out != NULL, "failed to alloc boundary parsed matrix");
	rc = ipahal_parse_stats(IPAHAL_HW_STATS_TETHERING, &init, raw, out);
	TETH_ASSERT(rc == 0, "boundary parse_stats failed");

	for (i = 0; i < ARRAY_SIZE(pairs); i++) {
		struct ipahal_stats_tethering *stats =
			&out->stats[pairs[i].prod][pairs[i].cons];

		TETH_ASSERT(stats->num_ipv4_bytes == pairs[i].num_ipv4_bytes,
			"boundary IPv4 bytes mismatch");
		TETH_ASSERT(stats->num_ipv4_pkts == pairs[i].num_ipv4_pkts,
			"boundary IPv4 packets mismatch");
		TETH_ASSERT(stats->num_ipv6_pkts == pairs[i].num_ipv6_pkts,
			"boundary IPv6 packets mismatch");
		TETH_ASSERT(stats->num_ipv6_bytes == pairs[i].num_ipv6_bytes,
			"boundary IPv6 bytes mismatch");
		TETH_ASSERT(stats->num_nonip_pkts == pairs[i].num_nonip_pkts,
			"boundary Non-IP packets mismatch");
		TETH_ASSERT(stats->num_nonip_bytes == pairs[i].num_nonip_bytes,
			"boundary Non-IP bytes mismatch");
	}

	IPA_UT_INFO("================ done ============\n");

restore:
	if (pyld)
		ipahal_destroy_stats_init_pyld(pyld);
	if (cfg)
		ipahal_destroy_stats_init_pyld(cfg);
	kfree(raw);
	kvfree(out);
	return rc;
}

/*
 * ipa_test_hw_stats_teth_v7_ip_only_capacity() - verify the maximum
 * production allocation: 13 headers and 174 IP-only producer/consumer pairs.
 *
 * This directly validates the allocation policy used by ipa_7_0_mem_part:
 * 13 * 24B headers + 174 * 24B counters = 0x1188, with 44B of CFG bits.
 */
static int ipa_test_hw_stats_teth_v7_ip_only_capacity(void *priv)
{
	int rc = 0;
	struct ipahal_stats_init_tethering init;
	struct ipahal_stats_init_pyld *pyld = NULL;
	struct ipahal_stats_init_pyld *cfg = NULL;
	struct ipahal_stats_get_offset_tethering goff;
	struct ipahal_stats_offset off = { 0 };
	int prod, cons;

	memset(&init, 0, sizeof(init));

	/* 13 producers x 13 consumers, then five more pairs = 174 pairs. */
	for (prod = 0; prod < 13; prod++) {
		init.prod_bitmask[ipahal_get_ep_reg_idx(prod)] |=
			ipahal_get_ep_bit(prod);
		for (cons = 32; cons < 45; cons++)
			init.cons_bitmask[prod][ipahal_get_ep_reg_idx(cons)] |=
				ipahal_get_ep_bit(cons);
	}
	for (cons = 45; cons < 50; cons++)
		init.cons_bitmask[0][ipahal_get_ep_reg_idx(cons)] |=
			ipahal_get_ep_bit(cons);

	IPA_UT_INFO("========teth v7.0 IP-only capacity========\n");

	pyld = ipahal_stats_generate_init_pyld(IPAHAL_HW_STATS_TETHERING,
		&init, false);
	TETH_ASSERT(pyld != NULL, "capacity generate_init_pyld returned NULL");
	TETH_ASSERT(sizeof(struct ipahal_stats_tethering_hdr_v7_0_hw) == 24,
		"IPA7 tethering header size != 24");
	TETH_ASSERT(sizeof(struct ipahal_stats_tethering_hw) == 24,
		"IP-only tethering counter size != 24");
	TETH_ASSERT(pyld->len == 0x1188,
		"13-header/174-pair IP-only payload size != 0x1188");

	memset(&goff, 0, sizeof(goff));
	goff.init = init;
	rc = ipahal_stats_get_offset(IPAHAL_HW_STATS_TETHERING, &goff, &off);
	TETH_ASSERT(rc == 0, "capacity get_offset failed");
	TETH_ASSERT(off.offset == 0x138, "capacity header size != 0x138");
	TETH_ASSERT(off.size == 0x1050, "capacity counter size != 0x1050");

	cfg = ipahal_stats_generate_teth_cfg_pyld(&init, false);
	TETH_ASSERT(cfg != NULL, "capacity generate_teth_cfg_pyld returned NULL");
	TETH_ASSERT(cfg->len == 0x2c, "174-pair CFG payload size != 44");
	TETH_ASSERT(cfg->len <= IPA_MEM_PART(stats_tethering_cfg_size),
		"capacity CFG payload exceeds SRAM region");

	IPA_UT_INFO("================ done ============\n");

restore:
	if (pyld)
		ipahal_destroy_stats_init_pyld(pyld);
	if (cfg)
		ipahal_destroy_stats_init_pyld(cfg);
	return rc;
}

static bool ipa_test_hw_stats_teth_pipe_enabled(
	const struct ipahal_stats_init_tethering *init, int pipe)
{
	u32 reg_idx = ipahal_get_ep_reg_idx(pipe);
	u32 bit = ipahal_get_ep_bit(pipe);
	int prod;

	if (init->prod_bitmask[reg_idx] & bit)
		return true;

	for (prod = 0; prod < IPAHAL_PIPES_NUM; prod++) {
		if (init->cons_bitmask[prod][reg_idx] & bit)
			return true;
	}

	return false;
}

/* Keep this predicate in lockstep with ipa_hw_stats_ep_is_l2(). */
static bool ipa_test_hw_stats_teth_pipe_is_l2(int pipe)
{
	enum ipa_client_type client;

	if (IPA_CLIENT_IS_ETH_PROD(pipe) ||
		IPA_CLIENT_IS_ETH_CONS_PIPE(pipe) ||
		IPA_CLIENT_IS_WLAN_PROD(pipe))
		return true;

	client = ipa3_get_client_by_pipe(pipe);
	return IPA_CLIENT_IS_WLAN_CONS(client) ||
		IPA_CLIENT_IS_WLAN_QOS_CONS(client);
}

#define TETH_MEM_PART_ASSERT(member, expected) \
	TETH_ASSERT(IPA_MEM_PART(member) == (expected), #member " mismatch")

/*
 * ipa_test_hw_stats_teth_v7_hw_cfg() - verify IPA7 tethering CFG programming.
 *
 * The test compares the live CFG SRAM bytes with a HAL-generated payload from
 * the driver's live init state, then checks the two base registers and the
 * IPA7 memory partition. The production policy retains IP-only counter
 * entries; mixed-stride coverage is provided by the HAL-only tests.
 */
static int ipa_test_hw_stats_teth_v7_hw_cfg(void *priv)
{
	int rc = 0;
	struct ipahal_stats_init_tethering init;
	struct ipahal_stats_init_pyld *pyld = NULL;
	struct ipahal_stats_init_pyld *cfg = NULL;
	void __iomem *cfg_sram = NULL;
	phys_addr_t sram_phys;
	u8 *actual_cfg = NULL;
	bool locked = false;
	int i;

	IPA_UT_INFO("========teth v7.0 hardware CFG verification========\n");

	TETH_ASSERT(ipa3_ctx->hw_stats && ipa3_ctx->hw_stats->enabled &&
		ipa3_ctx->hw_stats->teth_stats_enabled,
		"tethering stats are not initialized");

	mutex_lock(&ipa3_ctx->lock);
	locked = true;
	init = ipa3_ctx->hw_stats->teth.init;

	pyld = ipahal_stats_generate_init_pyld(IPAHAL_HW_STATS_TETHERING,
		&init, false);
	TETH_ASSERT(pyld != NULL, "live tethering init payload is empty");
	TETH_ASSERT(pyld->len <= IPA_MEM_PART(stats_tethering_size),
		"live tethering payload exceeds SRAM region");

	cfg = ipahal_stats_generate_teth_cfg_pyld(&init, false);
	TETH_ASSERT(cfg != NULL, "live tethering CFG payload is empty");
	TETH_ASSERT(cfg->len <= IPA_MEM_PART(stats_tethering_cfg_size),
		"live tethering CFG exceeds SRAM region");

	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {
		u32 reg_idx;
		u32 bit;
		bool is_l2;

		if (!ipa_test_hw_stats_teth_pipe_enabled(&init, i))
			continue;

		reg_idx = ipahal_get_ep_reg_idx(i);
		bit = ipahal_get_ep_bit(i);
		is_l2 = ipa_test_hw_stats_teth_pipe_is_l2(i);
		if (is_l2) {
			TETH_ASSERT(init.eth_bitmask[reg_idx] & bit,
				"enabled L2 pipe is missing ETH bit");
		} else {
			TETH_ASSERT(!(init.eth_bitmask[reg_idx] & bit),
				"enabled non-L2 pipe has ETH bit");
		}
		TETH_ASSERT(!(init.nonip_bitmask[reg_idx] & bit),
			"IP-only policy enabled a Non-IP bit");
	}

	TETH_ASSERT(ipahal_read_reg_n(IPA_STAT_TETHERING_BASE_n,
		ipa3_ctx->ee) == ipa3_ctx->smem_restricted_bytes +
		IPA_MEM_PART(stats_tethering_ofst),
		"tethering base register mismatch");
	TETH_ASSERT(ipahal_read_reg_n(IPA_STAT_TETHERING_CFG_BASE_n,
		ipa3_ctx->ee) == ipa3_ctx->smem_restricted_bytes +
		IPA_MEM_PART(stats_tethering_cfg_ofst),
		"tethering CFG base register mismatch");

	TETH_MEM_PART_ASSERT(stats_tethering_cfg_ofst, 0x20340);
	TETH_MEM_PART_ASSERT(stats_tethering_cfg_size, 0x30);
	TETH_MEM_PART_ASSERT(stats_tethering_ofst, 0x1f1b8);
	TETH_MEM_PART_ASSERT(stats_tethering_size, 0x1188);
	TETH_MEM_PART_ASSERT(stats_fnr_ofst, 0x20370);
	TETH_MEM_PART_ASSERT(ap_stats_drop_ofst, 0x21518);
	TETH_MEM_PART_ASSERT(ap_stats_drop_size, 0x230);
	TETH_MEM_PART_ASSERT(nat_tbl_ofst, 0x217a0);
	TETH_MEM_PART_ASSERT(ct_tbl_ofst, 0x217a0);
	TETH_MEM_PART_ASSERT(apps_v4_flt_nhash_ofst, 0x217a8);
	TETH_MEM_PART_ASSERT(apps_v6_flt_nhash_ofst, 0x221f0);
	TETH_MEM_PART_ASSERT(apps_v4_rt_nhash_ofst, 0x22c38);
	TETH_MEM_PART_ASSERT(apps_v6_rt_nhash_ofst, 0x22f40);
	TETH_MEM_PART_ASSERT(tethering_drop_stats_ofst, 0x23240);
	TETH_MEM_PART_ASSERT(nat_ct_stats_counters_ofst, 0x243f8);
	TETH_MEM_PART_ASSERT(nat_ct_stats_cfg_ofst, 0x2c3f8);
	TETH_MEM_PART_ASSERT(nat_ct_drop_stats_counters_ofst, 0x2c478);
	TETH_MEM_PART_ASSERT(nat_ct_drop_stats_cfg_ofst, 0x34478);
	TETH_MEM_PART_ASSERT(fnr_drop_counters_ofst, 0x344f8);
	TETH_MEM_PART_ASSERT(policing_drop_counters_ofst, 0x356a0);
	TETH_MEM_PART_ASSERT(shaping_drop_counters_ofst, 0x35aa0);
	TETH_MEM_PART_ASSERT(shaping_drop_pkt_byte_counters_ofst, 0x35ea0);
	TETH_MEM_PART_ASSERT(end_ofst, 0x36ea0);
	TETH_ASSERT(IPA_MEM_PART(stats_tethering_ofst) +
		IPA_MEM_PART(stats_tethering_size) ==
		IPA_MEM_PART(stats_tethering_cfg_ofst),
		"tethering counters and CFG are not adjacent");
	TETH_ASSERT(IPA_MEM_PART(stats_tethering_cfg_ofst) +
		IPA_MEM_PART(stats_tethering_cfg_size) ==
		IPA_MEM_PART(stats_fnr_ofst),
		"tethering CFG and FNR regions are not adjacent");
	TETH_ASSERT(IPA_MEM_PART(end_ofst) <= ipa3_ctx->smem_sz,
		"IPA7 memory map exceeds shared SRAM");

	sram_phys = ipa3_ctx->ipa_wrapper_base + ipa3_ctx->ctrl->ipa_reg_base_ofst +
		ipahal_get_reg_n_ofst(IPA_SW_AREA_RAM_DIRECT_ACCESS_n,
			ipa3_ctx->smem_restricted_bytes / 4) +
		IPA_MEM_PART(stats_tethering_cfg_ofst);
	cfg_sram = ioremap(sram_phys, cfg->len);
	TETH_ASSERT(cfg_sram != NULL, "failed to map tethering CFG SRAM");

	actual_cfg = kmalloc(cfg->len, GFP_KERNEL);
	TETH_ASSERT(actual_cfg != NULL, "failed to allocate CFG readback buffer");
	memcpy_fromio(actual_cfg, cfg_sram, cfg->len);
	TETH_ASSERT(!memcmp(actual_cfg, cfg->data, cfg->len),
		"tethering CFG SRAM contents mismatch");

	IPA_UT_INFO("================ done ============\n");

restore:
	kfree(actual_cfg);
	if (cfg_sram)
		iounmap(cfg_sram);
	if (cfg)
		ipahal_destroy_stats_init_pyld(cfg);
	if (pyld)
		ipahal_destroy_stats_init_pyld(pyld);
	if (locked)
		mutex_unlock(&ipa3_ctx->lock);
	return rc;
}

#undef TETH_MEM_PART_ASSERT
#undef TETH_ASSERT

static int ipa_test_hw_stats_reset_teth_stats(void *priv)
{
	int ret;
	unsigned int random_int;
	enum ipa_client_type prod;
	enum ipa_client_type cons;

	get_random_bytes(&random_int, sizeof(random_int));
	prod = (random_int % IPA_CLIENT_MAX) & 0x8FFFFFFE;

	get_random_bytes(&random_int, sizeof(random_int));
	cons = ((random_int % IPA_CLIENT_MAX) & 0x8FFFFFFF) | 0x1;

	IPA_UT_INFO("========reset some tethering stats========\n");

	ret = ipa_reset_teth_stats(prod, cons);
	if (ret)
		IPA_UT_ERR("ipa_reset_teth_stats failed %d\n", ret);

	IPA_UT_INFO("================ done ============\n");

	return ret;
}

static int ipa_test_hw_stats_query_quota_stats(void *priv)
{
       struct ipa_quota_stats_all *out;
       int i, reg_idx, ep_idx;
       int res;

       out = kzalloc(sizeof(*out), GFP_KERNEL);
       if (!out)
               return -ENOMEM;

       IPA_UT_INFO("========get all quota stats========\n");

       res = ipa_get_quota_stats(out);
       if (res) {
               IPA_UT_ERR("ipa_get_quota_stats failed with code %u\n", res);
               goto teardown;
       }

       for (i = 0; i < IPA_CLIENT_MAX; i++) {
               ep_idx = ipa_get_ep_mapping(i);

               if (ep_idx == IPA_EP_NOT_ALLOCATED)
                       continue;

               if (IPA_CLIENT_IS_TEST(i))
                       continue;

               reg_idx = ipahal_get_ep_reg_idx(ep_idx);
               if (!(ipa3_ctx->hw_stats->quota.init.enabled_bitmask[reg_idx] &
                       ipahal_get_ep_bit(ep_idx)))
                       continue;

               IPA_UT_INFO("%s:\n", ipa_clients_strings[i]);
	       if(i < ipa3_get_max_num_pipes()){
               	IPA_UT_INFO("num_ipv4_bytes=%llu\n", out->client[i].num_ipv4_bytes);
               	IPA_UT_INFO("num_ipv6_bytes=%llu\n", out->client[i].num_ipv6_bytes);
               	IPA_UT_INFO("num_ipv4_pkts=%u\n", out->client[i].num_ipv4_pkts);
               	IPA_UT_INFO("num_ipv6_pkts=%u\n", out->client[i].num_ipv6_pkts);
	       }

       }

       IPA_UT_INFO("================ done ============\n");

teardown:
       kfree(out);
       return res;
}

static int ipa_test_hw_stats_reset_all_quota_stats(void *priv)
{
       int ret;

       IPA_UT_INFO("========reset all drop stats========\n");

       ret = ipa_reset_all_quota_stats();
       if (ret)
               IPA_UT_ERR("ipa_reset_all_quota_stats failed %d\n", ret);

       IPA_UT_INFO("================ done ============\n");

       return ret;
}

static int ipa_test_hw_stats_set_uc_event_ring(void *priv)
{
	struct ipa_ioc_flt_rt_counter_alloc *counter = NULL;
	int ret = 0;

	/* set uc event ring */
	IPA_UT_INFO("========set uc event ring ========\n");

	if (ipa3_ctx->ipa_hw_type >= IPA_HW_v4_5 &&
		ipa3_ctx->ipa_hw_type != IPA_HW_v4_11 &&
		ipa3_ctx->ipa_hw_type != IPA_HW_v5_2) {
		if (ipa3_ctx->uc_ctx.uc_loaded &&
			!ipa3_ctx->uc_ctx.uc_event_ring_valid) {
			if (ipa3_uc_setup_event_ring()) {
				IPA_UT_ERR("failed to set uc_event ring\n");
				return -EFAULT;
			}
		} else
			IPA_UT_ERR("uc-loaded %d, ring-valid %d",
				ipa3_ctx->uc_ctx.uc_loaded,
				ipa3_ctx->uc_ctx.uc_event_ring_valid);
	}
	IPA_UT_INFO("================ done ============\n");

	/* allocate counters */
	IPA_UT_INFO("========set hw counter ========\n");

	counter = kzalloc(sizeof(struct ipa_ioc_flt_rt_counter_alloc),
					  GFP_KERNEL);
	if (!counter)
		return -ENOMEM;

	counter->hw_counter.num_counters = 4;
	counter->sw_counter.num_counters = 4;

	/* allocate counters */
	ret = ipa3_alloc_counter_id(counter);
	if (ret < 0) {
		IPA_UT_ERR("ipa3_alloc_counter_id fails\n");
		ret = -ENOMEM;
	}
	ipa3_ctx->fnr_info.hw_counter_offset = counter->hw_counter.start_id;
	ipa3_ctx->fnr_info.sw_counter_offset = counter->sw_counter.start_id;
	IPA_UT_INFO("hw-offset %d, sw-offset %d\n",
		ipa3_ctx->fnr_info.hw_counter_offset,
			ipa3_ctx->fnr_info.sw_counter_offset);

	kfree(counter);
	return ret;
}

static int ipa_test_hw_stats_set_quota(void *priv)
{
	int ret;
	uint64_t quota = 500;

	IPA_UT_INFO("========set quota ========\n");
	ret = ipa3_uc_quota_monitor(quota);
	if (ret < 0) {
		IPA_UT_ERR("ipa3_uc_quota_monitor fails\n");
		ret = -ENOMEM;
	}
	IPA_UT_INFO("================ done ============\n");
	return ret;
}

static int ipa_test_hw_stats_set_bw(void *priv)
{
	int ret;
	struct ipa_wdi_bw_info *info = NULL;

	IPA_UT_INFO("========set BW voting ========\n");
	info = kzalloc(sizeof(struct ipa_wdi_bw_info),
					  GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->num = 3;
	info->threshold[0] = 200;
	info->threshold[1] = 400;
	info->threshold[2] = 600;

	ret = ipa_uc_bw_monitor(info);
	if (ret < 0) {
		IPA_UT_ERR("ipa_uc_bw_monitor fails\n");
		ret = -ENOMEM;
	}

	IPA_UT_INFO("================ done ============\n");

	kfree(info);
	return ret;
}
static int ipa_test_hw_stats_hit_quota(void *priv)
{
	int ret = 0, counter_index, i;
	struct ipa_flt_rt_stats stats;

	/* set sw counters */
	IPA_UT_INFO("========set quota hit========\n");
	counter_index = ipa3_ctx->fnr_info.sw_counter_offset + UL_WLAN_TX;

	for (i = 0; i < 5; i++) {
		IPA_UT_INFO("========set 100 ========\n");
		memset(&stats, 0, sizeof(struct ipa_flt_rt_stats));
		stats.num_bytes = 100;
		stats.num_pkts = 69;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			counter_index, stats.num_pkts, stats.num_bytes);
		ret = ipa_set_flt_rt_stats(counter_index, stats);
		if (ret < 0) {
			IPA_UT_ERR("ipa_set_flt_rt_stats fails\n");
			ret = -ENOMEM;
		}
		msleep(1000);
		IPA_UT_INFO("========set 200 ========\n");
		memset(&stats, 0, sizeof(struct ipa_flt_rt_stats));
		stats.num_bytes = 200;
		stats.num_pkts = 69;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			counter_index, stats.num_pkts, stats.num_bytes);
		ret = ipa_set_flt_rt_stats(counter_index, stats);
		if (ret < 0) {
			IPA_UT_ERR("ipa_set_flt_rt_stats fails\n");
			ret = -ENOMEM;
		}
		msleep(1000);
		IPA_UT_INFO("========set 300 ========\n");
		memset(&stats, 0, sizeof(struct ipa_flt_rt_stats));
		stats.num_bytes = 300;
		stats.num_pkts = 69;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			counter_index, stats.num_pkts, stats.num_bytes);
		ret = ipa_set_flt_rt_stats(counter_index, stats);
		if (ret < 0) {
			IPA_UT_ERR("ipa_set_flt_rt_stats fails\n");
			ret = -ENOMEM;
		}
		msleep(1000);
		IPA_UT_INFO("========set 500 ========\n");
		memset(&stats, 0, sizeof(struct ipa_flt_rt_stats));
		stats.num_bytes = 500;
		stats.num_pkts = 69;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			counter_index, stats.num_pkts, stats.num_bytes);
		ret = ipa_set_flt_rt_stats(counter_index, stats);
		if (ret < 0) {
			IPA_UT_ERR("ipa_set_flt_rt_stats fails\n");
			ret = -ENOMEM;
		}
		msleep(1000);
		IPA_UT_INFO("========set 600 ========\n");
		memset(&stats, 0, sizeof(struct ipa_flt_rt_stats));
		stats.num_bytes = 600;
		stats.num_pkts = 69;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			counter_index, stats.num_pkts, stats.num_bytes);
		ret = ipa_set_flt_rt_stats(counter_index, stats);
		if (ret < 0) {
			IPA_UT_ERR("ipa_set_flt_rt_stats fails\n");
			ret = -ENOMEM;
		}
		msleep(1000);
		IPA_UT_INFO("========set 1000 ========\n");
		memset(&stats, 0, sizeof(struct ipa_flt_rt_stats));
		stats.num_bytes = 1000;
		stats.num_pkts = 69;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			counter_index, stats.num_pkts, stats.num_bytes);
		ret = ipa_set_flt_rt_stats(counter_index, stats);
		if (ret < 0) {
			IPA_UT_ERR("ipa_set_flt_rt_stats fails\n");
			ret = -ENOMEM;
		}
	}
	IPA_UT_INFO("================ done ============\n");
	return ret;
}



/* Suite definition block */
IPA_UT_DEFINE_SUITE_START(hw_stats, "HW stats test",
	ipa_test_hw_stats_suite_setup, ipa_test_hw_stats_suite_teardown)
{
	IPA_UT_ADD_TEST(verify_wide_counter_index,
		"Verify widened v7 FnR counter index",
		ipa_test_hw_stats_verify_wide_counter_index, true,
		IPA_HW_v7_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(configure, "Configure the setup",
		ipa_test_hw_stats_configure, false, IPA_HW_v4_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(add_rules, "Add FLT and RT rules",
		ipa_test_hw_stats_add_FnR, false, IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_stats_one_by_one, "Query one by one",
		ipa_test_hw_stats_query_FnR_one_by_one, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_stats_one_shot, "Query one shot",
		ipa_test_hw_stats_query_FnR_one_shot, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_stats_one_shot_clean, "Query and clean",
		ipa_test_hw_stats_query_FnR_clean, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_sw_stats, "Query SW stats",
		ipa_test_hw_stats_query_sw_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(set_sw_stats, "Set SW stats to dummy values",
		ipa_test_hw_stats_set_sw_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_drop_stats, "Query drop stats",
		ipa_test_hw_stats_query_drop_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(reset_drop_stats, "Reset drop stats",
		ipa_test_hw_stats_reset_all_drop_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(verify_max_counters,
		"Verify FnR/drop/NAT-CT max counter registers",
		ipa_test_hw_stats_verify_max_counters, true,
		IPA_HW_v7_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_teth_stats, "Query tethering stats",
		ipa_test_hw_stats_query_teth_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(teth_v7_roundtrip,
		"IPA7.0 tethering HAL mixed-stride round-trip",
		ipa_test_hw_stats_teth_v7_roundtrip, true,
		IPA_HW_v7_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(teth_v7_hal_boundary,
		"IPA7.0 tethering HAL CFG-byte and mixed-stride boundaries",
		ipa_test_hw_stats_teth_v7_hal_boundary, true,
		IPA_HW_v7_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(teth_v7_ip_only_capacity,
		"IPA7.0 tethering HAL IP-only capacity",
		ipa_test_hw_stats_teth_v7_ip_only_capacity, true,
		IPA_HW_v7_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(teth_v7_hw_cfg,
		"IPA7.0 tethering CFG SRAM and memory-map programming",
		ipa_test_hw_stats_teth_v7_hw_cfg, false,
		IPA_HW_v7_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(reset_teth_stats, "Reset tethering stats",
		ipa_test_hw_stats_reset_teth_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_quota_stats, "Query quota stats",
		ipa_test_hw_stats_query_quota_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(reset_quota_stats, "Reset quota stats",
		ipa_test_hw_stats_reset_all_quota_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(set_uc_evtring, "Set uc event ring",
		ipa_test_hw_stats_set_uc_event_ring, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(set_quota, "Set quota",
		ipa_test_hw_stats_set_quota, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(set_bw_voting, "Set bw_voting",
		ipa_test_hw_stats_set_bw, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(hit_quota, "quota hits",
		ipa_test_hw_stats_hit_quota, false,
		IPA_HW_v4_5, IPA_HW_MAX),

} IPA_UT_DEFINE_SUITE_END(hw_stats);
