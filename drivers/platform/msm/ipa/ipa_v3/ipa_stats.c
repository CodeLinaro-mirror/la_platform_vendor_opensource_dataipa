// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 *
 * Copyright (c) 2022-2025 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include "ipa_stats.h"
#include <linux/fs.h>
#include "ipa_i.h"
#include "ipahal.h"
#include "ipa_odl.h"
#include "ipa_common_i.h"
#include <linux/msm_ipa.h>
#include "gsi.h"
#include "ipahal_nat.h"

#define DRIVER_NAME "ipa_lnx_stats_ioctl"
#define DEV_NAME_IPA_LNX_STATS "ipa-lnx-stats"

static struct rt_table_name_lookup rt_table_lookup_table[] = {
	{"ipa_dflt_rt", 1},
	{"COMRTBLLANv4", 2},
	{"WANRTBLv4", 3},
	{"ODURTBLv4", 4},
	{"ipa_dflt_wan_rt", 5},
	{"COMRTBLv6", 6},
	{"WANRTBLv6", 7},
	{"ODURTBLv6", 8},
	{"IPSEC_ENCAP_v4", 9},
	{"IPSEC_ENCAP_v6", 10},
	{"IPSEC_DECAP_v4", 11},
	{"IPSEC_DECAP_v6", 12},
	{"IPSEC_DECAP_NO_POLICY_v4", 13},
	{"IPSEC_DECAP_NO_POLICY_v6", 14},
	{"RT_TABLE_NAME_MAX", 15},
};

#define IPA_STATS_DBG(fmt, args...) \
	do { \
		pr_debug(DEV_NAME_IPA_LNX_STATS " %s:%d " fmt, __func__,\
				__LINE__, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf(), \
				DEV_NAME_IPA_LNX_STATS " %s:%d " fmt, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
				DEV_NAME_IPA_LNX_STATS " %s:%d " fmt, ## args); \
	} while (0)

#define IPA_STATS_ERR(fmt, args...) \
	do { \
		pr_err(DEV_NAME_IPA_LNX_STATS " %s:%d " fmt, __func__,\
				__LINE__, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf(), \
				DEV_NAME_IPA_LNX_STATS " %s:%d " fmt, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
				DEV_NAME_IPA_LNX_STATS " %s:%d " fmt, ## args); \
	} while (0)

#define IPA_PERIPHERAL_STATS_MDM_NUM_ENTRIES 20
#define IPA_PERIPHERAL_STATS_MSM_NUM_ENTRIES 12

static unsigned int dev_num = 1;
static struct cdev ipa_lnx_stats_ioctl_cdev;
static struct class *class;
static dev_t device;

struct ipa_lnx_stats_tlpd_ctx ipa_lnx_agent_ctx;
static DEFINE_MUTEX(ipa_lnx_ctx_mutex);

struct wlan_intf_mode_cnt {
	u8 ap_cnt;
	u8 sta_cnt;
};

enum wlan_intf_mode {
	AP,
	AP_AP,
	AP_STA,
	AP_AP_STA,
	AP_AP_AP,
	AP_AP_AP_STA,
	AP_AP_AP_AP_,
	WLAN_INTF_MODE_MAX
};

union ipa_gsi_ring_prev_poll_info {
	struct {
		uint32_t num_tx_ring_100_perc_with_cred;
		uint32_t num_tx_ring_0_perc_with_cred;
		uint32_t num_tx_ring_above_75_perc_cred;
		uint32_t num_tx_ring_above_25_perc_cred;
		uint32_t num_tx_ring_stats_polled;
	} tx_cred_info;
	struct {
		uint32_t num_rx_ring_100_perc_with_pack;
		uint32_t num_rx_ring_0_perc_with_pack;
		uint32_t num_rx_ring_above_75_perc_pack;
		uint32_t num_rx_ring_above_25_perc_pack;
		uint32_t num_rx_ring_stats_polled;
	} rx_pack_info;
};

union ipa_gsi_ring_prev_poll_info poll_pack_and_cred_info[IPA_CLIENT_MAX];

static enum wlan_intf_mode ipa_get_wlan_intf_mode(void)
{
	struct wlan_intf_mode_cnt mode_cnt;
	mode_cnt.ap_cnt = ipa3_ctx->stats.msg_w[WLAN_AP_CONNECT] -
		ipa3_ctx->stats.msg_w[WLAN_AP_DISCONNECT];
	mode_cnt.sta_cnt = ipa3_ctx->stats.msg_w[WLAN_STA_CONNECT] -
		ipa3_ctx->stats.msg_w[WLAN_STA_DISCONNECT];
	if ((mode_cnt.ap_cnt < 0) || (mode_cnt.sta_cnt < 0))
		return WLAN_INTF_MODE_MAX;

	switch (mode_cnt.ap_cnt) {
	case 1:
		if (mode_cnt.sta_cnt == 1)
			return AP_STA;
		return AP;
	case 2:
		if (mode_cnt.sta_cnt == 1)
			return AP_AP_STA;
		return AP_AP;
	case 3:
		if (mode_cnt.sta_cnt == 1)
			return AP_AP_AP_STA;
		return AP_AP_AP;
	case 4:
		return AP_AP_AP_AP_;
	default:
		if (mode_cnt.ap_cnt > 4)
			return AP_AP_AP_AP_;
		return WLAN_INTF_MODE_MAX;
	}
}

static int ipa_stats_ioctl_open(struct inode *inode, struct file *filp)
{
	return 0;
}


static int ipa_get_generic_stats(unsigned long arg)
{
	int res;
	int i, j;
	struct ipa_lnx_generic_stats *generic_stats;
	struct ipa_drop_stats_all *out;
	int alloc_size;
	int reg_idx;
	struct ipa_uc_holb_client_info *holb_client;
	struct holb_discard_stats *holb_disc_stats_ptr;
	struct holb_monitor_stats *holb_mon_stats_ptr;

	if (!(ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_GENERIC_STATS)) {
		IPA_STATS_ERR("Log type GENERIC mask not set\n");
		return -EFAULT;
	}

	alloc_size = sizeof(struct ipa_lnx_generic_stats) +
		(sizeof(struct holb_discard_stats) *
			ipa_lnx_agent_ctx.alloc_info.num_holb_drop_stats_clients) +
		(sizeof(struct holb_monitor_stats) *
			ipa_lnx_agent_ctx.alloc_info.num_holb_mon_stats_clients);

	generic_stats = (struct ipa_lnx_generic_stats *) memdup_user((
		const void __user *)arg, alloc_size);
	if (IS_ERR(generic_stats)) {
		IPA_STATS_ERR("copy from user failed");
		return -ENOMEM;
	}

	generic_stats->tx_dma_pkts = ipa3_ctx->stats.tx_sw_pkts;
	generic_stats->tx_hw_pkts = ipa3_ctx->stats.tx_hw_pkts;
	generic_stats->tx_non_linear = ipa3_ctx->stats.tx_non_linear;
	generic_stats->tx_pkts_compl = ipa3_ctx->stats.tx_pkts_compl;
	generic_stats->stats_compl = ipa3_ctx->stats.stat_compl;
	generic_stats->active_eps =
		atomic_read(&ipa3_ctx->ipa3_active_clients.cnt);
	generic_stats->wan_rx_empty = ipa3_ctx->stats.wan_rx_empty;
	generic_stats->wan_repl_rx_empty = ipa3_ctx->stats.wan_repl_rx_empty;
	generic_stats->lan_rx_empty = ipa3_ctx->stats.lan_rx_empty;
	generic_stats->lan_repl_rx_empty = ipa3_ctx->stats.lan_repl_rx_empty;
	/* Page recycle stats */
	generic_stats->pg_rec_stats.coal_total_repl_buff =
		ipa3_ctx->stats.page_recycle_stats[0].total_replenished;
	generic_stats->pg_rec_stats.coal_temp_repl_buff =
		ipa3_ctx->stats.page_recycle_stats[0].tmp_alloc;
	generic_stats->pg_rec_stats.def_total_repl_buff =
		ipa3_ctx->stats.page_recycle_stats[1].total_replenished;
	generic_stats->pg_rec_stats.def_temp_repl_buff =
		ipa3_ctx->stats.page_recycle_stats[1].tmp_alloc;
	/* Exception stats */
	generic_stats->excep_stats.excptn_type_none =
		ipa3_ctx->stats.rx_excp_pkts[IPAHAL_PKT_STATUS_EXCEPTION_NONE];
	generic_stats->excep_stats.excptn_type_deaggr =
		ipa3_ctx->stats.rx_excp_pkts[IPAHAL_PKT_STATUS_EXCEPTION_DEAGGR];
	generic_stats->excep_stats.excptn_type_iptype =
		ipa3_ctx->stats.rx_excp_pkts[IPAHAL_PKT_STATUS_EXCEPTION_IPTYPE];
	generic_stats->excep_stats.excptn_type_pkt_len =
		ipa3_ctx->stats.rx_excp_pkts[
			IPAHAL_PKT_STATUS_EXCEPTION_PACKET_LENGTH];
	generic_stats->excep_stats.excptn_type_pkt_thrshld =
		ipa3_ctx->stats.rx_excp_pkts[
			IPAHAL_PKT_STATUS_EXCEPTION_PACKET_THRESHOLD];
	generic_stats->excep_stats.excptn_type_frag_rule_miss =
		ipa3_ctx->stats.rx_excp_pkts[
			IPAHAL_PKT_STATUS_EXCEPTION_FRAG_RULE_MISS];
	generic_stats->excep_stats.excptn_type_sw_flt =
		ipa3_ctx->stats.rx_excp_pkts[IPAHAL_PKT_STATUS_EXCEPTION_SW_FILT];
	generic_stats->excep_stats.excptn_type_nat =
		ipa3_ctx->stats.rx_excp_pkts[IPAHAL_PKT_STATUS_EXCEPTION_NAT];
	generic_stats->excep_stats.excptn_type_ipv6_ct =
		ipa3_ctx->stats.rx_excp_pkts[IPAHAL_PKT_STATUS_EXCEPTION_IPV6CT];
	generic_stats->excep_stats.excptn_type_csum =
		ipa3_ctx->stats.rx_excp_pkts[IPAHAL_PKT_STATUS_EXCEPTION_CSUM];
	/* ODL EP stats */
	if (ipa3_odl_ctx) {
		generic_stats->odl_stats.rx_pkt = ipa3_odl_ctx->stats.odl_rx_pkt;
		generic_stats->odl_stats.processed_pkt =
			ipa3_odl_ctx->stats.odl_tx_diag_pkt;
		generic_stats->odl_stats.dropped_pkt =
			ipa3_odl_ctx->stats.odl_drop_pkt;
		generic_stats->odl_stats.num_queue_pkt =
			atomic_read(&ipa3_odl_ctx->stats.numer_in_queue);
	}
	/* HOLB discard stats */
	if (!(ipa3_ctx->hw_stats && ipa3_ctx->hw_stats->enabled)) {
		generic_stats->holb_stats.num_holb_disc_pipes = 0;
		generic_stats->holb_stats.num_holb_mon_clients = 0;
	}

	generic_stats->holb_stats.num_holb_disc_pipes =
		ipa_lnx_agent_ctx.alloc_info.num_holb_drop_stats_clients;
	generic_stats->holb_stats.num_holb_mon_clients =
		ipa_lnx_agent_ctx.alloc_info.num_holb_mon_stats_clients;

	out = kzalloc(sizeof(*out), GFP_KERNEL);
	if (!out) {
		kfree(generic_stats);
		return -ENOMEM;
	}

	res = ipa_get_drop_stats(out);
	if (res) {
		kfree(out);
		kfree(generic_stats);
		return res;
	}

	/* HOLB Discard stats */
	holb_disc_stats_ptr = &generic_stats->holb_stats.holb_disc_stats[0];
	for (i = 0; i < IPA_CLIENT_MAX; i++) {
		int ep_idx = ipa_get_ep_mapping(i);

		if ((ep_idx == -1) || (!IPA_CLIENT_IS_CONS(i)) ||
			(IPA_CLIENT_IS_TEST(i)))
			continue;

		reg_idx = ipahal_get_ep_reg_idx(ep_idx);
		if (!(ipa3_ctx->hw_stats &&
			(ipa3_ctx->hw_stats->drop.init.enabled_bitmask[reg_idx] &
			ipahal_get_ep_bit(ep_idx))))
			continue;

		holb_disc_stats_ptr->client_type = i;
		holb_disc_stats_ptr->num_drp_cnt = out->client[i].drop_packet_cnt;
		holb_disc_stats_ptr->num_drp_bytes = out->client[i].drop_byte_cnt;
		holb_disc_stats_ptr = (struct holb_discard_stats *)((
			uint64_t)holb_disc_stats_ptr + sizeof(struct holb_discard_stats));
	}

	/* HOLB Monitor stats */
	holb_mon_stats_ptr = (struct holb_monitor_stats *)(
		(uint64_t)&generic_stats->holb_stats.holb_disc_stats[0] +
		(ipa_lnx_agent_ctx.alloc_info.num_holb_drop_stats_clients *
		sizeof(struct holb_discard_stats)));
	for (i = 0; i < generic_stats->holb_stats.num_holb_mon_clients; i++) {
		holb_client = &(ipa3_ctx->uc_ctx.holb_monitor.client[i]);
		/* Get the client type from gsi_hdl */
		for (j = 0; j < IPA_MAX_NUM_PIPES; j++) {
			if (ipa3_ctx->ep[j].gsi_chan_hdl == holb_client->gsi_chan_hdl) {
				holb_mon_stats_ptr->client_type = ipa3_ctx->ep[j].client;
				break;
			}
		}
		holb_mon_stats_ptr->curr_index = holb_client->current_idx;
		holb_mon_stats_ptr->num_en_cnt = holb_client->enable_cnt;
		holb_mon_stats_ptr->num_dis_cnt = holb_client->disable_cnt;
		holb_mon_stats_ptr = (struct holb_monitor_stats *)((
			uint64_t)holb_mon_stats_ptr + sizeof(struct holb_monitor_stats));
	}

	if(copy_to_user((void __user *)arg,
		(u8 *)generic_stats,
		alloc_size)) {
		kfree(generic_stats);
		kfree(out);
		IPA_STATS_ERR("copy to user failed");
		return -EFAULT;
	}

	kfree(out);
	kfree(generic_stats);
	return 0;
}

static int ipa_get_clock_stats(unsigned long arg)
{
	struct ipa_lnx_clock_stats *clock_stats;
	int i;
	int alloc_size;
	struct pm_client_stats *pm_stats_ptr;

	if (!(ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_CLOCK_STATS)) {
		IPA_STATS_ERR("Log type CLOCK mask not set\n");
		return -EFAULT;
	}

	alloc_size = sizeof(struct ipa_lnx_clock_stats) +
		(sizeof(struct pm_client_stats) *
		ipa_lnx_agent_ctx.alloc_info.num_pm_clients);

	clock_stats = (struct ipa_lnx_clock_stats *) memdup_user((
		const void __user *)arg, alloc_size);
	if (IS_ERR(clock_stats)) {
		IPA_STATS_ERR("copy from user failed\n");
		return -ENOMEM;
	}

	if(ipa_pm_get_scaling_bw_levels(clock_stats))
		IPA_STATS_ERR("Couldn't get scaling bw levels\n");
	clock_stats->aggr_bw =
		ipa_pm_get_aggregated_throughput();
	clock_stats->curr_clk_vote = ipa_pm_get_current_clk_vote();
	clock_stats->active_clients = 0;

	pm_stats_ptr = &clock_stats->pm_clnt_stats[0];
	for (i = 1; i < ipa_lnx_agent_ctx.alloc_info.num_pm_clients; i++) {
		if (ipa_get_pm_client_stats_filled(pm_stats_ptr, i)) {
			clock_stats->active_clients++;
			pm_stats_ptr = (struct pm_client_stats *)((uint64_t)pm_stats_ptr +
				sizeof(struct pm_client_stats));
		}
	}

	if(copy_to_user((void __user *)arg,
		(u8 *)clock_stats,
		alloc_size)) {
		kfree(clock_stats);
		IPA_STATS_ERR("copy to user failed");
		return -EFAULT;
	}

	kfree(clock_stats);
	return 0;
}

/**
 * ipa_get_gsi_pipe_info - API to fill gsi pipe info
 */
static void ipa_get_gsi_pipe_info(
	struct ipa_lnx_pipe_info *pipe_info_ptr_local, struct ipa3_ep_context *ep)
{
	const struct ipa_gsi_ep_config *gsi_ep_info;

	pipe_info_ptr_local->client_type = ep->client;
	if (ep->sys) {
		pipe_info_ptr_local->buff_size = ep->sys->buff_size;
		pipe_info_ptr_local->is_common_evt_ring =
			ep->sys->use_comm_evt_ring;
	}
	pipe_info_ptr_local->direction = IPA_CLIENT_IS_CONS(ep->client);
	pipe_info_ptr_local->num_free_buff = 0;
	pipe_info_ptr_local->gsi_chan_num = ep->gsi_chan_hdl;
	pipe_info_ptr_local->gsi_evt_num = ep->gsi_evt_ring_hdl;

	pipe_info_ptr_local->gsi_prot_type =
		gsi_get_chan_prot_type(ep->gsi_chan_hdl);
	pipe_info_ptr_local->gsi_chan_state =
		gsi_get_chan_state(ep->gsi_chan_hdl);
	pipe_info_ptr_local->gsi_chan_stop_stm =
		gsi_get_chan_stop_stm(ep->gsi_chan_hdl, gsi_get_peripheral_ee());
	pipe_info_ptr_local->gsi_poll_mode =
		gsi_get_chan_poll_mode(ep->gsi_chan_hdl);
	pipe_info_ptr_local->gsi_chan_ring_len =
		gsi_get_ring_len(ep->gsi_chan_hdl);
	pipe_info_ptr_local->gsi_db_in_bytes =
		gsi_get_chan_props_db_in_bytes(ep->gsi_chan_hdl);
	pipe_info_ptr_local->gsi_chan_ring_bp =
		gsi_read_chan_ring_bp(ep->gsi_chan_hdl);
	pipe_info_ptr_local->gsi_chan_ring_rp =
		gsi_read_chan_ring_rp(ep->gsi_chan_hdl, gsi_get_peripheral_ee());
	pipe_info_ptr_local->gsi_chan_ring_wp =
		gsi_read_chan_ring_wp(ep->gsi_chan_hdl, gsi_get_peripheral_ee());

	gsi_ep_info = ipa_get_gsi_ep_info(ep->client);
	pipe_info_ptr_local->gsi_ipa_if_tlv =
		gsi_ep_info ? gsi_ep_info->ipa_if_tlv : 0;
	pipe_info_ptr_local->gsi_ipa_if_aos =
		gsi_ep_info ? gsi_ep_info->ipa_if_aos : 0;

	pipe_info_ptr_local->gsi_desc_size =
		gsi_get_evt_ring_re_size(ep->gsi_evt_ring_hdl);
	pipe_info_ptr_local->gsi_evt_ring_len =
		gsi_get_evt_ring_len(ep->gsi_evt_ring_hdl);
	pipe_info_ptr_local->gsi_evt_ring_bp =
		gsi_read_event_ring_bp(ep->gsi_evt_ring_hdl);
	pipe_info_ptr_local->gsi_evt_ring_rp =
		gsi_get_evt_ring_rp(ep->gsi_evt_ring_hdl);
	pipe_info_ptr_local->gsi_evt_ring_wp =
		gsi_read_event_ring_wp(ep->gsi_evt_ring_hdl, gsi_get_peripheral_ee());
}

/**
 * ipa_lnx_calculate_gsi_ring_summay - API to calculate gsi ring summary
 * GSI tx_summary and rx_summary are calculated based on the difference between
 * the previous poll and the current poll. Both summaries are on a scale of 100
 * and will be rated based upon number of credits left(tx) or number or packets
 * filled(rx). 100 value being efficient and 0 being non efficient/stall/IPA idle
 */
static void ipa_lnx_calculate_gsi_ring_summay(
	struct ipa_lnx_gsi_tx_debug_stats *tx_instance_ptr_local,
	struct ipa_lnx_gsi_rx_debug_stats *rx_instance_ptr_local,
	int client_type)
{
	uint32_t diff_100_perc_cred;
	uint32_t diff_0_perc_cred;
	uint32_t diff_75_perc_cred;
	uint32_t diff_50_perc_cred;
	uint32_t diff_25_perc_cred;
	uint32_t diff_tx_polled;
	uint32_t diff_100_perc_pack;
	uint32_t diff_0_perc_pack;
	uint32_t diff_75_perc_pack;
	uint32_t diff_50_perc_pack;
	uint32_t diff_25_perc_pack;
	uint32_t diff_rx_polled;

	if (IPA_CLIENT_IS_CONS(client_type) && tx_instance_ptr_local) {
		if (tx_instance_ptr_local->num_tx_ring_100_perc_with_cred >=
			poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_100_perc_with_cred)
			diff_100_perc_cred =
				tx_instance_ptr_local->num_tx_ring_100_perc_with_cred -
				poll_pack_and_cred_info[
				client_type].tx_cred_info.num_tx_ring_100_perc_with_cred;
		else diff_100_perc_cred = (0xFFFFFFFF - poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_100_perc_with_cred) +
			tx_instance_ptr_local->num_tx_ring_100_perc_with_cred;
		if (tx_instance_ptr_local->num_tx_ring_above_75_perc_cred >=
			poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_above_75_perc_cred)
			diff_75_perc_cred =
				tx_instance_ptr_local->num_tx_ring_above_75_perc_cred -
				poll_pack_and_cred_info[
					client_type].tx_cred_info.num_tx_ring_above_75_perc_cred;
		else diff_75_perc_cred = (0xFFFFFFFF - poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_above_75_perc_cred) +
			tx_instance_ptr_local->num_tx_ring_above_75_perc_cred;
		if (tx_instance_ptr_local->num_tx_ring_above_25_perc_cred >=
			poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_above_25_perc_cred)
			diff_25_perc_cred =
				tx_instance_ptr_local->num_tx_ring_above_25_perc_cred -
				poll_pack_and_cred_info[
					client_type].tx_cred_info.num_tx_ring_above_25_perc_cred;
		else diff_25_perc_cred = (0xFFFFFFFF - poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_above_25_perc_cred) +
			tx_instance_ptr_local->num_tx_ring_above_25_perc_cred;
		if (tx_instance_ptr_local->num_tx_ring_0_perc_with_cred >=
			poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_0_perc_with_cred)
			diff_0_perc_cred =
				tx_instance_ptr_local->num_tx_ring_0_perc_with_cred -
				poll_pack_and_cred_info[
					client_type].tx_cred_info.num_tx_ring_0_perc_with_cred;
		else diff_0_perc_cred = (0xFFFFFFFF - poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_0_perc_with_cred) +
			tx_instance_ptr_local->num_tx_ring_0_perc_with_cred;
		if (tx_instance_ptr_local->num_tx_ring_stats_polled >=
			poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_stats_polled)
			diff_tx_polled =
				tx_instance_ptr_local->num_tx_ring_stats_polled -
				poll_pack_and_cred_info[
					client_type].tx_cred_info.num_tx_ring_stats_polled;
		else diff_tx_polled = (0xFFFFFFFF - poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_stats_polled) +
			tx_instance_ptr_local->num_tx_ring_stats_polled;

		poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_100_perc_with_cred =
			tx_instance_ptr_local->num_tx_ring_100_perc_with_cred;
		poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_0_perc_with_cred =
			tx_instance_ptr_local->num_tx_ring_0_perc_with_cred;
		poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_above_75_perc_cred =
			tx_instance_ptr_local->num_tx_ring_above_75_perc_cred;
		poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_above_25_perc_cred =
			tx_instance_ptr_local->num_tx_ring_above_25_perc_cred;
		poll_pack_and_cred_info[
			client_type].tx_cred_info.num_tx_ring_stats_polled =
			tx_instance_ptr_local->num_tx_ring_stats_polled;

		diff_50_perc_cred = diff_tx_polled - (diff_100_perc_cred +
			diff_75_perc_cred + diff_25_perc_cred + diff_0_perc_cred);
		/**
		 * TX ring scale(summary) - Varies between 0 to 100
		 * If the value tends towards 0, we can assume following things
		 *	1. DL throughput increasing or
		 *	2. Peripheral not pulling data fast enough.
		 *
		 * If the value tends towards 100, we can assume following things
		 *	1. Client processing data speed increasing or
		 *	2. Equal to 100 when no DL data transfer
		 */
		tx_instance_ptr_local->tx_summary = ((diff_100_perc_cred * 100) +
			(diff_75_perc_cred * 75) +
			(diff_50_perc_cred * 50) +
			(diff_25_perc_cred * 25) +
			(diff_0_perc_cred * 0))/diff_tx_polled;
	} else if(!IPA_CLIENT_IS_CONS(client_type) && rx_instance_ptr_local) {
		if (rx_instance_ptr_local->num_rx_ring_100_perc_with_pack >=
			poll_pack_and_cred_info[
				client_type].rx_pack_info.num_rx_ring_100_perc_with_pack)
			diff_100_perc_pack =
				rx_instance_ptr_local->num_rx_ring_100_perc_with_pack -
				poll_pack_and_cred_info[
					client_type].rx_pack_info.num_rx_ring_100_perc_with_pack;
		else diff_100_perc_pack = (0xFFFFFFFF - poll_pack_and_cred_info[
				client_type].rx_pack_info.num_rx_ring_100_perc_with_pack) +
				rx_instance_ptr_local->num_rx_ring_100_perc_with_pack;
		if (rx_instance_ptr_local->num_rx_ring_above_75_perc_pack >=
			poll_pack_and_cred_info[
				client_type].rx_pack_info.num_rx_ring_above_75_perc_pack)
			diff_75_perc_pack =
				rx_instance_ptr_local->num_rx_ring_above_75_perc_pack -
				poll_pack_and_cred_info[
					client_type].rx_pack_info.num_rx_ring_above_75_perc_pack;
		else diff_75_perc_pack = (0xFFFFFFFF - poll_pack_and_cred_info[
			client_type].rx_pack_info.num_rx_ring_above_75_perc_pack) +
			rx_instance_ptr_local->num_rx_ring_above_75_perc_pack;
		if (rx_instance_ptr_local->num_rx_ring_above_25_perc_pack >=
			poll_pack_and_cred_info[
				client_type].rx_pack_info.num_rx_ring_above_25_perc_pack)
			diff_25_perc_pack =
			rx_instance_ptr_local->num_rx_ring_above_25_perc_pack -
			poll_pack_and_cred_info[
				client_type].rx_pack_info.num_rx_ring_above_25_perc_pack;
		else diff_25_perc_pack = (0xFFFFFFFF - poll_pack_and_cred_info[
			client_type].rx_pack_info.num_rx_ring_above_25_perc_pack) +
			rx_instance_ptr_local->num_rx_ring_above_25_perc_pack;
		if (rx_instance_ptr_local->num_rx_ring_0_perc_with_pack >=
			poll_pack_and_cred_info[
				client_type].rx_pack_info.num_rx_ring_0_perc_with_pack)
			diff_0_perc_pack =
				rx_instance_ptr_local->num_rx_ring_0_perc_with_pack -
				poll_pack_and_cred_info[
					client_type].rx_pack_info.num_rx_ring_0_perc_with_pack;
		else diff_0_perc_pack = (0xFFFFFFFF - poll_pack_and_cred_info[
			client_type].rx_pack_info.num_rx_ring_0_perc_with_pack) +
			rx_instance_ptr_local->num_rx_ring_0_perc_with_pack;
		if (rx_instance_ptr_local->num_rx_ring_stats_polled >=
			poll_pack_and_cred_info[
				client_type].rx_pack_info.num_rx_ring_stats_polled)
			diff_rx_polled =
				rx_instance_ptr_local->num_rx_ring_stats_polled -
				poll_pack_and_cred_info[
					client_type].rx_pack_info.num_rx_ring_stats_polled;
		else diff_rx_polled = (0xFFFFFFFF - poll_pack_and_cred_info[
			client_type].rx_pack_info.num_rx_ring_stats_polled) +
			rx_instance_ptr_local->num_rx_ring_stats_polled;

		poll_pack_and_cred_info[
			client_type].rx_pack_info.num_rx_ring_100_perc_with_pack =
				rx_instance_ptr_local->num_rx_ring_100_perc_with_pack;
		poll_pack_and_cred_info[
			client_type].rx_pack_info.num_rx_ring_0_perc_with_pack =
				rx_instance_ptr_local->num_rx_ring_0_perc_with_pack;
		poll_pack_and_cred_info[
			client_type].rx_pack_info.num_rx_ring_above_75_perc_pack =
				rx_instance_ptr_local->num_rx_ring_above_75_perc_pack;
		poll_pack_and_cred_info[
			client_type].rx_pack_info.num_rx_ring_above_25_perc_pack =
				rx_instance_ptr_local->num_rx_ring_above_25_perc_pack;
		poll_pack_and_cred_info[
			client_type].rx_pack_info.num_rx_ring_stats_polled =
				rx_instance_ptr_local->num_rx_ring_stats_polled;

		diff_50_perc_pack = diff_rx_polled - (diff_100_perc_pack +
			diff_75_perc_pack + diff_25_perc_pack + diff_0_perc_pack);
		/**
		 * RX ring scale(summary) - Varies between 0 to 100
		 * If the value tends towards 0, we can assume following things
		 *	1. UL throughput is increasing or
		 *	2. IPA packet processing speed decreasing or
		 * 	3. Q6 packet pulling speed decreasing or
		 *	4. A7 packet pulling speed decreasing (Lan2Lan)
		 *
		 * If the value tends towards 100, we can assume following things
		 *	1. IPA processing data speed increasing or
		 *	2. Peripheral data pushing speed decreasing or
		 *	3. Equal to 100 during no UL data transfer
		 */
		rx_instance_ptr_local->rx_summary = ((diff_100_perc_pack * 0) +
			(diff_75_perc_pack * 25) +
			(diff_50_perc_pack * 50) +
			(diff_25_perc_pack * 75) +
			(diff_0_perc_pack * 100))/diff_rx_polled;
	}
}

static int ipa_get_wlan_inst_stats(unsigned long arg)
{
	struct ipa_lnx_wlan_inst_stats *wlan_stats;
	int i, j;
	int alloc_size;
	int ep_idx;
	int client_type;
	struct ipa_lnx_pipe_info *pipe_info_ptr = NULL;
	struct ipa_lnx_gsi_tx_debug_stats *tx_instance_ptr = NULL;
	struct ipa_lnx_gsi_rx_debug_stats *rx_instance_ptr = NULL;
	struct ipa_lnx_pipe_info *pipe_info_ptr_local = NULL;
	struct ipa_lnx_gsi_tx_debug_stats *tx_instance_ptr_local = NULL;
	struct ipa_lnx_gsi_rx_debug_stats *rx_instance_ptr_local = NULL;
	struct wlan_instance_info *instance_ptr = NULL;
	struct ipa_uc_dbg_ring_stats stats;

	if (!(ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_WLAN_STATS)) {
		IPA_STATS_ERR("Log type WLAN mask not set\n");
		return -EFAULT;
	}

	alloc_size = sizeof(struct ipa_lnx_wlan_inst_stats) +
			(ipa_lnx_agent_ctx.alloc_info.num_wlan_instances *
			sizeof(struct wlan_instance_info));
	for (i = 0; i < ipa_lnx_agent_ctx.alloc_info.num_wlan_instances; i++) {
		alloc_size = alloc_size +
			(ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[i].num_tx_instances *
			sizeof(struct ipa_lnx_gsi_tx_debug_stats)) +
			(ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[i].num_rx_instances *
			sizeof(struct ipa_lnx_gsi_rx_debug_stats)) +
			(ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[i].num_pipes *
			sizeof(struct ipa_lnx_pipe_info));
	}

	wlan_stats = (struct ipa_lnx_wlan_inst_stats *) memdup_user((
		const void __user *)arg, alloc_size);
	if (IS_ERR(wlan_stats)) {
		IPA_STATS_ERR("copy from user failed");
		return -ENOMEM;
	}

	if (!ipa_lnx_agent_ctx.alloc_info.num_wlan_instances)
		goto success;
	wlan_stats->num_wlan_instance =
		ipa_lnx_agent_ctx.alloc_info.num_wlan_instances;

	instance_ptr = &wlan_stats->instance_info[0];
	for (i = 0; i < wlan_stats->num_wlan_instance; i++) {
		instance_ptr->num_pipes =
			ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[i].num_pipes;
		instance_ptr->gsi_debug_stats.num_tx_instances =
			ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[i].num_tx_instances;
		instance_ptr->gsi_debug_stats.num_rx_instances =
			ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[i].num_rx_instances;
		if(ipa3_get_wdi3_gsi_stats(&stats)) {
			instance_ptr = (struct wlan_instance_info *)((
				uint64_t)instance_ptr + sizeof(struct wlan_instance_info) +
				(instance_ptr->gsi_debug_stats.num_tx_instances *
				sizeof(struct ipa_lnx_gsi_tx_debug_stats)) +
				(instance_ptr->gsi_debug_stats.num_rx_instances *
				sizeof(struct ipa_lnx_gsi_rx_debug_stats)) +
				(instance_ptr->num_pipes * sizeof(struct ipa_lnx_pipe_info)));
			continue;
		}
		instance_ptr->instance_id = i;
		instance_ptr->wdi_ver = ipa_get_wdi_version();
		instance_ptr->wlan_mode = ipa_get_wlan_intf_mode();
		instance_ptr->wdi_over_gsi = ipa3_ctx->ipa_wdi3_over_gsi;
		instance_ptr->dbs_mode = ipa_wdi_is_tx1_used();
		instance_ptr->pm_bandwidth =
			ipa_pm_get_pm_clnt_throughput(
				ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[
				i].tx_inst_client_type[0]);

		tx_instance_ptr = (struct ipa_lnx_gsi_tx_debug_stats *)((
			uint64_t)instance_ptr + sizeof(struct wlan_instance_info));
		for (j = 0; j < ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[
			i].num_tx_instances; j++) {
			tx_instance_ptr_local = (struct ipa_lnx_gsi_tx_debug_stats *)((
				uint64_t)tx_instance_ptr + (j *
				sizeof(struct ipa_lnx_gsi_tx_debug_stats)));

			client_type = ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[
				i].tx_inst_client_type[j];
			tx_instance_ptr_local->tx_client = client_type;
			tx_instance_ptr_local->num_tx_ring_100_perc_with_cred =
				stats.u.ring[1 + j].ringFull;
			tx_instance_ptr_local->num_tx_ring_0_perc_with_cred =
				stats.u.ring[1 + j].ringEmpty;
			tx_instance_ptr_local->num_tx_ring_above_75_perc_cred =
				stats.u.ring[1 + j].ringUsageHigh;
			tx_instance_ptr_local->num_tx_ring_above_25_perc_cred =
				stats.u.ring[1 + j].ringUsageLow;
			tx_instance_ptr_local->num_tx_ring_stats_polled =
				stats.u.ring[1 + j].RingUtilCount;
			ipa_lnx_calculate_gsi_ring_summay(
				tx_instance_ptr_local, NULL, client_type);

		}

		rx_instance_ptr = (struct ipa_lnx_gsi_rx_debug_stats *)((
			uint64_t)instance_ptr + sizeof(struct wlan_instance_info)
			+ (sizeof(struct ipa_lnx_gsi_tx_debug_stats) * (
			ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[
				i].num_tx_instances)));
		for (j = 0; j < ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[
			i].num_rx_instances; j++) {
			rx_instance_ptr_local = (struct ipa_lnx_gsi_rx_debug_stats *)((
				uint64_t)rx_instance_ptr + (j *
				sizeof(struct ipa_lnx_gsi_rx_debug_stats)));

			client_type =
				ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[
					i].rx_inst_client_type[j];
			rx_instance_ptr_local->rx_client =
				client_type;
			rx_instance_ptr_local->num_rx_ring_100_perc_with_pack =
				stats.u.ring[j].ringFull;
			rx_instance_ptr_local->num_rx_ring_0_perc_with_pack =
				stats.u.ring[j].ringEmpty;
			rx_instance_ptr_local->num_rx_ring_above_75_perc_pack =
				stats.u.ring[j].ringUsageHigh;
			rx_instance_ptr_local->num_rx_ring_above_25_perc_pack =
				stats.u.ring[j].ringUsageLow;
			rx_instance_ptr_local->num_rx_ring_stats_polled =
				stats.u.ring[j].RingUtilCount;
			rx_instance_ptr_local->num_rx_drop_stats = 0;
			ipa_lnx_calculate_gsi_ring_summay(
				NULL, rx_instance_ptr_local, client_type);

		}

		pipe_info_ptr = (struct ipa_lnx_pipe_info *)((uint64_t)instance_ptr +
			sizeof(struct wlan_instance_info)
			+ (sizeof(struct ipa_lnx_gsi_tx_debug_stats) *
			(ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[i].num_tx_instances))
			+ (sizeof(struct ipa_lnx_gsi_rx_debug_stats) *
			(ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[
				i].num_rx_instances)));
		for (j = 0; j < instance_ptr->num_pipes; j++) {
			pipe_info_ptr_local = (struct ipa_lnx_pipe_info *)((
				uint64_t)pipe_info_ptr +
				(j * sizeof(struct ipa_lnx_pipe_info)));

			ep_idx = ipa_get_ep_mapping(
				ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[
				i].pipes_client_type[j]);
			if (ep_idx == -1) {
				kfree(wlan_stats);
				return -EFAULT;
			}
			pipe_info_ptr_local->pipe_num = ep_idx;
			ipa_get_gsi_pipe_info(
				pipe_info_ptr_local, &ipa3_ctx->ep[ep_idx]);
		}

		instance_ptr = (struct wlan_instance_info *)((uint64_t)pipe_info_ptr +
			(sizeof(struct ipa_lnx_pipe_info) * (instance_ptr->num_pipes)));
	}

success:
	if(copy_to_user((void __user *)arg,
		(u8 *)wlan_stats,
		alloc_size)) {
		IPA_STATS_ERR("copy to user failed");
		kfree(wlan_stats);
		return -EFAULT;
	}

	kfree(wlan_stats);
	return 0;
}

static int ipa_get_eth_inst_stats(unsigned long arg)
{
	struct ipa_lnx_eth_inst_stats *eth_stats;
	int i, j;
	int alloc_size;
	int ep_idx;
	int client_type;
	struct ipa_lnx_pipe_info *pipe_info_ptr = NULL;
	struct ipa_lnx_gsi_tx_debug_stats *tx_instance_ptr = NULL;
	struct ipa_lnx_gsi_rx_debug_stats *rx_instance_ptr = NULL;
	struct ipa_lnx_pipe_info *pipe_info_ptr_local = NULL;
	struct ipa_lnx_gsi_tx_debug_stats *tx_instance_ptr_local = NULL;
	struct ipa_lnx_gsi_rx_debug_stats *rx_instance_ptr_local = NULL;
	struct eth_instance_info *instance_ptr = NULL;
	struct ipa_uc_dbg_ring_stats stats;
	struct ipa_ntn3_client_stats ntn3_stats;

	if (!(ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_ETH_STATS)) {
		IPA_STATS_ERR("Log type ETH mask not set\n");
		return -EFAULT;
	}

	alloc_size = sizeof(struct ipa_lnx_eth_inst_stats) +
			(ipa_lnx_agent_ctx.alloc_info.num_eth_instances *
				sizeof(struct eth_instance_info));
	for (i = 0; i < ipa_lnx_agent_ctx.alloc_info.num_eth_instances; i++) {
		int num_tx = ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].num_tx_instances;
		int num_rx = ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].num_rx_instances;

		for (j = 0; j < num_tx; j++) {
			u32 mode = ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].tx_inst_client_type[j];

			alloc_size += (mode == IPA_ETH_CLIENT_IEMAC)
				? sizeof(struct ipa_lnx_gsi_tx_ntn_debug_stats)
				: sizeof(struct ipa_lnx_gsi_tx_debug_stats);
		}
		for (j = 0; j < num_rx; j++) {
			/* For eth, tx_inst_client_type[j] carries the mode for both TX and RX instance j */
			u32 mode = ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].tx_inst_client_type[
				j < num_tx ? j : 0];

			alloc_size += (mode == IPA_ETH_CLIENT_IEMAC)
				? sizeof(struct ipa_lnx_gsi_rx_ntn_debug_stats)
				: sizeof(struct ipa_lnx_gsi_rx_debug_stats);
		}
		alloc_size += ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].num_pipes
			* sizeof(struct ipa_lnx_pipe_info);
	}

	eth_stats = (struct ipa_lnx_eth_inst_stats *) memdup_user((
		const void __user *)arg, alloc_size);
	if (IS_ERR(eth_stats)) {
		IPA_STATS_ERR("copy from user failed");
		return -ENOMEM;
	}

	eth_stats->num_eth_instance =
		ipa_lnx_agent_ctx.alloc_info.num_eth_instances;
	if (!ipa_lnx_agent_ctx.alloc_info.num_eth_instances)
		goto success;

	instance_ptr = &eth_stats->instance_info[0];
	for (i = 0; i < eth_stats->num_eth_instance; i++) {
		size_t tx_sz, rx_sz;
		size_t total_tx_size = 0, total_rx_size = 0;
		uint64_t tx_offset = 0, rx_offset = 0;
		int num_tx_i = ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].num_tx_instances;
		int num_rx_i = ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].num_rx_instances;

		for (j = 0; j < num_tx_i; j++) {
			u32 mode = ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].tx_inst_client_type[j];

			total_tx_size += (mode == IPA_ETH_CLIENT_IEMAC)
				? sizeof(struct ipa_lnx_gsi_tx_ntn_debug_stats)
				: sizeof(struct ipa_lnx_gsi_tx_debug_stats);
		}
		for (j = 0; j < num_rx_i; j++) {
			u32 mode = ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].tx_inst_client_type[
				j < num_tx_i ? j : 0];

			total_rx_size += (mode == IPA_ETH_CLIENT_IEMAC)
				? sizeof(struct ipa_lnx_gsi_rx_ntn_debug_stats)
				: sizeof(struct ipa_lnx_gsi_rx_debug_stats);
		}

		instance_ptr->instance_id = i;
		instance_ptr->num_pipes =
			ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].num_pipes;
		instance_ptr->gsi_debug_stats.num_tx_instances =
			ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].num_tx_instances;
		instance_ptr->gsi_debug_stats.num_rx_instances =
			ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].num_rx_instances;

		tx_instance_ptr = (struct ipa_lnx_gsi_tx_debug_stats *)((
			uint64_t)instance_ptr + sizeof(struct eth_instance_info));
		for (j = 0; j < ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
			i].num_tx_instances; j++) {
			u32 eth_mode_j = ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].tx_inst_client_type[j];

			tx_sz = (eth_mode_j == IPA_ETH_CLIENT_IEMAC)
				? sizeof(struct ipa_lnx_gsi_tx_ntn_debug_stats)
				: sizeof(struct ipa_lnx_gsi_tx_debug_stats);
			tx_instance_ptr_local = (struct ipa_lnx_gsi_tx_debug_stats *)((
				uint64_t)tx_instance_ptr + tx_offset);

			/* Eth mode is sent in the tx_inst_client_type variable only */
			instance_ptr->eth_mode =
				ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
					i].tx_inst_client_type[j];
			if (instance_ptr->eth_mode == IPA_ETH_CLIENT_AQC107 ||
				instance_ptr->eth_mode == IPA_ETH_CLIENT_AQC113 ||
				instance_ptr->eth_mode == IPA_ETH_CLIENT_NTN ||
#if IPA_ETH_API_VER >= 2
				instance_ptr->eth_mode == IPA_ETH_CLIENT_NTN3 ||
#endif
				instance_ptr->eth_mode == IPA_ETH_CLIENT_EMAC) {

				if(instance_ptr->eth_mode == IPA_ETH_CLIENT_NTN
#if IPA_ETH_API_VER >= 2
					|| instance_ptr->eth_mode == IPA_ETH_CLIENT_NTN3
#endif
					) {
					if(ipa3_get_ntn_gsi_stats(&stats)) {
						instance_ptr = (struct eth_instance_info *)((
							uint64_t)instance_ptr +
							sizeof(struct eth_instance_info) +
							total_tx_size +
							total_rx_size +
							(instance_ptr->num_pipes *
							sizeof(struct ipa_lnx_pipe_info)));
						continue;
					}
				} else {
					if(ipa3_get_aqc_gsi_stats(&stats)) {
						instance_ptr = (struct eth_instance_info *)((
							uint64_t)instance_ptr +
							sizeof(struct eth_instance_info) +
							total_tx_size +
							total_rx_size +
							(instance_ptr->num_pipes *
							sizeof(struct ipa_lnx_pipe_info)));
						continue;
					}
				}

				if (instance_ptr->eth_mode == IPA_ETH_CLIENT_NTN ||
					instance_ptr->eth_mode == IPA_ETH_CLIENT_EMAC)
					tx_instance_ptr_local->tx_client =
						IPA_CLIENT_ETHERNET_CONS;
				else
					tx_instance_ptr_local->tx_client =
						IPA_CLIENT_AQC_ETHERNET_CONS;
#if IPA_ETH_API_VER >= 2
				/* Get the client pipe info[0] from the allocation info context only if it is NTN3.
				   Each Ethernet instance j has two pipes (one TX, one RX) stored contiguously
				   in the pipes_client_type flat array.The * 2 is the stride to reach the TX entry for instance j,
				   and + 1 elsewhere gets the RX entry for the same instance.*/
				if (instance_ptr->eth_mode == IPA_ETH_CLIENT_NTN3) {
						tx_instance_ptr_local->tx_client =
							ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
							i].pipes_client_type[j * 2];
				}
