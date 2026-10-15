// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */
#include "ipa_i.h"
#include "ipahal_nat.h"

static char msg_buff[IPA_MAX_MSG_LEN + 1];

#define IPA_RC_TEST_MAX_FLT_RULES 8

/* Rules ipa_test_flt_order() installed, for cleanup. ep is the endpoint the
 * rule was installed on, so a cleanup can be scoped to one interface.
 */
static struct {
	u32 hdl;
	int ep;
} rc_test_flt[IPA_RC_TEST_MAX_FLT_RULES];
static int rc_test_flt_cnt;

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
	pr_err("10. -> WLAN AP Pkt drop for lan2lan case \n");
	pr_err("11. -> WLAN STA Flt_rule_order for level-2 recovery \n");
	pr_err("12. -> ETH1 chan_not_started \n");
	pr_err("13. -> WLAN2 chan_not_started \n");
	pr_err("14. -> WLAN3 chan_not_started \n");
	pr_err("15. -> ETH1 pkt_drop \n");
	pr_err("16. -> WLAN2 AP pkt_drop \n");
	pr_err("17. -> WLAN2 STA pkt_drop \n");
	pr_err("18. -> WLAN3 AP pkt_drop \n");
	pr_err("19. -> WLAN3 STA pkt_drop \n");
	pr_err("20. -> ETH1 Flt_rule_order \n");
	pr_err("21. -> WLAN2 AP Flt_rule_order \n");
	pr_err("22. -> WLAN2 STA Flt_rule_order \n");
	pr_err("23. -> WLAN3 AP Flt_rule_order \n");
	pr_err("24. -> WLAN3 STA Flt_rule_order \n");
	pr_err("25. -> MODEM pkts drop\n");
	pr_err("26. -> ETH WAN Flt_rule_order\n");
	pr_err("27. -> NAT init recovered (clear latch)\n");
	return;
}

static void ipa_test_eth_chan_not_start(enum ipa_client_type client)
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

	result = ipa_stop_gsi_channel(ep_idx);
	if (result)
		IPAERR("failed to stop gsi chanl %d\n", ep_idx);

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

	result = ipa_stop_gsi_channel(ep_idx);
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

static void ipa_test_pkt_drop_ip(bool is_ul, enum ipa_ip_type ip)
{
	struct ipa3_rt_tbl *tbl = NULL, *tbl_tmp = NULL;
	struct ipa3_rt_entry *entry, *tmp;
	struct ipa3_rt_tbl_set *set = NULL;
	const char *wan_tbl = (ip == IPA_IP_v6) ? "WANRTBLv6" : "WANRTBLv4";
	const char *lan_tbl = (ip == IPA_IP_v6) ? "COMRTBLv6" : "COMRTBLLANv4";

	set = &ipa3_ctx->rt_tbl_set[ip];

	mutex_lock(&ipa3_ctx->lock);
	if(list_empty(&set->head_rt_tbl_list)) {
		IPAERR("Rt table list is empty for ip=%d\n", ip);
		mutex_unlock(&ipa3_ctx->lock);
		return;
	}

	list_for_each_entry_safe(tbl, tbl_tmp, &set->head_rt_tbl_list, link) {
		if(list_empty(&tbl->head_rt_rule_list))
			continue;

		list_for_each_entry_safe(entry, tmp, &tbl->head_rt_rule_list, link) {
			if(is_ul && strcmp(entry->tbl->name, wan_tbl) == 0) {
				ipa_del_rt_rule_by_entry(entry);
			}
			else if(!is_ul) {
				if(strcmp(entry->tbl->name, lan_tbl) == 0
					&& entry->rule.dst != IPA_CLIENT_APPS_LAN_CONS)
					ipa_del_rt_rule_by_entry(entry);
			}
			else
				continue;

			if (ipa3_ctx->ctrl->ipa3_commit_rt(ip))
				IPAERR("failed to commit RT tbl for ip=%d\n", ip);
		}
	}
	mutex_unlock(&ipa3_ctx->lock);
	return;
}

