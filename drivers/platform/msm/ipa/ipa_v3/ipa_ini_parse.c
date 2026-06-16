// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/firmware.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include "ipa_ini_parse.h"
#include "ipa_i.h"


#define CONVERT_TO_INT(str , val) kstrtoint(str, 10, &val)
#define CONVERT_TO_UINT(str , val) kstrtouint(str, 10, &val)
#define CONVERT_TO_BOOL(str, bool_val) kstrtobool(str, &bool_val)

static int ipa_ini_read_file(const char *path, struct device *device,
				char **out_buf);
static int ipa_ini_read_values(char **head, char **read_key,
				char **read_value, bool *section_item);
static int ipa_param_handler(const char *key_store,
				const char * value_store,
				struct ipa3_plat_drv_res *drv_res);
static int ipa_ini_parse( const char *path, struct device *device,
				struct ipa3_plat_drv_res *drv_res);


static int ipa_ini_read_file(const char *path, struct device *device,
				char **out_buf)
{
	int errno;
	const struct firmware *fw;
	char *buf;
	*out_buf = NULL;

	// Validate path to prevent directory traversal
	if (strstr(path, "..") || path[0] == '/') {
		IPAERR("Invalid file path: %s", path);
		return -EINVAL;
	}

	errno = request_firmware(&fw, path, device);
	if (errno) {
		IPAERR("Failed to read file: %s errno: %d", path, errno);
		return errno;
	}

	// Enforce maximum file size
	if (fw->size > MAX_INI_FILE_SIZE) {
		IPAERR("INI file too large: %zu bytes", fw->size);
		release_firmware(fw);
		return -EFBIG;
	}
	/* +1 size ensures null-termination */
	buf = kzalloc((fw->size + 1),GFP_KERNEL);
	if (!buf) {
		release_firmware(fw);
		return -ENOMEM;
	}

	memcpy(buf, fw->data, fw->size);
	buf[fw->size] = '\0';
	release_firmware(fw);
	*out_buf = buf;

	return 0;
}

static int ipa_ini_read_values(char **head, char **read_key,
				char **read_value, bool *section_item)
{
	char *cursor = *head;

	/* foreach line */
	while (*cursor != '\0') {
		char *key = cursor;
		char *value = NULL;
		bool comment = false;
		bool eol = false;

		/*
		 * Looking for end of the line, while checking for
		 * value ('=') and comment ('#') indicators
		 */
		while (!eol) {
			switch (*cursor) {
			case '\r':
			case '\n':
				*cursor = '\0';
				cursor++;
				eol = true;
				break;
			case '\0':
				eol = true;
				break;

			case '=':
				/*
				 * key is before '=' and value is after
				 * '=' so added null terminator to get
				 * key then value will be getting after
				 * getting '\n' or '\0'.
				 */
				if (!value && !comment) {
					value = cursor + 1;
					*cursor = '\0';
				}

				cursor++;
				break;

			case '#':
				/*
				 * Added null terminator to not process
				 * comments
				 */
				comment = true;
				*cursor = '\0';
				cursor++;
				break;
			default:
				cursor++;
				break;
			}
		}

		if (key)
			key = strim(key);
		if (value)
			value = strim(value);
		/*
		 * Ignoring comments, a valid ini line contains one of:
		 *	1) some 'key=value' config item
		 *	2) section header
		 *	3) a line containing whitespace
		 */
		if (value) {
			*read_key = key;
			*read_value = value;
			*section_item = 0;
			*head = cursor;
			IPADBG("key '%s' and value '%s'\n", key, value);
			return 0;
		} else if (key[0] == '[') {
			int len = strlen(key);

			if (key[len - 1] != ']') {
				IPAERR("Invalid *.ini syntax '%s'", key);
				return -EINVAL;
			} else {
				key[len - 1] = '\0';
				*read_key = key + 1;
				*section_item = 1;
				*head = cursor;
				return 0;
			}
		} else if (key[0] != '\0') {
			IPAERR("Invalid *.ini syntax '%s'", key);
			return -EINVAL;
		}

		/* skip remaining EoL characters */
		while (*cursor == '\n' || *cursor == '\r')
			cursor++;
	}

	return -EINVAL;
}

static int ipa_param_handler(const char *key_store,
				const char * value_store,
				struct ipa3_plat_drv_res *drv_res)
{
	int status = 0;
	int param = 0;
	u32 value;
	bool config;

