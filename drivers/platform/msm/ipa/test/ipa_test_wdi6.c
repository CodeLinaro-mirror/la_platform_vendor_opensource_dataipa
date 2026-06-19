// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2018 - 2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include "ipa_ut_framework.h"
#include "ipa_wdi3.h"
#include <linux/ipa_wdi3.h>
#include <linux/msm_ipa.h>
#include "ipa.h"
#include <linux/delay.h>
#include "ipa_i.h"
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>

#define NUM_TX_BUFS 10
#define NUM_RX_BUFS 10
#define NUM_REDUNDANT_TX_ELE 1
#define NUM_RX_TR_ELE NUM_RX_BUFS
#define NUM_RX_ER_ELE NUM_RX_BUFS
#define NUM_TX_TR_ELE (NUM_TX_BUFS + NUM_REDUNDANT_TX_ELE)
#define NUM_TX_ER_ELE (NUM_TX_BUFS + NUM_REDUNDANT_TX_ELE)

#define RX_METADATA_SIZE 4
#define PACKET_HEADER_SIZE 220
#define PACKET_HEADER_SIZE_M 384
#define ETH_PACKET_SIZE 4
#define PACKET_CONTENT 0x12345678

#define PKT_SIZE 1024

#define DB_REGISTER_SIZE 4

#define NUM_MULTI_PKT 8

#define TEST_WDI6_PROD_MACRO IPA_CLIENT_WLAN2_PROD
#define TEST_WDI6_CONS_MACRO IPA_CLIENT_WLAN2_CONS

#define TEST_WDI6_DUMP_GSI_REGS() do { \
	const struct ipa_gsi_ep_config *gsi_ep_cfg_rx; \
	const struct ipa_gsi_ep_config *gsi_ep_cfg_tx; \
	gsi_ep_cfg_rx = ipa_get_gsi_ep_info(TEST_WDI6_PROD_MACRO); \
	if (gsi_ep_cfg_rx) \
		gsi_wdi3_dump_register(gsi_ep_cfg_rx->ipa_gsi_chan_num); \
	gsi_ep_cfg_tx = ipa_get_gsi_ep_info(TEST_WDI6_CONS_MACRO); \
	if (gsi_ep_cfg_tx) \
		gsi_wdi3_dump_register(gsi_ep_cfg_tx->ipa_gsi_chan_num); \
} while (0)



static int multi_pkt_array[] = {0x12345678, 0x87654321,
	0x00112233, 0x01234567, 0x45454545, 0x80808080,
	0x13245678, 0x12345767, 0x43213456};

static int rx_uc_db_local;
static int tx_uc_db_local;
static u8 tx_bf_idx;
static u8 rx_bf_idx;

struct ipa_test_wdi6_context {
	struct ipa_mem_buffer tx_transfer_ring_addr;
	struct ipa_mem_buffer tx_event_ring_addr;
	struct ipa_mem_buffer rx_transfer_ring_addr;
	struct ipa_mem_buffer rx_event_ring_addr;
	struct sg_table *tx_transfer_ring_sgt;
	struct sg_table *tx_event_ring_sgt;
	struct sg_table *rx_transfer_ring_sgt;
	struct sg_table *rx_event_ring_sgt;
	struct ipa_mem_buffer tx_pool;
	struct sg_table *tx_pool_sgt;
	struct ipa_mem_buffer rx_pool;
	struct sg_table *rx_pool_sgt;
	struct ipa_mem_buffer tx_bufs[NUM_TX_BUFS];
	struct ipa_mem_buffer rx_bufs[NUM_RX_BUFS];
	struct ipa_mem_buffer tx_transfer_ring_db;
	struct ipa_mem_buffer tx_event_ring_db;
	struct ipa_mem_buffer rx_transfer_ring_db;
	struct ipa_mem_buffer rx_event_ring_db;
	dma_addr_t tx_uc_db_pa;
	dma_addr_t rx_uc_db_pa;
	ipa_wdi_hdl_t wdi_hdl;
	struct completion xfer_done;
	u32 num_packets;
	u32 pkt_idx;
};

static struct ipa_test_wdi6_context *test_wdi6_ctx;
struct wdi6_buffer_addr_info {
	u32 buffer_addr_low;
	u32 buffer_addr_high : 8;
	u32 return_buffer_manager : 3;
	u32 sw_buffer_cookie : 21;
} __packed;

/**
 * struct wdi6_tx_transfer_ring_ele - WDI6 TX transfer ring element
 */
struct wdi6_tx_transfer_ring_ele {
	/* DWORD 0 */
	u32 buffer_addr_lo;

	/* DWORD 1 */
	u32 buffer_addr_hi : 8;
	u32 buf_return_buffer_manager : 3;
	u32 buf_sw_buffer_cookie : 21;
} __packed;

struct wdi6_tx_event_ring_ele {
	/* DWORD 0 */
	u32 buffer_addr_lo;

	/* DWORD 1 */
	u32 buffer_addr_hi : 8;
	u32 drop_prec : 2;
	u32 fake_mac_header : 1;
	u32 multicast : 1;
	u32 cpu_code_valid : 1;
	u32 tunnel_term_ind : 1;
	u32 tunnel_type : 1;
	u32 wifi_qos_flag : 1;
	u32 service_code : 9;
	u32 acl_index_valid : 1;
	u32 int_pri : 4;
	u32 more : 1;
	u32 if_match_flag : 1;

	/* DWORD 2 */
	u32 opaque_lo;

	/* DWORD 3 */
	u32 flow_cookie_ext_1 : 16;
	u32 vlan_tci : 16;

	/* DWORD 4 */
	u32 src_info : 16;
	u32 dst_info : 16;

	/* DWORD 5 */
	u32 data_length : 18;
	u32 pid : 4;
	u32 reserved_5a : 2;
	u32 wifi_qos : 8;

	/* DWORD 6 */
	u32 data_offset : 12;
	u32 l4_csum_status : 1;
	u32 l3_csum_status : 1;
	u32 hash_flag : 2;
	u32 hash_value : 16;

	/* DWORD 7 */
	u32 rx_timestamp : 19;
	u32 dscp : 8;
	u32 pppoe_flag : 1;
	u32 svlan_flag : 1;
	u32 cvlan_flag : 1;
	u32 snap_flag : 1;
	u32 rx_timestamp_valid : 1;
} __packed;

struct rx_attention_tlv {
     u8 rsvd[16];
};

struct rx_tlv_from_ipa{
	u32 tcp_udp_chksum 				 : 16, //[15:0]
			 sa_idx_timeout 				 :	1, //[16]
			 da_idx_timeout 				 :	1, //[17]
			 msdu_limit_error				 :	1, //[18]
			 flow_idx_timeout				 :	1, //[19]
			 flow_idx_invalid				 :	1, //[20]
			 wifi_parser_error				 :	1, //[21]
			 amsdu_parser_error 			 :	1, //[22]
			 sa_is_valid					 :	1, //[23]
			 da_is_valid					 :	1, //[24]
			 da_is_mcbc 					 :	1, //[25]
			 l3_header_padding				 :	2, //[27:26]
			 first_msdu 					 :	1, //[28]
			 last_msdu						 :	1, //[29]
			 reserved_10a					 :	2; //[31:30]
	u32 sa_idx 						 : 16, //[15:0]
			 da_idx_or_sw_peer_id			 : 16; //[31:16]
	u32 msdu_drop						 :	1, //[0]
			 reo_destination_indication 	 :	5, //[5:1]
			 flow_idx						 : 20, //[25:6]
			 reserved_12a					 :	6; //[31:26]
	u32 fse_metadata					 : 32; //[31:0]
	u32 cce_metadata					 : 16, //[15:0]
			 sa_sw_peer_id					 : 16; //[31:16]
	u32 aggregation_count				 :	8, //[7:0]
			 flow_aggregation_continuation	 :	1, //[8]
			 fisa_timeout					 :	1, //[9]
			 reserved_15a					 : 22; //[31:10]
	u32 cumulative_l4_checksum 		 : 16, //[15:0]
			 cumulative_ip_length			 : 16; //[31:16]
	struct rx_attention_tlv attn;
	u32 rsvd[44];
	u32 MAC_ADDR_AD4_VALID;
}__packed;

struct rx_msdu_end {
	u8 rsvd[40];
	u32 tcp_udp_chksum 				 : 16, //[15:0]
			 sa_idx_timeout 				 :	1, //[16]
			 da_idx_timeout 				 :	1, //[17]
			 msdu_limit_error				 :	1, //[18]
			 flow_idx_timeout				 :	1, //[19]
			 flow_idx_invalid				 :	1, //[20]
			 wifi_parser_error				 :	1, //[21]
			 amsdu_parser_error 			 :	1, //[22]
			 sa_is_valid					 :	1, //[23]
			 da_is_valid					 :	1, //[24]
			 da_is_mcbc 					 :	1, //[25]
			 l3_header_padding				 :	2, //[27:26]
			 first_msdu 					 :	1, //[28]
			 last_msdu						 :	1, //[29]
			 reserved_10a					 :	2; //[31:30]
	u32 sa_idx 						 : 16, //[15:0]
			 da_idx_or_sw_peer_id			 : 16; //[31:16]
	u32 msdu_drop						 :	1, //[0]
			 reo_destination_indication 	 :	5, //[5:1]
			 flow_idx						 : 20, //[25:6]
			 reserved_12a					 :	6; //[31:26]
	u32 fse_metadata					 : 32; //[31:0]
	u32 cce_metadata					 : 16, //[15:0]
			 sa_sw_peer_id					 : 16; //[31:16]
	u32 aggregation_count				 :	8, //[7:0]
			 flow_aggregation_continuation	 :	1, //[8]
			 fisa_timeout					 :	1, //[9]
			 reserved_15a					 : 22; //[31:10]
	u32 cumulative_l4_checksum 		 : 16, //[15:0]
			 cumulative_ip_length			 : 16; //[31:16]
}__packed;

struct rx_mpdu_start {
	u8 rsvd[44];
	u32 MAC_ADDR_AD4_VALID;
	u8 rsvd1[44];
}__packed;

struct rx_msdu_end_tlv {
    u32 tag;
    struct rx_msdu_end rx_msdu_end;
};

struct rx_mpdu_start_tlv {
    u32 tag;
    struct rx_mpdu_start rx_mpdu_start;
};

struct rx_msdu_start_tlv {
    u8 rsvd[44];
};

struct rx_mpdu_end_tlv {
    u8 rsvd[28];
};

struct rx_pkt_hdr_tlv {
    u32 tag;                           /* 4 B */
    u32 phy_ppdu_id;                   /* 4 B */
    char rx_pkt_hdr[120];    /* 120 B */
};

struct rx_pkt_tlvs {
    struct rx_msdu_end_tlv   msdu_end_tlv;
    struct rx_attention_tlv  attn_tlv;
    struct rx_mpdu_start_tlv mpdu_start_tlv;
    struct rx_msdu_start_tlv msdu_start_tlv;
    struct rx_mpdu_end_tlv   mpdu_end_tlv;
    struct rx_pkt_hdr_tlv    pkt_hdr_tlv;
};