static void ipa_test_pkt_drop(bool is_ul)
{
	/* Induce on both IPv4 and IPv6 WAN/LAN route tables. */
	ipa_test_pkt_drop_ip(is_ul, IPA_IP_v4);
	ipa_test_pkt_drop_ip(is_ul, IPA_IP_v6);
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

/*
 * A routing table kept permanently empty, one per IP family. An empty table is
 * skipped by ipa_translate_rt_tbl_to_hw_fmt() (sz == 0), so its HW header entry
 * keeps pointing at the zeroed apps_fltrt_empty_tbl region and every lookup
 * landing in it reports a rule miss.
 */
static u32 rc_test_empty_rt_hdl[IPA_IP_MAX];

static int ipa_rc_test_get_empty_rt_tbl(enum ipa_ip_type ip, u32 *hdl)
{
	struct ipa_ioc_add_rt_rule *add = NULL;
	struct ipa_ioc_get_rt_tbl lookup;
	const char *name;
	u32 rule_hdl = 0;
	int ret = -EFAULT;

	if (ip >= IPA_IP_MAX)
		return -EINVAL;

	if (rc_test_empty_rt_hdl[ip]) {
		*hdl = rc_test_empty_rt_hdl[ip];
		return 0;
	}

	name = (ip == IPA_IP_v6) ? "RCTSTEMPTYv6" : "RCTSTEMPTYv4";

	add = kzalloc(sizeof(*add) + sizeof(struct ipa_rt_rule_add), GFP_KERNEL);
	if (!add) {
		IPAERR("Kzalloc failed\n");
		return -ENOMEM;
	}

	/*
	 * __ipa_add_rt_tbl() is private to ipa_rt.c, so the table is created as a
	 * side effect of adding one throwaway rule, which is removed again below.
	 */
	add->commit = 1;
	add->ip = ip;
	add->num_rules = 1;
	strscpy(add->rt_tbl_name, name, IPA_RESOURCE_NAME_MAX);
	add->rules[0].at_rear = 1;
	add->rules[0].rule.dst = IPA_CLIENT_APPS_LAN_CONS;

	if (ipa_add_rt_rule(add) || add->rules[0].status) {
		IPAERR("failed to create rt tbl %s status=%d\n",
			name, add->rules[0].status);
		goto out;
	}
	rule_hdl = add->rules[0].rt_rule_hdl;

	/*
	 * Take a reference before emptying it: a table is only torn down when
	 * rule_cnt and ref_cnt are both zero.
	 */
	memset(&lookup, 0, sizeof(lookup));
	lookup.ip = ip;
	strscpy(lookup.name, name, IPA_RESOURCE_NAME_MAX);
	if (ipa3_get_rt_tbl(&lookup)) {
		IPAERR("failed to get rt tbl %s, leaving rule %u behind\n",
			name, rule_hdl);
		goto out;
	}

	mutex_lock(&ipa3_ctx->lock);
	if (__ipa3_del_rt_rule(rule_hdl))
		IPAERR("failed to empty rt tbl %s\n", name);
	if (ipa3_ctx->ctrl->ipa3_commit_rt(ip))
		IPAERR("failed to commit RT tbl for ip=%d\n", ip);
	mutex_unlock(&ipa3_ctx->lock);

	rc_test_empty_rt_hdl[ip] = lookup.hdl;
	*hdl = lookup.hdl;
	ret = 0;
	IPADBG("empty rt tbl %s ready hdl=%u\n", name, lookup.hdl);
out:
	kfree(add);
	return ret;
}

/*
 * Install a catch-all filter rule at the head of @client's filter table that
 * sends every packet ingressing on that pipe into the empty routing table.
 *
 * The route lookup then misses, IPA delivers the packet to the default route
 * pipe (APPS_LAN_CONS) with rt_rule_id == IPAHAL_PKT_STATUS_FLTRT_RULE_MISS_ID,
 * and ipa3_lan_rx_pyld_hdlr() drops it and charges the drop to the *source*
 * pipe - which is what makes it land on the intended rc_client. Unlike
 * ipa_test_pkt_drop_ip(), this does not touch the WAN route tables and so works
 * on a modem backhaul as well as an ethernet one, and it targets one endpoint
 * rather than every UL client at once.
 */
static void ipa_test_pkt_drop_client_ip(enum ipa_client_type client,
	enum ipa_ip_type ip)
{
	struct ipa_ioc_add_flt_rule *param;
	struct ipa_flt_rule_add flt_rule_entry;
	u32 pyld_sz, rt_hdl = 0;
	int ret, ep;

	ep = ipa_get_ep_mapping(client);
	if (ep == IPA_EP_NOT_ALLOCATED) {
		IPAERR("client %d is not mapped to a pipe\n", client);
		return;
	}

	if (ipa_rc_test_get_empty_rt_tbl(ip, &rt_hdl))
		return;

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
	param->ip = ip;
	param->num_rules = (uint8_t)1;

	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
	/*
	 * at_rear = false so this wins over the real UL rules; attrib_mask left
	 * at 0 so the rule generates no equations and therefore matches every
	 * packet; non-hashable so every packet is evaluated.
	 */
	flt_rule_entry.at_rear = false;
	flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
	flt_rule_entry.rule.rt_tbl_hdl = rt_hdl;
	flt_rule_entry.rule.retain_hdr = false;
	flt_rule_entry.rule.hashable = 0;

	memcpy(&(param->rules[0]), &flt_rule_entry,
		sizeof(struct ipa_flt_rule_add));

	ret = ipa3_add_flt_rule_usr(param, true);
	if (ret || param->rules[0].status) {
		IPAERR("failed to add drop rule ep=%d ip=%d ret=%d status=%d\n",
			ep, ip, ret, param->rules[0].status);
		goto out;
	}

	mutex_lock(&ipa3_ctx->lock);
	if (rc_test_flt_cnt < IPA_RC_TEST_MAX_FLT_RULES) {
		rc_test_flt[rc_test_flt_cnt].hdl = param->rules[0].flt_rule_hdl;
		rc_test_flt[rc_test_flt_cnt].ep = ep;
		rc_test_flt_cnt++;
	} else {
		IPAERR("hdl table full, rule %u won't be cleaned up\n",
			param->rules[0].flt_rule_hdl);
	}
	mutex_unlock(&ipa3_ctx->lock);

	IPADBG("ep=%d ip=%d catch-all route-miss rule installed hdl=%u\n",
		ep, ip, param->rules[0].flt_rule_hdl);
out:
	kfree(param);
	return;
}

static void ipa_test_pkt_drop_client(enum ipa_client_type client)
{
	ipa_test_pkt_drop_client_ip(client, IPA_IP_v4);
	ipa_test_pkt_drop_client_ip(client, IPA_IP_v6);
	return;
}

static void ipa_test_ap_pkt_drop_lan_to_lan(void)
{
	struct ipa3_rt_tbl *tbl = NULL, *tbl_tmp = NULL;
	struct ipa3_rt_entry *entry, *tmp;
	struct ipa3_rt_tbl_set *set = NULL;

	set = &ipa3_ctx->rt_tbl_set[IPA_IP_v4];

	mutex_lock(&ipa3_ctx->lock);
	if(list_empty(&set->head_rt_tbl_list)) {
		IPAERR("Rt table list is empty\n");
		mutex_unlock(&ipa3_ctx->lock);
		return;
	}

	list_for_each_entry_safe(tbl, tbl_tmp, &set->head_rt_tbl_list, link) {
		if(list_empty(&tbl->head_rt_rule_list))
			continue;

		list_for_each_entry_safe(entry, tmp, &tbl->head_rt_rule_list, link) {
			if((strcmp(entry->tbl->name, "eth_v4_ETH_II_to_ETH_II") == 0 ||
				strcmp(entry->tbl->name, "eth_v4_ETH_II_to_802_1Q") == 0)
				&& (entry->rule.dst == IPA_CLIENT_ETHERNET_CONS))
					ipa_del_rt_rule_by_entry(entry);
			else
				continue;

			if (ipa3_ctx->ctrl->ipa3_commit_rt(IPA_IP_v4))
				IPAERR("failed to commit RT tbl\n");
		}
	}
	mutex_unlock(&ipa3_ctx->lock);
	return;
}

static void ipa_test_flt_order(enum ipa_client_type client, uint32_t *metadata)
{
	u32 pyld_sz;
	struct ipa_ioc_add_flt_rule *param;
	struct ipa_flt_rule_add flt_rule_entry;
	int ret;

	IPADBG("[tc6] ipa_test_flt_order: client=%d metadata=%s\n",
		client, metadata ? "set" : "NULL");

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
	flt_rule_entry.rule.rule_id = 800;
	flt_rule_entry.rule.eq_attrib_type = true;
	if(metadata) {
		flt_rule_entry.rule.attrib.meta_data = *metadata;
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_META_DATA;
	}

	memcpy(&(param->rules[0]), &flt_rule_entry,
		sizeof(struct ipa_flt_rule_add));

	IPADBG("[tc6] ipa_test_flt_order: calling ipa3_add_flt_rule_usr ep=%d\n", client);
	/* installing dummy modem rule on top */
	ret = ipa3_add_flt_rule_usr((struct ipa_ioc_add_flt_rule *)param, true);
	if (ret)
		IPAERR("[tc6] add modem filter rule failed ret=%d\n", ret);
	else
		IPADBG("[tc6] ipa_test_flt_order: rule installed ok, rule_id=%d status=%d\n",
			param->rules[0].rule.rule_id, param->rules[0].status);

	/* Verify the rule appears in the filter table */
	{
		int pipe = ipa_get_ep_mapping(client);
		struct ipa3_flt_tbl *tbl;
		struct ipa3_flt_entry *e;
		int cnt = 0;

		IPADBG("[tc6] ipa_test_flt_order: verifying pipe=%d flt table\n", pipe);
		mutex_lock(&ipa3_ctx->lock);
		tbl = &ipa3_ctx->flt_tbl[pipe][IPA_IP_v4];
		list_for_each_entry(e, &tbl->head_flt_rule_list, link) {
			IPADBG("[tc6] flt_tbl rule[%d]: rule_id=%u eq=%d action=%d cookie=0x%x\n",
				cnt, e->rule_id, e->rule.eq_attrib_type,
				e->rule.action, e->cookie);
			cnt++;
		}
		IPADBG("[tc6] flt_tbl total rules: %d\n", cnt);
		mutex_unlock(&ipa3_ctx->lock);
	}

	if (!ret && !param->rules[0].status) {
		mutex_lock(&ipa3_ctx->lock);
		if (rc_test_flt_cnt < IPA_RC_TEST_MAX_FLT_RULES) {
			rc_test_flt[rc_test_flt_cnt].hdl = param->rules[0].flt_rule_hdl;
			rc_test_flt[rc_test_flt_cnt].ep = ipa_get_ep_mapping(client);
			rc_test_flt_cnt++;
		} else {
			IPAERR("hdl table full, rule %u won't be cleaned up\n",
				param->rules[0].flt_rule_hdl);
		}
		mutex_unlock(&ipa3_ctx->lock);
	}

	kfree(param);
	return;
}

static void ipa_test_eth_flt_order(void)
{
	ipa_test_flt_order(IPA_CLIENT_ETHERNET_PROD, NULL);
	return;
}

static void ipa_test_eth1_flt_order(void)
{
	ipa_test_flt_order(IPA_CLIENT_ETHERNET2_PROD, NULL);
	return;
}

static void ipa_test_wlan_ap_flt_order(void)
{
	struct ipa_rc_wlan_intf_info *it;

	mutex_lock(&rc_ctx->rc_lock);
	list_for_each_entry(it, &ipa_rc_wlan_info.head, link) {
		if(it->wlan_msg_type == WLAN_AP_CONNECT) {
			ipa_test_flt_order(IPA_CLIENT_WLAN1_PROD, &it->metadata);
			break;
		}
	}
	mutex_unlock(&rc_ctx->rc_lock);
	return;
}

static void ipa_test_wlan2_ap_flt_order(void)
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

static void ipa_test_wlan3_ap_flt_order(void)
{
	struct ipa_rc_wlan_intf_info *it;

	mutex_lock(&rc_ctx->rc_lock);
	list_for_each_entry(it, &ipa_rc_wlan_info.head, link) {
		if(it->wlan_msg_type == WLAN_AP_CONNECT) {
			ipa_test_flt_order(IPA_CLIENT_WLAN3_PROD, &it->metadata);
			break;
		}
	}
	mutex_unlock(&rc_ctx->rc_lock);
	return;
}

static void ipa_test_eth_sta_flt_order(void)
{
	struct ipa3_flt_tbl *tbl;
	struct ipa3_flt_entry *entry, *next;
	int pipe;

	pipe = ipa_get_ep_mapping(IPA_CLIENT_ETHERNET_PROD);
	if(pipe == IPA_EP_NOT_ALLOCATED) {
		IPAERR("Invalid client.\n");
		return;
	}

	mutex_lock(&ipa3_ctx->lock);
	tbl = &ipa3_ctx->flt_tbl[pipe][IPA_IP_v4];
	list_for_each_entry_safe(entry, next, &tbl->head_flt_rule_list, link) {
		if(entry->cat == IPA_FLT_RULE_CAT_DOWNLINK ||
			entry->cat == IPA_FLT_RULE_CAT_ETH_BH_CATCHALL ||
			entry->rule.action == IPA_PASS_TO_DST_NAT) {
			ipa_del_flt_rule_by_entry(entry);
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);
	return;
}

static void ipa_test_wlan_sta_flt_order(void)
{
	struct ipa3_flt_tbl *tbl;
	struct ipa3_flt_entry *entry, *next;
	int pipe;

	pipe = ipa_get_ep_mapping(IPA_CLIENT_WLAN1_PROD);
	if(pipe == IPA_EP_NOT_ALLOCATED) {
		IPAERR("Invalid client.\n");
		return;
	}

	mutex_lock(&ipa3_ctx->lock);
	tbl = &ipa3_ctx->flt_tbl[pipe][IPA_IP_v4];
	list_for_each_entry_safe(entry, next, &tbl->head_flt_rule_list, link) {
		if(entry->cat == IPA_FLT_RULE_CAT_DOWNLINK ||
			entry->cat == IPA_FLT_RULE_CAT_ETH_BH_CATCHALL ||
			entry->rule.action == IPA_PASS_TO_DST_NAT) {
			ipa_del_flt_rule_by_entry(entry);
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);
	return;
}

static void ipa_test_wlan2_sta_flt_order(void)
{
	struct ipa3_flt_tbl *tbl;
	struct ipa3_flt_entry *entry, *next;
	int pipe;

	pipe = ipa_get_ep_mapping(IPA_CLIENT_WLAN2_PROD);
	if(pipe == IPA_EP_NOT_ALLOCATED) {
		IPAERR("Invalid client.\n");
		return;
	}

	mutex_lock(&ipa3_ctx->lock);
	tbl = &ipa3_ctx->flt_tbl[pipe][IPA_IP_v4];
	list_for_each_entry_safe(entry, next, &tbl->head_flt_rule_list, link) {
		if(entry->cat == IPA_FLT_RULE_CAT_DOWNLINK ||
			entry->cat == IPA_FLT_RULE_CAT_ETH_BH_CATCHALL ||
			entry->rule.action == IPA_PASS_TO_DST_NAT) {
			ipa_del_flt_rule_by_entry(entry);
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);
	return;
}

static void ipa_test_wlan3_sta_flt_order(void)
{
	struct ipa3_flt_tbl *tbl;
	struct ipa3_flt_entry *entry, *next;
	int pipe;

	pipe = ipa_get_ep_mapping(IPA_CLIENT_WLAN3_PROD);
	if(pipe == IPA_EP_NOT_ALLOCATED) {
		IPAERR("Invalid client.\n");
		return;
	}

	mutex_lock(&ipa3_ctx->lock);
	tbl = &ipa3_ctx->flt_tbl[pipe][IPA_IP_v4];
	list_for_each_entry_safe(entry, next, &tbl->head_flt_rule_list, link) {
		if(entry->cat == IPA_FLT_RULE_CAT_DOWNLINK ||
			entry->cat == IPA_FLT_RULE_CAT_ETH_BH_CATCHALL ||
			entry->rule.action == IPA_PASS_TO_DST_NAT) {
			ipa_del_flt_rule_by_entry(entry);
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);
	return;
}

static void ipa_test_sta_flt_order(enum ipa_client_type client, uint32_t *metadata)
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
	flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
	flt_rule_entry.rule.retain_hdr = false;
	flt_rule_entry.rule.hashable = 0;
	flt_rule_entry.rule.eq_attrib_type = true;
	if(metadata) {
		flt_rule_entry.rule.attrib.meta_data = *metadata;
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_META_DATA;
	}

	memcpy(&(param->rules[0]), &flt_rule_entry,
		sizeof(struct ipa_flt_rule_add));

	/* installing dummy default rule on top */
	if (ipa3_add_flt_rule_usr((struct ipa_ioc_add_flt_rule *)param, true) ||
		param->rules[0].status) {
		IPAERR("add dummy flt rule failed\n");
		kfree(param);
		return;
	}

	mutex_lock(&ipa3_ctx->lock);
	if (rc_test_flt_cnt < IPA_RC_TEST_MAX_FLT_RULES) {
		rc_test_flt[rc_test_flt_cnt].hdl = param->rules[0].flt_rule_hdl;
		rc_test_flt[rc_test_flt_cnt].ep = ipa_get_ep_mapping(client);
		rc_test_flt_cnt++;
	} else {
		IPAERR("hdl table full, rule %u won't be cleaned up\n",
			param->rules[0].flt_rule_hdl);
	}
	mutex_unlock(&ipa3_ctx->lock);

	kfree(param);
	return;
}

static void ipa_test_wlan_sta_flt_order_lvl2(void)
{
	struct ipa_rc_wlan_intf_info *it;
	int pipe;

	pipe = ipa_get_ep_mapping(IPA_CLIENT_WLAN2_PROD);
	if(pipe == IPA_EP_NOT_ALLOCATED) {
		IPAERR("Invalid client.\n");
		return;
	}

	mutex_lock(&rc_ctx->rc_lock);
	list_for_each_entry(it, &ipa_rc_wlan_info.head, link) {
		if(it->wlan_msg_type == WLAN_STA_CONNECT) {
			ipa_test_sta_flt_order(IPA_CLIENT_WLAN2_PROD, &it->metadata);
			break;
		}
	}
	mutex_unlock(&rc_ctx->rc_lock);
	return;
}

static void ipa_test_nat_init(void)
{
	/* Latch the NAT-init anomaly, as ipa_backend does on a real failure. */
	ipa_rc_set_nat_init_status(true);
	return;
}

static void ipa_test_nat_recover(void)
{
	/* Clear the latch so IPA-RC emits the NAT recovered edge. */
	ipa_rc_set_nat_init_status(false);
	return;
}

void ipa_rc_test_cleanup_rules(int ep)
{
	int i, keep = 0, cnt = 0;

	mutex_lock(&ipa3_ctx->lock);
	for (i = 0; i < rc_test_flt_cnt; i++) {
		if (ep != IPA_RC_TEST_EP_ANY && rc_test_flt[i].ep != ep) {
			rc_test_flt[keep++] = rc_test_flt[i];
			continue;
		}
		if (!__ipa_del_flt_rule(rc_test_flt[i].hdl))
			cnt++;
	}
	rc_test_flt_cnt = keep;

	/*
	 * Both families are committed unconditionally. The rules are already
	 * gone from the SW list by this point, so short-circuiting past one
	 * commit would leave that family's catch-all rule live in the HW image
	 * with no handle left to remove it - a permanent silent drop on the
	 * pipe.
	 */
	if (cnt) {
		if (ipa3_ctx->ctrl->ipa3_commit_flt(IPA_IP_v4))
			IPAERR("failed to commit v4 flt tbl\n");
		if (ipa3_ctx->ctrl->ipa3_commit_flt(IPA_IP_v6))
			IPAERR("failed to commit v6 flt tbl\n");
	}
	mutex_unlock(&ipa3_ctx->lock);

	IPADBG("ep %d: removed %d testcase filter rule(s)\n", ep, cnt);
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
			ipa_test_wlan_chan_not_start(IPA_CLIENT_WLAN1_CONS);
			break;
		case 3:
			//IPA_DRIVER_ETH_PKT_DROP
			ipa_test_pkt_drop_client(IPA_CLIENT_ETHERNET_PROD);
			break;
		case 15: // ETH1 pkt drop
			ipa_test_pkt_drop_client(IPA_CLIENT_ETHERNET2_PROD);
			break;
		case 4:
			//IPA_DRIVER_WLAN_AP_PKT_DROP
		case 16: // WLAN2 AP pkt drop
		case 18: // WLAN3 AP pkt drop
			ipa_test_ul_pkt_drop();
			break;
		case 5:
			//IPA_DRIVER_WLAN_STA_PKT_DROP
		case 17: // WLAN2 STA pkt drop
		case 19: // WLAN3 STA pkt drop
		case 25: //MODEM pkt drop
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
		case 10:
			//IPA_DRIVER_WLAN_AP_PKT_DROP -> for lan2lan scenario
			ipa_test_ap_pkt_drop_lan_to_lan();
			break;
		case 11:
			//IPA_WLAN_STA_FILTER_RULE_INCORRECT -> to test level2 recovery
			ipa_test_wlan_sta_flt_order_lvl2();
			break;
		case 12:
			//IPA_CHANNEL_ETH1_NOT_STARTED
			ipa_test_eth_chan_not_start(IPA_CLIENT_ETHERNET2_CONS);
			break;
		case 13:
			//IPA_CHANNEL_WLAN2_NOT_STARTED
			ipa_test_wlan_chan_not_start(IPA_CLIENT_WLAN2_CONS);
			break;
		case 14:
			//IPA_CHANNEL_WLAN3_NOT_STARTED
			ipa_test_wlan_chan_not_start(IPA_CLIENT_WLAN4_CONS);
			break;
		case 20:
			//IPA_ETH1_FILTER_RULE_INCORRECT
			ipa_test_eth1_flt_order();
			break;
		case 21:
			//IPA_WLAN2_AP_FILTER_RULE_INCORRECT
			ipa_test_wlan2_ap_flt_order();
			break;
		case 22:
			//IPA_WLAN2_STA_FILTER_RULE_INCORRECT
			ipa_test_wlan2_sta_flt_order();
			break;
		case 23:
			//IPA_WLAN3_AP_FILTER_RULE_INCORRECT
			ipa_test_wlan3_ap_flt_order();
			break;
		case 24:
			//IPA_WLAN3_STA_FILTER_RULE_INCORRECT
			ipa_test_wlan3_sta_flt_order();
			break;
		case 26:
			//ETH WAN FILTER_RULE_INCORRECT;
			ipa_test_eth_sta_flt_order();
			break;
		case 27:
			//IPA_NAT_NOT_INITIALIZED recovery (clear latch)
			ipa_test_nat_recover();
			break;
		default:
			IPAERR("invalid testcase\n");
			break;
	}
	return count;
}
