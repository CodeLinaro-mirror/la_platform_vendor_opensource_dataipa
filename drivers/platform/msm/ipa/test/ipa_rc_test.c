// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */
#include "ipa_i.h"
#include "ipahal_nat.h"

static char msg_buff[IPA_MAX_MSG_LEN + 1];

static void print_testcases(void)
{
	pr_err("These are TCs to induce below conditions\n");
	pr_err("Echo below input no. for TCs\n");
	pr_err("1. -> ETH chan_not_started \n");
	pr_err("2. -> WLAN chan_not_started \n");
	pr_err("3. -> ETH pkt_drop \n");
	pr_err("4. -> WLAN AP pkt_drop \n");
	pr_err("5. -> WLAN STA pkt_drop \n");
	pr_err("6. -> ETH Flt_rule_order \n");
	pr_err("7. -> WLAN AP Flt_rule_order \n");
	pr_err("8. -> WLAN STA Flt_rule_order \n");
	pr_err("9. -> NAT not_init \n");
	return;
}

static void ipa_test_eth_chan_not_start(enum ipa_client_type client)
{
	ipa_uc_offload_disconn_pipes_internal((u32)IPA_UC_NTN);
	return;
}

static void ipa_test_wlan_chan_not_start(enum ipa_client_type client)
{
	int chan, ep_idx, result;

	ep_idx = ipa_get_ep_mapping(client);
	chan = ipa3_get_chan_by_client(client);

	IPADBG("ep: %d chan: %d\n", ep_idx, chan);

	result = ipa3_disable_data_path(ep_idx);
	if (result) {
		IPAERR("disable data path failed clnt=%d.\n", ep_idx);
		return;
	}

	result = ipa3_stop_gsi_channel(ep_idx);
	if (result)
		IPAERR("failed to stop gsi chanl %d\n", ep_idx);

	return;
}

static void ipa_del_rt_rule_by_entry(struct ipa3_rt_entry *entry)
{
	int id;
	struct ipa3_hdr_entry *hdr_entry;
	struct ipa3_hdr_proc_ctx_entry *hdr_proc_entry;

	if (entry->hdr) {
		hdr_entry = ipa3_id_find(entry->rule.hdr_hdl);
		if (!hdr_entry || hdr_entry->cookie != IPA_HDR_COOKIE) {
			IPAERR("Header entry has already deleted\n");
			entry->hdr = NULL;
		}
	} else if (entry->proc_ctx) {
		hdr_proc_entry = ipa3_id_find(entry->rule.hdr_proc_ctx_hdl);
		if (!hdr_proc_entry ||
			hdr_proc_entry->cookie != IPA_PROC_HDR_COOKIE) {
			IPAERR("Proc header entry has already deleted\n");
			entry->proc_ctx = NULL;
		}
	}

	IPADBG("freed hdr/proc entry from rt tbl\n");
	if (entry->hdr &&
		(!ipa3_check_idr_if_freed(entry->hdr)))
		__ipa3_release_hdr(entry->hdr->id);
	else if (entry->proc_ctx &&
		(!ipa3_check_idr_if_freed(entry->proc_ctx)))
		__ipa3_release_hdr_proc_ctx(entry->proc_ctx->id);

	list_del(&entry->link);
	entry->tbl->rule_cnt--;

	/* if rule id was allocated from idr, remove it */
	if (!entry->rule_id_valid)
		idr_remove(entry->tbl->rule_ids, entry->rule_id);
	if (entry->tbl->rule_cnt == 0 && entry->tbl->ref_cnt == 0) {
		if (__ipa_del_rt_tbl(entry->tbl))
			IPAERR_RL("fail to del RT tbl\n");
	}

	entry->cookie = 0;
	id = entry->id;
	kmem_cache_free(ipa3_ctx->rt_rule_cache, entry);
	ipa3_id_remove(id);
	return;
}

