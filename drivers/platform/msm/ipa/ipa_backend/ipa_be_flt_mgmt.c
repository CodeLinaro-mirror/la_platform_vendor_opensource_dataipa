// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include <linux/string.h>
#include <linux/jhash.h>
#include <linux/bitops.h>
#include <linux/msm_ipa.h>
#include "ipa_api.h"
#include "ipa_be.h"
#include "ipa_be_clientdb.h"
#include "ipa_be_flt_mgmt.h"
#include "ipa_i.h"

#define IPA_MAX_FLT_RULE 100

/* IPv6 address constants */
#define MULTICAST_IPV6_PREFIX 0xFF000000
#define LINK_LOCAL_IPV6_PREFIX 0xFE800000
#define LINK_LOCAL_IPV6_PREFIX_MASK 0xFFC00000
#define RESERVED_IPV6_PREFIX 0xFEC00000
#define IPACM_FIREWALL_IPPROTO_TCP 6
#define IPV6_HEADER_SIZE 40

extern int ipa3_qmi_filter_request_ex_send(
	struct ipa_install_fltr_rule_req_ex_msg_v01 *msg);

/* Helper function to convert addresses to network order */
static inline void addr2network(enum ipa_ip_type addr_type, void *addr)
{
	if (addr_type == IPA_IP_v4 || addr_type == IPA_IP_v6) {
		uint32_t *ptr = (uint32_t *)addr;

		if (addr_type == IPA_IP_v4) {
			ptr[0] = htonl(ptr[0]);
		} else {
			ptr[0] = htonl(ptr[0]);
			ptr[1] = htonl(ptr[1]);
			ptr[2] = htonl(ptr[2]);
			ptr[3] = htonl(ptr[3]);
		}
	}
}

/* Helper function to convert rule attributes to network order */
static int change_to_network_order_kernel(enum ipa_ip_type iptype, struct ipa_rule_attrib *attrib)
{
	if (!attrib) {
		IPA_BE_ERR("Bad attribute pointer is NULL.\n");
		return -EINVAL;
	}

	if (iptype == IPA_IP_v6) {
		addr2network(iptype, attrib->u.v6.src_addr);
		addr2network(iptype, attrib->u.v6.src_addr_mask);
		addr2network(iptype, attrib->u.v6.dst_addr);
		addr2network(iptype, attrib->u.v6.dst_addr_mask);
	} else {
		IPA_BE_DBG("IP type is not IPv6, do nothing: %d\n", iptype);
	}

	return 0;
}

enum ipa_filter_action_enum_v01
GetQmiFilterAction(enum ipa_flt_action ipa_filter_action)
{
	switch (ipa_filter_action) {
	case IPA_PASS_TO_ROUTING:
		return QMI_IPA_FILTER_ACTION_ROUTING_V01;
	case IPA_PASS_TO_SRC_NAT:
		return QMI_IPA_FILTER_ACTION_SRC_NAT_V01;
	case IPA_PASS_TO_DST_NAT:
		return QMI_IPA_FILTER_ACTION_DST_NAT_V01;
	case IPA_PASS_TO_EXCEPTION:
		return QMI_IPA_FILTER_ACTION_EXCEPTION_V01;
	default:
		return QMI_IPA_FILTER_ACTION_ROUTING_V01;
	}
}

uint32_t wan_ul_fl_rule_hdl_v4[IPA_MAX_NUM_PROPS][MAX_WAN_UL_FILTER_RULES] = {0};
uint32_t num_wan_ul_fl_rule_v4[IPA_MAX_NUM_PROPS] = {0};
uint32_t flt_rule_count_v4[IPA_CLIENT_MAX] = {0};
uint32_t flt_rule_count_v6[IPA_CLIENT_MAX] = {0};

enum flt_rule_type {
	FLT_RULE_TYPE_DEFAULT = 0,
	FLT_RULE_TYPE_CATCHUP = 1,
};

struct flt_rule
{
	struct list_head node;
	struct ipa_flt_rule_add flt_rule;
	uint8_t mux_id;
	enum flt_rule_type rule_type;
	int pdn_iface;
};

static LIST_HEAD(Pdn_flt_rule_v4_list);
static LIST_HEAD(Pdn_flt_rule_v6_list);
static DEFINE_SPINLOCK(pdn_flt_rule_v4_lock);
static DEFINE_SPINLOCK(pdn_flt_rule_v6_lock);
uint32_t wan_rule_count_v4 = 0;
uint32_t wan_rule_count_v6 = 0;

/* Structure to track installed uplink interface pairs and their usage count */
struct ipa_uplink_pair {
	struct list_head node;
	int pdn_iface;
	int client_iface;
	enum ipa_ip_type ip_type;  /* Track IP version separately */
	int ref_count;
};

static LIST_HEAD(ipa_uplink_pairs_list);
static DEFINE_SPINLOCK(ipa_uplink_lock);

/* Structure to track installed PDN-level filtering rules */
struct ipa_pdn_filter_rules {
	struct list_head node;
	int pdn_iface;
	enum ipa_ip_type ip_type;
	bool dft_rule_installed;
	bool catchup_rule_installed;
	int ref_count;
};

static LIST_HEAD(ipa_pdn_filter_list);
static DEFINE_SPINLOCK(ipa_pdn_filter_lock);

/* Helper functions for PDN filter tracking */
static struct ipa_pdn_filter_rules *ipa_find_pdn_filter_entry(int pdn_iface, enum ipa_ip_type ip_type)
{
	struct ipa_pdn_filter_rules *entry;

	list_for_each_entry(entry, &ipa_pdn_filter_list, node) {
		if (entry->pdn_iface == pdn_iface && entry->ip_type == ip_type) {
			return entry;
		}
	}
	return NULL;
}

static struct ipa_pdn_filter_rules *ipa_create_pdn_filter_entry(int pdn_iface, enum ipa_ip_type ip_type)
{
	struct ipa_pdn_filter_rules *new_entry;

	new_entry = kzalloc(sizeof(struct ipa_pdn_filter_rules), GFP_ATOMIC);
	if (!new_entry) {
		IPA_BE_ERR("Failed to allocate memory for PDN filter entry\n");
		return NULL;
	}

	new_entry->pdn_iface = pdn_iface;
	new_entry->ip_type = ip_type;
	new_entry->dft_rule_installed = false;
	new_entry->catchup_rule_installed = false;
	new_entry->ref_count = 0;

	list_add(&new_entry->node, &ipa_pdn_filter_list);
	return new_entry;
}

static void ipa_remove_pdn_filter_entry(struct ipa_pdn_filter_rules *entry)
{
	list_del(&entry->node);
	kfree(entry);
}

