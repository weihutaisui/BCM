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


#include "owsvc_api.h"
#include "ctlutils_api.h"
#include "me_handlers.h"

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1

#include "rut_gpon_flow.h"
#include "rut_veip.h"
#include "owrut_api.h"

#ifdef OMCI_DEBUG_SUPPORT
static char *ethActionName[] = OMCI_ETH_ACTION_NAME();
static char *vlanActionName[] = OMCI_VLAN_ACTION_NAME();
#endif

CmsRet rutVeip_getVeipVlanName(char *ifName)
{
    CmsRet ret = CMSRET_SUCCESS;

    if (ifName == NULL)
        return CMSRET_INVALID_ARGUMENTS;

    cmsUtl_strncpy(ifName, GPON_WAN_IF_NAME, CMS_IFNAME_LENGTH);

    return ret;
}

static void printPathVlanFilter(const OmciFlowFilterVlanVal_t *pVlanTag)
{
    if(OMCI_IS_DONT_CARE(pVlanTag->tpid))
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "tpid <X>, ");
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "tpid <0x%04X>, ", pVlanTag->tpid);
    }

    if(OMCI_IS_DONT_CARE(pVlanTag->pbits))
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "pbits<X>, ");
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "pbits<%d>, ", pVlanTag->pbits);
    }

    if(OMCI_IS_DONT_CARE(pVlanTag->dei))
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "dei<X>, ");
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "dei<%d>, ", pVlanTag->dei);
    }

    if(OMCI_IS_DONT_CARE(pVlanTag->vid))
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "vid<X>\n");
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "vid<%d>\n", pVlanTag->vid);
    }
}

static void printPathFlow(const OmciPathFlow_t *pPathFlow)
{
    UINT32 i = 0;

    if(OMCI_IS_DONT_CARE(pPathFlow->filter.vlan.nbrOfTags))
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "VLAN Tag Filters  : <X>\n");
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "VLAN Tag Filters  : <%d>\n", pPathFlow->filter.vlan.nbrOfTags);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "Outer VLAN Filter : ");
    printPathVlanFilter(&pPathFlow->filter.vlan.outer);

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "Inner VLAN Filter : ");
    printPathVlanFilter(&pPathFlow->filter.vlan.inner);

    if(OMCI_IS_DONT_CARE(pPathFlow->filter.ethType.val))
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "EtherType Filter  : <X>\n");
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "EtherType Filter  : val <0x%04X> cmpOp <%d>\n",
                       pPathFlow->filter.ethType.val, pPathFlow->filter.ethType.cmpOp);
    }

    for(i=0; i<pPathFlow->action.ethActionIx; ++i)
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "ETH Action[%d]  : type<%d><%s>\n",
                       i, pPathFlow->action.ethAction[i],
                       ethActionName[pPathFlow->action.ethAction[i]]);
    }

    for(i=0; i<pPathFlow->action.vlanActionIx; ++i)
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
            "VLAN Action[%d] : type<%d><%s>, val<%d>/<0x%04X>, toTag<%d>, fromTag<%d>\n",
            i, pPathFlow->action.vlanAction[i].type, vlanActionName[pPathFlow->action.vlanAction[i].type],
            pPathFlow->action.vlanAction[i].val, pPathFlow->action.vlanAction[i].val,
            pPathFlow->action.vlanAction[i].toTag, pPathFlow->action.vlanAction[i].fromTag);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "***************************************************\n");
}

