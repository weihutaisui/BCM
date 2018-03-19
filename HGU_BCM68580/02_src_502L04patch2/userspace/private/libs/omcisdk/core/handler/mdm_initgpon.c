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

#include "omciobj_defs.h"
#include "mdmlite_api.h"
#include "owsvc_api.h"
#include "me_handlers.h"
#include "ctlutils_api.h"
#include "owrut_api.h"


#define SWIMAGE_VER_STR_LEN 14
#define SWIMAGE_VER_HEX_LEN (SWIMAGE_VER_STR_LEN * 2)


/*
 * === IMPORTANT ===
 * ANI-G: "Total T-CONT Number" depends on:
 * GPON_TCONT_MAX
 */

static UINT32 gUniInst = 1, gMcastSubsInst = 1, gIpHostInst = 1;
static UINT32 gPrioQInst = 1, gTrSchInst = 1;
#ifdef OMCI_TR69_DUAL_STACK
static UINT32 gTr69Inst = 1;
#endif
static OmciSystemObject omciSys;

static CmsRet addOnuRemoteDebugObject(void);
static CmsRet addPowerMgmtObject(void);

CmsRet initOmciSystemObject(void)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *obj = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    omciSys._oid = MDMOID_OMCI_SYSTEM;
    ret = _owapi_rut_initAppCfgData(&omciSys);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("_owapi_rut_initAppCfgData() failed ret: %d", ret);
        return ret;
    }

    if ((ret = _cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, 0, (void*)&obj))
      == CMSRET_SUCCESS)
    {
        memcpy(obj, &omciSys, sizeof(OmciSystemObject));
        _cmsObj_set(obj, &iidStack);
        _cmsObj_free((void **)&obj);
    }

    rutGpon_setOmciDebugInfoLocal(omciSys.debugFlags);

    return ret;
}

static CmsRet addDefaultGponUniGObject(UINT32 meId)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    _UniGObject *mdmObj = NULL;
    const char *objName = "UniGObject";
    const UINT32 mdmOid = MDMOID_UNI_G;

    INIT_INSTANCE_ID_STACK(&iidStack);
    cmsLog_notice("Adding %s instance %d", objName, meId);
    _mdm_initPathDescriptor(&pathDesc);
    pathDesc.oid = mdmOid;
    PUSH_INSTANCE_ID(&(pathDesc.iidStack), gUniInst++);

    /* add new object instance */
    if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to add %s, ret=%d", objName, ret);
        return ret;
    }

    /* get the object we just added */
    iidStack = pathDesc.iidStack;
    if ((ret = _mdm_getObject(mdmOid,
                             &iidStack,
                             (void **)&mdmObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get %s, ret=%d", objName, ret);
        return ret;
    }

    /* Set the default values of the parameters */
    mdmObj->managedEntityId = meId;

    /* set nonOmciManagementId if UNI-G is for VEIP */
    if (!omciSys.veipPptpUni0 && meId >= omciSys.veipManagedEntityId1)
    {
        mdmObj->nonOmciManagementId = meId;
#ifdef OMCI_TR69_DUAL_STACK
        mdmObj->managementCapability = 2; //both omci and non-moci
#endif
    }
    else
        mdmObj->nonOmciManagementId = 0;

    /* set the object */
    ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to set %s, ret = %d", objName, ret);
        /* _mdm_setObject will steal the object on success, but on failure,
         * we need to free it.
         */
        _mdm_freeObject((void **) &mdmObj);
    }

    return ret;
}

static CmsRet addDefaultGponAniGObject(void)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    _AniGObject *mdmObj = NULL;
    const char *objName = "AniGObject";
    const UINT32 mdmOid = MDMOID_ANI_G;
    UBOOL8 srIndication;
    /* CardHolder meId: 0x01SS (SS=slotId) */
    /* ANI-G meId:      0xSSPP (SS=slotId; PP portId) */
    UINT16 meId = (UINT16)(omciSys.tcontManagedEntityId | 0x01);

    INIT_INSTANCE_ID_STACK(&iidStack);
    cmsLog_notice("Adding %s instance %d", objName, meId);
    _mdm_initPathDescriptor(&pathDesc);
    pathDesc.oid = mdmOid;
    PUSH_INSTANCE_ID(&(pathDesc.iidStack), 1);

    /* add new object instance */
    if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to add %s, ret=%d", objName, ret);
        return ret;
    }

    /* get the object we just added */
    iidStack = pathDesc.iidStack;
    if ((ret = _mdm_getObject(mdmOid,
                             &iidStack,
                             (void **)&mdmObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get %s, ret=%d", objName, ret);
        return ret;
    }

    /* Set the default values of the parameters */

    mdmObj->managedEntityId = meId;
    mdmObj->totalTcontNumber = omciSys.numberOfTConts;
    if (rutgpon_getSRIndication(&srIndication) == CMSRET_SUCCESS)
        mdmObj->srIndication = srIndication;
    /* set the object */
    ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to set %s, ret = %d", objName, ret);
        /* _mdm_setObject will steal the object on success, but on failure,
         * we need to free it.
         */
        _mdm_freeObject((void **) &mdmObj);
    }

    return ret;
}

static CmsRet addDefaultDsGponPriorityQueueGObject
    (const UINT32 oid, const UINT32 pptpMeId, UINT32 *priQNum)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    _PriorityQueueGObject *mdmObj = NULL;
    const char *objName = "PriorityQueueGObject";
    const UINT32 mdmOid = MDMOID_PRIORITY_QUEUE_G;
    UINT16 meId = 0x0000; /* range of DS queue ids is 0x0000 to 0x7FFF */
    UINT32 prio;
    char buf[BUFLEN_32] = {0};

    *priQNum = 0;

    switch (oid)
    {
        case MDMOID_PPTP_ETHERNET_UNI:
            meId = GPON_FIRST_ETH_MEID;
            meId += (pptpMeId - GPON_FIRST_ETH_MEID) * omciSys.numberOfDownstreamPriorityQueues;
            break;
        case MDMOID_PPTP_MOCA_UNI:
            meId = GPON_FIRST_MOCA_MEID;
            meId += (pptpMeId - GPON_FIRST_MOCA_MEID) * omciSys.numberOfDownstreamPriorityQueues;
            break;
        case MDMOID_PPTP_POTS_UNI:
            meId = GPON_FIRST_POTS_MEID;
            meId += (pptpMeId - GPON_FIRST_POTS_MEID) * omciSys.numberOfDownstreamPriorityQueues;
            break;
        case MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT:
            meId = GPON_FIRST_VEIP_MEID;
            meId += (pptpMeId - GPON_FIRST_VEIP_MEID) * omciSys.numberOfDownstreamPriorityQueues;
            break;
        default:
            break;
    }

    INIT_INSTANCE_ID_STACK(&iidStack);
    for(prio = 0; prio < GPON_PHY_DS_PQ_MAX; ++prio)
    {
        cmsLog_notice("Adding %s instance %d", objName, meId);
        _mdm_initPathDescriptor(&pathDesc);
        pathDesc.oid = mdmOid;
        PUSH_INSTANCE_ID(&(pathDesc.iidStack), gPrioQInst++);

        /* add new object instance */
        if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add %s, ret=%d", objName, ret);
            return ret;
        }

        /* get the object we just added */
        iidStack = pathDesc.iidStack;
        if ((ret = _mdm_getObject(mdmOid,
                                 &iidStack,
                                 (void **)&mdmObj)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to get %s, ret=%d", objName, ret);
            return ret;
        }

        /* Set the default values of the parameters */
        mdmObj->managedEntityId = meId++;

        /* PptpEthernetUni meId : 0xSSPP (SS=slotId; PP portId: range 1..255) */
        /* PptpMocaUni meId     : r0xSSPP (SS=slotId; PP=portId: range 1..255) */
        /* relatedPort          : 0xSSPPpppp (SS=slotId; PP=portId; pppp=priority: range 0x0000..0x0FFF) */
        if (_owapi_rut_tmctl_getQueueMap() == QID_PRIO_MAP_Q7P7)
        {
            mdmObj->relatedPort = (UINT32)(pptpMeId << 16) + (GPON_PHY_DS_PQ_MAX - 1 - prio);
        }
        else
        {
            mdmObj->relatedPort = (UINT32)(pptpMeId << 16) + prio;
        }

        // By default priority queue is pointed to TCont instead of Traffic Scheduler
        mdmObj->trafficSchedulerGPointer = 0;

        // By default weight is 1
        mdmObj->weight = 1;

        mdmObj->allocatedQueueSize= DS_PQ_MAX_SIZE;
        mdmObj->maxQueueSize = DS_PQ_MAX_SIZE;
        sprintf(buf, "%04x%04x%04x%04x",
          mdmObj->maxQueueSize, mdmObj->maxQueueSize,
          mdmObj->maxQueueSize, mdmObj->maxQueueSize);
        CMSMEM_REPLACE_STRING_FLAGS(mdmObj->packetDropQueueThresholds, buf, 0);

        /* set the object */
        ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to set %s, ret = %d", objName, ret);
            /* _mdm_setObject will steal the object on success, but on failure,
             * we need to free it.
             */
            _mdm_freeObject((void **) &mdmObj);
        }
    }

    *priQNum = GPON_PHY_DS_PQ_MAX;

    return ret;
}