#endif
				client_type = tx_instance_ptr_local->tx_client;
				instance_ptr->pm_bandwidth =
					ipa_pm_get_pm_clnt_throughput(client_type);
				tx_instance_ptr_local->num_tx_ring_100_perc_with_cred =
					stats.u.ring[1].ringFull;
				tx_instance_ptr_local->num_tx_ring_0_perc_with_cred =
					stats.u.ring[1].ringEmpty;
				tx_instance_ptr_local->num_tx_ring_above_75_perc_cred =
					stats.u.ring[1].ringUsageHigh;
				tx_instance_ptr_local->num_tx_ring_above_25_perc_cred =
					stats.u.ring[1].ringUsageLow;
				tx_instance_ptr_local->num_tx_ring_stats_polled =
					stats.u.ring[1].RingUtilCount;
				ipa_lnx_calculate_gsi_ring_summay(
					tx_instance_ptr_local, NULL, client_type);

			} else if (instance_ptr->eth_mode == IPA_ETH_CLIENT_RTK8111K ||
				instance_ptr->eth_mode == IPA_ETH_CLIENT_RTK8125B) {

				if(ipa3_get_rtk_gsi_stats(&stats)) {
					instance_ptr = (struct eth_instance_info *)((
						uint64_t)instance_ptr +
						sizeof(struct eth_instance_info) +
						total_tx_size +
						total_rx_size +
						(instance_ptr->num_pipes *
						sizeof(struct ipa_lnx_pipe_info)));
					continue;
				}
				client_type = IPA_CLIENT_RTK_ETHERNET_CONS;
				instance_ptr->pm_bandwidth =
					ipa_pm_get_pm_clnt_throughput(client_type);
				tx_instance_ptr_local->tx_client = client_type;
				tx_instance_ptr_local->num_tx_ring_100_perc_with_cred =
					stats.u.rtk[1].commStats.ringFull;
				tx_instance_ptr_local->num_tx_ring_0_perc_with_cred =
					stats.u.rtk[1].commStats.ringEmpty;
				tx_instance_ptr_local->num_tx_ring_above_75_perc_cred =
					stats.u.rtk[1].commStats.ringUsageHigh;
				tx_instance_ptr_local->num_tx_ring_above_25_perc_cred =
					stats.u.rtk[1].commStats.ringUsageLow;
				tx_instance_ptr_local->num_tx_ring_stats_polled =
					stats.u.rtk[1].commStats.RingUtilCount;
				ipa_lnx_calculate_gsi_ring_summay(
					tx_instance_ptr_local, NULL, client_type);

			} else if(instance_ptr->eth_mode == IPA_ETH_CLIENT_IEMAC){
				struct ipa_lnx_gsi_tx_ntn_debug_stats *ntn_tx =
					(struct ipa_lnx_gsi_tx_ntn_debug_stats *)tx_instance_ptr_local;
#if IPA_ETH_API_VER >= 2
				/* Get the client pipe info[0] from the allocation info context only if it is IEMAC */
						ntn_tx->base.tx_client =
							ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
							i].pipes_client_type[j * 2];
