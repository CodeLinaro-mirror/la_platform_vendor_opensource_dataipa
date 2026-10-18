// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include "ipahal.h"
#include "ipahal_hw_stats.h"
#include "ipahal_hw_stats_i.h"
#include "ipahal_i.h"

struct ipahal_hw_stats_obj {
	struct ipahal_stats_init_pyld *(*generate_init_pyld)(void *params,
		bool is_atomic_ctx);
	int (*get_offset)(void *params, struct ipahal_stats_offset *out);
	int (*parse_stats)(void *init_params, void *raw_stats,
		void *parsed_stats);
};

/* IPA 7.0 tethering CFG index is 1-based; index 0 is reserved. */
#define IPA_TETH_CFG_FIRST_INDEX 1

static int _count_ones(u32 number)
{
	int count = 0;

	while (number) {
		count++;
		number = number & (number - 1);
	}

	return count;
}

/*
 * ipahal_teth_pair_is_nonip() - true if the Non-IP bit is set for pair
 *	(prod, cons): the OR of the producer's and consumer's nonip_bitmask
 *	bits. Single source of truth for the mixed 24B/40B stride, used by the
 *	v7.0 generate/get_offset/parse paths so they agree on per-pair size.
 */
static bool ipahal_teth_pair_is_nonip(struct ipahal_stats_init_tethering *in,
	int prod, int cons)
{
	u32 prod_reg = ipahal_get_ep_reg_idx(prod);
	u32 cons_reg = ipahal_get_ep_reg_idx(cons);

	return (in->nonip_bitmask[prod_reg] & ipahal_get_ep_bit(prod)) ||
		(in->nonip_bitmask[cons_reg] & ipahal_get_ep_bit(cons));
}

/*
 * ipahal_teth_pair_size() - byte size of the HW counter entry for pair
 *	(prod, cons): 40B when the pair's Non-IP bit is set, 24B otherwise.
 */
static u32 ipahal_teth_pair_size(struct ipahal_stats_init_tethering *in,
	int prod, int cons)
{
	if (ipahal_teth_pair_is_nonip(in, prod, cons))
		return sizeof(struct ipahal_stats_tethering_v7_0_hw);
	return sizeof(struct ipahal_stats_tethering_hw);
}

static struct ipahal_stats_init_pyld *ipahal_generate_init_pyld_quota(
	void *params, bool is_atomic_ctx)
{
	struct ipahal_stats_init_pyld *pyld;
	struct ipahal_stats_init_quota *in =
		(struct ipahal_stats_init_quota *)params;
	int entries = _count_ones(in->enabled_bitmask[0]);

	IPAHAL_DBG_LOW("entries = %d\n", entries);
	pyld = IPAHAL_MEM_ALLOC(sizeof(*pyld) +
		entries * sizeof(struct ipahal_stats_quota_hw), is_atomic_ctx);
	if (!pyld) {
		IPAHAL_ERR("no mem\n");
		return NULL;
	}

	pyld->len = entries * sizeof(struct ipahal_stats_quota_hw);
	return pyld;
}

static struct ipahal_stats_init_pyld *ipahal_generate_init_pyld_quota_v5_0(
	void *params, bool is_atomic_ctx)
{
	struct ipahal_stats_init_pyld *pyld;
	struct ipahal_stats_init_quota *in =
		(struct ipahal_stats_init_quota *)params;
	int i;
	int entries = 0;

	for (i = 0; i < IPAHAL_PIPE_REG_NUM; i++)
		entries += _count_ones(in->enabled_bitmask[i]);

	IPAHAL_DBG_LOW("entries = %d\n", entries);
	pyld = IPAHAL_MEM_ALLOC(sizeof(*pyld) +
		entries * sizeof(struct ipahal_stats_quota_hw), is_atomic_ctx);
	if (!pyld) {
		IPAHAL_ERR("no mem\n");
		return NULL;
	}

	pyld->len = entries * sizeof(struct ipahal_stats_quota_hw);
	return pyld;
}

static int ipahal_get_offset_quota(void *params,
	struct ipahal_stats_offset *out)
{
	struct ipahal_stats_get_offset_quota *in =
		(struct ipahal_stats_get_offset_quota *)params;
	int entries = _count_ones(in->init.enabled_bitmask[0]);

	IPAHAL_DBG_LOW("\n");
	out->offset = 0;
	out->size = entries * sizeof(struct ipahal_stats_quota_hw);

	return 0;
}

static int ipahal_get_offset_quota_v5_0(void *params,
	struct ipahal_stats_offset *out)
{
	struct ipahal_stats_get_offset_quota *in =
		(struct ipahal_stats_get_offset_quota *)params;
	int i, entries = 0;

	for (i = 0; i < IPAHAL_PIPE_REG_NUM; i++)
		entries += _count_ones(in->init.enabled_bitmask[i]);

	IPAHAL_DBG_LOW("\n");
	out->offset = 0;
	out->size = entries * sizeof(struct ipahal_stats_quota_hw);

	return 0;
}

static int ipahal_parse_stats_quota(void *init_params, void *raw_stats,
	void *parsed_stats)
{
	struct ipahal_stats_init_quota *init =
		(struct ipahal_stats_init_quota *)init_params;
	struct ipahal_stats_quota_hw *raw_hw =
		(struct ipahal_stats_quota_hw *)raw_stats;
	struct ipahal_stats_quota_all *out =
		(struct ipahal_stats_quota_all *)parsed_stats;
	int stat_idx = 0;
	int i;

	memset(out, 0, sizeof(*out));
	IPAHAL_DBG_LOW("\n");
	for (i = 0; i < IPAHAL_MAX_PIPES; i++) {
		if (init->enabled_bitmask[0] & (1 << i)) {
			IPAHAL_DBG_LOW("pipe %d stat_idx %d\n", i, stat_idx);
			out->stats[i].num_ipv4_bytes =
				raw_hw[stat_idx].num_ipv4_bytes;
			out->stats[i].num_ipv4_pkts =
				raw_hw[stat_idx].num_ipv4_pkts;
			out->stats[i].num_ipv6_pkts =
				raw_hw[stat_idx].num_ipv6_pkts;
			out->stats[i].num_ipv6_bytes =
				raw_hw[stat_idx].num_ipv6_bytes;
			stat_idx++;
		}
	}

	return 0;
}

static int ipahal_parse_stats_quota_v5_0(void *init_params, void *raw_stats,
	void *parsed_stats)
{
	struct ipahal_stats_init_quota *init =
		(struct ipahal_stats_init_quota *)init_params;
	struct ipahal_stats_quota_hw *raw_hw =
		(struct ipahal_stats_quota_hw *)raw_stats;
	struct ipahal_stats_quota_all *out =
		(struct ipahal_stats_quota_all *)parsed_stats;
	int stat_idx = 0;
	int i, reg_idx;

	memset(out, 0, sizeof(*out));
	IPAHAL_DBG_LOW("\n");
	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {
		reg_idx = ipahal_get_ep_reg_idx(i);
		if (init->enabled_bitmask[reg_idx] & ipahal_get_ep_bit(i)) {
			IPAHAL_DBG_LOW("pipe %d stat_idx %d\n", i, stat_idx);
			out->stats[i].num_ipv4_bytes =
				raw_hw[stat_idx].num_ipv4_bytes;
			out->stats[i].num_ipv4_pkts =
				raw_hw[stat_idx].num_ipv4_pkts;
			out->stats[i].num_ipv6_pkts =
				raw_hw[stat_idx].num_ipv6_pkts;
			out->stats[i].num_ipv6_bytes =
				raw_hw[stat_idx].num_ipv6_bytes;
			stat_idx++;
		}
	}

	return 0;
}

