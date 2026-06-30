// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 */

#ifndef _IPA3_DIAG_LOG_H_
#define _IPA3_DIAG_LOG_H_

#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/cdev.h>

/*
 * ipa_diag_log - in-kernel ring that taps the control-path IPA log macros
 * (IPADBG / IPAERR / IPAERR_RL and subsystem equivalents) and exposes the
 * captured text lines to user space through the /dev/diag_ipa character
 * device. A user-space daemon (ipacm-diag) drains the device and forwards
 * each line to DIAG/QXDM.
 *
 * Design goals:
 *   - Zero per-message heap allocation (fixed pre-allocated slot array).
 *   - Safe to call from any context, including atomic / IRQ, hence a
 *     spinlock taken with irqsave rather than the ODL-style mutex+kmalloc.
 *   - Bounded memory; drop-oldest on overflow so the freshest logs survive.
 *   - Near-zero overhead when disabled (single branch on ->enabled).
 */

/* Each slot holds one formatted log line including the trailing '\n'. */
#define IPA_DIAG_LOG_LINE_SZ		256

/* Max length of the daemon-owned sink config string in the debugfs knob.
 * Sized to hold "file:" + a realistic absolute path without truncation.
 */
#define IPA_DIAG_LOG_SINK_CFG_SZ	128

/*
 * Number of slots in the ring. Power of two so the modulo reduces to a mask.
 * 2048 * 256B = 512KB, matched to the bursty control-path log rate.
 */
#define IPA_DIAG_LOG_NUM_SLOTS		2048

/*
 * Wake the blocked reader once at least this many unread lines have
 * accumulated (or when no reader has drained for a while - see the writer).
 * Batching wakeups instead of waking on every line drastically cuts
 * context-switch overhead under a heavy log rate, which lets the reader spend
 * its CPU draining rather than thrashing. A reader that is already running
 * keeps draining regardless; this only governs when a *sleeping* reader is
 * woken.
 */
#define IPA_DIAG_LOG_WAKE_BATCH		32

/**
 * struct ipa_diag_log_slot - one ring entry
 * @len:  number of valid bytes in @data (0 .. IPA_DIAG_LOG_LINE_SZ)
 * @data: one formatted, newline-terminated record. The record is framed as
 *            "<L>[ssssss.uuuuuu] <message>\n"
 *        where <L> is a single ASCII level digit ('0'+enum ipa_diag_log_level)
 *        and the bracketed field is a local_clock() timestamp. User space
 *        strips the leading level digit to choose the QXDM level and forwards
 *        the remainder (timestamp + message) verbatim.
 */
struct ipa_diag_log_slot {
	u16 len;
	char data[IPA_DIAG_LOG_LINE_SZ];
};

/**
 * struct ipa_diag_log_stats - counters (monotonic, for debugfs/diagnostics)
 * @enqueued: lines successfully written into the ring
 * @dropped:  lines discarded (overflow) because the ring was full
 * @read:     lines handed to user space via read()
 * @markers:  synthetic "N messages dropped" marker lines emitted to the
 *            reader (so a debugger sees gaps explicitly instead of silently)
 */
struct ipa_diag_log_stats {
	u64 enqueued;
	u64 dropped;
	u64 read;
	u64 markers;
};

/**
 * struct ipa_diag_log_cdev - character-device bookkeeping
 */
struct ipa_diag_log_cdev {
	struct class *class;
	dev_t dev_num;
	struct device *dev;
	struct cdev cdev;
};

/**
 * struct ipa_diag_log_context - global state for the diag log ring
 * @cdev:     /dev/diag_ipa character device
 * @ring:     pre-allocated slot array (IPA_DIAG_LOG_NUM_SLOTS entries)
 * @head:     producer index (next slot to write); wraps via mask
 * @tail:     consumer index (next slot to read); wraps via mask
 * @lock:     protects head/tail/ring/stats; taken with irqsave
 * @waitq:    readers block here until data is available
 * @stats:    diagnostic counters
 * @enabled:  runtime knob; when false the tap is a no-op (default true)
 * @min_level: kernel-side severity filter; a line is captured only when its
 *             level <= min_level (default IPA_DIAG_LVL_DBG=2, i.e. ERR/INFO/DBG
 *             captured, LOW dropped). Runtime-tunable via debugfs.
 * @drop_newest: overflow policy knob. false (default) = drop-oldest (keep the
 *               freshest lines); true = drop-newest (keep the earliest lines,
 *               useful when a root cause started early). Runtime-tunable.
 * @dropped_reported: value of stats.dropped already accounted for by an emitted
 *               drop marker; the gap (dropped - dropped_reported) is what the
 *               next marker reports. Reader-side, under lock.
 * @sink_cfg: opaque, daemon-owned sink configuration string (e.g. "diag" or
 *            "file:/data/diag_ipa.log"). The kernel never interprets it; the
 *            userspace daemon reads it from debugfs at startup to choose its
 *            sink. Guarded by @lock for read/modify.
 * @initialized: ring + cdev are ready
 * @cdev_added: the embedded char device has been cdev_add()ed and must be
 *              cdev_del()ed exactly once; gates char-device teardown so
 *              cleanup() is idempotent and add/del stay balanced
 *
 * The ring is empty when head == tail and full when
 * ((head + 1) & mask) == tail, i.e. one slot is always kept free to
 * disambiguate full from empty without a separate count.
 */
struct ipa_diag_log_context {
	struct ipa_diag_log_cdev cdev;
	struct ipa_diag_log_slot *ring;
	u32 head;
	u32 tail;
	spinlock_t lock;
	wait_queue_head_t waitq;
	struct ipa_diag_log_stats stats;
	bool enabled;
	u8 min_level;
	bool drop_newest;
	u64 dropped_reported;
	char sink_cfg[IPA_DIAG_LOG_SINK_CFG_SZ];
	bool initialized;
	bool cdev_added;
};

extern struct ipa_diag_log_context *ipa3_diag_log_ctx;

int ipa3_diag_log_init(void);
void ipa3_diag_log_cleanup(void);

/*
 * ipa3_diag_log_write() - tap entry point invoked by the log macros.
 * Declared (also) in ipa_common_i.h so every macro call site sees it
 * without pulling in this header. @level is enum ipa_diag_log_level.
 */
void ipa3_diag_log_write(u8 level, const char *fmt, ...) __printf(2, 3);

#endif /* _IPA3_DIAG_LOG_H_ */
