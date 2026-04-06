// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */


#include <linux/jhash.h>
#include <linux/msm_ipa.h>
#include <linux/atomic.h>
#include <linux/string.h>
#include <linux/netdevice.h>
#include <linux/vmalloc.h>
#include "ipa_api.h"
#include "ipa_be.h"
#include "ipa_be_clientdb.h"
#include "ipa_i.h"


#define ETH_IFACE_INDEX_LEN 10

/*
 * Random seed used during hash calculations
 */
uint32_t ipa_db_jhash_rnd __read_mostly;

ProcCtx *hash_table_ipa[TABLE_SIZE];
Hdr *hash_table_hdr[TABLE_SIZE];

/*
 * Mapping hash table
 */
#define IPA_DB_MAPPING_HASH_SLOTS 4096

static struct ipa_clientdb_mapping_instance **ipa_db_mapping_table;
static int *ipa_db_mapping_table_lengths;

static atomic_t ipa_client_db_mapping_count = ATOMIC_INIT(0);

/*
 * Global list.
 * All instances are inserted into global list - this allows easy iteration of all instances of a particular type.
 * The list is doubly linked for fast removal.  The list is in no particular order.
 */
struct ipa_clientdb_mapping_instance *ipa_db_mappings = NULL;

/*
 * Locking of the database - concurrency control
 *
 * Lock Hierarchy: Always acquire ipa_client_db_lock before ipa_hash_table_lock if both are needed.
 *
 * Both locks use mutexes to:
 * 1. Support sleeping operations (memory allocation with GFP_KERNEL, IPA API calls)
 * 2. Provide consistent locking primitive throughout the module
 * 3. Enable proper integration with kernel scheduling
 */
DEFINE_MUTEX(ipa_client_db_lock);					/* Protect the table from SMP access. */

/*
 * ClientDB Hash Table Lock - Protect hash_table_ipa and hash_table_hdr from concurrent access
 * Uses mutex to support sleeping operations in deletion functions (kzalloc with GFP_KERNEL, IPA API calls)
 */
static DEFINE_MUTEX(ipa_hash_table_lock);

static inline bool is_ip_addr_equal(const ip_addr_t a, const ip_addr_t b)
{
	return memcmp(a, b, sizeof(ip_addr_t)) == 0;
}

/*
 * ipa_be_is_zero_mac()
 *      Check if the mac address is all zero
 */
static inline bool ipa_be_is_zero_mac(const u8 *addr)
{
	return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
}

/*
 * ipa_clientdb_mapping_alloc()
 *	Allocate a mapping instance
 */
static struct ipa_clientdb_mapping_instance *
ipa_clientdb_mapping_alloc(void)
{
	struct ipa_clientdb_mapping_instance *mi = NULL;

	mi = (struct ipa_clientdb_mapping_instance *)kzalloc(sizeof(struct ipa_clientdb_mapping_instance), GFP_ATOMIC | __GFP_NOWARN);
	if (!mi) {
		IPA_BE_ERR("Alloc failed\n");
		return NULL;
	}

	atomic_inc(&ipa_client_db_mapping_count);
	IPA_BE_DBG("Mapping created %px\n", mi);
	return mi;
}

/*
 * ipa_db_mapping_generate_hash_index()
 * 	Calculate the hash index.
 */
static inline ipa_db_mapping_hash_t ipa_db_mapping_generate_hash_index(ip_addr_t address, uint32_t vlan)
{
	uint32_t tuple;
	uint32_t hash_val;

	IPA_IP_ADDR_HASH(tuple, address);
	IPA_BE_DBG("tuple: %d\n", tuple);
	hash_val = (uint32_t)jhash_1word(tuple, ipa_db_jhash_rnd);

	IPA_BE_DBG("hash_val: %d\n", hash_val);
	return (ipa_db_mapping_hash_t)(hash_val & (IPA_DB_MAPPING_HASH_SLOTS - 1));
}

/*
 * ipa_db_mapping_generate_hash_index()
 * 	Calculate the hash index.
 */
static inline ipa_db_mapping_hash_t ipa_proc_ctx_generate_hash_index(char* name)
{
	uint32_t tuple;
	uint32_t hash_val;

	if (!name) {
		IPA_BE_ERR("Invalid name parameter\n");
		return 0;
	}

	IPA_IP_STRING_HASH(tuple, name);
	hash_val = (uint32_t)jhash_1word(tuple, ipa_db_jhash_rnd);
	return (ipa_db_mapping_hash_t)(hash_val & (TABLE_SIZE - 1));
}

/*
 * ipa_be_clientdb_find_and_ref() - Find a mapping and increment its ref count.
 *
 * This function correctly handles hash collisions by iterating through the chain.
 */
bool ipa_be_clientdb_find_and_ref(ip_addr_t address, int vlan_id, bool lan2lan)
{
	ipa_db_mapping_hash_t hash_index;
	struct ipa_clientdb_mapping_instance *mi;

	hash_index = ipa_db_mapping_generate_hash_index(address, vlan_id);

	mutex_lock(&ipa_client_db_lock);
	/* Iterate through the hash chain to find the correct entry */
	for (mi = ipa_db_mapping_table[hash_index]; mi != NULL; mi = mi->hash_next) {
		if (is_ip_addr_equal(mi->address, address) && mi->vlan_id == vlan_id) {
			/* Found the correct mapping, increment ref count */
			if (lan2lan) {
				mi->lan2lan_info.ref_count++;
			} else {
				mi->lan2wan_info.ref_count++;
			}
			IPA_BE_DBG("Found existing mapping, ref_count now l2l: %d, l2w: %d\n",
				   mi->lan2lan_info.ref_count, mi->lan2wan_info.ref_count);
			mutex_unlock(&ipa_client_db_lock);
			return true;
		}
	}
	mutex_unlock(&ipa_client_db_lock);

	return false; /* Not found */
}
EXPORT_SYMBOL(ipa_be_clientdb_find_and_ref);

/*
 * ipa_db_mapping_add()
 *	Add a mapping instance into the database
 *
 * NOTE: The mapping will take a reference to the host instance.
 */
int ipa_client_db_mapping_add(struct ipa_clientdb_mapping_instance *mi, int vlan_id, ip_addr_t address,
						ipa_db_mapping_final_callback_t final, void *arg)
{
	ipa_db_mapping_hash_t hash_index;

	if (!mi) {
		IPA_BE_ERR("Invalid mi parameters\n");
		return -EINVAL;
	}

	/*
	 * Compute hash table position for insertion
	 */
	hash_index = ipa_db_mapping_generate_hash_index(address, vlan_id);
	mi->hash_index = hash_index;
	memcpy(mi->address, address, sizeof(ip_addr_t));
	IPA_BE_DBG("Adding this client info to hash_index %d " IPA_IP_ADDR_DOT_FMT " mi " IPA_IP_ADDR_DOT_FMT "\n",
		hash_index, IPA_IP_ADDR_TO_DOT(address), IPA_IP_ADDR_TO_DOT(mi->address));

	/*
	 * Save vlan id
	 */
	mi->vlan_id = vlan_id;

	mutex_lock(&ipa_client_db_lock);

	/*
	 * Add into the global list
	 */
	mi->prev = NULL;
	mi->next = ipa_db_mappings;
	if (ipa_db_mappings) {
		ipa_db_mappings->prev = mi;
	}
	ipa_db_mappings = mi;

	/*
	 * Insert mapping into the mappings hash table
	 */
	mi->hash_prev = NULL;
	mi->hash_next = ipa_db_mapping_table[hash_index];
	if (ipa_db_mapping_table[hash_index]) {
		ipa_db_mapping_table[hash_index]->hash_prev = mi;
	}
	ipa_db_mapping_table[hash_index] = mi;

	ipa_db_mapping_table_lengths[hash_index]++;
	IPA_BE_DBG(" table len %d, hash_index %d val %p\n",
		ipa_db_mapping_table_lengths[hash_index], hash_index, ipa_db_mapping_table[hash_index]);

	mutex_unlock(&ipa_client_db_lock);

	return 0;
}
EXPORT_SYMBOL(ipa_client_db_mapping_add);

/*
 * ipa_clientdb_mapping_init()
 */
bool ipa_clientdb_mapping_init(void)
{
	size_t size = sizeof(struct ipa_clientdb_mapping_instance *) *
			  IPA_DB_MAPPING_HASH_SLOTS;

	ipa_db_mapping_table = vzalloc(size);
	if (!ipa_db_mapping_table) {
		IPA_BE_ERR("Failed to allocate ipa_db_mapping_table\n");
		return false;
	}

	size = sizeof(int) * IPA_DB_MAPPING_HASH_SLOTS;
	ipa_db_mapping_table_lengths = vzalloc(size);
	if (!ipa_db_mapping_table_lengths) {
		IPA_BE_ERR("Failed to allocate ipa_db_mapping_table_lengths\n");
		vfree(ipa_db_mapping_table);
		ipa_db_mapping_table = NULL;
		return false;
	}

	IPA_BE_DBG("Client DB mapping initialized\n");
	return true;
}

/**
 * ipa_be_lookup_handle() - Looks up a handle by name in the proc context hash table.
 * @name: The name of the proc context to look up.
 *
 * Return: The handle if found, otherwise -1.
 */
int ipa_be_lookup_handle(char *name) {
	unsigned int index = ipa_proc_ctx_generate_hash_index(name);
	ProcCtx *entry = NULL;
	int handle = -1;

	mutex_lock(&ipa_hash_table_lock);
	entry = hash_table_ipa[index];
	while (entry) {
		if (strcmp(entry->name, name) == 0) {
			handle = entry->handle;
			break;
		}
		entry = entry->next;
	}
	mutex_unlock(&ipa_hash_table_lock);

	return handle;
}

/**
 * ipa_be_lookup_ref_count() - Looks up the reference count by name.
 * @name: The name of the proc context to look up.
 *
 * Return: The reference count if found, otherwise -1.
 */
int ipa_be_lookup_ref_count(char *name) {
	unsigned int index = ipa_proc_ctx_generate_hash_index(name);
	ProcCtx *entry = NULL;
	int ref_count = -1;

	mutex_lock(&ipa_hash_table_lock);
	entry = hash_table_ipa[index];
	while (entry) {
		if (strcmp(entry->name, name) == 0) {
			ref_count = entry->ref_count;
			break;
		}
		entry = entry->next;
	}
	mutex_unlock(&ipa_hash_table_lock);

	return ref_count;
}

/**
 * ipa_be_insert_proc_ctx() - Inserts or updates a proc context.
 * @name: The name of the proc context.
 * @handle: The handle to associate with the proc context.
 *
 * This function inserts a new proc context or increments the reference count
 * of an existing one.
 */
void ipa_be_insert_proc_ctx(char *name, int handle) {
	unsigned int index = ipa_proc_ctx_generate_hash_index(name);
	ProcCtx *entry = NULL;
	ProcCtx *new_ctx = NULL;

	mutex_lock(&ipa_hash_table_lock);

	entry = hash_table_ipa[index];
	// Check if the entry already exists
	while (entry) {
		if (strcmp(entry->name, name) == 0) {
			// If it exists, increment the ref_count
			entry->ref_count += 1;
			mutex_unlock(&ipa_hash_table_lock);
			return;
		}
		entry = entry->next;
	}

	// If not found, create a new entry with ref_count = 1
	new_ctx = kzalloc(sizeof(ProcCtx), GFP_KERNEL);
	if (!new_ctx) {
		mutex_unlock(&ipa_hash_table_lock);
		return;
	}
	memcpy(new_ctx->name, name, 32);
	new_ctx->handle = handle;
	new_ctx->ref_count = 1;
	new_ctx->next = hash_table_ipa[index];
	hash_table_ipa[index] = new_ctx;

	mutex_unlock(&ipa_hash_table_lock);
}

