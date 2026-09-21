// SPDX-License-Identifier: GPL-2.0-only
/*
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include <linux/in.h>
#include <linux/in6.h>
#include <linux/socket.h>
#include "ipa_stats_genl.h"
#include "ipa_api.h"
#include "ipa_be.h"
#include "ipa_be_clientdb.h"
#include "ipa_be_flt_mgmt.h"
#include "ipa_i.h"
#include "ipa_table.h"


#include "ipa_be_nat_mgmt.h"
#include "ipa_nat_utils.h"
#include "ipa_ipv6ct.h"
#include "ipa_nat_drv.h"


#define HDR_METADATA_MUX_ID_BMASK 0x00FF0000
#define HDR_METADATA_MUX_ID_SHFT 0x10

/*
 * BE_FLOW (RX metadata bit 29) marks backhaul-offloaded flows. The IEMAC pipe
 * sets this on the EP metadata register via ipa3_cfg_ep_metadata(), but for
 * IPv4 the NAT engine rewrites the metadata register from the PDN entry's
 * src_metadata after NAT - so BE_FLOW must be re-asserted here or it would be
 * lost post-NAT.
 */
#define HDR_METADATA_BE_FLOW (1 << 29)

/*
 * Placeholder public IP owning the dedicated PDN entry used by IPv4 connection
 * tracking (LAN2LAN). On IPA v7.0 the NAT hardware validates PDN[pdn_index]
 * before accepting a match: an all-zero PDN makes it reject an otherwise-correct
 * conn_tracking entry (nat_hit=0, NAT exception -> SW path). CT therefore cannot
 * simply point at PDN[0] — that slot belongs to whichever WAN PDN claimed it,
 * and it is zeroed again when that WAN goes down. Instead CT owns its own PDN,
 * keyed on this placeholder IP so no WAN can allocate, overwrite or deallocate
 * it (see ipa_be_ensure_ct_pdn).
 *
 * conn_tracking=1 suppresses source translation, so this address is never
 * written into a packet; it only needs to be non-zero and distinct from every
 * real WAN public IP. RFC 5737 TEST-NET-1 (192.0.2.1) is used so that if it were
 * ever erroneously applied it is unmistakable in a capture and never routable.
 */
#define IPA_BE_CT_PDN_PLACEHOLDER_IP 0xC0000201 /* 192.0.2.1 */

#ifdef FEATURE_WLAN_256_CLIENTS
#define IPA_MAX_NUM_WIFI_CLIENTS 350
#else
#define IPA_MAX_NUM_WIFI_CLIENTS  32
#endif


struct nat_app {
    nat_table_entry *cache;
	struct mutex cache_lock;  /* Protects cache and curCnt */
	uint32_t pub_ip_addr;
	uint32_t pub_ip_addr_pre;
	uint32_t nat_table_hdl;

	/*
	 * Dedicated PDN entry owned by IPv4 CT (LAN2LAN), keyed on
	 * IPA_BE_CT_PDN_PLACEHOLDER_IP. That keying is what makes it survive WAN
	 * PDN churn -- no WAN can allocate, overwrite or deallocate it, whatever
	 * index it happens to hold. Valid only while ct_pdn_valid is set.
	 */
	uint8_t ct_pdn_index;
	bool ct_pdn_valid;

	int curCnt, max_entries;

	const char* mem_type;

	uint32_t tcp_timeout;
	uint32_t udp_timeout;
	uint32_t gre_timeout;

	uint32_t pwr_save_ifs[IPA_MAX_NUM_WIFI_CLIENTS];

	struct nf_conntrack *ct;
	struct nfct_handle *ct_hdl;
};

struct nat_app *nat_app = NULL;

/* NAT entry direction enumeration */
enum nat_direction {
	NAT_DIRECTION_UNKNOWN,
	NAT_DIRECTION_OUTBOUND,
	NAT_DIRECTION_INBOUND,
	NAT_DIRECTION_MAX
};

/* Forward declaration for NAT entries collection */
struct nat_entries_collection;

/* IPv6 Connection Tracking entry structure */
struct ipv6_ct_entry {
	uint32_t rule_handle;
	uint64_t src_ipv6_msb;
	uint64_t src_ipv6_lsb;
	uint64_t dest_ipv6_msb;
	uint64_t dest_ipv6_lsb;
	uint16_t src_port;
	uint16_t dest_port;
	uint32_t flow_rule_id;
	uint16_t all_pkts_stats_cnt_index;
	uint16_t non_frag_stats_cnt_index;
	int pdn_iface;
	/* iface/top like nat_table_entry, plus MACs — ipa_be_v6_delete_filter_rule
	 * is MAC-keyed, so v6 must cache them (v4 delete is IP-keyed).
	 */
	int flow_iface;
	int return_iface;
	int flow_top_iface;
	int return_top_iface;
	u8  flow_mac[6];
	u8  return_mac[6];
	/* Original ECM add-time tuple (network order), pre-canonical-swap.
	 * See nat_table_entry.
	 */
	uint32_t orig_flow_ip[4];
	uint32_t orig_return_ip[4];
	uint16_t orig_flow_ident;
	uint16_t orig_return_ident;
};

struct NatEntryBase
{
	enum ipa_ip_type m_type;
	uint32_t m_timestamp;
	enum nat_direction m_direction;
	uint32_t m_ruleHandle;
	uint8_t m_protocol;
	bool m_enabled;

	/*
	 * Dummy traffic is a traffic that will be sent/received from APPS. Need to install NAT rules to
	 * catch DL traffic that otherwise will NAT miss and go back to Q6. E.g. embedded traffic.
	 */
	bool m_isDummy;

	bool m_ucp;
	bool m_s;
	uint16_t m_uc_activation_index;

	bool isVlan;
	bool IsVlanUp;

	/* IPv6 CT specific data */
	struct ipv6_ct_entry ipv6_ct;
};

struct NatBase
{
	enum ipa_ip_type m_type;
	struct nat_entries_collection *m_temp;
	//IpAddressesCollectionBase& m_pwrSaveIfs;

	int m_maxEntries;

	//NatProxyBase& m_proxy;
	//ConntrackTimestampUtil& m_ctTimestampUtil;
	struct NatEntryBase *m_cache;
	struct mutex cache_lock;  /* Protects m_cache for IPv6 CT entries */

	const char* ct_mem_type;

	uint32_t ct_table_hdl;
	//IpAddress& m_previousWanAddress;
};

struct NatBase *NatBase = NULL;

/* BE ctx for FLUSH_SWITCH_AE pushes from ipa_be_signal_iface_offline.
 * Set at init / cleared at exit under ipa_be_nat_mgmt_lock; non-NULL
 * implies nat_app and NatBase are valid.
 */
static struct ipa_ctx_instance_internal *g_ipa_ctx = NULL;

/*
 * Guards the nat_app / NatBase lifetime against cache readers like
 * ipa_be_query_tuple_stats(). The per-struct cache_lock can't: it lives inside
 * the struct exit() frees, so a reader that passed the NULL check could lock
 * freed memory. Held across the NULL check and cache walk; nests outside cache_lock.
 */
static DEFINE_MUTEX(ipa_be_nat_mgmt_lock);

/*
 * Unified cumulative stats, keyed by HW counter index (all_pkts_stats_cnt_index,
 * 1-based; slot 0 unused). A single 100 ms poller is the only reader of the
 * clear-on-read HW counters: it adds each counter's delta into total. Both ECM
 * (ipa_sync_*, via get_ct_stats_cached) and stats_mgr (ipa_be_query_tuple_stats)
 * then read total non-destructively, so they no longer clear the HW from under
 * each other. ipa_be_cum_lock nests inside ipa_be_nat_mgmt_lock / cache_lock.
 */
struct ipa_be_cum_counter {
	struct ipahal_stats_nat_ct total;       /* monotonic cumulative */
	struct ipahal_stats_nat_ct ecm_last;    /* last total reported to ECM */
	bool active;
};
static struct ipa_be_cum_counter *ipa_be_cum;
static u32 ipa_be_cum_n;                        /* entries in ipa_be_cum */
static DEFINE_MUTEX(ipa_be_cum_lock);

static struct delayed_work ipa_be_stats_poll_work;
static bool ipa_be_stats_poll_running;  /* poller queued; guarded by ipa_be_cum_lock */
static bool ipa_be_stats_poll_rearm;    /* counter assigned since the tick's snapshot */

#define IPA_BE_STATS_POLL_MS 100u

/* Collect distinct active counter indices from both caches into idx_out. @seen
 * (sized ipa_be_cum_n) dedupes any counter index that appears in more than one
 * cache entry so each is accumulated once per tick. Caller holds
 * ipa_be_nat_mgmt_lock.
 */
static u32 ipa_be_collect_active_counters(u16 *idx_out, bool *seen)
{
	u32 n = 0;
	int i;

	if (nat_app && nat_app->cache) {
		mutex_lock(&nat_app->cache_lock);
		for (i = 0; i < nat_app->max_entries; i++) {
			u16 c = nat_app->cache[i].all_pkts_stats_cnt_index;

			if (nat_app->cache[i].enabled && c && c < ipa_be_cum_n &&
			    !seen[c]) {
				seen[c] = true;
				idx_out[n++] = c;
			}
		}
		mutex_unlock(&nat_app->cache_lock);
	}

	if (NatBase && NatBase->m_cache) {
		mutex_lock(&NatBase->cache_lock);
		for (i = 0; i < NatBase->m_maxEntries; i++) {
			u16 c = NatBase->m_cache[i].ipv6_ct.all_pkts_stats_cnt_index;

			if (NatBase->m_cache[i].m_enabled &&
			    NatBase->m_cache[i].m_type == IPA_IP_v6 &&
			    c && c < ipa_be_cum_n && !seen[c]) {
				seen[c] = true;
				idx_out[n++] = c;
			}
		}
		mutex_unlock(&NatBase->cache_lock);
	}

	return n;
}

static void ipa_be_stats_poll_work_fn(struct work_struct *work)
{
	u16 *idx;
	bool *seen;
	u32 n, i, cap;

	mutex_lock(&ipa_be_nat_mgmt_lock);

	/* Table gone => BE torn down (exit cleared it); stop for good. A later
	 * re-init re-kicks. This is the only non-transient stop condition.
	 */
	if (!ipa_be_cum || ipa_be_cum_n < 2) {
		mutex_lock(&ipa_be_cum_lock);
		ipa_be_stats_poll_running = false;
		mutex_unlock(&ipa_be_cum_lock);
		mutex_unlock(&ipa_be_nat_mgmt_lock);
		return;
	}

	/*
	 * Clear the re-arm request before sampling the caches. A kick racing in
	 * after this point re-sets it, so we never stop with a freshly assigned
	 * counter unsampled.
	 */
	mutex_lock(&ipa_be_cum_lock);
	ipa_be_stats_poll_rearm = false;
	mutex_unlock(&ipa_be_cum_lock);

	/* One slot per cache entry: the true upper bound on collected indices. */
	cap = (nat_app ? nat_app->max_entries : 0) +
	      (NatBase ? NatBase->m_maxEntries : 0);

	idx = cap ? kmalloc_array(cap, sizeof(*idx), GFP_KERNEL) : NULL;
	seen = kcalloc(ipa_be_cum_n, sizeof(*seen), GFP_KERNEL);
	if (cap && (!idx || !seen)) {
		kfree(idx);
		kfree(seen);
		goto rearm;   /* transient alloc failure: retry next tick */
	}

	n = cap ? ipa_be_collect_active_counters(idx, seen) : 0;
	kfree(seen);

	/* HW read-and-clear outside cache_lock; once per counter this tick. */
	for (i = 0; i < n; i++) {
		struct ipahal_stats_nat_ct hw = {};

		if (ipa_get_nat_ct_stats(idx[i], &hw))
			continue;

		mutex_lock(&ipa_be_cum_lock);
		ipa_be_cum[idx[i]].active = true;
		ipa_be_cum[idx[i]].total.num_pkts_inbound        += hw.num_pkts_inbound;
		ipa_be_cum[idx[i]].total.num_pkts_cache_inbound  += hw.num_pkts_cache_inbound;
		ipa_be_cum[idx[i]].total.num_bytes_inbound       += hw.num_bytes_inbound;
		ipa_be_cum[idx[i]].total.num_pkts_outbound       += hw.num_pkts_outbound;
		ipa_be_cum[idx[i]].total.num_pkts_cache_outbound += hw.num_pkts_cache_outbound;
		ipa_be_cum[idx[i]].total.num_bytes_outbound      += hw.num_bytes_outbound;
		mutex_unlock(&ipa_be_cum_lock);
	}

	kfree(idx);

	/*
	 * Stop when idle: no active counters this tick and no kick raced in.
	 * The next ipa_be_cum_assign() re-kicks. Both the stop and kick decisions
	 * use ipa_be_stats_poll_running/_rearm under ipa_be_cum_lock, so a kick
	 * cannot be lost in the gap.
	 */
	mutex_lock(&ipa_be_cum_lock);
	if (n == 0 && !ipa_be_stats_poll_rearm) {
		ipa_be_stats_poll_running = false;
		mutex_unlock(&ipa_be_cum_lock);
		mutex_unlock(&ipa_be_nat_mgmt_lock);
		return;
	}
	mutex_unlock(&ipa_be_cum_lock);

rearm:
	schedule_delayed_work(&ipa_be_stats_poll_work,
			      msecs_to_jiffies(IPA_BE_STATS_POLL_MS));

	mutex_unlock(&ipa_be_nat_mgmt_lock);
}

/* Start the poller if idle. Safe to call from any entry-add path. */
static void ipa_be_stats_poll_kick(void)
{
	mutex_lock(&ipa_be_cum_lock);
	if (ipa_be_cum) {
		/* Force the in-flight (or next) tick to re-arm rather than stop,
		 * covering a kick that races a tick already past its snapshot.
		 */
		ipa_be_stats_poll_rearm = true;
		if (!ipa_be_stats_poll_running) {
			ipa_be_stats_poll_running = true;
			schedule_delayed_work(&ipa_be_stats_poll_work,
					      msecs_to_jiffies(IPA_BE_STATS_POLL_MS));
		}
	}
	mutex_unlock(&ipa_be_cum_lock);
}

/* Drop a counter's cumulative record when its HW counter is freed/recycled. */
static void ipa_be_cum_release(u16 counter_idx)
{
	if (!counter_idx)
		return;
	mutex_lock(&ipa_be_cum_lock);
	if (ipa_be_cum && counter_idx < ipa_be_cum_n)
		memset(&ipa_be_cum[counter_idx], 0, sizeof(ipa_be_cum[counter_idx]));
	mutex_unlock(&ipa_be_cum_lock);
}

/* Called when a cache entry is assigned a counter: start from a clean total and
 * ensure the poller is running. May hold a cache_lock (cum_lock nests inside).
 */
static void ipa_be_cum_assign(u16 counter_idx)
{
	ipa_be_cum_release(counter_idx);
	ipa_be_stats_poll_kick();
}

/**
 * ipa_be_free_nat_counters_v4() - Free counters for IPv4 NAT rule
 * @nat_table_hdl: NAT table handle
 * @private_ip: Client's private IP address
 * @all_pkts_counter: all_pkts counter index to free (0 = skip)
 * @non_frag_counter: non_frag counter index to free (0 = skip)
 *
 * Frees both all_pkts and non_frag counters for a NAT rule.
 * Handles counter_id == 0 gracefully (no-op).
 */
static void ipa_be_free_nat_counters_v4(
	uint32_t nat_table_hdl,
	uint32_t private_ip,
	uint16_t all_pkts_counter,
	uint16_t non_frag_counter)
{
	/* Free all_pkts counter */
	if (all_pkts_counter != 0) {
		IPA_BE_DBG("Freeing all_pkts counter %u for IP 0x%08X\n",
			all_pkts_counter, private_ip);
		ipa_nat_free_counter_v4(nat_table_hdl, private_ip,
			true, all_pkts_counter);
	}

	/* Free non_frag counter */
	if (non_frag_counter != 0) {
		IPA_BE_DBG("Freeing non_frag counter %u for IP 0x%08X\n",
			non_frag_counter, private_ip);
		ipa_nat_free_counter_v4(nat_table_hdl, private_ip,
			false, non_frag_counter);
	}
}

/**
 * ipa_be_free_ct_counters_v6() - Free counters for IPv6 CT rule
 * @ct_table_hdl: CT table handle
 * @src_ipv6_lsb: Client's source IPv6 address LSB
 * @src_ipv6_msb: Client's source IPv6 address MSB
 * @all_pkts_counter: all_pkts counter index to free (0 = skip)
 * @non_frag_counter: non_frag counter index to free (0 = skip)
 *
 * Frees both all_pkts and non_frag counters for a CT rule.
 * Handles counter_id == 0 gracefully (no-op).
 */
static void ipa_be_free_ct_counters_v6(
	uint32_t ct_table_hdl,
	uint64_t src_ipv6_lsb,
	uint64_t src_ipv6_msb,
	uint16_t all_pkts_counter,
	uint16_t non_frag_counter)
{
	/* Free all_pkts counter */
	if (all_pkts_counter != 0) {
		IPA_BE_DBG("Freeing all_pkts counter %u for IPv6 0x%016llX%016llX\n",
			all_pkts_counter, src_ipv6_msb, src_ipv6_lsb);
		ipa_ct_free_counter_v6(ct_table_hdl, src_ipv6_lsb,
			src_ipv6_msb, true, all_pkts_counter);
	}

	/* Free non_frag counter */
	if (non_frag_counter != 0) {
		IPA_BE_DBG("Freeing non_frag counter %u for IPv6 0x%016llX%016llX\n",
			non_frag_counter, src_ipv6_msb, src_ipv6_lsb);
		ipa_ct_free_counter_v6(ct_table_hdl, src_ipv6_lsb,
			src_ipv6_msb, false, non_frag_counter);
	}
}

void ipa_be_nat_mgmt_exit(void)
{
	struct ipa_be_cum_counter *cum;

	IPA_BE_DBG("DEBUG: inside be_nat_mgmt_exit\n");

	/*
	 * Detach the table first, then cancel. The poller re-arms unconditionally
	 * while ipa_be_cum is set (and checks it under ipa_be_nat_mgmt_lock at the
	 * top of each tick), so clearing it under both locks before the cancel
	 * guarantees no tick re-arms past cancel_delayed_work_sync(). Readers take
	 * ipa_be_cum_lock, so free under it.
	 */
	mutex_lock(&ipa_be_nat_mgmt_lock);
	mutex_lock(&ipa_be_cum_lock);
	cum = ipa_be_cum;
	ipa_be_cum = NULL;
	ipa_be_cum_n = 0;
	ipa_be_stats_poll_running = false;
	mutex_unlock(&ipa_be_cum_lock);
	mutex_unlock(&ipa_be_nat_mgmt_lock);

	if (cum) {
		cancel_delayed_work_sync(&ipa_be_stats_poll_work);
		kfree(cum);
	}

	mutex_lock(&ipa_be_nat_mgmt_lock);

	/* Clear g_ipa_ctx before tearing down nat_app/NatBase so a racing
	 * signal caller cannot observe a valid ctx alongside torn-down state.
	 */
	g_ipa_ctx = NULL;

	/* Clean up NatBase */
	if(NatBase)
	{
		if (NatBase->m_cache)
		{
			kfree(NatBase->m_cache);
			NatBase->m_cache = NULL;
			IPA_BE_DBG("DEBUG: freeing NatBase->m_cache\n");
		}

		/* Destroy IPv6 CT cache lock */
		mutex_destroy(&NatBase->cache_lock);

		kfree(NatBase);
		NatBase = NULL;
		IPA_BE_DBG("DEBUG: freed NatBase\n");
	}

	/* Clean up nat_app */
	if(nat_app)
	{
		/*
		 * Single teardown point for the shared IPv4 NAT table. The table
		 * is created lazily (by the first WAN PDN via ipa_be_add_pdn, or
		 * by ipa_be_ensure_nat_table_ct for LAN2LAN CT) and deliberately
		 * kept alive across PDN/CT removal (see ipa_be_remove_pdn). It is
		 * destroyed here, and only here.
		 */
		if (nat_app->nat_table_hdl) {
			int ret = ipa_nat_del_ipv4_tbl(nat_app->nat_table_hdl);

			if (ret)
				IPA_BE_ERR("unable to delete shared NAT table Error: %d\n",
					   ret);
			else
				IPA_BE_DBG("DEBUG: deleted shared IPv4 NAT table\n");
			ipa_be_reset();
		}

		if (nat_app->cache)
		{
			kfree(nat_app->cache);
			nat_app->cache = NULL;
			IPA_BE_DBG("DEBUG: freeing nat_app->cache\n");
		}

		/* Destroy cache lock */
		mutex_destroy(&nat_app->cache_lock);

		kfree(nat_app);
		nat_app = NULL;
		IPA_BE_DBG("DEBUG: freed nat_app\n");
	}

	mutex_unlock(&ipa_be_nat_mgmt_lock);
}

int ipa_be_nat_mgmt_init(struct ipa_ctx_instance_internal *ipa_ctx)
{
	int size = 0;

	IPA_BE_DBG("DEBUG: inside be_nat_mgmt_init\n");

	mutex_lock(&ipa_be_nat_mgmt_lock);

	/* Prevent double initialization of nat_app */
	if (nat_app) {
		IPA_BE_ERR("nat_app already initialized!\n");
		mutex_unlock(&ipa_be_nat_mgmt_lock);
		return -EALREADY;
	}

	/* Initialize context */
	nat_app = kzalloc(sizeof(*nat_app), GFP_KERNEL);
	if (!nat_app) {
		mutex_unlock(&ipa_be_nat_mgmt_lock);
		return -ENOMEM;
	}

	nat_app->curCnt = 0;
	nat_app->mem_type = "DDR";
	nat_app->max_entries = ipa3_ctx->max_ipv4_accel_conn;

	/* Initialize cache lock */
	mutex_init(&nat_app->cache_lock);

	size = (sizeof(nat_table_entry) * nat_app->max_entries);
	nat_app->cache = kzalloc(size, GFP_KERNEL);
	if(!nat_app->cache)
	{
		IPA_BE_ERR("Unable to allocate memory for cache\n");
		goto fail;
	}
	IPA_BE_DBG("Allocated %d bytes for config manager nat cache\n", size);

	/* Prevent double initialization of NatBase */
	if (NatBase) {
		IPA_BE_ERR("NatBase already initialized!\n");
		goto fail;
	}

	/* Initialize NatBase */
	NatBase = kzalloc(sizeof(*NatBase), GFP_KERNEL);
	if (!NatBase) {
		goto fail;
	}
	NatBase->ct_mem_type = "DDR";
	NatBase->m_maxEntries = ipa3_ctx->max_ipv6_accel_conn;

	/* Initialize IPv6 CT cache lock */
	mutex_init(&NatBase->cache_lock);

	size = (sizeof(struct NatEntryBase) * NatBase->m_maxEntries);
	NatBase->m_cache = kzalloc(size, GFP_KERNEL);
	if(!NatBase->m_cache)
	{
		IPA_BE_ERR("Unable to allocate memory for NatBase cache\n");
		goto fail;
	}
	IPA_BE_DBG("Allocated %d bytes for NatBase cache\n", size);

	/* Cumulative stats table sized to the HW counter range (1-based). */
	if (ipa3_ctx->hw_stats && ipa3_ctx->hw_stats->nat_ct.init.max_counters)
		ipa_be_cum_n = ipa3_ctx->hw_stats->nat_ct.init.max_counters + 1;
	else
		ipa_be_cum_n = 0;

	if (ipa_be_cum_n) {
		ipa_be_cum = kcalloc(ipa_be_cum_n, sizeof(*ipa_be_cum), GFP_KERNEL);
		if (!ipa_be_cum) {
			IPA_BE_ERR("Unable to allocate cumulative stats table\n");
			goto fail;
		}
		INIT_DELAYED_WORK(&ipa_be_stats_poll_work, ipa_be_stats_poll_work_fn);
	}

	g_ipa_ctx = ipa_ctx;

	mutex_unlock(&ipa_be_nat_mgmt_lock);
	return IPA_BE_SUCCESS;

fail:
	kfree(ipa_be_cum);
	ipa_be_cum = NULL;
	ipa_be_cum_n = 0;
	if (NatBase) {
		if (NatBase->m_cache) {
			kfree(NatBase->m_cache);
		}
		/* Destroy IPv6 CT cache lock if it was initialized */
		mutex_destroy(&NatBase->cache_lock);
		kfree(NatBase);
		NatBase = NULL;
	}
	if (nat_app) {
		if (nat_app->cache) {
			kfree(nat_app->cache);
		}
		/* Destroy cache lock if it was initialized */
		mutex_destroy(&nat_app->cache_lock);
		kfree(nat_app);
		nat_app = NULL;
	}
	mutex_unlock(&ipa_be_nat_mgmt_lock);
	return -ENOMEM;
}

