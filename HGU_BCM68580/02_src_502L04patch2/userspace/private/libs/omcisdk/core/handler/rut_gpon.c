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
#include "mdmlite_api.h"
#include "me_handlers.h"
#include "ctlutils_api.h"    // for gponCtl_getGemPort() function
#include "owrut_api.h"
#include "omci_pm.h"

/* Run-time OMCI control data/status. */
static FILE *fsDebug = NULL;
static UBOOL8 omciMibResetState = FALSE;
static omciDebug_t omciDebugSetting;


//=======================  Private GPON functions ========================

UBOOL8 rutGpon_isObjectExisted(UINT32 oid, UINT32 managedEntityId)
{
    UBOOL8 found = FALSE;
    void   *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    /* search instance that has id matched with the given id */
    while ((!found) &&
           (_cmsObj_getNextFlags(oid,
                                &iidStack,
                                OGF_NO_VALUE_UPDATE,
                                (void **) &obj) == CMSRET_SUCCESS))
    {
        // MacBridgePortBridgeTableDataObject is used as generic type
        // since it only has managedEntityId as its parameter
        found = (((MacBridgePortBridgeTableDataObject *)obj)->managedEntityId == managedEntityId);
        _cmsObj_free((void **) &obj);
    }

    return found;
}

CmsRet gponcDrvErrcodeConvert(int retGponDriver)
{
    CmsRet ret = CMSRET_SUCCESS;

    switch (retGponDriver)
    {
        case 0:
            ret = CMSRET_SUCCESS;
            break;
        case EINVAL_PLOAM_GEM_PORT:
            cmsLog_error("GEM Port ID not configured");
            ret = CMSRET_OBJECT_NOT_FOUND;
            break;
        case EINVAL_PLOAM_ARG:
        default:
            cmsLog_error("The input parameter is out of range, ret=%d\n", retGponDriver);
            ret = CMSRET_INVALID_ARGUMENTS;
    }

    return ret;
}

void rutGpon_closeDebugFile(void)
{
    if (fsDebug != NULL)
    {
        fclose(fsDebug);
        fsDebug = NULL;
    }
}

CmsRet rutGpon_openDebugFile(void)
{
    CmsRet ret = CMSRET_SUCCESS;

    rutGpon_closeDebugFile();
    unlink(OMCI_DEBUG_FILE_NAME);

    // if /var/omci/ directory does not exist then create it
    if (access(OMCI_DIRECTORY,  F_OK) !=  0)
    {
        mkdir(OMCI_DIRECTORY, 0777);
    }

    if ((fsDebug = fopen(OMCI_DEBUG_FILE_NAME, "w")) == NULL)
    {
        cmsLog_error("Failed to open %s for write", OMCI_DEBUG_FILE_NAME);
        ret = CMSRET_OPEN_FILE_ERROR;
    }

    return ret;
}

CmsRet get_obj_by_instance(MdmObjectId oid, void **pObj, UINT32 meInst, UINT32 present)
{
    void *obj;
    UINT32 objMeInst;
    UINT32 meInstCount;
    _MdmObjParamInfo paramInfo;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    *pObj = NULL;

    if (present)
    {
        if ((ret = _cmsObj_get(oid, &iidStack, 0, &obj)) != CMSRET_SUCCESS)
        {
            cmsLog_error("get of object id %d failed, ret=%d", oid, ret);
        }
        else
        {
            if((ret = _cmsObj_getNthParam(obj, 0, &paramInfo)) != CMSRET_SUCCESS)
            {
                cmsLog_error("get of parameter %d from object id %d failed, ret=%d",
                             0, oid, ret);
                _cmsObj_free(&obj);
            }
            else
            {
                objMeInst = *(UINT32 *)(paramInfo.val);
                if (objMeInst == meInst)
                {
                    *pObj = obj;
                }
                else
                {
                    _cmsObj_free(&obj);
                    ret = CMSRET_OBJECT_NOT_FOUND;
                }
            }
        }
    }
    else
    {
        meInstCount = 0;
        while (1)
        {
            ret = _cmsObj_getNextFlags(oid, &iidStack, OGF_NO_VALUE_UPDATE, &obj);
            if (ret == CMSRET_NO_MORE_INSTANCES)
            {
                /* no more instances available */
                cmsLog_error("Unknown Instance %d of object Id %d\n", meInst, oid);
                break;
            }
            else if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("getNext %d of object id %d failed, ret=%d",
                             meInstCount, oid, ret);
                break;
            }
            else
            {
                if((ret = _cmsObj_getNthParam(obj, 0, &paramInfo)) != CMSRET_SUCCESS)
                {
                    cmsLog_error("get of parameter %d from object id %d failed, ret=%d",
                                 0, oid, ret);
                    _cmsObj_free(&obj);
                    break;
                }

                objMeInst = *(UINT32 *)(paramInfo.val);

                /* get instance that matches ME Id */
                if (objMeInst == meInst)
                {
                    *pObj = obj;
                    break;
                }
                else
                {
                    _cmsObj_free(&obj);
                }
            }

            meInstCount++;
        }
    }

    return ret;
}

/*
 * helper functions
 */
static CmsRet configGemPortQosUs
    (const BCM_Ploam_EnableGemPortInfo *pInfo __attribute__((unused)),
     const GemPortNetworkCtpObject *ctp)
{
    UINT16 qid = 0;
    UINT16 omciPrio = 0, weight = 0;
    OmciSchedulePolicy policy = OMCI_SCHEDULE_POLICY_NONE;
    CmsRet ret = CMSRET_SUCCESS;
    PriorityQueueGObject pQInfo;
    tmctl_if_t tm_if;
    tmctl_queueCfg_t tmctl_queueCfg;

    if (getOnuOmciTmOption() == OMCI_TRAFFIC_MANAGEMENT_RATE)
    {
        cmsLog_notice("For rate controlled ONU, OMCI GEM port does not"
          "include PQ info.\n");
        return CMSRET_SUCCESS;
    }

    ret = rutGpon_getTContPolicyFromTContMeId(ctp->TContPointer, &policy);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("rutGpon_getTContPolicyFromTContMeId() failed, "
          "OMCI GEM Port ME %d", ctp->managedEntityId);
        return CMSRET_INVALID_ARGUMENTS;
    }

    rutGpon_getQidFromPriorityQueueV2(ctp->upstreamTrafficManagementPointer,
      &qid, &pQInfo);
    if (qid >= GPON_PHY_US_PQ_MAX)
    {
        cmsLog_error("OMCI GEM Port ME %d invalid qid=%d\n",
          ctp->managedEntityId, qid);
        return CMSRET_INTERNAL_ERROR;
    }

    memset(&tm_if, 0, sizeof(tm_if));
    memset(&tmctl_queueCfg, 0, sizeof(tmctl_queueCfg));
    tm_if.gponIf.tcontid = ctp->TContPointer - GPON_FIRST_TCONT_MEID;
    if (_owapi_rut_tmctl_getQueueCfg(TMCTL_DEV_GPON, &tm_if, qid,
      &tmctl_queueCfg) != TMCTL_SUCCESS)
    {
        cmsLog_error("tmctl_getQueueCfg failed, tcont=%d, qid=%d\n",
          tm_if.gponIf.tcontid, qid);
        return CMSRET_INTERNAL_ERROR;
    }

    if (policy == OMCI_SCHEDULE_POLICY_HOL)
    {
        if (rutGpon_getPrioFromPriorityQueue
          (&pQInfo, OMCI_FLOW_UPSTREAM, &omciPrio) != CMSRET_SUCCESS)
        {
            omciPrio = 0;
            cmsLog_error("rutGpon_getPrioFromPriorityQueue() failed, "
              "OMCI GEM Port ME %d, qid=%d, use prio 0.\n",
              ctp->managedEntityId, qid);
        }
        tmctl_queueCfg.schedMode = TMCTL_SCHED_SP;
        tmctl_queueCfg.priority = omciTmctlPrioConvert(omciPrio);
    }
    else if (policy == OMCI_SCHEDULE_POLICY_WRR)
    {
        if (rutGpon_getWeightFromPriorityQueue
          (&pQInfo, OMCI_FLOW_UPSTREAM, &weight) != CMSRET_SUCCESS)
        {
            cmsLog_error("rutGpon_getWeightFromPriorityQueue() failed, "
             "OMCI GEM Port ME %d, qid=%d, use weight 1\n",
             ctp->managedEntityId, qid);
            /* WRR case: weight can not be 0. */
            weight = 1;
        }

        tmctl_queueCfg.schedMode = TMCTL_SCHED_WRR;
        tmctl_queueCfg.weight = weight;
        /* WRR case: set priority value to 0. */
        tmctl_queueCfg.priority = 0;
    }

    if (_owapi_rut_tmctl_setQueueCfg(TMCTL_DEV_GPON, &tm_if,
      &tmctl_queueCfg) != TMCTL_SUCCESS)
    {
        cmsLog_error("tmctl_setQueueCfg failed, tcont=%d, qid=%d\n",
          tm_if.gponIf.tcontid, qid);
        return CMSRET_INTERNAL_ERROR;
    }

    return ret;
}

static CmsRet configGemPortShapingUs
    (const BCM_Ploam_EnableGemPortInfo *pInfo __attribute__((unused)),
     const GemPortNetworkCtpObject *ctp,
     const UBOOL8 reset)
{
    UINT16 qid = 0;
    PriorityQueueGObject pQInfo;
    UBOOL8 foundTrafficDesc = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    GemTrafficDescriptorObject *trafficDesc = NULL;
    CmsRet ret = CMSRET_SUCCESS;
    tmctl_if_t tm_if;
    tmctl_queueCfg_t tmctl_queueCfg;
    UINT16 omciPrio = 0;

    if (getOnuOmciTmOption() == OMCI_TRAFFIC_MANAGEMENT_RATE)
    {
        cmsLog_notice("For rate controlled ONU, OMCI GEM port does not"
          "include PQ info.\n");
        return CMSRET_SUCCESS;
    }

    rutGpon_getQidFromPriorityQueueV2(ctp->upstreamTrafficManagementPointer,
      &qid, &pQInfo);
    if (qid >= GPON_PHY_US_PQ_MAX)
    {
        cmsLog_error("OMCI GEM Port ME %d invalid qid=%d\n",
          ctp->managedEntityId, qid);
        return ret;
    }

    // search TrafficDescriptorObject
    while ((!foundTrafficDesc) &&
           (_cmsObj_getNext(MDMOID_GEM_TRAFFIC_DESCRIPTOR,
                           &iidStack,
                           (void**)&trafficDesc) == CMSRET_SUCCESS))
    {
        foundTrafficDesc = (trafficDesc->managedEntityId == \
          ctp->upstreamTrafficDescriptorProfilePointer);
        if (foundTrafficDesc == TRUE)
        {
            // set CIR = PIR when CIR is 0 to allow transmit up to PIR
            if (trafficDesc->CIR == 0 && trafficDesc->PIR > 0)
            {
                trafficDesc->CIR = trafficDesc->PIR;
            }

            memset(&tm_if, 0, sizeof(tm_if));
            memset(&tmctl_queueCfg, 0, sizeof(tmctl_queueCfg));
            tm_if.gponIf.tcontid = ctp->TContPointer - GPON_FIRST_TCONT_MEID;
            if (_owapi_rut_tmctl_getQueueCfg(TMCTL_DEV_GPON, &tm_if, qid,
              &tmctl_queueCfg) != TMCTL_SUCCESS)
            {
                cmsLog_error("tmctl_getQueueCfg failed, tcont=%d, qid=%d\n",
                  tm_if.gponIf.tcontid, qid);
                goto out0;
            }

            if (rutGpon_getPrioFromPriorityQueue
              (&pQInfo, OMCI_FLOW_UPSTREAM, &omciPrio) != CMSRET_SUCCESS )
            {
                omciPrio = 0;
                cmsLog_error("rutGpon_getPrioFromPriorityQueue failed, use 0 as default\n");
            }
            tmctl_queueCfg.priority = omciTmctlPrioConvert(omciPrio);
            if (!reset)
            {
                tmctl_queueCfg.shaper.shapingRate = trafficDesc->CIR * 8 / 1000;
                tmctl_queueCfg.shaper.shapingBurstSize = trafficDesc->CBS;
            }
            else
            {
                /* Shaping rate in kbps. 0 indicates no shaping. */
                tmctl_queueCfg.shaper.shapingRate = 0;
                tmctl_queueCfg.shaper.shapingBurstSize = 0;
            }

            if (_owapi_rut_tmctl_setQueueCfg(TMCTL_DEV_GPON, &tm_if,
              &tmctl_queueCfg) != TMCTL_SUCCESS)
            {
                cmsLog_error("tmctl_setQueueCfg failed, tcont=%d, qid=%d\n",
                  tm_if.gponIf.tcontid, qid);
                ret = CMSRET_INTERNAL_ERROR;
            }
        }
out0:
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
          "===> configGemPortShapingUs, gemPortIndex=%d, "
          "PIR=%d, CIR=%d, PBS=%d, CBS=%d, reset=%d, ret=%d\n",
          pInfo->gemPortIndex, trafficDesc->PIR, trafficDesc->CIR,
          trafficDesc->PBS, trafficDesc->CBS, reset, ret);
        _cmsObj_free((void**) &trafficDesc);
    }

    return ret;
}