	for (param = 0; param < IPA_PARAM_MAX; param++)
	{
		config = 0;
		value = 0;
		if(strcmp(ipa_ini_param[param], key_store) == 0)
		{
			switch (param) {
				case IPA_ETH_EMAC_VLAN_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("config is eth vlan '%s' is %d", value_store, config);
						drv_res->use_vlan_eth_emac_config = config;
					} else {
						 IPAERR("Error in converting '%s' value\n",value_store);
					}
					break;
				case IPA_ETH0_VLAN_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("config is eth0 vlan '%s' is %d", value_store, config);
						drv_res->use_vlan_eth0_config = config;
					} else {
						IPAERR("Error in converting '%s' value\n",value_store);
					}
					break;
				case IPA_ETH1_VLAN_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("config is eth1 vlan '%s' is %d", value_store, config);
						drv_res->use_vlan_eth1_config = config;
					} else {
						IPAERR("Error in converting '%s' value\n",value_store);
					}
					break;
				case IPA_RNDIS_VLAN_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("config is rndis vlan '%s' is %d", value_store, config);
						drv_res->use_vlan_rndis_config = config;
					} else {
						IPAERR("Error in converting '%s' value\n",value_store);
					}
					break;
				case IPA_ECM_VLAN_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("config is ecm vlan '%s' is %d", value_store, config);
						drv_res->use_vlan_ecm_config = config;
					} else {
						IPAERR("Error in converting '%s' value\n",value_store);
					}
					break;
				case IPA_MHI_ETH_VLAN_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("config is mhi_eth vlan '%s' is %d", value_store, config);
						drv_res->use_vlan_mhi_eth_config = config;
					} else {
						IPAERR("Error in converting '%s' value\n",value_store);
					}
					break;
				case IPA_USE_MHI_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("config use mhi config '%s' is %d", value_store, config);
						drv_res->use_mhi_config = config;
					} else {
						IPAERR("Error in converting '%s' value\n",value_store);
					}
					break;
				case IPA_USE_MHI_ETH_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("config use mhi eth config '%s' is %d", value_store, config);
						drv_res->use_mhi_eth_config = config;
					} else {
						IPAERR("Error in converting '%s' value\n",value_store);
					}
					break;
				case IPA_USE_RDKB_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("config use rdkb config '%s' is %d", value_store, config);
						drv_res->use_rdkb_config = config;
					} else {
						IPAERR("Error in converting '%s' value\n",value_store);
					}
					break;
				case IPA_USE_TSN_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("config use tsn config '%s' is %d", value_store, config);
						drv_res->use_tsn_config = config;
					} else {
						IPAERR("Error in converting '%s' value\n",value_store);
					}
					break;
				case IPA_USE_EZMESH_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("config use ezmesh config '%s' is %d", value_store, config);
						drv_res->use_ezmesh_config = config;
					} else {
						IPAERR("Error in converting '%s' value\n",value_store);
					}
					break;
				case IPA_USE_ETH_QOS_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("config use ethqos config '%s' is %d", value_store, config);
						drv_res->use_eth_qos_config = config;
					} else {
						IPAERR("Error in converting '%s' value\n",value_store);
					}
					break;
				case IPA_USE_IPSEC_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("config ip sec config '%s' is %d", value_store, config);
						drv_res->use_ipsec_config = config;
					} else {
						IPAERR("Error in converting '%s' value\n",value_store);
					}
					break;
				case IPA_NAT_STATS_MODE:
					status = CONVERT_TO_BOOL(value_store, config);
					if (!status)
					{
						IPADBG("nat_stats_mode '%s' is %d", value_store, config);
						drv_res->nat_stats_mode = config;
					}
					else
					{
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_MAX_IPV4_STATS_ACCEL_CONNECTIONS:
					status = CONVERT_TO_UINT(value_store, value);
					if(!status)
					{
						IPADBG("max v4 stats accel conn '%s' is %d", value_store, value);
						drv_res->max_ipv4_stats_accel_conn = value;
					} else {
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_MAX_IPV6_STATS_ACCEL_CONNECTIONS:
					status = CONVERT_TO_UINT(value_store, value);
					if(!status)
					{
						IPADBG("max v6 stats accel conn '%s' is %d", value_store, value);
						drv_res->max_ipv6_stats_accel_conn = value;
					} else {
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_DISABLE_PER_FLOW_STATS:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("Disable per flow stats: '%s' is %d", value_store, config);
						drv_res->ipa_disable_per_flow_stats = config;
					} else {
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_WDI3_2G_HOLB_TIMEOUT:
					status = CONVERT_TO_UINT(value_store, value);
					if(!status)
					{
						IPADBG("wdi3 2g holb timeout '%s' is %d", value_store, value);
						drv_res->ipa_wdi3_2g_holb_timeout = value;
					} else {
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_WDI3_5G_HOLB_TIMEOUT:
					status = CONVERT_TO_UINT(value_store, value);
					if(!status)
					{
						IPADBG("wdi3 5g holb timeout '%s' is %d", value_store, value);
						drv_res->ipa_wdi3_5g_holb_timeout = value;
					} else {
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_TX_WRAPPER_CACHE_MAX_SIZE:
					status = CONVERT_TO_UINT(value_store, value);
					if(!status)
					{
						IPADBG("tx wrapper cache max size '%s' is %d", value_store, value);
						drv_res->tx_wrapper_cache_max_size = value;
					} else {
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_FILTER_START_ID:
					status = CONVERT_TO_UINT(value_store, value);
					if(!status)
					{
						IPADBG("filter start id '%s' is %d", value_store, value);
						drv_res->filter_start_id = value;
					} else {
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_WAN_RX_RING_SIZE:
					status = CONVERT_TO_UINT(value_store, value);
					if(!status)
					{
						IPADBG("wan rx ring size '%s' is %d", value_store, value);
						drv_res->wan_rx_ring_size = value;
					} else {
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_LAN_RX_RING_SIZE:
					status = CONVERT_TO_UINT(value_store, value);
					if(!status)
					{
						IPADBG("lan rx ring size '%s' is %d", value_store, value);
						drv_res->lan_rx_ring_size = value;
					} else {
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_GEN_RX_CMN_PAGE_POOL_SZ_FACTOR:
					status = CONVERT_TO_UINT(value_store, value);
					if(!status)
					{
						IPADBG("ipa gen rx cmn page pool sz factor '%s' is %d", value_store, value);
						drv_res->ipa_gen_rx_cmn_page_pool_sz_factor = value;
					} else {
						 IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_GEN_RX_CMN_TEMP_POOL_SZ_FACTOR:
					status = CONVERT_TO_UINT(value_store, value);
					if(!status)
					{
						IPADBG("ipa_gen_rx_cmn_temp_pool_sz_factor '%s' is %d", value_store, value);
						drv_res->ipa_gen_rx_cmn_temp_pool_sz_factor = value;
					} else {
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_WAN_SKB_PAGE:
					status = CONVERT_TO_BOOL(value_store, config);
					if(!status)
					{
						IPADBG("ipa wan skb page '%s' is %d", value_store, config);
						drv_res->ipa_wan_skb_page = config;
					} else {
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_NAT_STATS_MAX_COUNTERS_V4:

					status = CONVERT_TO_UINT(value_store, value);
					if (!status)
					{
						IPADBG("nat_stats_max_counters_v4 '%s' is %u", value_store, value);
						drv_res->nat_stats_max_counters_v4 = (u16)value;
					}
					else
					{
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_NAT_STATS_MAX_COUNTERS_V6:
					status = CONVERT_TO_UINT(value_store, value);
					if (!status)
					{
						IPADBG("nat_stats_max_counters_v6 '%s' is %u", value_store, value);
						drv_res->nat_stats_max_counters_v6 = (u16)value;
					}
					else
					{
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				case IPA_USE_IPV6_NAT_CONFIG:
					status = CONVERT_TO_BOOL(value_store, config);
					if (!status)
					{
						IPADBG("config use ipv6 nat '%s' is %d", value_store, config);
						drv_res->use_ipv6_nat_config = config;
					}
					else
					{
						IPAERR("Error in converting '%s' value\n", value_store);
					}
					break;
				}
				return status;
		}
	}
	IPAERR("Invalid key '%s' found\n", key_store);
	return -EINVAL;
}

static int ipa_ini_parse( const char *path, struct device *device,
				struct ipa3_plat_drv_res *drv_res)
{
	int status = 0;
	char *read_key;
	char * read_value;
	int item_read_count = 0;
	char * fbuf = NULL;
	char *cursor;
	bool section_item;

	status = ipa_ini_read_file(path, device, &fbuf);
	if(status)
	{
		IPAERR("Failed to read file\n");
		return status;
	}
	cursor = fbuf;

	while (ipa_ini_read_values(&cursor, &read_key, &read_value,
						 &section_item) == 0) {
			if (!section_item) {
				status = ipa_param_handler(read_key, read_value, drv_res);
				if (status) {
					IPAERR("Failed to handle param %s: %d\n", read_key, status);
					break;
				}
				item_read_count++;
			} else {
				/* section part implemetation if needed */
			}
	}

	/* free allocated memory */
	kfree(fbuf);

	if (status) {
		IPAERR("INI file parsing failed with error %d\n", status);
		return status;
	}
	if (item_read_count == 0) {
		IPAERR("No valid items found in INI file\n");
		return -EINVAL;
	}

	IPADBG("INI file parse successful, %d items read\n", item_read_count);
	return 0;
}

int ipa_init_params_from_ini(struct device *device,
				struct ipa3_plat_drv_res *drv_res)
{
	int status = 0;
	char * path = INI_FILE_PATH;

	status = ipa_ini_parse(path, device, drv_res);
	if(status)
	{
		IPAERR("Error in parsing IPA ini file %s", path);
	} else {
		IPADBG("IPA INI file is parsed\n");
	}
	return status;
}
