// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */
#if !defined(_IPA_ACCESS_CONTROL_H_)
#define _IPA_ACCESS_CONTROL_H_

#include "ipa_reg_dump.h"

/*
 * AA_COMBO - actual read, actual write
 * AN_COMBO - actual read, no-op write
 * NA_COMBO - no-op read, actual write
 * NN_COMBO - no-op read, no-op write
 */

/*
 * The following is target specific.
 */
static struct reg_mem_access_map_t mem_access_map[] = {
	/*------------------------------------------------------------*/
	/*      Range               Use when              Use when    */
	/*  Begin    End           SD_ENABLED           SD_DISABLED   */
	/*------------------------------------------------------------*/
	{ 0x04000, 0x04FFF, { &io_matrix[AN_COMBO], &io_matrix[AN_COMBO] } },
	{ 0xA8000, 0xB7FFF, { &io_matrix[AN_COMBO], &io_matrix[AN_COMBO] } },
	{ 0x05000, 0x0EFFF, { &io_matrix[AN_COMBO], &io_matrix[AN_COMBO] } },
	{ 0x0F000, 0x0FFFF, { &io_matrix[NN_COMBO], &io_matrix[NN_COMBO] } },
	{ 0x18000, 0x29FFF, { &io_matrix[AA_COMBO], &io_matrix[AA_COMBO] } },
	{ 0x2A000, 0x3BFFF, { &io_matrix[AN_COMBO], &io_matrix[AN_COMBO] } },
	{ 0x3C000, 0x4DFFF, { &io_matrix[AN_COMBO], &io_matrix[AN_COMBO] } },
	{ 0x4E000, 0x5FFFF, { &io_matrix[AA_COMBO], &io_matrix[AA_COMBO] } },
	{ 0x60000, 0x71FFF, { &io_matrix[AA_COMBO], &io_matrix[AA_COMBO] } },
	{ 0x10000, 0x10FFF, { &io_matrix[AA_COMBO], &io_matrix[AA_COMBO] } },
	{ 0x11000, 0x11FFF, { &io_matrix[NN_COMBO], &io_matrix[NN_COMBO] } },
	{ 0x12000, 0x12FFF, { &io_matrix[NN_COMBO], &io_matrix[NN_COMBO] } },
	{ 0x13000, 0x13FFF, { &io_matrix[AA_COMBO], &io_matrix[AA_COMBO] } },
	{ 0x14000, 0x14FFF, { &io_matrix[AA_COMBO], &io_matrix[AA_COMBO] } },
	{ 0x12C000, 0x12CFFF, { &io_matrix[AA_COMBO], &io_matrix[AA_COMBO] } },
	{ 0x12D000, 0x12DFFF, { &io_matrix[NN_COMBO], &io_matrix[NN_COMBO] } },
	{ 0x12E000, 0x12EFFF, { &io_matrix[NN_COMBO], &io_matrix[NN_COMBO] } },
	{ 0x12F000, 0x12FFFF, { &io_matrix[AA_COMBO], &io_matrix[AA_COMBO] } },
	{ 0x130000, 0x130FFF, { &io_matrix[AA_COMBO], &io_matrix[AA_COMBO] } },
	{ 0x114000, 0x11BFFF, { &io_matrix[AA_COMBO], &io_matrix[AA_COMBO] } },
	{ 0x127000, 0x12BFFF, { &io_matrix[AN_COMBO], &io_matrix[AN_COMBO] } },
	{ 0x140000, 0x15FFFF, { &io_matrix[AA_COMBO], &io_matrix[AA_COMBO] } },
	{ 0x160000, 0x18FFFF, { &io_matrix[NN_COMBO], &io_matrix[AN_COMBO] } },
	{ 0x190000, 0x190FFF, { &io_matrix[NN_COMBO], &io_matrix[NN_COMBO] } },
	{ 0x191000, 0x192FFF, { &io_matrix[AN_COMBO], &io_matrix[AN_COMBO] } },
	{ 0x1A0000, 0x1BFFFF, { &io_matrix[NN_COMBO], &io_matrix[AN_COMBO] } },
	{ 0x1C0000, 0x1C1FFF, { &io_matrix[NN_COMBO], &io_matrix[NN_COMBO] } },
	{ 0x1C2000, 0x1C3FFF, { &io_matrix[AA_COMBO], &io_matrix[AA_COMBO] } },
	{ 0x102000, 0x107FFF, { &io_matrix[NN_COMBO], &io_matrix[NN_COMBO] } },
	{ 0x193000, 0x195FFF, { &io_matrix[AA_COMBO], &io_matrix[AA_COMBO] } },
};

#endif /* #if !defined(_IPA_ACCESS_CONTROL_H_) */
