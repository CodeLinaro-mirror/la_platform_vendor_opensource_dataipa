// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

/*
 * Lock Hierarchy — MUST be followed throughout this file:
 *
 *   1. ipa_client_db_lock  (outermost — acquire first if both are needed)
 *   2. msg_qos_param_lock  (inner — acquire second)
 *
 * Never acquire msg_qos_param_lock while holding ipa_client_db_lock, and
 * never call a function that acquires ipa_client_db_lock while holding
 * msg_qos_param_lock.  The collect-then-operate pattern used in
 * ipa_be_check_and_install_qos_for_new_rule() (snapshot the QoS list under
 * msg_qos_param_lock, release the lock, then call ipa_be_install_qos_route_rule()
 * which may acquire ipa_client_db_lock) is the required idiom for operations
 * that need both.
 */

/* Fixed-size pool for pre-allocating install tasks before taking
 * msg_qos_param_lock, avoiding GFP_ATOMIC under mutex. */
#define QOS_TASK_POOL_SIZE 16

#include "../ipa_common_i.h"
#include "../ipa_v3/ipa_pm.h"
#include "../ipa_v3/ipa_i.h"
#include <linux/sort.h>
#include <linux/version.h>
#include "ipa_api.h"
#include "ipa_be.h"
#include "ipa_be_flt_mgmt.h"
#include "ipa_be_clientdb.h"
#ifdef CONFIG_ECM_CONVERGENCE
#include "ipa_be_nat_mgmt.h"
#endif
#include "ipa_be_fse.h"

/* ========================================================================== */
/* MACROS AND DEFINITIONS                                                     */
/* ========================================================================== */

#define IPA_MAX_TIMEOUT 20	/* seconds */

#define ipa_ipv4_stopped(CTX) (rcu_dereference((CTX)->ipv4_stats_sync_cb) == NULL)
#define ipa_ipv6_stopped(CTX) (rcu_dereference((CTX)->ipv6_stats_sync_cb) == NULL)

/*
 * Message type of queued response message
 */
typedef enum {
	IPA_MSG_TYPE_IPV4,
	IPA_MSG_TYPE_IPV6
} ipa_msg_types_t;

/*
 * Queued response message,
 * will be sent back to caller in workqueue
 */
struct ipa_response_msg {
	struct list_head node;
	ipa_msg_types_t type;
	u8 msg[];
};

static struct ipa_ctx_instance_internal __ipa_be_ctx = {
	.lock = __SPIN_LOCK_UNLOCKED(__ipa_be_ctx.lock),
};

/*
 * Convert public IPA context to internal context
 */
#define IPA_CTX_TO_PRIVATE(base) (struct ipa_ctx_instance_internal *)(base)
/*
 * Convert internal IPA context to public context
 */
#define IPA_CTX_TO_PUBLIC(intrv) (struct ipa_ctx_instance *)(intrv)


/* ========================================================================== */
/* ETH BACKHAUL SUPPORT FUNCTIONS                                                          */
/* ========================================================================== */

#define IPA_BE_BACKHAUL_CACHE_SIZE 16

/*
 * Cache mapping a WAN interface index to its detected backhaul type.
 * ETH entries are invalidated when the last flow on that interface tears down.
 * MODEM entries are not invalidated per-flow (no per-iface refcount, no
 * iface-down hook); stale classification on index reuse is accepted as rare.
 */
struct ipa_be_backhaul_cache_entry {
	int wan_iface;
	enum ipa_backhaul_type type;
};

static struct ipa_be_backhaul_cache_entry ipa_be_backhaul_cache[IPA_BE_BACKHAUL_CACHE_SIZE];
static int ipa_be_backhaul_cache_count;
static DEFINE_MUTEX(ipa_be_backhaul_cache_lock);

/* Per-interface count of QoS route rules actually installed in HW (not the
 * SW cache count), driving the BE pipe HOLB retime.
 */
struct ipa_be_qos_retime_entry {
	struct list_head node;
	char dev_name[IPA_RESOURCE_NAME_MAX];
	/* Plain u32, not atomic_t: always accessed under ipa_be_qos_retime_lock. */
	u32 hw_rule_count;
};

static LIST_HEAD(ipa_be_qos_retime_list);
static DEFINE_MUTEX(ipa_be_qos_retime_lock);

/**
 * ipa_be_qos_retime_on_rule_add() - note a QoS HW rule installed on
 * @dev_name; tightens its BE pipe HOLB on the 0 -> 1 transition.
 * @dev_name: interface the rule was installed on (qos_param->dev_name)
 */
static void ipa_be_qos_retime_on_rule_add(const char *dev_name)
{
	struct ipa_be_qos_retime_entry *entry;
	bool first_rule = false;

	if (!dev_name || !dev_name[0])
		return;

	mutex_lock(&ipa_be_qos_retime_lock);
	list_for_each_entry(entry, &ipa_be_qos_retime_list, node) {
		if (!strcmp(entry->dev_name, dev_name)) {
			entry->hw_rule_count++;
			mutex_unlock(&ipa_be_qos_retime_lock);
			return;
		}
	}
	/* Not found: this is the interface's first HW QoS rule. */
	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry) {
		IPA_BE_ERR("Failed to allocate QoS retime entry for %s; BE pipe HOLB will not be tightened\n",
			   dev_name);
		mutex_unlock(&ipa_be_qos_retime_lock);
		return;
	}
	strscpy(entry->dev_name, dev_name, sizeof(entry->dev_name));
	entry->hw_rule_count = 1;
	list_add_tail(&entry->node, &ipa_be_qos_retime_list);
	first_rule = true;
	mutex_unlock(&ipa_be_qos_retime_lock);

	if (first_rule)
		ipa3_qos_retime_be_pipes(dev_name, true);
}

/**
 * ipa_be_qos_retime_on_rule_del() - note a QoS HW rule deleted from
 * @dev_name; restores its BE pipe HOLB on the 1 -> 0 transition.
 * @dev_name: interface the rule was deleted from
 */
static void ipa_be_qos_retime_on_rule_del(const char *dev_name)
{
	struct ipa_be_qos_retime_entry *entry;
	bool last_rule = false;

	if (!dev_name || !dev_name[0])
		return;

	mutex_lock(&ipa_be_qos_retime_lock);
	list_for_each_entry(entry, &ipa_be_qos_retime_list, node) {
		if (strcmp(entry->dev_name, dev_name))
			continue;
		/* Guard against underflow on a bookkeeping mismatch. */
		if (entry->hw_rule_count > 0) {
			entry->hw_rule_count--;
			if (entry->hw_rule_count == 0) {
				list_del(&entry->node);
				kfree(entry);
				last_rule = true;
			}
		} else {
			IPA_BE_ERR("QoS retime count for %s already 0 on delete; bookkeeping mismatch\n",
				   dev_name);
		}
		break;
	}
	mutex_unlock(&ipa_be_qos_retime_lock);

	if (last_rule)
		ipa3_qos_retime_be_pipes(dev_name, false);
}

/**
 * ipa_be_qos_retime_on_flush() - record that ALL QoS HW rules were deleted
 * (IPA_QOS_PARAM_FLUSH_EVENT). Restores BE pipe HOLB on every interface that
 * had a HW rule, then clears the tracking list.
 */
static void ipa_be_qos_retime_on_flush(void)
{
	struct ipa_be_qos_retime_entry *entry, *tmp;
	/* Retime calls happen after the lock is dropped and the list is emptied. */
	char (*devs)[IPA_RESOURCE_NAME_MAX];
	u32 num_devs = 0;
	u32 count = 0;
	u32 i;

	mutex_lock(&ipa_be_qos_retime_lock);
	list_for_each_entry(entry, &ipa_be_qos_retime_list, node)
		count++;
	if (count == 0) {
		mutex_unlock(&ipa_be_qos_retime_lock);
		return;
	}

	devs = kcalloc(count, sizeof(*devs), GFP_KERNEL);
	if (!devs) {
		/* Still drain the list -- FLUSH means every HW rule is gone. */
		IPA_BE_ERR("Failed to allocate QoS retime dev list on flush; BE pipes left tightened\n");
		list_for_each_entry_safe(entry, tmp,
					 &ipa_be_qos_retime_list, node) {
			list_del(&entry->node);
			kfree(entry);
		}
		mutex_unlock(&ipa_be_qos_retime_lock);
		return;
	}

	list_for_each_entry_safe(entry, tmp, &ipa_be_qos_retime_list, node) {
		if (num_devs < count)
			strscpy(devs[num_devs++], entry->dev_name,
				IPA_RESOURCE_NAME_MAX);
		list_del(&entry->node);
		kfree(entry);
	}
	mutex_unlock(&ipa_be_qos_retime_lock);

	for (i = 0; i < num_devs; i++)
		ipa3_qos_retime_be_pipes(devs[i], false);
	kfree(devs);
}

/**
 * ipa_be_detect_backhaul_type() - Detect backhaul type based on WAN interface
 * @wan_iface: WAN interface number (pdn_iface)
 *
 * Detection logic:
 *   - Modem interfaces have ext_props (for QMI communication)
 *   - Ethernet backhaul interfaces don't have ext_props
 *
 * Returns backhaul type (MODEM or ETH)
 */
int ipa_be_detect_backhaul_type(int wan_iface, enum ipa_backhaul_type *out_type)
{
	struct ipa_ioc_query_intf wan_intf;
	enum ipa_backhaul_type type;
	int i;

	mutex_lock(&ipa_be_backhaul_cache_lock);
	for (i = 0; i < ipa_be_backhaul_cache_count; i++) {
		if (ipa_be_backhaul_cache[i].wan_iface == wan_iface) {
			*out_type = ipa_be_backhaul_cache[i].type;
			mutex_unlock(&ipa_be_backhaul_cache_lock);
			return 0;
		}
	}

	if (!ipa3_query_iface(wan_iface, &wan_intf)) {
		mutex_unlock(&ipa_be_backhaul_cache_lock);
		IPA_BE_ERR("WAN interface %d does not exist\n", wan_iface);
		return -ENODEV;
	}

	if (wan_intf.num_ext_props > 0) {
		IPA_BE_DBG("WAN interface %d (%s) is modem (ext_props=%d)\n",
			wan_iface, wan_intf.name, wan_intf.num_ext_props);
		type = IPA_BACKHAUL_TYPE_MODEM;
	} else {
		IPA_BE_DBG("WAN interface %d (%s) is eth backhaul (no ext_props)\n",
			wan_iface, wan_intf.name);
		type = IPA_BACKHAUL_TYPE_ETH;
	}

	if (ipa_be_backhaul_cache_count < IPA_BE_BACKHAUL_CACHE_SIZE) {
		ipa_be_backhaul_cache[ipa_be_backhaul_cache_count].wan_iface = wan_iface;
		ipa_be_backhaul_cache[ipa_be_backhaul_cache_count].type = type;
		ipa_be_backhaul_cache_count++;
	} else {
		WARN_ONCE(1, "ipa_be_backhaul_cache full (size=%d); skipping insert for iface %d\n",
			IPA_BE_BACKHAUL_CACHE_SIZE, wan_iface);
	}
	mutex_unlock(&ipa_be_backhaul_cache_lock);
	*out_type = type;
	return 0;
}

void ipa_be_backhaul_cache_invalidate(int wan_iface)
{
	int i;

	mutex_lock(&ipa_be_backhaul_cache_lock);
	for (i = 0; i < ipa_be_backhaul_cache_count; i++) {
		if (ipa_be_backhaul_cache[i].wan_iface == wan_iface) {
			ipa_be_backhaul_cache[i] = ipa_be_backhaul_cache[ipa_be_backhaul_cache_count - 1];
			ipa_be_backhaul_cache_count--;
			break;
		}
	}
	mutex_unlock(&ipa_be_backhaul_cache_lock);
}

static bool ipa_be_is_uplink_lan2wan_eth_bh(int return_iface, int return_top_iface,
					   bool lan2lan)
{
	enum ipa_backhaul_type t;

	return !lan2lan &&
	       return_iface == return_top_iface &&
	       ipa_be_detect_backhaul_type(return_iface, &t) == 0 &&
	       t == IPA_BACKHAUL_TYPE_ETH;
}


const char* ipa_be_message_type_to_string(enum ipa_message_types type) {
	switch (type) {
		case IPA_TX_CREATE_RULE_MSG: return "IPA_TX_CREATE_RULE_MSG";
		case IPA_TX_DESTROY_RULE_MSG: return "IPA_TX_DESTROY_RULE_MSG";
		case IPA_RX_CONN_STATS_SYNC_MSG: return "IPA_RX_CONN_STATS_SYNC_MSG";
		case IPA_TX_CONN_STATS_SYNC_MANY_MSG: return "IPA_TX_CONN_STATS_SYNC_MANY_MSG";
		case IPA_TX_CONN_STATS_SYNC_MANY_TS_ONLY_MSG: return "IPA_TX_CONN_STATS_SYNC_MANY_TS_ONLY_MSG";
		case IPA_TX_CONN_STATS_SYNC_MANY_STATS_ONLY_MSG: return "IPA_TX_CONN_STATS_SYNC_MANY_STATS_ONLY_MSG";
		case IPA_TUN6RD_ADD_UPDATE_PEER: return "IPA_TUN6RD_ADD_UPDATE_PEER";
		case IPA_TX_CREATE_MULTICAST_RULE_MSG: return "IPA_TX_CREATE_MULTICAST_RULE_MSG";
		case IPA_TX_DESTROY_MULTICAST_RULE_MSG: return "IPA_TX_DESTROY_MULTICAST_RULE_MSG";
		case IPA_MAX_MSG_TYPES: return "IPA_MAX_MSG_TYPES";
		default: return "UNKNOWN";
	}
}

const char* ipa_be_cmn_response_to_string(enum ipa_cmn_response type) {
	switch (type) {
		case IPA_CMN_RESPONSE_ACK: return "IPA_CMN_RESPONSE_ACK";
		case IPA_CMN_RESPONSE_EVERSION: return "IPA_CMN_RESPONSE_EVERSION";
		case IPA_CMN_RESPONSE_EINTERFACE: return "IPA_CMN_RESPONSE_EINTERFACE";
		case IPA_CMN_RESPONSE_ELENGTH: return "IPA_CMN_RESPONSE_ELENGTH";
		case IPA_CMN_RESPONSE_EMSG: return "IPA_CMN_RESPONSE_EMSG";
		case IPA_CMN_RESPONSE_NOTIFY: return "IPA_CMN_RESPONSE_NOTIFY";
		case IPA_CMN_RESPONSE_LAST: return "IPA_CMN_RESPONSE_LAST";
		default: return "UNKNOWN";
	}
}

/*
 * ipa_enqueue_msg()
 *	Queue response message
 *
 * @param sfe_ctx IPA context
 * @param response response message to be queue
 */
static inline void ipa_enqueue_msg(struct ipa_ctx_instance_internal *ipa_be_ctx, struct ipa_response_msg *response)
{
	spin_lock_bh(&ipa_be_ctx->lock);
	list_add_tail(&response->node, &ipa_be_ctx->msg_queue);
	spin_unlock_bh(&ipa_be_ctx->lock);

	schedule_work(&ipa_be_ctx->work);
}


/*
 * ipa_incr_exceptions()
 *	Increase an exception counter.
 *
 * TODO:  Merge ipa_ctx stats to ipv4 and ipv6 percpu stats.
 */
static inline void ipa_incr_exceptions(ipa_exception_t except)
{
	struct ipa_ctx_instance_internal *ipa_ctx = &__ipa_be_ctx;

	spin_lock_bh(&ipa_ctx->lock);
	ipa_ctx->exceptions[except]++;
	spin_unlock_bh(&ipa_ctx->lock);
}

/*
 * ipa_alloc_response_msg()
 *	Alloc and construct new response message
 *
 * @param type message type
 * @param msg used to construct response message if not NULL
 *
 * @return !NULL, success; NULL, failed
 */
static struct ipa_response_msg *
ipa_alloc_response_msg(ipa_msg_types_t type, void *msg)
{
	struct ipa_response_msg *response;
	int size;

	switch (type) {
	case IPA_MSG_TYPE_IPV4:
		size = sizeof(struct ipa_ipv4_msg);
		break;
	case IPA_MSG_TYPE_IPV6:
		size = sizeof(struct ipa_ipv6_msg);
		break;
	default:
		IPA_BE_ERR("message type %d not supported\n", type);
		return NULL;
	}

	response = kzalloc(sizeof(struct ipa_response_msg) + size, GFP_ATOMIC);
	if (!response) {
		IPA_BE_ERR("allocate memory failed\n");
		return NULL;
	}

	response->type = type;
	if (msg) {
		memcpy(response->msg, msg, size);
	}

	return response;
}


struct ipa_ipv4_work {
	struct work_struct work;
	struct ipa_ctx_instance *ipa_ctx;
	struct ipa_ipv4_msg *msg;
};

struct ipa_ipv6_work {
	struct work_struct work;
	struct ipa_ctx_instance *ipa_ctx;
	struct ipa_ipv6_msg *msg;
};


static void ipa_ipv4_work_handler(struct work_struct *work)
{
	struct ipa_ipv4_work *ipv4_work = container_of(work, struct ipa_ipv4_work, work);
	enum ipa_cmn_response ret;
	struct ipa_response_msg *response;

	ret = ipa_be_ipv4_send_request_with_resp(ipv4_work->ipa_ctx, ipv4_work->msg);
	IPA_BE_DBG("ECMIPA deferred return %s\n", ipa_be_cmn_response_to_string(ret));

	/* Always allocate a response message */
	response = ipa_alloc_response_msg(IPA_MSG_TYPE_IPV4, ipv4_work->msg);
	if (!response) {
		IPA_BE_ERR("ECMIPA failed to allocate response message\n");
		goto out;
	}

	/* Set response to actual return value, even if it's an error */
	((struct ipa_ipv4_msg *)response->msg)->cm.response = ipv4_work->msg->cm.response = ret;

	if (ret == IPA_CMN_RESPONSE_EMSG) {
		ipa_incr_exceptions(IPA_EXCEPTION_IPV4_MSG_UNKNOWN);
		IPA_BE_DBG("ECMIPA deferred return IPA_TX_FAILURE_NOT_ENABLED\n");
	}

	ipa_enqueue_msg(IPA_CTX_TO_PRIVATE(ipv4_work->ipa_ctx), response);
	IPA_BE_DBG("ECMIPA response enqueued\n");

out:
	kfree(ipv4_work->msg);
	kfree(ipv4_work);
}

static void ipa_ipv6_work_handler(struct work_struct *work)
{
	struct ipa_ipv6_work *ipv6_work = container_of(work, struct ipa_ipv6_work, work);
	enum ipa_cmn_response ret;
	struct ipa_response_msg *response;

	ret = ipa_be_ipv6_send_request_with_resp(ipv6_work->ipa_ctx, ipv6_work->msg);
	IPA_BE_DBG("ECMIPA deferred return %d\n", ret);

	/* Always allocate a response message */
	response = ipa_alloc_response_msg(IPA_MSG_TYPE_IPV6, ipv6_work->msg);
	if (!response) {
		IPA_BE_ERR("ECMIPA failed to allocate response message\n");
		goto out;
	}

	/* Set response to actual return value, even if it's an error */
	((struct ipa_ipv6_msg *)response->msg)->cm.response = ipv6_work->msg->cm.response = ret;

	if (ret == IPA_CMN_RESPONSE_EMSG) {
		ipa_incr_exceptions(IPA_EXCEPTION_IPV6_MSG_UNKNOWN);
		IPA_BE_DBG("ECMIPA deferred return IPA_TX_FAILURE_NOT_ENABLED\n");
	}

	ipa_enqueue_msg(IPA_CTX_TO_PRIVATE(ipv6_work->ipa_ctx), response);
	IPA_BE_DBG("ECMIPA response enqueued\n");

out:
	kfree(ipv6_work->msg);
	kfree(ipv6_work);
}

/* ========================================================================== */
/* RULE PROCESSING FUNCTIONS                                                  */
/* ========================================================================== */

