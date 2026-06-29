// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 */

/*
 * IPA diag log tap.
 *
 * Captures the control-path IPA log macros (IPADBG / IPAERR / IPAERR_RL and
 * the HAL / backend / ETH / WAN equivalents) into a bounded, pre-allocated
 * ring and exposes the text to user space via /dev/diag_ipa. A user-space
 * daemon forwards the lines to DIAG/QXDM.
 *
 * This intentionally does NOT use the ODL pattern (per-message kmalloc +
 * kmemdup + linked list + mutex): that sleeps and allocates, which is illegal
 * at the many IPA log sites that run in atomic / IRQ context, and far too
 * heavy for a ~thousands-of-sites log firehose. Instead we use a fixed slot
 * array guarded by a spinlock taken with irqsave, with drop-oldest overflow.
 */

#include "ipa_i.h"
#include "ipa_diag_log.h"
#include <linux/poll.h>
#include <linux/sched/signal.h>
#include <linux/sched/clock.h>	/* local_clock() */
#include <linux/uaccess.h>
#include <linux/vmalloc.h>

#define IPA_DIAG_LOG_DEV_NAME "diag_ipa"
#define IPA_DIAG_LOG_MASK (IPA_DIAG_LOG_NUM_SLOTS - 1)

/*
 * Upper bound on how long a blocking reader sleeps before re-checking the ring
 * on its own. Pairs with the batched wakeup in the writer: it bounds delivery
 * latency for a trailing sub-batch burst without requiring a per-line wakeup.
 */
#define IPA_DIAG_LOG_POLL_TIMEOUT msecs_to_jiffies(100)

/*
 * The context is statically allocated and never freed. This deliberately
 * eliminates any use-after-free of the context (its lock and wait queue) by a
 * reader still sleeping in ->read() when ipa3_diag_log_cleanup() runs (e.g. on
 * driver remove or probe-failure unwind). Only the ring payload is allocated
 * dynamically, and it is torn down under the lock (see cleanup) so a reader can
 * never dereference freed ring memory.
 */
static struct ipa_diag_log_context diag_log_ctx_storage;
struct ipa_diag_log_context *ipa3_diag_log_ctx;

/* ring is empty when head == tail */
static inline bool ipa_diag_log_empty(const struct ipa_diag_log_context *ctx)
{
	return ctx->head == ctx->tail;
}

/* ring is full when advancing head would collide with tail (one slot spare) */
static inline bool ipa_diag_log_full(const struct ipa_diag_log_context *ctx)
{
	return ((ctx->head + 1) & IPA_DIAG_LOG_MASK) == ctx->tail;
}

/* number of unread lines currently in the ring (caller holds the lock) */
static inline u32 ipa_diag_log_inflight(const struct ipa_diag_log_context *ctx)
{
	return (ctx->head - ctx->tail) & IPA_DIAG_LOG_MASK;
}

/**
 * ipa3_diag_log_write() - format a log line into the ring.
 *
 * @level: severity (enum ipa_diag_log_level); lines with level > ctx->min_level
 *         are filtered out cheaply before any work.
 *
 * Each record is framed as "<L>[ssssss.uuuuuu] <message>\n": a single ASCII
 * level digit, a local_clock() timestamp, then the caller's message. User
 * space strips the level digit to pick the QXDM level and forwards the rest.
 *
 * Safe from any context (including atomic / IRQ). On overflow the configured
 * policy decides which line to discard: drop-oldest (default, keep freshest)
 * advances tail; drop-newest (keep earliest) discards the incoming line.
 * A no-op (single branch) when disabled, not ready, or filtered by level.
 *
 * Wakeups are batched: a sleeping reader is woken only once at least
 * IPA_DIAG_LOG_WAKE_BATCH lines have accumulated (or the ring just filled),
 * which cuts context-switch overhead under a heavy log rate. An already-running
 * reader drains everything regardless, so batching never strands lines.
 */
