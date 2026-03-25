// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

/*
 * This module implements IPA IPsec offload interface.
 *
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ipa.h>
#include <linux/inetdevice.h>
#include <net/addrconf.h>
#include "ipa_i.h"
#include <linux/netdevice.h>

#define ESP_PAD_LEN 16
#define IPA_IPSEC_DL_POL_FLT_ID (IPA_CLIENT_MAX)
#define META_IS_IPSEC 0x10
#define META_SA_MASK  0xF
#define META_SA_SHIFT 0
#define IPSEC_WORKQUEUE_NAME "ipa_ipsec_wq"

/* Static system storage for SA construction and mirroring, to avoid unaligned SRAM access */
static struct ipa_ipsec_sa_encap esa;
static struct ipa_ipsec_sa_decap dsa;

static struct {
	char encap_hdr[IPA_RESOURCE_NAME_MAX];
	char encap_rt[IPA_IP_MAX][IPA_RESOURCE_NAME_MAX];
	char decap_rt[IPA_IP_MAX][IPA_RESOURCE_NAME_MAX];
	char decap_no_policy_rt[IPA_IP_MAX][IPA_RESOURCE_NAME_MAX];
	char default_rt[IPA_RESOURCE_NAME_MAX];
	char dl_rt[IPA_IP_MAX][IPA_RESOURCE_NAME_MAX];
} __ipa_ipsec_s = {
	.encap_hdr = "IPSEC_SA_",
	.encap_rt = {"IPSEC_ENCAP_v4", "IPSEC_ENCAP_v6"},
	.decap_rt = {"IPSEC_DECAP_v4", "IPSEC_DECAP_v6"},
	.decap_no_policy_rt = {"IPSEC_DECAP_NO_POLICY_v4", "IPSEC_DECAP_NO_POLICY_v6"},
	.default_rt = IPA_DFLT_WAN_RT_TBL_NAME,
	.dl_rt = {"COMRTBLLANv4", "WANRTBLv6"},
};

static struct ipa_ipsec_algo _aalg_map[] = {
	{ .name = "digest_null",	.algo.a = IPA_IPSEC_AUTH_NONE },
	{ .name = "hmac(sha1)",		.algo.a = IPA_IPSEC_AUTH_HMAC_SHA1},
	{ .name = "hmac(sha256)",	.algo.a = IPA_IPSEC_AUTH_HMAC_SHA2_256},
	{ .name = "hmac(sha384)",	.algo.a = IPA_IPSEC_AUTH_HMAC_SHA2_384},
	{ .name = "hmac(sha512)",	.algo.a = IPA_IPSEC_AUTH_HMAC_SHA2_512},
	{ .name = NULL,			.algo.a = IPA_IPSEC_AUTH_MAX},
};

static struct ipa_ipsec_algo _ealg_map[] = {
	{ .name = "ecb(cipher_null)",	.algo.e = IPA_IPSEC_ENC_NULL},
	{ .name = "cbc(aes)",		.algo.e = IPA_IPSEC_ENC_AES_CBC},
	{ .name = NULL,			.algo.e = IPA_IPSEC_ENC_MAX},
};

static struct ipa_ipsec_algo _aead_map[] = {
	{ .name = "rfc4106(gcm(aes))",	.algo.e = IPA_IPSEC_ENC_AES_GCM_16},
	{ .name = NULL,			.algo.e = IPA_IPSEC_ENC_MAX},
};

static struct workqueue_struct *ipa_ipsec_wq;

bool ipa_ipsec_initialized(void)
{
	return ipa3_ctx->ipsec && ipa3_ctx->ipsec->initialized;
}

bool ipa_ipsec_enabled(void)
{
	return ipa3_ctx->ipsec && ipa3_ctx->ipsec->enabled &&
		!ipa3_ctx->eth_pdu_ctx.eth_pdu_mode_enabled;
}

/*
 * ipa_ipsec_install_key_test()
 * 	Install a key.
 */