static CmsRet addDefaultESCObject(void)
{
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    _ESCObject *mdmObj = NULL;
    const char *objName = "X_BROADCOM_COM_ESC";
    const UINT32 mdmOid = MDMOID_ESC;

    INIT_INSTANCE_ID_STACK(&iidStack);

    if ((ret = _mdm_getObject(mdmOid,
                             &iidStack,
                             (void **)&mdmObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get %s, ret=%d", objName, ret);
        return ret;
    }

    /* Set the default values of the parameters */
    mdmObj->managedEntityId = 0;

    /* TODO: Initialize encryption capabilities */

    /* set the object */
    ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to set %s, ret = %d", objName, ret);
        /* mdm_setObject will steal the object on success, but on failure,
         * we need to free it.
         */
        _mdm_freeObject((void **) &mdmObj);
    }

    return ret;
}


static CmsRet addDefaultMcastSubsMonObject(UINT32 oid, UINT32 meId)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    _BCM_MulticastSubscriberMonitorObject *mdmObj = NULL;
    const char *objName = "X_BROADCOM_COM_MulticastSubscriberMonitor";
    const UINT32 mdmOid = MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR;

    INIT_INSTANCE_ID_STACK(&iidStack);
    cmsLog_notice("Adding %s instance %d", objName, meId);
    _mdm_initPathDescriptor(&pathDesc);
    pathDesc.oid = mdmOid;
    PUSH_INSTANCE_ID(&(pathDesc.iidStack), gMcastSubsInst++);

    /* add new object instance */
    if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to add %s, ret=%d", objName, ret);
        return ret;
    }

    /* get the object we just added */
    iidStack = pathDesc.iidStack;
    if ((ret = _mdm_getObject(mdmOid,
                             &iidStack,
                             (void **)&mdmObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get %s, ret=%d", objName, ret);
        return ret;
    }

    /* Set the default values of the parameters */
    mdmObj->managedEntityId = meId;
    mdmObj->objectId = oid;

    /* set the object */
    ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to set %s, ret = %d", objName, ret);
        /* _mdm_setObject will steal the object on success, but on failure,
         * we need to free it.
         */
        _mdm_freeObject((void **) &mdmObj);
    }

    return ret;
}

#ifdef OMCI_TR69_DUAL_STACK
static CmsRet addDefaultGponTr69Object(UINT32 meId)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    _UniGObject *mdmObj = NULL;
    const char *objName = "Tr069ManagementServerObject";
    const UINT32 mdmOid = MDMOID_TR069_MANAGEMENT_SERVER;


    INIT_INSTANCE_ID_STACK(&iidStack);
    cmsLog_notice("Adding %s instance %d", objName, meId);
    _mdm_initPathDescriptor(&pathDesc);
    pathDesc.oid = mdmOid;
    PUSH_INSTANCE_ID(&(pathDesc.iidStack), gTr69Inst++);

    /* add new object instance */
    if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to add %s, ret=%d", objName, ret);
        return ret;
    }

    /* get the object we just added */
    iidStack = pathDesc.iidStack;
    if ((ret = _mdm_getObject(mdmOid,
                             &iidStack,
                             (void **)&mdmObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get %s, ret=%d", objName, ret);
        return ret;
    }

    /* Set the default values of the parameters */
    mdmObj->managedEntityId = meId;

    /* set the object */
    ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to set %s, ret = %d", objName, ret);
        /* _mdm_setObject will steal the object on success, but on failure,
         * we need to free it.
         */
        _mdm_freeObject((void **) &mdmObj);
    }

    return ret;
}
#endif

static CmsRet addDefaultGponVeipUniGObject(UINT32 *priQNumP)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 portNum = 0, meId = 0;
    UINT32 priQNum = 0;
    _PptpEthernetUniObject *mdmObj = NULL;
    const char *objName = "VirtualEthernetInterfacePointObject";
    const UINT32 mdmOid = MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT;

    *priQNumP = 0;
    INIT_INSTANCE_ID_STACK(&iidStack);
    for (portNum = 1; portNum <= omciSys.numberOfVeipPorts; portNum++)
    {
        // stop if portNum is greater than limitation
        if (portNum > GPON_PHY_VEIP_PORT_MAX)
            break;

        cmsLog_notice("Adding %s instance %d", objName, portNum);
        _mdm_initPathDescriptor(&pathDesc);
        pathDesc.oid = mdmOid;
        PUSH_INSTANCE_ID(&(pathDesc.iidStack), portNum);

        /* add new object instance */
        if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add %s, ret=%d", objName, ret);
            return ret;
        }

        /* get the object we just added */
        iidStack = pathDesc.iidStack;
        if ((ret = _mdm_getObject(mdmOid,
                                 &iidStack,
                                 (void **)&mdmObj)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to get %s, ret=%d", objName, ret);
            return ret;
        }

        /*
         * Set the default values of the parameters
         */

        /* CardHolder meId:      0x01SS (SS=slotId) */
        /* Veip meId: 0xSSPP (SS=slotId; PP portId, range 1..255) */
        mdmObj->managedEntityId = meId = omciSys.veipManagedEntityId1;

        /* set the object */
        ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to set %s, ret = %d", objName, ret);
            /* _mdm_setObject will steal the object on success, but on failure,
             * we need to free it.
             */
            _mdm_freeObject((void **) &mdmObj);
            return ret;
        }

        /* Note: cannot use mdmObj since it's stolen by _mdm_setObject */
        // create UniG Object that is associated with PPTP Ethernet
        ret = addDefaultGponUniGObject(meId);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to create UNI-G ME for %s meId %d (ret=%d)",
                         objName, meId, ret);
            return ret;
        }

#ifdef OMCI_TR69_DUAL_STACK
        // create TR-069 Object that is associated with Veip
        ret = addDefaultGponTr69Object(meId);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to create TR-069 ME for %s meId %d (ret=%d)",
                         objName, meId, ret);
            return ret;
        }
#endif

        // create X_BROADCOM_COM_MulticastSubscriberMonitor that is
        // associated with Virtual Ethernet Interface Point to monitor multicast
        // subscriber information
        ret = addDefaultMcastSubsMonObject(mdmOid, meId);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to create X_BROADCOM_COM_MulticastSubscriberMonitor ME for %s meId %d (ret=%d)",
                         objName, meId, ret);
            return ret;
        }

        ret = addDefaultDsGponPriorityQueueGObject(mdmOid, meId, &priQNum);
        *priQNumP += priQNum;
    }

    return ret;
}

static CmsRet addDefaultGponPptpEthernetUniGObject(UINT32 *priQNumP)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 portNum = 0, meId = 0;
    UINT32 priQNum = 0;
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
    OmciEthPortType_t eth;
    OmciEthPortType portType;
