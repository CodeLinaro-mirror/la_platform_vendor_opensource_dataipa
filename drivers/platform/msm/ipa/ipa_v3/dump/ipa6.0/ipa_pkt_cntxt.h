// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */
#if !defined(_IPA_PKT_CNTXT_H_)
#define _IPA_PKT_CNTXT_H_

#define IPA_HW_PKT_CTNTX_MAX        0x10

/*
 * Packet Context States
 */
enum ipa_hw_pkt_cntxt_state_e {
	IPA_HW_PKT_CNTXT_STATE_HFETCHER_INIT = 1,
	IPA_HW_PKT_CNTXT_STATE_HFETCHER_DMAR = 2,
	IPA_HW_PKT_CNTXT_STATE_H_DCPH = 3,
	IPA_HW_PKT_CNTXT_STATE_MULTI_DRBIP = 4,
	IPA_HW_PKT_CNTXT_STATE_PKT_PARSER = 5,
	IPA_HW_PKT_CNTXT_STATE_FILTER_NAT = 6,
	IPA_HW_PKT_CNTXT_STATE_ROUTER = 7,
	IPA_HW_PKT_CNTXT_STATE_HDRI = 8,
	IPA_HW_PKT_CNTXT_STATE_UCP = 9,
	IPA_HW_PKT_CNTXT_STATE_COAL_MASTER = 10,
	IPA_HW_PKT_CNTXT_STATE_ENQUEUER = 11,
	IPA_HW_PKT_CNTXT_STATE_DFETCHER = 12,
	IPA_HW_PKT_CNTXT_STATE_D_DCPH = 13,
	IPA_HW_PKT_CNTXT_STATE_DISPATCHER = 14,
	IPA_HW_PKT_CNTXT_STATE_TX = 15,
	IPA_HW_PKT_CNTXT_STATE_TX_ZLT = 16,
	IPA_HW_PKT_CNTXT_STATE_DFETCHER_DMAR = 17,
	IPA_HW_PKT_CNTXT_STATE_D_DCPH_2 = 19,
	IPA_HW_PKT_CNTXT_STATE_TX_RSRCREL = 20,
	IPA_HW_PKT_CNTXT_STATE_ENCAPS = 21,
	IPA_HW_PKT_CNTXT_STATE_DECAPS = 22,
};

/*
 * Packet Context fields as received from VI/Design
 */