uint32_t generate_met_data(uint8_t mux_id)
{
	return ((mux_id << HDR_METADATA_MUX_ID_SHFT) & HDR_METADATA_MUX_ID_BMASK) |
		HDR_METADATA_BE_FLOW;
}

/* Check for duplicate entries */
bool chk_for_dup(const nat_table_entry *rule)
{
	int cnt = 0;
	bool found = false;

	if (!rule || !nat_app) {
		IPA_BE_ERR("Invalid parameters or nat_app not initialized\n");
		return false;
	}

	IPA_BE_DBG("%s() %d\n", __FUNCTION__, __LINE__);

	mutex_lock(&nat_app->cache_lock);

	for(; cnt < nat_app->max_entries; cnt++)
	{
		if(nat_app->cache[cnt].private_ip == rule->private_ip &&
			nat_app->cache[cnt].target_ip == rule->target_ip &&
			nat_app->cache[cnt].private_port ==  rule->private_port  &&
			nat_app->cache[cnt].target_port == rule->target_port &&
			nat_app->cache[cnt].protocol == rule->protocol  &&
			nat_app->cache[cnt].dst_only == rule->dst_only  &&
			nat_app->cache[cnt].src_only == rule->src_only)
		{
			log_nat(rule->protocol,rule->private_ip,rule->target_ip,rule->private_port,\
			rule->public_port,rule->target_port, "Duplicate Rule\n");
			found = true;
			break;
		}
	}

	mutex_unlock(&nat_app->cache_lock);

	return found;
}

bool is_pwr_save_if(uint32_t ip_addr)
{
	int cnt;

	if (!nat_app) {
		IPA_BE_ERR("nat_app not initialized\n");
		return false;
	}

	for(cnt = 0; cnt < IPA_MAX_NUM_WIFI_CLIENTS; cnt++)
	{
		if(0 != nat_app->pwr_save_ifs[cnt] &&
			 ip_addr == nat_app->pwr_save_ifs[cnt])
		{
			return true;
		}
	}

	return false;
}

int ipa_be_add_pdn(uint32_t pub_ip, uint8_t mux_id, bool is_sta, bool ip_pass)
{
	int ret = 0;
	int cnt = 0;
	ipa_nat_ipv4_rule nat_rule;
	ipa_nat_pdn_entry entry;
	uint8_t pdn_index;
	uint8_t pdn_count = 0;
	IPA_BE_DBG("%s() %d\n", __FUNCTION__, __LINE__);

	memset(&entry, 0, sizeof(entry));
	entry.dst_metadata = 0;
	entry.src_metadata = generate_met_data(mux_id);
	entry.public_ip = pub_ip;
	entry.is_sta = is_sta;

	IPA_BE_DBG("Entry \n");
	/*
	 * Serialize the nat_table_hdl check-then-create/alloc against the other
	 * path that touches it (ipa_be_ensure_nat_table_ct).
	 */
	mutex_lock(&ipa_be_nat_mgmt_lock);
	ret = ipa_nat_get_pdn_count(&pdn_count);
	if(ret)
	{
		IPA_BE_ERR("unable to get pdn count Error:%d\n", ret);
		mutex_unlock(&ipa_be_nat_mgmt_lock);
		return ret;
	}
	IPA_BE_DBG("AddPDN isSta:%d, pdn_count: %d, ip 0x%X\n", is_sta,pdn_count,pub_ip);

	if (!nat_app->nat_table_hdl)
	{
		/*
		 * Create the table PDN-agnostic. ipa_nat_add_ipv4_tbl() writes the
		 * public IP it is given straight into pdns[0], bypassing the
		 * allocator, so pass 0: that seeds no slot and leaves every entry to
		 * ipa_nat_alloc_pdn() below, which places this PDN at the first free
		 * index and stamps its public IP and src metadata in one step. Same
		 * path for STA, modem and CT -- no slot is claimed out of band.
		 *
		 * The only thing the dropped argument fed was cmd.ip_addr in
		 * IPA_IOC_V4_INIT_NAT, the legacy pre-IPAv4 single-PDN field, which
		 * modem PDNs have always passed as 0.
		 */
		ret = ipa_nat_add_ipv4_tbl(0, nat_app->mem_type,
			nat_app->max_entries, &nat_app->nat_table_hdl);
		if(ret)
		{
			IPA_BE_ERR("unable to create nat table Error:%d\n", ret);
			ipa_rc_set_nat_init_status(true);
			mutex_unlock(&ipa_be_nat_mgmt_lock);
			return ret;
		}
		ipa_rc_set_nat_init_status(false);
		IPA_BE_DBG("succeesfully created NAT table for ip 0x%X\n", pub_ip);
	}

	/*
	 * Resolve this public IP's PDN slot, looking it up before trying to
	 * allocate. ipa_nat_alloc_pdn() would also return the index of an IP it
	 * already holds, but it rejects a full table before it scans for that
	 * match, so once all IPA_MAX_PDN_NUM slots are taken every connection on
	 * a PDN that is already up would start failing -- and this runs per
	 * connection, so the already-resolved case is the common path.
	 *
	 * On a miss, allocate: ipa_nat_alloc_pdn() hands out the first free slot --
	 * no index is reserved, so STA, modem and CT PDNs each take index 0 if they
	 * come up first -- and returns -EIO once every slot is used. Failing the
	 * connection is correct there -- reusing another PDN's slot would NAT it out
	 * of that PDN's public IP.
	 */
	if (!ipa_nat_get_pdn_index(pub_ip, &pdn_index))
	{
		IPA_BE_DBG("pdn already existed with index %d for ip 0x%X\n",
			pdn_index, pub_ip);
	}
	else
	{
		ret = ipa_nat_alloc_pdn(&entry, &pdn_index);
		if(ret)
		{
			IPA_BE_ERR("couldn't allocate a pdn index for ip 0x%X Error:%d\n",
				pub_ip, ret);
			mutex_unlock(&ipa_be_nat_mgmt_lock);
			return ret;
		}
		IPA_BE_DBG("allocated pdn index %d for ip 0x%X\n", pdn_index, pub_ip);
	}
	mutex_unlock(&ipa_be_nat_mgmt_lock);

	/* now traverse nat_app->cache and add the PDN entries */
	mutex_lock(&nat_app->cache_lock);

	for(cnt = 0; cnt < nat_app->max_entries; cnt++)
	{
		if((nat_app->cache[cnt].private_ip != 0)
			/* flush only entries which are related to this PDN;
			 * never CT entries (they belong to the dedicated CT PDN). */
			&& !nat_app->cache[cnt].conn_tracking
			&& (nat_app->cache[cnt].public_ip == pub_ip) && (nat_app->cache[cnt].enabled == false))
		{
			if(is_sta /*&& (isAlgPort(nat_app->cache[cnt].protocol, nat_app->cache[cnt].private_port) ||
				isAlgPort(nat_app->cache[cnt].protocol, nat_app->cache[cnt].target_port))*/) {
				IPA_BE_DBG("STA backhaul: connection using ALG Port, ignore\n");
				memset(&nat_app->cache[cnt], 0, sizeof(nat_app->cache[cnt]));
				nat_app->curCnt--;
				continue;
			}

			if (ip_pass && nat_app->cache[cnt].dummy_nat) {
				IPA_BE_DBG("IP Pass enabled: connection using dummy Nat, ignore\n");
				memset(&nat_app->cache[cnt], 0, sizeof(nat_app->cache[cnt]));
				nat_app->curCnt--;
				continue;
			}

			memset(&nat_rule, 0, sizeof(nat_rule));
			nat_rule.private_ip = nat_app->cache[cnt].private_ip;
			nat_rule.target_ip = nat_app->cache[cnt].target_ip;
			nat_rule.target_port = nat_app->cache[cnt].target_port;
			nat_rule.private_port = nat_app->cache[cnt].private_port;
			nat_rule.public_port = nat_app->cache[cnt].public_port;
			nat_rule.protocol = nat_app->cache[cnt].protocol;
			nat_rule.uc_activation_index = nat_app->cache[cnt].uc_activation_index;
			nat_rule.ucp = nat_app->cache[cnt].ucp;
			nat_rule.s = nat_app->cache[cnt].s;
			nat_rule.pdn_index = pdn_index;
			nat_app->cache[cnt].pdn_index = pdn_index;

			if(ipa_nat_add_ipv4_rule(nat_app->nat_table_hdl, &nat_rule, &nat_app->cache[cnt].rule_hdl) < 0)
			{
				IPA_BE_ERR("unable to add the rule delete from nat_app->cache\n");
				memset(&nat_app->cache[cnt], 0, sizeof(nat_app->cache[cnt]));
				nat_app->curCnt--;
				continue;
			}
			IPA_BE_DBG("nat_app->cache entry %d rule handle %d\n", cnt, nat_app->cache[cnt].rule_hdl);
			nat_app->cache[cnt].enabled = true;

			IPA_BE_DBG("new pdn added below rule successfully\n");
			iptodot("Private IP", nat_rule.private_ip);
			iptodot("Target IP", nat_rule.target_ip);
			IPA_BE_DBG("Private Port:%d \t Target Port: %d\n", nat_rule.private_port, nat_rule.target_port);
			IPA_BE_DBG("Public Port:%d\n", nat_rule.public_port);
			IPA_BE_DBG("protocol: %d\n", nat_rule.protocol);
			IPA_BE_DBG("pdn index: %d\n", nat_rule.pdn_index);
		}

	}

	mutex_unlock(&nat_app->cache_lock);

	IPA_BE_DBG("Exit\n");
	return ret;
}
EXPORT_SYMBOL(ipa_be_add_pdn);

int ipa_be_add_table_v6(const uint32_t v6_prefix[2])
{
	int ret = 0;
	IPA_BE_DBG("%s() %d\n", __FUNCTION__, __LINE__);
	uint32_t table_handle = 0;
	IPA_BE_DBG("Entry AddTable_v6\n");

	if(NatBase->ct_table_hdl)
	{
		IPA_BE_DBG("CT Table already exists\n");
		return 0;
	}
	/* For IPv6, we would typically use IPv6 connection tracking table */
	ret = ipa_ct_add_ipv6_tbl(NatBase->m_maxEntries, NatBase->ct_mem_type, &table_handle);
	if (ret)
	{
		IPA_BE_ERR("Error adding IPv6 CT table\n");
		return ret;
	}
	IPA_BE_DBG("Added table_handle = 0x%08x\n", table_handle);
	NatBase->ct_table_hdl = table_handle;

	IPA_BE_DBG("IPv6 prefix: 0x%08x%08x\n", v6_prefix[0], v6_prefix[1]);

	for (int cnt = 0; cnt < NatBase->m_maxEntries; ++cnt)
	{
		struct NatEntryBase entry = NatBase->m_cache[cnt];
		if (entry.m_enabled)
		{
			uint64_t src_ipv6_msb = 0;

			/*if(!(IPACM_Iface::ipacmcfg->ipv6_nat_enable))
			{
				if (entry.m_direction == NatEntryBase::DirectionOutbound ||
					entry.m_direction == NatEntryBase::DirectionUnknown ||
					entry.m_direction == NatEntryBase::DirectionInbound)
				{
					src_ipv6_msb =
						((Ipv6IpAddress &)entry.GetClientIp()).GetMsb();
				}
			}
			else*/
			{
				if (entry.m_direction == NAT_DIRECTION_OUTBOUND ||
					entry.m_direction == NAT_DIRECTION_UNKNOWN ||
					entry.m_direction == NAT_DIRECTION_INBOUND)
				{
					src_ipv6_msb = 0;
				}
			}
			/*if(ipv6prefixmatch(src_ipv6_msb, v6_prefix))
			{
				if (m_proxy.AddEntry(entry))
				{
					IPACMERR("unable to add the rule delete from cache\n");
					entry.Clear();
					--m_curCnt;
					continue;
				}
				entry.DebugDump("On wan-iface reset added below rule successfully\n");
			}*/
		}
	}

	/* TODO: Implement IPv6 connection tracking table creation */
	/* ret = ipa_ipv6ct_add_tbl(v6_prefix, nat_app->mem_type, nat_app->max_entries, &nat_app->ct_table_hdl); */

	IPA_BE_DBG("Exit AddTable_v6 ret=%d\n", ret);
	return ret;
}
EXPORT_SYMBOL(ipa_be_add_table_v6);

/* Fill rule's IP/port/protocol fields from v4_msg.
 * Handles downlink-IPPT, uplink-IPPT, downlink, and uplink cases.
 */
static void ipa_be_build_nat_rule_attrs(struct ipa_ipv4_rule_create_msg v4_msg,
					 nat_table_entry *rule)
{
	bool is_system_memory = ipa_get_hw_type() >= IPA_HW_v7_0;

	rule->protocol = v4_msg.tuple.protocol;
	rule->s = is_system_memory;

	if ((v4_msg.tuple.flow_ip == v4_msg.conn_rule.flow_ip_xlate) &&
	    (v4_msg.tuple.return_ip == v4_msg.conn_rule.return_ip_xlate)) {
		/* IPPT case */
		if (v4_msg.conn_rule.flow_interface_num ==
		    v4_msg.conn_rule.flow_top_interface_num) {
			/* Downlink IPPT */
			rule->private_ip   = ntohl(v4_msg.conn_rule.return_ip_xlate);
			rule->target_ip    = ntohl(v4_msg.tuple.flow_ip);
			rule->public_ip    = ntohl(v4_msg.tuple.return_ip);
			rule->private_port = ntohs(v4_msg.tuple.return_ident);
			rule->target_port  = ntohs(v4_msg.tuple.flow_ident);
			rule->public_port  = ntohs(v4_msg.conn_rule.return_ident_xlate);
		} else {
			/* Uplink IPPT */
			rule->private_ip   = ntohl(v4_msg.tuple.flow_ip);
			rule->target_ip    = ntohl(v4_msg.tuple.return_ip);
			rule->public_ip    = ntohl(v4_msg.conn_rule.flow_ip_xlate);
			rule->private_port = ntohs(v4_msg.tuple.flow_ident);
			rule->target_port  = ntohs(v4_msg.tuple.return_ident);
			rule->public_port  = ntohs(v4_msg.conn_rule.flow_ident_xlate);
		}
	} else if (v4_msg.tuple.flow_ip == v4_msg.conn_rule.flow_ip_xlate) {
		/* Downlink */
		rule->private_ip   = ntohl(v4_msg.conn_rule.return_ip_xlate);
		rule->target_ip    = ntohl(v4_msg.tuple.flow_ip);
		rule->public_ip    = ntohl(v4_msg.tuple.return_ip);
		rule->private_port = ntohs(v4_msg.tuple.return_ident);
		rule->target_port  = ntohs(v4_msg.tuple.flow_ident);
		rule->public_port  = ntohs(v4_msg.conn_rule.return_ident_xlate);
	} else {
		/* Uplink */
		rule->private_ip   = ntohl(v4_msg.tuple.flow_ip);
		rule->target_ip    = ntohl(v4_msg.tuple.return_ip);
		rule->public_ip    = ntohl(v4_msg.conn_rule.flow_ip_xlate);
		rule->private_port = ntohs(v4_msg.tuple.flow_ident);
		rule->target_port  = ntohs(v4_msg.tuple.return_ident);
		rule->public_port  = ntohs(v4_msg.conn_rule.flow_ident_xlate);
	}
}

int ipa_be_addpdn(struct ipa_ipv4_rule_create_msg v4_msg, int pdn_iface)
{
	uint8_t mux_id = 0;
	bool is_sta = 0;
	bool ip_pass = 0;
	bool isVlan = 0;
	uint32_t pub_ip = 0;
	struct ipa_ioc_query_intf_ext_props *ext_prop = NULL;
	struct ipa_ioc_query_intf pdn_intf;
	int ret = 0;

	IPA_BE_DBG("Entry \n");

	memset(&pdn_intf, 0, sizeof(pdn_intf));
	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(pdn_iface, &pdn_intf)) {
		IPA_BE_ERR("Unable to query iface prop\n");
		return -EINVAL;
	} else {
		IPA_BE_DBG("Interface with index %u exist.\n", pdn_iface);
	}

	/*
	 * Modem PDNs expose ext_props (mux_id, is_sta, etc) over QMI. Eth backhaul
	 * (eth) PDNs have num_ext_props == 0 — keep the defaults (mux_id=0,
	 * is_sta=false, ip_pass=false) and skip the ext_props query.
	 */
	if (pdn_intf.num_ext_props > 0) {
		ext_prop = (struct ipa_ioc_query_intf_ext_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_ext_props) +
			pdn_intf.num_ext_props * sizeof(struct ipa_ioc_ext_intf_prop), GFP_KERNEL);
		if (ext_prop == NULL) {
			IPA_BE_ERR("Unable to allocate ext_prop memory.\n");
			return -ENOMEM;
		}

		memcpy(ext_prop->name, pdn_intf.name, sizeof(pdn_intf.name));
		ext_prop->num_ext_props = pdn_intf.num_ext_props;
		IPA_BE_DBG("Query ext_prop %d name %s\n", ext_prop->num_ext_props, ext_prop->name);
		if (ipa3_query_intf_ext_props(ext_prop)) {
			IPA_BE_ERR("Failed to query ext_props for iface %s\n", ext_prop->name);
			kfree(ext_prop);
			return -EIO;
		}

		mux_id = ext_prop->ext[0].mux_id;
		IPA_BE_DBG("Query iface %s mux_id %d\n", ext_prop->name, mux_id);
	} else {
		enum ipa_backhaul_type bh_type;
		/*
		 * num_ext_props == 0 on a PDN we previously classified as modem
		 * indicates an inconsistency — likely a stale cache entry or a
		 * QMI misreport. We silently fall back to mux_id=0/is_sta=false;
		 * surface the case so it doesn't masquerade as a successful modem add.
		 */
		if (ipa_be_detect_backhaul_type(pdn_iface, &bh_type) == 0 &&
		    bh_type == IPA_BACKHAUL_TYPE_MODEM)
			WARN_ONCE(1, "ipa_be_addpdn: PDN %s (iface %d) classified MODEM but reports zero ext_props; using defaults\n",
				pdn_intf.name, pdn_iface);
		IPA_BE_DBG("Eth backhaul PDN %s: skipping ext_props query, using mux_id=0\n",
			pdn_intf.name);
	}

	/* IP Passthrough case */
	if (v4_msg.tuple.flow_ip == v4_msg.conn_rule.flow_ip_xlate &&
		v4_msg.tuple.return_ip == v4_msg.conn_rule.return_ip_xlate)
	{
		if(v4_msg.conn_rule.flow_interface_num == v4_msg.conn_rule.flow_top_interface_num)
		{
			pub_ip = (uint32_t)ntohl(v4_msg.tuple.return_ip);
		}
		else if (v4_msg.conn_rule.return_interface_num == v4_msg.conn_rule.return_top_interface_num)
		{
			pub_ip = (uint32_t)ntohl(v4_msg.tuple.flow_ip);
		}
		else
		{
			/* IPPT but no orientation matched: pub_ip unresolved.
			 * Fail rather than install a bogus 0.0.0.0 PDN.
			 */
			IPA_BE_ERR("IPPT topology unresolved (flow_if=%u/%u return_if=%u/%u); abort PDN add\n",
				v4_msg.conn_rule.flow_interface_num,
				v4_msg.conn_rule.flow_top_interface_num,
				v4_msg.conn_rule.return_interface_num,
				v4_msg.conn_rule.return_top_interface_num);
			ret = -EINVAL;
			goto cleanup;
		}
	}
	else if(v4_msg.tuple.flow_ip == v4_msg.conn_rule.flow_ip_xlate)
	{
		pub_ip = (uint32_t)ntohl(v4_msg.tuple.return_ip);
	}
	else
	{
		pub_ip = (uint32_t)ntohl(v4_msg.conn_rule.flow_ip_xlate);
	}
	IPA_BE_DBG("pub_ip %pI4 \n", &pub_ip);

	/*
	 * One allocator for both VLAN and non-VLAN PDNs. The VLAN arm was already
	 * correct; the non-VLAN arm used to seed the first modem PDN into slot 0
	 * directly at table creation and skip the cache replay for a known public
	 * IP. Every PDN now goes through ipa_nat_alloc_pdn().
	 */
	ret = ipa_be_add_pdn(pub_ip, mux_id, is_sta, ip_pass);
	if (ret)
	{
		IPA_BE_ERR("failed adding pdn\n");
		goto cleanup;
	}

	ret = ipa_be_add_entry(v4_msg, isVlan, false, false);
	if (ret)
	{
		IPA_BE_ERR("failed adding nat entry\n");
		goto cleanup;
	}

cleanup:
	/* Clean up allocated memory */
	kfree(ext_prop);
	IPA_BE_DBG("Exit ret=%d\n", ret);
	return ret;
}

static int ipv6ct_convert_to_ipa_rule(struct ipa_ipv6_rule_create_msg *v6_msg,
				       ipa_ipv6ct_rule *ipa_rule)
{
	struct ipa_ipv6_5tuple *tuple;
	bool is_system_memory = ipa_get_hw_type() >= IPA_HW_v7_0;

	if (!v6_msg || !ipa_rule) {
		IPA_BE_ERR("ipv6ct_convert_to_ipa_rule: NULL input (v6_msg=%p, ipa_rule=%p)\n",
			   v6_msg, ipa_rule);
		return -EINVAL;
	}

	tuple = &v6_msg->tuple;
	memset(ipa_rule, 0, sizeof(ipa_ipv6ct_rule));

	/* Convert IPv6 addresses */
	if (v6_msg->conn_rule.return_interface_num == v6_msg->conn_rule.return_top_interface_num) {
		/*Uplink*/
		ipa_rule->src_ipv6_msb = ((__u64)ntohl(tuple->flow_ip[0]) << 32) |
					  ntohl(tuple->flow_ip[1]);
		ipa_rule->src_ipv6_lsb = ((__u64)ntohl(tuple->flow_ip[2]) << 32) |
					  ntohl(tuple->flow_ip[3]);

		ipa_rule->dest_ipv6_msb = ((__u64)ntohl(tuple->return_ip[0]) << 32) |
					   ntohl(tuple->return_ip[1]);
		ipa_rule->dest_ipv6_lsb = ((__u64)ntohl(tuple->return_ip[2]) << 32) |
					   ntohl(tuple->return_ip[3]);

		/* Ports and protocol */
		ipa_rule->src_port  = ntohs(tuple->flow_ident);
		ipa_rule->dest_port = ntohs(tuple->return_ident);
		ipa_rule->protocol  = tuple->protocol;
	} else {
		/*Downlink*/
		ipa_rule->src_ipv6_msb = ((__u64)ntohl(tuple->return_ip[0]) << 32) |
					  ntohl(tuple->return_ip[1]);
		ipa_rule->src_ipv6_lsb = ((__u64)ntohl(tuple->return_ip[2]) << 32) |
					  ntohl(tuple->return_ip[3]);

		ipa_rule->dest_ipv6_msb = ((__u64)ntohl(tuple->flow_ip[0]) << 32) |
					   ntohl(tuple->flow_ip[1]);
		ipa_rule->dest_ipv6_lsb = ((__u64)ntohl(tuple->flow_ip[2]) << 32) |
					   ntohl(tuple->flow_ip[3]);

		/* Ports and protocol */
		ipa_rule->src_port  = ntohs(tuple->return_ident);
		ipa_rule->dest_port = ntohs(tuple->flow_ident);
		ipa_rule->protocol  = tuple->protocol;
	}

	ipa_rule->direction_settings = IPA_IPV6CT_DIRECTION_ALLOW_ALL;
	ipa_rule->ucp = false;
	ipa_rule->uc_activation_index = 0;
	ipa_rule->s = is_system_memory;

	return 0;
}

/**
 * ipv6ct_add_entry() - Add a v1 IPv6 CT entry (IPA < v7.0 only)
 * @v6_msg:      IPv6 rule create message
 * @rule_handle: Output handle for the added rule
 *
 * Not supported on IPA >= v7.0; use ipa_be_add_v6_ct_entry() instead.
 */
int ipv6ct_add_entry(struct ipa_ipv6_rule_create_msg *v6_msg,
             uint32_t *rule_handle)
{
    ipa_ipv6ct_rule ipa_rule;
    struct ipa_ipv6_5tuple *tuple;
    int ret;

    if (!v6_msg || !rule_handle) {
        IPA_BE_ERR("Invalid parameters (v6_msg=%p, rule_handle=%p)\n",
               v6_msg, rule_handle);
        return -EINVAL;
    }

    if (!NatBase || !NatBase->ct_table_hdl) {
        IPA_BE_ERR("NatBase or NatBase->ct_table_hdl is invalid (0)\n");
        return -ENODEV;
    }

    if (ipa_get_hw_type() >= IPA_HW_v7_0) {
        WARN_ONCE(1, "ipv6ct_add_entry: v1 API is not supported on IPA >= v7.0; use ipa_be_add_v6_ct_entry()\n");
        *rule_handle = 0;
        return -EOPNOTSUPP;
    }

    tuple = &v6_msg->tuple;

    ret = ipv6ct_convert_to_ipa_rule(v6_msg, &ipa_rule);
    if (ret) {
        IPA_BE_ERR("Failed to convert rule format: %d\n", ret);
        return ret;
    }

    ret = ipa_ct_add_ipv6_rule(NatBase->ct_table_hdl, &ipa_rule, rule_handle);
    if (ret) {
        IPA_BE_DBG("Failed to add IPv6 CT rule to IPA: %d\n", ret);
        return ret;
    }

    IPA_BE_DBG("Successfully added IPv6 CT entry: handle=%u, protocol=%u\n",
           *rule_handle, tuple->protocol);
    IPA_BE_DBG("Flow: %pI6:%u -> Return: %pI6:%u\n",
           tuple->flow_ip, ntohs(tuple->flow_ident),
           tuple->return_ip, ntohs(tuple->return_ident));

    return 0;
}
EXPORT_SYMBOL(ipv6ct_add_entry);

