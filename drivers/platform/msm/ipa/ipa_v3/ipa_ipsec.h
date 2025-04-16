/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#ifndef _IPA_IPSEC_H_
#define _IPA_IPSEC_H_

#include <linux/types.h>
#include <linux/pfkeyv2.h>
#include <net/xfrm.h>
#include <linux/ipa_qmi_service_v01.h>

#define IPA_IPSEC_MAX_SA_NUM 11
#define IPA_IPSEC_MAX_ENACAP_KEY_NUM 16
#define IPA_IPSEC_MAX_DEACAP_KEY_NUM 16
#define IPA_IPSEC_MAX_KEY_NUM (IPA_IPSEC_MAX_ENACAP_KEY_NUM + IPA_IPSEC_MAX_DEACAP_KEY_NUM)
#define IPA_IPSEC_MAX_KEY_LEN 64
#define XFRM_ALG_NAME_MAX 64
#define IPA_IPSEC_DL_FLT_PER_POL 10

#define IPA_IPSEC_OFFLOAD_MAGIC 0x01BA0000

#define IPA_IPSEC_SA_MISMATCH_ERR_QMAP_HDR_NAME "ipsec_sa_mismatch_err_qmap_hdr"

enum ipa_ipsec_key_type {
	IPA_IPSEC_KEY_ENC,
	IPA_IPSEC_KEY_AUTH,
	IPA_IPSEC_KEY_IV,
	IPA_IPSEC_KEY_MAX,
};

enum ipa_ipsec_key_type_v2 {
	IPA_IPSEC_KEY_ENCAP_ENC,
	IPA_IPSEC_KEY_DECAP_ENC,
	IPA_IPSEC_KEY_ENCAP_AUTH,
	IPA_IPSEC_KEY_ENCAP_IV,
	IPA_IPSEC_KEY_DECAP_AUTH,
	IPA_IPSEC_KEY_MAX_v2,
};

enum ipa_ipsec_key_len {
	IPA_IPSEC_KEY_0 = 0,
	IPA_IPSEC_KEY_128,
	IPA_IPSEC_KEY_256,
	IPA_IPSEC_KEY_512,
	IPA_IPSEC_KEY_160,
	IPA_IPSEC_KEY_384,
	IPA_IPSEC_KEY_LEN_MAX,
};

enum ipa_ipsec_sa_type {
	IPA_IPSEC_ENCAP,
	IPA_IPSEC_DECAP,
	IPA_IPSEC_TYPE_MAX,
};

enum ipa_ipsec_hpc_action {
	IPA_IPSEC_HPC_DISABLE = 0,
	IPA_IPSEC_HPC_ENCAP = 1,
	IPA_IPSEC_HPC_DECAP = 2,
	IPA_IPSEC_HPC_RESERVED = 3,
	IPA_IPSEC_HPC_MAX,
};

enum ipa_ipsec_error_type {
	IPA_IPSEC_ERROR_TYPE_ENCAP = 1,
	IPA_IPSEC_ERROR_TYPE_DECAP = 2,
};

enum ipa_ipsec_uc_thresh_type {
	IPA_IPSEC_UC_THRESH_SOFT = 0,
	IPA_IPSEC_UC_THRESH_HARD = 1,
	IPA_IPSEC_UC_THRESH_MAX,
};

enum ipa_ipsec_uc_sa_action {
	IPA_IPSEC_UC_SA_ACT_NONE = 0,
	IPA_IPSEC_UC_SA_ACT_ENCAP = 1,
	IPA_IPSEC_UC_SA_ACT_DECAP = 2,
	IPA_IPSEC_UC_SA_ACT_MAX,
};

enum ipa_ipsec_error_encap_code {
	IPA_IPSEC_ERROR_CODE_DISCARD_RULE = 64,
	IPA_IPSEC_ERROR_CODE_ENCAP_SA_DISABLED = 65,
	IPA_IPSEC_ERROR_CODE_SEQ_NUM_OVERFLOW = 66,
	IPA_IPSEC_ERROR_CODE_SEQ_NUM_EXCEED_MTU = 135,
};