static struct ipahal_stats_init_pyld *ipahal_generate_init_pyld_tethering(
	void *params, bool is_atomic_ctx)
{
	struct ipahal_stats_init_pyld *pyld;
	struct ipahal_stats_init_tethering *in =
		(struct ipahal_stats_init_tethering *)params;
	int hdr_entries = _count_ones(in->prod_bitmask[0]);
	int entries = 0;
	int i;
	void *pyld_ptr;
	u32 incremental_offset;

	IPAHAL_DBG_LOW("prod entries = %d\n", hdr_entries);
	for (i = 0; i < sizeof(in->prod_bitmask[0]) * 8; i++) {
		if (in->prod_bitmask[0] & (1 << i)) {
			if (in->cons_bitmask[i][0] == 0) {
				IPAHAL_ERR("no cons bitmask for prod %d\n", i);
				return NULL;
			}
			entries += _count_ones(in->cons_bitmask[i][0]);
		}
	}
	IPAHAL_DBG_LOW("sum all entries = %d\n", entries);

	pyld = IPAHAL_MEM_ALLOC(sizeof(*pyld) +
		hdr_entries * sizeof(struct ipahal_stats_tethering_hdr_hw) +
		entries * sizeof(struct ipahal_stats_tethering_hw),
		is_atomic_ctx);
	if (!pyld)
		return NULL;

	pyld->len = hdr_entries * sizeof(struct ipahal_stats_tethering_hdr_hw) +
		entries * sizeof(struct ipahal_stats_tethering_hw);

	pyld_ptr = pyld->data;
	incremental_offset =
		(hdr_entries * sizeof(struct ipahal_stats_tethering_hdr_hw))
			/ 8;
	for (i = 0; i < sizeof(in->prod_bitmask[0]) * 8; i++) {
		if (in->prod_bitmask[0] & (1 << i)) {
			struct ipahal_stats_tethering_hdr_hw *hdr = pyld_ptr;

			hdr->dst_mask = in->cons_bitmask[i][0];
			hdr->offset = incremental_offset;
			IPAHAL_DBG_LOW("hdr->dst_mask=0x%x\n", hdr->dst_mask);
			IPAHAL_DBG_LOW("hdr->offset=0x%x\n", hdr->offset);
			/* add the stats entry */
			incremental_offset += _count_ones(
				in->cons_bitmask[i][0]) *
				sizeof(struct ipahal_stats_tethering_hw) / 8;
			pyld_ptr += sizeof(*hdr);
		}
	}

	return pyld;
}

static struct ipahal_stats_init_pyld *ipahal_generate_init_pyld_tethering_v5_0(
	void *params, bool is_atomic_ctx)
{
	struct ipahal_stats_init_pyld *pyld;
	struct ipahal_stats_init_tethering *in =
		(struct ipahal_stats_init_tethering *)params;
	int hdr_entries = 0;
	int entries = 0;
	int i, j, reg_idx;
	void *pyld_ptr;
	u32 incremental_offset;

	for (i = 0; i < IPAHAL_IPA5_PIPE_REG_NUM; i++) {
		hdr_entries += _count_ones(in->prod_bitmask[i]);
	}

	IPAHAL_DBG_LOW("prod entries = %d\n", hdr_entries);
	reg_idx = 0;
	for (i = 0; i < IPAHAL_IPA6_PIPES_NUM; i++) {
		if (i > 0 && !(i % IPAHAL_MAX_PIPES_PER_REG)) {
			reg_idx++;
		}
		if ((reg_idx < IPAHAL_IPA5_PIPE_REG_NUM) &&
			(in->prod_bitmask[reg_idx] & ipahal_get_ep_bit(i))) {
			bool has_cons = false;

			for (j = 0; j < IPAHAL_IPA5_PIPE_REG_NUM; j++) {
				if (in->cons_bitmask[i][j]) {
					has_cons = true;
					entries +=
						_count_ones(in->cons_bitmask[i][j]);
				}
			}
			if (!has_cons) {
				IPAHAL_ERR("no cons bitmask for prod %d\n", i);
				return NULL;
			}
		}
	}
	IPAHAL_DBG_LOW("sum all entries = %d\n", entries);

	pyld = IPAHAL_MEM_ALLOC(sizeof(*pyld) +
		hdr_entries *
		sizeof(struct ipahal_stats_tethering_hdr_v5_0_hw) +
		entries * sizeof(struct ipahal_stats_tethering_hw),
		is_atomic_ctx);
	if (!pyld)
		return NULL;

	pyld->len = hdr_entries *
		sizeof(struct ipahal_stats_tethering_hdr_v5_0_hw) +
		entries * sizeof(struct ipahal_stats_tethering_hw);

	pyld_ptr = pyld->data;

	/*
	 * Note that the address of the offset in the RAM line is of RAM line
	 *(8-byte address) and not like the address in the "BASE" register,
	 * which is a byte address
	 */
	incremental_offset =
		(hdr_entries *
			sizeof(struct ipahal_stats_tethering_hdr_v5_0_hw))
		/ 8;

	reg_idx = 0;
	for (i = 0; i < IPAHAL_IPA6_PIPES_NUM; i++) {

		if (i > 0 && !(i % IPAHAL_MAX_PIPES_PER_REG)) {
			reg_idx++;
		}

		if ((reg_idx < IPAHAL_IPA5_PIPE_REG_NUM) &&
			(in->prod_bitmask[reg_idx] & ipahal_get_ep_bit(i))) {
			struct ipahal_stats_tethering_hdr_v5_0_hw *hdr =
				pyld_ptr;
			// TODO: for future versions of num HW consumers > 16
			hdr->dst_mask_31_0 =
				((in->cons_bitmask[i][0] >> IPAHAL_IPA5_PRODUCER_PIPE_NUM) |
				(in->cons_bitmask[i][1] << IPAHAL_IPA5_PRODUCER_PIPE_NUM));
			hdr->dst_mask_63_32 =
				in->cons_bitmask[i][1] >> IPAHAL_IPA5_PRODUCER_PIPE_NUM;
			// TODO: for future when num pipes > 64
			hdr->dst_mask_95_64 = 0;
			hdr->dst_mask_127_96 = 0;
			hdr->offset = incremental_offset;
			IPAHAL_DBG_LOW("Pipe: %d\n", i);
			IPAHAL_DBG_LOW("hdr->dst_mask_31_0=[0x%x],"
				"hdr->dst_mask_63_32=[0x%x],"
				"hdr->dst_mask_95_64=[0x%x],"
				"hdr->dst_mask_127_96=[0x%x]\n",
				hdr->dst_mask_31_0, hdr->dst_mask_63_32,
				hdr->dst_mask_95_64, hdr->dst_mask_127_96);
			IPAHAL_DBG_LOW("hdr->offset=0x%x\n", hdr->offset);
			/* add the stats entry */
			incremental_offset +=
				(_count_ones(in->cons_bitmask[i][0]) +
				_count_ones(in->cons_bitmask[i][1])) *
				sizeof(struct ipahal_stats_tethering_hw) / 8;
			pyld_ptr += sizeof(*hdr);
		}
	}

	return pyld;
}

static struct ipahal_stats_init_pyld *ipahal_generate_init_pyld_tethering_v6_0(
	void *params, bool is_atomic_ctx)
{
	struct ipahal_stats_init_pyld *pyld;
	struct ipahal_stats_init_tethering *in =
		(struct ipahal_stats_init_tethering *)params;
	int hdr_entries = 0;
	int entries = 0;
	int i, j, reg_idx;
	void *pyld_ptr;
	u32 incremental_offset;

	for (i = 0; i < IPAHAL_PIPE_REG_NUM; i++) {
		hdr_entries += _count_ones(in->prod_bitmask[i]);
	}

	IPAHAL_DBG_LOW("prod entries = %d\n", hdr_entries);
	reg_idx = 0;
	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {
		if (i > 0 && !(i % IPAHAL_MAX_PIPES_PER_REG)) {
			reg_idx++;
		}
		if ((reg_idx < IPAHAL_PIPE_REG_NUM) &&
			(in->prod_bitmask[reg_idx] & ipahal_get_ep_bit(i))) {
			bool has_cons = false;

			for (j = 0; j < IPAHAL_PIPE_REG_NUM; j++) {
				if (in->cons_bitmask[i][j]) {
					has_cons = true;
					entries +=
						_count_ones(in->cons_bitmask[i][j]);
				}
			}
			if (!has_cons) {
				IPAHAL_ERR("no cons bitmask for prod %d\n", i);
				return NULL;
			}
		}
	}
	IPAHAL_DBG_LOW("sum all entries = %d\n", entries);

	pyld = IPAHAL_MEM_ALLOC(sizeof(*pyld) +
		hdr_entries *
		sizeof(struct ipahal_stats_tethering_hdr_v5_0_hw) +
		entries * sizeof(struct ipahal_stats_tethering_hw),
		is_atomic_ctx);
	if (!pyld)
		return NULL;

	pyld->len = hdr_entries *
		sizeof(struct ipahal_stats_tethering_hdr_v5_0_hw) +
		entries * sizeof(struct ipahal_stats_tethering_hw);

	pyld_ptr = pyld->data;

	/*
	 * Note that the address of the offset in the RAM line is of RAM line
	 *(8-byte address) and not like the address in the “BASE” register,
	 * which is a byte address
	 */
	incremental_offset =
		(hdr_entries *
			sizeof(struct ipahal_stats_tethering_hdr_v5_0_hw))
		/ 8;

	reg_idx = 0;
	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {

		if (i > 0 && !(i % IPAHAL_MAX_PIPES_PER_REG)) {
			reg_idx++;
		}

		if ((reg_idx < IPAHAL_PIPE_REG_NUM) &&
			(in->prod_bitmask[reg_idx] & ipahal_get_ep_bit(i))) {
			struct ipahal_stats_tethering_hdr_v5_0_hw *hdr =
				pyld_ptr;
			hdr->dst_mask_31_0 =
				(((u64)in->cons_bitmask[i][0] >> IPAHAL_IPA5_2_PRODUCER_PIPE_NUM) |
				((u64)in->cons_bitmask[i][1] <<
				(IPAHAL_MAX_PIPES_PER_REG - IPAHAL_IPA5_2_PRODUCER_PIPE_NUM)));
			hdr->dst_mask_63_32 =
				(u64)in->cons_bitmask[i][1] >> IPAHAL_IPA5_2_PRODUCER_PIPE_NUM;
			// TODO: for future when num pipes > 64
			hdr->dst_mask_95_64 = 0;
			hdr->dst_mask_127_96 = 0;
			hdr->offset = incremental_offset;
			IPAHAL_DBG_LOW("Pipe: %d\n", i);
			IPAHAL_DBG_LOW("hdr->dst_mask_31_0=[0x%x],"
				"hdr->dst_mask_63_32=[0x%x],"
				"hdr->dst_mask_95_64=[0x%x],"
				"hdr->dst_mask_127_96=[0x%x]\n",
				hdr->dst_mask_31_0, hdr->dst_mask_63_32,
				hdr->dst_mask_95_64, hdr->dst_mask_127_96);
			IPAHAL_DBG_LOW("hdr->offset=0x%x\n", hdr->offset);
			/* add the stats entry */
			incremental_offset +=
				(_count_ones(in->cons_bitmask[i][0]) +
				_count_ones(in->cons_bitmask[i][1])) *
				sizeof(struct ipahal_stats_tethering_hw) / 8;
			pyld_ptr += sizeof(*hdr);
		}
	}

	return pyld;
}

