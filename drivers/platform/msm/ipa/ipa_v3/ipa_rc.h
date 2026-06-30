// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#ifndef _IPA_RC_H_
#define _IPA_RC_H_

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/msm_ipa.h>
#include "ipahal.h"

#define DATA_DIR 2        /* data traffic directions: uplink/downlink */
#define LIST_MAX_LEN_DEBUG 10  /* Store up to 10 instances for debug enhancement */
#define LIST_MAX_LEN 3         /* Number of instances used for status calculation */
#define IPA_MAX_MSG_LEN 4096   /* Buffer size allocated for IPA messages */
#define IPA_COLLECT_INTERVAL_MS 50000 /* Initial interval (ms) for scheduling the workqueue */
#define MAX_RC_CLIENTS 7  /* Max no. of type of clients for ipa-rc */
#define DROP_SAMPLE_WINDOW  10  /* rolling 1-sec sample window for drop detection */
#define DROP_CONFIRM_THRESH  3  /* min drop samples in window to confirm a real drop */

/*
 * ipa_rc_state_err error code names (bitmask)
 *
 * @IPA_HEALTH_OK: No health error detected.
 *
 * Channel not started errors:
 * @IPA_CHANNEL_ETH_NOT_STARTED: Ethernet tethering/IPA channel is not started.
 * @IPA_CHANNEL_WLAN_NOT_STARTED: WLAN tethering/IPA channel is not started.
 * @IPA_CHANNEL_ETH1_NOT_STARTED: Ethernet 1 tethering/IPA channel is not started.
 * @IPA_CHANNEL_WLAN2_NOT_STARTED: WLAN 2 tethering/IPA channel is not started.
 * @IPA_CHANNEL_WLAN3_NOT_STARTED: WLAN 3 tethering/IPA channel is not started.
 *
 * Buffer insufficient errors:
 * @IPA_CHANNEL_APPS_EMB_NO_BUFF: APPS embedded endpoint has no buffer.
 * @IPA_CHANNEL_Q6_NO_BUFF: Q6 (modem) endpoint has no buffer.
 * @IPA_CHANNEL_ETH_NO_BUFF: Ethernet endpoint has no buffer.
 * @IPA_CHANNEL_WLAN_NO_BUFF: WLAN endpoint has no buffer.
 * @IPA_CHANNEL_ETH1_NO_BUFF: Ethernet 1 endpoint has no buffer.
 * @IPA_CHANNEL_WLAN2_NO_BUFF: WLAN 2 endpoint has no buffer.
 * @IPA_CHANNEL_WLAN3_NO_BUFF: WLAN 3 endpoint has no buffer.
 *
 * Driver packet drop indications:
 * @IPA_DRIVER_ETH_PKT_DROP: Packets originating from Ethernet are dropping in IPA driver.
 * @IPA_DRIVER_WLAN_AP_PKT_DROP: Packets originating from WLAN AP path are dropping in IPA driver.
 * @IPA_DRIVER_WLAN_STA_PKT_DROP: Packets originating from WLAN STA path are dropping in IPA driver.
 * @IPA_DRIVER_ETH1_PKT_DROP: Packets originating from Ethernet 1 are dropping in IPA driver.
 * @IPA_DRIVER_WLAN2_AP_PKT_DROP: Packets originating from WLAN 2 AP path are dropping in IPA driver.
 * @IPA_DRIVER_WLAN2_STA_PKT_DROP: Packets originating from WLAN 2 STA path are dropping in IPA driver.
 * @IPA_DRIVER_WLAN3_AP_PKT_DROP: Packets originating from WLAN 3 AP path are dropping in IPA driver.
 * @IPA_DRIVER_WLAN3_STA_PKT_DROP: Packets originating from WLAN 3 STA path are dropping in IPA driver.
 *
 * Filtering rule validation errors:
 * @IPA_ETH_FILTER_RULE_INCORRECT: Ethernet filtering rule(s) incorrect / unexpected ordering.
 * @IPA_WLAN_AP_FILTER_RULE_INCORRECT: WLAN AP filtering rule(s) incorrect / unexpected ordering.
 * @IPA_WLAN_STA_FILTER_RULE_INCORRECT: WLAN STA filtering rule(s) incorrect / unexpected ordering.
 * @IPA_ETH1_FILTER_RULE_INCORRECT: Ethernet 1 filtering rule(s) incorrect / unexpected ordering.
 * @IPA_WLAN2_AP_FILTER_RULE_INCORRECT: WLAN 2 AP filtering rule(s) incorrect / unexpected ordering.
 * @IPA_WLAN2_STA_FILTER_RULE_INCORRECT: WLAN 2 STA filtering rule(s) incorrect / unexpected ordering.
 * @IPA_WLAN3_AP_FILTER_RULE_INCORRECT: WLAN 3 AP filtering rule(s) incorrect / unexpected ordering.
 * @IPA_WLAN3_STA_FILTER_RULE_INCORRECT: WLAN 3 STA filtering rule(s) incorrect / unexpected ordering.
 *
 * NAT initialization error:
 * @IPA_NAT_NOT_INITIALIZED: NAT table not initialized / NAT init validation failure.
 *
 * @IPA_HEALTH_MAX: max value (not an error code for reporting).
 */
