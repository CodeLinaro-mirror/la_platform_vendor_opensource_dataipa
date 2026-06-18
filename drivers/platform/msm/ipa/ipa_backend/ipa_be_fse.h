// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#ifndef _IPA_BE_FSE_H_
#define _IPA_BE_FSE_H_

#include <linux/types.h>
#include <linux/netdevice.h>
#include "ipa_api.h"
#include "ipa_fse.h"

/* APIs for IPA Backend to call when adding/deleting flows */
int ipa_be_fse_rule_create(struct ipa_fse_rule *finfo);
int ipa_be_fse_rule_destroy(struct ipa_fse_rule *finfo);
u8 ipa_be_get_ring_id(u32 intf_num);

static inline void ipa_be_fill_ipv4_fse_info(struct ipa_fse_rule *fse_info,
					     const struct ipa_ipv4_5tuple *tuple,
					     const struct ipa_ipv4_connection_rule *conn_rule)
{
	fse_info->tuple.ip.v4.src_ip = ntohl(tuple->flow_ip);
	fse_info->tuple.ip.v4.dst_ip = ntohl(tuple->return_ip);
	fse_info->tuple.src_port = ntohs(tuple->flow_ident);
	fse_info->tuple.dst_port = ntohs(tuple->return_ident);
	fse_info->tuple.protocol = tuple->protocol;
	fse_info->flags = IPA_FSE_IPV4 ;
	fse_info->ifindex = conn_rule->flow_interface_num;
	fse_info->rdi = ipa_be_get_ring_id(conn_rule->flow_interface_num);
	fse_info->stream_id = IPA_FSE_STREAM_ID_INVALID;
}

static inline void ipa_be_fill_ipv4_fse_info_reverse(struct ipa_fse_rule *fse_info,
						     const struct ipa_ipv4_5tuple *tuple,
						     const struct ipa_ipv4_connection_rule *conn_rule)
{
	/* Use the xlate (post-NAT private) IP so the FSE src matches the WLAN
	 * client address. When there is no NAT, return_ip_xlate == return_ip. */
	fse_info->tuple.ip.v4.src_ip = ntohl(conn_rule->return_ip_xlate);
	fse_info->tuple.ip.v4.dst_ip = ntohl(tuple->flow_ip);
	fse_info->tuple.src_port = ntohs(conn_rule->return_ident_xlate);
	fse_info->tuple.dst_port = ntohs(tuple->flow_ident);
	fse_info->tuple.protocol = tuple->protocol;
	fse_info->flags = IPA_FSE_IPV4 ;
	fse_info->ifindex = conn_rule->return_interface_num;
	fse_info->rdi = ipa_be_get_ring_id(conn_rule->return_interface_num);
	fse_info->stream_id = IPA_FSE_STREAM_ID_INVALID;
}

static inline void ipa_be_fill_ipv6_fse_info(struct ipa_fse_rule *fse_info,
					     const struct ipa_ipv6_5tuple *tuple,
					     const struct ipa_ipv6_connection_rule *conn_rule)
{
	int i;

	for (i = 0; i < 4; i++) {
		fse_info->tuple.ip.v6.src_ip[i] = ntohl(tuple->flow_ip[i]);
		fse_info->tuple.ip.v6.dst_ip[i] = ntohl(tuple->return_ip[i]);
	}
	fse_info->tuple.src_port = ntohs(tuple->flow_ident);
	fse_info->tuple.dst_port = ntohs(tuple->return_ident);
	fse_info->tuple.protocol = tuple->protocol;
	fse_info->flags = IPA_FSE_IPV6;
	fse_info->ifindex = conn_rule->flow_interface_num;
	fse_info->rdi = ipa_be_get_ring_id(conn_rule->flow_interface_num);
	fse_info->stream_id = IPA_FSE_STREAM_ID_INVALID;
}

static inline void ipa_be_fill_ipv6_fse_info_reverse(struct ipa_fse_rule *fse_info,
						     const struct ipa_ipv6_5tuple *tuple,
						     const struct ipa_ipv6_connection_rule *conn_rule)
{
	/* Use the xlate (post-NAT private) IP so the FSE src matches the WLAN
	 * client address. When there is no NAT, return_ip_xlate == return_ip. */
	int i;

	for (i = 0; i < 4; i++) {
		fse_info->tuple.ip.v6.src_ip[i] = ntohl(conn_rule->return_ip_xlate[i]);
		fse_info->tuple.ip.v6.dst_ip[i] = ntohl(tuple->flow_ip[i]);
	}
	fse_info->tuple.src_port = ntohs(conn_rule->return_ident_xlate);
	fse_info->tuple.dst_port = ntohs(tuple->flow_ident);
	fse_info->tuple.protocol = tuple->protocol;
	fse_info->flags = IPA_FSE_IPV6;
	fse_info->ifindex = conn_rule->return_interface_num;
	fse_info->rdi = ipa_be_get_ring_id(conn_rule->return_interface_num);
	fse_info->stream_id = IPA_FSE_STREAM_ID_INVALID;
}

#endif /* _IPA_BE_FSE_H_ */
