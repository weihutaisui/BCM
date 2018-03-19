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
#include "me_handlers.h"
#include "bridgeutil.h"
#include "omci_pm.h"
#include "bcm_mcast.h"
#include "bcm_mcast_api.h"


//=======================  Private GPON MCAST functions ========================

static UBOOL8 matchUniMacBpTp
    (UINT32 tpType, UINT32 tpPointer, UINT32 uniOid, UINT32 uniMeId)
{
    UBOOL8 found = FALSE;
    OmciBridgePortTpType expectedTpType = 0;

    switch (uniOid)
    {
        case MDMOID_PPTP_ETHERNET_UNI:
            expectedTpType = OMCI_BP_TP_PPTP_ETH;
            break;
        case MDMOID_PPTP_MOCA_UNI:
            expectedTpType = OMCI_BP_TP_PPTP_MOCA;
            break;
        case MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT:
            expectedTpType = OMCI_BP_TP_VEIP;
            break;
        default:
            break;
    }

    found = (tpType == expectedTpType) && (tpPointer == uniMeId);
    return found;
}

static UBOOL8 matchUniMapperTp
    (UINT32 tpType, UINT32 tpPointer, UINT32 uniOid, UINT32 uniMeId)
{
    UBOOL8 found = FALSE;
    OmciMapperSeriveTpType expectedTpType = 0;

    switch (uniOid)
    {
        case MDMOID_PPTP_ETHERNET_UNI:
            expectedTpType = OMCI_MS_TP_PPTP_ETH;
            break;
        case MDMOID_PPTP_MOCA_UNI:
            expectedTpType = OMCI_MS_TP_PPTP_MOCA;
            break;
        case MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT:
            expectedTpType = OMCI_MS_TP_VEIP;
            break;
        default:
            break;
    }

    found = (tpType == expectedTpType) && (tpPointer == uniMeId);
    return found;
}


// return value of field in Access Control List
// accessControl is array of UINT8 that has
// OMCI_MCAST_AC_SIZE
// The hexstring in MDM is stored in network order. The byte order conversion
// may be needed after cmsUtl_hexStringToBinaryBuf(), it is case-by-case based.
UINT32 getAccessControlField
    (const UINT8 *accessControl,
     const OmciMcastAccessControlField field)
{
    UINT32 val = 0x00000000;

    switch (field)
    {
        case OMCI_MCAST_AC_INDEX:
            val = (accessControl[OMCI_MCAST_AC_INDEX_START] << 8) +
                   accessControl[OMCI_MCAST_AC_INDEX_START + 1];
            break;
        case OMCI_MCAST_AC_GEM_PORT_ID:
            val = (accessControl[OMCI_MCAST_AC_GEM_PORT_ID_START] << 8) +
                   accessControl[OMCI_MCAST_AC_GEM_PORT_ID_START + 1];
            break;
        case OMCI_MCAST_AC_VLAN_ID:
            val = (accessControl[OMCI_MCAST_AC_VLAN_START] << 8) +
                   accessControl[OMCI_MCAST_AC_VLAN_START + 1];
            break;
        case OMCI_MCAST_AC_SRC_ADDR:
            val = OMCI_NTOHL(&accessControl[OMCI_MCAST_AC_SRC_ADDR_START]);
            break;
        case OMCI_MCAST_AC_DST_START_ADDR:
            val = OMCI_NTOHL(&accessControl[OMCI_MCAST_AC_DST_START_ADDR_START]);
            break;
        case OMCI_MCAST_AC_DST_END_ADDR:
            val = OMCI_NTOHL(&accessControl[OMCI_MCAST_AC_DST_END_ADDR_START]);
            break;
        case OMCI_MCAST_AC_BANDWIDTH:
            val = OMCI_NTOHL(&accessControl[OMCI_MCAST_AC_BANDWIDTH_START]);
            break;
        default:
            break;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> getAccessControlField, field=%d, val=%x\n",
                   field, val);

    return val;
}

CmsRet updateJoinMessageCounter
    (const UINT16 uniOid, const UINT16 uniMeId)
{
    UBOOL8 found = FALSE;
#ifdef OMCI_DEBUG_SUPPORT
    UINT32 counter = 0;
#endif
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BCM_MulticastSubscriberMonitorObject *mon = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    while ((!found) &&
           _cmsObj_getNext(MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR,
                          &iidStack,
                          (void **) &mon) == CMSRET_SUCCESS)
    {
        found = ((mon->objectId == uniOid) &&
                 (mon->managedEntityId == uniMeId));
        if (found == TRUE)
        {
            mon->joinMessagesCounter += 1;
#ifdef OMCI_DEBUG_SUPPORT
            counter = mon->joinMessagesCounter;
#endif
            ret = _cmsObj_set(mon, &iidStack);
        }
        _cmsObj_free((void **) &mon);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> updateJoinMessageCounter, uniOid=%d, uniMeId=%d, counter=%d, ret=%d\n",
                   uniOid, uniMeId, counter, ret);

    return ret;
}

CmsRet updateBandwidthExceededCounter
    (const UINT16 uniOid, const UINT16 uniMeId)
{
    UBOOL8 found = FALSE;
#ifdef OMCI_DEBUG_SUPPORT
    UINT32 counter = 0;
#endif
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BCM_MulticastSubscriberMonitorObject *mon = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    while ((!found) &&
           _cmsObj_getNext(MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR,
                          &iidStack,
                          (void **) &mon) == CMSRET_SUCCESS)
    {
        found = ((mon->objectId == uniOid) &&
                 (mon->managedEntityId == uniMeId));
        if (found == TRUE)
        {
            mon->bandwidthExceededCounter += 1;
#ifdef OMCI_DEBUG_SUPPORT
            counter = mon->bandwidthExceededCounter;
#endif
            ret = _cmsObj_set(mon, &iidStack);
        }
        _cmsObj_free((void **) &mon);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> updateBandwidthExceededCounter, uniOid=%d, uniMeId=%d, counter=%d, ret=%d\n",
                   uniOid, uniMeId, counter, ret);

    return ret;
}

UINT32 getCurrentImputedBandwidthUsage
    (const UINT16 uniOid,
     const UINT16 uniMeId)
{
    UBOOL8 found = FALSE;
    UINT32 bandwidth = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BCM_MulticastSubscriberMonitorObject *mon = NULL;

    while ((!found) &&
           _cmsObj_getNext(MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR,
                          &iidStack,
                          (void **) &mon) == CMSRET_SUCCESS)
    {
        found = ((mon->objectId == uniOid) &&
                 (mon->managedEntityId == uniMeId));
        if (found == TRUE)
            bandwidth = mon->currentMulticastBandwidth;
        _cmsObj_free((void **) &mon);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> getCurrentImputedBandwidthUsage, uniOid=%d, uniMeId=%d, bandwidth=%d\n",
                   uniOid, uniMeId, bandwidth);

    return bandwidth;
}

CmsRet updateCurrentImputedBandwidthUsage
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const UINT32 bandwidth,
     const OmciIgmpMsgType msgType)
{
    UBOOL8 found = FALSE;
#ifdef OMCI_DEBUG_SUPPORT
    UINT32 counter = 0;
#endif
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BCM_MulticastSubscriberMonitorObject *mon = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    while ((!found) &&
           _cmsObj_getNext(MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR,
                          &iidStack,
                          (void **) &mon) == CMSRET_SUCCESS)
    {
        found = ((mon->objectId == uniOid) &&
                 (mon->managedEntityId == uniMeId));
        if (found == TRUE)
        {
            if (msgType == OMCI_IGMP_MSG_JOIN)
                mon->currentMulticastBandwidth += bandwidth;
            else if (msgType == OMCI_IGMP_MSG_LEAVE)
                mon->currentMulticastBandwidth -= bandwidth;
#ifdef OMCI_DEBUG_SUPPORT
            counter = mon->currentMulticastBandwidth;
#endif
            ret = _cmsObj_set(mon, &iidStack);
        }
        _cmsObj_free((void **) &mon);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> updateCurrentImputedBandwidthUsage, uniOid=%d, uniMeId=%d, bandwidth=%d, msgType=%d, counter=%d, ret=%d\n",
                   uniOid, uniMeId, bandwidth, msgType, counter, ret);

    return ret;
}