/**
 * ipa_hdr_generate_hash_index() - Generate hash index for header lookup
 * @mac: MAC address
 * @vlan_tag: VLAN tag
 *
 * Return: Hash index
 */
static inline unsigned int ipa_hdr_generate_hash_index(mac_addr_t mac, uint32_t vlan_tag)
{
	uint32_t hash_val;
	uint32_t mac_hash = 0;
	int i;

	/* Create hash from MAC address */
	for (i = 0; i < IPA_MAC_ADDR_SIZE; i++) {
		mac_hash = (mac_hash << 8) | mac[i];
	}

	/* Combine MAC hash with VLAN tag */
	hash_val = jhash_2words(mac_hash, vlan_tag, ipa_db_jhash_rnd);
	return (unsigned int)(hash_val & (TABLE_SIZE - 1));
}

/**
 * ipa_be_lookup_hdr_handle() - Looks up a header handle by MAC and VLAN
 * @mac: MAC address
 * @vlan_tag: VLAN tag
 *
 * Return: The handle if found, otherwise -1
 */
static int ipa_be_lookup_hdr_handle(mac_addr_t mac, uint32_t vlan_tag, enum ipa_ip_type ip_type)
{
	unsigned int index = ipa_hdr_generate_hash_index(mac, vlan_tag);
	Hdr *entry = NULL;
	int handle = -1;

	mutex_lock(&ipa_hash_table_lock);
	entry = hash_table_hdr[index];
	while (entry) {
		if (memcmp(entry->mac, mac, IPA_MAC_ADDR_SIZE) == 0 &&
		    entry->vlan_tag == vlan_tag &&
		    entry->ip_type == ip_type) {
			handle = entry->handle;
			break;
		}
		entry = entry->next;
	}
	mutex_unlock(&ipa_hash_table_lock);

	return handle;
}

/**
 * ipa_be_insert_hdr() - Inserts or updates a header context
 * @mac: MAC address
 * @vlan_tag: VLAN tag
 * @handle: The handle to associate with the header
 *
 * This function inserts a new header context or increments the reference count
 * of an existing one.
 */
static void ipa_be_insert_hdr(mac_addr_t mac, uint32_t vlan_tag, enum ipa_ip_type ip_type, int handle)
{
	unsigned int index = ipa_hdr_generate_hash_index(mac, vlan_tag);
	Hdr *entry = NULL;
	Hdr *new_hdr = NULL;

	mutex_lock(&ipa_hash_table_lock);

	entry = hash_table_hdr[index];
	/* Check if the entry already exists */
	while (entry) {
		if (memcmp(entry->mac, mac, IPA_MAC_ADDR_SIZE) == 0 &&
		    entry->vlan_tag == vlan_tag &&
		    entry->ip_type == ip_type) {
			/* If it exists, increment the ref_count */
			entry->ref_count++;
			IPA_BE_DBG("Incremented hdr ref_count for MAC %pM VLAN 0x%x ip_type %d to %d\n",
				   mac, vlan_tag, ip_type, entry->ref_count);
			mutex_unlock(&ipa_hash_table_lock);
			return;
		}
		entry = entry->next;
	}

	/* If not found, create a new entry with ref_count = 1 */
	new_hdr = kzalloc(sizeof(Hdr), GFP_KERNEL);
	if (!new_hdr) {
		IPA_BE_ERR("Failed to allocate Hdr\n");
		mutex_unlock(&ipa_hash_table_lock);
		return;
	}

	memcpy(new_hdr->mac, mac, IPA_MAC_ADDR_SIZE);
	new_hdr->vlan_tag = vlan_tag;
	new_hdr->ip_type = ip_type;
	new_hdr->handle = handle;
	new_hdr->ref_count = 1;
	new_hdr->next = hash_table_hdr[index];
	hash_table_hdr[index] = new_hdr;

	IPA_BE_DBG("Inserted new hdr for MAC %pM VLAN 0x%x ip_type %d handle %d\n",
		   mac, vlan_tag, ip_type, handle);

	mutex_unlock(&ipa_hash_table_lock);
}

/**
 * ipa_be_delete_hdr_by_handle() - Decrements reference count and deletes header if needed
 * @hdr_hdl: Header handle
 *
 * This function decrements the reference count of a header entry by searching
 * for it using the header handle. If the reference count reaches 0, it deletes
 * the header from IPA and removes the entry from the hash table.
 *
 * This allows deletion without needing the MAC address or VLAN tag.
 *
 * Return: 0 on success, negative errno on failure
 */
int ipa_be_delete_hdr_by_handle(int hdr_hdl)
{
	unsigned int index;
	Hdr *entry = NULL;
	Hdr *prev = NULL;
	int ret = 0;

	if (hdr_hdl <= 0) {
		IPA_BE_DBG("Invalid header handle %d\n", hdr_hdl);
		return 0; /* Not an error, just nothing to delete */
	}

	mutex_lock(&ipa_hash_table_lock);

	/* Search all hash table entries to find the one with matching handle */
	for (index = 0; index < TABLE_SIZE; index++) {
		prev = NULL;
		entry = hash_table_hdr[index];

		while (entry) {
			if (entry->handle == hdr_hdl) {
				/* Found the entry, decrement reference count */
				entry->ref_count--;
				IPA_BE_DBG("Decremented hdr ref_count for handle %d (MAC %pM VLAN 0x%x) to %d\n",
					   hdr_hdl, entry->mac, entry->vlan_tag, entry->ref_count);

				/* If reference count reaches 0, delete the header */
				if (entry->ref_count <= 0) {
					/* Delete header from IPA */
					ret = ipa_be_delete_hdr_handle(entry->handle);

					/* Remove from hash table */
					if (prev) {
						prev->next = entry->next;
					} else {
						hash_table_hdr[index] = entry->next;
					}

					IPA_BE_DBG("Removed hdr entry for handle %d (MAC %pM VLAN 0x%x) from hash table\n",
						   hdr_hdl, entry->mac, entry->vlan_tag);
					kfree(entry);
				}
				mutex_unlock(&ipa_hash_table_lock);
				return ret;
			}
			prev = entry;
			entry = entry->next;
		}
	}

	mutex_unlock(&ipa_hash_table_lock);
	IPA_BE_DBG("Hdr entry with handle %d not found\n", hdr_hdl);
	return -ENOENT;
}

struct ipa_clientdb_mapping_instance *ipa_be_client_mapping_add_or_ref(ip_addr_t addr, int vlan_id, int lan2lan)
{
	struct ipa_clientdb_mapping_instance *nmi = NULL;

	IPA_BE_DBG("Establish mapping for " IPA_IP_ADDR_DOT_FMT " vlan :%d\n", IPA_IP_ADDR_TO_DOT(addr), vlan_id);

	if (ipa_be_clientdb_find_and_ref(addr, vlan_id, lan2lan))
	{
		IPA_BE_DBG("Client dst ip already exists,, increase ref \n");
		return nmi;
	}

	/*
	 * Create mapping instance and add a new entry
	 */
	nmi = ipa_clientdb_mapping_alloc();
	if (!nmi) {
		IPA_BE_ERR("Failed to establish mapping done\n");
		return NULL;
	}

	// Ensure address is stored in struct before adding
	memcpy(nmi->address, addr, sizeof(ip_addr_t));
	nmi->vlan_id = vlan_id;

	ipa_client_db_mapping_add(nmi, vlan_id,  addr,
							   NULL, NULL);

	IPA_BE_DBG("%px: mapping established " IPA_IP_ADDR_DOT_FMT " vlan :%d\n", nmi, IPA_IP_ADDR_TO_DOT(nmi->address), vlan_id);
	return nmi;
}

int ipa_be_mapping_deref_and_delete(ip_addr_t addr, bool lan2lan)
{
	ipa_db_mapping_hash_t hash_index;
	struct ipa_clientdb_mapping_instance *mi;
	int ref_count = -1;

	IPA_BE_DBG("Deleting mapping for " IPA_IP_ADDR_DOT_FMT "\n", IPA_IP_ADDR_TO_DOT(addr));
	hash_index = ipa_db_mapping_generate_hash_index(addr, 0);

	mutex_lock(&ipa_client_db_lock);

	for (mi = ipa_db_mapping_table[hash_index]; mi != NULL; mi = mi->hash_next) {
		if (is_ip_addr_equal(mi->address, addr)) {
			if (lan2lan) {
				if (mi->lan2lan_info.ref_count > 0)
					mi->lan2lan_info.ref_count--;
			}
			else {
				if (mi->lan2wan_info.ref_count > 0)
				mi->lan2wan_info.ref_count--;
			}

			IPA_BE_DBG("Ref count now l2l: %d, l2w: %d\n", mi->lan2lan_info.ref_count, mi->lan2wan_info.ref_count);
			ref_count = mi->lan2lan_info.ref_count + mi->lan2wan_info.ref_count;

			if (ref_count == 0) {
				// Remove from hash table
				if (mi->hash_prev)
					mi->hash_prev->hash_next = mi->hash_next;
				else
					ipa_db_mapping_table[hash_index] = mi->hash_next;

				if (mi->hash_next)
					mi->hash_next->hash_prev = mi->hash_prev;

				ipa_db_mapping_table_lengths[hash_index]--;

				// Remove from global list
				if (mi->prev)
					mi->prev->next = mi->next;
				else
					ipa_db_mappings = mi->next;

				if (mi->next)
					mi->next->prev = mi->prev;

				kfree(mi);
				atomic_dec(&ipa_client_db_mapping_count);

				IPA_BE_DBG("Deleted Client Db at hash index %d mapping count %d now\n", hash_index, atomic_read(&ipa_client_db_mapping_count));
				ref_count = 0;
			}

			break;
		}
	}

	mutex_unlock(&ipa_client_db_lock);
	return ref_count;
}

static int ipa_ipv4_header(struct ipa_ipv4_rule_create_msg v4_msg)
{
	int handle = 0;
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;
	int size = 0;

	IPA_BE_DBG("Entry\n");
	size = sizeof(struct ipa_ioc_add_hdr) + (1 * sizeof(struct ipa_hdr_add));
	pHeaderDescriptor = (struct ipa_ioc_add_hdr *)kzalloc(size, GFP_KERNEL);
	if (pHeaderDescriptor == NULL)
	{
		IPA_BE_ERR("calloc failed to allocate pHeaderDescriptor\n");
		return -EFAULT;
	}

	pHeaderDescriptor->commit = true;
	pHeaderDescriptor->num_hdrs = 1;

	memset(pHeaderDescriptor->hdr[0].name, 0,
		 sizeof(pHeaderDescriptor->hdr[0].name));

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(pHeaderDescriptor->hdr[0].name, V4_LAN_ROUTE_TABLE_NAME, sizeof(pHeaderDescriptor->hdr[0].name));
#else
	strlcpy(pHeaderDescriptor->hdr[0].name, V4_LAN_ROUTE_TABLE_NAME, sizeof(pHeaderDescriptor->hdr[0].name));
#endif
	pHeaderDescriptor->hdr[0].name[IPA_RESOURCE_NAME_MAX-1] = '\0';

	pHeaderDescriptor->hdr[0].hdr_len = 14;
	pHeaderDescriptor->hdr[0].hdr_hdl = -1;
	pHeaderDescriptor->hdr[0].is_partial = 0;
	pHeaderDescriptor->hdr[0].status = -1;

	if (ipa3_add_hdr_usr(
			(struct ipa_ioc_add_hdr *)pHeaderDescriptor, true)) {
			handle = -EFAULT;
			kfree(pHeaderDescriptor);
			return handle;
	}

	handle = pHeaderDescriptor->hdr[0].hdr_hdl;
	IPA_BE_DBG("Installed hdr proc ctx: hdl %d\n", handle);

	kfree(pHeaderDescriptor);
	return handle;
}

