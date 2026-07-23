// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 */
#ifndef CONFIG_DEBUG_FS
#include <linux/completion.h>
#include <linux/dma-mapping.h>
#include <linux/sysfs.h>
#include <linux/random.h>
#include <linux/uaccess.h>
#include <linux/msm_gsi.h>
#include "gsi.h"
#include "gsihal.h"

#define GSI_MAX_MSG_LEN 4096

#define TERR(fmt, args...) \
		pr_err("%s:%d " fmt, __func__, __LINE__, ## args)
#define TDBG(fmt, args...) \
		pr_debug("%s:%d " fmt, __func__, __LINE__, ## args)
#define PRT_STAT(fmt, args...) \
		pr_err(fmt, ## args)

static char dbg_buff[GSI_MAX_MSG_LEN];
static void *gsi_ipc_logbuf_low;

static void gsi_wq_print_dp_stats(struct work_struct *work);
static DECLARE_DELAYED_WORK(gsi_print_dp_stats_work, gsi_wq_print_dp_stats);
static void gsi_wq_update_dp_stats(struct work_struct *work);
static DECLARE_DELAYED_WORK(gsi_update_dp_stats_work, gsi_wq_update_dp_stats);

static ssize_t ev_dump_store(struct device *dev, struct device_attribute *attr, const char *ubuf, size_t count)
{
	u32 arg1;
	u32 arg2;
	char *sptr, *token;
	uint32_t val;
	struct gsi_evt_ctx *ctx;
	uint16_t i;

	if (count >= sizeof(dbg_buff))
		return -EINVAL;

	memcpy(dbg_buff, ubuf, count);

	dbg_buff[count] = '\0';

	sptr = dbg_buff;

	token = strsep(&sptr, " ");
	if (!token)
		return -EINVAL;
	if (kstrtou32(token, 0, &arg1))
		return -EINVAL;

	token = strsep(&sptr, " ");
	if (!token)
		return -EINVAL;
	if (kstrtou32(token, 0, &arg2))
		return -EINVAL;

	TDBG("arg1=%u arg2=%u\n", arg1, arg2);

	if (arg1 >= gsi_ctx->max_ev) {
		TERR("invalid evt ring id %u\n", arg1);
		return -EINVAL;
	}

	gsi_ctx->per.vote_clk_cb();

	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_0,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX0  0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_1,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX1  0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_2,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX2  0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_3,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX3  0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_4,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX4  0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_5,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX5  0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_6,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX6  0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_7,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX7  0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_8,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX8  0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_9,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX9  0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_10,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX10 0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_11,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX11 0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_12,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX12 0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_CNTXT_13,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d CTX13 0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_SCRATCH_0,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d SCR0  0x%x\n", arg1, val);
	val = gsihal_read_reg_nk(GSI_EE_n_EV_CH_k_SCRATCH_1,
		gsi_ctx->per.ee, arg1);
	TERR("EV%2d SCR1  0x%x\n", arg1, val);

	gsi_ctx->per.unvote_clk_cb();

	if (arg2) {
		ctx = &gsi_ctx->evtr[arg1];

		if (ctx->props.ring_base_vaddr) {
			for (i = 0; i < ctx->props.ring_len / 16; i++)
				TERR("EV%2d (0x%08llx) %08x %08x %08x %08x\n",
				arg1, ctx->props.ring_base_addr + i * 16,
				*(u32 *)((u8 *)ctx->props.ring_base_vaddr +
					i * 16 + 0),
				*(u32 *)((u8 *)ctx->props.ring_base_vaddr +
					i * 16 + 4),
				*(u32 *)((u8 *)ctx->props.ring_base_vaddr +
					i * 16 + 8),
				*(u32 *)((u8 *)ctx->props.ring_base_vaddr +
					i * 16 + 12));
		} else {
			TERR("No VA supplied for event ring id %u\n", arg1);
		}
	}

	return count;
}

static ssize_t ch_dump_store(struct device *dev, struct device_attribute *attr, const char *ubuf, size_t count)
{
	u32 arg1;
	u32 arg2;
	char *sptr, *token;
	struct gsi_chan_ctx *ctx;
	uint16_t i;

	if (count >= sizeof(dbg_buff))
		return -EINVAL;

	memcpy(dbg_buff, ubuf, count);

	dbg_buff[count] = '\0';

	sptr = dbg_buff;

	token = strsep(&sptr, " ");
	if (!token)
		return -EINVAL;
	if (kstrtou32(token, 0, &arg1))
		return -EINVAL;

	token = strsep(&sptr, " ");
	if (!token)
		return -EINVAL;
	if (kstrtou32(token, 0, &arg2))
		return -EINVAL;

	TDBG("arg1=%u arg2=%u\n", arg1, arg2);

	if (arg1 >= gsi_ctx->max_ch) {
		TERR("invalid chan id %u\n", arg1);
		return -EINVAL;
	}

	gsi_ctx->per.vote_clk_cb();
	gsi_dump_ch_info(arg1, 0, NULL);
	gsi_ctx->per.unvote_clk_cb();

	if (arg2) {
		ctx = &gsi_ctx->chan[arg1];

		if (ctx->props.ring_base_vaddr) {
			for (i = 0; i < ctx->props.ring_len / 16; i++)
				TERR("CH%2d (0x%08llx) %08x %08x %08x %08x\n",
				arg1, ctx->props.ring_base_addr + i * 16,
				*(u32 *)((u8 *)ctx->props.ring_base_vaddr +
					i * 16 + 0),
				*(u32 *)((u8 *)ctx->props.ring_base_vaddr +
					i * 16 + 4),
				*(u32 *)((u8 *)ctx->props.ring_base_vaddr +
					i * 16 + 8),
				*(u32 *)((u8 *)ctx->props.ring_base_vaddr +
					i * 16 + 12));
		} else {
			TERR("No VA supplied for chan id %u\n", arg1);
		}
	}

	return count;
}

static void gsi_dump_ch_stats(struct gsi_chan_ctx *ctx)
{
	if (!ctx->allocated)
		return;

	PRT_STAT("CH%2d:\n", ctx->props.ch_id);
	PRT_STAT("queued=%lu compl=%lu\n",
		ctx->stats.queued,
		ctx->stats.completed);
	PRT_STAT("cb->poll=%lu poll->cb=%lu poll_pend_irq=%lu\n",
		ctx->stats.callback_to_poll,
		ctx->stats.poll_to_callback,
		ctx->stats.poll_pending_irq);
	PRT_STAT("invalid_tre_error=%lu\n",
		ctx->stats.invalid_tre_error);
	PRT_STAT("poll_ok=%lu poll_empty=%lu\n",
		ctx->stats.poll_ok, ctx->stats.poll_empty);
	if (ctx->evtr)
		PRT_STAT("compl_evt=%lu\n",
			ctx->evtr->stats.completed);
	PRT_STAT("userdata_in_use=%lu\n", ctx->stats.userdata_in_use);

	PRT_STAT("ch_below_lo=%lu\n", ctx->stats.dp.ch_below_lo);
	PRT_STAT("ch_below_hi=%lu\n", ctx->stats.dp.ch_below_hi);
	PRT_STAT("ch_above_hi=%lu\n", ctx->stats.dp.ch_above_hi);
	PRT_STAT("time_empty=%lums\n", ctx->stats.dp.empty_time);
	PRT_STAT("\n");
}

static ssize_t gsi_stats_store(struct device *dev, struct device_attribute *attr, const char *ubuf, size_t count)
{
	int ch_id;
	int min, max, ret;

	ret = kstrtos32(ubuf, 0, &ch_id);
	if (ret)
		return ret;

	if (ch_id == -1) {
		min = 0;
		max = gsi_ctx->max_ch;
	} else if (ch_id < 0 || ch_id >= gsi_ctx->max_ch ||
		   !gsi_ctx->chan[ch_id].allocated) {
		goto error;
	} else {
		min = ch_id;
		max = ch_id + 1;
	}

	for (ch_id = min; ch_id < max; ch_id++)
		gsi_dump_ch_stats(&gsi_ctx->chan[ch_id]);

	return count;
error:
	TERR("Usage: echo ch_id > stats. Use -1 for all\n");
	return -EINVAL;
}

static int gsi_dbg_create_stats_wq(void)
{
	gsi_ctx->dp_stat_wq =
		create_singlethread_workqueue("gsi_stat");
	if (!gsi_ctx->dp_stat_wq) {
		TERR("failed create workqueue\n");
		return -ENOMEM;
	}

	return 0;
}

static void gsi_dbg_destroy_stats_wq(void)
{
	cancel_delayed_work_sync(&gsi_update_dp_stats_work);
	cancel_delayed_work_sync(&gsi_print_dp_stats_work);
	flush_workqueue(gsi_ctx->dp_stat_wq);
	destroy_workqueue(gsi_ctx->dp_stat_wq);
	gsi_ctx->dp_stat_wq = NULL;
}

static ssize_t enable_dp_stats_store(struct device *dev, struct device_attribute *attr, const char *ubuf, size_t count)
{
	int ch_id;
	bool enable;
	int ret;

	if (count >= sizeof(dbg_buff))
		goto error;

	if (memcpy(dbg_buff, ubuf, count))
		goto error;

	dbg_buff[count] = '\0';

	if (dbg_buff[0] != '+' && dbg_buff[0] != '-')
		goto error;

	enable = (dbg_buff[0] == '+');

	if (kstrtos32(dbg_buff + 1, 0, &ch_id))
		goto error;

	if (ch_id < 0 || ch_id >= gsi_ctx->max_ch ||
	    !gsi_ctx->chan[ch_id].allocated) {
		goto error;
	}

	if (gsi_ctx->chan[ch_id].enable_dp_stats == enable) {
		TERR("ch_%d: already enabled/disabled\n", ch_id);
		return -EINVAL;
	}
	gsi_ctx->chan[ch_id].enable_dp_stats = enable;

	if (enable)
		gsi_ctx->num_ch_dp_stats++;
	else
		gsi_ctx->num_ch_dp_stats--;

	if (enable) {
		if (gsi_ctx->num_ch_dp_stats == 1) {
			ret = gsi_dbg_create_stats_wq();
			if (ret)
				return ret;
		}
		cancel_delayed_work_sync(&gsi_update_dp_stats_work);
		queue_delayed_work(gsi_ctx->dp_stat_wq,
			&gsi_update_dp_stats_work, msecs_to_jiffies(10));
	} else if (!enable && gsi_ctx->num_ch_dp_stats == 0) {
		gsi_dbg_destroy_stats_wq();
	}

	return count;
error:
	TERR("Usage: echo [+-]ch_id > enable_dp_stats\n");
	return -EINVAL;
}

static ssize_t max_elem_dp_stats_store(struct device *dev, struct device_attribute *attr, const char *ubuf, size_t count)
{
	u32 ch_id;
	u32 max_elem;
	char *sptr, *token;

	if (count >= sizeof(dbg_buff))
		goto error;

	memcpy(dbg_buff, ubuf, count);

	dbg_buff[count] = '\0';

	sptr = dbg_buff;

	token = strsep(&sptr, " ");
	if (!token) {
		TERR("\n");
		goto error;
	}

	if (kstrtou32(token, 0, &ch_id)) {
		TERR("\n");
		goto error;
	}

	token = strsep(&sptr, " ");
	if (!token) {
		/* get */
		if (kstrtou32(dbg_buff, 0, &ch_id))
			goto error;
		if (ch_id >= gsi_ctx->max_ch)
			goto error;
		PRT_STAT("ch %d: max_re_expected=%d\n", ch_id,
			gsi_ctx->chan[ch_id].props.max_re_expected);
		return count;
	}
	if (kstrtou32(token, 0, &max_elem)) {
		TERR("\n");
		goto error;
	}

	TDBG("ch_id=%u max_elem=%u\n", ch_id, max_elem);

	if (ch_id >= gsi_ctx->max_ch) {
		TERR("invalid chan id %u\n", ch_id);
		goto error;
	}

	gsi_ctx->chan[ch_id].props.max_re_expected = max_elem;

	return count;

error:
	TERR("Usage: (set) echo <ch_id> <max_elem> > max_elem_dp_stats\n");
	TERR("Usage: (get) echo <ch_id> > max_elem_dp_stats\n");
	return -EINVAL;
}

static void gsi_wq_print_dp_stats(struct work_struct *work)
{
	int ch_id;

	for (ch_id = 0; ch_id < gsi_ctx->max_ch; ch_id++) {
		if (gsi_ctx->chan[ch_id].print_dp_stats)
			gsi_dump_ch_stats(&gsi_ctx->chan[ch_id]);
	}

	queue_delayed_work(gsi_ctx->dp_stat_wq, &gsi_print_dp_stats_work,
		msecs_to_jiffies(1000));
}

static void gsi_dbg_update_ch_dp_stats(struct gsi_chan_ctx *ctx)
{
	uint16_t start_hw;
	uint16_t end_hw;
	uint64_t rp_hw;
	uint64_t wp_hw;
	int ee = gsi_ctx->per.ee;
	uint16_t used_hw;

	gsi_ctx->per.vote_clk_cb();

	rp_hw = gsihal_read_reg_nk(GSI_EE_n_GSI_CH_k_CNTXT_4,
		ee, ctx->props.ch_id);
	rp_hw |= ((uint64_t)gsihal_read_reg_nk(GSI_EE_n_GSI_CH_k_CNTXT_5,
		ee, ctx->props.ch_id)) << 32;

	wp_hw = gsihal_read_reg_nk(GSI_EE_n_GSI_CH_k_CNTXT_6,
		ee, ctx->props.ch_id);
	wp_hw |= ((uint64_t)gsihal_read_reg_nk(GSI_EE_n_GSI_CH_k_CNTXT_7,
		ee, ctx->props.ch_id)) << 32;

	gsi_ctx->per.unvote_clk_cb();

	start_hw = gsi_find_idx_from_addr(&ctx->ring, rp_hw);
	end_hw = gsi_find_idx_from_addr(&ctx->ring, wp_hw);

	if (end_hw >= start_hw)
		used_hw = end_hw - start_hw;
	else
		used_hw = ctx->ring.max_num_elem + 1 - (start_hw - end_hw);

	TDBG("ch %d used %d\n", ctx->props.ch_id, used_hw);
	gsi_update_ch_dp_stats(ctx, used_hw);
}

static void gsi_wq_update_dp_stats(struct work_struct *work)
{
	int ch_id;

	for (ch_id = 0; ch_id < gsi_ctx->max_ch; ch_id++) {
		if (gsi_ctx->chan[ch_id].allocated &&
		    gsi_ctx->chan[ch_id].enable_dp_stats)
			gsi_dbg_update_ch_dp_stats(&gsi_ctx->chan[ch_id]);
	}

	queue_delayed_work(gsi_ctx->dp_stat_wq, &gsi_update_dp_stats_work,
		msecs_to_jiffies(10));
}


static ssize_t rst_stats_store(struct device *dev, struct device_attribute *attr, const char *ubuf, size_t count)
{
	int ch_id;
	int min, max, ret;

	ret = kstrtos32(ubuf, 0, &ch_id);
	if (ret)
		return ret;

	if (ch_id == -1) {
		min = 0;
		max = gsi_ctx->max_ch;
	} else if (ch_id < 0 || ch_id >= gsi_ctx->max_ch ||
		   !gsi_ctx->chan[ch_id].allocated) {
		goto error;
	} else {
		min = ch_id;
		max = ch_id + 1;
	}

	for (ch_id = min; ch_id < max; ch_id++)
		memset(&gsi_ctx->chan[ch_id].stats, 0,
			sizeof(gsi_ctx->chan[ch_id].stats));

	return count;
error:
	TERR("Usage: echo ch_id > rst_stats. Use -1 for all\n");
	return -EINVAL;
}

static ssize_t print_dp_stats_store(struct device *dev, struct device_attribute *attr, const char *ubuf, size_t count)
{
	int ch_id;
	bool enable;
	int ret;

	if (count >= sizeof(dbg_buff))
		goto error;

	memcpy(dbg_buff, ubuf, count);

	dbg_buff[count] = '\0';

	if (dbg_buff[0] != '+' && dbg_buff[0] != '-')
		goto error;

	enable = (dbg_buff[0] == '+');

	if (kstrtos32(dbg_buff + 1, 0, &ch_id))
		goto error;

	if (ch_id < 0 || ch_id >= gsi_ctx->max_ch ||
	    !gsi_ctx->chan[ch_id].allocated) {
		goto error;
	}

	if (gsi_ctx->chan[ch_id].print_dp_stats == enable) {
		TERR("ch_%d: already enabled/disabled\n", ch_id);
		return -EINVAL;
	}
	gsi_ctx->chan[ch_id].print_dp_stats = enable;

	if (enable)
		gsi_ctx->num_ch_dp_stats++;
	else
		gsi_ctx->num_ch_dp_stats--;

	if (enable) {
		if (gsi_ctx->num_ch_dp_stats == 1) {
			ret = gsi_dbg_create_stats_wq();
			if (ret)
				return ret;
		}
		cancel_delayed_work_sync(&gsi_print_dp_stats_work);
		queue_delayed_work(gsi_ctx->dp_stat_wq,
			&gsi_print_dp_stats_work, msecs_to_jiffies(10));
	} else if (!enable && gsi_ctx->num_ch_dp_stats == 0) {
		gsi_dbg_destroy_stats_wq();
	}

	return count;
error:
	TERR("Usage: echo [+-]ch_id > print_dp_stats\n");
	return -EINVAL;
}

static ssize_t ipc_low_store(struct device *dev, struct device_attribute *attr, const char *ubuf, size_t count)
{
	s8 option = 0;
	int ret;

	ret = kstrtos8(ubuf, 0, &option);
	if (ret)
		return ret;

	mutex_lock(&gsi_ctx->mlock);
	if (option) {
		if (!gsi_ipc_logbuf_low) {
			gsi_ipc_logbuf_low =
				ipc_log_context_create(GSI_IPC_LOG_PAGES,
					"gsi_low", MINIDUMP_MASK);
			if (gsi_ipc_logbuf_low == NULL)
				TERR("failed to get ipc_logbuf_low\n");
		}
		gsi_ctx->ipc_logbuf_low = gsi_ipc_logbuf_low;
	} else {
		gsi_ctx->ipc_logbuf_low = NULL;
	}
	mutex_unlock(&gsi_ctx->mlock);

	return count;
}

static ssize_t gsi_hw_profiling_stats_show(struct device *dev, struct device_attribute *attr, char *ubuf)
{
	struct gsi_hw_profiling_data stats;
	int ret, nbytes, cnt = 0;
	u64 totalCycles = 0, util = 0;

	if (gsi_ctx->per.ver < GSI_VER_2_9) {
		nbytes = scnprintf(dbg_buff, GSI_MAX_MSG_LEN,
			"This feature only support on GSI2.9+\n");
		cnt += nbytes;
		goto done;
	}

	gsi_ctx->per.vote_clk_cb();
	ret = gsi_get_hw_profiling_stats(&stats);
	gsi_ctx->per.unvote_clk_cb();

	if (!ret) {
		totalCycles = stats.mcs_busy_cnt + stats.mcs_idle_cnt +
			stats.bp_and_pending_cnt;
		if (totalCycles != 0)
			util = div_u64(
				100 * (stats.mcs_busy_cnt + stats.bp_and_pending_cnt),
				totalCycles);
		else
			util = 0;

		nbytes = scnprintf(dbg_buff, GSI_MAX_MSG_LEN,
			"bp_count=0x%llx\n"
			"bp_and_pending_count=0x%llx\n"
			"mcs_busy=0x%llx\n"
			"mcs_idle=0x%llx\n"
			"total_cycle_count=0x%llx\n"
			"utilization_percentage=%llu%%\n",
			stats.bp_cnt,
			stats.bp_and_pending_cnt,
			stats.mcs_busy_cnt,
			stats.mcs_idle_cnt,
			totalCycles,
			util);
		cnt += nbytes;
	} else {
		nbytes = scnprintf(dbg_buff, GSI_MAX_MSG_LEN,
			"Fail to read GSI HW Profiling stats\n");
		cnt += nbytes;
	}
done:
	memcpy(ubuf, dbg_buff, cnt);
	return cnt;
}

static ssize_t gsi_fw_version_show(struct device *dev, struct device_attribute *attr, char *ubuf)
{
	struct gsi_fw_version ver;
	int ret, nbytes, cnt = 0;

	if (gsi_ctx->per.ver < GSI_VER_2_9) {
		nbytes = scnprintf(dbg_buff, GSI_MAX_MSG_LEN,
			"This feature only support on GSI2.9+\n");
		cnt += nbytes;
		goto done;
	}

	gsi_ctx->per.vote_clk_cb();
	ret = gsi_get_fw_version(&ver);
	gsi_ctx->per.unvote_clk_cb();

	if (!ret) {
		nbytes = scnprintf(dbg_buff, GSI_MAX_MSG_LEN,
			"hw=%d\nflavor=%d\nfw=%d\n",
			ver.hw,
			ver.flavor,
			ver.fw);
		cnt += nbytes;
	} else {
		nbytes = scnprintf(dbg_buff, GSI_MAX_MSG_LEN,
			"Fail to read GSI FW version\n");
		cnt += nbytes;
	}
done:
	memcpy(ubuf, dbg_buff, cnt);
	return cnt;
}

static DEVICE_ATTR_WO(ev_dump);
static DEVICE_ATTR_WO(ch_dump);
static DEVICE_ATTR_WO(gsi_stats);
static DEVICE_ATTR_WO(enable_dp_stats);
static DEVICE_ATTR_WO(max_elem_dp_stats);
static DEVICE_ATTR_WO(rst_stats);
static DEVICE_ATTR_WO(print_dp_stats);
static DEVICE_ATTR_WO(ipc_low);
static DEVICE_ATTR_RO(gsi_hw_profiling_stats);
static DEVICE_ATTR_RO(gsi_fw_version);

static struct attribute *ipa_gsi_attrs[] = {
	&dev_attr_ev_dump.attr,
	&dev_attr_ch_dump.attr,
	&dev_attr_gsi_stats.attr,
	&dev_attr_enable_dp_stats.attr,
	&dev_attr_max_elem_dp_stats.attr,
	&dev_attr_rst_stats.attr,
	&dev_attr_print_dp_stats.attr,
	&dev_attr_ipc_low.attr,
	&dev_attr_gsi_hw_profiling_stats.attr,
	&dev_attr_gsi_fw_version.attr,
	NULL
};

const struct attribute_group ipa_gsi_attr_group = {
	.name		= "gsi",
	.attrs		= ipa_gsi_attrs,
};

int gsi_sysfs_init(void)
{
	int ret = -1;

	/* Create directory in /sys/kernel/gsi/ */
	
	ret = sysfs_create_group(kernel_kobj, &ipa_gsi_attr_group);
	if (ret != 0) {
		pr_err("Fail to create GSI syfs attribute\n");
	}
	return ret;
}

void gsi_sysfs_destroy(void)
{
	sysfs_remove_group(kernel_kobj, &ipa_gsi_attr_group);
}
#endif // In case debugfs is disabled