UINT32 getNumberOfCurrentSubscribers
    (const UINT16 uniOid,
     const UINT16 uniMeId)
{
    UBOOL8 found = FALSE;
    UINT32 numberOfSubscribers = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BCM_MulticastSubscriberMonitorObject *mon = NULL;

    while ((!found) &&
           _cmsObj_getNext(MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR,
                          &iidStack,
                          (void **) &mon) == CMSRET_SUCCESS)
    {
        found = ((mon->objectId == uniOid) &&
                 (mon->managedEntityId == uniMeId));
        if (found == TRUE)
            numberOfSubscribers = mon->numberOfSubscribers;
        _cmsObj_free((void **) &mon);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> getNumberOfCurrentSubscribers, uniOid=%d, uniMeId=%d, numberOfSubscribers=%d\n",
                   uniOid, uniMeId, numberOfSubscribers);

    return numberOfSubscribers;
}

CmsRet isSubscriberExisted
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const OmciIgmpMsgBody *msg)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidParent = EMPTY_INSTANCE_ID_STACK;
    BCM_MulticastSubscriberMonitorObject *mon = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    while ((!found) &&
           _cmsObj_getNext(MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR,
                          &iidParent,
                          (void **) &mon) == CMSRET_SUCCESS)
    {
        found = ((mon->objectId == uniOid) &&
                 (mon->managedEntityId == uniMeId));
        if (found == TRUE)
        {
            UBOOL8 foundChild = FALSE;
            InstanceIdStack iidChild = EMPTY_INSTANCE_ID_STACK;
            MulticastSubscriberObject *subs = NULL;

            while ((!foundChild) &&
                   (_cmsObj_getNextInSubTree(MDMOID_MULTICAST_SUBSCRIBER,
                                            &iidParent, &iidChild,
                                            (void **) &subs) == CMSRET_SUCCESS))
            {
                if ((msg->igmpVersion == IGMP_VERSION_3) && (msg->sourceIpAddress != UNSPECIFIED_SOURCE_IP))
                {
                    foundChild = ((subs->groupIpAddress == msg->groupIpAddress) &&
                                  (subs->clientIpAddress == msg->clientIpAddress) &&
                                  (subs->sourceIpAddress == msg->sourceIpAddress));
                }
                else
                {
                    foundChild = ((subs->groupIpAddress == msg->groupIpAddress) &&
                                  (subs->clientIpAddress == msg->clientIpAddress));
                }

                if (foundChild == TRUE)
                    ret = CMSRET_SUCCESS;
                _cmsObj_free((void **) &subs);
            }
        }
        _cmsObj_free((void **) &mon);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> isSubscriberExisted, uniOid=%d, uniMeId=%d, grpAddr=0x%08x, clntAddr=0x%08x, ret=%d\n",
                   uniOid, uniMeId, msg->groupIpAddress, msg->clientIpAddress, ret);

    return ret;
}

CmsRet updateCurrentSubscribers
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const OmciIgmpMsgBody *msg)
{
    UBOOL8 found = FALSE;
#ifdef OMCI_DEBUG_SUPPORT
    UINT32 numberOfSubscribers = 0;
#endif
    InstanceIdStack iidParent = EMPTY_INSTANCE_ID_STACK;
    BCM_MulticastSubscriberMonitorObject *mon = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    while ((!found) &&
           _cmsObj_getNext(MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR,
                          &iidParent,
                          (void **) &mon) == CMSRET_SUCCESS)
    {
        found = ((mon->objectId == uniOid) &&
                 (mon->managedEntityId == uniMeId));
        if (found == TRUE)
        {
            InstanceIdStack iidChild = EMPTY_INSTANCE_ID_STACK;
            MulticastSubscriberObject *subs = NULL;
            CmsTimestamp tms;

            if (msg->msgType == OMCI_IGMP_MSG_JOIN)
            {
                memcpy(&iidChild, &iidParent, sizeof(InstanceIdStack));
                ret = _cmsObj_addInstance(MDMOID_MULTICAST_SUBSCRIBER, &iidChild);
                if (ret == CMSRET_SUCCESS)
                {
                    ret = _cmsObj_get(MDMOID_MULTICAST_SUBSCRIBER, &iidChild, 0, (void **) &subs);
                    if (ret == CMSRET_SUCCESS)
                    {
                        // add and set subscriber info
                        subs->tci = msg->tci;
                        subs->sourceIpAddress = msg->sourceIpAddress;
                        subs->groupIpAddress = msg->groupIpAddress;
                        subs->clientIpAddress = msg->clientIpAddress;
                        cmsTms_get(&tms);
                        subs->joinedTime = tms.sec;
                        ret = _cmsObj_set(subs, &iidChild);
                        _cmsObj_free((void **) &subs);
                        // update numberOfSubscribers
                        mon->numberOfSubscribers += 1;
#ifdef OMCI_DEBUG_SUPPORT
                        numberOfSubscribers = mon->numberOfSubscribers;
#endif
                        mon->version = msg->igmpVersion;
                        ret = _cmsObj_set(mon, &iidParent);
                    }
                }
            }
            else if (msg->msgType == OMCI_IGMP_MSG_LEAVE)
            {
                UBOOL8 foundChild = FALSE;

                while ((!foundChild) &&
                       (_cmsObj_getNextInSubTree(MDMOID_MULTICAST_SUBSCRIBER,
                                                &iidParent, &iidChild,
                                                (void **) &subs) == CMSRET_SUCCESS))
                {
                    foundChild = ((subs->groupIpAddress == msg->groupIpAddress) &&
                                  (subs->clientIpAddress == msg->clientIpAddress));
                    if (foundChild == TRUE)
                    {
                        // delete subscriber info
                        ret = _cmsObj_deleteInstance(MDMOID_MULTICAST_SUBSCRIBER, &iidChild);
                        // update numberOfSubscribers
                        mon->numberOfSubscribers -= 1;
#ifdef OMCI_DEBUG_SUPPORT
                        numberOfSubscribers = mon->numberOfSubscribers;
#endif
                        ret = _cmsObj_set(mon, &iidParent);
                    }
                    _cmsObj_free((void **) &subs);
                }
            }
        }
        _cmsObj_free((void **) &mon);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> updateCurrentSubscribers, uniOid=%d, uniMeId=%d, "
                   "grpAddr=0x%08x, clntAddr=0x%08x, numberOfSubscribers=%d, ret=%d\n",
                   uniOid, uniMeId, msg->groupIpAddress, msg->clientIpAddress, numberOfSubscribers, ret);

    return ret;
}

// if grpAddr & srcAddr is allowed in accessControl
// get imputed bandwidth and return CMSRET_SUCCESS
CmsRet getImputedBandwidth
    (const UINT8 *accessControl,
     const UINT32 grpAddr,
     const UINT32 srcAddr,
     const UINT32 igmpVersion,
     UINT32 *pBandwidth)
{
    UINT32 startAddr = 0, endAddr = 0, sourceIp = 0;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    startAddr = getAccessControlField(accessControl, OMCI_MCAST_AC_DST_START_ADDR);
    endAddr = getAccessControlField(accessControl, OMCI_MCAST_AC_DST_END_ADDR);
    sourceIp = getAccessControlField(accessControl, OMCI_MCAST_AC_SRC_ADDR);

    if (grpAddr >= startAddr && grpAddr <= endAddr)
    {
        *pBandwidth = getAccessControlField(accessControl, OMCI_MCAST_AC_BANDWIDTH);
        ret = CMSRET_SUCCESS;
    }
    else
        *pBandwidth = 0;

    if (igmpVersion == IGMP_VERSION_3 && sourceIp != UNSPECIFIED_SOURCE_IP && srcAddr != sourceIp)
    {
        *pBandwidth = 0;
        ret = CMSRET_OBJECT_NOT_FOUND;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> getImputedBandwidth, grpAddr=%x, startAddr=%x, endAddr=%x, "
                   "srcAddr=%x sourceIp=%x igmpVersion=0x%x, bw=%d, ret=%d\n",
                   grpAddr, startAddr, endAddr, srcAddr, sourceIp, igmpVersion, *pBandwidth, ret);

    return ret;
}

