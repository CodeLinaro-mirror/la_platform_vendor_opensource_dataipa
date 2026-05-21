// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include <linux/msm_ipa.h>
#include "../ipa_common_i.h"
#include "../ipa_v3/ipa_pm.h"
#include "../ipa_v3/ipa_i.h"
#include <linux/sort.h>
#include "ipa_eth.h"
#include <linux/if_vlan.h>

#define OFFLOAD_DRV_NAME "ipa_eth"
#define IPA_ETH_DBG(fmt, args...) \
	do { \
		pr_debug(OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
	} while (0)

#define IPA_ETH_DBG_LOW(fmt, args...) \
	do { \
		pr_debug(OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
	} while (0)

#define IPA_ETH_ERR(fmt, args...) \
	do { \
		pr_err(OFFLOAD_DRV_NAME " %s:%d " fmt, \
			__func__, __LINE__, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
		IPA_IPC_LOGGING(ipa3_get_ipc_logbuf_low(), \
			OFFLOAD_DRV_NAME " %s:%d " fmt, ## args); \
	} while (0)

#define IPA_ETH_PIPES_NO 17
#define DMA_NUM_CHANNEL_EZMESH 4
#define DMA_NUM_CHANNEL_DEFAULT 2
#define DMA_NUM_CHANNEL_TSN 3

struct ipa_eth_ready_cb_wrapper {
	struct list_head link;
	struct ipa_eth_ready *info;
};

struct ipa_eth_per_client_info {
	u32 pm_hdl;
	atomic_t ref_cnt;
	bool existed;
};

struct ipa_eth_intf {
	struct list_head link;
	char netdev_name[IPA_RESOURCE_NAME_MAX];
	u8 hdr_len[IPA_IP_VLAN_MAX];
	u32 partial_hdr_hdl[IPA_IP_VLAN_MAX];
};

struct ipa_eth_context {
	struct list_head ready_cb_list;
	struct completion completion;
	struct ipa_eth_per_client_info
		client[IPA_ETH_CLIENT_MAX][IPA_ETH_INST_ID_MAX];
	struct mutex lock;
	struct workqueue_struct *wq;
	bool is_eth_ready;
	struct idr idr;
	spinlock_t idr_lock;
	struct list_head head_intf_list;
	void *client_priv;
	struct ipa_eth_qos_info tx_qos_info[IPA_ETH_INST_ID_MAX][IPA_ETH_MAX_TX_DMA_CHANNEL_QOS];
	struct ipa_eth_qos_info rx_qos_info[IPA_ETH_INST_ID_MAX][IPA_ETH_MAX_RX_DMA_CHANNEL_QOS];
	u8 rx_num_pipes[IPA_ETH_INST_ID_MAX];
	u8 tx_num_pipes[IPA_ETH_INST_ID_MAX];
};

/* Structure for double vlan ethernet header.
 * vlan_ethhdr: vlan ethernet header (ethhdr + vlan_hdr)
 * outer_vlan_id: Outer vlan id.
 * outer_vlan_encap: Packet type id.
 */
struct dbl_vlan_ethhdr
{
	struct vlan_ethhdr hdr;
	uint16_t outer_vlan_id;
	uint16_t outer_vlan_encap;
};

static struct ipa_eth_context *ipa_eth_ctx;

static int ipa_eth_uc_rdy_cb(struct notifier_block *nb,
	unsigned long action, void *data);

static void ipa_eth_ready_notify_work(struct work_struct *work);

static struct notifier_block uc_rdy_cb = {
	.notifier_call = ipa_eth_uc_rdy_cb,
};

static DECLARE_WORK(ipa_eth_ready_notify, ipa_eth_ready_notify_work);

static bool pipe_connected[IPA_ETH_PIPES_NO];
#if IPA_ETH_API_VER > 4
static bool pipe_enabled[IPA_ETH_PIPES_NO];
#endif

static int eth_qos_cmp(const void *a, const void *b) {
	if (((struct ipa_eth_qos_info *)a)->tc_bmap <= ((struct ipa_eth_qos_info *)b)->tc_bmap)
		return 1;
	return 0;
}

static void eth_qos_swap(void *a, void *b, int size) {
	struct ipa_eth_qos_info temp;

	temp = *((struct ipa_eth_qos_info *)b);
	*((struct ipa_eth_qos_info *)b) = *((struct ipa_eth_qos_info *)a);
	*((struct ipa_eth_qos_info *)a) = temp;

	return;
}

int ipa_eth_qos_get_num_pipes
(
	u8 inst_id, u8 *num_pipes, enum ipa_eth_pipe_direction dir
) {
	if (!num_pipes)
		return -1;
	if (dir == IPA_ETH_PIPE_DIR_TX)
		*num_pipes = ipa_eth_ctx->tx_num_pipes[inst_id];
	else
		*num_pipes = ipa_eth_ctx->rx_num_pipes[inst_id];
	return 0;
}
EXPORT_SYMBOL(ipa_eth_qos_get_num_pipes);

int ipa_eth_qos_get_qos_info
(
	u8 inst_id,
	u8 idx,
	struct ipa_eth_qos_info *info,
	enum ipa_eth_pipe_direction dir
) {
	if (!info)
		return -1;
	if (dir == IPA_ETH_PIPE_DIR_TX)
 		*info = ipa_eth_ctx->tx_qos_info[inst_id][idx];
	else
		*info = ipa_eth_ctx->rx_qos_info[inst_id][idx];
	return -1;
}
EXPORT_SYMBOL(ipa_eth_qos_get_qos_info);

static u8 eth_qos_get_tx_priority(u8 pipe_idx, u8 inst_id) {
	int i;
	for (i = 0; i < ipa_eth_ctx->tx_num_pipes[inst_id]; i++)
		if (ipa_eth_ctx->tx_qos_info[inst_id][i].pipe_idx == pipe_idx)
			return ipa_eth_ctx->tx_qos_info[inst_id][i].priority;
	/* Return default priority. */
	return ipa_eth_ctx->tx_num_pipes[inst_id]-1;
}

static u8 eth_qos_get_rx_priority(u8 pipe_idx, u8 inst_id) {
	int i;
	for (i = 0; i < ipa_eth_ctx->rx_num_pipes[inst_id]; i++)
		if (ipa_eth_ctx->rx_qos_info[inst_id][i].pipe_idx == pipe_idx)
			return ipa_eth_ctx->rx_qos_info[inst_id][i].priority;
	/* Return default priority. */
	return ipa_eth_ctx->rx_num_pipes[inst_id]-1;
}

static u8 client_to_pipe_index(enum ipa_client_type client_type)
{
	switch (client_type) {
	case IPA_CLIENT_ETHERNET_CONS:
		return 0;
		break;
	case IPA_CLIENT_ETHERNET_PROD:
		return 1;
		break;
	case IPA_CLIENT_RTK_ETHERNET_CONS:
		return 2;
		break;
	case IPA_CLIENT_RTK_ETHERNET_PROD:
		return 3;
		break;
	case IPA_CLIENT_AQC_ETHERNET_CONS:
		return 4;
		break;
	case IPA_CLIENT_AQC_ETHERNET_PROD:
		return 5;
		break;
	case IPA_CLIENT_ETHERNET2_CONS:
		return 6;
		break;
	case IPA_CLIENT_ETHERNET2_PROD:
		return 7;
		break;
	case IPA_CLIENT_ETHERNET_PROD1:
		return 8;
		break;
	case IPA_CLIENT_ETHERNET_LOW_LAT_CONS:
		return 9;
		break;
	case IPA_CLIENT_ETHERNET_CONS1:
		return 10;
		break;
	case IPA_CLIENT_ETHERNET_PROD2:
		return 11;
		break;
	case IPA_CLIENT_ETHERNET_CONS2:
		return 12;
		break;
	case IPA_CLIENT_ETHERNET_PROD3:
		return 13;
		break;
	case IPA_CLIENT_ETHERNET_CONS3:
		return 14;
		break;
	case IPA_CLIENT_ETHERNET_PROD4:
		return 15;
		break;
	case IPA_CLIENT_ETHERNET_CONS4:
		return 16;
		break;
	default:
		IPAERR("invalid eth client_type\n");
		ipa_assert();
	}
	return 0;
}

static int ipa_eth_init_internal(void)
{
	char buff[IPA_RESOURCE_NAME_MAX];
	int i, j;

	/* already initialized */
	if (ipa_eth_ctx)
		return 0;

	ipa_eth_ctx = kzalloc(sizeof(*ipa_eth_ctx), GFP_KERNEL);
	if (ipa_eth_ctx == NULL) {
		IPA_ETH_ERR("fail to alloc eth ctx\n");
		return -ENOMEM;
	}

	snprintf(buff, IPA_RESOURCE_NAME_MAX, "ipa_eth_wq");
	ipa_eth_ctx->wq = alloc_workqueue(buff,
		WQ_MEM_RECLAIM | WQ_UNBOUND | WQ_SYSFS, 1);
	if (!ipa_eth_ctx->wq) {
		goto wq_err;
	}
	mutex_init(&ipa_eth_ctx->lock);
	INIT_LIST_HEAD(&ipa_eth_ctx->ready_cb_list);
	ipa_eth_ctx->is_eth_ready = false;
	for (i = 0; i < IPA_ETH_CLIENT_MAX; i++) {
		for (j = 0; j < IPA_ETH_INST_ID_MAX; j++) {
			ipa_eth_ctx->client[i][j].pm_hdl = 0;
			ipa_eth_ctx->client[i][j].existed = false;
			atomic_set(&ipa_eth_ctx->client[i][j].ref_cnt, 0);
		}
	}
	idr_init(&ipa_eth_ctx->idr);
	spin_lock_init(&ipa_eth_ctx->idr_lock);
	INIT_LIST_HEAD(&ipa_eth_ctx->head_intf_list);
	ipa_eth_ctx->client_priv = NULL;
#ifdef CONFIG_DEBUG_FS
	ipa3_eth_debugfs_init();
#endif
	return 0;

wq_err:
	kfree(ipa_eth_ctx);
	ipa_eth_ctx = NULL;
	return -ENOMEM;
}

static int ipa_eth_cleanup_internal(void)
{
	struct ipa_eth_intf *entry;
	struct ipa_eth_intf *next;

	/* already deinitialized */
	if (!ipa_eth_ctx)
		return 0;
	/* clear interface list */
	list_for_each_entry_safe(entry, next,
		&ipa_eth_ctx->head_intf_list, link) {
		list_del(&entry->link);
		kfree(entry);
	}
	mutex_destroy(&ipa_eth_ctx->lock);
	destroy_workqueue(ipa_eth_ctx->wq);
	kfree(ipa_eth_ctx);
	ipa_eth_ctx = NULL;
	return 0;
}

static int ipa_eth_uc_rdy_cb(struct notifier_block *nb,
	unsigned long action, void *data)
{
	IPA_ETH_DBG("IPA uC is ready for eth");
	queue_work(ipa_eth_ctx->wq, &ipa_eth_ready_notify);
	return NOTIFY_OK;
}

static void ipa_eth_ready_notify_work(struct work_struct *work)
{
	struct ipa_eth_ready_cb_wrapper *entry;
	struct ipa_eth_ready_cb_wrapper *next;

	IPA_ETH_DBG("ipa_eth ready notify\n");
	mutex_lock(&ipa_eth_ctx->lock);
	ipa_eth_ctx->is_eth_ready = true;
	list_for_each_entry_safe(entry, next,
		&ipa_eth_ctx->ready_cb_list, link) {
		if (entry->info && entry->info->notify)
			entry->info->notify(entry->info->userdata);
		/* remove from list once notify is done */
		list_del(&entry->link);
		kfree(entry);
	}
	mutex_unlock(&ipa_eth_ctx->lock);
}

int ipa_eth_register_ready_cb(struct ipa_eth_ready *ready_info)
{
	int rc;
	struct ipa_eth_ready_cb_wrapper *ready_cb;

	/* validate user input */
	if (!ready_info) {
		IPA_ETH_ERR("null ready_info");
		return -EFAULT;
	}

	if (!ipa_eth_ctx) {
		rc = ipa_eth_init_internal();
		if (rc) {
			/* it is not normal to fail here */
			IPA_ETH_ERR("initialization failure\n");
			return rc;
		}
		IPA_ETH_DBG("ipa_eth register ready cb\n");
		mutex_lock(&ipa_eth_ctx->lock);
		ready_cb = kmalloc(sizeof(struct ipa_eth_ready_cb_wrapper),
			GFP_KERNEL);
		if (!ready_cb) {
			mutex_unlock(&ipa_eth_ctx->lock);
			ipa_eth_cleanup_internal();
			return -ENOMEM;
		}
		ready_cb->info = ready_info;
		list_add_tail(&ready_cb->link, &ipa_eth_ctx->ready_cb_list);
		mutex_unlock(&ipa_eth_ctx->lock);
		/* rely on uC ready callback, only register once */
		rc = ipa3_uc_register_ready_cb(&uc_rdy_cb);
		if (rc) {
			IPA_ETH_ERR("Failed to register ready cb\n");
			goto err_uc;
		}
	} else {
		/* assume only IOSS could register for cb */
		IPA_ETH_ERR("multiple eth register happens\n");
		mutex_lock(&ipa_eth_ctx->lock);
		ready_cb = kmalloc(sizeof(struct ipa_eth_ready_cb_wrapper),
			GFP_KERNEL);
		if (!ready_cb) {
			mutex_unlock(&ipa_eth_ctx->lock);
			return -ENOMEM;
		}
		ready_cb->info = ready_info;
		list_add_tail(&ready_cb->link, &ipa_eth_ctx->ready_cb_list);
		/* if already ready, directly callback from wq */
		if (ipa3_uc_loaded_check())
			queue_work(ipa_eth_ctx->wq, &ipa_eth_ready_notify);
		mutex_unlock(&ipa_eth_ctx->lock);
	}

	/* if uc is already ready, set the output param to true */
	if (ipa3_uc_loaded_check())
		ready_info->is_eth_ready = true;

	return 0;

err_uc:
	list_del(&ready_cb->link);
	ipa_eth_cleanup_internal();
	return rc;
}
EXPORT_SYMBOL(ipa_eth_register_ready_cb);

int ipa_eth_unregister_ready_cb(struct ipa_eth_ready *ready_info)
{
	struct ipa_eth_ready_cb_wrapper *entry;
	bool find_ready_info = false;

	/* validate user input */
	if (!ready_info) {
		IPA_ETH_ERR("null ready_info");
		return -EFAULT;
	}

	if (!ipa_eth_ctx) {
		IPA_ETH_ERR("unregister called before register\n");
		return -EFAULT;
	}

	IPA_ETH_DBG("ipa_eth unregister ready cb\n");
	mutex_lock(&ipa_eth_ctx->lock);
	list_for_each_entry(entry, &ipa_eth_ctx->ready_cb_list,
		link) {
		if (!entry)
			break;
		if (entry->info == ready_info) {
			list_del(&entry->link);
			find_ready_info = true;
			break;
		}
	}
	if (!find_ready_info) {
		IPA_ETH_ERR("unable to unregsiter, no ready_info\n");
		mutex_unlock(&ipa_eth_ctx->lock);
		return -EFAULT;
	}
	if (list_empty(&ipa_eth_ctx->ready_cb_list)) {
		mutex_unlock(&ipa_eth_ctx->lock);
		ipa_eth_cleanup_internal();
		return 0;
	}

	ready_info->is_eth_ready = false;
	mutex_unlock(&ipa_eth_ctx->lock);
	return 0;
}
EXPORT_SYMBOL(ipa_eth_unregister_ready_cb);

static u32 ipa_eth_pipe_hdl_alloc(void *ptr)
{
	ipa_eth_hdl_t hdl;

	idr_preload(GFP_KERNEL);
	spin_lock(&ipa_eth_ctx->idr_lock);
	hdl = idr_alloc(&ipa_eth_ctx->idr, ptr, 0, 0, GFP_NOWAIT);
	spin_unlock(&ipa_eth_ctx->idr_lock);
	idr_preload_end();

	return hdl;
}

static void ipa_eth_pipe_hdl_remove(ipa_eth_hdl_t hdl)
{
	spin_lock(&ipa_eth_ctx->idr_lock);
	idr_remove(&ipa_eth_ctx->idr, hdl);
	spin_unlock(&ipa_eth_ctx->idr_lock);
}

static enum ipa_client_type
	ipa_eth_get_ipa_client_type_from_pipe(
	struct ipa_eth_client_pipe_info *pipe,
	u8 rx_pipe_idx,
	u8 tx_pipe_idx)
{
	struct ipa_eth_client *client;
	int ipa_client_type = IPA_CLIENT_MAX;
	int traffic_type = 0;

	if (!pipe) {
		IPA_ETH_ERR("invalid pipe\n");
		return ipa_client_type;
	}

	client = pipe->client_info;
	if (!client) {
		IPA_ETH_ERR("invalid client\n");
		return ipa_client_type;
	}

#if IPA_ETH_API_VER >= 3
	traffic_type = pipe->traffic_type;
#else
	traffic_type = client->traffic_type;
#endif

	IPA_ETH_DBG("pipe %p traffic_type %d dir %d, client_type %d,"
				"rx idx %d tx idx %d\n",
				pipe, traffic_type, pipe->dir, client->client_type,
				rx_pipe_idx, tx_pipe_idx);

	switch (client->client_type) {
	case IPA_ETH_CLIENT_AQC107:
	case IPA_ETH_CLIENT_AQC113:
		if (traffic_type ==
			IPA_ETH_PIPE_BEST_EFFORT &&
			client->inst_id == 0) {
			if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
				ipa_client_type =
					IPA_CLIENT_AQC_ETHERNET_CONS;
			} else {
				ipa_client_type =
					IPA_CLIENT_AQC_ETHERNET_PROD;
			}
		}
		break;
	case IPA_ETH_CLIENT_RTK8111K:
	case IPA_ETH_CLIENT_RTK8125B:
		if (traffic_type == IPA_ETH_PIPE_BEST_EFFORT) {
			if (client->inst_id == 0) {
				if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
					ipa_client_type =
						IPA_CLIENT_RTK_ETHERNET_CONS;
				} else {
					ipa_client_type =
						IPA_CLIENT_RTK_ETHERNET_PROD;
				}

			} else if (client->inst_id == 1) {
				if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
					ipa_client_type =
						IPA_CLIENT_ETHERNET2_CONS;
				} else {
					ipa_client_type =
						IPA_CLIENT_ETHERNET2_PROD;
				}
			}
		}

		break;
	case IPA_ETH_CLIENT_NTN:
	case IPA_ETH_CLIENT_EMAC:
		if (traffic_type ==
			IPA_ETH_PIPE_BEST_EFFORT &&
			client->inst_id == 0) {
			if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
				ipa_client_type =
					IPA_CLIENT_ETHERNET_CONS;
			} else {
				ipa_client_type =
					IPA_CLIENT_ETHERNET_PROD;
			}
		}
		break;
#if IPA_ETH_API_VER >= 2
	case IPA_ETH_CLIENT_NTN3:
	case IPA_ETH_CLIENT_IEMAC:
		if (traffic_type == IPA_ETH_PIPE_BEST_EFFORT) {
			if (client->inst_id == 0) {
				if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
					ipa_client_type =
						IPA_CLIENT_ETHERNET_CONS;
				} else {
					ipa_client_type =
						IPA_CLIENT_ETHERNET_PROD;
				}
			} else if (client->inst_id == 1) {
				if (ipa3_ctx->tsn_iface &&
				    client->client_type ==
					    IPA_ETH_CLIENT_IEMAC) {
					IPA_ETH_ERR(
						"TSN mode doesnot support inst1\n");
					return IPA_CLIENT_MAX;
				}
				if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
					ipa_client_type =
						IPA_CLIENT_ETHERNET2_CONS;
				} else {
					ipa_client_type =
						IPA_CLIENT_ETHERNET2_PROD;
				}
			}
		} else if (traffic_type == IPA_ETH_PIPE_BEST_EFFORT_VLAN) {
			if (client->inst_id == 0) {
				if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
					ipa_client_type =
						IPA_CLIENT_ETHERNET_CONS;
				} else {
					ipa_client_type =
						IPA_CLIENT_ETHERNET_PROD1;
				}
			}
		}
		if ((ipa3_ctx->tsn_iface) &&
		    (traffic_type == IPA_ETH_PIPE_LOW_LATENCY) &&
		    (client->client_type == IPA_ETH_CLIENT_IEMAC)) {
			if (client->inst_id == 0) {
				if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
					ipa_client_type =
						IPA_CLIENT_ETHERNET_LOW_LAT_CONS;
				} else {
					IPA_ETH_ERR(
						"TSN mode rx low lat pipe not supported\n");
					ipa_client_type = IPA_CLIENT_MAX;
				}
			}
		} else if (traffic_type == IPA_ETH_PIPE_BEST_EFFORT_VLAN) {
			if (client->inst_id == 0) {
				if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
					ipa_client_type = IPA_CLIENT_ETHERNET_CONS;
				} else {
					ipa_client_type = IPA_CLIENT_ETHERNET_PROD1;
				}
			}
		} else if (traffic_type == IPA_ETH_PIPE_TRAFFIC_TYPE_QOS) {
			if (ipa3_ctx->ipa_config_is_auto && client->inst_id == 0) {
				if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
					switch (tx_pipe_idx) {
						case 0:
							ipa_client_type = IPA_CLIENT_ETHERNET_CONS;
							break;
						case 1:
							ipa_client_type = IPA_CLIENT_ETHERNET_CONS1;
							break;
						case 2:
							ipa_client_type = IPA_CLIENT_ETHERNET_CONS2;
							break;
						case 3:
							ipa_client_type = IPA_CLIENT_ETHERNET_CONS3;
							break;
						case 4:
							ipa_client_type = IPA_CLIENT_ETHERNET_CONS4;
							break;
						default:
							IPA_ETH_ERR("invalid client index%d\n",
								tx_pipe_idx);
							return ipa_client_type;
					}
				} else {
					ipa_client_type = IPA_CLIENT_ETHERNET_PROD1;
					switch (rx_pipe_idx) {
						case 0:
							ipa_client_type = IPA_CLIENT_ETHERNET_PROD;
							break;
						case 1:
							ipa_client_type = IPA_CLIENT_ETHERNET_PROD1;
							break;
						case 2:
							ipa_client_type = IPA_CLIENT_ETHERNET_PROD2;
							break;
						case 3:
							ipa_client_type = IPA_CLIENT_ETHERNET_PROD3;
							break;
						default:
							IPA_ETH_ERR("invalid client index%d\n",
								rx_pipe_idx);
							return ipa_client_type;
					}
				}
			} else {
				if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
					switch (tx_pipe_idx) {
						case 0:
							ipa_client_type = IPA_CLIENT_ETHERNET_CONS;
							break;
						case 1:
							ipa_client_type = IPA_CLIENT_ETHERNET_LOW_LAT_CONS;
							break;
						default:
							IPA_ETH_ERR("invalid client index%d\n",
								tx_pipe_idx);
							return ipa_client_type;
					}
				} else {
					switch (rx_pipe_idx) {
						case 0:
							ipa_client_type = IPA_CLIENT_ETHERNET_PROD;
							break;
						case 1:
							ipa_client_type = IPA_CLIENT_ETHERNET_PROD1;
							break;
						default:
							IPA_ETH_ERR("invalid client index%d\n",
								rx_pipe_idx);
							return ipa_client_type;
					}
				}
			}
		}
		break;
