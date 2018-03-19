/*
    <:copyright-BRCM:2015-2016:DUAL/GPL:standard
    
       Copyright (c) 2015-2016 Broadcom 
       All Rights Reserved
    
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

#include <bdmf_data_types.h>
#include <bdmf_errno.h>
#include "rdp_drv_policer.h"
#include "rdp_drv_proj_cntr.h"
#include "rdp_drv_proj_policer.h"

#define POLICER_PARAM_BUCKET_SIZE  4  /* 4 bytes */

typedef struct {
    uint32_t bucket_0;                /* parameters of bucket 0 */
} cnpl_single_policer_param_cfg_t;

typedef struct {
    uint32_t bucket_0;                /* parameters of bucket 0 */
    uint32_t bucket_1;                /* parameters of bucket 1 */
} cnpl_dual_policer_param_cfg_t;

bdmf_error_t drv_policer_group_init(void)
{
    int rc = BDMF_ERR_OK;
    cnpl_policer_cfg policer_cfg;

    RDD_BTRACE("Policer base_address = 0x%x; Param base_address = 0x%x; CNPL end address = 0x%x\n", (CNPL_POLICER_BASE_ADDR <<3), 
               (CNPL_POLICER_PARAM_BASE_ADDR <<3), (CNPL_MEMROY_END_ADDR <<3));

    /* first group */
    policer_cfg.bk_ba = CNPL_POLICER_BASE_ADDR;
    policer_cfg.pa_ba = CNPL_POLICER_PARAM_BASE_ADDR;
    policer_cfg.pl_double = 1;
    policer_cfg.pl_st = 0;
    policer_cfg.pl_end = CNPL_POLICER_NUM - 1;

    rc = ag_drv_cnpl_policer_cfg_set(CNPL_GROUP_DUAL_BUCKET_INDEX, &policer_cfg);

    /* second group. not used, but need to be defined with false policer number */
    policer_cfg.bk_ba = 0;
    policer_cfg.pa_ba = 0;
    policer_cfg.pl_double = 0;
    policer_cfg.pl_st = CNPL_POLICER_NUM;
    policer_cfg.pl_end = CNPL_POLICER_NUM;

    rc = rc ? rc : ag_drv_cnpl_policer_cfg_set(CNPL_GROUP_ONE_INDEX, &policer_cfg);

    rc = rc ? rc : ag_drv_cnpl_policers_configurations_per_up_set(drv_cnpl_periodic_update_us_to_n_get(CNPL_PERIODIC_UPDATE_US), 1);
    return rc;
}

void _drv_policer_shift_size_get(unsigned long burst_size, uint32_t rate_to_alloc_unit, uint32_t  *size_mult, uint32_t *shift_size)
{
    for (*shift_size = 3; *shift_size >= 0; (*shift_size)--)
    {
        *size_mult = burst_size << *shift_size;
        do_div(*size_mult, rate_to_alloc_unit);
        /* check if size_mult is valid using the specific shift_size */
        if ((*size_mult > 1) && (*size_mult <= 15))
            return;
    }
}

static inline uint32_t _drv_policer_rate_to_alloc_unit(unsigned long rate_bps, uint32_t period_us)
{
    unsigned long divided = rate_bps + (CNPL_SECOND_TO_US / period_us) / 2;
    uint32_t divider = CNPL_SECOND_TO_US / period_us;

    do_div(divided, divider); 
    return (uint32_t)divided;
}

bdmf_error_t drv_cnpl_policer_set(policer_cfg_t* policer_cfg)
{
    int rc = BDMF_ERR_OK;
    cnpl_policer_cfg policer_group_cfg;
    uint32_t policer_group, policer_param_offset, shift_size, policer_type_vector, 
        commited_rate_to_alloc_unit, peak_rate_to_alloc_unit, size_mult;
    cnpl_dual_policer_param_cfg_t cnpl_policer_param = {};  

    if(policer_cfg->is_dual)
        policer_group = CNPL_GROUP_DUAL_BUCKET_INDEX;
    else
        policer_group = CNPL_GROUP_ONE_INDEX;
    rc = ag_drv_cnpl_policer_cfg_get(policer_group, &policer_group_cfg);
    if (rc)
        return BDMF_ERR_INVALID_OP;
 
    /* set policer algorithim */
    ag_drv_cnpl_policers_configurations_pl_calc_type_get(policer_cfg->index/32, &policer_type_vector);
    if (policer_cfg->peak_rate && !policer_cfg->overflow)
        policer_type_vector = policer_type_vector | (1 << (policer_cfg->index%32));
    else
        policer_type_vector = policer_type_vector & ~(1 << (policer_cfg->index%32));
    ag_drv_cnpl_policers_configurations_pl_calc_type_set(policer_cfg->index/32, policer_type_vector);

    /* policer bucket parameters */
    /* bkt_rate  |  bkt_size_mult | bkt_shift_size | overflow |        */  
    /* 24b[31:8] |   4b[7:4]      | 2b[3:2]        | 1b[1]    | 1b[0]  */ 
    /* first bucket */

    commited_rate_to_alloc_unit = _drv_policer_rate_to_alloc_unit(policer_cfg->commited_rate, CNPL_PERIODIC_UPDATE_US);
    cnpl_policer_param.bucket_0 =  commited_rate_to_alloc_unit <<8; 

    _drv_policer_shift_size_get(policer_cfg->committed_burst_size, commited_rate_to_alloc_unit, &size_mult, &shift_size);

    cnpl_policer_param.bucket_0 |=  (size_mult & 0xF) <<4;
    cnpl_policer_param.bucket_0 |=  shift_size << 2;
    cnpl_policer_param.bucket_0 |=  policer_cfg->overflow <<1;

    /* second bucket */
    if(policer_group_cfg.pl_double)
    {
        /* if no rates -> single bucket */
        if (policer_cfg->peak_rate || policer_cfg->peak_burst_size)
        {
            if (policer_cfg->peak_rate)
            {
                peak_rate_to_alloc_unit = _drv_policer_rate_to_alloc_unit(policer_cfg->peak_rate, CNPL_PERIODIC_UPDATE_US);
                cnpl_policer_param.bucket_1 =  peak_rate_to_alloc_unit <<8; 

                _drv_policer_shift_size_get(policer_cfg->peak_burst_size, peak_rate_to_alloc_unit, &size_mult, &shift_size);
                cnpl_policer_param.bucket_1 |=  (size_mult & 0xF) <<4;
                /* for second bucket rate configuration use peak_rate */
                cnpl_policer_param.bucket_1 |=  1 << 1;
            }
            else
            {
                /* configured as best effort for second bucket - single rate overflow*/
                _drv_policer_shift_size_get(policer_cfg->peak_burst_size, commited_rate_to_alloc_unit, &size_mult, &shift_size);
                cnpl_policer_param.bucket_1 |=  (size_mult & 0xF) <<4;
                /* for second bucket rate configuration use commited_rate  bit1 = 0*/
            }
            cnpl_policer_param.bucket_1 |=  shift_size << 2;
        }
    }
    policer_param_offset = (policer_group_cfg.pa_ba  <<3)+ (policer_cfg->index * (policer_group_cfg.pl_double + 1) * POLICER_PARAM_BUCKET_SIZE);
    MWRITE_BLK_8((uint32_t *)DEVICE_ADDRESS(RU_BLK(CNPL).addr[0] + policer_param_offset), &cnpl_policer_param, ((policer_group_cfg.pl_double + 1) * POLICER_PARAM_BUCKET_SIZE));
    return rc;
}

