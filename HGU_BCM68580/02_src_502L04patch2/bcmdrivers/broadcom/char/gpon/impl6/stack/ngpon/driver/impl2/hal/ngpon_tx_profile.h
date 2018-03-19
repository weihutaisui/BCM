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

#ifndef _BCM6858_NGPON_TX_PROFILE_AG_H_
#define _BCM6858_NGPON_TX_PROFILE_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
typedef struct
{
    uint8_t delimiter_len;
    uint8_t preamble_len;
    uint8_t fec_ind;
    uint16_t preamble_total_len;
    uint8_t prof_en;
} ngpon_tx_profile_profile_ctrl;

int ag_drv_ngpon_tx_profile_profile_ctrl_set(uint32_t tx_profile_ctrl, const ngpon_tx_profile_profile_ctrl *profile_ctrl);
int ag_drv_ngpon_tx_profile_profile_ctrl_get(uint32_t tx_profile_ctrl, ngpon_tx_profile_profile_ctrl *profile_ctrl);
int ag_drv_ngpon_tx_profile_preamble_0_set(uint32_t tx_profile_preamble_0, uint32_t value);
int ag_drv_ngpon_tx_profile_preamble_0_get(uint32_t tx_profile_preamble_0, uint32_t *value);
int ag_drv_ngpon_tx_profile_preamble_1_set(uint32_t tx_profile_preamble_1, uint32_t value);
int ag_drv_ngpon_tx_profile_preamble_1_get(uint32_t tx_profile_preamble_1, uint32_t *value);
int ag_drv_ngpon_tx_profile_delimiter_0_set(uint32_t tx_profile_delimeter_0, uint32_t value);
int ag_drv_ngpon_tx_profile_delimiter_0_get(uint32_t tx_profile_delimeter_0, uint32_t *value);
int ag_drv_ngpon_tx_profile_delimiter_1_set(uint32_t tx_profile_delimeter_1, uint32_t value);
int ag_drv_ngpon_tx_profile_delimiter_1_get(uint32_t tx_profile_delimeter_1, uint32_t *value);
int ag_drv_ngpon_tx_profile_fec_type_set(uint32_t tx_profile_fec_type, uint8_t type);
int ag_drv_ngpon_tx_profile_fec_type_get(uint32_t tx_profile_fec_type, uint8_t *type);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_ngpon_tx_profile_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

