/*
 * <:copyright-BRCM:2015:proprietary:standard
 * 
 *    Copyright (c) 2015 Broadcom 
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
 * :> 
 */

#include <bdmf_dev.h>
#include <rdpa_api.h>
#include "rdpa_int.h"
#include "rdpa_egress_tm_inline.h"
#include "rdd_qos_mapper.h"
#include "rdpa_pbit_to_queue_ex.h"

int pbit_to_queue_set_single_entry_ex(struct bdmf_object *mo, bdmf_boolean set_to_rdd,
    bdmf_object_handle obj, bdmf_index pbit, bdmf_index queue_id, bdmf_boolean link)
{
    pbit_to_queue_drv_priv_t *tbl = (pbit_to_queue_drv_priv_t *)bdmf_obj_data(mo);
    rdd_vport_id_t vport;
    bdmf_number channel_idx, flow_idx;
    rdpa_if port;
    int rc_id = 0, queue = 0;
    int rc = 0;
    bdmf_boolean mgmt;
    uint8_t tc_table = 0;
    bdmf_object_handle gem = NULL;
    rdpa_gem_flow_us_cfg_t gem_us_cfg;

    if (obj->drv == rdpa_port_drv())
    {
        rc = rdpa_port_index_get(obj, &port);
        if (rdpa_if_is_lan_or_wifi(port))
        {
            vport = rdpa_port_rdpa_if_to_vport(port);
            if (link)
                rc = _rdpa_egress_tm_lan_port_queue_to_rdd(port, queue_id, &rc_id, &queue);
            if (set_to_rdd)
                rc = rc ? rc : rdd_pbit_to_queue_entry_set((uint8_t)vport, rdpa_dir_ds, pbit, queue);
        }
        else if (rdpa_if_is_wan(port))
        {
            bdmf_link_handle us_link = NULL;
            bdmf_number tc_tbl;

            if (!rdpa_is_gbe_mode() && !rdpa_is_car_mode())
            {
                BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo,
                    "upstream table can be linked to port object only in GBE or CAR mode\n");
            }

            while ((us_link = bdmf_get_next_us_link(obj, us_link)))
            {
                if (bdmf_us_link_to_object(us_link)->drv == rdpa_tc_to_queue_drv())
                {
                    rdpa_tc_to_queue_table_get(bdmf_us_link_to_object(us_link), &tc_tbl);
                    tc_table = (uint8_t)tc_tbl;
                    break;
                }
            }

            if (rdpa_is_gbe_mode() || rdpa_is_epon_ae_mode())
            {
                channel_idx = port;

                if (link)
                    rc = _rdpa_egress_tm_channel_queue_to_rdd(rdpa_dir_us, channel_idx, queue_id, &rc_id, &queue);

                if (set_to_rdd)
                {
                    vport = rdpa_port_rdpa_if_to_vport(port);
                    rc = rc ? rc : rdd_pbit_to_queue_entry_set((uint8_t)vport, rdpa_dir_us, pbit, queue);
                }
            }
            else if (rdpa_is_car_mode())
            {
                /* Traverse configured gems and assign the table */
                while ((gem = bdmf_get_next(rdpa_gem_drv(), gem, NULL)))
                {
                    rdpa_gem_us_cfg_get(gem, &gem_us_cfg);
                    rdpa_tcont_management_get(gem_us_cfg.tcont, &mgmt);
                    if (mgmt)
                        continue;

                    rdpa_tcont_index_get(gem_us_cfg.tcont, &channel_idx);
                    if (link)
                    {
                        rc = _rdpa_egress_tm_channel_queue_to_rdd(rdpa_dir_us, channel_idx,
                            queue_id, &rc_id, &queue);
                    }

                    if (set_to_rdd)
                    {
                        rdpa_gem_index_get(gem, &flow_idx);
                        rc = rc ? rc : rdd_pbit_to_queue_entry_set(flow_idx, rdpa_dir_us, pbit, queue);

                        BDMF_TRACE_DBG_OBJ(mo, "Configure US pbit %d to queue_id %d"
                            " (q,rc: %d %d) pbit_table %d tc_table %d in RDD", (int)pbit, (int)queue_id, 
                            queue, rc_id, (int)tbl->index, tc_table);
                    }

                    if (rc)
                    {
                        bdmf_put(gem);
                        break;
                    }
                }
            }
        }
    }
    else if (obj->drv == rdpa_tcont_drv())
    {
        /* OMCI management tcont is not a typical tcont*/
        rdpa_tcont_management_get(obj, &mgmt);
        if (mgmt)
            BDMF_TRACE_RET(BDMF_ERR_PERM, "Can't link pbit to queue to management tcont");

        rdpa_tcont_index_get(obj, &channel_idx);
        if (link)
            rc = _rdpa_egress_tm_channel_queue_to_rdd(rdpa_dir_us, channel_idx , queue_id, &rc_id, &queue);

        if (set_to_rdd)
        {
            /* Walk over all gems that points on that tcont */
            if (!rc)
            {
                while ((gem = bdmf_get_next(rdpa_gem_drv(), gem, NULL)))
                {
                    rdpa_gem_us_cfg_get(gem, &gem_us_cfg);
                    if (gem_us_cfg.tcont == obj)
                    {
                        rdpa_gem_index_get(gem, &flow_idx);
                        rc = rc ? rc : rdd_pbit_to_queue_entry_set(flow_idx, rdpa_dir_us, pbit, queue);
                    }
                }
            }
        }
    }
    else if (obj->drv == rdpa_llid_drv())
    {
        queue_id_info_t queue_id_info = {};
        llid_drv_priv_t *llid = (llid_drv_priv_t *)bdmf_obj_data(obj);

        if (!llid)
            BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "failed to get llid object");

        if (link)
        {
            rc = egress_tm_queue_id_info_get(rdpa_dir_us, llid->index , queue_id, &queue_id_info);
            BDMF_TRACE_INFO("llid->index=%d, queue_id=%d : [rc_id=%d, queue=%d, channel=%d]\n", 
                (int)llid->index, (int)queue_id, (int)queue_id_info.rc_id, (int)queue_id_info.queue,
                (int)queue_id_info.channel);
        }

        if (rc)
            BDMF_TRACE_RET_OBJ(rc, mo, "failed to get llid egress_tm for llid->index=%d", (int)llid->index);

        if (set_to_rdd)
        {
            BDMF_TRACE_INFO("llid->index=%d, queue_id=%d : [rc_id=%d, queue=%d, channel=%d]\n", 
                (int)llid->index, (int)queue_id, (int)queue_id_info.rc_id, (int)queue_id_info.queue,
                (int)queue_id_info.channel);
            rc = rdd_pbit_to_queue_entry_set((uint8_t)llid->index, rdpa_dir_us, pbit, queue_id_info.queue);
            if (!rc && (_rdpa_epon_mode_get() == rdpa_epon_ctc || _rdpa_epon_mode_get() == rdpa_epon_cuc))
            {
                for (channel_idx = 0; channel_idx < (llid->num_channels + 1); channel_idx++)
                {
                    rdd_qos_mapper_set_table_id_to_tx_flow(llid->index, llid->channels[channel_idx]);                
                }
            }
        }
    }

    if (rc)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "Can't set table index %d pbit %d to queue %d. err %d\n",
            (int)tbl->index, (int)pbit, (int)queue_id, rc);
    }
    return 0;
}


