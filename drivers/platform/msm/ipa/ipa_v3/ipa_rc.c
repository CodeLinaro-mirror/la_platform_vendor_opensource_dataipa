// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */
#include <linux/workqueue.h>
#include <linux/jiffies.h>
#include <linux/ctype.h>
#include <linux/etherdevice.h>
#include "ipahal_nat.h"
#include "gsi.h"
#include "ipa_i.h"

#define READ_BIT(val, n) (((val) >> (n)) & 0x1)
#define IPA_HM_MAX_RETRYCNT 10
#define STA_RULE_INCORRECT BIT(0)
#define AP_RULE_INCORRECT BIT(1)
#define IPA_BAD_STATE -3
#define MAX_NUM_CONS_CLIENT (IPA_CLIENT_MAX/2)

struct ipa_rc_queue rc_list;
struct ipa_rc_wlan_info ipa_rc_wlan_info;
unsigned int query_timer = 200; // is 200 milliseconds;

/* Global context pointer */
struct ipa_rc_wq_ctx *rc_ctx;
static bool has_ul_dl_rule, modem_rule;

static struct chan_param_monitor chan_info[MAX_NUM_CONS_CLIENT][2];

/* Common Queue Implementations */
void rc_list_init(struct ipa_rc_queue *q)
{
	if (!q) {
		IPAERR("rc list not inited\n");
		return;
	}
	INIT_LIST_HEAD(&q->head);
	q->size = 0;
	spin_lock_init(&q->lock);
}

bool rc_list_is_empty(struct ipa_rc_queue *q)
{
	unsigned long flags;
	bool empty;

	if (!q) {
		IPAERR("rc list not inited\n");
		return true;
	}
	spin_lock_irqsave(&q->lock, flags);
	empty = (q->size == 0);
	spin_unlock_irqrestore(&q->lock, flags);
	return empty;
}

bool rc_list_is_full(struct ipa_rc_queue *q)
{
	unsigned long flags;
	bool full;

	if (!q) {
		IPAERR("rc list not inited\n");
		return false;
	}
	spin_lock_irqsave(&q->lock, flags);
	full = (q->size >= LIST_MAX_LEN_DEBUG);
	spin_unlock_irqrestore(&q->lock, flags);
	return full;
}

size_t rc_list_size(struct ipa_rc_queue *q)
{
	unsigned long flags;
	size_t s;

	if (!q) {
		IPAERR("rc list not inited\n");
		return 0;
	}
	spin_lock_irqsave(&q->lock, flags);
	s = q->size;
	spin_unlock_irqrestore(&q->lock, flags);
	return s;
}

int rc_list_enqueue(struct ipa_rc_queue *q, struct ipa_rc_health_monitor *p)
{
	unsigned long flags;

	if (!q) {
		IPAERR("rc list not inited\n");
		return -EINVAL;
	}

	INIT_LIST_HEAD(&p->node);

	spin_lock_irqsave(&q->lock, flags);
	if (q->size >= LIST_MAX_LEN_DEBUG) {
		IPAERR("rc list full\n");
		spin_unlock_irqrestore(&q->lock, flags);
		return -ENOMEM;
	}
	list_add(&p->node, &q->head);
	q->size++;
	spin_unlock_irqrestore(&q->lock, flags);

	return 0;
}

void rc_list_dequeue(struct ipa_rc_queue *q)
{
	struct ipa_rc_health_monitor *p = NULL;
	unsigned long flags;

	if (!q) {
		IPAERR("rc list not inited\n");
		return;
	}

	spin_lock_irqsave(&q->lock, flags);
	if (q->size == 0) {
		IPAERR("rc list empty\n");
		spin_unlock_irqrestore(&q->lock, flags);
		return;
	}

	p = list_last_entry(&q->head, struct ipa_rc_health_monitor, node);
	list_del(&p->node);
	if(p)
	kfree(p);
	q->size--;
	spin_unlock_irqrestore(&q->lock, flags);

	return;
}

