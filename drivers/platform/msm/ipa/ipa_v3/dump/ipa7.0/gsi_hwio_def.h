// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.​
 */
 
#ifndef __GSI_HWIO_DEF_H__
#define __GSI_HWIO_DEF_H__
/*
===========================================================================
*/
/**
  @file gsi_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    GSI_TOP_GSI.*

  'Include' filters applied: <none>
  'Exclude' filters applied: RESERVED DUMMY 
*/

/*----------------------------------------------------------------------------
 * MODULE: GSI_TOP_GSI
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_cfg_s
{
  u32 gsi_enable : 1;
  u32 reserved0 : 3;
  u32 gsi_pwr_clps : 1;
  u32 bp_mtrix_disable : 1;
  u32 reserved1 : 2;
  u32 sleep_clk_div : 4;
  u32 reserved2 : 20;
};

/* Union definition of register */
union gsi_hwio_def_gsi_cfg_u
{
  struct gsi_hwio_def_gsi_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MANAGER_MCS_CODE_VER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_manager_mcs_code_ver_s
{
  u32 ver : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_manager_mcs_code_ver_u
{
  struct gsi_hwio_def_gsi_manager_mcs_code_ver_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_ZEROS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_zeros_s
{
  u32 zeros : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_zeros_u
{
  struct gsi_hwio_def_gsi_zeros_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_PERIPH_BASE_ADDR_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_periph_base_addr_lsb_s
{
  u32 base_addr : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_periph_base_addr_lsb_u
{
  struct gsi_hwio_def_gsi_periph_base_addr_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_PERIPH_BASE_ADDR_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_periph_base_addr_msb_s
{
  u32 base_addr : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_periph_base_addr_msb_u
{
  struct gsi_hwio_def_gsi_periph_base_addr_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_CGC_CTRL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_cgc_ctrl_s
{
  u32 region_1_hw_cgc_en : 1;
  u32 region_2_hw_cgc_en : 1;
  u32 region_3_hw_cgc_en : 1;
  u32 region_4_hw_cgc_en : 1;
  u32 region_5_hw_cgc_en : 1;
  u32 region_6_hw_cgc_en : 1;
  u32 region_7_hw_cgc_en : 1;
  u32 region_8_hw_cgc_en : 1;
  u32 region_9_hw_cgc_en : 1;
  u32 region_10_hw_cgc_en : 1;
  u32 region_11_hw_cgc_en : 1;
  u32 region_12_hw_cgc_en : 1;
  u32 region_13_hw_cgc_en : 1;
  u32 region_14_hw_cgc_en : 1;
  u32 region_15_hw_cgc_en : 1;
  u32 region_16_hw_cgc_en : 1;
  u32 region_17_hw_cgc_en : 1;
  u32 region_18_hw_cgc_en : 1;
  u32 region_19_hw_cgc_en : 1;
  u32 region_20_hw_cgc_en : 1;
  u32 reserved0 : 4;
  u32 region_25_32_hw_cgc_en : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_cgc_ctrl_u
{
  struct gsi_hwio_def_gsi_cgc_ctrl_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MOQA_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_moqa_cfg_s
{
  u32 client_req_prio : 8;
  u32 client_oord : 8;
  u32 client_oowr : 8;
  u32 reserved0 : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_moqa_cfg_u
{
  struct gsi_hwio_def_gsi_moqa_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_REE_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_ree_cfg_s
{
  u32 move_to_esc_clr_mode_trsh : 1;
  u32 channel_empty_int_enable : 1;
  u32 reserved0 : 6;
  u32 max_burst_size : 8;
  u32 ree_max_new_re_int : 4;
  u32 reserved1 : 12;
};

/* Union definition of register */
union gsi_hwio_def_gsi_ree_cfg_u
{
  struct gsi_hwio_def_gsi_ree_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_PERIPH_PENDING_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_periph_pending_n_s
{
  u32 chid_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_periph_pending_n_u
{
  struct gsi_hwio_def_gsi_periph_pending_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MSI_CACHEATTR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_msi_cacheattr_s
{
  u32 ashared : 1;
  u32 ainnershared : 1;
  u32 anoallocate : 1;
  u32 atransient : 1;
  u32 areqpriority : 2;
  u32 reserved0 : 26;
};

/* Union definition of register */
union gsi_hwio_def_gsi_msi_cacheattr_u
{
  struct gsi_hwio_def_gsi_msi_cacheattr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_EVENT_CACHEATTR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_event_cacheattr_s
{
  u32 ashared : 1;
  u32 ainnershared : 1;
  u32 anoallocate : 1;
  u32 atransient : 1;
  u32 areqpriority : 2;
  u32 reserved0 : 26;
};

/* Union definition of register */
union gsi_hwio_def_gsi_event_cacheattr_u
{
  struct gsi_hwio_def_gsi_event_cacheattr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DATA_CACHEATTR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_data_cacheattr_s
{
  u32 ashared : 1;
  u32 ainnershared : 1;
  u32 anoallocate : 1;
  u32 atransient : 1;
  u32 areqpriority : 2;
  u32 reserved0 : 26;
};

/* Union definition of register */
union gsi_hwio_def_gsi_data_cacheattr_u
{
  struct gsi_hwio_def_gsi_data_cacheattr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_TRE_CACHEATTR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_tre_cacheattr_s
{
  u32 ashared : 1;
  u32 ainnershared : 1;
  u32 anoallocate : 1;
  u32 atransient : 1;
  u32 areqpriority : 2;
  u32 reserved0 : 26;
};

/* Union definition of register */
union gsi_hwio_def_gsi_tre_cacheattr_u
{
  struct gsi_hwio_def_gsi_tre_cacheattr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_PROCESS_DESC_BCK_PRS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_process_desc_bck_prs_s
{
  u32 ree_dis_ch_comp_int : 1;
  u32 ree_new_re_int : 1;
  u32 ree_ch_empty_int : 1;
  u32 ev_eng_int : 1;
  u32 ie_int_mod_stop_int : 1;
  u32 ie_notify_mcs_int : 1;
  u32 csr_ch_ev_com_int : 1;
  u32 csr_generic_com_int : 1;
  u32 tlv_desc_proc_comp_int : 1;
  u32 tlv_stop_int : 1;
  u32 tlv_rst_int : 1;
  u32 tlv_ch_not_full_int : 1;
  u32 timer_eng_int : 1;
  u32 db_new_db_int : 1;
  u32 db_new_msi_db_int : 1;
  u32 rd_int : 1;
  u32 wr_int : 1;
  u32 ie_int_mod_cancel_int : 1;
  u32 reserved0 : 14;
};

/* Union definition of register */
union gsi_hwio_def_ic_process_desc_bck_prs_u
{
  struct gsi_hwio_def_ic_process_desc_bck_prs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_TLV_STOP_BCK_PRS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_tlv_stop_bck_prs_s
{
  u32 ree_dis_ch_comp_int : 1;
  u32 ree_new_re_int : 1;
  u32 ree_ch_empty_int : 1;
  u32 ev_eng_int : 1;
  u32 ie_int_mod_stop_int : 1;
  u32 ie_notify_mcs_int : 1;
  u32 csr_ch_ev_com_int : 1;
  u32 csr_generic_com_int : 1;
  u32 tlv_desc_proc_comp_int : 1;
  u32 tlv_stop_int : 1;
  u32 tlv_rst_int : 1;
  u32 tlv_ch_not_full_int : 1;
  u32 timer_eng_int : 1;
  u32 db_new_db_int : 1;
  u32 db_new_msi_db_int : 1;
  u32 rd_int : 1;
  u32 wr_int : 1;
  u32 ie_int_mod_cancel_int : 1;
  u32 reserved0 : 14;
};

/* Union definition of register */
union gsi_hwio_def_ic_tlv_stop_bck_prs_u
{
  struct gsi_hwio_def_ic_tlv_stop_bck_prs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_TLV_RESET_BCK_PRS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_tlv_reset_bck_prs_s
{
  u32 ree_dis_ch_comp_int : 1;
  u32 ree_new_re_int : 1;
  u32 ree_ch_empty_int : 1;
  u32 ev_eng_int : 1;
  u32 ie_int_mod_stop_int : 1;
  u32 ie_notify_mcs_int : 1;
  u32 csr_ch_ev_com_int : 1;
  u32 csr_generic_com_int : 1;
  u32 tlv_desc_proc_comp_int : 1;
  u32 tlv_stop_int : 1;
  u32 tlv_rst_int : 1;
  u32 tlv_ch_not_full_int : 1;
  u32 timer_eng_int : 1;
  u32 db_new_db_int : 1;
  u32 db_new_msi_db_int : 1;
  u32 rd_int : 1;
  u32 wr_int : 1;
  u32 ie_int_mod_cancel_int : 1;
  u32 reserved0 : 14;
};

/* Union definition of register */
union gsi_hwio_def_ic_tlv_reset_bck_prs_u
{
  struct gsi_hwio_def_ic_tlv_reset_bck_prs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_RGSTR_TIMER_BCK_PRS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_rgstr_timer_bck_prs_s
{
  u32 ree_dis_ch_comp_int : 1;
  u32 ree_new_re_int : 1;
  u32 ree_ch_empty_int : 1;
  u32 ev_eng_int : 1;
  u32 ie_int_mod_stop_int : 1;
  u32 ie_notify_mcs_int : 1;
  u32 csr_ch_ev_com_int : 1;
  u32 csr_generic_com_int : 1;
  u32 tlv_desc_proc_comp_int : 1;
  u32 tlv_stop_int : 1;
  u32 tlv_rst_int : 1;
  u32 tlv_ch_not_full_int : 1;
  u32 timer_eng_int : 1;
  u32 db_new_db_int : 1;
  u32 db_new_msi_db_int : 1;
  u32 rd_int : 1;
  u32 wr_int : 1;
  u32 ie_int_mod_cancel_int : 1;
  u32 reserved0 : 14;
};

/* Union definition of register */
union gsi_hwio_def_ic_rgstr_timer_bck_prs_u
{
  struct gsi_hwio_def_ic_rgstr_timer_bck_prs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_ASYNC_BCK_PRS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_async_bck_prs_s
{
  u32 ree_dis_ch_comp_int : 1;
  u32 ree_new_re_int : 1;
  u32 ree_ch_empty_int : 1;
  u32 ev_eng_int : 1;
  u32 ie_int_mod_stop_int : 1;
  u32 ie_notify_mcs_int : 1;
  u32 csr_ch_ev_com_int : 1;
  u32 csr_generic_com_int : 1;
  u32 tlv_desc_proc_comp_int : 1;
  u32 tlv_stop_int : 1;
  u32 tlv_rst_int : 1;
  u32 tlv_ch_not_full_int : 1;
  u32 timer_eng_int : 1;
  u32 db_new_db_int : 1;
  u32 db_new_msi_db_int : 1;
  u32 rd_int : 1;
  u32 wr_int : 1;
  u32 ie_int_mod_cancel_int : 1;
  u32 reserved0 : 14;
};

/* Union definition of register */
union gsi_hwio_def_ic_async_bck_prs_u
{
  struct gsi_hwio_def_ic_async_bck_prs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_INT_WEIGHT_REE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_int_weight_ree_s
{
  u32 stop_ch_comp_int_weight : 4;
  u32 new_re_int_weight : 4;
  u32 ch_empty_int_weight : 4;
  u32 reserved0 : 20;
};

/* Union definition of register */
union gsi_hwio_def_ic_int_weight_ree_u
{
  struct gsi_hwio_def_ic_int_weight_ree_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_INT_WEIGHT_EVT_ENG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_int_weight_evt_eng_s
{
  u32 evnt_eng_int_weight : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union gsi_hwio_def_ic_int_weight_evt_eng_u
{
  struct gsi_hwio_def_ic_int_weight_evt_eng_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_INT_WEIGHT_INT_ENG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_int_weight_int_eng_s
{
  u32 int_mod_stop_int_weight : 4;
  u32 notify_mcs_int_weight : 4;
  u32 int_mod_cancel_int_weight : 4;
  u32 reserved0 : 20;
};

/* Union definition of register */
union gsi_hwio_def_ic_int_weight_int_eng_u
{
  struct gsi_hwio_def_ic_int_weight_int_eng_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_INT_WEIGHT_CSR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_int_weight_csr_s
{
  u32 ch_cmd_int_weight : 4;
  u32 ee_generic_int_weight : 4;
  u32 reserved0 : 24;
};

/* Union definition of register */
union gsi_hwio_def_ic_int_weight_csr_u
{
  struct gsi_hwio_def_ic_int_weight_csr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_INT_WEIGHT_TLV_ENG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_int_weight_tlv_eng_s
{
  u32 tlv_0_int_weight : 4;
  u32 tlv_1_int_weight : 4;
  u32 tlv_2_int_weight : 4;
  u32 ch_not_full_int_weight : 4;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_ic_int_weight_tlv_eng_u
{
  struct gsi_hwio_def_ic_int_weight_tlv_eng_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_INT_WEIGHT_TIMER_ENG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_int_weight_timer_eng_s
{
  u32 timer_int_weight : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union gsi_hwio_def_ic_int_weight_timer_eng_u
{
  struct gsi_hwio_def_ic_int_weight_timer_eng_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_INT_WEIGHT_DB_ENG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_int_weight_db_eng_s
{
  u32 new_db_int_weight : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union gsi_hwio_def_ic_int_weight_db_eng_u
{
  struct gsi_hwio_def_ic_int_weight_db_eng_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_INT_WEIGHT_RD_ENG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_int_weight_rd_eng_s
{
  u32 read_int_weight : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union gsi_hwio_def_ic_int_weight_rd_eng_u
{
  struct gsi_hwio_def_ic_int_weight_rd_eng_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_INT_WEIGHT_WR_ENG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_int_weight_wr_eng_s
{
  u32 write_int_weight : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union gsi_hwio_def_ic_int_weight_wr_eng_u
{
  struct gsi_hwio_def_ic_int_weight_wr_eng_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: LOW_LATENCY_ARB_WEIGHT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_low_latency_arb_weight_s
{
  u32 ll_weight : 6;
  u32 reserved0 : 2;
  u32 non_ll_weight : 6;
  u32 reserved1 : 2;
  u32 ll_non_ll_fix_priority : 1;
  u32 reserved2 : 15;
};

/* Union definition of register */
union gsi_hwio_def_low_latency_arb_weight_u
{
  struct gsi_hwio_def_low_latency_arb_weight_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IC_INT_WEIGHT_SDMA 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ic_int_weight_sdma_s
{
  u32 sdma_int_0_weight : 4;
  u32 sdma_int_1_weight : 4;
  u32 sdma_int_2_weight : 4;
  u32 sdma_int_3_weight : 4;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_ic_int_weight_sdma_u
{
  struct gsi_hwio_def_ic_int_weight_sdma_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SDMA_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_sdma_cfg_s
{
  u32 max_os_write : 4;
  u32 max_os_read : 4;
  u32 max_burst_write : 8;
  u32 max_burst_read : 8;
  u32 reserved0 : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_sdma_cfg_u
{
  struct gsi_hwio_def_gsi_sdma_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SDMA_CACHEATTR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_sdma_cacheattr_s
{
  u32 ashared : 1;
  u32 ainnershared : 1;
  u32 anoallocate : 1;
  u32 atransient : 1;
  u32 areqpriority : 2;
  u32 reserved0 : 26;
};

/* Union definition of register */
union gsi_hwio_def_gsi_sdma_cacheattr_u
{
  struct gsi_hwio_def_gsi_sdma_cacheattr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SDMA_SG_IOVEC_LSB_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_sdma_sg_iovec_lsb_n_s
{
  u32 sdma_sg_iovec_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_sdma_sg_iovec_lsb_n_u
{
  struct gsi_hwio_def_gsi_sdma_sg_iovec_lsb_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SDMA_SG_IOVEC_MSB_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_sdma_sg_iovec_msb_n_s
{
  u32 sdma_sg_iovec_msb : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_sdma_sg_iovec_msb_n_u
{
  struct gsi_hwio_def_gsi_sdma_sg_iovec_msb_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MANAGER_EE_QOS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_manager_ee_qos_n_s
{
  u32 ee_prio : 2;
  u32 reserved0 : 6;
  u32 max_ch_alloc : 8;
  u32 max_ev_alloc : 8;
  u32 reserved1 : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_manager_ee_qos_n_u
{
  struct gsi_hwio_def_gsi_manager_ee_qos_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_CH_CNTXT_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_ch_cntxt_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_ch_cntxt_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_ch_cntxt_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_EV_CNTXT_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_ev_cntxt_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_ev_cntxt_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_ev_cntxt_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_RE_STORAGE_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_re_storage_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_re_storage_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_re_storage_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_RE_ESC_BUF_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_re_esc_buf_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_re_esc_buf_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_re_esc_buf_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_mcs_scratch_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_mcs_scratch_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_mcs_scratch_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_mcs_scratch1_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_mcs_scratch1_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_mcs_scratch1_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_mcs_scratch2_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_mcs_scratch2_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_mcs_scratch2_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_mcs_scratch3_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_mcs_scratch3_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_mcs_scratch3_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_mcs_scratch4_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_mcs_scratch4_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_mcs_scratch4_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_ee_scratch_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_ee_scratch_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_ee_scratch_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_WR_ENG_REORDER_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_wr_eng_reorder_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_wr_eng_reorder_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_wr_eng_reorder_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_COMMON_COMP_Q_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_common_comp_q_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_common_comp_q_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_common_comp_q_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_mcs_stats_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_mcs_stats_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_mcs_stats_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_USER_INFO_DATA_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_user_info_data_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_user_info_data_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_user_info_data_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_EE_CMD_FIFO_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_ee_cmd_fifo_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_ee_cmd_fifo_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_ee_cmd_fifo_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_CH_CMD_FIFO_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_ch_cmd_fifo_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_ch_cmd_fifo_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_ch_cmd_fifo_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_EVE_ED_STORAGE_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_eve_ed_storage_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_eve_ed_storage_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_eve_ed_storage_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_func_stack_base_addr_n_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_func_stack_base_addr_n_u
{
  struct gsi_hwio_def_gsi_shram_ptr_func_stack_base_addr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_TIMER_ENG_USER_DATA_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_timer_eng_user_data_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_timer_eng_user_data_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_timer_eng_user_data_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_PTR_INT_ENG_REORDER_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_ptr_int_eng_reorder_base_addr_s
{
  u32 shram_ptr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_ptr_int_eng_reorder_base_addr_u
{
  struct gsi_hwio_def_gsi_shram_ptr_int_eng_reorder_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_CH_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_ch_cmd_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_ch_cmd_u
{
  struct gsi_hwio_def_gsi_iram_ptr_ch_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_EE_GENERIC_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_ee_generic_cmd_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_ee_generic_cmd_u
{
  struct gsi_hwio_def_gsi_iram_ptr_ee_generic_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_TLV_CH_NOT_FULL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_tlv_ch_not_full_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_tlv_ch_not_full_u
{
  struct gsi_hwio_def_gsi_iram_ptr_tlv_ch_not_full_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_MSI_DB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_msi_db_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_msi_db_u
{
  struct gsi_hwio_def_gsi_iram_ptr_msi_db_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_CH_DB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_ch_db_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_ch_db_u
{
  struct gsi_hwio_def_gsi_iram_ptr_ch_db_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_NEW_RE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_new_re_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_new_re_u
{
  struct gsi_hwio_def_gsi_iram_ptr_new_re_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_CH_DIS_COMP 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_ch_dis_comp_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_ch_dis_comp_u
{
  struct gsi_hwio_def_gsi_iram_ptr_ch_dis_comp_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_CH_EMPTY 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_ch_empty_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_ch_empty_u
{
  struct gsi_hwio_def_gsi_iram_ptr_ch_empty_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_EVENT_GEN_COMP 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_event_gen_comp_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_event_gen_comp_u
{
  struct gsi_hwio_def_gsi_iram_ptr_event_gen_comp_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_periph_if_tlv_in_0_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_periph_if_tlv_in_0_u
{
  struct gsi_hwio_def_gsi_iram_ptr_periph_if_tlv_in_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_periph_if_tlv_in_2_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_periph_if_tlv_in_2_u
{
  struct gsi_hwio_def_gsi_iram_ptr_periph_if_tlv_in_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_periph_if_tlv_in_1_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_periph_if_tlv_in_1_u
{
  struct gsi_hwio_def_gsi_iram_ptr_periph_if_tlv_in_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_TIMER_EXPIRED 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_timer_expired_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_timer_expired_u
{
  struct gsi_hwio_def_gsi_iram_ptr_timer_expired_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_WRITE_ENG_COMP 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_write_eng_comp_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_write_eng_comp_u
{
  struct gsi_hwio_def_gsi_iram_ptr_write_eng_comp_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_READ_ENG_COMP 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_read_eng_comp_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_read_eng_comp_u
{
  struct gsi_hwio_def_gsi_iram_ptr_read_eng_comp_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_INT_MOD_STOPED 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_int_mod_stoped_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_int_mod_stoped_u
{
  struct gsi_hwio_def_gsi_iram_ptr_int_mod_stoped_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_SDMA_INT_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_sdma_int_n_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_sdma_int_n_u
{
  struct gsi_hwio_def_gsi_iram_ptr_sdma_int_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_INT_NOTIFY_MCS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_int_notify_mcs_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_int_notify_mcs_u
{
  struct gsi_hwio_def_gsi_iram_ptr_int_notify_mcs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRAM_PTR_INT_MOD_CANCELED 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_iram_ptr_int_mod_canceled_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_iram_ptr_int_mod_canceled_u
{
  struct gsi_hwio_def_gsi_iram_ptr_int_mod_canceled_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_INST_RAM_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_inst_ram_0_s
{
  u32 inst_byte_0 : 8;
  u32 inst_byte_1 : 8;
  u32 inst_byte_2 : 8;
  u32 inst_byte_3 : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_inst_ram_0_u
{
  struct gsi_hwio_def_gsi_inst_ram_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SHRAM_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_shram_0_s
{
  u32 shram : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_shram_0_u
{
  struct gsi_hwio_def_gsi_shram_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MAP_EE_n_CH_k_VP_TABLE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_map_ee_n_ch_k_vp_table_s
{
  u32 phy_ch : 8;
  u32 valid : 1;
  u32 reserved0 : 23;
};

/* Union definition of register */
union gsi_hwio_def_gsi_map_ee_n_ch_k_vp_table_u
{
  struct gsi_hwio_def_gsi_map_ee_n_ch_k_vp_table_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_TEST_BUS_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_test_bus_sel_s
{
  u32 gsi_testbus_sel : 8;
  u32 reserved0 : 8;
  u32 gsi_hw_events_sel : 4;
  u32 reserved1 : 12;
};

/* Union definition of register */
union gsi_hwio_def_gsi_test_bus_sel_u
{
  struct gsi_hwio_def_gsi_test_bus_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_TEST_BUS_REG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_test_bus_reg_s
{
  u32 gsi_testbus_reg : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_test_bus_reg_u
{
  struct gsi_hwio_def_gsi_test_bus_reg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_BUSY_REG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_busy_reg_s
{
  u32 csr_busy : 1;
  u32 ree_busy : 1;
  u32 reserved0 : 1;
  u32 timer_busy : 1;
  u32 rd_wr_busy : 1;
  u32 ev_eng_busy : 1;
  u32 int_eng_busy : 1;
  u32 ree_pwr_clps_busy : 1;
  u32 db_eng_busy : 1;
  u32 dbg_cnt_busy : 1;
  u32 reserved1 : 1;
  u32 ic_busy : 1;
  u32 sdma_busy : 1;
  u32 db_msi_eng_busy : 1;
  u32 rd_busy : 1;
  u32 wr_busy : 1;
  u32 mutex_req_busy : 8;
  u32 mcs_busy : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_busy_reg_u
{
  struct gsi_hwio_def_gsi_debug_busy_reg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MUTEX_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mutex_sel_s
{
  u32 mcs_sel : 3;
  u32 reserved0 : 5;
  u32 line_sel : 8;
  u32 region_type_sel : 2;
  u32 reserved1 : 14;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mutex_sel_u
{
  struct gsi_hwio_def_gsi_debug_mutex_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MUTEX_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mutex_status_s
{
  u32 table_empty : 8;
  u32 table_full : 8;
  u32 lock_overflow : 8;
  u32 reserved0 : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mutex_status_u
{
  struct gsi_hwio_def_gsi_debug_mutex_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MUTEX_LOCKED_LINE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mutex_locked_line_s
{
  u32 line : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mutex_locked_line_u
{
  struct gsi_hwio_def_gsi_debug_mutex_locked_line_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MUTEX_REGION_TYPE_01 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mutex_region_type_01_s
{
  u32 reserved0 : 16;
  u32 region_type_1 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mutex_region_type_01_u
{
  struct gsi_hwio_def_gsi_debug_mutex_region_type_01_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MUTEX_REGION_TYPE_23 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mutex_region_type_23_s
{
  u32 region_type_2 : 16;
  u32 region_type_3 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mutex_region_type_23_u
{
  struct gsi_hwio_def_gsi_debug_mutex_region_type_23_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MUTEX_LOCK_CNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mutex_lock_cnt_s
{
  u32 val : 17;
  u32 reserved0 : 15;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mutex_lock_cnt_u
{
  struct gsi_hwio_def_gsi_debug_mutex_lock_cnt_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mutex_busy_wait_timer_lsb_s
{
  u32 val : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mutex_busy_wait_timer_lsb_u
{
  struct gsi_hwio_def_gsi_debug_mutex_busy_wait_timer_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mutex_busy_wait_timer_msb_s
{
  u32 val : 17;
  u32 reserved0 : 15;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mutex_busy_wait_timer_msb_u
{
  struct gsi_hwio_def_gsi_debug_mutex_busy_wait_timer_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MUTEX_CNT_RST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mutex_cnt_rst_s
{
  u32 cnt_reset : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mutex_cnt_rst_u
{
  struct gsi_hwio_def_gsi_debug_mutex_cnt_rst_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_EVENT_PENDING_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_event_pending_n_s
{
  u32 chid_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_event_pending_n_u
{
  struct gsi_hwio_def_gsi_debug_event_pending_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_TIMER_PENDING_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_timer_pending_n_s
{
  u32 chid_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_timer_pending_n_u
{
  struct gsi_hwio_def_gsi_debug_timer_pending_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_RD_WR_PENDING_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_rd_wr_pending_n_s
{
  u32 chid_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_rd_wr_pending_n_u
{
  struct gsi_hwio_def_gsi_debug_rd_wr_pending_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_PC_FROM_SW_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_pc_from_sw_n_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_pc_from_sw_n_u
{
  struct gsi_hwio_def_gsi_debug_pc_from_sw_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_LEGACY_BP_CNT_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_legacy_bp_cnt_lsb_s
{
  u32 cnt_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_legacy_bp_cnt_lsb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_legacy_bp_cnt_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_LEGACY_BP_CNT_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_legacy_bp_cnt_msb_s
{
  u32 cnt_msb : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_legacy_bp_cnt_msb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_legacy_bp_cnt_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_async_routine_bp_cnt_lsb_s
{
  u32 cnt_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_async_routine_bp_cnt_lsb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_async_routine_bp_cnt_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_async_routine_bp_cnt_msb_s
{
  u32 cnt_msb : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_async_routine_bp_cnt_msb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_async_routine_bp_cnt_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_reorder_fifo_bp_cnt_lsb_s
{
  u32 cnt_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_reorder_fifo_bp_cnt_lsb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_reorder_fifo_bp_cnt_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_reorder_fifo_bp_cnt_msb_s
{
  u32 cnt_msb : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_reorder_fifo_bp_cnt_msb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_reorder_fifo_bp_cnt_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_STATS_WR_MISS_INDICATION 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_stats_wr_miss_indication_s
{
  u32 wr_was_missed : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_stats_wr_miss_indication_u
{
  struct gsi_hwio_def_gsi_mcs_stats_wr_miss_indication_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_STATS_CH_EN_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_stats_ch_en_n_s
{
  u32 chid_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_stats_ch_en_n_u
{
  struct gsi_hwio_def_gsi_mcs_stats_ch_en_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_LOCK_BY_ORDER_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_lock_by_order_sel_s
{
  u32 sel_ch : 8;
  u32 sel_irq : 4;
  u32 sel_eng : 4;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_lock_by_order_sel_u
{
  struct gsi_hwio_def_gsi_debug_lock_by_order_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_LOCK_BY_ORDER_RD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_lock_by_order_rd_s
{
  u32 is_masked : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_lock_by_order_rd_u
{
  struct gsi_hwio_def_gsi_debug_lock_by_order_rd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mutex_current_busy_timer_msb_region_id_s
{
  u32 timer_msb : 16;
  u32 region_id : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mutex_current_busy_timer_msb_region_id_u
{
  struct gsi_hwio_def_gsi_debug_mutex_current_busy_timer_msb_region_id_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mutex_current_busy_timer_lsb_s
{
  u32 timer_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mutex_current_busy_timer_lsb_u
{
  struct gsi_hwio_def_gsi_debug_mutex_current_busy_timer_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MUTEX_TABLE_RD_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mutex_table_rd_n_s
{
  u32 region_id : 16;
  u32 reserved0 : 15;
  u32 locked : 1;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mutex_table_rd_n_u
{
  struct gsi_hwio_def_gsi_debug_mutex_table_rd_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_REORDER_FIFO_STATUS_EVE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_reorder_fifo_status_eve_s
{
  u32 fifo_size : 8;
  u32 busy_reserved_slots : 8;
  u32 reserved_slots : 8;
  u32 reserved0 : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_reorder_fifo_status_eve_u
{
  struct gsi_hwio_def_gsi_debug_reorder_fifo_status_eve_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_REORDER_FIFO_STATUS_IE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_reorder_fifo_status_ie_s
{
  u32 fifo_size : 8;
  u32 busy_reserved_slots : 8;
  u32 reserved_slots : 8;
  u32 reserved0 : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_reorder_fifo_status_ie_u
{
  struct gsi_hwio_def_gsi_debug_reorder_fifo_status_ie_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_REORDER_FIFO_STATUS_WRE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_reorder_fifo_status_wre_s
{
  u32 fifo_size : 8;
  u32 busy_reserved_slots : 8;
  u32 reserved_slots : 8;
  u32 reserved0 : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_reorder_fifo_status_wre_u
{
  struct gsi_hwio_def_gsi_debug_reorder_fifo_status_wre_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_COMM_COMP_Q_BUFFER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_comm_comp_q_buffer_s
{
  u32 buffer_size : 8;
  u32 busy_slots : 8;
  u32 buff_aempty : 1;
  u32 reserved0 : 15;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_comm_comp_q_buffer_u
{
  struct gsi_hwio_def_gsi_debug_comm_comp_q_buffer_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_comm_comp_q_routine_stt_n_s
{
  u32 busy_complete_slots : 8;
  u32 busy_active_complete_slots : 8;
  u32 head_idx : 8;
  u32 tail_idx : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_comm_comp_q_routine_stt_n_u
{
  struct gsi_hwio_def_gsi_debug_comm_comp_q_routine_stt_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_assign_ch_to_this_mcs_status_n_s
{
  u32 chid_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_assign_ch_to_this_mcs_status_n_u
{
  struct gsi_hwio_def_gsi_debug_assign_ch_to_this_mcs_status_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_CNT_EN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_cnt_en_s
{
  u32 en_set : 8;
  u32 en_clr : 8;
  u32 cnt_reset : 8;
  u32 bp_cnt_reset : 1;
  u32 bp_and_pending_cnt_reset : 1;
  u32 legacy_bp_cnt_reset : 1;
  u32 async_routine_bp_cnt_reset : 1;
  u32 reorder_bp_cnt_reset : 1;
  u32 reserved0 : 3;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_cnt_en_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_cnt_en_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_CNT_EN_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_cnt_en_status_s
{
  u32 en_status : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_cnt_en_status_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_cnt_en_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SPARE_REG_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_spare_reg_1_s
{
  u32 fix_ieob_wrong_msk_disable : 1;
  u32 reserved0 : 3;
  u32 gsi_extra_wr_ot_disable : 1;
  u32 gsi_extra_rd_ot_disable : 1;
  u32 reserved1 : 26;
};

/* Union definition of register */
union gsi_hwio_def_gsi_spare_reg_1_u
{
  struct gsi_hwio_def_gsi_spare_reg_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MCS_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mcs_sel_s
{
  u32 mcs_id : 3;
  u32 reserved0 : 29;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mcs_sel_u
{
  struct gsi_hwio_def_gsi_debug_mcs_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_SW_STALL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_sw_stall_s
{
  u32 mcs_stall : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_sw_stall_u
{
  struct gsi_hwio_def_gsi_debug_sw_stall_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_PC_FOR_DEBUG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_pc_for_debug_s
{
  u32 iram_ptr : 13;
  u32 reserved0 : 19;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_pc_for_debug_u
{
  struct gsi_hwio_def_gsi_debug_pc_for_debug_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_PTR_REG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_ptr_reg_s
{
  u32 ev_eng : 8;
  u32 wr_eng : 8;
  u32 int_eng_0 : 8;
  u32 int_eng_1 : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_ptr_reg_u
{
  struct gsi_hwio_def_gsi_debug_ptr_reg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_QSB_LOG_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_qsb_log_sel_s
{
  u32 sel_write : 1;
  u32 reserved0 : 7;
  u32 sel_tid : 8;
  u32 sel_mid : 8;
  u32 reserved1 : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_qsb_log_sel_u
{
  struct gsi_hwio_def_gsi_debug_qsb_log_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_MCS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_mcs_s
{
  u32 halt_other_mcs_clr : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_mcs_u
{
  struct gsi_hwio_def_gsi_debug_mcs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_QSB_LOG_CLR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_qsb_log_clr_s
{
  u32 log_clr : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_qsb_log_clr_u
{
  struct gsi_hwio_def_gsi_debug_qsb_log_clr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_QSB_LOG_ERR_TRNS_ID 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_qsb_log_err_trns_id_s
{
  u32 err_write : 1;
  u32 reserved0 : 7;
  u32 err_tid : 8;
  u32 err_mid : 8;
  u32 err_saved : 1;
  u32 reserved1 : 7;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_qsb_log_err_trns_id_u
{
  struct gsi_hwio_def_gsi_debug_qsb_log_err_trns_id_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_QSB_LOG_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_qsb_log_0_s
{
  u32 addr_31_0 : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_qsb_log_0_u
{
  struct gsi_hwio_def_gsi_debug_qsb_log_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_QSB_LOG_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_qsb_log_1_s
{
  u32 addr_43_32 : 12;
  u32 ainnershared : 1;
  u32 anoallocate : 1;
  u32 ashared : 1;
  u32 acacheable : 1;
  u32 atransient : 1;
  u32 aooord : 1;
  u32 aooowr : 1;
  u32 reserved0 : 1;
  u32 alen : 4;
  u32 asize : 4;
  u32 areqpriority : 4;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_qsb_log_1_u
{
  struct gsi_hwio_def_gsi_debug_qsb_log_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_QSB_LOG_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_qsb_log_2_s
{
  u32 ammusid : 12;
  u32 amemtype : 4;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_qsb_log_2_u
{
  struct gsi_hwio_def_gsi_debug_qsb_log_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_QSB_LOG_LAST_MISC_IDn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_qsb_log_last_misc_idn_s
{
  u32 addr_20_0 : 21;
  u32 write : 1;
  u32 tid : 5;
  u32 mid : 5;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_qsb_log_last_misc_idn_u
{
  struct gsi_hwio_def_gsi_debug_qsb_log_last_misc_idn_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_SW_RF_n_WRITE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_sw_rf_n_write_s
{
  u32 data_in : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_sw_rf_n_write_u
{
  struct gsi_hwio_def_gsi_debug_sw_rf_n_write_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_SW_RF_n_READ 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_sw_rf_n_read_s
{
  u32 rf_reg : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_sw_rf_n_read_u
{
  struct gsi_hwio_def_gsi_debug_sw_rf_n_read_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_COUNTER_CFGn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_counter_cfgn_s
{
  u32 enable : 1;
  u32 stop_at_wrap_arnd : 1;
  u32 clr_at_read : 1;
  u32 evnt_type : 5;
  u32 ee : 4;
  u32 virtual_chnl : 8;
  u32 pc_msb : 1;
  u32 mcs_sel : 8;
  u32 reserved0 : 2;
  u32 chain : 1;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_counter_cfgn_u
{
  struct gsi_hwio_def_gsi_debug_counter_cfgn_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_COUNTERn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_countern_s
{
  u32 counter_value : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_countern_u
{
  struct gsi_hwio_def_gsi_debug_countern_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_SW_MSK_REG_n_SEC_k_WR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_sw_msk_reg_n_sec_k_wr_s
{
  u32 data_in : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_sw_msk_reg_n_sec_k_wr_u
{
  struct gsi_hwio_def_gsi_debug_sw_msk_reg_n_sec_k_wr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_sw_msk_reg_n_sec_k_rd_s
{
  u32 msk_reg : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_sw_msk_reg_n_sec_k_rd_u
{
  struct gsi_hwio_def_gsi_debug_sw_msk_reg_n_sec_k_rd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_EE_n_CH_k_VP_TABLE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_ee_n_ch_k_vp_table_s
{
  u32 phy_ch : 8;
  u32 valid : 1;
  u32 reserved0 : 23;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_ee_n_ch_k_vp_table_u
{
  struct gsi_hwio_def_gsi_debug_ee_n_ch_k_vp_table_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_EE_n_EV_k_VP_TABLE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_ee_n_ev_k_vp_table_s
{
  u32 phy_ev_ch : 8;
  u32 valid : 1;
  u32 reserved0 : 23;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_ee_n_ev_k_vp_table_u
{
  struct gsi_hwio_def_gsi_debug_ee_n_ev_k_vp_table_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_SDMA_TRANS_DB_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_sdma_trans_db_n_s
{
  u32 wr : 1;
  u32 blocking : 1;
  u32 vld : 1;
  u32 int_id : 4;
  u32 tid_cnt : 4;
  u32 user_data_lsb : 21;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_sdma_trans_db_n_u
{
  struct gsi_hwio_def_gsi_debug_sdma_trans_db_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_REE_PREFETCH_BUF_CH_ID 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_ree_prefetch_buf_ch_id_s
{
  u32 prefetch_buf_ch_id : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_ree_prefetch_buf_ch_id_u
{
  struct gsi_hwio_def_gsi_debug_ree_prefetch_buf_ch_id_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_DEBUG_REE_PREFETCH_BUF_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_debug_ree_prefetch_buf_status_s
{
  u32 prefetch_buf_status : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_debug_ree_prefetch_buf_status_u
{
  struct gsi_hwio_def_gsi_debug_ree_prefetch_buf_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_BP_CNT_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_bp_cnt_lsb_s
{
  u32 bp_cnt_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_bp_cnt_lsb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_bp_cnt_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_BP_CNT_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_bp_cnt_msb_s
{
  u32 bp_cnt_msb : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_bp_cnt_msb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_bp_cnt_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_BP_AND_PENDING_CNT_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_bp_and_pending_cnt_lsb_s
{
  u32 bp_and_pending_cnt_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_bp_and_pending_cnt_lsb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_bp_and_pending_cnt_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_BP_AND_PENDING_CNT_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_bp_and_pending_cnt_msb_s
{
  u32 bp_and_pending_cnt_msb : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_bp_and_pending_cnt_msb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_bp_and_pending_cnt_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_MCS_BUSY_CNT_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_mcs_busy_cnt_lsb_s
{
  u32 mcs_busy_cnt_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_mcs_busy_cnt_lsb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_mcs_busy_cnt_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_MCS_BUSY_CNT_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_mcs_busy_cnt_msb_s
{
  u32 mcs_busy_cnt_msb : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_mcs_busy_cnt_msb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_mcs_busy_cnt_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_MCS_IDLE_CNT_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_mcs_idle_cnt_lsb_s
{
  u32 mcs_idle_cnt_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_mcs_idle_cnt_lsb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_mcs_idle_cnt_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_MCS_IDLE_CNT_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_mcs_idle_cnt_msb_s
{
  u32 mcs_idle_cnt_msb : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_mcs_idle_cnt_msb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_mcs_idle_cnt_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_mcs_idle_bp_and_pending_cnt_lsb_s
{
  u32 cnt_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_mcs_idle_bp_and_pending_cnt_lsb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_mcs_idle_bp_and_pending_cnt_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_profiling_mcs_idle_bp_and_pending_cnt_msb_s
{
  u32 cnt_msb : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_profiling_mcs_idle_bp_and_pending_cnt_msb_u
{
  struct gsi_hwio_def_gsi_mcs_profiling_mcs_idle_bp_and_pending_cnt_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MAX_TRE_TLV_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_max_tre_tlv_n_s
{
  u32 val : 32;
};

/* Union definition of register */
union gsi_hwio_def_gsi_max_tre_tlv_n_u
{
  struct gsi_hwio_def_gsi_max_tre_tlv_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_irq_2_mcs_mapping_accn_table0_s
{
  u32 valid_mcs_for_irq0 : 8;
  u32 valid_mcs_for_irq1 : 8;
  u32 valid_mcs_for_irq2 : 8;
  u32 valid_mcs_for_irq3 : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_irq_2_mcs_mapping_accn_table0_u
{
  struct gsi_hwio_def_gsi_irq_2_mcs_mapping_accn_table0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_irq_2_mcs_mapping_accn_table1_s
{
  u32 valid_mcs_for_irq4 : 8;
  u32 valid_mcs_for_irq5 : 8;
  u32 valid_mcs_for_irq6 : 8;
  u32 valid_mcs_for_irq7 : 8;
};

/* Union definition of register */
union gsi_hwio_def_gsi_irq_2_mcs_mapping_accn_table1_u
{
  struct gsi_hwio_def_gsi_irq_2_mcs_mapping_accn_table1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_CURRENT_CH_PER_MCS_0_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_current_ch_per_mcs_0_1_s
{
  u32 chid_mcs_0 : 8;
  u32 chid_valid_mcs_0 : 1;
  u32 mcs_0_busy : 1;
  u32 reserved0 : 6;
  u32 chid_mcs_1 : 8;
  u32 chid_valid_mcs_1 : 1;
  u32 mcs_1_busy : 1;
  u32 reserved1 : 6;
};

/* Union definition of register */
union gsi_hwio_def_gsi_current_ch_per_mcs_0_1_u
{
  struct gsi_hwio_def_gsi_current_ch_per_mcs_0_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_CURRENT_CH_PER_MCS_2_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_current_ch_per_mcs_2_3_s
{
  u32 chid_mcs_2 : 8;
  u32 chid_valid_mcs_2 : 1;
  u32 mcs_2_busy : 1;
  u32 reserved0 : 6;
  u32 chid_mcs_3 : 8;
  u32 chid_valid_mcs_3 : 1;
  u32 mcs_3_busy : 1;
  u32 reserved1 : 6;
};

/* Union definition of register */
union gsi_hwio_def_gsi_current_ch_per_mcs_2_3_u
{
  struct gsi_hwio_def_gsi_current_ch_per_mcs_2_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_CURRENT_CH_PER_MCS_4_5 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_current_ch_per_mcs_4_5_s
{
  u32 chid_mcs_4 : 8;
  u32 chid_valid_mcs_4 : 1;
  u32 mcs_4_busy : 1;
  u32 reserved0 : 6;
  u32 chid_mcs_5 : 8;
  u32 chid_valid_mcs_5 : 1;
  u32 mcs_5_busy : 1;
  u32 reserved1 : 6;
};

/* Union definition of register */
union gsi_hwio_def_gsi_current_ch_per_mcs_4_5_u
{
  struct gsi_hwio_def_gsi_current_ch_per_mcs_4_5_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_CURRENT_CH_PER_MCS_6_7 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_current_ch_per_mcs_6_7_s
{
  u32 chid_mcs_6 : 8;
  u32 chid_valid_mcs_6 : 1;
  u32 mcs_6_busy : 1;
  u32 reserved0 : 6;
  u32 chid_mcs_7 : 8;
  u32 chid_valid_mcs_7 : 1;
  u32 mcs_7_busy : 1;
  u32 reserved1 : 6;
};

/* Union definition of register */
union gsi_hwio_def_gsi_current_ch_per_mcs_6_7_u
{
  struct gsi_hwio_def_gsi_current_ch_per_mcs_6_7_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_IC_LOCK_BY_ORDER_EN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_ic_lock_by_order_en_s
{
  u32 ree_dis_ch_comp_int : 1;
  u32 ree_new_re_int : 1;
  u32 ree_ch_empty_int : 1;
  u32 ev_eng_int : 1;
  u32 ie_int_mod_stop_int : 1;
  u32 ie_notify_mcs_int : 1;
  u32 csr_ch_ev_com_int : 1;
  u32 csr_generic_com_int : 1;
  u32 tlv_desc_proc_comp_int : 1;
  u32 tlv_stop_int : 1;
  u32 tlv_rst_int : 1;
  u32 tlv_ch_not_full_int : 1;
  u32 timer_eng_int : 1;
  u32 db_new_db_int : 1;
  u32 db_new_msi_db_int : 1;
  u32 rd_int : 1;
  u32 wr_int : 1;
  u32 ie_int_mod_cancel_int : 1;
  u32 reserved0 : 14;
};

/* Union definition of register */
union gsi_hwio_def_gsi_ic_lock_by_order_en_u
{
  struct gsi_hwio_def_gsi_ic_lock_by_order_en_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_CNTXT_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_0_s
{
  u32 chtype_protocol : 7;
  u32 chtype_dir : 1;
  u32 ee : 4;
  u32 chid : 8;
  u32 chstate : 4;
  u32 element_size : 8;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_cntxt_0_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_CNTXT_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_1_s
{
  u32 r_length : 24;
  u32 erindex : 8;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_cntxt_1_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_CNTXT_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_2_s
{
  u32 r_base_addr_lsbs : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_cntxt_2_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_CNTXT_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_3_s
{
  u32 r_base_addr_msbs : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_cntxt_3_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_CNTXT_4 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_4_s
{
  u32 read_ptr_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_cntxt_4_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_4_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_CNTXT_5 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_5_s
{
  u32 read_ptr_msb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_cntxt_5_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_5_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_CNTXT_6 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_6_s
{
  u32 write_ptr_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_cntxt_6_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_6_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_CNTXT_7 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_7_s
{
  u32 write_ptr_msb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_cntxt_7_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_7_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_CNTXT_8 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_8_s
{
  u32 db_msi_data : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_cntxt_8_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_cntxt_8_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_ELEM_SIZE_SHIFT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_elem_size_shift_s
{
  u32 elem_size_shift : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_elem_size_shift_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_elem_size_shift_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_ch_almst_empty_thrshold_s
{
  u32 ch_almst_empty_thrshold : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_ch_almst_empty_thrshold_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_ch_almst_empty_thrshold_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_RE_FETCH_READ_PTR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_re_fetch_read_ptr_s
{
  u32 read_ptr : 24;
  u32 reserved0 : 8;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_re_fetch_read_ptr_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_re_fetch_read_ptr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_re_fetch_write_ptr_s
{
  u32 re_intr_db : 24;
  u32 reserved0 : 8;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_re_fetch_write_ptr_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_re_fetch_write_ptr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_QOS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_qos_s
{
  u32 wrr_weight : 4;
  u32 reserved0 : 4;
  u32 max_prefetch : 1;
  u32 use_db_eng : 1;
  u32 prefetch_mode : 4;
  u32 reserved1 : 2;
  u32 empty_lvl_thrshold : 8;
  u32 db_in_bytes : 1;
  u32 low_latency_en : 1;
  u32 reserved2 : 6;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_qos_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_qos_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_SCRATCH_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_0_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_scratch_0_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_SCRATCH_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_1_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_scratch_1_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_SCRATCH_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_2_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_scratch_2_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_SCRATCH_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_3_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_scratch_3_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_SCRATCH_4 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_4_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_scratch_4_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_4_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_SCRATCH_5 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_5_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_scratch_5_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_5_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_SCRATCH_6 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_6_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_scratch_6_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_6_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_SCRATCH_7 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_7_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_scratch_7_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_7_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_SCRATCH_8 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_8_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_scratch_8_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_8_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_SCRATCH_9 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_9_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_scratch_9_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_scratch_9_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_DB_ENG_WRITE_PTR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_db_eng_write_ptr_s
{
  u32 last_db_2_mcs : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_db_eng_write_ptr_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_db_eng_write_ptr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_0_s
{
  u32 chtype : 7;
  u32 intype : 1;
  u32 evchid : 8;
  u32 ee : 4;
  u32 chstate : 4;
  u32 element_size : 8;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_0_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_1_s
{
  u32 r_length : 24;
  u32 reserved0 : 8;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_1_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_2_s
{
  u32 r_base_addr_lsbs : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_2_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_3_s
{
  u32 r_base_addr_msbs : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_3_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_4 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_4_s
{
  u32 read_ptr_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_4_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_4_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_5 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_5_s
{
  u32 read_ptr_msb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_5_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_5_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_6 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_6_s
{
  u32 write_ptr_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_6_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_6_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_7 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_7_s
{
  u32 write_ptr_msb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_7_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_7_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_8 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_8_s
{
  u32 int_modt : 16;
  u32 int_modc : 8;
  u32 int_mod_cnt : 8;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_8_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_8_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_9 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_9_s
{
  u32 intvec : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_9_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_9_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_10 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_10_s
{
  u32 msi_addr_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_10_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_10_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_11 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_11_s
{
  u32 msi_addr_msb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_11_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_11_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_12 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_12_s
{
  u32 rp_update_addr_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_12_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_12_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_CNTXT_13 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_13_s
{
  u32 rp_update_addr_msb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_cntxt_13_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_cntxt_13_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_ELEM_SIZE_SHIFT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_elem_size_shift_s
{
  u32 elem_size_shift : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_elem_size_shift_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_elem_size_shift_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_SCRATCH_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_scratch_0_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_scratch_0_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_scratch_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_SCRATCH_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_scratch_1_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_scratch_1_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_scratch_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_SCRATCH_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_scratch_2_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_scratch_2_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_scratch_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_DOORBELL_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_doorbell_0_s
{
  u32 write_ptr_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_doorbell_0_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_doorbell_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_k_DOORBELL_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_k_doorbell_1_s
{
  u32 write_ptr_msb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_k_doorbell_1_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_k_doorbell_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_DOORBELL_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_doorbell_0_s
{
  u32 write_ptr_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_doorbell_0_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_doorbell_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_k_DOORBELL_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_k_doorbell_1_s
{
  u32 write_ptr_msb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_k_doorbell_1_u
{
  struct gsi_hwio_def_ee_n_ev_ch_k_doorbell_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_status_s
{
  u32 enabled : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_status_u
{
  struct gsi_hwio_def_ee_n_gsi_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_CH_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ch_cmd_s
{
  u32 chid : 8;
  u32 reserved0 : 16;
  u32 opcode : 8;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ch_cmd_u
{
  struct gsi_hwio_def_ee_n_gsi_ch_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_EV_CH_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_ev_ch_cmd_s
{
  u32 chid : 8;
  u32 reserved0 : 16;
  u32 opcode : 8;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_ev_ch_cmd_u
{
  struct gsi_hwio_def_ee_n_ev_ch_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_EE_GENERIC_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_ee_generic_cmd_s
{
  u32 opcode : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_ee_generic_cmd_u
{
  struct gsi_hwio_def_ee_n_gsi_ee_generic_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_HW_PARAM_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_hw_param_0_s
{
  u32 gsi_ev_ch_num : 8;
  u32 gsi_ch_num : 8;
  u32 num_ees : 5;
  u32 periph_conf_addr_bus_w : 5;
  u32 periph_sec_grp : 5;
  u32 use_axi_m : 1;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_hw_param_0_u
{
  struct gsi_hwio_def_ee_n_gsi_hw_param_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_HW_PARAM_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_hw_param_1_s
{
  u32 gsi_m_data_bus_w : 8;
  u32 gsi_num_qad : 4;
  u32 gsi_nonsec_en : 4;
  u32 gsi_sec_en : 1;
  u32 gsi_vmidacr_en : 1;
  u32 gsi_qrib_en : 1;
  u32 gsi_use_xpu : 1;
  u32 gsi_num_timers : 6;
  u32 gsi_use_db_eng : 1;
  u32 reserved0 : 1;
  u32 gsi_escape_buf_only : 1;
  u32 gsi_simple_rd_wr : 1;
  u32 gsi_blk_int_access_region_1_en : 1;
  u32 gsi_blk_int_access_region_2_en : 1;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_hw_param_1_u
{
  struct gsi_hwio_def_ee_n_gsi_hw_param_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_HW_PARAM_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_hw_param_2_s
{
  u32 gsi_num_ch_per_ee : 8;
  u32 gsi_iram_size : 5;
  u32 gsi_ch_pend_translate : 1;
  u32 gsi_ch_full_logic : 1;
  u32 gsi_use_sdma : 1;
  u32 gsi_sdma_n_int : 3;
  u32 gsi_sdma_max_burst : 8;
  u32 gsi_sdma_n_iovec : 3;
  u32 gsi_use_rd_wr_eng : 1;
  u32 gsi_use_inter_ee : 1;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_hw_param_2_u
{
  struct gsi_hwio_def_ee_n_gsi_hw_param_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_MCS_CODE_VER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_mcs_code_ver_s
{
  u32 ver : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_mcs_code_ver_u
{
  struct gsi_hwio_def_ee_n_gsi_mcs_code_ver_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_HW_PARAM_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_hw_param_3_s
{
  u32 gsi_sdma_max_os_rd : 4;
  u32 gsi_sdma_max_os_wr : 4;
  u32 gsi_num_prefetch_bufs : 4;
  u32 gsi_m_addr_bus_w : 8;
  u32 gsi_ree_max_burst_len : 5;
  u32 gsi_use_irom : 1;
  u32 gsi_use_vir_ch_if : 1;
  u32 gsi_use_sleep_clk_div : 1;
  u32 gsi_use_db_msi_mode : 1;
  u32 gsi_use_bp_mtrix : 1;
  u32 reserved0 : 2;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_hw_param_3_u
{
  struct gsi_hwio_def_ee_n_gsi_hw_param_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_GSI_HW_PARAM_4 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_gsi_hw_param_4_s
{
  u32 gsi_num_ev_per_ee : 8;
  u32 gsi_iram_protocol_cnt : 8;
  u32 gsi_shram_size_dword : 16;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_gsi_hw_param_4_u
{
  struct gsi_hwio_def_ee_n_gsi_hw_param_4_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_TYPE_IRQ 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_type_irq_s
{
  u32 ch_ctrl : 1;
  u32 ev_ctrl : 1;
  u32 glob_ee : 1;
  u32 ieob : 1;
  u32 inter_ee_ch_ctrl : 1;
  u32 inter_ee_ev_ctrl : 1;
  u32 general : 1;
  u32 reserved0 : 25;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_type_irq_u
{
  struct gsi_hwio_def_ee_n_cntxt_type_irq_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_TYPE_IRQ_MSK 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_type_irq_msk_s
{
  u32 ch_ctrl : 1;
  u32 ev_ctrl : 1;
  u32 glob_ee : 1;
  u32 ieob : 1;
  u32 inter_ee_ch_ctrl : 1;
  u32 inter_ee_ev_ctrl : 1;
  u32 general : 1;
  u32 reserved0 : 25;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_type_irq_msk_u
{
  struct gsi_hwio_def_ee_n_cntxt_type_irq_msk_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_SRC_GSI_CH_IRQ_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_src_gsi_ch_irq_k_s
{
  u32 gsi_ch_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_src_gsi_ch_irq_k_u
{
  struct gsi_hwio_def_ee_n_cntxt_src_gsi_ch_irq_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_src_gsi_ch_irq_msk_k_s
{
  u32 gsi_ch_bit_map_msk : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_src_gsi_ch_irq_msk_k_u
{
  struct gsi_hwio_def_ee_n_cntxt_src_gsi_ch_irq_msk_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_src_gsi_ch_irq_clr_k_s
{
  u32 gsi_ch_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_src_gsi_ch_irq_clr_k_u
{
  struct gsi_hwio_def_ee_n_cntxt_src_gsi_ch_irq_clr_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_SRC_EV_CH_IRQ_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_src_ev_ch_irq_k_s
{
  u32 ev_ch_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_src_ev_ch_irq_k_u
{
  struct gsi_hwio_def_ee_n_cntxt_src_ev_ch_irq_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_src_ev_ch_irq_msk_k_s
{
  u32 ev_ch_bit_map_msk : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_src_ev_ch_irq_msk_k_u
{
  struct gsi_hwio_def_ee_n_cntxt_src_ev_ch_irq_msk_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_src_ev_ch_irq_clr_k_s
{
  u32 ev_ch_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_src_ev_ch_irq_clr_k_u
{
  struct gsi_hwio_def_ee_n_cntxt_src_ev_ch_irq_clr_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_SRC_IEOB_IRQ_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_src_ieob_irq_k_s
{
  u32 ev_ch_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_src_ieob_irq_k_u
{
  struct gsi_hwio_def_ee_n_cntxt_src_ieob_irq_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_src_ieob_irq_msk_k_s
{
  u32 ev_ch_bit_map_msk : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_src_ieob_irq_msk_k_u
{
  struct gsi_hwio_def_ee_n_cntxt_src_ieob_irq_msk_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_SRC_IEOB_IRQ_CLR_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_src_ieob_irq_clr_k_s
{
  u32 ev_ch_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_src_ieob_irq_clr_k_u
{
  struct gsi_hwio_def_ee_n_cntxt_src_ieob_irq_clr_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_GLOB_IRQ_STTS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_glob_irq_stts_s
{
  u32 error_int : 1;
  u32 gp_int1 : 1;
  u32 gp_int2 : 1;
  u32 gp_int3 : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_glob_irq_stts_u
{
  struct gsi_hwio_def_ee_n_cntxt_glob_irq_stts_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_GLOB_IRQ_EN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_glob_irq_en_s
{
  u32 error_int : 1;
  u32 gp_int1 : 1;
  u32 gp_int2 : 1;
  u32 gp_int3 : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_glob_irq_en_u
{
  struct gsi_hwio_def_ee_n_cntxt_glob_irq_en_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_GLOB_IRQ_CLR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_glob_irq_clr_s
{
  u32 error_int : 1;
  u32 gp_int1 : 1;
  u32 gp_int2 : 1;
  u32 gp_int3 : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_glob_irq_clr_u
{
  struct gsi_hwio_def_ee_n_cntxt_glob_irq_clr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_GSI_IRQ_STTS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_gsi_irq_stts_s
{
  u32 gsi_break_point : 8;
  u32 gsi_bus_error : 1;
  u32 reserved0 : 7;
  u32 gsi_cmd_fifo_ovrflow : 1;
  u32 reserved1 : 7;
  u32 gsi_mcs_stack_ovrflow : 8;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_gsi_irq_stts_u
{
  struct gsi_hwio_def_ee_n_cntxt_gsi_irq_stts_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_GSI_IRQ_EN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_gsi_irq_en_s
{
  u32 gsi_break_point : 8;
  u32 gsi_bus_error : 1;
  u32 reserved0 : 7;
  u32 gsi_cmd_fifo_ovrflow : 1;
  u32 reserved1 : 7;
  u32 gsi_mcs_stack_ovrflow : 8;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_gsi_irq_en_u
{
  struct gsi_hwio_def_ee_n_cntxt_gsi_irq_en_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_GSI_IRQ_CLR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_gsi_irq_clr_s
{
  u32 gsi_break_point : 8;
  u32 gsi_bus_error : 1;
  u32 reserved0 : 7;
  u32 gsi_cmd_fifo_ovrflow : 1;
  u32 reserved1 : 7;
  u32 gsi_mcs_stack_ovrflow : 8;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_gsi_irq_clr_u
{
  struct gsi_hwio_def_ee_n_cntxt_gsi_irq_clr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_INTSET 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_intset_s
{
  u32 intype : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_intset_u
{
  struct gsi_hwio_def_ee_n_cntxt_intset_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_MSI_BASE_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_msi_base_lsb_s
{
  u32 msi_addr_lsb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_msi_base_lsb_u
{
  struct gsi_hwio_def_ee_n_cntxt_msi_base_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_MSI_BASE_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_msi_base_msb_s
{
  u32 msi_addr_msb : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_msi_base_msb_u
{
  struct gsi_hwio_def_ee_n_cntxt_msi_base_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_INT_VEC 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_int_vec_s
{
  u32 int_vec : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_int_vec_u
{
  struct gsi_hwio_def_ee_n_cntxt_int_vec_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_ERROR_LOG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_error_log_s
{
  u32 error_description : 16;
  u32 channel_event_virtual_ix : 8;
  u32 error_type : 4;
  u32 ee : 4;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_error_log_u
{
  struct gsi_hwio_def_ee_n_error_log_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_ERROR_LOG_CLR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_error_log_clr_s
{
  u32 error_log_clr : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_error_log_clr_u
{
  struct gsi_hwio_def_ee_n_error_log_clr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_SCRATCH_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_scratch_0_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_scratch_0_u
{
  struct gsi_hwio_def_ee_n_cntxt_scratch_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: EE_n_CNTXT_SCRATCH_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_ee_n_cntxt_scratch_1_s
{
  u32 scratch : 32;
};

/* Union definition of register */
union gsi_hwio_def_ee_n_cntxt_scratch_1_u
{
  struct gsi_hwio_def_ee_n_cntxt_scratch_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MCS_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_mcs_cfg_s
{
  u32 mcs_enable : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union gsi_hwio_def_gsi_mcs_cfg_u
{
  struct gsi_hwio_def_gsi_mcs_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_TZ_FW_AUTH_LOCK 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_tz_fw_auth_lock_s
{
  u32 dis_iram_write : 1;
  u32 dis_debug_shram_write : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union gsi_hwio_def_gsi_tz_fw_auth_lock_u
{
  struct gsi_hwio_def_gsi_tz_fw_auth_lock_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_MSA_FW_AUTH_LOCK 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_msa_fw_auth_lock_s
{
  u32 dis_iram_write : 1;
  u32 dis_debug_shram_write : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union gsi_hwio_def_gsi_msa_fw_auth_lock_u
{
  struct gsi_hwio_def_gsi_msa_fw_auth_lock_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: GSI_SP_FW_AUTH_LOCK 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_gsi_sp_fw_auth_lock_s
{
  u32 dis_iram_write : 1;
  u32 dis_debug_shram_write : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union gsi_hwio_def_gsi_sp_fw_auth_lock_u
{
  struct gsi_hwio_def_gsi_sp_fw_auth_lock_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: INTER_EE_n_ORIGINATOR_EE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_inter_ee_n_originator_ee_s
{
  u32 ee_number : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union gsi_hwio_def_inter_ee_n_originator_ee_u
{
  struct gsi_hwio_def_inter_ee_n_originator_ee_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: INTER_EE_n_GSI_CH_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_inter_ee_n_gsi_ch_cmd_s
{
  u32 chid : 8;
  u32 reserved0 : 16;
  u32 opcode : 8;
};

/* Union definition of register */
union gsi_hwio_def_inter_ee_n_gsi_ch_cmd_u
{
  struct gsi_hwio_def_inter_ee_n_gsi_ch_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: INTER_EE_n_EV_CH_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_inter_ee_n_ev_ch_cmd_s
{
  u32 chid : 8;
  u32 reserved0 : 16;
  u32 opcode : 8;
};

/* Union definition of register */
union gsi_hwio_def_inter_ee_n_ev_ch_cmd_u
{
  struct gsi_hwio_def_inter_ee_n_ev_ch_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: INTER_EE_n_SRC_GSI_CH_IRQ_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_inter_ee_n_src_gsi_ch_irq_k_s
{
  u32 gsi_ch_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_inter_ee_n_src_gsi_ch_irq_k_u
{
  struct gsi_hwio_def_inter_ee_n_src_gsi_ch_irq_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_inter_ee_n_src_gsi_ch_irq_msk_k_s
{
  u32 gsi_ch_bit_map_msk : 32;
};

/* Union definition of register */
union gsi_hwio_def_inter_ee_n_src_gsi_ch_irq_msk_k_u
{
  struct gsi_hwio_def_inter_ee_n_src_gsi_ch_irq_msk_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: INTER_EE_n_SRC_GSI_CH_IRQ_CLR_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_inter_ee_n_src_gsi_ch_irq_clr_k_s
{
  u32 gsi_ch_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_inter_ee_n_src_gsi_ch_irq_clr_k_u
{
  struct gsi_hwio_def_inter_ee_n_src_gsi_ch_irq_clr_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: INTER_EE_n_SRC_EV_CH_IRQ_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_inter_ee_n_src_ev_ch_irq_k_s
{
  u32 ev_ch_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_inter_ee_n_src_ev_ch_irq_k_u
{
  struct gsi_hwio_def_inter_ee_n_src_ev_ch_irq_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: INTER_EE_n_SRC_EV_CH_IRQ_MSK_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_inter_ee_n_src_ev_ch_irq_msk_k_s
{
  u32 ev_ch_bit_map_msk : 32;
};

/* Union definition of register */
union gsi_hwio_def_inter_ee_n_src_ev_ch_irq_msk_k_u
{
  struct gsi_hwio_def_inter_ee_n_src_ev_ch_irq_msk_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: INTER_EE_n_SRC_EV_CH_IRQ_CLR_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct gsi_hwio_def_inter_ee_n_src_ev_ch_irq_clr_k_s
{
  u32 ev_ch_bit_map : 32;
};

/* Union definition of register */
union gsi_hwio_def_inter_ee_n_src_ev_ch_irq_clr_k_u
{
  struct gsi_hwio_def_inter_ee_n_src_ev_ch_irq_clr_k_s def;
  u32 value;
};


#endif /* __GSI_HWIO_DEF_H__ */