void ipa3_diag_log_write(u8 level, const char *fmt, ...)
{
	struct ipa_diag_log_context *ctx;
	struct ipa_diag_log_slot *slot;
	unsigned long flags;
	va_list args;
	int len;
	int pfx;
	u64 ts;
	unsigned long rem_nsec;
	bool full;
	bool do_wake = false;

	/*
	 * Acquire-load the published pointer: pairs with smp_store_release()
	 * in init so that on weakly-ordered architectures (e.g. the arm64
	 * modem target) a non-NULL ctx guarantees ->ring / ->initialized are
	 * already visible. Fast no-op path when off / not ready / filtered.
	 */
	ctx = smp_load_acquire(&ipa3_diag_log_ctx);
	if (!ctx || !READ_ONCE(ctx->enabled))
		return;
	/* Cheap severity filter before taking the lock or formatting. */
	if (level > READ_ONCE(ctx->min_level))
		return;

	/* Timestamp outside the lock to keep the critical section short. */
	ts = local_clock();
	rem_nsec = do_div(ts, 1000000000);

	spin_lock_irqsave(&ctx->lock, flags);

	/* Ring may have been torn down under the lock by cleanup(). */
	if (!ctx->ring) {
		spin_unlock_irqrestore(&ctx->lock, flags);
		return;
	}

	full = ipa_diag_log_full(ctx);
	if (full && READ_ONCE(ctx->drop_newest)) {
		/*
		 * Drop-newest: keep the earliest lines. Discard the incoming
		 * line entirely (just count it) and leave the ring untouched.
		 */
		ctx->stats.dropped++;
		spin_unlock_irqrestore(&ctx->lock, flags);
		return;
	}
	if (full) {
		/* Drop-oldest: make room by discarding the tail entry. */
		ctx->tail = (ctx->tail + 1) & IPA_DIAG_LOG_MASK;
		ctx->stats.dropped++;
	}

	slot = &ctx->ring[ctx->head];

	/*
	 * Frame: level digit + timestamp prefix, then the message. Cap the
	 * level digit to a single char even if a bogus value is passed.
	 */
	pfx = scnprintf(slot->data, IPA_DIAG_LOG_LINE_SZ - 1, "%c[%6llu.%06lu] ",
			'0' + (level & 0x7), (unsigned long long)ts,
			rem_nsec / 1000);

	va_start(args, fmt);
	/*
	 * Reserve one byte for a guaranteed trailing newline so user space can
	 * split the stream into records unambiguously. vscnprintf caps at
	 * (size - 1) chars + NUL, so len <= IPA_DIAG_LOG_LINE_SZ - 2 here,
	 * leaving room to append '\n' below.
	 */
	len = pfx + vscnprintf(slot->data + pfx, IPA_DIAG_LOG_LINE_SZ - 1 - pfx,
			fmt, args);
	va_end(args);

	/* Normalize to exactly one trailing newline. */
	if (len == 0 || slot->data[len - 1] != '\n')
		slot->data[len++] = '\n';

	slot->len = (u16)len;
	ctx->head = (ctx->head + 1) & IPA_DIAG_LOG_MASK;
	ctx->stats.enqueued++;

	/*
	 * Batched wakeup: only wake a sleeping reader once enough lines have
	 * piled up, or when the ring is full (to bound worst-case latency and
	 * limit drops). wake_up_interruptible is cheap when no one is blocked
	 * and is a no-op for an already-running reader, so this is purely an
	 * optimization for the heavy-rate case.
	 */
	if (full || ipa_diag_log_inflight(ctx) >= IPA_DIAG_LOG_WAKE_BATCH)
		do_wake = true;

	spin_unlock_irqrestore(&ctx->lock, flags);

	/* wake_up_interruptible is safe in atomic context (its own irqsave). */
	if (do_wake)
		wake_up_interruptible(&ctx->waitq);
}
EXPORT_SYMBOL(ipa3_diag_log_write);