int ipa_ipsec_install_key_test(u8 idx, enum ipa_ipsec_key_type type, enum ipa_ipsec_key_len len_code, void *key)
{
	struct ipa_ipsec_key_store *keys;
	size_t len;

	if (!ipa3_ctx->ipsec || !ipa3_ctx->ipsec->keys)
		return -EFAULT;

	if (idx >= IPA_IPSEC_MAX_KEY_NUM)
		return -EINVAL;

	if (len_code == IPA_IPSEC_KEY_0)
		return 0;

	keys = ipa3_ctx->ipsec->keys;

	switch (type) {
	case IPA_IPSEC_KEY_ENC:
		switch (len_code) {
		case IPA_IPSEC_KEY_128:
			len = 16;
			break;
		case IPA_IPSEC_KEY_256:
			len = 32;
			break;
		default:
			return -EINVAL;
		}
		memcpy_toio((void __iomem *)&keys->enc[idx], key, len);
		break;
	case IPA_IPSEC_KEY_AUTH:
		switch (len_code) {
		case IPA_IPSEC_KEY_128:
			len = 16;
			break;
		case IPA_IPSEC_KEY_256:
			len = 32;
			break;
		case IPA_IPSEC_KEY_512:
			len = 64;
			break;
		case IPA_IPSEC_KEY_160:
			len = 20;
			break;
		case IPA_IPSEC_KEY_384:
			len = 48;
			break;
		default:
			return -EINVAL;
		}
		memcpy_toio((void __iomem *)&keys->auth[idx], key, len);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

/*
 * ipa_ipsec_install_encap_sa()
 * 	Install an encap SA.
 */
static int ipa_ipsec_install_encap_sa(u8 idx, struct ipa_ipsec_sa_encap *sa)
{
	if (!sa || idx >= IPA_IPSEC_MAX_SA_NUM) {
		IPAERR("Invalid input sa is NULL, idx = %d", idx);
		return -EINVAL;
	}

	if (!ipa3_ctx->ipsec || !ipa3_ctx->ipsec->encap) {
		IPAERR("IPsec is not initialized\n");
		return -EFAULT;
	}

	/* memcopy to SRAM */
	memcpy_toio((void __iomem *)(ipa3_ctx->ipsec->encap + idx), (void *)sa,
		sizeof(struct ipa_ipsec_sa_encap));

	return 0;
}

/*
 * ipa_ipsec_install_decap_sa()
 * 	Install an decap SA.
 */
int ipa_ipsec_install_decap_sa(u8 idx, struct ipa_ipsec_sa_decap *sa)
{
	if (!sa || idx >= IPA_IPSEC_MAX_SA_NUM)
		return -EINVAL;

	if (!ipa3_ctx->ipsec || !ipa3_ctx->ipsec->decap)
		return -EFAULT;

	/* memcopy to SRAM */
	memcpy_toio((void __iomem *)(ipa3_ctx->ipsec->decap + idx), (void *)sa,
		sizeof(struct ipa_ipsec_sa_decap));

	return 0;
}

/*
 * ipa_ipsec_stop_sa()
 * 	Stop an SA.
 */
int ipa_ipsec_stop_sa(u8 idx, enum ipa_ipsec_sa_type sa_type)
{
	if (sa_type >= IPA_IPSEC_TYPE_MAX || idx >= IPA_IPSEC_MAX_SA_NUM)
		return -EINVAL;

	if (!ipa3_ctx->ipsec || !ipa3_ctx->ipsec->encap)
		return -EFAULT;

	switch (sa_type) {
	case IPA_IPSEC_ENCAP:
		ipa3_ctx->ipsec->encap[idx].dyna.is_stopped = 1;
		break;
	case IPA_IPSEC_DECAP:
		ipa3_ctx->ipsec->decap[idx].dyna.is_stopped = 1;
		break;
	default:
		break;
	}

	return 0;
}

/* Lookup for next free HW SA index, return IPA_IPSEC_MAX_SA_NUM, if not found */
static u8 _ipa_ipsec_next_free_sa(enum ipa_ipsec_sa_type sa_type)
{
	u8 idx = 0;
	while (ipa3_ctx->ipsec->sa_db[sa_type][idx].x && idx < IPA_IPSEC_MAX_SA_NUM)
		idx++;

	return idx;
}

/* Translate XFRM aead/ealg name into IPA enc algo ID */
static u32 _ipa_ipsec_xfrm_sa_enc_get(struct xfrm_state *x)
{
	int i = 0;

	IPADBG_LOW("Start\n");

	while (x->aead && _aead_map[i].name) {
		if (!strncmp(_aead_map[i].name, x->aead->alg_name, XFRM_ALG_NAME_MAX)) {
			IPADBG_LOW("_aead_map[i].name = %s\n", _aead_map[i].name);
			return _aead_map[i].algo.e;
		}
		i++;
	}

	i = 0;
	while (x->ealg && _ealg_map[i].name) {
		if (!strncmp(_ealg_map[i].name, x->ealg->alg_name, XFRM_ALG_NAME_MAX)) {
			IPADBG_LOW("_ealg_map[i].name = %s\n", _ealg_map[i].name);
			return _ealg_map[i].algo.e;
		}
		i++;
	}

	return IPA_IPSEC_ENC_MAX;
}

/* Translate ipa_ipsec_sa_auth enum to name */
const char * ipa_ipsec_get_auth_algo_name(enum ipa_ipsec_sa_auth auth_algo)
{
	int i = 0;

	while(_aalg_map[i].name) {
		if(_aalg_map[i].algo.a == auth_algo)
			return _aalg_map[i].name;
		i++;
	}

	return NULL;
}

/* Translate ipa_ipsec_sa_enc enum to name */
const char * ipa_ipsec_get_encr_algo_name(enum ipa_ipsec_sa_enc encr_algo)
{
	int i = 0;

	while(_aead_map[i].name) {
		if(_aead_map[i].algo.e == encr_algo)
			return _aead_map[i].name;
		i++;
	}

	i = 0;
	while(_ealg_map[i].name) {
		if(_ealg_map[i].algo.e == encr_algo)
			return _ealg_map[i].name;
		i++;
	}

	return NULL;
}

/* Translate XFRM aalg name into IPA auth algo ID */
static u32 _ipa_ipsec_xfrm_sa_auth_get(struct xfrm_state *x)
{
	int i = 0;

	IPADBG_LOW("Start\n");

	/* address of array 'x->aalg->alg_name' will always evaluate to 'true' */
	while (x->aalg && _aalg_map[i].name) {
		if (!strncmp(_aalg_map[i].name, x->aalg->alg_name, XFRM_ALG_NAME_MAX)) {
			IPADBG_LOW("_aalg_map[i].name = %s\n", _aalg_map[i].name);
			return _aalg_map[i].algo.a;
		}
		i++;
	}

	return IPA_IPSEC_AUTH_MAX;
}

/* Translate bit length into key length ID */
static enum ipa_ipsec_key_len _ipa_ipsec_key_len_from_b(u32 bits)
{
	IPADBG_LOW("bits = %u\n", bits);

	switch (bits) {
	case 0:
		return IPA_IPSEC_KEY_0;
	case 128:
		return IPA_IPSEC_KEY_128;
	case 160:
		return IPA_IPSEC_KEY_160;
	case 256:
		return IPA_IPSEC_KEY_256;
	case 384:
		return IPA_IPSEC_KEY_384;
	case 512:
		return IPA_IPSEC_KEY_512;
	default:
		return IPA_IPSEC_KEY_LEN_MAX;
	}
}

/* Translate key length ID into byte length */
int ipa_ipsec_key_len_to_byte(enum ipa_ipsec_key_len len)
{
	IPADBG_LOW("len = %u\n", len);

	switch (len) {
	case IPA_IPSEC_KEY_0:
		return 0;
	case IPA_IPSEC_KEY_128:
		return 16;
	case IPA_IPSEC_KEY_160:
		return 20;
	case IPA_IPSEC_KEY_256:
		return 32;
	case IPA_IPSEC_KEY_384:
		return 48;
	case IPA_IPSEC_KEY_512:
		return 64;
	default:
		return -EINVAL;
	}
}

/* Translate IPv4 pefix length into 32 bit mask */
static inline u32 ipa_ipv4_pref2mask(u8 pref)
{
	return ~((1 << (32 - pref)) - 1);
}

/* Translate IPv6 pefix length into 128 bit mask */
static inline u32 ipa_ipv6_pref2mask(u8 pref, int idx)
{
	if (idx < pref >> 5)
		return 0xFFFFFFFF;

	if (idx > pref >> 5)
		return 0x00000000;

	return ~((0x100000000 >> (pref & 0x1F)) - 1);
}

/* should be (2^n - 1)*32 Possible window size: 0, 32, 96, 224, 480, 992 */
static inline u32 xdo2ipa_replay_window_sz(u32 xdo_sz)
{
	IPADBG_LOW("xdo_sz=%d\n", xdo_sz);
	switch (xdo_sz) {
	case 0:
	case 32:
	case 96:
	case 224:
	case 480:
	case 992:
		return xdo_sz;
	default:
		return 32;
	}
}

/* Match XFRM template against an XFRM state */
static inline bool ipa_ipsec_tmpl_sa_match(const struct xfrm_tmpl *tmpl, const struct xfrm_state *x)
{
	IPADBG_LOW("tmpl->id.spi = 0x%08X  x->id.spi = 0x%08X\n",
		tmpl->id.spi, x ? x->id.spi : 0xFFFFFFFF);
	return	!!x && !!tmpl && tmpl->encap_family == x->props.family &&
		xfrm_id_proto_match(tmpl->id.proto, IPPROTO_ESP) &&
		(!tmpl->id.spi || x->id.spi == tmpl->id.spi) &&
		(xfrm_addr_any(&x->id.daddr, tmpl->encap_family) ||
		 xfrm_addr_equal(&x->id.daddr, &tmpl->id.daddr, tmpl->encap_family)) &&
		(!tmpl->reqid || x->props.reqid == tmpl->reqid) &&
		(!tmpl->mode || x->props.mode == tmpl->mode) &&
		(tmpl->allalgs ||
		 (tmpl->aalgos & (1<<x->props.aalgo)) || (tmpl->ealgos & (1<<x->props.ealgo))) &&
		!(xfrm_state_addr_cmp(tmpl, x, tmpl->encap_family));
}

/* Find offloaded SA matching an XFRM template */
static u8 ipa_ipsec_find_match_sa(const struct xfrm_tmpl *tmpl, u16 family,
					enum ipa_ipsec_sa_type type)
{
	u8 idx;

	IPADBG_LOW("Start\n");

	for (idx = 0; idx < IPA_IPSEC_MAX_SA_NUM; idx++) {
		if (ipa3_ctx->ipsec->sa_db[type][idx].x &&
		    ipa_ipsec_tmpl_sa_match(tmpl, ipa3_ctx->ipsec->sa_db[type][idx].x))
			return idx;
	}

	return IPA_IPSEC_MAX_SA_NUM;
}

/* Translate XFRM policy into IPA FnR attrib */
static void ipa_ipsec_xfrm_sp_to_ipa_attrib(
	struct xfrm_policy *xp, struct ipa_rule_attrib *attr, u8 idx)
{
	/* Internal function, assumes valid input pointers */
	struct xfrm_selector *sel = &xp->selector;
	enum ipa_ip_type ip_type = xp->selector.family == AF_INET6 ? IPA_IP_v6 : IPA_IP_v4;

	attr->attrib_mask = 0;

	if (!xfrm_addr_any(&sel->daddr, sel->family)) {
		attr->attrib_mask |= IPA_FLT_DST_ADDR;
		if (ip_type == IPA_IP_v4) {
			attr->u.v4.dst_addr = be32_to_cpu(sel->daddr.a4);
			attr->u.v4.dst_addr_mask = ipa_ipv4_pref2mask(sel->prefixlen_d);
		} else {
			attr->u.v6.dst_addr[0] = be32_to_cpu(sel->daddr.a6[0]);
			attr->u.v6.dst_addr[1] = be32_to_cpu(sel->daddr.a6[1]);
			attr->u.v6.dst_addr[2] = be32_to_cpu(sel->daddr.a6[2]);
			attr->u.v6.dst_addr[3] = be32_to_cpu(sel->daddr.a6[3]);
			attr->u.v6.dst_addr_mask[0] = ipa_ipv6_pref2mask(sel->prefixlen_d, 0);
			attr->u.v6.dst_addr_mask[1] = ipa_ipv6_pref2mask(sel->prefixlen_d, 1);
			attr->u.v6.dst_addr_mask[2] = ipa_ipv6_pref2mask(sel->prefixlen_d, 2);
			attr->u.v6.dst_addr_mask[3] = ipa_ipv6_pref2mask(sel->prefixlen_d, 3);
		}
	}

	if (!xfrm_addr_any(&sel->saddr, sel->family)) {
		attr->attrib_mask |= IPA_FLT_SRC_ADDR;
		if (ip_type == IPA_IP_v4) {
			attr->u.v4.src_addr = be32_to_cpu(sel->saddr.a4);
			attr->u.v4.src_addr_mask = ipa_ipv4_pref2mask(sel->prefixlen_s);
		} else {
			attr->u.v6.src_addr[0] = be32_to_cpu(sel->saddr.a6[0]);
			attr->u.v6.src_addr[1] = be32_to_cpu(sel->saddr.a6[1]);
			attr->u.v6.src_addr[2] = be32_to_cpu(sel->saddr.a6[2]);
			attr->u.v6.src_addr[3] = be32_to_cpu(sel->saddr.a6[3]);
			attr->u.v6.src_addr_mask[0] = ipa_ipv6_pref2mask(sel->prefixlen_s, 0);
			attr->u.v6.src_addr_mask[1] = ipa_ipv6_pref2mask(sel->prefixlen_s, 1);
			attr->u.v6.src_addr_mask[2] = ipa_ipv6_pref2mask(sel->prefixlen_s, 2);
			attr->u.v6.src_addr_mask[3] = ipa_ipv6_pref2mask(sel->prefixlen_s, 3);
		}
	}

	if (sel->proto) {
		if (ip_type == IPA_IP_v4) {
			attr->attrib_mask |= IPA_FLT_PROTOCOL;
			attr->u.v4.protocol = sel->proto;
		} else {
			attr->attrib_mask |= IPA_FLT_NEXT_HDR;
			attr->u.v6.next_hdr = sel->proto;
		}

		switch (sel->proto) {
		case IPPROTO_TCP:
		case IPPROTO_UDP:
		case IPPROTO_SCTP:
		case IPPROTO_DCCP:
			/* The following logic is in fact potentionally allowing wider port range,
			   than was configured in the IKE, but it mimics the logic in the SW path */
			IPADBG("sel->dport_mask = %X, sel->dport = %X\n",
				sel->dport_mask, sel->dport);
			IPADBG("sel->dport_mask = %X, sel->dport = %X\n",
				sel->dport_mask, sel->dport);
			if (be16_to_cpu(sel->dport_mask) == 0xFFFF) {
				attr->attrib_mask |= IPA_FLT_DST_PORT;
				attr->dst_port = be16_to_cpu(sel->dport);
			} else if (be16_to_cpu(sel->dport_mask) != 0x0) {
				attr->attrib_mask |= IPA_FLT_DST_PORT_RANGE;
				attr->dst_port_lo = be16_to_cpu(sel->dport & sel->dport_mask);
				attr->dst_port_hi = be16_to_cpu(sel->dport | ~sel->dport_mask);
			}
			if (be16_to_cpu(sel->sport_mask) == 0xFFFF) {
				attr->attrib_mask |= IPA_FLT_SRC_PORT;
				attr->src_port = be16_to_cpu(sel->sport);
			} else if (be16_to_cpu(sel->sport_mask) != 0x0) {
				attr->attrib_mask |= IPA_FLT_SRC_PORT_RANGE;
				attr->src_port_lo = be16_to_cpu(sel->sport & sel->sport_mask);
				attr->src_port_hi = be16_to_cpu(sel->sport | ~sel->sport_mask);
			}
			break;
		case IPPROTO_ICMP:
		default:
			break;
		}
	}

	if (idx < IPA_IPSEC_MAX_SA_NUM && xp->xdo.dir == XFRM_DEV_OFFLOAD_IN) {
		attr->attrib_mask |= IPA_FLT_META_DATA;
		attr->meta_data_mask = (META_IS_IPSEC | META_SA_MASK) << META_SA_SHIFT;
		attr->meta_data = (META_IS_IPSEC | (idx & META_SA_MASK)) << META_SA_SHIFT;
	}
}

/*
 * ipa_ipsec_install_key()
 * 	Install a key.
 */
static int ipa_ipsec_install_key(enum ipa_ipsec_sa_type dir, u8 idx, enum ipa_ipsec_key_type type,
	enum ipa_ipsec_key_len len_code, void *key)
{
	size_t len = ipa_ipsec_key_len_to_byte(len_code);
	u8 *to;

	idx += (dir == IPA_IPSEC_DECAP) ? IPA_IPSEC_MAX_ENACAP_KEY_NUM : 0;
	to = (type == IPA_IPSEC_KEY_ENC) ?
		(u8 *)&ipa3_ctx->ipsec->keys->enc[idx] : (u8 *)&ipa3_ctx->ipsec->keys->auth[idx];

	if (unlikely(!to)) {
		IPAERR("Key destination is null\n");
		return -EFAULT;
	}

	if (!len)
		return 0;

	if (type == IPA_IPSEC_KEY_ENC &&
		!(len_code == IPA_IPSEC_KEY_128 || len_code == IPA_IPSEC_KEY_256)) {
		IPAERR("Unsupported key length.\n");
		return -EINVAL;
	}

	while (len) {
		__raw_writeb(*((u8 *)key + len - 1), to);
		to++;
		len--;
	}

	return 0;
}

/*
 * ipa_ipsec_delete_key()
 * 	Delete a key.
 */
static void ipa_ipsec_delete_key(enum ipa_ipsec_sa_type dir, u8 idx, enum ipa_ipsec_key_type type)
{
	idx += (dir == IPA_IPSEC_DECAP) ? IPA_IPSEC_MAX_ENACAP_KEY_NUM : 0;

	switch (type) {
	case IPA_IPSEC_KEY_ENC:
		memset_io((void __iomem *)&ipa3_ctx->ipsec->keys->enc[idx], 0, 32);
		break;
	case IPA_IPSEC_KEY_AUTH:
		memset_io((void __iomem *)&ipa3_ctx->ipsec->keys->auth[idx], 0, 64);
		break;
	default:
		break;
	}
}

/*
 * ipa_ipsec_install_encap_hpc() - construct and install header template and HPC for encap SA
 * We assume this function to be called on fresh filled 'static struct ipa_ipsec_sa_encap esa'.
 * @x:   [in] XFRM state pointer
 * @idx: [in] HW encap SA index
 */
static int ipa_ipsec_install_encap_hpc(const struct xfrm_state *x, u8 idx, enum ipa_ip_type inner_iptype)
{
	int i, ret = 0;
	struct ipa_ioc_add_hdr *hdrs;
	struct ipa_hdr_add *hdr_add;
	struct ipa_ioc_add_hdr_proc_ctx *proc_ctxs = NULL;
	struct ipa_hdr_proc_ctx_add *proc_ctx_add;
	u8 *hdr;
	struct iphdr *iph;
	struct ipv6hdr *ipv6h;
	struct udphdr *udph;
	struct ip_esp_hdr *esph;

	IPADBG("Start\n");

	hdrs = (struct ipa_ioc_add_hdr *)kzalloc(
		sizeof(struct ipa_ioc_add_hdr) + sizeof(struct ipa_hdr_add), GFP_KERNEL);
	if (!hdrs) {
		IPAERR("Failed to allocate ipa_ioc_add_hdr\n");
		return -ENOMEM;
	}

	hdrs->num_hdrs = 1;
	hdr_add = &hdrs->hdr[0];

	scnprintf(hdr_add->name, sizeof(hdr_add->name), "%s%02u", __ipa_ipsec_s.encap_hdr, idx);
	hdr_add->hdr_len = 0;
	hdr = hdr_add->hdr;

	switch (x->props.family) {
	case AF_INET:
		iph = (struct iphdr *)hdr;
		iph->ihl = 5;
		iph->version = 4;
		iph->protocol = !!x->encap ? IPPROTO_UDP : IPPROTO_ESP;
		iph->tos = 0;
		iph->frag_off = (x->props.flags & XFRM_STATE_NOPMTUDISC);
		iph->ttl = 64;
		iph->saddr = x->props.saddr.a4;
		iph->daddr = x->id.daddr.a4;
		iph->id = 0;
		hdr += sizeof(struct iphdr);
		hdr_add->hdr_len += sizeof(struct iphdr);
		break;
	case AF_INET6:
		ipv6h = (struct ipv6hdr *)hdr;
		ipv6h->version = 6;
		ipv6h->nexthdr = !!x->encap ? IPPROTO_UDP : IPPROTO_ESP;
		ipv6h->hop_limit = 255;
		ipv6h->saddr = *(struct in6_addr *)&x->props.saddr;
		ipv6h->daddr = *(struct in6_addr *)&x->id.daddr;
		hdr += sizeof(struct ipv6hdr);
		hdr_add->hdr_len += sizeof(struct ipv6hdr);
		break;
	default:
		IPAERR("Unsupported AF\n");
		ret = -EINVAL;
		goto end;
	}

	/* For NAT-T add UDP header */
	if (!!x->encap) {
		udph = (struct udphdr *)hdr;
		udph->source = x->encap->encap_sport;
		udph->dest = x->encap->encap_dport;
		hdr += sizeof(struct udphdr);
		hdr_add->hdr_len += sizeof(struct udphdr);
	}

	/* Add ESP header */
	esph = (struct ip_esp_hdr *)hdr;
	esph->spi = x->id.spi;
	esph->seq_no = 0;
	hdr_add->hdr_len += sizeof(esph->spi) + sizeof(esph->seq_no);
	hdr = esph->enc_data;

	/* Add random IV */
	if (esa.shar.iv_sz) {
		get_random_bytes(hdr, esa.shar.iv_sz);
		hdr_add->hdr_len += esa.shar.iv_sz;
		hdr += esa.shar.iv_sz;
	}

	/* Fill padding pattern, Pad Len and Next Header */
	for (i = 0; i < ESP_PAD_LEN; i++)
		hdr[i] = i + 1;
	hdr += ESP_PAD_LEN;
	hdr[0] = ESP_PAD_LEN;
	hdr[1] = inner_iptype == IPA_IP_v6 ? IPPROTO_IPV6 : IPPROTO_IPIP;
	hdr_add->hdr_len += ESP_PAD_LEN + 2;

	/* Set ICV length */
	hdr_add->hdr_len += esa.shar.icv_sz;

	ret = ipa3_add_hdr_usr(hdrs, false);
	if (!!ret) {
		IPAERR("ipa3_add_hdr_usr returned %d\n", ret);
		goto end;
	}

	proc_ctxs = (struct ipa_ioc_add_hdr_proc_ctx *)kzalloc(
		sizeof(struct ipa_ioc_add_hdr_proc_ctx) + sizeof(struct ipa_hdr_proc_ctx_add),
		GFP_KERNEL);
	if (!proc_ctxs) {
		IPAERR("Failed to allocate ipa_ioc_add_hdr_proc_ctx\n");
		ret = -ENOMEM;
		goto end;
	}

	proc_ctxs->commit = 1; /* this will commit the header as well */
	proc_ctxs->num_proc_ctxs = 1;
	proc_ctx_add = &proc_ctxs->proc_ctx[0];
	proc_ctx_add->type = IPA_HDR_PROC_IPSEC_ENCAP_NXT_RND;
	proc_ctx_add->hdr_hdl = hdr_add->hdr_hdl;
	proc_ctx_add->ipsec_params.action = IPA_IPSEC_HPC_ENCAP;
	proc_ctx_add->ipsec_params.sa_idx = idx;
	proc_ctx_add->ipsec_params.flt_tbl_id = IPA_CLIENT_APPS_WAN_PROD;
	proc_ctx_add->ipsec_params.pre_params.encap.input_ip_version = inner_iptype;
	proc_ctx_add->ipsec_params.pre_params.encap.output_ip_version =
		x->props.family == AF_INET6 ? IPA_IP_v6 : IPA_IP_v4;
	proc_ctx_add->ipsec_params.pre_params.encap.retain_l2_header = 0;


	ret = ipa3_add_hdr_proc_ctx(proc_ctxs, false);
	if (!!ret) {
		IPAERR("ipa3_add_hdr_proc_ctx returned %d\n", ret);
		goto end;
	}

	ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hdr = hdr_add->hdr_hdl;
	ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc = proc_ctx_add->proc_ctx_hdl;

	IPADBG_LOW("hdr_add->hdr_hdl = %d\n", hdr_add->hdr_hdl);
	IPADBG_LOW("proc_ctx_add->proc_ctx_hdl = %d\n", proc_ctx_add->proc_ctx_hdl);
end:
	kfree(proc_ctxs);
	kfree(hdrs);
	return ret;
}

/*
 * ipa_ipsec_install_decap_hpc() - construct and install header template and HPC for decap SA
 * @x:   [in] XFRM state pointer
 * @idx: [in] HW decap SA index
 */
static int ipa_ipsec_install_decap_hpc(const struct xfrm_state *x, u8 idx)
{
	int ret = 0;
	enum ipa_ip_type ip_type;
	struct ipa_ioc_add_hdr_proc_ctx *proc_ctxs;
	struct ipa_hdr_proc_ctx_add *proc_ctx_add;

	IPADBG("Start\n");

	proc_ctxs = (struct ipa_ioc_add_hdr_proc_ctx *)kzalloc(
		sizeof(struct ipa_ioc_add_hdr_proc_ctx) + sizeof(struct ipa_hdr_proc_ctx_add),
		GFP_KERNEL);
	if (!proc_ctxs) {
		IPAERR("Failed to allocate ipa_ioc_add_hdr_proc_ctx\n");
		return -ENOMEM;
	}

	ip_type = x->props.family == AF_INET6 ? IPA_IP_v6 : IPA_IP_v4;

	proc_ctxs->commit = 1;
	proc_ctxs->num_proc_ctxs = 1;
	proc_ctx_add = &proc_ctxs->proc_ctx[0];

	proc_ctx_add->type = IPA_HDR_PROC_IPSEC_DECAP_NXT_RND;
	proc_ctx_add->ipsec_params.action = IPA_IPSEC_HPC_DECAP;
	proc_ctx_add->ipsec_params.sa_idx = idx;
	proc_ctx_add->ipsec_params.flt_tbl_id = ipa3_ctx->ipsec->dl_pol_flt[ip_type];
	proc_ctx_add->ipsec_params.pre_params.decap.input_ip_version = ip_type;
	proc_ctx_add->ipsec_params.pre_params.decap.retain_l2_header = 0;

	ret = ipa3_add_hdr_proc_ctx(proc_ctxs, false);
	if (!!ret) {
		IPAERR("ipa3_add_hdr_proc_ctx returned %d\n", ret);
		goto end;
	}

	ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].hpc = proc_ctx_add->proc_ctx_hdl;

	IPADBG_LOW("proc_ctx_add->proc_ctx_hdl = %d\n", proc_ctx_add->proc_ctx_hdl);
end:
	kfree(proc_ctxs);
	return ret;
}

/*
 * ipa_ipsec_install_decap_rt() - construct and install RT rule for decap SA
 * @x:   [in] XFRM state pointer
 * @idx: [in] HW decap SA index
 */
static int ipa_ipsec_install_decap_rt(const struct xfrm_state *x, u8 idx)
{
	int ret = 0;
	struct ipa_ioc_add_rt_rule_v2 *rt_tbl = NULL;
	struct ipa_rt_rule_add_v2 *rt_rule = NULL;
	enum ipa_ip_type ip_type;

	IPADBG("Start\n");

	if (ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].hpc == 0) {
		IPAERR("The HPC is not installed\n");
		return -EFAULT;
	}

	ip_type = x->props.family == AF_INET6 ? IPA_IP_v6 : IPA_IP_v4;

	rt_tbl = (struct ipa_ioc_add_rt_rule_v2 *)kzalloc(
		sizeof(struct ipa_ioc_add_rt_rule_v2), GFP_KERNEL);
	if (!rt_tbl) {
		IPAERR("Failed to allocate ipa_ioc_add_rt_rule_v2\n");
		return -ENOMEM;
	}
	rt_tbl->rules = (uint64_t)kzalloc(sizeof(struct ipa_rt_rule_add_v2), GFP_KERNEL);
	if (!rt_tbl->rules) {
		IPAERR("Failed to allocate ipa_rt_rule_add_v2\n");
		kfree(rt_tbl);
		return -ENOMEM;
	}

	rt_tbl->commit = 1;
	rt_tbl->num_rules = 1;
	rt_tbl->ip = ip_type;
	rt_tbl->rule_add_size = sizeof(struct ipa_rt_rule_add_v2);
	strlcpy(rt_tbl->rt_tbl_name, __ipa_ipsec_s.decap_rt[ip_type], IPA_RESOURCE_NAME_MAX);

	rt_rule = &(((struct ipa_rt_rule_add_v2 *)rt_tbl->rules)[0]);
	rt_rule->at_rear = 0;
	rt_rule->rt_rule_hdl = -1;
	rt_rule->status = -1;
	rt_rule->rule.dst = IPA_CLIENT_MAX; /* translated into IPA_INVALID_PIPE_IDX */
	rt_rule->rule.hashable = true;
	rt_rule->rule.hdr_proc_ctx_hdl = ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].hpc;
	rt_rule->rule.attrib.attrib_mask = IPA_FLT_SPI;
	rt_rule->rule.attrib.spi = be32_to_cpu(x->id.spi);

	if (x->encap) {
		rt_rule->rule.esp_after_udp = 1;
		rt_rule->rule.attrib.ext_attrib_mask = IPA_FLT_EXT_NAT_T;
		if (ip_type == IPA_IP_v4) {
			rt_rule->rule.attrib.attrib_mask |=
				IPA_FLT_PROTOCOL | IPA_FLT_SRC_PORT;
			rt_rule->rule.attrib.u.v4.protocol = IPPROTO_UDP;
		} else {
			rt_rule->rule.attrib.attrib_mask |=
				IPA_FLT_NEXT_HDR | IPA_FLT_SRC_PORT;
			rt_rule->rule.attrib.u.v6.next_hdr = IPPROTO_UDP;
		}
		rt_rule->rule.attrib.src_port = be16_to_cpu(x->encap->encap_sport);
	} else {
		if (ip_type == IPA_IP_v4) {
			rt_rule->rule.attrib.attrib_mask |= IPA_FLT_PROTOCOL;
			rt_rule->rule.attrib.u.v4.protocol = IPPROTO_ESP;
		} else {
			rt_rule->rule.attrib.attrib_mask |= IPA_FLT_NEXT_HDR;
			rt_rule->rule.attrib.u.v6.next_hdr = IPPROTO_ESP;
		}
	}

	if (!xfrm_addr_any(&x->id.daddr, x->props.family)) {
		rt_rule->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		if (ip_type == IPA_IP_v4) {
			rt_rule->rule.attrib.u.v4.dst_addr = be32_to_cpu(x->id.daddr.a4);
			rt_rule->rule.attrib.u.v4.dst_addr_mask = ipa_ipv4_pref2mask(32);
		} else {
			rt_rule->rule.attrib.u.v6.dst_addr[0] = be32_to_cpu(x->id.daddr.a6[0]);
			rt_rule->rule.attrib.u.v6.dst_addr[1] = be32_to_cpu(x->id.daddr.a6[1]);
			rt_rule->rule.attrib.u.v6.dst_addr[2] = be32_to_cpu(x->id.daddr.a6[2]);
			rt_rule->rule.attrib.u.v6.dst_addr[3] = be32_to_cpu(x->id.daddr.a6[3]);
			rt_rule->rule.attrib.u.v6.dst_addr_mask[0] = ipa_ipv6_pref2mask(128, 0);
			rt_rule->rule.attrib.u.v6.dst_addr_mask[1] = ipa_ipv6_pref2mask(128, 1);
			rt_rule->rule.attrib.u.v6.dst_addr_mask[2] = ipa_ipv6_pref2mask(128, 2);
			rt_rule->rule.attrib.u.v6.dst_addr_mask[3] = ipa_ipv6_pref2mask(128, 3);
		}
	}

	if (!xfrm_addr_any(&x->props.saddr, x->props.family)) {
		rt_rule->rule.attrib.attrib_mask |= IPA_FLT_SRC_ADDR;
		if (ip_type == IPA_IP_v4) {
			rt_rule->rule.attrib.u.v4.src_addr = be32_to_cpu(x->props.saddr.a4);
			rt_rule->rule.attrib.u.v4.src_addr_mask = ipa_ipv4_pref2mask(32);
		} else {
			rt_rule->rule.attrib.u.v6.src_addr[0] = be32_to_cpu(x->props.saddr.a6[0]);
			rt_rule->rule.attrib.u.v6.src_addr[1] = be32_to_cpu(x->props.saddr.a6[1]);
			rt_rule->rule.attrib.u.v6.src_addr[2] = be32_to_cpu(x->props.saddr.a6[2]);
			rt_rule->rule.attrib.u.v6.src_addr[3] = be32_to_cpu(x->props.saddr.a6[3]);
			rt_rule->rule.attrib.u.v6.src_addr_mask[0] = ipa_ipv6_pref2mask(128, 0);
			rt_rule->rule.attrib.u.v6.src_addr_mask[1] = ipa_ipv6_pref2mask(128, 1);
			rt_rule->rule.attrib.u.v6.src_addr_mask[2] = ipa_ipv6_pref2mask(128, 2);
			rt_rule->rule.attrib.u.v6.src_addr_mask[3] = ipa_ipv6_pref2mask(128, 3);
		}
	}

	ret = ipa3_add_rt_rule_v2(rt_tbl);
	if (!!ret)
		IPAERR("ipa3_add_rt_rule_v2 returned %d\n", ret);
	else
		ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].rt = rt_rule->rt_rule_hdl;

	kfree((void *)rt_tbl->rules);
	kfree(rt_tbl);

	IPADBG_LOW("ret = %d\n", ret);
	return ret;
}

