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

#ifndef _BCM6858_NGPON_RX_INT_AG_H_
#define _BCM6858_NGPON_RX_INT_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
typedef struct
{
    uint8_t tm_fifo_ovf;
    uint8_t bw_fifo_ovf;
    uint8_t aes_fail;
    uint8_t fwi_state_change;
    uint8_t lof_state_change;
    uint8_t lcdg_state_change;
    uint8_t lb_fifo_ovf;
    uint8_t tod_update;
    uint8_t bw_dis_tx;
    uint8_t inv_key;
    uint8_t bwmap_rec_done;
    uint8_t ponid_inconsist;
    uint8_t inv_portid_change;
    uint8_t fec_fatal_error;
    uint8_t sfc_indication;
    uint8_t amcc_grant;
    uint8_t dwba;
} ngpon_rx_int_isr;

typedef struct
{
    uint8_t tm_fifo_ovf;
    uint8_t bw_fifo_ovf;
    uint8_t aes_fail;
    uint8_t fwi_state_change;
    uint8_t lof_state_change;
    uint8_t lcdg_state_change;
    uint8_t lb_fifo_ovf;
    uint8_t tod_update;
    uint8_t bw_dis_tx;
    uint8_t inv_key;
    uint8_t bwmap_rec_done;
    uint8_t ponid_inconsist;
    uint8_t inv_portid_change;
    uint8_t fec_fatal_error;
    uint8_t sfc_indication;
    uint8_t amcc_grant;
    uint8_t dwba;
} ngpon_rx_int_ism;

typedef struct
{
    uint8_t tm_fifo_ovf_mask;
    uint8_t bw_fifo_ovf_mask;
    uint8_t aes_fail_mask;
    uint8_t fwi_state_change_mask;
    uint8_t lof_state_change_mask;
    uint8_t lcdg_state_change_mask;
    uint8_t lb_fifo_ovf_mask;
    uint8_t tod_update_mask;
    uint8_t bw_dis_tx_mask;
    uint8_t inv_key_mask;
    uint8_t bwmap_rec_done_mask;
    uint8_t ponid_inconsist_mask;
    uint8_t inv_portid_change_mask;
    uint8_t fec_fatal_error_mask;
    uint8_t sfc_indication_mask;
    uint8_t amcc_grant_mask;
    uint8_t dwba_mask;
} ngpon_rx_int_ier;

typedef struct
{
    uint8_t tm_fifo_ovf_test;
    uint8_t bw_fifo_ovf_test;
    uint8_t aes_fail_test;
    uint8_t fwi_state_change_test;
    uint8_t lof_state_change_test;
    uint8_t lcdg_state_change_test;
    uint8_t lb_fifo_ovf_test;
    uint8_t tod_update_test;
    uint8_t bw_dis_tx_test;
    uint8_t inv_key_test;
    uint8_t bwmap_rec_done_test;
    uint8_t ponid_inconsist_test;
    uint8_t inv_portid_change_test;
    uint8_t fec_fatal_error_test;
    uint8_t sfc_indication_test;
    uint8_t amcc_grant_test;
    uint8_t dwba_test;
} ngpon_rx_int_itr;

int ag_drv_ngpon_rx_int_isr_set(const ngpon_rx_int_isr *isr);
int ag_drv_ngpon_rx_int_isr_get(ngpon_rx_int_isr *isr);
int ag_drv_ngpon_rx_int_ism_get(ngpon_rx_int_ism *ism);
int ag_drv_ngpon_rx_int_ier_set(const ngpon_rx_int_ier *ier);
int ag_drv_ngpon_rx_int_ier_get(ngpon_rx_int_ier *ier);
int ag_drv_ngpon_rx_int_itr_set(const ngpon_rx_int_itr *itr);
int ag_drv_ngpon_rx_int_itr_get(ngpon_rx_int_itr *itr);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_ngpon_rx_int_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

