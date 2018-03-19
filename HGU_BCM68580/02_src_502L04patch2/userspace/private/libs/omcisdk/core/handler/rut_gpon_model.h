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

#ifndef __RUT_GPON_MODEL_H__
#define __RUT_GPON_MODEL_H__


/*!\file rut_gpon_model.h
 * \brief System level interface functions for generic GPON functionality.
 *
 * The functions in this file should only be called by
 * RCL, STL, and other RUT functions.
 */


#include "os_defs.h"
#include "rut_gpon.h"
#include "rut_gpon_vlan.h"
#include "omci_dm_defs.h"

#if defined(CONFIG_BCM_GPON_802_1Q_ENABLED)
#define MAX_VLAN_DOMAINS  4
typedef struct {
    UINT32 vid;
    UINT32 untaggedMap;
    UINT32 taggedMap;
}vlanDomainInfo_t;

typedef struct {
    UINT32 numDomains;
    vlanDomainInfo_t domainInfo[MAX_VLAN_DOMAINS];
}vlanDomains_t;
#endif /* CONFIG_BCM_GPON_802_1Q_ENABLED */

//=======================  Public GPON Model functions ========================

/** check the OMCI mapping filtering model to verify
 *  there's any completed path from ANI to UNI that
 *  contains the object that is identified by the
 *  given checkOid and checkMeId
 *  This function is only applied for objects under
 *  Layer2DataServices object and PPTP objects
 *
 * @param checkOid     (IN) object class ID.
 * @param checkMeId    (IN) managed entity ID.
 * @param action       (IN) action on this checked object.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_checkModelPath
    (UINT32 checkOid, UINT32 checkMeId, OmciObjectAction action);

/** check the OMCI mapping filtering model from UNI to ANI
 *  to verify there's any completed path from ANI to UNI that
 *  contains the object that is identified by the
 *  given checkOid and checkMeId
 *  This function is only applied for objects under
 *  Layer2DataServices object and PPTP objects
 *
 * @param checkOid     (IN) object class ID.
 * @param checkMeId    (IN) managed entity ID.
 * @param action       (IN) action on this checked object.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_checkModelPath_US
    (UINT32 checkOid, UINT32 checkMeId, OmciObjectAction action);

/** configure the OMCI mapping filtering model with
 *  the current information in MDM.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_configModelPath(void);

/** get the number of ethernet ports managed
 *  and store it in the given ethLanNum
 *
 * @param ethLanNum     (OUT) number of ethernet ports
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getEthernetLanNum(UINT32 *ethLanNum);

/** get the management entity ID of the first
 *  Ethernet port and store it in the given meId
 *
 * @param meId     (OUT) management entity ID.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getFirstEthernetMeId(UINT32 *meId);

/** get the management entity ID of the first
 *  MoCA port and store it in the given meId
 *
 * @param meId     (OUT) management entity ID.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getFirstMocaMeId(UINT32 *meId);

#ifdef SUPPORT_GPONCTL
/** get GEM port Index from the given
 *  GemPortNetworkCtpObject
 *
 * @param ctp          (IN) GemPortNetworkCtpObject.
 * @param gemPortIndex (OUT) GEM port index.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getIndexFromGemPortNetworkCtp
    (const GemPortNetworkCtpObject *ctp, UINT32 *gemPortIndex);
#endif    //#ifdef SUPPORT_GPONCTL

/** get PBITS in MapperServiceProfileObject
 *  from the given GemPortNetworkCtpObject
 *
 * @param ctp        (IN) GemPortNetworkCtpObject.
 * @param pbits      (OUT) priority bits.
 * @param mapperMeId (OUT) MeId of MapperServiceProfileObject.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getPbitsFromGemPortNeworkCtp
    (const  GemPortNetworkCtpObject *ctp,
     UINT8  *pbits,
     UINT32 *mapperMeId);

/** get DscpToPbitInfo in MapperServiceProfileObject
 *  from the given GemPortNetworkCtpObject
 *
 * @param ctp        (IN) GemPortNetworkCtpObject.
 * @param dscpToPbit (OUT) pointer to OmciDscpPbitMappingInfo_t.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getDscpToPbitFromGemPortNeworkCtp
    (const  GemPortNetworkCtpObject *ctp,
     OmciDscpPbitMappingInfo_t *dscpToPbit);

/** get DscpToPbitInfo in ExtendedVlanTaggingOperationConfigurationDataObject
 *  from the given uniOid, and uniMeId of PptpXXXUniObject
 *
 * @param uniOid     (IN)  Object ID of PptpXXXUniObject.
 * @param uniMeId    (IN)  MeId of PptpXXXUniObject.
 * @param dscpToPbit (OUT) pointer to OmciDscpPbitMappingInfo_t.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getDscpToPbitFromUniMeId
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     OmciDscpPbitMappingInfo_t *dscpToPbit);

/** get MeId of VlanTaggingFilterDataObject
 *  from the given MeId of GemPortNetworkCtpObject
 *
 * @param ctpMeId     (IN)  MeId of GemPortNetworkCtpObject.
 * @param bridgeMeId  (IN)  MeId of MacBridgeServiceProfileObject.
 * @param pFilterMeId (OUT) MeId of VlanTaggingFilterDataObject
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getVlanFilterMeIdFromGemPortNetworkCtpMeId
    (const UINT32 ctpMeId,
     const UINT32 bridgeMeId,
     UINT32 *pFilterMeId);

/** get MeId of MacBridgeServiceProfileObject
 *  from the given MeId of PptpEthernetObject or PptpMocaUniObject
 *
 * @param uniOid      (IN) Object ID of PptpXXXUniObject.
 * @param uniMeId     (IN) MeId of PptpXXXUniObject.
 * @param pBridgeMeId (OUT) MeId of MacBridgeServiceProfileObject
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getBridgeMeIdFromUniMeId
    (const UINT32 uniOid, const UINT32 uniMeId, UINT32 *pBridgeMeId);

/** get MeId of MacBridgePortConfigDataObject
 *  from the given MeId of GemPortNetworkCtpObject and
 *  MeId of MacBridgeServiceProfileObject
 *
 * @param ctpMeId     (IN)  MeId of GemPortNetworkCtpObject.
 * @param bridgeMeId  (IN)  MeId of MacBridgeServiceProfileObject
 * @param pPortMeId   (OUT) MeId of MacBridgePortConfigDataObject
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getBridgePortMeIdFromGemPortNetworkCtpMeId
    (const UINT32 ctpMeId, const UINT32 bridgeMeId, UINT32 *pPortMeId);

/** find MacBridgePortConfigDataObject that can connect
 *  the given MacBridgeServiceProfileObject to any
 *  GemPortNetworkCtpObject except the given one
 *
 * @param ctpMeId     (IN)  MeId of GemPortNetworkCtpObject that should NOT be searched
 * @param bridgeMeId  (IN)  MeId of MacBridgeServiceProfileObject
 *
 * @return TRUE if find bridge port that connects bridge to any gem port
 *         except the given gem port. Otherwise return FALSE.
 */