static CmsRet configPptpEthShapingDs
    (const UINT32 portNum,
     const MacBridgePortConfigDataObject *macBrPortCfg,
     const UBOOL8 reset)
{
    UBOOL8 foundTrafficDesc = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    GemTrafficDescriptorObject *trafficDesc = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    // search TrafficDescriptorObject
    while ((!foundTrafficDesc) &&
           (_cmsObj_getNext(MDMOID_GEM_TRAFFIC_DESCRIPTOR,
                           &iidStack,
                           (void **) &trafficDesc) == CMSRET_SUCCESS))
    {
        foundTrafficDesc = (trafficDesc->managedEntityId == macBrPortCfg->outTdPointer);
        // if TrafficDescriptorObject is found then
        if (foundTrafficDesc == TRUE)
        {
           omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> configPptpEthShapingDs, portNum =%d, macBridgeMeId=%d, PIR=%d, CIR=%d, PBS=%d, CBS=%d, reset=%d\n",
                   portNum, macBrPortCfg->managedEntityId, trafficDesc->PIR, trafficDesc->CIR,
                   trafficDesc->PBS, trafficDesc->CBS, reset);

           if(reset != TRUE)
           {
              UINT32 maxRate = 0;
              UINT32 maxBlockSize = 0;
              UINT32 kbits_sec = 0;
              UINT32 kbits_burst = 0;

              /*select the bigger value of PIR/CIR as the max rate*/
              maxRate = (trafficDesc->PIR > trafficDesc->CIR) ? trafficDesc->PIR : trafficDesc->CIR;
              if(trafficDesc->PIR > trafficDesc->CIR)
              {
                 maxBlockSize = trafficDesc->PBS;
              }
              else
              {
                 maxBlockSize = trafficDesc->CBS;
              }
              kbits_sec = maxRate * 8 / 1024;
              kbits_burst = maxBlockSize * 8 / 1024;

              if(0 != bcm_port_rate_egress_set(0, bcm_enet_map_oam_idx_to_phys_port(portNum), kbits_sec, kbits_burst))
              {
                 ret = CMSRET_INVALID_PARAM_VALUE;
                 cmsLog_error("set pptpEth Egress Rate Limit Failed, portNum = %d, rate = %d burst = %d\n", portNum, kbits_sec, kbits_burst);
              }
           }
           else
           {
              /*Clear the current rate limit configuration*/
              if(0 != bcm_port_rate_egress_set(0, bcm_enet_map_oam_idx_to_phys_port(portNum), 0, 0))
              {
                 ret = CMSRET_INVALID_PARAM_VALUE;
                 cmsLog_error("reset pptpEth Egress Rate Limit Failed, portNum = %d\n", portNum);
              }
           }
        }
        _cmsObj_free((void **) &trafficDesc);
    }

    return ret;
}

static CmsRet configPptpEthShapingUs
    (const UINT32 portNum,
     const MacBridgePortConfigDataObject *macBrPortCfg,
     const UBOOL8 reset)
{
    UBOOL8 foundTrafficDesc = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    GemTrafficDescriptorObject *trafficDesc = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    // search TrafficDescriptorObject
    while ((!foundTrafficDesc) &&
           (_cmsObj_getNext(MDMOID_GEM_TRAFFIC_DESCRIPTOR,
                           &iidStack,
                           (void **) &trafficDesc) == CMSRET_SUCCESS))
    {
        foundTrafficDesc = (trafficDesc->managedEntityId == macBrPortCfg->inTdPointer);
        // if TrafficDescriptorObject is found then
        if (foundTrafficDesc == TRUE)
        {

           omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> configPptpEthShapingUs, portNum =%d, macBridgeMeId=%d, PIR=%d, CIR=%d, PBS=%d, CBS=%d, reset=%d\n",
                   portNum, macBrPortCfg->managedEntityId, trafficDesc->PIR, trafficDesc->CIR,
                   trafficDesc->PBS, trafficDesc->CBS, reset);

           if(reset != TRUE)
           {
              UINT32 maxRate = 0;
              UINT32 maxBlockSize = 0;
              UINT32 kbits_sec = 0;
              UINT32 kbits_burst = 0;

              /*select the bigger value of PIR/CIR as the max rate*/
              maxRate = (trafficDesc->PIR > trafficDesc->CIR) ? trafficDesc->PIR : trafficDesc->CIR;
              if(trafficDesc->PIR > trafficDesc->CIR)
              {
                 maxBlockSize = trafficDesc->PBS;
              }
              else
              {
                 maxBlockSize = trafficDesc->CBS;
              }

              kbits_sec = maxRate * 8 / 1024;
              kbits_burst = maxBlockSize * 8 / 1024;
              if(0 != bcm_port_rate_ingress_set(0, bcm_enet_map_oam_idx_to_phys_port(portNum), kbits_sec, kbits_burst))
              {
                 ret = CMSRET_INVALID_PARAM_VALUE;
                 cmsLog_error("set pptpEth Ingress Rate Limit Failed, portNum = %d, rate = %d burst = %d\n", portNum, kbits_sec, kbits_burst);
              }
           }
           else
           {
              /*Clear the current rate limit configuration*/
              if(0 != bcm_port_rate_ingress_set(0, bcm_enet_map_oam_idx_to_phys_port(portNum), 0, 0))
              {
                 ret = CMSRET_INVALID_PARAM_VALUE;
                 cmsLog_error("reset pptpEth Ingress Rate Limit Failed, portNum = %d\n", portNum);
              }
           }
        }
        _cmsObj_free((void **) &trafficDesc);
    }

    return ret;
}

static CmsRet configPptpEthLearningInd (const UINT32 portNum, const MacBridgePortConfigDataObject *macBrPortCfg,  const UBOOL8 reset)
{
    UBOOL8 foundBridge = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MacBridgeServiceProfileObject *bridge= NULL;
    CmsRet ret = CMSRET_SUCCESS;

    if (reset)
    {
        if (0 != bcm_port_learning_ind_set(0, bcm_enet_map_oam_idx_to_phys_port(portNum), TRUE))
        {
            ret = CMSRET_INVALID_PARAM_VALUE;
            cmsLog_error("reset pptpEth Ingress Rate Limit Failed, portNum = %d\n", portNum);
        }
        return ret;
    }

    // search bridge object
    while ((!foundBridge) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_SERVICE_PROFILE,
                                &iidStack,
                                OGF_NO_VALUE_UPDATE,
                                (void **) &bridge) == CMSRET_SUCCESS))
    {
        foundBridge = (bridge->managedEntityId == macBrPortCfg->bridgeId);

        // if bridge is found then
        if (foundBridge == TRUE)
        {
           omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> configPptpEthLearningInd, portNum =%d, macBridgeMeId=%d,BridgeId=%d, reset=%d\n",
                   portNum, macBrPortCfg->managedEntityId, bridge->managedEntityId, reset);
            if (0 != bcm_port_learning_ind_set(0, bcm_enet_map_oam_idx_to_phys_port(portNum), bridge->learning))
            {
                ret = CMSRET_INVALID_PARAM_VALUE;
                cmsLog_error("set pptpEth sal dal Failed, portNum = %d, enable = %d\n", portNum, bridge->learning);
            }
        }

        _cmsObj_free((void **) &bridge);
    }

    return ret;
}

CmsRet rutGpon_configPptpEthShapingDs(const MacBridgePortConfigDataObject *macBrPortCfg, UBOOL8 reset)
{
    UBOOL8 foundPptpUni = FALSE;
    PptpEthernetUniObject *pptpEthernet = NULL;
    InstanceIdStack iidEth = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    while ((!foundPptpUni) &&
           (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                                &iidEth,
                                OGF_NO_VALUE_UPDATE,
                                (void **) &pptpEthernet) == CMSRET_SUCCESS))
    {
        if (rutGpon_isVeipPptpUni(MDMOID_PPTP_ETHERNET_UNI, pptpEthernet->managedEntityId) == FALSE)
        {
            foundPptpUni = (pptpEthernet->managedEntityId == macBrPortCfg->tpPointer);
        }

        if (foundPptpUni)
        {
            UINT32 portNum = 0;
            portNum = PEEK_INSTANCE_ID(&iidEth);
            portNum = portNum - 1;

            ret = configPptpEthShapingDs(portNum, macBrPortCfg, reset);
        }

        _cmsObj_free((void **) &pptpEthernet);
    }

    return ret;
}

CmsRet rutGpon_configPptpEthShapingUs(const MacBridgePortConfigDataObject *macBrPortCfg, UBOOL8 reset)
{
    UBOOL8 foundPptpUni = FALSE;
    PptpEthernetUniObject *pptpEthernet = NULL;
    InstanceIdStack iidEth = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    while ((!foundPptpUni) &&
           (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                                &iidEth,
                                OGF_NO_VALUE_UPDATE,
                                (void **) &pptpEthernet) == CMSRET_SUCCESS))
    {
        if (rutGpon_isVeipPptpUni(MDMOID_PPTP_ETHERNET_UNI, pptpEthernet->managedEntityId) == FALSE)
        {
            foundPptpUni = (pptpEthernet->managedEntityId == macBrPortCfg->tpPointer);
        }

        if (foundPptpUni)
        {
            UINT32 portNum = 0;
            portNum = PEEK_INSTANCE_ID(&iidEth);
            portNum = portNum - 1;

            ret = configPptpEthShapingUs(portNum, macBrPortCfg, reset);
        }

        _cmsObj_free((void **) &pptpEthernet);
    }

    return ret;
}

static void rutGpon_setWanLearningInd(UBOOL8 val)
{
    if (omciDm_getWanLearningMode() != val)
    {
        if (0 != bcm_port_learning_ind_set(1,
          bcm_enet_map_oam_idx_to_phys_port(0), val))
        {
            cmsLog_error("set wan learning mode %d failed\n", val);
            return;
        }
        omciDm_setWanLearningMode(val);
    }
}

CmsRet rutGpon_configPortLearningInd(const MacBridgePortConfigDataObject *macBrPortCfg, UBOOL8 reset)
{
    UBOOL8 foundBridge = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MacBridgeServiceProfileObject *bridge = NULL;
    CmsRet ret = CMSRET_SUCCESS;
    UBOOL8 foundPptpUni = FALSE;
    PptpEthernetUniObject *pptpEthernet = NULL;
    UINT32 firstPptpEthernetUniMeId;
    InstanceIdStack iidEth = EMPTY_INSTANCE_ID_STACK;

    if (macBrPortCfg->tpType == OMCI_BP_TP_PPTP_ETH)
    {
        while ((!foundPptpUni) &&
                (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                               &iidEth,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &pptpEthernet) == CMSRET_SUCCESS))
        {
            if (rutGpon_isVeipPptpUni(MDMOID_PPTP_ETHERNET_UNI, pptpEthernet->managedEntityId) == FALSE)
            {
                foundPptpUni = (pptpEthernet->managedEntityId == macBrPortCfg->tpPointer);
            }
            if (foundPptpUni)
            {
                UINT32 portNum = 0;
                ret = rutGpon_getFirstEthernetMeId(&firstPptpEthernetUniMeId);
                if (ret != CMSRET_SUCCESS)
                {
                    cmsLog_error("Cannot get first Ethernet ID, ret=%d", ret);
                    _cmsObj_free((void**)&pptpEthernet);
                    goto out;
                }
                portNum = pptpEthernet->managedEntityId - firstPptpEthernetUniMeId;
                ret = configPptpEthLearningInd(portNum, macBrPortCfg, reset);
            }
            _cmsObj_free((void **) &pptpEthernet);
        }
    }
    else if (macBrPortCfg->tpType == OMCI_BP_TP_GEM_INTERWORKING ||macBrPortCfg->tpType == OMCI_BP_TP_MAPPER_SERVICE )
    {
        if (reset)
        {
            rutGpon_setWanLearningInd(GPON_WAN_LEARNING_MODE_DEFAULT);
            return CMSRET_SUCCESS;
        }

        // search bridge object
        while ((!foundBridge) &&
              (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_SERVICE_PROFILE,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &bridge) == CMSRET_SUCCESS))
        {
            foundBridge = (bridge->managedEntityId == macBrPortCfg->bridgeId);
            // if bridge is found then
            if (foundBridge == TRUE)
            {
                omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                               "===> configWanLearningInd, portMeId=%d, bridgeMeId=%d, val=%d\n",
                               macBrPortCfg->managedEntityId,
                               bridge->managedEntityId,
                               bridge->learning);
                rutGpon_setWanLearningInd(bridge->learning);
            }
            _cmsObj_free((void **) &bridge);
        }
    }