#endif
	default:
		IPA_ETH_ERR("invalid client type%d\n",
			client->client_type);
	}
	return ipa_client_type;
}

#if IPA_ETH_API_VER < 2
static struct ipa_eth_client_pipe_info
	*ipa_eth_get_pipe_from_hdl(ipa_eth_hdl_t hdl)
{
	struct ipa_eth_client_pipe_info *pipe;

	spin_lock(&ipa_eth_ctx->idr_lock);
	pipe = idr_find(&ipa_eth_ctx->idr, hdl);
	spin_unlock(&ipa_eth_ctx->idr_lock);

	return pipe;
}
#endif

static int ipa_eth_client_connect_pipe(
	struct ipa_eth_client_pipe_info *pipe,
	int inst_id,
	u8 rx_pipe_idx,
	u8 tx_pipe_idx)
{
	enum ipa_client_type client_type;
	struct ipa_eth_client *client;
	int ret;
	u8 priority = 0, pipe_idx = 0;

	if (!pipe) {
		IPA_ETH_ERR("invalid pipe\n");
		return -EFAULT;
	}
	client = pipe->client_info;
	if (!client) {
		IPA_ETH_ERR("invalid client\n");
		return -EFAULT;
	}
	client_type =
		ipa_eth_get_ipa_client_type_from_pipe(pipe, rx_pipe_idx, tx_pipe_idx);
	if (client_type == IPA_CLIENT_MAX) {
		IPA_ETH_ERR("invalid client type\n");
		return -EFAULT;
	}

	if (pipe_connected[client_to_pipe_index(client_type)]) {
		IPA_ETH_ERR("client already connected\n");
		return -EFAULT;
	}

	pipe->pipe_hdl = ipa_eth_pipe_hdl_alloc((void *)pipe);

	if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
		priority = eth_qos_get_tx_priority(tx_pipe_idx, inst_id);
		pipe_idx = tx_pipe_idx;
	} else {
		priority = eth_qos_get_rx_priority(rx_pipe_idx, inst_id);
		pipe_idx = rx_pipe_idx;
	}

	IPADBG("Client_type: %d, Inst_id: %d, Priority: %d, Pipe_idx= %d",
		client_type, inst_id, priority, pipe_idx);
	ret = ipa3_eth_connect(pipe, client_type, inst_id, priority, pipe_idx);
	if (!ret) {
		pipe_connected[client_to_pipe_index(client_type)] = true;
	}

	return ret;
}