#endif
				client_type = ntn_tx->base.tx_client;
				instance_ptr->pm_bandwidth =
					ipa_pm_get_pm_clnt_throughput(client_type);
				ntn_tx->base.num_tx_ring_100_perc_with_cred =
					stats.u.ring[1].ringFull;
				ntn_tx->base.num_tx_ring_0_perc_with_cred =
					stats.u.ring[1].ringEmpty;
				ntn_tx->base.num_tx_ring_above_75_perc_cred =
					stats.u.ring[1].ringUsageHigh;
				ntn_tx->base.num_tx_ring_above_25_perc_cred =
					stats.u.ring[1].ringUsageLow;
				ntn_tx->base.num_tx_ring_stats_polled =
					stats.u.ring[1].RingUtilCount;
				ipa_lnx_calculate_gsi_ring_summay(
					&ntn_tx->base, NULL, client_type);
				memset(&(ntn3_stats.tx_stats), 0, sizeof(ntn3_stats.tx_stats));
				// ipa_eth_ntn3_get_status(&ntn3_stats, i);
				__ipa_ntn3_cons_stats_get(&(ntn3_stats.tx_stats), ntn_tx->base.tx_client);
				ntn_tx->ntn_stats.last_db_value = ntn3_stats.tx_stats.ntn_stats.last_db_value;
				ntn_tx->ntn_stats.next_re = ntn3_stats.tx_stats.ntn_stats.next_re;
				ntn_tx->ntn_stats.malformed_tre = ntn3_stats.tx_stats.ntn_stats.malformed_tre;
				ntn_tx->ntn_stats.invalid_tre_cnt = ntn3_stats.tx_stats.ntn_stats.invalid_tre_cnt;
				ntn_tx->ntn_stats.rollbacks_cnt = ntn3_stats.tx_stats.ntn_stats.rollbacks_cnt;
				ntn_tx->ntn_stats.outstanding_tlvs_cnt = ntn3_stats.tx_stats.ntn_stats.outstanding_tlvs_cnt;
			}
			else IPA_STATS_ERR("Eth tx client type not found. ETH client: %d",instance_ptr->eth_mode);
			tx_offset += tx_sz;
		}

		rx_instance_ptr = (struct ipa_lnx_gsi_rx_debug_stats *)((
			uint64_t)instance_ptr + sizeof(struct eth_instance_info)
			+ total_tx_size);
		for (j = 0; j < ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
			i].num_rx_instances; j++) {
			/* For eth, tx_inst_client_type[j] carries the mode for both TX and RX instance j */
			u32 eth_mode_j = ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].tx_inst_client_type[
				j < num_tx_i ? j : 0];

			rx_sz = (eth_mode_j == IPA_ETH_CLIENT_IEMAC)
				? sizeof(struct ipa_lnx_gsi_rx_ntn_debug_stats)
				: sizeof(struct ipa_lnx_gsi_rx_debug_stats);
			rx_instance_ptr_local = (struct ipa_lnx_gsi_rx_debug_stats *)((
				uint64_t)rx_instance_ptr + rx_offset);

			if ((eth_mode_j == IPA_ETH_CLIENT_AQC107 ||
				eth_mode_j == IPA_ETH_CLIENT_AQC113 ||
				eth_mode_j == IPA_ETH_CLIENT_NTN ||
#if IPA_ETH_API_VER >= 2
				eth_mode_j == IPA_ETH_CLIENT_NTN3 ||
#endif
				eth_mode_j == IPA_ETH_CLIENT_EMAC)) {

				if (eth_mode_j == IPA_ETH_CLIENT_NTN
#if IPA_ETH_API_VER >= 2
					|| eth_mode_j == IPA_ETH_CLIENT_NTN3
#endif
					) {
					if (ipa3_get_ntn_gsi_stats(&stats)) {
						rx_offset += rx_sz;
						continue;
					}
				} else if (eth_mode_j != IPA_ETH_CLIENT_IEMAC) {
					if (ipa3_get_aqc_gsi_stats(&stats)) {
						rx_offset += rx_sz;
						continue;
					}
				}

				if (eth_mode_j == IPA_ETH_CLIENT_NTN ||
					eth_mode_j == IPA_ETH_CLIENT_EMAC)
					rx_instance_ptr_local->rx_client =
					IPA_CLIENT_ETHERNET_PROD;
				else
					rx_instance_ptr_local->rx_client =
						IPA_CLIENT_AQC_ETHERNET_PROD;
#if IPA_ETH_API_VER >= 2
				/* Get the client pipe info[1] from the allocation info context only if it is NTN3 */
				if (eth_mode_j == IPA_ETH_CLIENT_NTN3) {
						rx_instance_ptr_local->rx_client =
							ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
							i].pipes_client_type[(j * 2) + 1];
				}
#endif
				client_type = rx_instance_ptr_local->rx_client;
				rx_instance_ptr_local->num_rx_ring_100_perc_with_pack =
					stats.u.ring[0].ringFull;
				rx_instance_ptr_local->num_rx_ring_0_perc_with_pack =
					stats.u.ring[0].ringEmpty;
				rx_instance_ptr_local->num_rx_ring_above_75_perc_pack =
					stats.u.ring[0].ringUsageHigh;
				rx_instance_ptr_local->num_rx_ring_above_25_perc_pack =
					stats.u.ring[0].ringUsageLow;
				rx_instance_ptr_local->num_rx_ring_stats_polled =
					stats.u.ring[0].RingUtilCount;
				rx_instance_ptr_local->num_rx_drop_stats = 0;
				ipa_lnx_calculate_gsi_ring_summay(
					NULL, rx_instance_ptr_local, client_type);

			} else if (eth_mode_j == IPA_ETH_CLIENT_RTK8111K ||
				eth_mode_j == IPA_ETH_CLIENT_RTK8125B) {

				if (ipa3_get_rtk_gsi_stats(&stats)) {
					rx_offset += rx_sz;
					continue;
				}
				client_type = IPA_CLIENT_RTK_ETHERNET_PROD;
				rx_instance_ptr_local->rx_client = client_type;
				rx_instance_ptr_local->num_rx_ring_100_perc_with_pack =
					stats.u.rtk[0].commStats.ringFull;
				rx_instance_ptr_local->num_rx_ring_0_perc_with_pack =
					stats.u.rtk[0].commStats.ringEmpty;
				rx_instance_ptr_local->num_rx_ring_above_75_perc_pack =
					stats.u.rtk[0].commStats.ringUsageHigh;
				rx_instance_ptr_local->num_rx_ring_above_25_perc_pack =
					stats.u.rtk[0].commStats.ringUsageLow;
				rx_instance_ptr_local->num_rx_ring_stats_polled =
					stats.u.rtk[0].commStats.RingUtilCount;
				rx_instance_ptr_local->num_rx_drop_stats = 0;
				ipa_lnx_calculate_gsi_ring_summay(
					NULL, rx_instance_ptr_local, client_type);

			} else if(eth_mode_j == IPA_ETH_CLIENT_IEMAC){
				struct ipa_lnx_gsi_rx_ntn_debug_stats *ntn_rx =
					(struct ipa_lnx_gsi_rx_ntn_debug_stats *)rx_instance_ptr_local;

				if (ipa3_get_ntn_gsi_stats(&stats)) {
					rx_offset += rx_sz;
					continue;
				}
#if IPA_ETH_API_VER >= 2
				/* Get the client pipe info[0] from the allocation info context only if it is IEMAC */
						ntn_rx->base.rx_client =
							ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
							i].pipes_client_type[(j * 2) + 1];
#endif
				client_type = ntn_rx->base.rx_client;
				ntn_rx->base.num_rx_ring_100_perc_with_pack =
					stats.u.ring[0].ringFull;
				ntn_rx->base.num_rx_ring_0_perc_with_pack =
					stats.u.ring[0].ringEmpty;
				ntn_rx->base.num_rx_ring_above_75_perc_pack =
					stats.u.ring[0].ringUsageHigh;
				ntn_rx->base.num_rx_ring_above_25_perc_pack =
					stats.u.ring[0].ringUsageLow;
				ntn_rx->base.num_rx_ring_stats_polled =
					stats.u.ring[0].RingUtilCount;
				ipa_lnx_calculate_gsi_ring_summay(
					NULL, &ntn_rx->base, client_type);

				memset(&(ntn3_stats.rx_stats), 0, sizeof(ntn3_stats.rx_stats));
				__ipa_ntn3_prod_stats_get(&(ntn3_stats.rx_stats), ntn_rx->base.rx_client);
				ntn_rx->base.num_rx_drop_stats = ntn3_stats.rx_stats.err_cnt;
				ntn_rx->ntn_stats.last_db_value = ntn3_stats.rx_stats.ntn_stats.last_db_value;
				ntn_rx->ntn_stats.next_re = ntn3_stats.rx_stats.ntn_stats.next_re;
				ntn_rx->ntn_stats.malformed_tre = ntn3_stats.rx_stats.ntn_stats.malformed_tre;
				ntn_rx->ntn_stats.zero_len_pkt_cnt = ntn3_stats.rx_stats.ntn_stats.zero_len_pkt_cnt;
				ntn_rx->ntn_stats.invalid_tre_cnt = ntn3_stats.rx_stats.ntn_stats.invalid_tre_cnt;
				ntn_rx->ntn_stats.rollbacks_cnt = ntn3_stats.rx_stats.ntn_stats.rollbacks_cnt;
				ntn_rx->ntn_stats.outstanding_tlvs_cnt = ntn3_stats.rx_stats.ntn_stats.outstanding_tlvs_cnt;
			}  else IPA_STATS_ERR("Eth rx client type not found. ETH client: %d", eth_mode_j);
			rx_offset += rx_sz;
		}

		pipe_info_ptr = (struct ipa_lnx_pipe_info *)((uint64_t)instance_ptr +
			sizeof(struct eth_instance_info)
			+ total_tx_size
			+ total_rx_size);
		for (j = 0; j < instance_ptr->num_pipes; j++) {
			pipe_info_ptr_local = (struct ipa_lnx_pipe_info *)((
				uint64_t)pipe_info_ptr + (j *
				sizeof(struct ipa_lnx_pipe_info)));

			ep_idx = ipa_get_ep_mapping(
				ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
					i].pipes_client_type[j]);
			if (ep_idx == -1) {
				kfree(eth_stats);
				return -EFAULT;
			}

			pipe_info_ptr_local->pipe_num = ep_idx;
			ipa_get_gsi_pipe_info(
				pipe_info_ptr_local, &ipa3_ctx->ep[ep_idx]);
		}

		instance_ptr = (struct eth_instance_info *)((
			uint64_t)pipe_info_ptr + (sizeof(struct ipa_lnx_pipe_info)
			* (instance_ptr->num_pipes)));
	}

success:
	if(copy_to_user((void __user *)arg,
		(u8 *)eth_stats,
		alloc_size)) {
		IPA_STATS_ERR("copy to user failed");
		kfree(eth_stats);
		return -EFAULT;
	}

	kfree(eth_stats);
	return 0;
}

static int ipa_get_usb_inst_stats(unsigned long arg)
{
	struct ipa_lnx_usb_inst_stats *usb_stats;
	int i, j;
	int alloc_size;
	int ep_idx;
	int client_type;
	struct ipa_lnx_pipe_info *pipe_info_ptr = NULL;
	struct ipa_lnx_gsi_tx_debug_stats *tx_instance_ptr = NULL;
	struct ipa_lnx_gsi_rx_debug_stats *rx_instance_ptr = NULL;
	struct ipa_lnx_pipe_info *pipe_info_ptr_local = NULL;
	struct ipa_lnx_gsi_tx_debug_stats *tx_instance_ptr_local = NULL;
	struct ipa_lnx_gsi_rx_debug_stats *rx_instance_ptr_local = NULL;
	struct usb_instance_info *instance_ptr = NULL;
	struct ipa_uc_dbg_ring_stats stats;

	if (!(ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_USB_STATS)) {
		IPA_STATS_ERR("Log type USB mask not set\n");
		return -EFAULT;
	}

	alloc_size = sizeof(struct ipa_lnx_usb_inst_stats) +
			(ipa_lnx_agent_ctx.alloc_info.num_usb_instances *
				sizeof(struct usb_instance_info));
	for (i = 0; i < ipa_lnx_agent_ctx.alloc_info.num_usb_instances; i++) {
		alloc_size = alloc_size +
			(ipa_lnx_agent_ctx.alloc_info.usb_inst_info[i].num_tx_instances *
				sizeof(struct ipa_lnx_gsi_tx_debug_stats)) +
			(ipa_lnx_agent_ctx.alloc_info.usb_inst_info[i].num_rx_instances *
				sizeof(struct ipa_lnx_gsi_rx_debug_stats)) +
			(ipa_lnx_agent_ctx.alloc_info.usb_inst_info[i].num_pipes *
				sizeof(struct ipa_lnx_pipe_info));
	}

	usb_stats = (struct ipa_lnx_usb_inst_stats *) memdup_user((
		const void __user *)arg, alloc_size);
	if (IS_ERR(usb_stats)) {
		IPA_STATS_ERR("copy from user failed");
		return -ENOMEM;
	}

	usb_stats->num_usb_instance =
		ipa_lnx_agent_ctx.alloc_info.num_usb_instances;
	if (!ipa_lnx_agent_ctx.alloc_info.num_usb_instances)
		goto success;

	instance_ptr = &usb_stats->instance_info[0];
	for (i = 0; i < usb_stats->num_usb_instance; i++) {
		instance_ptr->instance_id = i;
		instance_ptr->usb_mode = ipa_lnx_agent_ctx.usb_teth_prot[i];
		instance_ptr->pm_bandwidth =
			ipa_pm_get_pm_clnt_throughput(ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
				i].tx_inst_client_type[0]);
		instance_ptr->num_pipes =
			ipa_lnx_agent_ctx.alloc_info.usb_inst_info[i].num_pipes;
		instance_ptr->gsi_debug_stats.num_tx_instances =
			ipa_lnx_agent_ctx.alloc_info.usb_inst_info[i].num_tx_instances;
		instance_ptr->gsi_debug_stats.num_rx_instances =
			ipa_lnx_agent_ctx.alloc_info.usb_inst_info[i].num_rx_instances;
		if(ipa3_get_usb_gsi_stats(&stats)) {
			instance_ptr = (struct usb_instance_info *)((uint64_t)instance_ptr
				+ sizeof(struct usb_instance_info) +
				(instance_ptr->gsi_debug_stats.num_tx_instances *
				sizeof(struct ipa_lnx_gsi_tx_debug_stats)) +
				(instance_ptr->gsi_debug_stats.num_rx_instances *
				sizeof(struct ipa_lnx_gsi_rx_debug_stats)) +
				(instance_ptr->num_pipes * sizeof(struct ipa_lnx_pipe_info)));
			continue;
		}

		tx_instance_ptr = (struct ipa_lnx_gsi_tx_debug_stats *)((
			uint64_t)instance_ptr + sizeof(struct usb_instance_info));
		for (j = 0; j < ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
			i].num_tx_instances; j++) {
			tx_instance_ptr_local = (struct ipa_lnx_gsi_tx_debug_stats *)((
				uint64_t)tx_instance_ptr + (j *
				sizeof(struct ipa_lnx_gsi_tx_debug_stats)));

			client_type = ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
				i].tx_inst_client_type[j];
			tx_instance_ptr_local->tx_client = client_type;
			tx_instance_ptr_local->num_tx_ring_100_perc_with_cred =
				stats.u.ring[1 + j].ringFull;
			tx_instance_ptr_local->num_tx_ring_0_perc_with_cred =
				stats.u.ring[1 + j].ringEmpty;
			tx_instance_ptr_local->num_tx_ring_above_75_perc_cred =
				stats.u.ring[1 + j].ringUsageHigh;
			tx_instance_ptr_local->num_tx_ring_above_25_perc_cred =
				stats.u.ring[1 + j].ringUsageLow;
			tx_instance_ptr_local->num_tx_ring_stats_polled =
				stats.u.ring[1 + j].RingUtilCount;
			ipa_lnx_calculate_gsi_ring_summay(
				tx_instance_ptr_local, NULL, client_type);

		}

		rx_instance_ptr = (struct ipa_lnx_gsi_rx_debug_stats *) ((
			uint64_t)instance_ptr + sizeof(struct usb_instance_info)
			+ (sizeof(struct ipa_lnx_gsi_tx_debug_stats) *
			(ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
			i].num_tx_instances)));
		for (j = 0; j < ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
			i].num_rx_instances; j++) {
			rx_instance_ptr_local = (struct ipa_lnx_gsi_rx_debug_stats *)((
				uint64_t)rx_instance_ptr +
				(j * sizeof(struct ipa_lnx_gsi_rx_debug_stats)));

			client_type = ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
				i].rx_inst_client_type[j];
			rx_instance_ptr_local->rx_client = client_type;
			rx_instance_ptr_local->num_rx_ring_100_perc_with_pack =
				stats.u.ring[j].ringFull;
			rx_instance_ptr_local->num_rx_ring_0_perc_with_pack =
				stats.u.ring[j].ringEmpty;
			rx_instance_ptr_local->num_rx_ring_above_75_perc_pack =
				stats.u.ring[j].ringUsageHigh;
			rx_instance_ptr_local->num_rx_ring_above_25_perc_pack =
				stats.u.ring[j].ringUsageLow;
			rx_instance_ptr_local->num_rx_ring_stats_polled =
				stats.u.ring[j].RingUtilCount;
			rx_instance_ptr_local->num_rx_drop_stats = 0;
			ipa_lnx_calculate_gsi_ring_summay(
				NULL, rx_instance_ptr_local, client_type);

		}

		pipe_info_ptr = (struct ipa_lnx_pipe_info *)((uint64_t)instance_ptr +
			sizeof(struct usb_instance_info)
			+ (sizeof(struct ipa_lnx_gsi_tx_debug_stats) *
			(ipa_lnx_agent_ctx.alloc_info.usb_inst_info[i].num_tx_instances))
			+ (sizeof(struct ipa_lnx_gsi_rx_debug_stats) *
			(ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
				i].num_rx_instances)));
		for (j = 0; j < instance_ptr->num_pipes; j++) {
			pipe_info_ptr_local = (struct ipa_lnx_pipe_info *)((
				uint64_t)pipe_info_ptr + (j *
				sizeof(struct ipa_lnx_pipe_info)));

			ep_idx = ipa_get_ep_mapping(
				ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
					i].pipes_client_type[j]);
			if (ep_idx == -1) {
				kfree(usb_stats);
				return -EFAULT;
			}
			pipe_info_ptr_local->pipe_num = ep_idx;
			ipa_get_gsi_pipe_info(
				pipe_info_ptr_local, &ipa3_ctx->ep[ep_idx]);
		}

		instance_ptr = (struct usb_instance_info *) ((uint64_t)pipe_info_ptr +
			(sizeof(struct ipa_lnx_pipe_info) * (instance_ptr->num_pipes)));
	}

success:
	if(copy_to_user((void __user *)arg,
		(u8 *)usb_stats,
		alloc_size)) {
		IPA_STATS_ERR("copy to user failed");
		kfree(usb_stats);
		return -EFAULT;
	}

	kfree(usb_stats);
	return 0;
}

#if IS_ENABLED(CONFIG_IPA3_MHI_PRIME_MANAGER)
static int ipa_get_mhip_inst_stats(unsigned long arg)
{
	struct ipa_lnx_mhip_inst_stats *mhip_stats;
	int i, j;
	int alloc_size;
	int ep_idx;
	int client_type;
	struct ipa_lnx_pipe_info *pipe_info_ptr = NULL;
	struct ipa_lnx_gsi_tx_debug_stats *tx_instance_ptr = NULL;
	struct ipa_lnx_gsi_rx_debug_stats *rx_instance_ptr = NULL;
	struct ipa_lnx_pipe_info *pipe_info_ptr_local = NULL;
	struct ipa_lnx_gsi_tx_debug_stats *tx_instance_ptr_local = NULL;
	struct ipa_lnx_gsi_rx_debug_stats *rx_instance_ptr_local = NULL;
	struct mhip_instance_info *instance_ptr = NULL;
	struct ipa_uc_dbg_ring_stats stats;

	if (!(ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_MHIP_STATS)) {
		IPA_STATS_ERR("Log type MHIP mask not set\n");
		return -EFAULT;
	}

	alloc_size = sizeof(struct ipa_lnx_mhip_inst_stats) +
			(ipa_lnx_agent_ctx.alloc_info.num_mhip_instances *
				sizeof(struct mhip_instance_info));
	for (i = 0; i < ipa_lnx_agent_ctx.alloc_info.num_mhip_instances; i++) {
		alloc_size = alloc_size +
			(ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[i].num_tx_instances *
				sizeof(struct ipa_lnx_gsi_tx_debug_stats)) +
			(ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[i].num_rx_instances *
				sizeof(struct ipa_lnx_gsi_rx_debug_stats)) +
			(ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[i].num_pipes *
				sizeof(struct ipa_lnx_pipe_info));
	}

	mhip_stats = (struct ipa_lnx_mhip_inst_stats *) memdup_user((
		const void __user *)arg, alloc_size);
	if (IS_ERR(mhip_stats)) {
		IPA_STATS_ERR("copy from user failed");
		return -ENOMEM;
	}

	if (!ipa_lnx_agent_ctx.alloc_info.num_mhip_instances)
		goto success;
	mhip_stats->num_mhip_instance =
		ipa_lnx_agent_ctx.alloc_info.num_mhip_instances;

	instance_ptr = &mhip_stats->instance_info[0];
	for (i = 0; i < mhip_stats->num_mhip_instance; i++) {
		instance_ptr->instance_id = i;
		instance_ptr->mhip_mode =
			ipa_lnx_agent_ctx.usb_teth_prot[i];
		instance_ptr->pm_bandwidth =
			ipa_pm_get_pm_clnt_throughput(
				ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[
				i].tx_inst_client_type[0]);
		instance_ptr->num_pipes =
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[i].num_pipes;
		instance_ptr->gsi_debug_stats.num_tx_instances =
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[i].num_tx_instances;
		instance_ptr->gsi_debug_stats.num_rx_instances =
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[i].num_rx_instances;
		if(ipa3_get_mhip_gsi_stats(&stats)) {
			instance_ptr = (struct mhip_instance_info *)((
				uint64_t)instance_ptr + sizeof(struct mhip_instance_info) +
				(instance_ptr->gsi_debug_stats.num_tx_instances *
					sizeof(struct ipa_lnx_gsi_tx_debug_stats)) +
				(instance_ptr->gsi_debug_stats.num_rx_instances *
					sizeof(struct ipa_lnx_gsi_rx_debug_stats)) +
				(instance_ptr->num_pipes * sizeof(struct ipa_lnx_pipe_info)));
			continue;
		}

		tx_instance_ptr = (struct ipa_lnx_gsi_tx_debug_stats *)((
			uint64_t)instance_ptr + sizeof(struct mhip_instance_info));
		for (j = 0; j < ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[
			i].num_tx_instances; j++) {
			tx_instance_ptr_local = (struct ipa_lnx_gsi_tx_debug_stats *)((
				uint64_t)tx_instance_ptr + (j *
				sizeof(struct ipa_lnx_gsi_tx_debug_stats)));

			client_type = ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[
				i].tx_inst_client_type[j];
			tx_instance_ptr_local->tx_client = client_type;
			tx_instance_ptr_local->num_tx_ring_100_perc_with_cred =
				stats.u.ring[1 + (j*2)].ringFull;
			tx_instance_ptr_local->num_tx_ring_0_perc_with_cred =
				stats.u.ring[1 + (j*2)].ringEmpty;
			tx_instance_ptr_local->num_tx_ring_above_75_perc_cred =
				stats.u.ring[1 + (j*2)].ringUsageHigh;
			tx_instance_ptr_local->num_tx_ring_above_25_perc_cred =
				stats.u.ring[1 + (j*2)].ringUsageLow;
			tx_instance_ptr_local->num_tx_ring_stats_polled =
				stats.u.ring[1 + (j*2)].RingUtilCount;
			ipa_lnx_calculate_gsi_ring_summay(
				tx_instance_ptr_local, NULL, client_type);
		}

		rx_instance_ptr = (struct ipa_lnx_gsi_rx_debug_stats *)((
			uint64_t)instance_ptr + sizeof(struct mhip_instance_info)
			+ (sizeof(struct ipa_lnx_gsi_tx_debug_stats) * (
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[
			i].num_tx_instances)));
		for (j = 0; j < ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[
			i].num_rx_instances; j++) {
			rx_instance_ptr_local = (struct ipa_lnx_gsi_rx_debug_stats *)((
				uint64_t)rx_instance_ptr +
				(j * sizeof(struct ipa_lnx_gsi_rx_debug_stats)));

			client_type = ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[
				i].rx_inst_client_type[j];
			rx_instance_ptr_local->rx_client = client_type;
			rx_instance_ptr_local->num_rx_ring_100_perc_with_pack =
				stats.u.ring[2*j].ringFull;
			rx_instance_ptr_local->num_rx_ring_0_perc_with_pack =
				stats.u.ring[2*j].ringEmpty;
			rx_instance_ptr_local->num_rx_ring_above_75_perc_pack =
				stats.u.ring[2*j].ringUsageHigh;
			rx_instance_ptr_local->num_rx_ring_above_25_perc_pack =
				stats.u.ring[2*j].ringUsageLow;
			rx_instance_ptr_local->num_rx_ring_stats_polled =
				stats.u.ring[2*j].RingUtilCount;
			rx_instance_ptr_local->num_rx_drop_stats = 0;
			ipa_lnx_calculate_gsi_ring_summay(NULL,
				rx_instance_ptr_local, client_type);
		}

		pipe_info_ptr = (struct ipa_lnx_pipe_info *)((uint64_t)instance_ptr +
			sizeof(struct mhip_instance_info)
			+ (sizeof(struct ipa_lnx_gsi_tx_debug_stats) * (
				ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[
					i].num_tx_instances))
			+ (sizeof(struct ipa_lnx_gsi_rx_debug_stats) * (
				ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[
					i].num_rx_instances)));
		for (j = 0; j < instance_ptr->num_pipes; j++) {
			pipe_info_ptr_local = (struct ipa_lnx_pipe_info *)((uint64_t)
				pipe_info_ptr + (j * sizeof(struct ipa_lnx_pipe_info)));

			ep_idx = ipa_get_ep_mapping(
				ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[
					i].pipes_client_type[j]);
			if (ep_idx == -1) {
				kfree(mhip_stats);
				return -EFAULT;
			}
			pipe_info_ptr_local->pipe_num = ep_idx;
			ipa_get_gsi_pipe_info(
				pipe_info_ptr_local, &ipa3_ctx->ep[ep_idx]);
		}
		instance_ptr = (struct mhip_instance_info *)((uint64_t)pipe_info_ptr +
			(sizeof(struct ipa_lnx_pipe_info) * (instance_ptr->num_pipes)));
	}

success:
	if(copy_to_user((void __user *)arg,
		(u8 *)mhip_stats,
		alloc_size)) {
		IPA_STATS_ERR("copy to user failed");
		kfree(mhip_stats);
		return -EFAULT;
	}

	kfree(mhip_stats);
	return 0;
}
#endif

