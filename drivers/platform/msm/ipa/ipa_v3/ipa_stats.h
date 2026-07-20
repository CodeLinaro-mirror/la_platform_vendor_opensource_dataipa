/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 *
 * Copyright (c) 2022-2023, 2025 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#ifndef _IPA_LNX_STATS_I_H_
#define _IPA_LNX_STATS_I_H_

/* This whole header file is a copy of ipa_lnx_agent.h */

/*
 * unique magic number of the IPA_LNX_STATS interface
 */
#define IPA_LNX_STATS_IOC_MAGIC 0x72

#define IPA_LNX_IOC_GET_ALLOC_INFO _IOWR(IPA_LNX_STATS_IOC_MAGIC, \
	IPA_LNX_CMD_GET_ALLOC_INFO, \
	struct ipa_lnx_stats_tlpd_ctx)

#define IPA_LNX_IOC_GET_GENERIC_STATS _IOWR(IPA_LNX_STATS_IOC_MAGIC, \
	IPA_LNX_CMD_GENERIC_STATS, \
	struct ipa_lnx_generic_stats)

#define IPA_LNX_IOC_GET_CLOCK_STATS _IOWR(IPA_LNX_STATS_IOC_MAGIC, \
	IPA_LNX_CMD_CLOCK_STATS, \
	struct ipa_lnx_clock_stats)

#define IPA_LNX_IOC_GET_WLAN_INST_STATS _IOWR(IPA_LNX_STATS_IOC_MAGIC, \
	IPA_LNX_CMD_WLAN_INST_STATS, \
	struct ipa_lnx_wlan_inst_stats)

#define IPA_LNX_IOC_GET_ETH_INST_STATS _IOWR(IPA_LNX_STATS_IOC_MAGIC, \
	IPA_LNX_CMD_ETH_INST_STATS, \
	struct ipa_lnx_eth_inst_stats)

#define IPA_LNX_IOC_GET_USB_INST_STATS _IOWR(IPA_LNX_STATS_IOC_MAGIC, \
	IPA_LNX_CMD_USB_INST_STATS, \
	struct ipa_lnx_usb_inst_stats)

#define IPA_LNX_IOC_GET_MHIP_INST_STATS _IOWR(IPA_LNX_STATS_IOC_MAGIC, \
	IPA_LNX_CMD_MHIP_INST_STATS, \
	struct ipa_lnx_mhip_inst_stats)

#define IPA_LNX_IOC_GET_CONSOLIDATED_STATS _IOWR(IPA_LNX_STATS_IOC_MAGIC, \
	IPA_LNX_CMD_CONSOLIDATED_STATS, \
	int)

#define IPA_LNX_STATS_SUCCESS 0
#define IPA_LNX_STATS_FAILURE -1

#define TLPD_NUM_MAX_PIPES 6
#define TLPD_NUM_MAX_TX_INSTANCES 3
#define TLPD_NUM_MAX_RX_INSTANCES 3

#define TLPD_NUM_MAX_INSTANCES 2

#define IPA_LNX_PIPE_PAGE_RECYCLING_INTERVAL_COUNT 5
#define IPA_LNX_PIPE_PAGE_RECYCLING_INTERVAL_TIME 10 /* In milli second */

#define TLPD_NUM_MAX_RT_FLT_TBL 16
#define TLPD_NUM_MAX_RT_FLT_RULE 30

#define TLPD_NUM_MAX_NUM_EQ 2

#define TLPD_NUM_MAX_NAT_RULE 100

/**
 * This is used to indicate which set of logs is enabled from IPA
 * These bitmapped macros.
 */
#define TLPD_IPA_LOG_TYPE_GENERIC_STATS      0x00001
#define TLPD_IPA_LOG_TYPE_CLOCK_STATS        0x00002
#define TLPD_IPA_LOG_TYPE_WLAN_STATS         0x00004
#define TLPD_IPA_LOG_TYPE_ETH_STATS          0x00008
#define TLPD_IPA_LOG_TYPE_USB_STATS          0x00010
#define TLPD_IPA_LOG_TYPE_MHIP_STATS         0x00020
#define TLPD_IPA_LOG_TYPE_RECYCLE_STATS      0x00040
#define TLPD_IPA_LOG_TYPE_V4_RT_RULE_STATS   0x00080
#define TLPD_IPA_LOG_TYPE_V6_RT_RULE_STATS   0x00100
#define TLPD_IPA_LOG_TYPE_V4_FLT_RULE_STATS  0x00200
#define TLPD_IPA_LOG_TYPE_V6_FLT_RULE_STATS  0x00400
#define TLPD_IPA_LOG_TYPE_V4_NAT_RULE_STATS  0x00800
#define TLPD_IPA_LOG_TYPE_V6_NAT_RULE_STATS  0x01000

/**
 * Look up table for pm stats client names.
 * New entry to be added when new client
 * registers with pm
 */
struct pm_client_name_lookup { char *name; int idx_hdl;};

#define NUM_PM_CLIENT_NAMES 9

/**
 * Every structure is associated with the underlying macro
 * for it's length and that has to be updated every time there
 * is structure modification.This is NOT the sizeof(struct) but
 * it is addition of the specified type of variable included
 * inside the structre. Also update the internal structure lengths
 * in ipa_lnx_tlpd_stats.c to overcome backward and forward
 * compatibility between userspace and driver structures.
 */
/* IPA Linux generic stats structures */
struct pg_recycle_stats {
	uint64_t coal_total_repl_buff;
	uint64_t coal_temp_repl_buff;
	uint64_t def_total_repl_buff;
	uint64_t def_temp_repl_buff;
};

struct exception_stats {
	uint32_t excptn_type_none;
	uint32_t excptn_type_deaggr;
	uint32_t excptn_type_iptype;
	uint32_t excptn_type_pkt_len;
	uint32_t excptn_type_pkt_thrshld;
	uint32_t excptn_type_frag_rule_miss;
	uint32_t excptn_type_sw_flt;
	uint32_t excptn_type_nat;
	uint32_t excptn_type_ipv6_ct;
	uint32_t excptn_type_csum;
};

struct odl_ep_stats {
	uint32_t rx_pkt;
	uint32_t processed_pkt;
	uint32_t dropped_pkt;
	uint32_t num_queue_pkt;
};

struct holb_discard_stats {
	uint32_t client_type;
	uint32_t num_drp_cnt;
	uint32_t num_drp_bytes;
	uint32_t reserved;
};