static struct ipahal_stats_init_pyld *ipahal_generate_init_pyld_tethering_v7_0(
	void *params, bool is_atomic_ctx)
{
	struct ipahal_stats_init_pyld *pyld;
	struct ipahal_stats_init_tethering *in =
		(struct ipahal_stats_init_tethering *)params;
	int hdr_entries = 0;
	int entries = 0;
	u32 counters_bytes = 0;
	int i, j, reg_idx;
	void *pyld_ptr;
	u32 incremental_offset;
	u32 running_cfg_index = IPA_TETH_CFG_FIRST_INDEX;

	for (i = 0; i < IPAHAL_PIPE_REG_NUM; i++) {
		hdr_entries += _count_ones(in->prod_bitmask[i]);
	}

	IPAHAL_DBG_LOW("prod entries = %d\n", hdr_entries);
	reg_idx = 0;
	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {
		if (i > 0 && !(i % IPAHAL_MAX_PIPES_PER_REG)) {
			reg_idx++;
		}
		if ((reg_idx < IPAHAL_PIPE_REG_NUM) &&
			(in->prod_bitmask[reg_idx] & ipahal_get_ep_bit(i))) {
			bool has_cons = false;

			for (j = 0; j < IPAHAL_PIPES_NUM; j++) {
				u32 cons_reg = ipahal_get_ep_reg_idx(j);

				if (in->cons_bitmask[i][cons_reg] &
					ipahal_get_ep_bit(j)) {
					has_cons = true;
					entries++;
					/* mixed 24B/40B stride */
					counters_bytes +=
						ipahal_teth_pair_size(in, i, j);
				}
			}
			if (!has_cons) {
				IPAHAL_ERR("no cons bitmask for prod %d\n", i);
				return NULL;
			}
		}
	}
	IPAHAL_DBG_LOW("sum all entries = %d\n", entries);

	pyld = IPAHAL_MEM_ALLOC(sizeof(*pyld) +
		hdr_entries *
		sizeof(struct ipahal_stats_tethering_hdr_v7_0_hw) +
		counters_bytes,
		is_atomic_ctx);
	if (!pyld)
		return NULL;

	pyld->len = hdr_entries *
		sizeof(struct ipahal_stats_tethering_hdr_v7_0_hw) +
		counters_bytes;

	pyld_ptr = pyld->data;

	/*
	 * Note that the address of the offset in the RAM line is of RAM line
	 *(8-byte address) and not like the address in the �BASE� register,
	 * which is a byte address
	 */
	incremental_offset =
		(hdr_entries *
			sizeof(struct ipahal_stats_tethering_hdr_v7_0_hw))
		/ 8;

	reg_idx = 0;
	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {

		if (i > 0 && !(i % IPAHAL_MAX_PIPES_PER_REG)) {
			reg_idx++;
		}

		if ((reg_idx < IPAHAL_PIPE_REG_NUM) &&
			(in->prod_bitmask[reg_idx] & ipahal_get_ep_bit(i))) {
			struct ipahal_stats_tethering_hdr_v7_0_hw *hdr =
				pyld_ptr;
			u32 pair_bytes = 0;
			u32 pair_count = 0;

			hdr->dst_mask_31_0 =
			    (u32)(((u64)in->cons_bitmask[i][0] >> IPAHAL_PRODUCER_PIPE_NUM) |
			    ((u64)in->cons_bitmask[i][1] <<
			    (IPAHAL_MAX_PIPES_PER_REG - IPAHAL_PRODUCER_PIPE_NUM)));
			hdr->dst_mask_63_32 =
			    (u32)((u64)in->cons_bitmask[i][1] >> IPAHAL_PRODUCER_PIPE_NUM);
			// TODO: for future when num pipes > 64
			hdr->dst_mask_95_64 = 0;
			hdr->dst_mask_127_96 = 0;
			/* CFG index range for this consumer starts here */
			hdr->cfg_start_index = running_cfg_index;
			hdr->offset = incremental_offset;
			IPAHAL_DBG_LOW("Pipe: %d\n", i);
			IPAHAL_DBG_LOW("hdr->dst_mask_31_0=[0x%x],"
				"hdr->dst_mask_63_32=[0x%x],"
				"hdr->dst_mask_95_64=[0x%x],"
				"hdr->dst_mask_127_96=[0x%x]\n",
				hdr->dst_mask_31_0, hdr->dst_mask_63_32,
				hdr->dst_mask_95_64, hdr->dst_mask_127_96);
			IPAHAL_DBG_LOW("hdr->offset=0x%x\n", hdr->offset);
			/* sum this producer's pairs (each 24B or 40B) */
			for (j = 0; j < IPAHAL_PIPES_NUM; j++) {
				u32 cons_reg = ipahal_get_ep_reg_idx(j);

				if (!(in->cons_bitmask[i][cons_reg] &
					ipahal_get_ep_bit(j)))
					continue;
				pair_bytes += ipahal_teth_pair_size(in, i, j);
				pair_count++;
			}
			incremental_offset += pair_bytes / 8;
			running_cfg_index += pair_count;
			pyld_ptr += sizeof(*hdr);
		}
	}

	return pyld;
}