static CmsRet extractServicesInfoFromPathFlows
    (const GemPortNetworkCtpObject *ctp,
     const OmciMapFilterModelType type,
     const OmciPathFlow_t *pFlowUs,
     const OmciPathFlow_t *pFlowDs,
     OmciGemServicesInfo_t *pServicesInfo)
{
    UINT32 gemPortIndex = 0;
    UINT32 serviceIndex = pServicesInfo->numberOfEntries;
    OmciGponWanServiceType serviceType = OMCI_SERVICE_UNICAST;
    CmsRet ret = CMSRET_SUCCESS;

    if (serviceIndex >= OMCI_GEM_SERVICES_MAX)
    {
        cmsLog_error("Invalid serviceIndex %d", serviceIndex);
        return ret;
    }

    if ((ret = rutGpon_getIndexFromGemPortNetworkCtp(ctp, &gemPortIndex))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("rutGpon_getIndexFromGemPortNetworkCtp() failed, GEM %d",
          ctp->portIdValue);
        return ret;
    }

    if ((ret = rutVeip_getVeipVlanName(pServicesInfo->services[serviceIndex].l2Ifname))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("rutVeip_getVeipVlanName() failed, serviceIndex %d",
          serviceIndex);
        return ret;
    }

    if (rutGpon_isGemPortNetworkCtpMulticast(ctp) == TRUE)
        serviceType = OMCI_SERVICE_MULTICAST;
    else if (rutGpon_isGemPortNetworkCtpIncidentBroadcast(ctp) == TRUE)
        serviceType = OMCI_SERVICE_BROADCAST;
    else
        serviceType = OMCI_SERVICE_UNICAST;

    pServicesInfo->services[serviceIndex].linkParams.gemPortIndex = gemPortIndex;
    pServicesInfo->services[serviceIndex].linkParams.portID = ctp->portIdValue;
    pServicesInfo->services[serviceIndex].linkParams.serviceType = serviceType;
    pServicesInfo->services[serviceIndex].serviceParams.pbits = OMCI_FILTER_PRIO_NONE;
    pServicesInfo->services[serviceIndex].serviceParams.vlanId = OMCI_FILTER_VLANID_NONE;

    if (pFlowUs != NULL && pFlowDs != NULL)
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "******************** US Path Flow ********************\n");
        printPathFlow(pFlowUs);

        omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "******************** DS Path Flow ********************\n");
        printPathFlow(pFlowDs);

        pServicesInfo->services[serviceIndex].serviceParams.pbits = OMCI_FILTER_DONT_CARE;
        pServicesInfo->services[serviceIndex].serviceParams.vlanId = OMCI_FILTER_DONT_CARE;

        // if model is mapper model then only check for pbits filter
        if (type == OMCI_MF_MODEL_1_MAP ||
            type == OMCI_MF_MODEL_N_MAP)
        {
            // only increase numberOfEntries if there's pbits filter
            if(!OMCI_IS_DONT_CARE(pFlowUs->filter.vlan.inner.pbits))
            {
                pServicesInfo->services[serviceIndex].serviceParams.pbits = pFlowUs->filter.vlan.inner.pbits;
            }
            pServicesInfo->numberOfEntries++;
        }
        else
        {
            // only increase numberOfEntries if there's vlan filter
            if (!OMCI_IS_DONT_CARE(pFlowUs->filter.vlan.inner.pbits) &&
               !OMCI_IS_DONT_CARE(pFlowUs->filter.vlan.inner.vid))
            {
                pServicesInfo->services[serviceIndex].serviceParams.pbits = pFlowUs->filter.vlan.inner.pbits;
                pServicesInfo->services[serviceIndex].serviceParams.vlanId = pFlowUs->filter.vlan.inner.vid;
            }
            else if(OMCI_IS_DONT_CARE(pFlowUs->filter.vlan.inner.pbits) &&
                    !OMCI_IS_DONT_CARE(pFlowUs->filter.vlan.inner.vid))
            {
                pServicesInfo->services[serviceIndex].serviceParams.vlanId = pFlowUs->filter.vlan.inner.vid;
            }
            else if(!OMCI_IS_DONT_CARE(pFlowUs->filter.vlan.inner.pbits) &&
                    OMCI_IS_DONT_CARE(pFlowUs->filter.vlan.inner.vid))
            {
                pServicesInfo->services[serviceIndex].serviceParams.pbits = pFlowUs->filter.vlan.inner.pbits;
            }
//printf("===> extractServicesInfoFromPathFlows, pbits=%d, vlanId=%d, numberOfEntries=%d\n", pFlowUs->filter.vlan.inner.pbits, pFlowUs->filter.vlan.inner.vid, pServicesInfo->numberOfEntries);
            pServicesInfo->numberOfEntries++;
        }
    }
    else
    {
        // only increase numberOfEntries if there's no filter
        pServicesInfo->numberOfEntries++;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> extractServicesInfoFromPathFlows, aniMeId=%d, gemPortIndex=%d, serviceType=%d, pbits=%d, vlanId=%d, ifName=%s, serviceIdx=%d, numberOfEntries=%d, ret=%d\n",
                    ctp->managedEntityId, gemPortIndex, serviceType,
                    pServicesInfo->services[serviceIndex].serviceParams.pbits,
                    pServicesInfo->services[serviceIndex].serviceParams.vlanId,
                    pServicesInfo->services[serviceIndex].l2Ifname,
                    serviceIndex, pServicesInfo->numberOfEntries, ret);

    return ret;
}

