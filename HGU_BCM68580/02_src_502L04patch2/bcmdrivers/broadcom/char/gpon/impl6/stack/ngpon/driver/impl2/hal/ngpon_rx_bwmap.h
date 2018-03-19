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

#ifndef _BCM6858_NGPON_RX_BWMAP_AG_H_
#define _BCM6858_NGPON_RX_BWMAP_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
int ag_drv_ngpon_rx_bwmap_trfc_allocid_set(uint32_t tcont_idx, uint16_t value);
int ag_drv_ngpon_rx_bwmap_trfc_allocid_get(uint32_t tcont_idx, uint16_t *value);
int ag_drv_ngpon_rx_bwmap_rngng_allocid_set(uint32_t onu_id_idx, uint16_t value);
int ag_drv_ngpon_rx_bwmap_rngng_allocid_get(uint32_t onu_id_idx, uint16_t *value);
int ag_drv_ngpon_rx_bwmap_trfc_allocid_en1_set(uint32_t en);
int ag_drv_ngpon_rx_bwmap_trfc_allocid_en1_get(uint32_t *en);
int ag_drv_ngpon_rx_bwmap_trfc_allocid_en2_set(uint8_t en);
int ag_drv_ngpon_rx_bwmap_trfc_allocid_en2_get(uint8_t *en);
int ag_drv_ngpon_rx_bwmap_rngng_allocid_en_set(uint8_t en);
int ag_drv_ngpon_rx_bwmap_rngng_allocid_en_get(uint8_t *en);
int ag_drv_ngpon_rx_bwmap_trfc_tcnt_assoc_set(uint32_t tcont_idx, uint8_t a0_tnum, uint8_t a1_tnum, uint8_t a2_tnum, uint8_t a3_tnum);
int ag_drv_ngpon_rx_bwmap_trfc_tcnt_assoc_get(uint32_t tcont_idx, uint8_t *a0_tnum, uint8_t *a1_tnum, uint8_t *a2_tnum, uint8_t *a3_tnum);
int ag_drv_ngpon_rx_bwmap_rngng_tcnt_assoc_set(uint8_t rng_a0_tnum, uint8_t rng_a1_tnum, uint8_t rng_a2_tnum, uint8_t rng_a3_tnum);
int ag_drv_ngpon_rx_bwmap_rngng_tcnt_assoc_get(uint8_t *rng_a0_tnum, uint8_t *rng_a1_tnum, uint8_t *rng_a2_tnum, uint8_t *rng_a3_tnum);
int ag_drv_ngpon_rx_bwmap_bwcnt_assoc_set(uint8_t cnt9_assoc, uint8_t cnt10_assoc, uint8_t cnt11_assoc, uint8_t cnt12_assoc);
int ag_drv_ngpon_rx_bwmap_bwcnt_assoc_get(uint8_t *cnt9_assoc, uint8_t *cnt10_assoc, uint8_t *cnt11_assoc, uint8_t *cnt12_assoc);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_ngpon_rx_bwmap_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