enum ipa_rc_state_err
{
	IPA_HEALTH_OK  = 0x00,
	IPA_CHANNEL_ETH_NOT_STARTED  = 0x0001,
	IPA_CHANNEL_WLAN_NOT_STARTED =0x0002,
	IPA_CHANNEL_APPS_EMB_NO_BUFF =0x0004,
	IPA_CHANNEL_Q6_NO_BUFF =0x0008,
	IPA_CHANNEL_ETH_NO_BUFF =0x0010,
	IPA_CHANNEL_WLAN_NO_BUFF =0x0020,
	IPA_DRIVER_ETH_PKT_DROP =0x0040,
	IPA_DRIVER_WLAN_AP_PKT_DROP =0x0080,
	IPA_DRIVER_WLAN_STA_PKT_DROP =0x0100,
	IPA_ETH_FILTER_RULE_INCORRECT =0x0200,
	IPA_WLAN_AP_FILTER_RULE_INCORRECT =0x0400,
	IPA_WLAN_STA_FILTER_RULE_INCORRECT =0x0800,
	IPA_NAT_NOT_INITIALIZED =0x1000,
	IPA_CHANNEL_ETH1_NOT_STARTED =0x2000,
	IPA_CHANNEL_WLAN2_NOT_STARTED =0x4000,
	IPA_CHANNEL_WLAN3_NOT_STARTED =0x8000,
	IPA_CHANNEL_ETH1_NO_BUFF =0x10000,
	IPA_CHANNEL_WLAN2_NO_BUFF =0x20000,
	IPA_CHANNEL_WLAN3_NO_BUFF =0x40000,
	IPA_DRIVER_ETH1_PKT_DROP =0x80000,
	IPA_DRIVER_WLAN2_AP_PKT_DROP =0x100000,
	IPA_DRIVER_WLAN2_STA_PKT_DROP =0x200000,
	IPA_DRIVER_WLAN3_AP_PKT_DROP =0x400000,
	IPA_DRIVER_WLAN3_STA_PKT_DROP =0x800000,
	IPA_ETH1_FILTER_RULE_INCORRECT =0x1000000,
	IPA_WLAN2_AP_FILTER_RULE_INCORRECT =0x2000000,
	IPA_WLAN2_STA_FILTER_RULE_INCORRECT =0x4000000,
	IPA_WLAN3_AP_FILTER_RULE_INCORRECT =0x8000000,
	IPA_WLAN3_STA_FILTER_RULE_INCORRECT =0x10000000,
	IPA_DRIVER_MODEM_PKT_DROP = 0x20000000,
	IPA_RESERVE_ERROR_CODE_ICMP = 0x40000000,
	IPA_HEALTH_MAX = 0x80000000
};

/*
 * ipa_rc_clients client identifiers to be
 * tracked by IPA Health Monitor.
 *
 * @ETH: Ethernet client.
 * @WLAN: WLAN client.
 * @MODEM: Modem client.
 * @OTHERS: Other / miscellaneous clients.
 */
enum ipa_rc_clients
{
	ETH = 0,
	WLAN,
	MODEM,
	ETH1,
	WLAN2,
	WLAN3,
	OTHERS
};

extern const char * const ipa_rc_client_names[MAX_RC_CLIENTS];

