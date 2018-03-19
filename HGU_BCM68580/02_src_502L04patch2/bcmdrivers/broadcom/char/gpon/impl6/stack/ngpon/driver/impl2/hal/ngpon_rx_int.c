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
#include "ngpon_rx_int.h"
int ag_drv_ngpon_rx_int_isr_set(const ngpon_rx_int_isr *isr)
{
    uint32_t reg_isr=0;

#ifdef VALIDATE_PARMS
    if(!isr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
    }
    if((isr->tm_fifo_ovf >= _1BITS_MAX_VAL_) ||
       (isr->bw_fifo_ovf >= _1BITS_MAX_VAL_) ||
       (isr->aes_fail >= _1BITS_MAX_VAL_) ||
       (isr->fwi_state_change >= _1BITS_MAX_VAL_) ||
       (isr->lof_state_change >= _1BITS_MAX_VAL_) ||
       (isr->lcdg_state_change >= _1BITS_MAX_VAL_) ||
       (isr->lb_fifo_ovf >= _1BITS_MAX_VAL_) ||
       (isr->tod_update >= _1BITS_MAX_VAL_) ||
       (isr->bw_dis_tx >= _1BITS_MAX_VAL_) ||
       (isr->inv_key >= _1BITS_MAX_VAL_) ||
       (isr->bwmap_rec_done >= _1BITS_MAX_VAL_) ||
       (isr->ponid_inconsist >= _1BITS_MAX_VAL_) ||
       (isr->inv_portid_change >= _1BITS_MAX_VAL_) ||
       (isr->fec_fatal_error >= _1BITS_MAX_VAL_) ||
       (isr->sfc_indication >= _1BITS_MAX_VAL_) ||
       (isr->amcc_grant >= _1BITS_MAX_VAL_) ||
       (isr->dwba >= _4BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, TM_FIFO_OVF, reg_isr, isr->tm_fifo_ovf);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, BW_FIFO_OVF, reg_isr, isr->bw_fifo_ovf);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, AES_FAIL, reg_isr, isr->aes_fail);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, FWI_STATE_CHANGE, reg_isr, isr->fwi_state_change);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, LOF_STATE_CHANGE, reg_isr, isr->lof_state_change);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, LCDG_STATE_CHANGE, reg_isr, isr->lcdg_state_change);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, LB_FIFO_OVF, reg_isr, isr->lb_fifo_ovf);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, TOD_UPDATE, reg_isr, isr->tod_update);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, BW_DIS_TX, reg_isr, isr->bw_dis_tx);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, INV_KEY, reg_isr, isr->inv_key);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, BWMAP_REC_DONE, reg_isr, isr->bwmap_rec_done);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, PONID_INCONSIST, reg_isr, isr->ponid_inconsist);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, INV_PORTID_CHANGE, reg_isr, isr->inv_portid_change);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, FEC_FATAL_ERROR, reg_isr, isr->fec_fatal_error);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, SFC_INDICATION, reg_isr, isr->sfc_indication);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, AMCC_GRANT, reg_isr, isr->amcc_grant);
    reg_isr = RU_FIELD_SET(0, NGPON_RX_INT, ISR, DWBA, reg_isr, isr->dwba);

    RU_REG_WRITE(0, NGPON_RX_INT, ISR, reg_isr);

    return 0;
}

int ag_drv_ngpon_rx_int_isr_get(ngpon_rx_int_isr *isr)
{
    uint32_t reg_isr=0;

#ifdef VALIDATE_PARMS
    if(!isr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_INT, ISR, reg_isr);

    isr->tm_fifo_ovf = RU_FIELD_GET(0, NGPON_RX_INT, ISR, TM_FIFO_OVF, reg_isr);
    isr->bw_fifo_ovf = RU_FIELD_GET(0, NGPON_RX_INT, ISR, BW_FIFO_OVF, reg_isr);
    isr->aes_fail = RU_FIELD_GET(0, NGPON_RX_INT, ISR, AES_FAIL, reg_isr);
    isr->fwi_state_change = RU_FIELD_GET(0, NGPON_RX_INT, ISR, FWI_STATE_CHANGE, reg_isr);
    isr->lof_state_change = RU_FIELD_GET(0, NGPON_RX_INT, ISR, LOF_STATE_CHANGE, reg_isr);
    isr->lcdg_state_change = RU_FIELD_GET(0, NGPON_RX_INT, ISR, LCDG_STATE_CHANGE, reg_isr);
    isr->lb_fifo_ovf = RU_FIELD_GET(0, NGPON_RX_INT, ISR, LB_FIFO_OVF, reg_isr);
    isr->tod_update = RU_FIELD_GET(0, NGPON_RX_INT, ISR, TOD_UPDATE, reg_isr);
    isr->bw_dis_tx = RU_FIELD_GET(0, NGPON_RX_INT, ISR, BW_DIS_TX, reg_isr);
    isr->inv_key = RU_FIELD_GET(0, NGPON_RX_INT, ISR, INV_KEY, reg_isr);
    isr->bwmap_rec_done = RU_FIELD_GET(0, NGPON_RX_INT, ISR, BWMAP_REC_DONE, reg_isr);
    isr->ponid_inconsist = RU_FIELD_GET(0, NGPON_RX_INT, ISR, PONID_INCONSIST, reg_isr);
    isr->inv_portid_change = RU_FIELD_GET(0, NGPON_RX_INT, ISR, INV_PORTID_CHANGE, reg_isr);
    isr->fec_fatal_error = RU_FIELD_GET(0, NGPON_RX_INT, ISR, FEC_FATAL_ERROR, reg_isr);
    isr->sfc_indication = RU_FIELD_GET(0, NGPON_RX_INT, ISR, SFC_INDICATION, reg_isr);
    isr->amcc_grant = RU_FIELD_GET(0, NGPON_RX_INT, ISR, AMCC_GRANT, reg_isr);
    isr->dwba = RU_FIELD_GET(0, NGPON_RX_INT, ISR, DWBA, reg_isr);

    return 0;
}

