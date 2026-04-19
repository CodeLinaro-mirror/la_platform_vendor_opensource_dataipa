/*
 * Copyright (c) 2013, 2018-2019 The Linux Foundation. All rights reserved.
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
#include "ipa_nat_utils.h"

#ifdef CONFIG_ECM_CONVERGENCE
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/idr.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#else
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#endif

#define IPA_MAX_MSG_LEN 4096

#ifndef CONFIG_ECM_CONVERGENCE
static char dbg_buff[IPA_MAX_MSG_LEN];
#endif

#if !defined(MSM_IPA_TESTS) && !defined(USE_GLIB) && !defined(FEATURE_IPA_ANDROID)
size_t strlcpy(char* dst, const char* src, size_t size)
{
	size_t i;

	if (size == 0)
		return strlen(src);

	for (i = 0; i < (size - 1) && src[i] != '\0'; ++i)
		dst[i] = src[i];

	dst[i] = '\0';

	return i + strlen(src + i);
}
#endif

#ifdef CONFIG_ECM_CONVERGENCE
static struct {
	struct idr idr;
	spinlock_t lock;
	int ref_cnt;
	uint16_t max_counters;

	/* INI-based per-family caps (unified pool remains shared). 0 = disabled. */
	uint16_t ini_max_v4;
	uint16_t ini_max_v6;
	atomic_t allocated_v4;
	atomic_t allocated_v6;
} nat_stats;

static DEFINE_MUTEX(nat_stats_init_lock);
#endif

ipa_descriptor* ipa_descriptor_open(void)
{
	ipa_descriptor* desc_ptr;
#ifndef CONFIG_ECM_CONVERGENCE
		int res = 0;
#endif

	IPADBG("In\n");
#ifdef CONFIG_ECM_CONVERGENCE
	desc_ptr = kzalloc(sizeof(ipa_descriptor), GFP_KERNEL);
#else
	desc_ptr = calloc(1, sizeof(ipa_descriptor));
#endif

	if ( desc_ptr == NULL )
	{
		IPAERR("Unable to allocate ipa_descriptor\n");
		goto bail;
	}

#ifdef CONFIG_ECM_CONVERGENCE
	desc_ptr->ver = ipa3_ctx->ipa_hw_type;
	if (desc_ptr->ver >= IPA_HW_v7_0) {
		desc_ptr->max_stats_counters = ipa3_ctx->hw_stats->nat_ct.init.max_counters;
		desc_ptr->nat_stats_mode = ipa3_ctx->nat_stats_mode;

		/* 0 means disabled (no INI limitation). */
		nat_stats.ini_max_v4 = ipa3_ctx->nat_stats_max_counters_v4;
		nat_stats.ini_max_v6 = ipa3_ctx->nat_stats_max_counters_v6;
		IPADBG("Set NAT/CT stats INI limits: v4=%u v6=%u\n",
			nat_stats.ini_max_v4, nat_stats.ini_max_v6);

		IPADBG("Initialized counter limits: max_counters=%u, mode=%d\n",
			desc_ptr->max_stats_counters,
			desc_ptr->nat_stats_mode);
	}
#else
	desc_ptr->fd = open(IPA_DEV_NAME, O_RDONLY);

	if (desc_ptr->fd < 0)
	{
		IPAERR("Unable to open ipa device\n");
		goto hndl_fail;
	}

	res = ioctl(desc_ptr->fd, IPA_IOC_GET_HW_VERSION, &desc_ptr->ver);

	if (res == 0)
	{
		IPADBG("IPA version is %d\n", desc_ptr->ver);
	}
	else
	{
		IPAERR("Unable to get IPA version. Error %d\n", res);
		desc_ptr->ver = IPA_HW_None;
	}
#endif

	goto bail;


#ifdef CONFIG_ECM_CONVERGENCE
#else
hndl_fail:
	free(desc_ptr);
	desc_ptr = NULL;
#endif

bail:
	IPADBG("Out\n");

	return desc_ptr;
}

void ipa_descriptor_close(
	ipa_descriptor* desc_ptr)
{
	IPADBG("In\n");

	if ( desc_ptr )
	{
#ifdef CONFIG_ECM_CONVERGENCE
		kfree(desc_ptr);
#else
		if ( desc_ptr->fd >= 0)
		{
			close(desc_ptr->fd);
		}
		free(desc_ptr);
#endif
	}

	IPADBG("Out\n");
}

#ifndef CONFIG_ECM_CONVERGENCE
void ipa_read_debug_info(
	const char* debug_file_path)
{
	size_t result;
	FILE* debug_file;

	debug_file = fopen(debug_file_path, "r");
	if (debug_file == NULL)
	{
		printf("Failed to open %s\n", debug_file_path);
		return;
	}

	for (;;)
	{
		result = fread(dbg_buff, sizeof(char), IPA_MAX_MSG_LEN, debug_file);
		if (!result)
			break;

		if (ferror(debug_file))
		{
			printf("Failed to read from %s\n", debug_file_path);
			break;
		}

		dbg_buff[(result < IPA_MAX_MSG_LEN) ? result : (IPA_MAX_MSG_LEN - 1)] = '\0';

		printf("%s", dbg_buff);

		if (feof(debug_file))
			break;
	}
	fclose(debug_file);
}
#endif