/*
 * ipa_rc_flt_rule_grp filter rule group identifiers
 *
 * @DEFAULT_FLT_RULE: Default filter rule group.
 *
 * @LAN2LAN_FLT_RULE: LAN-to-LAN filter rule group.
 *
 * @PVT_SUBNET_FLT_RULE: Private subnet filter rule group.
 *                       Used for rules matching private subnet
 *                       address ranges.
 *
 * @MTU_FLT_RULE: MTU-related filter rule group.
 *
 * @ETH_UL_FLT_RULE: Ethernet uplink filter rule group.
 *
 * @ETH_DL_FLT_RULE: Ethernet downlink (WAN) filter rule group.
 *                   Used when Ethernet is configured as WAN.
 *                   Matches dst_addr/mask 0x0/0x0 with IPA_PASS_TO_DST_NAT.
 *
 * @WLAN_AP_UL_FLT_RULE: WLAN AP uplink filter rule group.
 *                       Used for WLAN Access Point uplink traffic
 *                       filtering.
 *
 * @WLAN_STA_DL_FLT_RULE: WLAN STA downlink filter rule group.
 *                        Used for WLAN Station downlink (DL)
 *                        traffic filtering.
 *
 * @MODEM_FLT_RULE: Modem filter rule group.
 *
 * @CATCHALL_DFLT_FLT_RULE: Catch-all default filter rule group.
 *                          Used as a fallback when traffic does not
 *                          match any specific filter group.
 *
 * @MAX_FLT_RULE_GRP: maximum filter rule group value.
 */
enum ipa_rc_flt_rule_grp
{
	DEFAULT_FLT_RULE = 0,
	LAN2LAN_FLT_RULE = DEFAULT_FLT_RULE,
	PVT_SUBNET_FLT_RULE,
	MTU_FLT_RULE,
	ETH_UL_FLT_RULE,
	ETH_DL_FLT_RULE,
	WLAN_AP_UL_FLT_RULE,
	WLAN_STA_DL_FLT_RULE,
	MODEM_FLT_RULE,
	CATCHALL_DFLT_FLT_RULE,
	MAX_FLT_RULE_GRP
};

/*
 * ipa_rc_instance instance identifiers
 *
 * @PREV: Previous instance.
 *        Represents the previously captured or stored instance.
 * @CUR: Current instance.
 *       Represents the currently latest instance.
 */
enum ipa_rc_instance
{
	PREV = 0,
	CUR  = 1
};

/*
 * ipa_rc_reg_monitor IPA register/state monitor
 *
 * @ipa_state: IPA global state register snapshot for IPA-RC.
 *             Captures the overall IPA HW state at the time of
 *             monitoring.
 *
 * @ipa_state_rx_active: IPA RX activity state.
 *                       Indicates whether RX path is active
 * @ipa_state_tx0: IPA TX 0 state.
 * @ipa_state_tx1: IPA TX 1 state.
 */
struct ipa_rc_reg_monitor
{
	uint32_t ipa_state;
	uint32_t ipa_state_rx_active;
	uint32_t ipa_state_tx0;
	uint32_t ipa_state_tx1;
};

/*
 * ipa_rc_last_cmd_pipe_info last cmd pipe information
 *
 * @pipe: Pipe identifier.
 *        Identifies the IPA pipe on which the last command
 *        was processed or observed.
 *
 * @engine: Engine identifier.
 *          Identifies the IPA engine associated with the
 *          last command.
 */
struct ipa_rc_last_cmd_pipe_info {
	u32 pipe;
	u8  engine;
};

/*
 * ipa_rc_health_monitor IPA health monitor context
 *
 * @reg: IPA register/state monitor snapshot.
 *       Captures IPA HW state information used for stall
 *       and health analysis.
 *
 * @last_cmd_pipe_info: Last command pipe information.
 *                      Identifies the IPA pipe and engine on which
 *                      the most recent command was observed.
 *
 * @drop_pkt_cnts: Packet drop counters.
 *                 Per-client and per-direction drop statistics,
 *                 indexed by [client][data direction].
 *
 * @is_filter_order_proper: Filter rule ordering status.
 *                          Indicates whether IPA filtering rules
 *                          are installed in the expected order.
 *
 * @is_nat_initialized: NAT initialization status.
 *                      Indicates whether the IPA NAT table has been
 *                      successfully initialized.
 *
 * @status_code: IPA health status code.
 *               Bitmask of ipa_rc_state_err values representing
 *               detected health or stall conditions.
 *
 * @node: List node.
 *        Used to link this structure into a list; must always
 *        remain as the last member of the structure.
 */
struct ipa_rc_health_monitor
{
	struct ipa_rc_reg_monitor reg;
	struct ipa_rc_last_cmd_pipe_info last_cmd_pipe_info;
	u32 drop_pkt_cnts[MAX_RC_CLIENTS][DATA_DIR];
	bool is_filter_order_proper;
	bool is_nat_initialized;
	enum ipa_rc_state_err status_code;
	/*always keep node in last*/
	struct list_head node;
};

