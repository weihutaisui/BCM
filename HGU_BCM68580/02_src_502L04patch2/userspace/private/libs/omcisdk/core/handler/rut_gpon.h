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

#ifndef __RUT_GPON_H__
#define __RUT_GPON_H__


/*!\file rut_gpon.h
 * \brief System level interface functions for generic GPON functionality.
 *
 * The functions in this file should only be called by
 * RCL, STL, and other RUT functions.
 */


#include "os_defs.h"
#include "omcipm_api.h"
#include "omci_util.h"
#include "omciobj_defs.h"
#include "omcicmn_defs.h"
#include "omcicust_defs.h"


#define Q_TAG_TPID                 0x8100
#define C_TAG_TPID                 0x8100
#define S_TAG_TPID                 0x88a8
#define D_TAG_TPID                 0x9100

#define ETH_TYPES_MAX              3

#define SUPPORT_DOUBLE_TAGS

#define CMS_OMCID_REQUEST_TIMEOUT (2 * MSECS_IN_SEC)

typedef OmciGponWanServiceType GemPortType_t;

typedef struct
{
    UINT32 phyId;
    OmciPhyType phyType;
    GemPortType_t gemPortType;
} OmciPhyInfo_t;

/* OMCI data model information associated with the UNI port. */
typedef struct
{
    UINT32 uniOid;
    UINT32 uniMeId;
    UINT32 bridgeMeId;
    UINT32 macBpMeId;
    UINT32 extVlanMeId;
    UBOOL8 isMapper;
    OmciPhyType phyType;
    UINT32 phyId;
    char devInterface[CMS_IFNAME_LENGTH];
    char vlanInterface[CMS_IFNAME_LENGTH];
} OmciUniInfo_t;

#ifdef CMS_LOG3
#define OMCI_DEBUG_SUPPORT
#endif