/*
 * ipa_ipsec_install_encap_rt() - construct and install RT rule for encap policy
 * @xp:  [in] XFRM policy pointer
 * @idx: [in] HW decap SA index
 */
static int ipa_ipsec_install_encap_rt(struct xfrm_policy *xp, u8 idx)
{
	int ret = 0;
	struct ipa_ioc_add_rt_rule_v2 *rt_tbl = NULL;
	struct ipa_rt_rule_add_v2 *rt_rule = NULL;
	enum ipa_ip_type ip_type;

	IPADBG("Start\n");

	if (ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc == 0) {
		IPAERR("The HPC is not installed\n");
		return -EFAULT;
	}

	ip_type = xp->selector.family == AF_INET6 ? IPA_IP_v6 : IPA_IP_v4;

	rt_tbl = (struct ipa_ioc_add_rt_rule_v2 *)kzalloc(
		sizeof(struct ipa_ioc_add_rt_rule_v2), GFP_KERNEL);
	if (!rt_tbl) {
		IPAERR("Failed to allocate ipa_ioc_add_rt_rule_v2\n");
		return -ENOMEM;
	}
	rt_tbl->rules = (uint64_t)kzalloc(sizeof(struct ipa_rt_rule_add_v2), GFP_KERNEL);
	if (!rt_tbl->rules) {
		IPAERR("Failed to allocate ipa_rt_rule_add_v2\n");
		kfree(rt_tbl);
		return -ENOMEM;
	}

	rt_tbl->commit = 1;
	rt_tbl->num_rules = 1;
	rt_tbl->ip = ip_type;
	rt_tbl->rule_add_size = sizeof(struct ipa_rt_rule_add_v2);
	strlcpy(rt_tbl->rt_tbl_name, __ipa_ipsec_s.encap_rt[ip_type], IPA_RESOURCE_NAME_MAX);

	rt_rule = &(((struct ipa_rt_rule_add_v2 *)rt_tbl->rules)[0]);
	rt_rule->at_rear = 0;
	rt_rule->rt_rule_hdl = -1;
	rt_rule->status = -1;
	rt_rule->rule.dst = IPA_CLIENT_MAX; /* translated into IPA_INVALID_PIPE_IDX */
	rt_rule->rule.hashable = true;
	rt_rule->rule.hdr_proc_ctx_hdl = ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc;

	ipa_ipsec_xfrm_sp_to_ipa_attrib(xp, &rt_rule->rule.attrib, IPA_IPSEC_MAX_SA_NUM);

	ret = ipa3_add_rt_rule_v2(rt_tbl);
	if (!!ret)
		IPAERR("ipa3_add_rt_rule_v2 returned %d\n", ret);
	else
		ret = rt_rule->rt_rule_hdl;

	kfree((void *)rt_tbl->rules);
	kfree(rt_tbl);

	IPADBG_LOW("ret = %d\n", ret);
	return ret;
}

/*
 * ipa_ipsec_install_decap_flt() - construct and install FLT rule for decap policy
 * @xp:  [in] XFRM policy pointer
 */
static int ipa_ipsec_install_decap_flt(struct xfrm_policy *xp, u8 idx)
{
	int ret = 0;
	struct ipa_flt_rule_add_v2 *flt_rule = NULL;
	struct ipa_ioc_add_flt_rule_v2 *flt_tbl = NULL;
	struct ipa_ioc_get_rt_tbl rt_lookup;
	enum ipa_ip_type ip_type;

	IPADBG("Start\n");

	ip_type = xp->selector.family == AF_INET6 ? IPA_IP_v6 : IPA_IP_v4;

	flt_tbl = kzalloc(sizeof(*flt_tbl), GFP_KERNEL);
	if (!flt_tbl) {
		IPAERR("Failed to allocate ipa_ioc_add_flt_rule_v2\n");
		return -ENOMEM;
	}
	flt_tbl->rules = (uint64_t)kzalloc(sizeof(struct ipa_flt_rule_add_v2), GFP_KERNEL);
	if (!flt_tbl->rules) {
		IPAERR("Failed to allocate ipa_flt_rule_add_v2\n");
		kfree(flt_tbl);
		return -ENOMEM;
	}

	flt_tbl->commit = 1;
	flt_tbl->ep = IPA_IPSEC_DL_POL_FLT_ID;
	flt_tbl->num_rules = 1;
	flt_tbl->ip = ip_type;
	flt_tbl->flt_rule_size = sizeof(struct ipa_flt_rule_add_v2);

	flt_rule = &(((struct ipa_flt_rule_add_v2 *)flt_tbl->rules)[0]);
	flt_rule->flt_rule_hdl = -1;
	flt_rule->status = -1;
	flt_rule->rule.hashable = true;
	/* NAT topo: Set action pass to NAT, change it to routing if F03 action is routing */
	/* No NAT topo: always to routing */
	flt_rule->rule.action = IPA_PASS_TO_ROUTING;

	rt_lookup.ip = ip_type;
	strlcpy(rt_lookup.name, __ipa_ipsec_s.dl_rt[ip_type], IPA_RESOURCE_NAME_MAX);
	ret = ipa3_get_rt_tbl(&rt_lookup);
	if (unlikely(!!ret)) {
		IPADBG("%s is not installed redirecting to the default route\n",
			__ipa_ipsec_s.dl_rt[ip_type]);

		strlcpy(rt_lookup.name, __ipa_ipsec_s.default_rt, IPA_RESOURCE_NAME_MAX);
		ret = ipa3_get_rt_tbl(&rt_lookup);
		if (unlikely(!!ret)) {
			IPAERR("%s is not installed!\n",
				__ipa_ipsec_s.dl_rt[ip_type]);
			goto end;
		}
		flt_rule->rule.action = IPA_PASS_TO_ROUTING;
	}
	IPADBG_LOW("ret = %d rt_lookup.name = %s rt_lookup.hdl = %d\n",
		ret, rt_lookup.name, rt_lookup.hdl);
	flt_rule->rule.rt_tbl_hdl = rt_lookup.hdl;

	if (ipa_put_rt_tbl(rt_lookup.hdl)) {
		IPAERR("ipa_put_rt_tbl() failure.\n");
	}

	ipa_ipsec_xfrm_sp_to_ipa_attrib(xp, &flt_rule->rule.attrib, idx);

	ret = ipa3_add_flt_rule_usr_v2(flt_tbl, false);
	if (!!ret) {
		IPAERR("ipa3_add_flt_rule_usr_v2 returned %d\n", ret);
		goto end;
	}

	ret = flt_rule->flt_rule_hdl;

end:
	kfree((void *)flt_tbl->rules);
	kfree(flt_tbl);

	return ret;
}

/* Install FnR for UL policies installed before a matching state */
static int ipa_ipsec_install_cached_ul_pols(u8 idx)
{
	int rc = 0;
	int rt = -1;
	struct ipa_ipsec_policy *pol;
	struct ipa_ioc_ipsec_ul_flt_attr *ul_flt, *ul_flt_del = NULL;

	IPADBG("Start\n");

	mutex_lock(&ipa3_ctx->ipsec->pol_list_lock);
	list_for_each_entry(pol, &ipa3_ctx->ipsec->pol_list, l) {
		if (ipa_ipsec_tmpl_sa_match(pol->xp->xfrm_vec,
			ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].x)) {

			ul_flt = (struct ipa_ioc_ipsec_ul_flt_attr *)kzalloc(
				sizeof(struct ipa_ioc_ipsec_ul_flt_attr), GFP_KERNEL);
			if (unlikely(!ul_flt)) {
				IPAERR("Failed allocating ipa_ioc_ipsec_ul_flt_attr\n");
				rc = -EFAULT;
				goto end;
			}
			ul_flt->ip = pol->xp->selector.family == AF_INET6 ? IPA_IP_v6 : IPA_IP_v4;
			ipa_ipsec_xfrm_sp_to_ipa_attrib(pol->xp, &ul_flt->attr, IPA_IPSEC_MAX_SA_NUM);

			/* If the policy is already mapped to an SA, we should cleanup first */
			if (pol->rt != -1) {
				mutex_lock(&ipa3_ctx->lock);
				rc = __ipa3_del_rt_rule(pol->rt);
				mutex_unlock(&ipa3_ctx->lock);

				if (rc) {
					IPAERR("Failed deleting RT hdl %d.\n", pol->rt);
					rc = -EFAULT;
					goto end;
				}
				IPADBG("Deleted RT rule %d\n", pol->rt);

				pol->rt = -1;

				/* Duplicate the message struct,
				   because it will be freed by the send routine */
				ul_flt_del = kmemdup(ul_flt, sizeof(*ul_flt), GFP_KERNEL);
				if (unlikely(!ul_flt_del)) {
					IPAERR("Failed allocating ipa_ioc_ipsec_ul_flt_attr\n");
					rc = -EFAULT;
					goto end;
				}
				rc = ipa3_send_ipsec_ul_flt(IPA_IPSEC_UL_FLT_DEL_EVENT, ul_flt_del);
				if (rc != 0) {
					IPAERR("Failed sending IPA_IPSEC_UL_FLT_DEL_EVENT\n");
					goto end;
				}
			}

			/* Construct and install header template and HPC */
			if (ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc == 0) {
				rc = ipa_ipsec_install_encap_hpc(
					ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].x,
					idx, ul_flt->ip);
				if (rc < 0) {
					IPAERR("ipa_ipsec_install_encap_hpc returned %d\n", rc);
					kfree(ul_flt);
					goto end;
				}
			}

			/* Install RT rule */
			rt = ipa_ipsec_install_encap_rt(pol->xp, idx);
			if (rt < 0) {
				mutex_lock(&ipa3_ctx->lock);
				IPAERR("ipa_ipsec_install_encap_rt returned %d\n", rt);
				__ipa3_release_hdr_proc_ctx(ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc);
				ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc = 0;
				__ipa3_release_hdr(ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hdr);
				ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hdr = 0;
				mutex_unlock(&ipa3_ctx->lock);
				kfree(ul_flt);
				rc = rt;
				goto end;
			}
			pol->rt = rt;

			IPADBG("pol->rt = %d\n", pol->rt);

			if (ipa3_send_ipsec_ul_flt(IPA_IPSEC_UL_FLT_ADD_EVENT, ul_flt) != 0) {
				rc = -EFAULT;
				goto end;
                        }
		}
	}

end:
	mutex_unlock(&ipa3_ctx->ipsec->pol_list_lock);
	return rc;
}

/* Install FnR for DL policies installed before a matching state */
static int ipa_ipsec_install_cached_dl_pols(u8 idx)
{
	int ret = 0, i = 0;
	u32 flt;
	struct ipa_ipsec_policy *pol;

	IPADBG("Start\n");

	mutex_lock(&ipa3_ctx->ipsec->pol_list_lock);
	list_for_each_entry(pol, &ipa3_ctx->ipsec->pol_list, l) {
		if (ipa_ipsec_tmpl_sa_match(pol->xp->xfrm_vec,
			ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].x)) {

			while (i < IPA_IPSEC_DL_FLT_PER_POL) {
				if (pol->flt[i] == -1)
					break;
				i++;
			}

			if (i == IPA_IPSEC_DL_FLT_PER_POL) {
				IPAERR("The policy already has %d rules.\n", IPA_IPSEC_DL_FLT_PER_POL);
				ret = -EFAULT;
				goto end;
			}

			flt = ipa_ipsec_install_decap_flt(pol->xp, idx);
			if (!flt) {
				ret = -EFAULT;
				goto end;
			}

			IPADBG("pol->flt[%d] = %d\n", i, pol->flt[i]);
			pol->flt[i] = flt;
		}
	}
end:
	mutex_unlock(&ipa3_ctx->ipsec->pol_list_lock);
	return ret;
}

/* Delete FnR for UL policies after destroying a matching state */
static int ipa_ipsec_delete_orphan_ul_fnr(u8 idx)
{
	int rc = 0;
	struct ipa_ipsec_policy *pol;
	struct ipa_ioc_ipsec_ul_flt_attr *ul_flt;

	IPADBG("Start\n");

	/* More than one policy may be mapped to the SA,
	   we will delete FnR for each mapped policy */
	mutex_lock(&ipa3_ctx->ipsec->pol_list_lock);
	list_for_each_entry(pol, &ipa3_ctx->ipsec->pol_list, l) {
		if (ipa_ipsec_tmpl_sa_match(pol->xp->xfrm_vec,
			ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].x)) {

			ul_flt = (struct ipa_ioc_ipsec_ul_flt_attr *)kzalloc(
				sizeof(struct ipa_ioc_ipsec_ul_flt_attr), GFP_KERNEL);
			if (!ul_flt) {
				IPAERR("Failed to allocate ipa_ioc_ipsec_ul_flt_attr\n");
				rc = -EFAULT;
				goto end;
			}
			ul_flt->ip = pol->xp->selector.family == AF_INET6 ? IPA_IP_v6 : IPA_IP_v4;
			ipa_ipsec_xfrm_sp_to_ipa_attrib(pol->xp, &ul_flt->attr, IPA_IPSEC_MAX_SA_NUM);

			/* RT rule may be previously deleted by e.g. table deletion */
			if (pol->rt != -1) {
				/* Delete RT rule */
				mutex_lock(&ipa3_ctx->lock);
				rc = __ipa3_del_rt_rule(pol->rt);
				mutex_unlock(&ipa3_ctx->lock);
				if (rc) {
					IPAERR("Failed deleting RT hdl %d.\n", pol->rt);
					rc = -EFAULT;
				}
				if (ipa3_commit_rt(ul_flt->ip)) {
					IPAERR("Failed commiting RT table.\n");
					rc = -EFAULT;
				}
				IPADBG("deleted RT rule %d\n", pol->rt);
				pol->rt = -1;
			}

			/* Send FLT deletion event to IPACM */
			if (ipa3_send_ipsec_ul_flt(IPA_IPSEC_UL_FLT_DEL_EVENT, ul_flt) != 0) {
				rc = -EFAULT;
				goto end;
			}
		}
	}

end:
	mutex_unlock(&ipa3_ctx->ipsec->pol_list_lock);
	return rc;
}

/* Delete FnR for DL policies after destroying a matching state */
static void ipa_ipsec_delete_orphan_dl_fnr(enum ipa_ip_type ip, u8 idx)
{
	int i;
	struct ipa_ipsec_policy *pol;
	struct ipa3_flt_entry *flt_rule;

	IPADBG("Start\n");

	/* Find and delete all DL policy filtering rules that check this SA metadata */
	mutex_lock(&ipa3_ctx->ipsec->pol_list_lock);
	list_for_each_entry(pol, &ipa3_ctx->ipsec->pol_list, l) {
		if (pol->xp->xdo.dir == XFRM_DEV_OFFLOAD_IN) {
			for (i = 0; i < IPA_IPSEC_DL_FLT_PER_POL; i++) {

				if (pol->flt[i] == -1)
					continue;

				flt_rule = ipa3_id_find(pol->flt[i]);
				if (!flt_rule) {
					pol->flt[i] = -1;
					continue;
				}

				if ((flt_rule->rule.attrib.meta_data & META_SA_MASK) == idx) {
					mutex_lock(&ipa3_ctx->lock);
					WARN_ON(__ipa_del_flt_rule(pol->flt[i]));
					IPADBG("deleted FLT rule %d\n",pol->flt[i]);
					mutex_unlock(&ipa3_ctx->lock);
					WARN_ON(ipa3_commit_flt(ip));
					pol->flt[i] = -1;
				}
			}
		}
	}
	mutex_unlock(&ipa3_ctx->ipsec->pol_list_lock);
}