int ipa_be_v4_add_filter_rule(struct ipa_ipv4_rule_create_msg v4_msg, bool lan2lan, int intf_num, mac_addr_t mac, int is_ret)
{
	struct ipa_ioc_add_flt_rule *pFilteringTable = NULL;
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa_ioc_query_intf temp_intf;
	int retval = 0, len = 0;
	struct ipa_ioc_query_intf_rx_props *rx_prop;
	int total_rules = 1;
	int idx = 0;
	int flt_hdl = 0;
	struct ipa_ioc_get_rt_tbl rt_tbl = {0};
	struct ipa3_flt_entry flt_entry = {0};

	IPA_BE_DBG("ECMIPA entry lan2lan %d\n", lan2lan);
	ipa_type_check_ipa_mac_addr(mac);

	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(intf_num, &temp_intf)) {
		IPA_BE_ERR("Interface with index %u does not exist.\n", intf_num);
		return -EINVAL;
	}

	rx_prop = (struct ipa_ioc_query_intf_rx_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_rx_props) +
		temp_intf.num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (rx_prop == NULL) {
		IPA_BE_ERR("Unable to allocate rx_prop memory.\n");
		return -EINVAL;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(rx_prop->name, temp_intf.name, sizeof(rx_prop->name));
#else
	strlcpy(rx_prop->name, temp_intf.name, sizeof(rx_prop->name));
#endif

	rx_prop->num_rx_props = temp_intf.num_rx_props;
	IPA_BE_DBG("Query rx_prop %d name %s\n", rx_prop->num_rx_props, temp_intf.name);
	ipa3_query_intf_rx_props(rx_prop);

	if (rx_prop->num_rx_props == 0) {
		IPA_BE_ERR("No rx props for iface %s\n", rx_prop->name);
		retval = -EINVAL;
		goto end;
	}

	IPA_BE_DBG("Query response rx_prop src %d hdr_l2_type %d\n", rx_prop->rx[idx].src_pipe,
		rx_prop->rx[idx].hdr_l2_type);

	if (lan2lan) {
		len = sizeof(struct ipa_ioc_add_flt_rule) + sizeof(struct ipa_flt_rule_add);
		pFilteringTable = (struct ipa_ioc_add_flt_rule *)kzalloc(len, GFP_KERNEL);
		if (!pFilteringTable) {
			IPA_BE_ERR("Failed to allocate ipa_ioc_add_flt_rule_after memory...\n");
			return -EINVAL;
		}

		IPA_BE_DBG("Install rules on Rx pipe at idx %d src_pipe %d\n", idx, rx_prop->rx[idx].src_pipe);
		memset(pFilteringTable, 0, len);

		/* add mac based rule*/
		pFilteringTable->commit = 1;
		pFilteringTable->ep = rx_prop->rx[idx].src_pipe;
		pFilteringTable->ip = IPA_IP_v4;
		pFilteringTable->num_rules = total_rules;
		/* pFilteringTable->add_after_hdl = 0; //tbd */

		memset(&flt_rule_entry, 0, sizeof(flt_rule_entry));

		flt_rule_entry.rule.retain_hdr = 0;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule_entry.rule.eq_attrib_type = 0;

		rt_tbl.ip = IPA_IP_v4;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(rt_tbl.name, V4_LAN_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#else
		strlcpy(rt_tbl.name, V4_LAN_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#endif

		IPA_BE_DBG("This flt rule points to rt tbl %s.\n", rt_tbl.name);

		if (ipa3_get_rt_tbl((struct ipa_ioc_get_rt_tbl *)&rt_tbl)) {
			IPA_BE_ERR("ECMIPA failed to get route hdl \n");
			retval = -EFAULT;
		}
		IPA_BE_DBG("Install filter rules with rt_tbl.hdl %d \n", rt_tbl.hdl);

		IPA_BE_DBG("Install rules on Rx pipe at idx %d \n", idx);
		flt_rule_entry.rule.rt_tbl_hdl = rt_tbl.hdl;

		flt_rule_entry.rule.hashable = true;
		flt_rule_entry.rule.rule_id = 1; /* LAN2LAN_RULE_ID */

		flt_rule_entry.flt_rule_category = IPA_FLT_RULE_CAT_LAN2LAN;

		memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[idx].attrib, sizeof(flt_rule_entry.rule.attrib));

		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;

		if(is_ret) {
			flt_rule_entry.rule.attrib.u.v4.dst_addr = (uint32_t)ntohl(v4_msg.tuple.return_ip);
		} else{
			flt_rule_entry.rule.attrib.u.v4.dst_addr = (uint32_t)ntohl(v4_msg.tuple.flow_ip);
		}
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;

		memcpy(&(pFilteringTable->rules[0]), &flt_rule_entry, sizeof(flt_rule_entry));

		if (ipa3_add_flt_rule_usr((struct ipa_ioc_add_flt_rule *)pFilteringTable,
			true)) {
			IPA_BE_ERR("ipa3_add_flt_rule_usr failed\n");
			retval = -EFAULT;
			goto end;
		} else {
			flt_hdl = pFilteringTable->rules[0].flt_rule_hdl;
			IPA_BE_DBG("Lan2Lan %d filter rule hdl: %d\n", lan2lan, flt_hdl);
			flt_entry.flt_hdl = flt_hdl;
			flt_entry.cat = IPA_FLT_RULE_CAT_LAN2LAN;
			flt_entry.ip_type = IPA_IP_v4;

			flt_entry.rule.attrib.attrib_mask = flt_rule_entry.rule.attrib.attrib_mask;
			memcpy(flt_entry.rule.attrib.dst_mac_addr, mac, sizeof(flt_entry.rule.attrib.dst_mac_addr));
			flt_entry.rule.attrib.u.v4.dst_addr = flt_rule_entry.rule.attrib.u.v4.dst_addr;
			flt_entry.rule.attrib.u.v4.dst_addr_mask = flt_rule_entry.rule.attrib.u.v4.dst_addr_mask;

			ipa3_add_filter_rules_entry(intf_num, flt_entry);
		}
	} else {
		IPA_BE_DBG("Lan2wan case \n");
	}

end:
	if (pFilteringTable)
		kfree(pFilteringTable);
	if (rx_prop)
		kfree(rx_prop);

	IPA_BE_DBG("Exit retval %d \n", retval);
	return retval;
}
EXPORT_SYMBOL(ipa_be_v4_add_filter_rule);

int ipa_be_v4_add_uplink_filter_rule(struct ipa_ipv4_rule_create_msg v4_msg, bool lan2lan, int pdn_iface, int client_iface)
{
	struct ipa_ioc_add_flt_rule *pFilteringTable = NULL;
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa_ioc_query_intf client_intf, pdn_intf;
	int retval = 0, len = 0;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	struct ipa_ioc_query_intf_ext_props *ext_prop = NULL;
	int total_rules = 1;
	int idx = 0;
	int j = 0, i = 0;
	struct ipa_fltr_installed_notif_req_msg_v01 *flt_index = NULL;
	int cnt;
	enum ipa_flt_action action_cache;
	enum ipa_ip_type iptype;
	int index = 0;
	bool notif_only = false;
	struct ipa3_flt_entry flt_entry = {0};
	struct ipa_uplink_pair *pair;
	struct ipa_uplink_pair *new_pair = NULL;
	bool is_xlat = false;
	uint8_t value = 0;
	/* Auto-enable XLAT for IPv4 192.0.0.x (192.0.0.0/24) flow/return IPs */
	if (((ntohl(v4_msg.tuple.flow_ip) & 0xFFFFFF00U) == 0xC0000000U) ||
	    ((ntohl(v4_msg.tuple.return_ip) & 0xFFFFFF00U) == 0xC0000000U) ||
		((ntohl(v4_msg.conn_rule.flow_ip_xlate) & 0xFFFFFF00U) == 0xC0000000U) ||
		((ntohl(v4_msg.conn_rule.return_ip_xlate & 0xFFFFFF00U) == 0xC0000000U))) {
		is_xlat = true;
		IPA_BE_DBG("XLAT enabled due to 192.0.0.x match: flow %pI4n return %pI4n\n",
			   &v4_msg.tuple.flow_ip, &v4_msg.tuple.return_ip);

		/* Create a dummy v6_msg for XLAT traffic */
		struct ipa_ipv6_rule_create_msg v6_msg_xlat;
		memset(&v6_msg_xlat, 0, sizeof(v6_msg_xlat));

		/* Call v6 uplink filter rule for XLAT */
		ipa_be_v6_add_uplink_filter_rule(v6_msg_xlat, lan2lan, pdn_iface, client_iface, is_xlat);
	}

	iptype = IPA_IP_v4;
	IPA_BE_DBG("ECMIPA entry lan2lan %d\n", lan2lan);

	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(client_iface, &client_intf)) {
		retval = -EINVAL;
		goto end;
	} else {
		IPA_BE_DBG("Interface with index %u exist.\n", client_iface);
	}

	spin_lock_bh(&ipa_uplink_lock);
	list_for_each_entry(pair, &ipa_uplink_pairs_list, node) {
		if (pair->pdn_iface == pdn_iface && pair->client_iface == client_iface && pair->ip_type == iptype) {
			pair->ref_count++;
			spin_unlock_bh(&ipa_uplink_lock);
			IPA_BE_DBG("Uplink filter for pdn %d, client %d, IP type %d already installed. ref_count: %d\n",
				pdn_iface, client_iface, iptype, pair->ref_count);
			return 0; /* Rules already exist, just increment ref count and exit */
		}
	}
	spin_unlock_bh(&ipa_uplink_lock);

	IPA_BE_DBG("Created new uplink pair tracking for pdn %d, client %d\n", pdn_iface, client_iface);

	rx_prop = (struct ipa_ioc_query_intf_rx_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_rx_props) +
		client_intf.num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (rx_prop == NULL) {
		IPA_BE_ERR("Unable to allocate rx_prop memory.\n");
		retval = -EINVAL;
		goto end;
	}
	memcpy(rx_prop->name, client_intf.name, sizeof(client_intf.name));
	rx_prop->num_rx_props = client_intf.num_rx_props;
	IPA_BE_DBG("Query rx_prop %d name %s\n", rx_prop->num_rx_props, client_intf.name);
	ipa3_query_intf_rx_props(rx_prop);
	IPA_BE_DBG("Query response rx_prop src %d hdr_l2_type %d\n", rx_prop->rx[idx].src_pipe,
		rx_prop->rx[idx].hdr_l2_type);

	/* ext Props */
	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(pdn_iface, &pdn_intf)) {
		retval = -EINVAL;
		goto end;
	} else {
		IPA_BE_DBG("Interface with index %u exist.\n", pdn_iface);
	}
	ext_prop = (struct ipa_ioc_query_intf_ext_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_ext_props) +
		pdn_intf.num_ext_props * sizeof(struct ipa_ioc_ext_intf_prop), GFP_KERNEL);
	if (ext_prop == NULL) {
		IPA_BE_ERR("Unable to allocate ext_prop memory.\n");
		retval = -EINVAL;
		goto end;
	}
	memcpy(ext_prop->name, pdn_intf.name, sizeof(pdn_intf.name));
	ext_prop->num_ext_props = pdn_intf.num_ext_props;
	IPA_BE_DBG("Query ext_prop %d name %s\n", ext_prop->num_ext_props, ext_prop->name);
	ipa3_query_intf_ext_props(ext_prop);

	flt_index = (struct ipa_fltr_installed_notif_req_msg_v01 *)kzalloc(
		sizeof(struct ipa_fltr_installed_notif_req_msg_v01), GFP_KERNEL);

	total_rules = ext_prop->num_ext_props;

	for (j = 0; j < rx_prop->num_rx_props / 2; j++) {
		idx = j * 2;
		if (rx_prop->rx[idx].ip != IPA_IP_v4) {
			IPA_BE_DBG("IP not matching required type %d .. continue\n", rx_prop->rx[idx].ip);
			continue;
		}
		IPA_BE_DBG("Install rules at idx %d\n", idx);

		memset(flt_index, 0, sizeof(struct ipa_fltr_installed_notif_req_msg_v01));
		flt_index->source_pipe_index = ipa_get_ep_mapping(rx_prop->rx[idx].src_pipe);
		flt_index->install_status = IPA_QMI_RESULT_SUCCESS_V01;

		flt_index->rule_id_valid = 1;
		flt_index->rule_id_len = total_rules;
		flt_index->embedded_pipe_index_valid = 1;
		flt_index->embedded_pipe_index = ipa_get_ep_mapping(IPA_CLIENT_APPS_LAN_WAN_PROD);
		flt_index->retain_header_valid = 1;
		flt_index->retain_header = 0;
		flt_index->embedded_call_mux_id_valid = 1;

		flt_index->embedded_call_mux_id = ext_prop->ext[0].mux_id;

		IPA_BE_DBG("flt_index: src pipe: %d, num of rules: %d, ebd pipe: %d, mux id: %d\n",
			flt_index->source_pipe_index, flt_index->rule_id_len, flt_index->embedded_pipe_index, flt_index->embedded_call_mux_id);

		len = sizeof(struct ipa_ioc_add_flt_rule) +
			total_rules * sizeof(struct ipa_flt_rule_add);

		pFilteringTable =
			(struct ipa_ioc_add_flt_rule *)kzalloc(len, GFP_KERNEL);

		if (pFilteringTable == NULL) {
			IPA_BE_ERR("Error Locate ipa_flt_rule_add memory...\n");
			retval = -EINVAL;
			goto end;
		}
		memset(pFilteringTable, 0, len);

		pFilteringTable->commit = 1;
		pFilteringTable->ep = rx_prop->rx[idx].src_pipe;
		pFilteringTable->global = false;
		pFilteringTable->ip = iptype;
		pFilteringTable->num_rules = total_rules;

		for (cnt = i = 0; cnt < ext_prop->num_ext_props && i < total_rules; cnt++) {
			if (ext_prop->ext[cnt].ip != IPA_IP_v4) {
				IPA_BE_ERR("iptype mismatch %d .. continue\n", ext_prop->ext[cnt].ip);
				continue;
			}

			IPA_BE_DBG("Ext prop cnt %d rt_tbl_idx: %d, rule_id: %d, is_xlat %d \n",
					cnt, ext_prop->ext[cnt].rt_tbl_idx,
					ext_prop->ext[cnt].rule_id,
					ext_prop->ext[cnt].is_xlat_rule);

			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
			flt_rule_entry.at_rear = 1;

			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;

			flt_rule_entry.rule.retain_hdr = 0;
			flt_rule_entry.rule.to_uc = 0;
			flt_rule_entry.rule.eq_attrib_type = 1;

			flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;
			action_cache = flt_rule_entry.rule.action;
			flt_rule_entry.rule.set_metadata = true;

			flt_rule_entry.flt_rule_category = IPA_FLT_RULE_CAT_UPLINK;

			index = flt_rule_count_v4[rx_prop->rx[idx].src_pipe];

			memcpy(&flt_rule_entry.rule.eq_attrib,
				&ext_prop->ext[cnt].eq_attrib,
				sizeof(ext_prop->ext[cnt].eq_attrib));

			if (flt_rule_entry.rule.eq_attrib.ipv4_frag_eq_present)
				flt_rule_entry.at_rear = 0;

			/* Populate the flt rule action from ext_prop */
			if (ext_prop->ext[cnt].action == IPA_PASS_TO_EXCEPTION) {
				/* Override the rule action if Q6 can't handle it, go A7 exception */
				flt_rule_entry.rule.action = ext_prop->ext[cnt].action;
				flt_rule_entry.rule.rt_tbl_idx = 0;
				IPA_BE_ERR("Override rule index %d to act: %d, rt_tbl_idx: %d to %d\n",
					cnt, flt_rule_entry.rule.action,
					ext_prop->ext[cnt].rt_tbl_idx,
					flt_rule_entry.rule.rt_tbl_idx);
			} else {
				/* restore the rule action */
				flt_rule_entry.rule.action = action_cache;
				flt_rule_entry.rule.rt_tbl_idx = ext_prop->ext[cnt].rt_tbl_idx;
				IPA_BE_DBG("Restore rule index %d to act: %d, rt_tbl_idx: %d \n",
					cnt, flt_rule_entry.rule.action,
					flt_rule_entry.rule.rt_tbl_idx);
			}

			/* Handle XLAT configuration */
			if (is_xlat && ext_prop->ext[cnt].is_xlat_rule) {
				/* fill the value of meta-data */
				if (v4_msg.vlan_primary_rule.egress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED)
				{
					value = v4_msg.vlan_primary_rule.egress_vlan_tag & 0x0FFF;
					IPA_BE_DBG("Vlan egress client vland id %d \n", value);
				}
				else if (v4_msg.vlan_primary_rule.ingress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED)
				{
					value = v4_msg.vlan_primary_rule.ingress_vlan_tag & 0x0FFF;
					IPA_BE_DBG("Vlan ingress client vland id %d \n", value);
				}
				else{
					value = ext_prop->ext[cnt].mux_id;
				}
				flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;
				flt_rule_entry.rule.eq_attrib.metadata_meq32_present = 1;
				flt_rule_entry.rule.eq_attrib.metadata_meq32.offset = 0;
				flt_rule_entry.rule.eq_attrib.metadata_meq32.value = (value & 0xFF) << 16;
				flt_rule_entry.rule.eq_attrib.metadata_meq32.mask = 0x00FF0000;
				IPA_BE_DBG("xlat meta-data is modified for rule: %d index %d metadata : 0x%x rule_id %d\n",
						   cnt, i, flt_rule_entry.rule.eq_attrib.metadata_meq32.value, flt_index->rule_id_ex[i]);

				//flt_index->rule_id_ex[idx_q6] = ext_prop->ext[cnt].rule_id;
				//idx_q6++;
			}

#if 0  /* TBD for vlan - */
			/* Handle XLAT configuration */
			if ((iptype == IPA_IP_v4) && prop->prop[cnt].is_xlat_rule && (pdn_mux_id || sIface) && is_xlat) {
				/* For vlan mpdn xlat rules will be installed with vlan id as metadata */
				if (is_dev_in_vlan_mode && IPACM_Iface::ipacmcfg->ipacm_mpdn_enable) {
					IPACMDBG("skip xlat mpdn rule id %d ext prop no. %d i %d\n",
						prop->prop[cnt].rule_id, cnt, i);
					continue;
				}

				/* for static policy, xlat rules will be installed with src_addr = XLAT PDN subnet */
				if (static_policy) {
					int meq32_n = flt_rule_entry.rule.eq_attrib.num_offset_meq_32;

					/* check if over max meq32 equatipons */
					if (meq32_n + 1 > IPA_IPFLTR_NUM_MEQ_32_EQNS) {
						IPACMERR("Can't add another meq_32 equation to this rule: %d index %d\n", cnt, index);
						continue;
					}
					flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].offset = 12;  /* SRC ADDR */
					flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].value = 0xC0000000;  /* XLAT PDN */
					flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].mask = 0xFFFFFF00;

					/* Add the bitmap that will point to the new meq32 eq */
					if (meq32_n == 0)
						flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1<<5);
					else
						flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1<<6);

					flt_rule_entry.rule.eq_attrib.num_offset_meq_32++;

					/* clear metadata bit */
					flt_rule_entry.rule.eq_attrib.rule_eq_bitmap &= ~(1<<9);
					flt_rule_entry.rule.eq_attrib.metadata_meq32_present = 0;

					/* change to pass to route since NATting is already done on 1st pass */
					flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
					flt_rule_entry.rule.set_metadata = false;

					IPACMDBG_H("xlat meta-data is modified for rule: %d has index %d with src subnet: 0x%X\n",
						cnt, index, flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].value);
				} else {
					/* fill the value of meta-data */
					value = pdn_mux_id;
					flt_rule_entry.rule.eq_attrib.metadata_meq32_present = 1;
					flt_rule_entry.rule.eq_attrib.metadata_meq32.offset = 0;
					flt_rule_entry.rule.eq_attrib.metadata_meq32.value = (value & 0xFF) << 16;
					flt_rule_entry.rule.eq_attrib.metadata_meq32.mask = 0x00FF0000;
					IPACMDBG_H("xlat meta-data is modified for rule: %d has index %d with xlat_mux_id: %d\n",
						cnt, index, pdn_mux_id);
				}
			}
#endif

			flt_rule_entry.rule.hashable = ext_prop->ext[cnt].is_rule_hashable;
			flt_rule_entry.rule.rule_id = ext_prop->ext[cnt].rule_id;
			/* Skip Metadata equation for WLAN VLAN and static policy scenarios to handle XLAT. */