static int ipa_ipv4_vlan_header(
	struct ipa_ipv4_rule_create_msg v4_msg,
	int vlan_id,
	char *name)
{
	int handle = 0;
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;
	struct ipa_ioc_copy_hdr sCopyHeader = {0};
	int size = 0;
	int hdr_len = 18;

	if (!name) {
		IPA_BE_ERR("Invalid name parameter\n");
		return -EINVAL;
	}

	IPA_BE_DBG("Entry\n");
	size = sizeof(struct ipa_ioc_add_hdr) + (1 * sizeof(struct ipa_hdr_add));
	pHeaderDescriptor = (struct ipa_ioc_add_hdr *)kzalloc(size, GFP_KERNEL);
	if (pHeaderDescriptor == NULL)
	{
		IPA_BE_ERR("calloc failed to allocate pHeaderDescriptor\n");
		return -EFAULT;
	}

	pHeaderDescriptor->commit = true;
	pHeaderDescriptor->num_hdrs = 1;

	memcpy(&sCopyHeader.name, name,
		 sizeof(sCopyHeader.name));

	if (ipa3_copy_hdr((struct ipa_ioc_copy_hdr *)&sCopyHeader)) {
			IPA_BE_ERR("Copy header failed\n");
			handle = -EFAULT;
			kfree(pHeaderDescriptor);
			return handle;
	}
	IPA_BE_DBG("header length: %d, partial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
	hdr_len = sCopyHeader.hdr_len;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(pHeaderDescriptor->hdr[0].name, V4_LAN_ROUTE_TABLE_NAME, sizeof(pHeaderDescriptor->hdr[0].name));
#else
	strlcpy(pHeaderDescriptor->hdr[0].name, V4_LAN_ROUTE_TABLE_NAME, sizeof(pHeaderDescriptor->hdr[0].name));
#endif

	pHeaderDescriptor->hdr[0].name[IPA_RESOURCE_NAME_MAX-1] = '\0';

	memcpy(pHeaderDescriptor->hdr[0].hdr,
					sCopyHeader.hdr,
					pHeaderDescriptor->hdr[0].hdr_len);

	pHeaderDescriptor->hdr[0].hdr_len = hdr_len;
	pHeaderDescriptor->hdr[0].type = sCopyHeader.type;
	pHeaderDescriptor->hdr[0].hdr_hdl = -1;
	pHeaderDescriptor->hdr[0].is_partial = sCopyHeader.is_partial;
	pHeaderDescriptor->hdr[0].status = -1;

	pHeaderDescriptor->hdr[0].hdr[hdr_len - 3] = (uint8_t)vlan_id & 0xFF;
	pHeaderDescriptor->hdr[0].hdr[hdr_len - 4] = (uint8_t)(vlan_id >> 8) & 0xFF;
	memset(pHeaderDescriptor->hdr[0].name, 0,
					 sizeof(pHeaderDescriptor->hdr[0].name));
	snprintf(pHeaderDescriptor->hdr[0].name, sizeof(pHeaderDescriptor->hdr[0].name),
		"ath12_ipv4_vlan%d", vlan_id);

	if (ipa3_add_hdr_usr(
			(struct ipa_ioc_add_hdr *)pHeaderDescriptor, true)) {
			handle = -EFAULT;
			kfree(pHeaderDescriptor);
			return handle;
	}

	handle = pHeaderDescriptor->hdr[0].hdr_hdl;
	IPA_BE_DBG("Installed hdr proc ctx: hdl %d\n", handle);

	kfree(pHeaderDescriptor);
	return handle;
}


/**
 * get_bridge_mac - Copy MAC address of interface into caller's buffer
 * @ifindex: Interface index (from ip link show)
 * @mac:     Pointer to caller's buffer (mac_addr_t)
 *
 * Returns 0 on success, negative errno on failure.
 */
static int get_bridge_mac(int ifindex, mac_addr_t mac)
{
	struct net_device *dev;
	int ret = 0;

	dev = dev_get_by_index(&init_net, ifindex);
	if (!dev)
		return -ENODEV;

	if (!dev->dev_addr) {
		ret = -EADDRNOTAVAIL;
		goto out;
	}

	memcpy(mac, dev->dev_addr, ETH_ALEN);

out:
	dev_put(dev);
	return ret;
}


static int ipa_eth_hdr_init(mac_addr_t client_mac, char *name, enum ipa_ip_type ip_type,
		int flow_interface_num, int return_interface_num, 
		int flow_top_interface_num, int return_top_interface_num, uint32_t vlan_tag)
{
	int handle = 0, len, res = 0;
	static int header_name_count;
	char index[ETH_IFACE_INDEX_LEN];
	struct ipa_ioc_copy_hdr sCopyHeader = {0};
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;
	int bridge_if_num = 0, client_if_num = 0;
	mac_addr_t bridge_mac = {0};
	const char *hdr_name_suffix;

	IPA_BE_DBG("Received Client MAC %pM\n", client_mac);

	if (ipa_be_is_zero_mac(client_mac)) {
		IPA_BE_ERR("Invalid client MAC\n");
		return -EINVAL;
	}

	/* Check if header already exists for this MAC, VLAN and IP type combination */
	handle = ipa_be_lookup_hdr_handle(client_mac, vlan_tag, ip_type);
	if (handle != -1) {
		/* Header exists, increment reference count and return existing handle */
		ipa_be_insert_hdr(client_mac, vlan_tag, ip_type, handle);
		IPA_BE_DBG("Reusing existing header handle %d for MAC %pM VLAN 0x%x ip_type %d\n",
			   handle, client_mac, vlan_tag, ip_type);
		return handle;
	}

	if (flow_interface_num == flow_top_interface_num) {
		bridge_if_num = return_top_interface_num;
		client_if_num = return_interface_num;
	} else if (return_interface_num == return_top_interface_num) {
		bridge_if_num = flow_top_interface_num;
		client_if_num = flow_interface_num;
	}
	IPA_BE_DBG("Bridge iface number %d client_if_num %d\n",
		   bridge_if_num, client_if_num);

	if (get_bridge_mac(bridge_if_num, bridge_mac) == 0)
		IPA_BE_DBG("Bridge MAC: %pM\n", bridge_mac);
	else
		IPA_BE_DBG("Failed to get bridge MAC for ifindex %d\n",
			   bridge_if_num);

	memcpy(sCopyHeader.name, name, sizeof(sCopyHeader.name));

	if (ipa3_copy_hdr(&sCopyHeader)) {
		IPA_BE_ERR("Copy header failed\n");
		return -EFAULT;
	}

	len = sizeof(struct ipa_ioc_add_hdr) + sizeof(struct ipa_hdr_add);
	pHeaderDescriptor = kzalloc(len, GFP_KERNEL);
	if (!pHeaderDescriptor) {
		IPA_BE_ERR("Failed to allocate pHeaderDescriptor\n");
		return -ENOMEM;
	}

	IPA_BE_DBG("header length: %d, partial: %d\n",
		   sCopyHeader.hdr_len, sCopyHeader.is_partial);
	IPA_BE_DBG("header eth2_ofst_valid: %d, eth2_ofst: %d\n",
		   sCopyHeader.is_eth2_ofst_valid, sCopyHeader.eth2_ofst);

	if (sCopyHeader.hdr_len > IPA_HDR_MAX_SIZE) {
		IPA_BE_ERR("Header oversize\n");
		res = -EFAULT;
		goto fail;
	}
	memcpy(pHeaderDescriptor->hdr[0].hdr, sCopyHeader.hdr,
	       sCopyHeader.hdr_len);

	if (sCopyHeader.is_eth2_ofst_valid)
		memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst],
		       client_mac, IPA_MAC_ADDR_SIZE);

	if (!ipa_be_is_zero_mac(bridge_mac)) {
		memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst +
						      IPA_MAC_ADDR_SIZE],
		       bridge_mac, IPA_MAC_ADDR_SIZE);
		IPA_BE_DBG("Device is in bridge mode %pM\n", bridge_mac);
	}

	/* Handle VLAN header if VLAN tag is provided */
	if (vlan_tag != 0 && vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED && sCopyHeader.is_eth2_ofst_valid) {
		uint16_t vlan_tci;

		/* Extract TCI from the complete vlan_tag (lower 16 bits) */
		vlan_tci = (uint16_t)(vlan_tag & 0xFFFF);

		/* Convert vlan_tci to network byte order */
		vlan_tci = htons(vlan_tci);

		/* Copy the VLAN TCI to the header at the correct position (after TPID) */
		memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst + 
			2 * IPA_MAC_ADDR_SIZE + 2], /* +2 for TPID size */
			&vlan_tci,
			sizeof(vlan_tci));

		IPA_BE_DBG("Updated VLAN TCI to 0x%04x, complete VLAN tag is 0x%08x\n", 
			   ntohs(vlan_tci), vlan_tag);
	}

	pHeaderDescriptor->commit = true;
	pHeaderDescriptor->num_hdrs = 1;

	snprintf(index, sizeof(index), "%d_", client_if_num);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(pHeaderDescriptor->hdr[0].name, index,
		sizeof(pHeaderDescriptor->hdr[0].name));
#else
	strlcpy(pHeaderDescriptor->hdr[0].name, index,
		sizeof(pHeaderDescriptor->hdr[0].name));