static int ipa_get_page_recycle_stats(unsigned long arg)
{
	struct ipa_lnx_pipe_page_recycling_stats *page_recycle_stats;
	int alloc_size;

	alloc_size = sizeof(struct ipa_lnx_pipe_page_recycling_stats);

	page_recycle_stats = (struct ipa_lnx_pipe_page_recycling_stats *) memdup_user((
		const void __user *)arg, alloc_size);
	if (IS_ERR(page_recycle_stats)) {
		IPA_STATS_ERR("copy from user failed");
		return -ENOMEM;
	}

	mutex_lock(&ipa3_ctx->recycle_stats_collection_lock);
	memcpy(page_recycle_stats, &ipa3_ctx->recycle_stats,
		sizeof(struct ipa_lnx_pipe_page_recycling_stats));

	/* Clear all the data and valid bits */
	memset(&ipa3_ctx->recycle_stats, 0,
		sizeof(struct ipa_lnx_pipe_page_recycling_stats));

	mutex_unlock(&ipa3_ctx->recycle_stats_collection_lock);

	if(copy_to_user((void __user *)arg,
		(u8 *)page_recycle_stats,
		alloc_size)) {
		IPA_STATS_ERR("copy to user failed");
		kfree(page_recycle_stats);
		return -EFAULT;
	}

	kfree(page_recycle_stats);
	return 0;
}

static int ipa_get_rt_tbl_index_from_name(char *rt_tbl_name)
{
	int i;
	struct rt_table_name_lookup *lookup;

	for (i = 0; i < RT_TABLE_NAME_MAX; i++) {
		lookup = &rt_table_lookup_table[i];
		if (!strcmp(lookup->name, rt_tbl_name))
			return lookup->index;
	}
	return RT_TABLE_NAME_MAX;
}

static int ipa_get_v4_rt_rule_stats(unsigned long arg)
{
	struct ipa_lnx_v4_rt_rule_stats *rt_rule_stats = NULL;
	int alloc_size;
	int rule_idx, tbl_idx;
	struct ipa3_rt_tbl *rt_tbl;
	struct ipa3_rt_entry *rt_entry;
	struct ipa3_rt_tbl_set *rt_set;
	u32 ofst;
	u32 ofst_words;
	int ret = 0, i;

	if (ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v4_tables >=
		TLPD_NUM_MAX_RT_FLT_TBL) {
		IPA_STATS_ERR("Rejecting rt rule log packet collection\n");
		return -EFAULT;
	}

	alloc_size = sizeof(struct ipa_lnx_v4_rt_rule_stats);

	rt_rule_stats = (struct ipa_lnx_v4_rt_rule_stats *) memdup_user((
		const void __user *)arg, alloc_size);
	if (IS_ERR(rt_rule_stats)) {
		IPA_STATS_ERR("copy from user failed");
		return -ENOMEM;
	}

	/* IPv4 RT structures */
	mutex_lock(&ipa3_ctx->lock);
	rt_set = &ipa3_ctx->rt_tbl_set[IPA_IP_v4];

	rt_rule_stats->ip4_rt_tbl_hash_local =
		ipa3_ctx->rt_tbl_hash_lcl[IPA_IP_v4];
	rt_rule_stats->ip4_rt_tbl_nhash_local =
		ipa3_ctx->rt_tbl_nhash_lcl[IPA_IP_v4];
	rt_rule_stats->num_v4_rt_table =
		ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v4_tables;
	IPA_STATS_ERR("consolidated num_v4_tables %d num_v6_tables %d\n",
		rt_rule_stats->num_v4_rt_table,
		ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_tables);

	tbl_idx = 0;

	list_for_each_entry(rt_tbl, &rt_set->head_rt_tbl_list, link) {
		if (tbl_idx == TLPD_NUM_MAX_RT_FLT_TBL && tbl_idx >=
			ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v4_tables) {
			IPA_STATS_ERR("RT4 table limit reached\n");
			goto stats_fail;
		}

		rt_rule_stats->v4_rt_tbl[tbl_idx].tbl_index = rt_tbl->idx;
		rt_rule_stats->v4_rt_tbl[tbl_idx].tbl_ref_count = rt_tbl->ref_cnt;
		rt_rule_stats->v4_rt_tbl[tbl_idx].tbl_name =
			(uint8_t) ipa_get_rt_tbl_index_from_name(rt_tbl->name);
		memcpy(rt_rule_stats->v4_rt_tbl[tbl_idx].rt_tbl_name, rt_tbl->name, IPA_RESOURCE_NAME_MAX);
		rt_rule_stats->v4_rt_tbl[tbl_idx].num_rt_rule =
			ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v4_rules[tbl_idx];

		rule_idx = 0;
		list_for_each_entry(rt_entry, &rt_tbl->head_rt_rule_list, link) {
			if (rule_idx == TLPD_NUM_MAX_RT_FLT_RULE && rule_idx >=
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v4_rules[tbl_idx]) {
				IPA_STATS_ERR("RT4 Rule limit reached\n");
				goto stats_fail;
			}


			if (rt_entry->proc_ctx &&
				(!ipa3_check_idr_if_freed(rt_entry->proc_ctx))) {
				ofst = rt_entry->proc_ctx->offset_entry->offset;
				ofst_words =
					rt_entry->proc_ctx->is_lcl ?
					(ofst + ipa3_ctx->hdr_proc_ctx_tbl[HPC_TBL_LCL]
					.start_offset) >> 5 :
					(ofst + ipa3_ctx->hdr_proc_ctx_tbl[HPC_TBL_SYS]
					.start_offset) >> 5;

				rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
				.proc_ctx_valid = 1;
				rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
				.offset_words = ofst_words;
				rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
				.is_in_sram = !rt_entry->proc_ctx->is_lcl;

				if ((IPA_HDR_PROC_MARK_DSCP == rt_entry->proc_ctx->type) &&
					rt_entry->proc_ctx->pdn_dscp_params.valid)
				{
					rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
					.dscp_mark = rt_entry->proc_ctx->pdn_dscp_params.dscp_val;
				}
			} else {
				if (rt_entry->hdr)
					ofst = rt_entry->hdr->offset_entry->offset;
				else
					ofst = 0;

				rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
				.proc_ctx_valid = 0;
				rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
				.offset_words = ofst >> 2;
				rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
				.is_in_sram = !(rt_entry->hdr && rt_entry->hdr->is_lcl);
			}

			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].end_point =
				ipa_get_ep_mapping(rt_entry->rule.dst);
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.destination = rt_entry->rule.dst;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.attribute_mask = rt_entry->rule.attrib.attrib_mask;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.rule_idx = rule_idx;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.rule_id = rt_entry->rule_id;

			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.retain_hdr = rt_entry->rule.retain_hdr;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.close_aggr_irq_mode = rt_entry->rule.close_aggr_irq_mod;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.ttl_update = rt_entry->rule.ttl_update;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.qos_class = rt_entry->rule.qos_class;

			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.skip_ingress = rt_entry->rule.skip_ingress;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.esp_after_udp = rt_entry->rule.esp_after_udp;

			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.enable_stats = rt_entry->rule.enable_stats;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.counter_id = rt_entry->rule.cnt_idx;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.hashable = rt_entry->rule.hashable;

			/* Rule attributes */
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].tos =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_TOS) ?
				(rt_entry->rule.attrib.u.v4.tos) : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].tos_value =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_TOS_MASKED) ?
				(rt_entry->rule.attrib.tos_value) : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].tos_mask =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_TOS_MASKED) ?
				(rt_entry->rule.attrib.tos_mask) : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].protocol =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_PROTOCOL) ?
				(rt_entry->rule.attrib.u.v4.protocol) : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].src_addr =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_SRC_ADDR) ?
				htonl(rt_entry->rule.attrib.u.v4.src_addr) : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].src_addr_mask =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_SRC_ADDR) ?
				htonl(rt_entry->rule.attrib.u.v4.src_addr_mask) : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].dst_addr =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_DST_ADDR) ?
				htonl(rt_entry->rule.attrib.u.v4.dst_addr) : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].dst_addr_mask =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_DST_ADDR) ?
				htonl(rt_entry->rule.attrib.u.v4.dst_addr_mask) : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].src_port_low =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_SRC_PORT_RANGE) ?
				rt_entry->rule.attrib.src_port_lo : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].src_port_high =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_SRC_PORT_RANGE) ?
				rt_entry->rule.attrib.src_port_hi : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].dst_port_low =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_DST_PORT_RANGE) ?
				rt_entry->rule.attrib.dst_port_lo : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].dst_port_high =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_DST_PORT_RANGE) ?
				rt_entry->rule.attrib.dst_port_hi : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].type =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_TYPE) ?
				rt_entry->rule.attrib.type : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].code =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_CODE) ?
				rt_entry->rule.attrib.code : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].spi =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_SPI) ?
				rt_entry->rule.attrib.spi : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].vlan_id =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_VLAN_ID) ?
				rt_entry->rule.attrib.vlan_id : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].src_port =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_SRC_PORT) ?
				rt_entry->rule.attrib.src_port : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].dst_port =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_DST_PORT) ?
				rt_entry->rule.attrib.dst_port : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].payload_length =
				(rt_entry->rule.attrib.ext_attrib_mask & IPA_FLT_EXT_MTU) ?
				rt_entry->rule.attrib.payload_length : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].ether_type =
				((rt_entry->rule.attrib.attrib_mask & IPA_FLT_MAC_ETHER_TYPE) ||
				 (rt_entry->rule.attrib.ext_attrib_mask &
					IPA_FLT_EXT_L2TP_UDP_INNER_ETHER_TYPE)) ?
					rt_entry->rule.attrib.ether_type : 0;


			if ((rt_entry->rule.attrib.attrib_mask & IPA_FLT_MAC_SRC_ADDR_ETHER_II) ||
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_MAC_SRC_ADDR_802_3) ||
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_MAC_SRC_ADDR_802_1Q)) {
				for (i = 0; i < ETH_ALEN; i++)
				{
					rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].src_mac_addr |=
						((uint64_t)rt_entry->rule.attrib.src_mac_addr[i] << (8 * (5 - i)));
				}
				rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
				.src_mac_addr_mask = (uint64_t)rt_entry->rule.attrib.attrib_mask &
					(IPA_FLT_MAC_SRC_ADDR_ETHER_II |
					IPA_FLT_MAC_SRC_ADDR_802_3 |
					IPA_FLT_MAC_SRC_ADDR_802_1Q);
			}

			if ((rt_entry->rule.attrib.attrib_mask & IPA_FLT_MAC_DST_ADDR_ETHER_II) ||
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_MAC_DST_ADDR_802_3) ||
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_MAC_DST_ADDR_L2TP) ||
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_MAC_DST_ADDR_802_1Q) ||
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_L2TP_UDP_INNER_MAC_DST_ADDR) ||
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_L2TP_INNER_IPV4_DST_ADDR)) {
				for (i = 0; i < ETH_ALEN; i++)
				{
					rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].dst_mac_addr |=
						((uint64_t)rt_entry->rule.attrib.dst_mac_addr[i] << (8 * (5 - i)));
				}

				rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
				.dst_mac_addr_mask = (uint64_t)rt_entry->rule.attrib.attrib_mask &
					(IPA_FLT_MAC_DST_ADDR_ETHER_II |
					IPA_FLT_MAC_DST_ADDR_802_3 | IPA_FLT_MAC_DST_ADDR_L2TP |
					IPA_FLT_MAC_DST_ADDR_802_1Q |
					IPA_FLT_L2TP_UDP_INNER_MAC_DST_ADDR |
					IPA_FLT_L2TP_INNER_IPV4_DST_ADDR);
			}

			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].meta_data =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_META_DATA) ?
				rt_entry->rule.attrib.meta_data : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.meta_data_mask =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_META_DATA) ?
				rt_entry->rule.attrib.meta_data_mask : 0;

			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.l2tp_inner_ip_type =
				(rt_entry->rule.attrib.attrib_mask &
				IPA_FLT_L2TP_INNER_IP_TYPE) ?
				rt_entry->rule.attrib.type : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].tcp_syn =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_TCP_SYN) ? 1 : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.tcp_syn_l2tp = ((rt_entry->rule.attrib.attrib_mask &
							IPA_FLT_TCP_SYN_L2TP) ||
							(rt_entry->rule.attrib.ext_attrib_mask &
							IPA_FLT_EXT_L2TP_UDP_TCP_SYN)) ? 1 : 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx].frag =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_FRAGMENT) ? 1 : 0;

			/* Attribute equations are currently NULL and can be enabled during rt_hw */
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.is_rt_hw = 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.protocol_eq = 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.num_offset_meq128 = 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.num_offset_meq32 = 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.num_ihl_offset_meq32 = 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.is_metadata_meq32_persent = 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.num_ihl_offset_range_16 = 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.num_ihl_offset_eq16 = 0;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.num_ihl_offset_eq32 = 0;

			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.prio = rt_entry->prio;
			rt_rule_stats->v4_rt_tbl[tbl_idx].v4_rt_rule[rule_idx]
			.max_prio = rt_entry->rule.max_prio;


			rule_idx++;
		}

		tbl_idx++;
	}

	if(copy_to_user((void __user *)arg,
		(u8 *)rt_rule_stats,
		alloc_size)) {
		IPA_STATS_ERR("copy to user failed");
		ret = -EFAULT;
		goto stats_fail;
	}

stats_fail:
	mutex_unlock(&ipa3_ctx->lock);
	kfree(rt_rule_stats);
	return ret;
}


static int ipa_get_v6_rt_rule_stats(unsigned long arg) {
	struct ipa_lnx_v6_rt_rule_stats *rt_rule_stats;
	int alloc_size;
	int j;
	int rule_idx, tbl_idx;
	struct ipa3_rt_tbl *rt_tbl;
	struct ipa3_rt_entry *rt_entry;
	struct ipa3_rt_tbl_set *rt_set;
	u32 ofst;
	u32 ofst_words;
	int ret = 0, i;

	if (ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_tables >=
		TLPD_NUM_MAX_RT_FLT_TBL) {
		IPA_STATS_ERR("Rejecting rt rule log packet collection\n");
		return -EFAULT;
	}

	alloc_size = sizeof(struct ipa_lnx_v6_rt_rule_stats) +
		(ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_tables *
		sizeof(struct ipa_lnx_v6_rt_rule_table));

	rt_rule_stats = (struct ipa_lnx_v6_rt_rule_stats *) memdup_user((
		const void __user *)arg, alloc_size);
	if (IS_ERR(rt_rule_stats)) {
		IPA_STATS_ERR("copy from user failed");
		return -ENOMEM;
	}

	/* IPv6 RT structures */
	mutex_lock(&ipa3_ctx->lock);
	rt_set = &ipa3_ctx->rt_tbl_set[IPA_IP_v6];

	rt_rule_stats->ip6_rt_tbl_hash_local =
		ipa3_ctx->rt_tbl_hash_lcl[IPA_IP_v6];
	rt_rule_stats->ip6_rt_tbl_nhash_local =
		ipa3_ctx->rt_tbl_nhash_lcl[IPA_IP_v6];
	rt_rule_stats->num_v6_rt_table =
		ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_tables;

	tbl_idx = 0;
	list_for_each_entry(rt_tbl, &rt_set->head_rt_tbl_list, link) {
		if (tbl_idx == TLPD_NUM_MAX_RT_FLT_TBL && tbl_idx >=
			ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_tables) {
			IPA_STATS_ERR("RT6 table limit reached\n");
			goto stats_fail;
		}

		rt_rule_stats->v6_rt_tbl[tbl_idx].tbl_index = rt_tbl->idx;
		rt_rule_stats->v6_rt_tbl[tbl_idx].tbl_ref_count = rt_tbl->ref_cnt;
		rt_rule_stats->v6_rt_tbl[tbl_idx].tbl_name =
			(uint8_t)ipa_get_rt_tbl_index_from_name(rt_tbl->name);
		memcpy(rt_rule_stats->v6_rt_tbl[tbl_idx].rt_tbl_name, rt_tbl->name,
			IPA_RESOURCE_NAME_MAX);
		rt_rule_stats->v6_rt_tbl[tbl_idx].num_rt_rule =
			ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_rules[tbl_idx];
		rule_idx = 0;
		list_for_each_entry(rt_entry, &rt_tbl->head_rt_rule_list, link) {
			if (rule_idx == TLPD_NUM_MAX_RT_FLT_RULE && rule_idx >=
			ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_rules[tbl_idx]) {
				IPA_STATS_ERR("RT6 Rule limit reached\n");
				goto stats_fail;
			}
			if (rt_entry->proc_ctx &&
				(!ipa3_check_idr_if_freed(rt_entry->proc_ctx))) {
				ofst = rt_entry->proc_ctx->offset_entry->offset;
				ofst_words =
					rt_entry->proc_ctx->is_lcl ?
					(ofst + ipa3_ctx->hdr_proc_ctx_tbl[HPC_TBL_LCL]
					.start_offset) >> 5 :
					(ofst + ipa3_ctx->hdr_proc_ctx_tbl[HPC_TBL_SYS]
					.start_offset) >> 5;
				rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
				.proc_ctx_valid = 1;
				rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
				.offset_words = ofst_words;
				rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
				.is_in_sram = !rt_entry->proc_ctx->is_lcl;

				if ((IPA_HDR_PROC_MARK_DSCP == rt_entry->proc_ctx->type) &&
					rt_entry->proc_ctx->pdn_dscp_params.valid)
				{
					rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
					.dscp_mark = rt_entry->proc_ctx->pdn_dscp_params.dscp_val;
				}
			} else {
				if (rt_entry->hdr) ofst = rt_entry->hdr->offset_entry->offset;
				else ofst = 0;
				rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
				.proc_ctx_valid = 0;
				rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
				.offset_words = ofst >> 2;
				rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
				.is_in_sram = !(rt_entry->hdr && rt_entry->hdr->is_lcl);
			}

			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].end_point =
				ipa_get_ep_mapping(rt_entry->rule.dst);
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.destination = rt_entry->rule.dst;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.attribute_mask = rt_entry->rule.attrib.attrib_mask;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].rule_idx =
				rule_idx;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].rule_id =
				rt_entry->rule_id;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].retain_hdr =
				rt_entry->rule.retain_hdr;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.close_aggr_irq_mode = rt_entry->rule.close_aggr_irq_mod;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.ttl_update = rt_entry->rule.ttl_update;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.qos_class = rt_entry->rule.qos_class;

			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.skip_ingress = rt_entry->rule.skip_ingress;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.esp_after_udp = rt_entry->rule.esp_after_udp;

			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.enable_stats = rt_entry->rule.enable_stats;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].counter_id =
				rt_entry->rule.cnt_idx;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].hashable =
				rt_entry->rule.hashable;
			/* Rule attributes */
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].tc =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_TC) ?
				(rt_entry->rule.attrib.u.v6.tc) : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].tos_value =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_TOS_MASKED) ?
				(rt_entry->rule.attrib.tos_value) : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].flow_label =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_FLOW_LABEL) ?
				(rt_entry->rule.attrib.u.v6.flow_label) : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].tos_mask =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_TOS_MASKED) ?
				(rt_entry->rule.attrib.tos_mask) : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].nxt_hdr =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_NEXT_HDR) ?
				(rt_entry->rule.attrib.u.v6.next_hdr) : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.nxt_hdr_ext = (rt_entry->rule.attrib.ext_attrib_mask &
							IPA_FLT_EXT_NEXT_HDR) ?
							(rt_entry->rule.attrib.u.v6.next_hdr) : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].type =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_TYPE) ?
				rt_entry->rule.attrib.type : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].code =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_CODE) ?
				rt_entry->rule.attrib.code : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].src_port_low =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_SRC_PORT_RANGE) ?
				rt_entry->rule.attrib.src_port_lo : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].src_port_high =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_SRC_PORT_RANGE) ?
				rt_entry->rule.attrib.src_port_hi : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].dst_port_low =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_DST_PORT_RANGE) ?
				rt_entry->rule.attrib.dst_port_lo : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].dst_port_high =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_DST_PORT_RANGE) ?
				rt_entry->rule.attrib.dst_port_hi : 0;

			for (j = 0; j < 4; j++) {
				if (rt_entry->rule.attrib.attrib_mask & IPA_FLT_SRC_ADDR) {
					rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
					.src_addr[j] =
						htonl(rt_entry->rule.attrib.u.v6.src_addr[j]);
					rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
					.src_addr_mask[j] =
						htonl(rt_entry->rule.attrib.u.v6.src_addr_mask[j]);
				}

				if (rt_entry->rule.attrib.attrib_mask & IPA_FLT_DST_ADDR) {
					rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
					.dst_addr[j] =
						htonl(rt_entry->rule.attrib.u.v6.dst_addr[j]);
					rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
					.dst_addr_mask[j] =
						htonl(rt_entry->rule.attrib.u.v6.dst_addr_mask[j]);
				}
			}

			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].spi =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_SPI) ?
				rt_entry->rule.attrib.spi : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].vlan_id =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_VLAN_ID) ?
				rt_entry->rule.attrib.vlan_id : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].src_port =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_SRC_PORT) ?
				rt_entry->rule.attrib.src_port : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].dst_port =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_DST_PORT) ?
				rt_entry->rule.attrib.dst_port : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.payload_length = (rt_entry->rule.attrib.ext_attrib_mask &
							IPA_FLT_EXT_MTU) ?
							rt_entry->rule.attrib.payload_length : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].ether_type =
				((rt_entry->rule.attrib.attrib_mask & IPA_FLT_MAC_ETHER_TYPE) ||
				 (rt_entry->rule.attrib.ext_attrib_mask &
					IPA_FLT_EXT_L2TP_UDP_INNER_ETHER_TYPE)) ?
					rt_entry->rule.attrib.ether_type : 0;

			if ((rt_entry->rule.attrib.attrib_mask &
				IPA_FLT_MAC_SRC_ADDR_ETHER_II) ||
				(rt_entry->rule.attrib.attrib_mask &
				IPA_FLT_MAC_SRC_ADDR_802_3) ||
				(rt_entry->rule.attrib.attrib_mask &
				IPA_FLT_MAC_SRC_ADDR_802_1Q)) {
				for (i = 0; i < ETH_ALEN; i++)
				{
					rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].src_mac_addr |=
						((uint64_t)rt_entry->rule.attrib.src_mac_addr[i] << (8 * (5 - i)));
				}
				rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
				.src_mac_addr_mask = (uint64_t)rt_entry->rule.attrib.attrib_mask &
										(IPA_FLT_MAC_SRC_ADDR_ETHER_II |
										IPA_FLT_MAC_SRC_ADDR_802_3 |
										IPA_FLT_MAC_SRC_ADDR_802_1Q);
			}

			if ((rt_entry->rule.attrib.attrib_mask & IPA_FLT_MAC_DST_ADDR_ETHER_II) ||
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_MAC_DST_ADDR_802_3) ||
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_MAC_DST_ADDR_L2TP) ||
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_MAC_DST_ADDR_802_1Q) ||
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_L2TP_UDP_INNER_MAC_DST_ADDR)) {
				for (i = 0; i < ETH_ALEN; i++)
				{
					rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].dst_mac_addr |=
						((uint64_t)rt_entry->rule.attrib.dst_mac_addr[i] << (8 * (5 - i)));
				}
				rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
				.dst_mac_addr_mask =
					(uint64_t)rt_entry->rule.attrib.attrib_mask &
					(IPA_FLT_MAC_DST_ADDR_ETHER_II |
					IPA_FLT_MAC_DST_ADDR_802_3 |
					IPA_FLT_MAC_DST_ADDR_L2TP |
					IPA_FLT_MAC_DST_ADDR_802_1Q |
					IPA_FLT_L2TP_UDP_INNER_MAC_DST_ADDR);
			}

			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].meta_data =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_META_DATA) ?
				rt_entry->rule.attrib.meta_data : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.meta_data_mask =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_META_DATA) ?
				rt_entry->rule.attrib.meta_data_mask : 0;

			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.l2tp_inner_ip_type =
				(rt_entry->rule.attrib.attrib_mask &
				IPA_FLT_L2TP_INNER_IP_TYPE) ?
				rt_entry->rule.attrib.type : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].tcp_syn =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_TCP_SYN) ? 1 : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.tcp_syn_l2tp =
				((rt_entry->rule.attrib.attrib_mask &
				IPA_FLT_TCP_SYN_L2TP) ||
				(rt_entry->rule.attrib.ext_attrib_mask &
				IPA_FLT_EXT_L2TP_UDP_TCP_SYN)) ?
				1 : 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx].frag =
				(rt_entry->rule.attrib.attrib_mask & IPA_FLT_FRAGMENT) ? 1 : 0;

			/* Attribute equations are currently NULL and can be enabled during rt_hw */
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.is_rt_hw = 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.protocol_eq = 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.num_offset_meq128 = 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.num_offset_meq32 = 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.num_ihl_offset_meq32 = 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.is_metadata_meq32_persent = 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.num_ihl_offset_range_16 = 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.num_ihl_offset_eq16 = 0;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.num_ihl_offset_eq32 = 0;

			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.prio = rt_entry->prio;
			rt_rule_stats->v6_rt_tbl[tbl_idx].v6_rt_rule[rule_idx]
			.max_prio = rt_entry->rule.max_prio;

			rule_idx++;
		}

		tbl_idx++;
	}

	mutex_unlock(&ipa3_ctx->lock);

	if (copy_to_user((void __user *)arg,
					 (u8 *)rt_rule_stats,
					 alloc_size)) {
		IPA_STATS_ERR("copy to user failed");
		ret = -EFAULT;
		goto stats_fail;
	}