/*			if (!idx && rx_prop->rx[idx].attrib.attrib_mask & IPA_FLT_META_DATA && */
/*				!(IPACM_Iface::ipacmcfg->ipacm_static_policy_enable && ext_prop->prop[cnt].is_xlat_rule)) */
			/* { //turn on meta-data equation */
				/* flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1 << 9); */
				/* flt_rule_entry.rule.eq_attrib.metadata_meq32_present = 1; */
				/* flt_rule_entry.rule.eq_attrib.metadata_meq32.offset = 0; */
				/* flt_rule_entry.rule.eq_attrib.metadata_meq32.value |= rx_prop->rx[idx].attrib.meta_data; */
				/* flt_rule_entry.rule.eq_attrib.metadata_meq32.mask |= rx_prop->rx[idx].attrib.meta_data_mask; */
				/* IPA_BE_ERR("turn on meta-data equation with value 0x%x\n", rx_prop->rx[idx].attrib.meta_data); */
			/* } */

			memcpy(&pFilteringTable->rules[i], &flt_rule_entry, sizeof(flt_rule_entry));

			IPA_BE_ERR("Modem UL filtering rule %d has index %d installed at %d\n", cnt, index, i);
			IPA_BE_ERR("Modem UL filtering rule_id %d at_rear:%d\n", flt_rule_entry.rule.rule_id, flt_rule_entry.at_rear);
			flt_index->rule_id[i] = ext_prop->ext[cnt].rule_id;
			index++;
			i++;
		}

		if (ipa3_qmi_filter_notify_send(flt_index)) {
			IPA_BE_ERR("IPACM->Q6 rule index fail\n");
			retval = -EFAULT;
			break;
		}

		if (notif_only) {
			IPA_BE_ERR("UL filtering rules already installed for %s, only sent notification for modem (mux %d)\n",
				rx_prop->name, ext_prop->ext[0].mux_id);

			if (pFilteringTable) {
				kfree(pFilteringTable);
				pFilteringTable = NULL;
			}
			retval = 0;
			continue;
		} else {
			IPA_BE_ERR("this is the first PDN for dev %s, commiting modem UL rules, mux %d\n", rx_prop->name, ext_prop->ext[0].mux_id);
		}

		if (ipa3_add_flt_rule_usr((struct ipa_ioc_add_flt_rule *)pFilteringTable,
			true)) {
			IPA_BE_ERR("ipa3_add_flt_rule_usr failed\n");
			retval = -EFAULT;
			goto end;
		} else {
			for (i = 0; i < pFilteringTable->num_rules; i++) {
				if (!pFilteringTable->rules[i].flt_rule_hdl)
					continue;

				IPA_BE_ERR("Modem uplink rules %d filter rule hdl: %d\n", lan2lan, pFilteringTable->rules[i].flt_rule_hdl);
				/* wan_ul_fl_rule_hdl_v4[j][num_wan_ul_fl_rule_v4[j]] = pFilteringTable->rules[i].flt_rule_hdl; */
				/* num_wan_ul_fl_rule_v4[j]++; */
				/* Map for dynamic insertion of xlat rules */
				/* if (is_dev_in_vlan_mode && IPACM_Iface::ipacmcfg->ipacm_mpdn_enable) */
				/* { */
				/*	xlat_ctx.ul_rule_id_hdl_map[j][i].rule_id = pFilteringTable->rules[i].rule.rule_id; */
				/*	xlat_ctx.ul_rule_id_hdl_map[j][i].flt_hdl = pFilteringTable->rules[i].flt_rule_hdl; */
				/* } */

				flt_entry.flt_hdl = pFilteringTable->rules[i].flt_rule_hdl;
				flt_entry.cat = IPA_FLT_RULE_CAT_UPLINK;  /* Modem uplink rules */
				flt_entry.ip_type = IPA_IP_v4;
				ipa3_add_filter_rules_entry(client_iface, flt_entry);
			}
			flt_rule_count_v4[rx_prop->rx[idx].src_pipe]++;
		}
		if (pFilteringTable) {
			kfree(pFilteringTable);
			pFilteringTable = NULL;
		}
	}

	if (retval == 0) {
		spin_lock_bh(&ipa_uplink_lock);
		new_pair = kzalloc(sizeof(struct ipa_uplink_pair), GFP_ATOMIC);
		if (!new_pair) {
			spin_unlock_bh(&ipa_uplink_lock);
			IPA_BE_ERR("Failed to allocate memory for uplink pair\n");
			retval = -EINVAL;
			goto end;
		}
		new_pair->pdn_iface = pdn_iface;
		new_pair->client_iface = client_iface;
		new_pair->ip_type = iptype;
		new_pair->ref_count = 1;

		/* Add to the list */
		list_add(&new_pair->node, &ipa_uplink_pairs_list);
		spin_unlock_bh(&ipa_uplink_lock);
	}

end:
	if (flt_index)
		kfree(flt_index);
	if (pFilteringTable)
		kfree(pFilteringTable);
	if (rx_prop)
		kfree(rx_prop);
	if (ext_prop)
		kfree(ext_prop);

	IPA_BE_ERR("Exit retval %d \n", retval);
	return retval;
}
EXPORT_SYMBOL(ipa_be_v4_add_uplink_filter_rule);

int ipa_be_delete_rules_by_category(int intf_num, int category, enum ipa_ip_type iptype)
{
	struct ipa3_flt_entry flt_entry = {0};
	int flt_hdl;
	int ret;

	flt_entry.cat = category;
	flt_entry.ip_type = iptype;  /* Pass IP type to filter deletion */
	for (;;) {
		flt_hdl = ipa3_delete_filter_rules_entry(intf_num, flt_entry);
		if (flt_hdl == -1)
			break;
		struct ipa_ioc_del_flt_rule flt_param = {0};

		flt_param.commit = 1;
		flt_param.num_hdls = 1;
		flt_param.ip = iptype;
		flt_param.hdl[0].status = -1;
		flt_param.hdl[0].hdl = flt_hdl;
		IPA_BE_DBG("Deleting filter hdl:(0x%x) with cat:%d ip type: %d\n", flt_hdl, category, iptype);
		ret = ipa3_del_flt_rule((struct ipa_ioc_del_flt_rule *)&flt_param);
		if (ret) {
			IPA_BE_ERR("Failed HW delete for intf:%d cat:%d ip:%d flt_hdl:%d, ipa3_del_flt_rule returned: %d\n",
				intf_num, category, iptype, flt_hdl, ret);
		} else if (flt_param.hdl[0].status != 0) {
			IPA_BE_ERR("HW delete for intf:%d cat:%d ip:%d flt_hdl:%d failed with status:%d\n",
				intf_num, category, iptype, flt_hdl, flt_param.hdl[0].status);
		} else {
			IPA_BE_ERR("Filter rule deleted successfully for intf:%d cat:%d ip:%d flt_hdl:0x%x\n",
				intf_num, category, iptype, flt_hdl);
		}
	}
	return 0;
}
EXPORT_SYMBOL(ipa_be_delete_rules_by_category);

int ipa_be_construct_mtu_rule(enum ipa_ip_type iptype, uint16_t mtu, int intf_num, int vlan_tag)
{
	struct ipa_ioc_add_flt_rule *pFilteringTable = NULL;
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa_ioc_query_intf temp_intf;
	int retval = 0, len = 0;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	int total_rules = 1;
	int idx = 0;
	int flt_hdl = 0;
	int j = 0;
	struct ipa_ioc_get_rt_tbl rt_tbl = {0};
	struct ipa3_flt_entry flt_entry = {0};

	/* Delete all existing MTU rules for this intf_num before adding new one. */
	ipa_be_delete_rules_by_category(intf_num, IPA_FLT_RULE_CAT_MTU, iptype);

	IPA_BE_ERR("ECMIPA entry mtu rule size %d\n", mtu);

	/*Check if the filter interface exists*/
	retval = ipa3_query_iface(intf_num, &temp_intf);
	if (!retval) {
		IPA_BE_DBG("Interface with index %u does not exist.\n", intf_num);
		retval = -EINVAL;
		goto end;
	}

	rx_prop = (struct ipa_ioc_query_intf_rx_props *)kzalloc(
		sizeof(struct ipa_ioc_query_intf_rx_props) +
		temp_intf.num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop),
		GFP_KERNEL);
	if (rx_prop == NULL) {
		IPA_BE_ERR("Unable to allocate rx_prop memory.\n");
		retval = -EINVAL;
		goto end;
	}

	memcpy(rx_prop->name, temp_intf.name, sizeof(temp_intf.name));
	rx_prop->num_rx_props = temp_intf.num_rx_props;
	IPA_BE_ERR("Query rx_prop %d name %s\n", rx_prop->num_rx_props, temp_intf.name);
	ipa3_query_intf_rx_props(rx_prop);

	/* avoid out-of-bounds access when logging below */
	if (rx_prop->num_rx_props > 0) {
		IPA_BE_ERR("Query response rx_prop src %d hdr_l2_type %d\n",
			rx_prop->rx[idx].src_pipe, rx_prop->rx[idx].hdr_l2_type);
	}

	len = sizeof(struct ipa_ioc_add_flt_rule) + sizeof(struct ipa_flt_rule_add);
	pFilteringTable = (struct ipa_ioc_add_flt_rule *)kzalloc(len, GFP_KERNEL);
	if (!pFilteringTable) {
		IPA_BE_ERR("Failed to allocate ipa_ioc_add_flt_rule_after memory...\n");
		retval = -EINVAL;
		goto end;
	}

	IPA_BE_ERR("Install rules on Rx pipe at idx %d src_pipe %d\n",
		idx, (rx_prop->num_rx_props > 0) ? rx_prop->rx[idx].src_pipe : -1);
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ep = (rx_prop->num_rx_props > 0) ? rx_prop->rx[idx].src_pipe : 0;
	pFilteringTable->ip = iptype; /* use requested IP type */
	pFilteringTable->num_rules = total_rules;
	/* pFilteringTable->add_after_hdl = 0; //tbd */

	for (j = 0; j < rx_prop->num_rx_props / 2; j++) {
		if (iptype == IPA_IP_v6)
			idx = j * 2 + 1;
		else
			idx = j * 2;

		if (idx >= rx_prop->num_rx_props)  /* bounds check */
			continue;

		if (rx_prop->rx[idx].ip != iptype) {
			IPA_BE_ERR("IP not matching required type %d .. continue\n", rx_prop->rx[idx].ip);
			continue;
		}

		IPA_BE_ERR("Install rules at idx %d\n", idx);
		memset(&flt_rule_entry, 0, sizeof(flt_rule_entry));

		flt_rule_entry.rule.retain_hdr = 0;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
		flt_rule_entry.rule.eq_attrib_type = 1;

		rt_tbl.ip = iptype;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(rt_tbl.name, V4_LAN_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#else
		strlcpy(rt_tbl.name, V4_LAN_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#endif

		IPA_BE_ERR("This flt rule points to rt tbl %s.\n", rt_tbl.name);

		if (ipa3_get_rt_tbl((struct ipa_ioc_get_rt_tbl *)&rt_tbl)) {
			IPA_BE_ERR("ECMIPA failed to get route hdl \n");
			retval = -EFAULT;
		}
		IPA_BE_ERR("Install filter rules with rt_tbl.hdl %d \n", rt_tbl.hdl);
		IPA_BE_ERR("Install rules on Rx pipe at idx %d \n", idx);

		flt_rule_entry.rule.rt_tbl_hdl = rt_tbl.hdl;
		flt_rule_entry.rule.hashable = true;
		/* flt_rule_entry.rule.rule_id = 1; //LAN2LAN_RULE_ID */

		if (vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED)
		{
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_VLAN_ID;
			flt_rule_entry.rule.attrib.vlan_id = vlan_tag & 0x3FF;
		}

		flt_rule_entry.flt_rule_category = IPA_FLT_RULE_CAT_MTU;

		memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[idx].attrib,
			sizeof(flt_rule_entry.rule.attrib));

		flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1<<10);
		flt_rule_entry.rule.eq_attrib.num_ihl_offset_range_16 = 1;

		if (iptype == IPA_IP_v4) {
			flt_rule_entry.rule.eq_attrib.ihl_offset_range_16[0].offset = 0x82;
			flt_rule_entry.rule.eq_attrib.ihl_offset_range_16[0].range_low = mtu + 1;
		} else {
			flt_rule_entry.rule.eq_attrib.ihl_offset_range_16[0].offset = 0x84;
			/* v6 uses payload length which doesnt include v6 header */
			flt_rule_entry.rule.eq_attrib.ihl_offset_range_16[0].range_low = mtu + 1 - IPV6_HEADER_SIZE;
		}
		flt_rule_entry.rule.eq_attrib.ihl_offset_range_16[0].range_high = UINT16_MAX; /* 0xFFFF */

		memcpy(&(pFilteringTable->rules[0]), &flt_rule_entry, sizeof(flt_rule_entry));

		if (ipa3_add_flt_rule_usr((struct ipa_ioc_add_flt_rule *)pFilteringTable, true)) {
			IPA_BE_ERR("ipa3_add_flt_rule_usr failed\n");
			retval = -EFAULT;
			goto end;
		} else {
			flt_hdl = pFilteringTable->rules[0].flt_rule_hdl;
			IPA_BE_ERR("MTU rule %d filter rule hdl: %d\n", mtu, flt_hdl);
			flt_entry.flt_hdl = flt_hdl;
			flt_entry.cat = IPA_FLT_RULE_CAT_MTU;

			ipa3_add_filter_rules_entry(intf_num, flt_entry);
		}
	}

	retval = 0;

end:
	if (pFilteringTable)
		kfree(pFilteringTable);
	if (rx_prop)
		kfree(rx_prop);
	return retval;
}

