// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017-2020, The Linux Foundation. All rights reserved.
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include "ipa_ut_framework.h"
#include "ipa_i.h"
#include "ipa_ipsec.h"
#include <linux/netdevice.h>

enum ipa_test_hw_stats_counter {
	IPA_TEST_HW_STATS_RT4_USB,
	IPA_TEST_HW_STATS_RT6_USB,
	IPA_TEST_HW_STATS_RT4_CONS,
	IPA_TEST_HW_STATS_RT6_CONS,
	IPA_TEST_HW_STATS_FLT4_USB,
	IPA_TEST_HW_STATS_FLT6_USB,
	IPA_TEST_HW_STATS_FLT4_PROD,
	IPA_TEST_HW_STATS_FLT6_PROD,
	IPA_TEST_HW_STATS_COUNTER_MAX,
};

struct ipa_test_hw_stats_ctx {
	u32 odu_prod_hdl;
	u32 odu_cons_hdl;
	enum ipa_client_type prod_client;
	enum ipa_client_type cons_client;
	u32 rt4_usb_rule_hdl;
	u32 rt6_usb_rule_hdl;
	u32 rt4_cons_rule_hdl;
	u32 rt6_cons_rule_hdl;
	u32 flt4_usb_rule_hdl;
	u32 flt6_usb_rule_hdl;
	u32 flt4_prod_rule_hdl;
	u32 flt6_prod_rule_hdl;
	int fnr_counter_hdl;
	atomic_t odu_pending;
	u8 hw_counter_start_id;
	bool odu_prod_pipe_setup;
	bool odu_cons_pipe_setup;
	bool rt4_usb_rule_added;
	bool rt6_usb_rule_added;
	bool rt4_cons_rule_added;
	bool rt6_cons_rule_added;
	bool flt4_usb_rule_added;
	bool flt6_usb_rule_added;
	bool flt4_prod_rule_added;
	bool flt6_prod_rule_added;
	bool fnr_counters_allocated;
	bool fnr_flt_commit_pending[IPA_IP_MAX];
	bool fnr_rt_commit_pending[IPA_IP_MAX];
};

static struct ipa_test_hw_stats_ctx *ctx;

static int ipa_test_hw_stats_suite_teardown(void *priv);

static u32 ipa_test_hw_stats_counter_id(
	enum ipa_test_hw_stats_counter counter)
{
	return ctx->hw_counter_start_id + counter;
}

static int ipa_test_hw_stats_suite_setup(void **ppriv)
{
	int ret;

	IPA_UT_DBG("Start Setup\n");

	if (!ctx) {
		ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
		if (!ctx)
			return -ENOMEM;
	} else if (ctx->odu_prod_pipe_setup || ctx->odu_cons_pipe_setup ||
		ctx->fnr_counters_allocated ||
		ctx->rt4_usb_rule_added || ctx->rt6_usb_rule_added ||
		ctx->rt4_cons_rule_added || ctx->rt6_cons_rule_added ||
		ctx->flt4_usb_rule_added || ctx->flt6_usb_rule_added ||
		ctx->flt4_prod_rule_added || ctx->flt6_prod_rule_added ||
		ctx->fnr_flt_commit_pending[IPA_IP_v4] ||
		ctx->fnr_flt_commit_pending[IPA_IP_v6] ||
		ctx->fnr_rt_commit_pending[IPA_IP_v4] ||
		ctx->fnr_rt_commit_pending[IPA_IP_v6]) {
		IPA_UT_INFO("retrying cleanup from the previous run\n");
		ret = ipa_test_hw_stats_suite_teardown(NULL);
		if (ret) {
			IPA_UT_ERR("previous cleanup is still incomplete %d\n",
				ret);
			return ret;
		}
	}

	return 0;
}

static int ipa_test_hw_stats_del_flt_rule(enum ipa_ip_type ip, u32 hdl,
	bool *rule_added, bool *commit_pending)
{
	struct ipa_ioc_del_flt_rule *flt_rule;
	int ret;

	if (!*rule_added)
		return 0;

	flt_rule = kzalloc(sizeof(*flt_rule) +
		sizeof(struct ipa_flt_rule_del), GFP_KERNEL);
	if (!flt_rule)
		return -ENOMEM;

	flt_rule->commit = 0;
	flt_rule->ip = ip;
	flt_rule->num_hdls = 1;
	flt_rule->hdl[0].hdl = hdl;
	flt_rule->hdl[0].status = -1;

	ret = ipa3_del_flt_rule(flt_rule);
	if (!ret && !flt_rule->hdl[0].status) {
		*rule_added = false;
		*commit_pending = true;
	}
	if (ret || flt_rule->hdl[0].status) {
		IPA_UT_ERR("failed to delete filter rule 0x%x\n", hdl);
		if (!ret)
			ret = -EFAULT;
	}

	kfree(flt_rule);
	return ret;
}

static int ipa_test_hw_stats_del_rt_rule(enum ipa_ip_type ip, u32 hdl,
	bool *rule_added, bool *commit_pending)
{
	struct ipa_ioc_del_rt_rule *rt_rule;
	int ret;

	if (!*rule_added)
		return 0;

	rt_rule = kzalloc(sizeof(*rt_rule) +
		sizeof(struct ipa_rt_rule_del), GFP_KERNEL);
	if (!rt_rule)
		return -ENOMEM;

	rt_rule->commit = 0;
	rt_rule->ip = ip;
	rt_rule->num_hdls = 1;
	rt_rule->hdl[0].hdl = hdl;
	rt_rule->hdl[0].status = -1;

	ret = ipa3_del_rt_rule(rt_rule);
	if (!ret && !rt_rule->hdl[0].status) {
		*rule_added = false;
		*commit_pending = true;
	}
	if (ret || rt_rule->hdl[0].status) {
		IPA_UT_ERR("failed to delete routing rule 0x%x\n", hdl);
		if (!ret)
			ret = -EFAULT;
	}

	kfree(rt_rule);
	return ret;
}

static int ipa_test_hw_stats_add_rt_rule(
	struct ipa_ioc_add_rt_rule_v2 *rt_rule, u32 *hdl, bool *rule_added)
{
	struct ipa_rt_rule_add_v2 *rule;
	int ret;

	rule = (struct ipa_rt_rule_add_v2 *)rt_rule->rules;
	rule->status = -1;
	ret = ipa3_add_rt_rule_v2(rt_rule);
	if (!rule->status) {
		*hdl = rule->rt_rule_hdl;
		*rule_added = true;
	}
	if (ret || rule->status) {
		IPA_UT_ERR("failed to install routing rule\n");
		if (!ret)
			ret = -EFAULT;
	}

	return ret;
}

