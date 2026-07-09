// SPDX-License-Identifier: GPL-2.0-only
/*
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

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
	IPA_BE_DBG("DEBUG: inside be_nat_mgmt_exit\n");

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
}

int ipa_be_nat_mgmt_init(void)
{
	int size = 0;

	IPA_BE_DBG("DEBUG: inside be_nat_mgmt_init\n");

	/* Prevent double initialization of nat_app */
	if (nat_app) {
		IPA_BE_ERR("nat_app already initialized!\n");
		return -EALREADY;
	}

	/* Initialize context */
	nat_app = kzalloc(sizeof(*nat_app), GFP_KERNEL);
	if (!nat_app) {
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

	return IPA_BE_SUCCESS;

fail:
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
	return -ENOMEM;
}

uint32_t generate_met_data(uint8_t mux_id)
{
	return (mux_id << HDR_METADATA_MUX_ID_SHFT) & HDR_METADATA_MUX_ID_BMASK;
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

int ipa_be_add_table(uint32_t pub_ip, uint8_t mux_id, bool is_sta, bool ip_pass)
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
	ret = ipa_nat_get_pdn_count(&pdn_count);
	if(ret)
	{
		IPA_BE_ERR("unable to get pdn count Error:%d\n", ret);
		return ret;
	}
	IPA_BE_DBG("AddPDN isSta:%d, pdn_count: %d, ip 0x%X\n", is_sta,pdn_count,pub_ip);

	/*
	 * If the NAT table already exists and PDN[0] holds 0xFFFFFFFF, update PDN[0] with the real public
	 * IP and metadata now that a WAN connection is available.
	 */
	if (nat_app->nat_table_hdl) {
		ret = ipa_nat_modify_dummy_pdn(nat_app->nat_table_hdl, 0, &entry);
		if (ret) {
			IPA_BE_ERR("unable to modify dummy PDN entry Error:%d\n", ret);
			return ret;
		}
	}

	if(nat_app->nat_table_hdl)
	{
		IPA_BE_DBG("nat_table_hdl already exist \n");
		/*
		 * Table already exists. If PDN[0] is the dummy LAN2LAN PDN,
		 * replace it with the real WAN public IP instead of allocating a
		 * new PDN for this LAN2WAN connection.
		 */
		if(ipa_nat_get_pdn_index(pub_ip, &pdn_index) < 0)
		{
			if (!ipa_nat_get_pdn_index(IPA_DUMMY_PDN_PUB_IP, &pdn_index) &&
			    pdn_index == 0) {
				ret = ipa_nat_modify_dummy_pdn(nat_app->nat_table_hdl,
							       pdn_index, &entry);
				if (ret) {
					IPA_BE_ERR("unable to replace dummy PDN entry Error:%d\n",
						   ret);
					return ret;
				}
				IPA_BE_DBG("replaced dummy PDN index %d with real ip 0x%X\n",
					   pdn_index, pub_ip);
			} else {
				ret = ipa_nat_alloc_pdn(&entry, &pdn_index);
				if(ret)
				{
					IPA_BE_ERR("couldn't allocate a pdn index for ip 0x%X\n", pub_ip);
					return ret;
				}
				IPA_BE_DBG("successfully allocated pdn index %d for ip 0x%X\n",
						pdn_index, pub_ip);
			}
		}
		else
		{
			IPA_BE_DBG("pdn already existed with index %d for ip 0x%X\n", pdn_index, pub_ip);
			return 0;
		}
	}
	else
	{
		/* create the NAT table, the PDN will be stored in index 0 */
		if(is_sta)
		{
			/* This function stores pub_ip at pdn[0].
			 * We don't want that for non STA PDNs*/
			ret = ipa_nat_add_ipv4_tbl(pub_ip, nat_app->mem_type, nat_app->max_entries, &nat_app->nat_table_hdl);
		}
		else
		{
			ret = ipa_nat_add_ipv4_tbl(pub_ip, nat_app->mem_type, nat_app->max_entries, &nat_app->nat_table_hdl);
		}
		if(ret)
		{
			IPA_BE_ERR("unable to create nat table Error:%d\n", ret);
			return ret;
		}
		IPA_BE_DBG("succeesfully created NAT table for ip 0x%X\n", pub_ip);
		entry.src_metadata = generate_met_data(mux_id);
		pdn_index = 0;

		/* modify PDN 0 so it will hold the mux ID in the src metadata field */
		ret = ipa_nat_modify_pdn(nat_app->nat_table_hdl, pdn_index, &entry);
		IPA_BE_DBG("modify pdn index %d\n", pdn_index);
		if(ret)
		{
			IPA_BE_ERR("unable to modify PDN 0 entry Error:%d\n", ret);
			return ret;
		}
	}

	/* now traverse nat_app->cache and add the PDN entries */
	mutex_lock(&nat_app->cache_lock);

	for(cnt = 0; cnt < nat_app->max_entries; cnt++)
	{
		if((nat_app->cache[cnt].private_ip != 0)
			/* flush only entries which are related to this PDN */
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
EXPORT_SYMBOL(ipa_be_add_table);

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
	ret = ipa_nat_get_pdn_count(&pdn_count);
	if(ret)
	{
		IPA_BE_ERR("unable to get pdn count Error:%d\n", ret);
		return ret;
	}
	IPA_BE_DBG("AddPDN isSta:%d, pdn_count: %d, ip 0x%X\n", is_sta,pdn_count,pub_ip);

	if (!nat_app->nat_table_hdl)
	{
		/* create the NAT table, the PDN will be stored in index 0 */
		if(is_sta)
		{
			/* This function stores pub_ip at pdn[0].
			 * We don't want that for non STA PDNs*/
			ret = ipa_nat_add_ipv4_tbl(pub_ip, nat_app->mem_type, nat_app->max_entries, &nat_app->nat_table_hdl);
		}
		else
		{
			ret = ipa_nat_add_ipv4_tbl(0, nat_app->mem_type, nat_app->max_entries, &nat_app->nat_table_hdl);
		}
		if(ret)
		{
			IPA_BE_ERR("unable to create nat table Error:%d\n", ret);
			return ret;
		}
		IPA_BE_DBG("succeesfully created NAT table for ip 0x%X\n", pub_ip);
		pdn_index = 1;

		/* This is the first VLAN PDN, modify index 1 to hold it */
		ret = ipa_nat_modify_pdn(nat_app->nat_table_hdl, pdn_index, &entry);
		IPA_BE_DBG("modify pdn index %d\n", pdn_index);
		if(ret)
		{
			IPA_BE_ERR("unable to modify PDN 1 entry Error:%d\n", ret);
			return ret;
		}
	}
	else
	{
		/* Table exists. Allocate a new PDN if this public_ip is new. */
		if(ipa_nat_get_pdn_index(pub_ip, &pdn_index) < 0)
		{
			ret = ipa_nat_alloc_pdn(&entry, &pdn_index);
			if(ret)
			{
				IPA_BE_ERR("couldn't allocate a pdn index\n");
				return ret;
			}
			IPA_BE_DBG("successfully allocated index %d for ip 0x%X\n",
					pdn_index, pub_ip);
		}
		else
		{
			IPA_BE_DBG("pdn already existed with index %d\n", pdn_index);
		}
	}

	/* now traverse nat_app->cache and add the PDN entries */
	mutex_lock(&nat_app->cache_lock);

	for(cnt = 0; cnt < nat_app->max_entries; cnt++)
	{
		if((nat_app->cache[cnt].private_ip != 0)
			/* flush only entries which are related to this PDN */
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

int ipa_be_addpdn(struct ipa_ipv4_rule_create_msg v4_msg, int pdn_iface, bool ct_enabled)
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
	if (ct_enabled)
	{
		pub_ip = IPA_DUMMY_PDN_PUB_IP;
	}
	else if (v4_msg.tuple.flow_ip == v4_msg.conn_rule.flow_ip_xlate &&
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

	if (v4_msg.vlan_primary_rule.egress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED||
		v4_msg.vlan_primary_rule.ingress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED)
	{
		ret = ipa_be_add_pdn(pub_ip, mux_id, is_sta, ip_pass);
		if (ret)
		{
			IPA_BE_ERR("failed adding pdn\n");
			goto cleanup;
		}
	}
	else
	{
		ret = ipa_be_add_table(pub_ip, mux_id, is_sta, ip_pass);
		if (ret)
		{
			IPA_BE_ERR("failed adding table\n");
			goto cleanup;
		}
	}

	/* For CT, determine canonical direction (smaller EP as private side)
	 * and pass a swap flag to ipa_be_add_entry — no struct copy needed.
	 */
	if (ipa_get_hw_type() >= IPA_HW_v7_0 && ct_enabled) {
		int ep_cmp = ipa_be_flow_canonical_cmp(v4_msg.conn_rule.flow_interface_num,
						v4_msg.conn_rule.return_interface_num,
						v4_msg.conn_rule.flow_mac,
						v4_msg.conn_rule.return_mac);
		if (ep_cmp < 0) {
			ret = -EINVAL;
			goto cleanup;
		}
		/* ep_cmp==0: return is canonical, swap private/target in ipa_be_add_entry */
		ret = ipa_be_add_entry(v4_msg, isVlan, ct_enabled, ep_cmp == 0);
	} else {
		ret = ipa_be_add_entry(v4_msg, isVlan, ct_enabled, false);
	}
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

    tuple = &v6_msg->tuple;

    ret = ipv6ct_convert_to_ipa_rule(v6_msg, &ipa_rule);
    if (ret) {
        IPA_BE_ERR("Failed to convert rule format: %d\n", ret);
        return ret;
    }

    if (ipa3_ctx->ipa_hw_type >= IPA_HW_v7_0) {
        ipa_ipv6ct_rule_v2 ipa_rule_v2;
        struct ipa_sw_producer_cookie cookie;

        ret = copy_from_ipa_ipv6ct_rule_v1_to_v2(&ipa_rule, &ipa_rule_v2);
        if (ret) {
            IPA_BE_ERR("Failed to copy IPv6CT rule v1 to v2: %d\n", ret);
            return ret;
        }

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
    } else {
        ret = ipa_ct_add_ipv6_rule(NatBase->ct_table_hdl, &ipa_rule, rule_handle);
    }
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
 * covers both inbound and outbound traffic for the LAN2LAN pair.
 */
static int ipv6ct_add_entry_v2(struct ipa_ipv6_rule_create_msg *v6_msg,
			       uint32_t *rule_handle,
			       uint16_t *all_pkts_counter,
			       uint16_t *non_frag_counter,
			       bool swap_src_dest)
{
    ipa_ipv6ct_rule_v2 ipa_rule_v2;
    struct ipa_ipv6_5tuple *tuple;
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
	/* Canonical (smaller-EP as src) key, used for dup check, add, and cleanup. */
	int      ep_cmp = 0;
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

	/* Canonical key: for v7.0+ LAN2LAN, smaller EP is src; otherwise flow_ip is src. */
	if ((ipa_get_hw_type() >= IPA_HW_v7_0) && lan2lan) {
		ep_cmp = ipa_be_flow_canonical_cmp(v6_msg.conn_rule.flow_interface_num,
						       v6_msg.conn_rule.return_interface_num,
						       v6_msg.conn_rule.flow_mac,
						       v6_msg.conn_rule.return_mac);
		if (ep_cmp < 0) {
			ret = -EINVAL;
			goto cleanup;
		}
		if (!ep_cmp) {
			can_src_msb = dest_ipv6_msb; can_src_lsb = dest_ipv6_lsb; can_src_port = dest_port;
			can_dst_msb = src_ipv6_msb;  can_dst_lsb = src_ipv6_lsb;  can_dst_port = src_port;
		} else {
			can_src_msb = src_ipv6_msb;  can_src_lsb = src_ipv6_lsb;  can_src_port = src_port;
			can_dst_msb = dest_ipv6_msb; can_dst_lsb = dest_ipv6_lsb; can_dst_port = dest_port;
		}
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
	if ((ipa_get_hw_type() >= IPA_HW_v7_0) && lan2lan) {
		/*
		 * swap_src_dest=false when ep_cmp=1: flow_ip is already
		 * the canonical src — no swap needed. swap_src_dest=true when
		 * ep_cmp=0: return_ip becomes the canonical src.
		 */
		ret = ipv6ct_add_entry_v2(&v6_msg, &rule_hdl_v2,
					  &all_pkts_counter, NULL,
					  !ep_cmp);
		if (ret) {
			IPA_BE_ERR("Failed to add IPv6 CT canonical entry\n");
			ret = -EFAULT;
			goto cleanup; /* entry not created; counters freed by ipv6ct_add_entry_v2 */
		}

		IPA_BE_DBG("IPv6 CT canonical entry added with hdl %d\n", rule_hdl_v2);
		cache_idx = store_ipv6_ct_entry(can_src_msb, can_src_lsb,
						can_dst_msb, can_dst_lsb,
						can_src_port, can_dst_port,
						tuple->protocol, rule_hdl_v2,
						tuple->flow_rule_id);
		if (cache_idx < 0) {
			IPA_BE_ERR("Failed to store IPv6 CT entry in cache\n");
			ret = -EFAULT;
			goto cleanup_fwd_rule;
		}
		NatBase->m_cache[cache_idx].ipv6_ct.all_pkts_stats_cnt_index = all_pkts_counter;
		NatBase->m_cache[cache_idx].ipv6_ct.non_frag_stats_cnt_index = non_frag_counter;
	} else {
		/* Legacy path (< IPA v7.0) || !lan2lan: single ALLOW_ALL entry */
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
						tuple->flow_rule_id);
		if (cache_idx < 0) {
			IPA_BE_ERR("Failed to store IPv6 CT entry in cache\n");
			ret = -EFAULT;
			goto cleanup_single_rule;
		}
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

	if (ct_enabled)
		rule->public_ip = IPA_DUMMY_PDN_PUB_IP;

	/*
	 * CT canonical direction: when ct_canonical_swap is set, return_ip is the
	 * canonical private side — swap private/target on the already-extracted rule.
	 */
	if (ct_canonical_swap) {
		uint32_t tmp_ip      = rule->private_ip;
		uint16_t tmp_port    = rule->private_port;

		rule->private_ip     = rule->target_ip;
		rule->target_ip      = tmp_ip;
		rule->private_port   = rule->target_port;
		rule->target_port    = tmp_port;
		rule->public_port    = rule->private_port; /* CT: no port NAT */
		IPA_BE_DBG("CT canonical swap: private=%pI4h:%u target=%pI4h:%u\n",
			   &rule->private_ip, (unsigned int)rule->private_port,
			   &rule->target_ip,  (unsigned int)rule->target_port);
	}

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

			/* Determine CT flags based on use case */
			bool is_ct_entry = false;
			bool ct_out_allowed = false;
			bool ct_in_allowed = false;

			if (ipa_ver >= IPA_HW_v7_0) {
				/*
				 * out_allowed=true for both NAT and CT on v7.0+.
				 * For CT (LAN2LAN): single bidirectional entry covers both directions.
				 * For NAT: out_allowed was already true (unchanged).
				 */
				ct_out_allowed = true;
				ct_in_allowed = true;
				is_ct_entry = ct_enabled;
				IPA_BE_DBG("IPA v7.0+ %s entry: in_allowed=1 out_allowed=1\n",
					   ct_enabled ? "CT" : "NAT");
			} else if (ct_enabled && ipa_ver < IPA_HW_v7_0) {
				/* LAN2LAN on IPA < v7.0: ERROR - should not create entry */
				IPA_BE_ERR("Invalid: LAN2LAN on IPA < v7.0 should not call ipa_be_add_entry\n");
				mutex_unlock(&nat_app->cache_lock);
				kfree(rule);
				return -EINVAL;
			}
			/* else: LAN2WAN uses NAT (is_ct_entry=false) on all IPA versions */

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
					 * SW producer cookie: set for NAT entries when the flow or
					 * return interface is WDI6 (vpnum_valid). For CT (LAN2LAN),
					 * the single canonical entry covers both directions so the
					 * cookie cannot distinguish per-direction producers — split
					 * CT entries are needed before this can be enabled for CT.
					 * This also means WLAN STA-to-AP case is not supported
					 * until split CT entries are introduced.
					 */
					if (!ct_enabled) {
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

					if (is_ct_entry) {
						IPA_BE_DBG("Added CT entry (v7.0+): cache[%d] hdl=%d, %pI4n:%u <-> %pI4n:%u\n",
							   cnt, nat_app->cache[cnt].rule_hdl,
							   &rule->private_ip, rule->private_port,
							   &rule->target_ip, rule->target_port);
					} else {
						IPA_BE_DBG("Added NAT entry (v7.0+): cache[%d] hdl=%d\n",
							   cnt, nat_app->cache[cnt].rule_hdl);
					}
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
			nat_app->cache[cnt].flow_rule_id = v4_msg.tuple.flow_rule_id;
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
		    nat_app->cache[cnt].public_ip == pub_ip)
			count++;
	}
	mutex_unlock(&nat_app->cache_lock);

	return count;
}

int ipa_be_remove_pdn(uint32_t pub_ip)
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

	/* remove all PDN entries */
	mutex_lock(&nat_app->cache_lock);
	for(int cnt = 0; cnt < nat_app->max_entries; cnt++)
	{
		if((nat_app->cache[cnt].pdn_index == pdn_index) &&
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

	/*
	 * PDN[0] is the primary WAN PDN. If it is being removed but CT/LAN2LAN
	 * entries that reference the dummy public IP (0xFFFFFFFF) still exist,
	 * restore PDN[0] to the dummy IP so those entries remain valid until
	 * they are explicitly deleted.
	 */
	if (pdn_index == 0 && count_nat_entries_for_pdn(IPA_DUMMY_PDN_PUB_IP) > 0)
	{
		ipa_nat_pdn_entry entry;
		/* memset zeros dst_metadata, src_metadata, and is_sta; only
		 * public_ip needs an explicit non-zero assignment below. */
		memset(&entry, 0, sizeof(entry));
		entry.public_ip = IPA_DUMMY_PDN_PUB_IP;

		ret = ipa_nat_modify_dummy_pdn(nat_app->nat_table_hdl, pdn_index, &entry);
		if (ret) {
			IPA_BE_ERR("unable to modify dummy PDN entry Error:%d\n", ret);
			return ret;
		}
	}

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

	if(!pdn_cnt)
	{
		IPA_BE_DBG("removing NAT table\n");
		ret = ipa_nat_del_ipv4_tbl(nat_app->nat_table_hdl);
		if(ret)
		{
			IPA_BE_ERR("unable to delete nat table Error: %d\n", ret);;
			return ret;
		}

		ipa_be_reset();
	}
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


void ipa_be_delete_entry(struct ipa_ipv4_rule_destroy_msg v4_msg, bool ct_enabled)
{
	nat_table_entry rule;
	struct ipa_ipv4_rule_create_msg create_msg;
	uint32_t pub_ip = 0;

	IPA_BE_DBG("Deleting IP4 NAT entry\n");
	memset(&rule, 0, sizeof(rule));
	rule.protocol = v4_msg.tuple.protocol;

	/*
	 * Reconstruct the key that was used during insertion.
	 * For CT (LAN2LAN): replay ipa_be_build_nat_rule_attrs + ct_canonical_swap
	 * to reproduce the exact key stored during ipa_be_add_entry.
	 * For NAT: the entry is always created from the uplink direction.
	 * The destroy message can come for either direction; use flow_ip_xlate
	 * to detect which direction this message represents.
	 */
	if (ct_enabled) {
		int ep_cmp;
		uint32_t tmp_ip;
		uint16_t tmp_port;

		ep_cmp = ipa_be_flow_canonical_cmp(v4_msg.conn_rule.flow_interface_num,
						   v4_msg.conn_rule.return_interface_num,
						   v4_msg.conn_rule.flow_mac,
						   v4_msg.conn_rule.return_mac);
		if (ep_cmp < 0) {
			return;
		}

		WARN_ON(v4_msg.tuple.flow_ip != v4_msg.conn_rule.flow_ip_xlate ||
			v4_msg.tuple.return_ip != v4_msg.conn_rule.return_ip_xlate);

		/*
		 * LAN2LAN has no IP translation, so flow_ip == flow_ip_xlate and
		 * return_ip == return_ip_xlate — the IPPT branch always fires.
		 * Replicate ipa_be_build_nat_rule_attrs IPPT logic to get the
		 * initial private/target assignment, then apply the same swap.
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

		/* Mirror ct_canonical_swap from ipa_be_addpdn: swap when ep_cmp==0 */
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
	} else {
		/* Reuse the add-path builder so the delete key matches the
		 * tuple stored at insertion (private_ip, target_ip, ports,
		 * protocol). ipa_be_build_nat_rule_attrs() takes a create_msg;
		 * tuple and conn_rule are shared between create and destroy
		 * messages, so a small local copy lets us call the helper
		 * without changing its signature.
		 */
		memset(&create_msg, 0, sizeof(create_msg));
		create_msg.tuple     = v4_msg.tuple;
		create_msg.conn_rule = v4_msg.conn_rule;
		ipa_be_build_nat_rule_attrs(create_msg, &rule);
	}

	/* pub_ip lets us drop the PDN once its last entry is gone (mirrors ipa_be_addpdn). */
	if (ct_enabled) {
		pub_ip = IPA_DUMMY_PDN_PUB_IP;
	} else if ((v4_msg.tuple.flow_ip == v4_msg.conn_rule.flow_ip_xlate) &&
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
		IPA_BE_DBG("Last NAT entry for PDN (pub_ip=0x%X) deleted, removing PDN\n",
			   pub_ip);
		if (ipa_be_remove_pdn(pub_ip))
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
				uint32_t flow_rule_id)
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
		int ep_cmp = ipa_be_flow_canonical_cmp(msg->conn_rule.flow_interface_num,
						msg->conn_rule.return_interface_num,
						msg->conn_rule.flow_mac,
						msg->conn_rule.return_mac);

		if (ep_cmp < 0) {
			return;
		}

		if (ep_cmp) {
			can_src_msb  = src_ipv6_msb;
			can_src_lsb  = src_ipv6_lsb;
			can_dst_msb  = dest_ipv6_msb;
			can_dst_lsb  = dest_ipv6_lsb;
			can_src_port = src_port;
			can_dst_port = dest_port;
		} else {
			can_src_msb  = dest_ipv6_msb;
			can_src_lsb  = dest_ipv6_lsb;
			can_dst_msb  = src_ipv6_msb;
			can_dst_lsb  = src_ipv6_lsb;
			can_src_port = dest_port;
			can_dst_port = src_port;
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
			mutex_unlock(&NatBase->cache_lock);
			IPA_BE_DBG("Successfully deleted IPv6 CT canonical entry\n");
		} else {
			IPA_BE_ERR("IPv6 CT canonical entry not found in cache for deletion\n");
		}
		return;
	}

	/* Find and delete the forward IPv6 CT entry */
	cache_idx = find_ipv6_ct_entry(src_ipv6_msb, src_ipv6_lsb, dest_ipv6_msb, dest_ipv6_lsb,
					src_port, dest_port, tuple->protocol);

	if (cache_idx >= 0) {
		uint16_t all_pkts_counter;
		uint16_t non_frag_counter;

		mutex_lock(&NatBase->cache_lock);
		rule_handle = NatBase->m_cache[cache_idx].ipv6_ct.rule_handle;
		all_pkts_counter = NatBase->m_cache[cache_idx].ipv6_ct.all_pkts_stats_cnt_index;
		non_frag_counter = NatBase->m_cache[cache_idx].ipv6_ct.non_frag_stats_cnt_index;
		IPA_BE_DBG("Found IPv6 CT fwd entry at cache index %d with handle %u\n", cache_idx, rule_handle);

		/* Delete the IPv6 CT entry from hardware */
		ipa_be_delete_v6_ct_entry(*msg, rule_handle);

		if (ipa_ver >= IPA_HW_v7_0) {
			ipa_be_free_ct_counters_v6(
				NatBase->ct_table_hdl,
				src_ipv6_lsb,
				src_ipv6_msb,
				all_pkts_counter,
				non_frag_counter);
		}

		memset(&NatBase->m_cache[cache_idx], 0, sizeof(NatBase->m_cache[cache_idx]));
		mutex_unlock(&NatBase->cache_lock);

		IPA_BE_DBG("Successfully deleted IPv6 CT fwd entry and cleared cache\n");
	} else {
		IPA_BE_ERR("IPv6 CT fwd entry not found in cache for deletion\n");
	}
}


/**
 * get_ct_stats_cached() - Read a stats counter, using a per-sync-cycle cache.
 *
 * Avoids re-reading a clear-on-read HW counter that was already consumed
 * earlier in the same sync cycle.  On a cache miss the HW counter is read
 * and the result is stored so that subsequent accesses within the same cycle
 * can reuse it without re-querying hardware.
 *
 * @idx:       entry index used as the cache key
 * @stats_cnt: HW stats counter index to query on a cache miss
 * @cache:     per-cycle stats cache array (indexed by entry index)
 * @valid:     per-cycle cache-valid flags array
 * @out:       output: populated with the stats on success
 *
 * Returns 0 on success, non-zero on HW read failure.
 */
static int get_ct_stats_cached(int idx, uint16_t stats_cnt,
				struct ipahal_stats_nat_ct *cache,
				bool *valid,
				struct ipahal_stats_nat_ct *out)
{
	if (valid[idx]) {
		*out = cache[idx];
		return 0;
	}

	int ret = ipa_get_nat_ct_stats(stats_cnt, out);

	if (!ret) {
		cache[idx] = *out;
		valid[idx] = true;
	}

	return ret;
}

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

	/*
	 * Per-entry stats cache: avoids re-reading a clear-on-read HW counter
	 * that was already consumed when processing an earlier entry in the same
	 * sync cycle. Indexed by IPv6 CT table entry index.
	 */
	struct ipahal_stats_nat_ct *entry_stats_cache = NULL;
	bool *entry_stats_cache_valid = NULL;

	if (is_stats) {
		entry_stats_cache = kcalloc(NatBase->m_maxEntries,
					    sizeof(*entry_stats_cache), GFP_KERNEL);
		entry_stats_cache_valid = kcalloc(NatBase->m_maxEntries,
						  sizeof(bool), GFP_KERNEL);
		if (!entry_stats_cache || !entry_stats_cache_valid) {
			IPA_BE_ERR("Failed to allocate stats cache (max_entries=%d)\n",
				   NatBase->m_maxEntries);
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

			ret = get_ct_stats_cached(i,
						  NatBase->m_cache[i].ipv6_ct.all_pkts_stats_cnt_index,
						  entry_stats_cache, entry_stats_cache_valid, &stats);
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

		/* Set reason as stats sync */
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
	int ret;
	bool is_timestamp = true;
	bool is_stats = true;

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

	/*
	 * Per-entry stats cache: avoids re-reading a clear-on-read HW counter
	 * that was already consumed when processing an earlier entry in the same
	 * sync cycle. Indexed by NAT table entry index.
	 */
	struct ipahal_stats_nat_ct *entry_stats_cache = NULL;
	bool *entry_stats_cache_valid = NULL;

	if (is_stats) {
		entry_stats_cache = kcalloc(nat_app->max_entries,
					    sizeof(*entry_stats_cache), GFP_KERNEL);
		entry_stats_cache_valid = kcalloc(nat_app->max_entries,
						  sizeof(bool), GFP_KERNEL);
		if (!entry_stats_cache || !entry_stats_cache_valid) {
			IPA_BE_ERR("Failed to allocate stats cache (max_entries=%d)\n",
				   nat_app->max_entries);
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
		conn_sync->flow_ip_xlate = htonl(nat_app->cache[i].public_ip);
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

		mutex_unlock(&nat_app->cache_lock);

		if (is_stats && nat_app->cache[i].all_pkts_stats_cnt_index > 0) {
			struct ipahal_stats_nat_ct stats = {};

			ret = get_ct_stats_cached(i, nat_app->cache[i].all_pkts_stats_cnt_index,
						  entry_stats_cache, entry_stats_cache_valid, &stats);
			if (ret) {
				IPA_BE_ERR("Failed to get stats for NAT entry %d: idx=%u, ret=%d\n",
					   i, nat_app->cache[i].all_pkts_stats_cnt_index, ret);
			} else {
				IPA_BE_DBG("Rule %d stats: idx=%u "
					   "inbound_pkts=%u inbound_cache_pkts=%u inbound_bytes=%llu "
					   "outbound_pkts=%u outbound_cache_pkts=%u outbound_bytes=%llu\n",
					   i, nat_app->cache[i].all_pkts_stats_cnt_index,
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
					   i, nat_app->cache[i].all_pkts_stats_cnt_index,
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

		/* Set reason as stats sync */
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
