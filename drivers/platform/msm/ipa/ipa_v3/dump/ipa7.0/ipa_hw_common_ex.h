// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.​
 */
#if !defined(_IPA_HW_COMMON_EX_H_)
#define _IPA_HW_COMMON_EX_H_

/* VLVL defs are available for 854 */
#define FEATURE_VLVL_DEFS                            true

/* Important Platform Specific Values : IRQ_NUM, IRQ_CNT, BCR */
#define IPA_HW_BAM_IRQ_NUM                           639

/* Q6 IRQ number for IPA. */
#define IPA_HW_IRQ_NUM                               640

/* Total number of different interrupts that can be enabled */
#define IPA_HW_IRQ_CNT_TOTAL                         23

/* IPAv4 spare reg value */
#define IPA_HW_SPARE_1_REG_VAL                       0xC0000005

/* Whether to allow setting step mode on IPA when we crash or not */
#define IPA_CFG_HW_IS_STEP_MODE_ALLOWED              (false)

/* GSI MHI related definitions */
#define IPA_HW_GSI_MHI_CONSUMER_CHANNEL_NUM          0x0
#define IPA_HW_GSI_MHI_PRODUCER_CHANNEL_NUM          0x1

#define IPA_HW_GSI_MHI_CONSUMER_EP_NUM               0x1
#define IPA_HW_GSI_MHI_PRODUCER_EP_NUM               0x11

/* IPA ZIP WA related Macros */
#define IPA_HW_DCMP_SRC_PIPE                         0x8
#define IPA_HW_DCMP_DEST_PIPE                        0x4
#define IPA_HW_ACK_MNGR_MASK                         0x1D
#define IPA_HW_DCMP_SRC_GRP                          0x5

/* IPA Clock resource name */
#define IPA_CLK_RESOURCE_NAME                        "/clk/pcnoc"

/* IPA Clock Bus Client name */
#define IPA_CLK_BUS_CLIENT_NAME                      "IPA_PCNOC_BUS_CLIENT"

/* HPS Sequences */
#define IPA_HW_PKT_PROCESS_HPS_DMA                      0x0
#define IPA_HW_PKT_PROCESS_HPS_DMA_DECIPH_CIPHE         0x1
#define IPA_HW_PKT_PROCESS_HPS_PKT_PRS_NO_DECIPH_UCP    0x2
#define IPA_HW_PKT_PROCESS_HPS_PKT_PRS_DECIPH_UCP       0x3
#define IPA_HW_PKT_PROCESS_HPS_2_PKT_PRS_NO_DECIPH      0x4
#define IPA_HW_PKT_PROCESS_HPS_2_PKT_PRS_DECIPH         0x5
#define IPA_HW_PKT_PROCESS_HPS_PKT_PRS_NO_DECIPH_NO_UCP 0x6
#define IPA_HW_PKT_PROCESS_HPS_PKT_PRS_DECIPH_NO_UCP    0x7
#define IPA_HW_PKT_PROCESS_HPS_DMA_PARSER               0x8
#define IPA_HW_PKT_PROCESS_HPS_DMA_DECIPH_PARSER        0x9
#define IPA_HW_PKT_PROCESS_HPS_2_PKT_PRS_UCP_TWICE_NO_DECIPH  0xA
#define IPA_HW_PKT_PROCESS_HPS_2_PKT_PRS_UCP_TWICE_DECIPH     0xB
#define IPA_HW_PKT_PROCESS_HPS_3_PKT_PRS_UCP_TWICE_NO_DECIPH  0xC
#define IPA_HW_PKT_PROCESS_HPS_3_PKT_PRS_UCP_TWICE_DECIPH     0xD
#define IPA_HW_PKT_PROCESS_HPS_PKT_PRS_NO_DECIPH_UCP_DRB_IP   0xF
#define IPA_HW_PKT_PROCESS_HPS_2_PKT_PRS_DECIPH_DRB_IP        0x10
#define IPA_HW_PKT_PROCESS_HPS_PKT_PRS_DRB_IP_NO_UCP          0x11
#define IPA_HW_PKT_PROCESS_HPS_DMA_DRB_IP_PARSER              0x12
#define IPA_HW_PKT_PROCESS_HPS_2_PKT_PRS_UCP_TWICE_DRB_IP     0x13
#define IPA_HW_PKT_PROCESS_HPS_3_PKT_PRS_UCP_TWICE_DRB_IP     0x14
#define IPA_HW_PKT_PROCESS_HPS_DMA_UCP                        0x15
#define IPA_HW_PKT_PROCESS_HPS_DMA_DRB_IP_UCP                 0x16
#define IPA_HW_PKT_PROCESS_HPS_DMA_UCP_1RND_IP_PROC           0x17
#define IPA_HW_PKT_PROCESS_HPS_DMA_DRB_IP_UCP_1RND_IP_PROC    0x18
#define IPA_HW_PKT_PROCESS_HPS_2_PKT_PRS_UCP_TWICE_ENCAPS_DRBIP 0x19
#define IPA_HW_PKT_PROCESS_HPS_3RD_PKT_PROCESS_PASS_2ND_UCP_DECAPS_DRBIP 0x1A
#define IPA_HW_PKT_PROCESS_HPS_3RD_PKT_PROCESS_PASS_2ND_UCP_ENCAPS_DRBIP 0x1B
#define IPA_HW_PKT_PROCESS_HPS_2_PKT_PRS_UCP_ONCE_DECAPS_DRBIP 0x1C
#define IPA_HW_PKT_PROCESS_HPS_DMA_CRYPTO_IN2RND_DRBIP_UCP    0x1D

