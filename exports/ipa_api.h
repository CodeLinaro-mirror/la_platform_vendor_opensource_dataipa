// SPDX-License-Identifier: GPL-2.0-only
/*
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
*/

/**
 * @file ipa_api.h
 *	IPA exported function headers for the IPA .
 */

#ifndef __IPA_API_H
#define __IPA_API_H


#include <linux/types.h>

#define ETH_ALEN 6

#define IPA_MESSAGE_VERSION 0x1

#define IPA_MAX_VLAN_DEPTH 2			/**< Maximum VLAN depth. */
#define IPA_VLAN_ID_NOT_CONFIGURED 0xfff	/**< VLAN ID not configured. */
#define IPA_INVALID_VLAN_PCP 0xff		/**< VLAN PCP remark is invalid for SAWF (Service Aware Wi-Fi). */
#define IPA_MAX_SERVICE_CLASS_ID 0x80		/**< Maximum service class ID. */
#define IPA_INVALID_SERVICE_CLASS_ID 0xff	/**< Service class ID not valid. */
#define IPA_SERVICE_CLASS_STATS_MAX_RETRY 100	/**< Maximum retries for fetching service class statistics. */
#define IPA_INVALID_MSDUQ 0xff			/**< Invalid MAC Service Data Unit Queue. */
#define IPA_MC_IF_MAX 16

/*
 * Rule creation validity flags.
 */
#define IPA_RULE_CREATE_CONN_VALID         (1<<0)	/**< IPv4 connection is valid. */
#define IPA_RULE_CREATE_TCP_VALID          (1<<1)	/**< TCP protocol fields are valid. */
#define IPA_RULE_CREATE_PPPOE_DECAP_VALID  (1<<2)	/**< PPPoE decapsulation fields are valid. */
#define IPA_RULE_CREATE_PPPOE_ENCAP_VALID  (1<<3)	/**< PPPoE encapsulation fields are valid. */
#define IPA_RULE_CREATE_QOS_VALID          (1<<4)	/**< QoS fields are valid. */
#define IPA_RULE_CREATE_VLAN_VALID         (1<<5)	/**< VLAN fields are valid. */
#define IPA_RULE_CREATE_DSCP_MARKING_VALID (1<<6)	/**< DSCP marking fields are valid. */
#define IPA_RULE_CREATE_VLAN_MARKING_VALID (1<<7)	/**< VLAN marking fields are valid. */
#define IPA_RULE_CREATE_DIRECTION_VALID    (1<<8)	/**< Acceleration direction is valid. */
#define IPA_RULE_CREATE_SRC_MAC_VALID      (1<<9)	/**< Source MAC address is valid. */
#define IPA_RULE_CREATE_MARK_VALID         (1<<10)	/**< SKB marking fields are valid. */
#define IPA_RULE_CREATE_TRUSTSEC_VALID     (1<<11)	/**< Trustsec fields are valid. */
#define IPA_RULE_CREATE_QDISC_RULE_VALID   (1<<12)	/**< QDISC rule is valid. */
#define IPA_RULE_CREATE_VLAN_FILTER_VALID  (1<<13)	/**< Bridge VLAN Filter rule is valid. */
#define IPA_RULE_CREATE_FLOW_FRAG_THRESH_VALID	(1<<14)	/** Flow ipv4_frag_thresh is valid */
#define IPA_RULE_CREATE_RETURN_FRAG_THRESH_VALID (1<<15) /** Return ipv4_frag_thresh is valid */
#define IPA_RULE_CREATE_SKIP_FRAG_OFFLOAD_VALID	 (1<<16) /** Fragmentation offload is invalid */

/*
 *  * Rule creation and rule update flags.
 *   */
#define IPA_RULE_CREATE_FLAG_NO_SEQ_CHECK (1<<0)		/**< Do not perform TCP sequence number checks. */
#define IPA_RULE_CREATE_FLAG_BRIDGE_FLOW  (1<<1)		/**< Rule is for a pure bridge forwarding flow. */
#define IPA_RULE_CREATE_FLAG_ROUTED       (1<<2)		/**< Rule is for a routed connection. */
#define IPA_RULE_CREATE_FLAG_DSCP_MARKING (1<<3)		/**< Rule has DSCP marking configured. */
#define IPA_RULE_CREATE_FLAG_NO_SRC_IDENT (1<<4)		/**< Zero out the source identifier. */
#define IPA_RULE_CREATE_FLAG_UNUSED0	  (1<<5)		/**< Rule flag unused 0. */
#define IPA_RULE_CREATE_FLAG_UNUSED1	  (1<<6)		/**< Rule flag unused 1. */
#define IPA_RULE_CREATE_FLAG_L2_ENCAP     (1<<7)		/**< Consists of an encapsulating protocol that carries an IPv4 payload within it. */
#define IPA_RULE_CREATE_FLAG_USE_FLOW_BOTTOM_INTERFACE (1<<8)	/**< Use flow interface number instead of top interface. */
#define IPA_RULE_CREATE_FLAG_USE_RETURN_BOTTOM_INTERFACE (1<<9) /**< Use return interface number instead of top interface. */
#define IPA_RULE_CREATE_FLAG_FLOW_SRC_INTERFACE_CHECK  (1<<10)  /**< Check source interface on the flow direction. */
#define IPA_RULE_CREATE_FLAG_RETURN_SRC_INTERFACE_CHECK  (1<<11)
								/**< Check source interface on the return direction. */
#define IPA_RULE_CREATE_FLAG_FLOW_TRANSMIT_FAST (1<<12)		/**< Original flow can be transmitted fast. */
#define IPA_RULE_CREATE_FLAG_RETURN_TRANSMIT_FAST (1<<13)	/**< Return flow can be transmitted fast. */
#define IPA_RULE_CREATE_FLAG_FLOW_SRC_INTERFACE_CHECK_NO_FLUSH  (1<<14)
								/**< Check source interface on the flow direction but do not flush the connection. */
#define IPA_RULE_CREATE_FLAG_RETURN_SRC_INTERFACE_CHECK_NO_FLUSH  (1<<15)
								/**< Check source interface on the return direction but do not flush the connection. */
#define IPA_RULE_CREATE_FLAG_FLOW_L2_DISABLE (1<<16)		/**< Disable L2 flow processing in original flow. */
#define IPA_RULE_CREATE_FLAG_RETURN_L2_DISABLE (1<<17)		/**< Disable L2 flow processing in return flow. */
#define IPA_RULE_CREATE_FLAG_BRIDGE_VLAN_PASSTHROUGH  (1<<18)	/**< Bridge VLAN passthrough. */
#define IPA_RULE_CREATE_FLAG_FLOW_VXLAN_GPE (1<<19)		/**< VXLAN-GPE rule for original flow. */
#define IPA_RULE_CREATE_FLAG_RETURN_VXLAN_GPE (1<<20)		/**< VXLAN-GPE rule for return flow. */