/**
 * ipv6ct_add_entry_v2() - Add a bidirectional IPv6 CT entry (IPA v7.0+)
 * @v6_msg:       IPv6 rule create message
 * @rule_handle:  Output handle for the added rule
 * @swap_src_dest: When false, use flow_ip as src (flow is canonical private side);
 *                 when true, swap src/dest so return_ip becomes src
 *
 * Uses IPA_IPV6CT_DIRECTION_ALLOW_ALL so the single canonical entry
 * covers both inbound and outbound traffic for the connection (LAN2LAN
 * pair or LAN2WAN flow). Also populates the rule's SW producer
 * classification cookie from flow/return interface vpnum, when applicable.
 */
static int ipv6ct_add_entry_v2(struct ipa_ipv6_rule_create_msg *v6_msg,
			       uint32_t *rule_handle,
			       uint16_t *all_pkts_counter,
			       uint16_t *non_frag_counter,
			       bool swap_src_dest)
{
    ipa_ipv6ct_rule_v2 ipa_rule_v2;
    struct ipa_ipv6_5tuple *tuple;
    struct ipa_sw_producer_cookie cookie;
    bool is_system_memory = ipa_get_hw_type() >= IPA_HW_v7_0;
    int ret;

    if (!v6_msg || !rule_handle) {
        IPA_BE_ERR("Invalid parameters (v6_msg=%p, rule_handle=%p)\n",
               v6_msg, rule_handle);
        return -EINVAL;
    }

    if (!NatBase || !NatBase->ct_table_hdl) {
        IPA_BE_ERR("NatBase or NatBase->ct_table_hdl is invalid (0)\n");
        return -ENODEV;
    }

    tuple = &v6_msg->tuple;

    memset(&ipa_rule_v2, 0, sizeof(ipa_rule_v2));

    if (!swap_src_dest) {
        /* flow_ip as src: flow is canonical private side */
        ipa_rule_v2.src_ipv6_msb = ((__u64)ntohl(tuple->flow_ip[0]) << 32) |
                                    ntohl(tuple->flow_ip[1]);
        ipa_rule_v2.src_ipv6_lsb = ((__u64)ntohl(tuple->flow_ip[2]) << 32) |
                                    ntohl(tuple->flow_ip[3]);
        ipa_rule_v2.dest_ipv6_msb = ((__u64)ntohl(tuple->return_ip[0]) << 32) |
                                     ntohl(tuple->return_ip[1]);
        ipa_rule_v2.dest_ipv6_lsb = ((__u64)ntohl(tuple->return_ip[2]) << 32) |
                                     ntohl(tuple->return_ip[3]);
        ipa_rule_v2.src_port  = ntohs(tuple->flow_ident);
        ipa_rule_v2.dest_port = ntohs(tuple->return_ident);
    } else {
        /* return_ip as src: return is canonical private side */
        ipa_rule_v2.src_ipv6_msb = ((__u64)ntohl(tuple->return_ip[0]) << 32) |
                                    ntohl(tuple->return_ip[1]);
        ipa_rule_v2.src_ipv6_lsb = ((__u64)ntohl(tuple->return_ip[2]) << 32) |
                                    ntohl(tuple->return_ip[3]);
        ipa_rule_v2.dest_ipv6_msb = ((__u64)ntohl(tuple->flow_ip[0]) << 32) |
                                     ntohl(tuple->flow_ip[1]);
        ipa_rule_v2.dest_ipv6_lsb = ((__u64)ntohl(tuple->flow_ip[2]) << 32) |
                                     ntohl(tuple->flow_ip[3]);
        ipa_rule_v2.src_port  = ntohs(tuple->return_ident);
        ipa_rule_v2.dest_port = ntohs(tuple->flow_ident);
    }

    ipa_rule_v2.protocol            = tuple->protocol;
    ipa_rule_v2.direction_settings  = IPA_IPV6CT_DIRECTION_ALLOW_ALL;
    ipa_rule_v2.ucp                 = false;
    ipa_rule_v2.uc_activation_index = 0;
    ipa_rule_v2.s                   = is_system_memory;
    ipa_rule_v2.all_pkts_stats_cnt_index = 0;
    ipa_rule_v2.non_frag_stats_cnt_index = 0;

    if (all_pkts_counter)
        *all_pkts_counter = 0;
    if (non_frag_counter)
        *non_frag_counter = 0;

    if (all_pkts_counter) {
        ret = ipa_ct_alloc_counter_v6(
            NatBase->ct_table_hdl,
            ipa_rule_v2.src_ipv6_lsb,
            ipa_rule_v2.src_ipv6_msb,
            true,
            &ipa_rule_v2.all_pkts_stats_cnt_index);
        if (ret) {
            IPA_BE_ERR("Failed to allocate all_pkts counter: %d\n", ret);
            ipa_rule_v2.all_pkts_stats_cnt_index = 0;
        }
    }

    if (non_frag_counter) {
        ret = ipa_ct_alloc_counter_v6(
            NatBase->ct_table_hdl,
            ipa_rule_v2.src_ipv6_lsb,
            ipa_rule_v2.src_ipv6_msb,
            false,
            &ipa_rule_v2.non_frag_stats_cnt_index);
        if (ret) {
            IPA_BE_ERR("Failed to allocate non_frag counter: %d\n", ret);
            ipa_rule_v2.non_frag_stats_cnt_index = 0;
        }
    }

    IPA_BE_DBG("Counters: all_pkts=%u%s non_frag=%u%s\n",
           ipa_rule_v2.all_pkts_stats_cnt_index,
           all_pkts_counter ? "" : " (skipped)",
           ipa_rule_v2.non_frag_stats_cnt_index,
           non_frag_counter ? "" : " (skipped)");

    /* Install SW producer cookie if src or dst interface is wlan */
    memset(&cookie, 0, sizeof(cookie));
    ipa3_populate_cookie_vpnum(v6_msg->conn_rule.flow_interface_num, &cookie);
    if (!cookie.raw)
        ipa3_populate_cookie_vpnum(v6_msg->conn_rule.return_interface_num, &cookie);
    ipa_rule_v2.sw_prod_classification_cookie = cookie.raw;
    IPA_BE_DBG("IPv6CT sw_prod_cookie=0x%llx flow_intf=%d ret_intf=%d\n",
        ipa_rule_v2.sw_prod_classification_cookie,
        v6_msg->conn_rule.flow_interface_num,
        v6_msg->conn_rule.return_interface_num);

    ret = ipa_ct_add_ipv6_rule_v2(NatBase->ct_table_hdl, &ipa_rule_v2, rule_handle);
    if (ret) {
        IPA_BE_ERR("Failed to add IPv6 CT v2 rule to IPA: %d\n", ret);
        ipa_be_free_ct_counters_v6(
            NatBase->ct_table_hdl,
            ipa_rule_v2.src_ipv6_lsb,
            ipa_rule_v2.src_ipv6_msb,
            ipa_rule_v2.all_pkts_stats_cnt_index,
            ipa_rule_v2.non_frag_stats_cnt_index);
        return ret;
    }

    if (all_pkts_counter)
        *all_pkts_counter = ipa_rule_v2.all_pkts_stats_cnt_index;
    if (non_frag_counter)
        *non_frag_counter = ipa_rule_v2.non_frag_stats_cnt_index;

    IPA_BE_DBG("Successfully added IPv6 CT v2 entry: handle=%u, protocol=%u, swap=%d\n",
           *rule_handle, tuple->protocol, swap_src_dest);

    return 0;
}

int ipv6ct_del_entry(struct ipa_ipv6_rule_destroy_msg *v6_msg, uint32_t rule_handle)
{
    int ret;

    if (!v6_msg) {
        IPA_BE_ERR("Invalid parameters (v6_msg=%p)\n", v6_msg);
        return -EINVAL;
    }

    if (rule_handle == IPA_TABLE_INVALID_ENTRY) {
        IPA_BE_ERR("Invalid rule handle: %u\n", rule_handle);
        return -EINVAL;
    }

    if (!NatBase || !NatBase->ct_table_hdl) {
        IPA_BE_ERR("NatBase or NatBase->ct_table_hdl is invalid (0)\n");
        return -ENODEV;
    }

    ret = ipa_ct_del_ipv6_rule(NatBase->ct_table_hdl, rule_handle);
    if (ret) {
        IPA_BE_ERR("Failed to delete IPv6 CT rule from IPA: %d (handle=%u)\n",
                   ret, rule_handle);
        return ret;
    }

    IPA_BE_DBG("Successfully deleted IPv6 CT entry: handle=%u, protocol=%u\n",
               rule_handle, v6_msg->tuple.protocol);
    IPA_BE_DBG("Flow: %pI6:%u -> Return: %pI6:%u\n",
               v6_msg->tuple.flow_ip, ntohs(v6_msg->tuple.flow_ident),
               v6_msg->tuple.return_ip, ntohs(v6_msg->tuple.return_ident));

    return 0;
}
EXPORT_SYMBOL(ipv6ct_del_entry);

/*
 * LAN2LAN v7.0+ canonical key: smaller-EP endpoint is src, regardless of which
 * orientation (uplink- or downlink-triggered) produced this call. Returns
 * -EINVAL on EP query failure, 0 otherwise with *swap_src_dest set.
 */
static int ipa_be_ipv6ct_canonical_swap(s32 flow_intf, s32 return_intf,
					 const u8 *flow_mac, const u8 *return_mac,
					 bool *swap_src_dest)
{
	int ep_cmp = ipa_be_flow_canonical_cmp(flow_intf, return_intf, flow_mac, return_mac);

	if (ep_cmp < 0)
		return -EINVAL;

	*swap_src_dest = !ep_cmp;
	return 0;
}

static void ipv6_ct_populate_topology(struct ipv6_ct_entry *cache,
				      const struct ipa_ipv6_rule_create_msg *v6_msg,
				      int pdn_iface)
{
	cache->pdn_iface        = pdn_iface;
	cache->flow_iface       = v6_msg->conn_rule.flow_interface_num;
	cache->return_iface     = v6_msg->conn_rule.return_interface_num;
	cache->flow_top_iface   = v6_msg->conn_rule.flow_top_interface_num;
	cache->return_top_iface = v6_msg->conn_rule.return_top_interface_num;
	memcpy(cache->flow_mac,   v6_msg->conn_rule.flow_mac,   6);
	memcpy(cache->return_mac, v6_msg->conn_rule.return_mac, 6);
	memcpy(cache->orig_flow_ip,   v6_msg->tuple.flow_ip,
	       sizeof(cache->orig_flow_ip));
	memcpy(cache->orig_return_ip, v6_msg->tuple.return_ip,
	       sizeof(cache->orig_return_ip));
	cache->orig_flow_ident   = v6_msg->tuple.flow_ident;
	cache->orig_return_ident = v6_msg->tuple.return_ident;
}

int ipa_be_add_v6_ct_entry(struct ipa_ipv6_rule_create_msg v6_msg, int pdn_iface, bool lan2lan)
{
	uint8_t mux_id = 0;
	int ret = 0;
	uint32_t v6_prefix[2];
	struct ipa_ioc_query_intf_ext_props *ext_prop = NULL;
	struct ipa_ioc_query_intf pdn_intf;
	uint32_t rule_hdl = 0;
	uint32_t rule_hdl_v2 = 0;
	struct ipa_ipv6_5tuple *tuple = &v6_msg.tuple;
	uint64_t src_ipv6_msb, src_ipv6_lsb;
	uint64_t dest_ipv6_msb, dest_ipv6_lsb;
	uint16_t src_port, dest_port;
	/* Canonical (orientation-invariant) key, used for dup check, add, and cleanup. */
	bool     swap_src_dest = false;
	uint64_t can_src_msb = 0, can_src_lsb = 0;
	uint64_t can_dst_msb = 0, can_dst_lsb = 0;
	uint16_t can_src_port = 0, can_dst_port = 0;
	int cache_idx;
	int existing_idx;
	uint16_t all_pkts_counter = 0, non_frag_counter = 0;

	IPA_BE_DBG("Entry ipa_be_add_v6_ct_entry\n");

	/* ext Props */
	memset(&pdn_intf, 0, sizeof(pdn_intf));
	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(pdn_iface, &pdn_intf)) {
		IPA_BE_ERR("Unable to query iface prop\n");
		ret = -EINVAL;
		goto cleanup;
	} else {
		IPA_BE_DBG("Interface with index %u exist.\n", pdn_iface);
	}

	ext_prop = (struct ipa_ioc_query_intf_ext_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_ext_props) +
		pdn_intf.num_ext_props * sizeof(struct ipa_ioc_ext_intf_prop), GFP_KERNEL);
	if (ext_prop == NULL) {
		IPA_BE_ERR("Unable to allocate ext_prop memory.\n");
		ret = -ENOMEM;
		goto cleanup;
	}

	memcpy(ext_prop->name, pdn_intf.name, sizeof(pdn_intf.name));
	ext_prop->num_ext_props = pdn_intf.num_ext_props;
	IPA_BE_DBG("Query ext_prop %d name %s\n", ext_prop->num_ext_props, ext_prop->name);
	ipa3_query_intf_ext_props(ext_prop);

	mux_id = ext_prop->ext[0].mux_id;
	IPA_BE_DBG("Query iface %s mux_id %d\n", ext_prop->name, mux_id);

	IPA_BE_DBG("flow_ip_xlate: %08x:%08x:%08x:%08x return_ip:%08x:%08x:%08x:%08x\n",
		ntohl(v6_msg.conn_rule.flow_ip_xlate[0]),  ntohl(v6_msg.conn_rule.flow_ip_xlate[1]),
		ntohl(v6_msg.conn_rule.flow_ip_xlate[2]),  ntohl(v6_msg.conn_rule.flow_ip_xlate[3]),
		ntohl(v6_msg.tuple.return_ip[0]), ntohl(v6_msg.tuple.return_ip[1]),
		ntohl(v6_msg.tuple.return_ip[2]), ntohl(v6_msg.tuple.return_ip[3]));

	IPA_BE_DBG("flow_ip: %08x:%08x:%08x:%08x return_ip:%08x:%08x:%08x:%08x\n",
		ntohl(v6_msg.tuple.flow_ip[0]),  ntohl(v6_msg.tuple.flow_ip[1]),
		ntohl(v6_msg.tuple.flow_ip[2]),  ntohl(v6_msg.tuple.flow_ip[3]),
		ntohl(v6_msg.conn_rule.return_ip_xlate[0]), ntohl(v6_msg.conn_rule.return_ip_xlate[1]),
		ntohl(v6_msg.conn_rule.return_ip_xlate[2]), ntohl(v6_msg.conn_rule.return_ip_xlate[3]));

	/* Extract IPv6 prefix - use the first 64 bits (2 x 32-bit words) for prefix */
	/* Apply ntohl to convert from network to host byte order for hardware */
	if (memcmp(v6_msg.tuple.flow_ip, v6_msg.conn_rule.flow_ip_xlate, sizeof(v6_msg.tuple.flow_ip)) == 0) {
		/* Downlink case - use return IP prefix (first 64 bits) */
		v6_prefix[0] = ntohl(v6_msg.tuple.return_ip[0]);
		v6_prefix[1] = ntohl(v6_msg.tuple.return_ip[1]);
	} else {
		/* Uplink case - use flow IP xlate prefix (first 64 bits) */
		v6_prefix[0] = ntohl(v6_msg.conn_rule.flow_ip_xlate[0]);
		v6_prefix[1] = ntohl(v6_msg.conn_rule.flow_ip_xlate[1]);
	}

	IPA_BE_DBG("IPv6 prefix: %08x:%08x\n", v6_prefix[0], v6_prefix[1]);

	/* Check VLAN configuration and call appropriate function */
	IPA_BE_DBG("No VLAN, using table approach\n");
	ret = ipa_be_add_table_v6(v6_prefix);
	if (ret) {
		IPA_BE_ERR("failed adding IPv6 CT table\n .. exiting..\n");
		goto cleanup;
	}

	/* Compute raw IPv6 64-bit tuples from network byte order. */
	src_ipv6_msb = ((__u64)ntohl(tuple->flow_ip[0]) << 32) | ntohl(tuple->flow_ip[1]);
	src_ipv6_lsb = ((__u64)ntohl(tuple->flow_ip[2]) << 32) | ntohl(tuple->flow_ip[3]);
	dest_ipv6_msb = ((__u64)ntohl(tuple->return_ip[0]) << 32) | ntohl(tuple->return_ip[1]);
	dest_ipv6_lsb = ((__u64)ntohl(tuple->return_ip[2]) << 32) | ntohl(tuple->return_ip[3]);
	src_port = ntohs(tuple->flow_ident);
	dest_port = ntohs(tuple->return_ident);

	if (ipa_get_hw_type() >= IPA_HW_v7_0) {
		if (lan2lan) {
			ret = ipa_be_ipv6ct_canonical_swap(v6_msg.conn_rule.flow_interface_num,
							    v6_msg.conn_rule.return_interface_num,
							    v6_msg.conn_rule.flow_mac,
							    v6_msg.conn_rule.return_mac,
							    &swap_src_dest);
			if (ret)
				goto cleanup;
		} else {
			/*
			 * LAN2WAN: Uplink (return side is the top/WAN interface)
			 * keeps flow_ip as src; Downlink (return side is not the
			 * top interface) swaps so return_ip (the LAN side) becomes
			 * src. Mirrors the pre-v7.0 ipv6ct_convert_to_ipa_rule()
			 * orientation so src/dest ordering stays version-independent.
			 */
			swap_src_dest = (v6_msg.conn_rule.return_interface_num !=
					 v6_msg.conn_rule.return_top_interface_num);
		}
	}

	if (swap_src_dest) {
		can_src_msb = dest_ipv6_msb; can_src_lsb = dest_ipv6_lsb; can_src_port = dest_port;
		can_dst_msb = src_ipv6_msb;  can_dst_lsb = src_ipv6_lsb;  can_dst_port = src_port;
	} else {
		can_src_msb = src_ipv6_msb;  can_src_lsb = src_ipv6_lsb;  can_src_port = src_port;
		can_dst_msb = dest_ipv6_msb; can_dst_lsb = dest_ipv6_lsb; can_dst_port = dest_port;
	}

	existing_idx = find_ipv6_ct_entry(can_src_msb, can_src_lsb, can_dst_msb, can_dst_lsb,
					  can_src_port, can_dst_port, tuple->protocol);
	if (existing_idx >= 0) {
		IPA_BE_DBG("Duplicate IPv6 CT entry at cache idx %d, skipping\n", existing_idx);
		ret = 0;
		goto cleanup;
	}

	/* Add IPv6 connection tracking entry */
	if (ipa_get_hw_type() >= IPA_HW_v7_0) {
		/*
		 * Use the same swap_src_dest computed above for the canonical
		 * key, so the installed HW entry matches can_src/can_dst
		 * regardless of which orientation triggered this call.
		 */
		ret = ipv6ct_add_entry_v2(&v6_msg, &rule_hdl_v2,
					  &all_pkts_counter, NULL,
					  swap_src_dest);
		if (ret) {
			IPA_BE_ERR("Failed to add IPv6 CT entry\n");
			ret = -EFAULT;
			goto cleanup; /* entry not created; counters freed by ipv6ct_add_entry_v2 */
		}

		IPA_BE_DBG("IPv6 CT entry added with hdl %d\n", rule_hdl_v2);
		cache_idx = store_ipv6_ct_entry(can_src_msb, can_src_lsb,
						can_dst_msb, can_dst_lsb,
						can_src_port, can_dst_port,
						tuple->protocol, rule_hdl_v2,
						tuple->flow_rule_id,
						all_pkts_counter, non_frag_counter);
		if (cache_idx < 0) {
			IPA_BE_ERR("Failed to store IPv6 CT entry in cache\n");
			ret = -EFAULT;
			goto cleanup_fwd_rule;
		}
		ipa_be_cum_assign(all_pkts_counter);
		ipv6_ct_populate_topology(&NatBase->m_cache[cache_idx].ipv6_ct,
					  &v6_msg, pdn_iface);
	} else {
		/* Legacy path (< IPA v7.0): single ALLOW_ALL entry, no counter allocation */
		ret = ipv6ct_add_entry(&v6_msg, &rule_hdl);
		if (ret) {
			IPA_BE_ERR("Failed to add IPv6 CT entry\n");
			goto cleanup;
		}

		IPA_BE_DBG("IPv6 CT entry added with hdl %d\n", rule_hdl);

		cache_idx = store_ipv6_ct_entry(src_ipv6_msb, src_ipv6_lsb,
						dest_ipv6_msb, dest_ipv6_lsb,
						src_port, dest_port,
						tuple->protocol, rule_hdl,
						tuple->flow_rule_id,
						0, 0);
		if (cache_idx < 0) {
			IPA_BE_ERR("Failed to store IPv6 CT entry in cache\n");
			ret = -EFAULT;
			goto cleanup_single_rule;
		}
		ipv6_ct_populate_topology(&NatBase->m_cache[cache_idx].ipv6_ct,
					  &v6_msg, pdn_iface);
	}

	goto cleanup;

cleanup_single_rule:
	if (ipv6ct_del_entry((struct ipa_ipv6_rule_destroy_msg *)&v6_msg, rule_hdl))
		IPA_BE_ERR("Failed to cleanup IPv6 CT rule hdl %u\n", rule_hdl);
	goto cleanup;

cleanup_fwd_rule:
	ipa_be_free_ct_counters_v6(NatBase->ct_table_hdl,
				   can_src_lsb, can_src_msb,
				   all_pkts_counter, non_frag_counter);
	if (ipv6ct_del_entry((struct ipa_ipv6_rule_destroy_msg *)&v6_msg, rule_hdl_v2))
		IPA_BE_ERR("Failed to cleanup IPv6 CT rule hdl %u\n", rule_hdl_v2);
cleanup:
	kfree(ext_prop);
	IPA_BE_DBG("Exit ipa_be_add_v6_ct_entry\n");
	return ret;
}

/*
 * ipa_be_ensure_ct_pdn() - Idempotently allocate the dedicated PDN entry that
 * every IPv4 CT (LAN2LAN) entry points at.
 *
 * On IPA v7.0 the NAT hardware validates PDN[pdn_index] before accepting a
 * match, so a CT entry needs a live PDN even though conn_tracking=1 suppresses
 * translation. Borrowing a WAN's PDN is not safe: CT would either inherit that
 * WAN's src_metadata (breaking CT while the WAN is up) or find an all-zero entry
 * once ipa_be_remove_pdn() deallocates it (breaking CT when the WAN goes down).
 *
 * Instead CT owns a PDN of its own, keyed on IPA_BE_CT_PDN_PLACEHOLDER_IP:
 *   - Allocation is keyed on public_ip, so it is idempotent and no WAN (whose
 *     public IP is never 192.0.2.1) can collide with, overwrite or deallocate it.
 *     That keying is the whole guarantee: ipa_nat_alloc_pdn() reserves no index
 *     and only ever hands back a free slot, so the index CT lands on -- 0
 *     included, when CT is what brings the table up -- does not matter.
 *   - src_metadata/dst_metadata stay 0, so nothing is substituted into CT
 *     packets.
 *
 * Serializes on ipa_be_nat_mgmt_lock, the same lock ipa_be_add_pdn uses around
 * its nat_table_hdl check-then-create, so a WAN PDN racing in concurrently
 * can't observe a half-allocated CT PDN.
 */