/* DPS Sequences */
#define IPA_HW_PKT_PROCESS_DPS_DMA                      0x0
#define IPA_HW_PKT_PROCESS_DPS_DMA_WITH_DECIPH          0x1
#define IPA_HW_PKT_PROCESS_DPS_DMA_WITH_DECOMP          0x2
#define IPA_HW_PKT_PROCESS_DPS_DMA_WITH_CIPH            0x3

#define IPA_HW_RSRP_GRP_0                            0x0
#define IPA_HW_RSRP_GRP_1                            0x1
#define IPA_HW_RSRP_GRP_2                            0x2
#define IPA_HW_RSRP_GRP_3                            0x3
#define IPA_HW_RSRP_GRP_4                            0x4
#define IPA_HW_RSRP_GRP_5                            0x5
#define IPA_HW_RSRP_GRP_6                            0x6
#define IPA_HW_RSRP_GRP_7                            0x7
#define IPA_HW_RSRP_GRP_8                            0x8
#define IPA_HW_RSRP_GRP_9                            0x9

#define IPA_HW_PCIE_SRC_RSRP_GRP                     IPA_HW_RSRP_GRP_0
#define IPA_HW_PCIE_DEST_RSRP_GRP                    IPA_HW_RSRP_GRP_0

#define IPA_HW_DDR_SRC_RSRP_GRP                      IPA_HW_RSRP_GRP_1
#define IPA_HW_DDR_DEST_RSRP_GRP                     IPA_HW_RSRP_GRP_1

#define IPA_HW_DMA_SRC_RSRP_GRP                      IPA_HW_RSRP_GRP_2
#define IPA_HW_DMA_DEST_RSRP_GRP                     IPA_HW_RSRP_GRP_2

#define IPA_HW_SRC_RSRP_TYPE_MAX 0x05
#define IPA_HW_DST_RSRP_TYPE_MAX 0x03

#define IPA_HW_SRC_RSRP_GRP_MAX 0x06
#define IPA_HW_DST_RSRP_GRP_MAX 0x08

#define GSI_HW_QSB_LOG_MISC_MAX 0x4

/* IPA Clock Bus Client name */
#define IPA_CLK_BUS_CLIENT_NAME                      "IPA_PCNOC_BUS_CLIENT"

/* Is IPA decompression feature enabled */
#define IPA_HW_IS_DECOMPRESSION_ENABLED              (1)

/* Whether to allow setting step mode on IPA when we crash or not */
#define IPA_HW_IS_STEP_MODE_ALLOWED                  (true)

/* Max number of virtual pipes for UL QBAP provided by HW */
#define IPA_HW_MAX_VP_NUM                             (32)

/*
 * HW specific clock vote freq values in KHz
 * (BIMC/SNOC/PCNOC/IPA/Q6 CPU)
 */
enum ipa_hw_clk_freq_e {
	/* BIMC */
	IPA_HW_CLK_FREQ_BIMC_PEAK       = 518400,
	IPA_HW_CLK_FREQ_BIMC_NOM_PLUS   = 404200,
	IPA_HW_CLK_FREQ_BIMC_NOM        = 404200,
	IPA_HW_CLK_FREQ_BIMC_SVS        = 100000,

	/* PCNOC */
	IPA_HW_CLK_FREQ_PCNOC_PEAK      = 133330,
	IPA_HW_CLK_FREQ_PCNOC_NOM_PLUS  = 100000,
	IPA_HW_CLK_FREQ_PCNOC_NOM       = 100000,
	IPA_HW_CLK_FREQ_PCNOC_SVS       = 50000,