int ipa_be_v6_add_uplink_filter_rule(struct ipa_ipv6_rule_create_msg v6_msg, bool lan2lan, int pdn_iface, int client_iface, bool is_xlat)
{
	struct ipa_ioc_add_flt_rule *pFilteringTable = NULL;
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa_ioc_query_intf client_intf, pdn_intf;
	int retval = 0, len = 0;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	struct ipa_ioc_query_intf_ext_props *ext_prop = NULL;
	int total_rules = 1;
	int idx = 0;
	int j = 0, i = 0;
	struct ipa_fltr_installed_notif_req_msg_v01 *flt_index = NULL;
	int cnt;
	enum ipa_flt_action action_cache;
	enum ipa_ip_type iptype;
	int index = 0;
	bool notif_only = false;
	struct ipa3_flt_entry flt_entry = {0};
	struct ipa_uplink_pair *pair;
	struct ipa_uplink_pair *new_pair = NULL;

	iptype = IPA_IP_v6;
	IPA_BE_ERR("ECMIPA entry lan2lan %d\n", lan2lan);

	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(client_iface, &client_intf)) {
		retval = -EINVAL;
		goto end;
	} else {
		IPA_BE_DBG("Interface with index %u exist.\n", client_iface);
	}

	spin_lock_bh(&ipa_uplink_lock);
	list_for_each_entry(pair, &ipa_uplink_pairs_list, node) {
		if (pair->pdn_iface == pdn_iface && pair->client_iface == client_iface && pair->ip_type == iptype) {
			pair->ref_count++;
			spin_unlock_bh(&ipa_uplink_lock);
			IPA_BE_DBG("Uplink filter for pdn %d, client %d, IP type %d already installed. ref_count: %d\n",
				pdn_iface, client_iface, iptype, pair->ref_count);
			return 0; /* Rules already exist, just increment ref count and exit */
		}
	}
	spin_unlock_bh(&ipa_uplink_lock);

	IPA_BE_DBG("Created new uplink pair tracking for pdn %d, client %d\n", pdn_iface, client_iface);

	rx_prop = (struct ipa_ioc_query_intf_rx_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_rx_props) +
		client_intf.num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (rx_prop == NULL) {
		IPA_BE_ERR("Unable to allocate rx_prop memory.\n");
		retval = -EINVAL;
		goto end;
	}
	memcpy(rx_prop->name, client_intf.name, sizeof(client_intf.name));
	rx_prop->num_rx_props = client_intf.num_rx_props;
	IPA_BE_ERR("Query rx_prop %d name %s\n", rx_prop->num_rx_props, client_intf.name);
	ipa3_query_intf_rx_props(rx_prop);
	IPA_BE_ERR("Query response rx_prop src %d hdr_l2_type %d\n", rx_prop->rx[idx].src_pipe,
		rx_prop->rx[idx].hdr_l2_type);

	/* ext Props */
	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(pdn_iface, &pdn_intf)) {
		retval = -EINVAL;
		goto end;
	} else {
		IPA_BE_DBG("Interface with index %u exist.\n", pdn_iface);
	}
	ext_prop = (struct ipa_ioc_query_intf_ext_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_ext_props) +
		pdn_intf.num_ext_props * sizeof(struct ipa_ioc_ext_intf_prop), GFP_KERNEL);
	if (ext_prop == NULL) {
		IPA_BE_ERR("Unable to allocate ext_prop memory.\n");
		retval = -EINVAL;
		goto end;
	}
	memcpy(ext_prop->name, pdn_intf.name, sizeof(pdn_intf.name));
	ext_prop->num_ext_props = pdn_intf.num_ext_props;
	IPA_BE_DBG("Query ext_prop %d name %s is_xlat %d\n", ext_prop->num_ext_props, ext_prop->name, is_xlat);
	ipa3_query_intf_ext_props(ext_prop);

	flt_index = (struct ipa_fltr_installed_notif_req_msg_v01 *)kzalloc(
		sizeof(struct ipa_fltr_installed_notif_req_msg_v01), GFP_KERNEL);

	total_rules = ext_prop->num_ext_props;

	/*for IPv6CT enabled mode, duplicate the pass to NAT modem UL rules and change to pass to route for XLAT packets */
	int v6_xlat_ul_rules = 0;
	if (is_xlat)
	{
		IPA_BE_DBG("IPv6CT is enabled, need pass to route modem UL rules for XLAT packets\n");
		for(i = 0; i < total_rules; i++)
			if(ext_prop->ext[i].action != IPA_PASS_TO_EXCEPTION)
				v6_xlat_ul_rules++;

		total_rules = total_rules + v6_xlat_ul_rules;
		IPA_BE_DBG("Need %d additional XLAT rules\n", v6_xlat_ul_rules);
	}

	for (j = 0; j < rx_prop->num_rx_props / 2; j++) {
		idx = j * 2 + 1;
		if (rx_prop->rx[idx].ip != IPA_IP_v6) {
			IPA_BE_ERR("IP not matching required type %d .. continue\n", rx_prop->rx[idx].ip);
			continue;
		}
		IPA_BE_ERR("Install rules at idx %d\n", idx);

		memset(flt_index, 0, sizeof(struct ipa_fltr_installed_notif_req_msg_v01));
		flt_index->source_pipe_index = ipa_get_ep_mapping(rx_prop->rx[idx].src_pipe);
		flt_index->install_status = IPA_QMI_RESULT_SUCCESS_V01;

		flt_index->rule_id_valid = 1;
		flt_index->rule_id_len = total_rules;
		flt_index->embedded_pipe_index_valid = 1;
		flt_index->embedded_pipe_index = ipa_get_ep_mapping(IPA_CLIENT_APPS_LAN_WAN_PROD);
		flt_index->retain_header_valid = 1;
		flt_index->retain_header = 0;
		flt_index->embedded_call_mux_id_valid = 1;
/*
#ifdef FEATURE_VLAN_MPDN
		if (is_xlat &&
		   (!(is_dev_in_vlan_mode || static_policy) &&
		   IPACM_Iface::ipacmcfg->ipacm_mpdn_enable))
			flt_index.embedded_call_mux_id = IPACM_Iface::ipacmcfg->GetQmapId();
		else
			flt_index.embedded_call_mux_id = pdn_mux_id;
#else
		flt_index.embedded_call_mux_id = IPACM_Iface::ipacmcfg->GetQmapId();
#endif
*/
		flt_index->embedded_call_mux_id = ext_prop->ext[0].mux_id;

		IPA_BE_ERR("flt_index: src pipe: %d, num of rules: %d, ebd pipe: %d, mux id: %d\n",
				   flt_index->source_pipe_index, flt_index->filter_index_list_len, flt_index->embedded_pipe_index, flt_index->embedded_call_mux_id);

		len = sizeof(struct ipa_ioc_add_flt_rule) +
			total_rules * sizeof(struct ipa_flt_rule_add);

		pFilteringTable =
			(struct ipa_ioc_add_flt_rule *)kzalloc(len, GFP_KERNEL);

		if (pFilteringTable == NULL) {
			IPA_BE_ERR("Error Locate ipa_flt_rule_add memory...\n");
			retval = -EINVAL;
			goto end;
		}
		memset(pFilteringTable, 0, len);

		pFilteringTable->commit = 1;
		pFilteringTable->ep = rx_prop->rx[idx].src_pipe;
		pFilteringTable->global = false;
		pFilteringTable->ip = iptype;
		pFilteringTable->num_rules = total_rules;

		for (cnt = i = 0; cnt < ext_prop->num_ext_props && i < total_rules; cnt++)
		{
			if (ext_prop->ext[cnt].ip != IPA_IP_v6)
			{
				IPA_BE_ERR("iptype mismatch %d .. continue\n", ext_prop->ext[cnt].ip);
				continue;
			}

			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add)); // Zero All Fields
			flt_rule_entry.at_rear = 1;

			if (flt_rule_entry.rule.eq_attrib.ipv4_frag_eq_present)
				flt_rule_entry.at_rear = 0;

			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;

			flt_rule_entry.rule.retain_hdr = 0;
			flt_rule_entry.rule.to_uc = 0;
			flt_rule_entry.rule.eq_attrib_type = 1;

			flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;
			/* cache the flt action */
			action_cache = flt_rule_entry.rule.action;

			flt_rule_entry.rule.set_metadata = true;
			flt_rule_entry.flt_rule_category = IPA_FLT_RULE_CAT_UPLINK;

			index = flt_rule_count_v6[rx_prop->rx[idx].src_pipe];

			memcpy(&flt_rule_entry.rule.eq_attrib,
				   &ext_prop->ext[cnt].eq_attrib,
				   sizeof(ext_prop->ext[cnt].eq_attrib));

			/* Populate the flt rule action from ext_prop */
			if (ext_prop->ext[cnt].action == IPA_PASS_TO_EXCEPTION) {
				/* Override the rule action if Q6 can't handle it, go A7 exception */
				flt_rule_entry.rule.action = ext_prop->ext[cnt].action;
				flt_rule_entry.rule.rt_tbl_idx = 0;
				IPA_BE_ERR("Override rule index %d to act: %d, rt_tbl_idx: %d to %d\n",
						   cnt, flt_rule_entry.rule.action,
						   ext_prop->ext[cnt].rt_tbl_idx,
						   flt_rule_entry.rule.rt_tbl_idx);
			} else {
				/* restore the rule action */
				flt_rule_entry.rule.action = action_cache;
				flt_rule_entry.rule.rt_tbl_idx = ext_prop->ext[cnt].rt_tbl_idx;
				IPA_BE_ERR("Restore rule index %d to act: %d, rt_tbl_idx: %d \n",
						   cnt, flt_rule_entry.rule.action,
						   flt_rule_entry.rule.rt_tbl_idx);
			}

			flt_rule_entry.rule.hashable = ext_prop->ext[cnt].is_rule_hashable;
			flt_rule_entry.rule.rule_id = ext_prop->ext[cnt].rule_id;
			/* Skip Metadata equation for WLAN VLAN and static policy scenarios to handle XLAT. */
//			if (!idx && rx_prop->rx[idx].attrib.attrib_mask & IPA_FLT_META_DATA &&
//				!(IPACM_Iface::ipacmcfg->ipacm_static_policy_enable && ext_prop->prop[cnt].is_xlat_rule))
			//{ //turn on meta-data equation
				//flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1 << 9);
				//flt_rule_entry.rule.eq_attrib.metadata_meq32_present = 1;
				//flt_rule_entry.rule.eq_attrib.metadata_meq32.offset = 0;
				//flt_rule_entry.rule.eq_attrib.metadata_meq32.value |= rx_prop->rx[idx].attrib.meta_data;
				//flt_rule_entry.rule.eq_attrib.metadata_meq32.mask |= rx_prop->rx[idx].attrib.meta_data_mask;
				//IPA_BE_ERR("turn on meta-data equation with value 0x%x\n", rx_prop->rx[idx].attrib.meta_data);
			//}

			memcpy(&pFilteringTable->rules[i], &flt_rule_entry, sizeof(flt_rule_entry));

			IPA_BE_ERR("Modem UL filtering rule %d has index %d installed at %d\n", cnt, index, i);
			IPA_BE_ERR("Modem UL filtering rule_id %d\n", flt_rule_entry.rule.rule_id);
			flt_index->rule_id[i] = ext_prop->ext[cnt].rule_id;
			index++;
			i++;

			//for IPv6CT enabled and XLAT, add a duplicate rule above that will let XLAT packets go to routing instead of NAT
			if (is_xlat && flt_rule_entry.rule.action != IPA_PASS_TO_EXCEPTION) {
				//duplicate the old rule to new index
				memcpy(&pFilteringTable->rules[i], &flt_rule_entry, sizeof(flt_rule_entry));

				//change old rule to pass to route and non hashable
				flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
				flt_rule_entry.rule.hashable = false;

				//add the eth header equation for v4 to the old rule
				int meq32_n = flt_rule_entry.rule.eq_attrib.num_offset_meq_32;

				if (meq32_n + 1 > IPA_IPFLTR_NUM_MEQ_32_EQNS) {
					IPA_BE_ERR("Can't add another meq_32 equation to this rule");
					memcpy(&pFilteringTable->rules[cnt], &flt_rule_entry, sizeof(flt_rule_entry));
					continue;
				}

				flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].offset = -4;
				flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].mask = 0xFFFF;
				flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].value = 0x0800;

				//Add the bitmap that will point to the new meq32 eq
				if (meq32_n == 0) flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1 << 5);
				else flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1 << 6);

				flt_rule_entry.rule.eq_attrib.num_offset_meq_32++;

				IPA_BE_DBG("Xlat rule %d flt_index rule id %d\n", flt_rule_entry.rule.eq_attrib.num_offset_meq_32, flt_index->rule_id[i]);

				//overwrite the old rule and increment the rule count
				memcpy(&pFilteringTable->rules[i - 1], &flt_rule_entry, sizeof(flt_rule_entry));
				index++;
				i++;
			}
		}

		if (ipa3_qmi_filter_notify_send(flt_index))
		{
			IPA_BE_ERR("IPACM->Q6 rule index fail\n");
			retval = -EFAULT;
			break;
		}

		if (notif_only) {
			IPA_BE_ERR("UL filtering rules already installed for %s, only sent notification for modem (mux %d)\n",
					   rx_prop->name,  ext_prop->ext[0].mux_id);

			if (pFilteringTable)
			{
				kfree(pFilteringTable);
				pFilteringTable = NULL;
			}
			retval = 0;
			continue;
		} else {
			IPA_BE_ERR("this is the first PDN for dev %s, commiting modem UL rules, mux %d\n", rx_prop->name, ext_prop->ext[0].mux_id);
		}

		if (ipa3_add_flt_rule_usr((struct ipa_ioc_add_flt_rule *)pFilteringTable,
				true)) {
			IPA_BE_ERR("ipa3_add_flt_rule_usr failed\n");
			retval = -EFAULT;
			goto end;
		}
		else
		{
			for (i = 0; i < pFilteringTable->num_rules; i++) {
				if (!pFilteringTable->rules[i].flt_rule_hdl)
				{
					continue;
				}
				IPA_BE_ERR("Modem uplink rules %d filter rule hdl: %d\n", lan2lan, pFilteringTable->rules[i].flt_rule_hdl);

				flt_entry.flt_hdl = pFilteringTable->rules[i].flt_rule_hdl;
				flt_entry.cat = IPA_FLT_RULE_CAT_UPLINK;  // Modem uplink rules v6
				flt_entry.ip_type = IPA_IP_v6;
				ipa3_add_filter_rules_entry(client_iface, flt_entry);
			}
		}
		if (pFilteringTable)
		{
			kfree(pFilteringTable);
			pFilteringTable = NULL;
		}
	}

	if (retval == 0)
	{
		spin_lock_bh(&ipa_uplink_lock);
		new_pair = kzalloc(sizeof(struct ipa_uplink_pair), GFP_ATOMIC);
		if (!new_pair) {
			spin_unlock_bh(&ipa_uplink_lock);
			IPA_BE_ERR("Failed to allocate memory for uplink pair\n");
			retval = -EINVAL;
			goto end;
		}
		new_pair->pdn_iface = pdn_iface;
		new_pair->client_iface = client_iface;
		new_pair->ip_type = iptype;
		new_pair->ref_count = 1;

		// Add to the list
		list_add(&new_pair->node, &ipa_uplink_pairs_list);
		spin_unlock_bh(&ipa_uplink_lock);
	}

end:
	if (flt_index)
		kfree(flt_index);
	if (pFilteringTable)
		kfree(pFilteringTable);
	if (rx_prop)
		kfree(rx_prop);
	if (ext_prop)
		kfree(ext_prop);

	IPA_BE_ERR("Exit retval %d \n", retval);
	return retval;
}
EXPORT_SYMBOL(ipa_be_v6_add_uplink_filter_rule);

