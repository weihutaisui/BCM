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

#include "rdd.h"
#include "rdd_wlan_mcast_common.h"

/*
 *  DHD List table management API
 */

wlan_mcast_dhd_list_table_t wlan_mcast_dhd_list_table_g = { NULL, 0 };

#ifdef CONFIG_DHD_RUNNER
int rdd_wlan_mcast_dhd_list_entry_add(uint32_t fwd_entry_index,
    RDD_WLAN_MCAST_DHD_LIST_ENTRY_DTS *dhd_list_entry, uint32_t *dhd_list_size)
{
    RDD_WLAN_MCAST_DHD_LIST_TABLE_DTS *dhd_list_table = wlan_mcast_dhd_list_table_g.virt_p;
    uint32_t list_size;
    uint32_t i;

    for (i = 0; i < RDD_WLAN_MCAST_DHD_LIST_ENTRY_ARRAY_DHD_STATION_NUMBER; ++i)
    {
        RDD_WLAN_MCAST_DHD_LIST_ENTRY_DTS *dhd_list_entry_ptr = (RDD_WLAN_MCAST_DHD_LIST_ENTRY_DTS *)
            &dhd_list_table->entry[fwd_entry_index].dhd_station[i];
        uint32_t valid;

        RDD_WLAN_MCAST_DHD_LIST_ENTRY_VALID_READ(valid, dhd_list_entry_ptr);

        if (!valid)
        {
            __debug("DHD List ADD: entry %p (%u) -> value %u\n", dhd_list_entry_ptr, i, dhd_list_entry->index);

            RDD_WLAN_MCAST_DHD_LIST_ENTRY_VALID_WRITE(1, dhd_list_entry_ptr);
            RDD_WLAN_MCAST_DHD_LIST_ENTRY_INDEX_WRITE(dhd_list_entry->index, dhd_list_entry_ptr);

            list_size = i + 1;

            if (list_size > *dhd_list_size)
                *dhd_list_size = list_size;

            return BDMF_ERR_OK;
        }
    }

    return BDMF_ERR_NORES;
}

int rdd_wlan_mcast_dhd_list_entry_delete(uint32_t fwd_entry_index,
    RDD_WLAN_MCAST_DHD_LIST_ENTRY_DTS *dhd_list_entry, uint32_t *dhd_list_size)
{
    RDD_WLAN_MCAST_DHD_LIST_TABLE_DTS *dhd_list_table = wlan_mcast_dhd_list_table_g.virt_p;
    uint32_t i;

    for (i = 0; i < RDD_WLAN_MCAST_DHD_LIST_ENTRY_ARRAY_DHD_STATION_NUMBER; ++i)
    {
        RDD_WLAN_MCAST_DHD_LIST_ENTRY_DTS *dhd_list_entry_ptr = (RDD_WLAN_MCAST_DHD_LIST_ENTRY_DTS *)
            &dhd_list_table->entry[fwd_entry_index].dhd_station[i];
        uint32_t valid;
        uint32_t index;

        RDD_WLAN_MCAST_DHD_LIST_ENTRY_VALID_READ(valid, dhd_list_entry_ptr);
        RDD_WLAN_MCAST_DHD_LIST_ENTRY_INDEX_READ(index, dhd_list_entry_ptr);

        if (valid && (index == dhd_list_entry->index))
        {
            __debug("DHD List DELETE: entry %p (%u) -> value %u\n", dhd_list_entry_ptr, i, dhd_list_entry->index);

            RDD_WLAN_MCAST_DHD_LIST_ENTRY_VALID_WRITE(0, dhd_list_entry_ptr);
            RDD_WLAN_MCAST_DHD_LIST_ENTRY_INDEX_WRITE(0, dhd_list_entry_ptr);

            return BDMF_ERR_OK;
        }
    }

    return BDMF_ERR_NORES;
}

