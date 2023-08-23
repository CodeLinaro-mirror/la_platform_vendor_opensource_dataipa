// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Linaro Ltd.
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/sysfs.h>

#include "ipa_sysfs.h"
#include "ipa_i.h"

static const char *ipa_qmap_string()
{
	return ipa3_ctx->ipa_hw_type < IPA_HW_v4_5 ? "MAPv4" : "MAPv5";
}

static ssize_t rx_offload_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%s\n", ipa_qmap_string());
}

static ssize_t tx_offload_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%s\n", ipa_qmap_string());
}

static DEVICE_ATTR_RO(tx_offload);
static DEVICE_ATTR_RO(rx_offload);

static struct attribute *ipa_feature_attrs[] = {
	&dev_attr_rx_offload.attr,
	&dev_attr_tx_offload.attr,
	NULL
};

const struct attribute_group ipa_feature_attribute_group = {
	.name		= "feature",
	.attrs		= ipa_feature_attrs,
};

static ssize_t ipa_endpoint_id_show(char *buf, enum ipa_client_type client)
{
	u32 endpoint_id = 0;
	const struct ipa_gsi_ep_config *gsi_ep_info = ipa3_get_gsi_ep_info(client);
	if(!gsi_ep_info)
		return -EFAULT;
	endpoint_id = gsi_ep_info->ipa_ep_num;

	return scnprintf(buf, PAGE_SIZE, "%u\n", endpoint_id);
}

static ssize_t rx_endpoint_id_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return ipa_endpoint_id_show(buf, IPA_CLIENT_APPS_WAN_CONS);
}

static ssize_t tx_endpoint_id_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return ipa_endpoint_id_show(buf, IPA_CLIENT_APPS_WAN_PROD);
}

static DEVICE_ATTR_RO(tx_endpoint_id);
static DEVICE_ATTR_RO(rx_endpoint_id);

static struct attribute *ipa_modem_attrs[] = {
	&dev_attr_rx_endpoint_id.attr,
	&dev_attr_tx_endpoint_id.attr,
	NULL
};

const struct attribute_group ipa_modem_attribute_group = {
	.name		= "modem",
	.attrs		= ipa_modem_attrs,
};
