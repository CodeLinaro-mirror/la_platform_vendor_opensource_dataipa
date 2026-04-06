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
	nat_app->max_entries = 4000;

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
	NatBase->m_maxEntries = 1000;

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

	if(nat_app->nat_table_hdl)
	{
		IPA_BE_DBG("nat_table_hdl already exist \n");
		return 0;
	}


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

void ipa_be_addpdn(struct ipa_ipv4_rule_create_msg v4_msg, int pdn_iface)
{
	uint8_t mux_id = 0;
	bool is_sta = 0;
	bool ip_pass = 0;
	bool isVlan = 0;
	uint32_t pub_ip;
	struct ipa_ioc_query_intf_ext_props *ext_prop = NULL;
	struct ipa_ioc_query_intf pdn_intf;

	IPA_BE_DBG("Entry \n");

	/* ext Props */
	memset(&pdn_intf, 0, sizeof(pdn_intf));
	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(pdn_iface, &pdn_intf)) {
		IPA_BE_ERR("Unable to query iface prop\n");
		return;
	} else {
		IPA_BE_DBG("Interface with index %u exist.\n", pdn_iface);
	}

	ext_prop = (struct ipa_ioc_query_intf_ext_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_ext_props) +
		pdn_intf.num_ext_props * sizeof(struct ipa_ioc_ext_intf_prop), GFP_KERNEL);
	if (ext_prop == NULL) {
		IPA_BE_ERR("Unable to allocate ext_prop memory.\n");
		return;
	}

	memcpy(ext_prop->name, pdn_intf.name, sizeof(pdn_intf.name));
	ext_prop->num_ext_props = pdn_intf.num_ext_props;
	IPA_BE_DBG("Query ext_prop %d name %s\n", ext_prop->num_ext_props, ext_prop->name);
	ipa3_query_intf_ext_props(ext_prop);

	mux_id = ext_prop->ext[0].mux_id;
	IPA_BE_DBG("Query iface %s mux_id %d\n", ext_prop->name, mux_id);

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
		if (ipa_be_add_pdn(pub_ip, mux_id, is_sta, ip_pass))
		{
			IPA_BE_ERR("failed adding pdn\n");
		}
	}
	else
	{
		if (ipa_be_add_table(pub_ip, mux_id, is_sta, ip_pass))
		{
			IPA_BE_ERR("failed adding table\n");
		}
	}

	if(ipa_be_add_entry(v4_msg, isVlan))
	{
		IPA_BE_ERR("failed adding nat entry\n");
	}

	/* Clean up allocated memory */
	kfree(ext_prop);
	IPA_BE_DBG("Exit \n");
}