static int ipa_diag_log_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int ipa_diag_log_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/**
 * ipa_diag_log_read() - drain newline-terminated log lines to user space.
 *
 * Copies as many whole lines as fit in @count. Blocks until at least one line
 * is available unless O_NONBLOCK is set. Whole-record semantics: a line is
 * either fully copied or not started, never split across reads.
 */
static ssize_t ipa_diag_log_read(struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos)
{
	struct ipa_diag_log_context *ctx;
	struct ipa_diag_log_slot *slot;
	size_t copied = 0;
	unsigned long flags;
	ssize_t ret = 0;
	bool too_small = false;
	DEFINE_WAIT_FUNC(wait, woken_wake_function);

	ctx = smp_load_acquire(&ipa3_diag_log_ctx);
	if (!ctx)
		return -ENODEV;

	/*
	 * ctx is statically allocated and never freed, so &ctx->waitq and
	 * &ctx->lock stay valid even across cleanup(). Only ctx->ring can go
	 * away (freed under the lock by cleanup) — every access below is
	 * guarded by the lock and a ->ring NULL check, so a reader that was
	 * blocked when the module/driver tore down wakes, sees ring == NULL,
	 * and returns -ENODEV instead of touching freed memory.
	 */
	add_wait_queue(&ctx->waitq, &wait);
	while (1) {
		spin_lock_irqsave(&ctx->lock, flags);

		/* Liveness: ring torn down by cleanup() => stop. */
		if (!ctx->ring) {
			spin_unlock_irqrestore(&ctx->lock, flags);
			if (!copied)
				ret = -ENODEV;
			else
				ret = copied;
			break;
		}

		/*
		 * Drop marker: if lines were discarded since we last told user
		 * space, emit one synthetic record announcing the gap BEFORE
		 * the surviving lines, so a debugger sees an explicit
		 * "N messages dropped" notice instead of a silent hole. Built
		 * and copied like any other record (whole-record semantics).
		 */
		if (ctx->stats.dropped > ctx->dropped_reported) {
			char marker[96];
			u64 gap = ctx->stats.dropped - ctx->dropped_reported;
			u64 ts = local_clock();
			unsigned long rem_nsec = do_div(ts, 1000000000);
			int mlen;

			/*
			 * Frame the marker exactly like a normal record
			 * ("<L>[sec.usec] msg") so user space parses it
			 * uniformly. Use ERR so the drop notice is never
			 * filtered out downstream. local_clock()/do_div() are
			 * atomic-safe (this runs under ctx->lock).
			 */
			mlen = scnprintf(marker, sizeof(marker),
				"%c[%6llu.%06lu] *** diag_ipa: %llu messages dropped ***\n",
				'0' + IPA_DIAG_LVL_ERR, (unsigned long long)ts,
				rem_nsec / 1000, gap);

			if ((size_t)mlen + copied > count) {
				/* No room now; deliver what we have (or flag
				 * too-small if this is the very first record).
				 */
				if (copied == 0)
					too_small = true;
				spin_unlock_irqrestore(&ctx->lock, flags);
				goto after_drain;
			}

			/* Account the gap now, under the lock, so a concurrent
			 * producer's new drops are reported by a later marker.
			 */
			ctx->dropped_reported = ctx->stats.dropped;
			ctx->stats.markers++;
			spin_unlock_irqrestore(&ctx->lock, flags);

			if (copy_to_user(buf + copied, marker, mlen)) {
				if (!copied)
					ret = -EFAULT;
				else
					ret = copied;
				goto out;
			}
			copied += mlen;

			spin_lock_irqsave(&ctx->lock, flags);
			if (!ctx->ring) {
				spin_unlock_irqrestore(&ctx->lock, flags);
				ret = copied;
				goto out;
			}
		}

		while (!ipa_diag_log_empty(ctx)) {
			slot = &ctx->ring[ctx->tail];

			/* Stop if the next whole line won't fit in the
			 * remaining user buffer. Leave it for the next read.
			 * If nothing has been copied yet, the caller's buffer
			 * is too small for even one record: flag it so we
			 * return an error instead of blocking forever.
			 */
			if (slot->len + copied > count) {
				if (copied == 0)
					too_small = true;
				break;
			}

			/*
			 * Copy the record into a local AND advance tail /
			 * counter atomically under the lock. This is what
			 * makes the dequeue race-free against a concurrent
			 * drop-oldest producer (which also advances tail): the
			 * slot is consumed and tail moved in one critical
			 * section, so the producer can never overwrite the
			 * slot we are about to deliver, and tail is never
			 * advanced twice for the same record. The user copy is
			 * then done from the local with the lock dropped.
			 */
			{
				struct ipa_diag_log_slot local;

				local.len = slot->len;
				memcpy(local.data, slot->data, slot->len);
				ctx->tail = (ctx->tail + 1) & IPA_DIAG_LOG_MASK;
				ctx->stats.read++;
				spin_unlock_irqrestore(&ctx->lock, flags);

				if (copy_to_user(buf + copied, local.data,
							local.len)) {
					/*
					 * Bad user buffer. The record is
					 * already dequeued; return the bytes
					 * successfully delivered so far (or
					 * EFAULT if this was the first). Losing
					 * one line to a userspace fault is
					 * acceptable and does not break the
					 * in-kernel conservation invariant
					 * (it was counted as read).
					 */
					if (!copied)
						ret = -EFAULT;
					else
						ret = copied;
					goto out;
				}
				copied += local.len;
				spin_lock_irqsave(&ctx->lock, flags);

				/* Ring may be torn down during the copy. */
				if (!ctx->ring) {
					spin_unlock_irqrestore(&ctx->lock,
							flags);
					ret = copied;
					goto out;
				}
			}
		}

		spin_unlock_irqrestore(&ctx->lock, flags);

after_drain:
		if (copied) {
			ret = copied;
			break;
		}

		/* Caller's buffer can't hold even one record. */
		if (too_small) {
			ret = -EINVAL;
			break;
		}

		ret = -EAGAIN;
		if (filp->f_flags & O_NONBLOCK)
			break;

		ret = -EINTR;
		if (signal_pending(current))
			break;

		/*
		 * Bounded wait. Because wakeups are batched (a sleeping reader
		 * is only woken once IPA_DIAG_LOG_WAKE_BATCH lines accumulate),
		 * a small trailing burst below that threshold could otherwise
		 * sit unread until the next batch. A periodic timeout makes a
		 * blocking reader re-check the ring on its own, bounding
		 * delivery latency to IPA_DIAG_LOG_POLL_TIMEOUT regardless of
		 * the wake batching, with negligible cost (it only fires while
		 * genuinely idle). poll()-based consumers are unaffected.
		 */
		wait_woken(&wait, TASK_INTERRUPTIBLE, IPA_DIAG_LOG_POLL_TIMEOUT);
	}
out:
	remove_wait_queue(&ctx->waitq, &wait);
	return ret;
}

