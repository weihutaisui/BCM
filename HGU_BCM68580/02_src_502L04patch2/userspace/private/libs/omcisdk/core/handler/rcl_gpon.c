/***********************************************************************
 *
 *  Copyright (c) 2008 Broadcom
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

#ifdef DMP_X_ITU_ORG_GPON_1

#include "owsvc_api.h"
#include "owrut_api.h"
#include "ctlutils_api.h"
#include "me_handlers.h"
#include "omci_pm.h"
#include "omci_util.h"
#include "bcm_pkt_lengths.h" /* for ENET_MAX_MTU_PAYLOAD_SIZE */


#define getGponDevType(isDirDs) ((isDirDs) ? TMCTL_DEV_ETH : TMCTL_DEV_GPON)
#define MIN(A,B) ((A) < (B) ? (A) : (B))


typedef struct
{
    MdmObjectId _oid;
    UINT32 meId;
    UINT32 intervalEndTime;
    UINT32 thrDataId;
} PmHdObjCmnHdr_t;

typedef struct
{
    UINT16 pktDQThrGMin;
    UINT16 pktDQThrGMax;
    UINT16 pktDQThrYMin;
    UINT16 pktDQThrYMax;
} PktDropQThr_t;


/* Private Function Prototypes. */

static CmsRet pktDropQThrConvert(const char *thrHexStr, PktDropQThr_t *pktDropQThr);
static void omciTmctlPQParamConvert(UBOOL8 isDirDs, UINT16 slotPortId,
  tmctl_if_t *tmIf, char *ifName);

/* Legacy context for accessing the database. */
static _MdmLibraryContext _mdmLibCtx = {0, FALSE, NULL};


/* Initialize the _mdmLibCtx context. */
void rutcmn_cxtInit(void *msgHandle, const UINT32 eid)
{
    _mdmLibCtx.eid = eid;
    _mdmLibCtx.msgHandle = msgHandle;
}

/*
 * RCL functions
 */