static int ipa_eth_client_disconnect_pipe(
	struct ipa_eth_client_pipe_info *pipe,
	u8 rx_pipe_idx,
	u8 tx_pipe_idx)
{
	enum ipa_client_type client_type;
	struct ipa_eth_client *client;
	int result;
	u8 pipe_idx;

	if (!pipe) {
		IPA_ETH_ERR("invalid pipe\n");
		return -EFAULT;
	}

	client = pipe->client_info;
	if (!client) {
		IPA_ETH_ERR("invalid client\n");
		return -EFAULT;
	}
	client_type =
		ipa_eth_get_ipa_client_type_from_pipe(pipe, rx_pipe_idx, tx_pipe_idx);
	if (client_type == IPA_CLIENT_MAX) {
		IPA_ETH_ERR("invalid client type\n");
		return -EFAULT;
	}

	if (!pipe_connected[client_to_pipe_index(client_type)]) {
		IPA_ETH_ERR("client not connected\n");
		return -EFAULT;
	}

	if (pipe->dir == IPA_ETH_PIPE_DIR_TX)
		pipe_idx = tx_pipe_idx;
	else
		pipe_idx = rx_pipe_idx;

	result = ipa3_eth_disconnect(pipe, client_type, pipe_idx);
	if (result)
		return result;

	pipe_connected[client_to_pipe_index(client_type)] = false;

	ipa_eth_pipe_hdl_remove(pipe->pipe_hdl);
	return 0;
}

#if IPA_ETH_API_VER > 4
static int ipa_eth_client_enable_pipe(
	struct ipa_eth_client_pipe_info *pipe,
	int inst_id,
	u8 rx_pipe_idx,
	u8 tx_pipe_idx)
{
	enum ipa_client_type client_type;
	struct ipa_eth_client *client;
	int ret;
	u8 priority = 0, pipe_idx = 0;

	IPA_ETH_DBG("client %d, inst_id %d, rx_idx %d, tx_idx %d\n",
		pipe->client_info->client_type, inst_id, rx_pipe_idx, tx_pipe_idx);

	if (!pipe) {
		IPA_ETH_ERR("invalid pipe\n");
		return -EFAULT;
	}
	client = pipe->client_info;
	if (!client) {
		IPA_ETH_ERR("invalid client\n");
		return -EFAULT;
	}
	client_type =
		ipa_eth_get_ipa_client_type_from_pipe(pipe, rx_pipe_idx, tx_pipe_idx);
	if (client_type == IPA_CLIENT_MAX) {
		IPA_ETH_ERR("invalid client type %d\n", client_type);
		return -EFAULT;
	}

	if (pipe_enabled[client_to_pipe_index(client_type)]) {
		IPA_ETH_ERR("client already enabled\n");
		return -EFAULT;
	}

	if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
		priority = eth_qos_get_tx_priority(tx_pipe_idx, inst_id);
		pipe_idx = tx_pipe_idx;
	} else {
		priority = eth_qos_get_rx_priority(rx_pipe_idx, inst_id);
		pipe_idx = rx_pipe_idx;
	}

	IPADBG("Client_type: %d, Inst_id: %d, Priority: %d, Pipe_idx= %d",
		client_type, inst_id, priority, pipe_idx);
	ret = ipa3_eth_enable(pipe, client_type, inst_id, priority, pipe_idx);
	if (!ret) {
		pipe_enabled[client_to_pipe_index(client_type)] = true;
	}

	return ret;
}

static int ipa_eth_client_disable_pipe(
	struct ipa_eth_client_pipe_info *pipe,
	int inst_id,
	u8 rx_pipe_idx,
	u8 tx_pipe_idx)
{
	enum ipa_client_type client_type;
	u8 pipe_idx = 0;
	int ret;

	if (!pipe) {
		IPA_ETH_ERR("invalid pipe\n");
		return -EFAULT;
	}

	client_type =
		ipa_eth_get_ipa_client_type_from_pipe(pipe, rx_pipe_idx, tx_pipe_idx);
	if (client_type == IPA_CLIENT_MAX) {
		IPA_ETH_ERR("invalid client type\n");
		return -EFAULT;
	}

	if (!pipe_enabled[client_to_pipe_index(client_type)]) {
		IPA_ETH_ERR("client not enabled\n");
		return -EFAULT;
	}

	if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
		pipe_idx = tx_pipe_idx;
	} else {
		pipe_idx = rx_pipe_idx;
	}


	IPADBG("Client_type: %d, Inst_id: %d, Pipe_idx= %d",
		client_type, inst_id, pipe_idx);
	ret = ipa3_eth_disable(pipe, client_type, pipe_idx);
	if (!ret) {
		pipe_enabled[client_to_pipe_index(client_type)] = false;
	}

	pipe_enabled[client_to_pipe_index(client_type)] = false;

	return 0;
}
#endif

static int ipa_eth_commit_partial_hdr(
	struct ipa_ioc_add_hdr *hdr,
	const char *netdev_name,
	struct ipa_eth_hdr_info *hdr_info,
	int num_hdrs)
{
	int i;

	if (!hdr || !hdr_info || !netdev_name) {
		IPA_ETH_ERR("Invalid input\n");
		return -EINVAL;
	}

	hdr->commit = 0;
	hdr->num_hdrs = num_hdrs;

	IPA_ETH_DBG("num_hdrs vlan %d", hdr->num_hdrs);

	snprintf(hdr->hdr[0].name, sizeof(hdr->hdr[0].name),
			 "%s_ipv4", netdev_name);
	snprintf(hdr->hdr[1].name, sizeof(hdr->hdr[1].name),
			 "%s_ipv6", netdev_name);

#if IPA_ETH_API_VER >= 6
	if((ipa3_ctx->device_mode == DEVMODE_APBRIDGE) &&
					ipa3_ctx->device_vlan_mode)
	{
		snprintf(hdr->hdr[0].name, sizeof(hdr->hdr[0].name),
				"%s_ipv4_qinq", netdev_name);
		snprintf(hdr->hdr[1].name, sizeof(hdr->hdr[1].name),
				"%s_ipv6_qinq", netdev_name);

	}
	else
#endif
	if (hdr->num_hdrs > 2) {
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
		IPA_ETH_DBG("hdr len %d eth2_ofst %d\n",hdr_info[i].hdr_len, hdr_info[i].dst_mac_addr_offset);
	}

	if (ipa_add_hdr(hdr)) {
		IPA_ETH_ERR("fail to add partial headers\n");
		return -EFAULT;
	}

	return 0;
}


static void ipa_eth_pm_cb(void *p, enum ipa_pm_cb_event event)
{
	IPA_ETH_ERR("received pm event %d\n", event);
}

static int ipa_eth_pm_register(struct ipa_eth_client *client)
{
	struct ipa_pm_register_params pm_params;
	int client_type, inst_id;
	char name[IPA_RESOURCE_NAME_MAX];
	struct ipa_eth_client_pipe_info *pipe;
	int rc;
	u8 rx_pipe_idx = 0, tx_pipe_idx = 0;

	/* validate user input */
	if (!client || (client->client_type >= IPA_ETH_CLIENT_MAX)) {
		IPA_ETH_ERR("null client or eth client doesn't exist");
		return -EFAULT;
	}
	client_type = client->client_type;
	inst_id = client->inst_id;

	if (atomic_read(
		&ipa_eth_ctx->client[client_type][inst_id].ref_cnt))
		goto add_pipe_list;

	memset(&pm_params, 0, sizeof(pm_params));
	snprintf(name, IPA_RESOURCE_NAME_MAX,
		"ipa_eth_%d_%d", client_type, inst_id);
	pm_params.name = name;
	pm_params.callback = ipa_eth_pm_cb;
	pm_params.user_data = NULL;
	pm_params.group = IPA_PM_GROUP_DEFAULT;
	if (ipa_pm_register(&pm_params,
		&ipa_eth_ctx->client[client_type][inst_id].pm_hdl)) {
		IPA_ETH_ERR("fail to register ipa pm\n");
		return -EFAULT;
	}
	/* vote IPA clock on */
	rc = ipa_pm_activate_sync(
		ipa_eth_ctx->client[client_type][inst_id].pm_hdl);
	if (rc) {
		IPA_ETH_ERR("fail to activate ipa pm\n");
		return -EFAULT;
	}
add_pipe_list:
	list_for_each_entry(pipe, &client->pipe_list,
		link) {
#if IPA_ETH_API_VER >= 3
		IPA_ETH_DBG("pipe %p traffic_type %d dir %d\n",
				pipe, pipe->traffic_type, pipe->dir);
#endif
		if (pipe->dir == IPA_ETH_PIPE_DIR_RX
#if IPA_ETH_API_VER >= 3
			|| pipe->traffic_type == IPA_ETH_PIPE_BEST_EFFORT_VLAN
#endif
			) {
				rx_pipe_idx++;
				continue;
			}
		rc = ipa_pm_associate_ipa_cons_to_client(
			ipa_eth_ctx->client[client_type][inst_id].pm_hdl,
			ipa_eth_get_ipa_client_type_from_pipe(pipe, rx_pipe_idx, tx_pipe_idx));
		if (rc) {
			IPA_ETH_ERR("fail to associate cons with PM %d\n", rc);
			ipa_pm_deregister(
			ipa_eth_ctx->client[client_type][inst_id].pm_hdl);
			ipa_eth_ctx->client[client_type][inst_id].pm_hdl = 0;
			ipa_assert();
			return rc;
		}
		if (pipe->dir == IPA_ETH_PIPE_DIR_TX)
			tx_pipe_idx++;
		if (pipe->dir == IPA_ETH_PIPE_DIR_RX)
			rx_pipe_idx++;
	}
	atomic_inc(
		&ipa_eth_ctx->client[client_type][inst_id].ref_cnt);
	return 0;
}

static int ipa_eth_pm_deregister(struct ipa_eth_client *client)
{
	int rc;
	int client_type, inst_id;

	/* validate user input */
	if (!client || (client->client_type >= IPA_ETH_CLIENT_MAX)) {
		IPA_ETH_ERR("null client or client type not defined");
		return -EFAULT;
	}
	client_type = client->client_type;
	inst_id = client->inst_id;
	if (atomic_read(
		&ipa_eth_ctx->client[client_type][inst_id].ref_cnt)
		== 1) {
		rc = ipa_pm_deactivate_sync(
			ipa_eth_ctx->client[client_type][inst_id].pm_hdl);
		if (rc) {
			IPA_ETH_ERR("fail to deactivate ipa pm\n");
			return -EFAULT;
		}
		if (ipa_pm_deregister(
			ipa_eth_ctx->client[client_type][inst_id].pm_hdl)) {
			IPA_ETH_ERR("fail to deregister ipa pm\n");
			return -EFAULT;
		}
	}
	atomic_dec(&ipa_eth_ctx->client[client_type][inst_id].ref_cnt);
	return 0;
}