	/*IPA_HW_CLK_SNOC*/
	IPA_HW_CLK_FREQ_SNOC_PEAK       = 200000,
	IPA_HW_CLK_FREQ_SNOC_NOM_PLUS   = 150000,
	IPA_HW_CLK_FREQ_SNOC_NOM        = 150000,
	IPA_HW_CLK_FREQ_SNOC_SVS        = 85000,
	IPA_HW_CLK_FREQ_SNOC_SVS_2      = 50000,

	/* IPA */
	IPA_HW_CLK_FREQ_IPA_PEAK        = 600000,
	IPA_HW_CLK_FREQ_IPA_NOM_PLUS    = 500000,
	IPA_HW_CLK_FREQ_IPA_NOM         = 500000,
	IPA_HW_CLK_FREQ_IPA_SVS         = 250000,
	IPA_HW_CLK_FREQ_IPA_SVS_2       = 150000,

	/* Q6 CPU */
	IPA_HW_CLK_FREQ_Q6_PEAK         = 729600,
	IPA_HW_CLK_FREQ_Q6_NOM_PLUS     = 729600,
	IPA_HW_CLK_FREQ_Q6_NOM          = 729600,
	IPA_HW_CLK_FREQ_Q6_SVS          = 729600,
};

enum ipa_hw_qtimer_gran_e {
	IPA_HW_QTIMER_GRAN_0 = 0, /* granularity 0 is 10us */
	IPA_HW_QTIMER_GRAN_1 = 1, /* granularity 1 is 100us */
	IPA_HW_QTIMER_GRAN_MAX,
};

/* Pipe ID of all the IPA pipes */
enum ipa_hw_pipe_id_e {
	IPA_HW_PIPE_ID_0,
	IPA_HW_PIPE_ID_1,
	IPA_HW_PIPE_ID_2,
	IPA_HW_PIPE_ID_3,
	IPA_HW_PIPE_ID_4,
	IPA_HW_PIPE_ID_5,
	IPA_HW_PIPE_ID_6,
	IPA_HW_PIPE_ID_7,
	IPA_HW_PIPE_ID_8,
	IPA_HW_PIPE_ID_9,
	IPA_HW_PIPE_ID_10,
	IPA_HW_PIPE_ID_11,
	IPA_HW_PIPE_ID_12,
	IPA_HW_PIPE_ID_13,
	IPA_HW_PIPE_ID_14,
	IPA_HW_PIPE_ID_15,
	IPA_HW_PIPE_ID_16,
	IPA_HW_PIPE_ID_17,
	IPA_HW_PIPE_ID_18,
	IPA_HW_PIPE_ID_19,
	IPA_HW_PIPE_ID_20,
	IPA_HW_PIPE_ID_21,
	IPA_HW_PIPE_ID_22,
	IPA_HW_PIPE_ID_23,
	IPA_HW_PIPE_ID_24,
	IPA_HW_PIPE_ID_25,
	IPA_HW_PIPE_ID_26,
	IPA_HW_PIPE_ID_27,
	IPA_HW_PIPE_ID_28,
	IPA_HW_PIPE_ID_29,
	IPA_HW_PIPE_ID_30,
	IPA_HW_PIPE_ID_31,
	IPA_HW_PIPE_ID_32,
	IPA_HW_PIPE_ID_33,
	IPA_HW_PIPE_ID_34,
	IPA_HW_PIPE_ID_35,
	IPA_HW_PIPE_ID_36,
	IPA_HW_PIPE_ID_37,
	IPA_HW_PIPE_ID_38,
	IPA_HW_PIPE_ID_39,
	IPA_HW_PIPE_ID_40,
	IPA_HW_PIPE_ID_41,
	IPA_HW_PIPE_ID_42,
	IPA_HW_PIPE_ID_43,
	IPA_HW_PIPE_ID_44,
	IPA_HW_PIPE_ID_45,
	IPA_HW_PIPE_ID_46,
	IPA_HW_PIPE_ID_47,
	IPA_HW_PIPE_ID_48,
	IPA_HW_PIPE_ID_49,
	IPA_HW_PIPE_ID_50,
	IPA_HW_PIPE_ID_51,
	IPA_HW_PIPE_ID_52,
	IPA_HW_PIPE_ID_53,
	IPA_HW_PIPE_ID_54,
	IPA_HW_PIPE_ID_55,
	IPA_HW_PIPE_ID_56,
	IPA_HW_PIPE_ID_57,
	IPA_HW_PIPE_ID_58,
	IPA_HW_PIPE_ID_59,
	IPA_HW_PIPE_ID_60,
	IPA_HW_PIPE_ID_61,
	IPA_HW_PIPE_ID_62,
	IPA_HW_PIPE_ID_63,
	IPA_HW_PIPE_ID_64,
	IPA_HW_PIPE_ID_65,
	IPA_HW_PIPE_ID_66,
	IPA_HW_PIPE_ID_67,
	IPA_HW_PIPE_ID_68,
	IPA_HW_PIPE_ID_69,
	IPA_HW_PIPE_ID_70,
	IPA_HW_PIPE_ID_71,
	IPA_HW_PIPE_ID_72,
	IPA_HW_PIPE_ID_73,
	IPA_HW_PIPE_ID_74,
	IPA_HW_PIPE_ID_MAX
};