static int ipv6ct_convert_to_ipa_rule(struct ipa_ipv6_rule_create_msg *v6_msg,
				       ipa_ipv6ct_rule *ipa_rule)
{
	struct ipa_ipv6_5tuple *tuple;

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
	ipa_rule->s = false;

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

void ipa_be_add_v6_ct_entry(struct ipa_ipv6_rule_create_msg v6_msg, int pdn_iface)
{
	uint8_t mux_id = 0;
	IPA_BE_DBG("Entry ipa_be_add_v6_ct_entry\n");
	uint32_t v6_prefix[2];  /* IPv6 prefix is 64 bits = 2 x 32-bit words */
	struct ipa_ioc_query_intf_ext_props *ext_prop = NULL;
	struct ipa_ioc_query_intf pdn_intf;
	uint32_t rule_hdl=0;

	/* ext Props */
	memset(&pdn_intf, 0, sizeof(pdn_intf));
	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(pdn_iface, &pdn_intf)) {
		IPA_BE_ERR("Unable to query iface prop\n");
		goto cleanup;
	} else {
		IPA_BE_DBG("Interface with index %u exist.\n", pdn_iface);
	}

	ext_prop = (struct ipa_ioc_query_intf_ext_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_ext_props) +
		pdn_intf.num_ext_props * sizeof(struct ipa_ioc_ext_intf_prop), GFP_KERNEL);
	if (ext_prop == NULL) {
		IPA_BE_ERR("Unable to allocate ext_prop memory.\n");
		return;
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
	if (ipa_be_add_table_v6(v6_prefix)) {
			IPA_BE_ERR("failed adding IPv6 CT table\n .. exiting..\n");
			goto cleanup;
	}

	/* Check for duplicate IPv6 CT entry before adding */
	struct ipa_ipv6_5tuple *tuple = &v6_msg.tuple;
	uint64_t src_ipv6_msb = ((__u64)ntohl(tuple->flow_ip[0]) << 32) | ntohl(tuple->flow_ip[1]);
	uint64_t src_ipv6_lsb = ((__u64)ntohl(tuple->flow_ip[2]) << 32) | ntohl(tuple->flow_ip[3]);
	uint64_t dest_ipv6_msb = ((__u64)ntohl(tuple->return_ip[0]) << 32) | ntohl(tuple->return_ip[1]);
	uint64_t dest_ipv6_lsb = ((__u64)ntohl(tuple->return_ip[2]) << 32) | ntohl(tuple->return_ip[3]);
	uint16_t src_port = ntohs(tuple->flow_ident);
	uint16_t dest_port = ntohs(tuple->return_ident);

	/* Check if entry already exists */
	int existing_idx = find_ipv6_ct_entry(src_ipv6_msb, src_ipv6_lsb, dest_ipv6_msb, dest_ipv6_lsb,
					      src_port, dest_port, tuple->protocol);

	if (existing_idx >= 0) {
		IPA_BE_DBG("Duplicate IPv6 CT entry found at cache index %d, ignoring addition\n", existing_idx);
		IPA_BE_DBG("Duplicate entry: Flow: %pI6:%u -> Return: %pI6:%u, protocol=%u\n",
			   tuple->flow_ip, src_port, tuple->return_ip, dest_port, tuple->protocol);
		goto cleanup;
	}

	/* Add IPv6 connection tracking entry */
	if (ipv6ct_add_entry(&v6_msg, &rule_hdl) == 0) {
		IPA_BE_DBG("IPv6 CT entry added with hdl %d\n", rule_hdl);

		/* Store the rule handle in cache for bookkeeping */
		int cache_idx = store_ipv6_ct_entry(src_ipv6_msb, src_ipv6_lsb, dest_ipv6_msb, dest_ipv6_lsb,
						    src_port, dest_port, tuple->protocol, rule_hdl);
		if (cache_idx < 0) {
			IPA_BE_ERR("Failed to store IPv6 CT entry in cache\n");
		}
	} else {
		IPA_BE_ERR("Failed to add IPv6 CT entry\n");
	}

cleanup:
	if (ext_prop) {
		kfree(ext_prop);
	}
	IPA_BE_DBG("Exit ipa_be_add_v6_ct_entry\n");
}

