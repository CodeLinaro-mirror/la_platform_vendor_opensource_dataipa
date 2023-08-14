// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */

/*
 * This module implements IPA IPsec offload interface.
 *
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ipa.h>
#include <net/xfrm.h>
#include "ipa_i.h"

bool ipa_ipsec_enabled(void)
{
	return !!ipa3_ctx->ipsec;
}

/*
 * ipa_ipsec_install_key()
 * 	Install a key.
 */
int ipa_ipsec_install_key(u8 idx, enum ipa_ipsec_key_type type, enum ipa_ipsec_key_len len_code, void *key)
{
	struct ipa_ipsec_key_store *keys;
	size_t len;

	if (!ipa3_ctx->ipsec || !ipa3_ctx->ipsec->keys)
		return -EFAULT;

	if (idx >= IPA_IPSEC_MAX_KEY_NUM)
		return -EINVAL;

	if (len_code == 0)
		return 0;

	keys = ipa3_ctx->ipsec->keys;

	switch (type) {
	case IPA_IPSEC_KEY_ENC:
		switch (len_code) {
		case IPA_IPSEC_KEY_128:
			len = 16;
			break;
		case IPA_IPSEC_KEY_256:
			len = 32;
			break;
		default:
			return -EINVAL;
		}
		memcpy_toio((void __iomem *)&keys->enc[idx], key, len);
		break;
	case IPA_IPSEC_KEY_AUTH:
		switch (len_code) {
		case IPA_IPSEC_KEY_128:
			len = 16;
			break;
		case IPA_IPSEC_KEY_256:
			len = 32;
			break;
		case IPA_IPSEC_KEY_512:
			len = 64;
			break;
		case IPA_IPSEC_KEY_160:
			len = 20;
			break;
		case IPA_IPSEC_KEY_384:
			len = 48;
			break;
		default:
			return -EINVAL;
		}
		memcpy_toio((void __iomem *)&keys->auth[idx], key, len);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

/*
 * ipa_ipsec_install_encap_sa()
 * 	Install an encap SA.
 */
int ipa_ipsec_install_encap_sa(u8 idx, struct ipa_ipsec_sa_encap *sa)
{
	if (!sa || idx >= IPA_IPSEC_MAX_SA_NUM)
		return -EINVAL;

	if (!ipa3_ctx->ipsec || !ipa3_ctx->ipsec->encap)
		return -EFAULT;

	/* memcopy to SRAM */
	memcpy_toio((void __iomem *)(ipa3_ctx->ipsec->encap + idx), (void *)sa,
		sizeof(struct ipa_ipsec_sa_encap));

	return 0;
}

/*
 * ipa_ipsec_install_decap_sa()
 * 	Install an decap SA.
 */
int ipa_ipsec_install_decap_sa(u8 idx, struct ipa_ipsec_sa_decap *sa)
{
	if (!sa || idx >= IPA_IPSEC_MAX_SA_NUM)
		return -EINVAL;

	if (!ipa3_ctx->ipsec || !ipa3_ctx->ipsec->decap)
		return -EFAULT;

	/* memcopy to SRAM */
	memcpy_toio((void __iomem *)(ipa3_ctx->ipsec->decap + idx), (void *)sa,
		sizeof(struct ipa_ipsec_sa_decap));

	return 0;
}

/*
 * ipa_ipsec_stop_sa()
 * 	Stop an SA.
 */
int ipa_ipsec_stop_sa(u8 idx, enum ipa_ipsec_sa_type sa_type)
{
	if (sa_type >= IPA_IPSEC_TYPE_MAX || idx >= IPA_IPSEC_MAX_SA_NUM)
		return -EINVAL;

	if (!ipa3_ctx->ipsec || !ipa3_ctx->ipsec->encap)
		return -EFAULT;

	switch (sa_type) {
	case IPA_IPSEC_ENCAP:
		ipa3_ctx->ipsec->encap[idx].dyna.is_stopped = 1;
		break;
	case IPA_IPSEC_DECAP:
		ipa3_ctx->ipsec->decap[idx].dyna.is_stopped = 1;
		break;
	default:
		break;
	}

	return 0;
}


int ipa_ipsec_ep_init_prod(void)
{
	u32 clnt_hdl;
	static bool allocated = false;
	struct ipa_sys_connect_params sys_in;

	IPADBG("Start\n");

	/* This may be called number of times from rmnet_ipa */
	if (allocated)
		return 0;

	/* IPsec encap (AP->IPA) */
	memset(&sys_in, 0, sizeof(struct ipa_sys_connect_params));
	sys_in.client = IPA_CLIENT_IPSEC_ENCAP_PROD;
	sys_in.desc_fifo_sz = IPA_SYS_TX_DATA_DESC_FIFO_SZ;
	sys_in.ipa_ep_cfg.mode.mode = IPA_BASIC;
	sys_in.ipa_ep_cfg.mode.dst = IPA_CLIENT_IPSEC_ENCAP_PROD;
	sys_in.ipa_ep_cfg.cfg.cs_offload_en = IPA_DISABLE_CS_OFFLOAD;
	sys_in.ipa_ep_cfg.aggr.aggr_en = IPA_BYPASS_AGGR;
	sys_in.ipa_ep_cfg.hdr.hdr_len = 4;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_metadata_valid = 1;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_metadata = 0;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_pkt_size = 2;
	sys_in.notify = apps_ipa_tx_complete_notify;
	sys_in.priv = ipa3_ctx->ipsec->dev;
	sys_in.ext_ioctl_v2 = true;
	sys_in.int_modt = 16;
	sys_in.int_modc = 20;
	if (ipa3_setup_sys_pipe(&sys_in, &clnt_hdl)) {
		IPAERR(":setup sys pipe (%s) failed.\n", ipa_clients_strings[sys_in.client]);
		return -EPERM;
	}

	/* IPsec decap (AP->IPA) */
	memset(&sys_in, 0, sizeof(struct ipa_sys_connect_params));
	sys_in.client = IPA_CLIENT_IPSEC_DECAP_PROD;
	sys_in.desc_fifo_sz = IPA_SYS_TX_DATA_DESC_FIFO_SZ;
	sys_in.ipa_ep_cfg.mode.mode = IPA_BASIC;
	sys_in.ipa_ep_cfg.mode.dst = IPA_CLIENT_IPSEC_DECAP_PROD;
	sys_in.ipa_ep_cfg.cfg.cs_offload_en = IPA_DISABLE_CS_OFFLOAD;
	sys_in.ipa_ep_cfg.aggr.aggr_en = IPA_BYPASS_AGGR;
	sys_in.ipa_ep_cfg.hdr.hdr_len = 0;
	sys_in.notify = apps_ipa_tx_complete_notify;
	sys_in.priv = ipa3_ctx->ipsec->dev;
	sys_in.ext_ioctl_v2 = true;
	sys_in.int_modt = 16;
	sys_in.int_modc = 20;
	if (ipa3_setup_sys_pipe(&sys_in, &clnt_hdl)) {
		IPAERR(":setup sys pipe (%s) failed.\n", ipa_clients_strings[sys_in.client]);
		return -EPERM;
	}

	allocated = true;
	return 0;
}
EXPORT_SYMBOL(ipa_ipsec_ep_init_prod);

int ipa_ipsec_ep_init_cons(void)
{
	u32 clnt_hdl;
	static bool allocated = false;
	struct ipa_sys_connect_params sys_in;

	IPADBG("Start\n");

	/* This may be called number of times from rmnet_ipa */
	if (allocated)
		return 0;

	WARN_ON(!ipa3_ctx->uc_ctx.uc_loaded);
	if (!ipa3_ctx->uc_ctx.uc_event_ring_valid && ipa3_uc_setup_event_ring()) {
		IPAERR("failed to set uc_event ring\n");
		return -EFAULT;
	}

	memset(&sys_in, 0, sizeof(struct ipa_sys_connect_params));

	/* IPsec decap recoverable error (IPA->AP) */
	sys_in.client = IPA_CLIENT_IPSEC_DECAP_RECOVERABLE_ERR_CONS;
	sys_in.notify = apps_ipa_packet_receive_notify;
	sys_in.priv = ipa3_ctx->ipsec->dev;
	sys_in.desc_fifo_sz = 256 * IPA_FIFO_ELEMENT_SIZE;
	sys_in.ext_ioctl_v2 = true;
	sys_in.int_modt = 16;
	sys_in.int_modc = 20;
	sys_in.buff_size = 4096;
	sys_in.bypass_agg = true;
	sys_in.ipa_ep_cfg.hdr.hdr_len = 12;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_metadata_valid = 1;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_metadata = 9;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_pkt_size_valid = 1;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_pkt_size = 2;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_total_len_or_pad_valid = true;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_total_len_or_pad = 0;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_payload_len_inc_padding = true;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_total_len_or_pad_offset = 0;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_little_endian = 0;
	sys_in.ipa_ep_cfg.metadata_mask.metadata_mask = 0xF;
	sys_in.ipa_ep_cfg.prod_cfg.error_qmap_en = true;
	if (ipa3_setup_sys_pipe(&sys_in, &clnt_hdl)) {
		IPAERR(":setup sys pipe (%s) failed.\n", ipa_clients_strings[sys_in.client]);
		return -EPERM;
	}

	/* IPsec decap non-recoverable error (IPA->AP) */
	sys_in.client = IPA_CLIENT_IPSEC_DECAP_NON_RECOVERABLE_ERR_CONS;
	if (ipa3_setup_sys_pipe(&sys_in, &clnt_hdl)) {
		IPAERR(":setup sys pipe (%s) failed.\n", ipa_clients_strings[sys_in.client]);
		return -EPERM;
	}

	/* IPsec encap error (IPA->AP) */
	sys_in.client = IPA_CLIENT_IPSEC_ENCAP_ERR_CONS;
	if (ipa3_setup_sys_pipe(&sys_in, &clnt_hdl)) {
		IPAERR(":setup sys pipe (%s) failed.\n", ipa_clients_strings[sys_in.client]);
		return -EPERM;
	}

	/* IPsec embedded RX (IPA->AP) */
	memset(&sys_in, 0, sizeof(struct ipa_sys_connect_params));
	sys_in.client = IPA_CLIENT_IPSEC_APPS_WAN_CONS;
	sys_in.notify = apps_ipa_packet_receive_notify;
	sys_in.priv = ipa3_ctx->ipsec->dev;
	sys_in.desc_fifo_sz = 256 * IPA_FIFO_ELEMENT_SIZE;
	sys_in.ext_ioctl_v2 = true;
	sys_in.int_modt = 16;
	sys_in.int_modc = 20;
	sys_in.buff_size = 4096;
	sys_in.bypass_agg = true;
	sys_in.ipa_ep_cfg.hdr.hdr_len = 8;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_metadata_valid = 1;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_metadata = 1;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_pkt_size_valid = 1;
	sys_in.ipa_ep_cfg.hdr.hdr_ofst_pkt_size = 2;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_total_len_or_pad_valid = true;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_total_len_or_pad = 0;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_payload_len_inc_padding = true;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_total_len_or_pad_offset = 0;
	sys_in.ipa_ep_cfg.hdr_ext.hdr_little_endian = 0;
	sys_in.ipa_ep_cfg.metadata_mask.metadata_mask = 0xFF000000;
	if (ipa3_setup_sys_pipe(&sys_in, &clnt_hdl)) {
		IPAERR(":setup sys pipe (%s) failed.\n", ipa_clients_strings[sys_in.client]);
		return -EPERM;
	}

	allocated = true;
	return 0;
}
EXPORT_SYMBOL(ipa_ipsec_ep_init_cons);

/*
 * ipa_ipsec_init()
 */
int ipa_ipsec_init(void)
{
	int ret, n;
	u32 keys_phys_base, sa_phys_base, ipsec_ep_cfg;
	void __iomem *key_mmio, __iomem *sa_mmio;

	IPADBG("IPA IPsec entry\n");

	if (!ipa3_ctx || !ipa3_ctx->ctrl) {
		IPAERR("Main IPA driver is not ready yet.\n");
		return -EAGAIN;
	}

	if (ipa3_ctx->ipsec) {
		IPAERR("Already initialyzed\n");
		return -EEXIST;
	}

	/* Allocate context */
	ipa3_ctx->ipsec = kzalloc(sizeof(struct ipa_ipsec_ctx), GFP_KERNEL);
	if (!ipa3_ctx->ipsec) {
		IPAERR("Failed allocatng IPsec context.\n");
		return -ENOMEM;
	}


	/* Map IPA IPsec Key SRAM */
	keys_phys_base = ipa3_ctx->ipa_wrapper_base + ipa3_ctx->ctrl->ipa_reg_base_ofst +
		ipahal_get_reg_n_ofst(IPA_IPSEC_AREA_RAM_DIRECT_ACCESS_n, 0);

	key_mmio = ioremap((phys_addr_t)keys_phys_base, sizeof(struct ipa_ipsec_key_store));
	if (!key_mmio) {
		IPAERR("Failed mapping IPsec key SRAM.\n");
		ret = -ENOMEM;
		goto free_ctx;
	}

	IPADBG_LOW("keys_phys_base 0x%08X key_mmio=0x%X\n", keys_phys_base, key_mmio);
	memset_io(key_mmio, 0, sizeof(struct ipa_ipsec_key_store));
	ipa3_ctx->ipsec->keys = (struct ipa_ipsec_key_store *)key_mmio;

	/* Compile time check the SA SRAM partition size and alignment */
	BUG_ON(IPA_MEM_PART(sa_contexts_size) < IPA_SA_DB_SIZE ||
		IPA_MEM_PART(sa_contexts_ofst) & 0x3);

	/* Map the SA database SRAM region */
	sa_phys_base = ipa3_ctx->ipa_wrapper_base +
		ipa3_ctx->ctrl->ipa_reg_base_ofst +
		ipahal_get_reg_n_ofst(IPA_SW_AREA_RAM_DIRECT_ACCESS_n,
			ipa3_ctx->smem_restricted_bytes / 4) +
		IPA_MEM_PART(sa_contexts_ofst);

	sa_mmio = ioremap((phys_addr_t)sa_phys_base, IPA_SA_DB_SIZE);
	if (!sa_mmio) {
		IPAERR("Failed mapping IPsec SA SRAM.\n");
		ret = -ENOMEM;
		goto unmap_keys;
	}

	/* Zero the SA SRAM */
	memset_io(sa_mmio, 0, IPA_SA_DB_SIZE);

	IPADBG_LOW("sa_phys_base 0x%08X sa_mmio=0x%X\n", sa_phys_base, sa_mmio);
	ipa3_ctx->ipsec->decap = (struct ipa_ipsec_sa_decap *)sa_mmio;
	ipa3_ctx->ipsec->encap =
		(struct ipa_ipsec_sa_encap *)(ipa3_ctx->ipsec->decap + IPA_IPSEC_MAX_SA_NUM);
	IPADBG_LOW("ipa3_ctx->ipsec->decap=0x%08X ipa3_ctx->ipsec->encap=0x%X\n",
		ipa3_ctx->ipsec->decap, ipa3_ctx->ipsec->encap);

	/*
	 * Configure IPA_IPSEC_SA_DECAPSULATION_BASE with address in SW SRAM address space.
	 * This register points to the start of the decapsulation SAs.
	 */
	ipahal_write_reg(IPA_IPSEC_SA_DECAPSULATION_BASE, IPA_MEM_PART(sa_contexts_ofst));

	/*
	 * Configure IPA_IPSEC_SA_ENCAPSULATION_BASE with address in SW SRAM address space.
	 * This register points to the start of the encapsulation SAs.
	 */
	ipahal_write_reg(IPA_IPSEC_SA_ENCAPSULATION_BASE, IPA_MEM_PART(sa_contexts_ofst) + IPA_DECAP_DB_SIZE);





	/*
	 * Configure IPA_ENDP_INIT_IPSEC_CFG_%n%::EXCEPTION_ENDP_IPSEC_POST_ENCAPS
	 * with post encapsulation exception pipe.
	 * All packets that have been successfully encapsulated but received a HW Exception or
	 * Packet error will be routed to the pipe, and reverted to the last snapshot.
	 */

	/*
	 * Configure IPA_ENDP_INIT_IPSEC_CFG_%n%:
	 *  - EXCEPTION_ENDP_IPSEC_DECAP field with exception producer to route IPsec packets
	 *  that failed in side DECAPS acl.
	 *  - EXCEPTION_ENDP_IPSEC_POST_DECAPS field with Indicates exception producer
	 *  to route IPsec packets that have passed DECAPS acl.
	 *  - DECAPS_NEXT_HDR_CHECK_DISABLE field with 1 to disable checking
	 *  for Next-header errors for traffic from matching consumer.
	 */
	ipsec_ep_cfg = (u32)(ipa3_get_ep_mapping(IPA_CLIENT_IPSEC_DECAP_NON_RECOVERABLE_ERR_CONS)) |
		(u32)(ipa3_get_ep_mapping(IPA_CLIENT_IPSEC_DECAP_RECOVERABLE_ERR_CONS)) << 8 |
		(u32)(ipa3_get_ep_mapping(IPA_CLIENT_IPSEC_ENCAP_ERR_CONS)) << 16;
	for (n = 0; n < IPA6_PROD_PIPES_NUM; n++) {
		ipahal_write_reg_n(IPA_ENDP_INIT_IPSEC_CFG_n, n, ipsec_ep_cfg);
	}


	return 0;

	iounmap(sa_mmio);
unmap_keys:
	iounmap(key_mmio);
free_ctx:
	kfree(ipa3_ctx->ipsec);
	ipa3_ctx->ipsec = NULL;

	return ret;
}