UBOOL8 rutGpon_isBridgeConnectedToAnyGemPorts
    (const UINT32 ctpMeId, const UINT32 bridgeMeId);

/** get MeId of MacBridgePortConfigDataObject
 *  from the given MeId of PptpEthernetObject or PptpMocaUniObject
 *
 * @param uniOid      (IN)  Object ID of PptpXXXUniObject.
 * @param uniMeId     (IN)  MeId of PptpXXXUniObject.
 * @param pPortMeId   (OUT) MeId of MacBridgePortConfigDataObject
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getBridgePortMeIdFromUniMeId
    (const UINT32 uniOid, const UINT32 uniMeId, UINT32 *pPortMeId);

/** get MeId of ExtendedVlanTaggingOperationConfigurationDataObject
 *  from the given MeId of PptpXXXUniObject
 *
 * @param uniOId    (IN) CMS object ID of object that is linked to
 *                       ExtendedVlanTaggingOperationConfigurationDataObject.
 * @param uniMeId   (IN) MeId of object that is linked to
 *                       ExtendedVlanTaggingOperationConfigurationDataObject.
 * @param pOperMeId (OUT) MeId of ExtendedVlanTaggingOperationConfigurationDataObject
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getExtVlanOperMeIdFromUniMeId
    (const UINT32 uniOid, const UINT32 uniMeId, UINT32 *pOperMeId);

/** get MeId of VlanTaggingOperationConfigurationDataObject
 *  from the given MeId of PptpXXXUniObject
 *
 * @param uniOId    (IN) CMS object ID of object that is linked to
 *                       VlanTaggingOperationConfigurationDataObject.
 * @param uniMeId   (IN) MeId of object that is linked to
 *                       VlanTaggingOperationConfigurationDataObject.
 * @param pOperMeId (OUT) MeId of VlanTaggingOperationConfigurationDataObject
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getVlanOperMeIdFromUniMeId
    (const UINT32 uniOid, const UINT32 uniMeId, UINT32 *pOperMeId);

/** get Oid and MeId of PptpXXXUniObject from the given
 *  MeId, and meType of MulticastSubscriberMonitorObject
 *
 * @param subsMeId   (IN) MeId of MulticastSubscriberMonitorObject.
 * @param meType     (IN) meType of MulticastSubscriberMonitorObject.
 * @param pUniOid    (OUT) Oid of PptpXXXUniObject
 * @param pUniMeId   (OUT) MeId of PptpXXXUniObject
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getUniMeIdFromMcastSubsMeId
    (const UINT32 subsMeId, const UINT32 meType,
     UINT32 *pUniOid, UINT32 *pUniMeId);

/** get downstream/upstream priority queue id from the given
*  gem port index.
 *
 * @param gemPortIndex   (IN) gem port index.
 * @param  dir,   (IN) direction upstream/downstream
 * @param pQid (OUT) priority queue id
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getQidFromGemPortIndex
    (const UINT32 gemPortIndex, const OmciFLowDirection dir, UINT16 *pQid);

/** return TRUE if the given GemPortNetworkCtpObject
 *  is connected to MulticastGemInterworkingTpObject
 *
 * @param ctp          (IN) GemPortNetworkCtpObject.
 *
 * @return TRUE or FALSE.
 */
