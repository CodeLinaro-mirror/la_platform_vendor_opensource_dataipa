// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include "ipa_ut_framework.h"
#include <linux/ipa_fse.h>

/* Forward declare to avoid missing headers in the test file */
int ipa_be_fse_rule_create(struct ipa_fse_rule *finfo);
int ipa_be_fse_rule_destroy(struct ipa_fse_rule *finfo);

static bool test_fse_create_called = false;
static bool test_fse_destroy_called = false;
static struct ipa_fse_rule test_fse_rule_created;
static struct ipa_fse_rule test_fse_rule_destroyed;

static int test_fse_create_rule(struct ipa_fse_rule *rule)
{
	test_fse_create_called = true;
	if (rule)
		memcpy(&test_fse_rule_created, rule, sizeof(*rule));
	return 0;
}

static int test_fse_destroy_rule(struct ipa_fse_rule *rule)
{
	test_fse_destroy_called = true;
	if (rule)
		memcpy(&test_fse_rule_destroyed, rule, sizeof(*rule));
	return 0;
}

static struct ipa_fse_ops test_fse_ops = {
	.create_fse_rule = test_fse_create_rule,
	.destroy_fse_rule = test_fse_destroy_rule,
};

static int ipa_test_fse_add_rule(void *priv)
{
	struct ipa_fse_rule rule;
	int ret;

	memset(&rule, 0, sizeof(rule));
	rule.tuple.protocol = 17; /* UDP */
	rule.tuple.src_port = 4321;
	rule.tuple.dst_port = 8765;
	rule.flags = IPA_FSE_IPV4;
	rule.tuple.ip.v4.src_ip = 0x08080808;
	rule.tuple.ip.v4.dst_ip = 0x04040404;
	rule.rdi = 8;

	/*
	 * In this test, we assume the WLAN driver has registered the FSE callbacks
	 * with the IPA backend driver. We attempt to add a rule.
	 */
	ret = ipa_be_fse_rule_create(&rule);
	if (ret) {
		IPA_UT_ERR("FSE rule creation failed\n");
		return ret;
	}

	IPA_UT_INFO("FSE rule successfully created in WLAN driver\n");
	return 0;
}

static int ipa_test_fse_delete_rule(void *priv)
{
	struct ipa_fse_rule rule;
	int ret;

	memset(&rule, 0, sizeof(rule));
	rule.tuple.protocol = 17; /* UDP */
	rule.tuple.src_port = 4321;
	rule.tuple.dst_port = 8765;
	rule.flags = IPA_FSE_IPV4;
	rule.tuple.ip.v4.src_ip = 0x08080808;
	rule.tuple.ip.v4.dst_ip = 0x04040404;
	rule.rdi = 8;

	/*
	 * In this test, we assume the WLAN driver has registered the FSE callbacks
	 * with the IPA backend driver. We attempt to delete a rule.
	 */
	ret = ipa_be_fse_rule_destroy(&rule);
	if (ret) {
		IPA_UT_ERR("FSE rule destruction failed\n");
		return ret;
	}

	IPA_UT_INFO("FSE rule successfully destroyed in WLAN driver\n");
	return 0;
}

static int ipa_test_fse_registration(void *priv)
{
	int ret;
	struct ipa_fse_rule rule;

	memset(&rule, 0, sizeof(rule));
	rule.tuple.protocol = 6; /* TCP */
	rule.tuple.src_port = 1234;
	rule.tuple.dst_port = 5678;
	rule.flags = IPA_FSE_IPV4;
	rule.tuple.ip.v4.src_ip = 0x01020304;
	rule.tuple.ip.v4.dst_ip = 0x05060708;

	/* Reset flags */
	test_fse_create_called = false;
	test_fse_destroy_called = false;

	/* Test before registration - should fail cleanly */
	if (!ipa_be_fse_rule_create(&rule)) {
		IPA_UT_ERR("FSE rule creation succeeded before registration\n");
		return -EFAULT;
	}

	/* Register ops */
	ret = ipa_fse_ops_register(&test_fse_ops);
	if (ret) {
		IPA_UT_ERR("FSE ops registration failed: %d\n", ret);
		return ret;
	}

	/* Test creation */
	if (ipa_be_fse_rule_create(&rule)) {
		IPA_UT_ERR("FSE rule creation failed\n");
		ipa_fse_ops_unregister();
		return -EFAULT;
	}

	if (!test_fse_create_called) {
		IPA_UT_ERR("FSE create callback not called\n");
		ipa_fse_ops_unregister();
		return -EFAULT;
	}

	if (memcmp(&rule, &test_fse_rule_created, sizeof(rule))) {
		IPA_UT_ERR("FSE created rule data mismatch\n");
		ipa_fse_ops_unregister();
		return -EFAULT;
	}

	/* Test destruction */
	if (ipa_be_fse_rule_destroy(&rule)) {
		IPA_UT_ERR("FSE rule destruction failed\n");
		ipa_fse_ops_unregister();
		return -EFAULT;
	}

	if (!test_fse_destroy_called) {
		IPA_UT_ERR("FSE destroy callback not called\n");
		ipa_fse_ops_unregister();
		return -EFAULT;
	}

	if (memcmp(&rule, &test_fse_rule_destroyed, sizeof(rule))) {
		IPA_UT_ERR("FSE destroyed rule data mismatch\n");
		ipa_fse_ops_unregister();
		return -EFAULT;
	}

	/* Unregister ops */
	ipa_fse_ops_unregister();

	/* Test after unregistration - should fail cleanly */
	test_fse_create_called = false;
	if (!ipa_be_fse_rule_create(&rule)) {
		IPA_UT_ERR("FSE rule creation succeeded after unregistration\n");
		return -EFAULT;
	}

	return 0;
}

/* Suite definition block */
IPA_UT_DEFINE_SUITE_START(fse, "FSE tests",
	NULL, NULL)
{
	IPA_UT_ADD_TEST(fse_add_rule,
		"FSE add rule with WLAN driver ops",
		ipa_test_fse_add_rule,
		false, IPA_HW_v3_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(fse_delete_rule,
		"FSE delete rule with WLAN driver ops",
		ipa_test_fse_delete_rule,
		false, IPA_HW_v3_0, IPA_HW_MAX),

	IPA_UT_ADD_TEST(fse_registration,
		"FSE framework registration and callback invocation",
		ipa_test_fse_registration,
		false, IPA_HW_v3_0, IPA_HW_MAX),

} IPA_UT_DEFINE_SUITE_END(fse);