int ipa_be_v6_add_filter_rule(struct ipa_ipv6_rule_create_msg v6_msg, bool lan2lan, int intf_num, mac_addr_t mac)
{
	struct ipa_ioc_add_flt_rule *pFilteringTable = NULL;
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa_ioc_query_intf temp_intf;
	int retval = 0, len = 0;
	struct ipa_ioc_query_intf_rx_props *rx_prop;
	int total_rules = 1;
	int idx = 0;
	int flt_hdl = 0;
	struct ipa_ioc_get_rt_tbl rt_tbl = {0};
	struct ipa3_flt_entry flt_entry = {0};

	IPA_BE_ERR("ECMIPA entry lan2lan %d\n", lan2lan);
	ipa_type_check_ipa_mac_addr(mac);

	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(intf_num, &temp_intf)) {
		return -EINVAL;
	} else {
		IPA_BE_DBG("Interface with index %u does not exist.\n", intf_num);
	}

	rx_prop = (struct ipa_ioc_query_intf_rx_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_rx_props) +
		temp_intf.num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (rx_prop == NULL) {
		IPA_BE_ERR("Unable to allocate rx_prop memory.\n");
		return -EINVAL;
	}
	memcpy(rx_prop->name, temp_intf.name, sizeof(temp_intf.name));
	rx_prop->num_rx_props = temp_intf.num_rx_props;
	IPA_BE_ERR("Query rx_prop %d name %s\n", rx_prop->num_rx_props, temp_intf.name);
	ipa3_query_intf_rx_props(rx_prop);
	IPA_BE_ERR("Query response rx_prop src %d hdr_l2_type %d\n", rx_prop->rx[idx].src_pipe,
		rx_prop->rx[idx].hdr_l2_type);

	if (lan2lan) {
		len = sizeof(struct ipa_ioc_add_flt_rule) + sizeof(struct ipa_flt_rule_add);
		pFilteringTable = (struct ipa_ioc_add_flt_rule *)kzalloc(len, GFP_KERNEL);
		if (!pFilteringTable) {
			IPA_BE_ERR("Failed to allocate ipa_ioc_add_flt_rule_after memory...\n");
			return -EINVAL;
		}

		IPA_BE_ERR("Install rules on Rx pipe at idx %d src_pipe %d\n", idx, rx_prop->rx[idx].src_pipe);
		memset(pFilteringTable, 0, len);

		/* add mac based rule*/
		pFilteringTable->commit = 1;
		pFilteringTable->ep = rx_prop->rx[idx].src_pipe;
		pFilteringTable->ip = IPA_IP_v6;
		pFilteringTable->num_rules = total_rules;
		/* pFilteringTable->add_after_hdl = 0; //tbd */

		memset(&flt_rule_entry, 0, sizeof(flt_rule_entry));

		flt_rule_entry.rule.retain_hdr = 0;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule_entry.rule.eq_attrib_type = 0;

		rt_tbl.ip = IPA_IP_v6;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(rt_tbl.name, V6_WAN_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#else
		strlcpy(rt_tbl.name, V6_WAN_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#endif

		IPA_BE_ERR("This flt rule points to rt tbl %s.\n", rt_tbl.name);

		if (ipa3_get_rt_tbl((struct ipa_ioc_get_rt_tbl *)&rt_tbl)) {
			IPA_BE_ERR("ECMIPA failed to get route hdl \n");
			retval = -EFAULT;
		}
		IPA_BE_ERR("Install filter rules with rt_tbl.hdl %d \n", rt_tbl.hdl);

		IPA_BE_ERR("Install rules on Rx pipe at idx %d \n", idx);
		flt_rule_entry.rule.rt_tbl_hdl = rt_tbl.hdl;

		flt_rule_entry.rule.hashable = true;
		flt_rule_entry.rule.rule_id = 1; /* LAN2LAN_RULE_ID */

		memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[idx].attrib, sizeof(flt_rule_entry.rule.attrib));

		if (v6_msg.return_vlan_filter_rule.egress_vlan_tag) {
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_VLAN_ID;
			flt_rule_entry.rule.attrib.vlan_id = v6_msg.return_vlan_filter_rule.egress_vlan_tag;
		}

		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		for (int i = 0; i < 4;i++)
		{
			flt_rule_entry.rule.attrib.u.v6.dst_addr[i] = (uint32_t)ntohl(v6_msg.tuple.return_ip[i]);
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[i] = 0xFFFFFFFF;
		}

		memcpy(&(pFilteringTable->rules[0]), &flt_rule_entry, sizeof(flt_rule_entry));

		if (ipa3_add_flt_rule_usr((struct ipa_ioc_add_flt_rule *)pFilteringTable,
			true)) {
			IPA_BE_ERR("ipa3_add_flt_rule_usr failed\n");
			retval = -EFAULT;
			goto end;
		} else {
			flt_hdl = pFilteringTable->rules[0].flt_rule_hdl;
			IPA_BE_ERR("Lan2Lan %d filter rule hdl: %d\n", lan2lan, flt_hdl);
			flt_entry.flt_hdl = flt_hdl;
			flt_entry.cat = 0;

			flt_entry.rule.attrib.attrib_mask = flt_rule_entry.rule.attrib.attrib_mask;
			memcpy(flt_entry.rule.attrib.dst_mac_addr, mac, sizeof(flt_entry.rule.attrib.dst_mac_addr));

			ipa3_add_filter_rules_entry(intf_num, flt_entry);
		}
	} else {
		IPA_BE_ERR("Lan2wan case \n");
	}


end:
	if (pFilteringTable)
		kfree(pFilteringTable);
	if (rx_prop)
		kfree(rx_prop);

	IPA_BE_ERR("Exit retval %d \n", retval);
	return retval;
}

EXPORT_SYMBOL(ipa_be_v6_add_filter_rule);

int ipa_be_v4_delete_filter_rule(struct ipa_ipv4_rule_destroy_msg v4_msg, int intf_idx, mac_addr_t mac, bool lan2lan)
{
	struct ipa_ioc_del_flt_rule flt_param = {0};
	int retval = 0;
	int num_rules = 1;
	int flt_hdl = -1;
	struct ipa3_flt_entry flt_entry = {0};

	IPA_BE_ERR("Entry ipa_be_v4_delete_filter_rule for mac %pM \n", mac);

	if (lan2lan) {
		flt_entry.cat = IPA_FLT_RULE_CAT_LAN2LAN;

		/* Match the same attributes used in add function */
		flt_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		flt_entry.rule.attrib.u.v4.dst_addr = (uint32_t)ntohl(v4_msg.tuple.return_ip);
		flt_entry.rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;

		flt_hdl = ipa3_delete_filter_rules_entry(intf_idx, flt_entry);
		if (flt_hdl == -1) {
			IPA_BE_ERR("No matching filter exists flt_hdl: %d \n", flt_hdl);
			retval = -1;
			goto fail;
		}


		/* Delete filter rules here */
		flt_param.commit = 1;
		flt_param.num_hdls = num_rules;
		flt_param.ip = IPA_IP_v4;

		flt_param.hdl[0].status = -1;
		flt_param.hdl[0].hdl = flt_hdl;
		IPA_BE_DBG("Deleting filter hdl:(0x%x) with ip type: %d\n", flt_hdl, IPA_IP_v4);

		if (ipa3_del_flt_rule((struct ipa_ioc_del_flt_rule *)&flt_param)) {
			IPA_BE_ERR("ipa3_del_flt_rule failed\n");
			retval = -EFAULT;
		} else {
			if (flt_param.hdl[0].status != 0) {
				IPA_BE_ERR("Filter rule hdl 0x%x deletion failed with error:%d\n",
				flt_param.hdl[0].hdl, flt_param.hdl[0].status);
				retval = -1;
				goto fail;
			}
			IPA_BE_ERR("Filter rule deleted successfully.. lan2lan: %d hdl: %d\n", lan2lan, flt_hdl);
		}
	}

fail:
	return retval;
}
EXPORT_SYMBOL(ipa_be_v4_delete_filter_rule);

int ipa_be_v6_delete_filter_rule(struct ipa_ipv6_rule_destroy_msg v6_msg, int intf_idx, mac_addr_t mac, bool lan2lan)
{
	struct ipa_ioc_del_flt_rule flt_param = {0};
	int retval = 0;
	int num_rules = 1;
	int flt_hdl = -1;
	struct ipa3_flt_entry flt_entry = {0};

	IPA_BE_ERR("Entry ipa_be_v4_delete_filter_rule for mac %pM \n", mac);

	if (lan2lan) {
		flt_entry.rule.attrib.attrib_mask |= IPA_FLT_MAC_DST_ADDR_ETHER_II;
		memcpy(flt_entry.rule.attrib.dst_mac_addr, mac, sizeof(flt_entry.rule.attrib.dst_mac_addr));

		flt_hdl = ipa3_delete_filter_rules_entry(intf_idx, flt_entry);
		if (flt_hdl == -1) {
			IPA_BE_ERR("No matching filter exists flt_hdl: %d \n", flt_hdl);
			retval = -1;
			goto fail;
		}

		/* Delete filter rules here */
		flt_param.commit = 1;
		flt_param.num_hdls = num_rules;
		flt_param.ip = IPA_IP_v6;

		flt_param.hdl[0].status = -1;
		flt_param.hdl[0].hdl = flt_hdl;
		IPA_BE_DBG("Deleting filter hdl:(0x%x) with ip type: %d\n", flt_hdl, IPA_IP_v6);

		if (ipa3_del_flt_rule((struct ipa_ioc_del_flt_rule *)&flt_param)) {
			IPA_BE_ERR("ipa3_del_flt_rule failed\n");
			retval = -EFAULT;
		} else {
			if (flt_param.hdl[0].status != 0) {
				IPA_BE_ERR("Filter rule hdl 0x%x deletion failed with error:%d\n",
				flt_param.hdl[0].hdl, flt_param.hdl[0].status);
				retval = -1;
				goto fail;
			}
			IPA_BE_ERR("Filter rule deleted successfully.. lan2lan: %d hdl: %d\n", lan2lan, flt_hdl);
		}
	}

fail:
	return retval;
}
EXPORT_SYMBOL(ipa_be_v6_delete_filter_rule);

int ipa_be_v4_delete_uplink_filter_rule(struct ipa_ipv4_rule_destroy_msg v4_msg, int pdn_iface, int client_iface)
{
	int retval = -1;
	struct ipa_uplink_pair *pair, *tmp;
	bool found = false;
	bool is_xlat = false;
	bool delete_ul_rules = true;

	IPA_BE_ERR("Entry ipa_be_v4_delete_filter_rule for pdn_iface %d client_iface %d \n", pdn_iface, client_iface);

	/* Check if this was XLAT traffic - same detection logic as in add function */
	if (((ntohl(v4_msg.tuple.flow_ip) & 0xFFFFFF00U) == 0xC0000000U) ||
	    ((ntohl(v4_msg.tuple.return_ip) & 0xFFFFFF00U) == 0xC0000000U) ||
	    ((ntohl(v4_msg.conn_rule.flow_ip_xlate) & 0xFFFFFF00U) == 0xC0000000U) ||
	    ((ntohl(v4_msg.conn_rule.return_ip_xlate & 0xFFFFFF00U) == 0xC0000000U))) {
		is_xlat = true;
		IPA_BE_DBG("XLAT traffic detected during deletion: flow %pI4n return %pI4n\n",
			   &v4_msg.tuple.flow_ip, &v4_msg.tuple.return_ip);
	}

	spin_lock_bh(&ipa_uplink_lock);
	list_for_each_entry_safe(pair, tmp, &ipa_uplink_pairs_list, node) {
		if (pair->pdn_iface == pdn_iface && pair->client_iface == client_iface && pair->ip_type == IPA_IP_v4) {
			pair->ref_count--;
			IPA_BE_DBG("Decremented ref_count for uplink pair pdn %d, client %d, IP type %d. New count: %d\n",
				pdn_iface, client_iface, pair->ip_type, pair->ref_count);
			if (pair->ref_count > 0) {
				/* Other connections are still using these rules, so don't delete them */
				spin_unlock_bh(&ipa_uplink_lock);
				return 0;
			}
			/* ref_count is 0, proceed with deletion and remove tracking entry */
			list_del(&pair->node);
			kfree(pair);
			found = true;
			break;
		}
	}

	/* Check if there are any other pairs using the same client_iface with this IP type */
	if (found) {
		list_for_each_entry(pair, &ipa_uplink_pairs_list, node) {
			if (pair->client_iface == client_iface && pair->ip_type == IPA_IP_v4) {
				delete_ul_rules = false;
				IPA_BE_DBG("Found another pair using client_iface %d with pdn %d, IP type %d. Not deleting rules.\n",
					client_iface, pair->pdn_iface, pair->ip_type);
				break;
			}
		}
	}
	spin_unlock_bh(&ipa_uplink_lock);

	if (!found) {
		IPA_BE_ERR("No uplink filter tracking entry found for pdn %d, client %d. Cannot delete.\n",
			pdn_iface, client_iface);
		return -ENOENT;
	}

	if (!delete_ul_rules) {
		IPA_BE_DBG("Other pairs still using client_iface %d. Skipping hardware rule deletion.\n", client_iface);
		return 0;
	}

	IPA_BE_DBG("No other pairs using client_iface %d, proceeding with hardware rule deletion for pdn %d, client %d.\n",
		client_iface, pdn_iface, client_iface);

	retval = ipa_be_delete_rules_by_category(client_iface, IPA_FLT_RULE_CAT_UPLINK, IPA_IP_v4);
	if (retval == 0) {
		IPA_BE_ERR("Uplink filter rules deleted successfully for client_iface:%d\n", client_iface);
	} else {
		IPA_BE_ERR("Failed to delete uplink filter rules for client_iface:%d, retval=%d\n", client_iface, retval);
	}

	/* If this was XLAT traffic, also delete the corresponding v6 uplink filter */
	if (is_xlat) {
		struct ipa_ipv6_rule_destroy_msg v6_msg_xlat;
		memset(&v6_msg_xlat, 0, sizeof(v6_msg_xlat));

		IPA_BE_DBG("Deleting corresponding v6 uplink filter for XLAT traffic\n");
		ipa_be_v6_delete_uplink_filter_rule(v6_msg_xlat, pdn_iface, client_iface);
	}

	return retval;

}
EXPORT_SYMBOL(ipa_be_v4_delete_uplink_filter_rule);

int ipa_be_v6_delete_uplink_filter_rule(struct ipa_ipv6_rule_destroy_msg v6_msg, int pdn_iface, int client_iface)
{
	int retval = -1;
	struct ipa_uplink_pair *pair, *tmp;
	bool found = false;
	bool delete_ul_rules = true;

	IPA_BE_ERR("Entry ipa_be_v6_delete_uplink_filter_rule for pdn_iface %d client_iface %d \n", pdn_iface, client_iface);

	spin_lock_bh(&ipa_uplink_lock);
	list_for_each_entry_safe(pair, tmp, &ipa_uplink_pairs_list, node) {
		if (pair->pdn_iface == pdn_iface && pair->client_iface == client_iface && pair->ip_type == IPA_IP_v6) {
			pair->ref_count--;
			IPA_BE_DBG("Decremented ref_count for uplink pair pdn %d, client %d, IP type %d. New count: %d\n",
				pdn_iface, client_iface, pair->ip_type, pair->ref_count);
			if (pair->ref_count > 0) {
				/* Other connections are still using these rules, so don't delete them */
				spin_unlock_bh(&ipa_uplink_lock);
				return 0;
			}
			/* ref_count is 0, proceed with deletion and remove tracking entry */
			list_del(&pair->node);
			kfree(pair);
			found = true;
			break;
		}
	}

	/* Check if there are any other pairs using the same client_iface with this IP type */
	if (found) {
		list_for_each_entry(pair, &ipa_uplink_pairs_list, node) {
			if (pair->client_iface == client_iface && pair->ip_type == IPA_IP_v6) {
				delete_ul_rules = false;
				IPA_BE_DBG("Found another pair using client_iface %d with pdn %d, IP type %d. Not deleting rules.\n",
					client_iface, pair->pdn_iface, pair->ip_type);
				break;
			}
		}
	}
	spin_unlock_bh(&ipa_uplink_lock);

	if (!found) {
		IPA_BE_ERR("No uplink filter tracking entry found for pdn %d, client %d. Cannot delete.\n",
			pdn_iface, client_iface);
		return -ENOENT;
	}

	if (!delete_ul_rules) {
		IPA_BE_DBG("Other pairs still using client_iface %d. Skipping hardware rule deletion.\n", client_iface);
		return 0;
	}

	IPA_BE_DBG("No other pairs using client_iface %d, proceeding with hardware rule deletion for pdn %d, client %d.\n",
		client_iface, pdn_iface, client_iface);

	retval = ipa_be_delete_rules_by_category(client_iface, IPA_FLT_RULE_CAT_UPLINK, IPA_IP_v6);
	if (retval == 0) {
		IPA_BE_ERR("Uplink filter rules deleted successfully for client_iface:%d\n", client_iface);
	} else {
		IPA_BE_ERR("Failed to delete uplink filter rules for client_iface:%d, retval=%d\n", client_iface, retval);
	}
	return retval;
}
EXPORT_SYMBOL(ipa_be_v6_delete_uplink_filter_rule);