static int ipa_be_ensure_ct_pdn(void)
{
	ipa_nat_pdn_entry ct_pdn;
	uint8_t pdn_index;
	int ret;

	mutex_lock(&ipa_be_nat_mgmt_lock);

	if (nat_app->ct_pdn_valid) {
		IPA_BE_DBG("CT PDN already allocated at index %u\n",
			   nat_app->ct_pdn_index);
		mutex_unlock(&ipa_be_nat_mgmt_lock);
		return 0;
	}

	memset(&ct_pdn, 0, sizeof(ct_pdn));
	ct_pdn.public_ip = IPA_BE_CT_PDN_PLACEHOLDER_IP;
	ct_pdn.src_metadata = 0;
	ct_pdn.dst_metadata = 0;
	ct_pdn.is_sta = false;

	ret = ipa_nat_alloc_pdn(&ct_pdn, &pdn_index);
	if (ret) {
		IPA_BE_ERR("unable to allocate dedicated CT PDN Error:%d\n", ret);
		mutex_unlock(&ipa_be_nat_mgmt_lock);
		return ret;
	}

	nat_app->ct_pdn_index = pdn_index;
	nat_app->ct_pdn_valid = true;

	IPA_BE_DBG("dedicated CT PDN allocated at index %u (placeholder 0x%08x)\n",
		   pdn_index, ct_pdn.public_ip);

	mutex_unlock(&ipa_be_nat_mgmt_lock);
	return 0;
}

/*
 * ipa_be_ensure_nat_table_ct() - Idempotently create the shared IPv4 NAT table
 * for connection tracking (LAN2LAN) and make sure the dedicated CT PDN exists.
 *
 * IPv4 CT shares the single NAT table (nat_app->nat_table_hdl) with LAN2WAN
 * NAT — the conn_tracking bit is what lets one table hold both. If the table
 * already exists (created by a WAN PDN or a previous CT entry), creation is
 * skipped.
 *
 * When CT does create it, the table is created with public_ip = 0. Table
 * creation is PDN-agnostic: ipanat seeds a PDN only when a non-zero public IP is
 * supplied (the pre-IPAv4 single-PDN model), so passing 0 claims no PDN slot.
 *
 * The CT PDN is then ensured on every call, not just on creation: a WAN PDN may
 * have created the table before the first LAN2LAN connection, and CT must still
 * own its own PDN rather than borrow the WAN's (see ipa_be_ensure_ct_pdn).
 */
static int ipa_be_ensure_nat_table_ct(void)
{
	bool created = false;
	int ret;

	/*
	 * Serialize against ipa_be_add_pdn: both do an unlocked
	 * check-then-create on nat_table_hdl otherwise, and a WAN PDN racing in
	 * here (or vice versa) can leave one table orphaned in ipanat.
	 * Released before calling ipa_be_ensure_ct_pdn(), which takes the same
	 * lock itself.
	 */
	mutex_lock(&ipa_be_nat_mgmt_lock);

	if (nat_app->nat_table_hdl) {
		IPA_BE_DBG("NAT table already exists (hdl=0x%x), reusing for CT\n",
			   nat_app->nat_table_hdl);
	} else {
		ret = ipa_nat_add_ipv4_tbl(0, nat_app->mem_type, nat_app->max_entries,
					   &nat_app->nat_table_hdl);
		if (ret) {
			IPA_BE_ERR("unable to create CT NAT table Error:%d\n", ret);
			mutex_unlock(&ipa_be_nat_mgmt_lock);
			return ret;
		}
		created = true;

		IPA_BE_DBG("created shared NAT table for CT, no PDN seeded (hdl=0x%x)\n",
			   nat_app->nat_table_hdl);
	}

	mutex_unlock(&ipa_be_nat_mgmt_lock);

	ret = ipa_be_ensure_ct_pdn();
	if (ret) {
		IPA_BE_ERR("failed ensuring dedicated CT PDN: %d\n", ret);
		/* Roll back only a table this call created. */
		if (created) {
			mutex_lock(&ipa_be_nat_mgmt_lock);
			ipa_nat_del_ipv4_tbl(nat_app->nat_table_hdl);
			nat_app->nat_table_hdl = 0;
			mutex_unlock(&ipa_be_nat_mgmt_lock);
		}
		return ret;
	}

	return 0;
}

/*
 * count_ct_nat_entries() - Count live connection-tracking entries in the
 * shared IPv4 NAT cache.
 *
 * CT entries are keyed on the conn_tracking bit rather than pdn_index/public_ip
 * (a CT entry has no public IP and shares one dedicated CT PDN). Used for
 * diagnostics; the shared table is never torn
 * down on the last CT entry (see ipa_be_nat_mgmt_exit for the single teardown).
 */
static int count_ct_nat_entries(void)
{
	int cnt;
	int count = 0;

	if (!nat_app)
		return 0;

	mutex_lock(&nat_app->cache_lock);
	for (cnt = 0; cnt < nat_app->max_entries; cnt++) {
		if (nat_app->cache[cnt].private_ip != 0 &&
		    nat_app->cache[cnt].conn_tracking)
			count++;
	}
	mutex_unlock(&nat_app->cache_lock);

	return count;
}

/*
 * ipa_be_add_ct_entry_v4() - Add an IPv4 connection-tracking (LAN2LAN) entry
 * to the shared NAT table.
 *
 * Mirrors ipa_be_add_v6_ct_entry (the IPv6 CT path) but targets the shared IPv4
 * NAT table. No WAN PDN is claimed and no public IP is cached: the entry is
 * installed with conn_tracking=1, public_ip=0 and pdn_index pointing at CT's own
 * dedicated PDN (see ipa_be_ensure_ct_pdn), which the HW needs to validate the
 * match but which contributes nothing to the packet. The single canonical entry
 * covers both directions, so no SW producer cookie is written (mirrors the CT
 * gate in the old NAT path). CT (LAN2LAN) requires IPA v7.0+.
 */
int ipa_be_add_ct_entry_v4(struct ipa_ipv4_rule_create_msg *v4_msg)
{
	int cnt = 0;
	int ret = 0;
	int ep_cmp;
	bool ct_canonical_swap;
	enum ipa_hw_type ipa_ver = ipa_get_hw_type();
	nat_table_entry *rule;

	IPA_BE_DBG("Entry ipa_be_add_ct_entry_v4\n");

	if (ipa_ver < IPA_HW_v7_0) {
		IPA_BE_ERR("IPv4 CT (LAN2LAN) requires IPA v7.0+\n");
		return -EINVAL;
	}

	/* Ensure the shared IPv4 NAT table and CT's dedicated PDN exist. */
	ret = ipa_be_ensure_nat_table_ct();
	if (ret) {
		IPA_BE_ERR("failed ensuring CT NAT table: %d\n", ret);
		return ret;
	}

	/*
	 * Canonical direction: the smaller EP/MAC is the private side. Mirrors
	 * the ct_canonical_swap decision the old ipa_be_addpdn made before
	 * calling ipa_be_add_entry, so add/delete keys stay consistent.
	 */
	ep_cmp = ipa_be_flow_canonical_cmp(v4_msg->conn_rule.flow_interface_num,
					   v4_msg->conn_rule.return_interface_num,
					   v4_msg->conn_rule.flow_mac,
					   v4_msg->conn_rule.return_mac);
	if (ep_cmp < 0)
		return -EINVAL;
	ct_canonical_swap = (ep_cmp == 0);

	rule = kzalloc(sizeof(nat_table_entry), GFP_KERNEL);
	if (!rule) {
		IPA_BE_ERR("Failed to allocate rule\n");
		return IPA_BE_FAILURE;
	}

	ipa_be_build_nat_rule_attrs(*v4_msg, rule);

	/* CT: no translation — public_ip carries no meaning. */
	rule->public_ip = 0;

	if (ct_canonical_swap) {
		uint32_t tmp_ip   = rule->private_ip;
		uint16_t tmp_port = rule->private_port;

		rule->private_ip   = rule->target_ip;
		rule->target_ip    = tmp_ip;
		rule->private_port = rule->target_port;
		rule->target_port  = tmp_port;
		rule->public_port  = rule->private_port; /* CT: no port NAT */
		IPA_BE_DBG("CT canonical swap: private=%pI4h:%u target=%pI4h:%u\n",
			   &rule->private_ip, (unsigned int)rule->private_port,
			   &rule->target_ip,  (unsigned int)rule->target_port);
	}

	CHK_TBL_HDL();

	log_nat(rule->protocol, rule->private_ip, rule->target_ip,
		rule->private_port, rule->public_port, rule->target_port,
		"for CT addition\n");

	if (rule->private_ip == 0 || rule->target_ip == 0 || rule->protocol == 0) {
		IPA_BE_DBG("Invalid Connection, ignoring it\n");
		kfree(rule);
		return IPA_BE_SUCCESS;
	}

	if (chk_for_dup(rule)) {
		IPA_BE_DBG("Duplicate CT rule. Ignore it\n");
		kfree(rule);
		return -1;
	}

	mutex_lock(&nat_app->cache_lock);

	for (; cnt < nat_app->max_entries; cnt++) {
		if (nat_app->cache[cnt].private_ip == 0 &&
		    nat_app->cache[cnt].target_ip == 0 &&
		    nat_app->cache[cnt].private_port == 0 &&
		    nat_app->cache[cnt].target_port == 0 &&
		    nat_app->cache[cnt].protocol == 0) {
			IPA_BE_DBG("found free cache entry %d\n", cnt);
			break;
		}
	}

	if (nat_app->max_entries == cnt) {
		IPA_BE_ERR("Error: Unable to add CT, reached maximum rules\n");
		mutex_unlock(&nat_app->cache_lock);
		kfree(rule);
		return -1;
	}

	/* Power save: cache the rule without installing to HW. */
	if (is_pwr_save_if(rule->private_ip) || is_pwr_save_if(rule->target_ip)) {
		IPA_BE_DBG("Device in Power Save: caching CT rule without HW install\n");
		nat_app->cache[cnt].enabled = false;
		nat_app->cache[cnt].rule_hdl = 0;
	} else {
		ipa_nat_ipv4_rule_v2 nat_rule_v2;
		uint16_t *all_pkts_ptr = NULL;

		memset(&nat_rule_v2, 0, sizeof(nat_rule_v2));

		if (rule->protocol == IPPROTO_GRE) {
			nat_rule_v2.private_ip = rule->private_ip;
			nat_rule_v2.target_ip  = rule->target_ip;
			nat_rule_v2.protocol   = rule->protocol;
		} else {
			nat_rule_v2.private_ip   = rule->private_ip;
			nat_rule_v2.target_ip    = rule->target_ip;
			nat_rule_v2.target_port  = rule->target_port;
			nat_rule_v2.private_port = rule->private_port;
			nat_rule_v2.public_port  = rule->public_port;
			nat_rule_v2.protocol     = rule->protocol;
		}
		/*
		 * CT points at its own dedicated PDN, never at PDN[0]: the HW
		 * validates PDN[pdn_index] on match, and PDN[0] belongs to a WAN
		 * (see ipa_be_ensure_ct_pdn).
		 */
		nat_rule_v2.pdn_index = nat_app->ct_pdn_index;
		nat_rule_v2.uc_activation_index = rule->uc_activation_index;
		nat_rule_v2.ucp = rule->ucp;
		nat_rule_v2.s = rule->s;

		/* Single bidirectional CT entry. */
		nat_rule_v2.conn_tracking = 1;
		nat_rule_v2.out_allowed = 1;
		nat_rule_v2.in_allowed = 1;
		nat_rule_v2.enable = 1;
		nat_rule_v2.all_pkts_stats_cnt_index = 0;
		nat_rule_v2.non_frag_stats_cnt_index = 0;

		/* Allocate the all_pkts stats counter (keyed on private IP). */
		all_pkts_ptr = &nat_rule_v2.all_pkts_stats_cnt_index;
		ret = ipa_nat_alloc_counter_v4(nat_app->nat_table_hdl,
					       rule->private_ip, true, all_pkts_ptr);
		if (ret) {
			IPA_BE_ERR("Failed to allocate all_pkts counter: %d\n", ret);
			*all_pkts_ptr = 0;
		}

		/*
		 * No SW producer cookie for CT: the single canonical entry covers
		 * both directions, so the cookie cannot distinguish per-direction
		 * producers.
		 */

		ret = ipa_nat_add_ipv4_rule_v2(nat_app->nat_table_hdl, &nat_rule_v2,
					       &nat_app->cache[cnt].rule_hdl);
		if (ret < 0) {
			IPA_BE_ERR("unable to add CT rule (v2 API)\n");
			ipa_be_free_nat_counters_v4(nat_app->nat_table_hdl,
						    rule->private_ip,
						    nat_rule_v2.all_pkts_stats_cnt_index,
						    nat_rule_v2.non_frag_stats_cnt_index);
			mutex_unlock(&nat_app->cache_lock);
			kfree(rule);
			return -1;
		}

		nat_app->cache[cnt].all_pkts_stats_cnt_index =
			nat_rule_v2.all_pkts_stats_cnt_index;
		nat_app->cache[cnt].non_frag_stats_cnt_index =
			nat_rule_v2.non_frag_stats_cnt_index;
		ipa_be_cum_assign(nat_rule_v2.all_pkts_stats_cnt_index);

		nat_app->cache[cnt].enabled = true;
		IPA_BE_DBG("Added CT entry: cache[%d] hdl=%d, %pI4n:%u <-> %pI4n:%u\n",
			   cnt, nat_app->cache[cnt].rule_hdl,
			   &rule->private_ip, rule->private_port,
			   &rule->target_ip, rule->target_port);
	}

	/*
	 * CT flags + tuple. No public IP (no translation), but pdn_index points at
	 * CT's own dedicated PDN rather than a WAN's PDN[0].
	 */
	nat_app->cache[cnt].conn_tracking = true;
	nat_app->cache[cnt].out_allowed = true;
	nat_app->cache[cnt].in_allowed = true;
	nat_app->cache[cnt].private_ip = rule->private_ip;
	nat_app->cache[cnt].target_ip = rule->target_ip;
	nat_app->cache[cnt].target_port = rule->target_port;
	nat_app->cache[cnt].private_port = rule->private_port;
	nat_app->cache[cnt].protocol = rule->protocol;
	nat_app->cache[cnt].timestamp = 0;
	nat_app->cache[cnt].public_port = rule->public_port;
	nat_app->cache[cnt].dst_nat = rule->dst_nat;
	nat_app->cache[cnt].pdn_index = nat_app->ct_pdn_index;
	nat_app->cache[cnt].public_ip = 0;
	nat_app->cache[cnt].ip_pass_entry = rule->ip_pass_entry;
	nat_app->cache[cnt].flow_rule_id = v4_msg->tuple.flow_rule_id;
	nat_app->curCnt++;

	mutex_unlock(&nat_app->cache_lock);

	IPA_BE_DBG("%s CT rule(%d)\n",
		   nat_app->cache[cnt].enabled ? "Added" : "Cached", cnt);

	kfree(rule);
	IPA_BE_DBG("Exit ipa_be_add_ct_entry_v4 ret=%d\n", ret);
	return IPA_BE_SUCCESS;
}
EXPORT_SYMBOL(ipa_be_add_ct_entry_v4);

/* Add new entry to the nat table on new connection */
int ipa_be_add_entry(struct ipa_ipv4_rule_create_msg v4_msg, bool isVlan,
		     bool ct_enabled, bool ct_canonical_swap)
{
	int cnt = 0;
	int ret = 0;
	bool cacheOnly = false;
	uint8_t pdn_index;
	ipa_nat_ipv4_rule nat_rule;

	nat_table_entry *rule = kzalloc(sizeof(nat_table_entry), GFP_KERNEL);
	if (!rule) {
		IPA_BE_ERR("Failed to allocate rule\n");
		return IPA_BE_FAILURE;
	}

	ipa_be_build_nat_rule_attrs(v4_msg, rule);

	/*
	 * LAN2WAN NAT only. CT (LAN2LAN) is handled by ipa_be_add_ct_entry_v4();
	 * the ct_enabled/ct_canonical_swap parameters are retained for ABI
	 * stability and are always false on this path.
	 */
	(void)ct_enabled;
	(void)ct_canonical_swap;

	IPA_BE_DBG("%s() %d\n", __FUNCTION__, __LINE__);

	CHK_TBL_HDL();

	log_nat(rule->protocol, rule->private_ip, rule->target_ip, rule->private_port,
		rule->public_port, rule->target_port, "for addition\n");

	if (rule->private_ip == 0 ||
	    rule->target_ip == 0 ||
	    rule->protocol == 0) {
		IPA_BE_DBG("Invalid Connection, ignoring it\n");
		kfree(rule);
		return IPA_BE_SUCCESS;
	}

	if (ipa_nat_get_pdn_index(rule->public_ip, &pdn_index)) {
		if (isVlan) {
			IPA_BE_DBG("vlan iface doesn't have a valid pdn, only moving to cache\n");
			iptodot("private ip", rule->private_ip);
			iptodot("target ip", rule->target_ip);
			iptodot("public ip", rule->public_ip);
			cacheOnly = true;
		} else {
			IPA_BE_ERR("couldn't acquire PDN index for public ip 0x%X\n",
				   rule->public_ip);
			kfree(rule);
			return IPA_BE_FAILURE;
		}
	}

	if (!chk_for_dup(rule)) {
		mutex_lock(&nat_app->cache_lock);

		for (; cnt < nat_app->max_entries; cnt++) {
			if (nat_app->cache[cnt].private_ip == 0 &&
			    nat_app->cache[cnt].target_ip == 0 &&
			    nat_app->cache[cnt].private_port == 0 &&
			    nat_app->cache[cnt].target_port == 0 &&
			    nat_app->cache[cnt].protocol == 0) {
				IPA_BE_DBG("found free cache entry %d\n", cnt);
				break;
			}
		}

		if (nat_app->max_entries == cnt) {
			IPA_BE_ERR("Error: Unable to add, reached maximum rules\n");
			mutex_unlock(&nat_app->cache_lock);
			kfree(rule);
			return -1;
		} else {
			/* Check IPA hardware version */
			enum ipa_hw_type ipa_ver = ipa_get_hw_type();

			/*
			 * LAN2WAN NAT only. On v7.0+ the v2 API sets out_allowed=1
			 * (in_allowed=1); conn_tracking stays 0. CT (LAN2LAN) has
			 * its own path (ipa_be_add_ct_entry_v4).
			 */
			bool is_ct_entry = false;
			bool ct_out_allowed = (ipa_ver >= IPA_HW_v7_0);
			bool ct_in_allowed = (ipa_ver >= IPA_HW_v7_0);

			if (ipa_ver >= IPA_HW_v7_0)
				IPA_BE_DBG("IPA v7.0+ NAT entry: in_allowed=1 out_allowed=1\n");

			/* Power save / cache-only handling (applies to all cases) */
			if (is_pwr_save_if(rule->private_ip) ||
			    is_pwr_save_if(rule->target_ip) ||
			    cacheOnly) {
				if (cacheOnly) {
					IPA_BE_DBG("only caching vlan rule\n");
				} else {
					IPA_BE_DBG("Device is Power Save mode: Dont insert into nat table but cache\n");
				}
				nat_app->cache[cnt].enabled = false;
				nat_app->cache[cnt].rule_hdl = 0;
			} else {
				/* Add rule to hardware */
				if (ipa_ver >= IPA_HW_v7_0) {
					/* IPA v7.0+: Use v2 API for both NAT and CT */
					ipa_nat_ipv4_rule_v2 nat_rule_v2;
					memset(&nat_rule_v2, 0, sizeof(nat_rule_v2));

					/* Populate v2 rule directly from rule structure */
					if (rule->protocol == IPPROTO_GRE) {
						nat_rule_v2.private_ip = rule->private_ip;
						nat_rule_v2.target_ip = rule->target_ip;
						nat_rule_v2.protocol = rule->protocol;
					} else {
						nat_rule_v2.private_ip = rule->private_ip;
						nat_rule_v2.target_ip = rule->target_ip;
						nat_rule_v2.target_port = rule->target_port;
						nat_rule_v2.private_port = rule->private_port;
						nat_rule_v2.public_port = rule->public_port;
						nat_rule_v2.protocol = rule->protocol;
					}
					nat_rule_v2.pdn_index = pdn_index;
					nat_rule_v2.uc_activation_index = rule->uc_activation_index;
					nat_rule_v2.ucp = rule->ucp;
					nat_rule_v2.s = rule->s;

					/* Set CT flags based on use case */
					nat_rule_v2.conn_tracking = is_ct_entry;
					nat_rule_v2.out_allowed = ct_out_allowed;
					nat_rule_v2.in_allowed = ct_in_allowed;
					nat_rule_v2.enable = 1;
					nat_rule_v2.all_pkts_stats_cnt_index = 0;
					nat_rule_v2.non_frag_stats_cnt_index = 0;

					if (!cacheOnly && !is_pwr_save_if(rule->private_ip) &&
					    !is_pwr_save_if(rule->target_ip)) {
						uint16_t *all_pkts_ptr = &nat_rule_v2.all_pkts_stats_cnt_index;
						uint16_t *non_frag_ptr = NULL; /* not currently used for IPv4 NAT */

						if (all_pkts_ptr) {
							ret = ipa_nat_alloc_counter_v4(
								nat_app->nat_table_hdl,
								rule->private_ip,
								true,
								all_pkts_ptr);
							if (ret) {
								IPA_BE_ERR("Failed to allocate all_pkts counter: %d\n", ret);
								*all_pkts_ptr = 0;
							}
						}

						if (non_frag_ptr) {
							ret = ipa_nat_alloc_counter_v4(
								nat_app->nat_table_hdl,
								rule->private_ip,
								false,
								non_frag_ptr);
							if (ret) {
								IPA_BE_ERR("Failed to allocate non_frag counter: %d\n", ret);
								*non_frag_ptr = 0;
							}
						}

						IPA_BE_DBG("Counters: all_pkts=%u%s non_frag=%u%s\n",
							   nat_rule_v2.all_pkts_stats_cnt_index,
							   all_pkts_ptr ? "" : " (skipped)",
							   nat_rule_v2.non_frag_stats_cnt_index,
							   non_frag_ptr ? "" : " (skipped)");
					}

					/*
					 * SW producer cookie: set for NAT entries when the
					 * flow or return interface is WDI6 (vpnum_valid).
					 */
					{
						struct ipa_sw_producer_cookie cookie;

						memset(&cookie, 0, sizeof(cookie));
						ipa3_populate_cookie_vpnum(
							v4_msg.conn_rule.flow_interface_num, &cookie);
						if (!cookie.raw)
							ipa3_populate_cookie_vpnum(
								v4_msg.conn_rule.return_interface_num, &cookie);
						nat_rule_v2.sw_prod_classification_cookie = cookie.raw;
						IPA_BE_DBG("NAT sw_prod_cookie=0x%llx flow_intf=%d ret_intf=%d\n",
							   nat_rule_v2.sw_prod_classification_cookie,
							   v4_msg.conn_rule.flow_interface_num,
							   v4_msg.conn_rule.return_interface_num);
					}

					/* Use v2 API */
					ret = ipa_nat_add_ipv4_rule_v2(nat_app->nat_table_hdl, &nat_rule_v2,
								       &nat_app->cache[cnt].rule_hdl);
					if (ret < 0) {
						IPA_BE_ERR("unable to add rule (v2 API)\n");
						ipa_be_free_nat_counters_v4(
							nat_app->nat_table_hdl,
							rule->private_ip,
							nat_rule_v2.all_pkts_stats_cnt_index,
							nat_rule_v2.non_frag_stats_cnt_index);
						mutex_unlock(&nat_app->cache_lock);
						kfree(rule);
						return -1;
					}

					nat_app->cache[cnt].all_pkts_stats_cnt_index =
						nat_rule_v2.all_pkts_stats_cnt_index;
					nat_app->cache[cnt].non_frag_stats_cnt_index =
						nat_rule_v2.non_frag_stats_cnt_index;
					ipa_be_cum_assign(nat_rule_v2.all_pkts_stats_cnt_index);

					IPA_BE_DBG("Added NAT entry (v7.0+): cache[%d] hdl=%d\n",
						   cnt, nat_app->cache[cnt].rule_hdl);
				} else {
					/* IPA < v7.0: Use v1 API (legacy NAT only) */
					memset(&nat_rule, 0, sizeof(nat_rule));
					if (rule->protocol == IPPROTO_GRE) {
						nat_rule.private_ip = rule->private_ip;
						nat_rule.target_ip = rule->target_ip;
						nat_rule.protocol = rule->protocol;
					} else {
						nat_rule.private_ip = rule->private_ip;
						nat_rule.target_ip = rule->target_ip;
						nat_rule.target_port = rule->target_port;
						nat_rule.private_port = rule->private_port;
						nat_rule.public_port = rule->public_port;
						nat_rule.protocol = rule->protocol;
					}
					nat_rule.uc_activation_index = rule->uc_activation_index;
					nat_rule.ucp = rule->ucp;
					nat_rule.s = rule->s;
					nat_rule.dst_only = rule->dst_only;
					nat_rule.src_only = rule->src_only;
					nat_rule.pdn_index = pdn_index;

					ret = ipa_nat_add_ipv4_rule(nat_app->nat_table_hdl, &nat_rule,
								    &nat_app->cache[cnt].rule_hdl);
					if (ret < 0) {
						IPA_BE_ERR("unable to add NAT rule (v1 API)\n");
						mutex_unlock(&nat_app->cache_lock);
						kfree(rule);
						return -1;
					}
					IPA_BE_DBG("Added NAT entry (legacy): cache[%d] hdl=%d\n",
						   cnt, nat_app->cache[cnt].rule_hdl);
				}

				nat_app->cache[cnt].enabled = true;
			}

			/* Store CT flags in cache (common to both paths above) */
			nat_app->cache[cnt].conn_tracking = is_ct_entry;
			nat_app->cache[cnt].out_allowed = ct_out_allowed;
			nat_app->cache[cnt].in_allowed = ct_in_allowed;

			nat_app->cache[cnt].private_ip = rule->private_ip;
			nat_app->cache[cnt].target_ip = rule->target_ip;
			nat_app->cache[cnt].target_port = rule->target_port;
			nat_app->cache[cnt].private_port = rule->private_port;
			nat_app->cache[cnt].protocol = rule->protocol;
			nat_app->cache[cnt].timestamp = 0;
			nat_app->cache[cnt].public_port = rule->public_port;
			nat_app->cache[cnt].dst_nat = rule->dst_nat;
			nat_app->cache[cnt].pdn_index = pdn_index;
			nat_app->cache[cnt].public_ip = rule->public_ip;
			nat_app->cache[cnt].ip_pass_entry = rule->ip_pass_entry;
			nat_app->cache[cnt].flow_iface = v4_msg.conn_rule.flow_interface_num;
			nat_app->cache[cnt].return_iface = v4_msg.conn_rule.return_interface_num;
			nat_app->cache[cnt].flow_top_iface = v4_msg.conn_rule.flow_top_interface_num;
			nat_app->cache[cnt].return_top_iface = v4_msg.conn_rule.return_top_interface_num;
			nat_app->cache[cnt].flow_rule_id = v4_msg.tuple.flow_rule_id;
			nat_app->cache[cnt].orig_flow_ip      = v4_msg.tuple.flow_ip;
			nat_app->cache[cnt].orig_return_ip    = v4_msg.tuple.return_ip;
			nat_app->cache[cnt].orig_flow_ident   = v4_msg.tuple.flow_ident;
			nat_app->cache[cnt].orig_return_ident = v4_msg.tuple.return_ident;
			nat_app->curCnt++;
		}

		mutex_unlock(&nat_app->cache_lock);
	} else {
		IPA_BE_DBG("Duplicate rule. Ignore it\n");
		kfree(rule);
		return -1;
	}

	if (nat_app->cache[cnt].enabled == true) {
		IPA_BE_DBG("Added rule(%d) successfully handle (%d)\n", cnt,
			   nat_app->cache[cnt].rule_hdl);
	} else {
		IPA_BE_DBG("Cached rule(%d) successfully\n", cnt);
	}

	kfree(rule);
	return IPA_BE_SUCCESS;
}
EXPORT_SYMBOL(ipa_be_add_entry);