int ag_drv_ngpon_rx_int_ism_get(ngpon_rx_int_ism *ism)
{
    uint32_t reg_ism=0;

#ifdef VALIDATE_PARMS
    if(!ism)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_INT, ISM, reg_ism);

    ism->tm_fifo_ovf = RU_FIELD_GET(0, NGPON_RX_INT, ISM, TM_FIFO_OVF, reg_ism);
    ism->bw_fifo_ovf = RU_FIELD_GET(0, NGPON_RX_INT, ISM, BW_FIFO_OVF, reg_ism);
    ism->aes_fail = RU_FIELD_GET(0, NGPON_RX_INT, ISM, AES_FAIL, reg_ism);
    ism->fwi_state_change = RU_FIELD_GET(0, NGPON_RX_INT, ISM, FWI_STATE_CHANGE, reg_ism);
    ism->lof_state_change = RU_FIELD_GET(0, NGPON_RX_INT, ISM, LOF_STATE_CHANGE, reg_ism);
    ism->lcdg_state_change = RU_FIELD_GET(0, NGPON_RX_INT, ISM, LCDG_STATE_CHANGE, reg_ism);
    ism->lb_fifo_ovf = RU_FIELD_GET(0, NGPON_RX_INT, ISM, LB_FIFO_OVF, reg_ism);
    ism->tod_update = RU_FIELD_GET(0, NGPON_RX_INT, ISM, TOD_UPDATE, reg_ism);
    ism->bw_dis_tx = RU_FIELD_GET(0, NGPON_RX_INT, ISM, BW_DIS_TX, reg_ism);
    ism->inv_key = RU_FIELD_GET(0, NGPON_RX_INT, ISM, INV_KEY, reg_ism);
    ism->bwmap_rec_done = RU_FIELD_GET(0, NGPON_RX_INT, ISM, BWMAP_REC_DONE, reg_ism);
    ism->ponid_inconsist = RU_FIELD_GET(0, NGPON_RX_INT, ISM, PONID_INCONSIST, reg_ism);
    ism->inv_portid_change = RU_FIELD_GET(0, NGPON_RX_INT, ISM, INV_PORTID_CHANGE, reg_ism);
    ism->fec_fatal_error = RU_FIELD_GET(0, NGPON_RX_INT, ISM, FEC_FATAL_ERROR, reg_ism);
    ism->sfc_indication = RU_FIELD_GET(0, NGPON_RX_INT, ISM, SFC_INDICATION, reg_ism);
    ism->amcc_grant = RU_FIELD_GET(0, NGPON_RX_INT, ISM, AMCC_GRANT, reg_ism);
    ism->dwba = RU_FIELD_GET(0, NGPON_RX_INT, ISM, DWBA, reg_ism);

    return 0;
}