int ipa_ipsec_xdo_state_add(struct xfrm_state *x, struct netlink_ext_ack *extack)
{
	int ret = 0;
	u8 idx = IPA_IPSEC_MAX_SA_NUM;
	u8 ealg, aalg, eklen, aklen, ivlen, icvlen;
	char *ekey, *akey;
	u32 *salt;

	IPADBG("Start\n");

	if (!ipa_ipsec_enabled()) {
		IPAERR("IPsec is not enabled.\n");
		return -EFAULT;
	}

	if (!x || (x->props.family != AF_INET && x->props.family != AF_INET6)) {
		IPADBG("Null state or invalid AF\n");
		return -EINVAL;
	}

	if (!ipa3_ctx->ipsec) {
		IPAERR("IPsec offload is not initialized\n");
		return -EFAULT;
	}

	if (x->props.mode != XFRM_MODE_TUNNEL) {
		IPAERR("Unsupported (not tunnel) mode: %d\n", x->props.mode);
		return -EINVAL;
	}

	if (x->tfcpad) {
		IPAERR("TFC padding is not supported\n");
		return -EINVAL;
	}

	/* Special state without algo information is being sent in the beginning of the trap.
	   We need to return success for it otherwize the whole trap sequence will fail. */
	if (!x->aead && !x->ealg && !x->aalg) {
		x->xso.offload_handle =
			(unsigned long)IPA_IPSEC_MAX_SA_NUM | IPA_IPSEC_OFFLOAD_MAGIC;
		return 0;
	}

	switch (ealg = _ipa_ipsec_xfrm_sa_enc_get(x)) {
	case IPA_IPSEC_ENC_NULL:
	case IPA_IPSEC_ENC_AES_CBC:
		if (!x->ealg || !x->aalg) {
			IPAERR("%s algo is NULL\n", (!x->ealg) ? "ealg" : "aalg");
			return -EINVAL;
		}
		aalg = _ipa_ipsec_xfrm_sa_auth_get(x);
		if (aalg <= IPA_IPSEC_AUTH_NONE || aalg >= IPA_IPSEC_AUTH_MAX) {
			IPAERR("Unsupported authentication algo\n");
			return -EINVAL;
		}
		eklen = _ipa_ipsec_key_len_from_b(x->ealg->alg_key_len);
		ekey = x->ealg->alg_key;
		ivlen = (ealg == IPA_IPSEC_ENC_AES_CBC) ? 16 : 0;
		aklen = _ipa_ipsec_key_len_from_b(x->aalg->alg_key_len);
		akey = x->aalg->alg_key;
		icvlen = x->aalg->alg_trunc_len / BITS_PER_BYTE;
		break;
	case IPA_IPSEC_ENC_AES_GCM_16:
		if (!x->aead) {
			IPAERR("aead algo is NULL\n");
			return -EINVAL;
		}
		icvlen = x->aead->alg_icv_len / BITS_PER_BYTE;
		if (icvlen != 16) {
			IPAERR("Only 16 byte ICV is supported in AES GCM\n");
			return -EINVAL;
		}
		eklen = _ipa_ipsec_key_len_from_b(x->aead->alg_key_len - 32); /* 4 bytes salt */
		ekey = x->aead->alg_key;
		salt = (u32 *)(x->aead->alg_key +
			       ALIGN(x->aead->alg_key_len, BITS_PER_BYTE) / BITS_PER_BYTE - 4);
		ivlen = 8;
		aalg = 0;
		aklen = 0;
		akey = NULL;
		break;
	case IPA_IPSEC_ENC_MAX:
	default:
		IPAERR("Unsupported encryption algo\n");
		if (x->aead) IPAERR("x->aead->alg_name = %s\n", x->aead->alg_name);
		if (x->ealg) IPAERR("x->ealg->alg_name = %s\n", x->ealg->alg_name);
		if (x->aalg) IPAERR("x->aalg->alg_name = %s\n", x->aalg->alg_name);
		return -EINVAL;
	}

	IPADBG("x->xso.dir = %u", x->xso.dir);
	switch (x->xso.dir) {
	case XFRM_DEV_OFFLOAD_OUT:
		/* We can use HW offloaded encap SAs only once the uC completed NextIV WA init */
		if (!ipa3_ctx->uc_ctx.ipsec_next_iv_wa_ready) {
			IPAERR("Next IV uC workaround is not yet ready.\n");
			return -EBUSY;
		}

		/* find a free index */
		idx = _ipa_ipsec_next_free_sa(IPA_IPSEC_ENCAP);
		if (idx >= IPA_IPSEC_MAX_SA_NUM) {
			IPAERR("No free SA index was found\n");
			return -EINVAL;
		}

		memset(&esa, 0, sizeof(esa));

		/* copy data to the temp SA struct */
		esa.shar.salt_val = 0x0;
		esa.shar.encr_algo = ealg;
		esa.shar.encrkey_len = eklen;
		if (ealg == IPA_IPSEC_ENC_AES_GCM_16) {
			esa.shar.salt_needed = 1;
			esa.shar.salt_val = be32_to_cpu(*salt);
		}
		esa.shar.iv_sz = ivlen;
		esa.shar.auth_algo = aalg;
		esa.shar.authkey_len = aklen;
		esa.shar.esn_en = !!(x->props.flags & XFRM_STATE_ESN);
		esa.shar.icv_sz = icvlen;
		esa.stat.nat_t = !!x->encap;
		esa.stat.copy_df = !(x->props.flags & XFRM_STATE_NOPMTUDISC);
		esa.stat.copy_dscp = !(x->props.extra_flags & XFRM_SA_XFLAG_DONT_ENCAP_DSCP);
		esa.stat.copy_ecn = !(x->props.flags & XFRM_STATE_NOECN);
		esa.stat.overflow_allowed = x->props.extra_flags & XFRM_SA_XFLAG_OSEQ_MAY_WRAP;
		esa.stat.copy_flow_lbl = 0;
		esa.stat.path_mtu = x->props.family == AF_INET ?
			ipa3_ctx->ipsec->mtu_v4 : ipa3_ctx->ipsec->mtu_v6;
		esa.stat.sa_life_bytes_wm = x->lft.soft_byte_limit ? : XFRM_INF;
		esa.stat.sa_life_bytes = x->lft.hard_byte_limit ? : XFRM_INF;
		esa.dyna.ipv4_id = 0;
		if (x->props.flags & XFRM_STATE_ESN) {
			esa.dyna.seq_num =
				(u64)(x->replay_esn->oseq) | ((u64)(x->replay_esn->oseq_hi) << 32);
		} else {
			esa.dyna.seq_num = (u64)(x->replay.oseq);
		}
		esa.dyna.volume_bytes = 0;

		IPA_ACTIVE_CLIENTS_INC_SIMPLE();
		/* install the SA into SRAM */
		ret = ipa_ipsec_install_encap_sa(idx, &esa);
		if (!!ret) {
			IPA_ACTIVE_CLIENTS_DEC_SIMPLE();
			goto state_end;
		}

		if (eklen) IPADBG_LOW("ekey = %32phN", ekey);
		ret = ipa_ipsec_install_key(IPA_IPSEC_ENCAP, idx, IPA_IPSEC_KEY_ENC, eklen, ekey);
		if (!!ret) {
			IPA_ACTIVE_CLIENTS_DEC_SIMPLE();
			goto clean_sa;
		}
		IPADBG_LOW("e key = %32phN", ipa3_ctx->ipsec->keys->enc[idx].b256.b);
		if (aklen) IPADBG_LOW("akey = %64phN", akey);
		ret = ipa_ipsec_install_key(IPA_IPSEC_ENCAP, idx, IPA_IPSEC_KEY_AUTH, aklen, akey);
		if (!!ret) {
			IPA_ACTIVE_CLIENTS_DEC_SIMPLE();
			goto zero_keys;
		}
		IPADBG_LOW("a key = %64phN", ipa3_ctx->ipsec->keys->auth[idx].b512.b);

		IPA_ACTIVE_CLIENTS_DEC_SIMPLE();

		ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].x = x;
		x->xso.offload_handle = (unsigned long)idx | IPA_IPSEC_OFFLOAD_MAGIC;

		/* Check existing UL policies and install RT if needed */
		ret = ipa_ipsec_install_cached_ul_pols(idx);
		if (!!ret)
			goto free_idx;
		else
			goto state_end;

		break;

	case XFRM_DEV_OFFLOAD_IN:
		/* find a free index */
		idx = _ipa_ipsec_next_free_sa(IPA_IPSEC_DECAP);
		if (idx >= IPA_IPSEC_MAX_SA_NUM) {
			IPAERR("No free SA index was found\n");
			return -EINVAL;
		}

		memset(&dsa, 0, sizeof(dsa));

		/* copy data to the temp SA struct */
		if (x->props.flags & XFRM_STATE_ESN) {
			if (!x->replay_esn) {
				IPAERR("XFRM_STATE_ESN is on, but x->replay_esn is NULL\n");
				return -EINVAL;
			}
			dsa.shar.antirep_win_sz =
				xdo2ipa_replay_window_sz(x->replay_esn->replay_window);
			dsa.shar.esn_en = 1;
		} else {
			dsa.shar.antirep_win_sz = xdo2ipa_replay_window_sz(x->props.replay_window);
		}
		dsa.intr.antirep_top = dsa.shar.antirep_win_sz ? dsa.shar.antirep_win_sz - 1 : 0;
		dsa.shar.salt_val = 0x0;
		dsa.shar.encr_algo = ealg;
		dsa.shar.encrkey_len = eklen;
		if (ealg == IPA_IPSEC_ENC_AES_GCM_16) {
			dsa.shar.salt_needed = 1;
			dsa.shar.salt_val = be32_to_cpu(*salt);
		}
		dsa.shar.no_pad_chk = 0;
		dsa.shar.iv_sz = ivlen;
		dsa.shar.auth_algo = aalg;
		dsa.shar.authkey_len = aklen;
		dsa.shar.icv_sz = icvlen;
		dsa.shar.ecn_fld_lut = 0xFFE4D4E4; /* Arch doc Table 6-30 and 6-31 */
		dsa.shar.ecn_expt_lut = 0x1390; /* Arch doc Table 6-30 and 6-31 */
		dsa.shar.ecn_upd = !(x->props.flags & XFRM_STATE_NOECN);
		dsa.stat.nat_t = !!x->encap;
		dsa.stat.sa_life_bytes_wm =
			x->lft.soft_byte_limit ? x->lft.soft_byte_limit : XFRM_INF;
		dsa.stat.sa_life_bytes =
			x->lft.hard_byte_limit ? x->lft.hard_byte_limit : XFRM_INF;
		dsa.dyna.volume_bytes = 0;

		IPA_ACTIVE_CLIENTS_INC_SIMPLE();
		/* install the SA into SRAM */
		ret = ipa_ipsec_install_decap_sa(idx, &dsa);
		if (!!ret) {
			IPA_ACTIVE_CLIENTS_DEC_SIMPLE();
			goto state_end;
		}

		if (eklen) IPADBG_LOW("ekey = %32phN", ekey);
		ret = ipa_ipsec_install_key(IPA_IPSEC_DECAP, idx, IPA_IPSEC_KEY_ENC, eklen, ekey);
		if (!!ret) {
			IPA_ACTIVE_CLIENTS_DEC_SIMPLE();
			goto clean_sa;
		}
		IPADBG_LOW("e key = %32ph",
			ipa3_ctx->ipsec->keys->enc[IPA_IPSEC_MAX_ENACAP_KEY_NUM + idx].b256.b);
		if (aklen) IPADBG_LOW("akey = %64phN", akey);
		ret = ipa_ipsec_install_key(IPA_IPSEC_DECAP, idx, IPA_IPSEC_KEY_AUTH, aklen, akey);
		if (!!ret) {
			IPA_ACTIVE_CLIENTS_DEC_SIMPLE();
			goto zero_keys;
		}
		IPADBG_LOW("a key = %64ph",
			ipa3_ctx->ipsec->keys->auth[IPA_IPSEC_MAX_ENACAP_KEY_NUM + idx].b512.b);

		IPA_ACTIVE_CLIENTS_DEC_SIMPLE();
		/* Construct and install HPC */
		ret = ipa_ipsec_install_decap_hpc(x, idx);
		if (!!ret)
			goto zero_keys;

		/* Install RT rule */
		ret = ipa_ipsec_install_decap_rt(x, idx);
		if (!!ret)
			goto del_hpc;

		ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].x = x;
		x->xso.offload_handle = (unsigned long)idx | IPA_IPSEC_OFFLOAD_MAGIC;

		ret = ipa_ipsec_install_cached_dl_pols(idx);
		if (!!ret) {
			IPAERR("Failed to update existing policies to allow the new state.\n");
			goto free_idx;
		} else
			goto state_end;

		break;

	case XFRM_DEV_OFFLOAD_FWD:
	default:
		return -EINVAL;
	}

free_idx:
	ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].x = 0;
	x->xso.offload_handle = 0;

	if (ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].rt > 0) {

		mutex_lock(&ipa3_ctx->lock);
		WARN_ON(__ipa3_del_rt_rule(ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].rt));
		mutex_unlock(&ipa3_ctx->lock);

		WARN_ON(ipa3_commit_rt((x->props.family == AF_INET6) ? IPA_IP_v6 : IPA_IP_v4));
		IPADBG("deleted RT rule %d\n",
			ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].rt);
		ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].rt = -1;
	}

del_hpc:
	mutex_lock(&ipa3_ctx->lock);
	if (x->xso.dir == XFRM_DEV_OFFLOAD_OUT) {
		if (ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc)
			WARN_ON(__ipa3_release_hdr_proc_ctx(
				ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc));
		ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc = 0;
		if (ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hdr)
			WARN_ON(__ipa3_release_hdr(
				ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hdr));
		ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hdr = 0;
	} else {
		if (ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].hpc)
			WARN_ON(__ipa3_release_hdr_proc_ctx(
				ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].hpc));
		ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].hpc = 0;
	}
	mutex_unlock(&ipa3_ctx->lock);

zero_keys:
	IPA_ACTIVE_CLIENTS_INC_SIMPLE();
	if (x->xso.dir == XFRM_DEV_OFFLOAD_OUT) {
		ipa_ipsec_delete_key(IPA_IPSEC_ENCAP, idx, IPA_IPSEC_KEY_ENC);
		ipa_ipsec_delete_key(IPA_IPSEC_ENCAP, idx, IPA_IPSEC_KEY_AUTH);
	} else {
		ipa_ipsec_delete_key(IPA_IPSEC_DECAP, idx, IPA_IPSEC_KEY_ENC);
		ipa_ipsec_delete_key(IPA_IPSEC_DECAP, idx, IPA_IPSEC_KEY_AUTH);
	}
	IPA_ACTIVE_CLIENTS_DEC_SIMPLE();

clean_sa:
	IPA_ACTIVE_CLIENTS_INC_SIMPLE();
	if (x->xso.dir == XFRM_DEV_OFFLOAD_OUT) {
		memset_io(ipa3_ctx->ipsec->encap + idx, 0, sizeof(struct ipa_ipsec_sa_encap));
		ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].x = NULL;
	} else {
		memset_io(ipa3_ctx->ipsec->decap + idx, 0, sizeof(struct ipa_ipsec_sa_decap));
		ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].x = NULL;
	}
	IPA_ACTIVE_CLIENTS_DEC_SIMPLE();

state_end:
	IPADBG_LOW("ret = %d\n", ret);
	return ret;
}

/* Placeholder - no real driver action is needed */
void ipa_ipsec_xdo_state_delete(struct xfrm_state *x)
{
	IPADBG_LOW("Start\n");
	if (!ipa_ipsec_enabled()) {
		IPAERR("IPsec is not enabled.\n");
		return;
	}

	if (!x)
		return;

	IPADBG("x->xso.offload_handle = %08lX\n", x->xso.offload_handle);
}

void ipa_ipsec_xdo_state_free_work(struct work_struct *work)
{
	struct ipa_ipsec_state_work_wrap *work_data =
		container_of(work, struct ipa_ipsec_state_work_wrap, work);
	u8 idx = work_data->idx;

	switch (work_data->dir) {
	case XFRM_DEV_OFFLOAD_OUT:
		WARN_ON(ipa_ipsec_delete_orphan_ul_fnr(idx));
		mutex_lock(&ipa3_ctx->lock);
		WARN_ON(__ipa3_release_hdr_proc_ctx(
			ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc));
		ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc = 0;
		WARN_ON(__ipa3_release_hdr(ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hdr));
		ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hdr = 0;
		mutex_unlock(&ipa3_ctx->lock);
		IPA_ACTIVE_CLIENTS_INC_SIMPLE();
		memset_io(ipa3_ctx->ipsec->encap + idx, 0, sizeof(struct ipa_ipsec_sa_encap));
		ipa_ipsec_delete_key(IPA_IPSEC_ENCAP, idx, IPA_IPSEC_KEY_ENC);
		ipa_ipsec_delete_key(IPA_IPSEC_ENCAP, idx, IPA_IPSEC_KEY_AUTH);
		IPA_ACTIVE_CLIENTS_DEC_SIMPLE();
		ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].x = NULL;
		break;
	case XFRM_DEV_OFFLOAD_IN:
		if (ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].rt) {
			ipa_ipsec_delete_orphan_dl_fnr(work_data->ip, idx);

			mutex_lock(&ipa3_ctx->lock);
			WARN_ON(__ipa3_del_rt_rule(ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].rt));
			mutex_unlock(&ipa3_ctx->lock);

			WARN_ON(ipa3_commit_rt(work_data->ip));
			IPADBG("deleted RT rule %d\n",
				ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].rt);
			ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].rt = -1;
		}
		if (ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].hpc) {
			mutex_lock(&ipa3_ctx->lock);
			WARN_ON(__ipa3_release_hdr_proc_ctx(
					ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].hpc));
			ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].hpc = 0;
			mutex_unlock(&ipa3_ctx->lock);
		}
		IPA_ACTIVE_CLIENTS_INC_SIMPLE();
		memset_io(ipa3_ctx->ipsec->decap + idx, 0, sizeof(struct ipa_ipsec_sa_decap));
		ipa_ipsec_delete_key(IPA_IPSEC_DECAP, idx, IPA_IPSEC_KEY_ENC);
		ipa_ipsec_delete_key(IPA_IPSEC_DECAP, idx, IPA_IPSEC_KEY_AUTH);
		IPA_ACTIVE_CLIENTS_DEC_SIMPLE();
		ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].x = NULL;
		break;
	default:
		return;
	}

	/* Reset SA statistics*/
	memset(&ipa3_ctx->ipsec->stats.decap_stats[idx], 0,
		sizeof(struct ipa_ipsec_decap_stats));
	memset(&ipa3_ctx->ipsec->stats.encap_stats[idx], 0,
		sizeof(struct ipa_ipsec_encap_stats));

	kfree(work_data);
}

void ipa_ipsec_xdo_state_free(struct xfrm_state *x)
{
	int idx = IPA_IPSEC_MAX_SA_NUM;
	struct ipa_ipsec_state_work_wrap *work_data;

	IPADBG_LOW("Start\n");
	if (!ipa_ipsec_enabled()) {
		IPAERR("IPsec is not enabled.\n");
		return;
	}

	if (!x || (x->xso.offload_handle & IPA_IPSEC_OFFLOAD_MAGIC) != IPA_IPSEC_OFFLOAD_MAGIC) {
		IPADBG("Called for a non-offloaded state\n");
		return;
	}

	IPADBG("x = 0x%p, x->xso.offload_handle = %08lX, x->xso.dir = %d\n",
		x, x->xso.offload_handle, x->xso.dir);

	if ((idx = x->xso.offload_handle & 0xF) >= IPA_IPSEC_MAX_SA_NUM) {
		IPADBG("Called for a dummy state\n");
		return;
	}

	work_data = kzalloc(sizeof(struct ipa_ipsec_state_work_wrap), GFP_ATOMIC);
	if (!work_data) {
		IPAERR("failed allocating ipa_ipsec_state_work_wrap\n");
		BUG();
	}
	INIT_WORK(&work_data->work, ipa_ipsec_xdo_state_free_work);
	work_data->ip = (x->props.family == AF_INET6) ? IPA_IP_v6 : IPA_IP_v4;
	work_data->idx = idx;
	work_data->dir = x->xso.dir;
	queue_work(ipa_ipsec_wq, &work_data->work);
}

bool ipa_ipsec_xdo_offload_ok(struct sk_buff *skb, struct xfrm_state *x)
{
	u8 idx;

	IPADBG_LOW("Start x = %p\n", x);
	if (!ipa_ipsec_enabled()) {
		IPAERR("IPsec is not enabled.\n");
		return false;
	}

	if (!skb || !x) {
		IPAERR("Null state or skb\n");
		return false;
	}

	if (x->xso.type != XFRM_DEV_OFFLOAD_PACKET ||
	    (x->xso.offload_handle & IPA_IPSEC_OFFLOAD_MAGIC) != IPA_IPSEC_OFFLOAD_MAGIC) {
		IPAERR("Non offloaded state\n");
		return false;
	}

	idx = (u8)(x->xso.offload_handle & ~IPA_IPSEC_OFFLOAD_MAGIC);
	if (idx >= IPA_IPSEC_MAX_SA_NUM) {
		IPAERR("Dummy state\n");
		return false;
	}

	if (((x->xso.dir == XFRM_DEV_OFFLOAD_OUT) &&
			(ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].x != x)) ||
	    ((x->xso.dir == XFRM_DEV_OFFLOAD_IN) &&
			(ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].x != x))) {
		IPAERR("The HW offloaded state doesn't match the XFRM state\n");
		return false;
	}

	/* Set skb cb for the datapath use */
	skb->ipa_skb_cb.magic = IPA_IPSEC_SKB_MAGIC;
	skb->ipa_skb_cb.sa_idx = idx;
	skb->ipa_skb_cb.sa_dir = x->xso.dir;

	IPADBG_LOW("success\n");
	return true;
}

/* Placeholder - no real driver action is needed */
void ipa_ipsec_xdo_state_advance_esn(struct xfrm_state *x)
{
	IPADBG("Start\n");
	if (!ipa_ipsec_enabled()) {
		IPAERR("IPsec is not enabled.\n");
		return;
	}

	if (!x)
		return;

}