out:
    return ret;
}


CmsRet rutGpon_configPortLearningIndVal(const MacBridgePortConfigDataObject *macBrPortCfg, UBOOL8 val)
{
    UBOOL8 foundPptpUni = FALSE;
    PptpEthernetUniObject *pptpEthernet = NULL;
    UINT32 firstPptpEthernetUniMeId;
    InstanceIdStack iidEth = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    if (macBrPortCfg->tpType == OMCI_BP_TP_PPTP_ETH )
    {
        while ((!foundPptpUni) && (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
          &iidEth, OGF_NO_VALUE_UPDATE, (void**)&pptpEthernet) == CMSRET_SUCCESS))
        {
            foundPptpUni = (pptpEthernet->managedEntityId == macBrPortCfg->tpPointer);
            if (foundPptpUni)
            {
                UINT32 portNum = 0;
                ret = rutGpon_getFirstEthernetMeId(&firstPptpEthernetUniMeId);
                if (ret != CMSRET_SUCCESS)
                {
                    cmsLog_error("Cannot get first Ethernet ID, ret=%d", ret);
                    _cmsObj_free((void**)&pptpEthernet);
                    goto out;
                }
                portNum = pptpEthernet->managedEntityId - firstPptpEthernetUniMeId;
                ret = bcm_port_learning_ind_set(0,
                  bcm_enet_map_oam_idx_to_phys_port(portNum), val);
            }
            _cmsObj_free((void **) &pptpEthernet);
        }
    }
    else 
    {
        if ((macBrPortCfg->tpType == OMCI_BP_TP_GEM_INTERWORKING) ||
          (macBrPortCfg->tpType == OMCI_BP_TP_MAPPER_SERVICE))
        {
            rutGpon_setWanLearningInd(val);
        }
    }
out:
   return ret;
}

CmsRet rutGpon_trafficDescConfig(GemTrafficDescriptorObject * trafficDescCfg, UBOOL8 reset)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    GemPortNetworkCtpObject *ctp = NULL;
    MacBridgePortConfigDataObject *port = NULL;
    CmsRet ret = CMSRET_SUCCESS;
    int retGponDriver = 0;

    /*Firstly : Check the gemPortCtp*/
    while (_cmsObj_getNext(MDMOID_GEM_PORT_NETWORK_CTP,
                          &iidStack,
                          (void **) &ctp) == CMSRET_SUCCESS)
    {
       UBOOL8 found = FALSE;

       found = (ctp->upstreamTrafficDescriptorProfilePointer == trafficDescCfg->managedEntityId);
       if(found)
       {
          BCM_Ploam_GemPortInfo gem;
          // getGemPort to find out current enable status
          gem.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
          gem.gemPortID = ctp->portIdValue;
          retGponDriver = gponCtl_getGemPort(&gem);
          if ( !retGponDriver )
          {
              BCM_Ploam_EnableGemPortInfo info;
              info.gemPortIndex = gem.gemPortIndex;
              info.gemPortID = gem.gemPortID;
              UBOOL8 enableGem = FALSE;
              if (gem.flags & BCM_PLOAM_GEM_PORT_FLAGS_PORT_ENABLED)
              { /* Gem port should be disabled before setting shaper */
                  enableGem = TRUE;
                  info.enable = FALSE;
                  gponCtl_enableGemPort(&info);
              }
              ret = configGemPortShapingUs(&info, ctp, reset);
              if (enableGem == TRUE)
              { /* Re-enable the gem port - if required */
                  info.enable = TRUE;
                  gponCtl_enableGemPort(&info);
              }
          }
       }
       _cmsObj_free((void **) &ctp);
    }

    /*Secondly : Check the macBridgePort*/
    while ((_cmsObj_getNext(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        UBOOL8 usFound = FALSE;
        UBOOL8 dsFound = FALSE;

        usFound = ((port->inTdPointer == trafficDescCfg->managedEntityId) && (port->tpType == OMCI_BP_TP_PPTP_ETH));
        dsFound = ((port->outTdPointer == trafficDescCfg->managedEntityId) && (port->tpType == OMCI_BP_TP_PPTP_ETH));
        if (usFound == TRUE)
        {
           rutGpon_configPptpEthShapingUs(port, reset);
        }
        if (dsFound == TRUE)
        {
           rutGpon_configPptpEthShapingDs(port, reset);
        }

        _cmsObj_free((void **) &port);
    }

    return ret;
}

static void addGemToDefaultGponIf(const UINT16 gemIndex)
{
    if (gemIndex >= CONFIG_BCM_MAX_GEM_PORTS)
    {
        cmsLog_error("Failed to addGemToDefaultGponIf: %d is out of range [0..%d]",
            gemIndex, CONFIG_BCM_MAX_GEM_PORTS - 1);
        return;
    }

    // init gponif device
    gponif_init();

    if (rutGpon_isInterfaceExisted(GPON_DEF_IFNAME) == FALSE)
    {
        // create default gpon interface
        gponif_create(GPON_DEF_IFNAME);
        rutGpon_ifUp(GPON_DEF_IFNAME);
    }

    // add the GEM port index to default gpon interface
    gponif_addgemidx(GPON_DEF_IFNAME, gemIndex);

    // close gponif device
    gponif_close();
}

static void removeGemFromDefaultGponIf(const UINT16 gemIndex)
{
    UINT32 gemNum = 0;

    if (gemIndex >= CONFIG_BCM_MAX_GEM_PORTS)
    {
        cmsLog_error("Failed to removeGemFromDefaultGponIf: %d is out of range [0..%d]",
            gemIndex, CONFIG_BCM_MAX_GEM_PORTS - 1);
        return;
    }

    if (rutGpon_isInterfaceExisted(GPON_DEF_IFNAME) == TRUE)
    {
        // init gponif device
        gponif_init();

        // remove gem id out of this interface
        gponif_remgemidx(GPON_DEF_IFNAME, gemIndex);

        // get number of gems that belong to this interface
        gponif_get_number_of_gems(GPON_DEF_IFNAME, &gemNum);

        // only delete default gpon interface
        // if no GEM ports are attached to it
        if (gemNum == 0)
        {
            // make default gpon interface down
            rutGpon_ifDown(GPON_DEF_IFNAME);
            // remove gpon interface
            gponif_delete(GPON_DEF_IFNAME);
        }

        // close gponif device
        gponif_close();
    }
}


//=======================  Public GPON functions ========================

CmsRet rutGpon_addAutoObject
    (UINT32 oid,
     UINT32 managedEntityId,
     UBOOL8 persistent __attribute__((unused)))
{
    UINT32 flags = 0;
    void   *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    /* do nothing if instance is already created */
    if (rutGpon_isObjectExisted(oid, managedEntityId) == TRUE)
    {
        cmsLog_debug("oid=%d, id = %d is already existed",
                     oid, managedEntityId);
        return ret;
    }

    /* add new instance */
    if ((ret = _cmsObj_addInstance(oid, &iidStack)) != CMSRET_SUCCESS)
    {
        cmsLog_error("could not create oid=%d, managedEntityId=%d, ret=%d",
                     oid, managedEntityId, ret);
        return ret;
    }

    cmsLog_debug("new oid=%d, managedEntityId=%d, created at %s",
                 oid, managedEntityId, _cmsMdm_dumpIidStack(&iidStack));

     /* get new instance */
    if ((ret = _cmsObj_get(oid, &iidStack, flags, (void **) &obj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("could not get oid=%d, managedEntityId=%d, ret=%d",
                     oid, managedEntityId, ret);
        return ret;
    }

    /* link auto object with created object
       MacBridgePortBridgeTableDataObject is used as generic type
       since it only has managedEntityId as its parameter */
    ((MacBridgePortBridgeTableDataObject *)obj)->managedEntityId = managedEntityId;

    /* set new instance */
    ret = _cmsObj_set(obj, &iidStack);

    _cmsObj_free((void **) &obj);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("could not set oid=%d, managedEntityId=%d, ret=%d",
                     oid, managedEntityId, ret);
        return ret;
    }

    return ret;
}

CmsRet rutGpon_setAutoObject(UINT32 oid, UINT32 oldId, UINT32 newId)
{
    UBOOL8 found = FALSE;
    void   *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    /* search instance that has id matched with the old id */
    while ((!found) &&
          ((ret = _cmsObj_getNext(oid, &iidStack, (void **) &obj)) == CMSRET_SUCCESS))
    {
        // MacBridgePortBridgeTableDataObject is used as generic type
        // since it only has managedEntityId as its parameter
        found = (((MacBridgePortBridgeTableDataObject *)obj)->managedEntityId == oldId);
        // only free obj when not found
        if (found == FALSE)
            _cmsObj_free((void **) &obj);
    }

    /* change managed entity id of instance to new id */
    if (found == TRUE)
    {
        cmsLog_debug("set oid=%d, id=%d, at %s",
                     oid, oldId, _cmsMdm_dumpIidStack(&iidStack));
        /* link auto object with created object
           MacBridgePortBridgeTableDataObject is used as generic type
           since it only has managedEntityId as its parameter */
        ((MacBridgePortBridgeTableDataObject *)obj)->managedEntityId = newId;
        ret = _cmsObj_set(obj, &iidStack);
        _cmsObj_free((void **) &obj);
        if (ret != CMSRET_SUCCESS)
            cmsLog_error("could not set oid=%d, id=%d, ret=%d",
                         oid, oldId, ret);
    }
    else
    {
        cmsLog_error("could not find oid=%d with managedEntityId=%d",
                     oid, oldId);
        ret = CMSRET_OBJECT_NOT_FOUND;
    }

    return ret;
}

CmsRet rutGpon_deleteAutoObject(UINT32 oid, UINT32 managedEntityId)
{
    UBOOL8 found = FALSE;
    void   *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    /* search instance that has id matched with the given id */
    while ((!found) &&
          ((ret = _cmsObj_getNext(oid, &iidStack, (void **) &obj)) == CMSRET_SUCCESS))
    {
        // MacBridgePortBridgeTableDataObject is used as generic type
        // since it only has managedEntityId as its parameter
        found = (((MacBridgePortBridgeTableDataObject *)obj)->managedEntityId == managedEntityId);
        _cmsObj_free((void **) &obj);
    }

    /* delete instance if found */
    if (found == TRUE)
    {
        cmsLog_debug("deleting oid=%d, id=%d, at %s", oid,
                     managedEntityId, _cmsMdm_dumpIidStack(&iidStack));
        ret = _cmsObj_deleteInstance(oid, &iidStack);
    }
    else
    {
        // reduce error level to notice level when object cannot be found
        cmsLog_notice("could not find oid=%d with managedEntityId=%d",
                      oid, managedEntityId);
        ret = CMSRET_SUCCESS;
    }

    return ret;
}

CmsRet rutGpon_addAutoMacBridgeConfigDataObject
    (MacBridgeServiceProfileObject *service)
{
    UINT32 flags = 0;
    MacBridgeConfigDataObject *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    /* do nothing if instance is already created */
    if (rutGpon_isObjectExisted(MDMOID_MAC_BRIDGE_CONFIG_DATA, service->managedEntityId) == TRUE)
    {
        cmsLog_debug("oid=%d, id = %d is already existed",
                     MDMOID_MAC_BRIDGE_CONFIG_DATA,
                     service->managedEntityId);
        return ret;
    }

    /* add new instance */
    if ((ret = _cmsObj_addInstance(MDMOID_MAC_BRIDGE_CONFIG_DATA, &iidStack)) != CMSRET_SUCCESS)
    {
        cmsLog_error("could not create oid=%d, managedEntityId=%d, ret=%d",
                     MDMOID_MAC_BRIDGE_CONFIG_DATA,
                     service->managedEntityId, ret);
        return ret;
    }

    cmsLog_debug("new oid=%d, managedEntityId=%d, created at %s",
                 MDMOID_MAC_BRIDGE_CONFIG_DATA,
                 service->managedEntityId,
                 _cmsMdm_dumpIidStack(&iidStack));

     /* get new instance */
    if ((ret = _cmsObj_get(MDMOID_MAC_BRIDGE_CONFIG_DATA, &iidStack, flags, (void **) &obj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("could not get oid=%d, managedEntityId=%d, ret=%d",
                     MDMOID_MAC_BRIDGE_CONFIG_DATA,
                     service->managedEntityId, ret);
        return ret;
    }

    /* link mac bridge config data object with
       mac bridge service profile object */
    obj->managedEntityId = service->managedEntityId;
    obj->priority = service->priority;
    obj->helloTime = service->helloTime;
    obj->forwardDelay = service->forwardDelay;

    /* set new instance */
    ret = _cmsObj_set(obj, &iidStack);
    _cmsObj_free((void **) &obj);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("could not set oid=%d, managedEntityId=%d, ret=%d",
                     MDMOID_MAC_BRIDGE_CONFIG_DATA,
                     service->managedEntityId, ret);
        return ret;
    }

    return ret;
}

CmsRet rutGpon_setAutoMacBridgeConfigDataObject
    (MacBridgeServiceProfileObject *service, UINT32 oldId)
{
    UBOOL8 found = FALSE;
    MacBridgeConfigDataObject *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    /* search instance that has id matched with the old id */
    while ((!found) &&
           ((ret = _cmsObj_getNext(MDMOID_MAC_BRIDGE_CONFIG_DATA,
                                 &iidStack, (void **) &obj))
            == CMSRET_SUCCESS))
    {
        found = (obj->managedEntityId == oldId);
        // only free obj when not found
        if (found == FALSE)
            _cmsObj_free((void **) &obj);
    }

    if (found == TRUE)
    {
        cmsLog_debug("set oid=%d, id=%d, at %s",
                     MDMOID_MAC_BRIDGE_CONFIG_DATA,
                     oldId, _cmsMdm_dumpIidStack(&iidStack));
        /* link mac bridge config data object with
           mac bridge service profile object */
        obj->managedEntityId = service->managedEntityId;
        obj->priority = service->priority;
        obj->helloTime = service->helloTime;
        obj->forwardDelay = service->forwardDelay;
        ret = _cmsObj_set(obj, &iidStack);
        _cmsObj_free((void **) &obj);
        if (ret != CMSRET_SUCCESS)
            cmsLog_error("could not set oid=%d, id=%d, ret=%d",
                         MDMOID_MAC_BRIDGE_CONFIG_DATA,
                         oldId, ret);
    }
    else
    {
        cmsLog_error("could not find oid=%d with managedEntityId=%d",
                     MDMOID_MAC_BRIDGE_CONFIG_DATA, oldId);
        ret = CMSRET_OBJECT_NOT_FOUND;
    }

    return ret;
}

CmsRet rutGpon_addDefaultReceivedFrameVlanTaggingOperationTableObject
    (const InstanceIdStack *pIidStackParent)
{
    UINT8 rowMax = 1, colMax = 16, i = 0, j = 0;
    UINT8 tagRuleTbl[rowMax][colMax];
    char *str = NULL;
    ReceivedFrameVlanTaggingOperationTableObject *tagTblEntry = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    // initialize tagRuleTbl
    for (i = 0; i < rowMax; i++)
        for (j = 0; j < colMax; j++)
            tagRuleTbl[i][j] = 0x00;

    // setup default tagRules that are defined
    // in section 9.3.13 Extended VLAN tagging operation
    // configuration data of ITU-T G.988

    // Note: update the rowMax value when reducing number of default rules
    tagRuleTbl[0][0] = 0xF0;
    tagRuleTbl[0][4] = 0xF0;
    tagRuleTbl[0][9] = 0x0F;
    tagRuleTbl[0][13] = 0x0F;

#if 0
    tagRuleTbl[1][0] = 0xF0;
    tagRuleTbl[1][4] = 0xE0;
    tagRuleTbl[1][9] = 0x0F;
    tagRuleTbl[1][13] = 0x0F;

    tagRuleTbl[2][0] = 0xE0;
    tagRuleTbl[2][4] = 0xE0;
    tagRuleTbl[2][9] = 0x0F;
    tagRuleTbl[2][13] = 0x0F;
#endif

    for (i = 0; i < rowMax; i++)
    {
        // copy parent instance stack to local instance stack
        memcpy(&iidStack, pIidStackParent, sizeof(InstanceIdStack));

        ret = _cmsObj_addInstance
                (MDMOID_RECEIVED_FRAME_VLAN_TAGGING_OPERATION_TABLE, &iidStack);
        if (ret == CMSRET_SUCCESS)
        {
            ret = _cmsObj_get(MDMOID_RECEIVED_FRAME_VLAN_TAGGING_OPERATION_TABLE,
                             &iidStack, 0, (void **) &tagTblEntry);
            if (ret == CMSRET_SUCCESS)
            {
                // note: 1 byte is stored as 2 ascii characters in
                // CMS database ==> taggingRule is 16 bytes so need
                // to allocate (16 * 2) + 1 for NULL character
                cmsMem_free(tagTblEntry->taggingRule);
                tagTblEntry->taggingRule = cmsMem_alloc
                    ((colMax * 2) + 1,
                     ALLOC_ZEROIZE);
                if(tagTblEntry->taggingRule == NULL)
                {
                    cmsLog_error("failed to allocate memory");
                    ret = CMSRET_INTERNAL_ERROR;
                }
                else
                {
                    cmsUtl_binaryBufToHexString(tagRuleTbl[i], colMax, &str);
                    memcpy(tagTblEntry->taggingRule, str, colMax * 2);
                    CMSMEM_FREE_BUF_AND_NULL_PTR(str);
                    ret = _cmsObj_set(tagTblEntry, &iidStack);
                }
                _cmsObj_free((void **)&tagTblEntry);
            }
            else
            {
                cmsLog_error
                    ("Could not get VLAN Tagging Operation Table object, ret = %d", ret);
            }
        }
        else
        {
            cmsLog_error
                ("Could not add VLAN Tagging Operation Table object, ret = %d", ret);
        }
    }

    return ret;
}

FILE *rutGpon_getOmciDebugFile(void)
{
    return fsDebug;
}

UINT32 rutGpon_getOmciDebugModule(OmciDebugModule module)
{
    UINT32 moduleResult = 0;
    omciDebug_t omciDebug;

    omciDebug.flags.all = rutGpon_getOmciDebugInfo();

    switch (module)
    {
        case OMCI_DEBUG_MODULE_OMCI:
            moduleResult = omciDebug.flags.bits.omci;
            break;
        case OMCI_DEBUG_MODULE_MODEL:
            moduleResult = omciDebug.flags.bits.model;
            break;
        case OMCI_DEBUG_MODULE_VLAN:
            moduleResult = omciDebug.flags.bits.vlan;
            break;
        case OMCI_DEBUG_MODULE_CMF:
            moduleResult = omciDebug.flags.bits.cmf;
            break;
        case OMCI_DEBUG_MODULE_FLOW:
            moduleResult = omciDebug.flags.bits.flow;
            break;
        case OMCI_DEBUG_MODULE_RULE:
            moduleResult = omciDebug.flags.bits.rule;
            break;
        case OMCI_DEBUG_MODULE_MCAST:
            moduleResult = omciDebug.flags.bits.mcast;
            break;
        case OMCI_DEBUG_MODULE_VOICE:
            moduleResult = omciDebug.flags.bits.voice;
            break;
        case OMCI_DEBUG_MODULE_FILE:
            moduleResult = omciDebug.flags.bits.file;
            break;
        default:
            break;
    }

    // Return whether specified module has enabled debugging.
    return moduleResult;
}

UINT32 rutGpon_getOmciDebugInfo(void)
{
    UINT32 debugFlags = omciDebugSetting.flags.all;

    return debugFlags;
}

void rutGpon_setOmciDebugInfoLocal(UINT32 info)
{
    omciDebugSetting.flags.all = info;
}

void rutGpon_setOmciDebugInfo(UINT32 info)
{
    omciDebug_t new, curr;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *obj = NULL;

    // Attempt to get OMCI object.
    if (_cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, 0, (void *) &obj) == CMSRET_SUCCESS)
    {
        new.flags.all = info;
        curr.flags.all = obj->debugFlags;

        if (curr.flags.bits.file == 0 &&
          new.flags.bits.file == 1)
            rutGpon_openDebugFile();
        else if (curr.flags.bits.file == 1 &&
          new.flags.bits.file == 0)
            rutGpon_closeDebugFile();

        obj->debugFlags = info;

        _cmsObj_set(obj, &iidStack);
        _cmsObj_free((void **)&obj);

        rutGpon_setOmciDebugInfoLocal(info);
    }
}

void rutGpon_setOmciPromiscInfo(UINT32 promiscMode)
{
    CmsRet cmsResult = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *obj = NULL;

    // Test for locking success.
    if (cmsResult == CMSRET_SUCCESS)
    {
        // Attempt to get OMCI object.
        if (_cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, 0, (void *) &obj) == CMSRET_SUCCESS)
        {
            obj->promiscMode = promiscMode;
            _cmsObj_set(obj, &iidStack);
            _cmsObj_free((void **)&obj);
        }
    }
}


UINT32 rutGpon_getOmciPromiscInfo(void)
{
    CmsRet cmsResult = CMSRET_SUCCESS;
    UINT32 promiscMode = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *obj = NULL;

    // Test for locking success.
    if (cmsResult == CMSRET_SUCCESS)
    {
        // Attempt to get OMCI object.
        if (_cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, 0, (void *) &obj) == CMSRET_SUCCESS)
        {
            promiscMode = obj->promiscMode;
            _cmsObj_free((void **)&obj);
        }
    }

    return promiscMode;
}