#ifndef CONFIG_ECM_CONVERGENCE
int currTimeAs(
	TimeAs_t  timeAs,
	uint64_t* valPtr )
{
	struct timespec timeSpec;

	int ret = 0;

	if ( ! VALID_TIMEAS(timeAs) || ! valPtr )
	{
		IPAERR("Bad arg: timeAs (%u) and/or valPtr (%p)\n",
			   timeAs, valPtr );
		ret = -1;
		goto bail;
	}

	memset(&timeSpec, 0, sizeof(timeSpec));

	if ( clock_gettime(CLOCK_MONOTONIC, &timeSpec) != 0 )
	{
		IPAERR("Can't get system clock time\n" );
		ret = -1;
		goto bail;
	}

	switch( timeAs )
	{
	case TimeAsNanSecs:
		*valPtr =
			(uint64_t) (SECS2NanSECS((uint64_t) timeSpec.tv_sec) +
						((uint64_t) timeSpec.tv_nsec));
		break;
	case TimeAsMicSecs:
		*valPtr =
			(uint64_t) (SECS2MicSECS((uint64_t) timeSpec.tv_sec) +
						((uint64_t) timeSpec.tv_nsec / 1000));
		break;
	case TimeAsMilSecs:
		*valPtr =
			(uint64_t) (SECS2MilSECS((uint64_t) timeSpec.tv_sec) +
						((uint64_t) timeSpec.tv_nsec / 1000000));
		break;
	default:
		IPAERR("Invalid timeAs\n" );
		break;
	}

bail:
	return ret;
}
#endif

#ifdef CONFIG_ECM_CONVERGENCE
void ipa_nat_stats_init(uint16_t max_counters)
{
	mutex_lock(&nat_stats_init_lock);
	if (nat_stats.ref_cnt == 0) {
		IPADBG("Initializing NAT stats IDR (max=%u), ini_max_v4=%u ini_max_v6=%u\n",
		       max_counters, nat_stats.ini_max_v4, nat_stats.ini_max_v6);
		idr_init(&nat_stats.idr);
		spin_lock_init(&nat_stats.lock);
		nat_stats.max_counters = max_counters;
		atomic_set(&nat_stats.allocated_v4, 0);
		atomic_set(&nat_stats.allocated_v6, 0);
	}
	nat_stats.ref_cnt++;
	mutex_unlock(&nat_stats_init_lock);
}

void ipa_nat_stats_destroy(void)
{
	mutex_lock(&nat_stats_init_lock);
	if (nat_stats.ref_cnt > 0) {
		nat_stats.ref_cnt--;
		if (nat_stats.ref_cnt == 0) {
			IPADBG("Destroying NAT stats IDR (allocated_v4=%d allocated_v6=%d)\n",
			       atomic_read(&nat_stats.allocated_v4),
			       atomic_read(&nat_stats.allocated_v6));
			idr_destroy(&nat_stats.idr);
			nat_stats.max_counters = 0;
			atomic_set(&nat_stats.allocated_v4, 0);
			atomic_set(&nat_stats.allocated_v6, 0);
		}
	}
	mutex_unlock(&nat_stats_init_lock);
}

int ipa_nat_stats_alloc_id(uint16_t *counter_id)
{
	int id;
	void *ptr = (void *)1; /* Non-NULL pointer for IDR */

	if (nat_stats.max_counters == 0) {
		IPAERR("NAT stats not initialized\n");
		return -EPERM;
	}

	spin_lock(&nat_stats.lock);
	/* Allocate ID from 1 to max_counters (0 means no counter) */
	id = idr_alloc(&nat_stats.idr, ptr, 1, nat_stats.max_counters + 1, GFP_ATOMIC);
	spin_unlock(&nat_stats.lock);

	if (id < 0) {
		if (id == -ENOSPC)
			IPAERR("No more counters available (max=%u)\n", nat_stats.max_counters);
		else
			IPAERR("IDR allocation failed: %d\n", id);
		return id;
	}

	*counter_id = (uint16_t)id;
	IPADBG("Allocated shared counter ID: %u\n", *counter_id);
	return 0;
}

static int __ipa_nat_stats_pre_alloc(atomic_t *allocated, uint16_t cap)
{
	if (cap == 0)
		return 0;

	if (atomic_inc_return(allocated) > cap) {
		atomic_dec(allocated);
		return -ENOSPC;
	}

	return 0;
}

int ipa_nat_stats_pre_alloc_v4(void)
{
	return __ipa_nat_stats_pre_alloc(&nat_stats.allocated_v4, nat_stats.ini_max_v4);
}

int ipa_nat_stats_pre_alloc_v6(void)
{
	return __ipa_nat_stats_pre_alloc(&nat_stats.allocated_v6, nat_stats.ini_max_v6);
}

static void __ipa_nat_stats_post_free(atomic_t *allocated, uint16_t cap)
{
	int v;

	if (cap == 0)
		return;

	/*
	 * Defensive against double-free / wrong-type free. Do not underflow.
	 * atomic_dec_if_positive returns new value, or -1 if already 0.
	 */
	v = atomic_dec_if_positive(allocated);
	if (v < 0)
		IPAERR("NAT/CT stats quota underflow (cap=%u)\n", cap);
}

void ipa_nat_stats_post_free_v4(void)
{
	__ipa_nat_stats_post_free(&nat_stats.allocated_v4, nat_stats.ini_max_v4);
}

void ipa_nat_stats_post_free_v6(void)
{
	__ipa_nat_stats_post_free(&nat_stats.allocated_v6, nat_stats.ini_max_v6);
}

void ipa_nat_stats_free_id(uint16_t counter_id)
{
	if (counter_id == 0)
		return;

	spin_lock(&nat_stats.lock);
	idr_remove(&nat_stats.idr, counter_id);
	spin_unlock(&nat_stats.lock);

	IPADBG("Freed shared counter ID: %u\n", counter_id);
}
#endif
