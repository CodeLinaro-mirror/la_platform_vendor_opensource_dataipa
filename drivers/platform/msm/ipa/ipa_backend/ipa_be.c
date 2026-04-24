// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include "../ipa_common_i.h"
#include "../ipa_v3/ipa_pm.h"
#include "../ipa_v3/ipa_i.h"
#include <linux/sort.h>
#include "ipa_api.h"
#include "ipa_be.h"
#include "ipa_be_flt_mgmt.h"
#include "ipa_be_clientdb.h"
#ifdef CONFIG_ECM_CONVERGENCE
#include "ipa_be_nat_mgmt.h"
#endif

/* ========================================================================== */
/* MACROS AND DEFINITIONS                                                     */
/* ========================================================================== */

#define IPA_MAX_TIMEOUT 20	/* seconds */

#define ipa_ipv4_stopped(CTX) (rcu_dereference((CTX)->ipv4_stats_sync_cb) == NULL)
#define ipa_ipv6_stopped(CTX) (rcu_dereference((CTX)->ipv6_stats_sync_cb) == NULL)

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

static struct ipa_ctx_instance_internal __ipa_be_ctx;

/*
 * Convert public IPA context to internal context
 */
#define IPA_CTX_TO_PRIVATE(base) (struct ipa_ctx_instance_internal *)(base)
/*
 * Convert internal IPA context to public context
 */
#define IPA_CTX_TO_PUBLIC(intrv) (struct ipa_ctx_instance *)(intrv)