void rc_list_clear(struct ipa_rc_queue *q)
{
	struct ipa_rc_health_monitor *p, *tmp;
	unsigned long flags;

	if (!q) {
		IPAERR("rc list not inited\n");
		return;
	}

	spin_lock_irqsave(&q->lock, flags);
	list_for_each_entry_safe(p, tmp, &q->head, node) {
		list_del_init(&p->node);
		q->size--;
		kfree(p);
	}
	spin_unlock_irqrestore(&q->lock, flags);
}

/* IPA HM helper functions */
bool is_wlan_sta_pkt(struct ipahal_pkt_status *status)
{
	enum ipa_client_type clnt;
	struct ipa_rc_wlan_intf_info *it;

	clnt = ipa3_get_client_by_pipe(status->endp_src_idx);
	if(!IPA_CLIENT_IS_WLAN_PROD(clnt))
		return false;

	mutex_lock(&rc_ctx->rc_lock);
	list_for_each_entry(it, &ipa_rc_wlan_info.head, link) {
		if(it->metadata == ntohl(status->metadata)) {
			mutex_unlock(&rc_ctx->rc_lock);
			return it->wlan_msg_type == WLAN_STA_CONNECT;
		}
	}
	mutex_unlock(&rc_ctx->rc_lock);
	return false;
}

int get_rc_client(int src_idx)
{
	enum ipa_client_type client;

	client = ipa3_get_client_by_pipe(src_idx);

	if(IPA_CLIENT_IS_ETH_PROD(src_idx))
		return ETH;
	else if(IPA_CLIENT_IS_WLAN_PROD(client))
		return WLAN;
	else if(IPA_CLIENT_IS_Q6_PROD(client))
		return MODEM;
	else
		return OTHERS;
}

bool is_frg_rule(struct ipa_rule_attrib *attrib)
{
	return (attrib->attrib_mask & IPA_FLT_FRAGMENT);
}

bool is_tcp_syn_rule(struct ipa_rule_attrib *attrib)
{
	if(attrib->attrib_mask & IPA_FLT_PROTOCOL) {
		return (attrib->attrib_mask & IPA_FLT_TCP_SYN) ||
			(attrib->u.v4.protocol == 0x01); /*ICMP prot*/
	}
	return false;
}

static bool is_dst_multi_or_broadcast_host(struct ipa_rule_attrib *attrib, enum ipa_ip_type ip)
{
	/* Only IPv4 supported currently */
	if (ip != IPA_IP_v4) {
		return false;
	}

	const uint32_t dst_addr = attrib->u.v4.dst_addr;
	const uint32_t dst_mask = attrib->u.v4.dst_addr_mask;

	/* 224.0.0.0 = 0xE0000000, 240.0.0.0 = 0xF0000000 */
	/* 255.255.255.255 = 0xFFFFFFFF */
	const bool is_multicast_block =
		(dst_addr == 0xE0000000u) && (dst_mask == 0xF0000000u);

	const bool is_limited_broadcast =
		(dst_addr == 0xFFFFFFFFu) && (dst_mask == 0xFFFFFFFFu);

	return is_multicast_block || is_limited_broadcast;
}

bool has_default_rt_tbl(u32 rt_tbl_idx)
{
	struct ipa3_rt_tbl *tbl = NULL;
	struct ipa3_rt_entry *entry = NULL;
	struct ipa3_rt_tbl_set *set = NULL;

	set = &ipa3_ctx->rt_tbl_set[IPA_IP_v4];

	if(list_empty(&set->head_rt_tbl_list)) {
		IPAERR("Rt table list is empty\n");
		return false;
	}

	list_for_each_entry(tbl, &set->head_rt_tbl_list, link) {
		if(!list_empty(&tbl->head_rt_rule_list)) {
			list_for_each_entry(entry, &tbl->head_rt_rule_list, link) {
				if(entry->tbl->idx == rt_tbl_idx) {
					return (strcmp(entry->tbl->name, "ipa_dflt_rt") == 0);
				}
			}
		}
	}
	return false;
}