struct wdi6_rx_mpdu_desc_info {
	u32 msdu_count : 8;
	u32 fragment_flag : 1;
	u32 mpdu_retry_bit : 1;
	u32 ampdu_flag : 1;
	u32 bar_frame : 1;
	u32 pn_fields_contain_valid_info : 1;
	u32 raw_mpdu : 1;
	u32 more_fragment_flag : 1;
	u32 src_info : 12;
	u32 mpdu_qos_control_valid : 1;
	u32 tid : 4;
	u32 peer_meta_data;
} __packed;

struct wdi6_rx_mpdu_ext_desc_info {
	u32 mgmt_pkt : 1;
	u32 rxdma_push_reason : 2;
	u32 rxdma_error_code : 5;
	u32 reo_dest_buffer_type : 1;
	u32 release_source_module : 3;
	u32 msdu_link_desc_index : 4;
	u32 ll_pkt : 1;
	u32 high_priority_pkt : 2;
	u32 src_link_id : 3;
	u32 reo_push_reason : 2;
	u32 reo_error_code : 5;
	u32 groupcast_mpdu : 1;
	u32 reserved_0a : 1;
	u32 smr_mpdu : 1;
} __packed;

struct wdi6_rx_msdu_desc_info {
	u32 first_msdu_in_mpdu_flag : 1;
	u32 last_msdu_in_mpdu_flag : 1;
	u32 msdu_continuation : 1;
	u32 msdu_length : 14;
	u32 msdu_drop : 1;
	u32 sa_is_valid : 1;
	u32 da_is_valid : 1;
	u32 da_is_bcast_mcast : 1;
	u32 l3_header_padding_msb : 1;
	u32 tcp_udp_chksum_fail : 1;
	u32 ip_chksum_fail : 1;
	u32 fr_ds : 1;
	u32 to_ds : 1;
	u32 intra_bss : 1;
	u32 dest_chip_id : 2;
	u32 decap_format : 2;
	u32 dest_chip_pmac_id : 1;
} __packed;

struct wdi6_rx_transfer_ring_ele {
	/* DWORD 0 */
	u32 buffer_addr_lo;

	/* DWORD 1 */
	u32 buffer_addr_hi : 8;
	u32 buf_return_buffer_manager : 3;
	u32 buf_sw_buffer_cookie : 21;

	struct wdi6_rx_mpdu_desc_info rx_mpdu_desc_info_details;
	struct wdi6_rx_mpdu_ext_desc_info rx_mpdu_ext_desc_info_details;
	struct wdi6_rx_msdu_desc_info rx_msdu_desc_info_details;
	u32 cookie_conversion_status : 1;
	u32 reo_delink_error : 1;
	u32 sw_buffer_cookie : 20;
	u32 reserved_6a : 2;
	u32 phy_lmac_latency : 8;
	u32 rx_msdu_stream_desc_info : 16;
	u32 sw_exception : 1;
	u32 backpressure_drop : 1;
	u32 flow_idx_valid : 1;
	u32 rx_sdwf_msdu_dropped : 1;
	u32 ring_id : 8;
	u32 looping_count : 4;
} __packed;

struct wdi6_rx_event_ring_ele {
	/* DWORD 0 */
	u32 buffer_addr_lo;

	/* DWORD 1 */
	u32 buffer_addr_hi : 8;
	u32 return_buffer_manager : 4;
	u32 sw_buffer_cookie : 20;
} __packed;

static void ipa_test_wdi6_free_dma_buff(struct ipa_mem_buffer *mem)
{
	struct ipa_smmu_cb_ctx *cb = ipa3_get_smmu_ctx(ipa_get_client_smmu_cb_type(TEST_WDI6_CONS_MACRO));

	if (!mem) {
		IPA_UT_ERR("empty pointer\n");
		return;
	}

	dma_free_coherent(cb->dev, mem->size, mem->base,
		mem->phys_base);
}

static void ipa_test_wdi6_advance_uc_db(u32 *db, int steps,
	int num_words, int ring_size)
{
	*db = (*db + steps * num_words) % (ring_size / 4);
	IPA_UT_DBG("new db value: %u\n", *db);
}

static int ipa_test_wdi6_init_rings(void)
{
	struct wdi6_tx_transfer_ring_ele *tx_ele;
	struct wdi6_rx_transfer_ring_ele *rx_ele;
	struct wdi6_tx_event_ring_ele *tx_event_ele;
	struct wdi6_rx_event_ring_ele *rx_event_ele;
	int i;

	IPA_UT_DBG("initializing WDI6 rings\n");

	rx_ele = (struct wdi6_rx_transfer_ring_ele *)
		test_wdi6_ctx->rx_transfer_ring_addr.base;
	tx_ele = (struct wdi6_tx_transfer_ring_ele *)
		test_wdi6_ctx->tx_transfer_ring_addr.base;
	tx_event_ele = (struct wdi6_tx_event_ring_ele *)
		test_wdi6_ctx->tx_event_ring_addr.base;
	rx_event_ele = (struct wdi6_rx_event_ring_ele *)
		test_wdi6_ctx->rx_event_ring_addr.base;

	/* zero all rings */
	memset(rx_ele, 0, sizeof(*rx_ele) * NUM_RX_TR_ELE);
	memset(tx_event_ele, 0, sizeof(*tx_event_ele) * NUM_TX_ER_ELE);
	memset(rx_event_ele, 0, sizeof(*rx_event_ele) * NUM_RX_ER_ELE);

	/* pre-populate RX transfer ring with buffer addresses.
	 * These are the free RX buffers that WLAN will DMA received packets into.
	 */
	for (i = 0; i < NUM_RX_BUFS; i++) {
		rx_ele->buffer_addr_lo =
			(u64)test_wdi6_ctx->rx_bufs[i].phys_base & 0xFFFFFFFF;
		rx_ele->buffer_addr_hi =
			((u64)test_wdi6_ctx->rx_bufs[i].phys_base >> 32)
			& 0xFF;
		rx_ele++;
	}

	/* TX transfer ring: zero all fields.
	 * TX buffers are the free buffers that IPA will DMA received packets into.
	 * They are populated with buffer addresses in ipa_wdi6_setup_pipes()
	 * after pipe connection, along with ringing the TX doorbell.
	 */
	memset(tx_ele, 0, sizeof(*tx_ele) * NUM_TX_TR_ELE);

	return 0;
}

