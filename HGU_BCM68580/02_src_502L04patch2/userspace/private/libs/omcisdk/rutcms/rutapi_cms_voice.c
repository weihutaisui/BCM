/***********************************************************************
 *
 *  Copyright (c) 2017 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2017:proprietary:omcid

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

/*****************************************************************************
*    Description:
*
*      OMCI Wrapper (OW) run-time utility (RUT) implementation based on CMS.
*      Functions in this file are VOIP specific.
*
*****************************************************************************/


#ifdef DMP_X_ITU_ORG_VOICE_1

#include "owsvc_api.h"

#include "mdm.h"
#include "mdm_object.h"
#include "cms_obj.h"
#include "cms_util.h"
#include "cms_msg.h"

#include "mdmlite_api.h"
#include "me_handlers.h"
#include "rut_pon_voice.h"
#include "rut_voice.h"

#include "ctlutils_api.h"
#include "owrut_api.h"
#include "owrut_cmsdefs.h"


#define OMCI_FAX_PASS_THRU 0

typedef struct {
   int  countryCode;
   char *tr104Txt;
} E164_COUNTRY_CODE_MAP;

#ifdef DMP_VOICE_SERVICE_1

#define ITU_RESERVED_COUNTRY_CODE 999

static E164_COUNTRY_CODE_MAP countryCodeMap[] =
{
   { 61                        , "AU"},
   { 32                        , "BE"},
   { 55                        , "BR"},
   { 56                        , "CL"},
   { 86                        , "CN"},
   { 420                       , "CZ"},
   { 45                        , "DK"},
   { 358                       , "FI"},
   { 33                        , "FR"},
   { 49                        , "DE"},
   { 36                        , "HU"},
   { 91                        , "IN"},
   { 39                        , "IT"},
   { 81                        , "JP"},
   { 31                        , "NL"},
   { 64                        , "NZ"},
   { 1                         , "US"},
   { 48                        , "PL"},
   { 34                        , "ES"},
   { 46                        , "SE"},
   { 47                        , "NO"},
   { 41                        , "CH"},
   { 44                        , "GB"},
   { 971                       , "AE"},
   { ITU_RESERVED_COUNTRY_CODE , "XX"}
};

#define IETF_PT_CODEC_DYNAMIC_LOWEST  96
#define IETF_PT_CODEC_DYNAMIC_HIGHEST 127

/* RFC3551 Encoding Names in comments. */
static char* rfc3551PayloadTypeMap [MAX_CODEC_RECS] =
{
   CODEC_PCMU_STR,        /* PCMU */
   CODEC_UNKNOWN_STR,     /* reserved */
   CODEC_UNKNOWN_STR,     /* reserved */
   CODEC_GSMAMR_795_STR,  /* GSM */
   CODEC_G7231_63_STR,    /* G723 */
   CODEC_UNKNOWN_STR,     /* DVI4 (8k) */
   CODEC_UNKNOWN_STR,     /* DVI4 (16k) */
   CODEC_UNKNOWN_STR,     /* LPC */
   CODEC_PCMA_STR,        /* PCMA */
   CODEC_G722_MODE_1_STR, /* G722 */
   CODEC_UNKNOWN_STR,     /* L16 (2 channel) */
   CODEC_UNKNOWN_STR,     /* L16 */
   CODEC_UNKNOWN_STR,     /* QCELP */
   CODEC_UNKNOWN_STR,     /* CN */
   CODEC_UNKNOWN_STR,     /* MPA */
   CODEC_G728_STR,        /* G728 */
   CODEC_UNKNOWN_STR,     /* DVI4 (11k) */
   CODEC_UNKNOWN_STR,     /* DVI4 (22k) */
   CODEC_G729_STR,        /* G728 */
   CODEC_G726_32_STR      /* G726-32 */
};

#define LOWEST_PRIORITY_CODEC 4
#endif /* DMP_VOICE_SERVICE_1 */


#ifdef DMP_VOICE_SERVICE_1

/*****************************************************************************
*  FUNCTION:  rutGpon_MapCodecSelection
*  PURPOSE:   Map codec selection.
*  PARAMETERS:
*      priority - codec selection order as defined in VoIP media profile ME.
*      omciVoIpMediaProfileObjectPtr - pointer to OMCI VoIpMediaProfileObject.
*      objCodecListItemPtr - pointer to TR-104 VoiceLineCodecListObject.
*  RETURNS:
*      1: successful operation; 0 otherwise.
*  NOTES:
*      None.
*****************************************************************************/
static int rutGpon_MapCodecSelection(int priority,
  VoIpMediaProfileObject* omciVoIpMediaProfileObjectPtr,
  VoiceLineCodecListObject* objCodecListItemPtr)
{
    UBOOL8 isSupported;
    int payloadType, period, silence;

    switch (priority)
    {
       case 1:
       {
           payloadType = (omciVoIpMediaProfileObjectPtr->codecSelection1) %
            IETF_PT_CODEC_DYNAMIC_LOWEST;
           period = omciVoIpMediaProfileObjectPtr->packetPeriodSelection1;
           silence = omciVoIpMediaProfileObjectPtr->silenceSuppression1;
           break;
       }
       case 2:
       {
           payloadType = (omciVoIpMediaProfileObjectPtr->codecSelection2) %
             IETF_PT_CODEC_DYNAMIC_LOWEST;
           period = omciVoIpMediaProfileObjectPtr->packetPeriodSelection1;
           silence = omciVoIpMediaProfileObjectPtr->silenceSuppression1;
           break;
       }
       case 3:
       {
           payloadType = (omciVoIpMediaProfileObjectPtr->codecSelection3) %
             IETF_PT_CODEC_DYNAMIC_LOWEST;
           period = omciVoIpMediaProfileObjectPtr->packetPeriodSelection1;
           silence = omciVoIpMediaProfileObjectPtr->silenceSuppression1;
           break;
       }
       case 4:
       {
           payloadType = (omciVoIpMediaProfileObjectPtr->codecSelection4) %
             IETF_PT_CODEC_DYNAMIC_LOWEST;
           period = omciVoIpMediaProfileObjectPtr->packetPeriodSelection1;
           silence = omciVoIpMediaProfileObjectPtr->silenceSuppression1;
           break;
       }
       default:
          cmsLog_error("Codec Priority %d not supported", priority);
          return 0;
    }

    /* Check for nonsense */
    if (payloadType >= MAX_CODEC_RECS)
    {
        cmsLog_error("Payload Type %d out-of-range", payloadType);
        return 0;
    }

    /* RFC-specified but not supported */
    if (strcmp(rfc3551PayloadTypeMap[payloadType], CODEC_UNKNOWN_STR) == 0)
    {
        cmsLog_error("Payload Type %d not supported", payloadType);
        return 0;
    }

    /* Supported by some configurations but not all */
    rutVoice_validateCodec(rfc3551PayloadTypeMap[payloadType], &isSupported);
    if (!isSupported)
    {
        cmsLog_error("Payload Type %d not supported", payloadType);
        return 0;
    }

    /* Supported and changeable */
    if (strcmp(rfc3551PayloadTypeMap[payloadType], objCodecListItemPtr->codec)
      == 0)
    {
        char packetizationPeriod[16];

        objCodecListItemPtr->enable = TRUE;
        objCodecListItemPtr->priority = priority;
        objCodecListItemPtr->silenceSuppression = silence;

        sprintf(packetizationPeriod, "%d", period);
        CMSMEM_REPLACE_STRING_FLAGS(objCodecListItemPtr->packetizationPeriod,
          packetizationPeriod, 0);
        return 1; /* found the codec, return 1 */
    }
    else
    {
        objCodecListItemPtr->priority = LOWEST_PRIORITY_CODEC + 1;
    }
    return 0;
}
#endif /* DMP_VOICE_SERVICE_1 */