#endif
    _PptpEthernetUniObject *mdmObj = NULL;
    const char *objName = "PptpEthernetUniObject";
    const UINT32 mdmOid = MDMOID_PPTP_ETHERNET_UNI;

    *priQNumP = 0;
    INIT_INSTANCE_ID_STACK(&iidStack);
    if (omciSys.veipPptpUni0)
        portNum = 0;
    else
        portNum = 1;
    for (;portNum <= omciSys.numberOfEthernetPorts; portNum++)
    {
        // stop if portNum is greater than limitation
        if (portNum > GPON_PHY_ETH_PORT_MAX)
            break;

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
        // PPTP should be created only for ethernet port that is member of
        // ONT bridge only.
        // So do not add PPTP for ethernet port that is member of RG bridge only
        if (portNum >= 1)
        {
            eth.types.all = omciSys.ethernetTypes;
            portType = omciUtil_getPortType(portNum - 1, eth.types.all);
            if ((portType == OMCI_ETH_PORT_TYPE_RG) ||
              (portType == OMCI_ETH_PORT_TYPE_NONE))
            {
                continue;
            }
        }
#endif    /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

        cmsLog_notice("Adding %s instance %d", objName, portNum + omciSys.veipPptpUni0);
        _mdm_initPathDescriptor(&pathDesc);
        pathDesc.oid = mdmOid;
        PUSH_INSTANCE_ID(&(pathDesc.iidStack), portNum + omciSys.veipPptpUni0);

        /* add new object instance */
        if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add %s, ret=%d", objName, ret);
            return ret;
        }

        /* get the object we just added */
        iidStack = pathDesc.iidStack;
        if ((ret = _mdm_getObject(mdmOid,
                                 &iidStack,
                                 (void **)&mdmObj)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to get %s, ret=%d", objName, ret);
            return ret;
        }

        /*
         * Set the default values of the parameters
         */

        /* CardHolder meId:      0x01SS (SS=slotId) */
        /* PptpEthernetUni meId: 0xSSPP (SS=slotId; PP portId, range 1..255) */
        switch (portNum)
        {
            case 0: //1st eth uni as veip
                meId = omciSys.veipManagedEntityId1;
                break;
            case 1:
                meId = omciSys.ethernetManagedEntityId1;
                break;
            case 2:
                meId = omciSys.ethernetManagedEntityId2;
                break;
            case 3:
                meId = omciSys.ethernetManagedEntityId3;
                break;
            case 4:
                meId = omciSys.ethernetManagedEntityId4;
                break;
            case 5:
                meId = omciSys.ethernetManagedEntityId5;
                break;
            case 6:
                meId = omciSys.ethernetManagedEntityId6;
                break;
            case 7:
                meId = omciSys.ethernetManagedEntityId7;
                break;
            case 8:
                meId = omciSys.ethernetManagedEntityId8;
                break;
            case 9:
                meId = omciSys.ethernetManagedEntityId9;
                break;
            case 10:
                meId = omciSys.ethernetManagedEntityId10;
                break;
            case 11:
                meId = omciSys.ethernetManagedEntityId11;
                break;
            case 12:
                meId = omciSys.ethernetManagedEntityId12;
                break;
            case 13:
                meId = omciSys.ethernetManagedEntityId13;
                break;
            case 14:
                meId = omciSys.ethernetManagedEntityId14;
                break;
            case 15:
                meId = omciSys.ethernetManagedEntityId15;
                break;
            case 16:
                meId = omciSys.ethernetManagedEntityId16;
                break;
            case 17:
                meId = omciSys.ethernetManagedEntityId17;
                break;
            case 18:
                meId = omciSys.ethernetManagedEntityId18;
                break;
            case 19:
                meId = omciSys.ethernetManagedEntityId19;
                break;
            case 20:
                meId = omciSys.ethernetManagedEntityId20;
                break;
            case 21:
                meId = omciSys.ethernetManagedEntityId21;
                break;
            case 22:
                meId = omciSys.ethernetManagedEntityId22;
                break;
            case 23:
                meId = omciSys.ethernetManagedEntityId23;
                break;
            case 24:
                meId = omciSys.ethernetManagedEntityId24;
                break;
            default:
                break;
        }

        // Note: Generated PPTP ME IDs may not be continuous. For example, for
        // an ONU device has 4 physical ETH ports,
        // If the setting is
        // {veipPptpUni0=0: ETH0=RG, ETH1=ONT, ETH2=RG, ETH3=ONT}
        // PPTP ETH UNI MEs will be
        // {258(ETH1), 260(ETH3)}
        // If the setting is
        // {veipPptpUni0=1: ETH0=ONT, ETH1=RG, ETH2=RG, ETH3=ONT}
        // PPTP ETH UNI MEs will be
        // {257 (for ETH1 & ETH2), 258(ETH0), 261(ETH3)}
        // This approach simplifies the mapping between the actual Ethernet
        // port and PPTP ETH UNI ME instances, which may be used by IGMP, PM,
        // alarm, etc.
        mdmObj->managedEntityId = meId;

        /* set the object */
        ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to set %s, ret = %d", objName, ret);
            /* _mdm_setObject will steal the object on success, but on failure,
             * we need to free it.
             */
            _mdm_freeObject((void **) &mdmObj);
            return ret;
        }

        omciDm_addUniEntry(MDMOID_PPTP_ETHERNET_UNI, meId);

        /* Note: cannot use mdmObj since it's stolen by _mdm_setObject */
        // create UniG Object that is associated with PPTP Ethernet
        ret = addDefaultGponUniGObject(meId);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to create UNI-G ME for %s meId %d (ret=%d)",
                         objName, meId, ret);
            return ret;
        }

        // create X_BROADCOM_COM_MulticastSubscriberMonitor that is
        // associated with PPPTP Ethernet to monitor multicast
        // subscriber information
        ret = addDefaultMcastSubsMonObject(mdmOid, meId);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to create X_BROADCOM_COM_MulticastSubscriberMonitor ME for %s meId %d (ret=%d)",
                         objName, meId, ret);
            return ret;
        }

        ret = addDefaultDsGponPriorityQueueGObject(mdmOid, meId, &priQNum);
        *priQNumP += priQNum;
    }

    return ret;
}

static CmsRet addDefaultGponPptpMocaUniGObject(UINT32 *priQNumP)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 portNum = 0, meId = 0;
    UINT32 priQNum = 0;
    _PptpMocaUniObject *mdmObj = NULL;
    const char *objName = "PptpMocaUniObject";
    const UINT32 mdmOid = MDMOID_PPTP_MOCA_UNI;

    *priQNumP = 0;
    INIT_INSTANCE_ID_STACK(&iidStack);
    for (portNum = 1; portNum <= omciSys.numberOfMocaPorts; portNum++)
    {
        // stop if portNum is greater than limitation
        if (portNum > GPON_PHY_MOCA_PORT_MAX)
            break;

        cmsLog_notice("Adding %s instance %d", objName, portNum);
        _mdm_initPathDescriptor(&pathDesc);
        pathDesc.oid = mdmOid;
        PUSH_INSTANCE_ID(&(pathDesc.iidStack), portNum);

        /* add new object instance */
        if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add %s, ret=%d", objName, ret);
            return ret;
        }

        /* get the object we just added */
        iidStack = pathDesc.iidStack;
        if ((ret = _mdm_getObject(mdmOid,
                                 &iidStack,
                                 (void **)&mdmObj)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to get %s, ret=%d", objName, ret);
            return ret;
        }

        /*
         * Set the default values of the parameters
         */
        /* CardHolder meId:  0x01SS (SS=slotId) */
        /* PptpMocaUni meId: 0xSSPP (SS=slotId; PP=portId: range 1..255) */
        if (portNum == 1)
            meId = omciSys.mocaManagedEntityId1;
        else if (portNum == 2)
            meId = omciSys.mocaManagedEntityId2;

        mdmObj->managedEntityId = meId;

        /* set the object */
        ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to set %s, ret = %d", objName, ret);
            /* _mdm_setObject will steal the object on success, but on failure,
             * we need to free it.
             */
            _mdm_freeObject((void **) &mdmObj);
            return ret;
        }

        /* Note: cannot use mdmObj since it's stolen by _mdm_setObject */
        // create UniG Object that is associated with PPTP MoCA
        ret = addDefaultGponUniGObject(meId);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to create UNI-G ME for %s meId %d (ret=%d)",
                         objName, meId, ret);
            return ret;
        }

        // create X_BROADCOM_COM_MulticastSubscriberMonitor that is
        // associated with PPPTP MoCA to monitor multicast
        // subscriber information
        ret = addDefaultMcastSubsMonObject(mdmOid, meId);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to create X_BROADCOM_COM_MulticastSubscriberMonitor ME for %s meId %d (ret=%d)",
                         objName, meId, ret);
            return ret;
        }

        ret = addDefaultDsGponPriorityQueueGObject(mdmOid, meId, &priQNum);
        *priQNumP += priQNum;
    }

    return ret;
}

static CmsRet addDefaultVoIpLineStatusObject(UINT32 meId, UINT32 portNum)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    _UniGObject *mdmObj = NULL;
    const char *objName = "VoIpLineStatusObject";
    const UINT32 mdmOid = MDMOID_VO_IP_LINE_STATUS;

    INIT_INSTANCE_ID_STACK(&iidStack);
    cmsLog_notice("Adding %s instance %d", objName, meId);
    _mdm_initPathDescriptor(&pathDesc);
    pathDesc.oid = mdmOid;
    PUSH_INSTANCE_ID(&(pathDesc.iidStack), portNum);

    /* add new object instance */
    if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to add %s, ret=%d", objName, ret);
        return ret;
    }

    /* get the object we just added */
    iidStack = pathDesc.iidStack;
    if ((ret = _mdm_getObject(mdmOid,
                             &iidStack,
                             (void **)&mdmObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get %s, ret=%d", objName, ret);
        return ret;
    }

    /* Set the default values of the parameters */
    mdmObj->managedEntityId = meId;

    /* set the object */
    ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to set %s, ret = %d", objName, ret);
        /* _mdm_setObject will steal the object on success, but on failure,
         * we need to free it.
         */
        _mdm_freeObject((void **) &mdmObj);
    }

    return ret;
}