static int ipa_test_hw_stats_add_flt_rule(
	struct ipa_ioc_add_flt_rule_v2 *flt_rule, u32 *hdl, bool *rule_added)
{
	struct ipa_flt_rule_add_v2 *rule;
	int ret;

	rule = (struct ipa_flt_rule_add_v2 *)flt_rule->rules;
	rule->status = -1;
	ret = ipa3_add_flt_rule_v2(flt_rule);
	if (!rule->status) {
		*hdl = rule->flt_rule_hdl;
		*rule_added = true;
	}
	if (ret || rule->status) {
		IPA_UT_ERR("failed to install filter rule\n");
		if (!ret)
			ret = -EFAULT;
	}

	return ret;
}

static int ipa_test_hw_stats_commit_rules(enum ipa_ip_type ip,
	bool *commit_pending, bool routing)
{
	int ret;

	if (!*commit_pending)
		return 0;

	ret = routing ? ipa3_commit_rt(ip) : ipa3_commit_flt(ip);
	if (ret) {
		IPA_UT_ERR("failed to commit FNR %s rules for IP %d %d\n",
			routing ? "routing" : "filter", ip, ret);
		return ret;
	}

	*commit_pending = false;
	return 0;
}

static int ipa_test_hw_stats_cleanup_FnR(void)
{
	int ret = 0;
	int res;

	/*
	 * Commit filter-rule removal before modifying routes. This prevents a
	 * hardware filter from referring to a route table that software releases.
	 * Keep failed commits pending so the next cleanup attempt can retry them.
	 */
	/* Stop USB traffic from reaching the test consumer first. */
	res = ipa_test_hw_stats_del_flt_rule(IPA_IP_v4,
		ctx->flt4_usb_rule_hdl, &ctx->flt4_usb_rule_added,
		&ctx->fnr_flt_commit_pending[IPA_IP_v4]);
	if (res)
		ret = res;
	res = ipa_test_hw_stats_del_flt_rule(IPA_IP_v6,
		ctx->flt6_usb_rule_hdl, &ctx->flt6_usb_rule_added,
		&ctx->fnr_flt_commit_pending[IPA_IP_v6]);
	if (res && !ret)
		ret = res;
	res = ipa_test_hw_stats_del_flt_rule(IPA_IP_v4,
		ctx->flt4_prod_rule_hdl, &ctx->flt4_prod_rule_added,
		&ctx->fnr_flt_commit_pending[IPA_IP_v4]);
	if (res && !ret)
		ret = res;
	res = ipa_test_hw_stats_del_flt_rule(IPA_IP_v6,
		ctx->flt6_prod_rule_hdl, &ctx->flt6_prod_rule_added,
		&ctx->fnr_flt_commit_pending[IPA_IP_v6]);
	if (res && !ret)
		ret = res;

	res = ipa_test_hw_stats_commit_rules(IPA_IP_v4,
		&ctx->fnr_flt_commit_pending[IPA_IP_v4], false);
	if (res && !ret)
		ret = res;
	res = ipa_test_hw_stats_commit_rules(IPA_IP_v6,
		&ctx->fnr_flt_commit_pending[IPA_IP_v6], false);
	if (res && !ret)
		ret = res;

	/* Remove test routes only after no filter can reference them. */
	if (!ctx->flt4_usb_rule_added && !ctx->flt4_prod_rule_added &&
		!ctx->fnr_flt_commit_pending[IPA_IP_v4]) {
		res = ipa_test_hw_stats_del_rt_rule(IPA_IP_v4,
			ctx->rt4_usb_rule_hdl, &ctx->rt4_usb_rule_added,
			&ctx->fnr_rt_commit_pending[IPA_IP_v4]);
		if (res && !ret)
			ret = res;
		res = ipa_test_hw_stats_del_rt_rule(IPA_IP_v4,
			ctx->rt4_cons_rule_hdl, &ctx->rt4_cons_rule_added,
			&ctx->fnr_rt_commit_pending[IPA_IP_v4]);
		if (res && !ret)
			ret = res;
	}

	if (!ctx->flt6_usb_rule_added && !ctx->flt6_prod_rule_added &&
		!ctx->fnr_flt_commit_pending[IPA_IP_v6]) {
		res = ipa_test_hw_stats_del_rt_rule(IPA_IP_v6,
			ctx->rt6_usb_rule_hdl, &ctx->rt6_usb_rule_added,
			&ctx->fnr_rt_commit_pending[IPA_IP_v6]);
		if (res && !ret)
			ret = res;
		res = ipa_test_hw_stats_del_rt_rule(IPA_IP_v6,
			ctx->rt6_cons_rule_hdl, &ctx->rt6_cons_rule_added,
			&ctx->fnr_rt_commit_pending[IPA_IP_v6]);
		if (res && !ret)
			ret = res;
	}

	res = ipa_test_hw_stats_commit_rules(IPA_IP_v4,
		&ctx->fnr_rt_commit_pending[IPA_IP_v4], true);
	if (res && !ret)
		ret = res;
	res = ipa_test_hw_stats_commit_rules(IPA_IP_v6,
		&ctx->fnr_rt_commit_pending[IPA_IP_v6], true);
	if (res && !ret)
		ret = res;

	if (ctx->fnr_counters_allocated &&
		!ctx->rt4_usb_rule_added && !ctx->rt6_usb_rule_added &&
		!ctx->rt4_cons_rule_added && !ctx->rt6_cons_rule_added &&
		!ctx->flt4_usb_rule_added && !ctx->flt6_usb_rule_added &&
		!ctx->flt4_prod_rule_added && !ctx->flt6_prod_rule_added &&
		!ctx->fnr_flt_commit_pending[IPA_IP_v4] &&
		!ctx->fnr_flt_commit_pending[IPA_IP_v6] &&
		!ctx->fnr_rt_commit_pending[IPA_IP_v4] &&
		!ctx->fnr_rt_commit_pending[IPA_IP_v6]) {
		res = ipa3_counter_remove_hdl(ctx->fnr_counter_hdl);
		if (res) {
			IPA_UT_ERR("failed to release FNR counters %d\n", res);
			if (!ret)
				ret = res;
		} else {
			ctx->fnr_counters_allocated = false;
		}
	}

	return ret;
}

static int ipa_test_hw_stats_suite_teardown(void *priv)
{
	int ret = 0;
	int res;

	IPA_UT_DBG("Start Teardown\n");

	res = ipa_test_hw_stats_cleanup_FnR();
	if (res) {
		IPA_UT_ERR("fail to clean up FNR rules %d\n", res);
		return res;
	}

	if (ctx->odu_cons_pipe_setup) {
		res = ipa_teardown_sys_pipe(ctx->odu_cons_hdl);
		if (res) {
			IPA_UT_ERR("fail to teardown sys pipe %s %d\n",
				ipa_clients_strings[ctx->cons_client], res);
			ret = res;
		} else {
			ctx->odu_cons_pipe_setup = false;
		}
	}

	if (ctx->odu_prod_pipe_setup) {
		res = ipa_teardown_sys_pipe(ctx->odu_prod_hdl);
		if (res) {
			IPA_UT_ERR("fail to teardown sys pipe %s %d\n",
				ipa_clients_strings[ctx->prod_client], res);
			ret = res;
		} else {
			ctx->odu_prod_pipe_setup = false;
		}
	}

	return ret;
}