CmsRet rutGpon_configIpHost
    (UINT32 oid, UINT32 meId, char *ifName)
{
    CmsRet ret = CMSRET_INVALID_ARGUMENTS;

#ifdef DMP_X_BROADCOM_COM_IPV6_1
    if (oid == MDMOID_IP_HOST_CONFIG_DATA)
        ret = _owapi_rut_configIpv4Host(meId, ifName);
    else if (oid == MDMOID_IPV6_HOST_CONFIG_DATA)
        ret = _owapi_rut_configIpv6Host(meId, ifName);
#else
    if (oid == MDMOID_IP_HOST_CONFIG_DATA)
        ret = _owapi_rut_configIpv4Host(meId, ifName);
#endif    // DMP_X_BROADCOM_COM_IPV6_1

    return ret;
}

void rutGpon_configTcpUdpByIpHost(UINT32 meId, UBOOL8 add __attribute__((unused)))
{
    UBOOL8 hasTcpUdp = FALSE;
    TcpUdpConfigDataObject *tcpUdp = NULL;
    InstanceIdStack tcpUdpIidStack = EMPTY_INSTANCE_ID_STACK;

    // remove rule of tcpUdp config data
    while ((!hasTcpUdp) &&
           (_cmsObj_getNext(MDMOID_TCP_UDP_CONFIG_DATA,
                                 &tcpUdpIidStack, (void **) &tcpUdp)
             == CMSRET_SUCCESS))
    {
        hasTcpUdp = (tcpUdp->ipHostPointer == meId);
        if (hasTcpUdp == TRUE)
        {
            _owapi_rut_configTcpUdp(tcpUdp, FALSE);
        }
        _cmsObj_free((void **) &tcpUdp);
    }
}

CmsRet rutGpon_deConfigGemPort(const GemPortNetworkCtpObject *ctp)
{
    int retGponDriver = 0;
    BCM_Ploam_GemPortInfo gem;
    BCM_Ploam_EnableGemPortInfo info;
    BCM_Ploam_DecfgGemPortInfo gemPort;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 phyId = 0;

    memset(&gem, 0, sizeof(BCM_Ploam_GemPortInfo));
    memset(&info, 0, sizeof(BCM_Ploam_EnableGemPortInfo));
    memset(&gemPort, 0, sizeof(BCM_Ploam_DecfgGemPortInfo));

    // getGemPort to find out gemPortIndex
    gem.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
    gem.gemPortID = ctp->portIdValue;
    retGponDriver = gponCtl_getGemPort(&gem);

    if ( !retGponDriver )
    {
        info.gemPortIndex = gem.gemPortIndex;
        info.gemPortID = gem.gemPortID;
        info.enable = FALSE;

        // disable gemPort
        gponCtl_enableGemPort(&info);

        // reset Upstream shaping
        if (ctp->direction != OMCI_FLOW_DOWNSTREAM && ctp->upstreamTrafficDescriptorProfilePointer)
            ret = configGemPortShapingUs(&info, ctp, TRUE);

        // Delete omci and vlan rules
        if (rutGpon_getIndexFromGemPortNetworkCtp(ctp, &phyId) == CMSRET_SUCCESS)
        {
            omci_rule_delete_by_phyId(phyId, OMCI_PHY_GPON);
        }

        // deconfigure gemPort
        gemPort.gemPortIndex = gem.gemPortIndex;
        gemPort.gemPortID = gem.gemPortID;
        gponCtl_deconfigGemPort(&gemPort);
        omciDm_removeGemEntry(ctp->managedEntityId);

        // remove gem port out of default gpon interface
        removeGemFromDefaultGponIf(gem.gemPortIndex);

        // re-launch MCPD everytime GPON interface is deleted
        rutGpon_reloadMcpd();
    }
    else
    {
        ret = gponcDrvErrcodeConvert(retGponDriver);
    }

    return ret;
}