void ipa_ipsec_xdo_state_update_curlft(struct xfrm_state *x)
{
	u8 idx;
	struct ipa_ipsec_sa_encap *e_sa;
	struct ipa_ipsec_sa_decap *d_sa;
	struct ipa_active_client_logging_info log_info;

	IPADBG_LOW("Start\n");
	if (!ipa_ipsec_enabled()) {
		IPAERR("IPsec is not enabled.\n");
		return;
	}

	if (!x || x->xso.type != XFRM_DEV_OFFLOAD_PACKET ||
		(x->xso.offload_handle & IPA_IPSEC_OFFLOAD_MAGIC) != IPA_IPSEC_OFFLOAD_MAGIC) {
		IPADBG("Non offloaded state\n");
		return;
	}

	idx = (u8)(x->xso.offload_handle & ~IPA_IPSEC_OFFLOAD_MAGIC);
	if (idx >= IPA_IPSEC_MAX_SA_NUM) {
		IPADBG("Dummy state\n");
		return;
	}

	/* The callback is being called from the timer,
	   thus we must avoid the mutex locking in the regular API */
	IPA_ACTIVE_CLIENTS_PREP_SIMPLE(log_info);
	/* Don't access SRAM, if IPA is not clocked on */
	if (ipa3_inc_client_enable_clks_no_block(&log_info)) {
		IPADBG("IPA is not clocked, keep stored values\n");
		return;
	}

	switch (x->xso.dir) {
	case XFRM_DEV_OFFLOAD_OUT:
		e_sa = ipa3_ctx->ipsec->encap + idx;
		/* Encap fields are aligned */
		x->curlft.bytes = readq_relaxed((void __iomem *)&e_sa->dyna.volume_bytes);
		x->curlft.use_time = readq_relaxed((void __iomem *)&e_sa->dyna.last_pkt_timestamp);
		break;
	case XFRM_DEV_OFFLOAD_IN:
	case XFRM_DEV_OFFLOAD_FWD:
		d_sa = ipa3_ctx->ipsec->decap + idx;
		/* Decap fields are not aligned, so we copy the whole section */
		memcpy_fromio(&dsa.dyna, (void __iomem *)&d_sa->dyna,
			sizeof(struct ipa_ipsec_sa_decap_dynamic));
		x->curlft.bytes = dsa.dyna.volume_bytes;
		x->curlft.use_time = dsa.dyna.last_pkt_timestamp;
		break;
	default:
		break;
	}

	ipa3_dec_client_disable_clks_no_block(&log_info);
}

int ipa_ipsec_xdo_policy_add(struct xfrm_policy *xp, struct netlink_ext_ack *extack)
{
	int i, rc, rt = -1, flt = -1;
	u8 idx = IPA_IPSEC_MAX_SA_NUM;
	struct ipa_ipsec_policy *pol;
	struct ipa_ioc_ipsec_ul_flt_attr *ul_flt;
	enum ipa_ip_type ip;

	IPADBG("Start\n");
	if (!ipa_ipsec_enabled()) {
		IPAERR("IPsec is not enabled.\n");
		return -EFAULT;
	}

	if (!xp || xp->xdo.type != XFRM_DEV_OFFLOAD_PACKET ||
		(xp->selector.family != AF_INET && xp->selector.family != AF_INET6)) {
		IPADBG("Null policy or wrong XDO or invalid AF\n");
		return -EINVAL;
	}

	ip = xp->selector.family == AF_INET6 ? IPA_IP_v6 : IPA_IP_v4;

	/* HW policy must be main  */
	if (xp->type != XFRM_POLICY_TYPE_MAIN && xp->action != XFRM_POLICY_ALLOW) {
		IPAERR("HW policy must be ALLOW and main\n");
		return -EINVAL;
	}

	/* HW out policy is mapped exactly to 1 SA  */
	if (xp->xfrm_nr != 1) {
		IPAERR("HW policy must be mapped exactly to 1 SA! xfrm_nr = %d\n", xp->xfrm_nr);
		return -EINVAL;
	}

	pol = (struct ipa_ipsec_policy *)kzalloc(sizeof(struct ipa_ipsec_policy), GFP_KERNEL);
	if (!pol) {
		IPAERR("Failed to allocate ipa_ipsec_policy\n");
		return -ENOMEM;
	}
	for (i = 0; i < IPA_IPSEC_DL_FLT_PER_POL; i++)
		pol->flt[i] = -1;
	pol->rt = -1;
	pol->xp = xp;

	switch (xp->xdo.dir) {
	case XFRM_DEV_OFFLOAD_IN:
		/* Install FLT rule/s */
		for (i = 0, idx = 0;
		     idx < IPA_IPSEC_MAX_SA_NUM && i < IPA_IPSEC_DL_FLT_PER_POL; idx++) {
			if (ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].x &&
				ipa_ipsec_tmpl_sa_match(xp->xfrm_vec,
					ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].x)) {
				flt = ipa_ipsec_install_decap_flt(xp, idx);
				if (flt < 0) {
					kfree(pol);
					return flt;
				}
				pol->flt[i++] = flt;
			}
		}
		break;

	case XFRM_DEV_OFFLOAD_OUT:
		idx = ipa_ipsec_find_match_sa(xp->xfrm_vec, xp->family, IPA_IPSEC_ENCAP);
		if (idx == IPA_IPSEC_MAX_SA_NUM) {
			IPAERR("No offloaded encap SA found, will install FnR later\n");
			/* Install FnR later, once the SA index is available */
			rt = -1;
			break;
		}

		/*
		 * The following code adds HPC, RT and sends event to the IPACM,
		 * which is duplicating the code of the ipa_ipsec_install_cached_ul_pols().
		 * TODO: Get rid of code duplication by uniformly calling the FnR installation
		 * from both SA and SP callbacks in both in and out dir, based on a matched bundle.
		 * TODO2: Fix cleanup in case of HPC installation success and RT installation fail.
		 */

		/* Construct and install header template and HPC */
		if (ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc == 0) {
			rc = ipa_ipsec_install_encap_hpc(
				ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].x, idx, ip);
			if (rc < 0) {
				IPAERR("ipa_ipsec_install_encap_hpc returned %d\n", rc);
				kfree(pol);
				return rc;
			}
		}

		/* Install RT rule */
		rt = ipa_ipsec_install_encap_rt(xp, idx);
		if (rt < 0) {
			mutex_lock(&ipa3_ctx->lock);
			__ipa3_release_hdr_proc_ctx(ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc);
			ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hpc = 0;
			__ipa3_release_hdr(ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hdr);
			ipa3_ctx->ipsec->sa_db[IPA_IPSEC_ENCAP][idx].hdr = 0;
			mutex_unlock(&ipa3_ctx->lock);
			kfree(pol);
			return rt;
		}
		pol->rt = rt;

		ul_flt = (struct ipa_ioc_ipsec_ul_flt_attr *)kzalloc(
			sizeof(struct ipa_ioc_ipsec_ul_flt_attr), GFP_KERNEL);
		if (!ul_flt) {
			IPAERR("Failed to allocate ipa_ioc_ipsec_ul_flt_attr\n");
			kfree(pol);
			return -EFAULT;
		}

		ipa_ipsec_xfrm_sp_to_ipa_attrib(xp, &ul_flt->attr, IPA_IPSEC_MAX_SA_NUM);
		ul_flt->ip = ip;
		if (ipa3_send_ipsec_ul_flt(IPA_IPSEC_UL_FLT_ADD_EVENT, ul_flt) != 0) {
			kfree(pol);
			return -EFAULT;
		}

		break;

	default:
		IPAERR("Cannot offload forward policy\n");
		kfree(pol);
		return -EINVAL;
	}

	mutex_lock(&ipa3_ctx->ipsec->pol_list_lock);
	list_add(&pol->l, &ipa3_ctx->ipsec->pol_list);
	xp->xdo.offload_handle = (unsigned long)pol;
	mutex_unlock(&ipa3_ctx->ipsec->pol_list_lock);

	return 0;
}

/* Top half of the SA threshold uC event handler */
void ipa_ipsec_handle_sa_thresh(u8 idx, enum ipa_ipsec_uc_sa_action action,
	enum ipa_ipsec_uc_thresh_type thresh_type)
{
	enum ipa_ipsec_sa_type sa_type;
	struct xfrm_state *x;

	IPADBG("Received uC event with params: idx = %d, action = %d, thresh_type = %d\n",
			 idx, action, thresh_type);

	if (idx >= IPA_IPSEC_MAX_SA_NUM ||
	    action == IPA_IPSEC_UC_SA_ACT_NONE || action >= IPA_IPSEC_UC_SA_ACT_MAX) {
		IPAERR("Received uC event with wrong params: idx = %d, action = %d\n",
			 idx, action);
		return;
	}

	sa_type = (action == IPA_IPSEC_UC_SA_ACT_ENCAP) ? IPA_IPSEC_ENCAP : IPA_IPSEC_DECAP;

	x = ipa3_ctx->ipsec->sa_db[sa_type][idx].x;
	if (unlikely(!x)) {
		IPAERR("%s SA%02d has no XFRM state pointer (0x%p) \n",
			(sa_type == IPA_IPSEC_ENCAP) ? "Encap" : "Decap", idx, x);
		return;
		//goto no_work;
	}

	IPADBG("%s SA%02d XFRM state pointer (0x%p) \n",
		(sa_type == IPA_IPSEC_ENCAP) ? "Encap" : "Decap", idx, x);

	spin_lock_bh(&x->lock);
	switch (thresh_type) {
	case IPA_IPSEC_UC_THRESH_SOFT:
		if (!x->km.dying) {
			x->km.dying = 1;
			km_state_expired(x, 0, 0);
		}
		break;
	case IPA_IPSEC_UC_THRESH_HARD:
		x->km.state = XFRM_STATE_EXPIRED;
		hrtimer_start(&x->mtimer, 0, HRTIMER_MODE_REL_SOFT);
		break;
	default:
		BUG();
	}
	spin_unlock_bh(&x->lock);
}

/* The DL (LAN) RT tables are being deleted on LAN down and recreated on LAN up.
 * Redirecting the IPsec policy FLT rules accordingly.
 */
int ipa_ipsec_handle_lan_up_down(enum ipa_ip_type ip, struct ipa3_rt_tbl *rt_tbl, bool up)
{
	int i;
	struct ipa_ipsec_policy *pol;
	struct ipa3_flt_entry *flt_rule;
	bool commit = false;

	IPADBG("IPv%d, %s %s\n", ip == IPA_IP_v4 ? 4 : 6, rt_tbl->name, up ? "added" : "deleted");

	if (strncmp(rt_tbl->name, __ipa_ipsec_s.dl_rt[ip], IPA_RESOURCE_NAME_MAX) != 0)
		/* We only take care of relevant RT tables */
		return 0;

	IPADBG("Will update DL policies\n");

	mutex_lock(&ipa3_ctx->ipsec->pol_list_lock);
	list_for_each_entry(pol, &ipa3_ctx->ipsec->pol_list, l) {
		if (pol->xp->xdo.dir == XFRM_DEV_OFFLOAD_IN &&
		    ((ip == IPA_IP_v4 && pol->xp->family == AF_INET) ||
		     (ip == IPA_IP_v6 && pol->xp->family == AF_INET6))) {
			for (i = 0; i < IPA_IPSEC_DL_FLT_PER_POL; i++) {

				IPADBG("pol->flt[%d] = %d\n", i, pol->flt[i]);

				flt_rule = ipa3_id_find(pol->flt[i]);
				if (unlikely(flt_rule == NULL)) {
					mutex_unlock(&ipa3_ctx->ipsec->pol_list_lock);
					IPAERR("FLT tbl not found\n");
					return -EFAULT;
				}

				/*  this function called under mutex_lock(&ipa3_ctx->lock),
				    so touching ref_cnt is safe */
				if (flt_rule->rt_tbl)
					flt_rule->rt_tbl->ref_cnt--;

				flt_rule->rt_tbl = up ? rt_tbl : ipa3_ctx->ipsec->default_rt;
				flt_rule->rule.rt_tbl_hdl =
					up ? rt_tbl->id : ipa3_ctx->ipsec->default_rt->id;
				flt_rule->rule.rt_tbl_idx =
					up ? rt_tbl->idx : ipa3_ctx->ipsec->default_rt->idx;

				flt_rule->rt_tbl->ref_cnt++;
			}
			commit = true;
		}
	}
	mutex_unlock(&ipa3_ctx->ipsec->pol_list_lock);

	if (commit)
		return ipa3_commit_flt(ip);

	IPADBG_LOW("Done\n");

	return 0;
}

/* Placeholder - no real driver action is needed */
void ipa_ipsec_xdo_policy_delete(struct xfrm_policy *xp)
{
	IPADBG_LOW("Start\n");
	if (!ipa_ipsec_enabled()) {
		IPAERR("IPsec is not enabled.\n");
		return;
	}

	if (!xp)
		return;

	IPADBG("xp->xdo.offload_handle = %08lX\n", xp->xdo.offload_handle);
}

void ipa_ipsec_xdo_policy_free_work(struct work_struct *work)
{
	int i;
	struct ipa_ioc_ipsec_ul_flt_attr *ul_flt = NULL;
	struct ipa3_rt_entry *rt_entry = NULL;
	struct ipa_ipsec_policy_work_wrap *work_data =
		container_of(work, struct ipa_ipsec_policy_work_wrap, work);
	struct ipa_ipsec_policy *pol = work_data->pol;

	for (i = 0; i < IPA_IPSEC_DL_FLT_PER_POL; i++) {
		if (pol->flt[i] != -1) {
			mutex_lock(&ipa3_ctx->lock);
			WARN_ON(__ipa_del_flt_rule(pol->flt[i]));
			IPADBG("deleted FLT rule %d\n", pol->flt[i]);
			mutex_unlock(&ipa3_ctx->lock);
			pol->flt[i] = -1;
		}
	}
	if (pol->rt != -1) {
		mutex_lock(&ipa3_ctx->lock);
		rt_entry = ipa3_id_find(pol->rt);

		if (work_data->dir == XFRM_DEV_OFFLOAD_OUT) {
			ul_flt = (struct ipa_ioc_ipsec_ul_flt_attr *)kzalloc(
				sizeof(struct ipa_ioc_ipsec_ul_flt_attr), GFP_KERNEL);
			BUG_ON(!ul_flt);
			memcpy(&ul_flt->attr, &rt_entry->rule.attrib,
				sizeof(struct ipa_rule_attrib));
			ul_flt->ip = work_data->ip;
		}
		WARN_ON(__ipa3_del_rt_rule(pol->rt));
		mutex_unlock(&ipa3_ctx->lock);

		IPADBG("deleted RT rule %d\n", pol->rt);
		pol->rt = -1;

		if (ul_flt)
			ipa3_send_ipsec_ul_flt(IPA_IPSEC_UL_FLT_DEL_EVENT, ul_flt);
	}

	/* This will commit flt as well */
	WARN_ON(ipa3_commit_rt(work_data->ip));

	mutex_lock(&ipa3_ctx->ipsec->pol_list_lock);
	list_del(&pol->l);
	kfree(pol);
	mutex_unlock(&ipa3_ctx->ipsec->pol_list_lock);
	kfree(work_data);
}

void ipa_ipsec_xdo_policy_free(struct xfrm_policy *xp)
{
	struct ipa_ipsec_policy_work_wrap *work_data;

	IPADBG("Start\n");
	if (!ipa_ipsec_enabled()) {
		IPAERR("IPsec is not enabled.\n");
		return;
	}

	if (!xp || (xp->selector.family != AF_INET && xp->selector.family != AF_INET6) ||
		xp->xdo.offload_handle == 0) {
		IPADBG("Called for a non-offloaded policy\n");
		return;
	}

	IPADBG("x->xso.offload_handle = %lX\n", xp->xdo.offload_handle);

	work_data = kzalloc(sizeof(struct ipa_ipsec_policy_work_wrap), GFP_ATOMIC);
	if (!work_data) {
		IPAERR("failed allocating ipa_ipsec_policy_work_wrap\n");
		return;
	}

	INIT_WORK(&work_data->work, ipa_ipsec_xdo_policy_free_work);
	work_data->ip = (xp->selector.family == AF_INET6) ? IPA_IP_v6 : IPA_IP_v4;
	work_data->pol = (struct ipa_ipsec_policy *)xp->xdo.offload_handle;
	work_data->dir = xp->xdo.dir;
	queue_work(ipa_ipsec_wq, &work_data->work);
	xp->xdo.offload_handle = 0;
}

int ipa_ipsec_udate_decap_rt_rule(u32 qmap_hdr_hdl)
{
	int ip, ret = 0;
	struct ipa_ioc_mdfy_rt_rule_v2  *rt_rule = NULL;
	struct ipa_rt_rule_mdfy_v2 *rt_rule_entry;

	IPADBG("qmap_hdr_hdl = %d\n", qmap_hdr_hdl);
	rt_rule = (struct ipa_ioc_mdfy_rt_rule_v2 *)kzalloc(sizeof(struct ipa_ioc_mdfy_rt_rule_v2), GFP_KERNEL);
	if (!rt_rule) {
		IPAERR("Failed to allocate ipa_ioc_mdfy_rt_rule_v2\n");
		ret = -ENOMEM;
		goto fail_rt;
	}
	rt_rule->rules = (uint64_t)kzalloc(2 * sizeof(struct ipa_rt_rule_mdfy_v2), GFP_KERNEL);
	if (!rt_rule->rules) {
		IPAERR("Failed to allocate ipa_rt_rule_mdfy_v2\n");
		ret = -ENOMEM;
		goto fail_rt_rules;
	}
	for (ip = IPA_IP_v4; ip < IPA_IP_MAX; ip++) {
		memset((void *)rt_rule->rules, 0, 2 * sizeof(struct ipa_rt_rule_mdfy_v2));
		rt_rule->commit = 1;
		rt_rule->ip = ip;
		rt_rule->num_rules++;
		rt_rule->rule_mdfy_size = 2 * sizeof(struct ipa_rt_rule_mdfy_v2);
		rt_rule_entry = &(((struct ipa_rt_rule_mdfy_v2 *)rt_rule->rules)[0]);
		rt_rule_entry->rt_rule_hdl = ipa3_ctx->ipsec->decap_rt_rule_hdl[ip].esp_src_port_hdl;
		rt_rule_entry->rule.hdr_hdl = qmap_hdr_hdl;

		/* UDP 4500 + ESP after UDP */
		rt_rule_entry->rule.dst = IPA_CLIENT_APPS_WAN_CONS;
		rt_rule_entry->rule.hashable = true;
		rt_rule_entry->rule.esp_after_udp = 1;
		if (ip == IPA_IP_v4) {
			rt_rule_entry->rule.attrib.attrib_mask =
				IPA_FLT_PROTOCOL | IPA_FLT_SRC_PORT;
			rt_rule_entry->rule.attrib.u.v4.protocol = IPPROTO_UDP;
		} else {
			rt_rule_entry->rule.attrib.attrib_mask =
				IPA_FLT_NEXT_HDR | IPA_FLT_SRC_PORT;
			rt_rule_entry->rule.attrib.u.v6.next_hdr = IPPROTO_UDP;
		}
		/* 4500 is the standard NAT-T port */
		rt_rule_entry->rule.attrib.src_port = 4500;

		/* Catch all */
		rt_rule->num_rules++;
		rt_rule_entry = &(((struct ipa_rt_rule_mdfy_v2 *)rt_rule->rules)[1]);
		rt_rule_entry->rt_rule_hdl = ipa3_ctx->ipsec->decap_rt_rule_hdl[ip].catchall_hdl;
		rt_rule_entry->rule.hdr_hdl = qmap_hdr_hdl;
		rt_rule_entry->rule.dst = IPA_CLIENT_APPS_WAN_CONS;
		rt_rule_entry->rule.hashable = true;

		ret = ipa3_mdfy_rt_rule_v2(rt_rule);
		if (!ret) {
			IPAERR("ipa3_mdfy_rt_rule returned %d\n", ret);
			goto end;
		}
	}
end:
	kfree((void *)rt_rule->rules);
fail_rt_rules:
	kfree(rt_rule);
fail_rt:
	return ret;


}