static CmsRet addDefaultGponPptpPotsUniGObject(void)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 portNum = 0, meId = 0;
    _PptpPotsUniObject *mdmObj = NULL;
    const char *objName = "PptpPotsUniObject";
    const UINT32 mdmOid = MDMOID_PPTP_POTS_UNI;

    INIT_INSTANCE_ID_STACK(&iidStack);
    for (portNum = 1; portNum <= omciSys.numberOfPotsPorts; portNum++)
    {
        // stop if portNum is greater than limitation
        if (portNum > GPON_PHY_POTS_PORT_MAX)
            break;

        cmsLog_notice("Adding %s instance %d", objName, portNum);
        _mdm_initPathDescriptor(&pathDesc);
        pathDesc.oid = mdmOid;
        PUSH_INSTANCE_ID(&(pathDesc.iidStack), portNum);

        /* add new object instance */
        if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add %s, ret=%d", objName, ret);
            return ret;
        }

        /* get the object we just added */
        iidStack = pathDesc.iidStack;
        if ((ret = _mdm_getObject(mdmOid,
                                 &iidStack,
                                 (void **)&mdmObj)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to get %s, ret=%d", objName, ret);
            return ret;
        }

        /*
         * Set the default values of the parameters
         */
        /* CardHolder meId:  0x01SS (SS=slotId) */
        /* PptpPotsUni meId: 0xSSPP (SS=slotId; PP=portId: range 1..255) */
        if (portNum == 1)
            meId = omciSys.potsManagedEntityId1;
        else if (portNum == 2)
            meId = omciSys.potsManagedEntityId2;

        mdmObj->managedEntityId = meId;

        /* set the object */
        ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to set %s, ret = %d", objName, ret);
            /* _mdm_setObject will steal the object on success, but on failure,
             * we need to free it.
             */
            _mdm_freeObject((void **) &mdmObj);
            return ret;
        }

        /* Note: cannot use mdmObj since it's stolen by _mdm_setObject */
        // create UniG Object that is associated with PPTP POTS
        ret = addDefaultGponUniGObject(meId);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to create UNI-G ME for %s meId %d (ret=%d)",
                         objName, meId, ret);
            return ret;
        }

        // create VoIP Line Status Object that is associated with PPTP POTS
        ret = addDefaultVoIpLineStatusObject(meId, portNum);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to create VoIP Linse Status ME for %s meId %d (ret=%d)",
                         objName, meId, ret);
            return ret;
        }
    }

    return ret;
}

static CmsRet addDefaultUsGponPriorityQueueGObject(UINT32 tcontMeId, UINT32 *priQNumP)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    _PriorityQueueGObject *mdmObj = NULL;
    const char *objName = "PriorityQueueGObject";
    const UINT32 mdmOid = MDMOID_PRIORITY_QUEUE_G;
    UINT16 meId = GPON_FIRST_TCONT_MEID; /* range of US queue ids is 0x8000 to 0xFFFF */
    UINT32 prio = 0;
    char buf[BUFLEN_32] = {0};

    *priQNumP = 0;
    /* tcont MeId starts at GPON_FIRST_TCONT_MEID */
    /* there're GPON_PHY_US_PQ_MAX priority queues per TCont */
    meId += (tcontMeId - GPON_FIRST_TCONT_MEID) * GPON_PHY_US_PQ_MAX;

    INIT_INSTANCE_ID_STACK(&iidStack);
    // By default there are GPON_PHY_US_PQ_MAX upstream Priority Queue per TCont
    for(prio = 0; prio < GPON_PHY_US_PQ_MAX; ++prio)
    {
        cmsLog_notice("Adding %s instance %d", objName, meId);
        _mdm_initPathDescriptor(&pathDesc);
        pathDesc.oid = mdmOid;
        PUSH_INSTANCE_ID(&(pathDesc.iidStack), gPrioQInst++);

        /* add new object instance */
        if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add %s, ret=%d", objName, ret);
            return ret;
        }

        /* get the object we just added */
        iidStack = pathDesc.iidStack;
        if ((ret = _mdm_getObject(mdmOid,
                                 &iidStack,
                                 (void **)&mdmObj)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to get %s, ret=%d", objName, ret);
            return ret;
        }

        /* Set the default values of the parameters */
        mdmObj->managedEntityId = meId++;

        // the first 2 bytes are the meId of the associated T-CONT
        // the last 2 bytes represent the priority of this queue (0 to 0x0FFF)
        /* By default there are GPON_PHY_US_PQ_MAX upstream Priority Queue per TCont */
        /* tcontMeId   : 0xSSPP (SS=slotId: 0x80; PP portId: range 0..255) */
        /* relatedPort : 0xSSPPpppp (SS=slotId; PP=portId; pppp=priority: range 0x0000..0x0FFF) */
        if (_owapi_rut_tmctl_getQueueMap() == QID_PRIO_MAP_Q7P7)
        {
            mdmObj->relatedPort = ((tcontMeId << 16) & 0xFFFF0000) + (GPON_PHY_US_PQ_MAX - 1 - prio);
        }
        else
        {
            mdmObj->relatedPort = ((tcontMeId << 16) & 0xFFFF0000) + prio;
        }

        // By default priority queue is pointed to TCont instead of Traffic Scheduler
        mdmObj->trafficSchedulerGPointer = 0;

        // By default weight is 1
        mdmObj->weight = 1;
        mdmObj->allocatedQueueSize = US_PQ_MAX_SIZE;
        mdmObj->maxQueueSize = US_PQ_MAX_SIZE;
        sprintf(buf, "%04x%04x%04x%04x",
          mdmObj->maxQueueSize, mdmObj->maxQueueSize,
          mdmObj->maxQueueSize, mdmObj->maxQueueSize);
        CMSMEM_REPLACE_STRING_FLAGS(mdmObj->packetDropQueueThresholds, buf, 0);

        /* set the object */
        ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to set %s, ret = %d", objName, ret);
            /* _mdm_setObject will steal the object on success, but on failure,
             * we need to free it.
             */
            _mdm_freeObject((void **) &mdmObj);
        }
    }

    *priQNumP = GPON_PHY_US_PQ_MAX;
    return ret;
}

static CmsRet addDefaultUsGponTrafficSchedulerGObject(UINT32 tcontMeId, UINT32 *tsNumP)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    _TrafficSchedulerGObject *mdmObj = NULL;
    const char *objName = "TrafficSchedulerGObject";
    const UINT32 mdmOid = MDMOID_TRAFFIC_SCHEDULER_G;
    UINT16 meId = 0x8000; /* range of US queue ids is 0x8000 to 0xFFFF */
    UINT32 i = 0;

    *tsNumP = 0;
    /* tcont MeId starts at GPON_FIRST_TCONT_MEID */
    /* there're GPON_PHY_US_TS_MAX priority queues per TCont */
    meId += (tcontMeId - GPON_FIRST_TCONT_MEID) * GPON_PHY_US_TS_MAX;

    INIT_INSTANCE_ID_STACK(&iidStack);
    // By default there are GPON_PHY_US_PQ_MAX upstream Priority Queue per TCont
    for(i = 0; i < GPON_PHY_US_TS_MAX; ++i)
    {
        cmsLog_notice("Adding %s instance %d", objName, meId);
        _mdm_initPathDescriptor(&pathDesc);
        pathDesc.oid = mdmOid;
        PUSH_INSTANCE_ID(&(pathDesc.iidStack), gTrSchInst++);

        /* add new object instance */
        if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add %s, ret=%d", objName, ret);
            return ret;
        }

        /* get the object we just added */
        iidStack = pathDesc.iidStack;
        if ((ret = _mdm_getObject(mdmOid,
                                 &iidStack,
                                 (void **)&mdmObj)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to get %s, ret=%d", objName, ret);
            return ret;
        }

        /* Set the default values of the parameters */
        mdmObj->managedEntityId = meId++;

        /* Set the default values of the parameters */
        mdmObj->TContPointer = tcontMeId;

        // There is no hierachy Traffic Scheduler G
        mdmObj->trafficSchedulerGPointer = 0;

        // Traffic Scheduler G policy is weighted round robin (WRR = 2)
        mdmObj->policy = OMCI_SCHEDULE_POLICY_WRR;

        // By default priority is 0 (this value is configurable later by OLT)
        mdmObj->priority = 0;

        /* set the object */
        ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to set %s, ret = %d", objName, ret);
            /* _mdm_setObject will steal the object on success, but on failure,
             * we need to free it.
             */
            _mdm_freeObject((void **) &mdmObj);
        }
    }

    *tsNumP = GPON_PHY_US_TS_MAX;

    return ret;
}

static CmsRet addDefaultGponTContObject(UINT32 *priQNumP, UINT32 *tsNumP)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 objInst;
    UINT32 meId;
    UINT32 priQNum = 0;
    UINT32 tsNum = 0;
    _TContObject *mdmObj = NULL;
    const char *objName = "TContObject";
    const UINT32 mdmOid = MDMOID_T_CONT;

    *priQNumP = 0;
    *tsNumP = 0;

    INIT_INSTANCE_ID_STACK(&iidStack);
    for (objInst = 0; objInst < omciSys.numberOfTConts; objInst++)
    {
        // stop if objInst is greater than limitation
        if (objInst >= GPON_TCONT_MAX)
            break;

        cmsLog_notice("Adding %s instance %d", objName, objInst);
        _mdm_initPathDescriptor(&pathDesc);
        pathDesc.oid = mdmOid;
        PUSH_INSTANCE_ID(&(pathDesc.iidStack), objInst + 1);

        /* add new object instance */
        if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add %s, ret=%d", objName, ret);
            return ret;
        }

        /* get the object we just added */
        iidStack = pathDesc.iidStack;
        if ((ret = _mdm_getObject(mdmOid,
                                 &iidStack,
                                 (void **)&mdmObj)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to get %s, ret=%d", objName, ret);
            return ret;
        }

        /*
         * Set the default values of the parameters
         */
        meId = omciSys.tcontManagedEntityId + objInst;
        mdmObj->managedEntityId = meId;

        // if bit in tcontPolicies is 1 then tcont's policy Weighted Round Robin (WRR = 2)
        if (((omciSys.tcontPolicies >> (meId - GPON_FIRST_TCONT_MEID)) & 0x00000001) == 1)
            mdmObj->policy = OMCI_SCHEDULE_POLICY_WRR;
        // if bit in tcontPolicies is 0 then tcont's policy Head of Line Queueing (HOL = 1)
        else
            mdmObj->policy = OMCI_SCHEDULE_POLICY_HOL;

        /* set the object */
        ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to set %s, ret = %d", objName, ret);
            /* _mdm_setObject will steal the object on success, but on failure,
             * we need to free it.
             */
            _mdm_freeObject((void **) &mdmObj);
            return ret;
        }

        // create the associated Priority Queue G
        ret = addDefaultUsGponPriorityQueueGObject(meId, &priQNum);

        // create the associated Traffic Scheduler G
        ret = addDefaultUsGponTrafficSchedulerGObject(meId, &tsNum);

        *priQNumP += priQNum;
        *tsNumP += tsNum;
    }

    // create AniG Object for gpon interface that has only 1 port
    ret = addDefaultGponAniGObject();

    return ret;
}