static struct ipahal_stats_init_pyld *ipahal_generate_init_pyld_tethering_v7_1(
	void *params, bool is_atomic_ctx)
{
	struct ipahal_stats_init_pyld *pyld;
	struct ipahal_stats_init_tethering *in =
		(struct ipahal_stats_init_tethering *)params;
	int hdr_entries = 0;
	int entries = 0;
	u32 counters_bytes = 0;
	int i, j, reg_idx;
	void *pyld_ptr;
	u32 incremental_offset;
	u32 running_cfg_index = IPA_TETH_CFG_FIRST_INDEX;

	for (i = 0; i < IPAHAL_PIPE_REG_NUM; i++) {
		hdr_entries += _count_ones(in->prod_bitmask[i]);
	}

	IPAHAL_DBG_LOW("prod entries = %d\n", hdr_entries);
	reg_idx = 0;
	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {
		if (i > 0 && !(i % IPAHAL_MAX_PIPES_PER_REG)) {
			reg_idx++;
		}
		if ((reg_idx < IPAHAL_PIPE_REG_NUM) &&
			(in->prod_bitmask[reg_idx] & ipahal_get_ep_bit(i))) {
			bool has_cons = false;

			for (j = 0; j < IPAHAL_PIPES_NUM; j++) {
				u32 cons_reg = ipahal_get_ep_reg_idx(j);

				if (in->cons_bitmask[i][cons_reg] &
					ipahal_get_ep_bit(j)) {
					has_cons = true;
					entries++;
					/* mixed 24B/40B stride */
					counters_bytes +=
						ipahal_teth_pair_size(in, i, j);
				}
			}
			if (!has_cons) {
				IPAHAL_ERR("no cons bitmask for prod %d\n", i);
				return NULL;
			}
		}
	}
	IPAHAL_DBG_LOW("sum all entries = %d\n", entries);

	pyld = IPAHAL_MEM_ALLOC(sizeof(*pyld) +
		hdr_entries *
		sizeof(struct ipahal_stats_tethering_hdr_v7_0_hw) +
		counters_bytes,
		is_atomic_ctx);
	if (!pyld)
		return NULL;

	pyld->len = hdr_entries *
		sizeof(struct ipahal_stats_tethering_hdr_v7_0_hw) +
		counters_bytes;

	pyld_ptr = pyld->data;

	/*
	 * Note that the address of the offset in the RAM line is of RAM line
	 *(8-byte address) and not like the address in the �BASE� register,
	 * which is a byte address
	 */
	incremental_offset =
		(hdr_entries *
			sizeof(struct ipahal_stats_tethering_hdr_v7_0_hw))
		/ 8;

	reg_idx = 0;
	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {

		if (i > 0 && !(i % IPAHAL_MAX_PIPES_PER_REG)) {
			reg_idx++;
		}

		if ((reg_idx < IPAHAL_PIPE_REG_NUM) &&
			(in->prod_bitmask[reg_idx] & ipahal_get_ep_bit(i))) {
			struct ipahal_stats_tethering_hdr_v7_0_hw *hdr =
				pyld_ptr;
			u32 pair_bytes = 0;
			u32 pair_count = 0;

			hdr->dst_mask_31_0 =
			    (u32)(((u64)in->cons_bitmask[i][0] >> IPAHAL_IPA7_1PRODUCER_PIPE_NUM) |
			    ((u64)in->cons_bitmask[i][1] <<
			    (IPAHAL_MAX_PIPES_PER_REG - IPAHAL_IPA7_1PRODUCER_PIPE_NUM)));
			hdr->dst_mask_63_32 =
			    (u32)((u64)in->cons_bitmask[i][1] >> IPAHAL_IPA7_1PRODUCER_PIPE_NUM);
			// TODO: for future when num pipes > 64
			hdr->dst_mask_95_64 = 0;
			hdr->dst_mask_127_96 = 0;
			/* CFG index range for this consumer starts here.
			 * NOTE: this v7_1 generator is currently dead code
			 * (no IPA_HW_v7_1 row in ipahal_hw_stats_objs); kept
			 * in lockstep with v7_0 for when v7.1 is wired up.
			 */
			hdr->cfg_start_index = running_cfg_index;
			hdr->offset = incremental_offset;
			IPAHAL_DBG_LOW("Pipe: %d\n", i);
			IPAHAL_DBG_LOW("hdr->dst_mask_31_0=[0x%x],"
				"hdr->dst_mask_63_32=[0x%x],"
				"hdr->dst_mask_95_64=[0x%x],"
				"hdr->dst_mask_127_96=[0x%x]\n",
				hdr->dst_mask_31_0, hdr->dst_mask_63_32,
				hdr->dst_mask_95_64, hdr->dst_mask_127_96);
			IPAHAL_DBG_LOW("hdr->offset=0x%x\n", hdr->offset);
			/* sum this producer's pairs (each 24B or 40B) */
			for (j = 0; j < IPAHAL_PIPES_NUM; j++) {
				u32 cons_reg = ipahal_get_ep_reg_idx(j);

				if (!(in->cons_bitmask[i][cons_reg] &
					ipahal_get_ep_bit(j)))
					continue;
				pair_bytes += ipahal_teth_pair_size(in, i, j);
				pair_count++;
			}
			incremental_offset += pair_bytes / 8;
			running_cfg_index += pair_count;
			pyld_ptr += sizeof(*hdr);
		}
	}

	return pyld;
}

/*
 * ipahal_stats_generate_teth_cfg_pyld() - see ipahal_hw_stats.h.
 *
 * Walks (producer, consumer) pairs pipe-ascending, the same order the parse
 * function assumes, so CFG index N matches counter entry N. CFG index is
 * 1-based; 4 counters share one byte (2 bits/counter: bit0 = ETH header
 * bytes, bit1 = Non-IP).
 */
struct ipahal_stats_init_pyld *ipahal_stats_generate_teth_cfg_pyld(
	struct ipahal_stats_init_tethering *init, bool is_atomic_ctx)
{
	struct ipahal_stats_init_pyld *pyld;
	int total_entries = 0;
	int i, j, prod_reg, cons_reg;
	u32 cfg_index = IPA_TETH_CFG_FIRST_INDEX;
	u16 len;
	u8 *data;

	if (!init)
		return NULL;

	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {
		prod_reg = ipahal_get_ep_reg_idx(i);
		if (!(init->prod_bitmask[prod_reg] & ipahal_get_ep_bit(i)))
			continue;
		for (j = 0; j < IPAHAL_PIPES_NUM; j++) {
			cons_reg = ipahal_get_ep_reg_idx(j);
			if (init->cons_bitmask[i][cons_reg] & ipahal_get_ep_bit(j))
				total_entries++;
		}
	}

	if (!total_entries) {
		IPAHAL_DBG_LOW("no entries, skip cfg pyld\n");
		return NULL;
	}

	/* 4 counters per byte, 1-based index */
	len = (total_entries + 3) / 4;

	pyld = IPAHAL_MEM_ALLOC(sizeof(*pyld) + len, is_atomic_ctx);
	if (!pyld)
		return NULL;

	pyld->len = len;
	data = pyld->data;

	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {
		prod_reg = ipahal_get_ep_reg_idx(i);
		if (!(init->prod_bitmask[prod_reg] & ipahal_get_ep_bit(i)))
			continue;
		for (j = 0; j < IPAHAL_PIPES_NUM; j++) {
			u32 byte_idx, shift;
			bool eth, nonip;
			u8 bits;

			cons_reg = ipahal_get_ep_reg_idx(j);
			if (!(init->cons_bitmask[i][cons_reg] & ipahal_get_ep_bit(j)))
				continue;

			eth = (init->eth_bitmask[prod_reg] & ipahal_get_ep_bit(i)) ||
				(init->eth_bitmask[cons_reg] & ipahal_get_ep_bit(j));
			/* shared source of truth with generate/offset/parse */
			nonip = ipahal_teth_pair_is_nonip(init, i, j);
			bits = (eth ? 0x1 : 0) | (nonip ? 0x2 : 0);

			byte_idx = (cfg_index - 1) / 4;
			shift = ((cfg_index - 1) % 4) * 2;
			data[byte_idx] |= bits << shift;
			cfg_index++;
		}
	}

	return pyld;
}

static int ipahal_get_offset_tethering_v7_0(void *params,
	struct ipahal_stats_offset *out)
{
	struct ipahal_stats_get_offset_tethering *in =
		(struct ipahal_stats_get_offset_tethering *)params;
	int entries = 0;
	u32 counters_bytes = 0;
	int i, j, reg_idx;

	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {
		reg_idx = ipahal_get_ep_reg_idx(i);

		if (in->init.prod_bitmask[reg_idx] & ipahal_get_ep_bit(i)) {
			bool has_cons = false;

			for (j = 0; j < IPAHAL_PIPES_NUM; j++) {
				u32 cons_reg = ipahal_get_ep_reg_idx(j);

				if (in->init.cons_bitmask[i][cons_reg] &
					ipahal_get_ep_bit(j)) {
					has_cons = true;
					entries++;
					/* mixed 24B/40B stride - see
					 * ipahal_teth_pair_size()
					 */
					counters_bytes += ipahal_teth_pair_size(
						&in->init, i, j);
				}
			}
			if (!has_cons) {
				IPAHAL_ERR("no cons bitmask for prod %d\n", i);
				return -EPERM;
			}
		}
	}
	IPAHAL_DBG_LOW("sum all entries = %d\n", entries);

	/* skip the header */
	out->offset = 0;
	for (j = 0; j < IPAHAL_PIPE_REG_NUM; j++)
		out->offset += _count_ones(in->init.prod_bitmask[j]) *
		sizeof(struct ipahal_stats_tethering_hdr_v7_0_hw);

	out->size = counters_bytes;

	return 0;
}


static int ipahal_get_offset_tethering(void *params,
	struct ipahal_stats_offset *out)
{
	struct ipahal_stats_get_offset_tethering *in =
		(struct ipahal_stats_get_offset_tethering *)params;
	int entries = 0;
	int i;