/* Install EP independent FLT table for DL policy rules */
int ipa_ipsec_install_dl_pol_flt(u32 qmap_hdr_hdl)
{
	int ip, ret = 0;
	struct ipa_flt_rule_add_v2 *flt_rule_frag = NULL;
	struct ipa_flt_rule_add_v2 *flt_rule_no_policy = NULL;
	struct ipa_flt_rule_add_v2 *flt_rule_catch_all = NULL;
	struct ipa_ioc_add_flt_rule_v2 *flt_tbl = NULL;
	struct ipa_ioc_get_rt_tbl rt_lookup;

	IPADBG("Start\n");
	if (!ipa3_ctx->ipsec) {
		IPAERR("IPsec is not initialized\n");
		ret = -EINVAL;
		return ret;
	}

	ipa_ipsec_udate_decap_rt_rule(qmap_hdr_hdl);
	flt_tbl = kzalloc(sizeof(*flt_tbl), GFP_KERNEL);
	if (!flt_tbl) {
		IPAERR("Failed to allocate ipa_ioc_add_flt_rule_v2\n");
		ret = -ENOMEM;
		return ret;
	}
	flt_tbl->rules = (uint64_t)kzalloc(3 *
		sizeof(struct ipa_flt_rule_add_v2), GFP_KERNEL);
	if (!flt_tbl->rules) {
		IPAERR("Failed to allocate ipa_flt_rule_add_v2\n");
		ret = -ENOMEM;
		goto end;
	}

	flt_tbl->commit = 1;
	flt_tbl->ep = IPA_IPSEC_DL_POL_FLT_ID;
	flt_tbl->num_rules = 3;
	flt_tbl->flt_rule_size = sizeof(struct ipa_flt_rule_add_v2);

	/* IPsec Frag Secondary packets rule */
	flt_rule_frag = &(((struct ipa_flt_rule_add_v2 *)flt_tbl->rules)[0]);
	flt_rule_frag->flt_rule_hdl = -1;
	flt_rule_frag->status = -1;
	flt_rule_frag->rule.hashable = false;
	flt_rule_frag->rule.attrib.attrib_mask = IPA_FLT_FRAGMENT;
	flt_rule_frag->rule.attrib.is_frag_encoding = 0x2; //Secondary fragmented packets
	flt_rule_frag->rule.action = IPA_PASS_TO_EXCEPTION;

	/* IPsec no policy metadata rule */
	flt_rule_no_policy = &(((struct ipa_flt_rule_add_v2 *)flt_tbl->rules)[1]);
	flt_rule_no_policy->at_rear = 1;
	flt_rule_no_policy->flt_rule_hdl = -1;
	flt_rule_no_policy->status = -1;
	flt_rule_no_policy->rule.hashable = false;
	flt_rule_no_policy->rule.attrib.attrib_mask = IPA_FLT_META_DATA;
	flt_rule_no_policy->rule.attrib.meta_data_mask = META_IS_IPSEC;
	flt_rule_no_policy->rule.attrib.meta_data = META_IS_IPSEC;
	flt_rule_no_policy->rule.action = IPA_PASS_TO_ROUTING;

	/* Catch all rule */
	flt_rule_catch_all = &(((struct ipa_flt_rule_add_v2 *)flt_tbl->rules)[2]);
	flt_rule_catch_all->at_rear = 1;
	flt_rule_catch_all->flt_rule_hdl = -1;
	flt_rule_catch_all->status = -1;
	flt_rule_catch_all->rule.hashable = false;
	flt_rule_catch_all->rule.action = IPA_PASS_TO_ROUTING;

	for (ip = IPA_IP_v4; ip < IPA_IP_MAX; ip++) {
		flt_tbl->ip = ip;
		rt_lookup.ip = ip;

		strlcpy(rt_lookup.name, __ipa_ipsec_s.decap_no_policy_rt[ip], IPA_RESOURCE_NAME_MAX);
		ret = ipa3_get_rt_tbl(&rt_lookup);
		if (unlikely(!!ret)) {
			IPAERR("%s is not installed\n", __ipa_ipsec_s.decap_no_policy_rt[ip]);
			goto end;
		}
		flt_rule_no_policy->rule.rt_tbl_hdl = rt_lookup.hdl;

		strlcpy(rt_lookup.name, __ipa_ipsec_s.default_rt, IPA_RESOURCE_NAME_MAX);
		ret = ipa3_get_rt_tbl(&rt_lookup);
		if (unlikely(!!ret)) {
			IPAERR("%s is not installed\n", __ipa_ipsec_s.default_rt);
			goto end;
		}
		flt_rule_catch_all->rule.rt_tbl_hdl = rt_lookup.hdl;

		if (ipa_put_rt_tbl(rt_lookup.hdl)) {
			IPAERR("ipa_put_rt_tbl() failure.\n");
		}

		ret = ipa3_add_flt_rule_usr_v2(flt_tbl, false);
		if (!!ret) {
			IPAERR("Fail to add FLT table for DL policy rules for ip %d, \
			flt_rule_catch_all->status %d , flt_rule_no_policy->status %d \
			flt_rule_frag->status %d\n",
			ip,
			flt_rule_catch_all->status, flt_rule_no_policy->status,
			flt_rule_frag->status);
			goto end;
		}

		ipa3_ctx->ipsec->dl_pol_flt[ip] = IPA_IPSEC_DL_POL_FLT_ID;
	}

end:
	kfree((void *)flt_tbl->rules);
	kfree(flt_tbl);

	return ret;
}
EXPORT_SYMBOL(ipa_ipsec_install_dl_pol_flt);

#define IPA_QMI_IPSEC_FLT_NUM 8
int ipa_ipsec_install_qmi_flt(struct ipa_install_fltr_rule_req_ex_msg_v01 *req)
{
	int pos, rc, temp;
	int mux_id = ipa3_ctx->ipsec->mux_id;
	enum ipa_ip_type ip;
	struct ipa3_rt_tbl *rt_tbl;
	struct ipa_rule_attrib attrib;
	struct ipa_ipfltri_rule_eq eq_atrb;

	IPADBG_LOW("Start\n");

	if (mux_id == 0 || req->filter_spec_ex_list_len == 0) {
		IPADBG("Cleanup or non-default PDN QMI. Won't send the IKE and IPsec rules.\n");
		return 0;
	}

	IPADBG("Default PDN mux_id = %d\n", mux_id);

	if (req->filter_spec_ex_list_len + IPA_QMI_IPSEC_FLT_NUM > QMI_IPA_MAX_FILTERS_EX_V01) {
		IPAERR("Not enough memory in the QMI buffer\n");
		return -ENOMEM;
	}

	temp = req->filter_spec_ex_list_len - 1;
	req->filter_spec_ex_list_len += IPA_QMI_IPSEC_FLT_NUM;
	req->filter_spec_ex_list_valid = 1;
	while (temp >= 0) {
		memcpy(&req->filter_spec_ex_list[temp + IPA_QMI_IPSEC_FLT_NUM],
			&req->filter_spec_ex_list[temp],
			sizeof(struct ipa_filter_spec_ex_type_v01));
		temp--;
	}

	pos = 0;
	for (ip = IPA_IP_v4; ip < IPA_IP_MAX; ip++) {
		/* IKE rules */

		memset(&attrib, 0, sizeof(attrib));
		memset(&eq_atrb, 0, sizeof(eq_atrb));

		mutex_lock(&ipa3_ctx->lock);
		rt_tbl = __ipa3_find_rt_tbl(ip, __ipa_ipsec_s.default_rt);
		mutex_unlock(&ipa3_ctx->lock);
		if (!rt_tbl) {
			IPAERR("__ipa3_find_rt_tbl returned null\n");
			return -EFAULT;
		}

		ipa3_ctx->ipsec->default_rt = rt_tbl;

		req->filter_spec_ex_list[pos].rule_id = QMI_IPA_IKE_FLT_ID_PREFIX + pos;
		req->filter_spec_ex_list[pos].filter_action = QMI_IPA_FILTER_ACTION_ROUTING_V01;
		req->filter_spec_ex_list[pos].is_mux_id_valid = true;
		req->filter_spec_ex_list[pos].mux_id = mux_id;
		req->filter_spec_ex_list[pos].is_rule_hashable = true;
		req->filter_spec_ex_list[pos].is_routing_table_index_valid = true;
		req->filter_spec_ex_list[pos].route_table_index = rt_tbl->idx;
		if (ip == IPA_IP_v4) {
			req->filter_spec_ex_list[pos].ip_type = QMI_IPA_IP_TYPE_V4_V01;
			attrib.attrib_mask = IPA_FLT_META_DATA|IPA_FLT_PROTOCOL|IPA_FLT_SRC_PORT;
			attrib.u.v4.protocol = IPPROTO_UDP;
		} else {
			req->filter_spec_ex_list[pos].ip_type = QMI_IPA_IP_TYPE_V6_V01;
			attrib.attrib_mask = IPA_FLT_META_DATA|IPA_FLT_NEXT_HDR|IPA_FLT_SRC_PORT;
			attrib.u.v6.next_hdr = IPPROTO_UDP;
		}
		attrib.src_port = 500;
		attrib.meta_data = mux_id << WWAN_METADATA_SHFT;
		attrib.meta_data_mask = WWAN_METADATA_MASK;
		rc = ipahal_flt_generate_equation(ip, &attrib, &eq_atrb);
		if (!!rc)
			return rc;
		IPADBG_LOW("pos = %d, copy %lu bytes from %p to %p", pos,
			sizeof(struct ipa_filter_rule_type_v01),
			&eq_atrb, &req->filter_spec_ex_list[pos].filter_rule);
		memcpy(&req->filter_spec_ex_list[pos].filter_rule, &eq_atrb,
			sizeof(struct ipa_filter_rule_type_v01));
		pos++;

		memset(&attrib, 0, sizeof(attrib));
		memset(&eq_atrb, 0, sizeof(eq_atrb));

		req->filter_spec_ex_list[pos].rule_id = QMI_IPA_IKE_FLT_ID_PREFIX + pos;
		req->filter_spec_ex_list[pos].filter_action = QMI_IPA_FILTER_ACTION_ROUTING_V01;
		req->filter_spec_ex_list[pos].ip_type = (ip == IPA_IP_v4) ?
			QMI_IPA_IP_TYPE_V4_V01 : QMI_IPA_IP_TYPE_V6_V01;
		req->filter_spec_ex_list[pos].is_mux_id_valid = true;
		req->filter_spec_ex_list[pos].mux_id = mux_id;
		req->filter_spec_ex_list[pos].is_rule_hashable = true;
		req->filter_spec_ex_list[pos].is_routing_table_index_valid = true;
		req->filter_spec_ex_list[pos].route_table_index = rt_tbl->idx;
		if (ip == IPA_IP_v4) {
			attrib.attrib_mask = IPA_FLT_META_DATA|IPA_FLT_PROTOCOL|IPA_FLT_SRC_PORT|IPA_FLT_SPI;
			attrib.u.v4.protocol = IPPROTO_UDP;
		} else {
			attrib.attrib_mask = IPA_FLT_META_DATA|IPA_FLT_NEXT_HDR|IPA_FLT_SRC_PORT|IPA_FLT_SPI;
			attrib.u.v6.next_hdr = IPPROTO_UDP;
		}
		attrib.src_port = 4500;
		attrib.spi = 0;
		attrib.ext_attrib_mask = IPA_FLT_EXT_NAT_T;
		attrib.meta_data = mux_id << WWAN_METADATA_SHFT;
		attrib.meta_data_mask = WWAN_METADATA_MASK;
		rc = ipahal_flt_generate_equation(ip, &attrib, &eq_atrb);
		if (!!rc)
			return rc;
		memcpy(&req->filter_spec_ex_list[pos].filter_rule, &eq_atrb,
			sizeof(struct ipa_filter_rule_type_v01));
		pos++;

		/* IPsec catch all rules
		   - must be after IKE rules,
		     because NAT-T UDP 4500 is a superset of the IKE UDP 4500 */
		rt_tbl = ipa3_id_find(ipa3_ctx->ipsec->decap_rt[ip]);
		if (!rt_tbl) {
			IPAERR("The IPsec encap RT is not created\n");
			return -EFAULT;
		}

		memset(&attrib, 0, sizeof(attrib));
		memset(&eq_atrb, 0, sizeof(eq_atrb));

		req->filter_spec_ex_list[pos].rule_id = QMI_IPA_IPSEC_FLT_ID_PREFIX + pos;
		req->filter_spec_ex_list[pos].filter_action = QMI_IPA_FILTER_ACTION_ROUTING_V01;
		req->filter_spec_ex_list[pos].is_mux_id_valid = true;
		req->filter_spec_ex_list[pos].mux_id = mux_id;
		req->filter_spec_ex_list[pos].is_rule_hashable = true;
		req->filter_spec_ex_list[pos].is_routing_table_index_valid = true;
		req->filter_spec_ex_list[pos].route_table_index = rt_tbl->idx;
		attrib.ext_attrib_mask = 0;
		if (ip == IPA_IP_v4) {
			req->filter_spec_ex_list[pos].ip_type = QMI_IPA_IP_TYPE_V4_V01;
			attrib.attrib_mask = IPA_FLT_META_DATA|IPA_FLT_PROTOCOL;
			attrib.u.v4.protocol = IPPROTO_ESP;
		} else {
			req->filter_spec_ex_list[pos].ip_type = QMI_IPA_IP_TYPE_V6_V01;
			attrib.attrib_mask = IPA_FLT_META_DATA|IPA_FLT_NEXT_HDR;
			attrib.u.v6.next_hdr = IPPROTO_ESP;
		}
		attrib.meta_data = mux_id << WWAN_METADATA_SHFT;
		attrib.meta_data_mask = WWAN_METADATA_MASK;
		rc = ipahal_flt_generate_equation(ip, &attrib, &eq_atrb);
		if (!!rc)
			return rc;
		memcpy(&req->filter_spec_ex_list[pos].filter_rule, &eq_atrb,
			sizeof(struct ipa_filter_rule_type_v01));
		pos++;

		memset(&attrib, 0, sizeof(attrib));
		memset(&eq_atrb, 0, sizeof(eq_atrb));

		req->filter_spec_ex_list[pos].rule_id = QMI_IPA_IPSEC_FLT_ID_PREFIX + pos;
		req->filter_spec_ex_list[pos].filter_action = QMI_IPA_FILTER_ACTION_ROUTING_V01;
		req->filter_spec_ex_list[pos].is_mux_id_valid = true;
		req->filter_spec_ex_list[pos].mux_id = mux_id;
		req->filter_spec_ex_list[pos].is_rule_hashable = true;
		req->filter_spec_ex_list[pos].is_routing_table_index_valid = true;
		req->filter_spec_ex_list[pos].route_table_index = rt_tbl->idx;
		if (ip == IPA_IP_v4) {
			req->filter_spec_ex_list[pos].ip_type = QMI_IPA_IP_TYPE_V4_V01;
			attrib.attrib_mask = IPA_FLT_META_DATA|IPA_FLT_PROTOCOL|IPA_FLT_SRC_PORT;
			attrib.u.v4.protocol = IPPROTO_UDP;
		} else {
			req->filter_spec_ex_list[pos].ip_type = QMI_IPA_IP_TYPE_V6_V01;
			attrib.attrib_mask = IPA_FLT_META_DATA|IPA_FLT_NEXT_HDR|IPA_FLT_SRC_PORT;
			attrib.u.v6.next_hdr = IPPROTO_UDP;
		}
		attrib.src_port = 4500;
		attrib.meta_data = mux_id << WWAN_METADATA_SHFT;
		attrib.meta_data_mask = WWAN_METADATA_MASK;
		rc = ipahal_flt_generate_equation(ip, &attrib, &eq_atrb);
		if (!!rc)
			return rc;
		memcpy(&req->filter_spec_ex_list[pos].filter_rule, &eq_atrb,
			sizeof(struct ipa_filter_rule_type_v01));
		pos++;
	}

	return 0;
}
EXPORT_SYMBOL(ipa_ipsec_install_qmi_flt);

void ipa_ipsec_ep_init_cons(struct work_struct *work)
{
	u32 clnt_hdl;
	static bool allocated = false;
	struct ipa_sys_connect_params sys_in;

	IPADBG("Start\n");

	/* This may be called number of times from rmnet_ipa */
	if (allocated)
		return;

	WARN_ON(!ipa3_ctx->uc_ctx.uc_loaded);
	if (!ipa3_ctx->uc_ctx.uc_event_ring_valid && ipa3_uc_setup_event_ring()) {
		IPAERR("failed to set uc_event ring\n");
		return;
	}

	/* We want to set up the IPsec consumer pipes, only if the feature is enabled.
	   Otherwise we only need the uC event ring, because uC will generate the IV ready event. */
	if (!ipa_ipsec_enabled()) {
		IPADBG("uc_event ring has been set up\n");
		return;
	}

	memset(&sys_in, 0, sizeof(struct ipa_sys_connect_params));

	/* IPsec decap recoverable error (IPA->AP) */
	sys_in.client = IPA_CLIENT_IPSEC_DECAP_RECOVERABLE_ERR_CONS;
	sys_in.notify = apps_ipa_ipsec_err_pkt_rcv_ntfy;
	sys_in.priv = ipa3_ctx->ipsec->dev;
	sys_in.desc_fifo_sz = 256 * IPA_FIFO_ELEMENT_SIZE;
	sys_in.ext_ioctl_v2 = true;
	sys_in.int_modt = 16;
	sys_in.int_modc = 20;
	sys_in.buff_size = 4096;
	sys_in.bypass_agg = true;
	sys_in.ipa_ep_cfg.hdr.hdr_len = 12;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_metadata_valid = 1;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_metadata = 9;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_pkt_size_valid = 1;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_pkt_size = 2;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_total_len_or_pad_valid = true;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_total_len_or_pad = 0;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_payload_len_inc_padding = true;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_total_len_or_pad_offset = 0;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_little_endian = 0;
	sys_in.ipa_ep_cfg.metadata_mask.metadata_mask = 0xF;
	sys_in.ipa_ep_cfg.prod_cfg.error_qmap_en = true;
	if (ipa_setup_sys_pipe(&sys_in, &clnt_hdl)) {
		IPAERR(":setup sys pipe (%s) failed.\n", ipa_clients_strings[sys_in.client]);
		return;
	}

	/* IPsec decap non-recoverable error (IPA->AP) */
	sys_in.client = IPA_CLIENT_IPSEC_DECAP_NON_RECOVERABLE_ERR_CONS;
	if (ipa_setup_sys_pipe(&sys_in, &clnt_hdl)) {
		IPAERR(":setup sys pipe (%s) failed.\n", ipa_clients_strings[sys_in.client]);
		return;
	}

	/* IPsec encap error (IPA->AP) */
	sys_in.client = IPA_CLIENT_IPSEC_ENCAP_ERR_CONS;
	if (ipa_setup_sys_pipe(&sys_in, &clnt_hdl)) {
		IPAERR(":setup sys pipe (%s) failed.\n", ipa_clients_strings[sys_in.client]);
		return;
	}

	/* IPsec embedded RX (IPA->AP) */
	memset(&sys_in, 0, sizeof(struct ipa_sys_connect_params));
	sys_in.client = IPA_CLIENT_IPSEC_APPS_WAN_CONS;
	sys_in.notify = apps_ipa_packet_receive_notify;
	sys_in.priv = ipa3_ctx->ipsec->dev;
	sys_in.desc_fifo_sz = 256 * IPA_FIFO_ELEMENT_SIZE;
	sys_in.ext_ioctl_v2 = true;
	sys_in.int_modt = 16;
	sys_in.int_modc = 20;
	sys_in.buff_size = 4096;
	sys_in.bypass_agg = true;
	sys_in.ipa_ep_cfg.hdr.hdr_len = 8;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_metadata_valid = 1;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_metadata = 1;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_pkt_size_valid = 1;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_pkt_size = 2;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_total_len_or_pad_valid = true;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_total_len_or_pad = 0;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_payload_len_inc_padding = true;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_total_len_or_pad_offset = 0;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_little_endian = 0;
	sys_in.ipa_ep_cfg.metadata_mask.metadata_mask = 0xFF000000;
	if (ipa_setup_sys_pipe(&sys_in, &clnt_hdl)) {
		IPAERR(":setup sys pipe (%s) failed.\n", ipa_clients_strings[sys_in.client]);
		return;
	}

	allocated = true;
	return;
}
EXPORT_SYMBOL(ipa_ipsec_ep_init_cons);

