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

#ifndef _BCM6858_GPON_RX_PM_COUNTER_AG_H_
#define _BCM6858_GPON_RX_PM_COUNTER_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
int ag_drv_gpon_rx_pm_counter_bwmaperr_get(uint32_t *bwmaperr);
int ag_drv_gpon_rx_pm_counter_ploamerr_get(uint32_t *ploamerr);
int ag_drv_gpon_rx_pm_counter_plenderr_get(uint32_t *plenderr);
int ag_drv_gpon_rx_pm_counter_sfmismatch_get(uint32_t *sfmismatch);
int ag_drv_gpon_rx_pm_counter_bip_get(uint32_t *bip);
int ag_drv_gpon_rx_pm_counter_gemfrag_get(uint32_t *gemfrag);
int ag_drv_gpon_rx_pm_counter_ethpack_get(uint32_t *ethpack);
int ag_drv_gpon_rx_pm_counter_tdmfrag_get(uint32_t *tdmfrag);
int ag_drv_gpon_rx_pm_counter_symerror_get(uint32_t *symerrors);
int ag_drv_gpon_rx_pm_counter_biterror_get(uint32_t *biterrors);
int ag_drv_gpon_rx_pm_counter_aeserror_get(uint32_t *aeserrcnt);
int ag_drv_gpon_rx_pm_counter_tdmstats_get(uint32_t *rcvbytes);
int ag_drv_gpon_rx_pm_counter_ethfrag_get(uint32_t *ethfrag);
int ag_drv_gpon_rx_pm_counter_ethbytecnt_get(uint32_t *ethbytecnt);
int ag_drv_gpon_rx_pm_counter_fecuccwcnt_get(uint16_t *fecuccwcnt);
int ag_drv_gpon_rx_pm_counter_fecccwcnt_get(uint32_t *fecccwcnt);
int ag_drv_gpon_rx_pm_counter_plonucnt_get(uint32_t *pl_onu_cnt);
int ag_drv_gpon_rx_pm_counter_plbcstcnt_get(uint32_t *pl_bcst_cnt);
int ag_drv_gpon_rx_pm_counter_bw_cnt_get(uint32_t tcont_idx, uint32_t *bw_cnt);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_rx_pm_counter_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