UBOOL8 rutGpon_isGemPortNetworkCtpMulticast
    (const GemPortNetworkCtpObject *ctp);

/** return TRUE if the given GemPortNetworkCtpObject
 *  is connected to GemInterworkingTpObject and
 *  its direction is downstream only
 *
 * @param ctp          (IN) GemPortNetworkCtpObject.
 *
 * @return TRUE or FALSE.
 */
UBOOL8 rutGpon_isGemPortNetworkCtpIncidentBroadcast
    (const GemPortNetworkCtpObject *ctp);

/** return TRUE if the given interface name is the
 *  interface of GemPortNetworkCtpObject that
 *  is connected to MulticastGemInterworkingTpObject
 *
 * @param interfaceName (IN) interface name.
 *
 * @return TRUE or FALSE.
 */
UBOOL8 rutGpon_isInterfaceAniMulticast
    (const char *interfaceName);

/** get bridge name from the given MeId of
 *  BCM_MacBridgeServiceProfileObject or BCM_MapperServiceProfileObject
 *
 * @param bridgeMeId   (IN) MeId of Bridge Object.
 * @param bridgeName   (OUT) bridge name
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getBridgeNameFromBridgeMeId
    (const UINT32 bridgeMeId, char *bridgeName);

/** get upstream or downstream priority (priority attribute of
 *  PriorityQueueGObject) from the given GemPortNetworkCtpObject
 *
 * @param pQInfoP(IN) PQ object.
 * @param dir    (IN) upstream or downstream
 * @param *pPrio (OUT) priority pointer.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getPrioFromPriorityQueue
    (PriorityQueueGObject *pQInfoP,
     const OmciFLowDirection dir,
     UINT16 *pPrio);

/** get queue index from a given OMCI ME instance
 *
 * @param priQMeId (IN)  Queue ME ID.
 * @param *pQid    (OUT) Queue index pointer.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getQidFromPriorityQueueV2
    (const UINT16 priQMeId,
     UINT16 *pQid,
     PriorityQueueGObject *pQInfoP);

/** get upstream or downstream priority (priority attribute of
 *  TrafficSchedulerGObject) from the given GemPortNetworkCtpObject
 *
 * @param ctp    (IN) GemPortNetworkCtpObject.
 * @param dir    (IN) upstream or downstream
 * @param *pPrio (OUT) priority pointer.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getPrioFromTrafficScheduler
    (const GemPortNetworkCtpObject *ctp,
     const OmciFLowDirection dir,
     UINT16 *pPrio);

/** get upstream or downstream weight (weight attribute of
 *  PriorityQueueGObject) from the given GemPortNetworkCtpObject
 *
 * @param pQInfoP  (IN) PQ object
 * @param dir      (IN) upstream or downstream
 * @param *pWeight (OUT) weight number.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getWeightFromPriorityQueue
    (PriorityQueueGObject *pQInfoP,
     const OmciFLowDirection dir,
     UINT16 *pWeight);

/** get upstream or downstream weight (priority attribute of
 *  TrafficSchedulerGObject) from the given GemPortNetworkCtpObject
 *
 * @param ctp      (IN) GemPortNetworkCtpObject.
 * @param dir      (IN) upstream or downstream
 * @param *pWeight (OUT) weight number.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getWeightFromTrafficScheduler
    (const GemPortNetworkCtpObject *ctp,
     const OmciFLowDirection dir,
     UINT16 *pWeight);

/** get policy of TCont from the
 *  given managed entity ID of TCont
 *
 * @param tContMeId (IN) managed entity ID of TCont.
 * @param *pPolicy  (OUT) policy of TCont.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_getTContPolicyFromTContMeId
    (const  UINT32 tContMeId,
     OmciSchedulePolicy *pPolicy);

/** find all connections from VEIP to CTPS.
 *  If path is found then send message to ssk
 *  to delete service interfaces (veip0.1, veip0.2, etc..)
 *  that is created from this path
 *
 *
 * @return none.
 */
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
void rutGpon_deleteAllVeipServices(void);
#endif   // DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1