#define IPA_MAX_VLAN_DEPTH 2			/**< Maximum VLAN depth. */
#define IPA_VLAN_ID_NOT_CONFIGURED 0xfff	/**< VLAN ID not configured. */
#define IPA_INVALID_VLAN_PCP 0xff		/**< VLAN PCP remark is invalid for SAWF (Service Aware Wi-Fi). */
#define IPA_MAX_SERVICE_CLASS_ID 0x80		/**< Maximum service class ID. */
#define IPA_INVALID_SERVICE_CLASS_ID 0xff	/**< Service class ID not valid. */
#define IPA_SERVICE_CLASS_STATS_MAX_RETRY 100	/**< Maximum retries for fetching service class statistics. */
#define IPA_INVALID_MSDUQ 0xff			/**< Invalid MAC Service Data Unit Queue. */
#define IPA_MC_IF_MAX 16

#define IPA_SPECIAL_INTERFACE_BASE 0x7f00	/**< Special interface base number. */
#define IPA_SPECIAL_INTERFACE_IPV4 (IPA_SPECIAL_INTERFACE_BASE + 1)	/**< Interface number for IPv4. */
#define IPA_SPECIAL_INTERFACE_IPV6 (IPA_SPECIAL_INTERFACE_BASE + 2)	/**< Interface number for IPv6. */
#define IPA_SPECIAL_INTERFACE_IPSEC (IPA_SPECIAL_INTERFACE_BASE + 3)	/**< Interface number for IPSec. */
#define IPA_SPECIAL_INTERFACE_L2TP (IPA_SPECIAL_INTERFACE_BASE + 4)	/**< Interface number for L2TP. */
#define IPA_SPECIAL_INTERFACE_PPTP (IPA_SPECIAL_INTERFACE_BASE + 5)	/**< Interface number for PPTP. */

/*
 *  * Source MAC address validity flags; used with the mac_valid_flags field in the ipa_ipv4_src_mac_rule structure.
 *   */
#define IPA_SRC_MAC_FLOW_VALID 0x01
		/**< MAC address for the flow interface is valid. */
#define IPA_SRC_MAC_RETURN_VALID 0x02
		/**< MAC address for the return interface is valid. */

/*
 *  * Ingress/egress trustsec validity flags; used with the CREATE_TRUSTSEC_VALID and ipa_trustsec_rule.
 *   */
#define IPA_TRUSTSEC_INGRESS_SGT_VALID 0x01
		/**< Ingress SGT for the flow interface is valid. */
#define IPA_TRUSTSEC_EGRESS_SGT_VALID 0x02

/*
 *  * Qdisc interface validity flags; used with the qdisc_valid_flags  field in the ipa_qdisc_rule structure.
 *   */
#define IPA_QDISC_RULE_FLOW_VALID 0x01
		/**< Qdisc interface for the flow interface is valid. */
#define IPA_QDISC_RULE_RETURN_VALID 0x02
		/**< Qdisc interface for the return interface is valid. */
#define IPA_QDISC_RULE_FLOW_PPE_QDISC_FAST_XMIT 0x04
		/**< Fast transmit via PPE Qdisc for the flow interface is valid. */
#define IPA_QDISC_RULE_RETURN_PPE_QDISC_FAST_XMIT 0x08
		/**< Fast transmit via PPE Qdisc for the return interface is valid. */


/**
 *  * SAWF_metadata information placement in mark field.
 *   */
#define IPA_SAWF_VALID_TAG              0xAA    /**< Valid SAWF tag value. */
#define IPA_SAWF_TAG_SHIFT              24      /**< Number of bit shifts for SAWF tag. */
#define IPA_SAWF_SERVICE_CLASS_SHIFT    16      /**< Number of bit shifts for SAWF service class ID. */
#define IPA_SAWF_SERVICE_CLASS_MASK     0xff    /**< Mask for SAWF service class ID. */
#define IPA_SAWF_MSDUQ_MASK             0xffff  /**< Mask for SAWF msduq. */

/**
 *  * SAWF_metadata extraction.
 *   */
#define IPA_GET_SAWF_TAG(sawf_meta)             (sawf_meta >> IPA_SAWF_TAG_SHIFT)
				/**< Get tag field in SAWF meta. */
#define IPA_SAWF_TAG_IS_VALID(tag)              ((tag == IPA_SAWF_VALID_TAG) ? true : false)
				/**< Check if tag is a valid SAWF tag. */

/**
 *  * MHT port information placement in mark field.
 *   */
#define IPA_MHT_VALID_TAG		0xBB	/**< Valid MHT tag value. */
#define IPA_MHT_TAG_SHIFT		24	/**< Number of bit shifts for MHT tag. */

#define IPA_MHT_MAX_ACCELERATION_RETRY	256	/**< Maximum retry for IPA acceleration for MHT. */


















/*
 * 32/64-bit pointer types.
 */
#ifdef __LP64__
typedef uint64_t ipa_ptr_t; /**< 64-bit pointer. */
#else
typedef uint32_t ipa_ptr_t; /**< 32-bit pointer. */
#endif

typedef uint32_t ip_addr_t[4];

/**
 * IPA context instance.
 */
struct ipa_ctx_instance {
	int not_used;	/**< Not used. */
};

/**
 * Tx command status.
 */
typedef enum {
	IPA_TX_SUCCESS = 0,		/**< Success. */
	IPA_TX_FAILURE,			/**< Failure other than descriptor not available. */
	IPA_TX_FAILURE_QUEUE,		/**< failure due to descriptor not available. */
	IPA_TX_FAILURE_NOT_READY,	/**< Failure due to IPA state uninitialized. */
	IPA_TX_FAILURE_TOO_LARGE,	/**< Command is too large to fit in one message. */
	IPA_TX_FAILURE_TOO_SHORT,	/**< Command or packet is shorter than expected. */
	IPA_TX_FAILURE_NOT_SUPPORTED,	/**< Command or packet not accepted for forwarding. */
	IPA_TX_FAILURE_BAD_PARAM,	/**< Failure due to bad parameters. */
	IPA_TX_FAILURE_NOT_ENABLED,	/**< Failure due to IPA not enabled. */
} ipa_tx_status_t;

/**
 * * Synchronize reason enum.
 * */
typedef enum /** @cond */ ipa_rule_sync_reason /** @endcond */ {
		IPA_RULE_SYNC_REASON_STATS,		/**< Synchronize statistics. */
		IPA_RULE_SYNC_REASON_FLUSH,		/**< Synchronize to flush an entry. */
		IPA_RULE_SYNC_REASON_EVICT,		/**< Synchronize to evict an entry. */
		IPA_RULE_SYNC_REASON_DESTROY,		/**< Synchronize to destroy an entry (requested by the connection manager). */
		IPA_RULE_SYNC_REASON_FLUSH_SWITCH_AE	/**< Synchronize to flush an entry and direct ECM switch acceleration engine */

} ipa_rule_sync_reason_t;

/*
 *  * Connection mark types.
 *   */
enum ipa_connection_mark_type {
	IPA_CONNECTION_MARK_TYPE_CONNMARK,      /**< Conntrack mark. */
	IPA_CONNECTION_MARK_TYPE_SAWFMARK,      /**< SAWF mark. */
	IPA_CONNECTION_MARK_TYPE_MAX            /**< Indicates the last item. */
};