static void odu_prod_notify(void *priv, enum ipa_dp_evt_type evt,
	unsigned long data)
{
	struct sk_buff *skb = (struct sk_buff *)data;

	switch (evt) {
	case IPA_RECEIVE:
		dev_kfree_skb_any(skb);
		break;
	case IPA_WRITE_DONE:
		atomic_dec(&ctx->odu_pending);
		dev_kfree_skb_any(skb);
		break;
	default:
		IPA_UT_ERR("unexpected evt %d\n", evt);
	}
}
static void odu_cons_notify(void *priv, enum ipa_dp_evt_type evt,
	unsigned long data)
{
	struct sk_buff *skb = (struct sk_buff *)data;
	int ret;

	switch (evt) {
	case IPA_RECEIVE:
		if (atomic_read(&ctx->odu_pending) >= 64)
			msleep(20);
		atomic_inc(&ctx->odu_pending);
		skb_put(skb, 100);
		ret = ipa_tx_dp(ctx->prod_client, skb, NULL);
		while (ret) {
			msleep(100);
			ret = ipa_tx_dp(ctx->prod_client, skb, NULL);
		}
		break;
	case IPA_WRITE_DONE:
		dev_kfree_skb_any(skb);
		break;
	default:
		IPA_UT_ERR("unexpected evt %d\n", evt);
	}
}

static int ipa_test_hw_stats_configure(void *priv)
{
	struct ipa_sys_connect_params odu_prod_params;
	struct ipa_sys_connect_params odu_emb_cons_params;
	int res;

	/* Retain the ODU endpoints before IPA v7.0 for compatibility. */
	if (ipa3_ctx->ipa_hw_type >= IPA_HW_v7_0) {
		ctx->prod_client = IPA_CLIENT_TEST_PROD;
		ctx->cons_client = IPA_CLIENT_IPSEC_APPS_WAN_CONS;
	} else {
		ctx->prod_client = IPA_CLIENT_ODU_PROD;
		ctx->cons_client = IPA_CLIENT_ODU_EMB_CONS;
	}

	if (ctx->cons_client == IPA_CLIENT_IPSEC_APPS_WAN_CONS &&
		ipa_ipsec_enabled()) {
		IPA_UT_ERR("IPSEC active: IPSEC_APPS_WAN_CONS pipe is in use; cannot run\n");
		return -EBUSY;
	}

	/* first connect all additional pipe */
	memset(&odu_prod_params, 0, sizeof(odu_prod_params));
	memset(&odu_emb_cons_params, 0, sizeof(odu_emb_cons_params));

	odu_prod_params.client = ctx->prod_client;
	odu_prod_params.desc_fifo_sz = 0x1000;
	odu_prod_params.priv = NULL;
	odu_prod_params.notify = odu_prod_notify;
	res = ipa_setup_sys_pipe(&odu_prod_params,
		&ctx->odu_prod_hdl);
	if (res) {
		IPA_UT_ERR("fail to setup sys pipe %s %d\n",
			ipa_clients_strings[ctx->prod_client], res);
		return res;
	}
	ctx->odu_prod_pipe_setup = true;

	odu_emb_cons_params.client = ctx->cons_client;
	odu_emb_cons_params.desc_fifo_sz = 0x1000;
	odu_emb_cons_params.priv = NULL;
	odu_emb_cons_params.notify = odu_cons_notify;
	if (ctx->cons_client == IPA_CLIENT_IPSEC_APPS_WAN_CONS) {
		/*
		 * Match the EP config that ipa_ipsec_ep_init_cons() programs in
		 * production so ipa3_wan_rx_pyld_status_hdlr parses correctly.
		 */
		ipa3_configure_ipsec_apps_wan_cons(&odu_emb_cons_params);
	}
	res = ipa_setup_sys_pipe(&odu_emb_cons_params,
		&ctx->odu_cons_hdl);
	if (res) {
		IPA_UT_ERR("fail to setup sys pipe %s %d\n",
			ipa_clients_strings[ctx->cons_client], res);
		ipa_test_hw_stats_suite_teardown(priv);
		return res;
	}
	ctx->odu_cons_pipe_setup = true;

	IPA_UT_INFO("Configured. Please connect USB RNDIS now\n");

	return 0;
}