/* Add new entry to the nat table on new connection */
int ipa_be_add_entry(struct ipa_ipv4_rule_create_msg v4_msg, bool isVlan)
{
	int cnt = 0;
	ipa_nat_ipv4_rule nat_rule;

	nat_table_entry *rule = kzalloc(sizeof(nat_table_entry), GFP_KERNEL);
	if (!rule) {
		IPA_BE_ERR("Failed to allocate rule\n");
		return IPA_BE_FAILURE;
	}

	if ((v4_msg.tuple.flow_ip == v4_msg.conn_rule.flow_ip_xlate) &&
		(v4_msg.tuple.return_ip == v4_msg.conn_rule.return_ip_xlate))
	{
		/* IPPT case */
		if(v4_msg.conn_rule.flow_interface_num == v4_msg.conn_rule.flow_top_interface_num)
		{
			rule->protocol = v4_msg.tuple.protocol;
			rule->private_ip = ntohl(v4_msg.conn_rule.return_ip_xlate);
			rule->target_ip = ntohl(v4_msg.tuple.flow_ip);
			rule->public_ip = ntohl(v4_msg.tuple.return_ip);
			rule->private_port = ntohs(v4_msg.tuple.return_ident);
			rule->target_port = ntohs(v4_msg.tuple.flow_ident);
			rule->public_port = ntohs(v4_msg.conn_rule.return_ident_xlate);

			IPA_BE_DBG("Downlink IPPT case\n");
		}
		else
		{
			rule->protocol = v4_msg.tuple.protocol;
			rule->private_ip = ntohl(v4_msg.tuple.flow_ip);
			rule->target_ip = ntohl(v4_msg.tuple.return_ip);
			rule->public_ip = ntohl(v4_msg.conn_rule.flow_ip_xlate);
			rule->private_port = ntohs(v4_msg.tuple.flow_ident);
			rule->target_port = ntohs(v4_msg.tuple.return_ident);
			rule->public_port = ntohs(v4_msg.conn_rule.flow_ident_xlate);

			IPA_BE_DBG("Uplink IPPT case\n");
		}
	}
	else if (v4_msg.tuple.flow_ip == v4_msg.conn_rule.flow_ip_xlate)
	{
		rule->protocol = v4_msg.tuple.protocol;
		rule->private_ip = ntohl(v4_msg.conn_rule.return_ip_xlate);
		rule->target_ip = ntohl(v4_msg.tuple.flow_ip);
		rule->public_ip = ntohl(v4_msg.tuple.return_ip);
		rule->private_port = ntohs(v4_msg.tuple.return_ident);
		rule->target_port = ntohs(v4_msg.tuple.flow_ident);
		rule->public_port = ntohs(v4_msg.conn_rule.return_ident_xlate);

		IPA_BE_DBG("Downlink case\n");
	}
	else
	{
		rule->protocol = v4_msg.tuple.protocol;
		rule->private_ip = ntohl(v4_msg.tuple.flow_ip);
		rule->target_ip = ntohl(v4_msg.tuple.return_ip);
		rule->public_ip = ntohl(v4_msg.conn_rule.flow_ip_xlate);
		rule->private_port = ntohs(v4_msg.tuple.flow_ident);
		rule->target_port = ntohs(v4_msg.tuple.return_ident);
		rule->public_port = ntohs(v4_msg.conn_rule.flow_ident_xlate);

		IPA_BE_DBG("Uplink case\n");
	}

	bool cacheOnly = false;
	uint8_t pdn_index;

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
				if (ipa_nat_add_ipv4_rule(nat_app->nat_table_hdl, &nat_rule,
							  &nat_app->cache[cnt].rule_hdl) < 0) {
					IPA_BE_ERR("unable to add the rule\n");
					mutex_unlock(&nat_app->cache_lock);
					kfree(rule);
					return -1;
				}
				IPA_BE_DBG("cache entry %d rule handle %d\n", cnt,
					   nat_app->cache[cnt].rule_hdl);
				nat_app->cache[cnt].enabled = true;
			}

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
					IPA_BE_ERR("%s() %d deletion failed\n", __FUNCTION__, __LINE__);
				} else {
					IPA_BE_DBG("Deleted Nat entry(%d) from HW Successfully\n", cnt);
				}
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