int ipa_eth_client_conn_pipes(struct ipa_eth_client *client)
{
	struct ipa_eth_client_pipe_info *pipe;
	int rc, i = 0, prio = 0;
	int client_type, inst_id, traff_type, ep_idx, rx_idx = 0,
						     tx_idx[2] = { 0 };
	u8 rx_pipe_idx = 0, tx_pipe_idx = 0;
	struct ipa_endp_desc_indication_msg_v01 req;
	struct ipa_ep_id_type_v01 *ep_info, *ep2_info;
	enum ipa_client_type ipa_client;
	int max_tx, max_rx;

	if (ipa3_ctx->ipa_config_is_auto)
	{
		max_tx = IPA_ETH_MAX_TX_DMA_CHANNEL_QOS_AUTO;
		max_rx = IPA_ETH_MAX_RX_DMA_CHANNEL_QOS_AUTO;
	}
	else
	{
		max_tx = IPA_ETH_MAX_TX_DMA_CHANNEL_QOS_CPE;
		max_rx = IPA_ETH_MAX_RX_DMA_CHANNEL_QOS_CPE;
	}

	memset(&req, 0, sizeof(struct ipa_endp_desc_indication_msg_v01));

	/* validate user input */
	if (!client || (client->client_type >= IPA_ETH_CLIENT_MAX)) {
		IPA_ETH_ERR("null client or client type not defined");
		return -EFAULT;
	}
	if (!ipa_eth_ctx) {
		IPA_ETH_ERR("connect called before register readiness\n");
		return -EFAULT;
	}

	if (ipa3_ctx->ipa_tiering_value & IPA_TIERING_DISABLE_ETH) {
			IPA_ETH_ERR("ETH offload is disabled by IPA Tiering, client %d\n",
				client->client_type);
			return -EFAULT;
	}

	if (!ipa_eth_ctx->is_eth_ready) {
		IPA_ETH_ERR("conn called before IPA eth ready\n");
		return -EFAULT;
	}
	ipa_eth_ctx->client_priv = client->priv;
	client_type = client->client_type;
	inst_id = client->inst_id;

#if IPA_ETH_API_VER >= 3
	IPA_ETH_DBG("ipa_eth conn client %d inst %d\n", client_type, inst_id);
#else
	traff_type = client->traffic_type;
	IPA_ETH_DBG("ipa_eth conn client %d inst %d, traffic %d\n",
			client_type, inst_id, traff_type);
#endif

	mutex_lock(&ipa_eth_ctx->lock);
	rc = ipa_eth_pm_register(client);
	if (rc) {
		IPA_ETH_ERR("pm register failed\n");
		mutex_unlock(&ipa_eth_ctx->lock);
		return -EFAULT;
	}

	if (ipa3_ctx->eth_qos) {
		ipa_eth_ctx->rx_num_pipes[inst_id] = 0;
		ipa_eth_ctx->tx_num_pipes[inst_id] = 0;
		memset(ipa_eth_ctx->rx_qos_info[inst_id], 0,
			max_rx * sizeof(struct ipa_eth_qos_info));
		memset(ipa_eth_ctx->tx_qos_info[inst_id], 0,
			max_tx * sizeof(struct ipa_eth_qos_info));
		list_for_each_entry(pipe, &client->pipe_list,
			link) {
			if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
				ipa_eth_ctx->tx_qos_info[inst_id][tx_pipe_idx].tc_bmap = pipe->tc_bmap;
				ipa_eth_ctx->tx_qos_info[inst_id][tx_pipe_idx].pipe_idx =
					tx_pipe_idx;
				ipa_eth_ctx->tx_qos_info[inst_id][tx_pipe_idx].priority = 0;
				ipa_eth_ctx->tx_qos_info[inst_id][tx_pipe_idx].client_type =
					ipa_eth_get_ipa_client_type_from_pipe(pipe, rx_pipe_idx,
						tx_pipe_idx);
				tx_pipe_idx++;
				ipa_eth_ctx->tx_num_pipes[inst_id]++;
			}
			if (pipe->dir == IPA_ETH_PIPE_DIR_RX) {
				ipa_eth_ctx->rx_qos_info[inst_id][rx_pipe_idx].tc_bmap = pipe->tc_bmap;
				ipa_eth_ctx->rx_qos_info[inst_id][rx_pipe_idx].pipe_idx =
					rx_pipe_idx;
				ipa_eth_ctx->rx_qos_info[inst_id][rx_pipe_idx].priority = 0;
				ipa_eth_ctx->rx_qos_info[inst_id][rx_pipe_idx].client_type =
					ipa_eth_get_ipa_client_type_from_pipe(pipe, rx_pipe_idx,
						tx_pipe_idx);
				rx_pipe_idx++;
				ipa_eth_ctx->rx_num_pipes[inst_id]++;
			}
		}
		/* Sort the elements based on tc_map. */
		sort(ipa_eth_ctx->tx_qos_info[inst_id], ipa_eth_ctx->tx_num_pipes[inst_id],
			sizeof(struct ipa_eth_qos_info), eth_qos_cmp, eth_qos_swap);
		sort(ipa_eth_ctx->rx_qos_info[inst_id], ipa_eth_ctx->rx_num_pipes[inst_id],
			sizeof(struct ipa_eth_qos_info), eth_qos_cmp, eth_qos_swap);
		/* calculate relative priority. */
		for (i = 0, prio = 0; i < ipa_eth_ctx->tx_num_pipes[inst_id] &&
				i < max_tx; i++) {
			if (ipa_eth_ctx->tx_qos_info[inst_id][i].tc_bmap == 0) {
				/* Default is Max priority. */
				ipa_eth_ctx->tx_qos_info[inst_id][i].priority =
					ipa_eth_ctx->tx_num_pipes[inst_id]-1;
			} else {
				ipa_eth_ctx->tx_qos_info[inst_id][i].priority = prio;
				prio++;
			}
			IPADBG("TX Index: %d, Pipe idx: %d, Tc_bmap: %d, Priority: %d", i,
				ipa_eth_ctx->tx_qos_info[inst_id][i].pipe_idx,
				ipa_eth_ctx->tx_qos_info[inst_id][i].tc_bmap,
				ipa_eth_ctx->tx_qos_info[inst_id][i].priority);
		}
		for (i = 0, prio = 0; i < ipa_eth_ctx->rx_num_pipes[inst_id] &&
				i < max_rx; i++) {
			if (ipa_eth_ctx->rx_qos_info[inst_id][i].tc_bmap == 0) {
				/* Default is Max priority. */
				ipa_eth_ctx->rx_qos_info[inst_id][i].priority =
					ipa_eth_ctx->rx_num_pipes[inst_id]-1;
			} else {
				ipa_eth_ctx->rx_qos_info[inst_id][i].priority = prio;
				prio++;
			}
			IPADBG("RX Index: %d, Pipe idx: %d, Tc_bmap: %d, Priority: %d", i,
				ipa_eth_ctx->rx_qos_info[inst_id][i].pipe_idx,
				ipa_eth_ctx->rx_qos_info[inst_id][i].tc_bmap,
				ipa_eth_ctx->rx_qos_info[inst_id][i].priority);
		}
	}

	rx_pipe_idx = 0;
	tx_pipe_idx = 0;
	list_for_each_entry(pipe, &client->pipe_list,
		link) {
#if IPA_ETH_API_VER >= 3
		IPA_ETH_DBG("Eth connect pipe %p traffic_type %d dir %d\n",
				pipe, pipe->traffic_type, pipe->dir);
		if (pipe->traffic_type == IPA_ETH_PIPE_BEST_EFFORT_VLAN &&
			pipe->dir == IPA_ETH_PIPE_DIR_TX) {
			IPA_ETH_DBG("traffic_type %d dir %d continue...\n",
				pipe->traffic_type, pipe->dir);
			tx_pipe_idx++;
			continue;
		}
#endif
		rc = ipa_eth_client_connect_pipe(pipe, inst_id, rx_pipe_idx, tx_pipe_idx);
		if (rc) {
			IPA_ETH_ERR("pipe connect fails\n");
			ipa_assert();
		}

#if IPA_ETH_API_VER >= 3
		traff_type = pipe->traffic_type;
		IPA_ETH_DBG("ipa_eth conn traffic %d\n", traff_type);
#endif
		if (pipe->dir == IPA_ETH_PIPE_DIR_TX)
			tx_pipe_idx++;
		if (pipe->dir == IPA_ETH_PIPE_DIR_RX)
			rx_pipe_idx++;

		if ((ipa3_ctx->eth_pdu_ctx.eth_pdu_mode_enabled) &&
		    (traff_type != IPA_ETH_PIPE_LOW_LATENCY)) {
			/* populate the QMI */
			ipa_client = ipa_eth_get_ipa_client_type_from_pipe(pipe,
							rx_pipe_idx, tx_pipe_idx);
			ep_idx = ipa_get_ep_mapping(ipa_client);

			/* NOTE: Only support single NIC for eth_pdu */
			if ((IPA_CLIENT_IS_PROD(ipa_client) && tx_idx[0]) &&
			    (IPA_CLIENT_IS_CONS(ipa_client) && rx_idx)) {
				IPAERR("QMI already set for ETH PDU tx id:%d rx id:%d\n",
				       tx_idx[0], rx_idx);
				continue;
			}
			req.ep_info_len++;
			req.ep_info_valid = true;
			req.num_eps_valid = true;
			req.num_eps++;
			ep_info = &req.ep_info[req.ep_info_len - 1];
			ep_info->ep_id = ep_idx;
			ep_info->ic_type = DATA_IC_TYPE_ETH_V01;

			if (IPA_CLIENT_IS_PROD(ipa_client)) {
				ep_info->ep_type =
					DATA_EP_DESC_TYPE_TETH_CONS_V01;
				rx_idx = ep_idx;
			} else if (IPA_CLIENT_IS_CONS(ipa_client)) {
				ep_info->ep_type =
					DATA_EP_DESC_TYPE_TETH_PROD_V01;
				tx_idx[0] = ep_idx;
			}
			ep_info->ep_status = DATA_EP_STATUS_CONNECTED_V01;
			/* Adding IPA_CLIENT_APPS_LAN_CONS pipe info in QMI
			 * for eth pdu enabled cases */
			req.ep_info_len++;
			req.num_eps++;
			ep2_info = &req.ep_info[req.ep_info_len - 1];
			ep2_info->ep_id = ipa_get_ep_mapping(IPA_CLIENT_APPS_LAN_CONS);
			ep2_info->ic_type = DATA_IC_TYPE_RESERVED_V01;
			ep2_info->ep_type = DATA_EP_DESC_TYPE_RESERVED_V01;
			ep2_info->ep_status = DATA_EP_STATUS_RESERVED_V01;
		} else if ((ipa3_ctx->eth_pdu_ctx.eth_pdu_mode_enabled) &&
			   (traff_type == IPA_ETH_PIPE_LOW_LATENCY)) {
			/* Populate the QMI */
			ipa_client = ipa_eth_get_ipa_client_type_from_pipe(pipe,
							rx_pipe_idx, tx_pipe_idx);
			ep_idx = ipa_get_ep_mapping(ipa_client);

			if (IPA_CLIENT_IS_PROD(ipa_client)) {
				IPAERR("TSN rx not supported\n");
				return -EFAULT;
			}

			/* NOTE: Only support single NIC for eth_pdu tsn*/
			if (IPA_CLIENT_IS_CONS(ipa_client) && tx_idx[1]) {
				IPAERR("QMI already set for ETH PDU tsn tx id:%d \n",
				       tx_idx[1]);
				continue;
			}
			req.ep_info_len++;
			req.ep_info_valid = true;
			req.num_eps_valid = true;
			req.num_eps++;
			ep_info = &req.ep_info[req.ep_info_len - 1];
			ep_info->ep_id = ep_idx;
			ep_info->ic_type = DATA_IC_TYPE_ETH_V01;

			if (IPA_CLIENT_IS_CONS(ipa_client)) {
				ep_info->ep_type =
					DATA_EP_DESC_TYPE_TETH_LL_PROD_V01;
				tx_idx[1] = ep_idx;
			}
			ep_info->ep_status = DATA_EP_STATUS_CONNECTED_V01;
			/* Adding IPA_CLIENT_APPS_LAN_CONS pipe info in QMI
			 * for eth pdu enabled cases */
			req.ep_info_len++;
			req.num_eps++;
			ep2_info = &req.ep_info[req.ep_info_len - 1];
			ep2_info->ep_id = ipa_get_ep_mapping(IPA_CLIENT_APPS_LAN_CONS);
			ep2_info->ic_type = DATA_IC_TYPE_RESERVED_V01;
			ep2_info->ep_type = DATA_EP_DESC_TYPE_RESERVED_V01;
			ep2_info->ep_status = DATA_EP_STATUS_RESERVED_V01;
		}
	}
	if (!ipa_eth_ctx->client[client_type][inst_id].existed) {
#ifdef CONFIG_DEBUG_FS
		ipa3_eth_debugfs_add_node(client);
#endif
		ipa_eth_ctx->client[client_type][inst_id].existed = true;
	}

	if (ipa3_ctx->eth_pdu_ctx.eth_pdu_mode_enabled)
	{
		ipa3_update_eth_pdu_ep_index(rx_idx, tx_idx);

		IPADBG("Sending ETH PDU endpoint QMI for client\n");
		if (req.ep_info_len > 0)
			if (ipa3_qmi_send_endp_desc_indication(&req))
				IPAERR("Failed to send eth pipe endp desc QMI\n");
	}

	mutex_unlock(&ipa_eth_ctx->lock);
	return 0;
}
EXPORT_SYMBOL(ipa_eth_client_conn_pipes);

