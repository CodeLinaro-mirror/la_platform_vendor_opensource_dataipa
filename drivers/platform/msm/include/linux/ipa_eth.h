/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2019-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2022-2024 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#ifndef _IPA_ETH_H_
#define _IPA_ETH_H_

#include "ipa.h"
#include <linux/msm_ipa.h>
#include <linux/msm_gsi.h>
#include <linux/netdev_features.h>

#define IPA_ETH_MAX_DMA_CH 12
#define IPA_ETH_CONFIG_LEN 20

/* New architecture prototypes */

typedef void (*ipa_eth_ready_cb)(void *user_data);
typedef u32 ipa_eth_hdl_t;

/**
 * struct ipa_eth_ready_cb - eth readiness parameters
 *
 * @notify: ipa_eth client ready callback notifier
 * @userdata: userdata for ipa_eth ready cb
 * @is_eth_ready: true if ipa_eth client is already ready
 */
struct ipa_eth_ready {
	ipa_eth_ready_cb notify;
	void *userdata;

	/* out params */
	bool is_eth_ready;
};

/**
 * enum ipa_eth_client_type - names for the various IPA
 * eth "clients".
 */
enum ipa_eth_client_type {
	IPA_ETH_CLIENT_AQC107,
	IPA_ETH_CLIENT_AQC113,
	IPA_ETH_CLIENT_RTK8111K,
	IPA_ETH_CLIENT_RTK8125B,
	IPA_ETH_CLIENT_NTN,
	IPA_ETH_CLIENT_EMAC,
	IPA_ETH_CLIENT_NTN3,
	IPA_ETH_CLIENT_IEMAC,
	IPA_ETH_CLIENT_MAX,
};

/**
 * enum ipa_eth_pipe_traffic_type - traffic type for the various IPA
 * eth "pipes".
 */
enum ipa_eth_pipe_traffic_type {
	IPA_ETH_PIPE_BEST_EFFORT,
	IPA_ETH_PIPE_LOW_LATENCY,
	IPA_ETH_PIPE_BEST_EFFORT_VLAN,
	IPA_ETH_PIPE_TRAFFIC_TYPE_QOS,
	IPA_ETH_PIPE_TRAFFIC_TYPE_MAX,
};

/**
 * struct ipa_eth_qos_info - ETH QOS Info.
 * client_type: client info.
 * @tc_bmap: bitmap associated with the client.
 * @pipe_idx: Pipe index.
 * @priority: Relative priority of the pipe.
 */
struct ipa_eth_qos_info {
	enum ipa_client_type client_type;
	u32 tc_bmap;
	u8 pipe_idx;
	u8 priority;
};

/**
 * enum ipa_eth_pipe_direction - pipe direcitons for same
 * ethernet client.
 */
enum ipa_eth_pipe_direction {
	IPA_ETH_PIPE_DIR_TX,
	IPA_ETH_PIPE_DIR_RX,
	IPA_ETH_PIPE_DIR_MAX,
};

#define IPA_ETH_INST_ID_MAX (3)

struct ipa_eth_dma_ch_config {
	enum ipa_eth_pipe_direction dir;
	enum ipa_eth_pipe_traffic_type traffic_type;
};

struct ipa_eth_config {
	int num_dma_channel;
	char config[IPA_ETH_CONFIG_LEN];

	/* dma channels config list */
	struct ipa_eth_dma_ch_config dma_config[IPA_ETH_MAX_DMA_CH];

#if IPA_ETH_API_VER >= 8
	/* Segmentation-offload capability — populated by ipa_eth_get_config_type().
	 * Generic across offload paths (ULSO today); not ULSO-specific.
	 */
	bool offload_supported;  /* IPA HW supports segmentation offload here */
	u32  offload_max_pkts;   /* IPA-recommended max in-flight offload TX pkts
				  * Use as high watermark for TX queue stop.
				  * Suggested low watermark: offload_max_pkts / 2 */
	u32  offload_headroom;   /* bytes to reserve via dev->needed_headroom
				  * for the QMAP extension header.
				  * Valid only when offload_supported is true.  */
	/**
	 * offload_features: netdev HW feature flags to enable on the net_device
	 * when offload_supported is true.  The ETH driver must set:
	 *   netdev->hw_features |= config.offload_features;
	 * Matches the flags set by rndis_ipa / ecm_ipa:
	 *   NETIF_F_RXCSUM | NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM |
	 *   NETIF_F_SG | NETIF_F_GRO_HW | NETIF_F_ALL_TSO | NETIF_F_GSO_UDP_L4
	 * Zero when offload_supported is false.
	 */
	netdev_features_t offload_features;
	/**
	 * offload_gso_max_size: value to assign to netdev->gso_max_size when
	 * offload_supported is true.  Zero when offload_supported is false.
	 */
	u32  offload_gso_max_size;
#endif
};

/**
 * struct ipa_eth_ntn_setup_info - parameters for ntn ethernet
 * offloading
 *
 * @bar_addr: bar PA to access NTN register
 * @tail_ptr_offs: tail ptr offset
 * @ioc_mod_threshold: Descriptors # per interrupt request from
 * NTN3 HW via descriptor bit as part of the protocol.
 */