static int ipa_test_hw_stats_add_FnR(void *priv)
{
	struct ipa_ioc_add_rt_rule_v2 *rt_rule;
	struct ipa_ioc_add_flt_rule_v2 *flt_rule;
	struct ipa_ioc_get_rt_tbl rt_lookup;
	struct ipa_ioc_flt_rt_counter_alloc *counter = NULL;
	struct ipa_ioc_flt_rt_query *query;
	u32 rt4_usb;
	u32 rt6_usb;
	u32 rt4_cons;
	u32 rt6_cons;
	int pyld_size = 0;
	int ret;

	rt_rule = kzalloc(sizeof(*rt_rule), GFP_KERNEL);
	if (!rt_rule) {
		IPA_UT_DBG("no mem\n");
		return -ENOMEM;
	}
	rt_rule->rules = (uint64_t)kzalloc(1 *
		sizeof(struct ipa_rt_rule_add_v2), GFP_KERNEL);
	if (!rt_rule->rules) {
		IPA_UT_DBG("no mem\n");
		ret = -ENOMEM;
		goto free_rt;
	}
	rt_rule->rule_add_size = sizeof(struct ipa_rt_rule_add_v2);

	flt_rule = kzalloc(sizeof(*flt_rule), GFP_KERNEL);
	if (!flt_rule) {
		IPA_UT_DBG("no mem\n");
		ret = -ENOMEM;
		goto free_rt;
	}
	flt_rule->rules = (uint64_t)kzalloc(1 *
		sizeof(struct ipa_flt_rule_add_v2), GFP_KERNEL);
	if (!flt_rule->rules) {
		ret = -ENOMEM;
		goto free_flt;
	}
	flt_rule->flt_rule_size = sizeof(struct ipa_flt_rule_add_v2);

	counter = kzalloc(sizeof(struct ipa_ioc_flt_rt_counter_alloc),
					  GFP_KERNEL);
	if (!counter) {
		ret = -ENOMEM;
		goto free_flt;
	}
	counter->hw_counter.num_counters = IPA_TEST_HW_STATS_COUNTER_MAX;
	counter->sw_counter.num_counters = 1;

	/* allocate counters */
	ret = ipa3_alloc_counter_id(counter);
	if (ret < 0) {
		IPA_UT_DBG("ipa3_alloc_counter_id fails\n");
		ret = -ENOMEM;
		goto free_counter;
	}
	ctx->hw_counter_start_id = counter->hw_counter.start_id;
	ctx->fnr_counter_hdl = counter->hdl;
	ctx->fnr_counters_allocated = true;

	/* initially clean all allocated counters */
	query = kzalloc(sizeof(struct ipa_ioc_flt_rt_query),
		GFP_KERNEL);
	if (!query) {
		ret = -ENOMEM;
		ipa_test_hw_stats_cleanup_FnR();
		goto free_counter;
	}
	query->start_id = counter->hw_counter.start_id;
	query->end_id = counter->hw_counter.start_id +
		counter->hw_counter.num_counters - 1;
	query->reset = true;
	query->stats_size = sizeof(struct ipa_flt_rt_stats);
	pyld_size = IPA_MAX_FLT_RT_CNT_INDEX *
		sizeof(struct ipa_flt_rt_stats);
	query->stats = (uint64_t)kzalloc(pyld_size, GFP_KERNEL);
	if (!query->stats) {
		ret = -ENOMEM;
		goto free_query;
	}
	ipa_get_flt_rt_stats(query);

	query->start_id = counter->sw_counter.start_id;
	query->end_id = counter->sw_counter.start_id +
		counter->sw_counter.num_counters - 1;
	query->reset = true;
	query->stats_size = sizeof(struct ipa_flt_rt_stats);
	ipa_get_flt_rt_stats(query);

	rt_rule->commit = 1;
	rt_rule->ip = IPA_IP_v4;
	rt_lookup.ip = rt_rule->ip;
	strscpy(rt_rule->rt_tbl_name, "V4_RT_TO_USB_CONS",
		IPA_RESOURCE_NAME_MAX);
	strscpy(rt_lookup.name, rt_rule->rt_tbl_name, IPA_RESOURCE_NAME_MAX);
	rt_rule->num_rules = 1;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.dst = IPA_CLIENT_USB_CONS;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.hashable = true;
	IPA_UT_INFO("rt4_usb_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_USB));
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_USB);
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.enable_stats = true;
	ret = ipa_test_hw_stats_add_rt_rule(rt_rule,
		&ctx->rt4_usb_rule_hdl, &ctx->rt4_usb_rule_added);
	if (ret)
		goto free_query;
	if (ipa3_get_rt_tbl(&rt_lookup)) {
		IPA_UT_ERR("failed to query V4 rules\n");
		ret = -EFAULT;
		goto free_query;
	}
	rt4_usb = rt_lookup.hdl;

	rt_rule->commit = 1;
	rt_rule->ip = IPA_IP_v6;
	rt_lookup.ip = rt_rule->ip;
	strscpy(rt_rule->rt_tbl_name, "V6_RT_TO_USB_CONS",
		IPA_RESOURCE_NAME_MAX);
	strscpy(rt_lookup.name, rt_rule->rt_tbl_name, IPA_RESOURCE_NAME_MAX);
	rt_rule->num_rules = 1;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.dst = IPA_CLIENT_USB_CONS;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.hashable = true;
	IPA_UT_INFO("rt6_usb_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT6_USB));
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT6_USB);
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.enable_stats = true;
	ret = ipa_test_hw_stats_add_rt_rule(rt_rule,
		&ctx->rt6_usb_rule_hdl, &ctx->rt6_usb_rule_added);
	if (ret)
		goto free_query;
	if (ipa3_get_rt_tbl(&rt_lookup)) {
		IPA_UT_ERR("failed to query V4 rules\n");
		ret = -EFAULT;
		goto free_query;
	}
	rt6_usb = rt_lookup.hdl;

	rt_rule->commit = 1;
	rt_rule->ip = IPA_IP_v4;
	rt_lookup.ip = rt_rule->ip;
	if (ctx->cons_client == IPA_CLIENT_IPSEC_APPS_WAN_CONS)
		strscpy(rt_rule->rt_tbl_name, "V4_RT_TO_IPSEC_WAN",
			IPA_RESOURCE_NAME_MAX);
	else
		strscpy(rt_rule->rt_tbl_name, "V4_RT_TO_ODU_CONS",
			IPA_RESOURCE_NAME_MAX);
	strscpy(rt_lookup.name, rt_rule->rt_tbl_name, IPA_RESOURCE_NAME_MAX);
	rt_rule->num_rules = 1;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.dst = ctx->cons_client;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.hashable = true;
	IPA_UT_INFO("rt4_odu_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_CONS));
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_CONS);
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.enable_stats = true;
	ret = ipa_test_hw_stats_add_rt_rule(rt_rule,
		&ctx->rt4_cons_rule_hdl, &ctx->rt4_cons_rule_added);
	if (ret)
		goto free_query;
	if (ipa3_get_rt_tbl(&rt_lookup)) {
		IPA_UT_ERR("failed to query V4 rules\n");
		ret = -EFAULT;
		goto free_query;
	}
	rt4_cons = rt_lookup.hdl;

	rt_rule->commit = 1;
	rt_rule->ip = IPA_IP_v6;
	rt_lookup.ip = rt_rule->ip;
	if (ctx->cons_client == IPA_CLIENT_IPSEC_APPS_WAN_CONS)
		strscpy(rt_rule->rt_tbl_name, "V6_RT_TO_IPSEC_WAN",
			IPA_RESOURCE_NAME_MAX);
	else
		strscpy(rt_rule->rt_tbl_name, "V6_RT_TO_ODU_CONS",
			IPA_RESOURCE_NAME_MAX);
	strscpy(rt_lookup.name, rt_rule->rt_tbl_name, IPA_RESOURCE_NAME_MAX);
	rt_rule->num_rules = 1;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.dst = ctx->cons_client;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.hashable = true;
	IPA_UT_INFO("rt6_odu_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT6_CONS));
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT6_CONS);
	((struct ipa_rt_rule_add_v2 *)
	rt_rule->rules)[0].rule.enable_stats = true;
	ret = ipa_test_hw_stats_add_rt_rule(rt_rule,
		&ctx->rt6_cons_rule_hdl, &ctx->rt6_cons_rule_added);
	if (ret)
		goto free_query;
	if (ipa3_get_rt_tbl(&rt_lookup)) {
		IPA_UT_ERR("failed to query V4 rules\n");
		ret = -EFAULT;
		goto free_query;
	}
	rt6_cons = rt_lookup.hdl;

	flt_rule->commit = 1;
	flt_rule->ip = IPA_IP_v4;
	flt_rule->ep = IPA_CLIENT_USB_PROD;
	flt_rule->num_rules = 1;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].at_rear = 0;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.action = IPA_PASS_TO_ROUTING;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rt_tbl_hdl = rt4_cons;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.hashable = 1;
	IPA_UT_INFO("flt4_usb_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT4_USB));
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT4_USB);
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.enable_stats = true;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
	ret = ipa_test_hw_stats_add_flt_rule(flt_rule,
		&ctx->flt4_usb_rule_hdl, &ctx->flt4_usb_rule_added);
	if (ret)
		goto free_query;

	flt_rule->commit = 1;
	flt_rule->ip = IPA_IP_v6;
	flt_rule->ep = IPA_CLIENT_USB_PROD;
	flt_rule->num_rules = 1;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].at_rear = 0;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.action = IPA_PASS_TO_ROUTING;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rt_tbl_hdl = rt6_cons;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.hashable = 1;
	IPA_UT_INFO("flt6_usb_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_USB));
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_USB);
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.enable_stats = true;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
	ret = ipa_test_hw_stats_add_flt_rule(flt_rule,
		&ctx->flt6_usb_rule_hdl, &ctx->flt6_usb_rule_added);
	if (ret)
		goto free_query;

	flt_rule->commit = 1;
	flt_rule->ip = IPA_IP_v4;
	flt_rule->ep = ctx->prod_client;
	flt_rule->num_rules = 1;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].at_rear = 0;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.action = IPA_PASS_TO_ROUTING;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rt_tbl_hdl = rt4_usb;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.hashable = 1;
	IPA_UT_INFO("flt4_odu_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT4_PROD));
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT4_PROD);
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.enable_stats = true;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
	ret = ipa_test_hw_stats_add_flt_rule(flt_rule,
		&ctx->flt4_prod_rule_hdl, &ctx->flt4_prod_rule_added);
	if (ret)
		goto free_query;

	flt_rule->commit = 1;
	flt_rule->ip = IPA_IP_v6;
	flt_rule->ep = ctx->prod_client;
	flt_rule->num_rules = 1;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].at_rear = 0;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.action = IPA_PASS_TO_ROUTING;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.attrib_mask = IPA_FLT_DST_PORT;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.attrib.dst_port = 5002;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rt_tbl_hdl = rt6_usb;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.hashable = 1;
	IPA_UT_INFO("flt6_odu_cnt_id %u\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_PROD));
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.cnt_idx =
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_PROD);
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.enable_stats = true;
	((struct ipa_flt_rule_add_v2 *)
	flt_rule->rules)[0].rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
	ret = ipa_test_hw_stats_add_flt_rule(flt_rule,
		&ctx->flt6_prod_rule_hdl, &ctx->flt6_prod_rule_added);
	if (ret)
		goto free_query;
	IPA_UT_INFO(
		"Rules added. Please start data transfer on ports 5001/5002\n");
	ret = 0;

