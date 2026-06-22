// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2013-2019, The Linux Foundation. All rights reserved.
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include "ipa_i.h"
#include <linux/msm_ipa.h>

struct ipa3_intf {
	char name[IPA_RESOURCE_NAME_MAX];
	struct list_head link;
	u32 num_tx_props;
	u32 num_rx_props;
	u32 num_ext_props;
	struct ipa_ioc_tx_intf_prop *tx;
	struct ipa_ioc_rx_intf_prop *rx;
	struct ipa_ioc_ext_intf_prop *ext;
	enum ipa_client_type excp_pipe;
	struct ipa3_flt_entry *flt_list;
	int intf_idx;
	u16 vpnum;
	bool vpnum_valid;
};

struct ipa3_push_msg {
	struct ipa_msg_meta meta;
	ipa_msg_free_fn callback;
	void *buff;
	struct list_head link;
};

struct ipa3_pull_msg {
	struct ipa_msg_meta meta;
	ipa_msg_pull_fn callback;
	struct list_head link;
};

/**
 * ipa_register_intf() - register "logical" interface
 * @name: [in] interface name
 * @tx:	[in] TX properties of the interface
 * @rx:	[in] RX properties of the interface
 *
 * Register an interface and its tx and rx properties, this allows
 * configuration of rules from user-space
 *
 * Returns:	0 on success, negative on failure
 *
 * Note:	Should not be called from atomic context
 */
int ipa_register_intf(const char *name, const struct ipa_tx_intf *tx,
		       const struct ipa_rx_intf *rx, int intf_idx)
{
	return ipa3_register_intf_ext(name, tx, rx, NULL, intf_idx);
}
EXPORT_SYMBOL(ipa_register_intf);

/**
 * ipa3_register_intf_ext() - register "logical" interface which has only
 * extended properties
 * @name: [in] interface name
 * @tx:	[in] TX properties of the interface
 * @rx:	[in] RX properties of the interface
 * @ext: [in] EXT properties of the interface
 * @intf_idx: [in] interface index
 *
 * Register an interface and its tx, rx and ext properties, this allows
 * configuration of rules from user-space
 *
 * Returns:	0 on success, negative on failure
 *
 * Note:	Should not be called from atomic context
 */
int ipa3_register_intf_ext(const char *name, const struct ipa_tx_intf *tx,
		       const struct ipa_rx_intf *rx,
		       const struct ipa_ext_intf *ext,
			   int intf_idx)
{
	struct ipa3_intf *intf;
	u32 len;

	IPAERR_RL("Interface idx passed %d for %s\n", intf_idx, name);

	if (name == NULL || (tx == NULL && rx == NULL && ext == NULL)) {
		IPAERR_RL("invalid params name=%pK tx=%pK rx=%pK ext=%pK\n",
				name, tx, rx, ext);
		return -EINVAL;
	}

	if (tx && tx->num_props > IPA_NUM_PROPS_MAX) {
		IPAERR_RL("invalid tx num_props=%d max=%d\n", tx->num_props,
				IPA_NUM_PROPS_MAX);
		return -EINVAL;
	}

	if (rx && rx->num_props > IPA_NUM_PROPS_MAX) {
		IPAERR_RL("invalid rx num_props=%d max=%d\n", rx->num_props,
				IPA_NUM_PROPS_MAX);
		return -EINVAL;
	}

	if (ext && ext->num_props > IPA_NUM_PROPS_MAX) {
		IPAERR_RL("invalid ext num_props=%d max=%d\n", ext->num_props,
				IPA_NUM_PROPS_MAX);
		return -EINVAL;
	}

	len = sizeof(struct ipa3_intf);
	intf = kzalloc(len, GFP_KERNEL);
	if (intf == NULL)
		return -ENOMEM;

	strscpy(intf->name, name, IPA_RESOURCE_NAME_MAX);

	if (tx) {
		intf->num_tx_props = tx->num_props;
		len = tx->num_props * sizeof(struct ipa_ioc_tx_intf_prop);
		intf->tx = kmemdup(tx->prop, len, GFP_KERNEL);
		if (intf->tx == NULL) {
			kfree(intf);
			return -ENOMEM;
		}
	}

	if (rx) {
		intf->num_rx_props = rx->num_props;
		len = rx->num_props * sizeof(struct ipa_ioc_rx_intf_prop);
		intf->rx = kmemdup(rx->prop, len, GFP_KERNEL);
		if (intf->rx == NULL) {
			kfree(intf->tx);
			kfree(intf);
			return -ENOMEM;
		}
		memcpy(intf->rx, rx->prop, len);
	}

	if (ext) {
		intf->num_ext_props = ext->num_props;
		len = ext->num_props * sizeof(struct ipa_ioc_ext_intf_prop);
		intf->ext = kmemdup(ext->prop, len, GFP_KERNEL);
		if (intf->ext == NULL) {
			kfree(intf->rx);
			kfree(intf->tx);
			kfree(intf);
			return -ENOMEM;
		}
		memcpy(intf->ext, ext->prop, len);
	}

	if (ext && ext->excp_pipe_valid)
		intf->excp_pipe = ext->excp_pipe;
	else
		intf->excp_pipe = IPA_CLIENT_APPS_LAN_CONS;

	intf->intf_idx = intf_idx;
	if (ext && ext->vpnum_valid) {
		intf->vpnum = ext->vpnum;
		intf->vpnum_valid = true;
	} else {
		intf->vpnum = 0;
		intf->vpnum_valid = false;
	}

	mutex_lock(&ipa3_ctx->lock);
	list_add_tail(&intf->link, &ipa3_ctx->intf_list);
	mutex_unlock(&ipa3_ctx->lock);

	return 0;
}

/**
 * ipa_deregister_intf() - de-register previously registered logical interface
 * @name: [in] interface name
 *
 * De-register a previously registered interface
 *
 * Returns:	0 on success, negative on failure
 *
 * Note:	Should not be called from atomic context
 */