static unsigned int ipa_diag_log_poll(struct file *filp, poll_table *wait)
{
	struct ipa_diag_log_context *ctx = smp_load_acquire(&ipa3_diag_log_ctx);
	unsigned int mask = 0;
	unsigned long flags;

	if (!ctx)
		return POLLERR;

	poll_wait(filp, &ctx->waitq, wait);

	spin_lock_irqsave(&ctx->lock, flags);
	if (!ctx->ring)
		mask |= POLLERR;
	else if (!ipa_diag_log_empty(ctx))
		mask |= POLLIN | POLLRDNORM;
	spin_unlock_irqrestore(&ctx->lock, flags);

	return mask;
}

static const struct file_operations ipa_diag_log_fops = {
	.owner = THIS_MODULE,
	.open = ipa_diag_log_open,
	.release = ipa_diag_log_release,
	.read = ipa_diag_log_read,
	.poll = ipa_diag_log_poll,
};

int ipa3_diag_log_init(void)
{
	struct ipa_diag_log_context *ctx = &diag_log_ctx_storage;
	struct cdev *cdev;
	int result;

	/*
	 * Idempotent: if the char device is already registered, do not run
	 * cdev_init()/cdev_add() again on the embedded static cdev. Re-running
	 * cdev_init() would re-initialize a kobject that may still be referenced
	 * (e.g. by an open fd), clobbering its refcount and leading to a later
	 * underflow. A genuine re-init must be preceded by ipa3_diag_log_cleanup()
	 * (which clears cdev_added), e.g. on probe-failure unwind then probe retry.
	 */
	if (ctx->cdev_added) {
		IPADBG("diag_ipa already initialized\n");
		return 0;
	}

	/*
	 * Static ctx reused across re-init (e.g. probe-failure unwind followed by
	 * a probe retry). Reset only the diagnostic counters here; every other
	 * field is explicitly (re)assigned below. Deliberately do NOT memset the
	 * whole struct: it embeds the cdev kobject, the spinlock and the wait
	 * queue, any of which may still be referenced after a non-blocking
	 * cdev_del() (cdev_del does not revoke already-open fds) or by a reader
	 * that has not yet finished unwinding — zeroing those would corrupt live
	 * kernel objects.
	 */
	memset(&ctx->stats, 0, sizeof(ctx->stats));

	/* vzalloc: 512KB ring, not physically contiguous and not DMA'd. */
	ctx->ring = vzalloc(sizeof(*ctx->ring) * IPA_DIAG_LOG_NUM_SLOTS);
	if (!ctx->ring) {
		result = -ENOMEM;
		goto fail_ring;
	}

	spin_lock_init(&ctx->lock);
	init_waitqueue_head(&ctx->waitq);
	ctx->head = 0;
	ctx->tail = 0;
	ctx->dropped_reported = 0;
	ctx->enabled = true;	/* default-on; runtime knob can disable */
	ctx->min_level = IPA_DIAG_LVL_DBG;	/* capture ERR/INFO/DBG, drop LOW */
	ctx->drop_newest = false;		/* default: keep freshest */
	strscpy(ctx->sink_cfg, "diag", sizeof(ctx->sink_cfg));

	ctx->cdev.class = class_create(IPA_DIAG_LOG_DEV_NAME);
	if (IS_ERR(ctx->cdev.class)) {
		IPAERR("diag_ipa class_create failed\n");
		result = -ENODEV;
		goto fail_class;
	}

	result = alloc_chrdev_region(&ctx->cdev.dev_num, 0, 1,
			IPA_DIAG_LOG_DEV_NAME);
	if (result) {
		IPAERR("diag_ipa alloc_chrdev_region err=%d\n", result);
		result = -ENODEV;
		goto fail_alloc_region;
	}

	ctx->cdev.dev = device_create(ctx->cdev.class, NULL,
			ctx->cdev.dev_num, ctx, IPA_DIAG_LOG_DEV_NAME);
	if (IS_ERR(ctx->cdev.dev)) {
		IPAERR("diag_ipa device_create err:%ld\n",
				PTR_ERR(ctx->cdev.dev));
		result = PTR_ERR(ctx->cdev.dev);
		goto fail_device;
	}

	cdev = &ctx->cdev.cdev;
	cdev_init(cdev, &ipa_diag_log_fops);
	cdev->owner = THIS_MODULE;
	cdev->ops = &ipa_diag_log_fops;

	result = cdev_add(cdev, ctx->cdev.dev_num, 1);
	if (result) {
		IPAERR("diag_ipa cdev_add err=%d\n", -result);
		goto fail_cdev_add;
	}
	ctx->cdev_added = true;

	ctx->initialized = true;
	/*
	 * Publish last with release semantics: pairs with the acquire-load in
	 * ipa3_diag_log_write()/read()/poll() so that a non-NULL ctx implies
	 * ->ring and the rest of the struct are fully visible on weakly-ordered
	 * architectures.
	 */
	smp_store_release(&ipa3_diag_log_ctx, ctx);

	IPADBG("diag_ipa cdev added. major:%d minor:%d slots:%d line:%d\n",
			MAJOR(ctx->cdev.dev_num), MINOR(ctx->cdev.dev_num),
			IPA_DIAG_LOG_NUM_SLOTS, IPA_DIAG_LOG_LINE_SZ);
	return 0;

fail_cdev_add:
	device_destroy(ctx->cdev.class, ctx->cdev.dev_num);
fail_device:
	unregister_chrdev_region(ctx->cdev.dev_num, 1);
fail_alloc_region:
	class_destroy(ctx->cdev.class);
fail_class:
	vfree(ctx->ring);
	ctx->ring = NULL;
fail_ring:
	return result;
}

