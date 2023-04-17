/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#ifndef _IPA_IPSEC_H_
#define _IPA_IPSEC_H_

#include <linux/types.h>

#define IPA_IPSEC_MAX_SA_NUM 11
#define IPA_IPSEC_MAX_ENACAP_KEY_NUM 16
#define IPA_IPSEC_MAX_DEACAP_KEY_NUM 16
#define IPA_IPSEC_MAX_KEY_NUM (IPA_IPSEC_MAX_ENACAP_KEY_NUM + IPA_IPSEC_MAX_DEACAP_KEY_NUM)

enum ipa_ipsec_key_type {
	IPA_IPSEC_KEY_ENC,
	IPA_IPSEC_KEY_AUTH,
	IPA_IPSEC_KEY_MAX,
};

enum ipa_ipsec_key_len {
	IPA_IPSEC_KEY_128 = 1,
	IPA_IPSEC_KEY_256,
	IPA_IPSEC_KEY_512,
	IPA_IPSEC_KEY_160,
	IPA_IPSEC_KEY_384,
};

enum ipa_ipsec_sa_type {
	IPA_IPSEC_ENCAP,
	IPA_IPSEC_DECAP,
	IPA_IPSEC_TYPE_MAX,
};

#pragma pack(push, 4)
/* Encap SA Shared, Apps writes, uC and HW read */
struct ipa_ipsec_sa_encap_shared {
	u32 salt_val;

	u32 encr_algo		:8;
	u32 encrkey_len		:3;
	u32 salt_needed		:1;
	u32 reserved1		:4;
	u32 iv_sz		:5;
	u32 reserved2		:11;

	u32 auth_algo		:8;
	u32 authkey_len		:3;
	u32 esn_en		:1;
	u32 reserved3		:4;
	u32 icv_sz		:7;
	u32 reserved4		:9;

	u32 reserved5;
};

/* Encap SA Dynamic, Apps reads/writes, uC reads/writes (not gated) */
struct ipa_ipsec_sa_encap_dynamic {
	u32 is_stopped		:1;
	u32 send_to_sw		:1;
	u32 reserved		:14;
	u32 ipv4_id		:16;
	u32 seq_overflow;
	u64 seq_num;
	u64 volume_bytes;
	u64 last_pkt_timestamp;
};

/* Encap SA Static, Apps writes, uC reads */
struct ipa_ipsec_sa_encap_static {
	u32 nat_t		:1;
	u32 copy_df		:1;
	u32 copy_dscp		:1;
	u32 copy_ecn		:1;
	u32 copy_flow_lbl	:1;
	u32 reserved1		:11;
	u32 path_mtu		:16;

	u64 sa_life_bytes_wm; /* soft threshold */
	u64 sa_life_bytes; /* hard threshold */
	u32 reserved2;
};

/* Encap SA Layout - 22B */
struct ipa_ipsec_sa_encap {
	struct {
		u32 iv[4];
	} intr;
	struct ipa_ipsec_sa_encap_shared shar;
	struct ipa_ipsec_sa_encap_dynamic dyna;
	struct ipa_ipsec_sa_encap_static stat;
};
#define IPA_ENCAP_DB_SIZE (IPA_IPSEC_MAX_SA_NUM * (sizeof(struct ipa_ipsec_sa_encap)))

/* Decap SA Intra, Apps reads/writes, HW writes/writes */
struct ipa_ipsec_sa_decap_intra {
	u32 intgr_fail;
	u32 pad_fail;
	u32 antirep_fail;
	u16 cons_intgr_fail;
	u16 reserved;
	u32 out_win_fail;
	u32 dummy_pkts;
	u64 antirep_win[16];
	u64 antirep_top;
	u64 antirep_bottom;
};

/* Decap SA Shared, Apps writes, uC and HW read */
struct ipa_ipsec_sa_decap_shared {
	u32 antirep_win_sz;
	u32 salt_val;

	u32 encr_algo		:8;
	u32 encrkey_len		:3;
	u32 salt_needed		:1;
	u32 no_pad_chk		:1;
	u32 reserved1		:3;
	u32 iv_sz		:5;
	u32 reserved2		:11;

	u32 auth_algo		:8;
	u32 authkey_len		:3;
	u32 esn_en		:1;
	u32 reserved3		:4;
	u32 icv_sz		:7;
	u32 reserved4		:9;

	u32 ecn_fld_lut;

	u32 ecn_expt_lut	:16;
	u32 reserved5		:15;
	u32 ecn_upd		:1;
};

/* Decap SA Dynamic, Apps reads/writes, uC reads/writes (not gated) */
struct ipa_ipsec_sa_decap_dynamic {
	u32 is_stopped		:1;
	u32 send_to_sw		:1;
	u32 reserved		:30;

	u64 volume_bytes;
	u64 last_pkt_timestamp;
	u32 last_prim_frag_seq_num;
};

/* Decap SA Static, Apps writes, uC reads */
struct ipa_ipsec_sa_decap_static {
	u32 nat_t		:1;
	u32 reserved1		:31;

	u64 sa_life_bytes_wm; /* soft threshold */
	u64 sa_life_bytes; /* hard threshold */
	u32 reserved2;
};

/* Decap SA Layout - 30B */
struct ipa_ipsec_sa_decap {
	struct ipa_ipsec_sa_decap_intra intr;
	struct ipa_ipsec_sa_decap_shared shar;
	struct ipa_ipsec_sa_decap_dynamic dyna;
	struct ipa_ipsec_sa_decap_static stat;
};
#define IPA_DECAP_DB_SIZE (IPA_IPSEC_MAX_SA_NUM * (sizeof(struct ipa_ipsec_sa_decap)))
#define IPA_SA_DB_SIZE (IPA_DECAP_DB_SIZE + IPA_ENCAP_DB_SIZE)

union ipa_ipsec_enc_key {
	struct {
		u8 b[16];
		u8 reserved[16];
	} b128;

	struct {
		u8 b[32];
	} b256;
};

union ipa_ipsec_auth_key {
	struct {
		u8 b[16];
		u8 reserved[48];
	} b128;

	struct {
		u8 b[20];
		u8 reserved[44];
	} b160;

	struct {
		u8 b[32];
		u8 reserved[32];
	} b256;

	struct {
		u8 b[48];
		u8 reserved[16];
	} b384;

	struct {
		u8 b[64];
	} b512;
};

struct ipa_ipsec_key_store {
	union ipa_ipsec_enc_key enc[IPA_IPSEC_MAX_KEY_NUM];
	union ipa_ipsec_auth_key auth[IPA_IPSEC_MAX_KEY_NUM];
};

struct ipa_ipsec_ctx {
	struct ipa_ipsec_key_store *keys;
	struct ipa_ipsec_sa_decap *decap;
	struct ipa_ipsec_sa_encap *encap;
};
#pragma pack(pop)

int ipa_ipsec_install_key(u8 idx, enum ipa_ipsec_key_type type, enum ipa_ipsec_key_len, void *key);
int ipa_ipsec_install_encap_sa(u8 idx, struct ipa_ipsec_sa_encap *sa);
int ipa_ipsec_install_decap_sa(u8 idx, struct ipa_ipsec_sa_decap *sa);
int ipa_ipsec_stop_sa(u8 idx, enum ipa_ipsec_sa_type sa_type);

#endif /* _IPA_IPSEC_H_ */
