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
#include "rdpa_common.h"
#include "rdd.h"
#include "rdpa_int.h"
#include "rdpa_egress_tm_inline.h"
#include "rdpa_rdd_inline.h"
#include "rdd_qos_mapper.h"
#include "rdpa_tc_to_queue_ex.h"

bdmf_object_handle ds_tc_to_queue_objects[RDPA_TC_TO_QUEUE_ID_MAX_TABLES / 2];
bdmf_object_handle us_tc_to_queue_objects[RDPA_TC_TO_QUEUE_ID_MAX_TABLES / 2];

void rdpa_tc_to_queue_obj_init_ex(bdmf_object_handle **container, int *max_tables, uint8_t *table_size, rdpa_traffic_dir dir)
{
    if (!*table_size)
        *table_size = RDPA_BS_TC_TO_QUEUE_TABLE_SIZE;
    *max_tables = RDPA_TC_TO_QUEUE_ID_MAX_TABLES / 2;
    *container = (dir == rdpa_dir_ds) ? ds_tc_to_queue_objects : us_tc_to_queue_objects;
}
static bdmf_error_t _rdpa_tc_to_queue_set_gbe_or_car_entry(struct bdmf_object *mo, bdmf_boolean set_to_rdd,
        bdmf_object_handle port_obj, bdmf_index tc, bdmf_index queue, bdmf_boolean link)
{
    tc_to_queue_drv_priv_t *tbl = (tc_to_queue_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_if port_idx;
    bdmf_object_handle gem = NULL;
    bdmf_number channel_idx, gem_index;
    bdmf_error_t rc = BDMF_ERR_OK;
    bdmf_boolean mgmt;
    rdpa_gem_flow_us_cfg_t gem_us_cfg;
    rdd_vport_id_t rdd_src_port;
    uint16_t qm_queue = 0;
    int rc_id = 0;

    rdpa_port_index_get(port_obj, &port_idx);
    if (rdpa_is_gbe_mode())
    {
        if (link)
            rc = _rdpa_egress_tm_channel_queue_to_rdd(rdpa_dir_us, port_idx , queue,
                    &rc_id, (int *)&qm_queue);
        if (set_to_rdd)
        {
            rdd_src_port = rdpa_port_rdpa_if_to_vport(port_idx);
            rc = rc ? rc : rdd_tc_to_queue_entry_set((uint8_t)rdd_src_port, tbl->dir,
                    &(tbl->size), (uint8_t)tc, qm_queue);
        }
    }
    else /* car mode */
    {
        if (tbl->size == RDPA_CS_TC_TO_QUEUE_TABLE_SIZE)
            BDMF_TRACE_RET(BDMF_ERR_NORES, "Can't configure tc_to_queue of size %d in Car Mode", RDPA_CS_TC_TO_QUEUE_TABLE_SIZE);
        /* walk over all configured gems and assign a table */
        while ((gem = bdmf_get_next(rdpa_gem_drv(), gem, NULL)))
        {
            rdpa_gem_us_cfg_get(gem, &gem_us_cfg);
            rdpa_gem_index_get(gem, &gem_index);
            rdpa_tcont_management_get(gem_us_cfg.tcont, &mgmt);
            if (mgmt)
                continue;
            rdpa_tcont_index_get(gem_us_cfg.tcont, &channel_idx);
            if (link)
            {
                rc = _rdpa_egress_tm_channel_queue_to_rdd(rdpa_dir_us, channel_idx, queue,
                        &rc_id, (int *)&qm_queue);
            }
            if (set_to_rdd)
            {
                rc = rc ? rc : rdd_tc_to_queue_entry_set((uint8_t)gem_index, tbl->dir,
                        &(tbl->size), (uint8_t) tc, qm_queue);
            }
            if (rc)
            {
                bdmf_put(gem);
                break;
            }
        }
    }
    return rc;
}

static bdmf_error_t _rdpa_tc_to_queue_set_us_entry_ex(struct bdmf_object *mo, bdmf_boolean set_to_rdd,
    bdmf_object_handle obj, bdmf_index tc, bdmf_index queue, bdmf_boolean link)
{
    tc_to_queue_drv_priv_t *tbl = (tc_to_queue_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_gem_flow_us_cfg_t gem_us_cfg;
    bdmf_number channel_idx, gem_index;
    uint16_t qm_queue = 0;
    bdmf_error_t rc = BDMF_ERR_OK;
    int rc_id = 0;

    if (obj->drv == rdpa_tcont_drv())
    {
        rdpa_tcont_index_get(obj, &channel_idx);

        if (link)
        {
            rc = _rdpa_egress_tm_channel_queue_to_rdd(rdpa_dir_us, channel_idx, queue,
                    &rc_id, (int *)&qm_queue);
        }
        if (rc)
            BDMF_TRACE_RET(rc, "Can't get rdd tm channel channel_idx=%d queue=%d", (int)channel_idx, (int)queue);
        if (set_to_rdd)
        {
            bdmf_object_handle gem = NULL;
            while ((gem = bdmf_get_next(rdpa_gem_drv(), gem, NULL)))
            {
                rdpa_gem_us_cfg_get(gem, &gem_us_cfg);
                if (gem_us_cfg.tcont == obj)
                {
                    rdpa_gem_index_get(gem, &gem_index);
                    rc = rc ? rc : rdd_tc_to_queue_entry_set((uint8_t)gem_index, tbl->dir,
                        &(tbl->size), (uint8_t)tc, qm_queue);
                    if (!rc)
                    {
                        bdmf_put(gem);
                        BDMF_TRACE_RET(rc, "Can't set US rdd tc to queue entry");
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
            rc = egress_tm_queue_id_info_get(rdpa_dir_us, llid->index , queue, &queue_id_info);
            BDMF_TRACE_INFO("llid->index=%d, queue=%d : [rc_id=%d, queue=%d, channel=%d]\n", 
                (int)llid->index, (int)queue, (int)queue_id_info.rc_id, (int)queue_id_info.queue,
                (int)queue_id_info.channel);
        }

        if (rc)
            BDMF_TRACE_RET_OBJ(rc, mo, "failed to get llid egress_tm for llid->index=%d", (int)llid->index);

        if (set_to_rdd)
        {
            BDMF_TRACE_INFO("llid->index=%d, queue=%d : [rc_id=%d, queue=%d, channel=%d]\n", 
                (int)llid->index, (int)queue, (int)queue_id_info.rc_id, (int)queue_id_info.queue,
                (int)queue_id_info.channel);
            rc = rdd_tc_to_queue_entry_set((uint8_t)llid->index, tbl->dir, &(tbl->size), (uint8_t)tc, queue_id_info.queue);
            if (!rc && (_rdpa_epon_mode_get() == rdpa_epon_ctc || _rdpa_epon_mode_get() == rdpa_epon_cuc))
            {
                for (channel_idx = 0; channel_idx < (llid->num_channels + 1); channel_idx++)
                {
                    rdd_qos_mapper_set_table_id_to_tx_flow(llid->index, llid->channels[channel_idx]);                
                }
            }
        }
    }

    return rc;
}

int rdpa_tc_to_queue_set_single_entry_ex(struct bdmf_object *mo, bdmf_boolean set_to_rdd,
    bdmf_object_handle other, bdmf_index tc, bdmf_index queue, bdmf_boolean link)
{
    tc_to_queue_drv_priv_t *tbl = (tc_to_queue_drv_priv_t *)bdmf_obj_data(mo);
    /* TODIO: will not work in US direction - consider using illegal qm queue offset (>128) and change FW */
    int qm_queue = QM_QUEUE_DROP;
    int rc = 0;

    if (tbl->dir == rdpa_dir_ds)
    {
        port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(other);

        if (port->index == rdpa_if_switch)
            /* TODO: implement later - in bridge flow */
            /*rc = set_switch_port(mo, set_to_rdd, link, tc, queue, 0, rdpa_tc_to_queue);*/
            rc = BDMF_ERR_OK;
        else
        {
            if (link)
            {
                if (!rdpa_if_is_cpu_port(port->index))
                {
                    int rc_id;
                    rc = _rdpa_egress_tm_lan_port_queue_to_rdd(port->index, queue, &rc_id, &qm_queue);
                    /* No rate controller in DS direction */
                }
                else
                {
                    /* TODO: should be inspected upon WIFI implementation */
                }
            }
            if (set_to_rdd)
            {
                rdd_vport_id_t rdd_src_port = rdpa_port_rdpa_if_to_vport(port->index);
                rc = rc ? rc : rdd_tc_to_queue_entry_set(rdd_src_port, tbl->dir,
                    &(tbl->size), (uint8_t)tc, qm_queue);
            }
        }
    }
    else /*handle upstream case */
    {
        /* TODO: add scheduler type resolution and validation */
        /* handle car mode */
        if (other->drv == rdpa_port_drv())
            rc = _rdpa_tc_to_queue_set_gbe_or_car_entry(mo, set_to_rdd,
                    other, tc, queue, link);
        else
            rc = _rdpa_tc_to_queue_set_us_entry_ex(mo, set_to_rdd,
                    other, tc, queue, link);
    }
    if (rc)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "Can't set table index %d dir %s tc %d to queue %d. err %d\n",
            (int)tbl->index, (tbl->dir == rdpa_dir_ds) ? "ds" : "us", (int)tc, (int)queue, rc);
    }
    return rc;
}

void rdpa_tc_to_queue_unlink_port_ex(struct bdmf_object *mo, struct bdmf_object *other, bdmf_number port)
{
    tc_to_queue_drv_priv_t *tbl = (tc_to_queue_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_error_t rc = BDMF_ERR_OK;
    bdmf_object_handle gem = NULL;
    rdpa_gem_flow_us_cfg_t gem_us_cfg;
    bdmf_number gem_port;
    rdpa_traffic_dir dir = tbl->dir;

    if (dir == rdpa_dir_ds || rdpa_is_gbe_mode())
    {
        rdd_qos_mapper_invalidate_table((uint16_t)rdpa_port_rdpa_if_to_vport(port), tbl->dir, tbl->size);
        if (rc)
        {
            BDMF_TRACE_ERR("Can't invalidate TC to queue table for port: %d\n",
                (uint16_t)rdpa_port_rdpa_if_to_vport(port));
        }
    }
    else
    {
        while ((gem = bdmf_get_next(rdpa_gem_drv(), gem, NULL)))
        {
            rdpa_gem_us_cfg_get(gem, &gem_us_cfg);
            if (gem_us_cfg.tcont == other)
            {
                rdpa_gem_gem_port_get(gem, &gem_port);
                rdd_qos_mapper_invalidate_table((uint16_t)gem_port, tbl->dir, tbl->size);
            }
        }
    }
}

bdmf_error_t rdpa_tc_to_queue_realloc_table_ex(int *tc_to_queue_linked_tcont_llid, tc_to_queue_drv_priv_t *tbl)
{
    /* check if table size should be increased , if so - allocate new, larger tables for all TCONTS/LLIDS */
    /* TODO : move to a function in ex files */
    /* TODO - for each TCONT - find wan_flow (gem) */
#if 0
    if (index >= tbl->size)
    {
        for (inst = 0; inst < RDPA_MAX_TCONT; inst++)
        {
            if (tc_to_queue_linked_tcont_llid[inst] == tbl->index)
            {
                rc = rdd_realloc_tc_to_queue_table(tbl->index, inst, tbl->dir, &(tbl->size));
                if (rc)
                    return rc;
            }
        }
    }
#endif
    return BDMF_ERR_OK;
}


