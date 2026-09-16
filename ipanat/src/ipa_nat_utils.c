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
 */
#include "ipa_nat_utils.h"
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdarg.h>

#define IPA_MAX_MSG_LEN 4096

static char dbg_buff[IPA_MAX_MSG_LEN];
static ipa_nat_log_handler_t ipa_nat_log_handler;
#ifdef USE_CLOG
static clog_context_t ipa_nat_clog_ctx;
static pthread_once_t ipa_nat_clog_once = PTHREAD_ONCE_INIT;

/* Registered lazily: CLOG_REGISTER_LIBRARY() derives the App ID from the
 * hosting process, which has not registered it yet at library load time. */
static void ipa_nat_clog_init_once(void)
{
	(void)CLOG_REGISTER_LIBRARY();
	ipa_nat_clog_ctx = CLOG_REGISTER_CONTEXT(IPA_NAT_CLOG_CTX_ID, IPA_NAT_CLOG_CTX_NAME,
			"IPA NAT Library", IPA_NAT_CLOG_DIAG_SSID,
			IPA_NAT_CLOG_CTX_NAME, IPA_NAT_CLOG_CTX_NAME);
}

static void __attribute__((destructor)) ipa_nat_clog_deinit(void)
{
	if (ipa_nat_clog_ctx != NULL)
	{
		(void)CLOG_UNREGISTER_CONTEXT(ipa_nat_clog_ctx);
		ipa_nat_clog_ctx = NULL;
	}
	(void)CLOG_UNREGISTER_LIBRARY();
}

static clog_level_t ipa_nat_to_clog_level(ipa_nat_log_level_t level)
{
	switch (level)
	{
	case IPA_NAT_LOG_LVL_ERR:  return CLOG_LEVEL_ERROR;
	case IPA_NAT_LOG_LVL_WARN: return CLOG_LEVEL_WARN;
	case IPA_NAT_LOG_LVL_INFO: return CLOG_LEVEL_INFO;
	case IPA_NAT_LOG_LVL_DBG:  return CLOG_LEVEL_DEBUG;
	default:                   return CLOG_LEVEL_VERBOSE;
	}
}
#endif

void ipa_nat_set_log_handler(ipa_nat_log_handler_t handler)
{
	ipa_nat_log_handler = handler;
}

void ipa_nat_log_emit(
	ipa_nat_log_level_t level,
	const char* fmt,
	... )
{
	va_list ap;
	char msg[256]; //256 bytes are enough
	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);
	/* vsnprintf() always NUL-terminates msg, truncating on overflow rather
	 * than dropping it; emit whatever fits instead of discarding the log. */
#ifdef USE_CLOG
	(void)pthread_once(&ipa_nat_clog_once, ipa_nat_clog_init_once);
	if (ipa_nat_clog_ctx != NULL)
	{
		CLOG_LOG(ipa_nat_clog_ctx, ipa_nat_to_clog_level(level), "%s", msg);
		return;
	}
	/* Registration failed: use the relay/legacy path below. */
#endif
	if ( ipa_nat_log_handler )
	{
		ipa_nat_log_handler(level, msg);
	}
}

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

ipa_descriptor* ipa_descriptor_open(void)
{
	ipa_descriptor* desc_ptr;
	int res = 0;

	IPADBG("In\n");

	desc_ptr = calloc(1, sizeof(ipa_descriptor));

	if ( desc_ptr == NULL )
	{
		IPAERR("Unable to allocate ipa_descriptor\n");
		goto bail;
	}

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

	goto bail;

hndl_fail:
	free(desc_ptr);
	desc_ptr = NULL;

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
		if ( desc_ptr->fd >= 0)
		{
			close(desc_ptr->fd);
		}
		free(desc_ptr);
	}

	IPADBG("Out\n");
}

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

		dbg_buff[(result < IPA_MAX_MSG_LEN) ? result : (size_t)(IPA_MAX_MSG_LEN - 1)] = '\0';

		IPADBG("%s", dbg_buff);

		if (feof(debug_file))
			break;
	}
	fclose(debug_file);
}

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