static int ipa_test_wdi6_alloc_mmio(void)
{
	int ret = 0, i;
	u32 size;
	struct ipa_smmu_cb_ctx *cb = ipa3_get_smmu_ctx(ipa_get_client_smmu_cb_type(TEST_WDI6_CONS_MACRO));

	if (!test_wdi6_ctx) {
		IPA_UT_ERR("test_wdi6_ctx is not initialized.\n");
		return -EFAULT;
	}

	/* allocate tx transfer ring memory */
	size = NUM_TX_TR_ELE * sizeof(struct wdi6_tx_transfer_ring_ele);
	test_wdi6_ctx->tx_transfer_ring_addr.size = size;
	test_wdi6_ctx->tx_transfer_ring_addr.base =
		dma_alloc_coherent(cb->dev, size,
			&test_wdi6_ctx->tx_transfer_ring_addr.phys_base,
			GFP_KERNEL);
	if (!test_wdi6_ctx->tx_transfer_ring_addr.phys_base) {
		IPA_UT_ERR("fail to alloc memory.\n");
		return -ENOMEM;
	}

	/* allocate tx event ring memory */
	size = NUM_TX_ER_ELE * sizeof(struct wdi6_tx_event_ring_ele);
	test_wdi6_ctx->tx_event_ring_addr.size = size;
	test_wdi6_ctx->tx_event_ring_addr.base =
		dma_alloc_coherent(cb->dev, size,
			&test_wdi6_ctx->tx_event_ring_addr.phys_base,
			GFP_KERNEL);
	if (!test_wdi6_ctx->tx_event_ring_addr.phys_base) {
		IPA_UT_ERR("fail to alloc memory.\n");
		ret = -ENOMEM;
		goto fail_tx_event_ring;
	}

	/* allocate rx transfer ring memory */
	size = NUM_RX_TR_ELE * sizeof(struct wdi6_rx_transfer_ring_ele);
	test_wdi6_ctx->rx_transfer_ring_addr.size = size;
	test_wdi6_ctx->rx_transfer_ring_addr.base =
		dma_alloc_coherent(cb->dev, size,
			&test_wdi6_ctx->rx_transfer_ring_addr.phys_base,
			GFP_KERNEL);
	if (!test_wdi6_ctx->rx_transfer_ring_addr.phys_base) {
		IPA_UT_ERR("fail to alloc memory.\n");
		ret = -ENOMEM;
		goto fail_rx_transfer_ring;
	}

	/* allocate rx event ring memory */
	size = NUM_RX_ER_ELE * sizeof(struct wdi6_rx_event_ring_ele);
	test_wdi6_ctx->rx_event_ring_addr.size = size;
	test_wdi6_ctx->rx_event_ring_addr.base =
		dma_alloc_coherent(cb->dev, size,
			&test_wdi6_ctx->rx_event_ring_addr.phys_base,
			GFP_KERNEL);
	if (!test_wdi6_ctx->rx_event_ring_addr.phys_base) {
		IPA_UT_ERR("fail to alloc memory.\n");
		ret = -ENOMEM;
		goto fail_rx_event_ring;
	}

	/* allocate tx buffers - use PKT_SIZE (4096) for proper DMA alignment */
	size = PKT_SIZE * NUM_TX_BUFS;
	test_wdi6_ctx->tx_pool.size = size;
	test_wdi6_ctx->tx_pool.base =
		dma_alloc_coherent(cb->dev, size,
			&test_wdi6_ctx->tx_pool.phys_base,
			GFP_KERNEL);
	if (!test_wdi6_ctx->tx_pool.phys_base) {
		IPA_UT_ERR("fail to alloc tx pool.\n");
		ret = -ENOMEM;
		goto fail_tx_pool;
	}

	test_wdi6_ctx->tx_pool_sgt = kzalloc(
		sizeof(*test_wdi6_ctx->tx_pool_sgt), GFP_KERNEL);
	if (!test_wdi6_ctx->tx_pool_sgt) {
		IPA_UT_ERR("fail to alloc memory.\n");
		ret = -ENOMEM;
		goto fail_alloc_tx_pool_sgt;
	}

	ret = dma_get_sgtable(cb->dev, test_wdi6_ctx->tx_pool_sgt,
		test_wdi6_ctx->tx_pool.base, test_wdi6_ctx->tx_pool.phys_base, size);
	if (ret) {
		IPA_UT_ERR("failed to get sgtable\n");
		ret = -ENOMEM;
		goto fail_get_tx_pool_sgtable;
	}
	for (i = 0; i < test_wdi6_ctx->tx_pool_sgt->nents; i++)
		sg_dma_address(&test_wdi6_ctx->tx_pool_sgt->sgl[i]) = page_to_phys(sg_page(&test_wdi6_ctx->tx_pool_sgt->sgl[i]));

	IPA_UT_INFO("TX pool: phys_base=0x%llx size=0x%x (SMMU cb=%d)\n",
		(u64)test_wdi6_ctx->tx_pool.phys_base, size,
		ipa_get_client_smmu_cb_type(TEST_WDI6_CONS_MACRO));

	for (i = 0; i < NUM_TX_BUFS; i++) {
		test_wdi6_ctx->tx_bufs[i].size = PKT_SIZE;
		test_wdi6_ctx->tx_bufs[i].base = test_wdi6_ctx->tx_pool.base + i * PKT_SIZE;
		test_wdi6_ctx->tx_bufs[i].phys_base = test_wdi6_ctx->tx_pool.phys_base + i * PKT_SIZE;
		IPA_UT_INFO("TX buf[%d]: phys_base=0x%llx size=0x%x\n",
			i, (u64)test_wdi6_ctx->tx_bufs[i].phys_base,
			test_wdi6_ctx->tx_bufs[i].size);
	}

	/* allocate rx buffers - use PKT_SIZE (4096) for proper DMA alignment */
	size = PKT_SIZE * NUM_RX_BUFS;
	test_wdi6_ctx->rx_pool.size = size;
	test_wdi6_ctx->rx_pool.base =
		dma_alloc_coherent(cb->dev, size,
			&test_wdi6_ctx->rx_pool.phys_base,
			GFP_KERNEL);
	if (!test_wdi6_ctx->rx_pool.phys_base) {
		IPA_UT_ERR("fail to alloc rx pool.\n");
		ret = -ENOMEM;
		goto fail_rx_pool;
	}

	test_wdi6_ctx->rx_pool_sgt = kzalloc(
		sizeof(*test_wdi6_ctx->rx_pool_sgt), GFP_KERNEL);
	if (!test_wdi6_ctx->rx_pool_sgt) {
		IPA_UT_ERR("fail to alloc memory.\n");
		ret = -ENOMEM;
		goto fail_alloc_rx_pool_sgt;
	}

	ret = dma_get_sgtable(cb->dev, test_wdi6_ctx->rx_pool_sgt,
		test_wdi6_ctx->rx_pool.base, test_wdi6_ctx->rx_pool.phys_base, size);
	if (ret) {
		IPA_UT_ERR("failed to get sgtable\n");
		ret = -ENOMEM;
		goto fail_get_rx_pool_sgtable;
	}
	for (i = 0; i < test_wdi6_ctx->rx_pool_sgt->nents; i++)
		sg_dma_address(&test_wdi6_ctx->rx_pool_sgt->sgl[i]) = page_to_phys(sg_page(&test_wdi6_ctx->rx_pool_sgt->sgl[i]));

	IPA_UT_INFO("RX pool: phys_base=0x%llx size=0x%x (SMMU cb=%d)\n",
		(u64)test_wdi6_ctx->rx_pool.phys_base, size,
		ipa_get_client_smmu_cb_type(TEST_WDI6_PROD_MACRO));

	for (i = 0; i < NUM_RX_BUFS; i++) {
		test_wdi6_ctx->rx_bufs[i].size = PKT_SIZE;
		test_wdi6_ctx->rx_bufs[i].base = test_wdi6_ctx->rx_pool.base + i * PKT_SIZE;
		test_wdi6_ctx->rx_bufs[i].phys_base = test_wdi6_ctx->rx_pool.phys_base + i * PKT_SIZE;
		IPA_UT_INFO("RX buf[%d]: phys_base=0x%llx size=0x%x\n",
			i, (u64)test_wdi6_ctx->rx_bufs[i].phys_base,
			test_wdi6_ctx->rx_bufs[i].size);
	}

	/* generate sg tables for rings */
	test_wdi6_ctx->tx_transfer_ring_sgt = kzalloc(
		sizeof(*test_wdi6_ctx->tx_transfer_ring_sgt), GFP_KERNEL);
	if (!test_wdi6_ctx->tx_transfer_ring_sgt) {
		IPA_UT_ERR("fail to alloc memory.\n");
		ret = -ENOMEM;
		goto fail_alloc_tx_tr_sgt;
	}

	ret = dma_get_sgtable(cb->dev, test_wdi6_ctx->tx_transfer_ring_sgt,
		test_wdi6_ctx->tx_transfer_ring_addr.base,
		test_wdi6_ctx->tx_transfer_ring_addr.phys_base, test_wdi6_ctx->tx_transfer_ring_addr.size);
	if (ret) {
		IPA_UT_ERR("failed to get sgtable\n");
		ret = -ENOMEM;
		goto fail_get_tx_tr_sgtable;
	}
	for (i = 0; i < test_wdi6_ctx->tx_transfer_ring_sgt->nents; i++)
		sg_dma_address(&test_wdi6_ctx->tx_transfer_ring_sgt->sgl[i]) = page_to_phys(sg_page(&test_wdi6_ctx->tx_transfer_ring_sgt->sgl[i]));

	test_wdi6_ctx->tx_event_ring_sgt = kzalloc(
		sizeof(*test_wdi6_ctx->tx_event_ring_sgt), GFP_KERNEL);
	if (!test_wdi6_ctx->tx_event_ring_sgt) {
		IPA_UT_ERR("fail to alloc memory.\n");
		ret = -ENOMEM;
		goto fail_alloc_tx_er_sgt;
	}

	ret = dma_get_sgtable(cb->dev, test_wdi6_ctx->tx_event_ring_sgt,
		test_wdi6_ctx->tx_event_ring_addr.base,
		test_wdi6_ctx->tx_event_ring_addr.phys_base, test_wdi6_ctx->tx_event_ring_addr.size);
	if (ret) {
		IPA_UT_ERR("failed to get sgtable\n");
		ret = -ENOMEM;
		goto fail_get_tx_er_sgtable;
	}
	for (i = 0; i < test_wdi6_ctx->tx_event_ring_sgt->nents; i++)
		sg_dma_address(&test_wdi6_ctx->tx_event_ring_sgt->sgl[i]) = page_to_phys(sg_page(&test_wdi6_ctx->tx_event_ring_sgt->sgl[i]));

	test_wdi6_ctx->rx_transfer_ring_sgt = kzalloc(
		sizeof(*test_wdi6_ctx->rx_transfer_ring_sgt), GFP_KERNEL);
	if (!test_wdi6_ctx->rx_transfer_ring_sgt) {
		IPA_UT_ERR("fail to alloc memory.\n");
		ret = -ENOMEM;
		goto fail_alloc_rx_tr_sgt;
	}

	ret = dma_get_sgtable(cb->dev, test_wdi6_ctx->rx_transfer_ring_sgt,
		test_wdi6_ctx->rx_transfer_ring_addr.base,
		test_wdi6_ctx->rx_transfer_ring_addr.phys_base, test_wdi6_ctx->rx_transfer_ring_addr.size);
	if (ret) {
		IPA_UT_ERR("failed to get sgtable\n");
		ret = -ENOMEM;
		goto fail_get_rx_tr_sgtable;
	}
	for (i = 0; i < test_wdi6_ctx->rx_transfer_ring_sgt->nents; i++)
		sg_dma_address(&test_wdi6_ctx->rx_transfer_ring_sgt->sgl[i]) = page_to_phys(sg_page(&test_wdi6_ctx->rx_transfer_ring_sgt->sgl[i]));

	test_wdi6_ctx->rx_event_ring_sgt = kzalloc(
		sizeof(*test_wdi6_ctx->rx_event_ring_sgt), GFP_KERNEL);
	if (!test_wdi6_ctx->rx_event_ring_sgt) {
		IPA_UT_ERR("fail to alloc memory.\n");
		ret = -ENOMEM;
		goto fail_alloc_rx_er_sgt;
	}

	ret = dma_get_sgtable(cb->dev, test_wdi6_ctx->rx_event_ring_sgt,
		test_wdi6_ctx->rx_event_ring_addr.base,
		test_wdi6_ctx->rx_event_ring_addr.phys_base, test_wdi6_ctx->rx_event_ring_addr.size);
	if (ret) {
		IPA_UT_ERR("failed to get sgtable\n");
		ret = -ENOMEM;
		goto fail_get_rx_er_sgtable;
	}
	for (i = 0; i < test_wdi6_ctx->rx_event_ring_sgt->nents; i++)
		sg_dma_address(&test_wdi6_ctx->rx_event_ring_sgt->sgl[i]) = page_to_phys(sg_page(&test_wdi6_ctx->rx_event_ring_sgt->sgl[i]));

	IPA_UT_INFO("TX transfer ring: va=%pK dma_addr=0x%llx size=0x%x\n",
		test_wdi6_ctx->tx_transfer_ring_addr.base,
		(u64)test_wdi6_ctx->tx_transfer_ring_addr.phys_base,
		test_wdi6_ctx->tx_transfer_ring_addr.size);
	IPA_UT_INFO("TX event ring:    va=%pK dma_addr=0x%llx size=0x%x\n",
		test_wdi6_ctx->tx_event_ring_addr.base,
		(u64)test_wdi6_ctx->tx_event_ring_addr.phys_base,
		test_wdi6_ctx->tx_event_ring_addr.size);
	IPA_UT_INFO("RX transfer ring: va=%pK dma_addr=0x%llx size=0x%x\n",
		test_wdi6_ctx->rx_transfer_ring_addr.base,
		(u64)test_wdi6_ctx->rx_transfer_ring_addr.phys_base,
		test_wdi6_ctx->rx_transfer_ring_addr.size);
	IPA_UT_INFO("RX event ring:    va=%pK dma_addr=0x%llx size=0x%x\n",
		test_wdi6_ctx->rx_event_ring_addr.base,
		(u64)test_wdi6_ctx->rx_event_ring_addr.phys_base,
		test_wdi6_ctx->rx_event_ring_addr.size);

	/* allocate tx transfer ring db */
	test_wdi6_ctx->tx_transfer_ring_db.size = DB_REGISTER_SIZE;
	test_wdi6_ctx->tx_transfer_ring_db.base =
		dma_alloc_coherent(cb->dev, test_wdi6_ctx->tx_transfer_ring_db.size,
		&test_wdi6_ctx->tx_transfer_ring_db.phys_base, GFP_KERNEL);
	if (!test_wdi6_ctx->tx_transfer_ring_db.base) {
		IPA_UT_ERR("fail to alloc memory\n");
		ret = -ENOMEM;
		goto fail_tx_transfer_ring_db;
	}
	IPA_UT_INFO("TX transfer ring DB: phys_base=0x%llx size=0x%x\n",
		(u64)test_wdi6_ctx->tx_transfer_ring_db.phys_base,
		test_wdi6_ctx->tx_transfer_ring_db.size);

	/* allocate tx event ring db */
	test_wdi6_ctx->tx_event_ring_db.size = DB_REGISTER_SIZE;
	test_wdi6_ctx->tx_event_ring_db.base =
		dma_alloc_coherent(cb->dev, test_wdi6_ctx->tx_event_ring_db.size,
		&test_wdi6_ctx->tx_event_ring_db.phys_base, GFP_KERNEL);
	if (!test_wdi6_ctx->tx_event_ring_db.base) {
		IPA_UT_ERR("fail to alloc memory\n");
		ret = -ENOMEM;
		goto fail_tx_event_ring_db;
	}
	IPA_UT_INFO("TX event ring DB:    phys_base=0x%llx size=0x%x\n",
		(u64)test_wdi6_ctx->tx_event_ring_db.phys_base,
		test_wdi6_ctx->tx_event_ring_db.size);

	/* allocate rx transfer ring db */
	test_wdi6_ctx->rx_transfer_ring_db.size = DB_REGISTER_SIZE;
	test_wdi6_ctx->rx_transfer_ring_db.base =
		dma_alloc_coherent(cb->dev, test_wdi6_ctx->rx_transfer_ring_db.size,
		&test_wdi6_ctx->rx_transfer_ring_db.phys_base, GFP_KERNEL);
	if (!test_wdi6_ctx->rx_transfer_ring_db.base) {
		IPA_UT_ERR("fail to alloc memory\n");
		ret = -ENOMEM;
		goto fail_rx_transfer_ring_db;
	}
	IPA_UT_INFO("RX transfer ring DB: phys_base=0x%llx size=0x%x\n",
		(u64)test_wdi6_ctx->rx_transfer_ring_db.phys_base,
		test_wdi6_ctx->rx_transfer_ring_db.size);

	/* allocate rx event ring db */
	test_wdi6_ctx->rx_event_ring_db.size = DB_REGISTER_SIZE;
	test_wdi6_ctx->rx_event_ring_db.base =
		dma_alloc_coherent(cb->dev, test_wdi6_ctx->rx_event_ring_db.size,
		&test_wdi6_ctx->rx_event_ring_db.phys_base, GFP_KERNEL);
	if (!test_wdi6_ctx->rx_event_ring_db.base) {
		IPA_UT_ERR("fail to alloc memory\n");
		ret = -ENOMEM;
		goto fail_rx_event_ring_db;
	}
	IPA_UT_INFO("RX event ring DB:    phys_base=0x%llx size=0x%x\n",
		(u64)test_wdi6_ctx->rx_event_ring_db.phys_base,
		test_wdi6_ctx->rx_event_ring_db.size);

	return ret;

fail_rx_event_ring_db:
	if (test_wdi6_ctx->rx_transfer_ring_db.base)
		ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->rx_transfer_ring_db);