const char* ipa_be_message_type_to_string(enum ipa_message_types type) {
	switch (type) {
		case IPA_TX_CREATE_RULE_MSG: return "IPA_TX_CREATE_RULE_MSG";
		case IPA_TX_DESTROY_RULE_MSG: return "IPA_TX_DESTROY_RULE_MSG";
		case IPA_RX_CONN_STATS_SYNC_MSG: return "IPA_RX_CONN_STATS_SYNC_MSG";
		case IPA_TX_CONN_STATS_SYNC_MANY_MSG: return "IPA_TX_CONN_STATS_SYNC_MANY_MSG";
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
			   v4_msg.vlan_primary_rule.egress_vlan_tag,
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
			   v6_msg.vlan_primary_rule.egress_vlan_tag,
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
	if (msg->cm.type == IPA_TX_CONN_STATS_SYNC_MANY_MSG) {
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

	if (msg->cm.type == IPA_TX_CONN_STATS_SYNC_MANY_MSG) {
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

/**
 * ipa_ipv4_create_rule - Creates IPv4 routing and filtering
 * rules for IPA
 * @v4_msg: IPv6 rule creation message containing connection
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
	int bridge_if_num = 0;
	int vlan_tag = 0;
	uint32_t mtu_size = 0;
	ip_addr_t lan_client_ip = {0};
	ip_addr_t ret_ip_key = {0};
	ip_addr_t flow_ip_key = {0};
	uint32_t wan_ip = 0;
	uint32_t flow_interface_num = 0;
	mac_addr_t mac, wan_mac;

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
		ret_ip_key[0] = v4_msg.tuple.return_ip;
		if (ipa_be_client_mapping_add_or_ref(ret_ip_key, 0, lan2lan, v4_msg.conn_rule.return_mac) != NULL)
		{
			is_ret =  true;
			step = 1;
			/* Add route entry */
			ret = ipa_ipv4_add_route_rule(v4_msg, lan2lan, v4_msg.conn_rule.return_interface_num, v4_msg.conn_rule.return_mac, is_ret);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add route rule for return IP\n");
				goto failed_ret;
			}
			step = 2;

			/* Add destination based filter rule */
			ret = ipa_be_v4_add_filter_rule(v4_msg, lan2lan, v4_msg.conn_rule.flow_interface_num, v4_msg.conn_rule.return_mac, is_ret);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add filter rule for return IP\n");
				goto failed_ret;
			}
			step = 3;
		}

		flow_ip_key[0] = v4_msg.tuple.flow_ip;
		if (ipa_be_client_mapping_add_or_ref(flow_ip_key, 0, lan2lan, v4_msg.conn_rule.flow_mac) != NULL)
		{
			is_ret =  false;
			step = 4;
			/* Add route entry */
			ret = ipa_ipv4_add_route_rule(v4_msg, lan2lan, v4_msg.conn_rule.flow_interface_num, v4_msg.conn_rule.flow_mac, is_ret);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add route rule for flow IP\n");
				goto failed_ret;
			}
			step = 5;

			/* Add destination based filter rule */
			ret = ipa_be_v4_add_filter_rule(v4_msg, lan2lan, v4_msg.conn_rule.return_interface_num, v4_msg.conn_rule.flow_mac, is_ret);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add filter rule for flow IP\n");
				goto failed_ret;
			}
		}
	}
	else {
		/* Set direction-specific parameters for lan2wan */
		if (v4_msg.conn_rule.return_interface_num == v4_msg.conn_rule.return_top_interface_num) {
			IPA_BE_DBG("Uplink flow - installing both uplink and downlink rules\n");
			pdn_iface          = v4_msg.conn_rule.return_interface_num;
			client_iface       = v4_msg.conn_rule.flow_interface_num;
			bridge_if_num      = v4_msg.conn_rule.flow_top_interface_num;
			vlan_tag           = v4_msg.vlan_primary_rule.ingress_vlan_tag;
			mtu_size           = v4_msg.conn_rule.flow_mtu;
			lan_client_ip[0]   = v4_msg.tuple.flow_ip;
			wan_ip             = v4_msg.conn_rule.return_ip_xlate;
			flow_interface_num = v4_msg.conn_rule.flow_interface_num;
			memcpy(mac, v4_msg.conn_rule.flow_mac, IPA_MAC_ADDR_SIZE);
			memcpy(wan_mac, v4_msg.conn_rule.return_mac, IPA_MAC_ADDR_SIZE);
		} else if (v4_msg.conn_rule.flow_interface_num == v4_msg.conn_rule.flow_top_interface_num) {
			IPA_BE_DBG("Downlink flow - installing both downlink and uplink rules\n");
			pdn_iface          = v4_msg.conn_rule.flow_interface_num;
			client_iface       = v4_msg.conn_rule.return_interface_num;
			bridge_if_num      = v4_msg.conn_rule.return_top_interface_num;
			vlan_tag           = v4_msg.vlan_primary_rule.egress_vlan_tag;
			mtu_size           = v4_msg.conn_rule.return_mtu;
			lan_client_ip[0]   = v4_msg.conn_rule.return_ip_xlate;
			wan_ip             = v4_msg.tuple.flow_ip;
			flow_interface_num = v4_msg.conn_rule.return_interface_num;
			memcpy(mac, v4_msg.conn_rule.return_mac, IPA_MAC_ADDR_SIZE);
			memcpy(wan_mac, v4_msg.conn_rule.flow_mac, IPA_MAC_ADDR_SIZE);
			is_ret = true;
		} else {
			IPA_BE_ERR("Unexpected param %d\n", ret);
			goto failed_ret;
		}

		/* Add route entry for the client */
		if (ipa_be_client_mapping_add_or_ref(lan_client_ip, 0, lan2lan, mac) != NULL) {
			step = 1;
			ret = ipa_ipv4_add_route_rule(v4_msg, lan2lan, flow_interface_num, mac, is_ret);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add route rule\n");
				goto failed_ret;
			}
			step = 2;
		}

		/* Install uplink filter rules */
		ret = ipa_be_v4_add_uplink_filter_rule(v4_msg, lan2lan, pdn_iface, client_iface);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add uplink filter rule\n");
			goto failed_ret;
		}
		step = 3;

		ret = ipa_be_construct_mtu_rule(IPA_IP_v4, mtu_size, client_iface, pdn_iface, vlan_tag);
		if (ret != 0) {
			IPA_BE_ERR("Failed to construct mtu rule\n");
			goto failed_ret;
		}
		step = 4;

		ret = ipa_be_handle_private_subnet(client_iface, bridge_if_num);
		if (ret != 0) {
			IPA_BE_ERR("Failed to handle private subnet\n");
			goto failed_ret;
		}
		step = 5;

		/* Install downlink rules */
		ret = add_dft_filtering_rule(pdn_iface, IPA_IP_v4);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add dft filtering rule\n");
			goto failed_ret;
		}
		step = 6;

		ret = add_catchup_all_filtering_rule_each_pdn(pdn_iface, IPA_IP_v4);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add catchup all filtering rule\n");
			goto failed_ret;
		}
		step = 7;

		ret = install_wan_filtering_rule();
		if (ret != 0) {
			IPA_BE_ERR("Failed to install wan filtering rule\n");
			goto failed_ret;
		}
		step = 8;

#ifdef CONFIG_ECM_CONVERGENCE
		/* Add NAT entry */
		ret = ipa_be_addpdn(v4_msg, pdn_iface);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add PDN entry\n");
			goto failed_ret;
		}
		step = 9;