void ipa_be_reset(void)
{
	int cnt = 0;

	nat_app->nat_table_hdl = 0;
	nat_app->pub_ip_addr = 0;

	/*
	 * The PDN table went away with the NAT table, so the dedicated CT PDN
	 * must be re-allocated before the next CT entry is installed.
	 */
	nat_app->ct_pdn_valid = false;
	nat_app->ct_pdn_index = 0;

	ipa_stats_process_tuple_flush();

	/* NAT tbl deleted, reset enabled bit */
	mutex_lock(&nat_app->cache_lock);
	for(cnt = 0; cnt < nat_app->max_entries; cnt++)
	{
		nat_app->cache[cnt].enabled = false;
	}
	mutex_unlock(&nat_app->cache_lock);
}
EXPORT_SYMBOL(ipa_be_reset);

/* Count remaining NAT entries for a given public IP (PDN).
 * Returns the number of cache entries whose public_ip matches pub_ip.
 * A return value of 0 means no connections are left on that PDN.
 */
static int count_nat_entries_for_pdn(uint32_t pub_ip)
{
	int cnt;
	int count = 0;

	if (!nat_app)
		return 0;

	mutex_lock(&nat_app->cache_lock);
	for (cnt = 0; cnt < nat_app->max_entries; cnt++) {
		if (nat_app->cache[cnt].private_ip != 0 &&
		    !nat_app->cache[cnt].conn_tracking &&
		    nat_app->cache[cnt].public_ip == pub_ip)
			count++;
	}
	mutex_unlock(&nat_app->cache_lock);

	return count;
}

static int count_v6_ct_entries_for_pdn(int pdn_iface)
{
	int cnt;
	int count = 0;

	if (!NatBase)
		return 0;

	mutex_lock(&NatBase->cache_lock);
	for (cnt = 0; cnt < NatBase->m_maxEntries; cnt++) {
		if (NatBase->m_cache[cnt].m_enabled &&
		    NatBase->m_cache[cnt].m_type == IPA_IP_v6 &&
		    NatBase->m_cache[cnt].ipv6_ct.pdn_iface == pdn_iface)
			count++;
	}
	mutex_unlock(&NatBase->cache_lock);

	return count;
}

int ipa_be_remove_pdn(uint32_t pub_ip, int pdn_iface)
{
	int ret;
	uint8_t pdn_index;
	uint8_t pdn_cnt;
	IPA_BE_DBG("%s() %d\n", __FUNCTION__, __LINE__);

	CHK_TBL_HDL();

	ret = ipa_nat_get_pdn_index(pub_ip, &pdn_index);

	IPA_BE_DBG("RemovePDN  pdn_index:%d, IP: %x\n",pdn_index, pub_ip);
	if(ret)
	{
		IPA_BE_ERR("pdn doesn't exist on pdn table\n");
		return IPA_BE_FAILURE;
	}

	/*
	 * Never tear down the PDN that IPv4 CT owns. A WAN public IP can never
	 * equal IPA_BE_CT_PDN_PLACEHOLDER_IP, so this cannot legitimately match;
	 * bail out rather than zero the PDN out from under every live LAN2LAN
	 * entry (which the HW would then reject with a NAT exception).
	 */
	if (nat_app->ct_pdn_valid && pdn_index == nat_app->ct_pdn_index)
	{
		IPA_BE_ERR("refusing to remove CT-owned PDN index %u (ip 0x%X)\n",
			   pdn_index, pub_ip);
		return IPA_BE_FAILURE;
	}

	/* remove all PDN entries (never CT entries — those use the CT PDN) */
	mutex_lock(&nat_app->cache_lock);
	for(int cnt = 0; cnt < nat_app->max_entries; cnt++)
	{
		if((nat_app->cache[cnt].pdn_index == pdn_index) &&
			!nat_app->cache[cnt].conn_tracking &&
			(nat_app->cache[cnt].enabled == true))
		{
			if(ipa_nat_del_ipv4_rule(nat_app->nat_table_hdl, nat_app->cache[cnt].rule_hdl) < 0)
			{
				IPA_BE_ERR("unable to delete rule with private ip 0x%X\n", nat_app->cache[cnt].private_ip);
				continue;
			}
			memset(&nat_app->cache[cnt], 0, sizeof(nat_app->cache[cnt]));
		}
	}
	mutex_unlock(&nat_app->cache_lock);

	ret = ipa_nat_dealloc_pdn(pdn_index);
	if(ret)
	{
		IPA_BE_ERR(" couldn't deallocate PDN in index %d\n",pdn_index);
		return IPA_BE_FAILURE;
	}

	ret = ipa_nat_get_pdn_count(&pdn_cnt);
	if(ret)
	{
		IPA_BE_ERR(" couldn't acquire number of PDNs\n");
		return IPA_BE_FAILURE;
	}

	/* Skip flush when pdn_iface is unresolved; the loop would find no
	 * matches (pdn_iface == -1 never matches a real pair) but logging
	 * the skip makes upstream resolution failures visible.
	 */
	if (pdn_iface >= 0) {
		IPA_BE_DBG("RemovePDN: flushing UL filter Q6 notify for pdn_iface=%d pub_ip=0x%X\n",
			pdn_iface, pub_ip);
		ipa_be_flush_uplink_rules_for_pdn(pdn_iface, IPA_IP_v4);
	} else {
		IPA_BE_DBG("RemovePDN: pdn_iface unresolved, skipping UL flush for pub_ip=0x%X\n",
			pub_ip);
	}

	/*
	 * Do NOT delete the shared IPv4 NAT table here even when the last PDN is
	 * gone: LAN2LAN CT entries share this table and use their own dedicated CT
	 * PDN, so a PDN count of zero does not imply the table is idle. The table
	 * is torn down exactly once, at ipa_be_nat_mgmt_exit().
	 */
	IPA_BE_DBG("RemovePDN  pdn_index:%d, pdn_count: %d, ip 0x%X\n",pdn_index,pdn_cnt,pub_ip);


	return IPA_BE_SUCCESS;
}
EXPORT_SYMBOL(ipa_be_remove_pdn);

/* Delete the entry from Nat table on connection close */
int ipa_be_delete_nat_entry(const nat_table_entry *rule)
{
	int cnt;
	enum ipa_hw_type ipa_ver = ipa_get_hw_type();

	if (!rule || !nat_app) {
		IPA_BE_ERR("Invalid parameters or nat_app not initialized\n");
		return IPA_BE_FAILURE;
	}

	IPA_BE_DBG("%s() %d\n", __FUNCTION__, __LINE__);

	log_nat(rule->protocol, rule->private_ip, rule->target_ip, rule->private_port,
		rule->public_port, rule->target_port, "for deletion\n");

	mutex_lock(&nat_app->cache_lock);

	for (cnt = 0; cnt < nat_app->max_entries; cnt++) {
		if (nat_app->cache[cnt].private_ip == rule->private_ip &&
		    nat_app->cache[cnt].target_ip == rule->target_ip &&
		    nat_app->cache[cnt].private_port == rule->private_port &&
		    nat_app->cache[cnt].target_port == rule->target_port &&
		    nat_app->cache[cnt].protocol == rule->protocol) {

			if (nat_app->cache[cnt].enabled == true) {
				log_nat(nat_app->cache[cnt].protocol, nat_app->cache[cnt].private_ip,
					nat_app->cache[cnt].target_ip, nat_app->cache[cnt].private_port,
					nat_app->cache[cnt].public_port, nat_app->cache[cnt].target_port,
					"deleting from HW\n");
				if (ipa_nat_del_ipv4_rule(nat_app->nat_table_hdl, nat_app->cache[cnt].rule_hdl) < 0) {
					/* HW delete failed: keep the cache slot so the
					 * rule isn't orphaned and a retry can act on it.
					 */
					IPA_BE_ERR("%s() %d HW deletion failed, keeping cache entry %d (hdl %d)\n",
						__FUNCTION__, __LINE__, cnt, nat_app->cache[cnt].rule_hdl);
					mutex_unlock(&nat_app->cache_lock);
					return IPA_BE_FAILURE;
				}
				if (ipa_ver >= IPA_HW_v7_0) {
					ipa_be_free_nat_counters_v4(
						nat_app->nat_table_hdl,
						nat_app->cache[cnt].private_ip,
						nat_app->cache[cnt].all_pkts_stats_cnt_index,
						nat_app->cache[cnt].non_frag_stats_cnt_index);
				}
				IPA_BE_DBG("Deleted Nat entry(%d) from HW Successfully\n", cnt);
			} else {
				IPA_BE_DBG("Deleted Nat entry(%d) from cache only\n", cnt);
			}

			memset(&nat_app->cache[cnt], 0, sizeof(nat_app->cache[cnt]));
			nat_app->curCnt--;
			mutex_unlock(&nat_app->cache_lock);
			return IPA_BE_SUCCESS; /* Found and processed */
		}
	}

	mutex_unlock(&nat_app->cache_lock);
	IPA_BE_DBG("NAT entry not found for deletion\n");
	return IPA_BE_FAILURE;
}
EXPORT_SYMBOL(ipa_be_delete_nat_entry);


/*
 * ipa_be_delete_ct_entry_v4() - Delete an IPv4 connection-tracking (LAN2LAN)
 * entry from the shared NAT table.
 *
 * The CT delete counterpart of ipa_be_add_ct_entry_v4(): it reconstructs the
 * canonical key that was stored at insertion (replaying the IPPT tuple logic
 * and the ct_canonical_swap decision) and removes the rule from the shared
 * table via ipa_be_delete_nat_entry(). It does NOT delete the shared table —
 * that is freed only at ipa_be_nat_mgmt_exit(), so a surviving WAN PDN (or later
 * CT entry) keeps it.
 *
 * It also deliberately leaves the dedicated CT PDN allocated. Deallocating it on
 * the last CT entry would hand the slot back for a WAN to reuse, and the next
 * LAN2LAN connection would have to re-allocate it; keeping it costs one PDN slot
 * and keeps every CT entry's pdn_index stable for the driver's lifetime.
 */
void ipa_be_delete_ct_entry_v4(struct ipa_ipv4_rule_destroy_msg v4_msg)
{
	nat_table_entry rule;
	int ep_cmp;
	uint32_t tmp_ip;
	uint16_t tmp_port;

	IPA_BE_DBG("Deleting IPv4 CT (LAN2LAN) entry\n");

	memset(&rule, 0, sizeof(rule));
	rule.protocol = v4_msg.tuple.protocol;

	ep_cmp = ipa_be_flow_canonical_cmp(v4_msg.conn_rule.flow_interface_num,
					   v4_msg.conn_rule.return_interface_num,
					   v4_msg.conn_rule.flow_mac,
					   v4_msg.conn_rule.return_mac);
	if (ep_cmp < 0)
		return;

	WARN_ON(v4_msg.tuple.flow_ip != v4_msg.conn_rule.flow_ip_xlate ||
		v4_msg.tuple.return_ip != v4_msg.conn_rule.return_ip_xlate);

	/*
	 * LAN2LAN has no IP translation, so flow_ip == flow_ip_xlate and
	 * return_ip == return_ip_xlate — the IPPT branch always fires.
	 * Replicate ipa_be_build_nat_rule_attrs IPPT logic to get the initial
	 * private/target assignment, then apply the same canonical swap.
	 */
	if (v4_msg.conn_rule.flow_interface_num ==
	    v4_msg.conn_rule.flow_top_interface_num) {
		/* Downlink IPPT (typical non-VLAN LAN2LAN) */
		rule.private_ip   = ntohl(v4_msg.conn_rule.return_ip_xlate);
		rule.target_ip    = ntohl(v4_msg.tuple.flow_ip);
		rule.private_port = ntohs(v4_msg.tuple.return_ident);
		rule.target_port  = ntohs(v4_msg.tuple.flow_ident);
	} else {
		/* Uplink IPPT (VLAN LAN2LAN, flow is sub-interface) */
		rule.private_ip   = ntohl(v4_msg.tuple.flow_ip);
		rule.target_ip    = ntohl(v4_msg.tuple.return_ip);
		rule.private_port = ntohs(v4_msg.tuple.flow_ident);
		rule.target_port  = ntohs(v4_msg.tuple.return_ident);
	}

	/* Mirror ct_canonical_swap from ipa_be_add_ct_entry_v4: swap when ep_cmp==0 */
	if (ep_cmp == 0) {
		tmp_ip            = rule.private_ip;
		tmp_port          = rule.private_port;
		rule.private_ip   = rule.target_ip;
		rule.target_ip    = tmp_ip;
		rule.private_port = rule.target_port;
		rule.target_port  = tmp_port;
	}
	rule.public_port = rule.private_port;
	IPA_BE_DBG("CT canonical delete: private=%pI4h:%u target=%pI4h:%u (flow_ep=%u ret_ep=%u)\n",
		   &rule.private_ip, (unsigned int)rule.private_port,
		   &rule.target_ip, (unsigned int)rule.target_port,
		   v4_msg.conn_rule.flow_interface_num,
		   v4_msg.conn_rule.return_interface_num);

	/*
	 * Remove the CT rule from the shared table. No PDN accounting and no
	 * table teardown: the CT PDN stays allocated and the table lives until
	 * ipa_be_nat_mgmt_exit().
	 */
	if (ipa_be_delete_nat_entry(&rule))
		IPA_BE_ERR("failed removing CT nat entry\n");
	else
		IPA_BE_DBG("CT entry removed; %d CT entries remain in shared table\n",
			   count_ct_nat_entries());
}
EXPORT_SYMBOL(ipa_be_delete_ct_entry_v4);

void ipa_be_delete_entry(struct ipa_ipv4_rule_destroy_msg v4_msg, bool ct_enabled)
{
	nat_table_entry rule;
	struct ipa_ipv4_rule_create_msg create_msg;
	uint32_t pub_ip = 0;

	IPA_BE_DBG("Deleting IP4 NAT entry\n");

	/*
	 * LAN2WAN NAT only. CT (LAN2LAN) is handled by ipa_be_delete_ct_entry_v4();
	 * the ct_enabled parameter is retained for ABI stability and is always
	 * false on this path.
	 */
	(void)ct_enabled;

	memset(&rule, 0, sizeof(rule));
	rule.protocol = v4_msg.tuple.protocol;

	/*
	 * Reconstruct the key that was used during insertion. For NAT the entry
	 * is always created from the uplink direction. The destroy message can
	 * come for either direction; the shared tuple/conn_rule let us rebuild
	 * the stored key via the add-path builder.
	 *
	 * ipa_be_build_nat_rule_attrs() takes a create_msg; tuple and conn_rule
	 * are shared between create and destroy messages, so a small local copy
	 * lets us call the helper without changing its signature.
	 */
	memset(&create_msg, 0, sizeof(create_msg));
	create_msg.tuple     = v4_msg.tuple;
	create_msg.conn_rule = v4_msg.conn_rule;
	ipa_be_build_nat_rule_attrs(create_msg, &rule);

	/* pub_ip lets us drop the PDN once its last entry is gone (mirrors ipa_be_addpdn). */
	if ((v4_msg.tuple.flow_ip == v4_msg.conn_rule.flow_ip_xlate) &&
	    (v4_msg.tuple.return_ip == v4_msg.conn_rule.return_ip_xlate)) {
		/* IP Passthrough case */
		if (v4_msg.conn_rule.flow_interface_num ==
		    v4_msg.conn_rule.flow_top_interface_num)
			pub_ip = ntohl(v4_msg.tuple.return_ip);
		else if (v4_msg.conn_rule.return_interface_num ==
			 v4_msg.conn_rule.return_top_interface_num)
			pub_ip = ntohl(v4_msg.tuple.flow_ip);
	} else if (v4_msg.tuple.flow_ip == v4_msg.conn_rule.flow_ip_xlate) {
		/* Downlink NAT: public IP is the WAN-side (return) address */
		pub_ip = ntohl(v4_msg.tuple.return_ip);
	} else {
		/* Uplink NAT: public IP is the translated flow address */
		pub_ip = ntohl(v4_msg.conn_rule.flow_ip_xlate);
	}

	if (ipa_be_delete_nat_entry(&rule)) {
		IPA_BE_ERR("failed removing nat entry\n");
		return;
	}

	if (pub_ip && count_nat_entries_for_pdn(pub_ip) == 0) {
		int pdn_iface = -1;

		if (v4_msg.conn_rule.return_interface_num ==
		    v4_msg.conn_rule.return_top_interface_num)
			pdn_iface = v4_msg.conn_rule.return_interface_num;
		else if (v4_msg.conn_rule.flow_interface_num ==
			 v4_msg.conn_rule.flow_top_interface_num)
			pdn_iface = v4_msg.conn_rule.flow_interface_num;

		IPA_BE_DBG("Last NAT entry for PDN (pub_ip=0x%X) deleted, removing PDN pdn_iface=%d\n",
			   pub_ip, pdn_iface);
		if (ipa_be_remove_pdn(pub_ip, pdn_iface))
			IPA_BE_ERR("failed removing PDN for pub_ip 0x%X\n", pub_ip);
	}

}

/* Helper function to find IPv6 CT entry in cache by tuple parameters */
int find_ipv6_ct_entry(uint64_t src_ipv6_msb, uint64_t src_ipv6_lsb,
			       uint64_t dest_ipv6_msb, uint64_t dest_ipv6_lsb,
			       uint16_t src_port, uint16_t dest_port, uint8_t protocol)
{
	int cnt;
	int found_idx = -1;

	mutex_lock(&NatBase->cache_lock);

	for (cnt = 0; cnt < NatBase->m_maxEntries; cnt++) {
		if (NatBase->m_cache[cnt].m_enabled &&
		    NatBase->m_cache[cnt].m_type == IPA_IP_v6 &&
		    NatBase->m_cache[cnt].ipv6_ct.src_ipv6_msb == src_ipv6_msb &&
		    NatBase->m_cache[cnt].ipv6_ct.src_ipv6_lsb == src_ipv6_lsb &&
		    NatBase->m_cache[cnt].ipv6_ct.dest_ipv6_msb == dest_ipv6_msb &&
		    NatBase->m_cache[cnt].ipv6_ct.dest_ipv6_lsb == dest_ipv6_lsb &&
		    NatBase->m_cache[cnt].ipv6_ct.src_port == src_port &&
		    NatBase->m_cache[cnt].ipv6_ct.dest_port == dest_port &&
		    NatBase->m_cache[cnt].m_protocol == protocol) {
			found_idx = cnt;
			break;
		}
	}

	mutex_unlock(&NatBase->cache_lock);

	return found_idx;
}

/* Helper function to store IPv6 CT entry in cache */
int store_ipv6_ct_entry(uint64_t src_ipv6_msb, uint64_t src_ipv6_lsb,
				uint64_t dest_ipv6_msb, uint64_t dest_ipv6_lsb,
				uint16_t src_port, uint16_t dest_port,
				uint8_t protocol, uint32_t rule_handle,
				uint32_t flow_rule_id,
				uint16_t all_pkts_stats_cnt_index,
				uint16_t non_frag_stats_cnt_index)
{
	int cnt;
	int stored_idx = -1;

	mutex_lock(&NatBase->cache_lock);

	/* Find an empty slot */
	for (cnt = 0; cnt < NatBase->m_maxEntries; cnt++) {
		if (!NatBase->m_cache[cnt].m_enabled) {
			/* Store the entry */
			NatBase->m_cache[cnt].m_type = IPA_IP_v6;
			NatBase->m_cache[cnt].m_enabled = true;
			NatBase->m_cache[cnt].m_protocol = protocol;
			NatBase->m_cache[cnt].m_direction = NAT_DIRECTION_UNKNOWN;
			NatBase->m_cache[cnt].m_timestamp = 0;

			/* Store IPv6 CT specific data */
			NatBase->m_cache[cnt].ipv6_ct.rule_handle = rule_handle;
			NatBase->m_cache[cnt].ipv6_ct.src_ipv6_msb = src_ipv6_msb;
			NatBase->m_cache[cnt].ipv6_ct.src_ipv6_lsb = src_ipv6_lsb;
			NatBase->m_cache[cnt].ipv6_ct.dest_ipv6_msb = dest_ipv6_msb;
			NatBase->m_cache[cnt].ipv6_ct.dest_ipv6_lsb = dest_ipv6_lsb;
			NatBase->m_cache[cnt].ipv6_ct.src_port = src_port;
			NatBase->m_cache[cnt].ipv6_ct.dest_port = dest_port;
			NatBase->m_cache[cnt].ipv6_ct.flow_rule_id = flow_rule_id;
			NatBase->m_cache[cnt].ipv6_ct.all_pkts_stats_cnt_index = all_pkts_stats_cnt_index;
			NatBase->m_cache[cnt].ipv6_ct.non_frag_stats_cnt_index = non_frag_stats_cnt_index;
			/* -1 sentinel so count_v6_ct_entries_for_pdn() never
			 * matches a not-yet-populated slot against pdn_iface==0.
			 */
			NatBase->m_cache[cnt].ipv6_ct.pdn_iface = -1;

			IPA_BE_DBG("Stored IPv6 CT entry at index %d with handle %u\n", cnt, rule_handle);
			stored_idx = cnt;
			break;
		}
	}

	mutex_unlock(&NatBase->cache_lock);

	if (stored_idx < 0) {
		IPA_BE_ERR("No free slot available for IPv6 CT entry\n");
	}

	return stored_idx;
}

void ipa_be_delete_v6_ct_entry(struct ipa_ipv6_rule_destroy_msg v6_msg, uint32_t rule_handle)
{
	IPA_BE_DBG("Deleting IPv6 CT entry with handle %u\n", rule_handle);

	if (ipv6ct_del_entry(&v6_msg, rule_handle)) {
		IPA_BE_ERR("failed removing IPv6 CT entry\n");
	} else {
		IPA_BE_DBG("Successfully removed IPv6 CT entry with handle %u\n", rule_handle);
	}
}
EXPORT_SYMBOL(ipa_be_delete_v6_ct_entry);

