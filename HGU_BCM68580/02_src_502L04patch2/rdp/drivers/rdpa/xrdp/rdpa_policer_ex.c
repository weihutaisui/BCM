/*
* <:copyright-BRCM:2013-2015:proprietary:standard
* 
*    Copyright (c) 2013-2015 Broadcom 
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

#include "bdmf_dev.h"
#include "rdpa_api.h"
#include "rdpa_int.h"
#include "rdp_drv_policer.h"
#include "rdpa_policer_ex.h"
#include "rdp_drv_cnpl.h"
#include "rdd_data_structures_auto.h"

static struct bdmf_object *global_policer_objects[RDPA_TM_MAX_POLICER];
static bdmf_boolean is_group_initialized;

int policer_pre_init_ex(policer_drv_priv_t *policer)
{
    int rc = 0;
    
    if (!is_group_initialized)
        rc = drv_policer_group_init();

    is_group_initialized = 1;

    return rc;
}

int policer_post_init_ex(struct bdmf_object *mo)
{
    policer_drv_priv_t *policer = (policer_drv_priv_t *)bdmf_obj_data(mo);
    int rc = 0;

    /* If policer index is set - make sure it is unique.
     * Otherwise, assign free
     */
    if (policer->index < 0)
    {
        int i;

        /* Find and assign free index */
        for (i = 0; i < RDPA_TM_MAX_POLICER; i++)
        {
            if (!global_policer_objects[i])
            {
                policer->index = i;
                break;
            }
        }
    }
    if ((unsigned)policer->index >= RDPA_TM_MAX_POLICER)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Too many policers or index %ld is out of range\n", policer->index);
    /* set object name */
    snprintf(mo->name, sizeof(mo->name), "policer/dir=%s,index=%ld",
        bdmf_attr_get_enum_text_hlp(&rdpa_traffic_dir_enum_table, policer->dir), policer->index);
    if (global_policer_objects[policer->index])
        BDMF_TRACE_RET(BDMF_ERR_ALREADY, "%s already exists\n", mo->name);

    rc = policer_rdd_update(mo, &policer->cfg);
    if (rc < 0)
        return rc;

    global_policer_objects[policer->index] = mo;

    return 0;
}

void _rdpa_policer_factor_bytes_cfg(bdmf_index policer_index, rdpa_policer_factor_bytes rdpa_factor_bytes)
{
    rdp_policer_factor_bytes rdp_factor_bytes;

    /* remap factor_bytes */
    switch (rdpa_factor_bytes)
    {
    case rdpa_policer_factor_bytes_0:
        rdp_factor_bytes = rdp_policer_factor_bytes_0;
        break;

    case rdpa_policer_factor_bytes_neg_8:
        rdp_factor_bytes = rdp_policer_factor_bytes_neg_8;
        break;

    default:
        rdp_factor_bytes = rdpa_factor_bytes;
    }
    RDD_POLICER_PARAMS_ENTRY_FACTOR_BYTES_WRITE_G(rdp_factor_bytes, RDD_POLICER_PARAMS_TABLE_ADDRESS_ARR, policer_index);
}

