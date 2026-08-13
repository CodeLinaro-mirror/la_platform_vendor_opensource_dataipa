// SPDX-License-Identifier: GPL-2.0-only
/*
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
*/

#ifndef _IPA_BE_
#define _IPA_BE_


#include <linux/msm_ipa.h>
#include <linux/jhash.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/cpumask.h>
#include "ipa_api.h"
#include "../ipa_common_i.h"


#define OFFLOAD_DRV_NAME "ipa_backend"

#define IPA_BE_MULTICAST_IPV6_PREFIX 0xFF000000
#define IPA_BE_LINK_LOCAL_IPV6_PREFIX 0xFE800000
#define IPA_BE_LINK_LOCAL_IPV6_PREFIX_MASK 0xFFC00000
#define IPA_BE_RESERVED_IPV6_PREFIX 0xFEC00000

#define IPA_BE_DBG(fmt, args...) \
	do { \
		pr_debug(OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
		ipa3_diag_log_write(IPA_DIAG_LVL_DBG, OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
	} while (0)

#define  IPA_BE_LOW(fmt, args...) \
	do { \
		pr_debug(OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
		if (ipa3_get_ipc_logbuf_low()) { \
			IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
				OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
			ipa3_diag_log_write(IPA_DIAG_LVL_LOW, \
				OFFLOAD_DRV_NAME " %s:%d " fmt, \
				__func__, __LINE__, ## args); \
		} \
	} while (0)

#define  IPA_BE_ERR(fmt, args...) \
	do { \
		pr_err(OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
		ipa3_diag_log_write(IPA_DIAG_LVL_ERR, OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
	} while (0)



#define MAX_NUM_EXT_PROPS 25
#define IPA_MAX_NUM_PROPS 5

#define MAX_WAN_UL_FILTER_RULES MAX_NUM_EXT_PROPS
#define IPA_MAX_ACCEL_CONNECTIONS_V4 4096
#define IPA_MAX_ACCEL_CONNECTIONS_V6 4096

/* Backhaul type enumeration */
enum ipa_backhaul_type {
	IPA_BACKHAUL_TYPE_MODEM = 0,      /* Modem backhaul (has ext_props/QMI) */
	IPA_BACKHAUL_TYPE_ETH = 1,        /* Ethernet backhaul (no ext_props) */
	IPA_BACKHAUL_TYPE_UNKNOWN = 2
};

typedef uint8_t mac_addr_t[IPA_MAC_ADDR_SIZE];

static inline void ipa_type_check_ipa_mac_addr(mac_addr_t mac_addr){}
static inline void ipa_type_check_ipa_ip_addr(ip_addr_t ipaddr){}

/* Returns true if the IPv6 destination address is link-local (fe80::/10).
 * ttl_update is left unset for link-local route rules; keep the two
 * lan2wan route callsites in sync through this helper.
 */
static inline bool ipa_be_v6_dst_is_link_local(const uint32_t dst_addr[4])
{
	return (dst_addr[0] & IPA_BE_LINK_LOCAL_IPV6_PREFIX_MASK) ==
		IPA_BE_LINK_LOCAL_IPV6_PREFIX;
}

typedef enum ipa_exception {
	IPA_EXCEPTION_IPV4_MSG_UNKNOWN,
	IPA_EXCEPTION_IPV6_MSG_UNKNOWN,
	IPA_EXCEPTION_CONNECTION_INVALID,
	IPA_EXCEPTION_NOT_SUPPORT_BRIDGE,
	IPA_EXCEPTION_TCP_INVALID,
	IPA_EXCEPTION_PROTOCOL_NOT_SUPPORT,
	IPA_EXCEPTION_SRC_DEV_NOT_L3,
	IPA_EXCEPTION_DEST_DEV_NOT_L3,
	IPA_EXCEPTION_CFG_ERR,
	IPA_EXCEPTION_CREATE_FAILED,
	IPA_EXCEPTION_ENQUEUE_FAILED,
	IPA_EXCEPTION_NOT_SUPPORT_6RD,
	IPA_EXCEPTION_NO_SYNC_CB,
	IPA_EXCEPTION_MAX
} ipa_exception_t;

/*
 * IPA context instance, private for IPA
 */
struct ipa_ctx_instance_internal {
	struct ipa_ctx_instance base;	/* Exported IPA context, is public to user of IPA*/

	/*
	 * Control state.
	 */
	struct kobject *sys_ipa;	/* Sysfs linkage */

	struct list_head msg_queue;	/* Response message queue*/
	spinlock_t lock;		/* Lock to protect message queue */

	struct work_struct work;	/* Work to send response message back to caller*/

	ipa_ipv4_msg_callback_t __rcu ipv4_stats_sync_cb;	/* Callback to call to sync ipv4 statistics */
	ipa_ipv4_msg_callback_t __rcu ipv4_stats_sync_many_cb;	/* Callback to call to sync many ipv4 statistics */
	void *ipv4_stats_sync_data;	/* Argument for above callback: ipv4_stats_sync_cb */

	ipa_ipv6_msg_callback_t __rcu ipv6_stats_sync_cb;	/* Callback to call to sync ipv6 statistics */
	ipa_ipv6_msg_callback_t __rcu ipv6_stats_sync_many_cb;	/* Callback to call to sync many ipv6 statistics */
	void *ipv6_stats_sync_data;	/* Argument for above callback: ipv6_stats_sync_cb */

	u32 exceptions[IPA_EXCEPTION_MAX];		/* Statistics for exception */

	int32_t l2_feature_support;		/* L2 feature support */
	int32_t ppe_rfs_feature_support;	/* PPE RFS feature support */

	struct workqueue_struct *ipa_ipv4_wq;	/* Workqueue for IPv4 processing */
	struct workqueue_struct *ipa_ipv6_wq;	/* Workqueue for IPv6 processing */
};

int ipa_be_addpdn(struct ipa_ipv4_rule_create_msg v4_msg, int pdn_iface);
int ipa_be_add_ct_entry_v4(struct ipa_ipv4_rule_create_msg *v4_msg);
int ipa_be_add_v6_ct_entry(struct ipa_ipv6_rule_create_msg v6_msg, int pdn_iface, bool lan2lan);
void ipa_be_delete_entry(struct ipa_ipv4_rule_destroy_msg v4_msg, bool ct_enabled);
void ipa_be_delete_ct_entry_v4(struct ipa_ipv4_rule_destroy_msg v4_msg);
int ipa_be_nat_mgmt_init(struct ipa_ctx_instance_internal *ipa_ctx);
void ipa_be_nat_mgmt_exit(void);


/* Shared with ipa_be_signal_iface_offline. Caller must not hold
 * ipa_be_nat_mgmt_lock; it is acquired internally.
 */
void ipa_ipv4_destroy_rule(struct ipa_ipv4_rule_destroy_msg *msg);
void ipa_ipv6_destroy_rule(struct ipa_ipv6_rule_destroy_msg *msg);

/* Tears down every offloaded flow, as ECM_DISCONNECT does per iface. */
void ipa_be_flush_all_flows(void);

/* Returns the IPA hardware endpoint index for an interface, or -1 on failure. */
int ipa_be_get_ep_for_intf(s32 intf_num);

/*
 * Returns 1 when flow_intf is canonical (smaller EP/MAC), 0 when return_intf
 * is canonical, or -1 on EP query failure or NULL MAC (always logged).
 */
int ipa_be_flow_canonical_cmp(s32 flow_intf, s32 return_intf,
		const u8 *flow_mac, const u8 *return_mac);

int ipa_be_detect_backhaul_type(int wan_iface, enum ipa_backhaul_type *out_type);
void ipa_be_backhaul_cache_invalidate(int wan_iface);

#define IPA_MAC_ADDR_HASH(h, a) \
{ \
	ipa_type_check_ipa_mac_addr(a); \
	h = a[0] ^ a[1] ^ a[2] ^ a[3] ^ a[4] ^ a[5] ; \
}

#define IPA_IP_ADDR_HASH(h, a)                    \
{                                                 \
	ipa_type_check_ipa_ip_addr(a);                \
	const uint8_t *ip_byte = (const uint8_t *)(a);\
	h = 0;                                        \
	for (int _i = 0; _i < 16; ++_i)               \
		h ^= ip_byte[_i];                         \
}

#define IPA_IP_STRING_HASH(h, s) \
	{ \
		char *str__ = (s); \
		(h) = 0; \
		for (size_t i__ = 0; str__[i__]; ++i__) { \
			(h) ^= (uint32_t)str__[i__]; \
			(h) = ((h) << 5) | ((h) >> 27); \
		} \
	}

const char* ipa_be_message_type_to_string(enum ipa_message_types type);
const char* ipa_be_cmn_response_to_string(enum ipa_cmn_response type);


int ipa_be_init_if(void);
int ipa_be_exit_if(void);
int ipa_be_install_qos_route_rule(struct ipa_ioc_qos_config *qos_param);
int ipa_be_delete_qos_route_rule(struct ipa_ioc_qos_config *qos_param);
void ipa_be_flush_qos_route_rules(void);
void ipa_be_cleanup_tc_bitmap_rules(const char *dev_name,
				    enum ipa_ip_type ip,
				    uint32_t removed_tcs);
int ipa_be_install_tc_bitmap_rules(const char *dev_name,
				   enum ipa_ip_type ip,
				   uint32_t added_tcs);

#endif /* _IPA_BE_ */