struct holb_monitor_stats {
	uint32_t client_type;
	uint32_t curr_index;
	uint32_t num_en_cnt;
	uint32_t num_dis_cnt;
};

struct holb_drop_and_mon_stats {
	uint32_t num_holb_disc_pipes;
	uint32_t num_holb_mon_clients;
	struct holb_discard_stats holb_disc_stats[0];
	struct holb_monitor_stats holb_mon_stats[0];
};

struct ipa_lnx_generic_stats {
	uint32_t tx_dma_pkts;
	uint32_t tx_hw_pkts;
	uint32_t tx_non_linear;
	uint32_t tx_pkts_compl;
	uint32_t stats_compl;
	uint32_t active_eps;
	uint32_t wan_rx_empty;
	uint32_t wan_repl_rx_empty;
	uint32_t lan_rx_empty;
	uint32_t lan_repl_rx_empty;
	struct pg_recycle_stats pg_rec_stats;
	struct exception_stats excep_stats;
	struct odl_ep_stats odl_stats;
	struct holb_drop_and_mon_stats holb_stats;
};

/* IPA Linux clock stats structures */
struct pm_client_stats {
	uint32_t pm_client_state;
	uint32_t pm_client_group;
	uint32_t pm_client_bw;
	uint32_t pm_client_hdl;
	uint32_t pm_client_type;
	uint32_t reserved;
};

struct ipa_lnx_clock_stats {
	uint32_t active_clients;
	uint32_t scale_thresh_svs;
	uint32_t scale_thresh_nom;
	uint32_t scale_thresh_tur;
	uint32_t aggr_bw;
	uint32_t curr_clk_vote;
	struct pm_client_stats pm_clnt_stats[0];
};

/* NTN-specific GSI debug stats, used in the NTN-extended GSI debug stats */
struct ipa_lnx_ntn_gsi_tx_debug_stats {
	uint32_t last_db_value;
	uint32_t next_re;
	uint32_t malformed_tre;
	uint32_t invalid_tre_cnt;
	uint32_t rollbacks_cnt;
	uint32_t outstanding_tlvs_cnt;
};

struct ipa_lnx_ntn_gsi_rx_debug_stats {
	uint32_t last_db_value;
	uint32_t next_re;
	uint32_t malformed_tre;
	uint32_t zero_len_pkt_cnt;
	uint32_t invalid_tre_cnt;
	uint32_t rollbacks_cnt;
	uint32_t outstanding_tlvs_cnt;
};

/* Generic instance structures */
struct ipa_lnx_gsi_rx_debug_stats {
	uint32_t rx_client;
	uint32_t num_rx_ring_100_perc_with_pack;
	uint32_t num_rx_ring_0_perc_with_pack;
	uint32_t num_rx_ring_above_75_perc_pack;
	uint32_t num_rx_ring_above_25_perc_pack;
	uint32_t num_rx_ring_stats_polled;
	uint32_t num_rx_drop_stats;
	uint32_t rx_summary;
};

struct ipa_lnx_gsi_tx_debug_stats {
	uint32_t tx_client;
	uint32_t num_tx_ring_100_perc_with_cred;
	uint32_t num_tx_ring_0_perc_with_cred;
	uint32_t num_tx_ring_above_75_perc_cred;
	uint32_t num_tx_ring_above_25_perc_cred;
	uint32_t num_tx_ring_stats_polled;
	uint32_t tx_summary;
	uint32_t reserved;
};

/* Extended GSI stats for NTN-protocol clients (NTN/NTN3/IEMAC only) */
struct ipa_lnx_gsi_tx_ntn_debug_stats {
	struct ipa_lnx_gsi_tx_debug_stats base;
	struct ipa_lnx_ntn_gsi_tx_debug_stats ntn_stats;
};

struct ipa_lnx_gsi_rx_ntn_debug_stats {
	struct ipa_lnx_gsi_rx_debug_stats base;
	struct ipa_lnx_ntn_gsi_rx_debug_stats ntn_stats;
};

struct ipa_lnx_gsi_debug_stats {
	uint32_t num_tx_instances;
	uint32_t num_rx_instances;
	struct ipa_lnx_gsi_tx_debug_stats gsi_tx_dbg_stats[0];
	struct ipa_lnx_gsi_rx_debug_stats gsi_rx_dbg_stats[0];
};

struct ipa_lnx_pipe_info {
	uint64_t gsi_chan_ring_bp;
	uint64_t gsi_chan_ring_rp;
	uint64_t gsi_chan_ring_wp;
	uint64_t gsi_evt_ring_bp;
	uint64_t gsi_evt_ring_rp;
	uint64_t gsi_evt_ring_wp;
	uint32_t gsi_evt_ring_len;
	uint32_t gsi_chan_ring_len;
	uint32_t buff_size;
	uint32_t num_free_buff;
	uint32_t gsi_ipa_if_tlv;
	uint32_t gsi_ipa_if_aos;
	uint32_t gsi_desc_size;
	uint32_t pipe_num;
	uint32_t direction;
	uint32_t client_type;
	uint32_t gsi_chan_num;
	uint32_t gsi_evt_num;
	uint32_t is_common_evt_ring;
	uint32_t gsi_prot_type;
	uint32_t gsi_chan_state;
	uint32_t gsi_chan_stop_stm;
	uint32_t gsi_poll_mode;
	uint32_t gsi_db_in_bytes;
};

/* IPA Linux wlan instance stats structures */
struct wlan_instance_info {
	uint32_t instance_id;
	uint32_t wdi_ver;
	uint32_t wlan_mode;
	uint32_t wdi_over_gsi;
	uint32_t dbs_mode;
	uint32_t pm_bandwidth;
	uint32_t num_pipes;
	uint32_t reserved;
	struct ipa_lnx_gsi_debug_stats gsi_debug_stats;
	struct ipa_lnx_pipe_info pipe_info[0];
};

struct ipa_lnx_wlan_inst_stats {
	uint32_t num_wlan_instance;
	uint32_t reserved;
	struct wlan_instance_info instance_info[0];
};

/* IPA Linux eth instance stats structures */
struct eth_instance_info {
	uint32_t instance_id;
	uint32_t eth_mode;
	uint32_t pm_bandwidth;
	uint32_t num_pipes;
	struct ipa_lnx_gsi_debug_stats gsi_debug_stats;
	struct ipa_lnx_pipe_info pipe_info[0];
};