// return CMSRET_OBJECT_NOT_FOUND if group address is not allowed
// return CMSRET_RESOURCE_EXCEEDED
//     if bandwidth usage is over maxMulticastBandwidth
//     or number of subscribers is over maxSimultaneousGroups
// otherwise return CMSRET_SUCCESS
CmsRet handleJoinMessage
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const OmciIgmpMsgBody *msg)
{
    UBOOL8 found = FALSE;
    UINT32 numberOfSubcribers = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MulticastOperationsProfileObject *profile = NULL;
    MulticastSubscriberConfigInfoObject subscriber;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    memset(&subscriber, 0, sizeof(MulticastSubscriberConfigInfoObject));

    ret = getMulticastSubscriberConfigInfoFromUniMeId
              (uniOid, uniMeId, &subscriber);

    // if there is no subscriber info then there is no
    // restriction so return success
    if (ret != CMSRET_SUCCESS)
    {
        ret = CMSRET_SUCCESS;
        goto out;
    }

    // if simultaneous groups limit is imposed
    if (subscriber.maxSimultaneousGroups != 0)
    {
        // if number of subscribers is already hit limit
        // then return CMSRET_RESOURCE_EXCEEDED
        numberOfSubcribers = getNumberOfCurrentSubscribers(uniOid, uniMeId);
        if (numberOfSubcribers == subscriber.maxSimultaneousGroups)
        {
            ret = CMSRET_RESOURCE_EXCEEDED;
            goto out;
        }
    }

    while ((!found) &&
           (_cmsObj_getNext
                (MDMOID_MULTICAST_OPERATIONS_PROFILE,
                 &iidStack,
                 (void **) &profile) == CMSRET_SUCCESS))
    {
        found = (subscriber.multicastOperationsProfilePointer == profile->managedEntityId);

        if (found == TRUE)
        {
            UINT8 *buf = NULL;
            UINT32 bufSize = 0, bandwidth = 0;
            InstanceIdStack iidStackChild = EMPTY_INSTANCE_ID_STACK;
            DynamicAccessControlListTableObject *dacEntry;

            found = FALSE;
            // walk through DynamicAccessControlList table
            while ((!found) &&
                   (_cmsObj_getNextInSubTreeFlags
                       (MDMOID_DYNAMIC_ACCESS_CONTROL_LIST_TABLE,
                        &iidStack,
                        &iidStackChild,
                        OGF_NO_VALUE_UPDATE,
                        (void **) &dacEntry) == CMSRET_SUCCESS))
            {
                ret = cmsUtl_hexStringToBinaryBuf
                          ((char *)dacEntry->dynamicAccessControlEntry, &buf, &bufSize);
                if (ret == CMSRET_SUCCESS && bufSize == OMCI_MCAST_AC_SIZE)
                {
                    ret = getImputedBandwidth(buf, OMCI_NTOHL(&msg->groupIpAddress), OMCI_NTOHL(&msg->sourceIpAddress), msg->igmpVersion, &bandwidth);
                    if (ret == CMSRET_SUCCESS)
                    {
                        // if maximum multicast bandwith limit is imposed
                        if (subscriber.maxMulticastBandwidth != 0 && (bandwidth + getCurrentImputedBandwidthUsage(uniOid, uniMeId) >  subscriber.maxMulticastBandwidth))
                        {
                            updateBandwidthExceededCounter(uniOid, uniMeId);
                            ret = CMSRET_RESOURCE_EXCEEDED;
                        }
                        else
                        {
                            updateJoinMessageCounter(uniOid, uniMeId);
                            updateCurrentImputedBandwidthUsage(uniOid, uniMeId,
                                                               bandwidth, msg->msgType);
                            updateCurrentSubscribers(uniOid, uniMeId, msg);
                        }
                        found = TRUE;
                    }
                }
                CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
                _cmsObj_free((void **) &dacEntry);
            }
        }
        _cmsObj_free((void **) &profile);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> handleJoinMessage, uniOid=%d, uniMeId=%d, grpAddr=0x%08x, ret=%d\n",
                   uniOid, uniMeId, msg->groupIpAddress, ret);

    return ret;
}

CmsRet handleRejoinMessage
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const OmciIgmpMsgBody *msg)
{
    UBOOL8 found = FALSE;
#ifdef OMCI_DEBUG_SUPPORT
    UINT32 counter = 0;
#endif
    InstanceIdStack iidParent = EMPTY_INSTANCE_ID_STACK;
    BCM_MulticastSubscriberMonitorObject *mon = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    while ((!found) &&
           _cmsObj_getNext(MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR,
                          &iidParent,
                          (void **) &mon) == CMSRET_SUCCESS)
    {
        found = ((mon->objectId == uniOid) &&
                 (mon->managedEntityId == uniMeId));
        if (found == TRUE)
        {
            UBOOL8 foundChild = FALSE;
            InstanceIdStack iidChild = EMPTY_INSTANCE_ID_STACK;
            MulticastSubscriberObject *subs = NULL;
            CmsTimestamp tms;

            while ((!foundChild) &&
                   (_cmsObj_getNextInSubTree(MDMOID_MULTICAST_SUBSCRIBER,
                                            &iidParent, &iidChild,
                                            (void **) &subs) == CMSRET_SUCCESS))
            {
                foundChild = ((subs->groupIpAddress == msg->groupIpAddress) &&
                              (subs->clientIpAddress == msg->clientIpAddress));
                if (foundChild == TRUE)
                {
                    // update joined time
                    cmsTms_get(&tms);
                    subs->joinedTime = tms.sec;
                    ret = _cmsObj_set(subs, &iidChild);
                    // update join message counter
                    mon->joinMessagesCounter += 1;
#ifdef OMCI_DEBUG_SUPPORT
                    counter = mon->joinMessagesCounter;
#endif
                    ret = _cmsObj_set(mon, &iidParent);
                }
                _cmsObj_free((void **) &subs);
            }
        }
        _cmsObj_free((void **) &mon);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> handleRejoinMessage, uniOid=%d, uniMeId=%d, "
                   "grpAddr=0x%08x, clntAddr=0x%08x, counter=%d, ret=%d\n",
                   uniOid, uniMeId, msg->groupIpAddress, msg->clientIpAddress, counter, ret);

    return ret;
}