static bool is_dst_pvt_subnet(struct ipa3_flt_entry *entry)
{
	u32 rt_tbl_idx;
	struct ipa3_rt_tbl *rt_tbl = NULL;

	rt_tbl = ipa3_id_find(entry->rule.rt_tbl_hdl);
	if (rt_tbl == NULL || rt_tbl->cookie != IPA_RT_TBL_COOKIE)
		rt_tbl_idx =  ~0;
	else
		rt_tbl_idx = rt_tbl->idx;

	/*attrib_mask has been checked as IPA_FLT_DST_ADDR before calling this api*/
	return (entry->rule.action == IPA_PASS_TO_ROUTING
		&& has_default_rt_tbl(rt_tbl_idx));
}

static bool is_lan2lan_rule(struct ipa_rule_attrib *attrib)
{
	return ((attrib->attrib_mask & IPA_FLT_MAC_DST_ADDR_ETHER_II) ||
		(attrib->attrib_mask & IPA_FLT_MAC_DST_ADDR_802_3) ||
		(attrib->attrib_mask & IPA_FLT_MAC_DST_ADDR_L2TP) ||
		(attrib->attrib_mask & IPA_FLT_MAC_DST_ADDR_802_1Q) ||
		(attrib->attrib_mask & IPA_FLT_L2TP_UDP_INNER_MAC_DST_ADDR));
}

/* IPA HM functions */
void ipa_rc_query_drop_stats(struct ipa_rc_health_monitor *ipa_state_info)
{
	int i;
	enum ipa_rc_state_err cur_status = IPA_HEALTH_OK;
	struct ipa_rc_health_monitor *prev_ele = NULL;
	static u32 (*drop_pkt_cnts)[DATA_DIR];
	int prev_ul_drop = 0, prev_dl_drop = 0;
	int ul_drop = 0, dl_drop = 0;
	bool ul_drop_inc, dl_drop_inc;

	drop_pkt_cnts = ipa_state_info->drop_pkt_cnts;

	if (!list_empty(&rc_list.head))
		prev_ele = list_first_entry(&rc_list.head, struct ipa_rc_health_monitor, node);

	for(i=0; i < MAX_RC_CLIENTS; i++) {
		drop_pkt_cnts[i][0] =
				ipa3_ctx->stats.rx_excp_pkts[i][IPAHAL_PKT_STATUS_EXCEPTION_DROP_UL];
		drop_pkt_cnts[i][1] =
				ipa3_ctx->stats.rx_excp_pkts[i][IPAHAL_PKT_STATUS_EXCEPTION_DROP_DL];

		if(ipa3_ctx->is_rc_log_enabled) {
			IPADBG("drop stats for rc_client: %d\n", i);
			IPADBG("drop stats UL: %u DL stats: %u\n",
					drop_pkt_cnts[i][0],
					drop_pkt_cnts[i][1]);
		}

		if(!prev_ele)
			continue;

		ul_drop = drop_pkt_cnts[i][0];
		dl_drop = drop_pkt_cnts[i][1];
		prev_ul_drop = prev_ele->drop_pkt_cnts[i][0];
		prev_dl_drop = prev_ele->drop_pkt_cnts[i][1];

		ul_drop_inc = ul_drop > prev_ul_drop;
		dl_drop_inc = dl_drop > prev_dl_drop;

		if(!ul_drop_inc && !dl_drop_inc)
			continue;

		if(i == WLAN) {
			if(ul_drop_inc)
				cur_status |= IPA_DRIVER_WLAN_AP_PKT_DROP;

			if(dl_drop_inc)
				cur_status |= IPA_DRIVER_WLAN_STA_PKT_DROP;
		}
		else if(i == ETH) {
			cur_status |= IPA_DRIVER_ETH_PKT_DROP;
		}
	}

	ipa_state_info->status_code |= cur_status;
	return;
}

void ipa_rc_query_chan(enum ipa_client_type clnt, int chan, struct chan_param_monitor* chan_params)
{
	int ee;

	ee = ipa3_get_ee_by_client(clnt);
	if (ee < 0) {
		IPAERR("Failed to get EE for client %d\n", clnt);
		return;
	}
	chan_params->client = clnt;
	chan_params->ch_id = chan;

	gsi_ctx->per.vote_clk_cb();
	gsi_dump_ch_info(chan, ee, chan_params);
	gsi_ctx->per.unvote_clk_cb();

	return;
}

