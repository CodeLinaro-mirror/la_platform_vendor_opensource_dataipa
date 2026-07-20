/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2016-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#ifndef _IPAHAL_I_H_
#define _IPAHAL_I_H_

#include "ipa.h"
#include "ipa_common_i.h"

#define IPAHAL_DRV_NAME "ipahal"

#define IPAHAL_DBG(fmt, args...) \
	do { \
		pr_debug(IPAHAL_DRV_NAME " %s:%d " fmt, __func__, __LINE__, \
			## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf(), \
			IPAHAL_DRV_NAME " %s:%d " fmt, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
			IPAHAL_DRV_NAME " %s:%d " fmt, ## args); \
		ipa3_diag_log_write(IPA_DIAG_LVL_DBG, IPAHAL_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
	} while (0)

#define IPAHAL_DBG_LOW(fmt, args...) \
	do { \
		pr_debug(IPAHAL_DRV_NAME " %s:%d " fmt, __func__, __LINE__, \
			## args); \
		if (ipa3_get_ipc_logbuf_low()) { \
			IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
				IPAHAL_DRV_NAME " %s:%d " fmt, ## args); \
			ipa3_diag_log_write(IPA_DIAG_LVL_LOW, \
				IPAHAL_DRV_NAME " %s:%d " fmt, \
				__func__, __LINE__, ## args); \
		} \
	} while (0)

#define IPAHAL_ERR(fmt, args...) \
	do { \
		pr_err(IPAHAL_DRV_NAME " %s:%d " fmt, __func__, __LINE__, \
			## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf(), \
			IPAHAL_DRV_NAME " %s:%d " fmt, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
			IPAHAL_DRV_NAME " %s:%d " fmt, ## args); \
		ipa3_diag_log_write(IPA_DIAG_LVL_ERR, IPAHAL_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
	} while (0)

#define IPAHAL_ERR_RL(fmt, args...) \
		do { \
			pr_err_ratelimited_ipa(IPAHAL_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
			IPA_IPC_LOGGING(ipa3_get_ipc_logbuf(), \
				IPAHAL_DRV_NAME " %s:%d " fmt, ## args); \
			IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
				IPAHAL_DRV_NAME " %s:%d " fmt, ## args); \
			ipa3_diag_log_write(IPA_DIAG_LVL_ERR, IPAHAL_DRV_NAME " %s:%d " fmt, \
				__func__, __LINE__, ## args); \
		} while (0)

#define IPAHAL_DBG_REG(fmt, args...) \
	do { \
		pr_err(fmt, ## args); \
		IPA_IPC_LOGGING(ipahal_ctx->regdumpbuf, \
			" %s:%d " fmt, ## args); \
	} while (0)

#define IPAHAL_DBG_REG_IPC_ONLY(fmt, args...) \
		IPA_IPC_LOGGING(ipahal_ctx->regdumpbuf, " %s:%d " fmt, ## args)

#define IPAHAL_MEM_ALLOC(__size, __is_atomic_ctx) \
	(kzalloc((__size), ((__is_atomic_ctx) ? GFP_ATOMIC : GFP_KERNEL)))

#define IPAHAL_IPC_LOG_PAGES 50

#define IPAHAL_PKT_STATUS_FLTRT_RULE_MISS_ID 0x3ff

#define IPAHAL_PKT_STATUS_PACKET_TYPE_NON_DMA_BMSK 0x1
#define IPAHAL_PKT_STATUS_PACKET_TYPE_NON_DMA_SHFT 0
#define IPAHAL_PKT_STATUS_PACKET_TYPE_NON_DMA(x) (\
	(x & IPAHAL_PKT_STATUS_PACKET_TYPE_NON_DMA_BMSK) >> \
	IPAHAL_PKT_STATUS_PACKET_TYPE_NON_DMA_SHFT)

#define IPAHAL_PKT_STATUS_PACKET_TYPE_NO_ETH_BMSK  0x2
#define IPAHAL_PKT_STATUS_PACKET_TYPE_NO_ETH_SHFT  1
#define IPAHAL_PKT_STATUS_PACKET_TYPE_NO_ETH(x) (\
	(x & IPAHAL_PKT_STATUS_PACKET_TYPE_NO_ETH_BMSK) >> \
	IPAHAL_PKT_STATUS_PACKET_TYPE_NO_ETH_SHFT)

#define IPAHAL_PKT_STATUS_PACKET_TYPE_IP_TYPE_BMSK 0xC
#define IPAHAL_PKT_STATUS_PACKET_TYPE_IP_TYPE_SHFT 2
#define IPAHAL_PKT_STATUS_PACKET_TYPE_IP_TYPE(x) (\
	(x & IPAHAL_PKT_STATUS_PACKET_TYPE_IP_TYPE_BMSK) >> \
	IPAHAL_PKT_STATUS_PACKET_TYPE_IP_TYPE_SHFT)

/*
 * struct ipahal_context - HAL global context data
 * @hw_type: IPA H/W type/version.
 * @base: Base address to be used for accessing IPA memory. This is
 *  I/O memory mapped address.
 *  Controlled by debugfs. default is off
 * @dent: Debugfs folder dir entry
 * @ipa_pdev: IPA Platform Device. Will be used for DMA memory
 * @empty_fltrt_tbl: Empty table to be used at tables init.
 */
struct ipahal_context {
	enum ipa_hw_type hw_type;
	void __iomem *base;
    u32 ipa_cfg_offset;
	struct dentry *dent;
	struct device *ipa_pdev;
	struct ipa_mem_buffer empty_fltrt_tbl;
	void *regdumpbuf;
};

extern struct ipahal_context *ipahal_ctx;



/* Immediate commands H/W structures */

/*
 * struct ipa_imm_cmd_hw_ip_v4_filter_init - IP_V4_FILTER_INIT command payload
 *  in H/W format.
 * Inits IPv4 filter block.
 * @hash_rules_addr: Addr in system mem where ipv4 hashable flt rules starts
 * @hash_rules_size: Size in bytes of the hashable tbl to cpy to local mem
 * @hash_local_addr: Addr in shared mem where ipv4 hashable flt tbl should
 *  be copied to
 * @nhash_rules_size: Size in bytes of the non-hashable tbl to cpy to local mem
 * @nhash_local_addr: Addr in shared mem where ipv4 non-hashable flt tbl should
 *  be copied to
 * @rsvd: reserved
 * @nhash_rules_addr: Addr in sys mem where ipv4 non-hashable flt tbl starts
 */
struct ipa_imm_cmd_hw_ip_v4_filter_init {
	u64 hash_rules_addr:64;
	u64 hash_rules_size:12;
	u64 hash_local_addr:16;
	u64 nhash_rules_size:12;
	u64 nhash_local_addr:16;
	u64 rsvd:8;
	u64 nhash_rules_addr:64;
};

/*
 * struct ipa_imm_cmd_hw_ip_v6_filter_init - IP_V6_FILTER_INIT command payload
 *  in H/W format.
 * Inits IPv6 filter block.
 * @hash_rules_addr: Addr in system mem where ipv6 hashable flt rules starts
 * @hash_rules_size: Size in bytes of the hashable tbl to cpy to local mem
 * @hash_local_addr: Addr in shared mem where ipv6 hashable flt tbl should
 *  be copied to
 * @nhash_rules_size: Size in bytes of the non-hashable tbl to cpy to local mem
 * @nhash_local_addr: Addr in shared mem where ipv6 non-hashable flt tbl should
 *  be copied to
 * @rsvd: reserved
 * @nhash_rules_addr: Addr in sys mem where ipv6 non-hashable flt tbl starts
 */
struct ipa_imm_cmd_hw_ip_v6_filter_init {
	u64 hash_rules_addr:64;
	u64 hash_rules_size:12;
	u64 hash_local_addr:16;
	u64 nhash_rules_size:12;
	u64 nhash_local_addr:16;
	u64 rsvd:8;
	u64 nhash_rules_addr:64;
};

/*
 * struct ipa_imm_cmd_hw_ip_v4_nat_init - IP_V4_NAT_INIT command payload
 *  in H/W format.
 * Inits IPv4 NAT block. Initiate NAT table with it dimensions, location
 *  cache address and other related parameters.
 * @ipv4_rules_addr: Addr in sys/shared mem where ipv4 NAT rules start
 * @ipv4_expansion_rules_addr: Addr in sys/shared mem where expansion NAT
 *  table starts. IPv4 NAT rules that result in hash collision are located
 *  in this table.
 * @index_table_addr: Addr in sys/shared mem where index table, which points
 *  to NAT table starts
 * @index_table_expansion_addr: Addr in sys/shared mem where expansion index
 *  table starts
 * @table_index: For future support of multiple NAT tables
 * @rsvd1: reserved
 * @ipv4_rules_addr_type: ipv4_rules_addr in sys or shared mem
 * @ipv4_expansion_rules_addr_type: ipv4_expansion_rules_addr in
 *  sys or shared mem
 * @index_table_addr_type: index_table_addr in sys or shared mem
 * @index_table_expansion_addr_type: index_table_expansion_addr in
 *  sys or shared mem
 * @size_base_tables: Num of entries in NAT tbl and idx tbl (each)
 * @size_expansion_tables: Num of entries in NAT expansion tbl and expansion
 *  idx tbl (each)
 * @rsvd2: reserved
 * @public_addr_info: Public IP addresses info suitable to the IPA H/W version
 *                    IPA H/W >= 4.0 - PDN config table offset in SMEM
 *                    IPA H/W < 4.0  - The public IP address
 */
struct ipa_imm_cmd_hw_ip_v4_nat_init {
	u64 ipv4_rules_addr:64;
	u64 ipv4_expansion_rules_addr:64;
	u64 index_table_addr:64;
	u64 index_table_expansion_addr:64;
	u64 table_index:3;
	u64 rsvd1:1;
	u64 ipv4_rules_addr_type:1;
	u64 ipv4_expansion_rules_addr_type:1;
	u64 index_table_addr_type:1;
	u64 index_table_expansion_addr_type:1;
	u64 size_base_tables:12;
	u64 size_expansion_tables:10;
	u64 rsvd2:2;
	u64 public_addr_info:32;
};

/*
 * struct ipa_imm_cmd_hw_ip_v4_nat_init_v7_0 - IP_V4_NAT_INIT command payload in H/W format.
 *
 * @ipv4_rules_addr: Addr in sys/shared mem where ipv4 NAT rules start
 * @ipv4_expansion_rules_addr: Addr in sys/shared mem where expansion NAT
 *  table starts. IPv4 NAT rules that result in hash collision are located
 *  in this table.
 * @index_table_addr: Addr in sys/shared mem where index table, which points
 *  to NAT table starts
 * @index_table_expansion_addr: Addr in sys/shared mem where expansion index
 *  table starts
 * @table_index: For future support of multiple NAT tables
 * @reserved1: reserved
 * @ipv4_rules_addr_type: ipv4_rules_addr in sys or shared mem
 * @ipv4_expansion_rules_addr_type: ipv4_expansion_rules_addr in
 *  sys or shared mem
 * @index_table_addr_type: index_table_addr in sys or shared mem
 * @index_table_expansion_addr_type: index_table_expansion_addr in
 *  sys or shared mem
 * @size_base_tables: Num of entries in NAT tbl and idx tbl (each)
 * @reserved2: reserved
 * @public_addr_info: Public IP addresses info suitable to the IPA H/W version
 *                    IPA H/W >= 4.0 - PDN config table offset in SMEM
 *                    IPA H/W < 4.0  - The public IP address
 *
 * Init IPv4 NAT functionality. Initialize the NAT table with its dimensions, location, cache
 * address and other related parameters. Should be called upon IPA initialization. Table is
 * initialized as empty, NAT_DMA should be called to add new rules to the table and remove unused
 * rules from the table. Subsequent calls to IP_NAT_INIT with existing Table_Index will result in a
 * reset-like functionality, including resetting any cache.
 */
struct ipa_imm_cmd_hw_ip_v4_nat_init_v7_0 {
	u64 ipv4_rules_addr:64;
	u64 ipv4_expansion_rules_addr:64;
	u64 index_table_addr:64;
	u64 index_table_expansion_addr:64;
	u64 table_index:3;
	u64 reserved1:1;
	u64 ipv4_rules_addr_type:1;
	u64 ipv4_expansion_rules_addr_type:1;
	u64 index_table_addr_type:1;
	u64 index_table_expansion_addr_type:1;
	u64 size_base_tables:4;
	u64 reserved2:20;
	u64 pdn_config_base_addr:32;
};

/*
 * struct ipa_imm_cmd_hw_ip_v6_ct_init - IP_V6_CONN_TRACK_INIT command payload
 *  in H/W format.
 * Inits IPv6CT block. Initiate IPv6CT table with it dimensions, location
 *  cache address and other related parameters.
 * @table_addr: Address in sys/shared mem where IPv6CT rules start
 * @expansion_table_addr: Address in sys/shared mem where IPv6CT expansion
 *  table starts. IPv6CT rules that result in hash collision are located
 *  in this table.
 * @table_index: For future support of multiple IPv6CT tables
 * @rsvd1: reserved
 * @table_addr_type: table_addr in sys or shared mem
 * @expansion_table_addr_type: expansion_table_addr in sys or shared mem
 * @rsvd2: reserved
 * @size_base_tables: Number of entries in IPv6CT table
 * @size_expansion_tables: Number of entries in IPv6CT expansion table
 * @rsvd3: reserved
 */
struct ipa_imm_cmd_hw_ip_v6_ct_init {
	u64 table_addr:64;
	u64 expansion_table_addr:64;
	u64 table_index:3;
	u64 rsvd1:1;
	u64 table_addr_type:1;
	u64 expansion_table_addr_type:1;
	u64 rsvd2:2;
	u64 size_base_table:12;
	u64 size_expansion_table:10;
	u64 rsvd3:34;
};

/*
 * struct ipa_imm_cmd_hw_ip_v6_ct_init_v7_0 - IPAv7 IP_V6_CONN_TRACK_INIT command payload
 *  in H/W format.
 * Inits IPv6CT block. Initiate IPv6CT table with it dimensions, location
 *  cache address and other related parameters.
 * @table_addr: Address in sys/shared mem where IPv6CT rules start
 * @expansion_table_addr: Address in sys/shared mem where IPv6CT expansion
 *  table starts. IPv6CT rules that result in hash collision are located
 *  in this table.
 * @table_index: For future support of multiple IPv6CT tables
 * @rsvd1: reserved
 * @table_addr_type: table_addr in sys or shared mem
 * @expansion_table_addr_type: expansion_table_addr in sys or shared mem
 * @rsvd2: reserved
 * @size_base_tables: Number of entries in IPv6CT table
 * @rsvd3: reserved
 */
struct ipa_imm_cmd_hw_ip_v6_ct_init_v7_0 {
	u64 table_addr:64;
	u64 expansion_table_addr:64;
	u64 table_index:3;
	u64 reserved1:1;
	u64 table_addr_type:1;
	u64 expansion_table_addr_type:1;
	u64 reserved2:2;
	u64 size_base_table:4;
	u64 reserved3:52;
};

/*
 * struct ipa_imm_cmd_hw_ip_v4_routing_init - IP_V4_ROUTING_INIT command payload
 *  in H/W format.
 * Inits IPv4 routing table/structure - with the rules and other related params
 * @hash_rules_addr: Addr in system mem where ipv4 hashable rt rules starts
 * @hash_rules_size: Size in bytes of the hashable tbl to cpy to local mem
 * @hash_local_addr: Addr in shared mem where ipv4 hashable rt tbl should
 *  be copied to
 * @nhash_rules_size: Size in bytes of the non-hashable tbl to cpy to local mem
 * @nhash_local_addr: Addr in shared mem where ipv4 non-hashable rt tbl should
 *  be copied to
 * @rsvd: reserved
 * @nhash_rules_addr: Addr in sys mem where ipv4 non-hashable rt tbl starts
 */
struct ipa_imm_cmd_hw_ip_v4_routing_init {
	u64 hash_rules_addr:64;
	u64 hash_rules_size:12;
	u64 hash_local_addr:16;
	u64 nhash_rules_size:12;
	u64 nhash_local_addr:16;
	u64 rsvd:8;
	u64 nhash_rules_addr:64;
};

/*
 * struct ipa_imm_cmd_hw_ip_v6_routing_init - IP_V6_ROUTING_INIT command payload
 *  in H/W format.
 * Inits IPv6 routing table/structure - with the rules and other related params
 * @hash_rules_addr: Addr in system mem where ipv6 hashable rt rules starts
 * @hash_rules_size: Size in bytes of the hashable tbl to cpy to local mem
 * @hash_local_addr: Addr in shared mem where ipv6 hashable rt tbl should
 *  be copied to
 * @nhash_rules_size: Size in bytes of the non-hashable tbl to cpy to local mem
 * @nhash_local_addr: Addr in shared mem where ipv6 non-hashable rt tbl should
 *  be copied to
 * @rsvd: reserved
 * @nhash_rules_addr: Addr in sys mem where ipv6 non-hashable rt tbl starts
 */
struct ipa_imm_cmd_hw_ip_v6_routing_init {
	u64 hash_rules_addr:64;
	u64 hash_rules_size:12;
	u64 hash_local_addr:16;
	u64 nhash_rules_size:12;
	u64 nhash_local_addr:16;
	u64 rsvd:8;
	u64 nhash_rules_addr:64;
};

/*
 * struct ipa_imm_cmd_hw_hdr_init_local - HDR_INIT_LOCAL command payload
 *  in H/W format.
 * Inits hdr table within local mem with the hdrs and their length.
 * @hdr_table_addr: Word address in sys mem where the table starts (SRC)
 * @size_hdr_table: Size of the above (in bytes)
 * @hdr_addr: header address in IPA sram (used as DST for memory copy)
 * @rsvd: reserved
 */
struct ipa_imm_cmd_hw_hdr_init_local {
	u64 hdr_table_addr:64;
	u64 size_hdr_table:12;
	u64 hdr_addr:16;
	u64 rsvd:4;
};

/*
 * struct ipa_imm_cmd_hw_nat_dma - NAT_DMA command payload
 *  in H/W format
 * Perform DMA operation on NAT related mem addressess. Copy data into
 *  different locations within NAT associated tbls. (For add/remove NAT rules)
 * @table_index: NAT tbl index. Defines the NAT tbl on which to perform DMA op.
 * @rsvd1: reserved
 * @base_addr: Base addr to which the DMA operation should be performed.
 * @rsvd2: reserved
 * @offset: offset in bytes from base addr to write 'data' to
 * @data: data to be written
 * @rsvd3: reserved
 */
struct ipa_imm_cmd_hw_nat_dma {
	u64 table_index:3;
	u64 rsvd1:1;
	u64 base_addr:2;
	u64 rsvd2:2;
	u64 offset:32;
	u64 data:16;
	u64 rsvd3:8;
};

/*
 * struct ipa_imm_cmd_hw_table_dma_ipav4 - TABLE_DMA command payload
 *  in H/W format
 * Perform DMA operation on NAT and ipv6 connection tracking related mem
 * addresses. Copy data into different locations within NAT associated tbls
 * (For add/remove NAT rules)
 * @table_index: NAT tbl index. Defines the NAT tbl on which to perform DMA op.
 * @rsvd1: reserved
 * @base_addr: Base addr to which the DMA operation should be performed.
 * @rsvd2: reserved
 * @offset: offset in bytes from base addr to write 'data' to
 * @data: data to be written
 * @rsvd3: reserved
 */
struct ipa_imm_cmd_hw_table_dma_ipav4 {
	u64 table_index : 3;
	u64 rsvd1 : 1;
	u64 base_addr : 3;
	u64 rsvd2 : 1;
	u64 offset : 32;
	u64 data : 16;
	u64 rsvd3 : 8;
};

/*
 * struct ipa_imm_cmd_hw_table_dma_v7_0 - TABLE_WRITE command payload
 *  in H/W format
 * Perform DMA operation on NAT and ipv6 connection tracking related mem
 * addresses. Copy data into different locations within NAT associated tbls
 * (For add/remove NAT rules)
 * @table_index: NAT tbl index. Defines the NAT tbl on which to perform DMA op.
 * @rsvd1: reserved
 * @table_Select: Type of table, from which the base address of the table can be inferred.
 * @rsvd2: reserved
 * @offset_within_entry: offset within the entry of the table
 * @entry_index: index of the entry in the table
 * @data: data to be written.
 * @cache_entry_evict: indicates whether data will be evicted to DDR
 * @no_write: indicates whether entry will be written to DDR
 * @rsvd3: reserved
 * @cache_entry_hash_value: HW uses this value in case of eviction
 * @write_bitmask: indicates which bits (out of 16 data bits) to write
 * @rsvd4: reserved
 */
struct ipa_imm_cmd_hw_table_dma_v7_0 {
	u64 table_index : 3;
	u64 reserved1 : 1;
	u64 table_Select : 2;
	u64 reserved2 : 2;
	u64 offset_within_entry : 8;
	u64 entry_index : 16;
	u64 data : 16;
	u64 cache_entry_evict : 1;
	u64 no_write : 1;
	u64 reserved3 : 14;
	u64 cache_entry_hash_value : 16;
	u64 write_bitmask : 16;
	u64 reserved4 : 32;
};

/*
 * struct ipa_imm_cmd_hw_hdr_init_system - HDR_INIT_SYSTEM command payload
 *  in H/W format.
 * Inits hdr table within sys mem with the hdrs and their length.
 * @hdr_table_addr: Word address in system memory where the hdrs tbl starts.
 */
struct ipa_imm_cmd_hw_hdr_init_system {
	u64 hdr_table_addr:64;
};

/*
 * struct ipa_imm_cmd_hw_ip_packet_init - IP_PACKET_INIT command payload
 *  in H/W format.
 * Configuration for specific IP pkt. Shall be called prior to an IP pkt
 *  data. Pkt will not go through IP pkt processing.
 * @destination_pipe_index: Destination pipe index  (in case routing
 *  is enabled, this field will overwrite the rt  rule)
 * @rsvd: reserved
 */
struct ipa_imm_cmd_hw_ip_packet_init {
	u64 destination_pipe_index:5;
	u64 rsv1:59;
};


/*
 * struct ipa_imm_cmd_hw_ip_packet_init_v_5_0 - IP_PACKET_INIT command payload
 *  in H/W format for IPA v5_0.
 * Configuration for specific IP pkt. Shall be called prior to an IP pkt
 *  data. Pkt will not go through IP pkt processing.
 * @destination_pipe_index: Destination pipe index  (in case routing
 *  is enabled, this field will overwrite the rt  rule)
 * @rsvd: reserved
 */
struct ipa_imm_cmd_hw_ip_packet_init_v_5_0 {
    u64 destination_pipe_index : 8;
    u64 rsv1 : 56;
};

/*
 * struct ipa_imm_cmd_hw_ip_packet_init_ex - IP_PACKET_INIT_EX command payload
 *  in H/W format for IPA v5_0.
 * @frag_disable: 1 - disabled. overrides IPA_ENDP_CONFIG_n:FRAG_OFFLOAD_EN
 * @filter_disable: 1 - disabled, 0 enabled
 * @nat_disable: 1 - disabled, 0 enabled
 * @route_disable: 1 - disabled, 0 enabled
 * @hdr_removal_insertion_disable: 1 - disabled, 0 enabled
 * @cs_disable: 1 - disabled, 0 enabled
 * @quota_tethering_stats_disable: 1 - disabled, 0 enabled
 * fields @flt_rt_tbl_idx - @rsvd4 are a copy of ipa5_0_flt_rule_hw_hdr
 * fields @rt_pipe_dest_idx - @rt_system are a copy of ipa5_0_rt_rule_hw_hdr
 */
struct ipa_imm_cmd_hw_ip_packet_init_ex {
    u64 rsvd1 : 16;
    u64 frag_disable : 1;
    u64 filter_disable : 1;
    u64 nat_disable : 1;
    u64 route_disable : 1;
    u64 hdr_removal_insertion_disable : 1;
    u64 cs_disable : 1;
    u64 quota_tethering_stats_disable : 1;
    u64 rsvd2 : 9;
	u64 flt_rt_tbl_idx : 8;
	u64 flt_stats_cnt_idx : 8;
	u64 flt_priority : 8;
	u64 rsvd3 : 1;
	u64 flt_close_aggr_irq_mod : 1;
	u64 flt_rule_id : 10;
	u64 flt_action : 5;
	u64 flt_pdn_idx : 4;
	u64 flt_set_metadata : 1;
	u64 flt_retain_hdr : 1;
	u64 rsvd4 : 1;
	u64 rt_pipe_dest_idx : 8;
	u64 rt_stats_cnt_idx : 8;
	u64 rt_priority : 8;
	u64 rt_rsvd : 1;
	u64 rt_close_aggr_irq_mod : 1;
	u64 rt_rule_id : 10;
	u64 rt_hdr_offset : 9;
	u64 rt_proc_ctx : 1;
	u64 rt_retain_hdr : 1;
	u64 rt_system : 1;
} __packed;

/*
 * struct ipa_imm_cmd_hw_ip_packet_init_ex_v5_5 - IP_PACKET_INIT_EX command payload
 *  in H/W format for IPA v5_5.
 * @frag_disable: 1 - disabled. overrides IPA_ENDP_CONFIG_n:FRAG_OFFLOAD_EN
 * @filter_disable: 1 - disabled, 0 enabled
 * @nat_disable: 1 - disabled, 0 enabled
 * @route_disable: 1 - disabled, 0 enabled
 * @hdr_removal_insertion_disable: 1 - disabled, 0 enabled
 * @cs_disable: 1 - disabled, 0 enabled
 * @quota_tethering_stats_disable: 1 - disabled, 0 enabled
 * @dpl_disable: 1 - disabled, 0 enabled
 * fields @flt_rt_tbl_idx - @rsvd4 are a copy of ipa5_5_flt_rule_hw_hdr
 * fields @rt_pipe_dest_idx - @rsvd5 are a copy of ipa5_5_rt_rule_hw_hdr
 */
struct ipa_imm_cmd_hw_ip_packet_init_ex_v5_5 {
	u64 rsvd1 : 16;
	u64 frag_disable : 1;
	u64 filter_disable : 1;
	u64 nat_disable : 1;
	u64 route_disable : 1;
	u64 hdr_removal_insertion_disable : 1;
	u64 cs_disable : 1;
	u64 quota_tethering_stats_disable : 1;
	u64 dpl_disable : 1;
	u64 rsvd2 : 40;
	u64 flt_rt_tbl_idx : 8;
	u64 flt_stats_cnt_idx : 8;
	u64 flt_priority : 8;
	u64 flt_ext_hdr : 1;
	u64 flt_close_aggr_irq_mod : 1;
	u64 flt_rule_id : 10;
	u64 flt_action : 5;
	u64 flt_pdn_idx : 4;
	u64 flt_set_metadata : 1;
	u64 flt_retain_hdr : 1;
	u64 rsvd3 : 1;
	u64 flt_ttl : 1;
	u64 flt_qos_class : 6;
	u64 rsvd4 : 9;
	u64 rt_pipe_dest_idx : 8;
	u64 rt_stats_cnt_idx : 8;
	u64 rt_priority : 8;
	u64 rt_ext_hdr : 1;
	u64 rt_close_aggr_irq_mod : 1;
	u64 rt_rule_id : 10;
	u64 rt_hdr_offset : 9;
	u64 rt_proc_ctx : 1;
	u64 rt_retain_hdr : 1;
	u64 rt_system : 1;
	u64 rt_ttl : 1;
	u64 rt_qos_class : 6;
	u64 rt_skip_ingress : 1;
	u64 rsvd5 : 8;
} __packed;

/*
 * struct ipa_imm_cmd_hw_ip_packet_init_ex_v6_0 - IP_PACKET_INIT_EX command payload
 *  in H/W format for IPA v6_0.
 * @frag_disable: 1 - disabled. overrides IPA_ENDP_CONFIG_n:FRAG_OFFLOAD_EN
 * @filter_disable: 1 - disabled, 0 enabled
 * @nat_disable: 1 - disabled, 0 enabled
 * @route_disable: 1 - disabled, 0 enabled
 * @hdr_removal_insertion_disable: 1 - disabled, 0 enabled
 * @cs_disable: 1 - disabled, 0 enabled
 * @quota_tethering_stats_disable: 1 - disabled, 0 enabled
 * @dpl_disable: 1 - disabled, 0 enabled
 * fields @flt_rt_tbl_idx - @rsvd5 are a copy of ipa5_5_flt_rule_hw_hdr and ipa6_0_flt_rule_hw_hdr_ext
 * fields @rt_pipe_dest_idx - @rsvd6 are a copy of ipa5_5_rt_rule_hw_hdr and ipa6_0_rt_rule_hw_hdr_ext
 */
struct ipa_imm_cmd_hw_ip_packet_init_ex_v6_0 {
	u64 rsvd1 : 16;
	u64 frag_disable : 1;
	u64 filter_disable : 1;
	u64 nat_disable : 1;
	u64 route_disable : 1;
	u64 hdr_removal_insertion_disable : 1;
	u64 cs_disable : 1;
	u64 quota_tethering_stats_disable : 1;
	u64 dpl_disable : 1;
	u64 rsvd2 : 40;
	u64 flt_rt_tbl_idx : 8;
	u64 flt_stats_cnt_idx : 8;
	u64 flt_priority : 8;
	u64 flt_ext_hdr : 1;
	u64 flt_close_aggr_irq_mod : 1;
	u64 flt_rule_id : 10;
	u64 flt_action : 5;
	u64 flt_pdn_idx : 4;
	u64 flt_set_metadata : 1;
	u64 flt_retain_hdr : 1;
	u64 rsvd3 : 1;
	u64 flt_ttl : 1;
	u64 flt_qos_class : 6;
	u64 rsvd4 : 1;
	u64 flt_esp_after_udp : 1;
	u64 rsvd5 : 7;
	u64 rt_pipe_dest_idx : 8;
	u64 rt_stats_cnt_idx : 8;
	u64 rt_priority : 8;
	u64 rt_ext_hdr : 1;
	u64 rt_close_aggr_irq_mod : 1;
	u64 rt_rule_id : 10;
	u64 rt_hdr_offset : 9;
	u64 rt_proc_ctx : 1;
	u64 rt_retain_hdr : 1;
	u64 rt_system : 1;
	u64 rt_ttl : 1;
	u64 rt_qos_class : 6;
	u64 rt_skip_ingress : 1;
	u64 rt_esp_after_udp : 1;
	u64 rsvd6 : 7;
} __packed;

/*
 * struct ipa_imm_cmd_hw_ip_packet_init_ex_v7_0 - IP_PACKET_INIT_EX command payload
 *  in H/W format for IPA v7.0.
 * @sw_reserved: Reserved for software use
 * @conn_track_nat_stats_ip_type: IP type for connection tracking/NAT statistics
 * @conn_track_nat_stats_direction: Direction for connection tracking/NAT statistics
 * @traffic_mode: Traffic mode configuration
 * @frag_disable: 1 - disabled. overrides IPA_ENDP_CONFIG_n:FRAG_OFFLOAD_EN
 * @filter_disable: 1 - disabled, 0 enabled
 * @nat_disable: 1 - disabled, 0 enabled
 * @route_disable: 1 - disabled, 0 enabled
 * @hdr_removal_insertion_disable: 1 - disabled, 0 enabled
 * @cs_disable: 1 - disabled, 0 enabled
 * @quota_tethering_stats_disable: 1 - disabled, 0 enabled
 * @dpl_disable: 1 - disabled, 0 enabled
 * @leading_header_size: Size of leading header in bytes
 * @conn_track_nat_stats_counter_idx: Counter index for connection tracking/NAT statistics
 * @rt_hdr_offset: Route header offset
 * Fields @rt_pipe_dest_idx - @rt_hpc_fetch_len are routing rule parameters
 * Fields @flt_rt_tbl_idx - @flt_set_metadata are filter rule parameters
 * @sw_classification_cookie: Software classification cookie value
 */
struct __attribute__((packed)) ipa_imm_cmd_hw_ip_packet_init_ex_v7_0 {
	u64 sw_reserved                      :12;
	u64 conn_track_nat_stats_ip_type     :1;
	u64 conn_track_nat_stats_direction   :1;
	u64 traffic_mode                     :2;
	u64 frag_disable                     :1;
	u64 filter_disable                   :1;
	u64 nat_disable                      :1;
	u64 route_disable                    :1;
	u64 hdr_removal_insertion_disable    :1;
	u64 cs_disable                       :1;
	u64 quota_tethering_stats_disable    :1;
	u64 dpl_disable                      :1;
	u64 leading_header_size              :8;
	u64 conn_track_nat_stats_counter_idx :16;
	u64 rt_hdr_offset                    :16;

	u64 rt_pipe_dest_idx                 :8;
	u64 rt_skip_ingress	             :1;
	u64 rt_close_aggr_irq_mod            :1;
	u64 rt_ttl                           :1;
	u64 rt_esp_after_udp                 :1;
	u64 rt_rule_type                     :3;
	u64 rt_retain_hdr                    :1;
	u64 rt_priority                      :8;
	u64 rt_qos_class                     :8;
	u64 rt_stats_cnt_idx                 :12;
	u64 rt_rule_id                       :10;
	u64 rt_system                        :1;
	u64 rt_proc_ctx                      :1;
	u64 rt_hpc_fetch_len                 :8;

	u64 flt_rt_tbl_idx                   :8;
	u64 flt_reserved                     :1;
	u64 flt_close_aggr_irq_mod           :1;
	u64 flt_ttl                          :1;
	u64 flt_esp_after_udp                :1;
	u64 flt_rule_type                    :3;
	u64 flt_retain_hdr                   :1;
	u64 flt_priority                     :8;
	u64 flt_qos_class                    :8;
	u64 flt_stats_cnt_idx                :12;
	u64 flt_rule_id                      :10;
	u64 flt_action                       :5;
	u64 flt_pdn_idx                      :4;
	u64 flt_set_metadata                 :1;

	u64 sw_classification_cookie         :64;
};

/*
 * struct ipa_imm_cmd_hw_register_write - REGISTER_WRITE command payload
 *  in H/W format.
 * Write value to register. Allows reg changes to be synced with data packet
 *  and other immediate command. Can be used to access the sram
 * @sw_rsvd: Ignored by H/W. May be used by S/W
 * @skip_pipeline_clear: 0 to wait until IPA pipeline is clear. 1 don't wait
 * @offset: offset from IPA base address - Lower 16bit of the IPA reg addr
 * @value: value to write to register
 * @value_mask: mask specifying which value bits to write to the register
 * @pipeline_clear_options: options for pipeline to clear
 *	0: HPS - no pkt inside HPS (not grp specific)
 *	1: source group - The immediate cmd src grp does not use any pkt ctxs
 *	2: Wait until no pkt reside inside IPA pipeline
 *	3: reserved
 * @rsvd: reserved - should be set to zero
 */
struct ipa_imm_cmd_hw_register_write {
	u64 sw_rsvd:15;
	u64 skip_pipeline_clear:1;
	u64 offset:16;
	u64 value:32;
	u64 value_mask:32;
	u64 pipeline_clear_options:2;
	u64 rsvd:30;
};

/*
 * struct ipa_imm_cmd_hw_register_write_v_4_0 - REGISTER_WRITE command payload
 *  in H/W format.
 * Write value to register. Allows reg changes to be synced with data packet
 *  and other immediate command. Can be used to access the sram
 * @sw_rsvd: Ignored by H/W. May be used by S/W
 * @offset_high: high bits of the Offset field - bits 17-20
 * @rsvd: reserved - should be set to zero
 * @offset: offset from IPA base address - Lower 16bit of the IPA reg addr
 * @value: value to write to register
 * @value_mask: mask specifying which value bits to write to the register
 * @rsvd2: reserved - should be set to zero
 */
struct ipa_imm_cmd_hw_register_write_v_4_0 {
	u64 sw_rsvd:11;
	u64 offset_high:4;
	u64 rsvd:1;
	u64 offset:16;
	u64 value:32;
	u64 value_mask:32;
	u64 rsvd2:32;
};

/*
 * struct ipa_imm_cmd_hw_register_read - REGISTER_READ command payload
 *  in H/W format.
 * Read value from register. Allows reg changes to be synced with data packet
 *  and other immediate command. Can be used to access the sram
 * @sw_rsvd: Ignored by H/W. May be used by S/W
 * @offset_high: high bits of the Offset field - bits 17-20
 * @rsvd: reserved - should be set to zero
 * @offset: offset from IPA base address - Lower 16bit of the IPA reg addr
 * @sys_addr: Address in system memory for storing register value
 */
struct ipa_imm_cmd_hw_register_read {
	u64 sw_rsvd:11;
	u64 offset_high:4;
	u64 rsvd:1;
	u64 offset:16;
	u64 sys_addr:32;
};

/*
 * struct ipa_imm_cmd_hw_dma_shared_mem - DMA_SHARED_MEM command payload
 *  in H/W format.
 * Perform mem copy into or out of the SW area of IPA local mem
 * @sw_rsvd: Ignored by H/W. My be used by S/W
 * @size: Size in bytes of data to copy. Expected size is up to 2K bytes
 * @local_addr: Address in IPA local memory
 * @direction: Read or write?
 *	0: IPA write, Write to local address from system address
 *	1: IPA read, Read from local address to system address
 * @skip_pipeline_clear: 0 to wait until IPA pipeline is clear. 1 don't wait
 * @pipeline_clear_options: options for pipeline to clear
 *	0: HPS - no pkt inside HPS (not grp specific)
 *	1: source group - The immediate cmd src grp does npt use any pkt ctxs
 *	2: Wait until no pkt reside inside IPA pipeline
 *	3: reserved
 * @rsvd: reserved - should be set to zero
 * @system_addr: Address in system memory
 */
struct ipa_imm_cmd_hw_dma_shared_mem {
	u64 sw_rsvd:16;
	u64 size:16;
	u64 local_addr:16;
	u64 direction:1;
	u64 skip_pipeline_clear:1;
	u64 pipeline_clear_options:2;
	u64 rsvd:12;
	u64 system_addr:64;
};

/*
 * struct ipa_imm_cmd_hw_dma_shared_mem - DMA_SHARED_MEM command payload
 *  in H/W format.
 * Perform mem copy into or out of the SW area of IPA local mem
 * @sw_rsvd: Ignored by H/W. My be used by S/W
 * @size: Size in bytes of data to copy. Expected size is up to 2K bytes
 * @clear_after_read: Clear local memory at the end of a read operation allows
 *  atomic read and clear if HPS is clear. Ignore for writes.
 * @local_addr: Address in IPA local memory
 * @direction: Read or write?
 *	0: IPA write, Write to local address from system address
 *	1: IPA read, Read from local address to system address
 * @rsvd: reserved - should be set to zero
 * @system_addr: Address in system memory
 */
struct ipa_imm_cmd_hw_dma_shared_mem_v_4_0 {
	u64 sw_rsvd:15;
	u64 clear_after_read:1;
	u64 size:16;
	u64 local_addr:16;
	u64 direction:1;
	u64 rsvd:15;
	u64 system_addr:64;
};

/*
 * struct ipa_imm_cmd_hw_dma_shared_mem - DMA_SHARED_MEM command payload
 *  in H/W format.
 * Perform mem copy into or out of the SW area of IPA local mem
 * @sw_rsvd: Ignored by H/W. My be used by S/W
 * @size: Size in bytes of data to copy. Expected size is up to 2K bytes
 * @clear_after_read: Clear local memory at the end of a read operation allows
 *  atomic read and clear if HPS is clear. Ignore for writes.
 * @local_addr: Address in IPA local memory
 * @direction: Read or write?
 *	0: IPA write, Write to local address from system address
 *	1: IPA read, Read from local address to system address
 * @rsvd: reserved - should be set to zero
 * @system_addr: Address in system memory
 */
struct ipa_imm_cmd_hw_dma_shared_mem_v_6_0 {
	u64 sw_rsvd:15;
	u64 clear_after_read:1;
	u64 size:16;
	u64 local_addr:20;
	u64 direction:1;
	u64 rsvd:11;
	u64 system_addr:64;
};

/*
 * struct ipa_imm_cmd_hw_ip_packet_tag_status -
 *  IP_PACKET_TAG_STATUS command payload in H/W format.
 * This cmd is used for to allow SW to track HW processing by setting a TAG
 *  value that is passed back to SW inside Packet Status information.
 *  TAG info will be provided as part of Packet Status info generated for
 *  the next pkt transferred over the pipe.
 *  This immediate command must be followed by a packet in the same transfer.
 * @sw_rsvd: Ignored by H/W. My be used by S/W
 * @tag: Tag that is provided back to SW
 */
struct ipa_imm_cmd_hw_ip_packet_tag_status {
	u64 sw_rsvd:16;
	u64 tag:48;
};

/*
 * struct ipa_imm_cmd_hw_dma_task_32b_addr -
 *	IPA_DMA_TASK_32B_ADDR command payload in H/W format.
 * Used by clients using 32bit addresses. Used to perform DMA operation on
 *  multiple descriptors.
 *  The Opcode is dynamic, where it holds the number of buffer to process
 * @sw_rsvd: Ignored by H/W. My be used by S/W
 * @cmplt: Complete flag: When asserted IPA will interrupt SW when the entire
 *  DMA related data was completely xfered to its destination.
 * @eof: Enf Of Frame flag: When asserted IPA will assert the EOT to the
 *  dest client. This is used used for aggr sequence
 * @flsh: Flush flag: When asserted, pkt will go through the IPA blocks but
 *  will not be xfered to dest client but rather will be discarded
 * @lock: Lock pipe flag: When asserted, IPA will stop processing descriptors
 *  from other EPs in the same src grp (RX queue)
 * @unlock: Unlock pipe flag: When asserted, IPA will stop exclusively
 *  servicing current EP out of the src EPs of the grp (RX queue)
 * @size1: Size of buffer1 data
 * @addr1: Pointer to buffer1 data
 * @packet_size: Total packet size. If a pkt send using multiple DMA_TASKs,
 *  only the first one needs to have this field set. It will be ignored
 *  in subsequent DMA_TASKs until the packet ends (EOT). First DMA_TASK
 *  must contain this field (2 or more buffers) or EOT.
 */
struct ipa_imm_cmd_hw_dma_task_32b_addr {
	u64 sw_rsvd:11;
	u64 cmplt:1;
	u64 eof:1;
	u64 flsh:1;
	u64 lock:1;
	u64 unlock:1;
	u64 size1:16;
	u64 addr1:32;
	u64 packet_size:16;
};

/**
 * struct ipa_imm_cmd_hw_shaping_control - SHAPING_CONTROL
 * command payload in H/W format.
 * @traffic_class_bitmap1:
 * @traffic_class_bitmap2:
 * @shaped_prod_token_bucket_reinit_bitmap:
 * @traffic_class_operation:
 */
struct ipa_imm_cmd_hw_shaping_control {
	u64 traffic_class_bitmap1:64;
	u64 traffic_class_bitmap2:64;
	u64 shaped_prod_token_bucket_reinit_bitmap:16;
	u64 reserved:44;
	u64 traffic_class_operation:4;
};

/**
 * struct ipa_imm_cmd_hw_modem_bearer_init - MODEM_BEARER_INIT
 * command payload in H/W format.
 * @pdn_id:
 * @context_idx:
 * @ipa_maci_size:
 * @ip_algorithm:
 * @direction:
 * @sdap_enabled:
 * @ip_key_idx:
 * @pdcp_headers_size:
 * @bearer:
 * @cipher_key_idx:
 * @cipher_algorithm:
 * @bearer_select:
 * @rqi_monitor:
 * @rdi_monitor:
 * @integrity_protection_processing_type:
 * @type:
 * @sdap_offset:
 * @cipher_offset:
 * @cipher_keystrm_offset:
 * @metadata:
 */
struct ipa_imm_cmd_hw_modem_bearer_init {
	u64 pdn_id:8;
	u64 context_idx:8;
	u64 ipa_maci_size:2;
	u64 ip_algorithm:4;
	u64 direction:1;
	u64 sdap_enabled:1;
	u64 ip_key_idx:8;
	u64 pdcp_headers_size:8;
	u64 bearer:8;
	u64 cipher_key_idx:8;
	u64 cipher_algorithm:4;
	u64 bearer_select:1;
	u64 rqi_monitor:1;
	u64 rdi_monitor:1;
	u64 integrity_protection_processing_type:1;

	u64 type:2;
	u64 sdap_offset:6;
	u64 cipher_offset:8;
	u64 cipher_keystrm_offset:8;
	u64 reserved:8;
	u64 metadata:32;
};

/**
 * struct ipa_imm_cmd_hw_modem_bearer_config -
 * MODEM_BEARER_CONFIG command payload in H/W format.
 * @reserved_for_sw:
 * @context_idx:
 * @size:
 * @count:
 */
struct ipa_imm_cmd_hw_modem_bearer_config {
	u64 reserved_for_sw:8;
	u64 context_idx:8;
	u64 size:16;
	u64 count:32;
};

/**
 * struct ipa_imm_cmd_hw_modem_keys_set - MODEM_KEYS_SET
 * command payload in H/W format.
 * @count:
 * @context_idx:
 */
struct ipa_imm_cmd_hw_modem_keys_set {
	u64 reserved_for_sw:16;
	u64 size_keys:16;
	u64 start_offset:16;
	u64 reserved:16;

	u64 keys_addr;
};

/* IPA Status packet H/W structures and info */

/*
 * struct ipa_status_pkt_hw - IPA status packet payload in H/W format.
 *  This structure describes the status packet H/W structure for the
 *   following statuses: IPA_STATUS_PACKET, IPA_STATUS_DROPPED_PACKET,
 *   IPA_STATUS_SUSPENDED_PACKET.
 *  Other statuses types has different status packet structure.
 * @status_opcode: The Type of the status (Opcode).
 * @exception: (not bitmask) - the first exception that took place.
 *  In case of exception, src endp and pkt len are always valid.
 * @status_mask: Bit mask specifying on which H/W blocks the pkt was processed.
 * @pkt_len: Pkt pyld len including hdr, include retained hdr if used. Does
 *  not include padding or checksum trailer len.
 * @endp_src_idx: Source end point index.
 * @rsvd1: reserved
 * @endp_dest_idx: Destination end point index.
 *  Not valid in case of exception
 * @rsvd2: reserved
 * @metadata: meta data value used by packet
 * @flt_local: Filter table location flag: Does matching flt rule belongs to
 *  flt tbl that resides in lcl memory? (if not, then system mem)
 * @flt_hash: Filter hash hit flag: Does matching flt rule was in hash tbl?
 * @flt_global: Global filter rule flag: Does matching flt rule belongs to
 *  the global flt tbl? (if not, then the per endp tables)
 * @flt_ret_hdr: Retain header in filter rule flag: Does matching flt rule
 *  specifies to retain header?
 *  Starting IPA4.5, this will be true only if packet has L2 header.
 * @flt_rule_id: The ID of the matching filter rule. This info can be combined
 *  with endp_src_idx to locate the exact rule. ID=0x3FF reserved to specify
 *  flt miss. In case of miss, all flt info to be ignored
 * @rt_local: Route table location flag: Does matching rt rule belongs to
 *  rt tbl that resides in lcl memory? (if not, then system mem)
 * @rt_hash: Route hash hit flag: Does matching rt rule was in hash tbl?
 * @ucp: UC Processing flag.
 * @rt_tbl_idx: Index of rt tbl that contains the rule on which was a match
 * @rt_rule_id: The ID of the matching rt rule. This info can be combined
 *  with rt_tbl_idx to locate the exact rule. ID=0x3FF reserved to specify
 *  rt miss. In case of miss, all rt info to be ignored
 * @nat_hit: NAT hit flag: Was their NAT hit?
 * @nat_entry_idx: Index of the NAT entry used of NAT processing
 * @nat_type: Defines the type of the NAT operation:
 *	00: No NAT
 *	01: Source NAT
 *	10: Destination NAT
 *	11: Reserved
 * @tag_info: S/W defined value provided via immediate command
 * @seq_num: Per source endp unique packet sequence number
 * @time_of_day_ctr: running counter from IPA clock
 * @hdr_local: Header table location flag: In header insertion, was the header
 *  taken from the table resides in local memory? (If no, then system mem)
 * @hdr_offset: Offset of used header in the header table
 * @frag_hit: Frag hit flag: Was their frag rule hit in H/W frag table?
 * @frag_rule: Frag rule index in H/W frag table in case of frag hit
 * @hw_specific: H/W specific reserved value
 */
struct ipa_gen_pkt_status_hw {
	u64 status_opcode:8;
	u64 exception:8;
	u64 status_mask:16;
	u64 pkt_len:16;
	u64 endp_src_idx:5;
	u64 rsvd1:3;
	u64 endp_dest_idx:5;
	u64 rsvd2:3;
	u64 metadata:32;
	u64 flt_local:1;
	u64 flt_hash:1;
	u64 flt_global:1;
	u64 flt_ret_hdr:1;
	u64 flt_rule_id:10;
	u64 rt_local:1;
	u64 rt_hash:1;
	u64 ucp:1;
	u64 rt_tbl_idx:5;
	u64 rt_rule_id:10;
	u64 nat_hit:1;
	u64 nat_entry_idx:13;
	u64 nat_type:2;
	u64 tag_info:48;
	u64 seq_num:8;
	u64 time_of_day_ctr:24;
	u64 hdr_local:1;
	u64 hdr_offset:10;
	u64 frag_hit:1;
	u64 frag_rule:4;
	u64 hw_specific:16;
} __packed;

/*
 * struct ipa_frag_pkt_status_hw - IPA status packet payload in H/W format.
 *  This structure describes the frag status packet H/W structure for the
 *   following statuses: IPA_NEW_FRAG_RULE.
 * @status_opcode: The Type of the status (Opcode).
 * @frag_rule_idx: Frag rule index value.
 * @rsvd1: reserved
 * @tbl_idx: Table index valid or not.
 * @endp_src_idx: Source end point index.
 * @exception: (not bitmask) - the first exception that took place.
 *  In case of exception, src endp and pkt len are always valid.
 * @rsvd2: reserved
 * @seq_num: Packet sequence number.
 * @src_ip_addr: Source packet IP address.
 * @dest_ip_addr: Destination packet IP address.
 * @rsvd3: reserved
 * @nat_type: Defines the type of the NAT operation:
 *	00: No NAT
 *	01: Source NAT
 *	10: Destination NAT
 *	11: Reserved
 * @protocol: Protocal number.
 * @ip_id: IP packet IP ID number.
 * @tlated_ip_addr: IP address.
 * @hdr_local: Header table location flag: In header insertion, was the header
 *  taken from the table resides in local memory? (If no, then system mem)
 * @hdr_offset: Offset of used header in the header table
 * @endp_dest_idx: Destination end point index.
 * @ip_cksum_diff: IP packet checksum difference.
 * @metadata: meta data value used by packet
 * @rsvd4: reserved
 */
struct ipa_frag_pkt_status_hw {
	u64 status_opcode:8;
	u64 frag_rule_idx:4;
	u64 reserved_1:3;
	u64 tbl_idx:1;
	u64 endp_src_idx:5;
	u64 exception:1;
	u64 reserved_2:2;
	u64 seq_num:8;
	u64 src_ip_addr:32;
	u64 dest_ip_addr:32;
	u64 reserved_3:6;
	u64 nat_type:2;
	u64 protocol:8;
	u64 ip_id:16;
	u64 tlated_ip_addr:32;
	u64 hdr_local:1;
	u64 hdr_offset:10;
	u64 endp_dest_idx:5;
	u64 ip_cksum_diff:16;
	u64 metadata:32;
	u64 reserved_4:32;
} __packed;

/*
 * struct ipa_status_pkt_hw_v5_0 - IPA v5.0 status packet payload in H/W format.
 *  This structure describes the status packet H/W structure for the
 *   following statuses: IPA_STATUS_PACKET, IPA_STATUS_DROPPED_PACKET,
 *   IPA_STATUS_SUSPENDED_PACKET.
 *  Other statuses types has different status packet structure.
 * @status_opcode: The Type of the status (Opcode).
 * @exception: (not bitmask) - the first exception that took place.
 *  In case of exception, src endp and pkt len are always valid.
 * @status_mask: Bit mask specifying on which H/W blocks the pkt was processed.
 * @pkt_len: Pkt pyld len including hdr, include retained hdr if used. Does
 *  not include padding or checksum trailer len.
 * @endp_src_idx: Source end point index.
 * @reserved_1: reserved
 * @rt_local: Route table location flag: Does matching rt rule belongs to
 *  rt tbl that resides in lcl memory? (if not, then system mem)
 * @rt_hash: Route hash hit flag: Does matching rt rule was in hash tbl?
 *  Not valid in case of exception
 * @reserved_2: reserved
 * @metadata: meta data value used by packet
 * @flt_local: Filter table location flag: Does matching flt rule belongs to
 *  flt tbl that resides in lcl memory? (if not, then system mem)
 * @flt_hash: Filter hash hit flag: Does matching flt rule was in hash tbl?
 * @flt_global: Global filter rule flag: Does matching flt rule belongs to
 *  the global flt tbl? (if not, then the per endp tables)
 * @flt_ret_hdr: Retain header in filter rule flag: Does matching flt rule
 *  specifies to retain header?
 *  Starting IPA4.5, this will be true only if packet has L2 header.
 * @flt_rule_id: The ID of the matching filter rule. This info can be combined
 *  with endp_src_idx to locate the exact rule. ID=0x3FF reserved to specify
 *  flt miss. In case of miss, all flt info to be ignored
 * @rt_tbl_idx: Index of rt tbl that contains the rule on which was a match
 * @rt_rule_id: The ID of the matching rt rule. This info can be combined
 *  with rt_tbl_idx to locate the exact rule. ID=0x3FF reserved to specify
 *  rt miss. In case of miss, all rt info to be ignored
 * @nat_hit: NAT hit flag: Was their NAT hit?
 * @nat_entry_idx: Index of the NAT entry used of NAT processing
 * @nat_type: Defines the type of the NAT operation:
 *	00: No NAT
 *	01: Source NAT
 *	10: Destination NAT
 *	11: Reserved
 * @tag_info: S/W defined value provided via immediate command
 * @seq_num: Per source endp unique packet sequence number
 * @time_of_day_ctr: running counter from IPA clock
 * @hdr_local: Header table location flag: In header insertion, was the header
 *  taken from the table resides in local memory? (If no, then system mem)
 * @hdr_offset: Offset of used header in the header table
 * @frag_hit: Frag hit flag: Was their frag rule hit in H/W frag table?
 * @frag_rule: Frag rule index in H/W frag table in case of frag hit
 * @endp_dest_idx: Destination end point index.
 * @hw_specific: H/W specific reserved value
 * @ucp: UC Processing flag.
 */
struct ipa_gen_pkt_status_hw_v5_0 {
	u64 status_opcode:8;
	u64 exception:8;
	u64 status_mask:16;
	u64 pkt_len:16;
	u64 endp_src_idx:8;
	u64 reserved_1:3;
	u64 rt_local:1;
	u64 rt_hash:1;
	u64 reserved_2:3;
	u64 metadata:32;
	u64 flt_local:1;
	u64 flt_hash:1;
	u64 flt_global:1;
	u64 flt_ret_hdr:1;
	u64 flt_rule_id:10;
	u64 rt_tbl_idx:8;
	u64 rt_rule_id:10;
	u64 nat_hit:1;
	u64 nat_entry_idx:13;
	u64 nat_type:2;
	u64 tag_info:48;
	u64 seq_num:8;
	u64 time_of_day_ctr:24;
	u64 hdr_local:1;
	u64 hdr_offset:10;
	u64 frag_hit:1;
	u64 frag_rule:4;
	u64 endp_dest_idx:8;
	u64 hw_specific:7;
	u64 ucp:1;
} __packed;

/*
 * struct ipa_frag_pkt_status_hw_v5_0 -
 * IPA v5.0 status packet payload in H/W format.
 *  This structure describes the frag status packet H/W structure for the
 *   following statuses: IPA_NEW_FRAG_RULE.
 * @status_opcode: The Type of the status (Opcode).
 * @frag_rule_idx: Frag rule index value.
 * @reserved_1: reserved
 * @exception: (not bitmask) - the first exception that took place.
 * @tbl_idx: Table index valid or not.
 * @endp_src_idx: Source end point index.
 *  In case of exception, src endp and pkt len are always valid.
 * @seq_num: Packet sequence number.
 * @src_ip_addr: Source packet IP address.
 * @dest_ip_addr: Destination packet IP address.
 * @reserved_2: reserved
 * @nat_type: Defines the type of the NAT operation:
 *	00: No NAT
 *	01: Source NAT
 *	10: Destination NAT
 *	11: Reserved
 * @protocol: Protocal number.
 * @ip_id: IP packet IP ID number.
 * @tlated_ip_addr: IP address.
 * @hdr_local: Header table location flag: In header insertion, was the header
 *  taken from the table resides in local memory? (If no, then system mem)
 * @hdr_offset: Offset of used header in the header table
 * @reserved_3: reserved
 * @ip_cksum_diff: IP packet checksum difference.
 * @metadata: meta data value used by packet
 * @reserved_4: reserved
 * @endp_dest_idx: Destination end point index.
 * @reserved_5: reserved
 */
struct ipa_frag_pkt_status_hw_v5_0 {
	u64 status_opcode:8;
	u64 frag_rule_idx:4;
	u64 reserved_1:2;
	u64 exception:1;
	u64 tbl_idx:1;
	u64 endp_src_idx:8;
	u64 seq_num:8;
	u64 src_ip_addr:32;
	u64 dest_ip_addr:32;
	u64 reserved_2:6;
	u64 nat_type:2;
	u64 protocol:8;
	u64 ip_id:16;
	u64 tlated_ip_addr:32;
	u64 hdr_local:1;
	u64 hdr_offset:10;
	u64 reserved_3:5;
	u64 ip_cksum_diff:16;
	u64 metadata:32;
	u64 reserved_4:16;
	u64 endp_dest_idx:8;
	u64 reserved_5:8;
} __packed;

/*
 * struct ipa_status_pkt_hw_v5_5 - IPA v5.5 status packet payload in H/W format.
 *  This structure describes the status packet H/W structure for the
 *   following statuses: IPA_STATUS_PACKET, IPA_STATUS_DROPPED_PACKET,
 *   IPA_STATUS_SUSPENDED_PACKET.
 *  Other statuses types has different status packet structure.
 * @status_opcode: The Type of the status (Opcode).
 * @exception: (not bitmask) - the first exception that took place.
 *  In case of exception, src endp and pkt len are always valid.
 * @status_mask: Bit mask specifying on which H/W blocks the pkt was processed.
 * @pkt_len: Pkt pyld len including hdr, include retained hdr if used. Does
 *  not include padding or checksum trailer len.
 * @endp_src_idx: Source end point index.
 * @reserved_1: reserved
 * @rt_local: Route table location flag: Does matching rt rule belongs to
 *  rt tbl that resides in lcl memory? (if not, then system mem)
 * @rt_hash: Route hash hit flag: Does matching rt rule was in hash tbl?
 *  Not valid in case of exception
 * @reserved_2: reserved
 * @metadata: meta data value used by packet
 * @flt_local: Filter table location flag: Does matching flt rule belongs to
 *  flt tbl that resides in lcl memory? (if not, then system mem)
 * @flt_hash: Filter hash hit flag: Does matching flt rule was in hash tbl?
 * @flt_global: Global filter rule flag: Does matching flt rule belongs to
 *  the global flt tbl? (if not, then the per endp tables)
 * @flt_ret_hdr: Retain header in filter rule flag: Does matching flt rule
 *  specifies to retain header?
 *  Starting IPA4.5, this will be true only if packet has L2 header.
 * @flt_rule_id: The ID of the matching filter rule. This info can be combined
 *  with endp_src_idx to locate the exact rule. ID=0x3FF reserved to specify
 *  flt miss. In case of miss, all flt info to be ignored
 * @rt_tbl_idx: Index of rt tbl that contains the rule on which was a match
 * @rt_rule_id: The ID of the matching rt rule. This info can be combined
 *  with rt_tbl_idx to locate the exact rule. ID=0x3FF reserved to specify
 *  rt miss. In case of miss, all rt info to be ignored
 * @nat_hit: NAT hit flag: Was their NAT hit?
 * @nat_entry_idx: Index of the NAT entry used of NAT processing
 * @nat_type: Defines the type of the NAT operation:
 *	00: No NAT
 *	01: Source NAT
 *	10: Destination NAT
 *	11: Reserved
 * @tag_info: S/W defined value provided via immediate command
 * @egress_tc: Egress traffic class index.
 * @ingress_tc: Ingress traffic class index.
 * @seq_num: Per source endp unique packet sequence number
 * @time_of_day_ctr: running counter from IPA clock
 * @hdr_local: Header table location flag: In header insertion, was the header
 *  taken from the table resides in local memory? (If no, then system mem)
 * @hdr_offset: Offset of used header in the header table
 * @frag_hit: Frag hit flag: Was their frag rule hit in H/W frag table?
 * @frag_rule: Frag rule index in H/W frag table in case of frag hit
 * @endp_dest_idx: Destination end point index.
 * @hw_specific: H/W specific reserved value
 * @ucp: UC Processing flag.
 * @nat_exc_suppress: nat exception supress flag, indicates whether
 * nat exception is suppressed.
 * @tsp: Traffic shaping policing flag, indicates traffic class info
 * overwrites tag info.
 * @ttl_dec: ttl update flag, indicates whether ttl is updated.
 */
struct ipa_gen_pkt_status_hw_v5_5 {
	u64 status_opcode:8;
	u64 exception:8;
	u64 status_mask:16;
	u64 pkt_len:16;
	u64 endp_src_idx:8;
	u64 reserved_1:3;
	u64 rt_local:1;
	u64 rt_hash:1;
	u64 reserved_2:3;
	u64 metadata:32;
	u64 flt_local:1;
	u64 flt_hash:1;
	u64 flt_global:1;
	u64 flt_ret_hdr:1;
	u64 flt_rule_id:10;
	u64 rt_tbl_idx:8;
	u64 rt_rule_id:10;
	u64 nat_hit:1;
	u64 nat_entry_idx:13;
	u64 nat_type:2;
	u64 tag_info:36;
	u64 egress_tc:6;
	u64 ingress_tc:6;
	u64 seq_num:8;
	u64 time_of_day_ctr:24;
	u64 hdr_local:1;
	u64 hdr_offset:10;
	u64 frag_hit:1;
	u64 frag_rule:4;
	u64 endp_dest_idx:8;
	u64 hw_specific:4;
	u64 nat_exc_suppress:1;
	u64 tsp:1;
	u64 ttl_dec:1;
	u64 ucp:1;
} __packed;

/*
 * struct ipa_frag_pkt_status_hw_v5_5 -
 * IPA v5.5 status packet payload in H/W format.
 *  This structure describes the frag status packet H/W structure for the
 *   following statuses: IPA_NEW_FRAG_RULE.
 * @status_opcode: The Type of the status (Opcode).
 * @frag_rule_idx: Frag rule index value.
 * @reserved_1: reserved
 * @exception: (not bitmask) - the first exception that took place.
 * @tbl_idx: Table index valid or not.
 * @endp_src_idx: Source end point index.
 *  In case of exception, src endp and pkt len are always valid.
 * @seq_num: Packet sequence number.
 * @src_ip_addr: Source packet IP address.
 * @dest_ip_addr: Destination packet IP address.
 * @ret: l2 header retained flag, indicates whether l2 header is retained
 * or not.
 * @ll: low latency indication.
 * @ttl_dec: ttl update indication.
 * @reserved_2: reserved
 * @nat_type: Defines the type of the NAT operation:
 *	00: No NAT
 *	01: Source NAT
 *	10: Destination NAT
 *	11: Reserved
 * @protocol: Protocal number.
 * @ip_id: IP packet IP ID number.
 * @tlated_ip_addr: IP address.
 * @hdr_offset: Offset of used header in the header table
 * @ingress_tc: ingress traffic class index
 * @ip_cksum_diff: IP packet checksum difference.
 * @metadata: meta data value used by packet
 * @reserved_4: reserved
 * @endp_dest_idx: Destination end point index.
 * @egress_tc: egress traffic class index
 * @pd: router disabled ingress policer.
 * @hdr_local: Header table location flag: In header insertion, was the header
 *  taken from the table resides in local memory? (If no, then system mem)
 */
struct ipa_frag_pkt_status_hw_v5_5 {
	u64 status_opcode:8;
	u64 frag_rule_idx:4;
	u64 reserved_1:2;
	u64 exception:1;
	u64 tbl_idx:1;
	u64 endp_src_idx:8;
	u64 seq_num:8;
	u64 src_ip_addr:32;
	u64 dest_ip_addr:32;
	u64 ret:1;
	u64 ll:1;
	u64 ttl_dec:1;
	u64 reserved_2:3;
	u64 nat_type:2;
	u64 protocol:8;
	u64 ip_id:16;
	u64 tlated_ip_addr:32;
	u64 hdr_offset:10;
	u64 ingress_tc:6;
	u64 ip_cksum_diff:16;
	u64 metadata:32;
	u64 reserved_4:16;
	u64 endp_dest_idx:8;
	u64 egress_tc:6;
	u64 pd:1;
	u64 hdr_local:1;
} __packed;

/*
 * struct ipa_frag_pkt_status_hw_v7_0 -
 * IPA v7.0 status packet payload in H/W format.
 *  This structure describes the frag status packet H/W structure for the
 *   following statuses: IPA_NEW_FRAG_RULE.
 * @status_opcode: The Type of the status (Opcode).
 * @frag_rule_idx: Frag rule index value.
 * @reserved_1: reserved
 * @exception: (not bitmask) - the first exception that took place.
 * @tbl_idx: Table index valid or not.
 * @endp_src_idx: Source end point index.
 *  In case of exception, src endp and pkt len are always valid.
 * @seq_num: Packet sequence number.
 * @src_ip_addr: Source packet IP address.
 * @dest_ip_addr: Destination packet IP address.
 * @endp_dest_idx: Destination end point index.
 * @protocol: Protocol number.
 * @ip_id: IP packet IP ID number.
 * @translated_ip_addr: IP address.
 * @hdr_offset: Offset of used header in the header table
 * @ip_cksum_diff: IP packet checksum difference.
 * @metadata: meta data value used by packet
 * @flt_stats_counter_idx: Filter statistics counter index.
 * @ret: l2 header retained flag, indicates whether l2 header is retained
 * or not.
 * @low_latency_or_close_aggr_mod: Low latency or close aggregation mode flag.
 * @ttl_dec: ttl update indication.
 * @packet_type: Type of packet.
 * @rt_stats_counter_idx: Route statistics counter index.
 * @nat_or_ct: NAT or Connection Tracking flag.
 * @hpc: Header Processing Context flag.
 * @pd: Router disabled ingress policer.
 * @hdr_local: Header table location flag: In header insertion, was the header
 *  taken from the table resides in local memory? (If no, then system mem)
 * @ergress_traffic_class: Egress traffic class index.
 * @ingress_traffic_class: Ingress traffic class index.
 * @reserved1: reserved
 * @nat_type: Defines the type of the NAT operation:
 *	00: No NAT
 *	01: Source NAT
 *	10: Destination NAT
 *	11: Reserved
 * @hpc_fetch_len: HPC fetch length.
 * @ct_stats_counter_idx: Connection tracking statistics counter index.
 * @reserved2: reserved
 * @sw_classification_cookie: Software classification cookie.
 */
struct ipa_frag_pkt_status_hw_v7_0 {
	u64 status_opcode:8;
	u64 frag_rule_idx:4;
	u64 reserved_1:2;
	u64 exception:1;
	u64 tbl_idx:1;
	u64 endp_src_idx:8;
	u64 seq_num:8;
	u64 src_ip_addr:32;
	u64 dest_ip_addr:32;
	u64 endp_dest_idx:8;
	u64 protocol:8;
	u64 ip_id:16;
	u64 translated_ip_addr:32;
	u64 hdr_offset:16;
	u64 ip_cksum_diff:16;
	u64 metadata:32;
	u64 flt_stats_counter_idx:12;
	u64 ret:1;
	u64 low_latency_or_close_aggr_mod:1;
	u64 ttl_dec:1;
	u64 packet_type:1;
	u64 rt_stats_counter_idx:12;
	u64 nat_or_ct:1;
	u64 hpc:1;
	u64 pd:1;
	u64 hdr_local:1;
	u64 ergress_traffic_class:8;
	u64 ingress_traffic_class:8;
	u64 reserved1:6;
	u64 nat_type:2;
	u64 hpc_fetch_len:8;
	u64 ct_stats_counter_idx:16;
	u64 reserved2:16;
	u64 sw_classification_cookie:64;
} __packed;

/*
 * struct ipa_status_pkt_hw_v6_0 - IPA v6.0 status packet payload in H/W format.
 *  This structure describes the status packet H/W structure for the
 *   following statuses: IPA_STATUS_PACKET, IPA_STATUS_DROPPED_PACKET,
 *   IPA_STATUS_SUSPENDED_PACKET.
 *  Other statuses types has different status packet structure.
 * @status_opcode: The Type of the status (Opcode).
 * @exception: (not bitmask) - the first exception that took place.
 *  In case of exception, src endp and pkt len are always valid.
 * @status_mask: Bit mask specifying on which H/W blocks the pkt was processed.
 * @pkt_len: Pkt pyld len including hdr, include retained hdr if used. Does
 *  not include padding or checksum trailer len.
 * @endp_src_idx: Source end point index.
 * @pure_ack: Indicates pure ack TCP packet.
 * @syn: Indicates TCP syn packet.
 * @fin_rst: Indicates TCP fin/rst packet.
 * @rt_local: Route table location flag: Does matching rt rule belongs to
 *  rt tbl that resides in lcl memory? (if not, then system mem)
 * @rt_hash: Route hash hit flag: Does matching rt rule was in hash tbl?
 *  Not valid in case of exception
 * @protocol_encoding: Defines the packet protocol:
 * 	0 – None (protocol encoding is not set)
 * 	1 – TCP
 * 	2 – UDP
 * 	3 – ICMP/ IPv6-ICMP (note that Status Mask already has an IPv4/IPv6 bit)
 * @metadata: meta data value used by packet
 * @flt_local: Filter table location flag: Does matching flt rule belongs to
 *  flt tbl that resides in lcl memory? (if not, then system mem)
 * @flt_hash: Filter hash hit flag: Does matching flt rule was in hash tbl?
 * @flt_global: Global filter rule flag: Does matching flt rule belongs to
 *  the global flt tbl? (if not, then the per endp tables)
 * @flt_ret_hdr: Retain header in filter rule flag: Does matching flt rule
 *  specifies to retain header?
 *  Starting IPA4.5, this will be true only if packet has L2 header.
 * @flt_rule_id: The ID of the matching filter rule. This info can be combined
 *  with endp_src_idx to locate the exact rule. ID=0x3FF reserved to specify
 *  flt miss. In case of miss, all flt info to be ignored
 * @rt_tbl_idx: Index of rt tbl that contains the rule on which was a match
 * @rt_rule_id: The ID of the matching rt rule. This info can be combined
 *  with rt_tbl_idx to locate the exact rule. ID=0x3FF reserved to specify
 *  rt miss. In case of miss, all rt info to be ignored
 * @nat_hit: NAT hit flag: Was their NAT hit?
 * @nat_entry_idx: Index of the NAT entry used of NAT processing
 * @nat_type: Defines the type of the NAT operation:
 *	00: No NAT
 *	01: Source NAT
 *	10: Destination NAT
 *	11: Reserved
 * @tag_info: S/W defined value provided via immediate command
 * @seq_num: Per source endp unique packet sequence number
 * @time_of_day_ctr: running counter from IPA clock
 * @hdr_local: Header table location flag: In header insertion, was the header
 *  taken from the table resides in local memory? (If no, then system mem)
 * @hdr_offset: Offset of used header in the header table
 * @frag_hit: Frag hit flag: Was their frag rule hit in H/W frag table?
 * @frag_rule: Frag rule index in H/W frag table in case of frag hit
 * @endp_dest_idx: Destination end point index.
 * @tag_msb_indicator: Indicate MSB of TAG allocation.
 *      00: legacy for TAG
 *      01: Traffic Shaping/Policing traffic class
 *      10: Filter Table Index
 *      11: Reserved
 * @nat_exc_suppress: nat exception supress flag, indicates whether
 * nat exception is suppressed.
 * @ttl_dec: ttl update flag, indicates whether ttl is updated.
 * @ucp: UC Processing flag.
 */
struct ipa_gen_pkt_status_hw_v6_0 {
	u64 status_opcode:8;
	u64 exception:8;
	u64 status_mask:16;
	u64 pkt_len:16;
	u64 endp_src_idx:8;
	u64 pure_ack:1;
	u64 syn:1;
	u64 fin_rst:1;
	u64 rt_local:1;
	u64 rt_hash:1;
	u64 protocol_encoding:3;
	u64 metadata:32;
	u64 flt_local:1;
	u64 flt_hash:1;
	u64 flt_global:1;
	u64 flt_ret_hdr:1;
	u64 flt_rule_id:10;
	u64 rt_tbl_idx:8;
	u64 rt_rule_id:10;
	u64 nat_hit:1;
	u64 nat_entry_idx:13;
	u64 nat_type:2;
	u64 tag_info:48;
	u64 seq_num:8;
	u64 time_of_day_ctr:24;
	u64 hdr_local:1;
	u64 hdr_offset:10;
	u64 frag_hit:1;
	u64 frag_rule:4;
	u64 endp_dest_idx:8;
	u64 reserved:3;
	u64 tag_msb_indicator:2;
	u64 nat_exc_suppress:1;
	u64 ttl_dec:1;
	u64 ucp:1;
} __packed;

/*
 * struct ipa_gen_pkt_status_hw_v7_0 - IPA v7.0 status packet payload in H/W format.
 *  This structure describes the status packet H/W structure for the
 *   following statuses: IPA_STATUS_PACKET, IPA_STATUS_DROPPED_PACKET,
 *   IPA_STATUS_SUSPENDED_PACKET.
 *  Other statuses types has different status packet structure.
 * @status_opcode: The Type of the status (Opcode).
 * @exception: (not bitmask) - the first exception that took place.
 *  In case of exception, src endp and pkt len are always valid.
 * @status_mask: Bit mask specifying on which H/W blocks the pkt was processed.
 * @pkt_len: Pkt pyld len including hdr, include retained hdr if used. Does
 *  not include padding or checksum trailer len.
 * @endp_src_idx: Source end point index.
 * @pure_ack: Indicates pure ack TCP packet.
 * @syn: Indicates TCP syn packet.
 * @fin_rst: Indicates TCP fin/rst packet.
 * @rt_local: Route table location flag: Does matching rt rule belongs to
 *  rt tbl that resides in lcl memory? (if not, then system mem)
 * @rt_cache_hit: Route cache hit flag: Does matching rt rule was in cache?
 *  Not valid in case of exception
 * @protocol_encoding: Defines the packet protocol:
 * 	0 – None (protocol encoding is not set)
 * 	1 – TCP
 * 	2 – UDP
 * 	3 – ICMP/ IPv6-ICMP (note that Status Mask already has an IPv4/IPv6 bit)
 * @metadata: meta data value used by packet
 * @flt_local: Filter table location flag: Does matching flt rule belongs to
 *  flt tbl that resides in lcl memory? (if not, then system mem)
 * @flt_cache_hit: Filter cache hit flag: Does matching flt rule was in cache?
 * @ucp: UC Processing flag.
 * @flt_ret_hdr: Retain header in filter rule flag: Does matching flt rule
 *  specifies to retain header?
 * @flt_rule_id: The ID of the matching filter rule. This info can be combined
 *  with endp_src_idx to locate the exact rule. ID=0x3FF reserved to specify
 *  flt miss. In case of miss, all flt info to be ignored
 * @rt_tbl_idx: Index of rt tbl that contains the rule on which was a match
 * @rt_rule_id: The ID of the matching rt rule. This info can be combined
 *  with rt_tbl_idx to locate the exact rule. ID=0x3FF reserved to specify
 *  rt miss. In case of miss, all rt info to be ignored
 * @nat_entry_idx: Index of the NAT entry used of NAT processing
 * @tag_info: S/W defined value provided via immediate command
 * @seq_num: Per source endp unique packet sequence number
 * @time_of_day_ctr: running counter from IPA clock
 * @hdr_offset: Offset of used header in the header table
 * @hpc: Header Processing Context flag
 * @hdr_in_sys: Header in system memory flag
 * @ttl_dec: ttl update flag, indicates whether ttl is updated.
 * @frag_hit: Frag hit flag: Was their frag rule hit in H/W frag table?
 * @packet_type: Type of packet
 * @endp_dest_idx: Destination end point index.
 * @ergress_traffic_class: Egress traffic class
 * @ingress_traffic_class: Ingress traffic class
 * @nat_hit: NAT hit flag: Was their NAT hit?
 * @nat_type: Defines the type of the NAT operation:
 *	00: No NAT
 *	01: Source NAT
 *	10: Destination NAT
 *	11: Reserved
 * @nat_cache_hit: NAT cache hit flag
 * @nat_or_ct: NAT or Connection Tracking flag
 * @nat_exc_suppress: nat exception supress flag, indicates whether
 * nat exception is suppressed.
 * @num_vlan_tags: Number of VLAN tags
 * @flt_table_idx: Filter table index
 * @frag_rule: Frag rule index in H/W frag table in case of frag hit
 * @metadata_origin: Origin of metadata
 * @reserved1: reserved
 * @reserved2: reserved
 * @reserved3: reserved
 */
struct ipa_gen_pkt_status_hw_v7_0 {
	u64 status_opcode:8;
	u64 exception:8;
	u64 status_mask:16;
	u64 pkt_len:16;
	u64 endp_src_idx:8;
	u64 pure_ack:1;
	u64 syn:1;
	u64 fin_rst:1;
	u64 rt_local:1;
	u64 rt_cache_hit:1;
	u64 protocol_encoding:3;
	u64 metadata:32;
	u64 flt_local:1;
	u64 flt_cache_hit:1;
	u64 ucp:1;
	u64 flt_ret_hdr:1;
	u64 flt_rule_id:10;
	u64 rt_tbl_idx:8;
	u64 rt_rule_id:10;
	u64 nat_entry_idx:16;
	u64 tag_info:48;
	u64 seq_num:8;
	u64 time_of_day_ctr:24;
	u64 hdr_offset:16;
	u64 hpc:1;
	u64 hdr_in_sys:1;
	u64 ttl_dec:1;
	u64 frag_hit:1;
	u64 packet_type:4;
	u64 endp_dest_idx:8;
	u64 ergress_traffic_class:8;
	u64 ingress_traffic_class:8;
	u64 nat_hit:1;
	u64 nat_type:2;
	u64 nat_cache_hit:1;
	u64 nat_or_ct:1;
	u64 nat_exc_suppress:1;
	u64 num_vlan_tags:2;
	u64 flt_table_idx:8;
	u64 frag_rule:4;
	u64 metadata_origin:4;
	u64 reserved1:24;
	u64 reserved2:32;
	u64 reserved3:32;
} __packed;

union ipa_pkt_status_hw {
	struct ipa_gen_pkt_status_hw ipa_pkt;
	struct ipa_frag_pkt_status_hw frag_pkt;
} __packed;

union ipa_pkt_status_hw_v5_0 {
	struct ipa_gen_pkt_status_hw_v5_0 ipa_pkt;
	struct ipa_frag_pkt_status_hw_v5_0 frag_pkt;
} __packed;

union ipa_pkt_status_hw_v5_5 {
	struct ipa_gen_pkt_status_hw_v5_5 ipa_pkt;
	struct ipa_frag_pkt_status_hw_v5_5 frag_pkt;
} __packed;

union ipa_pkt_status_hw_v6_0 {
	struct ipa_gen_pkt_status_hw_v6_0 ipa_pkt;
	struct ipa_frag_pkt_status_hw_v5_5 frag_pkt; //frag_pkt_status didn't changed in IPA6.0
} __packed;

union ipa_pkt_status_hw_v7_0 {
	struct ipa_gen_pkt_status_hw_v7_0 ipa_pkt;
	struct ipa_frag_pkt_status_hw_v7_0 frag_pkt;
} __packed;

/* Size of H/W Packet Status */
#define IPA3_0_PKT_STATUS_SIZE 32
#define IPA7_0_PKT_STATUS_SIZE 48

/* Headers and processing context H/W structures and definitions */

/* uCP command numbers (0-5 is for Q6 XLAT) */
#define IPA_HDR_UCP_802_3_TO_802_3          6
#define IPA_HDR_UCP_802_3_TO_ETHII          7
#define IPA_HDR_UCP_ETHII_TO_802_3          8
#define IPA_HDR_UCP_ETHII_TO_ETHII          9
#define IPA_HDR_UCP_L2TP_HEADER_ADD        10
#define IPA_HDR_UCP_L2TP_HEADER_REMOVE     11
#define IPA_HDR_UCP_L2TP_UDP_HEADER_ADD    12
#define IPA_HDR_UCP_L2TP_UDP_HEADER_REMOVE 13
#define IPA_HDR_UCP_ETHII_TO_ETHII_EX      14
#define IPA_HDR_UCP_SOCKSV5                15
#define IPA_HDR_UCP_SET_DSCP               16
#define IPA_HDR_UCP_EoGRE_HEADER_ADD       17
#define IPA_HDR_UCP_EoGRE_HEADER_REMOVE    18
#define IPA_HDR_UCP_MPLSoGRE_HEADER_ADD    19
#define IPA_HDR_UCP_MPLSoGRE_HEADER_REMOVE 20
#define IPA_HDR_UCP_IPSEC_PRE_ENCAP        21
#define IPA_HDR_UCP_IPSEC_PRE_DECAP        22
#define IPA_Q6_ETH_PDU_UL                  23
#define IPA_Q6_ETH_PDU_DL_14_BYTES         24
#define IPA_Q6_ETH_PDU_DL_18_BYTES         25
#define IPA_HDR_UCP_WWAN_TO_ETHII_EX       26
#define IPA_HDR_UCP_PMIPV6_HEADER_ADD      27
#define IPA_HDR_UCP_PMIPV6_HEADER_REMOVE   28
#define IPA_Q6_IPSEC_BANK_REFILL           29
#define IPA_HDR_UCP_2ND_PASS               30
#define IPA_HDR_UCP_MARK_DSCP              33
#define IPA_HDR_UCP_ETHII_TO_ETHII_EX_DST  37

/* Processing context TLV type */
#define IPA_PROC_CTX_TLV_TYPE_END 0
#define IPA_PROC_CTX_TLV_TYPE_HDR_ADD 1
#define IPA_PROC_CTX_TLV_TYPE_PROC_CMD 3
#define IPA_PROC_CTX_TLV_TYPE_UL_NLO 4
#define IPA_PROC_CTX_TLV_TYPE_TRLR_ADD 5
#define IPA_PROC_CTX_TLV_TYPE_IPSEC 6
#define IPA_PROC_CTX_TLV_TYPE_NXT_RND 7
#define IPA_PROC_CTX_TLV_TYPE_INL_HDR_ADD 8
#define IPA_PROC_CTX_TLV_TYPE_HDR_ADD_CFG 9
#define IPA_PROC_CTX_TLV_TYPE_SW_PROD_COOKIE 10
#define IPA_PROC_CTX_TLV_TYPE_HDR_ADD_ETH_IND 11

/* uC IPsec activate actions */
#define IPA_UC_IPSEC_ACT_DISABLE 0
#define IPA_UC_IPSEC_ACT_ENCAP 0
#define IPA_UC_IPSEC_ACT_DECAP 0

/**
 * struct ipa_hw_hdr_proc_ctx_tlv -
 * HW structure of IPA processing context header - TLV part
 * @type: 0 - end type
 *        1 - header addition type
 *        3 - processing command type
 *        4 - UL-NLO type
 *        5 - Trailer-addition type
 *        6 - IPsec activate
 *        7 - Next-Round rules
 * @length: number of bytes after tlv
 *        for type:
 *        0 - needs to be 0
 *        1 - header addition length
 *        3 - number of 32B including type and length.
 *        4 - TBD
 *        5 - TBD
 *        6 - TBD
 *        7 - TBD
 * @value: specific value for type
 *        for type:
 *        0 - needs to be 0
 *        1 - header length
 *        3 - command ID (see IPA_HDR_UCP_* definitions)
 *        4 - TBD
 *        5 - TBD
 *        6 - 16..17 - SA action (0 = disable, 1 = encap, 2 = decap, 3 = reserved)
 *            18..23 - reserved
 *            24..31 - SA index
 *        7 - 16..23 - FLT table index
 *            24..31 - reserved
 */
struct ipa_hw_hdr_proc_ctx_tlv {
	u32 type:8;
	u32 length:8;
	u32 value:16;
};

/**
 * struct ipa_hw_hdr_proc_ctx_hdr_add -
 * HW structure of IPA processing context - add header tlv
 * @tlv: IPA processing context TLV
 * @hdr_addr: processing context header address
 */
struct ipa_hw_hdr_proc_ctx_hdr_add {
	struct ipa_hw_hdr_proc_ctx_tlv tlv;
	u32 hdr_addr;
	u32 hdr_addr_hi;
};

/**
 * struct ipa_hw_hdr_proc_ctx_l2tp_add_hdr -
 * HW structure of IPA processing context - add l2tp header tlv
 * @tlv: IPA processing context TLV
 * @l2tp_params: l2tp parameters
 */
struct ipa_hw_hdr_proc_ctx_l2tp_add_hdr {
	struct ipa_hw_hdr_proc_ctx_tlv tlv;
	struct ipa_l2tp_header_add_procparams l2tp_params;
};

/**
 * struct ipa_hw_hdr_proc_ctx_l2tp_remove_hdr -
 * HW structure of IPA processing context - remove l2tp header tlv
 * @tlv: IPA processing context TLV
 * @l2tp_params: l2tp parameters
 */
struct ipa_hw_hdr_proc_ctx_l2tp_remove_hdr {
	struct ipa_hw_hdr_proc_ctx_tlv tlv;
	struct ipa_l2tp_header_remove_procparams l2tp_params;
};

/**
 * struct ipa_hw_hdr_proc_ctx_add_hdr_seq -
 * IPA processing context header - add header sequence
 * @hdr_add: add header command
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_add_hdr_seq {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_add_hdr_cmd_seq -
 * IPA processing context header - process command sequence
 * @hdr_add: add header command
 * @cmd: tlv processing command (cmd.type must be 3)
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_add_hdr_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_tlv cmd;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_add_l2tp_hdr_cmd_seq -
 * IPA processing context header - process command sequence
 * @hdr_add: add header command
 * @l2tp_params: l2tp params for header addition
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_add_l2tp_hdr_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_l2tp_add_hdr l2tp_params;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_remove_l2tp_hdr_cmd_seq -
 * IPA processing context header - process command sequence
 * @hdr_add: add header command
 * @l2tp_params: l2tp params for header removal
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_remove_l2tp_hdr_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_l2tp_remove_hdr l2tp_params;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_add_hdr_ex -
 * HW structure of IPA processing context - add generic header
 * @tlv: IPA processing context TLV
 * @params: generic eth2 to eth2 parameters
 */
struct ipa_hw_hdr_proc_ctx_add_hdr_ex {
	struct ipa_hw_hdr_proc_ctx_tlv tlv;
	struct ipa_eth_II_to_eth_II_ex_procparams params;
};

/**
 * struct ipa_hw_hdr_proc_ctx_add_hdr_cmd_seq_ex -
 * IPA processing context header - process command sequence
 * @hdr_add: add header command
 * @params: params for header generic header add
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_add_hdr_cmd_seq_ex {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_add_hdr_ex hdr_add_ex;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};


/**
 * struct ipa_hw_hdr_proc_ctx_add_hdr_wwan_ethII_ex -
 * HW structure of IPA processing context - add generic header
 * @tlv: IPA processing context TLV
 * @params: generic eth2 to eth2 parameters
 */
struct ipa_hw_hdr_proc_ctx_add_hdr_wwan_ethII_ex {
	struct ipa_hw_hdr_proc_ctx_tlv tlv;
	struct ipa_wwan_to_eth_II_ex_procparams params;
};

/**
 * struct ipa_hw_hdr_proc_ctx_add_hdr_wwan_ethII_cmd_seq_ex -
 * IPA processing context header - process command sequence
 * @hdr_add: add header command
 * @params: params for header generic header v2 add(for wwan_ethII)
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_add_hdr_wwan_ethII_cmd_seq_ex {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_add_hdr_wwan_ethII_ex hdr_add_ex;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_add_producer_cookie -
 * HW structure of IPA processing context - add producer cookie
 * @tlv: IPA processing context TLV
 * @params: producer cookie parameters
 */
struct ipa_hw_hdr_proc_ctx_add_producer_cookie {
	struct ipa_hw_hdr_proc_ctx_tlv tlv;
	struct ipa_producer_cookie_procparams params;
};

/**
 * struct ipa_hw_hdr_proc_ctx_remove_l2tp_udp_hdr_cmd_seq -
 * IPA processing context header - process command sequence
 * @l2tp_params: l2tp params for header removal
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_remove_l2tp_udp_hdr_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_l2tp_remove_hdr l2tp_params;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_eogre_add_hdr -
 * HW structure of IPA processing context - add eogre header tlv
 * @tlv: IPA processing context TLV
 * @eogre_params: eogre parameters
 */
struct ipa_hw_hdr_proc_ctx_eogre_add_hdr {
	struct ipa_hw_hdr_proc_ctx_tlv tlv;
	struct ipa_eogre_header_add_procparams eogre_params;
};

/**
 * struct ipa_hw_hdr_proc_ctx_eogre_remove_hdr -
 * HW structure of IPA processing context - remove eogre header tlv
 * @tlv: IPA processing context TLV
 * @eogre_params: eogre parameters
 */
struct ipa_hw_hdr_proc_ctx_eogre_remove_hdr {
	struct ipa_hw_hdr_proc_ctx_tlv tlv;
	struct ipa_eogre_header_remove_procparams eogre_params;
};

/**
 * struct ipa_hw_hdr_proc_ctx_pdn_dscp_upate -
 * HW structure of IPA processing context - update PDN DSCP update
 * @tlv: IPA processing context TLV
 * @pdn_dscp_params: PDN DSCP parameters
 */
struct ipa_hw_hdr_proc_ctx_pdn_dscp_upate {
	struct ipa_hw_hdr_proc_ctx_tlv tlv;
	struct ipa_pdn_dscp_procparams pdn_dscp_params;
};

/**
 * struct ipa_hw_hdr_proc_ctx_add_eogre_hdr_cmd_seq -
 * IPA processing context header - process command sequence
 * @hdr_add: add header command
 * @eogre_params: eogre params for header addition
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_add_eogre_hdr_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_eogre_add_hdr eogre_params;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_remove_eogre_hdr_cmd_seq -
 * IPA processing context header - process command sequence
 * @hdr_add: add header command
 * @eogre_params: eogre params for header removal
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_remove_eogre_hdr_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_eogre_remove_hdr eogre_params;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_tlv_crypto -
 * HW structure of IPA processing context header - TLV part (Crypto special)
 * @type: 6 - Crypto Activate
 * @length: number of bytes after tlv
 * @crypto_action: Crypto action (0 = disable, 1 = IPsec encap, 2 = IPsec decap, 3-15 = reserved)
 * @reserved: reserved
 * @sa_valid: 1 - a valid SA is associated with this Crypto_Action
 * @sa_index: SA index
 */
struct ipa_hw_hdr_proc_ctx_tlv_crypto {
	u32 type:8;
	u32 length:8;
	u32 crypto_action:4;
	u32 reserved:3;
	u32 sa_valid:1;
	u32 sa_index:8;
};

/**
 * struct ipa_hw_hdr_proc_ctx_tlv_nxt_rnd -
 * HW structure of IPA processing context header - TLV part (Next Round special)
 * @type: 7 - Next-Round rules
 * @length: number of bytes after tlv
 * @flt_idx: FLT table index
 * @reserved: reserved
 */
struct ipa_hw_hdr_proc_ctx_tlv_nxt_rnd {
	u32 type:8;
	u32 length:8;
	u32 flt_idx:8;
	u32 reserved:8;
};

/**
 * struct ipa_hw_hdr_proc_ctx_tlv_pre_ipsec -
 * HW structure of IPA processing context - ipsec pre-encap/pre-decap tlv
 * @tlv: IPA processing context TLV
 * @pre_params: pre-encap/pre-decap parameters
 */
struct ipa_hw_hdr_proc_ctx_tlv_pre_ipsec {
	struct ipa_hw_hdr_proc_ctx_tlv tlv;
	union ipa_ipsec_pre_procparams pre_params;
};

/**
 * struct ipa_hw_hdr_proc_ctx_hdr_add_nxt_rnd_cmd_seq -
 * IPA processing context add hdr + next round sequence
 * @hdr_add: add header command
 * @nxt_rnd: tlv Next round rules
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_hdr_add_nxt_rnd_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_tlv_nxt_rnd nxt_rnd;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_ipsec_decap_cmd_seq -
 * IPA processing context next round + command sequence
 * @nxt_rnd: tlv Next round rules
 * @cmd: tlv processing command
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_nxt_rnd_proc_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_tlv_nxt_rnd nxt_rnd;
	struct ipa_hw_hdr_proc_ctx_tlv cmd;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_nxt_rnd_cmd_seq -
 * IPA processing context next round sequence
 * @nxt_rnd: tlv Next round rules
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_nxt_rnd_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_tlv_nxt_rnd nxt_rnd;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_ipsec_proc_cmd_seq -
 * IPA processing context next round + Crypto + command sequence
 * @crypto: tlv Crypto Activate
 * @cmd: tlv uC processing command
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_ipsec_proc_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_tlv_crypto crypto;
	struct ipa_hw_hdr_proc_ctx_tlv_pre_ipsec cmd;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_nxt_rnd_ipsec_proc_cmd_seq -
 * IPA processing context next round + Crypto + command sequence
 * @nxt_rnd: tlv Next round rules
 * @crypto: tlv Crypto Activate
 * @cmd: tlv uC processing command
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_nxt_rnd_ipsec_proc_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_tlv_nxt_rnd nxt_rnd;
	struct ipa_hw_hdr_proc_ctx_tlv_crypto crypto;
	struct ipa_hw_hdr_proc_ctx_tlv_pre_ipsec cmd;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_add_nxt_rnd_proc_cmd_seq -
 * IPA processing context hdr add + next round + command sequence
 * @hdr_add: add header command
 * @nxt_rnd: tlv Next round rules
 * @cmd: tlv uC processing command
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_add_nxt_rnd_proc_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_tlv_nxt_rnd nxt_rnd;
	struct ipa_hw_hdr_proc_ctx_tlv cmd;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_add_ipsec_seq -
 * IPA processing context add + Crypto
 * @hdr_add: add header command
 * @crypto: tlv Crypto Activate
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_add_ipsec_seq {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_tlv_crypto crypto;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_add_ipsec_proc_cmd_seq -
 * IPA processing context add + Crypto + command sequence
 * @hdr_add: add header command
 * @crypto: tlv Crypto Activate
 * @cmd: tlv uC processing command
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_add_ipsec_proc_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_tlv_crypto crypto;
	struct ipa_hw_hdr_proc_ctx_tlv_pre_ipsec cmd;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_add_nxt_rnd_ipsec_proc_cmd_seq -
 * IPA processing context add + next round + Crypto + command sequence
 * @hdr_add: add header command
 * @nxt_rnd: tlv Next round rules
 * @crypto: tlv Crypto Activate
 * @cmd: tlv uC processing command
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_add_nxt_rnd_ipsec_proc_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_tlv_nxt_rnd nxt_rnd;
	struct ipa_hw_hdr_proc_ctx_tlv_crypto crypto;
	struct ipa_hw_hdr_proc_ctx_tlv_pre_ipsec cmd;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

/**
 * struct ipa_hw_hdr_proc_ctx_add_pdn_dscp_proc_cmd_seq -
 * IPA processing context PDN DSCP
 * @hdr_add: add header command
 * @pdn_dscp_params: PDN DSCP params
 * @end: tlv end command (cmd.type must be 0)
 */
struct ipa_hw_hdr_proc_ctx_add_pdn_dscp_proc_cmd_seq {
	struct ipa_hw_hdr_proc_ctx_hdr_add hdr_add;
	struct ipa_hw_hdr_proc_ctx_pdn_dscp_upate pdn_dscp_params;
	struct ipa_hw_hdr_proc_ctx_tlv end;
};

#endif /* _IPAHAL_I_H_ */