#else
		IPA_BE_DBG("NAT support disabled - skipping ipa_be_addpdn\n");
		return IPA_TX_FAILURE_NOT_ENABLED;
#endif
	}
	IPA_BE_DBG("Command  return %d \n", ret);
	return ret;

failed_ret:
	ret = IPA_CMN_RESPONSE_EMSG;
	if (lan2lan) {
		switch (step) {
			case 5:
				ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping((uint32_t *)&v4_msg.tuple.flow_ip, lan2lan, NULL, NULL, NULL), IPA_IP_v4);
				/* fallthrough */
			case 4:
				ipa_be_mapping_deref_and_delete((uint32_t *)&v4_msg.tuple.flow_ip, lan2lan);
				/* fallthrough */
			case 3:
				ipa_be_v4_delete_filter_rule(*((struct ipa_ipv4_rule_destroy_msg *)&v4_msg), v4_msg.conn_rule.flow_interface_num, v4_msg.conn_rule.return_mac, lan2lan);
				/* fallthrough */
			case 2:
				ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping((uint32_t *)&v4_msg.tuple.return_ip, lan2lan, NULL, NULL, NULL), IPA_IP_v4);
				/* fallthrough */
			case 1:
				ipa_be_mapping_deref_and_delete((uint32_t *)&v4_msg.tuple.return_ip, lan2lan);
		}
	} else {
		switch (step) {
			case 8:
				/* Need a way to rollback wan_filtering_rule if possible */
				/* fallthrough */
			case 7:
				delete_catchup_all_filtering_rule_each_pdn(pdn_iface, IPA_IP_v4);
				/* fallthrough */
			case 6:
				delete_dft_filtering_rule(pdn_iface, IPA_IP_v4);
				/* fallthrough */
			case 5:
				ipa_be_delete_private_subnet(client_iface, bridge_if_num, IPA_IP_v4);
				/* fallthrough */
			case 4:
				ipa_be_delete_mtu_rule(client_iface, pdn_iface, IPA_IP_v4);
				/* fallthrough */
			case 3:
				ipa_be_v4_delete_uplink_filter_rule(*((struct ipa_ipv4_rule_destroy_msg *)&v4_msg), pdn_iface, client_iface);
				/* fallthrough */
			case 2:
				ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping(lan_client_ip, lan2lan, NULL, NULL, NULL), IPA_IP_v4);
				/* fallthrough */
			case 1:
				ipa_be_mapping_deref_and_delete(lan_client_ip, lan2lan);
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
	uint32_t v6_prefix[2] = {0};
	ip_addr_t ret_ip6_key = {0};
	ip_addr_t flow_ip6_key = {0};
	mac_addr_t mac, wan_mac;

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
		if (ipa_be_client_mapping_add_or_ref(ret_ip6_key, 0, lan2lan, v6_msg.conn_rule.return_mac) != NULL)
		{
			is_ret =  true;
			step = 1;
			/* Add route entry */
			ret = ipa_ipv6_add_route_rule(v6_msg, lan2lan, v6_msg.conn_rule.return_interface_num, v6_msg.conn_rule.return_mac, is_ret);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add route rule for return IP\n");
				goto failed_ret;
			}
			step = 2;

			/* Add destination based filter rule */
			ret = ipa_be_v6_add_filter_rule(v6_msg, lan2lan, v6_msg.conn_rule.flow_interface_num, v6_msg.conn_rule.return_mac);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add filter rule for return IP\n");
				goto failed_ret;
			}
			step = 3;
		}

		memcpy(flow_ip6_key, &v6_msg.tuple.flow_ip, sizeof(flow_ip6_key));
		if (ipa_be_client_mapping_add_or_ref(flow_ip6_key, 0, lan2lan, v6_msg.conn_rule.flow_mac) != NULL)
		{
			is_ret =  false;
			step = 4;
			/* Add route entry */
			ret = ipa_ipv6_add_route_rule(v6_msg, lan2lan, v6_msg.conn_rule.flow_interface_num, v6_msg.conn_rule.flow_mac, is_ret);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add route rule for flow IP\n");
				goto failed_ret;
			}
			step = 5;

			/* Add destination based filter rule */
			ret = ipa_be_v6_add_filter_rule(v6_msg, lan2lan, v6_msg.conn_rule.return_interface_num, v6_msg.conn_rule.flow_mac);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add filter rule for flow IP\n");
				goto failed_ret;
			}
		}
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
			flow_interface_num = v6_msg.conn_rule.return_interface_num;
			memcpy(mac, v6_msg.conn_rule.return_mac, IPA_MAC_ADDR_SIZE);
			memcpy(wan_mac, v6_msg.conn_rule.flow_mac, IPA_MAC_ADDR_SIZE);
			is_ret = true;
		} else {
			IPA_BE_ERR("Unexpected param %d\n", ret);
			goto failed_ret;
		}

		/* Add route entry for the client */
		if (ipa_be_client_mapping_add_or_ref(flow_ip_ptr, 0, lan2lan, mac) != NULL) {
			step = 1;
			ret = ipa_ipv6_add_route_rule(v6_msg, lan2lan, flow_interface_num, mac, is_ret);
			if (ret != 0) {
				IPA_BE_ERR("Failed to add IPv6 route rule\n");
				goto failed_ret;
			}
			step = 2;
		}

		/* Install uplink filter rules */
		ret = ipa_be_v6_add_uplink_filter_rule(v6_msg, lan2lan, pdn_iface, client_iface, 0);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add IPv6 uplink filter rule\n");
			goto failed_ret;
		}
		step = 3;

		ret = ipa_be_construct_mtu_rule(IPA_IP_v6, mtu_size, client_iface, pdn_iface, vlan_tag);
		if (ret != 0) {
			IPA_BE_ERR("Failed to construct IPv6 mtu rule\n");
			goto failed_ret;
		}
		step = 4;

		v6_prefix[0] = ntohl(flow_ip_ptr[0]);
		v6_prefix[1] = ntohl(flow_ip_ptr[1]);
		ret = ipa_be_handle_ipv6_prefix_flt_rule(client_iface, v6_prefix);
		if (ret != 0) {
			IPA_BE_ERR("Failed to handle IPv6 prefix filter rule\n");
			goto failed_ret;
		}
		step = 5;

		/* Install downlink rules */
		ret = add_dft_filtering_rule(pdn_iface, IPA_IP_v6);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add IPv6 dft filtering rule\n");
			goto failed_ret;
		}
		step = 6;

		ret = add_catchup_all_filtering_rule_each_pdn(pdn_iface, IPA_IP_v6);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add IPv6 catchup all filtering rule\n");
			goto failed_ret;
		}
		step = 7;

		ret = install_wan_filtering_rule();
		if (ret != 0) {
			IPA_BE_ERR("Failed to install wan filtering rule\n");
			goto failed_ret;
		}
		step = 8;

