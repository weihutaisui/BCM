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

#ifndef _BCM6858_GPON_TX_PLOAM_AND_DBA_MEMORY_AG_H_
#define _BCM6858_GPON_TX_PLOAM_AND_DBA_MEMORY_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
int ag_drv_gpon_tx_ploam_and_dba_memory_rplm_set(uint32_t word_idx, uint8_t rplmc);
int ag_drv_gpon_tx_ploam_and_dba_memory_rplm_get(uint32_t word_idx, uint8_t *rplmc);
int ag_drv_gpon_tx_ploam_and_dba_memory_iplm_set(uint32_t word_idx, uint8_t iplc);
int ag_drv_gpon_tx_ploam_and_dba_memory_iplm_get(uint32_t word_idx, uint8_t *iplc);
int ag_drv_gpon_tx_ploam_and_dba_memory_uplm_set(uint32_t word_idx, uint8_t uplmc);
int ag_drv_gpon_tx_ploam_and_dba_memory_uplm_get(uint32_t word_idx, uint8_t *uplmc);
int ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_set(uint32_t word_idx, uint8_t nplmc);
int ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_get(uint32_t word_idx, uint8_t *nplmc);
int ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_set(uint32_t word_idx, uint8_t nplmc);
int ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_get(uint32_t word_idx, uint8_t *nplmc);
int ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_set(uint32_t word_idx, uint8_t nplmc);
int ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_get(uint32_t word_idx, uint8_t *nplmc);
int ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_set(uint32_t word_idx, uint8_t nplmc);
int ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_get(uint32_t word_idx, uint8_t *nplmc);
int ag_drv_gpon_tx_ploam_and_dba_memory_idbr_set(uint32_t idbcc);
int ag_drv_gpon_tx_ploam_and_dba_memory_idbr_get(uint32_t *idbcc);
int ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_set(uint8_t idbr);
int ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_get(uint8_t *idbr);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_set(uint32_t ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_get(uint32_t *ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_set(uint8_t ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_get(uint8_t *ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_set(uint32_t ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_get(uint32_t *ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_set(uint8_t ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_get(uint8_t *ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_set(uint32_t ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_get(uint32_t *ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_set(uint8_t ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_get(uint8_t *ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_set(uint32_t ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_get(uint32_t *ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_set(uint8_t ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_get(uint8_t *ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_set(uint32_t ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_get(uint32_t *ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_set(uint8_t ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_get(uint8_t *ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_set(uint32_t ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_get(uint32_t *ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_set(uint8_t ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_get(uint8_t *ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_set(uint32_t ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_get(uint32_t *ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_set(uint8_t ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_get(uint8_t *ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_set(uint32_t ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_get(uint32_t *ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_set(uint8_t ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_get(uint8_t *ndbcbyte5);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_set(uint32_t dbr_idx, uint32_t ndbc);
int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_get(uint32_t dbr_idx, uint32_t *ndbc);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_tx_ploam_and_dba_memory_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

