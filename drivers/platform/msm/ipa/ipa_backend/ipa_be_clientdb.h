// SPDX-License-Identifier: GPL-2.0-only
/*
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
*/

#ifndef _IPA_BE_CLIENTDB_
#define _IPA_BE_CLIENTDB_


#include "ipa_api.h"
#include "ipa_be.h"


#define TABLE_SIZE 128

#define IPA_IP_ADDR_DOT_FMT "%u.%u.%u.%u"
#define IPA_IP_ADDR_TO_DOT(ipaddrt) ((uint8_t *)ipaddrt)[3], ((uint8_t *)ipaddrt)[2], ((uint8_t *)ipaddrt)[1], ((uint8_t *)ipaddrt)[0]


#define V4_DEFAULT_ROUTE_TABLE_NAME  "ipa_dflt_rt"
#define V4_LAN_ROUTE_TABLE_NAME  "COMRTBLLANv4"
#define V4_WAN_ROUTE_TABLE_NAME  "WANRTBLv4"
#define WAN_DL_ROUTE_TABLE_NAME "ipa_dflt_wan_rt"
#define V6_COMMON_ROUTE_TABLE_NAME  "COMRTBLv6"
#define V6_WAN_ROUTE_TABLE_NAME  "WANRTBLv6"
#define V4_ODU_ROUTE_TABLE_NAME  "ODURTBLv4"
#define V6_ODU_ROUTE_TABLE_NAME  "ODURTBLv6"
#define V6_DEFAULT_ROUTE_TABLE_NAME  "ipa_dflt_rt"
#define V4_LAN_TO_LAN_ROUTE_TABLE_NAME  "lan_to_lan_v4"
#define V6_LAN_TO_LAN_ROUTE_TABLE_NAME  "lan_to_lan_v6"


#define IPA_ETH_HDR_NAME_v4  "IPACM_ETH_v4"
#define IPA_ETH_HDR_NAME_v6  "IPACM_ETH_v6"

/*
 * Mapping owner events
 */
typedef void (*ipa_db_mapping_final_callback_t)(void *arg);		/* Finaliser callback */


typedef struct ProcCtx {
	char name[32];
	int handle;
	int ref_count;
	struct ProcCtx *next;
} ProcCtx;

typedef struct Hdr {
	mac_addr_t mac;
	uint32_t vlan_tag;
	enum ipa_ip_type ip_type;
	int handle;
	int ref_count;
	struct Hdr *next;
} Hdr;

extern ProcCtx *hash_table_ipa[TABLE_SIZE];
extern Hdr *hash_table_hdr[TABLE_SIZE];


extern struct mutex ipa_client_db_lock;
extern struct ipa_clientdb_mapping_instance *ipa_db_mappings;
typedef uint32_t ipa_db_mapping_hash_t;

#define IPA_CLIENT_DB_MAPPING_INSTANCE_MAGIC 0x9876


struct ipa_clientdb_lan2lan_info {
	uint32_t rt_hdl;
	uint32_t proc_ctx_hdl;
	uint32_t hdr_hdl;
	uint32_t ref_count;			/* Count the number of active connections on this client*/
	char proc_ctx_name[32];		/* Name used for proc_ctx hash table lookup */
};

struct ipa_clientdb_lan2wan_info {
	uint32_t rt_hdl;
	uint32_t proc_ctx_hdl;
	uint32_t hdr_hdl;
	uint32_t ref_count;			/* Count the number of active connections on this client*/
	char proc_ctx_name[32];		/* Name used for proc_ctx hash table lookup */
};


/*
 * struct ipa_clientdb_mapping_instance
 */
struct ipa_clientdb_mapping_instance {
	struct ipa_clientdb_mapping_instance *next;				/* Next instance in global list */
	struct ipa_clientdb_mapping_instance *prev;				/* Previous instance in global list */

	struct ipa_clientdb_mapping_instance *hash_next;			/* Next mapping in the chain of mappings (IP-based) */
	struct ipa_clientdb_mapping_instance *hash_prev;			/* previous mapping in the chain of mappings (IP-based) */

	struct ipa_clientdb_mapping_instance *mac_hash_next;		/* Next mapping in the MAC-based hash chain */
	struct ipa_clientdb_mapping_instance *mac_hash_prev;		/* Previous mapping in the MAC-based hash chain */

	uint8_t mac_addr_t[IPA_MAC_ADDR_SIZE];
	uint32_t vlan_id;
	ip_addr_t address;

	struct ipa_clientdb_lan2lan_info lan2lan_info;
	struct ipa_clientdb_lan2wan_info lan2wan_info;

	char iface_name[IPA_RESOURCE_NAME_MAX];
	uint32_t iface_num;

	ipa_db_mapping_hash_t hash_index;
};


struct ipa_clientdb_mapping_instance *ipa_db_mapping_alloc(void);
bool ipa_be_clientdb_find_and_ref(ip_addr_t address, int vlan_id, bool lan2lan);
struct ipa_clientdb_mapping_instance *ipa_be_clientdb_get_mapping_by_mac(mac_addr_t mac, int vlan_id);
int ipa_client_db_mapping_add(struct ipa_clientdb_mapping_instance *mi, int vlan_id, ip_addr_t address,
							   ipa_db_mapping_final_callback_t final, void *arg);

struct ipa_clientdb_mapping_instance *ipa_be_client_mapping_add_or_ref(
	ip_addr_t addr, int vlan_id, int lan2lan, mac_addr_t mac);

int ipa_be_mapping_deref_and_delete(ip_addr_t addr, bool lan2lan);

bool ipa_clientdb_mapping_init(void);

int ipa_be_lookup_handle(char *name);
int ipa_be_lookup_ref_count(char *name);
void ipa_be_insert_proc_ctx(char *name, int handle);

int ipa_ipv4_add_route_rule(struct ipa_ipv4_rule_create_msg v4_msg, bool lan2lan, int intf_num, mac_addr_t mac, int is_ret);
int ipa_delete_route_rule(bool lan2lan, int rt_rule_hdl, enum ipa_ip_type ip);

int ipa_ipv6_add_route_rule(struct ipa_ipv6_rule_create_msg v6_msg, bool lan2lan, int intf_num, mac_addr_t mac, int is_ret);
int ipa_ipv6_delete_route_rule(struct ipa_ipv6_rule_destroy_msg v6_msg, bool lan2lan, int rt_rule_hdl, enum ipa_ip_type ip);

int ipa_be_update_lan_info_from_rule(struct ipa_ipv4_rule_create_msg *rule_msg, int hdl, int proc,
							  int lan2lan, int hdr_hdl, int is_ret, const char *proc_ctx_name);
int ipa_get_rt_hdl_from_mapping(ip_addr_t addr, bool lan2lan, int *hdr_hdl, int *proc_ctx_hdl, char *proc_ctx_name_out);

int ipa_be_update_lan_v6_info_from_rule(struct ipa_ipv6_rule_create_msg *rule_msg, int hdl, int proc,
							  int lan2lan, int hdr_hdl, int is_ret, const char *proc_ctx_name);

int ipa_be_delete_proc_ctx(char *name);
int ipa_be_delete_hdr_by_handle(int hdr_hdl);
int ipa_be_delete_hdr_handle(int hdr_hdl);

#endif /* _IPA_BE_CLIENTDB_ */