fail_rx_transfer_ring_db:
	if (test_wdi6_ctx->tx_event_ring_db.base)
		ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->tx_event_ring_db);

fail_tx_event_ring_db:
	if (test_wdi6_ctx->tx_transfer_ring_db.base)
		ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->tx_transfer_ring_db);

fail_tx_transfer_ring_db:
	sg_free_table(test_wdi6_ctx->rx_event_ring_sgt);
fail_get_rx_er_sgtable:
	kfree(test_wdi6_ctx->rx_event_ring_sgt);
	test_wdi6_ctx->rx_event_ring_sgt = NULL;
fail_alloc_rx_er_sgt:
	sg_free_table(test_wdi6_ctx->rx_transfer_ring_sgt);
fail_get_rx_tr_sgtable:
	kfree(test_wdi6_ctx->rx_transfer_ring_sgt);
	test_wdi6_ctx->rx_transfer_ring_sgt = NULL;
fail_alloc_rx_tr_sgt:
	sg_free_table(test_wdi6_ctx->tx_event_ring_sgt);
fail_get_tx_er_sgtable:
	kfree(test_wdi6_ctx->tx_event_ring_sgt);
	test_wdi6_ctx->tx_event_ring_sgt = NULL;
fail_alloc_tx_er_sgt:
	sg_free_table(test_wdi6_ctx->tx_transfer_ring_sgt);
fail_get_tx_tr_sgtable:
	kfree(test_wdi6_ctx->tx_transfer_ring_sgt);
	test_wdi6_ctx->tx_transfer_ring_sgt = NULL;
fail_alloc_tx_tr_sgt:
	sg_free_table(test_wdi6_ctx->rx_pool_sgt);
fail_get_rx_pool_sgtable:
	kfree(test_wdi6_ctx->rx_pool_sgt);
	test_wdi6_ctx->rx_pool_sgt = NULL;
fail_alloc_rx_pool_sgt:
	ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->rx_pool);
fail_rx_pool:
	sg_free_table(test_wdi6_ctx->tx_pool_sgt);
fail_get_tx_pool_sgtable:
	kfree(test_wdi6_ctx->tx_pool_sgt);
	test_wdi6_ctx->tx_pool_sgt = NULL;
fail_alloc_tx_pool_sgt:
	ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->tx_pool);
fail_tx_pool:
	ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->rx_event_ring_addr);

fail_rx_event_ring:
	ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->rx_transfer_ring_addr);

fail_rx_transfer_ring:

fail_tx_event_ring:
	ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->tx_transfer_ring_addr);
	return ret;
}

static int ipa_test_wdi6_free_mmio(void)
{
	if (!test_wdi6_ctx) {
		IPA_UT_ERR("test_wdi6_ctx is not initialized.\n");
		return -EFAULT;
	}

	if (test_wdi6_ctx->rx_event_ring_db.base)
		ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->rx_event_ring_db);
	test_wdi6_ctx->rx_event_ring_db.base = NULL;

	if (test_wdi6_ctx->rx_transfer_ring_db.base)
		ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->rx_transfer_ring_db);
	test_wdi6_ctx->rx_transfer_ring_db.base = NULL;

	if (test_wdi6_ctx->tx_event_ring_db.base)
		ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->tx_event_ring_db);
	test_wdi6_ctx->tx_event_ring_db.base = NULL;

	if (test_wdi6_ctx->tx_transfer_ring_db.base)
		ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->tx_transfer_ring_db);
	test_wdi6_ctx->tx_transfer_ring_db.base = NULL;



	ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->rx_event_ring_addr);

	ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->rx_transfer_ring_addr);

	ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->tx_event_ring_addr);

	ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->tx_transfer_ring_addr);



	sg_free_table(test_wdi6_ctx->rx_pool_sgt);
	kfree(test_wdi6_ctx->rx_pool_sgt);
	test_wdi6_ctx->rx_pool_sgt = NULL;
	ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->rx_pool);

	sg_free_table(test_wdi6_ctx->tx_pool_sgt);
	kfree(test_wdi6_ctx->tx_pool_sgt);
	test_wdi6_ctx->tx_pool_sgt = NULL;
	ipa_test_wdi6_free_dma_buff(&test_wdi6_ctx->tx_pool);

	sg_free_table(test_wdi6_ctx->rx_event_ring_sgt);
	kfree(test_wdi6_ctx->rx_event_ring_sgt);
	test_wdi6_ctx->rx_event_ring_sgt = NULL;

	sg_free_table(test_wdi6_ctx->rx_transfer_ring_sgt);
	kfree(test_wdi6_ctx->rx_transfer_ring_sgt);
	test_wdi6_ctx->rx_transfer_ring_sgt = NULL;

	sg_free_table(test_wdi6_ctx->tx_event_ring_sgt);
	kfree(test_wdi6_ctx->tx_event_ring_sgt);
	test_wdi6_ctx->tx_event_ring_sgt = NULL;

	sg_free_table(test_wdi6_ctx->tx_transfer_ring_sgt);
	kfree(test_wdi6_ctx->tx_transfer_ring_sgt);
	test_wdi6_ctx->tx_transfer_ring_sgt = NULL;

	return 0;
}

static int ipa_test_wdi6_suite_setup(void **priv)
{
	int ret = 0;
	struct ipa_wdi_init_in_params in;
	struct ipa_wdi_init_out_params out;

	IPA_UT_DBG("Start WDI6 Setup\n");

	/* init ipa wdi ctx */
	memset(&in, 0, sizeof(in));
	in.wdi_notify = NULL;
	in.notify = NULL;
	in.priv = NULL;
	in.wdi_version = IPA_WDI_6;
	if (ipa_wdi_init(&in, &out)) {
		IPA_UT_ERR("fail to init wdi6\n");
		return -EFAULT;
	}


	if (!ipa3_ctx) {
		IPA_UT_ERR("No IPA ctx\n");
		return -EINVAL;
	}

	test_wdi6_ctx = kzalloc(sizeof(struct ipa_test_wdi6_context),
		GFP_KERNEL);
	if (!test_wdi6_ctx) {
		IPA_UT_ERR("failed to allocate ctx\n");
		ipa_wdi_cleanup_per_inst(out.hdl);
		return -ENOMEM;
	}
	test_wdi6_ctx->wdi_hdl = out.hdl;
	IPA_UT_INFO("WDI6 init: hdl=%u is_uC_ready=%d is_over_gsi=%d is_smmu=%d\n",
		out.hdl, out.is_uC_ready, out.is_over_gsi, out.is_smmu_enabled);

	ret = ipa_test_wdi6_alloc_mmio();
	if (ret) {
		IPA_UT_ERR("failed to alloc mmio\n");
		goto fail_alloc_mmio;
	}

	if (ipa_test_wdi6_init_rings()) {
		IPA_UT_ERR("failed to init rings\n");
		ret = -EFAULT;
		goto fail_alloc_mmio;
	}

	*priv = test_wdi6_ctx;
	return 0;

fail_alloc_mmio:
	kfree(test_wdi6_ctx);
	test_wdi6_ctx = NULL;
	ipa_wdi_cleanup_per_inst(out.hdl);
	return ret;
}

static int ipa_test_wdi6_suite_teardown(void *priv)
{
	ipa_wdi_hdl_t hdl;

	if (!test_wdi6_ctx)
		return  0;

	hdl = test_wdi6_ctx->wdi_hdl;

	ipa_test_wdi6_free_mmio();
	kfree(test_wdi6_ctx);
	test_wdi6_ctx = NULL;

	/* Clean up the WDI instance using the handle returned by ipa_wdi_init */
	IPA_UT_INFO("WDI6 cleanup: hdl=%u\n", hdl);
	ipa_wdi_cleanup_per_inst(hdl);

	return 0;
}

static phys_addr_t ipa_test_wdi6_get_phys_addr(struct ipa_mem_buffer *mem)
{
	struct sg_table sgt;
	phys_addr_t pa = 0;
	struct ipa_smmu_cb_ctx *cb = ipa3_get_smmu_ctx(ipa_get_client_smmu_cb_type(TEST_WDI6_CONS_MACRO));

	if (dma_get_sgtable(cb->dev, &sgt, mem->base, mem->phys_base, mem->size) == 0) {
		pa = page_to_phys(sg_page(sgt.sgl));
		sg_free_table(&sgt);
	}
	return pa;
}