/* Fill sec_path skb member to make network stack accept the HW decapsulated packet
 * Use SA index from the metadata, filled by uC */
int ipa_ipsec_rx_update_sec_path(struct sk_buff *skb, u32 metadata)
{
	int idx;
	struct xfrm_state *x;
	struct xfrm_offload *xo;
	struct sec_path *sp;

	if (!(metadata & META_IS_IPSEC)) {
		IPADBG_LOW("No META_IS_IPSEC\n");
		return 0;
	}

	idx = (metadata & META_SA_MASK) >> META_SA_SHIFT;

	IPADBG_LOW("META_IS_IPSEC, idx = %d\n", idx);

	if (unlikely(idx >= IPA_IPSEC_MAX_SA_NUM)) {
		IPAERR_RL("Invalid IPsec info in the metadata (0x%X) \n", metadata);
		return -EINVAL;
	}

	atomic_inc(&ipa3_ctx->ipsec->stats.decap_stats[idx].ipsec_decap_rcv);

	x = ipa3_ctx->ipsec->sa_db[IPA_IPSEC_DECAP][idx].x;
	if (unlikely(!x)) {
		IPAERR_RL("Decap SA%02d has no XFRM state pointer (0x%p) \n", idx, x);
		return -EINVAL;
	}

	sp = secpath_set(skb);
	if (unlikely(!sp)) {
		IPAERR_RL("Failed setting the sec_path\n");
		return -EFAULT;
	}

	xfrm_state_hold(x);

	sp->xvec[sp->len++] = x;
	sp->olen++;

	xo = xfrm_offload(skb);
	if (unlikely(!xo)) {
		IPAERR_RL("skb is missing xfrm_offload pointer\n");
		return -EINVAL;
	}
	xo->flags = CRYPTO_DONE;
	xo->status = CRYPTO_SUCCESS;

	/* Record queue IPA_RMNET_RX_QUEUE_IPSEC for DL IPsec offloaded traffic */
	skb_record_rx_queue(skb, IPA_RMNET_RX_QUEUE_IPSEC);

	return 0;
}
EXPORT_SYMBOL(ipa_ipsec_rx_update_sec_path);

/* Install SA mismatch error Qmap header */
static int ipa_ipsec_setup_sa_mismatch_err_qmap_hdr(void)
{
	struct ipa_ioc_add_hdr *hdr;
	struct ipa_hdr_add *hdr_entry;
	struct error_qmap_hdr sa_mismatch_err_qmap_hdr = {0};
	u32 pyld_sz;
	int ret;

	pyld_sz = sizeof(struct ipa_ioc_add_hdr) + 1 *
			sizeof(struct ipa_hdr_add);
	hdr = kzalloc(pyld_sz, GFP_KERNEL);
	if (!hdr) {
		IPAERR("fail to alloc hdr with %d bytes\n", pyld_sz);
		return -ENOMEM;
	}

	hdr->num_hdrs = 1;
	hdr->commit = 1;
	hdr_entry = &hdr->hdr[0];

	strlcpy(hdr_entry->name, IPA_IPSEC_SA_MISMATCH_ERR_QMAP_HDR_NAME,
				IPA_RESOURCE_NAME_MAX);

	/* Fill Inner packet SA mismatch error QMAP header */
	sa_mismatch_err_qmap_hdr.next_hdr = 1;
	sa_mismatch_err_qmap_hdr.hdr_type = 4; // Error info
	sa_mismatch_err_qmap_hdr.error_type = IPA_IPSEC_ERROR_TYPE_DECAP;
	sa_mismatch_err_qmap_hdr.error_code = IPA_IPSEC_ERROR_CODE_INNER_PKT_SA_MISMATCH;

	memcpy(hdr_entry->hdr, &sa_mismatch_err_qmap_hdr, sizeof(sa_mismatch_err_qmap_hdr));
	hdr_entry->hdr_len = sizeof(sa_mismatch_err_qmap_hdr);

	if (ipa_add_hdr(hdr)) {
		IPAERR("fail to add SA mismatch error QMAP header\n");
		ret = -EPERM;
		goto bail;
	}

	ipa3_ctx->ipsec->sa_mismatch_qmap_hdr_hdl = hdr_entry->hdr_hdl;

	ret = 0;
bail:
	kfree(hdr);
	return ret;
}

/* Install static IKE and IPsec FnR */
static int ipa_ipsec_fnr_init(void)
{
	int ret = 0;
	enum ipa_ip_type ip;
	struct ipa_ioc_add_hdr_proc_ctx *proc_ctxs = NULL;
	struct ipa_hdr_proc_ctx_add *proc_ctx_add = NULL;
	struct ipa_ioc_add_flt_rule_v2 *flt_tbl = NULL;
	struct ipa_flt_rule_add_v2 *flt_rule = NULL;
	struct ipa_ioc_add_rt_rule_v2 *rt_tbl = NULL;
	struct ipa_rt_rule_add_v2 *rt_rule = NULL;
	struct ipa_ioc_get_rt_tbl rt_lookup;

	IPADBG("Start\n");

	proc_ctxs = (struct ipa_ioc_add_hdr_proc_ctx *)kzalloc(
		sizeof(struct ipa_ioc_add_hdr_proc_ctx) + sizeof(struct ipa_hdr_proc_ctx_add),
		GFP_KERNEL);
	if (!proc_ctxs) {
		IPAERR("Failed to allocate ipa_ioc_add_hdr_proc_ctx\n");
		ret = -ENOMEM;
		return ret;
	}
	proc_ctxs->commit = 1;
	proc_ctxs->num_proc_ctxs = 1;
	proc_ctx_add = &proc_ctxs->proc_ctx[0];
	proc_ctx_add->proc_ctx_hdl = -1;
	proc_ctx_add->status = -1;

	flt_tbl = kzalloc(sizeof(*flt_tbl), GFP_KERNEL);
	if (!flt_tbl) {
		IPAERR("Failed to allocate ipa_ioc_add_flt_rule_v2\n");
		ret = -ENOMEM;
		goto fail_flt;
	}
	flt_tbl->rules = (uint64_t)kzalloc(sizeof(struct ipa_flt_rule_add_v2), GFP_KERNEL);
	if (!flt_tbl->rules) {
		IPAERR("Failed to allocate ipa_flt_rule_add_v2\n");
		ret = -ENOMEM;
		goto fail_flt_rules;
	}
	flt_tbl->commit = 1;
	flt_tbl->num_rules = 1;
	flt_tbl->flt_rule_size = sizeof(struct ipa_flt_rule_add_v2);

	rt_tbl = (struct ipa_ioc_add_rt_rule_v2 *)kzalloc(sizeof(*rt_rule), GFP_KERNEL);
	if (!rt_tbl) {
		IPAERR("Failed to allocate ipa_ioc_add_rt_rule_v2\n");
		ret = -ENOMEM;
		goto fail_rt;
	}
	rt_tbl->rules = (uint64_t)kzalloc(2 * sizeof(struct ipa_rt_rule_add_v2), GFP_KERNEL);
	if (!rt_tbl->rules) {
		IPAERR("Failed to allocate ipa_rt_rule_add_v2\n");
		ret = -ENOMEM;
		goto fail_rt_rules;
	}
	rt_tbl->commit = 1;
	rt_tbl->rule_add_size = sizeof(struct ipa_rt_rule_add_v2);

	/* Install next-round HPC for encap catch all and IKE RT rules */
	proc_ctx_add->type = IPA_HDR_PROC_NXT_RND;
	proc_ctx_add->ipsec_params.flt_tbl_id = IPA_CLIENT_APPS_WAN_PROD;

	ret = ipa3_add_hdr_proc_ctx(proc_ctxs, false);
	if (!!ret) {
		IPAERR("ipa3_add_hdr_proc_ctx returned %d\n", ret);
		goto end;
	}

	IPADBG_LOW("proc_ctx_add->proc_ctx_hdl = %d\n", proc_ctx_add->proc_ctx_hdl);
	ipa3_ctx->ipsec->ul_hpc = proc_ctx_add->proc_ctx_hdl;

	/* Install encap RT tables */
	rt_tbl->num_rules = 1;
	for (ip = IPA_IP_v4; ip < IPA_IP_MAX; ip++) {
		rt_tbl->ip = ip;
		strlcpy(rt_tbl->rt_tbl_name, __ipa_ipsec_s.encap_rt[ip], IPA_RESOURCE_NAME_MAX);

		/* Catch all */
		rt_rule = &(((struct ipa_rt_rule_add_v2 *)rt_tbl->rules)[0]);
		rt_rule->at_rear = 1;
		rt_rule->rt_rule_hdl = -1;
		rt_rule->status = -1;
		rt_rule->rule.dst = IPA_CLIENT_APPS_LAN_CONS;
		rt_rule->rule.hashable = true;
#ifdef CONFIG_IPA_IPSEC_NAT_TOPO
		/* In cases when the policy flt rule exists, but policy rt rule not yet,
		   we want the catch all rt rule to sent the packets to APPs  */
		rt_rule->rule.dst = IPA_CLIENT_MAX; /* translated into IPA_INVALID_PIPE_IDX */
		rt_rule->rule.hdr_proc_ctx_hdl = ipa3_ctx->ipsec->ul_hpc;
#endif

		ret = ipa3_add_rt_rule_v2(rt_tbl);
		if (!!ret) {
			IPAERR("ipa3_add_hdr_proc_ctx returned %d\n", ret);
			goto end;
		}

		rt_lookup.ip = rt_tbl->ip;
		strlcpy(rt_lookup.name, rt_tbl->rt_tbl_name, IPA_RESOURCE_NAME_MAX);
		ret = ipa3_get_rt_tbl(&rt_lookup);
		if (!!ret) {
			IPAERR("ipa3_get_rt_tbl returned %d\n", ret);
			goto end;
		}
		ipa3_ctx->ipsec->encap_rt[ip] = rt_lookup.hdl;
		IPADBG_LOW("encap_rt[IPv%d] = %d\n", ip == IPA_IP_v4 ? 4 : 6,
			ipa3_ctx->ipsec->encap_rt[ip]);

		if (ipa_put_rt_tbl(rt_lookup.hdl)) {
			IPAERR("ipa_put_rt_tbl() failure.\n");
		}
	}

	/* Install decap RT table */
	rt_tbl->num_rules = 2;
	memset((void *)(rt_tbl->rules), 0, 2 * sizeof(struct ipa_rt_rule_add_v2));
	for (ip = IPA_IP_v4; ip < IPA_IP_MAX; ip++) {
		rt_tbl->ip = ip;
		strlcpy(rt_tbl->rt_tbl_name, __ipa_ipsec_s.decap_rt[ip], IPA_RESOURCE_NAME_MAX);

		/* UDP 4500 + ESP after UDP */
		rt_rule = &(((struct ipa_rt_rule_add_v2 *)rt_tbl->rules)[0]);
		rt_rule->at_rear = 0;
		rt_rule->rt_rule_hdl = -1;
		rt_rule->status = -1;
		rt_rule->rule.dst = IPA_CLIENT_APPS_WAN_CONS;
		rt_rule->rule.hashable = true;
		rt_rule->rule.esp_after_udp = 1;
		if (ip == IPA_IP_v4) {
			rt_rule->rule.attrib.attrib_mask =
				IPA_FLT_PROTOCOL | IPA_FLT_SRC_PORT;
			rt_rule->rule.attrib.u.v4.protocol = IPPROTO_UDP;
		} else {
			rt_rule->rule.attrib.attrib_mask =
				IPA_FLT_NEXT_HDR | IPA_FLT_SRC_PORT;
			rt_rule->rule.attrib.u.v6.next_hdr = IPPROTO_UDP;
		}
		/* 4500 is the standard NAT-T port */
		rt_rule->rule.attrib.src_port = 4500;

		/* Catch all */
		rt_rule = &(((struct ipa_rt_rule_add_v2 *)rt_tbl->rules)[1]);
		rt_rule->at_rear = 1;
		rt_rule->rt_rule_hdl = -1;
		rt_rule->status = -1;
		rt_rule->rule.dst = IPA_CLIENT_APPS_WAN_CONS;
		rt_rule->rule.hashable = true;

		ret = ipa3_add_rt_rule_v2(rt_tbl);
		if (!!ret) {
			IPAERR("ipa3_add_rt_rule_v2 returned %d\n", ret);
			goto end;
		}
		rt_rule = &(((struct ipa_rt_rule_add_v2 *)rt_tbl->rules)[0]);
		ipa3_ctx->ipsec->decap_rt_rule_hdl[ip].esp_src_port_hdl = rt_rule->rt_rule_hdl;
		rt_rule = &(((struct ipa_rt_rule_add_v2 *)rt_tbl->rules)[1]);
		ipa3_ctx->ipsec->decap_rt_rule_hdl[ip].catchall_hdl = rt_rule->rt_rule_hdl;

		rt_lookup.ip = rt_tbl->ip;
		strlcpy(rt_lookup.name, rt_tbl->rt_tbl_name, IPA_RESOURCE_NAME_MAX);
		ret = ipa3_get_rt_tbl(&rt_lookup);
		if (!!ret) {
			IPAERR("ipa3_get_rt_tbl returned %d\n", ret);
			goto end;
		}
		ipa3_ctx->ipsec->decap_rt[ip] = rt_lookup.hdl;
		IPADBG_LOW("decap_rt[IPv%d] = %d\n", ip == IPA_IP_v4 ? 4 : 6,
			ipa3_ctx->ipsec->decap_rt[ip]);

		if (ipa_put_rt_tbl(rt_lookup.hdl)) {
			IPAERR("ipa_put_rt_tbl() failure.\n");
		}
	}

	ret = ipa_ipsec_setup_sa_mismatch_err_qmap_hdr();
	if (ret) {
		IPAERR("Failed to setup SA mismatch qmap header\n");
		goto end;
	}

	/* Install decap RT no-policy table */
	rt_tbl->num_rules = 1;
	memset((void *)(rt_tbl->rules), 0, sizeof(struct ipa_rt_rule_add_v2));
	for (ip = IPA_IP_v4; ip < IPA_IP_MAX; ip++) {
		rt_tbl->ip = ip;
		strlcpy(rt_tbl->rt_tbl_name, __ipa_ipsec_s.decap_no_policy_rt[ip], IPA_RESOURCE_NAME_MAX);

		/* Catch all with SA mismatch error QMAP header */
		rt_rule = &(((struct ipa_rt_rule_add_v2 *)rt_tbl->rules)[0]);
		rt_rule->at_rear = 1;
		rt_rule->rt_rule_hdl = -1;
		rt_rule->status = -1;
		rt_rule->rule.dst = IPA_CLIENT_IPSEC_DECAP_NON_RECOVERABLE_ERR_CONS;
		rt_rule->rule.hashable = true;
		rt_rule->rule.hdr_hdl = ipa3_ctx->ipsec->sa_mismatch_qmap_hdr_hdl;

		ret = ipa3_add_rt_rule_v2(rt_tbl);
		if (!!ret)
			goto end;

		rt_lookup.ip = rt_tbl->ip;
		strlcpy(rt_lookup.name, rt_tbl->rt_tbl_name, IPA_RESOURCE_NAME_MAX);
		ret = ipa3_get_rt_tbl(&rt_lookup);
		if (!!ret)
			goto end;
		ipa3_ctx->ipsec->decap_no_policy_rt[ip] = rt_lookup.hdl;
		IPAERR("decap_no_policy_rt[IPv%d] = %d\n", ip == IPA_IP_v4 ? 4 : 6,
			ipa3_ctx->ipsec->decap_no_policy_rt[ip]);
	}


	/* Catch all - for all FLT tables */
	flt_rule = &(((struct ipa_flt_rule_add_v2 *)flt_tbl->rules)[0]);
	flt_rule->at_rear = 1;
	flt_rule->rule.action = IPA_PASS_TO_ROUTING;
	flt_rule->rule.hashable = true;

	/* Install embedded encap FLT table */
	flt_tbl->ep = IPA_CLIENT_IPSEC_ENCAP_PROD;
	for (ip = IPA_IP_v4; ip < IPA_IP_MAX; ip++) {
		flt_tbl->ip = ip;
		flt_rule->flt_rule_hdl = -1;
		flt_rule->status = -1;
		flt_rule->rule.rt_tbl_hdl = ipa3_ctx->ipsec->encap_rt[ip];

		ret = ipa3_add_flt_rule_usr_v2(flt_tbl, false);
		if (!!ret) {
			IPAERR("ipa3_add_flt_rule_usr_v2 returned %d\n", ret);
			goto end;
		}
		IPADBG_LOW("IPSEC_ENCAP_PROD: IPv%d flt_rule->status = %d flt_rule->flt_rule_hdl = %d\n",
			ip == IPA_IP_v4 ? 4 : 6, flt_rule->status, flt_rule->flt_rule_hdl);
	}

	/* Install embedded decap FLT table */
	flt_tbl->ep = IPA_CLIENT_IPSEC_DECAP_PROD;
	memset((void *)flt_rule, 0, sizeof(struct ipa_flt_rule_add_v2));
	for (ip = IPA_IP_v4; ip < IPA_IP_MAX; ip++) {
		flt_tbl->ip = ip;
		flt_rule->flt_rule_hdl = -1;
		flt_rule->status = -1;
		flt_rule->rule.rt_tbl_hdl = ipa3_ctx->ipsec->decap_rt[ip];

		ret = ipa3_add_flt_rule_usr_v2(flt_tbl, false);
		if (!!ret) {
			IPAERR("ipa3_add_flt_rule_usr_v2 returned %d\n", ret);
			goto end;
		}
		IPADBG_LOW("IPSEC_DECAP_PROD: IPv%d flt_rule->status = %d flt_rule->flt_rule_hdl = %d\n",
			ip == IPA_IP_v4 ? 4 : 6, flt_rule->status, flt_rule->flt_rule_hdl);
	}

	/* Install decap policy FLT table - is done in rmnet_ipa (ipa_ipsec_install_dl_pol_flt) */
	ipa3_ctx->ipsec->default_rt = NULL;

end:
	kfree((void *)rt_tbl->rules);
fail_rt_rules:
	kfree(rt_tbl);
fail_rt:
	kfree((void *)flt_tbl->rules);
fail_flt_rules:
	kfree(flt_tbl);
fail_flt:
	kfree(proc_ctxs);

	IPADBG_LOW("ret = %d\n", ret);
	return ret;
}