/**
 *  * Connection mark structure.
 *   */
struct ipa_connection_mark {
	int protocol;				/**< Protocol number. */
	__be32 src_ip[4];			/**< Source IP address. */
	__be32 dest_ip[4];			/**< Destination IP address. */
	__be16 src_port;			/**< Source port number. */
	__be16 dest_port;			/**< Destination port number. */
	u32 flow_mark;				/**< Mark to be updated for the flow direction. */
	u32 return_mark;			/**< Mark to be updated for the return direction. */
	u8 flow_svc_id;			/**< Service class in the flow direction. */
	u8 return_svc_id;		/**< Service class in the return direction. */
	u32 flags;				/**< State of marks. */
	enum ipa_connection_mark_type type;	/**< Type of the marking. */
};




/**
 * Common response types.
 */
enum ipa_cmn_response {
	IPA_CMN_RESPONSE_ACK,		/**< Message acknowledged. */
	IPA_CMN_RESPONSE_EVERSION,	/**< Version error. */
	IPA_CMN_RESPONSE_EINTERFACE,	/**< Interface error. */
	IPA_CMN_RESPONSE_ELENGTH,	/**< Length error. */
	IPA_CMN_RESPONSE_EMSG,		/**< Message error. */
	IPA_CMN_RESPONSE_NOTIFY,	/**< Message independant of request. */
	IPA_CMN_RESPONSE_LAST		/**< Indicates the last item. */
};

/**
 * IPv4 bridge/route rule messages.
 */
enum ipa_message_types {
	IPA_TX_CREATE_RULE_MSG,		/**< IPv4/IPv6 create rule message. */
	IPA_TX_DESTROY_RULE_MSG,	/**< IPv4/IPv6 destroy rule message. */
	IPA_RX_CONN_STATS_SYNC_MSG,	/**< IPv4/IPv6 connection statistics synchronize message. */
	IPA_TX_CONN_STATS_SYNC_MANY_MSG,/**< IPv4/IPv6 connection statistics synchronize many message. */
	IPA_TX_CONN_STATS_SYNC_MANY_TS_ONLY_MSG,/**< IPv4/IPv6 connection timestamp synchronize many message. */
	IPA_TX_CONN_STATS_SYNC_MANY_STATS_ONLY_MSG,/**< IPv4/IPv6 connection statistics synchronize many message (no timestamp). */
	IPA_TUN6RD_ADD_UPDATE_PEER,	/**< Add/update peer for 6RD tunnel. */
	IPA_TX_CREATE_MULTICAST_RULE_MSG,	/**< IPv4/IPv6 create multicast rule message. */
	IPA_TX_DESTROY_MULTICAST_RULE_MSG,     /**< IPv4/IPv6 destroy multicast rule message. */
	IPA_MAX_MSG_TYPES,		/**< IPv4/IPv6 message max type number. */
};



/**
 * Common 5-tuple structure.
 */
struct ipa_ipv4_5tuple {
	__be32 flow_ip;		/**< Flow IP address. */
	__be32 return_ip;	/**< Return IP address. */
	__be16 flow_ident;	/**< Flow identifier, e.g., TCP/UDP port. */
	__be16 return_ident;	/**< Return identifier, e.g., TCP/UDP port. */
	u8 protocol;		/**< Protocol number. */
	u8 reserved[3];		/**< Reserved; padding for alignment. */
};

/**
 * IPv4 connection rule structure.
 */
struct ipa_ipv4_connection_rule {
	u8 flow_mac[6];			/**< Flow MAC address. */
	u8 return_mac[6];		/**< Return MAC address. */
	s32 flow_interface_num;		/**< Flow interface number. */
	s32 return_interface_num;	/**< Return interface number. */
	s32 flow_top_interface_num;	/**< Top flow interface number. */
	s32 return_top_interface_num;	/**< Top return interface number. */
	u32 flow_mtu;			/**< Flow interface`s MTU. */
	u32 return_mtu;			/**< Return interface`s MTU. */
	__be32 flow_ip_xlate;		/**< Translated flow IP address. */
	__be32 return_ip_xlate;		/**< Translated return IP address. */
	__be16 flow_ident_xlate;	/**< Translated flow identifier, e.g., port. */
	__be16 return_ident_xlate;	/**< Translated return identifier, e.g., port. */
};

/**
 * TCP connection rule structure.
 */
struct ipa_protocol_tcp_rule {
	u32 flow_max_window;	/**< Flow direction's largest seen window. */
	u32 return_max_window;	/**< Return direction's largest seen window. */
	u32 flow_end;		/**< Flow direction's largest seen sequence + segment length. */
	u32 return_end;		/**< Return direction's largest seen sequence + segment length. */
	u32 flow_max_end;	/**< Flow direction's largest seen ack + max(1, win). */
	u32 return_max_end;	/**< Return direction's largest seen ack + max(1, win). */
	u8 flow_window_scale;	/**< Flow direction's window scaling factor. */
	u8 return_window_scale;	/**< Return direction's window scaling factor. */
	u16 reserved;		/**< Reserved; padding for alignment. */
};

/**
 * ipa_pppoe_br_accel_mode_t
 *	PPPoE bridge acceleration modes.
 */
typedef enum {
	IPA_PPPOE_BR_ACCEL_MODE_DISABLED,       /**< No acceleration */
	IPA_PPPOE_BR_ACCEL_MODE_EN_5T,          /**< 5-tuple (src_ip, dest_ip, src_port, dest_port, protocol) acceleration */
	IPA_PPPOE_BR_ACCEL_MODE_EN_3T,          /**< 3-tuple (src_ip, dest_ip, pppoe session id) acceleration */
	IPA_PPPOE_BR_ACCEL_MODE_MAX             /**< Indicates the last item */
} __attribute__ ((__packed__)) ipa_pppoe_br_accel_mode_t;

/**
 * PPPoE connection rules structure.
 */
struct ipa_pppoe_rule {
	u16 flow_pppoe_session_id;		/**< Flow direction`s PPPoE session ID. */
	u8 flow_pppoe_remote_mac[ETH_ALEN];	/**< Flow direction`s PPPoE server MAC address. */
	u16 return_pppoe_session_id;		/**< Return direction's PPPoE session ID. */
	u8 return_pppoe_remote_mac[ETH_ALEN];	/**< Return direction's PPPoE server MAC address. */
};

/**
 * Information for source MAC address rules.
 */
struct ipa_src_mac_rule {
	uint32_t mac_valid_flags;	/**< MAC address validity flags. */
	uint16_t flow_src_mac[3];	/**< Source MAC address for the flow direction. */
	uint16_t return_src_mac[3];	/**< Source MAC address for the return direction. */
};

/**
 * QoS connection rule structure.
 */
struct ipa_qos_rule {
	u32 flow_qos_tag;	/**< QoS tag associated with this rule for flow direction. */
	u32 return_qos_tag;	/**< QoS tag associated with this rule for return direction. */
	u8 flow_int_pri;	/**< PPE INT_PRI corresponding to flow direction when PPE Qdisc is configured. */
	u8 return_int_pri;	/**< PPE INT_PRI corresponding to return direction when PPE Qdisc is configured. */
};