	for (i = 0; i < sizeof(in->init.prod_bitmask[0]) * 8; i++) {
		if (in->init.prod_bitmask[0] & (1 << i)) {
			if (in->init.cons_bitmask[i][0] == 0) {
				IPAHAL_ERR("no cons bitmask for prod %d\n", i);
				return -EPERM;
			}
			entries += _count_ones(in->init.cons_bitmask[i][0]);
		}
	}
	IPAHAL_DBG_LOW("sum all entries = %d\n", entries);

	/* skip the header */
	out->offset = _count_ones(in->init.prod_bitmask[0]) *
		sizeof(struct ipahal_stats_tethering_hdr_hw);
	out->size = entries * sizeof(struct ipahal_stats_tethering_hw);

	return 0;
}

static int ipahal_get_offset_tethering_v5_0(void *params,
	struct ipahal_stats_offset *out)
{
	struct ipahal_stats_get_offset_tethering *in =
		(struct ipahal_stats_get_offset_tethering *)params;
	int entries = 0;
	int i, j, reg_idx;

	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {
		reg_idx = ipahal_get_ep_reg_idx(i);

		if (in->init.prod_bitmask[reg_idx] & ipahal_get_ep_bit(i)) {
			bool has_cons = false;

			for (j = 0; j < IPAHAL_PIPE_REG_NUM; j++) {
				if (in->init.cons_bitmask[i][j]) {
					has_cons = true;
					entries +=_count_ones(
						in->init.cons_bitmask[i][j]);
				}
			}
			if (!has_cons) {
				IPAHAL_ERR("no cons bitmask for prod %d\n", i);
				return -EPERM;
			}
		}
	}
	IPAHAL_DBG_LOW("sum all entries = %d\n", entries);

	/* skip the header */
	out->offset = 0;
	for (j = 0; j < IPAHAL_PIPE_REG_NUM; j++)
		out->offset += _count_ones(in->init.prod_bitmask[j]) *
		sizeof(struct ipahal_stats_tethering_hdr_v5_0_hw);

	out->size = entries * sizeof(struct ipahal_stats_tethering_hw);

	return 0;
}

static int ipahal_parse_stats_tethering(void *init_params, void *raw_stats,
	void *parsed_stats)
{
	struct ipahal_stats_init_tethering *init =
		(struct ipahal_stats_init_tethering *)init_params;
	struct ipahal_stats_tethering_hw *raw_hw =
		(struct ipahal_stats_tethering_hw *)raw_stats;
	struct ipahal_stats_tethering_all *out =
		(struct ipahal_stats_tethering_all *)parsed_stats;
	int i, j;
	int stat_idx = 0;

	memset(out, 0, sizeof(*out));
	IPAHAL_DBG_LOW("\n");
	for (i = 0; i < IPAHAL_MAX_PIPES; i++) {
		for (j = 0; j < IPAHAL_MAX_PIPES; j++) {
			if ((init->prod_bitmask[0] & (1 << i)) &&
			    init->cons_bitmask[i][0] & (1 << j)) {
				IPAHAL_DBG_LOW("prod %d cons %d\n", i, j);
				IPAHAL_DBG_LOW("stat_idx %d\n", stat_idx);
				out->stats[i][j].num_ipv4_bytes =
					raw_hw[stat_idx].num_ipv4_bytes;
				IPAHAL_DBG_LOW("num_ipv4_bytes %lld\n",
					out->stats[i][j].num_ipv4_bytes);
				out->stats[i][j].num_ipv4_pkts =
					raw_hw[stat_idx].num_ipv4_pkts;
				IPAHAL_DBG_LOW("num_ipv4_pkts %lld\n",
					out->stats[i][j].num_ipv4_pkts);
				out->stats[i][j].num_ipv6_pkts =
					raw_hw[stat_idx].num_ipv6_pkts;
				IPAHAL_DBG_LOW("num_ipv6_pkts %lld\n",
					out->stats[i][j].num_ipv6_pkts);
				out->stats[i][j].num_ipv6_bytes =
					raw_hw[stat_idx].num_ipv6_bytes;
				IPAHAL_DBG_LOW("num_ipv6_bytes %lld\n",
					out->stats[i][j].num_ipv6_bytes);
				stat_idx++;
			}
		}
	}

	return 0;
}

static int ipahal_parse_stats_tethering_v5_0(void *init_params, void *raw_stats,
	void *parsed_stats)
{
	struct ipahal_stats_init_tethering *init =
		(struct ipahal_stats_init_tethering *)init_params;
	struct ipahal_stats_tethering_hw *raw_hw =
		(struct ipahal_stats_tethering_hw *)raw_stats;
	struct ipahal_stats_tethering_all *out =
		(struct ipahal_stats_tethering_all *)parsed_stats;
	int i, j;
	int stat_idx = 0;
	int prod_idx, cons_idx;

	memset(out, 0, sizeof(*out));
	IPAHAL_DBG_LOW("\n");
	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {
		prod_idx = ipahal_get_ep_reg_idx(i);
		for (j = 0; j < IPAHAL_PIPES_NUM; j++) {
			cons_idx = ipahal_get_ep_reg_idx(j);
			if ((init->prod_bitmask[prod_idx] &
				ipahal_get_ep_bit(i)) &&
				init->cons_bitmask[i][cons_idx] &
				ipahal_get_ep_bit(j)) {
				IPAHAL_DBG_LOW("prod %d cons %d\n", i, j);
				IPAHAL_DBG_LOW("stat_idx %d\n", stat_idx);
				out->stats[i][j].num_ipv4_bytes =
					raw_hw[stat_idx].num_ipv4_bytes;
				IPAHAL_DBG_LOW("num_ipv4_bytes %lld\n",
					out->stats[i][j].num_ipv4_bytes);
				out->stats[i][j].num_ipv4_pkts =
					raw_hw[stat_idx].num_ipv4_pkts;
				IPAHAL_DBG_LOW("num_ipv4_pkts %lld\n",
					out->stats[i][j].num_ipv4_pkts);
				out->stats[i][j].num_ipv6_pkts =
					raw_hw[stat_idx].num_ipv6_pkts;
				IPAHAL_DBG_LOW("num_ipv6_pkts %lld\n",
					out->stats[i][j].num_ipv6_pkts);
				out->stats[i][j].num_ipv6_bytes =
					raw_hw[stat_idx].num_ipv6_bytes;
				IPAHAL_DBG_LOW("num_ipv6_bytes %lld\n",
					out->stats[i][j].num_ipv6_bytes);
				stat_idx++;
			}
		}
	}

	return 0;
}

/*
 * ipahal_parse_stats_tethering_v7_0() - parse IPA 7.0 tethering counters.
 *
 * Mixed-stride: a Non-IP pair occupies 40B, all others 24B. The raw pointer
 * advances per pair by ipahal_teth_pair_size(), matching the generate and
 * get_offset paths.
 */
static int ipahal_parse_stats_tethering_v7_0(void *init_params, void *raw_stats,
	void *parsed_stats)
{
	struct ipahal_stats_init_tethering *init =
		(struct ipahal_stats_init_tethering *)init_params;
	struct ipahal_stats_tethering_all *out =
		(struct ipahal_stats_tethering_all *)parsed_stats;
	u8 *raw_ptr = (u8 *)raw_stats;
	int i, j;
	int prod_idx, cons_idx;

	memset(out, 0, sizeof(*out));
	IPAHAL_DBG_LOW("\n");
	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {
		prod_idx = ipahal_get_ep_reg_idx(i);
		for (j = 0; j < IPAHAL_PIPES_NUM; j++) {
			cons_idx = ipahal_get_ep_reg_idx(j);
			if ((init->prod_bitmask[prod_idx] &
				ipahal_get_ep_bit(i)) &&
				init->cons_bitmask[i][cons_idx] &
				ipahal_get_ep_bit(j)) {
				IPAHAL_DBG_LOW("prod %d cons %d\n", i, j);
				if (ipahal_teth_pair_is_nonip(init, i, j)) {
					struct ipahal_stats_tethering_v7_0_hw
						*raw_hw = (struct
						ipahal_stats_tethering_v7_0_hw *)
						raw_ptr;

					out->stats[i][j].num_ipv4_bytes =
						raw_hw->num_ipv4_bytes;
					out->stats[i][j].num_ipv4_pkts =
						raw_hw->num_ipv4_pkts;
					out->stats[i][j].num_ipv6_pkts =
						raw_hw->num_ipv6_pkts;
					out->stats[i][j].num_ipv6_bytes =
						raw_hw->num_ipv6_bytes;
					out->stats[i][j].num_nonip_pkts =
						raw_hw->num_nonip_pkts;
					out->stats[i][j].num_nonip_bytes =
						raw_hw->num_nonip_bytes;
				} else {
					struct ipahal_stats_tethering_hw
						*raw_hw = (struct
						ipahal_stats_tethering_hw *)
						raw_ptr;

					out->stats[i][j].num_ipv4_bytes =
						raw_hw->num_ipv4_bytes;
					out->stats[i][j].num_ipv4_pkts =
						raw_hw->num_ipv4_pkts;
					out->stats[i][j].num_ipv6_pkts =
						raw_hw->num_ipv6_pkts;
					out->stats[i][j].num_ipv6_bytes =
						raw_hw->num_ipv6_bytes;
				}
				IPAHAL_DBG_LOW("num_ipv4_bytes %lld\n",
					out->stats[i][j].num_ipv4_bytes);
				IPAHAL_DBG_LOW("num_ipv4_pkts %lld\n",
					out->stats[i][j].num_ipv4_pkts);
				IPAHAL_DBG_LOW("num_ipv6_pkts %lld\n",
					out->stats[i][j].num_ipv6_pkts);
				IPAHAL_DBG_LOW("num_ipv6_bytes %lld\n",
					out->stats[i][j].num_ipv6_bytes);
				raw_ptr += ipahal_teth_pair_size(init, i, j);
			}
		}
	}

	return 0;
}