/*
 * All the IRQ's supported by the IPA HW. Use this enum to set IRQ_EN
 * register and read IRQ_STTS register
 */
enum ipa_hw_irq_e {
	IPA_HW_IRQ_GSI_HWP                     = (1 << 25),
	IPA_HW_IRQ_GSI_IPA_IF_TLV_RCVD         = (1 << 24),
	IPA_HW_IRQ_GSI_EE_IRQ                  = (1 << 23),
	IPA_HW_IRQ_DCMP_ERR                    = (1 << 22),
	IPA_HW_IRQ_HWP_ERR                     = (1 << 21),
	IPA_HW_IRQ_RED_MARKER_ABOVE            = (1 << 20),
	IPA_HW_IRQ_YELLOW_MARKER_ABOVE         = (1 << 19),
	IPA_HW_IRQ_RED_MARKER_BELOW            = (1 << 18),
	IPA_HW_IRQ_YELLOW_MARKER_BELOW         = (1 << 17),
	IPA_HW_IRQ_BAM_IDLE_IRQ                = (1 << 16),
	IPA_HW_IRQ_TX_HOLB_DROP                = (1 << 15),
	IPA_HW_IRQ_TX_SUSPEND                  = (1 << 14),
	IPA_HW_IRQ_PROC_ERR                    = (1 << 13),
	IPA_HW_IRQ_STEP_MODE                   = (1 << 12),
	IPA_HW_IRQ_TX_ERR                      = (1 << 11),
	IPA_HW_IRQ_DEAGGR_ERR                  = (1 << 10),
	IPA_HW_IRQ_RX_ERR                      = (1 << 9),
	IPA_HW_IRQ_PROC_TO_HW_ACK_Q_NOT_EMPTY  = (1 << 8),
	IPA_HW_IRQ_HWP_RX_CMD_Q_NOT_FULL       = (1 << 7),
	IPA_HW_IRQ_HWP_IN_Q_NOT_EMPTY          = (1 << 6),
	IPA_HW_IRQ_HWP_IRQ_3                   = (1 << 5),
	IPA_HW_IRQ_HWP_IRQ_2                   = (1 << 4),
	IPA_HW_IRQ_HWP_IRQ_1                   = (1 << 3),
	IPA_HW_IRQ_HWP_IRQ_0                   = (1 << 2),
	IPA_HW_IRQ_EOT_COAL                    = (1 << 1),
	IPA_HW_IRQ_BAD_SNOC_ACCESS             = (1 << 0),
	IPA_HW_IRQ_NONE                        = 0,
	IPA_HW_IRQ_ALL                         = 0xFFFFFFFF
};

/*
 * All the IRQ sources supported by the IPA HW. Use this enum to set
 * IRQ_SRCS register
 */