/**
* Mark rule structure.
*/
struct ipa_mark_rule {
	u32 flow_mark;		/**< SKB mark associated with this rule for flow direction. */
	u32 return_mark;	/**< SKB mark associated with this rule for return direction. */
};

/**
 * DSCP connection rule structure.
 */
struct ipa_dscp_rule {
	u8 flow_dscp;		/**< Egress DSCP value for flow direction. */
	u8 return_dscp;		/**< Egress DSCP value for return direction. */
	u8 reserved[2];		/**< Reserved; padding for alignment. */
};

/*
 * Bridge VLAN filter flags; used with IPA_RULE_CREATE_VLAN_FILTER_VALID and ipa_vlan_filter_rule structure.
 */
#define IPA_VLAN_FILTER_FLAG_VALID 		(1<<0) 	/**< VLAN filter is valid in the rule.  */
#define IPA_VLAN_FILTER_FLAG_INGRESS_PVID 	(1<<1)	/**< Add VLAN header at ingress for untagged packets. */
#define IPA_VLAN_FILTER_FLAG_EGRESS_UNTAGGED	(1<<2)	/**< Strip VLAN header associated with this VID at egress. */

/*
 *  * SAWF mark validity flags
 *   */
#define IPA_SAWF_MARK_FLOW_VALID        (1<<0)
		/**< SAWF mark in the flow direction is valid. */
#define IPA_SAWF_MARK_RETURN_VALID      (1<<1)
		/**< SAWF mark in the return direction is valid. */



/**
 * VLAN connection rule structure.
 */
struct ipa_vlan_rule {
	u32 ingress_vlan_tag;	/**< VLAN tag for ingress packets. */
	u32 egress_vlan_tag;	/**< VLAN tag for egress packets. */
};

/**
 * VLAN filter connection rule structure.
 * 	TODO: Combine flow/return information into one structure.
 */
struct ipa_vlan_filter_rule {
	u32 ingress_vlan_tag;	/**< VLAN tag for ingress packets. */
	u32 egress_vlan_tag;	/**< VLAN tag for egress packets. */
	u8 ingress_flags;	/**< VLAN flags at ingress. */
	u8 egress_flags;	/**< VLAN flags at egress. */
	u8 reserved[2];		/**< Reserved; padding for alignment. */
};

/**
 * Trustsec connection rule structure.
 */
struct ipa_trustsec_rule {
	u16 ingress_sgt;	/**< Trustsec SGT for ingress packets. */
	u16 egress_sgt;		/**< Trustsec SGT for egress packets. */
	u8 sgt_valid_flags;	/**< SGT validity flags. */
};

/**
 * Acceleration direction rule structure.
 * Sometimes it is useful to accelerate traffic in one direction and not in another.
 */
struct ipa_acceleration_direction_rule {
	u8 flow_accel;		/**< Accelerate in flow direction. */
	u8 return_accel;	/**< Accelerate in return direction. */
	u8 reserved[2];		/**< Reserved; padding for alignment. */
};

/**
 * Service class rule information in both directions.
 */
struct ipa_service_class_rule {
	uint32_t flow_mark;		/**< SAWF metadata information in flow direction. */
	uint32_t return_mark;		/**< SAWF metadata information in return direction. */
	uint8_t flow_svc_id;		/**< Service class id in flow direction. */
	uint8_t return_svc_id;		/**< Service class id in return direction. */
	uint16_t flow_frag_thresh;	/**< Threshold size value in flow direction. */
	uint16_t return_frag_thresh;	/**< Threshold size value in return direction. */
};

/**
 * Qdisc information for both the directions.
 *
 * In case of a single qdisc applied by user in the flow or return interface hierarchy, use this rule to
 * indicate the netdev on which the qdisc is applied. This
 * enables full IPA offload for the direction including the
 * qdisc processing.
 */
struct ipa_qdisc_rule {
	uint32_t valid_flags;       	/**< Qdisc interface validity flags. */
	s32 flow_qdisc_interface;	/**< Netdevice for flow direction on which qdisc is applied. */
	s32 return_qdisc_interface;	/**< Netdevice for return direction on which qdisc is applied. */
};

struct ipa_ipv4_rule_create_msg {
	/* Request */
	u32 valid_flags;				/**< Bit flags associated with paramater validity. */
	u32 rule_flags;					/**< Bit flags associated with the rule. */

	struct ipa_ipv4_5tuple tuple;			/**< Holds values of 5-tuple. */

	struct ipa_ipv4_connection_rule conn_rule;	/**< Basic connection-specific data. */
	struct ipa_protocol_tcp_rule tcp_rule;		/**< TCP-related acceleration parameters. */
	struct ipa_pppoe_rule pppoe_rule;		/**< PPPoE-related acceleration parameters. */
	struct ipa_qos_rule qos_rule;			/**< QoS-related acceleration parameters. */
	struct ipa_src_mac_rule src_mac_rule;		/**< Source MAC address rule. */
	struct ipa_mark_rule mark_rule;			/**< SKB mark-related acceleration parameters. */
	struct ipa_dscp_rule dscp_rule;			/**< DSCP-related acceleration parameters. */
	struct ipa_vlan_rule vlan_primary_rule;		/**< Primary VLAN-related acceleration parameters. */
	struct ipa_vlan_rule vlan_secondary_rule;	/**< Secondary VLAN-related acceleration parameters. */
	struct ipa_trustsec_rule trustsec_rule;		/**< Trustsec-related acceleration parameters. */

#ifdef CONFIG_XFRM
	struct ipa_acceleration_direction_rule direction_rule;
							/**< Direction related acceleration parameters. */
#endif
	/* Response */
	struct ipa_service_class_rule sawf_rule;
							/**< Service class related information */
	struct ipa_qdisc_rule qdisc_rule;		/**< Qdisc indication per direction */
	struct ipa_vlan_filter_rule flow_vlan_filter_rule;
							/**< TO-direction VLAN Filter related acceleration parameters. */
	struct ipa_vlan_filter_rule return_vlan_filter_rule;
							/**< FROM-direction VLAN Filter related acceleration parameters. */
	u32 index;					/**< Slot ID for cache statistics to host OS. */
};


/**
 * IPv4 rule destroy submessage structure.
 */
struct ipa_ipv4_rule_destroy_msg {
	u32 valid_flags;				/**< Bit flags associated with paramater validity. */
	u32 rule_flags;					/**< Bit flags associated with the rule. */

