// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.​
 */

#ifndef __IPA_HWIO_DEF_H__
#define __IPA_HWIO_DEF_H__
/*
===========================================================================
*/
/**
  @file ipa_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    IPA.*

  'Include' filters applied: <none>
  'Exclude' filters applied: RESERVED DUMMY 
*/

/*----------------------------------------------------------------------------
 * MODULE: IPA_UC_IPA_UC
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * MODULE: IPA_UC_IPA_UC_PER
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CONTROL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_control_s
{
  u32 reserved0 : 1;
  u32 uc_dsmode : 1;
  u32 qmb_snoc_bypass_dis : 1;
  u32 uc_clock_gating_dis : 1;
  u32 mbox_dis : 8;
  u32 reserved1 : 12;
  u32 warmboot_dis : 1;
  u32 reserved2 : 2;
  u32 uc_ram_rd_cli_cache_dis : 1;
  u32 reserved3 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_control_u
{
  struct ipa_hwio_def_ipa_uc_control_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_SYS_BUS_ATTRIB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_sys_bus_attrib_s
{
  u32 memtype : 3;
  u32 reserved0 : 1;
  u32 noallocate : 1;
  u32 reserved1 : 3;
  u32 innershared : 1;
  u32 reserved2 : 3;
  u32 shared : 1;
  u32 reserved3 : 19;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_sys_bus_attrib_u
{
  struct ipa_hwio_def_ipa_uc_sys_bus_attrib_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_SYS_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_sys_addr_s
{
  u32 addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_sys_addr_u
{
  struct ipa_hwio_def_ipa_uc_qmb_sys_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_SYS_ADDR_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_sys_addr_msb_s
{
  u32 addr_msb : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_sys_addr_msb_u
{
  struct ipa_hwio_def_ipa_uc_qmb_sys_addr_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_LOCAL_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_local_addr_s
{
  u32 addr : 20;
  u32 reserved0 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_local_addr_u
{
  struct ipa_hwio_def_ipa_uc_qmb_local_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_LENGTH 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_length_s
{
  u32 length : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_length_u
{
  struct ipa_hwio_def_ipa_uc_qmb_length_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_TRIGGER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_trigger_s
{
  u32 rsv : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_trigger_u
{
  struct ipa_hwio_def_ipa_uc_qmb_trigger_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_COMMAND_ATTR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_command_attr_s
{
  u32 direction : 1;
  u32 inorder : 1;
  u32 wait_for_response_mode : 1;
  u32 sync : 1;
  u32 interrupt_on_completion : 1;
  u32 queue_number : 1;
  u32 reserved0 : 10;
  u32 user : 11;
  u32 reserved1 : 5;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_command_attr_u
{
  struct ipa_hwio_def_ipa_uc_qmb_command_attr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_COMMAND_UCTAG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_command_uctag_s
{
  u32 uctag : 20;
  u32 reserved0 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_command_uctag_u
{
  struct ipa_hwio_def_ipa_uc_qmb_command_uctag_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_COMPLETED_FIFO_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_completed_fifo_n_s
{
  u32 uctag : 20;
  u32 reserved0 : 3;
  u32 fifo_cnt : 4;
  u32 error : 1;
  u32 rd_exok : 1;
  u32 wr_exfail : 1;
  u32 empty : 1;
  u32 full : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_completed_fifo_n_u
{
  struct ipa_hwio_def_ipa_uc_qmb_completed_fifo_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_COMPLETED_FIFO_PEEK_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_completed_fifo_peek_n_s
{
  u32 uctag : 20;
  u32 reserved0 : 3;
  u32 fifo_cnt : 4;
  u32 error : 1;
  u32 rd_exok : 1;
  u32 wr_exfail : 1;
  u32 empty : 1;
  u32 full : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_completed_fifo_peek_n_u
{
  struct ipa_hwio_def_ipa_uc_qmb_completed_fifo_peek_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_COMPLETED_FIFO_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_completed_fifo_status_s
{
  u32 fifo_size : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_completed_fifo_status_u
{
  struct ipa_hwio_def_ipa_uc_qmb_completed_fifo_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_CMD_FIFO_STATUS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_cmd_fifo_status_n_s
{
  u32 fifo_size : 4;
  u32 fifo_cnt : 4;
  u32 reserved0 : 8;
  u32 empty : 1;
  u32 full : 1;
  u32 reserved1 : 14;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_cmd_fifo_status_n_u
{
  struct ipa_hwio_def_ipa_uc_qmb_cmd_fifo_status_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_SYNC_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_sync_status_s
{
  u32 error_queue_0 : 1;
  u32 wr_exfail_queue_0 : 1;
  u32 rd_exok_queue_0 : 1;
  u32 reserved0 : 13;
  u32 error_queue_1 : 1;
  u32 rd_exok_queue_1 : 1;
  u32 wr_exfail_queue_1 : 1;
  u32 reserved1 : 13;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_sync_status_u
{
  struct ipa_hwio_def_ipa_uc_qmb_sync_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_BUS_ATTRIB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_bus_attrib_s
{
  u32 memtype : 3;
  u32 reserved0 : 1;
  u32 noallocate : 1;
  u32 reserved1 : 3;
  u32 innershared : 1;
  u32 reserved2 : 3;
  u32 shared : 1;
  u32 reserved3 : 3;
  u32 exclusive : 1;
  u32 reserved4 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_bus_attrib_u
{
  struct ipa_hwio_def_ipa_uc_qmb_bus_attrib_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_OUTSTANDING_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_outstanding_cfg_s
{
  u32 max_ot_overall : 8;
  u32 max_ot_rd : 8;
  u32 max_ot_wr : 8;
  u32 reserved0 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_outstanding_cfg_u
{
  struct ipa_hwio_def_ipa_uc_qmb_outstanding_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_OUTSTANDING_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_outstanding_status_s
{
  u32 current_ot_overall : 8;
  u32 current_ot_rd : 8;
  u32 current_ot_wr : 8;
  u32 reserved0 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_outstanding_status_u
{
  struct ipa_hwio_def_ipa_uc_qmb_outstanding_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_COMP_FIFO_INT_EN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_comp_fifo_int_en_s
{
  u32 comp_fifo_0_not_empty : 1;
  u32 comp_fifo_0_full : 1;
  u32 comp_fifo_0_ioc_cmd : 1;
  u32 reserved0 : 13;
  u32 comp_fifo_1_not_empty : 1;
  u32 comp_fifo_1_full : 1;
  u32 comp_fifo_1_ioc_cmd : 1;
  u32 reserved1 : 13;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_comp_fifo_int_en_u
{
  struct ipa_hwio_def_ipa_uc_qmb_comp_fifo_int_en_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_COMP_FIFO_INT_CLR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_comp_fifo_int_clr_s
{
  u32 comp_fifo_0_not_empty : 1;
  u32 comp_fifo_0_full : 1;
  u32 comp_fifo_0_ioc_cmd : 1;
  u32 reserved0 : 13;
  u32 comp_fifo_1_not_empty : 1;
  u32 comp_fifo_1_full : 1;
  u32 comp_fifo_1_ioc_cmd : 1;
  u32 reserved1 : 13;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_comp_fifo_int_clr_u
{
  struct ipa_hwio_def_ipa_uc_qmb_comp_fifo_int_clr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_COMP_FIFO_INT_SW_SET 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_comp_fifo_int_sw_set_s
{
  u32 comp_fifo_0_not_empty : 1;
  u32 comp_fifo_0_full : 1;
  u32 comp_fifo_0_ioc_cmd : 1;
  u32 reserved0 : 13;
  u32 comp_fifo_1_not_empty : 1;
  u32 comp_fifo_1_full : 1;
  u32 comp_fifo_1_ioc_cmd : 1;
  u32 reserved1 : 13;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_comp_fifo_int_sw_set_u
{
  struct ipa_hwio_def_ipa_uc_qmb_comp_fifo_int_sw_set_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_COMP_FIFO_INT_STTS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_comp_fifo_int_stts_s
{
  u32 comp_fifo_0_not_empty : 1;
  u32 comp_fifo_0_full : 1;
  u32 comp_fifo_0_ioc_cmd : 1;
  u32 reserved0 : 13;
  u32 comp_fifo_1_not_empty : 1;
  u32 comp_fifo_1_full : 1;
  u32 comp_fifo_1_ioc_cmd : 1;
  u32 reserved1 : 13;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_comp_fifo_int_stts_u
{
  struct ipa_hwio_def_ipa_uc_qmb_comp_fifo_int_stts_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_SYNC_COMPLETE_INT_EN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_sync_complete_int_en_s
{
  u32 sync_completed_0 : 1;
  u32 sync_completed_1 : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_sync_complete_int_en_u
{
  struct ipa_hwio_def_ipa_uc_qmb_sync_complete_int_en_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_SYNC_COMPLETE_INT_CLR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_sync_complete_int_clr_s
{
  u32 sync_completed_0 : 1;
  u32 sync_completed_1 : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_sync_complete_int_clr_u
{
  struct ipa_hwio_def_ipa_uc_qmb_sync_complete_int_clr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_SYNC_COMPLETE_INT_SW_SET 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_sync_complete_int_sw_set_s
{
  u32 sync_completed_0 : 1;
  u32 sync_completed_1 : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_sync_complete_int_sw_set_u
{
  struct ipa_hwio_def_ipa_uc_qmb_sync_complete_int_sw_set_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_QMB_SYNC_COMPLETE_INT_STTS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_qmb_sync_complete_int_stts_s
{
  u32 sync_completed_0 : 1;
  u32 sync_completed_1 : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_qmb_sync_complete_int_stts_u
{
  struct ipa_hwio_def_ipa_uc_qmb_sync_complete_int_stts_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_UC_AHB_SEC_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_uc_ahb_sec_cfg_s
{
  u32 user : 11;
  u32 reserved0 : 21;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_uc_ahb_sec_cfg_u
{
  struct ipa_hwio_def_ipa_uc_uc_ahb_sec_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_UC_AXI_SEC_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_uc_axi_sec_cfg_s
{
  u32 user : 11;
  u32 reserved0 : 21;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_uc_axi_sec_cfg_u
{
  struct ipa_hwio_def_ipa_uc_uc_axi_sec_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_MBOX_INT_STTS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_mbox_int_stts_n_s
{
  u32 irq_status : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_mbox_int_stts_n_u
{
  struct ipa_hwio_def_ipa_uc_mbox_int_stts_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_MBOX_INT_EN_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_mbox_int_en_n_s
{
  u32 irq_en : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_mbox_int_en_n_u
{
  struct ipa_hwio_def_ipa_uc_mbox_int_en_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_MBOX_INT_CLR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_mbox_int_clr_n_s
{
  u32 irq_clr : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_mbox_int_clr_n_u
{
  struct ipa_hwio_def_ipa_uc_mbox_int_clr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_MBOX_INT_SW_SET_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_mbox_int_sw_set_n_s
{
  u32 irq_sw_set : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_mbox_int_sw_set_n_u
{
  struct ipa_hwio_def_ipa_uc_mbox_int_sw_set_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_INT_STTS_GEN_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_int_stts_gen_n_s
{
  u32 irq_status : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_int_stts_gen_n_u
{
  struct ipa_hwio_def_ipa_uc_ipa_int_stts_gen_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_INT_EN_GEN_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_int_en_gen_n_s
{
  u32 irq_en : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_int_en_gen_n_u
{
  struct ipa_hwio_def_ipa_uc_ipa_int_en_gen_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_INT_CLR_GEN_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_int_clr_gen_n_s
{
  u32 irq_clr : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_int_clr_gen_n_u
{
  struct ipa_hwio_def_ipa_uc_ipa_int_clr_gen_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_INT_SW_SET_GEN_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_int_sw_set_gen_n_s
{
  u32 irq_sw_set : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_int_sw_set_gen_n_u
{
  struct ipa_hwio_def_ipa_uc_ipa_int_sw_set_gen_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_HWEV_INT_STTS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_hwev_int_stts_s
{
  u32 irq_status : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_hwev_int_stts_u
{
  struct ipa_hwio_def_ipa_uc_hwev_int_stts_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_HWEV_INT_EN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_hwev_int_en_s
{
  u32 irq_en : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_hwev_int_en_u
{
  struct ipa_hwio_def_ipa_uc_hwev_int_en_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_HWEV_INT_CLR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_hwev_int_clr_s
{
  u32 irq_clr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_hwev_int_clr_u
{
  struct ipa_hwio_def_ipa_uc_hwev_int_clr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_HWEV_INT_SW_SET 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_hwev_int_sw_set_s
{
  u32 irq_sw_set : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_hwev_int_sw_set_u
{
  struct ipa_hwio_def_ipa_uc_hwev_int_sw_set_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_LP_STARVATION 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_lp_starvation_s
{
  u32 iu_bitmap : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_lp_starvation_u
{
  struct ipa_hwio_def_ipa_uc_lp_starvation_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CRYPTO_ANTI_REPLAY_INT_STTS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_crypto_anti_replay_int_stts_s
{
  u32 irq_status : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_crypto_anti_replay_int_stts_u
{
  struct ipa_hwio_def_ipa_uc_crypto_anti_replay_int_stts_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CRYPTO_ANTI_REPLAY_INT_EN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_crypto_anti_replay_int_en_s
{
  u32 irq_en : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_crypto_anti_replay_int_en_u
{
  struct ipa_hwio_def_ipa_uc_crypto_anti_replay_int_en_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CRYPTO_ANTI_REPLAY_INT_CLR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_crypto_anti_replay_int_clr_s
{
  u32 irq_clr : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_crypto_anti_replay_int_clr_u
{
  struct ipa_hwio_def_ipa_uc_crypto_anti_replay_int_clr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CRYPTO_ANTI_REPLAY_INT_SW_SET 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_crypto_anti_replay_int_sw_set_s
{
  u32 irq_sw_set : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_crypto_anti_replay_int_sw_set_u
{
  struct ipa_hwio_def_ipa_uc_crypto_anti_replay_int_sw_set_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CRYPTO_WORKAROUND_INT_STTS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_crypto_workaround_int_stts_s
{
  u32 irq_status : 2;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_crypto_workaround_int_stts_u
{
  struct ipa_hwio_def_ipa_uc_crypto_workaround_int_stts_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CRYPTO_WORKAROUND_INT_EN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_crypto_workaround_int_en_s
{
  u32 irq_en : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_crypto_workaround_int_en_u
{
  struct ipa_hwio_def_ipa_uc_crypto_workaround_int_en_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CRYPTO_WORKAROUND_INT_CLR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_crypto_workaround_int_clr_s
{
  u32 irq_clr : 2;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_crypto_workaround_int_clr_u
{
  struct ipa_hwio_def_ipa_uc_crypto_workaround_int_clr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CRYPTO_WORKAROUND_SW_SET 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_crypto_workaround_sw_set_s
{
  u32 irq_sw_set : 2;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_crypto_workaround_sw_set_u
{
  struct ipa_hwio_def_ipa_uc_crypto_workaround_sw_set_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IRQ_INPUT_IC_MASKING 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_irq_input_ic_masking_s
{
  u32 hp_irq : 4;
  u32 ipa_irq : 7;
  u32 lp_irq : 20;
  u32 reserved0 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_irq_input_ic_masking_u
{
  struct ipa_hwio_def_ipa_uc_irq_input_ic_masking_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_NMI_INPUT_IC_MASK 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_nmi_input_ic_mask_s
{
  u32 nmi_mask : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_nmi_input_ic_mask_u
{
  struct ipa_hwio_def_ipa_uc_nmi_input_ic_mask_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_LP_IPA_INT_GENERIC_IU_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_lp_ipa_int_generic_iu_sel_s
{
  u32 iu_sel : 3;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_lp_ipa_int_generic_iu_sel_u
{
  struct ipa_hwio_def_ipa_uc_lp_ipa_int_generic_iu_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_LP_QMB_INT_IU_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_lp_qmb_int_iu_sel_s
{
  u32 qmb_sync_cmd_comp : 3;
  u32 qmb_completion : 3;
  u32 reserved0 : 26;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_lp_qmb_int_iu_sel_u
{
  struct ipa_hwio_def_ipa_uc_lp_qmb_int_iu_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_LP_TIMER_INT_IU_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_lp_timer_int_iu_sel_s
{
  u32 iu_sel : 3;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_lp_timer_int_iu_sel_u
{
  struct ipa_hwio_def_ipa_uc_lp_timer_int_iu_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_LP_MBOX_INT_IU_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_lp_mbox_int_iu_sel_s
{
  u32 iu_sel : 3;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_lp_mbox_int_iu_sel_u
{
  struct ipa_hwio_def_ipa_uc_lp_mbox_int_iu_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_LP_BAM_INT_IU_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_lp_bam_int_iu_sel_s
{
  u32 iu_sel : 3;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_lp_bam_int_iu_sel_u
{
  struct ipa_hwio_def_ipa_uc_lp_bam_int_iu_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_LP_HWEV_INT_IU_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_lp_hwev_int_iu_sel_s
{
  u32 iu_sel : 3;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_lp_hwev_int_iu_sel_u
{
  struct ipa_hwio_def_ipa_uc_lp_hwev_int_iu_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_HP_INT_IU_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_hp_int_iu_sel_s
{
  u32 timer : 3;
  u32 backpresure : 3;
  u32 reserved0 : 26;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_hp_int_iu_sel_u
{
  struct ipa_hwio_def_ipa_uc_hp_int_iu_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_NMI_SW_INT_IU_MASK 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_nmi_sw_int_iu_mask_s
{
  u32 iu_mask : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_nmi_sw_int_iu_mask_u
{
  struct ipa_hwio_def_ipa_uc_nmi_sw_int_iu_mask_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_IRQ_UCP_BLOCK_IU_MASK 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_irq_ucp_block_iu_mask_s
{
  u32 iu_mask : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_irq_ucp_block_iu_mask_u
{
  struct ipa_hwio_def_ipa_uc_ipa_irq_ucp_block_iu_mask_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_IRQ_INTRA_CRYPTO_BLOCK_IU_MASK 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_irq_intra_crypto_block_iu_mask_s
{
  u32 iu_mask : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_irq_intra_crypto_block_iu_mask_u
{
  struct ipa_hwio_def_ipa_uc_ipa_irq_intra_crypto_block_iu_mask_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_IRQ_POST_CRYPTO_BLOCK_IU_MASK 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_irq_post_crypto_block_iu_mask_s
{
  u32 iu_mask : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_irq_post_crypto_block_iu_mask_u
{
  struct ipa_hwio_def_ipa_uc_ipa_irq_post_crypto_block_iu_mask_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_TIMER_CTRL_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_timer_ctrl_n_s
{
  u32 count : 16;
  u32 event_sel : 7;
  u32 reserved0 : 1;
  u32 retrig : 1;
  u32 reserved1 : 5;
  u32 gran_sel : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_timer_ctrl_n_u
{
  struct ipa_hwio_def_ipa_uc_timer_ctrl_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_TIMER_STATUS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_timer_status_n_s
{
  u32 count : 16;
  u32 reserved0 : 8;
  u32 active : 1;
  u32 reserved1 : 7;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_timer_status_n_u
{
  struct ipa_hwio_def_ipa_uc_timer_status_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_EVENTS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_events_s
{
  u32 events : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_events_u
{
  struct ipa_hwio_def_ipa_uc_events_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_SYS_ADDR_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_sys_addr_msb_s
{
  u32 sys_addr_msb : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_sys_addr_msb_u
{
  struct ipa_hwio_def_ipa_uc_sys_addr_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_PC_RESTORE_WR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_pc_restore_wr_s
{
  u32 set_ipa_pc_ack : 1;
  u32 clear_ipa_pc_ack : 1;
  u32 set_ipa_restore_ack : 1;
  u32 clear_ipa_restore_ack : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_pc_restore_wr_u
{
  struct ipa_hwio_def_ipa_uc_pc_restore_wr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_PC_RESTORE_RD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_pc_restore_rd_s
{
  u32 ipa_pc_req : 1;
  u32 ipa_pc_ack : 1;
  u32 ipa_restore_req : 1;
  u32 ipa_restore_ack : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_pc_restore_rd_u
{
  struct ipa_hwio_def_ipa_uc_pc_restore_rd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_UCP_RESUME_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_ucp_resume_iu_n_s
{
  u32 reserved0 : 1;
  u32 ipa_ucp_resume_next_round_en : 1;
  u32 ipa_ucp_resume_dest_pipe_override : 1;
  u32 reserved1 : 1;
  u32 ipa_ucp_resume_ip_checksum_fix_en : 1;
  u32 ipa_ucp_resume_tport_checksum_fix_en : 1;
  u32 ipa_ucp_resume_new_snapshot_en : 1;
  u32 reserved2 : 1;
  u32 ipa_ucp_resume_dest_pipe_value : 8;
  u32 ipa_ucp_resume_exception : 1;
  u32 reserved3 : 2;
  u32 ipa_ucp_resume_next_pkt_parser_dis : 1;
  u32 ipa_ucp_resume_metadata_override : 1;
  u32 reserved4 : 3;
  u32 ipa_ucp_resume_ttl_updated : 1;
  u32 ipa_ucp_resume_ttl_exception : 1;
  u32 reserved5 : 1;
  u32 ipa_ucp_resume_reftech_context : 1;
  u32 ipa_ucp_resume_post_uc_hw_processing_dis : 1;
  u32 ipa_ucp_resume_exceed_mtu_exception : 1;
  u32 ipa_ucp_resume_outgoing_l4_csum_override : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_ucp_resume_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_ipa_ucp_resume_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_UCP_RESUME_METADATA_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_ucp_resume_metadata_iu_n_s
{
  u32 metadata : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_ucp_resume_metadata_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_ipa_ucp_resume_metadata_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_UCP_RESUME_METADATA_MASK_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_ucp_resume_metadata_mask_iu_n_s
{
  u32 metadata_mask : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_ucp_resume_metadata_mask_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_ipa_ucp_resume_metadata_mask_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_UC_SECTION_IDX_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_uc_section_idx_iu_n_s
{
  u32 section_index : 3;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_uc_section_idx_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_ipa_uc_section_idx_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_MUTEX_LOCK_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_mutex_lock_iu_n_s
{
  u32 lock : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_mutex_lock_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_mutex_lock_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_MUTEX_UNLOCK_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_mutex_unlock_iu_n_s
{
  u32 unlock : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_mutex_unlock_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_mutex_unlock_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_MUTEX_UNLOCK_BYORDER_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_mutex_unlock_byorder_iu_n_s
{
  u32 byorder : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_mutex_unlock_byorder_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_mutex_unlock_byorder_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_MUTEX_LOCK_BYORDER_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_mutex_lock_byorder_iu_n_s
{
  u32 byorder : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_mutex_lock_byorder_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_mutex_lock_byorder_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IU_INDEX_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_iu_index_0_s
{
  u32 idx : 2;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_iu_index_0_u
{
  struct ipa_hwio_def_ipa_uc_iu_index_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IU_INDEX_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_iu_index_1_s
{
  u32 idx : 2;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_iu_index_1_u
{
  struct ipa_hwio_def_ipa_uc_iu_index_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IU_INDEX_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_iu_index_2_s
{
  u32 idx : 2;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_iu_index_2_u
{
  struct ipa_hwio_def_ipa_uc_iu_index_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IU_INDEX_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_iu_index_3_s
{
  u32 idx : 2;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_iu_index_3_u
{
  struct ipa_hwio_def_ipa_uc_iu_index_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_IRQ_COMPLETE_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_irq_complete_iu_n_s
{
  u32 complete : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_irq_complete_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_ipa_irq_complete_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_BLOCK_INTERRUPTS_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_block_interrupts_iu_n_s
{
  u32 disable : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_block_interrupts_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_block_interrupts_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_STATUS_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_status_iu_n_s
{
  u32 sleepdeep : 1;
  u32 sleep : 1;
  u32 lockup : 1;
  u32 uc_enable : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_status_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_status_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_PEND_IRQ_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_pend_irq_iu_n_s
{
  u32 pend_irq : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_pend_irq_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_pend_irq_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_TRACE_BUFFER_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_trace_buffer_iu_n_s
{
  u32 trace_buffer : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_trace_buffer_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_trace_buffer_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_PC_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_pc_iu_n_s
{
  u32 pc : 17;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_pc_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_pc_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_LOW_PRIORITY_IRQ_REQ_TIMER_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_low_priority_irq_req_timer_n_s
{
  u32 timer : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_low_priority_irq_req_timer_n_u
{
  struct ipa_hwio_def_ipa_uc_low_priority_irq_req_timer_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_LOW_PRIORITY_IRQ_REQ_TIMER_CLR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_low_priority_irq_req_timer_clr_n_s
{
  u32 clr : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_low_priority_irq_req_timer_clr_n_u
{
  struct ipa_hwio_def_ipa_uc_low_priority_irq_req_timer_clr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CNT_GLOBAL_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_cnt_global_iu_n_s
{
  u32 count_en : 1;
  u32 count_cgc_open : 1;
  u32 reserved0 : 29;
  u32 clear_all : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_cnt_global_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_cnt_global_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CNT_CTL_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_cnt_ctl_iu_n_s
{
  u32 cycle_cnt_en : 1;
  u32 reserved0 : 1;
  u32 cycle_cnt_clr : 1;
  u32 reserved1 : 1;
  u32 idle_cnt_en : 1;
  u32 reserved2 : 1;
  u32 idle_cnt_clr : 1;
  u32 reserved3 : 1;
  u32 inst_cnt_en : 1;
  u32 inst_clr_after_rd : 1;
  u32 inst_cnt_clr : 1;
  u32 reserved4 : 1;
  u32 vuic_rd_cnt_en : 1;
  u32 vuic_wr_cnt_en : 1;
  u32 vuic_clr_after_rd : 1;
  u32 vuic_cnt_clr : 1;
  u32 dram_rd_cnt_en : 1;
  u32 dram_wr_cnt_en : 1;
  u32 dram_clr_after_rd : 1;
  u32 dram_cnt_clr : 1;
  u32 reserved5 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_cnt_ctl_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_cnt_ctl_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CNT_CLK_CYCLE_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_cnt_clk_cycle_iu_n_s
{
  u32 counter : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_cnt_clk_cycle_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_cnt_clk_cycle_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CNT_CLK_CYCLE_MSB_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_cnt_clk_cycle_msb_iu_n_s
{
  u32 counter : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_cnt_clk_cycle_msb_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_cnt_clk_cycle_msb_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CNT_IDLE_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_cnt_idle_iu_n_s
{
  u32 counter : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_cnt_idle_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_cnt_idle_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CNT_IDLE_MSB_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_cnt_idle_msb_iu_n_s
{
  u32 counter : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_cnt_idle_msb_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_cnt_idle_msb_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CNT_INST_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_cnt_inst_iu_n_s
{
  u32 counter : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_cnt_inst_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_cnt_inst_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CNT_DRAM_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_cnt_dram_iu_n_s
{
  u32 counter : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_cnt_dram_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_cnt_dram_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_CNT_VUIC_IU_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_cnt_vuic_iu_n_s
{
  u32 counter : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_cnt_vuic_iu_n_u
{
  struct ipa_hwio_def_ipa_uc_cnt_vuic_iu_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_VUIC_INT_STTS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_vuic_int_stts_n_s
{
  u32 irq_status : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_vuic_int_stts_n_u
{
  struct ipa_hwio_def_ipa_uc_vuic_int_stts_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_VUIC_INT_CLR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_vuic_int_clr_n_s
{
  u32 irq_clr : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_vuic_int_clr_n_u
{
  struct ipa_hwio_def_ipa_uc_vuic_int_clr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_VUIC_INT_SW_SET_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_vuic_int_sw_set_n_s
{
  u32 irq_sw_set : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_vuic_int_sw_set_n_u
{
  struct ipa_hwio_def_ipa_uc_vuic_int_sw_set_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_VUIC_INT_ADDRESS_LSB_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_vuic_int_address_lsb_n_s
{
  u32 addrress : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_vuic_int_address_lsb_n_u
{
  struct ipa_hwio_def_ipa_uc_vuic_int_address_lsb_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_VUIC_INT_ADDRESS_MSB_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_vuic_int_address_msb_n_s
{
  u32 addrress : 10;
  u32 reserved0 : 22;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_vuic_int_address_msb_n_u
{
  struct ipa_hwio_def_ipa_uc_vuic_int_address_msb_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_UC_PKT_PROCESS_BASE_ADDR_0_SECTION_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_base_addr_0_section_n_s
{
  u32 ipa_uc_pkt_process_context_base : 17;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_base_addr_0_section_n_u
{
  struct ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_base_addr_0_section_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_UC_PKT_PROCESS_BASE_ADDR_1_SECTION_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_base_addr_1_section_n_s
{
  u32 ipa_uc_pkt_process_pkt_base : 17;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_base_addr_1_section_n_u
{
  struct ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_base_addr_1_section_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_UC_PKT_PROCESS_BASE_ADDR_2_SECTION_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_base_addr_2_section_n_s
{
  u32 ipa_uc_pkt_process_hdr_base : 17;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_base_addr_2_section_n_u
{
  struct ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_base_addr_2_section_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_UC_PKT_PROCESS_BASE_ADDR_3_SECTION_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_base_addr_3_section_n_s
{
  u32 ipa_uc_pkt_process_sa_base : 17;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_base_addr_3_section_n_u
{
  struct ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_base_addr_3_section_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_UC_PKT_PROCESS_NUMBER_ACTIVE_SECTIONS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_number_active_sections_s
{
  u32 num : 3;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_number_active_sections_u
{
  struct ipa_hwio_def_ipa_uc_ipa_uc_pkt_process_number_active_sections_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_LOCAL_DEDICATED_DATA_BASELINE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_local_dedicated_data_baseline_addr_s
{
  u32 address : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_local_dedicated_data_baseline_addr_u
{
  struct ipa_hwio_def_ipa_uc_local_dedicated_data_baseline_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_LOCAL_DEDICATED_DATA_BASELINE_SIZE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_local_dedicated_data_baseline_size_s
{
  u32 address : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_local_dedicated_data_baseline_size_u
{
  struct ipa_hwio_def_ipa_uc_local_dedicated_data_baseline_size_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_LOCAL_SHARED_DATA_BASELINE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_local_shared_data_baseline_addr_s
{
  u32 address : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_local_shared_data_baseline_addr_u
{
  struct ipa_hwio_def_ipa_uc_local_shared_data_baseline_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_LOCAL_SHARED_DATA_BASELINE_SIZE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_local_shared_data_baseline_size_s
{
  u32 address : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_local_shared_data_baseline_size_u
{
  struct ipa_hwio_def_ipa_uc_local_shared_data_baseline_size_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_CRYPTO_DECAPS_UC_DRAM_SECTOR_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_crypto_decaps_uc_dram_sector_base_addr_s
{
  u32 val : 17;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_crypto_decaps_uc_dram_sector_base_addr_u
{
  struct ipa_hwio_def_ipa_uc_ipa_crypto_decaps_uc_dram_sector_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_CRYPTO_IRQ_RESUME 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_crypto_irq_resume_s
{
  u32 resume_post_crypto : 1;
  u32 resume_pre_crypto : 1;
  u32 resume_anti_replay : 1;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_crypto_irq_resume_u
{
  struct ipa_hwio_def_ipa_uc_ipa_crypto_irq_resume_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_DECAPS_RESULT_EDITING 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_decaps_result_editing_s
{
  u32 pkt_out_of_win_error_clr : 1;
  u32 pkt_out_of_win_error_set : 1;
  u32 anti_replay_error_clr : 1;
  u32 anti_replay_error_set : 1;
  u32 pad_error_clr : 1;
  u32 pad_error_set : 1;
  u32 auth_error_clr : 1;
  u32 auth_error_set : 1;
  u32 l2_l3_hdr_len_error_clr : 1;
  u32 l2_l3_hdr_len_error_set : 1;
  u32 reserved0 : 22;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_decaps_result_editing_u
{
  struct ipa_hwio_def_ipa_uc_ipa_decaps_result_editing_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_DECAPS_RESULT_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_decaps_result_status_s
{
  u32 pkt_out_of_win_error_stts : 1;
  u32 anti_replay_error_stts : 1;
  u32 pad_error_stts : 1;
  u32 auth_error_stts : 1;
  u32 l2_l3_hdr_len_error_stts : 1;
  u32 reserved0 : 27;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_decaps_result_status_u
{
  struct ipa_hwio_def_ipa_uc_ipa_decaps_result_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_CRYPTO_PRE_CRYPTO_STAGE_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_crypto_pre_crypto_stage_status_s
{
  u32 ctx_id : 8;
  u32 crypto_action : 4;
  u32 fsm_state : 6;
  u32 reserved0 : 2;
  u32 busy : 1;
  u32 reserved1 : 3;
  u32 sa_id : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_crypto_pre_crypto_stage_status_u
{
  struct ipa_hwio_def_ipa_uc_ipa_crypto_pre_crypto_stage_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_CRYPTO_POST_CRYPTO_STAGE_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_crypto_post_crypto_stage_status_s
{
  u32 ctx_id : 8;
  u32 crypto_action : 4;
  u32 fsm_state : 6;
  u32 reserved0 : 2;
  u32 busy : 1;
  u32 reserved1 : 3;
  u32 sa_id : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_crypto_post_crypto_stage_status_u
{
  struct ipa_hwio_def_ipa_uc_ipa_crypto_post_crypto_stage_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_IPA_CRYPTO_n_CRYPTO_STAGE_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ipa_crypto_n_crypto_stage_status_s
{
  u32 ctx_id : 8;
  u32 crypto_action : 4;
  u32 fsm_state : 6;
  u32 reserved0 : 2;
  u32 busy : 1;
  u32 reserved1 : 3;
  u32 sa_id : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ipa_crypto_n_crypto_stage_status_u
{
  struct ipa_hwio_def_ipa_uc_ipa_crypto_n_crypto_stage_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_SPARE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_spare_s
{
  u32 spare : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_spare_u
{
  struct ipa_hwio_def_ipa_uc_spare_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_UC_MBOX
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_MAILBOX_m_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_mailbox_m_n_s
{
  u32 data : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_mailbox_m_n_u
{
  struct ipa_hwio_def_ipa_uc_mailbox_m_n_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_RAM
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_SW_BLOCK_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_sw_block_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_sw_block_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_sw_block_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_SW_BLOCK_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_sw_block_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_sw_block_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_sw_block_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_SNIFF_HW_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_sniff_hw_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_sniff_hw_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_sniff_hw_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_SNIFF_HW_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_sniff_hw_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_sniff_hw_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_sniff_hw_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_FRAG_TABLE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_frag_table_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_frag_table_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_frag_table_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_FRAG_TABLE_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_frag_table_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_frag_table_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_frag_table_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPA_GSI_IF_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_gsi_if_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipa_gsi_if_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_gsi_if_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPA_GSI_IF_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_gsi_if_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipa_gsi_if_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_gsi_if_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPA_NLO_VP_CACHE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_nlo_vp_cache_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipa_nlo_vp_cache_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_nlo_vp_cache_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPA_NLO_VP_CACHE_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_nlo_vp_cache_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipa_nlo_vp_cache_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_nlo_vp_cache_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPA_COAL_VP_FIFO_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_coal_vp_fifo_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipa_coal_vp_fifo_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_coal_vp_fifo_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPA_COAL_VP_FIFO_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_coal_vp_fifo_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipa_coal_vp_fifo_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_coal_vp_fifo_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TSP_QMNGR_BLOCK_ALLOC_LEVEL1_BITMAP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_block_alloc_level1_bitmap_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_block_alloc_level1_bitmap_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_block_alloc_level1_bitmap_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TSP_QMNGR_BLOCK_ALLOC_LEVEL1_BITMAP_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_block_alloc_level1_bitmap_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_block_alloc_level1_bitmap_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_block_alloc_level1_bitmap_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TSP_QMNGR_BLOCK_ALLOC_LEVEL2_BITMAP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_block_alloc_level2_bitmap_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_block_alloc_level2_bitmap_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_block_alloc_level2_bitmap_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TSP_QMNGR_BLOCK_ALLOC_LEVEL2_BITMAP_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_block_alloc_level2_bitmap_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_block_alloc_level2_bitmap_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_block_alloc_level2_bitmap_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TSP_SHAPER_PRODS_NEXT_RTC_DB_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_shaper_prods_next_rtc_db_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tsp_shaper_prods_next_rtc_db_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_shaper_prods_next_rtc_db_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TSP_SHAPER_PRODS_NEXT_RTC_DB_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_shaper_prods_next_rtc_db_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tsp_shaper_prods_next_rtc_db_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_shaper_prods_next_rtc_db_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TSP_SHAPER_TCS_NEXT_RTC_DB_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_shaper_tcs_next_rtc_db_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tsp_shaper_tcs_next_rtc_db_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_shaper_tcs_next_rtc_db_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TSP_SHAPER_TCS_NEXT_RTC_DB_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_shaper_tcs_next_rtc_db_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tsp_shaper_tcs_next_rtc_db_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_shaper_tcs_next_rtc_db_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_AS_TABLE_HASHES_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_hashes_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_hashes_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_hashes_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_AS_TABLE_HASHES_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_hashes_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_hashes_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_hashes_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_AS_TABLE_POINTERS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_pointers_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_pointers_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_pointers_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_AS_TABLE_POINTERS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_pointers_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_pointers_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_pointers_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_AS_TABLE_ENTERIES_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_enteries_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_enteries_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_enteries_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_AS_TABLE_ENTERIES_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_enteries_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_enteries_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_as_table_enteries_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_NAS_TABLE_HASHES_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_hashes_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_hashes_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_hashes_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_NAS_TABLE_HASHES_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_hashes_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_hashes_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_hashes_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_NAS_TABLE_POINTERS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_pointers_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_pointers_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_pointers_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_NAS_TABLE_POINTERS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_pointers_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_pointers_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_pointers_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_NAS_TABLE_ENTERIES_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_enteries_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_enteries_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_enteries_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_NAS_TABLE_ENTERIES_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_enteries_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_enteries_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_nas_table_enteries_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_NOTIFICATION_FIFO_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_notification_fifo_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_notification_fifo_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_notification_fifo_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RQOS_NOTIFICATION_FIFO_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_notification_fifo_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rqos_notification_fifo_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rqos_notification_fifo_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_BEARER_CONTEXTS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_bearer_contexts_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_bearer_contexts_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_bearer_contexts_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_BEARER_CONTEXTS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_bearer_contexts_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_bearer_contexts_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_bearer_contexts_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_NAT_AND_CONN_TRACK_CACHE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_nat_and_conn_track_cache_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_nat_and_conn_track_cache_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_nat_and_conn_track_cache_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_NAT_AND_CONN_TRACK_CACHE_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_nat_and_conn_track_cache_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_nat_and_conn_track_cache_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_nat_and_conn_track_cache_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_FILTER_ROUTER_CACHE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_filter_router_cache_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_filter_router_cache_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_filter_router_cache_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_FILTER_ROUTER_CACHE_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_filter_router_cache_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_filter_router_cache_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_filter_router_cache_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_HDR_SNAPSHOT_SECTORS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_hdr_snapshot_sectors_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_hdr_snapshot_sectors_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_hdr_snapshot_sectors_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_HDR_SNAPSHOT_SECTORS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_hdr_snapshot_sectors_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_hdr_snapshot_sectors_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_hdr_snapshot_sectors_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TSP_QMNGR_QUEUE_DB_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_queue_db_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_queue_db_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_queue_db_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TSP_QMNGR_QUEUE_DB_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_queue_db_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_queue_db_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_queue_db_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_HDR_SECTORS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_hdr_sectors_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_hdr_sectors_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_hdr_sectors_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_HDR_SECTORS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_hdr_sectors_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_hdr_sectors_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_hdr_sectors_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_SEG_HDR_SECTORS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_seg_hdr_sectors_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_seg_hdr_sectors_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_seg_hdr_sectors_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_SEG_HDR_SECTORS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_seg_hdr_sectors_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_seg_hdr_sectors_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_seg_hdr_sectors_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_PKT_CTX_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_pkt_ctx_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_pkt_ctx_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_pkt_ctx_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_PKT_CTX_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_pkt_ctx_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_pkt_ctx_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_pkt_ctx_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_DATA_SECTORS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_data_sectors_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_data_sectors_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_data_sectors_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_DATA_SECTORS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_data_sectors_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_data_sectors_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_data_sectors_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TSP_QMNGR_HEAD_TAIL_FIFOS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_head_tail_fifos_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_head_tail_fifos_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_head_tail_fifos_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TSP_QMNGR_HEAD_TAIL_FIFOS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_head_tail_fifos_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_head_tail_fifos_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tsp_qmngr_head_tail_fifos_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_CONS_DPL_FIFO_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_cons_dpl_fifo_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_cons_dpl_fifo_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_cons_dpl_fifo_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_CONS_DPL_FIFO_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_cons_dpl_fifo_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_cons_dpl_fifo_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_cons_dpl_fifo_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TRAILER_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_trailer_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_trailer_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_trailer_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TRAILER_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_trailer_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_trailer_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_trailer_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_PROD_DPL_FIFO_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_prod_dpl_fifo_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_prod_dpl_fifo_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_prod_dpl_fifo_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_PROD_DPL_FIFO_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_prod_dpl_fifo_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_prod_dpl_fifo_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_prod_dpl_fifo_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_HDRI_TYPE1_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_hdri_type1_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_hdri_type1_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_hdri_type1_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_HDRI_TYPE1_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_hdri_type1_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_hdri_type1_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_hdri_type1_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_COAL_MASTER_VP_CTX_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_master_vp_ctx_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_coal_master_vp_ctx_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_master_vp_ctx_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_COAL_MASTER_VP_CTX_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_master_vp_ctx_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_coal_master_vp_ctx_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_master_vp_ctx_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_COAL_MASTER_VP_AGGR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_master_vp_aggr_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_coal_master_vp_aggr_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_master_vp_aggr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_COAL_MASTER_VP_AGGR_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_master_vp_aggr_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_coal_master_vp_aggr_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_master_vp_aggr_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_COAL_SLAVE_VP_CTX_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_slave_vp_ctx_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_coal_slave_vp_ctx_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_slave_vp_ctx_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_COAL_SLAVE_VP_CTX_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_slave_vp_ctx_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_coal_slave_vp_ctx_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_slave_vp_ctx_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_COAL_CHECKSUM_VP_CTX_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_checksum_vp_ctx_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_coal_checksum_vp_ctx_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_checksum_vp_ctx_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_COAL_CHECKSUM_VP_CTX_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_checksum_vp_ctx_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_coal_checksum_vp_ctx_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_checksum_vp_ctx_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_COAL_CHECKSUM_VP_RAM_FIFO_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_checksum_vp_ram_fifo_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_coal_checksum_vp_ram_fifo_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_checksum_vp_ram_fifo_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_COAL_CHECKSUM_VP_RAM_FIFO_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_checksum_vp_ram_fifo_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_coal_checksum_vp_ram_fifo_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_coal_checksum_vp_ram_fifo_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_HDRI_WRAPPER_TLV_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_hdri_wrapper_tlv_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_hdri_wrapper_tlv_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_hdri_wrapper_tlv_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_HDRI_WRAPPER_TLV_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_hdri_wrapper_tlv_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_hdri_wrapper_tlv_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_hdri_wrapper_tlv_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TX_MBIM_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_mbim_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tx_mbim_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_mbim_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TX_MBIM_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_mbim_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tx_mbim_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_mbim_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_AGGR_NLO_COUNTERS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_aggr_nlo_counters_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_aggr_nlo_counters_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_aggr_nlo_counters_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_AGGR_NLO_COUNTERS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_aggr_nlo_counters_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_aggr_nlo_counters_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_aggr_nlo_counters_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPA_COAL_VP_CACHE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_coal_vp_cache_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipa_coal_vp_cache_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_coal_vp_cache_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPA_COAL_VP_CACHE_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_coal_vp_cache_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipa_coal_vp_cache_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipa_coal_vp_cache_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_FILTER_RULES_HASH_MEMCP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_filter_rules_hash_memcp_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_filter_rules_hash_memcp_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_filter_rules_hash_memcp_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_FILTER_RULES_HASH_MEMCP_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_filter_rules_hash_memcp_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_filter_rules_hash_memcp_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_filter_rules_hash_memcp_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_FILTER_RULES_NON_HASH_MEMCP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_filter_rules_non_hash_memcp_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_filter_rules_non_hash_memcp_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_filter_rules_non_hash_memcp_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_FILTER_RULES_NON_HASH_MEMCP_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_filter_rules_non_hash_memcp_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_filter_rules_non_hash_memcp_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_filter_rules_non_hash_memcp_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_ROUTER_RULES_HASH_MEMCP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_router_rules_hash_memcp_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_router_rules_hash_memcp_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_router_rules_hash_memcp_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_ROUTER_RULES_HASH_MEMCP_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_router_rules_hash_memcp_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_router_rules_hash_memcp_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_router_rules_hash_memcp_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_ROUTER_RULES_NON_HASH_MEMCP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_router_rules_non_hash_memcp_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_router_rules_non_hash_memcp_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_router_rules_non_hash_memcp_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_ROUTER_RULES_NON_HASH_MEMCP_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_router_rules_non_hash_memcp_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_router_rules_non_hash_memcp_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_router_rules_non_hash_memcp_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_AGGR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_aggr_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_aggr_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_aggr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_AGGR_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_aggr_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_aggr_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_aggr_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RX_RNDIS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_rndis_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rx_rndis_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_rndis_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RX_RNDIS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_rndis_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rx_rndis_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_rndis_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_UL_NLO_AGGR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ul_nlo_aggr_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ul_nlo_aggr_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ul_nlo_aggr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_UL_NLO_AGGR_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ul_nlo_aggr_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ul_nlo_aggr_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ul_nlo_aggr_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TX_CHECKSUM_CTX_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_checksum_ctx_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tx_checksum_ctx_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_checksum_ctx_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TX_CHECKSUM_CTX_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_checksum_ctx_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tx_checksum_ctx_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_checksum_ctx_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TX_AGGR_FIFO_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_aggr_fifo_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tx_aggr_fifo_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_aggr_fifo_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TX_AGGR_FIFO_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_aggr_fifo_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tx_aggr_fifo_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_aggr_fifo_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TX_TRANSMISSION_FIFO_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_transmission_fifo_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tx_transmission_fifo_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_transmission_fifo_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TX_TRANSMISSION_FIFO_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_transmission_fifo_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tx_transmission_fifo_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_transmission_fifo_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_SEG_FRAME_DATABASE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_seg_frame_database_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_seg_frame_database_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_seg_frame_database_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_SEG_FRAME_DATABASE_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_seg_frame_database_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_seg_frame_database_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_seg_frame_database_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TX_CHECKSUM_SEG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_checksum_seg_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tx_checksum_seg_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_checksum_seg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TX_CHECKSUM_SEG_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_checksum_seg_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_tx_checksum_seg_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_tx_checksum_seg_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_NAT_RD_BUFFER_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_nat_rd_buffer_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_nat_rd_buffer_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_nat_rd_buffer_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_NAT_RD_BUFFER_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_nat_rd_buffer_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_nat_rd_buffer_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_nat_rd_buffer_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_NAT_WR_BUFFER_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_nat_wr_buffer_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_nat_wr_buffer_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_nat_wr_buffer_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_NAT_WR_BUFFER_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_nat_wr_buffer_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_nat_wr_buffer_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_nat_wr_buffer_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RX_MBIM_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_mbim_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rx_mbim_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_mbim_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RX_MBIM_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_mbim_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rx_mbim_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_mbim_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RX_IM_CMD_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_im_cmd_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rx_im_cmd_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_im_cmd_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RX_IM_CMD_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_im_cmd_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rx_im_cmd_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_im_cmd_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IMM_REG_RD_WR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_imm_reg_rd_wr_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_imm_reg_rd_wr_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_imm_reg_rd_wr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IMM_REG_RD_WR_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_imm_reg_rd_wr_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_imm_reg_rd_wr_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_imm_reg_rd_wr_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_DATA_DESC_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_data_desc_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_data_desc_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_data_desc_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_DATA_DESC_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_data_desc_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_data_desc_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_data_desc_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TEMP_FOR_INTERLEAVING_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_temp_for_interleaving_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_temp_for_interleaving_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_temp_for_interleaving_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_TEMP_FOR_INTERLEAVING_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_temp_for_interleaving_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_temp_for_interleaving_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_temp_for_interleaving_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RX_DL_NLO_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_dl_nlo_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rx_dl_nlo_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_dl_nlo_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_RX_DL_NLO_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_dl_nlo_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_rx_dl_nlo_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_rx_dl_nlo_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_POST_CRYPTO_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_post_crypto_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_post_crypto_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_post_crypto_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_POST_CRYPTO_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_post_crypto_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_post_crypto_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_post_crypto_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_DCPH_KEYS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_dcph_keys_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_dcph_keys_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_dcph_keys_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_DCPH_KEYS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_dcph_keys_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_dcph_keys_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_dcph_keys_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPSEC_ENCAPS_ENC_KEYS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_enc_keys_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_enc_keys_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_enc_keys_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPSEC_ENCAPS_ENC_KEYS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_enc_keys_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_enc_keys_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_enc_keys_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPSEC_DECAPS_ENC_KEYS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_decaps_enc_keys_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_decaps_enc_keys_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_decaps_enc_keys_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPSEC_DECAPS_ENC_KEYS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_decaps_enc_keys_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_decaps_enc_keys_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_decaps_enc_keys_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPSEC_ENCAPS_AUTH_KEYS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_auth_keys_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_auth_keys_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_auth_keys_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPSEC_ENCAPS_AUTH_KEYS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_auth_keys_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_auth_keys_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_auth_keys_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPSEC_ENCAPS_ENC_IV_KEYS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_enc_iv_keys_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_enc_iv_keys_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_enc_iv_keys_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPSEC_ENCAPS_ENC_IV_KEYS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_enc_iv_keys_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_enc_iv_keys_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_encaps_enc_iv_keys_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPSEC_DECAPS_AUTH_KEYS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_decaps_auth_keys_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_decaps_auth_keys_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_decaps_auth_keys_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_IPSEC_DECAPS_AUTH_KEYS_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_decaps_auth_keys_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_decaps_auth_keys_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_ipsec_decaps_auth_keys_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_ZUC256_TMPLT_TBL_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_zuc256_tmplt_tbl_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_zuc256_tmplt_tbl_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_zuc256_tmplt_tbl_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_ZUC256_TMPLT_TBL_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_zuc256_tmplt_tbl_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_zuc256_tmplt_tbl_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_zuc256_tmplt_tbl_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_UC_IRAM_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_uc_iram_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_uc_iram_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_uc_iram_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_UC_IRAM_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_uc_iram_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_uc_iram_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_uc_iram_last_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_UC_DRAM_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_uc_dram_n_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_uc_dram_n_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_uc_dram_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RAM_DIRECT_ACCESS_RAM_UC_DRAM_LAST 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ram_direct_access_ram_uc_dram_last_s
{
  u32 data_word : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ram_direct_access_ram_uc_dram_last_u
{
  struct ipa_hwio_def_ipa_ram_direct_access_ram_uc_dram_last_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_EE
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IRQ_STTS_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_irq_stts_ee_n_s
{
  u32 reserved0 : 2;
  u32 uc_irq_0 : 1;
  u32 uc_irq_1 : 1;
  u32 uc_irq_2 : 1;
  u32 uc_irq_3 : 1;
  u32 uc_in_q_not_empty_irq : 1;
  u32 uc_rx_cmd_q_not_full_irq : 1;
  u32 proc_to_uc_ack_q_not_empty_irq : 1;
  u32 tsp_flushed_tc_irq : 1;
  u32 reserved1 : 4;
  u32 tx_suspend_irq : 1;
  u32 tx_holb_drop_irq : 1;
  u32 gsi_idle_irq : 1;
  u32 reserved2 : 4;
  u32 ucp_irq : 1;
  u32 reserved3 : 1;
  u32 gsi_ee_irq : 1;
  u32 gsi_ipa_if_tlv_rcvd_irq : 1;
  u32 gsi_uc_irq : 1;
  u32 reserved4 : 4;
  u32 ipa_error_non_fatal_irq : 1;
  u32 ipa_error_fatal_irq : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_irq_stts_ee_n_u
{
  struct ipa_hwio_def_ipa_irq_stts_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IRQ_EN_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_irq_en_ee_n_s
{
  u32 reserved0 : 2;
  u32 uc_irq_0_irq_en : 1;
  u32 uc_irq_1_irq_en : 1;
  u32 uc_irq_2_irq_en : 1;
  u32 uc_irq_3_irq_en : 1;
  u32 uc_in_q_not_empty_irq_en : 1;
  u32 uc_rx_cmd_q_not_full_irq_en : 1;
  u32 proc_to_uc_ack_q_not_empty_irq_en : 1;
  u32 tsp_flushed_tc_irq_en : 1;
  u32 reserved1 : 4;
  u32 tx_suspend_irq_en : 1;
  u32 tx_holb_drop_irq_en : 1;
  u32 gsi_idle_irq_en : 1;
  u32 reserved2 : 4;
  u32 ucp_irq_en : 1;
  u32 reserved3 : 1;
  u32 gsi_ee_irq_en : 1;
  u32 gsi_ipa_if_tlv_rcvd_irq_en : 1;
  u32 gsi_uc_irq_en : 1;
  u32 reserved4 : 4;
  u32 ipa_error_non_fatal_irq_en : 1;
  u32 ipa_error_fatal_irq_en : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_irq_en_ee_n_u
{
  struct ipa_hwio_def_ipa_irq_en_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IRQ_CLR_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_irq_clr_ee_n_s
{
  u32 reserved0 : 2;
  u32 uc_irq_0_clr : 1;
  u32 uc_irq_1_clr : 1;
  u32 uc_irq_2_clr : 1;
  u32 uc_irq_3_clr : 1;
  u32 uc_in_q_not_empty_irq_clr : 1;
  u32 uc_rx_cmd_q_not_full_irq_clr : 1;
  u32 proc_to_uc_ack_q_not_empty_irq_clr : 1;
  u32 tsp_flushed_tc_irq_clr : 1;
  u32 reserved1 : 4;
  u32 tx_suspend_irq_clr : 1;
  u32 tx_holb_drop_irq_clr : 1;
  u32 gsi_idle_irq_clr : 1;
  u32 reserved2 : 4;
  u32 ucp_irq_clr : 1;
  u32 reserved3 : 1;
  u32 gsi_ee_irq_clr : 1;
  u32 gsi_ipa_if_tlv_rcvd_irq_clr : 1;
  u32 gsi_uc_irq_clr : 1;
  u32 reserved4 : 4;
  u32 ipa_error_non_fatal_clr : 1;
  u32 ipa_error_fatal_clr : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_irq_clr_ee_n_u
{
  struct ipa_hwio_def_ipa_irq_clr_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SNOC_FEC_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_snoc_fec_ee_n_s
{
  u32 client : 8;
  u32 noc_port : 1;
  u32 noc_master : 3;
  u32 tid : 5;
  u32 reserved0 : 11;
  u32 valid : 1;
  u32 clear : 1;
  u32 reserved1 : 1;
  u32 direction : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_snoc_fec_ee_n_u
{
  struct ipa_hwio_def_ipa_snoc_fec_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IRQ_EE_UC_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_irq_ee_uc_n_s
{
  u32 intr : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_irq_ee_uc_n_u
{
  struct ipa_hwio_def_ipa_irq_ee_uc_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FEC_FATAL_ADDR_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_fec_fatal_addr_ee_n_s
{
  u32 addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_fec_fatal_addr_ee_n_u
{
  struct ipa_hwio_def_ipa_fec_fatal_addr_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FEC_FATAL_ADDR_MSB_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_fec_fatal_addr_msb_ee_n_s
{
  u32 addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_fec_fatal_addr_msb_ee_n_u
{
  struct ipa_hwio_def_ipa_fec_fatal_addr_msb_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FEC_FATAL_ATTR_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_fec_fatal_attr_ee_n_s
{
  u32 opcode : 6;
  u32 error_info : 26;
};

/* Union definition of register */
union ipa_hwio_def_ipa_fec_fatal_attr_ee_n_u
{
  struct ipa_hwio_def_ipa_fec_fatal_attr_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SUSPEND_IRQ_INFO_EE_n_REG_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_suspend_irq_info_ee_n_reg_k_s
{
  u32 endpoints : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_suspend_irq_info_ee_n_reg_k_u
{
  struct ipa_hwio_def_ipa_suspend_irq_info_ee_n_reg_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SUSPEND_IRQ_EN_EE_n_REG_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_suspend_irq_en_ee_n_reg_k_s
{
  u32 endpoints : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_suspend_irq_en_ee_n_reg_k_u
{
  struct ipa_hwio_def_ipa_suspend_irq_en_ee_n_reg_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DRBIP_FEC_INFO_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drbip_fec_info_ee_n_s
{
  u32 error_code : 4;
  u32 src_grp : 4;
  u32 reserved0 : 4;
  u32 required_data_sectors : 9;
  u32 reserved1 : 2;
  u32 avail_data_sectors : 9;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drbip_fec_info_ee_n_u
{
  struct ipa_hwio_def_ipa_drbip_fec_info_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DRBIP_FEC_INFO_EXT_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drbip_fec_info_ext_ee_n_s
{
  u32 size : 16;
  u32 opcode : 8;
  u32 src_pipe : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drbip_fec_info_ext_ee_n_u
{
  struct ipa_hwio_def_ipa_drbip_fec_info_ext_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DISALLOW_IDLE_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_disallow_idle_ee_n_s
{
  u32 val : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_disallow_idle_ee_n_u
{
  struct ipa_hwio_def_ipa_disallow_idle_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SUSPEND_IRQ_CLR_EE_n_REG_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_suspend_irq_clr_ee_n_reg_k_s
{
  u32 endpoints : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_suspend_irq_clr_ee_n_reg_k_u
{
  struct ipa_hwio_def_ipa_suspend_irq_clr_ee_n_reg_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HOLB_DROP_IRQ_INFO_EE_n_REG_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_holb_drop_irq_info_ee_n_reg_k_s
{
  u32 endpoints : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_holb_drop_irq_info_ee_n_reg_k_u
{
  struct ipa_hwio_def_ipa_holb_drop_irq_info_ee_n_reg_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HOLB_DROP_IRQ_EN_EE_n_REG_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_holb_drop_irq_en_ee_n_reg_k_s
{
  u32 endpoints : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_holb_drop_irq_en_ee_n_reg_k_u
{
  struct ipa_hwio_def_ipa_holb_drop_irq_en_ee_n_reg_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HOLB_DROP_IRQ_CLR_EE_n_REG_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_holb_drop_irq_clr_ee_n_reg_k_s
{
  u32 endpoints : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_holb_drop_irq_clr_ee_n_reg_k_u
{
  struct ipa_hwio_def_ipa_holb_drop_irq_clr_ee_n_reg_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IRQ_STTS_EE_ERROR_FATAL_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_irq_stts_ee_error_fatal_n_s
{
  u32 bad_snoc_access_irq : 1;
  u32 proc_err_irq : 1;
  u32 rx_err_irq : 1;
  u32 tlv_len_min_dsm_irq : 1;
  u32 drbip_pkt_exceed_max_size_irq : 1;
  u32 drbip_data_sctr_cfg_error_irq : 1;
  u32 drbip_imm_cmd_no_flsh_hzrd_irq : 1;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_irq_stts_ee_error_fatal_n_u
{
  struct ipa_hwio_def_ipa_irq_stts_ee_error_fatal_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IRQ_EN_EE_ERROR_FATAL_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_irq_en_ee_error_fatal_n_s
{
  u32 bad_snoc_access_irq_en : 1;
  u32 proc_err_irq_en : 1;
  u32 rx_err_irq_en : 1;
  u32 tlv_len_min_dsm_irq_en : 1;
  u32 drbip_pkt_exceed_max_size_irq_en : 1;
  u32 drbip_data_sctr_cfg_error_irq_en : 1;
  u32 drbip_imm_cmd_no_flsh_hzrd_irq_en : 1;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_irq_en_ee_error_fatal_n_u
{
  struct ipa_hwio_def_ipa_irq_en_ee_error_fatal_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IRQ_CLR_EE_ERROR_FATAL_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_irq_clr_ee_error_fatal_n_s
{
  u32 bad_snoc_access_irq_clr : 1;
  u32 proc_err_irq_clr : 1;
  u32 rx_err_irq_clr : 1;
  u32 tlv_len_min_dsm_irq_clr : 1;
  u32 drbip_pkt_exceed_max_size_irq_clr : 1;
  u32 drbip_data_sctr_cfg_error_irq_clr : 1;
  u32 drbip_imm_cmd_no_flsh_hzrd_irq_clr : 1;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_irq_clr_ee_error_fatal_n_u
{
  struct ipa_hwio_def_ipa_irq_clr_ee_error_fatal_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IRQ_STTS_EE_ERROR_NON_FATAL_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_irq_stts_ee_error_non_fatal_n_s
{
  u32 deaggr_err_irq : 1;
  u32 proc_err_irq : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_irq_stts_ee_error_non_fatal_n_u
{
  struct ipa_hwio_def_ipa_irq_stts_ee_error_non_fatal_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IRQ_EN_EE_ERROR_NON_FATAL_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_irq_en_ee_error_non_fatal_n_s
{
  u32 deaggr_err_irq_en : 1;
  u32 proc_err_irq_en : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_irq_en_ee_error_non_fatal_n_u
{
  struct ipa_hwio_def_ipa_irq_en_ee_error_non_fatal_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IRQ_CLR_EE_ERROR_NON_FATAL_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_irq_clr_ee_error_non_fatal_n_s
{
  u32 deaggr_err_irq_clr : 1;
  u32 proc_err_irq_clr : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_irq_clr_ee_error_non_fatal_n_u
{
  struct ipa_hwio_def_ipa_irq_clr_ee_error_non_fatal_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FEC_NON_FATAL_ADDR_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_fec_non_fatal_addr_ee_n_s
{
  u32 addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_fec_non_fatal_addr_ee_n_u
{
  struct ipa_hwio_def_ipa_fec_non_fatal_addr_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FEC_NON_FATAL_ADDR_MSB_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_fec_non_fatal_addr_msb_ee_n_s
{
  u32 addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_fec_non_fatal_addr_msb_ee_n_u
{
  struct ipa_hwio_def_ipa_fec_non_fatal_addr_msb_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FEC_NON_FATAL_ATTR_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_fec_non_fatal_attr_ee_n_s
{
  u32 opcode : 6;
  u32 error_info : 26;
};

/* Union definition of register */
union ipa_hwio_def_ipa_fec_non_fatal_attr_ee_n_u
{
  struct ipa_hwio_def_ipa_fec_non_fatal_attr_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SECURED_PIPES_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_secured_pipes_n_s
{
  u32 endpoints : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_secured_pipes_n_u
{
  struct ipa_hwio_def_ipa_secured_pipes_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_REGS_INSIDE_IPA__CONTROL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_regs_inside_ipa__control_s
{
  u32 uc_enable : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_regs_inside_ipa__control_u
{
  struct ipa_hwio_def_ipa_uc_regs_inside_ipa__control_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_REGS_INSIDE_IPA__NMI 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_regs_inside_ipa__nmi_s
{
  u32 pulse : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_regs_inside_ipa__nmi_u
{
  struct ipa_hwio_def_ipa_uc_regs_inside_ipa__nmi_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_BEARER_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_bearer_cfg_s
{
  u32 bearer_context_index_width : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_bearer_cfg_u
{
  struct ipa_hwio_def_ipa_bearer_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RQOS_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rqos_cfg_s
{
  u32 rqos_notification_pipe : 8;
  u32 rqos_as_reduce_level : 8;
  u32 rqos_nas_reduce_level : 8;
  u32 rqos_as_reduce_level_en : 1;
  u32 rqos_nas_reduce_level_en : 1;
  u32 rqos_nas_unknown_protocol_pkt_cnt_reset : 1;
  u32 reserved0 : 5;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rqos_cfg_u
{
  struct ipa_hwio_def_ipa_rqos_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RQOS_NAS_UNKNOWN_PROTOCOL_PKT_CNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rqos_nas_unknown_protocol_pkt_cnt_s
{
  u32 count : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rqos_nas_unknown_protocol_pkt_cnt_u
{
  struct ipa_hwio_def_ipa_rqos_nas_unknown_protocol_pkt_cnt_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RQOS_ILLEGAL_PROTOCOL_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rqos_illegal_protocol_0_s
{
  u32 protocol_0 : 8;
  u32 protocol_1 : 8;
  u32 protocol_2 : 8;
  u32 protocol_3 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rqos_illegal_protocol_0_u
{
  struct ipa_hwio_def_ipa_rqos_illegal_protocol_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RQOS_ILLEGAL_PROTOCOL_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rqos_illegal_protocol_1_s
{
  u32 protocol_0 : 8;
  u32 protocol_1 : 8;
  u32 protocol_2 : 8;
  u32 protocol_3 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rqos_illegal_protocol_1_u
{
  struct ipa_hwio_def_ipa_rqos_illegal_protocol_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RQOS_ILLEGAL_PROTOCOL_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rqos_illegal_protocol_2_s
{
  u32 protocol_0 : 8;
  u32 protocol_1 : 8;
  u32 protocol_2 : 8;
  u32 protocol_3 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rqos_illegal_protocol_2_u
{
  struct ipa_hwio_def_ipa_rqos_illegal_protocol_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RQOS_ILLEGAL_PROTOCOL_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rqos_illegal_protocol_3_s
{
  u32 protocol_0 : 8;
  u32 protocol_1 : 8;
  u32 protocol_2 : 8;
  u32 protocol_3 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rqos_illegal_protocol_3_u
{
  struct ipa_hwio_def_ipa_rqos_illegal_protocol_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RQOS_THRESHOLD_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rqos_threshold_cfg_s
{
  u32 threshold : 12;
  u32 reserved0 : 4;
  u32 threshold_en : 1;
  u32 reserved1 : 3;
  u32 threshold_timer_gran_sel : 4;
  u32 reserved2 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rqos_threshold_cfg_u
{
  struct ipa_hwio_def_ipa_rqos_threshold_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MACI_ENGINE_CFG0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_maci_engine_cfg0_s
{
  u32 tbf_zuc_init_rnd : 6;
  u32 zuc256_sd_ver : 2;
  u32 zuc256_iv_ver : 1;
  u32 zuc256_iv_byte0_lsb : 3;
  u32 reserved0 : 4;
  u32 zuc256_extra_iv_msb : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_maci_engine_cfg0_u
{
  struct ipa_hwio_def_ipa_maci_engine_cfg0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MACI_ENGINE_CFG1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_maci_engine_cfg1_s
{
  u32 zuc256_extra_iv_lsb : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_maci_engine_cfg1_u
{
  struct ipa_hwio_def_ipa_maci_engine_cfg1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CPHR_SZ_ENGINE_CFG0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_cphr_sz_engine_cfg0_s
{
  u32 tbf_zuc_init_rnd : 6;
  u32 zuc256_sd_ver : 2;
  u32 zuc256_iv_ver : 1;
  u32 zuc256_iv_byte0 : 8;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_cphr_sz_engine_cfg0_u
{
  struct ipa_hwio_def_ipa_cphr_sz_engine_cfg0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CPHR_SZ_ENGINE_CFG1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_cphr_sz_engine_cfg1_s
{
  u32 zuc256_extra_iv_lsb : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_cphr_sz_engine_cfg1_u
{
  struct ipa_hwio_def_ipa_cphr_sz_engine_cfg1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CPHR_SZ_ENGINE_CFG2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_cphr_sz_engine_cfg2_s
{
  u32 zuc256_extra_iv_msb : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_cphr_sz_engine_cfg2_u
{
  struct ipa_hwio_def_ipa_cphr_sz_engine_cfg2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_BEARER_DROP_STATE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_bearer_drop_state_n_s
{
  u32 bearer_drop : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_bearer_drop_state_n_u
{
  struct ipa_hwio_def_ipa_bearer_drop_state_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SET_UC_IRQ_EE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_set_uc_irq_ee_n_s
{
  u32 set_uc_irq_0 : 1;
  u32 set_uc_irq_1 : 1;
  u32 set_uc_irq_2 : 1;
  u32 set_uc_irq_3 : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_set_uc_irq_ee_n_u
{
  struct ipa_hwio_def_ipa_set_uc_irq_ee_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SET_UC_IRQ_ALL_EES 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_set_uc_irq_all_ees_s
{
  u32 set_uc_irq_0 : 1;
  u32 set_uc_irq_1 : 1;
  u32 set_uc_irq_2 : 1;
  u32 set_uc_irq_3 : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_set_uc_irq_all_ees_u
{
  struct ipa_hwio_def_ipa_set_uc_irq_all_ees_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROC_UCP_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_proc_ucp_cfg_s
{
  u32 ipa_ucp_irq_sw_events_uc_mux_en : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_proc_ucp_cfg_u
{
  struct ipa_hwio_def_ipa_proc_ucp_cfg_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_CTX
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_0_s
{
  u32 opcode : 4;
  u32 state : 5;
  u32 stats_disable : 1;
  u32 exc_ucp : 1;
  u32 tx_pkt_dma_done : 1;
  u32 exc_deagg : 1;
  u32 exc_pkt_version : 1;
  u32 exc_pkt_len : 1;
  u32 exc_threshold : 1;
  u32 exc_sw : 1;
  u32 exc_nat : 1;
  u32 exc_frag_miss : 1;
  u32 exc_checksum_offload : 1;
  u32 exc_ipv4_checksum : 1;
  u32 exc_hdri : 1;
  u32 exc_pipe : 1;
  u32 exc_decaps : 1;
  u32 exc_drbip : 1;
  u32 exc_rqos : 1;
  u32 exc_ttl : 1;
  u32 exc_mtu_exceeded_uc : 1;
  u32 exc_mtu_exceeded_hpc : 1;
  u32 exc_incoming_l4_checksum : 1;
  u32 filter_bypass : 1;
  u32 router_bypass : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_0_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_1_s
{
  u32 nat_bypass : 1;
  u32 hdri_bypass : 1;
  u32 dcph_bypass : 1;
  u32 security_credentials_select : 1;
  u32 dcph_valid : 1;
  u32 round_bypass : 1;
  u32 next_round_en : 1;
  u32 bearer_valid : 1;
  u32 ucp_on : 1;
  u32 replication : 1;
  u32 src_stts_en : 1;
  u32 dest_stts_en : 1;
  u32 frag_stts_en : 1;
  u32 eot_dest : 1;
  u32 eot_notif : 1;
  u32 prev_eot_dest : 1;
  u32 l2_len : 9;
  u32 dsa_tagged : 1;
  u32 ucp_on_for_stts : 1;
  u32 pkt_parser_bypass : 1;
  u32 nat_in_hdrs : 1;
  u32 rx_flags_lsb : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_1_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_2_s
{
  u32 rx_flags_msb : 5;
  u32 rx_packet_length : 16;
  u32 revised_packet_length_lsb : 11;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_2_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_3_s
{
  u32 revised_packet_length_msb : 5;
  u32 frag_en : 1;
  u32 frag_bypass : 1;
  u32 frag_process : 1;
  u32 filter_aggr_force_close : 1;
  u32 router_aggr_force_close : 1;
  u32 src_id : 8;
  u32 src_pipe : 8;
  u32 rsrvd : 6;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_3_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_4 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_4_s
{
  u32 dest_pipe : 8;
  u32 crypto_action : 4;
  u32 ipsec_mode : 1;
  u32 ihl_offset : 6;
  u32 d_dcph_pass : 1;
  u32 protocol : 8;
  u32 tos_lsb : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_4_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_4_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_5 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_5_s
{
  u32 tos_msb : 4;
  u32 id : 16;
  u32 v6_reserved : 4;
  u32 ff : 1;
  u32 mf : 1;
  u32 pkt_is_frag : 1;
  u32 cs_disable_trailer_valid_bit : 1;
  u32 trnseq_0 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_5_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_5_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_6 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_6_s
{
  u32 trnseq_1 : 4;
  u32 trnseq_2 : 4;
  u32 trnseq_3 : 4;
  u32 trnseq_4 : 4;
  u32 trnseq_5 : 4;
  u32 trnseq_ex_length : 6;
  u32 trnseq_4_length_lsb : 6;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_6_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_6_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_7 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_7_s
{
  u32 trnseq_4_length_msb : 4;
  u32 trnseq_4_offset : 9;
  u32 trnseq_5_length : 10;
  u32 trnseq_5_offset : 9;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_7_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_7_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_8 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_8_s
{
  u32 dps_tx_pop_cnt : 2;
  u32 dps_tx_push_cnt : 2;
  u32 vol_ic_dcph_cfg : 1;
  u32 vol_ic_tag_stts : 1;
  u32 vol_ic_pkt_init_ex : 1;
  u32 vol_ic_pkt_init : 1;
  u32 trnseq_0_preucp : 1;
  u32 dest_pipe_overridden_ucp : 1;
  u32 force_to_default : 1;
  u32 close_vp_before : 1;
  u32 vol_ic_eob_bubble : 1;
  u32 ipsec_order_mutex : 1;
  u32 maci_bytes_in_trnseq : 1;
  u32 drop_drbip : 1;
  u32 drop_decaps_dummy_pkt : 1;
  u32 drbip_valid : 1;
  u32 tx_pkt_suspended : 1;
  u32 rb : 1;
  u32 packet_ethernet_parsing_done : 1;
  u32 tcp_win_size_lsb : 11;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_8_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_8_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_9 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_9_s
{
  u32 tcp_win_size_msb : 5;
  u32 trnseq_0_length : 10;
  u32 trnseq_0_offset : 9;
  u32 trnseq_1_length_lsb : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_9_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_9_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_10 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_10_s
{
  u32 trnseq_1_length_msb : 2;
  u32 trnseq_1_offset : 9;
  u32 trnseq_2_length : 10;
  u32 trnseq_2_offset : 9;
  u32 trnseq_3_length_lsb : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_10_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_10_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_11 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_11_s
{
  u32 trnseq_3_length_msb : 8;
  u32 trnseq_3_offset : 9;
  u32 rsrvd : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_11_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_11_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_12 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_12_s
{
  u32 dmar_valid_length : 16;
  u32 dcph_valid_length : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_12_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_12_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_13 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_13_s
{
  u32 frag_pipe : 8;
  u32 notif_pipe : 8;
  u32 coal_vp_valid : 1;
  u32 coal_vp_opcode_valid : 1;
  u32 coal_vp_open : 1;
  u32 coal_vp_eot : 1;
  u32 snapshot_req : 1;
  u32 vp_index : 8;
  u32 tsp_drop : 1;
  u32 desc_count_index_lsb : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_13_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_13_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_14 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_14_s
{
  u32 desc_count_index_msb : 7;
  u32 desc_offset : 10;
  u32 egress_tc : 8;
  u32 hps_round_cnt : 3;
  u32 dispatcher_pass : 1;
  u32 rsrvd : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_14_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_14_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_16 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_16_s
{
  u32 frag_stts_opcode : 8;
  u32 frag_stts_rule : 4;
  u32 frag_stts_space1 : 1;
  u32 frag_stts_exception : 1;
  u32 frag_stts_table : 2;
  u32 frag_stts_space2 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_16_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_16_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_17 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_17_s
{
  u32 frag_stts_src_ip_address : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_17_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_17_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_18 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_18_s
{
  u32 frag_stts_dst_ip_address : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_18_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_18_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_19 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_19_s
{
  u32 frag_stts_dest_pipe : 8;
  u32 frag_stts_protocol : 8;
  u32 frag_stts_id : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_19_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_19_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_20 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_20_s
{
  u32 frag_stts_nat_ip_address : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_20_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_20_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_21 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_21_s
{
  u32 frag_stts_header_offset : 16;
  u32 frag_stts_nat_ip_cs_diff : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_21_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_21_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_22 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_22_s
{
  u32 frag_stts_metadata_pre_nat : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_22_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_22_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_23 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_23_s
{
  u32 frag_stts_filter_stats_index : 12;
  u32 frag_stts_fr_ret : 1;
  u32 frag_stts_fnr_aggr_fc : 1;
  u32 frag_stts_ttl_update : 1;
  u32 frag_stts_packet_type : 1;
  u32 frag_stts_router_stats_index : 12;
  u32 frag_stts_ipv4_nat_or_conn_track : 1;
  u32 frag_stts_hpc_en : 1;
  u32 frag_stts_ingress_policer_dis : 1;
  u32 frag_stts_hdr_l : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_23_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_23_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_24 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_24_s
{
  u32 frag_stts_egress_tc : 8;
  u32 frag_stts_ingress_tc : 8;
  u32 frag_stts_space_3 : 6;
  u32 frag_stts_nat_type : 2;
  u32 frag_stts_hpc_fetch_length : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_24_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_24_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_25 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_25_s
{
  u32 frag_stts_nat_stats_index : 16;
  u32 frag_stts_reserved_word9 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_25_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_25_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_26 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_26_s
{
  u32 frag_stts_sw_cookie_lsb : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_26_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_26_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_27 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_27_s
{
  u32 frag_stts_sw_cookie_msb : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_27_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_27_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_28 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_28_s
{
  u32 pkt_stts_opcode : 8;
  u32 pkt_stts_exception_part1_or_drop_type : 4;
  u32 pkt_stts_exception_part2_or_drop_reason : 4;
  u32 pkt_stts_space6 : 1;
  u32 pkt_stts_filter_process : 1;
  u32 pkt_stts_nat_process : 1;
  u32 pkt_stts_router_process : 1;
  u32 pkt_stts_space7 : 4;
  u32 pkt_stts_checksum_process : 1;
  u32 pkt_stts_aggr_en : 1;
  u32 pkt_stts_open_frame : 1;
  u32 pkt_stts_deaggr_en : 1;
  u32 pkt_stts_deaggr_first : 1;
  u32 pkt_stts_src_eot : 1;
  u32 pkt_stts_rqos_nas_valid : 1;
  u32 pkt_stts_rqos_as_valid : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_28_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_28_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_29 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_29_s
{
  u32 pkt_stts_packet_length : 16;
  u32 pkt_stts_src_pipe : 8;
  u32 pkt_stts_pure_ack : 1;
  u32 pkt_stts_syn : 1;
  u32 pkt_stts_tcp_fin_rst : 1;
  u32 pkt_stts_rt_l : 1;
  u32 pkt_stts_rt_cache_hit : 1;
  u32 pkt_stts_protocol_encoding : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_29_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_29_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_30 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_30_s
{
  u32 pkt_stts_metadata : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_30_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_30_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_31 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_31_s
{
  u32 pkt_stts_fr_l : 1;
  u32 pkt_stts_fl_cache_hit : 1;
  u32 pkt_stts_ucp_en : 1;
  u32 pkt_stts_fr_ret : 1;
  u32 pkt_stts_fr_rule_id : 10;
  u32 pkt_stts_rt_table_index : 8;
  u32 pkt_stts_rt_rule_id : 10;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_31_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_31_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_32 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_32_s
{
  u32 pkt_stts_nat_entry_index : 16;
  u32 pkt_stts_tag_info_part1 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_32_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_32_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_33 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_33_s
{
  u32 pkt_stts_tag_info_part2 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_33_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_33_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_34 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_34_s
{
  u32 pkt_stts_seq_id : 8;
  u32 pkt_stts_tod : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_34_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_34_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_35 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_35_s
{
  u32 pkt_stts_header_offset : 16;
  u32 pkt_stts_hpc_en : 1;
  u32 pkt_stts_hdr_l : 1;
  u32 pkt_stts_ttl_updated : 1;
  u32 pkt_stts_frag_hit : 1;
  u32 pkt_stts_packet_type : 3;
  u32 pkt_stts_ipv4 : 1;
  u32 pkt_stts_dest_pipe : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_35_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_35_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_36 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_36_s
{
  u32 pkt_stts_egress_tc : 8;
  u32 pkt_stts_ingress_tc : 8;
  u32 pkt_stts_nat_hit : 1;
  u32 pkt_stts_nat_type : 2;
  u32 pkt_stts_nat_cache_hit : 1;
  u32 pkt_stts_ipv4_nat_or_conn_track : 1;
  u32 pkt_stts_nat_exc_suppress : 1;
  u32 pkt_stts_vlan_tag_cnt : 2;
  u32 pkt_stts_filter_table_index : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_36_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_36_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_37 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_37_s
{
  u32 pkt_stts_frag_rule : 4;
  u32 pkt_stts_metadata_type : 4;
  u32 pkt_stts_space15 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_37_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_37_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_38 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_38_s
{
  u32 pkt_stts_space16 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_38_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_38_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_39 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_39_s
{
  u32 pkt_stts_space17 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_39_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_39_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_40 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_40_s
{
  u32 error_qmap_pad : 6;
  u32 error_qmap_next_hdr_0 : 1;
  u32 error_qmap_cd : 1;
  u32 error_qmap_mux_id : 8;
  u32 error_qmap_packet_length_with_padding : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_40_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_40_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_41 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_41_s
{
  u32 error_qmap_next_hdr_1 : 1;
  u32 error_qmap_header_type : 7;
  u32 error_qmap_reserved_0 : 7;
  u32 error_qmap_csum_valid : 1;
  u32 error_qmap_error_type : 8;
  u32 error_qmap_error_code : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_41_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_41_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_42 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_42_s
{
  u32 error_qmap_ipsec_sa_index : 8;
  u32 error_qmap_reserved_1 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_42_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_42_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_43 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_43_s
{
  u32 ucp_cmd_id : 16;
  u32 l4_payload_checksum : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_43_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_43_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_44 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_44_s
{
  u32 ucp_cmd_params : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_44_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_44_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_45 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_45_s
{
  u32 bearer_cfg_count : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_45_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_45_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_46 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_46_s
{
  u32 mbim_aggr_sid : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_46_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_46_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_47 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_47_s
{
  u32 l4_pseudo_hdr_checksum : 16;
  u32 ipv4_cs_without_total_len : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_47_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_47_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_48 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_48_s
{
  u32 padding_bytes_cnt : 16;
  u32 ulso_mss : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_48_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_48_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_49 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_49_s
{
  u32 tcp_data_offset : 4;
  u32 original_hdr_size : 9;
  u32 ulso_udp_checksum_zero : 1;
  u32 fin : 1;
  u32 ipv4_vld_checksum : 1;
  u32 pkt_seg_cnt : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_49_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_49_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_50 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_50_s
{
  u32 ttl : 8;
  u32 router_stats_index : 12;
  u32 filter_stats_index : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_50_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_50_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_51 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_51_s
{
  u32 filter_action_params : 5;
  u32 original_src_hdr_len : 8;
  u32 frag_hdr_offset : 9;
  u32 last_hdr_before_l4_offset : 9;
  u32 snapshot_dsa_tagged : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_51_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_51_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_52 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_52_s
{
  u32 last_hdr_before_last_dest_opt_offset : 9;
  u32 syn : 1;
  u32 urg : 1;
  u32 cwr : 1;
  u32 df : 1;
  u32 original_pkt_type : 4;
  u32 hdri_payload_length_includes_padding : 1;
  u32 hdri_payload_length_includes_ethernet : 1;
  u32 hdri_pdding_or_total_length : 1;
  u32 hdri_payload_len_valid : 1;
  u32 hdri_padding_valid : 1;
  u32 hdri_endianess : 1;
  u32 rt_match : 1;
  u32 filter_result_valid : 1;
  u32 push : 1;
  u32 rst : 1;
  u32 pure_ack : 1;
  u32 bearer_context_index_lsb : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_52_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_52_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_53 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_53_s
{
  u32 bearer_context_index_msb : 4;
  u32 snapshot_eth_hdr_offset : 8;
  u32 maci_size : 2;
  u32 sdap_qfi : 6;
  u32 sdap_rqi : 1;
  u32 sdap_rdi : 1;
  u32 rqos_offload_valid : 1;
  u32 ingress_policing_dis : 1;
  u32 rqos_offload_enable : 1;
  u32 prod_dpl_dis : 1;
  u32 frag_hit_2nd : 1;
  u32 frag_filter_aggr_fc : 1;
  u32 router_rule_table_hit : 2;
  u32 filter_rule_table_hit : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_53_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_53_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_54 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_54_s
{
  u32 snapshot_ttl_updated : 1;
  u32 first_pkt_parser_done : 1;
  u32 filter_result : 6;
  u32 bearer_prcs_type : 1;
  u32 filter_ttl_update : 1;
  u32 router_ttl_update : 1;
  u32 snapshot_bypass : 1;
  u32 dcph_cfg_size : 16;
  u32 snapshot_metadata_type : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_54_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_54_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_55 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_55_s
{
  u32 ece : 1;
  u32 udp_with_zero_checksum : 1;
  u32 tsp : 1;
  u32 frag_router_aggr_fc : 1;
  u32 sdap_offset : 6;
  u32 ipsec_post_decaps : 1;
  u32 crypto_workaround : 3;
  u32 first_parse_exception : 1;
  u32 snapshot_payload_offset : 9;
  u32 bearer_id : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_55_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_55_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_56 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_56_s
{
  u32 nat_result_valid : 1;
  u32 hdri_offset_padding_total_length : 8;
  u32 hdri_offset_payload_len : 8;
  u32 hdri_dst_len_ret_no_eth : 9;
  u32 snapshot_l2_length_lsb : 6;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_56_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_56_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_57 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_57_s
{
  u32 snapshot_l2_length_msb : 2;
  u32 snapshot_trlr_length : 8;
  u32 hdri_additional_const_length : 8;
  u32 ipsec_outer_hdr_ecn : 2;
  u32 nat_stats_all_index_lsb : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_57_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_57_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_58 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_58_s
{
  u32 nat_stats_all_index_msb : 4;
  u32 nat_stats_nonfrag_index : 16;
  u32 frag_hit : 1;
  u32 ip_checksum_fix : 1;
  u32 tport_checksum_fix : 1;
  u32 nat_result : 6;
  u32 nat_hit_version : 1;
  u32 crypto_icv_start_offset_lsb : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_58_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_58_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_59 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_59_s
{
  u32 crypto_icv_start_offset_msb : 14;
  u32 crypto_enc_start_offset : 8;
  u32 crypto_auth_start_offset : 8;
  u32 ulso_ipv4_id_mode : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_59_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_59_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_60 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_60_s
{
  u32 crypto_iv_start_offset : 8;
  u32 ipsec_outer_hdr_dscp : 6;
  u32 ipsec_post_encaps : 1;
  u32 qmap_cs_valid_bit : 1;
  u32 ipsec_after_decaps : 1;
  u32 ack : 1;
  u32 ulso_frame_valid : 1;
  u32 crypto_seq_num_lsb_lsb : 13;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_60_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_60_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_61 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_61_s
{
  u32 crypto_seq_num_lsb_msb : 19;
  u32 mtu_exceeded_hpc_check_ip_length_lsb : 13;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_61_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_61_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_62 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_62_s
{
  u32 mtu_exceeded_hpc_check_ip_length_msb : 3;
  u32 crypto_seq_num_msb_lsb : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_62_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_62_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_63 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_63_s
{
  u32 crypto_seq_num_msb_msb : 3;
  u32 drbip_acl_l4_blind_csum : 16;
  u32 original_eth_hdr_len : 5;
  u32 next_round_filter_table_index : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_63_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_63_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_64 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_64_s
{
  u32 snapshot_trnseq_length : 10;
  u32 inc_ipv4_id : 1;
  u32 snapshot_pkt_type : 4;
  u32 prod_sw_cookie_lsb_lsb : 17;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_64_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_64_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_65 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_65_s
{
  u32 prod_sw_cookie_lsb_msb : 15;
  u32 prod_sw_cookie_msb_lsb : 17;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_65_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_65_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_66 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_66_s
{
  u32 prod_sw_cookie_msb_msb : 15;
  u32 snapshot_pad_length : 8;
  u32 eth_hdr_offset : 8;
  u32 mtu_exceeded_hpc_check_en : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_66_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_66_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PKT_CTX_NUM_m_FIELDS_67 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_67_s
{
  u32 hpc_fetch_length : 8;
  u32 snapshot_packet_len : 16;
  u32 ipsec_next_hdr_val : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pkt_ctx_num_m_fields_67_u
{
  struct ipa_hwio_def_ipa_pkt_ctx_num_m_fields_67_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_DEBUG
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RSRC_MNGR_SW_ACCESS_ALLOC_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rsrc_mngr_sw_access_alloc_cfg_s
{
  u32 alloc_rsrc_type : 3;
  u32 reserved0 : 1;
  u32 alloc_rsrc_grp : 4;
  u32 alloc_rsrc_id_curr : 9;
  u32 alloc_list_id : 9;
  u32 alloc_hold : 1;
  u32 alloc_reserved : 1;
  u32 alloc_list_type : 2;
  u32 reserved1 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rsrc_mngr_sw_access_alloc_cfg_u
{
  struct ipa_hwio_def_ipa_rsrc_mngr_sw_access_alloc_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RSRC_MNGR_SW_ACCESS_SRCH_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rsrc_mngr_sw_access_srch_cfg_s
{
  u32 srch_rsrc_type : 3;
  u32 reserved0 : 1;
  u32 srch_rsrc_cnt : 9;
  u32 reserved1 : 3;
  u32 srch_list_id : 9;
  u32 reserved2 : 3;
  u32 srch_list_type : 2;
  u32 reserved3 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rsrc_mngr_sw_access_srch_cfg_u
{
  struct ipa_hwio_def_ipa_rsrc_mngr_sw_access_srch_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RSRC_MNGR_SW_ACCESS_REL_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rsrc_mngr_sw_access_rel_cfg_s
{
  u32 rel_rsrc_type : 3;
  u32 reserved0 : 1;
  u32 rel_rsrc_grp : 4;
  u32 rel_rsrc_id : 9;
  u32 rel_list_id : 9;
  u32 reserved1 : 2;
  u32 rel_list_type : 2;
  u32 reserved2 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rsrc_mngr_sw_access_rel_cfg_u
{
  struct ipa_hwio_def_ipa_rsrc_mngr_sw_access_rel_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RSRC_MNGR_SW_ACCESS_RSRV_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rsrc_mngr_sw_access_rsrv_cfg_s
{
  u32 rsrv_rsrc_type : 3;
  u32 reserved0 : 1;
  u32 rsrv_rsrc_grp : 4;
  u32 rsrv_rsrc_amount : 6;
  u32 reserved1 : 18;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rsrc_mngr_sw_access_rsrv_cfg_u
{
  struct ipa_hwio_def_ipa_rsrc_mngr_sw_access_rsrv_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RSRC_MNGR_SW_ACCESS_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rsrc_mngr_sw_access_cmd_s
{
  u32 alloc_valid : 1;
  u32 srch_valid : 1;
  u32 rel_valid : 1;
  u32 rsrv_valid : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rsrc_mngr_sw_access_cmd_u
{
  struct ipa_hwio_def_ipa_rsrc_mngr_sw_access_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RSRC_MNGR_SW_ACCESS_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rsrc_mngr_sw_access_status_s
{
  u32 alloc_ready : 1;
  u32 srch_ready : 1;
  u32 rel_ready : 1;
  u32 rsrv_ready : 1;
  u32 alloc_rsrc_id_next : 9;
  u32 reserved0 : 3;
  u32 srch_rsrc_id_next : 9;
  u32 reserved1 : 7;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rsrc_mngr_sw_access_status_u
{
  struct ipa_hwio_def_ipa_rsrc_mngr_sw_access_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RSRC_MNGR_DB_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rsrc_mngr_db_cfg_s
{
  u32 rsrc_grp_sel : 4;
  u32 rsrc_type_sel : 3;
  u32 reserved0 : 1;
  u32 rsrc_id_sel : 12;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rsrc_mngr_db_cfg_u
{
  struct ipa_hwio_def_ipa_rsrc_mngr_db_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RSRC_MNGR_DB_RSRC_READ 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rsrc_mngr_db_rsrc_read_s
{
  u32 rsrc_occupied : 1;
  u32 rsrc_next_valid : 1;
  u32 reserved0 : 2;
  u32 rsrc_next_index : 9;
  u32 reserved1 : 19;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rsrc_mngr_db_rsrc_read_u
{
  struct ipa_hwio_def_ipa_rsrc_mngr_db_rsrc_read_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RSRC_MNGR_DB_LIST_READ 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rsrc_mngr_db_list_read_s
{
  u32 rsrc_list_valid : 1;
  u32 rsrc_list_hold : 1;
  u32 reserved0 : 2;
  u32 rsrc_list_head_rsrc : 9;
  u32 rsrc_list_head_cnt : 9;
  u32 rsrc_list_entry_cnt : 9;
  u32 reserved1 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rsrc_mngr_db_list_read_u
{
  struct ipa_hwio_def_ipa_rsrc_mngr_db_list_read_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RSRC_MNGR_CONTEXTS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rsrc_mngr_contexts_s
{
  u32 rsrc_occupied_contexts_bitmap : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rsrc_mngr_contexts_u
{
  struct ipa_hwio_def_ipa_rsrc_mngr_contexts_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RSRC_MNGR_CONTEXTS_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rsrc_mngr_contexts_1_s
{
  u32 rsrc_occupied_contexts_bitmap : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rsrc_mngr_contexts_1_u
{
  struct ipa_hwio_def_ipa_rsrc_mngr_contexts_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_BRESP_DB_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_bresp_db_cfg_s
{
  u32 sel_entry : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_bresp_db_cfg_u
{
  struct ipa_hwio_def_ipa_bresp_db_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_BRESP_DB_DATA 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_bresp_db_data_s
{
  u32 data : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_bresp_db_data_u
{
  struct ipa_hwio_def_ipa_bresp_db_data_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SNOC_MONITORING_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_snoc_monitoring_cfg_s
{
  u32 enable : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_snoc_monitoring_cfg_u
{
  struct ipa_hwio_def_ipa_snoc_monitoring_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PCIE_SNOC_MONITOR_CNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pcie_snoc_monitor_cnt_s
{
  u32 ar_value : 5;
  u32 reserved0 : 1;
  u32 aw_value : 5;
  u32 reserved1 : 1;
  u32 r_value : 5;
  u32 reserved2 : 1;
  u32 w_value : 5;
  u32 reserved3 : 1;
  u32 b_value : 5;
  u32 reserved4 : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pcie_snoc_monitor_cnt_u
{
  struct ipa_hwio_def_ipa_pcie_snoc_monitor_cnt_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DDR_SNOC_MONITOR_CNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ddr_snoc_monitor_cnt_s
{
  u32 ar_value : 5;
  u32 reserved0 : 1;
  u32 aw_value : 5;
  u32 reserved1 : 1;
  u32 r_value : 5;
  u32 reserved2 : 1;
  u32 w_value : 5;
  u32 reserved3 : 1;
  u32 b_value : 5;
  u32 reserved4 : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ddr_snoc_monitor_cnt_u
{
  struct ipa_hwio_def_ipa_ddr_snoc_monitor_cnt_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_GSI_SNOC_MONITOR_CNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_gsi_snoc_monitor_cnt_s
{
  u32 ar_value : 5;
  u32 reserved0 : 1;
  u32 aw_value : 5;
  u32 reserved1 : 1;
  u32 r_value : 5;
  u32 reserved2 : 1;
  u32 w_value : 5;
  u32 reserved3 : 1;
  u32 b_value : 5;
  u32 reserved4 : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_gsi_snoc_monitor_cnt_u
{
  struct ipa_hwio_def_ipa_gsi_snoc_monitor_cnt_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DEBUG_DATA 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_debug_data_s
{
  u32 debug_data : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_debug_data_u
{
  struct ipa_hwio_def_ipa_debug_data_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TESTBUS_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_testbus_sel_s
{
  u32 testbus_en : 1;
  u32 reserved0 : 3;
  u32 external_block_select : 8;
  u32 internal_block_select : 8;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_testbus_sel_u
{
  struct ipa_hwio_def_ipa_testbus_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HW_EVENTS_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hw_events_cfg_s
{
  u32 hw_events_select : 4;
  u32 rx_events_pipe_select : 8;
  u32 src_pipe_en : 1;
  u32 src_pipe : 8;
  u32 fifo_threshold : 8;
  u32 reserved0 : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hw_events_cfg_u
{
  struct ipa_hwio_def_ipa_hw_events_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HW_EVENTS_CFG_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hw_events_cfg_1_s
{
  u32 pkt_size_threshold : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hw_events_cfg_1_u
{
  struct ipa_hwio_def_ipa_hw_events_cfg_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CONS_LOG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_cons_log_s
{
  u32 reserved0 : 1;
  u32 log_en : 1;
  u32 reserved1 : 2;
  u32 log_pipe : 8;
  u32 log_length : 8;
  u32 log_reduction_en : 1;
  u32 log_dpl_l2_remove_en : 1;
  u32 reserved2 : 10;
};

/* Union definition of register */
union ipa_hwio_def_ipa_cons_log_u
{
  struct ipa_hwio_def_ipa_cons_log_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_CMD_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_cmd_addr_s
{
  u32 start_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_cmd_addr_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_cmd_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_CMD_ADDR_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_cmd_addr_msb_s
{
  u32 start_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_cmd_addr_msb_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_cmd_addr_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_CMD_WRITE_PTR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_cmd_write_ptr_s
{
  u32 writr_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_cmd_write_ptr_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_cmd_write_ptr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_CMD_WRITE_PTR_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_cmd_write_ptr_msb_s
{
  u32 writr_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_cmd_write_ptr_msb_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_cmd_write_ptr_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_CMD_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_cmd_cfg_s
{
  u32 size : 16;
  u32 enable : 1;
  u32 skip_ddr_dma : 1;
  u32 tpdm_ts_en : 1;
  u32 rst_fifo : 1;
  u32 wr_wrap_not_en : 1;
  u32 reserved0 : 11;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_cmd_cfg_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_cmd_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_CMD_RAM_PTR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_cmd_ram_ptr_s
{
  u32 read_ptr : 14;
  u32 trigger_event : 1;
  u32 reserved0 : 1;
  u32 write_ptr : 14;
  u32 full : 1;
  u32 snif_ram_reached_end : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_cmd_ram_ptr_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_cmd_ram_ptr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FORCE_ALLOW_IDLE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_force_allow_idle_s
{
  u32 val : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_force_allow_idle_u
{
  struct ipa_hwio_def_ipa_force_allow_idle_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_CMD_NOC_MASTER_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_cmd_noc_master_sel_s
{
  u32 noc_port_sel : 1;
  u32 qmb_rd_en : 1;
  u32 qmb_wr_en : 1;
  u32 gsi_rd_en : 1;
  u32 gsi_wr_en : 1;
  u32 uc_rd_en : 1;
  u32 uc_wr_en : 1;
  u32 qmb_resp_en : 1;
  u32 gsi_resp_en : 1;
  u32 uc_resp_en : 1;
  u32 reserved0 : 22;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_cmd_noc_master_sel_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_cmd_noc_master_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_BRESP_DB_DATA_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_bresp_db_data_1_s
{
  u32 data : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_bresp_db_data_1_u
{
  struct ipa_hwio_def_ipa_bresp_db_data_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_BRESP_DB_DATA_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_bresp_db_data_2_s
{
  u32 data : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_bresp_db_data_2_u
{
  struct ipa_hwio_def_ipa_bresp_db_data_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_BUS_MASTER_LEGACY_BURSTS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_bus_master_legacy_bursts_s
{
  u32 cross_128b_fetcher_imm_cmd : 1;
  u32 cross_128b_fetcher_dmar : 1;
  u32 cross_128b_dfetcher_ddmar : 1;
  u32 cross_128b_drbip_ddmar : 1;
  u32 cross_128b_hdri : 1;
  u32 cross_128b_rqos : 1;
  u32 cross_128b_uc_qmb : 1;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_bus_master_legacy_bursts_u
{
  struct ipa_hwio_def_ipa_bus_master_legacy_bursts_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CONS_LOG_THRESHOLD_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_cons_log_threshold_cfg_s
{
  u32 threshold : 12;
  u32 reserved0 : 4;
  u32 threshold_en : 1;
  u32 reserved1 : 3;
  u32 threshold_timer_gran_sel : 4;
  u32 reserved2 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_cons_log_threshold_cfg_u
{
  struct ipa_hwio_def_ipa_cons_log_threshold_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_LOG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_log_s
{
  u32 reserved0 : 1;
  u32 log_en : 1;
  u32 reserved1 : 2;
  u32 log_pipe : 8;
  u32 log_length : 8;
  u32 log_reduction_en : 1;
  u32 log_dpl_l2_remove_en : 1;
  u32 reserved2 : 10;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_log_u
{
  struct ipa_hwio_def_ipa_prod_log_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_LOG_THRESHOLD_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_log_threshold_cfg_s
{
  u32 threshold : 12;
  u32 reserved0 : 4;
  u32 threshold_en : 1;
  u32 reserved1 : 3;
  u32 threshold_timer_gran_sel : 4;
  u32 reserved2 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_log_threshold_cfg_u
{
  struct ipa_hwio_def_ipa_prod_log_threshold_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CACHE_CLEAR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_cache_clear_s
{
  u32 vmidmt_ahb_cache : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_cache_clear_u
{
  struct ipa_hwio_def_ipa_cache_clear_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_0_0_ACCS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_0_0_accs_s
{
  u32 acc_0 : 4;
  u32 acc_1 : 4;
  u32 acc_2 : 4;
  u32 acc_3 : 4;
  u32 acc_4 : 4;
  u32 acc_5 : 4;
  u32 acc_6 : 4;
  u32 acc_7 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_0_0_accs_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_0_0_accs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_0_1_ACCS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_0_1_accs_s
{
  u32 acc_8 : 4;
  u32 acc_9 : 4;
  u32 acc_10 : 4;
  u32 acc_11 : 4;
  u32 acc_12 : 4;
  u32 acc_13 : 4;
  u32 acc_14 : 4;
  u32 acc_15 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_0_1_accs_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_0_1_accs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_0_2_ACCS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_0_2_accs_s
{
  u32 acc_16 : 4;
  u32 acc_17 : 4;
  u32 acc_18 : 4;
  u32 acc_19 : 4;
  u32 acc_20 : 4;
  u32 acc_21 : 4;
  u32 acc_22 : 4;
  u32 acc_23 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_0_2_accs_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_0_2_accs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_0_3_ACCS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_0_3_accs_s
{
  u32 acc_24 : 4;
  u32 acc_25 : 4;
  u32 acc_26 : 4;
  u32 acc_27 : 4;
  u32 acc_28 : 4;
  u32 acc_29 : 4;
  u32 acc_30 : 4;
  u32 acc_31 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_0_3_accs_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_0_3_accs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_1_0_ACCS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_1_0_accs_s
{
  u32 acc_0 : 4;
  u32 acc_1 : 4;
  u32 acc_2 : 4;
  u32 acc_3 : 4;
  u32 acc_4 : 4;
  u32 acc_5 : 4;
  u32 acc_6 : 4;
  u32 acc_7 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_1_0_accs_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_1_0_accs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_1_1_ACCS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_1_1_accs_s
{
  u32 acc_8 : 4;
  u32 acc_9 : 4;
  u32 acc_10 : 4;
  u32 acc_11 : 4;
  u32 acc_12 : 4;
  u32 acc_13 : 4;
  u32 acc_14 : 4;
  u32 acc_15 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_1_1_accs_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_1_1_accs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_1_2_ACCS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_1_2_accs_s
{
  u32 acc_16 : 4;
  u32 acc_17 : 4;
  u32 acc_18 : 4;
  u32 acc_19 : 4;
  u32 acc_20 : 4;
  u32 acc_21 : 4;
  u32 acc_22 : 4;
  u32 acc_23 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_1_2_accs_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_1_2_accs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_1_3_ACCS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_1_3_accs_s
{
  u32 acc_24 : 4;
  u32 acc_25 : 4;
  u32 acc_26 : 4;
  u32 acc_27 : 4;
  u32 acc_28 : 4;
  u32 acc_29 : 4;
  u32 acc_30 : 4;
  u32 acc_31 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_1_3_accs_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_1_3_accs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_0_0_FLAGS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_0_0_flags_s
{
  u32 flag_0 : 3;
  u32 reserved0 : 1;
  u32 flag_1 : 4;
  u32 flag_2 : 4;
  u32 flag_3 : 3;
  u32 reserved1 : 1;
  u32 flag_4 : 3;
  u32 reserved2 : 1;
  u32 flag_5 : 3;
  u32 reserved3 : 1;
  u32 flag_6 : 3;
  u32 reserved4 : 1;
  u32 flag_7 : 3;
  u32 reserved5 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_0_0_flags_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_0_0_flags_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_0_1_FLAGS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_0_1_flags_s
{
  u32 flag_8 : 3;
  u32 reserved0 : 1;
  u32 flag_9 : 4;
  u32 flag_10 : 4;
  u32 flag_11 : 3;
  u32 reserved1 : 1;
  u32 flag_12 : 3;
  u32 reserved2 : 1;
  u32 flag_13 : 3;
  u32 reserved3 : 1;
  u32 flag_14 : 3;
  u32 reserved4 : 1;
  u32 flag_15 : 3;
  u32 reserved5 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_0_1_flags_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_0_1_flags_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_0_2_FLAGS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_0_2_flags_s
{
  u32 flag_16 : 3;
  u32 reserved0 : 1;
  u32 flag_17 : 4;
  u32 flag_18 : 4;
  u32 flag_19 : 3;
  u32 reserved1 : 1;
  u32 flag_20 : 3;
  u32 reserved2 : 1;
  u32 flag_21 : 3;
  u32 reserved3 : 1;
  u32 flag_22 : 3;
  u32 reserved4 : 1;
  u32 flag_23 : 3;
  u32 reserved5 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_0_2_flags_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_0_2_flags_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_0_3_FLAGS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_0_3_flags_s
{
  u32 flag_24 : 3;
  u32 reserved0 : 1;
  u32 flag_25 : 4;
  u32 flag_26 : 4;
  u32 flag_27 : 3;
  u32 reserved1 : 1;
  u32 flag_28 : 3;
  u32 reserved2 : 1;
  u32 flag_29 : 3;
  u32 reserved3 : 1;
  u32 flag_30 : 3;
  u32 reserved4 : 1;
  u32 flag_31 : 3;
  u32 reserved5 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_0_3_flags_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_0_3_flags_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_1_0_FLAGS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_1_0_flags_s
{
  u32 flag_0 : 3;
  u32 reserved0 : 1;
  u32 flag_1 : 4;
  u32 flag_2 : 4;
  u32 flag_3 : 3;
  u32 reserved1 : 1;
  u32 flag_4 : 3;
  u32 reserved2 : 1;
  u32 flag_5 : 3;
  u32 reserved3 : 1;
  u32 flag_6 : 3;
  u32 reserved4 : 1;
  u32 flag_7 : 3;
  u32 reserved5 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_1_0_flags_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_1_0_flags_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_1_1_FLAGS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_1_1_flags_s
{
  u32 flag_8 : 3;
  u32 reserved0 : 1;
  u32 flag_9 : 4;
  u32 flag_10 : 4;
  u32 flag_11 : 3;
  u32 reserved1 : 1;
  u32 flag_12 : 3;
  u32 reserved2 : 1;
  u32 flag_13 : 3;
  u32 reserved3 : 1;
  u32 flag_14 : 3;
  u32 reserved4 : 1;
  u32 flag_15 : 3;
  u32 reserved5 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_1_1_flags_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_1_1_flags_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_1_2_FLAGS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_1_2_flags_s
{
  u32 flag_16 : 3;
  u32 reserved0 : 1;
  u32 flag_17 : 4;
  u32 flag_18 : 4;
  u32 flag_19 : 3;
  u32 reserved1 : 1;
  u32 flag_20 : 3;
  u32 reserved2 : 1;
  u32 flag_21 : 3;
  u32 reserved3 : 1;
  u32 flag_22 : 3;
  u32 reserved4 : 1;
  u32 flag_23 : 3;
  u32 reserved5 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_1_2_flags_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_1_2_flags_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DYNAMIC_SEQ_1_3_FLAGS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dynamic_seq_1_3_flags_s
{
  u32 flag_24 : 3;
  u32 reserved0 : 1;
  u32 flag_25 : 4;
  u32 flag_26 : 4;
  u32 flag_27 : 3;
  u32 reserved1 : 1;
  u32 flag_28 : 3;
  u32 reserved2 : 1;
  u32 flag_29 : 3;
  u32 reserved3 : 1;
  u32 flag_30 : 3;
  u32 reserved4 : 1;
  u32 flag_31 : 3;
  u32 reserved5 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dynamic_seq_1_3_flags_u
{
  struct ipa_hwio_def_ipa_hps_dynamic_seq_1_3_flags_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_ACKQ_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_ackq_cmd_s
{
  u32 write_cmd : 1;
  u32 pop_cmd : 1;
  u32 release_rd_cmd : 1;
  u32 release_wr_cmd : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_ackq_cmd_u
{
  struct ipa_hwio_def_ipa_rx_ackq_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_ACKQ_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_ackq_cfg_s
{
  u32 block_rd_req : 1;
  u32 block_wr : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_ackq_cfg_u
{
  struct ipa_hwio_def_ipa_rx_ackq_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_ACKQ_DATA_WR_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_ackq_data_wr_0_s
{
  u32 ack_value1 : 16;
  u32 ack_value2 : 8;
  u32 ack_value1_type : 1;
  u32 reserved0 : 7;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_ackq_data_wr_0_u
{
  struct ipa_hwio_def_ipa_rx_ackq_data_wr_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_ACKQ_DATA_RD_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_ackq_data_rd_0_s
{
  u32 ack_value1 : 16;
  u32 ack_value2 : 8;
  u32 ack_value1_type : 1;
  u32 reserved0 : 7;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_ackq_data_rd_0_u
{
  struct ipa_hwio_def_ipa_rx_ackq_data_rd_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_ACKQ_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_ackq_status_s
{
  u32 status : 1;
  u32 ackq_empty : 1;
  u32 ackq_full : 1;
  u32 reserved0 : 1;
  u32 ackq_count : 4;
  u32 ackq_depth : 4;
  u32 block_rd_ack : 1;
  u32 reserved1 : 19;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_ackq_status_u
{
  struct ipa_hwio_def_ipa_rx_ackq_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_ACKQ_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ackq_cmd_s
{
  u32 write_cmd : 1;
  u32 pop_cmd : 1;
  u32 release_rd_cmd : 1;
  u32 release_wr_cmd : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ackq_cmd_u
{
  struct ipa_hwio_def_ipa_uc_ackq_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_ACKQ_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ackq_cfg_s
{
  u32 block_rd : 1;
  u32 block_wr : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ackq_cfg_u
{
  struct ipa_hwio_def_ipa_uc_ackq_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_ACKQ_DATA_WR_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ackq_data_wr_0_s
{
  u32 ack_value1 : 16;
  u32 ack_value2 : 8;
  u32 ack_value1_type : 1;
  u32 reserved0 : 7;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ackq_data_wr_0_u
{
  struct ipa_hwio_def_ipa_uc_ackq_data_wr_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_ACKQ_DATA_RD_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ackq_data_rd_0_s
{
  u32 ack_value1 : 16;
  u32 ack_value2 : 8;
  u32 ack_value1_type : 1;
  u32 reserved0 : 7;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ackq_data_rd_0_u
{
  struct ipa_hwio_def_ipa_uc_ackq_data_rd_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_ACKQ_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_ackq_status_s
{
  u32 status : 1;
  u32 ackq_empty : 1;
  u32 ackq_full : 1;
  u32 reserved0 : 1;
  u32 ackq_count : 5;
  u32 reserved1 : 3;
  u32 ackq_depth : 5;
  u32 reserved2 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_ackq_status_u
{
  struct ipa_hwio_def_ipa_uc_ackq_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_cmd_s
{
  u32 write_cmd : 1;
  u32 pop_cmd : 1;
  u32 cmd_client : 3;
  u32 rd_req : 1;
  u32 reserved0 : 26;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_cmd_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_RELEASE_WR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_release_wr_s
{
  u32 release_wr_cmd : 7;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_release_wr_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_release_wr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_RELEASE_RD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_release_rd_s
{
  u32 release_rd_cmd : 7;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_release_rd_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_release_rd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_CFG_WR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_cfg_wr_s
{
  u32 block_wr : 7;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_cfg_wr_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_cfg_wr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_CFG_RD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_cfg_rd_s
{
  u32 block_rd : 7;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_cfg_rd_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_cfg_rd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_DATA_WR_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_data_wr_0_s
{
  u32 cmdq_packet_len_f : 16;
  u32 cmdq_dest_len_f : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_data_wr_0_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_data_wr_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_DATA_WR_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_data_wr_1_s
{
  u32 cmdq_src_pipe_f : 8;
  u32 cmdq_order_f : 2;
  u32 cmdq_flags_f : 6;
  u32 cmdq_opcode_f : 8;
  u32 cmdq_metadata_f : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_data_wr_1_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_data_wr_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_DATA_WR_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_data_wr_2_s
{
  u32 cmdq_stats_disable_f : 1;
  u32 cmdq_addr_lsb_f : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_data_wr_2_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_data_wr_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_DATA_WR_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_data_wr_3_s
{
  u32 cmdq_addr_msb_f : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_data_wr_3_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_data_wr_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_DATA_RD_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_data_rd_0_s
{
  u32 cmdq_packet_len_f : 16;
  u32 cmdq_dest_len_f : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_data_rd_0_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_data_rd_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_DATA_RD_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_data_rd_1_s
{
  u32 cmdq_src_pipe_f : 8;
  u32 cmdq_order_f : 2;
  u32 cmdq_flags_f : 6;
  u32 cmdq_opcode_f : 8;
  u32 cmdq_metadata_f : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_data_rd_1_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_data_rd_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_DATA_RD_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_data_rd_2_s
{
  u32 cmdq_stats_disable_f : 1;
  u32 cmdq_addr_lsb_f : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_data_rd_2_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_data_rd_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_DATA_RD_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_data_rd_3_s
{
  u32 cmdq_addr_msb_f : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_data_rd_3_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_data_rd_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_status_s
{
  u32 status : 1;
  u32 cmdq_full : 1;
  u32 cmdq_depth : 7;
  u32 reserved0 : 23;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_status_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_STATUS_EMPTY 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_status_empty_s
{
  u32 cmdq_empty : 7;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_status_empty_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_status_empty_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_SNP 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_snp_s
{
  u32 snp_last : 1;
  u32 snp_write : 1;
  u32 snp_valid : 1;
  u32 snp_next_is_valid : 1;
  u32 snp_next : 5;
  u32 snp_head : 5;
  u32 snp_addr : 5;
  u32 reserved0 : 13;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_snp_u
{
  struct ipa_hwio_def_ipa_rx_hps_snp_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RX_HPS_CMDQ_COUNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rx_hps_cmdq_count_s
{
  u32 fifo_count : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rx_hps_cmdq_count_u
{
  struct ipa_hwio_def_ipa_rx_hps_cmdq_count_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DPS_CMDQ_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dps_cmdq_cmd_s
{
  u32 write_cmd : 1;
  u32 pop_cmd : 1;
  u32 rd_req : 1;
  u32 reserved0 : 1;
  u32 cmd_client : 8;
  u32 reserved1 : 20;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dps_cmdq_cmd_u
{
  struct ipa_hwio_def_ipa_hps_dps_cmdq_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DPS_CMDQ_DATA_WR_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dps_cmdq_data_wr_0_s
{
  u32 cmdq_ctx_id_f : 6;
  u32 cmdq_src_id_f : 8;
  u32 cmdq_src_pipe_f : 8;
  u32 reserved0 : 10;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dps_cmdq_data_wr_0_u
{
  struct ipa_hwio_def_ipa_hps_dps_cmdq_data_wr_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DPS_CMDQ_DATA_RD_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dps_cmdq_data_rd_0_s
{
  u32 cmdq_ctx_id_f : 6;
  u32 cmdq_src_id_f : 8;
  u32 cmdq_src_pipe_f : 8;
  u32 reserved0 : 10;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dps_cmdq_data_rd_0_u
{
  struct ipa_hwio_def_ipa_hps_dps_cmdq_data_rd_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DPS_CMDQ_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dps_cmdq_status_s
{
  u32 status : 1;
  u32 cmdq_full : 1;
  u32 reserved0 : 14;
  u32 cmdq_depth : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dps_cmdq_status_u
{
  struct ipa_hwio_def_ipa_hps_dps_cmdq_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DPS_SNP 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dps_snp_s
{
  u32 snp_last : 1;
  u32 snp_write : 1;
  u32 snp_valid : 1;
  u32 snp_next_is_valid : 1;
  u32 snp_next : 8;
  u32 snp_head : 8;
  u32 snp_addr : 8;
  u32 reserved0 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dps_snp_u
{
  struct ipa_hwio_def_ipa_hps_dps_snp_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DPS_CMDQ_COUNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dps_cmdq_count_s
{
  u32 fifo_count : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dps_cmdq_count_u
{
  struct ipa_hwio_def_ipa_hps_dps_cmdq_count_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DPS_CMDQ_RELEASE_WR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dps_cmdq_release_wr_n_s
{
  u32 release_wr_cmd : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dps_cmdq_release_wr_n_u
{
  struct ipa_hwio_def_ipa_hps_dps_cmdq_release_wr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DPS_CMDQ_RELEASE_RD_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dps_cmdq_release_rd_n_s
{
  u32 release_rd_cmd : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dps_cmdq_release_rd_n_u
{
  struct ipa_hwio_def_ipa_hps_dps_cmdq_release_rd_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DPS_CMDQ_CFG_WR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dps_cmdq_cfg_wr_n_s
{
  u32 block_wr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dps_cmdq_cfg_wr_n_u
{
  struct ipa_hwio_def_ipa_hps_dps_cmdq_cfg_wr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DPS_CMDQ_CFG_RD_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dps_cmdq_cfg_rd_n_s
{
  u32 block_rd : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dps_cmdq_cfg_rd_n_u
{
  struct ipa_hwio_def_ipa_hps_dps_cmdq_cfg_rd_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_DPS_CMDQ_STATUS_EMPTY_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_dps_cmdq_status_empty_n_s
{
  u32 cmdq_empty : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_dps_cmdq_status_empty_n_u
{
  struct ipa_hwio_def_ipa_hps_dps_cmdq_status_empty_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_TX_CMDQ_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_tx_cmdq_cmd_s
{
  u32 write_cmd : 1;
  u32 pop_cmd : 1;
  u32 rd_req : 1;
  u32 cmd_client : 4;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_tx_cmdq_cmd_u
{
  struct ipa_hwio_def_ipa_dps_tx_cmdq_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_TX_CMDQ_RELEASE_WR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_tx_cmdq_release_wr_s
{
  u32 release_wr_cmd : 12;
  u32 reserved0 : 20;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_tx_cmdq_release_wr_u
{
  struct ipa_hwio_def_ipa_dps_tx_cmdq_release_wr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_TX_CMDQ_RELEASE_RD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_tx_cmdq_release_rd_s
{
  u32 release_rd_cmd : 12;
  u32 reserved0 : 20;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_tx_cmdq_release_rd_u
{
  struct ipa_hwio_def_ipa_dps_tx_cmdq_release_rd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_TX_CMDQ_CFG_WR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_tx_cmdq_cfg_wr_s
{
  u32 block_wr : 12;
  u32 reserved0 : 20;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_tx_cmdq_cfg_wr_u
{
  struct ipa_hwio_def_ipa_dps_tx_cmdq_cfg_wr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_TX_CMDQ_CFG_RD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_tx_cmdq_cfg_rd_s
{
  u32 block_rd : 12;
  u32 reserved0 : 20;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_tx_cmdq_cfg_rd_u
{
  struct ipa_hwio_def_ipa_dps_tx_cmdq_cfg_rd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_TX_CMDQ_DATA_WR_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_tx_cmdq_data_wr_0_s
{
  u32 seg_ctx_id_f : 2;
  u32 reserved0 : 6;
  u32 cmdq_ctx_id_f : 6;
  u32 reserved1 : 2;
  u32 cmdq_src_pipe_f : 8;
  u32 cmdq_opcode_f : 4;
  u32 reserved2 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_tx_cmdq_data_wr_0_u
{
  struct ipa_hwio_def_ipa_dps_tx_cmdq_data_wr_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_TX_CMDQ_DATA_RD_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_tx_cmdq_data_rd_0_s
{
  u32 seg_ctx_id_f : 2;
  u32 reserved0 : 6;
  u32 cmdq_ctx_id_f : 6;
  u32 reserved1 : 2;
  u32 cmdq_src_pipe_f : 8;
  u32 cmdq_opcode_f : 4;
  u32 reserved2 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_tx_cmdq_data_rd_0_u
{
  struct ipa_hwio_def_ipa_dps_tx_cmdq_data_rd_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_TX_CMDQ_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_tx_cmdq_status_s
{
  u32 status : 1;
  u32 cmdq_full : 1;
  u32 reserved0 : 14;
  u32 cmdq_depth : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_tx_cmdq_status_u
{
  struct ipa_hwio_def_ipa_dps_tx_cmdq_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_TX_CMDQ_STATUS_EMPTY 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_tx_cmdq_status_empty_s
{
  u32 cmdq_empty : 12;
  u32 reserved0 : 20;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_tx_cmdq_status_empty_u
{
  struct ipa_hwio_def_ipa_dps_tx_cmdq_status_empty_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_TX_SNP 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_tx_snp_s
{
  u32 snp_last : 1;
  u32 snp_write : 1;
  u32 snp_valid : 1;
  u32 snp_next_is_valid : 1;
  u32 snp_next : 8;
  u32 snp_head : 8;
  u32 snp_addr : 8;
  u32 reserved0 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_tx_snp_u
{
  struct ipa_hwio_def_ipa_dps_tx_snp_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_TX_CMDQ_COUNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_tx_cmdq_count_s
{
  u32 fifo_count : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_tx_cmdq_count_u
{
  struct ipa_hwio_def_ipa_dps_tx_cmdq_count_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_cfg_n_s
{
  u32 snif_el_enable : 1;
  u32 snif_el_mode : 2;
  u32 reserved0 : 1;
  u32 snif_el_select : 6;
  u32 reserved1 : 2;
  u32 snif_el_sel_trig_bitmap : 3;
  u32 reserved2 : 17;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_cfg_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_COMP_VAL_0_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_comp_val_0_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_comp_val_0_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_comp_val_0_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_COMP_VAL_1_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_comp_val_1_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_comp_val_1_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_comp_val_1_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_COMP_VAL_2_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_comp_val_2_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_comp_val_2_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_comp_val_2_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_COMP_VAL_3_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_comp_val_3_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_comp_val_3_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_comp_val_3_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_MASK_VAL_0_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_mask_val_0_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_mask_val_0_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_mask_val_0_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_MASK_VAL_1_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_mask_val_1_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_mask_val_1_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_mask_val_1_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_MASK_VAL_2_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_mask_val_2_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_mask_val_2_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_mask_val_2_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_MASK_VAL_3_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_mask_val_3_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_mask_val_3_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_mask_val_3_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_TRIG_VAL_0_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_val_0_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_val_0_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_val_0_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_TRIG_VAL_1_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_val_1_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_val_1_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_val_1_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_TRIG_VAL_2_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_val_2_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_val_2_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_val_2_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_TRIG_VAL_3_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_val_3_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_val_3_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_val_3_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_TRIG_MASK_0_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_mask_0_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_mask_0_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_mask_0_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_TRIG_MASK_1_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_mask_1_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_mask_1_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_mask_1_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_TRIG_MASK_2_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_mask_2_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_mask_2_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_mask_2_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_EL_TRIG_MASK_3_CLI_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_mask_3_cli_n_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_mask_3_cli_n_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_el_trig_mask_3_cli_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_SNIF_PERF_CNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_snif_perf_cnt_s
{
  u32 counter : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_snif_perf_cnt_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_snif_perf_cnt_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ACKMNGR_CMDQ_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ackmngr_cmdq_cmd_s
{
  u32 write_cmd : 1;
  u32 pop_cmd : 1;
  u32 cmd_client : 8;
  u32 rd_req : 1;
  u32 reserved0 : 21;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ackmngr_cmdq_cmd_u
{
  struct ipa_hwio_def_ipa_ackmngr_cmdq_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ACKMNGR_CMDQ_DATA_RD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ackmngr_cmdq_data_rd_s
{
  u32 cmdq_src_id : 8;
  u32 cmdq_length : 16;
  u32 cmdq_origin : 1;
  u32 cmdq_sent : 1;
  u32 cmdq_src_id_valid : 1;
  u32 cmdq_error : 1;
  u32 cmdq_user_data : 3;
  u32 reserved0 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ackmngr_cmdq_data_rd_u
{
  struct ipa_hwio_def_ipa_ackmngr_cmdq_data_rd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ACKMNGR_CMDQ_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ackmngr_cmdq_status_s
{
  u32 status : 1;
  u32 cmdq_full : 1;
  u32 reserved0 : 14;
  u32 cmdq_depth : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ackmngr_cmdq_status_u
{
  struct ipa_hwio_def_ipa_ackmngr_cmdq_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ACKMNGR_CMDQ_STATUS_EMPTY_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ackmngr_cmdq_status_empty_n_s
{
  u32 cmdq_empty : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ackmngr_cmdq_status_empty_n_u
{
  struct ipa_hwio_def_ipa_ackmngr_cmdq_status_empty_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ACKMNGR_CMDQ_COUNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ackmngr_cmdq_count_s
{
  u32 fifo_count : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ackmngr_cmdq_count_u
{
  struct ipa_hwio_def_ipa_ackmngr_cmdq_count_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_GSI_FIFO_STATUS_CTRL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_gsi_fifo_status_ctrl_s
{
  u32 ipa_gsi_fifo_status_port_sel : 8;
  u32 ipa_gsi_fifo_status_en : 1;
  u32 reserved0 : 23;
};

/* Union definition of register */
union ipa_hwio_def_ipa_gsi_fifo_status_ctrl_u
{
  struct ipa_hwio_def_ipa_gsi_fifo_status_ctrl_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_GSI_TLV_FIFO_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_gsi_tlv_fifo_status_s
{
  u32 fifo_wr_ptr : 8;
  u32 fifo_rd_ptr : 8;
  u32 fifo_rd_pub_ptr : 8;
  u32 fifo_empty : 1;
  u32 fifo_empty_pub : 1;
  u32 fifo_almost_full : 1;
  u32 fifo_full : 1;
  u32 fifo_almost_full_pub : 1;
  u32 fifo_full_pub : 1;
  u32 fifo_head_is_bubble : 1;
  u32 reserved0 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_gsi_tlv_fifo_status_u
{
  struct ipa_hwio_def_ipa_gsi_tlv_fifo_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_GSI_AOS_FIFO_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_gsi_aos_fifo_status_s
{
  u32 fifo_wr_ptr : 8;
  u32 fifo_rd_ptr : 8;
  u32 fifo_rd_pub_ptr : 8;
  u32 fifo_empty : 1;
  u32 fifo_empty_pub : 1;
  u32 fifo_almost_full : 1;
  u32 fifo_full : 1;
  u32 fifo_almost_full_pub : 1;
  u32 fifo_full_pub : 1;
  u32 fifo_head_is_bubble : 1;
  u32 reserved0 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_gsi_aos_fifo_status_u
{
  struct ipa_hwio_def_ipa_gsi_aos_fifo_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_GSI_CONS_BYTES_TLV 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_gsi_cons_bytes_tlv_s
{
  u32 cons_bytes : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_gsi_cons_bytes_tlv_u
{
  struct ipa_hwio_def_ipa_endp_gsi_cons_bytes_tlv_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_GSI_CONS_BYTES_AOS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_gsi_cons_bytes_aos_s
{
  u32 cons_bytes : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_gsi_cons_bytes_aos_u
{
  struct ipa_hwio_def_ipa_endp_gsi_cons_bytes_aos_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOG_BUF_HW_GEN_RAM_OFFSET 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_log_buf_hw_gen_ram_offset_s
{
  u32 ram_region_baddr : 20;
  u32 ram_region_size : 4;
  u32 reserved0 : 7;
  u32 enable : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_log_buf_hw_gen_ram_offset_u
{
  struct ipa_hwio_def_ipa_log_buf_hw_gen_ram_offset_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_DYNAMIC_SEQ_0_ACCS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_dynamic_seq_0_accs_s
{
  u32 acc_0 : 3;
  u32 acc_1 : 3;
  u32 reserved0 : 26;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_dynamic_seq_0_accs_u
{
  struct ipa_hwio_def_ipa_dps_dynamic_seq_0_accs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_DYNAMIC_SEQ_0_FLAGS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_dynamic_seq_0_flags_s
{
  u32 flag_0 : 2;
  u32 flag_1 : 2;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_dynamic_seq_0_flags_u
{
  struct ipa_hwio_def_ipa_dps_dynamic_seq_0_flags_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_DYNAMIC_SEQ_1_ACCS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_dynamic_seq_1_accs_s
{
  u32 acc_0 : 3;
  u32 acc_1 : 3;
  u32 reserved0 : 26;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_dynamic_seq_1_accs_u
{
  struct ipa_hwio_def_ipa_dps_dynamic_seq_1_accs_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_DYNAMIC_SEQ_1_FLAGS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_dynamic_seq_1_flags_s
{
  u32 flag_0 : 2;
  u32 flag_1 : 2;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_dynamic_seq_1_flags_u
{
  struct ipa_hwio_def_ipa_dps_dynamic_seq_1_flags_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_RX_HND_CMDQ_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_cmd_s
{
  u32 write_cmd : 1;
  u32 pop_cmd : 1;
  u32 release_rd_cmd : 1;
  u32 release_wr_cmd : 1;
  u32 release_rd_pkt : 1;
  u32 release_wr_pkt : 1;
  u32 release_rd_pkt_enhanced : 1;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_rx_hnd_cmdq_cmd_u
{
  struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_RX_HND_CMDQ_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_cfg_s
{
  u32 block_rd : 1;
  u32 block_wr : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_rx_hnd_cmdq_cfg_u
{
  struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_RX_HND_CMDQ_DATA_WR_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_wr_0_s
{
  u32 cmdq_packet_len_f : 16;
  u32 cmdq_src_len_f : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_wr_0_u
{
  struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_wr_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_RX_HND_CMDQ_DATA_WR_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_wr_1_s
{
  u32 cmdq_src_pipe_f : 8;
  u32 cmdq_order_f : 2;
  u32 cmdq_flags_f : 6;
  u32 cmdq_opcode_f : 8;
  u32 cmdq_metadata_f : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_wr_1_u
{
  struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_wr_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_RX_HND_CMDQ_DATA_WR_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_wr_2_s
{
  u32 cmdq_stats_disable_f : 1;
  u32 cmdq_addr_lsb_f : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_wr_2_u
{
  struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_wr_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_RX_HND_CMDQ_DATA_WR_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_wr_3_s
{
  u32 cmdq_addr_msb_f : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_wr_3_u
{
  struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_wr_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_RX_HND_CMDQ_DATA_RD_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_rd_0_s
{
  u32 cmdq_packet_len_f : 16;
  u32 cmdq_src_len_f : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_rd_0_u
{
  struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_rd_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_RX_HND_CMDQ_DATA_RD_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_rd_1_s
{
  u32 cmdq_src_pipe_f : 8;
  u32 cmdq_order_f : 2;
  u32 cmdq_flags_f : 6;
  u32 cmdq_opcode_f : 8;
  u32 cmdq_metadata_f : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_rd_1_u
{
  struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_rd_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_RX_HND_CMDQ_DATA_RD_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_rd_2_s
{
  u32 cmdq_stats_disable_f : 1;
  u32 cmdq_addr_lsb_f : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_rd_2_u
{
  struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_rd_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_RX_HND_CMDQ_DATA_RD_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_rd_3_s
{
  u32 cmdq_addr_msb_f : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_rd_3_u
{
  struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_data_rd_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_UC_RX_HND_CMDQ_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_status_s
{
  u32 status : 1;
  u32 cmdq_empty : 1;
  u32 cmdq_full : 1;
  u32 cmdq_count : 4;
  u32 cmdq_depth : 4;
  u32 reserved0 : 21;
};

/* Union definition of register */
union ipa_hwio_def_ipa_uc_rx_hnd_cmdq_status_u
{
  struct ipa_hwio_def_ipa_uc_rx_hnd_cmdq_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SPARE_REG_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_spare_reg_1_s
{
  u32 spare_bits : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_spare_reg_1_u
{
  struct ipa_hwio_def_ipa_spare_reg_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_UC2SEQ_PUSH 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_uc2seq_push_s
{
  u32 src_pipe : 8;
  u32 src_flags : 3;
  u32 reserved0 : 1;
  u32 src_id : 8;
  u32 ctx_id : 8;
  u32 reserved1 : 2;
  u32 virt_opcode : 1;
  u32 type : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_uc2seq_push_u
{
  struct ipa_hwio_def_ipa_hps_uc2seq_push_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_UC2SEQ_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_uc2seq_status_s
{
  u32 fill_level : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_uc2seq_status_u
{
  struct ipa_hwio_def_ipa_hps_uc2seq_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_SEQ2UC_RD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_seq2uc_rd_s
{
  u32 src_pipe : 8;
  u32 src_flags : 3;
  u32 reserved0 : 1;
  u32 src_id : 8;
  u32 ctx_id : 8;
  u32 reserved1 : 3;
  u32 type : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_seq2uc_rd_u
{
  struct ipa_hwio_def_ipa_hps_seq2uc_rd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_SEQ2UC_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_seq2uc_status_s
{
  u32 fill_level : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_seq2uc_status_u
{
  struct ipa_hwio_def_ipa_hps_seq2uc_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HPS_SEQ2UC_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hps_seq2uc_cmd_s
{
  u32 pop : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hps_seq2uc_cmd_u
{
  struct ipa_hwio_def_ipa_hps_seq2uc_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_UC2SEQ_PUSH 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_uc2seq_push_s
{
  u32 src_pipe : 8;
  u32 src_flags : 3;
  u32 reserved0 : 1;
  u32 src_id : 8;
  u32 ctx_id : 8;
  u32 reserved1 : 3;
  u32 type : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_uc2seq_push_u
{
  struct ipa_hwio_def_ipa_dps_uc2seq_push_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_UC2SEQ_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_uc2seq_status_s
{
  u32 fill_level : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_uc2seq_status_u
{
  struct ipa_hwio_def_ipa_dps_uc2seq_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_SEQ2UC_RD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_seq2uc_rd_s
{
  u32 src_pipe : 8;
  u32 src_flags : 3;
  u32 reserved0 : 1;
  u32 src_id : 8;
  u32 ctx_id : 8;
  u32 reserved1 : 3;
  u32 type : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_seq2uc_rd_u
{
  struct ipa_hwio_def_ipa_dps_seq2uc_rd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_SEQ2UC_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_seq2uc_status_s
{
  u32 fill_level : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_seq2uc_status_u
{
  struct ipa_hwio_def_ipa_dps_seq2uc_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_SEQ2UC_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_seq2uc_cmd_s
{
  u32 pop : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_seq2uc_cmd_u
{
  struct ipa_hwio_def_ipa_dps_seq2uc_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NTF_TX_CMDQ_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ntf_tx_cmdq_cmd_s
{
  u32 write_cmd : 1;
  u32 pop_cmd : 1;
  u32 rd_req : 1;
  u32 reserved0 : 1;
  u32 cmd_client : 8;
  u32 reserved1 : 20;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ntf_tx_cmdq_cmd_u
{
  struct ipa_hwio_def_ipa_ntf_tx_cmdq_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NTF_TX_CMDQ_DATA_WR_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ntf_tx_cmdq_data_wr_0_s
{
  u32 seg_ctx_id_f : 2;
  u32 cmdq_ctx_id_f : 6;
  u32 reserved0 : 4;
  u32 seg_valid_f : 1;
  u32 drop_valid_f : 1;
  u32 reserved1 : 2;
  u32 drop_reason_f : 4;
  u32 drop_type_f : 4;
  u32 reserved2 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ntf_tx_cmdq_data_wr_0_u
{
  struct ipa_hwio_def_ipa_ntf_tx_cmdq_data_wr_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NTF_TX_CMDQ_DATA_RD_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ntf_tx_cmdq_data_rd_0_s
{
  u32 seg_ctx_id_f : 2;
  u32 cmdq_ctx_id_f : 6;
  u32 reserved0 : 4;
  u32 seg_valid_f : 1;
  u32 drop_valid_f : 1;
  u32 reserved1 : 2;
  u32 drop_reason_f : 4;
  u32 drop_type_f : 4;
  u32 reserved2 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ntf_tx_cmdq_data_rd_0_u
{
  struct ipa_hwio_def_ipa_ntf_tx_cmdq_data_rd_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NTF_TX_CMDQ_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ntf_tx_cmdq_status_s
{
  u32 status : 1;
  u32 cmdq_full : 1;
  u32 reserved0 : 14;
  u32 cmdq_depth : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ntf_tx_cmdq_status_u
{
  struct ipa_hwio_def_ipa_ntf_tx_cmdq_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NTF_TX_SNP 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ntf_tx_snp_s
{
  u32 snp_last : 1;
  u32 snp_write : 1;
  u32 snp_valid : 1;
  u32 snp_next_is_valid : 1;
  u32 snp_next : 8;
  u32 snp_head : 8;
  u32 snp_addr : 8;
  u32 reserved0 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ntf_tx_snp_u
{
  struct ipa_hwio_def_ipa_ntf_tx_snp_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NTF_TX_CMDQ_COUNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ntf_tx_cmdq_count_s
{
  u32 fifo_count : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ntf_tx_cmdq_count_u
{
  struct ipa_hwio_def_ipa_ntf_tx_cmdq_count_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_CMDQ_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_cmd_s
{
  u32 write_cmd : 1;
  u32 pop_cmd : 1;
  u32 cmd_client : 8;
  u32 rd_req : 1;
  u32 reserved0 : 21;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_cmdq_cmd_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_CMDQ_DATA_RD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_data_rd_s
{
  u32 cmdq_src_id : 8;
  u32 cmdq_length : 16;
  u32 cmdq_origin : 1;
  u32 cmdq_sent : 1;
  u32 cmdq_src_id_valid : 1;
  u32 reserved0 : 5;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_cmdq_data_rd_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_data_rd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_CMDQ_DATA_RD_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_data_rd_1_s
{
  u32 cmdq_userdata_0 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_cmdq_data_rd_1_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_data_rd_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_CMDQ_DATA_RD_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_data_rd_2_s
{
  u32 cmdq_userdata_1 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_cmdq_data_rd_2_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_data_rd_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_CMDQ_DATA_RD_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_data_rd_3_s
{
  u32 cmdq_userdata_2 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_cmdq_data_rd_3_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_data_rd_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_CMDQ_STATUS_EMPTY_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_status_empty_n_s
{
  u32 cmdq_empty : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_cmdq_status_empty_n_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_status_empty_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_CMDQ_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_status_s
{
  u32 status : 1;
  u32 cmdq_full : 1;
  u32 cmdq_depth : 8;
  u32 reserved0 : 22;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_cmdq_status_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_CMDQ_COUNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_count_s
{
  u32 fifo_count : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_cmdq_count_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_cmdq_count_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ACKMNGR_SW_ACCESS_ACKINJ_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ackmngr_sw_access_ackinj_cfg_s
{
  u32 reserved0 : 5;
  u32 ackinj_src_id_valid : 1;
  u32 ackinj_origin : 1;
  u32 ackinj_sent : 1;
  u32 ackinj_src_id : 8;
  u32 ackinj_length : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ackmngr_sw_access_ackinj_cfg_u
{
  struct ipa_hwio_def_ipa_ackmngr_sw_access_ackinj_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ACKMNGR_SW_ACCESS_ACKINJ_PIPE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ackmngr_sw_access_ackinj_pipe_s
{
  u32 cons_ackinj_src_pipe : 8;
  u32 prod_ackinj_src_pipe : 8;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ackmngr_sw_access_ackinj_pipe_u
{
  struct ipa_hwio_def_ipa_ackmngr_sw_access_ackinj_pipe_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ACKMNGR_SW_ACCESS_ACKUPD_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ackmngr_sw_access_ackupd_cfg_s
{
  u32 ackupd_src_pipe : 8;
  u32 ackupd_src_id : 8;
  u32 ackupd_error : 1;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ackmngr_sw_access_ackupd_cfg_u
{
  struct ipa_hwio_def_ipa_ackmngr_sw_access_ackupd_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ACKMNGR_SW_ACCESS_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ackmngr_sw_access_cmd_s
{
  u32 ackinj_valid : 1;
  u32 ackupd_valid : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ackmngr_sw_access_cmd_u
{
  struct ipa_hwio_def_ipa_ackmngr_sw_access_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ACKMNGR_SW_ACCESS_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ackmngr_sw_access_status_s
{
  u32 ackinj_ready : 1;
  u32 ackupd_ready : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ackmngr_sw_access_status_u
{
  struct ipa_hwio_def_ipa_ackmngr_sw_access_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_SW_ACCESS_ACKINJ_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackinj_cfg_s
{
  u32 reserved0 : 5;
  u32 ackinj_src_id_valid : 1;
  u32 ackinj_origin : 1;
  u32 ackinj_sent : 1;
  u32 ackinj_src_id : 8;
  u32 ackinj_length : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackinj_cfg_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackinj_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_SW_ACCESS_ACKUPD_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackupd_cfg_s
{
  u32 ackupd_src_pipe : 8;
  u32 ackupd_src_id : 8;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackupd_cfg_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackupd_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_SW_ACCESS_CMD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_cmd_s
{
  u32 ackinj_valid : 1;
  u32 ackupd_valid : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_sw_access_cmd_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_cmd_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_SW_ACCESS_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_status_s
{
  u32 ackinj_ready : 1;
  u32 ackupd_ready : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_sw_access_status_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_SW_ACCESS_ACKINJ_CFG1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackinj_cfg1_s
{
  u32 ackinj_userdata : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackinj_cfg1_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackinj_cfg1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_SW_ACCESS_ACKINJ_CFG2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackinj_cfg2_s
{
  u32 ackinj_userdata : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackinj_cfg2_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackinj_cfg2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROD_ACKMNGR_SW_ACCESS_ACKINJ_CFG3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackinj_cfg3_s
{
  u32 ackinj_userdata : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackinj_cfg3_u
{
  struct ipa_hwio_def_ipa_prod_ackmngr_sw_access_ackinj_cfg3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NTF_TX_CMDQ_RELEASE_WR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ntf_tx_cmdq_release_wr_n_s
{
  u32 release_wr_cmd : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ntf_tx_cmdq_release_wr_n_u
{
  struct ipa_hwio_def_ipa_ntf_tx_cmdq_release_wr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NTF_TX_CMDQ_RELEASE_RD_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ntf_tx_cmdq_release_rd_n_s
{
  u32 release_rd_cmd : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ntf_tx_cmdq_release_rd_n_u
{
  struct ipa_hwio_def_ipa_ntf_tx_cmdq_release_rd_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NTF_TX_CMDQ_CFG_WR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ntf_tx_cmdq_cfg_wr_n_s
{
  u32 block_wr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ntf_tx_cmdq_cfg_wr_n_u
{
  struct ipa_hwio_def_ipa_ntf_tx_cmdq_cfg_wr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NTF_TX_CMDQ_CFG_RD_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ntf_tx_cmdq_cfg_rd_n_s
{
  u32 block_rd : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ntf_tx_cmdq_cfg_rd_n_u
{
  struct ipa_hwio_def_ipa_ntf_tx_cmdq_cfg_rd_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NTF_TX_CMDQ_STATUS_EMPTY_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ntf_tx_cmdq_status_empty_n_s
{
  u32 cmdq_empty : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ntf_tx_cmdq_status_empty_n_u
{
  struct ipa_hwio_def_ipa_ntf_tx_cmdq_status_empty_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_BASE_ADDR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_base_addr_s
{
  u32 zero : 21;
  u32 base : 11;
};

/* Union definition of register */
union ipa_hwio_def_ipa_base_addr_u
{
  struct ipa_hwio_def_ipa_base_addr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_BASE_ADDR_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_base_addr_msb_s
{
  u32 base_msb : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_base_addr_msb_u
{
  struct ipa_hwio_def_ipa_base_addr_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPSEC_SA_ENCAPSULATION_SIZE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipsec_sa_encapsulation_size_s
{
  u32 hw_region_size : 8;
  u32 hw_uc_ro_region_size : 8;
  u32 uc_rw_region_size : 8;
  u32 uc_ro_region_size : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipsec_sa_encapsulation_size_u
{
  struct ipa_hwio_def_ipa_ipsec_sa_encapsulation_size_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPSEC_SA_DECAPSULATION_SIZE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipsec_sa_decapsulation_size_s
{
  u32 hw_region_size : 8;
  u32 hw_uc_ro_region_size : 8;
  u32 uc_rw_region_size : 8;
  u32 uc_ro_region_size : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipsec_sa_decapsulation_size_u
{
  struct ipa_hwio_def_ipa_ipsec_sa_decapsulation_size_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CRYPTO_n_CLIB_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_crypto_n_clib_status_s
{
  u32 clib_encr_din_fifo_empty : 1;
  u32 clib_encr_din_fifo_err : 1;
  u32 clib_encr_dout_fifo_full : 1;
  u32 clib_encr_dout_fifo_err : 1;
  u32 clib_auth_din_fifo_err : 1;
  u32 clib_auth_din_fifo_empty : 1;
  u32 clib_all_fifos_empty : 1;
  u32 clib_pad_fifo_err : 1;
  u32 clib_all_engines_inactive : 1;
  u32 clib_encr_aes_engine_busy : 2;
  u32 reserved0 : 21;
};

/* Union definition of register */
union ipa_hwio_def_ipa_crypto_n_clib_status_u
{
  struct ipa_hwio_def_ipa_crypto_n_clib_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CRYPTO_FALL_BACK_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_crypto_fall_back_cfg_s
{
  u32 dis_pipeline : 1;
  u32 dis_pipeline_for_same_sa : 1;
  u32 start_soft_anti_replay_in_crypto_stage : 1;
  u32 dis_hard_anti_replay : 1;
  u32 dis_bypass : 1;
  u32 dis_bypass_decaps_same_sa : 1;
  u32 swap_en_seq_n_l : 1;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_crypto_fall_back_cfg_u
{
  struct ipa_hwio_def_ipa_crypto_fall_back_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_SEQ2UC_RD_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_seq2uc_rd_1_s
{
  u32 dest_pipe : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_seq2uc_rd_1_u
{
  struct ipa_hwio_def_ipa_dps_seq2uc_rd_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPS_UC2SEQ_PUSH_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dps_uc2seq_push_1_s
{
  u32 dest_pipe : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dps_uc2seq_push_1_u
{
  struct ipa_hwio_def_ipa_dps_uc2seq_push_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TIERING_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tiering_cfg_s
{
  u32 disconnect_nat_clk : 1;
  u32 disconnect_ipsec_clk : 1;
  u32 fw_purpose_bits : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tiering_cfg_u
{
  struct ipa_hwio_def_ipa_tiering_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TIERING_CFG_WR_ONCE_INDICATION 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tiering_cfg_wr_once_indication_s
{
  u32 val : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tiering_cfg_wr_once_indication_u
{
  struct ipa_hwio_def_ipa_tiering_cfg_wr_once_indication_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FORCE_PCIE_ORDERING_PER_MID 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_force_pcie_ordering_per_mid_s
{
  u32 en_mid_bitmap : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_force_pcie_ordering_per_mid_u
{
  struct ipa_hwio_def_ipa_force_pcie_ordering_per_mid_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_GSI_CFG1_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_gsi_cfg1_n_s
{
  u32 reserved0 : 16;
  u32 endp_en : 1;
  u32 legacy_new_re : 1;
  u32 reserved1 : 13;
  u32 init_endp : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_gsi_cfg1_n_u
{
  struct ipa_hwio_def_ipa_endp_gsi_cfg1_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_GSI_CFG_TLV_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_gsi_cfg_tlv_n_s
{
  u32 fifo_base_addr : 16;
  u32 fifo_size : 8;
  u32 reserved0 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_gsi_cfg_tlv_n_u
{
  struct ipa_hwio_def_ipa_endp_gsi_cfg_tlv_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_GSI_CFG_AOS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_gsi_cfg_aos_n_s
{
  u32 fifo_base_addr : 16;
  u32 fifo_size : 8;
  u32 reserved0 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_gsi_cfg_aos_n_u
{
  struct ipa_hwio_def_ipa_endp_gsi_cfg_aos_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_COAL_VP_AOS_FIFO_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_coal_vp_aos_fifo_n_s
{
  u32 fifo_base_addr : 16;
  u32 fifo_size : 8;
  u32 reserved0 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_coal_vp_aos_fifo_n_u
{
  struct ipa_hwio_def_ipa_coal_vp_aos_fifo_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_QMB_DEBUG_CTRL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_qmb_debug_ctrl_s
{
  u32 ram_slaveway_access_protection_disable : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_qmb_debug_ctrl_u
{
  struct ipa_hwio_def_ipa_qmb_debug_ctrl_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TLV_RESERVE_PTR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tlv_reserve_ptr_n_s
{
  u32 wr_ptr : 8;
  u32 num_reserved : 8;
  u32 reserved_idx : 8;
  u32 valid : 1;
  u32 reserved0 : 7;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tlv_reserve_ptr_n_u
{
  struct ipa_hwio_def_ipa_tlv_reserve_ptr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_AOS_RESERVE_PTR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_aos_reserve_ptr_n_s
{
  u32 wr_ptr : 8;
  u32 num_reserved : 8;
  u32 reserved_idx : 8;
  u32 valid : 1;
  u32 reserved0 : 7;
};

/* Union definition of register */
union ipa_hwio_def_ipa_aos_reserve_ptr_n_u
{
  struct ipa_hwio_def_ipa_aos_reserve_ptr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CTXH_CTRL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ctxh_ctrl_s
{
  u32 ctxh_lock_id : 6;
  u32 reserved0 : 23;
  u32 ctxh_wr_block_on_noc_err : 1;
  u32 ctxh_lock_active : 1;
  u32 ctxh_lock : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ctxh_ctrl_u
{
  struct ipa_hwio_def_ipa_ctxh_ctrl_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SEG_CTX_ID_m_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_seg_ctx_id_m_0_s
{
  u32 dma_task_eof : 1;
  u32 filter_aggr_force_close : 1;
  u32 last : 1;
  u32 router_aggr_force_close : 1;
  u32 open_frame : 1;
  u32 reserved0 : 3;
  u32 trnseq_len_ext : 8;
  u32 data_sector_valid_length : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_seg_ctx_id_m_0_u
{
  struct ipa_hwio_def_ipa_seg_ctx_id_m_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SEG_CTX_ID_m_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_seg_ctx_id_m_1_s
{
  u32 l4_pseudo_hdr_checksum : 16;
  u32 revised_packet_length : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_seg_ctx_id_m_1_u
{
  struct ipa_hwio_def_ipa_seg_ctx_id_m_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SEG_CTX_ID_m_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_seg_ctx_id_m_2_s
{
  u32 trnseq_0_opcode : 4;
  u32 trnseq_0_length : 12;
  u32 trnseq_0_offset : 12;
  u32 reserved0 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_seg_ctx_id_m_2_u
{
  struct ipa_hwio_def_ipa_seg_ctx_id_m_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SEG_CTX_ID_m_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_seg_ctx_id_m_3_s
{
  u32 trnseq_1_opcode : 4;
  u32 trnseq_1_length : 12;
  u32 trnseq_1_offset : 12;
  u32 reserved0 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_seg_ctx_id_m_3_u
{
  struct ipa_hwio_def_ipa_seg_ctx_id_m_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SEG_CTX_ID_m_4 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_seg_ctx_id_m_4_s
{
  u32 trnseq_2_opcode : 4;
  u32 trnseq_2_length : 12;
  u32 trnseq_2_offset : 12;
  u32 reserved0 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_seg_ctx_id_m_4_u
{
  struct ipa_hwio_def_ipa_seg_ctx_id_m_4_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SEG_CTX_ID_m_5 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_seg_ctx_id_m_5_s
{
  u32 trnseq_3_opcode : 4;
  u32 trnseq_3_length : 12;
  u32 trnseq_3_offset : 12;
  u32 reserved0 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_seg_ctx_id_m_5_u
{
  struct ipa_hwio_def_ipa_seg_ctx_id_m_5_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SEG_CTX_ID_m_6 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_seg_ctx_id_m_6_s
{
  u32 trnseq_4_opcode : 4;
  u32 trnseq_4_length : 12;
  u32 trnseq_4_offset : 12;
  u32 reserved0 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_seg_ctx_id_m_6_u
{
  struct ipa_hwio_def_ipa_seg_ctx_id_m_6_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SID_PCIE_VFID_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_sid_pcie_vfid_cfg_n_s
{
  u32 sid_vfen_vfid : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_sid_pcie_vfid_cfg_n_u
{
  struct ipa_hwio_def_ipa_sid_pcie_vfid_cfg_n_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_CFG
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FLAVOR_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_flavor_0_s
{
  u32 ipa_pipes : 8;
  u32 ipa_cons_pipes : 8;
  u32 ipa_prod_pipes : 8;
  u32 ipa_prod_lowest : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_flavor_0_u
{
  struct ipa_hwio_def_ipa_flavor_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FLAVOR_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_flavor_1_s
{
  u32 ctx_n : 8;
  u32 mbim_deagg_en : 1;
  u32 ucp_en : 1;
  u32 d_dcph_2_en : 1;
  u32 d_dcph_en : 1;
  u32 h_dcph_en : 1;
  u32 reserved0 : 1;
  u32 filter_router_cache_gen : 1;
  u32 nat_acl_en : 1;
  u32 vmidmt_en : 1;
  u32 uc_num : 3;
  u32 cpr_en : 1;
  u32 cons_dpl_en : 1;
  u32 qmb0_slaveway_en : 1;
  u32 qmb1_slaveway_en : 1;
  u32 qmb1_en : 1;
  u32 dual_tx_en : 1;
  u32 rx_uc_handler_en : 1;
  u32 gsi_slaveway_en : 1;
  u32 pcie_path_en : 1;
  u32 d_dcph_engine_num : 2;
  u32 reserved1 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_flavor_1_u
{
  struct ipa_hwio_def_ipa_flavor_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FLAVOR_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_flavor_2_s
{
  u32 qmb0_outst_wr : 6;
  u32 reserved0 : 2;
  u32 qmb0_outst_rd : 6;
  u32 reserved1 : 2;
  u32 qmb1_outst_wr : 6;
  u32 reserved2 : 2;
  u32 qmb1_outst_rd : 6;
  u32 reserved3 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_flavor_2_u
{
  struct ipa_hwio_def_ipa_flavor_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FLAVOR_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_flavor_3_s
{
  u32 rsrc_grp_src_num_wout_uc : 4;
  u32 rsrc_grp_src_num_uc : 4;
  u32 rsrc_grp_dst_num_wo_uc : 4;
  u32 rsrc_grp_dst_num_uc : 4;
  u32 pkt_ctx_size : 8;
  u32 reserved0 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_flavor_3_u
{
  struct ipa_hwio_def_ipa_flavor_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FLAVOR_4 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_flavor_4_s
{
  u32 generic_agg_pipes : 8;
  u32 generic_deagg_pipes : 8;
  u32 bearer_init_ctx_num : 8;
  u32 mbim_agg_pipes : 4;
  u32 frag_tables_num : 3;
  u32 reserved0 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_flavor_4_u
{
  struct ipa_hwio_def_ipa_flavor_4_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FLAVOR_5 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_flavor_5_s
{
  u32 producer_ack_mngr_db_depth : 8;
  u32 ipa_num_ees : 4;
  u32 gsi_num_ees : 4;
  u32 rx_hps_cmdq_q_depth : 6;
  u32 consumer_ack_mngr_db_depth : 10;
};

/* Union definition of register */
union ipa_hwio_def_ipa_flavor_5_u
{
  struct ipa_hwio_def_ipa_flavor_5_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FLAVOR_6 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_flavor_6_s
{
  u32 hps_dmar_num : 4;
  u32 dps_dmar_num : 4;
  u32 data_descriptor_buffers : 9;
  u32 reserved0 : 3;
  u32 data_sectors : 10;
  u32 reserved1 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_flavor_6_u
{
  struct ipa_hwio_def_ipa_flavor_6_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FLAVOR_7 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_flavor_7_s
{
  u32 tlv_entry_num : 10;
  u32 reserved0 : 6;
  u32 aos_entry_num : 10;
  u32 coal_vp_num : 5;
  u32 reserved1 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_flavor_7_u
{
  struct ipa_hwio_def_ipa_flavor_7_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FLAVOR_8 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_flavor_8_s
{
  u32 multi_drbip_dmar_engine_num : 4;
  u32 multi_drbip_dcph_engine_num : 4;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_flavor_8_u
{
  struct ipa_hwio_def_ipa_flavor_8_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FLAVOR_9 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_flavor_9_s
{
  u32 ipa_max_supported_tsp_ingress_tcs : 8;
  u32 ipa_max_supported_tsp_egress_tcs : 8;
  u32 ipa_max_supported_tsp_producers : 8;
  u32 reserved0 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_flavor_9_u
{
  struct ipa_hwio_def_ipa_flavor_9_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FLAVOR_10 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_flavor_10_s
{
  u32 ipa_max_supported_qmngr_blocks : 17;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_flavor_10_u
{
  struct ipa_hwio_def_ipa_flavor_10_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_COMP_HW_VERSION 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_comp_hw_version_s
{
  u32 step : 16;
  u32 minor : 12;
  u32 major : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_comp_hw_version_u
{
  struct ipa_hwio_def_ipa_comp_hw_version_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VERSION 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_version_s
{
  u32 ipa_r_rev : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_version_u
{
  struct ipa_hwio_def_ipa_version_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_COMP_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_comp_cfg_s
{
  u32 ram_arb_priority_client_samp_fix_disable : 1;
  u32 gsi_snoc_bypass_dis : 1;
  u32 gen_qmb_0_snoc_bypass_dis : 1;
  u32 gen_qmb_1_snoc_bypass_dis : 1;
  u32 ipa_nat_uc_lock_mechanism_bresp_wait : 1;
  u32 ipa_qmb_select_by_address_cons_en : 1;
  u32 ipa_qmb_select_by_address_prod_en : 1;
  u32 gsi_multi_inorder_rd_dis : 1;
  u32 gsi_multi_inorder_wr_dis : 1;
  u32 gen_qmb_0_multi_inorder_rd_dis : 1;
  u32 gen_qmb_1_multi_inorder_rd_dis : 1;
  u32 gen_qmb_0_multi_inorder_wr_dis : 1;
  u32 gen_qmb_1_multi_inorder_wr_dis : 1;
  u32 gen_qmb_0_snoc_cnoc_loop_protection_disable : 1;
  u32 gsi_snoc_cnoc_loop_protection_disable : 1;
  u32 gsi_multi_axi_masters_dis : 1;
  u32 ipa_qmb_select_by_address_global_en : 1;
  u32 ipa_qmb1_256b_reads_dis : 1;
  u32 reserved0 : 1;
  u32 qmb_ram_rd_cache_disable : 1;
  u32 genqmb_aooowr : 1;
  u32 gsi_if_out_of_buf_stop_reset_mask_enable : 1;
  u32 ipa_atomic_fetcher_arb_lock_dis : 7;
  u32 reserved1 : 1;
  u32 gen_qmb_1_dynamic_asize : 1;
  u32 gen_qmb_0_dynamic_asize : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_comp_cfg_u
{
  struct ipa_hwio_def_ipa_comp_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CLKON_CFG_SPECIAL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_clkon_cfg_special_s
{
  u32 cgc_open_tpdm_cmb : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_clkon_cfg_special_u
{
  struct ipa_hwio_def_ipa_clkon_cfg_special_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CLKON_CFG_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_clkon_cfg_1_s
{
  u32 cgc_open_ipa_core_clk_phase : 1;
  u32 cgc_open_ipa_xpu_wrapper : 1;
  u32 cgc_open_ipa_tsp : 1;
  u32 cgc_open_prod_dpl_fifo : 1;
  u32 cgc_open_crypto : 1;
  u32 reserved0 : 27;
};

/* Union definition of register */
union ipa_hwio_def_ipa_clkon_cfg_1_u
{
  struct ipa_hwio_def_ipa_clkon_cfg_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CLKON_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_clkon_cfg_s
{
  u32 cgc_open_rx : 1;
  u32 cgc_open_proc : 1;
  u32 cgc_open_tx_wrapper : 1;
  u32 cgc_open_misc : 1;
  u32 cgc_open_ram_arb : 1;
  u32 cgc_open_ftch_hps : 1;
  u32 cgc_open_ftch_dps : 1;
  u32 cgc_open_hps : 1;
  u32 cgc_open_dps : 1;
  u32 cgc_open_rx_hps_cmdqs : 1;
  u32 cgc_open_hps_dps_cmdqs : 1;
  u32 cgc_open_dps_tx_cmdqs : 1;
  u32 cgc_open_rsrc_mngr : 1;
  u32 cgc_open_ctx_handler : 1;
  u32 cgc_open_ack_mngr : 1;
  u32 cgc_open_d_dcph : 1;
  u32 cgc_open_h_dcph : 1;
  u32 reserved0 : 1;
  u32 cgc_open_ntf_tx_cmdqs : 1;
  u32 cgc_open_tx_0 : 1;
  u32 cgc_open_tx_1 : 1;
  u32 cgc_open_fnr : 1;
  u32 cgc_open_qsb2axi_cmdq_l : 1;
  u32 cgc_open_aggr_wrapper : 1;
  u32 cgc_open_ram_slaveway : 1;
  u32 cgc_open_qmb : 1;
  u32 cgc_open_weight_arb : 1;
  u32 cgc_open_gsi_if : 1;
  u32 cgc_open_global : 1;
  u32 cgc_open_global_2x_clk : 1;
  u32 cgc_open_cons_dpl_fifo : 1;
  u32 cgc_open_drbip : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_clkon_cfg_u
{
  struct ipa_hwio_def_ipa_clkon_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ROUTE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_route_s
{
  u32 route_def_pipe : 8;
  u32 route_frag_def_pipe : 8;
  u32 route_def_hdr_ofst : 10;
  u32 route_def_hdr_table : 1;
  u32 route_def_retain_hdr : 1;
  u32 route_dis : 1;
  u32 reserved0 : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_route_u
{
  struct ipa_hwio_def_ipa_route_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ROUTE_EXT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_route_ext_s
{
  u32 route_def_hdr_ofst_msbs : 7;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_route_ext_u
{
  struct ipa_hwio_def_ipa_route_ext_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MASTER_PRIORITY 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_master_priority_s
{
  u32 qmb_0_rd : 2;
  u32 qmb_1_rd : 2;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_master_priority_u
{
  struct ipa_hwio_def_ipa_master_priority_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SHARED_MEM_SIZE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_shared_mem_size_s
{
  u32 shared_mem_size : 15;
  u32 shared_mem_baddr : 17;
};

/* Union definition of register */
union ipa_hwio_def_ipa_shared_mem_size_u
{
  struct ipa_hwio_def_ipa_shared_mem_size_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NAT_TIMER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nat_timer_s
{
  u32 nat_timer : 24;
  u32 reserved0 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nat_timer_u
{
  struct ipa_hwio_def_ipa_nat_timer_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TAG_TIMER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tag_timer_s
{
  u32 tag_timer : 24;
  u32 reserved0 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tag_timer_u
{
  struct ipa_hwio_def_ipa_tag_timer_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FRAG_RULES_CLR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_frag_rules_clr_s
{
  u32 clr : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_frag_rules_clr_u
{
  struct ipa_hwio_def_ipa_frag_rules_clr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PROC_IPH_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_proc_iph_cfg_s
{
  u32 reserved0 : 8;
  u32 iph_pkt_parser_protocol_stop_enable : 1;
  u32 iph_pkt_parser_protocol_stop_hop : 1;
  u32 iph_pkt_parser_protocol_stop_dest : 1;
  u32 iph_pkt_parser_ihl_to_2nd_frag_en : 1;
  u32 reserved1 : 4;
  u32 iph_pkt_parser_protocol_stop_value : 8;
  u32 d_dcph_multi_engine_disable : 1;
  u32 hps_sequencer_round_bypass_disable : 1;
  u32 reserved2 : 6;
};

/* Union definition of register */
union ipa_hwio_def_ipa_proc_iph_cfg_u
{
  struct ipa_hwio_def_ipa_proc_iph_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_QSB_MAX_WRITES 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_qsb_max_writes_s
{
  u32 gen_qmb_0_max_writes : 5;
  u32 reserved0 : 3;
  u32 gen_qmb_1_max_writes : 5;
  u32 reserved1 : 19;
};

/* Union definition of register */
union ipa_hwio_def_ipa_qsb_max_writes_u
{
  struct ipa_hwio_def_ipa_qsb_max_writes_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_QSB_MAX_READS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_qsb_max_reads_s
{
  u32 gen_qmb_0_max_reads : 5;
  u32 reserved0 : 3;
  u32 gen_qmb_1_max_reads : 5;
  u32 reserved1 : 3;
  u32 gen_qmb_0_max_read_beats : 8;
  u32 gen_qmb_1_max_read_beats : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_qsb_max_reads_u
{
  struct ipa_hwio_def_ipa_qsb_max_reads_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_QSB_OUTSTANDING_COUNTER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_qsb_outstanding_counter_s
{
  u32 gen_qmb_0_reads_cnt : 6;
  u32 reserved0 : 2;
  u32 gen_qmb_1_reads_cnt : 6;
  u32 reserved1 : 2;
  u32 gen_qmb_0_writes_cnt : 6;
  u32 reserved2 : 2;
  u32 gen_qmb_1_writes_cnt : 6;
  u32 reserved3 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_qsb_outstanding_counter_u
{
  struct ipa_hwio_def_ipa_qsb_outstanding_counter_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_QSB_OUTSTANDING_BEATS_COUNTER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_qsb_outstanding_beats_counter_s
{
  u32 gen_qmb_0_read_beats_cnt : 8;
  u32 gen_qmb_1_read_beats_cnt : 8;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_qsb_outstanding_beats_counter_u
{
  struct ipa_hwio_def_ipa_qsb_outstanding_beats_counter_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPL_TIMER_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dpl_timer_lsb_s
{
  u32 tod_lsb : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dpl_timer_lsb_u
{
  struct ipa_hwio_def_ipa_dpl_timer_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPL_TIMER_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dpl_timer_msb_s
{
  u32 tod_msb : 20;
  u32 reserved0 : 7;
  u32 gran_sel : 4;
  u32 timer_en : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dpl_timer_msb_u
{
  struct ipa_hwio_def_ipa_dpl_timer_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPL_TIMER_CTL_STS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dpl_timer_ctl_sts_s
{
  u32 legacy_timer : 1;
  u32 reserved0 : 3;
  u32 tod_valid : 1;
  u32 reserved1 : 27;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dpl_timer_ctl_sts_u
{
  struct ipa_hwio_def_ipa_dpl_timer_ctl_sts_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_TX_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_tx_1_s
{
  u32 stage_arb_ctx_id : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_tx_1_u
{
  struct ipa_hwio_def_ipa_state_tx_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_ACL_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_acl_1_s
{
  u32 ipa_hps_crypto_empty : 1;
  u32 ipa_hps_crypto_active : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_acl_1_u
{
  struct ipa_hwio_def_ipa_state_acl_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_RX_ACTIVE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_rx_active_n_s
{
  u32 endpoints : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_rx_active_n_u
{
  struct ipa_hwio_def_ipa_state_rx_active_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_TX_WRAPPER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_tx_wrapper_s
{
  u32 tx_idle : 1;
  u32 reserved0 : 1;
  u32 ipa_prod_ackmngr_db_empty : 1;
  u32 ipa_prod_ackmngr_state_idle : 1;
  u32 ipa_prod_bresp_empty : 1;
  u32 reserved1 : 1;
  u32 ipa_mbim_pkt_fsm_idle : 2;
  u32 mbim_direct_dma : 2;
  u32 reserved2 : 4;
  u32 nlo_direct_dma : 2;
  u32 coal_direct_dma : 2;
  u32 reserved3 : 14;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_tx_wrapper_u
{
  struct ipa_hwio_def_ipa_state_tx_wrapper_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_TX 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_tx_s
{
  u32 stage_arb_busy : 1;
  u32 stage_packet_processsor_busy : 1;
  u32 stage_address_resolution_busy : 1;
  u32 stage_packet_constructor_0_busy : 1;
  u32 stage_packet_constructor_1_busy : 1;
  u32 stage_transmission_0_busy : 1;
  u32 stage_transmission_1_busy : 1;
  u32 stage_checksum_handler_0_busy : 1;
  u32 stage_checksum_handler_1_busy : 1;
  u32 packet_drop_counter_busy : 1;
  u32 suspend_handler_busy : 1;
  u32 drop_handler_busy : 1;
  u32 packet_release_handler_busy : 1;
  u32 holb_mask_valid : 1;
  u32 dmaw_0_busy : 1;
  u32 dmaw_1_busy : 1;
  u32 stage_arb_dest_pipe : 8;
  u32 reserved0 : 4;
  u32 stage_arb_dma_type : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_tx_u
{
  struct ipa_hwio_def_ipa_state_tx_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_TX_HOLB_MASK_DPS_TX_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_tx_holb_mask_dps_tx_0_s
{
  u32 producer_mask_0_31 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_tx_holb_mask_dps_tx_0_u
{
  struct ipa_hwio_def_ipa_state_tx_holb_mask_dps_tx_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_TX_HOLB_MASK_NTF_TX_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_tx_holb_mask_ntf_tx_0_s
{
  u32 producer_mask_0_31 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_tx_holb_mask_ntf_tx_0_u
{
  struct ipa_hwio_def_ipa_state_tx_holb_mask_ntf_tx_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_TX_HOLB_MASK_NTF_TX_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_tx_holb_mask_ntf_tx_1_s
{
  u32 producer_mask_32_64 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_tx_holb_mask_ntf_tx_1_u
{
  struct ipa_hwio_def_ipa_state_tx_holb_mask_ntf_tx_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_FETCHER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_fetcher_s
{
  u32 ipa_hps_ftch_state_idle : 1;
  u32 ipa_hps_ftch_alloc_state_idle : 1;
  u32 ipa_hps_ftch_pkt_state_idle : 1;
  u32 ipa_hps_ftch_imm_state_idle : 1;
  u32 ipa_hps_ftch_cmplt_state_idle : 1;
  u32 ipa_hps_dmar_state_idle : 7;
  u32 ipa_hps_dmar_slot_state_idle : 7;
  u32 ipa_hps_imm_cmd_exec_state_idle : 1;
  u32 ipa_hps_desc_handler_state_idle : 8;
  u32 reserved0 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_fetcher_u
{
  struct ipa_hwio_def_ipa_state_fetcher_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_FETCHER_MASK_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_fetcher_mask_0_s
{
  u32 mask_queue_src_grp_dmar_outstanding : 8;
  u32 mask_queue_imm_exec : 8;
  u32 mask_queue_no_resources_context : 8;
  u32 mask_queue_no_resources_hps_dmar : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_fetcher_mask_0_u
{
  struct ipa_hwio_def_ipa_state_fetcher_mask_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_DFETCHER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_dfetcher_s
{
  u32 ipa_dps_ftch_pkt_state_idle : 1;
  u32 ipa_dps_ftch_cmplt_state_idle : 1;
  u32 reserved0 : 2;
  u32 ipa_dps_dmar_state_idle : 8;
  u32 reserved1 : 4;
  u32 ipa_dps_dmar_slot_state_idle : 8;
  u32 reserved2 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_dfetcher_u
{
  struct ipa_hwio_def_ipa_state_dfetcher_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_ACL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_acl_s
{
  u32 ipa_hps_h_dcph_empty : 1;
  u32 ipa_hps_h_dcph_active : 1;
  u32 ipa_hps_pkt_parser_empty : 1;
  u32 ipa_hps_pkt_parser_active : 1;
  u32 ipa_hps_filter_nat_empty : 1;
  u32 ipa_hps_filter_nat_active : 1;
  u32 ipa_hps_router_empty : 1;
  u32 ipa_hps_router_active : 1;
  u32 ipa_hps_hdri_empty : 1;
  u32 ipa_hps_hdri_active : 1;
  u32 ipa_hps_ucp_empty : 1;
  u32 ipa_hps_ucp_active : 1;
  u32 ipa_hps_enqueuer_empty : 1;
  u32 ipa_hps_enqueuer_active : 1;
  u32 ipa_dps_d_dcph_empty : 1;
  u32 ipa_dps_d_dcph_active : 1;
  u32 reserved0 : 2;
  u32 ipa_dps_dispatcher_empty : 1;
  u32 ipa_dps_dispatcher_active : 1;
  u32 ipa_dps_d_dcph_2_empty : 1;
  u32 ipa_dps_d_dcph_2_active : 1;
  u32 ipa_hps_sequencer_idle : 1;
  u32 ipa_dps_sequencer_idle : 1;
  u32 ipa_dps_d_dcph_2nd_empty : 1;
  u32 ipa_dps_d_dcph_2nd_active : 1;
  u32 ipa_hps_coal_master_empty : 1;
  u32 ipa_hps_coal_master_active : 1;
  u32 ipa_hps_multi_drbip_empty : 1;
  u32 ipa_hps_multi_drbip_active : 1;
  u32 ipa_hps_empty : 1;
  u32 ipa_hps_seq_ack_mngr_empty : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_acl_u
{
  struct ipa_hwio_def_ipa_state_acl_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_s
{
  u32 rx_wait : 1;
  u32 rx_idle : 1;
  u32 tx_idle : 1;
  u32 reserved0 : 1;
  u32 ipa_cons_dpl_fifo_idle : 1;
  u32 gsi_idle : 1;
  u32 ipa_hw_sniffer_idle : 1;
  u32 ipa_pcie_noc_idle : 1;
  u32 ipa_ddr_noc_idle : 1;
  u32 aggr_idle : 1;
  u32 mbim_aggr_idle : 1;
  u32 ipa_rsrc_mngr_db_empty : 1;
  u32 ipa_rsrc_state_idle : 1;
  u32 ipa_ackmngr_db_empty : 1;
  u32 ipa_ackmngr_state_idle : 1;
  u32 ipa_tx_ackq_full : 1;
  u32 ipa_prod_ackmngr_db_empty : 1;
  u32 ipa_prod_ackmngr_state_idle : 1;
  u32 ipa_prod_bresp_idle : 1;
  u32 ipa_full_idle : 1;
  u32 ipa_ntf_tx_empty : 1;
  u32 ipa_prod_dpl_fifo_idle : 1;
  u32 ipa_uc_ackq_empty : 1;
  u32 ipa_rx_ackq_empty : 1;
  u32 ipa_tx_commander_cmdq_empty : 1;
  u32 ipa_rx_hps_empty : 1;
  u32 ipa_hps_dps_empty : 1;
  u32 ipa_dps_tx_empty : 1;
  u32 ipa_uc_rx_hnd_cmdq_empty : 1;
  u32 reserved1 : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_u
{
  struct ipa_hwio_def_ipa_state_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_GSI_AOS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_gsi_aos_s
{
  u32 ipa_gsi_aos_fsm_idle : 1;
  u32 ipa_gsi_aos_nlo_fsm_idle : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_gsi_aos_u
{
  struct ipa_hwio_def_ipa_state_gsi_aos_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_COAL_SLAVE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_coal_slave_s
{
  u32 coal_slave_open_frame : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_coal_slave_u
{
  struct ipa_hwio_def_ipa_state_coal_slave_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_GSI_IF 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_gsi_if_s
{
  u32 ipa_gsi_prod_fsm_tx_0 : 4;
  u32 ipa_gsi_prod_fsm_tx_1 : 4;
  u32 ipa_gsi_toggle_fsm_idle : 1;
  u32 reserved0 : 7;
  u32 ipa_gsi_skip_fsm : 2;
  u32 reserved1 : 14;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_gsi_if_u
{
  struct ipa_hwio_def_ipa_state_gsi_if_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_RQOS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_rqos_s
{
  u32 ipa_rqos_nas_idle : 1;
  u32 ipa_rqos_as_idle : 1;
  u32 ipa_rqos_sw_idle : 1;
  u32 ipa_rqos_fifo_empty : 1;
  u32 ipa_rqos_fifo_pop_idle : 1;
  u32 reserved0 : 27;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_rqos_u
{
  struct ipa_hwio_def_ipa_state_rqos_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_GSI_IF_CONS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_gsi_if_cons_s
{
  u32 state_idle : 1;
  u32 cache_vld : 8;
  u32 reserved0 : 23;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_gsi_if_cons_u
{
  struct ipa_hwio_def_ipa_state_gsi_if_cons_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_FETCHER_MASK_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_fetcher_mask_1_s
{
  u32 mask_queue_no_resources_ack_entry : 8;
  u32 mask_queue_arb_lock : 8;
  u32 mask_queue_no_resources_desc_buffer : 8;
  u32 mask_queue_no_space_dpl_fifo : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_fetcher_mask_1_u
{
  struct ipa_hwio_def_ipa_state_fetcher_mask_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_FETCHER_MASK_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_fetcher_mask_2_s
{
  u32 mask_queue_drbip_no_data_sectors : 8;
  u32 mask_queue_drbip_pkt_exceed_max_size : 8;
  u32 mask_queue_no_space_rqos_fifo : 8;
  u32 mask_queue_desc_handler_uses_queue : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_fetcher_mask_2_u
{
  struct ipa_hwio_def_ipa_state_fetcher_mask_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_CONS_DPL_FIFO 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_cons_dpl_fifo_s
{
  u32 pop_fsm_state : 3;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_cons_dpl_fifo_u
{
  struct ipa_hwio_def_ipa_state_cons_dpl_fifo_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_COAL_MASTER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_coal_master_s
{
  u32 main_fsm_state : 4;
  u32 find_open_fsm_state : 4;
  u32 hash_calc_fsm_state : 4;
  u32 check_fit_fsm_state : 4;
  u32 init_vp_fsm_state : 4;
  u32 lru_vp : 6;
  u32 reserved0 : 6;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_coal_master_u
{
  struct ipa_hwio_def_ipa_state_coal_master_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_COAL_MASTER_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_coal_master_1_s
{
  u32 init_vp_wr_ctx_line : 6;
  u32 init_vp_rd_pkt_line : 6;
  u32 init_vp_fsm_state : 4;
  u32 check_fit_rd_ctx_line : 6;
  u32 check_fit_fsm_state : 4;
  u32 arbiter_state : 4;
  u32 reserved0 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_coal_master_1_u
{
  struct ipa_hwio_def_ipa_state_coal_master_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_NLO_AGGR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_nlo_aggr_s
{
  u32 nlo_aggr_state : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_nlo_aggr_u
{
  struct ipa_hwio_def_ipa_state_nlo_aggr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_CTXH 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_ctxh_s
{
  u32 ipa_ctxh_rd_idle : 1;
  u32 ipa_ctxh_wr_idle : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_ctxh_u
{
  struct ipa_hwio_def_ipa_state_ctxh_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_UC_QMB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_uc_qmb_s
{
  u32 ctrl_fsm_state_queue_0 : 2;
  u32 ot_table_empty_queue_0 : 1;
  u32 ot_table_full_queue_0 : 1;
  u32 comp_fifo_empty_queue_0 : 1;
  u32 comp_fifo_full_queue_0 : 1;
  u32 cmd_fifo_empty_queue_0 : 1;
  u32 cmd_fifo_full_queue_0 : 1;
  u32 queue_0_idle : 1;
  u32 reserved0 : 7;
  u32 ctrl_fsm_state_queue_1 : 2;
  u32 ot_table_empty_queue_1 : 1;
  u32 ot_table_full_queue_1 : 1;
  u32 comp_fifo_empty_queue_1 : 1;
  u32 comp_fifo_full_queue_1 : 1;
  u32 cmd_fifo_empty_queue_1 : 1;
  u32 cmd_fifo_full_queue_1 : 1;
  u32 queue_1_idle : 1;
  u32 reserved1 : 7;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_uc_qmb_u
{
  struct ipa_hwio_def_ipa_state_uc_qmb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_DRBIP 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_drbip_s
{
  u32 drbip_dmar_idle : 9;
  u32 reserved0 : 3;
  u32 drbip_dcph_idle : 3;
  u32 reserved1 : 1;
  u32 drbip_pkt_idle : 12;
  u32 reserved2 : 3;
  u32 drbip_shell_idle : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_drbip_u
{
  struct ipa_hwio_def_ipa_state_drbip_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_PKT_PARSER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_pkt_parser_s
{
  u32 pkt_parser_shell_idle : 1;
  u32 pkt_parser_dpl_idle : 1;
  u32 pkt_parser_frag_idle : 1;
  u32 pkt_parser_snapshot_idle : 1;
  u32 pkt_parser_pkt_idle : 6;
  u32 reserved0 : 2;
  u32 pkt_parser_analyzer_idle : 2;
  u32 reserved1 : 2;
  u32 pkt_parser_analyzer_update_idle : 2;
  u32 reserved2 : 14;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_pkt_parser_u
{
  struct ipa_hwio_def_ipa_state_pkt_parser_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_TX_HOLB_MASK_DPS_TX_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_tx_holb_mask_dps_tx_1_s
{
  u32 producer_mask_32_64 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_tx_holb_mask_dps_tx_1_u
{
  struct ipa_hwio_def_ipa_state_tx_holb_mask_dps_tx_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_COAL_MASTER_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_coal_master_2_s
{
  u32 vp_timer_expired : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_coal_master_2_u
{
  struct ipa_hwio_def_ipa_state_coal_master_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_COAL_MASTER_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_coal_master_3_s
{
  u32 vp_vld : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_coal_master_3_u
{
  struct ipa_hwio_def_ipa_state_coal_master_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_TSP 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_tsp_s
{
  u32 traffic_shaper_idle : 1;
  u32 traffic_shaper_fifo_empty : 1;
  u32 queue_mngr_idle : 1;
  u32 queue_mngr_head_idle : 1;
  u32 queue_mngr_shared_idle : 1;
  u32 queue_mngr_tail_idle : 1;
  u32 queue_mngr_block_ctrl_idle : 1;
  u32 reserved0 : 1;
  u32 queue_mngr_block_full : 1;
  u32 reserved1 : 23;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_tsp_u
{
  struct ipa_hwio_def_ipa_state_tsp_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_AGGR_ACTIVE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_aggr_active_n_s
{
  u32 endpoints : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_aggr_active_n_u
{
  struct ipa_hwio_def_ipa_state_aggr_active_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_GSI_TLV_FIFO_EMPTY_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_gsi_tlv_fifo_empty_n_s
{
  u32 pipe_fifo_empty : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_gsi_tlv_fifo_empty_n_u
{
  struct ipa_hwio_def_ipa_state_gsi_tlv_fifo_empty_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_GSI_AOS_FIFO_EMPTY_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_gsi_aos_fifo_empty_n_s
{
  u32 pipe_fifo_empty : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_gsi_aos_fifo_empty_n_u
{
  struct ipa_hwio_def_ipa_state_gsi_aos_fifo_empty_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_DRBIP_DROP_STATE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_drbip_drop_state_n_s
{
  u32 consumer_pipe_drop_state : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_drbip_drop_state_n_u
{
  struct ipa_hwio_def_ipa_state_drbip_drop_state_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_DFETCHER_MASK_0_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_dfetcher_mask_0_n_s
{
  u32 mask_queue_dst_grp_dmar_outstanding : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_dfetcher_mask_0_n_u
{
  struct ipa_hwio_def_ipa_state_dfetcher_mask_0_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_DFETCHER_MASK_1_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_dfetcher_mask_1_n_s
{
  u32 mask_queue_no_resources_data_sectors : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_dfetcher_mask_1_n_u
{
  struct ipa_hwio_def_ipa_state_dfetcher_mask_1_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_DFETCHER_MASK_2_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_dfetcher_mask_2_n_s
{
  u32 mask_queue_no_resources_dps_dmar : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_dfetcher_mask_2_n_u
{
  struct ipa_hwio_def_ipa_state_dfetcher_mask_2_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_DFETCHER_MASK_3_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_dfetcher_mask_3_n_s
{
  u32 mask_queue_no_resources_seg_ctx : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_dfetcher_mask_3_n_u
{
  struct ipa_hwio_def_ipa_state_dfetcher_mask_3_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ACTIVE_PIPES_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_active_pipes_n_s
{
  u32 endpoints : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_active_pipes_n_u
{
  struct ipa_hwio_def_ipa_active_pipes_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_TX_HOLB_MASK_DPS_TX_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_tx_holb_mask_dps_tx_2_s
{
  u32 producer_mask_64_96 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_tx_holb_mask_dps_tx_2_u
{
  struct ipa_hwio_def_ipa_state_tx_holb_mask_dps_tx_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_TX_HOLB_MASK_NTF_TX_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_tx_holb_mask_ntf_tx_2_s
{
  u32 producer_mask_64_96 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_tx_holb_mask_ntf_tx_2_u
{
  struct ipa_hwio_def_ipa_state_tx_holb_mask_ntf_tx_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FILT_ROUT_CACHE_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_filt_rout_cache_cfg_s
{
  u32 ipa_router_cache_en : 1;
  u32 reserved0 : 3;
  u32 ipa_filter_cache_en : 1;
  u32 reserved1 : 11;
  u32 cache_lru_eviction_threshold : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_filt_rout_cache_cfg_u
{
  struct ipa_hwio_def_ipa_filt_rout_cache_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FILT_ROUT_CACHE_REDUCE_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_filt_rout_cache_reduce_cfg_s
{
  u32 ipa_router_cache_reduce_en : 1;
  u32 reserved0 : 3;
  u32 ipa_filter_cache_reduce_en : 1;
  u32 reserved1 : 3;
  u32 ipa_router_cache_reduce_level : 9;
  u32 reserved2 : 3;
  u32 ipa_filter_cache_reduce_level : 9;
  u32 reserved3 : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_filt_rout_cache_reduce_cfg_u
{
  struct ipa_hwio_def_ipa_filt_rout_cache_reduce_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FILT_ROUT_CACHE_FLUSH 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_filt_rout_cache_flush_s
{
  u32 ipa_router_cache_flush : 1;
  u32 reserved0 : 3;
  u32 ipa_filter_cache_flush : 1;
  u32 reserved1 : 27;
};

/* Union definition of register */
union ipa_hwio_def_ipa_filt_rout_cache_flush_u
{
  struct ipa_hwio_def_ipa_filt_rout_cache_flush_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FILT_ROUT_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_filt_rout_cfg_s
{
  u32 router_prefetch_en : 1;
  u32 reserved0 : 3;
  u32 filter_prefetch_en : 1;
  u32 reserved1 : 3;
  u32 filt_rout_data_cache_en : 1;
  u32 reserved2 : 23;
};

/* Union definition of register */
union ipa_hwio_def_ipa_filt_rout_cfg_u
{
  struct ipa_hwio_def_ipa_filt_rout_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV4_NAT_EXC_SUPPRESS_ROUT_TABLE_INDX 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv4_nat_exc_suppress_rout_table_indx_s
{
  u32 ip_v4_nat_exc_suppress_rout_table_indx : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv4_nat_exc_suppress_rout_table_indx_u
{
  struct ipa_hwio_def_ipa_ipv4_nat_exc_suppress_rout_table_indx_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV6_CONN_TRACK_EXC_SUPPRESS_ROUT_TABLE_INDX 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv6_conn_track_exc_suppress_rout_table_indx_s
{
  u32 ip_v6_conn_track_exc_suppress_rout_table_indx : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv6_conn_track_exc_suppress_rout_table_indx_u
{
  struct ipa_hwio_def_ipa_ipv6_conn_track_exc_suppress_rout_table_indx_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV4_FILTER_INIT_VALUES 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv4_filter_init_values_s
{
  u32 ip_v4_filter_init_hashed_addr : 16;
  u32 ip_v4_filter_init_non_hashed_addr : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv4_filter_init_values_u
{
  struct ipa_hwio_def_ipa_ipv4_filter_init_values_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV6_FILTER_INIT_VALUES 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv6_filter_init_values_s
{
  u32 ip_v6_filter_init_hashed_addr : 16;
  u32 ip_v6_filter_init_non_hashed_addr : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv6_filter_init_values_u
{
  struct ipa_hwio_def_ipa_ipv6_filter_init_values_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV4_NAT_INIT_VALUES_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv4_nat_init_values_0_s
{
  u32 ip_v4_nat_init_rules_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv4_nat_init_values_0_u
{
  struct ipa_hwio_def_ipa_ipv4_nat_init_values_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV4_NAT_INIT_VALUES_0_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv4_nat_init_values_0_msb_s
{
  u32 ip_v4_nat_init_rules_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv4_nat_init_values_0_msb_u
{
  struct ipa_hwio_def_ipa_ipv4_nat_init_values_0_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV4_NAT_INIT_VALUES_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv4_nat_init_values_1_s
{
  u32 ip_v4_nat_init_exp_rules_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv4_nat_init_values_1_u
{
  struct ipa_hwio_def_ipa_ipv4_nat_init_values_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV4_NAT_INIT_VALUES_1_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv4_nat_init_values_1_msb_s
{
  u32 ip_v4_nat_init_exp_rules_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv4_nat_init_values_1_msb_u
{
  struct ipa_hwio_def_ipa_ipv4_nat_init_values_1_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV4_NAT_INIT_VALUES_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv4_nat_init_values_2_s
{
  u32 ip_v4_nat_init_index_table_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv4_nat_init_values_2_u
{
  struct ipa_hwio_def_ipa_ipv4_nat_init_values_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV4_NAT_INIT_VALUES_2_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv4_nat_init_values_2_msb_s
{
  u32 ip_v4_nat_init_index_table_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv4_nat_init_values_2_msb_u
{
  struct ipa_hwio_def_ipa_ipv4_nat_init_values_2_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV4_NAT_INIT_VALUES_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv4_nat_init_values_3_s
{
  u32 ip_v4_nat_init_index_table_exp_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv4_nat_init_values_3_u
{
  struct ipa_hwio_def_ipa_ipv4_nat_init_values_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV4_NAT_INIT_VALUES_3_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv4_nat_init_values_3_msb_s
{
  u32 ip_v4_nat_init_index_table_exp_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv4_nat_init_values_3_msb_u
{
  struct ipa_hwio_def_ipa_ipv4_nat_init_values_3_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV4_NAT_INIT_VALUES_4 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv4_nat_init_values_4_s
{
  u32 ip_v4_nat_init_table_index : 3;
  u32 reserved0 : 1;
  u32 ip_v4_nat_init_rules_addr_type : 1;
  u32 ip_v4_nat_init_exp_rules_addr_type : 1;
  u32 ip_v4_nat_init_index_table_addr_type : 1;
  u32 ip_v4_nat_init_index_table_exp_addr_type : 1;
  u32 ip_v4_nat_init_size_base_tables : 4;
  u32 ip_v4_nat_init_size_exp_tables : 16;
  u32 reserved1 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv4_nat_init_values_4_u
{
  struct ipa_hwio_def_ipa_ipv4_nat_init_values_4_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV4_NAT_INIT_VALUES_5 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv4_nat_init_values_5_s
{
  u32 ip_v4_nat_init_pdn_config_table_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv4_nat_init_values_5_u
{
  struct ipa_hwio_def_ipa_ipv4_nat_init_values_5_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV4_ROUTE_INIT_VALUES 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv4_route_init_values_s
{
  u32 ip_v4_route_init_hashed_addr : 16;
  u32 ip_v4_route_init_non_hashed_addr : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv4_route_init_values_u
{
  struct ipa_hwio_def_ipa_ipv4_route_init_values_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV6_ROUTE_INIT_VALUES 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv6_route_init_values_s
{
  u32 ip_v6_route_init_hashed_addr : 16;
  u32 ip_v6_route_init_non_hashed_addr : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv6_route_init_values_u
{
  struct ipa_hwio_def_ipa_ipv6_route_init_values_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV6_CONN_TRACK_INIT_VALUES_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv6_conn_track_init_values_0_s
{
  u32 ip_v6_conn_track_init_table_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv6_conn_track_init_values_0_u
{
  struct ipa_hwio_def_ipa_ipv6_conn_track_init_values_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV6_CONN_TRACK_INIT_VALUES_0_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv6_conn_track_init_values_0_msb_s
{
  u32 ip_v6_conn_track_init_table_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv6_conn_track_init_values_0_msb_u
{
  struct ipa_hwio_def_ipa_ipv6_conn_track_init_values_0_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV6_CONN_TRACK_INIT_VALUES_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv6_conn_track_init_values_1_s
{
  u32 ip_v6_conn_track_init_exp_table_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv6_conn_track_init_values_1_u
{
  struct ipa_hwio_def_ipa_ipv6_conn_track_init_values_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV6_CONN_TRACK_INIT_VALUES_1_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv6_conn_track_init_values_1_msb_s
{
  u32 ip_v6_conn_track_init_exp_table_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv6_conn_track_init_values_1_msb_u
{
  struct ipa_hwio_def_ipa_ipv6_conn_track_init_values_1_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPV6_CONN_TRACK_INIT_VALUES_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipv6_conn_track_init_values_2_s
{
  u32 ip_v6_conn_track_init_table_index : 3;
  u32 reserved0 : 1;
  u32 ip_v6_conn_track_init_table_addr_type : 1;
  u32 ip_v6_conn_track_init_exp_table_addr_type : 1;
  u32 reserved1 : 2;
  u32 ip_v6_conn_track_init_size_base_tables : 4;
  u32 ip_v6_conn_track_init_size_exp_tables : 16;
  u32 reserved2 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipv6_conn_track_init_values_2_u
{
  struct ipa_hwio_def_ipa_ipv6_conn_track_init_values_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HDR_INIT_LOCAL_VALUES 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hdr_init_local_values_s
{
  u32 reserved0 : 12;
  u32 hdr_init_local_hdr_addr : 16;
  u32 reserved1 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hdr_init_local_values_u
{
  struct ipa_hwio_def_ipa_hdr_init_local_values_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HDR_INIT_SYSTEM_VALUES 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hdr_init_system_values_s
{
  u32 hdr_init_system_hdr_table_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hdr_init_system_values_u
{
  struct ipa_hwio_def_ipa_hdr_init_system_values_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HDR_INIT_SYSTEM_VALUES_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_hdr_init_system_values_msb_s
{
  u32 hdr_init_system_hdr_table_addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_hdr_init_system_values_msb_u
{
  struct ipa_hwio_def_ipa_hdr_init_system_values_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IMM_CMD_ACCESS_PIPE_VALUES 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_imm_cmd_access_pipe_values_s
{
  u32 imm_cmd_filter_router_pipe : 8;
  u32 imm_cmd_nat_pipe : 8;
  u32 imm_cmd_conn_track_pipe : 8;
  u32 imm_cmd_hdri_pipe : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_imm_cmd_access_pipe_values_u
{
  struct ipa_hwio_def_ipa_imm_cmd_access_pipe_values_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IMM_CMD_ACCESS_PIPE_VALUES_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_imm_cmd_access_pipe_values_1_s
{
  u32 imm_cmd_gen_pipe : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_imm_cmd_access_pipe_values_1_u
{
  struct ipa_hwio_def_ipa_imm_cmd_access_pipe_values_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SYS_PKT_PROC_CNTXT_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_sys_pkt_proc_cntxt_base_s
{
  u32 reserved0 : 3;
  u32 addr : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_sys_pkt_proc_cntxt_base_u
{
  struct ipa_hwio_def_ipa_sys_pkt_proc_cntxt_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SYS_PKT_PROC_CNTXT_BASE_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_sys_pkt_proc_cntxt_base_msb_s
{
  u32 addr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_sys_pkt_proc_cntxt_base_msb_u
{
  struct ipa_hwio_def_ipa_sys_pkt_proc_cntxt_base_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_LOCAL_PKT_PROC_CNTXT_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_local_pkt_proc_cntxt_base_s
{
  u32 reserved0 : 3;
  u32 addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_local_pkt_proc_cntxt_base_u
{
  struct ipa_hwio_def_ipa_local_pkt_proc_cntxt_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_AOS_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_aos_cfg_s
{
  u32 ipa_aos_tx_rx_priority : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_aos_cfg_u
{
  struct ipa_hwio_def_ipa_aos_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TX_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tx_cfg_s
{
  u32 reserved0 : 2;
  u32 prefetch_almost_empty_size_tx0 : 4;
  u32 dmaw_scnd_outsd_pred_threshold : 4;
  u32 dmaw_scnd_outsd_pred_en : 1;
  u32 dmaw_max_beats_256_dis : 1;
  u32 pa_mask_en : 1;
  u32 prefetch_almost_empty_size_tx1 : 4;
  u32 dual_tx_enable : 1;
  u32 sspnd_pa_no_start_state : 1;
  u32 reserved1 : 1;
  u32 holb_sticky_drop_en : 1;
  u32 reserved2 : 11;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tx_cfg_u
{
  struct ipa_hwio_def_ipa_tx_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NAT_UC_EXTERNAL_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nat_uc_external_cfg_s
{
  u32 ipa_nat_uc_external_table_addr_lsb : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nat_uc_external_cfg_u
{
  struct ipa_hwio_def_ipa_nat_uc_external_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NAT_UC_LOCAL_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nat_uc_local_cfg_s
{
  u32 ipa_nat_uc_local_table_addr_lsb : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nat_uc_local_cfg_u
{
  struct ipa_hwio_def_ipa_nat_uc_local_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NAT_UC_SHARED_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nat_uc_shared_cfg_s
{
  u32 ipa_nat_uc_external_table_addr_msb : 16;
  u32 ipa_nat_uc_local_table_addr_msb : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nat_uc_shared_cfg_u
{
  struct ipa_hwio_def_ipa_nat_uc_shared_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CONN_TRACK_UC_EXTERNAL_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_conn_track_uc_external_cfg_s
{
  u32 ipa_conn_track_uc_external_table_addr_lsb : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_conn_track_uc_external_cfg_u
{
  struct ipa_hwio_def_ipa_conn_track_uc_external_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CONN_TRACK_UC_LOCAL_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_conn_track_uc_local_cfg_s
{
  u32 ipa_conn_track_uc_local_table_addr_lsb : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_conn_track_uc_local_cfg_u
{
  struct ipa_hwio_def_ipa_conn_track_uc_local_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_CONN_TRACK_UC_SHARED_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_conn_track_uc_shared_cfg_s
{
  u32 ipa_conn_track_uc_external_table_addr_msb : 16;
  u32 ipa_conn_track_uc_local_table_addr_msb : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_conn_track_uc_shared_cfg_u
{
  struct ipa_hwio_def_ipa_conn_track_uc_shared_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IDLE_INDICATION_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_idle_indication_cfg_s
{
  u32 enter_idle_debounce_thresh : 16;
  u32 idle_indication_enable : 1;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_idle_indication_cfg_u
{
  struct ipa_hwio_def_ipa_idle_indication_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_QTIME_TIMESTAMP_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_qtime_timestamp_cfg_s
{
  u32 reserved0 : 8;
  u32 tag_timestamp_lsb : 5;
  u32 reserved1 : 3;
  u32 nat_timestamp_lsb : 5;
  u32 reserved2 : 11;
};

/* Union definition of register */
union ipa_hwio_def_ipa_qtime_timestamp_cfg_u
{
  struct ipa_hwio_def_ipa_qtime_timestamp_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TIMERS_XO_CLK_DIV_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_timers_xo_clk_div_cfg_s
{
  u32 value : 9;
  u32 reserved0 : 22;
  u32 enable : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_timers_xo_clk_div_cfg_u
{
  struct ipa_hwio_def_ipa_timers_xo_clk_div_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TIMERS_PULSE_GRAN_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_timers_pulse_gran_cfg_s
{
  u32 gran_0 : 3;
  u32 gran_1 : 3;
  u32 gran_2 : 3;
  u32 gran_3 : 3;
  u32 reserved0 : 20;
};

/* Union definition of register */
union ipa_hwio_def_ipa_timers_pulse_gran_cfg_u
{
  struct ipa_hwio_def_ipa_timers_pulse_gran_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_QTIME_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_qtime_lsb_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_qtime_lsb_u
{
  struct ipa_hwio_def_ipa_qtime_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_QTIME_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_qtime_msb_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_qtime_msb_u
{
  struct ipa_hwio_def_ipa_qtime_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ATOMIC_LOCK_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_atomic_lock_cfg_s
{
  u32 groups_to_mask : 7;
  u32 reserved0 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_atomic_lock_cfg_u
{
  struct ipa_hwio_def_ipa_atomic_lock_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_GENERIC_RAM_ARBITER_PRIORITY 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_generic_ram_arbiter_priority_s
{
  u32 rd_priority_valid : 1;
  u32 wr_priority_valid : 1;
  u32 reserved0 : 2;
  u32 rd_priority_index : 8;
  u32 wr_priority_index : 8;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_generic_ram_arbiter_priority_u
{
  struct ipa_hwio_def_ipa_generic_ram_arbiter_priority_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPL_TIMER_SW_ADJ_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dpl_timer_sw_adj_lsb_s
{
  u32 tod_offset_lsb : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dpl_timer_sw_adj_lsb_u
{
  struct ipa_hwio_def_ipa_dpl_timer_sw_adj_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DPL_TIMER_SW_ADJ_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dpl_timer_sw_adj_msb_s
{
  u32 tod_offset_msb : 20;
  u32 reserved0 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dpl_timer_sw_adj_msb_u
{
  struct ipa_hwio_def_ipa_dpl_timer_sw_adj_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SRC_RSRC_TYPE_01_CTX_N_DESC_LIST_GRP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_src_rsrc_type_01_ctx_n_desc_list_grp_n_s
{
  u32 rsrc_type_src_ctx_min_limit : 8;
  u32 rsrc_type_src_ctx_max_limit : 8;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_src_rsrc_type_01_ctx_n_desc_list_grp_n_u
{
  struct ipa_hwio_def_ipa_src_rsrc_type_01_ctx_n_desc_list_grp_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SRC_RSRC_TYPE_23_DESC_BUFFER_N_HPS_DMAR_GRP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_src_rsrc_type_23_desc_buffer_n_hps_dmar_grp_n_s
{
  u32 rsrc_type_src_desc_buffer_min_limit : 8;
  u32 rsrc_type_src_desc_buffer_max_limit : 8;
  u32 rsrc_type_src_hps_dmar_min_limit : 8;
  u32 rsrc_type_src_hps_dmar_max_limit : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_src_rsrc_type_23_desc_buffer_n_hps_dmar_grp_n_u
{
  struct ipa_hwio_def_ipa_src_rsrc_type_23_desc_buffer_n_hps_dmar_grp_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SRC_RSRC_TYPE_4_ACK_ENTRY_GRP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_src_rsrc_type_4_ack_entry_grp_n_s
{
  u32 rsrc_type_src_ack_entry_min_limit : 8;
  u32 rsrc_type_src_ack_entry_max_limit : 8;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_src_rsrc_type_4_ack_entry_grp_n_u
{
  struct ipa_hwio_def_ipa_src_rsrc_type_4_ack_entry_grp_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SRC_RSRC_TYPE_CNT_0_WOUT_ACK_ENTRY_GRP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_src_rsrc_type_cnt_0_wout_ack_entry_grp_n_s
{
  u32 rsrc_type_src_ctx_cnt : 9;
  u32 reserved0 : 3;
  u32 rsrc_type_src_desc_buffer_cnt : 9;
  u32 reserved1 : 2;
  u32 rsrc_type_src_hps_dmar_cnt : 9;
};

/* Union definition of register */
union ipa_hwio_def_ipa_src_rsrc_type_cnt_0_wout_ack_entry_grp_n_u
{
  struct ipa_hwio_def_ipa_src_rsrc_type_cnt_0_wout_ack_entry_grp_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SRC_RSRC_TYPE_CNT_1_ACK_ENTRY_GRP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_src_rsrc_type_cnt_1_ack_entry_grp_n_s
{
  u32 rsrc_type_src_ack_entry_cnt : 9;
  u32 reserved0 : 23;
};

/* Union definition of register */
union ipa_hwio_def_ipa_src_rsrc_type_cnt_1_ack_entry_grp_n_u
{
  struct ipa_hwio_def_ipa_src_rsrc_type_cnt_1_ack_entry_grp_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SRC_RSRC_TYPE_AMOUNT_0_WOUT_ACK_ENTRY 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_src_rsrc_type_amount_0_wout_ack_entry_s
{
  u32 rsrc_type_src_ctx_amount : 9;
  u32 reserved0 : 3;
  u32 rsrc_type_src_desc_buffer_amount : 9;
  u32 rsrc_type_src_hps_dmar_amount : 9;
  u32 reserved1 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_src_rsrc_type_amount_0_wout_ack_entry_u
{
  struct ipa_hwio_def_ipa_src_rsrc_type_amount_0_wout_ack_entry_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SRC_RSRC_TYPE_AMOUNT_1_ACK_ENTRY 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_src_rsrc_type_amount_1_ack_entry_s
{
  u32 rsrc_type_src_ack_entry_amount : 9;
  u32 reserved0 : 23;
};

/* Union definition of register */
union ipa_hwio_def_ipa_src_rsrc_type_amount_1_ack_entry_u
{
  struct ipa_hwio_def_ipa_src_rsrc_type_amount_1_ack_entry_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NAT_AND_CONNECTION_TRACKING_CACHE_TIMESTAMPS_DB_FLUSH 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nat_and_connection_tracking_cache_timestamps_db_flush_s
{
  u32 ipa_nat_and_conn_track_cache_timestamps_db_flush : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nat_and_connection_tracking_cache_timestamps_db_flush_u
{
  struct ipa_hwio_def_ipa_nat_and_connection_tracking_cache_timestamps_db_flush_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NAT_AND_CONNECTION_TRACKING_CACHE_STATUS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nat_and_connection_tracking_cache_status_s
{
  u32 ipa_nat_and_conn_track_cache_flush_done : 1;
  u32 ipa_nat_and_conn_track_cache_timestamps_db_flush_done : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nat_and_connection_tracking_cache_status_u
{
  struct ipa_hwio_def_ipa_nat_and_connection_tracking_cache_status_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_NAT_AND_CONN_TRACK_MAX_COUNTERS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_nat_and_conn_track_max_counters_s
{
  u32 nat_and_conn_track_ipv4_max_counter : 16;
  u32 nat_and_conn_track_ipv6_max_counter : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_nat_and_conn_track_max_counters_u
{
  struct ipa_hwio_def_ipa_stat_nat_and_conn_track_max_counters_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_NAT_AND_CONN_TRACK_IPV4_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_nat_and_conn_track_ipv4_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_nat_and_conn_track_ipv4_base_u
{
  struct ipa_hwio_def_ipa_stat_nat_and_conn_track_ipv4_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_NAT_AND_CONN_TRACK_IPV6_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_nat_and_conn_track_ipv6_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_nat_and_conn_track_ipv6_base_u
{
  struct ipa_hwio_def_ipa_stat_nat_and_conn_track_ipv6_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NAT_AND_CONNECTION_TRACKING_CACHE_FLUSH 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nat_and_connection_tracking_cache_flush_s
{
  u32 ipa_nat_and_conn_track_cache_flush : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nat_and_connection_tracking_cache_flush_u
{
  struct ipa_hwio_def_ipa_nat_and_connection_tracking_cache_flush_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DST_RSRC_TYPE_AMOUNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dst_rsrc_type_amount_s
{
  u32 rsrc_type_dst_data_sctr_amount : 9;
  u32 reserved0 : 3;
  u32 rsrc_type_dst_dps_dmar_amount : 9;
  u32 rsrc_type_dst_seg_ctx_amount : 9;
  u32 reserved1 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dst_rsrc_type_amount_u
{
  struct ipa_hwio_def_ipa_dst_rsrc_type_amount_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NAT_AND_CONN_TRACK_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nat_and_conn_track_cfg_s
{
  u32 ipa_nat_and_conn_track_cache_en : 1;
  u32 ipa_nat_and_conn_track_directional_fin_rst_en : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nat_and_conn_track_cfg_u
{
  struct ipa_hwio_def_ipa_nat_and_conn_track_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NAT_AND_CONNECTION_TRACKING_CACHE_REDUCE_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nat_and_connection_tracking_cache_reduce_cfg_s
{
  u32 ipa_nat_and_conn_track_cache_reduce_en : 1;
  u32 reserved0 : 7;
  u32 ipa_nat_and_conn_track_cache_reduce_level : 8;
  u32 reserved1 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nat_and_connection_tracking_cache_reduce_cfg_u
{
  struct ipa_hwio_def_ipa_nat_and_connection_tracking_cache_reduce_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RSRC_GRP_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rsrc_grp_cfg_s
{
  u32 src_grp_special_valid : 1;
  u32 reserved0 : 3;
  u32 src_grp_special_index : 4;
  u32 dst_pipe_special_valid : 1;
  u32 reserved1 : 3;
  u32 dst_pipe_special_index : 8;
  u32 dst_grp_special_valid : 1;
  u32 reserved2 : 3;
  u32 dst_grp_special_index : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rsrc_grp_cfg_u
{
  struct ipa_hwio_def_ipa_rsrc_grp_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RSRC_GRP_CFG_EXT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_rsrc_grp_cfg_ext_s
{
  u32 src_grp_2nd_priority_special_valid : 1;
  u32 reserved0 : 3;
  u32 src_grp_2nd_priority_special_index : 4;
  u32 reserved1 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_rsrc_grp_cfg_ext_u
{
  struct ipa_hwio_def_ipa_rsrc_grp_cfg_ext_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_AXI_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_axi_cfg_s
{
  u32 relaxed_ordering_gsi_rd : 1;
  u32 relaxed_ordering_gsi_wr : 1;
  u32 relaxed_ordering_ipa_rd : 1;
  u32 relaxed_ordering_ipa_wr : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_axi_cfg_u
{
  struct ipa_hwio_def_ipa_axi_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_AGGR_FORCE_CLOSE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_aggr_force_close_n_s
{
  u32 aggr_force_close_pipe_bitmap : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_aggr_force_close_n_u
{
  struct ipa_hwio_def_ipa_aggr_force_close_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_QUOTA_BASE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_quota_base_n_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_quota_base_n_u
{
  struct ipa_hwio_def_ipa_stat_quota_base_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_TETHERING_BASE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_tethering_base_n_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_tethering_base_n_u
{
  struct ipa_hwio_def_ipa_stat_tethering_base_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_DROP_CNT_BASE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_drop_cnt_base_n_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_drop_cnt_base_n_u
{
  struct ipa_hwio_def_ipa_stat_drop_cnt_base_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_SHPR_TC_DROP_STATS_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_shpr_tc_drop_stats_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_shpr_tc_drop_stats_base_u
{
  struct ipa_hwio_def_ipa_tsp_shpr_tc_drop_stats_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_QUOTA_CFG_BASE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_quota_cfg_base_n_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_quota_cfg_base_n_u
{
  struct ipa_hwio_def_ipa_stat_quota_cfg_base_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_TETHERING_CFG_BASE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_tethering_cfg_base_n_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_tethering_cfg_base_n_u
{
  struct ipa_hwio_def_ipa_stat_tethering_cfg_base_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_FILTER_IPV4_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_filter_ipv4_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_filter_ipv4_base_u
{
  struct ipa_hwio_def_ipa_stat_filter_ipv4_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_FILTER_IPV6_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_filter_ipv6_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_filter_ipv6_base_u
{
  struct ipa_hwio_def_ipa_stat_filter_ipv6_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_FILTER_NONIP_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_filter_nonip_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_filter_nonip_base_u
{
  struct ipa_hwio_def_ipa_stat_filter_nonip_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_ROUTER_IPV4_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_router_ipv4_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_router_ipv4_base_u
{
  struct ipa_hwio_def_ipa_stat_router_ipv4_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_ROUTER_IPV6_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_router_ipv6_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_router_ipv6_base_u
{
  struct ipa_hwio_def_ipa_stat_router_ipv6_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_ROUTER_NONIP_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_router_nonip_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_router_nonip_base_u
{
  struct ipa_hwio_def_ipa_stat_router_nonip_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_IPV4_FILTER_ROUTER_MAX_COUNTERS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_ipv4_filter_router_max_counters_s
{
  u32 router : 16;
  u32 filter : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_ipv4_filter_router_max_counters_u
{
  struct ipa_hwio_def_ipa_stat_ipv4_filter_router_max_counters_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_IPV6_FILTER_ROUTER_MAX_COUNTERS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_ipv6_filter_router_max_counters_s
{
  u32 router : 16;
  u32 filter : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_ipv6_filter_router_max_counters_u
{
  struct ipa_hwio_def_ipa_stat_ipv6_filter_router_max_counters_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_NONIP_FILTER_ROUTER_MAX_COUNTERS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_nonip_filter_router_max_counters_s
{
  u32 router : 16;
  u32 filter : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_nonip_filter_router_max_counters_u
{
  struct ipa_hwio_def_ipa_stat_nonip_filter_router_max_counters_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_NAT_AND_CONN_TRACK_IPV4_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_nat_and_conn_track_ipv4_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_nat_and_conn_track_ipv4_cfg_base_u
{
  struct ipa_hwio_def_ipa_stat_nat_and_conn_track_ipv4_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_NAT_AND_CONN_TRACK_IPV6_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_nat_and_conn_track_ipv6_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_nat_and_conn_track_ipv6_cfg_base_u
{
  struct ipa_hwio_def_ipa_stat_nat_and_conn_track_ipv6_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_FILTER_IPV4_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_filter_ipv4_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_filter_ipv4_cfg_base_u
{
  struct ipa_hwio_def_ipa_stat_filter_ipv4_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_FILTER_IPV6_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_filter_ipv6_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_filter_ipv6_cfg_base_u
{
  struct ipa_hwio_def_ipa_stat_filter_ipv6_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_FILTER_NONIP_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_filter_nonip_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_filter_nonip_cfg_base_u
{
  struct ipa_hwio_def_ipa_stat_filter_nonip_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_ROUTER_IPV4_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_router_ipv4_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_router_ipv4_cfg_base_u
{
  struct ipa_hwio_def_ipa_stat_router_ipv4_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_ROUTER_IPV6_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_router_ipv6_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_router_ipv6_cfg_base_u
{
  struct ipa_hwio_def_ipa_stat_router_ipv6_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_ROUTER_NONIP_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_router_nonip_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_router_nonip_cfg_base_u
{
  struct ipa_hwio_def_ipa_stat_router_nonip_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_QUOTA_MASK_EE_n_REG_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_quota_mask_ee_n_reg_k_s
{
  u32 pipe_mask : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_quota_mask_ee_n_reg_k_u
{
  struct ipa_hwio_def_ipa_stat_quota_mask_ee_n_reg_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_TETHERING_MASK_EE_n_REG_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_tethering_mask_ee_n_reg_k_s
{
  u32 pipe_mask : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_tethering_mask_ee_n_reg_k_u
{
  struct ipa_hwio_def_ipa_stat_tethering_mask_ee_n_reg_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STAT_DROP_CNT_MASK_EE_n_REG_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_stat_drop_cnt_mask_ee_n_reg_k_s
{
  u32 pipe_mask : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_stat_drop_cnt_mask_ee_n_reg_k_u
{
  struct ipa_hwio_def_ipa_stat_drop_cnt_mask_ee_n_reg_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NLO_PP_CFG1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nlo_pp_cfg1_s
{
  u32 nlo_ack_pp : 8;
  u32 nlo_data_pp : 8;
  u32 nlo_status_pp : 8;
  u32 nlo_ack_max_vp : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nlo_pp_cfg1_u
{
  struct ipa_hwio_def_ipa_nlo_pp_cfg1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NLO_PP_CFG2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nlo_pp_cfg2_s
{
  u32 nlo_ack_close_padd : 8;
  u32 nlo_data_close_padd : 8;
  u32 nlo_ack_buffer_mode : 1;
  u32 nlo_data_buffer_mode : 1;
  u32 nlo_status_buffer_mode : 1;
  u32 reserved0 : 13;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nlo_pp_cfg2_u
{
  struct ipa_hwio_def_ipa_nlo_pp_cfg2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NLO_MIN_DSM_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nlo_min_dsm_cfg_s
{
  u32 nlo_ack_min_dsm_len : 16;
  u32 nlo_data_min_dsm_len : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nlo_min_dsm_cfg_u
{
  struct ipa_hwio_def_ipa_nlo_min_dsm_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NLO_VP_AGGR_CFG_LSB_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nlo_vp_aggr_cfg_lsb_n_s
{
  u32 vp_pkt_limit : 6;
  u32 vp_time_limit : 5;
  u32 vp_byte_limit : 6;
  u32 vp_hard_byte_limit_en : 1;
  u32 vp_aggr_gran_sel : 1;
  u32 reserved0 : 13;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nlo_vp_aggr_cfg_lsb_n_u
{
  struct ipa_hwio_def_ipa_nlo_vp_aggr_cfg_lsb_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NLO_VP_LIMIT_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nlo_vp_limit_cfg_n_s
{
  u32 lower_size : 16;
  u32 upper_size : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nlo_vp_limit_cfg_n_u
{
  struct ipa_hwio_def_ipa_nlo_vp_limit_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NLO_VP_FLUSH_REQ 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nlo_vp_flush_req_s
{
  u32 vp_flush_pp_indx : 8;
  u32 reserved0 : 8;
  u32 vp_flush_vp_indx : 8;
  u32 reserved1 : 7;
  u32 vp_flush_req : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nlo_vp_flush_req_u
{
  struct ipa_hwio_def_ipa_nlo_vp_flush_req_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NLO_VP_FLUSH_COOKIE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nlo_vp_flush_cookie_s
{
  u32 vp_flush_cookie : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nlo_vp_flush_cookie_u
{
  struct ipa_hwio_def_ipa_nlo_vp_flush_cookie_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NLO_VP_FLUSH_ACK 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nlo_vp_flush_ack_s
{
  u32 vp_flush_ack : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nlo_vp_flush_ack_u
{
  struct ipa_hwio_def_ipa_nlo_vp_flush_ack_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NLO_VP_DSM_OPEN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nlo_vp_dsm_open_s
{
  u32 vp_dsm_open : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nlo_vp_dsm_open_u
{
  struct ipa_hwio_def_ipa_nlo_vp_dsm_open_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_NLO_VP_QBAP_OPEN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_nlo_vp_qbap_open_s
{
  u32 vp_qbap_open : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_nlo_vp_qbap_open_u
{
  struct ipa_hwio_def_ipa_nlo_vp_qbap_open_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_COAL_MASTER_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_coal_master_cfg_s
{
  u32 coal_force_to_default : 1;
  u32 coal_enhanced_ipv4_id_en : 1;
  u32 coal_ipv4_id_ignore : 1;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_coal_master_cfg_u
{
  struct ipa_hwio_def_ipa_coal_master_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_COAL_EVICT_LRU 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_coal_evict_lru_s
{
  u32 coal_eviction_en : 1;
  u32 coal_vp_lru_gran_sel : 2;
  u32 coal_vp_lru_udp_thrshld : 5;
  u32 coal_vp_lru_tcp_thrshld : 5;
  u32 coal_vp_lru_udp_thrshld_en : 1;
  u32 coal_vp_lru_tcp_thrshld_en : 1;
  u32 coal_vp_lru_tcp_num : 5;
  u32 reserved0 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_coal_evict_lru_u
{
  struct ipa_hwio_def_ipa_coal_evict_lru_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_SNIFFER_QMB_SEL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_sniffer_qmb_sel_s
{
  u32 snif_qmb_sel : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_sniffer_qmb_sel_u
{
  struct ipa_hwio_def_ipa_sniffer_qmb_sel_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ULSO_CFG_IP_ID_MAX_VALUE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ulso_cfg_ip_id_max_value_n_s
{
  u32 ip_id_max_value : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ulso_cfg_ip_id_max_value_n_u
{
  struct ipa_hwio_def_ipa_ulso_cfg_ip_id_max_value_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ULSO_CFG_IP_ID_MIN_VALUE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ulso_cfg_ip_id_min_value_n_s
{
  u32 ip_id_min_value : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ulso_cfg_ip_id_min_value_n_u
{
  struct ipa_hwio_def_ipa_ulso_cfg_ip_id_min_value_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_DFETCHER_MASK_4_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_dfetcher_mask_4_n_s
{
  u32 mask_queue_prod_dpl_fifo_full : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_dfetcher_mask_4_n_u
{
  struct ipa_hwio_def_ipa_state_dfetcher_mask_4_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DL_NLO_PP_CFG_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dl_nlo_pp_cfg_0_s
{
  u32 dl_nlo_0_indx : 8;
  u32 dl_nlo_0_starvation : 8;
  u32 dl_nlo_0_en : 1;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dl_nlo_pp_cfg_0_u
{
  struct ipa_hwio_def_ipa_dl_nlo_pp_cfg_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DL_NLO_PP_CFG_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dl_nlo_pp_cfg_1_s
{
  u32 dl_nlo_1_indx : 8;
  u32 dl_nlo_1_starvation : 8;
  u32 dl_nlo_1_en : 1;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dl_nlo_pp_cfg_1_u
{
  struct ipa_hwio_def_ipa_dl_nlo_pp_cfg_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DL_NLO_PP_CFG_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dl_nlo_pp_cfg_2_s
{
  u32 dl_nlo_2_indx : 8;
  u32 dl_nlo_2_starvation : 8;
  u32 dl_nlo_2_en : 1;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dl_nlo_pp_cfg_2_u
{
  struct ipa_hwio_def_ipa_dl_nlo_pp_cfg_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_POLICER_DB_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_policer_db_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_policer_db_base_u
{
  struct ipa_hwio_def_ipa_tsp_policer_db_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_SHPR_PROD_DB_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_shpr_prod_db_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_shpr_prod_db_base_u
{
  struct ipa_hwio_def_ipa_tsp_shpr_prod_db_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_SHPR_TC_DB_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_shpr_tc_db_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_shpr_tc_db_base_u
{
  struct ipa_hwio_def_ipa_tsp_shpr_tc_db_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_SHPR_TC_STATS_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_shpr_tc_stats_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_shpr_tc_stats_base_u
{
  struct ipa_hwio_def_ipa_tsp_shpr_tc_stats_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_SHPR_TC_STATS_MASK_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_shpr_tc_stats_mask_n_s
{
  u32 tc_mask : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_shpr_tc_stats_mask_n_u
{
  struct ipa_hwio_def_ipa_tsp_shpr_tc_stats_mask_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_TETHERING_BASE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_tethering_base_n_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_tethering_base_n_u
{
  struct ipa_hwio_def_ipa_drop_stat_tethering_base_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_TETHERING_CFG_BASE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_tethering_cfg_base_n_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_tethering_cfg_base_n_u
{
  struct ipa_hwio_def_ipa_drop_stat_tethering_cfg_base_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_FILTER_IPV4_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_filter_ipv4_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_filter_ipv4_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_filter_ipv4_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_FILTER_IPV6_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_filter_ipv6_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_filter_ipv6_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_filter_ipv6_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_FILTER_NONIP_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_filter_nonip_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_filter_nonip_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_filter_nonip_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_ROUTER_IPV4_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_router_ipv4_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_router_ipv4_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_router_ipv4_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_ROUTER_IPV6_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_router_ipv6_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_router_ipv6_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_router_ipv6_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_ROUTER_NONIP_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_router_nonip_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_router_nonip_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_router_nonip_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DST_RSRC_TYPE_01_DATA_SCTR_N_DPS_DMAR_GRP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dst_rsrc_type_01_data_sctr_n_dps_dmar_grp_n_s
{
  u32 rsrc_type_dst_data_sctr_min_limit : 8;
  u32 rsrc_type_dst_data_sctr_max_limit : 8;
  u32 rsrc_type_dst_dps_dmar_min_limit : 8;
  u32 rsrc_type_dst_dps_dmar_max_limit : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dst_rsrc_type_01_data_sctr_n_dps_dmar_grp_n_u
{
  struct ipa_hwio_def_ipa_dst_rsrc_type_01_data_sctr_n_dps_dmar_grp_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DST_RSRC_TYPE_2_SEG_CTX_GRP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dst_rsrc_type_2_seg_ctx_grp_n_s
{
  u32 rsrc_type_dst_seg_ctx_min_limit : 8;
  u32 rsrc_type_dst_seg_ctx_max_limit : 8;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dst_rsrc_type_2_seg_ctx_grp_n_u
{
  struct ipa_hwio_def_ipa_dst_rsrc_type_2_seg_ctx_grp_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DST_RSRC_TYPE_CNT_GRP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dst_rsrc_type_cnt_grp_n_s
{
  u32 rsrc_type_dst_data_sctr_cnt : 9;
  u32 reserved0 : 3;
  u32 rsrc_type_dst_dps_dmar_cnt : 9;
  u32 reserved1 : 2;
  u32 rsrc_type_dst_seg_ctx_cnt : 9;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dst_rsrc_type_cnt_grp_n_u
{
  struct ipa_hwio_def_ipa_dst_rsrc_type_cnt_grp_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_FILTER_IPV4_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_filter_ipv4_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_filter_ipv4_cfg_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_filter_ipv4_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_FILTER_IPV6_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_filter_ipv6_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_filter_ipv6_cfg_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_filter_ipv6_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_FILTER_NONIP_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_filter_nonip_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_filter_nonip_cfg_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_filter_nonip_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_ROUTER_IPV4_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_router_ipv4_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_router_ipv4_cfg_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_router_ipv4_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_ROUTER_IPV6_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_router_ipv6_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_router_ipv6_cfg_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_router_ipv6_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_ROUTER_NONIP_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_router_nonip_cfg_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_router_nonip_cfg_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_router_nonip_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_IPV4_FILTER_ROUTER_MAX_COUNTERS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_ipv4_filter_router_max_counters_s
{
  u32 router : 16;
  u32 filter : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_ipv4_filter_router_max_counters_u
{
  struct ipa_hwio_def_ipa_drop_stat_ipv4_filter_router_max_counters_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_IPV6_FILTER_ROUTER_MAX_COUNTERS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_ipv6_filter_router_max_counters_s
{
  u32 router : 16;
  u32 filter : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_ipv6_filter_router_max_counters_u
{
  struct ipa_hwio_def_ipa_drop_stat_ipv6_filter_router_max_counters_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_NONIP_FILTER_ROUTER_MAX_COUNTERS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_nonip_filter_router_max_counters_s
{
  u32 router : 16;
  u32 filter : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_nonip_filter_router_max_counters_u
{
  struct ipa_hwio_def_ipa_drop_stat_nonip_filter_router_max_counters_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_NAT_AND_CONN_TRACK_MAX_COUNTERS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_max_counters_s
{
  u32 nat_and_conn_track_ipv4_max_counter : 16;
  u32 nat_and_conn_track_ipv6_max_counter : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_max_counters_u
{
  struct ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_max_counters_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_NAT_AND_CONN_TRACK_IPV4_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_ipv4_base_s
{
  u32 base_addr_offset : 3;
  u32 base_addr : 17;
  u32 reserved0 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_ipv4_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_ipv4_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_NAT_AND_CONN_TRACK_IPV6_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_ipv6_base_s
{
  u32 base_addr_offset : 3;
  u32 base_addr : 17;
  u32 reserved0 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_ipv6_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_ipv6_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_NAT_AND_CONN_TRACK_IPV4_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_ipv4_cfg_base_s
{
  u32 base_addr_offset : 3;
  u32 base_addr : 17;
  u32 reserved0 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_ipv4_cfg_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_ipv4_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_NAT_AND_CONN_TRACK_IPV6_CFG_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_ipv6_cfg_base_s
{
  u32 base_addr_offset : 3;
  u32 base_addr : 17;
  u32 reserved0 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_ipv6_cfg_base_u
{
  struct ipa_hwio_def_ipa_drop_stat_nat_and_conn_track_ipv6_cfg_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DROP_STAT_TETHERING_MASK_EE_n_REG_k 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_drop_stat_tethering_mask_ee_n_reg_k_s
{
  u32 pipe_mask : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_drop_stat_tethering_mask_ee_n_reg_k_u
{
  struct ipa_hwio_def_ipa_drop_stat_tethering_mask_ee_n_reg_k_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_QM_EXTERNAL_BADDR_LSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_qm_external_baddr_lsb_s
{
  u32 reserved0 : 10;
  u32 baddr_lsbs : 22;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_qm_external_baddr_lsb_u
{
  struct ipa_hwio_def_ipa_tsp_qm_external_baddr_lsb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_QM_EXTERNAL_BADDR_MSB 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_qm_external_baddr_msb_s
{
  u32 baddr_msbs : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_qm_external_baddr_msb_u
{
  struct ipa_hwio_def_ipa_tsp_qm_external_baddr_msb_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_QM_EXTERNAL_SIZE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_qm_external_size_s
{
  u32 size_in_64_blocks_resolution : 12;
  u32 reserved0 : 20;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_qm_external_size_u
{
  struct ipa_hwio_def_ipa_tsp_qm_external_size_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_INGRESS_POLICING_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_ingress_policing_cfg_n_s
{
  u32 include_l2_len_per_traffic_class_bitmap : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_ingress_policing_cfg_n_u
{
  struct ipa_hwio_def_ipa_tsp_ingress_policing_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_INGRESS_POLICING_CFG_ETH_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_ingress_policing_cfg_eth_n_s
{
  u32 include_eth_hdr_len_per_traffic_class_bitmap : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_ingress_policing_cfg_eth_n_u
{
  struct ipa_hwio_def_ipa_tsp_ingress_policing_cfg_eth_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_EGRESS_POLICING_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_egress_policing_cfg_n_s
{
  u32 disable_guaranteed_rate_per_traffic_class_bitmap : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_egress_policing_cfg_n_u
{
  struct ipa_hwio_def_ipa_tsp_egress_policing_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_QMNGR_QUEUE_NONEMPTY_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_qmngr_queue_nonempty_n_s
{
  u32 queue_nonempty_bitmap : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_qmngr_queue_nonempty_n_u
{
  struct ipa_hwio_def_ipa_state_qmngr_queue_nonempty_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_HALTED_TRAFFIC_CLASS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_halted_traffic_class_n_s
{
  u32 halt_tc_bitmap : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_halted_traffic_class_n_u
{
  struct ipa_hwio_def_ipa_tsp_halted_traffic_class_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_FLUSHED_TRAFFIC_CLASS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_flushed_traffic_class_n_s
{
  u32 flushed_tc_bitmap : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_flushed_traffic_class_n_u
{
  struct ipa_hwio_def_ipa_tsp_flushed_traffic_class_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_FLUSHED_TC_CLEAR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_flushed_tc_clear_n_s
{
  u32 flushed_clr_bitmap : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_flushed_tc_clear_n_u
{
  struct ipa_hwio_def_ipa_tsp_flushed_tc_clear_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_MAX_QUEUE_PKT_CNT_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_max_queue_pkt_cnt_n_s
{
  u32 pkt_size : 24;
  u32 reserved0 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_max_queue_pkt_cnt_n_u
{
  struct ipa_hwio_def_ipa_tsp_max_queue_pkt_cnt_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_MAX_QUEUE_BYTES_CNT_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_max_queue_bytes_cnt_n_s
{
  u32 bytes_size : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_max_queue_bytes_cnt_n_u
{
  struct ipa_hwio_def_ipa_tsp_max_queue_bytes_cnt_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_DROP_TIMER_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_drop_timer_n_s
{
  u32 time_limit : 5;
  u32 reserved0 : 3;
  u32 gran_sel : 2;
  u32 reserved1 : 22;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_drop_timer_n_u
{
  struct ipa_hwio_def_ipa_tsp_drop_timer_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_QUEUE_PKT_COUNTER_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_queue_pkt_counter_n_s
{
  u32 pkt_count : 24;
  u32 reserved0 : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_queue_pkt_counter_n_u
{
  struct ipa_hwio_def_ipa_tsp_queue_pkt_counter_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_QUEUE_BYTES_COUNTER_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_queue_bytes_counter_n_s
{
  u32 bytes_count : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_queue_bytes_counter_n_u
{
  struct ipa_hwio_def_ipa_tsp_queue_bytes_counter_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_SHPR_TC_DROP_STATS_MASK_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_shpr_tc_drop_stats_mask_n_s
{
  u32 tc_mask : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_shpr_tc_drop_stats_mask_n_u
{
  struct ipa_hwio_def_ipa_tsp_shpr_tc_drop_stats_mask_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_HOLB_DROP_IRQ_PER_TC_MASK_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_holb_drop_irq_per_tc_mask_n_s
{
  u32 tc_mask : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_holb_drop_irq_per_tc_mask_n_u
{
  struct ipa_hwio_def_ipa_holb_drop_irq_per_tc_mask_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_TC_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_tc_cfg_n_s
{
  u32 tc_priority : 3;
  u32 tsp_producer_index : 5;
  u32 tc_enable : 1;
  u32 reserved0 : 23;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_tc_cfg_n_u
{
  struct ipa_hwio_def_ipa_tsp_tc_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_STATE_PROD_DPL_FIFO 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_state_prod_dpl_fifo_s
{
  u32 pop_fsm_state : 3;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_state_prod_dpl_fifo_u
{
  struct ipa_hwio_def_ipa_state_prod_dpl_fifo_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_POLICER_TC_DROP_STATS_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_policer_tc_drop_stats_base_s
{
  u32 reserved0 : 3;
  u32 base_addr : 17;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_policer_tc_drop_stats_base_u
{
  struct ipa_hwio_def_ipa_tsp_policer_tc_drop_stats_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_TSP_POLICER_TC_DROP_STATS_MASK_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_tsp_policer_tc_drop_stats_mask_n_s
{
  u32 tsp_drop_ingress_tc_stat_mask_bitmap : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_tsp_policer_tc_drop_stats_mask_n_u
{
  struct ipa_hwio_def_ipa_tsp_policer_tc_drop_stats_mask_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DSA_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dsa_cfg_s
{
  u32 dsa_tag_val : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dsa_cfg_u
{
  struct ipa_hwio_def_ipa_dsa_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_DSA_FNR_CACHE_CFG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_dsa_fnr_cache_cfg_s
{
  u32 dsa_payload_mask : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_dsa_fnr_cache_cfg_u
{
  struct ipa_hwio_def_ipa_dsa_fnr_cache_cfg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPSEC_SA_DECAPSULATION_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipsec_sa_decapsulation_base_s
{
  u32 base_addr_reserved : 4;
  u32 base_addr : 16;
  u32 reserved0 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipsec_sa_decapsulation_base_u
{
  struct ipa_hwio_def_ipa_ipsec_sa_decapsulation_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_IPSEC_SA_ENCAPSULATION_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ipsec_sa_encapsulation_base_s
{
  u32 base_addr_reserved : 4;
  u32 base_addr : 16;
  u32 reserved0 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ipsec_sa_encapsulation_base_u
{
  struct ipa_hwio_def_ipa_ipsec_sa_encapsulation_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_WIREGUARD_SA_DECAPSULATION_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_wireguard_sa_decapsulation_base_s
{
  u32 base_addr_reserved : 4;
  u32 base_addr : 16;
  u32 reserved0 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_wireguard_sa_decapsulation_base_u
{
  struct ipa_hwio_def_ipa_wireguard_sa_decapsulation_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_WIREGUARD_SA_ENCAPSULATION_BASE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_wireguard_sa_encapsulation_base_s
{
  u32 base_addr_reserved : 4;
  u32 base_addr : 16;
  u32 reserved0 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_wireguard_sa_encapsulation_base_u
{
  struct ipa_hwio_def_ipa_wireguard_sa_encapsulation_base_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_CTRL_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_ctrl_n_s
{
  u32 reserved0 : 1;
  u32 endp_delay : 1;
  u32 reserved1 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_ctrl_n_u
{
  struct ipa_hwio_def_ipa_endp_init_ctrl_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_CTRL_SCND_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_ctrl_scnd_n_s
{
  u32 reserved0 : 1;
  u32 endp_delay : 1;
  u32 reserved1 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_ctrl_scnd_n_u
{
  struct ipa_hwio_def_ipa_endp_init_ctrl_scnd_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_cfg_n_s
{
  u32 frag_offload_en : 1;
  u32 cs_offload_en : 2;
  u32 cs_metadata_hdr_offset : 4;
  u32 reserved0 : 1;
  u32 gen_qmb_master_sel : 1;
  u32 pipe_replicate_en : 1;
  u32 mux_id_index : 2;
  u32 reserved1 : 20;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_cfg_n_u
{
  struct ipa_hwio_def_ipa_endp_init_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_HDR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_hdr_n_s
{
  u32 hdr_len : 6;
  u32 hdr_ofst_metadata_valid : 1;
  u32 hdr_ofst_metadata : 6;
  u32 hdr_additional_const_len : 6;
  u32 hdr_ofst_pkt_size_valid : 1;
  u32 hdr_ofst_pkt_size : 6;
  u32 reserved0 : 1;
  u32 hdr_len_inc_deagg_hdr : 1;
  u32 hdr_len_msb : 2;
  u32 hdr_ofst_metadata_msb : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_hdr_n_u
{
  struct ipa_hwio_def_ipa_endp_init_hdr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_HDR_EXT_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_hdr_ext_n_s
{
  u32 hdr_endianess : 1;
  u32 hdr_total_len_or_pad_valid : 1;
  u32 hdr_total_len_or_pad : 1;
  u32 hdr_payload_len_inc_padding : 1;
  u32 hdr_total_len_or_pad_offset : 6;
  u32 hdr_pad_to_alignment : 4;
  u32 hdr_payload_len_inc_eth : 1;
  u32 reserved0 : 1;
  u32 hdr_total_len_or_pad_offset_msb : 2;
  u32 hdr_ofst_pkt_size_msb : 2;
  u32 hdr_additional_const_len_msb : 2;
  u32 hdr_bytes_to_remove_valid : 1;
  u32 reserved1 : 1;
  u32 hdr_bytes_to_remove : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_hdr_ext_n_u
{
  struct ipa_hwio_def_ipa_endp_init_hdr_ext_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_HDR_METADATA_MASK_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_hdr_metadata_mask_n_s
{
  u32 metadata_mask : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_hdr_metadata_mask_n_u
{
  struct ipa_hwio_def_ipa_endp_init_hdr_metadata_mask_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_HDR_METADATA_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_hdr_metadata_n_s
{
  u32 metadata : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_hdr_metadata_n_u
{
  struct ipa_hwio_def_ipa_endp_init_hdr_metadata_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_MODE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_mode_n_s
{
  u32 mode : 3;
  u32 bearer_cntx_enable : 1;
  u32 dest_pipe_index : 8;
  u32 byte_threshold : 16;
  u32 filter_table_debug_en : 1;
  u32 pad_en : 1;
  u32 drbip_acl_enable : 1;
  u32 ipv4_checksum_check_en : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_mode_n_u
{
  struct ipa_hwio_def_ipa_endp_init_mode_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_AGGR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_aggr_n_s
{
  u32 aggr_en : 2;
  u32 aggr_type : 3;
  u32 aggr_byte_limit : 6;
  u32 reserved0 : 1;
  u32 aggr_time_limit : 5;
  u32 aggr_pkt_limit : 6;
  u32 aggr_sw_eof_active : 1;
  u32 aggr_force_close : 1;
  u32 reserved1 : 1;
  u32 aggr_hard_byte_limit_enable : 1;
  u32 aggr_gran_sel : 1;
  u32 aggr_coal_l2 : 1;
  u32 reserved2 : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_aggr_n_u
{
  struct ipa_hwio_def_ipa_endp_init_aggr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_HOL_BLOCK_EN_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_hol_block_en_n_s
{
  u32 en : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_hol_block_en_n_u
{
  struct ipa_hwio_def_ipa_endp_init_hol_block_en_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_HOL_BLOCK_TIMER_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_hol_block_timer_n_s
{
  u32 time_limit : 5;
  u32 reserved0 : 3;
  u32 gran_sel : 2;
  u32 reserved1 : 22;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_hol_block_timer_n_u
{
  struct ipa_hwio_def_ipa_endp_init_hol_block_timer_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_DEAGGR_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_deaggr_n_s
{
  u32 deaggr_hdr_len : 6;
  u32 syspipe_err_detection : 1;
  u32 packet_offset_valid : 1;
  u32 packet_offset_location : 6;
  u32 ignore_min_pkt_err : 1;
  u32 mbim_or_ncm_flag : 1;
  u32 max_packet_len : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_deaggr_n_u
{
  struct ipa_hwio_def_ipa_endp_init_deaggr_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_RSRC_GRP_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_rsrc_grp_n_s
{
  u32 rsrc_grp : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_rsrc_grp_n_u
{
  struct ipa_hwio_def_ipa_endp_init_rsrc_grp_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_SEQ_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_seq_n_s
{
  u32 hps_seq_type : 5;
  u32 reserved0 : 3;
  u32 dps_seq_type : 5;
  u32 reserved1 : 19;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_seq_n_u
{
  struct ipa_hwio_def_ipa_endp_init_seq_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_STATUS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_status_n_s
{
  u32 status_en : 1;
  u32 status_endp : 8;
  u32 status_pkt_supress : 1;
  u32 reserved0 : 22;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_status_n_u
{
  struct ipa_hwio_def_ipa_endp_status_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_SRC_ID_WRITE_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_src_id_write_n_s
{
  u32 src_id_write_value : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_src_id_write_n_u
{
  struct ipa_hwio_def_ipa_endp_src_id_write_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_SRC_ID_READ_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_src_id_read_n_s
{
  u32 src_id_read_value : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_src_id_read_n_u
{
  struct ipa_hwio_def_ipa_endp_src_id_read_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_DRBIP_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_drbip_cfg_n_s
{
  u32 data_sectors_for_imm_cmd : 6;
  u32 reserved0 : 2;
  u32 rsrc_grp_for_drbip_acl : 4;
  u32 reserved1 : 12;
  u32 drbip_operation_mode : 2;
  u32 reserved2 : 6;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_drbip_cfg_n_u
{
  struct ipa_hwio_def_ipa_endp_init_drbip_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_FILTER_CACHE_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_filter_cache_cfg_n_s
{
  u32 filter_cache_msk_src_id : 1;
  u32 filter_cache_msk_src_ip_add : 1;
  u32 filter_cache_msk_dst_ip_add : 1;
  u32 filter_cache_msk_src_port : 1;
  u32 filter_cache_msk_dst_port : 1;
  u32 filter_cache_msk_protocol : 1;
  u32 filter_cache_msk_metadata : 1;
  u32 filter_cache_msk_spi : 1;
  u32 filter_cache_msk_src_mac : 1;
  u32 filter_cache_msk_dest_mac : 1;
  u32 filter_cache_msk_eth_type : 1;
  u32 filter_cache_msk_ctag : 1;
  u32 filter_cache_msk_stag : 1;
  u32 filter_cache_msk_dsatag : 1;
  u32 reserved0 : 18;
};

/* Union definition of register */
union ipa_hwio_def_ipa_filter_cache_cfg_n_u
{
  struct ipa_hwio_def_ipa_filter_cache_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ROUTER_CACHE_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_router_cache_cfg_n_s
{
  u32 router_cache_msk_src_id : 1;
  u32 router_cache_msk_src_ip_add : 1;
  u32 router_cache_msk_dst_ip_add : 1;
  u32 router_cache_msk_src_port : 1;
  u32 router_cache_msk_dst_port : 1;
  u32 router_cache_msk_protocol : 1;
  u32 router_cache_msk_metadata : 1;
  u32 router_cache_msk_spi : 1;
  u32 router_cache_msk_src_mac : 1;
  u32 router_cache_msk_dest_mac : 1;
  u32 router_cache_msk_eth_type : 1;
  u32 router_cache_msk_ctag : 1;
  u32 router_cache_msk_stag : 1;
  u32 router_cache_msk_dsatag : 1;
  u32 reserved0 : 18;
};

/* Union definition of register */
union ipa_hwio_def_ipa_router_cache_cfg_n_u
{
  struct ipa_hwio_def_ipa_router_cache_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_CTRL_STATUS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_ctrl_status_n_s
{
  u32 endp_suspend_status : 1;
  u32 endp_delay_status : 1;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_ctrl_status_n_u
{
  struct ipa_hwio_def_ipa_endp_init_ctrl_status_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_PROD_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_prod_cfg_n_s
{
  u32 tx_sel : 1;
  u32 tsp_enable : 1;
  u32 prod_max_output_size_drop_enable : 1;
  u32 hw_data_error_qmap_en : 1;
  u32 tsp_producer_index : 5;
  u32 prod_max_output_size_size : 8;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_prod_cfg_n_u
{
  struct ipa_hwio_def_ipa_endp_init_prod_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_PROD_CFG2_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_prod_cfg2_n_s
{
  u32 dscp_extract_to_cookie_en : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_prod_cfg2_n_u
{
  struct ipa_hwio_def_ipa_endp_init_prod_cfg2_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_ULSO_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_ulso_cfg_n_s
{
  u32 ipv4_id_min_max_val_index : 2;
  u32 reserved0 : 30;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_ulso_cfg_n_u
{
  struct ipa_hwio_def_ipa_endp_init_ulso_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_UCP_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_ucp_cfg_n_s
{
  u32 ucp_command_id : 16;
  u32 ucp_trigger_en : 1;
  u32 reserved0 : 15;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_ucp_cfg_n_u
{
  struct ipa_hwio_def_ipa_endp_init_ucp_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_NAT_EXC_SUPPRESS_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_nat_exc_suppress_n_s
{
  u32 nat_exc_suppress : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_nat_exc_suppress_n_u
{
  struct ipa_hwio_def_ipa_endp_init_nat_exc_suppress_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_IPSEC_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_ipsec_cfg_n_s
{
  u32 exception_endp_ipsec_decap : 8;
  u32 exception_endp_ipsec_post_decaps : 8;
  u32 exception_endp_ipsec_post_encaps : 8;
  u32 decaps_next_hdr_check_disable : 1;
  u32 reserved0 : 7;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_ipsec_cfg_n_u
{
  struct ipa_hwio_def_ipa_endp_init_ipsec_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_ETHERNET_CFG_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_ethernet_cfg_n_s
{
  u32 metadata_extraction_type : 2;
  u32 reserved0 : 2;
  u32 dsa_tag_in_frame : 1;
  u32 reserved1 : 27;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_ethernet_cfg_n_u
{
  struct ipa_hwio_def_ipa_endp_init_ethernet_cfg_n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ENDP_INIT_MODE_EXT_n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_endp_init_mode_ext_n_s
{
  u32 src_padding_remove_en : 1;
  u32 reserved0 : 3;
  u32 incoming_l4_checksum_cfg : 2;
  u32 reserved1 : 26;
};

/* Union definition of register */
union ipa_hwio_def_ipa_endp_init_mode_ext_n_u
{
  struct ipa_hwio_def_ipa_endp_init_mode_ext_n_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_RA_XPU_SB_IPA_RA_XPU4_SB
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_SB_RA_XPU_SIDEBAND_STATIC_SLE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_sb_ra_xpu_sideband_static_sle_s
{
  u32 set : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_sb_ra_xpu_sideband_static_sle_u
{
  struct ipa_hwio_def_ipa_ra_xpu_sb_ra_xpu_sideband_static_sle_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_SB_RA_XPU_SIDEBAND_STATIC 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_sb_ra_xpu_sideband_static_s
{
  u32 cfgowns : 1;
  u32 apnsee : 1;
  u32 optrw_en : 1;
  u32 cmocesde : 1;
  u32 reserved0 : 1;
  u32 apsvioe : 1;
  u32 reserved1 : 26;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_sb_ra_xpu_sideband_static_u
{
  struct ipa_hwio_def_ipa_ra_xpu_sb_ra_xpu_sideband_static_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_SB_RA_XPU_SIDEBAND_STATIC_SLE_WAS_WRITTEN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_sb_ra_xpu_sideband_static_sle_was_written_s
{
  u32 val : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_sb_ra_xpu_sideband_static_sle_was_written_u
{
  struct ipa_hwio_def_ipa_ra_xpu_sb_ra_xpu_sideband_static_sle_was_written_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_VMIDMT_SMR_86_SSD8_SID8_MA128_SP1_40
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SCR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_scr0_s
{
  u32 clientpd : 1;
  u32 reserved0 : 1;
  u32 gfie : 1;
  u32 reserved1 : 1;
  u32 gcfgere : 1;
  u32 gcfgfie : 1;
  u32 transientcfg : 2;
  u32 stalld : 1;
  u32 gse : 1;
  u32 usfcfg : 1;
  u32 reserved2 : 5;
  u32 memattr : 3;
  u32 reserved3 : 1;
  u32 mtcfg : 1;
  u32 smcfcfg : 1;
  u32 shcfg : 2;
  u32 racfg : 2;
  u32 wacfg : 2;
  u32 nscfg : 2;
  u32 reserved4 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_scr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_scr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SCR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_scr1_s
{
  u32 reserved0 : 8;
  u32 nsnumsmrgo : 7;
  u32 reserved1 : 9;
  u32 gasrae : 1;
  u32 reserved2 : 7;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_scr1_u
{
  struct ipa_hwio_def_ipa_vmidmt_scr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SCR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_scr2_s
{
  u32 bpvmid : 5;
  u32 reserved0 : 27;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_scr2_u
{
  struct ipa_hwio_def_ipa_vmidmt_scr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SACR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sacr_s
{
  u32 bpreqpriority : 2;
  u32 reserved0 : 2;
  u32 bpreqprioritycfg : 1;
  u32 reserved1 : 23;
  u32 bprcosh : 1;
  u32 bprcish : 1;
  u32 bprcnsh : 1;
  u32 reserved2 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sacr_u
{
  struct ipa_hwio_def_ipa_vmidmt_sacr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SIDR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sidr0_s
{
  u32 numsmrg : 8;
  u32 reserved0 : 1;
  u32 numsidb : 4;
  u32 reserved1 : 14;
  u32 sms : 1;
  u32 reserved2 : 3;
  u32 ses : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sidr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_sidr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SIDR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sidr1_s
{
  u32 reserved0 : 8;
  u32 numssdndx : 4;
  u32 ssdtp : 1;
  u32 reserved1 : 2;
  u32 smcd : 1;
  u32 reserved2 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sidr1_u
{
  struct ipa_hwio_def_ipa_vmidmt_sidr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SIDR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sidr2_s
{
  u32 ias : 4;
  u32 oas : 4;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sidr2_u
{
  struct ipa_hwio_def_ipa_vmidmt_sidr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SIDR4 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sidr4_s
{
  u32 step : 16;
  u32 minor : 12;
  u32 major : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sidr4_u
{
  struct ipa_hwio_def_ipa_vmidmt_sidr4_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SIDR5 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sidr5_s
{
  u32 nvmid : 8;
  u32 qribe : 1;
  u32 msae : 1;
  u32 reserved0 : 6;
  u32 nummsdrb : 8;
  u32 numspdrb : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sidr5_u
{
  struct ipa_hwio_def_ipa_vmidmt_sidr5_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SIDR7 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sidr7_s
{
  u32 minor : 4;
  u32 major : 4;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sidr7_u
{
  struct ipa_hwio_def_ipa_vmidmt_sidr7_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SGFAR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sgfar0_s
{
  u32 sgfea0 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sgfar0_u
{
  struct ipa_hwio_def_ipa_vmidmt_sgfar0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SGFAR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sgfar1_s
{
  u32 sgfea1 : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sgfar1_u
{
  struct ipa_hwio_def_ipa_vmidmt_sgfar1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SGFSR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sgfsr_s
{
  u32 reserved0 : 1;
  u32 usf : 1;
  u32 smcf : 1;
  u32 reserved1 : 2;
  u32 caf : 1;
  u32 reserved2 : 24;
  u32 multi_cfg : 1;
  u32 multi_client : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sgfsr_u
{
  struct ipa_hwio_def_ipa_vmidmt_sgfsr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SGFSRRESTORE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sgfsrrestore_s
{
  u32 reserved0 : 1;
  u32 usf : 1;
  u32 smcf : 1;
  u32 reserved1 : 2;
  u32 caf : 1;
  u32 reserved2 : 24;
  u32 multi_cfg : 1;
  u32 multi_client : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sgfsrrestore_u
{
  struct ipa_hwio_def_ipa_vmidmt_sgfsrrestore_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SGFSYNDR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sgfsyndr0_s
{
  u32 reserved0 : 1;
  u32 wnr : 1;
  u32 reserved1 : 2;
  u32 nsstate : 1;
  u32 nsattr : 1;
  u32 reserved2 : 2;
  u32 mssselfauth : 1;
  u32 reserved3 : 23;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sgfsyndr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_sgfsyndr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SGFSYNDR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sgfsyndr1_s
{
  u32 streamindex : 8;
  u32 reserved0 : 8;
  u32 ssdindex : 8;
  u32 msdindex : 7;
  u32 reserved1 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sgfsyndr1_u
{
  struct ipa_hwio_def_ipa_vmidmt_sgfsyndr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SGFSYNDR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_sgfsyndr2_s
{
  u32 amid : 8;
  u32 apid : 5;
  u32 abid : 3;
  u32 avmid : 5;
  u32 reserved0 : 3;
  u32 atid : 5;
  u32 reserved1 : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_sgfsyndr2_u
{
  struct ipa_hwio_def_ipa_vmidmt_sgfsyndr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_VMIDMTSCR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_vmidmtscr0_s
{
  u32 clkonoffe : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_vmidmtscr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_vmidmtscr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_CR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_cr0_s
{
  u32 clientpd : 1;
  u32 reserved0 : 1;
  u32 gfie : 1;
  u32 reserved1 : 1;
  u32 gcfgere : 1;
  u32 gcfgfie : 1;
  u32 transientcfg : 2;
  u32 stalld : 1;
  u32 gse : 1;
  u32 usfcfg : 1;
  u32 vmidpne : 1;
  u32 reserved2 : 4;
  u32 memattr : 3;
  u32 reserved3 : 1;
  u32 mtcfg : 1;
  u32 smcfcfg : 1;
  u32 shcfg : 2;
  u32 racfg : 2;
  u32 wacfg : 2;
  u32 reserved4 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_cr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_cr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_CR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_cr2_s
{
  u32 bpvmid : 5;
  u32 reserved0 : 27;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_cr2_u
{
  struct ipa_hwio_def_ipa_vmidmt_cr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_ACR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_acr_s
{
  u32 bpreqpriority : 2;
  u32 reserved0 : 2;
  u32 bpreqprioritycfg : 1;
  u32 reserved1 : 23;
  u32 bprcosh : 1;
  u32 bprcish : 1;
  u32 bprcnsh : 1;
  u32 reserved2 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_acr_u
{
  struct ipa_hwio_def_ipa_vmidmt_acr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_IDR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_idr0_s
{
  u32 numsmrg : 8;
  u32 reserved0 : 1;
  u32 numsidb : 4;
  u32 reserved1 : 14;
  u32 sms : 1;
  u32 reserved2 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_idr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_idr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_IDR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_idr1_s
{
  u32 reserved0 : 8;
  u32 numssdndx : 4;
  u32 ssdtp : 1;
  u32 reserved1 : 2;
  u32 smcd : 1;
  u32 reserved2 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_idr1_u
{
  struct ipa_hwio_def_ipa_vmidmt_idr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_IDR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_idr2_s
{
  u32 ias : 4;
  u32 oas : 4;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_idr2_u
{
  struct ipa_hwio_def_ipa_vmidmt_idr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_IDR4 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_idr4_s
{
  u32 step : 16;
  u32 minor : 12;
  u32 major : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_idr4_u
{
  struct ipa_hwio_def_ipa_vmidmt_idr4_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_IDR5 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_idr5_s
{
  u32 nvmid : 8;
  u32 qribe : 1;
  u32 msae : 1;
  u32 reserved0 : 6;
  u32 nummsdrb : 8;
  u32 numspdrb : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_idr5_u
{
  struct ipa_hwio_def_ipa_vmidmt_idr5_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_IDR7 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_idr7_s
{
  u32 minor : 4;
  u32 major : 4;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_idr7_u
{
  struct ipa_hwio_def_ipa_vmidmt_idr7_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_GFAR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_gfar0_s
{
  u32 gfea0 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_gfar0_u
{
  struct ipa_hwio_def_ipa_vmidmt_gfar0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_GFAR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_gfar1_s
{
  u32 gfea1 : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_gfar1_u
{
  struct ipa_hwio_def_ipa_vmidmt_gfar1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_GFSR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_gfsr_s
{
  u32 reserved0 : 1;
  u32 usf : 1;
  u32 smcf : 1;
  u32 reserved1 : 2;
  u32 caf : 1;
  u32 reserved2 : 1;
  u32 pf : 1;
  u32 reserved3 : 22;
  u32 multi_cfg : 1;
  u32 multi_client : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_gfsr_u
{
  struct ipa_hwio_def_ipa_vmidmt_gfsr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_GFSRRESTORE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_gfsrrestore_s
{
  u32 reserved0 : 1;
  u32 usf : 1;
  u32 smcf : 1;
  u32 reserved1 : 2;
  u32 caf : 1;
  u32 reserved2 : 1;
  u32 pf : 1;
  u32 reserved3 : 22;
  u32 multi_cfg : 1;
  u32 multi_client : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_gfsrrestore_u
{
  struct ipa_hwio_def_ipa_vmidmt_gfsrrestore_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_GFSYNDR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_gfsyndr0_s
{
  u32 reserved0 : 1;
  u32 wnr : 1;
  u32 reserved1 : 2;
  u32 nsstate : 1;
  u32 nsattr : 1;
  u32 reserved2 : 2;
  u32 mssselfauth : 1;
  u32 reserved3 : 23;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_gfsyndr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_gfsyndr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_GFSYNDR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_gfsyndr1_s
{
  u32 streamindex : 8;
  u32 reserved0 : 8;
  u32 ssdindex : 8;
  u32 msdindex : 7;
  u32 reserved1 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_gfsyndr1_u
{
  struct ipa_hwio_def_ipa_vmidmt_gfsyndr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_GFSYNDR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_gfsyndr2_s
{
  u32 amid : 8;
  u32 apid : 5;
  u32 abid : 3;
  u32 avmid : 5;
  u32 reserved0 : 3;
  u32 atid : 5;
  u32 reserved1 : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_gfsyndr2_u
{
  struct ipa_hwio_def_ipa_vmidmt_gfsyndr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_VMIDMTCR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_vmidmtcr0_s
{
  u32 clkonoffe : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_vmidmtcr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_vmidmtcr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_VMIDMTACR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_vmidmtacr_s
{
  u32 rwe : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_vmidmtacr_u
{
  struct ipa_hwio_def_ipa_vmidmt_vmidmtacr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_NSCR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_nscr0_s
{
  u32 clientpd : 1;
  u32 reserved0 : 1;
  u32 gfie : 1;
  u32 reserved1 : 1;
  u32 gcfgere : 1;
  u32 gcfgfie : 1;
  u32 transientcfg : 2;
  u32 stalld : 1;
  u32 gse : 1;
  u32 usfcfg : 1;
  u32 vmidpne : 1;
  u32 reserved2 : 4;
  u32 memattr : 3;
  u32 reserved3 : 1;
  u32 mtcfg : 1;
  u32 smcfcfg : 1;
  u32 shcfg : 2;
  u32 racfg : 2;
  u32 wacfg : 2;
  u32 reserved4 : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_nscr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_nscr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_NSCR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_nscr2_s
{
  u32 bpvmid : 5;
  u32 reserved0 : 27;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_nscr2_u
{
  struct ipa_hwio_def_ipa_vmidmt_nscr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_NSACR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_nsacr_s
{
  u32 bpreqpriority : 2;
  u32 reserved0 : 2;
  u32 bpreqprioritycfg : 1;
  u32 reserved1 : 23;
  u32 bprcosh : 1;
  u32 bprcish : 1;
  u32 bprcnsh : 1;
  u32 reserved2 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_nsacr_u
{
  struct ipa_hwio_def_ipa_vmidmt_nsacr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_NSGFAR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_nsgfar0_s
{
  u32 gfea0 : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_nsgfar0_u
{
  struct ipa_hwio_def_ipa_vmidmt_nsgfar0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_NSGFAR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_nsgfar1_s
{
  u32 gfea1 : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_nsgfar1_u
{
  struct ipa_hwio_def_ipa_vmidmt_nsgfar1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_NSGFSR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_nsgfsr_s
{
  u32 reserved0 : 1;
  u32 usf : 1;
  u32 smcf : 1;
  u32 reserved1 : 2;
  u32 caf : 1;
  u32 reserved2 : 1;
  u32 pf : 1;
  u32 reserved3 : 22;
  u32 multi_cfg : 1;
  u32 multi_client : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_nsgfsr_u
{
  struct ipa_hwio_def_ipa_vmidmt_nsgfsr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_NSGFSRRESTORE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_nsgfsrrestore_s
{
  u32 reserved0 : 1;
  u32 usf : 1;
  u32 smcf : 1;
  u32 reserved1 : 2;
  u32 caf : 1;
  u32 reserved2 : 1;
  u32 pf : 1;
  u32 reserved3 : 22;
  u32 multi_cfg : 1;
  u32 multi_client : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_nsgfsrrestore_u
{
  struct ipa_hwio_def_ipa_vmidmt_nsgfsrrestore_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_NSGFSYNDR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_nsgfsyndr0_s
{
  u32 reserved0 : 1;
  u32 wnr : 1;
  u32 reserved1 : 2;
  u32 nsstate : 1;
  u32 nsattr : 1;
  u32 reserved2 : 2;
  u32 mssselfauth : 1;
  u32 reserved3 : 23;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_nsgfsyndr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_nsgfsyndr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_NSGFSYNDR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_nsgfsyndr1_s
{
  u32 streamindex : 8;
  u32 reserved0 : 8;
  u32 ssdindex : 8;
  u32 msdindex : 7;
  u32 reserved1 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_nsgfsyndr1_u
{
  struct ipa_hwio_def_ipa_vmidmt_nsgfsyndr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_NSGFSYNDR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_nsgfsyndr2_s
{
  u32 amid : 8;
  u32 apid : 5;
  u32 abid : 3;
  u32 avmid : 5;
  u32 reserved0 : 3;
  u32 atid : 5;
  u32 reserved1 : 3;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_nsgfsyndr2_u
{
  struct ipa_hwio_def_ipa_vmidmt_nsgfsyndr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_NSVMIDMTCR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_nsvmidmtcr0_s
{
  u32 clkonoffe : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_nsvmidmtcr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_nsvmidmtcr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SSDR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_ssdr0_s
{
  u32 rwe : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_ssdr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_ssdr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SSDR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_ssdr1_s
{
  u32 rwe : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_ssdr1_u
{
  struct ipa_hwio_def_ipa_vmidmt_ssdr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SSDR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_ssdr2_s
{
  u32 rwe : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_ssdr2_u
{
  struct ipa_hwio_def_ipa_vmidmt_ssdr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SSDR3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_ssdr3_s
{
  u32 rwe : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_ssdr3_u
{
  struct ipa_hwio_def_ipa_vmidmt_ssdr3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_MSDR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_msdr0_s
{
  u32 rwe : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_msdr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_msdr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_MSDR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_msdr1_s
{
  u32 rwe : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_msdr1_u
{
  struct ipa_hwio_def_ipa_vmidmt_msdr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_MSDR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_msdr2_s
{
  u32 rwe : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_msdr2_u
{
  struct ipa_hwio_def_ipa_vmidmt_msdr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_MSDR3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_msdr3_s
{
  u32 rwe : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_msdr3_u
{
  struct ipa_hwio_def_ipa_vmidmt_msdr3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_MCR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_mcr_s
{
  u32 bpsmsacfg : 1;
  u32 bpmsacfg : 1;
  u32 clkonoffe : 1;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_mcr_u
{
  struct ipa_hwio_def_ipa_vmidmt_mcr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SPDR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_spdr0_s
{
  u32 rwe : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_spdr0_u
{
  struct ipa_hwio_def_ipa_vmidmt_spdr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SPCR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_spcr_s
{
  u32 bpsspdcfg : 1;
  u32 bpspdcfg : 1;
  u32 clkonoffe : 1;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_spcr_u
{
  struct ipa_hwio_def_ipa_vmidmt_spcr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_S2VRn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_s2vrn_s
{
  u32 vmid : 5;
  u32 reserved0 : 3;
  u32 shcfg : 2;
  u32 reserved1 : 1;
  u32 mtcfg : 1;
  u32 memattr : 3;
  u32 reserved2 : 1;
  u32 type : 2;
  u32 nscfg : 2;
  u32 racfg : 2;
  u32 wacfg : 2;
  u32 reserved3 : 4;
  u32 transientcfg : 2;
  u32 reserved4 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_s2vrn_u
{
  struct ipa_hwio_def_ipa_vmidmt_s2vrn_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_AS2VRn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_as2vrn_s
{
  u32 reqpriority : 2;
  u32 reserved0 : 2;
  u32 reqprioritycfg : 1;
  u32 reserved1 : 23;
  u32 rcosh : 1;
  u32 rcish : 1;
  u32 rcnsh : 1;
  u32 reserved2 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_as2vrn_u
{
  struct ipa_hwio_def_ipa_vmidmt_as2vrn_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_VMIDMT_SMRn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_vmidmt_smrn_s
{
  u32 id : 8;
  u32 reserved0 : 8;
  u32 mask : 8;
  u32 reserved1 : 7;
  u32 valid : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_vmidmt_smrn_u
{
  struct ipa_hwio_def_ipa_vmidmt_smrn_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_RA_XPU_CFG_XPU4_AHB_WRAPPER_APUQ11_31_CL32_LE_SY
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * MODULE: IPA_RA_XPU_CFG_XPU4
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_IDR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr0_s
{
  u32 xpu_type : 2;
  u32 xpresscfgen : 1;
  u32 xpresscfgmultidie : 1;
  u32 client_pipeline_en : 1;
  u32 client_haltreqack_en : 1;
  u32 bled : 1;
  u32 reserved0 : 9;
  u32 nrg : 10;
  u32 reserved1 : 6;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr0_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_IDR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr1_s
{
  u32 addr_lsb : 6;
  u32 reserved0 : 2;
  u32 addr_msb : 6;
  u32 reserved1 : 2;
  u32 config_addr_width : 6;
  u32 reserved2 : 2;
  u32 client_addr_width : 6;
  u32 reserved3 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr1_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_IDR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr2_s
{
  u32 nqad : 5;
  u32 reserved0 : 3;
  u32 uselegacyintf : 1;
  u32 useahbwrapper : 1;
  u32 useqsiclientintf : 1;
  u32 useqsicfgintf : 1;
  u32 parityen : 1;
  u32 syncmodeen : 1;
  u32 reserved1 : 18;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr2_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_REV 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rev_s
{
  u32 step : 16;
  u32 minor : 12;
  u32 major : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rev_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rev_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_GCR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_gcr_s
{
  u32 apnspe : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_gcr_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_gcr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_RSR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rsr_s
{
  u32 numfrg : 16;
  u32 nxtfrg : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rsr_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rsr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_IDR3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr3_s
{
  u32 idr3_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr3_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_IDR4 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr4_s
{
  u32 idr4_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr4_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr4_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_IDR5 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr5_s
{
  u32 idr5_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr5_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr5_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_IDR6 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr6_s
{
  u32 idr6_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr6_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr6_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_IDR7 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr7_s
{
  u32 idr7_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr7_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr7_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_IDR8 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr8_s
{
  u32 idr8_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr8_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr8_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_IDR9 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr9_s
{
  u32 idr9_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr9_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr9_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_IDR10 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr10_s
{
  u32 idr10_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr10_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_idr10_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_CFGERE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_cfgere_s
{
  u32 cfgere_qad : 11;
  u32 reserved0 : 19;
  u32 cfgere_ns : 1;
  u32 cfgere_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_cfgere_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_cfgere_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_CLERE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_clere_s
{
  u32 clere_qad : 11;
  u32 reserved0 : 19;
  u32 clere_ns : 1;
  u32 clere_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_clere_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_clere_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_DBGAR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_dbgar_s
{
  u32 dbga_qad : 11;
  u32 reserved0 : 19;
  u32 dbga_ns : 1;
  u32 dbga_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_dbgar_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_dbgar_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_GCR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_gcr2_s
{
  u32 rg_cfg_rd_ac_en : 1;
  u32 dynamic_clk_en : 1;
  u32 error_clk_en : 1;
  u32 reserved0 : 1;
  u32 rsr_cgc_en : 1;
  u32 clpipe_cgc_en : 1;
  u32 syn_cgc_en : 1;
  u32 reserved1 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_gcr2_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_gcr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_CFGOWNER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_cfgowner_s
{
  u32 cfg_qad : 5;
  u32 reserved0 : 27;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_cfgowner_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_cfgowner_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_ESR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_esr_s
{
  u32 cfgerr : 1;
  u32 clerr : 1;
  u32 cfgmulti : 1;
  u32 clmulti : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_esr_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_esr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_SYNAR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synar0_s
{
  u32 synaddr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synar0_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synar0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_SYNAR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synar1_s
{
  u32 synaddr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synar1_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synar1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_SYNR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synr0_s
{
  u32 xprotns : 1;
  u32 inst : 1;
  u32 priv : 1;
  u32 req_opc : 4;
  u32 reserved0 : 1;
  u32 qad : 5;
  u32 reserved1 : 2;
  u32 len : 8;
  u32 ssize : 3;
  u32 apudecerr : 1;
  u32 qaderr : 1;
  u32 burstlen : 1;
  u32 ac_cl : 1;
  u32 ac_cfg : 1;
  u32 ph : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synr0_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_SYNR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synr1_s
{
  u32 mid : 8;
  u32 pid : 5;
  u32 bid : 3;
  u32 trtype : 3;
  u32 dieid : 2;
  u32 reserved0 : 3;
  u32 tid : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synr1_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_SYNR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synr2_s
{
  u32 dirtyinfo : 1;
  u32 cacheallocation : 4;
  u32 writethrough : 1;
  u32 noallocate : 1;
  u32 transient : 1;
  u32 memtype : 3;
  u32 innercacheable : 1;
  u32 redirbits : 4;
  u32 atopc : 4;
  u32 optrw_en : 1;
  u32 cesde : 1;
  u32 apnsee : 1;
  u32 cfg_owner : 1;
  u32 sle : 1;
  u32 apsvioe : 1;
  u32 reserved0 : 6;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synr2_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_synr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_RGCR0n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rgcr0n_s
{
  u32 rgwowp : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rgcr0n_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rgcr0n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_RGCR1n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rgcr1n_s
{
  u32 rge : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rgcr1n_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rgcr1n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_RGRDRn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rgrdrn_s
{
  u32 rda_qad : 11;
  u32 reserved0 : 19;
  u32 rda_ns : 1;
  u32 rda_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rgrdrn_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rgrdrn_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_RGWRRn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rgwrrn_s
{
  u32 wra_qad : 11;
  u32 reserved0 : 19;
  u32 wra_ns : 1;
  u32 wra_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rgwrrn_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_rgwrrn_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_RA_XPU_CFG_XPU4_QADRGLn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_qadrgln_s
{
  u32 rgl_qad : 11;
  u32 reserved0 : 19;
  u32 rgl_ns : 1;
  u32 rgl_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_qadrgln_u
{
  struct ipa_hwio_def_ipa_ra_xpu_cfg_xpu4_qadrgln_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_ER_XPU_CFG_XPU4_AHB_WRAPPER_APUQ11_31_CL32_LE_SY
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * MODULE: IPA_ER_XPU_CFG_XPU4
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_IDR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr0_s
{
  u32 xpu_type : 2;
  u32 xpresscfgen : 1;
  u32 xpresscfgmultidie : 1;
  u32 client_pipeline_en : 1;
  u32 client_haltreqack_en : 1;
  u32 bled : 1;
  u32 reserved0 : 9;
  u32 nrg : 10;
  u32 reserved1 : 6;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr0_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_IDR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr1_s
{
  u32 addr_lsb : 6;
  u32 reserved0 : 2;
  u32 addr_msb : 6;
  u32 reserved1 : 2;
  u32 config_addr_width : 6;
  u32 reserved2 : 2;
  u32 client_addr_width : 6;
  u32 reserved3 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr1_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_IDR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr2_s
{
  u32 nqad : 5;
  u32 reserved0 : 3;
  u32 uselegacyintf : 1;
  u32 useahbwrapper : 1;
  u32 useqsiclientintf : 1;
  u32 useqsicfgintf : 1;
  u32 parityen : 1;
  u32 syncmodeen : 1;
  u32 reserved1 : 18;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr2_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_REV 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rev_s
{
  u32 step : 16;
  u32 minor : 12;
  u32 major : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rev_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rev_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_GCR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_gcr_s
{
  u32 apnspe : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_gcr_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_gcr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_RSR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rsr_s
{
  u32 numfrg : 16;
  u32 nxtfrg : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rsr_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rsr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_IDR3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr3_s
{
  u32 idr3_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr3_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_IDR4 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr4_s
{
  u32 idr4_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr4_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr4_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_IDR5 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr5_s
{
  u32 idr5_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr5_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr5_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_IDR6 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr6_s
{
  u32 idr6_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr6_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr6_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_IDR7 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr7_s
{
  u32 idr7_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr7_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr7_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_IDR8 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr8_s
{
  u32 idr8_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr8_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr8_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_IDR9 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr9_s
{
  u32 idr9_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr9_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr9_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_IDR10 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr10_s
{
  u32 idr10_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr10_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_idr10_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_CFGERE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_cfgere_s
{
  u32 cfgere_qad : 11;
  u32 reserved0 : 19;
  u32 cfgere_ns : 1;
  u32 cfgere_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_cfgere_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_cfgere_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_CLERE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_clere_s
{
  u32 clere_qad : 11;
  u32 reserved0 : 19;
  u32 clere_ns : 1;
  u32 clere_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_clere_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_clere_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_DBGAR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_dbgar_s
{
  u32 dbga_qad : 11;
  u32 reserved0 : 19;
  u32 dbga_ns : 1;
  u32 dbga_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_dbgar_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_dbgar_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_GCR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_gcr2_s
{
  u32 rg_cfg_rd_ac_en : 1;
  u32 dynamic_clk_en : 1;
  u32 error_clk_en : 1;
  u32 reserved0 : 1;
  u32 rsr_cgc_en : 1;
  u32 clpipe_cgc_en : 1;
  u32 syn_cgc_en : 1;
  u32 reserved1 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_gcr2_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_gcr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_CFGOWNER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_cfgowner_s
{
  u32 cfg_qad : 5;
  u32 reserved0 : 27;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_cfgowner_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_cfgowner_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_ESR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_esr_s
{
  u32 cfgerr : 1;
  u32 clerr : 1;
  u32 cfgmulti : 1;
  u32 clmulti : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_esr_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_esr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_SYNAR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synar0_s
{
  u32 synaddr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synar0_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synar0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_SYNAR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synar1_s
{
  u32 synaddr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synar1_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synar1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_SYNR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synr0_s
{
  u32 xprotns : 1;
  u32 inst : 1;
  u32 priv : 1;
  u32 req_opc : 4;
  u32 reserved0 : 1;
  u32 qad : 5;
  u32 reserved1 : 2;
  u32 len : 8;
  u32 ssize : 3;
  u32 apudecerr : 1;
  u32 qaderr : 1;
  u32 burstlen : 1;
  u32 ac_cl : 1;
  u32 ac_cfg : 1;
  u32 ph : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synr0_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_SYNR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synr1_s
{
  u32 mid : 8;
  u32 pid : 5;
  u32 bid : 3;
  u32 trtype : 3;
  u32 dieid : 2;
  u32 reserved0 : 3;
  u32 tid : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synr1_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_SYNR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synr2_s
{
  u32 dirtyinfo : 1;
  u32 cacheallocation : 4;
  u32 writethrough : 1;
  u32 noallocate : 1;
  u32 transient : 1;
  u32 memtype : 3;
  u32 innercacheable : 1;
  u32 redirbits : 4;
  u32 atopc : 4;
  u32 optrw_en : 1;
  u32 cesde : 1;
  u32 apnsee : 1;
  u32 cfg_owner : 1;
  u32 sle : 1;
  u32 apsvioe : 1;
  u32 reserved0 : 6;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synr2_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_synr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_RGCR0n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rgcr0n_s
{
  u32 rgwowp : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rgcr0n_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rgcr0n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_RGCR1n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rgcr1n_s
{
  u32 rge : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rgcr1n_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rgcr1n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_RGRDRn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rgrdrn_s
{
  u32 rda_qad : 11;
  u32 reserved0 : 19;
  u32 rda_ns : 1;
  u32 rda_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rgrdrn_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rgrdrn_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_RGWRRn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rgwrrn_s
{
  u32 wra_qad : 11;
  u32 reserved0 : 19;
  u32 wra_ns : 1;
  u32 wra_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rgwrrn_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_rgwrrn_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_ER_XPU_CFG_XPU4_QADRGLn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_qadrgln_s
{
  u32 rgl_qad : 11;
  u32 reserved0 : 19;
  u32 rgl_ns : 1;
  u32 rgl_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_er_xpu_cfg_xpu4_qadrgln_u
{
  struct ipa_hwio_def_ipa_er_xpu_cfg_xpu4_qadrgln_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_MPU_IPA_MS_MPU_CFG
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * MODULE: IPA_MPU_XPU4
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_IDR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_idr0_s
{
  u32 xpu_type : 2;
  u32 xpresscfgen : 1;
  u32 xpresscfgmultidie : 1;
  u32 client_pipeline_en : 1;
  u32 client_haltreqack_en : 1;
  u32 bled : 1;
  u32 reserved0 : 9;
  u32 nrg : 10;
  u32 reserved1 : 6;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_idr0_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_idr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_IDR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_idr1_s
{
  u32 addr_lsb : 6;
  u32 reserved0 : 2;
  u32 addr_msb : 6;
  u32 reserved1 : 2;
  u32 config_addr_width : 6;
  u32 reserved2 : 2;
  u32 client_addr_width : 6;
  u32 reserved3 : 2;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_idr1_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_idr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_IDR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_idr2_s
{
  u32 nqad : 5;
  u32 reserved0 : 3;
  u32 uselegacyintf : 1;
  u32 useahbwrapper : 1;
  u32 useqsiclientintf : 1;
  u32 useqsicfgintf : 1;
  u32 parityen : 1;
  u32 syncmodeen : 1;
  u32 reserved1 : 18;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_idr2_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_idr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_REV 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_rev_s
{
  u32 step : 16;
  u32 minor : 12;
  u32 major : 4;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_rev_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_rev_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_GCR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_gcr_s
{
  u32 apnspe : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_gcr_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_gcr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_RSR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_rsr_s
{
  u32 numfrg : 16;
  u32 nxtfrg : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_rsr_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_rsr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_IDR3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_idr3_s
{
  u32 idr3_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_idr3_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_idr3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_IDR4 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_idr4_s
{
  u32 idr4_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_idr4_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_idr4_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_IDR5 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_idr5_s
{
  u32 idr5_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_idr5_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_idr5_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_IDR6 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_idr6_s
{
  u32 idr6_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_idr6_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_idr6_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_IDR7 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_idr7_s
{
  u32 idr7_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_idr7_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_idr7_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_IDR8 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_idr8_s
{
  u32 idr8_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_idr8_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_idr8_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_IDR9 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_idr9_s
{
  u32 idr9_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_idr9_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_idr9_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_IDR10 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_idr10_s
{
  u32 idr10_reserved : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_idr10_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_idr10_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_CFGERE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_cfgere_s
{
  u32 cfgere_qad : 11;
  u32 reserved0 : 19;
  u32 cfgere_ns : 1;
  u32 cfgere_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_cfgere_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_cfgere_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_CLERE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_clere_s
{
  u32 clere_qad : 11;
  u32 reserved0 : 19;
  u32 clere_ns : 1;
  u32 clere_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_clere_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_clere_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_DBGAR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_dbgar_s
{
  u32 dbga_qad : 11;
  u32 reserved0 : 19;
  u32 dbga_ns : 1;
  u32 dbga_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_dbgar_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_dbgar_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_GCR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_gcr2_s
{
  u32 rg_cfg_rd_ac_en : 1;
  u32 dynamic_clk_en : 1;
  u32 error_clk_en : 1;
  u32 reserved0 : 1;
  u32 rsr_cgc_en : 1;
  u32 clpipe_cgc_en : 1;
  u32 syn_cgc_en : 1;
  u32 reserved1 : 25;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_gcr2_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_gcr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_CFGOWNER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_cfgowner_s
{
  u32 cfg_qad : 5;
  u32 reserved0 : 27;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_cfgowner_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_cfgowner_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_UMRPERMREG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_umrpermreg_s
{
  u32 umr_qad : 11;
  u32 reserved0 : 19;
  u32 umr_ns : 1;
  u32 umr_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_umrpermreg_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_umrpermreg_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_ESR 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_esr_s
{
  u32 cfgerr : 1;
  u32 clerr : 1;
  u32 cfgmulti : 1;
  u32 clmulti : 1;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_esr_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_esr_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_SYNAR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_synar0_s
{
  u32 synaddr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_synar0_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_synar0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_SYNAR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_synar1_s
{
  u32 synaddr : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_synar1_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_synar1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_SYNR0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_synr0_s
{
  u32 xprotns : 1;
  u32 inst : 1;
  u32 priv : 1;
  u32 req_opc : 4;
  u32 reserved0 : 1;
  u32 qad : 5;
  u32 reserved1 : 2;
  u32 len : 8;
  u32 ssize : 3;
  u32 apudecerr : 1;
  u32 qaderr : 1;
  u32 burstlen : 1;
  u32 ac_cl : 1;
  u32 ac_cfg : 1;
  u32 ph : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_synr0_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_synr0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_SYNR1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_synr1_s
{
  u32 mid : 8;
  u32 pid : 5;
  u32 bid : 3;
  u32 trtype : 3;
  u32 dieid : 2;
  u32 reserved0 : 3;
  u32 tid : 8;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_synr1_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_synr1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_SYNR2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_synr2_s
{
  u32 dirtyinfo : 1;
  u32 cacheallocation : 4;
  u32 writethrough : 1;
  u32 noallocate : 1;
  u32 transient : 1;
  u32 memtype : 3;
  u32 innercacheable : 1;
  u32 redirbits : 4;
  u32 atopc : 4;
  u32 optrw_en : 1;
  u32 cesde : 1;
  u32 apnsee : 1;
  u32 cfg_owner : 1;
  u32 sle : 1;
  u32 apsvioe : 1;
  u32 reserved0 : 6;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_synr2_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_synr2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_RGCR0n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_rgcr0n_s
{
  u32 rgwowp : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_rgcr0n_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_rgcr0n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_RGCR1n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_rgcr1n_s
{
  u32 rge : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_rgcr1n_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_rgcr1n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_RGCSAR1n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_rgcsar1n_s
{
  u32 rgsa : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_rgcsar1n_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_rgcsar1n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_RGCSAR0n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_rgcsar0n_s
{
  u32 reserved0 : 12;
  u32 rgsa : 20;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_rgcsar0n_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_rgcsar0n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_RGCEAR1n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_rgcear1n_s
{
  u32 rgea : 4;
  u32 reserved0 : 28;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_rgcear1n_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_rgcear1n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_RGCEAR0n 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_rgcear0n_s
{
  u32 reserved0 : 12;
  u32 rgea : 20;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_rgcear0n_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_rgcear0n_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_RGRDRn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_rgrdrn_s
{
  u32 rda_qad : 11;
  u32 reserved0 : 19;
  u32 rda_ns : 1;
  u32 rda_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_rgrdrn_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_rgrdrn_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_RGWRRn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_rgwrrn_s
{
  u32 wra_qad : 11;
  u32 reserved0 : 19;
  u32 wra_ns : 1;
  u32 wra_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_rgwrrn_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_rgwrrn_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_MPU_XPU4_QADRGLn 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_mpu_xpu4_qadrgln_s
{
  u32 rgl_qad : 11;
  u32 reserved0 : 19;
  u32 rgl_ns : 1;
  u32 rgl_s : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_mpu_xpu4_qadrgln_u
{
  struct ipa_hwio_def_ipa_mpu_xpu4_qadrgln_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_THROTTLE_THROTTLE_0_THROTTLE_0_THROTTLE
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_CNTRL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_cntrl_s
{
  u32 throttle_en : 1;
  u32 reserved0 : 7;
  u32 threshold_level_0_en : 1;
  u32 threshold_level_1_en : 1;
  u32 threshold_level_2_en : 1;
  u32 threshold_level_3_en : 1;
  u32 threshold_level_4_en : 1;
  u32 threshold_level_5_en : 1;
  u32 threshold_level_6_en : 1;
  u32 threshold_level_7_en : 1;
  u32 reserved1 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_cntrl_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_cntrl_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_CGC_CNTRL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_cgc_cntrl_s
{
  u32 throttle_cgc_en : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_cgc_cntrl_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_cgc_cntrl_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_RESET_CNTRL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_reset_cntrl_s
{
  u32 core_sw_reset : 1;
  u32 reserved0 : 31;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_reset_cntrl_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_reset_cntrl_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_CONFIG 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_config_s
{
  u32 throttle_cntrl_max_skew : 3;
  u32 reserved0 : 29;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_config_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_config_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_GRANT_PERIOD 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_grant_period_s
{
  u32 grant_period : 14;
  u32 reserved0 : 18;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_grant_period_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_grant_period_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_GRANT_COUNT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_grant_count_s
{
  u32 grant_count : 16;
  u32 reserved0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_grant_count_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_grant_count_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_THRESHOLD_00 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_threshold_00_s
{
  u32 threshold_level_7 : 16;
  u32 threshold_level_6 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_threshold_00_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_threshold_00_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_THRESHOLD_01 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_threshold_01_s
{
  u32 threshold_level_5 : 16;
  u32 threshold_level_4 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_threshold_01_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_threshold_01_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_THRESHOLD_02 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_threshold_02_s
{
  u32 threshold_level_3 : 16;
  u32 threshold_level_2 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_threshold_02_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_threshold_02_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_THRESHOLD_03 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_threshold_03_s
{
  u32 threshold_level_1 : 16;
  u32 threshold_level_0 : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_threshold_03_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_threshold_03_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_PEAK_ACCUM_CREDIT 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_peak_accum_credit_s
{
  u32 peak_accum_credit : 20;
  u32 reserved0 : 12;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_peak_accum_credit_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_peak_accum_credit_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_DEBUG_CNTL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_debug_cntl_s
{
  u32 debug_en : 1;
  u32 reserved0 : 7;
  u32 debug_sel : 4;
  u32 reserved1 : 20;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_debug_cntl_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_debug_cntl_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_DEBUG_READBACK 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_debug_readback_s
{
  u32 debug_bus_readback : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_debug_readback_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_debug_readback_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_CONFIG_CLK_CONTROL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_config_clk_control_s
{
  u32 throttle_bwmon_cfg_access_clkon_cntr : 8;
  u32 reserved0 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_config_clk_control_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_config_clk_control_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_THROTTLE_THROTTLE_0_SPARE_REGS 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_throttle_throttle_0_spare_regs_s
{
  u32 spare_reg_bit_0 : 1;
  u32 spare_reg_bit_1 : 1;
  u32 spare_reg_bit_2 : 1;
  u32 spare_reg_bit_3 : 1;
  u32 spare_reg_bit_4 : 1;
  u32 spare_reg_bit_5 : 1;
  u32 spare_reg_bit_6 : 1;
  u32 spare_reg_bit_7 : 1;
  u32 spare_reg_bit_8 : 1;
  u32 spare_reg_bit_9 : 1;
  u32 spare_reg_bit_10 : 1;
  u32 spare_reg_bit_11 : 1;
  u32 spare_reg_bit_12 : 1;
  u32 spare_reg_bit_13 : 1;
  u32 spare_reg_bit_14 : 1;
  u32 spare_reg_bit_15 : 1;
  u32 spare_reg_bit_16 : 1;
  u32 spare_reg_bit_17 : 1;
  u32 spare_reg_bit_18 : 1;
  u32 spare_reg_bit_19 : 1;
  u32 spare_reg_bit_20 : 1;
  u32 spare_reg_bit_21 : 1;
  u32 spare_reg_bit_22 : 1;
  u32 spare_reg_bit_23 : 1;
  u32 spare_reg_bit_24 : 1;
  u32 spare_reg_bit_25 : 1;
  u32 spare_reg_bit_26 : 1;
  u32 spare_reg_bit_27 : 1;
  u32 spare_reg_bit_28 : 1;
  u32 spare_reg_bit_29 : 1;
  u32 spare_reg_bit_30 : 1;
  u32 spare_reg_bit_31 : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_throttle_throttle_0_spare_regs_u
{
  struct ipa_hwio_def_ipa_throttle_throttle_0_spare_regs_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_PMU
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_CONTROL_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_control_0_s
{
  u32 clk_gating_disable : 1;
  u32 default_on : 1;
  u32 en_qchannel_timer : 1;
  u32 sw_qactive : 1;
  u32 fsm_reset : 1;
  u32 disable_clk_management : 1;
  u32 disable_pwr_management : 1;
  u32 pmu_seq3_control : 1;
  u32 disable_fal1_sleep : 1;
  u32 pmu_seq2_control : 1;
  u32 pmu_control_spare : 5;
  u32 condition_ds_qactive_on_qdeny : 1;
  u32 restore_timer : 8;
  u32 early_dslock_accept_enable : 1;
  u32 macro_wakeup_state_disable : 1;
  u32 reserved0 : 5;
  u32 disable_micro_sleep : 1;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_control_0_u
{
  struct ipa_hwio_def_ipa_pmu_control_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_QCHANNEL_CONFIG_0 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_qchannel_config_0_s
{
  u32 sequence_qchannel : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_qchannel_config_0_u
{
  struct ipa_hwio_def_ipa_pmu_qchannel_config_0_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_QCHANNEL_CONFIG_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_qchannel_config_1_s
{
  u32 sequence_qchannel : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_qchannel_config_1_u
{
  struct ipa_hwio_def_ipa_pmu_qchannel_config_1_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_QCHAN_OBSERVATION_REG_QREQN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_qchan_observation_reg_qreqn_s
{
  u32 upstream_qreqn : 16;
  u32 ext_client_qreqn : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_qchan_observation_reg_qreqn_u
{
  struct ipa_hwio_def_ipa_pmu_qchan_observation_reg_qreqn_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_QCHAN_OBSERVATION_REG_QACCEPTN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_qchan_observation_reg_qacceptn_s
{
  u32 upstream_qacceptn : 16;
  u32 ext_client_qacceptn : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_qchan_observation_reg_qacceptn_u
{
  struct ipa_hwio_def_ipa_pmu_qchan_observation_reg_qacceptn_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_QCHAN_OBSERVATION_REG_QDENY 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_qchan_observation_reg_qdeny_s
{
  u32 upstream_qdeny : 16;
  u32 ext_client_qdeny : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_qchan_observation_reg_qdeny_u
{
  struct ipa_hwio_def_ipa_pmu_qchan_observation_reg_qdeny_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_QCHAN_OBSERVATION_REG_QACTIVE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_qchan_observation_reg_qactive_s
{
  u32 upstream_qactive : 16;
  u32 ext_client_qactive : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_qchan_observation_reg_qactive_u
{
  struct ipa_hwio_def_ipa_pmu_qchan_observation_reg_qactive_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_QCHAN_OBSERVATION_REG_FAL_TOL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_qchan_observation_reg_fal_tol_s
{
  u32 upstream_fal_tol : 16;
  u32 ext_client_fal_tol : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_qchan_observation_reg_fal_tol_u
{
  struct ipa_hwio_def_ipa_pmu_qchan_observation_reg_fal_tol_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_OBSERVATION_REG_PMU 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_observation_reg_pmu_s
{
  u32 pmu_clk_gate_req : 1;
  u32 pmu_clk_gate_ack : 1;
  u32 pmu_power_gate_req : 1;
  u32 pmu_power_gate_ack : 1;
  u32 pmu_vcd_clk_gate_req : 1;
  u32 pmu_vcd_clk_gate_ack : 1;
  u32 pmu_vcd_power_gate_req : 1;
  u32 pmu_vcd_power_gate_ack : 1;
  u32 pmu_qchannel_clk_on : 1;
  u32 pmu_fsm_state : 6;
  u32 reserved_31_15 : 17;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_observation_reg_pmu_u
{
  struct ipa_hwio_def_ipa_pmu_observation_reg_pmu_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_SW_OVERRIDE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_sw_override_s
{
  u32 qreq_seq0_override : 1;
  u32 aoss_pwr_gate_ack_override : 1;
  u32 aoss_clk_gate_ack_override : 1;
  u32 gcc_pmu_vcd_pwr_gate_req : 1;
  u32 gcc_pmu_vcd_clk_gate_req : 1;
  u32 gcc_pmu_pwr_gate_req_override : 1;
  u32 gcc_pmu_clk_gate_req_override : 1;
  u32 downstream_qaccept_override : 1;
  u32 downstream_qdeny_override : 1;
  u32 downstream_qactive_override : 1;
  u32 qreq_seq1_override : 1;
  u32 qreq_seq2_override : 1;
  u32 qreq_exit_seq0_override : 1;
  u32 qreq_exit_seq1_override : 1;
  u32 qreq_exit_seq2_override : 1;
  u32 reserved_31_15 : 17;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_sw_override_u
{
  struct ipa_hwio_def_ipa_pmu_sw_override_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_SW_OVERRIDE_VAL 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_sw_override_val_s
{
  u32 qreq_seq0_override : 1;
  u32 aoss_pwr_gate_ack_override : 1;
  u32 aoss_clk_gate_ack_override : 1;
  u32 gcc_pmu_vcd_pwr_gate_req : 1;
  u32 gcc_pmu_vcd_clk_gate_req : 1;
  u32 gcc_pmu_pwr_gate_req_override : 1;
  u32 gcc_pmu_clk_gate_req_override : 1;
  u32 downstream_qaccept_override : 1;
  u32 downstream_qdeny_override : 1;
  u32 downstream_qactive_override : 1;
  u32 qreq_seq1_override : 1;
  u32 qreq_seq2_override : 1;
  u32 qreq_exit_seq0_override : 1;
  u32 qreq_exit_seq1_override : 1;
  u32 qreq_exit_seq2_override : 1;
  u32 reserved_31_15 : 17;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_sw_override_val_u
{
  struct ipa_hwio_def_ipa_pmu_sw_override_val_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_SYNDROME_REG_FSM_STATE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_syndrome_reg_fsm_state_s
{
  u32 synd_fsm_state : 6;
  u32 synd_irq_state : 2;
  u32 reserved_31_8 : 24;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_syndrome_reg_fsm_state_u
{
  struct ipa_hwio_def_ipa_pmu_syndrome_reg_fsm_state_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_SYNDROME_REG_QACTIVE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_syndrome_reg_qactive_s
{
  u32 synd_upstream_qactive : 16;
  u32 synd_ext_client_qactive : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_syndrome_reg_qactive_u
{
  struct ipa_hwio_def_ipa_pmu_syndrome_reg_qactive_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_SYNDROME_REG_QACCEPTN 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_syndrome_reg_qacceptn_s
{
  u32 synd_upstream_qacceptn : 16;
  u32 synd_ext_client_qacceptn : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_syndrome_reg_qacceptn_u
{
  struct ipa_hwio_def_ipa_pmu_syndrome_reg_qacceptn_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_SYNDROME_REG_QDENY 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_syndrome_reg_qdeny_s
{
  u32 synd_upstream_qdeny : 16;
  u32 synd_ext_client_qdeny : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_syndrome_reg_qdeny_u
{
  struct ipa_hwio_def_ipa_pmu_syndrome_reg_qdeny_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_MACRO_NOTIFICATION 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_macro_notification_s
{
  u32 upstream_macro_notification : 16;
  u32 ext_client_macro_notification : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_macro_notification_u
{
  struct ipa_hwio_def_ipa_pmu_macro_notification_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_QCHANNEL_CONFIG_2 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_qchannel_config_2_s
{
  u32 sequence_qchannel : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_qchannel_config_2_u
{
  struct ipa_hwio_def_ipa_pmu_qchannel_config_2_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_QCHANNEL_CONFIG_3 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_qchannel_config_3_s
{
  u32 sequence_qchannel : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_qchannel_config_3_u
{
  struct ipa_hwio_def_ipa_pmu_qchannel_config_3_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_EARLY_SYNDROME_REG_QACTIVE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_early_syndrome_reg_qactive_s
{
  u32 early_synd_upstream_qactive : 16;
  u32 early_synd_ext_client_qactive : 16;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_early_syndrome_reg_qactive_u
{
  struct ipa_hwio_def_ipa_pmu_early_syndrome_reg_qactive_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_EARLY_SYNDROME_REG_FSM_STATE 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_early_syndrome_reg_fsm_state_s
{
  u32 early_synd_fsm_state : 6;
  u32 reserved0 : 26;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_early_syndrome_reg_fsm_state_u
{
  struct ipa_hwio_def_ipa_pmu_early_syndrome_reg_fsm_state_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_CONTROL_1 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_control_1_s
{
  u32 dsqactive_macro_sleep_select : 1;
  u32 fwd_ds_qactive_for_micro2macro : 1;
  u32 pmu_clkon_for_any_client_macro_notification : 1;
  u32 skip_qc_shadow_micro_sleep_state : 1;
  u32 disable_sw_qactive_macro_intr : 1;
  u32 pmu_hysteresis_counter_value : 16;
  u32 pmu_sw_reset : 1;
  u32 reserved0 : 10;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_control_1_u
{
  struct ipa_hwio_def_ipa_pmu_control_1_s def;
  u32 value;
};

/*----------------------------------------------------------------------------
 * MODULE: IPA_PMU_STATS_PMU_STATS
 *--------------------------------------------------------------------------*/

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_STATS_MICRO_SLEEP_COUNTER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_stats_micro_sleep_counter_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_stats_micro_sleep_counter_u
{
  struct ipa_hwio_def_ipa_pmu_stats_micro_sleep_counter_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of register: IPA_PMU_STATS_MICRO_SLEEP_RESIDENCY_COUNTER 
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_ipa_pmu_stats_micro_sleep_residency_counter_s
{
  u32 value : 32;
};

/* Union definition of register */
union ipa_hwio_def_ipa_pmu_stats_micro_sleep_residency_counter_u
{
  struct ipa_hwio_def_ipa_pmu_stats_micro_sleep_residency_counter_s def;
  u32 value;
};

/*===========================================================================*/
/*!
  @brief Bit Field definition of fc_stats
*/
/*===========================================================================*/
/* Structure definition of register */
struct ipa_hwio_def_fc_stats_state_s
{
  u32 reserved0 : 16;
  u32 flow_control : 1;
  u32 flow_control_primary : 1;
  u32 flow_control_secondary : 1;
  u32 pending_flow_control : 1;
  u32 reserved1 : 12;
};

/* Union definition of register */
union ipa_hwio_def_fc_stats_state_u
{
  struct ipa_hwio_def_fc_stats_state_s def;
  u32 value;
};

#endif /* __IPA_HWIO_DEF_H__ */