u8 ipa_be_get_ring_id(u32 intf_num)
{
	struct ipa_ioc_query_intf temp_intf;
	struct ipa_ioc_query_intf_rx_props *rx_prop;
	u8 ring_id = 0;

	if (!ipa3_query_iface(intf_num, &temp_intf))
		return 0;

	if (temp_intf.num_rx_props == 0)
		return 0;

	rx_prop = kzalloc(sizeof(struct ipa_ioc_query_intf_rx_props) +
		temp_intf.num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (!rx_prop)
		return 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(rx_prop->name, temp_intf.name, sizeof(rx_prop->name));
#else
	strlcpy(rx_prop->name, temp_intf.name, sizeof(rx_prop->name));
#endif
	rx_prop->num_rx_props = temp_intf.num_rx_props;

	if (ipa3_query_intf_rx_props(rx_prop) == 0) {
		if (rx_prop->num_rx_props > 0)
			ring_id = rx_prop->rx[0].rdi;
	}

	kfree(rx_prop);
	return ring_id;
}
EXPORT_SYMBOL(ipa_be_get_ring_id);

/* Returns the IPA hardware endpoint index for an interface, or -1 on failure. */
int ipa_be_get_ep_for_intf(s32 intf_num)
{
	return ipa3_get_ep_for_intf(intf_num);
}

/*
 * ipa_be_flow_canonical_cmp - compare two LAN client endpoints for canonical ordering.
 *
 * Returns 1 when flow is canonical (private/src side), 0 when return is
 * canonical, or -1 on EP query failure or NULL MAC (always logged).
 * Canonical order: smaller IPA EP index first; MAC tiebreaker when equal.
 */
int ipa_be_flow_canonical_cmp(s32 flow_intf, s32 return_intf,
		const u8 *flow_mac, const u8 *return_mac)
{
	int flow_ep;
	int return_ep;

	if (!flow_mac || !return_mac) {
		IPA_BE_ERR("NULL MAC address (flow_mac=%p return_mac=%p)\n",
			   flow_mac, return_mac);
		return -1;
	}

	flow_ep = ipa_be_get_ep_for_intf(flow_intf);
	return_ep = ipa_be_get_ep_for_intf(return_intf);

	if (flow_ep < 0 || return_ep < 0) {
		IPA_BE_ERR("EP query failed (flow_intf=%d ret_intf=%d)\n",
			   flow_intf, return_intf);
		return -1;
	}

	if (flow_ep != return_ep)
		return flow_ep < return_ep ? 1 : 0;

	/* Same EP: use MAC address as tiebreaker. */
	return memcmp(flow_mac, return_mac, IPA_MAC_ADDR_SIZE) < 0 ? 1 : 0;
}

static inline bool is_ipv4_lan_to_lan(const struct ipa_ipv4_rule_create_msg *msg)
{
	if (msg->rule_flags & IPA_RULE_CREATE_FLAG_BRIDGE_FLOW)
		return true;

	return false;
}

static inline bool is_ipv6_lan_to_lan(const struct ipa_ipv6_rule_create_msg *msg)
{
	if (msg->rule_flags & IPA_RULE_CREATE_FLAG_BRIDGE_FLOW)
		return true;

	return false;
}

static inline bool is_lan_to_lan_connection(enum ipa_ip_type ip, const void *msg)
{
	switch (ip) {
		case IPA_IP_v4:
			return is_ipv4_lan_to_lan((const struct ipa_ipv4_rule_create_msg *)msg);

		case IPA_IP_v6:
			return is_ipv6_lan_to_lan((const struct ipa_ipv6_rule_create_msg *)msg);

		default:
			return false;
    }
}

static void ipa_be_log_ipv4_rule_details(struct ipa_ipv4_rule_create_msg v4_msg)
{
	IPA_BE_DBG("ECMIPA rule flags 0x%x\n", v4_msg.rule_flags);
	IPA_BE_DBG("ECMIPA create flow_ip: %pI4n:%d protocol: %d\n", &v4_msg.tuple.flow_ip, ntohs(v4_msg.tuple.flow_ident), v4_msg.tuple.protocol);
	IPA_BE_DBG("ECMIPA create return_flow_ip: %pI4n:%d\n", &v4_msg.tuple.return_ip, ntohs(v4_msg.tuple.return_ident));

	IPA_BE_DBG("ECMIPA create flow_ip_xlate: %pI4n:%d protocol: %d\n", &v4_msg.conn_rule.flow_ip_xlate, ntohs(v4_msg.conn_rule.flow_ident_xlate), v4_msg.tuple.protocol);
	IPA_BE_DBG("ECMIPA create return flow_ip_xlate: %pI4n:%d protocol: %d\n", &v4_msg.conn_rule.return_ip_xlate, ntohs(v4_msg.conn_rule.return_ident_xlate), v4_msg.tuple.protocol);

	IPA_BE_DBG("ECMIPA create "
				"valid_flags: 0x%x "
				"rule_flags: 0x%x "
				"flow_interface_num: %u "
				"return_interface_num: %u "
				"flow_top_interface_num: %u "
				"return_top_interface_num: %u ",
				v4_msg.valid_flags,
			   v4_msg.rule_flags,
			   v4_msg.conn_rule.flow_interface_num,
			   v4_msg.conn_rule.return_interface_num,
			   v4_msg.conn_rule.flow_top_interface_num,
			   v4_msg.conn_rule.return_top_interface_num);

	IPA_BE_DBG("ECMIPA create "
				"flow_mac: %pM "
				"return_mac: %pM\n"
				"flow_mtu: %u\n"
				"return_mtu: %u\n",
			   v4_msg.conn_rule.flow_mac,
			   v4_msg.conn_rule.return_mac,
			   v4_msg.conn_rule.flow_mtu,
			   v4_msg.conn_rule.return_mtu);

	IPA_BE_DBG("ECMIPA create "
				"ingress_inner_vlan_tag: 0x%x\n"
				"egress_inner_vlan_tag: 0x%x\n"
				"ingress_outer_vlan_tag: 0x%x\n"
				"egress_outer_vlan_tag: 0x%x\n"
				"flow_pppoe_session_id: %u\n"
				"flow_pppoe_remote_mac: %pM\n"
				"return_pppoe_session_id: %u\n"
				"return_pppoe_remote_mac: %pM\n",
			   v4_msg.vlan_primary_rule.ingress_vlan_tag,
			   v4_msg.vlan_primary_rule.egress_vlan_tag,
			   v4_msg.vlan_secondary_rule.ingress_vlan_tag,
			   v4_msg.vlan_secondary_rule.egress_vlan_tag,
			   v4_msg.pppoe_rule.flow_pppoe_session_id,
			   v4_msg.pppoe_rule.flow_pppoe_remote_mac,
			   v4_msg.pppoe_rule.return_pppoe_session_id,
			   v4_msg.pppoe_rule.return_pppoe_remote_mac);

	IPA_BE_DBG("ECMIPA create "
				"tcp flow_max_window: %u\n"
				"tcp return_max_window: %u\n"
				"tcp flow_end: %u\n"
				"tcp return_end: %u\n"
				"tcp flow_max_end: %u\n"
				"tcp return_max_end: %u\n"
				"tcp flow_window_scale: %u\n"
				"tcp return_window_scale: %u\n"
				"flow_dscp: %u\n"
				"return_dscp: %u\n",
			   v4_msg.tcp_rule.flow_max_window,
			   v4_msg.tcp_rule.return_max_window,
			   v4_msg.tcp_rule.flow_end,
			   v4_msg.tcp_rule.return_end,
			   v4_msg.tcp_rule.flow_max_end,
			   v4_msg.tcp_rule.return_max_end,
			   v4_msg.tcp_rule.flow_window_scale,
			   v4_msg.tcp_rule.return_window_scale,
			   v4_msg.dscp_rule.flow_dscp,
			   v4_msg.dscp_rule.return_dscp);
}

static void ipa_be_log_ipv6_rule_details(struct ipa_ipv6_rule_create_msg v6_msg)
{
	IPA_BE_DBG("ECMIPA rule flags 0x%x\n", v6_msg.rule_flags);
	IPA_BE_DBG("ECMIPA create flow_ip: %pI6n:%d protocol: %d\n", &v6_msg.tuple.flow_ip, ntohs(v6_msg.tuple.flow_ident), v6_msg.tuple.protocol);
	IPA_BE_DBG("ECMIPA create return_flow_ip: %pI6n:%d\n", &v6_msg.tuple.return_ip, ntohs(v6_msg.tuple.return_ident));

	IPA_BE_DBG("ECMIPA create flow_ip_xlate: %pI6n:%d protocol: %d\n", &v6_msg.conn_rule.flow_ip_xlate, ntohs(v6_msg.conn_rule.flow_ident_xlate), v6_msg.tuple.protocol);
	IPA_BE_DBG("ECMIPA create return flow_ip_xlate: %pI6n:%d protocol: %d\n", &v6_msg.conn_rule.return_ip_xlate, ntohs(v6_msg.conn_rule.return_ident_xlate), v6_msg.tuple.protocol);

	IPA_BE_DBG("ECMIPA create "
				"valid_flags: 0x%x "
				"rule_flags: 0x%x "
				"flow_interface_num: %u "
				"return_interface_num: %u "
				"flow_top_interface_num: %u "
				"return_top_interface_num: %u ",
				v6_msg.valid_flags,
			   v6_msg.rule_flags,
			   v6_msg.conn_rule.flow_interface_num,
			   v6_msg.conn_rule.return_interface_num,
			   v6_msg.conn_rule.flow_top_interface_num,
			   v6_msg.conn_rule.return_top_interface_num);

	IPA_BE_DBG("ECMIPA create "
				"flow_mac: %pM "
				"return_mac: %pM\n"
				"flow_mtu: %u\n"
				"return_mtu: %u\n",
			   v6_msg.conn_rule.flow_mac,
			   v6_msg.conn_rule.return_mac,
			   v6_msg.conn_rule.flow_mtu,
			   v6_msg.conn_rule.return_mtu);

	IPA_BE_DBG("ECMIPA create "
				"ingress_inner_vlan_tag: 0x%x\n"
				"egress_inner_vlan_tag: 0x%x\n"
				"ingress_outer_vlan_tag: 0x%x\n"
				"egress_outer_vlan_tag: 0x%x\n"
				"flow_pppoe_session_id: %u\n"
				"flow_pppoe_remote_mac: %pM\n"
				"return_pppoe_session_id: %u\n"
				"return_pppoe_remote_mac: %pM\n",
			   v6_msg.vlan_primary_rule.ingress_vlan_tag,
			   v6_msg.vlan_primary_rule.egress_vlan_tag,
			   v6_msg.vlan_secondary_rule.ingress_vlan_tag,
			   v6_msg.vlan_secondary_rule.egress_vlan_tag,
			   v6_msg.pppoe_rule.flow_pppoe_session_id,
			   v6_msg.pppoe_rule.flow_pppoe_remote_mac,
			   v6_msg.pppoe_rule.return_pppoe_session_id,
			   v6_msg.pppoe_rule.return_pppoe_remote_mac);

	IPA_BE_DBG("ECMIPA create "
				"tcp flow_max_window: %u\n"
				"tcp return_max_window: %u\n"
				"tcp flow_end: %u\n"
				"tcp return_end: %u\n"
				"tcp flow_max_end: %u\n"
				"tcp return_max_end: %u\n"
				"tcp flow_window_scale: %u\n"
				"tcp return_window_scale: %u\n"
				"flow_dscp: %u\n"
				"return_dscp: %u\n",
			   v6_msg.tcp_rule.flow_max_window,
			   v6_msg.tcp_rule.return_max_window,
			   v6_msg.tcp_rule.flow_end,
			   v6_msg.tcp_rule.return_end,
			   v6_msg.tcp_rule.flow_max_end,
			   v6_msg.tcp_rule.return_max_end,
			   v6_msg.tcp_rule.flow_window_scale,
			   v6_msg.tcp_rule.return_window_scale,
			   v6_msg.dscp_rule.flow_dscp,
			   v6_msg.dscp_rule.return_dscp);
}

static void ipa_be_log_ipv4_destroy_details(const struct ipa_ipv4_rule_destroy_msg *msg)
{
	IPA_BE_DBG("ECMIPA destroy rule_flags: 0x%x valid_flags: 0x%x\n",
		   msg->rule_flags, msg->valid_flags);
	IPA_BE_DBG("ECMIPA destroy flow_ip: %pI4n:%d protocol: %d\n",
		   &msg->tuple.flow_ip, ntohs(msg->tuple.flow_ident), msg->tuple.protocol);
	IPA_BE_DBG("ECMIPA destroy return_ip: %pI4n:%d\n",
		   &msg->tuple.return_ip, ntohs(msg->tuple.return_ident));
	IPA_BE_DBG("ECMIPA destroy flow_ip_xlate: %pI4n:%d\n",
		   &msg->conn_rule.flow_ip_xlate, ntohs(msg->conn_rule.flow_ident_xlate));
	IPA_BE_DBG("ECMIPA destroy return_ip_xlate: %pI4n:%d\n",
		   &msg->conn_rule.return_ip_xlate, ntohs(msg->conn_rule.return_ident_xlate));
	IPA_BE_DBG("ECMIPA destroy "
		   "flow_interface_num: %u "
		   "return_interface_num: %u "
		   "flow_top_interface_num: %u "
		   "return_top_interface_num: %u\n",
		   msg->conn_rule.flow_interface_num,
		   msg->conn_rule.return_interface_num,
		   msg->conn_rule.flow_top_interface_num,
		   msg->conn_rule.return_top_interface_num);
	IPA_BE_DBG("ECMIPA destroy "
		   "flow_mac: %pM "
		   "return_mac: %pM\n"
		   "flow_mtu: %u\n"
		   "return_mtu: %u\n",
		   msg->conn_rule.flow_mac,
		   msg->conn_rule.return_mac,
		   msg->conn_rule.flow_mtu,
		   msg->conn_rule.return_mtu);
	IPA_BE_DBG("ECMIPA destroy "
		   "ingress_inner_vlan_tag: 0x%x\n"
		   "egress_inner_vlan_tag: 0x%x\n"
		   "ingress_outer_vlan_tag: 0x%x\n"
		   "egress_outer_vlan_tag: 0x%x\n"
		   "flow_pppoe_session_id: %u\n"
		   "flow_pppoe_remote_mac: %pM\n"
		   "return_pppoe_session_id: %u\n"
		   "return_pppoe_remote_mac: %pM\n",
		   msg->vlan_primary_rule.ingress_vlan_tag,
		   msg->vlan_primary_rule.egress_vlan_tag,
		   msg->vlan_secondary_rule.ingress_vlan_tag,
		   msg->vlan_secondary_rule.egress_vlan_tag,
		   msg->pppoe_rule.flow_pppoe_session_id,
		   msg->pppoe_rule.flow_pppoe_remote_mac,
		   msg->pppoe_rule.return_pppoe_session_id,
		   msg->pppoe_rule.return_pppoe_remote_mac);
	IPA_BE_DBG("ECMIPA destroy "
		   "tcp flow_max_window: %u\n"
		   "tcp return_max_window: %u\n"
		   "tcp flow_end: %u\n"
		   "tcp return_end: %u\n"
		   "tcp flow_max_end: %u\n"
		   "tcp return_max_end: %u\n"
		   "tcp flow_window_scale: %u\n"
		   "tcp return_window_scale: %u\n"
		   "flow_dscp: %u\n"
		   "return_dscp: %u\n",
		   msg->tcp_rule.flow_max_window,
		   msg->tcp_rule.return_max_window,
		   msg->tcp_rule.flow_end,
		   msg->tcp_rule.return_end,
		   msg->tcp_rule.flow_max_end,
		   msg->tcp_rule.return_max_end,
		   msg->tcp_rule.flow_window_scale,
		   msg->tcp_rule.return_window_scale,
		   msg->dscp_rule.flow_dscp,
		   msg->dscp_rule.return_dscp);
}

static void ipa_be_log_ipv6_destroy_details(const struct ipa_ipv6_rule_destroy_msg *msg)
{
	IPA_BE_DBG("ECMIPA destroy rule_flags: 0x%x valid_flags: 0x%x\n",
		   msg->rule_flags, msg->valid_flags);
	IPA_BE_DBG("ECMIPA destroy flow_ip: %pI6n:%d protocol: %d\n",
		   &msg->tuple.flow_ip, ntohs(msg->tuple.flow_ident), msg->tuple.protocol);
	IPA_BE_DBG("ECMIPA destroy return_ip: %pI6n:%d\n",
		   &msg->tuple.return_ip, ntohs(msg->tuple.return_ident));
	IPA_BE_DBG("ECMIPA destroy flow_ip_xlate: %pI6n:%d\n",
		   &msg->conn_rule.flow_ip_xlate, ntohs(msg->conn_rule.flow_ident_xlate));
	IPA_BE_DBG("ECMIPA destroy return_ip_xlate: %pI6n:%d\n",
		   &msg->conn_rule.return_ip_xlate, ntohs(msg->conn_rule.return_ident_xlate));
	IPA_BE_DBG("ECMIPA destroy "
		   "flow_interface_num: %u "
		   "return_interface_num: %u "
		   "flow_top_interface_num: %u "
		   "return_top_interface_num: %u\n",
		   msg->conn_rule.flow_interface_num,
		   msg->conn_rule.return_interface_num,
		   msg->conn_rule.flow_top_interface_num,
		   msg->conn_rule.return_top_interface_num);
	IPA_BE_DBG("ECMIPA destroy "
		   "flow_mac: %pM "
		   "return_mac: %pM\n"
		   "flow_mtu: %u\n"
		   "return_mtu: %u\n",
		   msg->conn_rule.flow_mac,
		   msg->conn_rule.return_mac,
		   msg->conn_rule.flow_mtu,
		   msg->conn_rule.return_mtu);
	IPA_BE_DBG("ECMIPA destroy "
		   "ingress_inner_vlan_tag: 0x%x\n"
		   "egress_inner_vlan_tag: 0x%x\n"
		   "ingress_outer_vlan_tag: 0x%x\n"
		   "egress_outer_vlan_tag: 0x%x\n"
		   "flow_pppoe_session_id: %u\n"
		   "flow_pppoe_remote_mac: %pM\n"
		   "return_pppoe_session_id: %u\n"
		   "return_pppoe_remote_mac: %pM\n",
		   msg->vlan_primary_rule.ingress_vlan_tag,
		   msg->vlan_primary_rule.egress_vlan_tag,
		   msg->vlan_secondary_rule.ingress_vlan_tag,
		   msg->vlan_secondary_rule.egress_vlan_tag,
		   msg->pppoe_rule.flow_pppoe_session_id,
		   msg->pppoe_rule.flow_pppoe_remote_mac,
		   msg->pppoe_rule.return_pppoe_session_id,
		   msg->pppoe_rule.return_pppoe_remote_mac);
	IPA_BE_DBG("ECMIPA destroy "
		   "tcp flow_max_window: %u\n"
		   "tcp return_max_window: %u\n"
		   "tcp flow_end: %u\n"
		   "tcp return_end: %u\n"
		   "tcp flow_max_end: %u\n"
		   "tcp return_max_end: %u\n"
		   "tcp flow_window_scale: %u\n"
		   "tcp return_window_scale: %u\n"
		   "flow_dscp: %u\n"
		   "return_dscp: %u\n",
		   msg->tcp_rule.flow_max_window,
		   msg->tcp_rule.return_max_window,
		   msg->tcp_rule.flow_end,
		   msg->tcp_rule.return_end,
		   msg->tcp_rule.flow_max_end,
		   msg->tcp_rule.return_max_end,
		   msg->tcp_rule.flow_window_scale,
		   msg->tcp_rule.return_window_scale,
		   msg->dscp_rule.flow_dscp,
		   msg->dscp_rule.return_dscp);
}

/**
 * ipa_be_ipv4_send_request()
 *	Transmit an IPv4 message to the ipa
 *
 * @param ipa_ctx IPA context
 * @param msg The IPv4 message
 *
 * @return ipa_tx_status_t The status of the Tx operation
 */
ipa_tx_status_t ipa_be_ipv4_send_request(struct ipa_ctx_instance *ipa_ctx, struct ipa_ipv4_msg *msg)
{
	//struct ipa_response_msg *response;
	//enum ipa_cmn_response ret;
	IPA_BE_DBG("ECMIPA msg type %u\n", msg->cm.type);
	if (msg->cm.type == IPA_TX_CONN_STATS_SYNC_MANY_MSG ||
	    msg->cm.type == IPA_TX_CONN_STATS_SYNC_MANY_TS_ONLY_MSG ||
	    msg->cm.type == IPA_TX_CONN_STATS_SYNC_MANY_STATS_ONLY_MSG) {
#ifdef CONFIG_ECM_CONVERGENCE
		return ipa_sync_ipv4_stats_many_msg(IPA_CTX_TO_PRIVATE(ipa_ctx), msg);
#else
		IPA_BE_DBG("NAT support disabled - skipping ipa_sync_ipv4_stats_many_msg\n");
		return IPA_TX_FAILURE_NOT_ENABLED;
#endif
	}

	struct ipa_ctx_instance_internal *ipa_be_ctx = &__ipa_be_ctx;
	struct ipa_ipv4_work *ipv4_work;

	IPA_BE_DBG("ECMIPA entry ipa_be_ipv4_send_request called ipa_ctx %p\n", ipa_ctx);

	ipv4_work = kmalloc(sizeof(*ipv4_work), GFP_ATOMIC);
	if (!ipv4_work) {
		IPA_BE_ERR("ECMIPA failed to allocate work struct\n");
		return IPA_TX_FAILURE_QUEUE;
	}

	INIT_WORK(&ipv4_work->work, ipa_ipv4_work_handler);
	ipv4_work->ipa_ctx = ipa_ctx;
	ipv4_work->msg = kmemdup(msg, sizeof(*msg), GFP_ATOMIC);
	if (!ipv4_work->msg) {
		IPA_BE_ERR("ECMIPA failed to duplicate msg\n");
		kfree(ipv4_work);
		return IPA_TX_FAILURE_QUEUE;
	}

	if (!ipa_be_ctx->ipa_ipv4_wq) {
		IPA_BE_ERR("Workqueue not initialized\n");
		kfree(ipv4_work->msg);
		kfree(ipv4_work);
		return IPA_TX_FAILURE_QUEUE;
	}

	if (!queue_work(ipa_be_ctx->ipa_ipv4_wq, &ipv4_work->work)) {
		IPA_BE_ERR("ECMIPA failed to queue work\n");
		kfree(ipv4_work->msg);
		kfree(ipv4_work);
		return IPA_TX_FAILURE_QUEUE;
	}

	IPA_BE_DBG("ECMIPA deferred work scheduled on ordered workqueue\n");
	return IPA_TX_SUCCESS;
}
EXPORT_SYMBOL(ipa_be_ipv4_send_request);

/**
 * ipa_be_ipv6_send_request()
 *	Transmit an IPv6 message to the ipa
 *
 * @param ipa_ctx IPA context
 * @param msg The IPv6 message
 *
 * @return ipa_tx_status_t The status of the Tx operation
 */
ipa_tx_status_t ipa_be_ipv6_send_request(struct ipa_ctx_instance *ipa_ctx, struct ipa_ipv6_msg *msg)
{
	struct ipa_ctx_instance_internal *ipa_be_ctx = &__ipa_be_ctx;
	struct ipa_ipv6_work *ipv6_work;

	IPA_BE_DBG("ECMIPA entry ipa_be_ipv6_send_request called ipa_ctx %p\n", ipa_ctx);
	IPA_BE_DBG("ECMIPA msg type %u\n", msg->cm.type);

	if (msg->cm.type == IPA_TX_CONN_STATS_SYNC_MANY_MSG ||
	    msg->cm.type == IPA_TX_CONN_STATS_SYNC_MANY_TS_ONLY_MSG ||
	    msg->cm.type == IPA_TX_CONN_STATS_SYNC_MANY_STATS_ONLY_MSG) {
#ifdef CONFIG_ECM_CONVERGENCE
		return ipa_sync_ipv6_stats_many_msg(IPA_CTX_TO_PRIVATE(ipa_ctx), msg);
#else
		IPA_BE_DBG("NAT support disabled - skipping ipa_sync_ipv6_stats_many_msg\n");
		return IPA_TX_FAILURE_NOT_ENABLED;
#endif
	}

	ipv6_work = kmalloc(sizeof(*ipv6_work), GFP_ATOMIC);
	if (!ipv6_work) {
		IPA_BE_ERR("ECMIPA failed to allocate work struct\n");
		return IPA_TX_FAILURE_QUEUE;
	}

	INIT_WORK(&ipv6_work->work, ipa_ipv6_work_handler);
	ipv6_work->ipa_ctx = ipa_ctx;
	ipv6_work->msg = kmemdup(msg, sizeof(*msg), GFP_ATOMIC);
	if (!ipv6_work->msg) {
		IPA_BE_ERR("ECMIPA failed to duplicate msg\n");
		kfree(ipv6_work);
		return IPA_TX_FAILURE_QUEUE;
	}

	if (!ipa_be_ctx->ipa_ipv6_wq) {
		IPA_BE_ERR("Workqueue not initialized\n");
		kfree(ipv6_work->msg);
		kfree(ipv6_work);
		return IPA_TX_FAILURE_QUEUE;
	}

	if (!queue_work(ipa_be_ctx->ipa_ipv6_wq, &ipv6_work->work)) {
		IPA_BE_ERR("ECMIPA failed to queue work\n");
		kfree(ipv6_work->msg);
		kfree(ipv6_work);
		return IPA_TX_FAILURE_QUEUE;
	}

	IPA_BE_DBG("ECMIPA deferred work scheduled on ordered v6 workqueue\n");
	return IPA_TX_SUCCESS;

}
EXPORT_SYMBOL(ipa_be_ipv6_send_request);

/* Step constants for create-rule rollback paths */
enum ipa_l2l_step {
	L2L_STEP_RET_MAPPING  = 1,
	L2L_STEP_RET_ROUTE    = 2,
	L2L_STEP_RET_FILTER   = 3,
	L2L_STEP_FLOW_MAPPING = 4,
	L2L_STEP_FLOW_ROUTE   = 5,
	L2L_STEP_FLOW_FILTER  = 6,
};

enum ipa_eth_step {
	ETH_STEP_LAN_MAPPING_ROUTE   = 1,
	ETH_STEP_WAN_UP_DOWNLINK     = 2,
	ETH_STEP_WAN_MAPPING         = 3,
	ETH_STEP_BACKHAUL_RULE_TRUE  = 4,
	ETH_STEP_BACKHAUL_RULE_FALSE = 5,
	ETH_STEP_WAN_UP_UPLINK       = 6,
	ETH_STEP_NAT_ENTRY           = 7,
};

enum ipa_v4_modem_step {
	V4_MDM_STEP_CLIENT_MAPPING = 1,
	V4_MDM_STEP_ROUTE_RULE     = 2,
	V4_MDM_STEP_UPLINK_FILTER  = 3,
	V4_MDM_STEP_MTU_RULE       = 4,
	V4_MDM_STEP_PRIVATE_SUBNET = 5,
	V4_MDM_STEP_DFT_FILTER     = 6,
	V4_MDM_STEP_ICMP_ALG       = 7,
	V4_MDM_STEP_CATCHUP_ALL    = 8,
	V4_MDM_STEP_WAN_FILTER     = 9,
	V4_MDM_STEP_NAT_ENTRY      = 10,
};

enum ipa_v6_modem_step {
	V6_MDM_STEP_CLIENT_MAPPING  = 1,
	V6_MDM_STEP_ROUTE_RULE      = 2,
	V6_MDM_STEP_UPLINK_FILTER   = 3,
	V6_MDM_STEP_MTU_RULE        = 4,
	V6_MDM_STEP_PREFIX_FLT_RULE = 5,
	V6_MDM_STEP_DFT_FILTER      = 6,
	V6_MDM_STEP_ICMP_ALG        = 7,
	V6_MDM_STEP_CATCHUP_ALL     = 8,
	V6_MDM_STEP_WAN_FILTER      = 9,
	V6_MDM_STEP_CT_ENTRY        = 10,
};

/* Free callback for IPA_QOS_RULE_INSTALLED_EVENT messages — matches
 * ipa_msg_free_fn signature (void (*)(void *, u32, u32)) required by
 * ipa_send_msg(). The framework invokes this after copy_to_user delivers
 * the payload to userspace, so we just free the heap allocation. */
static void ipa_qos_rule_installed_evt_free(void *buff, u32 len, u32 type)
{
	kfree(buff);
}

/**
 * ipa_be_check_and_install_qos_for_new_rule - Install matching QoS route rules
 * for a newly added LAN-to-WAN connection.
 * @flow_mac:    MAC address for the flow direction of the new rule
 * @flow_vlan:   VLAN tag for the flow direction (0 if no VLAN)
 * @return_mac:  MAC address for the return direction of the new rule
 * @return_vlan: VLAN tag for the return direction (0 if no VLAN)
 * @flow_ip_v4:  LAN client IPv4 address in network byte order (0 for IPv6)
 * @flow_ip_v6:  LAN client IPv6 address (4 x __be32, network byte order),
 *               or NULL for IPv4 connections
 *
 * Locking: acquires msg_qos_param_lock to snapshot the QoS config list,
 * then releases it before calling ipa_be_install_qos_route_rule() — which
 * may acquire ipa_client_db_lock.  This respects the lock hierarchy
 * (ipa_client_db_lock before msg_qos_param_lock).  Do NOT hold
 * msg_qos_param_lock on entry to this function.
 *
 * Iterates the cached QoS config list and installs any entry whose dst_mac_addr
 * matches either the flow or return MAC of the new connection.  When both the
 * QoS entry and the new rule carry a VLAN ID, the match uses the MAC+VLAN hash
 * table via ipa_be_clientdb_get_mapping_by_mac(); otherwise a direct MAC
 * comparison is performed.
 *
 * For IP-keyed rules (dst_mac == 0, dst_ip set) that were skipped at ADD time
 * because the client was not yet active, installation is retried here using
 * the connection's flow IP.
 */
static void ipa_be_check_and_install_qos_for_new_rule(
	uint8_t *flow_mac, int flow_vlan,
	uint8_t *return_mac, int return_vlan,
	__be32 flow_ip_v4,
	const __be32 *flow_ip_v6)
{
	struct ipa3_qos_config_node *qos_node;
	struct ipa_ioc_qos_config *qos_cfg;
	static const uint8_t null_mac[IPA_MAC_ADDR_SIZE] = {0};
	static const uint32_t null_v6[4] = {0};
	LIST_HEAD(install_list);
	int qos_vlan;
	bool flow_match, return_match;

	/*
	 * Carry both a config copy (for install outside the lock) and a
	 * back-pointer to the cache node so handles can be written back after
	 * install.
	 */
	struct qos_install_task {
		struct list_head list;
		struct ipa3_qos_config_node *orig_node;
		struct ipa_ioc_qos_config config;
	} *task, *tmp_task;
	struct qos_install_task **task_pool = NULL;
	int task_pool_cap = 0, task_pool_idx = 0, pi;

	if (!ipa3_ctx) {
		IPA_BE_ERR("ipa3_ctx is NULL\n");
		return;
	}

	/* Caller must not hold ipa_client_db_lock — ipa_be_install_qos_route_rule()
	 * acquires it internally, which would violate the lock hierarchy. */
	lockdep_assert_not_held(&ipa_client_db_lock);

	if (!flow_mac || !return_mac) {
		IPA_BE_ERR("NULL MAC address pointer\n");
		return;
	}

	if (!ipa3_ctx->num_qos_configs)
		return;

	IPA_BE_DBG("QoS check: new client flow_mac=%pM (vlan=%d) return_mac=%pM (vlan=%d)\n",
		   flow_mac, flow_vlan, return_mac, return_vlan);

	/* Pre-allocate a fixed-size task pool with GFP_KERNEL before taking
	 * the lock to avoid GFP_ATOMIC under mutex. QOS_TASK_POOL_SIZE covers
	 * typical deployments; GFP_ATOMIC is the fallback if pool exhausts. */
	task_pool = kcalloc(QOS_TASK_POOL_SIZE, sizeof(*task_pool), GFP_KERNEL);
	if (task_pool) {
		for (pi = 0; pi < QOS_TASK_POOL_SIZE; pi++) {
			task_pool[pi] = kzalloc(sizeof(*task), GFP_KERNEL);
			if (!task_pool[pi]) {
				IPA_BE_DBG("Pre-allocated %d of %d task slots\n",
					   pi, QOS_TASK_POOL_SIZE);
				break;
			}
			task_pool_cap++;
		}
	} else {
		IPA_BE_DBG("Failed to allocate task pool, will use GFP_ATOMIC\n");
	}

	mutex_lock(&ipa3_ctx->msg_qos_param_lock);
	list_for_each_entry(qos_node, &ipa3_ctx->qos_config_list, list) {
		bool should_install = false;

		qos_cfg = &qos_node->config;
		flow_match = false;
		return_match = false;

		IPA_BE_DBG("QoS check: rule dst_mac=%pM src_mac=%pM (vlan_count=%u qos_rule_hdl=%u)\n",
			   qos_cfg->dst_mac_addr, qos_cfg->src_mac_addr,
			   qos_cfg->vlan_count, qos_cfg->qos_rule_hdl);

		if (!memcmp(qos_cfg->dst_mac_addr, null_mac, IPA_MAC_ADDR_SIZE)) {
			bool has_dst_ip = (qos_cfg->dst_ip_addr != 0) ||
					  memcmp(qos_cfg->dst_v6_ip_addr, null_v6,
						 sizeof(null_v6));

			if (has_dst_ip) {
				/*
				 * IP-keyed rule: was skipped at ADD time if the
				 * client wasn't active yet.  Install now if this
				 * connection's flow IP matches dst_ip_addr and the
				 * rule hasn't been installed already.
				 */
				bool ip_match = false;
				int j;

				if (qos_cfg->ip_type == IPA_IP_v4 &&
				    qos_cfg->dst_ip_addr && flow_ip_v4) {
					ip_match = (qos_cfg->dst_ip_addr ==
						    ntohl(flow_ip_v4));
				} else if (qos_cfg->ip_type == IPA_IP_v6 &&
					   flow_ip_v6 != NULL) {
					ip_match = true;
					for (j = 0; j < 4; j++) {
						if (qos_cfg->dst_v6_ip_addr[j] !=
						    ntohl(flow_ip_v6[j]))
							ip_match = false;
					}
				}
				if (!ip_match || qos_cfg->qos_rule_hdl)
					continue;
				should_install = true;
			} else if (qos_cfg->vlan_count) {
				continue;
			} else {
				/* Catch-all: reinstall if not already active. */
				if (!qos_cfg->qos_rule_hdl)
					should_install = true;
			}
		} else {
			/*
			 * dst_mac-keyed entry: match dst_mac_addr against
			 * flow_mac or return_mac (either direction).
			 * src_mac_addr, if set, is passed as a filter attribute
			 * in the installed rule.
			 *
			 * Strict VLAN semantics:
			 *  - QoS has VLAN, new rule has VLAN: both must match.
			 *  - QoS has no VLAN: match by MAC only.
			 *  - QoS has VLAN, new rule has no VLAN: no match.
			 *
			 * Note: do NOT call ipa_be_clientdb_get_mapping_by_mac()
			 * here — it acquires ipa_client_db_lock, which must not
			 * be taken while holding msg_qos_param_lock.
			 */
			qos_vlan = (qos_cfg->vlan_count > 0) ? qos_cfg->vlan_ids[0] : 0;

			if (!memcmp(flow_mac, qos_cfg->dst_mac_addr, IPA_MAC_ADDR_SIZE)) {
				/*
				 * No VLAN in rule → match any client VLAN.
				 * VLAN in rule → client VLAN must match exactly.
				 */
				if (!qos_vlan || flow_vlan == qos_vlan)
					flow_match = true;
			}

			if (!memcmp(return_mac, qos_cfg->dst_mac_addr, IPA_MAC_ADDR_SIZE)) {
				if (!qos_vlan || return_vlan == qos_vlan)
					return_match = true;
			}

			if ((flow_match || return_match) && !qos_cfg->qos_rule_hdl)
				should_install = true;
		}

		IPA_BE_DBG("QoS check: flow_match=%d return_match=%d should_install=%d\n",
			   flow_match, return_match, should_install);

		if (should_install) {
			if (task_pool_idx < task_pool_cap) {
				task = task_pool[task_pool_idx];
				task_pool[task_pool_idx] = NULL; /* prevent future double-free */
				task_pool_idx++;
			} else {
				task = kzalloc(sizeof(*task), GFP_ATOMIC);
			}
			if (task) {
				task->orig_node = qos_node;
				task->config = *qos_cfg;
				list_add_tail(&task->list, &install_list);
			} else {
				IPA_BE_ERR("Failed to alloc install task, skipping QoS config\n");
			}
		}
	}
	mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

	/* Install rules outside any lock. */
	list_for_each_entry(task, &install_list, list)
		ipa_be_install_qos_route_rule(&task->config);

	/* Single writeback pass — one lock acquisition avoids any risk of
	 * acquiring msg_qos_param_lock while ipa_client_db_lock is held. */
	mutex_lock(&ipa3_ctx->msg_qos_param_lock);
	list_for_each_entry(task, &install_list, list) {
		if (task->config.qos_rule_hdl) {
			task->orig_node->config.qos_rule_hdl = task->config.qos_rule_hdl;
			task->orig_node->config.proc_ctx_hdl = task->config.proc_ctx_hdl;
		}
	}
	mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

	/* Notify userspace HAL of each successfully-installed deferred rule.
	 * The HAL tracks per-handle state under qos_id; on receipt of this
	 * event it fans out QOS_FILTER_SUCCESS to every handle that received
	 * QOS_FILTER_PENDING for this qos_id at createFilter() time. */
	list_for_each_entry(task, &install_list, list) {
		struct ipa_qos_rule_installed_msg *evt;
		struct ipa_msg_meta meta = {};

		if (!task->config.qos_rule_hdl)
			continue;
		if (!task->orig_node->qos_id)
			continue;

		evt = kzalloc(sizeof(*evt), GFP_KERNEL);
		if (!evt)
			continue;
		evt->qos_id        = task->orig_node->qos_id;
		evt->qos_rule_hdl  = task->config.qos_rule_hdl;
		evt->traffic_class = task->config.traffic_class;
		meta.msg_type = IPA_QOS_RULE_INSTALLED_EVENT;
		meta.msg_len  = sizeof(*evt);
		IPA_BE_DBG("Sending IPA_QOS_RULE_INSTALLED_EVENT qos_id=%u hdl=%u tc=%u\n",
			   evt->qos_id, evt->qos_rule_hdl, evt->traffic_class);
		if (ipa_send_msg(&meta, evt, ipa_qos_rule_installed_evt_free))
			kfree(evt);
	}

	list_for_each_entry_safe(task, tmp_task, &install_list, list) {
		list_del(&task->list);
		kfree(task);
	}
	/* Free unused pre-allocated tasks */
	for (pi = task_pool_idx; pi < task_pool_cap; pi++)
		kfree(task_pool[pi]);
	kfree(task_pool);
}

/**
 * struct qos_del_entry - snapshot of a QoS rule to be deleted outside
 *                        msg_qos_param_lock.
 *
 * Collect rule/proc_ctx handles under the lock, zero them in the cache, then
 * perform the actual HW deletions after releasing the lock to avoid potential
 * lock ordering issues with ipa_delete_route_rule() and
 * __ipa3_release_hdr_proc_ctx().
 */
#define MAX_QOS_DEL_BATCH 32
struct qos_del_entry {
	uint32_t rule_hdl;
	uint32_t proc_ctx_hdl;
	uint32_t ip_type;
};

/**
 * for a departing LAN-to-WAN connection.
 * @flow_mac:   MAC address for the flow direction
 * @return_mac: MAC address for the return direction
 *
 * Handles entries whose dst_mac_addr matches the departing connection's
 * flow or return MAC.  The destroy message carries no VLAN information, so
 * matching is done on MAC address alone.  The installed HW route rule and
 * any associated DSCP proc_ctx are both freed; qos_rule_hdl and
 * proc_ctx_hdl in the cache node are zeroed so the entry is eligible for
 * reinstallation when the client reconnects.
 *
 * IP-keyed and catch-all entries are handled separately by
 * ipa_be_delete_ip_keyed_qos_rules_v4/v6 and ipa_be_delete_qos_catch_all_rules.
 */
static void ipa_be_delete_qos_rules_for_connection(
	uint8_t *flow_mac, uint8_t *return_mac)
{
	struct ipa3_qos_config_node *qos_node;
	struct ipa_ioc_qos_config *qos_cfg;
	static const uint8_t null_mac[IPA_MAC_ADDR_SIZE] = {0};
	struct qos_del_entry entries[MAX_QOS_DEL_BATCH];
	int n = 0, i;

	if (!ipa3_ctx) {
		IPA_BE_ERR("ipa3_ctx is NULL\n");
		return;
	}

	if (!flow_mac || !return_mac) {
		IPA_BE_ERR("NULL MAC address pointer\n");
		return;
	}

	/* Single pass: collect handles and clear them under the lock. */
	mutex_lock(&ipa3_ctx->msg_qos_param_lock);
	list_for_each_entry(qos_node, &ipa3_ctx->qos_config_list, list) {
		qos_cfg = &qos_node->config;
		if (!memcmp(qos_cfg->dst_mac_addr, null_mac, IPA_MAC_ADDR_SIZE))
			continue;
		if (!qos_cfg->qos_rule_hdl)
			continue;
		if (memcmp(flow_mac, qos_cfg->dst_mac_addr, IPA_MAC_ADDR_SIZE) &&
		    memcmp(return_mac, qos_cfg->dst_mac_addr, IPA_MAC_ADDR_SIZE))
			continue;
		if (n < MAX_QOS_DEL_BATCH) {
			entries[n].rule_hdl = qos_cfg->qos_rule_hdl;
			entries[n].proc_ctx_hdl = qos_cfg->proc_ctx_hdl;
			entries[n].ip_type = qos_cfg->ip_type;
			n++;
			qos_cfg->qos_rule_hdl = 0;
			qos_cfg->proc_ctx_hdl = 0;
		} else {
			IPA_BE_ERR("Too many QoS rules to delete, max %d\n",
				   MAX_QOS_DEL_BATCH);
		}
	}
	mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

	for (i = 0; i < n; i++) {
		IPA_BE_DBG("Deleting QoS route rule hdl %u on connection teardown\n",
			entries[i].rule_hdl);
		ipa_delete_route_rule(false, entries[i].rule_hdl,
				      (enum ipa_ip_type)entries[i].ip_type);
		if (entries[i].proc_ctx_hdl)
			__ipa3_release_hdr_proc_ctx(entries[i].proc_ctx_hdl);
	}
}

/**
 * ipa_be_delete_ip_keyed_qos_rules_v4 - Delete IPv4 IP-keyed QoS route rules
 * matching a departing connection.
 * @flow_ip:   flow-direction IPv4 address (network byte order)
 * @return_ip: return-direction IPv4 address (network byte order)
 *
 * Called on IPv4 connection teardown for entries that were installed based
 * on an IPv4 destination address filter (dst_mac_addr == 0, dst_ip_addr set).
 * The matching HW rule and any associated proc_ctx are freed; cache handles
 * are zeroed so the entry can be reinstalled when the same IP reconnects.
 */
static void ipa_be_delete_ip_keyed_qos_rules_v4(__be32 flow_ip)
{
	struct ipa3_qos_config_node *qos_node;
	struct ipa_ioc_qos_config *qos_cfg;
	static const uint8_t null_mac[IPA_MAC_ADDR_SIZE] = {0};
	struct qos_del_entry entries[MAX_QOS_DEL_BATCH];
	int n = 0, i;

	if (!ipa3_ctx)
		return;

	/* Single pass: collect handles and clear them under the lock. */
	mutex_lock(&ipa3_ctx->msg_qos_param_lock);
	list_for_each_entry(qos_node, &ipa3_ctx->qos_config_list, list) {
		qos_cfg = &qos_node->config;
		if (memcmp(qos_cfg->dst_mac_addr, null_mac, IPA_MAC_ADDR_SIZE))
			continue;
		if (qos_cfg->ip_type != IPA_IP_v4 || !qos_cfg->dst_ip_addr)
			continue;
		if (!qos_cfg->qos_rule_hdl)
			continue;
		if (qos_cfg->dst_ip_addr != ntohl(flow_ip))
			continue;
		if (n < MAX_QOS_DEL_BATCH) {
			entries[n].rule_hdl = qos_cfg->qos_rule_hdl;
			entries[n].proc_ctx_hdl = qos_cfg->proc_ctx_hdl;
			entries[n].ip_type = IPA_IP_v4;
			n++;
			qos_cfg->qos_rule_hdl = 0;
			qos_cfg->proc_ctx_hdl = 0;
		} else {
			IPA_BE_ERR("Too many QoS rules to delete, max %d\n",
				   MAX_QOS_DEL_BATCH);
		}
	}
	mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

	for (i = 0; i < n; i++) {
		IPA_BE_DBG("Deleting IP-keyed v4 QoS route rule hdl %u on teardown\n",
			entries[i].rule_hdl);
		ipa_delete_route_rule(false, entries[i].rule_hdl, IPA_IP_v4);
		if (entries[i].proc_ctx_hdl)
			__ipa3_release_hdr_proc_ctx(entries[i].proc_ctx_hdl);
	}
}

/**
 * ipa_be_delete_ip_keyed_qos_rules_v6 - Delete IPv6 IP-keyed QoS route rules
 * matching a departing connection.
 * @flow_ip:   flow-direction IPv6 address (4 x __be32, network byte order)
 * @return_ip: return-direction IPv6 address (4 x __be32, network byte order)
 *
 * Same semantics as ipa_be_delete_ip_keyed_qos_rules_v4 but for IPv6.
 * dst_v6_ip_addr in the QoS config is in host byte order (as filled by the
 * IOCTL caller), so each word is converted before comparison.
 */
static void ipa_be_delete_ip_keyed_qos_rules_v6(const __be32 *flow_ip)
{
	struct ipa3_qos_config_node *qos_node;
	struct ipa_ioc_qos_config *qos_cfg;
	static const uint8_t null_mac[IPA_MAC_ADDR_SIZE] = {0};
	static const uint32_t null_v6[4] = {0};
	struct qos_del_entry entries[MAX_QOS_DEL_BATCH];
	int n = 0, i, j;

	if (!ipa3_ctx)
		return;

	if (!flow_ip) {
		IPA_BE_ERR("NULL flow_ip pointer\n");
		return;
	}

	/* Single pass: collect handles and clear them under the lock. */
	mutex_lock(&ipa3_ctx->msg_qos_param_lock);
	list_for_each_entry(qos_node, &ipa3_ctx->qos_config_list, list) {
		bool flow_match = true;

		qos_cfg = &qos_node->config;
		if (memcmp(qos_cfg->dst_mac_addr, null_mac, IPA_MAC_ADDR_SIZE))
			continue;
		if (qos_cfg->ip_type != IPA_IP_v6 ||
		    !memcmp(qos_cfg->dst_v6_ip_addr, null_v6, sizeof(null_v6)))
			continue;
		if (!qos_cfg->qos_rule_hdl)
			continue;
		for (j = 0; j < 4; j++) {
			if (qos_cfg->dst_v6_ip_addr[j] != ntohl(flow_ip[j]))
				flow_match = false;
		}
		if (!flow_match)
			continue;
		if (n < MAX_QOS_DEL_BATCH) {
			entries[n].rule_hdl = qos_cfg->qos_rule_hdl;
			entries[n].proc_ctx_hdl = qos_cfg->proc_ctx_hdl;
			entries[n].ip_type = IPA_IP_v6;
			n++;
			qos_cfg->qos_rule_hdl = 0;
			qos_cfg->proc_ctx_hdl = 0;
		} else {
			IPA_BE_ERR("Too many QoS rules to delete, max %d\n",
				   MAX_QOS_DEL_BATCH);
		}
	}
	mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

	for (i = 0; i < n; i++) {
		IPA_BE_DBG("Deleting IP-keyed v6 QoS route rule hdl %u on teardown\n",
			entries[i].rule_hdl);
		ipa_delete_route_rule(false, entries[i].rule_hdl, IPA_IP_v6);
		if (entries[i].proc_ctx_hdl)
			__ipa3_release_hdr_proc_ctx(entries[i].proc_ctx_hdl);
	}
}

/**
 * ipa_be_delete_qos_catch_all_rules - Delete all catch-all QoS route rules.
 *
 * A "catch-all" entry has no MAC, no IP, and no VLAN filter — it was installed
 * because at least one client was active.  This function is called after the
 * last active client deactivates (ipa_db_mappings == NULL) to remove the now-
 * invalid HW rule.  The cache node is kept so the rule is reinstalled when the
 * next client connects (ipa_be_check_and_install_qos_for_new_rule handles this).
 */
static void ipa_be_delete_qos_catch_all_rules(void)
{
	struct ipa3_qos_config_node *qos_node;
	struct ipa_ioc_qos_config *qos_cfg;
	static const uint8_t null_mac[IPA_MAC_ADDR_SIZE] = {0};
	static const uint32_t null_v6[4] = {0};
	struct qos_del_entry entries[MAX_QOS_DEL_BATCH];
	int n = 0, i;

	if (!ipa3_ctx)
		return;

	/* Single pass: collect handles and clear them under the lock. */
	mutex_lock(&ipa3_ctx->msg_qos_param_lock);
	list_for_each_entry(qos_node, &ipa3_ctx->qos_config_list, list) {
		qos_cfg = &qos_node->config;
		if (memcmp(qos_cfg->dst_mac_addr, null_mac, IPA_MAC_ADDR_SIZE))
			continue;
		if (qos_cfg->dst_ip_addr)
			continue;
		if (memcmp(qos_cfg->dst_v6_ip_addr, null_v6, sizeof(null_v6)))
			continue;
		if (qos_cfg->vlan_count)
			continue;
		if (!qos_cfg->qos_rule_hdl)
			continue;
		if (n < MAX_QOS_DEL_BATCH) {
			entries[n].rule_hdl = qos_cfg->qos_rule_hdl;
			entries[n].proc_ctx_hdl = qos_cfg->proc_ctx_hdl;
			entries[n].ip_type = qos_cfg->ip_type;
			n++;
			qos_cfg->qos_rule_hdl = 0;
			qos_cfg->proc_ctx_hdl = 0;
		} else {
			IPA_BE_ERR("Too many QoS rules to delete, max %d\n",
				   MAX_QOS_DEL_BATCH);
		}
	}
	mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

	for (i = 0; i < n; i++) {
		IPA_BE_DBG("Deleting catch-all QoS route rule hdl %u (no active clients)\n",
			entries[i].rule_hdl);
		ipa_delete_route_rule(false, entries[i].rule_hdl,
				      (enum ipa_ip_type)entries[i].ip_type);
		if (entries[i].proc_ctx_hdl)
			__ipa3_release_hdr_proc_ctx(entries[i].proc_ctx_hdl);
	}
}

/**
 * ipa_be_delete_qos_route_rule - Delete the HW QoS route rule for one
 * specific QoS config entry (IPA_QOS_PARAM_DELETE_EVENT path).
 * @qos_param: the config whose HW rule should be removed
 *
 * Finds the matching entry in qos_config_list using the same key fields as
 * ipa3_delete_qos_params(), deletes its route rule from HW, and clears
 * qos_rule_hdl so the subsequent ipa3_delete_qos_params() cache removal
 * cannot attempt a double-free.  Must be called BEFORE ipa3_delete_qos_params().
 *
 * Return: 0 on success, negative errno on failure
 */
int ipa_be_delete_qos_route_rule(struct ipa_ioc_qos_config *qos_param)
{
	struct ipa3_qos_config_node *node;
	uint32_t rule_hdl = 0, proc_ctx_hdl = 0;
	enum ipa_ip_type ip_type = IPA_IP_v4;
	char retime_dev[IPA_RESOURCE_NAME_MAX] = {0};

	if (!qos_param)
		return -EINVAL;

	if (!ipa3_ctx)
		return -ENODEV;

	IPA_BE_DBG("Entry ipa_be_delete_qos_route_rule: qos_rule_hdl %u (%s-based) tc %u ip_type %d "
		   "flt_cat %d protocol %u dscp %u pcp %u\n",
		   qos_param->qos_rule_hdl,
		   qos_param->qos_rule_hdl ? "handle" : "field",
		   qos_param->traffic_class, qos_param->ip_type,
		   qos_param->flt_cat, qos_param->protocol,
		   qos_param->dscp, qos_param->pcp);
	if (!qos_param->qos_rule_hdl) {
		IPA_BE_DBG("QoS delete params: src_ip 0x%x dst_ip 0x%x "
			"src_port [%u-%u] dst_port [%u-%u] src_mac %pM dst_mac %pM\n",
			qos_param->src_ip_addr, qos_param->dst_ip_addr,
			qos_param->src_port_start, qos_param->src_port_end,
			qos_param->dst_port_start, qos_param->dst_port_end,
			qos_param->src_mac_addr, qos_param->dst_mac_addr);
	}

	mutex_lock(&ipa3_ctx->msg_qos_param_lock);
	list_for_each_entry(node, &ipa3_ctx->qos_config_list, list) {
		/*
		 * Handle-based delete: if qos_rule_hdl is set, treat it as a
		 * qos_id and match the stable unique ID assigned at ADD time.
		 * Field-based delete leaves qos_rule_hdl == 0 and matches on
		 * the full filter spec.
		 */
		if (qos_param->qos_rule_hdl) {
			if (node->qos_id != qos_param->qos_rule_hdl)
				continue;
		} else {
			if (node->config.traffic_class != qos_param->traffic_class ||
			    node->config.flt_cat != qos_param->flt_cat ||
			    node->config.ip_type != qos_param->ip_type ||
			    node->config.src_ip_addr != qos_param->src_ip_addr ||
			    node->config.dst_ip_addr != qos_param->dst_ip_addr ||
			    node->config.src_port_start != qos_param->src_port_start ||
			    node->config.src_port_end != qos_param->src_port_end ||
			    node->config.dst_port_start != qos_param->dst_port_start ||
			    node->config.dst_port_end != qos_param->dst_port_end ||
			    node->config.protocol != qos_param->protocol ||
			    node->config.dscp != qos_param->dscp ||
			    node->config.pcp != qos_param->pcp ||
			    memcmp(node->config.src_v6_ip_addr, qos_param->src_v6_ip_addr,
				   sizeof(qos_param->src_v6_ip_addr)) ||
			    memcmp(node->config.dst_v6_ip_addr, qos_param->dst_v6_ip_addr,
				   sizeof(qos_param->dst_v6_ip_addr)) ||
			    memcmp(node->config.dst_mac_addr, qos_param->dst_mac_addr,
				   sizeof(qos_param->dst_mac_addr)) ||
			    memcmp(node->config.src_mac_addr, qos_param->src_mac_addr,
				   sizeof(qos_param->src_mac_addr)))
				continue;
		}

		rule_hdl = node->config.qos_rule_hdl;
		proc_ctx_hdl = node->config.proc_ctx_hdl;
		ip_type = (enum ipa_ip_type)node->config.ip_type;
		/* Capture before clearing; used for the retime decision below. */
		strscpy(retime_dev, node->config.dev_name, sizeof(retime_dev));
		node->config.qos_rule_hdl = 0;
		node->config.proc_ctx_hdl = 0;
		break;
	}
	mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

	if (rule_hdl) {
		IPA_BE_DBG("Deleting QoS route rule hdl %u on DELETE event\n", rule_hdl);
		ipa_delete_route_rule(false, rule_hdl, ip_type);
		/* Restore this interface's BE pipes if this was the last rule. */
		ipa_be_qos_retime_on_rule_del(retime_dev);
	} else {
		IPA_BE_DBG("QoS entry matched but no rule installed (hdl == 0)\n");
	}
	if (proc_ctx_hdl)
		__ipa3_release_hdr_proc_ctx(proc_ctx_hdl);

	if (!rule_hdl) {
		IPA_BE_ERR("ipa_be_delete_qos_route_rule: no matching entry found\n");
		return -ENOENT;
	}
	return 0;
}

/**
 * ipa_be_flush_qos_route_rules - Delete all currently-installed QoS route
 * rules from HW (IPA_QOS_PARAM_FLUSH_EVENT path).
 *
 * Iterates qos_config_list and removes the HW route rule for every entry
 * that has a non-zero qos_rule_hdl.  Must be called BEFORE the cache list
 * is freed so that all handles are still accessible.
 */
void ipa_be_flush_qos_route_rules(void)
{
	struct ipa3_qos_config_node *node;
	struct qos_del_entry *entries;
	int n = 0, max = 0, i;

	if (!ipa3_ctx)
		return;

	mutex_lock(&ipa3_ctx->msg_qos_param_lock);
	list_for_each_entry(node, &ipa3_ctx->qos_config_list, list) {
		if (node->config.qos_rule_hdl)
			max++;
	}
	mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

	if (!max) {
		/* Still clear the retime list defensively even with no cache rules. */
		ipa_be_qos_retime_on_flush();
		return;
	}

	entries = kcalloc(max, sizeof(*entries), GFP_KERNEL);
	if (!entries) {
		IPA_BE_ERR("Failed to allocate del entries, using batch fallback\n");
		/* Batch fallback: collect MAX_QOS_DEL_BATCH entries per pass,
		 * release lock once per batch instead of once per entry. */
		while (true) {
			struct qos_del_entry batch[MAX_QOS_DEL_BATCH];
			int batch_n = 0, bi;

			mutex_lock(&ipa3_ctx->msg_qos_param_lock);
			list_for_each_entry(node, &ipa3_ctx->qos_config_list, list) {
				if (!node->config.qos_rule_hdl)
					continue;
				if (batch_n >= MAX_QOS_DEL_BATCH)
					break;
				batch[batch_n].rule_hdl     = node->config.qos_rule_hdl;
				batch[batch_n].proc_ctx_hdl = node->config.proc_ctx_hdl;
				batch[batch_n].ip_type      = node->config.ip_type;
				node->config.qos_rule_hdl   = 0;
				node->config.proc_ctx_hdl   = 0;
				batch_n++;
			}
			mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

			if (!batch_n)
				break;

			for (bi = 0; bi < batch_n; bi++) {
				ipa_delete_route_rule(false, batch[bi].rule_hdl,
						      (enum ipa_ip_type)batch[bi].ip_type);
				if (batch[bi].proc_ctx_hdl)
					__ipa3_release_hdr_proc_ctx(batch[bi].proc_ctx_hdl);
			}
		}
		ipa_be_qos_retime_on_flush();
		return;
	}

	mutex_lock(&ipa3_ctx->msg_qos_param_lock);
	list_for_each_entry(node, &ipa3_ctx->qos_config_list, list) {
		if (!node->config.qos_rule_hdl)
			continue;
		if (n < max) {
			entries[n].rule_hdl = node->config.qos_rule_hdl;
			entries[n].proc_ctx_hdl = node->config.proc_ctx_hdl;
			entries[n].ip_type = node->config.ip_type;
			n++;
		}
		node->config.qos_rule_hdl = 0;
		node->config.proc_ctx_hdl = 0;
	}
	mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

	for (i = 0; i < n; i++) {
		IPA_BE_DBG("Flushing QoS route rule hdl %u\n", entries[i].rule_hdl);
		ipa_delete_route_rule(false, entries[i].rule_hdl,
				      (enum ipa_ip_type)entries[i].ip_type);
		if (entries[i].proc_ctx_hdl)
			__ipa3_release_hdr_proc_ctx(entries[i].proc_ctx_hdl);
	}
	kfree(entries);
	ipa_be_qos_retime_on_flush();
}

/**
 * ipa_be_cleanup_tc_bitmap_rules - remove QoS route rules for traffic
 * classes that are no longer present in a pipe's tc_bmap.
 *
 * Called after tc_bmap is updated to ensure that any installed QoS routing
 * rules for removed traffic classes are deleted from HW and their handles
 * cleared in the cache.
 *
 * @dev_name:    interface name
 * @ip:          IP type of the TX property being updated
 * @removed_tcs: bitmask of TCs that were in the old bitmap but not the new
 */
void ipa_be_cleanup_tc_bitmap_rules(const char *dev_name,
				    enum ipa_ip_type ip,
				    uint32_t removed_tcs)
{
	struct ipa3_qos_config_node *node;
	struct qos_del_entry entries[MAX_QOS_DEL_BATCH];
	int n = 0, i;
	size_t dev_name_len;

	if (!removed_tcs || !dev_name || !ipa3_ctx)
		return;

	dev_name_len = strnlen(dev_name, IPA_RESOURCE_NAME_MAX);
	if (dev_name_len == 0 || dev_name_len >= IPA_RESOURCE_NAME_MAX)
		return;

	/* Walk the full list — we cannot use a remaining_tcs early-exit because
	 * a single TC bit can have multiple matching nodes (e.g. v4 + v6) and
	 * an entry may be skipped due to MAX_QOS_DEL_BATCH overflow. Either
	 * scenario would cause a premature break to silently drop rules. The
	 * list size is bounded by the QoS rule cap so the cost is small. */
	mutex_lock(&ipa3_ctx->msg_qos_param_lock);
	list_for_each_entry(node, &ipa3_ctx->qos_config_list, list) {
		if (node->config.ip_type != (uint8_t)ip)
			continue;
		if (strncmp(node->config.dev_name, dev_name,
			    sizeof(node->config.dev_name)))
			continue;
		if (!(removed_tcs & (1U << node->config.traffic_class)))
			continue;
		if (!node->config.qos_rule_hdl)
			continue;

		if (n < MAX_QOS_DEL_BATCH) {
			entries[n].rule_hdl     = node->config.qos_rule_hdl;
			entries[n].proc_ctx_hdl = node->config.proc_ctx_hdl;
			entries[n].ip_type      = node->config.ip_type;
			n++;
			node->config.qos_rule_hdl  = 0;
			node->config.proc_ctx_hdl  = 0;
		} else {
			IPA_BE_ERR("Too many QoS rules to delete in one pass (max %d) — remaining will be retried on next tc_bitmap update\n",
				   MAX_QOS_DEL_BATCH);
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

	for (i = 0; i < n; i++) {
		IPA_BE_DBG("Removing QoS rule hdl %u (TC removed from bitmap)\n",
			   entries[i].rule_hdl);
		ipa_delete_route_rule(false, entries[i].rule_hdl,
				      (enum ipa_ip_type)entries[i].ip_type);
		if (entries[i].proc_ctx_hdl)
			__ipa3_release_hdr_proc_ctx(entries[i].proc_ctx_hdl);
	}
}

/**
 * ipa_be_install_tc_bitmap_rules - install QoS route rules for traffic
 * classes newly added to a pipe's tc_bmap.
 *
 * Called after tc_bmap is updated to opportunistically install routing rules
 * for cached QoS configs whose TC is now covered by the pipe.  Rules that
 * could not be installed when the QoS config was first added (because no
 * pipe had the TC bit set) are retried here.
 *
 * @dev_name:  interface name
 * @ip:        IP type of the TX property being updated
 * @added_tcs: bitmask of TCs newly present in the updated bitmap
 */
int ipa_be_install_tc_bitmap_rules(const char *dev_name,
				   enum ipa_ip_type ip,
				   uint32_t added_tcs)
{
	struct ipa3_qos_config_node *node;
	struct tc_install_task {
		struct ipa_ioc_qos_config    config;
		struct ipa3_qos_config_node *orig;
	} *tasks;
	int n = 0, i, failed_count = 0, task_count = 0;
	size_t dev_name_len;

	if (!added_tcs || !dev_name || !ipa3_ctx)
		return -EINVAL;

	dev_name_len = strnlen(dev_name, IPA_RESOURCE_NAME_MAX);
	if (dev_name_len == 0 || dev_name_len >= IPA_RESOURCE_NAME_MAX)
		return -EINVAL;

	/* First pass: count matching rules to allocate exactly what's needed. */
	mutex_lock(&ipa3_ctx->msg_qos_param_lock);
	list_for_each_entry(node, &ipa3_ctx->qos_config_list, list) {
		if (node->config.ip_type != (uint8_t)ip)
			continue;
		if (strncmp(node->config.dev_name, dev_name,
			    sizeof(node->config.dev_name)))
			continue;
		if (!(added_tcs & (1U << node->config.traffic_class)))
			continue;
		if (node->config.qos_rule_hdl != 0)
			continue;
		task_count++;
	}
	mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

	if (!task_count)
		return 0;

	tasks = kcalloc(task_count, sizeof(*tasks), GFP_KERNEL);
	if (!tasks) {
		IPA_BE_ERR("Failed to allocate %d install tasks\n", task_count);
		return -ENOMEM;
	}

	/* Second pass: collect tasks. */
	mutex_lock(&ipa3_ctx->msg_qos_param_lock);
	list_for_each_entry(node, &ipa3_ctx->qos_config_list, list) {
		if (node->config.ip_type != (uint8_t)ip)
			continue;
		if (strncmp(node->config.dev_name, dev_name,
			    sizeof(node->config.dev_name)))
			continue;
		if (!(added_tcs & (1U << node->config.traffic_class)))
			continue;
		if (node->config.qos_rule_hdl != 0)
			continue; /* already installed */
		if (n >= task_count) {
			IPA_BE_ERR("tc_bitmap install: more rules added since count pass — %d extra deferred to next update\n",
				   n - task_count + 1);
			break;
		}
		tasks[n].config = node->config;
		tasks[n].orig   = node;
		n++;
	}
	mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

	for (i = 0; i < n; i++) {
		int ret = ipa_be_install_qos_route_rule(&tasks[i].config);

		if (!ret && tasks[i].config.qos_rule_hdl) {
			struct ipa_qos_rule_installed_msg *evt;
			struct ipa_msg_meta meta = {};

			mutex_lock(&ipa3_ctx->msg_qos_param_lock);
			tasks[i].orig->config.qos_rule_hdl =
				tasks[i].config.qos_rule_hdl;
			tasks[i].orig->config.proc_ctx_hdl =
				tasks[i].config.proc_ctx_hdl;
			mutex_unlock(&ipa3_ctx->msg_qos_param_lock);
			IPA_BE_DBG("Installed QoS rule hdl %u for TC %u "
				   "(new bitmap)\n",
				   tasks[i].config.qos_rule_hdl,
				   tasks[i].config.traffic_class);

			/* Notify userspace that a previously-deferred rule is
			 * now in HW (TC bitmap re-add path). Mirrors the
			 * INSTALLED-event firing in
			 * ipa_be_check_and_install_qos_for_new_rule(). */
			if (!tasks[i].orig->qos_id)
				continue;
			evt = kzalloc(sizeof(*evt), GFP_KERNEL);
			if (!evt)
				continue;
			evt->qos_id        = tasks[i].orig->qos_id;
			evt->qos_rule_hdl  = tasks[i].config.qos_rule_hdl;
			evt->traffic_class = tasks[i].config.traffic_class;
			meta.msg_type = IPA_QOS_RULE_INSTALLED_EVENT;
			meta.msg_len  = sizeof(*evt);
			IPA_BE_DBG("Sending IPA_QOS_RULE_INSTALLED_EVENT qos_id=%u hdl=%u tc=%u (tc_bitmap re-add)\n",
				   evt->qos_id, evt->qos_rule_hdl,
				   evt->traffic_class);
			if (ipa_send_msg(&meta, evt,
					 ipa_qos_rule_installed_evt_free))
				kfree(evt);
		} else {
			failed_count++;
			IPA_BE_ERR("Failed to install QoS rule for TC %u (ret=%d)\n",
				   tasks[i].config.traffic_class, ret);
		}
	}
	if (failed_count)
		IPA_BE_ERR("Installed %d/%d tc_bitmap rules, %d failed\n",
			   n - failed_count, n, failed_count);
	/* Note: this function is best-effort — successfully installed rules
	 * are NOT rolled back on partial failure.  The caller (tc_bitmap update)
	 * has already committed the new bitmap; any failed rules will be retried
	 * the next time the TC bitmap is updated for this interface. */
	kfree(tasks);
	return failed_count ? -EAGAIN : 0;
}

/**
 * rules for IPA
 * @v4_msg: IPv4 rule creation message containing connection
 *  	 details and interface information
 *
 * Return: IPA_CMN_RESPONSE_ACK on success, error code on failure
 */
static int ipa_ipv4_create_rule(struct ipa_ipv4_rule_create_msg v4_msg)
{
	int ret = IPA_CMN_RESPONSE_ACK;
	bool lan2lan = false;
	int pdn_iface = 0;
	int client_iface = 0;
	int vlan_tag = 0;
	uint32_t mtu_size = 0;
	ip_addr_t lan_client_ip = {0};
	ip_addr_t ret_ip_key = {0};
	ip_addr_t flow_ip_key = {0};
	ip_addr_t wan_ip = {0};
	uint32_t flow_interface_num = 0;
	mac_addr_t mac, wan_mac;
	enum ipa_backhaul_type backhaul_type = IPA_BACKHAUL_TYPE_UNKNOWN;
	struct ipa_clientdb_mapping_instance *mapping = NULL;
	bool lan_route_added = false;
	bool wan_route_added = false;
	bool wan_catchall_acquired = false;
	bool ret_route_added = false;
	bool ret_filter_added = false;
	bool flow_route_added = false;
	bool flow_filter_added = false;
	bool dl_first_install = false;

	ipa_be_log_ipv4_rule_details(v4_msg);


	/* Check if the traffic is lan2lan or lan2wan */
	lan2lan = is_lan_to_lan_connection(IPA_IP_v4, &v4_msg);
	if (lan2lan) {
		IPA_BE_DBG("No xlated ip hence lan2lan: %d\n", lan2lan);
	}

	IPA_BE_DBG("is connection lan2lan: %d\n", lan2lan);

	/* Check if the destination connection already exists in Database.
	Add a new entry if it already not exists */
	int is_ret = false;
	int step = 0;
	if (lan2lan)
	{
		enum ipa_hw_type ipa_ver = ipa_get_hw_type();

		ret_ip_key[0] = v4_msg.tuple.return_ip;
		mapping = ipa_be_client_mapping_add_or_ref(ret_ip_key, 0, lan2lan, v4_msg.conn_rule.return_mac);
		if (IS_ERR(mapping)) {
			ret = IPA_CMN_RESPONSE_EMSG;
			goto failed_ret;
		}
		step = L2L_STEP_RET_MAPPING;
		is_ret = true;
		if (mapping != NULL)
		{
			/* Add route entry (route stays gated on the IP mapping:
			 * one route/proc_ctx/hdr per host is by design) */
			ret = ipa_ipv4_add_route_rule(v4_msg, lan2lan, v4_msg.conn_rule.return_interface_num, v4_msg.conn_rule.return_mac, is_ret);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add route rule for return IP\n");
				goto failed_ret;
			}
			step = L2L_STEP_RET_ROUTE;
			ret_route_added = true;
		}

		/* Add destination based filter rule. The filter is keyed
		 * globally by (dst host, ip type) and self-gates via its own
		 * refcount (installing on each ep that lacks the rule), so it
		 * must be attempted regardless of the IP-only mapping ref
		 * above. */
		ret = ipa_be_v4_add_filter_rule(v4_msg, lan2lan, v4_msg.conn_rule.flow_interface_num, v4_msg.conn_rule.return_mac, is_ret);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add filter rule for return IP\n");
			goto failed_ret;
		}
		step = L2L_STEP_RET_FILTER;
		ret_filter_added = true;

		flow_ip_key[0] = v4_msg.tuple.flow_ip;
		mapping = ipa_be_client_mapping_add_or_ref(flow_ip_key, 0, lan2lan, v4_msg.conn_rule.flow_mac);
		if (IS_ERR(mapping)) {
			ret = IPA_CMN_RESPONSE_EMSG;
			goto failed_ret;
		}
		step = L2L_STEP_FLOW_MAPPING;
		is_ret = false;
		if (mapping != NULL)
		{
			/* Add route entry (route stays gated on the IP mapping) */
			ret = ipa_ipv4_add_route_rule(v4_msg, lan2lan, v4_msg.conn_rule.flow_interface_num, v4_msg.conn_rule.flow_mac, is_ret);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add route rule for flow IP\n");
				goto failed_ret;
			}
			step = L2L_STEP_FLOW_ROUTE;
			flow_route_added = true;
		}

		/* Add destination based filter rule (self-gated, see above) */
		ret = ipa_be_v4_add_filter_rule(v4_msg, lan2lan, v4_msg.conn_rule.return_interface_num, v4_msg.conn_rule.flow_mac, is_ret);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add filter rule for flow IP\n");
			goto failed_ret;
		}
		step = L2L_STEP_FLOW_FILTER;
		flow_filter_added = true;

		#ifdef CONFIG_ECM_CONVERGENCE
			/* Add CT entry*/
			if (ipa_ver >= IPA_HW_v7_0) {
				pdn_iface = v4_msg.conn_rule.flow_interface_num;
				ret = ipa_be_add_ct_entry_v4(&v4_msg);
				if (ret != 0) {
					IPA_BE_ERR("Failed to add CT entry\n");
					goto failed_ret;
				}
			}
			else {
				IPA_BE_DBG("CT support disabled - skipping ipa_be_add_ct_entry_v4\n");
			}
		#else
			IPA_BE_DBG("CT support disabled - skipping ipa_be_add_ct_entry_v4\n");
		#endif
	}
	else {
		/* Set direction-specific parameters for lan2wan */
		if (v4_msg.conn_rule.return_interface_num == v4_msg.conn_rule.return_top_interface_num) {
			IPA_BE_DBG("Uplink flow - installing both uplink and downlink rules\n");
			pdn_iface          = v4_msg.conn_rule.return_interface_num;
			client_iface       = v4_msg.conn_rule.flow_interface_num;
			vlan_tag           = v4_msg.vlan_primary_rule.ingress_vlan_tag;
			mtu_size           = v4_msg.conn_rule.flow_mtu;
			lan_client_ip[0]   = v4_msg.tuple.flow_ip;
			wan_ip[0]          = v4_msg.conn_rule.return_ip_xlate;
			flow_interface_num = v4_msg.conn_rule.flow_interface_num;
			memcpy(mac, v4_msg.conn_rule.flow_mac, IPA_MAC_ADDR_SIZE);
			memcpy(wan_mac, v4_msg.conn_rule.return_mac, IPA_MAC_ADDR_SIZE);
		} else if (v4_msg.conn_rule.flow_interface_num == v4_msg.conn_rule.flow_top_interface_num) {
			IPA_BE_DBG("Downlink flow - installing both downlink and uplink rules\n");
			pdn_iface          = v4_msg.conn_rule.flow_interface_num;
			client_iface       = v4_msg.conn_rule.return_interface_num;
			vlan_tag           = v4_msg.vlan_primary_rule.egress_vlan_tag;
			mtu_size           = v4_msg.conn_rule.return_mtu;
			lan_client_ip[0]   = v4_msg.conn_rule.return_ip_xlate;
			wan_ip[0]          = v4_msg.tuple.flow_ip;
			flow_interface_num = v4_msg.conn_rule.return_interface_num;
			memcpy(mac, v4_msg.conn_rule.return_mac, IPA_MAC_ADDR_SIZE);
			memcpy(wan_mac, v4_msg.conn_rule.flow_mac, IPA_MAC_ADDR_SIZE);
			is_ret = true;
		} else {
			IPA_BE_ERR("Unexpected param: return_if=%d return_top_if=%d flow_if=%d flow_top_if=%d\n",
				v4_msg.conn_rule.return_interface_num, v4_msg.conn_rule.return_top_interface_num,
				v4_msg.conn_rule.flow_interface_num, v4_msg.conn_rule.flow_top_interface_num);
			goto failed_ret;
		}

		IPA_BE_DBG("v4: assigning qmapmux intf_idx for pdn_iface=%d\n", pdn_iface);
		ipa3_assign_qmapmux_intf_idx(pdn_iface);

		/* Detect backhaul type */
		ret = ipa_be_detect_backhaul_type(pdn_iface, &backhaul_type);
		if (ret < 0) {
			IPA_BE_ERR("Failed to detect backhaul for iface %d (%d)\n", pdn_iface, ret);
			ret = IPA_CMN_RESPONSE_EMSG;
			goto failed_ret;
		}
		IPA_BE_DBG("Backhaul type: %s\n",
			backhaul_type == IPA_BACKHAUL_TYPE_MODEM ? "MODEM" : "ETH");

		/* Install uplink filter rules */
		if (backhaul_type == IPA_BACKHAUL_TYPE_ETH) {
			/* Add route entry for the lan client */
			mapping = ipa_be_client_mapping_add_or_ref(lan_client_ip, 0, lan2lan, mac);
			if (IS_ERR(mapping)) {
				ret = IPA_CMN_RESPONSE_EMSG;
				goto failed_ret;
			}
			step = ETH_STEP_LAN_MAPPING_ROUTE;
			if (mapping != NULL) {
				ret = ipa_ipv4_add_route_rule(v4_msg, lan2lan, client_iface, mac, is_ret);
				if (ret != 0) {
					IPA_BE_ERR("Failed to add route rule, rolling back client mapping\n");
					ret = IPA_CMN_RESPONSE_EMSG;
					goto failed_ret;
				}
				lan_route_added = true;
			}

			/* install downlink filter rules on the backhaul pipe */
			ret = ipa_be_handle_wan_up(pdn_iface, client_iface, IPA_IP_v4, true,
				&v4_msg.vlan_primary_rule.egress_vlan_tag);
			if (ret != 0) {
				IPA_BE_ERR("Failed to handle wan up (downlink) for IPv4\n");
				goto failed_ret;
			}
			step = ETH_STEP_WAN_UP_DOWNLINK;

			/* Add route entry for the wan client */
			mapping = ipa_be_client_mapping_add_or_ref(wan_ip, 0, lan2lan, wan_mac);
			if (IS_ERR(mapping)) {
				ret = IPA_CMN_RESPONSE_EMSG;
				goto failed_ret;
			}
			step = ETH_STEP_WAN_MAPPING;
			if (mapping != NULL)
			{
				ret = ipa_ipv4_eth_backhaul_add_route_rule(v4_msg, lan2lan, pdn_iface, wan_mac, is_ret, true);
				if (ret != 0) {
					IPA_BE_ERR("Failed to add IPv4 eth backhaul route rule (is_catch_all=true)\n");
					goto failed_ret;
				}
				step = ETH_STEP_BACKHAUL_RULE_TRUE;
				wan_catchall_acquired = true;

				ret = ipa_ipv4_eth_backhaul_add_route_rule(v4_msg, lan2lan, pdn_iface, wan_mac, is_ret, false);
				if (ret != 0) {
					IPA_BE_ERR("Failed to add IPv4 eth backhaul route rule (is_catch_all=false)\n");
					goto failed_ret;
				}
				step = ETH_STEP_BACKHAUL_RULE_FALSE;
				wan_route_added = true;
			}

			/* For eth backhaul, set up default WAN filtering rules */
			ret = ipa_be_handle_wan_up(client_iface, pdn_iface, IPA_IP_v4, false, wan_ip);
			if (ret != 0) {
				IPA_BE_ERR("Failed to handle wan up (uplink) for IPv4\n");
				goto failed_ret;
			}
			step = ETH_STEP_WAN_UP_UPLINK;

#ifdef CONFIG_ECM_CONVERGENCE
			ret = ipa_be_addpdn(v4_msg, pdn_iface);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add PDN entry (eth backhaul)\n");
				goto failed_ret;
			}
			step = ETH_STEP_NAT_ENTRY;
#else
			IPA_BE_DBG("NAT support disabled - skipping ipa_be_addpdn (eth backhaul)\n");
#endif
		}
		else {
		/* Add route entry for the client */
		mapping = ipa_be_client_mapping_add_or_ref(lan_client_ip, 0, lan2lan, mac);
		if (IS_ERR(mapping)) {
			ret = IPA_CMN_RESPONSE_EMSG;
			goto failed_ret;
		}
		step = V4_MDM_STEP_CLIENT_MAPPING;
		if (mapping != NULL) {
			ret = ipa_ipv4_add_route_rule(v4_msg, lan2lan, flow_interface_num, mac, is_ret);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add route rule\n");
				goto failed_ret;
			}
			step = V4_MDM_STEP_ROUTE_RULE;
			lan_route_added = true;
		}

		/* Install uplink filter rules */
		ret = ipa_be_v4_add_uplink_filter_rule(v4_msg, lan2lan, pdn_iface, client_iface);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add uplink filter rule\n");
			goto failed_ret;
		}
		step = V4_MDM_STEP_UPLINK_FILTER;

		ret = ipa_be_construct_mtu_rule(IPA_IP_v4, mtu_size, client_iface, pdn_iface, vlan_tag);
		if (ret != 0) {
			IPA_BE_ERR("Failed to construct mtu rule\n");
			goto failed_ret;
		}
		step = V4_MDM_STEP_MTU_RULE;

		/* Install downlink rules */
		dl_first_install = !ipa_be_pdn_filter_installed(pdn_iface, IPA_IP_v4);

		ret = add_dft_filtering_rule(pdn_iface, IPA_IP_v4);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add dft filtering rule\n");
			goto failed_ret;
		}
		step = V4_MDM_STEP_DFT_FILTER;

		ret = add_icmp_alg_rules(pdn_iface, IPA_IP_v4);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add ICMP/ALG filtering rule\n");
			goto failed_ret;
		}
		step = V4_MDM_STEP_ICMP_ALG;

		ret = add_catchup_all_filtering_rule_each_pdn(pdn_iface, IPA_IP_v4);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add catchup all filtering rule\n");
			goto failed_ret;
		}
		step = V4_MDM_STEP_CATCHUP_ALL;

		if (dl_first_install) {
			ret = install_wan_filtering_rule(false);
			if (ret != 0) {
				IPA_BE_ERR("Failed to install wan filtering rule\n");
				goto failed_ret;
			}
			step = V4_MDM_STEP_WAN_FILTER;
		} else {
			IPA_BE_DBG("DL rules already installed for pdn_iface=%d IPv4; skip Q6 push\n",
				pdn_iface);
			step = V4_MDM_STEP_WAN_FILTER;
		}

