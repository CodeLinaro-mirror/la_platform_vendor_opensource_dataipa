/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#ifndef _IPAHAL_HW_STATS_H_
#define _IPAHAL_HW_STATS_H_

#include "ipa.h"

#define IPAHAL_MAX_PIPES 32
#define IPAHAL_MAX_PIPES_PER_REG 32
#define IPAHAL_IPA5_PIPES_NUM 36
#define IPAHAL_IPA6_PIPES_NUM 50
#define IPAHAL_IPA5_PIPE_REG_NUM 2
#define IPAHAL_IPA7_PIPES_NUM 75
#define IPAHAL_PIPES_NUM IPAHAL_IPA7_PIPES_NUM
#define IPAHAL_IPA5_PRODUCER_PIPE_NUM 16
#define IPAHAL_IPA5_2_PRODUCER_PIPE_NUM 11
#define IPAHAL_IPA6_PRODUCER_PIPE_NUM 22
#define IPAHAL_IPA7_PRODUCER_PIPE_NUM 32
#define IPAHAL_IPA7_1PRODUCER_PIPE_NUM 13
#define IPAHAL_PRODUCER_PIPE_NUM IPAHAL_IPA7_PRODUCER_PIPE_NUM
#define IPAHAL_MAX_RULE_ID_32 (1024 / 32) /* 10 bits of rule id */

#ifndef NUM_VARS_FOR_BITS
#define NUM_VARS_FOR_BITS(type, bits) \
	(((bits) + (sizeof(type) * BITS_PER_BYTE - 1)) / (sizeof(type) * BITS_PER_BYTE))
#endif

#define IPAHAL_PIPE_REG_NUM NUM_VARS_FOR_BITS(u32, IPAHAL_PIPES_NUM)

#define IPAHAL_NAT_CT_MAX_COUNTERS 1024

/*
 * Field packing for the "max counters" registers. Each such register holds two
 * 16-bit max-counter-index fields.
 *
 * The FnR (filter/route) registers
 * (IPA_STAT_{IPV4,IPV6,NONIP}_FILTER_ROUTER_MAX_COUNTERS and their
 * IPA_DROP_STAT_* counterparts) split into a filter half and a router half:
 */
#define IPAHAL_STAT_FNR_FILTER_MAX_COUNTERS_BMSK (0xffff0000)
#define IPAHAL_STAT_FNR_FILTER_MAX_COUNTERS_SHFT (0x10)
#define IPAHAL_STAT_FNR_ROUTER_MAX_COUNTERS_BMSK (0x0000ffff)
#define IPAHAL_STAT_FNR_ROUTER_MAX_COUNTERS_SHFT (0x0)

/*
 * The NAT/CT registers
 * (IPA_STAT_NAT_AND_CONN_TRACK_MAX_COUNTERS and
 * IPA_DROP_STAT_NAT_AND_CONN_TRACK_MAX_COUNTERS) split into an IPv4 half and
 * an IPv6 half:
 */
#define IPAHAL_STAT_IPV6_MAX_COUNTERS_BMSK (0xffff0000)
#define IPAHAL_STAT_IPV6_MAX_COUNTERS_SHFT (0x10)
#define IPAHAL_STAT_IPV4_MAX_COUNTERS_BMSK (0x0000ffff)
#define IPAHAL_STAT_IPV4_MAX_COUNTERS_SHFT (0x0)

/* Largest max-counter index representable in any of the 16-bit fields above */
#define IPAHAL_STAT_MAX_COUNTERS_LIMIT (0xffff)

enum ipahal_hw_stats_type {
	IPAHAL_HW_STATS_QUOTA,
	IPAHAL_HW_STATS_TETHERING,
	IPAHAL_HW_STATS_FNR,
	IPAHAL_HW_STATS_DROP,
	IPAHAL_HW_STATS_NAT_CT,
	IPAHAL_HW_STATS_MAX
};

/*
 * struct ipahal_stats_init_pyld - Statistics initialization payload
 * @len: length of payload
 * @data: actual payload data
 */
struct ipahal_stats_init_pyld {
	u16 len;
	u16 reserved;
	u8 data[0];
};