static CmsRet extractPathFlowsFromFiltersInfo
    (const GemPortNetworkCtpObject *ctp,
     const OmciMapFilterModelType type,
     const UINT8 *pbits,
     const OmciVlanTciFilterInfo_t *pTagFilterInfo,
     OmciExtVlanTagOperInfo_t *pExtTagOperInfo,
     OmciGemServicesInfo_t *pServicesInfo)
{
    UINT32 i = 0, j = 0, k = 0;
    OmciPathFlow_t flowUs, flowDs;
    CmsRet retFlow = CMSRET_INTERNAL_ERROR;
    CmsRet ret = CMSRET_INTERNAL_ERROR;

    memset(&flowUs, 0, sizeof(OmciPathFlow_t));
    memset(&flowDs, 0, sizeof(OmciPathFlow_t));

    if (pbits != NULL &&
        pExtTagOperInfo->numberOfEntries > 0 &&
        pTagFilterInfo->numberOfEntries > 0)
    {
        // PBITS is needed so config FCB with only valid pbits
        for (i = OMCI_FILTER_PRIO_MIN; i <= OMCI_FILTER_PRIO_MAX; i++)
        {
            if (pServicesInfo->numberOfEntries >= OMCI_GEM_SERVICES_MAX)
                break;

            if (pbits[i] >= OMCI_FILTER_PRIO_NONE)
                continue;

            for (j = 0; j < pExtTagOperInfo->numberOfEntries; j++)
            {
                if (pServicesInfo->numberOfEntries >= OMCI_GEM_SERVICES_MAX)
                    break;

                if (pExtTagOperInfo->pTagOperTbl[j].removeType == OMCI_TREATMENT_DISCARD_FRAME)
                    continue;

                for (k = 0; k < pTagFilterInfo->numberOfEntries; k++)
                {
                    if (pServicesInfo->numberOfEntries >= OMCI_GEM_SERVICES_MAX)
                        break;

                    retFlow = rutGpon_extractPathFlows(pExtTagOperInfo->inputTpid,
                        pExtTagOperInfo->outputTpid,
                        pExtTagOperInfo->downstreamMode,
                        &pExtTagOperInfo->pTagOperTbl[j],
                        pTagFilterInfo->forwardOperation,
                        &pTagFilterInfo->pVlanTciTbl[k],
                        pbits[i],
                        &flowUs,
                        &flowDs);

                    if (retFlow != CMSRET_SUCCESS)
                        continue;

                    ret = extractServicesInfoFromPathFlows(ctp, type, &flowUs, &flowDs, pServicesInfo);
                }
            }
        }
    }
    else if (pbits != NULL &&
             pExtTagOperInfo->numberOfEntries > 0 &&
             pTagFilterInfo->numberOfEntries == 0)
    {
        // PBITS is needed so config FCB with only valid pbits
        for (i = OMCI_FILTER_PRIO_MIN; i <= OMCI_FILTER_PRIO_MAX; i++)
        {
            if (pServicesInfo->numberOfEntries >= OMCI_GEM_SERVICES_MAX)
                break;

            if (pbits[i] >= OMCI_FILTER_PRIO_NONE)
                continue;

            for (j = 0; j < pExtTagOperInfo->numberOfEntries; j++)
            {
                if (pServicesInfo->numberOfEntries >= OMCI_GEM_SERVICES_MAX)
                    break;

                if (pExtTagOperInfo->pTagOperTbl[j].removeType == OMCI_TREATMENT_DISCARD_FRAME)
                    continue;

                retFlow = rutGpon_extractPathFlows(pExtTagOperInfo->inputTpid,
                    pExtTagOperInfo->outputTpid,
                    pExtTagOperInfo->downstreamMode,
                    &pExtTagOperInfo->pTagOperTbl[j],
                    0,
                    NULL,
                    pbits[i],
                    &flowUs,
                    &flowDs);

                if (retFlow != CMSRET_SUCCESS)
                    continue;

                ret = extractServicesInfoFromPathFlows(ctp, type, &flowUs, &flowDs, pServicesInfo);
            }
        }
    }
    else if (pbits != NULL &&
             pExtTagOperInfo->numberOfEntries == 0 &&
             pTagFilterInfo->numberOfEntries > 0)
    {
        // PBITS is needed so config FCB with only valid pbits
        for (i = OMCI_FILTER_PRIO_MIN; i <= OMCI_FILTER_PRIO_MAX; i++)
        {
            if (pServicesInfo->numberOfEntries >= OMCI_GEM_SERVICES_MAX)
                break;

            if (pbits[i] >= OMCI_FILTER_PRIO_NONE)
                continue;

            for (j = 0; j < pTagFilterInfo->numberOfEntries; j++)
            {
                if (pServicesInfo->numberOfEntries >= OMCI_GEM_SERVICES_MAX)
                    break;

                retFlow = rutGpon_extractPathFlows(0,
                    0,
                    OMCI_XVLAN_DOWNSTREAM_NONE,
                    NULL,
                    pTagFilterInfo->forwardOperation,
                    &pTagFilterInfo->pVlanTciTbl[j],
                    pbits[i],
                    &flowUs,
                    &flowDs);

                if (retFlow != CMSRET_SUCCESS)
                    continue;

                ret = extractServicesInfoFromPathFlows(ctp, type, &flowUs, &flowDs, pServicesInfo);
            }
        }
    }
    else if (pbits != NULL &&
             pExtTagOperInfo->numberOfEntries == 0 &&
             pTagFilterInfo->numberOfEntries == 0)
    {
        // PBITS is needed so config FCB with only valid pbits
        for (i = OMCI_FILTER_PRIO_MIN; i <= OMCI_FILTER_PRIO_MAX; i++)
        {
            if (pServicesInfo->numberOfEntries >= OMCI_GEM_SERVICES_MAX)
                break;

            if (pbits[i] >= OMCI_FILTER_PRIO_NONE)
                continue;

            retFlow = rutGpon_extractPathFlows(0,
                0,
                OMCI_XVLAN_DOWNSTREAM_NONE,
                NULL,
                0,
                NULL,
                pbits[i],
                &flowUs,
                &flowDs);

            if (retFlow != CMSRET_SUCCESS)
                continue;

            ret = extractServicesInfoFromPathFlows(ctp, type, &flowUs, &flowDs, pServicesInfo);
        }
    }
    else if (pbits == NULL &&
             pExtTagOperInfo->numberOfEntries > 0 &&
             pTagFilterInfo->numberOfEntries > 0)
    {
        for (i = 0; i < pExtTagOperInfo->numberOfEntries; i++)
        {
            if (pServicesInfo->numberOfEntries >= OMCI_GEM_SERVICES_MAX)
                break;

            if (pExtTagOperInfo->pTagOperTbl[i].removeType == OMCI_TREATMENT_DISCARD_FRAME)
                continue;

            for (j = 0; j < pTagFilterInfo->numberOfEntries; j++)
            {
                if (pServicesInfo->numberOfEntries >= OMCI_GEM_SERVICES_MAX)
                    break;

                retFlow = rutGpon_extractPathFlows(pExtTagOperInfo->inputTpid,
                    pExtTagOperInfo->outputTpid,
                    pExtTagOperInfo->downstreamMode,
                    &pExtTagOperInfo->pTagOperTbl[i],
                    pTagFilterInfo->forwardOperation,
                    &pTagFilterInfo->pVlanTciTbl[j],
                    OMCI_DONT_CARE,
                    &flowUs,
                    &flowDs);

                if (retFlow != CMSRET_SUCCESS)
                    continue;

                ret = extractServicesInfoFromPathFlows(ctp, type, &flowUs, &flowDs, pServicesInfo);
            }
        }
    }
    else if (pbits == NULL &&
             pExtTagOperInfo->numberOfEntries > 0 &&
             pTagFilterInfo->numberOfEntries == 0)
    {
        for (i = 0; i < pExtTagOperInfo->numberOfEntries; i++)
        {
            if (pServicesInfo->numberOfEntries >= OMCI_GEM_SERVICES_MAX)
                break;

            if (pExtTagOperInfo->pTagOperTbl[i].removeType == OMCI_TREATMENT_DISCARD_FRAME)
                continue;

            retFlow = rutGpon_extractPathFlows(pExtTagOperInfo->inputTpid,
                pExtTagOperInfo->outputTpid,
                pExtTagOperInfo->downstreamMode,
                &pExtTagOperInfo->pTagOperTbl[i],
                0,
                NULL,
                OMCI_DONT_CARE,
                &flowUs,
                &flowDs);

            if (retFlow != CMSRET_SUCCESS)
                continue;

            ret = extractServicesInfoFromPathFlows(ctp, type, &flowUs, &flowDs, pServicesInfo);
        }
    }
    else if (pbits == NULL &&
             pExtTagOperInfo->numberOfEntries == 0 &&
             pTagFilterInfo->numberOfEntries > 0)
    {
        for (i = 0; i < pTagFilterInfo->numberOfEntries; i++)
        {
            if (pServicesInfo->numberOfEntries >= OMCI_GEM_SERVICES_MAX)
                break;

            retFlow = rutGpon_extractPathFlows(0,
                0,
                OMCI_XVLAN_DOWNSTREAM_NONE,
                NULL,
                pTagFilterInfo->forwardOperation,
                &pTagFilterInfo->pVlanTciTbl[i],
                OMCI_DONT_CARE,
                &flowUs,
                &flowDs);

            if (retFlow != CMSRET_SUCCESS)
                continue;				

            ret = extractServicesInfoFromPathFlows(ctp, type, &flowUs, &flowDs, pServicesInfo);
        }
    }
    else if (pbits == NULL &&
             pExtTagOperInfo->numberOfEntries == 0 &&
             pTagFilterInfo->numberOfEntries == 0)
    {
        ret = extractServicesInfoFromPathFlows(ctp, type, &flowUs, &flowDs, pServicesInfo);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> extractPathFlowsFromFiltersInfo, aniMeId=%d, tagOperNum=%d, tagFilterNum=%d, ret=%d\n",
                    ctp->managedEntityId, pExtTagOperInfo->numberOfEntries, pTagFilterInfo->numberOfEntries, ret);

    return ret;
}

