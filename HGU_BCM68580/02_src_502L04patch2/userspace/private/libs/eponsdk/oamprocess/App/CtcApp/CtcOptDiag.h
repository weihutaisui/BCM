/*
 *<:copyright-BRCM:2013:proprietary:epon 
 *
 *   Copyright (c) 2013 Broadcom 
 *   All Rights Reserved
 *
 * This program is the proprietary software of Broadcom and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in
 * an Authorized License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and Broadcom
 * expressly reserves all rights in and to the Software and all intellectual
 * property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 * NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 * BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 *    constitutes the valuable trade secrets of Broadcom, and you shall use
 *    all reasonable efforts to protect the confidentiality thereof, and to
 *    use this information only in connection with your use of Broadcom
 *    integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *    PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *    LIMITED REMEDY.
:>
 */

 
#if !defined(CtcOptDiag_h)
#define CtcOptDiag_h
////////////////////////////////////////////////////////////////////////////////
/// \file CtcOptDiag.c
/// \brief China Telecom ether port control module
/// \author Chen Lingyong
/// \date Nov 2, 2010
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "CtcOam.h"
#include "Sff8472.h"

#if defined(__cplusplus)
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
/// CtcOptDiagSetThd - Set optical diagnosis threshold value
///
/// This function: sets optical diagnosis threshold value
///
 // Parameters:
/// \param src  pointer to var descriptors in request
///
/// \return:
///     None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcOptDiagSetThd(const OamCtcTlvPowerMonThd BULK * src);



////////////////////////////////////////////////////////////////////////////////
/// CtcOptDiagGetThd - Get optical diagnosis threshold value from alarm Id
///
/// This function gets optical diagnosis threshold value from alarm Id
///
 // Parameters:
/// \param almId Ctc Alarm Id
/// \param cont pointer to var containers in reply
///
/// \return:
///     None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcOptDiagGetThd(U16 almId, OamCtcTlvPowerMonThd BULK * cont);



////////////////////////////////////////////////////////////////////////////////
/// CtcOptDiagPoll-Poll the optical diagnosis.
///
/// This function polls the optical diagnosis, it compares the current value to
/// ctc threshold value and push/pop the threshold alarm into message queue
/// when meat the condition.It suggest to implement this polling in less than
/// 500ms Timer, for optical diagnosis have 20 items and each poll only poll
/// one optical diagnosis item.
///
/// Parameters:
/// \param None
///
/// \return:
///     None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcOptDiagPoll(void);



////////////////////////////////////////////////////////////////////////////////
/// CtcOptDiagInit - Optical diagnosis initialization
///
/// This function initialize optical diagnosis. All the threshold should be
/// set to be the max value that can't raise any alarm if the threshold is not
/// configure by the host.
///
/// Parameters:
/// \param None
///
/// \return:
///     None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcOptDiagInit(void);

#if defined(__cplusplus)
extern "C" }
#endif

#endif //CtcOptDiag.h