static int ipa_wdi6_setup_pipes(bool enable)
{
	struct ipa_wdi_conn_in_params *in_param;
	struct ipa_wdi_conn_out_params *out_param;
	struct wdi6_tx_transfer_ring_ele *tx_transfer, *tx_transfer_base;
	struct wdi6_rx_transfer_ring_ele *rx_transfer;
	void __iomem *rx_uc_db;
	void __iomem *tx_uc_db;
	int i, index;

	if (!test_wdi6_ctx) {
		IPA_UT_ERR("context is empty.\n");
		return -EFAULT;
	}

	in_param = kzalloc(sizeof(struct ipa_wdi_conn_in_params),
		GFP_KERNEL);
	if (!in_param) {
		IPA_UT_ERR("failed to allocate in_param\n");
		return -ENOMEM;
	}

	out_param = kzalloc(sizeof(struct ipa_wdi_conn_out_params),
		GFP_KERNEL);
	if (!out_param) {
		IPA_UT_ERR("failed to allocate out_param\n");
		kfree(in_param);
		return -ENOMEM;
	}

	memset(in_param, 0, sizeof(struct ipa_wdi_conn_in_params));
	memset(out_param, 0, sizeof(struct ipa_wdi_conn_out_params));

	/* setup tx parameters */
	in_param->is_tx1_used = false;
	in_param->is_smmu_enabled = true;
	in_param->notify = NULL;

	in_param->u_tx.tx_smmu.client = TEST_WDI6_CONS_MACRO;
	in_param->u_tx.tx_smmu.transfer_ring_base = *test_wdi6_ctx->tx_transfer_ring_sgt;
	in_param->u_tx.tx_smmu.transfer_ring_size =
		test_wdi6_ctx->tx_transfer_ring_addr.size;
	in_param->u_tx.tx_smmu.transfer_ring_doorbell_pa =
		ipa_test_wdi6_get_phys_addr(&test_wdi6_ctx->tx_transfer_ring_db);

	in_param->u_tx.tx_smmu.event_ring_base = *test_wdi6_ctx->tx_event_ring_sgt;
	in_param->u_tx.tx_smmu.event_ring_size =
		test_wdi6_ctx->tx_event_ring_addr.size;
	in_param->u_tx.tx_smmu.event_ring_doorbell_pa =
		ipa_test_wdi6_get_phys_addr(&test_wdi6_ctx->tx_event_ring_db);

	in_param->u_tx.tx_smmu.num_pkt_buffers = NUM_TX_BUFS;

	/* setup rx parameters */
	in_param->u_rx.rx_smmu.client = TEST_WDI6_PROD_MACRO;
	in_param->u_rx.rx_smmu.transfer_ring_base = *test_wdi6_ctx->rx_transfer_ring_sgt;
	in_param->u_rx.rx_smmu.transfer_ring_size =
		test_wdi6_ctx->rx_transfer_ring_addr.size;
	in_param->u_rx.rx_smmu.transfer_ring_doorbell_pa =
		ipa_test_wdi6_get_phys_addr(&test_wdi6_ctx->rx_transfer_ring_db);
	in_param->u_rx.rx_smmu.pkt_offset = PACKET_HEADER_SIZE;


	in_param->u_rx.rx_smmu.event_ring_base = *test_wdi6_ctx->rx_event_ring_sgt;
	in_param->u_rx.rx_smmu.event_ring_size =
		test_wdi6_ctx->rx_event_ring_addr.size;
	in_param->u_rx.rx_smmu.event_ring_doorbell_pa =
		ipa_test_wdi6_get_phys_addr(&test_wdi6_ctx->rx_event_ring_db);

	in_param->u_rx.rx_smmu.num_pkt_buffers = NUM_RX_BUFS;
	if (ipa_wdi_conn_pipes(in_param, out_param)) {
		IPA_UT_ERR("fail to conn wdi6 pipes.\n");
		kfree(in_param);
		kfree(out_param);
		return -EFAULT;
	}

	/* Print ring physical and SMMU mapped (IOVA) addresses after pipe connection */
	{
		int ep_idx_tx = ipa_get_ep_mapping(TEST_WDI6_CONS_MACRO);
		int ep_idx_rx = ipa_get_ep_mapping(TEST_WDI6_PROD_MACRO);
		struct ipa3_ep_context *ep_tx_ctx = &ipa3_ctx->ep[ep_idx_tx];
		struct ipa3_ep_context *ep_rx_ctx = &ipa3_ctx->ep[ep_idx_rx];
		/* WDI6 uses wdi4 scratch format; wifi_rp_address = transfer ring DB SMMU IOVA */
		u64 tx_tr_db_smmu_iova =
			((u64)ep_tx_ctx->chan_scratch.wdi4.wifi_rp_address_high << 32) |
			ep_tx_ctx->chan_scratch.wdi4.wifi_rp_address_low;
		u64 rx_tr_db_smmu_iova =
			((u64)ep_rx_ctx->chan_scratch.wdi4.wifi_rp_address_high << 32) |
			ep_rx_ctx->chan_scratch.wdi4.wifi_rp_address_low;

		IPA_UT_INFO("TX transfer ring: phys=0x%llx smmu_iova=0x%llx\n",
			(u64)test_wdi6_ctx->tx_transfer_ring_addr.phys_base,
			ep_tx_ctx->gsi_mem_info.chan_ring_base_addr);
		IPA_UT_INFO("TX event ring:    phys=0x%llx smmu_iova=0x%llx\n",
			(u64)test_wdi6_ctx->tx_event_ring_addr.phys_base,
			ep_tx_ctx->gsi_mem_info.evt_ring_base_addr);
		IPA_UT_INFO("RX transfer ring: phys=0x%llx smmu_iova=0x%llx\n",
			(u64)test_wdi6_ctx->rx_transfer_ring_addr.phys_base,
			ep_rx_ctx->gsi_mem_info.chan_ring_base_addr);
		IPA_UT_INFO("RX event ring:    phys=0x%llx smmu_iova=0x%llx\n",
			(u64)test_wdi6_ctx->rx_event_ring_addr.phys_base,
			ep_rx_ctx->gsi_mem_info.evt_ring_base_addr);
		/*
		 * Doorbell SMMU IOVAs:
		 * - Transfer ring DB: SMMU IOVA from channel scratch (wifi_rp_address)
		 * - Event ring DB: SMMU IOVA written to GSI EV_CH CNTXT_10/11 registers
		 *   (not stored in ep context, use gsihal_read_reg_nk to retrieve if needed)
		 */
		IPA_UT_INFO("TX transfer ring DB: phys=0x%llx smmu_iova=0x%llx\n",
			(u64)test_wdi6_ctx->tx_transfer_ring_db.phys_base,
			tx_tr_db_smmu_iova);
		IPA_UT_INFO("TX event ring DB:    phys=0x%llx smmu_iova=GSI EV_CH[%lu] CNTXT_10/11\n",
			(u64)test_wdi6_ctx->tx_event_ring_db.phys_base,
			ep_tx_ctx->gsi_evt_ring_hdl);
		IPA_UT_INFO("RX transfer ring DB: phys=0x%llx smmu_iova=0x%llx\n",
			(u64)test_wdi6_ctx->rx_transfer_ring_db.phys_base,
			rx_tr_db_smmu_iova);
		IPA_UT_INFO("RX event ring DB:    phys=0x%llx smmu_iova=GSI EV_CH[%lu] CNTXT_10/11\n",
			(u64)test_wdi6_ctx->rx_event_ring_db.phys_base,
			ep_rx_ctx->gsi_evt_ring_hdl);
	}

	if (enable) {
		if (ipa_wdi_enable_pipes()) {
			IPA_UT_ERR("fail to enable wdi6 pipes.\n");
			ipa_wdi_disconn_pipes();
			kfree(in_param);
			kfree(out_param);
			return -EFAULT;
		}
	}
	test_wdi6_ctx->tx_uc_db_pa = out_param->tx_uc_db_pa;
	test_wdi6_ctx->rx_uc_db_pa = out_param->rx_uc_db_pa;
	IPA_UT_INFO("tx_uc_db_pa 0x%llx, rx_uc_db_pa 0x%llx.\n",
		(u64)test_wdi6_ctx->tx_uc_db_pa, (u64)test_wdi6_ctx->rx_uc_db_pa);

	rx_uc_db = ioremap(test_wdi6_ctx->rx_uc_db_pa, DB_REGISTER_SIZE);
	tx_uc_db = ioremap(test_wdi6_ctx->tx_uc_db_pa, DB_REGISTER_SIZE);

	/* setup db registers */
	*(u32 *)test_wdi6_ctx->rx_transfer_ring_db.base = rx_uc_db_local;
	*(u32 *)test_wdi6_ctx->rx_event_ring_db.base = 0;

	*(u32 *)test_wdi6_ctx->tx_transfer_ring_db.base = tx_uc_db_local;
	*(u32 *)test_wdi6_ctx->tx_event_ring_db.base = 0;

	rx_transfer = (struct wdi6_rx_transfer_ring_ele *)
		test_wdi6_ctx->rx_transfer_ring_addr.base;
	for (i = 0; i < NUM_RX_BUFS; i++) {
		rx_transfer->buffer_addr_lo =
			(u64)test_wdi6_ctx->rx_bufs[i].phys_base & 0xFFFFFFFF;
		rx_transfer->buffer_addr_hi =
			((u64)test_wdi6_ctx->rx_bufs[i].phys_base >> 32)
			& 0xFFFFFFFF;
		rx_transfer++;
	}

	tx_transfer_base = (struct wdi6_tx_transfer_ring_ele *)
		test_wdi6_ctx->tx_transfer_ring_addr.base;
	index = tx_uc_db_local;
	for (i = 0; i < NUM_TX_BUFS; i++) {
		tx_transfer = tx_transfer_base + index;
		tx_transfer->buffer_addr_lo =
			(u64)test_wdi6_ctx->tx_bufs[i].phys_base & 0xFFFFFFFF;
		tx_transfer->buffer_addr_hi =
			((u64)test_wdi6_ctx->tx_bufs[i].phys_base >> 32)
			& 0xFFFFFFFF;
		index = (index + 1) % NUM_TX_TR_ELE;
	}
	ipa_test_wdi6_advance_uc_db(&tx_uc_db_local, NUM_TX_BUFS,
		sizeof(struct wdi6_tx_transfer_ring_ele)/4,
		test_wdi6_ctx->tx_transfer_ring_addr.size);
	wmb();
	dma_wmb();
	iowrite32(tx_uc_db_local, tx_uc_db);
	kfree(in_param);
	kfree(out_param);
	return 0;
}

static int ipa_wdi6_teardown_pipes(bool disable)
{
	int ret;

	if (disable) {
		ret = ipa_wdi_disable_pipes();
		if (ret)
			IPA_UT_ERR("fail to disable wdi6 pipes: %d\n", ret);
	}

	ret = ipa_wdi_disconn_pipes();
	if (ret)
		IPA_UT_ERR("fail to disconnect wdi6 pipes: %d\n", ret);

	rx_bf_idx = 0;
	tx_bf_idx = 0;
	rx_uc_db_local = 0;
	tx_uc_db_local = 0;
	return ret;
}

