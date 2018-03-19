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
*      Functions in this file are POTS interface specific.
*
*****************************************************************************/


/* ---- Include Files ----------------------------------------------------- */

#include "mdm.h"
#include "cms_obj.h"
#include "cms_util.h"
#include "cms_msg.h"

#include "mdmlite_api.h"

#include "owsvc_api.h"
#include "ctlutils_api.h"
#include "owrut_api.h"
#include "owrut_cmsdefs.h"
#include "me_handlers.h"


/* ---- Private Constants and Types --------------------------------------- */


/* ---- Macros ------------------------------------------------------------ */


/* ---- Private Function Prototypes --------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */


/* ---- Functions --------------------------------------------------------- */

#ifdef DMP_X_ITU_ORG_VOICE_1
#ifdef DMP_VOICE_SERVICE_1

/*****************************************************************************
*  FUNCTION:  owapi_rut_disablePptpPotsUni
*  PURPOSE:   Disable a POTS UNI interface.
*  PARAMETERS:
*      port - POTS port id.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_disablePptpPotsUni(UINT32 port)
{
    void* objService = NULL;
    void* objVoice = NULL;
    void* objVoiceProfile = NULL;
    InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidProfile = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoiceLine = EMPTY_INSTANCE_ID_STACK;
    VoiceLineObject* voiceLineObjectPtr;
    CmsRet cmsResult = CMSRET_SUCCESS;

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
                // Get MDMOID_VOICE_LINE object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}).
                while (cmsObj_getNextInSubTree(MDMOID_VOICE_LINE, &iidProfile,
                  &iidVoiceLine, (void**)&voiceLineObjectPtr) == CMSRET_SUCCESS)
                {
                    if (port ==
                      (iidVoiceLine.instance[iidVoiceLine.currentDepth-1] -1))
                    {
                        //Disable the service of this line
                        /* In cms-data-model.xml
                         <validstringarray name="enable3qValues" />
                         <element>Disabled</element>
                         <element>Quiescent</element>
                         <element>Enabled</element>
                        */
                        CMSMEM_REPLACE_STRING_FLAGS(voiceLineObjectPtr->enable,
                          MDMVS_DISABLED, 0);
                        cmsObj_set(voiceLineObjectPtr, &iidVoiceLine);
                    }

                    cmsObj_free((void**)&voiceLineObjectPtr);
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
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_enablePptpPotsUni
*  PURPOSE:   Enable a POTS UNI interface.
*  PARAMETERS:
*      port - POTS port id.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_enablePptpPotsUni(UINT32 port)
{
    void* objService = NULL;
    void* objVoice = NULL;
    void* objVoiceProfile = NULL;
    InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidProfile = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoiceLine = EMPTY_INSTANCE_ID_STACK;
    VoiceLineObject* voiceLineObjectPtr;
    CmsRet cmsResult = CMSRET_SUCCESS;

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
                // Get MDMOID_VOICE_LINE object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}).
                while (cmsObj_getNextInSubTree(MDMOID_VOICE_LINE, &iidProfile,
                  &iidVoiceLine, (void**)&voiceLineObjectPtr) == CMSRET_SUCCESS)
                {
                    if (port ==
                      (iidVoiceLine.instance[iidVoiceLine.currentDepth-1] -1))
                    {
                        //Enable the service of this line
                        /* In cms-data-model.xml
                         <validstringarray name="enable3qValues" />
                         <element>Disabled</element>
                         <element>Quiescent</element>
                         <element>Enabled</element>
                        */
                        CMSMEM_REPLACE_STRING_FLAGS(voiceLineObjectPtr->enable,
                          MDMVS_ENABLED, 0);
                        cmsObj_set(voiceLineObjectPtr, &iidVoiceLine);
                    }

                    cmsObj_free((void**)&voiceLineObjectPtr);
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
}
#else
void owapi_rut_disablePptpPotsUni(UINT32 port __attribute__((unused)))
{
}
void owapi_rut_enablePptpPotsUni(UINT32 port __attribute__((unused)))
{
}
#endif /* DMP_VOICE_SERVICE_1 */

#ifdef DMP_VOICE_SERVICE_1
/*****************************************************************************
*  FUNCTION:  owapi_rut_disableAllPptpPotsUni
*  PURPOSE:   Disable all POTS UNI interfaces.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_disableAllPptpPotsUni(void)
{
    void* objService = NULL;
    void* objVoice = NULL;
    void* objVoiceProfile = NULL;
    InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidProfile = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoiceLine = EMPTY_INSTANCE_ID_STACK;
    VoiceLineObject* voiceLineObjectPtr;
    CmsRet cmsResult = CMSRET_SUCCESS;

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
                // Get MDMOID_VOICE_LINE object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}).
                while (cmsObj_getNextInSubTree(MDMOID_VOICE_LINE, &iidProfile,
                  &iidVoiceLine, (void**)&voiceLineObjectPtr) == CMSRET_SUCCESS)
                {
                    //Disable the service of this line
                    /* In cms-data-model.xml
                     <validstringarray name="enable3qValues" />
                     <element>Disabled</element>
                     <element>Quiescent</element>
                     <element>Enabled</element>
                     */
                    CMSMEM_REPLACE_STRING_FLAGS(voiceLineObjectPtr->enable,
                      MDMVS_DISABLED, 0);
                    cmsObj_set(voiceLineObjectPtr, &iidVoiceLine);

                    cmsObj_free((void**)&voiceLineObjectPtr);
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
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_enableAllPptpPotsUni
*  PURPOSE:   Enable all POTS UNI interfaces.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_enableAllPptpPotsUni(void)
{
    void* objService = NULL;
    void* objVoice = NULL;
    void* objVoiceProfile = NULL;
    InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidProfile = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoiceLine = EMPTY_INSTANCE_ID_STACK;
    VoiceLineObject* voiceLineObjectPtr;
    CmsRet cmsResult = CMSRET_SUCCESS;

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
                // Get MDMOID_VOICE_LINE object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}).
                while (cmsObj_getNextInSubTree(MDMOID_VOICE_LINE, &iidProfile,
                  &iidVoiceLine, (void**)&voiceLineObjectPtr) == CMSRET_SUCCESS)
                {
                    //Enable the service of this line
                    /* In cms-data-model.xml
                     <validstringarray name="enable3qValues" />
                     <element>Disabled</element>
                     <element>Quiescent</element>
                     <element>Enabled</element>
                     */
                    CMSMEM_REPLACE_STRING_FLAGS(voiceLineObjectPtr->enable,
                      MDMVS_ENABLED, 0);
                    cmsObj_set(voiceLineObjectPtr, &iidVoiceLine);

                    cmsObj_free((void**)&voiceLineObjectPtr);
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
}
#else
/* DMP_VOICE_SERVICE_2 */

/*****************************************************************************
*  FUNCTION:  owapi_rut_disableAllPptpPotsUni
*  PURPOSE:   Disable all POTS UNI interfaces.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_disableAllPptpPotsUni(void)
{
    void* objService = NULL;
    void* objVoice = NULL;
    InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidSipClient = EMPTY_INSTANCE_ID_STACK;
    SipClientObject* voiceSipClientObjectPtr;
    CmsRet cmsResult = CMSRET_SUCCESS;

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
            // Get MDMOID_VOICE_LINE object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}).
            while (cmsObj_getNextInSubTree(MDMOID_SIP_CLIENT, &iidVoice,
              &iidSipClient, (void**)&voiceSipClientObjectPtr) == CMSRET_SUCCESS)
            {
                    //Disable the service of this line
                    /* In cms-data-model.xml
                     <validstringarray name="enable3qValues" />
                     <element>Disabled</element>
                     <element>Enabled</element>
                     */
                    voiceSipClientObjectPtr->enable = FALSE;

                    cmsObj_set(voiceSipClientObjectPtr, &iidSipClient);

                    cmsObj_free((void**)&voiceSipClientObjectPtr);
            }
            // Release MDMOID_VOICE object.
            cmsObj_free((void**)&objVoice);
        }
        // Release MDMOID_SERVICES object.
        cmsObj_free((void**)&objService);
    }
    cmsLck_releaseLock();
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_enableAllPptpPotsUni
*  PURPOSE:   Enable all POTS UNI interfaces.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_enableAllPptpPotsUni(void)
{
    void* objService = NULL;
    void* objVoice = NULL;
    InstanceIdStack iidService = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVoice = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidSipClient = EMPTY_INSTANCE_ID_STACK;
    SipClientObject* voiceSipClientObjectPtr;
    CmsRet cmsResult = CMSRET_SUCCESS;

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
            // Get MDMOID_VOICE_LINE object (InternetGatewayDevice.Services.VoiceService.{i}.VoiceProfile.{i}.Line.{i}).
            while (cmsObj_getNextInSubTree(MDMOID_SIP_CLIENT, &iidVoice,
              &iidSipClient, (void**)&voiceSipClientObjectPtr) == CMSRET_SUCCESS)
            {
                //Enable the service of this line
                /* In cms-data-model.xml
                 <validstringarray name="enable3qValues" />
                 <element>Disabled</element>
                 <element>Quiescent</element>
                 <element>Enabled</element>
                 */
                voiceSipClientObjectPtr->enable = TRUE;

                cmsObj_set(voiceSipClientObjectPtr, &iidSipClient);

                cmsObj_free((void**)&voiceSipClientObjectPtr);
            }
            // Release MDMOID_VOICE object.
            cmsObj_free((void**)&objVoice);
        }
        // Release MDMOID_SERVICES object.
        cmsObj_free((void**)&objService);
    }

    cmsLck_releaseLock();
}
#endif /* DMP_VOICE_SERVICE_1 */

#endif /* DMP_X_ITU_ORG_VOICE_1 */
