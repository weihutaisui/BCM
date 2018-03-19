/*
   Copyright (c) 2015 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard

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

#include "rdd_ghost_reporting.h"
#include "rdd_runner_proj_defs.h"
#include "rdp_drv_rnr.h"
#include "XRDP_AG.h"

bdmf_error_t rdd_ghost_reporting_timer_set(void)
{
    RDD_GHOST_REPORTING_GLOBAL_CFG_DTS *entry;
    bdmf_error_t rc = BDMF_ERR_OK;

    RDD_BTRACE("\n");

    entry = RDD_GHOST_REPORTING_GLOBAL_CFG_PTR(get_runner_idx(reporting_runner_image));

    RDD_GHOST_REPORTING_GLOBAL_CFG_ENTRY_TIMER_INTERVAL_WRITE(GHOST_REPORTING_TIMER_INTERVAL, entry);

    /* Make sure the timer configured before the cpu weakeup */
    WMB();

    rc = ag_drv_rnr_regs_cfg_cpu_wakeup_set(get_runner_idx(reporting_runner_image), REPORTING_THREAD_NUMBER);

    return rc;
}

bdmf_error_t rdd_ghost_reporting_mapping_queue_to_wan_channel(uint8_t queue_id, uint8_t wan_channel)
{
    RDD_REPORTING_QUEUE_DESCRIPTOR_DTS *entry;
    uint16_t counter_ptr;

    RDD_BTRACE("queue_id = %d, wan_channel = %d\n", queue_id, wan_channel);

    if ((queue_id >= RDD_REPORTING_QUEUE_DESCRIPTOR_TABLE_SIZE) ||
        (wan_channel >= RDD_REPORTING_COUNTER_TABLE_SIZE))
    {
        return BDMF_ERR_PARM;
    }

    entry = ((RDD_REPORTING_QUEUE_DESCRIPTOR_DTS *)RDD_REPORTING_QUEUE_DESCRIPTOR_TABLE_PTR(get_runner_idx(reporting_runner_image))) + queue_id;
    counter_ptr = REPORTING_COUNTER_ADDRESS + (wan_channel * sizeof(RDD_BYTES_4_DTS));
    RDD_REPORTING_QUEUE_DESCRIPTOR_COUNTER_PTR_WRITE(counter_ptr, entry);

    return BDMF_ERR_OK;
}

void rdd_ghost_reporting_mac_type_init(uint8_t mac_type)
{
    RDD_GHOST_REPORTING_GLOBAL_CFG_DTS *entry;

    RDD_BTRACE("mac_type = %d\n", mac_type);

    entry = RDD_GHOST_REPORTING_GLOBAL_CFG_PTR(get_runner_idx(reporting_runner_image));
    RDD_GHOST_REPORTING_GLOBAL_CFG_ENTRY_MAC_TYPE_WRITE(mac_type, entry);
}

/* RDD debug function */
bdmf_error_t rdd_ghost_reporting_ctr_get(uint8_t wan_channel, uint32_t *report)
{
    RDD_BYTES_4_DTS *entry;

    RDD_BTRACE("wan_channel = %d, report = %p\n", wan_channel, report);

    if ((wan_channel >= RDD_REPORTING_COUNTER_TABLE_SIZE) ||
        (report == NULL))
    {
        return BDMF_ERR_PARM;
    }

    entry = (RDD_BYTES_4_DTS *)RDD_REPORTING_COUNTER_TABLE_PTR(get_runner_idx(reporting_runner_image)) + wan_channel;
    RDD_BYTES_4_BITS_READ(*report, entry);

    return BDMF_ERR_OK;
}

