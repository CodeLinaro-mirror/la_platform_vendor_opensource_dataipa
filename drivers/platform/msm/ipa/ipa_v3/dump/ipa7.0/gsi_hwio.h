// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.​
 */

#ifndef __GSI_HWIO_H__
#define __GSI_HWIO_H__
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

  Attribute definitions for the HWIO_*_ATTR macros are as follows:
    0x0: Command register
    0x1: Read-Only
    0x2: Write-Only
    0x3: Read/Write
*/

/*----------------------------------------------------------------------------
 * MODULE: GSI_TOP_GSI
 *--------------------------------------------------------------------------*/

#define GSI_TOP_GSI_REG_BASE                                                                       (IPA_WRAPPER_BASE      + 0x00000000)
#define GSI_TOP_GSI_REG_BASE_PHYS                                                                  (IPA_WRAPPER_BASE_PHYS + 0x00000000)
#define GSI_TOP_GSI_REG_BASE_OFFS                                                                  0x00000000

#define HWIO_GSI_CFG_ADDR                                                                          (GSI_TOP_GSI_REG_BASE      + 0x00004000)
#define HWIO_GSI_CFG_PHYS                                                                          (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004000)
#define HWIO_GSI_CFG_OFFS                                                                          (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004000)
#define HWIO_GSI_CFG_RMSK                                                                               0xf31
#define HWIO_GSI_CFG_ATTR                                                                                 0x3
#define HWIO_GSI_CFG_IN          \
        in_dword_masked(HWIO_GSI_CFG_ADDR, HWIO_GSI_CFG_RMSK)
#define HWIO_GSI_CFG_INM(m)      \
        in_dword_masked(HWIO_GSI_CFG_ADDR, m)
#define HWIO_GSI_CFG_OUT(v)      \
        out_dword(HWIO_GSI_CFG_ADDR,v)
#define HWIO_GSI_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_CFG_ADDR,m,v,HWIO_GSI_CFG_IN)
#define HWIO_GSI_CFG_SLEEP_CLK_DIV_BMSK                                                                 0xf00
#define HWIO_GSI_CFG_SLEEP_CLK_DIV_SHFT                                                                   0x8
#define HWIO_GSI_CFG_BP_MTRIX_DISABLE_BMSK                                                               0x20
#define HWIO_GSI_CFG_BP_MTRIX_DISABLE_SHFT                                                                0x5
#define HWIO_GSI_CFG_GSI_PWR_CLPS_BMSK                                                                   0x10
#define HWIO_GSI_CFG_GSI_PWR_CLPS_SHFT                                                                    0x4
#define HWIO_GSI_CFG_GSI_ENABLE_BMSK                                                                      0x1
#define HWIO_GSI_CFG_GSI_ENABLE_SHFT                                                                      0x0

#define HWIO_GSI_MANAGER_MCS_CODE_VER_ADDR                                                         (GSI_TOP_GSI_REG_BASE      + 0x00004008)
#define HWIO_GSI_MANAGER_MCS_CODE_VER_PHYS                                                         (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004008)
#define HWIO_GSI_MANAGER_MCS_CODE_VER_OFFS                                                         (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004008)
#define HWIO_GSI_MANAGER_MCS_CODE_VER_RMSK                                                         0xffffffff
#define HWIO_GSI_MANAGER_MCS_CODE_VER_ATTR                                                                0x3
#define HWIO_GSI_MANAGER_MCS_CODE_VER_IN          \
        in_dword_masked(HWIO_GSI_MANAGER_MCS_CODE_VER_ADDR, HWIO_GSI_MANAGER_MCS_CODE_VER_RMSK)
#define HWIO_GSI_MANAGER_MCS_CODE_VER_INM(m)      \
        in_dword_masked(HWIO_GSI_MANAGER_MCS_CODE_VER_ADDR, m)
#define HWIO_GSI_MANAGER_MCS_CODE_VER_OUT(v)      \
        out_dword(HWIO_GSI_MANAGER_MCS_CODE_VER_ADDR,v)
#define HWIO_GSI_MANAGER_MCS_CODE_VER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_MANAGER_MCS_CODE_VER_ADDR,m,v,HWIO_GSI_MANAGER_MCS_CODE_VER_IN)
#define HWIO_GSI_MANAGER_MCS_CODE_VER_VER_BMSK                                                     0xffffffff
#define HWIO_GSI_MANAGER_MCS_CODE_VER_VER_SHFT                                                            0x0

#define HWIO_GSI_ZEROS_ADDR                                                                        (GSI_TOP_GSI_REG_BASE      + 0x00004010)
#define HWIO_GSI_ZEROS_PHYS                                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004010)
#define HWIO_GSI_ZEROS_OFFS                                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004010)
#define HWIO_GSI_ZEROS_RMSK                                                                        0xffffffff
#define HWIO_GSI_ZEROS_ATTR                                                                               0x1
#define HWIO_GSI_ZEROS_IN          \
        in_dword_masked(HWIO_GSI_ZEROS_ADDR, HWIO_GSI_ZEROS_RMSK)
#define HWIO_GSI_ZEROS_INM(m)      \
        in_dword_masked(HWIO_GSI_ZEROS_ADDR, m)
#define HWIO_GSI_ZEROS_ZEROS_BMSK                                                                  0xffffffff
#define HWIO_GSI_ZEROS_ZEROS_SHFT                                                                         0x0

#define HWIO_GSI_PERIPH_BASE_ADDR_LSB_ADDR                                                         (GSI_TOP_GSI_REG_BASE      + 0x00004018)
#define HWIO_GSI_PERIPH_BASE_ADDR_LSB_PHYS                                                         (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004018)
#define HWIO_GSI_PERIPH_BASE_ADDR_LSB_OFFS                                                         (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004018)
#define HWIO_GSI_PERIPH_BASE_ADDR_LSB_RMSK                                                         0xffffffff
#define HWIO_GSI_PERIPH_BASE_ADDR_LSB_ATTR                                                                0x3
#define HWIO_GSI_PERIPH_BASE_ADDR_LSB_IN          \
        in_dword_masked(HWIO_GSI_PERIPH_BASE_ADDR_LSB_ADDR, HWIO_GSI_PERIPH_BASE_ADDR_LSB_RMSK)
#define HWIO_GSI_PERIPH_BASE_ADDR_LSB_INM(m)      \
        in_dword_masked(HWIO_GSI_PERIPH_BASE_ADDR_LSB_ADDR, m)
#define HWIO_GSI_PERIPH_BASE_ADDR_LSB_OUT(v)      \
        out_dword(HWIO_GSI_PERIPH_BASE_ADDR_LSB_ADDR,v)
#define HWIO_GSI_PERIPH_BASE_ADDR_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_PERIPH_BASE_ADDR_LSB_ADDR,m,v,HWIO_GSI_PERIPH_BASE_ADDR_LSB_IN)
#define HWIO_GSI_PERIPH_BASE_ADDR_LSB_BASE_ADDR_BMSK                                               0xffffffff
#define HWIO_GSI_PERIPH_BASE_ADDR_LSB_BASE_ADDR_SHFT                                                      0x0

#define HWIO_GSI_PERIPH_BASE_ADDR_MSB_ADDR                                                         (GSI_TOP_GSI_REG_BASE      + 0x0000401c)
#define HWIO_GSI_PERIPH_BASE_ADDR_MSB_PHYS                                                         (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000401c)
#define HWIO_GSI_PERIPH_BASE_ADDR_MSB_OFFS                                                         (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000401c)
#define HWIO_GSI_PERIPH_BASE_ADDR_MSB_RMSK                                                         0xffffffff
#define HWIO_GSI_PERIPH_BASE_ADDR_MSB_ATTR                                                                0x3
#define HWIO_GSI_PERIPH_BASE_ADDR_MSB_IN          \
        in_dword_masked(HWIO_GSI_PERIPH_BASE_ADDR_MSB_ADDR, HWIO_GSI_PERIPH_BASE_ADDR_MSB_RMSK)
#define HWIO_GSI_PERIPH_BASE_ADDR_MSB_INM(m)      \
        in_dword_masked(HWIO_GSI_PERIPH_BASE_ADDR_MSB_ADDR, m)
#define HWIO_GSI_PERIPH_BASE_ADDR_MSB_OUT(v)      \
        out_dword(HWIO_GSI_PERIPH_BASE_ADDR_MSB_ADDR,v)
#define HWIO_GSI_PERIPH_BASE_ADDR_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_PERIPH_BASE_ADDR_MSB_ADDR,m,v,HWIO_GSI_PERIPH_BASE_ADDR_MSB_IN)
#define HWIO_GSI_PERIPH_BASE_ADDR_MSB_BASE_ADDR_BMSK                                               0xffffffff
#define HWIO_GSI_PERIPH_BASE_ADDR_MSB_BASE_ADDR_SHFT                                                      0x0

#define HWIO_GSI_CGC_CTRL_ADDR                                                                     (GSI_TOP_GSI_REG_BASE      + 0x00004020)
#define HWIO_GSI_CGC_CTRL_PHYS                                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004020)
#define HWIO_GSI_CGC_CTRL_OFFS                                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004020)
#define HWIO_GSI_CGC_CTRL_RMSK                                                                     0xff0fffff
#define HWIO_GSI_CGC_CTRL_ATTR                                                                            0x3
#define HWIO_GSI_CGC_CTRL_IN          \
        in_dword_masked(HWIO_GSI_CGC_CTRL_ADDR, HWIO_GSI_CGC_CTRL_RMSK)
#define HWIO_GSI_CGC_CTRL_INM(m)      \
        in_dword_masked(HWIO_GSI_CGC_CTRL_ADDR, m)
#define HWIO_GSI_CGC_CTRL_OUT(v)      \
        out_dword(HWIO_GSI_CGC_CTRL_ADDR,v)
#define HWIO_GSI_CGC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_CGC_CTRL_ADDR,m,v,HWIO_GSI_CGC_CTRL_IN)
#define HWIO_GSI_CGC_CTRL_REGION_25_32_HW_CGC_EN_BMSK                                              0xff000000
#define HWIO_GSI_CGC_CTRL_REGION_25_32_HW_CGC_EN_SHFT                                                    0x18
#define HWIO_GSI_CGC_CTRL_REGION_20_HW_CGC_EN_BMSK                                                    0x80000
#define HWIO_GSI_CGC_CTRL_REGION_20_HW_CGC_EN_SHFT                                                       0x13
#define HWIO_GSI_CGC_CTRL_REGION_19_HW_CGC_EN_BMSK                                                    0x40000
#define HWIO_GSI_CGC_CTRL_REGION_19_HW_CGC_EN_SHFT                                                       0x12
#define HWIO_GSI_CGC_CTRL_REGION_18_HW_CGC_EN_BMSK                                                    0x20000
#define HWIO_GSI_CGC_CTRL_REGION_18_HW_CGC_EN_SHFT                                                       0x11
#define HWIO_GSI_CGC_CTRL_REGION_17_HW_CGC_EN_BMSK                                                    0x10000
#define HWIO_GSI_CGC_CTRL_REGION_17_HW_CGC_EN_SHFT                                                       0x10
#define HWIO_GSI_CGC_CTRL_REGION_16_HW_CGC_EN_BMSK                                                     0x8000
#define HWIO_GSI_CGC_CTRL_REGION_16_HW_CGC_EN_SHFT                                                        0xf
#define HWIO_GSI_CGC_CTRL_REGION_15_HW_CGC_EN_BMSK                                                     0x4000
#define HWIO_GSI_CGC_CTRL_REGION_15_HW_CGC_EN_SHFT                                                        0xe
#define HWIO_GSI_CGC_CTRL_REGION_14_HW_CGC_EN_BMSK                                                     0x2000
#define HWIO_GSI_CGC_CTRL_REGION_14_HW_CGC_EN_SHFT                                                        0xd
#define HWIO_GSI_CGC_CTRL_REGION_13_HW_CGC_EN_BMSK                                                     0x1000
#define HWIO_GSI_CGC_CTRL_REGION_13_HW_CGC_EN_SHFT                                                        0xc
#define HWIO_GSI_CGC_CTRL_REGION_12_HW_CGC_EN_BMSK                                                      0x800
#define HWIO_GSI_CGC_CTRL_REGION_12_HW_CGC_EN_SHFT                                                        0xb
#define HWIO_GSI_CGC_CTRL_REGION_11_HW_CGC_EN_BMSK                                                      0x400
#define HWIO_GSI_CGC_CTRL_REGION_11_HW_CGC_EN_SHFT                                                        0xa
#define HWIO_GSI_CGC_CTRL_REGION_10_HW_CGC_EN_BMSK                                                      0x200
#define HWIO_GSI_CGC_CTRL_REGION_10_HW_CGC_EN_SHFT                                                        0x9
#define HWIO_GSI_CGC_CTRL_REGION_9_HW_CGC_EN_BMSK                                                       0x100
#define HWIO_GSI_CGC_CTRL_REGION_9_HW_CGC_EN_SHFT                                                         0x8
#define HWIO_GSI_CGC_CTRL_REGION_8_HW_CGC_EN_BMSK                                                        0x80
#define HWIO_GSI_CGC_CTRL_REGION_8_HW_CGC_EN_SHFT                                                         0x7
#define HWIO_GSI_CGC_CTRL_REGION_7_HW_CGC_EN_BMSK                                                        0x40
#define HWIO_GSI_CGC_CTRL_REGION_7_HW_CGC_EN_SHFT                                                         0x6
#define HWIO_GSI_CGC_CTRL_REGION_6_HW_CGC_EN_BMSK                                                        0x20
#define HWIO_GSI_CGC_CTRL_REGION_6_HW_CGC_EN_SHFT                                                         0x5
#define HWIO_GSI_CGC_CTRL_REGION_5_HW_CGC_EN_BMSK                                                        0x10
#define HWIO_GSI_CGC_CTRL_REGION_5_HW_CGC_EN_SHFT                                                         0x4
#define HWIO_GSI_CGC_CTRL_REGION_4_HW_CGC_EN_BMSK                                                         0x8
#define HWIO_GSI_CGC_CTRL_REGION_4_HW_CGC_EN_SHFT                                                         0x3
#define HWIO_GSI_CGC_CTRL_REGION_3_HW_CGC_EN_BMSK                                                         0x4
#define HWIO_GSI_CGC_CTRL_REGION_3_HW_CGC_EN_SHFT                                                         0x2
#define HWIO_GSI_CGC_CTRL_REGION_2_HW_CGC_EN_BMSK                                                         0x2
#define HWIO_GSI_CGC_CTRL_REGION_2_HW_CGC_EN_SHFT                                                         0x1
#define HWIO_GSI_CGC_CTRL_REGION_1_HW_CGC_EN_BMSK                                                         0x1
#define HWIO_GSI_CGC_CTRL_REGION_1_HW_CGC_EN_SHFT                                                         0x0

#define HWIO_GSI_MOQA_CFG_ADDR                                                                     (GSI_TOP_GSI_REG_BASE      + 0x00004030)
#define HWIO_GSI_MOQA_CFG_PHYS                                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004030)
#define HWIO_GSI_MOQA_CFG_OFFS                                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004030)
#define HWIO_GSI_MOQA_CFG_RMSK                                                                       0xffffff
#define HWIO_GSI_MOQA_CFG_ATTR                                                                            0x3
#define HWIO_GSI_MOQA_CFG_IN          \
        in_dword_masked(HWIO_GSI_MOQA_CFG_ADDR, HWIO_GSI_MOQA_CFG_RMSK)
#define HWIO_GSI_MOQA_CFG_INM(m)      \
        in_dword_masked(HWIO_GSI_MOQA_CFG_ADDR, m)
#define HWIO_GSI_MOQA_CFG_OUT(v)      \
        out_dword(HWIO_GSI_MOQA_CFG_ADDR,v)
#define HWIO_GSI_MOQA_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_MOQA_CFG_ADDR,m,v,HWIO_GSI_MOQA_CFG_IN)
#define HWIO_GSI_MOQA_CFG_CLIENT_OOWR_BMSK                                                           0xff0000
#define HWIO_GSI_MOQA_CFG_CLIENT_OOWR_SHFT                                                               0x10
#define HWIO_GSI_MOQA_CFG_CLIENT_OORD_BMSK                                                             0xff00
#define HWIO_GSI_MOQA_CFG_CLIENT_OORD_SHFT                                                                0x8
#define HWIO_GSI_MOQA_CFG_CLIENT_REQ_PRIO_BMSK                                                           0xff
#define HWIO_GSI_MOQA_CFG_CLIENT_REQ_PRIO_SHFT                                                            0x0

#define HWIO_GSI_REE_CFG_ADDR                                                                      (GSI_TOP_GSI_REG_BASE      + 0x00004038)
#define HWIO_GSI_REE_CFG_PHYS                                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004038)
#define HWIO_GSI_REE_CFG_OFFS                                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004038)
#define HWIO_GSI_REE_CFG_RMSK                                                                         0xfff03
#define HWIO_GSI_REE_CFG_ATTR                                                                             0x3
#define HWIO_GSI_REE_CFG_IN          \
        in_dword_masked(HWIO_GSI_REE_CFG_ADDR, HWIO_GSI_REE_CFG_RMSK)
#define HWIO_GSI_REE_CFG_INM(m)      \
        in_dword_masked(HWIO_GSI_REE_CFG_ADDR, m)
#define HWIO_GSI_REE_CFG_OUT(v)      \
        out_dword(HWIO_GSI_REE_CFG_ADDR,v)
#define HWIO_GSI_REE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_REE_CFG_ADDR,m,v,HWIO_GSI_REE_CFG_IN)
#define HWIO_GSI_REE_CFG_REE_MAX_NEW_RE_INT_BMSK                                                      0xf0000
#define HWIO_GSI_REE_CFG_REE_MAX_NEW_RE_INT_SHFT                                                         0x10
#define HWIO_GSI_REE_CFG_MAX_BURST_SIZE_BMSK                                                           0xff00
#define HWIO_GSI_REE_CFG_MAX_BURST_SIZE_SHFT                                                              0x8
#define HWIO_GSI_REE_CFG_CHANNEL_EMPTY_INT_ENABLE_BMSK                                                    0x2
#define HWIO_GSI_REE_CFG_CHANNEL_EMPTY_INT_ENABLE_SHFT                                                    0x1
#define HWIO_GSI_REE_CFG_MOVE_TO_ESC_CLR_MODE_TRSH_BMSK                                                   0x1
#define HWIO_GSI_REE_CFG_MOVE_TO_ESC_CLR_MODE_TRSH_SHFT                                                   0x0

#define HWIO_GSI_PERIPH_PENDING_n_ADDR(n)                                                          (GSI_TOP_GSI_REG_BASE      + 0x00004060 + 0x4 * (n))
#define HWIO_GSI_PERIPH_PENDING_n_PHYS(n)                                                          (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004060 + 0x4 * (n))
#define HWIO_GSI_PERIPH_PENDING_n_OFFS(n)                                                          (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004060 + 0x4 * (n))
#define HWIO_GSI_PERIPH_PENDING_n_RMSK                                                             0xffffffff
#define HWIO_GSI_PERIPH_PENDING_n_MAXn                                                                      2
#define HWIO_GSI_PERIPH_PENDING_n_ATTR                                                                    0x1
#define HWIO_GSI_PERIPH_PENDING_n_INI(n)        \
        in_dword_masked(HWIO_GSI_PERIPH_PENDING_n_ADDR(n), HWIO_GSI_PERIPH_PENDING_n_RMSK)
#define HWIO_GSI_PERIPH_PENDING_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_PERIPH_PENDING_n_ADDR(n), mask)
#define HWIO_GSI_PERIPH_PENDING_n_CHID_BIT_MAP_BMSK                                                0xffffffff
#define HWIO_GSI_PERIPH_PENDING_n_CHID_BIT_MAP_SHFT                                                       0x0

#define HWIO_GSI_MSI_CACHEATTR_ADDR                                                                (GSI_TOP_GSI_REG_BASE      + 0x00004080)
#define HWIO_GSI_MSI_CACHEATTR_PHYS                                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004080)
#define HWIO_GSI_MSI_CACHEATTR_OFFS                                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004080)
#define HWIO_GSI_MSI_CACHEATTR_RMSK                                                                      0x3f
#define HWIO_GSI_MSI_CACHEATTR_ATTR                                                                       0x3
#define HWIO_GSI_MSI_CACHEATTR_IN          \
        in_dword_masked(HWIO_GSI_MSI_CACHEATTR_ADDR, HWIO_GSI_MSI_CACHEATTR_RMSK)
#define HWIO_GSI_MSI_CACHEATTR_INM(m)      \
        in_dword_masked(HWIO_GSI_MSI_CACHEATTR_ADDR, m)
#define HWIO_GSI_MSI_CACHEATTR_OUT(v)      \
        out_dword(HWIO_GSI_MSI_CACHEATTR_ADDR,v)
#define HWIO_GSI_MSI_CACHEATTR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_MSI_CACHEATTR_ADDR,m,v,HWIO_GSI_MSI_CACHEATTR_IN)
#define HWIO_GSI_MSI_CACHEATTR_AREQPRIORITY_BMSK                                                         0x30
#define HWIO_GSI_MSI_CACHEATTR_AREQPRIORITY_SHFT                                                          0x4
#define HWIO_GSI_MSI_CACHEATTR_ATRANSIENT_BMSK                                                            0x8
#define HWIO_GSI_MSI_CACHEATTR_ATRANSIENT_SHFT                                                            0x3
#define HWIO_GSI_MSI_CACHEATTR_ANOALLOCATE_BMSK                                                           0x4
#define HWIO_GSI_MSI_CACHEATTR_ANOALLOCATE_SHFT                                                           0x2
#define HWIO_GSI_MSI_CACHEATTR_AINNERSHARED_BMSK                                                          0x2
#define HWIO_GSI_MSI_CACHEATTR_AINNERSHARED_SHFT                                                          0x1
#define HWIO_GSI_MSI_CACHEATTR_ASHARED_BMSK                                                               0x1
#define HWIO_GSI_MSI_CACHEATTR_ASHARED_SHFT                                                               0x0

#define HWIO_GSI_EVENT_CACHEATTR_ADDR                                                              (GSI_TOP_GSI_REG_BASE      + 0x00004084)
#define HWIO_GSI_EVENT_CACHEATTR_PHYS                                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004084)
#define HWIO_GSI_EVENT_CACHEATTR_OFFS                                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004084)
#define HWIO_GSI_EVENT_CACHEATTR_RMSK                                                                    0x3f
#define HWIO_GSI_EVENT_CACHEATTR_ATTR                                                                     0x3
#define HWIO_GSI_EVENT_CACHEATTR_IN          \
        in_dword_masked(HWIO_GSI_EVENT_CACHEATTR_ADDR, HWIO_GSI_EVENT_CACHEATTR_RMSK)
#define HWIO_GSI_EVENT_CACHEATTR_INM(m)      \
        in_dword_masked(HWIO_GSI_EVENT_CACHEATTR_ADDR, m)
#define HWIO_GSI_EVENT_CACHEATTR_OUT(v)      \
        out_dword(HWIO_GSI_EVENT_CACHEATTR_ADDR,v)
#define HWIO_GSI_EVENT_CACHEATTR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_EVENT_CACHEATTR_ADDR,m,v,HWIO_GSI_EVENT_CACHEATTR_IN)
#define HWIO_GSI_EVENT_CACHEATTR_AREQPRIORITY_BMSK                                                       0x30
#define HWIO_GSI_EVENT_CACHEATTR_AREQPRIORITY_SHFT                                                        0x4
#define HWIO_GSI_EVENT_CACHEATTR_ATRANSIENT_BMSK                                                          0x8
#define HWIO_GSI_EVENT_CACHEATTR_ATRANSIENT_SHFT                                                          0x3
#define HWIO_GSI_EVENT_CACHEATTR_ANOALLOCATE_BMSK                                                         0x4
#define HWIO_GSI_EVENT_CACHEATTR_ANOALLOCATE_SHFT                                                         0x2
#define HWIO_GSI_EVENT_CACHEATTR_AINNERSHARED_BMSK                                                        0x2
#define HWIO_GSI_EVENT_CACHEATTR_AINNERSHARED_SHFT                                                        0x1
#define HWIO_GSI_EVENT_CACHEATTR_ASHARED_BMSK                                                             0x1
#define HWIO_GSI_EVENT_CACHEATTR_ASHARED_SHFT                                                             0x0

#define HWIO_GSI_DATA_CACHEATTR_ADDR                                                               (GSI_TOP_GSI_REG_BASE      + 0x00004088)
#define HWIO_GSI_DATA_CACHEATTR_PHYS                                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004088)
#define HWIO_GSI_DATA_CACHEATTR_OFFS                                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004088)
#define HWIO_GSI_DATA_CACHEATTR_RMSK                                                                     0x3f
#define HWIO_GSI_DATA_CACHEATTR_ATTR                                                                      0x3
#define HWIO_GSI_DATA_CACHEATTR_IN          \
        in_dword_masked(HWIO_GSI_DATA_CACHEATTR_ADDR, HWIO_GSI_DATA_CACHEATTR_RMSK)
#define HWIO_GSI_DATA_CACHEATTR_INM(m)      \
        in_dword_masked(HWIO_GSI_DATA_CACHEATTR_ADDR, m)
#define HWIO_GSI_DATA_CACHEATTR_OUT(v)      \
        out_dword(HWIO_GSI_DATA_CACHEATTR_ADDR,v)
#define HWIO_GSI_DATA_CACHEATTR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_DATA_CACHEATTR_ADDR,m,v,HWIO_GSI_DATA_CACHEATTR_IN)
#define HWIO_GSI_DATA_CACHEATTR_AREQPRIORITY_BMSK                                                        0x30
#define HWIO_GSI_DATA_CACHEATTR_AREQPRIORITY_SHFT                                                         0x4
#define HWIO_GSI_DATA_CACHEATTR_ATRANSIENT_BMSK                                                           0x8
#define HWIO_GSI_DATA_CACHEATTR_ATRANSIENT_SHFT                                                           0x3
#define HWIO_GSI_DATA_CACHEATTR_ANOALLOCATE_BMSK                                                          0x4
#define HWIO_GSI_DATA_CACHEATTR_ANOALLOCATE_SHFT                                                          0x2
#define HWIO_GSI_DATA_CACHEATTR_AINNERSHARED_BMSK                                                         0x2
#define HWIO_GSI_DATA_CACHEATTR_AINNERSHARED_SHFT                                                         0x1
#define HWIO_GSI_DATA_CACHEATTR_ASHARED_BMSK                                                              0x1
#define HWIO_GSI_DATA_CACHEATTR_ASHARED_SHFT                                                              0x0

#define HWIO_GSI_TRE_CACHEATTR_ADDR                                                                (GSI_TOP_GSI_REG_BASE      + 0x00004090)
#define HWIO_GSI_TRE_CACHEATTR_PHYS                                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004090)
#define HWIO_GSI_TRE_CACHEATTR_OFFS                                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004090)
#define HWIO_GSI_TRE_CACHEATTR_RMSK                                                                      0x3f
#define HWIO_GSI_TRE_CACHEATTR_ATTR                                                                       0x3
#define HWIO_GSI_TRE_CACHEATTR_IN          \
        in_dword_masked(HWIO_GSI_TRE_CACHEATTR_ADDR, HWIO_GSI_TRE_CACHEATTR_RMSK)
#define HWIO_GSI_TRE_CACHEATTR_INM(m)      \
        in_dword_masked(HWIO_GSI_TRE_CACHEATTR_ADDR, m)
#define HWIO_GSI_TRE_CACHEATTR_OUT(v)      \
        out_dword(HWIO_GSI_TRE_CACHEATTR_ADDR,v)
#define HWIO_GSI_TRE_CACHEATTR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_TRE_CACHEATTR_ADDR,m,v,HWIO_GSI_TRE_CACHEATTR_IN)
#define HWIO_GSI_TRE_CACHEATTR_AREQPRIORITY_BMSK                                                         0x30
#define HWIO_GSI_TRE_CACHEATTR_AREQPRIORITY_SHFT                                                          0x4
#define HWIO_GSI_TRE_CACHEATTR_ATRANSIENT_BMSK                                                            0x8
#define HWIO_GSI_TRE_CACHEATTR_ATRANSIENT_SHFT                                                            0x3
#define HWIO_GSI_TRE_CACHEATTR_ANOALLOCATE_BMSK                                                           0x4
#define HWIO_GSI_TRE_CACHEATTR_ANOALLOCATE_SHFT                                                           0x2
#define HWIO_GSI_TRE_CACHEATTR_AINNERSHARED_BMSK                                                          0x2
#define HWIO_GSI_TRE_CACHEATTR_AINNERSHARED_SHFT                                                          0x1
#define HWIO_GSI_TRE_CACHEATTR_ASHARED_BMSK                                                               0x1
#define HWIO_GSI_TRE_CACHEATTR_ASHARED_SHFT                                                               0x0

#define HWIO_IC_PROCESS_DESC_BCK_PRS_ADDR                                                          (GSI_TOP_GSI_REG_BASE      + 0x000040a0)
#define HWIO_IC_PROCESS_DESC_BCK_PRS_PHYS                                                          (GSI_TOP_GSI_REG_BASE_PHYS + 0x000040a0)
#define HWIO_IC_PROCESS_DESC_BCK_PRS_OFFS                                                          (GSI_TOP_GSI_REG_BASE_OFFS + 0x000040a0)
#define HWIO_IC_PROCESS_DESC_BCK_PRS_RMSK                                                             0x3ffff
#define HWIO_IC_PROCESS_DESC_BCK_PRS_ATTR                                                                 0x1
#define HWIO_IC_PROCESS_DESC_BCK_PRS_IN          \
        in_dword_masked(HWIO_IC_PROCESS_DESC_BCK_PRS_ADDR, HWIO_IC_PROCESS_DESC_BCK_PRS_RMSK)
#define HWIO_IC_PROCESS_DESC_BCK_PRS_INM(m)      \
        in_dword_masked(HWIO_IC_PROCESS_DESC_BCK_PRS_ADDR, m)
#define HWIO_IC_PROCESS_DESC_BCK_PRS_IE_INT_MOD_CANCEL_INT_BMSK                                       0x20000
#define HWIO_IC_PROCESS_DESC_BCK_PRS_IE_INT_MOD_CANCEL_INT_SHFT                                          0x11
#define HWIO_IC_PROCESS_DESC_BCK_PRS_WR_INT_BMSK                                                      0x10000
#define HWIO_IC_PROCESS_DESC_BCK_PRS_WR_INT_SHFT                                                         0x10
#define HWIO_IC_PROCESS_DESC_BCK_PRS_RD_INT_BMSK                                                       0x8000
#define HWIO_IC_PROCESS_DESC_BCK_PRS_RD_INT_SHFT                                                          0xf
#define HWIO_IC_PROCESS_DESC_BCK_PRS_DB_NEW_MSI_DB_INT_BMSK                                            0x4000
#define HWIO_IC_PROCESS_DESC_BCK_PRS_DB_NEW_MSI_DB_INT_SHFT                                               0xe
#define HWIO_IC_PROCESS_DESC_BCK_PRS_DB_NEW_DB_INT_BMSK                                                0x2000
#define HWIO_IC_PROCESS_DESC_BCK_PRS_DB_NEW_DB_INT_SHFT                                                   0xd
#define HWIO_IC_PROCESS_DESC_BCK_PRS_TIMER_ENG_INT_BMSK                                                0x1000
#define HWIO_IC_PROCESS_DESC_BCK_PRS_TIMER_ENG_INT_SHFT                                                   0xc
#define HWIO_IC_PROCESS_DESC_BCK_PRS_TLV_CH_NOT_FULL_INT_BMSK                                           0x800
#define HWIO_IC_PROCESS_DESC_BCK_PRS_TLV_CH_NOT_FULL_INT_SHFT                                             0xb
#define HWIO_IC_PROCESS_DESC_BCK_PRS_TLV_RST_INT_BMSK                                                   0x400
#define HWIO_IC_PROCESS_DESC_BCK_PRS_TLV_RST_INT_SHFT                                                     0xa
#define HWIO_IC_PROCESS_DESC_BCK_PRS_TLV_STOP_INT_BMSK                                                  0x200
#define HWIO_IC_PROCESS_DESC_BCK_PRS_TLV_STOP_INT_SHFT                                                    0x9
#define HWIO_IC_PROCESS_DESC_BCK_PRS_TLV_DESC_PROC_COMP_INT_BMSK                                        0x100
#define HWIO_IC_PROCESS_DESC_BCK_PRS_TLV_DESC_PROC_COMP_INT_SHFT                                          0x8
#define HWIO_IC_PROCESS_DESC_BCK_PRS_CSR_GENERIC_COM_INT_BMSK                                            0x80
#define HWIO_IC_PROCESS_DESC_BCK_PRS_CSR_GENERIC_COM_INT_SHFT                                             0x7
#define HWIO_IC_PROCESS_DESC_BCK_PRS_CSR_CH_EV_COM_INT_BMSK                                              0x40
#define HWIO_IC_PROCESS_DESC_BCK_PRS_CSR_CH_EV_COM_INT_SHFT                                               0x6
#define HWIO_IC_PROCESS_DESC_BCK_PRS_IE_NOTIFY_MCS_INT_BMSK                                              0x20
#define HWIO_IC_PROCESS_DESC_BCK_PRS_IE_NOTIFY_MCS_INT_SHFT                                               0x5
#define HWIO_IC_PROCESS_DESC_BCK_PRS_IE_INT_MOD_STOP_INT_BMSK                                            0x10
#define HWIO_IC_PROCESS_DESC_BCK_PRS_IE_INT_MOD_STOP_INT_SHFT                                             0x4
#define HWIO_IC_PROCESS_DESC_BCK_PRS_EV_ENG_INT_BMSK                                                      0x8
#define HWIO_IC_PROCESS_DESC_BCK_PRS_EV_ENG_INT_SHFT                                                      0x3
#define HWIO_IC_PROCESS_DESC_BCK_PRS_REE_CH_EMPTY_INT_BMSK                                                0x4
#define HWIO_IC_PROCESS_DESC_BCK_PRS_REE_CH_EMPTY_INT_SHFT                                                0x2
#define HWIO_IC_PROCESS_DESC_BCK_PRS_REE_NEW_RE_INT_BMSK                                                  0x2
#define HWIO_IC_PROCESS_DESC_BCK_PRS_REE_NEW_RE_INT_SHFT                                                  0x1
#define HWIO_IC_PROCESS_DESC_BCK_PRS_REE_DIS_CH_COMP_INT_BMSK                                             0x1
#define HWIO_IC_PROCESS_DESC_BCK_PRS_REE_DIS_CH_COMP_INT_SHFT                                             0x0

#define HWIO_IC_TLV_STOP_BCK_PRS_ADDR                                                              (GSI_TOP_GSI_REG_BASE      + 0x000040a4)
#define HWIO_IC_TLV_STOP_BCK_PRS_PHYS                                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x000040a4)
#define HWIO_IC_TLV_STOP_BCK_PRS_OFFS                                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x000040a4)
#define HWIO_IC_TLV_STOP_BCK_PRS_RMSK                                                                 0x3ffff
#define HWIO_IC_TLV_STOP_BCK_PRS_ATTR                                                                     0x1
#define HWIO_IC_TLV_STOP_BCK_PRS_IN          \
        in_dword_masked(HWIO_IC_TLV_STOP_BCK_PRS_ADDR, HWIO_IC_TLV_STOP_BCK_PRS_RMSK)
#define HWIO_IC_TLV_STOP_BCK_PRS_INM(m)      \
        in_dword_masked(HWIO_IC_TLV_STOP_BCK_PRS_ADDR, m)
#define HWIO_IC_TLV_STOP_BCK_PRS_IE_INT_MOD_CANCEL_INT_BMSK                                           0x20000
#define HWIO_IC_TLV_STOP_BCK_PRS_IE_INT_MOD_CANCEL_INT_SHFT                                              0x11
#define HWIO_IC_TLV_STOP_BCK_PRS_WR_INT_BMSK                                                          0x10000
#define HWIO_IC_TLV_STOP_BCK_PRS_WR_INT_SHFT                                                             0x10
#define HWIO_IC_TLV_STOP_BCK_PRS_RD_INT_BMSK                                                           0x8000
#define HWIO_IC_TLV_STOP_BCK_PRS_RD_INT_SHFT                                                              0xf
#define HWIO_IC_TLV_STOP_BCK_PRS_DB_NEW_MSI_DB_INT_BMSK                                                0x4000
#define HWIO_IC_TLV_STOP_BCK_PRS_DB_NEW_MSI_DB_INT_SHFT                                                   0xe
#define HWIO_IC_TLV_STOP_BCK_PRS_DB_NEW_DB_INT_BMSK                                                    0x2000
#define HWIO_IC_TLV_STOP_BCK_PRS_DB_NEW_DB_INT_SHFT                                                       0xd
#define HWIO_IC_TLV_STOP_BCK_PRS_TIMER_ENG_INT_BMSK                                                    0x1000
#define HWIO_IC_TLV_STOP_BCK_PRS_TIMER_ENG_INT_SHFT                                                       0xc
#define HWIO_IC_TLV_STOP_BCK_PRS_TLV_CH_NOT_FULL_INT_BMSK                                               0x800
#define HWIO_IC_TLV_STOP_BCK_PRS_TLV_CH_NOT_FULL_INT_SHFT                                                 0xb
#define HWIO_IC_TLV_STOP_BCK_PRS_TLV_RST_INT_BMSK                                                       0x400
#define HWIO_IC_TLV_STOP_BCK_PRS_TLV_RST_INT_SHFT                                                         0xa
#define HWIO_IC_TLV_STOP_BCK_PRS_TLV_STOP_INT_BMSK                                                      0x200
#define HWIO_IC_TLV_STOP_BCK_PRS_TLV_STOP_INT_SHFT                                                        0x9
#define HWIO_IC_TLV_STOP_BCK_PRS_TLV_DESC_PROC_COMP_INT_BMSK                                            0x100
#define HWIO_IC_TLV_STOP_BCK_PRS_TLV_DESC_PROC_COMP_INT_SHFT                                              0x8
#define HWIO_IC_TLV_STOP_BCK_PRS_CSR_GENERIC_COM_INT_BMSK                                                0x80
#define HWIO_IC_TLV_STOP_BCK_PRS_CSR_GENERIC_COM_INT_SHFT                                                 0x7
#define HWIO_IC_TLV_STOP_BCK_PRS_CSR_CH_EV_COM_INT_BMSK                                                  0x40
#define HWIO_IC_TLV_STOP_BCK_PRS_CSR_CH_EV_COM_INT_SHFT                                                   0x6
#define HWIO_IC_TLV_STOP_BCK_PRS_IE_NOTIFY_MCS_INT_BMSK                                                  0x20
#define HWIO_IC_TLV_STOP_BCK_PRS_IE_NOTIFY_MCS_INT_SHFT                                                   0x5
#define HWIO_IC_TLV_STOP_BCK_PRS_IE_INT_MOD_STOP_INT_BMSK                                                0x10
#define HWIO_IC_TLV_STOP_BCK_PRS_IE_INT_MOD_STOP_INT_SHFT                                                 0x4
#define HWIO_IC_TLV_STOP_BCK_PRS_EV_ENG_INT_BMSK                                                          0x8
#define HWIO_IC_TLV_STOP_BCK_PRS_EV_ENG_INT_SHFT                                                          0x3
#define HWIO_IC_TLV_STOP_BCK_PRS_REE_CH_EMPTY_INT_BMSK                                                    0x4
#define HWIO_IC_TLV_STOP_BCK_PRS_REE_CH_EMPTY_INT_SHFT                                                    0x2
#define HWIO_IC_TLV_STOP_BCK_PRS_REE_NEW_RE_INT_BMSK                                                      0x2
#define HWIO_IC_TLV_STOP_BCK_PRS_REE_NEW_RE_INT_SHFT                                                      0x1
#define HWIO_IC_TLV_STOP_BCK_PRS_REE_DIS_CH_COMP_INT_BMSK                                                 0x1
#define HWIO_IC_TLV_STOP_BCK_PRS_REE_DIS_CH_COMP_INT_SHFT                                                 0x0

#define HWIO_IC_TLV_RESET_BCK_PRS_ADDR                                                             (GSI_TOP_GSI_REG_BASE      + 0x000040a8)
#define HWIO_IC_TLV_RESET_BCK_PRS_PHYS                                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x000040a8)
#define HWIO_IC_TLV_RESET_BCK_PRS_OFFS                                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x000040a8)
#define HWIO_IC_TLV_RESET_BCK_PRS_RMSK                                                                0x3ffff
#define HWIO_IC_TLV_RESET_BCK_PRS_ATTR                                                                    0x1
#define HWIO_IC_TLV_RESET_BCK_PRS_IN          \
        in_dword_masked(HWIO_IC_TLV_RESET_BCK_PRS_ADDR, HWIO_IC_TLV_RESET_BCK_PRS_RMSK)
#define HWIO_IC_TLV_RESET_BCK_PRS_INM(m)      \
        in_dword_masked(HWIO_IC_TLV_RESET_BCK_PRS_ADDR, m)
#define HWIO_IC_TLV_RESET_BCK_PRS_IE_INT_MOD_CANCEL_INT_BMSK                                          0x20000
#define HWIO_IC_TLV_RESET_BCK_PRS_IE_INT_MOD_CANCEL_INT_SHFT                                             0x11
#define HWIO_IC_TLV_RESET_BCK_PRS_WR_INT_BMSK                                                         0x10000
#define HWIO_IC_TLV_RESET_BCK_PRS_WR_INT_SHFT                                                            0x10
#define HWIO_IC_TLV_RESET_BCK_PRS_RD_INT_BMSK                                                          0x8000
#define HWIO_IC_TLV_RESET_BCK_PRS_RD_INT_SHFT                                                             0xf
#define HWIO_IC_TLV_RESET_BCK_PRS_DB_NEW_MSI_DB_INT_BMSK                                               0x4000
#define HWIO_IC_TLV_RESET_BCK_PRS_DB_NEW_MSI_DB_INT_SHFT                                                  0xe
#define HWIO_IC_TLV_RESET_BCK_PRS_DB_NEW_DB_INT_BMSK                                                   0x2000
#define HWIO_IC_TLV_RESET_BCK_PRS_DB_NEW_DB_INT_SHFT                                                      0xd
#define HWIO_IC_TLV_RESET_BCK_PRS_TIMER_ENG_INT_BMSK                                                   0x1000
#define HWIO_IC_TLV_RESET_BCK_PRS_TIMER_ENG_INT_SHFT                                                      0xc
#define HWIO_IC_TLV_RESET_BCK_PRS_TLV_CH_NOT_FULL_INT_BMSK                                              0x800
#define HWIO_IC_TLV_RESET_BCK_PRS_TLV_CH_NOT_FULL_INT_SHFT                                                0xb
#define HWIO_IC_TLV_RESET_BCK_PRS_TLV_RST_INT_BMSK                                                      0x400
#define HWIO_IC_TLV_RESET_BCK_PRS_TLV_RST_INT_SHFT                                                        0xa
#define HWIO_IC_TLV_RESET_BCK_PRS_TLV_STOP_INT_BMSK                                                     0x200
#define HWIO_IC_TLV_RESET_BCK_PRS_TLV_STOP_INT_SHFT                                                       0x9
#define HWIO_IC_TLV_RESET_BCK_PRS_TLV_DESC_PROC_COMP_INT_BMSK                                           0x100
#define HWIO_IC_TLV_RESET_BCK_PRS_TLV_DESC_PROC_COMP_INT_SHFT                                             0x8
#define HWIO_IC_TLV_RESET_BCK_PRS_CSR_GENERIC_COM_INT_BMSK                                               0x80
#define HWIO_IC_TLV_RESET_BCK_PRS_CSR_GENERIC_COM_INT_SHFT                                                0x7
#define HWIO_IC_TLV_RESET_BCK_PRS_CSR_CH_EV_COM_INT_BMSK                                                 0x40
#define HWIO_IC_TLV_RESET_BCK_PRS_CSR_CH_EV_COM_INT_SHFT                                                  0x6
#define HWIO_IC_TLV_RESET_BCK_PRS_IE_NOTIFY_MCS_INT_BMSK                                                 0x20
#define HWIO_IC_TLV_RESET_BCK_PRS_IE_NOTIFY_MCS_INT_SHFT                                                  0x5
#define HWIO_IC_TLV_RESET_BCK_PRS_IE_INT_MOD_STOP_INT_BMSK                                               0x10
#define HWIO_IC_TLV_RESET_BCK_PRS_IE_INT_MOD_STOP_INT_SHFT                                                0x4
#define HWIO_IC_TLV_RESET_BCK_PRS_EV_ENG_INT_BMSK                                                         0x8
#define HWIO_IC_TLV_RESET_BCK_PRS_EV_ENG_INT_SHFT                                                         0x3
#define HWIO_IC_TLV_RESET_BCK_PRS_REE_CH_EMPTY_INT_BMSK                                                   0x4
#define HWIO_IC_TLV_RESET_BCK_PRS_REE_CH_EMPTY_INT_SHFT                                                   0x2
#define HWIO_IC_TLV_RESET_BCK_PRS_REE_NEW_RE_INT_BMSK                                                     0x2
#define HWIO_IC_TLV_RESET_BCK_PRS_REE_NEW_RE_INT_SHFT                                                     0x1
#define HWIO_IC_TLV_RESET_BCK_PRS_REE_DIS_CH_COMP_INT_BMSK                                                0x1
#define HWIO_IC_TLV_RESET_BCK_PRS_REE_DIS_CH_COMP_INT_SHFT                                                0x0

#define HWIO_IC_RGSTR_TIMER_BCK_PRS_ADDR                                                           (GSI_TOP_GSI_REG_BASE      + 0x000040ac)
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_PHYS                                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x000040ac)
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_OFFS                                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x000040ac)
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_RMSK                                                              0x3ffff
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_ATTR                                                                  0x1
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_IN          \
        in_dword_masked(HWIO_IC_RGSTR_TIMER_BCK_PRS_ADDR, HWIO_IC_RGSTR_TIMER_BCK_PRS_RMSK)
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_INM(m)      \
        in_dword_masked(HWIO_IC_RGSTR_TIMER_BCK_PRS_ADDR, m)
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_IE_INT_MOD_CANCEL_INT_BMSK                                        0x20000
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_IE_INT_MOD_CANCEL_INT_SHFT                                           0x11
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_WR_INT_BMSK                                                       0x10000
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_WR_INT_SHFT                                                          0x10
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_RD_INT_BMSK                                                        0x8000
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_RD_INT_SHFT                                                           0xf
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_DB_NEW_MSI_DB_INT_BMSK                                             0x4000
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_DB_NEW_MSI_DB_INT_SHFT                                                0xe
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_DB_NEW_DB_INT_BMSK                                                 0x2000
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_DB_NEW_DB_INT_SHFT                                                    0xd
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_TIMER_ENG_INT_BMSK                                                 0x1000
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_TIMER_ENG_INT_SHFT                                                    0xc
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_TLV_CH_NOT_FULL_INT_BMSK                                            0x800
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_TLV_CH_NOT_FULL_INT_SHFT                                              0xb
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_TLV_RST_INT_BMSK                                                    0x400
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_TLV_RST_INT_SHFT                                                      0xa
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_TLV_STOP_INT_BMSK                                                   0x200
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_TLV_STOP_INT_SHFT                                                     0x9
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_TLV_DESC_PROC_COMP_INT_BMSK                                         0x100
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_TLV_DESC_PROC_COMP_INT_SHFT                                           0x8
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_CSR_GENERIC_COM_INT_BMSK                                             0x80
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_CSR_GENERIC_COM_INT_SHFT                                              0x7
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_CSR_CH_EV_COM_INT_BMSK                                               0x40
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_CSR_CH_EV_COM_INT_SHFT                                                0x6
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_IE_NOTIFY_MCS_INT_BMSK                                               0x20
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_IE_NOTIFY_MCS_INT_SHFT                                                0x5
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_IE_INT_MOD_STOP_INT_BMSK                                             0x10
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_IE_INT_MOD_STOP_INT_SHFT                                              0x4
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_EV_ENG_INT_BMSK                                                       0x8
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_EV_ENG_INT_SHFT                                                       0x3
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_REE_CH_EMPTY_INT_BMSK                                                 0x4
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_REE_CH_EMPTY_INT_SHFT                                                 0x2
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_REE_NEW_RE_INT_BMSK                                                   0x2
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_REE_NEW_RE_INT_SHFT                                                   0x1
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_REE_DIS_CH_COMP_INT_BMSK                                              0x1
#define HWIO_IC_RGSTR_TIMER_BCK_PRS_REE_DIS_CH_COMP_INT_SHFT                                              0x0

#define HWIO_IC_ASYNC_BCK_PRS_ADDR                                                                 (GSI_TOP_GSI_REG_BASE      + 0x000040b0)
#define HWIO_IC_ASYNC_BCK_PRS_PHYS                                                                 (GSI_TOP_GSI_REG_BASE_PHYS + 0x000040b0)
#define HWIO_IC_ASYNC_BCK_PRS_OFFS                                                                 (GSI_TOP_GSI_REG_BASE_OFFS + 0x000040b0)
#define HWIO_IC_ASYNC_BCK_PRS_RMSK                                                                    0x3ffff
#define HWIO_IC_ASYNC_BCK_PRS_ATTR                                                                        0x1
#define HWIO_IC_ASYNC_BCK_PRS_IN          \
        in_dword_masked(HWIO_IC_ASYNC_BCK_PRS_ADDR, HWIO_IC_ASYNC_BCK_PRS_RMSK)
#define HWIO_IC_ASYNC_BCK_PRS_INM(m)      \
        in_dword_masked(HWIO_IC_ASYNC_BCK_PRS_ADDR, m)
#define HWIO_IC_ASYNC_BCK_PRS_IE_INT_MOD_CANCEL_INT_BMSK                                              0x20000
#define HWIO_IC_ASYNC_BCK_PRS_IE_INT_MOD_CANCEL_INT_SHFT                                                 0x11
#define HWIO_IC_ASYNC_BCK_PRS_WR_INT_BMSK                                                             0x10000
#define HWIO_IC_ASYNC_BCK_PRS_WR_INT_SHFT                                                                0x10
#define HWIO_IC_ASYNC_BCK_PRS_RD_INT_BMSK                                                              0x8000
#define HWIO_IC_ASYNC_BCK_PRS_RD_INT_SHFT                                                                 0xf
#define HWIO_IC_ASYNC_BCK_PRS_DB_NEW_MSI_DB_INT_BMSK                                                   0x4000
#define HWIO_IC_ASYNC_BCK_PRS_DB_NEW_MSI_DB_INT_SHFT                                                      0xe
#define HWIO_IC_ASYNC_BCK_PRS_DB_NEW_DB_INT_BMSK                                                       0x2000
#define HWIO_IC_ASYNC_BCK_PRS_DB_NEW_DB_INT_SHFT                                                          0xd
#define HWIO_IC_ASYNC_BCK_PRS_TIMER_ENG_INT_BMSK                                                       0x1000
#define HWIO_IC_ASYNC_BCK_PRS_TIMER_ENG_INT_SHFT                                                          0xc
#define HWIO_IC_ASYNC_BCK_PRS_TLV_CH_NOT_FULL_INT_BMSK                                                  0x800
#define HWIO_IC_ASYNC_BCK_PRS_TLV_CH_NOT_FULL_INT_SHFT                                                    0xb
#define HWIO_IC_ASYNC_BCK_PRS_TLV_RST_INT_BMSK                                                          0x400
#define HWIO_IC_ASYNC_BCK_PRS_TLV_RST_INT_SHFT                                                            0xa
#define HWIO_IC_ASYNC_BCK_PRS_TLV_STOP_INT_BMSK                                                         0x200
#define HWIO_IC_ASYNC_BCK_PRS_TLV_STOP_INT_SHFT                                                           0x9
#define HWIO_IC_ASYNC_BCK_PRS_TLV_DESC_PROC_COMP_INT_BMSK                                               0x100
#define HWIO_IC_ASYNC_BCK_PRS_TLV_DESC_PROC_COMP_INT_SHFT                                                 0x8
#define HWIO_IC_ASYNC_BCK_PRS_CSR_GENERIC_COM_INT_BMSK                                                   0x80
#define HWIO_IC_ASYNC_BCK_PRS_CSR_GENERIC_COM_INT_SHFT                                                    0x7
#define HWIO_IC_ASYNC_BCK_PRS_CSR_CH_EV_COM_INT_BMSK                                                     0x40
#define HWIO_IC_ASYNC_BCK_PRS_CSR_CH_EV_COM_INT_SHFT                                                      0x6
#define HWIO_IC_ASYNC_BCK_PRS_IE_NOTIFY_MCS_INT_BMSK                                                     0x20
#define HWIO_IC_ASYNC_BCK_PRS_IE_NOTIFY_MCS_INT_SHFT                                                      0x5
#define HWIO_IC_ASYNC_BCK_PRS_IE_INT_MOD_STOP_INT_BMSK                                                   0x10
#define HWIO_IC_ASYNC_BCK_PRS_IE_INT_MOD_STOP_INT_SHFT                                                    0x4
#define HWIO_IC_ASYNC_BCK_PRS_EV_ENG_INT_BMSK                                                             0x8
#define HWIO_IC_ASYNC_BCK_PRS_EV_ENG_INT_SHFT                                                             0x3
#define HWIO_IC_ASYNC_BCK_PRS_REE_CH_EMPTY_INT_BMSK                                                       0x4
#define HWIO_IC_ASYNC_BCK_PRS_REE_CH_EMPTY_INT_SHFT                                                       0x2
#define HWIO_IC_ASYNC_BCK_PRS_REE_NEW_RE_INT_BMSK                                                         0x2
#define HWIO_IC_ASYNC_BCK_PRS_REE_NEW_RE_INT_SHFT                                                         0x1
#define HWIO_IC_ASYNC_BCK_PRS_REE_DIS_CH_COMP_INT_BMSK                                                    0x1
#define HWIO_IC_ASYNC_BCK_PRS_REE_DIS_CH_COMP_INT_SHFT                                                    0x0

#define HWIO_IC_INT_WEIGHT_REE_ADDR                                                                (GSI_TOP_GSI_REG_BASE      + 0x00004100)
#define HWIO_IC_INT_WEIGHT_REE_PHYS                                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004100)
#define HWIO_IC_INT_WEIGHT_REE_OFFS                                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004100)
#define HWIO_IC_INT_WEIGHT_REE_RMSK                                                                     0xfff
#define HWIO_IC_INT_WEIGHT_REE_ATTR                                                                       0x3
#define HWIO_IC_INT_WEIGHT_REE_IN          \
        in_dword_masked(HWIO_IC_INT_WEIGHT_REE_ADDR, HWIO_IC_INT_WEIGHT_REE_RMSK)
#define HWIO_IC_INT_WEIGHT_REE_INM(m)      \
        in_dword_masked(HWIO_IC_INT_WEIGHT_REE_ADDR, m)
#define HWIO_IC_INT_WEIGHT_REE_OUT(v)      \
        out_dword(HWIO_IC_INT_WEIGHT_REE_ADDR,v)
#define HWIO_IC_INT_WEIGHT_REE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_IC_INT_WEIGHT_REE_ADDR,m,v,HWIO_IC_INT_WEIGHT_REE_IN)
#define HWIO_IC_INT_WEIGHT_REE_CH_EMPTY_INT_WEIGHT_BMSK                                                 0xf00
#define HWIO_IC_INT_WEIGHT_REE_CH_EMPTY_INT_WEIGHT_SHFT                                                   0x8
#define HWIO_IC_INT_WEIGHT_REE_NEW_RE_INT_WEIGHT_BMSK                                                    0xf0
#define HWIO_IC_INT_WEIGHT_REE_NEW_RE_INT_WEIGHT_SHFT                                                     0x4
#define HWIO_IC_INT_WEIGHT_REE_STOP_CH_COMP_INT_WEIGHT_BMSK                                               0xf
#define HWIO_IC_INT_WEIGHT_REE_STOP_CH_COMP_INT_WEIGHT_SHFT                                               0x0

#define HWIO_IC_INT_WEIGHT_EVT_ENG_ADDR                                                            (GSI_TOP_GSI_REG_BASE      + 0x00004104)
#define HWIO_IC_INT_WEIGHT_EVT_ENG_PHYS                                                            (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004104)
#define HWIO_IC_INT_WEIGHT_EVT_ENG_OFFS                                                            (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004104)
#define HWIO_IC_INT_WEIGHT_EVT_ENG_RMSK                                                                   0xf
#define HWIO_IC_INT_WEIGHT_EVT_ENG_ATTR                                                                   0x3
#define HWIO_IC_INT_WEIGHT_EVT_ENG_IN          \
        in_dword_masked(HWIO_IC_INT_WEIGHT_EVT_ENG_ADDR, HWIO_IC_INT_WEIGHT_EVT_ENG_RMSK)
#define HWIO_IC_INT_WEIGHT_EVT_ENG_INM(m)      \
        in_dword_masked(HWIO_IC_INT_WEIGHT_EVT_ENG_ADDR, m)
#define HWIO_IC_INT_WEIGHT_EVT_ENG_OUT(v)      \
        out_dword(HWIO_IC_INT_WEIGHT_EVT_ENG_ADDR,v)
#define HWIO_IC_INT_WEIGHT_EVT_ENG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_IC_INT_WEIGHT_EVT_ENG_ADDR,m,v,HWIO_IC_INT_WEIGHT_EVT_ENG_IN)
#define HWIO_IC_INT_WEIGHT_EVT_ENG_EVNT_ENG_INT_WEIGHT_BMSK                                               0xf
#define HWIO_IC_INT_WEIGHT_EVT_ENG_EVNT_ENG_INT_WEIGHT_SHFT                                               0x0

#define HWIO_IC_INT_WEIGHT_INT_ENG_ADDR                                                            (GSI_TOP_GSI_REG_BASE      + 0x00004108)
#define HWIO_IC_INT_WEIGHT_INT_ENG_PHYS                                                            (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004108)
#define HWIO_IC_INT_WEIGHT_INT_ENG_OFFS                                                            (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004108)
#define HWIO_IC_INT_WEIGHT_INT_ENG_RMSK                                                                 0xfff
#define HWIO_IC_INT_WEIGHT_INT_ENG_ATTR                                                                   0x3
#define HWIO_IC_INT_WEIGHT_INT_ENG_IN          \
        in_dword_masked(HWIO_IC_INT_WEIGHT_INT_ENG_ADDR, HWIO_IC_INT_WEIGHT_INT_ENG_RMSK)
#define HWIO_IC_INT_WEIGHT_INT_ENG_INM(m)      \
        in_dword_masked(HWIO_IC_INT_WEIGHT_INT_ENG_ADDR, m)
#define HWIO_IC_INT_WEIGHT_INT_ENG_OUT(v)      \
        out_dword(HWIO_IC_INT_WEIGHT_INT_ENG_ADDR,v)
#define HWIO_IC_INT_WEIGHT_INT_ENG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_IC_INT_WEIGHT_INT_ENG_ADDR,m,v,HWIO_IC_INT_WEIGHT_INT_ENG_IN)
#define HWIO_IC_INT_WEIGHT_INT_ENG_INT_MOD_CANCEL_INT_WEIGHT_BMSK                                       0xf00
#define HWIO_IC_INT_WEIGHT_INT_ENG_INT_MOD_CANCEL_INT_WEIGHT_SHFT                                         0x8
#define HWIO_IC_INT_WEIGHT_INT_ENG_NOTIFY_MCS_INT_WEIGHT_BMSK                                            0xf0
#define HWIO_IC_INT_WEIGHT_INT_ENG_NOTIFY_MCS_INT_WEIGHT_SHFT                                             0x4
#define HWIO_IC_INT_WEIGHT_INT_ENG_INT_MOD_STOP_INT_WEIGHT_BMSK                                           0xf
#define HWIO_IC_INT_WEIGHT_INT_ENG_INT_MOD_STOP_INT_WEIGHT_SHFT                                           0x0

#define HWIO_IC_INT_WEIGHT_CSR_ADDR                                                                (GSI_TOP_GSI_REG_BASE      + 0x0000410c)
#define HWIO_IC_INT_WEIGHT_CSR_PHYS                                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000410c)
#define HWIO_IC_INT_WEIGHT_CSR_OFFS                                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000410c)
#define HWIO_IC_INT_WEIGHT_CSR_RMSK                                                                      0xff
#define HWIO_IC_INT_WEIGHT_CSR_ATTR                                                                       0x3
#define HWIO_IC_INT_WEIGHT_CSR_IN          \
        in_dword_masked(HWIO_IC_INT_WEIGHT_CSR_ADDR, HWIO_IC_INT_WEIGHT_CSR_RMSK)
#define HWIO_IC_INT_WEIGHT_CSR_INM(m)      \
        in_dword_masked(HWIO_IC_INT_WEIGHT_CSR_ADDR, m)
#define HWIO_IC_INT_WEIGHT_CSR_OUT(v)      \
        out_dword(HWIO_IC_INT_WEIGHT_CSR_ADDR,v)
#define HWIO_IC_INT_WEIGHT_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_IC_INT_WEIGHT_CSR_ADDR,m,v,HWIO_IC_INT_WEIGHT_CSR_IN)
#define HWIO_IC_INT_WEIGHT_CSR_EE_GENERIC_INT_WEIGHT_BMSK                                                0xf0
#define HWIO_IC_INT_WEIGHT_CSR_EE_GENERIC_INT_WEIGHT_SHFT                                                 0x4
#define HWIO_IC_INT_WEIGHT_CSR_CH_CMD_INT_WEIGHT_BMSK                                                     0xf
#define HWIO_IC_INT_WEIGHT_CSR_CH_CMD_INT_WEIGHT_SHFT                                                     0x0

#define HWIO_IC_INT_WEIGHT_TLV_ENG_ADDR                                                            (GSI_TOP_GSI_REG_BASE      + 0x00004110)
#define HWIO_IC_INT_WEIGHT_TLV_ENG_PHYS                                                            (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004110)
#define HWIO_IC_INT_WEIGHT_TLV_ENG_OFFS                                                            (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004110)
#define HWIO_IC_INT_WEIGHT_TLV_ENG_RMSK                                                                0xffff
#define HWIO_IC_INT_WEIGHT_TLV_ENG_ATTR                                                                   0x3
#define HWIO_IC_INT_WEIGHT_TLV_ENG_IN          \
        in_dword_masked(HWIO_IC_INT_WEIGHT_TLV_ENG_ADDR, HWIO_IC_INT_WEIGHT_TLV_ENG_RMSK)
#define HWIO_IC_INT_WEIGHT_TLV_ENG_INM(m)      \
        in_dword_masked(HWIO_IC_INT_WEIGHT_TLV_ENG_ADDR, m)
#define HWIO_IC_INT_WEIGHT_TLV_ENG_OUT(v)      \
        out_dword(HWIO_IC_INT_WEIGHT_TLV_ENG_ADDR,v)
#define HWIO_IC_INT_WEIGHT_TLV_ENG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_IC_INT_WEIGHT_TLV_ENG_ADDR,m,v,HWIO_IC_INT_WEIGHT_TLV_ENG_IN)
#define HWIO_IC_INT_WEIGHT_TLV_ENG_CH_NOT_FULL_INT_WEIGHT_BMSK                                         0xf000
#define HWIO_IC_INT_WEIGHT_TLV_ENG_CH_NOT_FULL_INT_WEIGHT_SHFT                                            0xc
#define HWIO_IC_INT_WEIGHT_TLV_ENG_TLV_2_INT_WEIGHT_BMSK                                                0xf00
#define HWIO_IC_INT_WEIGHT_TLV_ENG_TLV_2_INT_WEIGHT_SHFT                                                  0x8
#define HWIO_IC_INT_WEIGHT_TLV_ENG_TLV_1_INT_WEIGHT_BMSK                                                 0xf0
#define HWIO_IC_INT_WEIGHT_TLV_ENG_TLV_1_INT_WEIGHT_SHFT                                                  0x4
#define HWIO_IC_INT_WEIGHT_TLV_ENG_TLV_0_INT_WEIGHT_BMSK                                                  0xf
#define HWIO_IC_INT_WEIGHT_TLV_ENG_TLV_0_INT_WEIGHT_SHFT                                                  0x0

#define HWIO_IC_INT_WEIGHT_TIMER_ENG_ADDR                                                          (GSI_TOP_GSI_REG_BASE      + 0x00004114)
#define HWIO_IC_INT_WEIGHT_TIMER_ENG_PHYS                                                          (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004114)
#define HWIO_IC_INT_WEIGHT_TIMER_ENG_OFFS                                                          (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004114)
#define HWIO_IC_INT_WEIGHT_TIMER_ENG_RMSK                                                                 0xf
#define HWIO_IC_INT_WEIGHT_TIMER_ENG_ATTR                                                                 0x3
#define HWIO_IC_INT_WEIGHT_TIMER_ENG_IN          \
        in_dword_masked(HWIO_IC_INT_WEIGHT_TIMER_ENG_ADDR, HWIO_IC_INT_WEIGHT_TIMER_ENG_RMSK)
#define HWIO_IC_INT_WEIGHT_TIMER_ENG_INM(m)      \
        in_dword_masked(HWIO_IC_INT_WEIGHT_TIMER_ENG_ADDR, m)
#define HWIO_IC_INT_WEIGHT_TIMER_ENG_OUT(v)      \
        out_dword(HWIO_IC_INT_WEIGHT_TIMER_ENG_ADDR,v)
#define HWIO_IC_INT_WEIGHT_TIMER_ENG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_IC_INT_WEIGHT_TIMER_ENG_ADDR,m,v,HWIO_IC_INT_WEIGHT_TIMER_ENG_IN)
#define HWIO_IC_INT_WEIGHT_TIMER_ENG_TIMER_INT_WEIGHT_BMSK                                                0xf
#define HWIO_IC_INT_WEIGHT_TIMER_ENG_TIMER_INT_WEIGHT_SHFT                                                0x0

#define HWIO_IC_INT_WEIGHT_DB_ENG_ADDR                                                             (GSI_TOP_GSI_REG_BASE      + 0x00004118)
#define HWIO_IC_INT_WEIGHT_DB_ENG_PHYS                                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004118)
#define HWIO_IC_INT_WEIGHT_DB_ENG_OFFS                                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004118)
#define HWIO_IC_INT_WEIGHT_DB_ENG_RMSK                                                                    0xf
#define HWIO_IC_INT_WEIGHT_DB_ENG_ATTR                                                                    0x3
#define HWIO_IC_INT_WEIGHT_DB_ENG_IN          \
        in_dword_masked(HWIO_IC_INT_WEIGHT_DB_ENG_ADDR, HWIO_IC_INT_WEIGHT_DB_ENG_RMSK)
#define HWIO_IC_INT_WEIGHT_DB_ENG_INM(m)      \
        in_dword_masked(HWIO_IC_INT_WEIGHT_DB_ENG_ADDR, m)
#define HWIO_IC_INT_WEIGHT_DB_ENG_OUT(v)      \
        out_dword(HWIO_IC_INT_WEIGHT_DB_ENG_ADDR,v)
#define HWIO_IC_INT_WEIGHT_DB_ENG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_IC_INT_WEIGHT_DB_ENG_ADDR,m,v,HWIO_IC_INT_WEIGHT_DB_ENG_IN)
#define HWIO_IC_INT_WEIGHT_DB_ENG_NEW_DB_INT_WEIGHT_BMSK                                                  0xf
#define HWIO_IC_INT_WEIGHT_DB_ENG_NEW_DB_INT_WEIGHT_SHFT                                                  0x0

#define HWIO_IC_INT_WEIGHT_RD_ENG_ADDR                                                             (GSI_TOP_GSI_REG_BASE      + 0x0000411c)
#define HWIO_IC_INT_WEIGHT_RD_ENG_PHYS                                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000411c)
#define HWIO_IC_INT_WEIGHT_RD_ENG_OFFS                                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000411c)
#define HWIO_IC_INT_WEIGHT_RD_ENG_RMSK                                                                    0xf
#define HWIO_IC_INT_WEIGHT_RD_ENG_ATTR                                                                    0x3
#define HWIO_IC_INT_WEIGHT_RD_ENG_IN          \
        in_dword_masked(HWIO_IC_INT_WEIGHT_RD_ENG_ADDR, HWIO_IC_INT_WEIGHT_RD_ENG_RMSK)
#define HWIO_IC_INT_WEIGHT_RD_ENG_INM(m)      \
        in_dword_masked(HWIO_IC_INT_WEIGHT_RD_ENG_ADDR, m)
#define HWIO_IC_INT_WEIGHT_RD_ENG_OUT(v)      \
        out_dword(HWIO_IC_INT_WEIGHT_RD_ENG_ADDR,v)
#define HWIO_IC_INT_WEIGHT_RD_ENG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_IC_INT_WEIGHT_RD_ENG_ADDR,m,v,HWIO_IC_INT_WEIGHT_RD_ENG_IN)
#define HWIO_IC_INT_WEIGHT_RD_ENG_READ_INT_WEIGHT_BMSK                                                    0xf
#define HWIO_IC_INT_WEIGHT_RD_ENG_READ_INT_WEIGHT_SHFT                                                    0x0

#define HWIO_IC_INT_WEIGHT_WR_ENG_ADDR                                                             (GSI_TOP_GSI_REG_BASE      + 0x00004120)
#define HWIO_IC_INT_WEIGHT_WR_ENG_PHYS                                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004120)
#define HWIO_IC_INT_WEIGHT_WR_ENG_OFFS                                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004120)
#define HWIO_IC_INT_WEIGHT_WR_ENG_RMSK                                                                    0xf
#define HWIO_IC_INT_WEIGHT_WR_ENG_ATTR                                                                    0x3
#define HWIO_IC_INT_WEIGHT_WR_ENG_IN          \
        in_dword_masked(HWIO_IC_INT_WEIGHT_WR_ENG_ADDR, HWIO_IC_INT_WEIGHT_WR_ENG_RMSK)
#define HWIO_IC_INT_WEIGHT_WR_ENG_INM(m)      \
        in_dword_masked(HWIO_IC_INT_WEIGHT_WR_ENG_ADDR, m)
#define HWIO_IC_INT_WEIGHT_WR_ENG_OUT(v)      \
        out_dword(HWIO_IC_INT_WEIGHT_WR_ENG_ADDR,v)
#define HWIO_IC_INT_WEIGHT_WR_ENG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_IC_INT_WEIGHT_WR_ENG_ADDR,m,v,HWIO_IC_INT_WEIGHT_WR_ENG_IN)
#define HWIO_IC_INT_WEIGHT_WR_ENG_WRITE_INT_WEIGHT_BMSK                                                   0xf
#define HWIO_IC_INT_WEIGHT_WR_ENG_WRITE_INT_WEIGHT_SHFT                                                   0x0

#define HWIO_LOW_LATENCY_ARB_WEIGHT_ADDR                                                           (GSI_TOP_GSI_REG_BASE      + 0x00004128)
#define HWIO_LOW_LATENCY_ARB_WEIGHT_PHYS                                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004128)
#define HWIO_LOW_LATENCY_ARB_WEIGHT_OFFS                                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004128)
#define HWIO_LOW_LATENCY_ARB_WEIGHT_RMSK                                                              0x13f3f
#define HWIO_LOW_LATENCY_ARB_WEIGHT_ATTR                                                                  0x3
#define HWIO_LOW_LATENCY_ARB_WEIGHT_IN          \
        in_dword_masked(HWIO_LOW_LATENCY_ARB_WEIGHT_ADDR, HWIO_LOW_LATENCY_ARB_WEIGHT_RMSK)
#define HWIO_LOW_LATENCY_ARB_WEIGHT_INM(m)      \
        in_dword_masked(HWIO_LOW_LATENCY_ARB_WEIGHT_ADDR, m)
#define HWIO_LOW_LATENCY_ARB_WEIGHT_OUT(v)      \
        out_dword(HWIO_LOW_LATENCY_ARB_WEIGHT_ADDR,v)
#define HWIO_LOW_LATENCY_ARB_WEIGHT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LOW_LATENCY_ARB_WEIGHT_ADDR,m,v,HWIO_LOW_LATENCY_ARB_WEIGHT_IN)
#define HWIO_LOW_LATENCY_ARB_WEIGHT_LL_NON_LL_FIX_PRIORITY_BMSK                                       0x10000
#define HWIO_LOW_LATENCY_ARB_WEIGHT_LL_NON_LL_FIX_PRIORITY_SHFT                                          0x10
#define HWIO_LOW_LATENCY_ARB_WEIGHT_NON_LL_WEIGHT_BMSK                                                 0x3f00
#define HWIO_LOW_LATENCY_ARB_WEIGHT_NON_LL_WEIGHT_SHFT                                                    0x8
#define HWIO_LOW_LATENCY_ARB_WEIGHT_LL_WEIGHT_BMSK                                                       0x3f
#define HWIO_LOW_LATENCY_ARB_WEIGHT_LL_WEIGHT_SHFT                                                        0x0

#define HWIO_IC_INT_WEIGHT_SDMA_ADDR                                                               (GSI_TOP_GSI_REG_BASE      + 0x00004124)
#define HWIO_IC_INT_WEIGHT_SDMA_PHYS                                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004124)
#define HWIO_IC_INT_WEIGHT_SDMA_OFFS                                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004124)
#define HWIO_IC_INT_WEIGHT_SDMA_RMSK                                                                   0xffff
#define HWIO_IC_INT_WEIGHT_SDMA_ATTR                                                                      0x1
#define HWIO_IC_INT_WEIGHT_SDMA_IN          \
        in_dword_masked(HWIO_IC_INT_WEIGHT_SDMA_ADDR, HWIO_IC_INT_WEIGHT_SDMA_RMSK)
#define HWIO_IC_INT_WEIGHT_SDMA_INM(m)      \
        in_dword_masked(HWIO_IC_INT_WEIGHT_SDMA_ADDR, m)
#define HWIO_IC_INT_WEIGHT_SDMA_SDMA_INT_3_WEIGHT_BMSK                                                 0xf000
#define HWIO_IC_INT_WEIGHT_SDMA_SDMA_INT_3_WEIGHT_SHFT                                                    0xc
#define HWIO_IC_INT_WEIGHT_SDMA_SDMA_INT_2_WEIGHT_BMSK                                                  0xf00
#define HWIO_IC_INT_WEIGHT_SDMA_SDMA_INT_2_WEIGHT_SHFT                                                    0x8
#define HWIO_IC_INT_WEIGHT_SDMA_SDMA_INT_1_WEIGHT_BMSK                                                   0xf0
#define HWIO_IC_INT_WEIGHT_SDMA_SDMA_INT_1_WEIGHT_SHFT                                                    0x4
#define HWIO_IC_INT_WEIGHT_SDMA_SDMA_INT_0_WEIGHT_BMSK                                                    0xf
#define HWIO_IC_INT_WEIGHT_SDMA_SDMA_INT_0_WEIGHT_SHFT                                                    0x0

#define HWIO_GSI_SDMA_CFG_ADDR                                                                     (GSI_TOP_GSI_REG_BASE      + 0x0000403c)
#define HWIO_GSI_SDMA_CFG_PHYS                                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000403c)
#define HWIO_GSI_SDMA_CFG_OFFS                                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000403c)
#define HWIO_GSI_SDMA_CFG_RMSK                                                                       0xffffff
#define HWIO_GSI_SDMA_CFG_ATTR                                                                            0x1
#define HWIO_GSI_SDMA_CFG_IN          \
        in_dword_masked(HWIO_GSI_SDMA_CFG_ADDR, HWIO_GSI_SDMA_CFG_RMSK)
#define HWIO_GSI_SDMA_CFG_INM(m)      \
        in_dword_masked(HWIO_GSI_SDMA_CFG_ADDR, m)
#define HWIO_GSI_SDMA_CFG_MAX_BURST_READ_BMSK                                                        0xff0000
#define HWIO_GSI_SDMA_CFG_MAX_BURST_READ_SHFT                                                            0x10
#define HWIO_GSI_SDMA_CFG_MAX_BURST_WRITE_BMSK                                                         0xff00
#define HWIO_GSI_SDMA_CFG_MAX_BURST_WRITE_SHFT                                                            0x8
#define HWIO_GSI_SDMA_CFG_MAX_OS_READ_BMSK                                                               0xf0
#define HWIO_GSI_SDMA_CFG_MAX_OS_READ_SHFT                                                                0x4
#define HWIO_GSI_SDMA_CFG_MAX_OS_WRITE_BMSK                                                               0xf
#define HWIO_GSI_SDMA_CFG_MAX_OS_WRITE_SHFT                                                               0x0

#define HWIO_GSI_SDMA_CACHEATTR_ADDR                                                               (GSI_TOP_GSI_REG_BASE      + 0x00004094)
#define HWIO_GSI_SDMA_CACHEATTR_PHYS                                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004094)
#define HWIO_GSI_SDMA_CACHEATTR_OFFS                                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004094)
#define HWIO_GSI_SDMA_CACHEATTR_RMSK                                                                     0x3f
#define HWIO_GSI_SDMA_CACHEATTR_ATTR                                                                      0x1
#define HWIO_GSI_SDMA_CACHEATTR_IN          \
        in_dword_masked(HWIO_GSI_SDMA_CACHEATTR_ADDR, HWIO_GSI_SDMA_CACHEATTR_RMSK)
#define HWIO_GSI_SDMA_CACHEATTR_INM(m)      \
        in_dword_masked(HWIO_GSI_SDMA_CACHEATTR_ADDR, m)
#define HWIO_GSI_SDMA_CACHEATTR_AREQPRIORITY_BMSK                                                        0x30
#define HWIO_GSI_SDMA_CACHEATTR_AREQPRIORITY_SHFT                                                         0x4
#define HWIO_GSI_SDMA_CACHEATTR_ATRANSIENT_BMSK                                                           0x8
#define HWIO_GSI_SDMA_CACHEATTR_ATRANSIENT_SHFT                                                           0x3
#define HWIO_GSI_SDMA_CACHEATTR_ANOALLOCATE_BMSK                                                          0x4
#define HWIO_GSI_SDMA_CACHEATTR_ANOALLOCATE_SHFT                                                          0x2
#define HWIO_GSI_SDMA_CACHEATTR_AINNERSHARED_BMSK                                                         0x2
#define HWIO_GSI_SDMA_CACHEATTR_AINNERSHARED_SHFT                                                         0x1
#define HWIO_GSI_SDMA_CACHEATTR_ASHARED_BMSK                                                              0x1
#define HWIO_GSI_SDMA_CACHEATTR_ASHARED_SHFT                                                              0x0

#define HWIO_GSI_SDMA_SG_IOVEC_LSB_n_ADDR(n)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00004140 + 0x8 * (n))
#define HWIO_GSI_SDMA_SG_IOVEC_LSB_n_PHYS(n)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004140 + 0x8 * (n))
#define HWIO_GSI_SDMA_SG_IOVEC_LSB_n_OFFS(n)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004140 + 0x8 * (n))
#define HWIO_GSI_SDMA_SG_IOVEC_LSB_n_RMSK                                                          0xffffffff
#define HWIO_GSI_SDMA_SG_IOVEC_LSB_n_MAXn                                                                   3
#define HWIO_GSI_SDMA_SG_IOVEC_LSB_n_ATTR                                                                 0x1
#define HWIO_GSI_SDMA_SG_IOVEC_LSB_n_INI(n)        \
        in_dword_masked(HWIO_GSI_SDMA_SG_IOVEC_LSB_n_ADDR(n), HWIO_GSI_SDMA_SG_IOVEC_LSB_n_RMSK)
#define HWIO_GSI_SDMA_SG_IOVEC_LSB_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_SDMA_SG_IOVEC_LSB_n_ADDR(n), mask)
#define HWIO_GSI_SDMA_SG_IOVEC_LSB_n_SDMA_SG_IOVEC_LSB_BMSK                                        0xffffffff
#define HWIO_GSI_SDMA_SG_IOVEC_LSB_n_SDMA_SG_IOVEC_LSB_SHFT                                               0x0

#define HWIO_GSI_SDMA_SG_IOVEC_MSB_n_ADDR(n)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00004144 + 0x8 * (n))
#define HWIO_GSI_SDMA_SG_IOVEC_MSB_n_PHYS(n)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004144 + 0x8 * (n))
#define HWIO_GSI_SDMA_SG_IOVEC_MSB_n_OFFS(n)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004144 + 0x8 * (n))
#define HWIO_GSI_SDMA_SG_IOVEC_MSB_n_RMSK                                                          0xffffffff
#define HWIO_GSI_SDMA_SG_IOVEC_MSB_n_MAXn                                                                   3
#define HWIO_GSI_SDMA_SG_IOVEC_MSB_n_ATTR                                                                 0x1
#define HWIO_GSI_SDMA_SG_IOVEC_MSB_n_INI(n)        \
        in_dword_masked(HWIO_GSI_SDMA_SG_IOVEC_MSB_n_ADDR(n), HWIO_GSI_SDMA_SG_IOVEC_MSB_n_RMSK)
#define HWIO_GSI_SDMA_SG_IOVEC_MSB_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_SDMA_SG_IOVEC_MSB_n_ADDR(n), mask)
#define HWIO_GSI_SDMA_SG_IOVEC_MSB_n_SDMA_SG_IOVEC_MSB_BMSK                                        0xffffffff
#define HWIO_GSI_SDMA_SG_IOVEC_MSB_n_SDMA_SG_IOVEC_MSB_SHFT                                               0x0

#define HWIO_GSI_MANAGER_EE_QOS_n_ADDR(n)                                                          (GSI_TOP_GSI_REG_BASE      + 0x00004300 + 0x4 * (n))
#define HWIO_GSI_MANAGER_EE_QOS_n_PHYS(n)                                                          (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004300 + 0x4 * (n))
#define HWIO_GSI_MANAGER_EE_QOS_n_OFFS(n)                                                          (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004300 + 0x4 * (n))
#define HWIO_GSI_MANAGER_EE_QOS_n_RMSK                                                               0xffff03
#define HWIO_GSI_MANAGER_EE_QOS_n_MAXn                                                                      4
#define HWIO_GSI_MANAGER_EE_QOS_n_ATTR                                                                    0x3
#define HWIO_GSI_MANAGER_EE_QOS_n_INI(n)        \
        in_dword_masked(HWIO_GSI_MANAGER_EE_QOS_n_ADDR(n), HWIO_GSI_MANAGER_EE_QOS_n_RMSK)
#define HWIO_GSI_MANAGER_EE_QOS_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_MANAGER_EE_QOS_n_ADDR(n), mask)
#define HWIO_GSI_MANAGER_EE_QOS_n_OUTI(n,val)    \
        out_dword(HWIO_GSI_MANAGER_EE_QOS_n_ADDR(n),val)
#define HWIO_GSI_MANAGER_EE_QOS_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_GSI_MANAGER_EE_QOS_n_ADDR(n),mask,val,HWIO_GSI_MANAGER_EE_QOS_n_INI(n))
#define HWIO_GSI_MANAGER_EE_QOS_n_MAX_EV_ALLOC_BMSK                                                  0xff0000
#define HWIO_GSI_MANAGER_EE_QOS_n_MAX_EV_ALLOC_SHFT                                                      0x10
#define HWIO_GSI_MANAGER_EE_QOS_n_MAX_CH_ALLOC_BMSK                                                    0xff00
#define HWIO_GSI_MANAGER_EE_QOS_n_MAX_CH_ALLOC_SHFT                                                       0x8
#define HWIO_GSI_MANAGER_EE_QOS_n_EE_PRIO_BMSK                                                            0x3
#define HWIO_GSI_MANAGER_EE_QOS_n_EE_PRIO_SHFT                                                            0x0

#define HWIO_GSI_SHRAM_PTR_CH_CNTXT_BASE_ADDR_ADDR                                                 (GSI_TOP_GSI_REG_BASE      + 0x00004200)
#define HWIO_GSI_SHRAM_PTR_CH_CNTXT_BASE_ADDR_PHYS                                                 (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004200)
#define HWIO_GSI_SHRAM_PTR_CH_CNTXT_BASE_ADDR_OFFS                                                 (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004200)
#define HWIO_GSI_SHRAM_PTR_CH_CNTXT_BASE_ADDR_RMSK                                                     0xffff
#define HWIO_GSI_SHRAM_PTR_CH_CNTXT_BASE_ADDR_ATTR                                                        0x1
#define HWIO_GSI_SHRAM_PTR_CH_CNTXT_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_CH_CNTXT_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_CH_CNTXT_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_CH_CNTXT_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_CH_CNTXT_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_CH_CNTXT_BASE_ADDR_SHRAM_PTR_BMSK                                           0xffff
#define HWIO_GSI_SHRAM_PTR_CH_CNTXT_BASE_ADDR_SHRAM_PTR_SHFT                                              0x0

#define HWIO_GSI_SHRAM_PTR_EV_CNTXT_BASE_ADDR_ADDR                                                 (GSI_TOP_GSI_REG_BASE      + 0x00004204)
#define HWIO_GSI_SHRAM_PTR_EV_CNTXT_BASE_ADDR_PHYS                                                 (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004204)
#define HWIO_GSI_SHRAM_PTR_EV_CNTXT_BASE_ADDR_OFFS                                                 (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004204)
#define HWIO_GSI_SHRAM_PTR_EV_CNTXT_BASE_ADDR_RMSK                                                     0xffff
#define HWIO_GSI_SHRAM_PTR_EV_CNTXT_BASE_ADDR_ATTR                                                        0x1
#define HWIO_GSI_SHRAM_PTR_EV_CNTXT_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_EV_CNTXT_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_EV_CNTXT_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_EV_CNTXT_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_EV_CNTXT_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_EV_CNTXT_BASE_ADDR_SHRAM_PTR_BMSK                                           0xffff
#define HWIO_GSI_SHRAM_PTR_EV_CNTXT_BASE_ADDR_SHRAM_PTR_SHFT                                              0x0

#define HWIO_GSI_SHRAM_PTR_RE_STORAGE_BASE_ADDR_ADDR                                               (GSI_TOP_GSI_REG_BASE      + 0x00004208)
#define HWIO_GSI_SHRAM_PTR_RE_STORAGE_BASE_ADDR_PHYS                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004208)
#define HWIO_GSI_SHRAM_PTR_RE_STORAGE_BASE_ADDR_OFFS                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004208)
#define HWIO_GSI_SHRAM_PTR_RE_STORAGE_BASE_ADDR_RMSK                                                   0xffff
#define HWIO_GSI_SHRAM_PTR_RE_STORAGE_BASE_ADDR_ATTR                                                      0x1
#define HWIO_GSI_SHRAM_PTR_RE_STORAGE_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_RE_STORAGE_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_RE_STORAGE_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_RE_STORAGE_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_RE_STORAGE_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_RE_STORAGE_BASE_ADDR_SHRAM_PTR_BMSK                                         0xffff
#define HWIO_GSI_SHRAM_PTR_RE_STORAGE_BASE_ADDR_SHRAM_PTR_SHFT                                            0x0

#define HWIO_GSI_SHRAM_PTR_RE_ESC_BUF_BASE_ADDR_ADDR                                               (GSI_TOP_GSI_REG_BASE      + 0x0000420c)
#define HWIO_GSI_SHRAM_PTR_RE_ESC_BUF_BASE_ADDR_PHYS                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000420c)
#define HWIO_GSI_SHRAM_PTR_RE_ESC_BUF_BASE_ADDR_OFFS                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000420c)
#define HWIO_GSI_SHRAM_PTR_RE_ESC_BUF_BASE_ADDR_RMSK                                                   0xffff
#define HWIO_GSI_SHRAM_PTR_RE_ESC_BUF_BASE_ADDR_ATTR                                                      0x1
#define HWIO_GSI_SHRAM_PTR_RE_ESC_BUF_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_RE_ESC_BUF_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_RE_ESC_BUF_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_RE_ESC_BUF_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_RE_ESC_BUF_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_RE_ESC_BUF_BASE_ADDR_SHRAM_PTR_BMSK                                         0xffff
#define HWIO_GSI_SHRAM_PTR_RE_ESC_BUF_BASE_ADDR_SHRAM_PTR_SHFT                                            0x0

#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_ADDR                                              (GSI_TOP_GSI_REG_BASE      + 0x00004210)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_PHYS                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004210)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_OFFS                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004210)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_RMSK                                                  0xffff
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_ATTR                                                     0x3
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_OUT(v)      \
        out_dword(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_ADDR,v)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_ADDR,m,v,HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_IN)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_SHRAM_PTR_BMSK                                        0xffff
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH_BASE_ADDR_SHRAM_PTR_SHFT                                           0x0

#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_ADDR                                             (GSI_TOP_GSI_REG_BASE      + 0x00004214)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_PHYS                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004214)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_OFFS                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004214)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_RMSK                                                 0xffff
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_ATTR                                                    0x3
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_OUT(v)      \
        out_dword(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_ADDR,v)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_ADDR,m,v,HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_IN)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_SHRAM_PTR_BMSK                                       0xffff
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH1_BASE_ADDR_SHRAM_PTR_SHFT                                          0x0

#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_ADDR                                             (GSI_TOP_GSI_REG_BASE      + 0x00004218)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_PHYS                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004218)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_OFFS                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004218)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_RMSK                                                 0xffff
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_ATTR                                                    0x3
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_OUT(v)      \
        out_dword(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_ADDR,v)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_ADDR,m,v,HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_IN)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_SHRAM_PTR_BMSK                                       0xffff
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH2_BASE_ADDR_SHRAM_PTR_SHFT                                          0x0

#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_ADDR                                             (GSI_TOP_GSI_REG_BASE      + 0x0000421c)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_PHYS                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000421c)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_OFFS                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000421c)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_RMSK                                                 0xffff
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_ATTR                                                    0x3
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_OUT(v)      \
        out_dword(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_ADDR,v)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_ADDR,m,v,HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_IN)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_SHRAM_PTR_BMSK                                       0xffff
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH3_BASE_ADDR_SHRAM_PTR_SHFT                                          0x0

#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_ADDR                                             (GSI_TOP_GSI_REG_BASE      + 0x00004220)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_PHYS                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004220)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_OFFS                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004220)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_RMSK                                                 0xffff
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_ATTR                                                    0x3
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_OUT(v)      \
        out_dword(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_ADDR,v)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_ADDR,m,v,HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_IN)
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_SHRAM_PTR_BMSK                                       0xffff
#define HWIO_GSI_SHRAM_PTR_MCS_SCRATCH4_BASE_ADDR_SHRAM_PTR_SHFT                                          0x0

#define HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_ADDR                                               (GSI_TOP_GSI_REG_BASE      + 0x00004240)
#define HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_PHYS                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004240)
#define HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_OFFS                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004240)
#define HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_RMSK                                                   0xffff
#define HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_ATTR                                                      0x3
#define HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_OUT(v)      \
        out_dword(HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_ADDR,v)
#define HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_ADDR,m,v,HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_IN)
#define HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_SHRAM_PTR_BMSK                                         0xffff
#define HWIO_GSI_SHRAM_PTR_EE_SCRATCH_BASE_ADDR_SHRAM_PTR_SHFT                                            0x0

#define HWIO_GSI_SHRAM_PTR_WR_ENG_REORDER_BASE_ADDR_ADDR                                           (GSI_TOP_GSI_REG_BASE      + 0x00004244)
#define HWIO_GSI_SHRAM_PTR_WR_ENG_REORDER_BASE_ADDR_PHYS                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004244)
#define HWIO_GSI_SHRAM_PTR_WR_ENG_REORDER_BASE_ADDR_OFFS                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004244)
#define HWIO_GSI_SHRAM_PTR_WR_ENG_REORDER_BASE_ADDR_RMSK                                               0xffff
#define HWIO_GSI_SHRAM_PTR_WR_ENG_REORDER_BASE_ADDR_ATTR                                                  0x1
#define HWIO_GSI_SHRAM_PTR_WR_ENG_REORDER_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_WR_ENG_REORDER_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_WR_ENG_REORDER_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_WR_ENG_REORDER_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_WR_ENG_REORDER_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_WR_ENG_REORDER_BASE_ADDR_SHRAM_PTR_BMSK                                     0xffff
#define HWIO_GSI_SHRAM_PTR_WR_ENG_REORDER_BASE_ADDR_SHRAM_PTR_SHFT                                        0x0

#define HWIO_GSI_SHRAM_PTR_COMMON_COMP_Q_BASE_ADDR_ADDR                                            (GSI_TOP_GSI_REG_BASE      + 0x00004248)
#define HWIO_GSI_SHRAM_PTR_COMMON_COMP_Q_BASE_ADDR_PHYS                                            (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004248)
#define HWIO_GSI_SHRAM_PTR_COMMON_COMP_Q_BASE_ADDR_OFFS                                            (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004248)
#define HWIO_GSI_SHRAM_PTR_COMMON_COMP_Q_BASE_ADDR_RMSK                                                0xffff
#define HWIO_GSI_SHRAM_PTR_COMMON_COMP_Q_BASE_ADDR_ATTR                                                   0x1
#define HWIO_GSI_SHRAM_PTR_COMMON_COMP_Q_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_COMMON_COMP_Q_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_COMMON_COMP_Q_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_COMMON_COMP_Q_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_COMMON_COMP_Q_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_COMMON_COMP_Q_BASE_ADDR_SHRAM_PTR_BMSK                                      0xffff
#define HWIO_GSI_SHRAM_PTR_COMMON_COMP_Q_BASE_ADDR_SHRAM_PTR_SHFT                                         0x0

#define HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_ADDR                                                (GSI_TOP_GSI_REG_BASE      + 0x00004250)
#define HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_PHYS                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004250)
#define HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_OFFS                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004250)
#define HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_RMSK                                                    0xffff
#define HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_ATTR                                                       0x3
#define HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_OUT(v)      \
        out_dword(HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_ADDR,v)
#define HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_ADDR,m,v,HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_IN)
#define HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_SHRAM_PTR_BMSK                                          0xffff
#define HWIO_GSI_SHRAM_PTR_MCS_STATS_BASE_ADDR_SHRAM_PTR_SHFT                                             0x0

#define HWIO_GSI_SHRAM_PTR_USER_INFO_DATA_BASE_ADDR_ADDR                                           (GSI_TOP_GSI_REG_BASE      + 0x0000425c)
#define HWIO_GSI_SHRAM_PTR_USER_INFO_DATA_BASE_ADDR_PHYS                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000425c)
#define HWIO_GSI_SHRAM_PTR_USER_INFO_DATA_BASE_ADDR_OFFS                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000425c)
#define HWIO_GSI_SHRAM_PTR_USER_INFO_DATA_BASE_ADDR_RMSK                                               0xffff
#define HWIO_GSI_SHRAM_PTR_USER_INFO_DATA_BASE_ADDR_ATTR                                                  0x1
#define HWIO_GSI_SHRAM_PTR_USER_INFO_DATA_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_USER_INFO_DATA_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_USER_INFO_DATA_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_USER_INFO_DATA_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_USER_INFO_DATA_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_USER_INFO_DATA_BASE_ADDR_SHRAM_PTR_BMSK                                     0xffff
#define HWIO_GSI_SHRAM_PTR_USER_INFO_DATA_BASE_ADDR_SHRAM_PTR_SHFT                                        0x0

#define HWIO_GSI_SHRAM_PTR_EE_CMD_FIFO_BASE_ADDR_ADDR                                              (GSI_TOP_GSI_REG_BASE      + 0x00004260)
#define HWIO_GSI_SHRAM_PTR_EE_CMD_FIFO_BASE_ADDR_PHYS                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004260)
#define HWIO_GSI_SHRAM_PTR_EE_CMD_FIFO_BASE_ADDR_OFFS                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004260)
#define HWIO_GSI_SHRAM_PTR_EE_CMD_FIFO_BASE_ADDR_RMSK                                                  0xffff
#define HWIO_GSI_SHRAM_PTR_EE_CMD_FIFO_BASE_ADDR_ATTR                                                     0x1
#define HWIO_GSI_SHRAM_PTR_EE_CMD_FIFO_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_EE_CMD_FIFO_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_EE_CMD_FIFO_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_EE_CMD_FIFO_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_EE_CMD_FIFO_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_EE_CMD_FIFO_BASE_ADDR_SHRAM_PTR_BMSK                                        0xffff
#define HWIO_GSI_SHRAM_PTR_EE_CMD_FIFO_BASE_ADDR_SHRAM_PTR_SHFT                                           0x0

#define HWIO_GSI_SHRAM_PTR_CH_CMD_FIFO_BASE_ADDR_ADDR                                              (GSI_TOP_GSI_REG_BASE      + 0x00004264)
#define HWIO_GSI_SHRAM_PTR_CH_CMD_FIFO_BASE_ADDR_PHYS                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004264)
#define HWIO_GSI_SHRAM_PTR_CH_CMD_FIFO_BASE_ADDR_OFFS                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004264)
#define HWIO_GSI_SHRAM_PTR_CH_CMD_FIFO_BASE_ADDR_RMSK                                                  0xffff
#define HWIO_GSI_SHRAM_PTR_CH_CMD_FIFO_BASE_ADDR_ATTR                                                     0x1
#define HWIO_GSI_SHRAM_PTR_CH_CMD_FIFO_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_CH_CMD_FIFO_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_CH_CMD_FIFO_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_CH_CMD_FIFO_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_CH_CMD_FIFO_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_CH_CMD_FIFO_BASE_ADDR_SHRAM_PTR_BMSK                                        0xffff
#define HWIO_GSI_SHRAM_PTR_CH_CMD_FIFO_BASE_ADDR_SHRAM_PTR_SHFT                                           0x0

#define HWIO_GSI_SHRAM_PTR_EVE_ED_STORAGE_BASE_ADDR_ADDR                                           (GSI_TOP_GSI_REG_BASE      + 0x00004268)
#define HWIO_GSI_SHRAM_PTR_EVE_ED_STORAGE_BASE_ADDR_PHYS                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004268)
#define HWIO_GSI_SHRAM_PTR_EVE_ED_STORAGE_BASE_ADDR_OFFS                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004268)
#define HWIO_GSI_SHRAM_PTR_EVE_ED_STORAGE_BASE_ADDR_RMSK                                               0xffff
#define HWIO_GSI_SHRAM_PTR_EVE_ED_STORAGE_BASE_ADDR_ATTR                                                  0x1
#define HWIO_GSI_SHRAM_PTR_EVE_ED_STORAGE_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_EVE_ED_STORAGE_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_EVE_ED_STORAGE_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_EVE_ED_STORAGE_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_EVE_ED_STORAGE_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_EVE_ED_STORAGE_BASE_ADDR_SHRAM_PTR_BMSK                                     0xffff
#define HWIO_GSI_SHRAM_PTR_EVE_ED_STORAGE_BASE_ADDR_SHRAM_PTR_SHFT                                        0x0

#define HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_ADDR(n)                                          (GSI_TOP_GSI_REG_BASE      + 0x0000426c + 0x4 * (n))
#define HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_PHYS(n)                                          (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000426c + 0x4 * (n))
#define HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_OFFS(n)                                          (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000426c + 0x4 * (n))
#define HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_RMSK                                                 0xffff
#define HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_MAXn                                                      2
#define HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_ATTR                                                    0x3
#define HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_INI(n)        \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_ADDR(n), HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_RMSK)
#define HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_ADDR(n), mask)
#define HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_OUTI(n,val)    \
        out_dword(HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_ADDR(n),val)
#define HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_ADDR(n),mask,val,HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_INI(n))
#define HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_SHRAM_PTR_BMSK                                       0xffff
#define HWIO_GSI_SHRAM_PTR_FUNC_STACK_BASE_ADDR_n_SHRAM_PTR_SHFT                                          0x0

#define HWIO_GSI_SHRAM_PTR_TIMER_ENG_USER_DATA_BASE_ADDR_ADDR                                      (GSI_TOP_GSI_REG_BASE      + 0x0000428c)
#define HWIO_GSI_SHRAM_PTR_TIMER_ENG_USER_DATA_BASE_ADDR_PHYS                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000428c)
#define HWIO_GSI_SHRAM_PTR_TIMER_ENG_USER_DATA_BASE_ADDR_OFFS                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000428c)
#define HWIO_GSI_SHRAM_PTR_TIMER_ENG_USER_DATA_BASE_ADDR_RMSK                                          0xffff
#define HWIO_GSI_SHRAM_PTR_TIMER_ENG_USER_DATA_BASE_ADDR_ATTR                                             0x1
#define HWIO_GSI_SHRAM_PTR_TIMER_ENG_USER_DATA_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_TIMER_ENG_USER_DATA_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_TIMER_ENG_USER_DATA_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_TIMER_ENG_USER_DATA_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_TIMER_ENG_USER_DATA_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_TIMER_ENG_USER_DATA_BASE_ADDR_SHRAM_PTR_BMSK                                0xffff
#define HWIO_GSI_SHRAM_PTR_TIMER_ENG_USER_DATA_BASE_ADDR_SHRAM_PTR_SHFT                                   0x0

#define HWIO_GSI_SHRAM_PTR_INT_ENG_REORDER_BASE_ADDR_ADDR                                          (GSI_TOP_GSI_REG_BASE      + 0x00004290)
#define HWIO_GSI_SHRAM_PTR_INT_ENG_REORDER_BASE_ADDR_PHYS                                          (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004290)
#define HWIO_GSI_SHRAM_PTR_INT_ENG_REORDER_BASE_ADDR_OFFS                                          (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004290)
#define HWIO_GSI_SHRAM_PTR_INT_ENG_REORDER_BASE_ADDR_RMSK                                              0xffff
#define HWIO_GSI_SHRAM_PTR_INT_ENG_REORDER_BASE_ADDR_ATTR                                                 0x1
#define HWIO_GSI_SHRAM_PTR_INT_ENG_REORDER_BASE_ADDR_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_INT_ENG_REORDER_BASE_ADDR_ADDR, HWIO_GSI_SHRAM_PTR_INT_ENG_REORDER_BASE_ADDR_RMSK)
#define HWIO_GSI_SHRAM_PTR_INT_ENG_REORDER_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_PTR_INT_ENG_REORDER_BASE_ADDR_ADDR, m)
#define HWIO_GSI_SHRAM_PTR_INT_ENG_REORDER_BASE_ADDR_SHRAM_PTR_BMSK                                    0xffff
#define HWIO_GSI_SHRAM_PTR_INT_ENG_REORDER_BASE_ADDR_SHRAM_PTR_SHFT                                       0x0

#define HWIO_GSI_IRAM_PTR_CH_CMD_ADDR                                                              (GSI_TOP_GSI_REG_BASE      + 0x00004400)
#define HWIO_GSI_IRAM_PTR_CH_CMD_PHYS                                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004400)
#define HWIO_GSI_IRAM_PTR_CH_CMD_OFFS                                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004400)
#define HWIO_GSI_IRAM_PTR_CH_CMD_RMSK                                                                  0x1fff
#define HWIO_GSI_IRAM_PTR_CH_CMD_ATTR                                                                     0x3
#define HWIO_GSI_IRAM_PTR_CH_CMD_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_CH_CMD_ADDR, HWIO_GSI_IRAM_PTR_CH_CMD_RMSK)
#define HWIO_GSI_IRAM_PTR_CH_CMD_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_CH_CMD_ADDR, m)
#define HWIO_GSI_IRAM_PTR_CH_CMD_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_CH_CMD_ADDR,v)
#define HWIO_GSI_IRAM_PTR_CH_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_CH_CMD_ADDR,m,v,HWIO_GSI_IRAM_PTR_CH_CMD_IN)
#define HWIO_GSI_IRAM_PTR_CH_CMD_IRAM_PTR_BMSK                                                         0x1fff
#define HWIO_GSI_IRAM_PTR_CH_CMD_IRAM_PTR_SHFT                                                            0x0

#define HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_ADDR                                                      (GSI_TOP_GSI_REG_BASE      + 0x00004404)
#define HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_PHYS                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004404)
#define HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_OFFS                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004404)
#define HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_RMSK                                                          0x1fff
#define HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_ATTR                                                             0x3
#define HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_ADDR, HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_RMSK)
#define HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_ADDR, m)
#define HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_ADDR,v)
#define HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_ADDR,m,v,HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_IN)
#define HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_IRAM_PTR_BMSK                                                 0x1fff
#define HWIO_GSI_IRAM_PTR_EE_GENERIC_CMD_IRAM_PTR_SHFT                                                    0x0

#define HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_ADDR                                                     (GSI_TOP_GSI_REG_BASE      + 0x00004408)
#define HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_PHYS                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004408)
#define HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_OFFS                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004408)
#define HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_RMSK                                                         0x1fff
#define HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_ATTR                                                            0x3
#define HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_ADDR, HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_RMSK)
#define HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_ADDR, m)
#define HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_ADDR,v)
#define HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_ADDR,m,v,HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_IN)
#define HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_IRAM_PTR_BMSK                                                0x1fff
#define HWIO_GSI_IRAM_PTR_TLV_CH_NOT_FULL_IRAM_PTR_SHFT                                                   0x0

#define HWIO_GSI_IRAM_PTR_MSI_DB_ADDR                                                              (GSI_TOP_GSI_REG_BASE      + 0x00004414)
#define HWIO_GSI_IRAM_PTR_MSI_DB_PHYS                                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004414)
#define HWIO_GSI_IRAM_PTR_MSI_DB_OFFS                                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004414)
#define HWIO_GSI_IRAM_PTR_MSI_DB_RMSK                                                                  0x1fff
#define HWIO_GSI_IRAM_PTR_MSI_DB_ATTR                                                                     0x3
#define HWIO_GSI_IRAM_PTR_MSI_DB_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_MSI_DB_ADDR, HWIO_GSI_IRAM_PTR_MSI_DB_RMSK)
#define HWIO_GSI_IRAM_PTR_MSI_DB_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_MSI_DB_ADDR, m)
#define HWIO_GSI_IRAM_PTR_MSI_DB_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_MSI_DB_ADDR,v)
#define HWIO_GSI_IRAM_PTR_MSI_DB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_MSI_DB_ADDR,m,v,HWIO_GSI_IRAM_PTR_MSI_DB_IN)
#define HWIO_GSI_IRAM_PTR_MSI_DB_IRAM_PTR_BMSK                                                         0x1fff
#define HWIO_GSI_IRAM_PTR_MSI_DB_IRAM_PTR_SHFT                                                            0x0

#define HWIO_GSI_IRAM_PTR_CH_DB_ADDR                                                               (GSI_TOP_GSI_REG_BASE      + 0x00004418)
#define HWIO_GSI_IRAM_PTR_CH_DB_PHYS                                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004418)
#define HWIO_GSI_IRAM_PTR_CH_DB_OFFS                                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004418)
#define HWIO_GSI_IRAM_PTR_CH_DB_RMSK                                                                   0x1fff
#define HWIO_GSI_IRAM_PTR_CH_DB_ATTR                                                                      0x3
#define HWIO_GSI_IRAM_PTR_CH_DB_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_CH_DB_ADDR, HWIO_GSI_IRAM_PTR_CH_DB_RMSK)
#define HWIO_GSI_IRAM_PTR_CH_DB_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_CH_DB_ADDR, m)
#define HWIO_GSI_IRAM_PTR_CH_DB_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_CH_DB_ADDR,v)
#define HWIO_GSI_IRAM_PTR_CH_DB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_CH_DB_ADDR,m,v,HWIO_GSI_IRAM_PTR_CH_DB_IN)
#define HWIO_GSI_IRAM_PTR_CH_DB_IRAM_PTR_BMSK                                                          0x1fff
#define HWIO_GSI_IRAM_PTR_CH_DB_IRAM_PTR_SHFT                                                             0x0

#define HWIO_GSI_IRAM_PTR_NEW_RE_ADDR                                                              (GSI_TOP_GSI_REG_BASE      + 0x00004420)
#define HWIO_GSI_IRAM_PTR_NEW_RE_PHYS                                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004420)
#define HWIO_GSI_IRAM_PTR_NEW_RE_OFFS                                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004420)
#define HWIO_GSI_IRAM_PTR_NEW_RE_RMSK                                                                  0x1fff
#define HWIO_GSI_IRAM_PTR_NEW_RE_ATTR                                                                     0x3
#define HWIO_GSI_IRAM_PTR_NEW_RE_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_NEW_RE_ADDR, HWIO_GSI_IRAM_PTR_NEW_RE_RMSK)
#define HWIO_GSI_IRAM_PTR_NEW_RE_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_NEW_RE_ADDR, m)
#define HWIO_GSI_IRAM_PTR_NEW_RE_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_NEW_RE_ADDR,v)
#define HWIO_GSI_IRAM_PTR_NEW_RE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_NEW_RE_ADDR,m,v,HWIO_GSI_IRAM_PTR_NEW_RE_IN)
#define HWIO_GSI_IRAM_PTR_NEW_RE_IRAM_PTR_BMSK                                                         0x1fff
#define HWIO_GSI_IRAM_PTR_NEW_RE_IRAM_PTR_SHFT                                                            0x0

#define HWIO_GSI_IRAM_PTR_CH_DIS_COMP_ADDR                                                         (GSI_TOP_GSI_REG_BASE      + 0x00004424)
#define HWIO_GSI_IRAM_PTR_CH_DIS_COMP_PHYS                                                         (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004424)
#define HWIO_GSI_IRAM_PTR_CH_DIS_COMP_OFFS                                                         (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004424)
#define HWIO_GSI_IRAM_PTR_CH_DIS_COMP_RMSK                                                             0x1fff
#define HWIO_GSI_IRAM_PTR_CH_DIS_COMP_ATTR                                                                0x3
#define HWIO_GSI_IRAM_PTR_CH_DIS_COMP_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_CH_DIS_COMP_ADDR, HWIO_GSI_IRAM_PTR_CH_DIS_COMP_RMSK)
#define HWIO_GSI_IRAM_PTR_CH_DIS_COMP_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_CH_DIS_COMP_ADDR, m)
#define HWIO_GSI_IRAM_PTR_CH_DIS_COMP_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_CH_DIS_COMP_ADDR,v)
#define HWIO_GSI_IRAM_PTR_CH_DIS_COMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_CH_DIS_COMP_ADDR,m,v,HWIO_GSI_IRAM_PTR_CH_DIS_COMP_IN)
#define HWIO_GSI_IRAM_PTR_CH_DIS_COMP_IRAM_PTR_BMSK                                                    0x1fff
#define HWIO_GSI_IRAM_PTR_CH_DIS_COMP_IRAM_PTR_SHFT                                                       0x0

#define HWIO_GSI_IRAM_PTR_CH_EMPTY_ADDR                                                            (GSI_TOP_GSI_REG_BASE      + 0x00004428)
#define HWIO_GSI_IRAM_PTR_CH_EMPTY_PHYS                                                            (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004428)
#define HWIO_GSI_IRAM_PTR_CH_EMPTY_OFFS                                                            (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004428)
#define HWIO_GSI_IRAM_PTR_CH_EMPTY_RMSK                                                                0x1fff
#define HWIO_GSI_IRAM_PTR_CH_EMPTY_ATTR                                                                   0x3
#define HWIO_GSI_IRAM_PTR_CH_EMPTY_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_CH_EMPTY_ADDR, HWIO_GSI_IRAM_PTR_CH_EMPTY_RMSK)
#define HWIO_GSI_IRAM_PTR_CH_EMPTY_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_CH_EMPTY_ADDR, m)
#define HWIO_GSI_IRAM_PTR_CH_EMPTY_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_CH_EMPTY_ADDR,v)
#define HWIO_GSI_IRAM_PTR_CH_EMPTY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_CH_EMPTY_ADDR,m,v,HWIO_GSI_IRAM_PTR_CH_EMPTY_IN)
#define HWIO_GSI_IRAM_PTR_CH_EMPTY_IRAM_PTR_BMSK                                                       0x1fff
#define HWIO_GSI_IRAM_PTR_CH_EMPTY_IRAM_PTR_SHFT                                                          0x0

#define HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_ADDR                                                      (GSI_TOP_GSI_REG_BASE      + 0x0000442c)
#define HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_PHYS                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000442c)
#define HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_OFFS                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000442c)
#define HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_RMSK                                                          0x1fff
#define HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_ATTR                                                             0x3
#define HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_ADDR, HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_RMSK)
#define HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_ADDR, m)
#define HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_ADDR,v)
#define HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_ADDR,m,v,HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_IN)
#define HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_IRAM_PTR_BMSK                                                 0x1fff
#define HWIO_GSI_IRAM_PTR_EVENT_GEN_COMP_IRAM_PTR_SHFT                                                    0x0

#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_ADDR                                                  (GSI_TOP_GSI_REG_BASE      + 0x00004430)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_PHYS                                                  (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004430)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_OFFS                                                  (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004430)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_RMSK                                                      0x1fff
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_ATTR                                                         0x3
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_ADDR, HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_RMSK)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_ADDR, m)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_ADDR,v)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_ADDR,m,v,HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_IN)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_IRAM_PTR_BMSK                                             0x1fff
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_0_IRAM_PTR_SHFT                                                0x0

#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_ADDR                                                  (GSI_TOP_GSI_REG_BASE      + 0x00004434)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_PHYS                                                  (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004434)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_OFFS                                                  (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004434)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_RMSK                                                      0x1fff
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_ATTR                                                         0x3
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_ADDR, HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_RMSK)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_ADDR, m)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_ADDR,v)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_ADDR,m,v,HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_IN)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_IRAM_PTR_BMSK                                             0x1fff
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_2_IRAM_PTR_SHFT                                                0x0

#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_ADDR                                                  (GSI_TOP_GSI_REG_BASE      + 0x00004438)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_PHYS                                                  (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004438)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_OFFS                                                  (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004438)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_RMSK                                                      0x1fff
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_ATTR                                                         0x3
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_ADDR, HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_RMSK)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_ADDR, m)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_ADDR,v)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_ADDR,m,v,HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_IN)
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_IRAM_PTR_BMSK                                             0x1fff
#define HWIO_GSI_IRAM_PTR_PERIPH_IF_TLV_IN_1_IRAM_PTR_SHFT                                                0x0

#define HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_ADDR                                                       (GSI_TOP_GSI_REG_BASE      + 0x0000443c)
#define HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_PHYS                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000443c)
#define HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_OFFS                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000443c)
#define HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_RMSK                                                           0x1fff
#define HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_ATTR                                                              0x3
#define HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_ADDR, HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_RMSK)
#define HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_ADDR, m)
#define HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_ADDR,v)
#define HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_ADDR,m,v,HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_IN)
#define HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_IRAM_PTR_BMSK                                                  0x1fff
#define HWIO_GSI_IRAM_PTR_TIMER_EXPIRED_IRAM_PTR_SHFT                                                     0x0

#define HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_ADDR                                                      (GSI_TOP_GSI_REG_BASE      + 0x00004440)
#define HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_PHYS                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004440)
#define HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_OFFS                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004440)
#define HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_RMSK                                                          0x1fff
#define HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_ATTR                                                             0x3
#define HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_ADDR, HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_RMSK)
#define HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_ADDR, m)
#define HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_ADDR,v)
#define HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_ADDR,m,v,HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_IN)
#define HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_IRAM_PTR_BMSK                                                 0x1fff
#define HWIO_GSI_IRAM_PTR_WRITE_ENG_COMP_IRAM_PTR_SHFT                                                    0x0

#define HWIO_GSI_IRAM_PTR_READ_ENG_COMP_ADDR                                                       (GSI_TOP_GSI_REG_BASE      + 0x00004444)
#define HWIO_GSI_IRAM_PTR_READ_ENG_COMP_PHYS                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004444)
#define HWIO_GSI_IRAM_PTR_READ_ENG_COMP_OFFS                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004444)
#define HWIO_GSI_IRAM_PTR_READ_ENG_COMP_RMSK                                                           0x1fff
#define HWIO_GSI_IRAM_PTR_READ_ENG_COMP_ATTR                                                              0x3
#define HWIO_GSI_IRAM_PTR_READ_ENG_COMP_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_READ_ENG_COMP_ADDR, HWIO_GSI_IRAM_PTR_READ_ENG_COMP_RMSK)
#define HWIO_GSI_IRAM_PTR_READ_ENG_COMP_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_READ_ENG_COMP_ADDR, m)
#define HWIO_GSI_IRAM_PTR_READ_ENG_COMP_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_READ_ENG_COMP_ADDR,v)
#define HWIO_GSI_IRAM_PTR_READ_ENG_COMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_READ_ENG_COMP_ADDR,m,v,HWIO_GSI_IRAM_PTR_READ_ENG_COMP_IN)
#define HWIO_GSI_IRAM_PTR_READ_ENG_COMP_IRAM_PTR_BMSK                                                  0x1fff
#define HWIO_GSI_IRAM_PTR_READ_ENG_COMP_IRAM_PTR_SHFT                                                     0x0

#define HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_ADDR                                                      (GSI_TOP_GSI_REG_BASE      + 0x0000444c)
#define HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_PHYS                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000444c)
#define HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_OFFS                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000444c)
#define HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_RMSK                                                          0x1fff
#define HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_ATTR                                                             0x3
#define HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_ADDR, HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_RMSK)
#define HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_ADDR, m)
#define HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_ADDR,v)
#define HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_ADDR,m,v,HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_IN)
#define HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_IRAM_PTR_BMSK                                                 0x1fff
#define HWIO_GSI_IRAM_PTR_INT_MOD_STOPED_IRAM_PTR_SHFT                                                    0x0

#define HWIO_GSI_IRAM_PTR_SDMA_INT_n_ADDR(n)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00004450 + 0x4 * (n))
#define HWIO_GSI_IRAM_PTR_SDMA_INT_n_PHYS(n)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004450 + 0x4 * (n))
#define HWIO_GSI_IRAM_PTR_SDMA_INT_n_OFFS(n)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004450 + 0x4 * (n))
#define HWIO_GSI_IRAM_PTR_SDMA_INT_n_RMSK                                                              0x1fff
#define HWIO_GSI_IRAM_PTR_SDMA_INT_n_MAXn                                                                   3
#define HWIO_GSI_IRAM_PTR_SDMA_INT_n_ATTR                                                                 0x1
#define HWIO_GSI_IRAM_PTR_SDMA_INT_n_INI(n)        \
        in_dword_masked(HWIO_GSI_IRAM_PTR_SDMA_INT_n_ADDR(n), HWIO_GSI_IRAM_PTR_SDMA_INT_n_RMSK)
#define HWIO_GSI_IRAM_PTR_SDMA_INT_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_IRAM_PTR_SDMA_INT_n_ADDR(n), mask)
#define HWIO_GSI_IRAM_PTR_SDMA_INT_n_IRAM_PTR_BMSK                                                     0x1fff
#define HWIO_GSI_IRAM_PTR_SDMA_INT_n_IRAM_PTR_SHFT                                                        0x0

#define HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_ADDR                                                      (GSI_TOP_GSI_REG_BASE      + 0x00004470)
#define HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_PHYS                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004470)
#define HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_OFFS                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004470)
#define HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_RMSK                                                          0x1fff
#define HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_ATTR                                                             0x3
#define HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_ADDR, HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_RMSK)
#define HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_ADDR, m)
#define HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_ADDR,v)
#define HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_ADDR,m,v,HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_IN)
#define HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_IRAM_PTR_BMSK                                                 0x1fff
#define HWIO_GSI_IRAM_PTR_INT_NOTIFY_MCS_IRAM_PTR_SHFT                                                    0x0

#define HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_ADDR                                                    (GSI_TOP_GSI_REG_BASE      + 0x00004474)
#define HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_PHYS                                                    (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004474)
#define HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_OFFS                                                    (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004474)
#define HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_RMSK                                                        0x1fff
#define HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_ATTR                                                           0x3
#define HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_IN          \
        in_dword_masked(HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_ADDR, HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_RMSK)
#define HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_INM(m)      \
        in_dword_masked(HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_ADDR, m)
#define HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_OUT(v)      \
        out_dword(HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_ADDR,v)
#define HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_ADDR,m,v,HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_IN)
#define HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_IRAM_PTR_BMSK                                               0x1fff
#define HWIO_GSI_IRAM_PTR_INT_MOD_CANCELED_IRAM_PTR_SHFT                                                  0x0

/*
        Added Mannually, GSI_INST_RAM_n change to GSI_INST_RAM_0.
        HWIO gen needs help to create the HWIO_GSI_INST_RAM_0_ADDR_n macro
*/
#define HWIO_GSI_INST_RAM_0_ADDR_n(n)                                                              (GSI_TOP_GSI_REG_BASE      + 0x000a8000 + 0x4 * (n))
#define HWIO_GSI_INST_RAM_0_ADDR                                                                   (GSI_TOP_GSI_REG_BASE      + 0x000a8000)
#define HWIO_GSI_INST_RAM_0_PHYS                                                                   (GSI_TOP_GSI_REG_BASE_PHYS + 0x000a8000)
#define HWIO_GSI_INST_RAM_0_OFFS                                                                   (GSI_TOP_GSI_REG_BASE_OFFS + 0x000a8000)
#define HWIO_GSI_INST_RAM_0_RMSK                                                                   0xffffffff
#define HWIO_GSI_INST_RAM_0_ATTR                                                                          0x3
#define HWIO_GSI_INST_RAM_0_INI(n)        \
        in_dword_masked(HWIO_GSI_INST_RAM_0_ADDR_n(n), HWIO_GSI_INST_RAM_0_RMSK, HWIO_GSI_INST_RAM_0_ATTR)
#define HWIO_GSI_INST_RAM_0_IN          \
        in_dword_masked(HWIO_GSI_INST_RAM_0_ADDR, HWIO_GSI_INST_RAM_0_RMSK)
#define HWIO_GSI_INST_RAM_0_INM(m)      \
        in_dword_masked(HWIO_GSI_INST_RAM_0_ADDR, m)
#define HWIO_GSI_INST_RAM_0_OUT(v)      \
        out_dword(HWIO_GSI_INST_RAM_0_ADDR,v)
#define HWIO_GSI_INST_RAM_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_INST_RAM_0_ADDR,m,v,HWIO_GSI_INST_RAM_0_IN)
#define HWIO_GSI_INST_RAM_0_INST_BYTE_3_BMSK                                                       0xff000000
#define HWIO_GSI_INST_RAM_0_INST_BYTE_3_SHFT                                                             0x18
#define HWIO_GSI_INST_RAM_0_INST_BYTE_2_BMSK                                                         0xff0000
#define HWIO_GSI_INST_RAM_0_INST_BYTE_2_SHFT                                                             0x10
#define HWIO_GSI_INST_RAM_0_INST_BYTE_1_BMSK                                                           0xff00
#define HWIO_GSI_INST_RAM_0_INST_BYTE_1_SHFT                                                              0x8
#define HWIO_GSI_INST_RAM_0_INST_BYTE_0_BMSK                                                             0xff
#define HWIO_GSI_INST_RAM_0_INST_BYTE_0_SHFT                                                              0x0

/*
        Added Mannually, GSI_SHRAM_n change to GSI_SHRAM_0.
        HWIO gen needs help to create the HWIO_GSI_SHRAM_0_ADDR_n macro
*/
#define HWIO_GSI_SHRAM_0_ADDR_n(n)                                                                 (GSI_TOP_GSI_REG_BASE      + 0x00006200 + 0x4 * (n))
#define HWIO_GSI_SHRAM_0_ADDR                                                                      (GSI_TOP_GSI_REG_BASE      + 0x00006200)
#define HWIO_GSI_SHRAM_0_PHYS                                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006200)
#define HWIO_GSI_SHRAM_0_OFFS                                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006200)
#define HWIO_GSI_SHRAM_0_RMSK                                                                      0xffffffff
#define HWIO_GSI_SHRAM_0_ATTR                                                                             0x3
#define HWIO_GSI_SHRAM_0_INI(n)        \
        in_dword_masked(HWIO_GSI_SHRAM_0_ADDR_n(n), HWIO_GSI_SHRAM_0_RMSK, HWIO_GSI_SHRAM_0_ATTR)
#define HWIO_GSI_SHRAM_0_IN          \
        in_dword_masked(HWIO_GSI_SHRAM_0_ADDR, HWIO_GSI_SHRAM_0_RMSK)
#define HWIO_GSI_SHRAM_0_INM(m)      \
        in_dword_masked(HWIO_GSI_SHRAM_0_ADDR, m)
#define HWIO_GSI_SHRAM_0_OUT(v)      \
        out_dword(HWIO_GSI_SHRAM_0_ADDR,v,HWIO_GSI_SHRAM_0_ATTR)
#define HWIO_GSI_SHRAM_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_SHRAM_0_ADDR,m,v,HWIO_GSI_SHRAM_0_IN)
#define HWIO_GSI_SHRAM_0_SHRAM_BMSK                                                                0xffffffff
#define HWIO_GSI_SHRAM_0_SHRAM_SHFT                                                                       0x0

#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_ADDR(n,k)                                                  (GSI_TOP_GSI_REG_BASE      + 0x0000d200 + 0x400 * (n) + 0x4 * (k))
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_PHYS(n,k)                                                  (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000d200 + 0x400 * (n) + 0x4 * (k))
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_OFFS(n,k)                                                  (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000d200 + 0x400 * (n) + 0x4 * (k))
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_RMSK                                                            0x1ff
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_MAXn                                                                4
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_MAXk                                                               64
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_ATTR                                                              0x3
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_INI2(n,k)        \
        in_dword_masked(HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_ADDR(n,k), HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_RMSK)
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_ADDR(n,k), mask)
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_OUTI2(n,k,val)    \
        out_dword(HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_ADDR(n,k),val)
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_ADDR(n,k),mask,val,HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_INI2(n,k))
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_VALID_BMSK                                                      0x100
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_VALID_SHFT                                                        0x8
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_PHY_CH_BMSK                                                      0xff
#define HWIO_GSI_MAP_EE_n_CH_k_VP_TABLE_PHY_CH_SHFT                                                       0x0

#define HWIO_GSI_TEST_BUS_SEL_ADDR                                                                 (GSI_TOP_GSI_REG_BASE      + 0x00005000)
#define HWIO_GSI_TEST_BUS_SEL_PHYS                                                                 (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005000)
#define HWIO_GSI_TEST_BUS_SEL_OFFS                                                                 (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005000)
#define HWIO_GSI_TEST_BUS_SEL_RMSK                                                                    0xf00ff
#define HWIO_GSI_TEST_BUS_SEL_ATTR                                                                        0x3
#define HWIO_GSI_TEST_BUS_SEL_IN          \
        in_dword_masked(HWIO_GSI_TEST_BUS_SEL_ADDR, HWIO_GSI_TEST_BUS_SEL_RMSK)
#define HWIO_GSI_TEST_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_GSI_TEST_BUS_SEL_ADDR, m)
#define HWIO_GSI_TEST_BUS_SEL_OUT(v)      \
        out_dword(HWIO_GSI_TEST_BUS_SEL_ADDR,v,HWIO_GSI_TEST_BUS_SEL_ATTR)
#define HWIO_GSI_TEST_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_TEST_BUS_SEL_ADDR,m,v,HWIO_GSI_TEST_BUS_SEL_IN)
#define HWIO_GSI_TEST_BUS_SEL_GSI_HW_EVENTS_SEL_BMSK                                                  0xf0000
#define HWIO_GSI_TEST_BUS_SEL_GSI_HW_EVENTS_SEL_SHFT                                                     0x10
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_BMSK                                                       0xff
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_SHFT                                                        0x0
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_ZEROS_FVAL                                                  0x0
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_DB_ENG_FVAL                                                 0x9
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_REE_0_FVAL                                                  0xb
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_REE_1_FVAL                                                  0xc
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_REE_2_FVAL                                                  0xd
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_REE_3_FVAL                                                  0xe
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_REE_4_FVAL                                                  0xf
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_REE_5_FVAL                                                 0x10
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_REE_6_FVAL                                                 0x11
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_REE_7_FVAL                                                 0x12
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_EVE_0_FVAL                                                 0x13
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_EVE_1_FVAL                                                 0x14
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_EVE_2_FVAL                                                 0x15
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_EVE_3_FVAL                                                 0x16
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_EVE_4_FVAL                                                 0x17
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_EVE_5_FVAL                                                 0x18
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_IE_0_FVAL                                                  0x1b
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_IE_1_FVAL                                                  0x1c
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_IE_2_FVAL                                                  0x1d
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_IC_0_FVAL                                                  0x1f
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_IC_1_FVAL                                                  0x20
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_IC_2_FVAL                                                  0x21
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_IC_3_FVAL                                                  0x22
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_IC_4_FVAL                                                  0x23
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MOQA_0_FVAL                                                0x27
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MOQA_1_FVAL                                                0x28
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MOQA_2_FVAL                                                0x29
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MOQA_3_FVAL                                                0x2a
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_TMR_0_FVAL                                                 0x2b
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_TMR_1_FVAL                                                 0x2c
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_TMR_2_FVAL                                                 0x2d
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_TMR_3_FVAL                                                 0x2e
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_RD_WR_0_FVAL                                               0x33
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_RD_WR_1_FVAL                                               0x34
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_RD_WR_2_FVAL                                               0x35
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_RD_WR_3_FVAL                                               0x36
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_CSR_FVAL                                                   0x3a
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_SDMA_0_FVAL                                                0x3c
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_SMDA_1_FVAL                                                0x3d
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_CSR_1_FVAL                                                 0x3e
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_CSR_2_FVAL                                                 0x3f
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS_5_FVAL                                                 0x40
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_IC_5_FVAL                                                  0x41
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_CSR_3_FVAL                                                 0x42
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_TLV_0_FVAL                                                 0x43
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_REE_8_FVAL                                                 0x44
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_IE_NOTIFY_FVAL                                             0x45
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_DB_MSI_FVAL                                                0x46
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_REE_9_FVAL                                                 0x47
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS0_0_FVAL                                                0x48
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS0_1_FVAL                                                0x49
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS0_2_FVAL                                                0x4a
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS0_3_FVAL                                                0x4b
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS0_4_FVAL                                                0x4c
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS0_5_FVAL                                                0x4d
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS0_6_FVAL                                                0x4e
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS1_0_FVAL                                                0x4f
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS1_1_FVAL                                                0x50
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS1_2_FVAL                                                0x51
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS1_3_FVAL                                                0x52
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS1_4_FVAL                                                0x53
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS1_5_FVAL                                                0x54
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS1_6_FVAL                                                0x55
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS2_0_FVAL                                                0x56
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS2_1_FVAL                                                0x57
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS2_2_FVAL                                                0x58
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS2_3_FVAL                                                0x59
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS2_4_FVAL                                                0x5a
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS2_5_FVAL                                                0x5b
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS2_6_FVAL                                                0x5c
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS3_0_FVAL                                                0x5d
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS3_1_FVAL                                                0x5e
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS3_2_FVAL                                                0x5f
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS3_3_FVAL                                                0x60
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS3_4_FVAL                                                0x61
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS3_5_FVAL                                                0x62
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS3_6_FVAL                                                0x63
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS4_0_FVAL                                                0x64
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS4_1_FVAL                                                0x65
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS4_2_FVAL                                                0x66
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS4_3_FVAL                                                0x67
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS4_4_FVAL                                                0x68
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS4_5_FVAL                                                0x69
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS4_6_FVAL                                                0x6a
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS5_0_FVAL                                                0x6b
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS5_1_FVAL                                                0x6c
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS5_2_FVAL                                                0x6d
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS5_3_FVAL                                                0x6e
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS5_4_FVAL                                                0x6f
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS5_5_FVAL                                                0x70
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS5_6_FVAL                                                0x71
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS6_0_FVAL                                                0x72
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS6_1_FVAL                                                0x73
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS6_2_FVAL                                                0x74
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS6_3_FVAL                                                0x75
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS6_4_FVAL                                                0x76
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS6_5_FVAL                                                0x77
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS6_6_FVAL                                                0x78
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS7_0_FVAL                                                0x79
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS7_1_FVAL                                                0x7a
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS7_2_FVAL                                                0x7b
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS7_3_FVAL                                                0x7c
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS7_4_FVAL                                                0x7d
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS7_5_FVAL                                                0x7e
#define HWIO_GSI_TEST_BUS_SEL_GSI_TESTBUS_SEL_MCS7_6_FVAL                                                0x7f

#define HWIO_GSI_TEST_BUS_REG_ADDR                                                                 (GSI_TOP_GSI_REG_BASE      + 0x00005008)
#define HWIO_GSI_TEST_BUS_REG_PHYS                                                                 (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005008)
#define HWIO_GSI_TEST_BUS_REG_OFFS                                                                 (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005008)
#define HWIO_GSI_TEST_BUS_REG_RMSK                                                                 0xffffffff
#define HWIO_GSI_TEST_BUS_REG_ATTR                                                                        0x1
#define HWIO_GSI_TEST_BUS_REG_IN          \
        in_dword_masked(HWIO_GSI_TEST_BUS_REG_ADDR, HWIO_GSI_TEST_BUS_REG_RMSK, HWIO_GSI_TEST_BUS_REG_ATTR)
#define HWIO_GSI_TEST_BUS_REG_INM(m)      \
        in_dword_masked(HWIO_GSI_TEST_BUS_REG_ADDR, m, HWIO_GSI_TEST_BUS_REG_ATTR)
#define HWIO_GSI_TEST_BUS_REG_GSI_TESTBUS_REG_BMSK                                                 0xffffffff
#define HWIO_GSI_TEST_BUS_REG_GSI_TESTBUS_REG_SHFT                                                        0x0

#define HWIO_GSI_DEBUG_BUSY_REG_ADDR                                                               (GSI_TOP_GSI_REG_BASE      + 0x00005010)
#define HWIO_GSI_DEBUG_BUSY_REG_PHYS                                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005010)
#define HWIO_GSI_DEBUG_BUSY_REG_OFFS                                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005010)
#define HWIO_GSI_DEBUG_BUSY_REG_RMSK                                                               0xfffffbfb
#define HWIO_GSI_DEBUG_BUSY_REG_ATTR                                                                      0x1
#define HWIO_GSI_DEBUG_BUSY_REG_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_BUSY_REG_ADDR, HWIO_GSI_DEBUG_BUSY_REG_RMSK)
#define HWIO_GSI_DEBUG_BUSY_REG_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_BUSY_REG_ADDR, m)
#define HWIO_GSI_DEBUG_BUSY_REG_MCS_BUSY_BMSK                                                      0xff000000
#define HWIO_GSI_DEBUG_BUSY_REG_MCS_BUSY_SHFT                                                            0x18
#define HWIO_GSI_DEBUG_BUSY_REG_MUTEX_REQ_BUSY_BMSK                                                  0xff0000
#define HWIO_GSI_DEBUG_BUSY_REG_MUTEX_REQ_BUSY_SHFT                                                      0x10
#define HWIO_GSI_DEBUG_BUSY_REG_WR_BUSY_BMSK                                                           0x8000
#define HWIO_GSI_DEBUG_BUSY_REG_WR_BUSY_SHFT                                                              0xf
#define HWIO_GSI_DEBUG_BUSY_REG_RD_BUSY_BMSK                                                           0x4000
#define HWIO_GSI_DEBUG_BUSY_REG_RD_BUSY_SHFT                                                              0xe
#define HWIO_GSI_DEBUG_BUSY_REG_DB_MSI_ENG_BUSY_BMSK                                                   0x2000
#define HWIO_GSI_DEBUG_BUSY_REG_DB_MSI_ENG_BUSY_SHFT                                                      0xd
#define HWIO_GSI_DEBUG_BUSY_REG_SDMA_BUSY_BMSK                                                         0x1000
#define HWIO_GSI_DEBUG_BUSY_REG_SDMA_BUSY_SHFT                                                            0xc
#define HWIO_GSI_DEBUG_BUSY_REG_IC_BUSY_BMSK                                                            0x800
#define HWIO_GSI_DEBUG_BUSY_REG_IC_BUSY_SHFT                                                              0xb
#define HWIO_GSI_DEBUG_BUSY_REG_DBG_CNT_BUSY_BMSK                                                       0x200
#define HWIO_GSI_DEBUG_BUSY_REG_DBG_CNT_BUSY_SHFT                                                         0x9
#define HWIO_GSI_DEBUG_BUSY_REG_DB_ENG_BUSY_BMSK                                                        0x100
#define HWIO_GSI_DEBUG_BUSY_REG_DB_ENG_BUSY_SHFT                                                          0x8
#define HWIO_GSI_DEBUG_BUSY_REG_REE_PWR_CLPS_BUSY_BMSK                                                   0x80
#define HWIO_GSI_DEBUG_BUSY_REG_REE_PWR_CLPS_BUSY_SHFT                                                    0x7
#define HWIO_GSI_DEBUG_BUSY_REG_INT_ENG_BUSY_BMSK                                                        0x40
#define HWIO_GSI_DEBUG_BUSY_REG_INT_ENG_BUSY_SHFT                                                         0x6
#define HWIO_GSI_DEBUG_BUSY_REG_EV_ENG_BUSY_BMSK                                                         0x20
#define HWIO_GSI_DEBUG_BUSY_REG_EV_ENG_BUSY_SHFT                                                          0x5
#define HWIO_GSI_DEBUG_BUSY_REG_RD_WR_BUSY_BMSK                                                          0x10
#define HWIO_GSI_DEBUG_BUSY_REG_RD_WR_BUSY_SHFT                                                           0x4
#define HWIO_GSI_DEBUG_BUSY_REG_TIMER_BUSY_BMSK                                                           0x8
#define HWIO_GSI_DEBUG_BUSY_REG_TIMER_BUSY_SHFT                                                           0x3
#define HWIO_GSI_DEBUG_BUSY_REG_REE_BUSY_BMSK                                                             0x2
#define HWIO_GSI_DEBUG_BUSY_REG_REE_BUSY_SHFT                                                             0x1
#define HWIO_GSI_DEBUG_BUSY_REG_CSR_BUSY_BMSK                                                             0x1
#define HWIO_GSI_DEBUG_BUSY_REG_CSR_BUSY_SHFT                                                             0x0

#define HWIO_GSI_DEBUG_MUTEX_SEL_ADDR                                                              (GSI_TOP_GSI_REG_BASE      + 0x00005014)
#define HWIO_GSI_DEBUG_MUTEX_SEL_PHYS                                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005014)
#define HWIO_GSI_DEBUG_MUTEX_SEL_OFFS                                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005014)
#define HWIO_GSI_DEBUG_MUTEX_SEL_RMSK                                                                 0x3ff07
#define HWIO_GSI_DEBUG_MUTEX_SEL_ATTR                                                                     0x3
#define HWIO_GSI_DEBUG_MUTEX_SEL_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_SEL_ADDR, HWIO_GSI_DEBUG_MUTEX_SEL_RMSK)
#define HWIO_GSI_DEBUG_MUTEX_SEL_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_SEL_ADDR, m)
#define HWIO_GSI_DEBUG_MUTEX_SEL_OUT(v)      \
        out_dword(HWIO_GSI_DEBUG_MUTEX_SEL_ADDR,v)
#define HWIO_GSI_DEBUG_MUTEX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_DEBUG_MUTEX_SEL_ADDR,m,v,HWIO_GSI_DEBUG_MUTEX_SEL_IN)
#define HWIO_GSI_DEBUG_MUTEX_SEL_REGION_TYPE_SEL_BMSK                                                 0x30000
#define HWIO_GSI_DEBUG_MUTEX_SEL_REGION_TYPE_SEL_SHFT                                                    0x10
#define HWIO_GSI_DEBUG_MUTEX_SEL_LINE_SEL_BMSK                                                         0xff00
#define HWIO_GSI_DEBUG_MUTEX_SEL_LINE_SEL_SHFT                                                            0x8
#define HWIO_GSI_DEBUG_MUTEX_SEL_MCS_SEL_BMSK                                                             0x7
#define HWIO_GSI_DEBUG_MUTEX_SEL_MCS_SEL_SHFT                                                             0x0

#define HWIO_GSI_DEBUG_MUTEX_STATUS_ADDR                                                           (GSI_TOP_GSI_REG_BASE      + 0x0000501c)
#define HWIO_GSI_DEBUG_MUTEX_STATUS_PHYS                                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000501c)
#define HWIO_GSI_DEBUG_MUTEX_STATUS_OFFS                                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000501c)
#define HWIO_GSI_DEBUG_MUTEX_STATUS_RMSK                                                             0xffffff
#define HWIO_GSI_DEBUG_MUTEX_STATUS_ATTR                                                                  0x1
#define HWIO_GSI_DEBUG_MUTEX_STATUS_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_STATUS_ADDR, HWIO_GSI_DEBUG_MUTEX_STATUS_RMSK)
#define HWIO_GSI_DEBUG_MUTEX_STATUS_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_STATUS_ADDR, m)
#define HWIO_GSI_DEBUG_MUTEX_STATUS_LOCK_OVERFLOW_BMSK                                               0xff0000
#define HWIO_GSI_DEBUG_MUTEX_STATUS_LOCK_OVERFLOW_SHFT                                                   0x10
#define HWIO_GSI_DEBUG_MUTEX_STATUS_TABLE_FULL_BMSK                                                    0xff00
#define HWIO_GSI_DEBUG_MUTEX_STATUS_TABLE_FULL_SHFT                                                       0x8
#define HWIO_GSI_DEBUG_MUTEX_STATUS_TABLE_EMPTY_BMSK                                                     0xff
#define HWIO_GSI_DEBUG_MUTEX_STATUS_TABLE_EMPTY_SHFT                                                      0x0

#define HWIO_GSI_DEBUG_MUTEX_LOCKED_LINE_ADDR                                                      (GSI_TOP_GSI_REG_BASE      + 0x00005020)
#define HWIO_GSI_DEBUG_MUTEX_LOCKED_LINE_PHYS                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005020)
#define HWIO_GSI_DEBUG_MUTEX_LOCKED_LINE_OFFS                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005020)
#define HWIO_GSI_DEBUG_MUTEX_LOCKED_LINE_RMSK                                                      0xffffffff
#define HWIO_GSI_DEBUG_MUTEX_LOCKED_LINE_ATTR                                                             0x1
#define HWIO_GSI_DEBUG_MUTEX_LOCKED_LINE_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_LOCKED_LINE_ADDR, HWIO_GSI_DEBUG_MUTEX_LOCKED_LINE_RMSK)
#define HWIO_GSI_DEBUG_MUTEX_LOCKED_LINE_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_LOCKED_LINE_ADDR, m)
#define HWIO_GSI_DEBUG_MUTEX_LOCKED_LINE_LINE_BMSK                                                 0xffffffff
#define HWIO_GSI_DEBUG_MUTEX_LOCKED_LINE_LINE_SHFT                                                        0x0

#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_ADDR                                                   (GSI_TOP_GSI_REG_BASE      + 0x00005024)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_PHYS                                                   (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005024)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_OFFS                                                   (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005024)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_RMSK                                                   0xffff0000
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_ATTR                                                          0x3
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_ADDR, HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_RMSK)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_ADDR, m)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_OUT(v)      \
        out_dword(HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_ADDR,v)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_ADDR,m,v,HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_IN)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_REGION_TYPE_1_BMSK                                     0xffff0000
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_01_REGION_TYPE_1_SHFT                                           0x10

#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_ADDR                                                   (GSI_TOP_GSI_REG_BASE      + 0x00005028)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_PHYS                                                   (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005028)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_OFFS                                                   (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005028)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_RMSK                                                   0xffffffff
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_ATTR                                                          0x3
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_ADDR, HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_RMSK)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_ADDR, m)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_OUT(v)      \
        out_dword(HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_ADDR,v)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_ADDR,m,v,HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_IN)
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_REGION_TYPE_3_BMSK                                     0xffff0000
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_REGION_TYPE_3_SHFT                                           0x10
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_REGION_TYPE_2_BMSK                                         0xffff
#define HWIO_GSI_DEBUG_MUTEX_REGION_TYPE_23_REGION_TYPE_2_SHFT                                            0x0

#define HWIO_GSI_DEBUG_MUTEX_LOCK_CNT_ADDR                                                         (GSI_TOP_GSI_REG_BASE      + 0x0000502c)
#define HWIO_GSI_DEBUG_MUTEX_LOCK_CNT_PHYS                                                         (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000502c)
#define HWIO_GSI_DEBUG_MUTEX_LOCK_CNT_OFFS                                                         (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000502c)
#define HWIO_GSI_DEBUG_MUTEX_LOCK_CNT_RMSK                                                            0x1ffff
#define HWIO_GSI_DEBUG_MUTEX_LOCK_CNT_ATTR                                                                0x1
#define HWIO_GSI_DEBUG_MUTEX_LOCK_CNT_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_LOCK_CNT_ADDR, HWIO_GSI_DEBUG_MUTEX_LOCK_CNT_RMSK)
#define HWIO_GSI_DEBUG_MUTEX_LOCK_CNT_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_LOCK_CNT_ADDR, m)
#define HWIO_GSI_DEBUG_MUTEX_LOCK_CNT_VAL_BMSK                                                        0x1ffff
#define HWIO_GSI_DEBUG_MUTEX_LOCK_CNT_VAL_SHFT                                                            0x0

#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_LSB_ADDR                                              (GSI_TOP_GSI_REG_BASE      + 0x00005034)
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_LSB_PHYS                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005034)
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_LSB_OFFS                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005034)
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_LSB_RMSK                                              0xffffffff
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_LSB_ATTR                                                     0x1
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_LSB_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_LSB_ADDR, HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_LSB_RMSK)
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_LSB_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_LSB_ADDR, m)
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_LSB_VAL_BMSK                                          0xffffffff
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_LSB_VAL_SHFT                                                 0x0

#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_MSB_ADDR                                              (GSI_TOP_GSI_REG_BASE      + 0x00005038)
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_MSB_PHYS                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005038)
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_MSB_OFFS                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005038)
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_MSB_RMSK                                                 0x1ffff
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_MSB_ATTR                                                     0x1
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_MSB_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_MSB_ADDR, HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_MSB_RMSK)
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_MSB_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_MSB_ADDR, m)
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_MSB_VAL_BMSK                                             0x1ffff
#define HWIO_GSI_DEBUG_MUTEX_BUSY_WAIT_TIMER_MSB_VAL_SHFT                                                 0x0

#define HWIO_GSI_DEBUG_MUTEX_CNT_RST_ADDR                                                          (GSI_TOP_GSI_REG_BASE      + 0x0000503c)
#define HWIO_GSI_DEBUG_MUTEX_CNT_RST_PHYS                                                          (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000503c)
#define HWIO_GSI_DEBUG_MUTEX_CNT_RST_OFFS                                                          (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000503c)
#define HWIO_GSI_DEBUG_MUTEX_CNT_RST_RMSK                                                                 0x1
#define HWIO_GSI_DEBUG_MUTEX_CNT_RST_ATTR                                                                 0x2
#define HWIO_GSI_DEBUG_MUTEX_CNT_RST_OUT(v)      \
        out_dword(HWIO_GSI_DEBUG_MUTEX_CNT_RST_ADDR,v)
#define HWIO_GSI_DEBUG_MUTEX_CNT_RST_CNT_RESET_BMSK                                                       0x1
#define HWIO_GSI_DEBUG_MUTEX_CNT_RST_CNT_RESET_SHFT                                                       0x0

#define HWIO_GSI_DEBUG_EVENT_PENDING_n_ADDR(n)                                                     (GSI_TOP_GSI_REG_BASE      + 0x00005f80 + 0x4 * (n))
#define HWIO_GSI_DEBUG_EVENT_PENDING_n_PHYS(n)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005f80 + 0x4 * (n))
#define HWIO_GSI_DEBUG_EVENT_PENDING_n_OFFS(n)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005f80 + 0x4 * (n))
#define HWIO_GSI_DEBUG_EVENT_PENDING_n_RMSK                                                        0xffffffff
#define HWIO_GSI_DEBUG_EVENT_PENDING_n_MAXn                                                                 2
#define HWIO_GSI_DEBUG_EVENT_PENDING_n_ATTR                                                               0x1
#define HWIO_GSI_DEBUG_EVENT_PENDING_n_INI(n)        \
        in_dword_masked(HWIO_GSI_DEBUG_EVENT_PENDING_n_ADDR(n), HWIO_GSI_DEBUG_EVENT_PENDING_n_RMSK)
#define HWIO_GSI_DEBUG_EVENT_PENDING_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_EVENT_PENDING_n_ADDR(n), mask)
#define HWIO_GSI_DEBUG_EVENT_PENDING_n_CHID_BIT_MAP_BMSK                                           0xffffffff
#define HWIO_GSI_DEBUG_EVENT_PENDING_n_CHID_BIT_MAP_SHFT                                                  0x0

#define HWIO_GSI_DEBUG_TIMER_PENDING_n_ADDR(n)                                                     (GSI_TOP_GSI_REG_BASE      + 0x00005fa0 + 0x4 * (n))
#define HWIO_GSI_DEBUG_TIMER_PENDING_n_PHYS(n)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005fa0 + 0x4 * (n))
#define HWIO_GSI_DEBUG_TIMER_PENDING_n_OFFS(n)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005fa0 + 0x4 * (n))
#define HWIO_GSI_DEBUG_TIMER_PENDING_n_RMSK                                                        0xffffffff
#define HWIO_GSI_DEBUG_TIMER_PENDING_n_MAXn                                                                 2
#define HWIO_GSI_DEBUG_TIMER_PENDING_n_ATTR                                                               0x1
#define HWIO_GSI_DEBUG_TIMER_PENDING_n_INI(n)        \
        in_dword_masked(HWIO_GSI_DEBUG_TIMER_PENDING_n_ADDR(n), HWIO_GSI_DEBUG_TIMER_PENDING_n_RMSK)
#define HWIO_GSI_DEBUG_TIMER_PENDING_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_TIMER_PENDING_n_ADDR(n), mask)
#define HWIO_GSI_DEBUG_TIMER_PENDING_n_CHID_BIT_MAP_BMSK                                           0xffffffff
#define HWIO_GSI_DEBUG_TIMER_PENDING_n_CHID_BIT_MAP_SHFT                                                  0x0

#define HWIO_GSI_DEBUG_RD_WR_PENDING_n_ADDR(n)                                                     (GSI_TOP_GSI_REG_BASE      + 0x00005fc0 + 0x4 * (n))
#define HWIO_GSI_DEBUG_RD_WR_PENDING_n_PHYS(n)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005fc0 + 0x4 * (n))
#define HWIO_GSI_DEBUG_RD_WR_PENDING_n_OFFS(n)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005fc0 + 0x4 * (n))
#define HWIO_GSI_DEBUG_RD_WR_PENDING_n_RMSK                                                        0xffffffff
#define HWIO_GSI_DEBUG_RD_WR_PENDING_n_MAXn                                                                 2
#define HWIO_GSI_DEBUG_RD_WR_PENDING_n_ATTR                                                               0x1
#define HWIO_GSI_DEBUG_RD_WR_PENDING_n_INI(n)        \
        in_dword_masked(HWIO_GSI_DEBUG_RD_WR_PENDING_n_ADDR(n), HWIO_GSI_DEBUG_RD_WR_PENDING_n_RMSK)
#define HWIO_GSI_DEBUG_RD_WR_PENDING_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_RD_WR_PENDING_n_ADDR(n), mask)
#define HWIO_GSI_DEBUG_RD_WR_PENDING_n_CHID_BIT_MAP_BMSK                                           0xffffffff
#define HWIO_GSI_DEBUG_RD_WR_PENDING_n_CHID_BIT_MAP_SHFT                                                  0x0

#define HWIO_GSI_DEBUG_PC_FROM_SW_n_ADDR(n)                                                        (GSI_TOP_GSI_REG_BASE      + 0x00005fe0 + 0x4 * (n))
#define HWIO_GSI_DEBUG_PC_FROM_SW_n_PHYS(n)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005fe0 + 0x4 * (n))
#define HWIO_GSI_DEBUG_PC_FROM_SW_n_OFFS(n)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005fe0 + 0x4 * (n))
#define HWIO_GSI_DEBUG_PC_FROM_SW_n_RMSK                                                               0x1fff
#define HWIO_GSI_DEBUG_PC_FROM_SW_n_MAXn                                                                    2
#define HWIO_GSI_DEBUG_PC_FROM_SW_n_ATTR                                                                  0x3
#define HWIO_GSI_DEBUG_PC_FROM_SW_n_INI(n)        \
        in_dword_masked(HWIO_GSI_DEBUG_PC_FROM_SW_n_ADDR(n), HWIO_GSI_DEBUG_PC_FROM_SW_n_RMSK)
#define HWIO_GSI_DEBUG_PC_FROM_SW_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_PC_FROM_SW_n_ADDR(n), mask)
#define HWIO_GSI_DEBUG_PC_FROM_SW_n_OUTI(n,val)    \
        out_dword(HWIO_GSI_DEBUG_PC_FROM_SW_n_ADDR(n),val)
#define HWIO_GSI_DEBUG_PC_FROM_SW_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_GSI_DEBUG_PC_FROM_SW_n_ADDR(n),mask,val,HWIO_GSI_DEBUG_PC_FROM_SW_n_INI(n))
#define HWIO_GSI_DEBUG_PC_FROM_SW_n_IRAM_PTR_BMSK                                                      0x1fff
#define HWIO_GSI_DEBUG_PC_FROM_SW_n_IRAM_PTR_SHFT                                                         0x0

#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_LSB_ADDR                                              (GSI_TOP_GSI_REG_BASE      + 0x00006000)
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_LSB_PHYS                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006000)
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_LSB_OFFS                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006000)
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_LSB_RMSK                                              0xffffffff
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_LSB_ATTR                                                     0x1
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_LSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_LSB_ADDR, HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_LSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_LSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_LSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_LSB_CNT_LSB_BMSK                                      0xffffffff
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_LSB_CNT_LSB_SHFT                                             0x0

#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_MSB_ADDR                                              (GSI_TOP_GSI_REG_BASE      + 0x00006004)
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_MSB_PHYS                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006004)
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_MSB_OFFS                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006004)
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_MSB_RMSK                                                  0xffff
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_MSB_ATTR                                                     0x1
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_MSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_MSB_ADDR, HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_MSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_MSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_MSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_MSB_CNT_MSB_BMSK                                          0xffff
#define HWIO_GSI_MCS_PROFILING_LEGACY_BP_CNT_MSB_CNT_MSB_SHFT                                             0x0

#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_LSB_ADDR                                       (GSI_TOP_GSI_REG_BASE      + 0x00006008)
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_LSB_PHYS                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006008)
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_LSB_OFFS                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006008)
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_LSB_RMSK                                       0xffffffff
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_LSB_ATTR                                              0x1
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_LSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_LSB_ADDR, HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_LSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_LSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_LSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_LSB_CNT_LSB_BMSK                               0xffffffff
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_LSB_CNT_LSB_SHFT                                      0x0

#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_MSB_ADDR                                       (GSI_TOP_GSI_REG_BASE      + 0x0000600c)
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_MSB_PHYS                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000600c)
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_MSB_OFFS                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000600c)
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_MSB_RMSK                                           0xffff
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_MSB_ATTR                                              0x1
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_MSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_MSB_ADDR, HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_MSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_MSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_MSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_MSB_CNT_MSB_BMSK                                   0xffff
#define HWIO_GSI_MCS_PROFILING_ASYNC_ROUTINE_BP_CNT_MSB_CNT_MSB_SHFT                                      0x0

#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_LSB_ADDR                                        (GSI_TOP_GSI_REG_BASE      + 0x00006010)
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_LSB_PHYS                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006010)
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_LSB_OFFS                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006010)
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_LSB_RMSK                                        0xffffffff
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_LSB_ATTR                                               0x1
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_LSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_LSB_ADDR, HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_LSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_LSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_LSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_LSB_CNT_LSB_BMSK                                0xffffffff
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_LSB_CNT_LSB_SHFT                                       0x0

#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_MSB_ADDR                                        (GSI_TOP_GSI_REG_BASE      + 0x00006014)
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_MSB_PHYS                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006014)
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_MSB_OFFS                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006014)
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_MSB_RMSK                                            0xffff
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_MSB_ATTR                                               0x1
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_MSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_MSB_ADDR, HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_MSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_MSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_MSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_MSB_CNT_MSB_BMSK                                    0xffff
#define HWIO_GSI_MCS_PROFILING_REORDER_FIFO_BP_CNT_MSB_CNT_MSB_SHFT                                       0x0

#define HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_ADDR                                                 (GSI_TOP_GSI_REG_BASE      + 0x000061a8)
#define HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_PHYS                                                 (GSI_TOP_GSI_REG_BASE_PHYS + 0x000061a8)
#define HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_OFFS                                                 (GSI_TOP_GSI_REG_BASE_OFFS + 0x000061a8)
#define HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_RMSK                                                        0x1
#define HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_ATTR                                                        0x3
#define HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_IN          \
        in_dword_masked(HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_ADDR, HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_RMSK)
#define HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_ADDR, m)
#define HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_OUT(v)      \
        out_dword(HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_ADDR,v)
#define HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_ADDR,m,v,HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_IN)
#define HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_WR_WAS_MISSED_BMSK                                          0x1
#define HWIO_GSI_MCS_STATS_WR_MISS_INDICATION_WR_WAS_MISSED_SHFT                                          0x0

#define HWIO_GSI_MCS_STATS_CH_EN_n_ADDR(n)                                                         (GSI_TOP_GSI_REG_BASE      + 0x000061ac + 0x4 * (n))
#define HWIO_GSI_MCS_STATS_CH_EN_n_PHYS(n)                                                         (GSI_TOP_GSI_REG_BASE_PHYS + 0x000061ac + 0x4 * (n))
#define HWIO_GSI_MCS_STATS_CH_EN_n_OFFS(n)                                                         (GSI_TOP_GSI_REG_BASE_OFFS + 0x000061ac + 0x4 * (n))
#define HWIO_GSI_MCS_STATS_CH_EN_n_RMSK                                                            0xffffffff
#define HWIO_GSI_MCS_STATS_CH_EN_n_MAXn                                                                     2
#define HWIO_GSI_MCS_STATS_CH_EN_n_ATTR                                                                   0x3
#define HWIO_GSI_MCS_STATS_CH_EN_n_INI(n)        \
        in_dword_masked(HWIO_GSI_MCS_STATS_CH_EN_n_ADDR(n), HWIO_GSI_MCS_STATS_CH_EN_n_RMSK)
#define HWIO_GSI_MCS_STATS_CH_EN_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_MCS_STATS_CH_EN_n_ADDR(n), mask)
#define HWIO_GSI_MCS_STATS_CH_EN_n_OUTI(n,val)    \
        out_dword(HWIO_GSI_MCS_STATS_CH_EN_n_ADDR(n),val)
#define HWIO_GSI_MCS_STATS_CH_EN_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_GSI_MCS_STATS_CH_EN_n_ADDR(n),mask,val,HWIO_GSI_MCS_STATS_CH_EN_n_INI(n))
#define HWIO_GSI_MCS_STATS_CH_EN_n_CHID_BIT_MAP_BMSK                                               0xffffffff
#define HWIO_GSI_MCS_STATS_CH_EN_n_CHID_BIT_MAP_SHFT                                                      0x0

#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_ADDR                                                      (GSI_TOP_GSI_REG_BASE      + 0x00006100)
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_PHYS                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006100)
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_OFFS                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006100)
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_RMSK                                                          0xffff
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_ATTR                                                             0x3
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_ADDR, HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_RMSK)
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_ADDR, m)
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_OUT(v)      \
        out_dword(HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_ADDR,v)
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_ADDR,m,v,HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_IN)
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_SEL_ENG_BMSK                                                  0xf000
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_SEL_ENG_SHFT                                                     0xc
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_SEL_IRQ_BMSK                                                   0xf00
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_SEL_IRQ_SHFT                                                     0x8
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_SEL_CH_BMSK                                                     0xff
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_SEL_SEL_CH_SHFT                                                      0x0

#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_RD_ADDR                                                       (GSI_TOP_GSI_REG_BASE      + 0x00006104)
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_RD_PHYS                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006104)
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_RD_OFFS                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006104)
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_RD_RMSK                                                              0x1
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_RD_ATTR                                                              0x1
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_RD_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_LOCK_BY_ORDER_RD_ADDR, HWIO_GSI_DEBUG_LOCK_BY_ORDER_RD_RMSK)
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_RD_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_LOCK_BY_ORDER_RD_ADDR, m)
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_RD_IS_MASKED_BMSK                                                    0x1
#define HWIO_GSI_DEBUG_LOCK_BY_ORDER_RD_IS_MASKED_SHFT                                                    0x0

#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_ADDR                                 (GSI_TOP_GSI_REG_BASE      + 0x00006108)
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_PHYS                                 (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006108)
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_OFFS                                 (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006108)
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_RMSK                                 0xffffffff
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_ATTR                                        0x1
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_ADDR, HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_RMSK)
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_ADDR, m)
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_REGION_ID_BMSK                       0xffff0000
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_REGION_ID_SHFT                             0x10
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_TIMER_MSB_BMSK                           0xffff
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_MSB_REGION_ID_TIMER_MSB_SHFT                              0x0

#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_LSB_ADDR                                           (GSI_TOP_GSI_REG_BASE      + 0x0000610c)
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_LSB_PHYS                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000610c)
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_LSB_OFFS                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000610c)
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_LSB_RMSK                                           0xffffffff
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_LSB_ATTR                                                  0x1
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_LSB_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_LSB_ADDR, HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_LSB_RMSK)
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_LSB_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_LSB_ADDR, m)
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_LSB_TIMER_LSB_BMSK                                 0xffffffff
#define HWIO_GSI_DEBUG_MUTEX_CURRENT_BUSY_TIMER_LSB_TIMER_LSB_SHFT                                        0x0

#define HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_ADDR(n)                                                    (GSI_TOP_GSI_REG_BASE      + 0x00006110 + 0x4 * (n))
#define HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_PHYS(n)                                                    (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006110 + 0x4 * (n))
#define HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_OFFS(n)                                                    (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006110 + 0x4 * (n))
#define HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_RMSK                                                       0x8000ffff
#define HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_MAXn                                                                7
#define HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_ATTR                                                              0x1
#define HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_INI(n)        \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_ADDR(n), HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_RMSK)
#define HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_ADDR(n), mask)
#define HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_LOCKED_BMSK                                                0x80000000
#define HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_LOCKED_SHFT                                                      0x1f
#define HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_REGION_ID_BMSK                                                 0xffff
#define HWIO_GSI_DEBUG_MUTEX_TABLE_RD_n_REGION_ID_SHFT                                                    0x0

#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_EVE_ADDR                                                (GSI_TOP_GSI_REG_BASE      + 0x00006150)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_EVE_PHYS                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006150)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_EVE_OFFS                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006150)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_EVE_RMSK                                                  0xffffff
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_EVE_ATTR                                                       0x1
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_EVE_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_EVE_ADDR, HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_EVE_RMSK)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_EVE_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_EVE_ADDR, m)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_EVE_FIFO_SIZE_BMSK                                            0xff
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_EVE_FIFO_SIZE_SHFT                                             0x0

#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_IE_ADDR                                                 (GSI_TOP_GSI_REG_BASE      + 0x00006154)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_IE_PHYS                                                 (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006154)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_IE_OFFS                                                 (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006154)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_IE_RMSK                                                   0xffffff
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_IE_ATTR                                                        0x1
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_IE_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_IE_ADDR, HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_IE_RMSK)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_IE_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_IE_ADDR, m)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_IE_FIFO_SIZE_BMSK                                             0xff
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_IE_FIFO_SIZE_SHFT                                              0x0

#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_WRE_ADDR                                                (GSI_TOP_GSI_REG_BASE      + 0x00006158)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_WRE_PHYS                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006158)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_WRE_OFFS                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006158)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_WRE_RMSK                                                  0xffffff
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_WRE_ATTR                                                       0x1
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_WRE_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_WRE_ADDR, HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_WRE_RMSK)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_WRE_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_WRE_ADDR, m)
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_WRE_FIFO_SIZE_BMSK                                            0xff
#define HWIO_GSI_DEBUG_REORDER_FIFO_STATUS_WRE_FIFO_SIZE_SHFT                                             0x0

#define HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_ADDR                                                     (GSI_TOP_GSI_REG_BASE      + 0x0000615c)
#define HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_PHYS                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000615c)
#define HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_OFFS                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000615c)
#define HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_RMSK                                                        0x1ffff
#define HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_ATTR                                                            0x1
#define HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_ADDR, HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_RMSK)
#define HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_ADDR, m)
#define HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_BUFF_AEMPTY_BMSK                                            0x10000
#define HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_BUFF_AEMPTY_SHFT                                               0x10
#define HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_BUSY_SLOTS_BMSK                                              0xff00
#define HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_BUSY_SLOTS_SHFT                                                 0x8
#define HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_BUFFER_SIZE_BMSK                                               0xff
#define HWIO_GSI_DEBUG_COMM_COMP_Q_BUFFER_BUFFER_SIZE_SHFT                                                0x0

#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_ADDR(n)                                           (GSI_TOP_GSI_REG_BASE      + 0x00006160 + 0x4 * (n))
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_PHYS(n)                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006160 + 0x4 * (n))
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_OFFS(n)                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006160 + 0x4 * (n))
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_RMSK                                              0xffffffff
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_MAXn                                                       5
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_ATTR                                                     0x1
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_INI(n)        \
        in_dword_masked(HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_ADDR(n), HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_RMSK)
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_ADDR(n), mask)
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_TAIL_IDX_BMSK                                     0xff000000
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_TAIL_IDX_SHFT                                           0x18
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_HEAD_IDX_BMSK                                       0xff0000
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_HEAD_IDX_SHFT                                           0x10
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_BUSY_ACTIVE_COMPLETE_SLOTS_BMSK                       0xff00
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_BUSY_ACTIVE_COMPLETE_SLOTS_SHFT                          0x8
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_BUSY_COMPLETE_SLOTS_BMSK                                0xff
#define HWIO_GSI_DEBUG_COMM_COMP_Q_ROUTINE_STT_n_BUSY_COMPLETE_SLOTS_SHFT                                 0x0

#define HWIO_GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n_ADDR(n)                                      (GSI_TOP_GSI_REG_BASE      + 0x00006180 + 0x4 * (n))
#define HWIO_GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n_PHYS(n)                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x00006180 + 0x4 * (n))
#define HWIO_GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n_OFFS(n)                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x00006180 + 0x4 * (n))
#define HWIO_GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n_RMSK                                         0xffffffff
#define HWIO_GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n_MAXn                                                  2
#define HWIO_GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n_ATTR                                                0x1
#define HWIO_GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n_INI(n)        \
        in_dword_masked(HWIO_GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n_ADDR(n), HWIO_GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n_RMSK)
#define HWIO_GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n_ADDR(n), mask)
#define HWIO_GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n_CHID_BIT_MAP_BMSK                            0xffffffff
#define HWIO_GSI_DEBUG_ASSIGN_CH_TO_THIS_MCS_STATUS_n_CHID_BIT_MAP_SHFT                                   0x0

#define HWIO_GSI_MCS_PROFILING_CNT_EN_ADDR                                                         (GSI_TOP_GSI_REG_BASE      + 0x000061a0)
#define HWIO_GSI_MCS_PROFILING_CNT_EN_PHYS                                                         (GSI_TOP_GSI_REG_BASE_PHYS + 0x000061a0)
#define HWIO_GSI_MCS_PROFILING_CNT_EN_OFFS                                                         (GSI_TOP_GSI_REG_BASE_OFFS + 0x000061a0)
#define HWIO_GSI_MCS_PROFILING_CNT_EN_RMSK                                                         0x1fffffff
#define HWIO_GSI_MCS_PROFILING_CNT_EN_ATTR                                                                0x2
#define HWIO_GSI_MCS_PROFILING_CNT_EN_OUT(v)      \
        out_dword(HWIO_GSI_MCS_PROFILING_CNT_EN_ADDR,v)
#define HWIO_GSI_MCS_PROFILING_CNT_EN_REORDER_BP_CNT_RESET_BMSK                                    0x10000000
#define HWIO_GSI_MCS_PROFILING_CNT_EN_REORDER_BP_CNT_RESET_SHFT                                          0x1c
#define HWIO_GSI_MCS_PROFILING_CNT_EN_ASYNC_ROUTINE_BP_CNT_RESET_BMSK                               0x8000000
#define HWIO_GSI_MCS_PROFILING_CNT_EN_ASYNC_ROUTINE_BP_CNT_RESET_SHFT                                    0x1b
#define HWIO_GSI_MCS_PROFILING_CNT_EN_LEGACY_BP_CNT_RESET_BMSK                                      0x4000000
#define HWIO_GSI_MCS_PROFILING_CNT_EN_LEGACY_BP_CNT_RESET_SHFT                                           0x1a
#define HWIO_GSI_MCS_PROFILING_CNT_EN_BP_AND_PENDING_CNT_RESET_BMSK                                 0x2000000
#define HWIO_GSI_MCS_PROFILING_CNT_EN_BP_AND_PENDING_CNT_RESET_SHFT                                      0x19
#define HWIO_GSI_MCS_PROFILING_CNT_EN_BP_CNT_RESET_BMSK                                             0x1000000
#define HWIO_GSI_MCS_PROFILING_CNT_EN_BP_CNT_RESET_SHFT                                                  0x18
#define HWIO_GSI_MCS_PROFILING_CNT_EN_CNT_RESET_BMSK                                                 0xff0000
#define HWIO_GSI_MCS_PROFILING_CNT_EN_CNT_RESET_SHFT                                                     0x10
#define HWIO_GSI_MCS_PROFILING_CNT_EN_EN_CLR_BMSK                                                      0xff00
#define HWIO_GSI_MCS_PROFILING_CNT_EN_EN_CLR_SHFT                                                         0x8
#define HWIO_GSI_MCS_PROFILING_CNT_EN_EN_SET_BMSK                                                        0xff
#define HWIO_GSI_MCS_PROFILING_CNT_EN_EN_SET_SHFT                                                         0x0

#define HWIO_GSI_MCS_PROFILING_CNT_EN_STATUS_ADDR                                                  (GSI_TOP_GSI_REG_BASE      + 0x000061a4)
#define HWIO_GSI_MCS_PROFILING_CNT_EN_STATUS_PHYS                                                  (GSI_TOP_GSI_REG_BASE_PHYS + 0x000061a4)
#define HWIO_GSI_MCS_PROFILING_CNT_EN_STATUS_OFFS                                                  (GSI_TOP_GSI_REG_BASE_OFFS + 0x000061a4)
#define HWIO_GSI_MCS_PROFILING_CNT_EN_STATUS_RMSK                                                        0xff
#define HWIO_GSI_MCS_PROFILING_CNT_EN_STATUS_ATTR                                                         0x1
#define HWIO_GSI_MCS_PROFILING_CNT_EN_STATUS_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_CNT_EN_STATUS_ADDR, HWIO_GSI_MCS_PROFILING_CNT_EN_STATUS_RMSK)
#define HWIO_GSI_MCS_PROFILING_CNT_EN_STATUS_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_CNT_EN_STATUS_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_CNT_EN_STATUS_EN_STATUS_BMSK                                              0xff
#define HWIO_GSI_MCS_PROFILING_CNT_EN_STATUS_EN_STATUS_SHFT                                               0x0

#define HWIO_GSI_SPARE_REG_1_ADDR                                                                  (GSI_TOP_GSI_REG_BASE      + 0x00005030)
#define HWIO_GSI_SPARE_REG_1_PHYS                                                                  (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005030)
#define HWIO_GSI_SPARE_REG_1_OFFS                                                                  (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005030)
#define HWIO_GSI_SPARE_REG_1_RMSK                                                                        0x31
#define HWIO_GSI_SPARE_REG_1_ATTR                                                                         0x3
#define HWIO_GSI_SPARE_REG_1_IN          \
        in_dword_masked(HWIO_GSI_SPARE_REG_1_ADDR, HWIO_GSI_SPARE_REG_1_RMSK)
#define HWIO_GSI_SPARE_REG_1_INM(m)      \
        in_dword_masked(HWIO_GSI_SPARE_REG_1_ADDR, m)
#define HWIO_GSI_SPARE_REG_1_OUT(v)      \
        out_dword(HWIO_GSI_SPARE_REG_1_ADDR,v)
#define HWIO_GSI_SPARE_REG_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_SPARE_REG_1_ADDR,m,v,HWIO_GSI_SPARE_REG_1_IN)
#define HWIO_GSI_SPARE_REG_1_GSI_EXTRA_RD_OT_DISABLE_BMSK                                                0x20
#define HWIO_GSI_SPARE_REG_1_GSI_EXTRA_RD_OT_DISABLE_SHFT                                                 0x5
#define HWIO_GSI_SPARE_REG_1_GSI_EXTRA_WR_OT_DISABLE_BMSK                                                0x10
#define HWIO_GSI_SPARE_REG_1_GSI_EXTRA_WR_OT_DISABLE_SHFT                                                 0x4
#define HWIO_GSI_SPARE_REG_1_FIX_IEOB_WRONG_MSK_DISABLE_BMSK                                              0x1
#define HWIO_GSI_SPARE_REG_1_FIX_IEOB_WRONG_MSK_DISABLE_SHFT                                              0x0

#define HWIO_GSI_DEBUG_MCS_SEL_ADDR                                                                (GSI_TOP_GSI_REG_BASE      + 0x00005040)
#define HWIO_GSI_DEBUG_MCS_SEL_PHYS                                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005040)
#define HWIO_GSI_DEBUG_MCS_SEL_OFFS                                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005040)
#define HWIO_GSI_DEBUG_MCS_SEL_RMSK                                                                       0x7
#define HWIO_GSI_DEBUG_MCS_SEL_ATTR                                                                       0x3
#define HWIO_GSI_DEBUG_MCS_SEL_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_MCS_SEL_ADDR, HWIO_GSI_DEBUG_MCS_SEL_RMSK)
#define HWIO_GSI_DEBUG_MCS_SEL_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_MCS_SEL_ADDR, m)
#define HWIO_GSI_DEBUG_MCS_SEL_OUT(v)      \
        out_dword(HWIO_GSI_DEBUG_MCS_SEL_ADDR,v,HWIO_GSI_DEBUG_MCS_SEL_ATTR)
#define HWIO_GSI_DEBUG_MCS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_DEBUG_MCS_SEL_ADDR,m,v,HWIO_GSI_DEBUG_MCS_SEL_IN)
#define HWIO_GSI_DEBUG_MCS_SEL_MCS_ID_BMSK                                                                0x7
#define HWIO_GSI_DEBUG_MCS_SEL_MCS_ID_SHFT                                                                0x0

#define HWIO_GSI_DEBUG_SW_STALL_ADDR                                                               (GSI_TOP_GSI_REG_BASE      + 0x00005044)
#define HWIO_GSI_DEBUG_SW_STALL_PHYS                                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005044)
#define HWIO_GSI_DEBUG_SW_STALL_OFFS                                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005044)
#define HWIO_GSI_DEBUG_SW_STALL_RMSK                                                                     0xff
#define HWIO_GSI_DEBUG_SW_STALL_ATTR                                                                      0x3
#define HWIO_GSI_DEBUG_SW_STALL_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_SW_STALL_ADDR, HWIO_GSI_DEBUG_SW_STALL_RMSK)
#define HWIO_GSI_DEBUG_SW_STALL_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_SW_STALL_ADDR, m)
#define HWIO_GSI_DEBUG_SW_STALL_OUT(v)      \
        out_dword(HWIO_GSI_DEBUG_SW_STALL_ADDR,v)
#define HWIO_GSI_DEBUG_SW_STALL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_DEBUG_SW_STALL_ADDR,m,v,HWIO_GSI_DEBUG_SW_STALL_IN)
#define HWIO_GSI_DEBUG_SW_STALL_MCS_STALL_BMSK                                                           0xff
#define HWIO_GSI_DEBUG_SW_STALL_MCS_STALL_SHFT                                                            0x0

#define HWIO_GSI_DEBUG_PC_FOR_DEBUG_ADDR                                                           (GSI_TOP_GSI_REG_BASE      + 0x00005048)
#define HWIO_GSI_DEBUG_PC_FOR_DEBUG_PHYS                                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005048)
#define HWIO_GSI_DEBUG_PC_FOR_DEBUG_OFFS                                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005048)
#define HWIO_GSI_DEBUG_PC_FOR_DEBUG_RMSK                                                               0x1fff
#define HWIO_GSI_DEBUG_PC_FOR_DEBUG_ATTR                                                                  0x1
#define HWIO_GSI_DEBUG_PC_FOR_DEBUG_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_PC_FOR_DEBUG_ADDR, HWIO_GSI_DEBUG_PC_FOR_DEBUG_RMSK)
#define HWIO_GSI_DEBUG_PC_FOR_DEBUG_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_PC_FOR_DEBUG_ADDR, m)
#define HWIO_GSI_DEBUG_PC_FOR_DEBUG_IRAM_PTR_BMSK                                                      0x1fff
#define HWIO_GSI_DEBUG_PC_FOR_DEBUG_IRAM_PTR_SHFT                                                         0x0

#define HWIO_GSI_DEBUG_PTR_REG_ADDR                                                                (GSI_TOP_GSI_REG_BASE      + 0x0000504c)
#define HWIO_GSI_DEBUG_PTR_REG_PHYS                                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000504c)
#define HWIO_GSI_DEBUG_PTR_REG_OFFS                                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000504c)
#define HWIO_GSI_DEBUG_PTR_REG_RMSK                                                                0xffffffff
#define HWIO_GSI_DEBUG_PTR_REG_ATTR                                                                       0x1
#define HWIO_GSI_DEBUG_PTR_REG_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_PTR_REG_ADDR, HWIO_GSI_DEBUG_PTR_REG_RMSK)
#define HWIO_GSI_DEBUG_PTR_REG_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_PTR_REG_ADDR, m)
#define HWIO_GSI_DEBUG_PTR_REG_INT_ENG_1_BMSK                                                      0xff000000
#define HWIO_GSI_DEBUG_PTR_REG_INT_ENG_1_SHFT                                                            0x18
#define HWIO_GSI_DEBUG_PTR_REG_INT_ENG_0_BMSK                                                        0xff0000
#define HWIO_GSI_DEBUG_PTR_REG_INT_ENG_0_SHFT                                                            0x10
#define HWIO_GSI_DEBUG_PTR_REG_WR_ENG_BMSK                                                             0xff00
#define HWIO_GSI_DEBUG_PTR_REG_WR_ENG_SHFT                                                                0x8
#define HWIO_GSI_DEBUG_PTR_REG_EV_ENG_BMSK                                                               0xff
#define HWIO_GSI_DEBUG_PTR_REG_EV_ENG_SHFT                                                                0x0

#define HWIO_GSI_DEBUG_QSB_LOG_SEL_ADDR                                                            (GSI_TOP_GSI_REG_BASE      + 0x00005050)
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_PHYS                                                            (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005050)
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_OFFS                                                            (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005050)
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_RMSK                                                              0xffff01
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_ATTR                                                                   0x3
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_QSB_LOG_SEL_ADDR, HWIO_GSI_DEBUG_QSB_LOG_SEL_RMSK)
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_QSB_LOG_SEL_ADDR, m)
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_OUT(v)      \
        out_dword(HWIO_GSI_DEBUG_QSB_LOG_SEL_ADDR,v)
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_DEBUG_QSB_LOG_SEL_ADDR,m,v,HWIO_GSI_DEBUG_QSB_LOG_SEL_IN)
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_SEL_MID_BMSK                                                      0xff0000
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_SEL_MID_SHFT                                                          0x10
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_SEL_TID_BMSK                                                        0xff00
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_SEL_TID_SHFT                                                           0x8
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_SEL_WRITE_BMSK                                                         0x1
#define HWIO_GSI_DEBUG_QSB_LOG_SEL_SEL_WRITE_SHFT                                                         0x0

#define HWIO_GSI_DEBUG_MCS_ADDR                                                                    (GSI_TOP_GSI_REG_BASE      + 0x00005054)
#define HWIO_GSI_DEBUG_MCS_PHYS                                                                    (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005054)
#define HWIO_GSI_DEBUG_MCS_OFFS                                                                    (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005054)
#define HWIO_GSI_DEBUG_MCS_RMSK                                                                           0x1
#define HWIO_GSI_DEBUG_MCS_ATTR                                                                           0x2
#define HWIO_GSI_DEBUG_MCS_OUT(v)      \
        out_dword(HWIO_GSI_DEBUG_MCS_ADDR,v)
#define HWIO_GSI_DEBUG_MCS_HALT_OTHER_MCS_CLR_BMSK                                                        0x1
#define HWIO_GSI_DEBUG_MCS_HALT_OTHER_MCS_CLR_SHFT                                                        0x0

#define HWIO_GSI_DEBUG_QSB_LOG_CLR_ADDR                                                            (GSI_TOP_GSI_REG_BASE      + 0x00005058)
#define HWIO_GSI_DEBUG_QSB_LOG_CLR_PHYS                                                            (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005058)
#define HWIO_GSI_DEBUG_QSB_LOG_CLR_OFFS                                                            (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005058)
#define HWIO_GSI_DEBUG_QSB_LOG_CLR_RMSK                                                                   0x1
#define HWIO_GSI_DEBUG_QSB_LOG_CLR_ATTR                                                                   0x2
#define HWIO_GSI_DEBUG_QSB_LOG_CLR_OUT(v)      \
        out_dword(HWIO_GSI_DEBUG_QSB_LOG_CLR_ADDR,v)
#define HWIO_GSI_DEBUG_QSB_LOG_CLR_LOG_CLR_BMSK                                                           0x1
#define HWIO_GSI_DEBUG_QSB_LOG_CLR_LOG_CLR_SHFT                                                           0x0

#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_ADDR                                                    (GSI_TOP_GSI_REG_BASE      + 0x00005060)
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_PHYS                                                    (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005060)
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_OFFS                                                    (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005060)
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_RMSK                                                     0x1ffff01
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_ATTR                                                           0x1
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_ADDR, HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_RMSK)
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_ADDR, m)
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_ERR_SAVED_BMSK                                           0x1000000
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_ERR_SAVED_SHFT                                                0x18
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_ERR_MID_BMSK                                              0xff0000
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_ERR_MID_SHFT                                                  0x10
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_ERR_TID_BMSK                                                0xff00
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_ERR_TID_SHFT                                                   0x8
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_ERR_WRITE_BMSK                                                 0x1
#define HWIO_GSI_DEBUG_QSB_LOG_ERR_TRNS_ID_ERR_WRITE_SHFT                                                 0x0

#define HWIO_GSI_DEBUG_QSB_LOG_0_ADDR                                                              (GSI_TOP_GSI_REG_BASE      + 0x00005064)
#define HWIO_GSI_DEBUG_QSB_LOG_0_PHYS                                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005064)
#define HWIO_GSI_DEBUG_QSB_LOG_0_OFFS                                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005064)
#define HWIO_GSI_DEBUG_QSB_LOG_0_RMSK                                                              0xffffffff
#define HWIO_GSI_DEBUG_QSB_LOG_0_ATTR                                                                     0x1
#define HWIO_GSI_DEBUG_QSB_LOG_0_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_QSB_LOG_0_ADDR, HWIO_GSI_DEBUG_QSB_LOG_0_RMSK)
#define HWIO_GSI_DEBUG_QSB_LOG_0_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_QSB_LOG_0_ADDR, m)
#define HWIO_GSI_DEBUG_QSB_LOG_0_ADDR_31_0_BMSK                                                    0xffffffff
#define HWIO_GSI_DEBUG_QSB_LOG_0_ADDR_31_0_SHFT                                                           0x0

#define HWIO_GSI_DEBUG_QSB_LOG_1_ADDR                                                              (GSI_TOP_GSI_REG_BASE      + 0x00005068)
#define HWIO_GSI_DEBUG_QSB_LOG_1_PHYS                                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005068)
#define HWIO_GSI_DEBUG_QSB_LOG_1_OFFS                                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005068)
#define HWIO_GSI_DEBUG_QSB_LOG_1_RMSK                                                              0xfff7ffff
#define HWIO_GSI_DEBUG_QSB_LOG_1_ATTR                                                                     0x1
#define HWIO_GSI_DEBUG_QSB_LOG_1_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_QSB_LOG_1_ADDR, HWIO_GSI_DEBUG_QSB_LOG_1_RMSK)
#define HWIO_GSI_DEBUG_QSB_LOG_1_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_QSB_LOG_1_ADDR, m)
#define HWIO_GSI_DEBUG_QSB_LOG_1_AREQPRIORITY_BMSK                                                 0xf0000000
#define HWIO_GSI_DEBUG_QSB_LOG_1_AREQPRIORITY_SHFT                                                       0x1c
#define HWIO_GSI_DEBUG_QSB_LOG_1_ASIZE_BMSK                                                         0xf000000
#define HWIO_GSI_DEBUG_QSB_LOG_1_ASIZE_SHFT                                                              0x18
#define HWIO_GSI_DEBUG_QSB_LOG_1_ALEN_BMSK                                                           0xf00000
#define HWIO_GSI_DEBUG_QSB_LOG_1_ALEN_SHFT                                                               0x14
#define HWIO_GSI_DEBUG_QSB_LOG_1_AOOOWR_BMSK                                                          0x40000
#define HWIO_GSI_DEBUG_QSB_LOG_1_AOOOWR_SHFT                                                             0x12
#define HWIO_GSI_DEBUG_QSB_LOG_1_AOOORD_BMSK                                                          0x20000
#define HWIO_GSI_DEBUG_QSB_LOG_1_AOOORD_SHFT                                                             0x11
#define HWIO_GSI_DEBUG_QSB_LOG_1_ATRANSIENT_BMSK                                                      0x10000
#define HWIO_GSI_DEBUG_QSB_LOG_1_ATRANSIENT_SHFT                                                         0x10
#define HWIO_GSI_DEBUG_QSB_LOG_1_ACACHEABLE_BMSK                                                       0x8000
#define HWIO_GSI_DEBUG_QSB_LOG_1_ACACHEABLE_SHFT                                                          0xf
#define HWIO_GSI_DEBUG_QSB_LOG_1_ASHARED_BMSK                                                          0x4000
#define HWIO_GSI_DEBUG_QSB_LOG_1_ASHARED_SHFT                                                             0xe
#define HWIO_GSI_DEBUG_QSB_LOG_1_ANOALLOCATE_BMSK                                                      0x2000
#define HWIO_GSI_DEBUG_QSB_LOG_1_ANOALLOCATE_SHFT                                                         0xd
#define HWIO_GSI_DEBUG_QSB_LOG_1_AINNERSHARED_BMSK                                                     0x1000
#define HWIO_GSI_DEBUG_QSB_LOG_1_AINNERSHARED_SHFT                                                        0xc
#define HWIO_GSI_DEBUG_QSB_LOG_1_ADDR_43_32_BMSK                                                        0xfff
#define HWIO_GSI_DEBUG_QSB_LOG_1_ADDR_43_32_SHFT                                                          0x0

#define HWIO_GSI_DEBUG_QSB_LOG_2_ADDR                                                              (GSI_TOP_GSI_REG_BASE      + 0x0000506c)
#define HWIO_GSI_DEBUG_QSB_LOG_2_PHYS                                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000506c)
#define HWIO_GSI_DEBUG_QSB_LOG_2_OFFS                                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000506c)
#define HWIO_GSI_DEBUG_QSB_LOG_2_RMSK                                                                  0xffff
#define HWIO_GSI_DEBUG_QSB_LOG_2_ATTR                                                                     0x1
#define HWIO_GSI_DEBUG_QSB_LOG_2_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_QSB_LOG_2_ADDR, HWIO_GSI_DEBUG_QSB_LOG_2_RMSK)
#define HWIO_GSI_DEBUG_QSB_LOG_2_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_QSB_LOG_2_ADDR, m)
#define HWIO_GSI_DEBUG_QSB_LOG_2_AMEMTYPE_BMSK                                                         0xf000
#define HWIO_GSI_DEBUG_QSB_LOG_2_AMEMTYPE_SHFT                                                            0xc
#define HWIO_GSI_DEBUG_QSB_LOG_2_AMMUSID_BMSK                                                           0xfff
#define HWIO_GSI_DEBUG_QSB_LOG_2_AMMUSID_SHFT                                                             0x0

#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_ADDR(n)                                               (GSI_TOP_GSI_REG_BASE      + 0x00005070 + 0x4 * (n))
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_PHYS(n)                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005070 + 0x4 * (n))
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_OFFS(n)                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005070 + 0x4 * (n))
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_RMSK                                                  0xffffffff
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_MAXn                                                           3
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_ATTR                                                         0x1
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_INI(n)        \
        in_dword_masked(HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_ADDR(n), HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_RMSK)
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_ADDR(n), mask)
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_MID_BMSK                                              0xf8000000
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_MID_SHFT                                                    0x1b
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_TID_BMSK                                               0x7c00000
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_TID_SHFT                                                    0x16
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_WRITE_BMSK                                              0x200000
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_WRITE_SHFT                                                  0x15
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_ADDR_20_0_BMSK                                          0x1fffff
#define HWIO_GSI_DEBUG_QSB_LOG_LAST_MISC_IDn_ADDR_20_0_SHFT                                               0x0

#define HWIO_GSI_DEBUG_SW_RF_n_WRITE_ADDR(n)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00005080 + 0x4 * (n))
#define HWIO_GSI_DEBUG_SW_RF_n_WRITE_PHYS(n)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005080 + 0x4 * (n))
#define HWIO_GSI_DEBUG_SW_RF_n_WRITE_OFFS(n)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005080 + 0x4 * (n))
#define HWIO_GSI_DEBUG_SW_RF_n_WRITE_RMSK                                                          0xffffffff
#define HWIO_GSI_DEBUG_SW_RF_n_WRITE_MAXn                                                                  29
#define HWIO_GSI_DEBUG_SW_RF_n_WRITE_ATTR                                                                 0x2
#define HWIO_GSI_DEBUG_SW_RF_n_WRITE_OUTI(n,val)    \
        out_dword(HWIO_GSI_DEBUG_SW_RF_n_WRITE_ADDR(n),val)
#define HWIO_GSI_DEBUG_SW_RF_n_WRITE_DATA_IN_BMSK                                                  0xffffffff
#define HWIO_GSI_DEBUG_SW_RF_n_WRITE_DATA_IN_SHFT                                                         0x0

#define HWIO_GSI_DEBUG_SW_RF_n_READ_ADDR(n)                                                        (GSI_TOP_GSI_REG_BASE      + 0x00005100 + 0x4 * (n))
#define HWIO_GSI_DEBUG_SW_RF_n_READ_PHYS(n)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005100 + 0x4 * (n))
#define HWIO_GSI_DEBUG_SW_RF_n_READ_OFFS(n)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005100 + 0x4 * (n))
#define HWIO_GSI_DEBUG_SW_RF_n_READ_RMSK                                                           0xffffffff
#define HWIO_GSI_DEBUG_SW_RF_n_READ_MAXn                                                                   29
#define HWIO_GSI_DEBUG_SW_RF_n_READ_ATTR                                                                  0x1
#define HWIO_GSI_DEBUG_SW_RF_n_READ_INI(n)        \
        in_dword_masked(HWIO_GSI_DEBUG_SW_RF_n_READ_ADDR(n), HWIO_GSI_DEBUG_SW_RF_n_READ_RMSK, HWIO_GSI_DEBUG_SW_RF_n_READ_ATTR)
#define HWIO_GSI_DEBUG_SW_RF_n_READ_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_SW_RF_n_READ_ADDR(n), mask)
#define HWIO_GSI_DEBUG_SW_RF_n_READ_RF_REG_BMSK                                                    0xffffffff
#define HWIO_GSI_DEBUG_SW_RF_n_READ_RF_REG_SHFT                                                           0x0

#define HWIO_GSI_DEBUG_COUNTER_CFGn_ADDR(n)                                                        (GSI_TOP_GSI_REG_BASE      + 0x00005180 + 0x4 * (n))
#define HWIO_GSI_DEBUG_COUNTER_CFGn_PHYS(n)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005180 + 0x4 * (n))
#define HWIO_GSI_DEBUG_COUNTER_CFGn_OFFS(n)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005180 + 0x4 * (n))
#define HWIO_GSI_DEBUG_COUNTER_CFGn_RMSK                                                           0x9fffffff
#define HWIO_GSI_DEBUG_COUNTER_CFGn_MAXn                                                                    7
#define HWIO_GSI_DEBUG_COUNTER_CFGn_ATTR                                                                  0x3
#define HWIO_GSI_DEBUG_COUNTER_CFGn_INI(n)        \
        in_dword_masked(HWIO_GSI_DEBUG_COUNTER_CFGn_ADDR(n), HWIO_GSI_DEBUG_COUNTER_CFGn_RMSK)
#define HWIO_GSI_DEBUG_COUNTER_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_COUNTER_CFGn_ADDR(n), mask)
#define HWIO_GSI_DEBUG_COUNTER_CFGn_OUTI(n,val)    \
        out_dword(HWIO_GSI_DEBUG_COUNTER_CFGn_ADDR(n),val)
#define HWIO_GSI_DEBUG_COUNTER_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_GSI_DEBUG_COUNTER_CFGn_ADDR(n),mask,val,HWIO_GSI_DEBUG_COUNTER_CFGn_INI(n))
#define HWIO_GSI_DEBUG_COUNTER_CFGn_CHAIN_BMSK                                                     0x80000000
#define HWIO_GSI_DEBUG_COUNTER_CFGn_CHAIN_SHFT                                                           0x1f
#define HWIO_GSI_DEBUG_COUNTER_CFGn_MCS_SEL_BMSK                                                   0x1fe00000
#define HWIO_GSI_DEBUG_COUNTER_CFGn_MCS_SEL_SHFT                                                         0x15
#define HWIO_GSI_DEBUG_COUNTER_CFGn_PC_MSB_BMSK                                                      0x100000
#define HWIO_GSI_DEBUG_COUNTER_CFGn_PC_MSB_SHFT                                                          0x14
#define HWIO_GSI_DEBUG_COUNTER_CFGn_VIRTUAL_CHNL_BMSK                                                 0xff000
#define HWIO_GSI_DEBUG_COUNTER_CFGn_VIRTUAL_CHNL_SHFT                                                     0xc
#define HWIO_GSI_DEBUG_COUNTER_CFGn_EE_BMSK                                                             0xf00
#define HWIO_GSI_DEBUG_COUNTER_CFGn_EE_SHFT                                                               0x8
#define HWIO_GSI_DEBUG_COUNTER_CFGn_EVNT_TYPE_BMSK                                                       0xf8
#define HWIO_GSI_DEBUG_COUNTER_CFGn_EVNT_TYPE_SHFT                                                        0x3
#define HWIO_GSI_DEBUG_COUNTER_CFGn_CLR_AT_READ_BMSK                                                      0x4
#define HWIO_GSI_DEBUG_COUNTER_CFGn_CLR_AT_READ_SHFT                                                      0x2
#define HWIO_GSI_DEBUG_COUNTER_CFGn_STOP_AT_WRAP_ARND_BMSK                                                0x2
#define HWIO_GSI_DEBUG_COUNTER_CFGn_STOP_AT_WRAP_ARND_SHFT                                                0x1
#define HWIO_GSI_DEBUG_COUNTER_CFGn_ENABLE_BMSK                                                           0x1
#define HWIO_GSI_DEBUG_COUNTER_CFGn_ENABLE_SHFT                                                           0x0

#define HWIO_GSI_DEBUG_COUNTERn_ADDR(n)                                                            (GSI_TOP_GSI_REG_BASE      + 0x000051a0 + 0x4 * (n))
#define HWIO_GSI_DEBUG_COUNTERn_PHYS(n)                                                            (GSI_TOP_GSI_REG_BASE_PHYS + 0x000051a0 + 0x4 * (n))
#define HWIO_GSI_DEBUG_COUNTERn_OFFS(n)                                                            (GSI_TOP_GSI_REG_BASE_OFFS + 0x000051a0 + 0x4 * (n))
#define HWIO_GSI_DEBUG_COUNTERn_RMSK                                                                   0xffff
#define HWIO_GSI_DEBUG_COUNTERn_MAXn                                                                        7
#define HWIO_GSI_DEBUG_COUNTERn_ATTR                                                                      0x1
#define HWIO_GSI_DEBUG_COUNTERn_INI(n)        \
        in_dword_masked(HWIO_GSI_DEBUG_COUNTERn_ADDR(n), HWIO_GSI_DEBUG_COUNTERn_RMSK, HWIO_GSI_DEBUG_COUNTERn_ATTR)
#define HWIO_GSI_DEBUG_COUNTERn_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_COUNTERn_ADDR(n), mask)
#define HWIO_GSI_DEBUG_COUNTERn_COUNTER_VALUE_BMSK                                                     0xffff
#define HWIO_GSI_DEBUG_COUNTERn_COUNTER_VALUE_SHFT                                                        0x0

#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_WR_ADDR(n,k)                                             (GSI_TOP_GSI_REG_BASE      + 0x000051c0 + 0x4 * (n) + 0x28 * (k))
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_WR_PHYS(n,k)                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x000051c0 + 0x4 * (n) + 0x28 * (k))
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_WR_OFFS(n,k)                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x000051c0 + 0x4 * (n) + 0x28 * (k))
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_WR_RMSK                                                  0xffffffff
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_WR_MAXn                                                           9
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_WR_MAXk                                                           2
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_WR_ATTR                                                         0x2
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_WR_OUTI2(n,k,val)    \
        out_dword(HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_WR_ADDR(n,k),val)
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_WR_DATA_IN_BMSK                                          0xffffffff
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_WR_DATA_IN_SHFT                                                 0x0

#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_ADDR(n,k)                                             (GSI_TOP_GSI_REG_BASE      + 0x000052e0 + 0x4 * (n) + 0x28 * (k))
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_PHYS(n,k)                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x000052e0 + 0x4 * (n) + 0x28 * (k))
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_OFFS(n,k)                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x000052e0 + 0x4 * (n) + 0x28 * (k))
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_RMSK                                                  0xffffffff
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_MAXn                                                           9
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_MAXk                                                           2
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_ATTR                                                         0x1
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_INI2(n,k)        \
        in_dword_masked(HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_ADDR(n,k), HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_RMSK)
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_ADDR(n,k), mask)
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_MSK_REG_BMSK                                          0xffffffff
#define HWIO_GSI_DEBUG_SW_MSK_REG_n_SEC_k_RD_MSK_REG_SHFT                                                 0x0

#define HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_ADDR(n,k)                                                (GSI_TOP_GSI_REG_BASE      + 0x00005400 + 0x104 * (n) + 0x4 * (k))
#define HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_PHYS(n,k)                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005400 + 0x104 * (n) + 0x4 * (k))
#define HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_OFFS(n,k)                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005400 + 0x104 * (n) + 0x4 * (k))
#define HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_RMSK                                                          0x1ff
#define HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_MAXn                                                              4
#define HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_MAXk                                                             64
#define HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_ATTR                                                            0x1
#define HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_INI2(n,k)        \
        in_dword_masked(HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_ADDR(n,k), HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_RMSK)
#define HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_ADDR(n,k), mask)
#define HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_VALID_BMSK                                                    0x100
#define HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_VALID_SHFT                                                      0x8
#define HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_PHY_CH_BMSK                                                    0xff
#define HWIO_GSI_DEBUG_EE_n_CH_k_VP_TABLE_PHY_CH_SHFT                                                     0x0

#define HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_ADDR(n,k)                                                (GSI_TOP_GSI_REG_BASE      + 0x00005a00 + 0x100 * (n) + 0x4 * (k))
#define HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_PHYS(n,k)                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005a00 + 0x100 * (n) + 0x4 * (k))
#define HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_OFFS(n,k)                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005a00 + 0x100 * (n) + 0x4 * (k))
#define HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_RMSK                                                          0x1ff
#define HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_MAXn                                                              4
#define HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_MAXk                                                             61
#define HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_ATTR                                                            0x1
#define HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_INI2(n,k)        \
        in_dword_masked(HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_ADDR(n,k), HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_RMSK)
#define HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_ADDR(n,k), mask)
#define HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_VALID_BMSK                                                    0x100
#define HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_VALID_SHFT                                                      0x8
#define HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_PHY_EV_CH_BMSK                                                 0xff
#define HWIO_GSI_DEBUG_EE_n_EV_k_VP_TABLE_PHY_EV_CH_SHFT                                                  0x0

#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_ADDR(n)                                                     (GSI_TOP_GSI_REG_BASE      + 0x00005f00 + 0x4 * (n))
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_PHYS(n)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005f00 + 0x4 * (n))
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_OFFS(n)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005f00 + 0x4 * (n))
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_RMSK                                                        0xffffffff
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_MAXn                                                                 5
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_ATTR                                                               0x1
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_INI(n)        \
        in_dword_masked(HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_ADDR(n), HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_RMSK)
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_ADDR(n), mask)
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_USER_DATA_LSB_BMSK                                          0xfffff800
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_USER_DATA_LSB_SHFT                                                 0xb
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_TID_CNT_BMSK                                                     0x780
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_TID_CNT_SHFT                                                       0x7
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_INT_ID_BMSK                                                       0x78
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_INT_ID_SHFT                                                        0x3
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_VLD_BMSK                                                           0x4
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_VLD_SHFT                                                           0x2
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_BLOCKING_BMSK                                                      0x2
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_BLOCKING_SHFT                                                      0x1
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_WR_BMSK                                                            0x1
#define HWIO_GSI_DEBUG_SDMA_TRANS_DB_n_WR_SHFT                                                            0x0

#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_ADDR                                                 (GSI_TOP_GSI_REG_BASE      + 0x00005f54)
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_PHYS                                                 (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005f54)
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_OFFS                                                 (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005f54)
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_RMSK                                                       0xff
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_ATTR                                                        0x3
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_ADDR, HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_RMSK)
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_ADDR, m)
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_OUT(v)      \
        out_dword(HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_ADDR,v)
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_ADDR,m,v,HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_IN)
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_PREFETCH_BUF_CH_ID_BMSK                                    0xff
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_CH_ID_PREFETCH_BUF_CH_ID_SHFT                                     0x0

#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_STATUS_ADDR                                                (GSI_TOP_GSI_REG_BASE      + 0x00005f58)
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_STATUS_PHYS                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005f58)
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_STATUS_OFFS                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005f58)
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_STATUS_RMSK                                                0xffffffff
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_STATUS_ATTR                                                       0x1
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_STATUS_IN          \
        in_dword_masked(HWIO_GSI_DEBUG_REE_PREFETCH_BUF_STATUS_ADDR, HWIO_GSI_DEBUG_REE_PREFETCH_BUF_STATUS_RMSK)
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_STATUS_INM(m)      \
        in_dword_masked(HWIO_GSI_DEBUG_REE_PREFETCH_BUF_STATUS_ADDR, m)
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_STATUS_PREFETCH_BUF_STATUS_BMSK                            0xffffffff
#define HWIO_GSI_DEBUG_REE_PREFETCH_BUF_STATUS_PREFETCH_BUF_STATUS_SHFT                                   0x0

#define HWIO_GSI_MCS_PROFILING_BP_CNT_LSB_ADDR                                                     (GSI_TOP_GSI_REG_BASE      + 0x00005f5c)
#define HWIO_GSI_MCS_PROFILING_BP_CNT_LSB_PHYS                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005f5c)
#define HWIO_GSI_MCS_PROFILING_BP_CNT_LSB_OFFS                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005f5c)
#define HWIO_GSI_MCS_PROFILING_BP_CNT_LSB_RMSK                                                     0xffffffff
#define HWIO_GSI_MCS_PROFILING_BP_CNT_LSB_ATTR                                                            0x1
#define HWIO_GSI_MCS_PROFILING_BP_CNT_LSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_BP_CNT_LSB_ADDR, HWIO_GSI_MCS_PROFILING_BP_CNT_LSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_BP_CNT_LSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_BP_CNT_LSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_BP_CNT_LSB_BP_CNT_LSB_BMSK                                          0xffffffff
#define HWIO_GSI_MCS_PROFILING_BP_CNT_LSB_BP_CNT_LSB_SHFT                                                 0x0

#define HWIO_GSI_MCS_PROFILING_BP_CNT_MSB_ADDR                                                     (GSI_TOP_GSI_REG_BASE      + 0x00005f60)
#define HWIO_GSI_MCS_PROFILING_BP_CNT_MSB_PHYS                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005f60)
#define HWIO_GSI_MCS_PROFILING_BP_CNT_MSB_OFFS                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005f60)
#define HWIO_GSI_MCS_PROFILING_BP_CNT_MSB_RMSK                                                         0xffff
#define HWIO_GSI_MCS_PROFILING_BP_CNT_MSB_ATTR                                                            0x1
#define HWIO_GSI_MCS_PROFILING_BP_CNT_MSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_BP_CNT_MSB_ADDR, HWIO_GSI_MCS_PROFILING_BP_CNT_MSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_BP_CNT_MSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_BP_CNT_MSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_BP_CNT_MSB_BP_CNT_MSB_BMSK                                              0xffff
#define HWIO_GSI_MCS_PROFILING_BP_CNT_MSB_BP_CNT_MSB_SHFT                                                 0x0

#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_LSB_ADDR                                         (GSI_TOP_GSI_REG_BASE      + 0x00005f64)
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_LSB_PHYS                                         (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005f64)
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_LSB_OFFS                                         (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005f64)
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_LSB_RMSK                                         0xffffffff
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_LSB_ATTR                                                0x1
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_LSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_LSB_ADDR, HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_LSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_LSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_LSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_LSB_BP_AND_PENDING_CNT_LSB_BMSK                  0xffffffff
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_LSB_BP_AND_PENDING_CNT_LSB_SHFT                         0x0

#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_MSB_ADDR                                         (GSI_TOP_GSI_REG_BASE      + 0x00005f68)
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_MSB_PHYS                                         (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005f68)
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_MSB_OFFS                                         (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005f68)
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_MSB_RMSK                                             0xffff
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_MSB_ATTR                                                0x1
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_MSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_MSB_ADDR, HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_MSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_MSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_MSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_MSB_BP_AND_PENDING_CNT_MSB_BMSK                      0xffff
#define HWIO_GSI_MCS_PROFILING_BP_AND_PENDING_CNT_MSB_BP_AND_PENDING_CNT_MSB_SHFT                         0x0

#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_LSB_ADDR                                               (GSI_TOP_GSI_REG_BASE      + 0x00005f6c)
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_LSB_PHYS                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005f6c)
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_LSB_OFFS                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005f6c)
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_LSB_RMSK                                               0xffffffff
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_LSB_ATTR                                                      0x1
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_LSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_LSB_ADDR, HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_LSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_LSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_LSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_LSB_MCS_BUSY_CNT_LSB_BMSK                              0xffffffff
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_LSB_MCS_BUSY_CNT_LSB_SHFT                                     0x0

#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_MSB_ADDR                                               (GSI_TOP_GSI_REG_BASE      + 0x00005f70)
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_MSB_PHYS                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005f70)
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_MSB_OFFS                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005f70)
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_MSB_RMSK                                                   0xffff
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_MSB_ATTR                                                      0x1
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_MSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_MSB_ADDR, HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_MSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_MSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_MSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_MSB_MCS_BUSY_CNT_MSB_BMSK                                  0xffff
#define HWIO_GSI_MCS_PROFILING_MCS_BUSY_CNT_MSB_MCS_BUSY_CNT_MSB_SHFT                                     0x0

#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_LSB_ADDR                                               (GSI_TOP_GSI_REG_BASE      + 0x00005f74)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_LSB_PHYS                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005f74)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_LSB_OFFS                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005f74)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_LSB_RMSK                                               0xffffffff
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_LSB_ATTR                                                      0x1
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_LSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_LSB_ADDR, HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_LSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_LSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_LSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_LSB_MCS_IDLE_CNT_LSB_BMSK                              0xffffffff
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_LSB_MCS_IDLE_CNT_LSB_SHFT                                     0x0

#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_MSB_ADDR                                               (GSI_TOP_GSI_REG_BASE      + 0x00005f78)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_MSB_PHYS                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005f78)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_MSB_OFFS                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005f78)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_MSB_RMSK                                                   0xffff
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_MSB_ATTR                                                      0x1
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_MSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_MSB_ADDR, HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_MSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_MSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_MSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_MSB_MCS_IDLE_CNT_MSB_BMSK                                  0xffff
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_CNT_MSB_MCS_IDLE_CNT_MSB_SHFT                                     0x0

#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_LSB_ADDR                                (GSI_TOP_GSI_REG_BASE      + 0x00005018)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_LSB_PHYS                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005018)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_LSB_OFFS                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005018)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_LSB_RMSK                                0xffffffff
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_LSB_ATTR                                       0x1
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_LSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_LSB_ADDR, HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_LSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_LSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_LSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_LSB_CNT_LSB_BMSK                        0xffffffff
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_LSB_CNT_LSB_SHFT                               0x0

#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_MSB_ADDR                                (GSI_TOP_GSI_REG_BASE      + 0x00005f7c)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_MSB_PHYS                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00005f7c)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_MSB_OFFS                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00005f7c)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_MSB_RMSK                                    0xffff
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_MSB_ATTR                                       0x1
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_MSB_IN          \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_MSB_ADDR, HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_MSB_RMSK)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_MSB_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_MSB_ADDR, m)
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_MSB_CNT_MSB_BMSK                            0xffff
#define HWIO_GSI_MCS_PROFILING_MCS_IDLE_BP_AND_PENDING_CNT_MSB_CNT_MSB_SHFT                               0x0

#define HWIO_GSI_MAX_TRE_TLV_n_ADDR(n)                                                             (GSI_TOP_GSI_REG_BASE      + 0x00004500 + 0x4 * (n))
#define HWIO_GSI_MAX_TRE_TLV_n_PHYS(n)                                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004500 + 0x4 * (n))
#define HWIO_GSI_MAX_TRE_TLV_n_OFFS(n)                                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004500 + 0x4 * (n))
#define HWIO_GSI_MAX_TRE_TLV_n_RMSK                                                                0xffffffff
#define HWIO_GSI_MAX_TRE_TLV_n_MAXn                                                                         9
#define HWIO_GSI_MAX_TRE_TLV_n_ATTR                                                                       0x3
#define HWIO_GSI_MAX_TRE_TLV_n_INI(n)        \
        in_dword_masked(HWIO_GSI_MAX_TRE_TLV_n_ADDR(n), HWIO_GSI_MAX_TRE_TLV_n_RMSK)
#define HWIO_GSI_MAX_TRE_TLV_n_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_MAX_TRE_TLV_n_ADDR(n), mask)
#define HWIO_GSI_MAX_TRE_TLV_n_OUTI(n,val)    \
        out_dword(HWIO_GSI_MAX_TRE_TLV_n_ADDR(n),val)
#define HWIO_GSI_MAX_TRE_TLV_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_GSI_MAX_TRE_TLV_n_ADDR(n),mask,val,HWIO_GSI_MAX_TRE_TLV_n_INI(n))
#define HWIO_GSI_MAX_TRE_TLV_n_VAL_BMSK                                                            0xffffffff
#define HWIO_GSI_MAX_TRE_TLV_n_VAL_SHFT                                                                   0x0

#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_ADDR(n)                                             (GSI_TOP_GSI_REG_BASE      + 0x00004600 + 0x8 * (n))
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_PHYS(n)                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004600 + 0x8 * (n))
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_OFFS(n)                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004600 + 0x8 * (n))
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_RMSK                                                0xffffffff
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_MAXn                                                         9
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_ATTR                                                       0x3
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_INI(n)        \
        in_dword_masked(HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_ADDR(n), HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_RMSK)
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_ADDR(n), mask)
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_OUTI(n,val)    \
        out_dword(HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_ADDR(n),val)
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_ADDR(n),mask,val,HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_INI(n))
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_VALID_MCS_FOR_IRQ3_BMSK                             0xff000000
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_VALID_MCS_FOR_IRQ3_SHFT                                   0x18
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_VALID_MCS_FOR_IRQ2_BMSK                               0xff0000
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_VALID_MCS_FOR_IRQ2_SHFT                                   0x10
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_VALID_MCS_FOR_IRQ1_BMSK                                 0xff00
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_VALID_MCS_FOR_IRQ1_SHFT                                    0x8
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_VALID_MCS_FOR_IRQ0_BMSK                                   0xff
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE0_VALID_MCS_FOR_IRQ0_SHFT                                    0x0

#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_ADDR(n)                                             (GSI_TOP_GSI_REG_BASE      + 0x00004604 + 0x8 * (n))
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_PHYS(n)                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004604 + 0x8 * (n))
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_OFFS(n)                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004604 + 0x8 * (n))
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_RMSK                                                0xffffffff
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_MAXn                                                         9
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_ATTR                                                       0x3
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_INI(n)        \
        in_dword_masked(HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_ADDR(n), HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_RMSK)
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_INMI(n,mask)    \
        in_dword_masked(HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_ADDR(n), mask)
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_OUTI(n,val)    \
        out_dword(HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_ADDR(n),val)
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_ADDR(n),mask,val,HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_INI(n))
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_VALID_MCS_FOR_IRQ7_BMSK                             0xff000000
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_VALID_MCS_FOR_IRQ7_SHFT                                   0x18
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_VALID_MCS_FOR_IRQ6_BMSK                               0xff0000
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_VALID_MCS_FOR_IRQ6_SHFT                                   0x10
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_VALID_MCS_FOR_IRQ5_BMSK                                 0xff00
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_VALID_MCS_FOR_IRQ5_SHFT                                    0x8
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_VALID_MCS_FOR_IRQ4_BMSK                                   0xff
#define HWIO_GSI_IRQ_2_MCS_MAPPING_ACCn_TABLE1_VALID_MCS_FOR_IRQ4_SHFT                                    0x0

#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_ADDR                                                       (GSI_TOP_GSI_REG_BASE      + 0x00004680)
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_PHYS                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004680)
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_OFFS                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004680)
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_RMSK                                                        0x3ff03ff
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_ATTR                                                              0x1
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_IN          \
        in_dword_masked(HWIO_GSI_CURRENT_CH_PER_MCS_0_1_ADDR, HWIO_GSI_CURRENT_CH_PER_MCS_0_1_RMSK)
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_INM(m)      \
        in_dword_masked(HWIO_GSI_CURRENT_CH_PER_MCS_0_1_ADDR, m)
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_MCS_1_BUSY_BMSK                                             0x2000000
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_MCS_1_BUSY_SHFT                                                  0x19
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_CHID_VALID_MCS_1_BMSK                                       0x1000000
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_CHID_VALID_MCS_1_SHFT                                            0x18
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_CHID_MCS_1_BMSK                                              0xff0000
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_CHID_MCS_1_SHFT                                                  0x10
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_MCS_0_BUSY_BMSK                                                 0x200
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_MCS_0_BUSY_SHFT                                                   0x9
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_CHID_VALID_MCS_0_BMSK                                           0x100
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_CHID_VALID_MCS_0_SHFT                                             0x8
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_CHID_MCS_0_BMSK                                                  0xff
#define HWIO_GSI_CURRENT_CH_PER_MCS_0_1_CHID_MCS_0_SHFT                                                   0x0

#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_ADDR                                                       (GSI_TOP_GSI_REG_BASE      + 0x00004684)
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_PHYS                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004684)
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_OFFS                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004684)
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_RMSK                                                        0x3ff03ff
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_ATTR                                                              0x1
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_IN          \
        in_dword_masked(HWIO_GSI_CURRENT_CH_PER_MCS_2_3_ADDR, HWIO_GSI_CURRENT_CH_PER_MCS_2_3_RMSK)
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_INM(m)      \
        in_dword_masked(HWIO_GSI_CURRENT_CH_PER_MCS_2_3_ADDR, m)
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_MCS_3_BUSY_BMSK                                             0x2000000
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_MCS_3_BUSY_SHFT                                                  0x19
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_CHID_VALID_MCS_3_BMSK                                       0x1000000
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_CHID_VALID_MCS_3_SHFT                                            0x18
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_CHID_MCS_3_BMSK                                              0xff0000
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_CHID_MCS_3_SHFT                                                  0x10
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_MCS_2_BUSY_BMSK                                                 0x200
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_MCS_2_BUSY_SHFT                                                   0x9
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_CHID_VALID_MCS_2_BMSK                                           0x100
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_CHID_VALID_MCS_2_SHFT                                             0x8
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_CHID_MCS_2_BMSK                                                  0xff
#define HWIO_GSI_CURRENT_CH_PER_MCS_2_3_CHID_MCS_2_SHFT                                                   0x0

#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_ADDR                                                       (GSI_TOP_GSI_REG_BASE      + 0x00004688)
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_PHYS                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004688)
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_OFFS                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004688)
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_RMSK                                                        0x3ff03ff
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_ATTR                                                              0x1
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_IN          \
        in_dword_masked(HWIO_GSI_CURRENT_CH_PER_MCS_4_5_ADDR, HWIO_GSI_CURRENT_CH_PER_MCS_4_5_RMSK)
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_INM(m)      \
        in_dword_masked(HWIO_GSI_CURRENT_CH_PER_MCS_4_5_ADDR, m)
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_MCS_5_BUSY_BMSK                                             0x2000000
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_MCS_5_BUSY_SHFT                                                  0x19
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_CHID_VALID_MCS_5_BMSK                                       0x1000000
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_CHID_VALID_MCS_5_SHFT                                            0x18
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_CHID_MCS_5_BMSK                                              0xff0000
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_CHID_MCS_5_SHFT                                                  0x10
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_MCS_4_BUSY_BMSK                                                 0x200
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_MCS_4_BUSY_SHFT                                                   0x9
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_CHID_VALID_MCS_4_BMSK                                           0x100
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_CHID_VALID_MCS_4_SHFT                                             0x8
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_CHID_MCS_4_BMSK                                                  0xff
#define HWIO_GSI_CURRENT_CH_PER_MCS_4_5_CHID_MCS_4_SHFT                                                   0x0

#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_ADDR                                                       (GSI_TOP_GSI_REG_BASE      + 0x0000468c)
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_PHYS                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000468c)
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_OFFS                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000468c)
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_RMSK                                                        0x3ff03ff
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_ATTR                                                              0x1
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_IN          \
        in_dword_masked(HWIO_GSI_CURRENT_CH_PER_MCS_6_7_ADDR, HWIO_GSI_CURRENT_CH_PER_MCS_6_7_RMSK)
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_INM(m)      \
        in_dword_masked(HWIO_GSI_CURRENT_CH_PER_MCS_6_7_ADDR, m)
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_MCS_7_BUSY_BMSK                                             0x2000000
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_MCS_7_BUSY_SHFT                                                  0x19
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_CHID_VALID_MCS_7_BMSK                                       0x1000000
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_CHID_VALID_MCS_7_SHFT                                            0x18
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_CHID_MCS_7_BMSK                                              0xff0000
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_CHID_MCS_7_SHFT                                                  0x10
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_MCS_6_BUSY_BMSK                                                 0x200
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_MCS_6_BUSY_SHFT                                                   0x9
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_CHID_VALID_MCS_6_BMSK                                           0x100
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_CHID_VALID_MCS_6_SHFT                                             0x8
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_CHID_MCS_6_BMSK                                                  0xff
#define HWIO_GSI_CURRENT_CH_PER_MCS_6_7_CHID_MCS_6_SHFT                                                   0x0

#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_ADDR                                                          (GSI_TOP_GSI_REG_BASE      + 0x00004690)
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_PHYS                                                          (GSI_TOP_GSI_REG_BASE_PHYS + 0x00004690)
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_OFFS                                                          (GSI_TOP_GSI_REG_BASE_OFFS + 0x00004690)
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_RMSK                                                             0x3ffff
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_ATTR                                                                 0x3
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_IN          \
        in_dword_masked(HWIO_GSI_IC_LOCK_BY_ORDER_EN_ADDR, HWIO_GSI_IC_LOCK_BY_ORDER_EN_RMSK)
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_INM(m)      \
        in_dword_masked(HWIO_GSI_IC_LOCK_BY_ORDER_EN_ADDR, m)
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_OUT(v)      \
        out_dword(HWIO_GSI_IC_LOCK_BY_ORDER_EN_ADDR,v)
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_IC_LOCK_BY_ORDER_EN_ADDR,m,v,HWIO_GSI_IC_LOCK_BY_ORDER_EN_IN)
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_IE_INT_MOD_CANCEL_INT_BMSK                                       0x20000
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_IE_INT_MOD_CANCEL_INT_SHFT                                          0x11
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_WR_INT_BMSK                                                      0x10000
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_WR_INT_SHFT                                                         0x10
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_RD_INT_BMSK                                                       0x8000
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_RD_INT_SHFT                                                          0xf
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_DB_NEW_MSI_DB_INT_BMSK                                            0x4000
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_DB_NEW_MSI_DB_INT_SHFT                                               0xe
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_DB_NEW_DB_INT_BMSK                                                0x2000
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_DB_NEW_DB_INT_SHFT                                                   0xd
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_TIMER_ENG_INT_BMSK                                                0x1000
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_TIMER_ENG_INT_SHFT                                                   0xc
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_TLV_CH_NOT_FULL_INT_BMSK                                           0x800
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_TLV_CH_NOT_FULL_INT_SHFT                                             0xb
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_TLV_RST_INT_BMSK                                                   0x400
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_TLV_RST_INT_SHFT                                                     0xa
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_TLV_STOP_INT_BMSK                                                  0x200
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_TLV_STOP_INT_SHFT                                                    0x9
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_TLV_DESC_PROC_COMP_INT_BMSK                                        0x100
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_TLV_DESC_PROC_COMP_INT_SHFT                                          0x8
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_CSR_GENERIC_COM_INT_BMSK                                            0x80
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_CSR_GENERIC_COM_INT_SHFT                                             0x7
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_CSR_CH_EV_COM_INT_BMSK                                              0x40
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_CSR_CH_EV_COM_INT_SHFT                                               0x6
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_IE_NOTIFY_MCS_INT_BMSK                                              0x20
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_IE_NOTIFY_MCS_INT_SHFT                                               0x5
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_IE_INT_MOD_STOP_INT_BMSK                                            0x10
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_IE_INT_MOD_STOP_INT_SHFT                                             0x4
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_EV_ENG_INT_BMSK                                                      0x8
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_EV_ENG_INT_SHFT                                                      0x3
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_REE_CH_EMPTY_INT_BMSK                                                0x4
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_REE_CH_EMPTY_INT_SHFT                                                0x2
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_REE_NEW_RE_INT_BMSK                                                  0x2
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_REE_NEW_RE_INT_SHFT                                                  0x1
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_REE_DIS_CH_COMP_INT_BMSK                                             0x1
#define HWIO_GSI_IC_LOCK_BY_ORDER_EN_REE_DIS_CH_COMP_INT_SHFT                                             0x0

#define HWIO_EE_n_GSI_CH_k_CNTXT_0_ADDR(n,k)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00018000 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_PHYS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018000 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_OFFS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018000 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_RMSK                                                            0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_MAXn                                                                     4
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_MAXk                                                                    64
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_ATTR                                                                   0x3
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_0_ADDR(n,k), HWIO_EE_n_GSI_CH_k_CNTXT_0_RMSK)
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_0_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_CNTXT_0_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_CNTXT_0_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_CNTXT_0_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_ELEMENT_SIZE_BMSK                                               0xff000000
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_ELEMENT_SIZE_SHFT                                                     0x18
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHSTATE_BMSK                                                      0xf00000
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHSTATE_SHFT                                                          0x14
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHSTATE_NOT_ALLOCATED_FVAL                                             0x0
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHSTATE_ALLOCATED_FVAL                                                 0x1
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHSTATE_STARTED_FVAL                                                   0x2
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHSTATE_STOPED_FVAL                                                    0x3
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHSTATE_STOP_IN_PROC_FVAL                                              0x4
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHSTATE_ERROR_FVAL                                                     0xf
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHID_BMSK                                                          0xff000
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHID_SHFT                                                              0xc
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_EE_BMSK                                                              0xf00
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_EE_SHFT                                                                0x8
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHTYPE_DIR_BMSK                                                       0x80
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHTYPE_DIR_SHFT                                                        0x7
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHTYPE_DIR_INBOUND_FVAL                                                0x0
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHTYPE_DIR_OUTBOUND_FVAL                                               0x1
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHTYPE_PROTOCOL_BMSK                                                  0x7f
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHTYPE_PROTOCOL_SHFT                                                   0x0
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHTYPE_PROTOCOL_MHI_FVAL                                               0x0
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHTYPE_PROTOCOL_XHCI_FVAL                                              0x1
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHTYPE_PROTOCOL_GPI_FVAL                                               0x2
#define HWIO_EE_n_GSI_CH_k_CNTXT_0_CHTYPE_PROTOCOL_XDCI_FVAL                                              0x3

#define HWIO_EE_n_GSI_CH_k_CNTXT_1_ADDR(n,k)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00018004 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_PHYS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018004 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_OFFS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018004 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_RMSK                                                            0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_MAXn                                                                     4
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_MAXk                                                                    64
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_ATTR                                                                   0x3
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_1_ADDR(n,k), HWIO_EE_n_GSI_CH_k_CNTXT_1_RMSK)
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_1_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_CNTXT_1_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_CNTXT_1_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_CNTXT_1_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_ERINDEX_BMSK                                                    0xff000000
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_ERINDEX_SHFT                                                          0x18
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_R_LENGTH_BMSK                                                     0xffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_1_R_LENGTH_SHFT                                                          0x0

#define HWIO_EE_n_GSI_CH_k_CNTXT_2_ADDR(n,k)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00018008 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_2_PHYS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018008 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_2_OFFS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018008 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_2_RMSK                                                            0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_2_MAXn                                                                     4
#define HWIO_EE_n_GSI_CH_k_CNTXT_2_MAXk                                                                    64
#define HWIO_EE_n_GSI_CH_k_CNTXT_2_ATTR                                                                   0x3
#define HWIO_EE_n_GSI_CH_k_CNTXT_2_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_2_ADDR(n,k), HWIO_EE_n_GSI_CH_k_CNTXT_2_RMSK)
#define HWIO_EE_n_GSI_CH_k_CNTXT_2_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_2_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_CNTXT_2_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_CNTXT_2_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_CNTXT_2_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_CNTXT_2_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_CNTXT_2_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_2_R_BASE_ADDR_LSBS_BMSK                                           0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_2_R_BASE_ADDR_LSBS_SHFT                                                  0x0

#define HWIO_EE_n_GSI_CH_k_CNTXT_3_ADDR(n,k)                                                       (GSI_TOP_GSI_REG_BASE      + 0x0001800c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_3_PHYS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x0001800c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_3_OFFS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x0001800c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_3_RMSK                                                            0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_3_MAXn                                                                     4
#define HWIO_EE_n_GSI_CH_k_CNTXT_3_MAXk                                                                    64
#define HWIO_EE_n_GSI_CH_k_CNTXT_3_ATTR                                                                   0x3
#define HWIO_EE_n_GSI_CH_k_CNTXT_3_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_3_ADDR(n,k), HWIO_EE_n_GSI_CH_k_CNTXT_3_RMSK)
#define HWIO_EE_n_GSI_CH_k_CNTXT_3_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_3_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_CNTXT_3_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_CNTXT_3_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_CNTXT_3_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_CNTXT_3_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_CNTXT_3_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_3_R_BASE_ADDR_MSBS_BMSK                                           0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_3_R_BASE_ADDR_MSBS_SHFT                                                  0x0

#define HWIO_EE_n_GSI_CH_k_CNTXT_4_ADDR(n,k)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00018010 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_4_PHYS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018010 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_4_OFFS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018010 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_4_RMSK                                                            0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_4_MAXn                                                                     4
#define HWIO_EE_n_GSI_CH_k_CNTXT_4_MAXk                                                                    64
#define HWIO_EE_n_GSI_CH_k_CNTXT_4_ATTR                                                                   0x3
#define HWIO_EE_n_GSI_CH_k_CNTXT_4_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_4_ADDR(n,k), HWIO_EE_n_GSI_CH_k_CNTXT_4_RMSK)
#define HWIO_EE_n_GSI_CH_k_CNTXT_4_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_4_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_CNTXT_4_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_CNTXT_4_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_CNTXT_4_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_CNTXT_4_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_CNTXT_4_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_4_READ_PTR_LSB_BMSK                                               0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_4_READ_PTR_LSB_SHFT                                                      0x0

#define HWIO_EE_n_GSI_CH_k_CNTXT_5_ADDR(n,k)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00018014 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_5_PHYS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018014 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_5_OFFS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018014 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_5_RMSK                                                            0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_5_MAXn                                                                     4
#define HWIO_EE_n_GSI_CH_k_CNTXT_5_MAXk                                                                    64
#define HWIO_EE_n_GSI_CH_k_CNTXT_5_ATTR                                                                   0x1
#define HWIO_EE_n_GSI_CH_k_CNTXT_5_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_5_ADDR(n,k), HWIO_EE_n_GSI_CH_k_CNTXT_5_RMSK)
#define HWIO_EE_n_GSI_CH_k_CNTXT_5_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_5_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_CNTXT_5_READ_PTR_MSB_BMSK                                               0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_5_READ_PTR_MSB_SHFT                                                      0x0

#define HWIO_EE_n_GSI_CH_k_CNTXT_6_ADDR(n,k)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00018018 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_6_PHYS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018018 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_6_OFFS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018018 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_6_RMSK                                                            0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_6_MAXn                                                                     4
#define HWIO_EE_n_GSI_CH_k_CNTXT_6_MAXk                                                                    64
#define HWIO_EE_n_GSI_CH_k_CNTXT_6_ATTR                                                                   0x1
#define HWIO_EE_n_GSI_CH_k_CNTXT_6_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_6_ADDR(n,k), HWIO_EE_n_GSI_CH_k_CNTXT_6_RMSK)
#define HWIO_EE_n_GSI_CH_k_CNTXT_6_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_6_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_CNTXT_6_WRITE_PTR_LSB_BMSK                                              0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_6_WRITE_PTR_LSB_SHFT                                                     0x0

#define HWIO_EE_n_GSI_CH_k_CNTXT_7_ADDR(n,k)                                                       (GSI_TOP_GSI_REG_BASE      + 0x0001801c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_7_PHYS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x0001801c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_7_OFFS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x0001801c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_7_RMSK                                                            0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_7_MAXn                                                                     4
#define HWIO_EE_n_GSI_CH_k_CNTXT_7_MAXk                                                                    64
#define HWIO_EE_n_GSI_CH_k_CNTXT_7_ATTR                                                                   0x1
#define HWIO_EE_n_GSI_CH_k_CNTXT_7_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_7_ADDR(n,k), HWIO_EE_n_GSI_CH_k_CNTXT_7_RMSK)
#define HWIO_EE_n_GSI_CH_k_CNTXT_7_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_7_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_CNTXT_7_WRITE_PTR_MSB_BMSK                                              0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_7_WRITE_PTR_MSB_SHFT                                                     0x0

#define HWIO_EE_n_GSI_CH_k_CNTXT_8_ADDR(n,k)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00018020 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_8_PHYS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018020 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_8_OFFS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018020 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_8_RMSK                                                            0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_8_MAXn                                                                     4
#define HWIO_EE_n_GSI_CH_k_CNTXT_8_MAXk                                                                    64
#define HWIO_EE_n_GSI_CH_k_CNTXT_8_ATTR                                                                   0x3
#define HWIO_EE_n_GSI_CH_k_CNTXT_8_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_8_ADDR(n,k), HWIO_EE_n_GSI_CH_k_CNTXT_8_RMSK)
#define HWIO_EE_n_GSI_CH_k_CNTXT_8_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CNTXT_8_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_CNTXT_8_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_CNTXT_8_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_CNTXT_8_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_CNTXT_8_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_CNTXT_8_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_CNTXT_8_DB_MSI_DATA_BMSK                                                0xffffffff
#define HWIO_EE_n_GSI_CH_k_CNTXT_8_DB_MSI_DATA_SHFT                                                       0x0

#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_ADDR(n,k)                                               (GSI_TOP_GSI_REG_BASE      + 0x00018024 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_PHYS(n,k)                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018024 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_OFFS(n,k)                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018024 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_RMSK                                                           0xf
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_MAXn                                                             4
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_MAXk                                                            64
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_ATTR                                                           0x1
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_ADDR(n,k), HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_RMSK)
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_BMSK                                           0xf
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_SHFT                                           0x0
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_TWO_FVAL                                       0x0
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_THREE_FVAL                                     0x1
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_FOUR_FVAL                                      0x2
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_FIVE_FVAL                                      0x3
#define HWIO_EE_n_GSI_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_SIX_FVAL                                       0x4

#define HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_ADDR(n,k)                                       (GSI_TOP_GSI_REG_BASE      + 0x00018028 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_PHYS(n,k)                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018028 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_OFFS(n,k)                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018028 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_RMSK                                                0xffff
#define HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_MAXn                                                     4
#define HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_MAXk                                                    64
#define HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_ATTR                                                   0x3
#define HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_ADDR(n,k), HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_RMSK)
#define HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_CH_ALMST_EMPTY_THRSHOLD_BMSK                        0xffff
#define HWIO_EE_n_GSI_CH_k_CH_ALMST_EMPTY_THRSHOLD_CH_ALMST_EMPTY_THRSHOLD_SHFT                           0x0

#define HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_ADDR(n,k)                                             (GSI_TOP_GSI_REG_BASE      + 0x00018040 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_PHYS(n,k)                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018040 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_OFFS(n,k)                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018040 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_RMSK                                                    0xffffff
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_MAXn                                                           4
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_MAXk                                                          64
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_ATTR                                                         0x3
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_ADDR(n,k), HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_RMSK)
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_READ_PTR_BMSK                                           0xffffff
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_READ_PTR_READ_PTR_SHFT                                                0x0

#define HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_ADDR(n,k)                                            (GSI_TOP_GSI_REG_BASE      + 0x00018044 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_PHYS(n,k)                                            (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018044 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_OFFS(n,k)                                            (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018044 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_RMSK                                                   0xffffff
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_MAXn                                                          4
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_MAXk                                                         64
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_ATTR                                                        0x3
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_ADDR(n,k), HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_RMSK)
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_RE_INTR_DB_BMSK                                        0xffffff
#define HWIO_EE_n_GSI_CH_k_RE_FETCH_WRITE_PTR_RE_INTR_DB_SHFT                                             0x0

#define HWIO_EE_n_GSI_CH_k_QOS_ADDR(n,k)                                                           (GSI_TOP_GSI_REG_BASE      + 0x00018048 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_QOS_PHYS(n,k)                                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018048 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_QOS_OFFS(n,k)                                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018048 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_QOS_RMSK                                                                 0x3ff3f0f
#define HWIO_EE_n_GSI_CH_k_QOS_MAXn                                                                         4
#define HWIO_EE_n_GSI_CH_k_QOS_MAXk                                                                        64
#define HWIO_EE_n_GSI_CH_k_QOS_ATTR                                                                       0x3
#define HWIO_EE_n_GSI_CH_k_QOS_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_QOS_ADDR(n,k), HWIO_EE_n_GSI_CH_k_QOS_RMSK)
#define HWIO_EE_n_GSI_CH_k_QOS_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_QOS_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_QOS_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_QOS_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_QOS_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_QOS_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_QOS_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_QOS_LOW_LATENCY_EN_BMSK                                                  0x2000000
#define HWIO_EE_n_GSI_CH_k_QOS_LOW_LATENCY_EN_SHFT                                                       0x19
#define HWIO_EE_n_GSI_CH_k_QOS_DB_IN_BYTES_BMSK                                                     0x1000000
#define HWIO_EE_n_GSI_CH_k_QOS_DB_IN_BYTES_SHFT                                                          0x18
#define HWIO_EE_n_GSI_CH_k_QOS_EMPTY_LVL_THRSHOLD_BMSK                                               0xff0000
#define HWIO_EE_n_GSI_CH_k_QOS_EMPTY_LVL_THRSHOLD_SHFT                                                   0x10
#define HWIO_EE_n_GSI_CH_k_QOS_PREFETCH_MODE_BMSK                                                      0x3c00
#define HWIO_EE_n_GSI_CH_k_QOS_PREFETCH_MODE_SHFT                                                         0xa
#define HWIO_EE_n_GSI_CH_k_QOS_PREFETCH_MODE_USE_PREFETCH_BUFS_FVAL                                       0x0
#define HWIO_EE_n_GSI_CH_k_QOS_PREFETCH_MODE_ESCAPE_BUF_ONLY_FVAL                                         0x1
#define HWIO_EE_n_GSI_CH_k_QOS_PREFETCH_MODE_SMART_PRE_FETCH_FVAL                                         0x2
#define HWIO_EE_n_GSI_CH_k_QOS_PREFETCH_MODE_FREE_PRE_FETCH_FVAL                                          0x3
#define HWIO_EE_n_GSI_CH_k_QOS_USE_DB_ENG_BMSK                                                          0x200
#define HWIO_EE_n_GSI_CH_k_QOS_USE_DB_ENG_SHFT                                                            0x9
#define HWIO_EE_n_GSI_CH_k_QOS_MAX_PREFETCH_BMSK                                                        0x100
#define HWIO_EE_n_GSI_CH_k_QOS_MAX_PREFETCH_SHFT                                                          0x8
#define HWIO_EE_n_GSI_CH_k_QOS_MAX_PREFETCH_ONE_PREFETCH_SEG_FVAL                                         0x0
#define HWIO_EE_n_GSI_CH_k_QOS_MAX_PREFETCH_TWO_PREFETCH_SEG_FVAL                                         0x1
#define HWIO_EE_n_GSI_CH_k_QOS_WRR_WEIGHT_BMSK                                                            0xf
#define HWIO_EE_n_GSI_CH_k_QOS_WRR_WEIGHT_SHFT                                                            0x0

#define HWIO_EE_n_GSI_CH_k_SCRATCH_0_ADDR(n,k)                                                     (GSI_TOP_GSI_REG_BASE      + 0x0001804c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_0_PHYS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x0001804c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_0_OFFS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x0001804c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_0_RMSK                                                          0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_0_MAXn                                                                   4
#define HWIO_EE_n_GSI_CH_k_SCRATCH_0_MAXk                                                                  64
#define HWIO_EE_n_GSI_CH_k_SCRATCH_0_ATTR                                                                 0x3
#define HWIO_EE_n_GSI_CH_k_SCRATCH_0_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_0_ADDR(n,k), HWIO_EE_n_GSI_CH_k_SCRATCH_0_RMSK)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_0_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_0_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_0_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_SCRATCH_0_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_0_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_SCRATCH_0_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_SCRATCH_0_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_0_SCRATCH_BMSK                                                  0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_0_SCRATCH_SHFT                                                         0x0

#define HWIO_EE_n_GSI_CH_k_SCRATCH_1_ADDR(n,k)                                                     (GSI_TOP_GSI_REG_BASE      + 0x00018050 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_1_PHYS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018050 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_1_OFFS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018050 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_1_RMSK                                                          0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_1_MAXn                                                                   4
#define HWIO_EE_n_GSI_CH_k_SCRATCH_1_MAXk                                                                  64
#define HWIO_EE_n_GSI_CH_k_SCRATCH_1_ATTR                                                                 0x3
#define HWIO_EE_n_GSI_CH_k_SCRATCH_1_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_1_ADDR(n,k), HWIO_EE_n_GSI_CH_k_SCRATCH_1_RMSK)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_1_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_1_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_1_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_SCRATCH_1_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_1_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_SCRATCH_1_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_SCRATCH_1_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_1_SCRATCH_BMSK                                                  0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_1_SCRATCH_SHFT                                                         0x0

#define HWIO_EE_n_GSI_CH_k_SCRATCH_2_ADDR(n,k)                                                     (GSI_TOP_GSI_REG_BASE      + 0x00018054 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_2_PHYS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018054 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_2_OFFS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018054 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_2_RMSK                                                          0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_2_MAXn                                                                   4
#define HWIO_EE_n_GSI_CH_k_SCRATCH_2_MAXk                                                                  64
#define HWIO_EE_n_GSI_CH_k_SCRATCH_2_ATTR                                                                 0x3
#define HWIO_EE_n_GSI_CH_k_SCRATCH_2_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_2_ADDR(n,k), HWIO_EE_n_GSI_CH_k_SCRATCH_2_RMSK)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_2_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_2_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_2_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_SCRATCH_2_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_2_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_SCRATCH_2_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_SCRATCH_2_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_2_SCRATCH_BMSK                                                  0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_2_SCRATCH_SHFT                                                         0x0

#define HWIO_EE_n_GSI_CH_k_SCRATCH_3_ADDR(n,k)                                                     (GSI_TOP_GSI_REG_BASE      + 0x00018058 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_3_PHYS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018058 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_3_OFFS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018058 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_3_RMSK                                                          0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_3_MAXn                                                                   4
#define HWIO_EE_n_GSI_CH_k_SCRATCH_3_MAXk                                                                  64
#define HWIO_EE_n_GSI_CH_k_SCRATCH_3_ATTR                                                                 0x3
#define HWIO_EE_n_GSI_CH_k_SCRATCH_3_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_3_ADDR(n,k), HWIO_EE_n_GSI_CH_k_SCRATCH_3_RMSK)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_3_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_3_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_3_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_SCRATCH_3_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_3_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_SCRATCH_3_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_SCRATCH_3_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_3_SCRATCH_BMSK                                                  0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_3_SCRATCH_SHFT                                                         0x0

#define HWIO_EE_n_GSI_CH_k_SCRATCH_4_ADDR(n,k)                                                     (GSI_TOP_GSI_REG_BASE      + 0x0001805c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_4_PHYS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x0001805c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_4_OFFS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x0001805c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_4_RMSK                                                          0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_4_MAXn                                                                   4
#define HWIO_EE_n_GSI_CH_k_SCRATCH_4_MAXk                                                                  64
#define HWIO_EE_n_GSI_CH_k_SCRATCH_4_ATTR                                                                 0x3
#define HWIO_EE_n_GSI_CH_k_SCRATCH_4_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_4_ADDR(n,k), HWIO_EE_n_GSI_CH_k_SCRATCH_4_RMSK)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_4_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_4_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_4_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_SCRATCH_4_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_4_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_SCRATCH_4_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_SCRATCH_4_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_4_SCRATCH_BMSK                                                  0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_4_SCRATCH_SHFT                                                         0x0

#define HWIO_EE_n_GSI_CH_k_SCRATCH_5_ADDR(n,k)                                                     (GSI_TOP_GSI_REG_BASE      + 0x00018060 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_5_PHYS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018060 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_5_OFFS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018060 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_5_RMSK                                                          0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_5_MAXn                                                                   4
#define HWIO_EE_n_GSI_CH_k_SCRATCH_5_MAXk                                                                  64
#define HWIO_EE_n_GSI_CH_k_SCRATCH_5_ATTR                                                                 0x3
#define HWIO_EE_n_GSI_CH_k_SCRATCH_5_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_5_ADDR(n,k), HWIO_EE_n_GSI_CH_k_SCRATCH_5_RMSK)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_5_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_5_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_5_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_SCRATCH_5_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_5_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_SCRATCH_5_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_SCRATCH_5_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_5_SCRATCH_BMSK                                                  0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_5_SCRATCH_SHFT                                                         0x0

#define HWIO_EE_n_GSI_CH_k_SCRATCH_6_ADDR(n,k)                                                     (GSI_TOP_GSI_REG_BASE      + 0x00018064 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_6_PHYS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018064 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_6_OFFS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018064 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_6_RMSK                                                          0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_6_MAXn                                                                   4
#define HWIO_EE_n_GSI_CH_k_SCRATCH_6_MAXk                                                                  64
#define HWIO_EE_n_GSI_CH_k_SCRATCH_6_ATTR                                                                 0x3
#define HWIO_EE_n_GSI_CH_k_SCRATCH_6_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_6_ADDR(n,k), HWIO_EE_n_GSI_CH_k_SCRATCH_6_RMSK)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_6_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_6_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_6_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_SCRATCH_6_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_6_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_SCRATCH_6_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_SCRATCH_6_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_6_SCRATCH_BMSK                                                  0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_6_SCRATCH_SHFT                                                         0x0

#define HWIO_EE_n_GSI_CH_k_SCRATCH_7_ADDR(n,k)                                                     (GSI_TOP_GSI_REG_BASE      + 0x00018068 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_7_PHYS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018068 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_7_OFFS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018068 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_7_RMSK                                                          0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_7_MAXn                                                                   4
#define HWIO_EE_n_GSI_CH_k_SCRATCH_7_MAXk                                                                  64
#define HWIO_EE_n_GSI_CH_k_SCRATCH_7_ATTR                                                                 0x3
#define HWIO_EE_n_GSI_CH_k_SCRATCH_7_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_7_ADDR(n,k), HWIO_EE_n_GSI_CH_k_SCRATCH_7_RMSK)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_7_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_7_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_7_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_SCRATCH_7_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_7_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_SCRATCH_7_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_SCRATCH_7_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_7_SCRATCH_BMSK                                                  0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_7_SCRATCH_SHFT                                                         0x0

#define HWIO_EE_n_GSI_CH_k_SCRATCH_8_ADDR(n,k)                                                     (GSI_TOP_GSI_REG_BASE      + 0x0001806c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_8_PHYS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x0001806c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_8_OFFS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x0001806c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_8_RMSK                                                          0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_8_MAXn                                                                   4
#define HWIO_EE_n_GSI_CH_k_SCRATCH_8_MAXk                                                                  64
#define HWIO_EE_n_GSI_CH_k_SCRATCH_8_ATTR                                                                 0x3
#define HWIO_EE_n_GSI_CH_k_SCRATCH_8_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_8_ADDR(n,k), HWIO_EE_n_GSI_CH_k_SCRATCH_8_RMSK)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_8_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_8_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_8_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_SCRATCH_8_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_8_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_SCRATCH_8_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_SCRATCH_8_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_8_SCRATCH_BMSK                                                  0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_8_SCRATCH_SHFT                                                         0x0

#define HWIO_EE_n_GSI_CH_k_SCRATCH_9_ADDR(n,k)                                                     (GSI_TOP_GSI_REG_BASE      + 0x00018070 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_9_PHYS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018070 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_9_OFFS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018070 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_9_RMSK                                                          0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_9_MAXn                                                                   4
#define HWIO_EE_n_GSI_CH_k_SCRATCH_9_MAXk                                                                  64
#define HWIO_EE_n_GSI_CH_k_SCRATCH_9_ATTR                                                                 0x3
#define HWIO_EE_n_GSI_CH_k_SCRATCH_9_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_9_ADDR(n,k), HWIO_EE_n_GSI_CH_k_SCRATCH_9_RMSK)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_9_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_SCRATCH_9_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_9_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_SCRATCH_9_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_SCRATCH_9_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_SCRATCH_9_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_SCRATCH_9_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_SCRATCH_9_SCRATCH_BMSK                                                  0xffffffff
#define HWIO_EE_n_GSI_CH_k_SCRATCH_9_SCRATCH_SHFT                                                         0x0

#define HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_ADDR(n,k)                                              (GSI_TOP_GSI_REG_BASE      + 0x00018074 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_PHYS(n,k)                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00018074 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_OFFS(n,k)                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00018074 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_RMSK                                                       0xffff
#define HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_MAXn                                                            4
#define HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_MAXk                                                           64
#define HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_ATTR                                                          0x3
#define HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_ADDR(n,k), HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_RMSK)
#define HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_ADDR(n,k), mask)
#define HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_ADDR(n,k),mask,val,HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_INI2(n,k))
#define HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_LAST_DB_2_MCS_BMSK                                         0xffff
#define HWIO_EE_n_GSI_CH_k_DB_ENG_WRITE_PTR_LAST_DB_2_MCS_SHFT                                            0x0

#define HWIO_EE_n_EV_CH_k_CNTXT_0_ADDR(n,k)                                                        (GSI_TOP_GSI_REG_BASE      + 0x00020000 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_0_PHYS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020000 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_0_OFFS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020000 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_0_RMSK                                                             0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_0_MAXn                                                                      4
#define HWIO_EE_n_EV_CH_k_CNTXT_0_MAXk                                                                     61
#define HWIO_EE_n_EV_CH_k_CNTXT_0_ATTR                                                                    0x3
#define HWIO_EE_n_EV_CH_k_CNTXT_0_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_0_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_0_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_0_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_0_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_0_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_CNTXT_0_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_CNTXT_0_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_CNTXT_0_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_CNTXT_0_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_CNTXT_0_ELEMENT_SIZE_BMSK                                                0xff000000
#define HWIO_EE_n_EV_CH_k_CNTXT_0_ELEMENT_SIZE_SHFT                                                      0x18
#define HWIO_EE_n_EV_CH_k_CNTXT_0_CHSTATE_BMSK                                                       0xf00000
#define HWIO_EE_n_EV_CH_k_CNTXT_0_CHSTATE_SHFT                                                           0x14
#define HWIO_EE_n_EV_CH_k_CNTXT_0_CHSTATE_NOT_ALLOCATED_FVAL                                              0x0
#define HWIO_EE_n_EV_CH_k_CNTXT_0_CHSTATE_ALLOCATED_FVAL                                                  0x1
#define HWIO_EE_n_EV_CH_k_CNTXT_0_EE_BMSK                                                             0xf0000
#define HWIO_EE_n_EV_CH_k_CNTXT_0_EE_SHFT                                                                0x10
#define HWIO_EE_n_EV_CH_k_CNTXT_0_EVCHID_BMSK                                                          0xff00
#define HWIO_EE_n_EV_CH_k_CNTXT_0_EVCHID_SHFT                                                             0x8
#define HWIO_EE_n_EV_CH_k_CNTXT_0_INTYPE_BMSK                                                            0x80
#define HWIO_EE_n_EV_CH_k_CNTXT_0_INTYPE_SHFT                                                             0x7
#define HWIO_EE_n_EV_CH_k_CNTXT_0_INTYPE_MSI_FVAL                                                         0x0
#define HWIO_EE_n_EV_CH_k_CNTXT_0_INTYPE_IRQ_FVAL                                                         0x1
#define HWIO_EE_n_EV_CH_k_CNTXT_0_CHTYPE_BMSK                                                            0x7f
#define HWIO_EE_n_EV_CH_k_CNTXT_0_CHTYPE_SHFT                                                             0x0
#define HWIO_EE_n_EV_CH_k_CNTXT_0_CHTYPE_MHI_EV_FVAL                                                      0x0
#define HWIO_EE_n_EV_CH_k_CNTXT_0_CHTYPE_XHCI_EV_FVAL                                                     0x1
#define HWIO_EE_n_EV_CH_k_CNTXT_0_CHTYPE_GPI_EV_FVAL                                                      0x2
#define HWIO_EE_n_EV_CH_k_CNTXT_0_CHTYPE_XDCI_FVAL                                                        0x3

#define HWIO_EE_n_EV_CH_k_CNTXT_1_ADDR(n,k)                                                        (GSI_TOP_GSI_REG_BASE      + 0x00020004 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_1_PHYS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020004 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_1_OFFS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020004 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_1_RMSK                                                               0xffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_1_MAXn                                                                      4
#define HWIO_EE_n_EV_CH_k_CNTXT_1_MAXk                                                                     61
#define HWIO_EE_n_EV_CH_k_CNTXT_1_ATTR                                                                    0x3
#define HWIO_EE_n_EV_CH_k_CNTXT_1_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_1_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_1_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_1_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_1_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_1_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_CNTXT_1_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_CNTXT_1_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_CNTXT_1_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_CNTXT_1_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_CNTXT_1_R_LENGTH_BMSK                                                      0xffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_1_R_LENGTH_SHFT                                                           0x0

#define HWIO_EE_n_EV_CH_k_CNTXT_2_ADDR(n,k)                                                        (GSI_TOP_GSI_REG_BASE      + 0x00020008 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_2_PHYS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020008 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_2_OFFS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020008 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_2_RMSK                                                             0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_2_MAXn                                                                      4
#define HWIO_EE_n_EV_CH_k_CNTXT_2_MAXk                                                                     61
#define HWIO_EE_n_EV_CH_k_CNTXT_2_ATTR                                                                    0x3
#define HWIO_EE_n_EV_CH_k_CNTXT_2_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_2_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_2_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_2_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_2_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_2_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_CNTXT_2_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_CNTXT_2_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_CNTXT_2_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_CNTXT_2_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_CNTXT_2_R_BASE_ADDR_LSBS_BMSK                                            0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_2_R_BASE_ADDR_LSBS_SHFT                                                   0x0

#define HWIO_EE_n_EV_CH_k_CNTXT_3_ADDR(n,k)                                                        (GSI_TOP_GSI_REG_BASE      + 0x0002000c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_3_PHYS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x0002000c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_3_OFFS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x0002000c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_3_RMSK                                                             0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_3_MAXn                                                                      4
#define HWIO_EE_n_EV_CH_k_CNTXT_3_MAXk                                                                     61
#define HWIO_EE_n_EV_CH_k_CNTXT_3_ATTR                                                                    0x3
#define HWIO_EE_n_EV_CH_k_CNTXT_3_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_3_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_3_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_3_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_3_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_3_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_CNTXT_3_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_CNTXT_3_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_CNTXT_3_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_CNTXT_3_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_CNTXT_3_R_BASE_ADDR_MSBS_BMSK                                            0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_3_R_BASE_ADDR_MSBS_SHFT                                                   0x0

#define HWIO_EE_n_EV_CH_k_CNTXT_4_ADDR(n,k)                                                        (GSI_TOP_GSI_REG_BASE      + 0x00020010 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_4_PHYS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020010 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_4_OFFS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020010 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_4_RMSK                                                             0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_4_MAXn                                                                      4
#define HWIO_EE_n_EV_CH_k_CNTXT_4_MAXk                                                                     61
#define HWIO_EE_n_EV_CH_k_CNTXT_4_ATTR                                                                    0x3
#define HWIO_EE_n_EV_CH_k_CNTXT_4_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_4_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_4_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_4_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_4_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_4_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_CNTXT_4_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_CNTXT_4_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_CNTXT_4_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_CNTXT_4_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_CNTXT_4_READ_PTR_LSB_BMSK                                                0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_4_READ_PTR_LSB_SHFT                                                       0x0

#define HWIO_EE_n_EV_CH_k_CNTXT_5_ADDR(n,k)                                                        (GSI_TOP_GSI_REG_BASE      + 0x00020014 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_5_PHYS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020014 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_5_OFFS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020014 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_5_RMSK                                                             0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_5_MAXn                                                                      4
#define HWIO_EE_n_EV_CH_k_CNTXT_5_MAXk                                                                     61
#define HWIO_EE_n_EV_CH_k_CNTXT_5_ATTR                                                                    0x1
#define HWIO_EE_n_EV_CH_k_CNTXT_5_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_5_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_5_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_5_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_5_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_5_READ_PTR_MSB_BMSK                                                0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_5_READ_PTR_MSB_SHFT                                                       0x0

#define HWIO_EE_n_EV_CH_k_CNTXT_6_ADDR(n,k)                                                        (GSI_TOP_GSI_REG_BASE      + 0x00020018 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_6_PHYS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020018 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_6_OFFS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020018 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_6_RMSK                                                             0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_6_MAXn                                                                      4
#define HWIO_EE_n_EV_CH_k_CNTXT_6_MAXk                                                                     61
#define HWIO_EE_n_EV_CH_k_CNTXT_6_ATTR                                                                    0x1
#define HWIO_EE_n_EV_CH_k_CNTXT_6_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_6_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_6_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_6_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_6_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_6_WRITE_PTR_LSB_BMSK                                               0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_6_WRITE_PTR_LSB_SHFT                                                      0x0

#define HWIO_EE_n_EV_CH_k_CNTXT_7_ADDR(n,k)                                                        (GSI_TOP_GSI_REG_BASE      + 0x0002001c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_7_PHYS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x0002001c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_7_OFFS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x0002001c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_7_RMSK                                                             0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_7_MAXn                                                                      4
#define HWIO_EE_n_EV_CH_k_CNTXT_7_MAXk                                                                     61
#define HWIO_EE_n_EV_CH_k_CNTXT_7_ATTR                                                                    0x1
#define HWIO_EE_n_EV_CH_k_CNTXT_7_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_7_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_7_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_7_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_7_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_7_WRITE_PTR_MSB_BMSK                                               0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_7_WRITE_PTR_MSB_SHFT                                                      0x0

#define HWIO_EE_n_EV_CH_k_CNTXT_8_ADDR(n,k)                                                        (GSI_TOP_GSI_REG_BASE      + 0x00020020 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_8_PHYS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020020 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_8_OFFS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020020 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_8_RMSK                                                             0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_8_MAXn                                                                      4
#define HWIO_EE_n_EV_CH_k_CNTXT_8_MAXk                                                                     61
#define HWIO_EE_n_EV_CH_k_CNTXT_8_ATTR                                                                    0x3
#define HWIO_EE_n_EV_CH_k_CNTXT_8_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_8_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_8_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_8_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_8_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_8_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_CNTXT_8_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_CNTXT_8_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_CNTXT_8_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_CNTXT_8_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_CNTXT_8_INT_MOD_CNT_BMSK                                                 0xff000000
#define HWIO_EE_n_EV_CH_k_CNTXT_8_INT_MOD_CNT_SHFT                                                       0x18
#define HWIO_EE_n_EV_CH_k_CNTXT_8_INT_MODC_BMSK                                                      0xff0000
#define HWIO_EE_n_EV_CH_k_CNTXT_8_INT_MODC_SHFT                                                          0x10
#define HWIO_EE_n_EV_CH_k_CNTXT_8_INT_MODT_BMSK                                                        0xffff
#define HWIO_EE_n_EV_CH_k_CNTXT_8_INT_MODT_SHFT                                                           0x0

#define HWIO_EE_n_EV_CH_k_CNTXT_9_ADDR(n,k)                                                        (GSI_TOP_GSI_REG_BASE      + 0x00020024 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_9_PHYS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020024 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_9_OFFS(n,k)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020024 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_9_RMSK                                                             0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_9_MAXn                                                                      4
#define HWIO_EE_n_EV_CH_k_CNTXT_9_MAXk                                                                     61
#define HWIO_EE_n_EV_CH_k_CNTXT_9_ATTR                                                                    0x3
#define HWIO_EE_n_EV_CH_k_CNTXT_9_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_9_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_9_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_9_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_9_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_9_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_CNTXT_9_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_CNTXT_9_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_CNTXT_9_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_CNTXT_9_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_CNTXT_9_INTVEC_BMSK                                                      0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_9_INTVEC_SHFT                                                             0x0

#define HWIO_EE_n_EV_CH_k_CNTXT_10_ADDR(n,k)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00020028 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_10_PHYS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020028 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_10_OFFS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020028 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_10_RMSK                                                            0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_10_MAXn                                                                     4
#define HWIO_EE_n_EV_CH_k_CNTXT_10_MAXk                                                                    61
#define HWIO_EE_n_EV_CH_k_CNTXT_10_ATTR                                                                   0x3
#define HWIO_EE_n_EV_CH_k_CNTXT_10_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_10_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_10_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_10_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_10_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_10_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_CNTXT_10_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_CNTXT_10_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_CNTXT_10_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_CNTXT_10_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_CNTXT_10_MSI_ADDR_LSB_BMSK                                               0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_10_MSI_ADDR_LSB_SHFT                                                      0x0

#define HWIO_EE_n_EV_CH_k_CNTXT_11_ADDR(n,k)                                                       (GSI_TOP_GSI_REG_BASE      + 0x0002002c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_11_PHYS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x0002002c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_11_OFFS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x0002002c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_11_RMSK                                                            0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_11_MAXn                                                                     4
#define HWIO_EE_n_EV_CH_k_CNTXT_11_MAXk                                                                    61
#define HWIO_EE_n_EV_CH_k_CNTXT_11_ATTR                                                                   0x3
#define HWIO_EE_n_EV_CH_k_CNTXT_11_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_11_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_11_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_11_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_11_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_11_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_CNTXT_11_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_CNTXT_11_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_CNTXT_11_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_CNTXT_11_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_CNTXT_11_MSI_ADDR_MSB_BMSK                                               0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_11_MSI_ADDR_MSB_SHFT                                                      0x0

#define HWIO_EE_n_EV_CH_k_CNTXT_12_ADDR(n,k)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00020030 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_12_PHYS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020030 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_12_OFFS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020030 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_12_RMSK                                                            0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_12_MAXn                                                                     4
#define HWIO_EE_n_EV_CH_k_CNTXT_12_MAXk                                                                    61
#define HWIO_EE_n_EV_CH_k_CNTXT_12_ATTR                                                                   0x3
#define HWIO_EE_n_EV_CH_k_CNTXT_12_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_12_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_12_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_12_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_12_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_12_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_CNTXT_12_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_CNTXT_12_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_CNTXT_12_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_CNTXT_12_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_CNTXT_12_RP_UPDATE_ADDR_LSB_BMSK                                         0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_12_RP_UPDATE_ADDR_LSB_SHFT                                                0x0

#define HWIO_EE_n_EV_CH_k_CNTXT_13_ADDR(n,k)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00020034 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_13_PHYS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020034 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_13_OFFS(n,k)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020034 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_CNTXT_13_RMSK                                                            0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_13_MAXn                                                                     4
#define HWIO_EE_n_EV_CH_k_CNTXT_13_MAXk                                                                    61
#define HWIO_EE_n_EV_CH_k_CNTXT_13_ATTR                                                                   0x3
#define HWIO_EE_n_EV_CH_k_CNTXT_13_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_13_ADDR(n,k), HWIO_EE_n_EV_CH_k_CNTXT_13_RMSK)
#define HWIO_EE_n_EV_CH_k_CNTXT_13_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_CNTXT_13_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_CNTXT_13_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_CNTXT_13_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_CNTXT_13_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_CNTXT_13_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_CNTXT_13_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_CNTXT_13_RP_UPDATE_ADDR_MSB_BMSK                                         0xffffffff
#define HWIO_EE_n_EV_CH_k_CNTXT_13_RP_UPDATE_ADDR_MSB_SHFT                                                0x0

#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_ADDR(n,k)                                                (GSI_TOP_GSI_REG_BASE      + 0x00020038 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_PHYS(n,k)                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020038 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_OFFS(n,k)                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020038 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_RMSK                                                            0xf
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_MAXn                                                              4
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_MAXk                                                             61
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_ATTR                                                            0x1
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_ADDR(n,k), HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_RMSK)
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_BMSK                                            0xf
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_SHFT                                            0x0
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_TWO_FVAL                                        0x0
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_THREE_FVAL                                      0x1
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_FOUR_FVAL                                       0x2
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_FIVE_FVAL                                       0x3
#define HWIO_EE_n_EV_CH_k_ELEM_SIZE_SHIFT_ELEM_SIZE_SHIFT_SIX_FVAL                                        0x4

#define HWIO_EE_n_EV_CH_k_SCRATCH_0_ADDR(n,k)                                                      (GSI_TOP_GSI_REG_BASE      + 0x00020048 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_SCRATCH_0_PHYS(n,k)                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020048 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_SCRATCH_0_OFFS(n,k)                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020048 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_SCRATCH_0_RMSK                                                           0xffffffff
#define HWIO_EE_n_EV_CH_k_SCRATCH_0_MAXn                                                                    4
#define HWIO_EE_n_EV_CH_k_SCRATCH_0_MAXk                                                                   61
#define HWIO_EE_n_EV_CH_k_SCRATCH_0_ATTR                                                                  0x3
#define HWIO_EE_n_EV_CH_k_SCRATCH_0_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_SCRATCH_0_ADDR(n,k), HWIO_EE_n_EV_CH_k_SCRATCH_0_RMSK)
#define HWIO_EE_n_EV_CH_k_SCRATCH_0_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_SCRATCH_0_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_SCRATCH_0_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_SCRATCH_0_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_SCRATCH_0_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_SCRATCH_0_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_SCRATCH_0_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_SCRATCH_0_SCRATCH_BMSK                                                   0xffffffff
#define HWIO_EE_n_EV_CH_k_SCRATCH_0_SCRATCH_SHFT                                                          0x0

#define HWIO_EE_n_EV_CH_k_SCRATCH_1_ADDR(n,k)                                                      (GSI_TOP_GSI_REG_BASE      + 0x0002004c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_SCRATCH_1_PHYS(n,k)                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x0002004c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_SCRATCH_1_OFFS(n,k)                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x0002004c + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_SCRATCH_1_RMSK                                                           0xffffffff
#define HWIO_EE_n_EV_CH_k_SCRATCH_1_MAXn                                                                    4
#define HWIO_EE_n_EV_CH_k_SCRATCH_1_MAXk                                                                   61
#define HWIO_EE_n_EV_CH_k_SCRATCH_1_ATTR                                                                  0x3
#define HWIO_EE_n_EV_CH_k_SCRATCH_1_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_SCRATCH_1_ADDR(n,k), HWIO_EE_n_EV_CH_k_SCRATCH_1_RMSK)
#define HWIO_EE_n_EV_CH_k_SCRATCH_1_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_SCRATCH_1_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_SCRATCH_1_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_SCRATCH_1_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_SCRATCH_1_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_SCRATCH_1_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_SCRATCH_1_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_SCRATCH_1_SCRATCH_BMSK                                                   0xffffffff
#define HWIO_EE_n_EV_CH_k_SCRATCH_1_SCRATCH_SHFT                                                          0x0

#define HWIO_EE_n_EV_CH_k_SCRATCH_2_ADDR(n,k)                                                      (GSI_TOP_GSI_REG_BASE      + 0x00020050 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_SCRATCH_2_PHYS(n,k)                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x00020050 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_SCRATCH_2_OFFS(n,k)                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x00020050 + 0x12000 * (n) + 0x80 * (k))
#define HWIO_EE_n_EV_CH_k_SCRATCH_2_RMSK                                                           0xffffffff
#define HWIO_EE_n_EV_CH_k_SCRATCH_2_MAXn                                                                    4
#define HWIO_EE_n_EV_CH_k_SCRATCH_2_MAXk                                                                   61
#define HWIO_EE_n_EV_CH_k_SCRATCH_2_ATTR                                                                  0x3
#define HWIO_EE_n_EV_CH_k_SCRATCH_2_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_EV_CH_k_SCRATCH_2_ADDR(n,k), HWIO_EE_n_EV_CH_k_SCRATCH_2_RMSK)
#define HWIO_EE_n_EV_CH_k_SCRATCH_2_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_EV_CH_k_SCRATCH_2_ADDR(n,k), mask)
#define HWIO_EE_n_EV_CH_k_SCRATCH_2_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_SCRATCH_2_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_SCRATCH_2_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_EV_CH_k_SCRATCH_2_ADDR(n,k),mask,val,HWIO_EE_n_EV_CH_k_SCRATCH_2_INI2(n,k))
#define HWIO_EE_n_EV_CH_k_SCRATCH_2_SCRATCH_BMSK                                                   0xffffffff
#define HWIO_EE_n_EV_CH_k_SCRATCH_2_SCRATCH_SHFT                                                          0x0

#define HWIO_EE_n_GSI_CH_k_DOORBELL_0_ADDR(n,k)                                                    (GSI_TOP_GSI_REG_BASE      + 0x00028000 + 0x12000 * (n) + 0x8 * (k))
#define HWIO_EE_n_GSI_CH_k_DOORBELL_0_PHYS(n,k)                                                    (GSI_TOP_GSI_REG_BASE_PHYS + 0x00028000 + 0x12000 * (n) + 0x8 * (k))
#define HWIO_EE_n_GSI_CH_k_DOORBELL_0_OFFS(n,k)                                                    (GSI_TOP_GSI_REG_BASE_OFFS + 0x00028000 + 0x12000 * (n) + 0x8 * (k))
#define HWIO_EE_n_GSI_CH_k_DOORBELL_0_RMSK                                                         0xffffffff
#define HWIO_EE_n_GSI_CH_k_DOORBELL_0_MAXn                                                                  4
#define HWIO_EE_n_GSI_CH_k_DOORBELL_0_MAXk                                                                 64
#define HWIO_EE_n_GSI_CH_k_DOORBELL_0_ATTR                                                                0x2
#define HWIO_EE_n_GSI_CH_k_DOORBELL_0_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_DOORBELL_0_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_DOORBELL_0_WRITE_PTR_LSB_BMSK                                           0xffffffff
#define HWIO_EE_n_GSI_CH_k_DOORBELL_0_WRITE_PTR_LSB_SHFT                                                  0x0

#define HWIO_EE_n_GSI_CH_k_DOORBELL_1_ADDR(n,k)                                                    (GSI_TOP_GSI_REG_BASE      + 0x00028004 + 0x12000 * (n) + 0x8 * (k))
#define HWIO_EE_n_GSI_CH_k_DOORBELL_1_PHYS(n,k)                                                    (GSI_TOP_GSI_REG_BASE_PHYS + 0x00028004 + 0x12000 * (n) + 0x8 * (k))
#define HWIO_EE_n_GSI_CH_k_DOORBELL_1_OFFS(n,k)                                                    (GSI_TOP_GSI_REG_BASE_OFFS + 0x00028004 + 0x12000 * (n) + 0x8 * (k))
#define HWIO_EE_n_GSI_CH_k_DOORBELL_1_RMSK                                                         0xffffffff
#define HWIO_EE_n_GSI_CH_k_DOORBELL_1_MAXn                                                                  4
#define HWIO_EE_n_GSI_CH_k_DOORBELL_1_MAXk                                                                 64
#define HWIO_EE_n_GSI_CH_k_DOORBELL_1_ATTR                                                                0x2
#define HWIO_EE_n_GSI_CH_k_DOORBELL_1_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_GSI_CH_k_DOORBELL_1_ADDR(n,k),val)
#define HWIO_EE_n_GSI_CH_k_DOORBELL_1_WRITE_PTR_MSB_BMSK                                           0xffffffff
#define HWIO_EE_n_GSI_CH_k_DOORBELL_1_WRITE_PTR_MSB_SHFT                                                  0x0

#define HWIO_EE_n_EV_CH_k_DOORBELL_0_ADDR(n,k)                                                     (GSI_TOP_GSI_REG_BASE      + 0x00028800 + 0x12000 * (n) + 0x8 * (k))
#define HWIO_EE_n_EV_CH_k_DOORBELL_0_PHYS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00028800 + 0x12000 * (n) + 0x8 * (k))
#define HWIO_EE_n_EV_CH_k_DOORBELL_0_OFFS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00028800 + 0x12000 * (n) + 0x8 * (k))
#define HWIO_EE_n_EV_CH_k_DOORBELL_0_RMSK                                                          0xffffffff
#define HWIO_EE_n_EV_CH_k_DOORBELL_0_MAXn                                                                   4
#define HWIO_EE_n_EV_CH_k_DOORBELL_0_MAXk                                                                  61
#define HWIO_EE_n_EV_CH_k_DOORBELL_0_ATTR                                                                 0x2
#define HWIO_EE_n_EV_CH_k_DOORBELL_0_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_DOORBELL_0_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_DOORBELL_0_WRITE_PTR_LSB_BMSK                                            0xffffffff
#define HWIO_EE_n_EV_CH_k_DOORBELL_0_WRITE_PTR_LSB_SHFT                                                   0x0

#define HWIO_EE_n_EV_CH_k_DOORBELL_1_ADDR(n,k)                                                     (GSI_TOP_GSI_REG_BASE      + 0x00028804 + 0x12000 * (n) + 0x8 * (k))
#define HWIO_EE_n_EV_CH_k_DOORBELL_1_PHYS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_PHYS + 0x00028804 + 0x12000 * (n) + 0x8 * (k))
#define HWIO_EE_n_EV_CH_k_DOORBELL_1_OFFS(n,k)                                                     (GSI_TOP_GSI_REG_BASE_OFFS + 0x00028804 + 0x12000 * (n) + 0x8 * (k))
#define HWIO_EE_n_EV_CH_k_DOORBELL_1_RMSK                                                          0xffffffff
#define HWIO_EE_n_EV_CH_k_DOORBELL_1_MAXn                                                                   4
#define HWIO_EE_n_EV_CH_k_DOORBELL_1_MAXk                                                                  61
#define HWIO_EE_n_EV_CH_k_DOORBELL_1_ATTR                                                                 0x2
#define HWIO_EE_n_EV_CH_k_DOORBELL_1_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_EV_CH_k_DOORBELL_1_ADDR(n,k),val)
#define HWIO_EE_n_EV_CH_k_DOORBELL_1_WRITE_PTR_MSB_BMSK                                            0xffffffff
#define HWIO_EE_n_EV_CH_k_DOORBELL_1_WRITE_PTR_MSB_SHFT                                                   0x0

#define HWIO_EE_n_GSI_STATUS_ADDR(n)                                                               (GSI_TOP_GSI_REG_BASE      + 0x00029000 + 0x12000 * (n))
#define HWIO_EE_n_GSI_STATUS_PHYS(n)                                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029000 + 0x12000 * (n))
#define HWIO_EE_n_GSI_STATUS_OFFS(n)                                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029000 + 0x12000 * (n))
#define HWIO_EE_n_GSI_STATUS_RMSK                                                                         0x1
#define HWIO_EE_n_GSI_STATUS_MAXn                                                                           4
#define HWIO_EE_n_GSI_STATUS_ATTR                                                                         0x1
#define HWIO_EE_n_GSI_STATUS_INI(n)        \
        in_dword_masked(HWIO_EE_n_GSI_STATUS_ADDR(n), HWIO_EE_n_GSI_STATUS_RMSK)
#define HWIO_EE_n_GSI_STATUS_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_STATUS_ADDR(n), mask)
#define HWIO_EE_n_GSI_STATUS_ENABLED_BMSK                                                                 0x1
#define HWIO_EE_n_GSI_STATUS_ENABLED_SHFT                                                                 0x0

#define HWIO_EE_n_GSI_CH_CMD_ADDR(n)                                                               (GSI_TOP_GSI_REG_BASE      + 0x00029008 + 0x12000 * (n))
#define HWIO_EE_n_GSI_CH_CMD_PHYS(n)                                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029008 + 0x12000 * (n))
#define HWIO_EE_n_GSI_CH_CMD_OFFS(n)                                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029008 + 0x12000 * (n))
#define HWIO_EE_n_GSI_CH_CMD_RMSK                                                                  0xff0000ff
#define HWIO_EE_n_GSI_CH_CMD_MAXn                                                                           4
#define HWIO_EE_n_GSI_CH_CMD_ATTR                                                                         0x2
#define HWIO_EE_n_GSI_CH_CMD_OUTI(n,val)    \
        out_dword(HWIO_EE_n_GSI_CH_CMD_ADDR(n),val)
#define HWIO_EE_n_GSI_CH_CMD_OPCODE_BMSK                                                           0xff000000
#define HWIO_EE_n_GSI_CH_CMD_OPCODE_SHFT                                                                 0x18
#define HWIO_EE_n_GSI_CH_CMD_OPCODE_ALLOCATE_FVAL                                                         0x0
#define HWIO_EE_n_GSI_CH_CMD_OPCODE_START_FVAL                                                            0x1
#define HWIO_EE_n_GSI_CH_CMD_OPCODE_STOP_FVAL                                                             0x2
#define HWIO_EE_n_GSI_CH_CMD_OPCODE_RESET_FVAL                                                            0x9
#define HWIO_EE_n_GSI_CH_CMD_OPCODE_DE_ALLOC_FVAL                                                         0xa
#define HWIO_EE_n_GSI_CH_CMD_OPCODE_DB_STOP_FVAL                                                          0xb
#define HWIO_EE_n_GSI_CH_CMD_CHID_BMSK                                                                   0xff
#define HWIO_EE_n_GSI_CH_CMD_CHID_SHFT                                                                    0x0

#define HWIO_EE_n_EV_CH_CMD_ADDR(n)                                                                (GSI_TOP_GSI_REG_BASE      + 0x00029010 + 0x12000 * (n))
#define HWIO_EE_n_EV_CH_CMD_PHYS(n)                                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029010 + 0x12000 * (n))
#define HWIO_EE_n_EV_CH_CMD_OFFS(n)                                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029010 + 0x12000 * (n))
#define HWIO_EE_n_EV_CH_CMD_RMSK                                                                   0xff0000ff
#define HWIO_EE_n_EV_CH_CMD_MAXn                                                                            4
#define HWIO_EE_n_EV_CH_CMD_ATTR                                                                          0x2
#define HWIO_EE_n_EV_CH_CMD_OUTI(n,val)    \
        out_dword(HWIO_EE_n_EV_CH_CMD_ADDR(n),val)
#define HWIO_EE_n_EV_CH_CMD_OPCODE_BMSK                                                            0xff000000
#define HWIO_EE_n_EV_CH_CMD_OPCODE_SHFT                                                                  0x18
#define HWIO_EE_n_EV_CH_CMD_OPCODE_ALLOCATE_FVAL                                                          0x0
#define HWIO_EE_n_EV_CH_CMD_OPCODE_RESET_FVAL                                                             0x9
#define HWIO_EE_n_EV_CH_CMD_OPCODE_DE_ALLOC_FVAL                                                          0xa
#define HWIO_EE_n_EV_CH_CMD_CHID_BMSK                                                                    0xff
#define HWIO_EE_n_EV_CH_CMD_CHID_SHFT                                                                     0x0

#define HWIO_EE_n_GSI_EE_GENERIC_CMD_ADDR(n)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00029018 + 0x12000 * (n))
#define HWIO_EE_n_GSI_EE_GENERIC_CMD_PHYS(n)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029018 + 0x12000 * (n))
#define HWIO_EE_n_GSI_EE_GENERIC_CMD_OFFS(n)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029018 + 0x12000 * (n))
#define HWIO_EE_n_GSI_EE_GENERIC_CMD_RMSK                                                          0xffffffff
#define HWIO_EE_n_GSI_EE_GENERIC_CMD_MAXn                                                                   4
#define HWIO_EE_n_GSI_EE_GENERIC_CMD_ATTR                                                                 0x2
#define HWIO_EE_n_GSI_EE_GENERIC_CMD_OUTI(n,val)    \
        out_dword(HWIO_EE_n_GSI_EE_GENERIC_CMD_ADDR(n),val)
#define HWIO_EE_n_GSI_EE_GENERIC_CMD_OPCODE_BMSK                                                   0xffffffff
#define HWIO_EE_n_GSI_EE_GENERIC_CMD_OPCODE_SHFT                                                          0x0

#define HWIO_EE_n_GSI_HW_PARAM_0_ADDR(n)                                                           (GSI_TOP_GSI_REG_BASE      + 0x00029038 + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_0_PHYS(n)                                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029038 + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_0_OFFS(n)                                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029038 + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_0_RMSK                                                              0xffffffff
#define HWIO_EE_n_GSI_HW_PARAM_0_MAXn                                                                       4
#define HWIO_EE_n_GSI_HW_PARAM_0_ATTR                                                                     0x1
#define HWIO_EE_n_GSI_HW_PARAM_0_INI(n)        \
        in_dword_masked(HWIO_EE_n_GSI_HW_PARAM_0_ADDR(n), HWIO_EE_n_GSI_HW_PARAM_0_RMSK)
#define HWIO_EE_n_GSI_HW_PARAM_0_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_HW_PARAM_0_ADDR(n), mask)
#define HWIO_EE_n_GSI_HW_PARAM_0_USE_AXI_M_BMSK                                                    0x80000000
#define HWIO_EE_n_GSI_HW_PARAM_0_USE_AXI_M_SHFT                                                          0x1f
#define HWIO_EE_n_GSI_HW_PARAM_0_PERIPH_SEC_GRP_BMSK                                               0x7c000000
#define HWIO_EE_n_GSI_HW_PARAM_0_PERIPH_SEC_GRP_SHFT                                                     0x1a
#define HWIO_EE_n_GSI_HW_PARAM_0_PERIPH_CONF_ADDR_BUS_W_BMSK                                        0x3e00000
#define HWIO_EE_n_GSI_HW_PARAM_0_PERIPH_CONF_ADDR_BUS_W_SHFT                                             0x15
#define HWIO_EE_n_GSI_HW_PARAM_0_NUM_EES_BMSK                                                        0x1f0000
#define HWIO_EE_n_GSI_HW_PARAM_0_NUM_EES_SHFT                                                            0x10
#define HWIO_EE_n_GSI_HW_PARAM_0_GSI_CH_NUM_BMSK                                                       0xff00
#define HWIO_EE_n_GSI_HW_PARAM_0_GSI_CH_NUM_SHFT                                                          0x8
#define HWIO_EE_n_GSI_HW_PARAM_0_GSI_EV_CH_NUM_BMSK                                                      0xff
#define HWIO_EE_n_GSI_HW_PARAM_0_GSI_EV_CH_NUM_SHFT                                                       0x0

#define HWIO_EE_n_GSI_HW_PARAM_1_ADDR(n)                                                           (GSI_TOP_GSI_REG_BASE      + 0x0002903c + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_1_PHYS(n)                                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x0002903c + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_1_OFFS(n)                                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x0002903c + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_1_RMSK                                                              0xf7ffffff
#define HWIO_EE_n_GSI_HW_PARAM_1_MAXn                                                                       4
#define HWIO_EE_n_GSI_HW_PARAM_1_ATTR                                                                     0x1
#define HWIO_EE_n_GSI_HW_PARAM_1_INI(n)        \
        in_dword_masked(HWIO_EE_n_GSI_HW_PARAM_1_ADDR(n), HWIO_EE_n_GSI_HW_PARAM_1_RMSK)
#define HWIO_EE_n_GSI_HW_PARAM_1_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_HW_PARAM_1_ADDR(n), mask)
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_BLK_INT_ACCESS_REGION_2_EN_BMSK                               0x80000000
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_BLK_INT_ACCESS_REGION_2_EN_SHFT                                     0x1f
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_BLK_INT_ACCESS_REGION_1_EN_BMSK                               0x40000000
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_BLK_INT_ACCESS_REGION_1_EN_SHFT                                     0x1e
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_SIMPLE_RD_WR_BMSK                                             0x20000000
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_SIMPLE_RD_WR_SHFT                                                   0x1d
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_ESCAPE_BUF_ONLY_BMSK                                          0x10000000
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_ESCAPE_BUF_ONLY_SHFT                                                0x1c
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_USE_DB_ENG_BMSK                                                0x4000000
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_USE_DB_ENG_SHFT                                                     0x1a
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_NUM_TIMERS_BMSK                                                0x3f00000
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_NUM_TIMERS_SHFT                                                     0x14
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_USE_XPU_BMSK                                                     0x80000
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_USE_XPU_SHFT                                                        0x13
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_QRIB_EN_BMSK                                                     0x40000
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_QRIB_EN_SHFT                                                        0x12
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_VMIDACR_EN_BMSK                                                  0x20000
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_VMIDACR_EN_SHFT                                                     0x11
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_SEC_EN_BMSK                                                      0x10000
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_SEC_EN_SHFT                                                         0x10
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_NONSEC_EN_BMSK                                                    0xf000
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_NONSEC_EN_SHFT                                                       0xc
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_NUM_QAD_BMSK                                                       0xf00
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_NUM_QAD_SHFT                                                         0x8
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_M_DATA_BUS_W_BMSK                                                   0xff
#define HWIO_EE_n_GSI_HW_PARAM_1_GSI_M_DATA_BUS_W_SHFT                                                    0x0

#define HWIO_EE_n_GSI_HW_PARAM_2_ADDR(n)                                                           (GSI_TOP_GSI_REG_BASE      + 0x00029040 + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_2_PHYS(n)                                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029040 + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_2_OFFS(n)                                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029040 + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_2_RMSK                                                              0xffffffff
#define HWIO_EE_n_GSI_HW_PARAM_2_MAXn                                                                       4
#define HWIO_EE_n_GSI_HW_PARAM_2_ATTR                                                                     0x1
#define HWIO_EE_n_GSI_HW_PARAM_2_INI(n)        \
        in_dword_masked(HWIO_EE_n_GSI_HW_PARAM_2_ADDR(n), HWIO_EE_n_GSI_HW_PARAM_2_RMSK)
#define HWIO_EE_n_GSI_HW_PARAM_2_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_HW_PARAM_2_ADDR(n), mask)
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_USE_INTER_EE_BMSK                                             0x80000000
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_USE_INTER_EE_SHFT                                                   0x1f
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_USE_RD_WR_ENG_BMSK                                            0x40000000
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_USE_RD_WR_ENG_SHFT                                                  0x1e
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_SDMA_N_IOVEC_BMSK                                             0x38000000
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_SDMA_N_IOVEC_SHFT                                                   0x1b
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_SDMA_MAX_BURST_BMSK                                            0x7f80000
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_SDMA_MAX_BURST_SHFT                                                 0x13
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_SDMA_N_INT_BMSK                                                  0x70000
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_SDMA_N_INT_SHFT                                                     0x10
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_USE_SDMA_BMSK                                                     0x8000
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_USE_SDMA_SHFT                                                        0xf
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_CH_FULL_LOGIC_BMSK                                                0x4000
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_CH_FULL_LOGIC_SHFT                                                   0xe
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_CH_PEND_TRANSLATE_BMSK                                            0x2000
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_CH_PEND_TRANSLATE_SHFT                                               0xd
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_IRAM_SIZE_BMSK                                                    0x1f00
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_IRAM_SIZE_SHFT                                                       0x8
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_NUM_CH_PER_EE_BMSK                                                  0xff
#define HWIO_EE_n_GSI_HW_PARAM_2_GSI_NUM_CH_PER_EE_SHFT                                                   0x0

#define HWIO_EE_n_GSI_MCS_CODE_VER_ADDR(n)                                                         (GSI_TOP_GSI_REG_BASE      + 0x00029048 + 0x12000 * (n))
#define HWIO_EE_n_GSI_MCS_CODE_VER_PHYS(n)                                                         (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029048 + 0x12000 * (n))
#define HWIO_EE_n_GSI_MCS_CODE_VER_OFFS(n)                                                         (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029048 + 0x12000 * (n))
#define HWIO_EE_n_GSI_MCS_CODE_VER_RMSK                                                            0xffffffff
#define HWIO_EE_n_GSI_MCS_CODE_VER_MAXn                                                                     4
#define HWIO_EE_n_GSI_MCS_CODE_VER_ATTR                                                                   0x1
#define HWIO_EE_n_GSI_MCS_CODE_VER_INI(n)        \
        in_dword_masked(HWIO_EE_n_GSI_MCS_CODE_VER_ADDR(n), HWIO_EE_n_GSI_MCS_CODE_VER_RMSK)
#define HWIO_EE_n_GSI_MCS_CODE_VER_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_MCS_CODE_VER_ADDR(n), mask)
#define HWIO_EE_n_GSI_MCS_CODE_VER_VER_BMSK                                                        0xffffffff
#define HWIO_EE_n_GSI_MCS_CODE_VER_VER_SHFT                                                               0x0

#define HWIO_EE_n_GSI_HW_PARAM_3_ADDR(n)                                                           (GSI_TOP_GSI_REG_BASE      + 0x0002904c + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_3_PHYS(n)                                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x0002904c + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_3_OFFS(n)                                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x0002904c + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_3_RMSK                                                              0x3fffffff
#define HWIO_EE_n_GSI_HW_PARAM_3_MAXn                                                                       4
#define HWIO_EE_n_GSI_HW_PARAM_3_ATTR                                                                     0x1
#define HWIO_EE_n_GSI_HW_PARAM_3_INI(n)        \
        in_dword_masked(HWIO_EE_n_GSI_HW_PARAM_3_ADDR(n), HWIO_EE_n_GSI_HW_PARAM_3_RMSK)
#define HWIO_EE_n_GSI_HW_PARAM_3_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_HW_PARAM_3_ADDR(n), mask)
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_USE_BP_MTRIX_BMSK                                             0x20000000
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_USE_BP_MTRIX_SHFT                                                   0x1d
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_USE_DB_MSI_MODE_BMSK                                          0x10000000
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_USE_DB_MSI_MODE_SHFT                                                0x1c
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_USE_SLEEP_CLK_DIV_BMSK                                         0x8000000
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_USE_SLEEP_CLK_DIV_SHFT                                              0x1b
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_USE_VIR_CH_IF_BMSK                                             0x4000000
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_USE_VIR_CH_IF_SHFT                                                  0x1a
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_USE_IROM_BMSK                                                  0x2000000
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_USE_IROM_SHFT                                                       0x19
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_REE_MAX_BURST_LEN_BMSK                                         0x1f00000
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_REE_MAX_BURST_LEN_SHFT                                              0x14
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_M_ADDR_BUS_W_BMSK                                                0xff000
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_M_ADDR_BUS_W_SHFT                                                    0xc
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_NUM_PREFETCH_BUFS_BMSK                                             0xf00
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_NUM_PREFETCH_BUFS_SHFT                                               0x8
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_SDMA_MAX_OS_WR_BMSK                                                 0xf0
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_SDMA_MAX_OS_WR_SHFT                                                  0x4
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_SDMA_MAX_OS_RD_BMSK                                                  0xf
#define HWIO_EE_n_GSI_HW_PARAM_3_GSI_SDMA_MAX_OS_RD_SHFT                                                  0x0

#define HWIO_EE_n_GSI_HW_PARAM_4_ADDR(n)                                                           (GSI_TOP_GSI_REG_BASE      + 0x00029050 + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_4_PHYS(n)                                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029050 + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_4_OFFS(n)                                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029050 + 0x12000 * (n))
#define HWIO_EE_n_GSI_HW_PARAM_4_RMSK                                                              0xffffffff
#define HWIO_EE_n_GSI_HW_PARAM_4_MAXn                                                                       4
#define HWIO_EE_n_GSI_HW_PARAM_4_ATTR                                                                     0x1
#define HWIO_EE_n_GSI_HW_PARAM_4_INI(n)        \
        in_dword_masked(HWIO_EE_n_GSI_HW_PARAM_4_ADDR(n), HWIO_EE_n_GSI_HW_PARAM_4_RMSK)
#define HWIO_EE_n_GSI_HW_PARAM_4_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_GSI_HW_PARAM_4_ADDR(n), mask)
#define HWIO_EE_n_GSI_HW_PARAM_4_GSI_SHRAM_SIZE_DWORD_BMSK                                         0xffff0000
#define HWIO_EE_n_GSI_HW_PARAM_4_GSI_SHRAM_SIZE_DWORD_SHFT                                               0x10
#define HWIO_EE_n_GSI_HW_PARAM_4_GSI_IRAM_PROTOCOL_CNT_BMSK                                            0xff00
#define HWIO_EE_n_GSI_HW_PARAM_4_GSI_IRAM_PROTOCOL_CNT_SHFT                                               0x8
#define HWIO_EE_n_GSI_HW_PARAM_4_GSI_NUM_EV_PER_EE_BMSK                                                  0xff
#define HWIO_EE_n_GSI_HW_PARAM_4_GSI_NUM_EV_PER_EE_SHFT                                                   0x0

#define HWIO_EE_n_CNTXT_TYPE_IRQ_ADDR(n)                                                           (GSI_TOP_GSI_REG_BASE      + 0x00029080 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_TYPE_IRQ_PHYS(n)                                                           (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029080 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_TYPE_IRQ_OFFS(n)                                                           (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029080 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_TYPE_IRQ_RMSK                                                                    0x7f
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MAXn                                                                       4
#define HWIO_EE_n_CNTXT_TYPE_IRQ_ATTR                                                                     0x1
#define HWIO_EE_n_CNTXT_TYPE_IRQ_INI(n)        \
        in_dword_masked(HWIO_EE_n_CNTXT_TYPE_IRQ_ADDR(n), HWIO_EE_n_CNTXT_TYPE_IRQ_RMSK)
#define HWIO_EE_n_CNTXT_TYPE_IRQ_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_TYPE_IRQ_ADDR(n), mask)
#define HWIO_EE_n_CNTXT_TYPE_IRQ_GENERAL_BMSK                                                            0x40
#define HWIO_EE_n_CNTXT_TYPE_IRQ_GENERAL_SHFT                                                             0x6
#define HWIO_EE_n_CNTXT_TYPE_IRQ_INTER_EE_EV_CTRL_BMSK                                                   0x20
#define HWIO_EE_n_CNTXT_TYPE_IRQ_INTER_EE_EV_CTRL_SHFT                                                    0x5
#define HWIO_EE_n_CNTXT_TYPE_IRQ_INTER_EE_CH_CTRL_BMSK                                                   0x10
#define HWIO_EE_n_CNTXT_TYPE_IRQ_INTER_EE_CH_CTRL_SHFT                                                    0x4
#define HWIO_EE_n_CNTXT_TYPE_IRQ_IEOB_BMSK                                                                0x8
#define HWIO_EE_n_CNTXT_TYPE_IRQ_IEOB_SHFT                                                                0x3
#define HWIO_EE_n_CNTXT_TYPE_IRQ_GLOB_EE_BMSK                                                             0x4
#define HWIO_EE_n_CNTXT_TYPE_IRQ_GLOB_EE_SHFT                                                             0x2
#define HWIO_EE_n_CNTXT_TYPE_IRQ_EV_CTRL_BMSK                                                             0x2
#define HWIO_EE_n_CNTXT_TYPE_IRQ_EV_CTRL_SHFT                                                             0x1
#define HWIO_EE_n_CNTXT_TYPE_IRQ_CH_CTRL_BMSK                                                             0x1
#define HWIO_EE_n_CNTXT_TYPE_IRQ_CH_CTRL_SHFT                                                             0x0

#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_ADDR(n)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00029088 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_PHYS(n)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029088 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_OFFS(n)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029088 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_RMSK                                                                0x7f
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_MAXn                                                                   4
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_ATTR                                                                 0x3
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_INI(n)        \
        in_dword_masked(HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_ADDR(n), HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_RMSK)
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_ADDR(n), mask)
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_OUTI(n,val)    \
        out_dword(HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_ADDR(n),val)
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_ADDR(n),mask,val,HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_INI(n))
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_GENERAL_BMSK                                                        0x40
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_GENERAL_SHFT                                                         0x6
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_INTER_EE_EV_CTRL_BMSK                                               0x20
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_INTER_EE_EV_CTRL_SHFT                                                0x5
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_INTER_EE_CH_CTRL_BMSK                                               0x10
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_INTER_EE_CH_CTRL_SHFT                                                0x4
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_IEOB_BMSK                                                            0x8
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_IEOB_SHFT                                                            0x3
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_GLOB_EE_BMSK                                                         0x4
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_GLOB_EE_SHFT                                                         0x2
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_EV_CTRL_BMSK                                                         0x2
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_EV_CTRL_SHFT                                                         0x1
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_CH_CTRL_BMSK                                                         0x1
#define HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_CH_CTRL_SHFT                                                         0x0

#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_ADDR(n,k)                                                 (GSI_TOP_GSI_REG_BASE      + 0x00029090 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_PHYS(n,k)                                                 (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029090 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_OFFS(n,k)                                                 (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029090 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_RMSK                                                      0xffffffff
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_MAXn                                                               4
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_MAXk                                                               2
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_ATTR                                                             0x1
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_ADDR(n,k), HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_RMSK)
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_ADDR(n,k), mask)
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_GSI_CH_BIT_MAP_BMSK                                       0xffffffff
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_k_GSI_CH_BIT_MAP_SHFT                                              0x0

#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_ADDR(n,k)                                             (GSI_TOP_GSI_REG_BASE      + 0x00029094 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_PHYS(n,k)                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029094 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_OFFS(n,k)                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029094 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_RMSK                                                  0xffffffff
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_MAXn                                                           4
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_MAXk                                                           2
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_ATTR                                                         0x3
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_ADDR(n,k), HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_RMSK)
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_ADDR(n,k), mask)
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_ADDR(n,k),val)
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_ADDR(n,k),mask,val,HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_INI2(n,k))
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_GSI_CH_BIT_MAP_MSK_BMSK                               0xffffffff
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_k_GSI_CH_BIT_MAP_MSK_SHFT                                      0x0

#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_k_ADDR(n,k)                                             (GSI_TOP_GSI_REG_BASE      + 0x00029098 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_k_PHYS(n,k)                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029098 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_k_OFFS(n,k)                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029098 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_k_RMSK                                                  0xffffffff
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_k_MAXn                                                           4
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_k_MAXk                                                           2
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_k_ATTR                                                         0x2
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_k_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_k_ADDR(n,k),val)
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_k_GSI_CH_BIT_MAP_BMSK                                   0xffffffff
#define HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_k_GSI_CH_BIT_MAP_SHFT                                          0x0

#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_ADDR(n,k)                                                  (GSI_TOP_GSI_REG_BASE      + 0x0002909c + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_PHYS(n,k)                                                  (GSI_TOP_GSI_REG_BASE_PHYS + 0x0002909c + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_OFFS(n,k)                                                  (GSI_TOP_GSI_REG_BASE_OFFS + 0x0002909c + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_RMSK                                                       0xffffffff
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_MAXn                                                                4
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_MAXk                                                                1
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_ATTR                                                              0x1
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_ADDR(n,k), HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_RMSK)
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_ADDR(n,k), mask)
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_EV_CH_BIT_MAP_BMSK                                         0xffffffff
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_k_EV_CH_BIT_MAP_SHFT                                                0x0

#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_ADDR(n,k)                                              (GSI_TOP_GSI_REG_BASE      + 0x000290a0 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_PHYS(n,k)                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x000290a0 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_OFFS(n,k)                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x000290a0 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_RMSK                                                   0xffffffff
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_MAXn                                                            4
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_MAXk                                                            1
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_ATTR                                                          0x3
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_ADDR(n,k), HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_RMSK)
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_ADDR(n,k), mask)
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_ADDR(n,k),val)
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_ADDR(n,k),mask,val,HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_INI2(n,k))
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_EV_CH_BIT_MAP_MSK_BMSK                                 0xffffffff
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_k_EV_CH_BIT_MAP_MSK_SHFT                                        0x0

#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_k_ADDR(n,k)                                              (GSI_TOP_GSI_REG_BASE      + 0x000290a4 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_k_PHYS(n,k)                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x000290a4 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_k_OFFS(n,k)                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x000290a4 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_k_RMSK                                                   0xffffffff
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_k_MAXn                                                            4
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_k_MAXk                                                            1
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_k_ATTR                                                          0x2
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_k_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_k_ADDR(n,k),val)
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_k_EV_CH_BIT_MAP_BMSK                                     0xffffffff
#define HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_k_EV_CH_BIT_MAP_SHFT                                            0x0

#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_ADDR(n,k)                                                   (GSI_TOP_GSI_REG_BASE      + 0x000290a8 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_PHYS(n,k)                                                   (GSI_TOP_GSI_REG_BASE_PHYS + 0x000290a8 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_OFFS(n,k)                                                   (GSI_TOP_GSI_REG_BASE_OFFS + 0x000290a8 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_RMSK                                                        0xffffffff
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_MAXn                                                                 4
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_MAXk                                                                 1
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_ATTR                                                               0x1
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_ADDR(n,k), HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_RMSK)
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_ADDR(n,k), mask)
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_EV_CH_BIT_MAP_BMSK                                          0xffffffff
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_k_EV_CH_BIT_MAP_SHFT                                                 0x0

#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_ADDR(n,k)                                               (GSI_TOP_GSI_REG_BASE      + 0x000290ac + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_PHYS(n,k)                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x000290ac + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_OFFS(n,k)                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x000290ac + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_RMSK                                                    0xffffffff
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_MAXn                                                             4
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_MAXk                                                             1
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_ATTR                                                           0x3
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_INI2(n,k)        \
        in_dword_masked(HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_ADDR(n,k), HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_RMSK)
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_ADDR(n,k), mask)
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_ADDR(n,k),val)
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_ADDR(n,k),mask,val,HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_INI2(n,k))
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_EV_CH_BIT_MAP_MSK_BMSK                                  0xffffffff
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_k_EV_CH_BIT_MAP_MSK_SHFT                                         0x0

#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_CLR_k_ADDR(n,k)                                               (GSI_TOP_GSI_REG_BASE      + 0x000290b0 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_CLR_k_PHYS(n,k)                                               (GSI_TOP_GSI_REG_BASE_PHYS + 0x000290b0 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_CLR_k_OFFS(n,k)                                               (GSI_TOP_GSI_REG_BASE_OFFS + 0x000290b0 + 0x24 * (k) + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_CLR_k_RMSK                                                    0xffffffff
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_CLR_k_MAXn                                                             4
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_CLR_k_MAXk                                                             1
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_CLR_k_ATTR                                                           0x2
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_CLR_k_OUTI2(n,k,val)    \
        out_dword(HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_CLR_k_ADDR(n,k),val)
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_CLR_k_EV_CH_BIT_MAP_BMSK                                      0xffffffff
#define HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_CLR_k_EV_CH_BIT_MAP_SHFT                                             0x0

#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_ADDR(n)                                                      (GSI_TOP_GSI_REG_BASE      + 0x00029200 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_PHYS(n)                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029200 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_OFFS(n)                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029200 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_RMSK                                                                0xf
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_MAXn                                                                  4
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_ATTR                                                                0x1
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_INI(n)        \
        in_dword_masked(HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_ADDR(n), HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_RMSK)
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_ADDR(n), mask)
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_GP_INT3_BMSK                                                        0x8
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_GP_INT3_SHFT                                                        0x3
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_GP_INT2_BMSK                                                        0x4
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_GP_INT2_SHFT                                                        0x2
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_GP_INT1_BMSK                                                        0x2
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_GP_INT1_SHFT                                                        0x1
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_ERROR_INT_BMSK                                                      0x1
#define HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_ERROR_INT_SHFT                                                      0x0

#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_ADDR(n)                                                        (GSI_TOP_GSI_REG_BASE      + 0x00029204 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_PHYS(n)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029204 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_OFFS(n)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029204 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_RMSK                                                                  0xf
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_MAXn                                                                    4
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_ATTR                                                                  0x3
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_INI(n)        \
        in_dword_masked(HWIO_EE_n_CNTXT_GLOB_IRQ_EN_ADDR(n), HWIO_EE_n_CNTXT_GLOB_IRQ_EN_RMSK)
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_GLOB_IRQ_EN_ADDR(n), mask)
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_OUTI(n,val)    \
        out_dword(HWIO_EE_n_CNTXT_GLOB_IRQ_EN_ADDR(n),val)
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_CNTXT_GLOB_IRQ_EN_ADDR(n),mask,val,HWIO_EE_n_CNTXT_GLOB_IRQ_EN_INI(n))
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_GP_INT3_BMSK                                                          0x8
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_GP_INT3_SHFT                                                          0x3
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_GP_INT2_BMSK                                                          0x4
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_GP_INT2_SHFT                                                          0x2
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_GP_INT1_BMSK                                                          0x2
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_GP_INT1_SHFT                                                          0x1
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_ERROR_INT_BMSK                                                        0x1
#define HWIO_EE_n_CNTXT_GLOB_IRQ_EN_ERROR_INT_SHFT                                                        0x0

#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_ADDR(n)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00029208 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_PHYS(n)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029208 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_OFFS(n)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029208 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_RMSK                                                                 0xf
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_MAXn                                                                   4
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_ATTR                                                                 0x2
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_OUTI(n,val)    \
        out_dword(HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_ADDR(n),val)
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_GP_INT3_BMSK                                                         0x8
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_GP_INT3_SHFT                                                         0x3
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_GP_INT2_BMSK                                                         0x4
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_GP_INT2_SHFT                                                         0x2
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_GP_INT1_BMSK                                                         0x2
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_GP_INT1_SHFT                                                         0x1
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_ERROR_INT_BMSK                                                       0x1
#define HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_ERROR_INT_SHFT                                                       0x0

#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_ADDR(n)                                                       (GSI_TOP_GSI_REG_BASE      + 0x0002920c + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_PHYS(n)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x0002920c + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_OFFS(n)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x0002920c + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_RMSK                                                          0xff0101ff
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_MAXn                                                                   4
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_ATTR                                                                 0x1
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_INI(n)        \
        in_dword_masked(HWIO_EE_n_CNTXT_GSI_IRQ_STTS_ADDR(n), HWIO_EE_n_CNTXT_GSI_IRQ_STTS_RMSK)
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_GSI_IRQ_STTS_ADDR(n), mask)
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_GSI_MCS_STACK_OVRFLOW_BMSK                                    0xff000000
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_GSI_MCS_STACK_OVRFLOW_SHFT                                          0x18
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_GSI_CMD_FIFO_OVRFLOW_BMSK                                        0x10000
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_GSI_CMD_FIFO_OVRFLOW_SHFT                                           0x10
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_GSI_BUS_ERROR_BMSK                                                 0x100
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_GSI_BUS_ERROR_SHFT                                                   0x8
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_GSI_BREAK_POINT_BMSK                                                0xff
#define HWIO_EE_n_CNTXT_GSI_IRQ_STTS_GSI_BREAK_POINT_SHFT                                                 0x0

#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_ADDR(n)                                                         (GSI_TOP_GSI_REG_BASE      + 0x00029210 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_PHYS(n)                                                         (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029210 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_OFFS(n)                                                         (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029210 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_RMSK                                                            0xff0101ff
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_MAXn                                                                     4
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_ATTR                                                                   0x3
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_INI(n)        \
        in_dword_masked(HWIO_EE_n_CNTXT_GSI_IRQ_EN_ADDR(n), HWIO_EE_n_CNTXT_GSI_IRQ_EN_RMSK)
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_GSI_IRQ_EN_ADDR(n), mask)
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_OUTI(n,val)    \
        out_dword(HWIO_EE_n_CNTXT_GSI_IRQ_EN_ADDR(n),val)
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_CNTXT_GSI_IRQ_EN_ADDR(n),mask,val,HWIO_EE_n_CNTXT_GSI_IRQ_EN_INI(n))
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_GSI_MCS_STACK_OVRFLOW_BMSK                                      0xff000000
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_GSI_MCS_STACK_OVRFLOW_SHFT                                            0x18
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_GSI_CMD_FIFO_OVRFLOW_BMSK                                          0x10000
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_GSI_CMD_FIFO_OVRFLOW_SHFT                                             0x10
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_GSI_BUS_ERROR_BMSK                                                   0x100
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_GSI_BUS_ERROR_SHFT                                                     0x8
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_GSI_BREAK_POINT_BMSK                                                  0xff
#define HWIO_EE_n_CNTXT_GSI_IRQ_EN_GSI_BREAK_POINT_SHFT                                                   0x0

#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_ADDR(n)                                                        (GSI_TOP_GSI_REG_BASE      + 0x00029214 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_PHYS(n)                                                        (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029214 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_OFFS(n)                                                        (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029214 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_RMSK                                                           0xff0101ff
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_MAXn                                                                    4
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_ATTR                                                                  0x2
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_OUTI(n,val)    \
        out_dword(HWIO_EE_n_CNTXT_GSI_IRQ_CLR_ADDR(n),val)
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_GSI_MCS_STACK_OVRFLOW_BMSK                                     0xff000000
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_GSI_MCS_STACK_OVRFLOW_SHFT                                           0x18
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_GSI_CMD_FIFO_OVRFLOW_BMSK                                         0x10000
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_GSI_CMD_FIFO_OVRFLOW_SHFT                                            0x10
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_GSI_BUS_ERROR_BMSK                                                  0x100
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_GSI_BUS_ERROR_SHFT                                                    0x8
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_GSI_BREAK_POINT_BMSK                                                 0xff
#define HWIO_EE_n_CNTXT_GSI_IRQ_CLR_GSI_BREAK_POINT_SHFT                                                  0x0

#define HWIO_EE_n_CNTXT_INTSET_ADDR(n)                                                             (GSI_TOP_GSI_REG_BASE      + 0x00029220 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_INTSET_PHYS(n)                                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029220 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_INTSET_OFFS(n)                                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029220 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_INTSET_RMSK                                                                       0x1
#define HWIO_EE_n_CNTXT_INTSET_MAXn                                                                         4
#define HWIO_EE_n_CNTXT_INTSET_ATTR                                                                       0x3
#define HWIO_EE_n_CNTXT_INTSET_INI(n)        \
        in_dword_masked(HWIO_EE_n_CNTXT_INTSET_ADDR(n), HWIO_EE_n_CNTXT_INTSET_RMSK)
#define HWIO_EE_n_CNTXT_INTSET_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_INTSET_ADDR(n), mask)
#define HWIO_EE_n_CNTXT_INTSET_OUTI(n,val)    \
        out_dword(HWIO_EE_n_CNTXT_INTSET_ADDR(n),val)
#define HWIO_EE_n_CNTXT_INTSET_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_CNTXT_INTSET_ADDR(n),mask,val,HWIO_EE_n_CNTXT_INTSET_INI(n))
#define HWIO_EE_n_CNTXT_INTSET_INTYPE_BMSK                                                                0x1
#define HWIO_EE_n_CNTXT_INTSET_INTYPE_SHFT                                                                0x0
#define HWIO_EE_n_CNTXT_INTSET_INTYPE_MSI_FVAL                                                            0x0
#define HWIO_EE_n_CNTXT_INTSET_INTYPE_IRQ_FVAL                                                            0x1

#define HWIO_EE_n_CNTXT_MSI_BASE_LSB_ADDR(n)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00029230 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_MSI_BASE_LSB_PHYS(n)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029230 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_MSI_BASE_LSB_OFFS(n)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029230 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_MSI_BASE_LSB_RMSK                                                          0xffffffff
#define HWIO_EE_n_CNTXT_MSI_BASE_LSB_MAXn                                                                   4
#define HWIO_EE_n_CNTXT_MSI_BASE_LSB_ATTR                                                                 0x3
#define HWIO_EE_n_CNTXT_MSI_BASE_LSB_INI(n)        \
        in_dword_masked(HWIO_EE_n_CNTXT_MSI_BASE_LSB_ADDR(n), HWIO_EE_n_CNTXT_MSI_BASE_LSB_RMSK)
#define HWIO_EE_n_CNTXT_MSI_BASE_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_MSI_BASE_LSB_ADDR(n), mask)
#define HWIO_EE_n_CNTXT_MSI_BASE_LSB_OUTI(n,val)    \
        out_dword(HWIO_EE_n_CNTXT_MSI_BASE_LSB_ADDR(n),val)
#define HWIO_EE_n_CNTXT_MSI_BASE_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_CNTXT_MSI_BASE_LSB_ADDR(n),mask,val,HWIO_EE_n_CNTXT_MSI_BASE_LSB_INI(n))
#define HWIO_EE_n_CNTXT_MSI_BASE_LSB_MSI_ADDR_LSB_BMSK                                             0xffffffff
#define HWIO_EE_n_CNTXT_MSI_BASE_LSB_MSI_ADDR_LSB_SHFT                                                    0x0

#define HWIO_EE_n_CNTXT_MSI_BASE_MSB_ADDR(n)                                                       (GSI_TOP_GSI_REG_BASE      + 0x00029234 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_MSI_BASE_MSB_PHYS(n)                                                       (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029234 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_MSI_BASE_MSB_OFFS(n)                                                       (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029234 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_MSI_BASE_MSB_RMSK                                                          0xffffffff
#define HWIO_EE_n_CNTXT_MSI_BASE_MSB_MAXn                                                                   4
#define HWIO_EE_n_CNTXT_MSI_BASE_MSB_ATTR                                                                 0x3
#define HWIO_EE_n_CNTXT_MSI_BASE_MSB_INI(n)        \
        in_dword_masked(HWIO_EE_n_CNTXT_MSI_BASE_MSB_ADDR(n), HWIO_EE_n_CNTXT_MSI_BASE_MSB_RMSK)
#define HWIO_EE_n_CNTXT_MSI_BASE_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_MSI_BASE_MSB_ADDR(n), mask)
#define HWIO_EE_n_CNTXT_MSI_BASE_MSB_OUTI(n,val)    \
        out_dword(HWIO_EE_n_CNTXT_MSI_BASE_MSB_ADDR(n),val)
#define HWIO_EE_n_CNTXT_MSI_BASE_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_CNTXT_MSI_BASE_MSB_ADDR(n),mask,val,HWIO_EE_n_CNTXT_MSI_BASE_MSB_INI(n))
#define HWIO_EE_n_CNTXT_MSI_BASE_MSB_MSI_ADDR_MSB_BMSK                                             0xffffffff
#define HWIO_EE_n_CNTXT_MSI_BASE_MSB_MSI_ADDR_MSB_SHFT                                                    0x0

#define HWIO_EE_n_CNTXT_INT_VEC_ADDR(n)                                                            (GSI_TOP_GSI_REG_BASE      + 0x00029238 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_INT_VEC_PHYS(n)                                                            (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029238 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_INT_VEC_OFFS(n)                                                            (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029238 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_INT_VEC_RMSK                                                               0xffffffff
#define HWIO_EE_n_CNTXT_INT_VEC_MAXn                                                                        4
#define HWIO_EE_n_CNTXT_INT_VEC_ATTR                                                                      0x3
#define HWIO_EE_n_CNTXT_INT_VEC_INI(n)        \
        in_dword_masked(HWIO_EE_n_CNTXT_INT_VEC_ADDR(n), HWIO_EE_n_CNTXT_INT_VEC_RMSK)
#define HWIO_EE_n_CNTXT_INT_VEC_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_INT_VEC_ADDR(n), mask)
#define HWIO_EE_n_CNTXT_INT_VEC_OUTI(n,val)    \
        out_dword(HWIO_EE_n_CNTXT_INT_VEC_ADDR(n),val)
#define HWIO_EE_n_CNTXT_INT_VEC_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_CNTXT_INT_VEC_ADDR(n),mask,val,HWIO_EE_n_CNTXT_INT_VEC_INI(n))
#define HWIO_EE_n_CNTXT_INT_VEC_INT_VEC_BMSK                                                       0xffffffff
#define HWIO_EE_n_CNTXT_INT_VEC_INT_VEC_SHFT                                                              0x0

#define HWIO_EE_n_ERROR_LOG_ADDR(n)                                                                (GSI_TOP_GSI_REG_BASE      + 0x00029240 + 0x12000 * (n))
#define HWIO_EE_n_ERROR_LOG_PHYS(n)                                                                (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029240 + 0x12000 * (n))
#define HWIO_EE_n_ERROR_LOG_OFFS(n)                                                                (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029240 + 0x12000 * (n))
#define HWIO_EE_n_ERROR_LOG_RMSK                                                                   0xffffffff
#define HWIO_EE_n_ERROR_LOG_MAXn                                                                            4
#define HWIO_EE_n_ERROR_LOG_ATTR                                                                          0x3
#define HWIO_EE_n_ERROR_LOG_INI(n)        \
        in_dword_masked(HWIO_EE_n_ERROR_LOG_ADDR(n), HWIO_EE_n_ERROR_LOG_RMSK)
#define HWIO_EE_n_ERROR_LOG_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_ERROR_LOG_ADDR(n), mask)
#define HWIO_EE_n_ERROR_LOG_OUTI(n,val)    \
        out_dword(HWIO_EE_n_ERROR_LOG_ADDR(n),val)
#define HWIO_EE_n_ERROR_LOG_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_ERROR_LOG_ADDR(n),mask,val,HWIO_EE_n_ERROR_LOG_INI(n))
#define HWIO_EE_n_ERROR_LOG_EE_BMSK                                                                0xf0000000
#define HWIO_EE_n_ERROR_LOG_EE_SHFT                                                                      0x1c
#define HWIO_EE_n_ERROR_LOG_ERROR_TYPE_BMSK                                                         0xf000000
#define HWIO_EE_n_ERROR_LOG_ERROR_TYPE_SHFT                                                              0x18
#define HWIO_EE_n_ERROR_LOG_CHANNEL_EVENT_VIRTUAL_IX_BMSK                                            0xff0000
#define HWIO_EE_n_ERROR_LOG_CHANNEL_EVENT_VIRTUAL_IX_SHFT                                                0x10
#define HWIO_EE_n_ERROR_LOG_ERROR_DESCRIPTION_BMSK                                                     0xffff
#define HWIO_EE_n_ERROR_LOG_ERROR_DESCRIPTION_SHFT                                                        0x0

#define HWIO_EE_n_ERROR_LOG_CLR_ADDR(n)                                                            (GSI_TOP_GSI_REG_BASE      + 0x00029244 + 0x12000 * (n))
#define HWIO_EE_n_ERROR_LOG_CLR_PHYS(n)                                                            (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029244 + 0x12000 * (n))
#define HWIO_EE_n_ERROR_LOG_CLR_OFFS(n)                                                            (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029244 + 0x12000 * (n))
#define HWIO_EE_n_ERROR_LOG_CLR_RMSK                                                               0xffffffff
#define HWIO_EE_n_ERROR_LOG_CLR_MAXn                                                                        4
#define HWIO_EE_n_ERROR_LOG_CLR_ATTR                                                                      0x2
#define HWIO_EE_n_ERROR_LOG_CLR_OUTI(n,val)    \
        out_dword(HWIO_EE_n_ERROR_LOG_CLR_ADDR(n),val)
#define HWIO_EE_n_ERROR_LOG_CLR_ERROR_LOG_CLR_BMSK                                                 0xffffffff
#define HWIO_EE_n_ERROR_LOG_CLR_ERROR_LOG_CLR_SHFT                                                        0x0

#define HWIO_EE_n_CNTXT_SCRATCH_0_ADDR(n)                                                          (GSI_TOP_GSI_REG_BASE      + 0x00029400 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SCRATCH_0_PHYS(n)                                                          (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029400 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SCRATCH_0_OFFS(n)                                                          (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029400 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SCRATCH_0_RMSK                                                             0xffffffff
#define HWIO_EE_n_CNTXT_SCRATCH_0_MAXn                                                                      4
#define HWIO_EE_n_CNTXT_SCRATCH_0_ATTR                                                                    0x3
#define HWIO_EE_n_CNTXT_SCRATCH_0_INI(n)        \
        in_dword_masked(HWIO_EE_n_CNTXT_SCRATCH_0_ADDR(n), HWIO_EE_n_CNTXT_SCRATCH_0_RMSK)
#define HWIO_EE_n_CNTXT_SCRATCH_0_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_SCRATCH_0_ADDR(n), mask)
#define HWIO_EE_n_CNTXT_SCRATCH_0_OUTI(n,val)    \
        out_dword(HWIO_EE_n_CNTXT_SCRATCH_0_ADDR(n),val)
#define HWIO_EE_n_CNTXT_SCRATCH_0_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_CNTXT_SCRATCH_0_ADDR(n),mask,val,HWIO_EE_n_CNTXT_SCRATCH_0_INI(n))
#define HWIO_EE_n_CNTXT_SCRATCH_0_SCRATCH_BMSK                                                     0xffffffff
#define HWIO_EE_n_CNTXT_SCRATCH_0_SCRATCH_SHFT                                                            0x0

#define HWIO_EE_n_CNTXT_SCRATCH_1_ADDR(n)                                                          (GSI_TOP_GSI_REG_BASE      + 0x00029404 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SCRATCH_1_PHYS(n)                                                          (GSI_TOP_GSI_REG_BASE_PHYS + 0x00029404 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SCRATCH_1_OFFS(n)                                                          (GSI_TOP_GSI_REG_BASE_OFFS + 0x00029404 + 0x12000 * (n))
#define HWIO_EE_n_CNTXT_SCRATCH_1_RMSK                                                             0xffffffff
#define HWIO_EE_n_CNTXT_SCRATCH_1_MAXn                                                                      4
#define HWIO_EE_n_CNTXT_SCRATCH_1_ATTR                                                                    0x3
#define HWIO_EE_n_CNTXT_SCRATCH_1_INI(n)        \
        in_dword_masked(HWIO_EE_n_CNTXT_SCRATCH_1_ADDR(n), HWIO_EE_n_CNTXT_SCRATCH_1_RMSK)
#define HWIO_EE_n_CNTXT_SCRATCH_1_INMI(n,mask)    \
        in_dword_masked(HWIO_EE_n_CNTXT_SCRATCH_1_ADDR(n), mask)
#define HWIO_EE_n_CNTXT_SCRATCH_1_OUTI(n,val)    \
        out_dword(HWIO_EE_n_CNTXT_SCRATCH_1_ADDR(n),val)
#define HWIO_EE_n_CNTXT_SCRATCH_1_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_EE_n_CNTXT_SCRATCH_1_ADDR(n),mask,val,HWIO_EE_n_CNTXT_SCRATCH_1_INI(n))
#define HWIO_EE_n_CNTXT_SCRATCH_1_SCRATCH_BMSK                                                     0xffffffff
#define HWIO_EE_n_CNTXT_SCRATCH_1_SCRATCH_SHFT                                                            0x0

#define HWIO_GSI_MCS_CFG_ADDR                                                                      (GSI_TOP_GSI_REG_BASE      + 0x0000f000)
#define HWIO_GSI_MCS_CFG_PHYS                                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000f000)
#define HWIO_GSI_MCS_CFG_OFFS                                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000f000)
#define HWIO_GSI_MCS_CFG_RMSK                                                                            0xff
#define HWIO_GSI_MCS_CFG_ATTR                                                                             0x3
#define HWIO_GSI_MCS_CFG_IN          \
        in_dword_masked(HWIO_GSI_MCS_CFG_ADDR, HWIO_GSI_MCS_CFG_RMSK)
#define HWIO_GSI_MCS_CFG_INM(m)      \
        in_dword_masked(HWIO_GSI_MCS_CFG_ADDR, m)
#define HWIO_GSI_MCS_CFG_OUT(v)      \
        out_dword(HWIO_GSI_MCS_CFG_ADDR,v)
#define HWIO_GSI_MCS_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_MCS_CFG_ADDR,m,v,HWIO_GSI_MCS_CFG_IN)
#define HWIO_GSI_MCS_CFG_MCS_ENABLE_BMSK                                                                 0xff
#define HWIO_GSI_MCS_CFG_MCS_ENABLE_SHFT                                                                  0x0

#define HWIO_GSI_TZ_FW_AUTH_LOCK_ADDR                                                              (GSI_TOP_GSI_REG_BASE      + 0x0000f008)
#define HWIO_GSI_TZ_FW_AUTH_LOCK_PHYS                                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000f008)
#define HWIO_GSI_TZ_FW_AUTH_LOCK_OFFS                                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000f008)
#define HWIO_GSI_TZ_FW_AUTH_LOCK_RMSK                                                                     0x3
#define HWIO_GSI_TZ_FW_AUTH_LOCK_ATTR                                                                     0x3
#define HWIO_GSI_TZ_FW_AUTH_LOCK_IN          \
        in_dword_masked(HWIO_GSI_TZ_FW_AUTH_LOCK_ADDR, HWIO_GSI_TZ_FW_AUTH_LOCK_RMSK)
#define HWIO_GSI_TZ_FW_AUTH_LOCK_INM(m)      \
        in_dword_masked(HWIO_GSI_TZ_FW_AUTH_LOCK_ADDR, m)
#define HWIO_GSI_TZ_FW_AUTH_LOCK_OUT(v)      \
        out_dword(HWIO_GSI_TZ_FW_AUTH_LOCK_ADDR,v)
#define HWIO_GSI_TZ_FW_AUTH_LOCK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_TZ_FW_AUTH_LOCK_ADDR,m,v,HWIO_GSI_TZ_FW_AUTH_LOCK_IN)
#define HWIO_GSI_TZ_FW_AUTH_LOCK_DIS_DEBUG_SHRAM_WRITE_BMSK                                               0x2
#define HWIO_GSI_TZ_FW_AUTH_LOCK_DIS_DEBUG_SHRAM_WRITE_SHFT                                               0x1
#define HWIO_GSI_TZ_FW_AUTH_LOCK_DIS_IRAM_WRITE_BMSK                                                      0x1
#define HWIO_GSI_TZ_FW_AUTH_LOCK_DIS_IRAM_WRITE_SHFT                                                      0x0

#define HWIO_GSI_MSA_FW_AUTH_LOCK_ADDR                                                             (GSI_TOP_GSI_REG_BASE      + 0x0000f010)
#define HWIO_GSI_MSA_FW_AUTH_LOCK_PHYS                                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000f010)
#define HWIO_GSI_MSA_FW_AUTH_LOCK_OFFS                                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000f010)
#define HWIO_GSI_MSA_FW_AUTH_LOCK_RMSK                                                                    0x3
#define HWIO_GSI_MSA_FW_AUTH_LOCK_ATTR                                                                    0x3
#define HWIO_GSI_MSA_FW_AUTH_LOCK_IN          \
        in_dword_masked(HWIO_GSI_MSA_FW_AUTH_LOCK_ADDR, HWIO_GSI_MSA_FW_AUTH_LOCK_RMSK)
#define HWIO_GSI_MSA_FW_AUTH_LOCK_INM(m)      \
        in_dword_masked(HWIO_GSI_MSA_FW_AUTH_LOCK_ADDR, m)
#define HWIO_GSI_MSA_FW_AUTH_LOCK_OUT(v)      \
        out_dword(HWIO_GSI_MSA_FW_AUTH_LOCK_ADDR,v)
#define HWIO_GSI_MSA_FW_AUTH_LOCK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_MSA_FW_AUTH_LOCK_ADDR,m,v,HWIO_GSI_MSA_FW_AUTH_LOCK_IN)
#define HWIO_GSI_MSA_FW_AUTH_LOCK_DIS_DEBUG_SHRAM_WRITE_BMSK                                              0x2
#define HWIO_GSI_MSA_FW_AUTH_LOCK_DIS_DEBUG_SHRAM_WRITE_SHFT                                              0x1
#define HWIO_GSI_MSA_FW_AUTH_LOCK_DIS_IRAM_WRITE_BMSK                                                     0x1
#define HWIO_GSI_MSA_FW_AUTH_LOCK_DIS_IRAM_WRITE_SHFT                                                     0x0

#define HWIO_GSI_SP_FW_AUTH_LOCK_ADDR                                                              (GSI_TOP_GSI_REG_BASE      + 0x0000f018)
#define HWIO_GSI_SP_FW_AUTH_LOCK_PHYS                                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x0000f018)
#define HWIO_GSI_SP_FW_AUTH_LOCK_OFFS                                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x0000f018)
#define HWIO_GSI_SP_FW_AUTH_LOCK_RMSK                                                                     0x3
#define HWIO_GSI_SP_FW_AUTH_LOCK_ATTR                                                                     0x3
#define HWIO_GSI_SP_FW_AUTH_LOCK_IN          \
        in_dword_masked(HWIO_GSI_SP_FW_AUTH_LOCK_ADDR, HWIO_GSI_SP_FW_AUTH_LOCK_RMSK)
#define HWIO_GSI_SP_FW_AUTH_LOCK_INM(m)      \
        in_dword_masked(HWIO_GSI_SP_FW_AUTH_LOCK_ADDR, m)
#define HWIO_GSI_SP_FW_AUTH_LOCK_OUT(v)      \
        out_dword(HWIO_GSI_SP_FW_AUTH_LOCK_ADDR,v)
#define HWIO_GSI_SP_FW_AUTH_LOCK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GSI_SP_FW_AUTH_LOCK_ADDR,m,v,HWIO_GSI_SP_FW_AUTH_LOCK_IN)
#define HWIO_GSI_SP_FW_AUTH_LOCK_DIS_DEBUG_SHRAM_WRITE_BMSK                                               0x2
#define HWIO_GSI_SP_FW_AUTH_LOCK_DIS_DEBUG_SHRAM_WRITE_SHFT                                               0x1
#define HWIO_GSI_SP_FW_AUTH_LOCK_DIS_IRAM_WRITE_BMSK                                                      0x1
#define HWIO_GSI_SP_FW_AUTH_LOCK_DIS_IRAM_WRITE_SHFT                                                      0x0

#define HWIO_INTER_EE_n_ORIGINATOR_EE_ADDR(n)                                                      (GSI_TOP_GSI_REG_BASE      + 0x00010000 + 0x1000 * (n))
#define HWIO_INTER_EE_n_ORIGINATOR_EE_PHYS(n)                                                      (GSI_TOP_GSI_REG_BASE_PHYS + 0x00010000 + 0x1000 * (n))
#define HWIO_INTER_EE_n_ORIGINATOR_EE_OFFS(n)                                                      (GSI_TOP_GSI_REG_BASE_OFFS + 0x00010000 + 0x1000 * (n))
#define HWIO_INTER_EE_n_ORIGINATOR_EE_RMSK                                                                0xf
#define HWIO_INTER_EE_n_ORIGINATOR_EE_MAXn                                                                  4
#define HWIO_INTER_EE_n_ORIGINATOR_EE_ATTR                                                                0x3
#define HWIO_INTER_EE_n_ORIGINATOR_EE_INI(n)        \
        in_dword_masked(HWIO_INTER_EE_n_ORIGINATOR_EE_ADDR(n), HWIO_INTER_EE_n_ORIGINATOR_EE_RMSK)
#define HWIO_INTER_EE_n_ORIGINATOR_EE_INMI(n,mask)    \
        in_dword_masked(HWIO_INTER_EE_n_ORIGINATOR_EE_ADDR(n), mask)
#define HWIO_INTER_EE_n_ORIGINATOR_EE_OUTI(n,val)    \
        out_dword(HWIO_INTER_EE_n_ORIGINATOR_EE_ADDR(n),val)
#define HWIO_INTER_EE_n_ORIGINATOR_EE_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_INTER_EE_n_ORIGINATOR_EE_ADDR(n),mask,val,HWIO_INTER_EE_n_ORIGINATOR_EE_INI(n))
#define HWIO_INTER_EE_n_ORIGINATOR_EE_EE_NUMBER_BMSK                                                      0xf
#define HWIO_INTER_EE_n_ORIGINATOR_EE_EE_NUMBER_SHFT                                                      0x0

#define HWIO_INTER_EE_n_GSI_CH_CMD_ADDR(n)                                                         (GSI_TOP_GSI_REG_BASE      + 0x00010008 + 0x1000 * (n))
#define HWIO_INTER_EE_n_GSI_CH_CMD_PHYS(n)                                                         (GSI_TOP_GSI_REG_BASE_PHYS + 0x00010008 + 0x1000 * (n))
#define HWIO_INTER_EE_n_GSI_CH_CMD_OFFS(n)                                                         (GSI_TOP_GSI_REG_BASE_OFFS + 0x00010008 + 0x1000 * (n))
#define HWIO_INTER_EE_n_GSI_CH_CMD_RMSK                                                            0xff0000ff
#define HWIO_INTER_EE_n_GSI_CH_CMD_MAXn                                                                     4
#define HWIO_INTER_EE_n_GSI_CH_CMD_ATTR                                                                   0x2
#define HWIO_INTER_EE_n_GSI_CH_CMD_OUTI(n,val)    \
        out_dword(HWIO_INTER_EE_n_GSI_CH_CMD_ADDR(n),val)
#define HWIO_INTER_EE_n_GSI_CH_CMD_OPCODE_BMSK                                                     0xff000000
#define HWIO_INTER_EE_n_GSI_CH_CMD_OPCODE_SHFT                                                           0x18
#define HWIO_INTER_EE_n_GSI_CH_CMD_OPCODE_START_FVAL                                                      0x1
#define HWIO_INTER_EE_n_GSI_CH_CMD_OPCODE_STOP_FVAL                                                       0x2
#define HWIO_INTER_EE_n_GSI_CH_CMD_OPCODE_RESET_FVAL                                                      0x9
#define HWIO_INTER_EE_n_GSI_CH_CMD_OPCODE_DE_ALLOC_FVAL                                                   0xa
#define HWIO_INTER_EE_n_GSI_CH_CMD_OPCODE_DB_STOP_FVAL                                                    0xb
#define HWIO_INTER_EE_n_GSI_CH_CMD_CHID_BMSK                                                             0xff
#define HWIO_INTER_EE_n_GSI_CH_CMD_CHID_SHFT                                                              0x0

#define HWIO_INTER_EE_n_EV_CH_CMD_ADDR(n)                                                          (GSI_TOP_GSI_REG_BASE      + 0x00010010 + 0x1000 * (n))
#define HWIO_INTER_EE_n_EV_CH_CMD_PHYS(n)                                                          (GSI_TOP_GSI_REG_BASE_PHYS + 0x00010010 + 0x1000 * (n))
#define HWIO_INTER_EE_n_EV_CH_CMD_OFFS(n)                                                          (GSI_TOP_GSI_REG_BASE_OFFS + 0x00010010 + 0x1000 * (n))
#define HWIO_INTER_EE_n_EV_CH_CMD_RMSK                                                             0xff0000ff
#define HWIO_INTER_EE_n_EV_CH_CMD_MAXn                                                                      4
#define HWIO_INTER_EE_n_EV_CH_CMD_ATTR                                                                    0x2
#define HWIO_INTER_EE_n_EV_CH_CMD_OUTI(n,val)    \
        out_dword(HWIO_INTER_EE_n_EV_CH_CMD_ADDR(n),val)
#define HWIO_INTER_EE_n_EV_CH_CMD_OPCODE_BMSK                                                      0xff000000
#define HWIO_INTER_EE_n_EV_CH_CMD_OPCODE_SHFT                                                            0x18
#define HWIO_INTER_EE_n_EV_CH_CMD_OPCODE_RESET_FVAL                                                       0x9
#define HWIO_INTER_EE_n_EV_CH_CMD_OPCODE_DE_ALLOC_FVAL                                                    0xa
#define HWIO_INTER_EE_n_EV_CH_CMD_CHID_BMSK                                                              0xff
#define HWIO_INTER_EE_n_EV_CH_CMD_CHID_SHFT                                                               0x0

#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_ADDR(n,k)                                                 (GSI_TOP_GSI_REG_BASE      + 0x00010018 + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_PHYS(n,k)                                                 (GSI_TOP_GSI_REG_BASE_PHYS + 0x00010018 + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_OFFS(n,k)                                                 (GSI_TOP_GSI_REG_BASE_OFFS + 0x00010018 + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_RMSK                                                      0xffffffff
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_MAXn                                                               4
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_MAXk                                                               2
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_ATTR                                                             0x1
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_INI2(n,k)        \
        in_dword_masked(HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_ADDR(n,k), HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_RMSK)
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_ADDR(n,k), mask)
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_GSI_CH_BIT_MAP_BMSK                                       0xffffffff
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_k_GSI_CH_BIT_MAP_SHFT                                              0x0

#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_ADDR(n,k)                                             (GSI_TOP_GSI_REG_BASE      + 0x0001001c + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_PHYS(n,k)                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x0001001c + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_OFFS(n,k)                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x0001001c + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_RMSK                                                  0xffffffff
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_MAXn                                                           4
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_MAXk                                                           2
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_ATTR                                                         0x3
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_INI2(n,k)        \
        in_dword_masked(HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_ADDR(n,k), HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_RMSK)
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_ADDR(n,k), mask)
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_OUTI2(n,k,val)    \
        out_dword(HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_ADDR(n,k),val)
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_ADDR(n,k),mask,val,HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_INI2(n,k))
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_GSI_CH_BIT_MAP_MSK_BMSK                               0xffffffff
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_MSK_k_GSI_CH_BIT_MAP_MSK_SHFT                                      0x0

#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_CLR_k_ADDR(n,k)                                             (GSI_TOP_GSI_REG_BASE      + 0x00010020 + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_CLR_k_PHYS(n,k)                                             (GSI_TOP_GSI_REG_BASE_PHYS + 0x00010020 + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_CLR_k_OFFS(n,k)                                             (GSI_TOP_GSI_REG_BASE_OFFS + 0x00010020 + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_CLR_k_RMSK                                                  0xffffffff
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_CLR_k_MAXn                                                           4
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_CLR_k_MAXk                                                           2
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_CLR_k_ATTR                                                         0x2
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_CLR_k_OUTI2(n,k,val)    \
        out_dword(HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_CLR_k_ADDR(n,k),val)
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_CLR_k_GSI_CH_BIT_MAP_BMSK                                   0xffffffff
#define HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_CLR_k_GSI_CH_BIT_MAP_SHFT                                          0x0

#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_ADDR(n,k)                                                  (GSI_TOP_GSI_REG_BASE      + 0x00010024 + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_PHYS(n,k)                                                  (GSI_TOP_GSI_REG_BASE_PHYS + 0x00010024 + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_OFFS(n,k)                                                  (GSI_TOP_GSI_REG_BASE_OFFS + 0x00010024 + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_RMSK                                                       0xffffffff
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_MAXn                                                                4
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_MAXk                                                                1
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_ATTR                                                              0x1
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_INI2(n,k)        \
        in_dword_masked(HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_ADDR(n,k), HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_RMSK)
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_ADDR(n,k), mask)
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_EV_CH_BIT_MAP_BMSK                                         0xffffffff
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_k_EV_CH_BIT_MAP_SHFT                                                0x0

#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_ADDR(n,k)                                              (GSI_TOP_GSI_REG_BASE      + 0x00010028 + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_PHYS(n,k)                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x00010028 + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_OFFS(n,k)                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x00010028 + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_RMSK                                                   0xffffffff
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_MAXn                                                            4
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_MAXk                                                            1
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_ATTR                                                          0x3
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_INI2(n,k)        \
        in_dword_masked(HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_ADDR(n,k), HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_RMSK)
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_INMI2(n,k,mask)    \
        in_dword_masked(HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_ADDR(n,k), mask)
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_OUTI2(n,k,val)    \
        out_dword(HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_ADDR(n,k),val)
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_OUTMI2(n,k,mask,val) \
        out_dword_masked_ns(HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_ADDR(n,k),mask,val,HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_INI2(n,k))
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_EV_CH_BIT_MAP_MSK_BMSK                                 0xffffffff
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_MSK_k_EV_CH_BIT_MAP_MSK_SHFT                                        0x0

#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_CLR_k_ADDR(n,k)                                              (GSI_TOP_GSI_REG_BASE      + 0x0001002c + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_CLR_k_PHYS(n,k)                                              (GSI_TOP_GSI_REG_BASE_PHYS + 0x0001002c + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_CLR_k_OFFS(n,k)                                              (GSI_TOP_GSI_REG_BASE_OFFS + 0x0001002c + 0x18 * (k) + 0x1000 * (n))
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_CLR_k_RMSK                                                   0xffffffff
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_CLR_k_MAXn                                                            4
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_CLR_k_MAXk                                                            1
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_CLR_k_ATTR                                                          0x2
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_CLR_k_OUTI2(n,k,val)    \
        out_dword(HWIO_INTER_EE_n_SRC_EV_CH_IRQ_CLR_k_ADDR(n,k),val)
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_CLR_k_EV_CH_BIT_MAP_BMSK                                     0xffffffff
#define HWIO_INTER_EE_n_SRC_EV_CH_IRQ_CLR_k_EV_CH_BIT_MAP_SHFT                                            0x0


#endif /* __GSI_HWIO_H__ */