/*****************************************************************************
*  FUNCTION:  owapi_rut_MapNetworkDialPlan
*  PURPOSE:   Map OMCI Network dial plan table ME into TR-104 object.
*  PARAMETERS:
*      digitMap - OMCI dial plan tokens.
*      critDigitTimer - critical dial timeout as defined in OMCI.
*      partDigitTimer - partial dial timeout as defined in OMCI.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_MapNetworkDialPlan(UINT8 digitMap[] __attribute__((unused)),
  UINT32 critDigitTimer __attribute__((unused)),
  UINT32 partDigitTimer __attribute__((unused)))
{
#ifdef DMP_VOICE_SERVICE_1
    void* objService = NULL;
     VoiceObject* objVoice = NULL;
     VoiceProfObject* objVoiceProfile = NULL;
     InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
     InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
     InstanceIdStack iidProfile = EMPTY_INSTANCE_ID_STACK;
     CmsRet cmsResult;
     UBOOL8 isModified = FALSE;

     if ((cmsResult = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
       != CMSRET_SUCCESS)
     {
         cmsLog_error("Failed to get lock, ret=%d", cmsResult);
         return;
     }

     // Get MDMOID_SERVICES object (InternetGatewayDevice.Services).
     if (cmsObj_get(MDMOID_SERVICES, &iidService, 0, (void**)&objService)
       != CMSRET_SUCCESS)
     {
         // Uninitialized
         cmsLck_releaseLock();
         return;
     }

     // OMCI does not distinguish between Voice Profiles as such so
     while (cmsObj_getNextInSubTree(MDMOID_VOICE, &iidService, &iidVoice,
       (void**)&objVoice) == CMSRET_SUCCESS)
     {
        while (cmsObj_getNextInSubTree(MDMOID_VOICE_PROF, &iidVoice,
          &iidProfile, (void**)&objVoiceProfile) == CMSRET_SUCCESS)
        {
           if (digitMap[0] != 0)
           {
              // Setup digit map from OAM input array.
              CMSMEM_REPLACE_STRING_FLAGS(objVoiceProfile->digitMap,
                (void*)digitMap, 0);

              // Enable digit map.
              objVoiceProfile->digitMapEnable = TRUE;
              isModified = TRUE;
           }

           // Update Digit Timers
           if (critDigitTimer > 0)
           {
              objVoiceProfile->X_BROADCOM_COM_CriticalDigitTimer =
                critDigitTimer;
              isModified = TRUE;
           }

           if (partDigitTimer > 0)
           {
              objVoiceProfile->X_BROADCOM_COM_PartialDigitTimer =
                partDigitTimer;
              isModified = TRUE;
           }

           if (isModified)
           {
              // Write modified-object back to CMS.
              cmsResult = cmsObj_set(objVoiceProfile, &iidProfile);
              if (cmsResult != CMSRET_SUCCESS)
              {
                 // Log error.
                 cmsLog_error("Failed to set Voice Profile (%d)", cmsResult);
              }
              isModified = FALSE;
           }

           // Free MDMOID_VOICE_PROF object.
           cmsObj_free((void**)&objVoiceProfile);
        }
        // Release MDMOID_VOICE object.
        cmsObj_free((void**)&objVoice);
     }
     // Release MDMOID_SERVICES object.
     cmsObj_free((void**)&objService);
     cmsLck_releaseLock();

     cmsLog_debug("===> %s, isModified=%d\n",
     __FUNCTION__, isModified);
#endif /* DMP_VOICE_SERVICE_1 */
}

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
/*****************************************************************************
*  FUNCTION:  GetManagementProtocol
*  PURPOSE:   Get voice management protocol, it may be OMCI/TR69/OAM.
*  PARAMETERS:
*      protPtr - pointer to the protocol string buffer.
*      length - protocol string length.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet GetManagementProtocol(char* protPtr, unsigned int length)
{
    CmsRet ret = CMSRET_SUCCESS;
    VoiceObject *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    /* Get VoiceService.{1}. Object */
    if ((ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void**)&obj))
      != CMSRET_SUCCESS)
    {
        cmsLog_error( "Can't retrieve voice service object\n" );
        return (ret);
    }
    else
    {
        snprintf((char*)protPtr, length, "%s",
          obj->X_BROADCOM_COM_ManagementProtocol);
    }

    /* Free object */
    cmsObj_free((void **) &obj);

    return (ret);
}
#endif    // DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1