	struct ipa_ipv4_5tuple tuple;	/**< Holds values of 5-tuple. */
	struct ipa_ipv4_connection_rule conn_rule;	/**< Basic connection-specific data. */
	struct ipa_protocol_tcp_rule tcp_rule;		/**< TCP-related acceleration parameters. */
	struct ipa_pppoe_rule pppoe_rule;		/**< PPPoE-related acceleration parameters. */
	struct ipa_qos_rule qos_rule;			/**< QoS-related acceleration parameters. */
	struct ipa_src_mac_rule src_mac_rule;		/**< Source MAC address rule. */
	struct ipa_mark_rule mark_rule;			/**< SKB mark-related acceleration parameters. */
	struct ipa_dscp_rule dscp_rule;			/**< DSCP-related acceleration parameters. */
	struct ipa_vlan_rule vlan_primary_rule;		/**< Primary VLAN-related acceleration parameters. */
	struct ipa_vlan_rule vlan_secondary_rule;	/**< Secondary VLAN-related acceleration parameters. */
	struct ipa_trustsec_rule trustsec_rule;		/**< Trustsec-related acceleration parameters. */

#ifdef CONFIG_XFRM
	struct ipa_acceleration_direction_rule direction_rule;
							/**< Direction related acceleration parameters. */
#endif
	/* Response */
	struct ipa_service_class_rule sawf_rule;
							/**< Service class related information */
	struct ipa_qdisc_rule qdisc_rule;		/**< Qdisc indication per direction */
	struct ipa_vlan_filter_rule flow_vlan_filter_rule;
							/**< TO-direction VLAN Filter related acceleration parameters. */
	struct ipa_vlan_filter_rule return_vlan_filter_rule;
							/**< FROM-direction VLAN Filter related acceleration parameters. */
	u32 index;					/**< Slot ID for cache statistics to host OS. */
};

/*
 * Multicast destination interface entry
 */
struct ipa_ipv4_mc_device_entry {
	uint32_t rule_flags;		/**< Bit flag of rules indicating PPPOE session, Vlan. */
	uint32_t valid_flags;		/**< Valid Bit flags associated with the rule of vlan/pppoe. */
	uint32_t xlate_src_ip;		/**< nated source ip address. */
	uint32_t xlate_src_ident;	/**< nated source port number. */
	uint32_t egress_vlan_tag[IPA_MAX_VLAN_DEPTH]; /**< VLAN tag stack for the ingress packets. */
	uint32_t if_mtu;                        /**< Interface MTU */
	uint16_t if_mac[3];                     /**< Interface MAC address */
	u16 pppoe_session_id;			/**< PPPOE header offset */
	u8 pppoe_remote_mac[ETH_ALEN];		/**< PPPoE remote mac address. */
	u8 egress_vlan_hdr_cnt;			/**< vlan header number. */
	u8  reserved[1];			/**< Reserved; padding for alignment. */
	uint32_t if_num;			/**< Dest interface number (virtual or physical). */
	struct ipa_qdisc_rule qdisc_rule;		/**< Qdisc indication per direction */
};

/**
 * ipa_ipv4_mc_rule_create_msg
 *      IPv4 multicast rule for creating sub-messages.
 */
struct ipa_ipv4_mc_rule_create_msg {
	/* Request */
	u32 valid_flags;				/**< Bit flags associated with paramater validity. */
	u32 rule_flags;					/**< Bit flags associated with the rule. */

	struct ipa_ipv4_5tuple tuple;           /**< Holds values of the 5 tuple. */
	struct ipa_ipv4_connection_rule conn_rule;	/**< Basic connection-specific data. */
	struct ipa_pppoe_rule pppoe_rule;		/**< PPPoE-related acceleration parameters. */
	struct ipa_qos_rule qos_rule;			/**< QoS-related acceleration parameters. */
	struct ipa_src_mac_rule src_mac_rule;		/**< Source MAC address rule. */
	struct ipa_mark_rule mark_rule;			/**< SKB mark-related acceleration parameters. */
	struct ipa_dscp_rule dscp_rule;			/**< DSCP-related acceleration parameters. */
	struct ipa_vlan_rule vlan_primary_rule;		/**< Primary VLAN-related acceleration parameters. */
	struct ipa_vlan_rule vlan_secondary_rule;	/**< Secondary VLAN-related acceleration parameters. */
#ifdef CONFIG_XFRM
	struct ipa_acceleration_direction_rule direction_rule;
							/**< Direction related acceleration parameters. */
#endif
	struct ipa_service_class_rule sawf_rule;	/**< Service class related information. */
	uint16_t dest_mac[3];                   /**< Destination multicast MAC address. */
	uint16_t if_cnt;                      /**< Number of destination interfaces. */
	struct ipa_ipv4_mc_device_entry if_rule[IPA_MC_IF_MAX];	/**< Per-interface information. */
};


/**
 * ipa_ipv4_mc_rule_destroy_msg
 *      IPv4 multicast rule for destroy sub-messages.
 */
struct ipa_ipv4_mc_rule_destroy_msg {
	struct ipa_ipv4_5tuple tuple;           /**< Holds values of the 5 tuple. */
};

/**
 * The IPA IPv4 rule sync structure.
 */
struct ipa_ipv4_conn_sync {
	u32 index;			/**< Slot ID for cache statistics to host OS. */
	u8 protocol;			/**< Protocol number. */
	__be32 flow_ip;			/**< Flow IP address. */
	__be32 flow_ip_xlate;		/**< Translated flow IP address. */
	__be16 flow_ident;		/**< Flow identifier, e.g., port. */
	__be16 flow_ident_xlate;	/**< Translated flow identifier, e.g., port. */
	u32 flow_max_window;		/**< Flow direction's largest seen window. */
	u32 flow_end;			/**< Flow direction's largest seen sequence + segment length. */
	u32 flow_max_end;		/**< Flow direction's largest seen ack + max(1, win). */
	u32 flow_rx_packet_count;	/**< Flow interface's Rx packet count. */
	u64 flow_rx_byte_count;		/**< Flow interface's Rx byte count. */
	u32 flow_rx_packet_count_cache;	/**< Flow interface's Rx packet count from cache. */
	u32 flow_tx_packet_count;	/**< Flow interface's Tx packet count. */
	u64 flow_tx_byte_count;		/**< Flow interface's Tx byte count. */
	u32 flow_tx_packet_count_cache;	/**< Flow interface's Tx packet count from cache. */
	u16 flow_pppoe_session_id;	/**< Flow interface`s PPPoE session ID. */
	u16 flow_pppoe_remote_mac[3];	/**< Flow interface's PPPoE remote server MAC address (if present). */
	__be32 return_ip;		/**< Return IP address. */
	__be32 return_ip_xlate;		/**< Translated return IP address */
	__be16 return_ident;		/**< Return identifier, e.g., port. */
	__be16 return_ident_xlate;	/**< Translated return identifier, e.g., port. */
	u32 return_max_window;		/**< Return direction's largest seen window. */
	u32 return_end;			/**< Return direction's largest seen sequence + segment length. */
	u32 return_max_end;		/**< Return direction's largest seen ack + max(1, win). */
	u32 return_rx_packet_count;	/**< Return interface's Rx packet count. */
	u64 return_rx_byte_count;	/**< Return interface's Rx byte count. */
	u32 return_rx_packet_count_cache;	/**< Return interface's Rx packet count from cache. */
	u32 return_tx_packet_count;	/**< Return interface's Tx packet count. */
	u64 return_tx_byte_count;	/**< Return interface's Tx byte count. */
	u32 return_tx_packet_count_cache;	/**< Return interface's Tx packet count from cache. */
	u16 return_pppoe_session_id;	/**< Return interface`s PPPoE session ID. */
	u16 return_pppoe_remote_mac[3];	/**< Return interface's PPPoE remote server MAC address (if present). */
	u32 inc_ticks;			/**< Number of ticks since the last sync. */
	u32 reason;			/**< Synchronization reason. */