#endif
	pHeaderDescriptor->hdr[0].name[IPA_RESOURCE_NAME_MAX-1] = '\0';

	/* Select appropriate header name suffix based on IP type */
	hdr_name_suffix = (ip_type == IPA_IP_v4) ? IPA_ETH_HDR_NAME_v4 : IPA_ETH_HDR_NAME_v6;

	if (strlcat(pHeaderDescriptor->hdr[0].name, hdr_name_suffix,
		    sizeof(pHeaderDescriptor->hdr[0].name)) >=
	    IPA_RESOURCE_NAME_MAX) {
		IPA_BE_ERR("Header name construction failed, exceed length (%zu)\n",
			   strlen(pHeaderDescriptor->hdr[0].name));
		res = -EFAULT;
		goto fail;
	}
	/* Add VLAN ID to header name if VLAN tag is provided */
	if (vlan_tag != 0 && vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED) {
		uint16_t vlan_id = vlan_tag & 0x0FFF;  /* Extract VLAN ID from lower 12 bits */
		snprintf(index, sizeof(index), "_%d", vlan_id);
		if (strlcat(pHeaderDescriptor->hdr[0].name, index,
			    sizeof(pHeaderDescriptor->hdr[0].name)) >=
		    IPA_RESOURCE_NAME_MAX) {
			IPA_BE_ERR("Header name construction failed, exceed length (%zu)\n",
				   strlen(pHeaderDescriptor->hdr[0].name));
			res = -EFAULT;
			goto fail;
		}
	}

	header_name_count++;
	snprintf(index, sizeof(index), "_%d", header_name_count);
	if (strlcat(pHeaderDescriptor->hdr[0].name, index,
		    sizeof(pHeaderDescriptor->hdr[0].name)) >=
	    IPA_RESOURCE_NAME_MAX) {
		IPA_BE_ERR("Header name construction failed, exceed length (%zu)\n",
			   strlen(pHeaderDescriptor->hdr[0].name));
		res = -EFAULT;
		goto fail;
	}

	pHeaderDescriptor->hdr[0].hdr_len = sCopyHeader.hdr_len;
	pHeaderDescriptor->hdr[0].type = sCopyHeader.type;
	pHeaderDescriptor->hdr[0].hdr_hdl = -1;
	pHeaderDescriptor->hdr[0].is_partial = 0;
	pHeaderDescriptor->hdr[0].status = -1;

	if (ipa3_add_hdr_usr(pHeaderDescriptor, true)) {
		res = -EFAULT;
		goto fail;
	}

	handle = pHeaderDescriptor->hdr[0].hdr_hdl;
	IPA_BE_DBG("Installed wan hdr hdl %d\n", handle);

	/* Insert the new header into hash table with ref_count = 1 */
	ipa_be_insert_hdr(client_mac, vlan_tag, ip_type, handle);

fail:
	kfree(pHeaderDescriptor);
	return res ? res : handle;
}

static int ipa_ipv6_vlan_header(
	struct ipa_ipv6_rule_create_msg v6_msg,
	int vlan_id,
	char *name)
{
	int handle = 0;
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;
	struct ipa_ioc_copy_hdr sCopyHeader = {0};
	int size = 0;
	int hdr_len = 18;

	if (!name) {
		IPA_BE_ERR("Invalid name parameter\n");
		return -EINVAL;
	}

	IPA_BE_DBG("Entry\n");
	size = sizeof(struct ipa_ioc_add_hdr) + (1 * sizeof(struct ipa_hdr_add));
	pHeaderDescriptor = (struct ipa_ioc_add_hdr *)kzalloc(size, GFP_KERNEL);
	if (pHeaderDescriptor == NULL)
	{
		IPA_BE_ERR("calloc failed to allocate pHeaderDescriptor\n");
		return -EFAULT;
	}

	pHeaderDescriptor->commit = true;
	pHeaderDescriptor->num_hdrs = 1;

	memcpy(&sCopyHeader.name, name,
		 sizeof(sCopyHeader.name));

	if (ipa3_copy_hdr((struct ipa_ioc_copy_hdr *)&sCopyHeader)) {
			IPA_BE_ERR("Copy header failed\n");
			handle = -EFAULT;
			kfree(pHeaderDescriptor);
			return handle;
	}
	IPA_BE_DBG("header length: %d, partial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
	hdr_len = sCopyHeader.hdr_len;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(pHeaderDescriptor->hdr[0].name, V4_LAN_ROUTE_TABLE_NAME, sizeof(pHeaderDescriptor->hdr[0].name));
#else
	strlcpy(pHeaderDescriptor->hdr[0].name, V4_LAN_ROUTE_TABLE_NAME, sizeof(pHeaderDescriptor->hdr[0].name));
#endif

	pHeaderDescriptor->hdr[0].name[IPA_RESOURCE_NAME_MAX-1] = '\0';

	memcpy(pHeaderDescriptor->hdr[0].hdr,
					sCopyHeader.hdr,
					pHeaderDescriptor->hdr[0].hdr_len);

	pHeaderDescriptor->hdr[0].hdr_len = hdr_len;
	pHeaderDescriptor->hdr[0].type = sCopyHeader.type;
	pHeaderDescriptor->hdr[0].hdr_hdl = -1;
	pHeaderDescriptor->hdr[0].is_partial = sCopyHeader.is_partial;
	pHeaderDescriptor->hdr[0].status = -1;

	pHeaderDescriptor->hdr[0].hdr[hdr_len - 3] = (uint8_t)vlan_id & 0xFF;
	pHeaderDescriptor->hdr[0].hdr[hdr_len - 4] = (uint8_t)(vlan_id >> 8) & 0xFF;
	memset(pHeaderDescriptor->hdr[0].name, 0,
					 sizeof(pHeaderDescriptor->hdr[0].name));
	snprintf(pHeaderDescriptor->hdr[0].name, sizeof(pHeaderDescriptor->hdr[0].name),
		"ath12_ipv6_vlan%d", vlan_id);

	if (ipa3_add_hdr_usr(
			(struct ipa_ioc_add_hdr *)pHeaderDescriptor, true)) {
			handle = -EFAULT;
			kfree(pHeaderDescriptor);
			return handle;
	}

	handle = pHeaderDescriptor->hdr[0].hdr_hdl;
	IPA_BE_DBG("Installed hdr proc ctx: hdl %d\n", handle);

	kfree(pHeaderDescriptor);
	return handle;
}

static int ipa_ipv6_header(struct ipa_ipv6_rule_create_msg v6_msg)
{
	int handle = 0;
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;
	int size = 0;

	size = sizeof(struct ipa_ioc_add_hdr) + (1 * sizeof(struct ipa_hdr_add));
	pHeaderDescriptor = (struct ipa_ioc_add_hdr *)kzalloc(size, GFP_KERNEL);
	if (pHeaderDescriptor == NULL)
	{
		IPA_BE_ERR("calloc failed to allocate pHeaderDescriptor\n");
		return -EFAULT;
	}

	pHeaderDescriptor->commit = true;
	pHeaderDescriptor->num_hdrs = 1;

	memset(pHeaderDescriptor->hdr[0].name, 0,
		 sizeof(pHeaderDescriptor->hdr[0].name));

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
	strscpy(pHeaderDescriptor->hdr[0].name, V4_LAN_ROUTE_TABLE_NAME, sizeof(pHeaderDescriptor->hdr[0].name));
#else
	strlcpy(pHeaderDescriptor->hdr[0].name, V4_LAN_ROUTE_TABLE_NAME, sizeof(pHeaderDescriptor->hdr[0].name));
#endif
	pHeaderDescriptor->hdr[0].name[IPA_RESOURCE_NAME_MAX-1] = '\0';

	pHeaderDescriptor->hdr[0].hdr_len = 14;
	pHeaderDescriptor->hdr[0].hdr_hdl = -1;
	pHeaderDescriptor->hdr[0].is_partial = 0;
	pHeaderDescriptor->hdr[0].status = -1;

	if (ipa3_add_hdr_usr(
			(struct ipa_ioc_add_hdr *)pHeaderDescriptor, true)) {
			handle = -EFAULT;
			kfree(pHeaderDescriptor);
			return handle;
	}


	handle = pHeaderDescriptor->hdr[0].hdr_hdl;
	IPA_BE_DBG("Installed hdr proc ctx: hdl %d\n", handle);


	kfree(pHeaderDescriptor);
	return handle;
}


static int ipa_ipv4_header_proc_ctx(
	struct ipa_ipv4_rule_create_msg v4_msg,
	int* hdr_hdl,
	char *name,
	char *proc_ctx_name_out)
{
	int handle = 0;
	int egress_vlan_id = 0, ingress_vlan_id = 0;
	char client_mac_vlan_str[32];
	struct ipa_ioc_add_hdr_proc_ctx *hdr_proc_ctx_table;
	struct ipa_hdr_proc_ctx_add *hdr_proc_ctx;
	int size = 0;

	IPA_BE_DBG("ECMIPA entry ipa_ipv4_header_proc_ctx \n");
	/* vlan is the last 12 bits of vlan tag*/
	if (v4_msg.vlan_primary_rule.egress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED)
	{
		egress_vlan_id = v4_msg.vlan_primary_rule.egress_vlan_tag & 0x3FF;
	}
	if (v4_msg.vlan_primary_rule.ingress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED)
	{
		ingress_vlan_id = v4_msg.vlan_primary_rule.ingress_vlan_tag & 0x3FF;
	}
	IPA_BE_DBG("ECMIPA egress_vlan_id 0x%x  ingress_vlan_id 0x%x hdr_name %s\n", egress_vlan_id, ingress_vlan_id, name);

	snprintf(client_mac_vlan_str, sizeof(client_mac_vlan_str), "%02x%02x%02x%02x%02x%02x_%d",
			v4_msg.conn_rule.flow_mac[0], v4_msg.conn_rule.flow_mac[1],
			v4_msg.conn_rule.flow_mac[2], v4_msg.conn_rule.flow_mac[3],
			v4_msg.conn_rule.flow_mac[4], v4_msg.conn_rule.flow_mac[5],
			egress_vlan_id);

	/* Store the proc_ctx name for later cleanup */
	if (proc_ctx_name_out) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(proc_ctx_name_out, client_mac_vlan_str, 32);
#else
		strlcpy(proc_ctx_name_out, client_mac_vlan_str, 32);
#endif

	}

	handle = ipa_be_lookup_handle(client_mac_vlan_str);
	if (-1 != handle)
	{
		// connection for this client exists, only increase ref count
		ipa_be_insert_proc_ctx(client_mac_vlan_str, handle);
		return handle;
	}

	if (egress_vlan_id)
	{
		*hdr_hdl = ipa_ipv4_vlan_header(v4_msg,  egress_vlan_id, name);
	}
	else
	{
		*hdr_hdl = ipa_ipv4_header(v4_msg);
	}

	size = sizeof(struct ipa_ioc_add_hdr_proc_ctx) + sizeof(struct ipa_hdr_proc_ctx_add);
	hdr_proc_ctx_table = (struct ipa_ioc_add_hdr_proc_ctx*)kzalloc(size, GFP_KERNEL);
	if(hdr_proc_ctx_table == NULL)
	{
		IPA_BE_ERR("Failed to allocate memory.\n");
		return IPA_TX_FAILURE;
	}
	memset(hdr_proc_ctx_table, 0, size);

	hdr_proc_ctx_table->commit = 1;
	hdr_proc_ctx_table->num_proc_ctxs = 1;
	hdr_proc_ctx = &hdr_proc_ctx_table->proc_ctx[0];

	hdr_proc_ctx->type = IPA_HDR_PROC_ETHII_TO_ETHII_EX_DST;
	hdr_proc_ctx->hdr_hdl = *hdr_hdl;

	if (egress_vlan_id)
	{
		hdr_proc_ctx->generic_params.output_ethhdr_negative_offset = 18;
	}
	else
	{
		hdr_proc_ctx->generic_params.output_ethhdr_negative_offset = 14;
	}
	/*
	 * input_ethhdr_negative_offset is intentionally left 0 (zero-initialised
	 * by kzalloc/memset above).  ipa_backend operates in IPAv7 ETH traffic
	 * mode where the packet pointer already points at the Ethernet header.
	 * The proc_ctx must overwrite the existing ETH header in-place (offset 0
	 * from the packet start), not write before it as would be required in IP
	 * traffic mode (where the ETH header is stripped and a negative offset
	 * from the IP header start is needed).
	 */

	IPA_BE_DBG("input_ethhdr_negative_offset %d, output_ethhdr_negative_offset %d, type: %d\n",
	hdr_proc_ctx->generic_params.input_ethhdr_negative_offset,
	hdr_proc_ctx->generic_params.output_ethhdr_negative_offset,
	hdr_proc_ctx->type);

	if (ipa3_add_hdr_proc_ctx(
			(struct ipa_ioc_add_hdr_proc_ctx *)hdr_proc_ctx_table, true)) {
			handle = -EFAULT;
			kfree(hdr_proc_ctx_table);
			return handle;
	}

	handle = hdr_proc_ctx_table->proc_ctx[0].proc_ctx_hdl;
	IPA_BE_DBG("Installed hdr proc ctx: hdl %d\n", handle);

	/* Insert into hash table with ref_count = 1 */
	ipa_be_insert_proc_ctx(client_mac_vlan_str, handle);

	kfree(hdr_proc_ctx_table);
	return handle;
}

