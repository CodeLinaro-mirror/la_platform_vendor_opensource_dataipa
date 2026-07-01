// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#ifndef _IPA_STATS_GENL_H_
#define _IPA_STATS_GENL_H_

#include <uapi/linux/ipa_stats_genl.h>

int ipa_stats_process_tuple_add(struct ipa_stats_tuple_info *tuple);
void ipa_stats_process_tuple_del(struct ipa_stats_tuple_info *tuple);
void ipa_stats_process_tuple_flush(void);
void ipa_stats_process_notify_set(bool enable);

/*
 * ipa_stats.ko owns all tuple-list / timer / multicast state and the genl
 * family. ipam.ko (built with -DIPA_STATS_PROC_ONLY) only needs to trigger a
 * flush from its datapath teardown (ipa_be_reset). Since ipam.ko loads first
 * and cannot link against ipa_stats.ko, ipa_stats.ko registers its real flush
 * implementation into ipam.ko at module init via this callback; ipam.ko's
 * ipa_stats_process_tuple_flush() stub forwards to it (no-op until registered).
 */
void ipa_stats_register_flush_cb(void (*flush_cb)(void));


#endif /* _IPA_STATS_GENL_H_ */
