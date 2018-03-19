/***********************************************************************
 *
 *  Copyright (c) 2006-2010 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:omcid

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
 ************************************************************************/
#ifndef __RUT_GPON_VOICE_H__
#define __RUT_GPON_VOICE_H__

/*!\file rut_gpon_voice.h
 * \brief System level interface functions for GPON voice functionality.
 *
 * The functions in this file should only be called by
 * RCL, STL, and other RUT functions.
 */

#include "rut_gpon.h"

//=======================  Public GPON voice functions ========================

#ifdef DMP_X_ITU_ORG_GPON_1
#ifdef DMP_X_ITU_ORG_VOICE_1


/* Used by rut_gpon_voice.c */
#define MAX_DIAL_PLAN_STR_SIZE 200
#define MAX_DIAL_PLAN_TOKEN_SIZE 28
#define DIAL_PLAN_ADD_COMMAND 1
#define DIAL_PLAN_REMOVE_COMMAND 0

#define VOICE_STATUS_REGISTER_STR     "Registering"
#define VOICE_STATUS_REGISTER_VAL     0
#define VOICE_STATUS_ERROR_STR        "Error"
#define VOICE_STATUS_ERROR_VAL        9
#define VOICE_STATUS_DISABLED_STR     "Disabled"
#define VOICE_STATUS_DISABLED_VAL     10
#define VOICE_STATUS_UP_STR           "Up"
#define VOICE_STATUS_UP_VAL           2

/* Used by rutapi_cms_voice.c and rut_gpon_voice.c */
#define VOICE_STATE_IDLE_STR          "Idle"
#define VOICE_STATE_IDLE_VAL          1
#define VOICE_STATE_CALLING_STR       "Calling"
#define VOICE_STATE_CALLING_VAL       3
#define VOICE_STATE_RINGING_STR       "Ringing"
#define VOICE_STATE_RINGING_VAL       4
#define VOICE_STATE_CONNECTING_STR    "Connecting"
#define VOICE_STATE_CONNECTING_VAL    6
#define VOICE_STATE_INCALL_STR        "InCall"
#define VOICE_STATE_INCALL_VAL        7
#define VOICE_STATE_DISCONNECTING_STR "Disconnecting"
#define VOICE_STATE_DISCONNECTING_VAL 8

#define VOICE_STATE_RINGBACK_VAL      5

#define VOIP_VOICE_CTP_LOOP_START     1


#define MAX_VOICE_LINES 2

#define OMCI_COMPLETE_WAIT_SECS 3
#define VOIP_IF_NAME_STRING     "omci"
#define VOIP_STOP_ADDR_STRING   "0.0.0.0"

#define MAX_CODEC_RECS          20 /* 19 from G.988 + G726-32 */

/* Used by rutapi_cms_voice.c and rut_gpon_voice.c */
enum VOIP_CONFIG_ID
{
  VOIP_CONFIG_ID_NULL = 0,
  VOIP_CONFIG_ID_PROXY_SERVER_ADDR,
  VOIP_CONFIG_ID_OUTBOUND_PROXY_ADDR,
  VOIP_CONFIG_ID_HOST_PART_URI,
  VOIP_CONFIG_ID_SIP_REGISTRAR,
  VOIP_CONFIG_ID_DIRECT_CONNECT_URI,
  VOIP_CONFIG_ID_CONFERENCE_FACTORY_URI,
  VOIP_CONFIG_ID_AUTH_SEC_METHOD,
  VOIP_CONFIG_ID_USER_PART_AOR,
  VOIP_CONFIG_ID_VOICEMAIL_SERVER_SIP_URI,
  VOIP_CONFIG_ID_TCP_UDP,
  VOIP_CONFIG_ID_GEM_INTERWORKING_TP,
};

typedef enum VOIP_CONFIG_ID VOIP_CONFIG_ID_ENUM;


/* Used by rutapi_cms_voice.c */
typedef enum
{
  VOIP_CONFIG_METHOD_DNC = 0,
  VOIP_CONFIG_METHOD_OMCI,
  VOIP_CONFIG_METHOD_FILE,
  VOIP_CONFIG_METHOD_TR69,
  VOIP_CONFIG_METHOD_IETF,
} VOIP_CONFIG_METHODS;

#define CALL_WAITING_MASK 1
#define CALLER_ID_MASK 3
#define BLOCK_ANON_CALLER_ID 0x20
#define CALL_XFER_MASK 0x0002
#define CALL_DO_NOT_DISTURB_MASK 0x0010
#define CALL_MWI_MASK 0x000F


CmsRet rutGpon_SetDialPlan(NetworkDialPlanTableObject* dialPlanRecPtr, const InstanceIdStack* iidStackPtr);
UBOOL8 FindVoiceME(UINT16 obj_Type, UINT16 obj_ID, UBOOL8 voiceLineArray[]);
CmsRet rutGpon_setupOmciVoipObjects(void);


#endif // #ifdef DMP_X_ITU_ORG_VOICE_1
#endif // #ifdef DMP_X_ITU_ORG_GPON_1
#endif /* __RUT_GPON_VOICE_H__ */