CmsRet handleLeaveMessage
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const OmciIgmpMsgBody *msg)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MulticastOperationsProfileObject *profile = NULL;
    MulticastSubscriberConfigInfoObject subscriber;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    memset(&subscriber, 0, sizeof(MulticastSubscriberConfigInfoObject));

    ret = getMulticastSubscriberConfigInfoFromUniMeId
              (uniOid, uniMeId, &subscriber);

    // if there is no subscriber info then there is no
    // restriction so return success
    if (ret != CMSRET_SUCCESS)
    {
        ret = CMSRET_SUCCESS;
        goto out;
    }

    while ((!found) &&
           (_cmsObj_getNext
                (MDMOID_MULTICAST_OPERATIONS_PROFILE,
                 &iidStack,
                 (void **) &profile) == CMSRET_SUCCESS))
    {
        found = (subscriber.multicastOperationsProfilePointer == profile->managedEntityId);

        if (found == TRUE)
        {
            UINT8 *buf = NULL;
            UINT32 bufSize = 0, bandwidth = 0;
            InstanceIdStack iidStackChild = EMPTY_INSTANCE_ID_STACK;
            DynamicAccessControlListTableObject *dacEntry;

            found = FALSE;
            // walk through DynamicAccessControlList table
            while ((!found) &&
                   (_cmsObj_getNextInSubTreeFlags
                       (MDMOID_DYNAMIC_ACCESS_CONTROL_LIST_TABLE,
                        &iidStack,
                        &iidStackChild,
                        OGF_NO_VALUE_UPDATE,
                        (void **) &dacEntry) == CMSRET_SUCCESS))
            {
                ret = cmsUtl_hexStringToBinaryBuf
                          ((char *)dacEntry->dynamicAccessControlEntry, &buf, &bufSize);
                if (ret == CMSRET_SUCCESS && bufSize == OMCI_MCAST_AC_SIZE)
                {
                    ret = getImputedBandwidth(buf, OMCI_NTOHL(&msg->groupIpAddress), OMCI_NTOHL(&msg->sourceIpAddress),
                        msg->igmpVersion, &bandwidth);
                    if (ret == CMSRET_SUCCESS)
                    {
                        updateCurrentImputedBandwidthUsage(uniOid, uniMeId,
                                                           bandwidth, msg->msgType);
                        updateCurrentSubscribers(uniOid, uniMeId, msg);
                        found = TRUE;
                    }
                }
                CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
                _cmsObj_free((void **) &dacEntry);
            }
        }
        _cmsObj_free((void **) &profile);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> handleLeaveMessage, uniOid=%d, uniMeId=%d, grpAddr=0x%08x, ret=%d\n",
                   uniOid, uniMeId, msg->groupIpAddress, ret);

    return ret;
}

CmsRet getMulticastSubscriberConfigInfoFromUniMeId
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     MulticastSubscriberConfigInfoObject *pInfo)
{
    UBOOL8 found = FALSE, foundSubs = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MacBridgePortConfigDataObject *port = NULL;
    MulticastSubscriberConfigInfoObject *mcastSubs = NULL;
    MapperServiceProfileObject *mapper = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search MacBridgePortConfigDataObject
    while ((!found) &&
           (_cmsObj_getNext(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        found = matchUniMacBpTp(port->tpType, port->tpPointer, uniOid, uniMeId);
        if (found == TRUE)
        {
            INIT_INSTANCE_ID_STACK(&iidStack);
            while ((!foundSubs) &&
                   (_cmsObj_getNext(MDMOID_MULTICAST_SUBSCRIBER_CONFIG_INFO,
                                   &iidStack,
                                   (void **) &mcastSubs) == CMSRET_SUCCESS))
            {
                foundSubs = ((mcastSubs->managedEntityId == port->managedEntityId) &&
                             (mcastSubs->meType == OMCI_MCAST_METYPE_MAC_BPCD));
                if (foundSubs == TRUE)
                {
                    memcpy(pInfo, mcastSubs, sizeof(MulticastSubscriberConfigInfoObject));
                    ret = CMSRET_SUCCESS;
                }
                _cmsObj_free((void **) &mcastSubs);
            }
        }
        _cmsObj_free((void **) &port);
    }

    INIT_INSTANCE_ID_STACK(&iidStack);
    // search MapperServiceProfileObject
    while ((!found) &&
           (_cmsObj_getNext(MDMOID_MAPPER_SERVICE_PROFILE,
                           &iidStack,
                           (void **) &mapper) == CMSRET_SUCCESS))
    {
        found = matchUniMapperTp(mapper->tpType, mapper->tpPointer, uniOid, uniMeId);
        if (found == TRUE)
        {
            INIT_INSTANCE_ID_STACK(&iidStack);
            while ((!foundSubs) &&
                   (_cmsObj_getNext(MDMOID_MULTICAST_SUBSCRIBER_CONFIG_INFO,
                                   &iidStack,
                                   (void **) &mcastSubs) == CMSRET_SUCCESS))
            {
                foundSubs = ((mcastSubs->managedEntityId == mapper->managedEntityId) &&
                             (mcastSubs->meType == OMCI_MCAST_METYPE_MAPPER_SVC));
                if (foundSubs == TRUE)
                {
                    memcpy(pInfo, mcastSubs, sizeof(MulticastSubscriberConfigInfoObject));
                    ret = CMSRET_SUCCESS;
                }
                _cmsObj_free((void **) &mcastSubs);
            }
        }
        _cmsObj_free((void **) &mapper);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> getMulticastSubscriberConfigInfoFromUniMeId, uniOid=%d, uniMeId=%d, ret=%d\n",
                   uniOid, uniMeId, ret);

    return ret;
}

CmsRet handleIgmpMessage
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const OmciIgmpMsgBody *msg)
{
    CmsRet ret = CMSRET_SUCCESS;
    MulticastSubscriberConfigInfoObject subscriber;

    memset(&subscriber, 0, sizeof(MulticastSubscriberConfigInfoObject));

    // JIRA SWBCACPE-9441
    // only handle IGMP message when MulticastSubscriberConfigInfoObject exists
    if (getMulticastSubscriberConfigInfoFromUniMeId
        (uniOid, uniMeId, &subscriber) != CMSRET_SUCCESS)
        goto out;

    ret = isSubscriberExisted(uniOid, uniMeId, msg);

    switch (msg->msgType)
    {
        case OMCI_IGMP_MSG_JOIN:
            // if subscriber is NOT existed then handle join msg
            if (ret != CMSRET_SUCCESS)
                ret = handleJoinMessage(uniOid, uniMeId, msg);
            // if subscriber is already existed then do nothing
            break;
        case OMCI_IGMP_MSG_RE_JOIN:
            // if subscriber is existed then handle re-join msg
            if (ret == CMSRET_SUCCESS)
                ret = handleRejoinMessage(uniOid, uniMeId, msg);
            // if subscriber is NOT existed
            // then treat it as join message
            else
            {
                OmciIgmpMsgBody body;
                memcpy(&body, msg, sizeof(OmciIgmpMsgBody));
                body.msgType = OMCI_IGMP_MSG_JOIN;
                ret = handleJoinMessage(uniOid, uniMeId, &body);
            }
            break;
        case OMCI_IGMP_MSG_LEAVE:
            // if subscriber is existed then handle leave msg
            if (ret == CMSRET_SUCCESS)
                ret = handleLeaveMessage(uniOid, uniMeId, msg);
            // if subscriber is NOT existed then return error
            else
                ret = CMSRET_OBJECT_NOT_FOUND;
            break;
        default:
            break;
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> handleIgmpMessage, uniOid=%d, uniMeId=%d, grpAddr=0x%08x, msgType=%d, ret=%d\n",
                   uniOid, uniMeId, msg->groupIpAddress, msg->msgType, ret);

    return ret;
}

//=======================  Public GPON MCAST functions ========================

UINT32 rutGpon_getImputedBandwidth
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const UINT32 grpAddr,
     const UINT32 srcAddr,
     const UINT32 igmpVersion)
{
    UBOOL8 found = FALSE;
    UINT32 imputedBandwidth = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MulticastOperationsProfileObject *profile = NULL;
    MulticastSubscriberConfigInfoObject subscriber;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    memset(&subscriber, 0, sizeof(MulticastSubscriberConfigInfoObject));

    ret = getMulticastSubscriberConfigInfoFromUniMeId
              (uniOid, uniMeId, &subscriber);

    // if there is no subscriber info then return 0
    if (ret != CMSRET_SUCCESS)
        goto out;

    while ((!found) &&
           (_cmsObj_getNext
                (MDMOID_MULTICAST_OPERATIONS_PROFILE,
                 &iidStack,
                 (void **) &profile) == CMSRET_SUCCESS))
    {
        found = (subscriber.multicastOperationsProfilePointer == profile->managedEntityId);

        if (found == TRUE)
        {
            UINT8 *buf = NULL;
            UINT32 bufSize = 0, bandwidth = 0;
            InstanceIdStack iidStackChild = EMPTY_INSTANCE_ID_STACK;
            DynamicAccessControlListTableObject *dacEntry;

            found = FALSE;
            // walk through DynamicAccessControlList table
            while ((!found) &&
                   (_cmsObj_getNextInSubTreeFlags
                       (MDMOID_DYNAMIC_ACCESS_CONTROL_LIST_TABLE,
                        &iidStack,
                        &iidStackChild,
                        OGF_NO_VALUE_UPDATE,
                        (void **) &dacEntry) == CMSRET_SUCCESS))
            {
                ret = cmsUtl_hexStringToBinaryBuf
                          ((char *)dacEntry->dynamicAccessControlEntry, &buf, &bufSize);
                if (ret == CMSRET_SUCCESS && bufSize == OMCI_MCAST_AC_SIZE)
                {
                    ret = getImputedBandwidth(buf, grpAddr, srcAddr, igmpVersion, &bandwidth);
                    if (ret == CMSRET_SUCCESS)
                    {
                        imputedBandwidth = bandwidth;
                        found = TRUE;
                    }
                }
                CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
                _cmsObj_free((void **) &dacEntry);
            }
        }
        _cmsObj_free((void **) &profile);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> rutGpon_getImputedBandwidth, uniOid=%d, uniMeId=%d, grpAddr=%x, srcAddr=%x, igmpVersion=0x%x, imputedBandwidth=%d, ret=%d\n",
                   uniOid, uniMeId, grpAddr, srcAddr, igmpVersion, imputedBandwidth, ret);

    return imputedBandwidth;
}

