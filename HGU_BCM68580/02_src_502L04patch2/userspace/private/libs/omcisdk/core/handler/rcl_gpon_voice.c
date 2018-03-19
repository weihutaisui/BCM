/***********************************************************************
 *
 *  Copyright (c) 2007-2009 Broadcom
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

#ifdef DMP_X_ITU_ORG_VOICE_1

#include "os_defs.h"
#include "owsvc_api.h"
#include "me_handlers.h"
#include "owrut_api.h"
#include "omci_pm.h"


/*
 * RCL functions
 */

CmsRet rcl_voiceServicesObject( _VoiceServicesObject *newObj  __attribute__((unused)),
                const _VoiceServicesObject *currObj  __attribute__((unused)),
                const InstanceIdStack *iidStack  __attribute__((unused)),
                char **errorParam  __attribute__((unused)),
                CmsRet *errorCode  __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_pptpPotsUniObject( _PptpPotsUniObject *newObj,
                const _PptpPotsUniObject *currObj,
                const InstanceIdStack *iidStack  __attribute__((unused)),
                char **errorParam  __attribute__((unused)),
                CmsRet *errorCode  __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;
  UBOOL8 voiceLineArray[MAX_VOICE_LINES];
  UBOOL8 changeFlag = FALSE;
  int lineIndex;

  // Test for new object.
  if (ADD_NEW(newObj, currObj))
  {
    // Set CREATE/SET flag.
    changeFlag = TRUE;
  }
  else
  {
    // Test for set existing object.
    if (SET_EXISTING(newObj, currObj))
    {
      // Set CREATE/SET flag.
      changeFlag = TRUE;
    }
    else
    {
      // Test for delete existing object.
      if (DELETE_EXISTING(newObj, currObj))
      {
      }
      else
      {
        // Signal invalid command.
        cmsResult = CMSRET_REQUEST_DENIED;
      }
    }
  }

  // Test for CREATE/SET operation.
  if (changeFlag == TRUE)
  {
    // Attempt to find associated voice line (if known).
    if (FindVoiceME(MDMOID_PPTP_POTS_UNI, newObj->managedEntityId, voiceLineArray) == TRUE)
    {
      // Loop through voice line array to see which line is affected.
      for (lineIndex = 0;lineIndex < MAX_VOICE_LINES;lineIndex++)
      {
        // Test if voice line affected.
        if (voiceLineArray[lineIndex] == TRUE)
        {
          // Setup MDMOID_PPTP_POTS_UNI ME.
          _owapi_rut_MapPptpPotsUni(lineIndex, newObj);
        }
      }
    }
  }

  return cmsResult;
}


#ifdef DMP_X_ITU_ORG_VOICE_SIP_1
CmsRet rcl_sipUserDataObject( _SipUserDataObject *newObj __attribute__((unused)),
                const _SipUserDataObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;

  return cmsResult;
}


CmsRet rcl_sipAgentConfigDataObject( _SipAgentConfigDataObject *newObj,
                const _SipAgentConfigDataObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;
  UBOOL8 voiceLineArray[MAX_VOICE_LINES];
  UBOOL8 changeFlag = FALSE;
  int lineIndex;

  // Test for new object.
  if (ADD_NEW(newObj, currObj))
  {
    // Set CREATE/SET flag.
    changeFlag = TRUE;
  }
  else
  {
    // Test for set existing object.
    if (SET_EXISTING(newObj, currObj))
    {
      // Set CREATE/SET flag.
      changeFlag = TRUE;
    }
    else
    {
      // Test for delete existing object.
      if (DELETE_EXISTING(newObj, currObj))
      {
      }
      else
      {
        // Signal invalid command.
        cmsResult = CMSRET_REQUEST_DENIED;
      }
    }
  }

  // Test for CREATE/SET operation.
  if (changeFlag == TRUE)
  {
    // Attempt to find associated voice line (if known).
    if (FindVoiceME(MDMOID_SIP_AGENT_CONFIG_DATA, newObj->managedEntityId, voiceLineArray) == TRUE)
    {
      // Loop through voice line array to see which line is affected.
      for (lineIndex = 0;lineIndex < MAX_VOICE_LINES;lineIndex++)
      {
        // Test if voice line affected.
        if (voiceLineArray[lineIndex] == TRUE)
        {
          // Setup TR-104 RTP Profile Data object.
          _owapi_rut_MapSipAgentConfigData(lineIndex, newObj);
        }
      }
    }
  }

  return cmsResult;
}


CmsRet rcl_sipAgentPmHistoryDataObject( _SipAgentPmHistoryDataObject *newObj __attribute__((unused)),
                const _SipAgentPmHistoryDataObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_sipCallInitPmHistoryDataObject( _SipCallInitPmHistoryDataObject *newObj __attribute__((unused)),
                const _SipCallInitPmHistoryDataObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}
#endif /* DMP_X_ITU_ORG_VOICE_SIP_1 */

#ifdef DMP_X_ITU_ORG_VOICE_MGC_1

CmsRet rcl_mgcConfigDataObject( _MgcConfigDataObject *newObj,
                const _MgcConfigDataObject *currObj,
                const InstanceIdStack *iidStack,
                char **errorParam,
                CmsRet *errorCode)
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_mgcPmHistoryDataObject( _MgcPmHistoryDataObject *newObj,
                const _MgcPmHistoryDataObject *currObj,
                const InstanceIdStack *iidStack,
                char **errorParam,
                CmsRet *errorCode)
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

#endif /* DMP_X_ITU_ORG_VOICE_MGC_1 */

CmsRet rcl_voIpVoiceCtpObject( _VoIpVoiceCtpObject *newObj,
                const _VoIpVoiceCtpObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;
  UBOOL8 changeFlag = FALSE;

  // Test for new object.
  if (ADD_NEW(newObj, currObj))
  {
    // Set CREATE/SET flag.
    changeFlag = TRUE;
  }
  else
  {
    // Test for set existing object.
    if (SET_EXISTING(newObj, currObj))
    {
      // Set CREATE/SET flag.
      changeFlag = TRUE;
    }
    else
    {
      // Test for delete existing object.
      if (DELETE_EXISTING(newObj, currObj))
      {
      }
      else
      {
        // Signal invalid command.
        cmsResult = CMSRET_REQUEST_DENIED;
      }
    }
  }

  // Test for CREATE/SET operation.
  if (changeFlag == TRUE)
  {
    // Test for invalid 
    if (newObj->signallingCode != VOIP_VOICE_CTP_LOOP_START)
    {
      // Signal invalid command.
      cmsResult = CMSRET_REQUEST_DENIED;
    }
  }

  return cmsResult;
}

CmsRet rcl_voIpMediaProfileObject( _VoIpMediaProfileObject *newObj,
                const _VoIpMediaProfileObject *currObj,
                const InstanceIdStack *iidStack  __attribute__((unused)),
                char **errorParam  __attribute__((unused)),
                CmsRet *errorCode  __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;
  UBOOL8 voiceLineArray[MAX_VOICE_LINES];
  UBOOL8 changeFlag = FALSE;
  int lineIndex;

  // Test for new object.
  if (ADD_NEW(newObj, currObj))
  {
    // Set CREATE/SET flag.
    changeFlag = TRUE;
  }
  else
  {
    // Test for set existing object.
    if (SET_EXISTING(newObj, currObj))
    {
      // Set CREATE/SET flag.
      changeFlag = TRUE;
    }
    else
    {
      // Test for delete existing object.
      if (DELETE_EXISTING(newObj, currObj))
      {
      }
      else
      {
        // Signal invalid command.
        cmsResult = CMSRET_REQUEST_DENIED;
      }
    }
  }

  // Test for CREATE/SET operation.
  if (changeFlag == TRUE)
  {
    // Attempt to find associated voice line (if known).
    if (FindVoiceME(MDMOID_VO_IP_MEDIA_PROFILE, newObj->managedEntityId, voiceLineArray) == TRUE)
    {
      // Loop through voice line array to see which line is affected.
      for (lineIndex = 0;lineIndex < MAX_VOICE_LINES;lineIndex++)
      {
        // Test if voice line affected.
        if (voiceLineArray[lineIndex] == TRUE)
        {
          // Setup MDMOID_VO_IP_MEDIA_PROFILE ME.
          _owapi_rut_MapVoIpMediaProfile(lineIndex, newObj);
        }
      }
    }
  }

  return cmsResult;
}


CmsRet rcl_voiceServiceObject( _VoiceServiceObject *newObj,
                const _VoiceServiceObject *currObj,
                const InstanceIdStack *iidStack  __attribute__((unused)),
                char **errorParam  __attribute__((unused)),
                CmsRet *errorCode  __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;
  UBOOL8 voiceLineArray[MAX_VOICE_LINES];
  UBOOL8 changeFlag = FALSE;
  int lineIndex;

  // Test for new object.
  if (ADD_NEW(newObj, currObj))
  {
    // Set CREATE/SET flag.
    changeFlag = TRUE;
  }
  else
  {
    // Test for set existing object.
    if (SET_EXISTING(newObj, currObj))
    {
      // Set CREATE/SET flag.
      changeFlag = TRUE;
    }
    else
    {
      // Test for delete existing object.
      if (DELETE_EXISTING(newObj, currObj))
      {
      }
      else
      {
        // Signal invalid command.
        cmsResult = CMSRET_REQUEST_DENIED;
      }
    }
  }

  // Test for CREATE/SET operation.
  if (changeFlag == TRUE)
  {
    // Attempt to find associated voice line (if known).
    if (FindVoiceME(MDMOID_VOICE_SERVICE, newObj->managedEntityId, voiceLineArray) == TRUE)
    {
      // Loop through voice line array to see which line is affected.
      for (lineIndex = 0;lineIndex < MAX_VOICE_LINES;lineIndex++)
      {
        // Test if voice line affected.
        if (voiceLineArray[lineIndex] == TRUE)
        {
          // Setup TR-104 Voice Service Profile object.
          _owapi_rut_MapVoiceServiceProfile(lineIndex, newObj);
        }
      }
    }
  }

  return cmsResult;
}


CmsRet rcl_rtpProfileDataObject( _RtpProfileDataObject *newObj,
                const _RtpProfileDataObject *currObj,
                const InstanceIdStack *iidStack  __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;
  UBOOL8 voiceLineArray[MAX_VOICE_LINES];
  UBOOL8 changeFlag = FALSE;
  int lineIndex;

  // Test for new object.
  if (ADD_NEW(newObj, currObj))
  {
    // Set CREATE/SET flag.
    changeFlag = TRUE;
  }
  else
  {
    // Test for set existing object.
    if (SET_EXISTING(newObj, currObj))
    {
      // Set CREATE/SET flag.
      changeFlag = TRUE;
    }
    else
    {
      // Test for delete existing object.
      if (DELETE_EXISTING(newObj, currObj))
      {
      }
      else
      {
        // Signal invalid command.
        cmsResult = CMSRET_REQUEST_DENIED;
      }
    }
  }

  // Test for CREATE/SET operation.
  if (changeFlag == TRUE)
  {
    // Attempt to find associated voice line (if known).
    if (FindVoiceME(MDMOID_RTP_PROFILE_DATA, newObj->managedEntityId, voiceLineArray) == TRUE)
    {
      // Loop through voice line array to see which line is affected.
      for (lineIndex = 0;lineIndex < MAX_VOICE_LINES;lineIndex++)
      {
        // Test if voice line affected.
        if (voiceLineArray[lineIndex] == TRUE)
        {
          // Setup TR-104 RTP Profile Data object.
          _owapi_rut_MapRtpProfileData(newObj);
        }
      }
    }
  }

  return cmsResult;
}


#ifdef DMP_X_ITU_ORG_VOICE_SIP_1
CmsRet rcl_voIpAppServiceProfileObject( _VoIpAppServiceProfileObject *newObj,
                const _VoIpAppServiceProfileObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;
  UBOOL8 voiceLineArray[MAX_VOICE_LINES];
  UBOOL8 changeFlag = FALSE;
  int lineIndex;

  // Test for new object.
  if (ADD_NEW(newObj, currObj))
  {
    // Set CREATE/SET flag.
    changeFlag = TRUE;
  }
  else
  {
    // Test for set existing object.
    if (SET_EXISTING(newObj, currObj))
    {
      // Set CREATE/SET flag.
      changeFlag = TRUE;
    }
    else
    {
      // Test for delete existing object.
      if (DELETE_EXISTING(newObj, currObj))
      {
      }
      else
      {
        // Signal invalid command.
        cmsResult = CMSRET_REQUEST_DENIED;
      }
    }
  }

  // Test for CREATE/SET operation.
  if (changeFlag == TRUE)
  {
    // Attempt to find associated voice line (if known).
    if (FindVoiceME(MDMOID_VO_IP_APP_SERVICE_PROFILE, newObj->managedEntityId, voiceLineArray) == TRUE)
    {
      // Loop through voice line array to see which line is affected.
      for (lineIndex = 0;lineIndex < MAX_VOICE_LINES;lineIndex++)
      {
        // Test if voice line affected.
        if (voiceLineArray[lineIndex] == TRUE)
        {
          // Setup MDMOID_VO_IP_APP_SERVICE_PROFILE ME.
          _owapi_rut_MapVoIpApplServiceProfile(lineIndex, newObj);
        }
      }
    }
  }

  return cmsResult;
}


CmsRet rcl_voiceFeatureAccessCodesObject( _VoiceFeatureAccessCodesObject *newObj,
                const _VoiceFeatureAccessCodesObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;

  // Test for new object.
  if (ADD_NEW(newObj, currObj))
  {
  }
  else
  {
    // Test for set existing object.
    if (SET_EXISTING(newObj, currObj))
    {
    }
    else
    {
      // Test for delete existing object.
      if (DELETE_EXISTING(newObj, currObj))
      {
      }
      else
      {
        // Signal invalid command.
        cmsResult = CMSRET_REQUEST_DENIED;
      }
    }
  }

  return cmsResult;
}

CmsRet rcl_networkDialPlanTableObject( _NetworkDialPlanTableObject *newObj,
                const _NetworkDialPlanTableObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;

  // Test for set existing object.
  if (SET_EXISTING(newObj, currObj))
  {
    // Attempt to set Network Dial Plan via CMS message to voice stack.
    cmsResult = rutGpon_SetDialPlan(newObj, iidStack);
  }
  else
  {
    // Test for delete existing object.
    if (DELETE_EXISTING(newObj, currObj))
    {
      // Note: Stack continues to use last Provisioned value or default
    }
  }

  // Return operation result.
  return cmsResult;
}

CmsRet rcl_dialPlanTableObject( _DialPlanTableObject *newObj __attribute__((unused)),
                const _DialPlanTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}
#endif /* DMP_X_ITU_ORG_VOICE_SIP_1 */

CmsRet rcl_voIpLineStatusObject( _VoIpLineStatusObject *newObj __attribute__((unused)),
                const _VoIpLineStatusObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_callControlPmHistoryDataObject( _CallControlPmHistoryDataObject *newObj __attribute__((unused)),
                const _CallControlPmHistoryDataObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_rtpPmHistoryDataObject( _RtpPmHistoryDataObject *newObj,
                const _RtpPmHistoryDataObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;

    // Test for new PM object.
    if (ADD_NEW(newObj, currObj))
    {
        // do nothing
    }
    // Test for set existing PM object.
    else if (SET_EXISTING(newObj, currObj))
    {
        CmsRet found = omci_pm_findEntryById(MDMOID_RTP_PM_HISTORY_DATA,
                                             newObj->managedEntityId);

        if (found == CMSRET_OBJECT_NOT_FOUND)
        {
            ret = omci_pm_createObject(MDMOID_RTP_PM_HISTORY_DATA,
                                       newObj->managedEntityId,
                                       0,
                                       newObj->thresholdDataId);
        }
        else
        {
            omci_pm_setObject(MDMOID_RTP_PM_HISTORY_DATA,
                              newObj->managedEntityId,
                              newObj->thresholdDataId);
        }
    }
    // Test for delete existing PM object.
    if (DELETE_EXISTING(newObj, currObj))
    {
        omci_pm_deleteObject(MDMOID_RTP_PM_HISTORY_DATA,
                             currObj->managedEntityId);
    }

    return ret;
}

CmsRet rcl_voIpConfigDataObject( _VoIpConfigDataObject *newObj,
                const _VoIpConfigDataObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;
  UBOOL8 changeFlag = FALSE;

  // Test for new object.
  if (ADD_NEW(newObj, currObj))
  {
    // Set CREATE/SET flag.
    changeFlag = TRUE;
  }
  else
  {
    // Test for set existing object.
    if (SET_EXISTING(newObj, currObj))
    {
      // Set CREATE/SET flag.
      changeFlag = TRUE;
    }
    else
    {
      // Test for delete existing object.
      if (DELETE_EXISTING(newObj, currObj))
      {
      }
      else
      {
        // Signal invalid command.
        cmsResult = CMSRET_REQUEST_DENIED;
      }
    }
  }

  // Test for CREATE/SET operation.
  if (changeFlag == TRUE)
  {
    // Setup MDMOID_VO_IP_CONFIG_DATA ME.
    _owapi_rut_MapVoIpConfigData(newObj);
  }

  return cmsResult;
}

CmsRet rcl_sipConfigPortalObject(
  _SipConfigPortalObject *newObj __attribute__((unused)),
  const _SipConfigPortalObject *currObj __attribute__((unused)),
  const InstanceIdStack *iidStack __attribute__((unused)),
  char **errorParam __attribute__((unused)),
  CmsRet *errorCode __attribute__((unused)))
{
  return CMSRET_SUCCESS;
}

CmsRet rcl_mgcConfigPortalObject(
  _MgcConfigPortalObject *newObj __attribute__((unused)),
  const _MgcConfigPortalObject *currObj __attribute__((unused)),
  const InstanceIdStack *iidStack __attribute__((unused)),
  char **errorParam __attribute__((unused)),
  CmsRet *errorCode __attribute__((unused)))
{
  return CMSRET_SUCCESS;
}

#endif /* DMP_X_ITU_ORG_VOICE_1 */