CmsRet rcl_ituTOrgObject( _ItuTOrgObject *newObj __attribute__((unused)),
                const _ItuTOrgObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_g_988Object( _G_988Object *newObj __attribute__((unused)),
                const _G_988Object *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_equipmentManagementObject( _EquipmentManagementObject *newObj __attribute__((unused)),
                const _EquipmentManagementObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_attributeValueChangeOntG(const _OntGObject *newObj,
                                    const _OntGObject *currObj)
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 attrMask = 0;
    UINT32 binSize = 0, i = 0, j = 0;
    UINT8 *bin = NULL;
    // Max length of OMCI message is 32, and the first 2 bytes are attribute mask
    // so max length of attribute values is 30
    UINT16 msgSizeMax = OMCI_PACKET_A_MSG_SIZE - 2;
    UINT8 attrVal[msgSizeMax];

    memset(attrVal, 0, msgSizeMax);
    // if vendorId is changed then send attribute value change notification
    if (cmsUtl_strcmp(newObj->vendorId, currObj->vendorId) != 0)
    {
        attrMask |= 0x8000;
        cmsUtl_hexStringToBinaryBuf(newObj->vendorId, &bin, &binSize);
        for (i = 0; i < binSize && j < msgSizeMax; i++, j++)
        {
            attrVal[j] = bin[i];
        }
        cmsMem_free(bin);
    }
    // if version is changed then send attribute value change notification
    if (j < (UINT32) msgSizeMax &&
        cmsUtl_strcmp(newObj->version, currObj->version) != 0)
    {
        attrMask |= 0x4000;
        cmsUtl_hexStringToBinaryBuf(newObj->version, &bin, &binSize);
        for (i = 0; i < binSize && j < msgSizeMax; i++, j++)
        {
            attrVal[j] = bin[i];
        }
        cmsMem_free(bin);
    }
    // if serialNumber is changed then send attribute value change notification
    if (j < msgSizeMax &&
        cmsUtl_strcmp(newObj->serialNumber, currObj->serialNumber) != 0)
    {
        attrMask |= 0x2000;
        cmsUtl_hexStringToBinaryBuf(newObj->serialNumber, &bin, &binSize);
        for (i = 0; i < binSize && j < msgSizeMax; i++, j++)
        {
            attrVal[j] = bin[i];
        }
        cmsMem_free(bin);
    }
    // if operational state is changed then send attribute value change notification
    if (j < (UINT32) msgSizeMax - 1 &&
        newObj->operationalState != currObj->operationalState)
    {
        attrMask |= 0x0100;
        UINT8 state = (UINT8)newObj->operationalState;
        memcpy(&attrVal[j], &state, 1);
        j++;
    }
    // if attribute value is changed then send notification for ONT-G
    if (attrMask != 0)
    {
#ifdef BRCM_OMCI
        ret = omciUtl_sendAttributeValueChange(MDMOID_ONT_G, newObj->managedEntityId, attrMask, attrVal, j);
#endif // BRCM_OMCI
    }

    return ret;
}

CmsRet rcl_ontGObject( _OntGObject *newObj,
                const _OntGObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

    if(ADD_NEW(newObj, currObj))
    {
        /* rcl was called while the object is being added, so we do nothing */
        ret = CMSRET_SUCCESS;
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        /* rcl was called while the object is being deleted, so we do nothing */
        ret = CMSRET_SUCCESS;
    }
    else
    {
        if(newObj->administrativeState != currObj->administrativeState)
        {
            //G.988->9.1.1 Administrative state: locks(1), unlocks(0)
            if(newObj->administrativeState == ME_ADMIN_STATE_LOCKED)
            {
                rutGpon_disableAllPptpEthernetUni();
                rutGpon_disableAllGemPortNetworkCtp();
#ifdef DMP_X_ITU_ORG_VOICE_1
                _owapi_rut_disableAllPptpPotsUni();
#endif
            }
            else if(newObj->administrativeState == ME_ADMIN_STATE_UNLOCKED)
            {
                rutGpon_enableAllPptpEthernetUni();
                rutGpon_enableAllGemPortNetworkCtp();
#ifdef DMP_X_ITU_ORG_VOICE_1
                _owapi_rut_enableAllPptpPotsUni();
#endif
            }
            else
            {
                return CMSRET_INVALID_PARAM_VALUE;
            }

            omci_pm_syncAllAdminStates();
        }
    }

   return ret;
}

CmsRet rcl_ont2GObject( _Ont2GObject *newObj __attribute__((unused)),
                const _Ont2GObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_ontDataObject( _OntDataObject *newObj __attribute__((unused)),
                const _OntDataObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))

{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_softwareImageObject( _SoftwareImageObject *newObj __attribute__((unused)),
                const _SoftwareImageObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_cardHolderObject( _CardHolderObject *newObj __attribute__((unused)),
                const _CardHolderObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_attributeValueChangeCircuitPack(const _CircuitPackObject *newObj,
                                           const _CircuitPackObject *currObj)
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 attrMask = 0;
    UINT32 j = 0;
    // Max length of OMCI message is 32, and the first 2 bytes are attribute mask
    // so max length of attribute values is 30
    UINT16 msgSizeMax = OMCI_PACKET_A_MSG_SIZE - 2;
    UINT8 attrVal[msgSizeMax];

    memset(attrVal, 0, msgSizeMax);

    // if operational state is changed then send attribute value change notification
    if (j < (UINT32) msgSizeMax - 1 &&
        newObj->operationalState != currObj->operationalState)
    {
        attrMask |= 0x0200;
        UINT8 state = (UINT8)newObj->operationalState;
        memcpy(&attrVal[j], &state, 1);
        j++;
    }
    // if attribute value is changed then send notification for ONT-G
    if (attrMask != 0)
    {
#ifdef BRCM_OMCI
        ret = omciUtl_sendAttributeValueChange(MDMOID_CIRCUIT_PACK, newObj->managedEntityId, attrMask, attrVal, j);
#endif // BRCM_OMCI
    }

    return ret;
}

CmsRet rcl_circuitPackObject( _CircuitPackObject *newObj,
                const _CircuitPackObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

    if(ADD_NEW(newObj, currObj))
    {
        /* rcl was called while the object is being added, so we do nothing */
        ret = CMSRET_SUCCESS;
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        /* rcl was called while the object is being deleted, so we do nothing */
        ret = CMSRET_SUCCESS;
    }
    else
    {
        if(newObj->administrativeState != currObj->administrativeState)
        {
            /*G.988->9.1.1 Administrative state: locks(1), unlocks(0)
              in mdm_initgpon.c
              GPON_CARD_HOLDER_ETH_MEID vs type 47
              GPON_CARD_HOLDER_MOCA_MEID vs type 46
              GPON_CARD_HOLDER_POTS_MEID vs type 32
              GPON_CARD_HOLDER_GPON_MEID vs type 248
             */
            if(newObj->administrativeState == ME_ADMIN_STATE_LOCKED)
            {
                if(currObj->type == 47)
                {
                   rutGpon_disableAllPptpEthernetUni();
                }
                else if(currObj->type == 248)
                {
                   rutGpon_disableAllGemPortNetworkCtp();
                }
                else if(currObj->type == 32)
                {
#ifdef DMP_X_ITU_ORG_VOICE_1
                   _owapi_rut_disableAllPptpPotsUni();
#endif
                }
            }
            else if(newObj->administrativeState == ME_ADMIN_STATE_UNLOCKED)
            {
                if(currObj->type == 47)
                {
                   rutGpon_enableAllPptpEthernetUni();
                }
                else if(currObj->type == 248)
                {
                   rutGpon_enableAllGemPortNetworkCtp();
                }
                else if(currObj->type == 32)
                {
#ifdef DMP_X_ITU_ORG_VOICE_1
                   _owapi_rut_enableAllPptpPotsUni();
#endif
                }
            }
            else
            {
                return CMSRET_INVALID_PARAM_VALUE;
            }

            omci_pm_syncAllAdminStates();
        }
    }
   return ret;
}

CmsRet rcl_powerSheddingObject( _PowerSheddingObject *newObj __attribute__((unused)),
                const _PowerSheddingObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_portMappingPackageGObject( _PortMappingPackageGObject *newObj __attribute__((unused)),
                const _PortMappingPackageGObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_aniManagementObject( _AniManagementObject *newObj __attribute__((unused)),
                const _AniManagementObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_aniGObject( _AniGObject *newObj,
                const _AniGObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;

    // Test for set existing ANI-G object.
    if (SET_EXISTING(newObj, currObj))
    {
        // Test for MDMOID_ANI_G object changes (only those that apply to OMCIPMD).
        if ((newObj->alarmReportingControl != currObj->alarmReportingControl) ||
            (newObj->alarmReportingControlInterval  != currObj->alarmReportingControlInterval))
        {
            // Attempt to find an existing ARC entry.
            UBOOL8 found = omci_arc_exist(MDMOID_ANI_G,
                                          newObj->managedEntityId);

            if (newObj->alarmReportingControl == TRUE)
            {
                BCM_OMCIPM_ARC_ENTRY* arcEntryPtr = NULL;

                if (found == FALSE)
                {
                    omci_arc_create(MDMOID_ANI_G,
                                    newObj->managedEntityId);
                }

                // Attempt to find an existing ARC entry.
                arcEntryPtr = omci_arc_get(MDMOID_ANI_G,
                                           newObj->managedEntityId);

                if (arcEntryPtr != NULL &&
                    newObj->alarmReportingControlInterval != currObj->alarmReportingControlInterval)
                {
                    omci_arc_set(MDMOID_ANI_G,
                                 newObj->managedEntityId,
                                 newObj->alarmReportingControlInterval);
                }
            }
            else
            {
                if (found == TRUE)
                {
                    omci_arc_delete(MDMOID_ANI_G,
                                    newObj->managedEntityId);
                }
            }
        }

        if ((newObj->sfThreshold != currObj->sfThreshold) ||
            (newObj->sdThreshold != currObj->sdThreshold))
        {
            BCM_Ploam_SFSDthresholdInfo sfSdInfo;
            int gponResult;

            // Setup SF & SD configuration params.
            sfSdInfo.sf_exp = newObj->sfThreshold;
            sfSdInfo.sd_exp = newObj->sdThreshold;
            if ((gponResult = gponCtl_setSFSDThreshold(&sfSdInfo)) != 0)
            {
                // Report non-timeout error.
                cmsLog_error("Cannot set SF/SD configuration params.  Error: %d", gponResult);
            }
        }

        if ((newObj->lowerOptivalThreshold          != currObj->lowerOptivalThreshold)          ||
            (newObj->upperOptivalThreshold          != currObj->upperOptivalThreshold)          ||
            (newObj->lowerTransmitPowerThreshold    != currObj->lowerTransmitPowerThreshold)    ||
            (newObj->upperTransmitPowerThreshold    != currObj->upperTransmitPowerThreshold))
        {
            UBOOL8 existed = omci_th_isObjectExisted(MDMOID_THRESHOLD_DATA1,
                                                     MDMOID_ANI_G);
            ThresholdData1Object data;

            memset(&data, 0, sizeof(ThresholdData1Object));

            if (existed == FALSE)
            {
                omci_th_createObject(MDMOID_THRESHOLD_DATA1,
                                     MDMOID_ANI_G,
                                     (void *)NULL);

                data.managedEntityId = MDMOID_ANI_G;
                if (newObj->lowerOptivalThreshold != currObj->lowerOptivalThreshold)
                    data.thresholdValue1 = newObj->lowerOptivalThreshold;
                else
                    data.thresholdValue1 = DEFAULT_RX_THRESHOLD_VAL_NP;
                if (newObj->upperOptivalThreshold != currObj->upperOptivalThreshold)
                    data.thresholdValue2 = newObj->upperOptivalThreshold;
                else
                    data.thresholdValue2 = DEFAULT_RX_THRESHOLD_VAL_NP;
                if (newObj->lowerTransmitPowerThreshold != currObj->lowerTransmitPowerThreshold)
                    data.thresholdValue3 = newObj->lowerTransmitPowerThreshold;
                else
                    data.thresholdValue3 = DEFAULT_TX_THRESHOLD_VAL_NP;
                if (newObj->upperTransmitPowerThreshold != currObj->upperTransmitPowerThreshold)
                    data.thresholdValue4 = newObj->upperTransmitPowerThreshold;
                else
                    data.thresholdValue4 = DEFAULT_TX_THRESHOLD_VAL_NP;

                omci_th_setObject(MDMOID_THRESHOLD_DATA1,
                                  MDMOID_ANI_G,
                                  (void *)&data);
            }
            else
            {
                if (newObj->lowerOptivalThreshold != currObj->lowerOptivalThreshold)
                {
                    data.thresholdValue1 = newObj->lowerOptivalThreshold;
                    if (data.thresholdValue1 == DEFAULT_RX_THRESHOLD_VAL_NP)
                    {
                        omci_alarm_clear(ALARM_SOAK_ANIG_LO_RX_PWR);
                    }
                }
                else
                    data.thresholdValue1 = currObj->lowerOptivalThreshold;
                if (newObj->upperOptivalThreshold != currObj->upperOptivalThreshold)
                {
                    data.thresholdValue2 = newObj->upperOptivalThreshold;
                    if (data.thresholdValue2 == DEFAULT_RX_THRESHOLD_VAL_NP)
                    {
                        omci_alarm_clear(ALARM_SOAK_ANIG_HI_RX_PWR);
                    }
                }
                else
                    data.thresholdValue2 = currObj->upperOptivalThreshold;
                if (newObj->lowerTransmitPowerThreshold != currObj->lowerTransmitPowerThreshold)
                {
                    data.thresholdValue3 = newObj->lowerTransmitPowerThreshold;
                    if (data.thresholdValue3 == DEFAULT_TX_THRESHOLD_VAL_NP)
                    {
                        omci_alarm_clear(ALARM_SOAK_ANIG_LO_TX_PWR);
                    }
                }
                else
                    data.thresholdValue3 = currObj->lowerTransmitPowerThreshold;
                if (newObj->upperTransmitPowerThreshold != currObj->upperTransmitPowerThreshold)
                {
                    data.thresholdValue4 = newObj->upperTransmitPowerThreshold;
                    if (data.thresholdValue4 == DEFAULT_TX_THRESHOLD_VAL_NP)
                    {
                        omci_alarm_clear(ALARM_SOAK_ANIG_HI_TX_PWR);
                    }
                }
                else
                    data.thresholdValue4 = currObj->upperTransmitPowerThreshold;

                omci_th_setObject(MDMOID_THRESHOLD_DATA1,
                                  MDMOID_ANI_G,
                                  (void *)&data);
            }
        }
    }

    return ret;
}

static CmsRet resetTContObject(int tcont_idx)
{
    CmsRet ret = CMSRET_SUCCESS;
    BCM_Ploam_TcontAllocIdInfo info;
    int qId = 0;
    tmctl_if_t tm_if;

    memset(&tm_if, 0, sizeof(tm_if));
    tm_if.gponIf.tcontid = tcont_idx;

    for (qId= 0; qId < GPON_PHY_US_PQ_MAX; ++qId)
    {
        if (_owapi_rut_tmctl_delQueueCfg(TMCTL_DEV_GPON, &tm_if, qId) != TMCTL_SUCCESS)
        {
            cmsLog_error("tmctl_delQueueCfg failed :tcont=%d, qid=%d\n",
                tcont_idx, qId);
            return CMSRET_INTERNAL_ERROR;
        }
    }
    if (_owapi_rut_tmctl_portTmUninit(TMCTL_DEV_GPON, &tm_if) != TMCTL_SUCCESS)
    {
        cmsLog_error("Failed to uninit tm for tcont me %d\n", tcont_idx);
        return CMSRET_INTERNAL_ERROR;
    }
    memset(&info, 0, sizeof(BCM_Ploam_TcontAllocIdInfo));
    info.tcontIdx = tcont_idx;
    gponCtl_deconfigTcontAllocId(&info);
    return ret;
}

static CmsRet getPriorityQueueFromTcontMeIdAndPriority
    (UINT32 tcontMeid, UINT32 prio, PriorityQueueGObject *prioQ_o)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    PriorityQueueGObject *prioQ = NULL;

    // search PriorityQueueGObject
    while (_cmsObj_getNext(MDMOID_PRIORITY_QUEUE_G,
                           &iidStack,
                           (void **) &prioQ) == CMSRET_SUCCESS)
    {
        UINT32 relatedPortMeid = GetRelatedPortSlotPort(prioQ->relatedPort);
        UINT32 relatedPrio = GetRelatedPortPrio(prioQ->relatedPort);
        if ((relatedPortMeid == tcontMeid) && (relatedPrio == prio))
        {
            memcpy(prioQ_o, prioQ, sizeof(PriorityQueueGObject));
            _cmsObj_free((void **) &prioQ);
            return CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &prioQ);
    }
    return CMSRET_OBJECT_NOT_FOUND;
}

static CmsRet createPriorityQueueForTcont(int tcont_idx, UINT32 tcontMeid)
{
    UINT32 omciPrio = 0;
    OmciSchedulePolicy policy = OMCI_SCHEDULE_POLICY_NONE;
    CmsRet ret = CMSRET_SUCCESS;

    ret = rutGpon_getTContPolicyFromTContMeId(tcontMeid, &policy);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("rutGpon_getTContPolicyFromTContMeId() failed, "
          "OMCI T-CONT ME %d", tcontMeid);
        policy = OMCI_SCHEDULE_POLICY_HOL;
    }

    // By default there are GPON_PHY_US_PQ_MAX upstream Priority Queue per TCont
    for (omciPrio = 0; omciPrio < GPON_PHY_US_PQ_MAX; ++omciPrio)
    {
        tmctl_if_t tm_if;
        tmctl_queueCfg_t tmctl_queueCfg;
        PriorityQueueGObject prioQ;

        memset(&tm_if, 0, sizeof(tm_if));
        tm_if.gponIf.tcontid = tcont_idx;
        memset(&tmctl_queueCfg, 0, sizeof(tmctl_queueCfg));
        memset(&prioQ, 0, sizeof(prioQ));

        if (getPriorityQueueFromTcontMeIdAndPriority(tcontMeid, omciPrio, &prioQ) != CMSRET_SUCCESS)
        {
            cmsLog_error("getPriorityQueueFromTcontMeIdAndPriority failed :tcont=%d, prio=%d\n",
                tcont_idx, omciPrio);
            return CMSRET_INTERNAL_ERROR;
        }

        tmctl_queueCfg.qid = omciPrio;
        if (_owapi_rut_tmctl_getQueueMap() == QID_PRIO_MAP_Q7P7)
        {
            tmctl_queueCfg.priority = omciPrio;
        }
        else
        {
            tmctl_queueCfg.priority = omciTmctlPrioConvert(omciPrio);
        }

        tmctl_queueCfg.qsize = prioQ.allocatedQueueSize;
        tmctl_queueCfg.weight = prioQ.weight;
        if (policy == OMCI_SCHEDULE_POLICY_WRR)
        {
            tmctl_queueCfg.schedMode = TMCTL_SCHED_WRR;
            /* WRR case: set priority value to 0. */
            tmctl_queueCfg.priority = 0;
            /* WRR case: weight can not be 0. */
            if (tmctl_queueCfg.weight == 0)
            {
                tmctl_queueCfg.weight = 1;
            }
        }
        else
        {
            tmctl_queueCfg.schedMode = TMCTL_SCHED_SP;
        }

        tmctl_queueCfg.shaper.shapingRate = 0;
        tmctl_queueCfg.shaper.shapingBurstSize = 0;
        tmctl_queueCfg.shaper.minRate = 0;
        if (_owapi_rut_tmctl_setQueueCfg(TMCTL_DEV_GPON, &tm_if, &tmctl_queueCfg) != TMCTL_SUCCESS)
        {
            cmsLog_error("tmctl_setQueueCfg failed :tcont=%d, qid=%d\n",
                tcont_idx, omciPrio);
            return CMSRET_INTERNAL_ERROR;
        }
    }
    return CMSRET_SUCCESS;
}

CmsRet rcl_tContObject( _TContObject *newObj,
                const _TContObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;
    int tcont_idx;
    BCM_Ploam_TcontAllocIdInfo info;

    if (SET_EXISTING(newObj, currObj))
    {
        tcont_idx = newObj->managedEntityId - GPON_FIRST_TCONT_MEID;
        if ((newObj->allocId != currObj->allocId) &&
          (rutGpon_isAllocIdInitValue(newObj->allocId) == TRUE))
        {
            return resetTContObject(tcont_idx);
        }
        tcont_idx = newObj->managedEntityId - GPON_FIRST_TCONT_MEID;
        if (newObj->allocId != currObj->allocId)
        {
            memset(&info, 0, sizeof(BCM_Ploam_TcontAllocIdInfo));
            info.tcontIdx = tcont_idx;
            // Clear the earlier configured Alloc-ID on this index
            info.allocID = BCM_PLOAM_ALLOC_ID_UNASSIGNED;
            gponCtl_configTcontAllocId(&info);
            // Now set the new Alloc-ID
            info.allocID = newObj->allocId;
            gponCtl_configTcontAllocId(&info);
        }
        // create tcont, create pq
        if ((rutGpon_isAllocIdInitValue(newObj->allocId) != TRUE) &&
          (rutGpon_isAllocIdInitValue(currObj->allocId) == TRUE))
        {
            // setup tm for tcont
            tmctl_if_t tm_if;

            memset(&tm_if, 0, sizeof(tm_if));
            tm_if.gponIf.tcontid = tcont_idx;
            if (_owapi_rut_tmctl_portTmInit(TMCTL_DEV_GPON, &tm_if, 0) != TMCTL_SUCCESS)
            {
                cmsLog_error("Failed to setup tm for tcont me %d\n", tcont_idx);
                return CMSRET_INTERNAL_ERROR;
            }
            if (createPriorityQueueForTcont(tcont_idx, newObj->managedEntityId) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to createPriorityQueueForTcont tcont=%d\n", tcont_idx);
                return CMSRET_INTERNAL_ERROR;
            }
        }
        ret = rutGpon_configGemPortAllocId(newObj->managedEntityId, newObj->allocId);
        // TODO: If T-CONT policy is updated: no need to support. In future,
        // when we support flexible traffic management, what can be done is
        // to remove all existing queues and recreate new queues with a
        // different scheduling policy
    }
    else if (DELETE_EXISTING(newObj, currObj) &&
      (rutGpon_isAllocIdInitValue(currObj->allocId) != TRUE))
    {
        tcont_idx = currObj->managedEntityId - GPON_FIRST_TCONT_MEID;
        return resetTContObject(tcont_idx);
    }
    return ret;
}

CmsRet rcl_gemPortNetworkCtpObject( _GemPortNetworkCtpObject *newObj,
                const _GemPortNetworkCtpObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    UBOOL8 prevHideObjectsPendingDelete = _mdmLibCtx.hideObjectsPendingDelete;
    CmsRet ret = CMSRET_SUCCESS;

    if(ADD_NEW(newObj, currObj))
    {
        // Do not check completed path when CTP is created
        // by _cmsObj_addInstance() since GEMP port is only
        // assigned and enabled later by _cmsObj_set()
        /*if (_mdmLibCtx.eid != EID_HTTPD)
        {
            // create all flows for new GemPortNetworkCtpObject
            rutGpon_checkModelPath(MDMOID_GEM_PORT_NETWORK_CTP,
                                   newObj->managedEntityId,
                                   OMCI_ACTION_CREATE);
        }*/
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        // In OMCI MIB reset case, flows have been deleted earlier
        // in rutGpon_deleteAllRules(). Skip the time-consuming
        // rutGpon_checkModelPath().
        if (rutGpon_getOmciMibResetState() != TRUE)
        {
            _mdmLibCtx.hideObjectsPendingDelete = FALSE;
            // delete all existed flows for this GemPortNetworkCtpObject
            rutGpon_checkModelPath(MDMOID_GEM_PORT_NETWORK_CTP,
                                   currObj->managedEntityId,
                                   OMCI_ACTION_DELETE);
            _mdmLibCtx.hideObjectsPendingDelete = prevHideObjectsPendingDelete;
        }

        rutGpon_deConfigGemPort(currObj);
        rutRgFull_omciServiceCleanupByGemPort(currObj);
    }
    else
    {
        rutGpon_configGemPort(newObj, currObj);

        // create all flows for new GemPortNetworkCtpObject
        rutGpon_checkModelPath(MDMOID_GEM_PORT_NETWORK_CTP,
                               newObj->managedEntityId,
                               OMCI_ACTION_EDIT);
    }

    return ret;
}

CmsRet rcl_attributeValueChangeGemIwTp(const _GemInterworkingTpObject *newObj,
                                       const _GemInterworkingTpObject *currObj)
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 attrMask = 0;
    UINT32 j = 0;
    // Max length of OMCI message is 32, and the first 2 bytes are attribute mask
    // so max length of attribute values is 30
    UINT16 msgSizeMax = OMCI_PACKET_A_MSG_SIZE - 2;
    UINT8 attrVal[msgSizeMax];

    memset(attrVal, 0, msgSizeMax);

    // if operational state is changed then send attribute value change notification
    if (j < (UINT32) msgSizeMax - 1 &&
        newObj->operationalState != currObj->operationalState)
    {
        attrMask |= 0x0400;
        UINT8 state = (UINT8)newObj->operationalState;
        memcpy(&attrVal[j], &state, 1);
        j++;
    }
    // if attribute value is changed then send notification for GEM interworking TP
    if (attrMask != 0)
    {
#ifdef BRCM_OMCI
        ret = omciUtl_sendAttributeValueChange(MDMOID_GEM_INTERWORKING_TP, newObj->managedEntityId, attrMask, attrVal, j);
#endif // BRCM_OMCI
    }

    return ret;
}


CmsRet rcl_gemInterworkingTpObject( _GemInterworkingTpObject *newObj,
                const _GemInterworkingTpObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    UBOOL8 prevHideObjectsPendingDelete = _mdmLibCtx.hideObjectsPendingDelete;
    CmsRet ret = CMSRET_SUCCESS;

    if (ADD_NEW(newObj, currObj))
    {
        // Only check completed path when object is really created
        // by OMCID but not from HTTPD since HTTPD might create
        // then delete object right away just to know the paramters in this object
        if (_mdmLibCtx.eid != EID_HTTPD)
        {
            // create all flows for new GemInterworkingTpObject
            rutGpon_checkModelPath(MDMOID_GEM_INTERWORKING_TP,
                                   newObj->managedEntityId,
                                   OMCI_ACTION_CREATE);
        }
    }
    else if (SET_EXISTING(newObj, currObj))
    {
        // create all flows for new GemInterworkingTpObject
        rutGpon_checkModelPath(MDMOID_GEM_INTERWORKING_TP,
                               newObj->managedEntityId,
                               OMCI_ACTION_EDIT);
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        _mdmLibCtx.hideObjectsPendingDelete = FALSE;
        // delete existed flows of current GemInterworkingTpObject
        rutGpon_checkModelPath(MDMOID_GEM_INTERWORKING_TP,
                               currObj->managedEntityId,
                               OMCI_ACTION_DELETE);
        _mdmLibCtx.hideObjectsPendingDelete = prevHideObjectsPendingDelete;
    }

    return ret;
}

CmsRet rcl_attributeValueChangeMulticastGemIwTp(const _MulticastGemInterworkingTpObject *newObj,
                                                const _MulticastGemInterworkingTpObject *currObj)
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 attrMask = 0;
    UINT32 j = 0;
    // Max length of OMCI message is 32, and the first 2 bytes are attribute mask
    // so max length of attribute values is 30
    UINT16 msgSizeMax = OMCI_PACKET_A_MSG_SIZE - 2;
    UINT8 attrVal[msgSizeMax];

    memset(attrVal, 0, msgSizeMax);

    // if operational state is changed then send attribute value change notification
    if (j < (UINT32) msgSizeMax - 1 &&
        newObj->operationalState != currObj->operationalState)
    {
        attrMask |= 0x0400;
        UINT8 state = (UINT8)newObj->operationalState;
        memcpy(&attrVal[j], &state, 1);
        j++;
    }
    // if attribute value is changed then send notification for Multicast GEM interworking TP
    if (attrMask != 0)
    {
#ifdef BRCM_OMCI
        ret = omciUtl_sendAttributeValueChange(MDMOID_MULTICAST_GEM_INTERWORKING_TP, newObj->managedEntityId, attrMask, attrVal, j);
#endif // BRCM_OMCI
    }

    return ret;
}


CmsRet rcl_multicastGemInterworkingTpObject( _MulticastGemInterworkingTpObject *newObj,
                const _MulticastGemInterworkingTpObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    UBOOL8 prevHideObjectsPendingDelete = _mdmLibCtx.hideObjectsPendingDelete;
    CmsRet ret = CMSRET_SUCCESS;

    if (ADD_NEW(newObj, currObj))
    {
        // Only check completed path when object is really created
        // by OMCID but not from HTTPD since HTTPD might create
        // then delete object right away just to know the paramters in this object
        if (_mdmLibCtx.eid != EID_HTTPD)
        {
            // create all flows for new MulticastGemInterworkingTpObject
            rutGpon_checkModelPath(MDMOID_MULTICAST_GEM_INTERWORKING_TP,
                                   newObj->managedEntityId,
                                   OMCI_ACTION_CREATE);
        }
    }
    else if (SET_EXISTING(newObj, currObj))
    {
        if (newObj->gemPortNetworkCtpConnPointer != currObj->gemPortNetworkCtpConnPointer)
        {
            UBOOL8 found = FALSE;
            InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
            GemPortNetworkCtpObject *ctp = NULL;

            while ((!found) && (_cmsObj_getNextFlags(MDMOID_GEM_PORT_NETWORK_CTP,
              &iidStack, OGF_NO_VALUE_UPDATE, (void**)&ctp) == CMSRET_SUCCESS))
            {
                found = (newObj->gemPortNetworkCtpConnPointer == ctp->managedEntityId);
                if (found == TRUE)
                {
                    rutGpon_reconfigMcastGemPort(ctp);
                }
                _cmsObj_free((void**)&ctp);
            }
        }

        // create all flows for new MulticastGemInterworkingTpObject
        rutGpon_checkModelPath(MDMOID_MULTICAST_GEM_INTERWORKING_TP,
                               newObj->managedEntityId,
                               OMCI_ACTION_EDIT);
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        _mdmLibCtx.hideObjectsPendingDelete = FALSE;
        // delete existed flows of current MulticastGemInterworkingTpObject
        rutGpon_checkModelPath(MDMOID_MULTICAST_GEM_INTERWORKING_TP,
                               currObj->managedEntityId,
                               OMCI_ACTION_DELETE);
        _mdmLibCtx.hideObjectsPendingDelete = prevHideObjectsPendingDelete;
    }

   return ret;
}

CmsRet rcl_gemInterworkingTpMulticastAddressTableObject( _GemInterworkingTpMulticastAddressTableObject *newObj __attribute__((unused)),
                const _GemInterworkingTpMulticastAddressTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_gemPortPmHistoryDataObject( _GemPortPmHistoryDataObject *newObj,
                const _GemPortPmHistoryDataObject *currObj,
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
        CmsRet found = omci_pm_findEntryById(MDMOID_GEM_PORT_PM_HISTORY_DATA,
                                             newObj->managedEntityId);

        if (found == CMSRET_OBJECT_NOT_FOUND)
        {
            ret = omci_pm_createObject(MDMOID_GEM_PORT_PM_HISTORY_DATA,
                                       newObj->managedEntityId,
                                       0,
                                       newObj->thresholdDataId);
        }
        else
        {
            omci_pm_setObject(MDMOID_GEM_PORT_PM_HISTORY_DATA,
                              newObj->managedEntityId,
                              newObj->thresholdDataId);
        }
    }
    // Test for delete existing PM object.
    else if (DELETE_EXISTING(newObj, currObj))
    {
        omci_pm_deleteObject(MDMOID_GEM_PORT_PM_HISTORY_DATA,
                             currObj->managedEntityId);
    }

    return ret;
}

CmsRet rcl_galEthernetProfileObject( _GalEthernetProfileObject *newObj __attribute__((unused)),
                const _GalEthernetProfileObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_galEthernetPmHistoryDataObject( _GalEthernetPmHistoryDataObject *newObj,
                const _GalEthernetPmHistoryDataObject *currObj,
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
        CmsRet found = omci_pm_findEntryById(MDMOID_GAL_ETHERNET_PM_HISTORY_DATA,
                                             newObj->managedEntityId);

        if (found == CMSRET_OBJECT_NOT_FOUND)
        {
            ret = omci_pm_createObject(MDMOID_GAL_ETHERNET_PM_HISTORY_DATA,
                                       newObj->managedEntityId,
                                       0,
                                       newObj->thresholdDataId);
        }
        else
        {
            omci_pm_setObject(MDMOID_GAL_ETHERNET_PM_HISTORY_DATA,
                              newObj->managedEntityId,
                              newObj->thresholdDataId);
        }
    }
    // Test for delete existing PM object.
    else if (DELETE_EXISTING(newObj, currObj))
    {
        omci_pm_deleteObject(MDMOID_GAL_ETHERNET_PM_HISTORY_DATA,
                             currObj->managedEntityId);
    }

    return ret;
}

CmsRet rcl_fecPmHistoryDataObject( _FecPmHistoryDataObject *newObj,
                const _FecPmHistoryDataObject *currObj,
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
        CmsRet found = omci_pm_findEntryById(MDMOID_FEC_PM_HISTORY_DATA,
                                             newObj->managedEntityId);

        if (found == CMSRET_OBJECT_NOT_FOUND)
        {
            ret = omci_pm_createObject(MDMOID_FEC_PM_HISTORY_DATA,
                                       newObj->managedEntityId,
                                       0,
                                       newObj->thresholdDataId);
        }
        else
        {
            omci_pm_setObject(MDMOID_FEC_PM_HISTORY_DATA,
                              newObj->managedEntityId,
                              newObj->thresholdDataId);
        }
    }
    // Test for delete existing PM object.
    else if (DELETE_EXISTING(newObj, currObj))
    {
        omci_pm_deleteObject(MDMOID_FEC_PM_HISTORY_DATA,
                             currObj->managedEntityId);
    }

    return ret;
}


CmsRet rcl_layer2DataServicesObject( _Layer2DataServicesObject *newObj __attribute__((unused)),
                const _Layer2DataServicesObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_macBridgeServiceProfileObject( _MacBridgeServiceProfileObject *newObj,
                const _MacBridgeServiceProfileObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    UBOOL8 prevHideObjectsPendingDelete = _mdmLibCtx.hideObjectsPendingDelete;
    char cmd[BUFLEN_1024];
    char brName[BUFLEN_32];
    CmsRet ret = CMSRET_SUCCESS;

    // initialize object or add instance
    if (ADD_NEW(newObj, currObj))
    {
        // Only check completed path when object is really created
        // by OMCID but not from HTTPD since HTTPD might create
        // then delete object right away just to know the paramters in this object
        if (_mdmLibCtx.eid != EID_HTTPD)
        {
            // create all flows for new MacBridgeServiceProfileObject
            rutGpon_checkModelPath(MDMOID_MAC_BRIDGE_SERVICE_PROFILE,
                                   newObj->managedEntityId,
                                   OMCI_ACTION_CREATE);
        }
        //check Learning ind, only support disable learning
        if (newObj->learning == FALSE)
        {
            memset(brName, 0, sizeof(brName));
            if (rutGpon_getBridgeNameFromBridgeMeId(newObj->managedEntityId, brName) == CMSRET_SUCCESS)
            {
                //set age time as 0 to diable bridge learning.
                snprintf(cmd, sizeof(cmd), "brctl setageing %s 0", brName);
                _owapi_rut_doSystemAction("rcl_gpon", cmd);
            }
        }
        else if (rutGpon_getOmciPromiscInfo() == FALSE)
            ret = CMSRET_METHOD_NOT_SUPPORTED;
    }
    // set object
    else if (SET_EXISTING(newObj, currObj))
    {
        // create new object when current MeId is 0
        if (currObj->managedEntityId == 0)
        {
            ret = rutGpon_addAutoMacBridgeConfigDataObject(newObj);
            // create proprietary BCM_MacBridgeServiceProfileObject to keep bridge info
            ret = rutGpon_addAutoObject
                (MDMOID_BC_MAC_BRIDGE_SERVICE_PROFILE, newObj->managedEntityId, FALSE);
        }
	 // modify existed object when current MeId is not 0
        else
        {
            ret = rutGpon_setAutoMacBridgeConfigDataObject
                (newObj, currObj->managedEntityId);
            if (newObj->managedEntityId != currObj->managedEntityId)
            {
                ret = rutGpon_setAutoObject
                    (MDMOID_BC_MAC_BRIDGE_SERVICE_PROFILE,
                     currObj->managedEntityId, newObj->managedEntityId);
            }
        }
        // create all flows for new MacBridgeServiceProfileObject
        rutGpon_checkModelPath(MDMOID_MAC_BRIDGE_SERVICE_PROFILE,
                               newObj->managedEntityId,
                               OMCI_ACTION_EDIT);
         //check Learning ind
         if (newObj->learning != currObj->learning )
         {
            if (newObj->learning == FALSE)
            {
               memset(brName, 0, sizeof(brName));
               if (rutGpon_getBridgeNameFromBridgeMeId(newObj->managedEntityId, brName) == CMSRET_SUCCESS)
                  {
                  //set age time .
                  snprintf(cmd, sizeof(cmd), "brctl setageing %s %d", brName, newObj->learning ? 300 : 0);
                  _owapi_rut_doSystemAction("rcl_gpon", cmd);
                  }
            }
            rutGpon_updateBridgeLearningInd(currObj);
         }
    }
    // delete object
    else if (DELETE_EXISTING(newObj, currObj))
    {
        _mdmLibCtx.hideObjectsPendingDelete = FALSE;
        // delete existed flows of current MacBridgeServiceProfileObject
        rutGpon_checkModelPath(MDMOID_MAC_BRIDGE_SERVICE_PROFILE,
                               currObj->managedEntityId,
                               OMCI_ACTION_DELETE);
        _mdmLibCtx.hideObjectsPendingDelete = prevHideObjectsPendingDelete;
        ret = rutGpon_deleteAutoObject
            (MDMOID_MAC_BRIDGE_CONFIG_DATA, currObj->managedEntityId);
        ret = rutGpon_deleteAutoObject
            (MDMOID_BC_MAC_BRIDGE_SERVICE_PROFILE, currObj->managedEntityId);
    }

   return ret;
}

CmsRet rcl_bCM_MacBridgeServiceProfileObject( _BCM_MacBridgeServiceProfileObject *newObj,
                const _BCM_MacBridgeServiceProfileObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;

    if (SET_EXISTING(newObj, currObj))
    {
        // allow to create bridge that has the same Id 0 but
        // different name with default bridge (br0 vs bronu0)
        char ifName[CMS_IFNAME_LENGTH];
        if (newObj->bridgeName == NULL)
        {
            snprintf(ifName, sizeof(ifName), "bronu%d", newObj->managedEntityId);
            CMSMEM_REPLACE_STRING_FLAGS(newObj->bridgeName, ifName, 0);
            // add the new bridge interface
            rutGpon_addBridgeInterface(newObj->bridgeName);
        }
        else if (newObj->managedEntityId != currObj->managedEntityId)
        {
            // remove the current bridge interface except when
            // managedEntityId is 0 since all bridge are created with 0 first
            if (currObj->bridgeName != NULL &&
                rutGpon_isInterfaceExisted(currObj->bridgeName) == TRUE &&
                currObj->managedEntityId != 0)
            {
                rutGpon_deleteBridgeInterface(currObj->bridgeName);
            }
            snprintf(ifName, sizeof(ifName), "bronu%d", newObj->managedEntityId);
            CMSMEM_REPLACE_STRING_FLAGS(newObj->bridgeName, ifName, 0);
            // add the new bridge interface
            rutGpon_addBridgeInterface(newObj->bridgeName);
        }
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        if (currObj->bridgeName != NULL &&
            rutGpon_isInterfaceExisted(currObj->bridgeName) == TRUE)
        {
            // remove the curent bridge interface
            rutGpon_deleteBridgeInterface(currObj->bridgeName);
        }
    }

    return ret;
}

CmsRet rcl_macBridgeConfigDataObject( _MacBridgeConfigDataObject *newObj __attribute__((unused)),
                const _MacBridgeConfigDataObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_macBridgePmHistoryDataObject( _MacBridgePmHistoryDataObject *newObj,
                const _MacBridgePmHistoryDataObject *currObj,
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
        CmsRet found = omci_pm_findEntryById(MDMOID_MAC_BRIDGE_PM_HISTORY_DATA,
                                             newObj->managedEntityId);

        if (found == CMSRET_OBJECT_NOT_FOUND)
        {
            ret = omci_pm_createObject(MDMOID_MAC_BRIDGE_PM_HISTORY_DATA,
                                       newObj->managedEntityId,
                                       0,
                                       newObj->thresholdDataId);
        }
        else
        {
            omci_pm_setObject(MDMOID_MAC_BRIDGE_PM_HISTORY_DATA,
                              newObj->managedEntityId,
                              newObj->thresholdDataId);
        }
    }
    // Test for delete existing PM object.
    else if (DELETE_EXISTING(newObj, currObj))
    {
        omci_pm_deleteObject(MDMOID_MAC_BRIDGE_PM_HISTORY_DATA,
                             currObj->managedEntityId);
    }

    return ret;
}

CmsRet rcl_macBridgePortConfigDataObject( _MacBridgePortConfigDataObject *newObj,
                const _MacBridgePortConfigDataObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    UBOOL8 prevHideObjectsPendingDelete = _mdmLibCtx.hideObjectsPendingDelete;
    omciMcastCfgInfo mcastCfgInfo;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 mePointer = 0;
    UINT8 type = 0;

    // initialize object or add instance
    if (ADD_NEW(newObj, currObj))
    {
        ret = rutGpon_addAutoObject
            (MDMOID_MAC_BRIDGE_PORT_DESIGNATION_DATA, newObj->managedEntityId, TRUE);
        ret = rutGpon_addAutoObject
            (MDMOID_MAC_BRIDGE_PORT_FILTER_TABLE_DATA, newObj->managedEntityId, TRUE);
        ret = rutGpon_addAutoObject
            (MDMOID_MAC_BRIDGE_PORT_FILTER_PRE_ASSIGN_TABLE, newObj->managedEntityId, TRUE);
        ret = rutGpon_addAutoObject
            (MDMOID_MAC_BRIDGE_PORT_BRIDGE_TABLE_DATA, newObj->managedEntityId, TRUE);
        ret = rutGpon_addAutoObject
            (MDMOID_MAC_BP_ICMPV6_PROCESS_PREASSIGN_TABLE, newObj->managedEntityId, TRUE);
        // Only check completed path when object is really created
        // by OMCID but not from HTTPD since HTTPD might create
        // then delete object right away just to know the paramters in this object
        if (_mdmLibCtx.eid != EID_HTTPD)
        {
            // create all flows for new MacBridgePortConfigDataObject
            rutGpon_checkModelPath(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                                   newObj->managedEntityId,
                                   OMCI_ACTION_CREATE);
        }
    }
    // set object
    else if (SET_EXISTING(newObj, currObj))
    {
        if (newObj->managedEntityId != currObj->managedEntityId)
        {
            type = newObj->tpType;
            mePointer = newObj->tpPointer;
            rutGpon_macBpCfgDataSetUniConfigState(type, mePointer);

            ret = rutGpon_setAutoObject
                (MDMOID_MAC_BRIDGE_PORT_DESIGNATION_DATA,
                 currObj->managedEntityId, newObj->managedEntityId);
            ret = rutGpon_setAutoObject
                (MDMOID_MAC_BRIDGE_PORT_FILTER_TABLE_DATA,
                 currObj->managedEntityId, newObj->managedEntityId);
            ret = rutGpon_setAutoObject
                (MDMOID_MAC_BRIDGE_PORT_FILTER_PRE_ASSIGN_TABLE,
                 currObj->managedEntityId, newObj->managedEntityId);
            ret = rutGpon_setAutoObject
                (MDMOID_MAC_BRIDGE_PORT_BRIDGE_TABLE_DATA,
                 currObj->managedEntityId, newObj->managedEntityId);
            ret = rutGpon_setAutoObject
                (MDMOID_MAC_BP_ICMPV6_PROCESS_PREASSIGN_TABLE,
                 currObj->managedEntityId, newObj->managedEntityId);
        }
        // create all flows for new MacBridgePortConfigDataObject
        rutGpon_checkModelPath(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                               newObj->managedEntityId,
                               OMCI_ACTION_EDIT);

        if((newObj->outTdPointer != 0)
           &&(newObj->outTdPointer != currObj->outTdPointer)
           &&(newObj->tpType == OMCI_BP_TP_PPTP_ETH))
        {
           rutGpon_configPptpEthShapingDs(newObj, FALSE);
        }

        if((newObj->inTdPointer != 0)
           &&(newObj->inTdPointer != currObj->inTdPointer)
           &&(newObj->tpType == OMCI_BP_TP_PPTP_ETH))
        {
           rutGpon_configPptpEthShapingUs(newObj, FALSE);
        }
        rutGpon_configPortLearningInd(newObj,FALSE);
    }
    // delete object
    else if (DELETE_EXISTING(newObj, currObj))
    {
        type = currObj->tpType;
        mePointer = currObj->tpPointer;
        rutGpon_macBpCfgDataSetUniConfigState(type, mePointer);

        ret = rutGpon_deleteAutoObject
            (MDMOID_MAC_BRIDGE_PORT_DESIGNATION_DATA, currObj->managedEntityId);
        ret = rutGpon_deleteAutoObject
            (MDMOID_MAC_BRIDGE_PORT_FILTER_TABLE_DATA, currObj->managedEntityId);
        ret = rutGpon_deleteAutoObject
            (MDMOID_MAC_BRIDGE_PORT_FILTER_PRE_ASSIGN_TABLE, currObj->managedEntityId);
        ret = rutGpon_deleteAutoObject
            (MDMOID_MAC_BRIDGE_PORT_BRIDGE_TABLE_DATA, currObj->managedEntityId);
        ret = rutGpon_deleteAutoObject
            (MDMOID_MAC_BP_ICMPV6_PROCESS_PREASSIGN_TABLE, currObj->managedEntityId);
        _mdmLibCtx.hideObjectsPendingDelete = FALSE;
        // delete existed flows of current MacBridgePortConfigDataObject
        rutGpon_checkModelPath(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                               currObj->managedEntityId,
                               OMCI_ACTION_DELETE);

        if((currObj->outTdPointer != 0)
           &&(currObj->tpType == OMCI_BP_TP_PPTP_ETH))
        {
           rutGpon_configPptpEthShapingDs(currObj, TRUE);
        }

        if((currObj->inTdPointer != 0)
           &&(currObj->tpType == OMCI_BP_TP_PPTP_ETH))
        {
           rutGpon_configPptpEthShapingUs(currObj, TRUE);
        }

        rutGpon_configPortLearningInd(currObj, TRUE);

        /* Check if the MAC BPCD ME instance is associated with a UNI port. */
        if (isUniMacBpcd(currObj->tpType))
        {
            /* Reset the configuration when the MAC BPCD is deleted. */
            memset(&mcastCfgInfo, 0x0, sizeof(omciMcastCfgInfo));
            ret = _owapi_rut_setOmciMcastCfgInfo(&mcastCfgInfo);
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("owapi_rut_setOmciMcastCfgInfo(%d) failed, ret = %d",
                  mcastCfgInfo.upstreamIgmpRate, ret);
            }
        }

        _mdmLibCtx.hideObjectsPendingDelete = prevHideObjectsPendingDelete;
    }

    return ret;
}

CmsRet rcl_macBridgePortDesignationDataObject( _MacBridgePortDesignationDataObject *newObj __attribute__((unused)),
                const _MacBridgePortDesignationDataObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_macBridgePortFilterTableDataObject( _MacBridgePortFilterTableDataObject *newObj __attribute__((unused)),
                const _MacBridgePortFilterTableDataObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;

    return ret;
}

CmsRet rcl_macFilterTableObject( _MacFilterTableObject *newObj __attribute__((unused)),
                const _MacFilterTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_macBridgePortFilterPreAssignTableObject( _MacBridgePortFilterPreAssignTableObject *newObj __attribute__((unused)),
                const _MacBridgePortFilterPreAssignTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_macBridgePortBridgeTableDataObject( _MacBridgePortBridgeTableDataObject *newObj __attribute__((unused)),
                const _MacBridgePortBridgeTableDataObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_bridgeTableObject( _BridgeTableObject *newObj __attribute__((unused)),
                const _BridgeTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_macBridgePortPmHistoryDataObject( _MacBridgePortPmHistoryDataObject *newObj,
                const _MacBridgePortPmHistoryDataObject *currObj,
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
        CmsRet found = omci_pm_findEntryById(MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA,
                                             newObj->managedEntityId);

        if (found == CMSRET_OBJECT_NOT_FOUND)
        {
            ret = omci_pm_createObject(MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA,
                                       newObj->managedEntityId,
                                       0,
                                       newObj->thresholdDataId);
        }
        else
        {
            omci_pm_setObject(MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA,
                              newObj->managedEntityId,
                              newObj->thresholdDataId);
        }
    }
    // Test for delete existing PM object.
    else if (DELETE_EXISTING(newObj, currObj))
    {
        omci_pm_deleteObject(MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA,
                             currObj->managedEntityId);
    }

    return ret;
}

CmsRet rcl_mapperServiceProfileObject( _MapperServiceProfileObject *newObj,
                const _MapperServiceProfileObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    UBOOL8 prevHideObjectsPendingDelete = _mdmLibCtx.hideObjectsPendingDelete;
    CmsRet ret = CMSRET_SUCCESS;

    if (ADD_NEW(newObj, currObj))
    {
        // Only check completed path when object is really created
        // by OMCID but not from HTTPD since HTTPD might create
        // then delete object right away just to know the paramters in this object
        if (_mdmLibCtx.eid != EID_HTTPD)
        {
            // create all flows if path is completed
            rutGpon_checkModelPath(MDMOID_MAPPER_SERVICE_PROFILE,
                                   newObj->managedEntityId,
                                   OMCI_ACTION_CREATE);
        }
    }
    else if (SET_EXISTING(newObj, currObj))
    {
        // create new object when current MeId is 0
        if (currObj->managedEntityId == 0)
        {
            // create proprietary BCM_MapperServiceProfileObject to keep bridge info
            ret = rutGpon_addAutoObject
                (MDMOID_BC_MAPPER_SERVICE_PROFILE, newObj->managedEntityId, FALSE);
        }
	 // modify existed object when current MeId is not 0
        else
        {
            if (newObj->managedEntityId != currObj->managedEntityId)
            {
                ret = rutGpon_setAutoObject
                    (MDMOID_BC_MAPPER_SERVICE_PROFILE,
                     currObj->managedEntityId, newObj->managedEntityId);
            }
        }

#ifdef OMCIPROV_WORKAROUND
        /*
         * Setting interworkTPPointer in 802.1p mapper to 0xffff (null) may break
         * the model path before rutGpon_checkModelPath() is invoked. Ignore the setting.
         */
        if (newObj->interworkTpPointerPriority0 == 0xffff &&
            newObj->interworkTpPointerPriority0 != currObj->interworkTpPointerPriority0)
        {
            newObj->interworkTpPointerPriority0 = currObj->interworkTpPointerPriority0;
        }
        if (newObj->interworkTpPointerPriority1 == 0xffff &&
            newObj->interworkTpPointerPriority1 != currObj->interworkTpPointerPriority1)
        {
            newObj->interworkTpPointerPriority1 = currObj->interworkTpPointerPriority1;
        }
        if (newObj->interworkTpPointerPriority2 == 0xffff &&
            newObj->interworkTpPointerPriority2 != currObj->interworkTpPointerPriority2)
        {
            newObj->interworkTpPointerPriority2 = currObj->interworkTpPointerPriority2;
        }
        if (newObj->interworkTpPointerPriority3 == 0xffff &&
            newObj->interworkTpPointerPriority3 != currObj->interworkTpPointerPriority3)
        {
            newObj->interworkTpPointerPriority3 = currObj->interworkTpPointerPriority3;
        }
        if (newObj->interworkTpPointerPriority4 == 0xffff &&
            newObj->interworkTpPointerPriority4 != currObj->interworkTpPointerPriority4)
        {
            newObj->interworkTpPointerPriority4 = currObj->interworkTpPointerPriority4;
        }
        if (newObj->interworkTpPointerPriority5 == 0xffff &&
            newObj->interworkTpPointerPriority5 != currObj->interworkTpPointerPriority5)
        {
            newObj->interworkTpPointerPriority5 = currObj->interworkTpPointerPriority5;
        }
        if (newObj->interworkTpPointerPriority6 == 0xffff &&
            newObj->interworkTpPointerPriority6 != currObj->interworkTpPointerPriority6)
        {
            newObj->interworkTpPointerPriority6 = currObj->interworkTpPointerPriority6;
        }
        if (newObj->interworkTpPointerPriority7 == 0xffff &&
            newObj->interworkTpPointerPriority7 != currObj->interworkTpPointerPriority7)
        {
            newObj->interworkTpPointerPriority7 = currObj->interworkTpPointerPriority7;
        }
#endif /* OMCIPROV_WORKAROUND */

        // create all flows if path is completed
        rutGpon_checkModelPath(MDMOID_MAPPER_SERVICE_PROFILE,
                               newObj->managedEntityId,
                               OMCI_ACTION_EDIT);
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        _mdmLibCtx.hideObjectsPendingDelete = FALSE;
        // delete all flows for completed path
        rutGpon_checkModelPath(MDMOID_MAPPER_SERVICE_PROFILE,
                               currObj->managedEntityId,
                               OMCI_ACTION_DELETE);
        _mdmLibCtx.hideObjectsPendingDelete = prevHideObjectsPendingDelete;
        ret = rutGpon_deleteAutoObject
            (MDMOID_BC_MAPPER_SERVICE_PROFILE, currObj->managedEntityId);
    }

    return ret;
}

CmsRet rcl_bCM_MapperServiceProfileObject( _BCM_MapperServiceProfileObject *newObj,
                const _BCM_MapperServiceProfileObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;

    if (SET_EXISTING(newObj, currObj))
    {
        // allow to create bridge that has the same Id 0 but
        // different name with default bridge (br0 vs bronu0)
        char ifName[CMS_IFNAME_LENGTH];
        if (newObj->bridgeName == NULL)
        {
            snprintf(ifName, sizeof(ifName), "brmap%d", newObj->managedEntityId);
            CMSMEM_REPLACE_STRING_FLAGS(newObj->bridgeName, ifName, 0);
            // add the new bridge interface
            rutGpon_addBridgeInterface(newObj->bridgeName);
        }
        else if (newObj->managedEntityId != currObj->managedEntityId)
        {
            // remove the current bridge interface except when
            // managedEntityId is 0 since all bridge are created with 0 first
            if (currObj->bridgeName != NULL &&
                rutGpon_isInterfaceExisted(currObj->bridgeName) == TRUE &&
                currObj->managedEntityId != 0)
            {
                rutGpon_deleteBridgeInterface(currObj->bridgeName);
            }
            snprintf(ifName, sizeof(ifName), "brmap%d", newObj->managedEntityId);
            CMSMEM_REPLACE_STRING_FLAGS(newObj->bridgeName, ifName, 0);
            // add the new bridge interface
            rutGpon_addBridgeInterface(newObj->bridgeName);
        }
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        if (currObj->bridgeName != NULL &&
            rutGpon_isInterfaceExisted(currObj->bridgeName) == TRUE)
        {
            // remove the curent bridge interface
            rutGpon_deleteBridgeInterface(currObj->bridgeName);
        }
    }

    return ret;
}

CmsRet rcl_vlanTaggingFilterDataObject( _VlanTaggingFilterDataObject *newObj,
                const _VlanTaggingFilterDataObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    UBOOL8 prevHideObjectsPendingDelete = _mdmLibCtx.hideObjectsPendingDelete;
    CmsRet ret = CMSRET_SUCCESS;

    // VlanTaggingFilterDataObject is implicitly linked
    // to MacBridgePortConfigDataObject
    // so they have the same managedEntityId
    // Since VlanTaggingFilterDataObject is implicitly linked
    // with other objects, its existence does not effect the completeness of the path
    // from ANI to UNI. Other words, the path might be completed even when this
    // object is created or deleted. So it's neccessary to delete the old flows when this
    // object is created before creating the new flows. Also when this object is deleted the
    // new flows also need to be created after deleting the old flows
    if (ADD_NEW(newObj, currObj))
    {
        // Only check completed path when object is really created
        // by OMCID but not from HTTPD since HTTPD might create
        // then delete object right away just to know the paramters in this object
        if (_mdmLibCtx.eid != EID_HTTPD)
        {
            // if action is create
            // need to create new flows to avoid traffic interuption
            // before delete existed flows if path is completed
            rutGpon_checkModelPath(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                                   newObj->managedEntityId,
                                   OMCI_ACTION_CREATE);
        }
    }
    else if (SET_EXISTING(newObj, currObj))
    {
        // if action is edit
        // need to create new flows to avoid traffic interuption
        // before delete existed flows if path is completed
        rutGpon_checkModelPath(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                               newObj->managedEntityId,
                               OMCI_ACTION_EDIT);
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        // if action is delete
        // need to delete existed rules or flows
         _mdmLibCtx.hideObjectsPendingDelete = FALSE;
        rutGpon_checkModelPath(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                               currObj->managedEntityId,
                               OMCI_ACTION_DELETE);
        _mdmLibCtx.hideObjectsPendingDelete = prevHideObjectsPendingDelete;
        // if path is still completed after this
        // object is deleted then create new rules or flows
        rutGpon_checkModelPath(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                               currObj->managedEntityId,
                               OMCI_ACTION_CREATE);
    }

    return ret;
}

CmsRet rcl_vlanTaggingOperationConfigurationDataObject( _VlanTaggingOperationConfigurationDataObject *newObj,
                const _VlanTaggingOperationConfigurationDataObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    UINT8  type = 0;
    UBOOL8 prevHideObjectsPendingDelete = _mdmLibCtx.hideObjectsPendingDelete;
    UINT32 mePointer = 0, meId = 0, oid = 0;
    CmsRet ret = CMSRET_SUCCESS;
    OmciObjectAction action = OMCI_ACTION_NONE;

    if (ADD_NEW(newObj, currObj))
    {
        // Only check completed path when object is really created
        // by OMCID but not from HTTPD since HTTPD might create
        // then delete object right away just to know the paramters in this object
        if (_mdmLibCtx.eid != EID_HTTPD)
        {
            type = newObj->associationType;
            mePointer = newObj->associatedManagedEntityPointer;
            meId = newObj->managedEntityId;
            action = OMCI_ACTION_CREATE;
        }
    }
    else if (SET_EXISTING(newObj, currObj))
    {
        type = newObj->associationType;
        mePointer = newObj->associatedManagedEntityPointer;
        meId = newObj->managedEntityId;
        action = OMCI_ACTION_EDIT;
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        type = currObj->associationType;
        mePointer = currObj->associatedManagedEntityPointer;
        meId = currObj->managedEntityId;
        action = OMCI_ACTION_DELETE;
        _mdmLibCtx.hideObjectsPendingDelete = FALSE;
    }

    switch (type)
    {
        case OMCI_VLAN_TP_IP_HOST:
            oid = MDMOID_IP_HOST_CONFIG_DATA;
            break;
        case OMCI_VLAN_TP_MAPPER_SERVICE:
            oid = MDMOID_MAPPER_SERVICE_PROFILE;
            break;
        case OMCI_VLAN_TP_MAC_BRIDGE_PORT:
            oid = MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA;
            break;
        case OMCI_VLAN_TP_GEM_INTERWORKING:
            oid = MDMOID_GEM_INTERWORKING_TP;
            break;
        case OMCI_VLAN_TP_PPTP_MOCA:
            oid = MDMOID_PPTP_MOCA_UNI;
            break;
        case OMCI_VLAN_TP_PPTP_ETH:
            oid = MDMOID_PPTP_ETHERNET_UNI;
            omciDm_setUniEntryConfigState(MDMOID_PPTP_ETHERNET_UNI,
              mePointer, TRUE);
            break;
        case OMCI_VLAN_TP_DEFAULT:
        default:
            oid = MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA;
            mePointer = meId;
            break;
    }

    switch (action)
    {
        case OMCI_ACTION_CREATE:
        case OMCI_ACTION_EDIT:
            // if action is create or edit
            // need to create new flows to avoid traffic interuption
            // before delete existed flows if path is completed
            rutGpon_checkModelPath(oid, mePointer, action);
            break;
        case OMCI_ACTION_DELETE:
            // if action is delete
            // need to delete existed rules or flows
            rutGpon_checkModelPath(oid, mePointer, action);
            // reset hideObjectsPendingDelete
            _mdmLibCtx.hideObjectsPendingDelete = prevHideObjectsPendingDelete;
            // if path is still completed after this
            // object is deleted then create new rules or flows
            rutGpon_checkModelPath(oid, mePointer, OMCI_ACTION_CREATE);
            break;
        default:
            break;
    }

    return ret;
}

CmsRet rcl_extendedVlanTaggingOperationConfigurationDataObject( _ExtendedVlanTaggingOperationConfigurationDataObject *newObj,
                const _ExtendedVlanTaggingOperationConfigurationDataObject *currObj,
                const InstanceIdStack *iidStack,
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    UINT8  type = 0;
    UBOOL8 prevHideObjectsPendingDelete = _mdmLibCtx.hideObjectsPendingDelete;
    UINT32 mePointer = 0, oid = 0;
    CmsRet ret = CMSRET_SUCCESS;
    OmciObjectAction action = OMCI_ACTION_NONE;

    if (ADD_NEW(newObj, currObj))
    {
        // Only check completed path when object is really created
        // by OMCID but not from HTTPD since HTTPD might create
        // then delete object right away just to know the paramters in this object
        if (_mdmLibCtx.eid != EID_HTTPD)
        {
            type = newObj->associationType;
            mePointer = newObj->associatedManagedEntityPointer;
            // create all flows if path is completed
            action = OMCI_ACTION_CREATE;
            // create default VLAN tagging rule
            rutGpon_addDefaultReceivedFrameVlanTaggingOperationTableObject(iidStack);
        }
    }
    else if (SET_EXISTING(newObj, currObj))
    {
        type = newObj->associationType;
        mePointer = newObj->associatedManagedEntityPointer;
        // delete all flows for completed path
        action = OMCI_ACTION_EDIT;
        // create all flows if path is completed
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        type = currObj->associationType;
        mePointer = currObj->associatedManagedEntityPointer;
        _mdmLibCtx.hideObjectsPendingDelete = FALSE;
        // delete all flows for completed path
        action = OMCI_ACTION_DELETE;
    }

    switch (type)
    {
        case OMCI_XVLAN_TP_MAC_BRIDGE_PORT:
            oid = MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA;
            break;
        case OMCI_XVLAN_TP_MAPPER_SERVICE:
            oid = MDMOID_MAPPER_SERVICE_PROFILE;
            break;
        case OMCI_XVLAN_TP_PPTP_ETH:
            oid = MDMOID_PPTP_ETHERNET_UNI;
            omciDm_setUniEntryConfigState(MDMOID_PPTP_ETHERNET_UNI,
              mePointer, TRUE);
            break;
        case OMCI_XVLAN_TP_GEM_INTERWORKING:
            oid = MDMOID_GEM_INTERWORKING_TP;
            break;
        case OMCI_XVLAN_TP_PPTP_MOCA:
            oid = MDMOID_PPTP_MOCA_UNI;
            break;
        default:
            break;
    }

    // Since ExtendedVlanTaggingOperationConfigurationDataObject is implicitly linked
    // with other objects, its existence does not effect the completeness of the path
    // from ANI to UNI. Other words, the path might be completed even when this
    // object is created or deleted. So it's neccessary to delete the old flows when this
    // object is created before creating the new flows. Also when this object is deleted the
    // new flows also need to be created after deleting the old flows
    switch (action)
    {
        case OMCI_ACTION_CREATE:
        case OMCI_ACTION_EDIT:
            // if action is create or edit
            // need to create new flows to avoid traffic interuption
            // before delete existed flows if path is completed
            rutGpon_checkModelPath(oid, mePointer, action);
            break;
        case OMCI_ACTION_DELETE:
            // if action is delete
            // need to delete existed rules or flows
            rutGpon_checkModelPath(oid, mePointer, action);
            // reset hideObjectsPendingDelete
            _mdmLibCtx.hideObjectsPendingDelete = prevHideObjectsPendingDelete;
            // if path is still completed after this
            // object is deleted then create new rules or flows
            rutGpon_checkModelPath(oid, mePointer, OMCI_ACTION_CREATE);
            break;
        default:
            break;
    }

   return ret;
}

CmsRet rcl_receivedFrameVlanTaggingOperationTableObject( _ReceivedFrameVlanTaggingOperationTableObject *newObj __attribute__((unused)),
                const _ReceivedFrameVlanTaggingOperationTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_multicastOperationsProfileObject( _MulticastOperationsProfileObject *newObj __attribute__((unused)),
                const _MulticastOperationsProfileObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    omciMcastCfgInfo mcastCfgInfo;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 igmpRate = 0;
    MulticastSubscriberConfigInfoObject *msciObj = NULL;
    InstanceIdStack iidStack2 = EMPTY_INSTANCE_ID_STACK;
    UBOOL8 found = FALSE;

    memset(&mcastCfgInfo, 0x0, sizeof(omciMcastCfgInfo));

    if (SET_EXISTING(newObj, currObj))
    {
        if ((newObj->upstreamIgmpRate != currObj->upstreamIgmpRate) ||
          (newObj->unauthorizedJoinRequestBehaviour != currObj->unauthorizedJoinRequestBehaviour))
        {
            if ((!found) &&
              (_cmsObj_getNext(MDMOID_MULTICAST_SUBSCRIBER_CONFIG_INFO,
              &iidStack2, (void**)&msciObj)) == CMSRET_SUCCESS)
            {
                /*
                 * TODO: when owapi_rut_setOmciMcastCfgInfo() becomes per-port based,
                 * search further to find the associated UNI port.
                 */ 
                if (newObj->managedEntityId == msciObj->multicastOperationsProfilePointer)
                {
                    found = TRUE;
                    mcastCfgInfo.upstreamIgmpRate = newObj->upstreamIgmpRate;
                    mcastCfgInfo.igmpAdmission = TRUE;
                    mcastCfgInfo.joinForceForward = newObj->unauthorizedJoinRequestBehaviour;
                }
                _cmsObj_free((void**)&msciObj);
            }
        }
    }

    if (found == TRUE)
    {
        ret = _owapi_rut_setOmciMcastCfgInfo(&mcastCfgInfo);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("owapi_rut_setOmciMcastCfgInfo(%d) failed, ret = %d",
              igmpRate, ret);
        }
    }

    return ret;
}

CmsRet rcl_dynamicAccessControlListTableObject( _DynamicAccessControlListTableObject *newObj __attribute__((unused)),
                const _DynamicAccessControlListTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;
#ifdef DMP_X_BROADCOM_COM_MCAST_1
    if(_owapi_mcast_getHostCtrlConfig() == TRUE)
    {
        if (SET_EXISTING(newObj, currObj))
        {
            ret = rutGpon_updateDynamicAccessControl(currObj, newObj, iidStack);
        }
        else if(DELETE_EXISTING(newObj, currObj))
        {
            ret = rutGpon_deleteDynamicAccessControl(currObj, iidStack);
        }
    }
#endif

   return ret;
}

CmsRet rcl_staticAccessControlListTableObject( _StaticAccessControlListTableObject *newObj __attribute__((unused)),
                const _StaticAccessControlListTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_lostGroupsListTableObject( _LostGroupsListTableObject *newObj __attribute__((unused)),
                const _LostGroupsListTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_multicastSubscriberConfigInfoObject( _MulticastSubscriberConfigInfoObject *newObj __attribute__((unused)),
                const _MulticastSubscriberConfigInfoObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
#ifdef DMP_X_BROADCOM_COM_MCAST_1
    UBOOL8 foundProfile = FALSE;
    InstanceIdStack iidStack2 = EMPTY_INSTANCE_ID_STACK;
    MulticastOperationsProfileObject *mopObj = NULL;
    omciMcastCfgInfo mcastCfgInfo;
    MacBridgePortConfigDataObject *macBpcdObj = NULL;
    UBOOL8 found = FALSE;
    CmsRet ret = CMSRET_SUCCESS;

    memset(&mcastCfgInfo, 0x0, sizeof(omciMcastCfgInfo));
    if (SET_EXISTING(newObj, currObj))
    {
        /* Check if there is an associated MOP ME instance. */
        if (newObj->multicastOperationsProfilePointer != currObj->multicastOperationsProfilePointer)
        {
            INIT_INSTANCE_ID_STACK(&iidStack2);

            while ((!foundProfile) &&
              (_cmsObj_getNext(MDMOID_MULTICAST_OPERATIONS_PROFILE,
              &iidStack2, (void**)&mopObj) == CMSRET_SUCCESS))
            {
                if (newObj->multicastOperationsProfilePointer == mopObj->managedEntityId)
                {
                    foundProfile = TRUE;
                    mcastCfgInfo.upstreamIgmpRate = mopObj->upstreamIgmpRate;
                    mcastCfgInfo.igmpAdmission = TRUE;
                    mcastCfgInfo.joinForceForward = mopObj->unauthorizedJoinRequestBehaviour;
                }
                _cmsObj_free((void**)&mopObj);
            }
        }
    }

    if (foundProfile == TRUE)
    {
        INIT_INSTANCE_ID_STACK(&iidStack2);

        /* Check if the MSCI ME instance is associated with a UNI port. */
        while ((!found) && (_cmsObj_getNext(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
          &iidStack2, (void**)&macBpcdObj) == CMSRET_SUCCESS))
        {
            if ((isUniMacBpcd(macBpcdObj->tpType)) &&
              (macBpcdObj->managedEntityId == newObj->managedEntityId))
            {
                found = TRUE;
            }
            _cmsObj_free((void**)&macBpcdObj);
        }
    }

    if (found == TRUE)
    {
        ret = _owapi_rut_setOmciMcastCfgInfo(&mcastCfgInfo);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("owapi_rut_setOmciMcastCfgInfo(%d) failed, ret = %d",
              mcastCfgInfo.upstreamIgmpRate, ret);
        }
    }

    return _owapi_rut_processHostCtrlChange();
#endif
    return CMSRET_SUCCESS;
}

CmsRet rcl_multicastSubscriberMonitorObject( _MulticastSubscriberMonitorObject *newObj __attribute__((unused)),
                const _MulticastSubscriberMonitorObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_activeGroupsListTableObject( _ActiveGroupsListTableObject *newObj __attribute__((unused)),
                const _ActiveGroupsListTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_bCM_MulticastSubscriberMonitorObject( _BCM_MulticastSubscriberMonitorObject *newObj __attribute__((unused)),
                const _BCM_MulticastSubscriberMonitorObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_multicastSubscriberObject( _MulticastSubscriberObject *newObj __attribute__((unused)),
                const _MulticastSubscriberObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_layer3DataServicesObject( _Layer3DataServicesObject *newObj __attribute__((unused)),
                const _Layer3DataServicesObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_attributeValueChangeIpHost(const _IpHostConfigDataObject *newObj,
                const _IpHostConfigDataObject *currObj)
{
    CmsRet cmsReturn = CMSRET_SUCCESS;
    UINT32 attrMask = 0;
    UINT32 size;
    UINT32 i = 0, j = 0;
    // Max length of OMCI message is 32, and the first 2 bytes are attribute mask
    // so max length of attribute values is 30
    UINT16 msgSizeMax = OMCI_PACKET_A_MSG_SIZE - 2;
    UINT8 attrVal[msgSizeMax];
    UINT8 *bin = NULL;
    UINT32 binSize;


    if (newObj->currentAddress != currObj->currentAddress)
    {
        attrMask |= 0x0080;
        size = sizeof(newObj->currentAddress);
        for (i = 0; i < size; i++)
        {
            attrVal[j++] = (UINT8)((newObj->currentAddress >> ((size - 1 - i) * 8)) & 0xff);
        }
    }
    if (newObj->currentMask != currObj->currentMask)
    {
        attrMask |= 0x0040;
        size = sizeof(newObj->currentMask);
        for (i = 0; i < size; i++)
        {
            attrVal[j++] = (UINT8)((newObj->currentMask >> ((size - 1 - i) * 8)) & 0xff);
        }
    }
    if (newObj->currentGateway != currObj->currentGateway)
    {
        attrMask |= 0x0020;
        size = sizeof(newObj->currentGateway);
        for (i = 0; i < size; i++)
        {
            attrVal[j++] = (UINT8)((newObj->currentGateway >> ((size - 1 - i) * 8)) & 0xff);
        }
    }
    if (newObj->currentPrimaryDns != currObj->currentPrimaryDns)
    {
        attrMask |= 0x0010;
        size = sizeof(newObj->currentPrimaryDns);
        for (i = 0; i < size; i++)
        {
            attrVal[j++] = (UINT8)((newObj->currentPrimaryDns>> ((size - 1 - i) * 8)) & 0xff);
        }
    }
    if (newObj->currentSecondaryDns != currObj->currentSecondaryDns)
    {
        attrMask |= 0x0008;
        size = sizeof(newObj->currentSecondaryDns);
        for (i = 0; i < size; i++)
        {
            attrVal[j++] = (UINT8)((newObj->currentSecondaryDns>> ((size - 1 - i) * 8)) & 0xff);
        }
    }

    /* Only handle attribute value change notification for IP host config data
         * if changes are not from OMCID.
         */
    if (attrMask != 0)
    {
#ifdef BRCM_OMCI
       cmsReturn = omciUtl_sendAttributeValueChange(MDMOID_IP_HOST_CONFIG_DATA, newObj->managedEntityId, attrMask, attrVal, j);
#endif // BRCM_OMCI
    }

    //test domain
    memset(attrVal, 0, sizeof(attrVal));
    if (memcmp(newObj->domainName, currObj->domainName, 50))
    {
        attrMask = 0x0004;
        cmsUtl_hexStringToBinaryBuf(newObj->domainName, &bin, &binSize);
        memcpy(attrVal, bin, binSize > msgSizeMax ? msgSizeMax : binSize);
        cmsMem_free(bin);
#ifdef BRCM_OMCI
        cmsReturn = omciUtl_sendAttributeValueChange(MDMOID_IP_HOST_CONFIG_DATA, newObj->managedEntityId, attrMask, attrVal, 25);
#endif // BRCM_OMCI
    }

    //test hostName
    memset(attrVal, 0, sizeof(attrVal));
    if (memcmp(newObj->hostName, currObj->hostName, 50))
    {
        attrMask = 0x0002;
        cmsUtl_hexStringToBinaryBuf(newObj->hostName, &bin, &binSize);
        memcpy(attrVal, bin, binSize > msgSizeMax ? msgSizeMax : binSize);
        cmsMem_free(bin);
#ifdef BRCM_OMCI
        cmsReturn = omciUtl_sendAttributeValueChange(MDMOID_IP_HOST_CONFIG_DATA, newObj->managedEntityId, attrMask, attrVal, 25);
#endif // BRCM_OMCI
    }

    return cmsReturn;
}

CmsRet rcl_ipHostConfigDataObject( _IpHostConfigDataObject *newObj,
                const _IpHostConfigDataObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    UBOOL8 prevHideObjectsPendingDelete = _mdmLibCtx.hideObjectsPendingDelete;
    CmsRet ret = CMSRET_SUCCESS;

    if (ADD_NEW(newObj, currObj))
    {
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        // delete corresponding RUT object
        ret = _owapi_rut_delAutoObject(MDMOID_IP_HOST_CONFIG_DATA,
          currObj->managedEntityId);

        _mdmLibCtx.hideObjectsPendingDelete = FALSE;
        // delete all existed flows for this IpHostConfigDataObject
        rutGpon_checkModelPath(MDMOID_IP_HOST_CONFIG_DATA,
                               currObj->managedEntityId,
                               OMCI_ACTION_DELETE);
        _mdmLibCtx.hideObjectsPendingDelete = prevHideObjectsPendingDelete;
    }
    else
    {
        if ((currObj->managedEntityId == 0) &&
          (newObj->managedEntityId != 0))
        {
            // create corresponding RUT object
            ret = _owapi_rut_addAutoObject(MDMOID_IP_HOST_CONFIG_DATA,
              newObj->managedEntityId, FALSE);
        }

        // start DHCPC or activate IpHost interface
        // for new IpHostConfigDataObject
        rutGpon_checkModelPath(MDMOID_IP_HOST_CONFIG_DATA,
                               newObj->managedEntityId,
                               OMCI_ACTION_EDIT);
        if (newObj->managedEntityId != 0)
        {
            // set corresponding RUT object
            ret = _owapi_rut_setAutoObject(MDMOID_IP_HOST_CONFIG_DATA,
              newObj->managedEntityId, newObj);
        }
    }

    return ret;
}

CmsRet rcl_bCM_IpHostConfigDataObject( _BCM_IpHostConfigDataObject *newObj,
                const _BCM_IpHostConfigDataObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;

    if (ADD_NEW(newObj, currObj))
    {
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
    }
    else
    {
    }

   return ret;
}

CmsRet rcl_ipHostPmHistoryDataObject( _IpHostPmHistoryDataObject *newObj,
                const _IpHostPmHistoryDataObject *currObj,
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
        CmsRet found = omci_pm_findEntryById(MDMOID_IP_HOST_PM_HISTORY_DATA,
                                             newObj->managedEntityId);

        if (found == CMSRET_OBJECT_NOT_FOUND)
        {
            ret = omci_pm_createObject(MDMOID_IP_HOST_PM_HISTORY_DATA,
                                       newObj->managedEntityId,
                                       0,
                                       newObj->thresholdDataId);
        }
        else
        {
            omci_pm_setObject(MDMOID_IP_HOST_PM_HISTORY_DATA,
                              newObj->managedEntityId,
                              newObj->thresholdDataId);
        }
    }
    // Test for delete existing PM object.
    else if (DELETE_EXISTING(newObj, currObj))
    {
        omci_pm_deleteObject(MDMOID_IP_HOST_PM_HISTORY_DATA,
                             currObj->managedEntityId);
    }

    return ret;
}

CmsRet rcl_ipv6HostConfigDataObject( _Ipv6HostConfigDataObject *newObj,
                const _Ipv6HostConfigDataObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;

#ifdef SUPPORT_IPV6

    UBOOL8 prevHideObjectsPendingDelete = _mdmLibCtx.hideObjectsPendingDelete;

    if(ADD_NEW(newObj, currObj))
    {
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        // delete corresponding RUT object
        ret = _owapi_rut_delAutoObject(MDMOID_IPV6_HOST_CONFIG_DATA,
          currObj->managedEntityId);
        _mdmLibCtx.hideObjectsPendingDelete = FALSE;
        // delete all existed flows for this Ipv6HostConfigDataObject
        rutGpon_checkModelPath(MDMOID_IPV6_HOST_CONFIG_DATA,
                               currObj->managedEntityId,
                               OMCI_ACTION_DELETE);
        _mdmLibCtx.hideObjectsPendingDelete = prevHideObjectsPendingDelete;
    }
    else
    {
       if (newObj->managedEntityId != currObj->managedEntityId)
       {
           if ((currObj->managedEntityId == 0) &&
             (newObj->managedEntityId != 0))
           {
               // create corresponding RUT object
               ret = _owapi_rut_addAutoObject(MDMOID_IPV6_HOST_CONFIG_DATA,
                 newObj->managedEntityId, FALSE);
           }
       }

        // start DHCP6C or activate Ipv6Host interface
        // for new Ipv6HostConfigDataObject
        // TODO: For XAVI, if the following can trigger dhcp6c???
        rutGpon_checkModelPath(MDMOID_IPV6_HOST_CONFIG_DATA,
                               newObj->managedEntityId,
                               OMCI_ACTION_EDIT);
        if (newObj->managedEntityId != 0)
        {
            // set corresponding RUT object
            ret = _owapi_rut_setAutoObject(MDMOID_IPV6_HOST_CONFIG_DATA,
              newObj->managedEntityId, newObj);
        }
    }

#endif /* SUPPORT_IPV6 */

   return ret;
}

CmsRet rcl_bCM_Ipv6HostConfigDataObject(
                _BCM_Ipv6HostConfigDataObject *newObj __attribute__((unused)),
                const _BCM_Ipv6HostConfigDataObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;

#ifdef SUPPORT_IPV6
    if (ADD_NEW(newObj, currObj))
    {
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
    }
    else
    {
    }
#endif /* SUPPORT_IPV6 */

   return ret;
}

CmsRet rcl_ipv6CurrentAddressTableObject(
                _Ipv6CurrentAddressTableObject *newObj __attribute__((unused)),
                const _Ipv6CurrentAddressTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;
    return ret;
}

CmsRet rcl_ipv6CurrentDefaultRouterTableObject(
                _Ipv6CurrentDefaultRouterTableObject *newObj __attribute__((unused)),
                const _Ipv6CurrentDefaultRouterTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;
    return ret;
}

CmsRet rcl_ipv6CurrentDnsTableObject(
                _Ipv6CurrentDnsTableObject *newObj __attribute__((unused)),
                const _Ipv6CurrentDnsTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;
    return ret;
}

CmsRet rcl_ipv6CurrentOnlinkPrefixTableObject(
                _Ipv6CurrentOnlinkPrefixTableObject *newObj __attribute__((unused)),
                const _Ipv6CurrentOnlinkPrefixTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;
   return ret;
}

CmsRet rcl_tcpUdpConfigDataObject( _TcpUdpConfigDataObject *newObj,
                const _TcpUdpConfigDataObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;

    if (ADD_NEW(newObj, currObj))
    {
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        ret = _owapi_rut_delAutoObject(MDMOID_TCP_UDP_CONFIG_DATA,
          currObj->managedEntityId);
    }
    else
    {
        if ((currObj->managedEntityId == 0) &&
          (newObj->managedEntityId != 0))
        {
            // create corresponding RUT object
            ret = _owapi_rut_addAutoObject(MDMOID_TCP_UDP_CONFIG_DATA,
              newObj->managedEntityId, FALSE);
        }

        if (newObj->managedEntityId != 0)
        {
            ret = _owapi_rut_setAutoObject(MDMOID_TCP_UDP_CONFIG_DATA,
              newObj->managedEntityId, newObj);
        }
    }

    return ret;
}

CmsRet rcl_ethernetServicesObject( _EthernetServicesObject *newObj __attribute__((unused)),
                const _EthernetServicesObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_attributeValueChangePptpEthUni(const _PptpEthernetUniObject *newObj,
                                          const _PptpEthernetUniObject *currObj)
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 attrMask = 0;
    UINT32 j = 0;
    // Max length of OMCI message is 32, and the first 2 bytes are attribute mask
    // so max length of attribute values is 30
    UINT16 msgSizeMax = OMCI_PACKET_A_MSG_SIZE - 2;
    UINT8 attrVal[msgSizeMax];

    memset(attrVal, 0, msgSizeMax);

    // if operational state is changed then send attribute value change notification
    if (j < (UINT32) msgSizeMax - 1 &&
        newObj->operationalState != currObj->operationalState)
    {
        attrMask |= 0x0400;
        UINT8 state = (UINT8)newObj->operationalState;
        memcpy(&attrVal[j], &state, 1);
        j++;
    }
    // if attribute value is changed then send notification for Pptp Eth UNI
    if (attrMask != 0)
    {
#ifdef BRCM_OMCI
        ret = omciUtl_sendAttributeValueChange(MDMOID_PPTP_ETHERNET_UNI, newObj->managedEntityId, attrMask, attrVal, j);
#endif // BRCM_OMCI
    }

    return ret;
}

CmsRet rcl_pptpEthernetUniObject( _PptpEthernetUniObject *newObj,
                const _PptpEthernetUniObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;

    if (ADD_NEW(newObj, currObj))
    {
        cmsLog_notice("ADD_NEW - do nothing");
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        cmsLog_notice("DELETE_EXISTING - do nothing");
    }
    else
    {
#ifndef G9991
        // Set existing ENET PPTP object.
        UINT32 speed = 0, duplex = 0;
        UINT32 loopback = 0;
        UINT32 firstMeId = 0, portNum = 0;
        int ethRet = 0;

        omciDm_setUniEntryConfigState(MDMOID_PPTP_ETHERNET_UNI,
          newObj->managedEntityId, TRUE);

        // if Ethernet port is used for Veip then do nothing
        if (rutGpon_isVeipPptpUni(MDMOID_PPTP_ETHERNET_UNI, newObj->managedEntityId) == TRUE)
        {
            return ret;
        }

        // Test for MDMOID_PPTP_ETHERNET_UNI object changes (only those that apply to OMCIPMD).
        if (newObj->alarmReportingControl != currObj->alarmReportingControl ||
            newObj->alarmReportingControlInterval != currObj->alarmReportingControlInterval)
        {
            // Attempt to find an existing ARC entry.
            UBOOL8 found = omci_arc_exist(MDMOID_PPTP_ETHERNET_UNI,
                                          newObj->managedEntityId);

            if (newObj->alarmReportingControl == TRUE)
            {
                BCM_OMCIPM_ARC_ENTRY* arcEntryPtr = NULL;

                if (found == FALSE)
                {
                    omci_arc_create(MDMOID_PPTP_ETHERNET_UNI,
                                    newObj->managedEntityId);
                }

                // Attempt to find an existing ARC entry.
                arcEntryPtr = omci_arc_get(MDMOID_PPTP_ETHERNET_UNI,
                                           newObj->managedEntityId);

                if (arcEntryPtr != NULL &&
                    newObj->alarmReportingControlInterval != currObj->alarmReportingControlInterval)
                {
                    omci_arc_set(MDMOID_PPTP_ETHERNET_UNI,
                                 newObj->managedEntityId,
                                 newObj->alarmReportingControlInterval);
                }
            }
            else
            {
                if (found == TRUE)
                {
                    omci_arc_delete(MDMOID_PPTP_ETHERNET_UNI,
                                    newObj->managedEntityId);
                }
            }
        }

        rutGpon_getFirstEthernetMeId(&firstMeId);

        portNum = newObj->managedEntityId - firstMeId;

        if (newObj->administrativeState != currObj->administrativeState)
        {
            //G.988->9.5.1 Administrative state: locks(1), unlocks(0)

            if (newObj->administrativeState == ME_ADMIN_STATE_LOCKED)
            {
                omci_pm_syncEnetAdminStates(portNum, TRUE);
                ret = rutGpon_disablePptpEthernetUni(portNum);
            }
            else if ((newObj->administrativeState == ME_ADMIN_STATE_UNLOCKED)
                     &&(ME_ADMIN_STATE_UNLOCKED == rutGpon_getRelatedAdminState(MDMOID_PPTP_ETHERNET_UNI, currObj->managedEntityId)))
            {
                omci_pm_syncEnetAdminStates(portNum, FALSE);
                ret = rutGpon_enablePptpEthernetUni(portNum);
            }
            else
            {
                return CMSRET_INVALID_PARAM_VALUE;
            }
        }

        //check auto detection configuration
        if (newObj->autoDetectionConfiguration != currObj->autoDetectionConfiguration)
        {
            switch (newObj->autoDetectionConfiguration)
            {
                case 0x00:
                    speed = 0;
                    break;

                case 0x01:
                    speed = 10;
                    duplex = 1;
                    break;

                case 0x02:
                    speed = 100;
                    duplex = 1;
                    break;

                case 0x11:
                    speed = 10;
                    break;

                case 0x12:
                    speed = 100;
                    break;

                default:
                    cmsLog_error("Invalid autoDetectCfg codepoint 0x%x on eth port %d",
                                 newObj->autoDetectionConfiguration, portNum);
                    return CMSRET_INVALID_PARAM_VALUE;
                    break;
            }

            ethRet = bcm_phy_mode_set(0, bcm_enet_map_oam_idx_to_phys_port(portNum), speed, duplex);
        }

        //check loopback
        if (newObj->loopbackConfiguration != currObj->loopbackConfiguration)
        {
            switch (newObj->loopbackConfiguration)
            {
                case 0:
                    break;

                case 3:
                    loopback = 1;
                    break;

                default:
                    return CMSRET_INVALID_PARAM_VALUE;
                    break;
            }

            ethRet = bcm_port_loopback_set(0, portNum, loopback);
        }

        //check max frame size
        if (newObj->maxFrameSize != currObj->maxFrameSize)
        {
            if(newObj->maxFrameSize < ENET_MAX_MTU_PAYLOAD_SIZE) /*linux support maximum mtu of 2000 bytes*/
            {
                char cmd[512];

                snprintf(cmd, sizeof(cmd), "ifconfig eth%d mtu %d up", portNum, newObj->maxFrameSize);
                _owapi_rut_doSystemAction("rcl_gpon", cmd);
            }
        }

        if (ethRet != 0)
        {
            ret = CMSRET_INVALID_PARAM_VALUE;
        }
#endif
    }

    return ret;
}

static CmsRet __pmHistoryDataObject(const PmHdObjCmnHdr_t *newObj,
                const PmHdObjCmnHdr_t *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)),
                UINT32 meType)
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
        CmsRet found = CMSRET_SUCCESS;

        found = omci_pm_findEntryById(meType, newObj->meId);
        if (found == CMSRET_OBJECT_NOT_FOUND)
        {
            ret = omci_pm_createObject(meType, newObj->meId,
                0, // Will be overriden in alloc function
                newObj->thrDataId);
        }
        else
        {
            omci_pm_setObject(meType, newObj->meId, newObj->thrDataId);
        }
    }
    // Test for delete existing PM object.
    else if (DELETE_EXISTING(newObj, currObj))
    {
        omci_pm_deleteObject(meType, currObj->meId);
    }

    return ret;
}

static CmsRet __extPmHistoryDataObject(_EthernetFrameExtendedPMObject *newObj,
                const _EthernetFrameExtendedPMObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)),
                UINT32 meType)
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 bufSize = 0;
    UINT8 *buf = NULL;

    // Test for new PM object.
    if (ADD_NEW(newObj, currObj))
    {
        // do nothing
    }
    // Test for set existing PM object.
    else if (SET_EXISTING(newObj, currObj))
    {
        CmsRet found = CMSRET_SUCCESS;

        if (cmsUtl_hexStringToBinaryBuf(newObj->controlBlock, &buf, &bufSize) == CMSRET_SUCCESS)
        {
            if (bufSize != EXT_PM_CONTROL_BLOCK_SIZE)
            {
                cmsMem_free(buf);
                return CMSRET_INTERNAL_ERROR;
            }
            found = omci_pm_findEntryById(meType, newObj->managedEntityId);
            if (found == CMSRET_OBJECT_NOT_FOUND)
            {
                ret = omci_extPm_createObject(meType, newObj->managedEntityId, buf);
            }
            else
            {
                omci_extPm_setObject(meType, newObj->managedEntityId, buf);
            }

            /* Clear Accumulation Disable bit 16, the action bit. */
            if ((buf[6] & 0x80) != 0)
            {
                char tmp[3];
                char *newControlBlock = cmsMem_strdup(newObj->controlBlock);
                if (newControlBlock != NULL)
                {
                    sprintf(tmp, "%02x", buf[6] & 0x7F);
                    newControlBlock[12] = tmp[0];
                    CMSMEM_REPLACE_STRING_FLAGS(newObj->controlBlock, newControlBlock,
                      0);
                    cmsMem_free(newControlBlock);
                }
            }
            cmsMem_free(buf);
        }
        else
        {
            return CMSRET_INTERNAL_ERROR;
        }
    }
    // Test for delete existing PM object.
    else if (DELETE_EXISTING(newObj, currObj))
    {
        omci_pm_deleteObject(meType, currObj->managedEntityId);
    }

    return ret;
}

CmsRet rcl_ethernetPmHistoryDataObject(_EthernetPmHistoryDataObject *newObj,
     const _EthernetPmHistoryDataObject *currObj,
     const InstanceIdStack *iidStack,
     char **errorParam,
     CmsRet *errorCode)
{
    return __pmHistoryDataObject((PmHdObjCmnHdr_t*)newObj, (PmHdObjCmnHdr_t*)currObj,
        iidStack, errorParam, errorCode,
        MDMOID_ETHERNET_PM_HISTORY_DATA);
}

CmsRet rcl_ethernetPmHistoryData2Object(_EthernetPmHistoryData2Object *newObj,
     const _EthernetPmHistoryData2Object *currObj,
     const InstanceIdStack *iidStack,
     char **errorParam,
     CmsRet *errorCode)
{
    return __pmHistoryDataObject((PmHdObjCmnHdr_t*)newObj, (PmHdObjCmnHdr_t*)currObj,
        iidStack, errorParam, errorCode,
        MDMOID_ETHERNET_PM_HISTORY_DATA2);
}

CmsRet rcl_ethernetPmHistoryData3Object(_EthernetPmHistoryData3Object *newObj,
     const _EthernetPmHistoryData3Object *currObj,
     const InstanceIdStack *iidStack,
     char **errorParam,
     CmsRet *errorCode)
{
    return __pmHistoryDataObject((PmHdObjCmnHdr_t*)newObj, (PmHdObjCmnHdr_t*)currObj,
        iidStack, errorParam, errorCode,
        MDMOID_ETHERNET_PM_HISTORY_DATA3);
}

CmsRet rcl_downstreamEthernetFramePmHistoryDataObject(_DownstreamEthernetFramePmHistoryDataObject*newObj,
     const _DownstreamEthernetFramePmHistoryDataObject *currObj,
     const InstanceIdStack *iidStack,
     char **errorParam,
     CmsRet *errorCode)
{
    return __pmHistoryDataObject((PmHdObjCmnHdr_t*)newObj, (PmHdObjCmnHdr_t*)currObj,
        iidStack, errorParam, errorCode,
        MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA);
}

CmsRet rcl_upstreamEthernetFramePmHistoryDataObject(_UpstreamEthernetFramePmHistoryDataObject *newObj,
     const _UpstreamEthernetFramePmHistoryDataObject *currObj,
     const InstanceIdStack *iidStack,
     char **errorParam,
     CmsRet *errorCode)
{
    return __pmHistoryDataObject((PmHdObjCmnHdr_t*)newObj, (PmHdObjCmnHdr_t*)currObj,
        iidStack, errorParam, errorCode,
        MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA);
}

CmsRet rcl_ethernetFrameExtendedPMObject( _EthernetFrameExtendedPMObject *newObj,
                const _EthernetFrameExtendedPMObject *currObj,
                const InstanceIdStack *iidStack,
                char **errorParam,
                CmsRet *errorCode)
{
    return __extPmHistoryDataObject(newObj, currObj, iidStack, errorParam, errorCode,
        MDMOID_ETHERNET_FRAME_EXTENDED_P_M);
}

CmsRet rcl_attributeValueChangeVeip(const _VirtualEthernetInterfacePointObject *newObj,
                                          const _VirtualEthernetInterfacePointObject *currObj)
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 attrMask = 0;
    UINT32 j = 0;
    // Max length of OMCI message is 32, and the first 2 bytes are attribute mask
    // so max length of attribute values is 30
    UINT16 msgSizeMax = OMCI_PACKET_A_MSG_SIZE - 2;
    UINT8 attrVal[msgSizeMax];

    memset(attrVal, 0, msgSizeMax);

    // if operational state is changed then send attribute value change notification
    if (j < (UINT32) msgSizeMax - 1 &&
        newObj->operationalState != currObj->operationalState)
    {
        attrMask |= 0x0400;
        UINT8 state = (UINT8)newObj->operationalState;
        memcpy(&attrVal[j], &state, 1);
        j++;
    }
    // if attribute value is changed then send notification for Veip
    if (attrMask != 0)
    {
#ifdef BRCM_OMCI
        ret = omciUtl_sendAttributeValueChange(MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT, newObj->managedEntityId, attrMask, attrVal, j);
#endif // BRCM_OMCI
    }

    return ret;
}

CmsRet rcl_virtualEthernetInterfacePointObject( _VirtualEthernetInterfacePointObject *newObj,
                const _VirtualEthernetInterfacePointObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;

    if (SET_EXISTING(newObj, currObj))
    {
        if(newObj->administrativeState != currObj->administrativeState)
        {
            //Administrative state: locks(1), unlocks(0)
            if(newObj->administrativeState != ME_ADMIN_STATE_LOCKED &&
               newObj->administrativeState != ME_ADMIN_STATE_UNLOCKED)
            {
               return CMSRET_INVALID_PARAM_VALUE;
            }
        }
    }

    return ret;
}


CmsRet rcl_mocaServicesObject( _MocaServicesObject *newObj __attribute__((unused)),
                const _MocaServicesObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}


#ifdef SUPPORT_MOCA

CmsRet rcl_pptpMocaUniObject( _PptpMocaUniObject *newObj,
                const _PptpMocaUniObject *currObj,
                const InstanceIdStack *iidStack,
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   UINT32 id;
   CmsRet ret = CMSRET_SUCCESS;

   id = PEEK_INSTANCE_ID(iidStack);
   if (id != 1)
   {
      cmsLog_error("multiple instances of moca not supported, got %d, expected 1", id);
      return CMSRET_INTERNAL_ERROR;
   }


   if (newObj != NULL && currObj == NULL)
   {
      cmsLog_debug("Entered: new case, no action here since action was done in the TR-098 data model");
   }
   else if (newObj != NULL && currObj != NULL)
   {
      LanMocaIntfObject *mocaObj=NULL;
      InstanceIdStack mocaIidStack = EMPTY_INSTANCE_ID_STACK;

      cmsLog_debug("Entered: modify case, id=%d", id);

      /*
       * sync the new settings back up to the TR-098 data model moca object.
       * Let the RCL handler function there do the configuration.
       */
      if ((ret = rutMoca_findPrimaryMocaObject(iidStack, &mocaObj, &mocaIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not find corresponding moca obj");
         return ret;
      }

      /* copy the params to mocaObj */
      mocaObj->loopbackConfiguration = newObj->loopbackConfiguration;
      mocaObj->administrativeState = newObj->administrativeState;
      /* don't copy operationalState because it is read-only */
      mocaObj->maxFrameSize = newObj->maxFrameSize;
      mocaObj->alarmReportingControl = newObj->alarmReportingControl;
      mocaObj->alarmReportingControlInterval = newObj->alarmReportingControlInterval;
      mocaObj->pppoeFilter = newObj->pppoeFilter;
      /* don't copy networkstatus because it is read-only */
      //mocaObj->password = newObj->password;
      mocaObj->privacy = newObj->privacy;
      mocaObj->minBandwidthAlarmThreshold = newObj->minBandwidthAlarmThreshold;
      mocaObj->frequencyMask = newObj->frequencyMask;
      /* don't copy rfChannel because it is read-only */
      mocaObj->lastOperationalFrequency = newObj->lastOperationalFrequency;

      ret = _cmsObj_set(mocaObj, &mocaIidStack);

      _cmsObj_free((void **) &mocaObj);
   }
   else if (newObj == NULL && currObj != NULL)
   {
      /* this object can be deleted when OLT sends MIBReset OMCI command to ONT. */
      cmsLog_notice("something is deleting gpon moca object, id=%d", id);
   }

   return ret;
}

CmsRet rcl_brcmPptpMocaUniObject( _BrcmPptpMocaUniObject *newObj __attribute__((unused)),
                const _BrcmPptpMocaUniObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack,
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet              ret = CMSRET_SUCCESS;
   UINT32 id;

   id = PEEK_INSTANCE_ID(iidStack);
   if (id != 1)
   {
      cmsLog_error("multiple instances of moca not supported, got %d, expected 1", id);
      return CMSRET_INTERNAL_ERROR;
   }


   if (newObj != NULL && currObj == NULL)
   {
      cmsLog_debug("Entered: new case, no action here since action was done in the TR-098 data model");
   }
   else if (newObj != NULL && currObj != NULL)
   {
      LanMocaIntfObject *mocaObj=NULL;
      InstanceIdStack mocaIidStack = EMPTY_INSTANCE_ID_STACK;

      cmsLog_debug("Entered: modify case, id=%d", id);

      /*
       * sync the new settings back up to the TR-098 data model moca object.
       * Let the RCL handler function there do the configuration.
       */
      if ((ret = rutMoca_findPrimaryMocaObject(iidStack, &mocaObj, &mocaIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not find corresponding moca obj");
         return ret;
      }

      /* copy the params to mocaObj */
      mocaObj->ncMode = newObj->ncMode;
      mocaObj->autoNwSearch = newObj->autoNwSearch;
      mocaObj->txPwrControl = newObj->txPwrControl;
      mocaObj->continuousPowerMode = newObj->continuousPowerMode;
      mocaObj->mcastMode = newObj->mcastMode;
      mocaObj->labMode = newObj->labMode;
      mocaObj->eclQTagMode = newObj->eclQTagMode;
      mocaObj->maxTransmitTime = newObj->maxTransmitTime;
      mocaObj->maxTransmitPower = newObj->maxTransmitPower;
      mocaObj->snrMargin = newObj->snrMargin;
      mocaObj->outOfOrderLmo = newObj->outOfOrderLmo;
      mocaObj->lmoReportEnable = newObj->lmoReportEnable;
      mocaObj->tabooMaskStart = newObj->tabooMaskStart;
      mocaObj->tabooChannelMask = newObj->tabooChannelMask;
      mocaObj->continuousIerrInsert = newObj->continuousIerrInsert;
      mocaObj->continuousIeMapInsert = newObj->continuousIeMapInsert;
      mocaObj->maxPktAggr = newObj->maxPktAggr;
      mocaObj->maxConstellationNode = newObj->maxConstellationNode;
      mocaObj->maxConstellation = newObj->maxConstellation;
      mocaObj->preferedNetworkController = newObj->preferedNetworkController;
      mocaObj->pmkExchangeInterval = newObj->pmkExchangeInterval;
      mocaObj->tekExchangeInterval = newObj->tekExchangeInterval;

      ret = _cmsObj_set(mocaObj, &mocaIidStack);

      _cmsObj_free((void **) &mocaObj);
   }
   else if (newObj == NULL && currObj != NULL)
   {
      /* strange, this object should not get deleted. */
      cmsLog_error("something is deleting gpon moca object, id=%d", id);
   }

   return ret;
}

#else

/*
 * These versions of the functions will be used on 96369PGW systems, where
 * GPON is defined, but Moca is not defined.
 */
CmsRet rcl_pptpMocaUniObject( _PptpMocaUniObject *newObj __attribute__((unused)),
                const _PptpMocaUniObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   return CMSRET_SUCCESS;
}

CmsRet rcl_brcmPptpMocaUniObject( _BrcmPptpMocaUniObject *newObj __attribute__((unused)),
                const _BrcmPptpMocaUniObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

#endif  /* SUPPORT_MOCA */


CmsRet rcl_mocaStatusObject( _MocaStatusObject *newObj __attribute__((unused)),
                const _MocaStatusObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;
   /*
    * Doesn't make much sense to "set" a status object, so probably no
    * code needed in this function.
    */
   return ret;
}

CmsRet rcl_mocaStatsObject( _MocaStatsObject *newObj __attribute__((unused)),
                const _MocaStatsObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;
   /*
    * Doesn't make much sense to "set" a stats object, so probably no
    * code needed in this function.
    */
   return ret;
}

CmsRet rcl_mocaEthernetPmHistoryDataObject( _MocaEthernetPmHistoryDataObject *newObj,
                const _MocaEthernetPmHistoryDataObject *currObj,
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
        CmsRet found = omci_pm_findEntryById(MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA,
                                             newObj->managedEntityId);

        if (found == CMSRET_OBJECT_NOT_FOUND)
        {
            ret = omci_pm_createObject(MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA,
                                       newObj->managedEntityId,
                                       0,
                                       newObj->thresholdDataId);
        }
        else
        {
            omci_pm_setObject(MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA,
                              newObj->managedEntityId,
                              newObj->thresholdDataId);
        }
    }
    // Test for delete existing PM object.
    else if (DELETE_EXISTING(newObj, currObj))
    {
        omci_pm_deleteObject(MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA,
                             currObj->managedEntityId);
    }

    return ret;
}

CmsRet rcl_mocaInterfacePmHistoryDataObject( _MocaInterfacePmHistoryDataObject *newObj,
                const _MocaInterfacePmHistoryDataObject *currObj,
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
        CmsRet found = omci_pm_findEntryById(MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA,
                                             newObj->managedEntityId);

        if (found == CMSRET_OBJECT_NOT_FOUND)
        {
            ret = omci_pm_createObject(MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA,
                                       newObj->managedEntityId,
                                       0,
                                       newObj->thresholdDataId);
        }
        else
        {
            omci_pm_setObject(MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA,
                              newObj->managedEntityId,
                              newObj->thresholdDataId);
        }
    }
    // Test for delete existing PM object.
    else if (DELETE_EXISTING(newObj, currObj))
    {
        omci_pm_deleteObject(MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA,
                             currObj->managedEntityId);
    }

    return ret;
}

CmsRet rcl_mocaInterfacePmHistoryDataNodeTableObject( _MocaInterfacePmHistoryDataNodeTableObject *newObj __attribute__((unused)),
                const _MocaInterfacePmHistoryDataNodeTableObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_trafficManagementObject( _TrafficManagementObject *newObj __attribute__((unused)),
                const _TrafficManagementObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_priorityQueueGObject( _PriorityQueueGObject *newObj,
                const _PriorityQueueGObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT16 qid;
    UINT16 curSlotPortId;
    UINT16 newSlotPortId, newPriority;
    UBOOL8 isDirDs;
    PriorityQueueGObject pQInfo;

    if (SET_EXISTING(newObj, currObj))
    {
        newSlotPortId = GetRelatedPortSlotPort(newObj->relatedPort);
        curSlotPortId = GetRelatedPortSlotPort(currObj->relatedPort);

        isDirDs = IS_DS_QUEUE(newObj->managedEntityId);

        newPriority = GetRelatedPortPrio(newObj->relatedPort);
        ret = rutGpon_getQidFromPriorityQueueV2(newObj->managedEntityId,
            &qid, &pQInfo);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("rutGpon_getQidFromPriorityQueueV2() failed: "
                "meid=0x%04x\n", newObj->managedEntityId);
            return CMSRET_INTERNAL_ERROR;
        }

        omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
            "===> rcl_priorityQueueGObject, qid=%d, omcisp=0x%04x, prio=%d\n",
            qid, newSlotPortId, newPriority);

        if ((newSlotPortId != curSlotPortId) || (newPriority > OMCI_FILTER_PRIO_MAX))
        {
            cmsLog_error("Invalid parameters: "
                "qid=%d, cursp=0x%04x, newsp=0x%04x, prio=%d\n",
                qid, curSlotPortId, newSlotPortId, newPriority);
            return CMSRET_INVALID_PARAM_VALUE;
        }

        if ((newObj->trafficSchedulerGPointer != currObj->trafficSchedulerGPointer) &&
            (!isDirDs))
        {
            ret = rutGpon_configAllGemPortQosUs(newSlotPortId);
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("rutGpon_configAllGemPortQosUs() failed: "
                    "qid=%d, sp=0x%04x, prio=%d\n",
                    qid, newSlotPortId, newPriority);
                return CMSRET_INTERNAL_ERROR;
            }
        }

        if (newObj->dropPrecedenceColourMarking != currObj->dropPrecedenceColourMarking)
        {
            ret = rutGpon_configDropProcedence(isDirDs,
                newObj->dropPrecedenceColourMarking);
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("rutGpon_configDropProcedence() failed: "
                    "qid=%d, sp=0x%04x, prio=%d\n",
                    qid, newSlotPortId, newPriority);
                return CMSRET_INTERNAL_ERROR;
            }
        }

        if ((cmsUtl_strcmp(newObj->packetDropQueueThresholds,
            currObj->packetDropQueueThresholds) != 0) ||
            (newObj->weight != currObj->weight))
        {
            PktDropQThr_t pktDropQThr;
            char ifName[CMS_IFNAME_LENGTH] = {0};
            tmctl_ret_e tmctlRet = TMCTL_SUCCESS;
            tmctl_if_t tm_if;
            tmctl_devType_e dev;
            tmctl_queueDropAlg_t tmctl_dropAlgData;
            tmctl_queueCfg_t tmctl_queueCfg;

            omciTmctlPQParamConvert(isDirDs, newSlotPortId, &tm_if, ifName);
            dev = getGponDevType(isDirDs);
            memset(&pktDropQThr, 0x0, sizeof(PktDropQThr_t));
            pktDropQThrConvert(newObj->packetDropQueueThresholds, &pktDropQThr);

            if ((pktDropQThr.pktDQThrYMin == newObj->maxQueueSize) &&
              (pktDropQThr.pktDQThrYMax == newObj->maxQueueSize) &&
              (pktDropQThr.pktDQThrGMin == newObj->maxQueueSize) &&
              (pktDropQThr.pktDQThrGMax == newObj->maxQueueSize))
            {
                tmctl_dropAlgData.dropAlgorithm = TMCTL_DROP_DT;
                tmctlRet = _owapi_rut_tmctl_setQueueDropAlgExt(dev, &tm_if,
                  qid, &tmctl_dropAlgData);
            }
            else if (cmsUtl_strcmp(newObj->packetDropQueueThresholds,
                currObj->packetDropQueueThresholds) != 0)
            {
                tmctl_dropAlgData.dropAlgorithm = TMCTL_DROP_WRED;
                tmctl_dropAlgData.dropAlgLo.redMinThreshold = MIN(pktDropQThr.pktDQThrYMin,
                  newObj->allocatedQueueSize);
                tmctl_dropAlgData.dropAlgLo.redMaxThreshold = MIN(pktDropQThr.pktDQThrYMax,
                  newObj->allocatedQueueSize);
                tmctl_dropAlgData.dropAlgHi.redMinThreshold = MIN(pktDropQThr.pktDQThrGMin,
                  newObj->allocatedQueueSize);
                tmctl_dropAlgData.dropAlgHi.redMaxThreshold = MIN(pktDropQThr.pktDQThrGMax,
                  newObj->allocatedQueueSize);
                tmctl_dropAlgData.priorityMask0 = tmctl_dropAlgData.priorityMask1 = 0;
                tmctlRet = _owapi_rut_tmctl_setQueueDropAlgExt(dev, &tm_if,
                  qid, &tmctl_dropAlgData);
            }

            if (tmctlRet != TMCTL_SUCCESS)
            {
                cmsLog_error("tmctl_setQueueDropAlgExt() failed: "
                  "dev=%d, omcisp=0x%04x, qid=%d\n",
                  dev, newSlotPortId, qid);
                return CMSRET_INTERNAL_ERROR;
            }

            if (newObj->weight != currObj->weight)
            {
                tmctlRet = _owapi_rut_tmctl_getQueueCfg(dev, &tm_if, qid, &tmctl_queueCfg);
                if (tmctlRet != TMCTL_SUCCESS)
                {
                    cmsLog_error("tmctl_getQueueCfg failed: "
                        "dev=%d, omcisp=0x%04x, qid=%d\n",
                        dev, newSlotPortId, qid);
                    return CMSRET_INTERNAL_ERROR;
                }

                tmctl_queueCfg.weight = newObj->weight;

                /* Use prio from OMCI MIB because TMCtl does not save queue prio. */
                if (tmctl_queueCfg.schedMode == TMCTL_SCHED_SP)
                {
                    tmctl_queueCfg.priority = omciTmctlPrioConvert(newPriority);
                }
                tmctlRet = _owapi_rut_tmctl_setQueueCfg(dev, &tm_if, &tmctl_queueCfg);
                if (tmctlRet != TMCTL_SUCCESS)
                {
                    cmsLog_error("tmctl_setQueueCfg failed: "
                        "dev=%d, omcisp=0x%04x, qid=%d\n",
                        dev, newSlotPortId, qid);
                    return CMSRET_INTERNAL_ERROR;
                }
            }
        }
    }

    return ret;
}

CmsRet rcl_trafficSchedulerGObject( _TrafficSchedulerGObject *newObj,
                const _TrafficSchedulerGObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

    if (SET_EXISTING(newObj, currObj))
    {
        if (newObj->TContPointer != currObj->TContPointer)
            ret = CMSRET_INVALID_PARAM_VALUE;
        else if (newObj->policy != OMCI_SCHEDULE_POLICY_WRR)
            ret = CMSRET_INVALID_PARAM_VALUE;
    }

   return ret;
}

CmsRet rcl_gemTrafficDescriptorObject( _GemTrafficDescriptorObject *newObj,
                const _GemTrafficDescriptorObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   if (SET_EXISTING(newObj, currObj))
   {
      UINT32 currMax = 0;
      UINT32 newMax = 0;

      currMax = (currObj->PIR > currObj->CIR) ? currObj->PIR : currObj->CIR;
      newMax = (newObj->PIR > newObj->CIR) ? newObj->PIR : newObj->CIR;

      if(currMax != newMax)
      {
         rutGpon_trafficDescConfig(newObj, FALSE);
      }
   }
   else if (DELETE_EXISTING(newObj, currObj))
   {
      rutGpon_trafficDescConfig((GemTrafficDescriptorObject *)currObj, TRUE);
   }

   return ret;
}

CmsRet rcl_generalObject( _GeneralObject *newObj __attribute__((unused)),
                const _GeneralObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_uniGObject( _UniGObject *newObj,
                const _UniGObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;
   if (SET_EXISTING(newObj, currObj))
   {
      if(newObj->administrativeState != currObj->administrativeState)
      {
         //G.988->9.12.1 Administrative state: locks(1), unlocks(0)
         if(ME_ADMIN_STATE_LOCKED == newObj->administrativeState)
         {
            rutGpon_disableUniGAssociatedUni(newObj->managedEntityId);
            omci_pm_syncUniGAdminStates(newObj->managedEntityId, TRUE);
         }
         else if(ME_ADMIN_STATE_UNLOCKED == newObj->administrativeState)
         {
            rutGpon_enableUniGAssociatedUni(newObj->managedEntityId);
            omci_pm_syncUniGAdminStates(newObj->managedEntityId, FALSE);
         }
         else
         {
            return CMSRET_INVALID_PARAM_VALUE;
         }
      }
   }
   return ret;
}

CmsRet rcl_oltGObject( _OltGObject *newObj,
                const _OltGObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   if (SET_EXISTING(newObj, currObj))
    {
        int retVal = 0;
        UINT8 *bin = NULL;
        UINT32 binSize = 0;
        BCM_Ploam_TimeOfDayInfo info;

        memset(&info, 0, sizeof(BCM_Ploam_TimeOfDayInfo));

        retVal = gponCtl_getTodInfo(&info);
        if (retVal == 0)
        {
            // timeInfo:
            //    - 1st 4 bytes is the sequence number of the specified GEM superframe
            //    - last 10 bytes is TstampN:
            //            * 1st 6 bytes is seconds
            //            * last 4 bytes is nano seconds

            cmsUtl_hexStringToBinaryBuf(newObj->timeInfo, &bin, &binSize);

            memcpy(&info.superframe, &bin[0], 4);
            memcpy(&info.tStampN.secondsMSB, &bin[4], 2);
            memcpy(&info.tStampN.secondsLSB, &bin[6], 4);
            memcpy(&info.tStampN.nanoSeconds, &bin[10], 4);
            cmsMem_free(bin);

            info.superframe = OMCI_NTOHL(&info.superframe);
            info.tStampN.secondsMSB = OMCI_NTOHS(&info.tStampN.secondsMSB);
            info.tStampN.secondsLSB = OMCI_NTOHL(&info.tStampN.secondsLSB);
            info.tStampN.nanoSeconds = OMCI_NTOHL(&info.tStampN.nanoSeconds);

            info.pulseWidth = 10;
            info.enable = TRUE;
            info.enableUsrEvent = TRUE;

            retVal = gponCtl_setTodInfo(&info);
            if (retVal == 0)
                ret = CMSRET_SUCCESS;
            else
                cmsLog_error("Cannot gponCtl_setTodInfo: retVal=%d", retVal);
        }
        else
        {
            cmsLog_error("Cannot gponCtl_getTodInfo: retVal=%d", retVal);
        }
    }

   return ret;
}

CmsRet rcl_networkAddressObject(_NetworkAddressObject *newObj __attribute__((unused)),
                const _NetworkAddressObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;

  return cmsResult;
}

CmsRet rcl_authenticationSecurityMethodObject(_AuthenticationSecurityMethodObject *newObj __attribute__((unused)),
                const _AuthenticationSecurityMethodObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;

  return cmsResult;
}

CmsRet rcl_onuRemoteDebugObject(_OnuRemoteDebugObject *newObj,
  const _OnuRemoteDebugObject *currObj __attribute__((unused)),
  const InstanceIdStack *iidStack __attribute__((unused)),
  char **errorParam __attribute__((unused)),
  CmsRet *errorCode __attribute__((unused)))
{
    CmsRet cmsResult = CMSRET_SUCCESS;
    UINT8 *buf = NULL;
    UINT32 bufSize = 0;
    char cmd[OMCI_ENTRY_SIZE_25 + 1];

    if (SET_EXISTING(newObj, currObj))
    {
        cmsResult = cmsUtl_hexStringToBinaryBuf((char*)newObj->command,
          &buf, &bufSize);
        if (cmsResult == CMSRET_SUCCESS)
        {
            memcpy(cmd, buf, OMCI_ENTRY_SIZE_25);
            cmd[OMCI_ENTRY_SIZE_25] = '\0';
            debugCmdReqNotify((char*)cmd);
        }

        CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
    }

    return cmsResult;
}

CmsRet rcl_largeStringObject(_LargeStringObject *newObj __attribute__((unused)),
                const _LargeStringObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
  CmsRet cmsResult = CMSRET_SUCCESS;

  return cmsResult;
}


CmsRet rcl_thresholdData1Object( _ThresholdData1Object *newObj,
                const _ThresholdData1Object *currObj,
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
        UBOOL8 existed = omci_th_isObjectExisted(MDMOID_THRESHOLD_DATA1,
                                                 newObj->managedEntityId);

        if (existed == FALSE)
        {
            omci_th_createObject(MDMOID_THRESHOLD_DATA1,
                                 newObj->managedEntityId,
                                 (void *)newObj);
        }
        else
        {
            omci_th_setObject(MDMOID_THRESHOLD_DATA1,
                              newObj->managedEntityId,
                              (void *)newObj);
        }
    }
    // Test for delete existing PM object.
    if (DELETE_EXISTING(newObj, currObj))
    {
        omci_th_deleteObject(MDMOID_THRESHOLD_DATA1,
                             currObj->managedEntityId);
    }

    return ret;
}


CmsRet rcl_thresholdData2Object( _ThresholdData2Object *newObj,
                const _ThresholdData2Object *currObj,
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
        UBOOL8 existed = omci_th_isObjectExisted(MDMOID_THRESHOLD_DATA2,
                                                 newObj->managedEntityId);

        if (existed == FALSE)
        {
            omci_th_createObject(MDMOID_THRESHOLD_DATA2,
                                 newObj->managedEntityId,
                                 (void *)newObj);
        }
        else
        {
            omci_th_setObject(MDMOID_THRESHOLD_DATA2,
                              newObj->managedEntityId,
                              (void *)newObj);
        }
    }
    // Test for delete existing PM object.
    if (DELETE_EXISTING(newObj, currObj))
    {
        omci_th_deleteObject(MDMOID_THRESHOLD_DATA2,
                             currObj->managedEntityId);
    }

    return ret;
}

CmsRet rcl_omciObject( _OmciObject *newObj __attribute__((unused)),
                const _OmciObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_debugObject( _DebugObject *newObj __attribute__((unused)),
                const _DebugObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet rcl_gponOmciStatsObject( _GponOmciStatsObject *newObj __attribute__((unused)),
                const _GponOmciStatsObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_omciSystemObject( _OmciSystemObject *newObj __attribute__((unused)),
                const _OmciSystemObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_tr069ManagementServerObject( _Tr069ManagementServerObject *newObj __attribute__((unused)),
                const _Tr069ManagementServerObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;

    return ret;
}

// The hexstring in MDM is stored in network order. The byte order conversion
// may be needed after cmsUtl_hexStringToBinaryBuf(), it is case-by-case based.
static CmsRet pktDropQThrConvert(const char *thrHexStr, PktDropQThr_t *pktDropQThr)
{
    UINT32 size = 0;
    UINT8 *buf = NULL;
    CmsRet ret;

    /* Convert thresholds hexString (16 bytes) to a 8-byte buffer. */
    ret = cmsUtl_hexStringToBinaryBuf(thrHexStr, &buf, &size);
    if (ret == CMSRET_SUCCESS)
    {
        pktDropQThr->pktDQThrYMin = ((buf[4] << 8) | buf[5]);
        pktDropQThr->pktDQThrYMax = ((buf[6] << 8) | buf[7]);
        pktDropQThr->pktDQThrGMin = ((buf[0] << 8) | buf[1]);
        pktDropQThr->pktDQThrGMax = ((buf[2] << 8) | buf[3]);
        /* Free temporary memory. */
        cmsMem_free(buf);
    }
    else
    {
        cmsLog_error("cmsUtl_hexStringToBinaryBuf() failed, ret=%d\n", ret);
    }

    return ret;
}

#ifndef G9991
static UINT32 getLinkedEthPort(UINT16 slotPortId)
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 portIdx = 0;
    UINT32 portFound = 0;
    OmciEthPortType_t eth;
    OmciEthPortType portType;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *obj = NULL;
    UINT32 firstMeId = 0;

    if ((rutGpon_isVeipPptpUni(MDMOID_PPTP_ETHERNET_UNI, slotPortId) == TRUE)
      || (rutGpon_getVeipId() == slotPortId))
    {
        if ((ret = _cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, 0, (void*)&obj))
          == CMSRET_SUCCESS)
        {
            eth.types.all = obj->ethernetTypes;
            for (portIdx = 0; portIdx < obj->numberOfEthernetPorts; portIdx++)
            {
                portType = omciUtil_getPortType(portIdx, eth.types.all);
                if (portType == OMCI_ETH_PORT_TYPE_RG)
                {
                    portFound = portIdx;
                    break;
                }
            }
            _cmsObj_free((void**)&obj);
        }
    }
    else
    {
        rutGpon_getFirstEthernetMeId(&firstMeId);
        portFound = slotPortId - firstMeId;
    }

    return portFound;
}
#endif

static void omciTmctlPQParamConvert(UBOOL8 isDirDs, UINT16 slotPortId,
  tmctl_if_t *tmIf, char *ifName)
{
#ifndef G9991
    UINT32 portNum;
#endif

    if (isDirDs)
    {
#ifdef G9991
        sprintf(ifName, "sid%d", (slotPortId - GPON_FIRST_ETH_MEID));
#else
        portNum = getLinkedEthPort(slotPortId);
        sprintf(ifName, "eth%d", portNum);
#endif
        tmIf->ethIf.ifname = &ifName[0];
    }
    else
    {
        tmIf->gponIf.tcontid = slotPortId - GPON_FIRST_TCONT_MEID;
    }
}

#endif // DMP_X_ITU_ORG_GPON_1