stats_fail:
	mutex_unlock(&ipa3_ctx->lock);
	kfree(rt_rule_stats);
	return ret;
}

static int ipa_get_v4_flt_rule_stats(unsigned long arg)
{
	struct ipa_lnx_v4_flt_rule_stats *flt_rule_stats;
	int alloc_size;
	int i, j, s, r;
	int rule_idx, tbl_idx;
	struct ipa3_flt_tbl *flt_tbl;
	struct ipa3_flt_entry *flt_entry;
	struct ipa3_rt_tbl *rt_tbl;
	int eq;
	struct ipa_rule_attrib *attrib;

	if (ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v4_tables >=
		TLPD_NUM_MAX_RT_FLT_TBL) {
		IPA_STATS_ERR("Rejecting flt rule log packet collection\n");
		return -EFAULT;
	}

	alloc_size = sizeof(struct ipa_lnx_v4_flt_rule_stats);

	flt_rule_stats = (struct ipa_lnx_v4_flt_rule_stats *) memdup_user((
		const void __user *)arg, alloc_size);
	if (IS_ERR(flt_rule_stats)) {
		IPA_STATS_ERR("copy from user failed");
		return -ENOMEM;
	}

	/* IPv4 flt structures */
	mutex_lock(&ipa3_ctx->lock);
	tbl_idx = 0; rule_idx = 0;

	for (j = 0; j < ipa3_ctx->ipa_num_pipes; j++) {
		if (tbl_idx == TLPD_NUM_MAX_RT_FLT_TBL) {
			IPA_STATS_ERR("FLT4 table limit reached\n");
			break;
		}

		if (!ipa_is_ep_support_flt(j))
		{
			IPA_STATS_DBG("j %d tbl idx %d .. continue\n", j , tbl_idx);
			continue;
		}

		flt_tbl = &ipa3_ctx->flt_tbl[j][IPA_IP_v4];
		i = 0;
		rule_idx = 0;
		list_for_each_entry(flt_entry, &flt_tbl->head_flt_rule_list, link) {
			if (flt_entry->cookie != IPA_FLT_COOKIE) continue;
			if (flt_entry->rule.eq_attrib_type) {
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
				.rt_table_index = flt_entry->rule.rt_tbl_idx;
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
				.attrib_mask = flt_entry->rule.eq_attrib.rule_eq_bitmap;
				eq = true;
			} else {
				rt_tbl = ipa3_id_find(flt_entry->rule.rt_tbl_hdl);
				if (rt_tbl == NULL ||
					rt_tbl->cookie != IPA_RT_TBL_COOKIE) {
					flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.rt_table_index =  ~0;
				}
				else {
					flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.rt_table_index = rt_tbl->idx;
				}

				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
				.attrib_mask = flt_entry->rule.attrib.attrib_mask;
				eq = false;
			}

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].ep_idx = j;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].rule_idx =
				tbl_idx;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].action =
				flt_entry->rule.action;


			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].retain_hdr =
				flt_entry->rule.retain_hdr;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].equation = eq;

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].hashable =
				flt_entry->rule.hashable;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].rule_id =
				flt_entry->rule_id;

			if (flt_entry->rule.hashable) {
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].in_sys =
					flt_tbl->in_sys[IPA_RULE_HASHABLE];
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].force_sys =
					flt_tbl->force_sys[IPA_RULE_HASHABLE];

			} else {
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].in_sys =
					flt_tbl->in_sys[IPA_RULE_NON_HASHABLE];
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].force_sys =
					flt_tbl->force_sys[IPA_RULE_NON_HASHABLE];
			}

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].enable_stats =
				flt_entry->rule.enable_stats;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].counter_id =
				flt_entry->rule.cnt_idx;

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].pdn_index =
				flt_entry->rule.pdn_idx;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].set_metadata =
				flt_entry->rule.set_metadata;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.close_aggr_irq_mode = flt_entry->rule.close_aggr_irq_mod;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.ttl_update = flt_entry->rule.ttl_update;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.qos_class = flt_entry->rule.qos_class;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.esp_after_udp = flt_entry->rule.esp_after_udp;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.is_flt_hw = 0;


			attrib = &flt_entry->rule.attrib;
			// Rule attributes
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].tos =
				(attrib->attrib_mask & IPA_FLT_TOS) ?
				flt_entry->rule.attrib.u.v4.tos : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].tos_value =
				(attrib->attrib_mask & IPA_FLT_TOS_MASKED) ?
				flt_entry->rule.attrib.tos_value : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].tos_mask =
				(attrib->attrib_mask & IPA_FLT_TOS_MASKED) ?
				flt_entry->rule.attrib.tos_mask : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].protocol =
				(attrib->attrib_mask & IPA_FLT_PROTOCOL) ?
				flt_entry->rule.attrib.u.v4.protocol : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].src_addr =
				(attrib->attrib_mask & IPA_FLT_SRC_ADDR) ?
				htonl(flt_entry->rule.attrib.u.v4.src_addr) : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].src_addr_mask =
				(attrib->attrib_mask & IPA_FLT_SRC_ADDR) ?
				htonl(flt_entry->rule.attrib.u.v4.src_addr_mask) : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].dst_addr =
				(attrib->attrib_mask & IPA_FLT_DST_ADDR) ?
				htonl(flt_entry->rule.attrib.u.v4.dst_addr) : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].dst_addr_mask =
				(attrib->attrib_mask & IPA_FLT_DST_ADDR) ?
				htonl(flt_entry->rule.attrib.u.v4.dst_addr_mask) : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].src_port_low =
				(attrib->attrib_mask & IPA_FLT_SRC_PORT_RANGE) ?
				flt_entry->rule.attrib.src_port_lo : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].src_port_high =
				(attrib->attrib_mask & IPA_FLT_SRC_PORT_RANGE) ?
				flt_entry->rule.attrib.src_port_hi : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].dst_port_low =
				(attrib->attrib_mask & IPA_FLT_DST_PORT_RANGE) ?
				flt_entry->rule.attrib.dst_port_lo : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].dst_port_high =
				(attrib->attrib_mask & IPA_FLT_DST_PORT_RANGE) ?
				flt_entry->rule.attrib.dst_port_hi : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].type =
				(attrib->attrib_mask & IPA_FLT_TYPE) ?
				flt_entry->rule.attrib.type : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].code =
				(attrib->attrib_mask & IPA_FLT_CODE) ?
				flt_entry->rule.attrib.code : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].spi =
				(attrib->attrib_mask & IPA_FLT_SPI) ?
				flt_entry->rule.attrib.spi : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].src_port =
				(attrib->attrib_mask & IPA_FLT_SRC_PORT) ?
				flt_entry->rule.attrib.src_port : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].dst_port =
				(attrib->attrib_mask & IPA_FLT_DST_PORT) ?
				flt_entry->rule.attrib.dst_port : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].meta_data =
				(attrib->attrib_mask & IPA_FLT_META_DATA) ?
				flt_entry->rule.attrib.meta_data : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.meta_data_mask =
				(attrib->attrib_mask & IPA_FLT_META_DATA) ?
				flt_entry->rule.attrib.meta_data_mask : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].frag =
				(attrib->attrib_mask & IPA_FLT_META_DATA) ? 1 : 0;

			if ((attrib->attrib_mask & IPA_FLT_MAC_SRC_ADDR_ETHER_II) ||
				(attrib->attrib_mask & IPA_FLT_MAC_SRC_ADDR_802_3) ||
				(attrib->attrib_mask & IPA_FLT_MAC_SRC_ADDR_802_1Q)) {
				for (i = 0; i < ETH_ALEN; i++)
				{
					flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].src_mac_addr |=
						((uint64_t)flt_entry->rule.attrib.src_mac_addr[i] << (8 * (5 - i)));
					flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].src_mac_addr_mask |=
						(uint64_t)attrib->attrib_mask &
										(IPA_FLT_MAC_SRC_ADDR_ETHER_II |
										IPA_FLT_MAC_SRC_ADDR_802_3 |
										IPA_FLT_MAC_SRC_ADDR_802_1Q);
				}
			}

			if ((attrib->attrib_mask & IPA_FLT_MAC_DST_ADDR_ETHER_II) ||
				(attrib->attrib_mask & IPA_FLT_MAC_DST_ADDR_802_3) ||
				(attrib->attrib_mask & IPA_FLT_MAC_DST_ADDR_L2TP) ||
				(attrib->attrib_mask & IPA_FLT_MAC_DST_ADDR_802_1Q) ||
				(attrib->attrib_mask & IPA_FLT_L2TP_UDP_INNER_MAC_DST_ADDR)) {
				for (i = 0; i < ETH_ALEN; i++)
				{
					flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].dst_mac_addr |=
						((uint64_t)flt_entry->rule.attrib.dst_mac_addr[i] << (8 * (5 - i)));
					flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].dst_mac_addr_mask |=
						(uint64_t)attrib->attrib_mask &
										(IPA_FLT_MAC_DST_ADDR_ETHER_II |
										IPA_FLT_MAC_DST_ADDR_802_3 |
										IPA_FLT_MAC_DST_ADDR_L2TP |
										IPA_FLT_MAC_DST_ADDR_802_1Q|
										IPA_FLT_L2TP_UDP_INNER_MAC_DST_ADDR);
				}
			}

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.payload_length =
				(attrib->ext_attrib_mask & IPA_FLT_EXT_MTU) ?
				flt_entry->rule.attrib.payload_length : 0;

			if (attrib->attrib_mask & IPA_FLT_MAC_ETHER_TYPE ||
				attrib->ext_attrib_mask &
				IPA_FLT_EXT_L2TP_UDP_INNER_ETHER_TYPE) {
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
				.ether_type = flt_entry->rule.attrib.ether_type;
			}

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].vlan_id =
				(attrib->attrib_mask & IPA_FLT_VLAN_ID) ?
				flt_entry->rule.attrib.vlan_id : 0;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx].tcp_syn =
				(attrib->attrib_mask & IPA_FLT_TCP_SYN) ? 1 : 0;

			if (attrib->attrib_mask & IPA_FLT_TCP_SYN_L2TP ||
				attrib->ext_attrib_mask & IPA_FLT_EXT_L2TP_UDP_TCP_SYN) {
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
				.tcp_syn_l2tp = 1;
			}

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.l2tp_inner_ip_type =
				(attrib->attrib_mask & IPA_FLT_L2TP_INNER_IP_TYPE) ?
				flt_entry->rule.attrib.type : 0;

			if (attrib->attrib_mask & IPA_FLT_L2TP_INNER_IPV4_DST_ADDR)
			{
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
				.dst_addr = htonl(flt_entry->rule.attrib.u.v4.dst_addr);

				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
				.dst_addr_mask = htonl(flt_entry->rule.attrib.u.v4.dst_addr_mask);
			}

			/* Attribute equations are currently NULL and can be enabled during flt_hw */
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.num_offset_meq128 = flt_entry->rule.eq_attrib.num_offset_meq_128;
			for (r = 0; r < flt_entry->rule.eq_attrib.num_offset_meq_128; r++)
			{
				for (s = 0; s < 16; s ++)
				{
					flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.offset_meq_128[r].value[s] =
						flt_entry->rule.eq_attrib.offset_meq_128[r].value[s];
					flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.offset_meq_128[r].mask[s] =
						flt_entry->rule.eq_attrib.offset_meq_128[r].mask[s];
				}
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
				.offset_meq_128[r].offset =
					flt_entry->rule.eq_attrib.offset_meq_128[r].offset;
			}

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.num_offset_meq32 = flt_entry->rule.eq_attrib.num_offset_meq_32;
			for (r = 0; r < flt_entry->rule.eq_attrib.num_offset_meq_32; r++)
			{
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.offset_meq32[r].offset = flt_entry->rule.eq_attrib.offset_meq_32[r].offset;
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.offset_meq32[r].value = flt_entry->rule.eq_attrib.offset_meq_32[r].value;
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.offset_meq32[r].mask = flt_entry->rule.eq_attrib.offset_meq_32[r].mask;

			}

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.num_ihl_offset_meq32 = flt_entry->rule.eq_attrib.num_ihl_offset_meq_32;
			for (r = 0; r < flt_entry->rule.eq_attrib.num_ihl_offset_meq_32; r++)
			{
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_meq32[r].offset = flt_entry->rule.eq_attrib.ihl_offset_meq_32[r].offset;
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_meq32[r].value = flt_entry->rule.eq_attrib.ihl_offset_meq_32[r].value;
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_meq32[r].mask = flt_entry->rule.eq_attrib.ihl_offset_meq_32[r].mask;

			}

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.is_metadata_meq32_persent = flt_entry->rule.eq_attrib.metadata_meq32_present;
			for (r = 0; r < flt_entry->rule.eq_attrib.metadata_meq32_present; r++)
			{
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.metadata_meq32[r].offset = flt_entry->rule.eq_attrib.metadata_meq32.offset;
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.metadata_meq32[r].value = flt_entry->rule.eq_attrib.metadata_meq32.value;
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.metadata_meq32[r].mask = flt_entry->rule.eq_attrib.metadata_meq32.mask;
			}

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.num_ihl_offset_range_16 = flt_entry->rule.eq_attrib.num_ihl_offset_range_16;
			for (r = 0; r < flt_entry->rule.eq_attrib.num_ihl_offset_range_16; r++)
			{
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_range16[r].offset = flt_entry->rule.eq_attrib.ihl_offset_range_16[r].offset;
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_range16[r].range_low = flt_entry->rule.eq_attrib.ihl_offset_range_16[r].range_low;
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_range16[r].range_high = flt_entry->rule.eq_attrib.ihl_offset_range_16[r].range_high;
			}

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.num_ihl_offset_eq32 = flt_entry->rule.eq_attrib.ihl_offset_eq_32_present;
			for (r = 0; r < flt_entry->rule.eq_attrib.ihl_offset_eq_32_present; r++)
			{
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_eq32[r].offset = flt_entry->rule.eq_attrib.ihl_offset_eq_32.offset;
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_eq32[r].value = flt_entry->rule.eq_attrib.ihl_offset_eq_32.value;
			}

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.num_ihl_offset_eq16 = flt_entry->rule.eq_attrib.ihl_offset_eq_16_present;
			for (r = 0; r < flt_entry->rule.eq_attrib.ihl_offset_eq_16_present; r++)
			{
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_eq16[r].offset = flt_entry->rule.eq_attrib.ihl_offset_eq_16.offset;
				flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_eq16[r].value = flt_entry->rule.eq_attrib.ihl_offset_eq_16.value;
			}

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.protocol_eq = flt_entry->rule.eq_attrib.protocol_eq;

			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.pure_ack = flt_entry->rule.eq_attrib.tos_eq_present;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.frag_encoding = flt_entry->rule.eq_attrib.is_frag_encoding;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.prio = flt_entry->prio;
			flt_rule_stats->v4_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.max_prio = flt_entry->rule.max_prio;

			rule_idx++;
		}

		if (rule_idx)
		{
			flt_rule_stats->v4_flt_tbl[tbl_idx].num_flt_rule  =  rule_idx;
			tbl_idx++;
		}
	}
	flt_rule_stats->num_v4_flt_table = tbl_idx;

	mutex_unlock(&ipa3_ctx->lock);

	if(copy_to_user((void __user *)arg,
		(u8 *)flt_rule_stats,
		alloc_size)) {
		IPA_STATS_ERR("copy to user failed");
		kfree(flt_rule_stats);
		return -EFAULT;
	}

	kfree(flt_rule_stats);
	return 0;
}