static void getCircuitPackInfo(CircuitPackObject *cp)
{
    cp->totalTcontBufferNumber = 0;
    cp->totalTrafficSchedulerNumber = 0;
}

static CmsRet addDefaultCircuitPackObject(UINT32 cardHolderMeId, UINT32 objInst)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    _CircuitPackObject *mdmObj = NULL;
    const char *objName = "CircuitPackObject";
    const UINT32 mdmOid = MDMOID_CIRCUIT_PACK;
    UINT32 priQNum = 0;
    UINT32 tsNum = 0;

    cmsLog_notice("Adding %s instance %d", objName, (cardHolderMeId & 0x00FF));

    switch (cardHolderMeId)
    {
        case GPON_CARD_HOLDER_ETH_MEID:
            /* create PPTP objects for ethernet virtual circuit pack type */
            ret = addDefaultGponPptpEthernetUniGObject(&priQNum);
            break;
        case GPON_CARD_HOLDER_MOCA_MEID:
            /* create PPTP objects for moca virtual circuit pack type */
            ret = addDefaultGponPptpMocaUniGObject(&priQNum);
            break;
        case GPON_CARD_HOLDER_POTS_MEID:
            /* create PPTP objects for voice virtual circuit pack type */
            ret = addDefaultGponPptpPotsUniGObject();
            break;
        case GPON_CARD_HOLDER_VEIP_MEID:
            /* create VEIP objects for VEIP virtual circuit pack type */
            ret = addDefaultGponVeipUniGObject(&priQNum);
            break;
        case GPON_CARD_HOLDER_GPON_MEID:
            /* create TCONT objects for gpon virtual circuit pack type */
            ret = addDefaultGponTContObject(&priQNum, &tsNum);
            break;
    }

    INIT_INSTANCE_ID_STACK(&iidStack);
    _mdm_initPathDescriptor(&pathDesc);
    pathDesc.oid = mdmOid;
    PUSH_INSTANCE_ID(&(pathDesc.iidStack), objInst + 1);

    /* add new object instance */
    if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to add %s, ret=%d", objName, ret);
        return ret;
    }

    /* get the object we just added */
    iidStack = pathDesc.iidStack;
    if ((ret = _mdm_getObject(mdmOid,
                             &iidStack,
                             (void **)&mdmObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get %s, ret=%d", objName, ret);
        return ret;
    }

    /* Set the default values of the parameters */
    mdmObj->managedEntityId = cardHolderMeId;
    getCircuitPackInfo(mdmObj);

    switch (mdmObj->managedEntityId)
    {
        case GPON_CARD_HOLDER_ETH_MEID:
            mdmObj->type = 47;    // 10/100/1000 Base Tx Ethernet LAN IF
            mdmObj->numberOfPorts = omciSys.numberOfEthernetPorts + omciSys.veipPptpUni0;
            mdmObj->totalPriorityQueueNumber = priQNum;
            break;
        case GPON_CARD_HOLDER_MOCA_MEID:
            mdmObj->type = 46;    // MoCA
            mdmObj->numberOfPorts = omciSys.numberOfMocaPorts;
            mdmObj->totalPriorityQueueNumber = priQNum;
            break;
        case GPON_CARD_HOLDER_POTS_MEID:
            mdmObj->type = 32;   // POTS
            mdmObj->numberOfPorts = omciSys.numberOfPotsPorts;
            mdmObj->totalPriorityQueueNumber = 0;
            break;
        case GPON_CARD_HOLDER_VEIP_MEID:
            mdmObj->type = 48;   // VEIP
            mdmObj->numberOfPorts = omciSys.numberOfVeipPorts;
            mdmObj->totalPriorityQueueNumber = priQNum;
            break;
        case GPON_CARD_HOLDER_GPON_MEID:
            mdmObj->type = 248;   // GPON24881244
            mdmObj->numberOfPorts = GPON_PHY_GPON_PORT_MAX;
            mdmObj->totalTcontBufferNumber = omciSys.numberOfTConts;
            mdmObj->totalPriorityQueueNumber = priQNum;
            mdmObj->totalTrafficSchedulerNumber = tsNum;
            break;
    }

    /* set the object */
    ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to set %s, ret = %d", objName, ret);
        /* _mdm_setObject will steal the object on success, but on failure,
         * we need to free it.
         */
        _mdm_freeObject((void **) &mdmObj);
        return ret;
    }

    return ret;
}

static CmsRet addDefaultCardHolderObject(void)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 objInst = 0;
    UINT32 lastInst = 0;
    UINT32 objInstNbr[GPON_CARD_HOLDER_NUM_MAX] = {0, 0, 0, 0, 0};
    _CardHolderObject *mdmObj = NULL;
    const char *objName = "CardHolderObject";
    const UINT32 mdmOid = MDMOID_CARD_HOLDER;

    objInstNbr[lastInst++] = GPON_CARD_HOLDER_ETH_MEID;
#if defined(DMP_X_BROADCOM_COM_MOCALAN_1)
    objInstNbr[lastInst++] = GPON_CARD_HOLDER_MOCA_MEID;
#endif     /* DMP_X_BROADCOM_COM_MOCALAN_1 */
#if defined(DMP_X_ITU_ORG_VOICE_1)
    objInstNbr[lastInst++] = GPON_CARD_HOLDER_POTS_MEID;
#endif     /* DMP_X_ITU_ORG_VOICE_1 */
#if defined(DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1)
    objInstNbr[lastInst++] = GPON_CARD_HOLDER_VEIP_MEID;
#endif     /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */
    objInstNbr[lastInst++] = GPON_CARD_HOLDER_GPON_MEID;

    INIT_INSTANCE_ID_STACK(&iidStack);
    ret = _mdm_getNextObject(mdmOid, &iidStack, (void **)&mdmObj);
    if (ret != CMSRET_SUCCESS)
    {
        for (objInst = 0; objInst < lastInst; objInst++)
        {
#if defined(DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1)
            //run pptp uni#0 as veip, skip to create veip ME
            if (omciSys.veipPptpUni0 && objInstNbr[objInst] == GPON_CARD_HOLDER_VEIP_MEID)
                continue;
#endif
            cmsLog_notice("Adding %s instance %d", objName, objInst);

            _mdm_initPathDescriptor(&pathDesc);
            pathDesc.oid = mdmOid;
            PUSH_INSTANCE_ID(&(pathDesc.iidStack), objInst + 1);

            /* add new object instance */
            if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to add %s, ret=%d", objName, ret);
                return ret;
            }

            /* get the object we just added */
            iidStack = pathDesc.iidStack;
            if ((ret = _mdm_getObject(MDMOID_CARD_HOLDER,
                                     &iidStack,
                                     (void **)&mdmObj)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to get %s, ret=%d", objName, ret);
                return ret;
            }

            /*
             * Set the default values of the parameters
             */
            mdmObj->managedEntityId = objInstNbr[objInst];
            switch (mdmObj->managedEntityId)
            {
                case GPON_CARD_HOLDER_ETH_MEID:
                    mdmObj->actualPlugInUnitType = 47;    // 10/100/1000 Base Tx Ethernet LAN IF
                    mdmObj->expectedPlugInUnitType = 47;    // 10/100/1000 Base Tx Ethernet LAN IF
                    mdmObj->expectedPortCount = omciSys.numberOfEthernetPorts;
                    break;
                case GPON_CARD_HOLDER_MOCA_MEID:
                    mdmObj->actualPlugInUnitType = 46;    // MoCA
                    mdmObj->expectedPlugInUnitType = 46;    // MoCA
                    mdmObj->expectedPortCount = omciSys.numberOfMocaPorts;
                    break;
                case GPON_CARD_HOLDER_POTS_MEID:
                    mdmObj->actualPlugInUnitType = 32;    // POTS
                    mdmObj->expectedPlugInUnitType = 32;    // POTS
                    mdmObj->expectedPortCount = omciSys.numberOfPotsPorts;
                    break;
                case GPON_CARD_HOLDER_VEIP_MEID:
                    mdmObj->actualPlugInUnitType = 48;    // VEIP
                    mdmObj->expectedPlugInUnitType = 48;    // VEIP
                    mdmObj->expectedPortCount = omciSys.numberOfVeipPorts;
                    break;
                case GPON_CARD_HOLDER_GPON_MEID:
                    mdmObj->actualPlugInUnitType = 248;   // GPON24881244
                    mdmObj->expectedPlugInUnitType = 248;   // GPON24881244
                    mdmObj->expectedPortCount = GPON_PHY_GPON_PORT_MAX;
                    break;
            }

            /* set the object */
            ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to set %s, ret = %d", objName, ret);
                /* _mdm_setObject will steal the object on success, but on failure,
                 * we need to free it.
                 */
                _mdm_freeObject((void **) &mdmObj);
                return ret;
            }

            /* add default circuit pack corresponding with card holder */
            ret = addDefaultCircuitPackObject(objInstNbr[objInst], objInst);
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to add default circuit pack %d, ret = %d",
                             objInstNbr[objInst], ret);
                return ret;
            }
        }
    }
    else
    {
        _mdm_freeObject((void **)&mdmObj);
    }

    return CMSRET_SUCCESS;
}