/*
 * struct ipahal_stats_offset - Statistics offset parameters
 * @offset: offset of the statistic from beginning of stats table
 * @size: size of the statistics
 */
struct ipahal_stats_offset {
	u32 offset;
	u16 size;
};

/*
 * struct ipahal_stats_init_quota - Initializations parameters for quota
 * @enabled_bitmask: bit mask of pipes to be monitored
 */
struct ipahal_stats_init_quota {
	u32 enabled_bitmask[IPAHAL_PIPE_REG_NUM];
};

/*
 * struct ipahal_stats_get_offset_quota - Get offset parameters for quota
 * @init: initialization parameters used in initialization of stats
 */
struct ipahal_stats_get_offset_quota {
	struct ipahal_stats_init_quota init;
};

/*
 * struct ipahal_stats_quota - Quota statistics
 * @num_ipv4_bytes: IPv4 bytes
 * @num_ipv6_bytes: IPv6 bytes
 * @num_ipv4_pkts: IPv4 packets
 * @num_ipv6_pkts: IPv6 packets
 */
struct ipahal_stats_quota {
	u64 num_ipv4_bytes;
	u64 num_ipv6_bytes;
	u64 num_ipv4_pkts;
	u64 num_ipv6_pkts;
};

/*
 * struct ipahal_stats_quota_all - Quota statistics for all pipes
 * @stats: array of statistics per pipe
 */
struct ipahal_stats_quota_all {
	struct ipahal_stats_quota stats[IPAHAL_PIPES_NUM];
};

/*
 * struct ipahal_stats_init_tethering - Initializations parameters for tethering
 * @prod_bitmask: bit mask of producer pipes to be monitored
 * @cons_bitmask: bit mask of consumer pipes to be monitored per producer
 * @eth_bitmask: bit mask of pipes whose counters include ETH header bytes
 *	(IPA 7.0+ only). A pair's ETH bit is the OR of its producer's and
 *	consumer's bits, so a per-pipe mask suffices. Ignored pre-7.0.
 * @nonip_bitmask: bit mask of pipes whose counters carry a Non-IP field
 *	(IPA 7.0+ only). Same OR-of-endpoints policy as eth_bitmask.
 *	Ignored pre-7.0.
 */
struct ipahal_stats_init_tethering {
	u32 prod_bitmask[IPAHAL_PIPE_REG_NUM];
	u32 cons_bitmask[IPAHAL_PIPES_NUM][IPAHAL_PIPE_REG_NUM];
	u32 eth_bitmask[IPAHAL_PIPE_REG_NUM];
	u32 nonip_bitmask[IPAHAL_PIPE_REG_NUM];
};

/*
 * struct ipahal_stats_get_offset_tethering - Get offset parameters for
 *	tethering
 * @init: initialization parameters used in initialization of stats
 */
struct ipahal_stats_get_offset_tethering {
	struct ipahal_stats_init_tethering init;
};

/*
 * struct ipahal_stats_tethering - Tethering statistics
 * @num_ipv4_bytes: IPv4 bytes
 * @num_ipv6_bytes: IPv6 bytes
 * @num_ipv4_pkts: IPv4 packets
 * @num_ipv6_pkts: IPv6 packets
 * @num_nonip_bytes: Non-IP bytes (IPA 7.0+, only when the pair's Non-IP
 *	bit is set)
 * @num_nonip_pkts: Non-IP packets (IPA 7.0+, only when the pair's Non-IP
 *	bit is set)
 */
struct ipahal_stats_tethering {
	u64 num_ipv4_bytes;
	u64 num_ipv6_bytes;
	u64 num_ipv4_pkts;
	u64 num_ipv6_pkts;
	u64 num_nonip_bytes;
	u64 num_nonip_pkts;
};

/*
 * struct ipahal_stats_tethering_all - Tethering statistics for all pipes
 * @stats: matrix of statistics per pair of pipes
 */
struct ipahal_stats_tethering_all {
	struct ipahal_stats_tethering
		stats[IPAHAL_PIPES_NUM][IPAHAL_PIPES_NUM];
};

