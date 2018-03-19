/******************************************************************************
 *
 * Copyright (c) 2015   Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2009:proprietary:standard
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
 *
 ************************************************************************/

/***************************************************************************
 *
 *    Copyright (c) 2008-2009, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Description: MoCA userland daemon
 *
 ***************************************************************************/

#ifndef _MOCAD_MPS_H_
#define _MOCAD_MPS_H_

/*
 * Copyright 2015 Broadcom
 * XX ToDo
 */

#ifndef MPS_EN
#define MPS_EN 1
#endif

#ifndef MPS_DEBUG_EN
#define MPS_DEBUG_EN defined(__EMU_HOST_20__) && !defined(__LINUX__)
#endif

struct mocad_ctx;


void mocad_mps_restore_defaults(struct mocad_ctx *ctx);

int mocad_mps_button_press(struct mocad_ctx *ctx);
void mocad_mps_stop(struct mocad_ctx *ctx);
void mocad_mps_stop_unpaired_timer(struct mocad_ctx *ctx);
void mocad_mps_reset(struct mocad_ctx *ctx);
void mocad_clear_mps_data(struct mocad_ctx *ctx);
void mocad_reread_mps_data(struct mocad_ctx *ctx);

void mocad_handle_mps_init_scan_rec(struct mocad_ctx *ctx);
void mocad_handle_mps_trigger(struct mocad_ctx *ctx);
void mocad_handle_mps_privacy_changed(struct mocad_ctx *ctx);
void mocad_handle_mps_admission_nochange(struct mocad_ctx *ctx);
void mocad_handle_mps_pair_failed(struct mocad_ctx *ctx);
void mocad_handle_mps_request_mpskey(struct mocad_ctx *ctx, struct moca_mps_request_mpskey *pubkey);
void mocad_handle_mps_data_ready(struct mocad_ctx *ctx);
void mocad_mps_link_state(struct mocad_ctx *ctx, int new_state);

void mocad_mps_init(struct mocad_ctx *ctx);

#endif