static CmsRet addDefaultGponSoftwareImageObject(void)
{
_MdmPathDescriptor pathDesc;
InstanceIdStack iidStack;
CmsRet ret;
UINT32 objInst;
_SoftwareImageObject *mdmObj = NULL;

int AsciiStrLen, Lcv;
char AsciiVerStr[SWIMAGE_VER_STR_LEN + 1] = "";
char hexVerStr[SWIMAGE_VER_HEX_LEN + 1] = "";
int BootPartition = devCtl_getBootedImagePartition();
int BootState = devCtl_getImageState();
UINT32 BootedPartition = 1, NextBootPartition = 1;

struct
{
    int IsActive;
    int IsCommitted;
    int IsValid;
} SwImageAttributes[2] = {{FALSE,FALSE,FALSE},{FALSE,FALSE,FALSE}};

char DEFAULT_VERSION_STRING[2][SWIMAGE_VER_HEX_LEN + 1] = {OMCI_SW_IMAGE_0_VERSION, OMCI_SW_IMAGE_1_VERSION};


    //////////////////////////////////////////////////////////////////////////////
    // use boot state and booted partition values to preset attribute values for both MEs.
    switch (BootPartition)
    {
        case BOOTED_PART1_IMAGE:
        {
            SwImageAttributes[0].IsActive  = TRUE;              // must be true because we are booted to this image

            if (BOOT_SET_PART1_IMAGE == BootState)
            {
                // boot image == boot state
                SwImageAttributes[0].IsCommitted    = TRUE;     // must be true since bootstate points to this image for the next boot
                SwImageAttributes[0].IsValid        = TRUE;     // must be true since we are booting this image at all
            }
            else
            {
                NextBootPartition = 2;
                SwImageAttributes[0].IsValid        = TRUE;     // must be true since we are booted to this image
                SwImageAttributes[1].IsCommitted    = TRUE;     // must be true because boot state points to it for the next boot
                SwImageAttributes[1].IsValid        = TRUE;     // must be true because boot state points to it for the next boot
            }

            break;
        }
        case BOOTED_PART2_IMAGE:
        {
            BootedPartition = 2;
            SwImageAttributes[1].IsActive  = TRUE;              // must be true because we are booted to this image

            if (BOOT_SET_PART2_IMAGE == BootState)
            {
                NextBootPartition = 2;
                // boot image == boot state
                SwImageAttributes[1].IsCommitted    = TRUE;     // must be true since bootstate points to this image for the next boot
                SwImageAttributes[1].IsValid        = TRUE;     // must be true since we are booting this image at all
            }
            else
            {
                SwImageAttributes[1].IsValid        = TRUE;     // must be true since we are booted to this image
                SwImageAttributes[0].IsCommitted    = TRUE;     // must be true because boot state points to it for the next boot
                SwImageAttributes[0].IsValid        = TRUE;     // must be true because boot state points to it for the next boot
            }
            break;
        }
        default:
        {
            cmsLog_error("Unknown booted partition value %x, failed to set default attribute for sw image MEs \n", BootPartition);
        }
    }

    printf("Active partition = %d, Next partition = %d, BootState=%d\n", BootedPartition, NextBootPartition, BootState);

    //////////////////////////////////////////////////////////////////////////////
    // Create new software image MEs if needed
    INIT_INSTANCE_ID_STACK(&iidStack);
    ret = _mdm_getNextObject(MDMOID_SOFTWARE_IMAGE, &iidStack, (void **)&mdmObj);
    if (ret != CMSRET_SUCCESS)
    {
        // first object doesn't exist so recreate both...
        for(objInst=0; objInst<=1; objInst++)
        {
            cmsLog_notice("Adding %s instance %d", "SoftwareImageObject", objInst);
            _mdm_initPathDescriptor(&pathDesc);
            pathDesc.oid = MDMOID_SOFTWARE_IMAGE;
            PUSH_INSTANCE_ID(&(pathDesc.iidStack), objInst + 1);

            if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to add %s, ret=%d", "SoftwareImageObject", ret);
                return ret;
            }
            /* now get the object we have just created */
            iidStack = pathDesc.iidStack;
            if ((ret = _mdm_getObject(MDMOID_SOFTWARE_IMAGE, &iidStack, (void **)&mdmObj)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to get %s, ret=%d", "SoftwareImageObject", ret);
                return ret;
            }

            //now set default values
            mdmObj->isCommitted  = 0;
            mdmObj->isActive     = 0;
            mdmObj->isValid      = 0;

            /* set the object */
            ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to set %s, ret = %d", "SoftwareImageObject", ret);
                /* _mdm_setObject will steal the object on success, but on failure,
                 * we need to free it.
                 */
                _mdm_freeObject((void **) &mdmObj);
                return ret;
            }
        }
    }
    else
    {
       _mdm_freeObject((void **) &mdmObj);
    }

    //////////////////////////////////////////////////////////////////////////////
    // Get each of the software image MEs and set the attribute and version string values

    INIT_INSTANCE_ID_STACK(&iidStack);
    for(objInst=0; objInst<=1; objInst++)
    {
        if ((ret = _mdm_getNextObject(MDMOID_SOFTWARE_IMAGE, &iidStack, (void **)&mdmObj)) == CMSRET_SUCCESS)
        {
            // set all attribute values for instance 0 on partition 1
            mdmObj->isActive        = SwImageAttributes[objInst].IsActive;
            mdmObj->isCommitted     = SwImageAttributes[objInst].IsCommitted;
            mdmObj->isValid         = mdmObj->isValid || SwImageAttributes[objInst].IsValid;
            mdmObj->managedEntityId = objInst;

            memset(&AsciiVerStr, 0x0, sizeof(AsciiVerStr));
            memset(&hexVerStr, 0x0, sizeof(hexVerStr));
            if (0 != (AsciiStrLen = devCtl_getImageVersion(objInst+1, AsciiVerStr, SWIMAGE_VER_STR_LEN)))
            {
                for (Lcv=0; Lcv < AsciiStrLen; Lcv++)
                {
                    sprintf(hexVerStr+2*Lcv, "%.2X",AsciiVerStr[Lcv]);
                }
                printf("ASCII version string %s extracted from software image %d\n"
                  "Hexadecimal version string %s added to software image ME %d\n",
                  AsciiVerStr, objInst, hexVerStr, objInst);
            }
            else
            {
                printf("ASCII version string NOT extracted from software image %d\n"
                  "Default hexadecimal version string %s added to software image ME %d\n",
                  objInst, DEFAULT_VERSION_STRING[objInst], objInst);
                memcpy(hexVerStr, DEFAULT_VERSION_STRING[objInst], SWIMAGE_VER_HEX_LEN);
            }

            CMSMEM_REPLACE_STRING_FLAGS(mdmObj->version, hexVerStr, 0);

            ret = _mdm_setObject((void **)&mdmObj, &iidStack, FALSE);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to set Software Image Object, ret=%d", ret);
               /* _mdm_setObject will steal the object on success, but on failure,
                * we need to free it.
                */
               _mdm_freeObject((void **) &mdmObj);
            }
        }
        else
        {
            cmsLog_error("Failed to get %s %d, ret = %d", "SoftwareImageObject", objInst, ret);
            return ret;
        }
    }

    // to avoid getting extra MDM lock at boot up,
    // do not need to save Software Image to flash here
    //cmsMgm_saveConfigToFlash();

    return CMSRET_SUCCESS;
}

#ifdef DMP_X_ITU_ORG_VOICE_1
static CmsRet addDefaultGponVoiceObject(void)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret;
    _VoIpConfigDataObject *mdmObj = NULL;
    const char *objName = "VoIpConfigDataObject";
    const UINT32 mdmOid = MDMOID_VO_IP_CONFIG_DATA;

    INIT_INSTANCE_ID_STACK(&iidStack);
    ret = _mdm_getNextObject(mdmOid, &iidStack, (void **)&mdmObj);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_notice("Adding %s instance 1", objName);
        _mdm_initPathDescriptor(&pathDesc);
        pathDesc.oid = mdmOid;
        PUSH_INSTANCE_ID(&(pathDesc.iidStack), 1);

        /* add new object instance */
        if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add %s, ret=%d", objName, ret);
            return ret;
        }
    }
    else
    {
        _mdm_freeObject((void **)&mdmObj);
    }

    return CMSRET_SUCCESS;
}