int ipa_ipsec_ep_init_prod(void)
{
	u32 clnt_hdl;
	static bool allocated = false;
	struct ipa_sys_connect_params sys_in;
	int ret = 0;

	IPADBG("Start\n");

	/* This may be called number of times from rmnet_ipa */
	if (allocated)
		return 0;

	/*Initilize the filter/route rules before IPSEC setup pipes*/
	ret = ipa_ipsec_fnr_init();
	if (ret != 0) {
		IPAERR("Failed to init IPsec FnR\n");
		return ret;
	}

	/* IPsec encap (AP->IPA) */
	memset(&sys_in, 0, sizeof(struct ipa_sys_connect_params));
	sys_in.client = IPA_CLIENT_IPSEC_ENCAP_PROD;
	sys_in.desc_fifo_sz = IPA_SYS_TX_DATA_DESC_FIFO_SZ;
	sys_in.ipa_ep_cfg.mode.mode = IPA_BASIC;
	sys_in.ipa_ep_cfg.mode.dst = IPA_CLIENT_IPSEC_ENCAP_PROD;
	sys_in.ipa_ep_cfg.cfg.cs_offload_en = IPA_DISABLE_CS_OFFLOAD;
	sys_in.ipa_ep_cfg.aggr.aggr_en = IPA_BYPASS_AGGR;
	sys_in.ipa_ep_cfg.hdr.hdr_len = 4;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_metadata_valid = 1;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_metadata = 0;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_pkt_size = 2;
	sys_in.notify = apps_ipa_tx_complete_notify;
	sys_in.priv = ipa3_ctx->ipsec->dev;
	sys_in.ext_ioctl_v2 = true;
	sys_in.int_modt = 16;
	sys_in.int_modc = 20;
	if (ipa_setup_sys_pipe(&sys_in, &clnt_hdl)) {
		IPAERR(":setup sys pipe (%s) failed.\n", ipa_clients_strings[sys_in.client]);
		return -EPERM;
	}

	/* IPsec decap (AP->IPA) */
	memset(&sys_in, 0, sizeof(struct ipa_sys_connect_params));
	sys_in.client = IPA_CLIENT_IPSEC_DECAP_PROD;
	sys_in.desc_fifo_sz = IPA_SYS_TX_DATA_DESC_FIFO_SZ;
	sys_in.ipa_ep_cfg.mode.mode = IPA_BASIC;
	sys_in.ipa_ep_cfg.mode.dst = IPA_CLIENT_IPSEC_DECAP_PROD;
	sys_in.ipa_ep_cfg.cfg.cs_offload_en = IPA_DISABLE_CS_OFFLOAD;
	sys_in.ipa_ep_cfg.aggr.aggr_en = IPA_BYPASS_AGGR;
	sys_in.ipa_ep_cfg.hdr.hdr_len = 0;
	sys_in.notify = apps_ipa_tx_complete_notify;
	sys_in.priv = ipa3_ctx->ipsec->dev;
	sys_in.ext_ioctl_v2 = true;
	sys_in.int_modt = 16;
	sys_in.int_modc = 20;
	if (ipa_setup_sys_pipe(&sys_in, &clnt_hdl)) {
		IPAERR(":setup sys pipe (%s) failed.\n", ipa_clients_strings[sys_in.client]);
		return -EPERM;
	}

	allocated = true;
	return 0;
}
EXPORT_SYMBOL(ipa_ipsec_ep_init_prod);


/* Map uC SMMU for encap SAs. To be used by uC for NextIV WA */
static int ipa_ipsec_map_uc_smmu(phys_addr_t pa, unsigned long *iova)
{
	struct ipa_smmu_cb_ctx *cb = ipa3_get_smmu_ctx(IPA_SMMU_CB_UC);
	unsigned long va = roundup(cb->next_addr, PAGE_SIZE);
	size_t len = roundup(IPA_ENCAP_DB_SIZE + pa - rounddown(pa, PAGE_SIZE), PAGE_SIZE);
	int ret;

	if (!cb->valid) {
		IPAERR("The uC SMMU is not set up\n");
		return -EINVAL;
	}

	ret = ipa3_iommu_map(cb->iommu_domain, va, rounddown(pa, PAGE_SIZE), len,
		IOMMU_READ|IOMMU_WRITE|IOMMU_MMIO);
	if (ret) {
		IPAERR("iommu map failed for pa=%pa len=%zu\n", &pa, IPA_ENCAP_DB_SIZE);
		return -EINVAL;
	}

	cb->next_addr = va + len;
	*iova = va + pa - rounddown(pa, PAGE_SIZE);
	return 0;
}

/* Unmap uC SMMU for encap SAs */
static void ipa_ipsec_unmap_uc_smmu(phys_addr_t pa, unsigned long iova)
{
	struct ipa_smmu_cb_ctx *cb = ipa3_get_smmu_ctx(IPA_SMMU_CB_UC);

	iommu_unmap(cb->iommu_domain, rounddown(iova, PAGE_SIZE),
		roundup(IPA_ENCAP_DB_SIZE + pa - rounddown(pa, PAGE_SIZE), PAGE_SIZE));
}

/*
 * ipa_ipsec_init()
 */
int ipa_ipsec_init(void)
{
	int ret;
	u32 keys_phys_base, sa_phys_base;
	void __iomem *key_mmio, __iomem *sa_mmio;
	unsigned long uc_smmu_iova;

	IPADBG("IPA IPsec entry\n");

	if (!ipa3_ctx || !ipa3_ctx->ctrl) {
		IPAERR("Main IPA driver is not ready yet.\n");
		return -EAGAIN;
	}

	if (ipa3_ctx->ipa_tiering_value & IPA_TIERING_DISABLE_IPSEC) {
		IPADBG("IPsec offload is not enabled on this platform\n");
		return 0;
	}

	if (ipa3_ctx->ipsec) {
		IPAERR("Already initialized\n");
		return -EEXIST;
	}

	/* Allocate context */
	ipa3_ctx->ipsec = kzalloc(sizeof(struct ipa_ipsec_ctx), GFP_KERNEL);
	if (!ipa3_ctx->ipsec) {
		IPAERR("Failed allocatng IPsec context.\n");
		return -ENOMEM;
	}
	ipa3_ctx->ipsec->xfrmdev_ops =
		(struct xfrmdev_ops *)kzalloc(sizeof(struct xfrmdev_ops), GFP_KERNEL);
	if (!ipa3_ctx->ipsec->xfrmdev_ops) {
		IPAERR("Failed allocating xfrmdev_ops.\n");
		ret = -ENOMEM;
		goto free_ctx;
	}

	ipa3_ctx->ipsec->xfrmdev_ops->xdo_dev_state_add = ipa_ipsec_xdo_state_add;
	ipa3_ctx->ipsec->xfrmdev_ops->xdo_dev_state_delete = ipa_ipsec_xdo_state_delete;
	ipa3_ctx->ipsec->xfrmdev_ops->xdo_dev_state_free = ipa_ipsec_xdo_state_free;
	ipa3_ctx->ipsec->xfrmdev_ops->xdo_dev_offload_ok = ipa_ipsec_xdo_offload_ok;
	ipa3_ctx->ipsec->xfrmdev_ops->xdo_dev_state_advance_esn = ipa_ipsec_xdo_state_advance_esn;
	ipa3_ctx->ipsec->xfrmdev_ops->xdo_dev_state_update_curlft = ipa_ipsec_xdo_state_update_curlft;
	ipa3_ctx->ipsec->xfrmdev_ops->xdo_dev_policy_add = ipa_ipsec_xdo_policy_add;
	ipa3_ctx->ipsec->xfrmdev_ops->xdo_dev_policy_delete = ipa_ipsec_xdo_policy_delete;
	ipa3_ctx->ipsec->xfrmdev_ops->xdo_dev_policy_free = ipa_ipsec_xdo_policy_free;

	atomic_set(&ipa3_ctx->stats.ipsec_enacp_excp, 0);
	atomic_set(&ipa3_ctx->stats.ipsec_decap_excp, 0);

	INIT_LIST_HEAD(&ipa3_ctx->ipsec->pol_list);
	mutex_init(&ipa3_ctx->ipsec->pol_list_lock);

	/* Init SA threshold workqueue */
	ipa_ipsec_wq = create_singlethread_workqueue(IPSEC_WORKQUEUE_NAME);
	if (!ipa_ipsec_wq) {
		IPAERR("IPA IPsec workqueue creation failed\n");
		ret = -ENOMEM;
		goto free_xfrmdev_ops;
	}

	/* Map IPA IPsec Key SRAM */
	keys_phys_base = ipa3_ctx->ipa_wrapper_base + ipa3_ctx->ctrl->ipa_reg_base_ofst +
		ipahal_get_reg_n_ofst(IPA_IPSEC_AREA_RAM_DIRECT_ACCESS_n, 0);

	key_mmio = ioremap((phys_addr_t)keys_phys_base, sizeof(struct ipa_ipsec_key_store));
	if (!key_mmio) {
		IPAERR("Failed mapping IPsec key SRAM.\n");
		ret = -ENOMEM;
		goto free_wq;
	}

	IPADBG_LOW("keys_phys_base 0x%08X key_mmio=0x%p\n", keys_phys_base, key_mmio);
	memset_io(key_mmio, 0, sizeof(struct ipa_ipsec_key_store));
	ipa3_ctx->ipsec->keys = (struct ipa_ipsec_key_store *)key_mmio;

	/* Compile time check the SA SRAM partition size and alignment */
	WARN_ON(IPA_MEM_PART(sa_contexts_size) < IPA_SA_DB_SIZE ||
		IPA_MEM_PART(sa_contexts_ofst) & 0x3);

	/* Map the SA database SRAM region */
	sa_phys_base = ipa3_ctx->ipa_wrapper_base +
		ipa3_ctx->ctrl->ipa_reg_base_ofst +
		ipahal_get_reg_n_ofst(IPA_SW_AREA_RAM_DIRECT_ACCESS_n,
			ipa3_ctx->smem_restricted_bytes / 4) +
		IPA_MEM_PART(sa_contexts_ofst);

	sa_mmio = ioremap((phys_addr_t)sa_phys_base, IPA_SA_DB_SIZE);
	if (!sa_mmio) {
		IPAERR("Failed mapping IPsec SA SRAM.\n");
		ret = -ENOMEM;
		goto unmap_keys;
	}

	ret = ipa_ipsec_map_uc_smmu(sa_phys_base + IPA_DECAP_DB_SIZE, &uc_smmu_iova);
	if (ret != 0) {
		IPAERR("Failed to map encap SA SMMU for uC\n");
		goto unmap_sa;
	}
	ipa3_ctx->ipsec->uc_smmu_iova = uc_smmu_iova;

	/* Zero the SA SRAM */
	memset_io(sa_mmio, 0, IPA_SA_DB_SIZE);

	IPADBG_LOW("sa_phys_base 0x%08X sa_mmio=0x%p\n", sa_phys_base, sa_mmio);
	ipa3_ctx->ipsec->decap = (struct ipa_ipsec_sa_decap *)sa_mmio;
	ipa3_ctx->ipsec->encap =
		(struct ipa_ipsec_sa_encap *)(ipa3_ctx->ipsec->decap + IPA_IPSEC_MAX_SA_NUM);
	IPADBG_LOW("ipa3_ctx->ipsec->decap=0x%p ipa3_ctx->ipsec->encap=0x%p\n",
		ipa3_ctx->ipsec->decap, ipa3_ctx->ipsec->encap);

	/*
	 * Configure IPA_IPSEC_SA_DECAPSULATION_BASE with address in SW SRAM address space.
	 * This register points to the start of the decapsulation SAs.
	 */
	ipahal_write_reg(IPA_IPSEC_SA_DECAPSULATION_BASE, IPA_MEM_PART(sa_contexts_ofst));

	/*
	 * Configure IPA_IPSEC_SA_ENCAPSULATION_BASE with address in SW SRAM address space.
	 * This register points to the start of the encapsulation SAs.
	 */
	ipahal_write_reg(IPA_IPSEC_SA_ENCAPSULATION_BASE, IPA_MEM_PART(sa_contexts_ofst) + IPA_DECAP_DB_SIZE);

	/* Initialyze the default MTU values */
	ipa3_ctx->ipsec->mtu_v4 = MTU_BYTE;
	ipa3_ctx->ipsec->mtu_v6 = MTU_BYTE;
	ipa3_ctx->ipsec->initialized = true;

	return 0;

unmap_sa:
	iounmap(sa_mmio);
unmap_keys:
	iounmap(key_mmio);
free_wq:
	destroy_workqueue(ipa_ipsec_wq);
	ipa_ipsec_wq = NULL;
free_xfrmdev_ops:
	mutex_destroy(&ipa3_ctx->ipsec->pol_list_lock);
	kfree(ipa3_ctx->ipsec->xfrmdev_ops);
free_ctx:
	kfree(ipa3_ctx->ipsec);
	ipa3_ctx->ipsec = NULL;

	return ret;
}

int ipa_ipsec_enable(void)
{
	int n;
	u32 ipsec_ep_cfg;

	if (!ipa3_ctx->ipa_config_is_ipsec) {
		IPADBG("IPsec offload is not enabled from ipa_config.txt\n");
		return -ENXIO;
	}

	if (!ipa3_ctx->ipsec) {
		IPADBG("IPSEC not initialized\n");
		return -ENXIO;
	}

	/*
	 * Configure IPA_ENDP_INIT_IPSEC_CFG_%n%::EXCEPTION_ENDP_IPSEC_POST_ENCAPS
	 * with post encapsulation exception pipe.
	 * All packets that have been successfully encapsulated but received a HW Exception or
	 * Packet error will be routed to the pipe, and reverted to the last snapshot.
	 *
	 * We do that in the ipa_ipsec_enable() to avoid potential packet routing to
	 * an unconfigured pipe.
	 */

	/*
	 * Configure IPA_ENDP_INIT_IPSEC_CFG_%n%:
	 *  - EXCEPTION_ENDP_IPSEC_DECAP field with exception producer to route IPsec packets
	 *  that failed in side DECAPS acl.
	 *  - EXCEPTION_ENDP_IPSEC_POST_DECAPS field with Indicates exception producer
	 *  to route IPsec packets that have passed DECAPS acl.
	 *  - DECAPS_NEXT_HDR_CHECK_DISABLE field with 1 to disable checking
	 *  for Next-header errors for traffic from matching consumer.
	 */
	ipsec_ep_cfg = (u32)(ipa_get_ep_mapping(IPA_CLIENT_IPSEC_DECAP_NON_RECOVERABLE_ERR_CONS)) |
		(u32)(ipa_get_ep_mapping(IPA_CLIENT_IPSEC_DECAP_RECOVERABLE_ERR_CONS)) << 8 |
		(u32)(ipa_get_ep_mapping(IPA_CLIENT_IPSEC_ENCAP_ERR_CONS)) << 16;
	IPA_ACTIVE_CLIENTS_INC_SIMPLE();
	for (n = 0; n < IPA6_PROD_PIPES_NUM; n++) {
		ipahal_write_reg_n(IPA_ENDP_INIT_IPSEC_CFG_n, n, ipsec_ep_cfg);
	}
	IPA_ACTIVE_CLIENTS_DEC_SIMPLE();

	/* Update RMNET netdev */
	if (ipa3_ctx->ipsec->dev) {
		IPAWANDBG("IPsec offload is enabled\n");
		ipa3_ctx->ipsec->dev->features |= NETIF_F_HW_ESP;
		ipa3_ctx->ipsec->dev->hw_enc_features |= NETIF_F_HW_ESP;
		if (rtnl_trylock()) {
			netdev_update_features(ipa3_ctx->ipsec->dev);
			rtnl_unlock();
		}
		else {
			IPADBG("Unable to lock mutex to call netdev_update_features\n");
		}
	}

	ipa3_ctx->ipsec->enabled = true;
	return 0;
}

/* Clean up all IPsec allocations. To be called in case of the IPA driver unload. */
void ipa_ipsec_cleanup(void)
{
	u32 sa_phys_base;

	/* Not initialized or already cleaned */
	if (!ipa3_ctx->ipsec)
		return;

	IPA_ACTIVE_CLIENTS_INC_SIMPLE();
	/* Zero the SA and keys SRAM to avoid IPsec HW execution and for better security */
	memset_io(ipa3_ctx->ipsec->decap, 0, IPA_SA_DB_SIZE);
	memset_io(ipa3_ctx->ipsec->keys, 0, sizeof(struct ipa_ipsec_key_store));
	IPA_ACTIVE_CLIENTS_DEC_SIMPLE();

	sa_phys_base = ipa3_ctx->ipa_wrapper_base +
		ipa3_ctx->ctrl->ipa_reg_base_ofst +
		ipahal_get_reg_n_ofst(IPA_SW_AREA_RAM_DIRECT_ACCESS_n,
			ipa3_ctx->smem_restricted_bytes / 4) +
		IPA_MEM_PART(sa_contexts_ofst);

	/* Unmap uC SMMU */
	ipa_ipsec_unmap_uc_smmu(sa_phys_base + IPA_DECAP_DB_SIZE, ipa3_ctx->ipsec->uc_smmu_iova);

	/* Unmap SA and keys SRAM */
	iounmap(ipa3_ctx->ipsec->decap);
	iounmap(ipa3_ctx->ipsec->keys);

	/* Free allocated RAM */
	destroy_workqueue(ipa_ipsec_wq);
	ipa_ipsec_wq = NULL;
	destroy_workqueue(ipa_uc_ipsec_wq);
	ipa_uc_ipsec_wq = NULL;

	mutex_destroy(&ipa3_ctx->ipsec->pol_list_lock);

	kfree(ipa3_ctx->ipsec->xfrmdev_ops);
	kfree(ipa3_ctx->ipsec);
	ipa3_ctx->ipsec = NULL;
}