struct ipa_eth_ntn_setup_info {
	phys_addr_t bar_addr;
	phys_addr_t tail_ptr_offs;
	uint16_t ioc_mod_threshold;
};

/**
 * struct ipa_eth_aqc_setup_info - parameters for aqc ethernet
 * offloading
 *
 * @bar_addr: bar PA to access AQC register
 * @head_ptr_offs: head ptr offset
 * @aqc_ch: AQC ch number
 * @dest_tail_ptr_offs: tail ptr offset
 */
struct ipa_eth_aqc_setup_info {
	phys_addr_t bar_addr;
	phys_addr_t head_ptr_offs;
	u8 aqc_ch;
	phys_addr_t dest_tail_ptr_offs;
};


/**
 * struct ipa_eth_realtek_setup_info - parameters for realtek ethernet
 * offloading
 *
 * @bar_addr: bar PA to access RTK register
 * @bar_size: bar region size
 * @queue_number: Which RTK queue to check the status on
 * @dest_tail_ptr_offs: tail ptr offset
 * @num_queues_enabled: Total queues to be enable
 */
struct ipa_eth_realtek_setup_info {
	phys_addr_t bar_addr;
	u32 bar_size;
	u8 queue_number;
	phys_addr_t dest_tail_ptr_offs;
	u8 num_queues_enabled;
};

/**
 * struct ipa_eth_buff_smmu_map -  IPA iova->pa SMMU mapping
 * @iova: virtual address of the data buffer
 * @pa: physical address of the data buffer
 */
struct ipa_eth_buff_smmu_map {
	dma_addr_t iova;
	phys_addr_t pa;
};

/**
 * struct  ipa_eth_pipe_setup_info - info needed for IPA setups
 * @is_transfer_ring_valid: if transfer ring is needed
 * @transfer_ring_base:  the base of the transfer ring
 * @transfer_ring_sgt: sgtable of transfer ring
 * @transfer_ring_size:  size of the transfer ring
 * @is_buffer_pool_valid: if buffer pool is needed
 * @buffer_pool_base_addr:  base of buffer pool address
 * @buffer_pool_base_sgt:  sgtable of buffer pool
 * @data_buff_list_size: number of buffers
 * @data_buff_list: array of data buffer list
 * @fix_buffer_size: buffer size
 * @notify:	callback for exception/embedded packets
 * @priv: priv for exception callback
 * @client_info: vendor specific pipe setup info
 * @fcs_strip_en: request GSI FW to subtract 4-byte FCS from RX packet length (NTN/IEMAC only)
 * @db_pa: doorbell physical address
 * @db_val: doorbell value ethernet HW need to ring
 */
struct ipa_eth_pipe_setup_info {
	/* transfer ring info */
	bool is_transfer_ring_valid;
	dma_addr_t  transfer_ring_base;
	struct sg_table *transfer_ring_sgt;
	u32 transfer_ring_size;

	/* buffer pool info */
	bool is_buffer_pool_valid;
	dma_addr_t buffer_pool_base_addr;
	struct sg_table *buffer_pool_base_sgt;

	/* buffer info */
	u32 data_buff_list_size;
	struct ipa_eth_buff_smmu_map *data_buff_list;
	u32 fix_buffer_size;

	/* client notify cb */
	ipa_notify_cb notify;
	void *priv;

	/* vendor specific info */
	union {
		struct ipa_eth_aqc_setup_info aqc;
		struct ipa_eth_realtek_setup_info rtk;
		struct ipa_eth_ntn_setup_info ntn;
	} client_info;

#if IPA_ETH_API_VER >= 7
	/* request GSI FW to subtract 4-byte FCS trailer from RX packet length;
	 * generic feature for new ETH protocols, not required for legacy ones */
	bool fcs_strip_en;
#endif

	/* output params */
	phys_addr_t db_pa;
	u32 db_val;
};

/**
 * struct  ipa_eth_client_pipe_info - ETH pipe/gsi related configuration
 * @link: link of ep for different client function on same ethernet HW
 * @dir: TX or RX direction
 * @info: tx/rx pipe setup info
 * @client_info: client the pipe belongs to
 * @traffic_type: traffic type
 * @tc_bmap: Bit map indicating the traffic classes associated to the pipe
 * @pipe_hdl: output params, pipe handle
 */
struct ipa_eth_client_pipe_info {
	struct list_head link;
	enum ipa_eth_pipe_direction dir;
	struct ipa_eth_pipe_setup_info info;
	struct ipa_eth_client *client_info;
	enum ipa_eth_pipe_traffic_type traffic_type;
	u32 tc_bmap;
	/* output params */
	ipa_eth_hdl_t pipe_hdl;
};

/**
 * struct  ipa_eth_client - client info per traffic type
 * provided by offload client
 * @client_type: ethernet client type
 * @inst_id: instance id for dual NIC support
 * @net_dev: network device client belongs to
 * @traffic_type: traffic type
 * @pipe_list: list of pipes with same traffic type
 * @priv: private data for client
 * @test: is test client
 */