#ifdef CONFIG_ECM_CONVERGENCE
		/* Match against lan_client_ip (the client's real IP), not v4_msg.tuple.flow_ip (the WAN-side address for a DL flow). */
		ipa_be_check_and_install_qos_for_new_rule(
			v4_msg.conn_rule.flow_mac,
			v4_msg.vlan_primary_rule.ingress_vlan_tag,
			v4_msg.conn_rule.return_mac,
			v4_msg.vlan_primary_rule.egress_vlan_tag,
			lan_client_ip[0],
			NULL);

		/* Add NAT entry */
		ret = ipa_be_addpdn(v4_msg, pdn_iface);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add PDN entry\n");
			goto failed_ret;
		}
		step = V4_MDM_STEP_NAT_ENTRY;
#else
		IPA_BE_DBG("NAT support disabled - skipping ipa_be_addpdn\n");
#endif
		}
	}
	struct ipa_fse_rule fse_info;
	if (ret == IPA_CMN_RESPONSE_ACK) {
		/* FSE rule creation */
		if (ipa3_is_vpnum_valid(v4_msg.conn_rule.flow_interface_num)) {
			memset(&fse_info, 0, sizeof(fse_info));
			ipa_be_fill_ipv4_fse_info(&fse_info, &v4_msg.tuple,
						  &v4_msg.conn_rule);
			if (ipa_be_fse_rule_create(&fse_info)) {
				IPA_BE_ERR("Failed to create FSE rule for IPv4 flow\n");
			}
		}

		if (ipa3_is_vpnum_valid(v4_msg.conn_rule.return_interface_num)) {
			memset(&fse_info, 0, sizeof(fse_info));
			ipa_be_fill_ipv4_fse_info_reverse(&fse_info, &v4_msg.tuple,
							  &v4_msg.conn_rule);
			if (ipa_be_fse_rule_create(&fse_info)) {
				IPA_BE_ERR(
					"Failed to create FSE reverse rule for IPv4 flow\n");
			}
		}
	}
	IPA_BE_DBG("Command  return %d \n", ret);
	return ret;
