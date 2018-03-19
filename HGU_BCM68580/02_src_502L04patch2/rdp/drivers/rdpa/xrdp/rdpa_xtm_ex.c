/*
* <:copyright-BRCM:2012-2015:proprietary:standard
* 
*    Copyright (c) 2012-2015 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
 :>
*/

/*
 * rdpa_xtm_ex.c
 *
 *  Created on: 2017
 *      Author: srinies
 */

#include "rdd.h"
#include <bdmf_dev.h>
#include "rdpa_api.h"
#include "rdpa_common.h"
#include "rdpa_int.h"
#include "rdpa_platform.h"
#include "rdp_drv_bbh_rx.h"
#include "rdd_runner_proj_defs.h"
#include "rdd_tcam_ic.h"
#include "rdp_drv_bbh_tx.h"
#include "rdpa_xtm_ex.h"
#include "rdp_drv_proj_cntr.h"
#include "xrdp_drv_qm_ag.h"


/* "us_cfg" attribute "write" callback. */
int rdpa_us_wan_flow_config(uint32_t      wan_flow,
                            int           wan_channel,
                            uint32_t      wan_port_or_fstat,
                            bdmf_boolean  crc_calc,
                            int           ptm_bonding,
                            uint8_t       pbits_to_queue_table_index,
                            uint8_t       traffic_class_to_queue_table_index,
                            bdmf_boolean  enable)
{
    int rc = BDMF_ERR_OK;
    uint32_t data = 0;
    uint32_t cntr_id;

    rdd_tx_flow_enable(wan_flow, rdpa_dir_us, enable);

    if (enable)
    {
        data = wan_port_or_fstat || (crc_calc << 16);
        rc = ag_drv_bbh_tx_wan_configurations_flow2port_set(BBH_TX_ID_DSL, data, wan_flow, 1);
        rdpa_cntr_id_alloc(TX_FLOW_CNTR_GROUP_ID, &cntr_id);
        rdd_tm_flow_cntr_cfg(wan_flow, cntr_id);
    }
    else
    {
        rdpa_cntr_id_dealloc(TX_FLOW_CNTR_GROUP_ID, NONE_CNTR_SUB_GROUP_ID, wan_flow);
        rdd_tm_flow_cntr_cfg(wan_flow, TX_FLOW_CNTR_GROUP_INVLID_CNTR);
    }
    return rc;
}

int rdpa_wan_dsl_channel_base(void)
{
   return 0; /* until the multi-wan support */
}

int rdpa_flow_pm_counters_get(int index, rdpa_xtmflow_stat_t *stat)
{
   int rc = 0;

   /* TODO */
   /* US */
#if 0
   rc = rdd_flow_pm_counters_get(index, BL_LILAC_RDD_FLOW_PM_COUNTERS_BOTH, 0, &rdd_flow_counters);

   if (rc)
      BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Could not read RDD flow counters for xtmflow %ld\n", xtmflow->index);

   stat->tx_packets = rdd_flow_counters.good_tx_packet;               
   stat->tx_bytes = rdd_flow_counters.good_tx_bytes;               
   stat->tx_packets_discard = rdd_flow_counters.error_tx_packets_discard;     
#else
   stat->tx_packets = 0;
   stat->tx_bytes =  0;
   stat->tx_packets_discard = 0;
#endif

   return rc;
}