/* Function to handle IPv6 CT entry deletion during rule destroy */
void ipa_be_handle_v6_ct_deletion(struct ipa_ipv6_rule_destroy_msg *msg, bool lan2lan)
{
	struct ipa_ipv6_5tuple *tuple;
	uint64_t src_ipv6_msb, src_ipv6_lsb, dest_ipv6_msb, dest_ipv6_lsb;
	uint16_t src_port, dest_port;
	int cache_idx;
	uint32_t rule_handle;
	enum ipa_hw_type ipa_ver = ipa_get_hw_type();

	if (!msg || !NatBase) {
		IPA_BE_ERR("Invalid parameters or NatBase not initialized\n");
		return;
	}

	tuple = &msg->tuple;

	/* Convert IPv6 addresses to 64-bit values for comparison */
	src_ipv6_msb = ((__u64)ntohl(tuple->flow_ip[0]) << 32) | ntohl(tuple->flow_ip[1]);
	src_ipv6_lsb = ((__u64)ntohl(tuple->flow_ip[2]) << 32) | ntohl(tuple->flow_ip[3]);
	dest_ipv6_msb = ((__u64)ntohl(tuple->return_ip[0]) << 32) | ntohl(tuple->return_ip[1]);
	dest_ipv6_lsb = ((__u64)ntohl(tuple->return_ip[2]) << 32) | ntohl(tuple->return_ip[3]);
	src_port = ntohs(tuple->flow_ident);
	dest_port = ntohs(tuple->return_ident);

	/*
	 * Find and delete the single canonical CT entry.
	 * For v7.0+ LAN2LAN, the entry is keyed on smaller EP as src.
	 */
	if (lan2lan && ipa_ver >= IPA_HW_v7_0) {
		uint64_t can_src_msb, can_src_lsb, can_dst_msb, can_dst_lsb;
		uint16_t can_src_port, can_dst_port;
		bool swap_src_dest;

		if (ipa_be_ipv6ct_canonical_swap(msg->conn_rule.flow_interface_num,
						  msg->conn_rule.return_interface_num,
						  msg->conn_rule.flow_mac,
						  msg->conn_rule.return_mac,
						  &swap_src_dest))
			return;

		if (swap_src_dest) {
			can_src_msb  = dest_ipv6_msb;
			can_src_lsb  = dest_ipv6_lsb;
			can_dst_msb  = src_ipv6_msb;
			can_dst_lsb  = src_ipv6_lsb;
			can_src_port = dest_port;
			can_dst_port = src_port;
		} else {
			can_src_msb  = src_ipv6_msb;
			can_src_lsb  = src_ipv6_lsb;
			can_dst_msb  = dest_ipv6_msb;
			can_dst_lsb  = dest_ipv6_lsb;
			can_src_port = src_port;
			can_dst_port = dest_port;
		}

		cache_idx = find_ipv6_ct_entry(can_src_msb, can_src_lsb, can_dst_msb, can_dst_lsb,
					       can_src_port, can_dst_port, tuple->protocol);
		if (cache_idx >= 0) {
			uint16_t all_pkts_counter, non_frag_counter;

			mutex_lock(&NatBase->cache_lock);
			rule_handle = NatBase->m_cache[cache_idx].ipv6_ct.rule_handle;
			all_pkts_counter = NatBase->m_cache[cache_idx].ipv6_ct.all_pkts_stats_cnt_index;
			non_frag_counter = NatBase->m_cache[cache_idx].ipv6_ct.non_frag_stats_cnt_index;
			IPA_BE_DBG("Found IPv6 CT canonical entry at cache index %d with handle %u\n",
				   cache_idx, rule_handle);

			ipa_be_delete_v6_ct_entry(*msg, rule_handle);
			ipa_be_free_ct_counters_v6(NatBase->ct_table_hdl,
						   can_src_lsb, can_src_msb,
						   all_pkts_counter, non_frag_counter);

			memset(&NatBase->m_cache[cache_idx], 0, sizeof(NatBase->m_cache[cache_idx]));
			NatBase->m_cache[cache_idx].ipv6_ct.pdn_iface = -1;
			mutex_unlock(&NatBase->cache_lock);
			IPA_BE_DBG("Successfully deleted IPv6 CT canonical entry\n");

			/* LAN2LAN traffic does not transit a WAN PDN, so no
			 * ipa_be_flush_uplink_rules_for_pdn() trigger here —
			 * Q6 has no UL filter state to withdraw for LAN2LAN.
			 * The fwd path below handles real LAN→WAN v6 flows.
			 */
		} else {
			IPA_BE_ERR("IPv6 CT canonical entry not found in cache for deletion\n");
		}
		return;
	}

	/*
	 * Find and delete the forward IPv6 CT entry.
	 *
	 * Reached for pre-v7.0 LAN2LAN/LAN2WAN and for v7.0+ LAN2WAN (v7.0+
	 * LAN2LAN already returned above via its own canonical swap). For
	 * v7.0+ LAN2WAN, mirror the same Uplink/Downlink-based swap applied
	 * on the add path so the cache lookup key matches what was stored.
	 */
	if (ipa_ver >= IPA_HW_v7_0 &&
	    msg->conn_rule.return_interface_num != msg->conn_rule.return_top_interface_num) {
		/* Downlink LAN2WAN: return_ip is the stored src (swap). */
		cache_idx = find_ipv6_ct_entry(dest_ipv6_msb, dest_ipv6_lsb,
					       src_ipv6_msb, src_ipv6_lsb,
					       dest_port, src_port, tuple->protocol);
	} else {
		cache_idx = find_ipv6_ct_entry(src_ipv6_msb, src_ipv6_lsb,
					       dest_ipv6_msb, dest_ipv6_lsb,
					       src_port, dest_port, tuple->protocol);
	}

	if (cache_idx >= 0) {
		uint16_t all_pkts_counter;
		uint16_t non_frag_counter;
		uint64_t stored_src_lsb, stored_src_msb;
		int pdn_iface;

		mutex_lock(&NatBase->cache_lock);
		rule_handle = NatBase->m_cache[cache_idx].ipv6_ct.rule_handle;
		all_pkts_counter = NatBase->m_cache[cache_idx].ipv6_ct.all_pkts_stats_cnt_index;
		non_frag_counter = NatBase->m_cache[cache_idx].ipv6_ct.non_frag_stats_cnt_index;
		stored_src_msb = NatBase->m_cache[cache_idx].ipv6_ct.src_ipv6_msb;
		stored_src_lsb = NatBase->m_cache[cache_idx].ipv6_ct.src_ipv6_lsb;
		pdn_iface = NatBase->m_cache[cache_idx].ipv6_ct.pdn_iface;
		IPA_BE_DBG("Found IPv6 CT fwd entry at cache index %d with handle %u pdn_iface=%d\n",
			   cache_idx, rule_handle, pdn_iface);

		/* Delete the IPv6 CT entry from hardware */
		ipa_be_delete_v6_ct_entry(*msg, rule_handle);

		if (ipa_ver >= IPA_HW_v7_0) {
			ipa_be_free_ct_counters_v6(
				NatBase->ct_table_hdl,
				stored_src_lsb,
				stored_src_msb,
				all_pkts_counter,
				non_frag_counter);
		}

		memset(&NatBase->m_cache[cache_idx], 0, sizeof(NatBase->m_cache[cache_idx]));
		NatBase->m_cache[cache_idx].ipv6_ct.pdn_iface = -1;
		mutex_unlock(&NatBase->cache_lock);

		IPA_BE_DBG("Successfully deleted IPv6 CT fwd entry and cleared cache\n");

		/* Flush v6 UL rules on last CT entry for this PDN. */
		if (pdn_iface >= 0 &&
		    count_v6_ct_entries_for_pdn(pdn_iface) == 0) {
			IPA_BE_DBG("Last v6 CT entry for pdn_iface=%d gone, flushing v6 UL rules\n",
				   pdn_iface);
			ipa_be_flush_uplink_rules_for_pdn(pdn_iface, IPA_IP_v6);
		}
	} else {
		IPA_BE_ERR("IPv6 CT fwd entry not found in cache for deletion\n");
	}
}


/**
 * get_ct_stats_cached() - Return the ECM delta for a stats counter.
 *
 * Serves from the poller-maintained cumulative table instead of reading the
 * clear-on-read HW counter, so ECM no longer races stats_mgr. Reports the delta
 * since the previous ECM sync (cumulative total minus the last-reported
 * snapshot), preserving ECM's on-the-wire delta semantics. The per-cycle cache,
 * keyed by stats_cnt, ensures a counter shared by more than one entry in the
 * same cycle advances ecm_last only once and returns the same delta to all.
 *
 * @stats_cnt: HW stats counter index (cache key and ipa_be_cum[] index)
 * @cache:     per-cycle stats cache array (indexed by stats_cnt)
 * @valid:     per-cycle cache-valid flags array (indexed by stats_cnt)
 * @cache_len: allocated length of cache and valid arrays (snapshot of ipa_be_cum_n)
 * @out:       output: populated with the delta on success
 *
 * Returns 0 on success, non-zero if the counter has no cumulative record.
 */
static int get_ct_stats_cached(uint16_t stats_cnt,
				struct ipahal_stats_nat_ct *cache,
				bool *valid,
				u32 cache_len,
				struct ipahal_stats_nat_ct *out)
{
	struct ipa_be_cum_counter *c;

	if (stats_cnt >= cache_len)
		return -ENOENT;

	if (valid[stats_cnt]) {
		*out = cache[stats_cnt];
		return 0;
	}

	mutex_lock(&ipa_be_cum_lock);
	if (!ipa_be_cum || stats_cnt >= ipa_be_cum_n) {
		mutex_unlock(&ipa_be_cum_lock);
		return -ENOENT;
	}
	c = &ipa_be_cum[stats_cnt];
	{
		struct ipahal_stats_nat_ct *t = &c->total, *l = &c->ecm_last;

		out->num_pkts_inbound        = t->num_pkts_inbound - l->num_pkts_inbound;
		out->num_pkts_cache_inbound  = t->num_pkts_cache_inbound - l->num_pkts_cache_inbound;
		out->num_bytes_inbound       = t->num_bytes_inbound - l->num_bytes_inbound;
		out->num_pkts_outbound       = t->num_pkts_outbound - l->num_pkts_outbound;
		out->num_pkts_cache_outbound = t->num_pkts_cache_outbound - l->num_pkts_cache_outbound;
		out->num_bytes_outbound      = t->num_bytes_outbound - l->num_bytes_outbound;
	}
	c->ecm_last = c->total;
	mutex_unlock(&ipa_be_cum_lock);

	cache[stats_cnt] = *out;
	valid[stats_cnt] = true;
	return 0;
}

/**
 * ipa_be_get_cum_stats() - Read the poller-maintained cumulative RX/TX totals
 * for one stats counter index.
 *
 * Unlike get_ct_stats_cached(), returns the raw monotonic total without
 * advancing ecm_last - a read-only view for debugfs, no HW access.
 *
 * Return: 0 on success; -EINVAL on bad args; -ENOENT if BE not initialised,
 * index out of range, or no cumulative record yet.
 */
int ipa_be_get_cum_stats(u16 all_pkts_cnt_index,
			 struct ipahal_stats_nat_ct *out)
{
	if (!out)
		return -EINVAL;

	if (!all_pkts_cnt_index)
		return -ENOENT;

	mutex_lock(&ipa_be_cum_lock);
	if (!ipa_be_cum || all_pkts_cnt_index >= ipa_be_cum_n ||
	    !ipa_be_cum[all_pkts_cnt_index].active) {
		mutex_unlock(&ipa_be_cum_lock);
		return -ENOENT;
	}
	*out = ipa_be_cum[all_pkts_cnt_index].total;
	mutex_unlock(&ipa_be_cum_lock);

	return 0;
}
EXPORT_SYMBOL(ipa_be_get_cum_stats);

/**
 * ipa_be_query_tuple_stats() - Look up one stats_mgr 5-tuple in the NAT/CT
 * cache and return its cumulative packet/byte counters.
 *
 * @tuple: connection tuple from the stats_mgr genl module. Addresses and ports
 *         are in network byte order; @af selects AF_INET / AF_INET6.
 * @out:   on success, populated with the (truncated) uapi stats.
 *
 * Walks the appropriate cache (nat_app->cache for AF_INET, NatBase->m_cache for
 * AF_INET6) for an enabled entry matching @tuple in either the forward or the
 * src/dst-swapped orientation, resolves its all-packets counter index, then
 * returns that counter's cumulative total from the poller-maintained table. No
 * HW access, so concurrent ECM/stats_mgr reads no longer clear each other.
 *
 * Return: 0 on success; -EINVAL on bad args / af; -ENODEV if BE not
 *         initialised; -ENOENT if no matching enabled entry, no counter, or no
 *         cumulative record. On any non-zero return *out is left untouched.
 */
int ipa_be_query_tuple_stats(const struct ipa_stats_tuple_info *tuple,
			     struct stats_nat_ct *out)
{
	struct ipahal_stats_nat_ct hw = {};
	int idx = -1, i;
	uint16_t stats_cnt = 0;

	if (!tuple || !out)
		return -EINVAL;

	if (tuple->protocol != IPPROTO_TCP && tuple->protocol != IPPROTO_UDP)
		return -EINVAL;

	if (tuple->af == AF_INET) {
		/* genl tuple addr/port are network order; cache is host order */
		uint32_t s_ip = ntohl(tuple->src_addr[0]);
		uint32_t d_ip = ntohl(tuple->dst_addr[0]);
		uint16_t s_pt = ntohs(tuple->src_port);
		uint16_t d_pt = ntohs(tuple->dst_port);
		int rev_idx = -1;

		mutex_lock(&ipa_be_nat_mgmt_lock);

		if (!nat_app || !nat_app->cache) {
			mutex_unlock(&ipa_be_nat_mgmt_lock);
			IPA_BE_DBG("v4 NAT cache not initialised, dropping query\n");
			return -ENODEV;
		}

		mutex_lock(&nat_app->cache_lock);
		for (i = 0; i < nat_app->max_entries; i++) {
			nat_table_entry *e = &nat_app->cache[i];

			if (!e->enabled || e->private_ip == 0)
				continue;
			if (e->protocol != tuple->protocol)
				continue;

			/*
			 * Forward orientation (private==src, target==dst) is the
			 * exact match for the direction this tuple represents;
			 * take it and stop. Reverse orientation is only a
			 * fallback for a connection BE stored swapped relative to
			 * how the tuple is named -- remember the first such slot
			 * but keep scanning, so a forward-registered tuple and
			 * its reverse twin resolve to their own distinct entries
			 * (and counters) instead of colliding on whichever slot
			 * the scan reaches first.
			 */
			if (e->private_ip   == s_ip && e->target_ip   == d_ip &&
			    e->private_port == s_pt && e->target_port == d_pt) {
				idx = i;
				break;
			}
			if (rev_idx < 0 &&
			    e->private_ip   == d_ip && e->target_ip   == s_ip &&
			    e->private_port == d_pt && e->target_port == s_pt)
				rev_idx = i;
		}
		if (idx < 0)
			idx = rev_idx;
		if (idx >= 0)
			stats_cnt = nat_app->cache[idx].all_pkts_stats_cnt_index;
		mutex_unlock(&nat_app->cache_lock);

		mutex_unlock(&ipa_be_nat_mgmt_lock);

	} else if (tuple->af == AF_INET6) {
		uint64_t s_msb = ((uint64_t)ntohl(tuple->src_addr[0]) << 32) |
				 ntohl(tuple->src_addr[1]);
		uint64_t s_lsb = ((uint64_t)ntohl(tuple->src_addr[2]) << 32) |
				 ntohl(tuple->src_addr[3]);
		uint64_t d_msb = ((uint64_t)ntohl(tuple->dst_addr[0]) << 32) |
				 ntohl(tuple->dst_addr[1]);
		uint64_t d_lsb = ((uint64_t)ntohl(tuple->dst_addr[2]) << 32) |
				 ntohl(tuple->dst_addr[3]);
		uint16_t s_pt = ntohs(tuple->src_port);
		uint16_t d_pt = ntohs(tuple->dst_port);
		int rev_idx = -1;

		mutex_lock(&ipa_be_nat_mgmt_lock);

		if (!NatBase || !NatBase->m_cache) {
			mutex_unlock(&ipa_be_nat_mgmt_lock);
			IPA_BE_DBG("v6 CT cache not initialised, dropping query\n");
			return -ENODEV;
		}

		mutex_lock(&NatBase->cache_lock);
		for (i = 0; i < NatBase->m_maxEntries; i++) {
			struct NatEntryBase *e = &NatBase->m_cache[i];
			struct ipv6_ct_entry *c = &e->ipv6_ct;

			if (!e->m_enabled || e->m_type != IPA_IP_v6)
				continue;
			if (e->m_protocol != tuple->protocol)
				continue;

			/* Forward orientation wins; reverse is a fallback only
			 * (see the AF_INET path for the rationale).
			 */
			if (c->src_ipv6_msb  == s_msb && c->src_ipv6_lsb  == s_lsb &&
			    c->dest_ipv6_msb == d_msb && c->dest_ipv6_lsb == d_lsb &&
			    c->src_port      == s_pt  && c->dest_port      == d_pt) {
				idx = i;
				break;
			}
			if (rev_idx < 0 &&
			    c->src_ipv6_msb  == d_msb && c->src_ipv6_lsb  == d_lsb &&
			    c->dest_ipv6_msb == s_msb && c->dest_ipv6_lsb == s_lsb &&
			    c->src_port      == d_pt  && c->dest_port      == s_pt)
				rev_idx = i;
		}
		if (idx < 0)
			idx = rev_idx;
		if (idx >= 0)
			stats_cnt =
			    NatBase->m_cache[idx].ipv6_ct.all_pkts_stats_cnt_index;
		mutex_unlock(&NatBase->cache_lock);

		mutex_unlock(&ipa_be_nat_mgmt_lock);

	} else {
		return -EINVAL;
	}

	if (idx < 0 || stats_cnt == 0) {
		if (tuple->af == AF_INET)
			IPA_BE_DBG("no cache match: %pI4:%u<->%pI4:%u proto=%u\n",
				   &tuple->src_addr[0], ntohs(tuple->src_port),
				   &tuple->dst_addr[0], ntohs(tuple->dst_port),
				   tuple->protocol);
		else
			IPA_BE_DBG("no cache match: %pI6c:%u<->%pI6c:%u proto=%u\n",
				   (struct in6_addr *)tuple->src_addr, ntohs(tuple->src_port),
				   (struct in6_addr *)tuple->dst_addr, ntohs(tuple->dst_port),
				   tuple->protocol);
		return -ENOENT;
	}

	/* Serve the cumulative total maintained by the poller; no HW access. */
	mutex_lock(&ipa_be_cum_lock);
	if (!ipa_be_cum || stats_cnt >= ipa_be_cum_n) {
		mutex_unlock(&ipa_be_cum_lock);
		IPA_BE_DBG("cache_idx=%d cnt_idx=%u but no cumulative record (cum_n=%u)\n",
			   idx, stats_cnt, ipa_be_cum_n);
		return -ENOENT;
	}
	hw = ipa_be_cum[stats_cnt].total;
	mutex_unlock(&ipa_be_cum_lock);

	out->num_pkts_inbound        = hw.num_pkts_inbound;
	out->num_pkts_cache_inbound  = hw.num_pkts_cache_inbound;
	out->num_bytes_inbound       = hw.num_bytes_inbound;
	out->num_pkts_outbound       = hw.num_pkts_outbound;
	out->num_pkts_cache_outbound = hw.num_pkts_cache_outbound;
	out->num_bytes_outbound      = hw.num_bytes_outbound;

	if (tuple->af == AF_INET)
		IPA_BE_DBG("stats for %pI4:%u<->%pI4:%u proto=%u cache_idx=%d cnt_idx=%u: in_pkts=%u in_bytes=%llu out_pkts=%u out_bytes=%llu\n",
			   &tuple->src_addr[0], ntohs(tuple->src_port),
			   &tuple->dst_addr[0], ntohs(tuple->dst_port),
			   tuple->protocol, idx, stats_cnt,
			   hw.num_pkts_inbound, hw.num_bytes_inbound,
			   hw.num_pkts_outbound, hw.num_bytes_outbound);
	else
		IPA_BE_DBG("stats for %pI6c:%u<->%pI6c:%u proto=%u cache_idx=%d cnt_idx=%u: in_pkts=%u in_bytes=%llu out_pkts=%u out_bytes=%llu\n",
			   (struct in6_addr *)tuple->src_addr, ntohs(tuple->src_port),
			   (struct in6_addr *)tuple->dst_addr, ntohs(tuple->dst_port),
			   tuple->protocol, idx, stats_cnt,
			   hw.num_pkts_inbound, hw.num_bytes_inbound,
			   hw.num_pkts_outbound, hw.num_bytes_outbound);

	return 0;
}
EXPORT_SYMBOL(ipa_be_query_tuple_stats);

/**
 * ipa_sync_ipv6_stats_many_msg() - Synchronize statistics for multiple IPv6 CT connections
 * @ipa_ctx: IPA context
 * @msg: The IPv6 message containing the sync request
 *
 * This function queries timestamp information from the IPv6 CT table for all active connections
 * and populates the response message with index and timestamp data.
 *
 * Return: IPA_TX_SUCCESS on success, error code on failure
 */
