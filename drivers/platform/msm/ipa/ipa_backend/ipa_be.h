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
#define IPA_BE_DBG(fmt, args...) \
	do { \
		pr_debug(OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
	} while (0)

#define  IPA_BE_LOW(fmt, args...) \
	do { \
		pr_debug(OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
	} while (0)

#define  IPA_BE_ERR(fmt, args...) \
	do { \
		pr_err(OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
	} while (0)



#define MAX_NUM_EXT_PROPS 25
#define IPA_MAX_NUM_PROPS 5

#define MAX_WAN_UL_FILTER_RULES MAX_NUM_EXT_PROPS
#define IPA_MAX_ACCEL_CONNECTIONS_V4 32000
#define IPA_MAX_ACCEL_CONNECTIONS_V6 32000

typedef uint8_t mac_addr_t[IPA_MAC_ADDR_SIZE];

static inline void ipa_type_check_ipa_mac_addr(mac_addr_t mac_addr){}
static inline void ipa_type_check_ipa_ip_addr(ip_addr_t ipaddr){}

int ipa_be_addpdn(struct ipa_ipv4_rule_create_msg v4_msg, int pdn_iface);
int ipa_be_add_v6_ct_entry(struct ipa_ipv6_rule_create_msg v6_msg, int pdn_iface);
void ipa_be_delete_entry(struct ipa_ipv4_rule_destroy_msg v4_msg);
int ipa_be_nat_mgmt_init(void);
void ipa_be_nat_mgmt_exit(void);

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

#endif /* _IPA_BE_ */