	u8 flags;			/**< Bit flags associated with the rule. */
	u32 qos_tag;			/**< QoS tag. */
	u32 cause;			/**< Flush cause. */
};

/**
 * Information for a multiple IPv4 connection statistics synchronization message.
 */
struct ipa_ipv4_conn_sync_many_msg {
	/*
	 * Request
	 */
	uint16_t index;		/**< Request connection statistics from the index. */
	uint16_t size;		/**< Buffer size of this message. */

	/*
	 * Response
	 */
	uint16_t next;		/**< Firmware response for the next connection to be requested. */
	uint16_t count;		/**< Number of synchronized connections included in this message. */
	struct ipa_ipv4_conn_sync conn_sync[];	/**< Array for the statistics. */
};

/**
 * IPv6 5-tuple structure.
 */
struct ipa_ipv6_5tuple {
	__be32 flow_ip[4];	/**< Flow IP address. */
	__be32 return_ip[4];	/**< Return IP address. */
	__be16 flow_ident;	/**< Flow identifier, e.g.,TCP/UDP port. */
	__be16 return_ident;	/**< Return identifier, e.g., TCP/UDP port. */
	u8  protocol;		/**< Protocol number. */
	u8  reserved[3];	/**< Reserved; padding for alignment. */
};

/**
 * IPv6 connection rule structure.
 */
struct ipa_ipv6_connection_rule {
	u8 flow_mac[6];			/**< Flow MAC address. */
	u8 return_mac[6];		/**< Return MAC address. */
	s32 flow_interface_num;		/**< Flow interface number. */
	s32 return_interface_num;	/**< Return interface number. */
	s32 flow_top_interface_num;	/**< Top flow interface number. */
	s32 return_top_interface_num;	/**< Top return interface number. */
	u32 flow_mtu;			/**< Flow interface's MTU. */
	u32 return_mtu;			/**< Return interface's MTU. */
	__be32 flow_ip_xlate[4];		/**< Translated flow IP address. */
	__be32 return_ip_xlate[4];		/**< Translated return IP address. */
	__be16 flow_ident_xlate;	/**< Translated flow identifier, e.g., port. */
	__be16 return_ident_xlate;	/**< Translated return identifier, e.g., port. */
};


/**
 * IPv6 rule create submessage structure.
 */
struct ipa_ipv6_rule_create_msg {
	/*
	 * Request
	 */
	u32 valid_flags;				/**< Bit flags associated with parameter validity. */
	u32 rule_flags;					/**< Bit flags associated with the rule. */
	struct ipa_ipv6_5tuple tuple;			/**< Holds values of the ipa_ipv6_5tuple tuple. */
	struct ipa_ipv6_connection_rule conn_rule;	/**< Basic connection-specific data. */
	struct ipa_protocol_tcp_rule tcp_rule;		/**< Protocol-related acceleration parameters. */
	struct ipa_pppoe_rule pppoe_rule;		/**< PPPoE-related acceleration parameters. */
	struct ipa_qos_rule qos_rule;			/**< QoS-related acceleration parameters. */
	struct ipa_src_mac_rule src_mac_rule;		/**< Source MAC address rule. */
	struct ipa_mark_rule mark_rule;			/**< SKB mark-related acceleration parameters. */
	struct ipa_dscp_rule dscp_rule;			/**< DSCP-related acceleration parameters. */
	struct ipa_vlan_rule vlan_primary_rule;		/**< VLAN-related acceleration parameters. */
	struct ipa_vlan_rule vlan_secondary_rule;	/**< VLAN-related acceleration parameters. */
	struct ipa_trustsec_rule trustsec_rule;		/**< Trustsec-related acceleration parameters. */

#ifdef CONFIG_XFRM
	struct ipa_acceleration_direction_rule direction_rule;
							/**< Direction-related acceleration parameters. */
#endif
	/*
	 * Response
	 */
	struct ipa_service_class_rule sawf_rule;	/**< Service class related information. */
	struct ipa_qdisc_rule qdisc_rule;		/**< Qdisc indication per direction */
	struct ipa_vlan_filter_rule flow_vlan_filter_rule;
							/**< TO-direction VLAN Filter related acceleration parameters. */
	struct ipa_vlan_filter_rule return_vlan_filter_rule;
							/**< FROM-direction VLAN Filter related acceleration parameters. */
	u32 index;					/**< Slot ID for cache statistics to host OS. */
};

/**
 * IPv6 rule destroy submessage structure.
 */
struct ipa_ipv6_rule_destroy_msg {
	u32 valid_flags;				/**< Bit flags associated with parameter validity. */
	u32 rule_flags;					/**< Bit flags associated with the rule. */
	struct ipa_ipv6_5tuple tuple;	/**< Holds values of the ipa_ipv6_5tuple tuple */
	struct ipa_ipv6_connection_rule conn_rule;	/**< Basic connection-specific data. */
	struct ipa_protocol_tcp_rule tcp_rule;		/**< Protocol-related acceleration parameters. */
	struct ipa_pppoe_rule pppoe_rule;		/**< PPPoE-related acceleration parameters. */
	struct ipa_qos_rule qos_rule;			/**< QoS-related acceleration parameters. */
	struct ipa_src_mac_rule src_mac_rule;		/**< Source MAC address rule. */
	struct ipa_mark_rule mark_rule;			/**< SKB mark-related acceleration parameters. */
	struct ipa_dscp_rule dscp_rule;			/**< DSCP-related acceleration parameters. */
	struct ipa_vlan_rule vlan_primary_rule;		/**< VLAN-related acceleration parameters. */
	struct ipa_vlan_rule vlan_secondary_rule;	/**< VLAN-related acceleration parameters. */
	struct ipa_trustsec_rule trustsec_rule;		/**< Trustsec-related acceleration parameters. */

#ifdef CONFIG_XFRM
	struct ipa_acceleration_direction_rule direction_rule;
							/**< Direction-related acceleration parameters. */
#endif
	/*
	 * Response
	 */
	struct ipa_service_class_rule sawf_rule;	/**< Service class related information. */
	struct ipa_qdisc_rule qdisc_rule;		/**< Qdisc indication per direction */
	struct ipa_vlan_filter_rule flow_vlan_filter_rule;
							/**< TO-direction VLAN Filter related acceleration parameters. */
	struct ipa_vlan_filter_rule return_vlan_filter_rule;
							/**< FROM-direction VLAN Filter related acceleration parameters. */
	u32 index;					/**< Slot ID for cache statistics to host OS. */
};

/*
 * Multicast destination interface entry
 */