enum ipa_ipsec_error_decap_code {
	IPA_IPSEC_ERROR_CODE_DUP_SEQ_NUMBER = 1,
	IPA_IPSEC_ERROR_CODE_OUT_OF_WINDOW = 2,
	IPA_IPSEC_ERROR_CODE_AUTH_ERROR = 3,
	IPA_IPSEC_ERROR_CODE_INCORRECT_PADDING = 4,
	IPA_IPSEC_ERROR_CODE_INCORRECT_ESP_NEXT_HDR_PROTO = 5,
	IPA_IPSEC_ERROR_CODE_ECN_ERROR = 6,
	IPA_IPSEC_ERROR_CODE_POST_DECAP_NAT = 7,
	IPA_IPSEC_ERROR_CODE_INNER_PKT_EXCP = 8,
	IPA_IPSEC_ERROR_CODE_INNER_PKT_FLT_EXCP = 9,

	IPA_IPSEC_ERROR_CODE_DECAP_SA_DISABLED = 32,
	IPA_IPSEC_ERROR_CODE_SW_HANDLING = 33,
	IPA_IPSEC_ERROR_CODE_INNER_PKT_VALIDATION = 34,
	IPA_IPSEC_ERROR_CODE_INNER_PKT_SA_MISMATCH = 35,
	IPA_IPSEC_ERROR_CODE_FRAG = 36,
};

enum ipa_ipsec_error_action {
	IPA_IPSEC_ERROR_NO_ACTION = 0,
	IPA_IPSEC_ERROR_TO_NS_ACTION = 1,
	IPA_IPSEC_ERROR_SW_HANDLING_ACTION = 2,
	IPA_IPSEC_ERROR_DROP_ACTION = 3,
};

enum ipa_ipsec_sa_auth {
	IPA_IPSEC_AUTH_NONE = 0,
	IPA_IPSEC_AUTH_HMAC_SHA1 = 2,
	IPA_IPSEC_AUTH_HMAC_SHA2_256 = 12,
	IPA_IPSEC_AUTH_HMAC_SHA2_384 = 13,
	IPA_IPSEC_AUTH_HMAC_SHA2_512 = 14,
	IPA_IPSEC_AUTH_MAX,
};

enum ipa_ipsec_sa_enc {
	IPA_IPSEC_ENC_NULL = SADB_EALG_NULL,
	IPA_IPSEC_ENC_AES_CBC = SADB_X_EALG_AESCBC,
	IPA_IPSEC_ENC_AES_GCM_16 = SADB_X_EALG_AES_GCM_ICV16,
	IPA_IPSEC_ENC_MAX = SADB_EALG_MAX,
};