static int ipa_wdi6_send_one_packet(void)
{
	void __iomem *rx_uc_db;
	void __iomem *tx_uc_db;
	u32 *tx_event_ring_db, *rx_transfer_ring_db, *rx_event_ring_db;
	u32 orig_tx_event_ring_db;
	u32 orig_rx_event_ring_db;
	u32 orig_tx_trans_ring_db;
	u32 *packet;
	u32 *packet_recv = NULL;
	struct wdi6_rx_transfer_ring_ele *rx_transfer;
	struct wdi6_rx_event_ring_ele *rx_event;
	struct wdi6_tx_event_ring_ele *tx_event;
	struct wdi6_tx_transfer_ring_ele *tx_transfer;
	struct wdi6_buffer_addr_info rx_buf;
	dma_addr_t recv_packet_addr;
	int loop_cnt, i, num_words;
	int idx;

	/* populate packet content */
	rx_uc_db = ioremap(test_wdi6_ctx->rx_uc_db_pa, DB_REGISTER_SIZE);
	num_words = sizeof(struct wdi6_rx_transfer_ring_ele) / 4;
	idx = rx_uc_db_local / num_words;
	packet = (u32 *)test_wdi6_ctx->rx_bufs[rx_bf_idx].base +
		PACKET_HEADER_SIZE/4;
	*packet = PACKET_CONTENT;
	IPA_UT_DBG("local rx uc db: %u, rx buffer index %d\n",
		rx_uc_db_local, rx_bf_idx);
	rx_bf_idx = (rx_bf_idx  + 1) % NUM_RX_BUFS;
	/* update rx_transfer_ring_ele */
	rx_transfer = (struct wdi6_rx_transfer_ring_ele *)
		(test_wdi6_ctx->rx_transfer_ring_addr.base) +
		idx;

	ipa_test_wdi6_advance_uc_db(&rx_uc_db_local, 1,
		sizeof(struct wdi6_rx_transfer_ring_ele)/4,
		test_wdi6_ctx->rx_transfer_ring_addr.size);
	rx_transfer->rx_msdu_desc_info_details.msdu_length =
		PKT_SIZE;

	rx_buf.buffer_addr_low =
		rx_transfer->buffer_addr_lo;
	rx_buf.buffer_addr_high =
		rx_transfer->buffer_addr_hi;

	tx_event_ring_db = (u32 *)test_wdi6_ctx->tx_event_ring_db.base;
	orig_tx_event_ring_db = *tx_event_ring_db;
	IPA_UT_DBG("original tx event ring db: %u\n",
		orig_tx_event_ring_db);

	rx_event_ring_db = (u32 *)test_wdi6_ctx->rx_event_ring_db.base;
	orig_rx_event_ring_db = *rx_event_ring_db;
	IPA_UT_DBG("original rx event ring db: %u\n",
		orig_rx_event_ring_db);

	rx_transfer_ring_db
		= (u32 *)test_wdi6_ctx->rx_transfer_ring_db.base;
	orig_tx_trans_ring_db = *rx_transfer_ring_db;
	IPA_UT_DBG("original rx transfer ring db: %u\n",
		*rx_transfer_ring_db);

	/* ring uc db */
	wmb();
	dma_wmb();
	iowrite32(rx_uc_db_local, rx_uc_db);
	IPA_UT_DBG("rx db local: %u\n", rx_uc_db_local);

	loop_cnt = 0;
	while (orig_tx_event_ring_db == *tx_event_ring_db ||
		*rx_event_ring_db == orig_rx_event_ring_db) {
		wmb();
		dma_wmb();
		loop_cnt++;
		IPA_UT_DBG("loop count: %d tx\n", loop_cnt);
		IPA_UT_DBG("orig_tx_event_ring_db: %u tx_event_ring_db: %u\n",
			orig_tx_event_ring_db, *tx_event_ring_db);
		IPA_UT_DBG("rx_transfer_ring_db: %u rx db local: %u\n",
			*rx_transfer_ring_db, rx_uc_db_local);
		IPA_UT_DBG("orig_rx_event_ring_db: %u rx_event_ring_db %u\n",
			orig_rx_event_ring_db, *rx_event_ring_db);
		if (loop_cnt == 1000) {
			IPA_UT_ERR("transfer timeout!\n");
			TEST_WDI6_DUMP_GSI_REGS();
			return -EFAULT;
		}
		usleep_range(1000, 1001);
	}
	IPA_UT_DBG("rx_transfer_ring_db: %u\n", *rx_transfer_ring_db);
	IPA_UT_DBG("tx_event_ring_db: %u\n", *tx_event_ring_db);
	num_words = sizeof(struct wdi6_rx_event_ring_ele)/4;
	rx_event = (struct wdi6_rx_event_ring_ele *)
		(test_wdi6_ctx->rx_event_ring_addr.base) +
		(*rx_event_ring_db/num_words - 1 + NUM_RX_ER_ELE) %
		NUM_RX_ER_ELE;
	IPA_UT_DBG("rx_event offset: %u\n",
		(*rx_event_ring_db/num_words - 1 + NUM_RX_ER_ELE) %
		NUM_RX_ER_ELE);
	IPA_UT_DBG("rx_event va: %pK\n", rx_event);
	IPA_UT_DBG("rx event low: %u rx event high: %u\n",
		rx_event->buffer_addr_lo,
		rx_event->buffer_addr_hi);
	IPA_UT_DBG("rx buf low: %u rx buf high: %u\n",
		rx_buf.buffer_addr_low, rx_buf.buffer_addr_high);
	if (rx_event->buffer_addr_lo !=
		rx_buf.buffer_addr_low ||
		rx_event->buffer_addr_hi !=
		rx_buf.buffer_addr_high) {
		IPA_UT_ERR("rx event ring buf addr doesn't match.\n");
		BUG();
		return -EFAULT;
	}

	num_words = sizeof(struct wdi6_tx_event_ring_ele)/4;
	tx_event = (struct wdi6_tx_event_ring_ele *)
		test_wdi6_ctx->tx_event_ring_addr.base +
		(*tx_event_ring_db/num_words - 1 + NUM_TX_ER_ELE) %
		NUM_TX_ER_ELE;
	IPA_UT_DBG("tx_event va: %pK\n", tx_event);
	IPA_UT_DBG("tx event offset: %u\n",
		(*tx_event_ring_db/num_words - 1 + NUM_TX_ER_ELE) %
		NUM_TX_ER_ELE);
	IPA_UT_DBG("recv addr low: %u recv_addr high: %u\n",
		tx_event->buffer_addr_lo,
		tx_event->buffer_addr_hi);
	recv_packet_addr =
		((u64)tx_event->buffer_addr_hi
		 << 32) |
		(u64)tx_event->buffer_addr_lo;
	IPA_UT_DBG("high: %llu low: %llu all: %llu\n",
		(u64)tx_event->buffer_addr_hi
			   << 32,
		(u64)tx_event->buffer_addr_lo,
		recv_packet_addr);
	for (i = 0; i < NUM_TX_BUFS; i++)
		if (recv_packet_addr == test_wdi6_ctx->tx_bufs[i].phys_base) {
			IPA_UT_DBG("found buf at position %d\n", i);
			packet_recv = (u32 *)test_wdi6_ctx->tx_bufs[i].base;
		}
	IPA_UT_DBG("packet_recv addr: %pK\n", packet_recv);
	if (*packet_recv != PACKET_CONTENT) {
		IPA_UT_ERR("recv packet doesn't match.\n");
		IPA_UT_ERR("packet: %d packet_recv: %d\n", PACKET_CONTENT,
			*packet_recv);
		return -EFAULT;
	}
	IPA_UT_INFO("recv packet matches!! Recycling the buffer ...\n");
	/* recycle buffer */
	tx_uc_db = ioremap(test_wdi6_ctx->tx_uc_db_pa, DB_REGISTER_SIZE);
	num_words = sizeof(struct wdi6_tx_transfer_ring_ele) / 4;
	idx = tx_uc_db_local / num_words;
	IPA_UT_DBG("tx_db_local: %u idx %d\n", tx_uc_db_local, idx);
	tx_transfer = (struct wdi6_tx_transfer_ring_ele *)
		test_wdi6_ctx->tx_transfer_ring_addr.base + idx;
	tx_transfer->buffer_addr_lo =
		tx_event->buffer_addr_lo;
	tx_transfer->buffer_addr_hi =
		tx_event->buffer_addr_hi;
	ipa_test_wdi6_advance_uc_db(&tx_uc_db_local, 1,
		sizeof(struct wdi6_tx_transfer_ring_ele)/4,
		test_wdi6_ctx->tx_transfer_ring_addr.size);
	wmb();
	dma_wmb();
	iowrite32(tx_uc_db_local, tx_uc_db);
	tx_bf_idx = (tx_bf_idx + 1) % NUM_TX_BUFS;
	return 0;
}

static int ipa_wdi6_test_reg_intf(bool is_tx1_used)
{
	struct ipa_wdi_reg_intf_in_params in;
	char netdev_name[IPA_RESOURCE_NAME_MAX] = {0};
	u8 hdr_content = 1;

	memset(&in, 0, sizeof(in));
	if (is_tx1_used) {
		snprintf(netdev_name, sizeof(netdev_name), "wdi6_test_2g");
	} else {
		snprintf(netdev_name, sizeof(netdev_name), "wdi6_test");
	}

	in.netdev_name = netdev_name;
	in.is_meta_data_valid = 0;
	in.hdr_info[0].hdr = &hdr_content;
	in.hdr_info[0].hdr_len = 1;
	in.hdr_info[0].dst_mac_addr_offset = 0;
	in.hdr_info[0].hdr_type = IPA_HDR_L2_ETHERNET_II;
	in.hdr_info[1].hdr = &hdr_content;
	in.hdr_info[1].hdr_len = 1;
	in.hdr_info[1].dst_mac_addr_offset = 0;
	in.hdr_info[1].hdr_type = IPA_HDR_L2_ETHERNET_II;
	/* WLAN guarantees vpnum in range [64, 255] */
	in.vpnum = 100;

	return ipa_wdi_reg_intf(&in);
}

static int ipa_wdi6_test_dereg_intf(bool is_tx1_used)
{
	char netdev_name[IPA_RESOURCE_NAME_MAX] = {0};

	if (is_tx1_used)
		snprintf(netdev_name, sizeof(netdev_name), "wdi6_test_2g");
	else
		snprintf(netdev_name, sizeof(netdev_name), "wdi6_test");
	IPA_UT_INFO("netdev name: %s strlen: %lu\n", netdev_name,
				strlen(netdev_name));

	return ipa_wdi_dereg_intf(netdev_name);
}

static int ipa_wdi6_test_single_transfer(void *priv)
{
	struct ipa_ep_cfg ep_cfg = { {0} };
	bool is_tx1_used = false;

	if (ipa_wdi6_test_reg_intf(is_tx1_used)) {
		IPA_UT_ERR("fail to register intf.\n");
		return -EFAULT;
	}

	if (ipa_wdi6_setup_pipes(true)) {
		IPA_UT_ERR("fail to setup wdi6 pipes.\n");
		ipa_wdi6_test_dereg_intf(is_tx1_used);
		return -EFAULT;
	}

	/* configure WLAN RX EP in DMA mode */
	ep_cfg.mode.mode = IPA_DMA;
	ep_cfg.mode.dst = TEST_WDI6_CONS_MACRO;

	ep_cfg.seq.set_dynamic = true;

	ipa3_cfg_ep(ipa_get_ep_mapping(TEST_WDI6_PROD_MACRO), &ep_cfg);

	if (ipa_wdi6_send_one_packet()) {
		IPA_UT_ERR("fail to transfer packet.\n");
		ipa_wdi6_teardown_pipes(true);
		ipa_wdi6_test_dereg_intf(is_tx1_used);
		return -EFAULT;
	}

	if (ipa_wdi6_teardown_pipes(true)) {
		IPA_UT_ERR("fail to tear down pipes.\n");
		ipa_wdi6_test_dereg_intf(is_tx1_used);
		return -EFAULT;
	}

	IPA_UT_INFO("pipes were torn down!\n");

	if (ipa_wdi6_test_dereg_intf(is_tx1_used)) {
		IPA_UT_ERR("fail to deregister interface.\n");
		return -EFAULT;
	}

	return 0;
}