UINT32 rutGpon_getCurrentMcastBandwidth
    (const UINT16 uniOid,
     const UINT16 uniMeId)
{
    UINT32 counter = 0, meId = 0;

    if (uniOid == MDMOID_PPTP_ETHERNET_UNI)
    {
        rutGpon_getFirstEthernetMeId(&meId);
        // ethernet class = 0, ethernet port = uniMeId - meId
        omci_pm_mcast_get(0, uniMeId - meId, &counter);
    }
    else if (uniOid == MDMOID_PPTP_MOCA_UNI)
    {
        // only support 1 moca port that has its value is 0
        // moca class = 1, moca port = 0
        omci_pm_mcast_get(1, 0, &counter);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> rutGpon_getCurrentMcastBandwidth, uniOid=%d, uniMeId=%d, counter=%d\n",
                   uniOid, uniMeId, counter);

    return counter;
}

UINT32 rutGpon_getBandwidthExceededCounter
    (const UINT16 uniOid,
     const UINT16 uniMeId)
{
    UBOOL8 found = FALSE;
    UINT32 counter = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BCM_MulticastSubscriberMonitorObject *mon = NULL;

    while ((!found) &&
           _cmsObj_getNext(MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR,
                          &iidStack,
                          (void **) &mon) == CMSRET_SUCCESS)
    {
        found = ((mon->objectId == uniOid) &&
                 (mon->managedEntityId == uniMeId));
        if (found == TRUE)
            counter = mon->bandwidthExceededCounter;
        _cmsObj_free((void **) &mon);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> rutGpon_getBandwidthExceededCounter, uniOid=%d, uniMeId=%d, found=%d, counter=%d\n",
                   uniOid, uniMeId, found, counter);

    return counter;
}

UINT32 rutGpon_getJoinMessageCounter
    (const UINT16 uniOid,
     const UINT16 uniMeId)
{
    UBOOL8 found = FALSE;
    UINT32 counter = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BCM_MulticastSubscriberMonitorObject *mon = NULL;

    while ((!found) &&
           _cmsObj_getNext(MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR,
                          &iidStack,
                          (void **) &mon) == CMSRET_SUCCESS)
    {
        found = ((mon->objectId == uniOid) &&
                 (mon->managedEntityId == uniMeId));
        if (found == TRUE)
            counter = mon->joinMessagesCounter;
        _cmsObj_free((void **) &mon);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> rutGpon_getJoinMessageCounter, uniOid=%d, uniMeId=%d, found=%d, counter=%d\n",
                   uniOid, uniMeId, found, counter);

    return counter;
}

CmsRet rutGpon_updateActiveGroupList
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const InstanceIdStack *pMcastSubsMonIid)
{
    char *bufString = NULL;
    UBOOL8 found = FALSE;
    UINT8 bufHex[OMCI_MCAST_AC_SIZE];
    InstanceIdStack iid = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidParent = EMPTY_INSTANCE_ID_STACK;
    BCM_MulticastSubscriberMonitorObject *mon = NULL;
    ActiveGroupsListTableObject *agEntry = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    // remove the old active group list table
    // before adding the new one
    while (_cmsObj_getNextInSubTree(MDMOID_ACTIVE_GROUPS_LIST_TABLE,
                                   pMcastSubsMonIid, &iid,
                                   (void **) &agEntry) == CMSRET_SUCCESS)
    {
        _cmsObj_deleteInstance(MDMOID_ACTIVE_GROUPS_LIST_TABLE, &iid);
        _cmsObj_free((void **)&agEntry);
        INIT_INSTANCE_ID_STACK(&iid);
    }

    while ((!found) &&
           _cmsObj_getNext(MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR,
                          &iidParent,
                          (void **) &mon) == CMSRET_SUCCESS)
    {
        found = ((mon->objectId == uniOid) &&
                 (mon->managedEntityId == uniMeId));
        if (found == TRUE)
        {
            UINT16 vlanId = 0;
            UINT32 bandwidth = 0, joinedTime = 0;
            InstanceIdStack iidChild = EMPTY_INSTANCE_ID_STACK;
            MulticastSubscriberObject *subs = NULL;
            CmsTimestamp tms;

            while (_cmsObj_getNextInSubTree(MDMOID_MULTICAST_SUBSCRIBER,
                                           &iidParent, &iidChild,
                                           (void **) &subs) == CMSRET_SUCCESS)
            {
                memset(bufHex, 0, OMCI_MCAST_AC_SIZE);
                // bufHex has length is 24 with the following format
                // vlanId: 2 bytes, source: 4 bytes, group: 4 bytes
                // bandwidth: 4 bytes, client: 4 bytes, time: 4 bytes
                // reserved: 2 bytes
                // The hexstring in MDM is stored in network order.
                // The byte order conversion is needed here.
                vlanId = (((UINT16) subs->tci) & 0x0FFF);
                OMCI_HTONS(&bufHex[0], vlanId);
                OMCI_HTONL(&bufHex[2], subs->sourceIpAddress);
                OMCI_HTONL(&bufHex[6], subs->groupIpAddress);
                bandwidth = rutGpon_getImputedBandwidth(uniOid, uniMeId, subs->groupIpAddress,
                    subs->sourceIpAddress, mon->version);
                OMCI_HTONL(&bufHex[10], bandwidth);
                OMCI_HTONL(&bufHex[14], subs->clientIpAddress);
                cmsTms_get(&tms);
                joinedTime = tms.sec - subs->joinedTime;
                OMCI_HTONL(&bufHex[18], joinedTime);
                cmsUtl_binaryBufToHexString(bufHex, OMCI_MCAST_AC_SIZE, &bufString);

                memcpy(&iid, pMcastSubsMonIid, sizeof(InstanceIdStack));
                // add given entry to the table
                ret = _cmsObj_addInstance(MDMOID_ACTIVE_GROUPS_LIST_TABLE, &iid);
                if (ret == CMSRET_SUCCESS)
                {
                    ret = _cmsObj_get(MDMOID_ACTIVE_GROUPS_LIST_TABLE, &iid, 0, (void **) &agEntry);
                    if (ret == CMSRET_SUCCESS)
                    {
                        CMSMEM_FREE_BUF_AND_NULL_PTR(agEntry->activeGroupsListEntry);
                        agEntry->activeGroupsListEntry = cmsMem_alloc((OMCI_MCAST_AC_SIZE*2)+1,
                                                                  ALLOC_ZEROIZE);
                        if(agEntry->activeGroupsListEntry == NULL)
                        {
                            cmsLog_error("failed to allocate memory");
                            ret = CMSRET_INTERNAL_ERROR;
                        }
                        else
                        {
                            memcpy(agEntry->activeGroupsListEntry, bufString,
                                   OMCI_MCAST_AC_SIZE * 2);
                            ret = _cmsObj_set(agEntry, &iid);
                        }
                        _cmsObj_free((void **)&agEntry);
                    }
                    else
                        cmsLog_error("Could not get Active Groups List Table object, ret = %d", ret);
                }
                else
                    cmsLog_error("Could not add Active Groups List Table object, ret = %d", ret);

                cmsMem_free(bufString);
                _cmsObj_free((void **) &subs);
            }
        }
        _cmsObj_free((void **) &mon);
    }

    return ret;
}