struct ipa_lnx_eth_inst_stats {
	uint32_t num_eth_instance;
	uint32_t reserved;
	struct eth_instance_info instance_info[0];
};

/* IPA Linux usb instance stats structures */
struct usb_instance_info {
	uint32_t instance_id;
	uint32_t usb_mode;
	uint32_t pm_bandwidth;
	uint32_t num_pipes;
	struct ipa_lnx_gsi_debug_stats gsi_debug_stats;
	struct ipa_lnx_pipe_info pipe_info[0];
};

struct ipa_lnx_usb_inst_stats {
	uint32_t num_usb_instance;
	uint32_t reserved;
	struct usb_instance_info instance_info[0];
};

/* IPA Linux mhip instance stats structures */
struct mhip_instance_info {
	uint32_t instance_id;
	uint32_t mhip_mode;
	uint32_t pm_bandwidth;
	uint32_t num_pipes;
	struct ipa_lnx_gsi_debug_stats gsi_debug_stats;
	struct ipa_lnx_pipe_info pipe_info[0];
};

struct ipa_lnx_mhip_inst_stats {
	uint32_t num_mhip_instance;
	uint32_t reserved;
	struct mhip_instance_info instance_info[0];
};

struct ipa_lnx_consolidated_stats {
	uint64_t log_type_mask;
	struct ipa_lnx_generic_stats *generic_stats;
	struct ipa_lnx_clock_stats *clock_stats;
	struct ipa_lnx_wlan_inst_stats *wlan_stats;
	struct ipa_lnx_eth_inst_stats *eth_stats;
	struct ipa_lnx_usb_inst_stats *usb_stats;
	struct ipa_lnx_mhip_inst_stats *mhip_stats;
	struct ipa_lnx_pipe_page_recycling_stats *recycle_stats;
	struct ipa_lnx_v4_rt_rule_stats *v4_rt_rule_stats;
	struct ipa_lnx_v6_rt_rule_stats *v6_rt_rule_stats;
	struct ipa_lnx_v4_flt_rule_stats *v4_flt_rule_stats;
	struct ipa_lnx_v6_flt_rule_stats *v6_flt_rule_stats;
	struct ipa_lnx_v4_nat_rule_stats *v4_nat_rule_stats;
	struct ipa_lnx_v6_nat_rule_stats *v6_nat_rule_stats;
};

enum rx_channel_type {
	RX_WAN_COALESCING,
	RX_WAN_DEFAULT,
	RX_WAN_LOW_LAT_DATA,
	RX_LAN_COALESCING,
	RX_LAN_DEFAULT,
	RX_CHANNEL_MAX,
};

struct ipa_lnx_recycling_stats {
	uint64_t total_cumulative;
	uint64_t recycle_cumulative;
	uint64_t temp_cumulative;
	uint64_t total_diff;
	uint64_t recycle_diff;
	uint64_t temp_diff;
	uint64_t valid;
};

/**
 * The consolidated stats will be in the 0th index.
 * Diff. between each interval values will be in
 * indices 1 to (IPA_LNX_PIPE_PAGE_RECYCLING_INTERVAL_COUNT - 1)
 * @new_set: Indicates if this is the new set of data or previous data.
 * @interval_time_ms: Interval time in millisecond
 */
struct ipa_lnx_pipe_page_recycling_stats {
	uint32_t interval_time_in_ms;
	uint32_t default_coal_stats_index;
	uint32_t low_lat_stats_index;
	uint32_t sequence_id;
	uint64_t reserved;
	struct ipa_lnx_recycling_stats rx_channel[RX_CHANNEL_MAX][IPA_LNX_PIPE_PAGE_RECYCLING_INTERVAL_COUNT];
};

/**
 * Look up table for Routing table.
 * New entry to be added when new routing table is created
 */
struct rt_table_name_lookup { char *name; int index;};

enum rt_table_types {
	ipa_dflt_rt = 1,
	COMRTBLLANv4 = 2,
	WANRTBLv4 = 3,
	ODURTBLv4 = 4,
	ipa_dflt_wan_rt = 5,
	COMRTBLv6 = 6,
	WANRTBLv6 = 7,
	ODURTBLv6 = 8,
	RT_TABLE_NAME_MAX = 9,
};

struct ipa_lnx_offset_meq_128 {
	uint64_t offset : 32;
	uint64_t reserved : 32;
	uint8_t value[16];
	uint8_t mask[16];
};

struct ipa_lnx_offset_meq {
	uint64_t offset;
	uint64_t value : 32;
	uint64_t mask : 32;
};

struct ipa_lnx_ihl_offset_range {
	uint64_t offset : 32;
	uint64_t range_low : 16;
	uint64_t range_high : 16;
};

struct ipa_lnx_ihl_offset_eq {
	uint64_t offset : 32;
	uint64_t value : 32;
};

struct ipa_lnx_v4_rt_rules {
	uint64_t rule_idx : 16;
	uint64_t rule_id : 16;
	uint64_t retain_hdr : 8;
	uint64_t close_aggr_irq_mode : 8;
	uint64_t ttl_update : 8;
	uint64_t qos_class : 8;

	uint64_t skip_ingress : 8;
	uint64_t esp_after_udp : 8;
	uint64_t is_rt_hw : 1;
	uint64_t is_in_sram : 1;
	uint64_t reserved : 46;

	uint64_t enable_stats : 8;
	uint64_t counter_id : 8;
	uint64_t hashable : 8;
	uint64_t proc_ctx_valid : 2;
	uint64_t dscp_mark : 6;
	uint64_t offset_words : 32;

	uint64_t attribute_mask : 32;
	uint64_t destination : 16;
	uint64_t end_point : 16;

	uint64_t tos : 8;
	uint64_t tos_value : 8;
	uint64_t tos_mask : 8;
	uint64_t protocol : 8;
	uint64_t l2tp_inner_ip_type : 8;
	uint64_t frag : 8;
	uint64_t tcp_syn : 8;
	uint64_t tcp_syn_l2tp : 8;

	uint64_t src_addr : 32;
	uint64_t src_addr_mask : 32;

	uint64_t dst_addr : 32;
	uint64_t dst_addr_mask : 32;

	uint64_t src_port_low : 16;
	uint64_t src_port_high : 16;
	uint64_t dst_port_low : 16;
	uint64_t dst_port_high : 16;

