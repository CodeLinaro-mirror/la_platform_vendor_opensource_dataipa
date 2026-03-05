/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#ifndef _IPA_FSE_H_
#define _IPA_FSE_H_

#include <linux/ipa.h>

/**
 * struct ipa_fse_tuple - Flow tuple information for IPA FSE
 * @protocol: Protocol type (TCP, UDP, etc.)
 * @src_port: Source port number
 * @dst_port: Destination port number
 * @ip: Union containing IP address information
 * @ip.v4.src_ip: IPv4 source address
 * @ip.v4.dst_ip: IPv4 destination address
 * @ip.v6.src_ip: IPv6 source address (4x u32 array)
 * @ip.v6.dst_ip: IPv6 destination address (4x u32 array)
 *
 * This structure defines the 5-tuple flow information used by IPA FSE
 * to identify and classify network flows.
 */
struct ipa_fse_tuple {
	union {
		struct {
			u32 src_ip;
			u32 dst_ip;
		} v4;
		struct {
			u32 src_ip[4];
			u32 dst_ip[4];
		} v6;
	} ip;
	u16 src_port;
	u16 dst_port;
	u8 protocol;
};

/**
 * enum ipa_fse_flags - Flags for IPA FSE rule configuration
 * @IPA_FSE_IPV4: IPv4 flow flag
 * @IPA_FSE_IPV6: IPv6 flow flag
 *
 * These flags indicate the IP version type for the FSE rule.
 */
enum ipa_fse_flags {
	IPA_FSE_IPV4 = BIT(0),
	IPA_FSE_IPV6 = BIT(1),
};

/**
 * struct ipa_fse_rule - IPA FSE rule definition
 * @tuple: Flow tuple information (5-tuple)
 * @rdi: Ring ID of REO2SW
 * @ifindex: Network interface index
 * @flags: Rule flags (IPv4/IPv6, etc.)
 * @stream_id: RX stream identifier for the QoS flow, range [0, 255].
 *             Used by the Wi-Fi plugin to map the flow to its RX ring.
 *
 * This structure represents a complete FSE rule that combines flow
 * identification information with processing parameters.
 */
struct ipa_fse_rule {
	struct ipa_fse_tuple tuple;
	u8 rdi;
	int ifindex;
	u32 flags;
	u16 stream_id;
};

/**
 * struct ipa_fse_ops - IPA FSE operations structure
 * @create_fse_rule: Function pointer to create FSE rule
 * @destroy_fse_rule: Function pointer to destroy FSE rule
 *
 * This structure defines the callback operations for IPA FSE rule
 * management. These callbacks are registered with the IPA subsystem
 * to handle flow rule creation and deletion.
 */
struct ipa_fse_ops {
	bool (*create_fse_rule)(struct ipa_fse_rule *rule);
	bool (*destroy_fse_rule)(struct ipa_fse_rule *rule);
};

/* APIs for Wifi/plugins to register FSE callbacks */
int ipa_fse_ops_register(struct ipa_fse_ops *ops);
void ipa_fse_ops_unregister(void);

#endif /* _IPA_FSE_H_ */