enum ipa_hw_irq_srcs_e {
	IPA_HW_IRQ_SRCS_PIPE_0  = (1 << IPA_HW_PIPE_ID_0),
	IPA_HW_IRQ_SRCS_PIPE_1  = (1 << IPA_HW_PIPE_ID_1),
	IPA_HW_IRQ_SRCS_PIPE_2  = (1 << IPA_HW_PIPE_ID_2),
	IPA_HW_IRQ_SRCS_PIPE_3  = (1 << IPA_HW_PIPE_ID_3),
	IPA_HW_IRQ_SRCS_PIPE_4  = (1 << IPA_HW_PIPE_ID_4),
	IPA_HW_IRQ_SRCS_PIPE_5  = (1 << IPA_HW_PIPE_ID_5),
	IPA_HW_IRQ_SRCS_PIPE_6  = (1 << IPA_HW_PIPE_ID_6),
	IPA_HW_IRQ_SRCS_PIPE_7  = (1 << IPA_HW_PIPE_ID_7),
	IPA_HW_IRQ_SRCS_PIPE_8  = (1 << IPA_HW_PIPE_ID_8),
	IPA_HW_IRQ_SRCS_PIPE_9  = (1 << IPA_HW_PIPE_ID_9),
	IPA_HW_IRQ_SRCS_PIPE_10 = (1 << IPA_HW_PIPE_ID_10),
	IPA_HW_IRQ_SRCS_PIPE_11 = (1 << IPA_HW_PIPE_ID_11),
	IPA_HW_IRQ_SRCS_PIPE_12 = (1 << IPA_HW_PIPE_ID_12),
	IPA_HW_IRQ_SRCS_PIPE_13 = (1 << IPA_HW_PIPE_ID_13),
	IPA_HW_IRQ_SRCS_PIPE_14 = (1 << IPA_HW_PIPE_ID_14),
	IPA_HW_IRQ_SRCS_PIPE_15 = (1 << IPA_HW_PIPE_ID_15),
	IPA_HW_IRQ_SRCS_PIPE_16 = (1 << IPA_HW_PIPE_ID_16),
	IPA_HW_IRQ_SRCS_PIPE_17 = (1 << IPA_HW_PIPE_ID_17),
	IPA_HW_IRQ_SRCS_PIPE_18 = (1 << IPA_HW_PIPE_ID_18),
	IPA_HW_IRQ_SRCS_PIPE_19 = (1 << IPA_HW_PIPE_ID_19),
	IPA_HW_IRQ_SRCS_PIPE_20 = (1 << IPA_HW_PIPE_ID_20),
	IPA_HW_IRQ_SRCS_PIPE_21 = (1 << IPA_HW_PIPE_ID_21),
	IPA_HW_IRQ_SRCS_PIPE_22 = (1 << IPA_HW_PIPE_ID_22),
	IPA_HW_IRQ_SRCS_NONE    = 0,
	IPA_HW_IRQ_SRCS_ALL     = 0xFFFFFFFF,
};

/*
 * Total number of channel contexts that need to be saved for APPS
 */
#define IPA_HW_REG_SAVE_GSI_NUM_CH_CNTXT_A7          63

/*
 * Total number of channel contexts that need to be saved for UC
 */
#define IPA_HW_REG_SAVE_GSI_NUM_CH_CNTXT_UC          2

 /*
 * Total number of channel contexts that need to be saved for Q6
 */
#define IPA_HW_REG_SAVE_GSI_NUM_CH_CNTXT_Q6          14

/*
 * Total number of event ring contexts that need to be saved for APPS
 */
#define IPA_HW_REG_SAVE_GSI_NUM_EVT_CNTXT_A7         63

/*
 * Total number of event ring contexts that need to be saved for UC
 */
#define IPA_HW_REG_SAVE_GSI_NUM_EVT_CNTXT_UC         2

/*
 * Total number of event ring contexts that need to be saved for Q6
 */
#define IPA_HW_REG_SAVE_GSI_NUM_EVT_CNTXT_Q6         14

/*
 * Total number of endpoints for which ipa_reg_save.pipes[endp_number]
 * are not saved by default (only if ipa_cfg.gen.full_reg_trace =
 * true) There is no extra endpoints in Stingray
 */
#define IPA_HW_REG_SAVE_NUM_ENDP_EXTRA               0

/*
 * Total number of endpoints for which ipa_reg_save.pipes[endp_number]
 * are always saved
 */
#define IPA_HW_REG_SAVE_NUM_ACTIVE_PIPES             IPA_HW_PIPE_ID_MAX

/*
 * SHRAM Bytes per ch
 */
#define IPA_REG_SAVE_BYTES_PER_CHNL_SHRAM         20

/*
 * Although not necessary for the numbers below, the use of round_up
 * is so that future developers know that these particular constants
 * have to be a multiple of four bytes, because the IPA memory reads
 * that they drive are always 32 bits...
 */
#define IPA_IU_ADDR   0x001A0000
#define IPA_IU_SIZE   round_up(40704, sizeof(u32))

#define IPA_SRAM_ADDR 0x00150000
#define IPA_SRAM_SIZE round_up(19232, sizeof(u32))

#define IPA_MBOX_ADDR 0x001C2000
#define IPA_MBOX_SIZE round_up(256, sizeof(u32))

#define IPA_HRAM_ADDR 0x00160000
#define IPA_HRAM_SIZE round_up(47536, sizeof(u32))

#define IPA_SEQ_ADDR  0x00181000
#define IPA_SEQ_SIZE  round_up(768, sizeof(u32))

#define IPA_GSI_ADDR  0x00006000
#define IPA_GSI_SIZE  round_up(5376, sizeof(u32))

/*
 * Macro to define a particular register cfg entry for consumer pipe
 * indexed register
 */