static int ipa_get_v6_flt_rule_stats(unsigned long arg)
{
	struct ipa_lnx_v6_flt_rule_stats *flt_rule_stats;
	int alloc_size;
	int i, j, s, r;
	int rule_idx, tbl_idx;
	struct ipa3_flt_tbl *flt_tbl;
	struct ipa3_flt_entry *flt_entry;
	struct ipa3_rt_tbl *rt_tbl;
	int eq;
	struct ipa_rule_attrib *attrib;

	if (ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v6_tables >=
		TLPD_NUM_MAX_RT_FLT_TBL) {
		IPA_STATS_ERR("Rejecting flt rule log packet collection\n");
		return -EFAULT;
	}

	alloc_size = sizeof(struct ipa_lnx_v6_flt_rule_stats);

	flt_rule_stats = (struct ipa_lnx_v6_flt_rule_stats *) memdup_user((
		const void __user *)arg, alloc_size);
	if (IS_ERR(flt_rule_stats)) {
		IPA_STATS_ERR("copy from user failed");
		return -ENOMEM;
	}


	/* V6 flt rules*/
	mutex_lock(&ipa3_ctx->lock);
	tbl_idx = 0;
	rule_idx = 0;

	for (j = 0; j < ipa3_ctx->ipa_num_pipes; j++) {
		if (tbl_idx == TLPD_NUM_MAX_RT_FLT_TBL) {
			IPA_STATS_ERR("FLT6 table limit reached\n");
			break;
		}

		if (!ipa_is_ep_support_flt(j))
		{
			IPA_STATS_DBG("j %d tbl idx %d .. continue\n", j , tbl_idx);
			continue;
		}

		i = 0;
		flt_tbl = &ipa3_ctx->flt_tbl[j][IPA_IP_v6];
		rule_idx = 0;

		list_for_each_entry(flt_entry, &flt_tbl->head_flt_rule_list, link) {
			if (flt_entry->cookie != IPA_FLT_COOKIE)
				continue;
			if (flt_entry->rule.eq_attrib_type) {
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
				.rt_table_index = flt_entry->rule.rt_tbl_idx;
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
				.attrib_mask = flt_entry->rule.eq_attrib.rule_eq_bitmap;
				eq = true;
			} else {
				rt_tbl = ipa3_id_find(flt_entry->rule.rt_tbl_hdl);
				if (rt_tbl == NULL ||
					rt_tbl->cookie != IPA_RT_TBL_COOKIE) {
					flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.rt_table_index =  ~0;
				} else
				{
					flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.rt_table_index = rt_tbl->idx;
				}

				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
				.attrib_mask = flt_entry->rule.attrib.attrib_mask;
				eq = false;
			}

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].ep_idx = j;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].rule_idx =
				tbl_idx;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].action =
				flt_entry->rule.action;

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].retain_hdr =
				flt_entry->rule.retain_hdr;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.equation = eq;

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].hashable =
				flt_entry->rule.hashable;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].rule_id =
				flt_entry->rule_id;

			if (flt_entry->rule.hashable) {
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].in_sys =
					flt_tbl->in_sys[IPA_RULE_HASHABLE];
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].force_sys =
					flt_tbl->force_sys[IPA_RULE_HASHABLE];

			}
			else {
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].in_sys =
					flt_tbl->in_sys[IPA_RULE_NON_HASHABLE];
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].force_sys =
					flt_tbl->force_sys[IPA_RULE_NON_HASHABLE];
			}

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].enable_stats =
				flt_entry->rule.enable_stats;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].counter_id =
				flt_entry->rule.cnt_idx;

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].pdn_index =
				flt_entry->rule.pdn_idx;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].set_metadata =
				flt_entry->rule.set_metadata;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.close_aggr_irq_mode = flt_entry->rule.close_aggr_irq_mod;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.ttl_update = flt_entry->rule.ttl_update;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.qos_class = flt_entry->rule.qos_class;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.esp_after_udp = flt_entry->rule.esp_after_udp;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.is_flt_hw = 0;


			attrib = &flt_entry->rule.attrib;
			//Rule attributes
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].tos_value =
				(attrib->attrib_mask & IPA_FLT_TOS_MASKED) ?
				flt_entry->rule.attrib.tos_value : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].tos_mask =
				(attrib->attrib_mask & IPA_FLT_TOS_MASKED) ?
				flt_entry->rule.attrib.tos_mask : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.src_port_low =
				(attrib->attrib_mask & IPA_FLT_SRC_PORT_RANGE) ?
				flt_entry->rule.attrib.src_port_lo : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.src_port_high =
				(attrib->attrib_mask & IPA_FLT_SRC_PORT_RANGE) ?
				flt_entry->rule.attrib.src_port_hi : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.dst_port_low =
				(attrib->attrib_mask & IPA_FLT_DST_PORT_RANGE) ?
				flt_entry->rule.attrib.dst_port_lo : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.dst_port_high =
				(attrib->attrib_mask & IPA_FLT_DST_PORT_RANGE) ?
				flt_entry->rule.attrib.dst_port_hi : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].type =
				(attrib->attrib_mask & IPA_FLT_TYPE) ?
				flt_entry->rule.attrib.type : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].code =
				(attrib->attrib_mask & IPA_FLT_CODE) ?
				flt_entry->rule.attrib.code : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].spi =
				(attrib->attrib_mask & IPA_FLT_SPI) ?
				flt_entry->rule.attrib.spi : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].src_port =
				(attrib->attrib_mask & IPA_FLT_SRC_PORT) ?
				flt_entry->rule.attrib.src_port : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].dst_port =
				(attrib->attrib_mask & IPA_FLT_DST_PORT) ?
				flt_entry->rule.attrib.dst_port : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].tc =
				(attrib->attrib_mask & IPA_FLT_TC) ?
				flt_entry->rule.attrib.u.v6.tc : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].flow_label =
				(attrib->attrib_mask & IPA_FLT_FLOW_LABEL) ?
				flt_entry->rule.attrib.u.v6.flow_label : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].nxt_hdr =
				(attrib->attrib_mask & IPA_FLT_NEXT_HDR) ?
				flt_entry->rule.attrib.u.v6.next_hdr : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].frag =
				(attrib->attrib_mask & IPA_FLT_META_DATA) ? 1 : 0;

			for (i = 0; i < 4; i++) {
				if (flt_entry->rule.attrib.attrib_mask & IPA_FLT_SRC_ADDR) {
					flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.src_addr[i] =
						htonl(flt_entry->rule.attrib.u.v6.src_addr[i]);
					flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.src_addr_mask[i] =
						htonl(flt_entry->rule.attrib.u.v6.src_addr_mask[i]);
				}

				if (flt_entry->rule.attrib.attrib_mask & IPA_FLT_DST_ADDR) {
					flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.dst_addr[i] =
						htonl(flt_entry->rule.attrib.u.v6.dst_addr[i]);
					flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.dst_addr_mask[i] =
						htonl(flt_entry->rule.attrib.u.v6.dst_addr_mask[i]);
				}
			}

			if ((attrib->attrib_mask & IPA_FLT_MAC_SRC_ADDR_ETHER_II) ||
				(attrib->attrib_mask & IPA_FLT_MAC_SRC_ADDR_802_3) ||
				(attrib->attrib_mask & IPA_FLT_MAC_SRC_ADDR_802_1Q)) {
				for (i = 0; i < ETH_ALEN; i++)
				{
					flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.src_mac_addr |=
						((uint64_t)flt_entry->rule.attrib.src_mac_addr[i] << (8 * (5 - i)));
					flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.src_mac_addr_mask |=
						(uint64_t)attrib->attrib_mask &
										(IPA_FLT_MAC_SRC_ADDR_ETHER_II |
										IPA_FLT_MAC_SRC_ADDR_802_3 |
										IPA_FLT_MAC_SRC_ADDR_802_1Q);
				}
			}

			if ((attrib->attrib_mask & IPA_FLT_MAC_DST_ADDR_ETHER_II) ||
				(attrib->attrib_mask & IPA_FLT_MAC_DST_ADDR_802_3) ||
				(attrib->attrib_mask & IPA_FLT_MAC_DST_ADDR_L2TP) ||
				(attrib->attrib_mask & IPA_FLT_MAC_DST_ADDR_802_1Q) ||
				(attrib->attrib_mask & IPA_FLT_L2TP_UDP_INNER_MAC_DST_ADDR)) {
				for (i = 0; i < ETH_ALEN; i++)
				{
					flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].dst_mac_addr |=
						((uint64_t)flt_entry->rule.attrib.dst_mac_addr[i] << (8 * (5 - i)));
					flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.dst_mac_addr_mask |= (uint64_t)attrib->attrib_mask &
										(IPA_FLT_MAC_DST_ADDR_ETHER_II |
										IPA_FLT_MAC_DST_ADDR_802_3 |
										IPA_FLT_MAC_DST_ADDR_L2TP |
										IPA_FLT_MAC_DST_ADDR_802_1Q|
										IPA_FLT_L2TP_UDP_INNER_MAC_DST_ADDR);
				}
			}

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].meta_data =
				(attrib->attrib_mask & IPA_FLT_META_DATA) ?
				flt_entry->rule.attrib.meta_data : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.meta_data_mask =
				(attrib->attrib_mask & IPA_FLT_META_DATA) ?
				flt_entry->rule.attrib.meta_data_mask : 0;

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.payload_length =
				(attrib->ext_attrib_mask & IPA_FLT_EXT_MTU) ?
				flt_entry->rule.attrib.payload_length : 0;

			if (attrib->attrib_mask & IPA_FLT_MAC_ETHER_TYPE ||
				attrib->ext_attrib_mask &
				IPA_FLT_EXT_L2TP_UDP_INNER_ETHER_TYPE) {
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
				.ether_type =
					flt_entry->rule.attrib.ether_type;
			}

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].vlan_id =
				(attrib->attrib_mask & IPA_FLT_VLAN_ID) ?
				flt_entry->rule.attrib.vlan_id : 0;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx].tcp_syn =
				(attrib->attrib_mask & IPA_FLT_TCP_SYN) ? 1 : 0;

			if (attrib->attrib_mask & IPA_FLT_TCP_SYN_L2TP ||
				attrib->ext_attrib_mask & IPA_FLT_EXT_L2TP_UDP_TCP_SYN) {
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
				.tcp_syn_l2tp = 1;
			}

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.l2tp_inner_ip_type =
				(attrib->attrib_mask & IPA_FLT_L2TP_INNER_IP_TYPE) ?
				flt_entry->rule.attrib.type : 0;


			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.num_offset_meq128 = flt_entry->rule.eq_attrib.num_offset_meq_128;
			for (r = 0; r < flt_entry->rule.eq_attrib.num_offset_meq_128; r++)
			{
				for (s = 0; s < 16; s ++)
				{
					flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
						.offset_meq_128[r].value[s] = flt_entry->rule.eq_attrib.offset_meq_128[r].value[s];
					flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
						.offset_meq_128[r].mask[s] = flt_entry->rule.eq_attrib.offset_meq_128[r].mask[s];
				}
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
						.offset_meq_128[r].offset = flt_entry->rule.eq_attrib.offset_meq_128[r].offset;
			}

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.num_offset_meq32 = flt_entry->rule.eq_attrib.num_offset_meq_32;
			for (r = 0; r < flt_entry->rule.eq_attrib.num_offset_meq_32; r++)
			{
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.offset_meq32[r].offset = flt_entry->rule.eq_attrib.offset_meq_32[r].offset;
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.offset_meq32[r].value = flt_entry->rule.eq_attrib.offset_meq_32[r].value;
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.offset_meq32[r].mask = flt_entry->rule.eq_attrib.offset_meq_32[r].mask;

			}

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.num_ihl_offset_meq32 = flt_entry->rule.eq_attrib.num_ihl_offset_meq_32;
			for (r = 0; r < flt_entry->rule.eq_attrib.num_ihl_offset_meq_32; r++)
			{
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_meq32[r].offset = flt_entry->rule.eq_attrib.ihl_offset_meq_32[r].offset;
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_meq32[r].value = flt_entry->rule.eq_attrib.ihl_offset_meq_32[r].value;
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_meq32[r].mask = flt_entry->rule.eq_attrib.ihl_offset_meq_32[r].mask;

			}

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.is_metadata_meq32_persent = flt_entry->rule.eq_attrib.metadata_meq32_present;
			for (r = 0; r < flt_entry->rule.eq_attrib.metadata_meq32_present; r++)
			{
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.metadata_meq32[r].offset = flt_entry->rule.eq_attrib.metadata_meq32.offset;
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.metadata_meq32[r].value = flt_entry->rule.eq_attrib.metadata_meq32.value;
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.metadata_meq32[r].mask = flt_entry->rule.eq_attrib.metadata_meq32.mask;
			}

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.num_ihl_offset_range_16 = flt_entry->rule.eq_attrib.num_ihl_offset_range_16;
			for (r = 0; r < flt_entry->rule.eq_attrib.num_ihl_offset_range_16; r++)
			{
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_range16[r].offset = flt_entry->rule.eq_attrib.ihl_offset_range_16[r].offset;
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_range16[r].range_low = flt_entry->rule.eq_attrib.ihl_offset_range_16[r].range_low;
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_range16[r].range_high = flt_entry->rule.eq_attrib.ihl_offset_range_16[r].range_high;
			}

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.num_ihl_offset_eq32 = flt_entry->rule.eq_attrib.ihl_offset_eq_32_present;
			for (r = 0; r < flt_entry->rule.eq_attrib.ihl_offset_eq_32_present; r++)
			{
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_eq32[r].offset = flt_entry->rule.eq_attrib.ihl_offset_eq_32.offset;
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_eq32[r].value = flt_entry->rule.eq_attrib.ihl_offset_eq_32.value;
			}

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.num_ihl_offset_eq16 = flt_entry->rule.eq_attrib.ihl_offset_eq_16_present;
			for (r = 0; r < flt_entry->rule.eq_attrib.ihl_offset_eq_16_present; r++)
			{
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_eq16[r].offset = flt_entry->rule.eq_attrib.ihl_offset_eq_16.offset;
				flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
					.ihl_offset_eq16[r].value = flt_entry->rule.eq_attrib.ihl_offset_eq_16.value;
			}

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.protocol_eq = flt_entry->rule.eq_attrib.protocol_eq;

			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.pure_ack = flt_entry->rule.eq_attrib.tos_eq_present;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.frag_encoding = flt_entry->rule.eq_attrib.is_frag_encoding;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.prio = flt_entry->prio;
			flt_rule_stats->v6_flt_tbl[tbl_idx].flt_rule[rule_idx]
			.max_prio = flt_entry->rule.max_prio;


			rule_idx++;
		}
		if (rule_idx)
		{
			flt_rule_stats->v6_flt_tbl[tbl_idx].num_flt_rule  =  rule_idx;
			tbl_idx++;
		}
	}
	flt_rule_stats->num_v6_flt_table = tbl_idx;

	mutex_unlock(&ipa3_ctx->lock);

	if(copy_to_user((void __user *)arg,
		(u8 *)flt_rule_stats,
		alloc_size)) {
		IPA_STATS_ERR("copy to user failed");
		kfree(flt_rule_stats);
		return -EFAULT;
	}

	kfree(flt_rule_stats);
	return 0;
}

static int ipa_get_v4_nat_rule_stats(unsigned long arg)
{
	struct ipa_lnx_v4_nat_rule_stats *nat_rule_stats;
	int alloc_size;

	alloc_size = sizeof(struct ipa_lnx_v4_nat_rule_stats);

	IPA_STATS_DBG("Alloc size %d for filling v4_nat \n", alloc_size);
	nat_rule_stats = (struct ipa_lnx_v4_nat_rule_stats *) memdup_user((
		const void __user *)arg, alloc_size);
	if (IS_ERR(nat_rule_stats)) {
		IPA_STATS_ERR("copy from user failed");
		return -ENOMEM;
	}

	mutex_lock(&ipa3_ctx->lock);


	mutex_unlock(&ipa3_ctx->lock);

	if(copy_to_user((void __user *)arg,
		(u8 *)nat_rule_stats,
		alloc_size)) {
		IPA_STATS_ERR("copy to user failed");
		kfree(nat_rule_stats);
		return -EFAULT;
	}

	kfree(nat_rule_stats);
	return 0;
}

static int ipa_get_v6_nat_rule_stats(unsigned long arg)
{
	struct ipa_lnx_v6_nat_rule_stats *nat_rule_stats;
	int alloc_size;

	alloc_size = sizeof(struct ipa_lnx_v6_nat_rule_stats);

	IPA_STATS_DBG("Alloc size %d for filling v6_nat \n", alloc_size);
	nat_rule_stats = (struct ipa_lnx_v6_nat_rule_stats *) memdup_user((
		const void __user *)arg, alloc_size);
	if (IS_ERR(nat_rule_stats)) {
		IPA_STATS_ERR("copy from user failed");
		return -ENOMEM;
	}

	mutex_lock(&ipa3_ctx->lock);


	mutex_unlock(&ipa3_ctx->lock);

	if(copy_to_user((void __user *)arg,
		(u8 *)nat_rule_stats,
		alloc_size)) {
		IPA_STATS_ERR("copy to user failed");
		kfree(nat_rule_stats);
		return -EFAULT;
	}

	kfree(nat_rule_stats);
	return 0;
}

static int ipa_stats_get_alloc_info(unsigned long arg)
{
	int i = 0;
	int j, k;
	int holb_drop_stats_num_pipes = 0;
	int ipa_ep_idx_tx, ipa_ep_idx_rx;
	int ipa_client_type;
	int reg_idx;
	int index;
	int eth_instance_id;
	int result;
	struct ipa3_rt_tbl *rt_tbl;
	struct ipa3_rt_entry *rt_entry;
	struct ipa3_rt_tbl_set *rt_set;
	struct ipa3_flt_tbl *flt_tbl;
	struct ipa3_flt_entry *flt_entry;
	int rule_idx, tbl_idx;
	struct ipa_ipfltri_rule_eq *flt_attrib;
	size_t pdn_entry_size;
	char *pdn_entry;

	if (copy_from_user(&ipa_lnx_agent_ctx, u64_to_user_ptr((u64) arg),
		sizeof(struct ipa_lnx_stats_tlpd_ctx))) {
		IPA_STATS_ERR("copy from user failed");
		return -EFAULT;
	}

	/* For generic stats */
	if (ipa_lnx_agent_ctx.log_type_mask &
		TLPD_IPA_LOG_TYPE_GENERIC_STATS) {
		for (i = 0; i < IPA_CLIENT_MAX; i++) {
			int ep_idx = ipa_get_ep_mapping(i);

			if ((ep_idx == -1) || (!IPA_CLIENT_IS_CONS(i)) ||
				(IPA_CLIENT_IS_TEST(i)))
				continue;

			reg_idx = ipahal_get_ep_reg_idx(ep_idx);
			if (!(ipa3_ctx->hw_stats &&
				(ipa3_ctx->hw_stats->drop.init.enabled_bitmask[reg_idx] &
				ipahal_get_ep_bit(ep_idx))))
				continue;

			holb_drop_stats_num_pipes++;
		}
		ipa_lnx_agent_ctx.alloc_info.num_holb_drop_stats_clients =
			holb_drop_stats_num_pipes;
		ipa_lnx_agent_ctx.alloc_info.num_holb_mon_stats_clients =
			ipa3_ctx->uc_ctx.holb_monitor.num_holb_clients;
	}

	/* For clock stats */
	if (ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_CLOCK_STATS)
		ipa_lnx_agent_ctx.alloc_info.num_pm_clients =
			ipa3_get_max_num_pipes();

	/* For WLAN instance */
	if (ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_WLAN_STATS) {
		ipa_ep_idx_tx = ipa_get_ep_mapping(IPA_CLIENT_WLAN2_CONS);
		ipa_ep_idx_rx = ipa_get_ep_mapping(IPA_CLIENT_WLAN2_PROD);
		if ((ipa_ep_idx_tx == -1) || (ipa_ep_idx_rx == -1) ||
			!ipa3_ctx->ep[ipa_ep_idx_tx].valid ||
			!ipa3_ctx->ep[ipa_ep_idx_rx].valid) {
			ipa_lnx_agent_ctx.alloc_info.num_wlan_instances = 0;
		} else {
			ipa_lnx_agent_ctx.alloc_info.num_wlan_instances = 1;
			ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[0].num_pipes = 2;
			ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[0].num_tx_instances = 1;
			ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[0].num_rx_instances = 1;
			ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[0].pipes_client_type[0]
				= IPA_CLIENT_WLAN2_CONS;
			ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[0].pipes_client_type[1]
				= IPA_CLIENT_WLAN2_PROD;
			ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[0].tx_inst_client_type[0]
				= IPA_CLIENT_WLAN2_CONS;
			ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[0].rx_inst_client_type[0]
				= IPA_CLIENT_WLAN2_PROD;
			if(ipa_wdi_is_tx1_used() == 1) {
				ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[0].num_tx_instances++;
				ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[0].num_pipes++;
				ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[
					0].pipes_client_type[2] = IPA_CLIENT_WLAN2_CONS1;
				ipa_lnx_agent_ctx.alloc_info.wlan_inst_info[
					0].tx_inst_client_type[1] = IPA_CLIENT_WLAN2_CONS1;
		}
		}
	}

	/* For ETH instance */
	if (ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_ETH_STATS) {
		ipa_lnx_agent_ctx.alloc_info.num_eth_instances = 0;
		for (i = 0; i < IPA_ETH_INST_ID_MAX; i++) {
			ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].num_pipes = 0;
			ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].num_pipes = 0;
			ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].num_tx_instances
				= 0;
			ipa_lnx_agent_ctx.alloc_info.eth_inst_info[i].num_rx_instances
				= 0;
			k = 0;
			for (j = 0; (j < IPA_ETH_CLIENT_MAX) &&
				(k < TLPD_NUM_MAX_TX_INSTANCES); j++) {
				if (ipa_eth_client_exist(j, i) &&
					(ipa_lnx_agent_ctx.alloc_info.num_eth_instances < 2)) {
					eth_instance_id = ipa_lnx_agent_ctx.alloc_info.num_eth_instances;
					ipa_lnx_agent_ctx.alloc_info.eth_inst_info[eth_instance_id].num_pipes =
						ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
							eth_instance_id].num_pipes + 2;
					ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
						eth_instance_id].num_tx_instances++;
					ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
						eth_instance_id].num_rx_instances++;
					ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
						eth_instance_id].tx_inst_client_type[k] = j;
					ipa_client_type =
						ipa_eth_get_ipa_client_type_from_eth_type(
							j, IPA_ETH_PIPE_DIR_TX);
					if (ipa_client_type >= IPA_CLIENT_MAX)
						IPA_STATS_ERR("Eth tx client type not found");
#if IPA_ETH_API_VER >= 2
					/* Overwrite client type if it is NTN3 and 2nd instance */
					if (((j == IPA_ETH_CLIENT_NTN3) || (j == IPA_ETH_CLIENT_IEMAC)) && (i == 1))
						ipa_client_type = IPA_CLIENT_ETHERNET2_CONS;
#endif
					ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
						eth_instance_id].pipes_client_type[k*2] = ipa_client_type;
					ipa_client_type =
						ipa_eth_get_ipa_client_type_from_eth_type(
							j, IPA_ETH_PIPE_DIR_RX);
					if (ipa_client_type >= IPA_CLIENT_MAX)
						IPA_STATS_ERR("Eth rx client type not found");
#if IPA_ETH_API_VER >= 2
					/* Overwrite client type if it is NTN3 and 2nd instance */
					if (((j == IPA_ETH_CLIENT_NTN3) || (j == IPA_ETH_CLIENT_IEMAC)) && (i == 1))
						ipa_client_type = IPA_CLIENT_ETHERNET2_PROD;