#ifdef OMCI_DEBUG_SUPPORT
#define omciDebugPrint(module, fmt, arg...)                               \
    do {                                                                  \
        if (rutGpon_getOmciDebugModule(module) == 1)                      \
        {                                                                 \
            FILE *fs = rutGpon_getOmciDebugFile();                        \
            if (fs != NULL &&                                             \
                rutGpon_getOmciDebugModule(OMCI_DEBUG_MODULE_FILE) == 1)  \
                fprintf(fs, fmt, ##arg);                                  \
            else                                                          \
                printf(fmt, ##arg);                                       \
        }                                                                 \
    } while (0)
#else
#define omciDebugPrint(module, fmt, arg...)
#endif

#define OMCI_DONT_CARE (~0)

#define MAX_FLOWS_PER_GEM_PORT     104

#define MAX_ID_STACK_DEPTH 6

/*
 * Downstream queue id range: 0 to 0x7FFF;
 * Upstream queue id range: 0x8000 to 0xFFFF.
 */
#define IS_DS_QUEUE(omciQId) (omciQId <= 0x7fff)
#define GetRelatedPortSlotPort(rp) ((rp >> 16) & 0xFFFF)
#define GetRelatedPortPrio(rp) (rp & 0xFFFF)

/*
 * In TMCtl API, priority value definition is: larger value, higher priority.
 * In OMCI, the priority value definition is: larger value, lower priority.
 */
#define omciTmctlPrioConvert(omciPrio) (OMCI_PRIO_MAX - omciPrio)

/* Check whether this is a UNI-side MAC bridge port. */
#define isUniMacBpcd(tpType) \
  ((tpType == OMCI_BP_TP_PPTP_ETH) || (tpType == OMCI_BP_TP_PPTP_MOCA) || \
  (tpType == OMCI_BP_TP_IP_HOST) || (tpType == OMCI_BP_TP_VEIP))

/** Macro to determine if this is POTENTIALLY a modification of an
 * existing object.
 *
 * This macro must be used in conjunction with another function which
 * you provide that determines if any of the fields in the object has
 * changed.  This macro only verifies that the object currently exists
 * and is enabled. Example usage:
 *
 * if (SET_EXISTING(newObj, currObj) && fieldsHaveChanged(newObj, currObj)
 *
 */
#define SET_EXISTING(n, c) \
   (((n) != NULL && (c) != NULL))

#define US_PQ_MAX_SIZE TMCTL_DEF_ETH_Q_SZ_US
#define DS_PQ_MAX_SIZE TMCTL_DEF_ETH_Q_SZ_DS

#define ME_ADMIN_STATE_UNLOCKED 0
#define ME_ADMIN_STATE_LOCKED   1

/*
** The object deletion during OMCI MIB reset may trigger this function
** multiple times. Do MCPD_RELOAD only once in the end of OMCI MIB reset.
*/
#define rutGpon_reloadMcpd(void) \
{ \
    if (rutGpon_getOmciMibResetState() != TRUE) \
    { \
        _owapi_rut_reloadMcastCtrl(); \
    } \
}


//=======================  Public GPON functions ========================

/** add Object with the given managed entity ID.
 *  This object is automatically created while
 *  creating other object
 *
 * @param oid                (IN) object class ID.
 * @param managedEntityId    (IN) managed entity ID.
 * @param persistent         (IN) TRUE if object is persistent.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_addAutoObject
    (UINT32 oid,
     UINT32 managedEntityId,
     UBOOL8 persistent);

/** set managed entity ID of Object that matches the
 *  old managed entity ID to the new one
 *  This object is automatically created while
 *  creating other object
 *
 * @param oid      (IN) object class ID.
 * @param oldId    (IN) old managed entity ID.
 * @param newId    (IN) new managed entity ID.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_setAutoObject(UINT32 oid, UINT32 oldId, UINT32 newId);

/** delete Object that has managed entity ID
 *  matched with the given managed entity ID.
 *  This object is automatically created while
 *  creating other object
 *
 * @param oid                (IN) object class ID.
 * @param managedEntityId    (IN) managed entity ID.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_deleteAutoObject(UINT32 oid, UINT32 managedEntityId);

/** create MacBridgeConfigData Object with
 *  information given by MacBridgeServiceProfile Object
 *  This object is automatically created while
 *  creating MacBridgeServiceProfile object
 *
 * @param service      (IN) MacBridgeServiceProfile Object.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_addAutoMacBridgeConfigDataObject
    (MacBridgeServiceProfileObject *service);

/** set MacBridgeConfigData Object with
 *  information given by MacBridgeServiceProfile Object
 *  This object is automatically changed while
 *  MacBridgeServiceProfile object is changed
 *
 * @param service      (IN) MacBridgeServiceProfile Object.
 * @param oldId        (IN) old managed entity ID.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_setAutoMacBridgeConfigDataObject
    (MacBridgeServiceProfileObject *service, UINT32 oldId);

/** add default Extended VLAN tagging rules to
 *  ReceivedFrameVlanTaggingOperationTable.
 *  This function is called while creating
 *  ExtendedVlanTaggingOperationConfigurationDataObject
 *
 * @param pIidStackParent      (IN) pointer to instance ID of
 *        ExtendedVlanTaggingOperationConfigurationDataObject
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_addDefaultReceivedFrameVlanTaggingOperationTableObject
    (const InstanceIdStack *pIidStackParent);

/** get file descriptor of OMCI debug file
 *

 * @return file descriptor of OMCI debug file if existed
 *         otherwise return NULL.
 */
FILE *rutGpon_getOmciDebugFile(void);

/** get debug on/off for specific module that
 *  is used by OMCI daemon
 *
 * @param module      (IN) module ID.

 * @return debug information of the given module ID.
 */
UINT32 rutGpon_getOmciDebugModule(OmciDebugModule module);

/** get promisc on/off for all modules that
 *  are used by OMCI daemon
 *
 * @return promise mode information.
 */
UINT32 rutGpon_getOmciPromiscInfo(void);

/** set promisc on/off module that
 *  are used by OMCI daemon
 * @param promiseMode    (IN) promise Mode.
 *
 * @return none.
 */
void rutGpon_setOmciPromiscInfo(UINT32 promiscMode);

/** get debug on/off for all modules that
 *  are used by OMCI daemon
 *
 * @return debug information.
 */
UINT32 rutGpon_getOmciDebugInfo(void);

/** set debug on/off for each module that
 *  are used by OMCI daemon
 * @param debug    (IN) debug information.
 *
 * @return none.
 */
void rutGpon_setOmciDebugInfo(UINT32 info);

void rutGpon_setOmciDebugInfoLocal(UINT32 info);

/** close debug file and set its
 *  file descriptor to NULL
 *
 * @return none.
 */
void rutGpon_closeDebugFile(void);

/** open debug file and save its file descriptor
 *
 * @return cmsret.
 */
CmsRet rutGpon_openDebugFile(void);

/** start DHCP Client if it's required by IpHostConfigDatObject
 * @param oid    (IN) MDMOID_IP_HOST_CONFIG_DATA or MDMOID_IPV6_HOST_CONFIG_DATA.
 * @param meId    (IN) the MeID of IpHostConfigDatObject or Ipv6HostConfigDatObject
 * @param ifName  (IN) vlanInterface of IpHostConfigDatObject or Ipv6HostConfigDatObject
 *
 * @return cmsret.
 */
CmsRet rutGpon_configIpHost(UINT32 oid, UINT32 meId, char *ifName);

/** Add/Remove iptables mangle rule with the given TcpUdp Config Data
 * @param meId    (IN) the MeID of IpHostConfigDatObject
 * @param add        (IN) flag to add or remove.
 *
 * @return none.
 */
void rutGpon_configTcpUdpByIpHost(UINT32 meId, UBOOL8 add);

/** retrieve index of the given interface.
 *
 * @param interfaceName    (IN)  the interface name.
 *
 * @return interface index.
 */
SINT32 rutGpon_getInterfaceIndex(const char *interfaceName);

/** check the given interface is existed or not
 *
 * @param interfaceName    (IN)  the interface name.
 *
 * @return TRUE or FALSE.
 */
UBOOL8 rutGpon_isInterfaceExisted(const char *interfaceName);

/** add interface for the given bridge
 *
 * @param ifcName    (IN) the bridge interface name.
 *
 * @return cmsret.
 */
CmsRet rutGpon_addBridgeInterface
    (const char   *ifcName);

/** delete the given bridge interface
 *
 * @param ifcName    (IN) the bridge interface name.
 *
 * @return cmsret.
 */
CmsRet rutGpon_deleteBridgeInterface
    (const char   *ifcName);

/** store all bridge interface names to ifNames with
 * delimeter is space (ex: "br0 br1 br2")
 *
 * @param size    (IN)  size of ifNames.
 * @param ifNames (OUT) list of bridge interface names
 *
 * @return cmsret.
 */
CmsRet rutGpon_getAllBridgeInterfaces
    (const UINT32 size, char *ifNames);

/** add the given interface to the given bridge
 *
 * @param ifcName    (IN) the interface name.
 * @param toBridge   (IN) the bridge name.
 *
 * @return cmsret.
 */
CmsRet rutGpon_addInterfaceToBridge
    (const char   *ifcName,
     const char   *toBridge);

/** set the interface up
 *
 * @param ifName    (IN) interface name.
 *
 * @return cmsret.
 */
CmsRet rutGpon_ifUp(const char *ifName);

/** set the interface down
 *
 * @param ifName    (IN) interface name
 *
 * @return cmsret.
 */
CmsRet rutGpon_ifDown(const char *ifName);

/** disable GEM port, de-configure GEM port,
 * set interface down, remove GEM port interface
 *
 * @param ctp    (IN) GemPortNetworkCtpObject
 *
 * @return cmsret.
 */
CmsRet rutGpon_deConfigGemPort(const GemPortNetworkCtpObject *ctp);

/** configure GEM port, enable GEM port
 * create GEM port interface, set interface up
 *
 * @param newCtp    (IN) new GemPortNetworkCtpObject
 * @param currCtp    (IN) current GemPortNetworkCtpObject
 *
 * @return cmsret.
 */
CmsRet rutGpon_configGemPort
    (const GemPortNetworkCtpObject *newCtp,
     const GemPortNetworkCtpObject *currCtp);

/** configure upstream QoS for all GEM ports that
 * has its TContPointer match with the given tcontMeId
 *
 * @param tcontMeId  (IN) MeId of TContObject
 *
 * @return cmsret.
 */
CmsRet rutGpon_configAllGemPortQosUs
    (const UINT16 tcontMeId);

/** configure GEM port alloc ID to match with the
 * given alloc ID
 *
 * @param tcontMeId    (IN) MeId of TCont where GEM Port TCont Pointer points to
 * @param allocId    (IN) value of alloc ID
 *
 * @return cmsret.
 */
CmsRet rutGpon_configGemPortAllocId
    (const UINT32 tcontMeId, const UINT32 allocId);

/** configure MIPS rate limit:
 * Maximum number of packets per second that MIPS is allowed
 * so that MIPS can handle other tasks from CLI, WEB UI, etc...
 *
 * @return none.
 */
void rutGpon_configMipsRateLimit(void);

//=======================  Private GPON functions ========================

UBOOL8 rutGpon_isObjectExisted(UINT32 oid, UINT32 managedEntityId);

/** Block the UNI Traffic
 *
 * @param port    (IN) switch port number.
 *
 * @return cmsret.
 */
CmsRet rutGpon_disablePptpEthernetUni(UINT32 port);

/** unBlock the UNI Traffic
 *
 * @param port    (IN) switch port number.
 *
 * @return cmsret.
 */
CmsRet rutGpon_enablePptpEthernetUni(UINT32 port);

/** Block all the UNI Traffic
 *
 */
void rutGpon_disableAllPptpEthernetUni(void);

/** unBlock all the UNI Traffic
 *
 */
void rutGpon_enableAllPptpEthernetUni(void);

/** Block all the GemPortNetworkCtp Traffic
 *
 */
void rutGpon_disableAllGemPortNetworkCtp(void);

/** unBlock all the GemPortNetworkCtp Traffic
 *
 */
void rutGpon_enableAllGemPortNetworkCtp(void);

/** Block the UniG associated PptpUni Traffic
 *
 * @param port    (IN) UniG ME Id.
 *
 */
void rutGpon_disableUniGAssociatedUni(UINT32 uniGMeId);

/** unBlock the UniG associated PptpUni Traffic
 *
 * @param port    (IN) UniG ME Id.
 *
 */
void rutGpon_enableUniGAssociatedUni(UINT32 uniGMeId);

/** get the OntG administrativeState
 *
 */
int rutGpon_getOntGAdminState(void);

/** get the Pptp or Gem related administrativeState
 *
 * @param port    (IN) Mdm ObjectId.
 * @param port    (IN) ME Id.
 *
 */
int rutGpon_getRelatedAdminState(MdmObjectId oid, UINT32 meId);

/** get the Pptp or Gem related administrativeState with acquire lock
 *
 * @param port    (IN) Mdm ObjectId.
 * @param port    (IN) ME Id.
 *
 */
UBOOL8 rutGpon_getRelatedAdminStateWithLock(MdmObjectId oid, UINT32 meId);

/** configure pptpEth Downstream QoS Shaping
 *
 * @param macBrPortCfg    (IN) MacBridgePortConfigDataObject
 * @param reset           (IN) UBOOL8
 *
 * @return cmsret.
 */
CmsRet rutGpon_configPptpEthShapingDs(const MacBridgePortConfigDataObject *macBrPortCfg, UBOOL8 reset);

/** configure pptpEth Upstream QoS Shaping
 *
 * @param macBrPortCfg    (IN) MacBridgePortConfigDataObject
 * @param reset           (IN) UBOOL8
 *
 * @return cmsret.
 */
CmsRet rutGpon_configPptpEthShapingUs(const MacBridgePortConfigDataObject *macBrPortCfg, UBOOL8 reset);

/** configure bridge learning indication
 *
 * @param macBrPortCfg    (IN) MacBridgePortConfigDataObject
 *
 * @return cmsret.
 */
CmsRet rutGpon_updateBridgeLearningInd(const _MacBridgeServiceProfileObject *bridgeObj);

/** configure Port Learning Indication
 *
 * @param macBrPortCfg    (IN) MacBridgePortConfigDataObject
 * @param reset           (IN) UBOOL8
 *
 * @return cmsret.
 */
CmsRet rutGpon_configPortLearningInd(const MacBridgePortConfigDataObject *macBrPortCfg, UBOOL8 reset);

/** configure TrafficDescriptor
 *
 * @param trafficDescCfg    (IN) GemTrafficDescriptorObject
 * @param reset              (IN) UBOOL8
 *
 * @return cmsret.
 */
CmsRet rutGpon_trafficDescConfig(GemTrafficDescriptorObject * trafficDescCfg, UBOOL8 reset);

/** check the given ME is Veip pptp UNI
 *
 * @param oid    (IN)  MDM object id
 * @param meId    (IN)  ME id
 *
 * @return TRUE or FALSE.
 */
UBOOL8 rutGpon_isVeipPptpUni(UINT32 oid, UINT32 meId);

#if defined(DMP_X_ITU_ORG_VOICE_SIP_1) || defined(OMCI_TR69_DUAL_STACK)
/** get Large String the given ME
 *
 * @param largeStrObjID    (IN)  MDM object id
 *
 * @return pointer of large string.
 */
char* rutGpon_getLargeString(UINT32 largeStrObjID);
#endif // defined(DMP_X_ITU_ORG_VOICE_SIP_1) || defined(OMCI_TR69_DUAL_STACK)

/** get Ethernet line speed and duplex state
 *
 * @param portIndex  (IN)   Ethernet port index
 * @param portInfo   (OUT)  Ethernet line speed | duplex state
 *
 * @return cmsret.
 */
CmsRet rutGpon_getEnetInfo
    (UINT32 portIndex,
     UINT32 *pPortInfo);

void rutGpon_setOmciMibResetState(UBOOL8 state);
UBOOL8 rutGpon_getOmciMibResetState(void);

/** configure drop precedence
 *
 * @param dir (IN)  direction US/DS
 * @param dpcmKey (IN) drop precedence color marking key - DEI/PCP6P2D
 *
 * @return cmsret.
 */
CmsRet rutGpon_configDropProcedence(const UBOOL8 isDirDs, const UINT8 dpcmKey);


/** get SR Indication
 *
  * @param pSRIndication  (OUT)  SR Indication
 *
 * @return cmsret.
 */
CmsRet rutgpon_getSRIndication(UBOOL8 *pSRIndication);

CmsRet rutGpon_getEthPortType(UINT32 port, OmciEthPortType *type);

CmsRet rutGpon_getEthPortLinkedOmciMe(UINT32 port, UINT16 *uniOid, UINT16 *uniMeId);

CmsRet rutGpon_getVirtIntfMeId(UINT16 *uniOid, UINT16 *meId);

CmsRet get_obj_by_instance(MdmObjectId oid, void **pObj, UINT32 meInst,
  UINT32 present);

void rutGpon_pptpEthUniTmInit(void);

UINT32 getOnuOmciTmOption(void);
UINT32 getOnuOmciDsInvalidQueueAction(void);

UBOOL8 rutGpon_isAllocIdInitValue(UINT16 allocId);
OmciPonAppType rutGpon_getOmciAppType(void);
UBOOL8 rutGpon_getUniDataPathMode(void);
UBOOL8 rutGpon_getExtVlanDefaultRuleMode(void);

CmsRet rutGpon_reconfigMcastGemPort(GemPortNetworkCtpObject *ctp);

void rutGpon_macBpCfgDataSetUniConfigState(UINT8 type, UINT32 mePointer);

CmsRet rutGpon_getUniPhyInfo(UINT32 uniOid, UINT32 uniMeId,
  OmciPhyInfo_t *phyInfoP);


#endif /* __RUT_GPON_H__ */
