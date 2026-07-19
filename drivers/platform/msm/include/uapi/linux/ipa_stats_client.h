/* SPDX-License-Identifier: GPL-2.0-only WITH Linux-syscall-note */
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#ifndef _IPA_STATS_CLIENT_H_
#define _IPA_STATS_CLIENT_H_

#include <linux/ipa_stats_genl.h>

struct ipa_stats_client;
typedef struct ipa_stats_client *ipa_stats_client_t;

/* Invoked once per snapshot; entries[] covers all tracked connections. */
typedef void (*ipa_stats_cb_t)(const struct ipa_stats_snapshot_entry *entries,
			       unsigned int count);

/* Returns handle on success, NULL on failure. */
ipa_stats_client_t ipa_stats_client_init(ipa_stats_cb_t cb);

/* Stops the receive thread and frees the handle. No-op if client is NULL. */
void ipa_stats_client_deinit(ipa_stats_client_t client);

/* Returns 0 on success, negative errno on failure. */
int ipa_stats_tuple_add(ipa_stats_client_t client,
			const struct ipa_stats_tuple_info *tuple);

int ipa_stats_tuple_del(ipa_stats_client_t client,
			const struct ipa_stats_tuple_info *tuple);

int ipa_stats_tuple_flush(ipa_stats_client_t client);

int ipa_stats_notifications_set(ipa_stats_client_t client, int enable);

int ipa_stats_tuple_dump(ipa_stats_client_t client,
			 struct ipa_stats_snapshot_entry **out_entries,
			 unsigned int *out_count);

#endif /* _IPA_STATS_CLIENT_H_ */