CmsRet rutGpon_configGemPort
    (const GemPortNetworkCtpObject *newCtp,
     const GemPortNetworkCtpObject *currCtp)
{
    int retGponDriver = 0;
    BCM_Ploam_GemPortInfo gem;
    BCM_Ploam_CfgGemPortInfo gemPort;
    BCM_Ploam_EnableGemPortInfo info;
    TContObject *tcontObj = NULL;
    CmsRet ret = CMSRET_SUCCESS;
    UINT16 usPQId = 0;
    UINT16 dsPQId = 0;
    PriorityQueueGObject pQInfo;

    memset(&gem, 0, sizeof(BCM_Ploam_GemPortInfo));
    memset(&gemPort, 0, sizeof(BCM_Ploam_CfgGemPortInfo));
    memset(&info, 0, sizeof(BCM_Ploam_EnableGemPortInfo));

    // assign gemPortID and allocID before configuring GEM port
    gemPort.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
    gemPort.gemPortID = newCtp->portIdValue;
    gemPort.encRing = newCtp->encryptionKeyRing;
    gemPort.allocID = BCM_PLOAM_ALLOC_ID_UNASSIGNED;
    // if GEM port is bidirectional then get allocID from its T-CONT
    if (newCtp->direction != OMCI_FLOW_DOWNSTREAM)
    {
        /* Get T-CONT object */
        ret = get_obj_by_instance(MDMOID_T_CONT, (void *)&tcontObj, newCtp->TContPointer, 0);
        if (ret != CMSRET_SUCCESS || tcontObj == NULL)
        {
            cmsLog_error("failed to get T-CONT object, ret=%d", ret);
            ret = CMSRET_INTERNAL_ERROR;
            goto out;
        }
        else
        {
            // get allocID from its T-CONT
            gemPort.allocID = tcontObj->allocId;
            cmsLog_notice("OMCI: Mapped PortId 0x%04X to AllocId 0x%04X (TcAdaptergPtr=0x%04X)\n",
                          newCtp->portIdValue, tcontObj->allocId, newCtp->TContPointer);
            /* Free T-CONT object */
            _cmsObj_free((void *)&tcontObj);
        }
    }

    ret = rutGpon_getQidFromPriorityQueueV2(newCtp->upstreamTrafficManagementPointer,
      &usPQId, &pQInfo);
    ret = rutGpon_getQidFromPriorityQueueV2(newCtp->downstreamPriorityQueuePointer,
      &dsPQId, &pQInfo);

    if (gemPort.allocID == BCM_PLOAM_ALLOC_ID_UNASSIGNED)
    {
        gemPort.isMcast = rutGpon_isGemPortNetworkCtpMulticast(newCtp);
    }
    else
    {
        gemPort.isMcast = FALSE;
    }

    // configure GEM port, if GEM port is already configured then disable it
    retGponDriver = gponCtl_configGemPort(&gemPort);
    if ( !retGponDriver )
    {
        info.gemPortIndex = gemPort.gemPortIndex;
        info.gemPortID = gemPort.gemPortID;
        omciDm_addGemEntry(newCtp->managedEntityId, gemPort.gemPortIndex,
          usPQId, dsPQId);
    }
    else if (retGponDriver == EINVAL_PLOAM_DUPLICATE)
    {
        // getGemPort to find out gemPortIndex
        gem.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
        gem.gemPortID = newCtp->portIdValue;
        retGponDriver = gponCtl_getGemPort(&gem);

        if ( !retGponDriver )
        {
            // disable GEM port
            info.gemPortIndex = gem.gemPortIndex;
            info.gemPortID = gem.gemPortID;
            info.enable = FALSE;
            gponCtl_enableGemPort(&info);
        }
        else
        {
            ret = gponcDrvErrcodeConvert(retGponDriver);
            goto out;
        }
    }
    else
    {
        cmsLog_error("failed to gponCtl_configGemPort, retGponDriver=%d", retGponDriver);
        ret = CMSRET_INTERNAL_ERROR;
        goto out;
    }

    // configure Upstream QoS
    if (newCtp->direction != OMCI_FLOW_DOWNSTREAM &&
        newCtp->upstreamTrafficManagementPointer != currCtp->upstreamTrafficManagementPointer)
        configGemPortQosUs(&info, newCtp);

    // configure Upstream shaping
    if (newCtp->direction != OMCI_FLOW_DOWNSTREAM &&
        newCtp->upstreamTrafficDescriptorProfilePointer != currCtp->upstreamTrafficDescriptorProfilePointer)
        ret = configGemPortShapingUs(&info, newCtp, FALSE);

    // add gem port index to default gpon interface
    addGemToDefaultGponIf(info.gemPortIndex);

    // enable GEM port
    info.enable = TRUE;
    gponCtl_enableGemPort(&info);

    // re-launch MCPD everytime GPON interface is deleted
    rutGpon_reloadMcpd();

out:

    return ret;
}

CmsRet rutGpon_configAllGemPortQosUs
    (const UINT16 tcontMeId)
{
    int retGponDriver = 0;
    BCM_Ploam_GemPortInfo gem;
    BCM_Ploam_EnableGemPortInfo info;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    GemPortNetworkCtpObject *ctp = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    memset(&gem, 0, sizeof(BCM_Ploam_GemPortInfo));
    memset(&info, 0, sizeof(BCM_Ploam_EnableGemPortInfo));

    // search GemPortNetworkCtpObject
    while (_cmsObj_getNext(MDMOID_GEM_PORT_NETWORK_CTP,
                          &iidStack,
                          (void **) &ctp) == CMSRET_SUCCESS)
    {
        if (ctp->TContPointer == tcontMeId)
        {
            // getGemPort to find out gemPortIndex
            gem.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
            gem.gemPortID = ctp->portIdValue;
            retGponDriver = gponCtl_getGemPort(&gem);

            if ( !retGponDriver )
            {
                // disable GEM port
                info.gemPortIndex = gem.gemPortIndex;
                info.gemPortID = gem.gemPortID;
                info.enable = FALSE;
                gponCtl_enableGemPort(&info);
                // configure upstream GEM port QoS
                configGemPortQosUs(&info, ctp);
                // enable GEM port
                info.enable = TRUE;
                gponCtl_enableGemPort(&info);
            }
            else
            {
                ret = gponcDrvErrcodeConvert(retGponDriver);
            }
        }
        _cmsObj_free((void **) &ctp);
    }

    return ret;
}

CmsRet rutGpon_configGemPortAllocId
    (const UINT32 tcontMeId, const UINT32 allocId)
{
    int retGponDriver = 0;
    BCM_Ploam_GemPortInfo gem;
    BCM_Ploam_DecfgGemPortInfo deGemPort;
    BCM_Ploam_CfgGemPortInfo gemPort;
    BCM_Ploam_EnableGemPortInfo info;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    GemPortNetworkCtpObject *ctp = NULL;
    CmsRet ret = CMSRET_SUCCESS;
    UINT16 usPQId = 0;
    UINT16 dsPQId = 0;
    PriorityQueueGObject pQInfo;

    // search GemPortNetworkCtpObject
    while (_cmsObj_getNext(MDMOID_GEM_PORT_NETWORK_CTP,
                          &iidStack,
                          (void **) &ctp) == CMSRET_SUCCESS)
    {
        if (ctp->direction != OMCI_FLOW_DOWNSTREAM &&
            ctp->TContPointer == tcontMeId)
        {
            // getGemPort to find out gemPortIndex
            memset(&gem, 0, sizeof(BCM_Ploam_GemPortInfo));
            gem.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
            gem.gemPortID = ctp->portIdValue;
            retGponDriver = gponCtl_getGemPort(&gem);

            if (!retGponDriver)
            {
                if (gem.allocID != allocId)
                {
                    memset(&gemPort, 0, sizeof(BCM_Ploam_CfgGemPortInfo));
                    memset(&info, 0, sizeof(BCM_Ploam_EnableGemPortInfo));
                    memset(&deGemPort, 0, sizeof(BCM_Ploam_DecfgGemPortInfo));

                    deGemPort.gemPortIndex = gemPort.gemPortIndex = info.gemPortIndex = gem.gemPortIndex;
                    deGemPort.gemPortID = gemPort.gemPortID = info.gemPortID = gem.gemPortID;
                    // disable GEM port
                    info.enable = FALSE;
                    retGponDriver = gponCtl_enableGemPort(&info);

                    // deconfigure gemPort
                    retGponDriver = gponCtl_deconfigGemPort(&deGemPort);

                    // configure GEM port allocID
                    gemPort.allocID = allocId;
                    if (gemPort.allocID == BCM_PLOAM_ALLOC_ID_UNASSIGNED)
                    {
                        gemPort.isMcast = rutGpon_isGemPortNetworkCtpMulticast(ctp);
                    }
                    else
                    {
                        gemPort.isMcast = FALSE;
                    }

                    retGponDriver = gponCtl_configGemPort(&gemPort);

                    // configure upstream GEM port QoS
                    configGemPortQosUs(&info, ctp);

                    // configure Upstream shaping
                    configGemPortShapingUs(&info, ctp, FALSE);
    				
                    // enable GEM port
                    info.enable = TRUE;
                    retGponDriver = gponCtl_enableGemPort(&info);
                }
            }
            else
            {
                switch (retGponDriver)
                {
                    case EINVAL_PLOAM_GEM_PORT:
                        cmsLog_error("There is no configured GEM Port at the given GEM Port ID = %d", ctp->portIdValue);
                        memset(&gemPort, 0, sizeof(BCM_Ploam_CfgGemPortInfo));
                        memset(&info, 0, sizeof(BCM_Ploam_EnableGemPortInfo));
                        /*assign gemPortID and allocID before configuring GEM port*/
                        gemPort.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
                        gemPort.gemPortID = ctp->portIdValue;
                        gemPort.allocID = allocId;
                        if (gemPort.allocID == BCM_PLOAM_ALLOC_ID_UNASSIGNED)
                        {
                            gemPort.isMcast = rutGpon_isGemPortNetworkCtpMulticast(ctp);
                        }
                        else
                        {
                            gemPort.isMcast = FALSE;
                        }
                        retGponDriver = gponCtl_configGemPort(&gemPort);
                        if ( !retGponDriver )
                        {
                            rutGpon_getQidFromPriorityQueueV2(
                              ctp->upstreamTrafficManagementPointer,
                              &usPQId, &pQInfo);
                            rutGpon_getQidFromPriorityQueueV2(
                              ctp->downstreamPriorityQueuePointer,
                              &dsPQId, &pQInfo);
                            info.gemPortIndex = gemPort.gemPortIndex;
                            info.gemPortID = gemPort.gemPortID;
                            omciDm_addGemEntry(ctp->managedEntityId,
                              gemPort.gemPortIndex, usPQId, dsPQId);
                        }
                        else
                        {
                            cmsLog_error("failed to gponCtl_configGemPort, retGponDriver=%d", retGponDriver);
                            _cmsObj_free((void **) &ctp);
                            return CMSRET_INTERNAL_ERROR;
                        }
                        configGemPortQosUs(&info, ctp);
                        if(configGemPortShapingUs(&info, ctp, FALSE) != CMSRET_SUCCESS)
                            cmsLog_notice("Failed to configure gem port %d upstream queue' shaping info",gemPort.gemPortID);
                        /*add gem port index to default gpon interface*/
                        addGemToDefaultGponIf(info.gemPortIndex);
                         /*enable GEM port*/
                        info.enable = TRUE;
                        gponCtl_enableGemPort(&info);
                        /*re-launch MCPD everytime GPON interface is changed*/
                        rutGpon_reloadMcpd();
                        break;
                    case EINVAL_PLOAM_ARG:
                    default:
                        cmsLog_error("The input parameter is out of range, ret=%d\n", retGponDriver);
                        ret = CMSRET_INVALID_ARGUMENTS;
                        break;
                }
            }
        }
        _cmsObj_free((void **) &ctp);
    }

    return ret;
}

#ifdef CONFIG_MIPS_RATE_LIMIT
void rutGpon_configMipsRateLimit(void)
{
    int retEthDrv = 0;

    if (CONFIG_MIPS_RATE_LIMIT)
    {
        cmsLog_notice("CONFIG_MIPS_RATE_LIMIT: rate = %d", CONFIG_MIPS_RATE_LIMIT);
        printf("CONFIG_MIPS_RATE_LIMIT: rate = %d\n", CONFIG_MIPS_RATE_LIMIT);

        // enable MIPS package rate limit for device unit 0, channel 0
        retEthDrv = bcm_enet_driver_rx_pkt_rate_cfg_set(0, 0, TRUE);
        if (!retEthDrv)
        {
            // set MIPS package rate limit for device unit 0, channel 0
            retEthDrv = bcm_enet_driver_rx_pkt_rate_set(0, 0, CONFIG_MIPS_RATE_LIMIT);
            if (retEthDrv)
            {
                cmsLog_error("bcm_enet_driver_rx_pkt_rate_set return error: %d", retEthDrv);
            }
        }
        else
        {
            cmsLog_error("bcm_enet_driver_rx_pkt_rate_cfg_set return error: %d", retEthDrv);
        }
    }
}
#endif

