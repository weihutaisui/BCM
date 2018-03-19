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

////////////////////////////////////////////////////////////////////////////////
/// \file CtcAuthentication.c
/// \brief China Telecom extended OAM authentication module
/// \author
/// \date Feb 25, 2009
///
///
////////////////////////////////////////////////////////////////////////////////
#ifdef BRCM_CMS_BUILD
#include "Teknovus.h"
#include "Build.h"
#include "Alarms.h"
#include "Oam.h"
#include "OamUtil.h"
#include "CtcOam.h"
#include "CtcOnuOam.h"
#include "CtcAuthentication.h"
#include "cms_log.h"
#include "cms_core.h"
#include "eponctl_api.h"


#if REGISTER_NACK_BACKOFF_SUPPORT

static BOOL BULK authFailure = FALSE;
#endif


#if REGISTER_NACK_BACKOFF_SUPPORT
////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleAuthReq - Handle authentication request
///
/// \param msg  Pointer to incoming request OAMPDU
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleAuthReq (const OamCtcAuthHead BULK *msg)
    {
    OamCtcAuthHead BULK *pdu;
    OamCtcAuthResponsePdu BULK *res;
    XponObject *obj=NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;
    U16 pduLen;

    pdu = (OamCtcAuthHead BULK *)oamParser.reply.cur;
    res =(OamCtcAuthResponsePdu BULK *)(pdu + 1);

    pdu->ext = OamCtcAuthentication;
    pdu->opcode = OamCtcAuthResponse;

    if (*((U8 *)(msg + 1)) == AuthTypeLoid)
        {
        pduLen = sizeof(OamCtcAuthResponsePdu);
        pdu->length   = OAM_HTONS(pduLen);
        res->authType = AuthTypeLoid;

        if ((ret = cmsLck_acquireLock()) != CMSRET_SUCCESS)
        {
           cmsLog_error("failed to get lock, ret=%d", ret);
           return;
        }
     
        if ((ret = cmsObj_get(MDMOID_XPON, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
        {
           cmsLog_error("get of MDMOID_XPON object failed, ret=%d", ret);
           cmsLck_releaseLock();
           return;
        }

        if(obj->authenticationId != NULL)
            {
            memcpy(res->loid,
                  CtcStringSwap((U8 *)obj->authenticationId, MaxLoidLen),
                  MaxLoidLen);
            }
            else
            {
            memset(res->loid, 0x00, MaxLoidLen);     
            }

        if(obj->authenticationPassword != NULL)
            {
            memcpy(res->password,
                  CtcStringSwap((U8 *)obj->authenticationPassword, MaxAuthPassLen),
                  MaxAuthPassLen);
            }
        else
            {
            memset(res->password, 0x00, MaxAuthPassLen);     
            }

        cmsObj_free((void **) &obj);
        cmsLck_releaseLock();
		
        StreamSkip(&oamParser.reply, sizeof (OamCtcAuthHead)
                   + sizeof (OamCtcAuthResponsePdu));
        OamTransmit();
        }
    else
        {
        pduLen = sizeof(U16);
        pdu->length  = OAM_HTONS(pduLen);
        res->authType = AuthTypeOther;
        res->loid[0]  = AuthTypeLoid; //Desired auth type
        StreamSkip(&oamParser.reply, sizeof (OamCtcAuthHead) + sizeof (U16));
        OamTransmit();
        }
    } // OamCtcHandleAuthReq
#endif // REGISTER_NACK_BACKOFF_SUPPORT


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
//extern
void OamCtcSetAuthFailure (BOOL auth)
    {
    authFailure = auth;
    if (auth)
    	{
       cmsLog_debug("eponStack_CtlSetSilence time = %d", CtcDefaultSilenceTime);
       eponStack_CtlSetSilence(TRUE, CtcDefaultSilenceTime);
    	}
    }//OamCtcSetAuthFailure


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
//extern
BOOL OamCtcGetAuthFailure (void)
    {
    return authFailure;
    }//OamCtcSetAuthFailure


////////////////////////////////////////////////////////////////////////////////
void OamCtcHandleAuth (const OamCtcAuthHead BULK *pdu)
    {
#if REGISTER_NACK_BACKOFF_SUPPORT
    cmsLog_notice("opcode = %d", pdu->opcode);

    switch (pdu->opcode)
        {
        case OamCtcAuthRequest:
            OamCtcHandleAuthReq (pdu);
            break;

        case OamCtcAuthAckSuccess:
            OamCtcSetAuthFailure(FALSE);
            break;

        case OamCtcAuthAckFailure:
            OamCtcSetAuthFailure(TRUE);
            break;

        default:
            break;
        }
#else
    UNUSED (pdu);
#endif // REGISTER_NACK_BACKOFF_SUPPORT
    }

#endif

// CtcAuthentication.c
