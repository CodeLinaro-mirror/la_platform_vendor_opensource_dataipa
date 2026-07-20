// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include "ipa_be_fse.h"
#include "ipa_be.h"
#include <linux/export.h>
#include <linux/err.h>
#include <linux/spinlock.h>
#include <linux/stddef.h>

static struct ipa_fse_ops *g_ipa_fse_ops;
static bool g_ipa_fse_up;
static DEFINE_SPINLOCK(g_ipa_fse_lock);

int ipa_fse_ops_register(struct ipa_fse_ops *ops)
{
	if (!ops || !ops->create_fse_rule || !ops->destroy_fse_rule)
		return -EINVAL;

	IPA_BE_DBG("FSE ops registered: create=%pS destroy=%pS\n",
		   ops->create_fse_rule, ops->destroy_fse_rule);

	spin_lock(&g_ipa_fse_lock);
	g_ipa_fse_ops = ops;
	g_ipa_fse_up = true;
	spin_unlock(&g_ipa_fse_lock);

	return 0;
}
EXPORT_SYMBOL(ipa_fse_ops_register);

void ipa_fse_ops_unregister(void)
{
	spin_lock(&g_ipa_fse_lock);
	IPA_BE_DBG("FSE ops unregistered (g_ipa_fse_up was %d)\n", g_ipa_fse_up);
	g_ipa_fse_ops = NULL;
	g_ipa_fse_up = false;
	spin_unlock(&g_ipa_fse_lock);
}
EXPORT_SYMBOL(ipa_fse_ops_unregister);

int ipa_be_fse_rule_create(struct ipa_fse_rule *finfo)
{
	struct ipa_fse_ops *ops;
	int ret;

	spin_lock(&g_ipa_fse_lock);
	ops = g_ipa_fse_up ? g_ipa_fse_ops : NULL;
	spin_unlock(&g_ipa_fse_lock);

	if (!ops || !ops->create_fse_rule) {
		IPA_BE_DBG("FSE create skipped: fse_up=%d\n", g_ipa_fse_up);
		return -EPERM;
	}

	if (finfo->flags & IPA_FSE_IPV6)
		IPA_BE_DBG("FSE create IPv6: src=%pI6c:%u dst=%pI6c:%u proto=%u ifidx=%u rdi=%u\n",
			   finfo->tuple.ip.v6.src_ip, finfo->tuple.src_port,
			   finfo->tuple.ip.v6.dst_ip, finfo->tuple.dst_port,
			   finfo->tuple.protocol, finfo->ifindex, finfo->rdi);
	else
		IPA_BE_DBG("FSE create IPv4: src=%pI4h:%u dst=%pI4h:%u proto=%u ifidx=%u rdi=%u\n",
			   &finfo->tuple.ip.v4.src_ip, finfo->tuple.src_port,
			   &finfo->tuple.ip.v4.dst_ip, finfo->tuple.dst_port,
			   finfo->tuple.protocol, finfo->ifindex, finfo->rdi);

	ret = ops->create_fse_rule(finfo);
	return ret;
}
EXPORT_SYMBOL(ipa_be_fse_rule_create);

int ipa_be_fse_rule_destroy(struct ipa_fse_rule *finfo)
{
	struct ipa_fse_ops *ops;
	int ret;

	spin_lock(&g_ipa_fse_lock);
	ops = g_ipa_fse_up ? g_ipa_fse_ops : NULL;
	spin_unlock(&g_ipa_fse_lock);

	if (!ops || !ops->destroy_fse_rule) {
		IPA_BE_DBG("FSE destroy skipped: fse_up=%d\n", g_ipa_fse_up);
		return -EPERM;
	}

	if (finfo->flags & IPA_FSE_IPV6)
		IPA_BE_DBG("FSE destroy IPv6: src=%pI6c:%u dst=%pI6c:%u proto=%u ifidx=%u rdi=%u\n",
			   finfo->tuple.ip.v6.src_ip, finfo->tuple.src_port,
			   finfo->tuple.ip.v6.dst_ip, finfo->tuple.dst_port,
			   finfo->tuple.protocol, finfo->ifindex, finfo->rdi);
	else
		IPA_BE_DBG("FSE destroy IPv4: src=%pI4h:%u dst=%pI4h:%u proto=%u ifidx=%u rdi=%u\n",
			   &finfo->tuple.ip.v4.src_ip, finfo->tuple.src_port,
			   &finfo->tuple.ip.v4.dst_ip, finfo->tuple.dst_port,
			   finfo->tuple.protocol, finfo->ifindex, finfo->rdi);

	ret = ops->destroy_fse_rule(finfo);
	return ret;
}
EXPORT_SYMBOL(ipa_be_fse_rule_destroy);