int rutGpon_getOntGAdminState(void)
{
    OntGObject *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    int lock = ME_ADMIN_STATE_UNLOCKED;
    /*if _cmsObj_get failed, assume it as unlock status*/
    if (_cmsObj_get(MDMOID_ONT_G,
                   &iidStack,
                   OGF_NO_VALUE_UPDATE,
                   (void **) &obj) == CMSRET_SUCCESS)
    {
        lock = obj->administrativeState;
        _cmsObj_free((void **) &obj);
    }

    return lock;
}

int rutGpon_getUniGAdminState(UINT32 meId)
{
    UniGObject *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    int lock = ME_ADMIN_STATE_UNLOCKED;
    /*if _cmsObj_get failed, assume it as unlock status*/
    while (_cmsObj_getNextFlags(MDMOID_UNI_G,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &obj) == CMSRET_SUCCESS)
    {
        if(meId == obj->managedEntityId)
        {
           lock = obj->administrativeState;
        }
        _cmsObj_free((void **) &obj);
    }

    return lock;
}

int rutGpon_getCircuitPackAdminState(UINT32 type)
{
    CircuitPackObject *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    int lock = ME_ADMIN_STATE_UNLOCKED;
    /*if _cmsObj_get failed, assume it as unlock status*/
    while (_cmsObj_getNextFlags(MDMOID_CIRCUIT_PACK,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &obj) == CMSRET_SUCCESS)
    {
        if(obj->type == type)
        {
           lock = obj->administrativeState;
        }
        _cmsObj_free((void **) &obj);
    }

    return lock;
}

int rutGpon_getPptpEthernetUniAdminState(UINT32 meId)
{
    PptpEthernetUniObject *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    int lock = ME_ADMIN_STATE_UNLOCKED;
    /*if _cmsObj_get failed, assume it as unlock status*/
    while (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &obj) == CMSRET_SUCCESS)
    {
        if(obj->managedEntityId == meId)
        {
           lock = obj->administrativeState;
        }
        _cmsObj_free((void **) &obj);
    }

    return lock;
}

int rutGpon_getVeipAdminState(UINT32 meId)
{
    VirtualEthernetInterfacePointObject *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    int lock = ME_ADMIN_STATE_UNLOCKED;
    /*if _cmsObj_get failed, assume it as unlock status*/
    while (_cmsObj_getNextFlags(MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &obj) == CMSRET_SUCCESS)
    {
        if(obj->managedEntityId == meId)
        {
           lock = obj->administrativeState;
        }
        _cmsObj_free((void **) &obj);
    }

    return lock;
}

#ifdef DMP_X_ITU_ORG_VOICE_1
int rutGpon_getPptpPotsUniAdminState(UINT32 meId)
{
    PptpPotsUniObject *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    int lock = ME_ADMIN_STATE_UNLOCKED;
    /*if _cmsObj_get failed, assume it as unlock status*/
    while (_cmsObj_getNextFlags(MDMOID_PPTP_POTS_UNI,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &obj) == CMSRET_SUCCESS)
    {
        if(obj->managedEntityId == meId)
        {
           lock = obj->administrativeState;
        }
        _cmsObj_free((void **) &obj);
    }

    return lock;
}
#endif
/*
  in mdm_initgpon.c
  GPON_CARD_HOLDER_ETH_MEID vs type 47
  GPON_CARD_HOLDER_VEIP_MEID vs type 48
  GPON_CARD_HOLDER_MOCA_MEID vs type 46
  GPON_CARD_HOLDER_POTS_MEID vs type 32
  GPON_CARD_HOLDER_GPON_MEID vs type 248
*/
int rutGpon_getRelatedAdminState(MdmObjectId oid, UINT32 meId)
{
    int lock = ME_ADMIN_STATE_UNLOCKED;

    lock = rutGpon_getOntGAdminState();

    if(lock != ME_ADMIN_STATE_UNLOCKED)
    {
       return lock;
    }

    if(oid == MDMOID_PPTP_ETHERNET_UNI)
    {
       /*47 is GPON_CARD_HOLDER_ETH_MEID*/
       lock = rutGpon_getCircuitPackAdminState(47);
       if(lock != ME_ADMIN_STATE_UNLOCKED)
       {
          return lock;
       }

       lock = rutGpon_getUniGAdminState(meId);
       if(lock != ME_ADMIN_STATE_UNLOCKED)
       {
          return lock;
       }
       lock = rutGpon_getPptpEthernetUniAdminState(meId);
       if(lock != ME_ADMIN_STATE_UNLOCKED)
       {
          return lock;
       }
    }
    else if(oid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT)
    {
       /*48 is GPON_CARD_HOLDER_VEIP_MEID*/
       lock = rutGpon_getCircuitPackAdminState(48);
       if(lock != ME_ADMIN_STATE_UNLOCKED)
       {
          return lock;
       }

       lock = rutGpon_getUniGAdminState(meId);
       if(lock != ME_ADMIN_STATE_UNLOCKED)
       {
          return lock;
       }
       lock = rutGpon_getVeipAdminState(meId);
       if(lock != ME_ADMIN_STATE_UNLOCKED)
       {
          return lock;
       }
    }
#ifdef DMP_X_ITU_ORG_VOICE_1
    else if(oid == MDMOID_PPTP_POTS_UNI)
    {
       /*32 is GPON_CARD_HOLDER_POTS_MEID*/
       lock = rutGpon_getCircuitPackAdminState(32);
       if(lock != ME_ADMIN_STATE_UNLOCKED)
       {
          return lock;
       }

       lock = rutGpon_getUniGAdminState(meId);
       if(lock != ME_ADMIN_STATE_UNLOCKED)
       {
          return lock;
       }
       lock = rutGpon_getPptpPotsUniAdminState(meId);
       if(lock != ME_ADMIN_STATE_UNLOCKED)
       {
          return lock;
       }
    }
#endif
    /*The oid check for GEM_IW_TP and M_GEM_IW_TP may is not neccesary.
      It is because the G.988 doesn't define the administrativeState paramters for the two ME.
     */
    else if(oid == MDMOID_GEM_INTERWORKING_TP)
    {
       /*248 is GPON_CARD_HOLDER_GPON_MEID*/
       lock = rutGpon_getCircuitPackAdminState(248);
       if(lock != ME_ADMIN_STATE_UNLOCKED)
       {
          return lock;
       }

    }
    else if(oid == MDMOID_MULTICAST_GEM_INTERWORKING_TP)
    {
       /*248 is GPON_CARD_HOLDER_GPON_MEID*/
       lock = rutGpon_getCircuitPackAdminState(248);
       if(lock != ME_ADMIN_STATE_UNLOCKED)
       {
          return lock;
       }
    }
    else if(oid == MDMOID_ANI_G)
    {
       /*248 is GPON_CARD_HOLDER_GPON_MEID*/
       lock = rutGpon_getCircuitPackAdminState(248);
       if(lock != ME_ADMIN_STATE_UNLOCKED)
       {
          return lock;
       }
    }
    return lock;
}

UBOOL8 rutGpon_getRelatedAdminStateWithLock(MdmObjectId oid, UINT32 meId)
{
    UBOOL8 lock = FALSE;

    cmsLog_debug("mdm object id = %d, meId =%d\n", oid, meId);

    if (ME_ADMIN_STATE_LOCKED == rutGpon_getRelatedAdminState(oid, meId))
    {
        lock = TRUE;
    }

    cmsLog_debug("return mdm object id = %d, meId =%d lock=%d\n", oid, meId, lock);

    return lock;
}


void rutGpon_disableAllPptpEthernetUni(void)
{
    PptpEthernetUniObject *pptpEthernet = NULL;
    InstanceIdStack iidEth = EMPTY_INSTANCE_ID_STACK;

    while (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                               &iidEth,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &pptpEthernet) == CMSRET_SUCCESS)
    {
        UINT32 portNum = 0;
        /*
         ETH0: Swith Port 0: IGD.X_ITU_T_ORG.G_988.EthernetServices.PptpEthernetUni.1
         ETH1: Swith Port 1: IGD.X_ITU_T_ORG.G_988.EthernetServices.PptpEthernetUni.2
         ETH2: Swith Port 2: IGD.X_ITU_T_ORG.G_988.EthernetServices.PptpEthernetUni.3
         ETH3: Swith Port 3: IGD.X_ITU_T_ORG.G_988.EthernetServices.PptpEthernetUni.4
         */
        portNum = PEEK_INSTANCE_ID(&iidEth);
        portNum = portNum - 1;
        rutGpon_disablePptpEthernetUni(portNum);
        _cmsObj_free((void **) &pptpEthernet);
    }
}

void rutGpon_enableAllPptpEthernetUni(void)
{
    PptpEthernetUniObject *pptpEthernet = NULL;
    InstanceIdStack iidEth = EMPTY_INSTANCE_ID_STACK;

    while (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                               &iidEth,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &pptpEthernet) == CMSRET_SUCCESS)
    {
        if(pptpEthernet->administrativeState == ME_ADMIN_STATE_UNLOCKED)
        {
            if(ME_ADMIN_STATE_UNLOCKED == rutGpon_getRelatedAdminState(MDMOID_PPTP_ETHERNET_UNI, pptpEthernet->managedEntityId))
            {
                UINT32 portNum = 0;
                /*
                 ETH0: Swith Port 0: IGD.X_ITU_T_ORG.G_988.EthernetServices.PptpEthernetUni.1
                 ETH1: Swith Port 1: IGD.X_ITU_T_ORG.G_988.EthernetServices.PptpEthernetUni.2
                 ETH2: Swith Port 2: IGD.X_ITU_T_ORG.G_988.EthernetServices.PptpEthernetUni.3
                 ETH3: Swith Port 3: IGD.X_ITU_T_ORG.G_988.EthernetServices.PptpEthernetUni.4
                 */
                portNum = PEEK_INSTANCE_ID(&iidEth);
                portNum = portNum - 1;
                rutGpon_enablePptpEthernetUni(portNum);
            }
        }
        _cmsObj_free((void **) &pptpEthernet);
    }
}

/* Initialize ETH UNI TM and queues. */
void rutGpon_pptpEthUniTmInit(void)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciEthPortType_t eth;
    UINT32 portNum = 0;
    OmciSystemObject *obj = NULL;
    char *ethIfName = NULL;
    tmctl_if_t tm_if;
    UINT32 qidPrioMap;
    int tmctl_flags = 0;

    ret = _cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, 0, (void*)&obj);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("get(MDMOID_OMCI_SYSTEM) failed, ret= %d\n", ret);
        return;
    }

    eth.types.all = obj->ethernetTypes;
    for (portNum = 0; portNum < obj->numberOfEthernetPorts; portNum++)
    {
        omciUtil_getPortType(portNum, eth.types.all);
        memset(&tm_if, 0, sizeof(tm_if));
        rutGpon_getInterfaceName(OMCI_PHY_ETHERNET, portNum, &ethIfName);
        if (ethIfName != NULL)
        {
            tm_if.ethIf.ifname = ethIfName;
            qidPrioMap = _owapi_rut_tmctl_getQueueMap();
            tmctl_flags = TMCTL_SCHED_TYPE_SP | TMCTL_INIT_DEFAULT_QUEUES;
            tmctl_flags |= (qidPrioMap == QID_PRIO_MAP_Q7P7)? \
              TMCTL_QIDPRIO_MAP_Q7P7 : TMCTL_QIDPRIO_MAP_Q0P7;
            if (_owapi_rut_tmctl_portTmInit(TMCTL_DEV_ETH, &tm_if,
              tmctl_flags) == TMCTL_ERROR)
            {
                cmsLog_error("Failed to setup TM for ETH port %s\n",
                  ethIfName);
            }
            CMSMEM_FREE_BUF_AND_NULL_PTR(ethIfName);
        }
    }
    _cmsObj_free((void**)&obj);
}

CmsRet rutGpon_disableGemPortNetworkCtp(UINT32 index, UINT32 portId)
{
    BCM_Ploam_enableGemPortInfo info;
    int retGponDriver = 0;
    CmsRet ret = CMSRET_SUCCESS;

    // disable GEM port
    memset(&info, 0, sizeof(BCM_Ploam_enableGemPortInfo));
    info.gemPortIndex = index;
    info.gemPortID = portId;
    info.enable = FALSE;
    retGponDriver = gponCtl_enableGemPort(&info);
    ret = gponcDrvErrcodeConvert(retGponDriver);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("gponCtl_enableGemPort(0) for port ID %d failed.\n",
          portId);
    }

    return ret;
}