/*
 * struct ipahal_stats_init_flt_rt - Initializations parameters for flt_rt
 * @rule_id_bitmask: array describes which rule ids to monitor.
 *	rule_id bit is determined by:
 *		index to the array => rule_id / 32
 *		bit to enable => rule_id % 32
 */
struct ipahal_stats_init_flt_rt {
	u32 rule_id_bitmask[IPAHAL_MAX_RULE_ID_32];
};

/*
 * struct ipahal_stats_get_offset_flt_rt - Get offset parameters for flt_rt
 * @init: initialization parameters used in initialization of stats
 * @rule_id: rule_id to get the offset for
 */
struct ipahal_stats_get_offset_flt_rt {
	struct ipahal_stats_init_flt_rt init;
	u32 rule_id;
};

/*
 * struct ipahal_stats_flt_rt - flt_rt statistics
 * @num_packets: Total number of packets hit this rule
 * @num_packets_hash: Total number of packets hit this rule in hash table
 */
struct ipahal_stats_flt_rt {
	u32 num_packets;
	u32 num_packets_hash;
};

/*
 * struct ipahal_stats_flt_rt_v4_5 - flt_rt statistics
 * @num_packets: Total number of packets hit this rule
 * @num_packets_hash: Total number of packets hit this rule in hash table
 * @num_bytes: Total number of bytes hit this rule
 */
struct ipahal_stats_flt_rt_v4_5 {
	u32 num_packets;
	u32 num_packets_hash;
	u64 num_bytes;
};

/*
 * struct ipahal_stats_get_offset_flt_rt_v4_5 - Get offset parameters for flt_rt
 * @start_id: start_id to get the offset
 * @end_id: end_id to get the offset
 */
struct ipahal_stats_get_offset_flt_rt_v4_5 {
	u8 start_id;
	u8 end_id;
};

/*
 * struct ipahal_stats_init_drop - Initializations parameters for Drop
 * @enabled_bitmask: bit mask of pipes to be monitored
 */
struct ipahal_stats_init_drop {
	u32 enabled_bitmask[IPAHAL_PIPE_REG_NUM];
};

/*
 * struct ipahal_stats_get_offset_drop - Get offset parameters for Drop
 * @init: initialization parameters used in initialization of stats
 */
struct ipahal_stats_get_offset_drop {
	struct ipahal_stats_init_drop init;
};

/*
 * struct ipahal_stats_drop - Packet Drop statistics
 * @drop_packet_cnt: number of packets dropped
 * @drop_byte_cnt: number of bytes dropped
 */
struct ipahal_stats_drop {
	u32 drop_packet_cnt;
	u32 drop_byte_cnt;
};

/*
 * struct ipahal_stats_drop_all - Drop statistics for all pipes
 * @stats: array of statistics per pipes
 */
struct ipahal_stats_drop_all {
	struct ipahal_stats_drop stats[IPAHAL_PIPES_NUM];
};

/*
 * struct ipahal_stats_init_nat_ct - Initializations parameters for NAT/CT
 * @max_counters: max counters
 */
struct ipahal_stats_init_nat_ct {
	u16 max_counters;
};

/*
 * struct ipahal_stats_get_offset_nat_ct - Get offset parameters for NAT/CT
 * @counter_index: counter index to get the offset for (1-1024)
 */
struct ipahal_stats_get_offset_nat_ct {
	u16 counter_index;
	u16 max_counters;
};

/*
 * struct ipahal_stats_nat_ct - NAT/CT statistics
 * @num_pkts_inbound: Number of inbound packets
 * @num_pkts_cache_inbound: Number of cached inbound packets
 * @num_bytes_inbound: Number of inbound bytes
 * @num_pkts_outbound: Number of outbound packets
 * @num_pkts_cache_outbound: Number of cached outbound packets
 * @num_bytes_outbound: Number of outbound bytes
 */
struct ipahal_stats_nat_ct {
	u32 num_pkts_inbound;
	u32 num_pkts_cache_inbound;
	u64 num_bytes_inbound;
	u32 num_pkts_outbound;
	u32 num_pkts_cache_outbound;
	u64 num_bytes_outbound;
};