free_query:
	if (ret)
		ipa_test_hw_stats_cleanup_FnR();
	kfree((void *)(query->stats));
	kfree(query);
free_counter:
	kfree(counter);
free_flt:
	kfree((void *)(flt_rule->rules));
	kfree(flt_rule);
free_rt:
	kfree((void *)(rt_rule->rules));
	kfree(rt_rule);
	return ret;
}

static int ipa_test_hw_stats_query_FnR_one_by_one(void *priv)
{
	int ret;
	struct ipa_ioc_flt_rt_query *query;
	int pyld_size = 0;

	query = kzalloc(sizeof(struct ipa_ioc_flt_rt_query), GFP_KERNEL);
	if (!query)
		return -ENOMEM;
	pyld_size = IPA_MAX_FLT_RT_CNT_INDEX *
		sizeof(struct ipa_flt_rt_stats);
	query->stats = (uint64_t)kzalloc(pyld_size, GFP_KERNEL);
	if (!query->stats) {
		kfree(query);
		return -ENOMEM;
	}
	/* query 1 by 1 */
	IPA_UT_INFO("========query 1 by 1========\n");
	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT4_USB);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT4_USB);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"usb v4 route counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_USB),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT6_USB);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT6_USB);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"usb v6 route counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT6_USB),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT4_CONS);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT4_CONS);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"odu v4 route counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_CONS),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT6_CONS);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT6_CONS);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"odu v6 route counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT6_CONS),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT4_USB);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT4_USB);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"usb v4 filter counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT4_USB),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT6_USB);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT6_USB);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"usb v6 filter counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_USB),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT4_PROD);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT4_PROD);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"odu v4 filter counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT4_PROD),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);

	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT6_PROD);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT6_PROD);
	ipa_get_flt_rt_stats(query);
	IPA_UT_INFO(
		"odu v6 filter counter %u pkt_cnt %u bytes cnt %llu\n",
		ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_PROD),
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_pkts,
		((struct ipa_flt_rt_stats *)
		query->stats)[0].num_bytes);
	IPA_UT_INFO("================ done ============\n");

	ret = 0;
	kfree(query);
	return ret;
}

static int ipa_test_hw_stats_query_FnR_one_shot(void *priv)
{
	int ret, i, start = 0;
	struct ipa_ioc_flt_rt_query *query;
	int pyld_size = 0;

	query = kzalloc(sizeof(struct ipa_ioc_flt_rt_query), GFP_KERNEL);
	if (!query)
		return -ENOMEM;
	pyld_size = IPA_MAX_FLT_RT_CNT_INDEX *
		sizeof(struct ipa_flt_rt_stats);
	query->stats = (uint64_t)kzalloc(pyld_size, GFP_KERNEL);
	if (!query->stats) {
		kfree(query);
		return -ENOMEM;
	}

	/* query all together */
	IPA_UT_INFO("========query all together========\n");
	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT4_USB);
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT6_PROD);
	ipa_get_flt_rt_stats(query);
	start = 0;
	for (i = ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_USB);
		i <= ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_PROD);
		i++) {
		IPA_UT_INFO(
			"counter %u pkt_cnt %u bytes cnt %llu\n",
			i, ((struct ipa_flt_rt_stats *)
			query->stats)[start].num_pkts,
			((struct ipa_flt_rt_stats *)
			query->stats)[start].num_bytes);
		start++;
	}
	IPA_UT_INFO("================ done ============\n");

	ret = 0;
	kfree((void *)(query->stats));
	kfree(query);
	return ret;
}