CmsRet rutGpon_enableGemPortNetworkCtp(UINT32 index, UINT32 portId)
{
    BCM_Ploam_enableGemPortInfo info;
    int retGponDriver = 0;
    CmsRet ret = CMSRET_SUCCESS;

    // enable GEM port
    memset(&info, 0, sizeof(BCM_Ploam_enableGemPortInfo));
    info.gemPortIndex = index;
    info.gemPortID = portId;
    info.enable = TRUE;
    retGponDriver = gponCtl_enableGemPort(&info);
    ret = gponcDrvErrcodeConvert(retGponDriver);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("gponCtl_enableGemPort(1) for port ID %d failed.\n",
          portId);
    }

    return ret;
}

void rutGpon_disableAllGemPortNetworkCtp(void)
{
    GemPortNetworkCtpObject *gemPortCtpObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    while (_cmsObj_getNextFlags(MDMOID_GEM_PORT_NETWORK_CTP,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &gemPortCtpObj) == CMSRET_SUCCESS)
    {
        rutGpon_disableGemPortNetworkCtp(BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED, 
          gemPortCtpObj->portIdValue);
        _cmsObj_free((void **) &gemPortCtpObj);
    }
}

void rutGpon_enableAllGemPortNetworkCtp(void)
{
    GemPortNetworkCtpObject *gemPortCtpObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    while (_cmsObj_getNextFlags(MDMOID_GEM_PORT_NETWORK_CTP,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &gemPortCtpObj) == CMSRET_SUCCESS)
    {
        if(ME_ADMIN_STATE_UNLOCKED == rutGpon_getRelatedAdminState(MDMOID_GEM_PORT_NETWORK_CTP, gemPortCtpObj->managedEntityId))
        {
            rutGpon_enableGemPortNetworkCtp(BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED,
              gemPortCtpObj->portIdValue);
        }
        _cmsObj_free((void **) &gemPortCtpObj);
    }
}

/*
 * There are three types of GEM ports: bi-directional unicast GEM, downstream
 * only multicast GEM, and downstream only incidental broadcast GEM.
 * The difference between the latter two is that the multicast GEM is link with
 * a multicast GEM IWTP ME instance. The linkage information is not available
 * when the GEM port network CTP ME instance is created.
 * In order to pass the isMcast flag to the GPON driver, needs to reconfigure the
 * multicast GEM when a multicast GEM IWTP ME instance is created.
 */ 

CmsRet rutGpon_reconfigMcastGemPort(GemPortNetworkCtpObject *ctp)
{
    int retGponDriver = 0;
    BCM_Ploam_GemPortInfo curGemInfo;
    BCM_Ploam_GemPortInfo newGemInfo;
    BCM_Ploam_DecfgGemPortInfo gemPortDecfg;
    BCM_Ploam_CfgGemPortInfo gemPortCfg;
    CmsRet ret = CMSRET_SUCCESS;

    /* Get gemPort info from driver. */
    memset(&curGemInfo, 0, sizeof(BCM_Ploam_GemPortInfo));
    curGemInfo.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
    curGemInfo.gemPortID = ctp->portIdValue;
    retGponDriver = gponCtl_getGemPort(&curGemInfo);
    ret = gponcDrvErrcodeConvert(retGponDriver);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("gponCtl_getGemPort() for port ID %d failed.\n",
          ctp->portIdValue);
        return ret;
    }

    if (ctp->portIdValue != curGemInfo.gemPortID)
    {
        cmsLog_error("Unmatched GEM port ID, user %d, driver %d.\n",
          ctp->portIdValue, curGemInfo.gemPortID);
        return ret;
    }

    /* Disable gemPort. */
    ret = rutGpon_disableGemPortNetworkCtp(curGemInfo.gemPortIndex,
      curGemInfo.gemPortID);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("rutGpon_disableGemPortNetworkCtp() for (I%d, P%d) failed.\n",
          curGemInfo.gemPortIndex, curGemInfo.gemPortID);
        return ret;
    }

    /* Deconfigure gemPort. */
    memset(&gemPortDecfg, 0, sizeof(BCM_Ploam_DecfgGemPortInfo));
    gemPortDecfg.gemPortIndex = curGemInfo.gemPortIndex;
    gemPortDecfg.gemPortID = curGemInfo.gemPortID;
    retGponDriver = gponCtl_deconfigGemPort(&gemPortDecfg);
    ret = gponcDrvErrcodeConvert(retGponDriver);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("gponCtl_deconfigGemPort() for port ID %d failed.\n",
          curGemInfo.gemPortID);
        return ret;
    }

    /* Re-configure gemPort. */
    memset(&gemPortCfg, 0, sizeof(BCM_Ploam_CfgGemPortInfo));
    gemPortCfg.gemPortIndex = curGemInfo.gemPortIndex;
    gemPortCfg.gemPortID = curGemInfo.gemPortID;
    gemPortCfg.encRing = ctp->encryptionKeyRing;
    gemPortCfg.allocID = BCM_PLOAM_ALLOC_ID_UNASSIGNED;
    gemPortCfg.isMcast = TRUE;

    retGponDriver = gponCtl_configGemPort(&gemPortCfg);
    ret = gponcDrvErrcodeConvert(retGponDriver);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("gponCtl_configGemPort() for port ID %d failed.\n",
          curGemInfo.gemPortID);
        return ret;
    }

    ret = CMSRET_INTERNAL_ERROR;

    /* Enable GEM port. */
    ret = rutGpon_enableGemPortNetworkCtp(curGemInfo.gemPortIndex,
      curGemInfo.gemPortID);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("rutGpon_enableGemPortNetworkCtp() for (I%d, P%d) failed.\n",
          curGemInfo.gemPortIndex, curGemInfo.gemPortID);
        return ret;
    }

    /* Get gemPort info from driver again. */
    memset(&newGemInfo, 0, sizeof(BCM_Ploam_GemPortInfo));
    newGemInfo.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
    newGemInfo.gemPortID = ctp->portIdValue;
    retGponDriver = gponCtl_getGemPort(&newGemInfo);
    ret = gponcDrvErrcodeConvert(retGponDriver);
    if ((newGemInfo.gemPortIndex != curGemInfo.gemPortIndex) || 
      (ret != CMSRET_SUCCESS))
    {
        cmsLog_error("gponCtl_getGemPort() for (I%d, P%d) failed.\n",
          newGemInfo.gemPortIndex, ctp->portIdValue);
        return ret;
    }

    return ret;
}

void rutGpon_disableUniGAssociatedUni(UINT32 uniGMeId)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    PptpEthernetUniObject *pptpEthernet = NULL;
#ifdef DMP_X_ITU_ORG_VOICE_1
    PptpPotsUniObject *pptpPots = NULL;
#endif

    while (found == FALSE &&
           _cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &pptpEthernet) == CMSRET_SUCCESS)
    {
        if ((pptpEthernet->managedEntityId == uniGMeId) &&
            (rutGpon_isVeipPptpUni(MDMOID_PPTP_ETHERNET_UNI, uniGMeId) == FALSE))
        {
            UINT32 firstMeId = 0, portNum = 0;
            rutGpon_getFirstEthernetMeId(&firstMeId);
            portNum = pptpEthernet->managedEntityId - firstMeId;
            rutGpon_disablePptpEthernetUni(portNum);
            found = TRUE;
        }

        _cmsObj_free((void **) &pptpEthernet);
    }

#ifdef DMP_X_ITU_ORG_VOICE_1
    INIT_INSTANCE_ID_STACK(&iidStack);
    while (found == FALSE &&
           _cmsObj_getNextFlags(MDMOID_PPTP_POTS_UNI,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &pptpPots) == CMSRET_SUCCESS)
    {
        if (pptpPots->managedEntityId == uniGMeId)
        {
            UINT32 portNum = 0;
            portNum = PEEK_INSTANCE_ID(&iidStack);
            portNum = portNum - 1;
#ifdef DMP_VOICE_SERVICE_1
            _owapi_rut_disablePptpPotsUni(portNum);
#endif /* DMP_VOICE_SERVICE_1 */
            found = TRUE;
        }

        _cmsObj_free((void **) &pptpPots);
    }
#endif
}

void rutGpon_enableUniGAssociatedUni(UINT32 uniGMeId)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    PptpEthernetUniObject *pptpEthernet = NULL;
#ifdef DMP_X_ITU_ORG_VOICE_1
    PptpPotsUniObject *pptpPots = NULL;
#endif

    while (found == FALSE &&
           _cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &pptpEthernet) == CMSRET_SUCCESS)
    {
        if ((pptpEthernet->managedEntityId == uniGMeId) &&
            (rutGpon_isVeipPptpUni(MDMOID_PPTP_ETHERNET_UNI, uniGMeId) == FALSE) &&
            (rutGpon_getRelatedAdminState(MDMOID_PPTP_ETHERNET_UNI, uniGMeId) == ME_ADMIN_STATE_UNLOCKED))
        {
            UINT32 firstMeId = 0, portNum = 0;
            rutGpon_getFirstEthernetMeId(&firstMeId);
            portNum = pptpEthernet->managedEntityId - firstMeId;
            rutGpon_enablePptpEthernetUni(portNum);
            found = TRUE;
        }

        _cmsObj_free((void **) &pptpEthernet);
    }

#ifdef DMP_X_ITU_ORG_VOICE_1
    INIT_INSTANCE_ID_STACK(&iidStack);
    while (found == FALSE &&
           _cmsObj_getNextFlags(MDMOID_PPTP_POTS_UNI,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &pptpPots) == CMSRET_SUCCESS)
    {
        if ((pptpPots->managedEntityId == uniGMeId) &&
            (rutGpon_getRelatedAdminState(MDMOID_PPTP_POTS_UNI, uniGMeId) == ME_ADMIN_STATE_UNLOCKED))
        {
            UINT32 portNum = 0;
            portNum = PEEK_INSTANCE_ID(&iidStack);
            portNum = portNum - 1;
#ifdef DMP_VOICE_SERVICE_1
            _owapi_rut_enablePptpPotsUni(portNum);
#endif /* DMP_VOICE_SERVICE_1 */
            found = TRUE;
        }

        _cmsObj_free((void **) &pptpPots);
    }
#endif
}


UBOOL8 rutGpon_isVeipPptpUni(UINT32 oid, UINT32 meId)
{
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
  OmciSystemObject *omciSysObj = NULL;
  UBOOL8 found = FALSE;

  if (oid != MDMOID_PPTP_ETHERNET_UNI)
      return found;

  if (_cmsObj_get(MDMOID_OMCI_SYSTEM,
                 &iidStack,
                 OGF_NO_VALUE_UPDATE,
                 (void *) &omciSysObj) == CMSRET_SUCCESS)
  {
      if (omciSysObj->veipPptpUni0 &&
          omciSysObj->veipManagedEntityId1 == meId)
          found = TRUE;
      _cmsObj_free((void **) &omciSysObj);
  }

  return found;
}

#if defined(DMP_X_ITU_ORG_VOICE_SIP_1) || defined(OMCI_TR69_DUAL_STACK)
static int FormString(LargeStringObject* largeStrObjPtr, char* outputStr)
{
  int totalStrLen = 0;
  char** strPtrArrayPtr = &largeStrObjPtr->part1;
  int strPartCount = largeStrObjPtr->numberOfParts;
  char* tempCharPtr;

  // Loop until all parts are processed.
  while (strPartCount > 0)
  {
    // Setup part's string pointer.
    tempCharPtr = *strPtrArrayPtr++;

    // Test for valid string.
    if ((tempCharPtr != NULL) && (isascii(tempCharPtr[0]) != 0))
    {
      // Find & add part's string length.
      totalStrLen += strlen(tempCharPtr);

      // Test for valid output character array.
      if (outputStr != NULL)
      {
        // Add part's string to output character array.
        strcat(outputStr, tempCharPtr);
      }
    }
    else
    {
      // Done due to invalid pointer or characters.
      break;
    }

    // Dec object's string path count.
    strPartCount--;
  }

  // Return total string length.
  return totalStrLen;
}