void ipa_be_delete_entry(struct ipa_ipv4_rule_destroy_msg v4_msg)
{
	nat_table_entry rule;
	uint32_t pub_ip = 0;

	IPA_BE_DBG("Deleting IP4 NAT entry\n");
	memset(&rule, 0, sizeof(rule));
	rule.protocol = v4_msg.tuple.protocol;

	/*
	 * The NAT entry is always created from the uplink direction, where
	 * flow_ip != flow_ip_xlate. The key used to store the entry is
	 * based on the private-side tuple.
	 * The destroy message can come for either direction. We need to
	 * reconstruct the key that was used during insertion.
	 */
	if (v4_msg.tuple.flow_ip != v4_msg.conn_rule.flow_ip_xlate) {
		/*
		 * Destroy message is for the UPLINK flow.
		 * The key is the original tuple.
		 */
		rule.private_ip   = ntohl(v4_msg.tuple.flow_ip);
		rule.target_ip    = ntohl(v4_msg.tuple.return_ip);
		rule.private_port = ntohs(v4_msg.tuple.flow_ident);
		rule.target_port  = ntohs(v4_msg.tuple.return_ident);
	} else {
		/*
		 * Destroy message is for the DOWNLINK flow.
		 * We need to use the XLATE fields to find the original private tuple.
		 * Original private_ip was return_ip_xlate.
		 * Original target_ip was flow_ip (which is the public server ip).
		 */
		rule.private_ip   = ntohl(v4_msg.conn_rule.return_ip_xlate);
		rule.target_ip    = ntohl(v4_msg.tuple.flow_ip);
		rule.private_port = ntohs(v4_msg.conn_rule.return_ident_xlate);
		rule.target_port  = ntohs(v4_msg.tuple.flow_ident);
	}

	/*
	 * Compute the public IP (PDN IP) so we can check after deletion
	 * whether the PDN has become empty.  This mirrors the logic in
	 * ipa_be_addpdn().
	 */
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
				uint8_t protocol, uint32_t rule_handle)
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
void ipa_be_handle_v6_ct_deletion(struct ipa_ipv6_rule_destroy_msg *msg)
{
	struct ipa_ipv6_5tuple *tuple;
	uint64_t src_ipv6_msb, src_ipv6_lsb, dest_ipv6_msb, dest_ipv6_lsb;
	uint16_t src_port, dest_port;
	int cache_idx;
	uint32_t rule_handle;

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

	/* Find the IPv6 CT entry in cache to get the rule handle */
	cache_idx = find_ipv6_ct_entry(src_ipv6_msb, src_ipv6_lsb, dest_ipv6_msb, dest_ipv6_lsb,
					src_port, dest_port, tuple->protocol);

	if (cache_idx >= 0) {
		mutex_lock(&NatBase->cache_lock);
		rule_handle = NatBase->m_cache[cache_idx].ipv6_ct.rule_handle;
		IPA_BE_DBG("Found IPv6 CT entry at cache index %d with handle %u\n", cache_idx, rule_handle);
		mutex_unlock(&NatBase->cache_lock);

		/* Delete the IPv6 CT entry from hardware */
		ipa_be_delete_v6_ct_entry(*msg, rule_handle);

		/* Clear the cache entry */
		mutex_lock(&NatBase->cache_lock);
		memset(&NatBase->m_cache[cache_idx], 0, sizeof(NatBase->m_cache[cache_idx]));
		mutex_unlock(&NatBase->cache_lock);

		IPA_BE_DBG("Successfully deleted IPv6 CT entry and cleared cache\n");
	} else {
		IPA_BE_ERR("IPv6 CT entry not found in cache for deletion\n");
	}
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
	uint32_t timestamp;
	uint32_t rule_handle;
	uint32_t old_timestamp;
	int ret;

	if (!msg || !NatBase) {
		IPA_BE_ERR("Invalid parameters or NatBase not initialized\n");
		return IPA_TX_FAILURE_BAD_PARAM;
	}

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

	ret = ipa_ipv6ct_timestamp_flush(NatBase->ct_table_hdl);
	IPA_BE_DBG("Flushing timestamp for IPv6 CT table hdl=%u\n",
		NatBase->ct_table_hdl);

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

		/* Calculate inc_ticks as the difference between new and cached timestamp */
		/* Only set inc_ticks if timestamp has changed (entry is active) */
		old_timestamp = NatBase->m_cache[i].m_timestamp;
		if (timestamp != old_timestamp) {
			conn_sync->inc_ticks = timestamp - old_timestamp;
			/* Cache the new timestamp for next query */
			NatBase->m_cache[i].m_timestamp = timestamp;
		}
		/* If timestamp hasn't changed, leave inc_ticks as 0 (entry is inactive) */

		mutex_unlock(&NatBase->cache_lock);

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
	uint32_t timestamp;
	uint32_t rule_hdl;
	uint32_t old_timestamp;
	int ret;

	IPA_BE_DBG("=== ENTER ipa_sync_ipv4_stats_many_msg ===\n");

	if (!msg) {
		IPA_BE_ERR("Invalid parameter: msg is NULL\n");
		return IPA_TX_FAILURE_BAD_PARAM;
	}

	if (!nat_app) {
		IPA_BE_ERR("Invalid parameter: nat_app not initialized\n");
		return IPA_TX_FAILURE_BAD_PARAM;
	}

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

	ret = ipa_nat_timestamp_flush(nat_app->nat_table_hdl);
	IPA_BE_DBG("Flushing timestamp for nat table hdl=%u\n",
		nat_app->nat_table_hdl);

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

		/* Populate the connection sync structure */
		struct ipa_ipv4_conn_sync *conn_sync = &sync_many->conn_sync[count];
		memset(conn_sync, 0, sizeof(*conn_sync));

		/* Set the index to the NAT table index */
		conn_sync->index = i;

		/* Lock to safely read cache data and update timestamp */
		mutex_lock(&nat_app->cache_lock);

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

		/* Calculate inc_ticks as the difference between new and cached timestamp */
		/* Only set inc_ticks if timestamp has changed (entry is active) */
		old_timestamp = nat_app->cache[i].timestamp;
		if (timestamp != old_timestamp) {
			conn_sync->inc_ticks = timestamp - old_timestamp;
			/* Cache the new timestamp for next query */
			nat_app->cache[i].timestamp = timestamp;
		}
		/* If timestamp hasn't changed, leave inc_ticks as 0 (entry is inactive) */

		mutex_unlock(&nat_app->cache_lock);

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

	return IPA_TX_SUCCESS;
}
EXPORT_SYMBOL(ipa_sync_ipv4_stats_many_msg);