#if defined DMP_X_ITU_ORG_VOICE_SIP_1
static CmsRet addSipConfigPortalObject(void)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    _MdmPathDescriptor pathDesc;
    char *sipPortalObjName = "SipConfigPortalObject";
    UINT32 sipPortalMdmOid = MDMOID_SIP_CONFIG_PORTAL;
    CmsRet ret = CMSRET_SUCCESS;

    INIT_INSTANCE_ID_STACK(&iidStack);
    cmsLog_notice("Adding %s instance %d", sipPortalObjName, 0);
    _mdm_initPathDescriptor(&pathDesc);
    pathDesc.oid = sipPortalMdmOid;
    PUSH_INSTANCE_ID(&(pathDesc.iidStack), 1);

    if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to add %s, ret=%d", sipPortalObjName, ret);
    }
    return ret;
}
#endif /* DMP_X_ITU_ORG_VOICE_SIP_1 */

#if defined (DMP_X_ITU_ORG_VOICE_MGC_1)
static CmsRet addMgcConfigPortalObject(void)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    _MdmPathDescriptor pathDesc;
    char *mgcPortalObjName = "MgcConfigPortalObject";
    UINT32 mgcPortalMdmOid = MDMOID_MGC_CONFIG_PORTAL;
    CmsRet ret = CMSRET_SUCCESS;

    INIT_INSTANCE_ID_STACK(&iidStack);
    cmsLog_notice("Adding %s instance %d", mgcPortalObjName, 0);
    _mdm_initPathDescriptor(&pathDesc);
    pathDesc.oid = mgcPortalMdmOid;
    PUSH_INSTANCE_ID(&(pathDesc.iidStack), 1);

    if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to add %s, ret=%d", mgcPortalObjName, ret);
    }
    return ret;
}
#endif /* DMP_X_ITU_ORG_VOICE_MGC_1 */

#endif /* DMP_X_ITU_ORG_VOICE_1 */

#if defined(DMP_X_BROADCOM_COM_MOCALAN_1)

static CmsRet addDefaultGponBrcmPptpMocaUniGObject(void)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret;
    _PptpMocaUniObject *mdmObj = NULL;
    const char *objName = "BrcmPptpMocaUniObject";
    const UINT32 mdmOid = MDMOID_BRCM_PPTP_MOCA_UNI;

    INIT_INSTANCE_ID_STACK(&iidStack);
    ret = _mdm_getNextObject(mdmOid, &iidStack, (void **)&mdmObj);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_notice("Adding %s instance 1", objName);
        _mdm_initPathDescriptor(&pathDesc);
        pathDesc.oid = mdmOid;
        PUSH_INSTANCE_ID(&(pathDesc.iidStack), 1);

        /* add new object instance */
        if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add %s, ret=%d", objName, ret);
            return ret;
        }
    }
    else
    {
        _mdm_freeObject((void **)&mdmObj);
    }

    return CMSRET_SUCCESS;
}


static CmsRet addDefaultGponMocaStatusObject(void)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret;
    _PptpMocaUniObject *mdmObj = NULL;
    const char *objName = "MocaStatusObject";
    const UINT32 mdmOid = MDMOID_MOCA_STATUS;

    INIT_INSTANCE_ID_STACK(&iidStack);
    ret = _mdm_getNextObject(mdmOid, &iidStack, (void **)&mdmObj);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_notice("Adding %s instance 1", objName);
        _mdm_initPathDescriptor(&pathDesc);
        pathDesc.oid = mdmOid;
        PUSH_INSTANCE_ID(&(pathDesc.iidStack), 1);

        /* add new object instance */
        if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add %s, ret=%d", objName, ret);
            return ret;
        }
    }
    else
    {
        _mdm_freeObject((void **)&mdmObj);
    }

    return CMSRET_SUCCESS;
}


static CmsRet addDefaultGponMocaStatsObject(void)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret;
    _PptpMocaUniObject *mdmObj = NULL;
    const char *objName = "MocaStatsObject";
    const UINT32 mdmOid = MDMOID_MOCA_STATS;

    INIT_INSTANCE_ID_STACK(&iidStack);
    ret = _mdm_getNextObject(mdmOid, &iidStack, (void **)&mdmObj);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_notice("Adding %s instance 1", objName);
        _mdm_initPathDescriptor(&pathDesc);
        pathDesc.oid = mdmOid;
        PUSH_INSTANCE_ID(&(pathDesc.iidStack), 1);

        /* add new object instance */
        if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add %s, ret=%d", objName, ret);
            return ret;
        }
    }
    else
    {
        _mdm_freeObject((void **)&mdmObj);
    }

    return CMSRET_SUCCESS;
}

#endif /* DMP_X_BROADCOM_COM_MOCALAN_1 */

// addDefaultGponIpHostObject don't use mdm_XXX() functions
// but uses _cmsObj_XXX() functions since mdm_XXX() functions
// do not invoke rcl_ipHostConfigDataObject() function
static CmsRet addDefaultGponIpHostObject(void)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;
    IpHostConfigDataObject *mdmObj = NULL;
    const char *objName = "IpHostConfigDataObject";
    const UINT32 mdmOid = MDMOID_IP_HOST_CONFIG_DATA;
    UINT32 i = 0;
#if (defined(OMCI_TR69_DUAL_STACK) && !defined(DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1))
    UINT32 meId = 0;
#endif
    UINT8 macNum[MAC_ADDR_LEN];
    UINT8 hostName[25];
    char *hexStr=NULL;

    memset(hostName, 0, sizeof(hostName));

    INIT_INSTANCE_ID_STACK(&iidStack);
    ret = _mdm_getNextObject(mdmOid, &iidStack, (void **)&mdmObj);
    if (ret != CMSRET_SUCCESS)
    {
        for (i = 1; i <= GPON_IP_HOST_MAX; i++)
        {
            cmsLog_notice("Adding %s instance %d", objName, i);
            INIT_INSTANCE_ID_STACK(&iidStack);
            PUSH_INSTANCE_ID(&iidStack, i);

            if ((ret = _cmsObj_addInstance(mdmOid, &iidStack)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to add %s, ret=%d", objName, ret);
                return ret;
            }
            if ((ret = _cmsObj_get(mdmOid, &iidStack, 0, (void **) &mdmObj)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to get %s, ret=%d", objName, ret);
                return ret;
            }

            /* Set the default values of the parameters */
            mdmObj->managedEntityId = i;

            if ((ret = devCtl_getBaseMacAddress(macNum)) == CMSRET_SUCCESS)
            {
                if ((ret = cmsUtl_binaryBufToHexString(macNum, 6, &hexStr)) == CMSRET_SUCCESS)
                {
                    CMSMEM_REPLACE_STRING_FLAGS(mdmObj->macAddress, hexStr, 0);
                    CMSMEM_FREE_BUF_AND_NULL_PTR(hexStr);
                }
            }
            /* set hostname and domain*/
            if ((ret = cmsUtl_binaryBufToHexString(hostName, sizeof(hostName), &hexStr)) == CMSRET_SUCCESS)
            {
                CMSMEM_REPLACE_STRING_FLAGS(mdmObj->hostName, hexStr, 0);
                CMSMEM_REPLACE_STRING_FLAGS(mdmObj->domainName, hexStr, 0);
                CMSMEM_FREE_BUF_AND_NULL_PTR(hexStr);
            }

            if ((ret = _cmsObj_set(mdmObj, &iidStack)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to set %s, ret=%d", objName, ret);
            }

            /* _cmsObj_set does not steal obj, so caller must always free */
            _cmsObj_free((void **) &mdmObj);

#if (defined(OMCI_TR69_DUAL_STACK) && !defined(DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1))
            /* Set the default values of the parameters */
            meId = i;

            // create TR-069 Object that is associated with IpHost
            ret = addDefaultGponTr69Object(meId);
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to create TR-069 ME for %s meId %d (ret=%d)",
                             objName, meId, ret);
                return ret;
            }
#endif
        }
    }
    else
    {
        _mdm_freeObject((void **)&mdmObj);
    }

    return ret;
}

#ifdef SUPPORT_IPV6

