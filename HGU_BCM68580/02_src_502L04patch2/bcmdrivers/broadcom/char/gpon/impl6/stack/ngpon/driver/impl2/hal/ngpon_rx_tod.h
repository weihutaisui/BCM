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

#ifndef _BCM6858_NGPON_RX_TOD_AG_H_
#define _BCM6858_NGPON_RX_TOD_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
int ag_drv_ngpon_rx_tod_cfg_set(uint16_t req_phase, uint8_t incr, uint8_t lof_beh);
int ag_drv_ngpon_rx_tod_cfg_get(uint16_t *req_phase, uint8_t *incr, uint8_t *lof_beh);
int ag_drv_ngpon_rx_tod_cong_thr_set(uint8_t assert, uint8_t negate);
int ag_drv_ngpon_rx_tod_cong_thr_get(uint8_t *assert, uint8_t *negate);
int ag_drv_ngpon_rx_tod_update_time_ls_set(uint32_t value);
int ag_drv_ngpon_rx_tod_update_time_ls_get(uint32_t *value);
int ag_drv_ngpon_rx_tod_update_time_ms_set(uint32_t ms_value);
int ag_drv_ngpon_rx_tod_update_time_ms_get(uint32_t *ms_value);
int ag_drv_ngpon_rx_tod_update_val_set(uint32_t ngpon_rx_tod_value, uint32_t value);
int ag_drv_ngpon_rx_tod_update_val_get(uint32_t ngpon_rx_tod_value, uint32_t *value);
int ag_drv_ngpon_rx_tod_update_arm_set(uint8_t arm, uint8_t en, uint8_t rdarm);
int ag_drv_ngpon_rx_tod_update_arm_get(uint8_t *arm, uint8_t *en, uint8_t *rdarm);
int ag_drv_ngpon_rx_tod_nano_in_secs_set(uint32_t value);
int ag_drv_ngpon_rx_tod_nano_in_secs_get(uint32_t *value);
int ag_drv_ngpon_rx_tod_opps_set(uint32_t duty_cycle_time, uint8_t en);
int ag_drv_ngpon_rx_tod_opps_get(uint32_t *duty_cycle_time, uint8_t *en);
int ag_drv_ngpon_rx_tod_nanos_per_cycle_set(uint32_t value);
int ag_drv_ngpon_rx_tod_nanos_per_cycle_get(uint32_t *value);
int ag_drv_ngpon_rx_tod_partial_ns_inc_set(uint32_t value);
int ag_drv_ngpon_rx_tod_partial_ns_inc_get(uint32_t *value);
int ag_drv_ngpon_rx_tod_partial_ns_in_nano_set(uint32_t value);
int ag_drv_ngpon_rx_tod_partial_ns_in_nano_get(uint32_t *value);
int ag_drv_ngpon_rx_tod_ts_up_val_set(uint32_t ngpon_rx_tod_ts_up_value, uint32_t ts_val);
int ag_drv_ngpon_rx_tod_ts_up_val_get(uint32_t ngpon_rx_tod_ts_up_value, uint32_t *ts_val);
int ag_drv_ngpon_rx_tod_tod_rd_val_get(uint32_t ngpon_rx_tod_tod_rd_value, uint32_t *ts_val);
int ag_drv_ngpon_rx_tod_ts_rd_val_get(uint32_t ngpon_rx_tod_ts_rd_value, uint32_t *ts_val);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_ngpon_rx_tod_cli_init(bdmfmon_handle_t driver_dir);
#endif

#endif