static CmsRet getServiceInfoFromMEs
    (const GemPortNetworkCtpObject *ctp,
     const UINT32 uniOid,
     const UINT32 uniMeId,
     const OmciMapFilterModelType type,
     OmciGemServicesInfo_t *pServicesInfo)
{
    UBOOL8 isPbitsNeeded = FALSE, isFilterNeeded = FALSE;
    UINT8 pbits[OMCI_FILTER_PRIO_NONE];
    UINT32 mapperMeId = 0, bridgeMeId = 0;
    UINT32 extVlanNum = 0;
    OmciExtVlanTagOperInfo_t extTagOperInfo;
    OmciVlanTciFilterInfo_t tagFilterInfo;
    OmciVlanTagOperInfo_t tagOperInfo;
    CmsRet ret = CMSRET_INTERNAL_ERROR;

    // initialize local variables
    memset(&extTagOperInfo, 0, sizeof(OmciExtVlanTagOperInfo_t));
    memset(&tagFilterInfo, 0, sizeof(OmciVlanTciFilterInfo_t));
    memset(&tagOperInfo, 0, sizeof(OmciVlanTagOperInfo_t));

    // setup Extended VLAN tagging rule operation table
    ret = rutGpon_allocExtTagOperInfo(uniOid, uniMeId, &extTagOperInfo);
    if (ret != CMSRET_SUCCESS)
        goto out;

    // retrieve VLAN tagging operation configuration data if any
    rutGpon_getTagOperInfo(uniOid, uniMeId, &tagOperInfo);

    /* convert Vlan to XVlan if Vlan exists and XVlan does not */
    if (tagOperInfo.tagIsExisted == TRUE && extTagOperInfo.numberOfEntries == 0)
    {
        if (tagOperInfo.upstreamMode == OMCI_VLAN_UPSTREAM_TAG ||
            tagOperInfo.upstreamMode == OMCI_VLAN_UPSTREAM_PREPEND)
        {
            extTagOperInfo.numberOfEntries = extVlanNum = 2;
            extTagOperInfo.pTagOperTbl = cmsMem_alloc
                (sizeof(OmciExtVlanTagOper_t) * extVlanNum, ALLOC_ZEROIZE);
            if (extTagOperInfo.pTagOperTbl == NULL)
            {
                cmsLog_error("Cannot allocate memory for OmciExtVlanTagOper_t");
                goto out;
            }
            // set input tpid and output tpid to default values
            extTagOperInfo.inputTpid = extTagOperInfo.outputTpid = 0x8100;
            if (tagOperInfo.downstreamMode == OMCI_VLAN_DOWNSTREAM_STRIP)
                extTagOperInfo.downstreamMode = OMCI_XVLAN_DOWNSTREAM_INVERSE;
            else
                extTagOperInfo.downstreamMode = OMCI_XVLAN_DOWNSTREAM_NONE;
            // convert VLAN oper. to Extended VLAN oper.
            rutGpon_convertVlanToXVlan(&tagOperInfo, extTagOperInfo.pTagOperTbl);
        }
    }

    // only config VLAN filter when bridge service profile is existed
    isFilterNeeded = (type == OMCI_MF_MODEL_1_FILTER ||
                          type == OMCI_MF_MODEL_1_MAP_FILTER ||
                          type == OMCI_MF_MODEL_N_FILTER ||
                          type == OMCI_MF_MODEL_N_MAP_FILTER);
    if (isFilterNeeded == TRUE)
    {
        // get bridge Me ID
        ret = rutGpon_getBridgeMeIdFromUniMeId(uniOid, uniMeId, &bridgeMeId);
        if (ret != CMSRET_SUCCESS)
            goto out;
        // setup VLAN tagging filter table
        ret = rutGpon_allocTagFilterInfo(ctp->managedEntityId, bridgeMeId,
                                         &tagFilterInfo);
        if (ret != CMSRET_SUCCESS)
            goto out;
    }

    // only config PBITS when mapper service profile is existed
    // and only for upstream FCB
    isPbitsNeeded = ((ctp->direction != OMCI_FLOW_DOWNSTREAM) &&
                     (type == OMCI_MF_MODEL_1_MAP ||
                      type == OMCI_MF_MODEL_1_MAP_FILTER ||
                      type == OMCI_MF_MODEL_N_MAP ||
                      type == OMCI_MF_MODEL_N_MAP_FILTER));
    if (isPbitsNeeded == TRUE)
    {
        UINT32 i = 0;

        for (i = OMCI_FILTER_PRIO_MIN; i <= OMCI_FILTER_PRIO_MAX; i++)
            pbits[i] = OMCI_FILTER_PRIO_NONE;

        ret = rutGpon_getPbitsFromGemPortNeworkCtp(ctp, pbits, &mapperMeId);

        omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                       "===> getServiceInfoFromMEs, after call rutGpon_getPbitFromGemPortNeworkCtp, pbits=%d|%d|%d|%d|%d|%d|%d|%d, ret=%d\n",
                       pbits[0], pbits[1], pbits[2], pbits[3], pbits[4], pbits[5], pbits[6], pbits[7], ret);

        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Cannot rutGpon_getPbitFromGemPortNeworkCtp, ctpMeId=%d, ret=%d\n",
                         ctp->managedEntityId, ret);
            goto out;
        }

        ret = extractPathFlowsFromFiltersInfo
                    (ctp, type, pbits, &tagFilterInfo, &extTagOperInfo, pServicesInfo);
    }
    else
    {
        ret = extractPathFlowsFromFiltersInfo
                    (ctp, type, NULL, &tagFilterInfo, &extTagOperInfo, pServicesInfo);
    }