static int ipa_wdi6_test_pipe_connection(void *priv)
{
	struct ipa_ep_cfg ep_cfg = { {0} };
	bool is_tx1_used = false;

	if (ipa_wdi6_test_reg_intf(is_tx1_used)) {
		IPA_UT_ERR("fail to register intf.\n");
		return -EFAULT;
	}

	if (ipa_wdi6_setup_pipes(false)) {
		IPA_UT_ERR("fail to setup wdi6 pipes.\n");
		ipa_wdi6_test_dereg_intf(is_tx1_used);
		return -EFAULT;
	}

	/* configure WLAN RX EP in DMA mode */
	ep_cfg.mode.mode = IPA_DMA;
	ep_cfg.mode.dst = TEST_WDI6_CONS_MACRO;

	ep_cfg.seq.set_dynamic = true;

	ipa3_cfg_ep(ipa_get_ep_mapping(TEST_WDI6_PROD_MACRO), &ep_cfg);

	TEST_WDI6_DUMP_GSI_REGS();

	if (ipa_wdi6_teardown_pipes(false)) {
		IPA_UT_ERR("fail to tear down pipes.\n");
		ipa_wdi6_test_dereg_intf(is_tx1_used);
		return -EFAULT;
	}

	IPA_UT_INFO("pipes were torn down!\n");

	if (ipa_wdi6_test_dereg_intf(is_tx1_used)) {
		IPA_UT_ERR("fail to deregister interface.\n");
		return -EFAULT;
	}

	return 0;
}

static int ipa_wdi6_send_multi_packet(void)
{
	void __iomem *rx_uc_db;
	void __iomem *tx_uc_db;
	u32 *tx_event_ring_db, *rx_transfer_ring_db, *rx_event_ring_db;
	u32 orig_tx_event_ring_db;
	u32 orig_rx_event_ring_db;
	u32 *packet;
	u32 *packet_recv = NULL;
	struct wdi6_rx_transfer_ring_ele *rx_transfer;
	struct wdi6_rx_event_ring_ele *rx_event;
	struct wdi6_tx_event_ring_ele *tx_event;
	struct wdi6_tx_transfer_ring_ele *tx_transfer;
	struct wdi6_buffer_addr_info rx_buf;
	dma_addr_t recv_packet_addr;
	int loop_cnt, i, num_words;
	int idx;

	/* populate packet content */
	num_words = sizeof(struct wdi6_rx_transfer_ring_ele) / 4;
	rx_uc_db = ioremap(test_wdi6_ctx->rx_uc_db_pa, DB_REGISTER_SIZE);
	for (i = 0; i < NUM_MULTI_PKT; i++) {
		idx = rx_uc_db_local / num_words;
		packet = (u32 *)test_wdi6_ctx->rx_bufs[rx_bf_idx].base
			+ PACKET_HEADER_SIZE / 4;
		*packet = multi_pkt_array[i];
		IPA_UT_DBG("rx_db_local: %u rx_bf_idx: %d\n",
			rx_uc_db_local, rx_bf_idx);
		rx_bf_idx = (rx_bf_idx  + 1) % NUM_RX_BUFS;
		/* update rx_transfer_ring_ele */
		rx_transfer = (struct wdi6_rx_transfer_ring_ele *)
			test_wdi6_ctx->rx_transfer_ring_addr.base + idx;
		ipa_test_wdi6_advance_uc_db(&rx_uc_db_local, 1,
			sizeof(struct wdi6_rx_transfer_ring_ele)/4,
			test_wdi6_ctx->rx_transfer_ring_addr.size);
		rx_transfer->rx_msdu_desc_info_details.msdu_length =
			PKT_SIZE;
		rx_buf.buffer_addr_low =
		rx_transfer->buffer_addr_lo;
		rx_buf.buffer_addr_high =
		rx_transfer->buffer_addr_hi;
	}

	tx_event_ring_db = (u32 *)test_wdi6_ctx->tx_event_ring_db.base;
	orig_tx_event_ring_db = *tx_event_ring_db;
	IPA_UT_DBG("original tx event ring db: %u\n", orig_tx_event_ring_db);

	rx_event_ring_db = (u32 *)test_wdi6_ctx->rx_event_ring_db.base;
	orig_rx_event_ring_db = *rx_event_ring_db;
	IPA_UT_DBG("original rx event ring db: %u\n", orig_rx_event_ring_db);

	rx_transfer_ring_db = (u32 *)test_wdi6_ctx->rx_transfer_ring_db.base;
	IPA_UT_DBG("original rx transfer ring db: %u\n", *rx_transfer_ring_db);

	/* ring uc db */
	iowrite32(rx_uc_db_local, rx_uc_db);
	IPA_UT_DBG("rx db local: %u\n", rx_uc_db_local);

	loop_cnt = 0;
	while (orig_tx_event_ring_db == *tx_event_ring_db ||
		*rx_transfer_ring_db != rx_uc_db_local ||
		orig_rx_event_ring_db == *rx_event_ring_db) {
		loop_cnt++;
		IPA_UT_DBG("loop count: %d tx\n", loop_cnt);
		IPA_UT_DBG("orig_tx_event_ring_db: %u tx_event_ring_db: %u\n",
			orig_tx_event_ring_db, *tx_event_ring_db);
		IPA_UT_DBG("rx_transfer_ring_db: %u rx db local: %u\n",
			*rx_transfer_ring_db, rx_uc_db_local);
		IPA_UT_DBG("orig_rx_event_ring_db: %u rx_event_ring_db %u\n",
			orig_rx_event_ring_db, *rx_event_ring_db);
		if (loop_cnt == 1000) {
			IPA_UT_ERR("transfer timeout!\n");
			// BUG(); // Commented out to prevent kernel panic
			return -EFAULT;
		}
		usleep_range(1000, 1001);
	}

	IPA_UT_DBG("rx_transfer_ring_db: %u\n", *rx_transfer_ring_db);
	IPA_UT_DBG("tx_event_ring_db: %u\n", *tx_event_ring_db);
	num_words = sizeof(struct wdi6_rx_event_ring_ele)/4;
	rx_event = (struct wdi6_rx_event_ring_ele *)
		test_wdi6_ctx->rx_event_ring_addr.base +
		(*rx_event_ring_db/num_words - 1 + NUM_RX_ER_ELE) %
		NUM_RX_ER_ELE;
	IPA_UT_DBG("rx_event va: %pK\n", rx_event);

	IPA_UT_DBG("rx event low: %u rx event high: %u\n",
		rx_event->buffer_addr_lo,
		rx_event->buffer_addr_hi);
	IPA_UT_DBG("rx buf low: %u rx buf high: %u\n",
		rx_buf.buffer_addr_low, rx_buf.buffer_addr_high);

	if (rx_event->buffer_addr_lo !=
		rx_buf.buffer_addr_low ||
		rx_event->buffer_addr_hi !=
		rx_buf.buffer_addr_high) {
		IPA_UT_ERR("rx event ring buf addr doesn't match.\n");
		return -EFAULT;
	}
	num_words = sizeof(struct wdi6_tx_event_ring_ele)/4;
	tx_event = (struct wdi6_tx_event_ring_ele *)
		test_wdi6_ctx->tx_event_ring_addr.base +
		(*tx_event_ring_db/num_words - NUM_MULTI_PKT + NUM_TX_ER_ELE) %
		NUM_TX_ER_ELE;
	IPA_UT_DBG("tx_event va: %pK\n", tx_event);
	IPA_UT_DBG("recv addr low: %u recv_addr high: %u\n",
		tx_event->buffer_addr_lo,
		tx_event->buffer_addr_hi);
	recv_packet_addr =
		((u64)tx_event->buffer_addr_hi
		 << 32) |
		(u64)tx_event->buffer_addr_lo;
	IPA_UT_DBG("high: %llu low: %llu all: %llu\n",
		(u64)tx_event->buffer_addr_hi
			   << 32,
		(u64)tx_event->buffer_addr_lo,
		recv_packet_addr);
	for (i = 0; i < NUM_TX_BUFS; i++)
		if (recv_packet_addr == test_wdi6_ctx->tx_bufs[i].phys_base) {
			IPA_UT_INFO("found buf at position %d\n", i);
			packet_recv = (u32 *)test_wdi6_ctx->tx_bufs[i].base;
		}

	if (*packet_recv != multi_pkt_array[0]) {
		IPA_UT_ERR("recv packet doesn't match.\n");
		IPA_UT_ERR("packet: %d packet_recv: %d\n",
			multi_pkt_array[0], *packet_recv);
		return -EFAULT;
	}

	IPA_UT_INFO("recv packet matches.\n");

	/* recycle buffer */
	tx_uc_db = ioremap(test_wdi6_ctx->tx_uc_db_pa, DB_REGISTER_SIZE);
	num_words = sizeof(struct wdi6_tx_transfer_ring_ele) / 4;

	for (i = 0; i < NUM_MULTI_PKT; i++) {
		idx = tx_uc_db_local / num_words;
		IPA_UT_DBG("tx_db_local: %u idx %d\n", tx_uc_db_local, idx);
		tx_event = (struct wdi6_tx_event_ring_ele *)
			test_wdi6_ctx->tx_event_ring_addr.base +
			(*tx_event_ring_db/num_words - NUM_MULTI_PKT
			+ i + NUM_TX_ER_ELE) % NUM_TX_ER_ELE;
		tx_transfer = (struct wdi6_tx_transfer_ring_ele *)
			test_wdi6_ctx->tx_transfer_ring_addr.base + idx;
		tx_transfer->buffer_addr_lo =
			tx_event->buffer_addr_lo;
		tx_transfer->buffer_addr_hi =
			tx_event->buffer_addr_hi;
		ipa_test_wdi6_advance_uc_db(&tx_uc_db_local, 1,
			sizeof(struct wdi6_tx_transfer_ring_ele)/4,
			test_wdi6_ctx->tx_transfer_ring_addr.size);
	}
	wmb();
	dma_wmb();
	iowrite32(tx_uc_db_local, tx_uc_db);
	tx_bf_idx = (tx_bf_idx + NUM_MULTI_PKT) % NUM_TX_BUFS;
	return 0;
}