CmsRet rutGpon_requestIgmpMessage
    (const OmciIgmpMsgBody *msg)
{
    CmsRet ret = CMSRET_INVALID_ARGUMENTS;
    UINT16 uniOid = 0, uniMeId = 0;
    UINT32 meId = 0;

    {
        if (msg->phyType == OMCI_IGMP_PHY_ETHERNET)
        {
            ret = rutGpon_getEthPortLinkedOmciMe(msg->phyPort, &uniOid, &uniMeId);
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_notice("No matching OMCI UNI port for port=%d, ret=%d",
                  msg->phyPort, ret);
                goto out;
            }
            ret = handleIgmpMessage(uniOid, uniMeId, msg);
        }
        else if (msg->phyType == OMCI_IGMP_PHY_MOCA)
        {
            uniOid = MDMOID_PPTP_MOCA_UNI;
            ret = rutGpon_getFirstMocaMeId(&meId);
            if (ret != CMSRET_SUCCESS)
                goto out;
            uniMeId = meId + msg->phyPort;
            ret = handleIgmpMessage(uniOid, uniMeId, msg);
        }
    }

out:

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> rutGpon_requestIgmpMessage, tci=0x%x, src=0x%08x, grp=0x%08x, clnt=0x%08x, "
                   "phyPort=%d, phyType=%d, msgType=%d, uniOid=%d, uniMeId=%d, ret=%d\n",
                   msg->tci, msg->sourceIpAddress, msg->groupIpAddress,
                   msg->clientIpAddress, msg->phyPort, msg->phyType, msg->msgType,
                   uniOid, uniMeId, ret);

    return ret;
}

CmsRet getMopFromBridgeId(const UINT32 bridgeId, MulticastOperationsProfileObject *mopCfgP)
{
    UBOOL8 foundProfile = FALSE;
    UBOOL8 foundPort = FALSE;
    InstanceIdStack iidPort = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidSubs= EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidProfile= EMPTY_INSTANCE_ID_STACK;
    MacBridgePortConfigDataObject *port = NULL;
    MulticastSubscriberConfigInfoObject *mcastSubs = NULL;
    MulticastOperationsProfileObject *mcastProfile = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search MacBridgePortConfigDataObject
    while ((!foundProfile) &&
           (_cmsObj_getNext(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidPort,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        foundPort = ((port->tpType == OMCI_BP_TP_PPTP_ETH ||
                      port->tpType == OMCI_BP_TP_PPTP_MOCA ||
                      port->tpType == OMCI_BP_TP_VEIP) &&
                     (port->bridgeId == bridgeId));
        if (foundPort == TRUE)
        {
            while ((!foundProfile) &&
                    (_cmsObj_getNext(MDMOID_MULTICAST_SUBSCRIBER_CONFIG_INFO,
                                   &iidSubs,
                                    (void **) &mcastSubs) == CMSRET_SUCCESS))
            {
                if (mcastSubs->managedEntityId == port->managedEntityId)
                {
                    while ((!foundProfile) &&
                           (_cmsObj_getNext(MDMOID_MULTICAST_OPERATIONS_PROFILE,
                                           &iidProfile,
                                           (void **) &mcastProfile) == CMSRET_SUCCESS))
                    {
                        foundProfile = (mcastProfile->managedEntityId == mcastSubs->multicastOperationsProfilePointer);
                        if (foundProfile == TRUE)
                        {
                            memcpy(mopCfgP, mcastProfile, sizeof(MulticastOperationsProfileObject));
                            ret = CMSRET_SUCCESS;
                        }

                        _cmsObj_free((void **) &mcastProfile);
                    }
                }

                _cmsObj_free((void **) &mcastSubs);
            }
        }

        _cmsObj_free((void **) &port);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> getMopFromBridgeId, bridgeId=%d, ret=%d\n",
                   bridgeId, ret);

    return ret;
}


CmsRet getMopFromUniMeId(UINT32 uniOid, UINT32 uniMeId, MulticastOperationsProfileObject *mopCfgP)
{
    MulticastSubscriberConfigInfoObject msci;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MulticastOperationsProfileObject *mop = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
    UBOOL8 found = FALSE;

    memset(&msci, 0, sizeof(MulticastSubscriberConfigInfoObject));
    ret = getMulticastSubscriberConfigInfoFromUniMeId(uniOid, uniMeId, &msci);
    if (ret != CMSRET_SUCCESS)
    {
        goto out;
    }

    while ((!found) && (_cmsObj_getNext(MDMOID_MULTICAST_OPERATIONS_PROFILE,
      &iidStack, (void**)&mop) == CMSRET_SUCCESS))
    {
        if (mop->managedEntityId == msci.multicastOperationsProfilePointer)
        {
            found = TRUE;
            memcpy(mopCfgP, mop, sizeof(MulticastOperationsProfileObject));
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void**)&mop);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
      "===> getMopFromUniMeId, uniOid=%u, uniMeId=%u, ret=%d\n",
      uniOid, uniMeId, ret);

    return ret;
}

CmsRet rutGpon_getIgmpRateLimitFromBridgeId(const UINT32 bridgeId, UINT32 *pRate)
{
    MulticastOperationsProfileObject mop;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    ret = getMopFromBridgeId(bridgeId, &mop);
    if (ret == CMSRET_SUCCESS)
    {
        *pRate = mop.upstreamIgmpRate;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> rutGpon_getIgmpRateLimitFromBridgeId, bridgeId=%d, rate=%d, ret=%d\n",
                   bridgeId, *pRate, ret);
    return ret;
}

CmsRet rutGpon_getUsMcastTciFromUniMeId
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     UINT8 *pType,
     UINT16 *pTci)
{
    MulticastOperationsProfileObject mop;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    ret = getMopFromUniMeId(uniOid, uniMeId, &mop);
    if (ret == CMSRET_SUCCESS)
    {
        *pType = mop.upstreamIgmpTagControl;
        *pTci = mop.upstreamIgmpTci;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> rutGpon_getUsMcastTciFromUniMeId, uniOid=%d, uniMeId=%d, type=%d, tci=%d, ret=%d\n",
                   uniOid, uniMeId, *pType, *pTci, ret);
    return ret;
}

// The hexstring in MDM is stored in network order. The byte order conversion
// may be needed after cmsUtl_hexStringToBinaryBuf(), it is case-by-case based.
CmsRet rutGpon_getDsMcastTciFromBridgeMeId
    (const UINT32 bridgeMeId,
     UINT8 *pType,
     UINT16 *pTci)
{
    MulticastOperationsProfileObject mop;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    ret = getMopFromBridgeId(bridgeMeId, &mop);
    if (ret == CMSRET_SUCCESS)
    {
        UINT8 *buf = NULL;
        UINT32 bufSize = 0;
        ret = cmsUtl_hexStringToBinaryBuf
          ((char*)mop.downstreamIgmpTci, &buf, &bufSize);
        if (ret == CMSRET_SUCCESS)
        {
            *pType = buf[0];
            *pTci = OMCI_NTOHS(&buf[1]);
            cmsMem_free(buf);
        }
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> rutGpon_getDsMcastTciFromBridgeMeId, bridgeMeId=%d, type=%d, tci=%d, ret=%d\n",
                   bridgeMeId, *pType, *pTci, ret);

    return ret;
}