failed_ret:
	ret = IPA_CMN_RESPONSE_EMSG;
	if (lan2lan) {
		switch (step) {
			case L2L_STEP_FLOW_FILTER:
				if (flow_filter_added)
					ipa_be_v4_delete_filter_rule(*((struct ipa_ipv4_rule_destroy_msg *)&v4_msg), v4_msg.conn_rule.return_interface_num, v4_msg.conn_rule.flow_mac, lan2lan);
				/* fallthrough */
			case L2L_STEP_FLOW_ROUTE:
				if (flow_route_added)
					ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping(flow_ip_key, lan2lan, NULL, NULL, NULL), IPA_IP_v4);
				/* fallthrough */
			case L2L_STEP_FLOW_MAPPING:
				ipa_be_mapping_deref_and_delete(flow_ip_key, lan2lan, NULL);
				/* fallthrough */
			case L2L_STEP_RET_FILTER:
				if (ret_filter_added)
					ipa_be_v4_delete_filter_rule(*((struct ipa_ipv4_rule_destroy_msg *)&v4_msg), v4_msg.conn_rule.flow_interface_num, v4_msg.conn_rule.return_mac, lan2lan);
				/* fallthrough */
			case L2L_STEP_RET_ROUTE:
				if (ret_route_added)
					ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping(ret_ip_key, lan2lan, NULL, NULL, NULL), IPA_IP_v4);
				/* fallthrough */
			case L2L_STEP_RET_MAPPING:
				ipa_be_mapping_deref_and_delete(ret_ip_key, lan2lan, NULL);
		}
	} else if (backhaul_type == IPA_BACKHAUL_TYPE_ETH) {
		switch (step) {
			case ETH_STEP_NAT_ENTRY:
				/* no rollback available for NAT entry */
				/* fallthrough */
			case ETH_STEP_WAN_UP_UPLINK:
				ipa_be_handle_wan_down(client_iface, pdn_iface, IPA_IP_v4);
				/* fallthrough */
			case ETH_STEP_BACKHAUL_RULE_FALSE:
				if (wan_route_added)
					ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping(wan_ip, lan2lan, NULL, NULL, NULL), IPA_IP_v4);
				/* fallthrough */
			case ETH_STEP_BACKHAUL_RULE_TRUE:
				if (wan_catchall_acquired)
					ipa_be_wan_catchall_release(pdn_iface, IPA_IP_v4);
				/* fallthrough */
			case ETH_STEP_WAN_MAPPING:
				ipa_be_mapping_deref_and_delete(wan_ip, lan2lan, NULL);
				/* fallthrough */
			case ETH_STEP_WAN_UP_DOWNLINK:
				ipa_be_handle_wan_down(pdn_iface, client_iface, IPA_IP_v4);
				/* fallthrough */
			case ETH_STEP_LAN_MAPPING_ROUTE:
				if (lan_route_added)
					ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping(lan_client_ip, lan2lan, NULL, NULL, NULL), IPA_IP_v4);
				ipa_be_mapping_deref_and_delete(lan_client_ip, lan2lan, NULL);
		}
	} else {
		switch (step) {
			case V4_MDM_STEP_NAT_ENTRY:
				/* no rollback available for NAT entry */
				/* fallthrough */
			case V4_MDM_STEP_WAN_FILTER:
				/* Need a way to rollback wan_filtering_rule if possible */
				/* fallthrough */
			case V4_MDM_STEP_CATCHUP_ALL:
				delete_catchup_all_filtering_rule_each_pdn(pdn_iface, IPA_IP_v4);
				/* fallthrough */
			case V4_MDM_STEP_ICMP_ALG:
				delete_icmp_alg_rules(pdn_iface, IPA_IP_v4);
				/* fallthrough */
			case V4_MDM_STEP_DFT_FILTER:
				delete_dft_filtering_rule(pdn_iface, IPA_IP_v4);
				/* fallthrough */
			case V4_MDM_STEP_MTU_RULE:
				ipa_be_delete_mtu_rule(client_iface, pdn_iface, IPA_IP_v4);
				/* fallthrough */
			case V4_MDM_STEP_UPLINK_FILTER:
				ipa_be_v4_delete_uplink_filter_rule(*((struct ipa_ipv4_rule_destroy_msg *)&v4_msg), pdn_iface, client_iface);
				/* fallthrough */
			case V4_MDM_STEP_ROUTE_RULE:
				if (lan_route_added)
					ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping(lan_client_ip, lan2lan, NULL, NULL, NULL), IPA_IP_v4);
				/* fallthrough */
			case V4_MDM_STEP_CLIENT_MAPPING:
				ipa_be_mapping_deref_and_delete(lan_client_ip, lan2lan, NULL);
		}
	}
	IPA_BE_DBG("Command  return %d \n", ret);
	return ret;
}

/**
 * ipa_ipv6_create_rule - Creates IPv6 routing and filtering rules for IPA
 * @v6_msg: IPv6 rule creation message containing connection details and interface information
 *
 * Return: IPA_CMN_RESPONSE_ACK on success, error code on failure
 */
static int ipa_ipv6_create_rule(struct ipa_ipv6_rule_create_msg v6_msg)
{
	int ret = IPA_CMN_RESPONSE_ACK;
	bool lan2lan = false;
	int pdn_iface = 0;
	int client_iface = 0;
	int vlan_tag = 0;
	uint32_t mtu_size = 0;
	uint32_t *flow_ip_ptr = NULL;
	uint32_t flow_interface_num = 0;
	ip_addr_t ret_ip6_key = {0};
	ip_addr_t flow_ip6_key = {0};
	mac_addr_t mac, wan_mac;
	uint32_t *wan_ip_ptr = NULL;
	enum ipa_backhaul_type backhaul_type = IPA_BACKHAUL_TYPE_UNKNOWN;
	struct ipa_clientdb_mapping_instance *mapping = NULL;
	bool lan_route_added = false;
	bool wan_route_added = false;
	bool wan_catchall_acquired = false;
	bool ret_route_added = false;
	bool ret_filter_added = false;
	bool flow_route_added = false;
	bool flow_filter_added = false;
	bool dl_first_install = false;

	ipa_be_log_ipv6_rule_details(v6_msg);

	/* Check if the traffic is lan2lan or lan2wan */
	lan2lan = is_lan_to_lan_connection(IPA_IP_v6, &v6_msg);
	if (lan2lan) {
		IPA_BE_DBG("No xlated ip hence lan2lan: %d\n", lan2lan);
	}

	IPA_BE_DBG("is connection lan2lan: %d\n", lan2lan);

	/* Check if the destination connection already exists in Database.
	Add a new entry if it already not exists */
	int is_ret = false;
	int step = 0;
	if (lan2lan)
	{
		memcpy(ret_ip6_key, &v6_msg.tuple.return_ip, sizeof(ret_ip6_key));
		mapping = ipa_be_client_mapping_add_or_ref(ret_ip6_key, 0, lan2lan, v6_msg.conn_rule.return_mac);
		if (IS_ERR(mapping)) {
			ret = IPA_CMN_RESPONSE_EMSG;
			goto failed_ret;
		}
		step = L2L_STEP_RET_MAPPING;
		is_ret = true;
		if (mapping != NULL)
		{
			/* Add route entry (route stays gated on the IP mapping:
			 * one route/proc_ctx/hdr per host is by design) */
			ret = ipa_ipv6_add_route_rule(v6_msg, lan2lan, v6_msg.conn_rule.return_interface_num, v6_msg.conn_rule.return_mac, is_ret);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add route rule for return IP\n");
				goto failed_ret;
			}
			step = L2L_STEP_RET_ROUTE;
			ret_route_added = true;
		}

		/* Add destination based filter rule. The filter is keyed
		 * globally by (dst host, ip type) and self-gates via its own
		 * refcount (installing on each ep that lacks the rule), so it
		 * must be attempted regardless of the IP-only mapping ref
		 * above. */
		ret = ipa_be_v6_add_filter_rule(v6_msg, lan2lan, v6_msg.conn_rule.flow_interface_num, v6_msg.conn_rule.return_mac, is_ret);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add filter rule for return IP\n");
			goto failed_ret;
		}
		step = L2L_STEP_RET_FILTER;
		ret_filter_added = true;

		memcpy(flow_ip6_key, &v6_msg.tuple.flow_ip, sizeof(flow_ip6_key));
		mapping = ipa_be_client_mapping_add_or_ref(flow_ip6_key, 0, lan2lan, v6_msg.conn_rule.flow_mac);
		if (IS_ERR(mapping)) {
			ret = IPA_CMN_RESPONSE_EMSG;
			goto failed_ret;
		}
		step = L2L_STEP_FLOW_MAPPING;
		is_ret = false;
		if (mapping != NULL)
		{
			/* Add route entry (route stays gated on the IP mapping) */
			ret = ipa_ipv6_add_route_rule(v6_msg, lan2lan, v6_msg.conn_rule.flow_interface_num, v6_msg.conn_rule.flow_mac, is_ret);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add route rule for flow IP\n");
				goto failed_ret;
			}
			step = L2L_STEP_FLOW_ROUTE;
			flow_route_added = true;
		}

		/* Add destination based filter rule (self-gated, see above) */
		ret = ipa_be_v6_add_filter_rule(v6_msg, lan2lan, v6_msg.conn_rule.return_interface_num, v6_msg.conn_rule.flow_mac, is_ret);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add filter rule for flow IP\n");
			goto failed_ret;
		}

		step = L2L_STEP_FLOW_FILTER;
		flow_filter_added = true;