int ipa_eth_client_disconn_pipes(struct ipa_eth_client *client)
{
	int rc;
	int rx_idx = 0,tx_idx[2] = {0};
	struct ipa_eth_client_pipe_info *pipe;
	struct ipa_ep_cfg_holb holb;
	int  rx_pipe_idx = 0, tx_pipe_idx = 0;

	/* validate user input */
	if (!client) {
		IPA_ETH_ERR("null client");
		return -EFAULT;
	}

	if (!ipa_eth_ctx) {
		IPA_ETH_ERR("disconn called before register readiness\n");
		return -EFAULT;
	}

	if (!ipa_eth_ctx->is_eth_ready) {
		IPA_ETH_ERR("disconn called before IPA eth ready\n");
		return -EFAULT;
	}

#if IPA_ETH_API_VER >= 3
	IPA_ETH_DBG("ipa_eth disconn client %d inst %d\n",
		client->client_type, client->inst_id);
#else
	IPA_ETH_DBG("ipa_eth disconn client %d inst %d, traffic %d\n",
		client->client_type, client->inst_id,
		client->traffic_type);
#endif

	mutex_lock(&ipa_eth_ctx->lock);

	/* set holb on tx pipes first */
	list_for_each_entry(pipe, &client->pipe_list,
		link) {
		if (pipe->dir == IPA_ETH_PIPE_DIR_TX)
		{
#if IPA_ETH_API_VER >= 3
			IPA_ETH_DBG("ipa_eth disconn traffic %d\n",
				    pipe->traffic_type);
#endif
			IPA_ETH_DBG("Set holb on pipe = %d, pipe->dir = %d \n",
				ipa_get_ep_mapping(ipa_eth_get_ipa_client_type_from_pipe(pipe, rx_pipe_idx, tx_pipe_idx)),
				pipe->dir);
			holb.en = 1;
			holb.tmr_val = 0;
			ipa3_cfg_ep_holb(ipa_get_ep_mapping(
				ipa_eth_get_ipa_client_type_from_pipe(pipe, rx_pipe_idx, tx_pipe_idx)), &holb);
			tx_pipe_idx++;
		}
	}

	rx_pipe_idx = 0;
	tx_pipe_idx = 0;
	list_for_each_entry(pipe, &client->pipe_list,
		link) {
#if IPA_ETH_API_VER >= 3
		if (pipe->traffic_type == IPA_ETH_PIPE_BEST_EFFORT_VLAN
			&& pipe->dir == IPA_ETH_PIPE_DIR_TX) {
			IPA_ETH_DBG("traffic_type %d dir %d continue... \n",
				pipe->traffic_type, pipe->dir);
			tx_pipe_idx++;
			continue;
		}
#endif
		rc = ipa_eth_client_disconnect_pipe(pipe, rx_pipe_idx, tx_pipe_idx);
		if (rc) {
			IPA_ETH_ERR("pipe connect fails\n");
			ipa_assert();
		}
		if (pipe->dir == IPA_ETH_PIPE_DIR_TX)
			tx_pipe_idx++;
		if (pipe->dir == IPA_ETH_PIPE_DIR_RX)
			rx_pipe_idx++;
	}
	if (ipa_eth_pm_deregister(client)) {
		IPA_ETH_ERR("pm deregister failed\n");
		mutex_unlock(&ipa_eth_ctx->lock);
		return -EFAULT;
	}

	if (ipa3_ctx->eth_pdu_ctx.eth_pdu_mode_enabled)
		ipa3_update_eth_pdu_ep_index(rx_idx, tx_idx);

	mutex_unlock(&ipa_eth_ctx->lock);
	return 0;
}
EXPORT_SYMBOL(ipa_eth_client_disconn_pipes);

static void ipa_eth_msg_free_cb(void *buff, u32 len, u32 type)
{
	kfree(buff);
}

int ipa_eth_client_conn_evt(struct ipa_ecm_msg *msg)
{
	struct ipa_msg_meta msg_meta;
	struct ipa_ecm_msg *eth_msg;
	int ret;

	IPA_ETH_DBG("enter\n");

	eth_msg = kzalloc(sizeof(*eth_msg), GFP_KERNEL);
	if (eth_msg == NULL)
		return -ENOMEM;
	memcpy(eth_msg, msg, sizeof(struct ipa_ecm_msg));
	memset(&msg_meta, 0, sizeof(struct ipa_msg_meta));
	msg_meta.msg_len = sizeof(struct ipa_ecm_msg);
	msg_meta.msg_type = IPA_PERIPHERAL_CONNECT;

	IPA_ETH_DBG("send IPA_PERIPHERAL_CONNECT, len:%d, buff %pK", msg_meta.msg_len, eth_msg);
	ret = ipa_send_msg(&msg_meta, eth_msg, ipa_eth_msg_free_cb);

	IPA_ETH_DBG("exit\n");

	return ret;
}
EXPORT_SYMBOL(ipa_eth_client_conn_evt);

int ipa_eth_client_disconn_evt(struct ipa_ecm_msg *msg)
{
	struct ipa_msg_meta msg_meta;
	struct ipa_ecm_msg *eth_msg;
	int ret;

	IPA_ETH_DBG("enter\n");

	eth_msg = kzalloc(sizeof(*eth_msg), GFP_KERNEL);
	if (eth_msg == NULL)
		return -ENOMEM;
	memcpy(eth_msg, msg, sizeof(struct ipa_ecm_msg));
	memset(&msg_meta, 0, sizeof(struct ipa_msg_meta));
	msg_meta.msg_len = sizeof(struct ipa_ecm_msg);
	msg_meta.msg_type = IPA_PERIPHERAL_DISCONNECT;

	IPA_ETH_DBG("send PERIPHERAL_DISCONNECT, len:%d, buff %pK", msg_meta.msg_len, eth_msg);
	ret = ipa_send_msg(&msg_meta, eth_msg, ipa_eth_msg_free_cb);

	IPA_ETH_DBG("exit\n");

	return ret;
}
EXPORT_SYMBOL(ipa_eth_client_disconn_evt);