CmsRet rutGpon_getIgmpRateLimitOnRgBridge(UINT32 *pRate)
{
    UINT16 uniOid;
    UINT16 uniMeId;
    MulticastOperationsProfileObject mop;
    CmsRet ret = CMSRET_SUCCESS;

    ret = rutGpon_getVirtIntfMeId(&uniOid, &uniMeId);
    ret = (ret != CMSRET_SUCCESS) ? ret : getMopFromUniMeId((UINT32)uniOid, (UINT32)uniMeId, &mop);
    if (ret == CMSRET_SUCCESS)
    {
        *pRate = mop.upstreamIgmpRate;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> rutGpon_getIgmpRateLimitOnRgBridge, rate=%d, ret=%d\n",
                   *pRate, ret);
    return ret;
}

void getBridgeWanInfo(
	SINT32 *wanIfIndices,
	t_BCM_MCAST_WAN_INFO_ARRAY *wanInfo)
{
    SINT32 i, j;

    for (i = 0, j = 0; i < BRIDGE_MAX_IFS; i ++)
    {
    	if (wanIfIndices[i])
        {
            wanInfo[j]->ifi = wanIfIndices[i];
            wanInfo[j]->if_ops = BCM_MCAST_IF_BRIDGED;
            j ++;
        }
    }
}

CmsRet updateMcastIgmpTable
	(int filterMode,
	UINT8 *dac,
	char *brName,
	char *uniVirtName)
{
#if defined(DMP_X_BROADCOM_COM_IGMPSNOOP_1)
    UINT16 vlanId;
    int brNo, brPortNo;
    UINT32 grpIp, srcIp, repIp;
    UINT8 repMac[6];
    SINT32 wanIfIndices[BRIDGE_MAX_IFS];
    UINT32 wancnt = BRIDGE_MAX_IFS;
    t_BCM_MCAST_WAN_INFO_ARRAY wanInfo;

    /* first, we parse the dac data from DynamicAccessControl ME */
    repIp = 0;
    vlanId = getAccessControlField(dac, OMCI_MCAST_AC_VLAN_ID);
    grpIp = getAccessControlField(dac, OMCI_MCAST_AC_DST_START_ADDR);
    srcIp = getAccessControlField(dac, OMCI_MCAST_AC_SRC_ADDR);

    brNo = cmsNet_getIfindexByIfname (brName);
    if (brNo < 0)
    {
        return CMSRET_INVALID_PARAM_VALUE;
    }

    brPortNo = cmsNet_getIfindexByIfname (uniVirtName);
    if (brPortNo < 0)
    {
        return CMSRET_INVALID_PARAM_VALUE;
    }

    memset(wanIfIndices, 0, sizeof(wanIfIndices));
    if (br_util_get_bridge_wan_interfaces(brName, wanIfIndices, &wancnt))
    {
        return CMSRET_INVALID_PARAM_VALUE;
    }

    memset(&wanInfo, 0, sizeof(wanInfo));
    getBridgeWanInfo(wanIfIndices, &wanInfo);

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                   "===> updateMcastIgmpTable, brname: %s, uniVirtName: %s, port: %d, vid: 0x%x, mode: %d\n",
                   brName, uniVirtName, brPortNo, vlanId, filterMode);

    /* call library function */
    if (bcm_mcast_api_update_igmp_snoop(-1, brNo, brPortNo, vlanId, 0,
    		(struct in_addr*)&grpIp, (struct in_addr*)&grpIp,
    		(struct in_addr*)&srcIp, (struct in_addr*)&repIp,
    		repMac, 0, filterMode, &wanInfo, -1, 0) != 0)
    {
        cmsLog_error("bcm_mcast_api_update_igmp_snoop failed\n");
        return CMSRET_INVALID_PARAM_VALUE;
    }
#endif

    return CMSRET_SUCCESS;
}

CmsRet getMcastEthernetPortInformation
	(const InstanceIdStack *iidStack,
	char *brName,
	char *uniVirtName)
{
    CmsRet ret;
    UBOOL8 found = FALSE;
    UINT32 uniMeId;
    MulticastOperationsProfileObject *mopObj = NULL;
    MulticastSubscriberConfigInfoObject *msciObj = NULL;
    MacBridgePortConfigDataObject *mbpcdObj = NULL;
    MacBridgeServiceProfileObject *mbspObj = NULL;
    InstanceIdStack iidStackTmp;
    char *uniVirtIfNamePtr = NULL;
	
    /* get the parent MOP ME object */
    memcpy(&iidStackTmp, iidStack, sizeof(InstanceIdStack));
#if 0
    cmsLog_notice("iidStack: %s", cmsMdm_dumpIidStack(&iidStackTmp));
    if ((ret = _cmsObj_getAncestorFlags(MDMOID_MULTICAST_OPERATIONS_PROFILE,
		MDMOID_DYNAMIC_ACCESS_CONTROL_LIST_TABLE,
		&iidStackTmp, OGF_NO_VALUE_UPDATE, (void**) &mopObj)) != CMSRET_SUCCESS)
    {
    	cmsLog_error("could not get MOP ancestor obj\n");
        return ret;
    }
#else
    iidStackTmp.instance[iidStackTmp.currentDepth] = 0;
    iidStackTmp.currentDepth --;
    if ((ret = _cmsObj_get(MDMOID_MULTICAST_OPERATIONS_PROFILE,
		(const InstanceIdStack*)&iidStackTmp, 0,
		(void**) &mopObj)) != CMSRET_SUCCESS)
    {
    	cmsLog_error("could not get MOP ancestor obj");
        return ret;
    }
#endif

    /* get the MSCI ME object */
    // search MacBridgePortConfigDataObject
    INIT_INSTANCE_ID_STACK(&iidStackTmp);
    while ((!found) &&
    		(_cmsObj_getNext(MDMOID_MULTICAST_SUBSCRIBER_CONFIG_INFO,
    				&iidStackTmp,
    				(void **) &msciObj) == CMSRET_SUCCESS))
    {
        found = ((msciObj->multicastOperationsProfilePointer ==
				mopObj->managedEntityId) &&
    			(msciObj->meType == OMCI_MCAST_METYPE_MAC_BPCD));
	if (!found)
        	_cmsObj_free((void **) &msciObj);
    }
    if (!found)
    	{
    	cmsLog_error("could not get associated MSCI object");
        ret = CMSRET_INTERNAL_ERROR;
        goto mopExit;
    	}

    /* get MBPCD ME object from MSCI ME object */
    found = FALSE;
    INIT_INSTANCE_ID_STACK(&iidStackTmp);
    while ((!found) &&
           (_cmsObj_getNext(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStackTmp,
                           (void **) &mbpcdObj) == CMSRET_SUCCESS))
    {
        found = ((mbpcdObj->managedEntityId == msciObj->managedEntityId) &&
                 (mbpcdObj->tpType == OMCI_BP_TP_PPTP_ETH));
        if (!found)
            _cmsObj_free((void **) &mbpcdObj);
    }
    if (!found)
    	{
    	cmsLog_error("could not get associated MBPCD object");
        ret = CMSRET_INTERNAL_ERROR;
        goto msciExit;
    	}

    /* get MBSP ME object from MSCI ME object */
    found = FALSE;
    INIT_INSTANCE_ID_STACK(&iidStackTmp);
    while ((!found) &&
           (_cmsObj_getNext(MDMOID_MAC_BRIDGE_SERVICE_PROFILE,
                           &iidStackTmp,
                           (void **) &mbspObj) == CMSRET_SUCCESS))
    {
        found = (mbspObj->managedEntityId == mbpcdObj->bridgeId);
        if (!found)
            _cmsObj_free((void **) &mbpcdObj);
    }
    if (!found)
    	{
    	cmsLog_error("could not get associated MBSP object");
        ret = CMSRET_INTERNAL_ERROR;
        goto mbspExit;
    	}

    /* get UNI information from MBPCD ME object*/
    ret = rutGpon_getFirstEthernetMeId(&uniMeId);
    if (ret != CMSRET_SUCCESS)
    {
       cmsLog_error("could not get rirst ethernet MeId");
       ret = CMSRET_INTERNAL_ERROR;
       goto mbpcdExit;
    }
	
    rutGpon_getBridgeNameFromBridgeMeId(
   		mbspObj->managedEntityId, brName);

    /* get UNI virtual interface name */
    rutGpon_getVirtualInterfaceName(OMCI_PHY_ETHERNET,
		(mbpcdObj->tpPointer - uniMeId),
		mbspObj->managedEntityId,
		&uniVirtIfNamePtr);
    strcpy(uniVirtName, uniVirtIfNamePtr);
    cmsMem_free(uniVirtIfNamePtr);

mbspExit:
    _cmsObj_free((void **) &mbspObj);

mbpcdExit:
    _cmsObj_free((void **) &mbpcdObj);

msciExit:	
    _cmsObj_free((void **) &msciObj);
	
mopExit:
    _cmsObj_free((void **) &mopObj);
    return ret;
}