// addDefaultGponIpv6HostObject don't use mdm_XXX() functions
// but uses _cmsObj_XXX() functions since mdm_XXX() functions
// do not invoke rcl_ipv6HostConfigDataObject() function
static CmsRet addDefaultGponIpv6HostObject(void)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;
    IpHostConfigDataObject *mdmObj = NULL;
    const char *objName = "Ipv6HostConfigDataObject";
    const UINT32 mdmOid = MDMOID_IPV6_HOST_CONFIG_DATA;
    UINT32 i = 0;
    UINT8 macNum[MAC_ADDR_LEN];
    char *hexStr;

    ret = _mdm_getNextObject(mdmOid, &iidStack, (void **)&mdmObj);
    if (ret != CMSRET_SUCCESS)
    {
        for (i = 1; i <= GPON_IPV6_HOST_MAX; i++)
        {
            cmsLog_notice("Adding %s instance %d", objName, i);
            INIT_INSTANCE_ID_STACK(&iidStack);
            PUSH_INSTANCE_ID(&iidStack, i);

            if ((ret = _cmsObj_addInstance(mdmOid, &iidStack)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to add %s, ret=%d", objName, ret);
                return ret;
            }
            if ((ret = _cmsObj_get(mdmOid, &iidStack, 0, (void **) &mdmObj)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to get %s, ret=%d", objName, ret);
                return ret;
            }

            /* Set the default values of the parameters */
            mdmObj->managedEntityId = GPON_IP_HOST_MAX + i;

            if ((ret = devCtl_getBaseMacAddress(macNum)) == CMSRET_SUCCESS)
            {
                if ((ret = cmsUtl_binaryBufToHexString(macNum, 6, &hexStr)) == CMSRET_SUCCESS)
                {
                    CMSMEM_REPLACE_STRING_FLAGS(mdmObj->macAddress, hexStr, 0);
                    CMSMEM_FREE_BUF_AND_NULL_PTR(hexStr);
                }
            }

            if ((ret = _cmsObj_set(mdmObj, &iidStack)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to set %s, ret=%d", objName, ret);
            }

            /* _cmsObj_set does not steal obj, so caller must always free */
            _cmsObj_free((void **) &mdmObj);

#if (defined(OMCI_TR69_DUAL_STACK) && !defined(DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1))
            // create TR-069 Object that is associated with Ipv6Host
            ret = addDefaultGponTr69Object(mdmObj->managedEntityId);
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to create TR-069 ME for %s meId %d (ret=%d)",
                             objName, meId, ret);
                return ret;
            }
#endif
        }
    }
    else
    {
        _mdm_freeObject((void **)&mdmObj);
    }

    return ret;
}

#endif    // SUPPORT_IPV6

static CmsRet updateGponObjects(void)
{
    InstanceIdStack iidStack;
    _OntGObject *mdmObj;
    BCM_Ploam_SerialPasswdInfo info;
    char buf[BUFLEN_64] = {0};
    CmsRet cmsRet;
    int ret;

    INIT_INSTANCE_ID_STACK(&iidStack);

    if ((cmsRet = _mdm_getObject(MDMOID_ONT_G, &iidStack, (void**)&mdmObj)) !=
      CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get MDMOID_ONT_G, ret=%d", cmsRet);
        return cmsRet;
    }

    ret = gponCtl_getSerialPasswd(&info);
    if (ret == 0)
    {
        sprintf(buf, "%02x%02x%02x%02x%02x%02x%02x%02x",
          (unsigned char)info.serialNumber[0], (unsigned char)info.serialNumber[1],
          (unsigned char)info.serialNumber[2], (unsigned char)info.serialNumber[3],
          (unsigned char)info.serialNumber[4], (unsigned char)info.serialNumber[5],
          (unsigned char)info.serialNumber[6], (unsigned char)info.serialNumber[7]);
        if (mdmObj->serialNumber)
        {
            cmsMem_free(mdmObj->serialNumber);
        }
        mdmObj->serialNumber = cmsMem_strdupFlags(buf, 0);
    }

    mdmObj->trafficManagementOption = omciSys.trafficManagementOption;

    cmsRet = _mdm_setObject((void**)&mdmObj, &iidStack, FALSE);
    if (cmsRet != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to set MDMOID_ONT_G, ret = %d", cmsRet);
        _mdm_freeObject((void**)&mdmObj);
    }

    return cmsRet;
}

// when add codes to this function considering that
// it will be called everytime omcid receives MIB RESET message
CmsRet addDefaultGponObjects(void)
{
    CmsRet ret = CMSRET_SUCCESS;

    gUniInst = gMcastSubsInst = gIpHostInst = gPrioQInst = gTrSchInst = 1;
#ifdef OMCI_TR69_DUAL_STACK
    gTr69Inst = 1;
#endif

    if ((ret = addDefaultGponSoftwareImageObject()) != CMSRET_SUCCESS)
    {
        goto out;
    }
    updateGponObjects();
    addDefaultESCObject();
    addPowerMgmtObject();

    if ((ret = addDefaultCardHolderObject()) != CMSRET_SUCCESS)
    {
        goto out;
    }

#ifdef DMP_X_ITU_ORG_VOICE_1

    if ( _owapi_rut_IsVoiceOmciManaged() )
    {
        if ((ret = addDefaultGponVoiceObject()) != CMSRET_SUCCESS)
        {
            goto out;
        }
        if (omciSys.voiceModelOption == OMCI_VOIP_PATH_IP)
        {    
#if defined DMP_X_ITU_ORG_VOICE_SIP_1
            addSipConfigPortalObject();
#endif /* DMP_X_ITU_ORG_VOICE_SIP_1 */
#if defined (DMP_X_ITU_ORG_VOICE_MGC_1)
            addMgcConfigPortalObject();
#endif /* DMP_X_ITU_ORG_VOICE_MGC_1 */
        }
    }

#endif /* DMP_X_ITU_ORG_VOICE_1 */

#if defined(DMP_X_BROADCOM_COM_MOCALAN_1)

    if ((ret = addDefaultGponBrcmPptpMocaUniGObject()) != CMSRET_SUCCESS)
    {
        goto out;
    }

    if ((ret = addDefaultGponMocaStatusObject()) != CMSRET_SUCCESS)
    {
        goto out;
    }

    if ((ret = addDefaultGponMocaStatsObject()) != CMSRET_SUCCESS)
    {
        goto out;
    }

#endif /* DMP_X_BROADCOM_COM_MOCALAN_1 */

    if ((ret = addDefaultGponIpHostObject()) != CMSRET_SUCCESS)
    {
        goto out;
    }

#ifdef SUPPORT_IPV6

    if ((ret = addDefaultGponIpv6HostObject()) != CMSRET_SUCCESS)
    {
        goto out;
    }

#endif /* SUPPORT_IPV6 */

    if ((ret = addOnuRemoteDebugObject()) != CMSRET_SUCCESS)
    {
        goto out;
    }

out:
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to add OMCI objects, ret = %d", ret);
    }

    return ret;
}

static CmsRet addOnuRemoteDebugObject(void)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    _OnuRemoteDebugObject *mdmObj = NULL;
    const char *objName = "OnuRemoteDebug";
    const UINT32 mdmOid = MDMOID_ONU_REMOTE_DEBUG;

    INIT_INSTANCE_ID_STACK(&iidStack);
    cmsLog_notice("Adding %s instance %d", objName, 0);

    _mdm_initPathDescriptor(&pathDesc);
    pathDesc.oid = mdmOid;
    PUSH_INSTANCE_ID(&(pathDesc.iidStack), 1);

    if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to add %s, ret=%d", objName, ret);
        return ret;
    }

    iidStack = pathDesc.iidStack;
    if ((ret = _mdm_getObject(mdmOid, &iidStack, (void**)&mdmObj))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get %s, ret=%d", objName, ret);
        return ret;
    }

    mdmObj->managedEntityId = 0;
    ret = _mdm_setObject((void**)&mdmObj, &iidStack, FALSE);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to set %s, ret = %d", objName, ret);
        _mdm_freeObject((void**)&mdmObj);
    }

    return ret;
}

static CmsRet addPowerMgmtObject(void)
{
    _MdmPathDescriptor pathDesc;
    InstanceIdStack iidStack;
    CmsRet ret = CMSRET_SUCCESS;
    _DynamicPowerMgmtObject *mdmObj = NULL;
    const char *objName = "DynamicPowerMgmtObject";
    const UINT32 mdmOid = MDMOID_DYNAMIC_POWER_MGMT;

    INIT_INSTANCE_ID_STACK(&iidStack);
    cmsLog_notice("Adding %s instance %d", objName, 0);

    _mdm_initPathDescriptor(&pathDesc);
    pathDesc.oid = mdmOid;
    PUSH_INSTANCE_ID(&(pathDesc.iidStack), 1);

    /* add new object instance */
    if ((ret = _mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to add %s, ret=%d", objName, ret);
        return ret;
    }

    iidStack = pathDesc.iidStack;
    if ((ret = _mdm_getObject(mdmOid, &iidStack, (void**)&mdmObj))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get %s, ret=%d", objName, ret);
        return ret;
    }

    mdmObj->managedEntityId = 0;
    mdmObj->powerReductionManagementCapability = 0;
    mdmObj->powerReductionManagementMode = 0;
    mdmObj->itransinit = 0;
    mdmObj->itxinit = 0;
    mdmObj->maxSleepInterval = 0;
    mdmObj->maxReceiverOffInterval = 0;
    mdmObj->minAwareInterval = 0;
    mdmObj->minActiveHeldInterval = 0;
    mdmObj->maxiSleepIntervalExtension = 0;
    mdmObj->eponCapabilityExtension = 0;
    mdmObj->eponSetupExtension = 0;
    mdmObj->missingConsecutiveBurstsThreshold  = 4;

    ret = _mdm_setObject((void**)&mdmObj, &iidStack, FALSE);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to set %s, ret = %d", objName, ret);
        _mdm_freeObject((void**)&mdmObj);
    }

    return ret;
}
#endif /* DMP_X_ITU_ORG_GPON_1 */