static struct ipahal_stats_init_pyld *ipahal_generate_init_pyld_flt_rt_v4_5(
	void *params, bool is_atomic_ctx)
{
	struct ipahal_stats_init_pyld *pyld;
	long num = (long)(params); /* params is treated as a pointer. */
	long max_num = (ipahal_ctx->hw_type >= IPA_HW_v7_0) ?
		IPA_MAX_FLT_RT_CNT_INDEX_V7 : IPA_MAX_FLT_RT_CNT_INDEX;

	if (num > max_num || num <= 0) {
		IPAHAL_ERR("num %ld not valid\n", num);
		return NULL;
	}
	pyld = IPAHAL_MEM_ALLOC(sizeof(*pyld) +
		num *
		sizeof(struct ipahal_stats_flt_rt_v4_5_hw),
		is_atomic_ctx);
	if (!pyld)
		return NULL;
	pyld->len = num *
		sizeof(struct ipahal_stats_flt_rt_v4_5_hw);
	return pyld;
}

static int ipahal_get_offset_flt_rt_v4_5(void *params,
	struct ipahal_stats_offset *out)
{
	struct ipahal_stats_get_offset_flt_rt_v4_5 *in =
		(struct ipahal_stats_get_offset_flt_rt_v4_5 *)params;
	int num;

	out->offset = (in->start_id - 1) *
		sizeof(struct ipahal_stats_flt_rt_v4_5);
	num = in->end_id - in->start_id + 1;
	out->size = num * sizeof(struct ipahal_stats_flt_rt_v4_5);

	return 0;
}

static int ipahal_parse_stats_flt_rt_v4_5(void *init_params,
	void *raw_stats, void *parsed_stats)
{
	struct ipahal_stats_flt_rt_v4_5_hw *raw_hw =
		(struct ipahal_stats_flt_rt_v4_5_hw *)raw_stats;
	struct ipa_ioc_flt_rt_query *query =
		(struct ipa_ioc_flt_rt_query *)parsed_stats;
	int num, i;

	num = query->end_id - query->start_id + 1;
	IPAHAL_DBG_LOW("\n");
	for (i = 0; i < num; i++) {
		((struct ipa_flt_rt_stats *)
		query->stats)[i].num_bytes =
			raw_hw[i].num_bytes;
		((struct ipa_flt_rt_stats *)
		query->stats)[i].num_pkts_hash =
			raw_hw[i].num_packets_hash;
		((struct ipa_flt_rt_stats *)
		query->stats)[i].num_pkts =
			raw_hw[i].num_packets;
	}

	return 0;
}


static struct ipahal_stats_init_pyld *ipahal_generate_init_pyld_flt_rt(
	void *params, bool is_atomic_ctx)
{
	struct ipahal_stats_init_pyld *pyld;
	struct ipahal_stats_init_flt_rt *in =
		(struct ipahal_stats_init_flt_rt *)params;
	int hdr_entries;
	int num_rules = 0;
	int i, start_entry;
	void *pyld_ptr;
	u32 incremental_offset;

	for (i = 0; i < IPAHAL_MAX_RULE_ID_32; i++)
		num_rules += _count_ones(in->rule_id_bitmask[i]);

	if (num_rules == 0) {
		IPAHAL_ERR("no rule ids provided\n");
		return NULL;
	}
	IPAHAL_DBG_LOW("num_rules = %d\n", num_rules);

	hdr_entries = IPAHAL_MAX_RULE_ID_32;
	for (i = 0; i < IPAHAL_MAX_RULE_ID_32; i++) {
		if (in->rule_id_bitmask[i] != 0)
			break;
		hdr_entries--;
	}
	start_entry = i;

	for (i = IPAHAL_MAX_RULE_ID_32 - 1; i >= start_entry; i--) {
		if (in->rule_id_bitmask[i] != 0)
			break;
		hdr_entries--;
	}
	IPAHAL_DBG_LOW("hdr_entries = %d\n", hdr_entries);

	pyld = IPAHAL_MEM_ALLOC(sizeof(*pyld) +
		hdr_entries * sizeof(struct ipahal_stats_flt_rt_hdr_hw) +
		num_rules * sizeof(struct ipahal_stats_flt_rt_hw),
		is_atomic_ctx);
	if (!pyld) {
		IPAHAL_ERR("no mem\n");
		return NULL;
	}

	pyld->len = hdr_entries * sizeof(struct ipahal_stats_flt_rt_hdr_hw) +
		num_rules * sizeof(struct ipahal_stats_flt_rt_hw);

	pyld_ptr = pyld->data;
	incremental_offset =
		(hdr_entries * sizeof(struct ipahal_stats_flt_rt_hdr_hw))
			/ 8;
	for (i = start_entry; i < hdr_entries; i++) {
		struct ipahal_stats_flt_rt_hdr_hw *hdr = pyld_ptr;

		hdr->en_mask = in->rule_id_bitmask[i];
		hdr->cnt_offset = incremental_offset;
		/* add the stats entry */
		incremental_offset += _count_ones(in->rule_id_bitmask[i]) *
			sizeof(struct ipahal_stats_flt_rt_hw) / 8;
		pyld_ptr += sizeof(*hdr);
	}

	return pyld;
}

static int ipahal_get_offset_flt_rt(void *params,
	struct ipahal_stats_offset *out)
{
	struct ipahal_stats_get_offset_flt_rt *in =
		(struct ipahal_stats_get_offset_flt_rt *)params;
	int i;
	int hdr_entries;
	int skip_rules = 0;
	int start_entry;
	int rule_bit = in->rule_id % 32;
	int rule_idx = in->rule_id / 32;

	if (rule_idx >= IPAHAL_MAX_RULE_ID_32) {
		IPAHAL_ERR("invalid rule_id %d\n", in->rule_id);
		return -EPERM;
	}

	hdr_entries = IPAHAL_MAX_RULE_ID_32;
	for (i = 0; i < IPAHAL_MAX_RULE_ID_32; i++) {
		if (in->init.rule_id_bitmask[i] != 0)
			break;
		hdr_entries--;
	}

	if (hdr_entries == 0) {
		IPAHAL_ERR("no rule ids provided\n");
		return -EPERM;
	}
	start_entry = i;

	for (i = IPAHAL_MAX_RULE_ID_32 - 1; i >= 0; i--) {
		if (in->init.rule_id_bitmask[i] != 0)
			break;
		hdr_entries--;
	}
	IPAHAL_DBG_LOW("hdr_entries = %d\n", hdr_entries);

	/* skip the header */
	out->offset = hdr_entries * sizeof(struct ipahal_stats_flt_rt_hdr_hw);

	/* skip the previous rules  */
	for (i = start_entry; i < rule_idx; i++)
		skip_rules += _count_ones(in->init.rule_id_bitmask[i]);

	for (i = 0; i < rule_bit; i++)
		if (in->init.rule_id_bitmask[rule_idx] & (1 << i))
			skip_rules++;

	out->offset += skip_rules * sizeof(struct ipahal_stats_flt_rt_hw);
	out->size = sizeof(struct ipahal_stats_flt_rt_hw);

	return 0;
}

static int ipahal_parse_stats_flt_rt(void *init_params, void *raw_stats,
	void *parsed_stats)
{
	struct ipahal_stats_flt_rt_hw *raw_hw =
		(struct ipahal_stats_flt_rt_hw *)raw_stats;
	struct ipahal_stats_flt_rt *out =
		(struct ipahal_stats_flt_rt *)parsed_stats;

	memset(out, 0, sizeof(*out));
	IPAHAL_DBG_LOW("\n");
	out->num_packets = raw_hw->num_packets;
	out->num_packets_hash = raw_hw->num_packets_hash;

	return 0;
}