//=======================  Private GPON Model functions ========================

CmsRet PptpMocaUniObject_searchModelPath
    (PptpMocaUniObject *pptpMoca,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachOid,
     UINT32 *reachMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet PptpEthernetUniObject_searchModelPath
    (PptpEthernetUniObject *pptpEth,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachOid,
     UINT32 *reachMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet IpHostConfigDataObject_searchModelPath
    (IpHostConfigDataObject *ipHost,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachOid,
     UINT32 *reachMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet Ipv6HostConfigDataObject_searchModelPath
    (Ipv6HostConfigDataObject *ipHost,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachOid,
     UINT32 *reachMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet VirtualEthernetInterfacePointObject_searchModelPath
    (VirtualEthernetInterfacePointObject *veip,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachOid,
     UINT32 *reachMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet VlanTaggingFilterDataObject_searchModelPath
    (VlanTaggingFilterDataObject *vlanFilter,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachOid,
     UINT32 *reachMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet VlanTaggingOperationConfigurationDataObject_searchModelPath
    (VlanTaggingOperationConfigurationDataObject *vlanOper,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachOid,
     UINT32 *reachMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet ExtendedVlanTaggingOperationConfigurationDataObject_searchModelPath
    (ExtendedVlanTaggingOperationConfigurationDataObject *xVlanOper,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachOid,
     UINT32 *reachMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet MacBridgeServiceProfileObject_searchModelPath
    (MacBridgeServiceProfileObject *bridge,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachOid,
     UINT32 *reachMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet MacBridgePortConfigDataObject_searchModelPath
    (MacBridgePortConfigDataObject *port,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachOid,
     UINT32 *reachMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet MapperServiceProfileObject_searchModelPath
    (MapperServiceProfileObject *mapper,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachOid,
     UINT32 *reachMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet GemInterworkingTpObject_searchModelPath
    (GemInterworkingTpObject *tp,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachOid,
     UINT32 *reachMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet MulticastGemInterworkingTpObject_searchModelPath
    (MulticastGemInterworkingTpObject *mcast,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet GemPortNetworkCtpObject_searchModelPath
    (GemPortNetworkCtpObject *ctp,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachOid,
     UINT32 *reachMeId,
     UINT32 depth,
     UBOOL8 *inPath);

CmsRet getVlanFilterMeIdFromMacBridgePortConfigDataObject
    (const MacBridgePortConfigDataObject *port,
     const UINT32 bridgeMeId,
     UINT32 *pFilterMeId);

CmsRet getVlanFilterMeIdFromGemInterworkingTpObject
    (const UINT32 tpMeId,
     const UINT32 bridgeMeId,
     const UINT32 multicast,
     UINT32 *pFilterMeId);

CmsRet getExtVlanOperMeIdFromUniMeId
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     UINT32 *pOperMeId);

CmsRet getVlanOperMeIdFromUniMeId
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     UINT32 *pOperMeId);

UBOOL8 getPbitsFromMapperServiceProfile
    (const UINT32 tpMeId,
     const MapperServiceProfileObject *mapper,
     UINT8 *pbits);

#endif /* __RUT_GPON_MODEL_H__ */
