/***********************************************************************
 *
 *  Copyright (c) 2006-2008 Broadcom
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
#ifndef __RUT_GPON_MCAST_H__
#define __RUT_GPON_MCAST_H__


/*!\file rut_gpon_mcast.h
 * \brief System level interface functions for generic GPON functionality.
 *
 * The functions in this file should only be called by
 * RCL, STL, and other RUT functions.
 */


#include "omci_object.h"

/* Version definition according to bcm_mcast_api.h. */
#define IGMP_VERSION_1 0x12
#define IGMP_VERSION_2 0x16
#define IGMP_VERSION_3 0x22

#define UNSPECIFIED_SOURCE_IP 0x0

// Multicast Access Control definitions
// are defined to match with
// ITU-T G.988 in section
// 9.3.27 Multicast Operations Profile

#define OMCI_MCAST_AC_SIZE                 24
#define OMCI_MCAST_AC_INDEX_START           0
#define OMCI_MCAST_AC_GEM_PORT_ID_START     2
#define OMCI_MCAST_AC_VLAN_START            4
#define OMCI_MCAST_AC_SRC_ADDR_START        6
#define OMCI_MCAST_AC_DST_START_ADDR_START 10
#define OMCI_MCAST_AC_DST_END_ADDR_START   14
#define OMCI_MCAST_AC_BANDWIDTH_START      18
#define OMCI_MCAST_AC_RESERVE_START        22

// Multicast Access Control Field definition
typedef enum
{
    OMCI_MCAST_AC_INDEX = 0,
    OMCI_MCAST_AC_GEM_PORT_ID,
    OMCI_MCAST_AC_VLAN_ID,
    OMCI_MCAST_AC_SRC_ADDR,
    OMCI_MCAST_AC_DST_START_ADDR,
    OMCI_MCAST_AC_DST_END_ADDR,
    OMCI_MCAST_AC_BANDWIDTH
} OmciMcastAccessControlField;

// Multicast Upstream IGMP Tag Control Type
typedef enum
{
    OMCI_MCAST_US_TAG_NONE = 0,
    OMCI_MCAST_US_TAG_ADD,
    OMCI_MCAST_US_TAG_REPLACE_TCI,
    OMCI_MCAST_US_TAG_REPLACE_VLAN
} OmciMcastUpstreamTagControl;

// Multicast Downstream IGMP and Data Control Type
typedef enum
{
    OMCI_MCAST_DS_TAG_NONE = 0,
    OMCI_MCAST_DS_TAG_STRIP,
    OMCI_MCAST_DS_TAG_ADD,
    OMCI_MCAST_DS_TAG_REPLACE_TCI,
    OMCI_MCAST_DS_TAG_REPLACE_VLAN,
    OMCI_MCAST_DS_TAG_ADD_FROM_SUBS,
    OMCI_MCAST_DS_TAG_REPLACE_TCI_FROM_SUBS,
    OMCI_MCAST_DS_TAG_REPLACE_VLAN_FROM_SUBS
} OmciMcastDownstreamTagControl;

typedef struct OmciMcastSubs
{
    UINT16  oid;
    UINT16  meId;
    UINT32  bandwidth;
    UINT32  joinMessagesCounter;
    UINT32  bandwidthExceededCounter;
    UINT32  numberOfSubscribers;
} OmciMcastSubs_t;

typedef struct OmciMcastSubsInfo
{
    UINT32           numberOfEntries;
    OmciMcastSubs_t  *currSubsTbl;
} OmciMcastSubsInfo_t;


//=======================  Public GPON MCAST functions ========================

/** return CMSRET_OBJECT_NOT_FOUND if group address is not allowed
 * return CMSRET_RESOURCE_EXCEEDED
 *     if bandwidth usage is over maxMulticastBandwidth
 *     or number of subscribers is over maxSimultaneousGroups
 * return CMSRET_INVALID_ARGUMENTS if physical type is not
 *     Ethernet nor MoCA
 * otherwise return CMSRET_SUCCESS
 *
 * @param msg  (IN) pointer to OmciIgmpMsgBody structure.
 *
 * @return CmsRet enum.
 */
CmsRet rutGpon_requestIgmpMessage
    (const OmciIgmpMsgBody *msg);

/** return join messsage counter if uni is found
 * otherwise return 0
 *
 * @param uniOId  (IN) Object ID of PptpXXXUniObject.
 * @param uniMeId (IN) Management entity ID of PptpXXXUniObject.
 *
 * @return join message counter.
 */
UINT32 rutGpon_getJoinMessageCounter
    (const UINT16 uniOid,
     const UINT16 uniMeId);

/** return bandwidth exceeded counter if uni is found
 * otherwise return 0
 *
 * @param uniOId  (IN) Object ID of PptpXXXUniObject.
 * @param uniMeId (IN) Management entity ID of PptpXXXUniObject.
 *
 * @return bandwidth exceeded counter.
 */
UINT32 rutGpon_getBandwidthExceededCounter
    (const UINT16 uniOid,
     const UINT16 uniMeId);

/** return current multicast bandwidth if uni is found
 * otherwise return 0
 *
 * @param uniOId  (IN) Object ID of PptpXXXUniObject.
 * @param uniMeId (IN) Management entity ID of PptpXXXUniObject.
 *
 * @return current multicast bandwidth.
 */
