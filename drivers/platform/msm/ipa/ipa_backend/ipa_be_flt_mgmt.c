//SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include <linux/string.h>
#include <linux/jhash.h>
#include <linux/atomic.h>
#include <linux/bitops.h>
#include <linux/sort.h>
#include <linux/if_vlan.h>
#include <linux/msm_ipa.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <linux/notifier.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <net/addrconf.h>
#include <net/if_inet6.h>
#include <net/ipv6.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/ipa.h>
#include "ipa_api.h"
#include "ipa_be.h"
#include "ipa_be_clientdb.h"
#include "ipa_be_flt_mgmt.h"
#include "ipa_i.h"

#define IPA_MAX_FLT_RULE 100

#define IPACM_FIREWALL_IPPROTO_TCP 6
#define IPV6_HEADER_SIZE 40
#define IPA_ETHERTYPE_OFFSET_ETH (ETH_HLEN - sizeof(u32))
#define IPA_ETHERTYPE_OFFSET_VLAN (IPA_ETHERTYPE_OFFSET_ETH + VLAN_HLEN)
#define IPA_ETHERTYPE_OFFSET_IP (-sizeof(u32))
#define IPA_ETHERTYPE_MASK GENMASK((ETH_TLEN * BITS_PER_BYTE) - 1, 0)

extern int ipa3_qmi_filter_request_ex_send(
	struct ipa_install_fltr_rule_req_ex_msg_v01 *msg);

extern int ipahal_flt_generate_equation(enum ipa_ip_type ipt,
	const struct ipa_rule_attrib *attrib,
	struct ipa_ipfltri_rule_eq *eq_atrb);

/* Forward declaration: struct ipa_ul_group tracking is defined below this
 * point, but ipa_be_global_qmap_id_reset_if_empty() (defined just above it,
 * near the WAN-up ipa_uplink_pair tracking) needs to test this list's
 * emptiness under its own lock, nested inside ipa_uplink_lock -- declared
 * here (ahead of struct ipa_ul_group itself, which this doesn't need) so
 * that function can reference it directly instead of through a separate,
 * separately-locked helper call.
 */
static LIST_HEAD(ipa_ul_groups_list);
static DEFINE_SPINLOCK(ipa_ul_groups_lock);

/* Helper function to convert addresses to network order */
static inline void addr2network(enum ipa_ip_type addr_type, void *addr)
{
	if (addr_type == IPA_IP_v4 || addr_type == IPA_IP_v6) {
		uint32_t *ptr = (uint32_t *)addr;

		if (addr_type == IPA_IP_v4) {
			ptr[0] = htonl(ptr[0]);
		} else {
			ptr[0] = htonl(ptr[0]);
			ptr[1] = htonl(ptr[1]);
			ptr[2] = htonl(ptr[2]);
			ptr[3] = htonl(ptr[3]);
		}
	}
}

/* Helper function to convert rule attributes to network order */
static int change_to_network_order_kernel(enum ipa_ip_type iptype, struct ipa_rule_attrib *attrib)
{
	if (!attrib) {
		IPA_BE_ERR("Bad attribute pointer is NULL.\n");
		return -EINVAL;
	}

	if (iptype == IPA_IP_v6) {
		addr2network(iptype, attrib->u.v6.src_addr);
		addr2network(iptype, attrib->u.v6.src_addr_mask);
		addr2network(iptype, attrib->u.v6.dst_addr);
		addr2network(iptype, attrib->u.v6.dst_addr_mask);
	} else {
		IPA_BE_DBG("IP type is not IPv6, do nothing: %d\n", iptype);
	}

	return 0;
}

static enum ipa_filter_action_enum_v01
GetQmiFilterAction(enum ipa_flt_action ipa_filter_action)
{
	switch (ipa_filter_action) {
	case IPA_PASS_TO_ROUTING:
		return QMI_IPA_FILTER_ACTION_ROUTING_V01;
	case IPA_PASS_TO_SRC_NAT:
		return QMI_IPA_FILTER_ACTION_SRC_NAT_V01;
	case IPA_PASS_TO_DST_NAT:
		return QMI_IPA_FILTER_ACTION_DST_NAT_V01;
	case IPA_PASS_TO_EXCEPTION:
		return QMI_IPA_FILTER_ACTION_EXCEPTION_V01;
	default:
		return QMI_IPA_FILTER_ACTION_ROUTING_V01;
	}
}

struct ipa_be_qmi_rule_entry {
	const struct ipa_flt_rule_add *rule;
	uint8_t mux_id;
	bool mux_id_valid;
	enum ipa_ip_type ip_type;
	enum flt_rule_category category;
	enum rule_sub_category sub_category;
	int original_index;
};

static int ipa_be_qmi_rule_cmp(const void *lhs, const void *rhs)
{
	const struct ipa_be_qmi_rule_entry *left = lhs;
	const struct ipa_be_qmi_rule_entry *right = rhs;

	if (left->category != right->category)
		return (int)left->category - (int)right->category;

	if (left->sub_category != right->sub_category)
		return (int)left->sub_category - (int)right->sub_category;

	return left->original_index - right->original_index;
}

static void ipa_be_qmi_rule_swap(void *lhs, void *rhs, int size)
{
	struct ipa_be_qmi_rule_entry tmp;

	if (size != sizeof(tmp))
		return;

	memcpy(&tmp, lhs, sizeof(tmp));
	memcpy(lhs, rhs, sizeof(tmp));
	memcpy(rhs, &tmp, sizeof(tmp));
}

uint32_t wan_ul_fl_rule_hdl_v4[IPA_MAX_NUM_PROPS][MAX_WAN_UL_FILTER_RULES] = {0};
uint32_t num_wan_ul_fl_rule_v4[IPA_MAX_NUM_PROPS] = {0};
uint32_t flt_rule_count_v4[IPA_CLIENT_MAX] = {0};
uint32_t flt_rule_count_v6[IPA_CLIENT_MAX] = {0};

enum flt_rule_type {
	FLT_RULE_TYPE_DEFAULT = 0,
	FLT_RULE_TYPE_ICMP    = 1,
	FLT_RULE_TYPE_CATCHUP = 2,
};

struct flt_rule
{
	struct list_head node;
	struct ipa_flt_rule_add flt_rule;
	uint8_t mux_id;
	enum flt_rule_type rule_type;
	enum flt_rule_category category;
	enum rule_sub_category sub_category;
	int pdn_iface;
};

static LIST_HEAD(Pdn_flt_rule_v4_list);
static LIST_HEAD(Pdn_flt_rule_v6_list);
static DEFINE_SPINLOCK(pdn_flt_rule_v4_lock);
static DEFINE_SPINLOCK(pdn_flt_rule_v6_lock);
uint32_t wan_rule_count_v4 = 0;
uint32_t wan_rule_count_v6 = 0;

/*
 * The DL default rule set is identical for every modem PDN of a family: the
 * match equations carry no IPA_FLT_META_DATA term, so nothing in the set names
 * a PDN. Install it once and let every later PDN take a reference instead of
 * appending its own byte-identical copy -- 16 PDNs would otherwise assemble
 * 192 specs against the QMI_IPA_MAX_FILTERS_EX_V01 limit of 128.
 *
 * The set outlives whichever PDN happened to build it, so its records are
 * tagged with this pdn_iface rather than that PDN's index; tagging them with a
 * real index would let that one PDN's teardown unlink a set the others still
 * need. Both arrays are accessed under the family's pdn_flt_rule_v4_lock /
 * pdn_flt_rule_v6_lock.
 *
 * A set reference is tracked twice: by the counter here and by the owning PDN's
 * dft_rule_installed flag under ipa_pdn_filter_lock. Both must move in one step,
 * or a concurrent add/delete acts on stale state and leaks or drops a reference.
 * The same applies to every rule record: it is appended to a family list under
 * that family's lock while its owning entry is marked under ipa_pdn_filter_lock,
 * and an entry freed between the two leaves a record no delete path can reach.
 * Lock order, never the reverse:
 *
 *	ipa_pdn_filter_lock  ->  pdn_flt_rule_v4_lock / pdn_flt_rule_v6_lock
 *
 * Releasing the outer lock while holding the inner one is fine. The delete paths
 * for icmp and catchup rules still take the two locks in the opposite sequence
 * without overlapping -- safe, but they must not become a nest. The per-family
 * ordered workqueues in ipa_be.c serialise in-tree callers, but these symbols
 * are exported -- do not rely on it.
 */
#define IPA_BE_DFT_SET_PDN_IFACE (-1)
static bool ipa_be_dft_set_installed[IPA_IP_MAX];
static int ipa_be_dft_set_ref_count[IPA_IP_MAX];

/* Structure to track installed MTU rule pairs and their usage count */
struct ipa_mtu_rule_pair {
	struct list_head node;
	int client_iface;
	int pdn_iface;
	enum ipa_ip_type ip_type;
	atomic_t ref_count;
};

static LIST_HEAD(ipa_mtu_rule_pairs_list);
static DEFINE_SPINLOCK(ipa_mtu_rule_lock);

/* Structure to track installed uplink interface pairs and their usage count */
struct ipa_uplink_pair {
	struct list_head node;
	int pdn_iface;
	int client_iface;
	int src_pipe;
	enum ipa_ip_type ip_type;  /* Track IP version separately */
	atomic_t ref_count;
	bool installing;
	uint8_t notif_mux_id;
	/* Cached at install time; avoids ipa3_query_iface() + rx_prop walk in
	 * the delete-notify path (~80ms under contended mutex).
	 */
	uint32_t notif_src_pipe;
	uint32_t notif_ebd_pipe;
	enum ipa_client_type notif_src_client;
};

static LIST_HEAD(ipa_uplink_pairs_list);
static DEFINE_SPINLOCK(ipa_uplink_lock);
static DECLARE_WAIT_QUEUE_HEAD(ipa_uplink_wq);

/* wait_event predicate: re-locks and re-walks to avoid stale pointer on free. */
static bool ipa_uplink_pair_settled(int pdn_iface, int src_pipe,
				    enum ipa_ip_type ip_type)
{
	struct ipa_uplink_pair *pair;
	bool settled = true;

	spin_lock_bh(&ipa_uplink_lock);
	list_for_each_entry(pair, &ipa_uplink_pairs_list, node) {
		if (pair->pdn_iface == pdn_iface &&
		    pair->src_pipe == src_pipe &&
		    pair->ip_type == ip_type) {
			settled = !pair->installing;
			break;
		}
	}
	spin_unlock_bh(&ipa_uplink_lock);
	return settled;
}

/* Global mux_id latched from the first UL install; reset when all pairs are gone.
 * 0xFF = not yet captured.
 */
#define IPA_BE_GLOBAL_QMAP_UNSET 0xFF
static uint8_t ipa_be_global_qmap_id = IPA_BE_GLOBAL_QMAP_UNSET;

static uint8_t ipa_be_global_qmap_id_latch(uint8_t candidate)
{
	uint8_t latched;

	spin_lock_bh(&ipa_uplink_lock);
	if (ipa_be_global_qmap_id == IPA_BE_GLOBAL_QMAP_UNSET)
		ipa_be_global_qmap_id = candidate;
	latched = ipa_be_global_qmap_id;
	spin_unlock_bh(&ipa_uplink_lock);

	return latched;
}

/* The latch is only ever written by the ECM uplink add path below, whose
 * tracking lives on ipa_ul_groups_list (not ipa_uplink_pairs_list, which
 * backs the unrelated WAN-up/down eth-backhaul feature and never touches
 * this latch) -- so emptiness is judged against ipa_ul_groups_list.
 *
 * Takes ipa_uplink_lock *outer* and ipa_ul_groups_lock *inner* so the
 * emptiness check and the reset happen as one atomic step -- checking
 * emptiness via a separate ipa_ul_groups_all_empty() call first (as this
 * used to) leaves a window where a concurrent ipa_ul_group_acquire() +
 * ipa_be_global_qmap_id_latch() on another CPU can latch a real mux_id
 * right after this snapshot, which this function would then stomp back to
 * UNSET. Safe to nest this way: ipa_be_global_qmap_id_latch() never holds
 * ipa_ul_groups_lock while taking ipa_uplink_lock, so there's no reverse
 * ordering anywhere to deadlock against.
 */
static void ipa_be_global_qmap_id_reset_if_empty(void)
{
	spin_lock_bh(&ipa_uplink_lock);
	spin_lock_bh(&ipa_ul_groups_lock);
	if (list_empty(&ipa_ul_groups_list))
		ipa_be_global_qmap_id = IPA_BE_GLOBAL_QMAP_UNSET;
	spin_unlock_bh(&ipa_ul_groups_lock);
	spin_unlock_bh(&ipa_uplink_lock);
}

/* True once any WAN PDN has latched the global mux_id. */
bool ipa_be_wan_pdn_active(void)
{
	bool active;

	spin_lock_bh(&ipa_uplink_lock);
	active = ipa_be_global_qmap_id != IPA_BE_GLOBAL_QMAP_UNSET;
	spin_unlock_bh(&ipa_uplink_lock);

	return active;
}
EXPORT_SYMBOL(ipa_be_wan_pdn_active);

static inline bool ipa_uplink_pair_match(const struct ipa_uplink_pair *p,
		int pdn_iface, int client_iface, enum ipa_ip_type ip_type)
{
	return p->pdn_iface == pdn_iface &&
	       p->client_iface == client_iface &&
	       p->ip_type == ip_type;
}

/*
 * Tracking for the ECM-connect modem (Q6) uplink filter rules installed by
 * ipa_be_v{4,6}_add_uplink_filter_rule() / deleted by
 * ipa_be_v{4,6}_delete_uplink_filter_rule(). Deliberately a separate
 * list/lock from struct ipa_uplink_pair above, which backs the unrelated
 * eth-backhaul rule in ipa_be_handle_wan_up()/_down() (keyed by pdn_iface/
 * src_pipe/ip_type -- different lifetime, different key).
 *
 * A group is keyed by (client_iface, ip_type, cat, keyed_by_vlan, vlan_id).
 * Non-XLAT uplink rows are PDN-independent, so they're installed once per
 * (client_iface, ip_type) and shared by every attaching PDN
 * (keyed_by_vlan=false). The v4 XLAT row's match equation carries a VLAN
 * tag, so it's genuinely per-VLAN (keyed_by_vlan=true per vlan_id, or the
 * IPA_UL_VLAN_NONE sentinel). The v6 XLAT companion matches by EtherType,
 * not VLAN, so it's shared across all VLANs/PDNs on a client
 * (keyed_by_vlan=false, cat=CAT_XLAT).
 *
 * Each group tracks member PDNs individually (struct ipa_ul_pdn_ref) so a
 * PDN joining an already-installed group still gets its own QMI notify
 * without a HW reinstall; install/teardown is always driven by the group's
 * aggregate ref_count, never any single PDN's count.
 */
/* Sentinel vlan_id for the non-VLAN XLAT group (reuses the existing "no
 * VLAN configured" constant).
 */
#define IPA_UL_VLAN_NONE IPA_VLAN_ID_NOT_CONFIGURED

/* One PDN's membership in a group: its own flow ref_count and cached mux_id. */
struct ipa_ul_pdn_ref {
	struct list_head node;
	int pdn_iface;
	atomic_t ref_count;	/* flows from this pdn under the group */
	uint8_t mux_id;		/* cached from this pdn's ext_prop, for notify */
};

/* One rule_id this group owns, cached so a later notify-only call can
 * resend the install notify without rebuilding the HW table.
 */
struct ipa_ul_rule_id_node {
	struct list_head node;
	uint32_t rule_id;
};

/* One ext_prop row's assigned HW ordering priority, keyed by rule_id. */
struct ipa_ul_prio_node {
	struct list_head node;
	uint32_t rule_id;	/* which ext_prop row this applies to */
	uint8_t sub_cat;	/* assigned enum rule_sub_category value */
};

struct ipa_ul_group {
	struct list_head node;
	int client_iface;
	enum ipa_ip_type ip_type;
	enum flt_rule_category cat;	/* CAT_UPLINK or CAT_XLAT */
	bool keyed_by_vlan;		/* true only for the v4 per-VLAN XLAT row */
	uint16_t vlan_id;		/* valid iff keyed_by_vlan; else unused */
	atomic_t ref_count;		/* aggregate across pdn_list; drives install/teardown */
	bool installing;
	struct list_head pdn_list;	/* struct ipa_ul_pdn_ref */
	uint32_t *flt_hdls;		/* exact handles owned by this group */
	int num_hdls;
	struct list_head rule_id_list;	/* struct ipa_ul_rule_id_node; cached, for repeat notify */
	uint32_t notif_src_pipe;
	uint32_t notif_ebd_pipe;
	enum ipa_client_type notif_src_client;
	/* Modem expects uplink rows in increasing rule_id order. Populated on
	 * a base group at build time for every ext_prop row of this ip_type,
	 * including is_xlat_rule rows that actually belong to a separate
	 * per-VLAN group: that group, built standalone later, looks up its
	 * assigned sub_category here (ipa_ul_group_lookup_sub_cat()) to place
	 * its row(s) in the correct relative HW position without a rebuild.
	 */
	struct list_head prio_list;	/* struct ipa_ul_prio_node */
};

/* ipa_ul_groups_list/ipa_ul_groups_lock are declared earlier in this file
 * (ahead of ipa_be_global_qmap_id_reset_if_empty(), which needs them) --
 * only the wait queue is declared here, next to the struct it waits on.
 */
static DECLARE_WAIT_QUEUE_HEAD(ipa_ul_groups_wq);

static inline bool ipa_ul_group_match(const struct ipa_ul_group *g,
		int client_iface, enum ipa_ip_type ip_type,
		enum flt_rule_category cat, bool keyed_by_vlan, uint16_t vlan_id)
{
	if (g->client_iface != client_iface || g->ip_type != ip_type ||
	    g->cat != cat || g->keyed_by_vlan != keyed_by_vlan)
		return false;
	return !keyed_by_vlan || g->vlan_id == vlan_id;
}

/* Caller must hold ipa_ul_groups_lock. */
static struct ipa_ul_group *ipa_find_ul_group(int client_iface,
		enum ipa_ip_type ip_type, enum flt_rule_category cat,
		bool keyed_by_vlan, uint16_t vlan_id)
{
	struct ipa_ul_group *g;

	list_for_each_entry(g, &ipa_ul_groups_list, node) {
		if (ipa_ul_group_match(g, client_iface, ip_type, cat,
					keyed_by_vlan, vlan_id))
			return g;
	}
	return NULL;
}

/* Caller must hold ipa_ul_groups_lock. */
static struct ipa_ul_pdn_ref *ipa_find_ul_pdn_ref(struct ipa_ul_group *g,
		int pdn_iface)
{
	struct ipa_ul_pdn_ref *p;

	list_for_each_entry(p, &g->pdn_list, node) {
		if (p->pdn_iface == pdn_iface)
			return p;
	}
	return NULL;
}

/*
 * Looks up whether pdn_iface currently has a pdn_ref under ANY group
 * matching (client_iface, ip_type) -- i.e. consolidated membership across
 * base + every per-VLAN XLAT group of that type, not any single group.
 * When found and the out pointers are non-NULL, also returns that pdn's
 * cached mux_id and the shared pipe info (same pipe for every group of
 * this client_iface, by construction).
 */
static bool ipa_ul_client_pdn_lookup(int client_iface, enum ipa_ip_type ip_type,
	int pdn_iface, uint8_t *out_mux_id, uint32_t *out_src_pipe, uint32_t *out_ebd_pipe)
{
	struct ipa_ul_group *g;
	struct ipa_ul_pdn_ref *p;
	bool found = false;

	spin_lock_bh(&ipa_ul_groups_lock);
	list_for_each_entry(g, &ipa_ul_groups_list, node) {
		if (g->client_iface != client_iface || g->ip_type != ip_type)
			continue;
		if (g->installing)
			continue;
		p = ipa_find_ul_pdn_ref(g, pdn_iface);
		if (p) {
			if (out_mux_id)
				*out_mux_id = p->mux_id;
			if (out_src_pipe)
				*out_src_pipe = g->notif_src_pipe;
			if (out_ebd_pipe)
				*out_ebd_pipe = g->notif_ebd_pipe;
			found = true;
			break;
		}
	}
	spin_unlock_bh(&ipa_ul_groups_lock);
	return found;
}

/*
 * Sends ONE QMI notify for a (client_iface, ip_type) carrying the union of
 * rule_id_list across every matching group. The message replaces the
 * modem's picture for the pipe rather than adding to it, so it must always
 * be the full current set. mux_id is this PDN's own notify mux and is sent
 * as-is; it also drives the global qmap latch as a side effect.
 *
 * Deduplicated by rule_id (the modem's identity for a logical ext_prop
 * row): the per-VLAN v4 XLAT groups all carry the same XLAT rule IDs.
 *
 * No installing=true skip: set_rule_ids() publishes under the lock, so a
 * group is either empty (skipped) or complete. Skipping installing groups
 * suppressed the first notify entirely, as that group is the only match.
 *
 * Carries one source pipe -- all groups of a client share it, since every
 * in-tree client registers one RX prop pair per IP type.
 */
static int ipa_ul_send_client_notify(int client_iface, enum ipa_ip_type ip_type,
	uint8_t mux_id)
{
	struct ipa_fltr_installed_notif_req_msg_v01 flt_index;
	struct ipa_ul_group *g;
	struct ipa_ul_rule_id_node *node;
	int n = 0;
	bool got_pipe = false;

	memset(&flt_index, 0, sizeof(flt_index));
	spin_lock_bh(&ipa_ul_groups_lock);
	list_for_each_entry(g, &ipa_ul_groups_list, node) {
		bool truncated = false;

		if (g->client_iface != client_iface || g->ip_type != ip_type)
			continue;
		/* Nothing published yet: no pipe info, nothing to contribute. */
		if (list_empty(&g->rule_id_list))
			continue;
		if (!got_pipe) {
			flt_index.source_pipe_index = g->notif_src_pipe;
			flt_index.embedded_pipe_index = g->notif_ebd_pipe;
			got_pipe = true;
		}
		list_for_each_entry(node, &g->rule_id_list, node) {
			bool dup = false;
			int k;

			for (k = 0; k < n; k++) {
				if (flt_index.rule_id[k] == node->rule_id) {
					dup = true;
					break;
				}
			}
			if (dup)
				continue;
			if (n >= ARRAY_SIZE(flt_index.rule_id)) {
				truncated = true;
				break;
			}
			flt_index.rule_id[n++] = node->rule_id;
		}
		/* Stop entirely, not just this group -- avoids re-logging the
		 * same cap. Unreachable now the union is deduplicated: the
		 * build helpers already reject an ext_prop count over this cap.
		 */
		if (truncated) {
			IPA_BE_ERR("UL notify rule_id[] cap (%zu) exceeded for client %d ip %d; truncating\n",
				ARRAY_SIZE(flt_index.rule_id), client_iface, ip_type);
			break;
		}
	}
	spin_unlock_bh(&ipa_ul_groups_lock);

	if (!got_pipe) {
		IPA_BE_DBG("No uplink groups for client %d ip %d; nothing to notify\n",
			client_iface, ip_type);
		return 0;
	}

	flt_index.install_status = IPA_QMI_RESULT_SUCCESS_V01;
	flt_index.rule_id_valid = 1;
	flt_index.rule_id_len = n;
	flt_index.embedded_pipe_index_valid = 1;
	flt_index.retain_header_valid = 1;
	flt_index.retain_header = 0;
	flt_index.embedded_call_mux_id_valid = 1;
	/* Per-PDN value: this notify is sent on behalf of one PDN and the mux
	 * changes with it, so it must not be replaced by the global latch.
	 * The latch is still driven here -- it is the only place that sets the
	 * global, which ipa_be_wan_pdn_active() reads as a has-any-WAN-PDN
	 * flag -- but its return value is deliberately discarded.
	 */
	(void)ipa_be_global_qmap_id_latch(mux_id);
	flt_index.embedded_call_mux_id = mux_id;

	IPA_BE_DBG("Consolidated UL notify: client %d ip %d src_pipe %u mux %u rules %d\n",
		client_iface, ip_type, flt_index.source_pipe_index, mux_id, n);

	if (ipa3_qmi_filter_notify_send(&flt_index)) {
		IPA_BE_ERR("Consolidated UL notify failed client %d ip %d mux %u\n",
			client_iface, ip_type, mux_id);
		return -EFAULT;
	}
	return 0;
}

/* Frees every node on a rule_id_list, leaving the list empty. */
static void ipa_ul_free_rule_id_list(struct list_head *list)
{
	struct ipa_ul_rule_id_node *node, *tmp;

	list_for_each_entry_safe(node, tmp, list, node) {
		list_del(&node->node);
		kfree(node);
	}
}

/* Frees every node on a prio_list, leaving the list empty. */
static void ipa_ul_free_prio_list(struct list_head *list)
{
	struct ipa_ul_prio_node *node, *tmp;

	list_for_each_entry_safe(node, tmp, list, node) {
		list_del(&node->node);
		kfree(node);
	}
}

/*
 * Replaces g's cached rule_id_list with one node per entry in
 * rule_id[0..count), so a later notify-only call can resend without
 * rebuilding the HW table.
 *
 * Builds into a local list (GFP_KERNEL may sleep, so no lock held), then
 * swaps it in and publishes the notif_* pipe fields in one critical
 * section. A notify walker therefore sees the complete old or complete new
 * state -- never a half-freed list, never one pipe's IDs paired with
 * another's index. This is why notify needs no installing=true skip.
 *
 * All-or-nothing: a partial list would make the next notify advertise a
 * subset of the installed rules, so -ENOMEM leaves g untouched.
 */
static int ipa_ul_group_set_rule_ids(struct ipa_ul_group *g,
	const uint32_t *rule_id, int count, uint32_t src_pipe,
	uint32_t ebd_pipe, enum ipa_client_type src_client)
{
	struct ipa_ul_rule_id_node *node;
	struct list_head fresh, stale;
	int i;

	INIT_LIST_HEAD(&fresh);
	INIT_LIST_HEAD(&stale);

	for (i = 0; i < count; i++) {
		node = kzalloc(sizeof(*node), GFP_KERNEL);
		if (!node) {
			IPA_BE_ERR("Failed to allocate UL rule_id node for rule_id %u\n",
				rule_id[i]);
			ipa_ul_free_rule_id_list(&fresh);
			return -ENOMEM;
		}
		node->rule_id = rule_id[i];
		list_add_tail(&node->node, &fresh);
	}

	spin_lock_bh(&ipa_ul_groups_lock);
	list_splice_init(&g->rule_id_list, &stale);
	list_splice_tail_init(&fresh, &g->rule_id_list);
	g->notif_src_pipe = src_pipe;
	g->notif_ebd_pipe = ebd_pipe;
	g->notif_src_client = src_client;
	spin_unlock_bh(&ipa_ul_groups_lock);

	ipa_ul_free_rule_id_list(&stale);
	return 0;
}

/*
 * Computes a stable rule_id -> sub_category mapping for every ext_prop row
 * of the given ip_type, appended to the caller's prio_list. A base group
 * and its per-VLAN XLAT sibling share one category, so this ordering must
 * be computed once over the full row set and reused by both -- a narrowed
 * subset would give the wrong relative order. Only
 * IPA_RULE_SUB_CAT_MAX - IPA_RULE_PRIORITY_0 priority slots exist (HW/enum
 * constraint); rows beyond that are clamped to the lowest priority.
 *
 * ext_prop rows arrive already in ascending rule_id order (modem/IPACM
 * guarantee), so no sort is needed here. This doesn't replace the driver's
 * own category/sub_category sort (__ipa_add_flt_rule()), which still has
 * to run to order a later call's new row against rows a prior call already
 * installed -- it just makes the redundant in-memory re-sort unnecessary.
 */
static void ipa_be_compute_rule_priorities(
	struct ipa_ioc_query_intf_ext_props *ext_prop, enum ipa_ip_type ip_type,
	struct list_head *prio_list)
{
	const int max_slots = IPA_RULE_SUB_CAT_MAX - IPA_RULE_PRIORITY_0;
	struct ipa_ul_prio_node *new_node;
	int cnt, slot = 0;

	for (cnt = 0; cnt < ext_prop->num_ext_props; cnt++) {
		int this_slot = slot;

		if (ext_prop->ext[cnt].ip != ip_type)
			continue;

		new_node = kzalloc(sizeof(*new_node), GFP_KERNEL);
		if (!new_node) {
			IPA_BE_ERR("Failed to allocate UL priority node for rule_id %u\n",
				ext_prop->ext[cnt].rule_id);
			continue;
		}
		new_node->rule_id = ext_prop->ext[cnt].rule_id;

		if (this_slot >= max_slots) {
			IPA_BE_ERR("UL rule priority slots exhausted (cap %d); clamping rule_id %u to lowest priority\n",
				max_slots, new_node->rule_id);
			this_slot = max_slots - 1;
		}
		new_node->sub_cat = IPA_RULE_PRIORITY_0 + this_slot;
		list_add_tail(&new_node->node, prio_list);
		slot++;
	}
}

/*
 * Looks up the sub_category assigned to rule_id in a base group's cached
 * priority list (see ipa_be_compute_rule_priorities()). Used when building
 * a per-VLAN XLAT group standalone, after the base group already exists,
 * so its row lands in the position it would have had if built together
 * from scratch.
 */
static enum rule_sub_category ipa_ul_group_lookup_sub_cat(
	struct ipa_ul_group *g, uint32_t rule_id)
{
	struct ipa_ul_prio_node *node;

	list_for_each_entry(node, &g->prio_list, node) {
		if (node->rule_id == rule_id)
			return (enum rule_sub_category)node->sub_cat;
	}
	IPA_BE_ERR("No cached priority for rule_id %u in UL group client %d ip %d; falling back to NONE\n",
		rule_id, g->client_iface, g->ip_type);
	return IPA_RULE_SUB_CAT_NONE;
}

/*
 * Same lookup as ipa_ul_group_lookup_sub_cat(), but against a freshly
 * computed local priority list (ipa_be_compute_rule_priorities() output)
 * rather than one already cached on a group -- used while building a
 * group for the first time, before its own prio_list has been populated.
 */
static enum rule_sub_category ipa_be_lookup_local_sub_cat(
	struct list_head *prio_list, uint32_t rule_id)
{
	struct ipa_ul_prio_node *node;

	list_for_each_entry(node, prio_list, node) {
		if (node->rule_id == rule_id)
			return (enum rule_sub_category)node->sub_cat;
	}
	IPA_BE_ERR("No local priority for rule_id %u; falling back to NONE\n",
		rule_id);
	return IPA_RULE_SUB_CAT_NONE;
}

/*
 * Sends the Q6 "this mux_id's uplink rules are gone" notify (rule_id_len=0),
 * scoped to one PDN. Callers send this only after
 * ipa_ul_client_pdn_lookup() confirms the PDN's membership across EVERY
 * group of this client_iface+ip_type has dropped to zero -- one group's
 * ref hitting zero isn't enough, since a sibling group may still hold it.
 * This is the only removal notify sent, even when the group itself is
 * about to be torn down -- ipa_ul_group_teardown() sends none of its own.
 */
static void ipa_ul_send_pdn_removed_notify(uint32_t src_pipe,
	uint32_t ebd_pipe, uint8_t mux_id, int pdn_iface, int client_iface)
{
	struct ipa_fltr_installed_notif_req_msg_v01 flt_index;

	memset(&flt_index, 0, sizeof(flt_index));
	flt_index.source_pipe_index = src_pipe;
	flt_index.install_status = IPA_QMI_RESULT_SUCCESS_V01;
	flt_index.rule_id_valid = 1;
	flt_index.rule_id_len = 0;
	flt_index.embedded_pipe_index_valid = 1;
	flt_index.embedded_pipe_index = ebd_pipe;
	flt_index.retain_header_valid = 1;
	flt_index.retain_header = 0;
	flt_index.embedded_call_mux_id_valid = 1;
	flt_index.embedded_call_mux_id = mux_id;

	IPA_BE_DBG("UL per-PDN removed notify: pdn %d client %d src_pipe %u mux %u\n",
		pdn_iface, client_iface, src_pipe, mux_id);

	if (ipa3_qmi_filter_notify_send(&flt_index))
		IPA_BE_ERR("Per-PDN removed notify failed: pdn %d client %d mux %u\n",
			pdn_iface, client_iface, mux_id);
}

/* wait_event predicate: re-locks and re-walks to avoid stale pointer on free. */
static bool ipa_ul_group_settled(int client_iface, enum ipa_ip_type ip_type,
		enum flt_rule_category cat, bool keyed_by_vlan, uint16_t vlan_id)
{
	struct ipa_ul_group *g;
	bool settled;

	spin_lock_bh(&ipa_ul_groups_lock);
	g = ipa_find_ul_group(client_iface, ip_type, cat, keyed_by_vlan, vlan_id);
	settled = !g || !g->installing;
	spin_unlock_bh(&ipa_ul_groups_lock);
	return settled;
}

/*
 * Finds-or-creates the ipa_ul_group for the given key and the
 * ipa_ul_pdn_ref for pdn_iface within it, bumping both ref_counts.
 * Pre-allocates both before taking the lock so it's never dropped
 * mid-lookup -- a concurrent release could otherwise free the group out
 * from under a reacquired pointer.
 *
 * *is_new_group means the caller must build+install the HW rows, then call
 * ipa_ul_group_install_done() or ipa_ul_group_abort_new(); the group stays
 * installing=true (blocking other acquirers on the same key, who
 * wait_event on ipa_ul_group_settled()) until then. *is_new_pdn (when
 * !*is_new_group) means this PDN is newly attaching to an
 * already-installed group: no HW work, but it still needs its own QMI
 * notify (Q4/Q10). Neither flag set is a plain refcount bump.
 */
static int ipa_ul_group_acquire(int client_iface, enum ipa_ip_type ip_type,
		enum flt_rule_category cat, bool keyed_by_vlan, uint16_t vlan_id,
		int pdn_iface, uint8_t mux_id,
		struct ipa_ul_group **out_group,
		bool *is_new_group, bool *is_new_pdn)
{
	struct ipa_ul_group *g, *new_g;
	struct ipa_ul_pdn_ref *p, *new_p;

	*out_group = NULL;
	*is_new_group = false;
	*is_new_pdn = false;

	new_g = kzalloc(sizeof(*new_g), GFP_KERNEL);
	new_p = kzalloc(sizeof(*new_p), GFP_KERNEL);
	if (!new_g || !new_p) {
		kfree(new_g);
		kfree(new_p);
		return -ENOMEM;
	}
	INIT_LIST_HEAD(&new_g->pdn_list);
	INIT_LIST_HEAD(&new_g->rule_id_list);
	INIT_LIST_HEAD(&new_g->prio_list);
	new_g->client_iface = client_iface;
	new_g->ip_type = ip_type;
	new_g->cat = cat;
	new_g->keyed_by_vlan = keyed_by_vlan;
	new_g->vlan_id = vlan_id;

retry:
	spin_lock_bh(&ipa_ul_groups_lock);
	g = ipa_find_ul_group(client_iface, ip_type, cat, keyed_by_vlan, vlan_id);
	if (g && g->installing) {
		spin_unlock_bh(&ipa_ul_groups_lock);
		wait_event(ipa_ul_groups_wq,
			   ipa_ul_group_settled(client_iface, ip_type, cat,
						 keyed_by_vlan, vlan_id));
		goto retry;
	}
	if (!g) {
		g = new_g;
		new_g = NULL;
		g->installing = true;
		list_add(&g->node, &ipa_ul_groups_list);
		*is_new_group = true;
	}

	p = ipa_find_ul_pdn_ref(g, pdn_iface);
	if (!p) {
		p = new_p;
		new_p = NULL;
		p->pdn_iface = pdn_iface;
		atomic_set(&p->ref_count, 0);
		list_add(&p->node, &g->pdn_list);
		*is_new_pdn = true;
	}
	/* Always refresh, not just on first attach: mux_id can change across
	 * this PDN's lifetime while it stays attached to this group, and a
	 * stale value here would leak into the next notify for it.
	 */
	p->mux_id = mux_id;

	atomic_inc(&p->ref_count);
	atomic_inc(&g->ref_count);
	*out_group = g;
	spin_unlock_bh(&ipa_ul_groups_lock);

	kfree(new_g);
	kfree(new_p);
	return 0;
}

/* Marks a freshly-created group's HW install complete and wakes waiters. */
static void ipa_ul_group_install_done(struct ipa_ul_group *g)
{
	spin_lock_bh(&ipa_ul_groups_lock);
	g->installing = false;
	spin_unlock_bh(&ipa_ul_groups_lock);
	wake_up_all(&ipa_ul_groups_wq);
}

/*
 * Rolls back a group whose HW install failed. Only valid for a group this
 * call itself just created (is_new_group was true): since it was created
 * with installing=true, no other acquirer could have joined its pdn_list
 * in the meantime, so it is safe to unlink and free unconditionally.
 */
static void ipa_ul_group_abort_new(struct ipa_ul_group *g)
{
	struct ipa_ul_pdn_ref *p, *tmp;

	spin_lock_bh(&ipa_ul_groups_lock);
	list_del(&g->node);
	spin_unlock_bh(&ipa_ul_groups_lock);

	list_for_each_entry_safe(p, tmp, &g->pdn_list, node) {
		list_del(&p->node);
		kfree(p);
	}
	ipa_ul_free_rule_id_list(&g->rule_id_list);
	ipa_ul_free_prio_list(&g->prio_list);
	kfree(g->flt_hdls);
	kfree(g);

	wake_up_all(&ipa_ul_groups_wq);
}

/*
 * Releases this pdn_iface's membership (one flow's worth) from the group
 * identified by the given key. Returns 1 if the group's aggregate
 * ref_count reached 0 (already unlinked from ipa_ul_groups_list; caller
 * must ipa_ul_group_teardown() it), 0 if it still has other users, or a
 * negative errno if the group or this PDN's membership wasn't found.
 *
 * When this drops the PDN's ref_count under THIS group to zero,
 * *out_pdn_removed is set and the mux_id/pipe out-params are filled -- but
 * that alone doesn't mean the PDN's uplink rules are gone, since it may
 * still belong to a sibling group. This function sends no notify itself;
 * the caller must check ipa_ul_client_pdn_lookup() across the whole
 * client_iface+ip_type before deciding whether to send
 * ipa_ul_send_pdn_removed_notify() with the values returned here.
 *
 * Waits out an installing group before the lookup/decrement, same as
 * ipa_ul_group_acquire(). Decrementing through one used to leave it linked
 * with zero refs and live HW rows that nothing would reap, and could
 * return notif pipe fields the builder had not written yet. Callers are in
 * process context and never hold an installing group, so no self-deadlock.
 */
static int ipa_ul_group_release(int client_iface, enum ipa_ip_type ip_type,
		enum flt_rule_category cat, bool keyed_by_vlan, uint16_t vlan_id,
		int pdn_iface, struct ipa_ul_group **out_group_if_empty,
		bool *out_pdn_removed, uint8_t *out_mux_id,
		uint32_t *out_src_pipe, uint32_t *out_ebd_pipe)
{
	struct ipa_ul_group *g;
	struct ipa_ul_pdn_ref *p;
	bool group_empty = false;
	bool pdn_removed = false;
	uint8_t removed_mux_id = 0;
	uint32_t notif_src_pipe = 0, notif_ebd_pipe = 0;

	*out_group_if_empty = NULL;
	if (out_pdn_removed)
		*out_pdn_removed = false;

retry:
	spin_lock_bh(&ipa_ul_groups_lock);
	g = ipa_find_ul_group(client_iface, ip_type, cat, keyed_by_vlan, vlan_id);
	if (g && g->installing) {
		spin_unlock_bh(&ipa_ul_groups_lock);
		wait_event(ipa_ul_groups_wq,
			   ipa_ul_group_settled(client_iface, ip_type, cat,
						 keyed_by_vlan, vlan_id));
		/* The install may have aborted and freed the group, so redo
		 * the lookup from scratch rather than reusing g.
		 */
		goto retry;
	}
	if (!g) {
		spin_unlock_bh(&ipa_ul_groups_lock);
		IPA_BE_ERR("No uplink group for client %d ip %d cat %d vlan %d/%u\n",
			client_iface, ip_type, cat, keyed_by_vlan, vlan_id);
		return -ENOENT;
	}
	p = ipa_find_ul_pdn_ref(g, pdn_iface);
	if (!p) {
		spin_unlock_bh(&ipa_ul_groups_lock);
		IPA_BE_ERR("No pdn_ref for pdn %d in uplink group client %d ip %d cat %d\n",
			pdn_iface, client_iface, ip_type, cat);
		return -ENOENT;
	}
	/* Defensive: a caller releasing more times than it acquired for this
	 * pdn (e.g. a retried/duplicate destroy message) must not drive the
	 * counters negative or trigger a spurious teardown of still-active
	 * state -- treat it as a no-op error instead.
	 */
	if (atomic_read(&p->ref_count) <= 0) {
		spin_unlock_bh(&ipa_ul_groups_lock);
		IPA_BE_ERR("Spurious release: pdn %d already at 0 refs in uplink group client %d ip %d cat %d\n",
			pdn_iface, client_iface, ip_type, cat);
		return -EINVAL;
	}
	if (atomic_dec_return(&p->ref_count) <= 0) {
		pdn_removed = true;
		removed_mux_id = p->mux_id;
		notif_src_pipe = g->notif_src_pipe;
		notif_ebd_pipe = g->notif_ebd_pipe;
		list_del(&p->node);
		kfree(p);
	}
	/* installing is false here: the retry loop above waited it out, and
	 * acquire only sets it on a group it creates itself.
	 */
	if (atomic_dec_return(&g->ref_count) <= 0) {
		list_del(&g->node);
		group_empty = true;
	}
	spin_unlock_bh(&ipa_ul_groups_lock);

	if (pdn_removed) {
		if (out_pdn_removed)
			*out_pdn_removed = true;
		if (out_mux_id)
			*out_mux_id = removed_mux_id;
		if (out_src_pipe)
			*out_src_pipe = notif_src_pipe;
		if (out_ebd_pipe)
			*out_ebd_pipe = notif_ebd_pipe;
	}

	if (group_empty)
		*out_group_if_empty = g;
	return group_empty ? 1 : 0;
}

/* One distinct PDN still attached to a sibling group, collected so it can
 * be re-notified with the shrunk-but-still-valid rule set after a group
 * of the same client_iface+ip_type tears down.
 */
struct ipa_ul_sibling_pdn {
	struct list_head node;
	int pdn_iface;
	uint8_t mux_id;
};

/*
 * Collects the distinct (pdn_iface, mux_id) pairs attached to any group
 * still matching (client_iface, ip_type) -- used right after a sibling
 * group tears down, to find who needs an updated notify. Appends one
 * kzalloc'd node per distinct PDN onto the caller-supplied out_list (an
 * empty, INIT_LIST_HEAD'd list); caller owns and must free every node.
 * Takes ipa_ul_groups_lock itself -- caller must not already hold it.
 */
static void ipa_ul_collect_sibling_pdns(int client_iface, enum ipa_ip_type ip_type,
	struct list_head *out_list)
{
	struct ipa_ul_group *g;
	struct ipa_ul_pdn_ref *p;

	spin_lock_bh(&ipa_ul_groups_lock);
	list_for_each_entry(g, &ipa_ul_groups_list, node) {
		if (g->client_iface != client_iface || g->ip_type != ip_type)
			continue;
		if (g->installing)
			continue;
		list_for_each_entry(p, &g->pdn_list, node) {
			struct ipa_ul_sibling_pdn *s;
			bool dup = false;

			list_for_each_entry(s, out_list, node) {
				if (s->pdn_iface == p->pdn_iface) {
					dup = true;
					break;
				}
			}
			if (dup)
				continue;

			/* GFP_ATOMIC: allocating while holding
			 * ipa_ul_groups_lock under spin_lock_bh, can't sleep.
			 */
			s = kzalloc(sizeof(*s), GFP_ATOMIC);
			if (!s)
				continue;
			s->pdn_iface = p->pdn_iface;
			s->mux_id = p->mux_id;
			list_add_tail(&s->node, out_list);
		}
	}
	spin_unlock_bh(&ipa_ul_groups_lock);
}

/*
 * Deletes the HW rows owned by an empty group (already unlinked from
 * ipa_ul_groups_list by the caller) and frees it. No QMI notify is sent
 * here -- the departing PDN already got its "removed" notify from
 * ipa_ul_group_release(); sending another would duplicate it. Also
 * re-notifies every PDN still attached to a sibling group of the same
 * client_iface+ip_type, since this group's rows just vanished from their
 * shared rule set even though their own membership didn't change.
 */
static void ipa_ul_group_teardown(struct ipa_ul_group *g)
{
	struct ipa_ul_pdn_ref *p, *tmp;
	int client_iface = g->client_iface;
	enum ipa_ip_type ip_type = g->ip_type;

	if (g->num_hdls > 0 && g->flt_hdls) {
		struct ipa_ioc_del_flt_rule *dp;
		size_t sz = sizeof(*dp) + g->num_hdls * sizeof(struct ipa_flt_rule_del);

		dp = kzalloc(sz, GFP_KERNEL);
		if (dp) {
			int k;

			dp->commit = 1;
			dp->num_hdls = g->num_hdls;
			dp->ip = g->ip_type;
			for (k = 0; k < g->num_hdls; k++) {
				dp->hdl[k].status = -1;
				dp->hdl[k].hdl = g->flt_hdls[k];
			}
			if (ipa3_del_flt_rule(dp))
				IPA_BE_ERR("Failed to delete uplink group HW rules client %d ip %d cat %d\n",
					g->client_iface, g->ip_type, g->cat);
			kfree(dp);
		} else {
			/* g and its handle array are freed below regardless, so
			 * skipping the delete would strand the HW rows with no
			 * handles left to retry. Fall back to an on-stack
			 * single-handle request per row: slower (one commit
			 * each) but allocation-free.
			 */
			struct {
				struct ipa_ioc_del_flt_rule hdr;
				struct ipa_flt_rule_del hdl;
			} one;
			int k;

			IPA_BE_ERR("No memory for batched uplink group delete client %d ip %d cat %d; deleting %d row(s) one at a time\n",
				g->client_iface, g->ip_type, g->cat, g->num_hdls);

			for (k = 0; k < g->num_hdls; k++) {
				memset(&one, 0, sizeof(one));
				one.hdr.commit = 1;
				one.hdr.num_hdls = 1;
				one.hdr.ip = g->ip_type;
				one.hdl.status = -1;
				one.hdl.hdl = g->flt_hdls[k];
				if (ipa3_del_flt_rule(&one.hdr))
					IPA_BE_ERR("Failed to delete uplink group HW rule hdl %u client %d ip %d cat %d\n",
						g->flt_hdls[k], g->client_iface,
						g->ip_type, g->cat);
			}
		}
	}

	if ((unsigned int)g->notif_src_client < IPA_CLIENT_MAX) {
		if (g->ip_type == IPA_IP_v4 && flt_rule_count_v4[g->notif_src_client] > 0)
			flt_rule_count_v4[g->notif_src_client]--;
		else if (g->ip_type == IPA_IP_v6 && flt_rule_count_v6[g->notif_src_client] > 0)
			flt_rule_count_v6[g->notif_src_client]--;
	} else if (g->notif_src_client != IPA_CLIENT_MAX) {
		/* IPA_CLIENT_MAX is the expected sentinel for a zero-row
		 * group, not an error.
		 */
		IPA_BE_ERR("notif_src_client %d out of range, skipping flt_rule_count update\n",
			g->notif_src_client);
	}

	list_for_each_entry_safe(p, tmp, &g->pdn_list, node) {
		list_del(&p->node);
		kfree(p);
	}
	ipa_ul_free_rule_id_list(&g->rule_id_list);
	ipa_ul_free_prio_list(&g->prio_list);
	kfree(g->flt_hdls);
	kfree(g);

	/* g is already unlinked, so the collected set naturally excludes it. */
	{
		struct list_head siblings;
		struct ipa_ul_sibling_pdn *s, *s_tmp;

		INIT_LIST_HEAD(&siblings);
		ipa_ul_collect_sibling_pdns(client_iface, ip_type, &siblings);
		list_for_each_entry(s, &siblings, node)
			ipa_ul_send_client_notify(client_iface, ip_type, s->mux_id);
		list_for_each_entry_safe(s, s_tmp, &siblings, node) {
			list_del(&s->node);
			kfree(s);
		}
	}
}

/*
 * Maximum number of IPv6 /64 prefixes tracked/installed per interface. A LAN
 * bridge can carry several global prefixes simultaneously (e.g. multiple
 * delegated prefixes, or GUA + ULA); each needs its own private-subnet rule.
 */
#define IPA_BE_MAX_V6_PREFIXES 8

/*
 * Subnet match derived from a bridge for one IP family.
 *  - IPv4: a single subnet addr/mask (host byte order).
 *  - IPv6: up to IPA_BE_MAX_V6_PREFIXES /64 prefixes, each as the upper 64 bits
 *    (host byte order) in [hi, lo]; n_v6 is how many are valid.
 */
struct ipa_be_subnet_match {
	uint32_t v4_addr;
	uint32_t v4_mask;
	int n_v6;
	uint32_t v6_prefix[IPA_BE_MAX_V6_PREFIXES][2];
};

/*
 * Tracks the always-on private-subnet rules installed on an interface for one IP
 * type. Notifier-driven (no ref-counting): we store the programmed match so a
 * reconcile reprograms only when the bridge subnet/prefix set changes.
 * Keyed by (intf_num, ip_type); bridge_if_num is kept for reference/debug.
 */
struct ipa_private_subnet_entry {
	struct list_head node;
	int intf_num;
	int bridge_if_num;
	enum ipa_ip_type ip_type;
	struct ipa_be_subnet_match match;
};

static LIST_HEAD(ipa_private_subnet_entries_list);
static DEFINE_SPINLOCK(ipa_private_subnet_lock);

/* Structure to track installed PDN-level filtering rules */
struct ipa_pdn_filter_rules {
	struct list_head node;
	int pdn_iface;
	enum ipa_ip_type ip_type;
	bool dft_rule_installed;
	bool icmp_rule_installed;
	bool catchup_rule_installed;
	atomic_t ref_count;
	bool eth_bh_catchall_installed;
	bool eth_bh_catchall_installing;
	int eth_bh_catchall_ref_count;
	uint32_t eth_bh_catchall_flt_hdl[IPA_MAX_NUM_PROPS];
	int eth_bh_catchall_num_hdls;
	uint32_t eth_bh_catchall_wan_vlan;
};

static LIST_HEAD(ipa_pdn_filter_list);
static DEFINE_SPINLOCK(ipa_pdn_filter_lock);
static DECLARE_WAIT_QUEUE_HEAD(ipa_pdn_filter_wq);

/* Helper functions for PDN filter tracking */
static struct ipa_pdn_filter_rules *ipa_find_pdn_filter_entry(int pdn_iface, enum ipa_ip_type ip_type)
{
	struct ipa_pdn_filter_rules *entry;

	list_for_each_entry(entry, &ipa_pdn_filter_list, node) {
		if (entry->pdn_iface == pdn_iface && entry->ip_type == ip_type) {
			return entry;
		}
	}
	return NULL;
}

bool ipa_be_pdn_filter_installed(int pdn_iface, enum ipa_ip_type ip_type)
{
	bool installed;

	spin_lock_bh(&ipa_pdn_filter_lock);
	installed = ipa_find_pdn_filter_entry(pdn_iface, ip_type) != NULL;
	spin_unlock_bh(&ipa_pdn_filter_lock);
	return installed;
}
EXPORT_SYMBOL(ipa_be_pdn_filter_installed);

/* wait_event predicate: re-locks and re-walks to avoid stale pointer on free. */
static bool ipa_pdn_eth_bh_catchall_settled(int pdn_iface,
					    enum ipa_ip_type ip_type)
{
	struct ipa_pdn_filter_rules *entry;
	bool settled;

	spin_lock_bh(&ipa_pdn_filter_lock);
	entry = ipa_find_pdn_filter_entry(pdn_iface, ip_type);
	settled = !entry || !entry->eth_bh_catchall_installing;
	spin_unlock_bh(&ipa_pdn_filter_lock);
	return settled;
}

static void ipa_create_pdn_filter_entry(struct ipa_pdn_filter_rules *new_entry,
					int pdn_iface, enum ipa_ip_type ip_type)
{
	new_entry->pdn_iface = pdn_iface;
	new_entry->ip_type = ip_type;
	new_entry->dft_rule_installed = false;
	new_entry->icmp_rule_installed = false;
	new_entry->catchup_rule_installed = false;
	atomic_set(&new_entry->ref_count, 0);
	new_entry->eth_bh_catchall_installed = false;
	new_entry->eth_bh_catchall_installing = false;
	new_entry->eth_bh_catchall_ref_count = 0;
	memset(new_entry->eth_bh_catchall_flt_hdl, 0,
		sizeof(new_entry->eth_bh_catchall_flt_hdl));
	new_entry->eth_bh_catchall_num_hdls = 0;
	new_entry->eth_bh_catchall_wan_vlan = IPA_VLAN_ID_NOT_CONFIGURED;

	list_add(&new_entry->node, &ipa_pdn_filter_list);
}

static void ipa_remove_pdn_filter_entry(struct ipa_pdn_filter_rules *entry)
{
	list_del(&entry->node);
	kfree(entry);
}

int ipa_be_v4_add_filter_rule(struct ipa_ipv4_rule_create_msg v4_msg, bool lan2lan, int intf_num, mac_addr_t mac, int is_ret)
{
	struct ipa_ioc_add_flt_rule_v2 *pFilteringTable = NULL;
	struct ipa_flt_rule_add_v2 flt_rule_entry;
	struct ipa_ioc_query_intf temp_intf;
	int retval = 0, len = 0;
	struct ipa_ioc_query_intf_rx_props *rx_prop;
	int total_rules = 1;
	int idx = 0;
	int flt_hdl = 0;
	struct ipa_ioc_get_rt_tbl rt_tbl = {0};
	int l2l_ep = -1;
	u32 l2l_dst = 0;
	bool rt_tbl_got = false;

	IPA_BE_DBG("ECMIPA entry lan2lan %d\n", lan2lan);
	ipa_type_check_ipa_mac_addr(mac);

	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(intf_num, &temp_intf)) {
		IPA_BE_ERR("Interface with index %u does not exist.\n", intf_num);
		return -EINVAL;
	}

	rx_prop = (struct ipa_ioc_query_intf_rx_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_rx_props) +
		temp_intf.num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (rx_prop == NULL) {
		IPA_BE_ERR("Unable to allocate rx_prop memory.\n");
		return -EINVAL;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(rx_prop->name, temp_intf.name, sizeof(rx_prop->name));
#else
	strlcpy(rx_prop->name, temp_intf.name, sizeof(rx_prop->name));
#endif

	rx_prop->num_rx_props = temp_intf.num_rx_props;
	IPA_BE_DBG("Query rx_prop %d name %s\n", rx_prop->num_rx_props, temp_intf.name);
	ipa3_query_intf_rx_props(rx_prop);

	if (rx_prop->num_rx_props == 0) {
		IPA_BE_ERR("No rx props for iface %s\n", rx_prop->name);
		retval = -EINVAL;
		goto end;
	}

	IPA_BE_DBG("Query response rx_prop src %d hdr_l2_type %d\n", rx_prop->rx[idx].src_pipe,
		rx_prop->rx[idx].hdr_l2_type);

	if (lan2lan) {
		enum ipa_hw_type ipa_ver = ipa_get_hw_type();

		pFilteringTable = (struct ipa_ioc_add_flt_rule_v2 *)kzalloc(sizeof(struct ipa_ioc_add_flt_rule_v2), GFP_KERNEL);
		if (!pFilteringTable) {
			IPA_BE_ERR("Failed to allocate ipa_ioc_add_flt_rule_v2 memory...\n");
			retval = -EINVAL;
			goto end;
		}

		pFilteringTable->rules = (uintptr_t)kzalloc(total_rules * sizeof(struct ipa_flt_rule_add_v2), GFP_KERNEL);
		if (!pFilteringTable->rules) {
			kfree(pFilteringTable);
			pFilteringTable = NULL;
			IPA_BE_ERR("Failed to allocate ipa_flt_rule_add_v2 memory...\n");
			retval = -EINVAL;
			goto end;
		}

		IPA_BE_DBG("Install rules on Rx pipe at idx %d src_pipe %d\n", idx, rx_prop->rx[idx].src_pipe);

		/*
		 * lan2lan filters are refcounted globally per (dst host IP,
		 * ip type), but the HW rule is keyed on the physical pipe.
		 * Resolve this ingress ep; if it already holds the rule for
		 * this dst the reference is counted in the helper and we skip
		 * the duplicate HW install. Otherwise install below and
		 * register the ep + handle via ipa3_lan2lan_flt_add().
		 */
		{
			l2l_ep = ipa_get_ep_mapping(rx_prop->rx[idx].src_pipe);
			if (l2l_ep < 0) {
				/*
				 * Unmapped client -> no physical pipe to key the
				 * global (dst, ip type) presence set on. Bail out
				 * instead of registering a rule with ep = -1, which
				 * would never match a teardown lookup and leak the
				 * ipa3_lan2lan_ep_hdl entry.
				 */
				IPA_BE_ERR("lan2lan v4: no ep mapping for src_pipe %d, skipping install\n",
					   rx_prop->rx[idx].src_pipe);
				retval = -EINVAL;
				goto end;
			}
			if (is_ret)
				l2l_dst = (uint32_t)ntohl(v4_msg.tuple.return_ip);
			else
				l2l_dst = (uint32_t)ntohl(v4_msg.tuple.flow_ip);
			if (!ipa3_lan2lan_flt_need_install(IPA_IP_v4, &l2l_dst,
							   l2l_ep)) {
				IPA_BE_DBG("lan2lan v4 filter already present on ep %d, ref bumped\n",
					   l2l_ep);
				retval = 0;
				goto end;
			}
		}

		/* add mac based rule*/
		pFilteringTable->commit = 1;
		pFilteringTable->ep = rx_prop->rx[idx].src_pipe;
		pFilteringTable->ip = IPA_IP_v4;
		pFilteringTable->num_rules = total_rules;
		pFilteringTable->flt_rule_size = sizeof(struct ipa_flt_rule_add_v2);
		/* pFilteringTable->add_after_hdl = 0; //tbd */

		memset(&flt_rule_entry, 0, sizeof(flt_rule_entry));

		flt_rule_entry.rule.retain_hdr = 0;
		flt_rule_entry.rule.to_uc = 0;

		/* Set filter action based on IPA version and EP direction.
		 * V4 CT lookup key is target_ip (larger EP). MAC tiebreaker when EPs equal.
		 * DST_NAT when packet dst = target_ip; SRC_NAT when packet src = target_ip.
		 */
		if (ipa_ver >= IPA_HW_v7_0) {
			int ep_cmp = ipa_be_flow_canonical_cmp(
				v4_msg.conn_rule.flow_interface_num,
				v4_msg.conn_rule.return_interface_num,
				v4_msg.conn_rule.flow_mac,
				v4_msg.conn_rule.return_mac);

			if (ep_cmp < 0) {
				retval = -EINVAL;
				goto end;
			}

			if (is_ret)
				flt_rule_entry.rule.action = ep_cmp ?
					IPA_PASS_TO_DST_NAT : IPA_PASS_TO_SRC_NAT;
			else
				flt_rule_entry.rule.action = ep_cmp ?
					IPA_PASS_TO_SRC_NAT : IPA_PASS_TO_DST_NAT;
			IPA_BE_DBG("LAN2LAN (v7.0+): %s action (is_ret=%d)\n",
				   flt_rule_entry.rule.action == IPA_PASS_TO_DST_NAT ?
				   "DST_NAT" : "SRC_NAT", is_ret);
		} else {
			/* IPA < v7.0: Use routing (backward compatible) */
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
			IPA_BE_DBG("LAN2LAN (< v7.0): ROUTING action (legacy)\n");
		}

		flt_rule_entry.rule.eq_attrib_type = 0;

		rt_tbl.ip = IPA_IP_v4;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(rt_tbl.name, V4_LAN_TO_LAN_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#else
		strlcpy(rt_tbl.name, V4_LAN_TO_LAN_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#endif

		IPA_BE_DBG("This flt rule points to rt tbl %s.\n", rt_tbl.name);

		if (ipa3_get_rt_tbl((struct ipa_ioc_get_rt_tbl *)&rt_tbl)) {
			IPA_BE_ERR("ECMIPA failed to get route hdl \n");
			retval = -EFAULT;
			goto end;
		}
		rt_tbl_got = true;
		IPA_BE_DBG("Install filter rules with rt_tbl.hdl %d \n", rt_tbl.hdl);

		IPA_BE_DBG("Install rules on Rx pipe at idx %d \n", idx);
		flt_rule_entry.rule.rt_tbl_hdl = rt_tbl.hdl;
		/* Lookup ref held until 'end:', released after the rule is added. */

		flt_rule_entry.rule.hashable = true;
		flt_rule_entry.rule.rule_id = LAN2LAN_RULE_ID; /* LAN2LAN_RULE_ID */

		flt_rule_entry.flt_rule_category = IPA_FLT_RULE_CAT_LAN2LAN;

		memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[idx].attrib, sizeof(flt_rule_entry.rule.attrib));

		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;

		if (is_ret) {
			flt_rule_entry.rule.attrib.u.v4.dst_addr = (uint32_t)ntohl(v4_msg.tuple.return_ip);
		} else {
			flt_rule_entry.rule.attrib.u.v4.dst_addr = (uint32_t)ntohl(v4_msg.tuple.flow_ip);
		}
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;

		flt_rule_entry.rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
		memcpy(&(((struct ipa_flt_rule_add_v2 *)(uintptr_t)pFilteringTable->rules)[0]), &flt_rule_entry, sizeof(flt_rule_entry));

		if (ipa3_add_flt_rule_usr_v2((struct ipa_ioc_add_flt_rule_v2 *)pFilteringTable,
			true)) {
			IPA_BE_ERR("ipa3_add_flt_rule_usr_v2 failed\n");
			retval = -EFAULT;
			goto end;
		} else {
			flt_hdl = (((struct ipa_flt_rule_add_v2 *)(uintptr_t)pFilteringTable->rules)[0]).flt_rule_hdl;
			IPA_BE_DBG("Lan2Lan %d filter rule hdl: %d\n", lan2lan, flt_hdl);

			/* Register the new HW rule: record this ep in the
			 * global (dst, ip_type) presence set and bump the
			 * global refcount. */
			ipa3_lan2lan_flt_add(IPA_IP_v4, &l2l_dst, l2l_ep, flt_hdl);
		}
	} else {
		IPA_BE_DBG("Lan2wan case \n");
	}

end:
	if (rt_tbl_got)
		ipa_put_rt_tbl(rt_tbl.hdl);
	if (pFilteringTable) {
		if (pFilteringTable->rules)
			kfree((void *)(uintptr_t)pFilteringTable->rules);
		kfree(pFilteringTable);
	}
	if (rx_prop)
		kfree(rx_prop);

	IPA_BE_DBG("Exit retval %d \n", retval);
	return retval;
}
EXPORT_SYMBOL(ipa_be_v4_add_filter_rule);

/*
 * Builds and installs v4 uplink HW rows for one add event. At least one of
 * base_g/xlat_g must be non-NULL; both non-NULL means a combined build
 * (one HW add + one QMI notify) for a client's very first flow being an
 * XLAT flow. When only xlat_g is given (base already exists), its rows'
 * sub_category is looked up from prio_src (the client's base group)
 * instead of recomputed, so they land where a from-scratch build would
 * have placed them -- base and per-VLAN XLAT rows share
 * IPA_FLT_RULE_CAT_UPLINK, and the kernel's own category/sub_category sort
 * (__ipa_add_flt_rule()) relies on that shared, consistent ordering.
 */
static int ipa_be_v4_build_install_uplink_rows(struct ipa_ul_group *base_g,
	struct ipa_ul_group *xlat_g, struct ipa_ul_group *prio_src,
	struct ipa_ioc_query_intf_rx_props *rx_prop,
	struct ipa_ioc_query_intf_ext_props *ext_prop,
	bool has_vlan, uint16_t vlan_id, int client_iface, uint8_t mux_id)
{
	struct ipa_ioc_add_flt_rule_v2 *pFilteringTable = NULL;
	struct ipa_flt_rule_add_v2 flt_rule_entry;
	struct ipa_fltr_installed_notif_req_msg_v01 *flt_index = NULL;
	struct ipa3_flt_entry flt_entry = {0};
	struct ipa_ioc_write_qmapid mux;
	enum ipa_flt_action action_cache;
	struct list_head local_prio_list;
	int base_count = 0, xlat_count = 0, total_rules;
	int idx = 0, j, cnt, i = 0, index;
	int retval = 0;
	uint32_t *base_hdls = NULL, *xlat_hdls = NULL;
	int base_hdl_count = 0, xlat_hdl_count = 0;
	uint8_t value = 0;

	INIT_LIST_HEAD(&local_prio_list);

	for (cnt = 0; cnt < ext_prop->num_ext_props; cnt++) {
		if (ext_prop->ext[cnt].ip != IPA_IP_v4)
			continue;
		if (base_g && !ext_prop->ext[cnt].is_xlat_rule)
			base_count++;
		else if (xlat_g && ext_prop->ext[cnt].is_xlat_rule)
			xlat_count++;
	}
	total_rules = base_count + xlat_count;

	IPA_BE_DBG("v4 UL group build: client %d base_g %p(%d rows) xlat_g %p(%d rows)\n",
		client_iface, base_g, base_count, xlat_g, xlat_count);

	if (total_rules == 0) {
		/* Nothing to install: mark notif_src_client invalid on
		 * whichever group(s) were passed in, so later
		 * aggregation/teardown skip this group's unset pipe info
		 * instead of treating client 0 as real.
		 */
		if (base_g)
			base_g->notif_src_client = IPA_CLIENT_MAX;
		if (xlat_g)
			xlat_g->notif_src_client = IPA_CLIENT_MAX;
		return 0;
	}

	flt_index = kzalloc(sizeof(*flt_index), GFP_KERNEL);
	if (!flt_index)
		return -ENOMEM;

	if (total_rules > ARRAY_SIZE(flt_index->rule_id)) {
		IPA_BE_ERR("v4 total_rules %d exceeds rule_id[] cap (client=%d)\n",
			total_rules, client_iface);
		kfree(flt_index);
		return -E2BIG;
	}

	if (rx_prop->num_rx_props > 0) {
		if (base_count) {
			base_hdls = kzalloc((rx_prop->num_rx_props / 2) * base_count *
						 sizeof(uint32_t), GFP_KERNEL);
			if (!base_hdls) {
				kfree(flt_index);
				return -ENOMEM;
			}
		}
		if (xlat_count) {
			xlat_hdls = kzalloc((rx_prop->num_rx_props / 2) * xlat_count *
						 sizeof(uint32_t), GFP_KERNEL);
			if (!xlat_hdls) {
				kfree(base_hdls);
				kfree(flt_index);
				return -ENOMEM;
			}
		}
	}

	if (base_g) {
		/* Covers the full v4 ext_prop set -- spliced onto
		 * base_g->prio_list below so a later, separately-built XLAT
		 * group can reuse it instead of recomputing from a narrowed
		 * view.
		 */
		ipa_be_compute_rule_priorities(ext_prop, IPA_IP_v4, &local_prio_list);
	}

	for (j = 0; j < rx_prop->num_rx_props / 2; j++) {
		int base_in_batch;

		idx = j * 2;
		if (rx_prop->rx[idx].ip != IPA_IP_v4) {
			IPA_BE_DBG("IP not matching required type %d .. continue\n", rx_prop->rx[idx].ip);
			continue;
		}
		IPA_BE_DBG("Install rules at idx %d\n", idx);

		pFilteringTable = (struct ipa_ioc_add_flt_rule_v2 *)kzalloc(sizeof(struct ipa_ioc_add_flt_rule_v2), GFP_KERNEL);
		if (pFilteringTable == NULL) {
			IPA_BE_ERR("Error Locate ipa_ioc_add_flt_rule_v2 memory...\n");
			retval = -ENOMEM;
			goto fail;
		}

		pFilteringTable->rules = (uintptr_t)kzalloc(total_rules * sizeof(struct ipa_flt_rule_add_v2), GFP_KERNEL);
		if (!pFilteringTable->rules) {
			kfree(pFilteringTable);
			pFilteringTable = NULL;
			IPA_BE_ERR("Error Locate ipa_flt_rule_add_v2 memory...\n");
			retval = -ENOMEM;
			goto fail;
		}

		memset(flt_index, 0, sizeof(struct ipa_fltr_installed_notif_req_msg_v01));
		flt_index->source_pipe_index = ipa_get_ep_mapping(rx_prop->rx[idx].src_pipe);
		flt_index->install_status = IPA_QMI_RESULT_SUCCESS_V01;

		flt_index->rule_id_valid = 1;
		flt_index->rule_id_len = total_rules;
		flt_index->embedded_pipe_index_valid = 1;
		flt_index->embedded_pipe_index = ipa_get_ep_mapping(IPA_CLIENT_APPS_LAN_WAN_PROD);
		flt_index->retain_header_valid = 1;
		flt_index->retain_header = 0;

		IPA_BE_DBG("flt_index: src pipe: %d, num of rules: %d, ebd pipe: %d\n",
			flt_index->source_pipe_index, flt_index->rule_id_len, flt_index->embedded_pipe_index);

		pFilteringTable->commit = 1;
		pFilteringTable->ep = rx_prop->rx[idx].src_pipe;
		pFilteringTable->global = false;
		pFilteringTable->ip = IPA_IP_v4;
		pFilteringTable->num_rules = total_rules;
		pFilteringTable->flt_rule_size = sizeof(struct ipa_flt_rule_add_v2);

		i = 0;

		/* ---- Pass 1: base (non-is_xlat_rule) rows, if requested ---- */
		if (base_g) {
			for (cnt = 0; cnt < ext_prop->num_ext_props && i < total_rules; cnt++) {
				enum rule_sub_category sc;

				if (ext_prop->ext[cnt].ip != IPA_IP_v4) {
					IPA_BE_DBG("iptype mismatch %d .. continue\n", ext_prop->ext[cnt].ip);
					continue;
				}
				if (ext_prop->ext[cnt].is_xlat_rule)
					continue;

				IPA_BE_DBG("Ext prop cnt %d rt_tbl_idx: %d, rule_id: %d, is_xlat %d \n",
						cnt, ext_prop->ext[cnt].rt_tbl_idx,
						ext_prop->ext[cnt].rule_id,
						ext_prop->ext[cnt].is_xlat_rule);

				memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add_v2));
				flt_rule_entry.at_rear = 0;

				flt_rule_entry.flt_rule_hdl = -1;
				flt_rule_entry.status = -1;

				flt_rule_entry.rule.retain_hdr = 0;
				flt_rule_entry.rule.to_uc = 0;
				flt_rule_entry.rule.eq_attrib_type = 1;

				flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;
				action_cache = flt_rule_entry.rule.action;
				flt_rule_entry.rule.set_metadata = true;

				flt_rule_entry.flt_rule_category = IPA_FLT_RULE_CAT_UPLINK;

				index = flt_rule_count_v4[rx_prop->rx[idx].src_pipe];

				memcpy(&flt_rule_entry.rule.eq_attrib,
					&ext_prop->ext[cnt].eq_attrib,
					sizeof(ext_prop->ext[cnt].eq_attrib));

				/* IPv4-fragment rows must still match before any other
				 * uplink row (same intent the old at_rear=0-to-front
				 * trick had) -- pin them to the highest priority
				 * rather than their ascending-rule_id slot.
				 */
				sc = flt_rule_entry.rule.eq_attrib.ipv4_frag_eq_present ?
					IPA_RULE_PRIORITY_0 :
					ipa_be_lookup_local_sub_cat(&local_prio_list,
						ext_prop->ext[cnt].rule_id);
				flt_rule_entry.rule_sub_category = sc;

				/* Populate the flt rule action from ext_prop */
				if (ext_prop->ext[cnt].action == IPA_PASS_TO_EXCEPTION) {
					/* Q6 can't handle it, go A7 exception */
					flt_rule_entry.rule.action = ext_prop->ext[cnt].action;
					flt_rule_entry.rule.rt_tbl_idx = 0;
					IPA_BE_DBG("Override rule index %d to act: %d, rt_tbl_idx: %d to %d\n",
						cnt, flt_rule_entry.rule.action,
						ext_prop->ext[cnt].rt_tbl_idx,
						flt_rule_entry.rule.rt_tbl_idx);
				} else {
					flt_rule_entry.rule.action = action_cache;
					flt_rule_entry.rule.rt_tbl_idx = ext_prop->ext[cnt].rt_tbl_idx;
					IPA_BE_DBG("Restore rule index %d to act: %d, rt_tbl_idx: %d \n",
						cnt, flt_rule_entry.rule.action,
						flt_rule_entry.rule.rt_tbl_idx);
				}

				flt_rule_entry.rule.hashable = ext_prop->ext[cnt].is_rule_hashable;
				flt_rule_entry.rule.rule_id = ext_prop->ext[cnt].rule_id;

				/* Decrement TTL/hop-limit on uplink (client -> WAN) forwarding
				 * rules. Exception rules go to the A7 software path and must be
				 * left untouched to avoid a double TTL decrement.
				 */
				if (flt_rule_entry.rule.action != IPA_PASS_TO_EXCEPTION)
					flt_rule_entry.rule.ttl_update = 1;

				flt_rule_entry.rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
				memcpy(&(((struct ipa_flt_rule_add_v2 *)(uintptr_t)pFilteringTable->rules)[i]), &flt_rule_entry, sizeof(flt_rule_entry));

				IPA_BE_DBG("Modem UL filtering rule %d has index %d installed at %d sub_cat %u\n",
					cnt, index, i, flt_rule_entry.rule_sub_category);
				flt_index->rule_id[i] = ext_prop->ext[cnt].rule_id;
				index++;
				i++;
			}
		}
		base_in_batch = i;

		/* ---- Pass 2: is_xlat_rule row(s), if requested ---- */
		if (xlat_g) {
			for (cnt = 0; cnt < ext_prop->num_ext_props && i < total_rules; cnt++) {
				enum rule_sub_category sc;

				if (ext_prop->ext[cnt].ip != IPA_IP_v4) {
					IPA_BE_DBG("iptype mismatch %d .. continue\n", ext_prop->ext[cnt].ip);
					continue;
				}
				if (!ext_prop->ext[cnt].is_xlat_rule)
					continue;

				memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add_v2));
				flt_rule_entry.at_rear = 0;

				flt_rule_entry.flt_rule_hdl = -1;
				flt_rule_entry.status = -1;

				flt_rule_entry.rule.retain_hdr = 0;
				flt_rule_entry.rule.to_uc = 0;
				flt_rule_entry.rule.eq_attrib_type = 1;

				flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;
				action_cache = flt_rule_entry.rule.action;
				flt_rule_entry.rule.set_metadata = true;

				flt_rule_entry.flt_rule_category = IPA_FLT_RULE_CAT_UPLINK;

				index = flt_rule_count_v4[rx_prop->rx[idx].src_pipe];

				memcpy(&flt_rule_entry.rule.eq_attrib,
					&ext_prop->ext[cnt].eq_attrib,
					sizeof(ext_prop->ext[cnt].eq_attrib));

				sc = base_g ?
					ipa_be_lookup_local_sub_cat(&local_prio_list,
						ext_prop->ext[cnt].rule_id) :
					ipa_ul_group_lookup_sub_cat(prio_src, ext_prop->ext[cnt].rule_id);
				flt_rule_entry.rule_sub_category = sc;

				if (ext_prop->ext[cnt].action == IPA_PASS_TO_EXCEPTION) {
					flt_rule_entry.rule.action = ext_prop->ext[cnt].action;
					flt_rule_entry.rule.rt_tbl_idx = 0;
				} else {
					flt_rule_entry.rule.action = action_cache;
					flt_rule_entry.rule.rt_tbl_idx = ext_prop->ext[cnt].rt_tbl_idx;
				}

				/* Match this group's VLAN tag directly instead of via
				 * metadata (which the else branch below still uses for
				 * the non-VLAN case).
				 */
				flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;

				if (has_vlan) {
					/* 802.1Q offset_meq_32 match, mirroring the HW
					 * equation ipahal_flt_generate_equation builds for
					 * IPA_FLT_VLAN_ID -- avoids the metadata register's
					 * 8-bit truncation.
					 */
					int meq32_n = flt_rule_entry.rule.eq_attrib.num_offset_meq_32;

					if (meq32_n + 1 > IPA_IPFLTR_NUM_MEQ_32_EQNS) {
						IPA_BE_ERR("Can't add VLAN meq_32 eq to rule: %d index %d, falling back to mux_id metadata gate\n",
							cnt, i);

						value = ext_prop->ext[cnt].mux_id;

						flt_rule_entry.rule.eq_attrib.metadata_meq32_present = 1;
						flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1 << 9);
						flt_rule_entry.rule.eq_attrib.metadata_meq32.offset = 0;
						flt_rule_entry.rule.eq_attrib.metadata_meq32.value = (value & 0xFF) << 16;
						flt_rule_entry.rule.eq_attrib.metadata_meq32.mask = 0x00FF0000;
					} else {
						flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].offset =
							(ipa3_ctx->ipa_hw_type < IPA_HW_v7_0) ?
								(u8)-6 : NON_IHL_EQ_OFFSET_FROM_L2(ETH_ALEN * 2);
						flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].mask = 0xFFFF0FFF;
						flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].value =
							(0x8100 << 16) | (vlan_id & 0xFFF);

						if (meq32_n == 0)
							flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1 << 5);
						else
							flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1 << 6);

						flt_rule_entry.rule.eq_attrib.num_offset_meq_32++;

						flt_rule_entry.rule.eq_attrib.metadata_meq32_present = 0;
						flt_rule_entry.rule.eq_attrib.rule_eq_bitmap &= ~(1 << 9);
					}
					IPA_BE_DBG("xlat rule: %d index %d matches VLAN id %d directly sub_cat %u\n",
						cnt, i, vlan_id, sc);
				} else {
					value = ext_prop->ext[cnt].mux_id;

					flt_rule_entry.rule.eq_attrib.metadata_meq32_present = 1;
					flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1 << 9);
					flt_rule_entry.rule.eq_attrib.metadata_meq32.offset = 0;
					flt_rule_entry.rule.eq_attrib.metadata_meq32.value = (value & 0xFF) << 16;
					flt_rule_entry.rule.eq_attrib.metadata_meq32.mask = 0x00FF0000;
					IPA_BE_DBG("xlat meta-data is modified for rule: %d index %d metadata : 0x%x\n",
						cnt, i, flt_rule_entry.rule.eq_attrib.metadata_meq32.value);

					/* Non-VLAN only: a single pipe-level mux write
					 * can't represent more than one VLAN sharing the
					 * same pipe, which match by VLAN ID directly above.
					 */
					mux.client = rx_prop->rx[idx].src_pipe;
					mux.qmap_id = ext_prop->ext[cnt].mux_id;
					ipa3_write_qmap_id(&mux);
				}

				flt_rule_entry.rule.hashable = ext_prop->ext[cnt].is_rule_hashable;
				flt_rule_entry.rule.rule_id = ext_prop->ext[cnt].rule_id;

				if (flt_rule_entry.rule.action != IPA_PASS_TO_EXCEPTION)
					flt_rule_entry.rule.ttl_update = 1;

				flt_rule_entry.rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
				memcpy(&(((struct ipa_flt_rule_add_v2 *)(uintptr_t)pFilteringTable->rules)[i]), &flt_rule_entry, sizeof(flt_rule_entry));

				IPA_BE_DBG("Modem UL filtering rule %d has index %d installed at %d\n", cnt, index, i);
				flt_index->rule_id[i] = ext_prop->ext[cnt].rule_id;
				index++;
				i++;
			}
		}

		if (base_g) {
			retval = ipa_ul_group_set_rule_ids(base_g, flt_index->rule_id,
				base_in_batch,
				(uint32_t)flt_index->source_pipe_index,
				(uint32_t)flt_index->embedded_pipe_index,
				rx_prop->rx[idx].src_pipe);
			if (retval)
				goto fail;
		}
		if (xlat_g) {
			retval = ipa_ul_group_set_rule_ids(xlat_g,
				&flt_index->rule_id[base_in_batch],
				total_rules - base_in_batch,
				(uint32_t)flt_index->source_pipe_index,
				(uint32_t)flt_index->embedded_pipe_index,
				rx_prop->rx[idx].src_pipe);
			if (retval)
				goto fail;
		}

		/* Notify before install, per the original design: the rule_id/
		 * pipe info above doesn't depend on install success. If the
		 * install fails, ipa_ul_group_abort_new() drops the whole
		 * group, so nothing is left half-advertised.
		 */
		ipa_ul_send_client_notify(client_iface, IPA_IP_v4, mux_id);

		if (ipa3_add_flt_rule_usr_v2((struct ipa_ioc_add_flt_rule_v2 *)pFilteringTable,
			true)) {
			IPA_BE_ERR("ipa3_add_flt_rule_usr_v2 failed\n");
			retval = -EFAULT;
			goto fail;
		}

		for (i = 0; i < pFilteringTable->num_rules; i++) {
			struct ipa_flt_rule_add_v2 *installed_rule =
				&(((struct ipa_flt_rule_add_v2 *)(uintptr_t)pFilteringTable->rules)[i]);

			if (!installed_rule->flt_rule_hdl)
				continue;

			IPA_BE_DBG("Modem uplink rules filter rule hdl: %d\n", installed_rule->flt_rule_hdl);
			flt_entry.flt_hdl = installed_rule->flt_rule_hdl;
			flt_entry.cat = IPA_FLT_RULE_CAT_UPLINK;
			flt_entry.ip_type = IPA_IP_v4;
			if (i < base_in_batch) {
				if (base_hdls)
					base_hdls[base_hdl_count++] = flt_entry.flt_hdl;
			} else {
				if (xlat_hdls)
					xlat_hdls[xlat_hdl_count++] = flt_entry.flt_hdl;
			}
			ipa3_add_filter_rules_entry(client_iface, flt_entry);
		}
		/* Per group, not per add: base+XLAT can share one HW add but
		 * teardown decrements each. Zero-row groups skip both sides.
		 */
		if (base_g && base_count)
			flt_rule_count_v4[rx_prop->rx[idx].src_pipe]++;
		if (xlat_g && xlat_count)
			flt_rule_count_v4[rx_prop->rx[idx].src_pipe]++;

		kfree((void *)(uintptr_t)pFilteringTable->rules);
		kfree(pFilteringTable);
		pFilteringTable = NULL;
	}

	if (base_g) {
		base_g->flt_hdls = base_hdls;
		base_g->num_hdls = base_hdl_count;
		/* Ownership of the computed priority nodes moves onto
		 * base_g->prio_list; local_prio_list ends up empty.
		 */
		list_splice_tail_init(&local_prio_list, &base_g->prio_list);
	}
	if (xlat_g) {
		xlat_g->flt_hdls = xlat_hdls;
		xlat_g->num_hdls = xlat_hdl_count;
	}
	kfree(flt_index);
	return 0;

fail:
	ipa_ul_free_prio_list(&local_prio_list);
	if (base_hdl_count > 0 && base_hdls) {
		struct ipa_ioc_del_flt_rule *dp;
		size_t sz = sizeof(*dp) + base_hdl_count * sizeof(struct ipa_flt_rule_del);

		dp = kzalloc(sz, GFP_KERNEL);
		if (dp) {
			int k;

			dp->commit = 1;
			dp->num_hdls = base_hdl_count;
			dp->ip = IPA_IP_v4;
			for (k = 0; k < base_hdl_count; k++) {
				dp->hdl[k].status = -1;
				dp->hdl[k].hdl = base_hdls[k];
			}
			ipa3_del_flt_rule(dp);
			kfree(dp);
		}
	}
	if (xlat_hdl_count > 0 && xlat_hdls) {
		struct ipa_ioc_del_flt_rule *dp;
		size_t sz = sizeof(*dp) + xlat_hdl_count * sizeof(struct ipa_flt_rule_del);

		dp = kzalloc(sz, GFP_KERNEL);
		if (dp) {
			int k;

			dp->commit = 1;
			dp->num_hdls = xlat_hdl_count;
			dp->ip = IPA_IP_v4;
			for (k = 0; k < xlat_hdl_count; k++) {
				dp->hdl[k].status = -1;
				dp->hdl[k].hdl = xlat_hdls[k];
			}
			ipa3_del_flt_rule(dp);
			kfree(dp);
		}
	}
	kfree(base_hdls);
	kfree(xlat_hdls);
	kfree(flt_index);
	if (pFilteringTable) {
		if (pFilteringTable->rules)
			kfree((void *)(uintptr_t)pFilteringTable->rules);
		kfree(pFilteringTable);
	}
	return retval;
}

int ipa_be_v4_add_uplink_filter_rule(struct ipa_ipv4_rule_create_msg v4_msg, bool lan2lan, int pdn_iface, int client_iface)
{
	struct ipa_ioc_query_intf client_intf, pdn_intf;
	int retval = 0;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	struct ipa_ioc_query_intf_ext_props *ext_prop = NULL;
	bool is_xlat = false;
	bool has_vlan = false;
	uint16_t vlan_id = 0;
	uint16_t vlan_key;
	uint8_t pdn_mux_id = 0;
	struct ipa_ul_group *base_group = NULL;
	struct ipa_ul_group *xlat_group = NULL;
	bool base_new_group = false, base_new_pdn = false;
	bool xlat_new_group = false, xlat_new_pdn = false;
	int ret;

	/* Hoisted up front: the VLAN match equation is per-flow, not
	 * per-rule, so the XLAT group's key must be known before touching
	 * group tracking below.
	 */
	if (v4_msg.vlan_primary_rule.egress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED) {
		vlan_id = v4_msg.vlan_primary_rule.egress_vlan_tag & 0x0FFF;
		has_vlan = true;
		IPA_BE_DBG("Vlan egress client vlan id %d \n", vlan_id);
	} else if (v4_msg.vlan_primary_rule.ingress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED) {
		vlan_id = v4_msg.vlan_primary_rule.ingress_vlan_tag & 0x0FFF;
		has_vlan = true;
		IPA_BE_DBG("Vlan ingress client vlan id %d \n", vlan_id);
	}
	vlan_key = has_vlan ? vlan_id : IPA_UL_VLAN_NONE;

	/* Auto-enable XLAT for IPv4 192.0.0.x (192.0.0.0/24) flow/return IPs */
	if (((ntohl(v4_msg.tuple.flow_ip) & 0xFFFFFF00U) == 0xC0000000U) ||
	    ((ntohl(v4_msg.tuple.return_ip) & 0xFFFFFF00U) == 0xC0000000U) ||
		((ntohl(v4_msg.conn_rule.flow_ip_xlate) & 0xFFFFFF00U) == 0xC0000000U) ||
		((ntohl(v4_msg.conn_rule.return_ip_xlate) & 0xFFFFFF00U) == 0xC0000000U)) {
		is_xlat = true;
		IPA_BE_DBG("XLAT enabled due to 192.0.0.x match: flow %pI4n return %pI4n\n",
			   &v4_msg.tuple.flow_ip, &v4_msg.tuple.return_ip);

		/* Dummy v6_msg carrying the same VLAN key for consistent
		 * companion logging only -- the v6 XLAT companion group is
		 * EtherType-matched, not VLAN-keyed.
		 */
		struct ipa_ipv6_rule_create_msg v6_msg_xlat;
		memset(&v6_msg_xlat, 0, sizeof(v6_msg_xlat));
		v6_msg_xlat.vlan_primary_rule.egress_vlan_tag = IPA_VLAN_ID_NOT_CONFIGURED;
		v6_msg_xlat.vlan_primary_rule.ingress_vlan_tag = IPA_VLAN_ID_NOT_CONFIGURED;
		if (has_vlan)
			v6_msg_xlat.vlan_primary_rule.egress_vlan_tag = vlan_id;

		/* Called unconditionally for every is_xlat flow regardless of
		 * VLAN, so the v6 XLAT companion group's ref_count sums the
		 * total v4-XLAT flow count across every VLAN.
		 */
		ipa_be_v6_add_uplink_filter_rule(v6_msg_xlat, lan2lan, pdn_iface, client_iface, is_xlat);
	}

	IPA_BE_DBG("ECMIPA entry lan2lan %d\n", lan2lan);

	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(client_iface, &client_intf)) {
		retval = -EINVAL;
		goto end;
	} else {
		IPA_BE_DBG("Interface with index %u exist.\n", client_iface);
	}

	rx_prop = (struct ipa_ioc_query_intf_rx_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_rx_props) +
		client_intf.num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (rx_prop == NULL) {
		IPA_BE_ERR("Unable to allocate rx_prop memory.\n");
		retval = -EINVAL;
		goto end;
	}
	memcpy(rx_prop->name, client_intf.name, sizeof(client_intf.name));
	rx_prop->num_rx_props = client_intf.num_rx_props;
	IPA_BE_DBG("Query rx_prop %d name %s\n", rx_prop->num_rx_props, client_intf.name);
	ipa3_query_intf_rx_props(rx_prop);

	/* Queried against this specific pdn_iface every call (even when the
	 * base group already exists), so a newly attaching PDN's own mux_id
	 * can be cached/notified.
	 */
	if (!ipa3_query_iface(pdn_iface, &pdn_intf)) {
		retval = -EINVAL;
		goto end;
	} else {
		IPA_BE_DBG("Interface with index %u exist.\n", pdn_iface);
	}
	ext_prop = (struct ipa_ioc_query_intf_ext_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_ext_props) +
		pdn_intf.num_ext_props * sizeof(struct ipa_ioc_ext_intf_prop), GFP_KERNEL);
	if (ext_prop == NULL) {
		IPA_BE_ERR("Unable to allocate ext_prop memory.\n");
		retval = -EINVAL;
		goto end;
	}
	memcpy(ext_prop->name, pdn_intf.name, sizeof(pdn_intf.name));
	ext_prop->num_ext_props = pdn_intf.num_ext_props;
	IPA_BE_DBG("Query ext_prop %d name %s\n", ext_prop->num_ext_props, ext_prop->name);
	ipa3_query_intf_ext_props(ext_prop);

	pdn_mux_id = ext_prop->num_ext_props > 0 ? ext_prop->ext[0].mux_id : 0;

	/* Base group: PDN-independent, installed once per (client, v4). */
	ret = ipa_ul_group_acquire(client_iface, IPA_IP_v4, IPA_FLT_RULE_CAT_UPLINK,
				    false, 0, pdn_iface, pdn_mux_id,
				    &base_group, &base_new_group, &base_new_pdn);
	if (ret) {
		retval = ret;
		goto end;
	}

	/* XLAT group: per-VLAN (or non-VLAN), only for XLAT flows. Acquired
	 * before any HW build so that if base_group is ALSO newly created
	 * in this same call, both groups can be committed together in one
	 * HW add, per the modem's rule_id-order requirement.
	 */
	if (is_xlat) {
		ret = ipa_ul_group_acquire(client_iface, IPA_IP_v4, IPA_FLT_RULE_CAT_UPLINK,
					    true, vlan_key, pdn_iface, pdn_mux_id,
					    &xlat_group, &xlat_new_group, &xlat_new_pdn);
		if (ret) {
			/* XLAT is best-effort layered on top of a working base
			 * uplink path -- don't fail the whole call for it.
			 */
			IPA_BE_ERR("Failed to acquire v4 XLAT uplink group client %d vlan %u: %d\n",
				client_iface, vlan_key, ret);
			xlat_group = NULL;
		}
	}

	if (base_new_group && xlat_new_group) {
		/* Combined: both groups are brand new in this exact call. */
		retval = ipa_be_v4_build_install_uplink_rows(base_group, xlat_group, NULL,
				rx_prop, ext_prop, has_vlan, vlan_id, client_iface, pdn_mux_id);
		if (retval) {
			IPA_BE_ERR("Combined base+XLAT v4 UL install failed client %d vlan %u: %d\n",
				client_iface, vlan_key, retval);
			ipa_ul_group_abort_new(xlat_group);
			xlat_group = NULL;
			ipa_ul_group_abort_new(base_group);
			base_group = NULL;
			goto end;
		}
		ipa_ul_group_install_done(base_group);
		ipa_ul_group_install_done(xlat_group);
	} else {
		if (base_new_group) {
			retval = ipa_be_v4_build_install_uplink_rows(base_group, NULL, NULL,
					rx_prop, ext_prop, false, 0, client_iface, pdn_mux_id);
			if (retval) {
				ipa_ul_group_abort_new(base_group);
				base_group = NULL;
				/*
				 * xlat_group here is necessarily acquired-existing
				 * (xlat_new_group==false in this arm; the combined
				 * branch above handles the brand-new xlat case).
				 * Release the ref/pdn_ref that acquire bumped so
				 * counters stay balanced on this failure path.
				 * Must use release(), not abort_new(): the latter
				 * unlinks and frees unconditionally and would
				 * corrupt a group that other callers still hold.
				 */
				if (xlat_group) {
					struct ipa_ul_group *empty_group = NULL;
					bool pdn_removed = false;
					uint8_t removed_mux_id = 0;
					uint32_t removed_src_pipe = 0, removed_ebd_pipe = 0;
					int rel_ret;

					rel_ret = ipa_ul_group_release(client_iface, IPA_IP_v4,
							IPA_FLT_RULE_CAT_UPLINK, true, vlan_key,
							pdn_iface, &empty_group, &pdn_removed,
							&removed_mux_id, &removed_src_pipe,
							&removed_ebd_pipe);
					if (rel_ret < 0)
						IPA_BE_ERR("xlat release after base-install fail client %d vlan %u: %d\n",
							client_iface, vlan_key, rel_ret);
					if (empty_group)
						ipa_ul_group_teardown(empty_group);
					xlat_group = NULL;
				}
				goto end;
			}
			ipa_ul_group_install_done(base_group);
		} else if (base_new_pdn) {
			IPA_BE_DBG("Base v4 UL group already installed for client %d; new pdn %d (mux %d) joins it\n",
				client_iface, pdn_iface, pdn_mux_id);
		} else {
			IPA_BE_DBG("Uplink filter for pdn %d, client %d, IP type %d already installed/known.\n",
				pdn_iface, client_iface, IPA_IP_v4);
		}

		if (xlat_group && xlat_new_group) {
			/* Base already existed (or just failed/skipped above);
			 * build the XLAT row standalone, borrowing the base
			 * group's cached priority map for ordering.
			 */
			int xlat_ret = ipa_be_v4_build_install_uplink_rows(NULL, xlat_group,
					base_group, rx_prop, ext_prop, has_vlan, vlan_id,
					client_iface, pdn_mux_id);
			if (xlat_ret) {
				IPA_BE_ERR("Failed to install v4 XLAT uplink row(s) client %d vlan %u: %d\n",
					client_iface, vlan_key, xlat_ret);
				ipa_ul_group_abort_new(xlat_group);
				xlat_group = NULL;
			} else {
				ipa_ul_group_install_done(xlat_group);
			}
		} else if (xlat_group && xlat_new_pdn) {
			/* Existing XLAT group, no rule-set change -- the
			 * consolidated notify below (gated on xlat_new_pdn)
			 * is what tells the modem this pdn now has rules, not
			 * a separate per-group notify.
			 */
			IPA_BE_DBG("v4 XLAT uplink group vlan %u already installed for client %d; new pdn %d joins it\n",
				vlan_key, client_iface, pdn_iface);
		}
	}

	/* Notify-only fallback for a PDN joining an existing group with no
	 * build. OR'd, not &&'d: xlat_new_group isn't reset on an abort, so
	 * && would wrongly suppress the base group's own notify too.
	 */
	if ((!base_new_group && base_new_pdn) || (!xlat_new_group && xlat_new_pdn))
		ipa_ul_send_client_notify(client_iface, IPA_IP_v4, pdn_mux_id);

end:
	if (rx_prop)
		kfree(rx_prop);
	if (ext_prop)
		kfree(ext_prop);

	IPA_BE_DBG("Exit retval %d \n", retval);
	return retval;
}
EXPORT_SYMBOL(ipa_be_v4_add_uplink_filter_rule);

int ipa_be_delete_rules_by_category(int intf_num, int category, enum ipa_ip_type iptype)
{
	struct ipa3_flt_entry flt_entry = {0};
	int flt_hdl;
	int ret;
	int retval = 0;

	flt_entry.cat = category;
	flt_entry.ip_type = iptype;
	for (;;) {
		struct ipa3_flt_entry popped;

		/* Snapshot the entry so it can be restored if the HW delete
		 * fails; the primitive frees the SW entry on match.
		 */
		flt_hdl = ipa3_delete_filter_rules_entry_snapshot(intf_num, flt_entry, &popped);
		if (flt_hdl == -1)
			break;
		struct ipa_ioc_del_flt_rule flt_param = {0};

		flt_param.commit = 1;
		flt_param.num_hdls = 1;
		flt_param.ip = iptype;
		flt_param.hdl[0].status = -1;
		flt_param.hdl[0].hdl = flt_hdl;
		IPA_BE_DBG("Deleting filter hdl:(0x%x) with cat:%d ip type: %d\n", flt_hdl, category, iptype);
		ret = ipa3_del_flt_rule((struct ipa_ioc_del_flt_rule *)&flt_param);
		if (ret || flt_param.hdl[0].status != 0) {
			IPA_BE_ERR("HW delete failed intf:%d cat:%d ip:%d hdl:%d ret:%d/%d; restoring SW entry\n",
				intf_num, category, iptype, flt_hdl, ret, flt_param.hdl[0].status);
			/* Restore the SW entry and stop (re-matching it would
			 * spin); let the caller retry the whole teardown.
			 */
			if (!ipa3_add_filter_rules_entry(intf_num, popped))
				IPA_BE_ERR("CRITICAL: failed to restore SW filter entry hdl:%d intf:%d; SW/HW desynced\n",
					flt_hdl, intf_num);
			retval = ret ? ret : -EFAULT;
			break;
		}
		IPA_BE_DBG("Filter rule deleted successfully for intf:%d cat:%d ip:%d flt_hdl:0x%x\n",
			intf_num, category, iptype, flt_hdl);
	}
	return retval;
}
EXPORT_SYMBOL(ipa_be_delete_rules_by_category);

int ipa_be_construct_mtu_rule(enum ipa_ip_type iptype, uint16_t mtu, int intf_num, int pdn_iface, int vlan_tag)
{
	struct ipa_ioc_add_flt_rule_v2 *pFilteringTable = NULL;
	struct ipa_flt_rule_add_v2 flt_rule_entry;
	struct ipa_ioc_query_intf temp_intf;
	int retval = 0, len = 0;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	int total_rules = 1;
	int idx = 0;
	int flt_hdl = 0;
	int j = 0;
	struct ipa_ioc_get_rt_tbl rt_tbl = {0};
	struct ipa3_flt_entry flt_entry = {0};
	struct ipa_mtu_rule_pair *new_mtu_pair = NULL;
	struct ipa_mtu_rule_pair *existing_pair;

	/* An MTU of 0 (v4) or <= IPV6_HEADER_SIZE (v6) would build a match-all
	 * length range and except the whole flow to software. Skip the rule
	 * instead: degraded PMTUD beats a stalled datapath. Return 0 so the
	 * caller still offloads the flow.
	 */
	if (mtu == 0 || (iptype == IPA_IP_v6 && mtu <= IPV6_HEADER_SIZE)) {
		IPA_BE_ERR("Unusable MTU %u for ip_type %d on client %d pdn %d; skipping MTU rule\n",
			mtu, iptype, intf_num, pdn_iface);
		return 0;
	}

	/* Check if MTU rule already installed for this (client, pdn, ip) tuple */
	spin_lock_bh(&ipa_mtu_rule_lock);
	list_for_each_entry(existing_pair, &ipa_mtu_rule_pairs_list, node) {
		if (existing_pair->client_iface == intf_num &&
		    existing_pair->pdn_iface == pdn_iface &&
		    existing_pair->ip_type == iptype) {
			atomic_inc(&existing_pair->ref_count);
			spin_unlock_bh(&ipa_mtu_rule_lock);
			IPA_BE_DBG("MTU rule for client %d, pdn %d, ip_type %d already installed. ref_count: %d\n",
				intf_num, pdn_iface, iptype, atomic_read(&existing_pair->ref_count));
			return 0;
		}
	}
	spin_unlock_bh(&ipa_mtu_rule_lock);

	/* Delete all existing MTU rules for this intf_num before adding new one. */
	ipa_be_delete_rules_by_category(intf_num, IPA_FLT_RULE_CAT_MTU, iptype);

	IPA_BE_DBG("ECMIPA entry mtu rule size %d\n", mtu);

	/* Do not store ipa3_query_iface()'s bool return in retval -- success
	 * would leave retval == 1, and callers unwind on "ret != 0". */
	if (!ipa3_query_iface(intf_num, &temp_intf)) {
		IPA_BE_DBG("Interface with index %u does not exist.\n", intf_num);
		retval = -EINVAL;
		goto end;
	}

	rx_prop = (struct ipa_ioc_query_intf_rx_props *)kzalloc(
		sizeof(struct ipa_ioc_query_intf_rx_props) +
		temp_intf.num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop),
		GFP_KERNEL);
	if (rx_prop == NULL) {
		IPA_BE_ERR("Unable to allocate rx_prop memory.\n");
		retval = -EINVAL;
		goto end;
	}

	memcpy(rx_prop->name, temp_intf.name, sizeof(temp_intf.name));
	rx_prop->num_rx_props = temp_intf.num_rx_props;
	IPA_BE_DBG("Query rx_prop %d name %s\n", rx_prop->num_rx_props, temp_intf.name);
	ipa3_query_intf_rx_props(rx_prop);

	/* avoid out-of-bounds access when logging below */
	if (rx_prop->num_rx_props > 0) {
		IPA_BE_DBG("Query response rx_prop src %d hdr_l2_type %d\n",
			rx_prop->rx[idx].src_pipe, rx_prop->rx[idx].hdr_l2_type);
	}

	pFilteringTable = (struct ipa_ioc_add_flt_rule_v2 *)kzalloc(sizeof(struct ipa_ioc_add_flt_rule_v2), GFP_KERNEL);
	if (!pFilteringTable) {
		IPA_BE_ERR("Failed to allocate ipa_ioc_add_flt_rule_v2 memory...\n");
		retval = -EINVAL;
		goto end;
	}

	pFilteringTable->rules = (uintptr_t)kzalloc(total_rules * sizeof(struct ipa_flt_rule_add_v2), GFP_KERNEL);
	if (!pFilteringTable->rules) {
		kfree(pFilteringTable);
		pFilteringTable = NULL;
		IPA_BE_ERR("Failed to allocate ipa_flt_rule_add_v2 memory...\n");
		retval = -EINVAL;
		goto end;
	}

	IPA_BE_DBG("Install rules on Rx pipe at idx %d src_pipe %d\n",
		idx, (rx_prop->num_rx_props > 0) ? rx_prop->rx[idx].src_pipe : -1);

	pFilteringTable->commit = 1;
	pFilteringTable->ep = (rx_prop->num_rx_props > 0) ? rx_prop->rx[idx].src_pipe : 0;
	pFilteringTable->ip = iptype; /* use requested IP type */
	pFilteringTable->num_rules = total_rules;
	pFilteringTable->flt_rule_size = sizeof(struct ipa_flt_rule_add_v2);
	/* pFilteringTable->add_after_hdl = 0; //tbd */

	for (j = 0; j < rx_prop->num_rx_props / 2; j++) {
		if (iptype == IPA_IP_v6)
			idx = j * 2 + 1;
		else
			idx = j * 2;

		if (idx >= rx_prop->num_rx_props)  /* bounds check */
			continue;

		if (rx_prop->rx[idx].ip != iptype) {
			IPA_BE_DBG("IP not matching required type %d .. continue\n", rx_prop->rx[idx].ip);
			continue;
		}

		pFilteringTable->ep = rx_prop->rx[idx].src_pipe;

		IPA_BE_DBG("Install rules at idx %d\n", idx);
		memset(&flt_rule_entry, 0, sizeof(flt_rule_entry));

		flt_rule_entry.rule.retain_hdr = 0;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
		flt_rule_entry.rule.eq_attrib_type = 1;

		rt_tbl.ip = iptype;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(rt_tbl.name, (iptype == IPA_IP_v6) ? V6_WAN_ROUTE_TABLE_NAME : V4_LAN_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#else
		strlcpy(rt_tbl.name, (iptype == IPA_IP_v6) ? V6_WAN_ROUTE_TABLE_NAME : V4_LAN_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#endif

		IPA_BE_DBG("This flt rule points to rt tbl %s.\n", rt_tbl.name);

		if (ipa3_get_rt_tbl((struct ipa_ioc_get_rt_tbl *)&rt_tbl)) {
			IPA_BE_ERR("ECMIPA failed to get route hdl \n");
			retval = -EFAULT;
			goto end;
		}
		IPA_BE_DBG("Install filter rules with rt_tbl.hdl %d \n", rt_tbl.hdl);
		IPA_BE_DBG("Install rules on Rx pipe at idx %d \n", idx);

		flt_rule_entry.rule.rt_tbl_hdl = rt_tbl.hdl;
		/* Transient lookup ref; release inside the loop since each
		 * iteration re-looks-up. */
		ipa_put_rt_tbl(rt_tbl.hdl);
		flt_rule_entry.rule.hashable = true;
		/* flt_rule_entry.rule.rule_id = 1; //LAN2LAN_RULE_ID */

		memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[idx].attrib,
			sizeof(flt_rule_entry.rule.attrib));

		if (vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED)
		{
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_VLAN_ID;
			flt_rule_entry.rule.attrib.vlan_id = vlan_tag & 0x3FF;
		}

		flt_rule_entry.flt_rule_category = IPA_FLT_RULE_CAT_MTU;

		if (flt_rule_entry.rule.attrib.attrib_mask) {
			struct ipa_ioc_generate_flt_eq flt_eq;

			memset(&flt_eq, 0, sizeof(flt_eq));
			memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib,
				sizeof(flt_eq.attrib));
			flt_eq.ip = iptype;

			if (ipahal_flt_generate_equation(iptype, &flt_eq.attrib, &flt_eq.eq_attrib)) {
				IPA_BE_ERR("Failed generating equation for MTU rule\n");
				retval = -EFAULT;
				goto end;
			}
			memcpy(&flt_rule_entry.rule.eq_attrib, &flt_eq.eq_attrib,
				sizeof(flt_rule_entry.rule.eq_attrib));
		}

		flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1<<10);
		flt_rule_entry.rule.eq_attrib.num_ihl_offset_range_16 = 1;

		if (iptype == IPA_IP_v4) {
			flt_rule_entry.rule.eq_attrib.ihl_offset_range_16[0].offset = 0x82;
			flt_rule_entry.rule.eq_attrib.ihl_offset_range_16[0].range_low = mtu + 1;
		} else {
			flt_rule_entry.rule.eq_attrib.ihl_offset_range_16[0].offset = 0x84;
			/* v6 uses payload length which doesnt include v6 header */
			flt_rule_entry.rule.eq_attrib.ihl_offset_range_16[0].range_low = mtu + 1 - IPV6_HEADER_SIZE;
		}
		flt_rule_entry.rule.eq_attrib.ihl_offset_range_16[0].range_high = UINT16_MAX; /* 0xFFFF */

		flt_rule_entry.rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
		memcpy(&(((struct ipa_flt_rule_add_v2 *)(uintptr_t)pFilteringTable->rules)[0]), &flt_rule_entry, sizeof(flt_rule_entry));

		if (ipa3_add_flt_rule_usr_v2((struct ipa_ioc_add_flt_rule_v2 *)pFilteringTable, true)) {
			IPA_BE_ERR("ipa3_add_flt_rule_usr_v2 failed\n");
			retval = -EFAULT;
			goto end;
		} else {
			flt_hdl = (((struct ipa_flt_rule_add_v2 *)(uintptr_t)pFilteringTable->rules)[0]).flt_rule_hdl;
			IPA_BE_DBG("MTU rule %d filter rule hdl: %d\n", mtu, flt_hdl);
			flt_entry.flt_hdl = flt_hdl;
			flt_entry.cat = IPA_FLT_RULE_CAT_MTU;
			flt_entry.ip_type = iptype;

			ipa3_add_filter_rules_entry(intf_num, flt_entry);
		}
	}

	new_mtu_pair = kzalloc(sizeof(struct ipa_mtu_rule_pair), GFP_KERNEL);
	if (!new_mtu_pair) {
		IPA_BE_ERR("Failed to allocate memory for MTU rule tracking\n");
	} else {
		new_mtu_pair->client_iface = intf_num;
		new_mtu_pair->pdn_iface = pdn_iface;
		new_mtu_pair->ip_type = iptype;
		atomic_set(&new_mtu_pair->ref_count, 1);
		spin_lock_bh(&ipa_mtu_rule_lock);
		list_add(&new_mtu_pair->node, &ipa_mtu_rule_pairs_list);
		spin_unlock_bh(&ipa_mtu_rule_lock);
		IPA_BE_DBG("Added MTU rule tracking: client %d, pdn %d, ip_type %d, ref_count: 1\n",
			intf_num, pdn_iface, iptype);
	}

end:
	if (pFilteringTable) {
		if (pFilteringTable->rules)
			kfree((void *)(uintptr_t)pFilteringTable->rules);
		kfree(pFilteringTable);
	}
	if (rx_prop)
		kfree(rx_prop);
	return retval;
}

/**
 * ipa_be_delete_mtu_rule() - Delete MTU rule with reference counting
 * @client_iface: Client interface number
 * @pdn_iface: PDN interface number
 * @ip_type: IP version (IPA_IP_v4 or IPA_IP_v6)
 *
 * Decrements the reference count for the MTU rule associated with the
 * (client_iface, pdn_iface, ip_type) tuple. The hardware rule is only
 * deleted when the reference count reaches zero (last connection removed).
 *
 * Return: 0 on success, negative errno on failure
 */
int ipa_be_delete_mtu_rule(int client_iface, int pdn_iface, enum ipa_ip_type ip_type)
{
	struct ipa_mtu_rule_pair *pair, *tmp;
	bool found = false;
	bool delete_hw_rules = true;

	IPA_BE_DBG("ipa_be_delete_mtu_rule: client_iface=%d pdn_iface=%d ip_type=%d\n",
		client_iface, pdn_iface, ip_type);

	spin_lock_bh(&ipa_mtu_rule_lock);
	list_for_each_entry_safe(pair, tmp, &ipa_mtu_rule_pairs_list, node) {
		if (pair->client_iface == client_iface &&
		    pair->pdn_iface == pdn_iface &&
		    pair->ip_type == ip_type) {
			atomic_dec(&pair->ref_count);
			IPA_BE_DBG("Decremented MTU rule ref_count for client %d, pdn %d, ip_type %d. New count: %d\n",
				client_iface, pdn_iface, ip_type, atomic_read(&pair->ref_count));
			if (atomic_read(&pair->ref_count) > 0) {
				/* Other connections still using this MTU rule */
				spin_unlock_bh(&ipa_mtu_rule_lock);
				IPA_BE_DBG("MTU rule for client %d, pdn %d, ip_type %d still in use. ref_count: %d\n",
					client_iface, pdn_iface, ip_type, atomic_read(&pair->ref_count));
				return 0;
			}
			/* ref_count reached 0 - remove tracking entry */
			list_del(&pair->node);
			kfree(pair);
			found = true;
			break;
		}
	}

	/* Check if there are any other PDNs using the same client_iface with this IP type */
	if (found) {
		list_for_each_entry(pair, &ipa_mtu_rule_pairs_list, node) {
			if (pair->client_iface == client_iface && pair->ip_type == ip_type) {
				delete_hw_rules = false;
				IPA_BE_DBG("Found another PDN %d using client_iface %d, ip_type %d. Not deleting MTU rule.\n",
					pair->pdn_iface, client_iface, ip_type);
				break;
			}
		}
	}
	spin_unlock_bh(&ipa_mtu_rule_lock);

	if (!found) {
		IPA_BE_ERR("No MTU rule tracking entry found for client %d, pdn %d, ip_type %d\n",
			client_iface, pdn_iface, ip_type);
		return -ENOENT;
	}

	if (!delete_hw_rules) {
		IPA_BE_DBG("Other PDNs still using client_iface %d. Skipping hardware MTU rule deletion.\n", client_iface);
		return 0;
	}

	IPA_BE_DBG("No other PDNs using client_iface %d, proceeding with hardware MTU rule deletion for pdn %d, ip_type %d.\n",
		client_iface, pdn_iface, ip_type);

	return ipa_be_delete_rules_by_category(client_iface, IPA_FLT_RULE_CAT_MTU, ip_type);
}
EXPORT_SYMBOL(ipa_be_delete_mtu_rule);

/*
 * Find the tracking entry for (intf_num, ip_type, bridge_if_num); caller holds
 * ipa_private_subnet_lock. bridge_if_num is part of the key so an iface enslaved
 * to multiple bridges has one entry per bridge.
 */
static struct ipa_private_subnet_entry *ipa_be_subnet_find_locked(
	int intf_num, enum ipa_ip_type ip_type, int bridge_if_num)
{
	struct ipa_private_subnet_entry *e;

	list_for_each_entry(e, &ipa_private_subnet_entries_list, node) {
		if (e->intf_num == intf_num && e->ip_type == ip_type &&
		    e->bridge_if_num == bridge_if_num)
			return e;
	}
	return NULL;
}

/* True if addr (host byte order) is RFC1918 private: 10/8, 172.16/12, 192.168/16. */
static inline bool ipa_be_is_private_v4(uint32_t addr)
{
	return (addr & 0xFF000000) == 0x0A000000 ||
	       (addr & 0xFFF00000) == 0xAC100000 ||
	       (addr & 0xFFFF0000) == 0xC0A80000;
}

/*
 * Read the first usable private LAN subnet configured on the bridge net_device
 * into the match set. Returns true if one was found.
 *
 * Skips /32 host addresses (in IPPT-without-NAT the bridge holds the WAN address
 * as a single-host /32 with no LAN subnet behind it — and that address can fall
 * inside an RFC1918 range, e.g. 172.21.x.x, so the /32 check is essential) and
 * non-RFC1918 addresses. A bridge may carry both a private LAN IP and a public
 * WAN /32 (IPPT with NAT); this picks the private subnet.
 */
static bool ipa_be_subnet_get_v4(struct net_device *bridge_dev,
	struct ipa_be_subnet_match *m)
{
	struct in_device *in_dev;
	struct in_ifaddr *ifa;
	__be32 ip_addr_be = 0, ip_mask_be = 0;
	bool had_addr = false;

	in_dev = in_dev_get(bridge_dev);
	if (!in_dev)
		return false;

	rcu_read_lock();
	in_dev_for_each_ifa_rcu(ifa, in_dev) {
		uint32_t addr = ntohl(ifa->ifa_address);
		uint32_t mask = ntohl(ifa->ifa_mask);

		had_addr = true;

		if (mask == 0xFFFFFFFF) {
			IPA_BE_DBG("bridge %s: skip %pI4/32 host addr (IPPT without NAT)\n",
				bridge_dev->name, &ifa->ifa_address);
			continue;
		}
		if (!ipa_be_is_private_v4(addr)) {
			IPA_BE_DBG("bridge %s: skip %pI4 (not RFC1918)\n",
				bridge_dev->name, &ifa->ifa_address);
			continue;
		}

		IPA_BE_DBG("bridge %s: use %pI4 mask %pI4 for private-subnet rule\n",
			bridge_dev->name, &ifa->ifa_address, &ifa->ifa_mask);
		ip_addr_be = ifa->ifa_address;
		ip_mask_be = ifa->ifa_mask;
		break;
	}
	rcu_read_unlock();
	in_dev_put(in_dev);

	if (!ip_addr_be) {
		if (had_addr)
			IPA_BE_DBG("bridge %s: no usable private LAN subnet (only /32 host or public addrs); no rule\n",
				bridge_dev->name);
		else
			IPA_BE_DBG("bridge %s: no IPv4 address; no private-subnet rule\n",
				bridge_dev->name);
		return false;
	}

	m->v4_mask = ntohl(ip_mask_be);
	m->v4_addr = ntohl(ip_addr_be) & m->v4_mask;
	return true;
}

/*
 * Collect all global-unicast IPv6 /64 prefixes on the bridge into the match
 * set (upper 64 bits, host byte order). Duplicate prefixes (multiple addresses
 * sharing a /64) are coalesced. Returns true if at least one was found.
 */
static bool ipa_be_subnet_get_v6(struct net_device *bridge_dev,
	struct ipa_be_subnet_match *m)
{
	struct inet6_dev *idev;
	struct inet6_ifaddr *ifp;
	int i;

	idev = in6_dev_get(bridge_dev);
	if (!idev) {
		IPA_BE_DBG("bridge %s: no inet6_dev; no v6 private-subnet rule\n",
			bridge_dev->name);
		return false;
	}

	read_lock_bh(&idev->lock);
	list_for_each_entry(ifp, &idev->addr_list, if_list) {
		int type = ipv6_addr_type(&ifp->addr);
		uint32_t hi, lo;
		bool dup = false;

		if (type & (IPV6_ADDR_LINKLOCAL | IPV6_ADDR_MULTICAST |
			    IPV6_ADDR_LOOPBACK)) {
			IPA_BE_DBG("bridge %s: skip %pI6c (link-local/mcast/loopback, type 0x%x)\n",
				bridge_dev->name, &ifp->addr, type);
			continue;
		}
		if (!(type & IPV6_ADDR_UNICAST)) {
			IPA_BE_DBG("bridge %s: skip %pI6c (not unicast, type 0x%x)\n",
				bridge_dev->name, &ifp->addr, type);
			continue;
		}
		if (ifp->flags & IFA_F_TENTATIVE) {
			IPA_BE_DBG("bridge %s: skip %pI6c (tentative, DAD not complete; flags 0x%x)\n",
				bridge_dev->name, &ifp->addr, ifp->flags);
			continue;
		}

		hi = ntohl(ifp->addr.s6_addr32[0]);
		lo = ntohl(ifp->addr.s6_addr32[1]);

		/* coalesce addresses that share the same /64 prefix */
		for (i = 0; i < m->n_v6; i++) {
			if (m->v6_prefix[i][0] == hi &&
			    m->v6_prefix[i][1] == lo) {
				dup = true;
				break;
			}
		}
		if (dup)
			continue;

		if (m->n_v6 >= IPA_BE_MAX_V6_PREFIXES) {
			IPA_BE_ERR("bridge %s has >%d v6 prefixes; extra ignored\n",
				bridge_dev->name, IPA_BE_MAX_V6_PREFIXES);
			break;
		}

		m->v6_prefix[m->n_v6][0] = hi;
		m->v6_prefix[m->n_v6][1] = lo;
		m->n_v6++;
		IPA_BE_DBG("bridge %s: use %pI6c (/64 prefix 0x%08x:%08x) for v6 private-subnet rule\n",
			bridge_dev->name, &ifp->addr, hi, lo);
	}
	read_unlock_bh(&idev->lock);
	in6_dev_put(idev);

	if (m->n_v6 == 0)
		IPA_BE_DBG("bridge %s: no usable global v6 prefix (only link-local/tentative?); no rule\n",
			bridge_dev->name);

	return m->n_v6 > 0;
}

/*
 * Build and install one private-subnet rule on a single rx pipe for the given
 * match (v4 subnet, or one v6 /64 prefix in pfx[0..1]). Records the rule handle
 * under IPA_FLT_RULE_CAT_PRIVATE_SUBNET so it can be deleted by category.
 */
static int ipa_be_subnet_add_one(int intf_num, enum ipa_ip_type iptype,
	const struct ipa_ioc_rx_intf_prop *rx, int rt_tbl_hdl,
	uint32_t v4_addr, uint32_t v4_mask, const uint32_t *pfx)
{
	struct ipa_ioc_add_flt_rule *tbl;
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa3_flt_entry flt_entry = {0};
	int len, ret = 0;

	len = sizeof(struct ipa_ioc_add_flt_rule) +
		sizeof(struct ipa_flt_rule_add);
	tbl = (struct ipa_ioc_add_flt_rule *)kzalloc(len, GFP_KERNEL);
	if (!tbl) {
		IPA_BE_ERR("Failed to allocate filtering table memory\n");
		return -ENOMEM;
	}

	tbl->commit = 1;
	tbl->ep = rx->src_pipe;
	tbl->global = false;
	tbl->ip = iptype;
	tbl->num_rules = 1;

	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
	flt_rule_entry.at_rear = true;
	flt_rule_entry.rule.retain_hdr = 1;
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;
	flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
	flt_rule_entry.rule.hashable = true;
	flt_rule_entry.rule.rt_tbl_hdl = rt_tbl_hdl;
	flt_rule_entry.flt_rule_category = IPA_FLT_RULE_CAT_PRIVATE_SUBNET;

	memcpy(&flt_rule_entry.rule.attrib, &rx->attrib,
		sizeof(flt_rule_entry.rule.attrib));
	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;

	if (iptype == IPA_IP_v4) {
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = v4_mask;
		flt_rule_entry.rule.attrib.u.v4.dst_addr = v4_addr;
	} else {
		/* match upper 64 bits (/64 prefix) of dst addr */
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = pfx[0];
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = pfx[1];
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x0;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0x0;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x0;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x0;
	}

	memcpy(&tbl->rules[0], &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

	if (ipa3_add_flt_rule_usr((struct ipa_ioc_add_flt_rule *)tbl, true)) {
		IPA_BE_ERR("ipa3_add_flt_rule_usr failed for private subnet rule\n");
		ret = -EFAULT;
		goto out;
	}

	IPA_BE_DBG("Private subnet rule hdl: 0x%x for pipe %d ip %d\n",
		tbl->rules[0].flt_rule_hdl, rx->src_pipe, iptype);

	flt_entry.flt_hdl = tbl->rules[0].flt_rule_hdl;
	flt_entry.cat = IPA_FLT_RULE_CAT_PRIVATE_SUBNET;
	flt_entry.ip_type = iptype;
	ipa3_add_filter_rules_entry(intf_num, flt_entry);

out:
	kfree(tbl);
	return ret;
}

/*
 * Install the private-subnet "pass to default routing" rules on every rx pipe
 * of intf_num that matches iptype. For IPv4 one rule per pipe matches the
 * subnet; for IPv6 one rule per pipe per /64 prefix in the match set. The match
 * is supplied by the caller (derived from the bridge) instead of from a
 * connection tuple.
 *
 * @rolled_back (if non-NULL) is set true only when the fail: path ran its
 * interface-wide rollback, which also wipes other bridges' rules; early
 * failures leave it false so the caller can skip a needless rebuild.
 */
static int ipa_be_subnet_install_rules(int intf_num, enum ipa_ip_type iptype,
	const struct ipa_be_subnet_match *m, bool *rolled_back)
{
	struct ipa_ioc_query_intf temp_intf;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	struct ipa_ioc_get_rt_tbl rt_tbl = {0};
	int retval = 0, idx, p;
	bool rt_tbl_got = false;

	if (rolled_back)
		*rolled_back = false;

	/* Check if the filter interface exists */
	if (!ipa3_query_iface(intf_num, &temp_intf)) {
		IPA_BE_ERR("Interface with index %u does not exist.\n", intf_num);
		return -EINVAL;
	}

	rx_prop = (struct ipa_ioc_query_intf_rx_props *)kzalloc(
		sizeof(struct ipa_ioc_query_intf_rx_props) +
		temp_intf.num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop),
		GFP_KERNEL);
	if (!rx_prop) {
		IPA_BE_ERR("Unable to allocate rx_prop memory.\n");
		return -ENOMEM;
	}

	memcpy(rx_prop->name, temp_intf.name, sizeof(temp_intf.name));
	rx_prop->num_rx_props = temp_intf.num_rx_props;
	ipa3_query_intf_rx_props(rx_prop);

	if (rx_prop->num_rx_props == 0) {
		IPA_BE_ERR("No rx props for iface %s\n", rx_prop->name);
		retval = -EINVAL;
		goto end;
	}

	/* Default routing table - LAN-local traffic goes to Apps via dflt tbl */
	rt_tbl.ip = iptype;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(rt_tbl.name,
		(iptype == IPA_IP_v4) ? V4_DEFAULT_ROUTE_TABLE_NAME :
					V6_DEFAULT_ROUTE_TABLE_NAME,
		sizeof(rt_tbl.name));
#else
	strlcpy(rt_tbl.name,
		(iptype == IPA_IP_v4) ? V4_DEFAULT_ROUTE_TABLE_NAME :
					V6_DEFAULT_ROUTE_TABLE_NAME,
		sizeof(rt_tbl.name));
#endif
	if (ipa3_get_rt_tbl(&rt_tbl)) {
		IPA_BE_ERR("Failed to get default routing table for ip %d\n",
			iptype);
		retval = -EFAULT;
		goto end;
	}
	rt_tbl_got = true;
	IPA_BE_DBG("Private subnet (ip %d) uses rt_tbl hdl=%d\n",
		iptype, rt_tbl.hdl);

	/* Install rules on each rx pipe matching iptype */
	for (idx = 0; idx < rx_prop->num_rx_props; idx++) {
		if (rx_prop->rx[idx].ip != iptype)
			continue;

		if (iptype == IPA_IP_v4) {
			retval = ipa_be_subnet_add_one(intf_num, iptype,
				&rx_prop->rx[idx], rt_tbl.hdl,
				m->v4_addr, m->v4_mask, NULL);
			if (retval)
				goto fail;
		} else {
			/* one rule per /64 prefix on this pipe */
			for (p = 0; p < m->n_v6; p++) {
				retval = ipa_be_subnet_add_one(intf_num, iptype,
					&rx_prop->rx[idx], rt_tbl.hdl,
					0, 0, m->v6_prefix[p]);
				if (retval)
					goto fail;
			}
		}
	}

	kfree(rx_prop);
	return 0;

fail:
	/*
	 * Partial failure: rules already added on earlier pipes/prefixes were
	 * recorded under IPA_FLT_RULE_CAT_PRIVATE_SUBNET but won't be tracked
	 * by the caller. Roll them back. Log a warning if the HW delete itself
	 * fails — untracked HW rules may persist, but there is nothing further
	 * we can do here; the next reconcile will reinstall a clean set.
	 * The delete is interface-wide, so signal the caller it wiped other
	 * bridges' rules too.
	 */
	if (rolled_back)
		*rolled_back = true;
	if (ipa_be_delete_rules_by_category(intf_num,
		IPA_FLT_RULE_CAT_PRIVATE_SUBNET, iptype))
		IPA_BE_ERR("HW rollback failed for intf %d ip %d; untracked rules may linger\n",
			intf_num, iptype);
end:
	/* Lookup ref is transient; each filter rule installed above took its own rt_tbl ref. */
	if (rt_tbl_got)
		ipa_put_rt_tbl(rt_tbl.hdl);
	kfree(rx_prop);
	return retval;
}

/*
 * Reinstall the private-subnet rules for each entry in `entries` (HW assumed
 * already wiped by the caller). On full success the entries are re-tracked. On
 * any failure the loop stops, a category wipe clears partial HW state, and all
 * entries are freed so the next reconcile retries the whole interface.
 * Consumes `entries`: empty on return.
 */
static void ipa_be_subnet_reinstall_entries(int intf_num,
	enum ipa_ip_type iptype, struct list_head *entries)
{
	struct ipa_private_subnet_entry *e, *tmp;
	LIST_HEAD(ok);
	bool any_failed = false;

	list_for_each_entry_safe(e, tmp, entries, node) {
		if (any_failed)
			break;

		list_del(&e->node);
		IPA_BE_DBG("Reinstalling bridge %d rules on intf %d ip %d\n",
			e->bridge_if_num, intf_num, iptype);
		if (ipa_be_subnet_install_rules(intf_num, iptype, &e->match,
				NULL)) {
			IPA_BE_ERR("Reinstall failed bridge %d intf %d ip %d\n",
				e->bridge_if_num, intf_num, iptype);
			kfree(e);
			any_failed = true;
		} else {
			list_add(&e->node, &ok);
		}
	}

	if (!any_failed) {
		spin_lock_bh(&ipa_private_subnet_lock);
		list_splice(&ok, &ipa_private_subnet_entries_list);
		spin_unlock_bh(&ipa_private_subnet_lock);
		return;
	}

	/* Wipe partial HW state, then free all tracking; next reconcile retries. */
	IPA_BE_ERR("Reinstall failure on intf %d ip %d; wiping and clearing tracking\n",
		intf_num, iptype);
	if (ipa_be_delete_rules_by_category(intf_num,
		IPA_FLT_RULE_CAT_PRIVATE_SUBNET, iptype)) {
		/* Wipe failed: reattach ok tracking so it is not lost. */
		IPA_BE_ERR("Emergency wipe failed intf %d ip %d; reattaching ok tracking\n",
			intf_num, iptype);
		spin_lock_bh(&ipa_private_subnet_lock);
		list_splice(&ok, &ipa_private_subnet_entries_list);
		spin_unlock_bh(&ipa_private_subnet_lock);
	} else {
		list_for_each_entry_safe(e, tmp, &ok, node) {
			list_del(&e->node);
			kfree(e);
		}
	}
	list_for_each_entry_safe(e, tmp, entries, node) {
		list_del(&e->node);
		kfree(e);
	}
}

/*
 * Rebuild all tracked private-subnet rules for (intf_num, iptype) from scratch.
 * Called after install_rules' fail: path, whose interface-wide rollback may have
 * wiped other bridges' HW rules while their tracking still claims them installed.
 * The just-failed bridge is already untracked, so it is not reinstalled here.
 */
static void ipa_be_subnet_rebuild(int intf_num, enum ipa_ip_type iptype)
{
	struct ipa_private_subnet_entry *e, *tmp;
	LIST_HEAD(entries);

	spin_lock_bh(&ipa_private_subnet_lock);
	list_for_each_entry_safe(e, tmp, &ipa_private_subnet_entries_list, node) {
		if (e->intf_num == intf_num && e->ip_type == iptype) {
			list_del(&e->node);
			list_add(&e->node, &entries);
		}
	}
	spin_unlock_bh(&ipa_private_subnet_lock);

	if (list_empty(&entries))
		return;

	/* Clear any residual HW rules before reinstalling from tracking. */
	if (ipa_be_delete_rules_by_category(intf_num,
		IPA_FLT_RULE_CAT_PRIVATE_SUBNET, iptype)) {
		/* Wipe failed: reattach tracking, let next reconcile repair. */
		IPA_BE_ERR("Rebuild wipe failed intf %d ip %d; reattaching tracking\n",
			intf_num, iptype);
		spin_lock_bh(&ipa_private_subnet_lock);
		list_splice(&entries, &ipa_private_subnet_entries_list);
		spin_unlock_bh(&ipa_private_subnet_lock);
		return;
	}

	ipa_be_subnet_reinstall_entries(intf_num, iptype, &entries);
}

/*
 * Delete the private-subnet rules for (intf_num, iptype, bridge_if_num) from HW
 * and drop that bridge's tracking entry. The category delete is interface-wide,
 * so survivors are reinstalled via ipa_be_subnet_reinstall_entries().
 * bridge_if_num = 0 removes ALL bridge entries (interface down).
 */
static void ipa_be_subnet_remove(int intf_num, enum ipa_ip_type iptype,
	int bridge_if_num)
{
	struct ipa_private_subnet_entry *e, *tmp;
	LIST_HEAD(survivors);
	bool found = false;

	spin_lock_bh(&ipa_private_subnet_lock);
	list_for_each_entry_safe(e, tmp, &ipa_private_subnet_entries_list, node) {
		if (e->intf_num != intf_num || e->ip_type != iptype)
			continue;
		list_del(&e->node);
		if (bridge_if_num && e->bridge_if_num != bridge_if_num)
			list_add(&e->node, &survivors);
		else {
			kfree(e);
			found = true;
		}
	}
	spin_unlock_bh(&ipa_private_subnet_lock);

	/*
	 * If the target bridge entry was not found but survivors exist, there is
	 * nothing to remove. Reattach survivors and return without triggering a
	 * needless category wipe and reinstall cycle.
	 */
	if (!found) {
		if (!list_empty(&survivors)) {
			spin_lock_bh(&ipa_private_subnet_lock);
			list_splice(&survivors, &ipa_private_subnet_entries_list);
			spin_unlock_bh(&ipa_private_subnet_lock);
		}
		return;
	}

	IPA_BE_DBG("Removing private subnet rules intf %d bridge %d ip %d\n",
		intf_num, bridge_if_num, iptype);

	/* wipe all HW rules for this intf/iptype (category delete is interface-wide) */
	if (ipa_be_delete_rules_by_category(intf_num,
		IPA_FLT_RULE_CAT_PRIVATE_SUBNET, iptype)) {
		/*
		 * HW delete failed. Tracking for the removed bridge is already
		 * gone. Reattach survivors so they remain tracked — reinstalling
		 * on top of a partially-wiped HW state would create duplicates.
		 * The next reconcile will re-evaluate and repair.
		 */
		IPA_BE_ERR("Category wipe failed intf %d ip %d; reattaching survivors\n",
			intf_num, iptype);
		spin_lock_bh(&ipa_private_subnet_lock);
		list_splice(&survivors, &ipa_private_subnet_entries_list);
		spin_unlock_bh(&ipa_private_subnet_lock);
		return;
	}

	if (list_empty(&survivors))
		return;

	/* Survivors were wiped by the category delete above; reinstall them. */
	ipa_be_subnet_reinstall_entries(intf_num, iptype, &survivors);
}

/*
 * Compare two match sets. For IPv6 the prefix set is order-independent (the
 * kernel may enumerate addresses in any order), so compare as sets.
 */
static bool ipa_be_match_equal(enum ipa_ip_type iptype,
	const struct ipa_be_subnet_match *a,
	const struct ipa_be_subnet_match *b)
{
	int i, j;

	if (iptype == IPA_IP_v4)
		return a->v4_addr == b->v4_addr && a->v4_mask == b->v4_mask;

	if (a->n_v6 != b->n_v6)
		return false;

	/* every prefix in a must be present in b (counts equal => set equal) */
	for (i = 0; i < a->n_v6; i++) {
		bool found = false;

		for (j = 0; j < b->n_v6; j++) {
			if (a->v6_prefix[i][0] == b->v6_prefix[j][0] &&
			    a->v6_prefix[i][1] == b->v6_prefix[j][1]) {
				found = true;
				break;
			}
		}
		if (!found)
			return false;
	}
	return true;
}

/*
 * Reconcile intf_num/iptype against the supplied match: install if missing,
 * reprogram if changed, no-op if already installed. Runs in process context
 * (workqueue); HW APIs may sleep, so ipa_private_subnet_lock is never held
 * across them.
 */
static int ipa_be_subnet_program(int intf_num, int bridge_if_num,
	enum ipa_ip_type iptype, const struct ipa_be_subnet_match *m)
{
	struct ipa_private_subnet_entry *e;
	bool exists = false, changed = false;
	bool rolled_back = false;
	int ret;

	spin_lock_bh(&ipa_private_subnet_lock);
	e = ipa_be_subnet_find_locked(intf_num, iptype, bridge_if_num);
	if (e) {
		exists = true;
		changed = !ipa_be_match_equal(iptype, &e->match, m);
	}
	spin_unlock_bh(&ipa_private_subnet_lock);

	if (exists && !changed) {
		IPA_BE_DBG("Private subnet intf %d bridge %d ip %d unchanged, skip\n",
			intf_num, bridge_if_num, iptype);
		return 0;
	}

	/* match changed: tear down the stale rules before reinstalling */
	if (exists && changed)
		ipa_be_subnet_remove(intf_num, iptype, bridge_if_num);

	ret = ipa_be_subnet_install_rules(intf_num, iptype, m, &rolled_back);
	if (ret) {
		IPA_BE_ERR("Failed to install private subnet rules intf %d ip %d ret %d\n",
			intf_num, iptype, ret);
		/* Rebuild other bridges only if the fail: path wiped their rules. */
		if (rolled_back)
			ipa_be_subnet_rebuild(intf_num, iptype);
		return ret;
	}

	/* record the now-installed match so future reconciles can dedup */
	spin_lock_bh(&ipa_private_subnet_lock);
	e = ipa_be_subnet_find_locked(intf_num, iptype, bridge_if_num);
	if (!e) {
		e = kzalloc(sizeof(*e), GFP_ATOMIC);
		if (!e) {
			spin_unlock_bh(&ipa_private_subnet_lock);
			IPA_BE_ERR("subnet track alloc fail; untracked\n");
			return 0;
		}
		e->intf_num = intf_num;
		e->ip_type = iptype;
		list_add(&e->node, &ipa_private_subnet_entries_list);
	}
	e->bridge_if_num = bridge_if_num;
	e->match = *m;
	spin_unlock_bh(&ipa_private_subnet_lock);

	IPA_BE_DBG("Programmed private subnet intf %d bridge %d ip %d\n",
		intf_num, bridge_if_num, iptype);
	return 0;
}


/*
 * Always-on private-subnet reconcile engine.
 *
 * Keeps the private-subnet rule installed on every bridge-enslaved LAN RX iface,
 * independent of flow acceleration. netdevice/inet(6)addr notifiers walk a
 * bridge's slaves; each slave's net->ifindex is its intf_num (set by
 * ipa_register_intf). Notifiers may run atomic and the install path sleeps, so
 * work is deferred to the backend IPv4/IPv6 ordered workqueues.
 */

struct ipa_be_subnet_work {
	struct work_struct work;
	int bridge_ifindex;     /* bridge to reconcile (0 if slave-targeted) */
	int slave_ifindex;      /* if non-zero: remove this slave only (real-dev
				 * ifindex, resolved at queue time) */
	bool slave_was_vlan;    /* slave_ifindex was resolved from a VLAN */
	int slave_bridge_ifnum; /* specific bridge to scope slave removal (0 = all) */
	enum ipa_ip_type ip_type;
	bool remove_only;       /* true: tear down on this bridge's slaves */
};

/*
 * Workqueues used to defer reconcile work out of (possibly atomic) notifier
 * context. Set by ipa_be_subnet_notifier_init() from the backend's existing
 * IPv4/IPv6 ordered workqueues.
 */
static struct workqueue_struct *ipa_be_subnet_v4_wq;
static struct workqueue_struct *ipa_be_subnet_v6_wq;

static bool ipa_be_subnet_notifiers_registered;

/* coalesces bursts of NETDEV_UP/REGISTER into one pending full-bridge reseed */
static atomic_t ipa_be_subnet_seed_all_pending = ATOMIC_INIT(0);
#define IPA_BE_SEED_ALL_DEBOUNCE_MS 50
static struct delayed_work ipa_be_subnet_seed_all_dwork;

static void ipa_be_subnet_queue(int bridge_ifindex, enum ipa_ip_type ip_type,
	bool remove_only);
static void ipa_be_subnet_seed_existing(void);

/*
 * IPv6 globals are tentative until DAD completes and get_v6() skips them, so a
 * reconcile that runs before DAD finds nothing and no further event may arrive.
 * A bounded delayed re-reconcile retries; the budget stops IPPT-without-NAT
 * (legitimately no v6 prefix) from retrying forever.
 */
#define IPA_BE_V6_SETTLE_DELAY_MS 3000
#define IPA_BE_V6_SETTLE_MAX_TRIES 4
static struct delayed_work ipa_be_subnet_v6_settle_dwork;
static int ipa_be_subnet_v6_settle_tries;
static bool ipa_be_subnet_v6_settle_armed;
static DEFINE_SPINLOCK(ipa_be_subnet_settle_lock);

/*
 * Enqueue under the lock so it cannot race deinit, which zeroes the budget under
 * the same lock before cancelling the work. queue_delayed_work() is non-sleeping.
 */
static void ipa_be_subnet_v6_settle_arm(void)
{
	spin_lock_bh(&ipa_be_subnet_settle_lock);
	if (ipa_be_subnet_notifiers_registered &&
	    ipa_be_subnet_v6_wq &&
	    ipa_be_subnet_v6_settle_tries > 0 &&
	    !ipa_be_subnet_v6_settle_armed) {
		ipa_be_subnet_v6_settle_armed = true;
		IPA_BE_DBG("arming v6 settle re-reconcile in %dms (tries left %d)\n",
			IPA_BE_V6_SETTLE_DELAY_MS, ipa_be_subnet_v6_settle_tries);
		queue_delayed_work(ipa_be_subnet_v6_wq,
			&ipa_be_subnet_v6_settle_dwork,
			msecs_to_jiffies(IPA_BE_V6_SETTLE_DELAY_MS));
	}
	spin_unlock_bh(&ipa_be_subnet_settle_lock);
}

/*
 * Refresh the budget on a genuine external event so a later DAD window gets a
 * fresh set of retries. Never call from the settle worker's own re-seed.
 */
static void ipa_be_subnet_v6_settle_reset(void)
{
	spin_lock_bh(&ipa_be_subnet_settle_lock);
	if (ipa_be_subnet_notifiers_registered)
		ipa_be_subnet_v6_settle_tries = IPA_BE_V6_SETTLE_MAX_TRIES;
	spin_unlock_bh(&ipa_be_subnet_settle_lock);
}

static void ipa_be_subnet_v6_settle_fn(struct work_struct *work)
{
	struct net_device *dev;

	spin_lock_bh(&ipa_be_subnet_settle_lock);
	ipa_be_subnet_v6_settle_armed = false;
	if (!ipa_be_subnet_notifiers_registered ||
	    ipa_be_subnet_v6_settle_tries <= 0) {
		spin_unlock_bh(&ipa_be_subnet_settle_lock);
		return;
	}
	ipa_be_subnet_v6_settle_tries--;
	spin_unlock_bh(&ipa_be_subnet_settle_lock);

	IPA_BE_DBG("v6 settle re-reconcile firing (tries left %d)\n",
		ipa_be_subnet_v6_settle_tries);

	rtnl_lock();
	for_each_netdev(&init_net, dev) {
		if (netif_is_bridge_master(dev))
			ipa_be_subnet_queue(dev->ifindex, IPA_IP_v6, false);
	}
	rtnl_unlock();
}


/* per-walk context: collect IPA-registered slave ifindexes under RTNL */
#define IPA_BE_MAX_BRIDGE_SLAVES 16
struct ipa_be_reconcile_ctx {
	int slave_ifindex[IPA_BE_MAX_BRIDGE_SLAVES];
	int n_slaves;
};

/*
 * Collect one slave ifindex (called for each lower dev of the bridge, under
 * RTNL).
 */
static int ipa_be_collect_slave(struct net_device *slave,
	struct netdev_nested_priv *priv)
{
	struct ipa_be_reconcile_ctx *ctx =
		(struct ipa_be_reconcile_ctx *)priv->data;

	if (ctx->n_slaves >= IPA_BE_MAX_BRIDGE_SLAVES) {
		IPA_BE_ERR("bridge has more than %d slaves; some not reconciled\n",
			IPA_BE_MAX_BRIDGE_SLAVES);
		return 0;
	}

	ctx->slave_ifindex[ctx->n_slaves++] = slave->ifindex;
	return 0;
}

/*
 * Reconcile all IPA-registered slaves of bridge_dev for ip_type. Runs in
 * process context (workqueue). Slave enumeration happens under RTNL; the
 * IPA queries and the sleeping HW install/delete happen afterwards with no
 * locks held (to avoid nesting ipa3_ctx->lock under RTNL).
 */
static void ipa_be_subnet_reconcile(struct net_device *bridge_dev,
	int bridge_ifindex, enum ipa_ip_type ip_type, bool remove_only)
{
	struct ipa_be_reconcile_ctx ctx = {0};
	struct netdev_nested_priv priv = {0};
	struct ipa_ioc_query_intf tmp;
	struct ipa_be_subnet_match m = {0};
	bool has_match = false;
	int i;

	/* Read the bridge's current address(es), if any, before walking slaves. */
	if (!remove_only) {
		if (ip_type == IPA_IP_v4)
			has_match = ipa_be_subnet_get_v4(bridge_dev, &m);
		else
			has_match = ipa_be_subnet_get_v6(bridge_dev, &m);
	}

	priv.data = &ctx;
	rtnl_lock();
	netdev_walk_all_lower_dev(bridge_dev, ipa_be_collect_slave, &priv);
	rtnl_unlock();

	for (i = 0; i < ctx.n_slaves; i++) {
		int intf_num = ctx.slave_ifindex[i];
		/*
		 * IPA registers the physical interface (eth0), not VLAN
		 * sub-interfaces (eth0.10). If the bridge slave is a VLAN
		 * device, look through to its real underlying device so the
		 * private-subnet rule is programmed on the correct IPA pipe.
		 * Example: br-lan10 → eth0.10 → real_dev=eth0 → intf_num=eth0->ifindex
		 */
		if (!ipa3_query_iface(intf_num, &tmp)) {
			struct net_device *slave_dev;

			slave_dev = dev_get_by_index(&init_net, intf_num);
			if (slave_dev) {
				if (is_vlan_dev(slave_dev))
					intf_num = vlan_dev_real_dev(slave_dev)->ifindex;
				dev_put(slave_dev);
			}
			if (!ipa3_query_iface(intf_num, &tmp))
				continue;
		}

		if (remove_only || !has_match) {
			IPA_BE_DBG("Reconcile remove: intf %d bridge %d ip %d (%s)\n",
				intf_num, bridge_ifindex, ip_type,
				remove_only ? "remove_only" :
					"no private match (e.g. IPPT without NAT)");
			ipa_be_subnet_remove(intf_num, ip_type, bridge_ifindex);
		} else {
			ipa_be_subnet_program(intf_num, bridge_ifindex,
				ip_type, &m);
		}
	}

	/* v6 install attempt found no prefix: retry in case DAD is still running */
	if (ip_type == IPA_IP_v6 && !remove_only && !has_match)
		ipa_be_subnet_v6_settle_arm();
}

static void ipa_be_subnet_work_fn(struct work_struct *work)
{
	struct ipa_be_subnet_work *w =
		container_of(work, struct ipa_be_subnet_work, work);
	struct net_device *bridge_dev;

	/*
	 * Slave-targeted removal (interface down / released from bridge).
	 * Per-family, queued on that family's workqueue, so it stays FIFO-ordered
	 * against any re-add reconcile. slave_ifindex was resolved to the real
	 * device at queue time, so VLAN entries are found even after the subif
	 * has unregistered.
	 */
	if (w->slave_ifindex) {
		int intf_num = w->slave_ifindex;

		if (w->slave_bridge_ifnum || !w->slave_was_vlan) {
			/*
			 * Scoped remove (CHANGEUPPER unlink) or physical
			 * interface down: remove the specific bridge or all.
			 */
			ipa_be_subnet_remove(intf_num, w->ip_type,
				w->slave_bridge_ifnum);
		} else {
			/*
			 * VLAN subif down with no bridge master: remove each
			 * tracked bridge for this intf/iptype individually rather
			 * than bridge_ifnum=0, so other VLAN bridges on the same
			 * real device are not wiped.
			 */
			struct ipa_private_subnet_entry *e;
			int bridges[IPA_BE_MAX_BRIDGE_SLAVES];
			int n = 0, i;

			spin_lock_bh(&ipa_private_subnet_lock);
			list_for_each_entry(e, &ipa_private_subnet_entries_list,
					node) {
				if (e->intf_num == intf_num &&
				    e->ip_type == w->ip_type &&
				    n < IPA_BE_MAX_BRIDGE_SLAVES)
					bridges[n++] = e->bridge_if_num;
			}
			spin_unlock_bh(&ipa_private_subnet_lock);

			for (i = 0; i < n; i++)
				ipa_be_subnet_remove(intf_num, w->ip_type,
					bridges[i]);
		}
		kfree(w);
		return;
	}

	bridge_dev = dev_get_by_index(&init_net, w->bridge_ifindex);
	if (bridge_dev) {
		ipa_be_subnet_reconcile(bridge_dev, w->bridge_ifindex,
			w->ip_type, w->remove_only);
		dev_put(bridge_dev);
	} else {
		IPA_BE_DBG("bridge ifindex %d gone; skip reconcile\n",
			w->bridge_ifindex);
	}

	kfree(w);
}

/* Queue a reconcile for a bridge on the matching IPv4/IPv6 workqueue. */
static void ipa_be_subnet_queue(int bridge_ifindex, enum ipa_ip_type ip_type,
	bool remove_only)
{
	struct workqueue_struct *wq;
	struct ipa_be_subnet_work *w;

	wq = (ip_type == IPA_IP_v4) ? ipa_be_subnet_v4_wq : ipa_be_subnet_v6_wq;
	if (!wq)
		return;

	w = kzalloc(sizeof(*w), GFP_ATOMIC);
	if (!w) {
		IPA_BE_ERR("Failed to alloc subnet work item\n");
		return;
	}

	INIT_WORK(&w->work, ipa_be_subnet_work_fn);
	w->bridge_ifindex = bridge_ifindex;
	w->ip_type = ip_type;
	w->remove_only = remove_only;
	queue_work(wq, &w->work);
}

/* Queue a full-bridge reseed off any (possibly rtnl-held) caller context; see ipa_be_subnet_seed_existing(). */
/* Full walk vs. a scoped lookup: coalescing already bounds bursts; not worth the extra complexity. */
static void ipa_be_subnet_seed_all_dwork_fn(struct work_struct *work)
{
	/* clear before running: an event arriving mid-debounce must still queue a fresh one */
	atomic_set(&ipa_be_subnet_seed_all_pending, 0);

	/*
	 * Guard against racing teardown: cleanup may have cleared
	 * ipa_be_subnet_notifiers_registered before cancel_delayed_work_sync()
	 * reaches us. Mirrors the check in ipa_be_subnet_queue_seed_all().
	 */
	if (!ipa_be_subnet_notifiers_registered)
		return;

	ipa_be_subnet_seed_existing();
}

static void ipa_be_subnet_queue_seed_all(void)
{
	/* guards against queuing before INIT_DELAYED_WORK() has run, same as v6_settle_arm() */
	if (!ipa_be_subnet_notifiers_registered || !ipa_be_subnet_v4_wq)
		return;

	/* debounce a spread-out burst of UP/REGISTER events into one reseed */
	if (atomic_cmpxchg(&ipa_be_subnet_seed_all_pending, 0, 1))
		return;

	queue_delayed_work(ipa_be_subnet_v4_wq, &ipa_be_subnet_seed_all_dwork,
		msecs_to_jiffies(IPA_BE_SEED_ALL_DEBOUNCE_MS));
}

/*
 * Queue per-family removal for one slave (interface down / released from
 * bridge, where a bridge walk no longer sees it). Per-family on that family's
 * workqueue keeps the remove FIFO-ordered against a re-add reconcile.
 */
static void ipa_be_subnet_queue_slave_remove_family(int slave_ifindex,
	bool slave_was_vlan, enum ipa_ip_type ip_type, int bridge_ifnum)
{
	struct workqueue_struct *wq;
	struct ipa_be_subnet_work *w;

	wq = (ip_type == IPA_IP_v4) ? ipa_be_subnet_v4_wq : ipa_be_subnet_v6_wq;
	if (!wq)
		return;

	w = kzalloc(sizeof(*w), GFP_ATOMIC);
	if (!w) {
		IPA_BE_ERR("Failed to alloc subnet work item\n");
		return;
	}

	INIT_WORK(&w->work, ipa_be_subnet_work_fn);
	w->slave_ifindex = slave_ifindex;
	w->slave_was_vlan = slave_was_vlan;
	w->slave_bridge_ifnum = bridge_ifnum;
	w->ip_type = ip_type;
	queue_work(wq, &w->work);
}

/*
 * Queue per-family removal for a slave; bridge_ifnum scopes which bridge.
 * Resolves VLAN to real device here (under RTNL, netdev still live) since
 * tracking is keyed on the physical ifindex; resolving later could fail.
 */
static void ipa_be_subnet_queue_slave_remove(struct net_device *dev,
	int bridge_ifnum)
{
	int slave_ifindex = dev->ifindex;
	bool was_vlan = false;

	if (is_vlan_dev(dev)) {
		slave_ifindex = vlan_dev_real_dev(dev)->ifindex;
		was_vlan = true;
	}

	ipa_be_subnet_queue_slave_remove_family(slave_ifindex, was_vlan,
		IPA_IP_v4, bridge_ifnum);
	ipa_be_subnet_queue_slave_remove_family(slave_ifindex, was_vlan,
		IPA_IP_v6, bridge_ifnum);
}

/* Queue both address families for a bridge. */
static void ipa_be_subnet_queue_both(int bridge_ifindex, bool remove_only)
{
	/* install-side reconcile is a new topology event: refresh the v6 budget */
	if (!remove_only)
		ipa_be_subnet_v6_settle_reset();

	ipa_be_subnet_queue(bridge_ifindex, IPA_IP_v4, remove_only);
	ipa_be_subnet_queue(bridge_ifindex, IPA_IP_v6, remove_only);
}

/*
 * netdevice notifier: react to bridge enslave/release and interface up/down so
 * the rule follows the topology (a slave appearing under a bridge that already
 * has an address must trigger an install that a pure address notifier misses).
 */
static int ipa_be_netdev_event(struct notifier_block *nb,
	unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct net_device *bridge;

	if (!dev)
		return NOTIFY_DONE;

	switch (event) {
	case NETDEV_CHANGEUPPER: {
		struct netdev_notifier_changeupper_info *info = ptr;
		int b_ifindex;

		if (!info->upper_dev || !netif_is_bridge_master(info->upper_dev))
			break;
		b_ifindex = info->upper_dev->ifindex;
		if (info->linking) {
			ipa_be_subnet_queue_both(b_ifindex, false);
		} else {
			/* scope the remove to this bridge, then re-reconcile it: a sibling port may still reach the same real device */
			ipa_be_subnet_queue_slave_remove(dev, b_ifindex);
			ipa_be_subnet_queue_both(b_ifindex, false);
		}
		break;
	}
	case NETDEV_UP:
	case NETDEV_REGISTER:
		/* re-walk all bridges: a direct-master check on dev misses VLAN-indirect bridges */
		/* no fast-path+fallback: dev can be direct AND VLAN-indirect in different bridges at once */
		ipa_be_subnet_queue_seed_all();
		break;
	case NETDEV_DOWN:
	case NETDEV_UNREGISTER:
		if (!is_vlan_dev(dev)) {
			/* real device itself is gone: every bridge reaching it, direct or via any VLAN child, loses its rule here */
			ipa_be_subnet_queue_slave_remove(dev, 0);
			break;
		}
		/* VLAN subif's own state changed; a sibling port (direct or another VLAN) may still reach the same real device */
		rcu_read_lock();
		bridge = netdev_master_upper_dev_get_rcu(dev);
		if (bridge && netif_is_bridge_master(bridge)) {
			int b_ifindex = bridge->ifindex;

			rcu_read_unlock();
			ipa_be_subnet_queue_slave_remove(dev, b_ifindex);
			ipa_be_subnet_queue_both(b_ifindex, false);
		} else {
			rcu_read_unlock();
			/* not attached: a scoped remove+reconcile already ran at CHANGEUPPER */
		}
		break;
	default:
		break;
	}
	return NOTIFY_DONE;
}

/* IPv4 address add/del on a bridge -> reconcile its slaves. */
static int ipa_be_inetaddr_event(struct notifier_block *nb,
	unsigned long event, void *ptr)
{
	struct in_ifaddr *ifa = (struct in_ifaddr *)ptr;
	struct net_device *dev;

	if (!ifa || !ifa->ifa_dev || !ifa->ifa_dev->dev)
		return NOTIFY_DONE;

	dev = ifa->ifa_dev->dev;
	if (!netif_is_bridge_master(dev))
		return NOTIFY_DONE;

	if (event == NETDEV_UP || event == NETDEV_DOWN)
		ipa_be_subnet_queue(dev->ifindex, IPA_IP_v4, false);

	return NOTIFY_DONE;
}

/* IPv6 address add/del on a bridge -> reconcile its slaves. */
static int ipa_be_inet6addr_event(struct notifier_block *nb,
	unsigned long event, void *ptr)
{
	struct inet6_ifaddr *ifa = (struct inet6_ifaddr *)ptr;
	struct net_device *dev;

	if (!ifa || !ifa->idev || !ifa->idev->dev)
		return NOTIFY_DONE;

	dev = ifa->idev->dev;
	if (!netif_is_bridge_master(dev))
		return NOTIFY_DONE;

	if (event == NETDEV_UP || event == NETDEV_DOWN) {
		ipa_be_subnet_v6_settle_reset();
		ipa_be_subnet_queue(dev->ifindex, IPA_IP_v6, false);
	}

	return NOTIFY_DONE;
}

static struct notifier_block ipa_be_netdev_nb = {
	.notifier_call = ipa_be_netdev_event,
};
static struct notifier_block ipa_be_inetaddr_nb = {
	.notifier_call = ipa_be_inetaddr_event,
};
static struct notifier_block ipa_be_inet6addr_nb = {
	.notifier_call = ipa_be_inet6addr_event,
};

/*
 * Seed the engine with topology that exists at registration time: if bridges/
 * slaves/IPs came up before the notifiers registered, no event would arrive.
 * Queue a reconcile for every existing bridge.
 */
static void ipa_be_subnet_seed_existing(void)
{
	struct net_device *dev;

	rtnl_lock();
	for_each_netdev(&init_net, dev) {
		if (netif_is_bridge_master(dev))
			ipa_be_subnet_queue_both(dev->ifindex, false);
	}
	rtnl_unlock();
}

/* Re-walk all bridges: a direct-master check on the registering intf misses VLAN-indirect bridges (bridge2 <- eth0.2 <- eth0). */
void ipa_be_subnet_on_intf_registered(void)
{
	if (!ipa_be_subnet_notifiers_registered)
		return;

	IPA_BE_DBG("intf registered; re-seeding bridge reconcile\n");
	ipa_be_subnet_queue_seed_all();
}
EXPORT_SYMBOL(ipa_be_subnet_on_intf_registered);

/* Call synchronously before the caller frees its interface entry, so the HW delete can still find it. Sleep-capable context required. */
/* May race a concurrently queued notifier reconcile for the same intf_num; pre-existing, narrow, accepted. */
void ipa_be_subnet_on_intf_unregistering(int slave_ifindex)
{
	if (!ipa_be_subnet_notifiers_registered)
		return;

	ipa_be_subnet_remove(slave_ifindex, IPA_IP_v4, 0);
	ipa_be_subnet_remove(slave_ifindex, IPA_IP_v6, 0);
}
EXPORT_SYMBOL(ipa_be_subnet_on_intf_unregistering);

int ipa_be_subnet_notifier_init(struct workqueue_struct *v4_wq,
	struct workqueue_struct *v6_wq)
{
	int ret;

	if (ipa_be_subnet_notifiers_registered)
		return 0;

	ipa_be_subnet_v4_wq = v4_wq;
	ipa_be_subnet_v6_wq = v6_wq;

	ret = register_netdevice_notifier(&ipa_be_netdev_nb);
	if (ret) {
		IPA_BE_ERR("register_netdevice_notifier failed %d\n", ret);
		goto err_netdev;
	}
	ret = register_inetaddr_notifier(&ipa_be_inetaddr_nb);
	if (ret) {
		IPA_BE_ERR("register_inetaddr_notifier failed %d\n", ret);
		goto err_inet;
	}
	ret = register_inet6addr_notifier(&ipa_be_inet6addr_nb);
	if (ret) {
		IPA_BE_ERR("register_inet6addr_notifier failed %d\n", ret);
		goto err_inet6;
	}

	/* init budget and worker before marking active so arm() never sees them uninit */
	spin_lock_bh(&ipa_be_subnet_settle_lock);
	ipa_be_subnet_v6_settle_tries = IPA_BE_V6_SETTLE_MAX_TRIES;
	ipa_be_subnet_v6_settle_armed = false;
	spin_unlock_bh(&ipa_be_subnet_settle_lock);
	INIT_DELAYED_WORK(&ipa_be_subnet_v6_settle_dwork,
		ipa_be_subnet_v6_settle_fn);
	INIT_DELAYED_WORK(&ipa_be_subnet_seed_all_dwork,
		ipa_be_subnet_seed_all_dwork_fn);
	atomic_set(&ipa_be_subnet_seed_all_pending, 0);

	ipa_be_subnet_notifiers_registered = true;
	IPA_BE_DBG("private-subnet notifiers registered\n");

	/* program rules for any topology that already exists */
	ipa_be_subnet_seed_existing();
	return 0;

err_inet6:
	unregister_inetaddr_notifier(&ipa_be_inetaddr_nb);
err_inet:
	unregister_netdevice_notifier(&ipa_be_netdev_nb);
err_netdev:
	ipa_be_subnet_v4_wq = NULL;
	ipa_be_subnet_v6_wq = NULL;
	return ret;
}
EXPORT_SYMBOL(ipa_be_subnet_notifier_init);

/*
 * Stop the engine: unregister notifiers and drop workqueue refs so no new work
 * is queued. Call BEFORE the backend drains the workqueues. Tracking is freed
 * later by ipa_be_subnet_notifier_cleanup() (after drain), as in-flight work may
 * still touch the list.
 */
void ipa_be_subnet_notifier_deinit(void)
{
	if (!ipa_be_subnet_notifiers_registered)
		return;

	unregister_netdevice_notifier(&ipa_be_netdev_nb);
	unregister_inetaddr_notifier(&ipa_be_inetaddr_nb);
	unregister_inet6addr_notifier(&ipa_be_inet6addr_nb);
	ipa_be_subnet_notifiers_registered = false;

	/* zero budget under the lock, then cancel: no queue-after-cancel window */
	spin_lock_bh(&ipa_be_subnet_settle_lock);
	ipa_be_subnet_v6_settle_tries = 0;
	spin_unlock_bh(&ipa_be_subnet_settle_lock);
	cancel_delayed_work_sync(&ipa_be_subnet_v6_settle_dwork);
	cancel_delayed_work_sync(&ipa_be_subnet_seed_all_dwork);
	atomic_set(&ipa_be_subnet_seed_all_pending, 0);

	ipa_be_subnet_v4_wq = NULL;
	ipa_be_subnet_v6_wq = NULL;
	IPA_BE_DBG("private-subnet notifiers unregistered\n");
}
EXPORT_SYMBOL(ipa_be_subnet_notifier_deinit);

/*
 * Free leftover tracking entries. Call only after the backend workqueues have
 * been drained/destroyed (so no reconcile work can repopulate the list).
 */
void ipa_be_subnet_notifier_cleanup(void)
{
	struct ipa_private_subnet_entry *e, *tmp;

	spin_lock_bh(&ipa_private_subnet_lock);
	list_for_each_entry_safe(e, tmp, &ipa_private_subnet_entries_list, node) {
		list_del(&e->node);
		kfree(e);
	}
	spin_unlock_bh(&ipa_private_subnet_lock);
}
EXPORT_SYMBOL(ipa_be_subnet_notifier_cleanup);


/*
 * Builds and installs the v6 base uplink rows (PDN-independent, one row
 * per ext_prop entry with ip==IPA_IP_v6, same content regardless of which
 * PDN or VLAN triggered the install). Populates
 * g->flt_hdls/num_hdls/rule_id[]/notif_* on success; returns a negative
 * errno on failure (caller must ipa_ul_group_abort_new(g)).
 */
static int ipa_be_v6_build_install_base_rows(struct ipa_ul_group *g,
	struct ipa_ioc_query_intf_rx_props *rx_prop,
	struct ipa_ioc_query_intf_ext_props *ext_prop,
	int client_iface, uint8_t mux_id)
{
	struct ipa_ioc_add_flt_rule_v2 *pFilteringTable = NULL;
	struct ipa_flt_rule_add_v2 flt_rule_entry;
	struct ipa_fltr_installed_notif_req_msg_v01 *flt_index = NULL;
	struct ipa3_flt_entry flt_entry = {0};
	enum ipa_flt_action action_cache;
	struct list_head local_prio_list;
	int total_rules = 0;
	int idx = 0, j, cnt, i = 0, index;
	int retval = 0;
	uint32_t *installed_hdls = NULL;
	int installed_count = 0;

	INIT_LIST_HEAD(&local_prio_list);

	for (cnt = 0; cnt < ext_prop->num_ext_props; cnt++) {
		if (ext_prop->ext[cnt].ip == IPA_IP_v6)
			total_rules++;
	}

	IPA_BE_DBG("v6 base UL group build: client %d total_rules %d\n",
		client_iface, total_rules);

	if (total_rules == 0) {
		/* Nothing to install: mark notif_src_client invalid so later
		 * aggregation/teardown skip this group's unset pipe info
		 * instead of treating client 0 as real.
		 */
		g->notif_src_client = IPA_CLIENT_MAX;
		return 0;
	}

	flt_index = kzalloc(sizeof(*flt_index), GFP_KERNEL);
	if (!flt_index)
		return -ENOMEM;

	if (total_rules > ARRAY_SIZE(flt_index->rule_id)) {
		IPA_BE_ERR("v6 base total_rules %d exceeds rule_id[] cap (client=%d)\n",
			total_rules, client_iface);
		kfree(flt_index);
		return -E2BIG;
	}

	if (rx_prop->num_rx_props > 0) {
		installed_hdls = kzalloc((rx_prop->num_rx_props / 2) * total_rules *
					 sizeof(uint32_t), GFP_KERNEL);
		if (!installed_hdls) {
			kfree(flt_index);
			return -ENOMEM;
		}
	}

	/* This group is build-once, so ordering doesn't need to survive a
	 * later incremental add -- just assign a stable one here for
	 * robustness against ext_prop not already being rule_id-sorted.
	 * Purely transient: freed at the end rather than kept on g.
	 */
	ipa_be_compute_rule_priorities(ext_prop, IPA_IP_v6, &local_prio_list);

	for (j = 0; j < rx_prop->num_rx_props / 2; j++) {
		idx = j * 2 + 1;
		if (rx_prop->rx[idx].ip != IPA_IP_v6)
			continue;

		pFilteringTable = (struct ipa_ioc_add_flt_rule_v2 *)kzalloc(sizeof(struct ipa_ioc_add_flt_rule_v2), GFP_KERNEL);
		if (pFilteringTable == NULL) {
			retval = -ENOMEM;
			goto fail;
		}
		pFilteringTable->rules = (uintptr_t)kzalloc(total_rules * sizeof(struct ipa_flt_rule_add_v2), GFP_KERNEL);
		if (!pFilteringTable->rules) {
			kfree(pFilteringTable);
			pFilteringTable = NULL;
			retval = -ENOMEM;
			goto fail;
		}

		memset(flt_index, 0, sizeof(struct ipa_fltr_installed_notif_req_msg_v01));
		flt_index->source_pipe_index = ipa_get_ep_mapping(rx_prop->rx[idx].src_pipe);
		flt_index->install_status = IPA_QMI_RESULT_SUCCESS_V01;
		flt_index->rule_id_valid = 1;
		flt_index->rule_id_len = total_rules;
		flt_index->embedded_pipe_index_valid = 1;
		flt_index->embedded_pipe_index = ipa_get_ep_mapping(IPA_CLIENT_APPS_LAN_WAN_PROD);
		flt_index->retain_header_valid = 1;
		flt_index->retain_header = 0;

		pFilteringTable->commit = 1;
		pFilteringTable->ep = rx_prop->rx[idx].src_pipe;
		pFilteringTable->global = false;
		pFilteringTable->ip = IPA_IP_v6;
		pFilteringTable->num_rules = total_rules;
		pFilteringTable->flt_rule_size = sizeof(struct ipa_flt_rule_add_v2);

		for (cnt = i = 0; cnt < ext_prop->num_ext_props && i < total_rules; cnt++) {
			if (ext_prop->ext[cnt].ip != IPA_IP_v6)
				continue;

			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add_v2));
			flt_rule_entry.at_rear = 0;
			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.retain_hdr = 0;
			flt_rule_entry.rule.to_uc = 0;
			flt_rule_entry.rule.eq_attrib_type = 1;
			flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;
			action_cache = flt_rule_entry.rule.action;
			flt_rule_entry.rule.set_metadata = false;
			flt_rule_entry.flt_rule_category = IPA_FLT_RULE_CAT_UPLINK;
			flt_rule_entry.rule_sub_category = ipa_be_lookup_local_sub_cat(
				&local_prio_list, ext_prop->ext[cnt].rule_id);

			index = flt_rule_count_v6[rx_prop->rx[idx].src_pipe];

			memcpy(&flt_rule_entry.rule.eq_attrib,
				&ext_prop->ext[cnt].eq_attrib,
				sizeof(ext_prop->ext[cnt].eq_attrib));

			if (ext_prop->ext[cnt].action == IPA_PASS_TO_EXCEPTION) {
				flt_rule_entry.rule.action = ext_prop->ext[cnt].action;
				flt_rule_entry.rule.rt_tbl_idx = 0;
			} else {
				flt_rule_entry.rule.action = action_cache;
				flt_rule_entry.rule.rt_tbl_idx = ext_prop->ext[cnt].rt_tbl_idx;
			}

			flt_rule_entry.rule.hashable = ext_prop->ext[cnt].is_rule_hashable;
			flt_rule_entry.rule.rule_id = ext_prop->ext[cnt].rule_id;

			if (flt_rule_entry.rule.action != IPA_PASS_TO_EXCEPTION)
				flt_rule_entry.rule.ttl_update = 1;

			flt_rule_entry.rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
			memcpy(&(((struct ipa_flt_rule_add_v2 *)(uintptr_t)pFilteringTable->rules)[i]), &flt_rule_entry, sizeof(flt_rule_entry));

			IPA_BE_DBG("Modem UL filtering rule %d has index %d installed at %d\n", cnt, index, i);
			flt_index->rule_id[i] = ext_prop->ext[cnt].rule_id;
			index++;
			i++;
		}

		retval = ipa_ul_group_set_rule_ids(g, flt_index->rule_id,
			flt_index->rule_id_len,
			(uint32_t)flt_index->source_pipe_index,
			(uint32_t)flt_index->embedded_pipe_index,
			rx_prop->rx[idx].src_pipe);
		if (retval)
			goto fail;

		/* Notify before install -- see the v4 helper for rationale. */
		ipa_ul_send_client_notify(client_iface, IPA_IP_v6, mux_id);

		if (ipa3_add_flt_rule_usr_v2((struct ipa_ioc_add_flt_rule_v2 *)pFilteringTable, true)) {
			IPA_BE_ERR("ipa3_add_flt_rule_usr_v2 failed\n");
			retval = -EFAULT;
			goto fail;
		}

		for (i = 0; i < pFilteringTable->num_rules; i++) {
			struct ipa_flt_rule_add_v2 *installed_rule =
				&(((struct ipa_flt_rule_add_v2 *)(uintptr_t)pFilteringTable->rules)[i]);

			if (!installed_rule->flt_rule_hdl)
				continue;

			flt_entry.flt_hdl = installed_rule->flt_rule_hdl;
			flt_entry.cat = IPA_FLT_RULE_CAT_UPLINK;
			flt_entry.ip_type = IPA_IP_v6;
			if (installed_hdls)
				installed_hdls[installed_count++] = flt_entry.flt_hdl;
			ipa3_add_filter_rules_entry(client_iface, flt_entry);
		}
		flt_rule_count_v6[rx_prop->rx[idx].src_pipe]++;

		kfree((void *)(uintptr_t)pFilteringTable->rules);
		kfree(pFilteringTable);
		pFilteringTable = NULL;
	}

	g->flt_hdls = installed_hdls;
	g->num_hdls = installed_count;
	ipa_ul_free_prio_list(&local_prio_list);
	kfree(flt_index);
	return 0;

fail:
	ipa_ul_free_prio_list(&local_prio_list);
	if (installed_count > 0 && installed_hdls) {
		struct ipa_ioc_del_flt_rule *dp;
		size_t sz = sizeof(*dp) + installed_count * sizeof(struct ipa_flt_rule_del);

		dp = kzalloc(sz, GFP_KERNEL);
		if (dp) {
			int k;

			dp->commit = 1;
			dp->num_hdls = installed_count;
			dp->ip = IPA_IP_v6;
			for (k = 0; k < installed_count; k++) {
				dp->hdl[k].status = -1;
				dp->hdl[k].hdl = installed_hdls[k];
			}
			ipa3_del_flt_rule(dp);
			kfree(dp);
		}
	}
	kfree(installed_hdls);
	kfree(flt_index);
	if (pFilteringTable) {
		if (pFilteringTable->rules)
			kfree((void *)(uintptr_t)pFilteringTable->rules);
		kfree(pFilteringTable);
	}
	return retval;
}

/*
 * Builds and installs the v6 XLAT companion rows: shared/global per
 * client_iface, not VLAN-keyed -- unlike the v4 XLAT row, this duplicate
 * matches by EtherType, not VLAN ID, so any VLAN's traffic matches the
 * same shared rows. One IPA_PASS_TO_ROUTING, non-hashable, CAT_XLAT
 * duplicate row per non-exception v6 ext_prop entry; the base
 * IPA_PASS_TO_SRC_NAT row for that same entry is installed independently
 * by the base group, not duplicated here.
 */
static int ipa_be_v6_build_install_xlat_companion_rows(struct ipa_ul_group *g,
	struct ipa_ioc_query_intf_rx_props *rx_prop,
	struct ipa_ioc_query_intf_ext_props *ext_prop,
	int client_iface, uint8_t mux_id)
{
	struct ipa_ioc_add_flt_rule_v2 *pFilteringTable = NULL;
	struct ipa_flt_rule_add_v2 flt_rule_entry;
	struct ipa_fltr_installed_notif_req_msg_v01 *flt_index = NULL;
	struct ipa3_flt_entry flt_entry = {0};
	int total_rules = 0;
	int idx = 0, j, cnt, i = 0;
	int retval = 0;
	uint32_t *installed_hdls = NULL;
	int installed_count = 0;
	bool needs_legacy_ethertype_meq;

	for (cnt = 0; cnt < ext_prop->num_ext_props; cnt++) {
		if (ext_prop->ext[cnt].ip == IPA_IP_v6 &&
		    ext_prop->ext[cnt].action != IPA_PASS_TO_EXCEPTION)
			total_rules++;
	}

	IPA_BE_DBG("v6 XLAT companion group build: client %d total_rules %d\n",
		client_iface, total_rules);

	if (total_rules == 0) {
		/* Nothing to install: mark notif_src_client invalid so later
		 * aggregation/teardown skip this group's unset pipe info
		 * instead of treating client 0 as real.
		 */
		g->notif_src_client = IPA_CLIENT_MAX;
		return 0;
	}

	flt_index = kzalloc(sizeof(*flt_index), GFP_KERNEL);
	if (!flt_index)
		return -ENOMEM;

	if (total_rules > ARRAY_SIZE(flt_index->rule_id)) {
		IPA_BE_ERR("v6 XLAT companion total_rules %d exceeds rule_id[] cap (client=%d)\n",
			total_rules, client_iface);
		kfree(flt_index);
		return -E2BIG;
	}

	if (rx_prop->num_rx_props > 0) {
		installed_hdls = kzalloc((rx_prop->num_rx_props / 2) * total_rules *
					 sizeof(uint32_t), GFP_KERNEL);
		if (!installed_hdls) {
			kfree(flt_index);
			return -ENOMEM;
		}
	}

	for (j = 0; j < rx_prop->num_rx_props / 2; j++) {
		idx = j * 2 + 1;
		if (rx_prop->rx[idx].ip != IPA_IP_v6)
			continue;

		pFilteringTable = (struct ipa_ioc_add_flt_rule_v2 *)kzalloc(sizeof(struct ipa_ioc_add_flt_rule_v2), GFP_KERNEL);
		if (pFilteringTable == NULL) {
			retval = -ENOMEM;
			goto fail;
		}
		pFilteringTable->rules = (uintptr_t)kzalloc(total_rules * sizeof(struct ipa_flt_rule_add_v2), GFP_KERNEL);
		if (!pFilteringTable->rules) {
			kfree(pFilteringTable);
			pFilteringTable = NULL;
			retval = -ENOMEM;
			goto fail;
		}

		memset(flt_index, 0, sizeof(struct ipa_fltr_installed_notif_req_msg_v01));
		flt_index->source_pipe_index = ipa_get_ep_mapping(rx_prop->rx[idx].src_pipe);
		flt_index->install_status = IPA_QMI_RESULT_SUCCESS_V01;
		flt_index->rule_id_valid = 1;
		flt_index->rule_id_len = total_rules;
		flt_index->embedded_pipe_index_valid = 1;
		flt_index->embedded_pipe_index = ipa_get_ep_mapping(IPA_CLIENT_APPS_LAN_WAN_PROD);
		flt_index->retain_header_valid = 1;
		flt_index->retain_header = 0;

		pFilteringTable->commit = 1;
		pFilteringTable->ep = rx_prop->rx[idx].src_pipe;
		pFilteringTable->global = false;
		pFilteringTable->ip = IPA_IP_v6;
		pFilteringTable->num_rules = total_rules;
		pFilteringTable->flt_rule_size = sizeof(struct ipa_flt_rule_add_v2);

		/* On v7.0+ ETH/NON_DMA mode the uC rewrites the packet type; ethertype meq is not needed. */
		needs_legacy_ethertype_meq = ipa3_ctx->ipa_hw_type < IPA_HW_v7_0 ||
			ipa3_get_ep_traffic_mode(pFilteringTable->ep) == IPA_BASIC;

		for (cnt = i = 0; cnt < ext_prop->num_ext_props && i < total_rules; cnt++) {
			if (ext_prop->ext[cnt].ip != IPA_IP_v6)
				continue;
			if (ext_prop->ext[cnt].action == IPA_PASS_TO_EXCEPTION)
				continue;

			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add_v2));
			flt_rule_entry.at_rear = 1;
			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.retain_hdr = 0;
			flt_rule_entry.rule.to_uc = 0;
			flt_rule_entry.rule.eq_attrib_type = 1;
			flt_rule_entry.rule.set_metadata = false;

			memcpy(&flt_rule_entry.rule.eq_attrib,
				&ext_prop->ext[cnt].eq_attrib,
				sizeof(ext_prop->ext[cnt].eq_attrib));

			flt_rule_entry.rule.hashable = false;
			flt_rule_entry.rule.rule_id = ext_prop->ext[cnt].rule_id;
			flt_rule_entry.rule.ttl_update = 1;
			flt_rule_entry.rule.rt_tbl_idx = ext_prop->ext[cnt].rt_tbl_idx;
			flt_rule_entry.rule.rule_type = IPA_FLT_RULE_TYPE_IP;
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
			flt_rule_entry.flt_rule_category = IPA_FLT_RULE_CAT_XLAT;

			if (needs_legacy_ethertype_meq) {
				int meq32_n = flt_rule_entry.rule.eq_attrib.num_offset_meq_32;
				uint8_t ethertype_offset = 0;
				bool add_ethertype_meq = true;

				if (meq32_n + 1 > IPA_IPFLTR_NUM_MEQ_32_EQNS) {
					IPA_BE_ERR("Can't add another meq_32 equation to this rule");
					add_ethertype_meq = false;
				} else {
					switch (rx_prop->rx[idx].hdr_l2_type) {
					case IPA_HDR_L2_NONE:
						ethertype_offset = NON_IHL_EQ_OFFSET_FROM_L3(IPA_ETHERTYPE_OFFSET_IP);
						break;
					case IPA_HDR_L2_ETHERNET_II:
					case IPA_HDR_L2_ETHERNET_II_AST:
						ethertype_offset = NON_IHL_EQ_OFFSET_FROM_L2(IPA_ETHERTYPE_OFFSET_ETH);
						break;
					case IPA_HDR_L2_802_1Q:
					case IPA_HDR_L2_802_1Q_AST:
						ethertype_offset = NON_IHL_EQ_OFFSET_FROM_L2(IPA_ETHERTYPE_OFFSET_VLAN);
						break;
					case IPA_HDR_L2_802_3:
						add_ethertype_meq = false;
						break;
					default:
						break;
					}
				}

				if (add_ethertype_meq) {
					WARN_ON(ethertype_offset == 0);

					flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].offset = ethertype_offset;
					flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].mask = IPA_ETHERTYPE_MASK;
					flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].value = ETH_P_IP;
					IPA_BE_DBG("XLAT EtherType match: hdr_l2_type=%d offset=%d value=0x%x\n",
						rx_prop->rx[idx].hdr_l2_type,
						flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].offset,
						flt_rule_entry.rule.eq_attrib.offset_meq_32[meq32_n].value);

					if (meq32_n == 0)
						flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1 << 5);
					else
						flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1 << 6);

					flt_rule_entry.rule.eq_attrib.num_offset_meq_32++;
				} else {
					IPA_BE_DBG("XLAT EtherType match skipped: hdr_l2_type=%d\n",
						rx_prop->rx[idx].hdr_l2_type);
				}
			}

			memcpy(&(((struct ipa_flt_rule_add_v2 *)(uintptr_t)pFilteringTable->rules)[i]), &flt_rule_entry, sizeof(flt_rule_entry));

			IPA_BE_DBG("Xlat companion rule %d meq32_n %d\n", cnt, flt_rule_entry.rule.eq_attrib.num_offset_meq_32);
			flt_index->rule_id[i] = ext_prop->ext[cnt].rule_id;
			i++;
		}

		retval = ipa_ul_group_set_rule_ids(g, flt_index->rule_id,
			flt_index->rule_id_len,
			(uint32_t)flt_index->source_pipe_index,
			(uint32_t)flt_index->embedded_pipe_index,
			rx_prop->rx[idx].src_pipe);
		if (retval)
			goto fail;

		/* Notify before install -- see the v4 helper for rationale. */
		ipa_ul_send_client_notify(client_iface, IPA_IP_v6, mux_id);

		if (ipa3_add_flt_rule_usr_v2((struct ipa_ioc_add_flt_rule_v2 *)pFilteringTable, true)) {
			IPA_BE_ERR("ipa3_add_flt_rule_usr_v2 failed\n");
			retval = -EFAULT;
			goto fail;
		}

		for (i = 0; i < pFilteringTable->num_rules; i++) {
			struct ipa_flt_rule_add_v2 *installed_rule =
				&(((struct ipa_flt_rule_add_v2 *)(uintptr_t)pFilteringTable->rules)[i]);

			if (!installed_rule->flt_rule_hdl)
				continue;

			flt_entry.flt_hdl = installed_rule->flt_rule_hdl;
			flt_entry.cat = IPA_FLT_RULE_CAT_XLAT;
			flt_entry.ip_type = IPA_IP_v6;
			if (installed_hdls)
				installed_hdls[installed_count++] = flt_entry.flt_hdl;
			ipa3_add_filter_rules_entry(client_iface, flt_entry);
		}
		flt_rule_count_v6[rx_prop->rx[idx].src_pipe]++;

		kfree((void *)(uintptr_t)pFilteringTable->rules);
		kfree(pFilteringTable);
		pFilteringTable = NULL;
	}

	g->flt_hdls = installed_hdls;
	g->num_hdls = installed_count;
	kfree(flt_index);
	return 0;

fail:
	if (installed_count > 0 && installed_hdls) {
		struct ipa_ioc_del_flt_rule *dp;
		size_t sz = sizeof(*dp) + installed_count * sizeof(struct ipa_flt_rule_del);

		dp = kzalloc(sz, GFP_KERNEL);
		if (dp) {
			int k;

			dp->commit = 1;
			dp->num_hdls = installed_count;
			dp->ip = IPA_IP_v6;
			for (k = 0; k < installed_count; k++) {
				dp->hdl[k].status = -1;
				dp->hdl[k].hdl = installed_hdls[k];
			}
			ipa3_del_flt_rule(dp);
			kfree(dp);
		}
	}
	kfree(installed_hdls);
	kfree(flt_index);
	if (pFilteringTable) {
		if (pFilteringTable->rules)
			kfree((void *)(uintptr_t)pFilteringTable->rules);
		kfree(pFilteringTable);
	}
	return retval;
}

int ipa_be_v6_add_uplink_filter_rule(struct ipa_ipv6_rule_create_msg v6_msg, bool lan2lan, int pdn_iface, int client_iface, bool is_xlat)
{
	struct ipa_ioc_query_intf client_intf, pdn_intf;
	int retval = 0;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	struct ipa_ioc_query_intf_ext_props *ext_prop = NULL;
	uint8_t pdn_mux_id = 0;
	struct ipa_ul_group *base_group = NULL;
	struct ipa_ul_group *xlat_group = NULL;
	bool base_new_group = false, base_new_pdn = false;
	bool xlat_new_group = false, xlat_new_pdn = false;
	int ret;

	IPA_BE_DBG("ECMIPA entry lan2lan %d is_xlat %d\n", lan2lan, is_xlat);

	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(client_iface, &client_intf)) {
		retval = -EINVAL;
		goto end;
	} else {
		IPA_BE_DBG("Interface with index %u exist.\n", client_iface);
	}

	rx_prop = (struct ipa_ioc_query_intf_rx_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_rx_props) +
		client_intf.num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (rx_prop == NULL) {
		IPA_BE_ERR("Unable to allocate rx_prop memory.\n");
		retval = -EINVAL;
		goto end;
	}
	memcpy(rx_prop->name, client_intf.name, sizeof(client_intf.name));
	rx_prop->num_rx_props = client_intf.num_rx_props;
	IPA_BE_DBG("Query rx_prop %d name %s\n", rx_prop->num_rx_props, client_intf.name);
	ipa3_query_intf_rx_props(rx_prop);

	/* ext Props -- queried against this specific pdn_iface, every call. */
	if (!ipa3_query_iface(pdn_iface, &pdn_intf)) {
		retval = -EINVAL;
		goto end;
	} else {
		IPA_BE_DBG("Interface with index %u exist.\n", pdn_iface);
	}
	ext_prop = (struct ipa_ioc_query_intf_ext_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_ext_props) +
		pdn_intf.num_ext_props * sizeof(struct ipa_ioc_ext_intf_prop), GFP_KERNEL);
	if (ext_prop == NULL) {
		IPA_BE_ERR("Unable to allocate ext_prop memory.\n");
		retval = -EINVAL;
		goto end;
	}
	memcpy(ext_prop->name, pdn_intf.name, sizeof(pdn_intf.name));
	ext_prop->num_ext_props = pdn_intf.num_ext_props;
	IPA_BE_DBG("Query ext_prop %d name %s is_xlat %d\n", ext_prop->num_ext_props, ext_prop->name, is_xlat);
	ipa3_query_intf_ext_props(ext_prop);

	pdn_mux_id = ext_prop->num_ext_props > 0 ? ext_prop->ext[0].mux_id : 0;

	/* Base group: PDN-independent, installed once per (client, v6).
	 * Shared by both the direct ECM v6-connect path (is_xlat=false) and
	 * the v4-XLAT-triggered synthetic call (is_xlat=true) -- both
	 * represent genuine need for the base rules, so both contribute to
	 * the same aggregate ref_count.
	 */
	ret = ipa_ul_group_acquire(client_iface, IPA_IP_v6, IPA_FLT_RULE_CAT_UPLINK,
				    false, 0, pdn_iface, pdn_mux_id,
				    &base_group, &base_new_group, &base_new_pdn);
	if (ret) {
		retval = ret;
		goto end;
	}

	if (base_new_group) {
		retval = ipa_be_v6_build_install_base_rows(base_group, rx_prop, ext_prop,
				client_iface, pdn_mux_id);
		if (retval) {
			ipa_ul_group_abort_new(base_group);
			base_group = NULL;
			goto end;
		}
		ipa_ul_group_install_done(base_group);
	} else if (base_new_pdn) {
		IPA_BE_DBG("Base v6 UL group already installed for client %d; new pdn %d (mux %d) joins it\n",
			client_iface, pdn_iface, pdn_mux_id);
	}

	/* XLAT companion group: shared/global per client, EtherType-matched.
	 * This runs once per v4-XLAT-triggering call regardless of VLAN, so
	 * its ref_count equals the total v4-XLAT flow count summed across
	 * every VLAN -- it installs once and tears down only once every
	 * VLAN's v4 XLAT flows are gone.
	 */
	if (is_xlat) {
		ret = ipa_ul_group_acquire(client_iface, IPA_IP_v6, IPA_FLT_RULE_CAT_XLAT,
					    false, 0, pdn_iface, pdn_mux_id,
					    &xlat_group, &xlat_new_group, &xlat_new_pdn);
		if (ret) {
			IPA_BE_ERR("Failed to acquire v6 XLAT companion group client %d: %d\n",
				client_iface, ret);
		} else if (xlat_new_group) {
			int xlat_ret = ipa_be_v6_build_install_xlat_companion_rows(xlat_group,
					rx_prop, ext_prop, client_iface, pdn_mux_id);
			if (xlat_ret) {
				IPA_BE_ERR("Failed to install v6 XLAT companion rows client %d: %d\n",
					client_iface, xlat_ret);
				ipa_ul_group_abort_new(xlat_group);
				xlat_group = NULL;
			} else {
				ipa_ul_group_install_done(xlat_group);
			}
		} else if (xlat_new_pdn) {
			IPA_BE_DBG("v6 XLAT companion group already installed for client %d; new pdn %d joins it\n",
				client_iface, pdn_iface);
		}
	}

	/* Notify-only fallback for a PDN joining an existing group with no
	 * build. OR'd, not &&'d: xlat_new_group isn't reset on an abort, so
	 * && would wrongly suppress the base group's own notify too.
	 */
	if ((!base_new_group && base_new_pdn) || (!xlat_new_group && xlat_new_pdn))
		ipa_ul_send_client_notify(client_iface, IPA_IP_v6, pdn_mux_id);

end:
	if (rx_prop)
		kfree(rx_prop);
	if (ext_prop)
		kfree(ext_prop);

	IPA_BE_DBG("Exit retval %d \n", retval);
	return retval;
}
EXPORT_SYMBOL(ipa_be_v6_add_uplink_filter_rule);

int ipa_be_v6_add_filter_rule(struct ipa_ipv6_rule_create_msg v6_msg, bool lan2lan, int intf_num, mac_addr_t mac, int is_ret)
{
	struct ipa_ioc_add_flt_rule_v2 *pFilteringTable = NULL;
	struct ipa_flt_rule_add_v2 flt_rule_entry;
	struct ipa_ioc_query_intf temp_intf;
	int retval = 0, len = 0;
	struct ipa_ioc_query_intf_rx_props *rx_prop;
	int total_rules = 1;
	int idx = 0;
	int flt_hdl = 0;
	struct ipa_ioc_get_rt_tbl rt_tbl = {0};
	int l2l_ep = -1;
	u32 l2l_dst[4] = {0};

	IPA_BE_DBG("ECMIPA entry lan2lan %d\n", lan2lan);
	ipa_type_check_ipa_mac_addr(mac);

	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(intf_num, &temp_intf)) {
		return -EINVAL;
	} else {
		IPA_BE_DBG("Interface with index %u exists.\n", intf_num);
	}

	rx_prop = (struct ipa_ioc_query_intf_rx_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_rx_props) +
		temp_intf.num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (rx_prop == NULL) {
		IPA_BE_ERR("Unable to allocate rx_prop memory.\n");
		return -EINVAL;
	}
	memcpy(rx_prop->name, temp_intf.name, sizeof(temp_intf.name));
	rx_prop->num_rx_props = temp_intf.num_rx_props;
	IPA_BE_DBG("Query rx_prop %d name %s\n", rx_prop->num_rx_props, temp_intf.name);
	ipa3_query_intf_rx_props(rx_prop);
	if (rx_prop->num_rx_props == 0) {
		IPA_BE_ERR("No rx props for iface index %u\n", intf_num);
		retval = -EINVAL;
		goto end;
	}
	IPA_BE_DBG("Query response rx_prop src %d hdr_l2_type %d\n", rx_prop->rx[idx].src_pipe,
		rx_prop->rx[idx].hdr_l2_type);

	if (lan2lan) {
		enum ipa_hw_type ipa_ver = ipa_get_hw_type();

		pFilteringTable = (struct ipa_ioc_add_flt_rule_v2 *)kzalloc(sizeof(struct ipa_ioc_add_flt_rule_v2), GFP_KERNEL);
		if (!pFilteringTable) {
			IPA_BE_ERR("Failed to allocate ipa_ioc_add_flt_rule_v2 memory...\n");
			retval = -EINVAL;
			goto end;
		}

		pFilteringTable->rules = (uintptr_t)kzalloc(total_rules * sizeof(struct ipa_flt_rule_add_v2), GFP_KERNEL);
		if (!pFilteringTable->rules) {
			kfree(pFilteringTable);
			pFilteringTable = NULL;
			IPA_BE_ERR("Failed to allocate ipa_flt_rule_add_v2 memory...\n");
			retval = -EINVAL;
			goto end;
		}

		IPA_BE_DBG("Install rules on Rx pipe at idx %d src_pipe %d\n", idx, rx_prop->rx[idx].src_pipe);

		/*
		 * lan2lan filters are refcounted globally per (dst host IP,
		 * ip type); the HW rule is keyed on the physical pipe. Resolve
		 * this ingress ep and skip a duplicate install if it already
		 * holds the rule (see v4 twin).
		 */
		{
			l2l_ep = ipa_get_ep_mapping(rx_prop->rx[idx].src_pipe);
			if (l2l_ep < 0) {
				/* No physical pipe to key on; skip install (see v4 twin). */
				IPA_BE_ERR("lan2lan v6: no ep mapping for src_pipe %d, skipping install\n",
					   rx_prop->rx[idx].src_pipe);
				retval = -EINVAL;
				goto end;
			}
			for (int i = 0; i < 4; i++) {
				if (is_ret)
					l2l_dst[i] =
						(uint32_t)ntohl(v6_msg.tuple.return_ip[i]);
				else
					l2l_dst[i] =
						(uint32_t)ntohl(v6_msg.tuple.flow_ip[i]);
			}
			if (!ipa3_lan2lan_flt_need_install(IPA_IP_v6, l2l_dst,
							   l2l_ep)) {
				IPA_BE_DBG("lan2lan v6 filter already present on ep %d, ref bumped\n",
					   l2l_ep);
				retval = 0;
				goto end;
			}
		}

		/* add mac based rule*/
		pFilteringTable->commit = 1;
		pFilteringTable->ep = rx_prop->rx[idx].src_pipe;
		pFilteringTable->ip = IPA_IP_v6;
		pFilteringTable->num_rules = total_rules;
		pFilteringTable->flt_rule_size = sizeof(struct ipa_flt_rule_add_v2);
		/* pFilteringTable->add_after_hdl = 0; //tbd */

		memset(&flt_rule_entry, 0, sizeof(flt_rule_entry));

		flt_rule_entry.rule.retain_hdr = 0;
		flt_rule_entry.rule.to_uc = 0;

		/* Set filter action based on IPA version and EP direction.
		 * V6 CT lookup key is src_ip (canonical/smaller EP). MAC tiebreaker when EPs equal.
		 * SRC_NAT when packet src = canonical src_ip; DST_NAT when packet dst = canonical src_ip.
		 */
		if (ipa_ver >= IPA_HW_v7_0) {
			int ep_cmp = ipa_be_flow_canonical_cmp(
				v6_msg.conn_rule.flow_interface_num,
				v6_msg.conn_rule.return_interface_num,
				v6_msg.conn_rule.flow_mac,
				v6_msg.conn_rule.return_mac);

			if (ep_cmp < 0) {
				retval = -EINVAL;
				goto end;
			}

			if (is_ret)
				flt_rule_entry.rule.action = ep_cmp ?
					IPA_PASS_TO_SRC_NAT : IPA_PASS_TO_DST_NAT;
			else
				flt_rule_entry.rule.action = ep_cmp ?
					IPA_PASS_TO_DST_NAT : IPA_PASS_TO_SRC_NAT;
			IPA_BE_DBG("LAN2LAN v6 (v7.0+): %s action (is_ret=%d)\n",
				   flt_rule_entry.rule.action == IPA_PASS_TO_DST_NAT ?
				   "DST_NAT" : "SRC_NAT", is_ret);
		} else {
			/* IPA < v7.0: Use routing (backward compatible) */
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
			IPA_BE_DBG("LAN2LAN v6 (< v7.0): ROUTING action (legacy)\n");
		}

		flt_rule_entry.rule.eq_attrib_type = 0;

		rt_tbl.ip = IPA_IP_v6;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(rt_tbl.name, V6_LAN_TO_LAN_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#else
		strlcpy(rt_tbl.name, V6_LAN_TO_LAN_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#endif

		IPA_BE_DBG("This flt rule points to rt tbl %s.\n", rt_tbl.name);

		if (ipa3_get_rt_tbl((struct ipa_ioc_get_rt_tbl *)&rt_tbl)) {
			IPA_BE_ERR("ECMIPA failed to get route hdl \n");
			retval = -EFAULT;
			goto end;
		}
		IPA_BE_DBG("Install filter rules with rt_tbl.hdl %d \n", rt_tbl.hdl);

		IPA_BE_DBG("Install rules on Rx pipe at idx %d \n", idx);
		flt_rule_entry.rule.rt_tbl_hdl = rt_tbl.hdl;
		/* Lookup ref is transient; the filter rule takes its own rt_tbl ref on add. */
		ipa_put_rt_tbl(rt_tbl.hdl);

		flt_rule_entry.rule.hashable = true;
		flt_rule_entry.rule.rule_id = LAN2LAN_RULE_ID; /* LAN2LAN_RULE_ID */
		flt_rule_entry.flt_rule_category = IPA_FLT_RULE_CAT_LAN2LAN;

		memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[idx].attrib, sizeof(flt_rule_entry.rule.attrib));

		if (v6_msg.return_vlan_filter_rule.egress_vlan_tag) {
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_VLAN_ID;
			flt_rule_entry.rule.attrib.vlan_id = v6_msg.return_vlan_filter_rule.egress_vlan_tag;
		}

		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		for (int i = 0; i < 4; i++) {
			if (is_ret)
				flt_rule_entry.rule.attrib.u.v6.dst_addr[i] =
					(uint32_t)ntohl(v6_msg.tuple.return_ip[i]);
			else
				flt_rule_entry.rule.attrib.u.v6.dst_addr[i] =
					(uint32_t)ntohl(v6_msg.tuple.flow_ip[i]);

			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[i] = 0xFFFFFFFF;
		}

		flt_rule_entry.rule.rule_type = IPA_FLT_RULE_TYPE_MAX;
		memcpy(&(((struct ipa_flt_rule_add_v2 *)(uintptr_t)pFilteringTable->rules)[0]), &flt_rule_entry, sizeof(flt_rule_entry));

		if (ipa3_add_flt_rule_usr_v2((struct ipa_ioc_add_flt_rule_v2 *)pFilteringTable,
			true)) {
			IPA_BE_ERR("ipa3_add_flt_rule_usr_v2 failed\n");
			retval = -EFAULT;
			goto end;
		} else {
			flt_hdl = (((struct ipa_flt_rule_add_v2 *)(uintptr_t)pFilteringTable->rules)[0]).flt_rule_hdl;
			IPA_BE_DBG("Lan2Lan %d filter rule hdl: %d\n", lan2lan, flt_hdl);

			/* Register the new HW rule in the global (dst, ip_type)
			 * presence set and bump the global refcount (see v4
			 * twin). */
			ipa3_lan2lan_flt_add(IPA_IP_v6, l2l_dst, l2l_ep, flt_hdl);
		}
	} else {
		IPA_BE_DBG("Lan2wan case \n");
	}


end:
	if (pFilteringTable) {
		if (pFilteringTable->rules)
			kfree((void *)(uintptr_t)pFilteringTable->rules);
		kfree(pFilteringTable);
	}
	if (rx_prop)
		kfree(rx_prop);

	IPA_BE_DBG("Exit retval %d \n", retval);
	return retval;
}

EXPORT_SYMBOL(ipa_be_v6_add_filter_rule);

int ipa_be_v4_delete_filter_rule(struct ipa_ipv4_rule_destroy_msg v4_msg, int intf_idx, mac_addr_t mac, bool lan2lan)
{
	u8 flt_param_buf[sizeof(struct ipa_ioc_del_flt_rule) + sizeof(struct ipa_flt_rule_del)]
		__aligned(__alignof__(struct ipa_ioc_del_flt_rule));
	struct ipa_ioc_del_flt_rule *flt_param = (struct ipa_ioc_del_flt_rule *)flt_param_buf;
	int retval = 0;
	int num_rules = 1;
	int flt_hdl = -1;
	u32 dst;
	int hdls[IPA_MAX_NUM_PIPES];
	int num_hdls = 0;
	int i;

	IPA_BE_DBG("Entry ipa_be_v4_delete_filter_rule for mac %pM \n", mac);

	if (lan2lan) {
		int ref;

		/* Key by dst host IP + ip type, matching the install path.
		 * The RET filter (on the flow ingress ep) was installed with
		 * dst=return_ip and mac=return_mac; the FLOW filter (on the
		 * return ingress ep) with dst=flow_ip and mac=flow_mac. Pick
		 * the dst IP that pairs with @mac. */
		if (!memcmp(mac, v4_msg.conn_rule.return_mac, IPA_MAC_ADDR_SIZE))
			dst = (uint32_t)ntohl(v4_msg.tuple.return_ip);
		else
			dst = (uint32_t)ntohl(v4_msg.tuple.flow_ip);

		/* Drop this connection's global reference. The HW rule stays
		 * on every ep while other connections to the same dst host
		 * remain; only when the last reference is released are all of
		 * those per-ep rules torn down. */
		ref = ipa3_lan2lan_flt_deref(IPA_IP_v4, &dst, hdls,
					     IPA_MAX_NUM_PIPES, &num_hdls);
		if (ref < 0) {
			IPA_BE_ERR("No matching lan2lan filter exists for dst\n");
			retval = -1;
			goto fail;
		}
		if (ref > 0) {
			IPA_BE_DBG("lan2lan v4 filter still shared, global_ref now %d\n", ref);
			goto fail;
		}

		/* Last reference released: delete the HW rule on every ep it
		 * was installed on. */
		for (i = 0; i < num_hdls && i < IPA_MAX_NUM_PIPES; i++) {
			flt_hdl = hdls[i];

			memset(flt_param_buf, 0, sizeof(flt_param_buf));
			flt_param->commit = 1;
			flt_param->num_hdls = num_rules;
			flt_param->ip = IPA_IP_v4;

			flt_param->hdl[0].status = -1;
			flt_param->hdl[0].hdl = flt_hdl;
			IPA_BE_DBG("Deleting filter hdl:(0x%x) with ip type: %d\n", flt_hdl, IPA_IP_v4);

			if (ipa3_del_flt_rule(flt_param)) {
				IPA_BE_ERR("ipa3_del_flt_rule failed\n");
				retval = -EFAULT;
			} else {
				if (flt_param->hdl[0].status != 0) {
					IPA_BE_ERR("Filter rule hdl 0x%x deletion failed with error:%d\n",
					flt_param->hdl[0].hdl, flt_param->hdl[0].status);
					retval = -1;
					continue;
				}
				IPA_BE_DBG("Filter rule deleted successfully.. lan2lan: %d hdl: %d\n", lan2lan, flt_hdl);
			}
		}
	}

fail:
	return retval;
}
EXPORT_SYMBOL(ipa_be_v4_delete_filter_rule);

int ipa_be_v6_delete_filter_rule(struct ipa_ipv6_rule_destroy_msg v6_msg, int intf_idx, mac_addr_t mac, bool lan2lan)
{
	u8 flt_param_buf[sizeof(struct ipa_ioc_del_flt_rule) + sizeof(struct ipa_flt_rule_del)]
		__aligned(__alignof__(struct ipa_ioc_del_flt_rule));
	struct ipa_ioc_del_flt_rule *flt_param = (struct ipa_ioc_del_flt_rule *)flt_param_buf;
	int retval = 0;
	int num_rules = 1;
	int flt_hdl = -1;
	u32 dst[4] = {0};
	int hdls[IPA_MAX_NUM_PIPES];
	int num_hdls = 0;
	int i;

	IPA_BE_DBG("Entry ipa_be_v6_delete_filter_rule for mac %pM \n", mac);

	if (lan2lan) {
		int ref;

		/* Key by dst host IP + ip type, matching the install path.
		 * Pick the dst IP that pairs with @mac (see v4 twin). */
		for (i = 0; i < 4; i++) {
			if (!memcmp(mac, v6_msg.conn_rule.return_mac, IPA_MAC_ADDR_SIZE))
				dst[i] = (uint32_t)ntohl(v6_msg.tuple.return_ip[i]);
			else
				dst[i] = (uint32_t)ntohl(v6_msg.tuple.flow_ip[i]);
		}

		/* Drop this connection's global reference; tear down the HW
		 * rule on every ep only when the last reference is released
		 * (see v4 twin). */
		ref = ipa3_lan2lan_flt_deref(IPA_IP_v6, dst, hdls,
					     IPA_MAX_NUM_PIPES, &num_hdls);
		if (ref < 0) {
			IPA_BE_ERR("No matching lan2lan filter exists for dst\n");
			retval = -1;
			goto fail;
		}
		if (ref > 0) {
			IPA_BE_DBG("lan2lan v6 filter still shared, global_ref now %d\n", ref);
			goto fail;
		}

		/* Last reference released: delete the HW rule on every ep it
		 * was installed on. */
		for (i = 0; i < num_hdls && i < IPA_MAX_NUM_PIPES; i++) {
			flt_hdl = hdls[i];

			memset(flt_param_buf, 0, sizeof(flt_param_buf));
			flt_param->commit = 1;
			flt_param->num_hdls = num_rules;
			flt_param->ip = IPA_IP_v6;

			flt_param->hdl[0].status = -1;
			flt_param->hdl[0].hdl = flt_hdl;
			IPA_BE_DBG("Deleting filter hdl:(0x%x) with ip type: %d\n", flt_hdl, IPA_IP_v6);

			if (ipa3_del_flt_rule(flt_param)) {
				IPA_BE_ERR("ipa3_del_flt_rule failed\n");
				retval = -EFAULT;
			} else {
				if (flt_param->hdl[0].status != 0) {
					IPA_BE_ERR("Filter rule hdl 0x%x deletion failed with error:%d\n",
					flt_param->hdl[0].hdl, flt_param->hdl[0].status);
					retval = -1;
					continue;
				}
				IPA_BE_DBG("Filter rule deleted successfully.. lan2lan: %d hdl: %d\n", lan2lan, flt_hdl);
			}
		}
	}

fail:
	return retval;
}
EXPORT_SYMBOL(ipa_be_v6_delete_filter_rule);

int ipa_be_v4_delete_uplink_filter_rule(struct ipa_ipv4_rule_destroy_msg v4_msg, int pdn_iface, int client_iface)
{
	int retval = 0;
	bool is_xlat = false;
	bool has_vlan = false;
	uint16_t vlan_id = 0;
	uint16_t vlan_key;
	struct ipa_ul_group *empty_group = NULL;
	bool pdn_removed;
	uint8_t removed_mux_id;
	uint32_t removed_src_pipe, removed_ebd_pipe;
	int ret;

	IPA_BE_DBG("Entry ipa_be_v4_delete_filter_rule for pdn_iface %d client_iface %d \n", pdn_iface, client_iface);

	/* Check if this was XLAT traffic - same detection logic as in add function */
	if (((ntohl(v4_msg.tuple.flow_ip) & 0xFFFFFF00U) == 0xC0000000U) ||
	    ((ntohl(v4_msg.tuple.return_ip) & 0xFFFFFF00U) == 0xC0000000U) ||
	    ((ntohl(v4_msg.conn_rule.flow_ip_xlate) & 0xFFFFFF00U) == 0xC0000000U) ||
	    ((ntohl(v4_msg.conn_rule.return_ip_xlate) & 0xFFFFFF00U) == 0xC0000000U)) {
		is_xlat = true;
		IPA_BE_DBG("XLAT traffic detected during deletion: flow %pI4n return %pI4n\n",
			   &v4_msg.tuple.flow_ip, &v4_msg.tuple.return_ip);
	}

	/* Same VLAN-key derivation as the add path, so the release targets
	 * the same XLAT group this flow's add call acquired.
	 */
	if (v4_msg.vlan_primary_rule.egress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED) {
		vlan_id = v4_msg.vlan_primary_rule.egress_vlan_tag & 0x0FFF;
		has_vlan = true;
	} else if (v4_msg.vlan_primary_rule.ingress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED) {
		vlan_id = v4_msg.vlan_primary_rule.ingress_vlan_tag & 0x0FFF;
		has_vlan = true;
	}
	vlan_key = has_vlan ? vlan_id : IPA_UL_VLAN_NONE;

	/* Deletion is flow-driven: the base group is torn down the moment
	 * its aggregate ref_count hits zero, rather than waiting for a
	 * separate PDN-level flush.
	 */
	ret = ipa_ul_group_release(client_iface, IPA_IP_v4, IPA_FLT_RULE_CAT_UPLINK,
				    false, 0, pdn_iface, &empty_group,
				    &pdn_removed, &removed_mux_id, &removed_src_pipe, &removed_ebd_pipe);
	if (ret < 0)
		retval = ret;
	if (empty_group) {
		ipa_ul_group_teardown(empty_group);
		empty_group = NULL;
	}
	/* Removal notify owed only once membership drops to zero across
	 * EVERY v4 group of this client, not just this one.
	 */
	if (pdn_removed && !ipa_ul_client_pdn_lookup(client_iface, IPA_IP_v4, pdn_iface,
						      NULL, NULL, NULL))
		ipa_ul_send_pdn_removed_notify(removed_src_pipe, removed_ebd_pipe,
			removed_mux_id, pdn_iface, client_iface);

	if (is_xlat) {
		ret = ipa_ul_group_release(client_iface, IPA_IP_v4, IPA_FLT_RULE_CAT_UPLINK,
					    true, vlan_key, pdn_iface, &empty_group,
					    &pdn_removed, &removed_mux_id, &removed_src_pipe, &removed_ebd_pipe);
		if (ret < 0 && retval == 0)
			retval = ret;
		if (empty_group) {
			ipa_ul_group_teardown(empty_group);
			empty_group = NULL;
		}
		if (pdn_removed && !ipa_ul_client_pdn_lookup(client_iface, IPA_IP_v4, pdn_iface,
							      NULL, NULL, NULL))
			ipa_ul_send_pdn_removed_notify(removed_src_pipe, removed_ebd_pipe,
				removed_mux_id, pdn_iface, client_iface);

		/* XLAT add bumps the v6 companion every time; drop it every time. */
		{
			struct ipa_ipv6_rule_destroy_msg v6_msg_xlat;
			int v6_ret;

			memset(&v6_msg_xlat, 0, sizeof(v6_msg_xlat));

			IPA_BE_DBG("Decrementing v6 uplink companion for XLAT traffic\n");
			v6_ret = ipa_be_v6_delete_uplink_filter_rule(v6_msg_xlat, pdn_iface, client_iface, true);
			/* -ENOENT/-EINVAL: companion gone or already at 0 (duplicate/retried delete); ignore. */
			if (v6_ret && v6_ret != -ENOENT && v6_ret != -EINVAL) {
				IPA_BE_ERR("v6 companion dec failed: %d\n", v6_ret);
				if (retval == 0)
					retval = v6_ret;
			}
		}
	}

	/* Atomic: checks emptiness under the same lock pair it writes under,
	 * so no separate pre-check is needed (or safe, re: TOCTOU).
	 */
	ipa_be_global_qmap_id_reset_if_empty();

	return retval;
}
EXPORT_SYMBOL(ipa_be_v4_delete_uplink_filter_rule);

int ipa_be_v6_delete_uplink_filter_rule(struct ipa_ipv6_rule_destroy_msg v6_msg, int pdn_iface, int client_iface, bool is_xlat)
{
	struct ipa_ul_group *empty_group = NULL;
	bool pdn_removed;
	uint8_t removed_mux_id;
	uint32_t removed_src_pipe, removed_ebd_pipe;
	int ret;

	IPA_BE_DBG("Entry ipa_be_v6_delete_uplink_filter_rule for pdn_iface %d client_iface %d is_xlat %d\n",
		pdn_iface, client_iface, is_xlat);

	/* Base group: released by both the direct ECM v6-disconnect path
	 * (is_xlat=false) and the v4-XLAT-triggered companion call
	 * (is_xlat=true), mirroring the add side's shared ref_count.
	 */
	ret = ipa_ul_group_release(client_iface, IPA_IP_v6, IPA_FLT_RULE_CAT_UPLINK,
				    false, 0, pdn_iface, &empty_group,
				    &pdn_removed, &removed_mux_id, &removed_src_pipe, &removed_ebd_pipe);
	if (empty_group) {
		ipa_ul_group_teardown(empty_group);
		empty_group = NULL;
	}
	/* Removal notify owed only once membership drops to zero across
	 * EVERY v6 group of this client, not just this one.
	 */
	if (pdn_removed && !ipa_ul_client_pdn_lookup(client_iface, IPA_IP_v6, pdn_iface,
						      NULL, NULL, NULL))
		ipa_ul_send_pdn_removed_notify(removed_src_pipe, removed_ebd_pipe,
			removed_mux_id, pdn_iface, client_iface);

	/* XLAT companion group: only ever acquired by the is_xlat path, so
	 * only release it here when set -- otherwise a plain ECM v6
	 * disconnect would spuriously release a membership it never took.
	 */
	if (is_xlat) {
		int xlat_ret = ipa_ul_group_release(client_iface, IPA_IP_v6, IPA_FLT_RULE_CAT_XLAT,
						     false, 0, pdn_iface, &empty_group,
						     &pdn_removed, &removed_mux_id,
						     &removed_src_pipe, &removed_ebd_pipe);
		if (empty_group) {
			ipa_ul_group_teardown(empty_group);
			empty_group = NULL;
		}
		if (pdn_removed && !ipa_ul_client_pdn_lookup(client_iface, IPA_IP_v6, pdn_iface,
							      NULL, NULL, NULL))
			ipa_ul_send_pdn_removed_notify(removed_src_pipe, removed_ebd_pipe,
				removed_mux_id, pdn_iface, client_iface);
		if (ret == 0)
			ret = xlat_ret;
	}

	/* Atomic, same as the v4 call site -- no separate pre-check needed. */
	ipa_be_global_qmap_id_reset_if_empty();

	return ret < 0 ? ret : 0;
}
EXPORT_SYMBOL(ipa_be_v6_delete_uplink_filter_rule);

/*
 * Safety-net PDN-level sweep, kept for ipa_be_nat_mgmt.c's NAT/IPv6CT
 * triggers (which have no per-flow delete call to piggyback on). Normal
 * teardown is flow-driven (see ipa_be_v{4,6}_delete_uplink_filter_rule()
 * above). This force- (idle_only=false) or best-effort- (idle_only=true)
 * drops pdn_iface's membership from every group of the given ip_type on
 * any client_iface; tearing down a group's HW rows is a side effect of
 * that membership removal reaching an empty aggregate.
 */
static void __ipa_be_flush_uplink_rules_for_pdn(int pdn_iface,
		enum ipa_ip_type ip_type, bool idle_only)
{
	IPA_BE_DBG("flush Q6 UL notify for pdn_iface=%d ip_type=%d idle_only=%d\n",
		pdn_iface, ip_type, idle_only);

	while (true) {
		struct ipa_ul_group *g, *empty_g = NULL;
		struct ipa_ul_pdn_ref *p;
		bool found = false;
		uint8_t removed_mux_id = 0;
		uint32_t notif_src_pipe = 0, notif_ebd_pipe = 0;
		int group_client_iface = 0;

		spin_lock_bh(&ipa_ul_groups_lock);
		list_for_each_entry(g, &ipa_ul_groups_list, node) {
			if (g->ip_type != ip_type)
				continue;
			/* Skip a group mid-build: its notif_src_pipe/
			 * notif_ebd_pipe are written unsynchronized until
			 * ipa_ul_group_install_done() clears installing under
			 * this same lock.
			 */
			if (g->installing)
				continue;
			p = ipa_find_ul_pdn_ref(g, pdn_iface);
			if (!p)
				continue;
			if (idle_only && atomic_read(&p->ref_count) > 0)
				continue;
			found = true;
			group_client_iface = g->client_iface;
			removed_mux_id = p->mux_id;
			notif_src_pipe = g->notif_src_pipe;
			notif_ebd_pipe = g->notif_ebd_pipe;
			atomic_sub(atomic_read(&p->ref_count), &g->ref_count);
			list_del(&p->node);
			kfree(p);
			if (atomic_read(&g->ref_count) <= 0 && !g->installing) {
				list_del(&g->node);
				empty_g = g;
			}
			break;
		}
		spin_unlock_bh(&ipa_ul_groups_lock);

		if (!found)
			break;
		/* Removal notify owed only once membership drops to zero
		 * across EVERY group of this client+ip_type, checked fresh
		 * after dropping the lock -- this sweep processes one group
		 * at a time, so only the group that truly empties it sees
		 * this come back false.
		 */
		if (!ipa_ul_client_pdn_lookup(group_client_iface, ip_type, pdn_iface,
					       NULL, NULL, NULL))
			ipa_ul_send_pdn_removed_notify(notif_src_pipe, notif_ebd_pipe,
				removed_mux_id, pdn_iface, group_client_iface);
		if (empty_g)
			ipa_ul_group_teardown(empty_g);
	}

	/* Reset global mux_id when no uplink group remains so the next
	 * bring-up latches a fresh value.
	 */
	ipa_be_global_qmap_id_reset_if_empty();
}

void ipa_be_flush_uplink_rules_for_pdn(int pdn_iface, enum ipa_ip_type ip_type)
{
	__ipa_be_flush_uplink_rules_for_pdn(pdn_iface, ip_type, false);
}
EXPORT_SYMBOL(ipa_be_flush_uplink_rules_for_pdn);

/* Flush only pdn_refs with no live refs; safe against a concurrent add. */
void ipa_be_flush_idle_uplink_rules_for_pdn(int pdn_iface, enum ipa_ip_type ip_type)
{
	__ipa_be_flush_uplink_rules_for_pdn(pdn_iface, ip_type, true);
}
EXPORT_SYMBOL(ipa_be_flush_idle_uplink_rules_for_pdn);

static bool AddWanDLFilteringRule(
	struct ipa_ioc_add_flt_rule const *rule_table_v4,
	struct ipa_ioc_add_flt_rule const *rule_table_v6,
	uint8_t *mux_id_v4,
	uint8_t *mux_id_v6,
	enum flt_rule_category *category_v4,
	enum flt_rule_category *category_v6,
	enum rule_sub_category *sub_category_v4,
	enum rule_sub_category *sub_category_v6,
	bool is_sw_routing)
{
	int ret = 0, cnt, num_rules = 0, pos;
	int num_v4_rules = rule_table_v4 ? rule_table_v4->num_rules : 0;
	int num_v6_rules = rule_table_v6 ? rule_table_v6->num_rules : 0;
	struct ipa_be_qmi_rule_entry *sorted_rules = NULL;
	struct ipa_install_fltr_rule_req_ex_msg_v01 *qmi_rule_ex_msg;

	qmi_rule_ex_msg = kzalloc(sizeof(*qmi_rule_ex_msg), GFP_KERNEL);
	if (!qmi_rule_ex_msg) {
		IPA_BE_ERR("Failed to allocate memory for qmi_rule_ex_msg\n");
		return false;
	}

	if (is_sw_routing || READ_ONCE(ipa3_ctx->ipa_sw)) {
		struct ipa_ioc_get_rt_tbl_indx rt_idx;
		struct ipa_rule_attrib meta_attrib;
		struct ipa_ipfltri_rule_eq eq_attrib;
		uint8_t sw_mux_id = ipa_be_global_qmap_id;
		int n = 0;

		if (sw_mux_id == IPA_BE_GLOBAL_QMAP_UNSET) {
			IPA_BE_ERR("SW routing: no WAN PDN active (mux_id unset), skip Q6 catch-all\n");
			kfree(qmi_rule_ex_msg);
			return false;
		}

		memset(&rt_idx, 0, sizeof(rt_idx));
		strscpy(rt_idx.name, WAN_DL_ROUTE_TABLE_NAME,
			IPA_RESOURCE_NAME_MAX);
		rt_idx.ip = IPA_IP_v4;
		if (!ipa3_query_rt_index(&rt_idx)) {
			memset(&meta_attrib, 0, sizeof(meta_attrib));
			meta_attrib.attrib_mask = IPA_FLT_META_DATA;
			meta_attrib.meta_data =
				(u32)sw_mux_id << WWAN_METADATA_SHFT;
			meta_attrib.meta_data_mask = WWAN_METADATA_MASK;
			memset(&eq_attrib, 0, sizeof(eq_attrib));
			if (ipahal_flt_generate_equation(IPA_IP_v4,
					&meta_attrib, &eq_attrib)) {
				IPA_BE_ERR("SW routing: v4 eq gen failed\n");
			} else {
				qmi_rule_ex_msg->filter_spec_ex_list[n].ip_type =
					QMI_IPA_IP_TYPE_V4_V01;
				qmi_rule_ex_msg->filter_spec_ex_list[n]
					.filter_action =
					QMI_IPA_FILTER_ACTION_ROUTING_V01;
				qmi_rule_ex_msg->filter_spec_ex_list[n]
					.is_routing_table_index_valid = 1;
				qmi_rule_ex_msg->filter_spec_ex_list[n]
					.route_table_index = rt_idx.idx;
				qmi_rule_ex_msg->filter_spec_ex_list[n]
					.is_mux_id_valid = 1;
				qmi_rule_ex_msg->filter_spec_ex_list[n]
					.mux_id = sw_mux_id;
				qmi_rule_ex_msg->filter_spec_ex_list[n]
					.is_rule_hashable = 1;
				memcpy(&qmi_rule_ex_msg->filter_spec_ex_list[n]
					.filter_rule, &eq_attrib,
					sizeof(struct ipa_filter_rule_type_v01));
				n++;
			}
		}

		memset(&rt_idx, 0, sizeof(rt_idx));
		strscpy(rt_idx.name, WAN_DL_ROUTE_TABLE_NAME,
			IPA_RESOURCE_NAME_MAX);
		rt_idx.ip = IPA_IP_v6;
		if (!ipa3_query_rt_index(&rt_idx)) {
			memset(&meta_attrib, 0, sizeof(meta_attrib));
			meta_attrib.attrib_mask = IPA_FLT_META_DATA;
			meta_attrib.meta_data =
				(u32)sw_mux_id << WWAN_METADATA_SHFT;
			meta_attrib.meta_data_mask = WWAN_METADATA_MASK;
			memset(&eq_attrib, 0, sizeof(eq_attrib));
			if (ipahal_flt_generate_equation(IPA_IP_v6,
					&meta_attrib, &eq_attrib)) {
				IPA_BE_ERR("SW routing: v6 eq gen failed\n");
			} else {
				qmi_rule_ex_msg->filter_spec_ex_list[n].ip_type =
					QMI_IPA_IP_TYPE_V6_V01;
				qmi_rule_ex_msg->filter_spec_ex_list[n]
					.filter_action =
					QMI_IPA_FILTER_ACTION_ROUTING_V01;
				qmi_rule_ex_msg->filter_spec_ex_list[n]
					.is_routing_table_index_valid = 1;
				qmi_rule_ex_msg->filter_spec_ex_list[n]
					.route_table_index = rt_idx.idx;
				qmi_rule_ex_msg->filter_spec_ex_list[n]
					.is_mux_id_valid = 1;
				qmi_rule_ex_msg->filter_spec_ex_list[n]
					.mux_id = sw_mux_id;
				qmi_rule_ex_msg->filter_spec_ex_list[n]
					.is_rule_hashable = 1;
				memcpy(&qmi_rule_ex_msg->filter_spec_ex_list[n]
					.filter_rule, &eq_attrib,
					sizeof(struct ipa_filter_rule_type_v01));
				n++;
			}
		}

		if (n > 0) {
			qmi_rule_ex_msg->filter_spec_ex_list_valid = true;
			qmi_rule_ex_msg->filter_spec_ex_list_len = n;
			qmi_rule_ex_msg->source_pipe_index_valid = 0;
			IPA_BE_DBG("SW routing: sending %d catch-all rule(s) to Q6 (suppressing %d per-flow)\n",
				   n, num_v4_rules + num_v6_rules);
			ret = ipa3_qmi_filter_request_ex_send(qmi_rule_ex_msg);
			if (ret) {
				IPA_BE_ERR("SW routing: Q6 catch-all send failed: %d\n",
					   ret);
				kfree(qmi_rule_ex_msg);
				return false;
			}
			kfree(qmi_rule_ex_msg);
			return true;
		}
		/* RT table not found or eq gen failed — fall through to
		 * per-flow rule installation so WAN-up is not hard-blocked
		 * by a temporarily-unavailable routing table. qmi_rule_ex_msg
		 * is reused by the per-flow section below; do not free it here.
		 */
		IPA_BE_ERR("SW routing: no catch-all rules generated (WAN RT table not found or eq gen failed) — falling back to per-flow rules\n");
	}

	num_rules = num_v4_rules + num_v6_rules;

	if (rule_table_v4) {
		IPA_BE_DBG("Get %d WAN DL IPv4 filtering rules.\n",
			   rule_table_v4->num_rules);
	}
	if (rule_table_v6) {
		IPA_BE_DBG("Get %d WAN DL IPv6 filtering rules.\n",
			   rule_table_v6->num_rules);
	}

	if (num_rules > QMI_IPA_MAX_FILTERS_EX_V01) {
		IPA_BE_ERR("The number of filtering rules exceed limit.\n");
		kfree(qmi_rule_ex_msg);
		return false;
	}

	if (num_rules > 0) {
		sorted_rules = kcalloc(num_rules, sizeof(*sorted_rules),
				       GFP_KERNEL);
		if (!sorted_rules) {
			IPA_BE_ERR("Failed to allocate memory for sorted_rules\n");
			kfree(qmi_rule_ex_msg);
			return false;
		}
		qmi_rule_ex_msg->filter_spec_ex_list_valid = true;
	} else {
		qmi_rule_ex_msg->filter_spec_ex_list_valid = false;
	}

	qmi_rule_ex_msg->filter_spec_ex_list_len = num_rules;
	qmi_rule_ex_msg->source_pipe_index_valid = 0;

	IPA_BE_DBG("Get %d WAN DL filtering rules in total.\n", num_rules);

	pos = 0;
	for (cnt = 0; cnt < num_v4_rules; cnt++, pos++) {
		sorted_rules[pos].rule = &rule_table_v4->rules[cnt];
		sorted_rules[pos].mux_id_valid = !!mux_id_v4;
		sorted_rules[pos].mux_id = mux_id_v4 ? mux_id_v4[cnt] : 0;
		sorted_rules[pos].ip_type = IPA_IP_v4;
		sorted_rules[pos].category = category_v4[cnt];
		sorted_rules[pos].sub_category = sub_category_v4[cnt];
		sorted_rules[pos].original_index = cnt;
	}

	if (num_v4_rules > 1)
		sort(sorted_rules, num_v4_rules, sizeof(*sorted_rules),
		     ipa_be_qmi_rule_cmp, ipa_be_qmi_rule_swap);

	for (cnt = 0; cnt < num_v4_rules; cnt++) {
		int out_idx = cnt;
		int in_idx = num_v4_rules - 1 - cnt;

		qmi_rule_ex_msg->filter_spec_ex_list[out_idx].ip_type =
			QMI_IPA_IP_TYPE_V4_V01;
		qmi_rule_ex_msg->filter_spec_ex_list[out_idx].filter_action =
			GetQmiFilterAction(sorted_rules[in_idx].rule->rule.action);
		qmi_rule_ex_msg->filter_spec_ex_list[out_idx]
			.is_routing_table_index_valid = 1;
		qmi_rule_ex_msg->filter_spec_ex_list[out_idx]
			.route_table_index =
			sorted_rules[in_idx].rule->rule.rt_tbl_idx;
		if (sorted_rules[in_idx].mux_id_valid) {
			qmi_rule_ex_msg->filter_spec_ex_list[out_idx]
				.is_mux_id_valid = 1;
			qmi_rule_ex_msg->filter_spec_ex_list[out_idx].mux_id =
				sorted_rules[in_idx].mux_id;
		}
		qmi_rule_ex_msg->filter_spec_ex_list[out_idx].rule_id =
			sorted_rules[in_idx].rule->rule.rule_id;
		qmi_rule_ex_msg->filter_spec_ex_list[out_idx].is_rule_hashable =
			sorted_rules[in_idx].rule->rule.hashable;
		memcpy(&qmi_rule_ex_msg->filter_spec_ex_list[out_idx].filter_rule,
		       &sorted_rules[in_idx].rule->rule.eq_attrib,
		       sizeof(struct ipa_filter_rule_type_v01));
	}

	pos = 0;
	for (cnt = 0; cnt < num_v6_rules; cnt++, pos++) {
		sorted_rules[pos].rule = &rule_table_v6->rules[cnt];
		sorted_rules[pos].mux_id_valid = !!mux_id_v6;
		sorted_rules[pos].mux_id = mux_id_v6 ? mux_id_v6[cnt] : 0;
		sorted_rules[pos].ip_type = IPA_IP_v6;
		sorted_rules[pos].category = category_v6[cnt];
		sorted_rules[pos].sub_category = sub_category_v6[cnt];
		sorted_rules[pos].original_index = cnt;
	}

	if (num_v6_rules > 1)
		sort(sorted_rules, num_v6_rules, sizeof(*sorted_rules),
		     ipa_be_qmi_rule_cmp, ipa_be_qmi_rule_swap);

	for (cnt = 0; cnt < num_v6_rules; cnt++) {
		int out_idx = num_v4_rules + cnt;
		int in_idx = num_v6_rules - 1 - cnt;

		qmi_rule_ex_msg->filter_spec_ex_list[out_idx].ip_type =
			QMI_IPA_IP_TYPE_V6_V01;
		qmi_rule_ex_msg->filter_spec_ex_list[out_idx].filter_action =
			GetQmiFilterAction(sorted_rules[in_idx].rule->rule.action);
		qmi_rule_ex_msg->filter_spec_ex_list[out_idx]
			.is_routing_table_index_valid = 1;
		qmi_rule_ex_msg->filter_spec_ex_list[out_idx]
			.route_table_index =
			sorted_rules[in_idx].rule->rule.rt_tbl_idx;
		if (sorted_rules[in_idx].mux_id_valid) {
			qmi_rule_ex_msg->filter_spec_ex_list[out_idx]
				.is_mux_id_valid = 1;
			qmi_rule_ex_msg->filter_spec_ex_list[out_idx].mux_id =
				sorted_rules[in_idx].mux_id;
		}
		qmi_rule_ex_msg->filter_spec_ex_list[out_idx].rule_id =
			sorted_rules[in_idx].rule->rule.rule_id;
		qmi_rule_ex_msg->filter_spec_ex_list[out_idx].is_rule_hashable =
			sorted_rules[in_idx].rule->rule.hashable;
		memcpy(&qmi_rule_ex_msg->filter_spec_ex_list[out_idx].filter_rule,
		       &sorted_rules[in_idx].rule->rule.eq_attrib,
		       sizeof(struct ipa_filter_rule_type_v01));
	}

	IPA_BE_DBG("Calling the IOCTL to add %d num rules\n", num_rules);
	ret = ipa3_qmi_filter_request_ex_send(qmi_rule_ex_msg);
	if (ret) {
		IPA_BE_ERR("Failed adding Filtering rule %p with ret %d\n",
			   qmi_rule_ex_msg, ret);
		kfree(sorted_rules);
		kfree(qmi_rule_ex_msg);
		return false;
	}
	IPA_BE_DBG("Success adding %d num rules with ret %d\n", num_rules, ret);

	kfree(sorted_rules);
	kfree(qmi_rule_ex_msg);
	return true;
}

int add_dft_filtering_rule(int pdn_iface, enum ipa_ip_type iptype)
{
	struct ipa_ioc_add_flt_rule *pFilteringTable = NULL;
	struct ipa_ioc_query_intf temp_intf;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	struct ipa_ioc_get_rt_tbl rt_tbl = {0};
	struct ipa_ioc_get_rt_tbl_indx rt_tbl_idx = {0};
	int retval = 0;
	int len, i, num_rules;
	struct ipa_pdn_filter_rules *pdn_entry;
	struct ipa_pdn_filter_rules *prealloc;
	struct ipa_flt_rule_add flt_rule_entry;
	int m_ipv4_default_filterting_rules_count = 0;
	int m_ipv6_default_filterting_rules_count = 0;
	bool pdn_entry_newly_created = false;
	bool took_set_ref = false;
	spinlock_t *fam_lock;
	int set_refs = 0;
	int pdn_refs = 0;

	IPA_BE_DBG("Add dft flt rule for iface %d, ip type %d\n",
		pdn_iface, iptype);

	/*
	 * ipa_be_dft_set_installed[] and ipa_be_dft_set_ref_count[] are sized
	 * IPA_IP_MAX, but enum ipa_ip_type continues past it (IPA_IP_v4_VLAN,
	 * IPA_IP_v6_VLAN). Every in-tree caller passes a literal v4 or v6, but
	 * this is an exported symbol -- range check before indexing.
	 */
	if (!VALID_IPA_IP_TYPE(iptype)) {
		IPA_BE_ERR("Invalid IP type %d\n", iptype);
		return -EINVAL;
	}

	/*
	 * Validate the iface before anything is allocated. The shared-set path
	 * below returns early, so a check placed after it would only ever run for
	 * the one PDN that builds the set -- every later PDN would install a
	 * tracking entry for an iface that may not exist and take a reference on
	 * the set that nothing releases.
	 */
	if (!ipa3_query_iface(pdn_iface, &temp_intf)) {
		IPA_BE_ERR("iface %d doesn't exist\n", pdn_iface);
		return -EINVAL;
	}

	fam_lock = (iptype == IPA_IP_v4) ? &pdn_flt_rule_v4_lock :
					   &pdn_flt_rule_v6_lock;

	prealloc = kzalloc(sizeof(struct ipa_pdn_filter_rules), GFP_KERNEL);
	if (!prealloc) {
		IPA_BE_ERR("Failed to allocate memory for PDN filter entry\n");
		return -ENOMEM;
	}

	/* Check if default filtering rule is already installed for this PDN interface */
	spin_lock_bh(&ipa_pdn_filter_lock);
	pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
	if (pdn_entry) {
		if (pdn_entry->dft_rule_installed) {
			atomic_inc(&pdn_entry->ref_count);
			pdn_refs = atomic_read(&pdn_entry->ref_count);
			spin_unlock_bh(&ipa_pdn_filter_lock);
			kfree(prealloc);
			IPA_BE_DBG("Default filtering rule for pdn %d, IP type %d already installed. ref_count: %d\n",
				pdn_iface, iptype, pdn_refs);
			return 0; /* Rules already exist, just increment ref count and exit */
		}
	} else {
		ipa_create_pdn_filter_entry(prealloc, pdn_iface, iptype);
		pdn_entry = prealloc;
		prealloc = NULL;
		pdn_entry_newly_created = true;
	}

	/*
	 * A PDN of this family has already built the default set. Take a
	 * reference on it and return: none of the default equations carry an
	 * IPA_FLT_META_DATA term, so this PDN's copy would be byte-identical
	 * apart from a mux ID the modem does not match on. Nothing below is
	 * worth querying for a PDN that only takes a reference.
	 *
	 * The reference and this PDN's flag are set in one step: split them and a
	 * delete landing between bails before its atomic_dec, pinning the set.
	 */
	spin_lock_bh(fam_lock);
	lockdep_assert_held(&ipa_pdn_filter_lock);
	if (ipa_be_dft_set_installed[iptype]) {
		set_refs = ++ipa_be_dft_set_ref_count[iptype];
		pdn_entry->dft_rule_installed = true;
		atomic_inc(&pdn_entry->ref_count);
		took_set_ref = true;
	}
	spin_unlock_bh(fam_lock);
	spin_unlock_bh(&ipa_pdn_filter_lock);

	kfree(prealloc);

	if (took_set_ref) {
		IPA_BE_DBG("pdn %d shares the installed IP type %d default set. set ref_count: %d\n",
			pdn_iface, iptype, set_refs);
		return 0;
	}

	rx_prop = kzalloc(sizeof(*rx_prop) + temp_intf.num_rx_props *
		sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (!rx_prop) {
		IPA_BE_ERR("Failed to allocate rx_prop\n");
		retval = -ENOMEM;
		goto fail_pdn;
	}
	memcpy(rx_prop->name, temp_intf.name, sizeof(rx_prop->name));
	rx_prop->num_rx_props = temp_intf.num_rx_props;

	if (ipa3_query_intf_rx_props(rx_prop)) {
		IPA_BE_ERR("Failed to query rx_prop for iface %s\n",
			rx_prop->name);
		retval = -EIO;
		goto fail_rx;
	}

	if (rx_prop->num_rx_props == 0) {
		IPA_BE_ERR("No rx props for iface %s\n", rx_prop->name);
		retval = 0; /* Not an error, just nothing to do */
		goto fail_rx;
	}

	/*
	 * No ext-prop query here: the shared default set stamps mux_id = 0, so
	 * the PDN's mux ID -- the only thing this function used ext props for --
	 * is no longer needed.
	 */
	rt_tbl.ip = iptype;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(rt_tbl.name, WAN_DL_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#else
	strlcpy(rt_tbl.name, WAN_DL_ROUTE_TABLE_NAME, sizeof(rt_tbl.name));
#endif

	if (ipa3_get_rt_tbl(&rt_tbl)) {
		IPA_BE_ERR("Failed to get WAN DL routing table\n");
		retval = -EIO;
		goto fail_rx;
	}
	IPA_BE_DBG("rt_tbl hdl %d\n", rt_tbl.hdl);
	/* hdl only used for this log; not embedded in any rule below. */
	ipa_put_rt_tbl(rt_tbl.hdl);

	rt_tbl_idx.ip = iptype;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(rt_tbl_idx.name, WAN_DL_ROUTE_TABLE_NAME, sizeof(rt_tbl_idx.name));
#else
	strlcpy(rt_tbl_idx.name, WAN_DL_ROUTE_TABLE_NAME, sizeof(rt_tbl_idx.name));
#endif

	rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX - 1] = '\0';
	if (ipa3_query_rt_index(&rt_tbl_idx)) {
		IPA_BE_ERR("Failed to get DL routing table index\n");
		retval = -EIO;
		goto fail_rx;
	}
	IPA_BE_DBG("rt_tbl_idx %d\n", rt_tbl_idx.idx);

	if (iptype == IPA_IP_v4)
		num_rules = 3; /* Frag, Multicast, Broadcast */
	else
		num_rules = 5; /* Frag, TCP SYN, Multicast, Link-Local, Reserved */

	len = sizeof(*pFilteringTable) +
		num_rules * sizeof(struct ipa_flt_rule_add);
	pFilteringTable = kzalloc(len, GFP_KERNEL);
	if (!pFilteringTable) {
		IPA_BE_ERR("Failed to allocate filtering table\n");
		retval = -ENOMEM;
		goto fail_rx;
	}

	pFilteringTable->commit = 1;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = num_rules;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;

	if (iptype == IPA_IP_v4) {
		/* IPv4 Default Filtering Rules */

		/* Rule 1: Fragment Filtering Rule */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule_entry.rule.hashable = false; /* Fragment rules are not hashable */
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

		/* Set fragment attributes */
		flt_rule_entry.rule.attrib.attrib_mask = IPA_FLT_FRAGMENT;

		/* Convert to network order */
		retval = change_to_network_order_kernel(IPA_IP_v4, &flt_rule_entry.rule.attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to convert fragment rule to network order\n");
			goto fail_table;
		}

		/* Generate filter equation */
		retval = ipahal_flt_generate_equation(iptype, &flt_rule_entry.rule.attrib, &flt_rule_entry.rule.eq_attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to generate fragment filter equation\n");
			goto fail_table;
		}

		memcpy(&(pFilteringTable->rules[m_ipv4_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv4_default_filterting_rules_count++;

		/* Rule 2: Multicast Filtering Rule (224.0.0.0/4) */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule_entry.rule.hashable = true;
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

		/* Set multicast address attributes */
		flt_rule_entry.rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0xF0000000;
		flt_rule_entry.rule.attrib.u.v4.dst_addr = 0xE0000000;

		/* Convert to network order - for IPv4, this does nothing but we call it for consistency */
		retval = change_to_network_order_kernel(IPA_IP_v4, &flt_rule_entry.rule.attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to convert to network order\n");
			goto fail_table;
		}

		/* Generate filter equation */
		retval = ipahal_flt_generate_equation(iptype, &flt_rule_entry.rule.attrib, &flt_rule_entry.rule.eq_attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to generate multicast filter equation\n");
			goto fail_table;
		}

		memcpy(&(pFilteringTable->rules[m_ipv4_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv4_default_filterting_rules_count++;

		/* Rule 2: Broadcast Filtering Rule (255.255.255.255) */
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
		flt_rule_entry.rule.attrib.u.v4.dst_addr = 0xFFFFFFFF;

		/* Convert to network order */
		retval = change_to_network_order_kernel(IPA_IP_v4, &flt_rule_entry.rule.attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to convert broadcast rule to network order\n");
			goto fail_table;
		}

		/* Generate filter equation */
		retval = ipahal_flt_generate_equation(iptype, &flt_rule_entry.rule.attrib, &flt_rule_entry.rule.eq_attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to generate broadcast filter equation\n");
			goto fail_table;
		}

		memcpy(&(pFilteringTable->rules[m_ipv4_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv4_default_filterting_rules_count++;

		IPA_BE_DBG("Constructed %d default filtering rules for IPv4\n", m_ipv4_default_filterting_rules_count);

	} else if (iptype == IPA_IP_v6) {
		/* IPv6 Default Filtering Rules */

		/* Rule 1: Fragment Filtering Rule */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule_entry.rule.hashable = false; /* Fragment rules are not hashable */
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

		/* Set fragment attributes */
		flt_rule_entry.rule.attrib.attrib_mask = IPA_FLT_FRAGMENT;

		/* Convert to network order */
		retval = change_to_network_order_kernel(IPA_IP_v6, &flt_rule_entry.rule.attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to convert fragment rule to network order\n");
			goto fail_table;
		}

		/* Generate filter equation */
		retval = ipahal_flt_generate_equation(iptype, &flt_rule_entry.rule.attrib, &flt_rule_entry.rule.eq_attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to generate fragment filter equation\n");
			goto fail_table;
		}

		memcpy(&(pFilteringTable->rules[m_ipv6_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv6_default_filterting_rules_count++;


		/* Rule 2: TCP SYN Rule */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule_entry.rule.hashable = false; /* Multicast rules are hashable */
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

		flt_rule_entry.rule.eq_attrib.protocol_eq = 6;
		flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= 0x20<<flt_rule_entry.rule.eq_attrib.num_offset_meq_32;
		flt_rule_entry.rule.eq_attrib.offset_meq_32[flt_rule_entry.rule.eq_attrib.num_offset_meq_32].offset = 6;
		flt_rule_entry.rule.eq_attrib.offset_meq_32[flt_rule_entry.rule.eq_attrib.num_offset_meq_32].mask = 0xFF000000;
		flt_rule_entry.rule.eq_attrib.offset_meq_32[flt_rule_entry.rule.eq_attrib.num_offset_meq_32].value = 6 << 24;
		flt_rule_entry.rule.eq_attrib.num_offset_meq_32 ++;

		flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1<<8);
		flt_rule_entry.rule.eq_attrib.num_ihl_offset_meq_32 = 1;
		flt_rule_entry.rule.eq_attrib.ihl_offset_meq_32[0].offset = 12;

		/* add TCP SYN rule*/
		flt_rule_entry.rule.eq_attrib.ihl_offset_meq_32[0].value = (((uint32_t)1)<<17);
		flt_rule_entry.rule.eq_attrib.ihl_offset_meq_32[0].mask = (((uint32_t)1)<<17);

		memcpy(&(pFilteringTable->rules[m_ipv6_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv6_default_filterting_rules_count++;

		/* Rule 3: Multicast Filtering Rule (ff00::/8) */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule_entry.rule.hashable = true; /* Multicast rules are hashable */
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

		flt_rule_entry.rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0xFF000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = IPA_BE_MULTICAST_IPV6_PREFIX;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0x00000000;

		/* Convert to network order */
		retval = change_to_network_order_kernel(IPA_IP_v6, &flt_rule_entry.rule.attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to convert multicast rule to network order\n");
			goto fail_table;
		}

		/* Generate filter equation */
		retval = ipahal_flt_generate_equation(iptype, &flt_rule_entry.rule.attrib, &flt_rule_entry.rule.eq_attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to generate multicast filter equation\n");
			goto fail_table;
		}

		memcpy(&(pFilteringTable->rules[m_ipv6_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv6_default_filterting_rules_count++;

		/* Rule 4: Link-Local Unicast Filtering Rule (fe80::/10) */
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = IPA_BE_LINK_LOCAL_IPV6_PREFIX_MASK;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = IPA_BE_LINK_LOCAL_IPV6_PREFIX;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0x00000000;

		/* Convert to network order */
		retval = change_to_network_order_kernel(IPA_IP_v6, &flt_rule_entry.rule.attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to convert link-local rule to network order\n");
			goto fail_table;
		}

		/* Generate filter equation */
		retval = ipahal_flt_generate_equation(iptype, &flt_rule_entry.rule.attrib, &flt_rule_entry.rule.eq_attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to generate link-local filter equation\n");
			goto fail_table;
		}

		memcpy(&(pFilteringTable->rules[m_ipv6_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv6_default_filterting_rules_count++;

		/* Rule 5: Reserved by IETF Filtering Rule (fec0::/10) */
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = IPA_BE_LINK_LOCAL_IPV6_PREFIX_MASK;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = IPA_BE_RESERVED_IPV6_PREFIX;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0x00000000;

		/* Convert to network order */
		retval = change_to_network_order_kernel(IPA_IP_v6, &flt_rule_entry.rule.attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to convert reserved rule to network order\n");
			goto fail_table;
		}

		/* Generate filter equation */
		retval = ipahal_flt_generate_equation(iptype, &flt_rule_entry.rule.attrib, &flt_rule_entry.rule.eq_attrib);
		if (retval != 0) {
			IPA_BE_ERR("Failed to generate reserved filter equation\n");
			goto fail_table;
		}

		memcpy(&(pFilteringTable->rules[m_ipv6_default_filterting_rules_count]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		m_ipv6_default_filterting_rules_count++;

		IPA_BE_DBG("Constructed %d default filtering rules for IPv6\n", m_ipv6_default_filterting_rules_count);
	} else {
		IPA_BE_ERR("Invalid IP type %d\n", iptype);
		retval = -EINVAL;
		goto fail_table;
	}

	/* Add all rules to global linked lists using dynamic counts */
	if (iptype == IPA_IP_v4) {
		struct flt_rule **prealloc_v4 = NULL;
		int alloc_count = 0;
		bool shared_existing = false;

		if (m_ipv4_default_filterting_rules_count > 0) {
			prealloc_v4 = kzalloc(
				m_ipv4_default_filterting_rules_count * sizeof(*prealloc_v4),
				GFP_KERNEL);
			if (!prealloc_v4) {
				IPA_BE_ERR("Failed to allocate prealloc array for IPv4 rules\n");
				retval = -ENOMEM;
				goto fail_table;
			}
			for (i = 0; i < m_ipv4_default_filterting_rules_count; i++) {
				prealloc_v4[i] = kzalloc(sizeof(struct flt_rule), GFP_KERNEL);
				if (!prealloc_v4[i]) {
					IPA_BE_ERR("Failed to allocate memory for IPv4 rule\n");
					while (alloc_count > 0)
						kfree(prealloc_v4[--alloc_count]);
					kfree(prealloc_v4);
					retval = -ENOMEM;
					goto fail_table;
				}
				alloc_count++;
			}
		}
		/*
		 * Re-look-up: the build above ran unlocked. If the entry is gone,
		 * bail rather than referencing a set nothing will release.
		 */
		spin_lock_bh(&ipa_pdn_filter_lock);
		pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
		if (!pdn_entry) {
			spin_unlock_bh(&ipa_pdn_filter_lock);
			IPA_BE_ERR("PDN entry for pdn %d, IP type %d vanished building dft set\n",
				pdn_iface, iptype);
			for (i = 0; i < m_ipv4_default_filterting_rules_count; i++)
				kfree(prealloc_v4[i]);
			kfree(prealloc_v4);
			retval = -ENOENT;
			goto fail_table;
		}
		spin_lock_bh(&pdn_flt_rule_v4_lock);
		lockdep_assert_held(&ipa_pdn_filter_lock);
		if (ipa_be_dft_set_installed[IPA_IP_v4]) {
			/*
			 * Another PDN of this family got there first while we were
			 * building. Take a reference on its set and drop ours --
			 * appending both would leave two copies behind one refcount.
			 */
			set_refs = ++ipa_be_dft_set_ref_count[IPA_IP_v4];
			shared_existing = true;
		} else {
			for (i = 0; i < m_ipv4_default_filterting_rules_count; i++) {
				struct flt_rule *new_rule = prealloc_v4[i];

				memcpy(&new_rule->flt_rule, &pFilteringTable->rules[i], sizeof(struct ipa_flt_rule_add));
				/* default rules are not metadata dependant - mux_id is not relevant */
				new_rule->mux_id = 0;
				new_rule->rule_type = FLT_RULE_TYPE_DEFAULT;
				new_rule->category = IPA_FLT_RULE_CAT_DEFAULT;
				/*
				 * The set is shared by every PDN of this family, so it
				 * is not owned by the one that happened to build it.
				 */
				new_rule->pdn_iface = IPA_BE_DFT_SET_PDN_IFACE;
				list_add_tail(&new_rule->node, &Pdn_flt_rule_v4_list);
				wan_rule_count_v4++;
				IPA_BE_DBG("Added default v4 rule %d, rule count now %d\n", i, wan_rule_count_v4);
			}
			ipa_be_dft_set_installed[IPA_IP_v4] = true;
			ipa_be_dft_set_ref_count[IPA_IP_v4] = 1;
		}
		/* Flag and set reference move in one step -- see the file header. */
		pdn_entry->dft_rule_installed = true;
		atomic_inc(&pdn_entry->ref_count);
		pdn_refs = atomic_read(&pdn_entry->ref_count);
		spin_unlock_bh(&pdn_flt_rule_v4_lock);
		spin_unlock_bh(&ipa_pdn_filter_lock);

		if (shared_existing) {
			for (i = 0; i < m_ipv4_default_filterting_rules_count; i++)
				kfree(prealloc_v4[i]);
			IPA_BE_DBG("default v4 set installed concurrently, pdn %d shares it. set ref_count: %d\n",
				pdn_iface, set_refs);
		}
		kfree(prealloc_v4);
	} else {
		struct flt_rule **prealloc_v6 = NULL;
		int alloc_count = 0;
		bool shared_existing = false;

		if (m_ipv6_default_filterting_rules_count > 0) {
			prealloc_v6 = kzalloc(
				m_ipv6_default_filterting_rules_count * sizeof(*prealloc_v6),
				GFP_KERNEL);
			if (!prealloc_v6) {
				IPA_BE_ERR("Failed to allocate prealloc array for IPv6 rules\n");
				retval = -ENOMEM;
				goto fail_table;
			}
			for (i = 0; i < m_ipv6_default_filterting_rules_count; i++) {
				prealloc_v6[i] = kzalloc(sizeof(struct flt_rule), GFP_KERNEL);
				if (!prealloc_v6[i]) {
					IPA_BE_ERR("Failed to allocate memory for IPv6 rule\n");
					while (alloc_count > 0)
						kfree(prealloc_v6[--alloc_count]);
					kfree(prealloc_v6);
					retval = -ENOMEM;
					goto fail_table;
				}
				alloc_count++;
			}
		}
		/* See the matching re-look-up in the IPv4 branch above. */
		spin_lock_bh(&ipa_pdn_filter_lock);
		pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
		if (!pdn_entry) {
			spin_unlock_bh(&ipa_pdn_filter_lock);
			IPA_BE_ERR("PDN entry for pdn %d, IP type %d vanished building dft set\n",
				pdn_iface, iptype);
			for (i = 0; i < m_ipv6_default_filterting_rules_count; i++)
				kfree(prealloc_v6[i]);
			kfree(prealloc_v6);
			retval = -ENOENT;
			goto fail_table;
		}
		spin_lock_bh(&pdn_flt_rule_v6_lock);
		lockdep_assert_held(&ipa_pdn_filter_lock);
		if (ipa_be_dft_set_installed[IPA_IP_v6]) {
			/*
			 * Another PDN of this family got there first while we were
			 * building. Take a reference on its set and drop ours --
			 * appending both would leave two copies behind one refcount.
			 */
			set_refs = ++ipa_be_dft_set_ref_count[IPA_IP_v6];
			shared_existing = true;
		} else {
			for (i = 0; i < m_ipv6_default_filterting_rules_count; i++) {
				struct flt_rule *new_rule = prealloc_v6[i];

				memcpy(&new_rule->flt_rule, &pFilteringTable->rules[i], sizeof(struct ipa_flt_rule_add));
				/* default rules are not metadata dependant - mux_id is not relevant */
				new_rule->mux_id = 0;
				new_rule->rule_type = FLT_RULE_TYPE_DEFAULT;
				new_rule->category = IPA_FLT_RULE_CAT_DEFAULT;
				/*
				 * The set is shared by every PDN of this family, so it
				 * is not owned by the one that happened to build it.
				 */
				new_rule->pdn_iface = IPA_BE_DFT_SET_PDN_IFACE;
				list_add_tail(&new_rule->node, &Pdn_flt_rule_v6_list);
				wan_rule_count_v6++;
				IPA_BE_DBG("Added default v6 rule %d, rule count now %d\n", i, wan_rule_count_v6);
			}
			ipa_be_dft_set_installed[IPA_IP_v6] = true;
			ipa_be_dft_set_ref_count[IPA_IP_v6] = 1;
		}
		/* Flag and set reference move in one step -- see IPv4 branch. */
		pdn_entry->dft_rule_installed = true;
		atomic_inc(&pdn_entry->ref_count);
		pdn_refs = atomic_read(&pdn_entry->ref_count);
		spin_unlock_bh(&pdn_flt_rule_v6_lock);
		spin_unlock_bh(&ipa_pdn_filter_lock);

		if (shared_existing) {
			for (i = 0; i < m_ipv6_default_filterting_rules_count; i++)
				kfree(prealloc_v6[i]);
			IPA_BE_DBG("default v6 set installed concurrently, pdn %d shares it. set ref_count: %d\n",
				pdn_iface, set_refs);
		}
		kfree(prealloc_v6);
	}

	IPA_BE_DBG("Default filtering rule successfully installed for pdn %d, IP type %d. ref_count: %d\n",
		pdn_iface, iptype, pdn_refs);

	kfree(pFilteringTable);
	kfree(rx_prop);
	return 0;

fail_table:
	kfree(pFilteringTable);
fail_rx:
	kfree(rx_prop);
fail_pdn:
	if (pdn_entry_newly_created) {
		spin_lock_bh(&ipa_pdn_filter_lock);
		pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
		/*
		 * Drop the entry this call created only if nothing ended up installed
		 * on it. Testing retval instead would keep the entry on the "nothing
		 * to do" paths, which return 0 by design.
		 */
		if (pdn_entry && !pdn_entry->dft_rule_installed &&
			!pdn_entry->icmp_rule_installed &&
			!pdn_entry->catchup_rule_installed &&
			!pdn_entry->eth_bh_catchall_installed &&
			atomic_read(&pdn_entry->ref_count) == 0)
			ipa_remove_pdn_filter_entry(pdn_entry);
		spin_unlock_bh(&ipa_pdn_filter_lock);
	}
	return retval;
}


/**
 * add_icmp_alg_rules() - Add ICMP/ICMPv6 ALG filter rules for a PDN interface
 * @pdn_iface: PDN interface number
 * @iptype: IP version (IPA_IP_v4 or IPA_IP_v6)
 *
 * Adds ICMP (IPv4) or ICMPv6 (IPv6) filter rules to the WAN DL filter rule
 * list for the specified PDN interface. These rules direct ICMP/ICMPv6 traffic
 * to the WAN DL routing table (IPA_PASS_TO_ROUTING), ensuring ICMP packets
 * bypass NAT and are handled by the routing path.
 *
 * This function must be called after add_dft_filtering_rule() and before
 * add_catchup_all_filtering_rule_each_pdn() so that ICMP rules are ordered
 * before the catch-all rule in the WAN DL filter table.
 *
 * Reference counting: pdn_entry->ref_count is incremented on every call,
 * regardless of whether the ICMP rule was already installed. If the rule is
 * already installed (icmp_rule_installed == true), only the ref_count is
 * incremented and the function returns immediately without re-installing the
 * hardware rule. This allows multiple connections sharing the same PDN to
 * safely call this function without duplicating rules.
 *
 * Rules are stored as FLT_RULE_TYPE_ICMP and must be cleaned up by calling
 * delete_icmp_alg_rules() for each corresponding add_icmp_alg_rules() call.
 *
 * Return: 0 on success, negative errno on failure
 */
int add_icmp_alg_rules(int pdn_iface, enum ipa_ip_type iptype)
{
	struct ipa_ioc_query_intf temp_intf;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	struct ipa_ioc_query_intf_ext_props *ext_prop = NULL;
	struct ipa_ioc_get_rt_tbl_indx rt_tbl_idx = {0};
	struct ipa_ioc_generate_flt_eq flt_eq;
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa_pdn_filter_rules *pdn_entry;
	struct flt_rule *new_rule;
	spinlock_t *fam_lock;
	struct list_head *fam_list;
	uint32_t *fam_count;
	uint32_t rule_count;
	uint8_t mux_id = 0;
	int retval = 0;
	int rx_idx;
	int pdn_refs;

	IPA_BE_DBG("Add ICMP/ALG flt rule for iface %d, ip type %d\n",
		pdn_iface, iptype);

	/* Range check before the family selection below -- see the matching
	 * check in add_dft_filtering_rule(). enum ipa_ip_type continues past
	 * IPA_IP_MAX, and an out-of-range family would fall through the else
	 * and mutate the v6 list and count. */
	if (!VALID_IPA_IP_TYPE(iptype)) {
		IPA_BE_ERR("Invalid IP type %d\n", iptype);
		return -EINVAL;
	}

	if (iptype == IPA_IP_v4) {
		fam_lock = &pdn_flt_rule_v4_lock;
		fam_list = &Pdn_flt_rule_v4_list;
		fam_count = &wan_rule_count_v4;
	} else {
		fam_lock = &pdn_flt_rule_v6_lock;
		fam_list = &Pdn_flt_rule_v6_list;
		fam_count = &wan_rule_count_v6;
	}

	/*
	 * Check if a PDN entry exists and if the ICMP rule is already
	 * installed. Rules should only be added if a PDN entry has been
	 * created by add_dft_filtering_rule() first.
	 */
	spin_lock_bh(&ipa_pdn_filter_lock);
	pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
	if (!pdn_entry) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_ERR("No PDN entry for iface %d, ip %d. Cannot add ICMP rule.\n",
			   pdn_iface, iptype);
		return -EINVAL; /* Return error if PDN entry not found */
	}

	if (pdn_entry->icmp_rule_installed) {
		atomic_inc(&pdn_entry->ref_count);
		pdn_refs = atomic_read(&pdn_entry->ref_count);
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_DBG("ICMP rule for pdn %d, IP type %d already installed. ref_count: %d\n",
			   pdn_iface, iptype, pdn_refs);
		return 0; /* Rule already exists, just increment ref count and exit */
	}
	spin_unlock_bh(&ipa_pdn_filter_lock);

	if (!ipa3_query_iface(pdn_iface, &temp_intf)) {
		IPA_BE_ERR("iface %d doesn't exist\n", pdn_iface);
		return -EINVAL;
	}

	rx_prop = kzalloc(sizeof(*rx_prop) + temp_intf.num_rx_props *
		sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (!rx_prop) {
		IPA_BE_ERR("Failed to allocate rx_prop\n");
		return -ENOMEM;
	}
	memcpy(rx_prop->name, temp_intf.name, sizeof(rx_prop->name));
	rx_prop->num_rx_props = temp_intf.num_rx_props;

	if (ipa3_query_intf_rx_props(rx_prop)) {
		IPA_BE_ERR("Failed to query rx_prop for iface %s\n",
			rx_prop->name);
		retval = -EIO;
		goto fail_rx;
	}

	if (rx_prop->num_rx_props == 0) {
		IPA_BE_ERR("No rx props for iface %s\n", rx_prop->name);
		retval = 0; /* Not an error, just nothing to do */
		goto fail_rx;
	}

	/* Get ext props for mux_id */
	ext_prop = kzalloc(sizeof(*ext_prop) + temp_intf.num_ext_props *
		sizeof(struct ipa_ioc_ext_intf_prop), GFP_KERNEL);
	if (!ext_prop) {
		IPA_BE_ERR("Unable to allocate ext_prop memory.\n");
		retval = -ENOMEM;
		goto fail_rx;
	}
	memcpy(ext_prop->name, temp_intf.name, sizeof(temp_intf.name));
	ext_prop->num_ext_props = temp_intf.num_ext_props;
	ipa3_query_intf_ext_props(ext_prop);
	if (ext_prop->num_ext_props > 0)
		mux_id = ext_prop->ext[0].mux_id;

	/* Get WAN DL routing table index */
	rt_tbl_idx.ip = iptype;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(rt_tbl_idx.name, WAN_DL_ROUTE_TABLE_NAME, sizeof(rt_tbl_idx.name));
#else
	strlcpy(rt_tbl_idx.name, WAN_DL_ROUTE_TABLE_NAME, sizeof(rt_tbl_idx.name));
#endif
	rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX - 1] = '\0';
	if (ipa3_query_rt_index(&rt_tbl_idx)) {
		IPA_BE_ERR("Failed to get WAN DL routing table index\n");
		retval = -EIO;
		goto fail_ext;
	}
	IPA_BE_DBG("WAN DL routing table %s has index %d\n",
		WAN_DL_ROUTE_TABLE_NAME, rt_tbl_idx.idx);

	/* Build the ICMP/ICMPv6 filter rule */
	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
	flt_rule_entry.at_rear = true;
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;
	flt_rule_entry.rule.retain_hdr = 1;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.eq_attrib_type = 1;
	flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
	flt_rule_entry.rule.hashable = true;
	flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

	if (iptype == IPA_IP_v4) {
		/* Use IPv4 rx prop (even index 0) */
		rx_idx = 0;
		memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[rx_idx].attrib,
			sizeof(flt_rule_entry.rule.attrib));
		/* Multiple PDNs may exist so keep meta-data; add ICMP protocol match */
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_PROTOCOL;
		flt_rule_entry.rule.attrib.u.v4.protocol = (uint8_t)IPPROTO_ICMP;
	} else { /* IPA_IP_v6 */
		/* Use IPv6 rx prop (odd index 1 if available) */
		rx_idx = (rx_prop->num_rx_props > 1) ? 1 : 0;
		memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[rx_idx].attrib,
			sizeof(flt_rule_entry.rule.attrib));
		/* Multiple PDNs may exist so keep meta-data; add ICMPv6 next-header match */
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_NEXT_HDR;
		flt_rule_entry.rule.attrib.u.v6.next_hdr = (uint8_t)IPPROTO_ICMPV6;
	}

	/* Generate filter equation */
	memset(&flt_eq, 0, sizeof(flt_eq));
	memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
	flt_eq.ip = iptype;
	retval = ipahal_flt_generate_equation(iptype, &flt_eq.attrib, &flt_eq.eq_attrib);
	if (retval) {
		IPA_BE_ERR("Failed generating equation for ICMP rule ip type %d with ret %d\n",
			iptype, retval);
		retval = -EIO;
		goto fail_ext;
	}
	memcpy(&flt_rule_entry.rule.eq_attrib, &flt_eq.eq_attrib,
		sizeof(flt_rule_entry.rule.eq_attrib));

	/* Allocate and populate the list node */
	new_rule = kzalloc(sizeof(struct flt_rule), GFP_KERNEL);
	if (!new_rule) {
		IPA_BE_ERR("Failed to allocate memory for ICMP rule\n");
		retval = -ENOMEM;
		goto fail_ext;
	}
	memcpy(&new_rule->flt_rule, &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
	new_rule->mux_id = mux_id;
	new_rule->rule_type = FLT_RULE_TYPE_ICMP;
	new_rule->category = IPA_FLT_RULE_CAT_DOWNLINK;
	new_rule->pdn_iface = pdn_iface;

	/*
	 * Re-look-up: the build above ran unlocked. Append the record and mark the
	 * entry in one step, so a record can never outlive its owner.
	 */
	spin_lock_bh(&ipa_pdn_filter_lock);
	pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
	if (!pdn_entry) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_ERR("PDN entry for pdn %d, IP type %d vanished building ICMP rule\n",
			pdn_iface, iptype);
		kfree(new_rule);
		retval = -ENOENT;
		goto fail_ext;
	}
	spin_lock_bh(fam_lock);
	lockdep_assert_held(&ipa_pdn_filter_lock);
	list_add_tail(&new_rule->node, fam_list);
	(*fam_count)++;
	rule_count = *fam_count;
	pdn_entry->icmp_rule_installed = true;
	atomic_inc(&pdn_entry->ref_count);
	pdn_refs = atomic_read(&pdn_entry->ref_count);
	spin_unlock_bh(fam_lock);
	spin_unlock_bh(&ipa_pdn_filter_lock);

	IPA_BE_DBG("ICMP rule installed for pdn %d, ip %d. rule count %u, ref_count: %d\n",
		pdn_iface, iptype, rule_count, pdn_refs);

fail_ext:
	kfree(ext_prop);
fail_rx:
	kfree(rx_prop);
	return retval;
}

/**
 * delete_icmp_alg_rules() - Delete ICMP/ICMPv6 ALG filter rules for a PDN interface
 * @pdn_iface: PDN interface number
 * @iptype: IP version (IPA_IP_v4 or IPA_IP_v6)
 *
 * Decrements pdn_entry->ref_count and removes the ICMP/ICMPv6 filter rules
 * (FLT_RULE_TYPE_ICMP) from the WAN DL filter rule list for the specified PDN
 * interface only when ref_count reaches zero (i.e., the last connection using
 * this PDN's ICMP rule is being torn down). Also resets the
 * icmp_rule_installed tracking flag when the rule is actually removed.
 *
 * This is the counterpart to add_icmp_alg_rules() and must be called once
 * for each prior call to add_icmp_alg_rules() when tearing down a PDN
 * connection.
 *
 * Return: > 0 - number of staged WAN DL rules removed; the DL rule table
 *           changed and the caller must re-push it to Q6 via
 *           install_wan_filtering_rule().
 *         0 - no change; the rule is still referenced by other connections
 *           on this PDN, or was not installed. The caller must NOT re-push.
 *         < 0 - error (-ENOENT: no PDN filter entry, or rule flagged
 *           installed but absent from the staged list). The caller must NOT
 *           re-push.
 */
int delete_icmp_alg_rules(int pdn_iface, enum ipa_ip_type iptype)
{
	struct ipa_pdn_filter_rules *pdn_entry;
	struct flt_rule *rule_entry, *tmp;
	int deleted_count = 0;
	int pdn_refs;

	IPA_BE_DBG("Delete ICMP/ALG flt rule for iface %d, ip type %d\n",
		pdn_iface, iptype);

	/* Range check before the family selection below -- see the matching
	 * check in delete_dft_filtering_rule(). enum ipa_ip_type continues past
	 * IPA_IP_MAX, and an out-of-range family would fall through the else
	 * and mutate the v6 list and count. */
	if (!VALID_IPA_IP_TYPE(iptype)) {
		IPA_BE_ERR("Invalid IP type %d\n", iptype);
		return -EINVAL;
	}

	/* Find and update the PDN filter entry */
	spin_lock_bh(&ipa_pdn_filter_lock);
	pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
	if (!pdn_entry) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_ERR("No PDN filter entry found for pdn %d, IP type %d\n",
			pdn_iface, iptype);
		return -ENOENT;
	}

	if (!pdn_entry->icmp_rule_installed) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_DBG("ICMP rule for pdn %d, IP type %d not installed, nothing to delete\n",
			pdn_iface, iptype);
		return 0;
	}

	/* Decrement ref_count for ICMP rule */
	atomic_dec(&pdn_entry->ref_count);
	pdn_refs = atomic_read(&pdn_entry->ref_count);
	IPA_BE_DBG("Decremented ref_count for ICMP rule pdn %d, IP type %d. New count: %d\n",
		pdn_iface, iptype, pdn_refs);

	/*
	 * Only delete ICMP rules when the last connection to this PDN is being
	 * removed. The threshold is 1, not 0, because dft still holds its own
	 * reference on this shared counter at this point: every connection takes
	 * one reference per rule type (dft, icmp, catchup) and ipa_be.c tears them
	 * down catchup -> icmp -> dft, so exactly one -- dft's -- remains here.
	 * The matching thresholds are 2 in delete_catchup_all_filtering_rule_each_pdn()
	 * and 0 in delete_dft_filtering_rule(); all three must be revisited together
	 * if a rule type is added to or removed from that sequence, or if the
	 * teardown order changes. eth_bh_catchall is not counted -- it carries its
	 * own eth_bh_catchall_ref_count.
	 */
	if (pdn_refs > 1) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_DBG("ICMP rule for pdn %d, IP type %d still in use (ref_count=%d), skipping delete\n",
			pdn_iface, iptype, pdn_refs);
		return 0;
	}

	/* Mark ICMP rule as no longer installed */
	pdn_entry->icmp_rule_installed = false;
	spin_unlock_bh(&ipa_pdn_filter_lock);

	/* Remove FLT_RULE_TYPE_ICMP rules for this PDN from the linked list */
	if (iptype == IPA_IP_v4) {
		spin_lock_bh(&pdn_flt_rule_v4_lock);
		list_for_each_entry_safe(rule_entry, tmp, &Pdn_flt_rule_v4_list, node) {
			if (rule_entry->pdn_iface == pdn_iface &&
			    rule_entry->rule_type == FLT_RULE_TYPE_ICMP) {
				list_del(&rule_entry->node);
				kfree(rule_entry);
				wan_rule_count_v4--;
				deleted_count++;
			}
		}
		spin_unlock_bh(&pdn_flt_rule_v4_lock);
		IPA_BE_DBG("Deleted %d ICMP IPv4 rules for pdn %d, new count: %d\n",
			deleted_count, pdn_iface, wan_rule_count_v4);
	} else {
		spin_lock_bh(&pdn_flt_rule_v6_lock);
		list_for_each_entry_safe(rule_entry, tmp, &Pdn_flt_rule_v6_list, node) {
			if (rule_entry->pdn_iface == pdn_iface &&
			    rule_entry->rule_type == FLT_RULE_TYPE_ICMP) {
				list_del(&rule_entry->node);
				kfree(rule_entry);
				wan_rule_count_v6--;
				deleted_count++;
			}
		}
		spin_unlock_bh(&pdn_flt_rule_v6_lock);
		IPA_BE_DBG("Deleted %d ICMP IPv6 rules for pdn %d, new count: %d\n",
			deleted_count, pdn_iface, wan_rule_count_v6);
	}

	if (deleted_count == 0) {
		IPA_BE_ERR("No ICMP rules found in list for pdn %d, IP type %d\n",
			pdn_iface, iptype);
		return -ENOENT;
	}

	IPA_BE_DBG("ICMP/ALG rule deleted for pdn %d, IP type %d\n", pdn_iface, iptype);
	return deleted_count;
}
EXPORT_SYMBOL(delete_icmp_alg_rules);

int add_catchup_all_filtering_rule_each_pdn(int pdn_iface, enum ipa_ip_type iptype)
{
	struct ipa_ioc_add_flt_rule *pFilteringTable = NULL;
	struct ipa_ioc_query_intf temp_intf;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	struct ipa_ioc_query_intf_ext_props *ext_prop = NULL;
	struct ipa_ioc_get_rt_tbl rt_tbl = {0};
	struct ipa_ioc_get_rt_tbl_indx rt_tbl_idx = {0};
	int retval = 0;
	int len;
	const char *rt_tbl_name;
	uint8_t mux_id[16] = {0};
	struct ipa_ioc_generate_flt_eq flt_eq;
	struct ipa_pdn_filter_rules *pdn_entry;
	struct ipa_pdn_filter_rules *prealloc;
	struct flt_rule *new_rule;
	spinlock_t *fam_lock;
	struct list_head *fam_list;
	uint32_t *fam_count;
	uint32_t rule_count;
	bool pdn_entry_newly_created = false;
	int pdn_refs;

	IPA_BE_DBG("Add catchup flt rule for iface %d, ip type %d\n",
		   pdn_iface, iptype);

	/* Range check before the family selection below -- see the matching
	 * check in add_dft_filtering_rule(). enum ipa_ip_type continues past
	 * IPA_IP_MAX, and an out-of-range family would fall through the else
	 * and mutate the v6 list and count. */
	if (!VALID_IPA_IP_TYPE(iptype)) {
		IPA_BE_ERR("Invalid IP type %d\n", iptype);
		return -EINVAL;
	}

	if (iptype == IPA_IP_v4) {
		fam_lock = &pdn_flt_rule_v4_lock;
		fam_list = &Pdn_flt_rule_v4_list;
		fam_count = &wan_rule_count_v4;
	} else {
		fam_lock = &pdn_flt_rule_v6_lock;
		fam_list = &Pdn_flt_rule_v6_list;
		fam_count = &wan_rule_count_v6;
	}

	prealloc = kzalloc(sizeof(struct ipa_pdn_filter_rules), GFP_KERNEL);
	if (!prealloc) {
		IPA_BE_ERR("Failed to allocate memory for PDN filter entry\n");
		return -ENOMEM;
	}

	/* Check if catchup filtering rule is already installed for this PDN interface */
	spin_lock_bh(&ipa_pdn_filter_lock);
	pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
	if (pdn_entry) {
		if (pdn_entry->catchup_rule_installed) {
			atomic_inc(&pdn_entry->ref_count);
			pdn_refs = atomic_read(&pdn_entry->ref_count);
			spin_unlock_bh(&ipa_pdn_filter_lock);
			kfree(prealloc);
			IPA_BE_DBG("Catchup filtering rule for pdn %d, IP type %d already installed. ref_count: %d\n",
				pdn_iface, iptype, pdn_refs);
			return 0; /* Rules already exist, just increment ref count and exit */
		}
		kfree(prealloc);
	} else {
		ipa_create_pdn_filter_entry(prealloc, pdn_iface, iptype);
		pdn_entry = prealloc;
		pdn_entry_newly_created = true;
	}
	spin_unlock_bh(&ipa_pdn_filter_lock);

	if (!ipa3_query_iface(pdn_iface, &temp_intf)) {
		IPA_BE_ERR("iface %d doesn't exist\n", pdn_iface);
		retval = -EINVAL;
		goto fail_pdn;
	}

	rx_prop = kzalloc(sizeof(*rx_prop) + temp_intf.num_rx_props *
		sizeof(struct ipa_ioc_rx_intf_prop), GFP_KERNEL);
	if (!rx_prop) {
		IPA_BE_ERR("Failed to allocate rx_prop\n");
		retval = -ENOMEM;
		goto fail_pdn;
	}
	memcpy(rx_prop->name, temp_intf.name, sizeof(rx_prop->name));
	rx_prop->num_rx_props = temp_intf.num_rx_props;

	if (ipa3_query_intf_rx_props(rx_prop)) {
		IPA_BE_ERR("Failed to query rx_prop for iface %s\n",
			   rx_prop->name);
		retval = -EIO;
		goto fail_rx;
	}

	if (rx_prop->num_rx_props == 0) {
		IPA_BE_ERR("No rx props for iface %s\n", rx_prop->name);
		retval = 0; /* Not an error, just nothing to do */
		goto fail_rx;
	}

	ext_prop = kzalloc(sizeof(*ext_prop) + temp_intf.num_ext_props *
		sizeof(struct ipa_ioc_ext_intf_prop), GFP_KERNEL);
	if (!ext_prop) {
		IPA_BE_ERR("Unable to allocate ext_prop memory.\n");
		retval = -ENOMEM;
		goto fail_rx;
	}
	memcpy(ext_prop->name, temp_intf.name, sizeof(temp_intf.name));
	ext_prop->num_ext_props = temp_intf.num_ext_props;
	ipa3_query_intf_ext_props(ext_prop);
	if (ext_prop->num_ext_props > 0)
		mux_id[0] = ext_prop->ext[0].mux_id;

	if (iptype == IPA_IP_v4) {
		rt_tbl_name = V4_LAN_ROUTE_TABLE_NAME;
	} else {
		rt_tbl_name = V6_WAN_ROUTE_TABLE_NAME;
	}

	rt_tbl.ip = iptype;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(rt_tbl.name, rt_tbl_name, sizeof(rt_tbl.name));
#else
	strlcpy(rt_tbl.name, rt_tbl_name, sizeof(rt_tbl.name));
#endif

	if (ipa3_get_rt_tbl(&rt_tbl)) {
		IPA_BE_ERR("Failed to get WAN DL routing table\n");
		retval = -EIO;
		goto fail_ext;
	}
	IPA_BE_DBG("rt_tbl hdl %d for route table %s\n", rt_tbl.hdl, rt_tbl_name);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(rt_tbl_idx.name, rt_tbl_name, sizeof(rt_tbl_idx.name));
#else
	strlcpy(rt_tbl_idx.name, rt_tbl_name, sizeof(rt_tbl_idx.name));
#endif

	rt_tbl_idx.ip = iptype;
	rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX - 1] = '\0';
	if (ipa3_query_rt_index(&rt_tbl_idx)) {
		IPA_BE_ERR("Failed to get DL routing table index\n");
		retval = -EIO;
		goto fail_ext;
	}
	IPA_BE_DBG("rt_tbl_idx %d\n", rt_tbl_idx.idx);

	len = sizeof(*pFilteringTable) + sizeof(struct ipa_flt_rule_add);
	pFilteringTable = kzalloc(len, GFP_KERNEL);
	if (!pFilteringTable) {
		IPA_BE_ERR("Failed to allocate filtering table\n");
		retval = -ENOMEM;
		goto fail_ext;
	}

	pFilteringTable->commit = 1;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = 1;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;

	struct ipa_flt_rule_add *rule = &pFilteringTable->rules[0];

	rule->at_rear = true;
	rule->rule.action = IPA_PASS_TO_DST_NAT;

	rule->rule.rt_tbl_hdl = rt_tbl.hdl;
	/* Lookup ref is transient; the filter rule above takes its own rt_tbl ref on add. */
	ipa_put_rt_tbl(rt_tbl.hdl);
	rule->rule.rt_tbl_idx = rt_tbl_idx.idx;
	rule->rule.hashable = true;
	rule->rule.retain_hdr = 1;
	rule->rule.to_uc = 0;
	rule->rule.eq_attrib_type = 1;

	memcpy(&rule->rule.attrib, &rx_prop->rx[0].attrib,
		   sizeof(rule->rule.attrib));
	rule->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;

	/* Set destination address to 0 for both IPv4 and IPv6 */
	if (iptype == IPA_IP_v4) {
		rule->rule.attrib.u.v4.dst_addr_mask = 0;
		rule->rule.attrib.u.v4.dst_addr = 0;
	} else { /* IPA_IP_v6 */
		memset(rule->rule.attrib.u.v6.dst_addr_mask, 0,
			   sizeof(rule->rule.attrib.u.v6.dst_addr_mask));
		memset(rule->rule.attrib.u.v6.dst_addr, 0,
			   sizeof(rule->rule.attrib.u.v6.dst_addr));
	}

	/* Generate filter equation for both IPv4 and IPv6 */
	memset(&flt_eq, 0, sizeof(flt_eq));
	memcpy(&flt_eq.attrib, &rule->rule.attrib, sizeof(flt_eq.attrib));
	flt_eq.ip = iptype;

	retval = ipahal_flt_generate_equation(iptype, &flt_eq.attrib, &flt_eq.eq_attrib);
	if (retval) {
		IPA_BE_ERR("Failed generating equation for ip type %d with ret %d\n",
			   iptype, retval);
		retval = -EIO;
	} else {
		memcpy(&rule->rule.eq_attrib, &flt_eq.eq_attrib, sizeof(rule->rule.eq_attrib));

		new_rule = kzalloc(sizeof(struct flt_rule), GFP_KERNEL);
		if (!new_rule) {
			IPA_BE_ERR("Failed to allocate memory for catchup rule\n");
			retval = -ENOMEM;
			goto fail_table;
		}
		memcpy(&new_rule->flt_rule, rule, sizeof(struct ipa_flt_rule_add));
		new_rule->mux_id = mux_id[0];
		new_rule->rule_type = FLT_RULE_TYPE_CATCHUP;
		new_rule->category = IPA_FLT_RULE_CAT_DOWNLINK;
		new_rule->pdn_iface = pdn_iface;

		/*
		 * Re-look-up: the build above ran unlocked. Append the record and mark
		 * the entry in one step, so a record can never outlive its owner.
		 */
		spin_lock_bh(&ipa_pdn_filter_lock);
		pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
		if (!pdn_entry) {
			spin_unlock_bh(&ipa_pdn_filter_lock);
			IPA_BE_ERR("PDN entry for pdn %d, IP type %d vanished building catchup\n",
				pdn_iface, iptype);
			kfree(new_rule);
			retval = -ENOENT;
			goto fail_table;
		}
		spin_lock_bh(fam_lock);
		lockdep_assert_held(&ipa_pdn_filter_lock);
		list_add_tail(&new_rule->node, fam_list);
		(*fam_count)++;
		rule_count = *fam_count;
		pdn_entry->catchup_rule_installed = true;
		atomic_inc(&pdn_entry->ref_count);
		pdn_refs = atomic_read(&pdn_entry->ref_count);
		spin_unlock_bh(fam_lock);
		spin_unlock_bh(&ipa_pdn_filter_lock);

		IPA_BE_DBG("Catchup rule installed for pdn %d, IP type %d. rule count %u, ref_count: %d\n",
			pdn_iface, iptype, rule_count, pdn_refs);
	}

	IPA_BE_DBG("Filter rule attrib mask: 0x%x\n", rule->rule.attrib.attrib_mask);

fail_table:
	kfree(pFilteringTable);
fail_ext:
	kfree(ext_prop);
fail_rx:
	kfree(rx_prop);
fail_pdn:
	if (pdn_entry_newly_created) {
		spin_lock_bh(&ipa_pdn_filter_lock);
		pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
		/*
		 * Drop the entry this call created only if nothing ended up installed
		 * on it. Testing retval instead would keep the entry on the "no rx
		 * props" path, which returns 0 by design, and the success path falls
		 * through to here with retval 0 as well.
		 */
		if (pdn_entry && !pdn_entry->catchup_rule_installed &&
			!pdn_entry->dft_rule_installed &&
			!pdn_entry->icmp_rule_installed &&
			!pdn_entry->eth_bh_catchall_installed &&
			atomic_read(&pdn_entry->ref_count) == 0)
			ipa_remove_pdn_filter_entry(pdn_entry);
		spin_unlock_bh(&ipa_pdn_filter_lock);
	}
	return retval;
}

int install_wan_filtering_rule(bool is_sw_routing)
{
	int ret = 0, len = 0, cnt = 0;
	struct ipa_ioc_add_flt_rule *pFilteringTable_v4 = NULL;
	struct ipa_ioc_add_flt_rule *pFilteringTable_v6 = NULL;
	uint8_t *mux_id_v4 = NULL;
	uint8_t *mux_id_v6 = NULL;
	enum flt_rule_category *category_v4 = NULL;
	enum flt_rule_category *category_v6 = NULL;
	enum rule_sub_category *sub_category_v4 = NULL;
	enum rule_sub_category *sub_category_v6 = NULL;
	struct flt_rule *rule_entry;
	/*
	 * wan_rule_count_v4/v6 are only stable under the family lock, and the walk
	 * below takes that lock separately from the sizing done here. Snapshot each
	 * count once so the allocations, the loop bound and the num_rules field
	 * cannot disagree, and set num_rules from the number of rules actually
	 * copied -- if the list shrank in between, the stale count would leave
	 * zeroed trailing rules in the table handed to the modem.
	 *
	 * Clamp to U8_MAX because num_rules is a uint8_t: without the clamp a
	 * longer list would copy rules the field cannot count, and the modem would
	 * be told about fewer rules than the table holds. Overrunning it trips the
	 * mismatch error in the walk below.
	 *
	 * READ_ONCE() because these are the only reads of either counter taken
	 * outside the family lock -- it marks the snapshot as deliberate and keeps
	 * the compiler from reloading the value, so the allocation size and the
	 * loop bound are guaranteed to be the same number.
	 */
	uint32_t alloc_rules_v4 = min_t(uint32_t, READ_ONCE(wan_rule_count_v4),
					U8_MAX);
	uint32_t alloc_rules_v6 = min_t(uint32_t, READ_ONCE(wan_rule_count_v6),
					U8_MAX);

	/* Prepare IPv4 filtering rules (do not send yet) */
	if (alloc_rules_v4 > 0) {
		len = sizeof(struct ipa_ioc_add_flt_rule) +
			(alloc_rules_v4 *
			sizeof(struct ipa_flt_rule_add));

		pFilteringTable_v4 = kzalloc(len, GFP_KERNEL);
		if (!pFilteringTable_v4) {
			IPA_BE_ERR("Failed to allocate IPv4 filtering table\n");
			ret = -ENOMEM;
			goto cleanup;
		}

		mux_id_v4 = kzalloc(alloc_rules_v4 *
			sizeof(uint8_t), GFP_KERNEL);
		if (!mux_id_v4) {
			IPA_BE_ERR("Failed to allocate mux_id_v4 array\n");
			ret = -ENOMEM;
			goto cleanup;
		}

		category_v4 = kzalloc(alloc_rules_v4 *
			sizeof(enum flt_rule_category), GFP_KERNEL);
		if (!category_v4) {
			IPA_BE_ERR("Failed to allocate category_v4 array\n");
			ret = -ENOMEM;
			goto cleanup;
		}

		sub_category_v4 = kzalloc(alloc_rules_v4 *
			sizeof(enum rule_sub_category), GFP_KERNEL);
		if (!sub_category_v4) {
			IPA_BE_ERR("Failed to allocate sub_category_v4 array\n");
			ret = -ENOMEM;
			goto cleanup;
		}

		pFilteringTable_v4->commit = 1;
		pFilteringTable_v4->ep = IPA_CLIENT_APPS_WAN_PROD;
		pFilteringTable_v4->global = false;
		pFilteringTable_v4->ip = IPA_IP_v4;

		cnt = 0;
		spin_lock_bh(&pdn_flt_rule_v4_lock);
		list_for_each_entry(rule_entry, &Pdn_flt_rule_v4_list, node) {
			if (cnt >= alloc_rules_v4) {
				IPA_BE_ERR("IPv4 rule count mismatch: cnt=%d, allocated=%u\n",
					cnt, alloc_rules_v4);
				break;
			}
			memcpy(&pFilteringTable_v4->rules[cnt],
				&rule_entry->flt_rule,
				sizeof(struct ipa_flt_rule_add));
			mux_id_v4[cnt] = rule_entry->mux_id;
			category_v4[cnt] = rule_entry->category;
			sub_category_v4[cnt] = rule_entry->sub_category;
			cnt++;
		}
		/* Describe what was copied, not what the count said before the walk. */
		pFilteringTable_v4->num_rules = cnt;
		spin_unlock_bh(&pdn_flt_rule_v4_lock);

		IPA_BE_DBG("Prepared %d IPv4 WAN filtering rules for combined installation\n",
			pFilteringTable_v4->num_rules);
	}

	/* Prepare IPv6 filtering rules (do not send yet) */
	if (alloc_rules_v6 > 0) {
		len = sizeof(struct ipa_ioc_add_flt_rule) +
			(alloc_rules_v6 *
			sizeof(struct ipa_flt_rule_add));

		pFilteringTable_v6 = kzalloc(len, GFP_KERNEL);
		if (!pFilteringTable_v6) {
			IPA_BE_ERR("Failed to allocate IPv6 filtering table\n");
			ret = -ENOMEM;
			goto cleanup;
		}

		mux_id_v6 = kzalloc(alloc_rules_v6 *
			sizeof(uint8_t), GFP_KERNEL);
		if (!mux_id_v6) {
			IPA_BE_ERR("Failed to allocate mux_id_v6 array\n");
			ret = -ENOMEM;
			goto cleanup;
		}

		category_v6 = kzalloc(alloc_rules_v6 *
			sizeof(enum flt_rule_category), GFP_KERNEL);
		if (!category_v6) {
			IPA_BE_ERR("Failed to allocate category_v6 array\n");
			ret = -ENOMEM;
			goto cleanup;
		}

		sub_category_v6 = kzalloc(alloc_rules_v6 *
			sizeof(enum rule_sub_category), GFP_KERNEL);
		if (!sub_category_v6) {
			IPA_BE_ERR("Failed to allocate sub_category_v6 array\n");
			ret = -ENOMEM;
			goto cleanup;
		}

		pFilteringTable_v6->commit = 1;
		pFilteringTable_v6->ep = IPA_CLIENT_APPS_WAN_PROD;
		pFilteringTable_v6->global = false;
		pFilteringTable_v6->ip = IPA_IP_v6;

		cnt = 0;
		spin_lock_bh(&pdn_flt_rule_v6_lock);
		list_for_each_entry(rule_entry, &Pdn_flt_rule_v6_list, node) {
			if (cnt >= alloc_rules_v6) {
				IPA_BE_ERR("IPv6 rule count mismatch: cnt=%d, allocated=%u\n",
					cnt, alloc_rules_v6);
				break;
			}
			memcpy(&pFilteringTable_v6->rules[cnt],
				&rule_entry->flt_rule,
				sizeof(struct ipa_flt_rule_add));
			mux_id_v6[cnt] = rule_entry->mux_id;
			category_v6[cnt] = rule_entry->category;
			sub_category_v6[cnt] = rule_entry->sub_category;
			cnt++;
		}
		/* Describe what was copied, not what the count said before the walk. */
		pFilteringTable_v6->num_rules = cnt;
		spin_unlock_bh(&pdn_flt_rule_v6_lock);

		IPA_BE_DBG("Prepared %d IPv6 WAN filtering rules for combined installation\n",
			pFilteringTable_v6->num_rules);
	}

	/* Send both IPv4 and IPv6 rules together */
	IPA_BE_DBG("Sending combined WAN filtering rules to modem: v4=%d, v6=%d\n",
		pFilteringTable_v4 ? pFilteringTable_v4->num_rules : 0,
		pFilteringTable_v6 ? pFilteringTable_v6->num_rules : 0);

	if (!AddWanDLFilteringRule(pFilteringTable_v4,
		pFilteringTable_v6, mux_id_v4, mux_id_v6,
		category_v4, category_v6,
		sub_category_v4, sub_category_v6,
		is_sw_routing)) {
		IPA_BE_ERR("Failed to add combined WAN filtering rules via QMI\n");
		ret = -EIO;
		goto cleanup;
	}

	IPA_BE_DBG("Successfully installed %d IPv4 filtering rules (combined)\n",
		pFilteringTable_v4 ? pFilteringTable_v4->num_rules : 0);
	IPA_BE_DBG("Successfully installed %d IPv6 filtering rules (combined)\n",
		pFilteringTable_v6 ? pFilteringTable_v6->num_rules : 0);

cleanup:
	kfree(mux_id_v4);
	kfree(mux_id_v6);
	kfree(category_v4);
	kfree(category_v6);
	kfree(sub_category_v4);
	kfree(sub_category_v6);
	kfree(pFilteringTable_v4);
	kfree(pFilteringTable_v6);

	return ret;
}

/**
 * delete_dft_filtering_rule() - Delete default WAN DL filter rules for a PDN interface
 * @pdn_iface: PDN interface number
 * @iptype: IP version (IPA_IP_v4 or IPA_IP_v6)
 *
 * Decrements pdn_entry->ref_count and removes the DEFAULT filter rules
 * (FLT_RULE_TYPE_DEFAULT) from the WAN DL filter rule list for the specified
 * PDN interface only when ref_count reaches zero and no catchup/ICMP/
 * eth-catchall rules remain installed for this PDN entry.
 *
 * Return: > 0 - number of staged WAN DL rules removed; the DL rule table
 *           changed and the caller must re-push it to Q6 via
 *           install_wan_filtering_rule().
 *         0 - no change; the rule is still referenced by other connections
 *           on this PDN, was not installed, or the PDN entry could not yet
 *           be removed because other rule types are still installed. The
 *           caller must NOT re-push.
 *         < 0 - error (-ENOENT: no PDN filter entry for this PDN and IP
 *           type). The caller must NOT re-push.
 */
int delete_dft_filtering_rule(int pdn_iface, enum ipa_ip_type iptype)
{
	struct ipa_pdn_filter_rules *pdn_entry;
	struct flt_rule *rule_entry, *tmp;
	spinlock_t *fam_lock;
	struct list_head *fam_list;
	uint32_t *fam_count;
	int set_refs, rule_count;
	int deleted_count = 0;
	int pdn_refs;
	bool entry_freed = false;

	IPA_BE_DBG("Delete dft flt rule for iface %d, ip type %d\n",
		   pdn_iface, iptype);

	/* Range check before indexing the per-family set arrays -- see the
	 * matching check in add_dft_filtering_rule(). */
	if (!VALID_IPA_IP_TYPE(iptype)) {
		IPA_BE_ERR("Invalid IP type %d\n", iptype);
		return -EINVAL;
	}

	if (iptype == IPA_IP_v4) {
		fam_lock = &pdn_flt_rule_v4_lock;
		fam_list = &Pdn_flt_rule_v4_list;
		fam_count = &wan_rule_count_v4;
	} else {
		fam_lock = &pdn_flt_rule_v6_lock;
		fam_list = &Pdn_flt_rule_v6_list;
		fam_count = &wan_rule_count_v6;
	}

	/* Find and update the PDN filter entry */
	spin_lock_bh(&ipa_pdn_filter_lock);
	pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
	if (!pdn_entry) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_ERR("No PDN filter entry found for pdn %d, IP type %d\n", pdn_iface, iptype);
		return -ENOENT;
	}

	if (!pdn_entry->dft_rule_installed) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_DBG("Default filtering rule for pdn %d, IP type %d not installed, nothing to delete\n",
			pdn_iface, iptype);
		return 0;
	}

	atomic_dec(&pdn_entry->ref_count);
	pdn_refs = atomic_read(&pdn_entry->ref_count);
	IPA_BE_DBG("Decremented ref_count for pdn %d, IP type %d. New count: %d\n",
		pdn_iface, iptype, pdn_refs);

	/*
	 * The threshold is 0 because dft is torn down last: every connection takes
	 * one reference per rule type (dft, icmp, catchup) and ipa_be.c tears them
	 * down catchup -> icmp -> dft, so no other rule type's reference remains
	 * here. The matching thresholds are 2 in
	 * delete_catchup_all_filtering_rule_each_pdn() and 1 in
	 * delete_icmp_alg_rules(); all three must be revisited together if a rule
	 * type is added to or removed from that sequence, or if the teardown order
	 * changes. Getting this one wrong is now family-wide rather than local: it
	 * gates release of the shared default set below, so releasing a reference
	 * early strips the default rules from every PDN of this family still up.
	 * eth_bh_catchall is not counted -- it carries its own
	 * eth_bh_catchall_ref_count.
	 */
	if (pdn_refs > 0) {
		/* Other connections are still using these rules, so don't delete them */
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_DBG("Default filtering rule for pdn %d, IP type %d still in use. ref_count: %d\n",
			pdn_iface, iptype, pdn_refs);
		return 0;
	}

	if (pdn_refs < 0)
		IPA_BE_ERR("pdn %d, IP type %d ref_count went negative: %d\n",
			pdn_iface, iptype, pdn_refs);

	/*
	 * This PDN is done with the default set now that its last reference is
	 * gone, whether or not its entry can be freed yet. Clear the flag and fall
	 * through to the set release on both paths: holding the reference until the
	 * catchup/icmp/eth-catchall teardown completes would pin the shared set for
	 * the life of the module, and leaving the flag set would let a second
	 * delete for this PDN drop a second reference for the one it took.
	 */
	pdn_entry->dft_rule_installed = false;

	/*
	 * Free the entry only once no other rule type is still installed on it and
	 * the count landed exactly on zero. A negative count means the add/delete
	 * accounting is already broken elsewhere and some caller may still believe
	 * it holds a reference -- leave the entry in place rather than free it out
	 * from under them.
	 */
	if (!pdn_entry->catchup_rule_installed && !pdn_entry->icmp_rule_installed &&
		!pdn_entry->eth_bh_catchall_installed && pdn_refs == 0) {
		list_del(&pdn_entry->node);
		entry_freed = true;
	}

	/*
	 * One default set serves every PDN of this family, so this PDN can only
	 * drop its reference. The records go away with the last reference --
	 * unlinking them on the first teardown would strip the set from under
	 * every PDN still up.
	 *
	 * Take fam_lock before releasing the pdn lock so the flag clear above and
	 * the release below are one step -- otherwise an add lands between, takes
	 * a fresh reference, and has it decremented here.
	 */
	spin_lock_bh(fam_lock);
	lockdep_assert_held(&ipa_pdn_filter_lock);
	spin_unlock_bh(&ipa_pdn_filter_lock);

	if (ipa_be_dft_set_ref_count[iptype] > 0)
		ipa_be_dft_set_ref_count[iptype]--;
	set_refs = ipa_be_dft_set_ref_count[iptype];

	if (set_refs == 0 && ipa_be_dft_set_installed[iptype]) {
		list_for_each_entry_safe(rule_entry, tmp, fam_list, node) {
			if (rule_entry->pdn_iface == IPA_BE_DFT_SET_PDN_IFACE &&
				rule_entry->rule_type == FLT_RULE_TYPE_DEFAULT) {
				list_del(&rule_entry->node);
				kfree(rule_entry);
				(*fam_count)--;
				deleted_count++;
			}
		}
		ipa_be_dft_set_installed[iptype] = false;
	}
	rule_count = *fam_count;
	spin_unlock_bh(fam_lock);

	/* Already off the list; freed outside both locks. */
	if (entry_freed) {
		kfree(pdn_entry);
		IPA_BE_DBG("Removed PDN filter entry for pdn %d, IP type %d\n",
			pdn_iface, iptype);
	} else {
		IPA_BE_DBG("PDN filter entry for pdn %d, IP type %d kept: other rules installed or ref_count %d not zero\n",
			pdn_iface, iptype, pdn_refs);
	}

	IPA_BE_DBG("pdn %d released the IP type %d default set. set ref_count: %d, deleted %d rules, new count: %d\n",
		pdn_iface, iptype, set_refs, deleted_count, rule_count);

	IPA_BE_DBG("Default filtering rule deleted for pdn %d, IP type %d\n", pdn_iface, iptype);
	return deleted_count;
}
EXPORT_SYMBOL(delete_dft_filtering_rule);

/**
 * delete_catchup_all_filtering_rule_each_pdn() - Delete catchup-all WAN DL
 * filter rules for a PDN interface
 * @pdn_iface: PDN interface number
 * @iptype: IP version (IPA_IP_v4 or IPA_IP_v6)
 *
 * Decrements pdn_entry->ref_count and removes the CATCHUP filter rules
 * (FLT_RULE_TYPE_CATCHUP) from the WAN DL filter rule list for the specified
 * PDN interface only when ref_count drops to 2 or below (i.e., the last
 * connection using this PDN's catchup-all rule is being torn down).
 *
 * Return: > 0 - number of staged WAN DL rules removed; the DL rule table
 *           changed and the caller must re-push it to Q6 via
 *           install_wan_filtering_rule().
 *         0 - no change; the rule is still referenced by other connections
 *           on this PDN, or was not installed. The caller must NOT re-push.
 *         < 0 - error (-ENOENT: no PDN filter entry for this PDN and IP
 *           type). The caller must NOT re-push.
 */
int delete_catchup_all_filtering_rule_each_pdn(int pdn_iface, enum ipa_ip_type iptype)
{
	struct ipa_pdn_filter_rules *pdn_entry;
	struct flt_rule *rule_entry, *tmp;
	int deleted_count = 0;
	int pdn_refs;

	IPA_BE_DBG("Delete catchup flt rule for iface %d, ip type %d\n",
		   pdn_iface, iptype);

	/* Range check before the family selection below -- see the matching
	 * check in delete_dft_filtering_rule(). enum ipa_ip_type continues past
	 * IPA_IP_MAX, and an out-of-range family would fall through the else
	 * and mutate the v6 list and count. */
	if (!VALID_IPA_IP_TYPE(iptype)) {
		IPA_BE_ERR("Invalid IP type %d\n", iptype);
		return -EINVAL;
	}

	/* Find and update the PDN filter entry */
	spin_lock_bh(&ipa_pdn_filter_lock);
	pdn_entry = ipa_find_pdn_filter_entry(pdn_iface, iptype);
	if (!pdn_entry) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_ERR("No PDN filter entry found for pdn %d, IP type %d\n", pdn_iface, iptype);
		return -ENOENT;
	}

	if (!pdn_entry->catchup_rule_installed) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_DBG("Catchup filtering rule for pdn %d, IP type %d not installed, nothing to delete\n",
			pdn_iface, iptype);
		return 0;
	}

	atomic_dec(&pdn_entry->ref_count);
	pdn_refs = atomic_read(&pdn_entry->ref_count);
	IPA_BE_DBG("Decremented ref_count for pdn %d, IP type %d. New count: %d\n",
		pdn_iface, iptype, pdn_refs);

	/*
	 * The threshold is 2, not 0, because dft and icmp still hold their own
	 * references on this shared counter at this point: every connection takes
	 * one reference per rule type (dft, icmp, catchup) and ipa_be.c tears them
	 * down catchup -> icmp -> dft, so catchup is first and exactly two remain.
	 * The matching thresholds are 1 in delete_icmp_alg_rules() and 0 in
	 * delete_dft_filtering_rule(); all three must be revisited together if a
	 * rule type is added to or removed from that sequence, or if the teardown
	 * order changes. eth_bh_catchall is not counted -- it carries its own
	 * eth_bh_catchall_ref_count.
	 */
	if (pdn_refs > 2) {
		/* Other connections are still using these rules, so don't delete them */
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_DBG("Catchup filtering rule for pdn %d, IP type %d still in use. ref_count: %d\n",
			pdn_iface, iptype, pdn_refs);
		return 0;
	}

	/* Mark as not installed */
	pdn_entry->catchup_rule_installed = false;

	/* If no default/icmp/eth-catchall rules remain and ref_count is 0, remove the entry */
	if (!pdn_entry->dft_rule_installed && !pdn_entry->icmp_rule_installed &&
		!pdn_entry->eth_bh_catchall_installed && pdn_refs == 0) {
		ipa_remove_pdn_filter_entry(pdn_entry);
		IPA_BE_DBG("Removed PDN filter entry for pdn %d, IP type %d\n", pdn_iface, iptype);
	}

	spin_unlock_bh(&ipa_pdn_filter_lock);

	/* Remove CATCHUP rules for this PDN from the linked list */
	if (iptype == IPA_IP_v4) {
		spin_lock_bh(&pdn_flt_rule_v4_lock);
		list_for_each_entry_safe(rule_entry, tmp, &Pdn_flt_rule_v4_list, node) {
			if (rule_entry->pdn_iface == pdn_iface && rule_entry->rule_type == FLT_RULE_TYPE_CATCHUP) {
				list_del(&rule_entry->node);
				kfree(rule_entry);
				wan_rule_count_v4--;
				deleted_count++;
			}
		}
		spin_unlock_bh(&pdn_flt_rule_v4_lock);
		IPA_BE_DBG("Deleted %d catchup IPv4 rules for pdn %d, new count: %d\n",
			deleted_count, pdn_iface, wan_rule_count_v4);
	} else {
		spin_lock_bh(&pdn_flt_rule_v6_lock);
		list_for_each_entry_safe(rule_entry, tmp, &Pdn_flt_rule_v6_list, node) {
			if (rule_entry->pdn_iface == pdn_iface && rule_entry->rule_type == FLT_RULE_TYPE_CATCHUP) {
				list_del(&rule_entry->node);
				kfree(rule_entry);
				wan_rule_count_v6--;
				deleted_count++;
			}
		}
		spin_unlock_bh(&pdn_flt_rule_v6_lock);
		IPA_BE_DBG("Deleted %d catchup IPv6 rules for pdn %d, new count: %d\n",
			deleted_count, pdn_iface, wan_rule_count_v6);
	}

	IPA_BE_DBG("Catchup filtering rule deleted for pdn %d, IP type %d\n", pdn_iface, iptype);
	return deleted_count;
}
EXPORT_SYMBOL(delete_catchup_all_filtering_rule_each_pdn);

static int ipa_be_eth_bh_catchall_hw_delete(uint32_t flt_rule_hdl,
					    enum ipa_ip_type ip_type)
{
	struct ipa_ioc_del_flt_rule *del_param;
	size_t size = sizeof(*del_param) + sizeof(struct ipa_flt_rule_del);
	int ret;

	del_param = kzalloc(size, GFP_KERNEL);
	if (!del_param)
		return -ENOMEM;

	del_param->commit = 1;
	del_param->num_hdls = 1;
	del_param->ip = ip_type;
	del_param->hdl[0].status = -1;
	del_param->hdl[0].hdl = flt_rule_hdl;
	ret = ipa3_del_flt_rule(del_param);
	kfree(del_param);
	return ret;
}

static int ipa_be_eth_bh_install_dl_catchall(int pdn_iface,
					     enum ipa_ip_type ip_type,
					     uint32_t wan_vlan)
{
	struct ipa_pdn_filter_rules *entry;
	struct ipa_pdn_filter_rules *new_entry = NULL;
	struct ipa_ioc_query_intf temp_intf;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	struct ipa_ioc_add_flt_rule *flt_table = NULL;
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa_ioc_get_rt_tbl rt_tbl_lookup = {0};
	bool rt_tbl_got = false;
	uint32_t installed_hdls[IPA_MAX_NUM_PROPS] = {0};
	int num_installed = 0;
	int j, idx;
	int retval = 0;

retry:
	spin_lock_bh(&ipa_pdn_filter_lock);
	entry = ipa_find_pdn_filter_entry(pdn_iface, ip_type);
	if (entry) {
		if (entry->eth_bh_catchall_installing) {
			spin_unlock_bh(&ipa_pdn_filter_lock);
			wait_event(ipa_pdn_filter_wq,
				   ipa_pdn_eth_bh_catchall_settled(pdn_iface, ip_type));
			goto retry;
		}
		if (entry->eth_bh_catchall_installed) {
			entry->eth_bh_catchall_ref_count++;
			spin_unlock_bh(&ipa_pdn_filter_lock);
			IPA_BE_DBG("ETH_BH catchall already installed pdn %d ip %d ref %d\n",
				pdn_iface, ip_type,
				entry->eth_bh_catchall_ref_count);
			return 0;
		}
		entry->eth_bh_catchall_installing = true;
		spin_unlock_bh(&ipa_pdn_filter_lock);
	} else {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		new_entry = kzalloc(sizeof(*new_entry), GFP_KERNEL);
		if (!new_entry)
			return -ENOMEM;

		spin_lock_bh(&ipa_pdn_filter_lock);
		entry = ipa_find_pdn_filter_entry(pdn_iface, ip_type);
		if (entry) {
			spin_unlock_bh(&ipa_pdn_filter_lock);
			kfree(new_entry);
			new_entry = NULL;
			goto retry;
		}
		ipa_create_pdn_filter_entry(new_entry, pdn_iface, ip_type);
		entry = new_entry;
		new_entry = NULL;
		entry->eth_bh_catchall_installing = true;
		spin_unlock_bh(&ipa_pdn_filter_lock);
	}

	if (!ipa3_query_iface(pdn_iface, &temp_intf)) {
		IPA_BE_ERR("ETH_BH catchall: pdn_iface %d does not exist\n",
			pdn_iface);
		retval = -EINVAL;
		goto finalize;
	}

	rx_prop = kzalloc(sizeof(*rx_prop) +
			  temp_intf.num_rx_props *
				  sizeof(struct ipa_ioc_rx_intf_prop),
			  GFP_KERNEL);
	if (!rx_prop) {
		retval = -ENOMEM;
		goto finalize;
	}

	memcpy(rx_prop->name, temp_intf.name, sizeof(temp_intf.name));
	rx_prop->num_rx_props = temp_intf.num_rx_props;
	if (ipa3_query_intf_rx_props(rx_prop)) {
		IPA_BE_ERR("ETH_BH catchall: query rx_prop failed for %s\n",
			rx_prop->name);
		retval = -EIO;
		goto finalize;
	}

	if (rx_prop->num_rx_props == 0) {
		IPA_BE_DBG("ETH_BH catchall: no rx_props for %s\n",
			rx_prop->name);
		retval = 0;
		goto finalize;
	}

	rt_tbl_lookup.ip = ip_type;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	if (ip_type == IPA_IP_v4)
		strscpy(rt_tbl_lookup.name, V4_LAN_ROUTE_TABLE_NAME,
			sizeof(rt_tbl_lookup.name));
	else
		strscpy(rt_tbl_lookup.name, V6_WAN_ROUTE_TABLE_NAME,
			sizeof(rt_tbl_lookup.name));
#else
	if (ip_type == IPA_IP_v4)
		strlcpy(rt_tbl_lookup.name, V4_LAN_ROUTE_TABLE_NAME,
			sizeof(rt_tbl_lookup.name));
	else
		strlcpy(rt_tbl_lookup.name, V6_WAN_ROUTE_TABLE_NAME,
			sizeof(rt_tbl_lookup.name));
#endif

	if (ipa3_get_rt_tbl(&rt_tbl_lookup)) {
		IPA_BE_ERR("ETH_BH catchall: failed to get rt_tbl ip %d\n",
			ip_type);
		retval = -EFAULT;
		goto finalize;
	}
	rt_tbl_got = true;

	for (j = 0; j < rx_prop->num_rx_props / 2 && j < IPA_MAX_NUM_PROPS;
	     j++) {
		idx = (ip_type == IPA_IP_v6) ? (j * 2 + 1) : (j * 2);
		if (idx >= rx_prop->num_rx_props)
			break;

		if (rx_prop->rx[idx].ip != ip_type) {
			IPA_BE_DBG("IP not matching required type %d .. continue\n",
				rx_prop->rx[idx].ip);
			continue;
		}

		flt_table = kzalloc(sizeof(*flt_table) +
				    sizeof(struct ipa_flt_rule_add),
				    GFP_KERNEL);
		if (!flt_table) {
			retval = -ENOMEM;
			goto finalize;
		}

		flt_table->commit = 1;
		flt_table->ep = rx_prop->rx[idx].src_pipe;
		flt_table->global = false;
		flt_table->ip = ip_type;
		flt_table->num_rules = 1;

		memset(&flt_rule_entry, 0, sizeof(flt_rule_entry));
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;
		flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
#ifdef FEATURE_IPA_V3
		flt_rule_entry.rule.hashable = true;
#endif
		flt_rule_entry.rule.rt_tbl_hdl = rt_tbl_lookup.hdl;
		memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[idx].attrib,
		       sizeof(flt_rule_entry.rule.attrib));
		if (wan_vlan != 0 && wan_vlan != IPA_VLAN_ID_NOT_CONFIGURED) {
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_VLAN_ID;
			flt_rule_entry.rule.attrib.vlan_id = wan_vlan & 0x0FFF;
		}
		flt_rule_entry.flt_rule_category =
			IPA_FLT_RULE_CAT_ETH_BH_CATCHALL;

		memcpy(&flt_table->rules[0], &flt_rule_entry,
		       sizeof(flt_rule_entry));

		if (ipa3_add_flt_rule_usr(flt_table, true)) {
			IPA_BE_ERR("ETH_BH catchall: ipa3_add_flt_rule_usr failed\n");
			kfree(flt_table);
			flt_table = NULL;
			retval = -EFAULT;
			goto finalize;
		} else {
			struct ipa3_flt_entry flt_track = {0};

			IPA_BE_DBG("ETH_BH catchall flt rule hdl=0x%x status=0x%x\n",
				flt_table->rules[0].flt_rule_hdl,
				flt_table->rules[0].status);
			installed_hdls[num_installed++] =
				flt_table->rules[0].flt_rule_hdl;
			flt_track.flt_hdl = flt_table->rules[0].flt_rule_hdl;
			flt_track.cat = IPA_FLT_RULE_CAT_ETH_BH_CATCHALL;
			flt_track.ip_type = ip_type;
			ipa3_add_filter_rules_entry(pdn_iface, flt_track);
		}

		kfree(flt_table);
		flt_table = NULL;
	}

finalize:
	/* Lookup ref is transient; each filter rule installed above took its own rt_tbl ref. */
	if (rt_tbl_got)
		ipa_put_rt_tbl(rt_tbl_lookup.hdl);
	if (flt_table)
		kfree(flt_table);
	if (rx_prop)
		kfree(rx_prop);

	/* On failure, delete any partially-installed HW rules before
	 * touching the tracking entry. */
	if (retval != 0) {
		struct ipa3_flt_entry flt_track = {0};

		for (j = 0; j < num_installed; j++)
			ipa_be_eth_bh_catchall_hw_delete(installed_hdls[j], ip_type);

		/* Purge SW tracking entries; HW delete above doesn't touch them. */
		flt_track.cat = IPA_FLT_RULE_CAT_ETH_BH_CATCHALL;
		flt_track.ip_type = ip_type;
		for (;;) {
			if (ipa3_delete_filter_rules_entry(pdn_iface, flt_track) == -1)
				break;
		}
	}

	spin_lock_bh(&ipa_pdn_filter_lock);
	if (retval == 0 && num_installed > 0) {
		entry->eth_bh_catchall_installed = true;
		entry->eth_bh_catchall_ref_count = 1;
		entry->eth_bh_catchall_num_hdls = num_installed;
		entry->eth_bh_catchall_wan_vlan = wan_vlan;
		for (j = 0; j < num_installed; j++)
			entry->eth_bh_catchall_flt_hdl[j] = installed_hdls[j];
	} else if (retval == 0 && num_installed == 0) {
		IPA_BE_ERR("ETH_BH catchall: no matching rx props for pdn %d ip %d — rx layout mismatch\n",
			pdn_iface, ip_type);
		retval = -EINVAL;
	}
	entry->eth_bh_catchall_installing = false;

	if (!entry->eth_bh_catchall_installed && !entry->dft_rule_installed &&
	    !entry->icmp_rule_installed && !entry->catchup_rule_installed &&
	    atomic_read(&entry->ref_count) == 0) {
		list_del(&entry->node);
		spin_unlock_bh(&ipa_pdn_filter_lock);
		kfree(entry);
	} else {
		spin_unlock_bh(&ipa_pdn_filter_lock);
	}

	wake_up_all(&ipa_pdn_filter_wq);

	return retval;
}

static int ipa_be_eth_bh_remove_dl_catchall(int pdn_iface,
					    enum ipa_ip_type ip_type)
{
	struct ipa_pdn_filter_rules *entry;
	uint32_t saved_hdls[IPA_MAX_NUM_PROPS] = {0};
	struct ipa3_flt_entry flt_entry = {0};
	int num_hdls = 0;
	bool free_entry = false;
	int j;

	spin_lock_bh(&ipa_pdn_filter_lock);
	entry = ipa_find_pdn_filter_entry(pdn_iface, ip_type);
	if (!entry || !entry->eth_bh_catchall_installed) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_ERR("ETH_BH catchall remove: no entry for pdn %d ip %d\n",
			pdn_iface, ip_type);
		return -ENOENT;
	}

	entry->eth_bh_catchall_ref_count--;
	if (entry->eth_bh_catchall_ref_count > 0) {
		spin_unlock_bh(&ipa_pdn_filter_lock);
		IPA_BE_DBG("ETH_BH catchall ref-- pdn %d ip %d ref %d\n",
			pdn_iface, ip_type,
			entry->eth_bh_catchall_ref_count);
		return 0;
	}

	num_hdls = entry->eth_bh_catchall_num_hdls;
	for (j = 0; j < num_hdls; j++)
		saved_hdls[j] = entry->eth_bh_catchall_flt_hdl[j];

	entry->eth_bh_catchall_installed = false;
	entry->eth_bh_catchall_num_hdls = 0;
	memset(entry->eth_bh_catchall_flt_hdl, 0,
	       sizeof(entry->eth_bh_catchall_flt_hdl));

	if (!entry->dft_rule_installed && !entry->icmp_rule_installed &&
	    !entry->catchup_rule_installed && atomic_read(&entry->ref_count) == 0) {
		list_del(&entry->node);
		free_entry = true;
	}
	spin_unlock_bh(&ipa_pdn_filter_lock);

	if (free_entry)
		kfree(entry);

	for (j = 0; j < num_hdls; j++)
		ipa_be_eth_bh_catchall_hw_delete(saved_hdls[j], ip_type);

	flt_entry.cat = IPA_FLT_RULE_CAT_ETH_BH_CATCHALL;
	flt_entry.ip_type = ip_type;
	for (;;) {
		if (ipa3_delete_filter_rules_entry(pdn_iface, flt_entry) == -1)
			break;
	}

	return 0;
}

/**
 * ipa_be_handle_wan_up() - Install eth-backhaul WAN filtering rules
 * @pdn_iface: For downlink (is_eth_bh_downlink=true):  WAN interface.
 *             For uplink  (is_eth_bh_downlink=false): LAN client interface.
 * @client_iface: Unused for downlink.  For uplink: the WAN (PDN) interface.
 * @ip_type: IPA_IP_v4 or IPA_IP_v6
 * @is_eth_bh_downlink: true  -> install/ref the shared per-WAN DL catch-all rule
 *                               (WAN-VLAN match, DST_NAT, LAN routing table).
 *                      false -> install/ref the shared per-physical-pipe UL rule
 *                               (match-all, SRC_NAT, WAN routing table).
 * @dst_ip: For downlink: pointer to the WAN VLAN tag (vlan_primary_rule.egress_vlan_tag).
 *          For uplink: ignored.
 *
 * Return: 0 on success, negative errno on failure
 */
int ipa_be_handle_wan_up(
	int pdn_iface,
	int client_iface,
	enum ipa_ip_type ip_type,
	bool is_eth_bh_downlink,
	const uint32_t *dst_ip)
{
	struct ipa_flt_rule_add flt_rule_entry;
	int len = 0, idx = 0;
	int key_idx, key_pipe;
	int rules_installed = 0;
	struct ipa_ioc_add_flt_rule *m_pFilteringTable = NULL;
	int j = 0;
	struct ipa_ioc_get_rt_tbl rt_tbl_wan = {0};
	struct ipa_ioc_query_intf temp_intf;
	struct ipa_ioc_query_intf_rx_props *rx_prop = NULL;
	int retval = 0;
	struct ipa_uplink_pair *pair;
	struct ipa_uplink_pair *new_pair = NULL;
	enum flt_rule_category flt_cat = IPA_FLT_RULE_CAT_UPLINK;

	if (is_eth_bh_downlink)
		return ipa_be_eth_bh_install_dl_catchall(pdn_iface, ip_type,
			dst_ip ? *dst_ip : IPA_VLAN_ID_NOT_CONFIGURED);

	IPA_BE_DBG("Set WAN interface as default filter rule for pdn_iface %d client_iface %d\n",
		pdn_iface, client_iface);

	/* Query rx properties early so we can use src_pipe as the dedup key. */
	if (!ipa3_query_iface(pdn_iface, &temp_intf)) {
		IPA_BE_ERR("Interface with index %u does not exist.\n", pdn_iface);
		return -EINVAL;
	}

	rx_prop = (struct ipa_ioc_query_intf_rx_props *)kzalloc(
		sizeof(struct ipa_ioc_query_intf_rx_props) +
		temp_intf.num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop),
		GFP_KERNEL);
	if (rx_prop == NULL) {
		IPA_BE_ERR("Unable to allocate rx_prop memory.\n");
		return -ENOMEM;
	}

	memcpy(rx_prop->name, temp_intf.name, sizeof(temp_intf.name));
	rx_prop->num_rx_props = temp_intf.num_rx_props;
	IPA_BE_DBG("Query rx_prop %d name %s\n", rx_prop->num_rx_props, temp_intf.name);

	if (ipa3_query_intf_rx_props(rx_prop)) {
		IPA_BE_ERR("Failed to query rx_prop for iface %s\n", rx_prop->name);
		kfree(rx_prop);
		return -EIO;
	}

	if (rx_prop->num_rx_props == 0) {
		IPA_BE_DBG("No rx properties for iface %s\n", rx_prop->name);
		kfree(rx_prop);
		return 0;
	}

	/* Use the same rx index the install loop uses: rx[1] for v6, rx[0] for v4. */
	key_idx = (ip_type == IPA_IP_v6) ? 1 : 0;
	if (key_idx >= rx_prop->num_rx_props)
		key_idx = 0;
	key_pipe = rx_prop->rx[key_idx].src_pipe;

	IPA_BE_DBG("Query response rx_prop src %d hdr_l2_type %d\n",
		key_pipe, rx_prop->rx[key_idx].hdr_l2_type);

	new_pair = kzalloc(sizeof(struct ipa_uplink_pair), GFP_KERNEL);
	if (!new_pair) {
		IPA_BE_ERR("Failed to allocate memory for WAN-up pair tracking\n");
		kfree(rx_prop);
		return -ENOMEM;
	}

	/* Dedup on (pdn_iface, src_pipe, ip_type): all VLAN sub-interfaces on
	 * one physical port share the same src_pipe and therefore the same
	 * match-all uplink filter rule. */
retry:
	spin_lock_bh(&ipa_uplink_lock);
	list_for_each_entry(pair, &ipa_uplink_pairs_list, node) {
		if (pair->pdn_iface == pdn_iface &&
		    pair->src_pipe == key_pipe &&
		    pair->ip_type == ip_type) {
			if (pair->installing) {
				spin_unlock_bh(&ipa_uplink_lock);
				wait_event(ipa_uplink_wq,
					   ipa_uplink_pair_settled(pdn_iface,
								   key_pipe,
								   ip_type));
				goto retry;
			}
			atomic_inc(&pair->ref_count);
			spin_unlock_bh(&ipa_uplink_lock);
			kfree(new_pair);
			new_pair = NULL;
			IPA_BE_DBG("WAN-up filter for pdn %d, src_pipe %d, IP type %d already installed. ref_count: %d\n",
				pdn_iface, key_pipe, ip_type, atomic_read(&pair->ref_count));
			kfree(rx_prop);
			return 0;
		}
	}
	new_pair->pdn_iface = pdn_iface;
	new_pair->client_iface = client_iface;
	new_pair->src_pipe = key_pipe;
	new_pair->ip_type = ip_type;
	atomic_set(&new_pair->ref_count, 1);
	new_pair->installing = true;
	list_add(&new_pair->node, &ipa_uplink_pairs_list);
	spin_unlock_bh(&ipa_uplink_lock);
	IPA_BE_DBG("Pre-registered WAN-up pair tracking for pdn %d, src_pipe %d, IP type %d\n",
		pdn_iface, key_pipe, ip_type);

	for (j = 0; j < rx_prop->num_rx_props / 2 && j < IPA_MAX_NUM_PROPS; j++) {

		idx = (ip_type == IPA_IP_v6) ? (j * 2 + 1) : (j * 2);

		/* Validate array bounds before accessing */
		if (idx >= rx_prop->num_rx_props) {
			IPA_BE_ERR("Index %d exceeds num_rx_props %d, breaking loop\n",
				idx, rx_prop->num_rx_props);
			break;
		}

		if (rx_prop->rx[idx].ip != ip_type) {
			IPA_BE_DBG("IP not matching required type %d .. continue\n",
				rx_prop->rx[idx].ip);
			continue;
		}

		IPA_BE_DBG("Install rules at idx %d\n", idx);

		if (ip_type == IPA_IP_v4) {
			/* Add MTU rules for ipv4 */
			/* TODO: Call modify_private_subnet() equivalent if needed */

			len = sizeof(struct ipa_ioc_add_flt_rule) + (1 * sizeof(struct ipa_flt_rule_add));
			m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)kzalloc(len, GFP_KERNEL);
			if (!m_pFilteringTable) {
				IPA_BE_ERR("Error allocating ipa_flt_rule_add memory\n");
				retval = -ENOMEM;
				goto cleanup;
			}

			m_pFilteringTable->commit = 1;
			m_pFilteringTable->ep = rx_prop->rx[idx].src_pipe;
			m_pFilteringTable->global = false;
			m_pFilteringTable->ip = IPA_IP_v4;
			m_pFilteringTable->num_rules = 1;

			IPA_BE_DBG("Retrieving routing handle for WAN v4 table\n");
			rt_tbl_wan.ip = IPA_IP_v4;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
			strscpy(rt_tbl_wan.name, V4_WAN_ROUTE_TABLE_NAME,
				sizeof(rt_tbl_wan.name));
#else
			strlcpy(rt_tbl_wan.name, V4_WAN_ROUTE_TABLE_NAME,
				sizeof(rt_tbl_wan.name));
#endif
			if (ipa3_get_rt_tbl(&rt_tbl_wan)) {
				IPA_BE_ERR("Failed to get WAN v4 routing table\n");
				kfree(m_pFilteringTable);
				retval = -EFAULT;
				goto cleanup;
			}
			IPA_BE_DBG("Routing handle for table: %d\n", rt_tbl_wan.hdl);

			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
			flt_rule_entry.at_rear = true;
			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;

			flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;
#ifdef FEATURE_IPA_V3
			flt_rule_entry.rule.hashable = true;
#endif
			flt_rule_entry.rule.rt_tbl_hdl = rt_tbl_wan.hdl;
			/* Transient lookup ref; release before rt_tbl_wan is
			 * reused for the v6 lookup below. */
			ipa_put_rt_tbl(rt_tbl_wan.hdl);

			memcpy(&flt_rule_entry.rule.attrib,
				   &rx_prop->rx[idx].attrib,
				   sizeof(flt_rule_entry.rule.attrib));

			/* Match-all catch-all: keep DST_ADDR attrib with zero
			 * addr/mask so the rule matches every destination. */
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			flt_rule_entry.rule.attrib.u.v4.dst_addr = 0x0;
			flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0x0;

			flt_rule_entry.flt_rule_category = flt_cat;

			memcpy(&m_pFilteringTable->rules[0], &flt_rule_entry, sizeof(flt_rule_entry));

			if (ipa3_add_flt_rule_usr(m_pFilteringTable, true)) {
				IPA_BE_ERR("Error adding filtering rule to IPA\n");
				kfree(m_pFilteringTable);
				retval = -EFAULT;
				goto cleanup;
			} else {
				struct ipa3_flt_entry flt_entry = {0};

				IPA_BE_DBG("flt rule hdl=0x%x, status=0x%x\n",
						   m_pFilteringTable->rules[0].flt_rule_hdl,
						   m_pFilteringTable->rules[0].status);
				flt_entry.flt_hdl = m_pFilteringTable->rules[0].flt_rule_hdl;
				flt_entry.cat = flt_cat;
				flt_entry.ip_type = ip_type;
				ipa3_add_filter_rules_entry(pdn_iface, flt_entry);
				rules_installed++;
			}

			kfree(m_pFilteringTable);

		} else if (ip_type == IPA_IP_v6) {
			/* Add ipv6_mtu rule */
			/* TODO: Call modify_ipv6_prefix_flt_rule() equivalent if needed */

			/* MTU might have changed. Need to update ipv4 MTU rule if up */
			/* TODO: Check if WAN is up and call modify_private_subnet() if needed */

			/* Add default v6 filter rule */
			m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
				kzalloc(sizeof(struct ipa_ioc_add_flt_rule) +
					   1 * sizeof(struct ipa_flt_rule_add), GFP_KERNEL);

			if (!m_pFilteringTable) {
				IPA_BE_ERR("Error allocating ipa_flt_rule_add memory\n");
				retval = -ENOMEM;
				goto cleanup;
			}

			m_pFilteringTable->commit = 1;
			m_pFilteringTable->ep = rx_prop->rx[idx].src_pipe;
			m_pFilteringTable->global = false;
			m_pFilteringTable->ip = IPA_IP_v6;
			m_pFilteringTable->num_rules = 1;

			rt_tbl_wan.ip = IPA_IP_v6;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
			strscpy(rt_tbl_wan.name, V6_WAN_ROUTE_TABLE_NAME, sizeof(rt_tbl_wan.name));
#else
			strlcpy(rt_tbl_wan.name, V6_WAN_ROUTE_TABLE_NAME, sizeof(rt_tbl_wan.name));
#endif
			if (ipa3_get_rt_tbl(&rt_tbl_wan)) {
				IPA_BE_ERR("Failed to get WAN v6 routing table\n");
				kfree(m_pFilteringTable);
				retval = -EFAULT;
				goto cleanup;
			}

			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

			flt_rule_entry.at_rear = true;
			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;

			flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;

#ifdef FEATURE_IPA_V3
			flt_rule_entry.rule.hashable = true;
#endif
			flt_rule_entry.rule.rt_tbl_hdl = rt_tbl_wan.hdl;
			/* Lookup ref is transient; the filter rule takes its own rt_tbl ref on add. */
			ipa_put_rt_tbl(rt_tbl_wan.hdl);

			memcpy(&flt_rule_entry.rule.attrib,
				   &rx_prop->rx[idx].attrib,
				   sizeof(flt_rule_entry.rule.attrib));

			/* Match-all catch-all: keep DST_ADDR attrib with zero
			 * addr/mask so the rule matches every destination. */
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = 0x0;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x0;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x0;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0x0;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0x0;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x0;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x0;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x0;

			flt_rule_entry.flt_rule_category = flt_cat;

			memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

			if (ipa3_add_flt_rule_usr(m_pFilteringTable, true)) {
				IPA_BE_ERR("Error adding filtering rule\n");
				kfree(m_pFilteringTable);
				retval = -EFAULT;
				goto cleanup;
			} else {
				struct ipa3_flt_entry flt_entry = {0};

				IPA_BE_DBG("flt rule hdl=0x%x, status=0x%x\n",
					m_pFilteringTable->rules[0].flt_rule_hdl,
					m_pFilteringTable->rules[0].status);
				flt_entry.flt_hdl = m_pFilteringTable->rules[0].flt_rule_hdl;
				flt_entry.cat = flt_cat;
				flt_entry.ip_type = ip_type;
				ipa3_add_filter_rules_entry(pdn_iface, flt_entry);
				rules_installed++;
			}

			kfree(m_pFilteringTable);
		}
	}

	/* Roll back the pre-registered pair if installation failed */
cleanup:
	if (retval == 0 && rules_installed == 0) {
		IPA_BE_ERR("WAN-up: no rules installed for pdn %d ip %d — rx layout mismatch\n",
			pdn_iface, ip_type);
		retval = -EINVAL;
	}
	if (new_pair && retval == 0) {
		spin_lock_bh(&ipa_uplink_lock);
		if (atomic_read(&new_pair->ref_count) > 0) {
			new_pair->installing = false;
			spin_unlock_bh(&ipa_uplink_lock);
		} else {
			list_del(&new_pair->node);
			spin_unlock_bh(&ipa_uplink_lock);
			kfree(new_pair);
			new_pair = NULL;
			ipa_be_delete_rules_by_category(pdn_iface, IPA_FLT_RULE_CAT_UPLINK, ip_type);
		}
	}
	if (retval != 0 && new_pair) {
		spin_lock_bh(&ipa_uplink_lock);
		list_del(&new_pair->node);
		kfree(new_pair);
		new_pair = NULL;
		spin_unlock_bh(&ipa_uplink_lock);
		ipa_be_delete_rules_by_category(pdn_iface, IPA_FLT_RULE_CAT_UPLINK, ip_type);
		IPA_BE_ERR("Rolled back WAN-up pair tracking for pdn %d, client %d due to install failure\n",
			pdn_iface, client_iface);
	}

	wake_up_all(&ipa_uplink_wq);

	if (rx_prop)
		kfree(rx_prop);

	return retval;
}
EXPORT_SYMBOL(ipa_be_handle_wan_up);

/**
 * ipa_be_handle_wan_down() - Remove WAN filtering rules installed by ipa_be_handle_wan_up
 * @pdn_iface: Interface passed as first arg to ipa_be_handle_wan_up
 * @client_iface: Interface passed as second arg to ipa_be_handle_wan_up
 * @ip_type: IP type (IPA_IP_v4 or IPA_IP_v6)
 *
 * For eth-backhaul, ipa_be.c calls this twice per connection teardown:
 *   wan_down(pdn_iface, client_iface)  - pdn_iface is the WAN iface; no uplink
 *                                        pair carries WAN as pdn, so !found ->
 *                                        removes the shared DL catch-all ref.
 *   wan_down(client_iface, pdn_iface)  - client_iface is the LAN iface; the
 *                                        uplink pair was installed with pdn=LAN,
 *                                        so found -> removes UPLINK rule.
 *
 * Return: 0 on success, negative errno on failure
 */
int ipa_be_handle_wan_down(
	int pdn_iface,
	int client_iface,
	enum ipa_ip_type ip_type)
{
	struct ipa_uplink_pair *pair, *tmp;
	bool found = false;

	IPA_BE_DBG("WAN down for pdn_iface %d client_iface %d ip_type %d\n",
		pdn_iface, client_iface, ip_type);

	spin_lock_bh(&ipa_uplink_lock);
	list_for_each_entry_safe(pair, tmp, &ipa_uplink_pairs_list, node) {
		if (pair->pdn_iface == pdn_iface && pair->ip_type == ip_type) {
			atomic_dec(&pair->ref_count);
			IPA_BE_DBG("Decremented ref_count for WAN-up pair pdn %d, src_pipe %d, IP type %d. New count: %d\n",
				pdn_iface, pair->src_pipe, ip_type, atomic_read(&pair->ref_count));
			if (atomic_read(&pair->ref_count) > 0) {
				spin_unlock_bh(&ipa_uplink_lock);
				IPA_BE_DBG("WAN filter for pdn %d, src_pipe %d, IP type %d still in use\n",
					pdn_iface, pair->src_pipe, ip_type);
				return 0;
			}
			if (pair->installing) {
				spin_unlock_bh(&ipa_uplink_lock);
				return 0;
			}
			list_del(&pair->node);
			kfree(pair);
			found = true;
			break;
		}
	}
	spin_unlock_bh(&ipa_uplink_lock);

	if (!found)
		return ipa_be_eth_bh_remove_dl_catchall(pdn_iface, ip_type);

	IPA_BE_DBG("Deleting WAN uplink filter rules for pdn_iface %d, IP type %d\n",
		pdn_iface, ip_type);

	return ipa_be_delete_rules_by_category(pdn_iface, IPA_FLT_RULE_CAT_UPLINK, ip_type);
}
EXPORT_SYMBOL(ipa_be_handle_wan_down);