ipa_tx_status_t ipa_sync_ipv6_stats_many_msg(struct ipa_ctx_instance_internal *ipa_ctx, struct ipa_ipv6_msg *msg)
{
	struct ipa_ipv6_conn_sync_many_msg *sync_many;
	uint16_t start_index;
	uint16_t count = 0;
	uint16_t next_index;
	int i;
	uint32_t timestamp = 0;
	uint32_t rule_handle;
	uint32_t old_timestamp;
	int ret;
	bool is_timestamp = true;
	bool is_stats = true;
	u32 entry_stats_cache_len = 0;
	struct ipahal_stats_nat_ct *entry_stats_cache = NULL;
	bool *entry_stats_cache_valid = NULL;

	if (!msg || !NatBase) {
		IPA_BE_ERR("Invalid parameters or NatBase not initialized\n");
		return IPA_TX_FAILURE_BAD_PARAM;
	}

	switch (msg->cm.type) {
	case IPA_TX_CONN_STATS_SYNC_MANY_TS_ONLY_MSG:
		is_timestamp = true;
		is_stats = false;
		break;
	case IPA_TX_CONN_STATS_SYNC_MANY_STATS_ONLY_MSG:
		is_timestamp = false;
		is_stats = true;
		break;
	case IPA_TX_CONN_STATS_SYNC_MANY_MSG:
	default:
		is_timestamp = true;
		is_stats = true;
		break;
	}

	IPA_BE_DBG("Sampling mode: type=%u is_timestamp=%d is_stats=%d\n",
		   msg->cm.type, is_timestamp, is_stats);

	sync_many = &msg->msg.conn_stats_many;
	start_index = sync_many->index;

	IPA_BE_DBG("Sync many stats request (IPv6): start_index=%u, size=%u\n",
		   start_index, sync_many->size);

	/* Validate IPv6 CT table handle */
	if (!NatBase->ct_table_hdl) {
		IPA_BE_ERR("IPv6 CT table not initialized\n");
		return IPA_TX_FAILURE_NOT_READY;
	}

	/* Calculate maximum number of entries that can fit in the response */
	uint16_t max_entries = (sync_many->size - sizeof(struct ipa_ipv6_conn_sync_many_msg)) /
			       sizeof(struct ipa_ipv6_conn_sync);

	if (max_entries == 0) {
		IPA_BE_ERR("Buffer size too small for any entries\n");
		return IPA_TX_FAILURE_TOO_SHORT;
	}

	if (is_timestamp) {
		IPA_BE_DBG("Flushing timestamp for IPv6 CT table hdl=%u\n",
			NatBase->ct_table_hdl);
		ret = ipa_ipv6ct_timestamp_flush(NatBase->ct_table_hdl);
		if (ret < 0) {
			IPA_BE_ERR("Failed to flush IPv6 CT timestamp, hdl=%u ret=%d\n",
				NatBase->ct_table_hdl, ret);
			return IPA_TX_FAILURE;
		}
	}

	if (is_stats) {
		mutex_lock(&ipa_be_cum_lock);
		entry_stats_cache_len = ipa_be_cum_n;
		mutex_unlock(&ipa_be_cum_lock);
		entry_stats_cache = kcalloc(entry_stats_cache_len,
					    sizeof(*entry_stats_cache), GFP_KERNEL);
		entry_stats_cache_valid = kcalloc(entry_stats_cache_len,
						  sizeof(bool), GFP_KERNEL);
		if (!entry_stats_cache || !entry_stats_cache_valid) {
			IPA_BE_ERR("Failed to allocate stats cache (ipa_be_cum_n=%u)\n",
				   entry_stats_cache_len);
			kfree(entry_stats_cache);
			kfree(entry_stats_cache_valid);
			return IPA_TX_FAILURE;
		}
	}

	/* Iterate through IPv6 CT cache entries starting from the requested index */
	for (i = start_index; i < NatBase->m_maxEntries && count < max_entries; i++) {
		/* Lock to safely check entry state and read rule handle */
		mutex_lock(&NatBase->cache_lock);

		/* Skip empty or disabled entries, or non-IPv6 entries */
		if (!NatBase->m_cache[i].m_enabled || NatBase->m_cache[i].m_type != IPA_IP_v6) {
			mutex_unlock(&NatBase->cache_lock);
			continue;
		}

		/* Read rule handle while holding lock */
		rule_handle = NatBase->m_cache[i].ipv6_ct.rule_handle;

		mutex_unlock(&NatBase->cache_lock);

		if (is_timestamp) {
			/* Query timestamp for this IPv6 CT entry (without holding lock) */
			ret = ipa_ct_query_timestamp(NatBase->ct_table_hdl,
						     rule_handle,
						     &timestamp);
			if (ret) {
				IPA_BE_DBG("Failed to query timestamp for IPv6 CT entry %d, rule_hdl=%u\n",
					   i, rule_handle);
				/* Continue to next entry instead of failing completely */
				continue;
			}
		}

		/* Populate the connection sync structure */
		struct ipa_ipv6_conn_sync *conn_sync = &sync_many->conn_sync[count];
		memset(conn_sync, 0, sizeof(*conn_sync));

		/* Set the index to the cache table index */
		conn_sync->index = i;

		/* Lock to safely read cache data and update timestamp */
		mutex_lock(&NatBase->cache_lock);

		/* Set protocol */
		conn_sync->protocol = NatBase->m_cache[i].m_protocol;

		/* Set IPv6 addresses - convert from 64-bit values back to IPv6 format */
		conn_sync->flow_ip[0] = htonl((uint32_t)(NatBase->m_cache[i].ipv6_ct.src_ipv6_msb >> 32));
		conn_sync->flow_ip[1] = htonl((uint32_t)(NatBase->m_cache[i].ipv6_ct.src_ipv6_msb & 0xFFFFFFFF));
		conn_sync->flow_ip[2] = htonl((uint32_t)(NatBase->m_cache[i].ipv6_ct.src_ipv6_lsb >> 32));
		conn_sync->flow_ip[3] = htonl((uint32_t)(NatBase->m_cache[i].ipv6_ct.src_ipv6_lsb & 0xFFFFFFFF));

		conn_sync->return_ip[0] = htonl((uint32_t)(NatBase->m_cache[i].ipv6_ct.dest_ipv6_msb >> 32));
		conn_sync->return_ip[1] = htonl((uint32_t)(NatBase->m_cache[i].ipv6_ct.dest_ipv6_msb & 0xFFFFFFFF));
		conn_sync->return_ip[2] = htonl((uint32_t)(NatBase->m_cache[i].ipv6_ct.dest_ipv6_lsb >> 32));
		conn_sync->return_ip[3] = htonl((uint32_t)(NatBase->m_cache[i].ipv6_ct.dest_ipv6_lsb & 0xFFFFFFFF));

		/* Set ports */
		conn_sync->flow_ident = htons(NatBase->m_cache[i].ipv6_ct.src_port);
		conn_sync->return_ident = htons(NatBase->m_cache[i].ipv6_ct.dest_port);

		conn_sync->flow_rule_id = NatBase->m_cache[i].ipv6_ct.flow_rule_id;

		/* Populate xlate fields for IPv6. Mirror original tuple when NAT is not used */
		conn_sync->flow_ip_xlate[0] = conn_sync->flow_ip[0];
		conn_sync->flow_ip_xlate[1] = conn_sync->flow_ip[1];
		conn_sync->flow_ip_xlate[2] = conn_sync->flow_ip[2];
		conn_sync->flow_ip_xlate[3] = conn_sync->flow_ip[3];

		conn_sync->return_ip_xlate[0] = conn_sync->return_ip[0];
		conn_sync->return_ip_xlate[1] = conn_sync->return_ip[1];
		conn_sync->return_ip_xlate[2] = conn_sync->return_ip[2];
		conn_sync->return_ip_xlate[3] = conn_sync->return_ip[3];

		conn_sync->flow_ident_xlate = conn_sync->flow_ident;
		conn_sync->return_ident_xlate = conn_sync->return_ident;

		if (is_timestamp) {
			/* Only set inc_ticks if timestamp has changed (entry is active) */
			old_timestamp = NatBase->m_cache[i].m_timestamp;
			if (timestamp != old_timestamp) {
				conn_sync->inc_ticks = timestamp - old_timestamp;
				/* Cache the new timestamp for next query */
				NatBase->m_cache[i].m_timestamp = timestamp;
			}
			/* If timestamp hasn't changed, leave inc_ticks as 0 (entry is inactive) */
		}

		mutex_unlock(&NatBase->cache_lock);

		if (is_stats &&
		    NatBase->m_cache[i].ipv6_ct.all_pkts_stats_cnt_index > 0) {
			struct ipahal_stats_nat_ct stats = {};

			ret = get_ct_stats_cached(
					NatBase->m_cache[i].ipv6_ct.all_pkts_stats_cnt_index,
					entry_stats_cache, entry_stats_cache_valid,
					entry_stats_cache_len, &stats);
			if (ret) {
				IPA_BE_ERR("Failed to get stats for IPv6 CT entry %d: idx=%u, ret=%d\n",
					   i,
					   NatBase->m_cache[i].ipv6_ct.all_pkts_stats_cnt_index,
					   ret);
			} else {
				IPA_BE_DBG("Rule %d stats: idx=%u "
					   "inbound_pkts=%u inbound_cache_pkts=%u inbound_bytes=%llu "
					   "outbound_pkts=%u outbound_cache_pkts=%u outbound_bytes=%llu\n",
					   i, NatBase->m_cache[i].ipv6_ct.all_pkts_stats_cnt_index,
					   stats.num_pkts_inbound, stats.num_pkts_cache_inbound,
					   stats.num_bytes_inbound,
					   stats.num_pkts_outbound, stats.num_pkts_cache_outbound,
					   stats.num_bytes_outbound);

				conn_sync->flow_tx_packet_count         = stats.num_pkts_inbound;
				conn_sync->flow_tx_byte_count           = stats.num_bytes_inbound;
				conn_sync->flow_tx_packet_count_cache   = stats.num_pkts_cache_inbound;
				conn_sync->return_rx_packet_count       = stats.num_pkts_inbound;
				conn_sync->return_rx_byte_count         = stats.num_bytes_inbound;
				conn_sync->return_rx_packet_count_cache = stats.num_pkts_cache_inbound;
				conn_sync->flow_rx_packet_count         = stats.num_pkts_outbound;
				conn_sync->flow_rx_byte_count           = stats.num_bytes_outbound;
				conn_sync->flow_rx_packet_count_cache   = stats.num_pkts_cache_outbound;
				conn_sync->return_tx_packet_count       = stats.num_pkts_outbound;
				conn_sync->return_tx_byte_count         = stats.num_bytes_outbound;
				conn_sync->return_tx_packet_count_cache = stats.num_pkts_cache_outbound;

				IPA_BE_DBG("Collected stats for entry %d: idx=%u\n"
					   "flow_rx_pkts=%u flow_rx_bytes=%u flow_rx_pkt_cache=%u\n"
					   "flow_tx_pkts=%u flow_tx_bytes=%u flow_tx_pkt_cache=%u\n"
					   "return_rx_pkts=%u return_rx_bytes=%u return_rx_pkt_cache=%u\n"
					   "return_tx_pkts=%u return_tx_bytes=%u return_tx_pkt_cache=%u\n",
					   i, NatBase->m_cache[i].ipv6_ct.all_pkts_stats_cnt_index,
					   conn_sync->flow_rx_packet_count,
					   conn_sync->flow_rx_byte_count,
					   conn_sync->flow_rx_packet_count_cache,
					   conn_sync->flow_tx_packet_count,
					   conn_sync->flow_tx_byte_count,
					   conn_sync->flow_tx_packet_count_cache,
					   conn_sync->return_rx_packet_count,
					   conn_sync->return_rx_byte_count,
					   conn_sync->return_rx_packet_count_cache,
					   conn_sync->return_tx_packet_count,
					   conn_sync->return_tx_byte_count,
					   conn_sync->return_tx_packet_count_cache);
			}
		}

		/* Poll path always emits STATS; FLUSH_SWITCH_AE is pushed
		 * separately by ipa_be_signal_iface_offline.
		 */
		conn_sync->reason = IPA_RULE_SYNC_REASON_STATS;

		IPA_BE_DBG("IPv6 CT Entry %d: protocol=%u, src_port=%u, dest_port=%u, timestamp=%u, inc_ticks=%u, reason=%u\n",
			   i, conn_sync->protocol,
			   ntohs(conn_sync->flow_ident),
			   ntohs(conn_sync->return_ident),
			   timestamp, conn_sync->inc_ticks, conn_sync->reason);

		count++;
	}

	/* Set the response fields */
	sync_many->count = count;

	/* Find next valid entry index for continuation */
	next_index = i;
	for (; next_index < NatBase->m_maxEntries; next_index++) {
		if (NatBase->m_cache[next_index].m_enabled &&
		    NatBase->m_cache[next_index].m_type == IPA_IP_v6) {
			break;
		}
	}

	/*
	 * If we've reached the end or no more entries, set next to 0 to indicate
	 * to the frontend that IPv6 CT entries are exhausted and it needs to start over
	 */
	sync_many->next = (next_index < NatBase->m_maxEntries) ? next_index : 0;

	IPA_BE_DBG("Sync many stats response (IPv6): count=%u, next=%u\n",
		   sync_many->count, sync_many->next);

	kfree(entry_stats_cache);
	kfree(entry_stats_cache_valid);

	/* Set response status */
	msg->cm.response = IPA_CMN_RESPONSE_ACK;

	return IPA_TX_SUCCESS;
}
EXPORT_SYMBOL(ipa_sync_ipv6_stats_many_msg);

/**
 * ipa_sync_ipv4_stats_many_msg() - Synchronize statistics for multiple IPv4 NAT connections
 * @ipa_ctx: IPA context
 * @msg: The IPv4 message containing the sync request
 *
 * This function queries timestamp information from the NAT table for all active connections
 * and populates the response message with index and timestamp data.
 *
 * Return: IPA_TX_SUCCESS on success, error code on failure
 */
ipa_tx_status_t ipa_sync_ipv4_stats_many_msg(struct ipa_ctx_instance_internal *ipa_ctx, struct ipa_ipv4_msg *msg)
{
	struct ipa_ipv4_conn_sync_many_msg *sync_many;
	uint16_t start_index;
	uint16_t count = 0;
	uint16_t next_index;
	int i;
	uint32_t timestamp = 0;
	uint32_t rule_hdl;
	uint32_t old_timestamp;
	uint16_t stats_cnt_index;
	int ret;
	bool is_timestamp = true;
	bool is_stats = true;
	u32 entry_stats_cache_len = 0;
	struct ipahal_stats_nat_ct *entry_stats_cache = NULL;
	bool *entry_stats_cache_valid = NULL;

	IPA_BE_DBG("=== ENTER ipa_sync_ipv4_stats_many_msg ===\n");

	if (!msg) {
		IPA_BE_ERR("Invalid parameter: msg is NULL\n");
		return IPA_TX_FAILURE_BAD_PARAM;
	}

	if (!nat_app) {
		IPA_BE_ERR("Invalid parameter: nat_app not initialized\n");
		return IPA_TX_FAILURE_BAD_PARAM;
	}

	switch (msg->cm.type) {
	case IPA_TX_CONN_STATS_SYNC_MANY_TS_ONLY_MSG:
		is_timestamp = true;
		is_stats = false;
		break;
	case IPA_TX_CONN_STATS_SYNC_MANY_STATS_ONLY_MSG:
		is_timestamp = false;
		is_stats = true;
		break;
	case IPA_TX_CONN_STATS_SYNC_MANY_MSG:
	default:
		is_timestamp = true;
		is_stats = true;
		break;
	}

	IPA_BE_DBG("Sampling mode: type=%u is_timestamp=%d is_stats=%d\n",
		   msg->cm.type, is_timestamp, is_stats);

	IPA_BE_DBG("Input validation passed: msg=%p, nat_app=%p\n", msg, nat_app);

	sync_many = &msg->msg.conn_stats_many;
	start_index = sync_many->index;

	IPA_BE_DBG("Sync many stats request: start_index=%u, size=%u\n",
		   start_index, sync_many->size);

	/* Validate NAT table handle */
	if (!nat_app->nat_table_hdl) {
		IPA_BE_ERR("NAT table not initialized (handle=0)\n");
		return IPA_TX_FAILURE_NOT_READY;
	}

	IPA_BE_DBG("NAT table info: nat_table_hdl=0x%x, max_entries=%d, curCnt=%d\n",
		   nat_app->nat_table_hdl, nat_app->max_entries, nat_app->curCnt);

	/* Validate start index */
	if (start_index >= nat_app->max_entries) {
		IPA_BE_ERR("Invalid start_index=%u (max_entries=%d)\n",
			   start_index, nat_app->max_entries);
		return IPA_TX_FAILURE_BAD_PARAM;
	}

	/* Calculate maximum number of entries that can fit in the response */
	uint16_t max_entries = (sync_many->size - sizeof(struct ipa_ipv4_conn_sync_many_msg)) /
			       sizeof(struct ipa_ipv4_conn_sync);

	IPA_BE_DBG("Buffer capacity: max_entries=%u (buffer_size=%u, header_size=%zu, entry_size=%zu)\n",
		   max_entries, sync_many->size,
		   sizeof(struct ipa_ipv4_conn_sync_many_msg),
		   sizeof(struct ipa_ipv4_conn_sync));

	if (max_entries == 0) {
		IPA_BE_ERR("Buffer size too small for any entries (size=%u)\n", sync_many->size);
		return IPA_TX_FAILURE_TOO_SHORT;
	}

	IPA_BE_DBG("Starting iteration from index %u to %d (buffer capacity=%u)\n",
		   start_index, nat_app->max_entries, max_entries);

	if (is_timestamp) {
		IPA_BE_DBG("Flushing timestamp for nat table hdl=%u\n",
			nat_app->nat_table_hdl);
		ret = ipa_nat_timestamp_flush(nat_app->nat_table_hdl);
		if (ret < 0) {
			IPA_BE_ERR("Failed to flush NAT timestamp, hdl=%u ret=%d\n",
				nat_app->nat_table_hdl, ret);
			return IPA_TX_FAILURE;
		}
	}

	if (is_stats) {
		mutex_lock(&ipa_be_cum_lock);
		entry_stats_cache_len = ipa_be_cum_n;
		mutex_unlock(&ipa_be_cum_lock);
		entry_stats_cache = kcalloc(entry_stats_cache_len,
					    sizeof(*entry_stats_cache), GFP_KERNEL);
		entry_stats_cache_valid = kcalloc(entry_stats_cache_len,
						  sizeof(bool), GFP_KERNEL);
		if (!entry_stats_cache || !entry_stats_cache_valid) {
			IPA_BE_ERR("Failed to allocate stats cache (ipa_be_cum_n=%u)\n",
				   entry_stats_cache_len);
			kfree(entry_stats_cache);
			kfree(entry_stats_cache_valid);
			return IPA_TX_FAILURE;
		}
	}

	/* Iterate through NAT table entries starting from the requested index */
	for (i = start_index; i < nat_app->max_entries && count < max_entries; i++) {
		/* Log progress every 1000 entries */
		if ((i - start_index) > 0 && (i - start_index) % 1000 == 0) {
			IPA_BE_DBG("Progress: scanned %d entries, collected %u stats so far\n",
				   i - start_index, count);
		}

		/* Lock to safely check entry state and read rule handle */
		mutex_lock(&nat_app->cache_lock);

		/* Skip empty or disabled entries */
		if (nat_app->cache[i].private_ip == 0) {
			mutex_unlock(&nat_app->cache_lock);
			//IPA_BE_DBG("Skipping empty entry at index %d\n", i);
			continue;
		}

		if (!nat_app->cache[i].enabled) {
			IPA_BE_DBG("Skipping disabled entry at index %d (private_ip=%pI4n, rule_hdl=%u)\n",
				   i, &nat_app->cache[i].private_ip, nat_app->cache[i].rule_hdl);
			mutex_unlock(&nat_app->cache_lock);
			continue;
		}

		/* Read rule handle while holding lock */
		rule_hdl = nat_app->cache[i].rule_hdl;

		IPA_BE_DBG("Querying timestamp for entry %d: rule_hdl=%u, private_ip=%pI4n:%u -> target_ip=%pI4n:%u\n",
			   i, rule_hdl,
			   &nat_app->cache[i].private_ip, nat_app->cache[i].private_port,
			   &nat_app->cache[i].target_ip, nat_app->cache[i].target_port);

		mutex_unlock(&nat_app->cache_lock);

		if (is_timestamp) {
			/* Query timestamp for this NAT entry (without holding lock) */
			ret = ipa_nat_query_timestamp(nat_app->nat_table_hdl,
						      rule_hdl,
						      &timestamp);
			if (ret) {
				IPA_BE_ERR("Failed to query timestamp for entry %d: rule_hdl=%u, ret=%d\n",
					   i, rule_hdl, ret);
				mutex_lock(&nat_app->cache_lock);
				IPA_BE_DBG("  Entry: private_ip=%pI4n:%u, target_ip=%pI4n:%u, protocol=%u, pdn_index=%u\n",
					   &nat_app->cache[i].private_ip, nat_app->cache[i].private_port,
					   &nat_app->cache[i].target_ip, nat_app->cache[i].target_port,
					   nat_app->cache[i].protocol, nat_app->cache[i].pdn_index);
				mutex_unlock(&nat_app->cache_lock);
				/* Continue to next entry instead of failing completely */
				continue;
			}
		}

		/* Populate the connection sync structure */
		struct ipa_ipv4_conn_sync *conn_sync = &sync_many->conn_sync[count];
		memset(conn_sync, 0, sizeof(*conn_sync));

		/* Set the index to the NAT table index */
		conn_sync->index = i;

		/* Lock to safely read cache data and update timestamp */
		mutex_lock(&nat_app->cache_lock);

		/* Read conn_tracking under lock before unlock */
		bool entry_is_ct = nat_app->cache[i].conn_tracking;

		/* Set protocol and addresses */
		conn_sync->protocol = nat_app->cache[i].protocol;
		conn_sync->flow_ip = htonl(nat_app->cache[i].private_ip);
		/*
		 * CT (LAN2LAN) performs no address translation and caches no
		 * public IP, so report the flow/private IP as the "xlate"
		 * address (matching IPv6 CT behavior). LAN2WAN NAT keeps
		 * reporting the cached public IP.
		 */
		conn_sync->flow_ip_xlate = entry_is_ct ?
			htonl(nat_app->cache[i].private_ip) :
			htonl(nat_app->cache[i].public_ip);
		conn_sync->return_ip = htonl(nat_app->cache[i].target_ip);
		conn_sync->return_ip_xlate = htonl(nat_app->cache[i].target_ip);

		/* Set ports */
		conn_sync->flow_ident = htons(nat_app->cache[i].private_port);
		conn_sync->flow_ident_xlate = htons(nat_app->cache[i].public_port);
		conn_sync->return_ident = htons(nat_app->cache[i].target_port);
		conn_sync->return_ident_xlate = htons(nat_app->cache[i].target_port);
		conn_sync->flow_rule_id = nat_app->cache[i].flow_rule_id;
		if (is_timestamp) {
			/* Only set inc_ticks if timestamp has changed (entry is active) */
			old_timestamp = nat_app->cache[i].timestamp;
			if (timestamp != old_timestamp) {
				conn_sync->inc_ticks = timestamp - old_timestamp;
				/* Cache the new timestamp for next query */
				nat_app->cache[i].timestamp = timestamp;
			}
			/* If timestamp hasn't changed, leave inc_ticks as 0 (entry is inactive) */
		}

		stats_cnt_index = nat_app->cache[i].all_pkts_stats_cnt_index;

		mutex_unlock(&nat_app->cache_lock);

		if (is_stats && stats_cnt_index > 0) {
			struct ipahal_stats_nat_ct stats = {};

			ret = get_ct_stats_cached(stats_cnt_index,
						  entry_stats_cache, entry_stats_cache_valid,
						  entry_stats_cache_len, &stats);
			if (ret) {
				IPA_BE_ERR("Failed to get stats for NAT entry %d: idx=%u, ret=%d\n",
					   i, stats_cnt_index, ret);
			} else {
				IPA_BE_DBG("Rule %d stats: idx=%u "
					   "inbound_pkts=%u inbound_cache_pkts=%u inbound_bytes=%llu "
					   "outbound_pkts=%u outbound_cache_pkts=%u outbound_bytes=%llu\n",
					   i, stats_cnt_index,
					   stats.num_pkts_inbound, stats.num_pkts_cache_inbound,
					   stats.num_bytes_inbound,
					   stats.num_pkts_outbound, stats.num_pkts_cache_outbound,
					   stats.num_bytes_outbound);

				conn_sync->flow_tx_packet_count         = stats.num_pkts_inbound;
				conn_sync->flow_tx_byte_count           = stats.num_bytes_inbound;
				conn_sync->flow_tx_packet_count_cache   = stats.num_pkts_cache_inbound;
				conn_sync->return_rx_packet_count       = stats.num_pkts_inbound;
				conn_sync->return_rx_byte_count         = stats.num_bytes_inbound;
				conn_sync->return_rx_packet_count_cache = stats.num_pkts_cache_inbound;
				conn_sync->flow_rx_packet_count         = stats.num_pkts_outbound;
				conn_sync->flow_rx_byte_count           = stats.num_bytes_outbound;
				conn_sync->flow_rx_packet_count_cache   = stats.num_pkts_cache_outbound;
				conn_sync->return_tx_packet_count       = stats.num_pkts_outbound;
				conn_sync->return_tx_byte_count         = stats.num_bytes_outbound;
				conn_sync->return_tx_packet_count_cache = stats.num_pkts_cache_outbound;

				IPA_BE_DBG("Collected stats for entry %d: idx=%u%s\n"
					   "flow_rx_pkts=%u flow_rx_bytes=%u flow_rx_pkt_cache=%u\n"
					   "flow_tx_pkts=%u flow_tx_bytes=%u flow_tx_pkt_cache=%u\n"
					   "return_rx_pkts=%u return_rx_bytes=%u return_rx_pkt_cache=%u\n"
					   "return_tx_pkts=%u return_tx_bytes=%u return_tx_pkt_cache=%u\n",
					   i, stats_cnt_index,
					   entry_is_ct ? " (CT)" : "",
					   conn_sync->flow_rx_packet_count,
					   conn_sync->flow_rx_byte_count,
					   conn_sync->flow_rx_packet_count_cache,
					   conn_sync->flow_tx_packet_count,
					   conn_sync->flow_tx_byte_count,
					   conn_sync->flow_tx_packet_count_cache,
					   conn_sync->return_rx_packet_count,
					   conn_sync->return_rx_byte_count,
					   conn_sync->return_rx_packet_count_cache,
					   conn_sync->return_tx_packet_count,
					   conn_sync->return_tx_byte_count,
					   conn_sync->return_tx_packet_count_cache);
			}
		}

		/* Poll path always emits STATS; FLUSH_SWITCH_AE is pushed
		 * separately by ipa_be_signal_iface_offline.
		 */
		conn_sync->reason = IPA_RULE_SYNC_REASON_STATS;

		IPA_BE_DBG("Successfully added entry %d to response[%u]: protocol=%u, flow_ip=%pI4h:%u, flow_ip_xlate=%pI4h:%u, return_ip=%pI4h:%u, return_ip_xlate=%pI4h:%u, timestamp=%u, inc_ticks=%u, reason=%u, pdn_index=%u\n",
			   i, count, conn_sync->protocol,
			   &conn_sync->flow_ip, ntohs(conn_sync->flow_ident),
			   &conn_sync->flow_ip_xlate, ntohs(conn_sync->flow_ident_xlate),
			   &conn_sync->return_ip, ntohs(conn_sync->return_ident),
			   &conn_sync->return_ip_xlate, ntohs(conn_sync->return_ident_xlate),
			   timestamp, conn_sync->inc_ticks, conn_sync->reason, nat_app->cache[i].pdn_index);

		count++;
	}

	/* Set the response fields */
	sync_many->count = count;

	IPA_BE_DBG("Iteration complete: stopped at index %d, collected %u entries\n", i, count);

	/* Find next valid entry index for continuation */
	next_index = i;
	for (; next_index < nat_app->max_entries; next_index++) {
		if (nat_app->cache[next_index].private_ip != 0 &&
		    nat_app->cache[next_index].enabled) {
			IPA_BE_DBG("Found next valid entry at index %u\n", next_index);
			break;
		}
	}

	/*
	 * If we've reached the end or no more entries, set next to 0 to indicate
	 * to the frontend that NAT entries are exhausted and it needs to start over
	 */
	sync_many->next = (next_index < nat_app->max_entries) ? next_index : 0;

	if (sync_many->next == 0) {
		IPA_BE_DBG("No more entries available, signaling completion (next=0)\n");
	}

	IPA_BE_DBG("Sync many stats response: count=%u, next=%u\n",
		   sync_many->count, sync_many->next);

	/* Set response status */
	msg->cm.response = IPA_CMN_RESPONSE_ACK;

	IPA_BE_DBG("=== EXIT ipa_sync_ipv4_stats_many_msg: SUCCESS ===\n");
	IPA_BE_DBG("Summary: start_index=%u, end_index=%d, scanned=%d, collected=%u, next=%u\n",
		   start_index, i, i - start_index, count, sync_many->next);

	kfree(entry_stats_cache);
	kfree(entry_stats_cache_valid);
	return IPA_TX_SUCCESS;
}
EXPORT_SYMBOL(ipa_sync_ipv4_stats_many_msg);