	uint64_t src_port : 16;
	uint64_t dst_port : 16;
	uint64_t payload_length : 16;
	uint64_t ether_type : 16;

	uint64_t spi : 32;
	uint64_t vlan_id : 16;
	uint64_t type : 8;
	uint64_t code : 8;

	uint64_t src_mac_addr;
	uint64_t src_mac_addr_mask;
	uint64_t dst_mac_addr;
	uint64_t dst_mac_addr_mask;

	uint64_t meta_data : 32;
	uint64_t meta_data_mask : 32;

	uint64_t pure_ack : 8;
	uint64_t frag_encoding : 8;
	uint64_t prio : 8;
	uint64_t max_prio : 8;
	uint64_t reserved4 : 32;

	/* Attribute equation */
	uint64_t protocol_eq : 8;
	uint64_t num_offset_meq128 : 8;
	uint64_t num_offset_meq32 : 8;
	uint64_t num_ihl_offset_meq32 : 8;
	uint64_t is_metadata_meq32_persent : 8;
	uint64_t num_ihl_offset_range_16 : 8;
	uint64_t num_ihl_offset_eq16 : 8;
	uint64_t num_ihl_offset_eq32 : 8;

	struct ipa_lnx_offset_meq_128 offset_meq_128[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_offset_meq offset_meq32[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_offset_meq ihl_offset_meq32[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_offset_meq metadata_meq32[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_ihl_offset_range ihl_offset_range16[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_ihl_offset_eq ihl_offset_eq16[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_ihl_offset_eq ihl_offset_eq32[TLPD_NUM_MAX_NUM_EQ];
};

struct ipa_lnx_v6_rt_rules {
	uint64_t rule_idx : 16;
	uint64_t rule_id : 16;
	uint64_t retain_hdr : 8;
	uint64_t close_aggr_irq_mode : 8;
	uint64_t ttl_update : 8;
	uint64_t qos_class : 8;

	uint64_t skip_ingress : 8;
	uint64_t esp_after_udp : 8;
	uint64_t is_rt_hw : 1;
	uint64_t is_in_sram : 1;
	uint64_t reserved : 46;

	uint64_t enable_stats : 8;
	uint64_t counter_id : 8;
	uint64_t hashable : 8;
	uint64_t proc_ctx_valid : 2;
	uint64_t dscp_mark : 6;
	uint64_t offset_words : 32;

	uint64_t attribute_mask : 32;
	uint64_t destination : 16;
	uint64_t end_point : 16;

	uint64_t tc : 8;
	uint64_t flow_label : 8;
	uint64_t tos_value : 8;
	uint64_t tos_mask : 8;
	uint64_t nxt_hdr : 8;
	uint64_t nxt_hdr_ext : 8;
	uint64_t type : 8;
	uint64_t code : 8;

	uint32_t src_addr[4];
	uint32_t src_addr_mask[4];
	uint32_t dst_addr[4];
	uint32_t dst_addr_mask[4];

	uint64_t src_port_low : 16;
	uint64_t src_port_high : 16;
	uint64_t dst_port_low : 16;
	uint64_t dst_port_high : 16;

	uint64_t spi : 32;
	uint64_t src_port : 16;
	uint64_t dst_port : 16;

	uint64_t payload_length : 16;
	uint64_t ether_type : 16;
	uint64_t vlan_id : 16;
	uint64_t reserved2 : 16;

	uint64_t tcp_syn : 8;
	uint64_t tcp_syn_l2tp : 8;
	uint64_t frag : 8;
	uint64_t l2tp_inner_ip_type : 8;
	uint64_t reserved3 : 32;

	uint64_t src_mac_addr;
	uint64_t src_mac_addr_mask;
	uint64_t dst_mac_addr;
	uint64_t dst_mac_addr_mask;

	uint64_t meta_data : 32;
	uint64_t meta_data_mask : 32;

	uint64_t pure_ack : 8;
	uint64_t frag_encoding : 8;
	uint64_t prio : 8;
	uint64_t max_prio : 8;
	uint64_t reserved4 : 32;

	/* Attribute equation */
	uint64_t protocol_eq : 8;
	uint64_t num_offset_meq128 : 8;
	uint64_t num_offset_meq32 : 8;
	uint64_t num_ihl_offset_meq32 : 8;
	uint64_t is_metadata_meq32_persent : 8;
	uint64_t num_ihl_offset_range_16 : 8;
	uint64_t num_ihl_offset_eq16 : 8;
	uint64_t num_ihl_offset_eq32 : 8;

	struct ipa_lnx_offset_meq_128 offset_meq_128[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_offset_meq offset_meq32[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_offset_meq ihl_offset_meq32[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_offset_meq metadata_meq32[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_ihl_offset_range ihl_offset_range16[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_ihl_offset_eq ihl_offset_eq16[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_ihl_offset_eq ihl_offset_eq32[TLPD_NUM_MAX_NUM_EQ];
};

struct ipa_lnx_v4_rt_rule_table {
	uint64_t tbl_index : 16;
	uint64_t tbl_ref_count : 16;
	uint64_t num_rt_rule : 16;
	uint64_t tbl_name : 8;
	uint64_t reserved : 8;
	char rt_tbl_name[40];
	struct ipa_lnx_v4_rt_rules v4_rt_rule[TLPD_NUM_MAX_RT_FLT_RULE];
};

struct ipa_lnx_v6_rt_rule_table {
	uint64_t tbl_index : 16;
	uint64_t tbl_ref_count : 16;
	uint64_t num_rt_rule : 16;
	uint64_t tbl_name : 8;
	uint64_t reserved : 8;
	char rt_tbl_name[40];
	struct ipa_lnx_v6_rt_rules v6_rt_rule[TLPD_NUM_MAX_RT_FLT_RULE];
};

struct ipa_lnx_v4_rt_rule_stats {
	uint32_t ip4_rt_tbl_hash_local : 8;
	uint32_t ip4_rt_tbl_nhash_local : 8;
	uint32_t num_v4_rt_table : 16;
	struct ipa_lnx_v4_rt_rule_table v4_rt_tbl[TLPD_NUM_MAX_RT_FLT_TBL];
};

struct ipa_lnx_v6_rt_rule_stats {
	uint32_t ip6_rt_tbl_hash_local : 8;
	uint32_t ip6_rt_tbl_nhash_local : 8;
	uint32_t num_v6_rt_table : 16;
	struct ipa_lnx_v6_rt_rule_table v6_rt_tbl[TLPD_NUM_MAX_RT_FLT_TBL];
};


/* IPA Linux v4/v6 filtering rule stats structure */
struct ipa_lnx_v4_flt_rules {
	uint64_t rule_idx : 32;
	uint64_t ep_idx : 32;

	uint64_t action : 8;
	uint64_t retain_hdr : 8;
	uint64_t attrib_mask : 16;
	uint64_t rt_table_index : 32;

	uint64_t rule_id : 16;
	uint64_t equation : 8;
	uint64_t in_sys : 8;
	uint64_t force_sys : 8;
	uint64_t hashable : 8;
	uint64_t enable_stats : 8;
	uint64_t counter_id : 8;

	uint64_t pdn_index : 8;
	uint64_t set_metadata : 8;
	uint64_t close_aggr_irq_mode : 16;
	uint64_t ttl_update : 8;
	uint64_t qos_class : 8;
	uint64_t esp_after_udp : 8;
	uint64_t is_flt_hw : 8;

	uint64_t tos : 8;
	uint64_t tos_value : 8;
	uint64_t tos_mask : 8;
	uint64_t protocol : 8;
	uint64_t l2tp_inner_ip_type : 8;
	uint64_t frag : 8;
	uint64_t tcp_syn : 8;
	uint64_t tcp_syn_l2tp : 8;

	uint64_t src_addr : 32;
	uint64_t src_addr_mask : 32;

	uint64_t dst_addr : 32;
	uint64_t dst_addr_mask : 32;

	uint64_t src_port_low : 16;
	uint64_t src_port_high : 16;
	uint64_t dst_port_low : 16;
	uint64_t dst_port_high : 16;

	uint64_t src_port : 16;
	uint64_t dst_port : 16;
	uint64_t payload_length : 16;
	uint64_t ether_type : 16;

	uint64_t spi : 32;
	uint64_t vlan_id : 16;
	uint64_t type : 8;
	uint64_t code : 8;

	uint64_t src_mac_addr;
	uint64_t src_mac_addr_mask;
	uint64_t dst_mac_addr;
	uint64_t dst_mac_addr_mask;

	uint64_t meta_data : 32;
	uint64_t meta_data_mask : 32;

	uint64_t pure_ack : 8;
	uint64_t frag_encoding : 8;
	uint64_t prio : 8;
	uint64_t max_prio : 8;
	uint64_t reserved4 : 32;

	/* Attribute equation */
	uint64_t protocol_eq : 8;
	uint64_t num_offset_meq128 : 8;
	uint64_t num_offset_meq32 : 8;
	uint64_t num_ihl_offset_meq32 : 8;
	uint64_t is_metadata_meq32_persent : 8;
	uint64_t num_ihl_offset_range_16 : 8;
	uint64_t num_ihl_offset_eq16 : 8;
	uint64_t num_ihl_offset_eq32 : 8;

	struct ipa_lnx_offset_meq_128 offset_meq_128[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_offset_meq offset_meq32[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_offset_meq ihl_offset_meq32[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_offset_meq metadata_meq32[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_ihl_offset_range ihl_offset_range16[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_ihl_offset_eq ihl_offset_eq16[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_ihl_offset_eq ihl_offset_eq32[TLPD_NUM_MAX_NUM_EQ];
};

struct ipa_lnx_v6_flt_rules {
	uint64_t rule_idx : 32;
	uint64_t ep_idx : 32;

	uint64_t rt_table_index : 32;
	uint64_t attrib_mask : 16;
	uint64_t action : 8;
	uint64_t retain_hdr : 8;

	uint64_t equation : 8;
	uint64_t hashable : 8;
	uint64_t in_sys : 8;
	uint64_t force_sys : 8;
	uint64_t enable_stats : 8;
	uint64_t counter_id : 8;
	uint64_t rule_id : 16;

	uint64_t pdn_index : 8;
	uint64_t set_metadata : 8;
	uint64_t close_aggr_irq_mode : 16;
	uint64_t ttl_update : 8;
	uint64_t qos_class : 8;
	uint64_t esp_after_udp : 8;
	uint64_t is_flt_hw : 8;

	uint64_t tc : 8;
	uint64_t flow_label : 8;
	uint64_t tos_value : 8;
	uint64_t tos_mask : 8;
	uint64_t nxt_hdr : 8;
	uint64_t protocol : 8;
	uint64_t type : 8;
	uint64_t code : 8;

	uint32_t src_addr[4];
	uint32_t src_addr_mask[4];
	uint32_t dst_addr[4];
	uint32_t dst_addr_mask[4];

	uint64_t src_port_low : 16;
	uint64_t src_port_high : 16;
	uint64_t dst_port_low : 16;
	uint64_t dst_port_high : 16;

	uint64_t spi : 32;
	uint64_t src_port : 16;
	uint64_t dst_port : 16;

	uint64_t payload_length : 16;
	uint64_t ether_type : 16;
	uint64_t vlan_id : 16;
	uint64_t reserved2 : 16;

	uint64_t tcp_syn : 8;
	uint64_t tcp_syn_l2tp : 8;
	uint64_t frag : 8;
	uint64_t l2tp_inner_ip_type : 8;
	uint64_t reserved3 : 32;

	uint64_t src_mac_addr;
	uint64_t src_mac_addr_mask;
	uint64_t dst_mac_addr;
	uint64_t dst_mac_addr_mask;

	uint64_t meta_data : 32;
	uint64_t meta_data_mask : 32;

	uint64_t pure_ack : 8;
	uint64_t frag_encoding : 8;
	uint64_t prio : 8;
	uint64_t max_prio : 8;
	uint64_t reserved4 : 32;

	/* Attribute equation */
	uint64_t protocol_eq : 8;
	uint64_t num_offset_meq128 : 8;
	uint64_t num_offset_meq32 : 8;
	uint64_t num_ihl_offset_meq32 : 8;
	uint64_t is_metadata_meq32_persent : 8;
	uint64_t num_ihl_offset_range_16 : 8;
	uint64_t num_ihl_offset_eq16 : 8;
	uint64_t num_ihl_offset_eq32 : 8;

	struct ipa_lnx_offset_meq_128 offset_meq_128[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_offset_meq offset_meq32[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_offset_meq ihl_offset_meq32[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_offset_meq metadata_meq32[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_ihl_offset_range ihl_offset_range16[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_ihl_offset_eq ihl_offset_eq16[TLPD_NUM_MAX_NUM_EQ];
	struct ipa_lnx_ihl_offset_eq ihl_offset_eq32[TLPD_NUM_MAX_NUM_EQ];
};

struct ipa_lnx_v4_flt_rule_table {
	uint64_t tbl_index : 32;
	uint64_t num_flt_rule : 32;
	struct ipa_lnx_v4_flt_rules flt_rule[TLPD_NUM_MAX_RT_FLT_RULE];
};

struct ipa_lnx_v6_flt_rule_table {
	uint64_t tbl_index : 32;
	uint64_t num_flt_rule : 32;
	struct ipa_lnx_v6_flt_rules flt_rule[TLPD_NUM_MAX_RT_FLT_RULE];
};

struct ipa_lnx_v4_flt_rule_stats {
	uint64_t ip4_flt_tbl_hash_local : 8;
	uint64_t ip4_flt_tbl_nhash_local : 8;
	uint64_t num_v4_flt_table : 16;
	struct ipa_lnx_v4_flt_rule_table v4_flt_tbl[TLPD_NUM_MAX_RT_FLT_TBL];
};

struct ipa_lnx_v6_flt_rule_stats {
	uint64_t ip6_flt_tbl_hash_local : 8;
	uint64_t ip6_flt_tbl_nhash_local : 8;
	uint64_t num_v6_flt_table : 16;
	struct ipa_lnx_v6_flt_rule_table v6_flt_tbl[TLPD_NUM_MAX_RT_FLT_TBL];
};

/* IPA Linux NAT stats structure */
struct ipa_lnx_nat_table {
	uint64_t enable: 16;
	uint64_t direct_to_apps : 16;
	uint64_t index_tbl_entry : 16;
	uint64_t tcp_udp_chksum : 16;

	uint64_t private_ip : 32;
	uint64_t target_ip : 32;

	uint64_t private_port : 16;
	uint64_t target_port : 16;
	uint64_t public_port : 16;
	uint64_t ip_checksum : 16;

	uint64_t protocol : 8;
	uint64_t time_stamp : 24;
	uint64_t next_index : 16;
	uint64_t prev_index : 16;

	uint64_t entry_index : 16;
	uint64_t pdn_index : 1;
	uint64_t ucp : 1;
	uint64_t address_in_system : 1;
	uint64_t uc_activation_index : 13;
	uint64_t tbl_in_sram : 1;
	uint64_t is_expansion_table : 1;
	uint64_t reserved : 30;
};

struct ipa_lnx_nat_idx_table {
	uint64_t entry_index : 16;
	uint64_t tbl_entry : 16;
	uint64_t nxt_index : 16;
	uint64_t tbl_in_sram : 1;
	uint64_t is_expansion_table : 1;
	uint64_t reserved : 14;
};

struct ipa_lnx_pdn_table {
	uint32_t public_ip;
	uint32_t src_metadata;
	uint32_t dst_metadata;
	uint32_t valid;
};

struct ipa_lnx_v6_ct_table {
	uint64_t direct_to_apps : 8;
	uint64_t enable : 8;
	uint64_t in_allowed : 8;
	uint64_t out_allowed : 8;
	uint64_t time_stamp : 24;
	uint64_t protocol : 8;

	uint64_t src_ipv6_lsb;
	uint64_t src_ipv6_msb;
	uint64_t dest_ipv6_lsb;
	uint64_t dest_ipv6_msb;

	uint64_t next_index : 16;
	uint64_t dest_port : 16;
	uint64_t src_port : 16;
	uint64_t prev_index : 16;
};

struct ipa_lnx_v4_nat_rule_stats {
	uint64_t is_nat_initialized : 8;
	uint64_t is_hw_nat : 8;
	uint64_t ovarall_sram_entries : 16;
	uint64_t reserved : 32;

	uint64_t num_nat : 16;
	uint64_t num_nat_idx : 16;

	struct ipa_lnx_nat_table nat_tbl[TLPD_NUM_MAX_NAT_RULE];
	struct ipa_lnx_nat_idx_table nat_idx_tbl[TLPD_NUM_MAX_NAT_RULE];
};

struct ipa_lnx_v6_nat_rule_stats {
	uint64_t is_nat_initialized : 8;
	uint64_t is_hw_nat : 8;
	uint64_t ovarall_sram_entries : 16;
	uint64_t reserved : 32;

	uint64_t num_pdn : 16;
	uint64_t num_v6_ct : 16;

	struct ipa_lnx_pdn_table pdn_tbl[TLPD_NUM_MAX_NAT_RULE];
	struct ipa_lnx_v6_ct_table v6_ct_tbl[TLPD_NUM_MAX_NAT_RULE];
};

/* Explain below structures */
struct ipa_lnx_each_inst_alloc_info {
	uint32_t pipes_client_type[TLPD_NUM_MAX_PIPES];
	uint32_t tx_inst_client_type[TLPD_NUM_MAX_TX_INSTANCES];
	uint32_t rx_inst_client_type[TLPD_NUM_MAX_RX_INSTANCES];
	uint32_t num_pipes;
	uint32_t num_tx_instances;
	uint32_t num_rx_instances;
	uint32_t reserved;
};

struct ipa_lnx_rt_flt_alloc_info {
	uint16_t num_v4_tables;
	uint16_t num_v6_tables;
	uint8_t num_v4_rules[TLPD_NUM_MAX_RT_FLT_TBL];
	uint8_t num_v6_rules[TLPD_NUM_MAX_RT_FLT_TBL];
	uint8_t v4_metadata_meq32_present[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
	uint8_t num_v4_offset_meq128s[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
	uint8_t num_v4_offset_meq32s[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
	uint8_t num_v4_ihl_offset_eq16s[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
	uint8_t num_v4_ihl_offset_eq32s[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
	uint8_t num_v4_ihl_offset_meq32s[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
	uint8_t num_v4_ihl_offset_range_16s[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
	uint8_t v6_metadata_meq32_present[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
	uint8_t num_v6_offset_meq128s[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
	uint8_t num_v6_offset_meq32s[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
	uint8_t num_v6_ihl_offset_meq32s[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
	uint8_t num_v6_ihl_offset_range_16s[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
	uint8_t num_v6_ihl_offset_eq16s[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
	uint8_t num_v6_ihl_offset_eq32s[TLPD_NUM_MAX_RT_FLT_TBL][TLPD_NUM_MAX_RT_FLT_RULE];
};

struct ipa_lnx_stats_alloc_info {
	uint32_t num_holb_drop_stats_clients;
	uint32_t num_holb_mon_stats_clients;
	uint32_t num_pm_clients;
	uint32_t num_wlan_instances;
	uint32_t num_eth_instances;
	uint32_t num_usb_instances;
	uint32_t num_mhip_instances;
	uint32_t num_page_rec_interval;
	struct ipa_lnx_rt_flt_alloc_info rt_alloc_info;
	struct ipa_lnx_rt_flt_alloc_info flt_alloc_info;
	uint16_t num_nat_tbl;
	uint16_t num_nat_idx_tbl;
	uint16_t num_pdn_tbl;
	uint16_t num_v6_ct_tbl;
	struct ipa_lnx_each_inst_alloc_info wlan_inst_info[TLPD_NUM_MAX_INSTANCES];
	struct ipa_lnx_each_inst_alloc_info eth_inst_info[TLPD_NUM_MAX_INSTANCES];
	struct ipa_lnx_each_inst_alloc_info usb_inst_info[TLPD_NUM_MAX_INSTANCES];
	struct ipa_lnx_each_inst_alloc_info mhip_inst_info[TLPD_NUM_MAX_INSTANCES];
};

struct ipa_lnx_stats_tlpd_ctx {
	uint32_t usb_teth_prot[TLPD_NUM_MAX_INSTANCES];
	uint32_t log_type_mask;
	struct ipa_lnx_stats_alloc_info alloc_info;
};

/* enum ipa_lnx_stats_ioc_cmd_type - IOCTL Command types for IPA lnx stats
 *
 */
enum ipa_lnx_stats_ioc_cmd_type {
	IPA_LNX_CMD_GET_ALLOC_INFO,
	IPA_LNX_CMD_GENERIC_STATS,
	IPA_LNX_CMD_CLOCK_STATS,
	IPA_LNX_CMD_WLAN_INST_STATS,
	IPA_LNX_CMD_ETH_INST_STATS,
	IPA_LNX_CMD_USB_INST_STATS,
	IPA_LNX_CMD_MHIP_INST_STATS,
	IPA_LNX_CMD_CONSOLIDATED_STATS,
	IPA_LNX_CMD_STATS_MAX,
};

int ipa_tlpd_stats_init(void);

/* Peripheral stats for Q6, should be in the same order, defined by Q6 */
struct ipa_peripheral_mdm_stats {
	uint32_t canary;

	uint16_t num_entries;
	uint16_t reserved;

	/* TLV for number of peripherals connected to APROC */
	/* value = IPA_PER_STATS_TYPE_NUM_PERS */
	uint16_t periph_id;
	uint16_t periph_len;
	uint32_t periph_val;

	/* TLV for number of periphers from/to traffic flowing from modem */
	/* value = IPA_PER_STATS_TYPE_NUM_PERS_WWAN */
	uint16_t periph_wwan_id;
	uint16_t periph_wwan_len;
	uint32_t periph_wwan_val;

	/* TLV for bitmask for active/connected peripherals */
	/* value = IPA_PER_STATS_TYPE_PER_TYPE */
	uint16_t periph_type_id;
	uint16_t periph_type_len;
	uint32_t periph_type_val;

	/* TLV for Current gen info if PCIe interconnect is valid */
	/* value = IPA_PER_STATS_TYPE_PCIE_GEN */
	uint16_t pcie_gen_type_id;
	uint16_t pcie_gen_type_len;
	uint32_t pcie_gen_type_val;

	/* TLV for Current width info if PCIe interconnect is valid */
	/* value = IPA_PER_STATS_TYPE_PCIE_WIDTH */
	uint16_t pcie_width_type_id;
	uint16_t pcie_width_type_len;
	uint32_t pcie_width_type_val;

	/* TLV for Max PCIe speed in current gen in Mbps */
	/* value = IPA_PER_STATS_TYPE_PCIE_MAX_SPEED */
	uint16_t pcie_max_speed_id;
	uint16_t pcie_max_speed_len;
	uint32_t pcie_max_speed_val;

	/* TLV for number PCIe LPM transitions */
	/* value = IPA_PER_STATS_TYPE_PCIE_NUM_LPM */
	uint16_t pcie_num_lpm_trans_id;
	uint16_t pcie_num_lpm_trans_len;
	uint16_t pcie_num_lpm_trans_d3;
	uint16_t pcie_num_lpm_trans_m1;
	uint16_t pcie_num_lpm_trans_m2;
	uint16_t pcie_num_lpm_trans_m0;

	/* TLV for USB enumeration type */
	/* value = IPA_PER_STATS_TYPE_USB_TYPE */
	uint16_t usb_enum_id;
	uint16_t usb_enum_len;
	uint32_t usb_enum_value;

	/* TLV for Current USB protocol enumeration if active */
	/* value = IPA_PER_STATS_TYPE_USB_PROT */
	uint16_t usb_prot_enum_id;
	uint16_t usb_prot_enum_len;
	uint32_t usb_prot_enum_value;

	/* TLV for Max USB speed in current gen in Mbps */
	/* value = IPA_PER_STATS_TYPE_USB_MAX_SPEED */
	uint16_t usb_max_speed_id;
	uint16_t usb_max_speed_len;
	uint32_t usb_max_speed_val;

	/* TLV for Total number of USB plug in/outs */
	/* value = IPA_PER_STATS_TYPE_USB_PIPO */
	uint16_t usb_pipo_id;
	uint16_t usb_pipo_len;
	uint32_t usb_pipo_val;

	/* TLV for Wifi enumeration type*/
	/* value = IPA_PER_STATS_TYPE_WIFI_ENUM_TYPE */
	uint16_t wifi_enum_type_id;
	uint16_t wifi_enum_type_len;
	uint32_t wifi_enum_type_val;

	/* TLV for Theoritical Max WLAN speed in current gen in Mbps (pipe for 5GHz in case of dual band) */
	/* value = IPA_PER_STATS_TYPE_WIFI_MAX_SPEED */
	uint16_t wifi_max_speed_id;
	uint16_t wifi_max_speed_len;
	uint32_t wifi_max_speed_val;

	/* TLV for Theoretical Max WLAN speed on the 2.4GHz pipe, value of 0 means disabled */
	/* value = IPA_PER_STATS_TYPE_WIFI_DUAL_BAND_EN */
	uint16_t wifi_dual_band_enabled_id;
	uint16_t wifi_dual_band_enabled_len;
	uint32_t wifi_dual_band_enabled_val;

	/* TLV for the type of ethernet client - Realtek/AQC */
	/* value = IPA_PER_STATS_TYPE_ETH_CLIENT */
	uint16_t eth_client_id;
	uint16_t eth_client_len;
	uint32_t eth_client_val;

	/* TLV for Max Eth link speed */
	/* value = IPA_PER_STATS_TYPE_ETH_MAX_SPEED */
	uint16_t eth_max_speed_id;
	uint16_t eth_max_speed_len;
	uint32_t eth_max_speed_val;

	/* TLV for Total number of bytes txferred through IPA DMA channels over PCIe */
	/* For cases where GSI used for QDSS direct DMA, need to extract bytes stats from GSI FW */
	/* value = IPA_PER_STATS_TYPE_IPA_DMA_BYTES */
	uint16_t ipa_dma_bytes_id;
	uint16_t ipa_dma_bytes_len;
	uint32_t ipa_dma_bytes_val;

	/* TLV for number of wifi peripherals connected to APROC */
	/* value = IPA_PER_STATS_TYPE_WIFI_HOLB_UC */
	uint16_t wifi_holb_uc_stats_id;
	uint16_t wifi_holb_uc_stats_len;
	uint16_t wifi_holb_uc_stats_num_periph_bad;
	uint16_t wifi_holb_uc_stats_num_periph_recovered;

	/* TLV for number of eth peripherals connected to APROC */
	/* value = IPA_PER_STATS_TYPE_ETH_HOLB_UC */
	uint16_t eth_holb_uc_stats_id;
	uint16_t eth_holb_uc_stats_len;
	uint16_t eth_holb_uc_stats_num_periph_bad;
	uint16_t eth_holb_uc_stats_num_periph_recovered;

	/* TLV for number of usb peripherals connected to APROC */
	/* value = IPA_PER_STATS_TYPE_USB_HOLB_UC */
	uint16_t usb_holb_uc_stats_id;
	uint16_t usb_holb_uc_stats_len;
	uint16_t usb_holb_uc_stats_num_periph_bad;
	uint16_t usb_holb_uc_stats_num_periph_recovered;
};

struct ipa_peripheral_msm_stats {
	uint32_t canary;

	uint16_t num_entries;
	uint16_t reserved;

	/* TLV for number of peripherals connected to APROC */
	/* value = IPA_PER_STATS_TYPE_NUM_PERS */
	uint16_t periph_id;
	uint16_t periph_len;
	uint32_t periph_val;

	/* TLV for number of periphers from/to traffic flowing from modem */
	/* value = IPA_PER_STATS_TYPE_NUM_PERS_WWAN */
	uint16_t periph_wwan_id;
	uint16_t periph_wwan_len;
	uint32_t periph_wwan_val;

	/* TLV for bitmask for active/connected peripherals */
	/* value = IPA_PER_STATS_TYPE_PER_TYPE */
	uint16_t periph_type_id;
	uint16_t periph_type_len;
	uint32_t periph_type_val;

	/* TLV for USB enumeration type */
	/* value = IPA_PER_STATS_TYPE_USB_TYPE */
	uint16_t usb_enum_id;
	uint16_t usb_enum_len;
	uint32_t usb_enum_value;

	/* TLV for Current USB protocol enumeration if active */
	/* value = IPA_PER_STATS_TYPE_USB_PROT */
	uint16_t usb_prot_enum_id;
	uint16_t usb_prot_enum_len;
	uint32_t usb_prot_enum_value;

	/* TLV for Max USB speed in current gen in Mbps */
	/* value = IPA_PER_STATS_TYPE_USB_MAX_SPEED */
	uint16_t usb_max_speed_id;
	uint16_t usb_max_speed_len;
	uint32_t usb_max_speed_val;

	/* TLV for Total number of USB plug in/outs */
	/* value = IPA_PER_STATS_TYPE_USB_PIPO */
	uint16_t usb_pipo_id;
	uint16_t usb_pipo_len;
	uint32_t usb_pipo_val;

	/* TLV for Wifi enumeration type*/
	/* value = IPA_PER_STATS_TYPE_WIFI_ENUM_TYPE */
	uint16_t wifi_enum_type_id;
	uint16_t wifi_enum_type_len;
	uint32_t wifi_enum_type_val;

	/* TLV for Theoritical Max WLAN speed in current gen in Mbps (pipe for 5GHz in case of dual band) */
	/* value = IPA_PER_STATS_TYPE_WIFI_MAX_SPEED */
	uint16_t wifi_max_speed_id;
	uint16_t wifi_max_speed_len;
	uint32_t wifi_max_speed_val;

	/* TLV for Theoretical Max WLAN speed on the 2.4GHz pipe, value of 0 means disabled */
	/* value = IPA_PER_STATS_TYPE_WIFI_DUAL_BAND_EN */
	uint16_t wifi_dual_band_enabled_id;
	uint16_t wifi_dual_band_enabled_len;
	uint32_t wifi_dual_band_enabled_val;

	/* TLV for number of wifi peripherals connected to APROC */
	/* value = IPA_PER_STATS_TYPE_WIFI_HOLB_UC */
	uint16_t wifi_holb_uc_stats_id;
	uint16_t wifi_holb_uc_stats_len;
	uint16_t wifi_holb_uc_stats_num_periph_bad;
	uint16_t wifi_holb_uc_stats_num_periph_recovered;

	/* TLV for number of usb peripherals connected to APROC */
	/* value = IPA_PER_STATS_TYPE_USB_HOLB_UC */
	uint16_t usb_holb_uc_stats_id;
	uint16_t usb_holb_uc_stats_len;
	uint16_t usb_holb_uc_stats_num_periph_bad;
	uint16_t usb_holb_uc_stats_num_periph_recovered;
};

union ipa_peripheral_stats {
	struct ipa_peripheral_mdm_stats mdm;
	struct ipa_peripheral_msm_stats msm;
};

int ipa3_peripheral_stats_init(union ipa_peripheral_stats *smem_addr);

#endif // _UAPI_IPA_LNX_STATS_H_