struct ipa_ipsec_algo {
	char *name;
	union {
		enum ipa_ipsec_sa_auth a;
		enum ipa_ipsec_sa_enc e;
	} algo;
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
	u32 thresh_met		:1;
	u32 reserved		:13;
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
	u32 overflow_allowed	:1;
	u32 reserved1		:10;
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
	u32 thresh_met		:1;
	u32 reserved		:29;

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

union ipa_ipsec_iv_key {
	struct {
		u8 b[16];
	} b128;
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

struct ipa_ipsec_key_store_v2 {
	union ipa_ipsec_enc_key encap_enc[IPA_IPSEC_MAX_ENACAP_KEY_NUM];
	union ipa_ipsec_enc_key decap_enc[IPA_IPSEC_MAX_DEACAP_KEY_NUM];
	union ipa_ipsec_auth_key encap_auth[IPA_IPSEC_MAX_ENACAP_KEY_NUM];
	union ipa_ipsec_iv_key encap_iv[IPA_IPSEC_MAX_ENACAP_KEY_NUM];
	union ipa_ipsec_auth_key decap_auth[IPA_IPSEC_MAX_DEACAP_KEY_NUM];
};
#pragma pack(pop)

#define IPA_IPSEC_KEYS(ipa_ver) ((ipa_ver < IPA_HW_v7_0) ? (ipa3_ctx->ipsec->keys) : (ipa3_ctx->ipsec->keys_v2))

/**
 * struct ipa_ipsec_policy - IPA IPsec policy list member
 * @l: kernel list sub struct
 * @xp: XFRM policy pointer
 * @flt: Filtering rules, if applicable
 * @rt: Routing rule, if applicable
 */
struct ipa_ipsec_policy {
	struct list_head l;
	struct xfrm_policy *xp;
	u32 flt[IPA_IPSEC_DL_FLT_PER_POL];
	u32 rt;
};

struct ipa_ipsec_encap_stats {
	atomic_t ipsec_encap_xmit;
	atomic_t error_code_encap_sa_disabled;
	atomic_t error_code_seq_num_overflow;
	atomic_t ipsec_excp_exceed_mtu;
};

struct ipa_ipsec_decap_stats {
	atomic_t ipsec_decap_xmit;
	atomic_t ipsec_decap_rcv;
	atomic_t error_code_dup_seq_number;
	atomic_t error_code_out_of_window;
	atomic_t error_code_auth_error;
	atomic_t error_code_incorrect_padding;
	atomic_t error_code_incorrect_esp_next_hdr_proto;
	atomic_t error_code_ecn_error;
	atomic_t error_code_post_decap_nat;
	atomic_t error_code_inner_pkt_excp;
	atomic_t error_code_inner_pkt_flt_excp;
	atomic_t error_code_decap_sa_disabled;
	atomic_t error_code_sw_handling;
	atomic_t error_code_inner_pkt_validation;
	atomic_t error_code_inner_pkt_sa_mismatch;
};

struct ipa_ipsec_stats {
	struct ipa_ipsec_encap_stats encap_stats[IPA_IPSEC_MAX_SA_NUM];
	struct ipa_ipsec_decap_stats decap_stats[IPA_IPSEC_MAX_SA_NUM];