static int ipa_ipv6_header_proc_ctx(
	struct ipa_ipv6_rule_create_msg v6_msg,
	int* hdr_hdl,
	char *name,
	char *proc_ctx_name_out)
{
	int handle = 0;
	int egress_vlan_id = 0, ingress_vlan_id = 0;
	char client_mac_vlan_str[32];
	struct ipa_ioc_add_hdr_proc_ctx *hdr_proc_ctx_table;
	struct ipa_hdr_proc_ctx_add *hdr_proc_ctx;
	int size = 0;

	IPA_BE_DBG("ECMIPA entry ipa_ipv6_header_proc_ctx \n");
	/* vlan is the last 12 bits of vlan tag*/
	if (v6_msg.vlan_primary_rule.egress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED)
	{
		egress_vlan_id = v6_msg.vlan_primary_rule.egress_vlan_tag & 0x3FF;
	}
	if (v6_msg.vlan_primary_rule.ingress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED)
	{
		ingress_vlan_id = v6_msg.vlan_primary_rule.ingress_vlan_tag & 0x3FF;
	}
	IPA_BE_DBG("ECMIPA egress_vlan_id 0x%x  ingress_vlan_id 0x%x hdr_name %s\n", egress_vlan_id, ingress_vlan_id, name);

	snprintf(client_mac_vlan_str, sizeof(client_mac_vlan_str), "%02x%02x%02x%02x%02x%02x_%d",
			v6_msg.conn_rule.flow_mac[0], v6_msg.conn_rule.flow_mac[1],
			v6_msg.conn_rule.flow_mac[2], v6_msg.conn_rule.flow_mac[3],
			v6_msg.conn_rule.flow_mac[4], v6_msg.conn_rule.flow_mac[5],
			egress_vlan_id);

	/* Store the proc_ctx name for later cleanup */
	if (proc_ctx_name_out) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(proc_ctx_name_out, client_mac_vlan_str, 32);
#else
		strlcpy(proc_ctx_name_out, client_mac_vlan_str, 32);
#endif
	}

	handle = ipa_be_lookup_handle(client_mac_vlan_str);
	if (-1 != handle)
	{
		// connection for this client exists, only increase ref count
		ipa_be_insert_proc_ctx(client_mac_vlan_str, handle);
		return handle;
	}

	if (egress_vlan_id)
	{
		*hdr_hdl = ipa_ipv6_vlan_header(v6_msg,  egress_vlan_id, name);
	}
	else
	{
		*hdr_hdl = ipa_ipv6_header(v6_msg);
	}

	size = sizeof(struct ipa_ioc_add_hdr_proc_ctx) + sizeof(struct ipa_hdr_proc_ctx_add);
	hdr_proc_ctx_table = (struct ipa_ioc_add_hdr_proc_ctx*)kzalloc(size, GFP_KERNEL);
	if(hdr_proc_ctx_table == NULL)
	{
		IPA_BE_ERR("Failed to allocate memory.\n");
		return IPA_TX_FAILURE;
	}
	memset(hdr_proc_ctx_table, 0, size);

	hdr_proc_ctx_table->commit = 1;
	hdr_proc_ctx_table->num_proc_ctxs = 1;
	hdr_proc_ctx = &hdr_proc_ctx_table->proc_ctx[0];

	hdr_proc_ctx->type = IPA_HDR_PROC_ETHII_TO_ETHII_EX_DST;
	hdr_proc_ctx->hdr_hdl = *hdr_hdl;

	if (egress_vlan_id)
	{
		hdr_proc_ctx->generic_params.output_ethhdr_negative_offset = 18;
	}
	else
	{
		hdr_proc_ctx->generic_params.output_ethhdr_negative_offset = 14;
	}
	/*
	 * input_ethhdr_negative_offset is intentionally left 0 (zero-initialised
	 * by kzalloc/memset above).  ipa_backend operates in IPAv7 ETH traffic
	 * mode where the packet pointer already points at the Ethernet header.
	 * The proc_ctx must overwrite the existing ETH header in-place (offset 0
	 * from the packet start), not write before it as would be required in IP
	 * traffic mode (where the ETH header is stripped and a negative offset
	 * from the IP header start is needed).
	 */

	IPA_BE_DBG("input_ethhdr_negative_offset %d, output_ethhdr_negative_offset %d, type: %d\n",
	hdr_proc_ctx->generic_params.input_ethhdr_negative_offset,
	hdr_proc_ctx->generic_params.output_ethhdr_negative_offset,
	hdr_proc_ctx->type);

	if (ipa3_add_hdr_proc_ctx(
			(struct ipa_ioc_add_hdr_proc_ctx *)hdr_proc_ctx_table, true)) {
			handle = -EFAULT;
			kfree(hdr_proc_ctx_table);
			return handle;
	}

	handle = hdr_proc_ctx_table->proc_ctx[0].proc_ctx_hdl;
	IPA_BE_DBG("Installed hdr proc ctx: hdl %d\n", handle);

	ipa_be_insert_proc_ctx(client_mac_vlan_str, handle);

	kfree(hdr_proc_ctx_table);
	return handle;
}

/**
 * ipa_vlan_header_proc_ctx() - Create a VLAN header processing context
 * @hdr_hdl: Pointer to header handle
 * @name: Header name (unused, kept for compatibility)
 * @mac: Client MAC address for generating proc_ctx name
 * @vlan_tag: VLAN tag for generating proc_ctx name
 * @proc_ctx_name_out: Output buffer for proc_ctx name
 *
 * This function creates a header processing context for VLAN headers.
 * It's IP version agnostic and can be used for both IPv4 and IPv6.
 *
 * Return: Processing context handle on success, negative errno on failure
 */
static int ipa_vlan_header_proc_ctx(
	int* hdr_hdl,
	char *name,
	mac_addr_t mac,
	uint32_t vlan_tag,
	char *proc_ctx_name_out)
{
	int handle = 0;
	struct ipa_ioc_add_hdr_proc_ctx *hdr_proc_ctx_table;
	struct ipa_hdr_proc_ctx_add *hdr_proc_ctx;
	int size = 0;
	char client_mac_vlan_str[32];
	int vlan_id = 0;

	IPA_BE_DBG("ECMIPA entry ipa_vlan_header_proc_ctx \n");

	/* Generate proc_ctx name from MAC and VLAN */
	if (vlan_tag != 0 && vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED) {
		vlan_id = vlan_tag & 0x0FFF;  /* Extract VLAN ID from lower 12 bits */
	}

	snprintf(client_mac_vlan_str, sizeof(client_mac_vlan_str), "%02x%02x%02x%02x%02x%02x_%d_l2w",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], vlan_id);

	/* Store the proc_ctx name for later cleanup */
	if (proc_ctx_name_out) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(proc_ctx_name_out, client_mac_vlan_str, 32);
#else
		strlcpy(proc_ctx_name_out, client_mac_vlan_str, 32);
#endif
	}

	/* Check if proc_ctx already exists */
	handle = ipa_be_lookup_handle(client_mac_vlan_str);
	if (-1 != handle) {
		/* Connection for this client exists, only increase ref count */
		ipa_be_insert_proc_ctx(client_mac_vlan_str, handle);
		IPA_BE_DBG("Reusing existing proc_ctx handle %d for %s\n", handle, client_mac_vlan_str);
		return handle;
	}

	size = sizeof(struct ipa_ioc_add_hdr_proc_ctx) + sizeof(struct ipa_hdr_proc_ctx_add);
	hdr_proc_ctx_table = (struct ipa_ioc_add_hdr_proc_ctx*)kzalloc(size, GFP_KERNEL);
	if(hdr_proc_ctx_table == NULL)
	{
		IPA_BE_ERR("Failed to allocate memory.\n");
		return IPA_TX_FAILURE;
	}
	memset(hdr_proc_ctx_table, 0, size);

	hdr_proc_ctx_table->commit = 1;
	hdr_proc_ctx_table->num_proc_ctxs = 1;
	hdr_proc_ctx = &hdr_proc_ctx_table->proc_ctx[0];

	hdr_proc_ctx->type = IPA_HDR_PROC_NONE;
	hdr_proc_ctx->hdr_hdl = *hdr_hdl;


	if (ipa3_add_hdr_proc_ctx(
			(struct ipa_ioc_add_hdr_proc_ctx *)hdr_proc_ctx_table, true)) {
			handle = -EFAULT;
			kfree(hdr_proc_ctx_table);
			return handle;
	}

	handle = hdr_proc_ctx_table->proc_ctx[0].proc_ctx_hdl;
	IPA_BE_DBG("Installed hdr proc ctx: hdl %d with name %s\n", handle, client_mac_vlan_str);

	/* Insert into hash table with ref_count = 1 */
	ipa_be_insert_proc_ctx(client_mac_vlan_str, handle);

	kfree(hdr_proc_ctx_table);
	return handle;
}


/**
 * ipa_be_update_lan_info_from_rule - Updates the LAN information for a given rule
 *
 * This function updates the LAN information for a given rule in the IPA client database.
 * It takes into account the handle, process context, LAN to LAN flag, header handle, and
 * whether it's a return rule or not.
 *
 * @param rule_msg: The IPv4 rule create message
 * @param hdl: The handle
 * @param proc: The process context
 * @param lan2lan: The LAN to LAN flag
 * @param hdr_hdl: The header handle
 * @param is_ret: Whether it's a return rule or not
 * @param proc_ctx_name: The proc_ctx name for cleanup
 *
 * @return 0 on success, -1 on failure
 */