bool AddWanDLFilteringRule(struct ipa_ioc_add_flt_rule const *rule_table_v4,
			   struct ipa_ioc_add_flt_rule const *rule_table_v6,
			   uint8_t *mux_id_v4, uint8_t *mux_id_v6)
{
	int ret = 0, cnt, num_rules = 0, pos = 0;
	struct ipa_install_fltr_rule_req_ex_msg_v01 *qmi_rule_ex_msg;

	qmi_rule_ex_msg = kzalloc(sizeof(*qmi_rule_ex_msg), GFP_KERNEL);
	if (!qmi_rule_ex_msg) {
		IPA_BE_ERR("Failed to allocate memory for qmi_rule_ex_msg\n");
		return false;
	}

	if (rule_table_v4) {
		num_rules += rule_table_v4->num_rules;
		IPA_BE_DBG("Get %d WAN DL IPv4 filtering rules.\n",
			   rule_table_v4->num_rules);
	}
	if (rule_table_v6) {
		num_rules += rule_table_v6->num_rules;
		IPA_BE_DBG("Get %d WAN DL IPv6 filtering rules.\n",
			   rule_table_v6->num_rules);
	}

	if (num_rules > QMI_IPA_MAX_FILTERS_EX_V01) {
		IPA_BE_ERR("The number of filtering rules exceed limit.\n");
		kfree(qmi_rule_ex_msg);
		return false;
	}

	if (num_rules > 0)
		qmi_rule_ex_msg->filter_spec_ex_list_valid = true;
	else
		qmi_rule_ex_msg->filter_spec_ex_list_valid = false;

	qmi_rule_ex_msg->filter_spec_ex_list_len = num_rules;
	qmi_rule_ex_msg->source_pipe_index_valid = 0;

	IPA_BE_DBG("Get %d WAN DL filtering rules in total.\n", num_rules);

	if (rule_table_v4) {
		for (cnt = rule_table_v4->num_rules - 1; cnt >= 0; cnt--) {
			if (pos >= QMI_IPA_MAX_FILTERS_EX_V01) {
				IPA_BE_ERR("QMI only support max %d rules, current (%d)\n",
					   QMI_IPA_MAX_FILTERS_EX_V01, pos);
				break;
			}
			qmi_rule_ex_msg->filter_spec_ex_list[pos].ip_type =
				QMI_IPA_IP_TYPE_V4_V01;
			qmi_rule_ex_msg->filter_spec_ex_list[pos].filter_action =
				GetQmiFilterAction(
					rule_table_v4->rules[cnt].rule.action);
			qmi_rule_ex_msg->filter_spec_ex_list[pos]
				.is_routing_table_index_valid = 1;
			qmi_rule_ex_msg->filter_spec_ex_list[pos]
				.route_table_index =
				rule_table_v4->rules[cnt].rule.rt_tbl_idx;
			if (mux_id_v4) {
				qmi_rule_ex_msg->filter_spec_ex_list[pos]
					.is_mux_id_valid = 1;
				qmi_rule_ex_msg->filter_spec_ex_list[pos]
					.mux_id = mux_id_v4[cnt];
			}
			qmi_rule_ex_msg->filter_spec_ex_list[pos].rule_id =
				rule_table_v4->rules[cnt].rule.rule_id;
			qmi_rule_ex_msg->filter_spec_ex_list[pos]
				.is_rule_hashable =
				rule_table_v4->rules[cnt].rule.hashable;
			memcpy(&qmi_rule_ex_msg->filter_spec_ex_list[pos]
					.filter_rule,
			       &rule_table_v4->rules[cnt].rule.eq_attrib,
			       sizeof(struct ipa_filter_rule_type_v01));

			pos++;
		}
	}

	if (rule_table_v6) {
		for (cnt = rule_table_v6->num_rules - 1; cnt >= 0; cnt--) {
			if (pos >= QMI_IPA_MAX_FILTERS_EX_V01) {
				IPA_BE_ERR("QMI only support max %d rules, current (%d)\n",
					   QMI_IPA_MAX_FILTERS_EX_V01, pos);
				break;
			}
			qmi_rule_ex_msg->filter_spec_ex_list[pos].ip_type =
				QMI_IPA_IP_TYPE_V6_V01;
			qmi_rule_ex_msg->filter_spec_ex_list[pos].filter_action =
				GetQmiFilterAction(
					rule_table_v6->rules[cnt].rule.action);
			qmi_rule_ex_msg->filter_spec_ex_list[pos]
				.is_routing_table_index_valid = 1;
			qmi_rule_ex_msg->filter_spec_ex_list[pos]
				.route_table_index =
				rule_table_v6->rules[cnt].rule.rt_tbl_idx;
			if (mux_id_v6) {
				qmi_rule_ex_msg->filter_spec_ex_list[pos]
					.is_mux_id_valid = 1;
				qmi_rule_ex_msg->filter_spec_ex_list[pos]
					.mux_id = mux_id_v6[cnt];
			}
			qmi_rule_ex_msg->filter_spec_ex_list[pos].rule_id =
				rule_table_v6->rules[cnt].rule.rule_id;
			qmi_rule_ex_msg->filter_spec_ex_list[pos]
				.is_rule_hashable =
				rule_table_v6->rules[cnt].rule.hashable;
			memcpy(&qmi_rule_ex_msg->filter_spec_ex_list[pos]
					.filter_rule,
			       &rule_table_v6->rules[cnt].rule.eq_attrib,
			       sizeof(struct ipa_filter_rule_type_v01));

			pos++;
		}
	}
	IPA_BE_DBG("Calling the IOCTL to add %d num rules\n", num_rules);
	ret = ipa3_qmi_filter_request_ex_send(qmi_rule_ex_msg);
	if (ret) {
		IPA_BE_ERR("Failed adding Filtering rule %p with ret %d\n",
			   qmi_rule_ex_msg, ret);
		kfree(qmi_rule_ex_msg);
		return false;
	}
	IPA_BE_DBG("Success adding %d num rules with ret %d\n", num_rules, ret);

	kfree(qmi_rule_ex_msg);
	return true;
}