static struct ipahal_stats_init_pyld *ipahal_generate_init_pyld_drop(
	void *params, bool is_atomic_ctx)
{
	struct ipahal_stats_init_pyld *pyld;
	struct ipahal_stats_init_drop *in =
		(struct ipahal_stats_init_drop *)params;
	int entries = _count_ones(in->enabled_bitmask[0]);

	IPAHAL_DBG_LOW("entries = %d\n", entries);
	pyld = IPAHAL_MEM_ALLOC(sizeof(*pyld) +
		entries * sizeof(struct ipahal_stats_drop_hw), is_atomic_ctx);
	if (!pyld)
		return NULL;

	pyld->len = entries * sizeof(struct ipahal_stats_drop_hw);

	return pyld;
}

static struct ipahal_stats_init_pyld *ipahal_generate_init_pyld_drop_v5_0(
	void *params, bool is_atomic_ctx)
{
	struct ipahal_stats_init_pyld *pyld;
	struct ipahal_stats_init_drop *in =
		(struct ipahal_stats_init_drop *)params;
	int entries = 0;
	int i;

	for (i = 0; i < IPAHAL_PIPE_REG_NUM; i++)
		entries += _count_ones(in->enabled_bitmask[i]);
	IPAHAL_DBG_LOW("entries = %d\n", entries);
	pyld = IPAHAL_MEM_ALLOC(sizeof(*pyld) +
		entries * sizeof(struct ipahal_stats_drop_hw), is_atomic_ctx);
	if (!pyld)
		return NULL;

	pyld->len = entries * sizeof(struct ipahal_stats_drop_hw);

	return pyld;
}

static int ipahal_get_offset_drop(void *params,
	struct ipahal_stats_offset *out)
{
	struct ipahal_stats_get_offset_drop *in =
		(struct ipahal_stats_get_offset_drop *)params;
	int entries = _count_ones(in->init.enabled_bitmask[0]);

	IPAHAL_DBG_LOW("\n");
	out->offset = 0;
	out->size = entries * sizeof(struct ipahal_stats_drop_hw);

	return 0;
}

static int ipahal_get_offset_drop_v5_0(void *params,
	struct ipahal_stats_offset *out)
{
	struct ipahal_stats_get_offset_drop *in =
		(struct ipahal_stats_get_offset_drop *)params;
	int entries = 0;
	int i;

	for (i = 0; i < IPAHAL_PIPE_REG_NUM; i++)
		entries += _count_ones(in->init.enabled_bitmask[i]);
	IPAHAL_DBG_LOW("entries %d\n", entries);

	out->offset = 0;
	out->size = entries * sizeof(struct ipahal_stats_drop_hw);

	return 0;
}

static int ipahal_parse_stats_drop(void *init_params, void *raw_stats,
	void *parsed_stats)
{
	struct ipahal_stats_init_drop *init =
		(struct ipahal_stats_init_drop *)init_params;
	struct ipahal_stats_drop_hw *raw_hw =
		(struct ipahal_stats_drop_hw *)raw_stats;
	struct ipahal_stats_drop_all *out =
		(struct ipahal_stats_drop_all *)parsed_stats;
	int stat_idx = 0;
	int i;

	memset(out, 0, sizeof(*out));
	IPAHAL_DBG_LOW("\n");
	for (i = 0; i < IPAHAL_MAX_PIPES; i++) {
		if (init->enabled_bitmask[0] & (1 << i)) {
			out->stats[i].drop_byte_cnt =
				raw_hw[stat_idx].drop_byte_cnt;
			out->stats[i].drop_packet_cnt =
				raw_hw[stat_idx].drop_packet_cnt;
			stat_idx++;
		}
	}

	return 0;
}

static int ipahal_parse_stats_drop_v5_0(void *init_params, void *raw_stats,
	void *parsed_stats)
{
	struct ipahal_stats_init_drop *init =
		(struct ipahal_stats_init_drop *)init_params;
	struct ipahal_stats_drop_hw *raw_hw =
		(struct ipahal_stats_drop_hw *)raw_stats;
	struct ipahal_stats_drop_all *out =
		(struct ipahal_stats_drop_all *)parsed_stats;
	int stat_idx = 0;
	int i, reg_idx;

	memset(out, 0, sizeof(*out));
	IPAHAL_DBG_LOW("\n");
	for (i = 0; i < IPAHAL_PIPES_NUM; i++) {
		reg_idx = ipahal_get_ep_reg_idx(i);
		if (init->enabled_bitmask[reg_idx] & ipahal_get_ep_bit(i)) {
			out->stats[i].drop_byte_cnt =
				raw_hw[stat_idx].drop_byte_cnt;
			out->stats[i].drop_packet_cnt =
				raw_hw[stat_idx].drop_packet_cnt;
			stat_idx++;
		}
	}

	return 0;
}

static struct ipahal_stats_init_pyld *ipahal_generate_init_pyld_nat_ct(
	void *params, bool is_atomic_ctx)
{
	struct ipahal_stats_init_pyld *pyld;
	struct ipahal_stats_init_nat_ct *init =
		(struct ipahal_stats_init_nat_ct *)params;
	int num_counters;

	if (!init) {
		IPAHAL_ERR("no params\n");
		return NULL;
	}
	num_counters = init->max_counters;

	if (num_counters > IPAHAL_NAT_CT_MAX_COUNTERS) {
		IPAHAL_ERR("num_counters %d > %d\n", num_counters, IPAHAL_NAT_CT_MAX_COUNTERS);
		return NULL;
	}

	pyld = IPAHAL_MEM_ALLOC(sizeof(*pyld) +
		num_counters * sizeof(struct ipahal_stats_nat_ct_hw) +
		num_counters,
		is_atomic_ctx);
	if (!pyld)
		return NULL;

	pyld->len = num_counters * sizeof(struct ipahal_stats_nat_ct_hw) + num_counters;
	return pyld;
}

static int ipahal_get_offset_nat_ct(void *params,
	struct ipahal_stats_offset *out)
{
	struct ipahal_stats_get_offset_nat_ct *in =
		(struct ipahal_stats_get_offset_nat_ct *)params;

	if (in->counter_index < 1 ||
		in->counter_index > in->max_counters) {
		IPAHAL_ERR("Invalid counter_index %d\n", in->counter_index);
		return -EINVAL;
	}

	out->offset = (in->counter_index - 1) * sizeof(struct ipahal_stats_nat_ct_hw);
	out->size = sizeof(struct ipahal_stats_nat_ct_hw);

	return 0;
}

static int ipahal_parse_stats_nat_ct(void *init_params, void *raw_stats,
	void *parsed_stats)
{
	struct ipahal_stats_nat_ct_hw *raw_hw =
		(struct ipahal_stats_nat_ct_hw *)raw_stats;
	struct ipahal_stats_nat_ct *out =
		(struct ipahal_stats_nat_ct *)parsed_stats;

	out->num_pkts_inbound = raw_hw->num_pkts_inbound;
	out->num_pkts_cache_inbound = raw_hw->num_pkts_cache_inbound;
	out->num_bytes_inbound = raw_hw->num_bytes_inbound;
	out->num_pkts_outbound = raw_hw->num_pkts_outbound;
	out->num_pkts_cache_outbound = raw_hw->num_pkts_cache_outbound;
	out->num_bytes_outbound = raw_hw->num_bytes_outbound;

	return 0;
}