int policer_rdd_update(struct bdmf_object *mo, rdpa_tm_policer_cfg_t *cfg)
{
    policer_drv_priv_t *policer = (policer_drv_priv_t *)bdmf_obj_data(mo);
    int rc;
    policer_cfg_t policer_cfg = {};
    uint32_t min_burst_size, max_burst_size;
    
    if (cfg == NULL)
        return BDMF_ERR_PARM;

    if (cfg->commited_rate < RDPA_POLICER_MIN_SR || cfg->commited_rate > RDPA_POLICER_MAX_SR)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Committed rate is invalid\n");

    drv_cnpl_policer_max_cbs_get(cfg->commited_rate, &max_burst_size);

    if (cfg->committed_burst_size > max_burst_size)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Max. committed burst size is %dB\n", max_burst_size);
    
    drv_cnpl_policer_min_cbs_get(cfg->commited_rate, &min_burst_size);

    if (cfg->committed_burst_size < min_burst_size)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Committed burst size must be larger than %dB\n", min_burst_size);
    
    if ((cfg->type == rdpa_tm_policer_sr_overflow_dual_token_bucket) ||
        (cfg->type == rdpa_tm_policer_tr_dual_token_bucket) ||
        (cfg->type == rdpa_tm_policer_tr_overflow_dual_token_bucket))
    {
        if ((cfg->type == rdpa_tm_policer_tr_dual_token_bucket) || (cfg->type == rdpa_tm_policer_tr_overflow_dual_token_bucket))
        {
            drv_cnpl_policer_max_cbs_get(cfg->peak_rate, &max_burst_size);
            drv_cnpl_policer_min_cbs_get(cfg->peak_rate, &min_burst_size);
        }
        if (cfg->peak_burst_size > max_burst_size)
            BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Max. peak burst size is %dB\n", max_burst_size);

        if (cfg->peak_burst_size < min_burst_size)
            BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Peak burst size must be larger than %dB\n", min_burst_size);
    }

    policer_cfg.commited_rate = cfg->commited_rate;
    policer_cfg.committed_burst_size = cfg->committed_burst_size * 8;

    switch (cfg->type)
    {
    case rdpa_tm_policer_sr_overflow_dual_token_bucket:
        policer_cfg.peak_burst_size = cfg->peak_burst_size * 8;
        policer_cfg.overflow = 1;            
        break;

    case rdpa_tm_policer_tr_overflow_dual_token_bucket:
        policer_cfg.overflow = 1;            
    case rdpa_tm_policer_tr_dual_token_bucket:
        policer_cfg.peak_rate = cfg->peak_rate;
        policer_cfg.peak_burst_size = cfg->peak_burst_size * 8;
        break;

    case rdpa_tm_policer_single_token_bucket:
    default: 
        break;
    }

    policer_cfg.index = policer->index;
    policer_cfg.is_dual = 1; /* CNPL HW group configured as dual anyway */
    rc = drv_cnpl_policer_set(&policer_cfg);
    if (rc)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Failed to configure RDD policer %d, error %d\n",
                           (int)policer->index, rc);
    }

    RDD_POLICER_PARAMS_ENTRY_DEI_MODE_WRITE_G(cfg->dei_mode, RDD_POLICER_PARAMS_TABLE_ADDRESS_ARR, policer->index);
    _rdpa_policer_factor_bytes_cfg(policer->index, cfg->factor_bytes);
    return 0;
}

void policer_destroy_ex(struct bdmf_object *mo)
{
    int rc = 0;
    policer_drv_priv_t *policer = (policer_drv_priv_t *)bdmf_obj_data(mo);
    int max_policers = RDPA_TM_MAX_POLICER;

    if ((unsigned)policer->index >= max_policers || global_policer_objects[policer->index] != mo)
        return;

    rc = drv_cntr_counter_clr(POLICER_CNTR_GROUP_ID, policer->index);
    if (rc)
        BDMF_TRACE_ERR("Cannot clear Policer counter %d; err: %d\n", (int)policer->index, rc);

    /* Ariel does all objectes which referanced to destroyed policer object are updated ???? */
    global_policer_objects[policer->index] = NULL;
}


int policer_attr_stat_read_ex(policer_drv_priv_t *policer, rdpa_tm_policer_stat_t *stat)
{
    int rc = 0;
    uint32_t rx_cntr_arr[MAX_NUM_OF_COUNTERS_PER_READ] = {};

    rc = drv_cntr_counter_read(POLICER_CNTR_GROUP_ID, policer->index, rx_cntr_arr);
    if (rc)
        BDMF_TRACE_ERR("Could not read Policer counters for context %d. err: %d\n", (int)policer->index, rc);

    memset(stat, 0, sizeof(rdpa_tm_policer_stat_t));

    stat->green.packets = rx_cntr_arr[0];
    stat->green.bytes = rx_cntr_arr[1];
    stat->yellow.packets = rx_cntr_arr[2];
    stat->yellow.bytes = rx_cntr_arr[3];
    stat->red.packets = rx_cntr_arr[4];
    stat->red.bytes = rx_cntr_arr[5];

    return rc;
}

int policer_attr_stat_write_ex(policer_drv_priv_t *policer)
{
    int rc = 0;

    rc = drv_cntr_counter_clr(POLICER_CNTR_GROUP_ID, policer->index);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Could not clear Policer counters: %ld\n", policer->index);

    return rc;
}

int rdpa_policer_get_ex(const rdpa_policer_key_t *_key_, bdmf_object_handle *_obj_)
{
    return rdpa_obj_get(global_policer_objects, RDPA_TM_MAX_POLICER, _key_->index, _obj_);
}