struct ipa_ipv6_mc_device_entry {
	uint32_t rule_flags;		/**< Bit flag of rules indicating PPPOE session, Vlan. */
	uint32_t valid_flags;		/**< Valid Bit flags associated with the rule of vlan/pppoe. */
	uint32_t xlate_src_ip[4];	/**< nated source ip address. */
	uint32_t xlate_src_ident;	/**< nated source port number. */

	uint32_t egress_vlan_tag[IPA_MAX_VLAN_DEPTH]; /**< VLAN tag stack for the ingress packets. */
	uint32_t if_mtu;                        /* Interface MTU */
	uint32_t if_num;		/**< Dest interface number (virtual or physical). */
	uint16_t if_mac[3];                     /* Interface MAC address */
	u16 pppoe_session_id;			/* PPPOE header offset */
	u8 pppoe_remote_mac[ETH_ALEN];
	u8 egress_vlan_hdr_cnt;
	u8  reserved[1];			/**< Reserved; padding for alignment. */
	struct ipa_qdisc_rule qdisc_rule;		/**< Qdisc indication per direction */
};

/**
 * ipa_ipv6_mc_rule_create_msg
 *      IPv6 multicast rule for creating sub-messages.
 */
struct ipa_ipv6_mc_rule_create_msg {
	/* Request */
	u32 valid_flags;				/**< Bit flags associated with paramater validity. */
	u32 rule_flags;					/**< Bit flags associated with the rule. */

	struct ipa_ipv6_5tuple tuple;           /**< Holds values of the 5 tuple. */
	struct ipa_ipv6_connection_rule conn_rule;	/**< Basic connection-specific data. */
	struct ipa_pppoe_rule pppoe_rule;		/**< PPPoE-related acceleration parameters. */
	struct ipa_qos_rule qos_rule;			/**< QoS-related acceleration parameters. */
	struct ipa_src_mac_rule src_mac_rule;		/**< Source MAC address rule. */
	struct ipa_mark_rule mark_rule;			/**< SKB mark-related acceleration parameters. */
	struct ipa_dscp_rule dscp_rule;			/**< DSCP-related acceleration parameters. */
	struct ipa_vlan_rule vlan_primary_rule;		/**< Primary VLAN-related acceleration parameters. */
	struct ipa_vlan_rule vlan_secondary_rule;	/**< Secondary VLAN-related acceleration parameters. */
#ifdef CONFIG_XFRM
	struct ipa_acceleration_direction_rule direction_rule;
							/**< Direction-related acceleration parameters. */
#endif
	struct ipa_service_class_rule sawf_rule;	/**< Service class related information. */
	uint16_t dest_mac[3];                   /**< Destination multicast MAC address. */
	uint16_t if_cnt;                      /**< Number of destination interfaces. */
	struct ipa_ipv6_mc_device_entry if_rule[IPA_MC_IF_MAX];	/**< Per-interface information. */

};

/**
 * ipa_ipv6_mc_rule_destroy_msg
 *      IPv6 multicast rule for destroy sub-messages.
 */
struct ipa_ipv6_mc_rule_destroy_msg {
	struct ipa_ipv6_5tuple tuple;           /**< Holds values of the 5 tuple. */
};

/**
 * IPA IPv6 rule sync structure.
 */
struct ipa_ipv6_conn_sync {
	u32 index;			/**< Slot ID for cache statistics to host OS. */
	u8 protocol;			/**< Protocol number. */
	__be32 flow_ip[4];		/**< Flow IP address. */
	__be32 flow_ip_xlate[4];		/**< Translated flow IP address. */
	__be16 flow_ident;		/**< Flow identifier, e.g., port. */
	__be16 flow_ident_xlate;	/**< Translated flow identifier, e.g., port. */
	u32 flow_max_window;		/**< Flow direction's largest seen window. */
	u32 flow_end;			/**< Flow direction's largest seen sequence + segment length. */
	u32 flow_max_end;		/**< Flow direction's largest seen ack + max(1, win). */
	u32 flow_rx_packet_count;	/**< Flow interface's Rx packet count. */
	u64 flow_rx_byte_count;		/**< Flow interface's Rx byte count. */
	u32 flow_rx_packet_count_cache;	/**< Flow interface's Rx packet count from cache. */
	u32 flow_tx_packet_count;	/**< Flow interface's Tx packet count. */
	u64 flow_tx_byte_count;		/**< Flow interface's Tx byte count. */
	u32 flow_tx_packet_count_cache;	/**< Flow interface's Tx packet count from cache. */
	u16 flow_pppoe_session_id;	/**< Flow interface`s PPPoE session ID. */
	u16 flow_pppoe_remote_mac[3];	/**< Flow interface's PPPoE remote server MAC address (if present). */
	__be32 return_ip[4];		/**< Return IP address. */
	__be32 return_ip_xlate[4];		/**< Translated return IP address. */
	__be16 return_ident;		/**< Return identifer, e.g., port. */
	__be16 return_ident_xlate;	/**< Translated return identifier, e.g., port. */
	u32 return_max_window;		/**< Return direction's largest seen window. */
	u32 return_end;			/**< Return direction's largest seen sequence + segment length. */
	u32 return_max_end;		/**< Return direction's largest seen ack + max(1, win). */
	u32 return_rx_packet_count;	/**< Return interface's Rx packet count. */
	u64 return_rx_byte_count;	/**< Return interface's Rx byte count. */
	u32 return_rx_packet_count_cache;	/**< Return interface's Rx packet count from cache. */
	u32 return_tx_packet_count;	/**< Return interface's Tx packet count. */
	u64 return_tx_byte_count;	/**< Return interface's Tx byte count. */
	u32 return_tx_packet_count_cache;	/**< Return interface's Tx packet count from cache. */
	u16 return_pppoe_session_id;	/**< Return interface`s PPPoE session ID. */
	u16 return_pppoe_remote_mac[3];	/**< Return interface's PPPoE remote server MAC address (if present). */
	u32 inc_ticks;			/**< Number of ticks since the last sync. */
	u32 reason;			/**< Sync reason. */
	u8 flags;			/**< Bit flags associated with the rule. */
	u32 qos_tag;			/**< QoS tag. */
	u32 cause;			/**< Flush cause associated with the rule. */
};

/**
 * Information for a multiple IPv6 connection statistics synchronization message.
 */
struct ipa_ipv6_conn_sync_many_msg {
	/*
	 * Request:
	 */
	uint16_t index;		/**< Request connection statistics from the index. */
	uint16_t size;		/**< Buffer size of this message. */

	/*
	 * Response:
	 */
	uint16_t next;		/**< Firmware response for the next connection to be requested. */
	uint16_t count;		/**< Number of synchronized connections included in this message. */
	struct ipa_ipv6_conn_sync conn_sync[];	/**< Array for the statistics. */
};

/**
 * Common message structure.
 */
struct ipa_cmn_msg {
	u16 version;			/**< Version ID for the main message format. */
	u16 interface;			/**< Primary key for all messages. */
	enum ipa_cmn_response response;	/**< Primary response. */
	u32 type;			/**< Decentralized request ID used to match response ID. */
	u32 error;			/**< Decentralized specific error message; response == EMSG. */
	ipa_ptr_t cb;			/**< Callback pointer. */
	ipa_ptr_t app_data;		/**< Application data. */
	u32 len;			/**< Length of the message excluding this header. */
};