enum ipa_rc_state_err ipa_rc_detect_chan_stall(enum ipa_client_type client,
			struct chan_param_monitor* chan_params)
{
	uint32_t rp, wp;
	uint32_t prev_rp, prev_wp;
	enum ipa_rc_state_err cur_status = IPA_HEALTH_OK;
	int cons_num;

	if(!chan_params) {
		IPAERR("chan param NULL\n");
		return cur_status;
	}

	if(ipa3_ctx->is_rc_log_enabled) {
		IPADBG("ch_state %d :\n", chan_params->ch_state);
		IPADBG("ch_id %u :\n", chan_params->ch_id);
		IPADBG("ring_base 0x%x :\n", chan_params->ring_base);
		IPADBG("rp_ptr 0x%x :\n", chan_params->rp_ptr);
		IPADBG("wp_ptr 0x%x :\n", chan_params->wp_ptr);
	}

	cons_num = (client - 1)>>1;
	chan_info[cons_num][PREV] =  chan_info[cons_num][CUR];
	chan_info[cons_num][CUR] = *chan_params;

	if(chan_params->ch_state != GSI_CHAN_STATE_STARTED) {
		if(IPA_CLIENT_IS_ETH_CONS(client)) {
			cur_status |= IPA_CHANNEL_ETH_NOT_STARTED;
		}
		else if(IPA_CLIENT_IS_WLAN_CONS(client)){
			cur_status |= IPA_CHANNEL_WLAN_NOT_STARTED;
		}
		return cur_status;
	}

	rp = chan_info[cons_num][CUR].rp_ptr;
	wp = chan_info[cons_num][CUR].wp_ptr;
	prev_rp = chan_info[cons_num][PREV].rp_ptr;
	prev_wp = chan_info[cons_num][PREV].wp_ptr;

	if(rp == wp && prev_rp == prev_wp && rp == prev_rp) {
		if(IPA_CLIENT_IS_ETH_CONS(client))
			cur_status |= IPA_CHANNEL_ETH_NO_BUFF;
		else if(IPA_CLIENT_IS_WLAN_CONS(client))
			cur_status |= IPA_CHANNEL_WLAN_NO_BUFF;
		else if(IPA_CLIENT_IS_APPS_CONS(client))
			cur_status |= IPA_CHANNEL_APPS_EMB_NO_BUFF;
		else if(IPA_CLIENT_IS_Q6_CONS(client))
			cur_status |= IPA_CHANNEL_Q6_NO_BUFF;
		else
			IPADBG("Unsupported client\n");
	}

	return cur_status;
}

void ipa_rc_query_detect_chan_n(struct ipa_rc_health_monitor *ipa_state_info)
{
	int chan, i, num_pipe;
	enum ipa_client_type client;
	enum ipa_rc_state_err status = IPA_HEALTH_OK;;
	struct chan_param_monitor chan_params;

	num_pipe = ipa3_get_max_num_pipes();

	for(i = 0; i < num_pipe; i++) {
		client = ipa3_get_client_by_pipe(i);

		if(IPA_CLIENT_IS_ETH_CONS(client)) {
			if(ipa3_find_chan_by_intf(client) != 0) {
				if(ipa3_ctx->is_rc_log_enabled) {
					IPADBG("eth ep %d not configured\n", i);
				}
				continue;
			}
		}
		else if(!IPA_CLIENT_IS_Q6_CONS(client) && !ipa3_ctx->ep[i].valid) {
			if(ipa3_ctx->is_rc_log_enabled) {
				IPADBG("ep %d not configured\n", i);
			}
			continue;
		}

		if(IPA_CLIENT_IS_ETH_CONS(client) ||
			IPA_CLIENT_IS_WLAN_CONS(client) ||
			IPA_CLIENT_IS_APPS_CONS(client) ||
			IPA_CLIENT_IS_Q6_CONS(client))
		{
			chan = ipa3_get_chan_by_client(client);
			if(ipa3_ctx->is_rc_log_enabled) {
				IPADBG("for client : %s :\n", ipa_clients_strings[client]);
			}

			memset(&chan_params, 0, sizeof(chan_params));
			ipa_rc_query_chan(client, chan, &chan_params);
			status |= ipa_rc_detect_chan_stall(client, &chan_params);
		}
	}
	ipa_state_info->status_code |= status;

	return;
}

