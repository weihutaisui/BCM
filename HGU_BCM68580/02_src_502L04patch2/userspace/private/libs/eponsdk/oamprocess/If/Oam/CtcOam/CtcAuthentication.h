/*
* <:copyright-BRCM:2013:proprietary:epon
* 
*    Copyright (c) 2013 Broadcom 
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

#if !defined(CtcAuthentication_h)
#define CtcAuthentication_h

#include "CtcOam.h"

typedef struct
    {
    U8  ctcAuthId[MaxLoidLen];
    U8  ctcAuthPass[MaxAuthPassLen];
    } PACK CtcAuthRec;


////////////////////////////////////////////////////////////////////////////////
/// \file CtcAuthentication.h
/// \brief China Telecom CtcAuthentication module
///
///
////////////////////////////////////////////////////////////////////////////////
#include "Teknovus.h"

#ifdef BRCM_CMS_BUILD
#include "CtcOam.h"

////////////////////////////////////////////////////////////////////////////////
/// CtcAuthInit - Init the Ctc Loid Autentication module
///
///
 // Parameters:
//  \param None
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void CtcAuthInit (void);


////////////////////////////////////////////////////////////////////////////////
/// CtcAuthLoidGet:  Gets Personality CTC LOID configuration
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcAuthLoidGet (U8 BULK *buf);


////////////////////////////////////////////////////////////////////////////////
/// CtcAuthLoidSet:  Sets Personality CTC LOID configuration
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcAuthLoidSet (const U8 BULK *buf);


////////////////////////////////////////////////////////////////////////////////
/// CtcAuthPwdGet:  Gets Personality CTC password configuration
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcAuthPwdGet (U8 BULK *buf);


////////////////////////////////////////////////////////////////////////////////
/// CtcAuthPwdSet:  Sets Personality CTC password configuration
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcAuthPwdSet (const U8 BULK *buf);


////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetAuthFailure - Set Oam Ctc authorize failure
///
///
 // Parameters:
//  \param auth authorize state
///
/// \return
///      Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcSetAuthFailure (BOOL auth);


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetAuthFailure - Get Oam Ctc authorize failure
///
///
 // Parameters:
//  \param None
///
/// \return
///         authorize state
////////////////////////////////////////////////////////////////////////////////
extern
BOOL OamCtcGetAuthFailure (void);


////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleAuth - Handle authentication OAMPDU
///
/// This function handles incoming China Telecom authentication OAMPDU messages.
///
/// \param pdu  Pointer to incoming OAMPDU
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcHandleAuth (const OamCtcAuthHead BULK *pdu);

#else

#define OamCtcSetAuthFailure(auth)
#define OamCtcGetAuthFailure()     FALSE
#define OamCtcHandleAuth(p_pdu)

#endif

#endif
// End of File CtcAuthentication.h