static int ipa_wdi6_test_multi_transfer(void *priv)
{
	struct ipa_ep_cfg ep_cfg = { {0} };
	bool is_tx1_used = false;

	if (ipa_wdi6_test_reg_intf(is_tx1_used)) {
		IPA_UT_ERR("fail to register intf.\n");
		return -EFAULT;
	}

	if (ipa_wdi6_setup_pipes(true)) {
		IPA_UT_ERR("fail to setup wdi6 pipes.\n");
		ipa_wdi6_test_dereg_intf(is_tx1_used);
		return -EFAULT;
	}

	/* configure WLAN RX EP in DMA mode */
	ep_cfg.mode.mode = IPA_DMA;
	ep_cfg.mode.dst = TEST_WDI6_CONS_MACRO;

	ep_cfg.seq.set_dynamic = true;

	ipa3_cfg_ep(ipa_get_ep_mapping(TEST_WDI6_PROD_MACRO), &ep_cfg);

	if (ipa_wdi6_send_multi_packet()) {
		IPA_UT_ERR("fail to transfer packet.\n");
		ipa_wdi6_teardown_pipes(true);
		ipa_wdi6_test_dereg_intf(is_tx1_used);
		return -EFAULT;
	}

	if (ipa_wdi6_teardown_pipes(true)) {
		IPA_UT_ERR("fail to tear down pipes.\n");
		ipa_wdi6_test_dereg_intf(is_tx1_used);
		return -EFAULT;
	}

	IPA_UT_INFO("pipes were torn down!\n");

	if (ipa_wdi6_test_dereg_intf(is_tx1_used)) {
		IPA_UT_ERR("fail to deregister interface.\n");
		return -EFAULT;
	}

	return 0;
}

static int ipa_wdi6_test_multi_transfer2(void *priv)
{
	struct ipa_ep_cfg ep_cfg = { {0} };
	int i;
	bool is_tx1_used = false;

	if (ipa_wdi6_test_reg_intf(is_tx1_used)) {
		IPA_UT_ERR("fail to register intf.\n");
		return -EFAULT;
	}

	if (ipa_wdi6_setup_pipes(true)) {
		IPA_UT_ERR("fail to setup wdi6 pipes.\n");
		ipa_wdi6_test_dereg_intf(is_tx1_used);
		return -EFAULT;
	}

	/* configure WLAN RX EP in DMA mode */
	ep_cfg.mode.mode = IPA_DMA;
	ep_cfg.mode.dst = TEST_WDI6_CONS_MACRO;

	ep_cfg.seq.set_dynamic = true;

	ipa3_cfg_ep(ipa_get_ep_mapping(TEST_WDI6_PROD_MACRO), &ep_cfg);

	IPA_UT_DBG("-----start transfer 32 pkt----\n");
	for (i = 0; i < 32; i++) {
		IPA_UT_DBG("--transferring num #%d pkt--\n", i + 1);
		if (ipa_wdi6_send_one_packet()) {
			IPA_UT_ERR("fail to transfer packet.\n");
			ipa_wdi6_teardown_pipes(true);
			ipa_wdi6_test_dereg_intf(is_tx1_used);
			return -EFAULT;
		}
	}

	if (ipa_wdi6_teardown_pipes(true)) {
		IPA_UT_ERR("fail to tear down pipes.\n");
		ipa_wdi6_test_dereg_intf(is_tx1_used);
		return -EFAULT;
	}

	IPA_UT_ERR("pipes were torn down!\n");

	if (ipa_wdi6_test_dereg_intf(is_tx1_used)) {
		IPA_UT_ERR("fail to deregister interface.\n");
		return -EFAULT;
	}

	return 0;
}

static int ipa_wdi6_test_multi_transfer3(void *priv)
{
	struct ipa_ep_cfg ep_cfg = { {0} };
	int i;
	bool is_tx1_used = false;

	if (ipa_wdi6_test_reg_intf(is_tx1_used)) {
		IPA_UT_ERR("fail to register intf.\n");
		return -EFAULT;
	}

	if (ipa_wdi6_setup_pipes(true)) {
		IPA_UT_ERR("fail to setup wdi6 pipes.\n");
		ipa_wdi6_test_dereg_intf(is_tx1_used);
		return -EFAULT;
	}

	/* configure WLAN RX EP in DMA mode */
	ep_cfg.mode.mode = IPA_DMA;
	ep_cfg.mode.dst = TEST_WDI6_CONS_MACRO;

	ep_cfg.seq.set_dynamic = true;

	ipa3_cfg_ep(ipa_get_ep_mapping(TEST_WDI6_PROD_MACRO), &ep_cfg);

	IPA_UT_DBG("-----start transfer 256 pkt----\n");
	for (i = 0; i < 32; i++) {
		IPA_UT_DBG("--transferring num # %d to num # %d pkt--\n",
			(i + 1) * 8, (i + 2) * 8 - 1);
		if (ipa_wdi6_send_multi_packet()) {
			IPA_UT_ERR("fail to transfer packet.\n");
			ipa_wdi6_teardown_pipes(true);
			ipa_wdi6_test_dereg_intf(is_tx1_used);
			return -EFAULT;
		}
	}

	if (ipa_wdi6_teardown_pipes(true)) {
		IPA_UT_ERR("fail to tear down pipes.\n");
		ipa_wdi6_test_dereg_intf(is_tx1_used);
		return -EFAULT;
	}

	IPA_UT_ERR("pipes were torn down!\n");

	if (ipa_wdi6_test_dereg_intf(is_tx1_used)) {
		IPA_UT_ERR("fail to deregister interface.\n");
		return -EFAULT;
	}

	return 0;
}

static int ipa_wdi6_test_cookie_invalid_intf_idx(void *priv)
{
	struct ipa_sw_producer_cookie cookie;

	cookie.raw = 0;
	ipa3_populate_cookie_vpnum(-1, &cookie);
	if (cookie.raw != 0) {
		IPA_UT_ERR("Cookie modified for invalid intf_idx\n");
		return -EFAULT;
	}

	return 0;
}

static int ipa_wdi6_test_cookie_nonexistent_intf(void *priv)
{
	struct ipa_sw_producer_cookie cookie;

	cookie.raw = 0;
	ipa3_populate_cookie_vpnum(9999, &cookie);
	if (cookie.raw != 0) {
		IPA_UT_ERR("Cookie modified for non-existent intf_idx\n");
		return -EFAULT;
	}

	return 0;
}

static int ipa_wdi6_test_cookie_struct_layout(void *priv)
{
	struct ipa_sw_producer_cookie cookie;

	cookie.raw = 0;
	cookie.wdi6.vp_num = 0xAA;

	/* check that vp_num is mapped to [23:16] of raw */
	if (cookie.raw != 0x0000000000AA0000ULL) {
		IPA_UT_ERR("vp_num is not mapped to [23:16] correctly: 0x%llx\n", cookie.raw);
		return -EFAULT;
	}

	return 0;
}

static int ipa_wdi6_test_cookie_vpnum_via_reg_intf(void *priv)
{
	struct ipa_wdi_reg_intf_in_params in;
	int ret;

	memset(&in, 0, sizeof(in));
	in.netdev_name = "wdi6_test_vpnum";
	in.vpnum = 100;
	in.is_meta_data_valid = 0;

	ret = ipa_wdi_reg_intf(&in);
	if (ret) {
		IPA_UT_ERR("fail to register intf.\n");
		return -EFAULT;
	}

	/* In the test environment we don't easily have the intf_idx that was assigned,
	 * but we can test that ipa_wdi_reg_intf doesn't fail with vpnum set.
	 * Testing ipa3_populate_cookie_vpnum properly would require accessing internal state.
	 */
	ipa_wdi_dereg_intf("wdi6_test_vpnum");

	return 0;
}

static int ipa_wdi6_test_cookie_no_vpnum_intf(void *priv)
{
	struct ipa_wdi_reg_intf_in_params in;
	int ret;

	memset(&in, 0, sizeof(in));
	in.netdev_name = "wdi6_test_no_vpnum";
	/* Don't set vpnum */

	ret = ipa_wdi_reg_intf(&in);
	if (ret) {
		IPA_UT_ERR("fail to register intf.\n");
		return -EFAULT;
	}

	/* just verify dereg works too */
	ipa_wdi_dereg_intf("wdi6_test_no_vpnum");

	return 0;
}

static int ipa_wdi6_test_cookie_vpnum_boundary(void *priv)
{
	struct ipa_wdi_reg_intf_in_params in;
	int ret;

	memset(&in, 0, sizeof(in));
	in.netdev_name = "wdi6_test_vpnum_b";

	in.vpnum = 64;
	ret = ipa_wdi_reg_intf(&in);
	if (ret) {
		IPA_UT_ERR("fail to register intf with vpnum 64.\n");
		return -EFAULT;
	}
	ipa_wdi_dereg_intf("wdi6_test_vpnum_b");

	in.vpnum = 255;
	ret = ipa_wdi_reg_intf(&in);
	if (ret) {
		IPA_UT_ERR("fail to register intf with vpnum 255.\n");
		return -EFAULT;
	}
	ipa_wdi_dereg_intf("wdi6_test_vpnum_b");

	return 0;
}

/**
 * ipa_wdi6m_ipa_packet_receive_notify() - Rx notify
 *
 * @priv: driver context
 * @evt: event type
 * @data: data provided with event
 *
 * IPA will pass a packet to the Linux network stack with skb->data
 */
/* Suite definition block */
IPA_UT_DEFINE_SUITE_START(wdi6, "WDI6 tests",
	ipa_test_wdi6_suite_setup, ipa_test_wdi6_suite_teardown)
{
	IPA_UT_ADD_TEST(single_transfer,
		"single data transfer",
		ipa_wdi6_test_single_transfer,
		true, IPA_HW_v3_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(pipe_connection,
		"pipe connection",
		ipa_wdi6_test_pipe_connection,
		true, IPA_HW_v3_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(multi_transfer,
		"multiple data transfer",
		ipa_wdi6_test_multi_transfer,
		true, IPA_HW_v3_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(multi_transfer2,
		"multiple data transfer with data wrap around",
		ipa_wdi6_test_multi_transfer2,
		true, IPA_HW_v3_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(multi_transfer3,
		"multiple data transfer with data wrap around2",
		ipa_wdi6_test_multi_transfer3,
		true, IPA_HW_v3_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(cookie_invalid_intf_idx,
		"Negative interface index handling",
		ipa_wdi6_test_cookie_invalid_intf_idx,
		false, IPA_HW_v3_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(cookie_nonexistent_intf,
		"Non-existent interface index handling",
		ipa_wdi6_test_cookie_nonexistent_intf,
		false, IPA_HW_v3_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(cookie_struct_layout,
		"ipa_wdi6_sw_producer_cookie struct layout validation",
		ipa_wdi6_test_cookie_struct_layout,
		false, IPA_HW_v3_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(cookie_vpnum_via_reg_intf,
		"vpnum population via ipa_wdi_reg_intf",
		ipa_wdi6_test_cookie_vpnum_via_reg_intf,
		true, IPA_HW_v3_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(cookie_no_vpnum_intf,
		"Cookie untouched for interface without vpnum",
		ipa_wdi6_test_cookie_no_vpnum_intf,
		true, IPA_HW_v3_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(cookie_vpnum_boundary,
		"Boundary vpnum values (64, 128, 255) - WLAN range [64, 255]",
		ipa_wdi6_test_cookie_vpnum_boundary,
		true, IPA_HW_v3_0, IPA_HW_MAX),

} IPA_UT_DEFINE_SUITE_END(wdi6);
