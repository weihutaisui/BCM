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

#ifndef _BCM6858_GPON_TX_GEN_CTRL_AG_H_
#define _BCM6858_GPON_TX_GEN_CTRL_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
typedef struct
{
    uint8_t txen;
    uint8_t scren;
    uint8_t bipen;
    uint8_t fecen;
    uint8_t loopben;
    uint8_t plsen;
    uint8_t tdmen;
    uint8_t misctxen;
    uint8_t rngen;
} gpon_tx_gen_ctrl_ten;

int ag_drv_gpon_tx_gen_ctrl_tdly_set(uint16_t bdly, uint8_t fdly, uint8_t sdly);
int ag_drv_gpon_tx_gen_ctrl_tdly_get(uint16_t *bdly, uint8_t *fdly, uint8_t *sdly);
int ag_drv_gpon_tx_gen_ctrl_ten_set(const gpon_tx_gen_ctrl_ten *ten);
int ag_drv_gpon_tx_gen_ctrl_ten_get(gpon_tx_gen_ctrl_ten *ten);
int ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_set(uint8_t doutpol);
int ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_get(uint8_t *doutpol);
int ag_drv_gpon_tx_gen_ctrl_nprep_set(uint8_t prep0, uint8_t prep1, uint8_t prep2, uint8_t prep3);
int ag_drv_gpon_tx_gen_ctrl_nprep_get(uint8_t *prep0, uint8_t *prep1, uint8_t *prep2, uint8_t *prep3);
int ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_set(uint8_t npvld, uint8_t uplvd);
int ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get(uint8_t *npvld, uint8_t *uplvd);
int ag_drv_gpon_tx_gen_ctrl_tdbvld_set(uint32_t dbav);
int ag_drv_gpon_tx_gen_ctrl_tdbvld_get(uint32_t *dbav);
int ag_drv_gpon_tx_gen_ctrl_tdbconf_set(uint8_t dbav, uint8_t dbr_gen_en, uint8_t divrate, uint8_t dbflush);
int ag_drv_gpon_tx_gen_ctrl_tdbconf_get(uint8_t *dbav, uint8_t *dbr_gen_en, uint8_t *divrate, uint8_t *dbflush);
int ag_drv_gpon_tx_gen_ctrl_tpclr_set(uint8_t tpclrc);
int ag_drv_gpon_tx_gen_ctrl_tpclr_get(uint8_t *tpclrc);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_tx_gen_ctrl_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

