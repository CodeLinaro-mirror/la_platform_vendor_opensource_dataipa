/* SPDX-License-Identifier: GPL-2.0-only WITH Linux-syscall-note */
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#ifndef _UAPI_IPA_STATS_GENL_H_
#define _UAPI_IPA_STATS_GENL_H_

#include <linux/types.h>

#define IPA_STATS_GENL_FAMILY_NAME "ipa_stats"
#define IPA_STATS_GENL_MCGRP_NAME  "ipa_stats_mcgrp"

enum ipa_stats_genl_cmd {
	IPA_STATS_GENL_CMD_UNSPEC,
	IPA_STATS_GENL_CMD_TUPLE_ADD,
	IPA_STATS_GENL_CMD_TUPLE_DEL,
	IPA_STATS_GENL_CMD_STATS_UPDATE,
	IPA_STATS_GENL_CMD_TUPLE_FLUSH,
	IPA_STATS_GENL_CMD_NOTIFY_SET,
	IPA_STATS_GENL_CMD_QUERY_STATS,
	__IPA_STATS_GENL_CMD_MAX,
};

enum ipa_stats_genl_attr {
	IPA_STATS_GENL_ATTR_UNSPEC,
	IPA_STATS_GENL_ATTR_TUPLE_INFO,         /* used by TUPLE_ADD / TUPLE_DEL */
	IPA_STATS_GENL_ATTR_STATS_SNAPSHOT,     /* flat array of ipa_stats_snapshot_entry */
	IPA_STATS_GENL_ATTR_NOTIFY_ENABLE,
	__IPA_STATS_GENL_ATTR_MAX,
};

struct ipa_stats_tuple_info {
	__u8   af;           /* AF_INET or AF_INET6 */
	__u8   protocol;     /* IPPROTO_TCP=6 or IPPROTO_UDP=17 */
	__u8   reserved8_1;
	__u8   reserved8_2;
	__u16  src_port;
	__u16  dst_port;
	__be32 src_addr[4];  /* v4: [0] only; v6: all 4 words */
	__be32 dst_addr[4];  /* v4: [0] only; v6: all 4 words */
	__u32  timer_ticks;  /* snapshot period in 1 ms ticks; must be > 0 */
	__u32  reserved32_1;
	__u16  reserved16_1;
	__u16  reserved16_2;
};

struct stats_nat_ct {
	__u32 num_pkts_inbound;
	__u32 num_pkts_cache_inbound;
	__u64 num_bytes_inbound;
	__u32 num_pkts_outbound;
	__u32 num_pkts_cache_outbound;
	__u64 num_bytes_outbound;
};

/*
 * One entry in a STATS_UPDATE snapshot.
 * IPA_STATS_GENL_ATTR_STATS_SNAPSHOT carries a flat C array of these,
 * so count = nla_len / sizeof(struct ipa_stats_snapshot_entry).
 */
struct ipa_stats_snapshot_entry {
	struct ipa_stats_tuple_info tuple;
	struct stats_nat_ct         stats;
};

#endif /* _UAPI_IPA_STATS_GENL_H_ */