out:
    if (extTagOperInfo.numberOfEntries > 0 && extTagOperInfo.pTagOperTbl != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(extTagOperInfo.pTagOperTbl);

    if (tagFilterInfo.numberOfEntries > 0 && tagFilterInfo.pVlanTciTbl != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(tagFilterInfo.pVlanTciTbl);

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> getServiceInfoFromMEs, aniMeId=%d, tagOperNum=%d, tagFilterNum=%d, ret=%d\n",
                    ctp->managedEntityId, extTagOperInfo.numberOfEntries, tagFilterInfo.numberOfEntries, ret);

    return ret;
}

static CmsRet configDefaultLanInterfaces
    (const UINT32 port, const OmciEthPortType type)
{
    CmsRet ret = CMSRET_SUCCESS;
    char   *devInterface = NULL, *vlanInterface = NULL;
    char cmd[BUFLEN_1024];

    // get device interface for ethernet (port)
    rutGpon_getInterfaceName(OMCI_PHY_ETHERNET, port, &devInterface);

    if (rutGpon_isInterfaceExisted(devInterface) == FALSE)
    {
        cmsLog_error("Device interface %s does not exist", devInterface);
        ret = CMSRET_INTERNAL_ERROR;
        goto out;
    }

    // remove devInterface out of default RG bridge br0
    // if ethernet port type is ONT only
    if (type == OMCI_ETH_PORT_TYPE_ONT)
    {
        // remove devInterface (eth0, eth1, etc...) out of default RG bridge br0
#ifdef SUPPORT_LANVLAN
        snprintf(cmd, sizeof(cmd), "brctl delif br0 %s.0 2>/dev/null", devInterface);
#else
        snprintf(cmd, sizeof(cmd), "brctl delif br0 %s 2>/dev/null", devInterface);
#endif /* SUPPORT_LANVLAN */
        _owapi_rut_doSystemAction("rut_gponrg_full", cmd);
    }
    // only add vlanInterface to default RG bridge br0 if
    // ethernet port type is RG
#ifndef SUPPORT_LANVLAN
    else if (type == OMCI_ETH_PORT_TYPE_RG)
    {
        UINT32 i = 0;

        // remove devInterface (eth0, eth1, etc...) out of default RG bridge br0
        // it has to be done before create virtual vlan interface
        // since vlan interface is inherited priv_flags including IFF_BRIDGE_PORT
        // from its devInterface
        snprintf(cmd, sizeof(cmd), "brctl delif br0 %s 2>/dev/null", devInterface);
        _owapi_rut_doSystemAction("rut_gponrg_full", cmd);

        // get virtual interface for ethernet (port) that is member of default RG bridge br0
        rutGpon_getVirtualInterfaceName(OMCI_PHY_ETHERNET, port, 0, &vlanInterface);

        vlanCtl_init();
        vlanCtl_setIfSuffix(".");
        // always set isMulticast to 1 for supporting broadcast
        // create virtual interface (eth0.0, eth1.0, etc...) with isRouted=1, and mcast=1
        rutGpon_vlanCtl_createVlanInterface(devInterface, 0, 1, 1); 
        if (type == OMCI_ETH_PORT_TYPE_RG) 
        {
            vlanCtl_setRealDevMode(devInterface, BCM_VLAN_MODE_RG);
        }
        else
        {
            vlanCtl_setRealDevMode(devInterface, BCM_VLAN_MODE_ONT);
        }
        for (i = 0; i < BCM_VLAN_MAX_TAGS; ++i)
        {
            vlanCtl_setDefaultAction(devInterface, VLANCTL_DIRECTION_RX, i,
                                     VLANCTL_ACTION_ACCEPT, vlanInterface);
        }
        vlanCtl_cleanup();

        if (rutGpon_isInterfaceExisted(vlanInterface) == FALSE)
        {
            cmsLog_error("Failed to create %s", vlanInterface);
            ret = CMSRET_INTERNAL_ERROR;
        }
        else
        {
            char macStr[MAC_STR_LEN+1];
            UINT8 macNum[MAC_ADDR_LEN];

            // bring virtual interface up if it's created successfully
            devCtl_getBaseMacAddress(macNum);
            cmsUtl_macNumToStr(macNum, macStr);
            snprintf(cmd, sizeof(cmd), "ifconfig %s hw ether %s up",
                                 vlanInterface, macStr);
            _owapi_rut_doSystemAction("rcl_gpon", cmd);

            // add virtual interface (eth0.0, eth1.0, etc...) to default RG bridge br0
            snprintf(cmd, sizeof(cmd), "brctl addif br0 %s 2>/dev/null", vlanInterface);
            _owapi_rut_doSystemAction("rut_gponrg_full", cmd);
        }
    }
#endif    // SUPPORT_LANVLAN

out:
    if (vlanInterface != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(vlanInterface);
    if (devInterface != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(devInterface);

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> configDefaultLanInterfaces, port=%d, type=%d, ret=%d\n",
                    port, type, ret);

    return ret;
}

CmsRet rutRgFull_configDefaultLanInterfaces(void)
{
    UINT32 port = 0;
    OmciEthPortType_t eth;
    OmciEthPortType portType;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *obj = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = _cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, 0, (void *) &obj)) == CMSRET_SUCCESS)
    {
        eth.types.all = obj->ethernetTypes;
        for (port = 0; port < obj->numberOfEthernetPorts; port++)
        {
            portType = omciUtil_getPortType(port, eth.types.all);
            if (portType != OMCI_ETH_PORT_TYPE_NONE)
            {
                ret = configDefaultLanInterfaces(port, portType);
            }
        }

        _cmsObj_free((void **)&obj);
    }

    return ret;
}