#define IPA_REG_SAVE_CFG_ENTRY_PROD_PIPE_ENDP(reg_name, var_name)	\
	{ GEN_1xVECTOR_REG_OFST(reg_name, 0), \
		(u32 *)&ipa_reg_save.ipa.pipes[0].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 1), \
		(u32 *)&ipa_reg_save.ipa.pipes[1].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 2), \
		(u32 *)&ipa_reg_save.ipa.pipes[2].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 3), \
		(u32 *)&ipa_reg_save.ipa.pipes[3].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 4), \
		(u32 *)&ipa_reg_save.ipa.pipes[4].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 5), \
		(u32 *)&ipa_reg_save.ipa.pipes[5].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 6), \
		(u32 *)&ipa_reg_save.ipa.pipes[6].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 7), \
		(u32 *)&ipa_reg_save.ipa.pipes[7].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 8), \
		(u32 *)&ipa_reg_save.ipa.pipes[8].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 9), \
		(u32 *)&ipa_reg_save.ipa.pipes[9].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 10), \
		(u32 *)&ipa_reg_save.ipa.pipes[10].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 11), \
		(u32 *)&ipa_reg_save.ipa.pipes[11].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 12), \
		(u32 *)&ipa_reg_save.ipa.pipes[12].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 13), \
		(u32 *)&ipa_reg_save.ipa.pipes[13].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 14), \
		(u32 *)&ipa_reg_save.ipa.pipes[14].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 15), \
		(u32 *)&ipa_reg_save.ipa.pipes[15].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 16), \
		(u32 *)&ipa_reg_save.ipa.pipes[16].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 17), \
		(u32 *)&ipa_reg_save.ipa.pipes[17].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 18), \
		(u32 *)&ipa_reg_save.ipa.pipes[18].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 19), \
		(u32 *)&ipa_reg_save.ipa.pipes[19].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 20), \
		(u32 *)&ipa_reg_save.ipa.pipes[20].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 21), \
		(u32 *)&ipa_reg_save.ipa.pipes[21].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 22), \
		(u32 *)&ipa_reg_save.ipa.pipes[22].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 23), \
		(u32 *)&ipa_reg_save.ipa.pipes[23].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 24), \
		(u32 *)&ipa_reg_save.ipa.pipes[24].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 25), \
		(u32 *)&ipa_reg_save.ipa.pipes[25].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 26), \
		(u32 *)&ipa_reg_save.ipa.pipes[26].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 27), \
		(u32 *)&ipa_reg_save.ipa.pipes[27].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 28), \
		(u32 *)&ipa_reg_save.ipa.pipes[28].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 29), \
		(u32 *)&ipa_reg_save.ipa.pipes[29].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 30), \
		(u32 *)&ipa_reg_save.ipa.pipes[30].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 31), \
		(u32 *)&ipa_reg_save.ipa.pipes[31].endp.var_name, \
		GEN_REG_ATTR(reg_name) }

/*
 * Macro to define a particular register cfg entry for producer pipe
 * indexed register
 */
#define IPA_REG_SAVE_CFG_ENTRY_CONS_PIPE_ENDP(reg_name, var_name)	\
	{ GEN_1xVECTOR_REG_OFST(reg_name, 32), \
		(u32 *)&ipa_reg_save.ipa.pipes[32].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 33), \
		(u32 *)&ipa_reg_save.ipa.pipes[33].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 34), \
		(u32 *)&ipa_reg_save.ipa.pipes[34].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 35), \
		(u32 *)&ipa_reg_save.ipa.pipes[35].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 36), \
		(u32 *)&ipa_reg_save.ipa.pipes[36].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 37), \
		(u32 *)&ipa_reg_save.ipa.pipes[37].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 38), \
		(u32 *)&ipa_reg_save.ipa.pipes[38].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 39), \
		(u32 *)&ipa_reg_save.ipa.pipes[39].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 40), \
		(u32 *)&ipa_reg_save.ipa.pipes[40].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 41), \
		(u32 *)&ipa_reg_save.ipa.pipes[41].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 42), \
		(u32 *)&ipa_reg_save.ipa.pipes[42].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 43), \
		(u32 *)&ipa_reg_save.ipa.pipes[43].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 44), \
		(u32 *)&ipa_reg_save.ipa.pipes[44].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 45), \
		(u32 *)&ipa_reg_save.ipa.pipes[45].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 46), \
		(u32 *)&ipa_reg_save.ipa.pipes[46].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 47), \
		(u32 *)&ipa_reg_save.ipa.pipes[47].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 48), \
		(u32 *)&ipa_reg_save.ipa.pipes[48].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 49), \
		(u32 *)&ipa_reg_save.ipa.pipes[49].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 50), \
	(u32 *)&ipa_reg_save.ipa.pipes[50].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 51), \
		(u32 *)&ipa_reg_save.ipa.pipes[51].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 52), \
		(u32 *)&ipa_reg_save.ipa.pipes[52].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 53), \
		(u32 *)&ipa_reg_save.ipa.pipes[53].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 54), \
		(u32 *)&ipa_reg_save.ipa.pipes[54].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 55), \
		(u32 *)&ipa_reg_save.ipa.pipes[55].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 56), \
		(u32 *)&ipa_reg_save.ipa.pipes[56].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 57), \
		(u32 *)&ipa_reg_save.ipa.pipes[57].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 58), \
		(u32 *)&ipa_reg_save.ipa.pipes[58].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 59), \
		(u32 *)&ipa_reg_save.ipa.pipes[59].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 60), \
		(u32 *)&ipa_reg_save.ipa.pipes[60].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 61), \
		(u32 *)&ipa_reg_save.ipa.pipes[61].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 62), \
		(u32 *)&ipa_reg_save.ipa.pipes[62].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 63), \
		(u32 *)&ipa_reg_save.ipa.pipes[63].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 64), \
		(u32 *)&ipa_reg_save.ipa.pipes[64].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 65), \
		(u32 *)&ipa_reg_save.ipa.pipes[65].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 66), \
		(u32 *)&ipa_reg_save.ipa.pipes[66].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 67), \
		(u32 *)&ipa_reg_save.ipa.pipes[67].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 68), \
		(u32 *)&ipa_reg_save.ipa.pipes[68].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 69), \
		(u32 *)&ipa_reg_save.ipa.pipes[69].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 70), \
		(u32 *)&ipa_reg_save.ipa.pipes[70].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 71), \
		(u32 *)&ipa_reg_save.ipa.pipes[71].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 72), \
		(u32 *)&ipa_reg_save.ipa.pipes[72].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 73), \
		(u32 *)&ipa_reg_save.ipa.pipes[73].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 74), \
		(u32 *)&ipa_reg_save.ipa.pipes[74].endp.var_name, \
		GEN_REG_ATTR(reg_name) }