void ipa_rc_query_detect_chan_reg(struct ipa_rc_health_monitor *ipa_state_info)
{
	if(!ipa_state_info)
		return;

	ipa_rc_query_detect_chan_n(ipa_state_info);
	return;
}

int get_group_id(struct ipa3_flt_entry *entry, enum ipa_ip_type ip, int pipe_num)
{
	bool eq;
	u32 attrib_mask;
	int res = MAX_FLT_RULE_GRP;
	struct ipa_rule_attrib *attrib;
	struct ipa_ipfltri_rule_eq *eq_attrib;
	enum ipa_client_type client;

	if(!entry) {
		IPAERR("NULL entryl\n");
		return -EINVAL;
	}

	eq = (entry->rule.eq_attrib_type) ? true : false;
	client = ipa3_get_client_by_pipe(pipe_num);

	if(entry->rule_id >= ipahal_get_rule_id_hi_bit()) {
		modem_rule = true;
		return MODEM_FLT_RULE;
	}

	if(entry->rule.action == IPA_PASS_TO_SRC_NAT ||
		entry->rule.action == IPA_PASS_TO_DST_NAT)
		has_ul_dl_rule = true;

	if(!eq) {
		attrib = &entry->rule.attrib;
		attrib_mask = entry->rule.attrib.attrib_mask;

		/* for frg, icmp, tcp-syn etc rules */
		if((is_frg_rule(attrib) ||
			is_tcp_syn_rule(attrib)) &&
			entry->rule.action == IPA_PASS_TO_EXCEPTION) {
			return DEFAULT_FLT_RULE;
		}

		if(attrib->attrib_mask & IPA_FLT_DST_ADDR) {
			if(is_dst_multi_or_broadcast_host(attrib, ip) &&
				entry->rule.action == IPA_PASS_TO_EXCEPTION)
				return DEFAULT_FLT_RULE;

			if(is_dst_pvt_subnet(entry))
				return PVT_SUBNET_FLT_RULE;

			if(attrib->u.v4.dst_addr == 0x0u &&
				attrib->u.v4.dst_addr_mask == 0x0u) {
					if(IPA_CLIENT_IS_ETH_PROD(pipe_num)) {
						if((attrib->attrib_mask & IPA_FLT_VLAN_ID) && (entry->rule.action == IPA_PASS_TO_SRC_NAT))
							return ETH_UL_FLT_RULE;
					}
					else if(IPA_CLIENT_IS_WLAN_PROD(client)) {
						if(entry->rule.action == IPA_PASS_TO_SRC_NAT)
							return WLAN_AP_UL_FLT_RULE;
						else if((entry->rule.action == IPA_PASS_TO_DST_NAT))
							return WLAN_STA_DL_FLT_RULE;
					}
			}
		}

		if(is_lan2lan_rule(attrib)) {
			if(is_valid_ether_addr(attrib->dst_mac_addr) &&
				(entry->rule.action == IPA_PASS_TO_ROUTING)) {
					if(IPA_CLIENT_IS_ETH_PROD(pipe_num) &&
						(attrib->attrib_mask & IPA_FLT_VLAN_ID))
						return LAN2LAN_FLT_RULE;

					if(IPA_CLIENT_IS_WLAN_PROD(client) &&
						!(attrib->attrib_mask & IPA_FLT_VLAN_ID))
						return LAN2LAN_FLT_RULE;
				}
		}
	}
	else {
		eq_attrib = &entry->rule.eq_attrib;
		attrib_mask = entry->rule.eq_attrib.rule_eq_bitmap;

		if(eq_attrib->num_ihl_offset_range_16 >= 1 &&
			entry->rule.action == IPA_PASS_TO_EXCEPTION)
			return MTU_FLT_RULE;
	}

	if(attrib_mask == 0x0u && entry->rule.action == IPA_PASS_TO_EXCEPTION)
		return CATCHALL_DFLT_FLT_RULE;

	return res;
}