UINT32 rutRgFull_configVeip(UBOOL8 isCreate, 
                            UINT32 reachableOid,
                            UINT32 reachableMeId,
                            OmciMapFilterModelType type, 
                            const GemPortNetworkCtpObject *ctp)
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 serviceFlowNum = 0;

    if (reachableOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT ||
        rutGpon_isVeipPptpUni(reachableOid, reachableMeId))
    {
        UINT32 i = 0;
        OmciGemServicesInfo_t servicesInfo;
        int adminState = rutGpon_getRelatedAdminState(reachableOid, reachableMeId);

        memset(&servicesInfo, 0, sizeof(OmciGemServicesInfo_t));
        ret = getServiceInfoFromMEs(ctp, reachableOid, reachableMeId, type, &servicesInfo);
        if (ret == CMSRET_SUCCESS)
        {
            for (i = 0; i < servicesInfo.numberOfEntries; i++)
            {
                if (isCreate && adminState == ME_ADMIN_STATE_UNLOCKED)
                {
                     servicesInfo.services[i].serviceParams.serviceStatus = TRUE;
                     omci_service_create(&(servicesInfo.services[i]));
omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
    "i=%d, numberOfEntries=%d, gemPortIndex=%d, pbits=%d, vlanId=%d, ifName=%s, serviceType=%d, OMCI_SERVICE_UP\n",
    i, servicesInfo.numberOfEntries, servicesInfo.services[i].linkParams.gemPortIndex,
    servicesInfo.services[i].serviceParams.pbits, servicesInfo.services[i].serviceParams.vlanId,
    servicesInfo.services[i].l2Ifname, servicesInfo.services[i].linkParams.serviceType);
                }
                else
                {
                     servicesInfo.services[i].serviceParams.serviceStatus = FALSE;
                     omci_service_delete(servicesInfo.services[i].serviceParams.pbits, servicesInfo.services[i].serviceParams.vlanId, servicesInfo.services[i].l2Ifname);
omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
    "i=%d, numberOfEntries=%d, gemPortIndex=%d, pbits=%d, vlanId=%d, ifName=%s, serviceType=%d, OMCI_SERVICE_DOWN\n",
    i, servicesInfo.numberOfEntries, servicesInfo.services[i].linkParams.gemPortIndex,
    servicesInfo.services[i].serviceParams.pbits, servicesInfo.services[i].serviceParams.vlanId,
    servicesInfo.services[i].l2Ifname, servicesInfo.services[i].linkParams.serviceType);
                }
                _owapi_rut_sendServiceInfoMsg(&(servicesInfo.services[i]));
            }
        }
    }

   serviceFlowNum = omci_service_num_get();
   return serviceFlowNum;
}

void rutRgFull_omciServiceCleanupByGemPort(const GemPortNetworkCtpObject *ctp)
{
   omci_service_delete_by_gemPort(ctp->portIdValue, _owapi_rut_sendServiceInfoMsg);
}


#else

/* For non GponRG full Omci, this is empty call */
UINT32 rutRgFull_configVeip(UBOOL8 isCreate __attribute__((unused)),
                            UINT32 reachableOid __attribute__((unused)),
                            UINT32 reachableMeId __attribute__((unused)),
                            OmciMapFilterModelType type __attribute__((unused)),
                            const GemPortNetworkCtpObject *ctp __attribute__((unused)))
{
    return 0;
}

void rutRgFull_omciServiceCleanupByGemPort(const GemPortNetworkCtpObject *ctp __attribute__((unused)))
{
}

#endif /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