#ifdef CONFIG_ECM_CONVERGENCE
		/* Add CT entry */
		ret = ipa_be_add_v6_ct_entry(v6_msg, pdn_iface);
		if (ret != 0) {
			IPA_BE_ERR("Failed to add IPv6 CT entry\n");
			goto failed_ret;
		}
		step = 9;
#else
		IPA_BE_DBG("NAT support disabled - skipping ipa_be_add_v6_ct_entry\n");
#endif
	}
	IPA_BE_DBG("Command  return %d \n", ret);
	return ret;

failed_ret:
	ret = IPA_CMN_RESPONSE_EMSG;
	if (lan2lan) {
		switch (step) {
			case 5:
				ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping((uint32_t *)&v6_msg.tuple.flow_ip, lan2lan, NULL, NULL, NULL), IPA_IP_v6);
				/* fallthrough */
			case 4:
				ipa_be_mapping_deref_and_delete((uint32_t *)&v6_msg.tuple.flow_ip, lan2lan);
				/* fallthrough */
			case 3:
				ipa_be_v6_delete_filter_rule(*((struct ipa_ipv6_rule_destroy_msg *)&v6_msg), v6_msg.conn_rule.flow_interface_num, v6_msg.conn_rule.return_mac, lan2lan);
				/* fallthrough */
			case 2:
				ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping((uint32_t *)&v6_msg.tuple.return_ip, lan2lan, NULL, NULL, NULL), IPA_IP_v6);
				/* fallthrough */
			case 1:
				ipa_be_mapping_deref_and_delete((uint32_t *)&v6_msg.tuple.return_ip, lan2lan);
		}
	} else {
		switch (step) {
			case 8:
				/* Need a way to rollback wan_filtering_rule if possible */
				/* fallthrough */
			case 7:
				delete_catchup_all_filtering_rule_each_pdn(pdn_iface, IPA_IP_v6);
				/* fallthrough */
			case 6:
				delete_dft_filtering_rule(pdn_iface, IPA_IP_v6);
				/* fallthrough */
			case 5:
				ipa_be_delete_ipv6_prefix_flt_rule(client_iface);
				/* fallthrough */
			case 4:
				ipa_be_delete_mtu_rule(client_iface, pdn_iface, IPA_IP_v6);
				/* fallthrough */
			case 3:
				ipa_be_v6_delete_uplink_filter_rule(*((struct ipa_ipv6_rule_destroy_msg *)&v6_msg), pdn_iface, client_iface);
				/* fallthrough */
			case 2:
				ipa_delete_route_rule(lan2lan, ipa_get_rt_hdl_from_mapping(flow_ip_ptr, lan2lan, NULL, NULL, NULL), IPA_IP_v6);
				/* fallthrough */
			case 1:
				ipa_be_mapping_deref_and_delete(flow_ip_ptr, lan2lan);
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



static void ipa_ipv4_destroy_rule(struct ipa_ipv4_rule_destroy_msg *msg)
{
	bool lan2lan = false;
	int ref = 0;
	int rt_hdl = 0;
	int pdn_iface = 0;
	int client_iface = 0;
	int bridge_if_num = 0;
	int hdr_hdl = -1;
	int proc_ctx_hdl = -1;
	char proc_ctx_name[32] = {0};
	ip_addr_t lan_client_ip = {0};
	ip_addr_t ret_addr = {0};
	ip_addr_t flow_addr = {0};

	IPA_BE_DBG("Entry  ipa_ipv4_destroy_rule \n");


	IPA_BE_DBG("ECMIPA create flow_ip: %pI4n:%d protocol: %d\n", &msg->tuple.flow_ip, ntohs(msg->tuple.flow_ident), msg->tuple.protocol);
	IPA_BE_DBG("ECMIPA create return_flow_ip: %pI4n:%d\n", &msg->tuple.return_ip, ntohs(msg->tuple.return_ident));

	IPA_BE_DBG("ECMIPA create flow_ip_xlate: %pI4n:%d protocol: %d\n", &msg->conn_rule.flow_ip_xlate, ntohs(msg->conn_rule.flow_ident_xlate), msg->tuple.protocol);
	IPA_BE_DBG("ECMIPA create return flow_ip_xlate: %pI4n:%d protocol: %d\n", &msg->conn_rule.return_ip_xlate, ntohs(msg->conn_rule.return_ident_xlate), msg->tuple.protocol);


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
				"flow_mac: %pM"
				"return_mac: %pM\n"
				"flow_mtu: %u\n"
				"return_mtu: %u\n",
			   msg->conn_rule.flow_mac,
			   msg->conn_rule.return_mac,
			   msg->conn_rule.flow_mtu,
			   msg->conn_rule.return_mtu);

	if ((msg->conn_rule.flow_interface_num == msg->conn_rule.flow_top_interface_num) &&
		(msg->conn_rule.return_interface_num == msg->conn_rule.return_top_interface_num))
	{
		lan2lan = true;
		IPA_BE_DBG("No natted ip hence lan2lan: %d\n", lan2lan);
	}
	IPA_BE_DBG("Is connection lan2lan: %d\n", lan2lan);

	/* Zero-pad IPv4 address into ip_addr_t so hash/compare only sees the IPv4 word */
	ret_addr[0] = msg->tuple.return_ip;
	rt_hdl = ipa_get_rt_hdl_from_mapping(ret_addr,
		lan2lan, &hdr_hdl, &proc_ctx_hdl, proc_ctx_name);
	IPA_BE_DBG("Rt hdl %d \n", rt_hdl);

	// Check reference count first, then delete route rule only if ref will become 0
	ref = ipa_be_mapping_deref_and_delete(ret_addr, lan2lan);
	if (ref == -1)
	{
		IPA_BE_DBG("Entry %pI4n does not exist \n", &msg->tuple.return_ip);
	}
	else if (ref > 0)
	{
		IPA_BE_DBG("Entry %pI4n has other references ref count decreased %d\n", &msg->tuple.return_ip, ref);
		/* Don't delete route rules - other connections still using them */
	}
	else if (ref == 0)
	{
		IPA_BE_DBG("Ref now %d, deleting route rule\n", ref);
		/* Only delete route rule when ref count reaches 0 */
		if (rt_hdl)
		{
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
	}

	if (lan2lan)
	{
		ipa_be_v4_delete_filter_rule(*msg, msg->conn_rule.flow_interface_num, msg->conn_rule.return_mac, lan2lan);

		// Delete reverse flow only for LAN2LAN - check reference count first
		/* Zero-pad IPv4 address into ip_addr_t so hash/compare only sees the IPv4 word */
		flow_addr[0] = msg->tuple.flow_ip;
		rt_hdl = ipa_get_rt_hdl_from_mapping(flow_addr,
			lan2lan, &hdr_hdl, &proc_ctx_hdl, proc_ctx_name);
		IPA_BE_DBG("Rt hdl %d \n", rt_hdl);

		ref = ipa_be_mapping_deref_and_delete(flow_addr, lan2lan);
		if (ref == -1)
		{
			IPA_BE_DBG("Entry %pI4n does not exist \n", &msg->tuple.flow_ip);
			return;
		}
		else if (ref > 0)
		{
			IPA_BE_DBG("Entry %pI4n has other references ref count decreased %d\n", &msg->tuple.flow_ip, ref);
			/* Don't delete route rules - other connections still using them */
			return;
		}
		else if (ref == 0)
		{
			IPA_BE_DBG("Ref now %d, deleting route rule\n", ref);
			/* Only delete route rule when ref count reaches 0 */
			if (rt_hdl)
			{
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
		}

		ipa_be_v4_delete_filter_rule(*msg, msg->conn_rule.return_interface_num, msg->conn_rule.flow_mac, lan2lan);
	}
	else
	{
		/* LAN2WAN: Determine direction-specific parameters first, then delete all rules */
		if (msg->conn_rule.return_interface_num == msg->conn_rule.return_top_interface_num) {
			IPA_BE_DBG("Uplink flow destroy - deleting both uplink and downlink rules\n");
			pdn_iface        = msg->conn_rule.return_interface_num;
			client_iface     = msg->conn_rule.flow_interface_num;
			bridge_if_num    = msg->conn_rule.flow_top_interface_num;
			lan_client_ip[0] = msg->tuple.flow_ip;
		} else if (msg->conn_rule.flow_interface_num == msg->conn_rule.flow_top_interface_num) {
			IPA_BE_DBG("Downlink flow destroy - deleting both downlink and uplink rules\n");
			pdn_iface        = msg->conn_rule.flow_interface_num;
			client_iface     = msg->conn_rule.return_interface_num;
			bridge_if_num    = msg->conn_rule.return_top_interface_num;
			lan_client_ip[0] = msg->conn_rule.return_ip_xlate;
		} else {
			IPA_BE_ERR("Invalid WAN flow param\n");
			goto skip_wan_rules;
		}

		/* Delete route rule for client */
		rt_hdl = ipa_get_rt_hdl_from_mapping(lan_client_ip,
			lan2lan, &hdr_hdl, &proc_ctx_hdl, proc_ctx_name);

		ref = ipa_be_mapping_deref_and_delete(lan_client_ip, lan2lan);
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
		ipa_be_v4_delete_uplink_filter_rule(*msg, pdn_iface, client_iface);

		/* Delete MTU rule for the client/pdn/ip tuple (ref-counted) */
		if (ipa_be_delete_mtu_rule(client_iface, pdn_iface, IPA_IP_v4) != 0) {
			IPA_BE_ERR("Failed to delete MTU rule for client %d pdn %d\n", client_iface, pdn_iface);
		}

		/* Delete private subnet rules (ref-counted per intf/bridge pair) */
		ipa_be_delete_private_subnet(client_iface, bridge_if_num, IPA_IP_v4);

		/* Delete downlink rules */
		delete_catchup_all_filtering_rule_each_pdn(pdn_iface, IPA_IP_v4);
		delete_dft_filtering_rule(pdn_iface, IPA_IP_v4);
		install_wan_filtering_rule();

#ifdef CONFIG_ECM_CONVERGENCE
		ipa_be_delete_entry(*msg);
#else
		IPA_BE_DBG("NAT support disabled - skipping ipa_be_delete_entry\n");
#endif
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
static void ipa_ipv6_destroy_rule(struct ipa_ipv6_rule_destroy_msg *msg)
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

	if (!msg) {
		IPA_BE_ERR("Invalid message pointer\n");
		return;
	}

	IPA_BE_DBG("Entry ipa_ipv6_destroy_rule\n");

	IPA_BE_DBG("ECMIPA destroy flow_ip: %pI6n:%d protocol: %d\n", &msg->tuple.flow_ip, ntohs(msg->tuple.flow_ident), msg->tuple.protocol);
	IPA_BE_DBG("ECMIPA destroy return_flow_ip: %pI6n:%d\n", &msg->tuple.return_ip, ntohs(msg->tuple.return_ident));
	IPA_BE_DBG("ECMIPA destroy flow_ip_xlate: %pI6n:%d protocol: %d\n", &msg->conn_rule.flow_ip_xlate, ntohs(msg->conn_rule.flow_ident_xlate), msg->tuple.protocol);
	IPA_BE_DBG("ECMIPA destroy return flow_ip_xlate: %pI6n:%d protocol: %d\n", &msg->conn_rule.return_ip_xlate, ntohs(msg->conn_rule.return_ident_xlate), msg->tuple.protocol);
	IPA_BE_DBG("ECMIPA destroy "
				"flow_interface_num: %u "
				"return_interface_num: %u "
				"flow_top_interface_num: %u "
				"return_top_interface_num: %u\n"
				"flow_mac: %pM "
				"return_mac: %pM\n"
				"flow_mtu: %u\n"
				"return_mtu: %u\n",
			   msg->conn_rule.flow_interface_num,
			   msg->conn_rule.return_interface_num,
			   msg->conn_rule.flow_top_interface_num,
			   msg->conn_rule.return_top_interface_num,
			   msg->conn_rule.flow_mac,
			   msg->conn_rule.return_mac,
			   msg->conn_rule.flow_mtu,
			   msg->conn_rule.return_mtu);

	/* Determine if this is a LAN-to-LAN connection */
	if ((msg->conn_rule.flow_interface_num == msg->conn_rule.flow_top_interface_num) &&
		(msg->conn_rule.return_interface_num == msg->conn_rule.return_top_interface_num)) {
		lan2lan = true;
		IPA_BE_DBG("No xlated ip hence lan2lan: %d\n", lan2lan);
	}
	IPA_BE_DBG("Is connection lan2lan: %d\n", lan2lan);

	rt_hdl = ipa_get_rt_hdl_from_mapping((uint32_t *)&msg->tuple.return_ip,
		lan2lan, &hdr_hdl, &proc_ctx_hdl, proc_ctx_name);
	IPA_BE_DBG("Return flow rt_hdl %d\n", rt_hdl);

	/* Check reference count first for return flow */
	ref = ipa_be_mapping_deref_and_delete((uint32_t *)&msg->tuple.return_ip, lan2lan);
	if (ref == -1) {
		IPA_BE_DBG("Entry %pI6n does not exist\n", &msg->tuple.return_ip);
	} else if (ref > 0) {
		IPA_BE_DBG("Entry %pI6n has other references, ref count decreased to %d\n", &msg->tuple.return_ip, ref);
		/* Don't delete route rules - other connections still using them */
	} else if (ref == 0) {
		IPA_BE_DBG("Ref now %d, deleting route rule\n", ref);
		/* Only delete route rule when ref count reaches 0 */
		if (rt_hdl > 0) {
			if (ipa_ipv6_delete_route_rule(*msg, lan2lan, rt_hdl, IPA_IP_v6) != 0) {
				IPA_BE_ERR("Failed to delete return flow route rule hdl %d\n", rt_hdl);
			}

			/* Clean up header using handle-based reference counting */
			if (hdr_hdl > 0) {
				ipa_be_delete_hdr_by_handle(hdr_hdl);
			}

			/* Clean up proc_ctx using name-based reference counting */
			if (proc_ctx_name[0] != '\0') {
				ipa_be_delete_proc_ctx(proc_ctx_name);
			}
		}
	}

	if (lan2lan) {
		/* LAN2LAN: Delete filter rule for return flow */
		if (ipa_be_v6_delete_filter_rule(*msg, msg->conn_rule.flow_interface_num, msg->conn_rule.return_mac, lan2lan) != 0) {
			IPA_BE_ERR("Failed to delete LAN2LAN filter rule for return flow\n");
		}

		/* LAN2LAN: Also delete forward flow - check reference count first */
		rt_hdl = ipa_get_rt_hdl_from_mapping((uint32_t *)&msg->tuple.flow_ip, lan2lan, &hdr_hdl, &proc_ctx_hdl, proc_ctx_name);
		IPA_BE_DBG("Forward flow rt_hdl %d\n", rt_hdl);

		ref = ipa_be_mapping_deref_and_delete((uint32_t *)&msg->tuple.flow_ip, lan2lan);
		if (ref == -1) {
			IPA_BE_DBG("Entry %pI6n does not exist\n", &msg->tuple.flow_ip);
			return;
		} else if (ref > 0) {
			IPA_BE_DBG("Entry %pI6n has other references, ref count decreased to %d\n", &msg->tuple.flow_ip, ref);
			/* Don't delete route rules - other connections still using them */
			return;
		} else if (ref == 0) {
			IPA_BE_DBG("Ref now %d, deleting route rule\n", ref);
			/* Only delete route rule when ref count reaches 0 */
			if (rt_hdl) {
				if (ipa_delete_route_rule(lan2lan, rt_hdl, IPA_IP_v6) != 0) {
					IPA_BE_ERR("Failed to delete forward flow route rule hdl %d\n", rt_hdl);
				}

				/* Clean up header using handle-based reference counting */
				if (hdr_hdl > 0) {
					ipa_be_delete_hdr_by_handle(hdr_hdl);
				}

				/* Clean up proc_ctx using name-based reference counting */
				if (proc_ctx_name[0] != '\0') {
					ipa_be_delete_proc_ctx(proc_ctx_name);
				}
			}
		}

		/* Delete filter rules for forward flow */
		if (ipa_be_v6_delete_filter_rule(*msg, msg->conn_rule.return_interface_num, msg->conn_rule.flow_mac, lan2lan) != 0) {
			IPA_BE_ERR("Failed to delete filter rule for forward flow\n");
		}
	} else {
		/* LAN2WAN: Determine direction-specific parameters first, then delete all rules */
		if (msg->conn_rule.return_interface_num == msg->conn_rule.return_top_interface_num) {
			IPA_BE_DBG("Uplink flow destroy - deleting both uplink and downlink rules\n");
			pdn_iface    = msg->conn_rule.return_interface_num;
			client_iface = msg->conn_rule.flow_interface_num;
			flow_ip_ptr  = (uint32_t *)&msg->tuple.flow_ip;
		} else if (msg->conn_rule.flow_interface_num == msg->conn_rule.flow_top_interface_num) {
			IPA_BE_DBG("Downlink flow destroy - deleting both downlink and uplink rules\n");
			pdn_iface    = msg->conn_rule.flow_interface_num;
			client_iface = msg->conn_rule.return_interface_num;
			flow_ip_ptr  = (uint32_t *)&msg->tuple.return_ip;
		} else {
			IPA_BE_ERR("Invalid WAN flow param\n");
			goto skip_v6_wan_rules;
		}

		/* Delete route rule for client */
		rt_hdl = ipa_get_rt_hdl_from_mapping(flow_ip_ptr,
			lan2lan, &hdr_hdl, &proc_ctx_hdl, proc_ctx_name);

		ref = ipa_be_mapping_deref_and_delete(flow_ip_ptr, lan2lan);
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
		if (ipa_be_v6_delete_uplink_filter_rule(*msg, pdn_iface, client_iface) != 0) {
			IPA_BE_ERR("Failed to delete uplink filter rule for pdn %d, client %d\n", pdn_iface, client_iface);
		}

		/* Delete MTU rule for the client/pdn/ip tuple (ref-counted) */
		if (ipa_be_delete_mtu_rule(client_iface, pdn_iface, IPA_IP_v6) != 0) {
			IPA_BE_ERR("Failed to delete MTU rule for client %d pdn %d\n", client_iface, pdn_iface);
		}

		/* Delete IPv6 prefix filter rules (ref-counted per intf) */
		ipa_be_delete_ipv6_prefix_flt_rule(client_iface);

		/* Delete downlink rules */
		delete_catchup_all_filtering_rule_each_pdn(pdn_iface, IPA_IP_v6);
		delete_dft_filtering_rule(pdn_iface, IPA_IP_v6);
		install_wan_filtering_rule();

#ifdef CONFIG_ECM_CONVERGENCE
		/* Handle IPv6 CT entry deletion */
		ipa_be_handle_v6_ct_deletion(msg);
#else
		IPA_BE_DBG("NAT support disabled - skipping ipa_be_handle_v6_ct_deletion\n");
#endif

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
		ipa_ipv4_msg_callback_t many_rules_cb,void *app_data)
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
		ipa_ipv6_msg_callback_t many_rules_cb,void *app_data)
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
		return -ENOMEM;
	}

#ifdef CONFIG_ECM_CONVERGENCE
	if(ipa_be_nat_mgmt_init()){
		IPA_BE_ERR("failed be_nat_mgmt_init\n");
	}
#else
	IPA_BE_DBG("NAT support disabled - skipping ipa_be_nat_mgmt_init\n");
#endif
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