static int ipa_test_hw_stats_query_FnR_clean(void *priv)
{
	int ret, i, start = 0;
	struct ipa_ioc_flt_rt_query *query;
	int pyld_size = 0;

	query = kzalloc(sizeof(struct ipa_ioc_flt_rt_query), GFP_KERNEL);
	if (!query) {
		IPA_UT_DBG("no mem\n");
		return -ENOMEM;
	}
	pyld_size = IPA_MAX_FLT_RT_CNT_INDEX *
		sizeof(struct ipa_flt_rt_stats);
	query->stats = (uint64_t)kzalloc(pyld_size, GFP_KERNEL);
	if (!query->stats) {
		kfree(query);
		return -ENOMEM;
	}

	/* query and reset */
	IPA_UT_INFO("========query and reset========\n");
	query->start_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_RT4_USB);
	query->reset = true;
	query->end_id = ipa_test_hw_stats_counter_id(
		IPA_TEST_HW_STATS_FLT6_PROD);
	start = 0;
	ipa_get_flt_rt_stats(query);
	for (i = ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_RT4_USB);
		i <= ipa_test_hw_stats_counter_id(IPA_TEST_HW_STATS_FLT6_PROD);
		i++) {
		IPA_UT_INFO(
			"counter %u pkt_cnt %u bytes cnt %llu\n",
			i, ((struct ipa_flt_rt_stats *)
			query->stats)[start].num_pkts,
			((struct ipa_flt_rt_stats *)
			query->stats)[start].num_bytes);
		start++;
	}
	IPA_UT_INFO("================ done ============\n");

	ret = 0;
	kfree((void *)(query->stats));
	kfree(query);
	return ret;
}


static int ipa_test_hw_stats_query_sw_stats(void *priv)
{
	int ret, i, start = 0;
	struct ipa_ioc_flt_rt_query *query;
	int pyld_size = 0;

	query = kzalloc(sizeof(struct ipa_ioc_flt_rt_query), GFP_KERNEL);
	if (!query)
		return -ENOMEM;
	pyld_size = IPA_MAX_FLT_RT_CNT_INDEX *
		sizeof(struct ipa_flt_rt_stats);
	query->stats = (uint64_t)kzalloc(pyld_size, GFP_KERNEL);
	if (!query->stats) {
		kfree(query);
		return -ENOMEM;
	}

	/* query all together */
	IPA_UT_INFO("========query all SW counters========\n");
	query->start_id = IPA_FLT_RT_HW_COUNTER + 1;
	query->end_id = IPA_MAX_FLT_RT_CNT_INDEX;
	ipa_get_flt_rt_stats(query);
	start = 0;
	for (i = IPA_FLT_RT_HW_COUNTER + 1;
		i <= IPA_MAX_FLT_RT_CNT_INDEX; i++) {
		IPA_UT_INFO(
			"counter %u pkt_cnt %u bytes cnt %llu\n",
			i, ((struct ipa_flt_rt_stats *)
			query->stats)[start].num_pkts,
			((struct ipa_flt_rt_stats *)
			query->stats)[start].num_bytes);
		start++;
	}
	IPA_UT_INFO("================ done ============\n");

	ret = 0;
	kfree((void *)(query->stats));
	kfree(query);
	return ret;
}

static int ipa_test_hw_stats_set_sw_stats(void *priv)
{
	int i, start = 0;
	struct ipa_flt_rt_stats stats;

	/* set sw counters */
	IPA_UT_INFO("========set all SW counters========\n");
	for (i = IPA_FLT_RT_HW_COUNTER + 1;
		i <= IPA_MAX_FLT_RT_CNT_INDEX; i++) {
		stats.num_bytes = start;
		stats.num_pkts_hash = start + 10;
		stats.num_pkts = start + 100;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			i, stats.num_pkts, stats.num_bytes);
		ipa_set_flt_rt_stats(i, stats);
		start++;
	}
	IPA_UT_INFO("================ done ============\n");

	return 0;
}

static int ipa_test_hw_stats_query_drop_stats(void *priv)
{
       int ret, i, ep_idx, reg_idx;
       struct ipa_drop_stats_all *query;

       query = kzalloc(sizeof(struct ipa_drop_stats_all), GFP_KERNEL);
       if (!query)
               return -ENOMEM;

       IPA_UT_INFO("========query all drop stats========\n");

       ret = ipa_get_drop_stats(query);
       if (!ret)
               goto fail;

       for (i = 0; i <= IPA_CLIENT_MAX; i++) {
               ep_idx = ipa_get_ep_mapping(i);
               if (ep_idx == -1 || !IPA_CLIENT_IS_CONS(i) || IPA_CLIENT_IS_TEST(i))
                       continue;

               reg_idx = ipahal_get_ep_reg_idx(ep_idx);
               if (!(ipa3_ctx->hw_stats->drop.init.enabled_bitmask[reg_idx] &
                       ipahal_get_ep_bit(ep_idx)))
                       continue;

               IPA_UT_INFO("Client %u pkt_cnt %u bytes cnt %u\n", i,
                       query->client[i].drop_packet_cnt, query->client[i].drop_byte_cnt);
       }

       IPA_UT_INFO("================ done ============\n");

fail:
       kfree(query);
       return ret;
}

static int ipa_test_hw_stats_reset_all_drop_stats(void *priv)
{
       int ret;

       IPA_UT_INFO("========reset all drop stats========\n");

       ret = ipa_reset_all_drop_stats();
       if (ret)
               IPA_UT_ERR("ipa_reset_all_drop_stats failed %d\n", ret);

       IPA_UT_INFO("================ done ============\n");

       return ret;
}

