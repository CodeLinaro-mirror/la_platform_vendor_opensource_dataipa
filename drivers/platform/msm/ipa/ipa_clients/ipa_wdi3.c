// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include "ipa_wdi3.h"
#include <linux/msm_ipa.h>
#include <linux/string.h>
#include "ipa_common_i.h"
#include "ipa_pm.h"
#include "ipa_i.h"

#define OFFLOAD_DRV_NAME "ipa_wdi"
#define VDEV_ID_MASK 0xFF
#define VDEV_BIT_SHIFT 24
#define IPA_WDI_DBG(fmt, args...) \
	do { \
		pr_debug(OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
	} while (0)

#define IPA_WDI_DBG_LOW(fmt, args...) \
	do { \
		pr_debug(OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
	} while (0)

#define IPA_WDI_ERR(fmt, args...) \
	do { \
		pr_err(OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
	} while (0)

#define IPA_CLIENT_IS_WLAN0_INSTANCE(inst_id) \
	(inst_id == 0 || inst_id == -1)
#define IPA_CLIENT_IS_WLAN1_INSTANCE(inst_id) \
	(inst_id == 1)
#define IPA_CLIENT_IS_WLAN2_INSTANCE(inst_id) \
	(inst_id == 2)

#define DEFAULT_INSTANCE_ID (-1)
#define INVALID_INSTANCE_ID (-2)

struct ipa_wdi_intf_info {
	char netdev_name[IPA_RESOURCE_NAME_MAX];
	u8 hdr_len;
	u32 partial_hdr_hdl[IPA_IP_MAX_WLAN];
	struct list_head link;
};

struct ipa_wdi_context {
	struct list_head head_intf_list;
	struct completion wdi_completion;
	struct mutex lock;
	enum ipa_wdi_version wdi_version;
	enum ipa_client_type tx_client;
	enum ipa_client_type rx_client;
	enum ipa_client_type tx1_client;
	enum ipa_client_type rx1_client;
	u8 is_smmu_enabled;
	u32 tx_pipe_hdl;
	u32 rx_pipe_hdl;
	u8 num_sys_pipe_needed;
	bool is_tx1_used;
	u32 sys_pipe_hdl[IPA_WDI_MAX_SUPPORTED_SYS_PIPE];
	u32 ipa_pm_hdl;
	int inst_id;
#ifdef IPA_WAN_MSG_IPv6_ADDR_GW_LEN
	ipa_wdi_meter_notifier_cb wdi_notify;
#endif
	bool ast_update;
	bool is_rx1_used;
	u8 rdi;
	u8 rx1_rdi;
};

static struct ipa_wdi_context *ipa_wdi_ctx_list[IPA_WDI_INST_MAX];

static inline bool is_single_instance_wdi_version(enum ipa_wdi_version version)
{
	return (version >= IPA_WDI_1 && version < IPA_WDI_3) ||
	       (version == IPA_WDI_6);
}

/**
 * function to Assign Handle for instance
 *
 * Note: If it is called for Old API then
 * max one handle is allowed.
 *
 * @Return handle on success, negative on failure
 */
static int assign_hdl_for_inst(int inst_id)
{
	int hdl;

	IPA_WDI_DBG("Assigning handle for instance id %d\n", inst_id);
	if (inst_id <= INVALID_INSTANCE_ID) {
		IPA_WDI_ERR("Invalid instance id %d\n", inst_id);
		return -EINVAL;
	}
	else if (ipa_wdi_ctx_list[0] && (inst_id == DEFAULT_INSTANCE_ID ||
			ipa_wdi_ctx_list[0]->inst_id == DEFAULT_INSTANCE_ID)) {
		IPA_WDI_ERR("Invalid instance id %d\n", inst_id);
		return -EINVAL;
	}
	else {
		for (hdl = 0; hdl < IPA_WDI_INST_MAX; hdl++) {
			if (!ipa_wdi_ctx_list[hdl])
				break;
		}
	}
	if (hdl == IPA_WDI_INST_MAX) {
		IPA_WDI_ERR("Already Maximum Instance Registered\n");
		return -EINVAL;
	}

	return hdl;
}

int ipa_get_wdi_version(void)
{
	if (ipa_wdi_ctx_list[0])
		return ipa_wdi_ctx_list[0]->wdi_version;
	else if (ipa_wdi_ctx_list[1])
		return ipa_wdi_ctx_list[1]->wdi_version;
	else if (ipa_wdi_ctx_list[2])
		return ipa_wdi_ctx_list[2]->wdi_version;
	else
		/* default version is IPA_WDI_1 */
		return IPA_WDI_1;
}
EXPORT_SYMBOL(ipa_get_wdi_version);

bool ipa_wdi_is_tx1_used(void)
{
	if (ipa_wdi_ctx_list[0])
		return ipa_wdi_ctx_list[0]->is_tx1_used;
	return 0;
}
EXPORT_SYMBOL(ipa_wdi_is_tx1_used);

static void ipa_wdi_pm_cb(void *p, enum ipa_pm_cb_event event)
{
        IPA_WDI_DBG("received pm event %d\n", event);
}

static int ipa_wdi_commit_partial_hdr(
	struct ipa_ioc_add_hdr *hdr,
	const char *netdev_name,
	struct ipa_wdi_hdr_info *hdr_info)
{
	int i;

	if (!hdr || !hdr_info || !netdev_name) {
		IPA_WDI_ERR("Invalid input\n");
		return -EINVAL;
	}

	hdr->commit = 0;

	snprintf(hdr->hdr[0].name, sizeof(hdr->hdr[0].name),
			 "%s_ipv4", netdev_name);
	snprintf(hdr->hdr[1].name, sizeof(hdr->hdr[1].name),
			 "%s_ipv6", netdev_name);
	if (hdr->num_hdrs == 4) {
		snprintf(hdr->hdr[2].name, sizeof(hdr->hdr[2].name),
			 "%s_ipv4_vlan", netdev_name);
		snprintf(hdr->hdr[3].name, sizeof(hdr->hdr[3].name),
			 "%s_ipv6_vlan", netdev_name);
	}
	for (i = 0; i < hdr->num_hdrs; i++) {
		hdr->hdr[i].hdr_len = hdr_info[i].hdr_len;
		memcpy(hdr->hdr[i].hdr, hdr_info[i].hdr, hdr->hdr[i].hdr_len);
		hdr->hdr[i].type = hdr_info[i].hdr_type;
		/* For IPv4, keep the header in SRAM.
		 * It will be used in header proc context for L2L.
		 * TBD: for a better design in Kobuk. */
		hdr->hdr[i].is_partial = (i == 0) ? 0 : 1;
		hdr->hdr[i].is_eth2_ofst_valid = 1;
		hdr->hdr[i].eth2_ofst = hdr_info[i].dst_mac_addr_offset;
	}

	if (ipa_add_hdr(hdr)) {
		IPA_WDI_ERR("fail to add partial headers\n");
		return -EFAULT;
	}

	return 0;
}

/**
 * function to know the WDI capabilities
 *
 * Note: Should not be called from atomic context and only
 * after checking IPA readiness using ipa_register_ipa_ready_cb()
 *
 * @Return 0 on success, negative on failure
 */
int ipa_wdi_get_capabilities(
	struct ipa_wdi_capabilities_out_params *out)
{
	if (out == NULL) {
		IPA_WDI_ERR("invalid params out=%pK\n", out);
		return -EINVAL;
	}

	out->num_of_instances = IPA_WDI_INST_MAX;
	IPA_WDI_DBG("Wdi Capability: %d\n", out->num_of_instances);
	return 0;
}
EXPORT_SYMBOL(ipa_wdi_get_capabilities);

/**
 * function to init WDI IPA offload data path
 *
 * Note: Should not be called from atomic context and only
 * after checking IPA readiness using ipa_register_ipa_ready_cb()
 *
 * @Return 0 on success, negative on failure
 */
int ipa_wdi_init_per_inst(struct ipa_wdi_init_in_params *in,
	struct ipa_wdi_init_out_params *out)
{
	struct ipa_wdi_uc_ready_params uc_ready_params;
	struct ipa_smmu_in_params smmu_in;
	struct ipa_smmu_out_params smmu_out;
	int hdl;

	if (!(in && out)) {
		IPA_WDI_ERR("empty parameters. in=%pK out=%pK\n", in, out);
		return -EINVAL;
	}

	if (in->wdi_version >= IPA_WDI_VER_MAX || in->wdi_version < IPA_WDI_1) {
		IPA_WDI_ERR("wrong wdi version: %d\n", in->wdi_version);
		return -EFAULT;
	}

	hdl = assign_hdl_for_inst(in->inst_id);
	if (hdl < 0) {
		IPA_WDI_ERR("Error assigning hdl\n");
		return hdl;
	}

	IPA_WDI_DBG("Assigned Handle %d\n",hdl);
	ipa_wdi_ctx_list[hdl] = kzalloc(sizeof(struct ipa_wdi_context), GFP_KERNEL);
	if (ipa_wdi_ctx_list[hdl] == NULL) {
		IPA_WDI_ERR("fail to alloc wdi ctx\n");
		return -ENOMEM;
	}
	mutex_init(&ipa_wdi_ctx_list[hdl]->lock);
	init_completion(&ipa_wdi_ctx_list[hdl]->wdi_completion);
	INIT_LIST_HEAD(&ipa_wdi_ctx_list[hdl]->head_intf_list);

	ipa_wdi_ctx_list[hdl]->inst_id = in->inst_id;
	ipa_wdi_ctx_list[hdl]->wdi_version = in->wdi_version;
	ipa_wdi_ctx_list[hdl]->ast_update = in->ast_update;
	ipa_wdi_ctx_list[hdl]->tx_client = IPA_EP_NOT_ALLOCATED;
	ipa_wdi_ctx_list[hdl]->rx_client = IPA_EP_NOT_ALLOCATED;
	ipa_wdi_ctx_list[hdl]->tx1_client = IPA_EP_NOT_ALLOCATED;
	ipa_wdi_ctx_list[hdl]->rx1_client = IPA_EP_NOT_ALLOCATED;
	ipa_wdi_ctx_list[hdl]->is_tx1_used = false;
	ipa_wdi_ctx_list[hdl]->is_rx1_used = false;
	uc_ready_params.notify = in->notify;
	uc_ready_params.priv = in->priv;
	uc_ready_params.inst_id = in->inst_id;

	if (ipa3_uc_reg_rdyCB(&uc_ready_params) != 0) {
		mutex_destroy(&ipa_wdi_ctx_list[hdl]->lock);
		kfree(ipa_wdi_ctx_list[hdl]);
		ipa_wdi_ctx_list[hdl] = NULL;
		return -EFAULT;
	}

	out->is_uC_ready = uc_ready_params.is_uC_ready;

	if (IPA_CLIENT_IS_WLAN0_INSTANCE(ipa_wdi_ctx_list[hdl]->inst_id))
		smmu_in.smmu_client = IPA_SMMU_WLAN_CLIENT;
	else if(IPA_CLIENT_IS_WLAN1_INSTANCE(ipa_wdi_ctx_list[hdl]->inst_id))
		smmu_in.smmu_client = IPA_SMMU_WLAN1_CLIENT;
	else if(IPA_CLIENT_IS_WLAN2_INSTANCE(ipa_wdi_ctx_list[hdl]->inst_id))
		smmu_in.smmu_client = IPA_SMMU_WLAN2_CLIENT;
	else
		return -EFAULT;


	if (ipa_get_smmu_params(&smmu_in, &smmu_out))
		out->is_smmu_enabled = false;
	else
		out->is_smmu_enabled = smmu_out.smmu_enable;

	ipa_wdi_ctx_list[hdl]->is_smmu_enabled = out->is_smmu_enabled;

	if (IPA_WDI2_OVER_GSI() || (in->wdi_version >= IPA_WDI_3))
		out->is_over_gsi = true;
	else
		out->is_over_gsi = false;

	out->hdl = hdl;

	return 0;
}
EXPORT_SYMBOL(ipa_wdi_init_per_inst);

/**
 * function to register interface
 *
 * Note: Should not be called from atomic context
 *
 * @Return 0 on success, negative on failure
 */
int ipa_wdi_reg_intf_per_inst(
	struct ipa_wdi_reg_intf_in_params *in)
{
	struct ipa_ioc_add_hdr *hdr;
	struct ipa_wdi_intf_info *new_intf;
	struct ipa_wdi_intf_info *entry;
	struct ipa_tx_intf tx;
	struct ipa_rx_intf rx;
	struct ipa_ioc_tx_intf_prop tx_prop[4];
	struct ipa_ioc_rx_intf_prop rx_prop[4];
	char iface_name[IPA_RESOURCE_NAME_MAX] = {'\0'};
	u32 len;
	int i = 0;
	int ret = 0;
	int num_hdr = 0;
	struct ipa_ext_intf ext;

	if (in == NULL) {
		IPA_WDI_ERR("invalid params in=%pK\n", in);
		return -EINVAL;
	}

	if (in->hdl < 0 || in->hdl >=IPA_WDI_INST_MAX) {
		IPA_WDI_ERR("Invalid handle =%d\n", in->hdl);
		return -EINVAL;
	}

	if (!ipa_wdi_ctx_list[in->hdl]) {
		IPA_WDI_ERR("wdi ctx is not initialized\n");
		return -EPERM;
	}

	if (is_single_instance_wdi_version(ipa_wdi_ctx_list[in->hdl]->wdi_version) &&
		in->hdl > 0) {
		IPA_WDI_ERR("More than one instance not supported for WDI ver = %d\n",
					ipa_wdi_ctx_list[in->hdl]->wdi_version);
		return -EPERM;
	}

	if ((ipa_wdi_ctx_list[in->hdl]->wdi_version == IPA_WDI_4) &&
					(in->mld_enabled)){
		IPA_WDI_DBG("mlo supported iface, stitching\n");
		snprintf((char *)iface_name, sizeof(iface_name),
						"%s_%d_%d", in->netdev_name, (uint32_t)in->hdl,
						((in->meta_data>> VDEV_BIT_SHIFT)& VDEV_ID_MASK));

	}
	else {
		IPA_WDI_DBG("non-mlo iface\n");
		strscpy(iface_name, in->netdev_name, sizeof(iface_name));
	}
	IPA_WDI_DBG("register interface for netdev %s %s\n",
		in->netdev_name, iface_name);

	IPA_WDI_DBG("is_rx1_used: %d\n", in->is_rx1_used);

	num_hdr = in->is_rx1_used ? 4 : 2;

	mutex_lock(&ipa_wdi_ctx_list[in->hdl]->lock);
	list_for_each_entry(entry, &ipa_wdi_ctx_list[in->hdl]->head_intf_list, link)
		if (strcmp(entry->netdev_name, iface_name) == 0) {
			IPA_WDI_DBG("intf was added before.\n");
			mutex_unlock(&ipa_wdi_ctx_list[in->hdl]->lock);
			return 0;
		}

	if (ipa3_ctx->ipa_wdi3_over_gsi &&
		!in->is_tx1_used) {
		IPA_WDI_DBG(
			"tx1 reg intr not sprtd, adng it to default pipe\n");
	}

	IPA_WDI_DBG("intf was not added before, proceed.\n");
	new_intf = kzalloc(sizeof(*new_intf), GFP_KERNEL);
	if (new_intf == NULL) {
		IPA_WDI_ERR("fail to alloc new intf\n");
		mutex_unlock(&ipa_wdi_ctx_list[in->hdl]->lock);
		return -ENOMEM;
	}

	INIT_LIST_HEAD(&new_intf->link);
	strscpy(new_intf->netdev_name, in->netdev_name,
		sizeof(new_intf->netdev_name));
	new_intf->hdr_len = in->hdr_info[0].hdr_len;

	/* add partial header */
	len = sizeof(struct ipa_ioc_add_hdr) + num_hdr * sizeof(struct ipa_hdr_add);
	hdr = kzalloc(len, GFP_KERNEL);
	if (hdr == NULL) {
		IPA_WDI_ERR("fail to alloc %d bytes\n", len);
		ret = -EFAULT;
		goto fail_alloc_hdr;
	}
	hdr->num_hdrs = num_hdr;

	if (ipa_wdi_commit_partial_hdr(hdr, iface_name, in->hdr_info)) {
		IPA_WDI_ERR("fail to commit partial headers\n");
		ret = -EFAULT;
		goto fail_commit_hdr;
	}

	for (i = 0; i < hdr->num_hdrs; i++) {
		new_intf->partial_hdr_hdl[i] = hdr->hdr[i].hdr_hdl;
		IPA_WDI_DBG("enum ipa_ip_type: %d hdr hdl: %d \n", i, hdr->hdr[i].hdr_hdl);
	}

	/* populate tx prop */
	tx.num_props = in->is_rx1_used ? 4 : 2;
	tx.prop = tx_prop;
	IPA_WDI_DBG("Setting tx/rx props\n");
	memset(tx_prop, 0, sizeof(tx_prop));
 	tx_prop[0].ip = IPA_IP_v4;
 	tx_prop[1].ip = IPA_IP_v6;

	if (in->is_tx1_used) {
		tx_prop[0].dst_pipe =  ipa_wdi_ctx_list[in->hdl]->tx1_client;
		tx_prop[1].dst_pipe =  ipa_wdi_ctx_list[in->hdl]->tx1_client;
	} else {
		tx_prop[0].dst_pipe = ipa_wdi_ctx_list[in->hdl]->tx_client;
		tx_prop[1].dst_pipe = ipa_wdi_ctx_list[in->hdl]->tx_client;
	}
 	tx_prop[0].alt_dst_pipe = in->alt_dst_pipe;
 	tx_prop[0].hdr_l2_type = in->hdr_info[0].hdr_type;
 	strscpy(tx_prop[0].hdr_name, hdr->hdr[IPA_IP_v4].name,
 		sizeof(tx_prop[0].hdr_name));

 	tx_prop[1].alt_dst_pipe = in->alt_dst_pipe;
 	tx_prop[1].hdr_l2_type = in->hdr_info[1].hdr_type;
 	strscpy(tx_prop[1].hdr_name, hdr->hdr[IPA_IP_v6].name,
 		sizeof(tx_prop[1].hdr_name));

 	/* populate rx prop */
 	rx.num_props = in->is_rx1_used ? 4 : 2;
 	rx.prop = rx_prop;
 	memset(rx_prop, 0, sizeof(rx_prop));
 	rx_prop[0].ip = IPA_IP_v4;

        rx_prop[0].src_pipe = ipa_wdi_ctx_list[in->hdl]->rx_client;
        rx_prop[1].src_pipe = ipa_wdi_ctx_list[in->hdl]->rx_client;

 	rx_prop[0].hdr_l2_type = in->hdr_info[0].hdr_type;
	if (ipa_wdi_ctx_list[in->hdl]->wdi_version == IPA_WDI_6)
		rx_prop[0].rdi = ipa_wdi_ctx_list[in->hdl]->rdi;
 	if (in->is_meta_data_valid) {
 		rx_prop[0].attrib.attrib_mask |= IPA_FLT_META_DATA;
 		rx_prop[0].attrib.meta_data = in->meta_data;
 		rx_prop[0].attrib.meta_data_mask = in->meta_data_mask;
 	}

 	rx_prop[1].ip = IPA_IP_v6;
 	rx_prop[1].hdr_l2_type = in->hdr_info[1].hdr_type;
	if (ipa_wdi_ctx_list[in->hdl]->wdi_version == IPA_WDI_6)
		rx_prop[1].rdi = ipa_wdi_ctx_list[in->hdl]->rdi;
 	if (in->is_meta_data_valid) {
 		rx_prop[1].attrib.attrib_mask |= IPA_FLT_META_DATA;
 		rx_prop[1].attrib.meta_data = in->meta_data;
 		rx_prop[1].attrib.meta_data_mask = in->meta_data_mask;
 	}

	if (in->is_rx1_used) {
		rx_prop[2].ip = IPA_IP_v4;
		if (ipa_wdi_ctx_list[in->hdl]->wdi_version >= IPA_WDI_3)
			rx_prop[2].src_pipe = ipa_wdi_ctx_list[in->hdl]->rx1_client;

		rx_prop[2].hdr_l2_type = in->hdr_info[2].hdr_type;
		if (ipa_wdi_ctx_list[in->hdl]->wdi_version == IPA_WDI_6)
			rx_prop[2].rdi = ipa_wdi_ctx_list[in->hdl]->rx1_rdi;
		if (in->is_meta_data_valid) {
			rx_prop[2].attrib.attrib_mask |= IPA_FLT_META_DATA;
			rx_prop[2].attrib.meta_data = in->meta_data;
			rx_prop[2].attrib.meta_data_mask = in->meta_data_mask;
		}

		rx_prop[3].ip = IPA_IP_v6;
		if (ipa_wdi_ctx_list[in->hdl]->wdi_version >= IPA_WDI_3)
			rx_prop[3].src_pipe = ipa_wdi_ctx_list[in->hdl]->rx1_client;

		rx_prop[3].hdr_l2_type = in->hdr_info[3].hdr_type;
		if (ipa_wdi_ctx_list[in->hdl]->wdi_version == IPA_WDI_6)
			rx_prop[3].rdi = ipa_wdi_ctx_list[in->hdl]->rx1_rdi;
		if (in->is_meta_data_valid) {
			rx_prop[3].attrib.attrib_mask |= IPA_FLT_META_DATA;
			rx_prop[3].attrib.meta_data = in->meta_data;
			rx_prop[3].attrib.meta_data_mask = in->meta_data_mask;
		}

		/* set up tx2 and tx3 properties for vlan as well*/
		tx_prop[2].ip = IPA_IP_v4;
		tx_prop[2].dst_pipe = ipa_wdi_ctx_list[in->hdl]->tx_client;
		tx_prop[2].alt_dst_pipe = in->alt_dst_pipe;
		tx_prop[2].hdr_l2_type = in->hdr_info[2].hdr_type;
		strscpy(tx_prop[2].hdr_name, hdr->hdr[IPA_IP_v4_VLAN].name,
				sizeof(tx_prop[2].hdr_name));

		tx_prop[3].ip = IPA_IP_v6;
		tx_prop[3].dst_pipe = ipa_wdi_ctx_list[in->hdl]->tx_client;
		tx_prop[3].alt_dst_pipe = in->alt_dst_pipe;
		tx_prop[3].hdr_l2_type = in->hdr_info[3].hdr_type;
		strscpy(tx_prop[3].hdr_name, hdr->hdr[IPA_IP_v6_VLAN].name,
				sizeof(tx_prop[3].hdr_name));
	}

	if (ipa_wdi_ctx_list[in->hdl]->wdi_version == IPA_WDI_6) {
		memset(&ext, 0, sizeof(ext));
		ext.vpnum_valid = true;
		ext.vpnum = in->vpnum;
		IPA_WDI_DBG("vp num %d intf_idx %d\n", ext.vpnum, in->intf_idx);

		if (ipa3_register_intf_ext(iface_name, &tx, &rx, &ext, in->intf_idx)) {
			IPA_WDI_ERR("fail to add interface prop\n");
			ret = -EFAULT;
		}
	} else {
		if (ipa_register_intf(iface_name, &tx, &rx, 0)) {
			IPA_WDI_ERR("fail to add interface prop\n");
			ret = -EFAULT;
		}
	}

	IPA_WDI_DBG("Done Register Interface\n");
	list_add(&new_intf->link, &ipa_wdi_ctx_list[in->hdl]->head_intf_list);
	init_completion(&ipa_wdi_ctx_list[in->hdl]->wdi_completion);

	kfree(hdr);
	mutex_unlock(&ipa_wdi_ctx_list[in->hdl]->lock);
	return 0;

fail_commit_hdr:
	kfree(hdr);
fail_alloc_hdr:
	kfree(new_intf);
	mutex_unlock(&ipa_wdi_ctx_list[in->hdl]->lock);
	return ret;
}
EXPORT_SYMBOL(ipa_wdi_reg_intf_per_inst);

/**
 * function to connect pipes
 *
 * @in: [in] input parameters from client
 * @out: [out] output params to client
 *
 * Note: Should not be called from atomic context
 *
 * @Return 0 on success, negative on failure
 */
int ipa_wdi_conn_pipes_per_inst(struct ipa_wdi_conn_in_params *in,
	struct ipa_wdi_conn_out_params *out)
{
	int i, j, ret = 0;
	struct ipa_pm_register_params pm_params;
	struct ipa_wdi_in_params in_tx;
	struct ipa_wdi_in_params in_rx;
	struct ipa_wdi_out_params out_tx;
	struct ipa_wdi_out_params out_rx;
	int ipa_ep_idx_tx1 = IPA_EP_NOT_ALLOCATED;
	int ipa_ep_idx_rx1 = IPA_EP_NOT_ALLOCATED;

	if (!(in && out)) {
		IPA_WDI_ERR("empty parameters. in=%pK out=%pK\n", in, out);
		return -EINVAL;
	}

	if (in->hdl < 0 || in->hdl >= IPA_WDI_INST_MAX) {
		IPA_WDI_ERR("Invalid handle %d \n", in->hdl);
		return -EINVAL;
	}

	if (!ipa_wdi_ctx_list[in->hdl]) {
		IPA_WDI_ERR("wdi ctx is not initialized\n");
		return -EPERM;
	}

	if (is_single_instance_wdi_version(ipa_wdi_ctx_list[in->hdl]->wdi_version) &&
		in->hdl > 0) {
		IPA_WDI_ERR("More than one instance not supported for WDI ver = %d\n",
					ipa_wdi_ctx_list[in->hdl]->wdi_version);
		return -EPERM;
	}

	if (in->num_sys_pipe_needed > IPA_WDI_MAX_SUPPORTED_SYS_PIPE) {
		IPA_WDI_ERR("ipa can only support up to %d sys pipe\n",
			IPA_WDI_MAX_SUPPORTED_SYS_PIPE);
		return -EINVAL;
	}
	ipa_wdi_ctx_list[in->hdl]->num_sys_pipe_needed = in->num_sys_pipe_needed;
	ipa_wdi_ctx_list[in->hdl]->tx_client = in->u_tx.tx.client;
	ipa_wdi_ctx_list[in->hdl]->rx_client = in->u_rx.rx.client;
	if (ipa_wdi_ctx_list[in->hdl]->wdi_version == IPA_WDI_6) {
		if (in->is_smmu_enabled)
			ipa_wdi_ctx_list[in->hdl]->rdi = in->u_rx.rx_smmu.rdi;
		else
			ipa_wdi_ctx_list[in->hdl]->rdi = in->u_rx.rx.rdi;
	}
	IPA_WDI_DBG("number of sys pipe %d\n", in->num_sys_pipe_needed);

	if (in->is_tx1_used) {
		ipa_wdi_ctx_list[in->hdl]->is_tx1_used = in->is_tx1_used;
		ipa_wdi_ctx_list[in->hdl]->tx1_client = in->u_tx1.tx.client;
		ipa_ep_idx_tx1 = ipa_get_ep_mapping(ipa_wdi_ctx_list[in->hdl]->tx1_client);
	}

	if (in->is_rx1_used) {
		ipa_wdi_ctx_list[in->hdl]->is_rx1_used = in->is_rx1_used;
		ipa_wdi_ctx_list[in->hdl]->rx1_client = in->u_rx1.rx.client;
		if (ipa_wdi_ctx_list[in->hdl]->wdi_version == IPA_WDI_6) {
			if (in->is_smmu_enabled)
				ipa_wdi_ctx_list[in->hdl]->rx1_rdi = in->u_rx1.rx_smmu.rdi;
			else
				ipa_wdi_ctx_list[in->hdl]->rx1_rdi = in->u_rx1.rx.rdi;
		}
		ipa_ep_idx_rx1 = ipa_get_ep_mapping(ipa_wdi_ctx_list[in->hdl]->rx1_client);
	}

	IPA_WDI_DBG("number of sys pipe %d,Tx1 asked=%d,"
		"Rx1 asked=%d,Tx1 supported = %d,Rx1 supported=%d smmu_en=%d\n",
		in->num_sys_pipe_needed, in->is_tx1_used,
		in->is_rx1_used, ipa_wdi_ctx_list[in->hdl]->is_tx1_used,
		ipa_wdi_ctx_list[in->hdl]->is_rx1_used, in->is_smmu_enabled);

	/* setup sys pipe when needed */
	for (i = 0; i < in->num_sys_pipe_needed; i++) {
		ret = ipa_setup_sys_pipe(&in->sys_in[i],
			&ipa_wdi_ctx_list[in->hdl]->sys_pipe_hdl[i]);
		if (ret) {
			IPA_WDI_ERR("fail to setup sys pipe %d\n", i);
			ret = -EFAULT;
			goto fail_setup_sys_pipe;
		}
	}

	memset(&pm_params, 0, sizeof(pm_params));
	if (IPA_CLIENT_IS_WLAN0_INSTANCE(ipa_wdi_ctx_list[in->hdl]->inst_id))
		pm_params.name = "wdi";
	else if (IPA_CLIENT_IS_WLAN1_INSTANCE(ipa_wdi_ctx_list[in->hdl]->inst_id))
		pm_params.name = "wdi1";
	else if (IPA_CLIENT_IS_WLAN2_INSTANCE(ipa_wdi_ctx_list[in->hdl]->inst_id))
		pm_params.name = "wdi2";
	else
		IPA_WDI_ERR("Unsupported instance id value received %d\n", ipa_wdi_ctx_list[in->hdl]->inst_id);

	pm_params.callback = ipa_wdi_pm_cb;
	pm_params.user_data = NULL;
	pm_params.group = IPA_PM_GROUP_DEFAULT;
	if (ipa_pm_register(&pm_params, &ipa_wdi_ctx_list[in->hdl]->ipa_pm_hdl)) {
		IPA_WDI_ERR("fail to register ipa pm\n");
		ret = -EFAULT;
		goto fail_setup_sys_pipe;
	}
	IPA_WDI_DBG("PM handle Registered\n");
	if (ipa_wdi_ctx_list[in->hdl]->wdi_version >= IPA_WDI_3) {
		if (ipa3_conn_wdi3_pipes(in, out, ipa_wdi_ctx_list[in->hdl]->wdi_notify,
			ipa_wdi_ctx_list[in->hdl]->ast_update)) {
			IPA_WDI_ERR("fail to setup wdi pipes\n");
			ret = -EFAULT;
			goto fail_connect_pipe;
		}
	} else {
		memset(&in_tx, 0, sizeof(in_tx));
		memset(&in_rx, 0, sizeof(in_rx));
		memset(&out_tx, 0, sizeof(out_tx));
		memset(&out_rx, 0, sizeof(out_rx));
#ifdef IPA_WAN_MSG_IPv6_ADDR_GW_LEN
		in_rx.wdi_notify = ipa_wdi_ctx_list[in->hdl]->wdi_notify;
#endif
		if (in->is_smmu_enabled == false) {
			/* firsr setup rx pipe */
			in_rx.sys.ipa_ep_cfg = in->u_rx.rx.ipa_ep_cfg;
			in_rx.sys.client = in->u_rx.rx.client;
			in_rx.sys.notify = in->notify;
			in_rx.sys.priv = in->priv;
			in_rx.smmu_enabled = in->is_smmu_enabled;
			in_rx.u.ul.rdy_ring_base_pa =
				in->u_rx.rx.transfer_ring_base_pa;
			in_rx.u.ul.rdy_ring_size =
				in->u_rx.rx.transfer_ring_size;
			in_rx.u.ul.rdy_ring_rp_pa =
				in->u_rx.rx.transfer_ring_doorbell_pa;
			in_rx.u.ul.rdy_comp_ring_base_pa =
				in->u_rx.rx.event_ring_base_pa;
			in_rx.u.ul.rdy_comp_ring_wp_pa =
				in->u_rx.rx.event_ring_doorbell_pa;
			in_rx.u.ul.rdy_comp_ring_size =
				in->u_rx.rx.event_ring_size;
			in_rx.u.ul.is_txr_rn_db_pcie_addr =
				in->u_rx.rx.is_txr_rn_db_pcie_addr;
			in_rx.u.ul.is_evt_rn_db_pcie_addr =
				in->u_rx.rx.is_evt_rn_db_pcie_addr;
			if (ipa_connect_wdi_pipe(&in_rx, &out_rx)) {
				IPA_WDI_ERR("fail to setup rx pipe\n");
				ret = -EFAULT;
				goto fail_connect_pipe;
			}
			ipa_wdi_ctx_list[in->hdl]->rx_pipe_hdl = out_rx.clnt_hdl;
			out->rx_uc_db_pa = out_rx.uc_door_bell_pa;
			IPA_WDI_DBG("rx uc db pa: 0x%pad\n", &out->rx_uc_db_pa);

			/* then setup tx pipe */
			in_tx.sys.ipa_ep_cfg = in->u_tx.tx.ipa_ep_cfg;
			in_tx.sys.client = in->u_tx.tx.client;
			in_tx.smmu_enabled = in->is_smmu_enabled;
			in_tx.u.dl.comp_ring_base_pa =
				in->u_tx.tx.transfer_ring_base_pa;
			in_tx.u.dl.comp_ring_size =
				in->u_tx.tx.transfer_ring_size;
			in_tx.u.dl.ce_ring_base_pa =
				in->u_tx.tx.event_ring_base_pa;
			in_tx.u.dl.ce_door_bell_pa =
				in->u_tx.tx.event_ring_doorbell_pa;
			in_tx.u.dl.ce_ring_size =
				in->u_tx.tx.event_ring_size;
			in_tx.u.dl.num_tx_buffers =
				in->u_tx.tx.num_pkt_buffers;
			in_tx.u.dl.is_txr_rn_db_pcie_addr =
				in->u_tx.tx.is_txr_rn_db_pcie_addr;
			in_tx.u.dl.is_evt_rn_db_pcie_addr =
				in->u_tx.tx.is_evt_rn_db_pcie_addr;
			if (ipa_connect_wdi_pipe(&in_tx, &out_tx)) {
				IPA_WDI_ERR("fail to setup tx pipe\n");
				ret = -EFAULT;
				goto fail;
			}
			ipa_wdi_ctx_list[in->hdl]->tx_pipe_hdl = out_tx.clnt_hdl;
			out->tx_uc_db_pa = out_tx.uc_door_bell_pa;
			IPA_WDI_DBG("tx uc db pa: 0x%pad\n", &out->tx_uc_db_pa);
		} else { /* smmu is enabled */
			/* firsr setup rx pipe */
			in_rx.sys.ipa_ep_cfg = in->u_rx.rx_smmu.ipa_ep_cfg;
			in_rx.sys.client = in->u_rx.rx_smmu.client;
			in_rx.sys.notify = in->notify;
			in_rx.sys.priv = in->priv;
			in_rx.smmu_enabled = in->is_smmu_enabled;
			in_rx.u.ul_smmu.rdy_ring =
				in->u_rx.rx_smmu.transfer_ring_base;
			in_rx.u.ul_smmu.rdy_ring_size =
				in->u_rx.rx_smmu.transfer_ring_size;
			in_rx.u.ul_smmu.rdy_ring_rp_pa =
				in->u_rx.rx_smmu.transfer_ring_doorbell_pa;
			in_rx.u.ul_smmu.rdy_comp_ring =
				in->u_rx.rx_smmu.event_ring_base;
			in_rx.u.ul_smmu.rdy_comp_ring_wp_pa =
				in->u_rx.rx_smmu.event_ring_doorbell_pa;
			in_rx.u.ul_smmu.rdy_comp_ring_size =
				in->u_rx.rx_smmu.event_ring_size;
			in_rx.u.ul_smmu.is_txr_rn_db_pcie_addr =
				in->u_rx.rx_smmu.is_txr_rn_db_pcie_addr;
			in_rx.u.ul_smmu.is_evt_rn_db_pcie_addr =
				in->u_rx.rx_smmu.is_evt_rn_db_pcie_addr;
			if (ipa_connect_wdi_pipe(&in_rx, &out_rx)) {
				IPA_WDI_ERR("fail to setup rx pipe\n");
				ret = -EFAULT;
				goto fail_connect_pipe;
			}
			ipa_wdi_ctx_list[in->hdl]->rx_pipe_hdl = out_rx.clnt_hdl;
			out->rx_uc_db_pa = out_rx.uc_door_bell_pa;
			IPA_WDI_DBG("rx uc db pa: 0x%pad\n", &out->rx_uc_db_pa);

			/* then setup tx pipe */
			in_tx.sys.ipa_ep_cfg = in->u_tx.tx_smmu.ipa_ep_cfg;
			in_tx.sys.client = in->u_tx.tx_smmu.client;
			in_tx.smmu_enabled = in->is_smmu_enabled;
			in_tx.u.dl_smmu.comp_ring =
				in->u_tx.tx_smmu.transfer_ring_base;
			in_tx.u.dl_smmu.comp_ring_size =
				in->u_tx.tx_smmu.transfer_ring_size;
			in_tx.u.dl_smmu.ce_ring =
				in->u_tx.tx_smmu.event_ring_base;
			in_tx.u.dl_smmu.ce_door_bell_pa =
				in->u_tx.tx_smmu.event_ring_doorbell_pa;
			in_tx.u.dl_smmu.ce_ring_size =
				in->u_tx.tx_smmu.event_ring_size;
			in_tx.u.dl_smmu.num_tx_buffers =
				in->u_tx.tx_smmu.num_pkt_buffers;
			in_tx.u.dl_smmu.is_txr_rn_db_pcie_addr =
				in->u_tx.tx_smmu.is_txr_rn_db_pcie_addr;
			in_tx.u.dl_smmu.is_evt_rn_db_pcie_addr =
				in->u_tx.tx_smmu.is_evt_rn_db_pcie_addr;
			if (ipa_connect_wdi_pipe(&in_tx, &out_tx)) {
				IPA_WDI_ERR("fail to setup tx pipe\n");
				ret = -EFAULT;
				goto fail;
			}
			ipa_wdi_ctx_list[in->hdl]->tx_pipe_hdl = out_tx.clnt_hdl;
			out->tx_uc_db_pa = out_tx.uc_door_bell_pa;
			ret = ipa_pm_associate_ipa_cons_to_client(ipa_wdi_ctx_list[in->hdl]->ipa_pm_hdl,
				in_tx.sys.client);
			if (ret) {
				IPA_WDI_ERR("fail to associate cons with PM %d\n", ret);
				goto fail;
			}
			IPA_WDI_DBG("tx uc db pa: 0x%pad\n", &out->tx_uc_db_pa);
		}
	IPA_WDI_DBG("conn pipes done\n");
	}

	return 0;

fail:
	ipa_disconnect_wdi_pipe(ipa_wdi_ctx_list[in->hdl]->rx_pipe_hdl);
fail_connect_pipe:
	ipa_pm_deregister(ipa_wdi_ctx_list[in->hdl]->ipa_pm_hdl);

fail_setup_sys_pipe:
	for (j = 0; j < i; j++)
		ipa_teardown_sys_pipe(ipa_wdi_ctx_list[in->hdl]->sys_pipe_hdl[j]);
	return ret;
}
EXPORT_SYMBOL(ipa_wdi_conn_pipes_per_inst);

/**
 * function to enable IPA offload data path
 *
 * @hdl: hdl to wdi client
 * Note: Should not be called from atomic context
 *
 * Returns: 0 on success, negative on failure
 */
int ipa_wdi_enable_pipes_per_inst(ipa_wdi_hdl_t hdl)
{
	int ret;
	int ipa_ep_idx_tx, ipa_ep_idx_rx;
	int ipa_ep_idx_tx1 = IPA_EP_NOT_ALLOCATED;
	int ipa_ep_idx_rx1 = IPA_EP_NOT_ALLOCATED;

	if (hdl < 0 || hdl >= IPA_WDI_INST_MAX) {
		IPA_WDI_ERR("Invalid handle %d\n", hdl);
	}

	if (!ipa_wdi_ctx_list[hdl]) {
		IPA_WDI_ERR("wdi ctx is not initialized.\n");
		return -EPERM;
	}

	if (is_single_instance_wdi_version(ipa_wdi_ctx_list[hdl]->wdi_version) &&
		hdl > 0) {
		IPA_WDI_ERR("More than one instance not supported for WDI ver = %d\n",
					ipa_wdi_ctx_list[hdl]->wdi_version);
		return -EPERM;
	}

	ipa_ep_idx_rx = ipa_get_ep_mapping(ipa_wdi_ctx_list[hdl]->rx_client);
	ipa_ep_idx_tx = ipa_get_ep_mapping(ipa_wdi_ctx_list[hdl]->tx_client);

	if (ipa_wdi_ctx_list[hdl]->wdi_version >= IPA_WDI_3) {
		if (ipa_wdi_ctx_list[hdl]->is_rx1_used)
			ipa_ep_idx_rx1 = ipa_get_ep_mapping(ipa_wdi_ctx_list[hdl]->rx1_client);
		if (ipa_wdi_ctx_list[hdl]->is_tx1_used)
			ipa_ep_idx_tx1 = ipa_get_ep_mapping(ipa_wdi_ctx_list[hdl]->tx1_client);
	}

	if (ipa_ep_idx_tx <= 0 || ipa_ep_idx_rx <= 0)
		return -EFAULT;
	ret = ipa_pm_activate_sync(ipa_wdi_ctx_list[hdl]->ipa_pm_hdl);
	if (ret) {
		IPA_WDI_ERR("fail to activate ipa pm\n");
		return -EFAULT;
	}
	IPA_WDI_DBG("Enable WDI pipes\n");
	if (ipa_wdi_ctx_list[hdl]->wdi_version >= IPA_WDI_3) {
		if (ipa3_enable_wdi3_pipes(
			ipa_ep_idx_tx, ipa_ep_idx_rx, ipa_ep_idx_tx1,ipa_ep_idx_rx1)) {
			IPA_WDI_ERR("fail to enable wdi pipes\n");
			return -EFAULT;
		}
	} else {
		if ((ipa_wdi_ctx_list[hdl]->tx_pipe_hdl >= IPA3_MAX_NUM_PIPES) ||
			(ipa_wdi_ctx_list[hdl]->tx_pipe_hdl < 0) ||
			(ipa_wdi_ctx_list[hdl]->rx_pipe_hdl >= IPA3_MAX_NUM_PIPES) ||
			(ipa_wdi_ctx_list[hdl]->rx_pipe_hdl < 0)) {
			IPA_WDI_ERR("pipe handle not valid\n");
			return -EFAULT;
		}
		if (ipa_enable_wdi_pipe(ipa_wdi_ctx_list[hdl]->tx_pipe_hdl)) {
			IPA_WDI_ERR("fail to enable wdi tx pipe\n");
			return -EFAULT;
		}
		if (ipa_resume_wdi_pipe(ipa_wdi_ctx_list[hdl]->tx_pipe_hdl)) {
			IPA_WDI_ERR("fail to resume wdi tx pipe\n");
			return -EFAULT;
		}
		if (ipa_enable_wdi_pipe(ipa_wdi_ctx_list[hdl]->rx_pipe_hdl)) {
			IPA_WDI_ERR("fail to enable wdi rx pipe\n");
			return -EFAULT;
		}
		if (ipa_resume_wdi_pipe(ipa_wdi_ctx_list[hdl]->rx_pipe_hdl)) {
			IPA_WDI_ERR("fail to resume wdi rx pipe\n");
			return -EFAULT;
		}
	}

	return 0;
}
EXPORT_SYMBOL(ipa_wdi_enable_pipes_per_inst);

/**
 * set IPA clock bandwidth based on data rates
 *
 * @hdl: hdl to wdi client
 * @profile: [in] BandWidth profile to use
 *
 * Returns: 0 on success, negative on failure
 */
int ipa_wdi_set_perf_profile_per_inst(ipa_wdi_hdl_t hdl,
	struct ipa_wdi_perf_profile *profile)
{
	int res = 0;
	if (profile == NULL) {
		IPA_WDI_ERR("Invalid input\n");
		return -EINVAL;
	}

	if (hdl < 0 || hdl >= IPA_WDI_INST_MAX) {
		IPA_WDI_ERR("Invalid Handle %d\n",hdl);
		return -EFAULT;
	}

	if (is_single_instance_wdi_version(ipa_wdi_ctx_list[hdl]->wdi_version) &&
		hdl > 0) {
		IPA_WDI_ERR("More than one instance not supported for WDI ver = %d\n",
				ipa_wdi_ctx_list[hdl]->wdi_version);
		return -EPERM;
	}

	if (ipa3_ctx->use_pm_wrapper) {
		res = ipa_pm_wrapper_wdi_set_perf_profile_internal(profile);
	} else {
		res = ipa_pm_set_throughput(ipa_wdi_ctx_list[hdl]->ipa_pm_hdl,
			profile->max_supported_bw_mbps);
	}

	if (res) {
		IPA_WDI_ERR("fail to set pm throughput\n");
		return -EFAULT;
	}

	return res;
}
EXPORT_SYMBOL(ipa_wdi_set_perf_profile_per_inst);

/**
 * function to create smmu mapping
 *
 * @hdl: hdl to wdi client
 * @num_buffers: number of buffers
 * @info: wdi buffer info
 */
int ipa_wdi_create_smmu_mapping_per_inst(ipa_wdi_hdl_t hdl,
	u32 num_buffers,
	struct ipa_wdi_buffer_info *info)
{
	struct ipa_smmu_cb_ctx *cb;
	int i;
	int ret = 0;
	int prot = IOMMU_READ | IOMMU_WRITE;

	if (!info) {
		IPAERR_RL("info = %pK\n", info);
		return -EINVAL;
	}

	if (hdl < 0 || hdl >= IPA_WDI_INST_MAX) {
		IPA_WDI_ERR("Invalid Handle %d\n",hdl);
		return -EFAULT;
	}

	if (IPA_CLIENT_IS_WLAN0_INSTANCE(ipa_wdi_ctx_list[hdl]->inst_id))
		cb = ipa3_get_smmu_ctx(IPA_SMMU_CB_WLAN);
	else if(IPA_CLIENT_IS_WLAN1_INSTANCE(ipa_wdi_ctx_list[hdl]->inst_id))
		cb = ipa3_get_smmu_ctx(IPA_SMMU_CB_WLAN1);
	else if(IPA_CLIENT_IS_WLAN2_INSTANCE(ipa_wdi_ctx_list[hdl]->inst_id))
		cb = ipa3_get_smmu_ctx(IPA_SMMU_CB_WLAN2);
	else {
		IPA_WDI_ERR("Unsupported instanace id %d\n", ipa_wdi_ctx_list[hdl]->inst_id);
		return -EFAULT;
	}
	if (!cb->valid) {
		IPA_WDI_ERR("No SMMU CB setup\n");
		return -EINVAL;
	}

	if ((IPA_CLIENT_IS_WLAN0_INSTANCE(ipa_wdi_ctx_list[hdl]->inst_id) &&
			ipa3_ctx->s1_bypass_arr[IPA_SMMU_CB_WLAN]) ||
		(IPA_CLIENT_IS_WLAN1_INSTANCE(ipa_wdi_ctx_list[hdl]->inst_id) &&
			ipa3_ctx->s1_bypass_arr[IPA_SMMU_CB_WLAN1]) ||
		(IPA_CLIENT_IS_WLAN2_INSTANCE(ipa_wdi_ctx_list[hdl]->inst_id) &&
			ipa3_ctx->s1_bypass_arr[IPA_SMMU_CB_WLAN2])) {
			IPA_WDI_ERR("IPA SMMU not enabled\n");
			return -EINVAL;
	}

	for (i = 0; i < num_buffers; i++) {
		IPA_WDI_DBG_LOW("i=%d pa=0x%pa iova=0x%lx sz=0x%zx\n", i,
			&info[i].pa, info[i].iova, info[i].size);
		info[i].result = ipa3_iommu_map(cb->iommu_domain,
			rounddown(info[i].iova, PAGE_SIZE),
			rounddown(info[i].pa, PAGE_SIZE),
			roundup(info[i].size + info[i].pa -
				rounddown(info[i].pa, PAGE_SIZE), PAGE_SIZE),
				prot);
	}

	return ret;
}
EXPORT_SYMBOL(ipa_wdi_create_smmu_mapping_per_inst);


/**
 * function to release smmu mapping
 *
 * @hdl: hdl to wdi client
 * @num_buffers: number of buffers
 *
 * @info: wdi buffer info
 */
int ipa_wdi_release_smmu_mapping_per_inst(ipa_wdi_hdl_t hdl,
	u32 num_buffers,
	struct ipa_wdi_buffer_info *info)
{
	struct ipa_smmu_cb_ctx *cb;
	int i;
	int ret = 0;

	if (!info) {
		IPAERR_RL("info = %pK\n", info);
		return -EINVAL;
	}

	if (hdl < 0 || hdl >= IPA_WDI_INST_MAX) {
		IPA_WDI_ERR("Invalid Handle %d\n",hdl);
		return -EFAULT;
	}

	if (IPA_CLIENT_IS_WLAN0_INSTANCE(ipa_wdi_ctx_list[hdl]->inst_id))
		cb = ipa3_get_smmu_ctx(IPA_SMMU_CB_WLAN);
	else if (IPA_CLIENT_IS_WLAN1_INSTANCE(ipa_wdi_ctx_list[hdl]->inst_id))
		cb = ipa3_get_smmu_ctx(IPA_SMMU_CB_WLAN1);
	else if (IPA_CLIENT_IS_WLAN2_INSTANCE(ipa_wdi_ctx_list[hdl]->inst_id))
		cb = ipa3_get_smmu_ctx(IPA_SMMU_CB_WLAN2);
	else {
		IPA_WDI_ERR("Unsupported instance id value %d\n", ipa_wdi_ctx_list[hdl]->inst_id);
		return -EFAULT;
	}

	if (!cb->valid) {
		IPA_WDI_ERR("No SMMU CB setup\n");
		return -EINVAL;
	}

	for (i = 0; i < num_buffers; i++) {
		IPA_WDI_DBG_LOW("i=%d pa=0x%pa iova=0x%lx sz=0x%zx\n", i,
			&info[i].pa, info[i].iova, info[i].size);
		info[i].result = iommu_unmap(cb->iommu_domain,
			rounddown(info[i].iova, PAGE_SIZE),
			roundup(info[i].size + info[i].pa -
				rounddown(info[i].pa, PAGE_SIZE), PAGE_SIZE));
	}

	return ret;
}
EXPORT_SYMBOL(ipa_wdi_release_smmu_mapping_per_inst);

/**
 * clean up WDI IPA offload data path
 *
 * @hdl: hdl to wdi client
 *
 * @Return 0 on success, negative on failure
 */
int ipa_wdi_cleanup_per_inst(ipa_wdi_hdl_t hdl)
{
	struct ipa_wdi_intf_info *entry;
	struct ipa_wdi_intf_info *next;
	struct ipa_ioc_del_hdr *hdr = NULL;
	int i, len, num_hdr;

	if (hdl < 0 || hdl >= IPA_WDI_INST_MAX) {
		IPA_WDI_ERR("Invalid Handle %d\n", hdl);
		return -EFAULT;
	}

	if (!ipa_wdi_ctx_list[hdl]) {
		IPA_WDI_ERR("wdi ctx is not initialized.\n");
		return -EPERM;
	}

	IPA_WDI_DBG("client hdl = %d, Instance = %d\n", hdl, ipa_wdi_ctx_list[hdl]->inst_id);

	if (is_single_instance_wdi_version(ipa_wdi_ctx_list[hdl]->wdi_version) &&
		hdl > 0) {
		IPA_WDI_ERR("More than one instance not supported for WDI ver = %d\n",
				ipa_wdi_ctx_list[hdl]->wdi_version);
		return -EPERM;
	}

	num_hdr = ipa_wdi_ctx_list[hdl]->is_rx1_used ? 4 : 2;

	/* clear interface list */
	list_for_each_entry_safe(entry, next,
		&ipa_wdi_ctx_list[hdl]->head_intf_list, link) {
		len = sizeof(struct ipa_ioc_del_hdr) +
			num_hdr * sizeof(struct ipa_hdr_del);
		hdr = kzalloc(len, GFP_KERNEL);
		if (hdr == NULL) {
			IPA_WDI_ERR("fail to alloc %d bytes\n", len);
		} else {
			hdr->commit = 1;
			hdr->num_hdls = num_hdr;
			for (i = 0; i < num_hdr; i++) {
				hdr->hdl[i].hdl = entry->partial_hdr_hdl[i];
				IPA_WDI_DBG("hdr hdl: %d\n", hdr->hdl[i].hdl);
			}

			if (ipa_del_hdr(hdr))
				IPA_WDI_ERR("fail to delete partial header\n");
			kfree(hdr);
		}

		if (ipa_deregister_intf(entry->netdev_name))
			IPA_WDI_ERR("fail to del interface props\n");

		list_del(&entry->link);
		kfree(entry);
	}

	if (!ipa3_uc_dereg_per_inst_rdyCB(ipa_wdi_ctx_list[hdl]->inst_id))
		IPAERR("entry not found for id %d\n", ipa_wdi_ctx_list[hdl]->inst_id);

	mutex_destroy(&ipa_wdi_ctx_list[hdl]->lock);
	kfree(ipa_wdi_ctx_list[hdl]);
	ipa_wdi_ctx_list[hdl] = NULL;
	return 0;
}
EXPORT_SYMBOL(ipa_wdi_cleanup_per_inst);

/*
 * Deregister MLO stiched iface.
 *
 * Return 0 on success negetive on failure.
 */

int ipa_wdi_dereg_intf_per_inst_mlo(const char *netdev_name,
				ipa_wdi_hdl_t hdl, uint8_t vdev_id, bool mld_enabled)
{
	int ret = 0;
	char iface_name[IPA_RESOURCE_NAME_MAX] = {'\0'};

	if (!netdev_name) {
		IPA_WDI_ERR("no netdev name. \n");
		return -EINVAL;
	}

	if ((ipa_wdi_ctx_list[hdl]->wdi_version == IPA_WDI_4) && (mld_enabled)){
		IPA_WDI_DBG("stitching mlo enabled iface\n");
		snprintf((char *)iface_name, sizeof(iface_name), "%s_%d_%d",
						netdev_name, hdl, vdev_id);
	}
	else {
		IPA_WDI_DBG("non mlo iface\n");
		strscpy(iface_name, netdev_name, sizeof(iface_name));
	}
	ret = ipa_wdi_dereg_intf_per_inst(iface_name, hdl);

	return ret;
}
EXPORT_SYMBOL(ipa_wdi_dereg_intf_per_inst_mlo);

/**
 * function to deregister before unload and after disconnect
 *
 * @Return 0 on success, negative on failure
 */
int ipa_wdi_dereg_intf_per_inst(const char *netdev_name, ipa_wdi_hdl_t hdl)
{
	int i, len, ret = 0;
	struct ipa_ioc_del_hdr *hdr = NULL;
	struct ipa_wdi_intf_info *entry;
	struct ipa_wdi_intf_info *next;
	int num_hdr = 0;

	if (!netdev_name) {
		IPA_WDI_ERR("no netdev name.\n");
		return -EINVAL;
	}

	if (hdl < 0 || hdl >= IPA_WDI_INST_MAX) {
		IPA_WDI_ERR("Invalid Handle %d\n",hdl);
		return -EFAULT;
	}

	IPA_WDI_DBG("deregister interface for netdev %s\n", netdev_name);

	if (is_single_instance_wdi_version(ipa_wdi_ctx_list[hdl]->wdi_version) &&
		hdl > 0) {
		IPA_WDI_ERR("More than one instance not supported for WDI ver = %d\n",
					ipa_wdi_ctx_list[hdl]->wdi_version);
		return -EPERM;
	}

	if (!ipa_wdi_ctx_list[hdl]) {
		IPA_WDI_ERR("wdi ctx is not initialized.\n");
		return -EPERM;
	}

	num_hdr = ipa_wdi_ctx_list[hdl]->is_rx1_used ? 4 : 2;

	IPA_WDI_DBG("Deregister Instance hdl %d\n",hdl);
	mutex_lock(&ipa_wdi_ctx_list[hdl]->lock);
	list_for_each_entry_safe(entry, next, &ipa_wdi_ctx_list[hdl]->head_intf_list,
		link)
		if (strcmp(entry->netdev_name, netdev_name) == 0) {
			len = sizeof(struct ipa_ioc_del_hdr) +
				num_hdr * sizeof(struct ipa_hdr_del);
			hdr = kzalloc(len, GFP_KERNEL);
			if (hdr == NULL) {
				IPA_WDI_ERR("fail to alloc %d bytes\n", len);
				mutex_unlock(&ipa_wdi_ctx_list[hdl]->lock);
				return -ENOMEM;
			}

			hdr->commit = 1;
			hdr->num_hdls = num_hdr;
			for (i = 0; i < num_hdr; i++) {
				hdr->hdl[i].hdl = entry->partial_hdr_hdl[i];
				IPA_WDI_DBG("hdr hdl: %d\n", hdr->hdl[i].hdl);
			}

			if (ipa_del_hdr(hdr)) {
				IPA_WDI_ERR("fail to delete partial header\n");
				ret = -EFAULT;
				goto fail;
			}

			if (ipa_deregister_intf(entry->netdev_name)) {
				IPA_WDI_ERR("fail to del interface props\n");
				ret = -EFAULT;
				goto fail;
			}

			list_del(&entry->link);
			kfree(entry);

			break;
		}

fail:
	kfree(hdr);
	mutex_unlock(&ipa_wdi_ctx_list[hdl]->lock);
	return ret;
}
EXPORT_SYMBOL(ipa_wdi_dereg_intf_per_inst);

/**
 * function to disconnect pipes
 *
 * @hdl: hdl to wdi client
 * Note: Should not be called from atomic context
 *
 * Returns: 0 on success, negative on failure
 */
int ipa_wdi_disconn_pipes_per_inst(ipa_wdi_hdl_t hdl)
{
	int i, ipa_ep_idx_rx, ipa_ep_idx_tx;
	int ipa_ep_idx_tx1 = IPA_EP_NOT_ALLOCATED;
	int ipa_ep_idx_rx1 = IPA_EP_NOT_ALLOCATED;

	if (hdl < 0 || hdl >= IPA_WDI_INST_MAX) {
		IPA_WDI_ERR("Invalid Handle %d\n",hdl);
		return -EFAULT;
	}

	if (is_single_instance_wdi_version(ipa_wdi_ctx_list[hdl]->wdi_version) &&
		hdl > 0) {
		IPA_WDI_ERR("More than one instance not supported for WDI ver = %d\n",
				ipa_wdi_ctx_list[hdl]->wdi_version);
		return -EPERM;
	}

	if (!ipa_wdi_ctx_list[hdl]) {
		IPA_WDI_ERR("wdi ctx is not initialized\n");
		return -EPERM;
	}
	IPA_WDI_DBG("Disconnect pipes for hdl %d\n",hdl);
	/* tear down sys pipe if needed */
	for (i = 0; i < ipa_wdi_ctx_list[hdl]->num_sys_pipe_needed; i++) {
		if (ipa_teardown_sys_pipe(ipa_wdi_ctx_list[hdl]->sys_pipe_hdl[i])) {
			IPA_WDI_ERR("fail to tear down sys pipe %d\n", i);
			return -EFAULT;
		}
	}

	ipa_ep_idx_rx = ipa_get_ep_mapping(ipa_wdi_ctx_list[hdl]->rx_client);
	ipa_ep_idx_tx = ipa_get_ep_mapping(ipa_wdi_ctx_list[hdl]->tx_client);

	if (ipa_wdi_ctx_list[hdl]->wdi_version >= IPA_WDI_3) {
		if (ipa_wdi_ctx_list[hdl]->is_rx1_used)
			ipa_ep_idx_rx1 = ipa_get_ep_mapping(ipa_wdi_ctx_list[hdl]->rx1_client);
		if (ipa_wdi_ctx_list[hdl]->is_tx1_used)
			ipa_ep_idx_tx1 = ipa_get_ep_mapping(ipa_wdi_ctx_list[hdl]->tx1_client);
    }

	if (ipa_wdi_ctx_list[hdl]->wdi_version >= IPA_WDI_3) {
		if (ipa3_disconn_wdi3_pipes(
			ipa_ep_idx_tx, ipa_ep_idx_rx, ipa_ep_idx_tx1, ipa_ep_idx_rx1)) {
			IPA_WDI_ERR("fail to tear down wdi pipes\n");
			return -EFAULT;
		}
	} else {
		if (ipa_disconnect_wdi_pipe(ipa_wdi_ctx_list[hdl]->tx_pipe_hdl)) {
			IPA_WDI_ERR("fail to tear down wdi tx pipes\n");
			return -EFAULT;
		}
		if (ipa_disconnect_wdi_pipe(ipa_wdi_ctx_list[hdl]->rx_pipe_hdl)) {
			IPA_WDI_ERR("fail to tear down wdi rx pipes\n");
			return -EFAULT;
		}
	}

	if (ipa_pm_deregister(ipa_wdi_ctx_list[hdl]->ipa_pm_hdl)) {
		IPA_WDI_ERR("fail to deregister ipa pm\n");
		return -EFAULT;
	}

	return 0;
}
EXPORT_SYMBOL(ipa_wdi_disconn_pipes_per_inst);

/**
 * function to disable IPA offload data path
 *
 * @hdl: hdl to wdi client
 * Note: Should not be called from atomic context
 *
 * Returns: 0 on success, negative on failure
 */
int ipa_wdi_disable_pipes_per_inst(ipa_wdi_hdl_t hdl)
{
	int ret;
	int ipa_ep_idx_tx, ipa_ep_idx_rx;
	int ipa_ep_idx_tx1 = IPA_EP_NOT_ALLOCATED;
	int ipa_ep_idx_rx1 = IPA_EP_NOT_ALLOCATED;


	if (hdl < 0 || hdl >= IPA_WDI_INST_MAX) {
		IPA_WDI_ERR("Invalid Handle %d\n",hdl);
		return -EFAULT;
	}

	if (is_single_instance_wdi_version(ipa_wdi_ctx_list[hdl]->wdi_version) &&
		hdl > 0) {
		IPA_WDI_ERR("More than one instance not supported for WDI ver = %d\n",
					ipa_wdi_ctx_list[hdl]->wdi_version);
		return -EPERM;
	}

	if (!ipa_wdi_ctx_list[hdl]) {
		IPA_WDI_ERR("wdi ctx is not initialized.\n");
		return -EPERM;
	}

	ipa_ep_idx_rx = ipa_get_ep_mapping(ipa_wdi_ctx_list[hdl]->rx_client);
	ipa_ep_idx_tx = ipa_get_ep_mapping(ipa_wdi_ctx_list[hdl]->tx_client);

	if (ipa_wdi_ctx_list[hdl]->wdi_version >= IPA_WDI_3) {
		if (ipa_wdi_ctx_list[hdl]->is_rx1_used)
			ipa_ep_idx_rx1 = ipa_get_ep_mapping(ipa_wdi_ctx_list[hdl]->rx1_client);
		if (ipa_wdi_ctx_list[hdl]->is_tx1_used)
			ipa_ep_idx_tx1 = ipa_get_ep_mapping(ipa_wdi_ctx_list[hdl]->tx1_client);
    }

	if (ipa_wdi_ctx_list[hdl]->wdi_version >= IPA_WDI_3) {
		if (ipa3_disable_wdi3_pipes(
			ipa_ep_idx_tx, ipa_ep_idx_rx, ipa_ep_idx_tx1, ipa_ep_idx_rx1)) {
			IPA_WDI_ERR("fail to disable wdi pipes\n");
			return -EFAULT;
		}
	} else {
		if (ipa_suspend_wdi_pipe(ipa_wdi_ctx_list[hdl]->tx_pipe_hdl)) {
			IPA_WDI_ERR("fail to suspend wdi tx pipe\n");
			return -EFAULT;
		}
		if (ipa_disable_wdi_pipe(ipa_wdi_ctx_list[hdl]->tx_pipe_hdl)) {
			IPA_WDI_ERR("fail to disable wdi tx pipe\n");
			return -EFAULT;
		}
		if (ipa_suspend_wdi_pipe(ipa_wdi_ctx_list[hdl]->rx_pipe_hdl)) {
			IPA_WDI_ERR("fail to suspend wdi rx pipe\n");
			return -EFAULT;
		}
		if (ipa_disable_wdi_pipe(ipa_wdi_ctx_list[hdl]->rx_pipe_hdl)) {
			IPA_WDI_ERR("fail to disable wdi rx pipe\n");
			return -EFAULT;
		}
	}

	ret = ipa_pm_deactivate_sync(ipa_wdi_ctx_list[hdl]->ipa_pm_hdl);
	if (ret) {
		IPA_WDI_ERR("fail to deactivate ipa pm\n");
		return -EFAULT;
	}

	return 0;
}
EXPORT_SYMBOL(ipa_wdi_disable_pipes_per_inst);

int ipa_wdi_init(struct ipa_wdi_init_in_params *in,
	struct ipa_wdi_init_out_params *out)
{
	if (in == NULL) {
		IPA_WDI_ERR("invalid params in=%pK\n", in);
		return -EINVAL;
	}
	/*
	 * As part of previous chipset(ex:- HSP) instance id wont be
	 * passed and "ipa_wdi_init_internal" would be called.
	 * for latest chipsets wlan directly calls "ipa_wdi_init_per_inst_internal.
	 */
	in->inst_id = DEFAULT_INSTANCE_ID;
	return ipa_wdi_init_per_inst(in, out);
}
EXPORT_SYMBOL(ipa_wdi_init);

int ipa_wdi_cleanup(void)
{
	return ipa_wdi_cleanup_per_inst(0);
}
EXPORT_SYMBOL(ipa_wdi_cleanup);

int ipa_wdi_reg_intf(struct ipa_wdi_reg_intf_in_params *in)
{
	if (in == NULL) {
		IPA_WDI_ERR("invalid params in=%pK\n", in);
		return -EINVAL;
	}
	in->hdl = 0;
	return ipa_wdi_reg_intf_per_inst(in);
}
EXPORT_SYMBOL(ipa_wdi_reg_intf);

int ipa_wdi_dereg_intf(const char *netdev_name)
{
	return ipa_wdi_dereg_intf_per_inst(netdev_name, 0);
}
EXPORT_SYMBOL(ipa_wdi_dereg_intf);

int ipa_wdi_conn_pipes(struct ipa_wdi_conn_in_params *in,
			struct ipa_wdi_conn_out_params *out)
{
	if (!(in && out)) {
		IPA_WDI_ERR("empty parameters. in=%pK out=%pK\n", in, out);
		 return -EINVAL;
	}

	in->hdl = 0;
	return ipa_wdi_conn_pipes_per_inst(in, out);
}
EXPORT_SYMBOL(ipa_wdi_conn_pipes);

int ipa_wdi_disconn_pipes(void)
{
	return ipa_wdi_disconn_pipes_per_inst(0);
}
EXPORT_SYMBOL(ipa_wdi_disconn_pipes);

int ipa_wdi_enable_pipes(void)
{
	return ipa_wdi_enable_pipes_per_inst(0);
}
EXPORT_SYMBOL(ipa_wdi_enable_pipes);

int ipa_wdi_disable_pipes(void)
{
	return ipa_wdi_disable_pipes_per_inst(0);
}
EXPORT_SYMBOL(ipa_wdi_disable_pipes);

int ipa_wdi_set_perf_profile(struct ipa_wdi_perf_profile *profile)
{
	if (profile == NULL) {
		IPA_WDI_ERR("Invalid input\n");
		return -EINVAL;
	}

	return ipa_wdi_set_perf_profile_per_inst(0, profile);
}
EXPORT_SYMBOL(ipa_wdi_set_perf_profile);
