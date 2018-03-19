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
#ifndef _RDP_DRV_POLICER_H_
#define _RDP_DRV_POLICER_H_

#include "rdp_drv_cnpl.h"
#include "rdpa_policer.h"

typedef struct {
    uint8_t index;                    /* policer index */
    uint8_t is_dual;                  /* 0 - single bucket ; 1 - dual bucket */
    uint64_t commited_rate;           /* Committed Information Rate (CIR) - bps */
    uint64_t peak_rate;               /* PEAK Information Rate (PIR) - bps */
    uint64_t committed_burst_size;    /* Committed Burst Size (CBS) - bytes */
    uint64_t peak_burst_size;         /* PEAK Burst Size (PBS) - bytes */
    uint8_t overflow;                 /* dual bucket mode */
    uint8_t dei_mode;                 /* 0 - disable ; 1 - enable */
} policer_cfg_t;

/* Policer factor to be added for every packet */
typedef enum {
    rdp_policer_factor_bytes_neg_8,   /* subtract 8B to policing */
    rdp_policer_factor_bytes_neg_4,   /* subtract 4B to policing */
    rdp_policer_factor_bytes_0,       /* don't add bytes to policing */
    rdp_policer_factor_bytes_4,       /* add 4B to policing */
    rdp_policer_factor_bytes_8,       /* add 8B to policing */
    rdp_policer_factor_num_of,        /* Number of possible types */
} rdp_policer_factor_bytes;

bdmf_error_t drv_policer_group_init(void);
bdmf_error_t drv_cnpl_policer_set(policer_cfg_t* policer_cfg);

static inline void drv_cnpl_policer_max_cbs_get(policer_rate_size_t commited_rate, uint32_t* max_committed_burst_byte_size)
{
    *max_committed_burst_byte_size = (((commited_rate / POLICER_TIMER_PERIOD) * BUCKET_SIZE_RATE_MULT_MAX) / 8);
}

static inline void drv_cnpl_policer_min_cbs_get(policer_rate_size_t  commited_rate, uint32_t* min_committed_burst_byte_size)
{
    *min_committed_burst_byte_size = (commited_rate / POLICER_TIMER_PERIOD) / 8;
}

#endif