int ag_drv_ngpon_rx_int_ier_set(const ngpon_rx_int_ier *ier)
{
    uint32_t reg_ier=0;

#ifdef VALIDATE_PARMS
    if(!ier)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
    }
    if((ier->tm_fifo_ovf_mask >= _1BITS_MAX_VAL_) ||
       (ier->bw_fifo_ovf_mask >= _1BITS_MAX_VAL_) ||
       (ier->aes_fail_mask >= _1BITS_MAX_VAL_) ||
       (ier->fwi_state_change_mask >= _1BITS_MAX_VAL_) ||
       (ier->lof_state_change_mask >= _1BITS_MAX_VAL_) ||
       (ier->lcdg_state_change_mask >= _1BITS_MAX_VAL_) ||
       (ier->lb_fifo_ovf_mask >= _1BITS_MAX_VAL_) ||
       (ier->tod_update_mask >= _1BITS_MAX_VAL_) ||
       (ier->bw_dis_tx_mask >= _1BITS_MAX_VAL_) ||
       (ier->inv_key_mask >= _1BITS_MAX_VAL_) ||
       (ier->bwmap_rec_done_mask >= _1BITS_MAX_VAL_) ||
       (ier->ponid_inconsist_mask >= _1BITS_MAX_VAL_) ||
       (ier->inv_portid_change_mask >= _1BITS_MAX_VAL_) ||
       (ier->fec_fatal_error_mask >= _1BITS_MAX_VAL_) ||
       (ier->sfc_indication_mask >= _1BITS_MAX_VAL_) ||
       (ier->amcc_grant_mask >= _1BITS_MAX_VAL_) ||
       (ier->dwba_mask >= _4BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, TM_FIFO_OVF_MASK, reg_ier, ier->tm_fifo_ovf_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, BW_FIFO_OVF_MASK, reg_ier, ier->bw_fifo_ovf_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, AES_FAIL_MASK, reg_ier, ier->aes_fail_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, FWI_STATE_CHANGE_MASK, reg_ier, ier->fwi_state_change_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, LOF_STATE_CHANGE_MASK, reg_ier, ier->lof_state_change_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, LCDG_STATE_CHANGE_MASK, reg_ier, ier->lcdg_state_change_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, LB_FIFO_OVF_MASK, reg_ier, ier->lb_fifo_ovf_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, TOD_UPDATE_MASK, reg_ier, ier->tod_update_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, BW_DIS_TX_MASK, reg_ier, ier->bw_dis_tx_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, INV_KEY_MASK, reg_ier, ier->inv_key_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, BWMAP_REC_DONE_MASK, reg_ier, ier->bwmap_rec_done_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, PONID_INCONSIST_MASK, reg_ier, ier->ponid_inconsist_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, INV_PORTID_CHANGE_MASK, reg_ier, ier->inv_portid_change_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, FEC_FATAL_ERROR_MASK, reg_ier, ier->fec_fatal_error_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, SFC_INDICATION_MASK, reg_ier, ier->sfc_indication_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, AMCC_GRANT_MASK, reg_ier, ier->amcc_grant_mask);
    reg_ier = RU_FIELD_SET(0, NGPON_RX_INT, IER, DWBA_MASK, reg_ier, ier->dwba_mask);

    RU_REG_WRITE(0, NGPON_RX_INT, IER, reg_ier);

    return 0;
}

int ag_drv_ngpon_rx_int_ier_get(ngpon_rx_int_ier *ier)
{
    uint32_t reg_ier=0;

#ifdef VALIDATE_PARMS
    if(!ier)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_INT, IER, reg_ier);

    ier->tm_fifo_ovf_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, TM_FIFO_OVF_MASK, reg_ier);
    ier->bw_fifo_ovf_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, BW_FIFO_OVF_MASK, reg_ier);
    ier->aes_fail_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, AES_FAIL_MASK, reg_ier);
    ier->fwi_state_change_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, FWI_STATE_CHANGE_MASK, reg_ier);
    ier->lof_state_change_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, LOF_STATE_CHANGE_MASK, reg_ier);
    ier->lcdg_state_change_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, LCDG_STATE_CHANGE_MASK, reg_ier);
    ier->lb_fifo_ovf_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, LB_FIFO_OVF_MASK, reg_ier);
    ier->tod_update_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, TOD_UPDATE_MASK, reg_ier);
    ier->bw_dis_tx_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, BW_DIS_TX_MASK, reg_ier);
    ier->inv_key_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, INV_KEY_MASK, reg_ier);
    ier->bwmap_rec_done_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, BWMAP_REC_DONE_MASK, reg_ier);
    ier->ponid_inconsist_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, PONID_INCONSIST_MASK, reg_ier);
    ier->inv_portid_change_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, INV_PORTID_CHANGE_MASK, reg_ier);
    ier->fec_fatal_error_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, FEC_FATAL_ERROR_MASK, reg_ier);
    ier->sfc_indication_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, SFC_INDICATION_MASK, reg_ier);
    ier->amcc_grant_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, AMCC_GRANT_MASK, reg_ier);
    ier->dwba_mask = RU_FIELD_GET(0, NGPON_RX_INT, IER, DWBA_MASK, reg_ier);

    return 0;
}