#ifdef CONFIG_ECM_CONVERGENCE
		pdn_iface = v6_msg.conn_rule.flow_interface_num;
		ret = ipa_be_add_v6_ct_entry(v6_msg, pdn_iface, lan2lan);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add IPv6 CT entry\n");
			goto failed_ret;
		}
#else
		IPA_BE_DBG("IPv6 CT LAN2LAN disabled - CONFIG_ECM_CONVERGENCE not set\n");
#endif
	}
	else {
		/* Set direction-specific parameters for lan2wan */
		if (v6_msg.conn_rule.return_interface_num == v6_msg.conn_rule.return_top_interface_num) {
			IPA_BE_DBG("Uplink flow - installing both uplink and downlink rules\n");
			pdn_iface          = v6_msg.conn_rule.return_interface_num;
			client_iface       = v6_msg.conn_rule.flow_interface_num;
			vlan_tag           = v6_msg.vlan_primary_rule.ingress_vlan_tag;
			mtu_size           = v6_msg.conn_rule.flow_mtu;
			flow_ip_ptr        = (uint32_t *)&v6_msg.tuple.flow_ip;
			wan_ip_ptr         = (uint32_t *)&v6_msg.tuple.return_ip;
			flow_interface_num = v6_msg.conn_rule.flow_interface_num;
			memcpy(mac, v6_msg.conn_rule.flow_mac, IPA_MAC_ADDR_SIZE);
			memcpy(wan_mac, v6_msg.conn_rule.return_mac, IPA_MAC_ADDR_SIZE);
		} else if (v6_msg.conn_rule.flow_interface_num == v6_msg.conn_rule.flow_top_interface_num) {
			IPA_BE_DBG("Downlink flow - installing both downlink and uplink rules\n");
			pdn_iface          = v6_msg.conn_rule.flow_interface_num;
			client_iface       = v6_msg.conn_rule.return_interface_num;
			vlan_tag           = v6_msg.vlan_primary_rule.egress_vlan_tag;
			mtu_size           = v6_msg.conn_rule.return_mtu;
			flow_ip_ptr        = (uint32_t *)&v6_msg.tuple.return_ip;
			wan_ip_ptr         = (uint32_t *)&v6_msg.tuple.flow_ip;
			flow_interface_num = v6_msg.conn_rule.return_interface_num;
			memcpy(mac, v6_msg.conn_rule.return_mac, IPA_MAC_ADDR_SIZE);
			memcpy(wan_mac, v6_msg.conn_rule.flow_mac, IPA_MAC_ADDR_SIZE);
			is_ret = true;
		} else {
			IPA_BE_ERR("Unexpected param: return_if=%d return_top_if=%d flow_if=%d flow_top_if=%d\n",
				v6_msg.conn_rule.return_interface_num, v6_msg.conn_rule.return_top_interface_num,
				v6_msg.conn_rule.flow_interface_num, v6_msg.conn_rule.flow_top_interface_num);
			goto failed_ret;
		}

		IPA_BE_DBG("v6: assigning qmapmux intf_idx for pdn_iface=%d\n", pdn_iface);
		ipa3_assign_qmapmux_intf_idx(pdn_iface);

		/* Detect backhaul type */
		ret = ipa_be_detect_backhaul_type(pdn_iface, &backhaul_type);
		if (ret < 0) {
			IPA_BE_ERR("Failed to detect backhaul for iface %d (%d)\n", pdn_iface, ret);
			ret = IPA_CMN_RESPONSE_EMSG;
			goto failed_ret;
		}
		IPA_BE_DBG("Backhaul type: %s\n",
			backhaul_type == IPA_BACKHAUL_TYPE_MODEM ? "MODEM" : "ETH");

		/* Install uplink filter rules */
		if (backhaul_type == IPA_BACKHAUL_TYPE_ETH) {
			/* Add route entry for the lan client */
			mapping = ipa_be_client_mapping_add_or_ref(flow_ip_ptr, 0, lan2lan, mac);
			if (IS_ERR(mapping)) {
				ret = IPA_CMN_RESPONSE_EMSG;
				goto failed_ret;
			}
			step = ETH_STEP_LAN_MAPPING_ROUTE;
			if (mapping != NULL) {
				ret = ipa_ipv6_add_route_rule(v6_msg, lan2lan, client_iface, mac, is_ret);
				if (ret != 0) {
					IPA_BE_ERR("Failed to add route rule, rolling back client mapping\n");
					ret = IPA_CMN_RESPONSE_EMSG;
					goto failed_ret;
				}
				lan_route_added = true;
			}

			/* install downlink filter rules on the backhaul pipe */
			ret = ipa_be_handle_wan_up(pdn_iface, client_iface, IPA_IP_v6, true,
				&v6_msg.vlan_primary_rule.egress_vlan_tag);
			if (ret != 0) {
				IPA_BE_ERR("Failed to handle wan up (downlink) for IPv6\n");
				goto failed_ret;
			}
			step = ETH_STEP_WAN_UP_DOWNLINK;

			/* Add route entry for the wan client */
			mapping = ipa_be_client_mapping_add_or_ref(wan_ip_ptr, 0, lan2lan, wan_mac);
			if (IS_ERR(mapping)) {
				ret = IPA_CMN_RESPONSE_EMSG;
				goto failed_ret;
			}
			step = ETH_STEP_WAN_MAPPING;
			if (mapping != NULL) {
				ret = ipa_ipv6_eth_backhaul_add_route_rule(v6_msg, lan2lan, pdn_iface, wan_mac, is_ret, true);
				if (ret != 0) {
					IPA_BE_ERR("Failed to add IPv6 eth backhaul route rule (is_catch_all=true)\n");
					goto failed_ret;
				}
				step = ETH_STEP_BACKHAUL_RULE_TRUE;
				wan_catchall_acquired = true;

				ret = ipa_ipv6_eth_backhaul_add_route_rule(v6_msg, lan2lan, pdn_iface, wan_mac, is_ret, false);
				if (ret != 0) {
					IPA_BE_ERR("Failed to add IPv6 eth backhaul route rule (is_catch_all=false)\n");
					goto failed_ret;
				}
				step = ETH_STEP_BACKHAUL_RULE_FALSE;
				wan_route_added = true;
			}

			/* For eth backhaul, set up default WAN filtering rules */
			ret = ipa_be_handle_wan_up(client_iface, pdn_iface, IPA_IP_v6, false, wan_ip_ptr);
			if (ret != 0) {
				IPA_BE_ERR("Failed to handle wan up (uplink) for IPv6\n");
				goto failed_ret;
			}
			step = ETH_STEP_WAN_UP_UPLINK;
		} else {
			/* Add route entry for the client */
			mapping = ipa_be_client_mapping_add_or_ref(flow_ip_ptr, 0, lan2lan, mac);
			if (IS_ERR(mapping)) {
				ret = IPA_CMN_RESPONSE_EMSG;
				goto failed_ret;
			}
			step = V6_MDM_STEP_CLIENT_MAPPING;
			if (mapping != NULL) {
				ret = ipa_ipv6_add_route_rule(v6_msg, lan2lan, flow_interface_num, mac, is_ret);
				if (ret != 0) {
					IPA_BE_ERR("Failed to add IPv6 route rule\n");
					goto failed_ret;
				}
				step = V6_MDM_STEP_ROUTE_RULE;
				lan_route_added = true;
			}

			/* Install uplink filter rules */
			ret = ipa_be_v6_add_uplink_filter_rule(v6_msg, lan2lan, pdn_iface, client_iface, 0);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add IPv6 uplink filter rule\n");
				goto failed_ret;
			}
			step = V6_MDM_STEP_UPLINK_FILTER;

			ret = ipa_be_construct_mtu_rule(IPA_IP_v6, mtu_size, client_iface, pdn_iface, vlan_tag);
			if (ret != 0) {
				IPA_BE_ERR("Failed to construct IPv6 mtu rule\n");
				goto failed_ret;
			}
			step = V6_MDM_STEP_MTU_RULE;

			dl_first_install = !ipa_be_pdn_filter_installed(pdn_iface, IPA_IP_v6);

			ret = add_dft_filtering_rule(pdn_iface, IPA_IP_v6);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add IPv6 dft filtering rule\n");
				goto failed_ret;
			}
			step = V6_MDM_STEP_DFT_FILTER;

			ret = add_icmp_alg_rules(pdn_iface, IPA_IP_v6);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add IPv6 ICMP/ALG filtering rule\n");
				goto failed_ret;
			}
			step = V6_MDM_STEP_ICMP_ALG;

			ret = add_catchup_all_filtering_rule_each_pdn(pdn_iface, IPA_IP_v6);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add IPv6 catchup all filtering rule\n");
				goto failed_ret;
			}
			step = V6_MDM_STEP_CATCHUP_ALL;

			if (dl_first_install) {
				ret = install_wan_filtering_rule(false);
				if (ret != 0) {
					IPA_BE_ERR("Failed to install wan filtering rule\n");
					goto failed_ret;
				}
				step = V6_MDM_STEP_WAN_FILTER;
			} else {
				IPA_BE_DBG("DL rules already installed for pdn_iface=%d IPv6; skip Q6 push\n",
					pdn_iface);
				step = V6_MDM_STEP_WAN_FILTER;
			}

#ifdef CONFIG_ECM_CONVERGENCE
			/* Match against flow_ip_ptr, not v6_msg.tuple.flow_ip -- see the v4 path above. */
			ipa_be_check_and_install_qos_for_new_rule(
				v6_msg.conn_rule.flow_mac,
				v6_msg.vlan_primary_rule.ingress_vlan_tag,
				v6_msg.conn_rule.return_mac,
				v6_msg.vlan_primary_rule.egress_vlan_tag,
				0,
				(const __be32 *)flow_ip_ptr);

			/* Add CT entry */
			ret = ipa_be_add_v6_ct_entry(v6_msg, pdn_iface, lan2lan);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add IPv6 CT entry\n");
				goto failed_ret;
			}
			step = V6_MDM_STEP_CT_ENTRY;
#else
			IPA_BE_DBG("NAT support disabled - skipping ipa_be_add_v6_ct_entry\n");
#endif
		}
	}

	struct ipa_fse_rule fse_info;
	if (ret == IPA_CMN_RESPONSE_ACK) {
		/* FSE rule creation */
		if (ipa3_is_vpnum_valid(v6_msg.conn_rule.flow_interface_num)) {
			memset(&fse_info, 0, sizeof(fse_info));
			ipa_be_fill_ipv6_fse_info(&fse_info, &v6_msg.tuple,
						  &v6_msg.conn_rule);
			if (ipa_be_fse_rule_create(&fse_info)) {
				IPA_BE_ERR("Failed to create FSE rule for IPv6 flow\n");
			}
		}

		if (ipa3_is_vpnum_valid(v6_msg.conn_rule.return_interface_num)) {
			memset(&fse_info, 0, sizeof(fse_info));
			ipa_be_fill_ipv6_fse_info_reverse(&fse_info, &v6_msg.tuple,
							  &v6_msg.conn_rule);
			if (ipa_be_fse_rule_create(&fse_info)) {
				IPA_BE_ERR(
					"Failed to create FSE reverse rule for IPv6 flow\n");
			}
		}
	}
	IPA_BE_DBG("Command  return %d \n", ret);
	return ret;

failed_ret:
	ret = IPA_CMN_RESPONSE_EMSG;
	if (lan2lan) {
		switch (step) {
			case L2L_STEP_FLOW_FILTER:
				if (flow_filter_added)
					ipa_be_v6_delete_filter_rule(*((struct ipa_ipv6_rule_destroy_msg *)&v6_msg), v6_msg.conn_rule.return_interface_num, v6_msg.conn_rule.flow_mac, lan2lan);
				/* fallthrough */
			case L2L_STEP_FLOW_ROUTE:
				if (flow_route_added)
					ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping((uint32_t *)&v6_msg.tuple.flow_ip, lan2lan, NULL, NULL, NULL), IPA_IP_v6);
				/* fallthrough */
			case L2L_STEP_FLOW_MAPPING:
				ipa_be_mapping_deref_and_delete((uint32_t *)&v6_msg.tuple.flow_ip, lan2lan, NULL);
				/* fallthrough */
			case L2L_STEP_RET_FILTER:
				if (ret_filter_added)
					ipa_be_v6_delete_filter_rule(*((struct ipa_ipv6_rule_destroy_msg *)&v6_msg), v6_msg.conn_rule.flow_interface_num, v6_msg.conn_rule.return_mac, lan2lan);
				/* fallthrough */
			case L2L_STEP_RET_ROUTE:
				if (ret_route_added)
					ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping((uint32_t *)&v6_msg.tuple.return_ip, lan2lan, NULL, NULL, NULL), IPA_IP_v6);
				/* fallthrough */
			case L2L_STEP_RET_MAPPING:
				ipa_be_mapping_deref_and_delete((uint32_t *)&v6_msg.tuple.return_ip, lan2lan, NULL);
		}
	} else if (backhaul_type == IPA_BACKHAUL_TYPE_ETH) {
		switch (step) {
			case ETH_STEP_WAN_UP_UPLINK:
				ipa_be_handle_wan_down(client_iface, pdn_iface, IPA_IP_v6);
				/* fallthrough */
			case ETH_STEP_BACKHAUL_RULE_FALSE:
				if (wan_route_added)
					ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping(wan_ip_ptr, lan2lan, NULL, NULL, NULL), IPA_IP_v6);
				/* fallthrough */
			case ETH_STEP_BACKHAUL_RULE_TRUE:
				if (wan_catchall_acquired)
					ipa_be_wan_catchall_release(pdn_iface, IPA_IP_v6);
				/* fallthrough */
			case ETH_STEP_WAN_MAPPING:
				ipa_be_mapping_deref_and_delete(wan_ip_ptr, lan2lan, NULL);
				/* fallthrough */
			case ETH_STEP_WAN_UP_DOWNLINK:
				ipa_be_handle_wan_down(pdn_iface, client_iface, IPA_IP_v6);
				/* fallthrough */
			case ETH_STEP_LAN_MAPPING_ROUTE:
				if (lan_route_added)
					ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping(flow_ip_ptr, lan2lan, NULL, NULL, NULL), IPA_IP_v6);
				ipa_be_mapping_deref_and_delete(flow_ip_ptr, lan2lan, NULL);
		}
	} else {
		switch (step) {
			case V6_MDM_STEP_CT_ENTRY:
				/* no rollback available for CT entry */
				/* fallthrough */
			case V6_MDM_STEP_WAN_FILTER:
				/* Need a way to rollback wan_filtering_rule if possible */
				/* fallthrough */
			case V6_MDM_STEP_CATCHUP_ALL:
				delete_catchup_all_filtering_rule_each_pdn(pdn_iface, IPA_IP_v6);
				/* fallthrough */
			case V6_MDM_STEP_ICMP_ALG:
				delete_icmp_alg_rules(pdn_iface, IPA_IP_v6);
				/* fallthrough */
			case V6_MDM_STEP_DFT_FILTER:
				delete_dft_filtering_rule(pdn_iface, IPA_IP_v6);
				/* fallthrough */
			case V6_MDM_STEP_MTU_RULE:
				ipa_be_delete_mtu_rule(client_iface, pdn_iface, IPA_IP_v6);
				/* fallthrough */
			case V6_MDM_STEP_UPLINK_FILTER:
				ipa_be_v6_delete_uplink_filter_rule(*((struct ipa_ipv6_rule_destroy_msg *)&v6_msg), pdn_iface, client_iface);
				/* fallthrough */
			case V6_MDM_STEP_ROUTE_RULE:
				if (lan_route_added)
					ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping(flow_ip_ptr, lan2lan, NULL, NULL, NULL), IPA_IP_v6);
				/* fallthrough */
			case V6_MDM_STEP_CLIENT_MAPPING:
				ipa_be_mapping_deref_and_delete(flow_ip_ptr, lan2lan, NULL);
		}
	}
	IPA_BE_DBG("Command  return %d \n", ret);
	return ret;
}


/**
 * ipa_create_ipv4_rule_msg()
 *	Convert create message format from ecm to ipa
 *
 * @param ipa_ctx IPA context
 * @param msg The IPv4 message
 *
 * @return ipa_cmn_response The response generated by the Tx operation
 */
static enum ipa_cmn_response ipa_create_ipv4_rule_msg(
	struct ipa_ctx_instance_internal *ipa_ctx,
	struct ipa_ipv4_msg *msg)
{
	enum ipa_cmn_response ret = IPA_CMN_RESPONSE_ACK;


	IPA_BE_DBG("ECMIPA msg type %u \n", msg->cm.type);

	if (!(msg->msg.rule_create.valid_flags & IPA_RULE_CREATE_CONN_VALID)) {
		ret = IPA_CMN_RESPONSE_EMSG;
		ipa_incr_exceptions(IPA_EXCEPTION_CONNECTION_INVALID);
		goto failed_ret;
	}

	switch (msg->msg.rule_create.tuple.protocol) {
	case IPPROTO_TCP:
		if (!(msg->msg.rule_create.valid_flags & IPA_RULE_CREATE_TCP_VALID)) {
			ret = IPA_CMN_RESPONSE_EMSG;
			ipa_incr_exceptions(IPA_EXCEPTION_TCP_INVALID);
			goto failed_ret;
		}
		break;

	case IPPROTO_UDP:
	case IPPROTO_GRE:
	case IPPROTO_L2TP:
	case IPPROTO_IPV6:
	case IPPROTO_ESP:
	case IPPROTO_RAW:
		/*
		 * IPPROTO_RAW for accelerating PPPoE bridged flows using 3-tuple information
		 */
		break;

	default:
		ret = IPA_CMN_RESPONSE_EMSG;
		ipa_incr_exceptions(IPA_EXCEPTION_PROTOCOL_NOT_SUPPORT);
		goto failed_ret;
	}


	if (!ipa_ipv4_create_rule(msg->msg.rule_create)) {
		/* success */
		IPA_BE_DBG("Command create rule success .. return\n");
		ret = IPA_CMN_RESPONSE_ACK;
	} else {
		/* Failed */
		ret = IPA_CMN_RESPONSE_EMSG;
		ipa_incr_exceptions(IPA_EXCEPTION_CREATE_FAILED);
		IPA_BE_ERR("Command create rule failure .. return\n");
	}

	/*
	 * Fall through
	 */
failed_ret:
	IPA_BE_DBG("Command  return %d \n", ret);
	return ret;
}

/**
 * ipa_create_ipv6_rule_msg()
 *	Convert create message format from ecm to ipa
 *
 * @param ipa_ctx IPA context
 * @param msg The IPv6 message
 *
 * @return ipa_cmn_response The response generated by the Tx operation
 */
static enum ipa_cmn_response ipa_create_ipv6_rule_msg(
	struct ipa_ctx_instance_internal *ipa_ctx,
	struct ipa_ipv6_msg *msg)
{
	enum ipa_cmn_response ret = IPA_CMN_RESPONSE_ACK;


	IPA_BE_DBG("ECMIPA msg type %u \n", msg->cm.type);

	if (!(msg->msg.rule_create.valid_flags & IPA_RULE_CREATE_CONN_VALID)) {
		ret = IPA_CMN_RESPONSE_EMSG;
		ipa_incr_exceptions(IPA_EXCEPTION_CONNECTION_INVALID);
		goto failed_ret;
	}

	switch (msg->msg.rule_create.tuple.protocol) {
	case IPPROTO_TCP:
		if (!(msg->msg.rule_create.valid_flags & IPA_RULE_CREATE_TCP_VALID)) {
			ret = IPA_CMN_RESPONSE_EMSG;
			ipa_incr_exceptions(IPA_EXCEPTION_TCP_INVALID);
			goto failed_ret;
		}
		break;

	case IPPROTO_UDP:
	case IPPROTO_GRE:
	case IPPROTO_L2TP:
	case IPPROTO_IPV6:
	case IPPROTO_ESP:
	case IPPROTO_RAW:
		/*
		 * IPPROTO_RAW for accelerating PPPoE bridged flows using 3-tuple information
		 */
		break;

	default:
		ret = IPA_CMN_RESPONSE_EMSG;
		ipa_incr_exceptions(IPA_EXCEPTION_PROTOCOL_NOT_SUPPORT);
		goto failed_ret;
	}


	if (!ipa_ipv6_create_rule(msg->msg.rule_create)) {
		/* success */
		IPA_BE_DBG("Command create rule success .. return\n");
		ret = IPA_CMN_RESPONSE_ACK;
	} else {
		/* Failed */
		ret = IPA_CMN_RESPONSE_EMSG;
		ipa_incr_exceptions(IPA_EXCEPTION_CREATE_FAILED);
		IPA_BE_ERR("Command create rule failure .. return\n");
	}

	/*
	 * Fall through
	 */
failed_ret:
	IPA_BE_DBG("Command  return %d \n", ret);
	return ret;
}



/**
 * ipa_be_teardown_pdn_dl_rules() - drop this connection's reference to the
 * PDN-global DL rule set, re-pushing to Q6 only if the table changed.
 * @pdn_iface: PDN interface number
 * @iptype: IP version (IPA_IP_v4 or IPA_IP_v6)
 *
 * The default / ICMP-ALG / catchup-all DL rules are shared by every
 * connection on a PDN, so these deletes are ref-counted and are usually
 * no-ops. Re-pushing an unchanged table makes Q6 rewrite its DL filter table
 * for nothing, which costs DL packets on the *other* connections that are
 * still accelerated and were never defuncted.
 *
 * Delete order is load-bearing: the three helpers decrement one shared
 * ref_count using staggered thresholds tuned to catchup -> icmp -> dft.
 * The helpers log their own failures, so only the push is logged here.
 */
static void ipa_be_teardown_pdn_dl_rules(int pdn_iface, enum ipa_ip_type iptype)
{
	int removed = 0;
	int rc;

	rc = delete_catchup_all_filtering_rule_each_pdn(pdn_iface, iptype);
	if (rc > 0)
		removed += rc;

	rc = delete_icmp_alg_rules(pdn_iface, iptype);
	if (rc > 0)
		removed += rc;

	rc = delete_dft_filtering_rule(pdn_iface, iptype);
	if (rc > 0)
		removed += rc;

	if (!removed) {
		IPA_BE_DBG("DL rules unchanged for pdn %d ip %d; skip Q6 push\n",
			   pdn_iface, iptype);
		return;
	}

	rc = install_wan_filtering_rule(false);
	if (rc)
		IPA_BE_ERR("Failed to push DL rules to Q6, pdn %d ip %d: %d\n",
			   pdn_iface, iptype, rc);
}