int ipa_be_update_lan_info_from_rule(struct ipa_ipv4_rule_create_msg *rule_msg, int hdl, int proc,
							  int lan2lan, int hdr_hdl, int is_ret, const char *proc_ctx_name)
{
	IPA_BE_DBG("ECMIPA entry ipa_be_update_lan_info_from_rule hdl %d proc %d lan2lan %d hdr_hdl %d int is_ret %d\n",
		hdl, proc, lan2lan, hdr_hdl, is_ret);
	if (!rule_msg)
		return -1;

	ipa_db_mapping_hash_t hash_index;
	struct ipa_clientdb_mapping_instance *mi;
	ip_addr_t key_addr = {0};

	// Compute hash index from rule_msg fields
	if (is_ret)
	{
		key_addr[0] = rule_msg->tuple.return_ip;
		hash_index = ipa_db_mapping_generate_hash_index(key_addr, 0);
		IPA_BE_DBG("Establish mapping for is_ret " IPA_IP_ADDR_DOT_FMT "\n", IPA_IP_ADDR_TO_DOT(key_addr));
	}
	else
	{
		key_addr[0] = rule_msg->tuple.flow_ip;
		hash_index = ipa_db_mapping_generate_hash_index(key_addr, 0);
		IPA_BE_DBG("Establish mapping for " IPA_IP_ADDR_DOT_FMT "\n", IPA_IP_ADDR_TO_DOT(key_addr));
	}
	IPA_BE_DBG("hash_index: %d\n", hash_index);

	if (ipa_db_mapping_table[hash_index] == NULL)
	{
		IPA_BE_ERR("No mapping exists for this hash_index: %d\n", hash_index);
		return -1;
	}

	mutex_lock(&ipa_client_db_lock);

	// Traverse the hash chain to find the matching mapping instance
	for (mi = ipa_db_mapping_table[hash_index]; mi != NULL; mi = mi->hash_next) {
		IPA_BE_DBG("mi: %p " IPA_IP_ADDR_DOT_FMT "\n", mi, IPA_IP_ADDR_TO_DOT((uint32_t *)mi->address));
		if (is_ip_addr_equal(mi->address, key_addr)) {

			if (lan2lan && hdl)
			{
				// Update lan2lan_info
				mi->lan2lan_info.rt_hdl = hdl;
				mi->lan2lan_info.proc_ctx_hdl = proc;
				mi->lan2lan_info.hdr_hdl = hdr_hdl;
				mi->lan2lan_info.ref_count++;
				/* Store proc_ctx name for cleanup */
				if (proc_ctx_name) {

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
					strscpy(mi->lan2lan_info.proc_ctx_name, proc_ctx_name, sizeof(mi->lan2lan_info.proc_ctx_name));
#else
					strlcpy(mi->lan2lan_info.proc_ctx_name, proc_ctx_name, sizeof(mi->lan2lan_info.proc_ctx_name));
#endif
				}
			}

			if (!lan2lan && hdl)
			{
				// Update lan2wan_info
				mi->lan2wan_info.rt_hdl = hdl;
				mi->lan2wan_info.proc_ctx_hdl = proc;
				mi->lan2wan_info.hdr_hdl = hdr_hdl;
				mi->lan2wan_info.ref_count++;
				/* Store proc_ctx name for cleanup */
				if (proc_ctx_name) {

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
					strscpy(mi->lan2wan_info.proc_ctx_name, proc_ctx_name, sizeof(mi->lan2wan_info.proc_ctx_name));
#else
					strlcpy(mi->lan2wan_info.proc_ctx_name, proc_ctx_name, sizeof(mi->lan2wan_info.proc_ctx_name));
#endif
				}
			}

			break;
		}
	}

	mutex_unlock(&ipa_client_db_lock);
	if (mi) {
		IPA_BE_DBG("Exit ..new ref count lan2lan %d l2w %d\n", mi->lan2lan_info.ref_count, mi->lan2wan_info.ref_count);
	} else {
		IPA_BE_DBG("No matching mapping instance found for update\n");
	}
	return 0;
}

int ipa_be_update_lan_v6_info_from_rule(struct ipa_ipv6_rule_create_msg *rule_msg, int hdl, int proc,
							  int lan2lan, int hdr_hdl, int is_ret, const char *proc_ctx_name)
{
	IPA_BE_DBG("ECMIPA entry ipa_be_update_lan_v6_info_from_rule hdl %d proc %d lan2lan %d hdr_hdl %d int is_ret %d\n",
		hdl, proc, lan2lan, hdr_hdl, is_ret);
	if (!rule_msg)
		return -1;

	ipa_db_mapping_hash_t hash_index;
	struct ipa_clientdb_mapping_instance *mi;
	uint32_t *key_ip = NULL;

	// Compute hash index from rule_msg fields
	if (is_ret)
	{
		hash_index = ipa_db_mapping_generate_hash_index((uint32_t *)&rule_msg->tuple.return_ip, 0);
		IPA_BE_DBG("Establish mapping for is_ret " IPA_IP_ADDR_DOT_FMT "\n", IPA_IP_ADDR_TO_DOT((uint32_t *)&rule_msg->tuple.return_ip));
	}
	else
	{
		hash_index = ipa_db_mapping_generate_hash_index((uint32_t *)&rule_msg->tuple.flow_ip, 0);
		IPA_BE_DBG("Establish mapping for " IPA_IP_ADDR_DOT_FMT "\n", IPA_IP_ADDR_TO_DOT((uint32_t *)&rule_msg->tuple.flow_ip));
	}
	IPA_BE_DBG("hash_index: %d\n", hash_index);

	if (ipa_db_mapping_table[hash_index] == NULL)
	{
		IPA_BE_ERR("No mapping exists for this hash_index: %d\n", hash_index);
		return -1;
	}

	key_ip = is_ret ? (uint32_t *)&rule_msg->tuple.return_ip : (uint32_t *)&rule_msg->tuple.flow_ip;

	mutex_lock(&ipa_client_db_lock);

	// Traverse the hash chain to find the matching mapping instance
	for (mi = ipa_db_mapping_table[hash_index]; mi != NULL; mi = mi->hash_next) {
		IPA_BE_DBG("mi: %p " IPA_IP_ADDR_DOT_FMT "\n", mi, IPA_IP_ADDR_TO_DOT((uint32_t *)mi->address));
		if (is_ip_addr_equal(mi->address, (uint32_t *)key_ip)) {

			if (lan2lan && hdl)
			{
				// Update lan2lan_info
				mi->lan2lan_info.rt_hdl = hdl;
				mi->lan2lan_info.proc_ctx_hdl = proc;
				mi->lan2lan_info.hdr_hdl = hdr_hdl;
				mi->lan2lan_info.ref_count++;
				/* Store proc_ctx name for cleanup */
				if (proc_ctx_name) {

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
					strscpy(mi->lan2lan_info.proc_ctx_name, proc_ctx_name, sizeof(mi->lan2lan_info.proc_ctx_name));
#else
					strlcpy(mi->lan2lan_info.proc_ctx_name, proc_ctx_name, sizeof(mi->lan2lan_info.proc_ctx_name));
#endif

				}
			}

			if (!lan2lan && hdl)
			{
				// Update lan2wan_info
				mi->lan2wan_info.rt_hdl = hdl;
				mi->lan2wan_info.proc_ctx_hdl = proc;
				mi->lan2wan_info.hdr_hdl = hdr_hdl;
				mi->lan2wan_info.ref_count++;
				/* Store proc_ctx name for cleanup */
				if (proc_ctx_name) {

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
					strscpy(mi->lan2wan_info.proc_ctx_name, proc_ctx_name, sizeof(mi->lan2wan_info.proc_ctx_name));
#else
					strlcpy(mi->lan2wan_info.proc_ctx_name, proc_ctx_name, sizeof(mi->lan2wan_info.proc_ctx_name));
#endif

				}
			}

			break;
		}
	}

	mutex_unlock(&ipa_client_db_lock);
	if (mi) {
		IPA_BE_DBG("Exit ..new ref count lan2lan %d l2w %d\n", mi->lan2lan_info.ref_count, mi->lan2wan_info.ref_count);
	} else {
		IPA_BE_DBG("No matching mapping instance found for update\n");
	}
	return 0;
}

int ipa_get_rt_hdl_from_mapping(ip_addr_t addr, bool lan2lan, int *hdr_hdl, int *proc_ctx_hdl, char *proc_ctx_name_out)
{
	ipa_db_mapping_hash_t hash_index;
	struct ipa_clientdb_mapping_instance *mi;
	int rt_hdl = -1;

	/* Initialize output parameters */
	if (hdr_hdl)
		*hdr_hdl = -1;
	if (proc_ctx_hdl)
		*proc_ctx_hdl = -1;
	if (proc_ctx_name_out)
		proc_ctx_name_out[0] = '\0';

	hash_index = ipa_db_mapping_generate_hash_index(addr, 0);
	IPA_BE_DBG("hash_index %d\n", hash_index);

	mutex_lock(&ipa_client_db_lock);

	for (mi = ipa_db_mapping_table[hash_index]; mi != NULL; mi = mi->hash_next) {
		IPA_BE_DBG("try ..\n");
		IPA_BE_DBG("ECMIPA create flow_ip mi addr: %pI4n: addr %pI4n n", &mi->address, &addr);
		IPA_BE_DBG("before lan2lan_info %d, lan2wan_info %d\n", mi->lan2lan_info.rt_hdl, mi->lan2wan_info.rt_hdl);
		if (is_ip_addr_equal(mi->address, addr)) {
			IPA_BE_DBG("is_ip_addr_equal true lan2lan_info %d, lan2wan_info %d\n", mi->lan2lan_info.rt_hdl, mi->lan2wan_info.rt_hdl);
			if (lan2lan) {
				rt_hdl = mi->lan2lan_info.rt_hdl;
				if (hdr_hdl)
					*hdr_hdl = mi->lan2lan_info.hdr_hdl;
				if (proc_ctx_hdl)
					*proc_ctx_hdl = mi->lan2lan_info.proc_ctx_hdl;
				if (proc_ctx_name_out && mi->lan2lan_info.proc_ctx_name[0] != '\0')

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
					strscpy(proc_ctx_name_out, mi->lan2lan_info.proc_ctx_name, 32);
#else
					strlcpy(proc_ctx_name_out, mi->lan2lan_info.proc_ctx_name, 32);
#endif

			} else {
				rt_hdl = mi->lan2wan_info.rt_hdl;
				if (hdr_hdl)
					*hdr_hdl = mi->lan2wan_info.hdr_hdl;
				if (proc_ctx_hdl)
					*proc_ctx_hdl = mi->lan2wan_info.proc_ctx_hdl;
				if (proc_ctx_name_out && mi->lan2wan_info.proc_ctx_name[0] != '\0')

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
					strscpy(proc_ctx_name_out, mi->lan2wan_info.proc_ctx_name, 32);
#else
					strlcpy(proc_ctx_name_out, mi->lan2wan_info.proc_ctx_name, 32);
#endif
			}
			break;
		}
		IPA_BE_DBG("lan2lan_info %d, lan2wan_info %d\n", mi->lan2lan_info.rt_hdl, mi->lan2wan_info.rt_hdl);
	}

	IPA_BE_DBG("rt_hdl %d, hdr_hdl %d, proc_ctx_hdl %d\n", rt_hdl, 
		   hdr_hdl ? *hdr_hdl : -1, proc_ctx_hdl ? *proc_ctx_hdl : -1);
	mutex_unlock(&ipa_client_db_lock);
	return rt_hdl;
}

