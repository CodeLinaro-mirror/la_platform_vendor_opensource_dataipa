// SPDX-License-Identifier: GPL-2.0-only
/*
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
*/

#ifndef _IPA_BE_NAT_MGMT_
#define _IPA_BE_NAT_MGMT_

#define __divsf3

#include "ipa_api.h"
#include "ipa_be.h"

#ifdef IPA_BE_DBG
#define iptodot(X,Y) \
		 IPA_BE_DBG(" %s(0x%x): %d.%d.%d.%d\n", X, Y, ((Y>>24) & 0xFF), ((Y>>16) & 0xFF), ((Y>>8) & 0xFF), (Y & 0xFF));
#endif

#define log_nat(A,B,C,D,E,F,G) \
		IPA_BE_DBG("protocol %d Private IP: %d.%d.%d.%d\t Target IP: %d.%d.%d.%d\t private port: %d public port: %d target port: %d %s",A,((B>>24) & 0xFF), ((B>>16) & 0xFF), ((B>>8) & 0xFF), (B & 0xFF), ((C>>24) & 0xFF), ((C>>16) & 0xFF),((C>>8) & 0xFF),(C & 0xFF),D,E,F,G);

#define CHK_TBL_HDL()  if(nat_app->nat_table_hdl == 0){ return -1; }

#define IPA_BE_SUCCESS 0  /* Successful operation */
#define IPA_BE_FAILURE -1  /* Unsuccessful operation */

/* Forward declaration */
struct ipa_ctx_instance_internal;

typedef struct _nat_table_entry
{
	uint32_t private_ip;
	uint16_t private_port;

	uint32_t target_ip;
	uint16_t target_port;

	uint32_t public_ip;
	uint16_t public_port;
	uint8_t pdn_index;

	u_int8_t  protocol;
	uint32_t timestamp;

	bool dst_nat;
	bool enabled;
	uint32_t rule_hdl;

	uint16_t uc_activation_index;
	bool s;
	bool ucp;
	bool dst_only;
	bool src_only;
	bool dummy_nat;
	bool ip_pass_entry;

	/* IPv4 CT support fields (IPA v7.0+) */
	bool conn_tracking;        /* 0=NAT, 1=CT */
	bool out_allowed;          /* Outbound direction allowed (for CT) */
	bool in_allowed;           /* Inbound direction allowed (for CT) */
	uint16_t all_pkts_stats_cnt_index; /* All packets statistics counter index */
	uint16_t non_frag_stats_cnt_index; /* Non fragmented statistics counter index */

}nat_table_entry;

bool chk_for_dup(const nat_table_entry *rule);
void ipa_be_reset(void);
bool is_pwr_save_if(uint32_t ip_addr);
uint32_t generate_met_data(uint8_t mux_id);

int ipa_be_add_table(uint32_t pub_ip, uint8_t mux_id, bool is_sta, bool ip_pass);
int ipa_be_add_pdn(uint32_t pub_ip, uint8_t mux_id, bool is_sta, bool ip_pass);
int ipa_be_remove_pdn(uint32_t pub_ip);

//bool ChkForDupGRE(const nat_table_entry *);
// bool isAlgPort(uint8_t, uint16_t);

// int AddTable(uint32_t, uint8_t mux_id, bool is_sta);
// int DeleteTable(uint32_t);
// int MoveTable(bool to_ddr);

int ipa_be_add_entry(struct ipa_ipv4_rule_create_msg v4_msg, bool isVlan,
		     bool ct_enabled, bool ct_canonical_swap);
int ipa_be_delete_nat_entry(const nat_table_entry *rule);

/* IPv6 Connection Tracking functions */
int ipv6ct_add_entry(struct ipa_ipv6_rule_create_msg *v6_msg, uint32_t *rule_handle);
int ipv6ct_del_entry(struct ipa_ipv6_rule_destroy_msg *v6_msg, uint32_t rule_handle);
void ipa_be_delete_v6_ct_entry(struct ipa_ipv6_rule_destroy_msg v6_msg, uint32_t rule_handle);
int ipa_be_add_table_v6(const uint32_t v6_prefix[2]);
void ipa_be_handle_v6_ct_deletion(struct ipa_ipv6_rule_destroy_msg *msg, bool lan2lan);
// int DeleteEntryGRE(const nat_table_entry *);
// void UpdateUDPTimeStamp(void);

// int UpdatePwrSaveIf(uint32_t);
// int ResetPwrSaveIf(uint32_t);
// int DelEntriesOnClntDiscon(uint32_t);
// int DelDummyNatEntries(uint32_t ip_addr);
// int DelEntriesOnSTAClntDiscon(uint32_t);

// void Read_TcpUdp_Timeout(void);

// void AddTempEntry(const nat_table_entry *);
// void CacheEntry(const nat_table_entry *);
// void DeleteTempEntry(const nat_table_entry *);
// #ifdef FEATURE_VLAN_MPDN
// 	void FlushAndCacheVlanTempEntries(uint32_t ip_addr, bool *entry_exists, uint32_t *public_ip);
// #endif
// void FlushTempEntries(uint32_t, bool, bool isDummy);
// void DeleteTempEntry_port(uint16_t);
// int DeleteEntry_port(uint16_t);

int store_ipv6_ct_entry(uint64_t src_ipv6_msb, uint64_t src_ipv6_lsb,
				uint64_t dest_ipv6_msb, uint64_t dest_ipv6_lsb,
				uint16_t src_port, uint16_t dest_port,
				uint8_t protocol, uint32_t rule_handle);

int find_ipv6_ct_entry(uint64_t src_ipv6_msb, uint64_t src_ipv6_lsb,
			       uint64_t dest_ipv6_msb, uint64_t dest_ipv6_lsb,
			       uint16_t src_port, uint16_t dest_port, uint8_t protocol);

ipa_tx_status_t ipa_sync_ipv4_stats_many_msg(struct ipa_ctx_instance_internal *ipa_ctx,
	struct ipa_ipv4_msg *msg);
ipa_tx_status_t ipa_sync_ipv6_stats_many_msg(struct ipa_ctx_instance_internal *ipa_ctx,
	struct ipa_ipv6_msg *msg);
#endif /* _IPA_BE_NAT_MGMT_ */