	/* General IPsec error codes (not related to SA index) */
	atomic_t encap_error_code_discard_rule;
	atomic_t error_code_frag;
};

/**
 * struct ipa_ipsec_ctx - IPA IPsec context
 * @dev: netdev pointer
 * @keys: SRAM mapped to the keys storage (32 * 32 + 32 * 64 = 3072 bytes)
 * @keys_v2: SRAM mapped to the keys storage for IPAv7.0 and newer
 * @decap: SRAM mapped to the decap SAs area (11 * 88 = 968 bytes)
 * @encap: SRAM mapped to the encap SAs area (11 * 256 = 2816 bytes)
 * @uc_smmu_iova: uC SMMU VA for cleanup
 * @xfrmdev_ops: Pointer to XFRM callbacks struct
 * @sa_db: Per SA handlers: x, hdr, hpc, rt
 * @x: Pointer to the XFRM state
 * @hdr: Header template handle (only for encap)
 * @hpc: HPC handle
 * @rt: RT rule handle
 * @pol_list: List head for offloaded XFRM policies
 * @ul_hpc: next-round HPC for encap catch all and IKE RT rules
 * @dl_hpc: next-round HPC for decap RT rules
 * @dl_pol_flt: ID (IPA_CLIENT_MAX + n) of the EP independent FLT table for DL policy rules
 * @encap_rt: Encap RT table handle
 * @decap_rt: Decap RT table handle
 * @decap_no_policy_rt: RT table for decapsulated packets with no policy hit handle
 * @default_rt: Default route table pointer
 * @stats: IPsec stats struct
 * @sa_mismatch_qmap_hdr_hdl: IPsec error QMAP header hdl for "no-policy" RT rule
 * @mux_id: Mux ID of the default PDN
 * @initialized: Boolean to indicate the whole IPsec feature initialized
 * @enabled: Boolean to indicate the whole IPsec feature enabled
 */
struct ipa_ipsec_ctx {
	struct net_device *dev;
	struct ipa_ipsec_key_store __iomem *keys;
	struct ipa_ipsec_key_store_v2 __iomem *keys_v2;
	struct ipa_ipsec_sa_decap __iomem *decap;
	struct ipa_ipsec_sa_encap __iomem *encap;
	unsigned long uc_smmu_iova;
	struct xfrmdev_ops *xfrmdev_ops;
	struct {
		struct xfrm_state *x;
		u32 hdr;
		u32 hpc;
		u32 rt;
	} sa_db[IPA_IPSEC_TYPE_MAX][IPA_IPSEC_MAX_SA_NUM];
	struct list_head pol_list;
	struct mutex pol_list_lock;
	u32 ul_hpc;
	u32 dl_hpc;
	u32 dl_pol_flt[IPA_IP_MAX];
	u32 encap_rt[IPA_IP_MAX];
	u32 decap_rt[IPA_IP_MAX];
	u32 decap_no_policy_rt[IPA_IP_MAX];
	struct ipa3_rt_tbl *default_rt;
	struct ipa_ipsec_stats stats;
	u32 sa_mismatch_qmap_hdr_hdl;
	u32 mux_id;
	u16 mtu_v4;
	u16 mtu_v6;
	bool initialized;
	bool enabled;
};

struct ipa_ipsec_state_work_wrap {
	struct work_struct work;
	enum ipa_ip_type ip;
	u8 idx;
	u8 dir;
};

struct ipa_ipsec_policy_work_wrap {
	struct work_struct work;
	struct ipa_ipsec_policy *pol;
	enum ipa_ip_type ip;
	u8 dir;
};

void apps_ipa_ipsec_err_pkt_rcv_ntfy(void *priv, enum ipa_dp_evt_type evt, unsigned long data);

#ifdef CONFIG_IPA_IPSEC
bool ipa_ipsec_initialized(void);
bool ipa_ipsec_enabled(void);
int ipa_ipsec_init(void);
int ipa_ipsec_enable(void);
void ipa_ipsec_cleanup(void);
int ipa_ipsec_install_dl_pol_flt(void);
int ipa_ipsec_install_qmi_flt(struct ipa_install_fltr_rule_req_ex_msg_v01 *req);
int ipa_ipsec_handle_lan_up_down(enum ipa_ip_type ip, struct ipa3_rt_tbl *entry, bool up);
int ipa_ipsec_rx_update_sec_path(struct sk_buff *skb, u32 metadata);
int ipa_ipsec_ep_init_prod(void);
void ipa_ipsec_ep_init_cons(struct work_struct *work);
void ipa_ipsec_handle_sa_thresh(u8 idx, enum ipa_ipsec_uc_sa_action action,
	enum ipa_ipsec_uc_thresh_type thresh_type);
const char * ipa_ipsec_get_auth_algo_name(enum ipa_ipsec_sa_auth auth_algo);
const char * ipa_ipsec_get_encr_algo_name(enum ipa_ipsec_sa_enc encr_algo);
#define IPA_UC_IPSEC_WORKQUEUE_NAME "ipa_uc_ipsec_wq"
static struct workqueue_struct *ipa_uc_ipsec_wq;
#else
inline bool ipa_ipsec_initialized(void) {return false;}
inline bool ipa_ipsec_enabled(void) {return false;}
inline int ipa_ipsec_init(void) {return 0;}
inline int ipa_ipsec_enable(void) {return 0;}
inline void ipa_ipsec_cleanup(void) {}
inline int ipa_ipsec_install_dl_pol_flt(void) {return 0;}
inline int ipa_ipsec_install_qmi_flt(struct ipa_install_fltr_rule_req_ex_msg_v01 *req) {return 0;}
inline int ipa_ipsec_handle_lan_up_down(enum ipa_ip_type ip, struct ipa3_rt_tbl *entry, bool up)
{return 0;}
inline int ipa_ipsec_rx_update_sec_path(struct sk_buff *skb, u32 metadata) {return 0;}
inline int ipa_ipsec_ep_init_prod(void) {return 0;}
inline int ipa_ipsec_ep_init_cons(void) {return 0;}
inline void ipa_ipsec_handle_sa_thresh(u8 idx, enum ipa_ipsec_uc_sa_action action,
	enum ipa_ipsec_uc_thresh_type thresh_type) {return;}
inline const char * ipa_ipsec_get_auth_algo_name(enum ipa_ipsec_sa_auth auth_algo) {return NULL;}
inline const char * ipa_ipsec_get_encr_algo_name(enum ipa_ipsec_sa_enc encr_algo) {return NULL;}
#endif

#endif /* _IPA_IPSEC_H_ */