static char* FormStringFromObject(LargeStringObject* largeStrObjPtr)
{
  char* strPtr = NULL;
  int totalStrLen;

  // Test for valid part.
  if ((largeStrObjPtr->numberOfParts > 0) && (largeStrObjPtr->numberOfParts < MAX_LARGE_STRING_PARTS))
  {
    // Find Large String object's total string length.
    totalStrLen = FormString(largeStrObjPtr, NULL);

    // Test for valid string length.
    if (totalStrLen > 0)
    {
      // Allocate temporary output character array.
      strPtr = (char*)cmsMem_alloc(totalStrLen + 10, ALLOC_ZEROIZE);

      // Test for valid output character array.
      if (strPtr != NULL)
      {
        // Copy Large String object's string parts into output character array.
        FormString(largeStrObjPtr, strPtr);
      }
    }
  }

  // Return valid output character array on success, NULL on failure
  return strPtr;
}


char* rutGpon_getLargeString(UINT32 largeStrObjID)
{
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
  LargeStringObject* largeStringObjPtr;
  char* resultCharPtr = NULL;
  UBOOL8 largeStringFoundFlag = FALSE;

  // Loop until object found, end-of-list, or error.
  while ((largeStringFoundFlag == FALSE) && (_cmsObj_getNext(MDMOID_LARGE_STRING, &iidStack, (void**)&largeStringObjPtr) == CMSRET_SUCCESS))
  {
    // Test for requested MDMOID_LARGE_STRING object.
    if (largeStringObjPtr->managedEntityId == largeStrObjID)
    {
      // Attempt to form string from requested MDMOID_LARGE_STRING object.
      resultCharPtr = FormStringFromObject(largeStringObjPtr);

      // Signal object found.
      largeStringFoundFlag = TRUE;
    }

    // Free MDMOID_LARGE_STRING object.
    _cmsObj_free((void**)&largeStringObjPtr);
  }

  // Return specified string on success, NULL on failure.
  return resultCharPtr;
}
#endif // defined(DMP_X_ITU_ORG_VOICE_SIP_1) || defined(OMCI_TR69_DUAL_STACK)

CmsRet rutGpon_updateBridgeLearningInd(const _MacBridgeServiceProfileObject *bridgeObj)
{
    const MacBridgePortConfigDataObject *macBrPortCfg;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    // search bridge port config objects
    while(_cmsObj_getNext(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           (void **) &macBrPortCfg) == CMSRET_SUCCESS)
    {
      if (macBrPortCfg->bridgeId == bridgeObj->managedEntityId)
      {
            rutGpon_configPortLearningIndVal(macBrPortCfg, bridgeObj->learning);
      }
       _cmsObj_free((void **) &macBrPortCfg);
    }
    return ret;
}


void rutGpon_setOmciMibResetState(UBOOL8 state)
{
    omciMibResetState = state;
}


UBOOL8 rutGpon_getOmciMibResetState(void)
{
    return omciMibResetState;
}


CmsRet rutGpon_configDropProcedence(const UBOOL8 isDirDs, const UINT8 dpcmKey)
{
    CmsRet ret = CMSRET_SUCCESS;
    bcmVlan_flowDir_t dir;
    int vlanCtlRet;

    dir = (isDirDs == TRUE)? BCM_VLAN_FLOWDIR_DS : BCM_VLAN_FLOWDIR_US;

    vlanCtl_init();
    vlanCtlRet = vlanCtl_setDropPrecedence(dir, (bcmVlan_dpCode_t)dpcmKey);
    if (vlanCtlRet != 0)
    {
        ret = CMSRET_INTERNAL_ERROR;
    }
    vlanCtl_cleanup();

    return ret;
}


CmsRet rutgpon_getSRIndication(UBOOL8 *pSRIndication)
{
    CmsRet ret = CMSRET_SUCCESS;
    BCM_Ploam_SRIndInfo srInd = {0};

    ret = gponCtl_getSRIndication(&srInd);
    if (!ret)
        *pSRIndication= srInd.srIndication;
    return ret;
}


CmsRet rutGpon_getEthPortType(UINT32 port, OmciEthPortType *type)
{
    UINT32 typesAll;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *obj = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    if (port >= GPON_PHY_ETH_PORT_MAX)
    {
        ret = CMSRET_INVALID_ARGUMENTS;
        return ret;
    }

    ret = _cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, 0, (void*)&obj);
    if (ret == CMSRET_SUCCESS)
    {
        typesAll = obj->ethernetTypes;
        *type = omciUtil_getPortType(port, typesAll);
        _cmsObj_free((void**)&obj);
    }

    return ret;
}


CmsRet rutGpon_getEthPortLinkedOmciMe(UINT32 port, UINT16 *uniOid, UINT16 *uniMeId)
{
    OmciSystemObject *omciSysObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciEthPortType portType;
    UINT32 typesAll;
    UINT32 ethMeId;
    UINT16 oid, meId;
    CmsRet ret = CMSRET_SUCCESS;

    if (port >= GPON_PHY_ETH_PORT_MAX)
    {
        ret = CMSRET_INVALID_ARGUMENTS;
        return ret;
    }

    ret = _cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, 0, (void*)&omciSysObj);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("_cmsObj_get(MDMOID_OMCI_SYSTEM) failed, ret=%d", ret);
        return ret;
    }

    /* SFU. */
    if (omciSysObj->numberOfVeipPorts == 0)
    {
        portType = OMCI_ETH_PORT_TYPE_ONT;
    }
    else
    {
        if (omciSysObj->veipPptpUni0)
        {
            /* HGU, PPTP ETH UNI as VEIP. */
            typesAll = omciSysObj->ethernetTypes;
            portType = omciUtil_getPortType(port, typesAll);
        }
        else
        {
            /* HGU. */
            portType = OMCI_ETH_PORT_TYPE_RG;
        }
    }

    if (portType == OMCI_ETH_PORT_TYPE_ONT)
    {
        ret = rutGpon_getFirstEthernetMeId(&ethMeId);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("rutGpon_getFirstEthernetMeId() failed, ret=%d", ret);
            goto out;
        }

        oid = MDMOID_PPTP_ETHERNET_UNI;
        meId = ethMeId + port;
    }
    else
    {
        if (omciSysObj->veipPptpUni0)
        {
            oid = MDMOID_PPTP_ETHERNET_UNI;
        }
        else
        {
            oid = MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT;
        }
        meId = omciSysObj->veipManagedEntityId1;
    }

    if (rutGpon_isObjectExisted(oid, meId) != TRUE)
    {
        cmsLog_error("No matching OMCI UNI port, port=%d, oid=%d, meId=%d",
          port, oid, meId);
        goto out;
    }

    *uniOid = oid;
    *uniMeId = meId;

out:
    _cmsObj_free((void**)&omciSysObj);
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
      "===> ETHUNI port=%u, uniOid=%u, uniMeId=%u, ret=%d\n",
      port, *uniOid, *uniMeId, ret);
    return ret;
}


CmsRet rutGpon_getVirtIntfMeId(UINT16 *uniOid, UINT16 *meId)
{
    OmciSystemObject *omciSysObj = NULL;
    VirtualEthernetInterfacePointObject *veipObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    if (_cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, 0, (void *) &omciSysObj) == CMSRET_SUCCESS)
    {
        if (omciSysObj->veipPptpUni0)
        {
            *uniOid = MDMOID_PPTP_ETHERNET_UNI;
            *meId = omciSysObj->veipManagedEntityId1;
            ret = CMSRET_SUCCESS;
        }

        _cmsObj_free((void**)&omciSysObj);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> ETHUNI uniOid=%d, uniMeId=%d, ret=%d\n",
                   *uniOid, *meId, ret);

    if (ret != CMSRET_SUCCESS)
    {
        INIT_INSTANCE_ID_STACK(&iidStack);
        if (_cmsObj_getNext(MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                           &iidStack,
                           (void **) &veipObj) == CMSRET_SUCCESS)
        {
            *uniOid = MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT;
            *meId = veipObj->managedEntityId;
            ret = CMSRET_SUCCESS;
            _cmsObj_free((void **) &veipObj);
        }
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> VEIP uniOid=%d, uniMeId=%d, ret=%d\n",
                   *uniOid, *meId, ret);
    return ret;
}

UINT32 getOnuOmciTmOption(void)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OntGObject *ontGObj = NULL;
    UINT32 tmOption = OMCI_TRAFFIC_MANAGEMENT_PRIO;

    if (_cmsObj_get(MDMOID_ONT_G, &iidStack, OGF_NO_VALUE_UPDATE,
      (void**)&ontGObj) == CMSRET_SUCCESS)
    {
        tmOption = ontGObj->trafficManagementOption;
        _cmsObj_free((void**)&ontGObj);
    }

    return tmOption;
}

UINT32 getOnuOmciDsInvalidQueueAction(void)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *omciSysObj = NULL;
    UINT32 dsInvalidQueueAction = OMCI_DS_INVALID_QUEUE_ACTION_PBIT_EXT;

    if (_cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, OGF_NO_VALUE_UPDATE,
      (void*)&omciSysObj) == CMSRET_SUCCESS)
    {
        dsInvalidQueueAction = omciSysObj->dsInvalidQueueAction;
        _cmsObj_free((void**)&omciSysObj);
    }

    return dsInvalidQueueAction;
}

OmciPonAppType rutGpon_getOmciAppType(void)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *omciSysObj = NULL;
    OmciPonAppType ponAppType = OMCI_PON_APP_TYPE_984;

    if ((ret = _cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, OGF_NO_VALUE_UPDATE,
      (void*)&omciSysObj)) == CMSRET_SUCCESS)
    {
        ponAppType= omciSysObj->ponAppType;
        _cmsObj_free((void**)&omciSysObj);
    }

    return ponAppType;
}

UBOOL8 rutGpon_getUniDataPathMode(void)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *omciSysObj = NULL;
    UBOOL8 uniDataPathMode = FALSE;

    if ((ret = _cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, OGF_NO_VALUE_UPDATE,
      (void*)&omciSysObj)) == CMSRET_SUCCESS)
    {
        uniDataPathMode = omciSysObj->uniDataPathMode;
        _cmsObj_free((void**)&omciSysObj);
    }

    return uniDataPathMode;
}

UBOOL8 rutGpon_getExtVlanDefaultRuleMode(void)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *omciSysObj = NULL;
    UBOOL8 extVlanDefaultRuleEnable = FALSE;

    if ((ret = _cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, OGF_NO_VALUE_UPDATE,
      (void*)&omciSysObj)) == CMSRET_SUCCESS)
    {
        extVlanDefaultRuleEnable = omciSysObj->extVlanDefaultRuleEnable;
        _cmsObj_free((void**)&omciSysObj);
    }

    return extVlanDefaultRuleEnable;
}


UBOOL8 rutGpon_isAllocIdInitValue(UINT16 allocId)
{
    OmciPonAppType ponAppType;
    UBOOL8 result = FALSE;

    ponAppType = rutGpon_getOmciAppType();
    if (allocId == 0xffff)
    {
        result = TRUE;
    }
    else if (allocId == 0xff)
    {
        if (ponAppType == OMCI_PON_APP_TYPE_984)
        {
            result = TRUE;
        }
    }

    return result;
}

void rutGpon_macBpCfgDataSetUniConfigState(UINT8 type, UINT32 mePointer)
{
    switch (type)
    {    
        case OMCI_BP_TP_PPTP_ETH:
            omciDm_setUniEntryConfigState(MDMOID_PPTP_ETHERNET_UNI,
              mePointer, TRUE);
            break;
        case OMCI_BP_TP_IP_HOST:
        case OMCI_BP_TP_VEIP:
        case OMCI_BP_TP_PPTP_MOCA:
            break;
        default:
            break;
    }    
}

/*****************************************************************************
*  FUNCTION:  rutGpon_getUniPhyInfo
*  PURPOSE:   Get physical port information of a "real" UNI port (ETH or MOCA).
*  PARAMETERS:
*      uniOid - UNI OID.
*      uniMeId - UNI ME id.
*      phyInfoP - derived physical port information.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet rutGpon_getUniPhyInfo(UINT32 uniOid, UINT32 uniMeId,
  OmciPhyInfo_t *phyInfoP)
{
    UINT32 meId = 0;
    CmsRet ret = CMSRET_INTERNAL_ERROR;

    switch (uniOid)
    {
        case MDMOID_PPTP_ETHERNET_UNI:
            ret = rutGpon_getFirstEthernetMeId(&meId);
            if (ret == CMSRET_SUCCESS)
            {
                phyInfoP->phyId = uniMeId - meId;
                phyInfoP->phyType = OMCI_PHY_ETHERNET;
            }
            else
            {
                cmsLog_error("Cannot get first Ethernet ME ID, ret=%d", ret);
            }
            break;
        case MDMOID_PPTP_MOCA_UNI:
            phyInfoP->phyId = 0;
            phyInfoP->phyType = OMCI_PHY_MOCA; 
            ret = CMSRET_SUCCESS;
            break;
        default:
            phyInfoP->phyType = OMCI_PHY_NONE; 
            break;
    }

    return ret;
}

#endif /* DMP_X_ITU_ORG_GPON_1 */
