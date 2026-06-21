// SPDX-License-Identifier: GPL-2.0-only
/*
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
*/

#ifndef _IPA_BE_FLT_MGMT_
#define _IPA_BE_FLT_MGMT_


#include "ipa_api.h"
#include "ipa_be.h"

#define IPV6_HEADER_SIZE 40
#define UINT16_MAX (65535U)

#define LAN2LAN_RULE_ID 1

int ipa_be_v4_add_filter_rule(struct ipa_ipv4_rule_create_msg v4_msg, bool lan2lan, int intf_num, mac_addr_t mac, int is_ret);
int ipa_be_v4_delete_filter_rule(struct ipa_ipv4_rule_destroy_msg v4_msg, int intf_idx, mac_addr_t mac, bool lan2lan);

int ipa_be_v4_add_uplink_filter_rule(struct ipa_ipv4_rule_create_msg v4_msg, bool lan2lan, int pdn_iface, int client_iface);
int ipa_be_v4_delete_uplink_filter_rule(struct ipa_ipv4_rule_destroy_msg v4_msg, int pdn_iface, int client_iface);


int ipa_be_v6_add_filter_rule(struct ipa_ipv6_rule_create_msg v6_msg, bool lan2lan, int intf_num, mac_addr_t mac, int is_ret);
int ipa_be_v6_delete_filter_rule(struct ipa_ipv6_rule_destroy_msg v6_msg, int intf_idx, mac_addr_t mac, bool lan2lan);

int ipa_be_v6_add_uplink_filter_rule(struct ipa_ipv6_rule_create_msg v6_msg, bool lan2lan, int pdn_iface, int client_iface, bool is_xlat);
int ipa_be_v6_delete_uplink_filter_rule(struct ipa_ipv6_rule_destroy_msg v6_msg, int pdn_iface, int client_iface);

int ipa_be_construct_mtu_rule(enum ipa_ip_type iptype, uint16_t mtu, int intf_num, int pdn_iface, int vlan_tag);
int ipa_be_delete_mtu_rule(int client_iface, int pdn_iface, enum ipa_ip_type ip_type);

int ipa_be_subnet_notifier_init(struct workqueue_struct *v4_wq,
	struct workqueue_struct *v6_wq);
void ipa_be_subnet_notifier_deinit(void);
void ipa_be_subnet_notifier_cleanup(void);
void ipa_be_subnet_on_intf_registered(int slave_ifindex);

int ipa_be_delete_rules_by_category(int intf_num, int category, enum ipa_ip_type iptype);

int add_dft_filtering_rule(int pdn_iface, enum ipa_ip_type iptype);
int add_icmp_alg_rules(int pdn_iface, enum ipa_ip_type iptype);
int add_catchup_all_filtering_rule_each_pdn(int pdn_iface, enum ipa_ip_type iptype);
int delete_dft_filtering_rule(int pdn_iface, enum ipa_ip_type iptype);
int delete_icmp_alg_rules(int pdn_iface, enum ipa_ip_type iptype);
int delete_catchup_all_filtering_rule_each_pdn(int pdn_iface, enum ipa_ip_type iptype);

int install_wan_filtering_rule(void);
#endif /* _IPA_BE_FLT_MGMT_ */