struct ipa_pkt_ctntx_s {
	u64	opcode                           : 8; /* Word 0 Bits 0-7 */
	u64	state                            : 5; /* Word 0 Bits 8-12 */
	u64	stats_disable                    : 1; /* Word 0 Bit 13 */
	u64	exc_ucp                          : 1; /* Word 0 Bit 14 */
	u64	tx_pkt_dma_done                  : 1; /* Word 0 Bit 15 */
	u64	exc_deagg                        : 1; /* Word 0 Bit 16 */
	u64	exc_pkt_version                  : 1; /* Word 0 Bit 17 */
	u64	exc_pkt_len                      : 1; /* Word 0 Bit 18 */
	u64	exc_threshold                    : 1; /* Word 0 Bit 19 */
	u64	exc_sw                           : 1; /* Word 0 Bit 20 */
	u64	exc_nat                          : 1; /* Word 0 Bit 21 */
	u64	exc_frag_miss                    : 1; /* Word 0 Bit 22 */
	u64	filter_bypass                    : 1; /* Word 0 Bit 23 */
	u64	router_bypass                    : 1; /* Word 0 Bit 24 */
	u64	nat_bypass                       : 1; /* Word 0 Bit 25 */
	u64	hdri_bypass                      : 1; /* Word 0 Bit 26 */
	u64	dcph_bypass                      : 1; /* Word 0 Bit 27 */
	u64	security_credentials_select      : 1; /* Word 0 Bit 28 */
	u64	dcph_valid                       : 1; /* Word 0 Bit 29 */
	u64	round_bypass                     : 1; /* Word 0 Bit 30 */
	u64	next_round_enable                : 1; /* Word 0 Bit 31 */
	u64	bearer_valid                     : 1; /* Word 0 Bit 32 */
	u64	ucp_on                           : 1; /* Word 0 Bit 33 */
	u64	replication                      : 1; /* Word 0 Bit 34 */
	u64	src_status_en                    : 1; /* Word 0 Bit 35 */
	u64	dest_status_en                   : 1; /* Word 0 Bit 36 */
	u64	frag_status_en                   : 1; /* Word 0 Bit 37 */
	u64	eot_dest                         : 1; /* Word 0 Bit 38 */
	u64	eot_notif                        : 1; /* Word 0 Bit 39 */
	u64	prev_eot_dest                    : 1; /* Word 0 Bit 40 */
	u64	l2_len                           : 9; /* Word 0 Bits 41-49 */
	u64	dispatcher_pass                  : 1; /* Word 0 Bit 50 */
	u64	ucp_on_for_rts                   : 1; /* Word 0 Bit 51 */
	u64	exc_hdri                         : 1; /* Word 0 Bit 52 */
	u64	pkt_parser_bypass                : 1; /* Word 0 Bit 53 */
	u64	exc_pipe                         : 1; /* Word 0 Bit 54 */
	u64	nat_in_hdrs                      : 1; /* Word 0 Bit 55 */
	u64	rx_flags                         : 8; /* Word 0 Bits 56-63 */
	u64	rx_packet_length                 : 16; /* Word 1 Bits 0-15 */
	u64	revised_packet_length            : 16; /* Word 1 Bits 16-31 */
	u64	frag_en                          : 1; /* Word 1 Bit 32 */ 
	u64	frag_bypass                      : 1; /* Word 1 Bit 33 */
	u64	frag_process                     : 1; /* Word 1 Bit 34 */
	u64	tx_pkt_transferred               : 1; /* Word 1 Bit 35 */
	u64	filter_aggr_force_close          : 1; /* Word 1 Bit 36 */
	u64	router_aggr_force_close          : 1; /* Word 1 Bit 37 */
	u64	not_used_1                       : 2; /* Word 1 Bits 38-39 */
	u64	src_id                           : 8; /* Word 1 Bits 40-47 */
	u64	src_pipe                         : 8; /* Word 1 Bits 48-55 */
	u64	dest_pipe                        : 8; /* Word 1 Bits 56-63 */
	u64	ihl_offset                       : 6; /* Word 2 Bits 0-5 */
	u64	d_dcph_pass                      : 1; /* Word 2 Bit 6 */
	u64	not_used_2                       : 1; /* Word 2 Bit 7 */
	u64	protocol                         : 8; /* Word 2 Bits 8-15 */
	u64	tos                              : 8; /* Word 2 Bits 16-23 */
	u64	id                               : 16; /* Word 2 Bits 24-39 */
	u64	v6_reserved                      : 4; /* Word 2 Bits 40-43 */
	u64	ff                               : 1; /* Word 2 Bit 44 */
	u64	mf                               : 1; /* Word 2 Bit 45 */
	u64	pkt_is_frag                      : 1; /* Word 2 Bit 46 */
	u64	cs_disable_trailer_valid_bit     : 1; /* Word 2 Bit 47 */
	u64	exc_checksum_offload             : 1; /* Word 2 Bit 48 */
	u64	exc_ipv4_checksum                : 1; /* Word 2 Bit 49 */
	u64	trnseq_0                         : 4; /* Word 2 Bits 50-53 */
	u64	trnseq_1                         : 4; /* Word 2 Bits 54-57 */
	u64	trnseq_2                         : 4; /* Word 2 Bits 58-61 */
	u64	not_used_3                       : 2; /* Word 2 Bits 62-63 */
	u64	trnseq_3                         : 4; /* Word 3 Bits 0-3 */
	u64	trnseq_4                         : 4; /* Word 3 Bits 4-7 */
	u64	trnseq_5                         : 4; /* Word 3 Bits 8-11 */
	u64	trnseq_ex_length                 : 6; /* Word 3 Bits 12-17 */
	u64	trnseq_4_length                  : 10; /* Word 3 Bits 18-27 */
	u64	trnseq_4_offset                  : 9; /* Word 3 Bits 28-36 */
	u64	trnseq_5_length                  : 10; /* Word 3 Bits 37-46 */
	u64	trnseq_5_offset                  : 9; /* Word 3 Bits 47-55 */
	u64	dps_tx_pop_cnt                   : 2; /* Word 3 Bits 56-57 */
	u64	dps_tx_push_cnt                  : 2; /* Word 3 Bits 58-59 */
	u64	vol_ic_dcph_cfg                  : 1; /* Word 3 Bit 60 */
	u64	vol_ic_tag_stts                  : 1; /* Word 3 Bit 61 */
	u64	vol_ic_pkt_init_ex              : 1; /* Word 3 Bit 62 */
	u64	vol_ic_pkt_init                  : 1; /* Word 3 Bit 63 */
	u64	trnseq_0_preucp                  : 1; /* Word 4 Bit 0 */
	u64	dest_pipe_overridden_ucp         : 1; /* Word 4 Bit 1 */
	u64	force_to_default                 : 1; /* Word 4 Bit 2 */
	u64	close_vp_before                  : 1; /* Word 4 Bit 3 */
	u64	vol_ic_eob_bubble                : 1; /* Word 4 Bit 4 */
	u64	not_used_4                       : 5; /* Word 4 Bits 5-9 */
	u64	maci_bytes_in_trnseq             : 1; /* Word 4 Bit 10 */
	u64	drop_drbip                       : 1; /* Word 4 Bit 11 */
	u64	drop_decaps_dummy_pkt            : 1; /* Word 4 Bit 12 */
	u64	exc_decaps                       : 1; /* Word 4 Bit 13 */
	u64	exc_drbip                        : 1; /* Word 4 Bit 14 */
	u64	drbip_valid                      : 1; /* Word 4 Bit 15 */
	u64	tx_pkt_suspended                 : 1; /* Word 4 Bit 16 */
	u64	rb                               : 1; /* Word 4 Bit 17 */
	u64	packet_ethernet_parsing_done     : 1; /* Word 4 Bit 18 */
	u64	exc_rqos                         : 1; /* Word 4 Bit 19 */
	u64	tcp_win_size                     : 16; /* Word 4 Bits 20-35 */
	u64	trnseq_0_length                  : 10; /* Word 4 Bits 36-45 */
	u64	trnseq_0_offset                  : 9; /* Word 4 Bits 46-54 */
	u64	data_cmdq_ptr                    : 8; /* Word 4 Bits 55-62 */
	u64	not_used_5                       : 1; /* Word 4 Bit 63 */
	u64	trnseq_1_length                  : 10; /* Word 5 Bits 0-9 */
	u64	trnseq_1_offset                  : 9; /* Word 5 Bits 10-18 */
	u64	trnseq_2_length                  : 10; /* Word 5 Bits 19-28 */
	u64	trnseq_2_offset                  : 9; /* Word 5 Bits 29-37 */
	u64	trnseq_3_length                  : 10; /* Word 5 Bits 38-47 */
	u64	trnseq_3_offset                  : 9; /* Word 5 Bits 48-56 */
	u64	exc_ttl                          : 1; /* Word 5 Bit 57 */
	u64	tsp_drop                         : 1; /* Word 5 Bit 58 */
	u64	exc_exceed_mtu                   : 1; /* Word 5 Bit 59 */
	u64	not_used_6                       : 4; /* Word 5 Bit 60-63 */
	u64	dmar_valid_length                : 16; /* Word 6 Bits 0-15 */
	u64	dcph_valid_length                : 16; /* Word 6 Bits 16-31 */
	u64	frag_pipe                        : 8; /* Word 6 Bits 32-39 */
	u64	notif_pipe                       : 8; /* Word 6 Bits 40-47 */
	u64	coal_vp_valid                    : 1; /* Word 6 Bit 48 */
	u64	coal_vp_opcode_valid             : 1; /* Word 6 Bit 49 */
	u64	coal_vp_open                     : 1; /* Word 6 Bit 50 */
	u64	coal_vp_eot                      : 1; /* Word 6 Bit 51 */
	u64	not_used_7                       : 4; /* Word 6 Bits 52-55 */
	u64	vp_index                         : 8; /* Word 6 Bits 56-63 */
	u64	frag_status_opcode               : 8; /* Word 7 Bits 0-7 */
	u64	frag_rule                        : 4; /* Word 7 Bits 8-11 */
	u64	not_used_8                       : 1; /* Word 7 Bit 12 */
	u64	frag_exception                   : 1; /* Word 7 Bit 13 */
	u64	frag_table                       : 2; /* Word 7 Bits 14-15 */
	u64	not_used_9                       : 16; /* Word 7 Bits 16-31 */
	u64	frag_src_ip_address              : 32; /* Word 7 Bits 32-63 */
	u64	frag_dst_ip_address              : 32; /* Word 8 Bits 0-31 */
	u64	frag_fr_ret                      : 1; /* Word 8 Bit 32 */
	u64	frag_fnr_aggr_fc                 : 1; /* Word 8 Bit 33 */
	u64	frag_ttl_update                  : 1; /* Word 8 Bit 34 */
	u64	not_used_10                      : 3; /* Word 8 Bits 35-37 */
	u64	frag_nat_type                    : 2; /* Word 8 Bits 38-39 */
	u64	frag_protocol                    : 8; /* Word 8 Bits 40-47 */
	u64	frag_id                          : 16; /* Word 8 Bits 48-63 */
	u64	frag_nat_ip_address              : 32; /* Word 9 Bits 0-31 */
	u64	frag_header_offset               : 10; /* Word 9 Bits 32-41 */
	u64	frag_ingress_tc                  : 6; /* Word 9 Bits 42-47 */
	u64	frag_nat_ip_cs_diff              : 16; /* Word 9 Bits 48-63 */
	u64	metadata_pre_nat                 : 32; /* Word 10 Bits 0-31 */
	u64	frag_router_stats_index          : 8; /* Word 10 Bits 32-39 */
	u64	frag_filter_stats_index          : 8; /* Word 10 Bits 40-47 */
	u64	frag_dest_pipe                   : 8; /* Word 10 Bits 48-55 */
	u64	frag_egress_tc                   : 6; /* Word 10 Bits 56-61 */
	u64	frag_ingress_policer_dis         : 1; /* Word 10 Bit 62 */
	u64	frag_hdr_l                       : 1; /* Word 10 Bit 63 */
	u64	packet_status_op_code            : 8; /* Word 11 Bits 0-7 */
	u64	packet_status_exception_part1_or_drop_type : 4; /* Word 11 Bits 8-11 */
	u64	packet_status_exception_part2_or_drop_reason : 4; /* Word 11 Bits 12-15 */
	u64	not_used_11                      : 1; /* Word 11 Bit 16 */
	u64	filter_process                   : 1; /* Word 11 Bit 17 */
	u64	nat_process                      : 1; /* Word 11 Bit 18 */
	u64	router_process                   : 1; /* Word 11 Bit 19 */
	u64	not_used_12                      : 4; /* Word 11 Bits 20-23 */
	u64	packet_status_checksum_process   : 1; /* Word 11 Bit 24 */
	u64	packet_status_aggr_en            : 1; /* Word 11 Bit 25 */
	u64	packet_status_open_frame         : 1; /* Word 11 Bit 26 */
	u64	packet_status_deaggr_en          : 1; /* Word 11 Bit 27 */
	u64	packet_status_deaggr_first       : 1; /* Word 11 Bit 28 */
	u64	packet_status_src_eot            : 1; /* Word 11 Bit 29 */
	u64	packet_status_rqos_nas_valid     : 1; /* Word 11 Bit 30 */
	u64	packet_status_rqos_as_valid      : 1; /* Word 11 Bit 31 */
	u64	not_used_13                      : 24; /* Word 11 Bits 32-55 */
	u64	packet_status_pure_ack           : 1; /* Word 11 Bit 56 */
	u64	packet_status_syn                : 1; /* Word 11 Bits 57 */
	u64	packet_status_tcp_fin_rst        : 1; /* Word 11 Bits 58 */
	u64	rt_l                             : 1; /* Word 11 Bits 59 */
	u64	rt_h                             : 1; /* Word 11 Bits 60 */
	u64	packet_status_protocol_encoding  : 3; /* Word 11 Bits 61-63 */
	u64	metadata                         : 32; /* Word 12 Bit 0-31 */
	u64	fr_l                             : 1; /* Word 12 Bit 32 */
	u64	fl_h                             : 1; /* Word 12 Bit 33 */
	u64	fr_g                             : 1; /* Word 12 Bit 34 */
	u64	fr_ret                           : 1; /* Word 12 Bit 35 */
	u64	fr_rule_id                       : 10; /* Word 12 Bits 36-45 */
	u64	rt_table_index                   : 8; /* Word 12 Bits 46-53 */
	u64	rt_rule_id                       : 10; /* Word 12 Bits 54-63 */
	u64	nat_hit                          : 1; /* Word 13 Bit 0 */
	u64	nat_table_index                  : 13; /* Word 13 Bits 1-13 */
	u64	nat_type                         : 2; /* Word 13 Bits 14-15 */
	u64	tag_info_part1                   : 36; /* Word 13 Bit 16-51 */
	u64	tag_info_part2_or_egress_tc      : 4; /* Word 13 Bit 52-55 */
	u64	tag_info_part3_or_egress_tc_or_filter_table : 2; /* Word 13 Bits 56-57 */
	u64	tag_info_part4_or_ingress_tc_or_filter_table : 6; /* Word 13 Bits 58-63 */
	u64	not_used_14                      : 32; /* Word 14 Bits 0-31 */
	u64	hdr_l                            : 1; /* Word 14 Bit 32 */
	u64	header_offset                    : 10; /* Word 14 Bits 33-42 */
	u64	packet_status_frag_hit           : 1; /* Word 14 Bit 43 */
	u64	packet_status_frag_rule          : 4; /* Word 14 Bit 44-47 */
	u64	not_used_15                      : 11; /* Word 14 Bits 48-58 */
	u64	tmi                              : 2; /* Word 14 Bits 59-60 */
	u64	nat_exc_suppress                 : 1; /* Word 14 Bit 61 */
	u64	ttl_update                       : 1; /* Word 14 Bit 62 */
	u64	not_used_16                      : 1; /* Word 14 Bit 63 */
	u64	error_qmap_pad                   : 6; /* Word 15 Bits 0-5 */
	u64	error_qmap_next_hdr_0            : 1; /* Word 15 Bit 6 */
	u64	error_qmap_cd                    : 1; /* Word 15 Bit 7 */
	u64	error_qmap_mux_id                : 8; /* Word 15 Bits 8-15 */
	u64	error_qmap_packet_len_with_padding : 16; /* Word 15 Bits 16-31 */
	u64	error_qmap_next_hdr_1            : 1; /* Word 15 Bit 32 */
	u64	error_qmap_header_type           : 7; /* Word 15 Bits 33-39 */
	u64	error_qmap_reserved_0            : 7; /* Word 15 Bits 40-46 */
	u64	error_qmap_csum_valid            : 1; /* Word 15 Bit 47 */
	u64	error_qmap_error_type            : 8; /* Word 15 Bits 48-55 */
	u64	error_qmap_error_code            : 8; /* Word 15 Bits 56-63 */
	u64	ipsec_sa_index                   : 8; /* Word 16 Bits 0-7 */
	u64	error_qmap_reserved_1            : 24; /* Word 16 Bits 8-31 */
	u64	ucp_cmd_id                       : 16; /* Word 16 Bits 32-47 */
	u64	l4_payload_checksum              : 16; /* Word 16 Bits 48-63 */
	u64	ucp_cmd_params                   : 32; /* Word 17 Bits 0-31 */
	u64	bearer_cfg_count                 : 32; /* Word 17 Bits 32-63 */
	u64	mbim_aggr_sid                    : 32; /* Word 18 Bits 0-31 */	
	u64	l4_pseudo_hdr_checksum           : 16; /* Word 18 Bits 32-47 */
	u64	ipv4_cs_without_total_len        : 16; /* Word 18 Bits 48-63 */
	u64	padding_bytes_cnt                : 16; /* Word 19 Bits 0-15 */
	u64	ulso_mss                         : 16; /* Word 19 Bits 16-31 */
	u64	tcp_data_offset                  : 4; /* Word 19 Bits 32-35 */
	u64	original_hdr_size                : 9; /* Word 19 Bits 36-44 */
	u64	ulso_udp_checksum_zero           : 1; /* Word 19 Bit 45 */
	u64	fin                              : 1; /* Word 19 Bit 46 */
	u64	ipv4_vld_checksum                : 1; /* Word 19 Bit 47 */
	u64	ttl                              : 8; /* Word 19 Bits 48-55 */
	u64	router_stats_index               : 8; /* Word 19 Bits 56-63 */
	u64	filter_stats_index               : 8; /* Word 20 Bits 0-7 */
	u64	filter_action_params             : 5; /* Word 20 Bits 8-12 */
	u64	metadata_type                    : 3; /* Word 20 Bits 13-15 */
	u64	original_src_hdr_len             : 8; /* Word 20 Bits 16-23 */
	u64	egress_tc                        : 6; /* Word 20 Bits 24-29 */
	u64	ingress_tc                       : 6; /* Word 20 Bits 30-35 */
	u64	frag_hdr_offset                  : 9; /* Word 20 Bits 36-44 */
	u64	syn                              : 1; /* Word 20 Bit 45 */
	u64	urg                              : 1; /* Word 20 Bit 46 */
	u64	cwr                              : 1; /* Word 20 Bit 47 */
	u64	df                               : 1; /* Word 20 Bit 48 */
	u64	original_ip_version              : 2; /* Word 20 Bits 49-50 */
	u64	hdri_payload_length_includes_padding : 1; /* Word 20 Bit 51 */
	u64	hdri_pdding_or_total_length      : 1; /* Word 20 Bit 52 */
	u64	hdri_payload_len_valid           : 1; /* Word 20 Bit 53 */
	u64	hdri_padding_valid               : 1; /* Word 20 Bit 54 */
	u64	hdri_endianess                   : 1; /* Word 20 Bit 55 */
	u64	rt_match                         : 1; /* Word 20 Bit 56 */
	u64	filter_result_valid              : 1; /* Word 20 Bit 57 */
	u64	push                             : 1; /* Word 20 Bit 58 */
	u64	rst                              : 1; /* Word 20 Bit 59 */
	u64	pure_ack                         : 1; /* Word 20 Bit 60 */
	u64	ip_checksum_fix                  : 1; /* Word 20 Bit 61 */
	u64	tport_checksum_fix               : 1; /* Word 20 Bit 62 */
	u64	ack                              : 1; /* Word 20 Bit 63 */
	u64	frag_hit                         : 1; /* Word 21 Bit 0 */
	u64	bearer_context_index             : 8; /* Word 21 Bits 1-8 */
	u64	ulso_ipv4_id_mode                : 2; /* Word 21 Bits 9-10 */
	u64	ulso_frame_valid                 : 1; /* Word 21 Bit 11 */
	u64	inc_ipv4_id                      : 1; /* Word 21 Bit 12 */
	u64	filter_ttl_update                : 1; /* Word 21 Bit 13 */
	u64	router_ttl_update                : 1; /* Word 21 Bit 14 */
	u64	sdap_qfi                         : 6; /* Word 21 Bits 15-20 */
	u64	sdap_rqi                         : 1; /* Word 21 Bit 21 */
	u64	sdap_rdi                         : 1; /* Word 21 Bit 22 */
	u64	rqos_offload_valid               : 1; /* Word 21 Bit 23 */
	u64	ingress_policer_dis              : 1; /* Word 21 Bit 24 */
	u64	rqos_offload_enable              : 1; /* Word 21 Bit 25 */
	u64	snapshot_req                     : 1; /* Word 21 Bit 26 */
	u64	prod_dpl_dis                     : 1; /* Word 21 Bit 27 */
	u64	frag_hit_2nd                     : 1; /* Word 21 Bit 28 */
	u64	frag_filter_aggr_fc              : 1; /* Word 21 Bit 29 */
	u64	router_rule_table_hit            : 1; /* Word 21 Bit 30 */
	u64	filter_rule_table_hit            : 1; /* Word 21 Bit 31 */
	u64	hps_round_cnt                    : 2; /* Word 21 Bits 32-33 */
	u64	first_pkt_parser_done            : 1; /* Word 21 Bit 34 */
	u64	filter_result                    : 6; /* Word 21 Bits 35-40 */
	u64	maci_size                        : 2; /* Word 21 Bits 41-42 */
	u64	bearer_prcs_type                 : 1; /* Word 21 Bit 43 */
	u64	dcph_cfg_size                    : 16; /* Word 21 Bits 44-59 */
	u64	not_used_17                      : 4; /* Word 21 Bit 60-63 */
	u64	snapshot_payload_offset          : 9; /* Word 22 Bits 0-8 */
	u64	frag_router_aggr_fc              : 1; /* Word 22 Bit 9 */
	u64	qmap_cs_valid_bit                : 1; /* Word 22 Bit 10 */
	u64	bearer_id                        : 8; /* Word 22 Bits 11-18 */
	u64	nat_result_valid                 : 1; /* Word 22 Bit 19 */
	u64	nat_result                       : 6; /* Word 22 Bits 20-25 */
	u64	hdri_offset_padding_total_length : 8; /* Word 22 Bits 26-33 */
	u64	hdri_offset_payload_len          : 8; /* Word 22 Bit 34-41 */
	u64	hdri_dst_len                     : 8; /* Word 22 Bits 42-49 */
	u64	hdri_additional_const_length     : 8; /* Word 22 Bits 50-57 */
	u64	not_used_18                      : 6; /* Word 22 Bit 58-63 */
	u64	ipsec_icv_start_offset           : 16; /* Word 23 Bits 0-15 */
	u64	tsp                              : 1; /* Word 23 Bit 16 */
	u64	ipsec_enc_start_offset           : 8; /* Word 23 Bits 17-24 */
	u64	ipsec_auth_start_offset          : 8; /* Word 23 Bits 25-32 */
	u64	ipsec_iv_start_offset            : 8; /* Word 23 Bits 33-40 */
	u64	ipsec_outer_hdr_ecn              : 2; /* Word 23 Bits 41-42 */
	u64	ipsec_action                     : 2; /* Word 23 Bits 43-44 */
	u64	ipsec_post_decaps                : 1; /* Word 23 Bit 45 */
	u64	snapshot_bypass                  : 1; /* Word 23 Bit 46 */
	u64	ece                              : 1; /* Word 23 Bit 47 */
	u64	udp_with_zero_checksum           : 1; /* Word 23 Bit 48 */
	u64	snapshot_pad_length              : 8; /* Word 23 Bit 49-56 */
	u64	not_used_19                      : 7; /* Word 23 Bit 57-63 */
	u64	ipsec_seq_num_lsb                : 32; /* Word 24 Bits 0-31 */
	u64	ipsec_seq_num_msb                : 32; /* Word 24 Bits 32-63 */
	u64	filter_table_index               : 8; /* Word 25 Bits 0-7 */
	u64	snapshot_packet_len              : 16; /* Word 25 Bits 8-23 */
	u64	ipsec_next_hdr_val               : 8; /* Word 25 Bits 24-31 */
	u64	snapshot_ttl_updated             : 1; /* Word 25 Bit 32 */
	u64	snapshot_trnseq_lenght           : 10; /* Word 25 Bits 33-42 */
	u64	snapshot_l2_length               : 8; /* Word 25 Bits 43-50 */
	u64	snapshot_t_length                : 8; /* Word 25 Bits 51-58 */
	u64	ipsec_post_encaps                : 1; /* Word 25 Bit 59 */
	u64	snapshot_metadata_type           : 3; /* Word 25 Bits 60-62 */
	u64	not_used_20                      : 1; /* Word 25 Bit 63 */
} __packed;

#endif /* #if !defined(_IPA_PKT_CNTXT_H_) */