int is_flt_rule_ordered(int pipe_num, struct ipa3_flt_tbl *tbl,
						enum ipa_ip_type ip, uint32_t* metadata, int *has_sta_rule)
{
	int grp_id, last = -1;
	int i=0;
	struct ipa3_flt_entry *entry = NULL;

	list_for_each_entry(entry, &tbl->head_flt_rule_list, link) {
		if (entry->cookie != IPA_FLT_COOKIE)
			continue;

		i++;
		if(metadata && *metadata != entry->rule.attrib.meta_data)
			continue;

		grp_id = get_group_id(entry, ip, pipe_num);
		if(ipa3_ctx->is_rc_log_enabled) {
			IPADBG("Rule grp id:%d, idx:%d\n", grp_id, i);
		}

		if(grp_id == WLAN_STA_DL_FLT_RULE)
			*has_sta_rule = 1;

		/* To make sure, default rule always stay on top and
		then follow respective order */
		if((last == -1 && grp_id != DEFAULT_FLT_RULE) ||
			(grp_id < 0 || grp_id == MAX_FLT_RULE_GRP))
			return IPA_BAD_STATE;

		if (grp_id < last)
			return IPA_BAD_STATE;
		if (grp_id >= last)
			last = grp_id;
	}
	return 0;
}

/* To check if Pdn0 has nat entry present */
bool is_nat_present(void)
{
	int result;
	bool entry_zeroed, entry_valid;
	char *pdn_entry;

	if(ipa3_ctx->nat_mem.pdn_mem.base) {
		pdn_entry = ipa3_ctx->nat_mem.pdn_mem.base;

		result = ipahal_nat_is_entry_zeroed(
						IPAHAL_NAT_IPV4_PDN,
						pdn_entry, &entry_zeroed);
		if (result) {
			IPAERR("ipahal_nat_is_entry_zeroed() fail\n");
			goto last;
		}

		if(!entry_zeroed) {
			result = ipahal_nat_is_entry_valid(
						IPAHAL_NAT_IPV4_PDN,
						pdn_entry, &entry_valid);
			if (result) {
				IPAERR("Failed to determine whether the PDN entry is valid\n");
				goto last;
			}
			return entry_valid;
		}
	}
last:
	return false;
}

int is_wlan_flt_rule_ordered(int pipe_num, struct ipa3_flt_tbl *tbl, enum ipa_ip_type ip)
{
	int res = 0, ret;
	struct ipa_rc_wlan_intf_info *it;
	int has_sta_rule = 0;
	struct ipa3_flt_entry *entry = NULL;

	mutex_lock(&rc_ctx->rc_lock);
	list_for_each_entry(it, &ipa_rc_wlan_info.head, link) {
		list_for_each_entry(entry, &tbl->head_flt_rule_list, link) {
			if(it->metadata == entry->rule.attrib.meta_data) {
				ret = is_flt_rule_ordered(pipe_num, tbl, ip, &it->metadata, &has_sta_rule);
				if( ret < 0) {
					if(it->wlan_msg_type == WLAN_STA_CONNECT)
						res |= STA_RULE_INCORRECT;
					else if(it->wlan_msg_type == WLAN_AP_CONNECT)
						res |= AP_RULE_INCORRECT;
				}
				break;
			}
		}
	}
	mutex_unlock(&rc_ctx->rc_lock);

	if(!has_sta_rule && is_nat_present()) {
		IPAERR_RL("has_sta_rule: %d\n", has_sta_rule);
		res |= STA_RULE_INCORRECT;
	}

	return res;
}