int ipa_ipv4_add_route_rule(struct ipa_ipv4_rule_create_msg v4_msg, bool lan2lan, int intf_num, mac_addr_t mac, int is_ret)
{
	int retval = 0;
	struct ipa_ioc_add_rt_rule_v2 *rt_rule = NULL;
	struct ipa_rt_rule_add_v2 *rt_rule_entry = NULL;
	int NUM = 1;
	struct ipa_ioc_query_intf_tx_props *tx_prop = NULL;
	int tx_index = 0;
	struct ipa_ioc_query_intf temp_intf;
	int proc_ctx_hdl = 0;
	int rt_hdl = 0;
	int hdr_hdl = 0;

	IPA_BE_DBG("ECMIPA entry ipa_ipv4_add_route_rule lan2lan %d is_ret %d\n", lan2lan, is_ret);
	ipa_type_check_ipa_mac_addr(mac);

	IPA_BE_DBG("ECMIPA checking intf_num %d \n", intf_num);
	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(intf_num, &temp_intf)) {
		retval = -EINVAL;
		goto cleanup;
	}
	else {
		IPA_BE_DBG("Interface with index %u does not exist.\n", intf_num);
	}

	IPA_BE_DBG("ECMIPA checking intf_num %d exists\n", intf_num);
	tx_prop = (struct ipa_ioc_query_intf_tx_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_tx_props) +
							temp_intf.num_tx_props * sizeof(struct ipa_ioc_tx_intf_prop), GFP_KERNEL);

	memcpy(tx_prop->name, temp_intf.name, sizeof(temp_intf.name));
	tx_prop->num_tx_props = temp_intf.num_tx_props;
	IPA_BE_DBG("Query tx_prop %d name %s\n", tx_prop->num_tx_props, temp_intf.name);
	ipa3_query_intf_tx_props(tx_prop);


	rt_rule = (struct ipa_ioc_add_rt_rule_v2 *)kzalloc(sizeof(struct ipa_ioc_add_rt_rule_v2), GFP_KERNEL);
	if (rt_rule == NULL)
	{
		IPA_BE_ERR("Error Locate ipa_ioc_add_rt_rule memory...\n");
		retval = -EFAULT;
		goto cleanup;
	}

	rt_rule->rules = (uintptr_t)kzalloc(NUM * sizeof(struct ipa_rt_rule_add_v2), GFP_KERNEL);
	if (!rt_rule->rules)
	{
		IPA_BE_ERR("Error Locate ipa_rt_rule_add_v2 memory...\n");
		retval = -EFAULT;
		goto cleanup;
	}

	rt_rule->commit = true;
	rt_rule->num_rules = (uint8_t)NUM;
	rt_rule->ip = 0;
	rt_rule->rule_add_size = sizeof(struct ipa_rt_rule_add_v2);

	rt_rule_entry = &(((struct ipa_rt_rule_add_v2 *)(uintptr_t)rt_rule->rules)[0]);
	rt_rule_entry->at_rear = false;

	char proc_ctx_name[32] = {0};

	if (lan2lan)
	{
		proc_ctx_hdl = ipa_ipv4_header_proc_ctx(v4_msg, &hdr_hdl, tx_prop->tx[0].hdr_name, proc_ctx_name);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(rt_rule->rt_tbl_name, V4_LAN_ROUTE_TABLE_NAME, sizeof(rt_rule->rt_tbl_name));
#else
		strlcpy(rt_rule->rt_tbl_name, V4_LAN_ROUTE_TABLE_NAME, sizeof(rt_rule->rt_tbl_name));
#endif
		rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX - 1] = '\0';

		rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
		IPA_BE_DBG("Install rules at destination pipe %d\n", tx_prop->tx[tx_index].dst_pipe);

		memcpy(&rt_rule_entry->rule.attrib,
				&tx_prop->tx[tx_index].attrib,
				sizeof(rt_rule_entry->rule.attrib));

		if (is_ret)
		{
			rt_rule_entry->rule.attrib.u.v4.dst_addr = (uint32_t)ntohl(v4_msg.tuple.return_ip);
		}
		else
		{
			rt_rule_entry->rule.attrib.u.v4.dst_addr = (uint32_t)ntohl(v4_msg.tuple.flow_ip);
		}
		rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;

		rt_rule_entry->rule.hdr_proc_ctx_hdl = proc_ctx_hdl;
		rt_rule_entry->rt_rule_category = IPA_RT_RULE_CAT_LAN2LAN;
	}
	else
	{

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(rt_rule->rt_tbl_name, V4_LAN_ROUTE_TABLE_NAME, sizeof(rt_rule->rt_tbl_name));
#else
		strlcpy(rt_rule->rt_tbl_name, V4_LAN_ROUTE_TABLE_NAME, sizeof(rt_rule->rt_tbl_name));
#endif
		rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX - 1] = '\0';

		rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
		IPA_BE_DBG("Install lan2wan rules at destination pipe %d\n", tx_prop->tx[tx_index].dst_pipe);

		memcpy(&rt_rule_entry->rule.attrib,
				&tx_prop->tx[tx_index].attrib,
				sizeof(rt_rule_entry->rule.attrib));

		//rt_rule_entry->rule.hdr_hdl = ipa_ipv4_eth_hdr_init(mac, v4_msg, tx_prop->tx[0].hdr_name);

		hdr_hdl = ipa_eth_hdr_init(mac, tx_prop->tx[0].hdr_name, IPA_IP_v4,
					   v4_msg.conn_rule.flow_interface_num,
					   v4_msg.conn_rule.return_interface_num,
					   v4_msg.conn_rule.flow_top_interface_num,
					   v4_msg.conn_rule.return_top_interface_num,
					   v4_msg.vlan_primary_rule.egress_vlan_tag);

		if(hdr_hdl <= 0)
		{
			IPA_BE_ERR("Error creating header handle\n");
			retval = -EFAULT;
			goto cleanup;
		}
		else if(v4_msg.vlan_primary_rule.egress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED){
			proc_ctx_hdl = ipa_vlan_header_proc_ctx(&hdr_hdl, tx_prop->tx[0].hdr_name, mac, v4_msg.vlan_primary_rule.egress_vlan_tag, proc_ctx_name);
			rt_rule_entry->rule.hdr_proc_ctx_hdl = proc_ctx_hdl;
		}
		else {
			rt_rule_entry->rule.hdr_hdl = hdr_hdl;
		}

		rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;

		if (is_ret) {
			rt_rule_entry->rule.attrib.u.v4.dst_addr = (uint32_t)ntohl(v4_msg.conn_rule.return_ip_xlate);
		}
		else {
			rt_rule_entry->rule.attrib.u.v4.dst_addr = (uint32_t)ntohl(v4_msg.tuple.flow_ip);
		}
		rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;

		rt_rule_entry->rule.hashable = true;
		rt_rule_entry->rt_rule_category = IPA_RT_RULE_CAT_CLIENT;
	}

	if (ipa3_add_rt_rule_usr_v2((struct ipa_ioc_add_rt_rule_v2 *)rt_rule,
				true)) {
			IPA_BE_ERR("Route rule add failed\n");
			retval = -EFAULT;
	}
	else
	{
		rt_hdl = rt_rule_entry->rt_rule_hdl;
		IPA_BE_DBG("Lan2Lan %d route rule hdl: %d\n", lan2lan, rt_hdl);

		ipa_be_update_lan_info_from_rule(&v4_msg, rt_hdl, proc_ctx_hdl, lan2lan, hdr_hdl, is_ret, proc_ctx_name);
	}

cleanup:
	IPA_BE_DBG("Exit retval %d\n", retval);
	if (rt_rule) {
		if (rt_rule->rules)
			kfree((void *)(uintptr_t)rt_rule->rules);
		kfree(rt_rule);
	}
	kfree(tx_prop);
	return retval;
}

int ipa_ipv6_add_route_rule(struct ipa_ipv6_rule_create_msg v6_msg, bool lan2lan, int intf_num, mac_addr_t mac, int is_ret)
{
	int retval = 0;
	struct ipa_ioc_add_rt_rule_v2 *rt_rule = NULL;
	struct ipa_rt_rule_add_v2 *rt_rule_entry = NULL;
	int NUM = 1;
	struct ipa_ioc_query_intf_tx_props *tx_prop = NULL;
	int tx_index = 1;
	struct ipa_ioc_query_intf temp_intf;
	int proc_ctx_hdl = 0;
	int rt_hdl = 0;
	int hdr_hdl = 0;

	IPA_BE_DBG("ECMIPA entry ipa_ipv6_add_route_rule lan2lan %d is_ret %d\n", lan2lan, is_ret);
	ipa_type_check_ipa_mac_addr(mac);

	IPA_BE_DBG("ECMIPA checking intf_num %d \n", intf_num);
	/*Check if the filter interface exists*/
	if (!ipa3_query_iface(intf_num, &temp_intf)) {
		retval = -EINVAL;
		goto cleanup;
	}
	else {
		IPA_BE_DBG("Interface with index %u does not exist.\n", intf_num);
	}

	IPA_BE_DBG("ECMIPA checking intf_num %d exists\n", intf_num);
	tx_prop = (struct ipa_ioc_query_intf_tx_props *)kzalloc(sizeof(struct ipa_ioc_query_intf_tx_props) +
							temp_intf.num_tx_props * sizeof(struct ipa_ioc_tx_intf_prop), GFP_KERNEL);

	memcpy(tx_prop->name, temp_intf.name, sizeof(temp_intf.name));
	tx_prop->num_tx_props = temp_intf.num_tx_props;
	IPA_BE_DBG("Query tx_prop %d name %s\n", tx_prop->num_tx_props, temp_intf.name);
	ipa3_query_intf_tx_props(tx_prop);

	rt_rule = (struct ipa_ioc_add_rt_rule_v2 *)kzalloc(sizeof(struct ipa_ioc_add_rt_rule_v2), GFP_KERNEL);
	if (rt_rule == NULL)
	{
		IPA_BE_ERR("Error Locate ipa_ioc_add_rt_rule memory...\n");
		retval = -EFAULT;
		goto cleanup;
	}

	rt_rule->rules = (uintptr_t)kzalloc(NUM * sizeof(struct ipa_rt_rule_add_v2), GFP_KERNEL);
	if (!rt_rule->rules)
	{
		IPA_BE_ERR("Error Locate ipa_rt_rule_add_v2 memory...\n");
		retval = -EFAULT;
		goto cleanup;
	}

	rt_rule->commit = true;
	rt_rule->num_rules = (uint8_t)NUM;
	rt_rule->ip = IPA_IP_v6;
	rt_rule->rule_add_size = sizeof(struct ipa_rt_rule_add_v2);

	rt_rule_entry = &(((struct ipa_rt_rule_add_v2 *)(uintptr_t)rt_rule->rules)[0]);
	rt_rule_entry->at_rear = false;

	char proc_ctx_name[32] = {0};

	if (lan2lan)
	{
		proc_ctx_hdl = ipa_ipv6_header_proc_ctx(v6_msg, &hdr_hdl, tx_prop->tx[0].hdr_name, proc_ctx_name);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(rt_rule->rt_tbl_name, V6_WAN_ROUTE_TABLE_NAME, sizeof(rt_rule->rt_tbl_name));
#else
		strlcpy(rt_rule->rt_tbl_name, V6_WAN_ROUTE_TABLE_NAME, sizeof(rt_rule->rt_tbl_name));
#endif
		rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX - 1] = '\0';

		rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
		IPA_BE_DBG("Install rules at destination pipe %d\n", tx_prop->tx[tx_index].dst_pipe);

		memcpy(&rt_rule_entry->rule.attrib,
				&tx_prop->tx[tx_index].attrib,
				sizeof(rt_rule_entry->rule.attrib));

		if (is_ret)
		{
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			for (int i = 0; i < 4;i++)
			{
				rt_rule_entry->rule.attrib.u.v6.dst_addr[i] = (uint32_t)ntohl(v6_msg.tuple.return_ip[i]);
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[i] = 0xFFFFFFFF;
			}
		}
		else
		{
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			for (int i = 0; i < 4;i++)
			{
				rt_rule_entry->rule.attrib.u.v6.dst_addr[i] = (uint32_t)ntohl(v6_msg.tuple.flow_ip[i]);
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[i] = 0xFFFFFFFF;
			}
		}

		//To do
		rt_rule_entry->rule.hdr_proc_ctx_hdl = proc_ctx_hdl;
		rt_rule_entry->rt_rule_category = IPA_RT_RULE_CAT_LAN2LAN;
	}
	else
	{

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 18, 0)
		strscpy(rt_rule->rt_tbl_name, V6_WAN_ROUTE_TABLE_NAME, sizeof(rt_rule->rt_tbl_name));