#endif
					ipa_lnx_agent_ctx.alloc_info.eth_inst_info[
						eth_instance_id].pipes_client_type[(k*2) + 1] = ipa_client_type;
					ipa_lnx_agent_ctx.alloc_info.num_eth_instances++;
					k++;
				}
			}
		}
	}

	/* For USB instance */
	if (ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_USB_STATS) {
		ipa_lnx_agent_ctx.alloc_info.num_usb_instances = 0;
		index = 0;
		for (i = 0; (i < IPA_USB_MAX_TETH_PROT_SIZE) &&
			(index < TLPD_NUM_MAX_INSTANCES); i++) {
			if(ipa_usb_is_teth_prot_connected(i)) {
				if (index == TLPD_NUM_MAX_INSTANCES) {
					IPA_STATS_ERR("USB alloc info max size reached\n");
					break;
				}
				ipa_lnx_agent_ctx.usb_teth_prot[index] = i;
				if (ipa_lnx_agent_ctx.usb_teth_prot[index] ==
					IPA_USB_RMNET_CV2X) {
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].num_pipes = 2;
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].num_tx_instances = 1;
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].num_rx_instances = 1;
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].pipes_client_type[0] = IPA_CLIENT_USB2_PROD;
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].pipes_client_type[1] = IPA_CLIENT_USB2_CONS;
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].tx_inst_client_type[0] = IPA_CLIENT_USB2_CONS;
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].rx_inst_client_type[0] = IPA_CLIENT_USB2_PROD;
				} else if (ipa_lnx_agent_ctx.usb_teth_prot[index] ==
					IPA_USB_DIAG) {
					/* USB DIAG stats not supported, can be added in future */
					continue;
				} else {
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].num_pipes = 2;
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].num_tx_instances = 1;
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].num_rx_instances = 1;
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].pipes_client_type[0] = IPA_CLIENT_USB_PROD;
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].pipes_client_type[1] = IPA_CLIENT_USB_CONS;
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].tx_inst_client_type[0] = IPA_CLIENT_USB_CONS;
					ipa_lnx_agent_ctx.alloc_info.usb_inst_info[
						index].rx_inst_client_type[0] = IPA_CLIENT_USB_PROD;
				}
				ipa_lnx_agent_ctx.alloc_info.num_usb_instances++;
				index++;
			}
		}
	}

	/* For MHIP instance */
	if (ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_MHIP_STATS) {
#if IS_ENABLED(CONFIG_IPA3_MHI_PRIME_MANAGER)
		if (!ipa3_ctx->mhip_ctx.dbg_stats.uc_dbg_stats_mmio) {
			ipa_lnx_agent_ctx.alloc_info.num_mhip_instances = 0;
		} else {
			if (ipa_usb_is_teth_prot_connected(IPA_USB_RNDIS))
				ipa_lnx_agent_ctx.usb_teth_prot[0] = IPA_USB_RNDIS;
			else if(ipa_usb_is_teth_prot_connected(IPA_USB_RMNET))
				ipa_lnx_agent_ctx.usb_teth_prot[0] = IPA_USB_RMNET;
			else ipa_lnx_agent_ctx.usb_teth_prot[0] = IPA_USB_MAX_TETH_PROT_SIZE;
			ipa_lnx_agent_ctx.alloc_info.num_mhip_instances = 1;
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[0].num_pipes = 4;
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[0].num_tx_instances = 2;
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[0].num_rx_instances = 2;
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[0].pipes_client_type[0] =
				IPA_CLIENT_MHI_PRIME_TETH_CONS;
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[0].pipes_client_type[1] =
				IPA_CLIENT_MHI_PRIME_TETH_PROD;
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[0].pipes_client_type[2] =
				IPA_CLIENT_MHI_PRIME_RMNET_CONS;
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[0].pipes_client_type[3] =
				IPA_CLIENT_MHI_PRIME_RMNET_PROD;
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[0].tx_inst_client_type[0]
				= IPA_CLIENT_MHI_PRIME_TETH_CONS;
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[0].tx_inst_client_type[1]
				= IPA_CLIENT_MHI_PRIME_RMNET_CONS;
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[0].rx_inst_client_type[0]
				= IPA_CLIENT_MHI_PRIME_TETH_PROD;
			ipa_lnx_agent_ctx.alloc_info.mhip_inst_info[0].rx_inst_client_type[1]
				= IPA_CLIENT_MHI_PRIME_RMNET_PROD;
		}
#else
		/* MHI Prime is not enabled */
		ipa_lnx_agent_ctx.alloc_info.num_mhip_instances = 0;
#endif
	}

	/* For Page recycling stats for default, coal and Low lat pipes */
	if (ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_RECYCLE_STATS)
		ipa_lnx_agent_ctx.alloc_info.num_page_rec_interval =
			IPA_LNX_PIPE_PAGE_RECYCLING_INTERVAL_COUNT;

	/* For v4 RT Rule */
	if (ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_V4_RT_RULE_STATS) {
		/* IPv4 RT structures */
		mutex_lock(&ipa3_ctx->lock);
		rt_set = &ipa3_ctx->rt_tbl_set[IPA_IP_v4];

		tbl_idx = 0;
		list_for_each_entry(rt_tbl, &rt_set->head_rt_tbl_list, link) {
			if (tbl_idx == TLPD_NUM_MAX_RT_FLT_TBL) {
				IPA_STATS_ERR("RT4 table limit reached\n");
				break;
			}

			rule_idx = 0;
			list_for_each_entry(rt_entry, &rt_tbl->head_rt_rule_list, link) {
				if (rule_idx == TLPD_NUM_MAX_RT_FLT_RULE) {
					IPA_STATS_ERR("RT4 Rule limit reached\n");
					break;
				}
				/**
				 * Filling number of attribute eq info 0 for rt rules.
				 * Can be opened up in future for rt_hw_rules
				 */
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v4_offset_meq128s[tbl_idx][rule_idx] = 0;
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v4_offset_meq32s[tbl_idx][rule_idx] = 0;
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v4_ihl_offset_meq32s[tbl_idx][rule_idx] = 0;
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.v4_metadata_meq32_present[tbl_idx][rule_idx] = 0;
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v4_ihl_offset_range_16s[tbl_idx][rule_idx] = 0;
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v4_ihl_offset_eq16s[tbl_idx][rule_idx] = 0;
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v4_ihl_offset_eq32s[tbl_idx][rule_idx] = 0;
				rule_idx++;
			}
			ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v4_rules[tbl_idx] = rule_idx;
			IPA_STATS_DBG("IP v4 rule %d rule %d\n", tbl_idx, rule_idx);
			tbl_idx++;
		}
		ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v4_tables = tbl_idx;
		mutex_unlock(&ipa3_ctx->lock);
	}

	if (ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_V6_RT_RULE_STATS) {
		/* IPv6 RT structures */
		mutex_lock(&ipa3_ctx->lock);
		rt_set = &ipa3_ctx->rt_tbl_set[IPA_IP_v6];

		tbl_idx = 0;
		list_for_each_entry(rt_tbl, &rt_set->head_rt_tbl_list, link) {
			if (tbl_idx == TLPD_NUM_MAX_RT_FLT_TBL) {
				IPA_STATS_ERR("RT6 table limit reached\n");
				break;
			}

			rule_idx = 0;
			list_for_each_entry(rt_entry, &rt_tbl->head_rt_rule_list, link) {
				/**
				 * Filling number of attribute eq info 0 for rt rules.
				 * Can be opened up in future for rt_hw_rules
				 */
				if (rule_idx == TLPD_NUM_MAX_RT_FLT_RULE) {
					IPA_STATS_ERR("RT6 Rule limit reached\n");
					break;
				}
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_offset_meq128s[tbl_idx][rule_idx] = 0;
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_offset_meq32s[tbl_idx][rule_idx] = 0;
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_ihl_offset_meq32s[tbl_idx][rule_idx] = 0;
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.v6_metadata_meq32_present[tbl_idx][rule_idx] = 0;
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_ihl_offset_range_16s[tbl_idx][rule_idx] = 0;
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_ihl_offset_eq16s[tbl_idx][rule_idx] = 0;
				ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_ihl_offset_eq32s[tbl_idx][rule_idx] = 0;
				rule_idx++;
			}
			ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_rules[tbl_idx] = rule_idx;
			IPA_STATS_DBG("IP v6 rule %d rule %d\n", tbl_idx, rule_idx);
			tbl_idx++;
		}
		ipa_lnx_agent_ctx.alloc_info.rt_alloc_info.num_v6_tables = tbl_idx;
		mutex_unlock(&ipa3_ctx->lock);
	}

	/* For FLT Rule */
	if (ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_V4_FLT_RULE_STATS) {
		/* IPv4 structures */
		tbl_idx = 0;
		mutex_lock(&ipa3_ctx->lock);
		for (j = 0; j < ipa3_ctx->ipa_num_pipes; j++) {
			if (tbl_idx == TLPD_NUM_MAX_RT_FLT_TBL) {
				IPA_STATS_ERR("FLT4 table limit reached\n");
				break;
			}

			if (!ipa_is_ep_support_flt(j)) continue;
			flt_tbl = &ipa3_ctx->flt_tbl[j][IPA_IP_v4];
			i = 0;
			rule_idx = 0;
			list_for_each_entry(flt_entry, &flt_tbl->head_flt_rule_list, link) {
				/**
				 * Filling number of attribute eq info for flt rules.
				 * Can be opened up in future for flt_hw_rules as well
				 */
				if (rule_idx == TLPD_NUM_MAX_RT_FLT_RULE) {
					IPA_STATS_ERR("FLT4 Rule limit reached\n");
					break;
				}
				if (flt_entry->rule.eq_attrib_type) {
					flt_attrib = &flt_entry->rule.eq_attrib;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v4_offset_meq128s[tbl_idx][rule_idx] =
						(flt_attrib->num_offset_meq_128 <= IPA_IPFLTR_NUM_MEQ_128_EQNS) ? flt_attrib->num_offset_meq_128 : IPA_IPFLTR_NUM_MEQ_128_EQNS;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v4_offset_meq32s[tbl_idx][rule_idx] =
						(flt_attrib->num_offset_meq_32 <= IPA_IPFLTR_NUM_MEQ_32_EQNS) ? flt_attrib->num_offset_meq_32 : IPA_IPFLTR_NUM_MEQ_32_EQNS;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v4_ihl_offset_meq32s[tbl_idx][rule_idx] =
						(flt_attrib->num_ihl_offset_meq_32 <= IPA_IPFLTR_NUM_IHL_MEQ_32_EQNS) ? flt_attrib->num_ihl_offset_meq_32 : IPA_IPFLTR_NUM_IHL_MEQ_32_EQNS;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.v4_metadata_meq32_present[tbl_idx][rule_idx] = flt_attrib->metadata_meq32_present;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v4_ihl_offset_range_16s[tbl_idx][rule_idx] =
						(flt_attrib->num_ihl_offset_range_16 <= IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS) ? flt_attrib->num_ihl_offset_range_16 : IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v4_ihl_offset_eq16s[tbl_idx][rule_idx] = flt_attrib->ihl_offset_eq_16_present;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v4_ihl_offset_eq32s[tbl_idx][rule_idx] = flt_attrib->ihl_offset_eq_32_present;
				}
				rule_idx++;
			}

			if (rule_idx)
			{
				ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v4_rules[tbl_idx] = rule_idx;
				IPA_STATS_DBG("IP v4 flt save flt table %d rule %d \n", tbl_idx, rule_idx);
				tbl_idx++;
			}

		}
		ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v4_tables = tbl_idx;
		mutex_unlock(&ipa3_ctx->lock);
	}

	if (ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_V6_FLT_RULE_STATS) {
		/* IPv6 structures */
		tbl_idx = 0;
		mutex_lock(&ipa3_ctx->lock);
		for (j = 0; j < ipa3_ctx->ipa_num_pipes; j++) {
			if (tbl_idx == TLPD_NUM_MAX_RT_FLT_TBL) {
				IPA_STATS_ERR("FLT6 table limit reached\n");
				break;
			} else if (!ipa_is_ep_support_flt(j))
				continue;
			flt_tbl = &ipa3_ctx->flt_tbl[j][IPA_IP_v6];
			rule_idx = 0;
			list_for_each_entry(flt_entry, &flt_tbl->head_flt_rule_list, link) {
				/**
				 * Filling number of attribute eq info for flt rules.
				 * Can be opened up in future for flt_hw_rules as well
				 */
				if (rule_idx == TLPD_NUM_MAX_RT_FLT_RULE) {
					IPA_STATS_ERR("FLT6 Rule limit reached\n");
					break;
				}
				if (flt_entry->rule.eq_attrib_type) {
					flt_attrib = &flt_entry->rule.eq_attrib;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v6_offset_meq128s[tbl_idx][rule_idx] =
						(flt_attrib->num_offset_meq_128 <= IPA_IPFLTR_NUM_MEQ_128_EQNS) ? flt_attrib->num_offset_meq_128 : IPA_IPFLTR_NUM_MEQ_128_EQNS;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v6_offset_meq32s[tbl_idx][rule_idx] =
						(flt_attrib->num_offset_meq_32 <= IPA_IPFLTR_NUM_MEQ_32_EQNS) ? flt_attrib->num_offset_meq_32 : IPA_IPFLTR_NUM_MEQ_32_EQNS;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v6_ihl_offset_meq32s[tbl_idx][rule_idx] =
						(flt_attrib->num_ihl_offset_meq_32 <= IPA_IPFLTR_NUM_IHL_MEQ_32_EQNS) ? flt_attrib->num_ihl_offset_meq_32 : IPA_IPFLTR_NUM_IHL_MEQ_32_EQNS;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.v6_metadata_meq32_present[tbl_idx][rule_idx] = flt_attrib->metadata_meq32_present;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v6_ihl_offset_range_16s[tbl_idx][rule_idx] =
						(flt_attrib->num_ihl_offset_range_16 <= IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS) ? flt_attrib->num_ihl_offset_range_16 : IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v6_ihl_offset_eq16s[tbl_idx][rule_idx] = flt_attrib->ihl_offset_eq_16_present;
					ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v6_ihl_offset_eq32s[tbl_idx][rule_idx] = flt_attrib->ihl_offset_eq_32_present;
				}
				rule_idx++;
			}
			if (rule_idx)
			{
				ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v4_rules[tbl_idx] = rule_idx;
				IPA_STATS_DBG("IP v6 flt save flt table %d rule %d \n", tbl_idx, rule_idx);
				tbl_idx++;
			}

		}
		ipa_lnx_agent_ctx.alloc_info.flt_alloc_info.num_v6_tables = tbl_idx;
		mutex_unlock(&ipa3_ctx->lock);
	}

	/* For NAT Rule instance */
	if (ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_V4_NAT_RULE_STATS) {
		/* For PDN table */
		if (ipa3_ctx->nat_mem.pdn_mem.base) {
			result = ipahal_nat_entry_size(
				IPAHAL_NAT_IPV4_PDN, &pdn_entry_size);
			if (result) {
				IPAERR("Failed to retrieve size of PDN entry");
				return -EFAULT;
			}
			for (i = 0, pdn_entry = ipa3_ctx->nat_mem.pdn_mem.base;
				i < ipa3_get_max_pdn();
				++i, pdn_entry += pdn_entry_size)
				ipa_lnx_agent_ctx.alloc_info.num_pdn_tbl++;
		}

		/* For NAT table */
		ipa_lnx_agent_ctx.alloc_info.num_nat_tbl = 0;
		ipa_lnx_agent_ctx.alloc_info.num_nat_idx_tbl = 0;
	}

	if (ipa_lnx_agent_ctx.log_type_mask & TLPD_IPA_LOG_TYPE_V6_NAT_RULE_STATS) {
		/* For PDN table */
		if (ipa3_ctx->nat_mem.pdn_mem.base) {
			result = ipahal_nat_entry_size(
				IPAHAL_NAT_IPV4_PDN, &pdn_entry_size);
			if (result) {
				IPAERR("Failed to retrieve size of PDN entry");
				return -EFAULT;
			}
			for (i = 0, pdn_entry = ipa3_ctx->nat_mem.pdn_mem.base;
				i < ipa3_get_max_pdn();
				++i, pdn_entry += pdn_entry_size)
				ipa_lnx_agent_ctx.alloc_info.num_pdn_tbl++;
		}

		/* For NAT table */
		ipa_lnx_agent_ctx.alloc_info.num_nat_tbl = 0;
		ipa_lnx_agent_ctx.alloc_info.num_nat_idx_tbl = 0;
	}

	if(copy_to_user((u8 *)arg,
		&ipa_lnx_agent_ctx,
		sizeof(struct ipa_lnx_stats_tlpd_ctx))) {
		IPA_STATS_ERR("copy to user failed");
		return -EFAULT;
	}
	return 0;
}

static long ipa_lnx_stats_ioctl(struct file *filp,
	unsigned int cmd,
	unsigned long arg)
{
	int retval = IPA_LNX_STATS_SUCCESS;
	struct ipa_lnx_consolidated_stats *consolidated_stats;

	if (_IOC_TYPE(cmd) != IPA_LNX_STATS_IOC_MAGIC) {
		IPA_STATS_ERR("IOC type mismatch %d\n", cmd);
		return -ENOTTY;
	}

	if(!ipa3_ctx) {
		IPA_STATS_ERR("IPA driver is not up, rejecting the ioctl\n");
		return -EPERM;
	}

	IPA_STATS_DBG("ipa get alloc info 0x%lx 0x%x", IPA_LNX_IOC_GET_ALLOC_INFO, cmd);

	mutex_lock(&ipa_lnx_ctx_mutex);
	switch (cmd) {
	case IPA_LNX_IOC_GET_ALLOC_INFO:
		retval = ipa_stats_get_alloc_info(arg);
		if (retval)
			IPA_STATS_ERR("ipa get alloc info fail");
		break;
	case IPA_LNX_IOC_GET_GENERIC_STATS:
		retval = ipa_get_generic_stats(arg);
		if (retval)
			IPA_STATS_ERR("ipa get generic stats fail");
		break;
	case IPA_LNX_IOC_GET_CLOCK_STATS:
		retval = ipa_get_clock_stats(arg);
		if (retval)
			IPA_STATS_ERR("ipa get clock stats fail");
		break;
	case IPA_LNX_IOC_GET_WLAN_INST_STATS:
		retval = ipa_get_wlan_inst_stats(arg);
		if (retval)
			IPA_STATS_ERR("ipa get wlan inst stats fail");
		break;
	case IPA_LNX_IOC_GET_ETH_INST_STATS:
		retval = ipa_get_eth_inst_stats(arg);
		if (retval)
			IPA_STATS_ERR("ipa get eth inst stats fail");
		break;
	case IPA_LNX_IOC_GET_USB_INST_STATS:
		retval = ipa_get_usb_inst_stats(arg);
		if (retval)
			IPA_STATS_ERR("ipa get usb inst stats fail");
		break;
	case IPA_LNX_IOC_GET_MHIP_INST_STATS:
#if IS_ENABLED(CONFIG_IPA3_MHI_PRIME_MANAGER)
		retval = ipa_get_mhip_inst_stats(arg);
		if (retval)
			IPA_STATS_ERR("ipa get mhip inst stats fail");
#else
		retval = IPA_LNX_STATS_SUCCESS;
#endif
		break;
	case IPA_LNX_IOC_GET_CONSOLIDATED_STATS:
		consolidated_stats = (struct ipa_lnx_consolidated_stats *) memdup_user((
				const void __user *)arg, sizeof(struct ipa_lnx_consolidated_stats));
		if (IS_ERR(consolidated_stats)) {
			IPA_STATS_ERR("copy from user failed");
			return -ENOMEM;
		}

		if (consolidated_stats->log_type_mask & TLPD_IPA_LOG_TYPE_GENERIC_STATS) {
			retval = ipa_get_generic_stats((unsigned long) consolidated_stats->generic_stats);
			if (retval) {
				IPA_STATS_ERR("ipa get generic stats fail");
				break;
			}
		}
		if (consolidated_stats->log_type_mask & TLPD_IPA_LOG_TYPE_CLOCK_STATS) {
			retval = ipa_get_clock_stats((unsigned long) consolidated_stats->clock_stats);
			if (retval) {
				IPA_STATS_ERR("ipa get clock stats fail");
				break;
			}
		}
		if (consolidated_stats->log_type_mask & TLPD_IPA_LOG_TYPE_WLAN_STATS) {
			retval = ipa_get_wlan_inst_stats((unsigned long) consolidated_stats->wlan_stats);
			if (retval) {
				IPA_STATS_ERR("ipa get wlan inst stats fail");
				break;
			}
		}
		if (consolidated_stats->log_type_mask & TLPD_IPA_LOG_TYPE_ETH_STATS) {
			retval = ipa_get_eth_inst_stats((unsigned long) consolidated_stats->eth_stats);
			if (retval) {
				IPA_STATS_ERR("ipa get eth inst stats fail");
				break;
			}
		}
		if (consolidated_stats->log_type_mask & TLPD_IPA_LOG_TYPE_USB_STATS) {
			retval = ipa_get_usb_inst_stats((unsigned long) consolidated_stats->usb_stats);
			if (retval) {
				IPA_STATS_ERR("ipa get usb inst stats fail");
				break;
			}
		}
		if (consolidated_stats->log_type_mask & TLPD_IPA_LOG_TYPE_MHIP_STATS) {
#if IS_ENABLED(CONFIG_IPA3_MHI_PRIME_MANAGER)
			retval = ipa_get_mhip_inst_stats((unsigned long) consolidated_stats->mhip_stats);
			if (retval) {
				IPA_STATS_ERR("ipa get mhip inst stats fail");
				break;
			}
#endif
		}
		if (consolidated_stats->log_type_mask & TLPD_IPA_LOG_TYPE_RECYCLE_STATS) {
			retval = ipa_get_page_recycle_stats((unsigned long) consolidated_stats->recycle_stats);
			if (retval) {
				IPA_STATS_ERR("ipa get page recycle stats fail\n");
				break;
			}
		}
		if (consolidated_stats->log_type_mask & TLPD_IPA_LOG_TYPE_V4_RT_RULE_STATS) {
			IPA_STATS_DBG("ipa get v4 rt rule stats called\n");
			retval = ipa_get_v4_rt_rule_stats((unsigned long) consolidated_stats->v4_rt_rule_stats);
			if (retval) {
				IPA_STATS_ERR("ipa get v4 rt rule stats fail\n");
				break;
			}
			IPA_STATS_DBG("ipa get v4 rt rule stats passed\n");
		}
		if (consolidated_stats->log_type_mask & TLPD_IPA_LOG_TYPE_V6_RT_RULE_STATS) {
			IPA_STATS_DBG("ipa get v6 rt rule stats called\n");
			retval = ipa_get_v6_rt_rule_stats((unsigned long) consolidated_stats->v6_rt_rule_stats);
			if (retval) {
				IPA_STATS_ERR("ipa get v6 rt rule stats fail\n");
				break;
			}
			IPA_STATS_DBG("ipa get v6 rt rule stats passed\n");
		}
		if (consolidated_stats->log_type_mask & TLPD_IPA_LOG_TYPE_V4_FLT_RULE_STATS) {
			IPA_STATS_DBG("ipa get v4 flt rule stats called\n");
			retval = ipa_get_v4_flt_rule_stats((unsigned long) consolidated_stats->v4_flt_rule_stats);
			if (retval) {
				IPA_STATS_ERR("ipa get flt rule stats fail\n");
				break;
			}
			IPA_STATS_DBG("ipa get v4 flt rule stats passed\n");
		}
		if (consolidated_stats->log_type_mask & TLPD_IPA_LOG_TYPE_V6_FLT_RULE_STATS) {
			IPA_STATS_DBG("ipa get v6 flt rule stats called\n");
			retval = ipa_get_v6_flt_rule_stats((unsigned long) consolidated_stats->v6_flt_rule_stats);
			if (retval) {
				IPA_STATS_ERR("ipa get flt rule stats fail\n");
				break;
			}
			IPA_STATS_DBG("ipa get v6 flt rule stats passed\n");
		}
		if (consolidated_stats->log_type_mask & TLPD_IPA_LOG_TYPE_V4_NAT_RULE_STATS) {
			retval = ipa_get_v4_nat_rule_stats((unsigned long) consolidated_stats->v4_nat_rule_stats);
			if (retval) {
				IPA_STATS_ERR("ipa get nat rule stats fail\n");
				break;
			}
		}
		if (consolidated_stats->log_type_mask & TLPD_IPA_LOG_TYPE_V6_NAT_RULE_STATS) {
			retval = ipa_get_v6_nat_rule_stats((unsigned long) consolidated_stats->v6_nat_rule_stats);
			if (retval) {
				IPA_STATS_ERR("ipa get nat rule stats fail\n");
				break;
			}
		}
		break;
	default:
		retval = -ENOTTY;
	}

	IPA_STATS_ERR("Exit .. return %d\n", retval);
	mutex_unlock(&ipa_lnx_ctx_mutex);
	return retval;
}

#ifdef CONFIG_COMPAT

long compat_ipa_lnx_stats_ioctl(struct file *filp,
	unsigned int cmd,
	unsigned long arg)
{
	IPADBG("compat_ipa_lnx_stats_ioctl cmd=%x nr=%d\n", cmd, _IOC_NR(cmd));

	if (_IOC_TYPE(cmd) != IPA_LNX_STATS_IOC_MAGIC) {
		IPA_STATS_ERR("IOC type mismatch %d\n", cmd);
		return -ENOTTY;
	}
	if(!ipa3_ctx) {
		IPA_STATS_ERR("IPA driver is not up, rejecting the ioctl\n");
		return -EPERM;
	}
	switch (_IOC_NR(cmd)) {
	case IPA_LNX_CMD_GET_ALLOC_INFO:
		if(_IOC_DIR(cmd) != _IOC_DIR(IPA_LNX_IOC_GET_ALLOC_INFO))
			return -ENOTTY;
		cmd = IPA_LNX_IOC_GET_ALLOC_INFO;
		break;
	case IPA_LNX_CMD_GENERIC_STATS:
		if(_IOC_DIR(cmd) != _IOC_DIR(IPA_LNX_IOC_GET_GENERIC_STATS))
			return -ENOTTY;
		cmd = IPA_LNX_IOC_GET_GENERIC_STATS;
		break;
	case IPA_LNX_CMD_CLOCK_STATS:
		if(_IOC_DIR(cmd) != _IOC_DIR(IPA_LNX_IOC_GET_CLOCK_STATS))
			return -ENOTTY;
		cmd = IPA_LNX_IOC_GET_CLOCK_STATS;
		break;
	case IPA_LNX_CMD_WLAN_INST_STATS:
		if(_IOC_DIR(cmd) != _IOC_DIR(IPA_LNX_IOC_GET_WLAN_INST_STATS))
			return -ENOTTY;
		cmd = IPA_LNX_IOC_GET_WLAN_INST_STATS;
		break;
	case IPA_LNX_CMD_ETH_INST_STATS:
		if(_IOC_DIR(cmd) != _IOC_DIR(IPA_LNX_IOC_GET_ETH_INST_STATS))
			return -ENOTTY;
		cmd = IPA_LNX_IOC_GET_ETH_INST_STATS;
		break;
	case IPA_LNX_CMD_USB_INST_STATS:
		if(_IOC_DIR(cmd) != _IOC_DIR(IPA_LNX_IOC_GET_USB_INST_STATS))
			return -ENOTTY;
		cmd = IPA_LNX_IOC_GET_USB_INST_STATS;
		break;
	case IPA_LNX_CMD_MHIP_INST_STATS:
		if(_IOC_DIR(cmd) != _IOC_DIR(IPA_LNX_IOC_GET_MHIP_INST_STATS))
			return -ENOTTY;
		cmd = IPA_LNX_IOC_GET_MHIP_INST_STATS;
		break;
	case IPA_LNX_CMD_CONSOLIDATED_STATS:
		if(_IOC_DIR(cmd) != _IOC_DIR(IPA_LNX_IOC_GET_CONSOLIDATED_STATS))
			return -ENOTTY;
		cmd = IPA_LNX_IOC_GET_CONSOLIDATED_STATS;
		break;
	default:
		return -ENOIOCTLCMD;
	}
	return ipa_lnx_stats_ioctl(filp, cmd, (unsigned long) compat_ptr(arg));
}