/*
 * Macro to define a particular register cfg entry for all pipe
 * indexed register
 */
#define IPA_REG_SAVE_CFG_ENTRY_PIPE_ENDP(reg_name, var_name)	\
	{ GEN_1xVECTOR_REG_OFST(reg_name, 0), \
		(u32 *)&ipa_reg_save.ipa.pipes[0].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 1), \
		(u32 *)&ipa_reg_save.ipa.pipes[1].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 2), \
		(u32 *)&ipa_reg_save.ipa.pipes[2].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 3), \
		(u32 *)&ipa_reg_save.ipa.pipes[3].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 4), \
		(u32 *)&ipa_reg_save.ipa.pipes[4].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 5), \
		(u32 *)&ipa_reg_save.ipa.pipes[5].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 6), \
		(u32 *)&ipa_reg_save.ipa.pipes[6].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 7), \
		(u32 *)&ipa_reg_save.ipa.pipes[7].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 8), \
		(u32 *)&ipa_reg_save.ipa.pipes[8].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 9), \
		(u32 *)&ipa_reg_save.ipa.pipes[9].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 10), \
		(u32 *)&ipa_reg_save.ipa.pipes[10].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 11), \
		(u32 *)&ipa_reg_save.ipa.pipes[11].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 12), \
		(u32 *)&ipa_reg_save.ipa.pipes[12].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 13), \
		(u32 *)&ipa_reg_save.ipa.pipes[13].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 14), \
		(u32 *)&ipa_reg_save.ipa.pipes[14].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 15), \
		(u32 *)&ipa_reg_save.ipa.pipes[15].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 16), \
		(u32 *)&ipa_reg_save.ipa.pipes[16].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 17), \
		(u32 *)&ipa_reg_save.ipa.pipes[17].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 18), \
		(u32 *)&ipa_reg_save.ipa.pipes[18].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 19), \
		(u32 *)&ipa_reg_save.ipa.pipes[19].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 20), \
		(u32 *)&ipa_reg_save.ipa.pipes[20].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 21), \
		(u32 *)&ipa_reg_save.ipa.pipes[21].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 22), \
		(u32 *)&ipa_reg_save.ipa.pipes[22].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 23), \
		(u32 *)&ipa_reg_save.ipa.pipes[23].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 24), \
		(u32 *)&ipa_reg_save.ipa.pipes[24].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 25), \
		(u32 *)&ipa_reg_save.ipa.pipes[25].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 26), \
		(u32 *)&ipa_reg_save.ipa.pipes[26].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 27), \
		(u32 *)&ipa_reg_save.ipa.pipes[27].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 28), \
		(u32 *)&ipa_reg_save.ipa.pipes[28].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 29), \
		(u32 *)&ipa_reg_save.ipa.pipes[29].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 30), \
		(u32 *)&ipa_reg_save.ipa.pipes[30].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 31), \
		(u32 *)&ipa_reg_save.ipa.pipes[31].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 32), \
		(u32 *)&ipa_reg_save.ipa.pipes[32].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 33), \
		(u32 *)&ipa_reg_save.ipa.pipes[33].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 34), \
		(u32 *)&ipa_reg_save.ipa.pipes[34].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 35), \
		(u32 *)&ipa_reg_save.ipa.pipes[35].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 36), \
		(u32 *)&ipa_reg_save.ipa.pipes[36].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 37), \
		(u32 *)&ipa_reg_save.ipa.pipes[37].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 38), \
		(u32 *)&ipa_reg_save.ipa.pipes[38].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 39), \
		(u32 *)&ipa_reg_save.ipa.pipes[39].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 40), \
		(u32 *)&ipa_reg_save.ipa.pipes[40].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 41), \
		(u32 *)&ipa_reg_save.ipa.pipes[41].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 42), \
		(u32 *)&ipa_reg_save.ipa.pipes[42].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 43), \
		(u32 *)&ipa_reg_save.ipa.pipes[43].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 44), \
		(u32 *)&ipa_reg_save.ipa.pipes[44].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 45), \
		(u32 *)&ipa_reg_save.ipa.pipes[45].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 46), \
		(u32 *)&ipa_reg_save.ipa.pipes[46].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 47), \
		(u32 *)&ipa_reg_save.ipa.pipes[47].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 48), \
		(u32 *)&ipa_reg_save.ipa.pipes[48].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 49), \
		(u32 *)&ipa_reg_save.ipa.pipes[49].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 50), \
	(u32 *)&ipa_reg_save.ipa.pipes[50].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 51), \
		(u32 *)&ipa_reg_save.ipa.pipes[51].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 52), \
		(u32 *)&ipa_reg_save.ipa.pipes[52].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 53), \
		(u32 *)&ipa_reg_save.ipa.pipes[53].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 54), \
		(u32 *)&ipa_reg_save.ipa.pipes[54].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 55), \
		(u32 *)&ipa_reg_save.ipa.pipes[55].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 56), \
		(u32 *)&ipa_reg_save.ipa.pipes[56].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 57), \
		(u32 *)&ipa_reg_save.ipa.pipes[57].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 58), \
		(u32 *)&ipa_reg_save.ipa.pipes[58].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 59), \
		(u32 *)&ipa_reg_save.ipa.pipes[59].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 60), \
		(u32 *)&ipa_reg_save.ipa.pipes[60].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 61), \
		(u32 *)&ipa_reg_save.ipa.pipes[61].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 62), \
		(u32 *)&ipa_reg_save.ipa.pipes[62].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 63), \
		(u32 *)&ipa_reg_save.ipa.pipes[63].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 64), \
		(u32 *)&ipa_reg_save.ipa.pipes[64].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 65), \
		(u32 *)&ipa_reg_save.ipa.pipes[65].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 66), \
		(u32 *)&ipa_reg_save.ipa.pipes[66].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 67), \
		(u32 *)&ipa_reg_save.ipa.pipes[67].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 68), \
		(u32 *)&ipa_reg_save.ipa.pipes[68].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 69), \
		(u32 *)&ipa_reg_save.ipa.pipes[69].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 70), \
		(u32 *)&ipa_reg_save.ipa.pipes[70].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 71), \
		(u32 *)&ipa_reg_save.ipa.pipes[71].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 72), \
		(u32 *)&ipa_reg_save.ipa.pipes[72].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 73), \
		(u32 *)&ipa_reg_save.ipa.pipes[73].endp.var_name, \
		GEN_REG_ATTR(reg_name) }, \
	{ GEN_1xVECTOR_REG_OFST(reg_name, 74), \
		(u32 *)&ipa_reg_save.ipa.pipes[74].endp.var_name, \
		GEN_REG_ATTR(reg_name) }

/*
 * Macro to define a particular register cfg entry for the remaining
 * pipe indexed register.  In Stingray case we don't have extra
 * endpoints so it is intentially empty
 */
#define IPA_HW_REG_SAVE_CFG_ENTRY_PIPE_ENDP_EXTRA(REG_NAME, VAR_NAME)  \
	{ 0, 0 }

/*
 * Macro to set the active flag for all active pipe indexed register
 * In Stingray case we don't have extra endpoints so it is intentially
 * empty
 */
#define IPA_HW_REG_SAVE_CFG_ENTRY_PIPE_ENDP_EXTRA_ACTIVE()  \
	do { \
	} while (0)

#endif /* #if !defined(_IPA_HW_COMMON_EX_H_) */