static void ipa_del_flt_rule_by_entry(struct ipa3_flt_entry *entry)
{
	int id, rule_id;

	if (ipa3_id_find(entry->id) == NULL) {
		WARN_ON_RATELIMIT_IPA(1);
		return;
	}

	if (entry->ipacm_installed) {
		list_del(&entry->link);
		entry->tbl->rule_cnt--;
		if (entry->rt_tbl &&
			(!ipa3_check_idr_if_freed(
				entry->rt_tbl)))
			entry->rt_tbl->ref_cnt--;

		/* if rule id was allocated from idr, remove */
		rule_id = entry->rule_id;
		id = entry->id;
		if ((rule_id < ipahal_get_rule_id_hi_bit()) &&
			(rule_id >= ipahal_get_low_rule_id()))
			idr_remove(entry->tbl->rule_ids,
				rule_id);
		entry->cookie = 0;
		kmem_cache_free(ipa3_ctx->flt_rule_cache,
						entry);

		/* remove the handle from the database */
		ipa3_id_remove(id);
	}
}

static void ipa_test_pkt_drop(bool is_ul)
{
	struct ipa3_rt_tbl *tbl = NULL;
	struct ipa3_rt_entry *entry, *tmp;
	struct ipa3_rt_tbl_set *set = NULL;

	set = &ipa3_ctx->rt_tbl_set[IPA_IP_v4];

	mutex_lock(&ipa3_ctx->lock);
	if(list_empty(&set->head_rt_tbl_list)) {
		IPAERR("Rt table list is empty\n");
		mutex_unlock(&ipa3_ctx->lock);
		return;
	}

	list_for_each_entry(tbl, &set->head_rt_tbl_list, link) {
		if(list_empty(&tbl->head_rt_rule_list))
			continue;

		list_for_each_entry_safe(entry, tmp, &tbl->head_rt_rule_list, link) {
			if(is_ul && strcmp(entry->tbl->name, "WANRTBLv4") == 0) {
				ipa_del_rt_rule_by_entry(entry);
			}
			else if(!is_ul) {
				if(strcmp(entry->tbl->name, "COMRTBLLANv4") == 0
					&& entry->rule.dst != IPA_CLIENT_APPS_LAN_CONS)
					ipa_del_rt_rule_by_entry(entry);
			}
			else
				continue;

			if (ipa3_ctx->ctrl->ipa3_commit_rt(IPA_IP_v4))
				IPAERR("failed to commit RT tbl\n");
		}
	}
	mutex_unlock(&ipa3_ctx->lock);
	return;
}

static void ipa_test_ul_pkt_drop(void)
{
	ipa_test_pkt_drop(true);
	return;
}

static void ipa_test_dl_pkt_drop(void)
{
	ipa_test_pkt_drop(false);
	return;
}

static void ipa_test_flt_order(enum ipa_client_type client, uint32_t *metadata)
{
	u32 pyld_sz;
	struct ipa_ioc_add_flt_rule *param;
	struct ipa_flt_rule_add flt_rule_entry;

	pyld_sz = sizeof(struct ipa_ioc_add_flt_rule) +
	   sizeof(struct ipa_flt_rule_add);
	param = kzalloc(pyld_sz, GFP_KERNEL);
	if (!param) {
		IPAERR("Kzalloc failed\n");
		return;
	}

	param->commit = 1;
	param->ep = client;
	param->global = false;
	param->num_rules = (uint8_t)1;
	param->ip = IPA_IP_v4;

	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

	flt_rule_entry.at_rear = false;
	flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;
	flt_rule_entry.rule.retain_hdr = false;
	flt_rule_entry.rule.hashable = 0;
	flt_rule_entry.rule.rule_id = 530;
	flt_rule_entry.rule.eq_attrib_type = true;
	if(metadata) {
		flt_rule_entry.rule.attrib.meta_data = *metadata;
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_META_DATA;
	}

	memcpy(&(param->rules[0]), &flt_rule_entry,
		sizeof(struct ipa_flt_rule_add));

	/* installing dummy modem rule on top */
	if (ipa3_add_flt_rule_usr((struct ipa_ioc_add_flt_rule *)param, true))
		IPAERR("add modem filter rule failed\n");

	return;
}