/**
 * Message structure to send/receive IPv4 bridge/route commands
 */
struct ipa_ipv4_msg {
	struct ipa_cmn_msg cm;					/**< Message header. */
	union {
		struct ipa_ipv4_rule_create_msg rule_create;	/**< Rule create message. */
		struct ipa_ipv4_rule_destroy_msg rule_destroy;	/**< Rule destroy message. */
		struct ipa_ipv4_conn_sync conn_stats;		/**< Connection statistics synchronization message. */
		struct ipa_ipv4_conn_sync_many_msg conn_stats_many;
					/**< Many connections' statistics synchronization message. */
		struct ipa_ipv4_mc_rule_create_msg mc_rule_create;/**< MC rule create message. */
		struct ipa_ipv4_mc_rule_destroy_msg mc_rule_destroy; /**<MC rule destroy message. */
	} msg;							/**< IPv4 message. */
};

/**
 * Message structure to send/receive IPv6 bridge/route commands.
 */
struct ipa_ipv6_msg {
	struct ipa_cmn_msg cm;		/**< Message header. */
	union {
		struct ipa_ipv6_rule_create_msg rule_create;
					/**< Rule create message. */
		struct ipa_ipv6_rule_destroy_msg rule_destroy;
					/**< Rule destroy message. */
		struct ipa_ipv6_conn_sync conn_stats;
					/**< Statistics synchronization message. */
		struct ipa_ipv6_conn_sync_many_msg conn_stats_many;
					/**< Many Connections' statistics synchronizaion message. */
		struct ipa_ipv6_mc_rule_create_msg mc_rule_create;/**< MC rule create message. */
		struct ipa_ipv6_mc_rule_destroy_msg mc_rule_destroy; /**<MC rule destroy message. */

	} msg;				/**< IPv6 message. */
};


/**
 * IPv4 message received callback.
 */
typedef void (*ipa_ipv4_msg_callback_t)(void *app_data, struct ipa_ipv4_msg *msg);

/**
 * IPv6 message received callback.
 */
typedef void (*ipa_ipv6_msg_callback_t)(void *app_data, struct ipa_ipv6_msg *msg);

/**
 * Indicates whether the l2 feature flag is enabled or disabled.
 *  *
 *   * @return
 *    * True if enabled; false if disabled.
 *     */
bool ipa_is_l2_feature_enabled(void);


/**
 * Transmits an IPv4 message to the IPA.
 *
 * @param	ipa_ctx		IPA context.
 * @param	msg		The IPv4 message.
 *
 * @return
 * The status of the Tx operation (#ipa_tx_status_t).
 */
extern ipa_tx_status_t ipa_be_ipv4_send_request(struct ipa_ctx_instance *ipa_ctx, struct ipa_ipv4_msg *msg);

/**
 * Transmits an IPv4 message to the IPA.
 *
 * @param	ipa_ctx		IPA context.
 * @param	msg		The IPv4 message.
 *
 * @return
 * ACK/NACK msg based on successful accel/deaccel.
 */
extern enum ipa_cmn_response ipa_be_ipv4_send_request_with_resp(struct ipa_ctx_instance *ipa_ctx, struct ipa_ipv4_msg *msg);


/**
 * Transmits an IPv6 message to the IPA.
 *
 * @param	ipa_ctx		IPA context.
 * @param	msg		The IPv6 message.
 *
 * @return
 * The status of the Tx operation (#ipa_tx_status_t).
 */
extern ipa_tx_status_t ipa_be_ipv6_send_request(struct ipa_ctx_instance *ipa_ctx, struct ipa_ipv6_msg *msg);

/**
 * Transmits an IPv6 message to the IPA.
 *
 * @param	ipa_ctx		IPA context.
 * @param	msg		The IPv6 message.
 *
 * @return
 * ACK/NACK msg based on successful accel/deaccel.
 */
extern enum ipa_cmn_response ipa_be_ipv6_send_request_with_resp(struct ipa_ctx_instance *ipa_ctx, struct ipa_ipv6_msg *msg);


/**
 * Registers a notifier callback for IPv4 messages from the IPA.
 *
 * @param	one_rule_cb		The callback pointer for one rule.
 * @param	many_rules_cb		The callback pointer for many rules.
 * @param	app_data	The application context for this message.
 *
 * @return
 * The IPA context (#ipa_ctx_instance).
 */
extern struct ipa_ctx_instance *ipa_ipv4_notify_register(ipa_ipv4_msg_callback_t one_rule_cb,
		ipa_ipv4_msg_callback_t many_rules_cb,void *app_data);


/**
 * Registers a notifier callback for IPv6 messages from the IPA.
 *
 * @param	one_rule_cb		The callback pointer for one rule.
 * @param	many_rules_cb		The callback pointer for many rules.
 *
 * @return
 * The IPA context (#ipa_ctx_instance).
 */
extern struct ipa_ctx_instance *ipa_ipv6_notify_register(ipa_ipv6_msg_callback_t one_rule_cb,
		ipa_ipv6_msg_callback_t many_rules_cb,void *app_data);


/**
 * Unregisters a notifier callback for IPv4 messages from the
 * IPA.
															   */
extern void ipa_ipv4_notify_unregister(void);


/**
 * Unregisters a notifier callback for IPv6 messages from the
 * IPA.
																*/
extern void ipa_ipv6_notify_unregister(void);


/**
 * Initializes an IPv4 message.
 *
 * @param	nim		The IPv4 message pointer.
 * @param	if_num		The interface number.
 * @param	type		The type of the message.
 * @param	len		The length of the message.
 * @param	cb		The message callback.
 * @param	app_data	The application context for this message.
 *
 */
extern void ipa_ipv4_msg_init(struct ipa_ipv4_msg *nim, u16 if_num, u32 type, u32 len,
			ipa_ipv4_msg_callback_t cb, void *app_data);


/**
 * Initializes an IPv6 message.
 *
 * @param	nim		The IPv6 message pointer.
 * @param	if_num		The interface number.
 * @param	type		The type of the message.
 * @param	len		The length of the message.
 * @param	cb		The message callback.
 * @param	app_data	The application context for this message.
 *
 * @return
 * None.
 */
extern void ipa_ipv6_msg_init(struct ipa_ipv6_msg *nim, u16 if_num, u32 type, u32 len,
			ipa_ipv6_msg_callback_t cb, void *app_data);


/**
 * Queries max v4 connections supported by IPA
 *
 * @param	void
 *
 * @return max number of connections
 *
 */
extern int ipa_be_ipv4_get_max_accel_conn(void);

/**
 * Queries max v6 connections supported by IPA
 *
 * @param	void
 *
 * @return max number of connections
 *
 */
extern int ipa_be_ipv6_get_max_accel_conn(void);

/**
 * Queries max timeout in seconds
 *
 * @param	void
 *
 * @return max number of connections
 *
 */
extern int ipa_be_get_max_timeout(void);

#endif /* _IPA_API_ */