/*****************************************************************************
*  FUNCTION:  owapi_rut_MapPptpPotsUni
*  PURPOSE:   Map OMCI PPTP POTS UNI ME attributes into TR-104 object.
*  PARAMETERS:
*      voiceLineIndex - voice line id.
*      omciPptpPotsUniObjectPtr - pointer to PptpPotsUniObject.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_MapPptpPotsUni(int voiceLineIndex __attribute__((unused)),
  PptpPotsUniObject* omciPptpPotsUniObjectPtr __attribute__((unused)))
{
  #ifdef DMP_VOICE_SERVICE_1
    void* objService = NULL;
    void* objVoice = NULL;
    void* objVoiceProfile = NULL;
    void* objLine = NULL;
    InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidProfile = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoiceLine = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoiceProcessing = EMPTY_INSTANCE_ID_STACK;
    VoiceLineProcessingObject* objVoiceProcessingPtr;
    CmsRet cmsResult;
    int tempLineIndex;
    UBOOL8 objFound = FALSE;

    if ((cmsResult = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", cmsResult);
        return;
    }

    // Get MDMOID_SERVICES object (InternetGatewayDevice.Services).
    if (cmsObj_get(MDMOID_SERVICES, &iidService, 0, (void**)&objService)
      == CMSRET_SUCCESS)
    {
      // Get MDMOID_VOICE object (InternetGatewayDevice.Services.VoiceService.{i}).
      if (cmsObj_getNextInSubTree(MDMOID_VOICE, &iidService, &iidVoice,
        (void**)&objVoice) == CMSRET_SUCCESS)
      {
        // Get MDMOID_VOICE_PROF object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}).
        if (cmsObj_getNextInSubTree(MDMOID_VOICE_PROF, &iidVoice, &iidProfile,
          (void**)&objVoiceProfile) == CMSRET_SUCCESS)
        {
          // Test for valid voice line (valid if known, invalid if not).
          if (voiceLineIndex < MAX_VOICE_LINES)
          {
            // Init flag to stop search loop.
            objFound = FALSE;

            // Init loop iteration line index.
            tempLineIndex = 0;

            // Get MDMOID_VOICE_LINE object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}).
            while ((objFound == FALSE) && (cmsObj_getNextInSubTree(
              MDMOID_VOICE_LINE, &iidProfile, &iidVoiceLine, (void**)&objLine)
              == CMSRET_SUCCESS))
            {
              // Test for correct line.
              if (tempLineIndex++ == voiceLineIndex)
              {
                // Set flag to stop search loop.
                objFound = TRUE;

                // Get MDMOID_VOICE_LINE_PROCESSING object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}.VoiceProcessing).
                if (cmsObj_getNextInSubTree(MDMOID_VOICE_LINE_PROCESSING,
                  &iidVoiceLine, &iidVoiceProcessing,
                  (void**)&objVoiceProcessingPtr) == CMSRET_SUCCESS)
                {
                  // Setup TX gain.
                  if ((omciPptpPotsUniObjectPtr->txGain & 0xff) & 0x80)
                  {
                    objVoiceProcessingPtr->transmitGain =
                      ~(omciPptpPotsUniObjectPtr->txGain) + 1;
                    objVoiceProcessingPtr->transmitGain &= 0xff;
                    objVoiceProcessingPtr->transmitGain *= -1;
                  }
                  else
                  {
                    objVoiceProcessingPtr->transmitGain =
                      omciPptpPotsUniObjectPtr->txGain;
                  }

                  // Setup RX gain.
                  if ((omciPptpPotsUniObjectPtr->rxGain & 0xff) & 0x80)
                  {
                    objVoiceProcessingPtr->receiveGain =
                      ~(omciPptpPotsUniObjectPtr->rxGain) + 1;
                    objVoiceProcessingPtr->receiveGain &= 0xff;
                    objVoiceProcessingPtr->receiveGain *= -1;
                  }
                  else
                  {
                    objVoiceProcessingPtr->receiveGain =
                      omciPptpPotsUniObjectPtr->rxGain;
                  }

                  // Attempt to write ME back to MDM & test for success.
                  cmsResult = cmsObj_set(objVoiceProcessingPtr,
                   &iidVoiceProcessing);
                  if (cmsResult != CMSRET_SUCCESS)
                  {
                    // Log error.
                    cmsLog_error("cmsObj_set error, cmsResult: %d", cmsResult);
                  }

                  // Release MDMOID_VOICE_LINE_PROCESSING object.
                  cmsObj_free((void**)&objVoiceProcessingPtr);
                }
              }

              // Release MDMOID_VOICE_LINE object.
              cmsObj_free((void**)&objLine);
            }
          }

          // Release MDMOID_VOICE_PROF object.
          cmsObj_free((void**)&objVoiceProfile);
        }

        // Release MDMOID_VOICE object.
        cmsObj_free((void**)&objVoice);
      }

      // Release MDMOID_SERVICES object.
      cmsObj_free((void**)&objService);
    }

    cmsLck_releaseLock();

    cmsLog_debug("===> %s, objFound=%d, txGain=%d, rxGain=%d\n",
      __FUNCTION__, objFound,
      omciPptpPotsUniObjectPtr->txGain, omciPptpPotsUniObjectPtr->rxGain);
#endif /* DMP_VOICE_SERVICE_1 */
}

