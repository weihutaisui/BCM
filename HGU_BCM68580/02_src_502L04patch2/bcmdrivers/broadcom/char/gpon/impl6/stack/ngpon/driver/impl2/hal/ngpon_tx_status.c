/*
* <:copyright-BRCM:2016:proprietary:gpon
* 
*    Copyright (c) 2016 Broadcom 
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

#include "ngpon_common.h"
#include "ngpon_tx_status.h"
int ag_drv_ngpon_tx_status_tcont_flsh_done_get(uint8_t *value)
{
    uint32_t reg_tcont_flsh_done=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_STATUS, TCONT_FLSH_DONE, reg_tcont_flsh_done);

    *value = RU_FIELD_GET(0, NGPON_TX_STATUS, TCONT_FLSH_DONE, VALUE, reg_tcont_flsh_done);

    return 0;
}

int ag_drv_ngpon_tx_status_tx_q_avail_get(uint32_t ngpon_tx_q_id, uint16_t *value)
{
    uint32_t reg_tx_q_avail=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_q_id >= 40))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_q_id, NGPON_TX_STATUS, TX_Q_AVAIL, reg_tx_q_avail);

    *value = RU_FIELD_GET(0, NGPON_TX_STATUS, TX_Q_AVAIL, VALUE, reg_tx_q_avail);

    return 0;
}

int ag_drv_ngpon_tx_status_tx_pd_avail_get(uint32_t ngpon_tx_pd_id, uint16_t *tqpdcntc)
{
    uint32_t reg_tx_pd_avail=0;

#ifdef VALIDATE_PARMS
    if(!tqpdcntc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_pd_id >= 40))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_pd_id, NGPON_TX_STATUS, TX_PD_AVAIL, reg_tx_pd_avail);

    *tqpdcntc = RU_FIELD_GET(0, NGPON_TX_STATUS, TX_PD_AVAIL, TQPDCNTC, reg_tx_pd_avail);

    return 0;
}