int ipa_eth_client_reg_intf(struct ipa_eth_intf_info *intf)
{
	struct ipa_eth_intf *new_intf = NULL;
	struct ipa_eth_intf *entry = NULL;
	struct ipa_ioc_add_hdr *hdr = NULL;
	struct ipa_tx_intf tx;
	struct ipa_rx_intf rx;
	enum ipa_client_type tx_client[IPA_ETH_MAX_TX_DMA_CHANNEL_QOS] = {0};
	enum ipa_client_type rx_client[IPA_ETH_MAX_RX_DMA_CHANNEL_QOS] = {0};
	struct ipa_ioc_tx_intf_prop *tx_prop =  NULL;
	struct ipa_ioc_rx_intf_prop *rx_prop = NULL;
	struct ipa_eth_client_pipe_info *pipe;
	u32 len = 0;
	int ret = 0, i = 0;
	int  rx_pipe_idx = 0, tx_pipe_idx = 0;
#if IPA_ETH_API_VER >= 2
	struct ipa_ecm_msg msg ;
	bool vlan_mode = false;
	bool ezmesh = false;
#if IPA_ETH_API_VER >= 3
	struct ipa_eth_hdr_info intf_hdr[IPA_IP_VLAN_MAX];
#else
	struct ipa_eth_hdr_info intf_hdr[IPA_IP_MAX];
#endif
	struct ethhdr l_ethhdr[IPA_IP_MAX] = { 0 };
	struct vlan_ethhdr l_vlan_ethhdr[IPA_IP_MAX] = { {0} };
	struct dbl_vlan_ethhdr l_dbl_vlan[IPA_IP_MAX] =  {0};
#endif
	int num_hdrs = 0;
	int traffic_type = 0;
#if IPA_ETH_API_VER >= 4
	u8 tx_tc[IPA_ETH_MAX_TX_DMA_CHANNEL_QOS] = { 0 };
	u8 rx_tc[IPA_ETH_MAX_RX_DMA_CHANNEL_QOS] = { 0 };
#endif

	if (intf == NULL) {
		IPA_ETH_ERR("invalid params intf=%pK\n", intf);
		return -EINVAL;
	}
	if (!ipa_eth_ctx) {
		IPA_ETH_ERR("disconn called before register readiness\n");
		return -EFAULT;
	}

#if IPA_ETH_API_VER >= 2
	if (!intf->client) {
		IPA_ETH_ERR("invalid intf->client\n");
		return -EFAULT;
	}
	if (!intf->client->net_dev) {
		IPA_ETH_ERR("invalid netdev\n");
		return -EFAULT;
	}
	if (!intf->net_dev)
		intf->net_dev = intf->client->net_dev;

	IPA_ETH_DBG("register interface for netdev %s\n", intf->net_dev->name);
	/* multiple attach support */
	if (strnstr(intf->net_dev->name, STR_ETH0_IFACE, strlen(intf->net_dev->name))) {
		ret = ipa_is_vlan_mode(IPA_VLAN_IF_ETH0, &vlan_mode);
		if (ret) {
			IPA_ETH_ERR("Could not determine IPA VLAN mode\n");
			return ret;
		}
	} else if (strnstr(intf->net_dev->name, STR_ETH1_IFACE, strlen(intf->net_dev->name))) {
		ret = ipa_is_vlan_mode(IPA_VLAN_IF_ETH1, &vlan_mode);
		if (ret) {
			IPA_ETH_ERR("Could not determine IPA VLAN mode\n");
			return ret;
		}
	} else {
		ret = ipa_is_vlan_mode(IPA_VLAN_IF_ETH, &vlan_mode);
		if (ret) {
			IPA_ETH_ERR("Could not determine IPA VLAN mode\n");
			return ret;
		}
	}
	/* Check if the interface is an ezmesh iface */
	if (strnstr(intf->net_dev->name, STR_ETH0_IFACE, strlen(intf->net_dev->name))) {
		ret = ipa3_is_spcl_iface(IPA_VLAN_IF_ETH0, &ezmesh);
		if (ret) {
			IPA_ETH_ERR("Could not determine IPA ezmesh mode\n");
			return ret;
		}
	}
#else
	IPA_ETH_DBG("register interface for netdev %s\n",
		intf->netdev_name);
#endif

	mutex_lock(&ipa_eth_ctx->lock);
	list_for_each_entry(entry, &ipa_eth_ctx->head_intf_list, link)
#if IPA_ETH_API_VER >= 2
		if (strcmp(entry->netdev_name, intf->net_dev->name) == 0) {
#else
		if (strcmp(entry->netdev_name, intf->netdev_name) == 0) {
#endif
			IPA_ETH_DBG("intf was added before.\n");
			mutex_unlock(&ipa_eth_ctx->lock);
			return 0;
		}
	memset(intf_hdr, 0, sizeof(intf_hdr));
	IPA_ETH_DBG("Ezmesh state %d \n", ezmesh);
	if (ezmesh) {
		struct ethhdr *eth_h;
		struct vlan_ethhdr *vlan_eth_h;

		intf_hdr[0].hdr = (u8 *)&l_ethhdr[0];
		eth_h = (struct ethhdr *) intf_hdr[0].hdr;
		memcpy(&eth_h->h_source, intf->net_dev->dev_addr, ETH_ALEN);
		eth_h->h_proto = htons(ETH_P_IP);
		intf_hdr[0].hdr_len = ETH_HLEN;
		intf_hdr[0].hdr_type = IPA_HDR_L2_ETHERNET_II;

		intf_hdr[1].hdr = (u8 *)&l_ethhdr[1];
		eth_h = (struct ethhdr *) intf_hdr[1].hdr;
		memcpy(&eth_h->h_source, intf->net_dev->dev_addr, ETH_ALEN);
		eth_h->h_proto = htons(ETH_P_IPV6);
		intf_hdr[1].hdr_len = ETH_HLEN;
		intf_hdr[1].hdr_type = IPA_HDR_L2_ETHERNET_II;

		intf_hdr[2].hdr = (u8 *)&l_vlan_ethhdr[0];
		vlan_eth_h = (struct vlan_ethhdr *) intf_hdr[2].hdr;
		memcpy(&vlan_eth_h->h_source, intf->net_dev->dev_addr, ETH_ALEN);
		vlan_eth_h->h_vlan_proto = htons(ETH_P_8021Q);
		vlan_eth_h->h_vlan_encapsulated_proto = htons(ETH_P_IP);
		intf_hdr[2].hdr_len = VLAN_ETH_HLEN;
		intf_hdr[2].hdr_type = IPA_HDR_L2_802_1Q;

		intf_hdr[3].hdr = (u8 *)&l_vlan_ethhdr[1];
		vlan_eth_h = (struct vlan_ethhdr *) intf_hdr[3].hdr;
		memcpy(&vlan_eth_h->h_source, intf->net_dev->dev_addr, ETH_ALEN);
		vlan_eth_h->h_vlan_proto = htons(ETH_P_8021Q);
		vlan_eth_h->h_vlan_encapsulated_proto = htons(ETH_P_IPV6);
		intf_hdr[3].hdr_len = VLAN_ETH_HLEN;
		intf_hdr[3].hdr_type = IPA_HDR_L2_802_1Q;
		IPA_ETH_DBG("Ezmesh state true configure 4 hdrs \n");
	}
#if IPA_ETH_API_VER >= 6
	else if((ipa3_ctx->device_mode == DEVMODE_APBRIDGE)
					&& (ipa3_ctx->device_vlan_mode))
	{
		struct dbl_vlan_ethhdr *dbl_vlan_ethhdr;
		intf_hdr[0].hdr = (u8 *)&l_dbl_vlan[0];
		dbl_vlan_ethhdr = (struct dbl_vlan_ethhdr *) intf_hdr[0].hdr;
		memcpy(&dbl_vlan_ethhdr->hdr.h_source, intf->net_dev->dev_addr, ETH_ALEN);
		dbl_vlan_ethhdr->hdr.h_vlan_proto = htons(ETH_P_8021Q);
		dbl_vlan_ethhdr->hdr.h_vlan_encapsulated_proto = htons(ETH_P_8021Q);
		dbl_vlan_ethhdr->outer_vlan_encap = htons(ETH_P_IP);
		intf_hdr[0].hdr_len = VLAN_VLAN_ETH_HLEN;
		intf_hdr[0].hdr_type = IPA_HDR_L2_802_Q_IN_Q;


		intf_hdr[1].hdr = (u8 *)&l_dbl_vlan[1];
		dbl_vlan_ethhdr = (struct dbl_vlan_ethhdr *) intf_hdr[1].hdr;
		memcpy(&dbl_vlan_ethhdr->hdr.h_source, intf->net_dev->dev_addr, ETH_ALEN);
		dbl_vlan_ethhdr->hdr.h_vlan_proto = htons(ETH_P_8021Q);
		dbl_vlan_ethhdr->hdr.h_vlan_encapsulated_proto = htons(ETH_P_8021Q);
		dbl_vlan_ethhdr->outer_vlan_encap = htons(ETH_P_IPV6);
		intf_hdr[1].hdr_len = VLAN_VLAN_ETH_HLEN;
		intf_hdr[1].hdr_type = IPA_HDR_L2_802_Q_IN_Q;
		IPADBG("hdr_len %d hdr type %d\n", intf_hdr[0].hdr_len, intf_hdr[0].hdr_type);
		IPADBG("hdr_len %d hdr type %d\n", intf_hdr[1].hdr_len, intf_hdr[1].hdr_type);

	}
#endif
#if IPA_ETH_API_VER >= 2
	else if (vlan_mode) {
		struct vlan_ethhdr *vlan_eth_h;

		intf_hdr[0].hdr = (u8 *)&l_vlan_ethhdr[0];
		vlan_eth_h = (struct vlan_ethhdr *) intf_hdr[0].hdr;
		memcpy(&vlan_eth_h->h_source, intf->net_dev->dev_addr, ETH_ALEN);
		vlan_eth_h->h_vlan_proto = htons(ETH_P_8021Q);
		vlan_eth_h->h_vlan_encapsulated_proto = htons(ETH_P_IP);
		intf_hdr[0].hdr_len = VLAN_ETH_HLEN;
		intf_hdr[0].hdr_type = IPA_HDR_L2_802_1Q;

		intf_hdr[1].hdr = (u8 *)&l_vlan_ethhdr[1];
		vlan_eth_h = (struct vlan_ethhdr *) intf_hdr[1].hdr;
		memcpy(&vlan_eth_h->h_source, intf->net_dev->dev_addr, ETH_ALEN);
		vlan_eth_h->h_vlan_proto = htons(ETH_P_8021Q);
		vlan_eth_h->h_vlan_encapsulated_proto = htons(ETH_P_IPV6);
		intf_hdr[1].hdr_len = VLAN_ETH_HLEN;
		intf_hdr[1].hdr_type = IPA_HDR_L2_802_1Q;;
	}
#endif
	else {
		struct ethhdr *eth_h;

		intf_hdr[0].hdr = (u8 *)&l_ethhdr[0];
		eth_h = (struct ethhdr *) intf_hdr[0].hdr;
		memcpy(&eth_h->h_source, intf->net_dev->dev_addr, ETH_ALEN);
		eth_h->h_proto = htons(ETH_P_IP);
		intf_hdr[0].hdr_len = ETH_HLEN;
		intf_hdr[0].hdr_type = IPA_HDR_L2_ETHERNET_II;

		intf_hdr[1].hdr = (u8 *)&l_ethhdr[1];
		eth_h = (struct ethhdr *) intf_hdr[1].hdr;
		memcpy(&eth_h->h_source, intf->net_dev->dev_addr, ETH_ALEN);
		eth_h->h_proto = htons(ETH_P_IPV6);
		intf_hdr[1].hdr_len = ETH_HLEN;
		intf_hdr[1].hdr_type = IPA_HDR_L2_ETHERNET_II;
	}

	new_intf = kzalloc(sizeof(*new_intf), GFP_KERNEL);
	if (new_intf == NULL) {
		IPA_ETH_ERR("fail to alloc new intf\n");
		mutex_unlock(&ipa_eth_ctx->lock);
		return -ENOMEM;
	}
	INIT_LIST_HEAD(&new_intf->link);
#if IPA_ETH_API_VER >= 2
	strlcpy(new_intf->netdev_name, intf->net_dev->name, sizeof(new_intf->netdev_name));
#else
	strlcpy(new_intf->netdev_name, intf->netdev_name,
			sizeof(new_intf->netdev_name));
#endif

	num_hdrs = ezmesh ? 4 : 2;
	for (i = 0; i < num_hdrs; i++)
	{
		new_intf->hdr_len[i] = intf_hdr[i].hdr_len;
	}

	/* add partial header */
	len = sizeof(struct ipa_ioc_add_hdr) + num_hdrs * sizeof(struct ipa_hdr_add);
	hdr = kzalloc(len, GFP_KERNEL);
	if (hdr == NULL) {
		IPA_ETH_ERR("fail to alloc %d bytes\n", len);
		ret = -EFAULT;
		goto fail_alloc_hdr;
	}
#if IPA_ETH_API_VER >= 2
	if (ipa_eth_commit_partial_hdr(hdr, intf->net_dev->name, (struct ipa_eth_hdr_info *)intf_hdr, num_hdrs)) {
#else
	if (ipa_eth_commit_partial_hdr(hdr,
		intf->netdev_name, intf->hdr, num_hdrs)) {
#endif
		IPA_ETH_ERR("fail to commit partial headers\n");
		ret = -EFAULT;
		goto fail_commit_hdr;
	}

	new_intf->partial_hdr_hdl[IPA_IP_v4] = hdr->hdr[IPA_IP_v4].hdr_hdl;
	new_intf->partial_hdr_hdl[IPA_IP_v6] = hdr->hdr[IPA_IP_v6].hdr_hdl;
	IPA_ETH_DBG("IPv4 hdr hdl: %d IPv6 hdr hdl: %d\n",
		hdr->hdr[IPA_IP_v4].hdr_hdl, hdr->hdr[IPA_IP_v6].hdr_hdl);
#if IPA_ETH_API_VER >= 3
	if (hdr->num_hdrs > 2 && num_hdrs > 2) {
		new_intf->partial_hdr_hdl[IPA_IP_v4_VLAN] = hdr->hdr[IPA_IP_v4_VLAN].hdr_hdl;
		new_intf->partial_hdr_hdl[IPA_IP_v6_VLAN] = hdr->hdr[IPA_IP_v6_VLAN].hdr_hdl;
		IPA_ETH_DBG("IPv4 vlan hdr hdl: %d IPv6 vlan hdr hdl: %d\n",
			hdr->hdr[IPA_IP_v4_VLAN].hdr_hdl, hdr->hdr[IPA_IP_v6_VLAN].hdr_hdl);
	} else {
		new_intf->partial_hdr_hdl[IPA_IP_v4_VLAN] = -1;
		new_intf->partial_hdr_hdl[IPA_IP_v6_VLAN] = -1;
		IPA_ETH_DBG("IPv4 vlan hdr hdl: -1 IPv6 vlan hdr hdl: -1\n");
	}
#endif

	memset(&tx, 0, sizeof(struct ipa_tx_intf));
	memset(&rx, 0, sizeof(struct ipa_rx_intf));
#if IPA_ETH_API_VER >= 2
	list_for_each_entry(pipe, &intf->client->pipe_list, link) {
#else
	for (i = 0; i < intf->pipe_hdl_list_size; i++) {
		pipe = ipa_eth_get_pipe_from_hdl(intf->pipe_hdl_list[i]);
#endif

#if IPA_ETH_API_VER >= 3
		traffic_type = pipe->traffic_type;
#else
		traffic_type = intf->client->traffic_type;
#endif
		if (ezmesh) {
			if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
				if (traffic_type == IPA_ETH_PIPE_BEST_EFFORT_VLAN) {
					tx_client[1] =
						ipa_eth_get_ipa_client_type_from_pipe(pipe,
						rx_pipe_idx, tx_pipe_idx);
				}
				else {
					tx_client[0] =
						ipa_eth_get_ipa_client_type_from_pipe(pipe,
						rx_pipe_idx, tx_pipe_idx);
				}
				tx.num_props++;
			} else {
				if (traffic_type == IPA_ETH_PIPE_BEST_EFFORT_VLAN) {
					rx_client[1] =
						ipa_eth_get_ipa_client_type_from_pipe(pipe,
						rx_pipe_idx, tx_pipe_idx);
				}
				else {
					rx_client[0] =
						ipa_eth_get_ipa_client_type_from_pipe(pipe,
						rx_pipe_idx, tx_pipe_idx);
				}
				rx.num_props++;
			}
		}
		else {
			if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
				tx_client[tx.num_props] =
				ipa_eth_get_ipa_client_type_from_pipe(pipe, rx_pipe_idx, tx_pipe_idx);
#if IPA_ETH_API_VER >= 4
				tx_tc[tx.num_props] = pipe->tc_bmap;
#endif
				tx.num_props++;
			} else {
				rx_client[rx.num_props] =
				ipa_eth_get_ipa_client_type_from_pipe(pipe, rx_pipe_idx, tx_pipe_idx);
#if IPA_ETH_API_VER >= 4
				rx_tc[rx.num_props] = pipe->tc_bmap;
#endif
				rx.num_props++;
			}

			if (pipe->dir == IPA_ETH_PIPE_DIR_TX)
				tx_pipe_idx++;
			if (pipe->dir == IPA_ETH_PIPE_DIR_RX)
				rx_pipe_idx++;
		}
	}

	/* populate tx prop */
	if (tx.num_props) {
		tx_prop = kmalloc(
			sizeof(*tx_prop) * tx.num_props *
			IPA_IP_MAX, GFP_KERNEL);
		if (!tx_prop) {
			IPAERR("failed to allocate memory\n");
			ret = -ENOMEM;
			goto fail_commit_hdr;
		}
		memset(tx_prop, 0, sizeof(*tx_prop) *
			tx.num_props * IPA_IP_MAX);
		tx.prop = tx_prop;
		for (i = 0; i < tx.num_props; i++) {
			tx_prop[2*i].ip = IPA_IP_v4;
			tx_prop[2 * i].dst_pipe = tx_client[i];
#if IPA_ETH_API_VER >= 4
			tx_prop[2 * i].tc_bmap = tx_tc[i];
#endif
#if IPA_ETH_API_VER >= 2
			tx_prop[2*i].hdr_l2_type = intf_hdr[(2*i)%num_hdrs].hdr_type;
#else
			tx_prop[2*i].hdr_l2_type = intf->hdr[2*i].hdr_type;
#endif
			strlcpy(tx_prop[2*i].hdr_name, hdr->hdr[(2*i)%num_hdrs].name,
				sizeof(tx_prop[i].hdr_name));

			tx_prop[2*i+1].ip = IPA_IP_v6;
			tx_prop[2 * i + 1].dst_pipe = tx_client[i];	
#if IPA_ETH_API_VER >= 4
			tx_prop[2 * i+1].tc_bmap = tx_tc[i];
#endif
#if IPA_ETH_API_VER >= 2
			tx_prop[2*i+1].hdr_l2_type = intf_hdr[(2*i+1)%num_hdrs].hdr_type;
#else
			tx_prop[2*i+1].hdr_l2_type = intf->hdr[1].hdr_type;
#endif
			strlcpy(tx_prop[2*i+1].hdr_name, hdr->hdr[(2*i+1)%num_hdrs].name,
				sizeof(tx_prop[2*i+1].hdr_name));

			IPA_ETH_DBG("tx_prop i %d dst pipe v4:%d v6:%d\n",i, tx_prop[2*i].dst_pipe, tx_prop[2*i + 1].dst_pipe);
			IPA_ETH_DBG("tx_prop i %d l2_hdr v4:%d v6:%d\n",i, tx_prop[2*i].hdr_l2_type, tx_prop[2*i + 1].hdr_l2_type);
		}
	}
	/* populate rx prop */
	if (rx.num_props) {
		rx_prop = kmalloc(
			sizeof(*rx_prop) * rx.num_props *
			IPA_IP_MAX, GFP_KERNEL);
		if (!rx_prop) {
			IPAERR("failed to allocate memory\n");
			ret = -ENOMEM;
			goto fail_commit_hdr;
		}
		memset(rx_prop, 0, sizeof(*rx_prop) *
			rx.num_props * IPA_IP_MAX);
		rx.prop = rx_prop;
		for (i = 0; i < rx.num_props; i++) {
			rx_prop[2*i].ip = IPA_IP_v4;
			rx_prop[2 * i].src_pipe = rx_client[i];
#if IPA_ETH_API_VER >= 4
			rx_prop[2 * i].tc_bmap = rx_tc[i];
#endif
#if IPA_ETH_API_VER >= 2
			rx_prop[2*i].hdr_l2_type = intf_hdr[(2*i)%num_hdrs].hdr_type;
#else
			rx_prop[2*i].hdr_l2_type = intf->hdr[2*i].hdr_type;
#endif

			rx_prop[2*i+1].ip = IPA_IP_v6;
			rx_prop[2 * i + 1].src_pipe = rx_client[i];
#if IPA_ETH_API_VER >= 4
			rx_prop[2 * i + 1].tc_bmap = rx_tc[i];
#endif
#if IPA_ETH_API_VER >= 2
			rx_prop[2*i+1].hdr_l2_type = intf_hdr[(2*i+1)%num_hdrs].hdr_type;
#else
			rx_prop[2*i+1].hdr_l2_type = intf->hdr[2*i+1].hdr_type;
#endif

			IPA_ETH_DBG("rx_prop i %d src pipe v4:%d v6:%d\n",i, rx_prop[2*i].src_pipe, rx_prop[2*i + 1].src_pipe);
			IPA_ETH_DBG("rx_prop i %d l2_hdr v4:%d v6:%d\n",i, rx_prop[2*i].hdr_l2_type, rx_prop[2*i + 1].hdr_l2_type);

		}

		tx.num_props *= IPA_IP_MAX;
		rx.num_props *= IPA_IP_MAX;
	}
	IPA_ETH_DBG("num tx %d, rx %d \n", tx.num_props, rx.num_props);
#if IPA_ETH_API_VER >= 2
	if (ipa_register_intf(intf->net_dev->name, &tx, &rx)) {
#else
	if (ipa_register_intf(intf->netdev_name, &tx, &rx)) {
#endif
		IPA_ETH_ERR("fail to add interface prop\n");
		ret = -EFAULT;
		goto fail_commit_hdr;
	}

	list_add(&new_intf->link, &ipa_eth_ctx->head_intf_list);

	kfree(hdr);
	kfree(tx_prop);
	kfree(rx_prop);
	mutex_unlock(&ipa_eth_ctx->lock);

#if IPA_ETH_API_VER >= 2
	if (intf->is_conn_evt) {
		strlcpy(msg.name, intf->net_dev->name, sizeof(msg.name));
		msg.ifindex = intf->net_dev->ifindex;
		ipa_eth_client_conn_evt(&msg);
	}
#endif
	return 0;
fail_commit_hdr:
	kfree(hdr);
	kfree(tx_prop);
	kfree(rx_prop);
fail_alloc_hdr:
	kfree(new_intf);
	mutex_unlock(&ipa_eth_ctx->lock);
	return ret;
}
EXPORT_SYMBOL(ipa_eth_client_reg_intf);

int ipa_eth_client_unreg_intf(struct ipa_eth_intf_info *intf)
{
	int len, ret = 0;
	struct ipa_ioc_del_hdr *hdr = NULL;
	struct ipa_eth_intf *entry;
	struct ipa_eth_intf *next;
#if IPA_ETH_API_VER >= 2
	struct ipa_ecm_msg msg;
#endif

	if (intf == NULL) {
		IPA_ETH_ERR("invalid params intf=%pK\n", intf);
		return -EINVAL;
	}
	if (!ipa_eth_ctx) {
		IPA_ETH_ERR("disconn called before register readiness\n");
		return -EFAULT;
	}
#if IPA_ETH_API_VER >= 2
	if (!intf->net_dev) {
		IPA_ETH_ERR("invalid netdev\n");
		return -EFAULT;
	}
	IPA_ETH_DBG("unregister interface for netdev %s\n", intf->net_dev->name);
#else
	IPA_ETH_DBG("unregister interface for netdev %s\n",
		intf->netdev_name);
#endif
	mutex_lock(&ipa_eth_ctx->lock);
	list_for_each_entry_safe(entry, next, &ipa_eth_ctx->head_intf_list,
		link)
#if IPA_ETH_API_VER >= 2
		if (strcmp(entry->netdev_name, intf->net_dev->name) == 0) {
#else
		if (strcmp(entry->netdev_name, intf->netdev_name) == 0) {
#endif
			len = sizeof(struct ipa_ioc_del_hdr) +
				IPA_IP_MAX * sizeof(struct ipa_hdr_del);
			hdr = kzalloc(len, GFP_KERNEL);
			if (hdr == NULL) {
				IPA_ETH_ERR("fail to alloc %d bytes\n", len);
				mutex_unlock(&ipa_eth_ctx->lock);
				return -ENOMEM;
			}

			hdr->commit = 1;
			hdr->num_hdls = 2;
			hdr->hdl[0].hdl = entry->partial_hdr_hdl[0];
			hdr->hdl[1].hdl = entry->partial_hdr_hdl[1];
			IPA_ETH_DBG("IPv4 hdr hdl: %d IPv6 hdr hdl: %d\n",
				hdr->hdl[0].hdl, hdr->hdl[1].hdl);

			if (ipa_del_hdr(hdr)) {
				IPA_ETH_ERR("fail to delete partial header\n");
				ret = -EFAULT;
				goto fail;
			}

			if (ipa_deregister_intf(entry->netdev_name)) {
				IPA_ETH_ERR("fail to del interface props\n");
				ret = -EFAULT;
				goto fail;
			}

			list_del(&entry->link);
			kfree(entry);

			break;
		}
fail:
	kfree(hdr);
	mutex_unlock(&ipa_eth_ctx->lock);
#if IPA_ETH_API_VER >= 2
	if (intf->is_conn_evt) {
		strlcpy(msg.name, intf->net_dev->name, sizeof(msg.name));
		msg.ifindex = intf->net_dev->ifindex;
		ipa_eth_client_disconn_evt(&msg);
	}
#endif
	return ret;

}
EXPORT_SYMBOL(ipa_eth_client_unreg_intf);

int ipa_eth_client_set_perf_profile(struct ipa_eth_client *client,
	struct ipa_eth_perf_profile *profile)
{
	int client_type, inst_id;

	if ((!profile) || (!client) || (client->client_type >= IPA_ETH_CLIENT_MAX)) {
		IPA_ETH_ERR("Invalid input\n");
		return -EINVAL;
	}

	client_type = client->client_type;
	inst_id = client->inst_id;

	if (ipa_pm_set_throughput(
		ipa_eth_ctx->client[client_type][inst_id].pm_hdl,
		profile->max_supported_bw_mbps)) {
		IPA_ETH_ERR("fail to set pm throughput\n");
		return -EFAULT;
	}

	return 0;
}
EXPORT_SYMBOL(ipa_eth_client_set_perf_profile);

enum ipa_client_type ipa_eth_get_ipa_client_type_from_eth_type(
	enum ipa_eth_client_type eth_client_type, enum ipa_eth_pipe_direction dir)
{
	int ipa_client_type = IPA_CLIENT_MAX;

	switch (eth_client_type) {
	case IPA_ETH_CLIENT_AQC107:
	case IPA_ETH_CLIENT_AQC113:
		if (dir == IPA_ETH_PIPE_DIR_TX) {
			ipa_client_type =
				IPA_CLIENT_AQC_ETHERNET_CONS;
		} else {
			ipa_client_type =
				IPA_CLIENT_AQC_ETHERNET_PROD;
		}
		break;
	case IPA_ETH_CLIENT_RTK8111K:
	case IPA_ETH_CLIENT_RTK8125B:
			if (dir == IPA_ETH_PIPE_DIR_TX) {
				ipa_client_type =
					IPA_CLIENT_RTK_ETHERNET_CONS;
			} else {
				ipa_client_type =
					IPA_CLIENT_RTK_ETHERNET_PROD;
			}
		break;
	case IPA_ETH_CLIENT_NTN:
	case IPA_ETH_CLIENT_EMAC:
#if IPA_ETH_API_VER >= 2
	case IPA_ETH_CLIENT_NTN3:
	case IPA_ETH_CLIENT_IEMAC:
#endif
			if (dir == IPA_ETH_PIPE_DIR_TX) {
				ipa_client_type =
					IPA_CLIENT_ETHERNET_CONS;
			} else {
				ipa_client_type =
					IPA_CLIENT_ETHERNET_PROD;
			}
		break;
	default:
		IPA_ETH_ERR("invalid client type%d\n",
			eth_client_type);
	}
	return ipa_client_type;
}
EXPORT_SYMBOL(ipa_eth_get_ipa_client_type_from_eth_type);

bool ipa_eth_client_exist(enum ipa_eth_client_type eth_client_type, int inst_id)
{
	if (ipa_eth_ctx)
		return ipa_eth_ctx->client[eth_client_type][inst_id].existed;
	else return false;
}
EXPORT_SYMBOL(ipa_eth_client_exist);

int ipa_eth_get_config_type(
	enum ipa_eth_client_type client_type, int inst_id, struct ipa_eth_config *eth_config)
{
	int ret = 0;
	bool ezmesh = false;

	if (!eth_config) {
		IPA_ETH_ERR("Invalid parameter\n");
		return -EFAULT;
	}

	IPA_ETH_DBG("Config type requested for client %d, inst_id %d\n", client_type, inst_id);

	/* Check if the interface is an ezmesh iface */
#if IPA_ETH_API_VER >= 3
	if ((IPA_ETH_CLIENT_IEMAC == client_type) && (0 == inst_id) &&
		!ipa3_ctx->tsn_iface) {
		ret = ipa3_is_spcl_iface(IPA_VLAN_IF_ETH0, &ezmesh);
		if (ret) {
			IPA_ETH_ERR("Could not determine special iface\n");
			return ret;
		}
	}
#endif

	if (ezmesh) {
		snprintf(eth_config->config, sizeof(eth_config->config) ,"ezmesh");
		eth_config->num_dma_channel = DMA_NUM_CHANNEL_EZMESH;

		eth_config->dma_config[0].dir = IPA_ETH_PIPE_DIR_TX;
		eth_config->dma_config[0].traffic_type = IPA_ETH_PIPE_BEST_EFFORT;

		eth_config->dma_config[1].dir = IPA_ETH_PIPE_DIR_RX;
		eth_config->dma_config[1].traffic_type = IPA_ETH_PIPE_BEST_EFFORT;

		eth_config->dma_config[2].dir = IPA_ETH_PIPE_DIR_TX;
		eth_config->dma_config[2].traffic_type = IPA_ETH_PIPE_BEST_EFFORT_VLAN;

		eth_config->dma_config[3].dir = IPA_ETH_PIPE_DIR_RX;
		eth_config->dma_config[3].traffic_type = IPA_ETH_PIPE_BEST_EFFORT_VLAN;

		IPA_ETH_DBG("Ezmesh configuration for client %d, inst_id %d\n", client_type, inst_id);
	} else if (ipa3_ctx->tsn_iface) {
		snprintf(eth_config->config, sizeof(eth_config->config), "tsn");
		eth_config->num_dma_channel = DMA_NUM_CHANNEL_TSN;

		ipa3_notify_uc_tsn_enable();

		eth_config->dma_config[0].dir = IPA_ETH_PIPE_DIR_TX;
		eth_config->dma_config[0].traffic_type =
			IPA_ETH_PIPE_BEST_EFFORT;

		eth_config->dma_config[1].dir = IPA_ETH_PIPE_DIR_RX;
		eth_config->dma_config[1].traffic_type =
			IPA_ETH_PIPE_BEST_EFFORT;

		eth_config->dma_config[2].dir = IPA_ETH_PIPE_DIR_TX;
		eth_config->dma_config[2].traffic_type =
			IPA_ETH_PIPE_LOW_LATENCY;

		IPA_ETH_DBG("TSN configuration for client %d, inst_id %d\n",
			    client_type, inst_id);
	}
#if IPA_ETH_API_VER >= 4
	else if (ipa3_ctx->eth_qos && inst_id == 0) {
		int i = 0;
		snprintf(eth_config->config, sizeof(eth_config->config), "qos");

		if (ipa3_ctx->ipa_config_is_auto)
			eth_config->num_dma_channel = IPA_ETH_MAX_DMA_CHANNEL_QOS_AUTO;
		else
			eth_config->num_dma_channel = IPA_ETH_MAX_DMA_CHANNEL_QOS_CPE;

		for (i = 0; i < eth_config->num_dma_channel; i++) {
			eth_config->dma_config[i].dir = (i % 2) ? IPA_ETH_PIPE_DIR_RX :
				IPA_ETH_PIPE_DIR_TX;
			eth_config->dma_config[i].traffic_type =
				IPA_ETH_PIPE_TRAFFIC_TYPE_QOS;
		}

		IPA_ETH_DBG("QOS configuration for client %d, inst_id %d\n",
			client_type, inst_id);
		IPA_ETH_DBG("Max num DMA channels: %d\n",
			eth_config->num_dma_channel);
	}
#endif
#if IPA_ETH_API_VER >= 6
	else if (ipa3_ctx->device_mode == DEVMODE_APBRIDGE)
	{
		snprintf(eth_config->config, sizeof(eth_config->config), "dbl_vlan");
		eth_config->num_dma_channel = DMA_NUM_CHANNEL_DEFAULT;

		eth_config->dma_config[0].dir = IPA_ETH_PIPE_DIR_TX;
		eth_config->dma_config[0].traffic_type = IPA_ETH_PIPE_TRAFFIC_TYPE_DOUBLE_TAG;

		eth_config->dma_config[1].dir = IPA_ETH_PIPE_DIR_RX;
		eth_config->dma_config[1].traffic_type = IPA_ETH_PIPE_TRAFFIC_TYPE_DOUBLE_TAG;
		IPA_ETH_DBG("double vlan configuration for client %d, inst_id %d\n", client_type, inst_id);

	}
#endif
	else {
		snprintf(eth_config->config, sizeof(eth_config->config), "default");
		eth_config->num_dma_channel = DMA_NUM_CHANNEL_DEFAULT;

		eth_config->dma_config[0].dir = IPA_ETH_PIPE_DIR_TX;
		eth_config->dma_config[0].traffic_type = IPA_ETH_PIPE_BEST_EFFORT;

		eth_config->dma_config[1].dir = IPA_ETH_PIPE_DIR_RX;
		eth_config->dma_config[1].traffic_type = IPA_ETH_PIPE_BEST_EFFORT;
		IPA_ETH_DBG("Default configuration for client %d, inst_id %d\n", client_type, inst_id);
	}
	return ret;
}
EXPORT_SYMBOL(ipa_eth_get_config_type);


#if IPA_ETH_API_VER > 4
int ipa_eth_client_enable_pipes(struct ipa_eth_client *client)
{
	struct ipa_eth_client_pipe_info *pipe;
	int rc;
	int client_type, inst_id, traff_type;
	u8 rx_pipe_idx = 0, tx_pipe_idx = 0, pipe_idx = 0;
	int priority = 0;

	/* validate user input */
	if (!client || (client->client_type >= IPA_ETH_CLIENT_MAX)) {
		IPA_ETH_ERR("null client or client type not defined");
		return -EFAULT;
	}
	if (!ipa_eth_ctx) {
		IPA_ETH_ERR("client enable called before register readiness\n");
		return -EFAULT;
	}

	if (ipa3_ctx->ipa_tiering_value & IPA_TIERING_DISABLE_ETH) {
			IPA_ETH_ERR("ETH offload is disabled by IPA Tiering, client %d\n",
				client->client_type);
			return -EFAULT;
	}

	if (!ipa_eth_ctx->is_eth_ready) {
		IPA_ETH_ERR("client enable called before IPA eth ready\n");
		return -EFAULT;
	}
	ipa_eth_ctx->client_priv = client->priv;
	client_type = client->client_type;
	inst_id = client->inst_id;

#if IPA_ETH_API_VER >= 3
	IPA_ETH_DBG("ipa_eth conn client %d inst %d\n", client_type, inst_id);
#else
	traff_type = client->traffic_type;
	IPA_ETH_DBG("ipa_eth conn client %d inst %d, traffic %d\n",
			client_type, inst_id, traff_type);
#endif

	mutex_lock(&ipa_eth_ctx->lock);

	rx_pipe_idx = 0;
	tx_pipe_idx = 0;
	list_for_each_entry(pipe, &client->pipe_list,
		link) {
#if IPA_ETH_API_VER >= 3
		IPA_ETH_DBG("Eth connect pipe %p traffic_type %d dir %d\n",
				pipe, pipe->traffic_type, pipe->dir);
		if (pipe->traffic_type == IPA_ETH_PIPE_BEST_EFFORT_VLAN &&
			pipe->dir == IPA_ETH_PIPE_DIR_TX) {
			IPA_ETH_DBG("traffic_type %d dir %d continue...\n",
				pipe->traffic_type, pipe->dir);
			tx_pipe_idx++;
			continue;
		}
#endif
		if (pipe->dir == IPA_ETH_PIPE_DIR_TX) {
			priority = eth_qos_get_tx_priority(tx_pipe_idx, inst_id);
			pipe_idx = tx_pipe_idx;
		} else {
			priority = eth_qos_get_rx_priority(rx_pipe_idx, inst_id);
			pipe_idx = rx_pipe_idx;
		}

		rc = ipa_eth_client_enable_pipe(pipe, inst_id, priority, pipe_idx);
		if (rc) {
			IPA_ETH_ERR("pipe enable fails\n");
			ipa_assert();
		}

#if IPA_ETH_API_VER >= 3
		traff_type = pipe->traffic_type;
		IPA_ETH_DBG("ipa_eth conn traffic %d\n", traff_type);
#endif
		if (pipe->dir == IPA_ETH_PIPE_DIR_TX)
			tx_pipe_idx++;
		if (pipe->dir == IPA_ETH_PIPE_DIR_RX)
			rx_pipe_idx++;
	}

	if (!ipa_eth_ctx->client[client_type][inst_id].existed) {
#ifdef CONFIG_DEBUG_FS
		ipa3_eth_debugfs_add_node(client);
#endif
		ipa_eth_ctx->client[client_type][inst_id].existed = true;
	}

	mutex_unlock(&ipa_eth_ctx->lock);
	return 0;
}
EXPORT_SYMBOL(ipa_eth_client_enable_pipes);

int ipa_eth_client_disable_pipes(struct ipa_eth_client *client)
{
	int rc;
	struct ipa_eth_client_pipe_info *pipe;
	int  rx_pipe_idx = 0, tx_pipe_idx = 0;
	int  inst_id;

	/* validate user input */
	if (!client) {
		IPA_ETH_ERR("null client");
		return -EFAULT;
	}

	if (!ipa_eth_ctx) {
		IPA_ETH_ERR("disconn called before register readiness\n");
		return -EFAULT;
	}

	if (!ipa_eth_ctx->is_eth_ready) {
		IPA_ETH_ERR("disconn called before IPA eth ready\n");
		return -EFAULT;
	}
	inst_id = client->inst_id;
#if IPA_ETH_API_VER >= 3
	IPA_ETH_DBG("ipa_eth disable client %d inst %d\n",
		client->client_type, client->inst_id);
#else
	IPA_ETH_DBG("ipa_eth disable client %d inst %d, traffic %d\n",
		client->client_type, client->inst_id,
		client->traffic_type);
#endif

	mutex_lock(&ipa_eth_ctx->lock);

	rx_pipe_idx = 0;
	tx_pipe_idx = 0;
	list_for_each_entry(pipe, &client->pipe_list,
		link) {
#if IPA_ETH_API_VER >= 3
		if (pipe->traffic_type == IPA_ETH_PIPE_BEST_EFFORT_VLAN
			&& pipe->dir == IPA_ETH_PIPE_DIR_TX) {
			IPA_ETH_DBG("traffic_type %d dir %d continue...\n",
				pipe->traffic_type, pipe->dir);
			tx_pipe_idx++;
			continue;
		}
#endif
		rc = ipa_eth_client_disable_pipe(pipe, inst_id, rx_pipe_idx, tx_pipe_idx);
		if (rc) {
			IPA_ETH_ERR("pipe disable fails\n");
			ipa_assert();
		}
		if (pipe->dir == IPA_ETH_PIPE_DIR_TX)
			tx_pipe_idx++;
		if (pipe->dir == IPA_ETH_PIPE_DIR_RX)
			rx_pipe_idx++;
	}

	mutex_unlock(&ipa_eth_ctx->lock);
	return 0;
}
EXPORT_SYMBOL(ipa_eth_client_disable_pipes);
#endif

