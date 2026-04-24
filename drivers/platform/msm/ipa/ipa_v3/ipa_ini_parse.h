// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include "ipa_i.h"

#define IPA_PARAM_STR_LENGTH 40
#define INI_FILE_PATH "ipa_be/ipa_ini_file.ini"
#define MAX_INI_FILE_SIZE (64 * 1024)  // 64KB limit

enum ipa_ini_file_param {
		IPA_ETH_EMAC_VLAN_CONFIG,
		IPA_ETH0_VLAN_CONFIG,
		IPA_ETH1_VLAN_CONFIG,
		IPA_RNDIS_VLAN_CONFIG,
		IPA_ECM_VLAN_CONFIG,
		IPA_MHI_ETH_VLAN_CONFIG,
		IPA_USE_MHI_CONFIG,
		IPA_USE_MHI_ETH_CONFIG,
		IPA_USE_RDKB_CONFIG,
		IPA_USE_TSN_CONFIG,
		IPA_USE_EZMESH_CONFIG,
		IPA_USE_ETH_QOS_CONFIG,
		IPA_USE_IPSEC_CONFIG,
		IPA_USE_IPV6_NAT_CONFIG,
		IPA_MAX_IPV4_STATS_ACCEL_CONNECTIONS,
		IPA_MAX_IPV6_STATS_ACCEL_CONNECTIONS,
		IPA_DISABLE_PER_FLOW_STATS,
		IPA_WDI3_2G_HOLB_TIMEOUT,
		IPA_WDI3_5G_HOLB_TIMEOUT,
		IPA_TX_WRAPPER_CACHE_MAX_SIZE,
		IPA_WAN_SKB_PAGE,
		IPA_FILTER_START_ID,
		IPA_WAN_RX_RING_SIZE,
		IPA_LAN_RX_RING_SIZE,
		IPA_GEN_RX_CMN_PAGE_POOL_SZ_FACTOR,
		IPA_GEN_RX_CMN_TEMP_POOL_SZ_FACTOR,
		IPA_PARAM_MAX,
	};

static char ipa_ini_param[IPA_PARAM_MAX][IPA_PARAM_STR_LENGTH] = {
		"use_vlan_eth_emac_config",
		"use_vlan_eth0_config",
		"use_vlan_eth1_config",
		"use_vlan_rndis_config",
		"use_vlan_ecm_config",
		"use_vlan_mhi_eth_config",
		"use_mhi_config",
		"use_mhi_eth_config",
		"use_rdkb_config",
		"use_tsn_config",
		"use_ezmesh_config",
		"use_eth_qos_config",
		"use_ipsec_config",
		"use_ipv6_nat_config",
		"max_ipv4_stats_accel_conn",
		"max_ipv6_stats_accel_conn",
		"ipa_disable_per_flow_stats",
		"ipa_wdi3_2g_holb_timeout",
		"ipa_wdi3_5g_holb_timeout",
		"tx_wrapper_cache_max_size",
		"ipa_wan_skb_page",
		"filter_start_id",
		"wan_rx_ring_size",
		"lan_rx_ring_size",
		"ipa_gen_rx_cmn_page_pool_sz_factor",
		"ipa_gen_rx_cmn_temp_pool_sz_factor",
	};

int ipa_init_params_from_ini(struct device *device,
				struct ipa3_plat_drv_res *ipa_drv_res);