#else
		strlcpy(rt_rule->rt_tbl_name, V6_WAN_ROUTE_TABLE_NAME, sizeof(rt_rule->rt_tbl_name));
#endif
		rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX - 1] = '\0';

		rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
		IPA_BE_DBG("Install lan2wan rules at destination pipe %d\n", tx_prop->tx[tx_index].dst_pipe);

		memcpy(&rt_rule_entry->rule.attrib,
				&tx_prop->tx[tx_index].attrib,
				sizeof(rt_rule_entry->rule.attrib));

		hdr_hdl = ipa_eth_hdr_init(mac, tx_prop->tx[1].hdr_name, IPA_IP_v6,
					   v6_msg.conn_rule.flow_interface_num,
					   v6_msg.conn_rule.return_interface_num,
					   v6_msg.conn_rule.flow_top_interface_num,
					   v6_msg.conn_rule.return_top_interface_num,
					   v6_msg.vlan_primary_rule.egress_vlan_tag);

		if(hdr_hdl <= 0)
		{
			IPA_BE_ERR("Error Install header handle...\n");
			retval = -EFAULT;
			goto cleanup;
		}

		if(v6_msg.vlan_primary_rule.egress_vlan_tag != IPA_VLAN_ID_NOT_CONFIGURED){
			proc_ctx_hdl = ipa_vlan_header_proc_ctx(&hdr_hdl, tx_prop->tx[1].hdr_name, mac, v6_msg.vlan_primary_rule.egress_vlan_tag, proc_ctx_name);
			rt_rule_entry->rule.hdr_proc_ctx_hdl = proc_ctx_hdl;
		}
		else {
			rt_rule_entry->rule.hdr_hdl = hdr_hdl;
		}

		rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		for (int i = 0; i < 4;i++)
		{
			if (is_ret)
			{
				rt_rule_entry->rule.attrib.u.v6.dst_addr[i] = (uint32_t)ntohl(v6_msg.conn_rule.return_ip_xlate[i]);
			}
			else {
				rt_rule_entry->rule.attrib.u.v6.dst_addr[i] = (uint32_t)ntohl(v6_msg.tuple.flow_ip[i]);
			}
			IPA_BE_DBG("dst_addr[%d]: 0x%x\n", i, rt_rule_entry->rule.attrib.u.v6.dst_addr[i]);
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[i] = 0xFFFFFFFF;
		}

		rt_rule_entry->rule.hashable = true;
		rt_rule_entry->rt_rule_category = IPA_RT_RULE_CAT_CLIENT;
	}

	if (ipa3_add_rt_rule_usr_v2((struct ipa_ioc_add_rt_rule_v2 *)rt_rule,
				true)) {
			IPA_BE_ERR("Route rule add failed\n");
			retval = -EFAULT;
	}
	else
	{
		rt_hdl = rt_rule_entry->rt_rule_hdl;
		IPA_BE_DBG("Lan2Lan %d route rule hdl: %d\n", lan2lan, rt_hdl);

		ipa_be_update_lan_v6_info_from_rule(&v6_msg, rt_hdl, proc_ctx_hdl, lan2lan, hdr_hdl, is_ret, proc_ctx_name);
	}

cleanup:
	IPA_BE_DBG("Exit retval %d\n", retval);
	if (rt_rule) {
		if (rt_rule->rules)
			kfree((void *)(uintptr_t)rt_rule->rules);
		kfree(rt_rule);
	}
	kfree(tx_prop);
	return retval;
}

/**
 * ipa_be_delete_proc_ctx() - Decrements reference count and deletes proc context if needed
 * @name: The name of the proc context to delete
 *
 * This function decrements the reference count of a proc context entry.
 * If the reference count reaches 0, it deletes the proc context from IPA
 * and removes the entry from the hash table.
 *
 * Return: 0 on success, negative errno on failure
 */
int ipa_be_delete_proc_ctx(char *name)
{
	unsigned int index;
	ProcCtx *entry, *prev = NULL;
	struct ipa_ioc_del_hdr_proc_ctx *del_proc_ctx;
	int ret = 0;

	if (!name) {
		IPA_BE_ERR("Invalid name parameter\n");
		return -EINVAL;
	}

	index = ipa_proc_ctx_generate_hash_index(name);
	entry = hash_table_ipa[index];

	/* Find the entry */
	while (entry) {
		if (strcmp(entry->name, name) == 0) {
			/* Decrement reference count */
			entry->ref_count--;
			IPA_BE_DBG("Decremented ref_count for proc ctx %s to %d\n", name, entry->ref_count);

			/* If reference count reaches 0, delete the proc context */
			if (entry->ref_count <= 0) {
				/* Delete from IPA */
				del_proc_ctx = kzalloc(sizeof(struct ipa_ioc_del_hdr_proc_ctx) + 
						      sizeof(struct ipa_hdr_proc_ctx_del), GFP_KERNEL);
				if (!del_proc_ctx) {
					IPA_BE_ERR("Failed to allocate memory for del_proc_ctx\n");
					return -ENOMEM;
				}

				del_proc_ctx->commit = true;
				del_proc_ctx->num_hdls = 1;
				del_proc_ctx->hdl[0].hdl = entry->handle;

				if (ipa3_del_hdr_proc_ctx(del_proc_ctx)) {
					IPA_BE_ERR("Failed to delete proc ctx handle %d\n", entry->handle);
					ret = -EFAULT;
				} else {
					IPA_BE_DBG("Successfully deleted proc ctx handle %d\n", entry->handle);
				}

				kfree(del_proc_ctx);

				/* Remove from hash table */
				if (prev) {
					prev->next = entry->next;
				} else {
					hash_table_ipa[index] = entry->next;
				}

				kfree(entry);
				IPA_BE_DBG("Removed proc ctx entry %s from hash table\n", name);
			}
			return ret;
		}
		prev = entry;
		entry = entry->next;
	}

	IPA_BE_DBG("Proc ctx entry %s not found\n", name);
	return -ENOENT;
}

/**
 * ipa_be_delete_hdr_handle() - Deletes a header handle from IPA
 * @hdr_hdl: The header handle to delete
 *
 * Return: 0 on success, negative errno on failure
 */
int ipa_be_delete_hdr_handle(int hdr_hdl)
{
	struct ipa_ioc_del_hdr *del_hdr;
	int ret = 0;

	if (hdr_hdl <= 0) {
		IPA_BE_DBG("Invalid header handle %d\n", hdr_hdl);
		return 0; /* Not an error, just nothing to delete */
	}

	del_hdr = kzalloc(sizeof(struct ipa_ioc_del_hdr) + sizeof(struct ipa_hdr_del), GFP_KERNEL);
	if (!del_hdr) {
		IPA_BE_ERR("Failed to allocate memory for del_hdr\n");
		return -ENOMEM;
	}

	del_hdr->commit = true;
	del_hdr->num_hdls = 1;
	del_hdr->hdl[0].hdl = hdr_hdl;

	if (ipa3_del_hdr_by_user(del_hdr, true)) {
		IPA_BE_ERR("Failed to delete header handle %d\n", hdr_hdl);
		ret = -EFAULT;
	} else {
		IPA_BE_DBG("Successfully deleted header handle %d\n", hdr_hdl);
	}

	kfree(del_hdr);
	return ret;
}

int ipa_delete_route_rule(bool lan2lan, int rt_rule_hdl, enum ipa_ip_type ip)
{
	int retval = 0;
	const uint8_t NUM_RULES = 1;
	struct ipa_ioc_del_rt_rule *rt_rule;
	struct ipa_rt_rule_del *rt_rule_entry;
	bool res = true;
	int len = 0;

	IPA_BE_DBG("Entry ipa_delete_route_rule\n");

	if (rt_rule_hdl == 0)
	{
		IPA_BE_ERR(" No route handle passed. Ignoring it\n");
		return res;
	}

	len = (sizeof(struct ipa_ioc_del_rt_rule)) + (NUM_RULES * sizeof(struct ipa_rt_rule_del));
	rt_rule = (struct ipa_ioc_del_rt_rule *)kzalloc(len, GFP_KERNEL);
	if (rt_rule == NULL)
	{
		IPA_BE_ERR("unable to allocate memory for del route rule\n");
		return false;
	}

	memset(rt_rule, 0, len);
	rt_rule->commit = true;
	rt_rule->num_hdls = NUM_RULES;
	rt_rule->ip = ip;

	rt_rule_entry = &rt_rule->hdl[0];
	rt_rule_entry->status = -1;
	rt_rule_entry->hdl = rt_rule_hdl;

	IPA_BE_DBG("Entry ip type %d\n", rt_rule->ip);
	if (ipa3_del_rt_rule((struct ipa_ioc_del_rt_rule *)rt_rule)) {
			retval = -EFAULT;
		}

	IPA_BE_DBG("Delete route rule hdl %d successfully\n", rt_rule_hdl);
	return retval;
}

int ipa_ipv6_delete_route_rule(struct ipa_ipv6_rule_destroy_msg v6_msg, bool lan2lan, int rt_rule_hdl, enum ipa_ip_type ip)
{
	int retval = 0;
	const uint8_t NUM_RULES = 1;
	struct ipa_ioc_del_rt_rule *rt_rule;
	struct ipa_rt_rule_del *rt_rule_entry;
	int len = 0;

	IPA_BE_DBG("Entry ipa_ipv6_delete_route_rule rt_rule_hdl %d\n", rt_rule_hdl);

	if (rt_rule_hdl == 0) {
		IPA_BE_ERR("No route handle passed. Ignoring it\n");
		return 0;
	}

	len = sizeof(struct ipa_ioc_del_rt_rule) + (NUM_RULES * sizeof(struct ipa_rt_rule_del));
	rt_rule = (struct ipa_ioc_del_rt_rule *)kzalloc(len, GFP_KERNEL);
	if (rt_rule == NULL) {
		IPA_BE_ERR("Unable to allocate memory for del route rule\n");
		return -ENOMEM;
	}

	memset(rt_rule, 0, len);
	rt_rule->commit = true;
	rt_rule->num_hdls = NUM_RULES;
	rt_rule->ip = ip;

	rt_rule_entry = &rt_rule->hdl[0];
	rt_rule_entry->status = -1;
	rt_rule_entry->hdl = rt_rule_hdl;

	IPA_BE_DBG("Deleting IPv6 route rule with ip type %d\n", rt_rule->ip);
	if (ipa3_del_rt_rule(rt_rule)) {
		IPA_BE_ERR("Failed to delete IPv6 route rule hdl %d\n", rt_rule_hdl);
		retval = -EFAULT;
	} else {
		IPA_BE_DBG("Successfully deleted IPv6 route rule hdl %d\n", rt_rule_hdl);
	}

	kfree(rt_rule);
	return retval;
}