void ipa_ipv4_destroy_rule(struct ipa_ipv4_rule_destroy_msg *msg)
{
	bool lan2lan = false;
	int ref = 0;
	int rt_hdl = 0;
	int pdn_iface = 0;
	int client_iface = 0;
	int hdr_hdl = -1;
	int proc_ctx_hdl = -1;
	char proc_ctx_name[32] = {0};
	ip_addr_t lan_client_ip = {0};
	ip_addr_t ret_addr = {0};
	ip_addr_t flow_addr = {0};
	ip_addr_t wan_ip = {0};
	int wan_rt_hdl = 0;
	int wan_ref = -1;
	enum ipa_backhaul_type backhaul_type = IPA_BACKHAUL_TYPE_UNKNOWN;
	enum ipa_hw_type ipa_ver = ipa_get_hw_type();

	IPA_BE_DBG("Entry  ipa_ipv4_destroy_rule \n");
	ipa_be_log_ipv4_destroy_details(msg);

	struct ipa_fse_rule fse_info;
	/* FSE rule destruction */
	if (ipa3_is_vpnum_valid(msg->conn_rule.flow_interface_num)) {
		memset(&fse_info, 0, sizeof(fse_info));
		ipa_be_fill_ipv4_fse_info(&fse_info, &msg->tuple, &msg->conn_rule);
		if (ipa_be_fse_rule_destroy(&fse_info)) {
			IPA_BE_ERR("Failed to destroy FSE rule for IPv4 flow\n");
		}
	}

	if (ipa3_is_vpnum_valid(msg->conn_rule.return_interface_num)) {
		memset(&fse_info, 0, sizeof(fse_info));
		ipa_be_fill_ipv4_fse_info_reverse(&fse_info, &msg->tuple, &msg->conn_rule);
		if (ipa_be_fse_rule_destroy(&fse_info)) {
			IPA_BE_ERR("Failed to destroy FSE reverse rule for IPv4 flow\n");
		}
	}

	if ((msg->conn_rule.flow_interface_num == msg->conn_rule.flow_top_interface_num) &&
		(msg->conn_rule.return_interface_num == msg->conn_rule.return_top_interface_num))
	{
		lan2lan = true;
		IPA_BE_DBG("No natted ip hence lan2lan: %d\n", lan2lan);
	}
	IPA_BE_DBG("Is connection lan2lan: %d\n", lan2lan);

	/*
	 * tuple.return_ip mapping only exists in the lan2lan create path.
	 * For LAN2WAN, running this deref against return_ip_xlate in the
	 * no-translation case (return_ip_xlate == tuple.return_ip) would
	 * double-deref the WAN mapping and skip the catch-all release.
	 */
	if (lan2lan)
	{
		/* Delete CT entry per-connection, not gated on shared flow_ip ref count. */
		if (ipa_ver >= IPA_HW_v7_0) {
			IPA_BE_DBG("Deleting CT entry for LAN2LAN on IPA v7.0+\n");
			ipa_be_delete_ct_entry_v4(*msg);
		}

		ret_addr[0] = msg->tuple.return_ip;
		rt_hdl = ipa_get_rt_hdl_from_mapping(ret_addr,
			lan2lan, &hdr_hdl, &proc_ctx_hdl, proc_ctx_name);
		IPA_BE_DBG("Rt hdl %d\n", rt_hdl);

		ref = ipa_be_mapping_deref_and_delete(ret_addr, lan2lan, NULL);
		if (ref == -1) {
			IPA_BE_DBG("Entry %pI4n does not exist \n", &msg->tuple.return_ip);
		} else if (ref > 0) {
			IPA_BE_DBG("Entry %pI4n has other references ref count decreased %d\n", &msg->tuple.return_ip, ref);
		} else if (ref == 0) {
			IPA_BE_DBG("Ref now %d, deleting route rule\n", ref);
			if (rt_hdl) {
				ipa_delete_route_rule(lan2lan, rt_hdl, IPA_IP_v4);

				if (hdr_hdl > 0) {
					ipa_be_delete_hdr_by_handle(hdr_hdl);
				}

				if (proc_ctx_name[0] != '\0') {
					ipa_be_delete_proc_ctx(proc_ctx_name);
				}
			}
		}
		/* Filter delete is self-gated by its own global (dst,ip) refcount,
		 * so drop the forward-direction filter reference unconditionally. */
		if (ipa_be_v4_delete_filter_rule(*msg, msg->conn_rule.flow_interface_num, msg->conn_rule.return_mac, lan2lan))
			IPA_BE_ERR("Failed to delete forward filter rule\n");

		/* Delete reverse flow only for LAN2LAN */
		flow_addr[0] = msg->tuple.flow_ip;
		rt_hdl = ipa_get_rt_hdl_from_mapping(flow_addr,
			lan2lan, &hdr_hdl, &proc_ctx_hdl, proc_ctx_name);
		IPA_BE_DBG("Rt hdl %d \n", rt_hdl);

		ref = ipa_be_mapping_deref_and_delete(flow_addr, lan2lan, NULL);
		if (ref == -1) {
			IPA_BE_DBG("Entry %pI4n does not exist \n", &msg->tuple.flow_ip);
		} else if (ref > 0) {
			IPA_BE_DBG("Entry %pI4n has other references ref count decreased %d\n", &msg->tuple.flow_ip, ref);
		} else if (ref == 0) {
			IPA_BE_DBG("Ref now %d, deleting route rule\n", ref);
			if (rt_hdl) {
				ipa_delete_route_rule(lan2lan, rt_hdl, IPA_IP_v4);

				if (hdr_hdl > 0) {
					ipa_be_delete_hdr_by_handle(hdr_hdl);
				}

				if (proc_ctx_name[0] != '\0') {
					ipa_be_delete_proc_ctx(proc_ctx_name);
				}
			}
		}
		/* Filter delete is self-gated by its own global (dst,ip) refcount,
		 * so drop the return-direction filter reference unconditionally
		 * (symmetric with the flow-direction delete above). */
		if (ipa_be_v4_delete_filter_rule(*msg, msg->conn_rule.return_interface_num, msg->conn_rule.flow_mac, lan2lan))
			IPA_BE_ERR("Failed to delete return filter rule\n");
	}
	else
	{
		/* LAN2WAN: Determine direction-specific parameters first, then delete all rules */
		if (msg->conn_rule.return_interface_num == msg->conn_rule.return_top_interface_num) {
			IPA_BE_DBG("Uplink flow destroy - deleting both uplink and downlink rules\n");
			pdn_iface        = msg->conn_rule.return_interface_num;
			client_iface     = msg->conn_rule.flow_interface_num;
			lan_client_ip[0] = msg->tuple.flow_ip;
		} else if (msg->conn_rule.flow_interface_num == msg->conn_rule.flow_top_interface_num) {
			IPA_BE_DBG("Downlink flow destroy - deleting both downlink and uplink rules\n");
			pdn_iface        = msg->conn_rule.flow_interface_num;
			client_iface     = msg->conn_rule.return_interface_num;
			lan_client_ip[0] = msg->conn_rule.return_ip_xlate;
		} else {
			IPA_BE_ERR("Invalid WAN flow param\n");
			goto skip_wan_rules;
		}

		/* Delete route rule for client */
		rt_hdl = ipa_get_rt_hdl_from_mapping(lan_client_ip,
			lan2lan, &hdr_hdl, &proc_ctx_hdl, proc_ctx_name);

		ref = ipa_be_mapping_deref_and_delete(lan_client_ip, lan2lan, NULL);
		if (ref == 0 && rt_hdl) {
			ipa_delete_route_rule(lan2lan, rt_hdl, IPA_IP_v4);

			/* Clean up header using handle-based reference counting */
			if (hdr_hdl > 0) {
				ipa_be_delete_hdr_by_handle(hdr_hdl);
			}

			/* Clean up proc_ctx using name-based reference counting */
			if (proc_ctx_name[0] != '\0') {
				ipa_be_delete_proc_ctx(proc_ctx_name);
			}
		}

		/* Delete uplink filter rules */
		if (ipa_be_detect_backhaul_type(pdn_iface, &backhaul_type) < 0) {
			IPA_BE_ERR("Failed to detect backhaul for iface %d during destroy; defaulting to MODEM\n",
				pdn_iface);
			backhaul_type = IPA_BACKHAUL_TYPE_MODEM;
		}
		if (backhaul_type == IPA_BACKHAUL_TYPE_ETH) {
			if (msg->conn_rule.return_interface_num == msg->conn_rule.return_top_interface_num)
				wan_ip[0] = msg->conn_rule.return_ip_xlate;
			else
				wan_ip[0] = msg->tuple.flow_ip;

			/* Uplink keys WAN on return_ip_xlate, downlink on flow_ip. */
			wan_rt_hdl = -1;
			wan_ref = ipa_be_mapping_deref_and_delete(wan_ip, lan2lan, &wan_rt_hdl);
			if (wan_ref == 0) {
				if (wan_rt_hdl > 0)
					ipa_delete_route_rule(lan2lan, wan_rt_hdl, IPA_IP_v4);
				ipa_be_wan_catchall_release(pdn_iface, IPA_IP_v4);
			}

			if (ipa_be_handle_wan_down(pdn_iface, client_iface, IPA_IP_v4) != 0)
				IPA_BE_ERR("Failed to delete WAN filter for pdn %d, client %d\n",
					pdn_iface, client_iface);
			if (ipa_be_handle_wan_down(client_iface, pdn_iface, IPA_IP_v4) != 0)
				IPA_BE_ERR("Failed to delete WAN filter for pdn %d, client %d\n",
					client_iface, pdn_iface);

			/* Invalidate only on last flow; per-flow would thrash the cache. */
			if (wan_ref == 0)
				ipa_be_backhaul_cache_invalidate(pdn_iface);

			/* NAT entry delete FIRST — UL Q6 notify fires while DL filter
			 * state is still active, so Q6's lookup succeeds.
			 */
#ifdef CONFIG_ECM_CONVERGENCE
			/* Hold ipa_client_db_lock across the entire QoS cleanup sequence to
			 * prevent a race where Thread B adds a connection and installs
			 * catch-all rules between our MAC/IP deletions and the NULL check.
			 * Lock hierarchy: ipa_client_db_lock → msg_qos_param_lock.
			 */
			mutex_lock(&ipa_client_db_lock);
			ipa_be_delete_qos_rules_for_connection(
				msg->conn_rule.flow_mac,
				msg->conn_rule.return_mac);
			ipa_be_delete_ip_keyed_qos_rules_v4(
				msg->tuple.flow_ip);

			ipa_be_delete_entry(*msg, false);

			if (ipa_db_mappings == NULL)
				ipa_be_delete_qos_catch_all_rules();
			mutex_unlock(&ipa_client_db_lock);
#else
			IPA_BE_DBG("NAT support disabled - skipping ipa_be_delete_entry (eth backhaul)\n");
#endif
		} else {
			ipa_be_v4_delete_uplink_filter_rule(*msg, pdn_iface, client_iface);

			/* Delete MTU rule for the client/pdn/ip tuple (ref-counted) */
			if (ipa_be_delete_mtu_rule(client_iface, pdn_iface, IPA_IP_v4) != 0) {
				IPA_BE_ERR("Failed to delete MTU rule for client %d pdn %d\n", client_iface, pdn_iface);
			}

			/* Delete downlink rules; re-push to Q6 only if the table changed */
			ipa_be_teardown_pdn_dl_rules(pdn_iface, IPA_IP_v4);

#ifdef CONFIG_ECM_CONVERGENCE
			ipa_be_delete_entry(*msg, false);
#else
			IPA_BE_DBG("NAT support disabled - skipping ipa_be_delete_entry\n");
#endif
		}
	skip_wan_rules:
	}

	IPA_BE_DBG("Deleted entry %d \n", ref);
	return;
}

/**
 * Destroys an IPv6 rule and related resources.
 *
 * Deletes route/filter rules for a given IPv6 flow and its reverse,
 * manages reference counts, logs destruction, and cleans up topology/resources.
 *
 * @param msg   Pointer to IPv6 rule destroy message containing tuple and connection info.
 */
void ipa_ipv6_destroy_rule(struct ipa_ipv6_rule_destroy_msg *msg)
{
	bool lan2lan = false;
	int ref = 0;
	int rt_hdl = 0;
	int pdn_iface = 0;
	int client_iface = 0;
	int hdr_hdl = -1;
	int proc_ctx_hdl = -1;
	char proc_ctx_name[32] = {0};
	uint32_t *flow_ip_ptr = NULL;
	uint32_t *wan_ip_ptr = NULL;
	int wan_rt_hdl = 0;
	int wan_ref = -1;
	enum ipa_backhaul_type backhaul_type = IPA_BACKHAUL_TYPE_UNKNOWN;

	if (!msg) {
		IPA_BE_ERR("Invalid message pointer\n");
		return;
	}

	IPA_BE_DBG("Entry ipa_ipv6_destroy_rule\n");

	struct ipa_fse_rule fse_info;
	/* FSE rule destruction */
	if (ipa3_is_vpnum_valid(msg->conn_rule.flow_interface_num)) {
		memset(&fse_info, 0, sizeof(fse_info));
		ipa_be_fill_ipv6_fse_info(&fse_info, &msg->tuple, &msg->conn_rule);
		if (ipa_be_fse_rule_destroy(&fse_info)) {
			IPA_BE_ERR("Failed to destroy FSE rule for IPv6 flow\n");
		}
	}

	if (ipa3_is_vpnum_valid(msg->conn_rule.return_interface_num)) {
		memset(&fse_info, 0, sizeof(fse_info));
		ipa_be_fill_ipv6_fse_info_reverse(&fse_info, &msg->tuple, &msg->conn_rule);
		if (ipa_be_fse_rule_destroy(&fse_info)) {
			IPA_BE_ERR("Failed to destroy FSE reverse rule for IPv6 flow\n");
		}
	}

	ipa_be_log_ipv6_destroy_details(msg);

	/* Determine if this is a LAN-to-LAN connection */
	if ((msg->conn_rule.flow_interface_num == msg->conn_rule.flow_top_interface_num) &&
		(msg->conn_rule.return_interface_num == msg->conn_rule.return_top_interface_num)) {
		lan2lan = true;
		IPA_BE_DBG("No xlated ip hence lan2lan: %d\n", lan2lan);
	}
	IPA_BE_DBG("Is connection lan2lan: %d\n", lan2lan);

	/*
	 * For downlink LAN2WAN, tuple.return_ip is the LAN client mapping, which
	 * is deref'd later via flow_ip_ptr — skipping the early block here prevents
	 * a double-deref (premature route/hdr deletion while another flow is active).
	 * lan2lan and uplink LAN2WAN legitimately key on return_ip here.
	 */
	if (lan2lan ||
	    msg->conn_rule.return_interface_num == msg->conn_rule.return_top_interface_num) {
		rt_hdl = ipa_get_rt_hdl_from_mapping((uint32_t *)&msg->tuple.return_ip,
			lan2lan, &hdr_hdl, &proc_ctx_hdl, proc_ctx_name);
		IPA_BE_DBG("Return flow rt_hdl %d\n", rt_hdl);

		ref = ipa_be_mapping_deref_and_delete((uint32_t *)&msg->tuple.return_ip, lan2lan, NULL);
		if (ref == -1) {
			IPA_BE_DBG("Entry %pI6n does not exist\n", &msg->tuple.return_ip);
		} else if (ref > 0) {
			IPA_BE_DBG("Entry %pI6n has other references, ref count decreased to %d\n", &msg->tuple.return_ip, ref);
		} else if (ref == 0) {
			IPA_BE_DBG("Ref now %d, deleting route rule\n", ref);
			if (rt_hdl > 0) {
				if (ipa_ipv6_delete_route_rule(*msg, lan2lan, rt_hdl, IPA_IP_v6) != 0) {
					IPA_BE_ERR("Failed to delete return flow route rule hdl %d\n", rt_hdl);
				}
			}
			if (ipa_be_is_uplink_lan2wan_eth_bh(msg->conn_rule.return_interface_num,
							   msg->conn_rule.return_top_interface_num,
							   lan2lan)) {
				ipa_be_wan_catchall_release(msg->conn_rule.return_interface_num, IPA_IP_v6);
				/* Invalidate only on last flow; per-flow would thrash the cache. */
				ipa_be_backhaul_cache_invalidate(msg->conn_rule.return_interface_num);
			}
			if (rt_hdl > 0) {
				if (hdr_hdl > 0) {
					ipa_be_delete_hdr_by_handle(hdr_hdl);
				}

				if (proc_ctx_name[0] != '\0') {
					ipa_be_delete_proc_ctx(proc_ctx_name);
				}
			}
		}
		/* Filter delete is self-gated by its own per-(intf,dst,ip) refcount,
		 * so drop the return-flow filter reference unconditionally. */
		if (lan2lan) {
			if (ipa_be_v6_delete_filter_rule(*msg, msg->conn_rule.flow_interface_num, msg->conn_rule.return_mac, lan2lan) != 0)
				IPA_BE_ERR("Failed to delete LAN2LAN filter rule for return flow\n");
		}
	}

	if (lan2lan) {
#ifdef CONFIG_ECM_CONVERGENCE
		/* Delete IPv6 CT entry per-connection, not gated on shared flow_ip ref count. */
		IPA_BE_DBG("Deleting IPv6 CT entry for LAN2LAN\n");
		ipa_be_handle_v6_ct_deletion(msg, true);
#endif

		/* LAN2LAN: Also delete forward flow - check reference count first */
		rt_hdl = ipa_get_rt_hdl_from_mapping((uint32_t *)&msg->tuple.flow_ip, lan2lan, &hdr_hdl, &proc_ctx_hdl, proc_ctx_name);
		IPA_BE_DBG("Forward flow rt_hdl %d\n", rt_hdl);

		ref = ipa_be_mapping_deref_and_delete((uint32_t *)&msg->tuple.flow_ip, lan2lan, NULL);
		if (ref == -1) {
			IPA_BE_ERR("Entry %pI6n does not exist\n", &msg->tuple.flow_ip);
		} else if (ref > 0) {
			IPA_BE_DBG("Entry %pI6n has other references, ref count decreased to %d\n", &msg->tuple.flow_ip, ref);
		} else if (ref == 0) {
			IPA_BE_DBG("Ref now %d, deleting route rule\n", ref);
			if (rt_hdl) {
				if (ipa_delete_route_rule(lan2lan, rt_hdl, IPA_IP_v6) != 0) {
					IPA_BE_ERR("Failed to delete forward flow route rule hdl %d\n", rt_hdl);
				}

				if (hdr_hdl > 0) {
					ipa_be_delete_hdr_by_handle(hdr_hdl);
				}

				if (proc_ctx_name[0] != '\0') {
					ipa_be_delete_proc_ctx(proc_ctx_name);
				}
			}
		}

		/* Filter delete is self-gated by its own per-(intf,dst,ip) refcount,
		 * so drop the forward-flow filter reference unconditionally
		 * (symmetric with the return-flow delete above). */
		if (ipa_be_v6_delete_filter_rule(*msg, msg->conn_rule.return_interface_num, msg->conn_rule.flow_mac, lan2lan) != 0)
			IPA_BE_ERR("Failed to delete filter rule for forward flow\n");
	} else {
		/* LAN2WAN: Determine direction-specific parameters first, then delete all rules */
		if (msg->conn_rule.return_interface_num == msg->conn_rule.return_top_interface_num) {
			IPA_BE_DBG("Uplink flow destroy - deleting both uplink and downlink rules\n");
			pdn_iface    = msg->conn_rule.return_interface_num;
			client_iface = msg->conn_rule.flow_interface_num;
			flow_ip_ptr  = (uint32_t *)&msg->tuple.flow_ip;
			/* WAN IP for uplink (return_ip) already cleaned by early block above */
		} else if (msg->conn_rule.flow_interface_num == msg->conn_rule.flow_top_interface_num) {
			IPA_BE_DBG("Downlink flow destroy - deleting both downlink and uplink rules\n");
			pdn_iface    = msg->conn_rule.flow_interface_num;
			client_iface = msg->conn_rule.return_interface_num;
			flow_ip_ptr  = (uint32_t *)&msg->tuple.return_ip;
			/* WAN IP for downlink not covered by early block; track for cleanup below */
			wan_ip_ptr   = (uint32_t *)&msg->tuple.flow_ip;
		} else {
			IPA_BE_ERR("Invalid WAN flow param\n");
			goto skip_v6_wan_rules;
		}

		/* Delete route rule for client */
		rt_hdl = ipa_get_rt_hdl_from_mapping(flow_ip_ptr,
			lan2lan, &hdr_hdl, &proc_ctx_hdl, proc_ctx_name);

		ref = ipa_be_mapping_deref_and_delete(flow_ip_ptr, lan2lan, NULL);
		if (ref == 0 && rt_hdl) {
			ipa_ipv6_delete_route_rule(*msg, lan2lan, rt_hdl, IPA_IP_v6);

			/* Clean up header using handle-based reference counting */
			if (hdr_hdl > 0) {
				ipa_be_delete_hdr_by_handle(hdr_hdl);
			}

			/* Clean up proc_ctx using name-based reference counting */
			if (proc_ctx_name[0] != '\0') {
				ipa_be_delete_proc_ctx(proc_ctx_name);
			}
		}

		/* Delete uplink filter rules */
		if (ipa_be_detect_backhaul_type(pdn_iface, &backhaul_type) < 0) {
			IPA_BE_ERR("Failed to detect backhaul for iface %d during destroy; defaulting to MODEM\n",
				pdn_iface);
			backhaul_type = IPA_BACKHAUL_TYPE_MODEM;
		}
		if (backhaul_type == IPA_BACKHAUL_TYPE_ETH) {
			/* For downlink: WAN IP was not handled by the early block, clean it up now */
			if (wan_ip_ptr) {
				wan_rt_hdl = -1;
				wan_ref = ipa_be_mapping_deref_and_delete(wan_ip_ptr, lan2lan, &wan_rt_hdl);
				if (wan_ref == 0) {
					if (wan_rt_hdl > 0)
						ipa_delete_route_rule(lan2lan, wan_rt_hdl, IPA_IP_v6);
					ipa_be_wan_catchall_release(pdn_iface, IPA_IP_v6);
				}
			}

			if (ipa_be_handle_wan_down(pdn_iface, client_iface, IPA_IP_v6) != 0)
				IPA_BE_ERR("Failed to delete WAN filter for pdn %d, client %d\n",
					pdn_iface, client_iface);
			if (ipa_be_handle_wan_down(client_iface, pdn_iface, IPA_IP_v6) != 0)
				IPA_BE_ERR("Failed to delete WAN filter for pdn %d, client %d\n",
					client_iface, pdn_iface);

			/* Invalidate only on last flow; per-flow would thrash the cache. */
			if (wan_ip_ptr && wan_ref == 0)
				ipa_be_backhaul_cache_invalidate(pdn_iface);

			/* CT entry delete FIRST — same rationale as v4: fires UL Q6 notify
			 * while DL filter state is still active in Q6.
			 */
#ifdef CONFIG_ECM_CONVERGENCE
			ipa_be_handle_v6_ct_deletion(msg, false);
#else
			IPA_BE_DBG("NAT support disabled - skipping ipa_be_handle_v6_ct_deletion\n");
#endif
		} else {
			if (ipa_be_v6_delete_uplink_filter_rule(*msg, pdn_iface, client_iface) != 0)
				IPA_BE_ERR("Failed to delete uplink filter rule for pdn %d, client %d\n",
					pdn_iface, client_iface);

			/* Delete MTU rule for the client/pdn/ip tuple (ref-counted) */
			if (ipa_be_delete_mtu_rule(client_iface, pdn_iface, IPA_IP_v6) != 0) {
				IPA_BE_ERR("Failed to delete MTU rule for client %d pdn %d\n", client_iface, pdn_iface);
			}

			/* Delete downlink rules; re-push to Q6 only if the table changed */
			ipa_be_teardown_pdn_dl_rules(pdn_iface, IPA_IP_v6);

#ifdef CONFIG_ECM_CONVERGENCE
			/* Hold ipa_client_db_lock across the entire QoS cleanup sequence to
			 * prevent a race where Thread B adds a connection and installs
			 * catch-all rules between our MAC/IP deletions and the NULL check.
			 * Lock hierarchy: ipa_client_db_lock → msg_qos_param_lock.
			 */
			mutex_lock(&ipa_client_db_lock);
			ipa_be_delete_qos_rules_for_connection(
				msg->conn_rule.flow_mac,
				msg->conn_rule.return_mac);
			ipa_be_delete_ip_keyed_qos_rules_v6(
				msg->tuple.flow_ip);

			/* Handle IPv6 CT entry deletion (also removes client from mapping) */
			ipa_be_handle_v6_ct_deletion(msg, false);

			if (ipa_db_mappings == NULL)
				ipa_be_delete_qos_catch_all_rules();
			mutex_unlock(&ipa_client_db_lock);
#else
			IPA_BE_DBG("NAT support disabled - skipping ipa_be_handle_v6_ct_deletion\n");
#endif
		}

	skip_v6_wan_rules:
	}

	IPA_BE_DBG("Successfully deleted IPv6 rule, final ref count: %d\n", ref);
}


/**
 * ipa_destroy_ipv4_rule_msg()
 *	Convert destroy message format from ecm to ipa
 *
 * @param ipa_be_ctx IPA context
 * @param msg The IPv4 message
 *
 * @return ipa_cmn_response The response generated by the Tx operation
 */
static enum ipa_cmn_response ipa_destroy_ipv4_rule_msg(
	struct ipa_ctx_instance_internal *ipa_be_ctx,
	struct ipa_ipv4_msg *msg)
{
	IPA_BE_DBG("ECMIPA entry ipa_destroy_ipv4_rule_msg\n");
	ipa_ipv4_destroy_rule(&msg->msg.rule_destroy);
	return IPA_CMN_RESPONSE_ACK;
}

/**
 * ipa_destroy_ipv6_rule_msg()
 *	Convert destroy message format from ecm to ipa
 *
 * @param ipa_be_ctx IPA context
 * @param msg The IPv6 message
 *
 * @return ipa_cmn_response The response generated by the Tx operation
 */
static enum ipa_cmn_response ipa_destroy_ipv6_rule_msg(
	struct ipa_ctx_instance_internal *ipa_be_ctx,
	struct ipa_ipv6_msg *msg)
{
	IPA_BE_DBG("ECMIPA entry ipa_destroy_ipv6_rule_msg\n");
	ipa_ipv6_destroy_rule(&msg->msg.rule_destroy);
	return IPA_CMN_RESPONSE_ACK;
}


/**
 * ipa_process_response_msg()
 *	Send all pending response message to ECM by calling callback function included in message
 *
 * @param work work structure
 */
static void ipa_process_response_msg(struct work_struct *work)
{
	struct ipa_ctx_instance_internal *ipa_be_ctx = container_of(work, struct ipa_ctx_instance_internal, work);
	struct ipa_response_msg *response = NULL;
	int quota = 2;

	IPA_BE_DBG("Entry Processing response \n");
	spin_lock_bh(&ipa_be_ctx->lock);
	while (quota-- && (response = list_first_entry_or_null(&ipa_be_ctx->msg_queue, struct ipa_response_msg, node))) {
		list_del(&response->node);
		spin_unlock_bh(&ipa_be_ctx->lock);

		IPA_BE_DBG("Processing response quota %d\n", quota);
		/*
		 * Send response message back to caller
		 */
		if ((response->type == IPA_MSG_TYPE_IPV4) && !ipa_ipv4_stopped(ipa_be_ctx)) {
			IPA_BE_DBG("Calling callbacks v4\n");
			struct ipa_ipv4_msg *msg = (struct ipa_ipv4_msg *)response->msg;
			ipa_ipv4_msg_callback_t callback = (ipa_ipv4_msg_callback_t)msg->cm.cb;
			if (callback) {
				callback((void *)msg->cm.app_data, msg);
			}
		} else if ((response->type == IPA_MSG_TYPE_IPV6) && !ipa_ipv6_stopped(ipa_be_ctx)) {
			IPA_BE_DBG("Calling callbacks v6\n");
			struct ipa_ipv6_msg *msg = (struct ipa_ipv6_msg *)response->msg;
			ipa_ipv6_msg_callback_t callback = (ipa_ipv6_msg_callback_t)msg->cm.cb;
			if (callback) {
				callback((void *)msg->cm.app_data, msg);
			}
		}

		/*
		 * Free response message
		 */
		kfree(response);
		spin_lock_bh(&ipa_be_ctx->lock);
	}
	spin_unlock_bh(&ipa_be_ctx->lock);

	/*
	 * If having more msg in the queue, schedule it again.
	 */
	if (response) {
		IPA_BE_DBG("More response to process  ... queue again\n");
		schedule_work(&ipa_be_ctx->work);
	}

	IPA_BE_DBG("Exit Processing response \n");
}

/**
 * ipa_be_ipv4_send_request_with_resp()
 *	Transmit an IPv4 message to the ipa
 *
 * @param ipa_ctx IPA context
 * @param msg The IPv4 message
 *
 * @return ipa_cmn_response ACK/NACK Msg
 */
enum ipa_cmn_response ipa_be_ipv4_send_request_with_resp(struct ipa_ctx_instance *ipa_ctx, struct ipa_ipv4_msg *msg)
{
	IPA_BE_DBG("ECMIPA entry ipa_ctx %p msg %p type %s\n", ipa_ctx, msg, ipa_be_message_type_to_string(msg->cm.type));
	switch (msg->cm.type) {
	case IPA_TX_CREATE_RULE_MSG:
		return ipa_create_ipv4_rule_msg(IPA_CTX_TO_PRIVATE(ipa_ctx), msg);
	case IPA_TX_DESTROY_RULE_MSG:
		return ipa_destroy_ipv4_rule_msg(IPA_CTX_TO_PRIVATE(ipa_ctx), msg);
	/*
	case IPA_TX_CREATE_MULTICAST_RULE_MSG:
		return ipa_create_ipv4_mc_rule_msg(IPA_CTX_TO_PRIVATE(ipa_ctx), msg);
	case IPA_TX_DESTROY_MULTICAST_RULE_MSG:
		return ipa_destroy_ipv4_mc_rule_msg(IPA_CTX_TO_PRIVATE(ipa_ctx), msg);
	*/
	default:
		ipa_incr_exceptions(IPA_EXCEPTION_IPV4_MSG_UNKNOWN);
		return IPA_CMN_RESPONSE_EMSG;
	}
}
EXPORT_SYMBOL(ipa_be_ipv4_send_request_with_resp);


/**
 * ipa_be_ipv6_send_request_with_resp()
 *	Transmit an IPv6 message to the ipa
 *
 * @param ipa_ctx IPA context
 * @param msg The IPv6 message
 *
 * @return ipa_cmn_response ACK/NACK Msg
 */
enum ipa_cmn_response ipa_be_ipv6_send_request_with_resp(struct ipa_ctx_instance *ipa_ctx, struct ipa_ipv6_msg *msg)
{
	IPA_BE_DBG("ECMIPA entry ipa_ctx %p msg %p type %s\n", ipa_ctx, msg, ipa_be_message_type_to_string(msg->cm.type));
	switch (msg->cm.type) {
	case IPA_TX_CREATE_RULE_MSG:
		return ipa_create_ipv6_rule_msg(IPA_CTX_TO_PRIVATE(ipa_ctx), msg);
	case IPA_TX_DESTROY_RULE_MSG:
		return ipa_destroy_ipv6_rule_msg(IPA_CTX_TO_PRIVATE(ipa_ctx), msg);
	case IPA_TX_CREATE_MULTICAST_RULE_MSG:
		//return ipa_create_ipv4_mc_rule_msg(IPA_CTX_TO_PRIVATE(ipa_ctx), msg);
	case IPA_TX_DESTROY_MULTICAST_RULE_MSG:
		//return ipa_destroy_ipv4_mc_rule_msg(IPA_CTX_TO_PRIVATE(ipa_ctx), msg);
	default:
		ipa_incr_exceptions(IPA_EXCEPTION_IPV6_MSG_UNKNOWN);
		return IPA_CMN_RESPONSE_EMSG;
	}
}
EXPORT_SYMBOL(ipa_be_ipv6_send_request_with_resp);



/**
 * ipa_ipv4_notify_register()
 *	Register a notifier callback for IPv4 messages from IPA
 *
 * @param cb The callback pointer
 * @param app_data The application context for this message
 *
 * @return struct ipa_ctx_instance * The IPA context
 */