static int ipa_test_hw_stats_query_teth_stats(void *priv)
{
       int i, j, prod_reg, cons_reg;
       int res;
       struct ipa_quota_stats_all *stats;

       stats = kzalloc(sizeof(*stats), GFP_KERNEL);
       if (!stats)
               return -ENOMEM;

       IPA_UT_INFO("========get all tethering stats========\n");
       res = ipa_get_teth_stats();
       if (res) {
               IPA_UT_ERR("ipa_get_teth_stats failed with code %u\n", res);
               goto teardown;
       }

       for (i = 0; i < IPA_CLIENT_MAX; i++) {
               int ep_idx = ipa_get_ep_mapping(i);

               if (ep_idx == IPA_EP_NOT_ALLOCATED)
                       continue;

               if (!IPA_CLIENT_IS_PROD(i))
                       continue;

               if (IPA_CLIENT_IS_TEST(i))
                       continue;

               prod_reg = ipahal_get_ep_reg_idx(ep_idx);
               if (!(ipa3_ctx->hw_stats->teth.init.prod_bitmask[prod_reg] &
                       ipahal_get_ep_bit(ep_idx)))
                       continue;

               res = ipa_query_teth_stats(i, stats, false);
               if (res) {
                       IPA_UT_ERR("ipa_query_teth_stats failed with code %u\n", res);
                       goto teardown;
               }

               for (j = 0; j < IPA_CLIENT_MAX; j++) {
                       int cons_idx = ipa_get_ep_mapping(j);

                       if (cons_idx == IPA_EP_NOT_ALLOCATED)
                               continue;

                       if (IPA_CLIENT_IS_TEST(j))
                               continue;

                       cons_reg = ipahal_get_ep_reg_idx(j);
                       if (!(ipa3_ctx->hw_stats->teth.init.
                               cons_bitmask[ep_idx][cons_reg]
                               & ipahal_get_ep_bit(cons_idx)))
                               continue;

                       IPA_UT_INFO("%s->%s:\n", ipa_clients_strings[i],
                               ipa_clients_strings[j]);
		       if(j < ipa3_get_max_num_pipes()){
                       	IPA_UT_INFO("num_ipv4_bytes=%llu\n",
                                               stats->client[j].num_ipv4_bytes);
                       	IPA_UT_INFO("num_ipv6_bytes=%llu\n",
                                               stats->client[j].num_ipv6_bytes);
                       	IPA_UT_INFO("num_ipv4_pkts=%u\n",
                                               stats->client[j].num_ipv4_pkts);
                       	IPA_UT_INFO("num_ipv6_pkts=%u\n",
                                               stats->client[j].num_ipv6_pkts);
		       }
               }
       }

       IPA_UT_INFO("================ done ============\n");

teardown:
       kfree(stats);
       return res;
}

static int ipa_test_hw_stats_reset_teth_stats(void *priv)
{
	int ret;
	unsigned int random_int;
	enum ipa_client_type prod;
	enum ipa_client_type cons;

	get_random_bytes(&random_int, sizeof(random_int));
	prod = (random_int % IPA_CLIENT_MAX) & 0x8FFFFFFE;

	get_random_bytes(&random_int, sizeof(random_int));
	cons = ((random_int % IPA_CLIENT_MAX) & 0x8FFFFFFF) | 0x1;

	IPA_UT_INFO("========reset some tethering stats========\n");

	ret = ipa_reset_teth_stats(prod, cons);
	if (ret)
		IPA_UT_ERR("ipa_reset_teth_stats failed %d\n", ret);

	IPA_UT_INFO("================ done ============\n");

	return ret;
}

static int ipa_test_hw_stats_query_quota_stats(void *priv)
{
       struct ipa_quota_stats_all *out;
       int i, reg_idx, ep_idx;
       int res;

       out = kzalloc(sizeof(*out), GFP_KERNEL);
       if (!out)
               return -ENOMEM;

       IPA_UT_INFO("========get all quota stats========\n");

       res = ipa_get_quota_stats(out);
       if (res) {
               IPA_UT_ERR("ipa_get_quota_stats failed with code %u\n", res);
               goto teardown;
       }

       for (i = 0; i < IPA_CLIENT_MAX; i++) {
               ep_idx = ipa_get_ep_mapping(i);

               if (ep_idx == IPA_EP_NOT_ALLOCATED)
                       continue;

               if (IPA_CLIENT_IS_TEST(i))
                       continue;

               reg_idx = ipahal_get_ep_reg_idx(ep_idx);
               if (!(ipa3_ctx->hw_stats->quota.init.enabled_bitmask[reg_idx] &
                       ipahal_get_ep_bit(ep_idx)))
                       continue;

               IPA_UT_INFO("%s:\n", ipa_clients_strings[i]);
	       if(i < ipa3_get_max_num_pipes()){
               	IPA_UT_INFO("num_ipv4_bytes=%llu\n", out->client[i].num_ipv4_bytes);
               	IPA_UT_INFO("num_ipv6_bytes=%llu\n", out->client[i].num_ipv6_bytes);
               	IPA_UT_INFO("num_ipv4_pkts=%u\n", out->client[i].num_ipv4_pkts);
               	IPA_UT_INFO("num_ipv6_pkts=%u\n", out->client[i].num_ipv6_pkts);
	       }

       }

       IPA_UT_INFO("================ done ============\n");

teardown:
       kfree(out);
       return res;
}

static int ipa_test_hw_stats_reset_all_quota_stats(void *priv)
{
       int ret;

       IPA_UT_INFO("========reset all drop stats========\n");

       ret = ipa_reset_all_quota_stats();
       if (ret)
               IPA_UT_ERR("ipa_reset_all_quota_stats failed %d\n", ret);

       IPA_UT_INFO("================ done ============\n");

       return ret;
}

static int ipa_test_hw_stats_set_uc_event_ring(void *priv)
{
	struct ipa_ioc_flt_rt_counter_alloc *counter = NULL;
	int ret = 0;

	/* set uc event ring */
	IPA_UT_INFO("========set uc event ring ========\n");

	if (ipa3_ctx->ipa_hw_type >= IPA_HW_v4_5 &&
		ipa3_ctx->ipa_hw_type != IPA_HW_v4_11 &&
		ipa3_ctx->ipa_hw_type != IPA_HW_v5_2) {
		if (ipa3_ctx->uc_ctx.uc_loaded &&
			!ipa3_ctx->uc_ctx.uc_event_ring_valid) {
			if (ipa3_uc_setup_event_ring()) {
				IPA_UT_ERR("failed to set uc_event ring\n");
				return -EFAULT;
			}
		} else
			IPA_UT_ERR("uc-loaded %d, ring-valid %d",
				ipa3_ctx->uc_ctx.uc_loaded,
				ipa3_ctx->uc_ctx.uc_event_ring_valid);
	}
	IPA_UT_INFO("================ done ============\n");

	/* allocate counters */
	IPA_UT_INFO("========set hw counter ========\n");

	counter = kzalloc(sizeof(struct ipa_ioc_flt_rt_counter_alloc),
					  GFP_KERNEL);
	if (!counter)
		return -ENOMEM;

	counter->hw_counter.num_counters = 4;
	counter->sw_counter.num_counters = 4;

	/* allocate counters */
	ret = ipa3_alloc_counter_id(counter);
	if (ret < 0) {
		IPA_UT_ERR("ipa3_alloc_counter_id fails\n");
		ret = -ENOMEM;
	}
	ipa3_ctx->fnr_info.hw_counter_offset = counter->hw_counter.start_id;
	ipa3_ctx->fnr_info.sw_counter_offset = counter->sw_counter.start_id;
	IPA_UT_INFO("hw-offset %d, sw-offset %d\n",
		ipa3_ctx->fnr_info.hw_counter_offset,
			ipa3_ctx->fnr_info.sw_counter_offset);

	kfree(counter);
	return ret;
}