CmsRet rutGpon_deleteDynamicAccessControl
	(const DynamicAccessControlListTableObject *currObj,
	const InstanceIdStack *iidStack)
{
    UINT32 dacSize = 0;
    UINT8 *dac = NULL;
    char brName[16];
    char uniVirtName[16];

    cmsUtl_hexStringToBinaryBuf(currObj->dynamicAccessControlEntry,
    							&dac, &dacSize);
    if (dacSize != OMCI_MCAST_AC_SIZE)
    {
        cmsLog_error("Invalid data for %s %s",
                   _mdm_oidToGenericPath(currObj->_oid),
                   _cmsMdm_dumpIidStack(iidStack));
        cmsMem_free(dac);
        return CMSRET_INTERNAL_ERROR;
    }

    memset(brName, 0, sizeof(brName));
    memset(uniVirtName, 0, sizeof(uniVirtName));
    getMcastEthernetPortInformation(iidStack, brName, uniVirtName);
    updateMcastIgmpTable(BCM_MCAST_SNOOP_EX_CLEAR, dac, brName, uniVirtName);
    cmsMem_free(dac);
	
    return CMSRET_SUCCESS;
}

CmsRet rutGpon_updateDynamicAccessControl
	(const DynamicAccessControlListTableObject * currObj,
	DynamicAccessControlListTableObject *newObj,
	const InstanceIdStack *iidStack)
{
    UINT32 dacSize = 0;
    UINT8 *dac = NULL;
    char brName[16];
    char uniVirtName[16];
	
    if ((currObj == NULL) ||
		(newObj == NULL) ||
		(iidStack == NULL))
    {
    	return CMSRET_INTERNAL_ERROR;
    }

    if((strlen(currObj->dynamicAccessControlEntry) == OMCI_ENTRY_SIZE_48) &&
		strncmp(currObj->dynamicAccessControlEntry,
			newObj->dynamicAccessControlEntry,
			OMCI_ENTRY_SIZE_48))
    {
        /* found a entry need to be overwrite.
        here we delete current entry then apply new entry*/
        rutGpon_deleteDynamicAccessControl(currObj, iidStack);
    }

    /* now, we apply the new entry */
    cmsUtl_hexStringToBinaryBuf(newObj->dynamicAccessControlEntry,
    							&dac, &dacSize);
    if (dacSize != OMCI_MCAST_AC_SIZE)
    {
        cmsLog_error("Invalid data for %s %s",
                   _mdm_oidToGenericPath(newObj->_oid),
                   _cmsMdm_dumpIidStack(iidStack));
        cmsMem_free(dac);
        return CMSRET_INTERNAL_ERROR;
    }

    memset(brName, 0, sizeof(brName));
    memset(uniVirtName, 0, sizeof(uniVirtName));
    getMcastEthernetPortInformation(iidStack, brName, uniVirtName);
    updateMcastIgmpTable(BCM_MCAST_SNOOP_EX_ADD, dac, brName, uniVirtName);
    cmsMem_free(dac);
	
    return CMSRET_SUCCESS;
}

/*****************************************************************************
*  FUNCTION:  rutGpon_getMcastAclAniVidListFromUniMeId
*  PURPOSE:   Look up the dynamic ACL list to find the list of ANI (multicast)
*             VLAN ID(s).
*  PARAMETERS:
*      uniOid - UNI OID.
*      uniMeId - UNI ME id.
*      findAllB - whether to find all VLAN IDs or just the first one.
*      vidListNum - pointer to the number of VLAN IDs found.
*      vidListP - pointer to the list of VLAN IDs found.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet rutGpon_getMcastAclAniVidListFromUniMeId(UINT32 uniOid, UINT32 uniMeId,
  UBOOL8 findAllB, UINT32 *vidListNum, UINT16 *vidListP)
{
    UBOOL8 found = FALSE;
    UBOOL8 walkB = TRUE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MulticastOperationsProfileObject *mop = NULL;
    MulticastSubscriberConfigInfoObject msci;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
    UINT32 listNum = 0;

    memset(&msci, 0, sizeof(MulticastSubscriberConfigInfoObject));

    if (getMulticastSubscriberConfigInfoFromUniMeId(uniOid, uniMeId, &msci)
      != CMSRET_SUCCESS)
    {
        *vidListNum = 0;
        return CMSRET_OBJECT_NOT_FOUND;
    }

    while ((!found) && (_cmsObj_getNext(MDMOID_MULTICAST_OPERATIONS_PROFILE,
      &iidStack, (void**)&mop) == CMSRET_SUCCESS))
    {
        found = (msci.multicastOperationsProfilePointer ==
          mop->managedEntityId);
        if (found == TRUE)
        {
            UINT8 *buf = NULL;
            UINT32 bufSize = 0;
            InstanceIdStack iidStackChild = EMPTY_INSTANCE_ID_STACK;
            DynamicAccessControlListTableObject *dacEntry;
            UINT32 idx = 0, vid = 0, vidListMax = 0;

            /* Walk through DynamicAccessControlList table. */
            while ((walkB) && _cmsObj_getNextInSubTree
              (MDMOID_DYNAMIC_ACCESS_CONTROL_LIST_TABLE,
              &iidStack,
              &iidStackChild,
              (void**)&dacEntry) == CMSRET_SUCCESS)
            {
                ret = cmsUtl_hexStringToBinaryBuf
                  ((char*)dacEntry->dynamicAccessControlEntry, &buf, &bufSize);
                if ((ret == CMSRET_SUCCESS) && (bufSize == OMCI_MCAST_AC_SIZE))
                {
                    vid = getAccessControlField(buf, OMCI_MCAST_AC_VLAN_ID);
                    /* Check if vid already existed in vidList. */
                    for (idx = 0; idx < vidListMax; idx++)
                    {
                        if (vid == vidListP[idx])
                            break;
                    }
                    if ((idx == vidListMax) && (idx < MCAST_VID_LIST_MAX))
                    {
                        /* Add vid to vidList. */
                        vidListP[vidListMax++] = vid;
                        listNum++;
                        omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
                          "===> rutGpon_getMcastAclAniVidListFromUniMeId(), "
                          "vid=%d, vidListNum=%d\n",
                          vid, listNum);
                    }

                    walkB = (vidListMax < MCAST_VID_LIST_MAX) ? findAllB : FALSE;
                }
                CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
                _cmsObj_free((void**)&dacEntry);
            }
        }
        _cmsObj_free((void**)&mop);
    }

    *vidListNum = listNum;
    ret = (*vidListNum == 0) ? CMSRET_OBJECT_NOT_FOUND : CMSRET_SUCCESS;

    omciDebugPrint(OMCI_DEBUG_MODULE_MCAST,
      "===> rutGpon_getMcastAclAniVidListFromUniMeId(), vidListNum=%d, ret=%d\n",
      *vidListNum, ret);

    return ret;
}
#endif /* DMP_X_ITU_ORG_GPON_1 */
