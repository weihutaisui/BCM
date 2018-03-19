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

#ifndef _BCM6858_GPON_TX_INTERRUPT_CONTROLLER_AG_H_
#define _BCM6858_GPON_TX_INTERRUPT_CONTROLLER_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
int ag_drv_gpon_tx_interrupt_controller_txisr0_set(uint32_t db_irq_idx, uint8_t db_irq_id);
int ag_drv_gpon_tx_interrupt_controller_txisr0_get(uint32_t db_irq_idx, uint8_t *db_irq_id);
int ag_drv_gpon_tx_interrupt_controller_txisr1_set(uint32_t db_irq_idx, uint8_t db_irq_id);
int ag_drv_gpon_tx_interrupt_controller_txisr1_get(uint32_t db_irq_idx, uint8_t *db_irq_id);
int ag_drv_gpon_tx_interrupt_controller_txisr2_set(uint32_t db_irq_idx, uint8_t db_irq_id);
int ag_drv_gpon_tx_interrupt_controller_txisr2_get(uint32_t db_irq_idx, uint8_t *db_irq_id);
int ag_drv_gpon_tx_interrupt_controller_txier0_set(uint32_t int_src_idx, uint8_t int_src_word);
int ag_drv_gpon_tx_interrupt_controller_txier0_get(uint32_t int_src_idx, uint8_t *int_src_word);
int ag_drv_gpon_tx_interrupt_controller_txier1_set(uint32_t int_src_idx, uint8_t int_src_word);
int ag_drv_gpon_tx_interrupt_controller_txier1_get(uint32_t int_src_idx, uint8_t *int_src_word);
int ag_drv_gpon_tx_interrupt_controller_txier2_set(uint32_t int_src_idx, uint8_t int_src_word);
int ag_drv_gpon_tx_interrupt_controller_txier2_get(uint32_t int_src_idx, uint8_t *int_src_word);
int ag_drv_gpon_tx_interrupt_controller_txitr0_set(uint32_t int_src_idx, uint8_t data);
int ag_drv_gpon_tx_interrupt_controller_txitr0_get(uint32_t int_src_idx, uint8_t *data);
int ag_drv_gpon_tx_interrupt_controller_txitr1_set(uint32_t int_src_idx, uint8_t int_src_word);
int ag_drv_gpon_tx_interrupt_controller_txitr1_get(uint32_t int_src_idx, uint8_t *int_src_word);
int ag_drv_gpon_tx_interrupt_controller_txitr2_set(uint32_t int_src_idx, uint8_t int_src_word);
int ag_drv_gpon_tx_interrupt_controller_txitr2_get(uint32_t int_src_idx, uint8_t *int_src_word);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_tx_interrupt_controller_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