#ifdef DMP_X_ITU_ORG_VOICE_SIP_1
/*****************************************************************************
*  FUNCTION:  owapi_rut_MapSipAgentConfigData
*  PURPOSE:   Map OMCI SIP agent config data ME attributes into TR-104 object.
*  PARAMETERS:
*      voiceLineIndex - voice line id.
*      omciSipAgentConfigDataPtr - pointer to SipAgentConfigDataObject.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_MapSipAgentConfigData(int voiceLineIndex __attribute__((unused)),
   SipAgentConfigDataObject* omciSipAgentConfigDataPtr __attribute__((unused)))
{
#ifdef DMP_VOICE_SERVICE_1
    void* objService = NULL;
    void* objVoice = NULL;
    void* objLine = NULL;
    void* objVoiceProfile = NULL;
    VoiceLineSipObject* voiceLineSipObjectPtr;
    InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidProfile = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoiceLine = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidSip = EMPTY_INSTANCE_ID_STACK;
    char* tempCharStr;
    CmsRet cmsResult;
    int tempLineIndex;
    UBOOL8 objFound = FALSE;

    if ((cmsResult = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", cmsResult);
        return;
    }

    // Get MDMOID_SERVICES object (InternetGatewayDevice.Services).
    if (cmsObj_get(MDMOID_SERVICES, &iidService, 0, (void**)&objService)
      == CMSRET_SUCCESS)
    {
      // Get MDMOID_VOICE object (InternetGatewayDevice.Services.VoiceService.{i}).
      if (cmsObj_getNextInSubTree(MDMOID_VOICE, &iidService, &iidVoice,
        (void**)&objVoice) == CMSRET_SUCCESS)
      {
        // Get MDMOID_VOICE_PROF object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}).
        if (cmsObj_getNextInSubTree(MDMOID_VOICE_PROF, &iidVoice,
          &iidProfile, (void**)&objVoiceProfile) == CMSRET_SUCCESS)
        {
          // Test for valid voice line (valid if known, invalid if not).
          if (voiceLineIndex < MAX_VOICE_LINES)
          {
            // Init flag to stop search loop.
            objFound = FALSE;

            // Init loop iteration line index.
            tempLineIndex = 0;

            // Get MDMOID_VOICE_LINE object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line).
            while ((objFound == FALSE) && (cmsObj_getNextInSubTree(
              MDMOID_VOICE_LINE, &iidProfile, &iidVoiceLine,
              (void**)&objLine) == CMSRET_SUCCESS))
            {
              // Test for correct line.
              if (tempLineIndex++ == voiceLineIndex)
              {
                // Set flag to stop search loop.
                objFound = TRUE;

                // Get MDMOID_VOICE_LINE_SIP object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}.SIP).
                if (cmsObj_getNextInSubTree(MDMOID_VOICE_LINE_SIP,
                  &iidVoiceLine, &iidSip, (void**)&voiceLineSipObjectPtr)
                  == CMSRET_SUCCESS)
                {
                  // Attempt to get SIP Proxy Server Address string.
                  tempCharStr = rutGpon_getLargeString(omciSipAgentConfigDataPtr->hostPartUri);

                  // Test for valid string.
                  if (tempCharStr != NULL)
                  {
                    // Setup host part URI.
                    CMSMEM_REPLACE_STRING_FLAGS(voiceLineSipObjectPtr->URI, tempCharStr, 0);

                    // Release temp string.
                    cmsMem_free(tempCharStr);
                  }

                  // Attempt to write ME back to MDM & test for success.
                  cmsResult = cmsObj_set(voiceLineSipObjectPtr, &iidSip);
                  if (cmsResult != CMSRET_SUCCESS)
                  {
                    // Log error.
                    cmsLog_error("cmsObj_set error, cmsResult: %d", cmsResult);
                  }

                  // Release MDMOID_VOICE_LINE_SIP object.
                  cmsObj_free((void**)&voiceLineSipObjectPtr);
                }
              }

              // Release MDMOID_VOICE_LINE object.
              cmsObj_free((void**)&objLine);
            }
          }

          // Release MDMOID_VOICE_PROF object.
          cmsObj_free((void**)&objVoiceProfile);
        }

        // Release MDMOID_VOICE object.
        cmsObj_free((void**)&objVoice);
      }

      // Release MDMOID_SERVICES object.
      cmsObj_free((void**)&objService);
    }
    cmsLck_releaseLock();

    cmsLog_debug("===> %s, objFound=%d, voiceLineIndex=%d\n",
      __FUNCTION__, objFound, voiceLineIndex);
  #endif /* DMP_VOICE_SERVICE_1 */
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_MapVoIpApplServiceProfile
*  PURPOSE:   Map OMCI VoIP application service profile ME attributes into
*             TR-104 object.
*  PARAMETERS:
*      voiceLineIndex - voice line id.
*      omciVoIpAppServiceProfilePtr - pointer to VoIpAppServiceProfileObject.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_MapVoIpApplServiceProfile(int voiceLineIndex
  __attribute__((unused)),
  VoIpAppServiceProfileObject* omciVoIpAppServiceProfilePtr
  __attribute__((unused)))
{
  #ifdef DMP_VOICE_SERVICE_1
    void* objService = NULL;
    void* objVoice = NULL;
    void* objLine = NULL;
    void* objVoiceProfile = NULL;
    VoiceLineCallingFeaturesObject* callingFeaturesObjectPtr;
    InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidProfile = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoiceLine = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidCallingFeatures = EMPTY_INSTANCE_ID_STACK;
    CmsRet cmsResult;
    int tempLineIndex;
    UBOOL8 objFound = FALSE;

    if ((cmsResult = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", cmsResult);
        return;
    }

    // Get MDMOID_SERVICES object (InternetGatewayDevice.Services).
    if (cmsObj_get(MDMOID_SERVICES, &iidService, 0, (void**)&objService)
      == CMSRET_SUCCESS)
    {
      // Get MDMOID_VOICE object (InternetGatewayDevice.Services.VoiceService.{i}).
      if (cmsObj_getNextInSubTree(MDMOID_VOICE, &iidService, &iidVoice,
        (void**)&objVoice) == CMSRET_SUCCESS)
      {
        // Get MDMOID_VOICE_PROF object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}).
        if (cmsObj_getNextInSubTree(MDMOID_VOICE_PROF, &iidVoice, &iidProfile,
          (void**)&objVoiceProfile) == CMSRET_SUCCESS)
        {
          // Test for valid voice line (valid if known, invalid if not).
          if (voiceLineIndex < MAX_VOICE_LINES)
          {
            // Init flag to stop search loop.
            objFound = FALSE;

            // Init loop iteration line index.
            tempLineIndex = 0;

            // Get MDMOID_VOICE_LINE object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line).
            while ((objFound == FALSE) && (cmsObj_getNextInSubTree(
              MDMOID_VOICE_LINE, &iidProfile, &iidVoiceLine, (void**)&objLine)
              == CMSRET_SUCCESS))
            {
              // Test for correct line.
              if (tempLineIndex++ == voiceLineIndex)
              {
                // Set flag to stop search loop.
                objFound = TRUE;

                // Get MDMOID_VOICE_LINE_CALLING_FEATURES object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}.CallingFeatures).
                if (cmsObj_getNextInSubTree(MDMOID_VOICE_LINE_CALLING_FEATURES,
                  &iidVoiceLine, &iidCallingFeatures,
                  (void**)&callingFeaturesObjectPtr) == CMSRET_SUCCESS)
                {
                  //// Test enable Caller_ID (CID).
                  //if ((omciVoIpAppServiceProfilePtr->cidFeatures & CALLER_ID_MASK) != FALSE)
                  //{
                  //  // Enable Caller_ID (CID).
                  //  callingFeaturesObjectPtr->callerIDEnable = TRUE;
                  //}
                  //else
                  //{
                  //  // Disable Caller_ID (CID).
                  //  callingFeaturesObjectPtr->callerIDEnable = FALSE;
                  //}

                  //// Test enable Block Anonymous calls.
                  //if ((omciVoIpAppServiceProfilePtr->cidFeatures & BLOCK_ANON_CALLER_ID) != FALSE)
                  //{
                  //  // Enable Block Anonymous calls.
                  //  callingFeaturesObjectPtr->anonymousCallBlockEnable = TRUE;
                  //}
                  //else
                  //{
                  //  // Disable Block Anonymous calls.
                  //  callingFeaturesObjectPtr->anonymousCallBlockEnable = FALSE;
                  //}

                  // Test enable call waiting.
                  if ((omciVoIpAppServiceProfilePtr->callWaitingFeatures &
                    CALL_WAITING_MASK) != FALSE)
                  {
                    // Enable call waiting.
                    callingFeaturesObjectPtr->callWaitingEnable = TRUE;
                  }
                  else
                  {
                    // Disable call waiting.
                    callingFeaturesObjectPtr->callWaitingEnable = FALSE;
                  }

                  // Test enable Call Transfer.
                  if ((omciVoIpAppServiceProfilePtr->callProgressFeatures &
                    CALL_XFER_MASK) != FALSE)
                  {
                    // Enable Call Transfer.
                    callingFeaturesObjectPtr->callTransferEnable = TRUE;
                  }
                  else
                  {
                    // Disable Call Transfer.
                    callingFeaturesObjectPtr->callTransferEnable = FALSE;
                  }

                  // Test enable DO_NOT_DISTURB.
                  if ((omciVoIpAppServiceProfilePtr->callProgressFeatures &
                    CALL_DO_NOT_DISTURB_MASK) != FALSE)
                  {
                    // Enable DO_NOT_DISTURB.
                    callingFeaturesObjectPtr->doNotDisturbEnable = TRUE;
                  }
                  else
                  {
                    // Disable DO_NOT_DISTURB.
                    callingFeaturesObjectPtr->doNotDisturbEnable = FALSE;
                  }

                  // Test enable Call Presentation.
                  if ((omciVoIpAppServiceProfilePtr->callPresentationFeatures &
                    CALL_MWI_MASK) != FALSE)
                  {
                    // Enable Message Waiting Indication.
                    callingFeaturesObjectPtr->MWIEnable = TRUE;
                  }
                  else
                  {
                    // Disable Message Waiting Indication.
                    callingFeaturesObjectPtr->MWIEnable = FALSE;
                  }

                  // Attempt to write ME back to MDM & test for success.
                  cmsResult = cmsObj_set(callingFeaturesObjectPtr,
                    &iidCallingFeatures);
                  if (cmsResult != CMSRET_SUCCESS)
                  {
                    // Log error.
                    cmsLog_error("cmsObj_set error, cmsResult: %d", cmsResult);
                  }

                  // Release MDMOID_VOICE_LINE_CALLING_FEATURES object.
                  cmsObj_free((void**)&callingFeaturesObjectPtr);
                }
              }

              // Release MDMOID_VOICE_LINE object.
              cmsObj_free((void**)&objLine);
            }
          }

          // Release MDMOID_VOICE_PROF object.
          cmsObj_free((void**)&objVoiceProfile);
        }

        // Release MDMOID_VOICE object.
        cmsObj_free((void**)&objVoice);
      }

      // Release MDMOID_SERVICES object.
      cmsObj_free((void**)&objService);
    }

    cmsLck_releaseLock();

    cmsLog_debug("===> %s, objFound=%d, voiceLineIndex=%d\n",
      __FUNCTION__, objFound, voiceLineIndex);
#endif /* DMP_VOICE_SERVICE_1 */
}
#endif /* DMP_X_ITU_ORG_VOICE_SIP_1 */

/*****************************************************************************
*  FUNCTION:  owapi_rut_MapVoiceServiceProfile
*  PURPOSE:   Map OMCI Voice service profile ME attributes into TR-104 object.
*  PARAMETERS:
*      voiceLineIndex - voice line id.
*      omciVoiceServiceObjectPtr - pointer to VoiceServiceObject.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_MapVoiceServiceProfile(int voiceLineIndex __attribute__((unused)),
  VoiceServiceObject* omciVoiceServiceObjectPtr __attribute__((unused)))
{
#ifdef DMP_VOICE_SERVICE_1
    void* objService = NULL;
    void* objVoice = NULL;
    void* objLine = NULL;
    VoiceProfObject* objVoiceProfile;
    VoiceLineProcessingObject* objVoiceProcessingPtr;
    InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidProfile = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoiceLine = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoiceProcessing = EMPTY_INSTANCE_ID_STACK;
    CmsRet cmsResult;
    int tempLineIndex;
    UBOOL8 objFound = FALSE;
    int idxCountryCode = 0;

    if ((cmsResult = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", cmsResult);
        return;
    }

    // Get MDMOID_SERVICES object (InternetGatewayDevice.Services).
    if (cmsObj_get(MDMOID_SERVICES, &iidService, 0, (void**)&objService)
      == CMSRET_SUCCESS)
    {
      // Get MDMOID_VOICE object (InternetGatewayDevice.Services.VoiceService.{i}).
      if (cmsObj_getNextInSubTree(MDMOID_VOICE, &iidService, &iidVoice,
        (void**)&objVoice) == CMSRET_SUCCESS)
      {
        // Get MDMOID_VOICE_PROF object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}).
        if (cmsObj_getNextInSubTree(MDMOID_VOICE_PROF, &iidVoice, &iidProfile,
          (void**)&objVoiceProfile) == CMSRET_SUCCESS)
        {
          while (countryCodeMap[idxCountryCode].countryCode !=
            ITU_RESERVED_COUNTRY_CODE)
          {
            if (countryCodeMap[idxCountryCode].countryCode ==
              (int)omciVoiceServiceObjectPtr->pstnProtocolVariant)
            {
              CMSMEM_REPLACE_STRING_FLAGS(objVoiceProfile->region,
                countryCodeMap[idxCountryCode].tr104Txt, 0);

              cmsResult = cmsObj_set(objVoiceProfile, &iidProfile);
              if (cmsResult != CMSRET_SUCCESS)
              {
                 // Log error.
                 cmsLog_error("Failed to set Voice Profile (%d)", cmsResult);
              }
              break;
            }
            idxCountryCode++;
          }

          // Test for valid voice line (valid if known, invalid if not).
          if (voiceLineIndex < MAX_VOICE_LINES)
          {
            // Init flag to stop search loop.
            objFound = FALSE;

            // Init loop iteration line index.
            tempLineIndex = 0;

            // Get MDMOID_VOICE_LINE object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}).
            while ((objFound == FALSE) && (cmsObj_getNextInSubTree(
              MDMOID_VOICE_LINE, &iidProfile, &iidVoiceLine, (void**)&objLine)
              == CMSRET_SUCCESS))
            {
              // Test for correct line.
              if (tempLineIndex++ == voiceLineIndex)
              {
                // Set flag to stop search loop.
                objFound = TRUE;

                // Get MDMOID_VOICE_LINE_PROCESSING object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}.VoiceProcessing).
                if (cmsObj_getNextInSubTree(MDMOID_VOICE_LINE_PROCESSING,
                  &iidVoiceLine, &iidVoiceProcessing,
                  (void**)&objVoiceProcessingPtr) == CMSRET_SUCCESS)
                {
                  // Test for disable echo cancellation.
                  if (omciVoiceServiceObjectPtr->echoCancel == FALSE)
                  {
                    // Disable echo cancellation.
                    objVoiceProcessingPtr->echoCancellationEnable = FALSE;
                  }
                  else
                  {
                    // Enableecho cancellation.
                    objVoiceProcessingPtr->echoCancellationEnable = TRUE;
                  }

                  // CMS message for internal parameters
                  //omciVoiceServiceObjectPtr->hookFlashMinTime
                  //omciVoiceServiceObjectPtr->hookFlashMaxTime

                  // Attempt to write ME back to MDM & test for success.
                  cmsResult = cmsObj_set(objVoiceProcessingPtr,
                    &iidVoiceProcessing);
                  if (cmsResult != CMSRET_SUCCESS)
                  {
                    // Log error.
                    cmsLog_error("cmsObj_set error, cmsResult: %d", cmsResult);
                  }

                  // Release MDMOID_VOICE_LINE_PROCESSING object.
                  cmsObj_free((void**)&objVoiceProcessingPtr);
                }
              }

              // Release MDMOID_VOICE_LINE object.
              cmsObj_free((void**)&objLine);
            }
          }

          // Release MDMOID_VOICE_PROF object.
          cmsObj_free((void**)&objVoiceProfile);
        }

        // Release MDMOID_VOICE object.
        cmsObj_free((void**)&objVoice);
      }

      // Release MDMOID_SERVICES object.
      cmsObj_free((void**)&objService);
    }
    cmsLck_releaseLock();

    cmsLog_debug("===> %s, objFound=%d, voiceLineIndex=%d\n",
      __FUNCTION__, objFound, voiceLineIndex);
#endif /* DMP_VOICE_SERVICE_1 */
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_MapVoIpMediaProfile
*  PURPOSE:   Map OMCI VoIP media profile ME attributes into TR-104 object.
*  PARAMETERS:
*      voiceLineIndex - voice line id.
*      omciVoIpMediaProfileObjectPtr - pointer to VoIpMediaProfileObject.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_MapVoIpMediaProfile(int voiceLineIndex __attribute__((unused)),
  VoIpMediaProfileObject* omciVoIpMediaProfileObjectPtr __attribute__((unused)))
{
#ifdef DMP_VOICE_SERVICE_1
    void* objService = NULL;
    void* objVoice = NULL;
    void* objLine = NULL;
    void* objCodec = NULL;
    VoiceProfObject* objVoiceProfPtr;
    VoiceProfFaxT38Object* objVoiceProfFaxT38Ptr;
    VoiceLineCodecListObject* objCodecListItemPtr;
    InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidProfile = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidFaxT38 = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoiceLine = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidCodec = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidCodecList = EMPTY_INSTANCE_ID_STACK;
    CmsRet cmsResult;
    int tempLineIndex;
    UBOOL8 objFound = FALSE;

    if ((cmsResult = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", cmsResult);
        return;
    }

    // Get MDMOID_SERVICES object (InternetGatewayDevice.Services).
    if (cmsObj_get(MDMOID_SERVICES, &iidService, 0, (void**)&objService)
      == CMSRET_SUCCESS)
    {
      // Get MDMOID_VOICE object (InternetGatewayDevice.Services.VoiceService.{i}).
      if (cmsObj_getNextInSubTree(MDMOID_VOICE, &iidService, &iidVoice,
        (void**)&objVoice) == CMSRET_SUCCESS)
      {
        // Get MDMOID_VOICE_PROF object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}).
        if (cmsObj_getNextInSubTree(MDMOID_VOICE_PROF, &iidVoice, &iidProfile,
          (void**)&objVoiceProfPtr) == CMSRET_SUCCESS)
        {
          // Test for OOB DTMF disabled.
          if (omciVoIpMediaProfileObjectPtr->oobDtmf == FALSE)
          {
            // Setup bound interface name pointer.
            CMSMEM_REPLACE_STRING_FLAGS(objVoiceProfPtr->DTMFMethod,
              MDMVS_INBAND, 0);
          }
          else
          {
            // Setup bound interface name pointer.
            CMSMEM_REPLACE_STRING_FLAGS(objVoiceProfPtr->DTMFMethod,
              MDMVS_RFC2833, 0);
          }

          // Attempt to write ME back to MDM & test for success.
          cmsResult = cmsObj_set(objVoiceProfPtr, &iidProfile);
          if (cmsResult != CMSRET_SUCCESS)
          {
            // Log error.
            cmsLog_error("cmsObj_set error, cmsResult: %d", cmsResult);
          }

          // Get MDMOID_VOICE_PROF_FAX_T38 object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.FaxT38).
          if (cmsObj_getNextInSubTree(MDMOID_VOICE_PROF_FAX_T38, &iidProfile,
            &iidFaxT38, (void**)&objVoiceProfFaxT38Ptr) == CMSRET_SUCCESS)
          {
            // Test for disable FAX T38.
            if (omciVoIpMediaProfileObjectPtr->faxMode == OMCI_FAX_PASS_THRU)
            {
              // Disable T38 FAX mode.
              objVoiceProfFaxT38Ptr->enable = FALSE;
            }
            else
            {
              // Enable T38 FAX mode.
              objVoiceProfFaxT38Ptr->enable = TRUE;
            }

            // Attempt to write ME back to MDM & test for success.
            cmsResult = cmsObj_set(objVoiceProfFaxT38Ptr, &iidFaxT38);
            if (cmsResult != CMSRET_SUCCESS)
            {
              // Log error.
              cmsLog_error("cmsObj_set error, cmsResult: %d", cmsResult);
            }

            // Release MDMOID_VOICE_PROF_FAX_T38 object.
            cmsObj_free((void**)&objVoiceProfFaxT38Ptr);
          }

          // Test for valid voice line (valid if known, invalid if not).
          if (voiceLineIndex < MAX_VOICE_LINES)
          {
            // Init flag to stop search loop.
            objFound = FALSE;

            // Init loop iteration line index.
            tempLineIndex = 0;

            // Get MDMOID_VOICE_LINE object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line).
            while ((objFound == FALSE) && (cmsObj_getNextInSubTree(
              MDMOID_VOICE_LINE, &iidProfile, &iidVoiceLine, (void**)&objLine)
              == CMSRET_SUCCESS))
            {
              // Test for correct line.
              if (tempLineIndex++ == voiceLineIndex)
              {
                // Set flag to stop search loop.
                objFound = TRUE;

                // Get MDMOID_VOICE_LINE_CODEC object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}.Codec).
                if (cmsObj_getNextInSubTree(MDMOID_VOICE_LINE_CODEC,
                  &iidVoiceLine, &iidCodec, (void**)&objCodec)
                  == CMSRET_SUCCESS)
                {
                  // Get MDMOID_VOICE_LINE_CODEC_LIST object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}.Codec.List.{i}).
                  while (cmsObj_getNextInSubTree(MDMOID_VOICE_LINE_CODEC_LIST,
                    &iidCodec, &iidCodecList, (void**)&objCodecListItemPtr)
                    == CMSRET_SUCCESS)
                  {
                    int priority;
                    for (priority = 1; priority <= LOWEST_PRIORITY_CODEC;
                      priority++)
                    {
                       if (rutGpon_MapCodecSelection(priority,
                         omciVoIpMediaProfileObjectPtr, objCodecListItemPtr))
                       {
                          break;
                       }
                    }

                    // Write MDMOID_VOICE_LINE_CODEC_LIST object back to MDM & test for success.
                    cmsResult = cmsObj_set((void**)objCodecListItemPtr,
                      &iidCodecList);
                    if (cmsResult != CMSRET_SUCCESS)
                    {
                        // Log error.
                        cmsLog_error("cmsObj_set error, cmsResult: %d",
                          cmsResult);
                    }

                    // Release MDMOID_VOICE_LINE_CODEC_LIST object.
                    cmsObj_free((void**)&objCodecListItemPtr);
                  }

                  // Release MDMOID_VOICE_LINE_CODEC object.
                  cmsObj_free((void**)&objCodec);
                }
              }

              // Release MDMOID_VOICE_LINE object.
              cmsObj_free((void**)&objLine);
            }
          }

          // Release MDMOID_VOICE_PROF object.
          cmsObj_free((void**)&objVoiceProfPtr);
        }

        // Release MDMOID_VOICE object.
        cmsObj_free((void**)&objVoice);
      }

      // Release MDMOID_SERVICES object.
      cmsObj_free((void**)&objService);
    }
    cmsLck_releaseLock();
    cmsLog_debug("===> %s, objFound=%d, voiceLineIndex=%d\n",
      __FUNCTION__, objFound, voiceLineIndex);
#endif /* DMP_VOICE_SERVICE_1 */
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_MapRtpProfileData
*  PURPOSE:   Map OMCI RTP profile data ME attributes into TR-104 object.
*  PARAMETERS:
*      omciRtpProfilePtr - pointer to RtpProfileDataObject.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_MapRtpProfileData(RtpProfileDataObject* omciRtpProfilePtr
  __attribute__((unused)))
{
#ifdef DMP_VOICE_SERVICE_1
    void *objService = NULL, *objVoice = NULL, *objProfile = NULL;
    VoiceProfRtpObject* rtpPtr;
    CmsRet cmsResult;
    InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidProfile = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidRtp = EMPTY_INSTANCE_ID_STACK;
    UBOOL8 objFound = FALSE;

    if ((cmsResult = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", cmsResult);
        return;
    }

    // Get MDMOID_SERVICES object (InternetGatewayDevice.Services).
    if (cmsObj_get(MDMOID_SERVICES, &iidService, 0, (void**)&objService)
      == CMSRET_SUCCESS)
    {
      // Get MDMOID_VOICE object (InternetGatewayDevice.Services.VoiceService.{i}).
      if (cmsObj_getNextInSubTree(MDMOID_VOICE, &iidService, &iidVoice,
        (void**)&objVoice) == CMSRET_SUCCESS)
      {
        // Get MDMOID_VOICE_PROF object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}).
        if (cmsObj_getNextInSubTree(MDMOID_VOICE_PROF, &iidVoice, &iidProfile,
          (void**)&objProfile) == CMSRET_SUCCESS)
        {
            // Get MDMOID_VOICE_PROF_RTP object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.RTP).
            if (cmsObj_getNextInSubTree(MDMOID_VOICE_PROF_RTP, &iidProfile,
              &iidRtp, (void**)&rtpPtr) == CMSRET_SUCCESS)
            {
                objFound = TRUE;
                // Transfer OMCI fields to TR-104.
                rtpPtr->localPortMin = omciRtpProfilePtr->localPortMin;
                rtpPtr->localPortMax = omciRtpProfilePtr->localPortMax;
                rtpPtr->DSCPMark = omciRtpProfilePtr->dscpMark;

                // Attempt to write changes to MDM & test for success.
                cmsResult = cmsObj_set(rtpPtr, &iidRtp);
                if (cmsResult != CMSRET_SUCCESS)
                {
                    // Log error.
                    cmsLog_error("cmsObj_set error, cmsResult: %d", cmsResult);
                }

                // Release MDMOID_VOICE_PROF_RTP object.
                cmsObj_free((void**)&rtpPtr);
            }

            // Release MDMOID_VOICE_PROF object.
            cmsObj_free((void**)&objProfile);
        }

        // Release MDMOID_VOICE object.
        cmsObj_free((void**)&objVoice);
      }

      // Release MDMOID_SERVICES object.
      cmsObj_free((void**)&objService);
    }
    cmsLck_releaseLock();

    cmsLog_debug("===> %s, objFound=%d, portMin=%d, portMax=%d\n",
      __FUNCTION__, objFound, omciRtpProfilePtr->localPortMin,
      omciRtpProfilePtr->localPortMax);
#endif /* DMP_VOICE_SERVICE_1 */
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_MapVoIpConfigData
*  PURPOSE:   Map OMCI VoIP config data ME attributes into TR-104 object.
*  PARAMETERS:
*      voipConfigDataPtr - pointer to VoIpConfigDataObject.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_MapVoIpConfigData(VoIpConfigDataObject* voipConfigDataPtr
  __attribute__((unused)))
{
#ifdef DMP_VOICE_SERVICE_1
    CmsRet cmsResult = CMSRET_SUCCESS;
    void* objService = NULL;
    VoiceObject* objVoicePtr = NULL;
    InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
    UBOOL8 objFound = FALSE;

    if ((cmsResult = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", cmsResult);
        return;
    }

    // Get MDMOID_SERVICES object (InternetGatewayDevice.Services).
    if (cmsObj_get(MDMOID_SERVICES, &iidService, 0, (void**)&objService)
      == CMSRET_SUCCESS)
    {
        // Get MDMOID_VOICE object (InternetGatewayDevice.Services.VoiceService.{i}).
        if (cmsObj_getNextInSubTree(MDMOID_VOICE, &iidService, &iidVoice,
          (void**)&objVoicePtr) == CMSRET_SUCCESS)
        {
            objFound = TRUE;
            switch (voipConfigDataPtr->usedVoIpConfigMethods)
            {
            case VOIP_CONFIG_METHOD_OMCI:
              CMSMEM_REPLACE_STRING_FLAGS(
                objVoicePtr->X_BROADCOM_COM_ManagementProtocol,
                MDMVS_OMCI, 0);
              break;
            case VOIP_CONFIG_METHOD_TR69:
              CMSMEM_REPLACE_STRING_FLAGS(
                objVoicePtr->X_BROADCOM_COM_ManagementProtocol,
                MDMVS_TR69, 0);
              break;
            default:
              break;
            }

            // Attempt to write ME back to MDM & test for success.
            cmsResult = cmsObj_set(objVoicePtr, &iidVoice);
            if (cmsResult != CMSRET_SUCCESS)
            {
                // Log error.
                cmsLog_error("cmsObj_set error, cmsResult: %d", cmsResult);
            }

            // Release MDMOID_VOICE object.
            cmsObj_free((void**)&objVoicePtr);
        }
        // Release MDMOID_SERVICES object.
        cmsObj_free((void**)&objService);
    }

    cmsLck_releaseLock();

    cmsLog_debug("===> %s, objFound=%d, usedVoIpConfigMethods=%d\n",
      __FUNCTION__, objFound, voipConfigDataPtr->usedVoIpConfigMethods);
#endif /* DMP_VOICE_SERVICE_1 */
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_setVoipBoundIfNameAddress
*  PURPOSE:   Set up the IP address of the voice-bound IP interface.
*  PARAMETERS:
*      ipHostMeId - IP host ME id.
*      ipAddr - pointer to IP address string.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_setVoipBoundIfNameAddress(UINT32 ipHostMeId, char *ipAddr)
{
    UBOOL8 found = FALSE;
    BCM_IpHostConfigDataObject *bcmIpHost = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        return ret;
    }

    while ((found == FALSE) &&
      (cmsObj_getNext(MDMOID_BCM_OMCI_RTD_IP_HOST_CONFIG_DATA_EXT,
      &iidStack, (void**)&bcmIpHost) == CMSRET_SUCCESS))
    {
        if ((bcmIpHost->managedEntityId == ipHostMeId) &&
          (bcmIpHost->interfaceName != NULL))
        {
            // Process IpHostConfigDataObject configuration.
            setVoipBoundIfNameAddress(bcmIpHost->interfaceName, ipAddr);
            found = TRUE;
        }
        cmsObj_free((void**)&bcmIpHost);
    }

    cmsLck_releaseLock();

    cmsLog_debug("===> setVoipBoundIfNameAddress, iphost=%d, ipAddr=%s, found = %d\n",
      ipHostMeId, ipAddr, found);

    return CMSRET_SUCCESS;
}

#ifdef DMP_X_ITU_ORG_VOICE_SIP_1
/*****************************************************************************
*  FUNCTION:  owapi_rut_setSipAuthUsername
*  PURPOSE:   Map OMCI SIP authentication user name into TR-104 object.
*  PARAMETERS:
*      port - voice line id.
*      user - pointer to username in Authentication security method ME.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_setSipAuthUsername
    (const UINT32 port,
     const char* user)
{
    CmsRet ret = CMSRET_REQUEST_DENIED;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        return ret;
    }

    ret = setSipAuthUsername(port, user);

    cmsLck_releaseLock();

    cmsLog_debug("===> setSipAuthUsername, port=%d, user=%s, ret = %d\n",
      port, user, ret);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_setSipAuthPassword
*  PURPOSE:   Map OMCI SIP authentication password into TR-104 object.
*  PARAMETERS:
*      port - voice line id.
*      user - pointer to password in Authentication security method ME.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_setSipAuthPassword
    (const UINT32 port,
     const char* password)
{
    CmsRet ret = CMSRET_REQUEST_DENIED;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        return ret;
    }

    ret = setSipAuthPassword(port, password);

    cmsLck_releaseLock();

    cmsLog_debug("===> setSipAuthPassword, port=%d, password=%s, ret = %d\n",
      port, password, ret);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_setSipUserPartAor
*  PURPOSE:   Map OMCI SIP user part AOR into TR-104 object.
*  PARAMETERS:
*      port - voice line id.
*      userPartAor - pointer to User port AOR in SIP user data ME.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_setSipUserPartAor
    (const UINT32 port,
     const char* userPartAor)
{
    CmsRet ret = CMSRET_REQUEST_DENIED;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        return ret;
    }

    ret = setSipUserPartAor(port, userPartAor);

    cmsLck_releaseLock();

    cmsLog_debug("===> setSipUserPartAor, port=%d, userPartAor=%s, ret = %d\n",
      port, userPartAor, ret);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_setSipReregHeadStartTime
*  PURPOSE:   Map OMCI SIP rereg head start time into TR-104 object.
*  PARAMETERS:
*      port - voice line id.
*      reregVal - SIP rereg head start time as defined in G.988.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_setSipReregHeadStartTime
    (const UINT32 port,
     const UINT32 reregVal)
{
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        return ret;
    }

    ret = setSipReregHeadStartTime(port, reregVal);

    cmsLck_releaseLock();

    cmsLog_debug("===> setSipReregHeadStartTime, port=%d, reregVal=%d, ret = %d\n",
      port, reregVal, ret);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_setSipRegisterExpirationTime
*  PURPOSE:   Map OMCI SIP reg exp time into TR-104 object.
*  PARAMETERS:
*      port - voice line id.
*      expireVal - SIP registration expiration time as defined in G.988.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_setSipRegisterExpirationTime
    (const UINT32 port,
     const UINT32 expireVal)
{
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        return ret;
    }

    ret = setSipRegisterExpirationTime(port, expireVal);
    cmsLck_releaseLock();

    cmsLog_debug("===> setSipRegisterExpirationTime, port=%d, expireVal=%d, ret = %d\n",
      port, expireVal, ret);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_setSipOutboundProxyAddress
*  PURPOSE:   Map OMCI SIP outbound proxy address into TR-104 object.
*  PARAMETERS:
*      port - voice line id.
*      address - SIP outbound proxy address as defined in G.988.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_setSipOutboundProxyAddress
    (const UINT32 port,
     const char *address)
{
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        return ret;
    }

    ret = setSipOutboundProxyAddress(port, address);

    cmsLck_releaseLock();

    cmsLog_debug("===> setSipOutboundProxyAddress, port=%d, address=%s, ret = %d\n",
      port, address, ret);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_setSipProxyServerAddress
*  PURPOSE:   Map OMCI SIP proxy server address into TR-104 object.
*  PARAMETERS:
*      port - voice line id.
*      address - SIP proxy server address as defined in G.988.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_setSipProxyServerAddress
    (const UINT32 port,
     const char *address)
{
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        return ret;
    }

    ret = setSipProxyServerAddress(port, address);

    cmsLck_releaseLock();

    cmsLog_debug("===> setSipProxyServerAddress, port=%d, address=%s, ret = %d\n",
      port, address, ret);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_setSipRegistrarAddress
*  PURPOSE:   Map OMCI SIP registrar address into TR-104 object.
*  PARAMETERS:
*      port - voice line id.
*      address - SIP registrar address as defined in G.988.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_setSipRegistrarAddress
    (const UINT32 port,
     const char *address)
{
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        return ret;
    }

    ret = setSipRegistrarAddress(port, address);

    cmsLck_releaseLock();

    cmsLog_debug("===> setSipRegistrarAddress, port=%d, address=%s, ret = %d\n",
      port, address, ret);

    return ret;
}
#endif /* DMP_X_ITU_ORG_VOICE_SIP_1 */

/*****************************************************************************
*  FUNCTION:  owapi_rut_SendUploadComplete
*  PURPOSE:   Inform the voice application the OMCI configuration mapping is
*             complete. The voice application may choose to restart.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_SendUploadComplete(void)
{
    CmsRet cmsResult;

    cmsResult = SendUploadComplete();

    return cmsResult;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_IsVoiceOmciManaged
*  PURPOSE:   Check whether the voice service is OMCI managed.
*  PARAMETERS:
*      None.
*  RETURNS:
*      TRUE/FALSE.
*  NOTES:
*      None.
*****************************************************************************/
UBOOL8 owapi_rut_IsVoiceOmciManaged(void)
{
    UBOOL8 isManaged = TRUE;

#ifndef DESKTOP_LINUX
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
    CmsRet ret = CMSRET_SUCCESS;
    char mdmMgtProt[BUFLEN_24] = {0};

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        return ret;
    }

    ret = GetManagementProtocol(mdmMgtProt, sizeof(mdmMgtProt));
    if ( (ret != CMSRET_SUCCESS) || (cmsUtl_strcmp(mdmMgtProt, MDMVS_OMCI)) )
    {
        isManaged = FALSE;
    }

    cmsLck_releaseLock();
#endif /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */
#endif /* DESKTOP_LINUX */

    return isManaged;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_GetRtpStats
*  PURPOSE:   Get RDP statistics.
*  PARAMETERS:
*      physPortId - voice line id.
*      rtpStatsRec - pointer to RTP counters.
*  RETURNS:
*      BCM_OMCI_PM_STATUS.
*  NOTES:
*      None.
*****************************************************************************/
BCM_OMCI_PM_STATUS owapi_rut_GetRtpStats(int physPortId,
  BCM_OMCI_PM_RTP_COUNTER* rtpStatsRec)
{
    CmsRet cmsResult = CMSRET_SUCCESS;
    CmsMsgHeader msgHdr = EMPTY_MSG_HEADER;
    BCM_OMCI_PM_STATUS omciResult = OMCI_PM_STATUS_ERROR;
    char replyMsgBuffer[sizeof(CmsMsgHeader) +
      sizeof(BCM_OMCI_PM_RTP_COUNTER)] = {0};
    CmsMsgHeader* replyMsgPtr = (CmsMsgHeader*)replyMsgBuffer;
    BCM_OMCI_PM_RTP_COUNTER* inputStatsPtr;

    // Attempt to send CMS message, wait for response & test result.
    if (rutIsVoipRunning())
    {
        // Setup CMS message header.
        memset(&msgHdr, 0, sizeof(CmsMsgHeader));
        memset(&replyMsgBuffer, 0, sizeof(CmsMsgHeader) +
          sizeof(BCM_OMCI_PM_RTP_COUNTER));
        /* DESKTOP_LINUX: set to EID_SSK to debug without real VOICE stack. */
        msgHdr.dst = EID_VOICE;
        msgHdr.src = getCmsEid();
        msgHdr.type = CMS_MSG_VOICE_GET_RTP_STATS;
        msgHdr.wordData = physPortId;
        msgHdr.flags_request = 1;

        cmsResult = cmsMsg_sendAndGetReplyBufWithTimeout(getCmsMsgHandle(),
          (CmsMsgHeader*)&msgHdr, (CmsMsgHeader**)&replyMsgPtr,
          CMS_OMCID_REQUEST_TIMEOUT);
        if (cmsResult != CMSRET_SUCCESS)
        {
          // Log error.
          cmsLog_error("Send message failure, cmsResult: %d", cmsResult);
        }
        else
        {
          // Setup stats pointer.
          inputStatsPtr = (BCM_OMCI_PM_RTP_COUNTER*)&replyMsgBuffer[
            sizeof(CmsMsgHeader)];

          // Copy stats to user's rec.
          *rtpStatsRec = *inputStatsPtr;

          // Setup success result.
          omciResult = OMCI_PM_STATUS_SUCCESS;
        }
    }

    // Return operation result.
    return omciResult;
}

#endif // DMP_X_ITU_ORG_VOICE_1