UINT32 rutGpon_getCurrentMcastBandwidth
    (const UINT16 uniOid,
     const UINT16 uniMeId);

/** return imputed bandwidth if uni and grpAddr is found
 * otherwise return 0
 *
 * @param uniOId  (IN) Object ID of PptpXXXUniObject.
 * @param uniMeId (IN) Management entity ID of PptpXXXUniObject.
 * @param grpAddr (IN) destination multicast group address.
 *
 * @return current multicast bandwidth.
 */
UINT32 rutGpon_getImputedBandwidth
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const UINT32 grpAddr,
     const UINT32 srcAddr,
     const UINT32 igmpVersion);

/** update active group list table under
 *  MulticastSubscriberMonitorObject
 *
 * @param uniOId  (IN) Object ID of PptpXXXUniObject.
 * @param uniMeId (IN) Management entity ID of PptpXXXUniObject.
 * @param pMcastSubsMonIid (IN) pointer to instance id stack of
 *                             MulticastSubscriberMonitorObject.
 *
 * @return CmsRet.
 */
CmsRet rutGpon_updateActiveGroupList
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const InstanceIdStack *pMcastSubsMonIid);

/** get IGMP rate limit from the given Bridge MeId
 *
 * @param bridgeId (IN) Management entity ID of MacBridgeServiceProfileObject.
 * @param pRate (OUT) IGMP rate limit
 *
 * @return CmsRet.
 */
CmsRet rutGpon_getIgmpRateLimitFromBridgeId
    (const UINT32 bridgeId,
     UINT32 *pRate);

/** get upstream IGMP TCI from the given UNI OID and MeId
 *
 * @param uniOid (IN) OID of UNI
 * @param uniMeId (IN) MeID of UNI
 * @param pType (OUT) upstream IGMP tag control type
 * @param pTci (OUT) upstream IGMP TCI
 *
 * @return CmsRet.
 */
CmsRet rutGpon_getUsMcastTciFromUniMeId
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     UINT8 *pType,
     UINT16 *pTci);

/** get downstream IGMP and multicast TCI from the given Bridge MeId
 *
 * @param bridgeMeId (IN) MeID of Bridge
 * @param pType (OUT) downstream IGMP and muticast control type
 * @param pTci (OUT) downstream multicast TCI
 *
 * @return CmsRet.
 */
CmsRet rutGpon_getDsMcastTciFromBridgeMeId
    (const UINT32 bridgeMeId,
     UINT8 *pType,
     UINT16 *pTci);

//=======================  Private GPON MCAST functions ========================

UINT32 getAccessControlField
    (const UINT8 *accessControl,
     const OmciMcastAccessControlField field);

CmsRet updateJoinMessageCounter
    (const UINT16 uniOid, const UINT16 uniMeId);

CmsRet updateBandwidthExceededCounter
    (const UINT16 uniOid, const UINT16 uniMeId);

UINT32 getCurrentImputedBandwidthUsage
    (const UINT16 uniOid,
     const UINT16 uniMeId);

UINT32 getNumberOfCurrentSubscribers
    (const UINT16 uniOid,
     const UINT16 uniMeId);

CmsRet getImputedBandwidth
    (const UINT8 *accessControl,
     const UINT32 grpAddr,
     const UINT32 srcAddr,
     const UINT32 igmpVersion,
     UINT32 *pBandwidth);

CmsRet updateCurrentImputedBandwidthUsage
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const UINT32 bandwidth,
     const OmciIgmpMsgType msgType);

CmsRet isSubscriberExisted
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const OmciIgmpMsgBody *msg);

CmsRet updateCurrentSubscribers
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const OmciIgmpMsgBody *msg);

CmsRet handleJoinMessage
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const OmciIgmpMsgBody *msg);

CmsRet handleLeaveMessage
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const OmciIgmpMsgBody *msg);

/** return CMSRET_OBJECT_NOT_FOUND if group address is not allowed
 * return CMSRET_RESOURCE_EXCEEDED
 *     if bandwidth usage is over maxMulticastBandwidth
 *     or number of subscribers is over maxSimultaneousGroups
 * otherwise return CMSRET_SUCCESS
 *
 * @param uniOId  (IN) Object ID of PptpXXXUniObject.
 * @param uniMeId (IN) Management entity ID of PptpXXXUniObject.
 * @param msg     (IN) IGMP message that is received from MCPD
 *
 * @return CmsRet enum.
 */
CmsRet handleIgmpMessage
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     const OmciIgmpMsgBody *msg);

CmsRet getMulticastSubscriberConfigInfoFromUniMeId
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     MulticastSubscriberConfigInfoObject *pInfo);

CmsRet rutGpon_getIgmpRateLimitOnRgBridge(UINT32 *pRate);

CmsRet rutGpon_deleteDynamicAccessControl
	(const DynamicAccessControlListTableObject * currObj,
	const InstanceIdStack *iidStack);

CmsRet rutGpon_updateDynamicAccessControl
	(const DynamicAccessControlListTableObject * currObj,
	DynamicAccessControlListTableObject *newObj,
	const InstanceIdStack *iidStack);

CmsRet rutGpon_getMcastAclAniVidListFromUniMeId(UINT32 uniOid, UINT32 uniMeId, 
  UBOOL8 findAllB, UINT32 *vidListNum, UINT16 *vidListP);


#endif /* __RUT_GPON_MCAST_H__ */