struct ipa_ctx_instance *ipa_ipv4_notify_register(ipa_ipv4_msg_callback_t one_rule_cb,
	ipa_ipv4_msg_callback_t many_rules_cb, void *app_data)
{
	struct ipa_ctx_instance_internal *ipa_be_ctx = &__ipa_be_ctx;

	IPA_BE_DBG("ECMIPA ipa_ipv4_notify_register called\n");
	IPA_BE_DBG("ECMIPA DBG ipa_ipv4_notify_register called\n");
	spin_lock_bh(&ipa_be_ctx->lock);
	/*
	 * Hook the shortcut sync callback.
	 */
	if (one_rule_cb && !ipa_be_ctx->ipv4_stats_sync_cb) {
		//ipa_ipv4_register_sync_rule_callback(ipa_ipv4_stats_one_sync_callback);
	}
	rcu_assign_pointer(ipa_be_ctx->ipv4_stats_sync_cb, one_rule_cb);

	if (many_rules_cb && !ipa_be_ctx->ipv4_stats_sync_many_cb) {
		//ipa_ipv4_register_many_sync_callback(ipa_ipv4_stats_many_sync_callback);
	}
	rcu_assign_pointer(ipa_be_ctx->ipv4_stats_sync_many_cb, many_rules_cb);

	ipa_be_ctx->ipv4_stats_sync_data = app_data;

	spin_unlock_bh(&ipa_be_ctx->lock);

	IPA_BE_DBG("ECMIPA exit ipa_ipv4_notify_register called ipa_be_ctx %p \n", ipa_be_ctx);
	return IPA_CTX_TO_PUBLIC(ipa_be_ctx);
}
EXPORT_SYMBOL(ipa_ipv4_notify_register);

/**
 * ipa_ipv6_notify_register()
 *	Register a notifier callback for IPv4 messages from IPA
 *
 * @param cb The callback pointer
 * @param app_data The application context for this message
 *
 * @return struct ipa_ctx_instance * The IPA context
 */
struct ipa_ctx_instance *ipa_ipv6_notify_register(ipa_ipv6_msg_callback_t one_rule_cb,
	ipa_ipv6_msg_callback_t many_rules_cb, void *app_data)
{
	struct ipa_ctx_instance_internal *ipa_be_ctx = &__ipa_be_ctx;

	IPA_BE_DBG("ECMIPA ipa_ipv6_notify_register called\n");
	IPA_BE_DBG("ECMIPA DBG ipa_ipv6_notify_register called\n");
	spin_lock_bh(&ipa_be_ctx->lock);
	/*
	 * Hook the shortcut sync callback.
	 */
	if (one_rule_cb && !ipa_be_ctx->ipv6_stats_sync_cb) {
		//ipa_ipv4_register_sync_rule_callback(ipa_ipv4_stats_one_sync_callback);
	}
	rcu_assign_pointer(ipa_be_ctx->ipv6_stats_sync_cb, one_rule_cb);

	if (many_rules_cb && !ipa_be_ctx->ipv6_stats_sync_many_cb) {
		//ipa_ipv4_register_many_sync_callback(ipa_ipv4_stats_many_sync_callback);
	}
	rcu_assign_pointer(ipa_be_ctx->ipv6_stats_sync_many_cb, many_rules_cb);

	ipa_be_ctx->ipv6_stats_sync_data = app_data;

	spin_unlock_bh(&ipa_be_ctx->lock);

	IPA_BE_DBG("ECMIPA exit ipa_ipv6_notify_register called ipa_be_ctx %p \n", ipa_be_ctx);
	return IPA_CTX_TO_PUBLIC(ipa_be_ctx);
}
EXPORT_SYMBOL(ipa_ipv6_notify_register);


/**
 * ipa_ipv4_notify_unregister()
 *	Un-Register the notifier callback for IPv4 messages from IPA
 */
void ipa_ipv4_notify_unregister(void)
{
	struct ipa_ctx_instance_internal *ipa_be_ctx = &__ipa_be_ctx;

	IPA_BE_DBG("ECMIPA DBG ipa_ipv4_notify_unregister called\n");

	spin_lock_bh(&ipa_be_ctx->lock);

	// Clear the registered callbacks and app data
	rcu_assign_pointer(ipa_be_ctx->ipv4_stats_sync_cb, NULL);
	rcu_assign_pointer(ipa_be_ctx->ipv4_stats_sync_many_cb, NULL);
	ipa_be_ctx->ipv4_stats_sync_data = NULL;

	spin_unlock_bh(&ipa_be_ctx->lock);

	IPA_BE_DBG("ECMIPA exit ipa_ipv4_notify_unregister called ipa_be_ctx %p \n", ipa_be_ctx);

	return;
}
EXPORT_SYMBOL(ipa_ipv4_notify_unregister);


/**
 * ipa_ipv6_notify_unregister()
 *	Un-Register the notifier callback for IPv6 messages from IPA
 */
void ipa_ipv6_notify_unregister(void)
{
	struct ipa_ctx_instance_internal *ipa_be_ctx = &__ipa_be_ctx;

	IPA_BE_DBG("ECMIPA ipa_ipv6_notify_unregister called\n");
	IPA_BE_DBG("ECMIPA DBG ipa_ipv6_notify_unregister called\n");

	spin_lock_bh(&ipa_be_ctx->lock);
	// Clear the registered callbacks and app data
	rcu_assign_pointer(ipa_be_ctx->ipv6_stats_sync_cb, NULL);
	rcu_assign_pointer(ipa_be_ctx->ipv6_stats_sync_many_cb, NULL);
	ipa_be_ctx->ipv6_stats_sync_data = NULL;
	spin_unlock_bh(&ipa_be_ctx->lock);

	IPA_BE_DBG("ECMIPA exit ipa_ipv6_notify_unregister called ipa_be_ctx %p \n", ipa_be_ctx);

	return;
}
EXPORT_SYMBOL(ipa_ipv6_notify_unregister);



/**
 * ipa_cmn_msg_init()
 *	Initialize the common message structure.
 *
 * @param ncm message to init
 * @param if_num interface number related with this message
 * @param type message type
 * @param cb callback function to process repsonse of this message
 * @param app_data argument for above callback function
 */
static void ipa_cmn_msg_init(struct ipa_cmn_msg *ncm, u16 if_num, u32 type,  u32 len, void *cb, void *app_data)
{
	ncm->interface = if_num;
	ncm->version = IPA_MESSAGE_VERSION;
	ncm->type = type;
	ncm->len = len;
	ncm->cb = (ipa_ptr_t)cb;
	ncm->app_data = (ipa_ptr_t)app_data;
}

/**
 * ipa_ipv4_msg_init()
 *	Initialize IPv4 message.
 */
void ipa_ipv4_msg_init(struct ipa_ipv4_msg *nim, u16 if_num, u32 type, u32 len,
	ipa_ipv4_msg_callback_t cb, void *app_data)
{
	ipa_cmn_msg_init(&nim->cm, if_num, type, len, (void *)cb, app_data);
}
EXPORT_SYMBOL(ipa_ipv4_msg_init);


/**
 * ipa_ipv6_msg_init()
 *	Initialize IPv6 message.
 */
void ipa_ipv6_msg_init(struct ipa_ipv6_msg *nim, u16 if_num, u32 type, u32 len,
	ipa_ipv6_msg_callback_t cb, void *app_data)
{
	ipa_cmn_msg_init(&nim->cm, if_num, type, len, (void *)cb, app_data);
}
EXPORT_SYMBOL(ipa_ipv6_msg_init);


int ipa_be_ipv4_get_max_accel_conn(void)
{
	return ipa3_ctx->max_ipv4_accel_conn;
}
EXPORT_SYMBOL(ipa_be_ipv4_get_max_accel_conn);

int ipa_be_ipv6_get_max_accel_conn(void)
{
	return ipa3_ctx->max_ipv6_accel_conn;
}
EXPORT_SYMBOL(ipa_be_ipv6_get_max_accel_conn);

int ipa_be_get_max_timeout(void)
{
	return IPA_MAX_TIMEOUT;
}
EXPORT_SYMBOL(ipa_be_get_max_timeout);


/**
 * ipa_be_init_if - Initializes the IPA Backend context and related resources.
 *
 * This function is responsible for initializing various components required
 * by the IPA (IP Accelerator) backend. It performs the following operations:
 * - Initializes a spinlock for the IPA backend context to protect shared data.
 * - Initializes a message queue (linked list) for handling responses.
 * - Initializes a work item to process response messages asynchronously.
 * - Initializes the client database mapping.
 * - Creates and initializes two ordered workqueues:
 *   - `ipa_ipv4_wq`: For processing IPv4 related tasks.
 *   - `ipa_ipv6_wq`: For processing IPv6 related tasks.
 *   Both workqueues are allocated with `WQ_MEM_RECLAIM` flag, allowing memory
 *   reclaim if necessary.
 * - Attempts to raise the priority of both IPv4 and IPv6 workqueues.
 *
 * @return 0 on successful initialization, or a negative errno code if
 *         memory allocation for workqueues fails.
 */
int ipa_be_init_if(void)
{
	struct ipa_ctx_instance_internal *ipa_be_ctx = &__ipa_be_ctx;

	IPA_BE_DBG("ECMIPA entry ipa_be_init_if \n");

	spin_lock_init(&ipa_be_ctx->lock);

	INIT_LIST_HEAD(&ipa_be_ctx->msg_queue);
	INIT_WORK(&ipa_be_ctx->work, ipa_process_response_msg);

	ipa_clientdb_mapping_init();

	ipa_be_ctx->ipa_ipv4_wq = alloc_ordered_workqueue("ipa_ipv4_wq", WQ_MEM_RECLAIM | WQ_HIGHPRI);
	if (!ipa_be_ctx->ipa_ipv4_wq) {
		IPA_BE_ERR("Failed to create IPA IPv4 ordered workqueue\n");
		return -ENOMEM;
	}

	ipa_be_ctx->ipa_ipv6_wq = alloc_ordered_workqueue("ipa_ipv6_wq", WQ_MEM_RECLAIM | WQ_HIGHPRI);
	if (!ipa_be_ctx->ipa_ipv6_wq) {
		IPA_BE_ERR("Failed to create IPA IPv6 ordered workqueue\n");
		destroy_workqueue(ipa_be_ctx->ipa_ipv4_wq);
		ipa_be_ctx->ipa_ipv4_wq = NULL;
		return -ENOMEM;
	}

#ifdef CONFIG_ECM_CONVERGENCE
	if (ipa_be_nat_mgmt_init(&__ipa_be_ctx)) {
		IPA_BE_ERR("failed be_nat_mgmt_init\n");
	}
#else
	IPA_BE_DBG("NAT support disabled - skipping ipa_be_nat_mgmt_init\n");
#endif

	/*
	 * Register the always-on private-subnet notifiers (defer work to the
	 * IPv4/IPv6 ordered workqueues). Non-fatal: on failure init rolls back
	 * and operation continues without the subnet rules.
	 */
	if (ipa_be_subnet_notifier_init(ipa_be_ctx->ipa_ipv4_wq,
		ipa_be_ctx->ipa_ipv6_wq))
		IPA_BE_ERR("failed to register private-subnet notifiers\n");

	IPA_BE_DBG("ECMIPA exit ipa_be_init_if \n");

	return 0;
}
EXPORT_SYMBOL(ipa_be_init_if);

/**
 * ipa_be_exit_if - De-initialize the IPA backend module.
 *
 * This function is responsible for de-initializing the IPA (IP Accelerator)
 * backend context. It cancels and synchronizes any pending work, processes
 * remaining response messages, and destroys the workqueues used for IPv4
 * and IPv6 processing if they exist.
 *
 * Context:
 * This function is typically called during the teardown phase of the IPA
 * backend, for example, when the module is being unloaded.
 *
 * Returns:
 * 0 on successful de-initialization.
 */
int ipa_be_exit_if(void)
{
	struct ipa_ctx_instance_internal *ipa_be_ctx = &__ipa_be_ctx;

	IPA_BE_DBG("ECMIPA entry ipa_be_exit_if \n");

	/*
	 * Stop the private-subnet notifiers first so no new reconcile work is
	 * queued while we drain and destroy the workqueues below.
	 */
	ipa_be_subnet_notifier_deinit();

#ifdef CONFIG_ECM_CONVERGENCE
	ipa_be_nat_mgmt_exit();
#else
	IPA_BE_DBG("NAT support disabled - skipping ipa_be_nat_mgmt_exit\n");
#endif
	/*
	 * stop work queue, and flush all pending message in queue
	 */

	cancel_work_sync(&ipa_be_ctx->work);
	ipa_process_response_msg(&ipa_be_ctx->work);

	if (ipa_be_ctx->ipa_ipv4_wq)
	{
		destroy_workqueue(ipa_be_ctx->ipa_ipv4_wq);
		ipa_be_ctx->ipa_ipv4_wq = NULL;
	}

	if (ipa_be_ctx->ipa_ipv6_wq)
	{
		destroy_workqueue(ipa_be_ctx->ipa_ipv6_wq);
		ipa_be_ctx->ipa_ipv6_wq = NULL;
	}

	/*
	 * Workqueues are drained/destroyed above, so no reconcile work can run
	 * now; safe to free the private-subnet tracking state.
	 */
	ipa_be_subnet_notifier_cleanup();

	IPA_BE_DBG("ECMIPA exit ipa_be_exit_if \n");
	return 0;
}
EXPORT_SYMBOL(ipa_be_exit_if);


/*
 * ipa_is_l2_feature_enabled()
 * Check if l2 features flag feature is enabled or not. (VLAN, PPPOE, BRIDGE and tunnels)
 * 32bit read is atomic. No need of locks.
 */
bool ipa_is_l2_feature_enabled(void)
{
	struct ipa_ctx_instance_internal *ipa_ctx = &__ipa_be_ctx;

	return (ipa_ctx->l2_feature_support == 1);
}
EXPORT_SYMBOL(ipa_is_l2_feature_enabled);

/**
 * ipa_be_qos_dscp_mark_proc_ctx - Create a DSCP marking header processing
 * context for QoS route rules
 * @hdr_hdl: Header handle to use
 * @dscp_mark_val: DSCP value to mark
 * @proc_ctx_hdl_out: Output parameter for the proc_ctx handle
 *
 * Return: 0 on success, negative errno on failure
 */
static int ipa_be_qos_dscp_mark_proc_ctx(int hdr_hdl, uint8_t dscp_mark_val,
					  int *proc_ctx_hdl_out)
{
	struct ipa_ioc_add_hdr_proc_ctx *hdr_proc_ctx_table;
	struct ipa_hdr_proc_ctx_add *hdr_proc_ctx;
	int size;
	int handle;

	if (!proc_ctx_hdl_out) {
		IPA_BE_ERR("NULL output parameter\n");
		return -EINVAL;
	}

	size = sizeof(struct ipa_ioc_add_hdr_proc_ctx) +
	       sizeof(struct ipa_hdr_proc_ctx_add);
	hdr_proc_ctx_table = (struct ipa_ioc_add_hdr_proc_ctx *)kzalloc(size, GFP_KERNEL);
	if (!hdr_proc_ctx_table) {
		IPA_BE_ERR("Failed to allocate memory for QoS hdr_proc_ctx\n");
		return -ENOMEM;
	}

	hdr_proc_ctx_table->commit = 1;
	hdr_proc_ctx_table->num_proc_ctxs = 1;
	hdr_proc_ctx = &hdr_proc_ctx_table->proc_ctx[0];

	hdr_proc_ctx->type = IPA_HDR_PROC_MARK_DSCP;
	hdr_proc_ctx->hdr_hdl = hdr_hdl;
	hdr_proc_ctx->pdn_dscp_params.valid = 1;
	hdr_proc_ctx->pdn_dscp_params.dscp_val = dscp_mark_val;

	if (ipa3_add_hdr_proc_ctx(hdr_proc_ctx_table, true)) {
		IPA_BE_ERR("Failed to add QoS DSCP marking hdr_proc_ctx\n");
		kfree(hdr_proc_ctx_table);
		return -EFAULT;
	}

	handle = hdr_proc_ctx_table->proc_ctx[0].proc_ctx_hdl;
	IPA_BE_DBG("Installed QoS DSCP marking proc_ctx: hdl %d dscp_val %u base_hdr_hdl %d\n",
		   handle, dscp_mark_val, hdr_hdl);

	*proc_ctx_hdl_out = handle;
	kfree(hdr_proc_ctx_table);
	return 0;
}

/**
 * ipa_be_qos_install_one_rule - Build and install a single QoS routing rule.
 * @qos_param:      QoS configuration (ip_type, dev_name, protocol, ports, DSCP,
 *                  VLAN, and src/dst IPs when not overridden by @client_addr).
 * @tx_prop:        TX interface properties already queried by the caller.
 * @tx_index:       Selected TX pipe index within @tx_prop.
 * @client_hdr_hdl: Ethernet header handle for this client (0 or less = none).
 * @client_addr:    Client IP in network byte order to use as dst_addr.
 *                  Pass a zero-initialised array to fall back to @qos_param's IP.
 * @src_mac_addr:   Source MAC address to add as a HW filter attribute, or NULL
 *                  to omit the src MAC filter.
 * @dst_mac_addr:   Destination MAC address (currently not added as a HW filter
 *                  attribute — pass for future use or documentation purposes).
 *
 * Return: 0 on success, negative errno on failure.
 */
static int ipa_be_qos_install_one_rule(
	struct ipa_ioc_qos_config *qos_param,
	struct ipa_ioc_query_intf_tx_props *tx_prop,
	int tx_index,
	int client_hdr_hdl,
	const ip_addr_t client_addr,
	const uint8_t *src_mac_addr,
	const uint8_t *dst_mac_addr)
{
	int retval = 0;
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	int proc_ctx_hdl = 0;
	int hdr_hdl = 0;
	int i;

	if (client_hdr_hdl > 0)
		hdr_hdl = client_hdr_hdl;

	if (qos_param->dscp_mark_val && hdr_hdl > 0) {
		int ctx_ret = ipa_be_qos_dscp_mark_proc_ctx(
			hdr_hdl, qos_param->dscp_mark_val, &proc_ctx_hdl);
		if (ctx_ret == 0 && proc_ctx_hdl > 0) {
			IPA_BE_DBG("Using DSCP marking proc_ctx_hdl: %d\n", proc_ctx_hdl);
		} else {
			IPA_BE_ERR("DSCP proc_ctx creation failed (%d), falling back to hdr_hdl\n",
				   ctx_ret);
		}
	} else if (qos_param->dscp_mark_val && hdr_hdl <= 0) {
		/*
		 * DSCP marking was requested but no base header handle is
		 * available for this client (client_hdr_hdl == 0), so the
		 * MARK_DSCP proc_ctx (which rewrites on top of a base header)
		 * cannot be created. The rule still installs for routing, but
		 * WITHOUT egress DSCP rewrite. This was previously silent.
		 */
		IPA_BE_ERR("DSCP mark %u requested but no hdr_hdl (client_hdr_hdl=%d) - marking SKIPPED, rule installs without DSCP rewrite\n",
			   qos_param->dscp_mark_val, client_hdr_hdl);
	}

	rt_rule = kzalloc(sizeof(struct ipa_ioc_add_rt_rule) +
			  sizeof(struct ipa_rt_rule_add), GFP_KERNEL);
	if (!rt_rule) {
		IPA_BE_ERR("Failed to allocate rt_rule\n");
		return -ENOMEM;
	}

	rt_rule->commit = true;
	rt_rule->num_rules = 1;
	rt_rule->ip = (enum ipa_ip_type)qos_param->ip_type;

	if (qos_param->ip_type == IPA_IP_v4) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(rt_rule->rt_tbl_name, V4_LAN_ROUTE_TABLE_NAME,
			sizeof(rt_rule->rt_tbl_name));
#else
		strlcpy(rt_rule->rt_tbl_name, V4_LAN_ROUTE_TABLE_NAME,
			sizeof(rt_rule->rt_tbl_name));
#endif
	} else {
		/* Use WANRTBLv6, not COMRTBLv6 -- nothing routes DL traffic to COMRTBLv6, so a QoS rule there is unreachable (same bug as bec260bd9). */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(rt_rule->rt_tbl_name, V6_WAN_ROUTE_TABLE_NAME,
			sizeof(rt_rule->rt_tbl_name));
#else
		strlcpy(rt_rule->rt_tbl_name, V6_WAN_ROUTE_TABLE_NAME,
			sizeof(rt_rule->rt_tbl_name));
#endif
	}
	rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX - 1] = '\0';

	rt_rule_entry = &rt_rule->rules[0];
	rt_rule_entry->at_rear = false;
	/*
	 * Categorize as a QoS rule so it always sorts ahead of the client's
	 * default LAN route rule. Route-rule ordering in the kernel is by
	 * rt_rule_category first (ascending): IPA_RT_RULE_CAT_QOS (1) sorts
	 * before IPA_RT_RULE_CAT_CLIENT (3), which is what the default LAN
	 * rule uses. Without this the QoS rule defaults to CAT_NONE (0) and,
	 * per __ipa_add_rt_rule's cat==0 special case, gets placed BEHIND the
	 * categorized client rule regardless of at_rear/insertion order. This
	 * mirrors how ipa_be_clientdb.c tags CLIENT/LAN2LAN rules and is
	 * stable across later client-rule adds (no re-seating needed).
	 */
	rt_rule_entry->rt_rule_category = IPA_RT_RULE_CAT_QOS;
	/*
	 * Map (flt_cat, traffic_class) -> sub_cat: __ipa_add_rt_rule() ranks
	 * ascending sub_cat first, so user rules (IPA_RULE_PRIORITY_0 + tc)
	 * always outrank autonomous rules (IPA_RULE_SUB_CAT_MAX + tc), and TC0
	 * outranks TC15 within each band. The autonomous band intentionally
	 * lies outside the named enum -- do not add a sub_cat range check
	 * without reworking this. traffic_class is unchecked upstream; clamp
	 * to the band's last slot.
	 */
	{
		unsigned int band_base =
			(qos_param->flt_cat == IPA_QOS_FLT_AUTO) ?
			IPA_RULE_SUB_CAT_MAX :
			IPA_RULE_PRIORITY_0;

		if (qos_param->traffic_class < IPA_MAX_TRAFFIC_CLASSES)
			rt_rule_entry->rule_sub_category =
				band_base + qos_param->traffic_class;
		else
			rt_rule_entry->rule_sub_category =
				band_base + (IPA_MAX_TRAFFIC_CLASSES - 1);
	}
	rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
	memcpy(&rt_rule_entry->rule.attrib, &tx_prop->tx[tx_index].attrib,
	       sizeof(rt_rule_entry->rule.attrib));

	if (proc_ctx_hdl > 0)
		rt_rule_entry->rule.hdr_proc_ctx_hdl = proc_ctx_hdl;
	else if (hdr_hdl > 0)
		rt_rule_entry->rule.hdr_hdl = hdr_hdl;

	if (qos_param->ip_type == IPA_IP_v4) {
		/* Client IP is always set as dst_addr to uniquely scope the rule
		 * per client and prevent header collisions (mirrors IPACM logic). */
		rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		rt_rule_entry->rule.attrib.u.v4.dst_addr = ntohl(client_addr[0]);
		rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;

		if (qos_param->src_ip_addr) {
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_SRC_ADDR;
			rt_rule_entry->rule.attrib.u.v4.src_addr =
				qos_param->src_ip_addr;
			rt_rule_entry->rule.attrib.u.v4.src_addr_mask =
				qos_param->src_subnet ?
				qos_param->src_subnet : 0xFFFFFFFF;
		}

		if (qos_param->protocol) {
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_PROTOCOL;
			rt_rule_entry->rule.attrib.u.v4.protocol = qos_param->protocol;
		}

		if (qos_param->dscp) {
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_TOS_MASKED;
			rt_rule_entry->rule.attrib.tos_value = qos_param->dscp << 2;
			rt_rule_entry->rule.attrib.tos_mask = 0xFC;
			rt_rule_entry->rule.hashable = false;
		} else {
			rt_rule_entry->rule.hashable = true;
		}

	} else { /* IPA_IP_v6 */
		/* Always set client's IPv6 addr as dst_addr. */
		rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		for (i = 0; i < 4; i++) {
			rt_rule_entry->rule.attrib.u.v6.dst_addr[i] =
				ntohl(client_addr[i]);
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[i] = 0xFFFFFFFF;
		}

		if (qos_param->src_v6_ip_addr[0] || qos_param->src_v6_ip_addr[1] ||
		    qos_param->src_v6_ip_addr[2] || qos_param->src_v6_ip_addr[3]) {
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_SRC_ADDR;
			for (i = 0; i < 4; i++) {
				rt_rule_entry->rule.attrib.u.v6.src_addr[i] =
					qos_param->src_v6_ip_addr[i];
				rt_rule_entry->rule.attrib.u.v6.src_addr_mask[i] =
					qos_param->src_v6_ip_subnet[i] ?
					qos_param->src_v6_ip_subnet[i] :
					0xFFFFFFFF;
			}
		}

		if (qos_param->protocol) {
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_NEXT_HDR;
			rt_rule_entry->rule.attrib.u.v6.next_hdr = qos_param->protocol;
		}

		if (qos_param->dscp) {
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_TOS_MASKED;
			rt_rule_entry->rule.attrib.tos_value = qos_param->dscp << 2;
			rt_rule_entry->rule.attrib.tos_mask = 0xFC;
			rt_rule_entry->rule.hashable = false;
		} else {
			rt_rule_entry->rule.hashable = true;
		}
	}

	if (qos_param->src_port_start) {
		if (!qos_param->src_port_end ||
		    qos_param->src_port_start == qos_param->src_port_end) {
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_SRC_PORT;
			rt_rule_entry->rule.attrib.src_port = qos_param->src_port_start;
		} else {
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_SRC_PORT_RANGE;
			rt_rule_entry->rule.attrib.src_port_lo = qos_param->src_port_start;
			rt_rule_entry->rule.attrib.src_port_hi = qos_param->src_port_end;
		}
	}

	if (qos_param->dst_port_start) {
		if (!qos_param->dst_port_end ||
		    qos_param->dst_port_start == qos_param->dst_port_end) {
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_PORT;
			rt_rule_entry->rule.attrib.dst_port = qos_param->dst_port_start;
		} else {
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_PORT_RANGE;
			rt_rule_entry->rule.attrib.dst_port_lo = qos_param->dst_port_start;
			rt_rule_entry->rule.attrib.dst_port_hi = qos_param->dst_port_end;
		}
	}

	if (qos_param->vlan_count > 0) {
		rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_VLAN_ID;
		rt_rule_entry->rule.attrib.vlan_id = qos_param->vlan_ids[0];
	}

	if (src_mac_addr &&
	    memcmp(src_mac_addr, (uint8_t[ETH_ALEN]){0}, ETH_ALEN) != 0) {
		rt_rule_entry->rule.attrib.attrib_mask |=
			IPA_FLT_MAC_SRC_ADDR_ETHER_II;
		memcpy(rt_rule_entry->rule.attrib.src_mac_addr,
		       src_mac_addr, ETH_ALEN);
		memset(rt_rule_entry->rule.attrib.src_mac_addr_mask,
		       0xFF, ETH_ALEN);
	}

	/*
	 * dst_mac_addr is available but not currently added as a HW filter
	 * attribute. It can be enabled by setting:
	 *   attrib_mask |= IPA_FLT_MAC_DST_ADDR_ETHER_II
	 *   memcpy(attrib.dst_mac_addr, dst_mac_addr, ETH_ALEN)
	 *   memset(attrib.dst_mac_addr_mask, 0xFF, ETH_ALEN)
	 */
	(void)dst_mac_addr;

	/*
	 * Dump the fully-built rule right before commit so the exact match
	 * tuple, target pipe, priority and DSCP-marking action are visible in
	 * the logs (proc_ctx_hdl != 0 means a MARK_DSCP hdr_proc_ctx is
	 * attached; hdr_hdl is the plain L2 header otherwise). Addresses are
	 * stored in host byte order in the rule; htonl() into temps so the
	 * %pI4/%pI6 printers render them as dotted quads (mirrors ip4_rt).
	 */
	if (qos_param->ip_type == IPA_IP_v4) {
		__be32 s_addr = htonl(rt_rule_entry->rule.attrib.u.v4.src_addr);
		__be32 s_mask = htonl(rt_rule_entry->rule.attrib.u.v4.src_addr_mask);
		__be32 d_addr = htonl(rt_rule_entry->rule.attrib.u.v4.dst_addr);
		__be32 d_mask = htonl(rt_rule_entry->rule.attrib.u.v4.dst_addr_mask);

		IPA_BE_DBG("QoS rt rule (v4): dst_pipe %d attrib_mask 0x%x cat %u "
			   "sub_cat %u flt_cat %u tc %u max_prio %d "
			   "src_addr %pI4/%pI4 dst_addr %pI4/%pI4 proto %u "
			   "src_port %u dst_port %u tos_val 0x%x tos_mask 0x%x\n",
			   rt_rule_entry->rule.dst,
			   rt_rule_entry->rule.attrib.attrib_mask,
			   rt_rule_entry->rt_rule_category,
			   rt_rule_entry->rule_sub_category,
			   qos_param->flt_cat,
			   qos_param->traffic_class,
			   rt_rule_entry->rule.max_prio,
			   &s_addr, &s_mask, &d_addr, &d_mask,
			   rt_rule_entry->rule.attrib.u.v4.protocol,
			   rt_rule_entry->rule.attrib.src_port,
			   rt_rule_entry->rule.attrib.dst_port,
			   rt_rule_entry->rule.attrib.tos_value,
			   rt_rule_entry->rule.attrib.tos_mask);
	} else {
		__be32 s6[4], d6[4];
		int k;

		for (k = 0; k < 4; k++) {
			s6[k] = htonl(rt_rule_entry->rule.attrib.u.v6.src_addr[k]);
			d6[k] = htonl(rt_rule_entry->rule.attrib.u.v6.dst_addr[k]);
		}
		IPA_BE_DBG("QoS rt rule (v6): dst_pipe %d attrib_mask 0x%x cat %u "
			   "sub_cat %u flt_cat %u tc %u max_prio %d "
			   "src_addr %pI6 dst_addr %pI6 next_hdr %u "
			   "src_port %u dst_port %u tos_val 0x%x tos_mask 0x%x\n",
			   rt_rule_entry->rule.dst,
			   rt_rule_entry->rule.attrib.attrib_mask,
			   rt_rule_entry->rt_rule_category,
			   rt_rule_entry->rule_sub_category,
			   qos_param->flt_cat,
			   qos_param->traffic_class,
			   rt_rule_entry->rule.max_prio,
			   s6, d6,
			   rt_rule_entry->rule.attrib.u.v6.next_hdr,
			   rt_rule_entry->rule.attrib.src_port,
			   rt_rule_entry->rule.attrib.dst_port,
			   rt_rule_entry->rule.attrib.tos_value,
			   rt_rule_entry->rule.attrib.tos_mask);
	}
	IPA_BE_DBG("QoS rt rule action: hashable %d dscp_mark_val %u "
		   "proc_ctx_hdl %d hdr_hdl %d\n",
		   rt_rule_entry->rule.hashable,
		   qos_param->dscp_mark_val,
		   rt_rule_entry->rule.hdr_proc_ctx_hdl,
		   rt_rule_entry->rule.hdr_hdl);

	/* Share IPA_SHARED_RT_RULE_ID across all client route rules instead of
	 * auto-allocating one each. */
	if (ipa3_add_rt_rule_with_id((struct ipa_ioc_add_rt_rule *)rt_rule,
				     true, IPA_SHARED_RT_RULE_ID)) {
		IPA_BE_ERR("QoS route rule add failed for %s\n", qos_param->dev_name);
		retval = -EFAULT;
	} else {
		qos_param->qos_rule_hdl = rt_rule->rules[0].rt_rule_hdl;
		qos_param->proc_ctx_hdl = proc_ctx_hdl;
		IPA_BE_DBG("QoS route rule installed, hdl: %d\n", qos_param->qos_rule_hdl);
		/* Tighten this interface's BE pipes if this is the first rule. */
		ipa_be_qos_retime_on_rule_add(qos_param->dev_name);
	}

	if (retval && proc_ctx_hdl > 0) {
		/* Use stack allocation so cleanup always succeeds regardless
		 * of memory pressure. */
		struct {
			struct ipa_ioc_del_hdr_proc_ctx header;
			struct ipa_hdr_proc_ctx_del     hdl_entry;
		} del_ctx_buf = {};
		struct ipa_ioc_del_hdr_proc_ctx *del_ctx = &del_ctx_buf.header;

		del_ctx->commit    = true;
		del_ctx->num_hdls  = 1;
		del_ctx->hdl[0].hdl = proc_ctx_hdl;
		if (ipa3_del_hdr_proc_ctx(del_ctx))
			IPA_BE_ERR("Failed to delete QoS proc_ctx hdl %d on error path\n",
				   proc_ctx_hdl);
	}

	kfree(rt_rule);
	return retval;
}