static struct ipahal_hw_stats_obj
	ipahal_hw_stats_objs[IPA_HW_MAX][IPAHAL_HW_STATS_MAX] = {
	/* IPAv4 */
	[IPA_HW_v4_0][IPAHAL_HW_STATS_QUOTA] = {
		ipahal_generate_init_pyld_quota,
		ipahal_get_offset_quota,
		ipahal_parse_stats_quota
	},
	[IPA_HW_v4_0][IPAHAL_HW_STATS_TETHERING] = {
		ipahal_generate_init_pyld_tethering,
		ipahal_get_offset_tethering,
		ipahal_parse_stats_tethering
	},
	[IPA_HW_v4_0][IPAHAL_HW_STATS_FNR] = {
		ipahal_generate_init_pyld_flt_rt,
		ipahal_get_offset_flt_rt,
		ipahal_parse_stats_flt_rt
	},
	[IPA_HW_v4_0][IPAHAL_HW_STATS_DROP] = {
		ipahal_generate_init_pyld_drop,
		ipahal_get_offset_drop,
		ipahal_parse_stats_drop
	},

	/* IPAv4_5 */
	[IPA_HW_v4_5][IPAHAL_HW_STATS_QUOTA] = {
		ipahal_generate_init_pyld_quota,
		ipahal_get_offset_quota,
		ipahal_parse_stats_quota
	},
	[IPA_HW_v4_5][IPAHAL_HW_STATS_FNR] = {
		ipahal_generate_init_pyld_flt_rt_v4_5,
		ipahal_get_offset_flt_rt_v4_5,
		ipahal_parse_stats_flt_rt_v4_5
	},
	[IPA_HW_v4_5][IPAHAL_HW_STATS_TETHERING] = {
		ipahal_generate_init_pyld_tethering,
		ipahal_get_offset_tethering,
		ipahal_parse_stats_tethering
	},
	[IPA_HW_v4_5][IPAHAL_HW_STATS_DROP] = {
		ipahal_generate_init_pyld_drop,
		ipahal_get_offset_drop,
		ipahal_parse_stats_drop
	},

	/* IPAv5_0 */
	[IPA_HW_v5_0][IPAHAL_HW_STATS_TETHERING] = {
	ipahal_generate_init_pyld_tethering_v5_0,
	ipahal_get_offset_tethering_v5_0,
	ipahal_parse_stats_tethering_v5_0
	},
	[IPA_HW_v5_0][IPAHAL_HW_STATS_QUOTA] = {
	ipahal_generate_init_pyld_quota_v5_0,
	ipahal_get_offset_quota_v5_0,
	ipahal_parse_stats_quota_v5_0
	},
	[IPA_HW_v5_0][IPAHAL_HW_STATS_DROP] = {
	ipahal_generate_init_pyld_drop_v5_0,
	ipahal_get_offset_drop_v5_0,
	ipahal_parse_stats_drop_v5_0
	},

	/* IPAv6_0 */
	[IPA_HW_v6_0][IPAHAL_HW_STATS_TETHERING] = {
	ipahal_generate_init_pyld_tethering_v6_0,
	ipahal_get_offset_tethering_v5_0,
	ipahal_parse_stats_tethering_v5_0
	},

	/* IPAv7_0 */
	[IPA_HW_v7_0][IPAHAL_HW_STATS_TETHERING] = {
	ipahal_generate_init_pyld_tethering_v7_0,
	ipahal_get_offset_tethering_v7_0,
	ipahal_parse_stats_tethering_v7_0
	},
	[IPA_HW_v7_0][IPAHAL_HW_STATS_NAT_CT] = {
		ipahal_generate_init_pyld_nat_ct,
		ipahal_get_offset_nat_ct,
		ipahal_parse_stats_nat_ct
	},
};

int ipahal_hw_stats_init(enum ipa_hw_type ipa_hw_type)
{
	int i;
	int j;
	struct ipahal_hw_stats_obj zero_obj;
	struct ipahal_hw_stats_obj *hw_stat_ptr;

	IPAHAL_DBG_LOW("Entry - HW_TYPE=%d\n", ipa_hw_type);

	if ((ipa_hw_type < 0) || (ipa_hw_type >= IPA_HW_MAX)) {
		IPAHAL_ERR("invalid IPA HW type (%d)\n", ipa_hw_type);
		return -EINVAL;
	}

	memset(&zero_obj, 0, sizeof(zero_obj));
	for (i = IPA_HW_v4_0 ; i < ipa_hw_type ; i++) {
		for (j = 0; j < IPAHAL_HW_STATS_MAX; j++) {
			if (!memcmp(&ipahal_hw_stats_objs[i + 1][j], &zero_obj,
				sizeof(struct ipahal_hw_stats_obj))) {
				memcpy(&ipahal_hw_stats_objs[i + 1][j],
					&ipahal_hw_stats_objs[i][j],
					sizeof(struct ipahal_hw_stats_obj));
			} else {
				/*
				 * explicitly overridden stat.
				 * Check validity
				 */
				hw_stat_ptr = &ipahal_hw_stats_objs[i + 1][j];
				if (!hw_stat_ptr->get_offset) {
					IPAHAL_ERR(
					  "stat=%d get_offset null ver=%d\n",
					  j, i+1);
					WARN_ON(1);
				}
				if (!hw_stat_ptr->parse_stats) {
					IPAHAL_ERR(
					  "stat=%d parse_stats null ver=%d\n",
						j, i + 1);
					WARN_ON(1);
				}
			}
		}
	}

	return 0;
}

int ipahal_stats_get_offset(enum ipahal_hw_stats_type type, void *params,
	struct ipahal_stats_offset *out)
{
	if (type < 0 || type >= IPAHAL_HW_STATS_MAX) {
		IPAHAL_ERR("Invalid type stat=%d\n", type);
		WARN_ON(1);
		return -EFAULT;
	}

	if (!params || !out) {
		IPAHAL_ERR("Null arg\n");
		WARN_ON(1);
		return -EFAULT;
	}

	return ipahal_hw_stats_objs[ipahal_ctx->hw_type][type].get_offset(
		params, out);
}

struct ipahal_stats_init_pyld *ipahal_stats_generate_init_pyld(
	enum ipahal_hw_stats_type type, void *params, bool is_atomic_ctx)
{
	struct ipahal_hw_stats_obj *hw_obj_ptr;

	if (type < 0 || type >= IPAHAL_HW_STATS_MAX) {
		IPAHAL_ERR("Invalid type stat=%d\n", type);
		WARN_ON(1);
		return NULL;
	}

	hw_obj_ptr = &ipahal_hw_stats_objs[ipahal_ctx->hw_type][type];
	return hw_obj_ptr->generate_init_pyld(params, is_atomic_ctx);
}

int ipahal_parse_stats(enum ipahal_hw_stats_type type, void *init_params,
	void *raw_stats, void *parsed_stats)
{
	if (WARN((type < 0 || type >= IPAHAL_HW_STATS_MAX),
		"Invalid type stat = %d\n", type))
		return -EFAULT;

	if (WARN((!raw_stats || !parsed_stats), "Null arg\n"))
		return -EFAULT;

	return ipahal_hw_stats_objs[ipahal_ctx->hw_type][type].parse_stats(
		init_params, raw_stats, parsed_stats);
}

void ipahal_set_flt_rt_sw_stats(void *raw_stats,
	struct ipa_flt_rt_stats sw_stats)
{
	struct ipahal_stats_flt_rt_v4_5_hw *raw_hw =
		(struct ipahal_stats_flt_rt_v4_5_hw *)raw_stats;

	IPAHAL_DBG_LOW("\n");
	raw_hw->num_bytes = sw_stats.num_bytes;
	raw_hw->num_packets_hash = sw_stats.num_pkts_hash;
	raw_hw->num_packets = sw_stats.num_pkts;
}

void ipahal_set_nat_ct_sw_stats(void *raw_stats,
	struct ipahal_stats_nat_ct sw_stats)
{
	struct ipahal_stats_nat_ct_hw *raw_hw =
		(struct ipahal_stats_nat_ct_hw *)raw_stats;

	IPAHAL_DBG_LOW("\n");
	raw_hw->num_pkts_inbound = sw_stats.num_pkts_inbound;
	raw_hw->num_pkts_cache_inbound = sw_stats.num_pkts_cache_inbound;
	raw_hw->num_bytes_inbound = sw_stats.num_bytes_inbound;
	raw_hw->num_pkts_outbound = sw_stats.num_pkts_outbound;
	raw_hw->num_pkts_cache_outbound = sw_stats.num_pkts_cache_outbound;
	raw_hw->num_bytes_outbound = sw_stats.num_bytes_outbound;
}

u32 ipahal_stats_get_hw_entry_size(enum ipahal_hw_stats_type type)
{
	switch (type) {
	case IPAHAL_HW_STATS_QUOTA:
		return sizeof(struct ipahal_stats_quota_hw);
	case IPAHAL_HW_STATS_TETHERING:
		return sizeof(struct ipahal_stats_tethering_hw);
	case IPAHAL_HW_STATS_FNR:
		/*
		 * The FnR counter layout grew at v4_5 (a num_bytes field was
		 * added); pre-v4_5 HW uses the narrower entry.
		 */
		if (ipahal_ctx->hw_type < IPA_HW_v4_5)
			return sizeof(struct ipahal_stats_flt_rt_hw);
		return sizeof(struct ipahal_stats_flt_rt_v4_5_hw);
	case IPAHAL_HW_STATS_DROP:
		return sizeof(struct ipahal_stats_drop_hw);
	case IPAHAL_HW_STATS_NAT_CT:
		return sizeof(struct ipahal_stats_nat_ct_hw);
	default:
		IPAHAL_ERR("unexpected stats type %d\n", type);
		return 0;
	}
}