#endif

const struct file_operations ipa_stats_fops = {
	.owner = THIS_MODULE,
	.open = ipa_stats_ioctl_open,
	.read = NULL,
	.unlocked_ioctl = ipa_lnx_stats_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = compat_ipa_lnx_stats_ioctl,
#endif
};

static int ipa_tlpd_stats_ioctl_init(void)
{
	unsigned int ipa_lnx_stats_ioctl_major = 0;
	int ret;
	struct device *dev;

	device = MKDEV(ipa_lnx_stats_ioctl_major, 0);

	ret = alloc_chrdev_region(&device, 0, dev_num, DRIVER_NAME);
	if (ret) {
		IPA_STATS_ERR(":device_alloc err.\n");
		goto dev_alloc_err;
	}
	ipa_lnx_stats_ioctl_major = MAJOR(device);

	class = class_create(DRIVER_NAME);

	if (IS_ERR(class)) {
		IPA_STATS_ERR(":class_create err.\n");
		goto class_err;
	}

	dev = device_create(class, NULL, device,
			NULL, DRIVER_NAME);
	if (IS_ERR(dev)) {
		IPA_STATS_ERR(":device_create err.\n");
		goto device_err;
	}

	cdev_init(&ipa_lnx_stats_ioctl_cdev, &ipa_stats_fops);
	ret = cdev_add(&ipa_lnx_stats_ioctl_cdev, device, dev_num);
	if (ret) {
		IPA_STATS_ERR(":cdev_add err.\n");
		goto cdev_add_err;
	}

	IPA_STATS_ERR("IPA %s major(%d) initial ok :>>>>\n",
		DRIVER_NAME, ipa_lnx_stats_ioctl_major);
	return 0;

cdev_add_err:
	device_destroy(class, device);
device_err:
	class_destroy(class);
class_err:
	unregister_chrdev_region(device, dev_num);
dev_alloc_err:
	return -ENODEV;
}

int ipa_tlpd_stats_init(void)
{
	int ret;

	ret = ipa_tlpd_stats_ioctl_init();
	if(ret) {
		IPA_STATS_ERR("IPA_LNX_STATS_IOCTL init failure = %d\n", ret);
		return -1;
	}
	memset(&poll_pack_and_cred_info, 0, sizeof(poll_pack_and_cred_info));
	IPA_STATS_ERR("IPA_LNX_STATS_IOCTL init success\n");

	return 0;
}

/* Non periodic/Event based stats update */
int ipa3_update_usb_per_stats(enum ipa_per_stats_type_e stats_type, uint32_t data) {
	union ipa_peripheral_stats *peripheral_stats =
		(union ipa_peripheral_stats *) ipa3_ctx->per_stats_smem_va;
	if (ipa3_ctx->platform_type == IPA_PLAT_TYPE_MDM) {
		peripheral_stats->mdm.usb_enum_value = IPA_PER_USB_ENUM_TYPE_INVALID;
		peripheral_stats->mdm.usb_prot_enum_value = IPA_PER_USB_PROT_TYPE_INVALID;
		peripheral_stats->mdm.usb_max_speed_val = 0;
		peripheral_stats->mdm.usb_pipo_val = 0;
	} else if (ipa3_ctx->platform_type == IPA_PLAT_TYPE_MSM) {
		peripheral_stats->msm.usb_enum_value = IPA_PER_USB_ENUM_TYPE_INVALID;
		peripheral_stats->msm.usb_prot_enum_value = IPA_PER_USB_PROT_TYPE_INVALID;
		peripheral_stats->msm.usb_max_speed_val = 0;
		peripheral_stats->msm.usb_pipo_val = 0;
	}
	return 0;
}

int ipa3_update_pcie_per_stats(enum ipa_per_stats_type_e stats_type, uint32_t data) {
	union ipa_peripheral_stats *peripheral_stats =
		(union ipa_peripheral_stats *) ipa3_ctx->per_stats_smem_va;
	if (ipa3_ctx->platform_type == IPA_PLAT_TYPE_MDM) {
		peripheral_stats->mdm.pcie_gen_type_val = 0;
		peripheral_stats->mdm.pcie_width_type_val = PCIE_LINK_WIDTH_DEF;
		peripheral_stats->mdm.pcie_max_speed_val = 0;
		peripheral_stats->mdm.pcie_num_lpm_trans_d3 = 0;
		peripheral_stats->mdm.pcie_num_lpm_trans_m1 = 0;
		peripheral_stats->mdm.pcie_num_lpm_trans_m2 = 0;
		peripheral_stats->mdm.pcie_num_lpm_trans_m0 = 0;
	}
	return 0;
}

int ipa3_update_wifi_per_stats(enum ipa_per_stats_type_e stats_type, uint32_t data) {
	union ipa_peripheral_stats *peripheral_stats =
		(union ipa_peripheral_stats *) ipa3_ctx->per_stats_smem_va;
	if (ipa3_ctx->platform_type == IPA_PLAT_TYPE_MDM) {
		peripheral_stats->mdm.wifi_enum_type_val = IPA_PER_WIFI_ENUM_TYPE_INVALID;
		peripheral_stats->mdm.wifi_max_speed_val = 0;
		peripheral_stats->mdm.wifi_dual_band_enabled_val = 0;
	} else if (ipa3_ctx->platform_type == IPA_PLAT_TYPE_MSM) {
		peripheral_stats->msm.wifi_enum_type_val = IPA_PER_WIFI_ENUM_TYPE_INVALID;
		peripheral_stats->msm.wifi_max_speed_val = 0;
		peripheral_stats->msm.wifi_dual_band_enabled_val = 0;
	}
	return 0;
}

int ipa3_update_eth_per_stats(enum ipa_per_stats_type_e stats_type, uint32_t data) {
	union ipa_peripheral_stats *peripheral_stats =
		(union ipa_peripheral_stats *) ipa3_ctx->per_stats_smem_va;
	if (ipa3_ctx->platform_type == IPA_PLAT_TYPE_MDM) {
		peripheral_stats->mdm.eth_client_val = 0;
		peripheral_stats->mdm.eth_max_speed_val = 0;
	}
	return 0;
}

int ipa3_update_apps_per_stats(enum ipa_per_stats_type_e stats_type, uint32_t data) {
	union ipa_peripheral_stats *peripheral_stats =
		(union ipa_peripheral_stats *) ipa3_ctx->per_stats_smem_va;
	if (ipa3_ctx->platform_type == IPA_PLAT_TYPE_MDM) {
		peripheral_stats->mdm.periph_val = 0;
		peripheral_stats->mdm.periph_wwan_val = 0;
		peripheral_stats->mdm.periph_type_val = IPA_PER_TYPE_BITMASK_NONE;
	} else if (ipa3_ctx->platform_type == IPA_PLAT_TYPE_MSM) {
		peripheral_stats->msm.periph_val = 0;
		peripheral_stats->msm.periph_wwan_val = 0;
		peripheral_stats->msm.periph_type_val = IPA_PER_TYPE_BITMASK_NONE;
	}
	return 0;
}

/* Periodic stats update */
int ipa3_update_client_holb_per_stats(enum ipa_per_stats_type_e stats_type, uint32_t data) {
	union ipa_peripheral_stats *peripheral_stats =
		(union ipa_peripheral_stats *) ipa3_ctx->per_stats_smem_va;
	if (ipa3_ctx->platform_type == IPA_PLAT_TYPE_MDM) {
		peripheral_stats->mdm.wifi_holb_uc_stats_num_periph_bad = 0;
		peripheral_stats->mdm.wifi_holb_uc_stats_num_periph_recovered = 0;

		peripheral_stats->mdm.eth_holb_uc_stats_num_periph_bad = 0;
		peripheral_stats->mdm.eth_holb_uc_stats_num_periph_recovered = 0;

		peripheral_stats->mdm.usb_holb_uc_stats_num_periph_bad = 0;
		peripheral_stats->mdm.usb_holb_uc_stats_num_periph_recovered = 0;
	} else if (ipa3_ctx->platform_type == IPA_PLAT_TYPE_MSM) {
		peripheral_stats->msm.wifi_holb_uc_stats_num_periph_bad = 0;
		peripheral_stats->msm.wifi_holb_uc_stats_num_periph_recovered = 0;

		peripheral_stats->msm.usb_holb_uc_stats_num_periph_bad = 0;
		peripheral_stats->msm.usb_holb_uc_stats_num_periph_recovered = 0;
	}
	return 0;
}

int ipa3_update_dma_per_stats(enum ipa_per_stats_type_e stats_type, uint32_t data) {
	union ipa_peripheral_stats *peripheral_stats =
		(union ipa_peripheral_stats *) ipa3_ctx->per_stats_smem_va;
	peripheral_stats->mdm.ipa_dma_bytes_val = 0;
	return 0;
}

int ipa3_peripheral_stats_init(union ipa_peripheral_stats *peripheral_stats) {

	if (ipa3_ctx->platform_type == IPA_PLAT_TYPE_MDM) {
		peripheral_stats->mdm.num_entries = IPA_PERIPHERAL_STATS_MDM_NUM_ENTRIES;

		/* TLV for number of peripherals connected to APROC */
		/* value = IPA_PER_STATS_TYPE_NUM_PERS */
		peripheral_stats->mdm.periph_id = IPA_PER_STATS_TYPE_NUM_PERS;
		peripheral_stats->mdm.periph_len = 4;
		peripheral_stats->mdm.periph_val = 0;

		/* TLV for number of periphers from/to traffic flowing from modem */
		/* value = IPA_PER_STATS_TYPE_NUM_PERS_WWAN */
		peripheral_stats->mdm.periph_wwan_id = IPA_PER_STATS_TYPE_NUM_PERS_WWAN;
		peripheral_stats->mdm.periph_wwan_len = 4;
		peripheral_stats->mdm.periph_wwan_val = 0;

		/* TLV for bitmask for active/connected peripherals */
		/* value = IPA_PER_STATS_TYPE_ACT_PER_TYPE */
		peripheral_stats->mdm.periph_type_id = IPA_PER_STATS_TYPE_ACT_PER_TYPE;
		peripheral_stats->mdm.periph_type_len = 4;
		peripheral_stats->mdm.periph_type_val = IPA_PER_TYPE_BITMASK_NONE;

		/* TLV for Current gen info if PCIe interconnect is valid */
		/* value = IPA_PER_STATS_TYPE_PCIE_GEN */
		peripheral_stats->mdm.pcie_gen_type_id = IPA_PER_STATS_TYPE_PCIE_GEN;
		peripheral_stats->mdm.pcie_gen_type_len = 4;
		peripheral_stats->mdm.pcie_gen_type_val = 0;

		/* TLV for Current gen info if PCIe interconnect is valid */
		/* value = IPA_PER_STATS_TYPE_PCIE_GEN */
		peripheral_stats->mdm.pcie_width_type_id = IPA_PER_STATS_TYPE_PCIE_WIDTH;
		peripheral_stats->mdm.pcie_width_type_len = 4;
		peripheral_stats->mdm.pcie_width_type_val = PCIE_LINK_WIDTH_DEF;

		/* TLV for Max PCIe speed in current gen in Mbps */
		/* value = IPA_PER_STATS_TYPE_PCIE_MAX_SPEED */
		peripheral_stats->mdm.pcie_max_speed_id = IPA_PER_STATS_TYPE_PCIE_MAX_SPEED;
		peripheral_stats->mdm.pcie_max_speed_len = 4;
		peripheral_stats->mdm.pcie_max_speed_val = 0;

		/* TLV for number PCIe LPM transitions */
		/* value = IPA_PER_STATS_TYPE_PCIE_NUM_LPM */
		peripheral_stats->mdm.pcie_num_lpm_trans_id = IPA_PER_STATS_TYPE_PCIE_NUM_LPM;
		peripheral_stats->mdm.pcie_num_lpm_trans_len = 8;
		peripheral_stats->mdm.pcie_num_lpm_trans_d3 = 0;
		peripheral_stats->mdm.pcie_num_lpm_trans_m1 = 0;
		peripheral_stats->mdm.pcie_num_lpm_trans_m2 = 0;
		peripheral_stats->mdm.pcie_num_lpm_trans_m0 = 0;

		/* TLV for USB enumeration type */
		/* value = IPA_PER_STATS_TYPE_USB_TYPE */
		peripheral_stats->mdm.usb_enum_id = IPA_PER_STATS_TYPE_USB_TYPE;
		peripheral_stats->mdm.usb_enum_len = 4;
		peripheral_stats->mdm.usb_enum_value = IPA_PER_USB_ENUM_TYPE_INVALID;

		/* TLV for Current USB protocol enumeration if active */
		/* value = IPA_PER_STATS_TYPE_USB_PROT */
		peripheral_stats->mdm.usb_prot_enum_id = IPA_PER_STATS_TYPE_USB_PROT;
		peripheral_stats->mdm.usb_prot_enum_len = 4;
		peripheral_stats->mdm.usb_prot_enum_value = IPA_PER_USB_PROT_TYPE_INVALID;

		/* TLV for Max USB speed in current gen in Mbps */
		/* value = IPA_PER_STATS_TYPE_USB_MAX_SPEED */
		peripheral_stats->mdm.usb_max_speed_id = IPA_PER_STATS_TYPE_USB_MAX_SPEED;
		peripheral_stats->mdm.usb_max_speed_len = 4;
		peripheral_stats->mdm.usb_max_speed_val = 0;

		/* TLV for Total number of USB plug in/outs, count is only plug ins */
		/* value = IPA_PER_STATS_TYPE_USB_PIPO */
		peripheral_stats->mdm.usb_pipo_id = IPA_PER_STATS_TYPE_USB_PIPO;
		peripheral_stats->mdm.usb_pipo_len = 4;
		peripheral_stats->mdm.usb_pipo_val = 0;

		/* TLV for Wifi enumeration type*/
		/* value = IPA_PER_STATS_TYPE_WIFI_ENUM_TYPE */
		peripheral_stats->mdm.wifi_enum_type_id = IPA_PER_STATS_TYPE_WIFI_ENUM_TYPE;
		peripheral_stats->mdm.wifi_enum_type_len = 4;
		peripheral_stats->mdm.wifi_enum_type_val = IPA_PER_WIFI_ENUM_TYPE_INVALID;

		/* TLV for Theoritical Max WLAN speed in current gen in Mbps (pipe for 5GHz in case of dual band) */
		/* value = IPA_PER_STATS_TYPE_WIFI_MAX_SPEED */
		peripheral_stats->mdm.wifi_max_speed_id = IPA_PER_STATS_TYPE_WIFI_MAX_SPEED;
		peripheral_stats->mdm.wifi_max_speed_len = 4;
		peripheral_stats->mdm.wifi_max_speed_val = 0;

		/* TLV for Theoretical Max WLAN speed on the 2.4GHz pipe, value of 0 means disabled */
		/* value = IPA_PER_STATS_TYPE_WIFI_DUAL_BAND_EN */
		peripheral_stats->mdm.wifi_dual_band_enabled_id = IPA_PER_STATS_TYPE_WIFI_DUAL_BAND_EN;
		peripheral_stats->mdm.wifi_dual_band_enabled_len = 4;
		peripheral_stats->mdm.wifi_dual_band_enabled_val = 0;

		/* TLV for the type of ethernet client - Realtek/AQC */
		/* value = IPA_PER_STATS_TYPE_ETH_CLIENT */
		peripheral_stats->mdm.eth_client_id = IPA_PER_STATS_TYPE_ETH_CLIENT;
		peripheral_stats->mdm.eth_client_len = 4;
		peripheral_stats->mdm.eth_client_val = 0;

		/* TLV for Max Eth link speed */
		/* value = IPA_PER_STATS_TYPE_ETH_MAX_SPEED */
		peripheral_stats->mdm.eth_max_speed_id = IPA_PER_STATS_TYPE_ETH_MAX_SPEED;
		peripheral_stats->mdm.eth_max_speed_len = 4;
		peripheral_stats->mdm.eth_max_speed_val = 0;

		/* TLV for Total number of bytes txferred through IPA DMA channels over PCIe */
		/* For cases where GSI used for QDSS direct DMA, need to extract bytes stat from GSI FW */
		/* value = IPA_PER_STATS_TYPE_IPA_DMA_BYTES */
		peripheral_stats->mdm.ipa_dma_bytes_id = IPA_PER_STATS_TYPE_IPA_DMA_BYTES;
		peripheral_stats->mdm.ipa_dma_bytes_len = 4;
		peripheral_stats->mdm.ipa_dma_bytes_val = 0;

		/* TLV for number of wifi peripherals connected to APROC */
		/* value = IPA_PER_STATS_TYPE_WIFI_HOLB_UC */
		peripheral_stats->mdm.wifi_holb_uc_stats_id = IPA_PER_STATS_TYPE_WIFI_HOLB_UC;
		peripheral_stats->mdm.wifi_holb_uc_stats_len = 4;
		peripheral_stats->mdm.wifi_holb_uc_stats_num_periph_bad = 0;
		peripheral_stats->mdm.wifi_holb_uc_stats_num_periph_recovered = 0;

		/* TLV for number of eth peripherals connected to APROC */
		/* value = IPA_PER_STATS_TYPE_ETH_HOLB_UC */
		peripheral_stats->mdm.eth_holb_uc_stats_id = IPA_PER_STATS_TYPE_ETH_HOLB_UC;
		peripheral_stats->mdm.eth_holb_uc_stats_len = 4;
		peripheral_stats->mdm.eth_holb_uc_stats_num_periph_bad = 0;
		peripheral_stats->mdm.eth_holb_uc_stats_num_periph_recovered = 0;

		/* TLV for number of usb peripherals connected to APROC */
		/* value = IPA_PER_STATS_TYPE_USB_HOLB_UC */
		peripheral_stats->mdm.usb_holb_uc_stats_id = IPA_PER_STATS_TYPE_USB_HOLB_UC;
		peripheral_stats->mdm.usb_holb_uc_stats_len = 4;
		peripheral_stats->mdm.usb_holb_uc_stats_num_periph_bad = 0;
		peripheral_stats->mdm.usb_holb_uc_stats_num_periph_recovered = 0;

	} else if (ipa3_ctx->platform_type == IPA_PLAT_TYPE_MSM) {
		peripheral_stats->msm.num_entries = IPA_PERIPHERAL_STATS_MSM_NUM_ENTRIES;

		/* TLV for number of peripherals connected to APROC */
		/* value = IPA_PER_STATS_TYPE_NUM_PERS */
		peripheral_stats->msm.periph_id = IPA_PER_STATS_TYPE_NUM_PERS;
		peripheral_stats->msm.periph_len = 4;
		peripheral_stats->msm.periph_val = 0;

		/* TLV for number of periphers from/to traffic flowing from modem */
		/* value = IPA_PER_STATS_TYPE_NUM_PERS_WWAN */
		peripheral_stats->msm.periph_wwan_id = IPA_PER_STATS_TYPE_NUM_PERS_WWAN;
		peripheral_stats->msm.periph_wwan_len = 4;
		peripheral_stats->msm.periph_wwan_val = 0;

		/* TLV for bitmask for active/connected peripherals */
		/* value = IPA_PER_STATS_TYPE_ACT_PER_TYPE */
		peripheral_stats->msm.periph_type_id = IPA_PER_STATS_TYPE_ACT_PER_TYPE;
		peripheral_stats->msm.periph_type_len = 4;
		peripheral_stats->msm.periph_type_val = IPA_PER_TYPE_BITMASK_NONE;

		/* TLV for USB enumeration type */
		/* value = IPA_PER_STATS_TYPE_USB_TYPE */
		peripheral_stats->msm.usb_enum_id = IPA_PER_STATS_TYPE_USB_TYPE;
		peripheral_stats->msm.usb_enum_len = 4;
		peripheral_stats->msm.usb_enum_value = IPA_PER_USB_ENUM_TYPE_INVALID;

		/* TLV for Current USB protocol enumeration if active */
		/* value = IPA_PER_STATS_TYPE_USB_PROT */
		peripheral_stats->msm.usb_prot_enum_id = IPA_PER_STATS_TYPE_USB_PROT;
		peripheral_stats->msm.usb_prot_enum_len = 4;
		peripheral_stats->msm.usb_prot_enum_value = IPA_PER_USB_PROT_TYPE_INVALID;

		/* TLV for Max USB speed in current gen in Mbps */
		/* value = IPA_PER_STATS_TYPE_USB_MAX_SPEED */
		peripheral_stats->msm.usb_max_speed_id = IPA_PER_STATS_TYPE_USB_MAX_SPEED;
		peripheral_stats->msm.usb_max_speed_len = 4;
		peripheral_stats->msm.usb_max_speed_val = 0;

		/* TLV for Total number of USB plug in/outs, count is only plug ins */
		/* value = IPA_PER_STATS_TYPE_USB_PIPO */
		peripheral_stats->msm.usb_pipo_id = IPA_PER_STATS_TYPE_USB_PIPO;
		peripheral_stats->msm.usb_pipo_len = 4;
		peripheral_stats->msm.usb_pipo_val = 0;

		/* TLV for Wifi enumeration type*/
		/* value = IPA_PER_STATS_TYPE_WIFI_ENUM_TYPE */
		peripheral_stats->msm.wifi_enum_type_id = IPA_PER_STATS_TYPE_WIFI_ENUM_TYPE;
		peripheral_stats->msm.wifi_enum_type_len = 4;
		peripheral_stats->msm.wifi_enum_type_val = IPA_PER_WIFI_ENUM_TYPE_INVALID;

		/* TLV for Theoritical Max WLAN speed in current gen in Mbps (pipe for 5GHz in case of dual band) */
		/* value = IPA_PER_STATS_TYPE_WIFI_MAX_SPEED */
		peripheral_stats->msm.wifi_max_speed_id = IPA_PER_STATS_TYPE_WIFI_MAX_SPEED;
		peripheral_stats->msm.wifi_max_speed_len = 4;
		peripheral_stats->msm.wifi_max_speed_val = 0;

		/* TLV for Theoretical Max WLAN speed on the 2.4GHz pipe, value of 0 means disabled */
		/* value = IPA_PER_STATS_TYPE_WIFI_DUAL_BAND_EN */
		peripheral_stats->msm.wifi_dual_band_enabled_id = IPA_PER_STATS_TYPE_WIFI_DUAL_BAND_EN;
		peripheral_stats->msm.wifi_dual_band_enabled_len = 4;
		peripheral_stats->msm.wifi_dual_band_enabled_val = 0;

		/* TLV for number of wifi peripherals connected to APROC */
		/* value = IPA_PER_STATS_TYPE_WIFI_HOLB_UC */
		peripheral_stats->msm.wifi_holb_uc_stats_id = IPA_PER_STATS_TYPE_WIFI_HOLB_UC;
		peripheral_stats->msm.wifi_holb_uc_stats_len = 4;
		peripheral_stats->msm.wifi_holb_uc_stats_num_periph_bad = 0;
		peripheral_stats->msm.wifi_holb_uc_stats_num_periph_recovered = 0;

		/* TLV for number of usb peripherals connected to APROC */
		/* value = IPA_PER_STATS_TYPE_USB_HOLB_UC */
		peripheral_stats->msm.usb_holb_uc_stats_id = IPA_PER_STATS_TYPE_USB_HOLB_UC;
		peripheral_stats->msm.usb_holb_uc_stats_len = 4;
		peripheral_stats->msm.usb_holb_uc_stats_num_periph_bad = 0;
		peripheral_stats->msm.usb_holb_uc_stats_num_periph_recovered = 0;
	}

	return 0;
}