void ipa_rc_detect_flt_order(struct ipa_rc_health_monitor *ipa_state_info, enum ipa_ip_type ip)
{
	int i, res;
	struct ipa3_flt_tbl *tbl = NULL;
	enum ipa_rc_state_err status = IPA_HEALTH_OK;
	enum ipa_client_type client;

	mutex_lock(&ipa3_ctx->lock);

	status = ipa_state_info->status_code;

	for (i = 0; i < ipa3_ctx->ipa_num_pipes; i++) {
		if (!ipa_is_ep_support_flt(i))
			continue;

		tbl = &ipa3_ctx->flt_tbl[i][ip];
		if(!list_empty(&tbl->head_flt_rule_list)) {
			client = ipa3_get_client_by_pipe(i);
			if(ipa3_ctx->is_rc_log_enabled) {
				IPADBG("client: %s\n", ipa_clients_strings[client]);
			}

			if(IPA_CLIENT_IS_WLAN_PROD(client)) {
				res = is_wlan_flt_rule_ordered(i, tbl, ip);
				if(res & STA_RULE_INCORRECT)
					status |= IPA_WLAN_STA_FILTER_RULE_INCORRECT;
				if(res & AP_RULE_INCORRECT)
					status |= IPA_WLAN_AP_FILTER_RULE_INCORRECT;
			}
			else if(IPA_CLIENT_IS_ETH_PROD(i)) {
				res = is_flt_rule_ordered(i, tbl, ip, NULL, NULL);
				if(res < 0)
					status |= IPA_ETH_FILTER_RULE_INCORRECT;
			}

			if(ipa3_ctx->is_rc_log_enabled) {
				IPADBG("cur status: %d :\n", status);
			}
		}
	}

	ipa_state_info->status_code |= status;
	mutex_unlock(&ipa3_ctx->lock);

	return;
}

void ipa_rc_nat_init(struct ipa_rc_health_monitor *ipa_state_info)
{
	enum ipa_rc_state_err cur_status = IPA_HEALTH_OK;

	if(!has_ul_dl_rule && !modem_rule)
		return;

	struct ipa3_nat_ipv6ct_common_mem *ndev = &ipa3_ctx->nat_mem.dev;
	struct ipa3_nat_mem *nm_ptr = (struct ipa3_nat_mem *) ndev;
	bool any_table_active = (nm_ptr->ddr_in_use || nm_ptr->sram_in_use);

	if (!ndev->is_dev_init || !ndev->is_hw_init || !any_table_active) {
            if(ipa3_ctx->is_rc_log_enabled)
            {
		IPAERR_RL("NAT hasn't been initialized\n");
            }
		cur_status = IPA_NAT_NOT_INITIALIZED;
	}

	ipa_state_info->status_code |= cur_status;
	return;
}

int ipa_rc_query_detect_for_instance(void)
{
	int ret, retries = 0;
	struct ipa_rc_health_monitor *cur_state=NULL;
	enum ipa_rc_state_err *status_code = NULL;

	cur_state = kmalloc(sizeof(*cur_state), GFP_KERNEL);
	if (!cur_state) {
		IPAERR("kmalloc failed\n");
		return -ENOMEM;
	}

	memset(cur_state, 0, offsetof(struct ipa_rc_health_monitor, node));
	has_ul_dl_rule = false;
	modem_rule = false;

	status_code = &cur_state->status_code;

	ipa_rc_query_detect_chan_reg(cur_state);

	ipa_rc_query_drop_stats(cur_state);

	ipa_rc_detect_flt_order(cur_state, IPA_IP_v4);

	ipa_rc_nat_init(cur_state);

	do {
		if(rc_list_size(&rc_list) >= LIST_MAX_LEN_DEBUG) {
			if(ipa3_ctx->is_rc_log_enabled)
				IPADBG("ipa HM list full, making room\n");
			rc_list_dequeue(&rc_list);
		}

		ret = rc_list_enqueue(&rc_list, cur_state);
		if(!ret)
			goto last;

	} while(++retries < IPA_HM_MAX_RETRYCNT);

	if(retries >= IPA_HM_MAX_RETRYCNT) {
		IPAERR("failed to enqueue cur entry\n");
		kfree(cur_state);
		cur_state = NULL;
		return -ENOMEM;
	}

last:
	return *status_code;
}

int ipa_rc_monitor_health(void)
{
	int i = 0;
	struct ipa_rc_health_monitor *entry = NULL;
	enum ipa_rc_state_err health_status = 0;

	list_for_each_entry(entry, &rc_list.head, node) {
		if(health_status == 0)
			health_status = 0x1FFF;
		health_status &= entry->status_code;
		i++;
		if(i>=5)
			break;
	}
	return health_status;
}