/*
 * ipahal_stats_generate_init_pyld - Generate the init payload for stats
 * @type: type of stats
 * @params: init_pyld parameters based of stats type
 * @is_atomic_ctx: is calling context atomic ?
 *
 * This function will generate the initialization payload for a particular
 * statistic in hardware. IPA driver is expected to use this payload to
 * initialize the SRAM.
 *
 * Return: pointer to ipahal_stats_init_pyld on success or NULL on failure.
 */
struct ipahal_stats_init_pyld *ipahal_stats_generate_init_pyld(
	enum ipahal_hw_stats_type type, void *params, bool is_atomic_ctx);

/*
 * ipahal_stats_generate_teth_cfg_pyld - Generate the tethering CFG payload
 * @init: tethering init parameters (eth_bitmask/nonip_bitmask and the
 *	prod/cons bitmasks)
 * @is_atomic_ctx: is calling context atomic ?
 *
 * Packs the per-counter ETH-header-byte and Non-IP config bits (2 bits per
 * counter, 4 counters per byte) for the IPA 7.0+ tethering CFG SRAM region,
 * written separately from the header/counter payload from
 * ipahal_stats_generate_init_pyld().
 *
 * Return: pointer to ipahal_stats_init_pyld on success or NULL on failure.
 */
struct ipahal_stats_init_pyld *ipahal_stats_generate_teth_cfg_pyld(
	struct ipahal_stats_init_tethering *init, bool is_atomic_ctx);

/*
 * ipahal_destroy_stats_init_pyld() - Destroy/Release bulk that was built
 *  by the ipahal_stats_generate_init_pyld function.
 */
static inline void ipahal_destroy_stats_init_pyld(
	struct ipahal_stats_init_pyld *pyld)
{
	kfree(pyld);
}

/*
 * ipahal_stats_get_offset - Get the offset / size of payload for stats
 * @type: type of stats
 * @params: get_offset parameters based of stats type
 * @out: out parameter for the offset and size.
 *
 * This function will return the offset of the counter from beginning of
 * the table.IPA driver is expected to read this portion in SRAM and pass
 * it to ipahal_parse_stats() to interprete the stats.
 *
 * Return: 0 on success and negative on failure
 */
int ipahal_stats_get_offset(enum ipahal_hw_stats_type type, void *params,
	struct ipahal_stats_offset *out);

/*
 * ipahal_parse_stats - parse statistics
 * @type: type of stats
 * @init_params: init_pyld parameters used on init
 * @raw_stats: stats read from IPA SRAM
 * @parsed_stats: pointer to parsed stats based on type
 *
 * Return: 0 on success and negative on failure
 */
int ipahal_parse_stats(enum ipahal_hw_stats_type type, void *init_params,
	void *raw_stats, void *parsed_stats);


/*
 * ipahal_set_flt_rt_sw_stats - set sw counter stats for FnR
 * @raw_stats: stats write to IPA SRAM
 * @sw_stats: FnR sw stats to be written
 *
 * Return: None
 */
void ipahal_set_flt_rt_sw_stats(void *raw_stats,
	struct ipa_flt_rt_stats sw_stats);

/*
 * ipahal_set_nat_ct_sw_stats - set sw counter stats for NAT/CT
 * @raw_stats: stats write to IPA SRAM
 * @sw_stats: NAT/CT sw stats to be written
 *
 * Return: None
 */
void ipahal_set_nat_ct_sw_stats(void *raw_stats,
	struct ipahal_stats_nat_ct sw_stats);

/*
 * ipahal_stats_get_hw_entry_size - Get the HW (on-SRAM) size of a single
 *	counter entry for the given stats type
 * @type: type of stats
 *
 * The per-counter HW layout is internal to ipahal; callers use this to derive
 * how many counter entries fit in a given SRAM partition without depending on
 * the internal struct definitions. The size returned reflects the IPA HW
 * version ipahal was initialized with.
 *
 * Return: entry size in bytes, or 0 for an unsupported type
 */
u32 ipahal_stats_get_hw_entry_size(enum ipahal_hw_stats_type type);

#endif /* _IPAHAL_HW_STATS_H_ */