struct ipa_eth_client {
	enum ipa_eth_client_type client_type;
	u8 inst_id;

	/* traffic type */
	enum ipa_eth_pipe_traffic_type traffic_type;
	struct list_head pipe_list;

	/* client specific priv data*/
	void *priv;
	bool test;

	/* vendor driver */
	struct net_device *net_dev;
};

/**
 * struct  ipa_eth_perf_profile - To set BandWidth profile
 *
 * @max_supported_bw_mbps: maximum bandwidth needed (in Mbps)
 */
struct ipa_eth_perf_profile {
	u32 max_supported_bw_mbps;
};

/**
 * struct ipa_eth_hdr_info - Header to install on IPA HW
 *
 * @hdr: header to install on IPA HW
 * @hdr_len: length of header
 * @dst_mac_addr_offset: destination mac address offset
 * @hdr_type: layer two header type
 */
struct ipa_eth_hdr_info {
	u8 *hdr;
	u8 hdr_len;
	u8 dst_mac_addr_offset;
	enum ipa_hdr_l2_type hdr_type;
};

/**
 * struct ipa_eth_intf_info - parameters for ipa offload
 *	interface registration
 *
 * @client: ipa ethernet client associated with the interface
 * @is_conn_evt: whether or not trigger periph conn/disconn event
 * @net_dev: network device
 */
struct ipa_eth_intf_info {
	struct ipa_eth_client *client;

	/* trigger iface peripheral event */
	bool is_conn_evt;

	/* IPA internal fields */
	struct net_device *net_dev;
};

int ipa_eth_register_ready_cb(struct ipa_eth_ready *ready_info);
int ipa_eth_unregister_ready_cb(struct ipa_eth_ready *ready_info);
int ipa_eth_client_conn_pipes(struct ipa_eth_client *client);
int ipa_eth_client_disconn_pipes(struct ipa_eth_client *client);
int ipa_eth_client_reg_intf(struct ipa_eth_intf_info *intf);
int ipa_eth_client_unreg_intf(struct ipa_eth_intf_info *intf);
int ipa_eth_client_set_perf_profile(struct ipa_eth_client *client,
	struct ipa_eth_perf_profile *profile);
enum ipa_client_type ipa_eth_get_ipa_client_type_from_eth_type(
	enum ipa_eth_client_type eth_client_type, enum ipa_eth_pipe_direction dir);
bool ipa_eth_client_exist(
	enum ipa_eth_client_type eth_client_type, int inst_id);
int ipa_eth_get_config_type(
	enum ipa_eth_client_type client_type, int inst_id, struct ipa_eth_config *eth_config);
int ipa_eth_qos_get_num_pipes(
	u8 inst_id, u8 *num_pipes, enum ipa_eth_pipe_direction dir);
int ipa_eth_qos_get_qos_info(
	u8 inst_id,
	u8 idx,
	struct ipa_eth_qos_info *info,
	enum ipa_eth_pipe_direction dir);

int ipa_eth_client_enable_pipes(struct ipa_eth_client *client);
int ipa_eth_client_disable_pipes(struct ipa_eth_client *client);

#if IPA_ETH_API_VER >= 8
/* IPA-recommended maximum in-flight offload TX packets (high watermark).
 * Returned in ipa_eth_config.offload_max_pkts by ipa_eth_get_config_type().
 * Suggested low watermark (queue wake): IPA_ETH_OFFLOAD_MAX_PKTS / 2.  */
#define IPA_ETH_OFFLOAD_MAX_PKTS  128

/* Maximum GSO size when segmentation offload is enabled.
 * Assign to netdev->gso_max_size; also returned in
 * ipa_eth_config.offload_gso_max_size by ipa_eth_get_config_type().   */
#define IPA_ETH_OFFLOAD_GSO_MAX_SIZE  64000

/**
 * ipa_eth_offload_tx_dp - Transmit an skb over the ETH offload SW TX path
 * @pipe: TX pipe registered via ipa_eth_client_conn_pipes(); must have
 *        dir == IPA_ETH_PIPE_DIR_TX and a valid client_info back-pointer
 * @skb: packet to transmit; skb->dev->mtu is used as the segmentation MTU
 *
 * For TCP/UDP over IPv4/IPv6, prepends the QMAP extension header
 * (hdr_type=0x3) when IPA segmentation offload is supported, and enqueues the
 * packet into the IPA->ETH TX pipe bound to the pipe's client. Non-offloaded
 * traffic (offload unsupported, or non-TCP/UDP) is forwarded as-is.
 *
 * Returns 0 on success, -EAGAIN if PM is activating (stop TX queue),
 * -ENOSPC if the GSI descriptor ring is full (stop TX queue; restart from
 * the TX-complete notify callback), or a negative errno on failure.
 */
int ipa_eth_offload_tx_dp(struct ipa_eth_client_pipe_info *pipe,
			  struct sk_buff *skb);
#endif /* IPA_ETH_API_VER >= 8 */

#endif // _IPA_ETH_H_