/*
 * ipa_be_qos_resolve_tx_pipe() - Resolve the TX pipe a client's QoS rule
 * should target, from the interface it is actually accelerated on.
 * @iface_name: interface name from the client's ClientDB entry (not from
 *              QoS_CLI/HAL -- the driver never trusts a caller-supplied
 *              dev_name for pipe selection). May be empty.
 * @iface_num: interface index from the same ClientDB entry, used when
 *             @iface_name is empty or no longer resolves (e.g. the entry
 *             predates iface_name being recorded).
 * @ip_type: IPA_IP_v4 or IPA_IP_v6.
 * @traffic_class: requested QoS traffic class.
 * @tx_prop_out: on success, set to a kzalloc'd tx props array; caller owns it (kfree()).
 * @tx_index_out: on success, set to the selected index within *tx_prop_out.
 *
 * A pipe whose tc_bmap includes @traffic_class is preferred, matching
 * multi-queue interfaces such as ETH. If none exists, the first pipe of the
 * right ip_type is used regardless of tc_bmap: WLAN has exactly one TX pipe
 * per IP type and never tags it with a tc_bmap (its dedicated per-class QoS
 * pipes in ipa_utils.c are defined but never connected). IPACM's own WLAN
 * QoS path (IPACM_Wlan.cpp) already disables the equivalent tc_bmap gate for
 * the same reason.
 *
 * Return: 0 on success, negative errno on failure.
 */
static int ipa_be_qos_resolve_tx_pipe(const char *iface_name,
				       uint32_t iface_num,
				       enum ipa_ip_type ip_type,
				       uint8_t traffic_class,
				       struct ipa_ioc_query_intf_tx_props **tx_prop_out,
				       int *tx_index_out)
{
	struct ipa_ioc_query_intf temp_intf;
	struct ipa_ioc_query_intf_tx_props *tx_prop;
	int tx_index, fallback_index = -1;
	bool resolved = false;

	memset(&temp_intf, 0, sizeof(temp_intf));

	if (iface_name && iface_name[0]) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(temp_intf.name, iface_name, sizeof(temp_intf.name));
#else
		strlcpy(temp_intf.name, iface_name, sizeof(temp_intf.name));
#endif
		resolved = (ipa3_query_intf(&temp_intf) == 0);
		if (!resolved)
			IPA_BE_DBG("Name lookup failed for %s; trying iface index %u\n",
				   iface_name, iface_num);
	}

	/* Fall back to the index the client's rt rule was installed against.
	 * Index 0 is never used as a key -- it is the unassigned default
	 * shared by many entries, so it resolves to an arbitrary interface. */
	if (!resolved && iface_num) {
		memset(&temp_intf, 0, sizeof(temp_intf));
		resolved = ipa3_query_iface((int)iface_num, &temp_intf);
	}

	if (!resolved) {
		IPA_BE_ERR("Failed to resolve interface (name '%s', index %u)\n",
			   (iface_name && iface_name[0]) ? iface_name : "<none>",
			   iface_num);
		return -EINVAL;
	}

	if (temp_intf.num_tx_props == 0) {
		IPA_BE_ERR("Interface %s has no TX properties\n", temp_intf.name);
		return -EINVAL;
	}

	tx_prop = (struct ipa_ioc_query_intf_tx_props *)kzalloc(
		sizeof(struct ipa_ioc_query_intf_tx_props) +
		temp_intf.num_tx_props * sizeof(struct ipa_ioc_tx_intf_prop),
		GFP_KERNEL);
	if (!tx_prop)
		return -ENOMEM;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(tx_prop->name, temp_intf.name, sizeof(tx_prop->name));
#else
	strlcpy(tx_prop->name, temp_intf.name, sizeof(tx_prop->name));
#endif
	tx_prop->num_tx_props = temp_intf.num_tx_props;
	if (ipa3_query_intf_tx_props(tx_prop)) {
		IPA_BE_ERR("Failed to query TX props for interface %s\n", temp_intf.name);
		kfree(tx_prop);
		return -EINVAL;
	}

	for (tx_index = 0; tx_index < (int)temp_intf.num_tx_props; tx_index++) {
		if (tx_prop->tx[tx_index].ip != ip_type)
			continue;
		if (fallback_index < 0)
			fallback_index = tx_index;
		if (tx_prop->tx[tx_index].tc_bmap & (1U << traffic_class)) {
			*tx_prop_out = tx_prop;
			*tx_index_out = tx_index;
			return 0;
		}
	}

	if (fallback_index < 0) {
		IPA_BE_ERR("No TX pipe for ip_type %d on %s\n", ip_type, temp_intf.name);
		kfree(tx_prop);
		return -EINVAL;
	}

	IPA_BE_DBG("No tc_bmap match for tc %d on %s; falling back to pipe index %d\n",
		   traffic_class, temp_intf.name, fallback_index);
	*tx_prop_out = tx_prop;
	*tx_index_out = fallback_index;
	return 0;
}

/**
 * ipa_be_install_qos_route_rule - Install a QoS routing rule in IPA
 * @qos_param: QoS configuration parameters including IP tuple, traffic class,
 *             DSCP marking, and interface information
 *
 * Implements the same logic as ipa_ipv4_add_route_rule in ipa_be_clientdb.c
 * but selects the TX pipe based on traffic_class using tc_bmap, and adds
 * IP tuple matching attributes (src/dst IP, ports, protocol, DSCP).
 *
 * Return: 0 on success, negative errno on failure
 */
int ipa_be_install_qos_route_rule(struct ipa_ioc_qos_config *qos_param)
{
	int retval = 0;
	struct ipa_ioc_query_intf_tx_props *tx_prop = NULL;
	int tx_index = -1;
	struct ipa_clientdb_mapping_instance *mi = NULL;
	bool iterate_all_clients = false;
	bool mac_filter_active = false;
	uint8_t mac_filter[IPA_MAC_ADDR_SIZE] = {0};
	ip_addr_t mi_addr = {0};
	int mi_hdr_hdl = 0;
	/* Header handle from the explicit dst_ip path (v4/v6); separate from mi_hdr_hdl, which the MAC+VLAN lookup below populates. */
	int dst_ip_hdr_hdl = 0;
	bool found_client = false;
	/* Interface the resolved client is actually accelerated on, per
	 * ClientDB -- never the caller-supplied qos_param->dev_name. Only
	 * meaningful for the found_client/explicit-dst_ip single-client
	 * paths; iterate_all_clients resolves this per client instead.
	 */
	char iface_name[IPA_RESOURCE_NAME_MAX] = {0};
	uint32_t iface_num = 0;
	struct qos_client_entry {
		ip_addr_t addr;
		int hdr_hdl;
		char iface_name[IPA_RESOURCE_NAME_MAX];
		uint32_t iface_num;
	} *clients = NULL;

	if (!qos_param) {
		IPA_BE_ERR("NULL qos_param\n");
		return -EINVAL;
	}

	/* UL QoS rules are not supported on IPA in the ECM path. */
	if (qos_param->dir != 0) {
		IPAERR("UL QoS rule not supported on IPA\n");
		return -EOPNOTSUPP;
	}

	/*
	 * Gate rule installation on whether a matching active client exists,
	 * and determine which install path to take:
	 *  1. MAC (+ optional VLAN) specified → look up by MAC; skip if absent.
	 *  2. IP address specified            → verify client exists via IP table;
	 *                                       skip if absent.
	 *  3. No MAC/IP filter               → install per active client;
	 *                                       each rule gets that client's IP
	 *                                       baked in as dst_addr.
	 */
	if (memcmp(qos_param->dst_mac_addr,
		   (uint8_t[IPA_MAC_ADDR_SIZE]){0}, IPA_MAC_ADDR_SIZE) != 0) {
		uint16_t check_vlan = (qos_param->vlan_count > 0) ?
				      qos_param->vlan_ids[0] : 0;

		if (check_vlan) {
			/*
			 * VLAN specified: use exact MAC+VLAN hash lookup.
			 * Only the client with this specific MAC+VLAN combination
			 * gets the rule installed.
			 */
			uint32_t mi_rt_hdl = 0;

			mutex_lock(&ipa_client_db_lock);
			mi = ipa_be_clientdb_get_mapping_by_mac_locked(
				qos_param->dst_mac_addr, check_vlan);
			if (mi) {
				memcpy(mi_addr, mi->address, sizeof(ip_addr_t));
				mi_hdr_hdl = (int)mi->lan2wan_info.hdr_hdl;
				mi_rt_hdl = mi->lan2wan_info.rt_hdl;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
				strscpy(iface_name, mi->iface_name, sizeof(iface_name));
#else
				strlcpy(iface_name, mi->iface_name, sizeof(iface_name));
#endif
				iface_num = mi->iface_num;
				found_client = true;
			}
			mutex_unlock(&ipa_client_db_lock);

			if (found_client) {
				IPA_BE_DBG("Client MAC %pM vlan %u already accelerated "
					   "(lan2wan rt_hdl %u); installing QoS rule on top\n",
					   qos_param->dst_mac_addr, check_vlan, mi_rt_hdl);
			} else {
				IPA_BE_DBG("Client MAC %pM vlan %u not yet accelerated\n",
					   qos_param->dst_mac_addr, check_vlan);
				goto cleanup;
			}
		} else {
			/*
			 * No VLAN: install for ALL active clients with this MAC,
			 * regardless of their VLAN. Use the iterate_all_clients
			 * path with a MAC filter applied during iteration.
			 */
			iterate_all_clients = true;
			mac_filter_active = true;
			memcpy(mac_filter, qos_param->dst_mac_addr, IPA_MAC_ADDR_SIZE);
		}
	} else if (qos_param->ip_type == IPA_IP_v4) {
		if (qos_param->dst_ip_addr) {
			int ip_rt_hdl;
			int ip_hdr_hdl = -1, ip_proc_ctx_hdl = -1;
			char ip_proc_ctx_name[32] = {0};
			ip_addr_t lookup_addr = {0};
			uint32_t src_ip_be = htonl(qos_param->src_ip_addr);

			lookup_addr[0] = htonl(qos_param->dst_ip_addr);

			IPA_BE_DBG("QoS IP check: client(dst_ip)=%pI4n rule src=%pI4n dst=%pI4n\n",
				   &lookup_addr[0],
				   &src_ip_be,
				   &lookup_addr[0]);

			ip_rt_hdl = ipa_get_rt_hdl_from_mapping(
					lookup_addr,
					false /* lan2wan */,
					&ip_hdr_hdl, &ip_proc_ctx_hdl,
					ip_proc_ctx_name);
			if (ip_rt_hdl > 0) {
				/* Retain the header handle -- without it the rule installs with hdr_ofst 0 (no L2 header) and DSCP marking can't apply. */
				if (ip_hdr_hdl > 0)
					dst_ip_hdr_hdl = ip_hdr_hdl;

				mutex_lock(&ipa_client_db_lock);
				mi = ipa_be_clientdb_get_mapping_by_ip_locked(lookup_addr);
				if (mi) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
					strscpy(iface_name, mi->iface_name, sizeof(iface_name));
#else
					strlcpy(iface_name, mi->iface_name, sizeof(iface_name));
#endif
					iface_num = mi->iface_num;
				}
				mutex_unlock(&ipa_client_db_lock);

				IPA_BE_DBG("IPv4 client IP %pI4n found in mapping "
					   "(rt_hdl %d hdr_hdl %d iface %s); installing QoS rule\n",
					   &lookup_addr[0], ip_rt_hdl, ip_hdr_hdl, iface_name);
				if (qos_param->src_ip_addr)
					IPA_BE_DBG("src_ip=%pI4n also set as filter attribute\n",
						   &src_ip_be);
			} else {
				IPA_BE_ERR("IPv4 client IP %pI4n NOT found in mapping table (rt_hdl=%d), skipping\n",
					   &lookup_addr[0], ip_rt_hdl);
				goto cleanup;
			}
		} else {
			/* No dst_ip filter → catch-all (src_ip, if set, is a filter attr). */
			iterate_all_clients = true;
		}
	} else if (qos_param->ip_type == IPA_IP_v6) {
		static const uint32_t null_v6[4] = {0};
		bool has_dst_v6 = memcmp(qos_param->dst_v6_ip_addr, null_v6,
					 sizeof(null_v6)) != 0;

		if (has_dst_v6) {
			int ip_rt_hdl;
			int ip_hdr_hdl = -1, ip_proc_ctx_hdl = -1;
			char ip_proc_ctx_name[32] = {0};
			uint32_t dst_v6_be[4];
			uint32_t src_v6_be[4];
			int idx;

			for (idx = 0; idx < 4; idx++) {
				dst_v6_be[idx] = htonl(qos_param->dst_v6_ip_addr[idx]);
				src_v6_be[idx] = htonl(qos_param->src_v6_ip_addr[idx]);
			}

			IPA_BE_DBG("QoS IP check: client(dst_ip)=%pI6n rule src=%pI6n dst=%pI6n\n",
				   dst_v6_be, src_v6_be, dst_v6_be);

			ip_rt_hdl = ipa_get_rt_hdl_from_mapping(
					dst_v6_be,
					false /* lan2wan */,
					&ip_hdr_hdl, &ip_proc_ctx_hdl,
					ip_proc_ctx_name);
			if (ip_rt_hdl > 0) {
				/* Same rationale as the v4 path above. */
				if (ip_hdr_hdl > 0)
					dst_ip_hdr_hdl = ip_hdr_hdl;

				mutex_lock(&ipa_client_db_lock);
				mi = ipa_be_clientdb_get_mapping_by_ip_locked(dst_v6_be);
				if (mi) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
					strscpy(iface_name, mi->iface_name, sizeof(iface_name));
#else
					strlcpy(iface_name, mi->iface_name, sizeof(iface_name));
#endif
					iface_num = mi->iface_num;
				}
				mutex_unlock(&ipa_client_db_lock);

				IPA_BE_DBG("IPv6 client IP %pI6n found in mapping "
					   "(rt_hdl %d hdr_hdl %d iface %s); installing QoS rule\n",
					   dst_v6_be, ip_rt_hdl, ip_hdr_hdl, iface_name);
			} else {
				IPA_BE_ERR("IPv6 client IP %pI6n NOT found in mapping table (rt_hdl=%d), skipping\n",
					   dst_v6_be, ip_rt_hdl);
				goto cleanup;
			}
		} else {
			/* No dst_v6 IP filter → catch-all. */
			iterate_all_clients = true;
		}
	} else {
		/* No specific MAC/IP filter: install per active client. */
		iterate_all_clients = true;
	}

	IPA_BE_DBG("Entry ipa_be_install_qos_route_rule dev_name %s dir %u ip_type %d tc %d "
		"iface_cat %d flt_cat %d protocol %u dscp %u pcp %u dscp_mark_val %u\n",
		qos_param->dev_name, qos_param->dir, qos_param->ip_type,
		qos_param->traffic_class, qos_param->iface_cat, qos_param->flt_cat,
		qos_param->protocol, qos_param->dscp, qos_param->pcp,
		qos_param->dscp_mark_val);
	IPA_BE_DBG("QoS add params: src_ip 0x%x/0x%x dst_ip 0x%x/0x%x "
		"src_port [%u-%u] dst_port [%u-%u]\n",
		qos_param->src_ip_addr, qos_param->src_subnet,
		qos_param->dst_ip_addr, qos_param->dst_subnet,
		qos_param->src_port_start, qos_param->src_port_end,
		qos_param->dst_port_start, qos_param->dst_port_end);
	IPA_BE_DBG("QoS add params: src_v6 %08x:%08x:%08x:%08x dst_v6 %08x:%08x:%08x:%08x\n",
		qos_param->src_v6_ip_addr[0], qos_param->src_v6_ip_addr[1],
		qos_param->src_v6_ip_addr[2], qos_param->src_v6_ip_addr[3],
		qos_param->dst_v6_ip_addr[0], qos_param->dst_v6_ip_addr[1],
		qos_param->dst_v6_ip_addr[2], qos_param->dst_v6_ip_addr[3]);
	IPA_BE_DBG("QoS add params: src_mac %pM dst_mac %pM vlan_count %u vlan[0] %u\n",
		qos_param->src_mac_addr, qos_param->dst_mac_addr,
		qos_param->vlan_count,
		qos_param->vlan_count ? qos_param->vlan_ids[0] : 0);

	/*
	 * Resolve the TX pipe from the client's own ClientDB-tracked
	 * interface -- never from qos_param->dev_name. iterate_all_clients
	 * resolves this per client (different clients in a catch-all call
	 * can be on different interfaces); the single-client paths
	 * (found_client / explicit dst_ip) resolve it once here.
	 */
	if (!iterate_all_clients) {
		retval = ipa_be_qos_resolve_tx_pipe(iface_name, iface_num,
						     qos_param->ip_type,
						     qos_param->traffic_class,
						     &tx_prop, &tx_index);
		if (retval)
			goto cleanup;

		IPA_BE_DBG("Using TX index %d (dst_pipe %d, tc_bmap 0x%x) for QoS rule\n",
			tx_index, tx_prop->tx[tx_index].dst_pipe,
			tx_prop->tx[tx_index].tc_bmap);
	}

	if (found_client) {
		/* MAC+VLAN case: one rule for this specific client. */
		retval = ipa_be_qos_install_one_rule(qos_param, tx_prop, tx_index,
						     mi_hdr_hdl, mi_addr,
						     qos_param->src_mac_addr,
						     qos_param->dst_mac_addr);
	} else if (iterate_all_clients) {
		/*
		 * No MAC/IP filter (or MAC filter with no VLAN): snapshot all
		 * matching active clients under lock, then install one rule per
		 * client with their IP as dst_addr.
		 *
		 * NOTE: ipa_be_qos_install_one_rule() writes qos_param->qos_rule_hdl
		 * on each successful install. For this multi-client path, only the
		 * last client's handle is retained in the cache node.  This is
		 * intentional for catch-all rules — deletion is handled by
		 * ipa_be_delete_qos_catch_all_rules() when the last client
		 * disconnects, not by individual handle tracking.
		 *
		 * Two-pass: count outside lock, allocate with headroom outside
		 * lock (no GFP_KERNEL under mutex), then collect under lock.
		 */
		struct ipa_clientdb_mapping_instance *it;
		int num_clients = 0;
		int capacity = 0;

		/* Pass 1: count matching clients outside of lock. */
		mutex_lock(&ipa_client_db_lock);
		for (it = ipa_db_mappings; it; it = it->next) {
			bool client_is_v6 = it->address[1] || it->address[2] ||
					    it->address[3];
			bool want_v6 = (qos_param->ip_type == IPA_IP_v6);

			if (client_is_v6 != want_v6)
				continue;
			if (!it->address[0] && !it->address[1] &&
			    !it->address[2] && !it->address[3])
				continue;
			if (mac_filter_active &&
			    memcmp(it->mac_addr_t, mac_filter, IPA_MAC_ADDR_SIZE))
				continue;
			/* Scope the filter to its interface when both names are
			 * known. An empty dev_name means "any interface": the
			 * HAL sends none for WLAN. */
			if (qos_param->dev_name[0] && it->iface_name[0] &&
			    strcmp(qos_param->dev_name, it->iface_name))
				continue;
			capacity++;
		}
		mutex_unlock(&ipa_client_db_lock);

		if (!capacity) {
			IPA_BE_DBG("No active clients matching ip_type %d; skipping QoS rule\n",
				   qos_param->ip_type);
			goto cleanup;
		}

		/* Allocate with headroom outside lock — no GFP_KERNEL under mutex. */
		capacity += 8;
		clients = kcalloc(capacity, sizeof(*clients), GFP_KERNEL);
		if (!clients) {
			retval = -ENOMEM;
			goto cleanup;
		}

		/* Pass 2: collect under lock, no reallocation needed. */
		mutex_lock(&ipa_client_db_lock);
		for (it = ipa_db_mappings; it && num_clients < capacity; it = it->next) {
			bool client_is_v6 = it->address[1] || it->address[2] ||
					    it->address[3];
			bool want_v6 = (qos_param->ip_type == IPA_IP_v6);

			if (client_is_v6 != want_v6)
				continue;
			if (!it->address[0] && !it->address[1] &&
			    !it->address[2] && !it->address[3])
				continue;
			if (mac_filter_active &&
			    memcmp(it->mac_addr_t, mac_filter, IPA_MAC_ADDR_SIZE))
				continue;
			/* Keep in sync with the counting pass above. */
			if (qos_param->dev_name[0] && it->iface_name[0] &&
			    strcmp(qos_param->dev_name, it->iface_name))
				continue;

			memcpy(clients[num_clients].addr, it->address, sizeof(ip_addr_t));
			clients[num_clients].hdr_hdl = (int)it->lan2wan_info.hdr_hdl;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
			strscpy(clients[num_clients].iface_name, it->iface_name,
				sizeof(clients[num_clients].iface_name));
#else
			strlcpy(clients[num_clients].iface_name, it->iface_name,
				sizeof(clients[num_clients].iface_name));
#endif
			clients[num_clients].iface_num = it->iface_num;
			num_clients++;
		}
		mutex_unlock(&ipa_client_db_lock);

		if (!num_clients) {
			IPA_BE_DBG("No active clients matching ip_type %d; skipping QoS rule\n",
				   qos_param->ip_type);
			goto cleanup;
		}

		retval = -EFAULT; /* assume failure; cleared on first successful install */
		for (int c = 0; c < num_clients; c++) {
			struct ipa_ioc_query_intf_tx_props *client_tx_prop = NULL;
			int client_tx_index;
			int r;

			/* Cleared before resolve: a skipped iteration must not
			 * leave a previous client's handle here to be cached. */
			qos_param->proc_ctx_hdl = 0;

			r = ipa_be_qos_resolve_tx_pipe(clients[c].iface_name,
							clients[c].iface_num,
							qos_param->ip_type,
							qos_param->traffic_class,
							&client_tx_prop, &client_tx_index);
			if (r) {
				IPA_BE_ERR("Failed to resolve TX pipe for client %d on %s: %d\n",
					   c, clients[c].iface_name, r);
				continue;
			}

			r = ipa_be_qos_install_one_rule(qos_param, client_tx_prop, client_tx_index,
							clients[c].hdr_hdl,
							clients[c].addr,
							qos_param->src_mac_addr,
							qos_param->dst_mac_addr);
			kfree(client_tx_prop);
			if (r) {
				IPA_BE_ERR("Failed to install QoS rule for client %d: %d\n",
					   c, r);
			} else {
				retval = 0;
				/* Cache keeps only the last client's handle, so
				 * drop the install-time ref for the others (the
				 * rt rule holds its own) and clear it. */
				if (c < num_clients - 1 && qos_param->proc_ctx_hdl) {
					__ipa3_release_hdr_proc_ctx(qos_param->proc_ctx_hdl);
					qos_param->proc_ctx_hdl = 0;
				}
			}
		}
	} else {
		/* Explicit dst_ip case: use dst_ip_addr/dst_v6_ip_addr as client address. */
		ip_addr_t ip_client_addr = {0};
		int k;

		if (qos_param->ip_type == IPA_IP_v4) {
			ip_client_addr[0] = htonl(qos_param->dst_ip_addr);
		} else {
			for (k = 0; k < 4; k++)
				ip_client_addr[k] = htonl(qos_param->dst_v6_ip_addr[k]);
		}
		retval = ipa_be_qos_install_one_rule(qos_param, tx_prop, tx_index,
						     dst_ip_hdr_hdl, ip_client_addr,
						     qos_param->src_mac_addr,
						     qos_param->dst_mac_addr);
	}

cleanup:
	kfree(clients);
	kfree(tx_prop);
	return retval;
}

