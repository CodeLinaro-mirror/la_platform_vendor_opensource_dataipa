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
	};

int ipa_init_params_from_ini(struct device *device,
				struct ipa3_plat_drv_res *ipa_drv_res);