int add_dft_filtering_rule(int pdn_iface, enum ipa_ip_type iptype)
{
	struct ipa_ioc_add_flt_rule *pFilteringTable = NULL;
	struct ipa_ioc_query_intf temp_intf;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	struct ipa_ioc_query_intf_ext_props *ext_prop = NULL;
	struct ipa_ioc_get_rt_tbl rt_tbl = {0};
	struct ipa_ioc_get_rt_tbl_indx rt_tbl_idx = {0};
	int retval = 0;
	int len, i, num_rules;
	uint8_t mux_id[16] = {0};
	struct ipa_pdn_filter_rules *pdn_entry;
	struct ipa_flt_rule_add flt_rule_entry;
	int m_ipv4_default_filterting_rules_count = 0;
	int m_ipv6_default_filterting_rules_count = 0;

	IPA_BE_DBG("Add dft flt rule for iface %d, ip type %d\n",
		pdn_iface, iptype);

	/* Check if default filtering rule is already installed for this PDN interface */
	spin_lock_bh(&ipa_pdn_filter_lock);
	pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
	if (pdn_entry) {
		if (pdn_entry->dft_rule_installed) {
			pdn_entry->ref_count++;
			spin_unlock_bh(&ipa_pdn_filter_lock);
			IPA_BE_DBG("Default filtering rule for pdn %d, IP type %d already installed. ref_count: %d\n",
				pdn_iface, iptype, pdn_entry->ref_count);
			return 0; /* Rules already exist, just increment ref count and exit */
		}
	} else {
		/* Create new entry */
		pdn_entry = ipa_create_pdn_filter_entry(pdn_iface, iptype);
		if (!pdn_entry) {
			spin_unlock_bh(&ipa_pdn_filter_lock);
			IPA_BE_ERR("Failed to create PDN filter entry for pdn %d, IP type %d\n", pdn_iface, iptype);
			return -ENOMEM;
		}
	}
	spin_unlock_bh(&ipa_pdn_filter_lock);

	if (!ipa3_query_iface(pdn_iface, &temp_intf)) {
		IPA_BE_ERR("iface %d doesn't exist\n", pdn_iface);
		return -EINVAL;
	}

	rx_prop = kzalloc(sizeof(*rx_prop) + temp_intf.num_rx_props *
		sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (!rx_prop) {
		IPA_BE_ERR("Failed to allocate rx_prop\n");
		return -ENOMEM;
	}
	memcpy(rx_prop->name, temp_intf.name, sizeof(rx_prop->name));
	rx_prop->num_rx_props = temp_intf.num_rx_props;

	if (ipa3_query_intf_rx_props(rx_prop)) {
		IPA_BE_ERR("Failed to query rx_prop for iface %s\n",
			rx_prop->name);
		retval = -EIO;
		goto fail_rx;
	}

	if (rx_prop->num_rx_props == 0) {
		IPA_BE_ERR("No rx props for iface %s\n", rx_prop->name);
		retval = 0; /* Not an error, just nothing to do */
		goto fail_rx;
	}

	/* ext Props */
	/* Check if the filter interface exists */
	ext_prop = (struct ipa_ioc_query_intf_ext_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_ext_props) +
		temp_intf.num_ext_props * sizeof(struct ipa_ioc_ext_intf_prop), GFP_KERNEL);
	if (ext_prop == NULL) {
		IPA_BE_ERR("Unable to allocate ext_prop memory.\n");
		retval = -EINVAL;
		goto fail_rx;
	}
	memcpy(ext_prop->name, temp_intf.name, sizeof(temp_intf.name));
	ext_prop->num_ext_props = temp_intf.num_ext_props;
	IPA_BE_ERR("Query ext_prop %d name %s\n", ext_prop->num_ext_props, ext_prop->name);
	ipa3_query_intf_ext_props(ext_prop);

	mux_id[0] = ext_prop->ext[0].mux_id;

	rt_tbl.ip = iptype;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(rt_tbl.name, WAN_DL_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#else
	strlcpy(rt_tbl.name, WAN_DL_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#endif

	if (ipa3_get_rt_tbl(&rt_tbl)) {
		IPA_BE_ERR("Failed to get WAN DL routing table\n");
		retval = -EIO;
		goto fail_ext;
	}
	IPA_BE_DBG("rt_tbl hdl %d\n", rt_tbl.hdl);

	rt_tbl_idx.ip = iptype;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(rt_tbl_idx.name, WAN_DL_ROUTE_TABLE_NAME, sizeof(rt_tbl_idx.name));
#else
	strlcpy(rt_tbl_idx.name, WAN_DL_ROUTE_TABLE_NAME, sizeof(rt_tbl_idx.name));
#endif

	rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX - 1] = '\0';
	if (ipa3_query_rt_index(&rt_tbl_idx)) {
		IPA_BE_ERR("Failed to get DL routing table index\n");
		retval = -EIO;
		goto fail_rx;
	}
	IPA_BE_DBG("rt_tbl_idx %d\n", rt_tbl_idx.idx);

	if (iptype == IPA_IP_v4)
		num_rules = 2; /* Multicast, Broadcast */
	else
		num_rules = 4; /* Frag, Multicast, Link-Local, Reserved */

	len = sizeof(*pFilteringTable) +
		num_rules * sizeof(struct ipa_flt_rule_add);
	pFilteringTable = kzalloc(len, GFP_KERNEL);
	if (!pFilteringTable) {
		IPA_BE_ERR("Failed to allocate filtering table\n");
		retval = -ENOMEM;
		goto fail_rx;
	}

	pFilteringTable->commit = 1;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = num_rules;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;

	if (iptype == IPA_IP_v4) {
		/* IPv4 Default Filtering Rules */

		/* Rule 1: Multicast Filtering Rule (224.0.0.0/4) */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule_entry.rule.hashable = true;
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

		/* Set multicast address attributes */
		flt_rule_entry.rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0xF0000000;
		flt_rule_entry.rule.attrib.u.v4.dst_addr = 0xE0000000;

		/* Convert to network order - for IPv4, this does nothing but we call it for consistency */
		retval = change_to_network_order_kernel(IPA_IP_v4, &flt_rule_entry.rule.attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to convert to network order\n");
			goto fail_rx;
		}

		/* Generate filter equation */
		retval = ipahal_flt_generate_equation(iptype, &flt_rule_entry.rule.attrib, &flt_rule_entry.rule.eq_attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to generate multicast filter equation\n");
			goto fail_rx;
		}

		memcpy(&(pFilteringTable->rules[m_ipv4_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv4_default_filterting_rules_count++;

		/* Rule 2: Broadcast Filtering Rule (255.255.255.255) */
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
		flt_rule_entry.rule.attrib.u.v4.dst_addr = 0xFFFFFFFF;

		/* Convert to network order */
		retval = change_to_network_order_kernel(IPA_IP_v4, &flt_rule_entry.rule.attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to convert broadcast rule to network order\n");
			goto fail_rx;
		}

		/* Generate filter equation */
		retval = ipahal_flt_generate_equation(iptype, &flt_rule_entry.rule.attrib, &flt_rule_entry.rule.eq_attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to generate broadcast filter equation\n");
			goto fail_rx;
		}

		memcpy(&(pFilteringTable->rules[m_ipv4_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv4_default_filterting_rules_count++;

		IPA_BE_DBG("Constructed %d default filtering rules for IPv4\n", m_ipv4_default_filterting_rules_count);

	} else if (iptype == IPA_IP_v6) {
		/* IPv6 Default Filtering Rules */

		/* Rule 1: Fragment Filtering Rule */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule_entry.rule.hashable = false; /* Fragment rules are not hashable */
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

		/* Set fragment attributes */
		flt_rule_entry.rule.attrib.attrib_mask = IPA_FLT_FRAGMENT;

		/* Convert to network order */
		retval = change_to_network_order_kernel(IPA_IP_v6, &flt_rule_entry.rule.attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to convert fragment rule to network order\n");
			goto fail_rx;
		}

		/* Generate filter equation */
		retval = ipahal_flt_generate_equation(iptype, &flt_rule_entry.rule.attrib, &flt_rule_entry.rule.eq_attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to generate fragment filter equation\n");
			goto fail_rx;
		}

		memcpy(&(pFilteringTable->rules[m_ipv6_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv6_default_filterting_rules_count++;

		/* Rule 2: Multicast Filtering Rule (ff00::/8) */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule_entry.rule.hashable = true; /* Multicast rules are hashable */
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

		flt_rule_entry.rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0xFF000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = MULTICAST_IPV6_PREFIX;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0x00000000;

		/* Convert to network order */
		retval = change_to_network_order_kernel(IPA_IP_v6, &flt_rule_entry.rule.attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to convert multicast rule to network order\n");
			goto fail_rx;
		}

		/* Generate filter equation */
		retval = ipahal_flt_generate_equation(iptype, &flt_rule_entry.rule.attrib, &flt_rule_entry.rule.eq_attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to generate multicast filter equation\n");
			goto fail_rx;
		}

		memcpy(&(pFilteringTable->rules[m_ipv6_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv6_default_filterting_rules_count++;

		/* Rule 3: Link-Local Unicast Filtering Rule (fe80::/10) */
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = LINK_LOCAL_IPV6_PREFIX_MASK;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = LINK_LOCAL_IPV6_PREFIX;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0x00000000;

		/* Convert to network order */
		retval = change_to_network_order_kernel(IPA_IP_v6, &flt_rule_entry.rule.attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to convert link-local rule to network order\n");
			goto fail_rx;
		}

		/* Generate filter equation */
		retval = ipahal_flt_generate_equation(iptype, &flt_rule_entry.rule.attrib, &flt_rule_entry.rule.eq_attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to generate link-local filter equation\n");
			goto fail_rx;
		}

		memcpy(&(pFilteringTable->rules[m_ipv6_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv6_default_filterting_rules_count++;

		/* Rule 4: Reserved by IETF Filtering Rule (fec0::/10) */
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = LINK_LOCAL_IPV6_PREFIX_MASK;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = RESERVED_IPV6_PREFIX;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0x00000000;

		/* Convert to network order */
		retval = change_to_network_order_kernel(IPA_IP_v6, &flt_rule_entry.rule.attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to convert reserved rule to network order\n");
			goto fail_rx;
		}

		/* Generate filter equation */
		retval = ipahal_flt_generate_equation(iptype, &flt_rule_entry.rule.attrib, &flt_rule_entry.rule.eq_attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to generate reserved filter equation\n");
			goto fail_rx;
		}

		memcpy(&(pFilteringTable->rules[m_ipv6_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv6_default_filterting_rules_count++;

		IPA_BE_DBG("Constructed %d default filtering rules for IPv6\n", m_ipv6_default_filterting_rules_count);
	} else {
		IPA_BE_ERR("Invalid IP type %d\n", iptype);
		retval = -EINVAL;
		goto fail_rx;
	}

	/* Add all rules to global linked lists using dynamic counts */
	if (iptype == IPA_IP_v4) {
		spin_lock_bh(&pdn_flt_rule_v4_lock);
		for (i = 0; i < m_ipv4_default_filterting_rules_count; i++) {
			struct flt_rule *new_rule = kzalloc(sizeof(struct flt_rule), GFP_ATOMIC);
			if (!new_rule) {
				spin_unlock_bh(&pdn_flt_rule_v4_lock);
				IPA_BE_ERR("Failed to allocate memory for IPv4 rule\n");
				retval = -ENOMEM;
				goto fail_table;
			}
			memcpy(&new_rule->flt_rule, &pFilteringTable->rules[i], sizeof(struct ipa_flt_rule_add));
			new_rule->mux_id = mux_id[0];
			new_rule->rule_type = FLT_RULE_TYPE_DEFAULT;
			new_rule->pdn_iface = pdn_iface;
			list_add_tail(&new_rule->node, &Pdn_flt_rule_v4_list);
			wan_rule_count_v4++;
			IPA_BE_DBG("Added default v4 rule %d, rule count now %d\n", i, wan_rule_count_v4);
		}
		spin_unlock_bh(&pdn_flt_rule_v4_lock);
	} else {
		spin_lock_bh(&pdn_flt_rule_v6_lock);
		for (i = 0; i < m_ipv6_default_filterting_rules_count; i++) {
			struct flt_rule *new_rule = kzalloc(sizeof(struct flt_rule), GFP_ATOMIC);
			if (!new_rule) {
				spin_unlock_bh(&pdn_flt_rule_v6_lock);
				IPA_BE_ERR("Failed to allocate memory for IPv6 rule\n");
				retval = -ENOMEM;
				goto fail_table;
			}
			memcpy(&new_rule->flt_rule, &pFilteringTable->rules[i], sizeof(struct ipa_flt_rule_add));
			new_rule->mux_id = mux_id[0];
			new_rule->rule_type = FLT_RULE_TYPE_DEFAULT;
			new_rule->pdn_iface = pdn_iface;
			list_add_tail(&new_rule->node, &Pdn_flt_rule_v6_list);
			wan_rule_count_v6++;
			IPA_BE_DBG("Added default v6 rule %d, rule count now %d\n", i, wan_rule_count_v6);
		}
		spin_unlock_bh(&pdn_flt_rule_v6_lock);
	}

	/* Mark the default rule as installed and increment reference count */
	spin_lock_bh(&ipa_pdn_filter_lock);
	pdn_entry->dft_rule_installed = true;
	pdn_entry->ref_count++;
	spin_unlock_bh(&ipa_pdn_filter_lock);

	IPA_BE_DBG("Default filtering rule successfully installed for pdn %d, IP type %d. ref_count: %d\n",
		pdn_iface, iptype, pdn_entry->ref_count);

	kfree(pFilteringTable);
	kfree(ext_prop);
	kfree(rx_prop);
	return 0;

fail_table:
	kfree(pFilteringTable);
fail_ext:
	kfree(ext_prop);
fail_rx:
	kfree(rx_prop);
	return retval;
}


int add_catchup_all_filtering_rule_each_pdn(int pdn_iface, enum ipa_ip_type iptype)
{
	struct ipa_ioc_add_flt_rule *pFilteringTable = NULL;
	struct ipa_ioc_query_intf temp_intf;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	struct ipa_ioc_query_intf_ext_props *ext_prop = NULL;
	struct ipa_ioc_get_rt_tbl rt_tbl = {0};
	struct ipa_ioc_get_rt_tbl_indx rt_tbl_idx = {0};
	int retval = 0;
	int len;
	const char *rt_tbl_name;
	uint8_t mux_id[16] = {0};
	struct ipa_ioc_generate_flt_eq flt_eq;
	struct ipa_pdn_filter_rules *pdn_entry;

	IPA_BE_DBG("Add catchup flt rule for iface %d, ip type %d\n",
		   pdn_iface, iptype);

	/* Check if catchup filtering rule is already installed for this PDN interface */
	spin_lock_bh(&ipa_pdn_filter_lock);
	pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
	if (pdn_entry) {
		if (pdn_entry->catchup_rule_installed) {
			pdn_entry->ref_count++;
			spin_unlock_bh(&ipa_pdn_filter_lock);
			IPA_BE_DBG("Catchup filtering rule for pdn %d, IP type %d already installed. ref_count: %d\n",
				pdn_iface, iptype, pdn_entry->ref_count);
			return 0; /* Rules already exist, just increment ref count and exit */
		}
	} else {
		/* Create new entry */
		pdn_entry = ipa_create_pdn_filter_entry(pdn_iface, iptype);
		if (!pdn_entry) {
			spin_unlock_bh(&ipa_pdn_filter_lock);
			IPA_BE_ERR("Failed to create PDN filter entry for pdn %d, IP type %d\n", pdn_iface, iptype);
			return -ENOMEM;
		}
	}
	spin_unlock_bh(&ipa_pdn_filter_lock);

	if (!ipa3_query_iface(pdn_iface, &temp_intf)) {
		IPA_BE_ERR("iface %d doesn't exist\n", pdn_iface);
		return -EINVAL;
	}

	rx_prop = kzalloc(sizeof(*rx_prop) + temp_intf.num_rx_props *
		sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (!rx_prop) {
		IPA_BE_ERR("Failed to allocate rx_prop\n");
		return -ENOMEM;
	}
	memcpy(rx_prop->name, temp_intf.name, sizeof(rx_prop->name));
	rx_prop->num_rx_props = temp_intf.num_rx_props;

	if (ipa3_query_intf_rx_props(rx_prop)) {
		IPA_BE_ERR("Failed to query rx_prop for iface %s\n",
			   rx_prop->name);
		retval = -EIO;
		goto fail_rx;
	}

	if (rx_prop->num_rx_props == 0) {
		IPA_BE_ERR("No rx props for iface %s\n", rx_prop->name);
		retval = 0; /* Not an error, just nothing to do */
		goto fail_rx;
	}

	ext_prop = kzalloc(sizeof(*ext_prop) + temp_intf.num_ext_props *
		sizeof(struct ipa_ioc_ext_intf_prop), GFP_KERNEL);
	if (!ext_prop) {
		IPA_BE_ERR("Unable to allocate ext_prop memory.\n");
		retval = -ENOMEM;
		goto fail_rx;
	}
	memcpy(ext_prop->name, temp_intf.name, sizeof(temp_intf.name));
	ext_prop->num_ext_props = temp_intf.num_ext_props;
	ipa3_query_intf_ext_props(ext_prop);
	if (ext_prop->num_ext_props > 0)
		mux_id[0] = ext_prop->ext[0].mux_id;

	if (iptype == IPA_IP_v4) {
		rt_tbl_name = V4_LAN_ROUTE_TABLE_NAME;
	} else {
		rt_tbl_name = V6_WAN_ROUTE_TABLE_NAME;
	}

	rt_tbl.ip = iptype;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(rt_tbl.name, rt_tbl_name, sizeof(rt_tbl.name));
#else
	strlcpy(rt_tbl.name, rt_tbl_name, sizeof(rt_tbl.name));
#endif

	if (ipa3_get_rt_tbl(&rt_tbl)) {
		IPA_BE_ERR("Failed to get WAN DL routing table\n");
		retval = -EIO;
		goto fail_ext;
	}
	IPA_BE_DBG("rt_tbl hdl %d for route table %s\n", rt_tbl.hdl, rt_tbl_name);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(rt_tbl_idx.name, rt_tbl_name, sizeof(rt_tbl_idx.name));
#else
	strlcpy(rt_tbl_idx.name, rt_tbl_name, sizeof(rt_tbl_idx.name));
#endif

	rt_tbl_idx.ip = iptype;
	rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX - 1] = '\0';
	if (ipa3_query_rt_index(&rt_tbl_idx)) {
		IPA_BE_ERR("Failed to get DL routing table index\n");
		retval = -EIO;
		goto fail_ext;
	}
	IPA_BE_DBG("rt_tbl_idx %d\n", rt_tbl_idx.idx);

	len = sizeof(*pFilteringTable) + sizeof(struct ipa_flt_rule_add);
	pFilteringTable = kzalloc(len, GFP_KERNEL);
	if (!pFilteringTable) {
		IPA_BE_ERR("Failed to allocate filtering table\n");
		retval = -ENOMEM;
		goto fail_ext;
	}

	pFilteringTable->commit = 1;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = 1;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;

	struct ipa_flt_rule_add *rule = &pFilteringTable->rules[0];

	rule->at_rear = true;
	rule->rule.action = IPA_PASS_TO_DST_NAT;

	rule->rule.rt_tbl_hdl = rt_tbl.hdl;
	rule->rule.rt_tbl_idx = rt_tbl_idx.idx;
	rule->rule.hashable = true;
	rule->rule.retain_hdr = 1;
	rule->rule.to_uc = 0;
	rule->rule.eq_attrib_type = 1;

	memcpy(&rule->rule.attrib, &rx_prop->rx[0].attrib,
		   sizeof(rule->rule.attrib));
	rule->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;

	/* Set destination address to 0 for both IPv4 and IPv6 */
	if (iptype == IPA_IP_v4) {
		rule->rule.attrib.u.v4.dst_addr_mask = 0;
		rule->rule.attrib.u.v4.dst_addr = 0;
	} else { /* IPA_IP_v6 */
		memset(rule->rule.attrib.u.v6.dst_addr_mask, 0,
			   sizeof(rule->rule.attrib.u.v6.dst_addr_mask));
		memset(rule->rule.attrib.u.v6.dst_addr, 0,
			   sizeof(rule->rule.attrib.u.v6.dst_addr));
	}

	/* Generate filter equation for both IPv4 and IPv6 */
	memset(&flt_eq, 0, sizeof(flt_eq));
	memcpy(&flt_eq.attrib, &rule->rule.attrib, sizeof(flt_eq.attrib));
	flt_eq.ip = iptype;

	retval = ipahal_flt_generate_equation(iptype, &flt_eq.attrib, &flt_eq.eq_attrib);
	if (retval) {
		IPA_BE_ERR("Failed generating equation for ip type %d with ret %d\n",
			   iptype, retval);
		retval = -EIO;
	} else {
		memcpy(&rule->rule.eq_attrib, &flt_eq.eq_attrib, sizeof(rule->rule.eq_attrib));

		if (iptype == IPA_IP_v4) {
			struct flt_rule *new_rule = kzalloc(sizeof(struct flt_rule), GFP_KERNEL);
			if (!new_rule) {
				IPA_BE_ERR("Failed to allocate memory for IPv4 catchup rule\n");
				retval = -ENOMEM;
				goto fail_table;
			}
			spin_lock_bh(&pdn_flt_rule_v4_lock);
			memcpy(&new_rule->flt_rule, rule, sizeof(struct ipa_flt_rule_add));
			new_rule->mux_id = mux_id[0];
			new_rule->rule_type = FLT_RULE_TYPE_CATCHUP;
			new_rule->pdn_iface = pdn_iface;
			list_add_tail(&new_rule->node, &Pdn_flt_rule_v4_list);
			wan_rule_count_v4++;
			spin_unlock_bh(&pdn_flt_rule_v4_lock);
			IPA_BE_DBG("Added catchup v4 rule, rule count now %d\n", wan_rule_count_v4);
		}
		else{
			struct flt_rule *new_rule = kzalloc(sizeof(struct flt_rule), GFP_KERNEL);
			if (!new_rule) {
				IPA_BE_ERR("Failed to allocate memory for IPv6 catchup rule\n");
				retval = -ENOMEM;
				goto fail_table;
			}
			spin_lock_bh(&pdn_flt_rule_v6_lock);
			memcpy(&new_rule->flt_rule, rule, sizeof(struct ipa_flt_rule_add));
			new_rule->mux_id = mux_id[0];
			new_rule->rule_type = FLT_RULE_TYPE_CATCHUP;
			new_rule->pdn_iface = pdn_iface;
			list_add_tail(&new_rule->node, &Pdn_flt_rule_v6_list);
			wan_rule_count_v6++;
			spin_unlock_bh(&pdn_flt_rule_v6_lock);
			IPA_BE_DBG("Added catchup v6 rule, rule count now %d\n", wan_rule_count_v6);
		}

		/* Mark the catchup rule as installed and increment reference count */
		spin_lock_bh(&ipa_pdn_filter_lock);
		pdn_entry->catchup_rule_installed = true;
		pdn_entry->ref_count++;
		spin_unlock_bh(&ipa_pdn_filter_lock);

		IPA_BE_DBG("Catchup filtering rule successfully installed for pdn %d, IP type %d. ref_count: %d\n",
			pdn_iface, iptype, pdn_entry->ref_count);
	}

	IPA_BE_DBG("Filter rule attrib mask: 0x%x\n", rule->rule.attrib.attrib_mask);

fail_table:
	kfree(pFilteringTable);
fail_ext:
	kfree(ext_prop);
fail_rx:
	kfree(rx_prop);
	return retval;
}


int install_wan_filtering_rule(void)
{
    int ret = 0, len = 0, cnt = 0;
    struct ipa_ioc_add_flt_rule *pFilteringTable_v4 = NULL;
    struct ipa_ioc_add_flt_rule *pFilteringTable_v6 = NULL;
	uint8_t *mux_id_v4 = NULL;
	uint8_t *mux_id_v6 = NULL;
	struct flt_rule *rule_entry;

    IPA_BE_DBG("IPACM_WAN_KERNEL: Installing WAN filtering rules\n");

    /* Install IPv4 filtering rules */
    if (wan_rule_count_v4 > 0) {
        len = sizeof(struct ipa_ioc_add_flt_rule) +
              (wan_rule_count_v4 * sizeof(struct ipa_flt_rule_add));

        pFilteringTable_v4 = kzalloc(len, GFP_KERNEL);
        if (!pFilteringTable_v4) {
            IPA_BE_ERR("Failed to allocate IPv4 filtering table\n");
            return -ENOMEM;
        }

        mux_id_v4 = kzalloc(wan_rule_count_v4 * sizeof(uint8_t), GFP_KERNEL);
        if (!mux_id_v4) {
            IPA_BE_ERR("Failed to allocate mux_id_v4 array\n");
            kfree(pFilteringTable_v4);
            return -ENOMEM;
        }

        pFilteringTable_v4->commit = 1;
        pFilteringTable_v4->ep = IPA_CLIENT_APPS_WAN_PROD;
        pFilteringTable_v4->global = false;
        pFilteringTable_v4->ip = IPA_IP_v4;
        pFilteringTable_v4->num_rules = wan_rule_count_v4;

        /* Copy rules from linked list */
        cnt = 0;
        spin_lock_bh(&pdn_flt_rule_v4_lock);
        list_for_each_entry(rule_entry, &Pdn_flt_rule_v4_list, node) {
            if (cnt >= wan_rule_count_v4) {
                IPA_BE_ERR("IPv4 rule count mismatch: cnt=%d, wan_rule_count_v4=%d\n",
                    cnt, wan_rule_count_v4);
                break;
            }
            memcpy(&pFilteringTable_v4->rules[cnt],
                   &rule_entry->flt_rule,
                   sizeof(struct ipa_flt_rule_add));
            mux_id_v4[cnt] = rule_entry->mux_id;
            cnt++;
        }
        spin_unlock_bh(&pdn_flt_rule_v4_lock);

        if (!AddWanDLFilteringRule(pFilteringTable_v4, NULL, mux_id_v4, NULL)) {
            IPA_BE_ERR("Failed to add v4 filtering rules\n");
            ret = -EIO;
        } else {
            IPA_BE_DBG("Successfully installed %d IPv4 filtering rules\n", wan_rule_count_v4);
        }
    }

    /* Install IPv6 filtering rules */
    if (wan_rule_count_v6 > 0) {
        len = sizeof(struct ipa_ioc_add_flt_rule) +
              (wan_rule_count_v6 * sizeof(struct ipa_flt_rule_add));

        pFilteringTable_v6 = kzalloc(len, GFP_KERNEL);
        if (!pFilteringTable_v6) {
            IPA_BE_ERR("Failed to allocate IPv6 filtering table\n");
            ret = -ENOMEM;
            goto cleanup;
        }

        mux_id_v6 = kzalloc(wan_rule_count_v6 * sizeof(uint8_t), GFP_KERNEL);
        if (!mux_id_v6) {
            IPA_BE_ERR("Failed to allocate mux_id_v6 array\n");
            kfree(pFilteringTable_v6);
            ret = -ENOMEM;
            goto cleanup;
        }

        pFilteringTable_v6->commit = 1;
        pFilteringTable_v6->ep = IPA_CLIENT_APPS_WAN_PROD;
        pFilteringTable_v6->global = false;
        pFilteringTable_v6->ip = IPA_IP_v6;
        pFilteringTable_v6->num_rules = wan_rule_count_v6;

        /* Copy rules from linked list */
        cnt = 0;
        spin_lock_bh(&pdn_flt_rule_v6_lock);
        list_for_each_entry(rule_entry, &Pdn_flt_rule_v6_list, node) {
            if (cnt >= wan_rule_count_v6) {
                IPA_BE_ERR("IPv6 rule count mismatch: cnt=%d, wan_rule_count_v6=%d\n",
                    cnt, wan_rule_count_v6);
                break;
            }
            memcpy(&pFilteringTable_v6->rules[cnt],
                   &rule_entry->flt_rule,
                   sizeof(struct ipa_flt_rule_add));
            mux_id_v6[cnt] = rule_entry->mux_id;
            cnt++;
        }
        spin_unlock_bh(&pdn_flt_rule_v6_lock);

        if (!AddWanDLFilteringRule(NULL, pFilteringTable_v6, NULL, mux_id_v6)) {
            IPA_BE_ERR("Failed to add v6 filtering rules\n");
            ret = -EIO;
        } else {
            IPA_BE_DBG("Successfully installed %d IPv6 filtering rules\n", wan_rule_count_v6);
        }
    }

cleanup:
    if (mux_id_v4)
        kfree(mux_id_v4);
    if (mux_id_v6)
        kfree(mux_id_v6);
    if (pFilteringTable_v4)
        kfree(pFilteringTable_v4);
    if (pFilteringTable_v6)
        kfree(pFilteringTable_v6);

    return ret;
}

int delete_dft_filtering_rule(int pdn_iface, enum ipa_ip_type iptype)
{
	struct ipa_pdn_filter_rules *pdn_entry;
	struct flt_rule *rule_entry, *tmp;
	int retval = 0;
	int deleted_count = 0;

	IPA_BE_DBG("Delete dft flt rule for iface %d, ip type %d\n",
		   pdn_iface, iptype);

	/* Find and update the PDN filter entry */
	spin_lock_bh(&ipa_pdn_filter_lock);
	pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
	if (!pdn_entry) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_ERR("No PDN filter entry found for pdn %d, IP type %d\n", pdn_iface, iptype);
		return -ENOENT;
	}

	if (!pdn_entry->dft_rule_installed) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_ERR("Default filtering rule for pdn %d, IP type %d not installed\n", pdn_iface, iptype);
		return -ENOENT;
	}

	pdn_entry->ref_count--;
	IPA_BE_DBG("Decremented ref_count for pdn %d, IP type %d. New count: %d\n",
		pdn_iface, iptype, pdn_entry->ref_count);

	if (pdn_entry->ref_count > 0) {
		/* Other connections are still using these rules, so don't delete them */
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_DBG("Default filtering rule for pdn %d, IP type %d still in use. ref_count: %d\n",
			pdn_iface, iptype, pdn_entry->ref_count);
		return 0;
	}

	/* Mark as not installed */
	pdn_entry->dft_rule_installed = false;

	/* If both default and catchup rules are not installed and ref_count is 0, remove the entry */
	if (!pdn_entry->catchup_rule_installed && pdn_entry->ref_count == 0) {
		ipa_remove_pdn_filter_entry(pdn_entry);
		IPA_BE_DBG("Removed PDN filter entry for pdn %d, IP type %d\n", pdn_iface, iptype);
	}

	spin_unlock_bh(&ipa_pdn_filter_lock);

	/* Remove DEFAULT rules for this PDN from the linked list */
	if (iptype == IPA_IP_v4) {
		spin_lock_bh(&pdn_flt_rule_v4_lock);
		list_for_each_entry_safe(rule_entry, tmp, &Pdn_flt_rule_v4_list, node) {
			if (rule_entry->pdn_iface == pdn_iface && rule_entry->rule_type == FLT_RULE_TYPE_DEFAULT) {
				list_del(&rule_entry->node);
				kfree(rule_entry);
				wan_rule_count_v4--;
				deleted_count++;
			}
		}
		spin_unlock_bh(&pdn_flt_rule_v4_lock);
		IPA_BE_DBG("Deleted %d default IPv4 rules for pdn %d, new count: %d\n",
			deleted_count, pdn_iface, wan_rule_count_v4);
	} else {
		spin_lock_bh(&pdn_flt_rule_v6_lock);
		list_for_each_entry_safe(rule_entry, tmp, &Pdn_flt_rule_v6_list, node) {
			if (rule_entry->pdn_iface == pdn_iface && rule_entry->rule_type == FLT_RULE_TYPE_DEFAULT) {
				list_del(&rule_entry->node);
				kfree(rule_entry);
				wan_rule_count_v6--;
				deleted_count++;
			}
		}
		spin_unlock_bh(&pdn_flt_rule_v6_lock);
		IPA_BE_DBG("Deleted %d default IPv6 rules for pdn %d, new count: %d\n",
			deleted_count, pdn_iface, wan_rule_count_v6);
	}

	IPA_BE_DBG("Default filtering rule deleted for pdn %d, IP type %d\n", pdn_iface, iptype);
	return retval;
}
EXPORT_SYMBOL(delete_dft_filtering_rule);

int delete_catchup_all_filtering_rule_each_pdn(int pdn_iface, enum ipa_ip_type iptype)
{
	struct ipa_pdn_filter_rules *pdn_entry;
	struct flt_rule *rule_entry, *tmp;
	int retval = 0;
	int deleted_count = 0;

	IPA_BE_DBG("Delete catchup flt rule for iface %d, ip type %d\n",
		   pdn_iface, iptype);

	/* Find and update the PDN filter entry */
	spin_lock_bh(&ipa_pdn_filter_lock);
	pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
	if (!pdn_entry) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_ERR("No PDN filter entry found for pdn %d, IP type %d\n", pdn_iface, iptype);
		return -ENOENT;
	}

	if (!pdn_entry->catchup_rule_installed) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_ERR("Catchup filtering rule for pdn %d, IP type %d not installed\n", pdn_iface, iptype);
		return -ENOENT;
	}

	pdn_entry->ref_count--;
	IPA_BE_DBG("Decremented ref_count for pdn %d, IP type %d. New count: %d\n",
		pdn_iface, iptype, pdn_entry->ref_count);

	if (pdn_entry->ref_count > 0) {
		/* Other connections are still using these rules, so don't delete them */
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_DBG("Catchup filtering rule for pdn %d, IP type %d still in use. ref_count: %d\n",
			pdn_iface, iptype, pdn_entry->ref_count);
		return 0;
	}

	/* Mark as not installed */
	pdn_entry->catchup_rule_installed = false;

	/* If both default and catchup rules are not installed and ref_count is 0, remove the entry */
	if (!pdn_entry->dft_rule_installed && pdn_entry->ref_count == 0) {
		ipa_remove_pdn_filter_entry(pdn_entry);
		IPA_BE_DBG("Removed PDN filter entry for pdn %d, IP type %d\n", pdn_iface, iptype);
	}

	spin_unlock_bh(&ipa_pdn_filter_lock);

	/* Remove CATCHUP rules for this PDN from the linked list */
	if (iptype == IPA_IP_v4) {
		spin_lock_bh(&pdn_flt_rule_v4_lock);
		list_for_each_entry_safe(rule_entry, tmp, &Pdn_flt_rule_v4_list, node) {
			if (rule_entry->pdn_iface == pdn_iface && rule_entry->rule_type == FLT_RULE_TYPE_CATCHUP) {
				list_del(&rule_entry->node);
				kfree(rule_entry);
				wan_rule_count_v4--;
				deleted_count++;
			}
		}
		spin_unlock_bh(&pdn_flt_rule_v4_lock);
		IPA_BE_DBG("Deleted %d catchup IPv4 rules for pdn %d, new count: %d\n",
			deleted_count, pdn_iface, wan_rule_count_v4);
	} else {
		spin_lock_bh(&pdn_flt_rule_v6_lock);
		list_for_each_entry_safe(rule_entry, tmp, &Pdn_flt_rule_v6_list, node) {
			if (rule_entry->pdn_iface == pdn_iface && rule_entry->rule_type == FLT_RULE_TYPE_CATCHUP) {
				list_del(&rule_entry->node);
				kfree(rule_entry);
				wan_rule_count_v6--;
				deleted_count++;
			}
		}
		spin_unlock_bh(&pdn_flt_rule_v6_lock);
		IPA_BE_DBG("Deleted %d catchup IPv6 rules for pdn %d, new count: %d\n",
			deleted_count, pdn_iface, wan_rule_count_v6);
	}

	IPA_BE_DBG("Catchup filtering rule deleted for pdn %d, IP type %d\n", pdn_iface, iptype);
	return retval;
}
EXPORT_SYMBOL(delete_catchup_all_filtering_rule_each_pdn);
