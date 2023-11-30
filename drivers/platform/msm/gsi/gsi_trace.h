/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#undef TRACE_SYSTEM
#define TRACE_SYSTEM gsi
#define TRACE_INCLUDE_FILE gsi_trace
#include <linux/timekeeping.h>


#if !defined(_GSI_TRACE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _GSI_TRACE_H

#include <linux/tracepoint.h>


TRACE_EVENT(
	gsi_qtimer,

	TP_PROTO(u64 qtimer, bool is_ll, uint8_t evt, uint32_t ch, uint32_t msk),

	TP_ARGS(qtimer, is_ll, evt, ch, msk),

	TP_STRUCT__entry(
		__field(u64,		qtimer)
		__field(bool,		is_ll)
		__field(uint8_t,	evt)
		__field(uint32_t,	ch)
		__field(uint32_t,	msk)
		__field(u64,	qtime)
	),

	TP_fast_assign(
		__entry->qtimer = qtimer;
		__entry->is_ll = is_ll;
		__entry->evt = evt;
		__entry->ch = ch;
		__entry->msk = msk;
		__entry->qtime = ktime_get_real();
	),

	TP_printk("qtimer=%llu is_ll=%s, evt=%u, ch=0x%x, msk=0x%x UTC time %lld",
		__entry->qtimer,
		__entry->is_ll ? "true" : "false",
		__entry->evt,
		__entry->ch,
		__entry->msk,
		__entry->qtime)
);

#endif /* _GSI_TRACE_H */

/* This part must be outside protection */
#undef TRACE_INCLUDE_PATH
#ifdef CONFIG_IPA_VENDOR_DLKM
#define TRACE_INCLUDE_PATH ../../../../vendor/qcom/opensource/dataipa/drivers/platform/msm/gsi
#else
#define TRACE_INCLUDE_PATH ../gsi
#endif
#include <trace/define_trace.h>