#define IPA_BE_IFACE_ANY (-1)
#define IPA_BE_FLUSH_MAX_PASSES 3

/* iface_idx -> ep, populated on ipa_be_signal_iface_online() while the iface
 * is still registered. ipa_be_signal_iface_offline() falls back to this when
 * the live intf_list lookup misses, since ipa_deregister_intf() already
 * dropped the iface before the disconnect signal reaches it. Separate lock
 * from ipa3_ctx->lock, which ipa_rc_test_cleanup_rules() also takes.
 */
#define IPA_BE_IFACE_EP_CACHE_SIZE 8
static struct {
	int iface_idx;
	int ep;
	bool valid;
} ipa_be_iface_ep_cache[IPA_BE_IFACE_EP_CACHE_SIZE];
static int ipa_be_iface_ep_cache_next;
static DEFINE_MUTEX(ipa_be_iface_ep_cache_lock);

static void ipa_be_iface_ep_cache_put(int iface_idx, int ep)
{
	int i, slot = -1;

	if (iface_idx == IPA_BE_IFACE_ANY)
		return;

	mutex_lock(&ipa_be_iface_ep_cache_lock);
	for (i = 0; i < IPA_BE_IFACE_EP_CACHE_SIZE; i++) {
		if (ipa_be_iface_ep_cache[i].valid &&
		    ipa_be_iface_ep_cache[i].iface_idx == iface_idx) {
			slot = i;
			break;
		}
		if (!ipa_be_iface_ep_cache[i].valid && slot < 0)
			slot = i;
	}

	if (slot < 0) {
		slot = ipa_be_iface_ep_cache_next;
		ipa_be_iface_ep_cache_next =
			(ipa_be_iface_ep_cache_next + 1) % IPA_BE_IFACE_EP_CACHE_SIZE;
	}

	ipa_be_iface_ep_cache[slot].iface_idx = iface_idx;
	ipa_be_iface_ep_cache[slot].ep = ep;
	ipa_be_iface_ep_cache[slot].valid = true;
	mutex_unlock(&ipa_be_iface_ep_cache_lock);
}

static int ipa_be_iface_ep_cache_get(int iface_idx)
{
	int i, ep = -1;

	mutex_lock(&ipa_be_iface_ep_cache_lock);
	for (i = 0; i < IPA_BE_IFACE_EP_CACHE_SIZE; i++) {
		if (ipa_be_iface_ep_cache[i].valid &&
		    ipa_be_iface_ep_cache[i].iface_idx == iface_idx) {
			ep = ipa_be_iface_ep_cache[i].ep;
			break;
		}
	}
	mutex_unlock(&ipa_be_iface_ep_cache_lock);

	return ep;
}

static void ipa_be_iface_ep_cache_evict(int iface_idx)
{
	int i;

	mutex_lock(&ipa_be_iface_ep_cache_lock);
	for (i = 0; i < IPA_BE_IFACE_EP_CACHE_SIZE; i++) {
		if (ipa_be_iface_ep_cache[i].valid &&
		    ipa_be_iface_ep_cache[i].iface_idx == iface_idx) {
			ipa_be_iface_ep_cache[i].valid = false;
			break;
		}
	}
	mutex_unlock(&ipa_be_iface_ep_cache_lock);
}

static inline bool ipa_be_entry_iface_match_v4(const nat_table_entry *e, int iface_idx)
{
	return iface_idx == IPA_BE_IFACE_ANY ||
		e->flow_iface == iface_idx || e->return_iface == iface_idx;
}

static inline bool ipa_be_entry_iface_match_v6(const struct ipv6_ct_entry *e, int iface_idx)
{
	return iface_idx == IPA_BE_IFACE_ANY ||
		e->flow_iface == iface_idx || e->return_iface == iface_idx;
}

static int ipa_be_flush_flows(int iface_idx)
{
	int i;
	int v4_count = 0;
	int v6_count = 0;
	int max;

	/* Per matching entry: snapshot under cache_lock, capture HW stats
	 * (timestamp before counters, per HW/driver sequence), drop
	 * nat_mgmt_lock, call destroy_rule (FSE + backhaul + QoS + filters
	 * + HW NAT/CT delete + PDN mgmt), then push FLUSH_SWITCH_AE to FE
	 * with the captured stats. All flows on the iface are torn down
	 * unconditionally; ECM handles the SFE transition via the defunct
	 * path (see ecm_front_end_common_connection_defunct_check).
	 */

	/* v4 pass */
	mutex_lock(&ipa_be_nat_mgmt_lock);
	if (!nat_app || !nat_app->cache || !nat_app->nat_table_hdl) {
		mutex_unlock(&ipa_be_nat_mgmt_lock);
		goto v6_path;
	}
	max = nat_app->max_entries;
	mutex_unlock(&ipa_be_nat_mgmt_lock);

	for (i = 0; i < max; i++) {
		nat_table_entry local;
		struct ipahal_stats_nat_ct stats = {};
		uint32_t ts __maybe_unused = 0;
		struct ipa_ipv4_rule_destroy_msg dmsg;
		ipa_ipv4_msg_callback_t push_cb;
		bool lan2lan;
		bool is_downlink;
		bool got = false;

		mutex_lock(&ipa_be_nat_mgmt_lock);
		if (!nat_app || !nat_app->cache || !nat_app->nat_table_hdl) {
			mutex_unlock(&ipa_be_nat_mgmt_lock);
			break;
		}
		mutex_lock(&nat_app->cache_lock);
		if (nat_app->cache[i].private_ip != 0 &&
		    nat_app->cache[i].enabled &&
		    ipa_be_entry_iface_match_v4(&nat_app->cache[i], iface_idx)) {
			local = nat_app->cache[i];
			got = true;
		}
		mutex_unlock(&nat_app->cache_lock);
		if (!got) {
			mutex_unlock(&ipa_be_nat_mgmt_lock);
			continue;
		}

		/* Capture final HW stats: timestamp first, then per-flow
		 * counters (HW/driver-mandated sequence).
		 */
		IPA_ACTIVE_CLIENTS_INC_SIMPLE();
		ipa_nat_query_timestamp(nat_app->nat_table_hdl, local.rule_hdl, &ts);
		if (local.all_pkts_stats_cnt_index > 0)
			ipa_get_nat_ct_stats(local.all_pkts_stats_cnt_index, &stats);
		IPA_ACTIVE_CLIENTS_DEC_SIMPLE();
		mutex_unlock(&ipa_be_nat_mgmt_lock);

		lan2lan = (local.flow_iface == local.flow_top_iface &&
			   local.return_iface == local.return_top_iface);
		is_downlink = !lan2lan &&
			(local.flow_iface == local.flow_top_iface);

		/* Synthesize destroy msg. LAN2LAN uses the cached pre-swap
		 * tuple with no xlate; LAN2WAN uses the NAT-normalized tuple
		 * with proper xlate.
		 */
		memset(&dmsg, 0, sizeof(dmsg));
		if (lan2lan) {
			dmsg.tuple.flow_ip             = local.orig_flow_ip;
			dmsg.tuple.return_ip           = local.orig_return_ip;
			dmsg.tuple.flow_ident          = local.orig_flow_ident;
			dmsg.tuple.return_ident        = local.orig_return_ident;
			dmsg.conn_rule.flow_ip_xlate   = local.orig_flow_ip;
			dmsg.conn_rule.return_ip_xlate = local.orig_return_ip;
		} else {
			dmsg.tuple.flow_ip             = htonl(local.private_ip);
			dmsg.tuple.return_ip           = htonl(local.target_ip);
			dmsg.tuple.flow_ident          = htons(local.private_port);
			dmsg.tuple.return_ident        = htons(local.target_port);
			dmsg.conn_rule.flow_ip_xlate   = htonl(local.public_ip);
			dmsg.conn_rule.return_ip_xlate = htonl(local.target_ip);
		}
		dmsg.tuple.protocol                     = local.protocol;
		dmsg.conn_rule.flow_interface_num       = local.flow_iface;
		dmsg.conn_rule.return_interface_num     = local.return_iface;
		dmsg.conn_rule.flow_top_interface_num   = local.flow_top_iface;
		dmsg.conn_rule.return_top_interface_num = local.return_top_iface;
		/* v4 nat_table_entry doesn't cache MACs; destroy_rule's
		 * MAC-keyed QoS cleanup no-ops on the zeroed
		 * dmsg.conn_rule.{flow,return}_mac.
		 */

		ipa_ipv4_destroy_rule(&dmsg);
		v4_count++;

		/* Push FLUSH_SWITCH_AE to FE with the pre-teardown HW stats.
		 * Tuple/stats orientation is handled below: cs is built
		 * UL-normalized for both UL and DL (see ECM-lookup note), so
		 * both share the same inverted stats mapping vs. LAN2LAN.
		 */
		if (!g_ipa_ctx)
			continue;
		rcu_read_lock();
		push_cb = rcu_dereference(g_ipa_ctx->ipv4_stats_sync_cb);
		if (push_cb) {
			struct ipa_ipv4_conn_sync cs = {};
			struct ipa_ipv4_msg nim;

			cs.protocol = local.protocol;
			if (lan2lan) {
				cs.flow_ip            = local.orig_flow_ip;
				cs.flow_ip_xlate      = local.orig_flow_ip;
				cs.return_ip          = local.orig_return_ip;
				cs.return_ip_xlate    = local.orig_return_ip;
				cs.flow_ident         = local.orig_flow_ident;
				cs.flow_ident_xlate   = local.orig_flow_ident;
				cs.return_ident       = local.orig_return_ident;
				cs.return_ident_xlate = local.orig_return_ident;
			} else if (is_downlink) {
				/* UL-normalized (flow_ip = private) so ECM's
				 * ipa_uplink_case = ECM_IP_ADDR_MATCH(flow_ip,
				 * from_ip) matches the sync convention and
				 * counters are attributed to the correct
				 * direction. Mirrors the v4 poll path.
				 */
				cs.flow_ip            = htonl(local.private_ip);
				cs.flow_ip_xlate      = htonl(local.public_ip);
				cs.return_ip          = htonl(local.target_ip);
				cs.return_ip_xlate    = htonl(local.target_ip);
				cs.flow_ident         = htons(local.private_port);
				cs.flow_ident_xlate   = htons(local.public_port);
				cs.return_ident       = htons(local.target_port);
				cs.return_ident_xlate = htons(local.target_port);
			} else {
				cs.flow_ip            = htonl(local.private_ip);
				cs.flow_ip_xlate      = htonl(local.public_ip);
				cs.return_ip          = htonl(local.target_ip);
				cs.return_ip_xlate    = htonl(local.target_ip);
				cs.flow_ident         = htons(local.private_port);
				cs.flow_ident_xlate   = htons(local.public_port);
				cs.return_ident       = htons(local.target_port);
				cs.return_ident_xlate = htons(local.target_port);
			}
			/* Stats direction: LAN2LAN direct; UL and DL both use
			 * the UL-normalized tuple above, so both map with the
			 * UL orientation here.
			 */
			if (lan2lan) {
				cs.flow_tx_packet_count         = stats.num_pkts_outbound;
				cs.flow_tx_byte_count           = stats.num_bytes_outbound;
				cs.flow_tx_packet_count_cache   = stats.num_pkts_cache_outbound;
				cs.return_rx_packet_count       = stats.num_pkts_outbound;
				cs.return_rx_byte_count         = stats.num_bytes_outbound;
				cs.return_rx_packet_count_cache = stats.num_pkts_cache_outbound;
				cs.flow_rx_packet_count         = stats.num_pkts_inbound;
				cs.flow_rx_byte_count           = stats.num_bytes_inbound;
				cs.flow_rx_packet_count_cache   = stats.num_pkts_cache_inbound;
				cs.return_tx_packet_count       = stats.num_pkts_inbound;
				cs.return_tx_byte_count         = stats.num_bytes_inbound;
				cs.return_tx_packet_count_cache = stats.num_pkts_cache_inbound;
			} else {
				cs.flow_tx_packet_count         = stats.num_pkts_inbound;
				cs.flow_tx_byte_count           = stats.num_bytes_inbound;
				cs.flow_tx_packet_count_cache   = stats.num_pkts_cache_inbound;
				cs.return_rx_packet_count       = stats.num_pkts_inbound;
				cs.return_rx_byte_count         = stats.num_bytes_inbound;
				cs.return_rx_packet_count_cache = stats.num_pkts_cache_inbound;
				cs.flow_rx_packet_count         = stats.num_pkts_outbound;
				cs.flow_rx_byte_count           = stats.num_bytes_outbound;
				cs.flow_rx_packet_count_cache   = stats.num_pkts_cache_outbound;
				cs.return_tx_packet_count       = stats.num_pkts_outbound;
				cs.return_tx_byte_count         = stats.num_bytes_outbound;
				cs.return_tx_packet_count_cache = stats.num_pkts_cache_outbound;
			}
			cs.inc_ticks    = 0;
			cs.flow_rule_id = local.flow_rule_id;
			cs.reason       = IPA_RULE_SYNC_REASON_FLUSH_SWITCH_AE;

			memset(&nim, 0, sizeof(nim));
			ipa_ipv4_msg_init(&nim, IPA_SPECIAL_INTERFACE_IPV4,
					  IPA_RX_CONN_STATS_SYNC_MSG,
					  sizeof(nim), NULL, NULL);
			nim.msg.conn_stats = cs;
			push_cb(g_ipa_ctx->ipv4_stats_sync_data, &nim);
		}
		rcu_read_unlock();
	}

v6_path:
	/* v6 pass: same shape as v4. */
	mutex_lock(&ipa_be_nat_mgmt_lock);
	if (!NatBase || !NatBase->m_cache || !NatBase->ct_table_hdl) {
		mutex_unlock(&ipa_be_nat_mgmt_lock);
		goto out;
	}
	max = NatBase->m_maxEntries;
	mutex_unlock(&ipa_be_nat_mgmt_lock);
	for (i = 0; i < max; i++) {
		struct ipv6_ct_entry local6;
		u8 proto = 0;
		struct ipahal_stats_nat_ct stats6 = {};
		uint32_t ts6 __maybe_unused = 0;
		struct ipa_ipv6_rule_destroy_msg dmsg6;
		ipa_ipv6_msg_callback_t push_cb6;
		bool lan2lan6;
		bool got6 = false;

		mutex_lock(&ipa_be_nat_mgmt_lock);
		if (!NatBase || !NatBase->m_cache || !NatBase->ct_table_hdl) {
			mutex_unlock(&ipa_be_nat_mgmt_lock);
			break;
		}
		mutex_lock(&NatBase->cache_lock);
		if (NatBase->m_cache[i].m_enabled &&
		    NatBase->m_cache[i].m_type == IPA_IP_v6 &&
		    ipa_be_entry_iface_match_v6(&NatBase->m_cache[i].ipv6_ct,
						iface_idx)) {
			local6 = NatBase->m_cache[i].ipv6_ct;
			proto  = NatBase->m_cache[i].m_protocol;
			got6   = true;
		}
		mutex_unlock(&NatBase->cache_lock);
		if (!got6) {
			mutex_unlock(&ipa_be_nat_mgmt_lock);
			continue;
		}

		/* Capture final HW stats: timestamp first, then counters. */
		IPA_ACTIVE_CLIENTS_INC_SIMPLE();
		ipa_ct_query_timestamp(NatBase->ct_table_hdl,
				       local6.rule_handle, &ts6);
		if (local6.all_pkts_stats_cnt_index > 0)
			ipa_get_nat_ct_stats(local6.all_pkts_stats_cnt_index,
					     &stats6);
		IPA_ACTIVE_CLIENTS_DEC_SIMPLE();
		mutex_unlock(&ipa_be_nat_mgmt_lock);

		lan2lan6 = (local6.flow_iface == local6.flow_top_iface &&
			    local6.return_iface == local6.return_top_iface);

		/* Synthesize destroy msg. LAN2LAN uses the cached pre-swap
		 * tuple; LAN2WAN rebuilds from the CT entry's src/dest ipv6
		 * (add-time UL labeling: src=flow, dest=return).
		 */
		memset(&dmsg6, 0, sizeof(dmsg6));
		if (lan2lan6) {
			memcpy(dmsg6.tuple.flow_ip, local6.orig_flow_ip,
			       sizeof(dmsg6.tuple.flow_ip));
			memcpy(dmsg6.tuple.return_ip, local6.orig_return_ip,
			       sizeof(dmsg6.tuple.return_ip));
			dmsg6.tuple.flow_ident   = local6.orig_flow_ident;
			dmsg6.tuple.return_ident = local6.orig_return_ident;
		} else {
			dmsg6.tuple.flow_ip[0]   = htonl((u32)(local6.src_ipv6_msb >> 32));
			dmsg6.tuple.flow_ip[1]   = htonl((u32)(local6.src_ipv6_msb & 0xFFFFFFFF));
			dmsg6.tuple.flow_ip[2]   = htonl((u32)(local6.src_ipv6_lsb >> 32));
			dmsg6.tuple.flow_ip[3]   = htonl((u32)(local6.src_ipv6_lsb & 0xFFFFFFFF));
			dmsg6.tuple.return_ip[0] = htonl((u32)(local6.dest_ipv6_msb >> 32));
			dmsg6.tuple.return_ip[1] = htonl((u32)(local6.dest_ipv6_msb & 0xFFFFFFFF));
			dmsg6.tuple.return_ip[2] = htonl((u32)(local6.dest_ipv6_lsb >> 32));
			dmsg6.tuple.return_ip[3] = htonl((u32)(local6.dest_ipv6_lsb & 0xFFFFFFFF));
			dmsg6.tuple.flow_ident   = htons(local6.src_port);
			dmsg6.tuple.return_ident = htons(local6.dest_port);
		}
		dmsg6.tuple.protocol                     = proto;
		dmsg6.conn_rule.flow_interface_num       = local6.flow_iface;
		dmsg6.conn_rule.return_interface_num     = local6.return_iface;
		dmsg6.conn_rule.flow_top_interface_num   = local6.flow_top_iface;
		dmsg6.conn_rule.return_top_interface_num = local6.return_top_iface;
		memcpy(dmsg6.conn_rule.flow_mac, local6.flow_mac, 6);
		memcpy(dmsg6.conn_rule.return_mac, local6.return_mac, 6);

		ipa_ipv6_destroy_rule(&dmsg6);
		v6_count++;

		/* Push FLUSH_SWITCH_AE to FE with pre-teardown HW stats.
		 * Mirror the v6 poll path (ipa_sync_ipv6_stats_many_msg):
		 * cs6.flow_ip = dmsg6.tuple.flow_ip (= src_ipv6, canonicalized
		 * to the LAN client) regardless of UL/DL, and stats always map
		 * flow_tx=inbound / flow_rx=outbound. ECM v6 (ecm_ipa_ipv6.c)
		 * looks up connections by flow_rule_id and infers direction
		 * from ECM_IP_ADDR_MATCH(flow_ip, from_ip); a DL-flipped push
		 * would break that inference and mis-attribute counters.
		 */
		if (!g_ipa_ctx)
			continue;
		rcu_read_lock();
		push_cb6 = rcu_dereference(g_ipa_ctx->ipv6_stats_sync_cb);
		if (push_cb6) {
			struct ipa_ipv6_conn_sync cs6 = {};
			struct ipa_ipv6_msg nim6;

			cs6.protocol = proto;
			memcpy(cs6.flow_ip, dmsg6.tuple.flow_ip,
			       sizeof(cs6.flow_ip));
			memcpy(cs6.flow_ip_xlate, dmsg6.tuple.flow_ip,
			       sizeof(cs6.flow_ip_xlate));
			memcpy(cs6.return_ip, dmsg6.tuple.return_ip,
			       sizeof(cs6.return_ip));
			memcpy(cs6.return_ip_xlate, dmsg6.tuple.return_ip,
			       sizeof(cs6.return_ip_xlate));
			cs6.flow_ident         = dmsg6.tuple.flow_ident;
			cs6.flow_ident_xlate   = dmsg6.tuple.flow_ident;
			cs6.return_ident       = dmsg6.tuple.return_ident;
			cs6.return_ident_xlate = dmsg6.tuple.return_ident;
			cs6.flow_tx_packet_count         = stats6.num_pkts_inbound;
			cs6.flow_tx_byte_count           = stats6.num_bytes_inbound;
			cs6.flow_tx_packet_count_cache   = stats6.num_pkts_cache_inbound;
			cs6.return_rx_packet_count       = stats6.num_pkts_inbound;
			cs6.return_rx_byte_count         = stats6.num_bytes_inbound;
			cs6.return_rx_packet_count_cache = stats6.num_pkts_cache_inbound;
			cs6.flow_rx_packet_count         = stats6.num_pkts_outbound;
			cs6.flow_rx_byte_count           = stats6.num_bytes_outbound;
			cs6.flow_rx_packet_count_cache   = stats6.num_pkts_cache_outbound;
			cs6.return_tx_packet_count       = stats6.num_pkts_outbound;
			cs6.return_tx_byte_count         = stats6.num_bytes_outbound;
			cs6.return_tx_packet_count_cache = stats6.num_pkts_cache_outbound;
			cs6.inc_ticks    = 0;
			cs6.flow_rule_id = local6.flow_rule_id;
			cs6.reason       = IPA_RULE_SYNC_REASON_FLUSH_SWITCH_AE;

			memset(&nim6, 0, sizeof(nim6));
			ipa_ipv6_msg_init(&nim6, IPA_SPECIAL_INTERFACE_IPV6,
					  IPA_RX_CONN_STATS_SYNC_MSG,
					  sizeof(nim6), NULL, NULL);
			nim6.msg.conn_stats = cs6;
			push_cb6(g_ipa_ctx->ipv6_stats_sync_data, &nim6);
		}
		rcu_read_unlock();
	}

out:
	IPA_BE_DBG("iface %d flush: torn down v4=%d v6=%d flows via destroy_rule\n",
		   iface_idx, v4_count, v6_count);
	return v4_count + v6_count;
}

void ipa_be_signal_iface_offline(int iface_idx)
{
	/* Resolved before the flush: the lookup needs the iface still
	 * registered, and it takes ipa3_ctx->lock, which the testcase cleanup
	 * takes as well.
	 */
	int ep = ipa_be_get_ep_for_intf(iface_idx);

	if (ep < 0)
		ep = ipa_be_iface_ep_cache_get(iface_idx);

	ipa_be_flush_flows(iface_idx);

	if (ep >= 0) {
		ipa_rc_test_cleanup_rules(ep);
		/*
		 * Order matters: the injected rules have to be gone before the
		 * latch is released, or traffic still hitting the route miss
		 * re-confirms the drop within DROP_CONFIRM_THRESH samples.
		 */
		ipa_rc_clear_drop_latch(ep);
	} else {
		IPA_BE_DBG("iface %d: no ep, testcase rules left installed\n",
			   iface_idx);
	}

	ipa_be_iface_ep_cache_evict(iface_idx);
}

void ipa_be_flush_all_flows(void)
{
	int pass;

	/* ipa_be_flush_flows() drops ipa_be_nat_mgmt_lock between entries, so
	 * a concurrent add can land in an already-scanned slot. Repeat until a
	 * pass tears nothing down.
	 */
	for (pass = 0; pass < IPA_BE_FLUSH_MAX_PASSES; pass++)
		if (!ipa_be_flush_flows(IPA_BE_IFACE_ANY))
			break;
}

void ipa_be_signal_iface_online(int iface_idx)
{
	int ep = ipa_be_get_ep_for_intf(iface_idx);

	if (ep >= 0) {
		ipa_be_iface_ep_cache_put(iface_idx, ep);
		/*
		 * Closing half of a suspend/resume recovery. The offline half
		 * already removed any injected testcase rules, so nothing
		 * justifies the latch any more and it must be released here or
		 * the anomaly stays asserted for the life of the driver. Unlike
		 * the offline half this cannot miss the endpoint: the interface
		 * is registered at this point, so ipa3_get_ep_for_intf()
		 * resolves without needing the ep cache. If a genuine fault
		 * survived the recovery it re-confirms within
		 * DROP_CONFIRM_THRESH samples and is reported as a new assert.
		 */
		ipa_rc_clear_drop_latch(ep);
	}

	IPA_BE_DBG("iface %d online\n", iface_idx);
}
