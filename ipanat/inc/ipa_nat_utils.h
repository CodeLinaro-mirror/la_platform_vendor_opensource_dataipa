/*
 * Copyright (c) 2013, 2018-2020 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Changes from Qualcomm Technologies, Inc. are provided under the following license:
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 *
 */
#ifndef IPA_NAT_UTILS_H
#define IPA_NAT_UTILS_H

#include <linux/msm_ipa.h>
#ifdef CONFIG_ECM_CONVERGENCE
#include "ipa_i.h"
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <stdbool.h>
#include <time.h>
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAX_DMA_ENTRIES_FOR_ADD 4
#define MAX_DMA_ENTRIES_FOR_DEL 3

#define IPA_DUMMY_PDN_PUB_IP 0xFFFFFFFF

#if !defined(MSM_IPA_TESTS) && !defined(FEATURE_IPA_ANDROID)
#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#else
size_t strlcpy(char* dst, const char* src, size_t size);
#endif
#endif

#ifndef CONFIG_ECM_CONVERGENCE
#define IPAERR(fmt, ...)  printf("ERR: %s:%d %s() " fmt, __FILE__,  __LINE__, __FUNCTION__, ##__VA_ARGS__);
#endif
#define IPAINFO(fmt, ...)  printf("INFO: %s:%d %s() " fmt, __FILE__,  __LINE__, __FUNCTION__, ##__VA_ARGS__);

#define IPAWARN(fmt, ...)  printf("WARN: %s:%d %s() " fmt, __FILE__,  __LINE__, __FUNCTION__, ##__VA_ARGS__);

#ifdef NAT_DEBUG
#define IPADBG(fmt, ...) printf("%s:%d %s() " fmt, __FILE__,  __LINE__, __FUNCTION__, ##__VA_ARGS__);
#else
#ifndef CONFIG_ECM_CONVERGENCE
#define IPADBG(fmt, ...)
#endif
#endif

typedef struct
{
	#ifndef CONFIG_ECM_CONVERGENCE
	int              fd;
	#endif
	enum ipa_hw_type ver;
} ipa_descriptor;

ipa_descriptor* ipa_descriptor_open(void);

void ipa_descriptor_close(
	ipa_descriptor*);

#ifndef CONFIG_ECM_CONVERGENCE
void ipa_read_debug_info(
	const char* debug_file_path);
#endif

static inline char* prep_ioc_table_write_cmd_4print(
	struct ipa_ioc_table_write_cmd* cmd_ptr,
	char*                       buf_ptr,
	uint32_t                    buf_sz )
{
	uint32_t i, len, buf_left;

	if ( cmd_ptr && buf_ptr && buf_sz )
	{
		snprintf(
			buf_ptr,
			buf_sz,
			"TABLE_WRITE_CMD: mem_type(%u) entries(%u) ",
			cmd_ptr->mem_type,
			cmd_ptr->entries);

		for ( i = 0; i < cmd_ptr->entries; i++ )
		{
			len = strlen(buf_ptr);

			buf_left = buf_sz - len;

			if ( buf_left > 0 && buf_left < buf_sz )
			{
				snprintf(
					buf_ptr + len,
					buf_left,
					"[%u](table_index(0x%02X) table_Select(0x%02X) offset_within_entry(0x%02X) entry_index(0x%04X) data(0x%04X) cache_entry_evict(0x%01X) no_write(0x%01X) cache_entry_hash_value(0x%04X) write_bitmask(0x%04X)) ",
					i,
					cmd_ptr->dma[i].table_index,
					cmd_ptr->dma[i].table_Select,
					cmd_ptr->dma[i].offset_within_entry,
					cmd_ptr->dma[i].entry_index,
					cmd_ptr->dma[i].data,
					cmd_ptr->dma[i].cache_entry_evict,
					cmd_ptr->dma[i].no_write,
					cmd_ptr->dma[i].cache_entry_hash_value,
					cmd_ptr->dma[i].write_bitmask);
			}
		}
	}

	return buf_ptr;
}

static inline char* prep_ioc_nat_dma_cmd_4print(
	struct ipa_ioc_nat_dma_cmd* cmd_ptr,
	char*                       buf_ptr,
	uint32_t                    buf_sz )
{
	uint32_t i, len, buf_left;

	if ( cmd_ptr && buf_ptr && buf_sz )
	{
		snprintf(
			buf_ptr,
			buf_sz,
			"NAT_DMA_CMD: mem_type(%u) entries(%u) ",
			cmd_ptr->mem_type,
			cmd_ptr->entries);

		for ( i = 0; i < cmd_ptr->entries; i++ )
		{
			len = strlen(buf_ptr);

			buf_left = buf_sz - len;

			if ( buf_left > 0 && buf_left < buf_sz )
			{
				snprintf(
					buf_ptr + len,
					buf_left,
					"[%u](table_index(0x%02X) base_addr(0x%02X) offset(0x%08X) data(0x%04X)) ",
					i,
					cmd_ptr->dma[i].table_index,
					cmd_ptr->dma[i].base_addr,
					cmd_ptr->dma[i].offset,
					(uint32_t) cmd_ptr->dma[i].data);
			}
		}
	}

	return buf_ptr;
}

#undef NANOS_PER_SEC
#undef MICROS_PER_SEC
#undef MILLIS_PER_SEC

#define NANOS_PER_SEC  1000000000
#define MICROS_PER_SEC    1000000
#define MILLIS_PER_SEC       1000

/**
 * A macro for converting seconds to nanoseconds...
 */
#define SECS2NanSECS(x) ((x) * NANOS_PER_SEC)

/**
 * A macro for converting seconds to microseconds...
 */
#define SECS2MicSECS(x) ((x) * MICROS_PER_SEC)

/**
 * A macro for converting seconds to milliseconds...
 */
#define SECS2MilSECS(x) ((x) * MILLIS_PER_SEC)

/******************************************************************************/

typedef enum
{
	TimeAsNanSecs = 0,
	TimeAsMicSecs = 1,
	TimeAsMilSecs = 2
} TimeAs_t;

#undef VALID_TIMEAS
#define VALID_TIMEAS(ta)   \
	( (ta) == TimeAsNanSecs ||					\
	  (ta) == TimeAsMicSecs ||					\
	  (ta) == TimeAsMilSecs )

int currTimeAs(
	TimeAs_t  timeAs,
	uint64_t* valPtr );

#endif /* IPA_NAT_UTILS_H */