int ag_drv_ngpon_rx_int_itr_set(const ngpon_rx_int_itr *itr)
{
    uint32_t reg_itr=0;

#ifdef VALIDATE_PARMS
    if(!itr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
    }
    if((itr->tm_fifo_ovf_test >= _1BITS_MAX_VAL_) ||
       (itr->bw_fifo_ovf_test >= _1BITS_MAX_VAL_) ||
       (itr->aes_fail_test >= _1BITS_MAX_VAL_) ||
       (itr->fwi_state_change_test >= _1BITS_MAX_VAL_) ||
       (itr->lof_state_change_test >= _1BITS_MAX_VAL_) ||
       (itr->lcdg_state_change_test >= _1BITS_MAX_VAL_) ||
       (itr->lb_fifo_ovf_test >= _1BITS_MAX_VAL_) ||
       (itr->tod_update_test >= _1BITS_MAX_VAL_) ||
       (itr->bw_dis_tx_test >= _1BITS_MAX_VAL_) ||
       (itr->inv_key_test >= _1BITS_MAX_VAL_) ||
       (itr->bwmap_rec_done_test >= _1BITS_MAX_VAL_) ||
       (itr->ponid_inconsist_test >= _1BITS_MAX_VAL_) ||
       (itr->inv_portid_change_test >= _1BITS_MAX_VAL_) ||
       (itr->fec_fatal_error_test >= _1BITS_MAX_VAL_) ||
       (itr->sfc_indication_test >= _1BITS_MAX_VAL_) ||
       (itr->amcc_grant_test >= _1BITS_MAX_VAL_) ||
       (itr->dwba_test >= _4BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, TM_FIFO_OVF_TEST, reg_itr, itr->tm_fifo_ovf_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, BW_FIFO_OVF_TEST, reg_itr, itr->bw_fifo_ovf_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, AES_FAIL_TEST, reg_itr, itr->aes_fail_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, FWI_STATE_CHANGE_TEST, reg_itr, itr->fwi_state_change_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, LOF_STATE_CHANGE_TEST, reg_itr, itr->lof_state_change_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, LCDG_STATE_CHANGE_TEST, reg_itr, itr->lcdg_state_change_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, LB_FIFO_OVF_TEST, reg_itr, itr->lb_fifo_ovf_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, TOD_UPDATE_TEST, reg_itr, itr->tod_update_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, BW_DIS_TX_TEST, reg_itr, itr->bw_dis_tx_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, INV_KEY_TEST, reg_itr, itr->inv_key_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, BWMAP_REC_DONE_TEST, reg_itr, itr->bwmap_rec_done_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, PONID_INCONSIST_TEST, reg_itr, itr->ponid_inconsist_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, INV_PORTID_CHANGE_TEST, reg_itr, itr->inv_portid_change_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, FEC_FATAL_ERROR_TEST, reg_itr, itr->fec_fatal_error_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, SFC_INDICATION_TEST, reg_itr, itr->sfc_indication_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, AMCC_GRANT_TEST, reg_itr, itr->amcc_grant_test);
    reg_itr = RU_FIELD_SET(0, NGPON_RX_INT, ITR, DWBA_TEST, reg_itr, itr->dwba_test);

    RU_REG_WRITE(0, NGPON_RX_INT, ITR, reg_itr);

    return 0;
}

int ag_drv_ngpon_rx_int_itr_get(ngpon_rx_int_itr *itr)
{
    uint32_t reg_itr=0;

#ifdef VALIDATE_PARMS
    if(!itr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_INT, ITR, reg_itr);

    itr->tm_fifo_ovf_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, TM_FIFO_OVF_TEST, reg_itr);
    itr->bw_fifo_ovf_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, BW_FIFO_OVF_TEST, reg_itr);
    itr->aes_fail_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, AES_FAIL_TEST, reg_itr);
    itr->fwi_state_change_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, FWI_STATE_CHANGE_TEST, reg_itr);
    itr->lof_state_change_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, LOF_STATE_CHANGE_TEST, reg_itr);
    itr->lcdg_state_change_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, LCDG_STATE_CHANGE_TEST, reg_itr);
    itr->lb_fifo_ovf_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, LB_FIFO_OVF_TEST, reg_itr);
    itr->tod_update_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, TOD_UPDATE_TEST, reg_itr);
    itr->bw_dis_tx_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, BW_DIS_TX_TEST, reg_itr);
    itr->inv_key_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, INV_KEY_TEST, reg_itr);
    itr->bwmap_rec_done_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, BWMAP_REC_DONE_TEST, reg_itr);
    itr->ponid_inconsist_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, PONID_INCONSIST_TEST, reg_itr);
    itr->inv_portid_change_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, INV_PORTID_CHANGE_TEST, reg_itr);
    itr->fec_fatal_error_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, FEC_FATAL_ERROR_TEST, reg_itr);
    itr->sfc_indication_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, SFC_INDICATION_TEST, reg_itr);
    itr->amcc_grant_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, AMCC_GRANT_TEST, reg_itr);
    itr->dwba_test = RU_FIELD_GET(0, NGPON_RX_INT, ITR, DWBA_TEST, reg_itr);

    return 0;
}