int ipa_deregister_intf(const char *name)
{
	struct ipa3_intf *entry;
	struct ipa3_intf *next;
	int result = -EINVAL;

	if ((name == NULL) ||
	    (strnlen(name, IPA_RESOURCE_NAME_MAX) == IPA_RESOURCE_NAME_MAX)) {
		IPAERR_RL("invalid param name=%s\n", name);
		return result;
	}

	mutex_lock(&ipa3_ctx->lock);
	list_for_each_entry_safe(entry, next, &ipa3_ctx->intf_list, link) {
		if (!strcmp(entry->name, name)) {
			list_del(&entry->link);
			kfree(entry->ext);
			kfree(entry->rx);
			kfree(entry->tx);
			kfree(entry);
			result = 0;
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);

	return result;
}
EXPORT_SYMBOL(ipa_deregister_intf);

/**
 * ipa3_query_intf() - query logical interface properties
 * @lookup:	[inout] interface name and number of properties
 *
 * Obtain the handle and number of tx and rx properties for the named
 * interface, used as part of querying the tx and rx properties for
 * configuration of various rules from user-space
 *
 * Returns:	0 on success, negative on failure
 *
 * Note:	Should not be called from atomic context
 */
int ipa3_query_intf(struct ipa_ioc_query_intf *lookup)
{
	struct ipa3_intf *entry;
	int result = -EINVAL;

	if (lookup == NULL) {
		IPAERR_RL("invalid param lookup=%pK\n", lookup);
		return result;
	}

	lookup->name[IPA_RESOURCE_NAME_MAX-1] = '\0';
	if (strnlen(lookup->name, IPA_RESOURCE_NAME_MAX) ==
			IPA_RESOURCE_NAME_MAX) {
		IPAERR_RL("Interface name too long. (%s)\n", lookup->name);
		return result;
	}

	mutex_lock(&ipa3_ctx->lock);
	list_for_each_entry(entry, &ipa3_ctx->intf_list, link) {
		if (!strcmp(entry->name, lookup->name)) {
			lookup->num_tx_props = entry->num_tx_props;
			lookup->num_rx_props = entry->num_rx_props;
			lookup->num_ext_props = entry->num_ext_props;
			lookup->excp_pipe = entry->excp_pipe;
			result = 0;
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);

	return result;
}
EXPORT_SYMBOL(ipa3_query_intf);

/**
 * ipa3_query_intf_tx_props() - qeury TX props of an interface
 * @tx:  [inout] interface tx attributes
 *
 * Obtain the tx properties for the specified interface
 *
 * Returns:	0 on success, negative on failure
 *
 * Note:	Should not be called from atomic context
 */
int ipa3_query_intf_tx_props(struct ipa_ioc_query_intf_tx_props *tx)
{
	struct ipa3_intf *entry;
	int result = -EINVAL;

	if (tx == NULL) {
		IPAERR_RL("null args: tx\n");
		return result;
	}

	tx->name[IPA_RESOURCE_NAME_MAX-1] = '\0';
	if (strnlen(tx->name, IPA_RESOURCE_NAME_MAX) == IPA_RESOURCE_NAME_MAX) {
		IPAERR_RL("Interface name too long. (%s)\n", tx->name);
		return result;
	}

	mutex_lock(&ipa3_ctx->lock);
	list_for_each_entry(entry, &ipa3_ctx->intf_list, link) {
		if (!strcmp(entry->name, tx->name)) {
			/* add the entry check */
			if (entry->num_tx_props != tx->num_tx_props) {
				IPAERR("invalid entry number(%u %u)\n",
					entry->num_tx_props,
						tx->num_tx_props);
				mutex_unlock(&ipa3_ctx->lock);
				return result;
			}
			memcpy(tx->tx, entry->tx, entry->num_tx_props *
			       sizeof(struct ipa_ioc_tx_intf_prop));
			result = 0;
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);

	return result;
}

/**
 * ipa3_query_intf_rx_props() - qeury RX props of an interface
 * @rx:  [inout] interface rx attributes
 *
 * Obtain the rx properties for the specified interface
 *
 * Returns:	0 on success, negative on failure
 *
 * Note:	Should not be called from atomic context
 */
int ipa3_query_intf_rx_props(struct ipa_ioc_query_intf_rx_props *rx)
{
	struct ipa3_intf *entry;
	int result = -EINVAL;

	if (rx == NULL) {
		IPAERR_RL("null args: rx\n");
		return result;
	}

	rx->name[IPA_RESOURCE_NAME_MAX-1] = '\0';
	if (strnlen(rx->name, IPA_RESOURCE_NAME_MAX) == IPA_RESOURCE_NAME_MAX) {
		IPAERR_RL("Interface name too long. (%s)\n", rx->name);
		return result;
	}

	mutex_lock(&ipa3_ctx->lock);
	list_for_each_entry(entry, &ipa3_ctx->intf_list, link) {
		if (!strcmp(entry->name, rx->name)) {
			/* add the entry check */
			if (entry->num_rx_props != rx->num_rx_props) {
				IPAERR("invalid entry number(%u %u)\n",
					entry->num_rx_props,
						rx->num_rx_props);
				mutex_unlock(&ipa3_ctx->lock);
				return result;
			}
			memcpy(rx->rx, entry->rx, entry->num_rx_props *
					sizeof(struct ipa_ioc_rx_intf_prop));
			result = 0;
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);

	return result;
}

/**
 * ipa3_query_intf_ext_props() - qeury EXT props of an interface
 * @ext:  [inout] interface ext attributes
 *
 * Obtain the ext properties for the specified interface
 *
 * Returns:	0 on success, negative on failure
 *
 * Note:	Should not be called from atomic context
 */
int ipa3_query_intf_ext_props(struct ipa_ioc_query_intf_ext_props *ext)
{
	struct ipa3_intf *entry;
	int result = -EINVAL;

	if (ext == NULL) {
		IPAERR_RL("invalid param ext=%pK\n", ext);
		return result;
	}

	mutex_lock(&ipa3_ctx->lock);
	list_for_each_entry(entry, &ipa3_ctx->intf_list, link) {
		if (!strcmp(entry->name, ext->name)) {
			/* add the entry check */
			if (entry->num_ext_props != ext->num_ext_props) {
				IPAERR("invalid entry number(%u %u)\n",
					entry->num_ext_props,
						ext->num_ext_props);
				mutex_unlock(&ipa3_ctx->lock);
				return result;
			}
			memcpy(ext->ext, entry->ext, entry->num_ext_props *
					sizeof(struct ipa_ioc_ext_intf_prop));
			result = 0;
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);
	return result;
}

static void ipa_send_msg_free(void *buff, u32 len, u32 type)
{
	kfree(buff);
}

static void wlan_ex_conn_mac_print( void *buff)
{
	struct ipa_wlan_msg_ex *event_ex_cur_con = buff;
	int cnt = 0;

	for (cnt = 0; cnt < event_ex_cur_con->num_of_attribs; cnt++) {
		if (event_ex_cur_con->attribs[cnt].attrib_type ==
				WLAN_HDR_ATTRIB_MAC_ADDR) {
			IPADBG("%02x:%02x:%02x:%02x:%02x:%02x\n",
				event_ex_cur_con->attribs[cnt].u.mac_addr[0],
				event_ex_cur_con->attribs[cnt].u.mac_addr[1],
				event_ex_cur_con->attribs[cnt].u.mac_addr[2],
				event_ex_cur_con->attribs[cnt].u.mac_addr[3],
				event_ex_cur_con->attribs[cnt].u.mac_addr[4],
				event_ex_cur_con->attribs[cnt].u.mac_addr[5]);
		}
	}
}

static int wlan_msg_process(struct ipa_msg_meta *meta, void *buff)
{
	struct ipa3_push_msg *msg_dup;
	struct ipa_wlan_msg_ex *event_ex_list = NULL;
	struct ipa_wlan_msg *event_list = NULL;
	struct ipa_wlan_msg *event_ex_cur_discon = NULL;
	struct ipa_wlan_msg *event_cur_con = NULL;
	void *data_dup = NULL;
	struct ipa3_push_msg *entry;
	struct ipa3_push_msg *next;
	int cnt = 0, total = 0, max = 0;
	uint8_t mac[IPA_MAC_ADDR_SIZE];
	uint8_t mac2[IPA_MAC_ADDR_SIZE];

	if (!buff)
		return -EINVAL;
	if (WLAN_IPA_CONNECT_EVENT(meta->msg_type)) {
		if (meta->msg_type == WLAN_CLIENT_CONNECT_EX) {
			/* debug print */
			wlan_ex_conn_mac_print(buff);
		} else {
			event_cur_con = buff;
			IPADBG("%02x:%02x:%02x:%02x:%02x:%02x,(%d)\n",
					event_cur_con->mac_addr[0],
					event_cur_con->mac_addr[1],
					event_cur_con->mac_addr[2],
					event_cur_con->mac_addr[3],
					event_cur_con->mac_addr[4],
					event_cur_con->mac_addr[5],
					meta->msg_type);
		}

		mutex_lock(&ipa3_ctx->msg_wlan_client_lock);
		msg_dup = kzalloc(sizeof(*msg_dup), GFP_KERNEL);
		if (msg_dup == NULL) {
			mutex_unlock(&ipa3_ctx->msg_wlan_client_lock);
			return -ENOMEM;
		}
		msg_dup->meta = *meta;
		if (meta->msg_len > 0 && buff) {
			data_dup = kmemdup(buff, meta->msg_len, GFP_KERNEL);
			if (data_dup == NULL) {
				kfree(msg_dup);
				mutex_unlock(&ipa3_ctx->msg_wlan_client_lock);
				return -ENOMEM;
			}
			memcpy(data_dup, buff, meta->msg_len);
			msg_dup->buff = data_dup;
			msg_dup->callback = ipa_send_msg_free;
		} else {
			IPAERR("msg_len %d\n", meta->msg_len);
			kfree(msg_dup);
			mutex_unlock(&ipa3_ctx->msg_wlan_client_lock);
			return -ENOMEM;
		}
		list_add_tail(&msg_dup->link, &ipa3_ctx->msg_wlan_client_list);
		mutex_unlock(&ipa3_ctx->msg_wlan_client_lock);
	}

	/* remove the cache */
	if (WLAN_IPA_DISCONNECT_EVENT(meta->msg_type)) {
		/* debug print */
		event_ex_cur_discon = buff;
		IPADBG("Mac %pM, msg %d\n",
		event_ex_cur_discon->mac_addr,
		meta->msg_type);
		memcpy(mac2,
			event_ex_cur_discon->mac_addr,
			sizeof(mac2));

		mutex_lock(&ipa3_ctx->msg_wlan_client_lock);
		list_for_each_entry_safe(entry, next,
				&ipa3_ctx->msg_wlan_client_list,
				link) {
			if(entry->meta.msg_type == WLAN_CLIENT_CONNECT_EX) {
				event_ex_list = entry->buff;
				max = event_ex_list->num_of_attribs;
				for (cnt = 0; cnt < max; cnt++) {
					memcpy(mac,
						event_ex_list->attribs[cnt].u.mac_addr,
						sizeof(mac));
					if (event_ex_list->attribs[cnt].attrib_type == WLAN_HDR_ATTRIB_MAC_ADDR) {
						/* compare to delete one*/
						if (memcmp(mac2, mac, sizeof(mac)) == 0) {
							IPADBG("clean %d\n", total);
							list_del(&entry->link);
							entry->callback(entry->buff,
								entry->meta.msg_len, entry->meta.msg_type);
							kfree(entry);
							break;
						}
					}
				}
			} else {
				event_list = entry->buff;
				memcpy(mac, event_list->mac_addr, sizeof(mac));
				/* compare to delete one*/
				if (memcmp(mac2, mac, sizeof(mac)) == 0) {
					IPADBG("clean %d\n", total);
					list_del(&entry->link);
					entry->callback(entry->buff, entry->meta.msg_len, entry->meta.msg_type);
					kfree(entry);
					break;
				}
			}
			total++;
		}
		mutex_unlock(&ipa3_ctx->msg_wlan_client_lock);
	}
	return 0;
}

static int lan_msg_process(struct ipa_msg_meta *meta, void *buff)
{
	struct ipa3_push_msg *msg_dup = NULL;
	struct ipa_ecm_msg *ecm_msg_con = NULL;
	struct ipa_ecm_msg *ecm_event_list = NULL;
	struct ipa_ecm_msg *ecm_msg_discon = NULL;
	struct ipa3_push_msg *entry;
	struct ipa3_push_msg *next;
	void *data_dup = NULL;
	int iface_index = 0;

	if (!buff)
		return -EINVAL;

	if (meta->msg_type == ECM_CONNECT) {
		/*debug print */
		ecm_msg_con = buff;
		IPADBG("ifindex: %d\n", ecm_msg_con->ifindex);
		IPADBG("interface name: %s\n", ecm_msg_con->name);

		mutex_lock(&ipa3_ctx->msg_lan_lock);
		if (meta->msg_len > 0 && buff) {

			msg_dup = kzalloc(sizeof(*msg_dup), GFP_KERNEL);
			if (msg_dup == NULL) {
				mutex_unlock(&ipa3_ctx->msg_lan_lock);
				return -ENOMEM;
			}

			msg_dup->meta = *meta;
			data_dup = kmalloc(meta->msg_len, GFP_KERNEL);
			if (data_dup == NULL) {
				kfree(msg_dup);
				mutex_unlock(&ipa3_ctx->msg_lan_lock);
				return -ENOMEM;
			}
			memcpy(data_dup, buff, meta->msg_len);
			msg_dup->buff = data_dup;
			msg_dup->callback = ipa_send_msg_free;
		} else {
			IPAERR("msg_len %d\n", meta->msg_len);
			mutex_unlock(&ipa3_ctx->msg_lan_lock);
			return -EINVAL;
		}
		list_add_tail(&msg_dup->link, &ipa3_ctx->msg_lan_list);
		mutex_unlock(&ipa3_ctx->msg_lan_lock);
	}

	/* remove the cache */
	if (meta->msg_type == ECM_DISCONNECT) {
		/* debug print */
		ecm_msg_discon = buff;
		iface_index = ecm_msg_discon->ifindex;

		IPADBG("ifindex: %d\n", ecm_msg_discon->ifindex);
		IPADBG("interface name: %s\n", ecm_msg_discon->name);

		mutex_lock(&ipa3_ctx->msg_lan_lock);
		list_for_each_entry_safe(entry, next,
			&ipa3_ctx->msg_lan_list, link) {
			ecm_event_list = entry->buff;

			/* compare to delete one*/
			if (iface_index == ecm_event_list->ifindex) {
				IPADBG("Delete event for iface index: %d\n",
				iface_index);
				list_del(&entry->link);
				entry->callback(entry->buff, entry->meta.msg_len, entry->meta.msg_type);
				kfree(entry);
			}
		}

		mutex_unlock(&ipa3_ctx->msg_lan_lock);

	}

	return 0;
}

static int qos_msg_process(struct ipa_msg_meta *meta, void *buff)
{
	struct ipa3_push_msg *msg_dup;
	void *data_dup = NULL;
	struct ipa3_push_msg *entry;
	struct ipa3_push_msg *next;
	struct ipa_ioc_qos_config *event_ex_list = NULL;
	struct ipa_ioc_qos_config *qos_param = NULL;

	if (!buff)
		return -EINVAL;
	if (meta->msg_type == IPA_QOS_PARAM_ADD_EVENT) {
		qos_param = buff;
		mutex_lock(&ipa3_ctx->msg_qos_param_lock);
		list_for_each_entry_safe(entry, next,
				&ipa3_ctx->msg_qos_param_list,
				link) {
			event_ex_list = entry->buff;
			if (event_ex_list->traffic_class ==
					qos_param->traffic_class &&
				event_ex_list->ip_type ==
					qos_param->ip_type &&
				event_ex_list->src_ip_addr ==
					qos_param->src_ip_addr &&
				event_ex_list->dst_ip_addr ==
					qos_param->dst_ip_addr &&
				event_ex_list->src_port_start ==
					qos_param->src_port_start &&
				event_ex_list->src_port_end ==
					qos_param->src_port_end &&
				event_ex_list->dst_port_start ==
					qos_param->dst_port_start &&
				event_ex_list->dst_port_end ==
					qos_param->dst_port_end &&
				event_ex_list->protocol ==
					qos_param->protocol &&
				event_ex_list->dscp ==
					qos_param->dscp &&
				event_ex_list->pcp ==
					qos_param->pcp &&
				!memcmp(event_ex_list->src_v6_ip_addr,
					qos_param->src_v6_ip_addr,
					sizeof(qos_param->src_v6_ip_addr)) &&
				!memcmp(event_ex_list->dst_v6_ip_addr,
					qos_param->dst_v6_ip_addr,
					sizeof(qos_param->dst_v6_ip_addr)) &&
				!memcmp(event_ex_list->dst_mac_addr,
					qos_param->dst_mac_addr,
					sizeof(qos_param->dst_mac_addr)) &&
				!memcmp(event_ex_list->src_mac_addr,
					qos_param->src_mac_addr,
					sizeof(qos_param->src_mac_addr))
				) {
					IPADBG("Duplicate entry, ignore\n");
					mutex_unlock(&ipa3_ctx->msg_qos_param_lock);
					return 0;
				}
		}
		msg_dup = kzalloc(sizeof(*msg_dup), GFP_KERNEL);
		if (msg_dup == NULL) {
			mutex_unlock(&ipa3_ctx->msg_qos_param_lock);
			return -ENOMEM;
		}
		msg_dup->meta = *meta;
		if (meta->msg_len > 0 && buff) {
			data_dup = kmemdup(buff, meta->msg_len, GFP_KERNEL);
			if (data_dup == NULL) {
				kfree(msg_dup);
				mutex_unlock(&ipa3_ctx->msg_qos_param_lock);
				return -ENOMEM;
			}
			memcpy(data_dup, buff, meta->msg_len);
			msg_dup->buff = data_dup;
			msg_dup->callback = ipa_send_msg_free;
		} else {
			IPAERR("msg_len %d\n", meta->msg_len);
			kfree(msg_dup);
			mutex_unlock(&ipa3_ctx->msg_qos_param_lock);
			return -ENOMEM;
		}
		list_add_tail(&msg_dup->link, &ipa3_ctx->msg_qos_param_list);
		mutex_unlock(&ipa3_ctx->msg_qos_param_lock);
	} else if (meta->msg_type == IPA_QOS_PARAM_DELETE_EVENT) {
		/* debug print */
		qos_param = buff;
		mutex_lock(&ipa3_ctx->msg_qos_param_lock);
		list_for_each_entry_safe(entry, next,
				&ipa3_ctx->msg_qos_param_list,
				link) {
			if(entry->meta.msg_type == IPA_QOS_PARAM_ADD_EVENT) {
				event_ex_list = entry->buff;
				if (event_ex_list->traffic_class ==
						qos_param->traffic_class &&
					event_ex_list->ip_type ==
						qos_param->ip_type &&
					event_ex_list->src_ip_addr ==
						qos_param->src_ip_addr &&
					event_ex_list->dst_ip_addr ==
						qos_param->dst_ip_addr &&
					event_ex_list->src_port_start ==
						qos_param->src_port_start &&
					event_ex_list->src_port_end ==
						qos_param->src_port_end &&
					event_ex_list->dst_port_start ==
						qos_param->dst_port_start &&
					event_ex_list->dst_port_end ==
						qos_param->dst_port_end &&
					event_ex_list->protocol ==
						qos_param->protocol &&
					event_ex_list->dscp ==
						qos_param->dscp &&
					event_ex_list->pcp ==
						qos_param->pcp &&
					!memcmp(event_ex_list->src_v6_ip_addr,
						qos_param->src_v6_ip_addr,
						sizeof(qos_param->src_v6_ip_addr)) &&
					!memcmp(event_ex_list->dst_v6_ip_addr,
						qos_param->dst_v6_ip_addr,
						sizeof(qos_param->dst_v6_ip_addr)) &&
					!memcmp(event_ex_list->dst_mac_addr,
						qos_param->dst_mac_addr,
						sizeof(qos_param->dst_mac_addr)) &&
					!memcmp(event_ex_list->src_mac_addr,
						qos_param->src_mac_addr,
						sizeof(qos_param->src_mac_addr))
					) {
					IPADBG("clean the entry\n");
					list_del(&entry->link);
					entry->callback(entry->buff,
						entry->meta.msg_len,
						entry->meta.msg_type);
					kfree(entry);
					break;
				}
			}
		}
		mutex_unlock(&ipa3_ctx->msg_qos_param_lock);
	}else if (meta->msg_type == IPA_QOS_PARAM_FLUSH_EVENT) {
		IPADBG("Delete all entries\n");
		mutex_lock(&ipa3_ctx->msg_qos_param_lock);
		list_for_each_entry_safe(entry, next,
				&ipa3_ctx->msg_qos_param_list,
				link) {
				list_del(&entry->link);
				entry->callback(entry->buff,
					entry->meta.msg_len, entry->meta.msg_type);
				kfree(entry);
		}
		mutex_unlock(&ipa3_ctx->msg_qos_param_lock);
	}
	return 0;
}

/**
 * ipa_send_msg() - Send "message" from kernel client to IPA driver
 * @meta: [in] message meta-data
 * @buff: [in] the payload for message
 * @callback: [in] free callback
 *
 * Client supplies the message meta-data and payload which IPA driver buffers
 * till read by user-space. After read from user space IPA driver invokes the
 * callback supplied to free the message payload. Client must not touch/free
 * the message payload after calling this API.
 *
 * Returns:	0 on success, negative on failure
 *
 * Note:	Should not be called from atomic context
 */
int ipa_send_msg(struct ipa_msg_meta *meta, void *buff,
		  ipa_msg_free_fn callback)
{
	struct ipa3_push_msg *msg;
	void *data = NULL;

	if (meta == NULL || (buff == NULL && callback != NULL) ||
	    (buff != NULL && callback == NULL)) {
		IPAERR_RL("invalid param meta=%pK buff=%pK, callback=%pK\n",
		       meta, buff, callback);
		return -EINVAL;
	}

	if (meta->msg_type >= IPA_EVENT_MAX_NUM) {
		IPAERR_RL("unsupported message type %d\n", meta->msg_type);
		return -EINVAL;
	}

	msg = kzalloc(sizeof(struct ipa3_push_msg), GFP_KERNEL);
	if (msg == NULL)
		return -ENOMEM;

	msg->meta = *meta;
	if (meta->msg_len > 0 && buff) {
		data = kmemdup(buff, meta->msg_len, GFP_KERNEL);
		if (data == NULL) {
			kfree(msg);
			return -ENOMEM;
		}
		msg->buff = data;
		msg->callback = ipa_send_msg_free;
	}

	mutex_lock(&ipa3_ctx->msg_lock);
	list_add_tail(&msg->link, &ipa3_ctx->msg_list);
	/* support for softap client event cache */
	if (wlan_msg_process(meta, buff))
		IPADBG("wlan_msg_process failed\n");

	if (lan_msg_process(meta, buff))
		IPADBG("lan_msg_process failed\n");

	if (qos_msg_process(meta, buff))
		IPAERR_RL("qos_msg_process failed\n");
	/* unlock only after process */
	mutex_unlock(&ipa3_ctx->msg_lock);
	IPA_STATS_INC_CNT(ipa3_ctx->stats.msg_w[meta->msg_type]);

	wake_up(&ipa3_ctx->msg_waitq);
	if (buff)
		callback(buff, meta->msg_len, meta->msg_type);

	return 0;
}
EXPORT_SYMBOL(ipa_send_msg);

/**
 * ipa3_resend_wlan_msg() - Resend cached "message" to IPACM
 *
 * resend wlan client connect/AP_CONNECT/STA_CONNECT events to 
 * user-space
 *
 * Returns:	0 on success, negative on failure
 *
 * Note:	Should not be called from atomic context
 */
int ipa3_resend_wlan_msg(void)
{
	struct ipa_wlan_msg_ex *event_ex_list = NULL;
	struct ipa3_push_msg *entry;
	struct ipa3_push_msg *next;
	struct ipa_msg_meta *meta;
	int cnt = 0, total = 0;
	struct ipa3_push_msg *msg;
	void *data = NULL;

	IPADBG("\n");

	mutex_lock(&ipa3_ctx->msg_wlan_client_lock);
	list_for_each_entry_safe(entry, next, &ipa3_ctx->msg_wlan_client_list,
			link) {

		meta = &(entry->meta);
		if (meta->msg_type == WLAN_CLIENT_CONNECT_EX) {
			event_ex_list = entry->buff;
			for (cnt = 0; cnt < event_ex_list->num_of_attribs; cnt++) {
				if (event_ex_list->attribs[cnt].attrib_type ==
						WLAN_HDR_ATTRIB_MAC_ADDR) {
					IPADBG("%d-Mac %pM\n", total,
					event_ex_list->attribs[cnt].u.mac_addr);
				}
			}
		}

		msg = kzalloc(sizeof(*msg), GFP_KERNEL);
		if (msg == NULL) {
			mutex_unlock(&ipa3_ctx->msg_wlan_client_lock);
			return -ENOMEM;
		}
		msg->meta = entry->meta;
		data = kmemdup(entry->buff, entry->meta.msg_len, GFP_KERNEL);
		if (data == NULL) {
			kfree(msg);
			mutex_unlock(&ipa3_ctx->msg_wlan_client_lock);
			return -ENOMEM;
		}
		msg->buff = data;
		msg->callback = ipa_send_msg_free;
		mutex_lock(&ipa3_ctx->msg_lock);
		list_add_tail(&msg->link, &ipa3_ctx->msg_list);
		mutex_unlock(&ipa3_ctx->msg_lock);
		wake_up(&ipa3_ctx->msg_waitq);

		total++;
	}
	mutex_unlock(&ipa3_ctx->msg_wlan_client_lock);
	return 0;
}

/**
 * ipa3_resend_lan_msg() - Resend cached "message" to IPACM
 *
 * resend ecm connect/disconnect events to user-space
 *
 * Returns:     0 on success, negative on failure
 *
 * Note:        Should not be called from atomic context
 */

int ipa3_resend_lan_msg(void)
{
	struct ipa3_push_msg *entry = NULL;
	struct ipa3_push_msg *next = NULL;
	struct ipa_ecm_msg *ecm_msg = NULL;
	struct ipa3_push_msg *msg = NULL;
	void *data = NULL;

	IPADBG("\n");
	mutex_lock(&ipa3_ctx->msg_lan_lock);
	list_for_each_entry_safe(entry, next, &ipa3_ctx->msg_lan_list, link) {
		ecm_msg = entry->buff;

		IPADBG("ifindex: %d\n", ecm_msg->ifindex);
		IPADBG("interface name: %s\n", ecm_msg->name);

		msg = kzalloc(sizeof(*msg), GFP_KERNEL);
		if (msg == NULL) {
			mutex_unlock(&ipa3_ctx->msg_lan_lock);
			return -ENOMEM;
		}
		msg->meta = entry->meta;
		data = kzalloc(entry->meta.msg_len, GFP_KERNEL);
		if (data == NULL) {
			kfree(msg);
			mutex_unlock(&ipa3_ctx->msg_lan_lock);
			return -ENOMEM;
		}
		memcpy(data, entry->buff, entry->meta.msg_len);
		msg->buff = data;
		msg->callback = ipa_send_msg_free;
		mutex_lock(&ipa3_ctx->msg_lock);
		list_add_tail(&msg->link, &ipa3_ctx->msg_list);
		mutex_unlock(&ipa3_ctx->msg_lock);
		wake_up(&ipa3_ctx->msg_waitq);
	}
	mutex_unlock(&ipa3_ctx->msg_lan_lock);

	return 0;
}

/**
 * ipa3_resend_qos_msg() - Resend cached "message" to IPACM
 *
 * resend QOS events to user-space
 *
 * Returns:     0 on success, negative on failure
 *
 * Note:        Should not be called from atomic context
 */

int ipa3_resend_qos_msg(void)
{
	struct ipa3_push_msg *entry = NULL;
	struct ipa3_push_msg *next = NULL;
	struct ipa_ioc_qos_config *qos_param = NULL;
	struct ipa3_push_msg *msg = NULL;
	void *data = NULL;

	IPADBG("\n");
	mutex_lock(&ipa3_ctx->msg_qos_param_lock);
	list_for_each_entry_safe(entry, next, &ipa3_ctx->msg_qos_param_list, link) {
		qos_param = entry->buff;

		IPADBG("QOS Msg type: %d\n", qos_param->qos_param_evt_type);
		IPADBG("interface name: %s\n", qos_param->dev_name);

		msg = kzalloc(sizeof(*msg), GFP_KERNEL);
		if (msg == NULL) {
			mutex_unlock(&ipa3_ctx->msg_qos_param_lock);
			return -ENOMEM;
		}
		msg->meta = entry->meta;
		data = kzalloc(entry->meta.msg_len, GFP_KERNEL);
		if (data == NULL) {
			kfree(msg);
			mutex_unlock(&ipa3_ctx->msg_qos_param_lock);
			return -ENOMEM;
		}
		memcpy(data, entry->buff, entry->meta.msg_len);
		msg->buff = data;
		msg->callback = ipa_send_msg_free;
		mutex_lock(&ipa3_ctx->msg_lock);
		list_add_tail(&msg->link, &ipa3_ctx->msg_list);
		mutex_unlock(&ipa3_ctx->msg_lock);
		wake_up(&ipa3_ctx->msg_waitq);
	}
	mutex_unlock(&ipa3_ctx->msg_qos_param_lock);

	return 0;
}

/*
 * ipa3_send_done_restore_msg() - Resend done_restore_msg to IPACM
 *
 * Returns:     0 on success, negative on failure
 *
 * Note:        Should not be called from atomic context
 *
 */

static int ipa3_send_done_restore_msg(void)
{
	struct ipa3_push_msg *msg = NULL;

	IPADBG("\n");

	msg = kzalloc(sizeof(*msg), GFP_KERNEL);
	if (msg == NULL)
		return -ENOMEM;
	msg->meta.msg_type = IPA_DONE_RESTORE_EVENT;
	msg->buff = NULL;
	msg->callback = ipa_send_msg_free;

	mutex_lock(&ipa3_ctx->msg_lock);
	list_add_tail(&msg->link, &ipa3_ctx->msg_list);
	mutex_unlock(&ipa3_ctx->msg_lock);
	wake_up(&ipa3_ctx->msg_waitq);

	return 0;
}

/*
 * ipa3_resend_driver_msg() - Resend done_restore_msg to IPACM
 *
 * Returns:     0 on success, negative on failure
 *
 */

int ipa3_resend_driver_msg(void)
{
	int retval = 0;
	IPADBG("resend wlan msg\n");
	retval = ipa3_resend_wlan_msg();
	if (retval)
		goto fail;

	IPADBG("resend lan msg\n");
	retval = ipa3_resend_lan_msg();
	if (retval)
		goto fail;

	IPADBG("resend qos msg\n");
	retval = ipa3_resend_qos_msg();
	if (retval)
		goto fail;

	IPADBG("send IPA_DONE_RESTORE_EVENT\n");
	retval = ipa3_send_done_restore_msg();

fail:
	return retval;
}

/**
 * ipa3_register_pull_msg() - register pull message type
 * @meta: [in] message meta-data
 * @callback: [in] pull callback
 *
 * Register message callback by kernel client with IPA driver for IPA driver to
 * pull message on-demand.
 *
 * Returns:	0 on success, negative on failure
 *
 * Note:	Should not be called from atomic context
 */
int ipa3_register_pull_msg(struct ipa_msg_meta *meta, ipa_msg_pull_fn callback)
{
	struct ipa3_pull_msg *msg;

	if (meta == NULL || callback == NULL) {
		IPAERR_RL("invalid param meta=%pK callback=%pK\n",
				meta, callback);
		return -EINVAL;
	}

	msg = kzalloc(sizeof(struct ipa3_pull_msg), GFP_KERNEL);
	if (msg == NULL)
		return -ENOMEM;

	msg->meta = *meta;
	msg->callback = callback;

	mutex_lock(&ipa3_ctx->msg_lock);
	list_add_tail(&msg->link, &ipa3_ctx->pull_msg_list);
	mutex_unlock(&ipa3_ctx->msg_lock);

	return 0;
}

/**
 * ipa3_deregister_pull_msg() - De-register pull message type
 * @meta: [in] message meta-data
 *
 * De-register "message" by kernel client from IPA driver
 *
 * Returns:	0 on success, negative on failure
 *
 * Note:	Should not be called from atomic context
 */
int ipa3_deregister_pull_msg(struct ipa_msg_meta *meta)
{
	struct ipa3_pull_msg *entry;
	struct ipa3_pull_msg *next;
	int result = -EINVAL;

	if (meta == NULL) {
		IPAERR_RL("null arg: meta\n");
		return result;
	}

	mutex_lock(&ipa3_ctx->msg_lock);
	list_for_each_entry_safe(entry, next, &ipa3_ctx->pull_msg_list, link) {
		if (entry->meta.msg_len == meta->msg_len &&
		    entry->meta.msg_type == meta->msg_type) {
			list_del(&entry->link);
			kfree(entry);
			result = 0;
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->msg_lock);
	return result;
}

/**
 * ipa3_read() - read message from IPA device
 * @filp:	[in] file pointer
 * @buf:	[out] buffer to read into
 * @count:	[in] size of above buffer
 * @f_pos:	[inout] file position
 *
 * Uer-space should continually read from /dev/ipa, read wll block when there
 * are no messages to read. Upon return, user-space should read the ipa_msg_meta
 * from the start of the buffer to know what type of message was read and its
 * length in the remainder of the buffer. Buffer supplied must be big enough to
 * hold the message meta-data and the largest defined message type
 *
 * Returns:	how many bytes copied to buffer
 *
 * Note:	Should not be called from atomic context
 */
ssize_t ipa3_read(struct file *filp, char __user *buf, size_t count,
		  loff_t *f_pos)
{
	char __user *start;
	struct ipa3_push_msg *msg = NULL;
	int ret;
	DEFINE_WAIT_FUNC(wait, woken_wake_function);
	int locked;

	start = buf;

	add_wait_queue(&ipa3_ctx->msg_waitq, &wait);
	while (1) {
		mutex_lock(&ipa3_ctx->msg_lock);
		locked = 1;

		if (!list_empty(&ipa3_ctx->msg_list)) {
			msg = list_first_entry(&ipa3_ctx->msg_list,
					struct ipa3_push_msg, link);
			list_del(&msg->link);
		}

		IPADBG_LOW("msg=%pK\n", msg);

		if (msg) {
			locked = 0;
			mutex_unlock(&ipa3_ctx->msg_lock);
			if (count < sizeof(struct ipa_msg_meta)) {
				kfree(msg);
				msg = NULL;
				ret = -EFAULT;
				break;
			}
			if (copy_to_user(buf, &msg->meta,
					  sizeof(struct ipa_msg_meta))) {
				IPAERR_RL("Failed to copy the data to user space\n");

				ret = -EFAULT;
				if(msg->buff) {
					msg->callback(msg->buff, msg->meta.msg_len,
						msg->meta.msg_type);
				}
				kfree(msg);
				msg = NULL;
				break;
			}
			buf += sizeof(struct ipa_msg_meta);
			count -= sizeof(struct ipa_msg_meta);
			if (msg->buff) {
				if (count >= msg->meta.msg_len) {
					if (copy_to_user(buf, msg->buff,
								msg->meta.msg_len)) {
						IPAERR_RL("Failed to copy the data to user space\n");
						if(msg->buff) {
							msg->callback(msg->buff, msg->meta.msg_len,
									msg->meta.msg_type);
						}
						ret = -EFAULT;
						kfree(msg);
						msg = NULL;
						break;
					}
				} else {
					ret = -EFAULT;
					kfree(msg);
					msg = NULL;
					break;
				}
				buf += msg->meta.msg_len;
				count -= msg->meta.msg_len;
				msg->callback(msg->buff, msg->meta.msg_len,
					       msg->meta.msg_type);
			}
			IPA_STATS_INC_CNT(
				ipa3_ctx->stats.msg_r[msg->meta.msg_type]);
			kfree(msg);
			msg = NULL;
		}

		ret = -EAGAIN;
		if (filp->f_flags & O_NONBLOCK)
			break;

		ret = -EINTR;
		if (signal_pending(current))
			break;

		if (start != buf)
			break;

		locked = 0;
		mutex_unlock(&ipa3_ctx->msg_lock);
		wait_woken(&wait, TASK_INTERRUPTIBLE, MAX_SCHEDULE_TIMEOUT);
	}

	remove_wait_queue(&ipa3_ctx->msg_waitq, &wait);
	if (start != buf && ret != -EFAULT)
		ret = buf - start;

	if (locked)
		mutex_unlock(&ipa3_ctx->msg_lock);

	return ret;
}

/**
 * ipa3_pull_msg() - pull the specified message from client
 * @meta: [in] message meta-data
 * @buf:  [out] buffer to read into
 * @count: [in] size of above buffer
 *
 * Populate the supplied buffer with the pull message which is fetched
 * from client, the message must have previously been registered with
 * the IPA driver
 *
 * Returns:	how many bytes copied to buffer
 *
 * Note:	Should not be called from atomic context
 */
int ipa3_pull_msg(struct ipa_msg_meta *meta, char *buff, size_t count)
{
	struct ipa3_pull_msg *entry;
	int result = -EINVAL;

	if (meta == NULL || buff == NULL || !count) {
		IPAERR_RL("invalid param name=%pK buff=%pK count=%zu\n",
				meta, buff, count);
		return result;
	}

	mutex_lock(&ipa3_ctx->msg_lock);
	list_for_each_entry(entry, &ipa3_ctx->pull_msg_list, link) {
		if (entry->meta.msg_len == meta->msg_len &&
		    entry->meta.msg_type == meta->msg_type) {
			result = entry->callback(buff, count, meta->msg_type);
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->msg_lock);
	return result;
}

/**
 * ipa3_query_iface() - pull the specified message from client
 * @intf_idx: [in] message meta-data
 * @target_intf:  [out] buffer to read into
 *
 * Populate the supplied buffer with the pull message which is fetched
 * from client, the message must have previously been registered with
 * the IPA driver
 *
 * Returns:	if iface exists or not
 *
 */
bool ipa3_query_iface(int intf_idx, struct ipa_ioc_query_intf *target_intf)
{
	bool ret = false;
	struct ipa3_intf *entry;
	IPADBG("Entry \n");
	if (target_intf == NULL) {
		 return ret;
	}

	mutex_lock(&ipa3_ctx->lock);
	list_for_each_entry(entry, &ipa3_ctx->intf_list, link) {
		IPADBG("Checking entry->intf_idx %s :%d with intf_idx %d \n", entry->name , entry->intf_idx, intf_idx);
		if (entry->intf_idx == intf_idx) {
			IPADBG("Iface found at idx %d \n", intf_idx);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
			strscpy(target_intf->name, entry->name, IPA_RESOURCE_NAME_MAX);
#else
			strlcpy(target_intf->name, entry->name, IPA_RESOURCE_NAME_MAX);
#endif
			target_intf->num_tx_props = entry->num_tx_props;
			target_intf->num_rx_props = entry->num_rx_props;
			target_intf->num_ext_props = entry->num_ext_props;
			target_intf->excp_pipe = entry->excp_pipe;

			ret = true;
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);

	IPADBG("Exit \n");
	return ret;
}
EXPORT_SYMBOL(ipa3_query_iface);

int ipa3_update_intf_idx(const char *name, int intf_idx)
{
	struct ipa3_intf *entry;
	int ret = -ENOENT;

	if (!name) {
		IPAERR("Invalid name (NULL), intf_idx=%d\n", intf_idx);
		return -EINVAL;
	}

	IPADBG("Entry: name=%s intf_idx=%d\n", name, intf_idx);

	mutex_lock(&ipa3_ctx->lock);
	list_for_each_entry(entry, &ipa3_ctx->intf_list, link) {
		IPADBG("Scanning intf %s (intf_idx=%d)\n",
			entry->name, entry->intf_idx);
		if (strcmp(entry->name, name) == 0) {
			if (entry->intf_idx != intf_idx) {
				IPADBG("Updating %s intf_idx %d -> %d\n",
					entry->name, entry->intf_idx, intf_idx);
				entry->intf_idx = intf_idx;
			} else {
				IPADBG("%s already at intf_idx=%d, no-op\n",
					entry->name, intf_idx);
			}
			ret = 0;
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);

	if (ret == -ENOENT)
		IPADBG("intf %s not found in intf_list\n", name);

	IPADBG("Exit: name=%s ret=%d\n", name, ret);
	return ret;
}

/**
 * ipa3_populate_cookie_vpnum() - get vpnum from interface index
 * @intf_idx: [in] interface index
 * @cookie:   [out] SW producer cookie to populate with vpnum (wdi6 only)
 *
 * Behaviour per interface type:
 *  - WDI6 (vpnum_valid == true):  sets cookie->wdi6.vp_num = vpnum
 *  - Other WDI protocol (vpnum_valid == false):
 *      cookie is left untouched (protocol has its own cookie layout)
 *  - Non-WDI interface:
 *      cookie is left untouched
 *  - Interface not found: cookie is left untouched
 */
void ipa3_populate_cookie_vpnum(int intf_idx, struct ipa_sw_producer_cookie *cookie)
{
	struct ipa3_intf *entry;

	if (!cookie)
		return;

	/* Validate interface index is non-negative */
	if (intf_idx < 0) {
		IPAERR("Invalid interface index %d\n", intf_idx);
		return;
	}

	mutex_lock(&ipa3_ctx->lock);
	list_for_each_entry(entry, &ipa3_ctx->intf_list, link) {
		if (entry->intf_idx == intf_idx) {
			if (entry->vpnum_valid) {
				/* WDI6: populate the vp_num field */
				cookie->wdi6.vp_num = entry->vpnum;
				IPADBG("vp_num %d\n", entry->vpnum);
			} else {
				IPADBG("Interface index %d found but no vp num\n", intf_idx);
			}
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);
}
EXPORT_SYMBOL(ipa3_populate_cookie_vpnum);
/**
 * ipa3_is_vpnum_valid() - check if interface has a valid vpnum
 * @intf_idx: [in] interface index
 *
 * Return: true if valid, false otherwise
 */
bool ipa3_is_vpnum_valid(int intf_idx)
{
	struct ipa3_intf *entry;
	bool valid = false;

	/* Validate interface index */
	if (intf_idx < 0) {
		IPAERR("Invalid interface index %d\n", intf_idx);
		return false;
	}

	mutex_lock(&ipa3_ctx->lock);
	list_for_each_entry(entry, &ipa3_ctx->intf_list, link) {
		if (entry->intf_idx == intf_idx) {
			valid = entry->vpnum_valid;
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);

	IPADBG("intf_idx=%d vpnum_valid=%d\n", intf_idx, valid);
	return valid;
}
EXPORT_SYMBOL(ipa3_is_vpnum_valid);

/**
 * ipa3_add_filter_rules_entry - Add filter entry to interface filter list
 * @intf_idx:   Network interface index for adding the filter entry
 * @flt_entry:  Filter entry struct containing rule and associated data
 *
 * Searches for the network interface with index @intf_idx in ipa3_ctx's
 * interface list. If the interface is found, allocates and appends a
 * new filter entry to its filter list. If the filter list does not exist,
 * it is allocated and initialized first. All fields of the new filter
 * entry are copied from @flt_entry.
 *
 * The function uses @ipa3_ctx->lock mutex for thread safety. Key events
 * and errors are logged for debugging.
 *
 * Return: true if interface found and entry added successfully, false
 * otherwise (e.g., out of memory, interface not found).
 */
bool ipa3_add_filter_rules_entry(int intf_idx, struct ipa3_flt_entry flt_entry)
{
	bool ret = false;
	struct ipa3_intf *entry;
	struct ipa3_flt_entry *new_entry;
	struct ipa3_flt_entry *iter;
	int list_len = 0;

	IPADBG("Entry\n");

	mutex_lock(&ipa3_ctx->lock);
	list_for_each_entry(entry, &ipa3_ctx->intf_list, link) {
		IPADBG("Checking entry->intf_idx %s :%d with intf_idx %d\n", entry->name, entry->intf_idx, intf_idx);
		if (entry->intf_idx == intf_idx) {
			IPADBG("Iface found at idx %d\n", intf_idx);

			if (!entry->flt_list) {
				IPADBG("flt_list is NULL for intf_idx %d, allocating...\n", intf_idx);

				entry->flt_list = kzalloc(sizeof(*entry->flt_list), GFP_KERNEL);
				if (!entry->flt_list) {
					IPAERR("Failed to allocate memory for flt_list\n");
					kfree(new_entry);  // cleanup if already allocated
					break;
				}

				INIT_LIST_HEAD(&entry->flt_list->link);
			}


			new_entry = kzalloc(sizeof(*new_entry), GFP_KERNEL);
			if (!new_entry) {
				IPAERR("Failed to allocate memory for new filter entry\n");
				break;
			}

            // Copy values from input entry to new entry
			new_entry->cookie = flt_entry.cookie;
			new_entry->rule = flt_entry.rule;
			new_entry->tbl = flt_entry.tbl;
            new_entry->rt_tbl = flt_entry.rt_tbl;
            new_entry->hw_len = flt_entry.hw_len;
            new_entry->id = flt_entry.id;
            new_entry->prio = flt_entry.prio;
            new_entry->rule_id = flt_entry.rule_id;
            new_entry->cnt_idx = flt_entry.cnt_idx;
            new_entry->ipacm_installed = flt_entry.ipacm_installed;
			new_entry->flt_hdl = flt_entry.flt_hdl;
			new_entry->cat = flt_entry.cat;
			new_entry->ip_type = flt_entry.ip_type;

			INIT_LIST_HEAD(&new_entry->link);
			list_add_tail(&new_entry->link, &entry->flt_list->link);

			// Count entries in the list
            list_for_each_entry(iter, &entry->flt_list->link, link) {
                list_len++;
            }
            IPADBG("Filter list size after addition: %d\n", list_len);

			IPADBG("Added flt_hdl %d, cat %d to the list , list size now %d\n", flt_entry.flt_hdl, flt_entry.cat, list_len);
			ret = true;
			break;
        }
    }
    mutex_unlock(&ipa3_ctx->lock);

    IPADBG("Exit\n");
    return ret;
}

/**
 * ipa3_delete_filter_rules_entry - Delete a filter rule from an
 * interface's filter list.
 * @intf_idx: Interface index to identify the target interface.
 * @flt_entry: Filter entry containing the rule to be deleted.
 *
 * This function searches the filter list associated with the
 * specified interface for a rule matching the given filter
 * entry. If a matching rule is found, it is removed from the
 * list and its memory is freed. The function returns the filter
 * handle (flt_hdl) of the deleted rule.
 *
 * Matching is currently based on rule_id, priority, and attrib_mask.
 * Additional fields can be added to the match criteria for stricter validation.
 *
 * Return: flt_hdl of the deleted rule if found, -1 otherwise.
 */
int ipa3_delete_filter_rules_entry(int intf_idx, struct ipa3_flt_entry flt_entry)
{
	struct ipa3_intf *entry;
	struct ipa3_flt_entry *iter, *tmp;
	int flt_hdl = -1;

	IPADBG("Delete Entry Start\n");

	mutex_lock(&ipa3_ctx->lock);
	list_for_each_entry(entry, &ipa3_ctx->intf_list, link) {
		IPADBG("Checking interface: %s (index: %d)\n", entry->name, entry->intf_idx);

		if (entry->intf_idx == intf_idx) {
			IPADBG("Matched interface index: %d\n", intf_idx);

			if (!entry->flt_list) {
				IPAERR("Filter list is NULL for intf_idx %d\n", intf_idx);
				break;
			}

			list_for_each_entry_safe(iter, tmp, &entry->flt_list->link, link) {
				IPADBG("Inspecting rule: rule_id=%d, prio=%d, attrib_mask=0x%x, flt_hdl=%d cat=%d\n",
				iter->rule.rule_id, iter->prio,
				iter->rule.attrib.attrib_mask, iter->flt_hdl, iter->cat);

				if ((iter->rule.attrib.attrib_mask & flt_entry.rule.attrib.attrib_mask ||
					iter->cat == flt_entry.cat) && iter->ip_type == flt_entry.ip_type) {
					IPADBG("Match found. Deleting rule with rule_id: %d, flt_hdl: %d cat: %d ip_type: %d\n",
					iter->rule.rule_id, iter->flt_hdl, iter->cat, iter->ip_type);

					flt_hdl = iter->flt_hdl;
					list_del(&iter->link);
					kfree(iter);

					IPADBG("Rule deleted successfully. Returning flt_hdl: %d\n", flt_hdl);
					goto unlock_and_exit;
				}
			}

			IPADBG("No matching rule found in filter list for intf_idx %d\n", intf_idx);
			break;
		}
	}

	IPADBG("Interface index %d not found in interface list\n", intf_idx);

unlock_and_exit:
	mutex_unlock(&ipa3_ctx->lock);
	IPADBG("Delete Entry Exit\n");
	return flt_hdl;
}

/**
 * ipa3_get_ep_for_intf() - atomically look up the RX endpoint for an interface
 * @intf_idx: interface index (intf_idx field of struct ipa3_intf)
 *
 * Holds ipa3_ctx->lock for the full duration to avoid a TOCTOU race between
 * the existence check and the rx-props read.
 *
 * Returns: IPA endpoint index on success, or -1 if the interface is not found
 * or has no RX properties.
 */
int ipa3_get_ep_for_intf(int intf_idx)
{
	struct ipa3_intf *entry;
	int ep = -1;

	mutex_lock(&ipa3_ctx->lock);
	list_for_each_entry(entry, &ipa3_ctx->intf_list, link) {
		if (entry->intf_idx == intf_idx) {
			if (entry->num_rx_props > 0)
				ep = ipa_get_ep_mapping(entry->rx[0].src_pipe);
			break;
		}
	}
	mutex_unlock(&ipa3_ctx->lock);

	return ep;
}
