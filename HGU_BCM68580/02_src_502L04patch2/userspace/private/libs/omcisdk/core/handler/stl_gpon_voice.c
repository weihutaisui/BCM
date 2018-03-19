/***********************************************************************
 *
 *  Copyright (c) 2010 Broadcom
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

#include "omci_stl.h"
#include "owsvc_api.h"
#include "rut_gpon.h"
#include "omci_pm.h"
#include "mdmlite_api.h"

CmsRet stl_voiceServicesObject(_VoiceServicesObject *obj __attribute__((unused)),
                               const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_pptpPotsUniObject(_PptpPotsUniObject *obj __attribute__((unused)),
                             const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

#ifdef DMP_X_ITU_ORG_VOICE_SIP_1

CmsRet stl_sipUserDataObject(_SipUserDataObject *obj __attribute__((unused)),
                             const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_sipAgentConfigDataObject(_SipAgentConfigDataObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_sipAgentPmHistoryDataObject(_SipAgentPmHistoryDataObject *obj __attribute__((unused)),
                                       const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_sipCallInitPmHistoryDataObject(_SipCallInitPmHistoryDataObject *obj __attribute__((unused)),
                                          const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

#endif /* DMP_X_ITU_ORG_VOICE_SIP_1 */

#ifdef DMP_X_ITU_ORG_VOICE_MGC_1

CmsRet stl_mgcConfigDataObject(_MgcConfigDataObject *obj __attribute__((unused)),
                               const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_mgcPmHistoryDataObject(_MgcPmHistoryDataObject *obj __attribute__((unused)),
                                  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

#endif /* DMP_X_ITU_ORG_VOICE_MGC_1 */

CmsRet stl_voIpVoiceCtpObject(_VoIpVoiceCtpObject *obj __attribute__((unused)),
                              const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_voIpMediaProfileObject(_VoIpMediaProfileObject *obj __attribute__((unused)),
                                  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_voiceServiceObject(_VoiceServiceObject *obj __attribute__((unused)),
                              const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_rtpProfileDataObject(_RtpProfileDataObject *obj __attribute__((unused)),
                                const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_voIpAppServiceProfileObject(_VoIpAppServiceProfileObject *obj __attribute__((unused)),
                                       const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_voiceFeatureAccessCodesObject(_VoiceFeatureAccessCodesObject *obj __attribute__((unused)),
                                         const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_networkDialPlanTableObject(_NetworkDialPlanTableObject *obj __attribute__((unused)),
                                      const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_dialPlanTableObject(_DialPlanTableObject *obj __attribute__((unused)),
                               const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_voIpLineStatusObject(_VoIpLineStatusObject *obj __attribute__((unused)),
                                const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_callControlPmHistoryDataObject(_CallControlPmHistoryDataObject *obj __attribute__((unused)),
                                          const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_rtpPmHistoryDataObject(_RtpPmHistoryDataObject *obj __attribute__((unused)),
                                  const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_RTP_COUNTER* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_RTP_PM_HISTORY_DATA,
                          obj->managedEntityId,
                          (void **)&pData,
                          &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_RTP_PM_HISTORY_DATA,
                                 obj->managedEntityId,
                                 (void **)&pData,
                                 &dataLen);
    }

    _cmsObj_free((void **) &cmsObj);

    if (dataLen != 0 && pData != NULL)
    {
        // Setup reply interval value.
        obj->intervalEndTime = omci_pm_getIntervalCounter();

        // Setup reply stats.
        obj->rtpErrors = pData->rtpErrors;
        obj->packetLoss = pData->packetLoss;
        obj->maxJitter = pData->maxJitter;
        obj->maxTimeBetweenRtcpPackets = pData->maxTimeBetweenRtcpPackets;
        obj->bufferUnderflows = pData->bufferUnderflows;
        obj->bufferOverflows = pData->bufferOverflows;

        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}

CmsRet stl_voIpConfigDataObject(_VoIpConfigDataObject *obj __attribute__((unused)),
                                const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_sipConfigPortalObject(
  _SipConfigPortalObject *obj __attribute__((unused)),
  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS;
}

CmsRet stl_mgcConfigPortalObject(
  _MgcConfigPortalObject *obj __attribute__((unused)),
  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS;
}

#endif /* DMP_X_ITU_ORG_VOICE_1 */
