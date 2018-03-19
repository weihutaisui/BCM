/*
  Copyright (c) 2014 Broadcom Corporation
  All Rights Reserved

  <:label-BRCM:2014:DUAL/GPL:standard
    
  Unless you and Broadcom execute a separate written software license
  agreement governing use of this software, this software is licensed
  to you under the terms of the GNU General Public License version 2
  (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
  with the following added to such license:
    
  As a special exception, the copyright holders of this software give
  you permission to link this software with independent modules, and
  to copy and distribute the resulting executable under terms of your
  choice, provided that you also meet, for each linked independent
  module, the terms and conditions of the license of that module.
  An independent module is a module which is not derived from this
  software.  The special exception does not apply to any modifications
  of the software.
    
  Not withstanding the above, under no circumstances may you combine
  this software in any way with any other Broadcom software provided
  under a license other than the GPL, without Broadcom's express prior
  written consent.
    
  :>
*/

#ifdef CONFIG_WLAN_MCAST

#include "rdd.h"
#include "rdd_wlan_mcast_common.h"
#ifdef CONFIG_DHD_RUNNER
#include "rdd_dhd_helper.h"
#include "rdd_ag_dhd_tx_post.h"
#endif

#ifdef CONFIG_DHD_RUNNER
/* SSID Macs for proxy mode */

static int _rdd_wlan_mcast_ssid_mac_address_get(uint32_t ssid_mac_idx, RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY_DTS *entry,
    int ref_count_only)
{
    if (ssid_mac_idx >= RDD_WLAN_MCAST_SSID_MAC_ADDRESS_TABLE_SIZE)
        return BDMF_ERR_RANGE;

    RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY_REFERENCE_COUNT_READ_G(entry->reference_count,
        RDD_WLAN_MCAST_SSID_MAC_ADDRESS_TABLE_ADDRESS_ARR, ssid_mac_idx);
    if (!entry->reference_count)
        return BDMF_ERR_NOENT;

    if (!ref_count_only)
    {
        RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY_MAC_ADDRESS_HIGH_READ_G(entry->mac_address_high,
            RDD_WLAN_MCAST_SSID_MAC_ADDRESS_TABLE_ADDRESS_ARR, ssid_mac_idx);

        RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY_MAC_ADDRESS_LOW_READ_G(entry->mac_address_low,
            RDD_WLAN_MCAST_SSID_MAC_ADDRESS_TABLE_ADDRESS_ARR, ssid_mac_idx);
    }

    return 0;
}

static void _rdd_wlan_mcast_ssid_mac_address_set(uint32_t ssid_mac_idx,
    RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY_DTS *entry)
{
    RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY_REFERENCE_COUNT_WRITE_G(entry->reference_count,
        RDD_WLAN_MCAST_SSID_MAC_ADDRESS_TABLE_ADDRESS_ARR, ssid_mac_idx);

    RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY_MAC_ADDRESS_HIGH_WRITE_G(entry->mac_address_high,
        RDD_WLAN_MCAST_SSID_MAC_ADDRESS_TABLE_ADDRESS_ARR, ssid_mac_idx);

    RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY_MAC_ADDRESS_LOW_WRITE_G(entry->mac_address_low,
        RDD_WLAN_MCAST_SSID_MAC_ADDRESS_TABLE_ADDRESS_ARR, ssid_mac_idx);
}

int rdd_wlan_mcast_ssid_mac_address_add(uint32_t radio_index, uint32_t ssid,
    RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY_DTS *entry, uint32_t *ssid_mac_idx)
{
    RDD_BTRACE("radio_index = %d, ssid = %d, entry = %p, ssid_mac_idx = %p\n",
        radio_index, ssid, entry, ssid_mac_idx);

    *ssid_mac_idx = RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY_INDEX(radio_index, ssid);
    if (_rdd_wlan_mcast_ssid_mac_address_get(*ssid_mac_idx, entry, 1) == BDMF_ERR_RANGE)
        return BDMF_ERR_RANGE;

    entry->reference_count++;
    _rdd_wlan_mcast_ssid_mac_address_set(*ssid_mac_idx, entry);

    return 0;
}

int rdd_wlan_mcast_ssid_mac_address_delete(uint32_t ssid_mac_idx)
{
    RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY_DTS entry;
    int rc;

    RDD_BTRACE("ssid_mac_idx = %d\n", ssid_mac_idx);

    rc = _rdd_wlan_mcast_ssid_mac_address_get(ssid_mac_idx, &entry, 0);
    if (rc)
        return rc;

    if (!--entry.reference_count)
    {
        entry.mac_address_low = 0;
        entry.mac_address_high = 0;
    }
    _rdd_wlan_mcast_ssid_mac_address_set(ssid_mac_idx, &entry);
    return 0;
}

int rdd_wlan_mcast_ssid_mac_address_read(uint32_t ssid_mac_idx,
    RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY_DTS *entry)
{
    RDD_BTRACE("ssid_mac_idx = %d, entry = %p\n", ssid_mac_idx, entry);

    return _rdd_wlan_mcast_ssid_mac_address_get(ssid_mac_idx, entry, 0);
}

#endif /* CONFIG_DHD_RUNNER */

int rdd_wlan_mcast_init(wlan_mcast_dhd_list_table_t *table)
{
    int rc;

    RDD_BTRACE("DFT PTR: Virt = %p, Phys = %llx\n", table ? table->virt_p : NULL,
        table ? (long long)table->phys_addr : 0);
    rc = rdd_wlan_mcast_init_common(table);
    if (rc)
        return rc;

#ifdef CONFIG_DHD_RUNNER
    rdd_wlan_mcast_dft_init(table->phys_addr);
#endif
    return 0;
}

#ifdef CONFIG_DHD_RUNNER
void rdd_wlan_mcast_dft_init(bdmf_phys_addr_t dft_phys_addr)
{
    uint32_t addr_hi, addr_lo;
    int i;

    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, dft_phys_addr);
    rdd_ag_dhd_tx_post_wlan_mcast_dft_addr_set(addr_lo, addr_hi);

    for (i = 0; i < RDD_WLAN_MCAST_DFT_LIST_SIZE_SIZE; i++)
        rdd_ag_dhd_tx_post_wlan_mcast_dft_list_size_set(i, 0);
}
#endif

#endif