void ipa3_diag_log_cleanup(void)
{
	struct ipa_diag_log_context *ctx = ipa3_diag_log_ctx;
	unsigned long flags;
	void *ring;

	if (!ctx)
		return;

	/*
	 * Stop new writers: clear enabled and unpublish the pointer. A
	 * producer that already acquire-loaded a non-NULL ctx will still take
	 * the lock and re-check ->ring before touching it.
	 */
	WRITE_ONCE(ctx->enabled, false);
	ctx->initialized = false;
	/* Unpublish (release) so new writers observe NULL. */
	smp_store_release(&ipa3_diag_log_ctx, NULL);

	/*
	 * Remove the char device exactly once. cdev_del() waits for in-flight
	 * fops to return, but a reader BLOCKED in wait_woken is not "in-flight"
	 * for cdev_del — so we must wake it and let it observe ring == NULL.
	 *
	 * Guard with cdev_added so a second cleanup() (e.g. probe-failure unwind
	 * followed by module/driver remove, or a redundant teardown) does not
	 * call cdev_del() a second time on an already-deleted cdev: that second
	 * kobject_put() would drive the embedded cdev kobject refcount below zero
	 * (refcount underflow / use-after-free). cdev_add() and cdev_del() must
	 * be balanced 1:1 on the embedded static cdev.
	 */
	if (ctx->cdev_added) {
		cdev_del(&ctx->cdev.cdev);
		device_destroy(ctx->cdev.class, ctx->cdev.dev_num);
		unregister_chrdev_region(ctx->cdev.dev_num, 1);
		class_destroy(ctx->cdev.class);
		ctx->cdev_added = false;
	}

	/*
	 * Tear down the ring under the lock: NULL the pointer so any reader or
	 * writer that holds the lock sees it gone, then free outside the lock.
	 * The context itself is static and never freed, so a reader still
	 * sleeping on ctx->waitq (woken below) can safely run remove_wait_queue
	 * and return -ENODEV without dereferencing freed memory.
	 */
	spin_lock_irqsave(&ctx->lock, flags);
	ring = ctx->ring;
	ctx->ring = NULL;
	spin_unlock_irqrestore(&ctx->lock, flags);

	/* Wake any blocked reader so it observes ring == NULL and exits. */
	wake_up_interruptible(&ctx->waitq);

	/*
	 * A reader woken above may still be between wait_woken and
	 * remove_wait_queue. Because the context is static, that is safe; we
	 * only need to ensure the ring memory is not freed while a reader
	 * could still touch it. Readers re-check ctx->ring under the lock
	 * before every slot access and bail when it is NULL, and the lock
	 * above ordered that NULL-store before this free.
	 */
	vfree(ring);
}