static void ipa_test_eth_flt_order(void)
{
	ipa_test_flt_order(IPA_CLIENT_ETHERNET_PROD, NULL);
	return;
}

static void ipa_test_wlan_ap_flt_order(void)
{
	struct ipa_rc_wlan_intf_info *it;

	mutex_lock(&rc_ctx->rc_lock);
	list_for_each_entry(it, &ipa_rc_wlan_info.head, link) {
		if(it->wlan_msg_type == WLAN_AP_CONNECT) {
			ipa_test_flt_order(IPA_CLIENT_WLAN2_PROD, &it->metadata);
			break;
		}
	}
	mutex_unlock(&rc_ctx->rc_lock);
	return;
}

static void ipa_test_wlan_sta_flt_order(void)
{
	struct ipa3_flt_tbl *tbl;
	struct ipa3_flt_entry *entry, *next;
	int pipe;

	pipe = ipa3_get_ep_mapping(IPA_CLIENT_WLAN2_PROD);
	if(pipe == IPA_EP_NOT_ALLOCATED) {
		IPAERR("Invalid client.\n");
		return;
	}

	mutex_lock(&ipa3_ctx->lock);
	tbl = &ipa3_ctx->flt_tbl[pipe][IPA_IP_v4];
	list_for_each_entry_safe(entry, next, &tbl->head_flt_rule_list, link) {
		if(WLAN_STA_DL_FLT_RULE == get_group_id(entry, IPA_IP_v4, pipe)) {
			ipa_del_flt_rule_by_entry(entry);
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);
	return;
}

static void ipa_test_nat_init()
{
	ipa3_nat_ipv6ct_free_mem(&ipa3_ctx->nat_mem.dev);
	return;
}

ssize_t timer_val_store(struct device *dev, struct device_attribute *attr,
			const char *ubuf, size_t count)
{
	u32 val;
	if (count >= sizeof(msg_buff))
		return -EFAULT;

	memcpy(msg_buff, ubuf, count);
	msg_buff[count] = '\0';

	if(kstrtou32(msg_buff, 0, &val))
		return -EINVAL;

	query_timer = val;
	return count;
}

ssize_t testcase_show(struct device *dev, struct device_attribute *attr, char *ubuf)
{
	print_testcases();
	return 0;
}

ssize_t testcase_store(struct device *dev, struct device_attribute *attr,
			const char *ubuf, size_t count)
{
	u32 cmd;
	if (count >= sizeof(msg_buff))
		return -EFAULT;

	memcpy(msg_buff, ubuf, count);
	msg_buff[count] = '\0';

	if(kstrtou32(msg_buff, 0, &cmd))
		return -EINVAL;

	switch(cmd) {
		case 0:
			print_testcases();
			break;
		case 1:
			//IPA_CHANNEL_ETH_NOT_STARTED
			ipa_test_eth_chan_not_start(IPA_CLIENT_ETHERNET_CONS);
			break;
		case 2:
			//IPA_CHANNEL_WLAN_NOT_STARTED
			ipa_test_wlan_chan_not_start(IPA_CLIENT_WLAN2_CONS);
			break;
		case 3:
			//IPA_DRIVER_ETH_PKT_DROP
		case 4:
			//IPA_DRIVER_WLAN_AP_PKT_DROP
			ipa_test_ul_pkt_drop();
			break;
		case 5:
			//IPA_DRIVER_WLAN_STA_PKT_DROP
			ipa_test_dl_pkt_drop();
			break;
		case 6:
			//IPA_ETH_FILTER_RULE_INCORRECT
			ipa_test_eth_flt_order();
			break;
		case 7:
			//IPA_WLAN_AP_FILTER_RULE_INCORRECT
			ipa_test_wlan_ap_flt_order();
			break;
		case 8:
			//IPA_WLAN_STA_FILTER_RULE_INCORRECT
			ipa_test_wlan_sta_flt_order();
			break;
		case 9:
			//IPA_NAT_NOT_INITIALIZED
			ipa_test_nat_init();
			break;
		default:
			IPAERR("invalid testcase\n");
			break;
	}
	return count;
}