/*
 * ipa_rc_queue IPA RC queue container
 *
 * @head: List head.
 *        Head of the linked list representing the queue elements.
 *
 * @size: Queue size.
 *        Number of elements currently present in the queue.
 *
 * @lock: Spinlock.
 *        Protects access to the queue (head and size) to ensure
 *        safe concurrent access.
 */
struct ipa_rc_queue {
	struct list_head head;
	size_t size;
	spinlock_t lock;
};

/*
 * ipa_rc_wlan_intf_info WLAN interface information for IPA RC
 *
 * @name: WLAN interface name.
 *        Stores the name of the WLAN network interface associated
 *        with IPA.
 *
 * @wlan_msg_type: WLAN message type.
 *                 Encodes the WLAN-specific message (AP/STA)
 *                 associated with this interface.
 *
 * @metadata: Metadata value.
 *            Metadata associated with the WLAN interface.
 *
 * @metadata_mask: Metadata mask.
 *                 Mask applied to metadata for matching or filtering
 *                 purposes.
 *
 * @link: List node.
 *        Used to link this WLAN interface information structure into
 *        a list.
 */
struct ipa_rc_wlan_intf_info {
	char name[IPA_RESOURCE_NAME_MAX];
	uint8_t wlan_msg_type;
	uint32_t metadata;
	uint32_t metadata_mask;
	struct list_head link;
};

/*
 * ipa_rc_wlan_info WLAN interface collection
 *
 * @head: List head.
 *        Head of the linked list containing WLAN interface
 *        information entries.
 *
 * @size: WLAN interface count.
 *        Number of WLAN interface entries currently present
 *        in the list.
 */
struct ipa_rc_wlan_info {
	struct list_head head;
	size_t size;
};

/*
 * ipa_rc_wq_ctx IPA RC workqueue context
 *
 * @rc_wq: IPA RC workqueue.
 *         Workqueue used to schedule and execute task
 *         for monitoring ipa robustness.
 *
 * @dwork: Delayed work item.
 *         Delayed work structure associated with the RC
 *         workqueue, used for time-based or deferred execution.
 *
 * @rc_lock: RC context mutex.
 *          Protects IPA RC context data accessed from
 *          workqueue and other execution paths.
 */
struct ipa_rc_wq_ctx {
	struct workqueue_struct *rc_wq;
	struct delayed_work dwork;
	struct mutex rc_lock;
};

/* IPA RC global context and state */
extern unsigned int query_timer;
extern struct ipa_rc_queue rc_list;
extern struct ipa_rc_wlan_info ipa_rc_wlan_info;
extern struct ipa_rc_wq_ctx *rc_ctx;

/* HM Utility functions */
int ipa_rc_monitor_health(void);
bool is_wlan_sta_pkt(struct ipahal_pkt_status *status);
int get_rc_client(int src_idx);

/**
 * ipa_rc_reset_drop_pkt_stats() - Reset drop detection state on IPACM restart.
 *
 * Clears all drop detection state including:
 *  - ipa3_ctx->stats.rx_excp_pkts[][EXCEPTION_DROP_UL/DL] for all clients
 *  - persistent drop latch, rolling sample window, and previous sample counters
 *
 * Must be called when IPACM restart is detected via ioctl.
 * This is the only recovery path — the drop detection algorithm does not self-heal.
 */
void ipa_rc_reset_drop_pkt_stats(void);

/* Queue functions */
void rc_list_init(struct ipa_rc_queue *q);
bool rc_list_is_empty(struct ipa_rc_queue *q);
bool rc_list_is_full(struct ipa_rc_queue *q);
size_t rc_list_size(struct ipa_rc_queue *q);
int rc_list_enqueue(struct ipa_rc_queue *q, struct ipa_rc_health_monitor *p);
void rc_list_dequeue(struct ipa_rc_queue *q);
void rc_list_clear(struct ipa_rc_queue *q);

/* Health Monitor Interfaces */
ssize_t status_show(struct device *dev, struct device_attribute *attr, char *ubuf);
ssize_t timer_val_store(struct device *dev, struct device_attribute *attr, const char *ubuf, size_t count);
ssize_t testcase_show(struct device *dev, struct device_attribute *attr, char *ubuf);
ssize_t testcase_store(struct device *dev, struct device_attribute *attr, const char *ubuf, size_t count);

#endif /* _IPA_RC_H_ */