int rdd_wlan_mcast_dhd_list_entry_find(uint32_t fwd_entry_index,
    RDD_WLAN_MCAST_DHD_LIST_ENTRY_DTS *dhd_list_entry)
{
    RDD_WLAN_MCAST_DHD_LIST_TABLE_DTS *dhd_list_table = wlan_mcast_dhd_list_table_g.virt_p;
    uint32_t i;

    __debug("Entered, fwd_entry_index %d, dhd_list_entry->index = %d, dhd_list_entry->valid = %d\n",
        fwd_entry_index, dhd_list_entry->index, dhd_list_entry->valid);
    for (i = 0; i < RDD_WLAN_MCAST_DHD_LIST_ENTRY_ARRAY_DHD_STATION_NUMBER; ++i)
    {
        RDD_WLAN_MCAST_DHD_LIST_ENTRY_DTS *dhd_list_entry_ptr = (RDD_WLAN_MCAST_DHD_LIST_ENTRY_DTS *)
            &dhd_list_table->entry[fwd_entry_index].dhd_station[i];
        uint32_t valid;
        uint32_t index;

        RDD_WLAN_MCAST_DHD_LIST_ENTRY_VALID_READ(valid, dhd_list_entry_ptr);
        RDD_WLAN_MCAST_DHD_LIST_ENTRY_INDEX_READ(index, dhd_list_entry_ptr);

        if (valid && (index == dhd_list_entry->index))
        {
            __debug("DHD List FIND: entry %p (%u) -> value %u\n", dhd_list_entry_ptr, i, dhd_list_entry->index);

            return BDMF_ERR_OK;
        }
    }

    return BDMF_ERR_NORES;
}

void rdd_wlan_mcast_dhd_list_delete(uint32_t fwd_entry_index)
{
    uint32_t i;

    for (i = 0; i < RDD_WLAN_MCAST_DHD_LIST_ENTRY_ARRAY_DHD_STATION_NUMBER; ++i)
    {
        RDD_WLAN_MCAST_DHD_LIST_TABLE_DTS *dhd_list_table = wlan_mcast_dhd_list_table_g.virt_p;
        RDD_WLAN_MCAST_DHD_LIST_ENTRY_DTS *dhd_list_entry_ptr = (RDD_WLAN_MCAST_DHD_LIST_ENTRY_DTS *)
            &dhd_list_table->entry[fwd_entry_index].dhd_station[i];

        RDD_WLAN_MCAST_DHD_LIST_ENTRY_VALID_WRITE(0, dhd_list_entry_ptr);
    }
}

uint8_t rdd_wlan_mcast_dhd_list_scan(uint32_t fwd_entry_index,
    RDD_WLAN_MCAST_DHD_LIST_ENTRY_ARRAY_DTS *dhd_list_entry_array)
{
    RDD_WLAN_MCAST_DHD_LIST_TABLE_DTS *dhd_list_table = wlan_mcast_dhd_list_table_g.virt_p;
    uint8_t size = 0;
    uint32_t valid;
    int i;

    for (i = 0; i < RDD_WLAN_MCAST_DHD_LIST_ENTRY_ARRAY_DHD_STATION_NUMBER; ++i)
    {
        RDD_WLAN_MCAST_DHD_LIST_ENTRY_DTS *dhd_list_entry_ptr = (RDD_WLAN_MCAST_DHD_LIST_ENTRY_DTS *)
            &dhd_list_table->entry[fwd_entry_index].dhd_station[i];

        if (dhd_list_entry_array)
            dhd_list_entry_array->dhd_station[i] = dhd_list_table->entry[fwd_entry_index].dhd_station[i];

        RDD_WLAN_MCAST_DHD_LIST_ENTRY_VALID_READ(valid, dhd_list_entry_ptr);

        if (valid)
            (size)++;
    }

    return size;
}
#endif /* CONFIG_DHD_RUNNER */

/*
 *  Initialization API
 */

int rdd_wlan_mcast_init_common(wlan_mcast_dhd_list_table_t *table)
{
    /* Initialize the DHD FWD Table */
    if (table)
    {
        wlan_mcast_dhd_list_table_g.virt_p = table->virt_p;
        wlan_mcast_dhd_list_table_g.phys_addr = table->phys_addr;
    }
    else
    {
        wlan_mcast_dhd_list_table_g.virt_p = 0;
        wlan_mcast_dhd_list_table_g.phys_addr = 0;
    }
    return 0;
}

