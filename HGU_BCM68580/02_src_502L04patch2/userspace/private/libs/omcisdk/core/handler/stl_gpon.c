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
#include "mocalib.h"
#include "omci_pm.h"
#include "laser.h"


CmsRet stl_ituTOrgObject(_ItuTOrgObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_g_988Object(_G_988Object *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_equipmentManagementObject(_EquipmentManagementObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_ontGObject(_OntGObject *obj __attribute__((unused)),
                             const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_ont2GObject(_Ont2GObject *obj,
                          const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsTimestamp ts;

    cmsTms_get(&ts);
    //the unit is 10ms
    obj->sysUpTime = ts.sec * MSECS_IN_SEC / 10 + (ts.nsec % NSECS_IN_SEC) / NSECS_IN_MSEC / 10;

    obj->qosConfigFlexibiltiy = 48;

    return CMSRET_SUCCESS;
}

CmsRet stl_ontDataObject(_OntDataObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_softwareImageObject(_SoftwareImageObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_cardHolderObject(_CardHolderObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_circuitPackObject(_CircuitPackObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    BCM_Ploam_SerialPasswdInfo info;
    char buf[BUFLEN_64]={0};
    int ret = gponCtl_getSerialPasswd(&info);

    if (ret == 0)
    {
        sprintf(buf, "%02x%02x%02x%02x%02x%02x%02x%02x",
                (unsigned char) info.serialNumber[0], (unsigned char) info.serialNumber[1],
                (unsigned char) info.serialNumber[2], (unsigned char) info.serialNumber[3],
                (unsigned char) info.serialNumber[4], (unsigned char) info.serialNumber[5],
                (unsigned char) info.serialNumber[6], (unsigned char) info.serialNumber[7]);
        if(obj->serialNumber)
        {
            cmsMem_free(obj->serialNumber);
        }
        obj->serialNumber = cmsMem_strdupFlags(buf, 0);
    }
    return CMSRET_SUCCESS;
}
CmsRet stl_powerSheddingObject(_PowerSheddingObject *obj __attribute__((unused)),
                               const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_portMappingPackageGObject(_PortMappingPackageGObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_aniManagementObject(_AniManagementObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_aniGObject(_AniGObject * obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{

    CmsRet RetVal = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    unsigned long TempPower[2] = {0,0};
    int LaserFd;
    UBOOL8 srIndication;

    if (rutgpon_getSRIndication(&srIndication) == CMSRET_SUCCESS)
        obj->srIndication = srIndication;

    // Attempt to open laser pipe.
    LaserFd = open("/dev/laser_dev", O_RDWR);
    if (LaserFd >= 0)
    {
        if (ioctl(LaserFd, LASER_IOCTL_GET_RX_PWR, &TempPower[0]) >= 0)
        {
            if (ioctl(LaserFd, LASER_IOCTL_GET_TX_PWR, &TempPower[1]) >= 0)
            {
                obj->opticalSignalLevel =
                  convertPointOneMicroWattsToOmcidB("RX", TempPower[0], 
                    -32768, 32767, 2, 1);
                obj->transmitOpticalLevel =
                  convertPointOneMicroWattsToOmcidB("TX", TempPower[1], 
                    -32768, 32767, 2, 1);

                RetVal = CMSRET_SUCCESS;
            }
            else
            {
                cmsLog_error("Laser driver IOCTL error on Transmit Optical Signal Level");
            }
        }
        else
        {
            cmsLog_error("Laser driver IOCTL error on Receive Optical Signal Level");
        }

        close(LaserFd);
    }
    else
    {
        cmsLog_error("Laser driver open error");
    }

    return RetVal;
}
CmsRet stl_tContObject(_TContObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_gemPortNetworkCtpObject(_GemPortNetworkCtpObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_gemInterworkingTpObject(_GemInterworkingTpObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_multicastGemInterworkingTpObject(_MulticastGemInterworkingTpObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_gemInterworkingTpMulticastAddressTableObject(_GemInterworkingTpMulticastAddressTableObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_gemPortPmHistoryDataObject(_GemPortPmHistoryDataObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_GEM_PORT_COUNTER_64* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_GEM_PORT_PM_HISTORY_DATA,
                          obj->managedEntityId,
                          (void **)&pData,
                          &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_GEM_PORT_PM_HISTORY_DATA,
                                 obj->managedEntityId,
                                 (void **)&pData,
                                 &dataLen);
    }

    _cmsObj_free((void **) &cmsObj);

    if (dataLen != 0 && pData != NULL)
    {
        // Setup reply interval value.
        obj->intervalEndTime = omci_pm_getIntervalCounter();

        // Setup 64-bit reply stats first.
        obj->receivedPayloadBytes = pData->receivedPayloadBytes_64;
        obj->transmittedPayloadBytes = pData->transmittedPayloadBytes_64;

        // Setup 32-bit reply stats last.
        obj->transmittedGEMFrames = pData->transmittedGEMFrames;
        obj->receivedGEMFrames = pData->receivedGEMFrames;

        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}
CmsRet stl_galEthernetProfileObject(_GalEthernetProfileObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_galEthernetPmHistoryDataObject(_GalEthernetPmHistoryDataObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_GAL_ETHERNET_COUNTER* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_GAL_ETHERNET_PM_HISTORY_DATA,
                          obj->managedEntityId,
                          (void **)&pData,
                          &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_GAL_ETHERNET_PM_HISTORY_DATA,
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
        obj->discardedFrames = pData->discardedFrames;

        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}
CmsRet stl_fecPmHistoryDataObject(_FecPmHistoryDataObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_FEC_COUNTER* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_FEC_PM_HISTORY_DATA,
                          obj->managedEntityId,
                          (void **)&pData,
                          &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_FEC_PM_HISTORY_DATA,
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
        obj->correctedBytes = pData->correctedBytes;
        obj->correctedCodeWords = pData->correctedCodeWords;
        obj->uncorrectedCodeWords = pData->uncorrectedCodeWords;
        obj->totalCodeWords = pData->totalCodeWords;
        obj->fecSeconds = pData->fecSeconds;

        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}
CmsRet stl_layer2DataServicesObject(_Layer2DataServicesObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_macBridgeServiceProfileObject(_MacBridgeServiceProfileObject *obj __attribute__((unused)),
                                         const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_bCM_MacBridgeServiceProfileObject(
                                    _BCM_MacBridgeServiceProfileObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_macBridgeConfigDataObject(_MacBridgeConfigDataObject *obj __attribute__((unused)),
                                     const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_macBridgePmHistoryDataObject(_MacBridgePmHistoryDataObject *obj __attribute__((unused)),
                                        const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_MAC_BRIDGE_COUNTER* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_MAC_BRIDGE_PM_HISTORY_DATA,
                       obj->managedEntityId,
                       (void **)&pData,
                       &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_MAC_BRIDGE_PM_HISTORY_DATA,
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
        obj->learningEntryDiscardCounter = pData->learningDiscaredEntries;

        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}
CmsRet stl_macBridgePortConfigDataObject(_MacBridgePortConfigDataObject *obj __attribute__((unused)),
                                         const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_macBridgePortDesignationDataObject(_MacBridgePortDesignationDataObject *obj __attribute__((unused)),
                                              const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_macBridgePortFilterTableDataObject(_MacBridgePortFilterTableDataObject *obj __attribute__((unused)),
                                              const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_macFilterTableObject(_MacFilterTableObject *obj __attribute__((unused)),
                                const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_macBridgePortFilterPreAssignTableObject(_MacBridgePortFilterPreAssignTableObject *obj __attribute__((unused)),
                                              const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_macBridgePortBridgeTableDataObject(_MacBridgePortBridgeTableDataObject *obj __attribute__((unused)),
                                              const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_bridgeTableObject(_BridgeTableObject *obj __attribute__((unused)),
                             const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_macBridgePortPmHistoryDataObject(_MacBridgePortPmHistoryDataObject *obj __attribute__((unused)),
                                            const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA,
                          obj->managedEntityId,
                          (void **)&pData,
                          &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA,
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
        obj->delayDiscardCounter = pData->delayDiscardedFrames;
        obj->forwardFrameCounter = pData->forwardedFrames;
        obj->mtuDiscardCounter = pData->mtuDiscardedFrames;
        obj->receiveDiscardCounter = pData->receivedDiscardedFrames;
        obj->receiveFrameCounter = pData->receivedFrames;

        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}
CmsRet stl_mapperServiceProfileObject(_MapperServiceProfileObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_bCM_MapperServiceProfileObject(
                                    _BCM_MapperServiceProfileObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_vlanTaggingFilterDataObject(_VlanTaggingFilterDataObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_vlanTaggingOperationConfigurationDataObject(_VlanTaggingOperationConfigurationDataObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_extendedVlanTaggingOperationConfigurationDataObject(_ExtendedVlanTaggingOperationConfigurationDataObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_receivedFrameVlanTaggingOperationTableObject(_ReceivedFrameVlanTaggingOperationTableObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_multicastOperationsProfileObject(_MulticastOperationsProfileObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_dynamicAccessControlListTableObject(_DynamicAccessControlListTableObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_staticAccessControlListTableObject(_StaticAccessControlListTableObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_lostGroupsListTableObject(_LostGroupsListTableObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_multicastSubscriberConfigInfoObject(_MulticastSubscriberConfigInfoObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_multicastSubscriberMonitorObject(_MulticastSubscriberMonitorObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    UINT32 uniOid = 0, uniMeId = 0;
    CmsRet ret = CMSRET_SUCCESS_OBJECT_UNCHANGED;

    ret = rutGpon_getUniMeIdFromMcastSubsMeId(obj->managedEntityId, obj->meType,
                                              &uniOid, &uniMeId);
    if ((ret == CMSRET_SUCCESS) &&
        ((uniOid == MDMOID_PPTP_ETHERNET_UNI || uniOid == MDMOID_PPTP_MOCA_UNI) ||
        (uniOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT)))
    {
        obj->currentMulticastBandwidth = rutGpon_getCurrentMcastBandwidth(uniOid, uniMeId);
        obj->joinMessagesCounter = rutGpon_getJoinMessageCounter(uniOid, uniMeId);
        obj->bandwidthExceededCounter = rutGpon_getBandwidthExceededCounter(uniOid, uniMeId);
        ret = rutGpon_updateActiveGroupList(uniOid, uniMeId, iidStack);
    }
    else
        ret = CMSRET_SUCCESS_OBJECT_UNCHANGED;

    cmsLog_debug("returning %d", ret);

    return ret;
}
CmsRet stl_activeGroupsListTableObject(_ActiveGroupsListTableObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_bCM_MulticastSubscriberMonitorObject(
                                    _BCM_MulticastSubscriberMonitorObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_multicastSubscriberObject(
                                    _MulticastSubscriberObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_layer3DataServicesObject(_Layer3DataServicesObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_ipHostConfigDataObject(_IpHostConfigDataObject *obj __attribute__((unused)),
                                  const InstanceIdStack *iidStack __attribute__((unused)))
{
    omciIpv4HostInfo iphostInfo;
    char *hexStr = NULL;

    /* Obtain the information via RUT API. */
    if (_owapi_rut_getIpv4HostInfo(obj->managedEntityId, &iphostInfo)
      == CMSRET_SUCCESS)
    {
        obj->currentAddress = iphostInfo.currentAddress;
        obj->currentMask = iphostInfo.currentMask;
        obj->currentGateway = iphostInfo.currentGateway;
        obj->currentPrimaryDns = iphostInfo.currentPrimaryDns;
        obj->currentSecondaryDns = iphostInfo.currentSecondaryDns;
        if (cmsUtl_binaryBufToHexString((UINT8*)iphostInfo.hostName,
          OWRUT_HOST_NAME_LEN, &hexStr) == CMSRET_SUCCESS)
        {
            CMSMEM_REPLACE_STRING(obj->hostName, hexStr);
            CMSMEM_FREE_BUF_AND_NULL_PTR(hexStr);
        }

        if (cmsUtl_binaryBufToHexString((UINT8*)iphostInfo.domainName,
          OWRUT_DOMAIN_NAME_LEN, &hexStr) == CMSRET_SUCCESS)
        {
            CMSMEM_REPLACE_STRING(obj->domainName, hexStr);
            CMSMEM_FREE_BUF_AND_NULL_PTR(hexStr);
        }

        return CMSRET_SUCCESS;
    }

    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_bCM_IpHostConfigDataObject(
                                    _BCM_IpHostConfigDataObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_ipHostPmHistoryDataObject(_IpHostPmHistoryDataObject *obj,
                                     const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_IP_HOST_COUNTER* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_IP_HOST_PM_HISTORY_DATA,
                          obj->managedEntityId,
                          (void **)&pData,
                          &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_IP_HOST_PM_HISTORY_DATA,
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
        obj->icmpErrors = pData->icmpErrors;
        obj->dnsErrors = pData->dnsErrors;

        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}
CmsRet stl_ipv6HostConfigDataObject( _Ipv6HostConfigDataObject *obj __attribute__((unused)),
                                  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_bCM_Ipv6HostConfigDataObject( _BCM_Ipv6HostConfigDataObject *obj __attribute__((unused)),
                                  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_ipv6CurrentAddressTableObject( _Ipv6CurrentAddressTableObject *obj __attribute__((unused)),
                                  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_ipv6CurrentDefaultRouterTableObject( _Ipv6CurrentDefaultRouterTableObject *obj __attribute__((unused)),
                                  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_ipv6CurrentDnsTableObject( _Ipv6CurrentDnsTableObject *obj __attribute__((unused)),
                                  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_ipv6CurrentOnlinkPrefixTableObject( _Ipv6CurrentOnlinkPrefixTableObject *obj __attribute__((unused)),
                                  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_tcpUdpConfigDataObject(_TcpUdpConfigDataObject *obj __attribute__((unused)),
                                   const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_ethernetServicesObject(_EthernetServicesObject *obj __attribute__((unused)),
                                   const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_pptpEthernetUniObject(_PptpEthernetUniObject *obj __attribute__((unused)),
                          const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;

    if (obj != NULL &&
        rutGpon_isVeipPptpUni(MDMOID_PPTP_ETHERNET_UNI, obj->managedEntityId) == FALSE)
    {
#ifndef G9991
        UINT32 firstMeId = 0, portIndex = 0, portInfo = 0;

        rutGpon_getFirstEthernetMeId(&firstMeId);

        portIndex = obj->managedEntityId - firstMeId;

        cmsReturn = rutGpon_getEnetInfo(portIndex, &portInfo);

        if (cmsReturn == CMSRET_SUCCESS)
        {
            // Setup reply configuration type.
            obj->configurationInd = portInfo;
            // Set line sensed-type field to 10/100/1000.
            obj->sensedType = LINE_SENSE_10_100_1000;
        }
        else
        {
            // Reset cmsReturn to return without error
            cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
        }
#else
        obj->sensedType = LINE_SENSE_10_100_1000;
        obj->configurationInd = OMCI_LINE_SENSE_1000 | OMCI_LINE_FULL_DUPLEX;
#endif
    }

    return cmsReturn;
}

CmsRet stl_ethernetPmHistoryDataObject(_EthernetPmHistoryDataObject *obj __attribute__((unused)),
                         const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_ETHERNET_COUNTER* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_ETHERNET_PM_HISTORY_DATA,
                          obj->managedEntityId,
                          (void **)&pData,
                          &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_ETHERNET_PM_HISTORY_DATA,
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
        obj->fcsErrors = pData->fcsErrors;
        obj->excessiveCollisionCounter = pData->excessiveCollisionCounter;
        obj->lateCollisionCounter = pData->lateCollisionCounter;
        obj->frameTooLongs = pData->frameTooLongs;
        obj->bufferOverflowsOnReceive = pData->bufferOverflowsOnReceive;
        obj->bufferOverflowsOnTransmit = pData->bufferOverflowsOnTransmit;
        obj->singleCollisionFrameCounter = pData->singleCollisionFrameCounter;
        obj->multipleCollisionsFrameCounter = pData->multipleCollisionsFrameCounter;
        obj->sqeCounter = pData->sqeCounter;
        obj->deferredTransmissionCounter = pData->deferredTransmissionCounter;
        obj->internalMacTransmitErrorCounter = pData->internalMacTransmitErrorCounter;
        obj->carrierSenseErrorCounter = pData->carrierSenseErrorCounter;
        obj->alignmentErrorCounter = pData->alignmentErrorCounter;
        obj->internalMacReceiveErrorCounter = pData->internalMacReceiveErrorCounter;

        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}
CmsRet stl_ethernetPmHistoryData2Object(_EthernetPmHistoryData2Object *obj __attribute__((unused)),
                                       const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_ETHERNET_2_COUNTER* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_ETHERNET_PM_HISTORY_DATA2,
                          obj->managedEntityId,
                          (void **)&pData,
                          &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_ETHERNET_PM_HISTORY_DATA2,
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
        obj->pppoeFilterFrameCounter = pData->pppoeFilterFrameCounter;

        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}
CmsRet stl_ethernetPmHistoryData3Object(_EthernetPmHistoryData3Object *obj __attribute__((unused)),
                                        const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_ETHERNET_3_COUNTER* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_ETHERNET_PM_HISTORY_DATA3,
                          obj->managedEntityId,
                          (void **)&pData,
                          &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_ETHERNET_PM_HISTORY_DATA3,
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
        obj->dropEvents = pData->dropEvents;
        obj->octets = pData->octets;
        obj->packets = pData->packets;
        obj->broadcastPackets = pData->broadcastPackets;
        obj->multicastPackets = pData->multicastPackets;
        obj->undersizePackets = pData->undersizePackets;
        obj->fragments = pData->fragments;
        obj->jabbers = pData->jabbers;
        obj->packets64Octets = pData->packets64Octets;
        obj->packets127Octets = pData->packets127Octets;
        obj->packets255Octets = pData->packets255Octets;
        obj->packets511Octets = pData->packets511Octets;
        obj->packets1023Octets = pData->packets1023Octets;
        obj->packets1518Octets = pData->packets1518Octets;
        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}


CmsRet stl_upstreamEthernetFramePmHistoryDataObject(_UpstreamEthernetFramePmHistoryDataObject *obj __attribute__((unused)),
                                        const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA,
                          obj->managedEntityId,
                          (void **)&pData,
                          &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA,
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
        obj->dropEvents = pData->dropEvents;
        obj->octets = pData->octets;
        obj->packets = pData->packets;
        obj->broadcastPackets = pData->broadcastPackets;
        obj->multicastPackets = pData->multicastPackets;
        obj->crcErroredPackets = pData->crcErroredPackets;
        obj->undersizePackets = pData->undersizePackets;
        obj->oversizePackets = pData->oversizePackets;
        obj->packets64Octets = pData->packets64Octets;
        obj->packets127Octets = pData->packets127Octets;
        obj->packets255Octets = pData->packets255Octets;
        obj->packets511Octets = pData->packets511Octets;
        obj->packets1023Octets = pData->packets1023Octets;
        obj->packets1518Octets = pData->packets1518Octets;

        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}


CmsRet stl_downstreamEthernetFramePmHistoryDataObject(_DownstreamEthernetFramePmHistoryDataObject *obj __attribute__((unused)),
                                        const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA,
                          obj->managedEntityId,
                          (void **)&pData,
                          &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA,
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
        obj->dropEvents = pData->dropEvents;
        obj->octets = pData->octets;
        obj->packets = pData->packets;
        obj->broadcastPackets = pData->broadcastPackets;
        obj->multicastPackets = pData->multicastPackets;
        obj->crcErroredPackets = pData->crcErroredPackets;
        obj->undersizePackets = pData->undersizePackets;
        obj->oversizePackets = pData->oversizePackets;
        obj->packets64Octets = pData->packets64Octets;
        obj->packets127Octets = pData->packets127Octets;
        obj->packets255Octets = pData->packets255Octets;
        obj->packets511Octets = pData->packets511Octets;
        obj->packets1023Octets = pData->packets1023Octets;
        obj->packets1518Octets = pData->packets1518Octets;

        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}

CmsRet stl_virtualEthernetInterfacePointObject(_VirtualEthernetInterfacePointObject *obj __attribute__((unused)),
                                   const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_mocaServicesObject(_MocaServicesObject *obj __attribute__((unused)),
                                             const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_pptpMocaUniObject(_PptpMocaUniObject *obj __attribute__((unused)),
                           const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_brcmPptpMocaUniObject(_BrcmPptpMocaUniObject *obj __attribute__((unused)),
                           const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


#ifdef SUPPORT_MOCA

CmsRet stl_mocaStatusObject(_MocaStatusObject *obj, const InstanceIdStack *iidStack)
{
   struct moca_drv_info drv_info;
   struct moca_node_status node_status;
   struct moca_fw_version fw_version;
   struct moca_network_status net_status;
   struct moca_interface_status if_status;
   void * pMoca = NULL;
   UINT32 id;
   CmsRet ret = 0;

   id = PEEK_INSTANCE_ID(iidStack);
   cmsLog_debug("entered, instanceId=%d", id);
   if (id != 1)
   {
      cmsLog_error("multiple instances of moca not supported, got %d, expected 1", id);
   }

   if (obj != NULL)
   {
      pMoca = moca_open(NULL);

      if (pMoca != NULL)
      {
         ret = moca_get_node_status(pMoca, &node_status);
         ret |= moca_get_drv_info(pMoca, 0, &drv_info);
         ret |= moca_get_fw_version(pMoca, &fw_version);
         ret |= moca_get_network_status(pMoca, &net_status);
         ret |= moca_get_interface_status(pMoca, &if_status);

         if (ret == 0)
         {
            /* copy from driver structure to TR-098 object */
            obj->vendorId = node_status.vendor_id;
            obj->hwVersion = node_status.moca_hw_version;
            obj->softwareVersion = node_status.moca_sw_version_major;
            obj->selfMoCAVersion = node_status.self_moca_version;
            obj->networkVersionNumber = net_status.network_moca_version;
            obj->qam256Support = node_status.qam_256_support;
            obj->operationalStatus = 1;
            obj->linkStatus = if_status.link_status;
            obj->connectedNodes = net_status.connected_nodes;
            obj->nodeId = net_status.node_id;
            obj->networkControllerNodeId = net_status.nc_node_id;
            obj->upTime = drv_info.core_uptime;
            obj->linkUpTime = drv_info.link_uptime;
            obj->backupNetworkControllerNodeId = net_status.backup_nc_id;
            obj->rfChannel = if_status.rf_channel;
            obj->bwStatus = net_status.bw_status;
         }
         else
         {
            cmsLog_error("getStatistics failed, ret=%d", ret);
         }

         moca_close(pMoca);
      }
      else
      {
         cmsLog_error("moca_open failed, ret=%d", ret);
      }
   }
   else
   {
      /* obj == NULL means reset, not supported for this object */
      cmsLog_error("reset not valid on this object");
   }

   cmsLog_debug("returning %d", ret);

   return ret;
}


CmsRet stl_mocaStatsObject(_MocaStatsObject *obj, const InstanceIdStack *iidStack)
{
   void * pMoca = NULL;
   struct moca_gen_stats stats;
   UINT32 id;
   int ret = 0;

   id = PEEK_INSTANCE_ID(iidStack);
   cmsLog_debug("entered, instanceId=%d", id);
   if (id != 1)
   {
      cmsLog_error("multiple instances of moca not supported, got %d, expected 1", id);
   }

   memset(&stats, 0, sizeof(struct moca_gen_stats));

   pMoca = moca_open(NULL);

   if (obj != NULL)
   {
      if (pMoca != NULL)
      {
         ret = moca_get_gen_stats(pMoca, 0, &stats);
         if (ret == 0)
         {
            obj->inUcPkts = stats.ecl_tx_ucast_pkts;
            obj->inDiscardPktsEcl = stats.ecl_tx_mcast_drops + stats.ecl_tx_ucast_drops;
            obj->inDiscardPktsMac = 0; // TBD
            obj->inUnKnownPkts = stats.ecl_tx_ucast_unknown + stats.ecl_tx_mcast_unknown;
            obj->inMcPkts = stats.ecl_tx_mcast_pkts;
            obj->inBcPkts = stats.ecl_tx_bcast_pkts;
            obj->inOctetsLow = stats.ecl_tx_total_bytes;
            obj->outUcPkts = stats.ecl_rx_ucast_pkts;
            obj->outDiscardPkts = stats.ecl_rx_mcast_filter_pkts + stats.ecl_rx_ucast_drops;
            obj->outBcPkts = stats.ecl_rx_bcast_pkts;
            obj->outOctetsLow = stats.ecl_rx_total_bytes;
            obj->networkControllerHandOffs = stats.nc_handoff_counter;
            obj->networkControllerBackups = stats.nc_backup_counter;
         }
         else
         {
            cmsLog_error("getStatistics failed, ret=%d", ret);
         }
      }

      moca_close(pMoca);
   }
   else
   {
      /* obj == NULL means reset stats */
      cmsLog_debug("reset stats");
      ret = moca_set_reset_stats(pMoca);
   }

   moca_close(pMoca);
   cmsLog_debug("returning %d", ret);

   return ret;
}

#else

/*
 * These versions of the function will be used on 96369PGW systems, where
 * GPON is defined, but Moca is not defined.
 */

CmsRet stl_mocaStatusObject(_MocaStatusObject *obj __attribute__((unused)), const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_mocaStatsObject(_MocaStatsObject *obj __attribute__((unused)), const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

#endif  /* SUPPORT_MOCA */


CmsRet stl_mocaEthernetPmHistoryDataObject(_MocaEthernetPmHistoryDataObject *obj __attribute__((unused)),
                           const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_MOCA_ETHERNET_COUNTER* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA,
                          obj->managedEntityId,
                          (void **)&pData,
                          &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA,
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
        obj->incomingUnicastPackets = pData->incomingUnicastPackets;
        obj->incomingDiscardedPackets = pData->incomingDiscardedPackets;
        obj->incomingErroredPackets = pData->incomingErroredPackets;
        obj->incomingUnknownPackets = pData->incomingUnknownPackets;
        obj->incomingMulticastPackets = pData->incomingMulticastPackets;
        obj->incomingBroadcastPackets = pData->incomingBroadcastPackets;
        obj->incomingOctets = pData->incomingOctets_low;                  // NOTE: Only least-significant 32-bits returned.
        obj->outgoingUnicastPackets = pData->outgoingUnicastPackets;
        obj->outgoingDiscardedPackets = pData->outgoingDiscardedPackets;
        obj->outgoingErroredPackets = pData->outgoingErroredPackets;
        obj->outgoingUnknownPackets = pData->outgoingUnknownPackets;
        obj->outgoingMulticastPackets = pData->outgoingMulticastPackets;
        obj->outgoingBroadcastPackets = pData->outgoingBroadcastPackets;
        obj->outgoingOctets = pData->outgoingOctets_low;                  // NOTE: Only least-significant 32-bits returned.

        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}


CmsRet stl_mocaInterfacePmHistoryDataObject(_MocaInterfacePmHistoryDataObject *obj __attribute__((unused)),
                                  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_mocaInterfacePmHistoryDataNodeTableObject(_MocaInterfacePmHistoryDataNodeTableObject *obj __attribute__((unused)),
                                  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_trafficManagementObject(_TrafficManagementObject *obj __attribute__((unused)),
                          const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_priorityQueueGObject(_PriorityQueueGObject *obj __attribute__((unused)),
                                           const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_trafficSchedulerGObject(_TrafficSchedulerGObject *obj __attribute__((unused)),
                                                 const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_gemTrafficDescriptorObject(_GemTrafficDescriptorObject *obj __attribute__((unused)),
                                     const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_generalObject(_GeneralObject *obj __attribute__((unused)),
                                 const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_uniGObject(_UniGObject *obj __attribute__((unused)),
                                  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_oltGObject(_OltGObject *obj __attribute__((unused)),
                                          const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_networkAddressObject(_NetworkAddressObject *obj __attribute__((unused)),
                                const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_authenticationSecurityMethodObject(_AuthenticationSecurityMethodObject *obj __attribute__((unused)),
                                              const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_onuRemoteDebugObject(_OnuRemoteDebugObject *newObj __attribute__((unused)),
  const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_largeStringObject(_LargeStringObject *obj __attribute__((unused)),
                             const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_thresholdData1Object(_ThresholdData1Object *obj __attribute__((unused)),
                                       const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_thresholdData2Object(_ThresholdData2Object *obj __attribute__((unused)),
                                              const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
CmsRet stl_omciObject(_OmciObject *obj __attribute__((unused)),
                      const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_gponOmciStatsObject(_GponOmciStatsObject *obj __attribute__((unused)),
                               const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_debugObject(_DebugObject *obj __attribute__((unused)),
                            const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_omciSystemObject(_OmciSystemObject *obj __attribute__((unused)),
                            const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_tr069ManagementServerObject(_Tr069ManagementServerObject *obj __attribute__((unused)), const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS;
}

CmsRet stl_ethernetFrameExtendedPMObject(_EthernetFrameExtendedPMObject *obj,
                                        const InstanceIdStack *iidStack __attribute__((unused)))
{
    CmsRet cmsReturn = CMSRET_SUCCESS_OBJECT_UNCHANGED;
    UINT16 dataLen = 0;
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER* pData = NULL;
    InstanceIdStack cmsIidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *cmsObj = NULL;

    if ((cmsReturn = _cmsObj_get(MDMOID_OMCI_SYSTEM, &cmsIidStack, 0, (void *) &cmsObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", cmsReturn);
        return CMSRET_SUCCESS_OBJECT_UNCHANGED;
    }

    if (cmsObj->getPmMode == OMCI_PM_GET_15_MINUTES)
    {
        omci_pm_getObject(MDMOID_ETHERNET_FRAME_EXTENDED_P_M,
                          obj->managedEntityId,
                          (void **)&pData,
                          &dataLen);
    }
    else
    {
        omci_pm_getCurrentObject(MDMOID_ETHERNET_FRAME_EXTENDED_P_M,
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
        obj->dropEvents = pData->dropEvents;
        obj->octets = pData->octets;
        obj->frames = pData->packets;
        obj->broadcastFrames = pData->broadcastPackets;
        obj->multicastFrames = pData->multicastPackets;
        obj->crcErroredFrames = pData->crcErroredPackets;
        obj->undersizeFrames = pData->undersizePackets;
        obj->oversizeFrames = pData->oversizePackets;
        obj->frames64Octets = pData->packets64Octets;
        obj->frames127Octets = pData->packets127Octets;
        obj->frames255Octets = pData->packets255Octets;
        obj->frames511Octets = pData->packets511Octets;
        obj->frames1023Octets = pData->packets1023Octets;
        obj->frames1518Octets = pData->packets1518Octets;

        cmsReturn = CMSRET_SUCCESS;
    }

    return cmsReturn;
}
#if 0
CmsRet stl_(_ *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
    return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
#endif

#endif /* DMP_X_ITU_ORG_GPON_1 */