/* IPA-RC Workqueue Handler */
static void ipa_rc_work_handler(struct work_struct *work)
{
	struct ipa_rc_wq_ctx *lcl_ctx;
	uint32_t res = 0;

	/* Retrieve the context from the work structure */
	lcl_ctx = container_of(work, struct ipa_rc_wq_ctx, dwork.work);

	res = ipa_rc_query_detect_for_instance();
	if(res != 0) {
		IPAERR_RL("instance ipa status code: %u\n", res);
	}

	/* Reschedule work to run again with default 200ms configurable timer */
	if (lcl_ctx->rc_wq) {
		mod_delayed_work(lcl_ctx->rc_wq, &lcl_ctx->dwork,
				msecs_to_jiffies(query_timer));
	}
}

static DEVICE_ATTR_RO(status);
static DEVICE_ATTR_RW(testcase);
static DEVICE_ATTR_WO(timer_val);

static struct attribute *hm_attrs[] = {
	&dev_attr_status.attr,
	&dev_attr_testcase.attr,
	&dev_attr_timer_val.attr,
	NULL
};

const struct attribute_group hm_attr_group = {
	.name		= "health_monitor",
	.attrs		= hm_attrs,
};

static struct kobject *ipa_kobj = NULL;

int ipa_rc_init(void)
{
	int ret = -1;

	/* Create sysfs kobj /sys/kernel/ipa */
	ipa_kobj = kobject_create_and_add("ipa", kernel_kobj);
	if (!ipa_kobj) {
		printk(KERN_ERR "Failed to create kobject 'ipa'\n");
		return -ENOMEM;
	}

	/* Create sysfs file in /sys/kernel/ipa/health_monitor */
	ret = sysfs_create_group(ipa_kobj, &hm_attr_group);
	if (ret != 0) {
		IPAERR("Fail to create health_monitor syfs attribute\n");
		return ret;
	}

	rc_ctx = kzalloc(sizeof(struct ipa_rc_wq_ctx), GFP_KERNEL);
	if (!rc_ctx) {
		IPAERR("failed to create rc work ctx\n");
		return -ENOMEM;
	}

	rc_ctx->rc_wq = alloc_workqueue("ipa_rc_wq", WQ_UNBOUND | WQ_MEM_RECLAIM, 1);
	if (!rc_ctx->rc_wq) {
		IPAERR("Fail to allocate rc WQ\n");
		kfree(rc_ctx);
		rc_ctx = NULL;
		return -ENOMEM;
	}

	memset(chan_info, 0, sizeof(chan_info));
	INIT_DELAYED_WORK(&rc_ctx->dwork, ipa_rc_work_handler);
	rc_list_init(&rc_list);
	mutex_init(&rc_ctx->rc_lock);
	INIT_LIST_HEAD(&ipa_rc_wlan_info.head);
	ipa_rc_wlan_info.size = 0;

	IPADBG("ipa rc init complete\n");
	return ret;
}

void ipa_rc_deinit()
{
	struct ipa_rc_wlan_intf_info *entry, *tmp;

	rc_list_clear(&rc_list);

	mutex_lock(&rc_ctx->rc_lock);
	list_for_each_entry_safe(entry, tmp, &ipa_rc_wlan_info.head, link) {
		list_del_init(&entry->link);
		ipa_rc_wlan_info.size--;
		kfree(entry);
	}
	mutex_unlock(&rc_ctx->rc_lock);

	if(ipa_kobj) {
		sysfs_remove_group(ipa_kobj, &hm_attr_group);
		kobject_put(ipa_kobj);
	}

	if (rc_ctx) {
		cancel_delayed_work_sync(&rc_ctx->dwork);
		if (rc_ctx->rc_wq) {
			IPAERR("destroying rc wq\n");
			destroy_workqueue(rc_ctx->rc_wq);
		}
		kfree(rc_ctx);
		rc_ctx = NULL;
	}

	IPADBG("ipa rc deinit complete\n");
}

ssize_t status_show(struct device *dev, struct device_attribute *attr, char *ubuf)
{
	uint32_t res = 0;

	res = ipa_rc_monitor_health();
	IPAERR("ipa cur status code = %u\n", res);

	return scnprintf(ubuf, PAGE_SIZE, "%u\n", res);
}