static int ipa_test_hw_stats_set_quota(void *priv)
{
	int ret;
	uint64_t quota = 500;

	IPA_UT_INFO("========set quota ========\n");
	ret = ipa3_uc_quota_monitor(quota);
	if (ret < 0) {
		IPA_UT_ERR("ipa3_uc_quota_monitor fails\n");
		ret = -ENOMEM;
	}
	IPA_UT_INFO("================ done ============\n");
	return ret;
}

static int ipa_test_hw_stats_set_bw(void *priv)
{
	int ret;
	struct ipa_wdi_bw_info *info = NULL;

	IPA_UT_INFO("========set BW voting ========\n");
	info = kzalloc(sizeof(struct ipa_wdi_bw_info),
					  GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->num = 3;
	info->threshold[0] = 200;
	info->threshold[1] = 400;
	info->threshold[2] = 600;

	ret = ipa_uc_bw_monitor(info);
	if (ret < 0) {
		IPA_UT_ERR("ipa_uc_bw_monitor fails\n");
		ret = -ENOMEM;
	}

	IPA_UT_INFO("================ done ============\n");

	kfree(info);
	return ret;
}
static int ipa_test_hw_stats_hit_quota(void *priv)
{
	int ret = 0, counter_index, i;
	struct ipa_flt_rt_stats stats;

	/* set sw counters */
	IPA_UT_INFO("========set quota hit========\n");
	counter_index = ipa3_ctx->fnr_info.sw_counter_offset + UL_WLAN_TX;

	for (i = 0; i < 5; i++) {
		IPA_UT_INFO("========set 100 ========\n");
		memset(&stats, 0, sizeof(struct ipa_flt_rt_stats));
		stats.num_bytes = 100;
		stats.num_pkts = 69;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			counter_index, stats.num_pkts, stats.num_bytes);
		ret = ipa_set_flt_rt_stats(counter_index, stats);
		if (ret < 0) {
			IPA_UT_ERR("ipa_set_flt_rt_stats fails\n");
			ret = -ENOMEM;
		}
		msleep(1000);
		IPA_UT_INFO("========set 200 ========\n");
		memset(&stats, 0, sizeof(struct ipa_flt_rt_stats));
		stats.num_bytes = 200;
		stats.num_pkts = 69;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			counter_index, stats.num_pkts, stats.num_bytes);
		ret = ipa_set_flt_rt_stats(counter_index, stats);
		if (ret < 0) {
			IPA_UT_ERR("ipa_set_flt_rt_stats fails\n");
			ret = -ENOMEM;
		}
		msleep(1000);
		IPA_UT_INFO("========set 300 ========\n");
		memset(&stats, 0, sizeof(struct ipa_flt_rt_stats));
		stats.num_bytes = 300;
		stats.num_pkts = 69;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			counter_index, stats.num_pkts, stats.num_bytes);
		ret = ipa_set_flt_rt_stats(counter_index, stats);
		if (ret < 0) {
			IPA_UT_ERR("ipa_set_flt_rt_stats fails\n");
			ret = -ENOMEM;
		}
		msleep(1000);
		IPA_UT_INFO("========set 500 ========\n");
		memset(&stats, 0, sizeof(struct ipa_flt_rt_stats));
		stats.num_bytes = 500;
		stats.num_pkts = 69;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			counter_index, stats.num_pkts, stats.num_bytes);
		ret = ipa_set_flt_rt_stats(counter_index, stats);
		if (ret < 0) {
			IPA_UT_ERR("ipa_set_flt_rt_stats fails\n");
			ret = -ENOMEM;
		}
		msleep(1000);
		IPA_UT_INFO("========set 600 ========\n");
		memset(&stats, 0, sizeof(struct ipa_flt_rt_stats));
		stats.num_bytes = 600;
		stats.num_pkts = 69;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			counter_index, stats.num_pkts, stats.num_bytes);
		ret = ipa_set_flt_rt_stats(counter_index, stats);
		if (ret < 0) {
			IPA_UT_ERR("ipa_set_flt_rt_stats fails\n");
			ret = -ENOMEM;
		}
		msleep(1000);
		IPA_UT_INFO("========set 1000 ========\n");
		memset(&stats, 0, sizeof(struct ipa_flt_rt_stats));
		stats.num_bytes = 1000;
		stats.num_pkts = 69;
		IPA_UT_INFO(
			"set counter %u pkt_cnt %u bytes cnt %llu\n",
			counter_index, stats.num_pkts, stats.num_bytes);
		ret = ipa_set_flt_rt_stats(counter_index, stats);
		if (ret < 0) {
			IPA_UT_ERR("ipa_set_flt_rt_stats fails\n");
			ret = -ENOMEM;
		}
	}
	IPA_UT_INFO("================ done ============\n");
	return ret;
}



/* Suite definition block */
IPA_UT_DEFINE_SUITE_START(hw_stats, "HW stats test",
	ipa_test_hw_stats_suite_setup, ipa_test_hw_stats_suite_teardown)
{
	IPA_UT_ADD_TEST(configure, "Configure the setup",
		ipa_test_hw_stats_configure, false, IPA_HW_v4_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(add_rules, "Add FLT and RT rules",
		ipa_test_hw_stats_add_FnR, false, IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_stats_one_by_one, "Query one by one",
		ipa_test_hw_stats_query_FnR_one_by_one, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_stats_one_shot, "Query one shot",
		ipa_test_hw_stats_query_FnR_one_shot, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_stats_one_shot_clean, "Query and clean",
		ipa_test_hw_stats_query_FnR_clean, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_sw_stats, "Query SW stats",
		ipa_test_hw_stats_query_sw_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(set_sw_stats, "Set SW stats to dummy values",
		ipa_test_hw_stats_set_sw_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_drop_stats, "Query drop stats",
		ipa_test_hw_stats_query_drop_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(reset_drop_stats, "Reset drop stats",
		ipa_test_hw_stats_reset_all_drop_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_teth_stats, "Query tethering stats",
		ipa_test_hw_stats_query_teth_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(reset_teth_stats, "Reset tethering stats",
		ipa_test_hw_stats_reset_teth_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(query_quota_stats, "Query quota stats",
		ipa_test_hw_stats_query_quota_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(reset_quota_stats, "Reset quota stats",
		ipa_test_hw_stats_reset_all_quota_stats, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(set_uc_evtring, "Set uc event ring",
		ipa_test_hw_stats_set_uc_event_ring, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(set_quota, "Set quota",
		ipa_test_hw_stats_set_quota, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(set_bw_voting, "Set bw_voting",
		ipa_test_hw_stats_set_bw, false,
		IPA_HW_v4_5, IPA_HW_MAX),

	IPA_UT_ADD_TEST(hit_quota, "quota hits",
		ipa_test_hw_stats_hit_quota, false,
		IPA_HW_v4_5, IPA_HW_MAX),

} IPA_UT_DEFINE_SUITE_END(hw_stats);
