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

#ifndef _BCM6858_NGPON_RX_ENC_AG_H_
#define _BCM6858_NGPON_RX_ENC_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
typedef struct
{
    uint8_t key1_onu1;
    uint8_t key2_onu1;
    uint8_t key1_onu2;
    uint8_t key2_onu2;
    uint8_t key1_mcst;
    uint8_t key2_mcst;
} ngpon_rx_enc_key_valid;

int ag_drv_ngpon_rx_enc_key_valid_set(const ngpon_rx_enc_key_valid *key_valid);
int ag_drv_ngpon_rx_enc_key_valid_get(ngpon_rx_enc_key_valid *key_valid);
int ag_drv_ngpon_rx_enc_ploam_load_set(uint8_t ploamik_onu1, uint8_t ploamik_onu2, uint8_t ploamik_bcst);
int ag_drv_ngpon_rx_enc_ploam_load_get(uint8_t *ploamik_onu1, uint8_t *ploamik_onu2, uint8_t *ploamik_bcst);
int ag_drv_ngpon_rx_enc_key1_onu1_set(uint32_t key1_onu1_word_idx, uint32_t value);
int ag_drv_ngpon_rx_enc_key1_onu1_get(uint32_t key1_onu1_word_idx, uint32_t *value);
int ag_drv_ngpon_rx_enc_key2_onu1_set(uint32_t key2_onu1_word_idx, uint32_t value);
int ag_drv_ngpon_rx_enc_key2_onu1_get(uint32_t key2_onu1_word_idx, uint32_t *value);
int ag_drv_ngpon_rx_enc_key1_onu2_set(uint32_t key1_onu2_word_idx, uint32_t value);
int ag_drv_ngpon_rx_enc_key1_onu2_get(uint32_t key1_onu2_word_idx, uint32_t *value);
int ag_drv_ngpon_rx_enc_key2_onu2_set(uint32_t key2_onu2_word_idx, uint32_t value);
int ag_drv_ngpon_rx_enc_key2_onu2_get(uint32_t key2_onu2_word_idx, uint32_t *value);
int ag_drv_ngpon_rx_enc_ploamik_onu1_set(uint32_t ploam_ik_onu1_word_idx, uint32_t value);
int ag_drv_ngpon_rx_enc_ploamik_onu1_get(uint32_t ploam_ik_onu1_word_idx, uint32_t *value);
int ag_drv_ngpon_rx_enc_ploamik_onu2_set(uint32_t ploam_ik_onu2_word_idx, uint32_t value);
int ag_drv_ngpon_rx_enc_ploamik_onu2_get(uint32_t ploam_ik_onu2_word_idx, uint32_t *value);
int ag_drv_ngpon_rx_enc_ploamik_bcst_set(uint32_t ploam_ik_bcst_word_idx, uint32_t value);
int ag_drv_ngpon_rx_enc_ploamik_bcst_get(uint32_t ploam_ik_bcst_word_idx, uint32_t *value);
int ag_drv_ngpon_rx_enc_cmack2_onu1_0_set(uint32_t value);
int ag_drv_ngpon_rx_enc_cmack2_onu1_0_get(uint32_t *value);
int ag_drv_ngpon_rx_enc_cmack2_onu1_1_set(uint32_t value);
int ag_drv_ngpon_rx_enc_cmack2_onu1_1_get(uint32_t *value);
int ag_drv_ngpon_rx_enc_cmack2_onu1_2_set(uint32_t value);
int ag_drv_ngpon_rx_enc_cmack2_onu1_2_get(uint32_t *value);
int ag_drv_ngpon_rx_enc_cmack2_onu1_3_set(uint32_t value);
int ag_drv_ngpon_rx_enc_cmack2_onu1_3_get(uint32_t *value);
int ag_drv_ngpon_rx_enc_cmack2_onu2_0_set(uint32_t value);
int ag_drv_ngpon_rx_enc_cmack2_onu2_0_get(uint32_t *value);
int ag_drv_ngpon_rx_enc_cmack2_onu2_1_set(uint32_t value);
int ag_drv_ngpon_rx_enc_cmack2_onu2_1_get(uint32_t *value);
int ag_drv_ngpon_rx_enc_cmack2_onu2_2_set(uint32_t value);
int ag_drv_ngpon_rx_enc_cmack2_onu2_2_get(uint32_t *value);
int ag_drv_ngpon_rx_enc_cmack2_onu2_3_set(uint32_t value);
int ag_drv_ngpon_rx_enc_cmack2_onu2_3_get(uint32_t *value);
int ag_drv_ngpon_rx_enc_cmack2_bcst_0_set(uint32_t value);
int ag_drv_ngpon_rx_enc_cmack2_bcst_0_get(uint32_t *value);
int ag_drv_ngpon_rx_enc_cmack2_bcst_1_set(uint32_t value);
int ag_drv_ngpon_rx_enc_cmack2_bcst_1_get(uint32_t *value);
int ag_drv_ngpon_rx_enc_cmack2_bcst_2_set(uint32_t value);
int ag_drv_ngpon_rx_enc_cmack2_bcst_2_get(uint32_t *value);
int ag_drv_ngpon_rx_enc_cmack2_bcst_3_set(uint32_t value);
int ag_drv_ngpon_rx_enc_cmack2_bcst_3_get(uint32_t *value);
int ag_drv_ngpon_rx_enc_key1_mcst_set(uint32_t mcast_key1_word_idx, uint32_t value);
int ag_drv_ngpon_rx_enc_key1_mcst_get(uint32_t mcast_key1_word_idx, uint32_t *value);
int ag_drv_ngpon_rx_enc_key2_mcst_set(uint32_t mcast_key2_word_idx, uint32_t value);
int ag_drv_ngpon_rx_enc_key2_mcst_get(uint32_t mcast_key2_word_idx, uint32_t *value);
int ag_drv_ngpon_rx_enc_ploamik_lookup_get(uint32_t ploam_ik_lookup_word_idx, uint32_t *value);
int ag_drv_ngpon_rx_enc_cmack2_lookup_get(uint32_t cmac2_lookup_word_idx, uint32_t *value);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_ngpon_rx_enc_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

