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


#define VEIP_PHY_ID               0
#define VEIP_DEV_ID               0

#define VEIP_PHY_INTERFACE        "gpon0"
#define VEIP_DEV_INTERFACE        "gpon0.0"


#define CHECK_INTF_PTR(intf) (intf != NULL) ? intf : "(null)"

#define isNewTpid(tpid) ((tpid != 0) && (tpid != C_TAG_TPID) && \
  (tpid != S_TAG_TPID) && (tpid != D_TAG_TPID))

#define skipSetQueue(numTag, pbitQId, isQFound) \
  ((numTag == 0) && (pbitQId == OMCI_QID_NOT_EXIST) && (isQFound == FALSE))

#define getGemPortType(b, m) (broadcast ? OMCI_SERVICE_BROADCAST : \
  (multicast ? OMCI_SERVICE_MULTICAST : OMCI_SERVICE_UNICAST))


/* Enable the following flag if the ONU/RG uses OMCI (instead of RG) for
 * the GEM-queue mapping.
 */
/* #define FORCE_OMCI_QUEUE_MAPPING 1 */

static void getUpstreamDirectionFromUniOid(const UINT16 uniOid,
  const UINT16 uniMeId, vlanCtl_direction_t *pDir, char *vlanInterface);
static void getDownstreamDirectionFromUniOid(const UINT16 uniOid,
  const UINT16 uniMeId, vlanCtl_direction_t *pDir, char *vlanInterface);


//=======================  Private GPON functions ========================

int rutwrap_vlanCtl_cmdSetSkbMarkQueue(unsigned int queue)
{
    int rc = 0;

#if !defined(FORCE_OMCI_QUEUE_MAPPING)
    if (_owapi_rut_tmctl_isUserTMCtlOwner(TMCTL_USER_OMCI) == TRUE)
#endif
    {
        rc = vlanCtl_cmdSetSkbMarkQueue(queue);
    }

    return rc;
}

/*
 * When the "priority queue pointer for downstream" attribute in a
 * GemPortNetworkCtp ME instance is unknown, there are several options for
 * the possible action:
 *  0. Skip vlanCtl_cmdSetSkbMarkQueue().
 *  1. Set queue ID = packet pbit value. The queue setting is done in the VLAN
       driver, and suitable for ONU/RG mode.
 *  2. Set queue ID = packet pbit value. The queue setting is done in OMCI by
 *     expanding VLANCtl rules with filter-pbits fields. It is suitable for both
 *     SFU and ONU/RG. Note the number of rules may be multiplied by 8 (possible
 *     pbits values) in some cases.
 *  2 is the default setting.
 *
 */
int rutwrap_setDsSkbMarkQueueByPbits(void)
{
    int rc = 0;

#if !defined(FORCE_OMCI_QUEUE_MAPPING)
    if (_owapi_rut_tmctl_isUserTMCtlOwner(TMCTL_USER_OMCI) == TRUE)
#endif
    {
        rc = vlanCtl_cmdSetSkbMarkQueueByPbits();
    }

    return rc;
}

UBOOL8 rutGpon_getDsQueueIdRange(const OmciPhyInfo_t *phyInfoP,
  UINT16 *pbitQIdStart, UINT16 *pbitQIdEnd)
{
    UINT16 qid;
    UINT32 setDsQOption;
    UBOOL8 isQFound = FALSE;

    if (phyInfoP->phyType != OMCI_PHY_GPON)
    {
        *pbitQIdStart = OMCI_QID_NOT_EXIST;
        *pbitQIdEnd = OMCI_QID_NOT_EXIST;
        return isQFound;
    }

    setDsQOption = getOnuOmciDsInvalidQueueAction();

    if (rutGpon_getQidFromGemPortIndex(phyInfoP->phyId, OMCI_FLOW_DOWNSTREAM, &qid)
      == CMSRET_SUCCESS)
    {
        *pbitQIdStart = qid;
        *pbitQIdEnd = qid;
        isQFound = TRUE;
    }
    else
    {
        if ((setDsQOption == OMCI_DS_INVALID_QUEUE_ACTION_PBIT_EXT) &&
          (phyInfoP->gemPortType != OMCI_SERVICE_MULTICAST))
        {
            *pbitQIdStart = OMCI_FILTER_PRIO_MIN;
            *pbitQIdEnd = OMCI_FILTER_PRIO_MAX;
        }
        else
        {
            *pbitQIdStart = OMCI_QID_NOT_EXIST;
            *pbitQIdEnd = OMCI_QID_NOT_EXIST;
        }
    }

    return isQFound;
}

void rutwrap_setDsSkbMarkQueue(UINT16 pbitQId, UBOOL8 markOnly)
{
    UINT32 setDsQOption;
    UBOOL8 applied = FALSE;

    setDsQOption = getOnuOmciDsInvalidQueueAction();

    if (markOnly == TRUE)
    {
        if (pbitQId != OMCI_QID_NOT_EXIST)
        {
            rutwrap_vlanCtl_cmdSetSkbMarkQueue(pbitQId);
            applied = TRUE;
        }
    }
    else
    {
        if (setDsQOption == OMCI_DS_INVALID_QUEUE_ACTION_PBIT_EXT)
        {
            if (pbitQId != OMCI_QID_NOT_EXIST)
            {
                vlanCtl_filterOnTagPbits(pbitQId, 0);
                rutwrap_vlanCtl_cmdSetSkbMarkQueue(pbitQId);
                applied = TRUE;
            }
        }
        else if (setDsQOption == OMCI_DS_INVALID_QUEUE_ACTION_PBIT)
        {
            rutwrap_setDsSkbMarkQueueByPbits();
            applied = TRUE;
        }
    }

    if (applied != TRUE)
    {
        cmsLog_notice("Set queue not applied, dsoption=%d, pbitQId=%d, markOnly=%d",
          setDsQOption, pbitQId, markOnly);
    }
}

static void setIgmpRateLimitOnBridge(UINT32 omciBridgeMeId, char *bridgeName,
  const char *vlanInterface, UBOOL8 delFlag)
{
    UINT32 rate = 0;
    CmsRet ret = CMSRET_SUCCESS;
#if defined(DMP_X_BROADCOM_COM_MCAST_1)
    int ifi;
#endif

    // only configure IGMP rate limit when LAN/VEIP interface is added to bridge
    if ((strstr(vlanInterface, "eth") != NULL) || (strstr(vlanInterface, GPON_IFC_STR) != NULL))
    {
        if (delFlag == TRUE)
        {
            rate = 0;
        }
        else
        {
            ret = rutGpon_getIgmpRateLimitFromBridgeId(omciBridgeMeId, &rate);
            if (ret != CMSRET_SUCCESS)
            {
                rate = 0;
            }
        }
#if defined(DMP_X_BROADCOM_COM_MCAST_1)
        ifi = cmsNet_getIfindexByIfname(bridgeName);
        bcm_mcast_api_set_proto_rate_limit(-1, ifi, BCM_MCAST_PROTO_IPV4, rate);
#endif
    }
}

static CmsRet deleteMapperBridge
    (const UINT32 mapperMeId)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BCM_MapperServiceProfileObject *bcmMapper = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search BCM_MapperServiceProfileObject
    while ((!found) &&
           (_cmsObj_getNext(MDMOID_BC_MAPPER_SERVICE_PROFILE,
                           &iidStack,
                           (void **) &bcmMapper) == CMSRET_SUCCESS))
    {
        found = (bcmMapper->managedEntityId == mapperMeId);
        // if BCM_MapperServiceProfileObject is found then
        // get bridge name of mapper
        if (found == TRUE && bcmMapper->bridgeName != NULL)
        {
            rutGpon_deleteBridgeInterface(bcmMapper->bridgeName);
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &bcmMapper);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> deleteMapperBridge, mapperMeId=%d, ret=%d\n",
                   mapperMeId, ret);

    return ret;
}

static CmsRet deleteServiceBridge
    (const UINT32 serviceMeId)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BCM_MacBridgeServiceProfileObject *bcmBridge = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search BCM_MacBridgeServiceProfileObject
    while ((!found) &&
           (_cmsObj_getNext(MDMOID_BC_MAC_BRIDGE_SERVICE_PROFILE,
                           &iidStack,
                           (void **) &bcmBridge) == CMSRET_SUCCESS))
    {
        found = (bcmBridge->managedEntityId == serviceMeId);
        // if BCM_MacBridgeServiceProfileObject is found then
        // get bridge name of mapper
        if (found == TRUE && bcmBridge->bridgeName != NULL)
        {
            rutGpon_deleteBridgeInterface(bcmBridge->bridgeName);
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &bcmBridge);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> deleteServiceBridge, serviceMeId=%d, ret=%d\n",
                   serviceMeId, ret);

    return ret;
}

static CmsRet getIpHostInterfaces
    (const UINT32                  ipHostOid,
     const UINT32                  ipHostMeId,
     const OmciMapFilterModelType  type,
     UINT32                        *devId,
     UINT32                        *bridgeMeId,
     char                          *devInterface,
     char                          *vlanInterface)
{
    CmsRet ret = CMSRET_INTERNAL_ERROR;

    // In Mapper model, IpHostConfigDataObject does not connect
    // to MacBridgePortConfigDataObject so assign devId to 0
    if (type != OMCI_MF_MODEL_1_MAP && type != OMCI_MF_MODEL_N_MAP)
    {
        if ((ret = rutGpon_getBridgePortMeIdFromUniMeId(ipHostOid, ipHostMeId,
                                                        devId)) != CMSRET_SUCCESS)
            goto out;
    }
    else
        *devId = 0;

    if ((ret = rutGpon_getBridgeMeIdFromUniMeId(ipHostOid, ipHostMeId,
                                                bridgeMeId)) != CMSRET_SUCCESS)
        goto out;

    // for iphost, bridge name is its device interface name
    if ((ret = rutGpon_getBridgeNameFromBridgeMeId(*bridgeMeId, devInterface)) != CMSRET_SUCCESS)
        goto out;

    if (rutGpon_isInterfaceExisted(devInterface) == TRUE)
    {
        // generate iphost virtual interface name
        snprintf(vlanInterface, CMS_IFNAME_LENGTH, "%s.%d", devInterface, *devId);
        ret = CMSRET_SUCCESS;
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> getIpHostInterfaces, ipHostOid=%d, ipHostMeId=%d, devId=%d, bridgeMeId=%d, devIfc=%s, vlanIfc=%s, ret=%d\n",
                   ipHostOid, ipHostMeId, *devId, *bridgeMeId, devInterface, vlanInterface, ret);

    return ret;
}

static CmsRet getVeipVlanName(char *ifName)
{
    CmsRet ret = CMSRET_SUCCESS;

    if (ifName == NULL)
        return CMSRET_INVALID_ARGUMENTS;

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
    ret = rutVeip_getVeipVlanName(ifName);
#else
    cmsUtl_strncpy(ifName, "veip0", CMS_IFNAME_LENGTH);
#endif /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

    return ret;
}

/*****************************************************************************
*  FUNCTION:  buildOmciModelUniInfo
*  PURPOSE:   Build UNI information structure from the OMCI model. The
*             information may be saved for better performance.
*  PARAMETERS:
*      uniOid - UNI OID.
*      uniMeId - UNI ME id.
*      isVeip - whether the UNI is VEIP.
*      uniInfoP - pointer to the derived UNI-side information.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet buildOmciModelUniInfo(const UINT32 uniOid,
  const UINT32 uniMeId, UBOOL8 isVeip, OmciUniInfo_t *uniInfoP)
{
    OmciPhyInfo_t phyInfo;
    UINT32 bridgeMeId = 0;
    char *devIfName = NULL, *vlanIfName = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    memset(uniInfoP, 0x0, sizeof(OmciUniInfo_t));

    ret = rutGpon_getBridgeMeIdFromUniMeId(uniOid, uniMeId, &bridgeMeId);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("rutGpon_getBridgeMeIdFromUniMeId() failed, UNI (%d:%d)",
          uniOid, uniMeId);
        goto out;
    }

    if (isVeip != TRUE)
    {
        ret = rutGpon_getUniPhyInfo(uniOid, uniMeId, &phyInfo);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("rutGpon_getUniPhyInfo() failed, UNI (%d:%d)",
              uniOid, uniMeId);
            goto out;
        }
        ret = rutGpon_getInterfaceName(phyInfo.phyType, phyInfo.phyId,
          &devIfName);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("rutGpon_getInterfaceName() failed, UNI (%d:%d)",
              uniOid, uniMeId);
            goto out;
        }

        ret = rutGpon_getVirtualInterfaceName(phyInfo.phyType, phyInfo.phyId,
          bridgeMeId, &vlanIfName);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("rutGpon_getVirtualInterfaceName() failed, "
              "UNI (%d:%d)",
              uniOid, uniMeId);
            goto out;
        }

        uniInfoP->phyType = phyInfo.phyType;
        uniInfoP->phyId = phyInfo.phyId;
        cmsUtl_strncpy(uniInfoP->devInterface, devIfName,
          CMS_IFNAME_LENGTH);
        cmsUtl_strncpy(uniInfoP->vlanInterface, vlanIfName,
          CMS_IFNAME_LENGTH);
out:
        CMSMEM_FREE_BUF_AND_NULL_PTR(devIfName);
        CMSMEM_FREE_BUF_AND_NULL_PTR(vlanIfName);
    }
    else
    {
        char veipVlanName[CMS_IFNAME_LENGTH] = {0};

        getVeipVlanName(veipVlanName);
        uniInfoP->phyType = OMCI_PHY_ETHERNET;
        uniInfoP->phyId = VEIP_DEV_ID;
        cmsUtl_strncpy(uniInfoP->devInterface, VEIP_DEV_INTERFACE,
          CMS_IFNAME_LENGTH);
        cmsUtl_strncpy(uniInfoP->vlanInterface, veipVlanName,
          CMS_IFNAME_LENGTH);
    }

    uniInfoP->uniOid = uniOid;
    uniInfoP->uniMeId = uniMeId;
    uniInfoP->bridgeMeId = bridgeMeId;

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
      "===> buildOmciModelUniInfo, "
      "uniOid=%d, uniMeId=%d, bridgeMeId=%d, phyType=%d, phyId=%d, "
      "devIf=%s, vlanIf=%s, ret=%d\n",
      uniInfoP->uniOid, uniInfoP->uniMeId, uniInfoP->bridgeMeId,
      uniInfoP->phyType, uniInfoP->phyId,
      uniInfoP->devInterface, uniInfoP->vlanInterface, ret);

    return ret;
}

static CmsRet deleteUniVlanInterface
    (const UINT32                  uniOid,
     const UINT32                  uniMeId,
     const OmciMapFilterModelType  type __attribute__((unused)))
{

    OmciUniInfo_t uniInfo;
    UINT32 phyId = 0, bridgeMeId = 0;
    char   *devInterface = NULL, *vlanInterface = NULL;
    CmsRet ret = CMSRET_INTERNAL_ERROR;

    ret = buildOmciModelUniInfo(uniOid, uniMeId, FALSE, &uniInfo);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("buildOmciModelUniInfo() failed, UNI (%d:%d)",
          uniOid, uniMeId);
        goto out;
    }

    devInterface = uniInfo.devInterface;
    vlanInterface = uniInfo.vlanInterface;
    phyId = uniInfo.phyId;

    if (rutGpon_isInterfaceExisted(devInterface) == TRUE)
    {
        // delete all rules that are stored in llist for this specific devInterface
        if (omci_rule_exist(devInterface, phyId) == CMSRET_SUCCESS)
            omci_rule_delete(devInterface, phyId);
        // delete UNI virtual interface to purge all tag rules that are associated with it
        if (rutGpon_isInterfaceExisted(vlanInterface) == TRUE)
        {
            vlanCtl_init();
            rutGpon_vlanCtl_deleteVlanInterface(vlanInterface);
            vlanCtl_cleanup();
            // re-launch MCPD everytime UNI vlan interface is deleted
            rutGpon_reloadMcpd();
        }
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> deleteUniVlanInterface, uniOid=%d, uniMeId=%d, "
                   "uniPhyId=%d, bridgeMeId=%d, ret=%d\n",
                   uniOid, uniMeId, phyId, bridgeMeId, ret);

    return ret;
}

static CmsRet deleteIpHostVlanInterface
    (const UINT32                  ipHostOid,
     const UINT32                  ipHostMeId,
     const OmciMapFilterModelType  type)
{
    UINT32 devId = 0, bridgeMeId = 0;
    char   devInterface[CMS_IFNAME_LENGTH], vlanInterface[CMS_IFNAME_LENGTH];
    CmsRet ret = CMSRET_INTERNAL_ERROR;

    memset(devInterface, 0, CMS_IFNAME_LENGTH);
    memset(vlanInterface, 0, CMS_IFNAME_LENGTH);

    if ((ret = getIpHostInterfaces(ipHostOid, ipHostMeId, type,
                                   &devId, &bridgeMeId,
                                   devInterface, vlanInterface)) == CMSRET_SUCCESS)
    {
        // delete all rules that are stored in llist for this specific devInterface
        if (omci_rule_exist(devInterface, devId) == CMSRET_SUCCESS)
            omci_rule_delete(devInterface, devId);

        // delete IpHost virtual interface to purge all tag rules that are associated with it
        if (rutGpon_isInterfaceExisted(vlanInterface) == TRUE)
        {
            vlanCtl_init();
            rutGpon_vlanCtl_deleteVlanInterface(vlanInterface);
            vlanCtl_cleanup();
            // re-launch MCPD everytime IPHost vlan interface is deleted
            rutGpon_reloadMcpd();
        }
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> deleteIpHostVlanInterface, ipHostOid=%d, ipHostMeId=%d, devIfc=%s, vlanIfc=%s, ret=%d\n",
                   ipHostOid, ipHostMeId, devInterface, vlanInterface, ret);

    return ret;
}

static CmsRet deleteVeipVlanInterface(void)
{
    char veipVlanName[CMS_IFNAME_LENGTH] = {0};

    getVeipVlanName(veipVlanName);

    if (rutGpon_isInterfaceExisted(VEIP_DEV_INTERFACE) == TRUE)
    {
        // delete all rules that are stored in llist for this specific devInterface
        if (omci_rule_exist(VEIP_DEV_INTERFACE, VEIP_DEV_ID) == CMSRET_SUCCESS)
            omci_rule_delete(VEIP_DEV_INTERFACE, VEIP_DEV_ID);
    }

    // delete VEIP virtual interface to purge all tag rules that are associated with it
    if (rutGpon_isInterfaceExisted(veipVlanName) == TRUE)
    {
        vlanCtl_init();
        rutGpon_vlanCtl_deleteVlanInterface(veipVlanName);
        vlanCtl_cleanup();
        // re-launch MCPD everytime VEIP vlan interface is deleted
        rutGpon_reloadMcpd();
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> deleteVeipVlanInterface, devIfc=%s, vlanIfc=%s\n",
                   VEIP_DEV_INTERFACE, veipVlanName);

    return CMSRET_SUCCESS;
}

static CmsRet deleteAniInterface
    (const GemPortNetworkCtpObject *ctp,
     const UINT32                  uniOid,
     const UINT32                  uniMeId)
{
    UINT32 phyId = 0, bridgeMeId = 0, omciBridgeMeId = 0;
    char   *devInterface = NULL, *vlanInterface = NULL;
    CmsRet ret = CMSRET_INTERNAL_ERROR;

    if ((ret = rutGpon_getIndexFromGemPortNetworkCtp(ctp, &phyId)) != CMSRET_SUCCESS)
        goto out;

    if ((ret = rutGpon_getBridgeMeIdFromUniMeId(uniOid, uniMeId,
                                                &omciBridgeMeId)) != CMSRET_SUCCESS)
        goto out;

    // only assign bridgeMeId to real omciBridgeMeId when UNI is NOT VEIP
    // since VEIP is connected to default bridge that has bridgeMeId is 0
    if (uniOid != MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT &&
        !rutGpon_isVeipPptpUni(uniOid, uniMeId))
    {
        bridgeMeId = omciBridgeMeId;
    }

    // generate ANI device interface name
    rutGpon_getInterfaceName(OMCI_PHY_GPON, bridgeMeId, &devInterface);

    if (rutGpon_isInterfaceExisted(devInterface) == TRUE)
    {
        // delete all rules that are stored in llist for this specific devInterface
        if (omci_rule_exist(devInterface, phyId) == CMSRET_SUCCESS)
            omci_rule_delete(devInterface, phyId);

        // only delete devInterface if no GEM ports that are attached to it
        if (rutGpon_isBridgeConnectedToAnyGemPorts(ctp->managedEntityId, omciBridgeMeId) == FALSE)
        {
            vlanCtl_init();

            // generate ANI virtual interface name
            rutGpon_getVirtualInterfaceName(OMCI_PHY_GPON, bridgeMeId, bridgeMeId, &vlanInterface);

            // delete ANI virtual interface to purge all tag rules that are associated with it
            if (rutGpon_isInterfaceExisted(vlanInterface) == TRUE)
            {
                // delete vlan interface that is attached to bridge or VEIP
                rutGpon_vlanCtl_deleteVlanInterface(vlanInterface);
                // remove mcast info for MCPD if multicast gem port is deleted
                if (rutGpon_isGemPortNetworkCtpMulticast(ctp) == TRUE)
                    _owapi_rut_updateMcastCtrlIfNames("");
            }

            // free ANI virtual interface name
            CMSMEM_FREE_BUF_AND_NULL_PTR(vlanInterface);

            // make  device interface down
            rutGpon_ifDown(devInterface);
            // delete device interface
            rutGpon_vlanCtl_deleteVlanInterface(devInterface);

            vlanCtl_cleanup();
        }
    }

    // free ANI device interface name
    CMSMEM_FREE_BUF_AND_NULL_PTR(devInterface);

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> deleteAniInterface, ctpMeId=%d, phyId=%d, bridgeMeId=%d, omciBridgeMeId=%d, ret=%d\n",
                   ctp->managedEntityId, phyId, bridgeMeId, omciBridgeMeId, ret);

    return ret;
}

static UBOOL8 isUniVlanInterfaceExisted
    (const UINT32                  uniOid,
     const UINT32                  uniMeId,
     const OmciMapFilterModelType  type __attribute__((unused)))
{
    OmciUniInfo_t uniInfo;
    UBOOL8 exist = FALSE;
    CmsRet ret = CMSRET_INTERNAL_ERROR;

    ret = buildOmciModelUniInfo(uniOid, uniMeId, FALSE, &uniInfo);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("buildOmciModelUniInfo() failed, UNI (%d:%d)",
          uniOid, uniMeId);
        goto out;
    }

    if (rutGpon_isInterfaceExisted(uniInfo.devInterface) == TRUE)
    {
        exist = rutGpon_isInterfaceExisted(uniInfo.vlanInterface);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> isUniVlanInterfaceExisted, uniOid=%d, uniMeId=%d, "
                   "phyType=%d, phyId=%d, bridgeMeId=%d, ret=%d, exist=%d\n",
                   uniOid, uniMeId, uniInfo.phyType, uniInfo.phyId,
                   uniInfo.bridgeMeId, ret, exist);

    return exist;
}

static UBOOL8 isIpHostVlanInterfaceExisted
    (const UINT32                  ipHostOid,
     const UINT32                  ipHostMeId,
     const OmciMapFilterModelType  type)
{
    UBOOL8 exist = FALSE;
    UINT32 devId = 0, bridgeMeId = 0;
    char   devInterface[CMS_IFNAME_LENGTH], vlanInterface[CMS_IFNAME_LENGTH];
    CmsRet ret = CMSRET_INTERNAL_ERROR;

    memset(devInterface, 0, CMS_IFNAME_LENGTH);
    memset(vlanInterface, 0, CMS_IFNAME_LENGTH);

    if ((ret = getIpHostInterfaces(ipHostOid, ipHostMeId, type,
                                   &devId, &bridgeMeId,
                                   devInterface, vlanInterface)) == CMSRET_SUCCESS)
    {
        exist = rutGpon_isInterfaceExisted(vlanInterface);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> isIpHostVlanInterfaceExisted, ipHostOid=%d, ipHostMeId=%d, devIfc=%s, vlanIfc=%s, ret=%d, exist=%d\n",
                   ipHostOid, ipHostMeId, devInterface, vlanInterface, ret, exist);

    return exist;
}

static UBOOL8 isAniVlanInterfaceExisted
    (const GemPortNetworkCtpObject *ctp,
     const UINT32                  uniOid,
     const UINT32                  uniMeId,
     const OmciMapFilterModelType  type __attribute__((unused)))
{
    UBOOL8 exist = FALSE;
    UINT32 phyId = 0, bridgeMeId = 0, omciBridgeMeId = 0;
    char   *devInterface = NULL, *vlanInterface = NULL;
    CmsRet ret = CMSRET_INTERNAL_ERROR;

    if ((ret = rutGpon_getIndexFromGemPortNetworkCtp(ctp, &phyId)) != CMSRET_SUCCESS)
        goto out;

    if ((ret = rutGpon_getBridgeMeIdFromUniMeId(uniOid, uniMeId,
                                                &omciBridgeMeId)) != CMSRET_SUCCESS)
        goto out;

    // only assign bridgeMeId to real omciBridgeMeId when UNI is NOT VEIP
    // since VEIP is connected to default bridge that has bridgeMeId is 0
    if (uniOid != MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT &&
        !rutGpon_isVeipPptpUni(uniOid, uniMeId))
    {
        bridgeMeId = omciBridgeMeId;
    }

    // generate ANI device interface name
    rutGpon_getInterfaceName(OMCI_PHY_GPON, bridgeMeId, &devInterface);
    if (rutGpon_isInterfaceExisted(devInterface) == TRUE)
    {
        // generate ANI virtual interface name
        rutGpon_getVirtualInterfaceName(OMCI_PHY_GPON, bridgeMeId, bridgeMeId, &vlanInterface);
        exist = rutGpon_isInterfaceExisted(vlanInterface);
        // free ANI virtual interface name
        CMSMEM_FREE_BUF_AND_NULL_PTR(vlanInterface);
    }
    // free ANI device interface name
    CMSMEM_FREE_BUF_AND_NULL_PTR(devInterface);

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> isAniVlanInterfaceExisted, ctpMeId=%d, phyId=%d, bridgeMeId=%d, omciBridgeMeId=%d, ret=%d, exist=%d\n",
                   ctp->managedEntityId, phyId, bridgeMeId, omciBridgeMeId, ret, exist);

    return exist;
}

static CmsRet addInterfaceToMapperBridge
    (const char   *devInterface __attribute__((unused)),
     const char   *vlanInterface,
     const UINT32 mapperMeId)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BCM_MapperServiceProfileObject *bcmMapper = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search BCM_MapperServiceProfileObject
    while ((!found) &&
           (_cmsObj_getNext(MDMOID_BC_MAPPER_SERVICE_PROFILE,
                           &iidStack,
                           (void **) &bcmMapper) == CMSRET_SUCCESS))
    {
        found = (bcmMapper->managedEntityId == mapperMeId);
        // if BCM_MapperServiceProfileObject is found then
        // add interface to the bridge of mapper
        if (found == TRUE && bcmMapper->bridgeName != NULL)
        {
            ret = rutGpon_addInterfaceToBridge(vlanInterface, bcmMapper->bridgeName);
            setIgmpRateLimitOnBridge(mapperMeId, bcmMapper->bridgeName,
              vlanInterface, FALSE);
        }
        _cmsObj_free((void **) &bcmMapper);
    }

omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "===> addInterfaceToMapperBridge, mapperMeId=%d, vlanInterface=%s, ret=%d\n", mapperMeId, vlanInterface, ret);

    return ret;
}

static CmsRet addInterfaceToServiceBridge
    (const char   *devInterface __attribute__((unused)),
     const char   *vlanInterface,
     const UINT32 bridgeMeId)
{
    char brName[BUFLEN_32];
    CmsRet ret;

    // search BCM_MacBridgeServiceProfileObject
    ret = rutGpon_getBridgeNameFromBridgeMeId(bridgeMeId, brName);
    if (ret == CMSRET_SUCCESS)
        // add interface to the bridge of service
    {
        ret = rutGpon_addInterfaceToBridge(vlanInterface, brName);
        setIgmpRateLimitOnBridge(bridgeMeId, brName, vlanInterface, FALSE);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "===> addInterfaceToServiceBridge, bridgeMeId=%d, vlanInterface=%s, ret=%d\n", bridgeMeId, vlanInterface, ret);

    return ret;
}

static CmsRet createVlanInterface
    (const UINT32      devId,
     const UBOOL8      isRouted,
     const UBOOL8      isMulticast __attribute__((unused)),
     char              *devInterface,
     char              *vlanInterface)
{
    UINT32 i = 0;
    CmsRet ret = CMSRET_SUCCESS;

    vlanCtl_init();
    vlanCtl_setIfSuffix(".");
    // always set isMulticast to 1 for supporting broadcast
    rutGpon_vlanCtl_createVlanInterface(devInterface, devId, isRouted, 1);
    vlanCtl_setRealDevMode(devInterface, BCM_VLAN_MODE_ONT);
    for (i = 0; i < BCM_VLAN_MAX_TAGS; ++i)
    {
        vlanCtl_setDefaultAction(devInterface, VLANCTL_DIRECTION_TX, i, VLANCTL_ACTION_DROP, vlanInterface);
        vlanCtl_setDefaultAction(devInterface, VLANCTL_DIRECTION_RX, i, VLANCTL_ACTION_DROP, vlanInterface);
    }
    vlanCtl_cleanup();

    if (rutGpon_isInterfaceExisted(vlanInterface) == FALSE)
    {
        cmsLog_error("Failed to create %s", vlanInterface);
        ret = CMSRET_INTERNAL_ERROR;
    }
    else
    {
        char cmd[BUFLEN_1024], macStr[MAC_STR_LEN+1];
        UINT8 macNum[MAC_ADDR_LEN];

        // bring virtual interface up if it's created successfully
        devCtl_getBaseMacAddress(macNum);
        cmsUtl_macNumToStr(macNum, macStr);
        snprintf(cmd, sizeof(cmd), "ifconfig %s hw ether %s up",
                             vlanInterface, macStr);
        _owapi_rut_doSystemAction("rcl_gpon", cmd);
    }

omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "===> createVlanInterface, devId=%d, isRouted=%d, isMulticast=%d, devIfc=%s, vlanIfc=%s, ret=%d\n", devId, isRouted, isMulticast, devInterface, vlanInterface, ret);

    return ret;
}

static CmsRet createVeipVlanInterface(void)
{
    UINT32 i = 0;
    char veipVlanName[CMS_IFNAME_LENGTH] = {0};
    CmsRet ret = CMSRET_SUCCESS;

    getVeipVlanName(veipVlanName);

    if (rutGpon_isInterfaceExisted(VEIP_DEV_INTERFACE) == FALSE)
    {
        cmsLog_error("Failed to create %s since %s does not exist yet", veipVlanName, VEIP_DEV_INTERFACE);
        ret = CMSRET_INTERNAL_ERROR;
        goto out;
    }

    vlanCtl_init();
    vlanCtl_setIfSuffix(".");
    // always set isMulticast to 1 for supporting broadcast
    rutGpon_vlanCtl_createVlanInterfaceByName(VEIP_DEV_INTERFACE, veipVlanName, 1, 1);
    vlanCtl_setRealDevMode(VEIP_DEV_INTERFACE, BCM_VLAN_MODE_ONT);
    for (i = 0; i < BCM_VLAN_MAX_TAGS; ++i)
    {
        vlanCtl_setDefaultAction(VEIP_DEV_INTERFACE, VLANCTL_DIRECTION_TX, i, VLANCTL_ACTION_DROP, veipVlanName);
        vlanCtl_setDefaultAction(VEIP_DEV_INTERFACE, VLANCTL_DIRECTION_RX, i, VLANCTL_ACTION_DROP, veipVlanName);
    }
    vlanCtl_cleanup();

    if (rutGpon_isInterfaceExisted(veipVlanName) == FALSE)
    {
        cmsLog_error("Failed to create %s", veipVlanName);
        ret = CMSRET_INTERNAL_ERROR;
    }
    else
    {
        char cmd[BUFLEN_1024], macStr[MAC_STR_LEN];
        UINT8 macNum[MAC_ADDR_LEN];

        // bring virtual interface up if it's created successfully
        devCtl_getBaseMacAddress(macNum);
        cmsUtl_macNumToStr(macNum, macStr);
        snprintf(cmd, sizeof(cmd), "ifconfig %s hw ether %s up",
                             veipVlanName, macStr);
        _owapi_rut_doSystemAction("rcl_gpon", cmd);
    }

out:
omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "===> createVeipVlanInterface, devId=%d, devIfc=%s, vlanIfc=%s, ret=%d\n", VEIP_DEV_ID, VEIP_DEV_INTERFACE, veipVlanName, ret);

    return ret;
}

static void setDefaultAction
    (char *devInterface,
     char *vlanInterface,
     const vlanCtl_defaultAction_t action,
     const UINT16 numTag,
     const OmciPhyInfo_t *phyInfo,
     const vlanCtl_direction_t direction)
{
    int retVlanCtl = 0;
    unsigned int nbrOfRules = 0;
    UINT32 tagRuleId = VLANCTL_DONT_CARE;
    UINT16 pbitQId = 0;
    UINT16 pbitQIdStart = 0;
    UINT16 pbitQIdEnd = 0;
    UBOOL8 isQFound = FALSE;
    //vlanCtl_defaultAction_t defaultAction = action;

    // get number of tag rules in table
    retVlanCtl = vlanCtl_getNbrOfRulesInTable(devInterface, direction, numTag, &nbrOfRules);

    // only insert default tag rule if table is empty
    if (retVlanCtl == 0 && nbrOfRules == 0)
    {
        if (action == VLANCTL_ACTION_ACCEPT)
        {
            if (direction == VLANCTL_DIRECTION_RX)
            {
                isQFound = rutGpon_getDsQueueIdRange(phyInfo,
                  &pbitQIdStart, &pbitQIdEnd);
                if ((numTag == 0) && (isQFound != TRUE))
                {
                    pbitQIdStart = pbitQIdEnd = OMCI_QID_NOT_EXIST;
                }
                for (pbitQId = pbitQIdStart; pbitQId <= pbitQIdEnd; pbitQId++)
                {
                    vlanCtl_initTagRule();
                    vlanCtl_setReceiveVlanDevice(vlanInterface);
                    if (skipSetQueue(numTag, pbitQId, isQFound) != TRUE)
                    {
                        rutwrap_setDsSkbMarkQueue(pbitQId, isQFound);
                    }
                    vlanCtl_insertTagRule(devInterface, direction,
                                          numTag, VLANCTL_POSITION_APPEND,
                                          VLANCTL_DONT_CARE, &tagRuleId);
                }
            }
            else if (direction == VLANCTL_DIRECTION_TX)
            {
                vlanCtl_initTagRule();
                // apply filter on vlanInterface
                vlanCtl_filterOnTxVlanDevice(vlanInterface);
                // Set rule to the tx tables
                vlanCtl_insertTagRule(devInterface, direction,
                                      numTag, VLANCTL_POSITION_APPEND,
                                      VLANCTL_DONT_CARE, &tagRuleId);
                // since NO-OP rule is treated as default rule
                // it does not need to be removed later ==>
                // it should not be added to omci_rule link list
            }
            //defaultAction = VLANCTL_ACTION_DROP;
        }
    }

//    vlanCtl_setDefaultAction(devInterface, direction, numTag, defaultAction, vlanInterface);

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> setDefaultAction, devIfc=%s, vlanIfc=%s, direction=%d, action=%d, nbrOfTags=%d, nbrOfRules=%d, retVlanCtl=%d\n",
                   CHECK_INTF_PTR(devInterface), CHECK_INTF_PTR(vlanInterface),
                   direction, action, numTag, nbrOfRules, retVlanCtl);
}

static void insertDefaultTagRules
    (char *devInterface,
     char *vlanInterface,
     const OmciPhyInfo_t *phyInfo,
     const vlanCtl_direction_t direction,
     const vlanCtl_defaultAction_t action)
{
    UINT32 i = 0;

    vlanCtl_init();

    for (i = 0; i < BCM_VLAN_MAX_TAGS; ++i)
    {
        setDefaultAction(devInterface, vlanInterface, action, i, phyInfo, direction);
    }

    vlanCtl_cleanup();

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> insertDefaultTagRules, devIfc=%s, vlanIfc=%s, direction=%d, action=%d\n",
                   devInterface, vlanInterface, direction, action);
}

/*****************************************************************************
*  FUNCTION:  insertDefaultAniTagRules
*  PURPOSE:   Insert default ANI rules on gpondef or gponx interfaces.
*  PARAMETERS:
*      devInterface - "real" device interface name.
*      vlanInterface - virtual interface name.
*      phyInfo - pointer to the ANI-side information.
*      direction - rule direction.
*      action - default rule action.
*  RETURNS:
*      None.
*  NOTES:
*      The downstream multicast GEM port case requires special handling -
*      both EXT VLAN (171) and MOP (309) ME may have rules applicable to the
*      IGMP/MLD or the multicast flow, and those rules may overlap. The MOP
*      derived rule should have higher priority than the EXT VLAN. In this
*      case, the multicast GEM port related RX rules on both gpondef and gponx
*      should be given higher priority than the unicast GEM ports.
*
*      Another possible option is to always include "--filter-skb-mark-port"
*      to filter based on GEM ports on the UNI interface. However it will
*      create the rule explosion situation: # of VLAN rules on the UNI
*      interface = (# of EXT VLAN rules) * (# of GEM ports).
*****************************************************************************/
static void insertDefaultAniTagRules
    (char *devInterface,
     char *vlanInterface,
     const OmciPhyInfo_t *phyInfo,
     const vlanCtl_direction_t direction,
     const vlanCtl_defaultAction_t action)
{
    UINT32 i = 0;
    UINT32 tagRuleId = VLANCTL_DONT_CARE;
    UINT16 pbitQId = 0;
    UINT16 pbitQIdStart = 0;
    UINT16 pbitQIdEnd = 0;
    UINT16 start = 0;
    UINT16 end = 0;
    UBOOL8 isQFound = FALSE;

    vlanCtl_init();

    for (i = 0; i < BCM_VLAN_MAX_TAGS; ++i)
    {
        vlanCtl_setDefaultAction(devInterface, direction, i, action, vlanInterface);
    }

    if (direction == VLANCTL_DIRECTION_RX)
    {
        isQFound = rutGpon_getDsQueueIdRange(phyInfo, &pbitQIdStart, &pbitQIdEnd);

        for (i = 0; i < BCM_VLAN_MAX_TAGS; ++i)
        {
            start = ((i == 0) && (isQFound != TRUE))?
              OMCI_QID_NOT_EXIST : pbitQIdStart;
            end = ((i == 0) && (isQFound != TRUE))?
              OMCI_QID_NOT_EXIST : pbitQIdEnd;

            for (pbitQId = start; pbitQId <= end; pbitQId++)
            {
                vlanCtl_initTagRule();
                // Forward frame to this vlanInterface
                vlanCtl_setReceiveVlanDevice(vlanInterface);
                // Filter packets on GEM-Index
                vlanCtl_filterOnSkbMarkPort(phyInfo->phyId);
                // Set downstream multicast priority queue if phyId
                // is gem port index of multicast gem port network ctp
                // and this gem port network ctp has configured
                // with downstream priority queue
                if (skipSetQueue(i, pbitQId, isQFound) != TRUE)
                {
                    rutwrap_setDsSkbMarkQueue(pbitQId, isQFound);
                }

                if (phyInfo->gemPortType == OMCI_SERVICE_MULTICAST)
                {
                    // Set rule to the top of the rx table
                    tagRuleId = VLANCTL_DONT_CARE;
                    vlanCtl_insertTagRule(devInterface, direction,
                                      i, VLANCTL_POSITION_BEFORE,
                                      VLANCTL_DONT_CARE, &tagRuleId);
                }
                else
                {
                    // Set rule to the end of the rx table
                    vlanCtl_insertTagRule(devInterface, direction,
                                      i, VLANCTL_POSITION_APPEND,
                                      VLANCTL_DONT_CARE, &tagRuleId);
                }
                omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, i, tagRuleId);
            }
        }
    }
    else if (direction == VLANCTL_DIRECTION_TX)
    {
        // On tx, filter with the given TCI
        vlanCtl_initTagRule();
        // apply filter on vlanInterface
        vlanCtl_filterOnTxVlanDevice(vlanInterface);
        // set gem id for these tag rules to forward
        // traffic to this gem port on upstream direction
        vlanCtl_cmdSetSkbMarkPort(phyInfo->phyId);
        if ((getOnuOmciTmOption() != OMCI_TRAFFIC_MANAGEMENT_RATE) &&
          (rutGpon_getQidFromGemPortIndex(phyInfo->phyId, OMCI_FLOW_UPSTREAM, &pbitQId)
          == CMSRET_SUCCESS))
        {
            rutwrap_vlanCtl_cmdSetSkbMarkQueue(pbitQId);
        }
        // Set rule to the tx tables
        for (i = 0; i < BCM_VLAN_MAX_TAGS; ++i)
        {
            vlanCtl_insertTagRule(devInterface, direction,
                                  i, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, i, tagRuleId);
        }
    }

    vlanCtl_cleanup();

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> insertDefaultAniTagRules, devIfc=%s, vlanIfc=%s, phyId=%d, direction=%d, action=%d\n",
                   devInterface, vlanInterface, phyInfo->phyId, direction, action);
}

static OmciTagAction getExtVlanTagAction
    (const OmciExtVlanTagOper_t *pExtVlanTag)
{
    OmciTagAction tagAction = OMCI_TAG_DO_NOTHING;

    switch (pExtVlanTag->filterType)
    {
        case OMCI_FILTER_TYPE_DOUBLE_TAG:
            switch (pExtVlanTag->removeType)
            {
                case OMCI_TREATMENT_REMOVE_BOTH:
                    if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                        pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // double-tagged rule: 2 -2 + 2 = 2 => replace 2 current tags
                        tagAction = OMCI_DOUBLE_TAG_REPLACE;
                    else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // double-tagged rule: 2 -2 + 1 = 1 => remove outer tag
                        tagAction = OMCI_SINGLE_TAG_REMOVE;
                    else
                        // double-tagged rule: 2 -2 + 0 = 0 => remove outer and inner tags
                        tagAction = OMCI_DOUBLE_TAG_REMOVE;
                    break;
                case OMCI_TREATMENT_REMOVE_OUTER:
                    if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                        pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // double-tagged rule: 2 -1 + 2 = 3 => invalid, but replace outer tag
                        tagAction = OMCI_DOUBLE_TAG_REPLACE;
                    else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // double-tagged rule: 2 -1 + 1 = 2 => replace outer tag
                        tagAction = OMCI_DOUBLE_TAG_REPLACE;
                    else
                        // double-tagged rule: 2 -1 + 0 = 1 => remove outer tag
                        tagAction = OMCI_SINGLE_TAG_REMOVE;
                    break;
                default:
                    if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                        pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // double-tagged rule: 2 + 0 + 2 = 4 => invalid, do nothing
                        tagAction = OMCI_TAG_DO_NOTHING;
                    else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // double-tagged rule: 2 + 0 + 1 = 3 => invalid, do nothing
                        tagAction = OMCI_TAG_DO_NOTHING;
                    else
                        // double-tagged rule: 2 + 0 + 0 = 2 => keep current tags, do nothing
                        tagAction = OMCI_TAG_DO_NOTHING;
                    break;
                    break;
            }
            break;
        case OMCI_FILTER_TYPE_SINGLE_TAG:
            switch (pExtVlanTag->removeType)
            {
                case OMCI_TREATMENT_REMOVE_BOTH:
                    if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                        pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // single-tagged rule: 1 -2 + 2 = 1 => keep current tag
                        tagAction = OMCI_SINGLE_TAG_REPLACE;
                    else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // single-tagged rule: 1 -2 + 1 = 0 => remove outer tag
                        tagAction = OMCI_SINGLE_TAG_REMOVE;
                    else
                        // single-tagged rule: 1 -2 + 0 = -1 => invalid (but remove outer tag)
                        tagAction = OMCI_SINGLE_TAG_REMOVE;
                    break;
                case OMCI_TREATMENT_REMOVE_OUTER:
                    if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                        pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // single-tagged rule: 1 -1 + 2 = 2 => add outer tag
                        tagAction = OMCI_SINGLE_TAG_ADD;
                    else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // single-tagged rule: 1 -1 + 1 = 1 => replace current tag
                        tagAction = OMCI_SINGLE_TAG_REPLACE;
                    else
                        // single-tagged rule: 1 -1 + 0 = 0 => remove outer tag
                        tagAction = OMCI_SINGLE_TAG_REMOVE;
                    break;
                default:
                    if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                        pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // single-tagged rule: 1 + 0 + 2 = 3 => invalid (but add outer tag)
                        tagAction = OMCI_SINGLE_TAG_ADD;
                    else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // single-tagged rule: 1 + 0 + 1 = 2 => add outer tag
                        tagAction = OMCI_SINGLE_TAG_ADD;
                    else
                        // single-tagged rule: 1 + 0 + 0 = 1 => keep current tag, do nothing
                        tagAction = OMCI_TAG_DO_NOTHING;
                    break;
            }
            break;
        default:
            switch (pExtVlanTag->removeType)
            {
                case OMCI_TREATMENT_REMOVE_BOTH:
                    if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                        pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // un-tagged rule: -2 + 2 = 0 => do nothing
                        tagAction = OMCI_TAG_DO_NOTHING;
                    else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // un-tagged rule: -2 + 1 = -1 => invalid => do nothing
                        tagAction = OMCI_TAG_DO_NOTHING;
                    else
                        // un-tagged rule: -2 + 0 = -2 => invalid => do nothing
                        tagAction = OMCI_TAG_DO_NOTHING;
                    break;
                case OMCI_TREATMENT_REMOVE_OUTER:
                    if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                        pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // un-tagged rule: -1 + 2 = 1 => add outer tag
                        tagAction = OMCI_SINGLE_TAG_ADD;
                    else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // un-tagged rule: -1 + 1 = 0 => do nothing
                        tagAction = OMCI_TAG_DO_NOTHING;
                    else
                        // un-tagged rule: -1 + 0 = -1 => invalid => do nothing
                        tagAction = OMCI_TAG_DO_NOTHING;
                    break;
                default:
                    if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                        pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // un-tagged rule: 0 + 2 = 2 => add outer and inner tags
                        tagAction = OMCI_DOUBLE_TAG_ADD;
                    else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                        // un-tagged rule: 0 + 1 = 1 => add outer tag
                        tagAction = OMCI_SINGLE_TAG_ADD;
                    else
                        // un-tagged rule: 0 + 0 = 0 => do nothing
                        tagAction = OMCI_TAG_DO_NOTHING;
                    break;
            }
            break;
    }

    return tagAction;
}

static void insertExtVlanUsTagRules
    (const UINT16                    uniOid,
     const UINT16                    uniMeId,
     const UINT16                    inputTpid,
     const UINT16                    outputTpid,
     const OmciTagAction             tagAction,
     const OmciExtVlanTagOper_t      *pExtVlanTag,
     const OmciPhyInfo_t *phyInfo,
     char  *devInterface,
     char  *vlanInterface)
{
    UINT32 tagIndex = 0;
    UINT32 tagRuleId = VLANCTL_DONT_CARE;
    vlanCtl_direction_t direction = VLANCTL_DIRECTION_RX;

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "===> insertExtVlanUsTagRules: tagAction=%d, phyId=%d, devIfc=%s, vlanIfc=%s\n",
      tagAction, phyInfo->phyId, devInterface, vlanInterface);

    vlanCtl_initTagRule();

    // get direction and apply filter
    getUpstreamDirectionFromUniOid(uniOid, uniMeId, &direction, vlanInterface);

    // filter outer
    if (pExtVlanTag->outer.filter.prio != OMCI_FILTER_IGNORE)
    {
        // filter outer priority
        if (pExtVlanTag->outer.filter.prio <= OMCI_FILTER_PRIO_MAX)
            vlanCtl_filterOnTagPbits(pExtVlanTag->outer.filter.prio, tagIndex);
        // filter outer vlan id
        if (pExtVlanTag->outer.filter.vlanId <= OMCI_FILTER_VLANID_MAX)
            vlanCtl_filterOnTagVid(pExtVlanTag->outer.filter.vlanId, tagIndex);
        // setup tagIndex for filter inner
        tagIndex = 1;
    }
    // filter inner
    if (pExtVlanTag->inner.filter.prio != OMCI_FILTER_IGNORE)
    {
        // filter inner priority
        if (pExtVlanTag->inner.filter.prio <= OMCI_FILTER_PRIO_MAX)
            vlanCtl_filterOnTagPbits(pExtVlanTag->inner.filter.prio, tagIndex);
        // filter inner vlan id
        if (pExtVlanTag->inner.filter.vlanId <= OMCI_FILTER_VLANID_MAX)
            vlanCtl_filterOnTagVid(pExtVlanTag->inner.filter.vlanId, tagIndex);
    }

    // filter tpid_de and ethertype
    switch (pExtVlanTag->filterType)
    {
        case OMCI_FILTER_TYPE_DOUBLE_TAG:
            // filter outer tpid_de which is ethertype
            switch (pExtVlanTag->outer.filter.tpid_de)
            {
                case OMCI_FILTER_TPID_8100_DE_X:
                    vlanCtl_filterOnEthertype(Q_TAG_TPID);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_X:
                    vlanCtl_filterOnEthertype(inputTpid);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_0:
                    vlanCtl_filterOnEthertype(inputTpid);
                    vlanCtl_filterOnTagCfi(0, 0);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_1:
                    vlanCtl_filterOnEthertype(inputTpid);
                    vlanCtl_filterOnTagCfi(1, 0);
                    break;
                default:
                    break;
            }
            // filter inner tpid_de which is outer tag ethertype
            switch (pExtVlanTag->inner.filter.tpid_de)
            {
                case OMCI_FILTER_TPID_8100_DE_X:
                    vlanCtl_filterOnTagEtherType(Q_TAG_TPID, 0);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_X:
                    vlanCtl_filterOnTagEtherType(inputTpid, 0);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_0:
                    vlanCtl_filterOnTagEtherType(inputTpid, 0);
                    vlanCtl_filterOnTagCfi(0, 1);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_1:
                    vlanCtl_filterOnTagEtherType(inputTpid, 0);
                    vlanCtl_filterOnTagCfi(1, 1);
                    break;
                default:
                    break;
            }
            // filter ethertype which is inner tag ethertype
            switch (pExtVlanTag->etherType)
            {
                case OMCI_FILTER_ETHER_IPOE:
                    vlanCtl_filterOnTagEtherType(OMCI_FILTER_ETHER_0800, 1);
                    break;
                case OMCI_FILTER_ETHER_PPPOE:
                    vlanCtl_filterOnTagEtherType(OMCI_FILTER_ETHER_8863, 1);
                    break;
                case OMCI_FILTER_ETHER_PPPOE_2:
                    vlanCtl_filterOnTagEtherType(OMCI_FILTER_ETHER_8864, 1);
                    break;
                case OMCI_FILTER_ETHER_ARP:
                    vlanCtl_filterOnTagEtherType(OMCI_FILTER_ETHER_0806, 1);
                    break;
                default:
                    break;
            }
            break;
        case OMCI_FILTER_TYPE_SINGLE_TAG:
            // filter inner tpid_de which is ethertype
            switch (pExtVlanTag->inner.filter.tpid_de)
            {
                case OMCI_FILTER_TPID_8100_DE_X:
                    vlanCtl_filterOnEthertype(Q_TAG_TPID);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_X:
                    vlanCtl_filterOnEthertype(inputTpid);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_0:
                    vlanCtl_filterOnEthertype(inputTpid);
                    vlanCtl_filterOnTagCfi(0, 0);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_1:
                    vlanCtl_filterOnEthertype(inputTpid);
                    vlanCtl_filterOnTagCfi(1, 0);
                    break;
                default:
                    break;
            }
            // filter ethertype which is outer tag ethertype
            switch (pExtVlanTag->etherType)
            {
                case OMCI_FILTER_ETHER_IPOE:
                    vlanCtl_filterOnTagEtherType(OMCI_FILTER_ETHER_0800, 0);
                    break;
                case OMCI_FILTER_ETHER_PPPOE:
                    vlanCtl_filterOnTagEtherType(OMCI_FILTER_ETHER_8863, 0);
                    break;
                case OMCI_FILTER_ETHER_PPPOE_2:
                    vlanCtl_filterOnTagEtherType(OMCI_FILTER_ETHER_8864, 0);
                    break;
                case OMCI_FILTER_ETHER_ARP:
                    vlanCtl_filterOnTagEtherType(OMCI_FILTER_ETHER_0806, 0);
                    break;
                default:
                    break;
            }
            break;
        default:
            // filter ethertype which is the real ethertype
            switch (pExtVlanTag->etherType)
            {
                case OMCI_FILTER_ETHER_IPOE:
                    vlanCtl_filterOnEthertype(OMCI_FILTER_ETHER_0800);
                    break;
                case OMCI_FILTER_ETHER_PPPOE:
                    vlanCtl_filterOnEthertype(OMCI_FILTER_ETHER_8863);
                    break;
                case OMCI_FILTER_ETHER_PPPOE_2:
                    vlanCtl_filterOnEthertype(OMCI_FILTER_ETHER_8864);
                    break;
                case OMCI_FILTER_ETHER_ARP:
                    vlanCtl_filterOnEthertype(OMCI_FILTER_ETHER_0806);
                    break;
                default:
                    break;
            }
            break;
    }

    // handle action tag
    switch (tagAction)
    {
        case OMCI_SINGLE_TAG_ADD:
             /* push the outer tag */
            vlanCtl_cmdPushVlanTag();
            break;
        case OMCI_SINGLE_TAG_REMOVE:
             /* pop the outer tag */
            vlanCtl_cmdPopVlanTag();
            break;
        case OMCI_DOUBLE_TAG_ADD:
             /* push both outer and inner tags */
            vlanCtl_cmdPushVlanTag();
            vlanCtl_cmdPushVlanTag();
            break;
        case OMCI_DOUBLE_TAG_REMOVE:
             /* pop both outer and inner tags */
            vlanCtl_cmdPopVlanTag();
            vlanCtl_cmdPopVlanTag();
            break;
        default:
            break;
    }

    // treatment outer priority
    if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
        pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
    {
        // set treatment tagIndex to outer tag
        tagIndex = 0;
        // Set pbits in tag number 0, which is always the outer tag of the frame
        if (pExtVlanTag->outer.treatment.prio <= OMCI_TREATMENT_PRIO_MAX)
            vlanCtl_cmdSetTagPbits(pExtVlanTag->outer.treatment.prio, tagIndex);
        // handle OMCI_TREATMENT_PRIO_COPY_FROM_INNER or OMCI_TREATMENT_PRIO_COPY_FROM_OUTER
        else
        {
            // double tagged rule: replace both outer and inner tags
            // then copy pbits as request for outer
            if (tagAction == OMCI_DOUBLE_TAG_REPLACE)
            {
                if (pExtVlanTag->outer.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_INNER)
                    vlanCtl_cmdCopyTagPbits(1, tagIndex);
                else if (pExtVlanTag->outer.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
                    vlanCtl_cmdCopyTagPbits(0, tagIndex);
                else if (pExtVlanTag->outer.treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP)
                    vlanCtl_cmdDscpToPbits(tagIndex);
            }
        }
        // Set vid in tag number 0, which is always the outer tag of the frame
        if (pExtVlanTag->outer.treatment.vlanId <= OMCI_TREATMENT_VLANID_MAX)
            vlanCtl_cmdSetTagVid(pExtVlanTag->outer.treatment.vlanId, tagIndex);
        // handle OMCI_TREATMENT_VLANID_COPY_FROM_INNER or OMCI_TREATMENT_VLANID_COPY_FROM_OUTER
        else
        {
            // double tagged rule: replace both outer and inner tags
            // then copy vlanId as request for outer
            if (tagAction == OMCI_DOUBLE_TAG_REPLACE)
            {
                if (pExtVlanTag->outer.treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_INNER)
                    vlanCtl_cmdCopyTagVid(1, tagIndex);
                else if (pExtVlanTag->outer.treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_OUTER)
                    vlanCtl_cmdCopyTagVid(0, tagIndex);
            }
        }

        // Set outer tpid which is the real ethertype
        // Set cfi in tag number 0 which is the outer tag of the frame
        switch (pExtVlanTag->outer.treatment.tpid_de)
        {
/*
            case OMCI_TREATMENT_TPID_DE_COPY_FROM_INNER:
                vlanCtl_cmdCopyTagEtherType(1, tagIndex);
                vlanCtl_cmdCopyTagCfi(1, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_DE_COPY_FROM_OUTER:
                vlanCtl_cmdCopyTagEtherType(0, tagIndex);
                vlanCtl_cmdCopyTagCfi(0, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER:
                vlanCtl_cmdSetEtherType(outputTpid);
                vlanCtl_cmdCopyTagCfi(1, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_OUTER:
                vlanCtl_cmdSetEtherType(outputTpid);
                vlanCtl_cmdCopyTagCfi(0, tagIndex);
                break;
*/
            case OMCI_TREATMENT_TPID_8100_DE_X:
                vlanCtl_cmdSetEtherType(C_TAG_TPID);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
                vlanCtl_cmdSetEtherType(outputTpid);
                vlanCtl_cmdSetTagCfi(0, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
                vlanCtl_cmdSetEtherType(outputTpid);
                vlanCtl_cmdSetTagCfi(1, tagIndex);
                break;
            default:
                break;
         }

        // set treatment tagIndex to inner tag
        tagIndex = 1;
        // Set pbits in tag number 1, which is always the inner tag of the frame
        if (pExtVlanTag->inner.treatment.prio <= OMCI_TREATMENT_PRIO_MAX)
            vlanCtl_cmdSetTagPbits(pExtVlanTag->inner.treatment.prio, tagIndex);
        // handle OMCI_TREATMENT_PRIO_COPY_FROM_INNER or OMCI_TREATMENT_PRIO_COPY_FROM_OUTER
        else
        {
            // double tagged rule: replace both outer and inner tags
            // then copy pbits as request for outer
            if (tagAction == OMCI_DOUBLE_TAG_REPLACE)
            {
                if (pExtVlanTag->inner.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_INNER)
                    vlanCtl_cmdCopyTagPbits(1, tagIndex);
                else if (pExtVlanTag->inner.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
                    vlanCtl_cmdCopyTagPbits(0, tagIndex);
                else if (pExtVlanTag->inner.treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP)
                    vlanCtl_cmdDscpToPbits(tagIndex);
            }
        }
        // Set vid in tag number 1, which is always the inner tag of the frame
        if (pExtVlanTag->inner.treatment.vlanId <= OMCI_TREATMENT_VLANID_MAX)
            vlanCtl_cmdSetTagVid(pExtVlanTag->inner.treatment.vlanId, tagIndex);
        // handle OMCI_TREATMENT_VLANID_COPY_FROM_INNER or OMCI_TREATMENT_VLANID_COPY_FROM_OUTER
        else
        {
            // double tagged rule: replace both outer and inner tags
            // then copy vlanId as request for inner
            if (tagAction == OMCI_DOUBLE_TAG_REPLACE)
            {
                if (pExtVlanTag->inner.treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_INNER)
                    vlanCtl_cmdCopyTagVid(1, tagIndex);
                else if (pExtVlanTag->inner.treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_OUTER)
                    vlanCtl_cmdCopyTagVid(0, tagIndex);
            }
        }

        // Set inner tpid which is the outer tag ethertype
        // Set cfi in tag number 1 which is the inner tag of the frame
        switch (pExtVlanTag->inner.treatment.tpid_de)
        {
/*
            case OMCI_TREATMENT_TPID_DE_COPY_FROM_INNER:
                vlanCtl_cmdCopyTagEtherType(1, 0);
                vlanCtl_cmdCopyTagCfi(1, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_DE_COPY_FROM_OUTER:
                vlanCtl_cmdCopyTagEtherType(0, 0);
                vlanCtl_cmdCopyTagCfi(0, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER:
                vlanCtl_cmdSetTagEtherType(outputTpid, 0);
                vlanCtl_cmdCopyTagCfi(1, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_OUTER:
                vlanCtl_cmdSetTagEtherType(outputTpid, 0);
                vlanCtl_cmdCopyTagCfi(0, tagIndex);
                break;
*/
            case OMCI_TREATMENT_TPID_8100_DE_X:
                vlanCtl_cmdSetTagEtherType(C_TAG_TPID, 0);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
                vlanCtl_cmdSetTagEtherType(outputTpid, 0);
                vlanCtl_cmdSetTagCfi(0, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
                vlanCtl_cmdSetTagEtherType(outputTpid, 0);
                vlanCtl_cmdSetTagCfi(1, tagIndex);
                break;
            default:
                break;
         }
    }
    else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
    {
        // set treatment tagIndex to outer tag
        tagIndex = 0;
        // Set pbits in tag number 0, which is the outer tag of the frame
        if (pExtVlanTag->inner.treatment.prio <= OMCI_TREATMENT_PRIO_MAX)
            vlanCtl_cmdSetTagPbits(pExtVlanTag->inner.treatment.prio, tagIndex);
        else if (pExtVlanTag->inner.treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP)
            vlanCtl_cmdDscpToPbits(tagIndex);
        else if (pExtVlanTag->inner.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_INNER)
        {
            // single tagged rule: push outer to inner
            // then copy pbits from inner to outer
            if (pExtVlanTag->filterType == OMCI_FILTER_TYPE_SINGLE_TAG &&
                tagAction == OMCI_SINGLE_TAG_ADD)
                vlanCtl_cmdCopyTagPbits(1, tagIndex);
            // double tagged rule: replace outer tag by inner treatment
            // then copy pbits from inner to outer
            else if (tagAction == OMCI_DOUBLE_TAG_REPLACE)
                vlanCtl_cmdCopyTagPbits(1, tagIndex);
        }
        else if (pExtVlanTag->inner.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
        {
            // do nothing
        }

        // Set vid in tag number 0, which is the outer tag of the frame
        if (pExtVlanTag->inner.treatment.vlanId <= OMCI_TREATMENT_VLANID_MAX)
            vlanCtl_cmdSetTagVid(pExtVlanTag->inner.treatment.vlanId, tagIndex);
        else if (pExtVlanTag->inner.treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_INNER)
        {
            // single tagged rule: push outer to inner
            // then copy vlanid from inner to outer
            if (pExtVlanTag->filterType == OMCI_FILTER_TYPE_SINGLE_TAG &&
                tagAction == OMCI_SINGLE_TAG_ADD)
                vlanCtl_cmdCopyTagVid(1, tagIndex);
            // double tagged rule: replace outer tag by inner treatment
            // then copy vlanid from inner to outer
            else if (tagAction == OMCI_DOUBLE_TAG_REPLACE)
                vlanCtl_cmdCopyTagVid(1, tagIndex);
        }
        else if (pExtVlanTag->inner.treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_OUTER)
        {
            // do nothing
        }

        // Set outer tpid which is the real ethertype
        // Set cfi in tag number 0 which is the outer tag of the frame
        switch (pExtVlanTag->inner.treatment.tpid_de)
        {
/*
            case OMCI_TREATMENT_TPID_DE_COPY_FROM_INNER:
                vlanCtl_cmdCopyTagEtherType(1, tagIndex);
                vlanCtl_cmdCopyTagCfi(1, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_DE_COPY_FROM_OUTER:
                vlanCtl_cmdCopyTagEtherType(0, tagIndex);
                vlanCtl_cmdCopyTagCfi(0, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER:
                vlanCtl_cmdSetEtherType(outputTpid);
                vlanCtl_cmdCopyTagCfi(1, tagIndex);
                break;
*/
            case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_OUTER:
                vlanCtl_cmdSetEtherType(outputTpid);
                break;
            case OMCI_TREATMENT_TPID_8100_DE_X:
                vlanCtl_cmdSetEtherType(C_TAG_TPID);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
                vlanCtl_cmdSetEtherType(outputTpid);
                vlanCtl_cmdSetTagCfi(0, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
                vlanCtl_cmdSetEtherType(outputTpid);
                vlanCtl_cmdSetTagCfi(1, tagIndex);
                break;
            default:
                break;
         }
    }

    if (pExtVlanTag->filterType == OMCI_FILTER_TYPE_DOUBLE_TAG)
    {
        // insert tag rule to double-tagged rule table
        vlanCtl_insertTagRule(devInterface, direction,
                              2, VLANCTL_POSITION_APPEND,
                              VLANCTL_DONT_CARE, &tagRuleId);
        omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 2, tagRuleId);
    }
    else if (pExtVlanTag->filterType == OMCI_FILTER_TYPE_SINGLE_TAG)
    {
        // insert tag rule to singe-tagged rule table
        vlanCtl_insertTagRule(devInterface, direction,
                              1, VLANCTL_POSITION_APPEND,
                              VLANCTL_DONT_CARE, &tagRuleId);
        omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 1, tagRuleId);
    }
    else
    {
        // insert tag rule to un-tagged rule table
        vlanCtl_insertTagRule(devInterface, direction,
                              0, VLANCTL_POSITION_APPEND,
                              VLANCTL_DONT_CARE, &tagRuleId);
        omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 0, tagRuleId);
    }
}

static void insertExtVlanDsTagRules
    (const UINT16                    uniOid,
     const UINT16                    uniMeId,
     const UINT16                    inputTpid,
     const UINT16                    outputTpid,
     const OmciTagAction             tagAction,
     const OmciExtVlanTagOper_t      *pExtVlanTag,
     const OmciPhyInfo_t *phyInfo,
     char  *devInterface,
     char  *vlanInterface)
{
    UINT32 tagIndex = 0;
    UINT32 tagRuleId = VLANCTL_DONT_CARE;
    OmciFilterType txFilterType = OMCI_FILTER_TYPE_UNTAG;
    vlanCtl_direction_t direction = VLANCTL_DIRECTION_TX;

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE, "===> insertExtVlanDsTagRules: tagAction=%d, devIfc=%s, vlanIfc=%s\n", tagAction, devInterface, vlanInterface);

    vlanCtl_initTagRule();

    getDownstreamDirectionFromUniOid(uniOid, uniMeId, &direction,
      vlanInterface);

    // For Tx direction:
    // treatment info becomes filter info

    // filter outer
    if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
        pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
    {
        // filter outer priority
        if (pExtVlanTag->outer.treatment.prio <= OMCI_FILTER_PRIO_MAX)
            vlanCtl_filterOnTagPbits(pExtVlanTag->outer.treatment.prio, tagIndex);
        // filter outer vlan id
        if (pExtVlanTag->outer.treatment.vlanId <= OMCI_FILTER_VLANID_MAX)
            vlanCtl_filterOnTagVid(pExtVlanTag->outer.treatment.vlanId, tagIndex);
        // filter outer tpid which is the real ethertype
        // filter cfi in tag number 0 which is the outer tag of the frame
        switch (pExtVlanTag->outer.treatment.tpid_de)
        {
/*
            case OMCI_TREATMENT_TPID_DE_COPY_FROM_INNER:
                vlanCtl_cmdCopyTagEtherType(1, tagIndex);
                vlanCtl_cmdCopyTagCfi(1, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_DE_COPY_FROM_OUTER:
                vlanCtl_cmdCopyTagEtherType(0, tagIndex);
                vlanCtl_cmdCopyTagCfi(0, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER:
                vlanCtl_cmdSetEtherType(outputTpid);
                vlanCtl_cmdCopyTagCfi(1, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_OUTER:
                vlanCtl_cmdSetEtherType(outputTpid);
                vlanCtl_cmdCopyTagCfi(0, tagIndex);
                break;
*/
            case OMCI_TREATMENT_TPID_8100_DE_X:
                vlanCtl_filterOnEthertype(Q_TAG_TPID);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
                vlanCtl_filterOnEthertype(outputTpid);
                vlanCtl_filterOnTagCfi(0, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
                vlanCtl_filterOnEthertype(outputTpid);
                vlanCtl_filterOnTagCfi(1, tagIndex);
                break;
            default:
                break;
        }
        // setup tagIndex for filter inner
        tagIndex = 1;

        // filter inner priority
        if (pExtVlanTag->inner.treatment.prio <= OMCI_FILTER_PRIO_MAX)
            vlanCtl_filterOnTagPbits(pExtVlanTag->inner.treatment.prio, tagIndex);
        // filter inner vlan id
        if (pExtVlanTag->inner.treatment.vlanId <= OMCI_FILTER_VLANID_MAX)
            vlanCtl_filterOnTagVid(pExtVlanTag->inner.treatment.vlanId, tagIndex);
        // Filter inner tpid which is the outer tag ethertype
        // Filter cfi in tag number 1 which is the inner tag of the frame
        switch (pExtVlanTag->inner.treatment.tpid_de)
        {
/*
            case OMCI_TREATMENT_TPID_DE_COPY_FROM_INNER:
                vlanCtl_cmdCopyTagEtherType(1, 0);
                vlanCtl_cmdCopyTagCfi(1, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_DE_COPY_FROM_OUTER:
                vlanCtl_cmdCopyTagEtherType(0, 0);
                vlanCtl_cmdCopyTagCfi(0, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER:
                vlanCtl_cmdSetTagEtherType(outputTpid, 0);
                vlanCtl_cmdCopyTagCfi(1, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_OUTER:
                vlanCtl_cmdSetTagEtherType(outputTpid, 0);
                vlanCtl_cmdCopyTagCfi(0, tagIndex);
                break;
*/
            case OMCI_TREATMENT_TPID_8100_DE_X:
                vlanCtl_filterOnTagEtherType(Q_TAG_TPID, 0);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
                vlanCtl_filterOnTagEtherType(outputTpid, 0);
                vlanCtl_filterOnTagCfi(0, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
                vlanCtl_filterOnTagEtherType(outputTpid, 0);
                vlanCtl_filterOnTagCfi(1, tagIndex);
                break;
            default:
                break;
         }
    }
    // inner treatments become outer filters
    else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
    {
        // setup tagIndex for outer filter
        tagIndex = 0;

        // inner priority treatment becomes outer priority filter
        if (pExtVlanTag->inner.treatment.prio <= OMCI_FILTER_PRIO_MAX)
            vlanCtl_filterOnTagPbits(pExtVlanTag->inner.treatment.prio, tagIndex);
        else if (pExtVlanTag->inner.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_INNER)
        {
            // RX: single tagged rule: push outer to inner
            // then copy pbits from inner to outer ===>
            // TX: double tagged rule: pop outer
            // then set pbits filter from inner to outer
            if (pExtVlanTag->filterType == OMCI_FILTER_TYPE_SINGLE_TAG &&
                tagAction == OMCI_SINGLE_TAG_ADD)
                vlanCtl_filterOnTagPbits(pExtVlanTag->inner.filter.prio, tagIndex);
            // RX: double tagged rule: replace outer tag by pop
            // and push then copy pbits from inner to outer ===>
            // TX: double tagged rule: pop outer and push
            // then set pbits filter from inner to outer
            else if (tagAction == OMCI_DOUBLE_TAG_REPLACE)
                vlanCtl_filterOnTagPbits(pExtVlanTag->inner.filter.prio, tagIndex);
        }
        else if (pExtVlanTag->inner.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
        {
            // do nothing
        }

        // inner vlanId treatment becomes outer vlanId filter
        if (pExtVlanTag->inner.treatment.vlanId <= OMCI_FILTER_VLANID_MAX)
            vlanCtl_filterOnTagVid(pExtVlanTag->inner.treatment.vlanId, tagIndex);
        else if (pExtVlanTag->inner.treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_INNER)
        {
            // RX: single tagged rule: push outer to inner
            // then copy vlanid from inner to outer ===>
            // TX: double tagged rule: pop outer
            // then set vlanid filter from inner to outer
            if (pExtVlanTag->filterType == OMCI_FILTER_TYPE_SINGLE_TAG &&
                tagAction == OMCI_SINGLE_TAG_ADD)
                vlanCtl_filterOnTagVid(pExtVlanTag->inner.filter.vlanId, tagIndex);
            // RX : double tagged rule: replace outer tag by pop
            // and push then copy vlanid from inner to outer ==>
            // TX: double tagged rule: pop outer and push out
            // then set vlanid filter from inner to outer
            else if (tagAction == OMCI_DOUBLE_TAG_REPLACE)
                vlanCtl_filterOnTagVid(pExtVlanTag->inner.filter.vlanId, tagIndex);
        }
        else if (pExtVlanTag->inner.treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_OUTER)
        {
            // do nothing
        }

        // Filter inner tpid which is the real ethertype
        // Filter cfi in tag number 0 which is the outer tag of the frame
        switch (pExtVlanTag->inner.treatment.tpid_de)
        {
/*
            case OMCI_TREATMENT_TPID_DE_COPY_FROM_INNER:
                vlanCtl_cmdCopyTagEtherType(1, tagIndex);
                vlanCtl_cmdCopyTagCfi(1, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_DE_COPY_FROM_OUTER:
                vlanCtl_cmdCopyTagEtherType(0, tagIndex);
                vlanCtl_cmdCopyTagCfi(0, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER:
                vlanCtl_cmdSetEtherType(outputTpid);
                vlanCtl_cmdCopyTagCfi(1, tagIndex);
                break;
*/
            case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_OUTER:
                vlanCtl_filterOnEthertype(outputTpid);
                break;
            case OMCI_TREATMENT_TPID_8100_DE_X:
                vlanCtl_filterOnEthertype(Q_TAG_TPID);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
                vlanCtl_filterOnEthertype(outputTpid);
                vlanCtl_filterOnTagCfi(0, tagIndex);
                break;
            case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
                vlanCtl_filterOnEthertype(outputTpid);
                vlanCtl_filterOnTagCfi(1, tagIndex);
                break;
            default:
                break;
         }
    }

    // handle action tag
    // For Tx direction: action is reversed
    switch (tagAction)
    {
        case OMCI_SINGLE_TAG_ADD:
             /* pop the outer tag */
            vlanCtl_cmdPopVlanTag();
            break;
        case OMCI_SINGLE_TAG_REMOVE:
             /* push the outer tag */
            vlanCtl_cmdPushVlanTag();
            break;
        case OMCI_DOUBLE_TAG_ADD:
             /* pop both outer and inner tags */
            vlanCtl_cmdPopVlanTag();
            vlanCtl_cmdPopVlanTag();
            break;
        case OMCI_DOUBLE_TAG_REMOVE:
             /* push both outer and inner tags */
            vlanCtl_cmdPushVlanTag();
            vlanCtl_cmdPushVlanTag();
            break;
        default:
            break;
    }

    // For Tx direction:
    // treatment info becomes filter info

    // treatment outer priority
    if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
        pExtVlanTag->outer.filter.prio != OMCI_FILTER_IGNORE &&
        pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
        pExtVlanTag->inner.filter.prio != OMCI_FILTER_IGNORE )
    {
        // set treatment tagIndex to outer tag
        tagIndex = 0;

        // Set pbits in tag number 0, which is always the outer tag of the frame
        if (pExtVlanTag->outer.filter.prio <= OMCI_TREATMENT_PRIO_MAX)
            vlanCtl_cmdSetTagPbits(pExtVlanTag->outer.filter.prio, tagIndex);
        // Set vid in tag number 0, which is always the outer tag of the frame
        if (pExtVlanTag->outer.filter.vlanId <= OMCI_TREATMENT_VLANID_MAX)
            vlanCtl_cmdSetTagVid(pExtVlanTag->outer.filter.vlanId, tagIndex);

        // filter tpid_de ==> treatment tpid_de
        // outer tpid_de is ethertype
        switch (pExtVlanTag->outer.filter.tpid_de)
        {
            case OMCI_FILTER_TPID_8100_DE_X:
                vlanCtl_cmdSetEtherType(C_TAG_TPID);
                break;
            case OMCI_FILTER_TPID_INPUT_DE_X:
                vlanCtl_cmdSetEtherType(inputTpid);
                break;
            case OMCI_FILTER_TPID_INPUT_DE_0:
                vlanCtl_cmdSetEtherType(inputTpid);
                vlanCtl_cmdSetTagCfi(0, tagIndex);
                break;
            case OMCI_FILTER_TPID_INPUT_DE_1:
                vlanCtl_cmdSetEtherType(inputTpid);
                vlanCtl_cmdSetTagCfi(1, tagIndex);
                break;
            default:
                break;
        }
        // inner tpid_de is outer tag ethertype
        switch (pExtVlanTag->inner.filter.tpid_de)
        {
            case OMCI_FILTER_TPID_8100_DE_X:
                vlanCtl_cmdSetTagEtherType(C_TAG_TPID, tagIndex);
                break;
            case OMCI_FILTER_TPID_INPUT_DE_X:
                vlanCtl_cmdSetTagEtherType(inputTpid, tagIndex);
                break;
            case OMCI_FILTER_TPID_INPUT_DE_0:
                vlanCtl_cmdSetTagEtherType(inputTpid, tagIndex);
                vlanCtl_cmdSetTagCfi(0, 1);
                break;
            case OMCI_FILTER_TPID_INPUT_DE_1:
                vlanCtl_cmdSetTagEtherType(inputTpid, tagIndex);
                vlanCtl_cmdSetTagCfi(1, 1);
                break;
            default:
                break;
        }

        // set treatment tagIndex to inner tag
        tagIndex = 1;

        // Set pbits in tag number 1, which is always the inner tag of the frame
        if (pExtVlanTag->inner.filter.prio <= OMCI_TREATMENT_PRIO_MAX)
            vlanCtl_cmdSetTagPbits(pExtVlanTag->inner.filter.prio, tagIndex);
        // Set vid in tag number 1, which is always the inner tag of the frame
        if (pExtVlanTag->inner.filter.vlanId <= OMCI_TREATMENT_VLANID_MAX)
            vlanCtl_cmdSetTagVid(pExtVlanTag->inner.filter.vlanId, tagIndex);

        // filter ethertype ==> treatment ethertype
        // ethertype is inner tag ethertype
        switch (pExtVlanTag->etherType)
        {
            case OMCI_FILTER_ETHER_IPOE:
                vlanCtl_cmdSetTagEtherType(OMCI_FILTER_ETHER_0800, tagIndex);
                break;
            case OMCI_FILTER_ETHER_PPPOE:
                vlanCtl_cmdSetTagEtherType(OMCI_FILTER_ETHER_8863, tagIndex);
                break;
            case OMCI_FILTER_ETHER_ARP:
                vlanCtl_cmdSetTagEtherType(OMCI_FILTER_ETHER_0806, tagIndex);
                break;
            default:
                break;
        }
    }
    else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD  &&
             pExtVlanTag->inner.filter.prio != OMCI_FILTER_IGNORE)
    {
        // set treatment tagIndex to outer tag
        tagIndex = 0;

        if (pExtVlanTag->filterType == OMCI_FILTER_TYPE_DOUBLE_TAG)
        {
            // G.988 suggests that if rule has inner treatment without outer treatment
            // then for RX direction, inner treatment is applied as outer filter, and
            // for TX direction, outer filter is applied as outer treatment.

            // Set pbits in tag number 0, which is always the outer tag of the frame
            if (pExtVlanTag->outer.filter.prio <= OMCI_TREATMENT_PRIO_MAX)
                vlanCtl_cmdSetTagPbits(pExtVlanTag->outer.filter.prio, tagIndex);

            // Set vid in tag number 0, which is always the outer tag of the frame
            if (pExtVlanTag->outer.filter.vlanId <= OMCI_TREATMENT_VLANID_MAX)
                vlanCtl_cmdSetTagVid(pExtVlanTag->outer.filter.vlanId, tagIndex);

            // filter tpid_de ==> treatment tpid_de
            // inner tpid_de is ethertype
            switch (pExtVlanTag->outer.filter.tpid_de)
            {
                case OMCI_FILTER_TPID_8100_DE_X:
                    vlanCtl_cmdSetEtherType(C_TAG_TPID);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_X:
                    vlanCtl_cmdSetEtherType(inputTpid);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_0:
                    vlanCtl_cmdSetEtherType(inputTpid);
                    vlanCtl_cmdSetTagCfi(0, tagIndex);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_1:
                    vlanCtl_cmdSetEtherType(inputTpid);
                    vlanCtl_cmdSetTagCfi(1, tagIndex);
                    break;
                default:
                    break;
            }
        }
        else
        {
            // Set pbits in tag number 0, which is always the outer tag of the frame
            if (pExtVlanTag->inner.filter.prio <= OMCI_TREATMENT_PRIO_MAX)
                vlanCtl_cmdSetTagPbits(pExtVlanTag->inner.filter.prio, tagIndex);

            // Set vid in tag number 0, which is always the outer tag of the frame
            if (pExtVlanTag->inner.filter.vlanId <= OMCI_TREATMENT_VLANID_MAX)
                vlanCtl_cmdSetTagVid(pExtVlanTag->inner.filter.vlanId, tagIndex);

            // filter tpid_de ==> treatment tpid_de
            // inner tpid_de is ethertype
            switch (pExtVlanTag->inner.filter.tpid_de)
            {
                case OMCI_FILTER_TPID_8100_DE_X:
                    vlanCtl_cmdSetEtherType(C_TAG_TPID);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_X:
                    vlanCtl_cmdSetEtherType(inputTpid);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_0:
                    vlanCtl_cmdSetEtherType(inputTpid);
                    vlanCtl_cmdSetTagCfi(0, tagIndex);
                    break;
                case OMCI_FILTER_TPID_INPUT_DE_1:
                    vlanCtl_cmdSetEtherType(inputTpid);
                    vlanCtl_cmdSetTagCfi(1, tagIndex);
                    break;
                default:
                    break;
            }
        }
    }

    // figure out TX filter type
    if (pExtVlanTag->filterType == OMCI_FILTER_TYPE_DOUBLE_TAG)
    {
        if (pExtVlanTag->removeType == OMCI_TREATMENT_REMOVE_BOTH)
        {
            if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_DOUBLE_TAG;
            else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_SINGLE_TAG;
            else
                txFilterType = OMCI_FILTER_TYPE_UNTAG;
        }
        else if (pExtVlanTag->removeType == OMCI_TREATMENT_REMOVE_OUTER)
        {
            if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_DOUBLE_TAG;
            else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_DOUBLE_TAG;
            else
                txFilterType = OMCI_FILTER_TYPE_SINGLE_TAG;
        }
        else
        {
            txFilterType = OMCI_FILTER_TYPE_DOUBLE_TAG;
        }
    }
    else if (pExtVlanTag->filterType == OMCI_FILTER_TYPE_SINGLE_TAG)
    {
        if (pExtVlanTag->removeType == OMCI_TREATMENT_REMOVE_BOTH)
        {
            if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_SINGLE_TAG;
            else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_UNTAG;
            else
                txFilterType = OMCI_FILTER_TYPE_UNTAG;
        }
        else if (pExtVlanTag->removeType == OMCI_TREATMENT_REMOVE_OUTER)
        {
            if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_DOUBLE_TAG;
            else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_SINGLE_TAG;
            else
                txFilterType = OMCI_FILTER_TYPE_UNTAG;
        }
        else
        {
            if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_DOUBLE_TAG;
            else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_DOUBLE_TAG;
            else
                txFilterType = OMCI_FILTER_TYPE_SINGLE_TAG;
        }
    }
    else
    {
        if (pExtVlanTag->removeType == OMCI_TREATMENT_REMOVE_BOTH)
        {
            txFilterType = OMCI_FILTER_TYPE_UNTAG;
        }
        else if (pExtVlanTag->removeType == OMCI_TREATMENT_REMOVE_OUTER)
        {
            if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_SINGLE_TAG;
            else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_UNTAG;
            else
                txFilterType = OMCI_FILTER_TYPE_UNTAG;
        }
        else
        {
            if (pExtVlanTag->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD &&
                pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_DOUBLE_TAG;
            else if (pExtVlanTag->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                txFilterType = OMCI_FILTER_TYPE_SINGLE_TAG;
            else
                txFilterType = OMCI_FILTER_TYPE_UNTAG;
        }
    }

    // insert rule to the table that is determined by TX filter type
    if (txFilterType == OMCI_FILTER_TYPE_DOUBLE_TAG)
    {
        // insert tag rule to double-tagged rule table
        vlanCtl_insertTagRule(devInterface, direction,
                              2, VLANCTL_POSITION_APPEND,
                              VLANCTL_DONT_CARE, &tagRuleId);
        omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 2, tagRuleId);
    }
    else if (txFilterType == OMCI_FILTER_TYPE_SINGLE_TAG)
    {
        // insert tag rule to singe-tagged rule table
        vlanCtl_insertTagRule(devInterface, direction,
                              1, VLANCTL_POSITION_APPEND,
                              VLANCTL_DONT_CARE, &tagRuleId);
        omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 1, tagRuleId);
    }
    else
    {
        // insert tag rule to un-tagged rule table
        vlanCtl_insertTagRule(devInterface, direction,
                              0, VLANCTL_POSITION_APPEND,
                              VLANCTL_DONT_CARE, &tagRuleId);
        omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 0, tagRuleId);
    }
}

static void getUpstreamDirectionFromUniOidNoFilter
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     vlanCtl_direction_t *pDir,
     char *vlanInterface __attribute__((unused)))
{
    // for normal UNI (before bridge), upstream is RX direction
    if (uniOid != MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT &&
        uniOid != MDMOID_IP_HOST_CONFIG_DATA &&
        uniOid != MDMOID_IPV6_HOST_CONFIG_DATA &&
        !rutGpon_isVeipPptpUni(uniOid, uniMeId))
    {
        *pDir = VLANCTL_DIRECTION_RX;
    }
    // for VEIP or IpHost (after bridge), upstream is TX direction
    else
    {
        *pDir = VLANCTL_DIRECTION_TX;
    }
}

static void getUpstreamDirectionFromUniOid
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     vlanCtl_direction_t *pDir,
     char *vlanInterface)
{
    getUpstreamDirectionFromUniOidNoFilter(uniOid, uniMeId, pDir,
      vlanInterface);

    if (*pDir == VLANCTL_DIRECTION_RX)
    {
        // RX direction is from real device (devInterface)
        // to virtual device (vlanInterface)
        // Forward frame to this vlanInterface
        vlanCtl_setReceiveVlanDevice(vlanInterface);
    }
    else
    {
        // TX direction is from virtual device (vlanInterface)
        // to real device (devInterface)
        // apply filter on vlanInterface
        vlanCtl_filterOnTxVlanDevice(vlanInterface);
    }
}

static void getDownstreamDirectionFromUniOid
    (const UINT16 uniOid,
     const UINT16 uniMeId,
     vlanCtl_direction_t *pDir,
     char *vlanInterface)
{
    // for normal UNI (before bridge), downstream is TX direction
    if (uniOid != MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT &&
        uniOid != MDMOID_IP_HOST_CONFIG_DATA &&
        uniOid != MDMOID_IPV6_HOST_CONFIG_DATA &&
        !rutGpon_isVeipPptpUni(uniOid, uniMeId))
    {
        // TX direction is from virtual device (vlanInterface)
        // to real device (devInterface)
        // apply filter on vlanInterface
        vlanCtl_filterOnTxVlanDevice(vlanInterface);
        *pDir = VLANCTL_DIRECTION_TX;
    }
    // for VEIP or IpHost (after bridge), downstream is RX direction
    else
    {
        // RX direction is from real device (devInterface)
        // to virtual device (vlanInterface)
        // Forward frame to this vlanInterface
        vlanCtl_setReceiveVlanDevice(vlanInterface);
        *pDir = VLANCTL_DIRECTION_RX;
    }
}

static void insertVlanTagRules
    (const OmciVlanTagOperInfo_t *pTagOperInfo,
     const UINT16 uniOid,
     const UINT16 uniMeId,
     const OmciPhyInfo_t *phyInfo,
     char  *devInterface,
     char  *vlanInterface)
{
    UINT32 tagIndex = 0;
    UINT32 tagRuleId = VLANCTL_DONT_CARE;
    vlanCtl_direction_t direction = VLANCTL_DIRECTION_TX;

    // RX direction is from real device (devInterface)
    // to virtual device (vlanInterface)

    // since VLAN operation configuration data should
    // be always attached to MacBridgePortConfigurationDataObject
    // at the UNI side, upstream is RX direction
    switch (pTagOperInfo->upstreamMode)
    {
        case OMCI_VLAN_UPSTREAM_TAG:
            // create tag rule for un-tagged frame
            vlanCtl_initTagRule();
            getUpstreamDirectionFromUniOid(uniOid, uniMeId, &direction, vlanInterface);
             /* insert the outer tag */
            vlanCtl_cmdPushVlanTag();
            // Set pbits in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagPbits(pTagOperInfo->upstreamTci.pbits, tagIndex);
            // Set cfi in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagCfi(pTagOperInfo->upstreamTci.cfi, tagIndex);
            // Set vid in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagVid(pTagOperInfo->upstreamTci.vlanId, tagIndex);
            // insert tag rule to un-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  0, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 0, tagRuleId);

            // create tag rule for single-tagged and double-tagged frame
            vlanCtl_initTagRule();
            // Forward frame to this vlanInterface
            vlanCtl_setReceiveVlanDevice(vlanInterface);
            // Set pbits in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagPbits(pTagOperInfo->upstreamTci.pbits, tagIndex);
            // Set cfi in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagCfi(pTagOperInfo->upstreamTci.cfi, tagIndex);
            // Set vid in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagVid(pTagOperInfo->upstreamTci.vlanId, tagIndex);
            // insert tag rule to single-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  1, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 1, tagRuleId);
            // insert tag rule to double-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  2, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 2, tagRuleId);
            break;
        case OMCI_VLAN_UPSTREAM_PREPEND:
            // create tag rule for un-tagged and single-tagged frames
            vlanCtl_initTagRule();
            getUpstreamDirectionFromUniOid(uniOid, uniMeId, &direction, vlanInterface);
             /* insert the outer tag */
            vlanCtl_cmdPushVlanTag();
            // Set pbits in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagPbits(pTagOperInfo->upstreamTci.pbits, tagIndex);
            // Set cfi in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagCfi(pTagOperInfo->upstreamTci.cfi, tagIndex);
            // Set vid in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagVid(pTagOperInfo->upstreamTci.vlanId, tagIndex);
            // insert tag rule to un-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  0, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 0, tagRuleId);
            // insert tag rule to single-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  1, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 1, tagRuleId);


            // create tag rule for double-tagged frame
            vlanCtl_initTagRule();
            // Set pbits in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagPbits(pTagOperInfo->upstreamTci.pbits, tagIndex);
            // Set cfi in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagCfi(pTagOperInfo->upstreamTci.cfi, tagIndex);
            // Set vid in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagVid(pTagOperInfo->upstreamTci.vlanId, tagIndex);
            // insert tag rule to double-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  2, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 2, tagRuleId);
            break;
        default :
            getUpstreamDirectionFromUniOid(uniOid, uniMeId, &direction, vlanInterface);
            // On upstream, accept
            insertDefaultTagRules(devInterface, vlanInterface, phyInfo,
                                  direction, VLANCTL_ACTION_ACCEPT);
            break;
    }

    // since VLAN operation configuration data should
    // be always attached to MacBridgePortConfigurationDataObject
    // at the UNI side, downstream is TX direction
    switch (pTagOperInfo->downstreamMode)
    {
        case OMCI_VLAN_DOWNSTREAM_STRIP:
            // create tag rule for single-tagged and double-tagged frame
            vlanCtl_initTagRule();
            getDownstreamDirectionFromUniOid(uniOid, uniMeId, &direction, vlanInterface);
            // strip the outer tag
            vlanCtl_cmdPopVlanTag();
            // insert tag rule to single-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  1, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 1, tagRuleId);
            // insert tag rule to double-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  2, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 2, tagRuleId);
            break;
        default :
            getDownstreamDirectionFromUniOid(uniOid, uniMeId, &direction, vlanInterface);
            // On downstream, accept
            insertDefaultTagRules(devInterface, vlanInterface, phyInfo,
                                  direction, VLANCTL_ACTION_ACCEPT);
            break;
    }
}

static void insertUniUsIgmpTagRules
    (const UINT8 usMcastType,
     const UINT16 usMcastTci,
     const UINT32 uniOid,
     const UINT32 uniMeId,
     const OmciPhyInfo_t *phyInfo,
     char  *devInterface,
     char  *vlanInterface)
{
    UINT8 pbits = ((usMcastTci & 0xE000) >> 13);
    UINT8 cfi = ((usMcastTci & 0x1000) >> 12);
    UINT16 vlanId = (usMcastTci & 0x0FFF);
    UINT32 tagIndex = 0;
    UINT32 tagRuleId = VLANCTL_DONT_CARE;
    vlanCtl_direction_t direction = VLANCTL_DIRECTION_RX;

    vlanCtl_init();
    vlanCtl_initTagRule();

    getUpstreamDirectionFromUniOid(uniOid, uniMeId, &direction, vlanInterface);

    switch (usMcastType)
    {
        case OMCI_MCAST_US_TAG_ADD:
            vlanCtl_filterOnIpProto(IPPROTO_IGMP);
             /* insert the outer tag */
            vlanCtl_cmdPushVlanTag();
            // Set pbits in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagPbits(pbits, tagIndex);
            // Set cfi in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagCfi(cfi, tagIndex);
            // Set vid in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagVid(vlanId, tagIndex);
            // insert tag rule to un-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  0, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 0, tagRuleId);
            // insert tag rule to single-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  1, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 1, tagRuleId);
            break;
        case OMCI_MCAST_US_TAG_REPLACE_TCI:
            vlanCtl_filterOnIpProto(IPPROTO_IGMP);
            // Set pbits in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagPbits(pbits, tagIndex);
            // Set cfi in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagCfi(cfi, tagIndex);
            // Set vid in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagVid(vlanId, tagIndex);
            // insert tag rule to single-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  1, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 1, tagRuleId);
            // insert tag rule to double-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  2, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 2, tagRuleId);
            break;
        case OMCI_MCAST_US_TAG_REPLACE_VLAN:
            vlanCtl_filterOnIpProto(IPPROTO_IGMP);
            // Set vid in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagVid(vlanId, tagIndex);
            // insert tag rule to single-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  1, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 1, tagRuleId);
            // insert tag rule to double-tagged rule table
            vlanCtl_insertTagRule(devInterface, direction,
                                  2, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 2, tagRuleId);
            break;
        default:
            break;
    }

    vlanCtl_cleanup();

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> insertUniUsIgmpTagRules: usMcastType=%d, usMcastTci=%d, direction=%d, phyId=%d, devIfc=%s, vlanIfc=%s\n",
                   usMcastType, usMcastTci, direction, phyInfo->phyId, devInterface, vlanInterface);
}

static UBOOL8 isDefaultFilterRule
    (const OmciExtVlanTagOper_t *pExtVlanTag)
{
    UBOOL8 ret = FALSE;

    if ((pExtVlanTag->outer.filter.prio == 0x0F) &&
      (pExtVlanTag->inner.filter.prio == 0x0F) &&
      (pExtVlanTag->etherType == OMCI_FILTER_ETHER_NONE))
    {
        ret = TRUE;
    }
    else if ((pExtVlanTag->outer.filter.prio == 0x0F) &&
      (pExtVlanTag->inner.filter.prio == 0x0E))
    {
        ret = TRUE;
    }
    else if ((pExtVlanTag->outer.filter.prio == 0x0E) &&
      (pExtVlanTag->inner.filter.prio == 0x0E))
    {
        ret = TRUE;
    }

    return ret;
}

static UBOOL8 isDefaultTagRule
    (const OmciExtVlanTagOper_t *pExtVlanTag)
{
    UBOOL8 ret = FALSE;

    if ((pExtVlanTag->removeType == OMCI_TREATMENT_REMOVE_NONE) &&
        (pExtVlanTag->outer.treatment.prio == 0x0F) &&
        (pExtVlanTag->inner.treatment.prio == 0x0F))
    {
        if (isDefaultFilterRule(pExtVlanTag) == TRUE)
        {
            ret = TRUE;
        }
    }

    return ret;
}

static UBOOL8 setDscpToPbitsTable
    (const UINT8 *dscpToPbit,
     char  *devInterface)
{
    UINT32 i = 0;
    UBOOL8 isValidDscpToPbits = TRUE;

    for (i = 0; i < OMCI_DSCP_NUM_MAX; i++)
    {
        if (dscpToPbit[i] >= OMCI_FILTER_PRIO_NONE)
        {
            isValidDscpToPbits = FALSE;
            break;
        }
    }

    if (isValidDscpToPbits == TRUE)
    {
        vlanCtl_init();
        for (i = 0; i < OMCI_DSCP_NUM_MAX; i++)
            vlanCtl_setDscpToPbits(devInterface, i, dscpToPbit[i]);
        vlanCtl_cleanup();
    }

    return isValidDscpToPbits;
}

static UBOOL8 setUniDscpToPbitsTable
    (const GemPortNetworkCtpObject *ctp,
     const UINT32 uniOid,
     const UINT32 uniMeId,
     char  *devInterface)
{
    UBOOL8 isValidDscpToPbits = FALSE;
    OmciDscpPbitMappingInfo_t dscpToPbitInfo;

    memset(&dscpToPbitInfo, 0, sizeof(OmciDscpPbitMappingInfo_t));

    // get DscpToPbit from 802.1p mapper service profile first
    if (rutGpon_getDscpToPbitFromGemPortNeworkCtp
            (ctp, &dscpToPbitInfo) == CMSRET_SUCCESS)
    {
        // setup DSCP to Pbit mapping table
        isValidDscpToPbits = setDscpToPbitsTable
            (dscpToPbitInfo.dscpToPbit, devInterface);
    }

    // if DscpToPbit info in 802.1p mapper service profile is
    // NOT valid then get DscpToPbit info in extended vlan
    if (isValidDscpToPbits == FALSE)
    {
        memset(&dscpToPbitInfo, 0, sizeof(OmciDscpPbitMappingInfo_t));

        if (rutGpon_getDscpToPbitFromUniMeId
                (uniOid, uniMeId, &dscpToPbitInfo) == CMSRET_SUCCESS)
        {
            // setup DSCP to Pbit mapping table
            isValidDscpToPbits = setDscpToPbitsTable
                (dscpToPbitInfo.dscpToPbit, devInterface);
        }
    }

    return isValidDscpToPbits;
}

static void setTpidTable
    (char *devInterface,
     const UINT16 tpid1,
     const UINT16 tpid2,
     const UINT16 tpid3)
{
    UINT32 i = 0;
    unsigned int tpidTable[BCM_VLAN_MAX_TPID_VALUES];

    // setup TPID table
    for (i = 0; i < BCM_VLAN_MAX_TPID_VALUES; i++)
        tpidTable[i] = Q_TAG_TPID;

    tpidTable[BCM_VLAN_MAX_TPID_VALUES - 3] = tpid1;
    tpidTable[BCM_VLAN_MAX_TPID_VALUES - 2] = tpid2;
    tpidTable[BCM_VLAN_MAX_TPID_VALUES - 1] = tpid3;

    vlanCtl_init();
    vlanCtl_setTpidTable(devInterface, tpidTable);
    vlanCtl_cleanup();
}

static void setTpidTableWithExtVlanTpid
    (char *devInterface,
     const UINT16 inputTpid,
     const UINT16 outputTpid)
{
    UINT16 newTpid = C_TAG_TPID;

    if (isNewTpid(outputTpid))
    {
        newTpid = outputTpid;
    }
    else if (isNewTpid(inputTpid))
    {
        newTpid = inputTpid;
    }

    setTpidTable(devInterface, S_TAG_TPID, D_TAG_TPID, newTpid);
}

static void insertUniTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32 uniOid,
     const UINT32 uniMeId,
     const OmciPhyInfo_t *phyInfo,
     char  *devInterface,
     char  *vlanInterface)
{
    UINT8 usMcastType = 0;
    UINT16 usMcastTci = 0;
    OmciExtVlanTagOperInfo_t extTagOperInfo;
    OmciVlanTagOperInfo_t tagOperInfo;
    UINT32 ignoreEntry = 0;

    memset(&extTagOperInfo, 0, sizeof(OmciExtVlanTagOperInfo_t));
    memset(&tagOperInfo, 0, sizeof(OmciVlanTagOperInfo_t));

    // create upstream IGMP rules before ExtVlan rules
    // so that IGMP rules has higher priority
    if ((rutGpon_getUsMcastTciFromUniMeId(uniOid,
                                          uniMeId,
                                          &usMcastType,
                                          &usMcastTci) == CMSRET_SUCCESS) &&
        (usMcastType != OMCI_MCAST_US_TAG_NONE))
    {
        // create upstream IGMP traffic rules
        insertUniUsIgmpTagRules(usMcastType, usMcastTci,
                                uniOid, uniMeId,
                                phyInfo, devInterface, vlanInterface);
    }

    // setup Extended VLAN tagging rule operation table
    if (rutGpon_allocExtTagOperInfo(uniOid, uniMeId, &extTagOperInfo) != CMSRET_SUCCESS)
        goto out;

    setUniDscpToPbitsTable(ctp, uniOid, uniMeId, devInterface);

    if (extTagOperInfo.numberOfEntries > 0)
    {
        UINT32 i = 0;
        OmciTagAction tagAction;

        setTpidTableWithExtVlanTpid(devInterface, extTagOperInfo.inputTpid,
          extTagOperInfo.outputTpid);

        vlanCtl_init();

        for (i = 0; i < extTagOperInfo.numberOfEntries; i++)
        {
            // Do nothing if remove type is to discard frame
            if (extTagOperInfo.pTagOperTbl[i].removeType ==
              OMCI_TREATMENT_DISCARD_FRAME)
            {
                ignoreEntry++;
                continue;
            }

            if (isDefaultTagRule(&(extTagOperInfo.pTagOperTbl[i])) == TRUE)
            {
                // Check Extended VLAN ME default tag rules to accept traffic
                // filterType = 0, 1, 2
                if (rutGpon_getExtVlanDefaultRuleMode() == TRUE)
                {
                    // As per G.988, in the downstream direction, the
                    // upstream default rules do not apply
                    vlanCtl_direction_t usDir;

                    getUpstreamDirectionFromUniOidNoFilter(uniOid, uniMeId, 
                      &usDir, vlanInterface);
                    vlanCtl_setDefaultAction(devInterface, usDir,
                      extTagOperInfo.pTagOperTbl[i].filterType,
                      VLANCTL_ACTION_ACCEPT, vlanInterface);
                }
                else
                {
                    ignoreEntry++;
                }
                continue;
            }

            tagAction = getExtVlanTagAction(&(extTagOperInfo.pTagOperTbl[i]));
            insertExtVlanUsTagRules(uniOid, uniMeId,
                                    extTagOperInfo.inputTpid,
                                    extTagOperInfo.outputTpid,
                                    tagAction,
                                    &(extTagOperInfo.pTagOperTbl[i]),
                                    phyInfo, devInterface, vlanInterface);
            /*
             * There are two different interpretations to the downstream action of
             * a default upstream rule defined in ITU-T G.988 Extended VLAN ME.
             * For example, if the default rule in the upstream direction is
             * "filter untagged, add VID 10", then the possible VLAN action in the
             * downstream direction can be:
             * - Interpretation #1: to pass through.
             * - Interpretation #2: (current OMCI stack implementation) to POP VLAN.
             */
#ifndef DS_BYPASS_ON_DEFAULT_USRULES
            if (extTagOperInfo.downstreamMode == OMCI_XVLAN_DOWNSTREAM_INVERSE)
#else
            if ((extTagOperInfo.downstreamMode == OMCI_XVLAN_DOWNSTREAM_INVERSE) &&
              (isDefaultFilterRule(&(extTagOperInfo.pTagOperTbl[i])) != TRUE))
#endif
                insertExtVlanDsTagRules(uniOid, uniMeId,
                                        extTagOperInfo.inputTpid,
                                        extTagOperInfo.outputTpid,
                                        tagAction,
                                        &(extTagOperInfo.pTagOperTbl[i]),
                                        phyInfo, devInterface, vlanInterface);

            // when etherType is pppoe, we need to fiter 0x8863 and 0x8864
            if (extTagOperInfo.pTagOperTbl[i].etherType == OMCI_FILTER_ETHER_PPPOE)
            {
                extTagOperInfo.pTagOperTbl[i].etherType = OMCI_FILTER_ETHER_PPPOE_2;
                insertExtVlanUsTagRules(uniOid, uniMeId,
                                        extTagOperInfo.inputTpid,
                                        extTagOperInfo.outputTpid,
                                        tagAction,
                                        &(extTagOperInfo.pTagOperTbl[i]),
                                        phyInfo, devInterface, vlanInterface);
                if (extTagOperInfo.downstreamMode == OMCI_XVLAN_DOWNSTREAM_INVERSE)
                    insertExtVlanDsTagRules(uniOid, uniMeId,
                                            extTagOperInfo.inputTpid,
                                            extTagOperInfo.outputTpid,
                                            tagAction,
                                            &(extTagOperInfo.pTagOperTbl[i]),
                                            phyInfo, devInterface, vlanInterface);
            }
        }

        vlanCtl_cleanup();

#if 0
        // Extended VLAN ME should have default rules to
        // accept traffic for tag table that have no rules
        // On tx, accept
        insertDefaultTagRules(devInterface, vlanInterface, phyId,
                              VLANCTL_DIRECTION_TX, VLANCTL_ACTION_ACCEPT);
        // On rx, accept
        insertDefaultTagRules(devInterface, vlanInterface, phyId,
                              VLANCTL_DIRECTION_RX, VLANCTL_ACTION_ACCEPT);
#endif
    }

    // retrieve VLAN tagging operation configuration data if any
    rutGpon_getTagOperInfo(uniOid, uniMeId, &tagOperInfo);

    if (tagOperInfo.tagIsExisted == TRUE)
    {
        vlanCtl_init();
        insertVlanTagRules(&tagOperInfo, uniOid, uniMeId, phyInfo, devInterface, vlanInterface);
        vlanCtl_cleanup();
    }

    // Completed path without VLAN MEs should have default rules to accept traffic.
    if ((extTagOperInfo.numberOfEntries == 0 || ignoreEntry == extTagOperInfo.numberOfEntries) &&
        tagOperInfo.tagIsExisted == FALSE)
    {
#if 0
        // On tx, accept
        insertDefaultTagRules(devInterface, vlanInterface, phyId,
                              VLANCTL_DIRECTION_TX, VLANCTL_ACTION_ACCEPT);
        // On rx, accept
        insertDefaultTagRules(devInterface, vlanInterface, phyId,
                              VLANCTL_DIRECTION_RX, VLANCTL_ACTION_ACCEPT);
#else  // fix me, it does not handle UNI port on multiple bridge service
       int i;
       UINT32 tagRuleId = VLANCTL_DONT_CARE;

       // Accept all on tx/rx
       vlanCtl_init();

       for (i = 0; i < BCM_VLAN_MAX_TAGS; ++i)
       {
           vlanCtl_initTagRule();
           vlanCtl_setReceiveVlanDevice(vlanInterface);
           vlanCtl_insertTagRule(devInterface, VLANCTL_DIRECTION_RX,
                                 i, VLANCTL_POSITION_APPEND,
                                 VLANCTL_DONT_CARE, &tagRuleId);
           omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, VLANCTL_DIRECTION_RX, i, tagRuleId);

           vlanCtl_initTagRule();
           vlanCtl_filterOnTxVlanDevice(vlanInterface);
           vlanCtl_insertTagRule(devInterface, VLANCTL_DIRECTION_TX,
                                 i, VLANCTL_POSITION_APPEND,
                                 VLANCTL_DONT_CARE, &tagRuleId);
           omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, VLANCTL_DIRECTION_TX, i, tagRuleId);
       }

       vlanCtl_cleanup();

#endif
    }

out:
    if (extTagOperInfo.numberOfEntries > 0 && extTagOperInfo.pTagOperTbl != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(extTagOperInfo.pTagOperTbl);

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> insertUniTagRules, phyId=%d, devIfc=%s, vlanIfc=%s, numberOfEntries=%d, tagExisted=%d\n",
                   phyInfo->phyId, devInterface, vlanInterface, extTagOperInfo.numberOfEntries, tagOperInfo.tagIsExisted);
}

static void insertTciFilterTagRules
    (const UINT8  pbits,
     const UINT8  cfi,
     const UINT16 vlanId,
     const vlanCtl_defaultAction_t action,
     const vlanCtl_direction_t direction,
     const OmciPhyInfo_t *phyInfo,
     char  *devInterface,
     char  *vlanInterface)
{
    UINT16 qid = 0;
    UINT32 tagRuleId = VLANCTL_DONT_CARE;

    // On tx, filter with the given TCI
    vlanCtl_initTagRule();
    if (direction == VLANCTL_DIRECTION_TX)
        // apply filter on vlanInterface
        vlanCtl_filterOnTxVlanDevice(vlanInterface);
    else if (direction == VLANCTL_DIRECTION_RX)
    {
        // Forward frame to this vlanInterface
        vlanCtl_setReceiveVlanDevice(vlanInterface);
        // filter on gem port index
        vlanCtl_filterOnSkbMarkPort(phyInfo->phyId);
    }

    // use Pbits as filter
    if (pbits != (UINT8)OMCI_DONT_CARE)
    {
        vlanCtl_filterOnTagPbits(pbits, 0);
    }

    // use CFI as filter
    if (cfi != (UINT8)OMCI_DONT_CARE)
    {
        vlanCtl_filterOnTagCfi(cfi, 0);
    }

    // use Vlan ID as filter
    if (vlanId != (UINT16)OMCI_DONT_CARE)
    {
        vlanCtl_filterOnTagVid(vlanId, 0);
    }

    // set gem id for these tag rules
    // only when filter on upstream direction
    if (strstr(devInterface, "gpon") != NULL &&
        direction == VLANCTL_DIRECTION_TX)
    {
        vlanCtl_cmdSetSkbMarkPort(phyInfo->phyId);
        if ((getOnuOmciTmOption() != OMCI_TRAFFIC_MANAGEMENT_RATE) &&
          (rutGpon_getQidFromGemPortIndex(phyInfo->phyId, OMCI_FLOW_UPSTREAM, &qid)
          == CMSRET_SUCCESS))
        {
           rutwrap_vlanCtl_cmdSetSkbMarkQueue(qid);
        }
    }

    if (direction == VLANCTL_DIRECTION_RX)
    {
        // Set downstream multicast priority queue if phyId
        // is gem port index of multicast gem port network ctp
        // and this gem port network ctp has configured
        // with downstream priority queue
        if (rutGpon_getQidFromGemPortIndex(phyInfo->phyId, OMCI_FLOW_DOWNSTREAM, &qid) == CMSRET_SUCCESS)
        {
            rutwrap_vlanCtl_cmdSetSkbMarkQueue(qid);
        }
        else
        {
            rutwrap_setDsSkbMarkQueue(pbits, FALSE);
        }
    }

    // drop any frames if action is drop
    if (action == VLANCTL_ACTION_DROP)
        vlanCtl_cmdDropFrame();

    // Set rule to the single-tagged and double-tagged tx tables
    vlanCtl_insertTagRule(devInterface, direction,
                          1, VLANCTL_POSITION_APPEND,
                          VLANCTL_DONT_CARE, &tagRuleId);
    omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 1, tagRuleId);
    vlanCtl_insertTagRule(devInterface, direction,
                          2, VLANCTL_POSITION_APPEND,
                          VLANCTL_DONT_CARE, &tagRuleId);
    omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 2, tagRuleId);
}

static void insertPbitsFilterTagRules
    (const UINT8  pbits,
     const vlanCtl_defaultAction_t action,
     const vlanCtl_direction_t direction,
     const OmciPhyInfo_t *phyInfo,
     char  *devInterface,
     char  *vlanInterface)
{
    UINT16 qid = 0;
    UINT32 tagRuleId = VLANCTL_DONT_CARE;

    // On tx, filter with the given Pbits
    vlanCtl_initTagRule();
    if (direction == VLANCTL_DIRECTION_TX)
        // apply filter on vlanInterface
        vlanCtl_filterOnTxVlanDevice(vlanInterface);
    else if (direction == VLANCTL_DIRECTION_RX)
    {
        // Forward frame to this vlanInterface
        vlanCtl_setReceiveVlanDevice(vlanInterface);
        // filter on gem port index
        vlanCtl_filterOnSkbMarkPort(phyInfo->phyId);
    }

    // use Pbits as filter
    vlanCtl_filterOnTagPbits(pbits, 0);

    // set gem id for these tag rules
    // only when filter on upstream direction
    if (strstr(devInterface, "gpon") != NULL &&
        direction == VLANCTL_DIRECTION_TX)
    {
        vlanCtl_cmdSetSkbMarkPort(phyInfo->phyId);
        if ((getOnuOmciTmOption() != OMCI_TRAFFIC_MANAGEMENT_RATE) &&
          (rutGpon_getQidFromGemPortIndex(phyInfo->phyId, OMCI_FLOW_UPSTREAM, &qid)
          == CMSRET_SUCCESS))
        {
           rutwrap_vlanCtl_cmdSetSkbMarkQueue(qid);
        }
    }
    if (direction == VLANCTL_DIRECTION_RX)
    {
        // Set downstream multicast priority queue if phyId
        // is gem port index of multicast gem port network ctp
        // and this gem port network ctp has configured
        // with downstream priority queue
        if (rutGpon_getQidFromGemPortIndex(phyInfo->phyId, OMCI_FLOW_DOWNSTREAM, &qid) == CMSRET_SUCCESS)
        {
            rutwrap_vlanCtl_cmdSetSkbMarkQueue(qid);
        }
        else
        {
            rutwrap_setDsSkbMarkQueue(pbits, FALSE);
        }
    }

    // drop any frames if action is drop
    if (action == VLANCTL_ACTION_DROP)
        vlanCtl_cmdDropFrame();

    // Set rule to the single-tagged and double-tagged tx tables
    vlanCtl_insertTagRule(devInterface, direction,
                          1, VLANCTL_POSITION_APPEND,
                          VLANCTL_DONT_CARE, &tagRuleId);
    omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 1, tagRuleId);
    vlanCtl_insertTagRule(devInterface, direction,
                          2, VLANCTL_POSITION_APPEND,
                          VLANCTL_DONT_CARE, &tagRuleId);
    omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 2, tagRuleId);
}

static void insertDscpToPbitsFilterTagRules
    (const UINT8  pbits,
     const vlanCtl_defaultAction_t action,
     const vlanCtl_direction_t direction,
     const OmciPhyInfo_t *phyInfo,
     char  *devInterface,
     char  *vlanInterface)
{
    UINT16 qid = 0;
    UINT32 tagRuleId = VLANCTL_DONT_CARE;

    // On tx, filter with the given Pbits
    vlanCtl_initTagRule();
    if (direction == VLANCTL_DIRECTION_TX)
        // apply filter on vlanInterface
        vlanCtl_filterOnTxVlanDevice(vlanInterface);
    else if (direction == VLANCTL_DIRECTION_RX)
    {
        // Forward frame to this vlanInterface
        vlanCtl_setReceiveVlanDevice(vlanInterface);
        // filter on gem port index
        vlanCtl_filterOnSkbMarkPort(phyInfo->phyId);
    }

    // use Pbits as DSCP to Pbits filter
    vlanCtl_filterOnDscp2Pbits(pbits);

    // set gem id for these tag rules
    // only when filter on upstream direction
    if (strstr(devInterface, "gpon") != NULL &&
        direction == VLANCTL_DIRECTION_TX)
    {
        vlanCtl_cmdSetSkbMarkPort(phyInfo->phyId);
        if ((getOnuOmciTmOption() != OMCI_TRAFFIC_MANAGEMENT_RATE) &&
          (rutGpon_getQidFromGemPortIndex(phyInfo->phyId, OMCI_FLOW_UPSTREAM, &qid)
          == CMSRET_SUCCESS))
        {
           rutwrap_vlanCtl_cmdSetSkbMarkQueue(qid);
        }
    }
    if (direction == VLANCTL_DIRECTION_RX)
    {
        // Set downstream multicast priority queue if phyId
        // is gem port index of multicast gem port network ctp
        // and this gem port network ctp has configured
        // with downstream priority queue
        if (rutGpon_getQidFromGemPortIndex(phyInfo->phyId, OMCI_FLOW_DOWNSTREAM, &qid) == CMSRET_SUCCESS)
        {
            rutwrap_vlanCtl_cmdSetSkbMarkQueue(qid);
        }
        else
        {
            rutwrap_setDsSkbMarkQueue(pbits, FALSE);
        }
    }

    // drop any frames if action is drop
    if (action == VLANCTL_ACTION_DROP)
        vlanCtl_cmdDropFrame();

    // Set rule to the non-tag tx tables
    vlanCtl_insertTagRule(devInterface, direction,
                          0, VLANCTL_POSITION_APPEND,
                          VLANCTL_DONT_CARE, &tagRuleId);
    omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 0, tagRuleId);
}

static void insertVlanIdFilterTagRules
    (const UINT16  vlanId,
     const vlanCtl_defaultAction_t action,
     const vlanCtl_direction_t direction,
     const OmciPhyInfo_t *phyInfo,
     char  *devInterface,
     char  *vlanInterface)
{
    UINT32 tagRuleId = VLANCTL_DONT_CARE;
    UINT16 pbitQId = 0;
    UINT16 pbitQIdStart = 0;
    UINT16 pbitQIdEnd = 0;
    UBOOL8 isQFound = FALSE;

    if (direction == VLANCTL_DIRECTION_TX)
    {
        vlanCtl_initTagRule();

        // apply filter on vlanInterface
        vlanCtl_filterOnTxVlanDevice(vlanInterface);
        // use Vlan ID as filter
        vlanCtl_filterOnTagVid(vlanId, 0);
        // set gem id for these tag rules
        // only when filter on upstream direction
        if (strstr(devInterface, "gpon") != NULL)
        {
            vlanCtl_cmdSetSkbMarkPort(phyInfo->phyId);
            if ((getOnuOmciTmOption() != OMCI_TRAFFIC_MANAGEMENT_RATE) &&
              (rutGpon_getQidFromGemPortIndex(phyInfo->phyId, OMCI_FLOW_UPSTREAM, &pbitQId)
              == CMSRET_SUCCESS))
            {
               rutwrap_vlanCtl_cmdSetSkbMarkQueue(pbitQId);
            }
        }

        // drop any frames if action is drop
        if (action == VLANCTL_ACTION_DROP)
        {
            vlanCtl_cmdDropFrame();
        }

        // Set rule to the tx tables
        vlanCtl_insertTagRule(devInterface, direction,
                              1, VLANCTL_POSITION_APPEND,
                              VLANCTL_DONT_CARE, &tagRuleId);
        omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 1, tagRuleId);
        vlanCtl_insertTagRule(devInterface, direction,
                              2, VLANCTL_POSITION_APPEND,
                              VLANCTL_DONT_CARE, &tagRuleId);
        omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 2, tagRuleId);
    }
    else if (direction == VLANCTL_DIRECTION_RX)
    {
        // Set downstream multicast priority queue if phyId
        // is gem port index of multicast gem port network ctp
        // and this gem port network ctp has configured
        // with downstream priority queue

        isQFound = rutGpon_getDsQueueIdRange(phyInfo, &pbitQIdStart, &pbitQIdEnd);
        for (pbitQId = pbitQIdStart; pbitQId <= pbitQIdEnd; pbitQId++)
        {
            vlanCtl_initTagRule();
            // Forward frame to this vlanInterface
            vlanCtl_setReceiveVlanDevice(vlanInterface);
            // filter on gem port index
            vlanCtl_filterOnSkbMarkPort(phyInfo->phyId);
            // use Vlan ID as filter
            vlanCtl_filterOnTagVid(vlanId, 0);

            rutwrap_setDsSkbMarkQueue(pbitQId, isQFound);
            // drop any frames if action is drop
            if (action == VLANCTL_ACTION_DROP)
            {
                vlanCtl_cmdDropFrame();
            }

            // Set rule to the tx tables
            vlanCtl_insertTagRule(devInterface, direction,
                                  1, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 1, tagRuleId);
            vlanCtl_insertTagRule(devInterface, direction,
                                  2, VLANCTL_POSITION_APPEND,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(devInterface, phyInfo->phyId, phyInfo->phyType, direction, 2, tagRuleId);
        }
    }
}

static void insertMapperTagRules
    (const UINT8 *pbits,
     const OmciDscpPbitMappingInfo_t *pDscpToPbitInfo,
     const vlanCtl_direction_t direction,
     const OmciPhyInfo_t *phyInfo,
     char  *devInterface,
     char  *vlanInterface)
{
    UINT32 i = 0;

    if (pbits == NULL)
        return;

    vlanCtl_init();

    for (i = OMCI_FILTER_PRIO_MIN; i <= OMCI_FILTER_PRIO_MAX; i++)
    {
        // On tx, filter with the given Pbits
        if (pbits[i] < OMCI_FILTER_PRIO_NONE)
        {
            insertPbitsFilterTagRules(pbits[i], VLANCTL_ACTION_ACCEPT,
                                      direction, phyInfo, devInterface, vlanInterface);
            // use Pbits as DscpToPbit filter
            if (pDscpToPbitInfo->unmarkedFrameOption == OMCI_UNMARKED_FRAME_DSCP_TO_PBIT)
                insertDscpToPbitsFilterTagRules(pbits[i], VLANCTL_ACTION_ACCEPT,
                                                direction, phyInfo, devInterface, vlanInterface);
        }
    }

    // use defaultPBitMarking as DscpToPbit filter
    if (pDscpToPbitInfo->unmarkedFrameOption == OMCI_UNMARKED_FRAME_DEFAULT_PBIT)
        insertDscpToPbitsFilterTagRules(pDscpToPbitInfo->defaultPBitMarking,
                                        VLANCTL_ACTION_ACCEPT,
                                        direction, phyInfo, devInterface, vlanInterface);

    vlanCtl_cleanup();

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> insertMapperTagRules, pbits=[%d|%d|%d|%d|%d|%d|%d|%d], option=%d, defaultPbit=%d, direction=%d, phyId=%d, devIfc=%s, vlanIfc=%s\n",
                   pbits[0], pbits[1], pbits[2], pbits[3], pbits[4], pbits[5], pbits[6], pbits[7],
                   pDscpToPbitInfo->unmarkedFrameOption, pDscpToPbitInfo->defaultPBitMarking,
                   direction, phyInfo->phyId, devInterface, vlanInterface);
}

static void insertAniMapperTagRules
    (const UINT8 *pbits,
     const OmciDscpPbitMappingInfo_t *pDscpToPbitInfo,
     const OmciPhyInfo_t *phyInfo,
     char  *devInterface,
     char  *vlanInterface)
{
    // On tx, insert pbit filter tag rules
    insertMapperTagRules(pbits, pDscpToPbitInfo, VLANCTL_DIRECTION_TX, phyInfo, devInterface, vlanInterface);

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> insertAniMapperTagRules, pbits=[%d|%d|%d|%d|%d|%d|%d|%d], option=%d, defaultPbit=%d, phyId=%d, devIfc=%s, vlanIfc=%s\n",
                   pbits[0], pbits[1], pbits[2], pbits[3], pbits[4], pbits[5], pbits[6], pbits[7],
                   pDscpToPbitInfo->unmarkedFrameOption, pDscpToPbitInfo->defaultPBitMarking,
                   phyInfo->phyId, devInterface, vlanInterface);
}

static void insertAniFilterTagRules
    (const UINT32 vlanFilterId __attribute__((unused)),
     const OmciVlanTciFilterInfo_t *pTagFilterInfo,
     const vlanCtl_direction_t direction,
     const OmciPhyInfo_t *phyInfo,
     char  *devInterface,
     char  *vlanInterface)
{
    UINT32 i = 0;
    OmciVlanTciFilter_t filters[OMCI_TCI_VLANID_LEN];

    if (pTagFilterInfo == NULL)
        return;

    memset(filters, OMCI_DONT_CARE, sizeof(OmciVlanTciFilter_t) * OMCI_TCI_VLANID_LEN);

    vlanCtl_init();

    switch (pTagFilterInfo->forwardOperation)
    {
        case 0x15:
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 1, phyInfo, direction);
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 2, phyInfo, direction);
            break;
        case 0x0C:
        case 0x0B:
        case 0x14:
        case 0x13:
        case 0x1B:
        case 0x1A:
        case 0x21:
        case 0x20:
            if (pTagFilterInfo->forwardOperation == 0x0B ||
                pTagFilterInfo->forwardOperation == 0x13||
                pTagFilterInfo->forwardOperation == 0x1A ||
                pTagFilterInfo->forwardOperation == 0x20)
                setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            else
                setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 1, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 2, phyInfo, direction);
            for (i = 1; i < pTagFilterInfo->numberOfEntries; i++)
            {
                // On tx, filter with the given TCI
                insertTciFilterTagRules(pTagFilterInfo->pVlanTciTbl[i].pbits,
                                          pTagFilterInfo->pVlanTciTbl[i].cfi,
                                          pTagFilterInfo->pVlanTciTbl[i].vlanId,
                                          VLANCTL_ACTION_ACCEPT, direction,
                                          phyInfo, devInterface, vlanInterface);
            }
            break;
        case 0x08:
        case 0x07:
        case 0x12:
        case 0x11:
        case 0x19:
        case 0x18:
        case 0x1F:
        case 0x1E:
            if (pTagFilterInfo->forwardOperation == 0x07 ||
                pTagFilterInfo->forwardOperation == 0x11 ||
                pTagFilterInfo->forwardOperation == 0x18 ||
                pTagFilterInfo->forwardOperation == 0x1E)
                setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            else
                setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 1, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 2, phyInfo, direction);
            for (i = 1; i < pTagFilterInfo->numberOfEntries; i++)
            {
                // On tx, filter with the given Pbits
                insertPbitsFilterTagRules(pTagFilterInfo->pVlanTciTbl[i].pbits,
                                            VLANCTL_ACTION_ACCEPT, direction,
                                            phyInfo, devInterface, vlanInterface);
            }
            break;
        case 0x04:
        case 0x03:
        case 0x10:
        case 0x0F:
        case 0x17:
        case 0x16:
        case 0x1D:
        case 0x1C:
            if (pTagFilterInfo->forwardOperation == 0x03 ||
                pTagFilterInfo->forwardOperation == 0x0F ||
                pTagFilterInfo->forwardOperation == 0x16 ||
                pTagFilterInfo->forwardOperation == 0x1C)
                setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            else
                setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 1, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 2, phyInfo, direction);
            for (i = 1; i < pTagFilterInfo->numberOfEntries; i++)
            {
                // On tx, filter with the given Vlan ID
                insertVlanIdFilterTagRules(pTagFilterInfo->pVlanTciTbl[i].vlanId,
                                             VLANCTL_ACTION_ACCEPT, direction,
                                             phyInfo, devInterface, vlanInterface);
            }
            break;
        case 0x0E:
        case 0x0D:
            if (pTagFilterInfo->forwardOperation == 0x0D)
                setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            else
                setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            // setup negative filtering by TCI
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 1, phyInfo, direction);
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 2, phyInfo, direction);
            for (i = 1; i < pTagFilterInfo->numberOfEntries; i++)
            {
                // On tx, filter with the given TCI
                insertTciFilterTagRules(pTagFilterInfo->pVlanTciTbl[i].pbits,
                                          pTagFilterInfo->pVlanTciTbl[i].cfi,
                                          pTagFilterInfo->pVlanTciTbl[i].vlanId,
                                          VLANCTL_ACTION_DROP, direction,
                                          phyInfo, devInterface, vlanInterface);
            }
            break;
        case 0x0A:
        case 0x09:
            if (pTagFilterInfo->forwardOperation == 0x09)
                setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            else
                setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            // setup negative filtering by Pbits
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 1, phyInfo, direction);
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 2, phyInfo, direction);
            for (i = 1; i < pTagFilterInfo->numberOfEntries; i++)
            {
                // On tx, filter with the given Pbits
                // drop any frames that match with Pbits filter for this bridge port
                insertPbitsFilterTagRules(pTagFilterInfo->pVlanTciTbl[i].pbits,
                                            VLANCTL_ACTION_DROP, direction,
                                            phyInfo, devInterface, vlanInterface);
            }
            break;
        case 0x06:
        case 0x05:
            if (pTagFilterInfo->forwardOperation == 0x05)
                setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            else
                setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            // setup negative filtering by VlanId
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 1, phyInfo, direction);
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 2, phyInfo, direction);
            for (i = 1; i < pTagFilterInfo->numberOfEntries; i++)
            {
                // On tx, filter with the given Vlan ID
                // drop any frames that match with vlan filter for this bridge port
                insertVlanIdFilterTagRules(pTagFilterInfo->pVlanTciTbl[i].vlanId,
                                             VLANCTL_ACTION_DROP, direction,
                                             phyInfo, devInterface, vlanInterface);
            }
            break;
        case 0x02:
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 1, phyInfo, direction);
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 2, phyInfo, direction);
            break;
        case 0x01:
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 1, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 2, phyInfo, direction);
            break;
        case 0x00:
        default:
            if (direction == VLANCTL_DIRECTION_RX)
            {
                // On rx, default is drop on vlanInterface
                // always filter packets on GEM-Index
                insertDefaultAniTagRules(devInterface, vlanInterface, phyInfo,
                                      direction, VLANCTL_ACTION_DROP);
            }
            else if (direction == VLANCTL_DIRECTION_TX)
            {
                // On tx, setSkbMarkPort to gem id
	            // (phyId) for forwarding traffic to this gem port
                insertDefaultAniTagRules(devInterface, vlanInterface, phyInfo,
                                      direction, VLANCTL_ACTION_ACCEPT);
            }
            break;
    }

    vlanCtl_cleanup();

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> insertAniFilterTagRules: numberOfEntries=%d, forwardOperation=%d, direction=%d, phyId=%d, devIfc=%s, vlanIfc=%s\n",
                   pTagFilterInfo->numberOfEntries, pTagFilterInfo->forwardOperation, direction, phyInfo->phyId, devInterface, vlanInterface);
}

static void insertAniMapperFilterTagRules
    (const UINT8  *pbits,
     const OmciDscpPbitMappingInfo_t *pDscpToPbitInfo,
     const UINT32 vlanFilterId __attribute__((unused)),
     const OmciVlanTciFilterInfo_t *pTagFilterInfo,
     const vlanCtl_direction_t direction,
     const OmciPhyInfo_t *phyInfo,
     char  *devInterface,
     char  *vlanInterface)
{
    UINT32 i = 0, j = 0;
    OmciVlanTciFilter_t filters[OMCI_TCI_VLANID_LEN];

    memset(filters, OMCI_DONT_CARE, sizeof(OmciVlanTciFilter_t) * OMCI_TCI_VLANID_LEN);

    vlanCtl_init();

    switch (pTagFilterInfo->forwardOperation)
    {
        case 0x15:
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 1, phyInfo, direction);
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 2, phyInfo, direction);
            insertMapperTagRules(pbits, pDscpToPbitInfo, direction, phyInfo, devInterface, vlanInterface);
            break;
        case 0x0C:
        case 0x0B:
        case 0x14:
        case 0x13:
        case 0x1B:
        case 0x1A:
        case 0x21:
        case 0x20:
            if (pTagFilterInfo->forwardOperation == 0x0B ||
                pTagFilterInfo->forwardOperation == 0x13||
                pTagFilterInfo->forwardOperation == 0x1A ||
                pTagFilterInfo->forwardOperation == 0x20)
                setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            else
                setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 1, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 2, phyInfo, direction);
            for (i = OMCI_FILTER_PRIO_MIN; i <= OMCI_FILTER_PRIO_MAX; i++)
            {
                if (pbits[i] >= OMCI_FILTER_PRIO_NONE)
                    continue;
                if (pTagFilterInfo->numberOfEntries > 0)
                {
                    // On tx, filter with the given Pbits and TCI
                    for (j = 1; j < pTagFilterInfo->numberOfEntries; j++)
                    {
                        if (pbits[i] == pTagFilterInfo->pVlanTciTbl[j].pbits)
                        {
                            insertTciFilterTagRules(pTagFilterInfo->pVlanTciTbl[j].pbits,
                                                      pTagFilterInfo->pVlanTciTbl[j].cfi,
                                                      pTagFilterInfo->pVlanTciTbl[j].vlanId,
                                                      VLANCTL_ACTION_ACCEPT, direction,
                                                      phyInfo, devInterface, vlanInterface);
                            // use Pbits as DscpToPbit filter
                            if (pDscpToPbitInfo->unmarkedFrameOption == OMCI_UNMARKED_FRAME_DSCP_TO_PBIT)
                                insertDscpToPbitsFilterTagRules(pbits[i],
                                                VLANCTL_ACTION_ACCEPT,
                                                direction, phyInfo, devInterface, vlanInterface);
                        }
                    }
                }
                else
                {
                    // On tx, filter with the given Pbits
                    insertPbitsFilterTagRules(pbits[i],
                                                VLANCTL_ACTION_ACCEPT, direction,
                                                phyInfo, devInterface, vlanInterface);
                    // use Pbits as DscpToPbit filter
                    if (pDscpToPbitInfo->unmarkedFrameOption == OMCI_UNMARKED_FRAME_DSCP_TO_PBIT)
                        insertDscpToPbitsFilterTagRules(pbits[i], VLANCTL_ACTION_ACCEPT,
                                                        direction, phyInfo, devInterface, vlanInterface);
                }
            }
            // use defaultPBitMarking as DscpToPbit filter
            if (pDscpToPbitInfo->unmarkedFrameOption == OMCI_UNMARKED_FRAME_DEFAULT_PBIT)
                insertDscpToPbitsFilterTagRules(pDscpToPbitInfo->defaultPBitMarking,
                                                VLANCTL_ACTION_ACCEPT,
                                                direction, phyInfo, devInterface, vlanInterface);
            break;
        case 0x08:
        case 0x07:
        case 0x12:
        case 0x11:
        case 0x19:
        case 0x18:
        case 0x1F:
        case 0x1E:
            if (pTagFilterInfo->forwardOperation == 0x07 ||
                pTagFilterInfo->forwardOperation == 0x11 ||
                pTagFilterInfo->forwardOperation == 0x18 ||
                pTagFilterInfo->forwardOperation == 0x1E)
                setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            else
                setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 1, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 2, phyInfo, direction);
            for (i = OMCI_FILTER_PRIO_MIN; i <= OMCI_FILTER_PRIO_MAX; i++)
            {
                if (pbits[i] >= OMCI_FILTER_PRIO_NONE)
                    continue;
                if (pTagFilterInfo->numberOfEntries > 0)
                {
                    // On tx, filter with the given Pbits
                    for (j = 1; j < pTagFilterInfo->numberOfEntries; j++)
                    {
                        if (pbits[i] == pTagFilterInfo->pVlanTciTbl[j].pbits)
                        {
                            insertPbitsFilterTagRules(pbits[i],
                                                        VLANCTL_ACTION_ACCEPT, direction,
                                                        phyInfo, devInterface, vlanInterface);
                            // use Pbits as DscpToPbit filter
                            if (pDscpToPbitInfo->unmarkedFrameOption == OMCI_UNMARKED_FRAME_DSCP_TO_PBIT)
                                insertDscpToPbitsFilterTagRules(pbits[i],
                                                VLANCTL_ACTION_ACCEPT,
                                                direction, phyInfo, devInterface, vlanInterface);
                        }
                    }
                }
                else
                {
                    // On tx, filter with the given Pbits
                    insertPbitsFilterTagRules(pbits[i],
                                                VLANCTL_ACTION_ACCEPT, direction,
                                                phyInfo, devInterface, vlanInterface);
                    // use Pbits as DscpToPbit filter
                    if (pDscpToPbitInfo->unmarkedFrameOption == OMCI_UNMARKED_FRAME_DSCP_TO_PBIT)
                        insertDscpToPbitsFilterTagRules(pbits[i], VLANCTL_ACTION_ACCEPT,
                                                        direction, phyInfo, devInterface, vlanInterface);
                }
            }
            // use defaultPBitMarking as DscpToPbit filter
            if (pDscpToPbitInfo->unmarkedFrameOption == OMCI_UNMARKED_FRAME_DEFAULT_PBIT)
                insertDscpToPbitsFilterTagRules(pDscpToPbitInfo->defaultPBitMarking,
                                                VLANCTL_ACTION_ACCEPT,
                                                direction, phyInfo, devInterface, vlanInterface);
            break;
        case 0x04:
        case 0x03:
        case 0x10:
        case 0x0F:
        case 0x17:
        case 0x16:
        case 0x1D:
        case 0x1C:
            if (pTagFilterInfo->forwardOperation == 0x03 ||
                pTagFilterInfo->forwardOperation == 0x0F ||
                pTagFilterInfo->forwardOperation == 0x16 ||
                pTagFilterInfo->forwardOperation == 0x1C)
                setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            else
                setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 1, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 2, phyInfo, direction);
            for (i = OMCI_FILTER_PRIO_MIN; i <= OMCI_FILTER_PRIO_MAX; i++)
            {
                if (pbits[i] >= OMCI_FILTER_PRIO_NONE)
                    continue;
                if (pTagFilterInfo->numberOfEntries > 0)
                {
                    // On tx, filter with the given Pbits and VLanId
                    for (j = 1; j < pTagFilterInfo->numberOfEntries; j++)
                    {
                        insertTciFilterTagRules(pbits[i],
                                                (UINT8)OMCI_DONT_CARE,
                                                pTagFilterInfo->pVlanTciTbl[j].vlanId,
                                                VLANCTL_ACTION_ACCEPT, direction,
                                                phyInfo, devInterface, vlanInterface);
                        // use Pbits as DscpToPbit filter
                        if (pDscpToPbitInfo->unmarkedFrameOption == OMCI_UNMARKED_FRAME_DSCP_TO_PBIT)
                            insertDscpToPbitsFilterTagRules(pbits[i],
                                                VLANCTL_ACTION_ACCEPT,
                                                direction, phyInfo, devInterface, vlanInterface);
                    }
                }
                else
                {
                    // On tx, filter with the given Pbits
                    insertPbitsFilterTagRules(pbits[i],
                                                VLANCTL_ACTION_ACCEPT, direction,
                                                phyInfo, devInterface, vlanInterface);
                    // use Pbits as DscpToPbit filter
                    if (pDscpToPbitInfo->unmarkedFrameOption == OMCI_UNMARKED_FRAME_DSCP_TO_PBIT)
                        insertDscpToPbitsFilterTagRules(pbits[i], VLANCTL_ACTION_ACCEPT,
                                                        direction, phyInfo, devInterface, vlanInterface);
                }
            }
            // use defaultPBitMarking as DscpToPbit filter
            if (pDscpToPbitInfo->unmarkedFrameOption == OMCI_UNMARKED_FRAME_DEFAULT_PBIT)
                insertDscpToPbitsFilterTagRules(pDscpToPbitInfo->defaultPBitMarking,
                                                VLANCTL_ACTION_ACCEPT,
                                                direction, phyInfo, devInterface, vlanInterface);
            break;
        case 0x0E:
        case 0x0D:
            if (pTagFilterInfo->forwardOperation == 0x0D)
                setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            else
                setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            // setup negative filtering by TCI
            for (i = 1; i < pTagFilterInfo->numberOfEntries; i++)
            {
                // On tx, filter with the given TCI
                insertTciFilterTagRules(pTagFilterInfo->pVlanTciTbl[i].pbits,
                                          pTagFilterInfo->pVlanTciTbl[i].cfi,
                                          pTagFilterInfo->pVlanTciTbl[i].vlanId,
                                          VLANCTL_ACTION_DROP, direction,
                                          phyInfo, devInterface, vlanInterface);
            }
            insertMapperTagRules(pbits, pDscpToPbitInfo, direction, phyInfo, devInterface, vlanInterface);
            break;
        case 0x0A:
        case 0x09:
            if (pTagFilterInfo->forwardOperation == 0x09)
                setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            else
                setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            // setup negative filtering by Pbits
            for (i = 1; i < pTagFilterInfo->numberOfEntries; i++)
            {
                // On tx, filter with the given Pbits
                // drop any frames that match with Pbits filter for this bridge port
                insertPbitsFilterTagRules(pTagFilterInfo->pVlanTciTbl[i].pbits,
                                            VLANCTL_ACTION_DROP, direction,
                                            phyInfo, devInterface, vlanInterface);
            }
            insertMapperTagRules(pbits, pDscpToPbitInfo, direction, phyInfo, devInterface, vlanInterface);
            break;
        case 0x06:
        case 0x05:
            if (pTagFilterInfo->forwardOperation == 0x05)
                setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            else
                setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            // setup negative filtering by VlanId
            for (i = 1; i < pTagFilterInfo->numberOfEntries; i++)
            {
                // On tx, filter with the given Vlan ID
                // drop any frames that match with vlan filter for this bridge port
                insertVlanIdFilterTagRules(pTagFilterInfo->pVlanTciTbl[i].vlanId,
                                             VLANCTL_ACTION_DROP, direction,
                                             phyInfo, devInterface, vlanInterface);
            }
            insertMapperTagRules(pbits, pDscpToPbitInfo, direction, phyInfo, devInterface, vlanInterface);
            break;
        case 0x02:
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 0, phyInfo, direction);
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 1, phyInfo, direction);
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 2, phyInfo, direction);
            insertMapperTagRules(pbits, pDscpToPbitInfo, direction, phyInfo, devInterface, vlanInterface);
            break;
        case 0x01:
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 1, phyInfo, direction);
            setDefaultAction(devInterface, NULL, VLANCTL_ACTION_DROP, 2, phyInfo, direction);
            insertMapperTagRules(pbits, pDscpToPbitInfo, direction, phyInfo, devInterface, vlanInterface);
            break;
        case 0x00:
        default:
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 0, phyInfo, direction);
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 1, phyInfo, direction);
            setDefaultAction(devInterface, vlanInterface, VLANCTL_ACTION_ACCEPT, 2, phyInfo, direction);
            insertMapperTagRules(pbits, pDscpToPbitInfo, direction, phyInfo, devInterface, vlanInterface);
            break;
    }

    vlanCtl_cleanup();

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> insertAniMapperFilterTagRules: numberOfEntries=%d, forwardOperation=%d, direction=%d, phyId=%d, devIfc=%s, vlanIfc=%s\n",
                   pTagFilterInfo->numberOfEntries, pTagFilterInfo->forwardOperation, direction, phyInfo->phyId, devInterface, vlanInterface);
}

/*****************************************************************************
*  FUNCTION:  insertUniDsIgmpMcastTagRules
*  PURPOSE:   Insert VLAN rules related to "Downstream IGMP and multicast TCI"
*             in "Multicast operations profile" ME (309).
*             As the ME defines the subscriber configuration, the VLAN rules
*             are applied on the UNI-side.
*  PARAMETERS:
*      aniPhyInfo - ANI-side(GEM) information.
*      uniInfo - UNI information.
*      direction - VLAN rule direction.
*      dsTagType - downstream tag control type.
*      isIgmp - the rule is applied to IGMP/MLD (true) or multicast (false).
*      mcastAniVid - ANI VLAN ID from dynamic ACL.
*      dsMcastTci - the TCI to be applied.
*  RETURNS:
*      None.
*  NOTES:
*      For the VLAN interface associated with a "real" PPTP UNI, the VLAN
*      rules are applied in the TX direction.
*      For the VLAN interface associated with "veip", the VLAN rules are
*      applied in the RX direction.
*****************************************************************************/
static void insertUniDsIgmpMcastTagRules
    (OmciPhyInfo_t *aniPhyInfo,
     OmciUniInfo_t *uniInfo,
     vlanCtl_direction_t direction,
     UINT8 dsTagType,
     UBOOL8 isIgmp,
     UINT16 mcastAniVid,
     UINT16 dsMcastTci)
{
    UINT8 pbits = ((dsMcastTci & 0xE000) >> 13);
    UINT8 cfi = ((dsMcastTci & 0x1000) >> 12);
    UINT16 vlanId = (dsMcastTci & 0x0FFF);
    UINT32 tagIndex = 0;
    UINT32 tagRuleId = VLANCTL_DONT_CARE;

    vlanCtl_init();

    switch (dsTagType)
    {
        case OMCI_MCAST_DS_TAG_STRIP:
            vlanCtl_initTagRule();
            (direction == VLANCTL_DIRECTION_TX) ?
              vlanCtl_filterOnTxVlanDevice(uniInfo->vlanInterface) :
              vlanCtl_setReceiveVlanDevice(uniInfo->vlanInterface);
            vlanCtl_filterOnSkbMarkPort(aniPhyInfo->phyId);
            if (isIgmp == TRUE)
            {
                vlanCtl_filterOnIpProto(IPPROTO_IGMP);
            }
            else if (mcastAniVid != (UINT16)OMCI_DONT_CARE)
            {
                vlanCtl_filterOnTagVid(mcastAniVid, tagIndex);
            }

            // strip the outer tag
            vlanCtl_cmdPopVlanTag();
            // insert tag rule to single-tagged rule table
            vlanCtl_insertTagRule(uniInfo->devInterface, direction,
                                  1, VLANCTL_POSITION_BEFORE,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(uniInfo->devInterface, uniInfo->phyId, uniInfo->phyType,
              direction, 1, tagRuleId);

            // insert tag rule to double-tagged rule table
            tagRuleId = VLANCTL_DONT_CARE;
            vlanCtl_insertTagRule(uniInfo->devInterface, direction,
                                  2, VLANCTL_POSITION_BEFORE,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(uniInfo->devInterface, uniInfo->phyId, uniInfo->phyType,
              direction, 2, tagRuleId);
            break;
        case OMCI_MCAST_DS_TAG_ADD:
            vlanCtl_initTagRule();
            (direction == VLANCTL_DIRECTION_TX) ?
              vlanCtl_filterOnTxVlanDevice(uniInfo->vlanInterface) :
              vlanCtl_setReceiveVlanDevice(uniInfo->vlanInterface);
            vlanCtl_filterOnSkbMarkPort(aniPhyInfo->phyId);
            if (isIgmp == TRUE)
            {
                vlanCtl_filterOnIpProto(IPPROTO_IGMP);
            }
            else if (mcastAniVid != (UINT16)OMCI_DONT_CARE)
            {
                vlanCtl_filterOnTagVid(mcastAniVid, tagIndex);
            }

            /* insert the outer tag */
            vlanCtl_cmdPushVlanTag();
            // Set pbits in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagPbits(pbits, tagIndex);
            // Set cfi in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagCfi(cfi, tagIndex);
            // Set vid in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagVid(vlanId, tagIndex);
            // insert tag rule to un-tagged rule table
            vlanCtl_insertTagRule(uniInfo->devInterface, direction,
                                  0, VLANCTL_POSITION_BEFORE,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(uniInfo->devInterface, uniInfo->phyId, uniInfo->phyType,
              direction, 0, tagRuleId);

            // insert tag rule to single-tagged rule table
            tagRuleId = VLANCTL_DONT_CARE;
            vlanCtl_insertTagRule(uniInfo->devInterface, direction,
                                  1, VLANCTL_POSITION_BEFORE,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(uniInfo->devInterface, uniInfo->phyId, uniInfo->phyType,
              direction, 1, tagRuleId);
            break;
        case OMCI_MCAST_DS_TAG_REPLACE_TCI:
            vlanCtl_initTagRule();
            (direction == VLANCTL_DIRECTION_TX) ?
              vlanCtl_filterOnTxVlanDevice(uniInfo->vlanInterface) :
              vlanCtl_setReceiveVlanDevice(uniInfo->vlanInterface);
            vlanCtl_filterOnSkbMarkPort(aniPhyInfo->phyId);
            if (isIgmp == TRUE)
            {
                vlanCtl_filterOnIpProto(IPPROTO_IGMP);
            }
            else if (mcastAniVid != (UINT16)OMCI_DONT_CARE)
            {
                vlanCtl_filterOnTagVid(mcastAniVid, tagIndex);
            }

            // Set pbits in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagPbits(pbits, tagIndex);
            // Set cfi in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagCfi(cfi, tagIndex);
            // Set vid in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagVid(vlanId, tagIndex);

            // insert tag rule to single-tagged rule table
            vlanCtl_insertTagRule(uniInfo->devInterface, direction,
                                  1, VLANCTL_POSITION_BEFORE,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(uniInfo->devInterface, uniInfo->phyId, uniInfo->phyType,
              direction, 1, tagRuleId);
            // insert tag rule to double-tagged rule table
            tagRuleId = VLANCTL_DONT_CARE;
            vlanCtl_insertTagRule(uniInfo->devInterface, direction,
                                  2, VLANCTL_POSITION_BEFORE,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(uniInfo->devInterface, uniInfo->phyId, uniInfo->phyType,
              direction, 2, tagRuleId);
            break;
        case OMCI_MCAST_DS_TAG_REPLACE_VLAN:
            vlanCtl_initTagRule();
            (direction == VLANCTL_DIRECTION_TX) ?
              vlanCtl_filterOnTxVlanDevice(uniInfo->vlanInterface) :
              vlanCtl_setReceiveVlanDevice(uniInfo->vlanInterface);
            vlanCtl_filterOnSkbMarkPort(aniPhyInfo->phyId);
            if (isIgmp == TRUE)
            {
                vlanCtl_filterOnIpProto(IPPROTO_IGMP);
            }
            else if (mcastAniVid != (UINT16)OMCI_DONT_CARE)
            {
                vlanCtl_filterOnTagVid(mcastAniVid, tagIndex);
            }

            // Set vid in tag index 0, which is always the outer tag of the frame
            vlanCtl_cmdSetTagVid(vlanId, tagIndex);
            // insert tag rule to single-tagged rule table
            vlanCtl_insertTagRule(uniInfo->devInterface, direction,
                                  1, VLANCTL_POSITION_BEFORE,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(uniInfo->devInterface, uniInfo->phyId, uniInfo->phyType,
              direction, 1, tagRuleId);
            // insert tag rule to double-tagged rule table
            tagRuleId = VLANCTL_DONT_CARE;
            vlanCtl_insertTagRule(uniInfo->devInterface, direction,
                                  2, VLANCTL_POSITION_BEFORE,
                                  VLANCTL_DONT_CARE, &tagRuleId);
            omci_rule_add(uniInfo->devInterface, uniInfo->phyId, uniInfo->phyType,
              direction, 2, tagRuleId);
            break;
        default:
            break;
    }

    vlanCtl_cleanup();

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
      "===> insertUniDsIgmpMcastTagRules: aniPhyId=%d, "
      "UNI(phyId=%d, devIfc=%s, vlanIfc=%s), dir=%d, "
      "dsTagType=%d, isIgmp=%d, mcastAniVid=%d, dsMcastTci=%d\n",
      aniPhyInfo->phyId, uniInfo->phyId, uniInfo->devInterface,
      uniInfo->vlanInterface, direction, dsTagType, isIgmp,
      mcastAniVid, dsMcastTci);
}

/*****************************************************************************
*  FUNCTION:  buildOmciModelAniInfo
*  PURPOSE:   Build ANI information structure from the OMCI model. The
*             information may be saved for better performance.
*  PARAMETERS:
*      ctp - pointer to GEM object.
*      aniPhyInfo - pointer to the derived ANI-side(GEM) information.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet buildOmciModelAniInfo(const GemPortNetworkCtpObject *ctp,
  OmciPhyInfo_t *aniPhyInfo)
{
    UBOOL8 multicast;
    UBOOL8 broadcast;
    UINT32 phyId;
    CmsRet ret = CMSRET_SUCCESS;

    multicast = rutGpon_isGemPortNetworkCtpMulticast(ctp);
    broadcast = rutGpon_isGemPortNetworkCtpIncidentBroadcast(ctp);

    ret = rutGpon_getIndexFromGemPortNetworkCtp(ctp, &phyId);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("getIndexFromGemPortNetworkCtp() failed, "
          "GEM portIdValue=%d)", ctp->portIdValue);
        return ret;
    }

    aniPhyInfo->phyId = phyId;
    aniPhyInfo->phyType = OMCI_PHY_GPON;
    aniPhyInfo->gemPortType = getGemPortType(broadcast, multicast);

    return CMSRET_SUCCESS;
}

/*****************************************************************************
*  FUNCTION:  procUniDsIgmpMcastTag
*  PURPOSE:   Handles "Downstream IGMP and multicast TCI" attribute.
*  PARAMETERS:
*      ctp - pointer to GEM object.
*      uniOid - UNI OID.
*      uniMeId - UNI ME id.
*      isVeip - whether the UNI is VEIP.
*      direction - rule direction.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void procUniDsIgmpMcastTag(const GemPortNetworkCtpObject *ctp,
  const UINT32 uniOid, const UINT32 uniMeId, UINT8 isVeip,
  vlanCtl_direction_t direction)
{
    UINT8 dsTagType;
    UINT16 dsMcastTci;
    UINT32 vidListNum;
    UINT16 vidList[MCAST_VID_LIST_MAX] = {0};
    OmciPhyInfo_t aniPhyInfo;
    OmciUniInfo_t uniInfo;
    CmsRet ret = CMSRET_SUCCESS;

    ret = buildOmciModelAniInfo(ctp, &aniPhyInfo);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("buildOmciModelAniInfo() failed, GEM portIdValue=%d)",
          ctp->portIdValue);
        return;
    }

    ret = buildOmciModelUniInfo(uniOid, uniMeId, isVeip, &uniInfo);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("buildOmciModelUniInfo() failed, UNI (%d:%d)",
          uniOid, uniMeId);
        return;
    }

    ret = rutGpon_getDsMcastTciFromBridgeMeId(uniInfo.bridgeMeId, &dsTagType,
      &dsMcastTci);
    if ((ret == CMSRET_SUCCESS) && (dsTagType != OMCI_MCAST_DS_TAG_NONE))
    {
        ret = rutGpon_getMcastAclAniVidListFromUniMeId(uniInfo.uniOid,
          uniInfo.uniMeId, 1, &vidListNum, &vidList[0]);
        if (ret != CMSRET_SUCCESS)
        {
            vidList[0] = (UINT16)OMCI_DONT_CARE;
        }

        // IGMP traffic can be in unicast or multicast gem
        if (aniPhyInfo.gemPortType != OMCI_SERVICE_BROADCAST)
        {
            insertUniDsIgmpMcastTagRules(&aniPhyInfo, &uniInfo, direction,
              dsTagType, TRUE, OMCI_DONT_CARE, dsMcastTci);
        }
        // multicast data traffic can be in multicast gem only
        if (aniPhyInfo.gemPortType == OMCI_SERVICE_MULTICAST)
        {
            insertUniDsIgmpMcastTagRules(&aniPhyInfo, &uniInfo, direction,
              dsTagType, FALSE, vidList[0], dsMcastTci);
        }
    }
}

void createVlanFlows(char *rxIface, char *txIface)
{
    vlanCtl_init();
    vlanCtl_createVlanFlows(rxIface, txIface);
    vlanCtl_cleanup();
}

void saveVlanFlowstoCreate(char *rxIface, char *txIface)
{
    omciDm_addVlanFlowIfPair(rxIface, txIface);
}

void createVlanFlowsAction(OmciDmIfPair *infoP)
{
    createVlanFlows(infoP->rxIf, infoP->txIf);
    createVlanFlows(infoP->txIf, infoP->rxIf);
}

static CmsRet createUniMapperTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32 uniOid,
     const UINT32 uniMeId,
     char         **uniVirtualIfaceName)
{
    UINT8  pbits[OMCI_FILTER_PRIO_NONE], i = 0;
    UBOOL8 multicast = rutGpon_isGemPortNetworkCtpMulticast(ctp);
    UINT32 mapperMeId = 0;
    OmciPhyInfo_t phyInfo;
    UINT32 phyId = 0;
    char *devInterface = NULL, *vlanInterface = NULL;

    CmsRet ret = CMSRET_SUCCESS;

    for (i = OMCI_FILTER_PRIO_MIN; i <= OMCI_FILTER_PRIO_MAX; i++)
        pbits[i] = OMCI_FILTER_PRIO_NONE;

    if ((ret = rutGpon_getPbitsFromGemPortNeworkCtp
                  (ctp, pbits, &mapperMeId)) != CMSRET_SUCCESS)
        goto out;

    ret = rutGpon_getUniPhyInfo(uniOid, uniMeId, &phyInfo);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("rutGpon_getUniPhyInfo() failed, UNI (%d:%d)",
          uniOid, uniMeId);
        goto out;
    }

    rutGpon_getInterfaceName(phyInfo.phyType, phyInfo.phyId, &devInterface);
    rutGpon_getVirtualInterfaceName(phyInfo.phyType, phyInfo.phyId, mapperMeId, &vlanInterface);

    // interface does not existed yet then
    // create new interface and link it to bridge
    if (rutGpon_isInterfaceExisted(vlanInterface) == FALSE)
    {
        char cmd[BUFLEN_256];
        // remove devInterface (eth0, eth1, etc...) out of default bridge br0

#ifdef SUPPORT_LANVLAN
        snprintf(cmd, sizeof(cmd), "brctl delif br0 %s.0 2>/dev/null", devInterface);
#else
        snprintf(cmd, sizeof(cmd), "brctl delif br0 %s 2>/dev/null", devInterface);
#endif /* SUPPORT_LANVLAN */
        _owapi_rut_doSystemAction("rut_gpon_rule", cmd);

        if (rutGpon_checkInterfaceUp(devInterface) == FALSE)
            rutGpon_ifUp(devInterface);

        if ((ret = createVlanInterface(mapperMeId, 0, multicast,
                                       devInterface, vlanInterface)) != CMSRET_SUCCESS)
            goto out;
        if ((ret = addInterfaceToMapperBridge(devInterface, vlanInterface,
                                              mapperMeId)) != CMSRET_SUCCESS)
            goto out;
    }
    else
    {
        // interface already exists then copy its current
        // tag rules so that they can be deleted later
        if (omci_rule_exist(devInterface, phyInfo.phyId) == CMSRET_SUCCESS)
            omci_rule_copy(devInterface, phyInfo.phyId);
    }

    phyInfo.gemPortType = OMCI_SERVICE_UNICAST;
    insertUniTagRules(ctp, uniOid, uniMeId, &phyInfo, devInterface, vlanInterface);

    // interface already exists then remove old tag rules
    if (omci_rule_exist(devInterface, phyId) == CMSRET_SUCCESS)
        omci_rule_remove(devInterface, phyId);

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createUniMapperTagRules, aniMeId=%d, uniOid=%d, uniMeId=%d, mapperMeId=%d, phyId=%d, devIfc=%s, vlanIfc=%s, ret=%d\n",
                   ctp->managedEntityId, uniOid, uniMeId, mapperMeId, phyInfo.phyId,
                   CHECK_INTF_PTR(devInterface), CHECK_INTF_PTR(vlanInterface),
                   ret);

    if (uniOid == MDMOID_PPTP_ETHERNET_UNI)
        *uniVirtualIfaceName = vlanInterface;
    else
        CMSMEM_FREE_BUF_AND_NULL_PTR(vlanInterface);
    CMSMEM_FREE_BUF_AND_NULL_PTR(devInterface);

    return ret;
}

static CmsRet createIpHostMapperTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32 ipHostOid,
     const UINT32 ipHostMeId,
     const OmciMapFilterModelType  type)
{
    UBOOL8 multicast = rutGpon_isGemPortNetworkCtpMulticast(ctp);
    UINT32 devId = 0, mapperMeId = 0;
    char   devInterface[CMS_IFNAME_LENGTH], vlanInterface[CMS_IFNAME_LENGTH];
    OmciPhyInfo_t phyInfo;
    CmsRet ret = CMSRET_SUCCESS;

    memset(devInterface, 0, CMS_IFNAME_LENGTH);
    memset(vlanInterface, 0, CMS_IFNAME_LENGTH);

    if ((ret = getIpHostInterfaces(ipHostOid, ipHostMeId, type,
                                   &devId, &mapperMeId,
                                   devInterface, vlanInterface)) != CMSRET_SUCCESS)
        goto out;

    // interface does not existed yet then
    // create new interface and link it to bridge
    if (rutGpon_isInterfaceExisted(vlanInterface) == FALSE)
    {
        if ((ret = createVlanInterface(devId, 0, multicast,
                                       devInterface, vlanInterface)) != CMSRET_SUCCESS)
            goto out;
    }
    else
    {
        // interface already exists then copy its current
        // tag rules so that they can be deleted later
        if (omci_rule_exist(devInterface, devId) == CMSRET_SUCCESS)
            omci_rule_copy(devInterface, devId);
    }

    phyInfo.phyId = devId;
    phyInfo.phyType = OMCI_PHY_ETHERNET;
    phyInfo.gemPortType = OMCI_SERVICE_UNICAST;
    insertUniTagRules(ctp, ipHostOid, ipHostMeId, &phyInfo, devInterface, vlanInterface);

    // interface already exists then remove old tag rules
    if (omci_rule_exist(devInterface, devId) == CMSRET_SUCCESS)
        omci_rule_remove(devInterface, devId);

    ret = rutGpon_configIpHost(ipHostOid, ipHostMeId, vlanInterface);

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createIpHostMapperTagRules, aniMeId=%d, ipHostOid=%d, ipHostMeId=%d, mapperMeId=%d, devId=%d, devIfc=%s, vlanIfc=%s, ret=%d\n",
                   ctp->managedEntityId, ipHostOid, ipHostMeId, mapperMeId, devId, devInterface, vlanInterface, ret);

    return ret;
}

static CmsRet createVeipMapperTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32 uniOid,
     const UINT32 uniMeId)
{
    UINT32 bridgeMeId = 0;
    char veipVlanName[CMS_IFNAME_LENGTH] = {0};
    OmciPhyInfo_t phyInfo;
    CmsRet ret = CMSRET_SUCCESS;

    getVeipVlanName(veipVlanName);

    // delete bridge interface that is connected from VEIP to ANI since
    // should NOT have bridge between veipVlanName
    // and VEIP_DEV_INTERFACE by design
    if ((ret = rutGpon_getBridgeMeIdFromUniMeId(uniOid, uniMeId,
                                                &bridgeMeId)) == CMSRET_SUCCESS)
        deleteMapperBridge(bridgeMeId);


    // veipVlanName does not existed yet then
    // create new veipVlanName that links to VEIP_DEV_INTERFACE
    if (rutGpon_isInterfaceExisted(veipVlanName) == FALSE)
    {
        if ((ret = createVeipVlanInterface()) != CMSRET_SUCCESS)
            goto out;
    }
    else
    {
        // interface already exists then copy its current
        // tag rules so that they can be deleted later
        if (omci_rule_exist(VEIP_DEV_INTERFACE, VEIP_DEV_ID) == CMSRET_SUCCESS)
            omci_rule_copy(VEIP_DEV_INTERFACE, VEIP_DEV_ID);
    }

    setIgmpRateLimitOnBridge(bridgeMeId, DEFAULT_RG_BRIDGE_NAME,
      veipVlanName, FALSE);

    phyInfo.phyId = VEIP_DEV_ID;
    phyInfo.phyType = OMCI_PHY_ETHERNET;
    phyInfo.gemPortType = OMCI_SERVICE_UNICAST;
    insertUniTagRules(ctp, uniOid, uniMeId, &phyInfo, VEIP_DEV_INTERFACE, veipVlanName);

    // interface already exists then remove old tag rules
    if (omci_rule_exist(VEIP_DEV_INTERFACE, VEIP_DEV_ID) == CMSRET_SUCCESS)
        omci_rule_remove(VEIP_DEV_INTERFACE, VEIP_DEV_ID);

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createVeipMapperTagRules, aniMeId=%d, uniOid=%d, uniMeId=%d, bridgeMeId=%d, phyId=%d, devIfc=%s, vlanIfc=%s, ret=%d\n",
                   ctp->managedEntityId, uniOid, uniMeId, bridgeMeId, VEIP_DEV_ID, VEIP_DEV_INTERFACE, veipVlanName, ret);

    return ret;
}

static CmsRet createAniInterface
    (char *vlanInterface)
{
    CmsRet ret = CMSRET_SUCCESS;

    vlanCtl_init();
    // always set isMulticast to 1 for supporting broadcast
    rutGpon_vlanCtl_createVlanInterfaceByName(GPON_DEF_IFNAME, vlanInterface, 0, 1);
    vlanCtl_setRealDevMode(GPON_DEF_IFNAME, BCM_VLAN_MODE_ONT);
    vlanCtl_cleanup();

    if (rutGpon_isInterfaceExisted(vlanInterface) == FALSE)
    {
        cmsLog_error("Failed to create %s", vlanInterface);
        ret = CMSRET_INTERNAL_ERROR;
    }
    else
    {
        rutGpon_ifUp(vlanInterface);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createAniInterface, devIfc=%s, ret=%d\n",
                   vlanInterface, ret);

    return ret;
}

static CmsRet createAniMapperTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32                  uniOid,
     const UINT32                  uniMeId,
     char                          **aniVirtualIfaceName)
{
    UINT8  pbits[OMCI_FILTER_PRIO_NONE], i = 0;
    UBOOL8 multicast = rutGpon_isGemPortNetworkCtpMulticast(ctp);
    UBOOL8 broadcast = rutGpon_isGemPortNetworkCtpIncidentBroadcast(ctp);
    UBOOL8 hasDscpToPbit = TRUE;
    UBOOL8 isVeipPptpUni = rutGpon_isVeipPptpUni(uniOid, uniMeId);
    UINT32 phyId = 0, mapperMeId = 0;
    char *devInterface = NULL, *vlanInterface = NULL;
    OmciDscpPbitMappingInfo_t dscpToPbitInfo;
    OmciPhyInfo_t phyInfo;
    CmsRet ret = CMSRET_SUCCESS;

    for (i = OMCI_FILTER_PRIO_MIN; i <= OMCI_FILTER_PRIO_MAX; i++)
        pbits[i] = OMCI_FILTER_PRIO_NONE;

    // values of pbits are used either vlan interface is
    // already created or not ==> get their values first anyway
    if ((ret = rutGpon_getPbitsFromGemPortNeworkCtp
                  (ctp, pbits, &mapperMeId)) != CMSRET_SUCCESS)
        goto out;

    // if UNI is VEIP then assign mapperMeId to 0
    // since VEIP should be connected to default bridge br0
    if (uniOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT || isVeipPptpUni)
    {
        mapperMeId = 0;
    }

    // initialize DSCP Pbit mapping info
    memset(&dscpToPbitInfo, 0, sizeof(OmciDscpPbitMappingInfo_t));

    if ((ret = rutGpon_getDscpToPbitFromGemPortNeworkCtp
                  (ctp, &dscpToPbitInfo)) != CMSRET_SUCCESS)
    {
        if (rutGpon_getDscpToPbitFromUniMeId
                (uniOid, uniMeId, &dscpToPbitInfo) != CMSRET_SUCCESS)
            hasDscpToPbit = FALSE;
    }

    // no valid dscp to pbit table
    if (hasDscpToPbit == FALSE)
    {
        dscpToPbitInfo.defaultPBitMarking = 0;
        dscpToPbitInfo.unmarkedFrameOption = OMCI_UNMARKED_FRAME_NONE;
    }

    if ((ret = rutGpon_getIndexFromGemPortNetworkCtp(ctp, &phyId)) != CMSRET_SUCCESS)
        goto out;

    rutGpon_getInterfaceName(OMCI_PHY_GPON, mapperMeId, &devInterface);
    // create devInterface if it does not exist
    if (rutGpon_isInterfaceExisted(devInterface) == FALSE)
    {
        if ((ret = createAniInterface(devInterface)) != CMSRET_SUCCESS)
            goto out;
    }

    // On rx, default is drop on devInterface
    phyInfo.phyId = phyId;
    phyInfo.phyType = OMCI_PHY_GPON;
    phyInfo.gemPortType = getGemPortType(broadcast, multicast);

    if (omci_rule_exist(GPON_DEF_IFNAME, phyId) != CMSRET_SUCCESS || multicast)
        insertDefaultAniTagRules(GPON_DEF_IFNAME, devInterface, &phyInfo,
                                 VLANCTL_DIRECTION_RX, VLANCTL_ACTION_DROP);

    rutGpon_getVirtualInterfaceName(OMCI_PHY_GPON, mapperMeId, mapperMeId, &vlanInterface);

    // interface does not existed yet then
    // create new interface and link it to bridge
    if (rutGpon_isInterfaceExisted(vlanInterface) == FALSE)
    {
        if ((ret = createVlanInterface(mapperMeId, 0, multicast,
                                       devInterface, vlanInterface)) != CMSRET_SUCCESS)
            goto out;

        // only add interface to bridge  when UNI is NOT VEIP
        // since VEIP_VLAN_INTERFACE is stacked to VEIP_DEV_INTERFACE
        // and VEIP_PHY_INTERFACE instead of connecting through bridge
        if (uniOid != MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT && !isVeipPptpUni)
        {
            if ((ret = addInterfaceToMapperBridge(devInterface, vlanInterface,
                                                   mapperMeId)) != CMSRET_SUCCESS)
                    goto out;
        }

        // if interface is multicast then update mcast info in MCPD configuration file
        // should call before addInterfaceToMapperBridge()
        if (multicast == TRUE)
            _owapi_rut_updateMcastCtrlIfNames(vlanInterface);

        // default rules should be added once only when interface is created
        // since these rules do not apply for specific gem port (phyId)
        // On tx, default is drop
        insertDefaultTagRules(devInterface, vlanInterface, &phyInfo,
                              VLANCTL_DIRECTION_TX, VLANCTL_ACTION_DROP);
    }
    else
    {
        // interface already exists then copy its current
        // tag rules so that they can be deleted later
        if (omci_rule_exist(devInterface, phyId) == CMSRET_SUCCESS)
            omci_rule_copy(devInterface, phyId);
    }

    // Setup TPID table for ANI interface
    setTpidTable(devInterface, C_TAG_TPID, S_TAG_TPID, D_TAG_TPID);

    // On rx, default is drop on vlanInterface
    // always filter packets on GEM-Index since no vlan filter
    insertDefaultAniTagRules(devInterface, vlanInterface, &phyInfo,
                      VLANCTL_DIRECTION_RX, VLANCTL_ACTION_DROP);

    // if unmarked frame uses DSCP to Pbit mapping then setDscpToPbitsTable
    if (dscpToPbitInfo.unmarkedFrameOption == OMCI_UNMARKED_FRAME_DSCP_TO_PBIT)
        setDscpToPbitsTable(dscpToPbitInfo.dscpToPbit, devInterface);

    // don't configure tx rule on multicast or broadcast gem
    if (multicast == FALSE && broadcast == FALSE)
        insertAniMapperTagRules(pbits, &dscpToPbitInfo, &phyInfo, devInterface, vlanInterface);

    // interface already exists then remove old tag rules
    if (omci_rule_exist(devInterface, phyId) == CMSRET_SUCCESS)
        omci_rule_remove(devInterface, phyId);

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createAniMapperTagRules, aniMeId=%d, gemPort=%d, pbits=[%d|%d|%d|%d|%d|%d|%d|%d], phyId=%d, devIfc=%s, vlanIfc=%s, ret=%d\n",
                   ctp->managedEntityId, phyId, pbits[0], pbits[1], pbits[2], pbits[3],
                   pbits[4], pbits[5], pbits[6], pbits[7], phyId,
                   CHECK_INTF_PTR(devInterface), CHECK_INTF_PTR(vlanInterface), ret);

    if (devInterface != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(devInterface);

    *aniVirtualIfaceName = vlanInterface;

    return ret;
}

static CmsRet createUniFilterTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32 uniOid,
     const UINT32 uniMeId,
     char         **uniVirtualIfaceName)
{
    UBOOL8 multicast = rutGpon_isGemPortNetworkCtpMulticast(ctp);
    UINT32 bridgeMeId = 0;
    char *devInterface = NULL, *vlanInterface = NULL;
    OmciPhyInfo_t phyInfo;
    CmsRet ret = CMSRET_SUCCESS;

    ret = rutGpon_getUniPhyInfo(uniOid, uniMeId, &phyInfo);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("rutGpon_getUniPhyInfo() failed, UNI (%d:%d)",
          uniOid, uniMeId);
        goto out;
    }
    if ((ret = rutGpon_getBridgeMeIdFromUniMeId(uniOid, uniMeId,
                                                &bridgeMeId)) != CMSRET_SUCCESS)
        goto out;

    rutGpon_getInterfaceName(phyInfo.phyType, phyInfo.phyId, &devInterface);
    rutGpon_getVirtualInterfaceName(phyInfo.phyType, phyInfo.phyId, bridgeMeId, &vlanInterface);

    // interface does not existed yet then
    // create new interface and link it to bridge
    if (rutGpon_isInterfaceExisted(vlanInterface) == FALSE)
    {
        char cmd[BUFLEN_256];
        // remove devInterface (eth0, eth1, etc...) out of default bridge br0
#ifdef SUPPORT_LANVLAN
        snprintf(cmd, sizeof(cmd), "brctl delif br0 %s.0 2>/dev/null", devInterface);
#else
        snprintf(cmd, sizeof(cmd), "brctl delif br0 %s 2>/dev/null", devInterface);
#endif /* SUPPORT_LANVLAN */
        _owapi_rut_doSystemAction("rut_gpon_rule", cmd);

        if (rutGpon_checkInterfaceUp(devInterface) == FALSE)
            rutGpon_ifUp(devInterface);

        if ((ret = createVlanInterface(bridgeMeId, 0, multicast,
                                       devInterface, vlanInterface)) != CMSRET_SUCCESS)
            goto out;
    }
    else
    {
        // interface already exists then copy its current
        // tag rules so that they can be deleted later
        if (omci_rule_exist(devInterface, phyInfo.phyId) == CMSRET_SUCCESS)
            omci_rule_copy(devInterface, phyInfo.phyId);
    }

    if ((ret = addInterfaceToServiceBridge(devInterface, vlanInterface,
                                           bridgeMeId)) != CMSRET_SUCCESS)
        goto out;

    phyInfo.gemPortType = OMCI_SERVICE_UNICAST;
    insertUniTagRules(ctp, uniOid, uniMeId, &phyInfo, devInterface, vlanInterface);

    // interface already exists then remove old tag rules
    if (omci_rule_exist(devInterface, phyInfo.phyId) == CMSRET_SUCCESS)
        omci_rule_remove(devInterface, phyInfo.phyId);

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createUniFilterTagRules, aniMeId=%d, uniOid=%d, uniMeId=%d, bridgeMeId=%d, phyId=%d, devIfc=%s, vlanIfc=%s, ret=%d\n",
                   ctp->managedEntityId, uniOid, uniMeId, bridgeMeId, phyInfo.phyId,
                   CHECK_INTF_PTR(devInterface), CHECK_INTF_PTR(vlanInterface), ret);

    if (uniOid == MDMOID_PPTP_ETHERNET_UNI)
        *uniVirtualIfaceName = vlanInterface;
    else
        CMSMEM_FREE_BUF_AND_NULL_PTR(vlanInterface);
    CMSMEM_FREE_BUF_AND_NULL_PTR(devInterface);

    return ret;
}

static CmsRet createIpHostFilterTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32 ipHostOid,
     const UINT32 ipHostMeId,
     const OmciMapFilterModelType  type)
{
    UBOOL8 multicast = rutGpon_isGemPortNetworkCtpMulticast(ctp);
    UINT32 devId = 0, bridgeMeId = 0;
    char   devInterface[CMS_IFNAME_LENGTH], vlanInterface[CMS_IFNAME_LENGTH];
    OmciPhyInfo_t phyInfo;
    CmsRet ret = CMSRET_SUCCESS;

    memset(devInterface, 0, CMS_IFNAME_LENGTH);
    memset(vlanInterface, 0, CMS_IFNAME_LENGTH);

    if ((ret = getIpHostInterfaces(ipHostOid, ipHostMeId, type,
                                   &devId, &bridgeMeId,
                                   devInterface, vlanInterface)) != CMSRET_SUCCESS)
        goto out;

    // interface does not existed yet then
    // create new interface and link it to bridge
    if (rutGpon_isInterfaceExisted(vlanInterface) == FALSE)
    {
        if ((ret = createVlanInterface(devId, 0, multicast,
                                       devInterface, vlanInterface)) != CMSRET_SUCCESS)
            goto out;
    }
    else
    {
        // interface already exists then copy its current
        // tag rules so that they can be deleted later
        if (omci_rule_exist(devInterface, devId) == CMSRET_SUCCESS)
            omci_rule_copy(devInterface, devId);
    }

    phyInfo.phyId = devId;
    phyInfo.phyType = OMCI_PHY_ETHERNET;
    phyInfo.gemPortType = OMCI_SERVICE_UNICAST;
    insertUniTagRules(ctp, ipHostOid, ipHostMeId, &phyInfo, devInterface, vlanInterface);

    // interface already exists then remove old tag rules
    if (omci_rule_exist(devInterface, devId) == CMSRET_SUCCESS)
        omci_rule_remove(devInterface, devId);

    ret = rutGpon_configIpHost(ipHostOid, ipHostMeId, vlanInterface);

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createIpHostFilterTagRules, aniMeId=%d, ipHostOid=%d, ipHostMeId=%d, bridgeMeId=%d, devId=%d, devIfc=%s, vlanIfc=%s, ret=%d\n",
                   ctp->managedEntityId, ipHostOid, ipHostMeId, bridgeMeId, devId, devInterface, vlanInterface, ret);

    return ret;
}

static CmsRet createVeipFilterTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32 uniOid,
     const UINT32 uniMeId)
{
    UINT32 bridgeMeId = 0;
    char veipVlanName[CMS_IFNAME_LENGTH] = {0};
    OmciPhyInfo_t phyInfo;
    CmsRet ret = CMSRET_SUCCESS;

    getVeipVlanName(veipVlanName);

    // delete bridge interface that is connected from VEIP to ANI since
    // should NOT have bridge between veipVlanName
    // and VEIP_DEV_INTERFACE by design
    if ((ret = rutGpon_getBridgeMeIdFromUniMeId(uniOid, uniMeId,
                                                &bridgeMeId)) == CMSRET_SUCCESS)
        deleteServiceBridge(bridgeMeId);

    // veipVlanName does not existed yet then
    // create new veipVlanName that links to VEIP_DEV_INTERFACE
    if (rutGpon_isInterfaceExisted(veipVlanName) == FALSE)
    {
        if ((ret = createVeipVlanInterface()) != CMSRET_SUCCESS)
            goto out;
    }
    else
    {
        // interface already exists then copy its current
        // tag rules so that they can be deleted later
        if (omci_rule_exist(VEIP_DEV_INTERFACE, VEIP_DEV_ID) == CMSRET_SUCCESS)
            omci_rule_copy(VEIP_DEV_INTERFACE, VEIP_DEV_ID);
    }

    setIgmpRateLimitOnBridge(bridgeMeId, DEFAULT_RG_BRIDGE_NAME,
      veipVlanName, FALSE);

    phyInfo.phyId = VEIP_DEV_ID;
    phyInfo.phyType = OMCI_PHY_ETHERNET;
    phyInfo.gemPortType = OMCI_SERVICE_UNICAST;
    insertUniTagRules(ctp, uniOid, uniMeId, &phyInfo, VEIP_DEV_INTERFACE, veipVlanName);

    // interface already exists then remove old tag rules
    if (omci_rule_exist(VEIP_DEV_INTERFACE, VEIP_DEV_ID) == CMSRET_SUCCESS)
        omci_rule_remove(VEIP_DEV_INTERFACE, VEIP_DEV_ID);

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createVeipFilterTagRules, aniMeId=%d, uniOid=%d, uniMeId=%d, bridgeMeId=%d, phyId=%d, devIfc=%s, vlanIfc=%s, ret=%d\n",
                   ctp->managedEntityId, uniOid, uniMeId, bridgeMeId, VEIP_DEV_ID, VEIP_DEV_INTERFACE, veipVlanName, ret);

    return ret;
}

static CmsRet createAniFilterTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32                  uniOid,
     const UINT32                  uniMeId,
     char                          **aniVirtualIfaceName)
{
    UBOOL8 multicast = rutGpon_isGemPortNetworkCtpMulticast(ctp);
    UBOOL8 broadcast = rutGpon_isGemPortNetworkCtpIncidentBroadcast(ctp);
    UBOOL8 isVeipPptpUni = rutGpon_isVeipPptpUni(uniOid, uniMeId);
    UINT32 vlanFilterId = 0, phyId = 0, bridgeMeId = 0, omciBridgeMeId = 0;
    char *devInterface = NULL, *vlanInterface = NULL;
    OmciVlanTciFilterInfo_t tagFilterInfo;
    OmciPhyInfo_t phyInfo;
    UBOOL8 defaultRuleToAdd = FALSE;
    CmsRet ret = CMSRET_SUCCESS;

    // initialize local variables
    memset(&tagFilterInfo, 0, sizeof(OmciVlanTciFilterInfo_t));

    if ((ret = rutGpon_getIndexFromGemPortNetworkCtp(ctp, &phyId)) != CMSRET_SUCCESS)
        goto out;

    if ((ret = rutGpon_getBridgeMeIdFromUniMeId(uniOid, uniMeId,
                                                &omciBridgeMeId)) != CMSRET_SUCCESS)
        goto out;

    // only assign bridgeMeId to real omciBridgeMeId when UNI is NOT VEIP
    // since VEIP is connected to default bridge that has bridgeMeId is 0
    if (uniOid != MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT && !isVeipPptpUni)
    {
        bridgeMeId = omciBridgeMeId;
    }

    rutGpon_getInterfaceName(OMCI_PHY_GPON, bridgeMeId, &devInterface);
    // create devInterface if it does not exist
    if (rutGpon_isInterfaceExisted(devInterface) == FALSE)
    {
        if ((ret = createAniInterface(devInterface)) != CMSRET_SUCCESS)
            goto out;
    }

    // On rx, default is drop on devInterface
    phyInfo.phyId = phyId;
    phyInfo.phyType = OMCI_PHY_GPON;
    phyInfo.gemPortType = getGemPortType(broadcast, multicast);

    if (omci_rule_exist(GPON_DEF_IFNAME, phyId) != CMSRET_SUCCESS || multicast)
        insertDefaultAniTagRules(GPON_DEF_IFNAME, devInterface, &phyInfo,
                                 VLANCTL_DIRECTION_RX, VLANCTL_ACTION_DROP);

    rutGpon_getVirtualInterfaceName(OMCI_PHY_GPON, bridgeMeId, bridgeMeId, &vlanInterface);

    // interface does not existed yet then
    // create new interface and link it to bridge
    if (rutGpon_isInterfaceExisted(vlanInterface) == FALSE)
    {
        if ((ret = createVlanInterface(bridgeMeId, 0, multicast,
                                       devInterface, vlanInterface)) != CMSRET_SUCCESS)
            goto out;
    }
    else
    {
        // interface already exists then copy its current
        // tag rules so that they can be deleted later
        if (omci_rule_exist(devInterface, phyId) == CMSRET_SUCCESS)
            omci_rule_copy(devInterface, phyId);
    }

    // Setup TPID table for ANI interface
    setTpidTable(devInterface, C_TAG_TPID, S_TAG_TPID, D_TAG_TPID);

    // if interface is multicast then update mcast info in MCPD configuration file
    // should call before addInterfaceToServiceBridge()
    if (multicast == TRUE)
        _owapi_rut_updateMcastCtrlIfNames(vlanInterface);

    // only add interface to bridge  when UNI is NOT VEIP
    // since VEIP_VLAN_INTERFACE is stacked to VEIP_DEV_INTERFACE
    // and VEIP_PHY_INTERFACE instead of connecting through bridge
    if (uniOid != MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT && !isVeipPptpUni)
    {
        if ((ret = addInterfaceToServiceBridge(devInterface, vlanInterface,
                                               bridgeMeId)) != CMSRET_SUCCESS)
                goto out;
    }

    // always use real omciBridgeMeId (instead of bridgeMeId) to get vlan info
    if (rutGpon_getVlanFilterMeIdFromGemPortNetworkCtpMeId(ctp->managedEntityId,
                                   omciBridgeMeId,
                                   &vlanFilterId) == CMSRET_SUCCESS &&
        rutGpon_allocTagFilterInfo(ctp->managedEntityId, omciBridgeMeId,
                                   &tagFilterInfo) == CMSRET_SUCCESS)
    {
        if (multicast == FALSE && broadcast == FALSE)
        {
            if (tagFilterInfo.forwardOperation == 0x03 ||
                tagFilterInfo.forwardOperation == 0x04 ||
                tagFilterInfo.forwardOperation == 0x07 ||
                tagFilterInfo.forwardOperation == 0x08 ||
                tagFilterInfo.forwardOperation == 0x0b ||
                tagFilterInfo.forwardOperation == 0x0c ||
                tagFilterInfo.forwardOperation == 0x0f ||
                tagFilterInfo.forwardOperation == 0x10 ||
                tagFilterInfo.forwardOperation == 0x11 ||
                tagFilterInfo.forwardOperation == 0x12 ||
                tagFilterInfo.forwardOperation == 0x13 ||
                tagFilterInfo.forwardOperation == 0x14 ||
                tagFilterInfo.forwardOperation == 0x1c ||
                tagFilterInfo.forwardOperation == 0x1d ||
                tagFilterInfo.forwardOperation == 0x1e ||
                tagFilterInfo.forwardOperation == 0x1f ||
                tagFilterInfo.forwardOperation == 0x20 ||
                tagFilterInfo.forwardOperation == 0x21    )
            {
                insertAniFilterTagRules(vlanFilterId,
                                        &tagFilterInfo,
                                        VLANCTL_DIRECTION_RX,
                                        &phyInfo, devInterface, vlanInterface);
            }
            else
            {
                // On rx, default is drop on vlanInterface
                // only filter packets on GEM-Index when gem is not multicast gem
                insertDefaultAniTagRules(devInterface, vlanInterface, &phyInfo,
                                         VLANCTL_DIRECTION_RX, VLANCTL_ACTION_DROP);
            }
            // if gemport is not used for multicast traffic
            // nor incident broadcast then filter on TX direction
            insertAniFilterTagRules(vlanFilterId,
                                    &tagFilterInfo,
                                    VLANCTL_DIRECTION_TX,
                                    &phyInfo, devInterface, vlanInterface);
        }
        else
        {
            // On rx, default is drop on vlanInterface
            insertDefaultTagRules(devInterface, vlanInterface, &phyInfo,
                                  VLANCTL_DIRECTION_RX, VLANCTL_ACTION_DROP);
            // if gemport is used for multicast traffic or
            // incident broadcast then filter on RX direction
            insertAniFilterTagRules(vlanFilterId,
                                    &tagFilterInfo,
                                    VLANCTL_DIRECTION_RX,
                                    &phyInfo, devInterface, vlanInterface);
        }
    }
    else
    {
        defaultRuleToAdd = TRUE;

        // On tx, if no vlan filter then accept only bi-directional unicast
        if (multicast == FALSE && broadcast == FALSE)
        {
            // On tx, if no vlan filter then setSkbMarkPort to gem id
            // (phyId) for forwarding traffic to this gem port
            insertDefaultAniTagRules(devInterface, vlanInterface, &phyInfo,
                                  VLANCTL_DIRECTION_TX, VLANCTL_ACTION_ACCEPT);
        }
    }

    //check DynamicAccessControlListTable of multicastOperationsProfile
    if (multicast == TRUE)
    {
        UINT16 vidList[MCAST_VID_LIST_MAX] = {0};
        UINT32 vidListNum = 0;
        UINT32 idx = 0;

        if (rutGpon_getMcastAclAniVidListFromUniMeId(uniOid, uniMeId, TRUE,
          &vidListNum, &vidList[0]) == CMSRET_SUCCESS)
        {
            if (vidListNum != 0)
            {
                defaultRuleToAdd = FALSE;
            }
            /* create vlanctl only in case vid is not in vidList */
            for (idx = 0; idx < vidListNum; idx++)
            {
	        /* On rx, filter with the given vid */
                vlanCtl_init();
                insertVlanIdFilterTagRules(vidList[idx],
                  VLANCTL_ACTION_ACCEPT,
                  VLANCTL_DIRECTION_RX,
                  &phyInfo, devInterface, vlanInterface);
                 vlanCtl_cleanup();
            }
        }
    }

    if (defaultRuleToAdd == TRUE)
    {
        // On rx, default is drop on vlanInterface
        // always filter packets on GEM-Index since no vlan filter
        insertDefaultAniTagRules(devInterface, vlanInterface, &phyInfo,
                          VLANCTL_DIRECTION_RX, VLANCTL_ACTION_DROP);
    }

    // interface already exists then remove old tag rules
    if (omci_rule_exist(devInterface, phyId) == CMSRET_SUCCESS)
        omci_rule_remove(devInterface, phyId);

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createAniFilterTagRules, aniMeId=%d, gemPort=%d, vlanFilterId=%d, bridgeMeId=%d, omciBridgeMeId=%d, devIfc=%s, vlanIfc=%s, ret=%d\n",
                   ctp->managedEntityId, phyId, vlanFilterId, bridgeMeId, omciBridgeMeId,
                   CHECK_INTF_PTR(devInterface), CHECK_INTF_PTR(vlanInterface), ret);

    if (tagFilterInfo.numberOfEntries > 0 && tagFilterInfo.pVlanTciTbl != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(tagFilterInfo.pVlanTciTbl);
    if (devInterface != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(devInterface);

    *aniVirtualIfaceName = vlanInterface;

    return ret;
}

static CmsRet createAniMapperFilterTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32                  uniOid,
     const UINT32                  uniMeId,
     char                          **aniVirtualIfaceName)
{
    UINT8  pbits[OMCI_FILTER_PRIO_NONE], i = 0;
    UBOOL8 multicast = rutGpon_isGemPortNetworkCtpMulticast(ctp);
    UBOOL8 broadcast = rutGpon_isGemPortNetworkCtpIncidentBroadcast(ctp);
    UBOOL8 isVeipPptpUni = rutGpon_isVeipPptpUni(uniOid, uniMeId);
    UBOOL8 hasDscpToPbit = TRUE;
    UINT32 vlanFilterId = 0, phyId = 0;
    UINT32 bridgeMeId = 0, omciBridgeMeId = 0, mapperMeId = 0;
    char *devInterface = NULL, *vlanInterface = NULL;
    OmciVlanTciFilterInfo_t tagFilterInfo;
    OmciDscpPbitMappingInfo_t dscpToPbitInfo;
    OmciPhyInfo_t phyInfo;
    CmsRet ret = CMSRET_SUCCESS;

    // initialize local variables
    for (i = OMCI_FILTER_PRIO_MIN; i <= OMCI_FILTER_PRIO_MAX; i++)
        pbits[i] = OMCI_FILTER_PRIO_NONE;
    memset(&tagFilterInfo, 0, sizeof(OmciVlanTciFilterInfo_t));

    // values of pbits are used either vlan interface is
    // already created or not ==> get their values first anyway
    if ((ret = rutGpon_getPbitsFromGemPortNeworkCtp
                  (ctp, pbits, &mapperMeId)) != CMSRET_SUCCESS)
        goto out;

    if ((ret = deleteMapperBridge(mapperMeId)) != CMSRET_SUCCESS)
        goto out;

    // initialize DSCP Pbit mapping info
    memset(&dscpToPbitInfo, 0, sizeof(OmciDscpPbitMappingInfo_t));

    if ((ret = rutGpon_getDscpToPbitFromGemPortNeworkCtp
                  (ctp, &dscpToPbitInfo)) != CMSRET_SUCCESS)
    {
        if (rutGpon_getDscpToPbitFromUniMeId
                (uniOid, uniMeId, &dscpToPbitInfo) != CMSRET_SUCCESS)
            hasDscpToPbit = FALSE;
    }

    // no valid dscp to pbit table
    if (hasDscpToPbit == FALSE)
    {
        dscpToPbitInfo.defaultPBitMarking = 0;
        dscpToPbitInfo.unmarkedFrameOption = OMCI_UNMARKED_FRAME_NONE;
    }

    if ((ret = rutGpon_getIndexFromGemPortNetworkCtp(ctp, &phyId)) != CMSRET_SUCCESS)
        goto out;

    if ((ret = rutGpon_getBridgeMeIdFromUniMeId(uniOid, uniMeId,
                                                &omciBridgeMeId)) != CMSRET_SUCCESS)
        goto out;

    // only assign bridgeMeId to real omciBridgeMeId when UNI is NOT VEIP
    // since VEIP should connected to default bridge br0
    if (uniOid != MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT && !isVeipPptpUni)
    {
        bridgeMeId = omciBridgeMeId;
    }

    rutGpon_getInterfaceName(OMCI_PHY_GPON, bridgeMeId, &devInterface);
    // create devInterface if it does not exist
    if (rutGpon_isInterfaceExisted(devInterface) == FALSE)
    {
        if ((ret = createAniInterface(devInterface)) != CMSRET_SUCCESS)
            goto out;
    }

    // On rx, default is drop on devInterface
    phyInfo.phyId = phyId;
    phyInfo.phyType = OMCI_PHY_GPON;
    phyInfo.gemPortType = getGemPortType(broadcast, multicast);

    if (omci_rule_exist(GPON_DEF_IFNAME, phyId) != CMSRET_SUCCESS || multicast)
        insertDefaultAniTagRules(GPON_DEF_IFNAME, devInterface, &phyInfo,
                                 VLANCTL_DIRECTION_RX, VLANCTL_ACTION_DROP);

    rutGpon_getVirtualInterfaceName(OMCI_PHY_GPON, bridgeMeId, bridgeMeId, &vlanInterface);

    // interface does not existed yet then
    // create new interface and link it to bridge
    if (rutGpon_isInterfaceExisted(vlanInterface) == FALSE)
    {
        if ((ret = createVlanInterface(bridgeMeId, 0, multicast,
                                       devInterface, vlanInterface)) != CMSRET_SUCCESS)
            goto out;
    }
    else
    {
        // interface already exists then copy its current
        // tag rules so that they can be deleted later
        if (omci_rule_exist(devInterface, phyId) == CMSRET_SUCCESS)
            omci_rule_copy(devInterface, phyId);
    }

    // Setup TPID table for ANI interface
    setTpidTable(devInterface, C_TAG_TPID, S_TAG_TPID, D_TAG_TPID);

    // if interface is multicast then update mcast info in MCPD configuration file
    // should call before addInterfaceToServiceBridge()
    if (multicast == TRUE)
        _owapi_rut_updateMcastCtrlIfNames(vlanInterface);

    // only add interface to bridge  when UNI is NOT VEIP
    // since VEIP_VLAN_INTERFACE is stacked to VEIP_DEV_INTERFACE
    // and VEIP_PHY_INTERFACE instead of connecting through bridge
    if (uniOid != MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT && !isVeipPptpUni)
    {
        if ((ret = addInterfaceToServiceBridge(devInterface, vlanInterface,
                                               bridgeMeId)) != CMSRET_SUCCESS)
            goto out;
    }

    // if unmarked frame uses DSCP to Pbit mapping then setDscpToPbitsTable
    if (dscpToPbitInfo.unmarkedFrameOption == OMCI_UNMARKED_FRAME_DSCP_TO_PBIT)
        setDscpToPbitsTable(dscpToPbitInfo.dscpToPbit, devInterface);

    // always use real omciBridgeMeId (instead of bridgeMeId) to get vlan info
    if (rutGpon_getVlanFilterMeIdFromGemPortNetworkCtpMeId(ctp->managedEntityId,
                                   omciBridgeMeId,
                                   &vlanFilterId) == CMSRET_SUCCESS)
    {
        if (rutGpon_allocTagFilterInfo(ctp->managedEntityId, omciBridgeMeId,
                                       &tagFilterInfo) == CMSRET_SUCCESS)
        {
            if (multicast == FALSE && broadcast == FALSE)
            {
                if (tagFilterInfo.forwardOperation == 0x03 ||
                    tagFilterInfo.forwardOperation == 0x04 ||
                    tagFilterInfo.forwardOperation == 0x07 ||
                    tagFilterInfo.forwardOperation == 0x08 ||
                    tagFilterInfo.forwardOperation == 0x0b ||
                    tagFilterInfo.forwardOperation == 0x0c ||
                    tagFilterInfo.forwardOperation == 0x0f ||
                    tagFilterInfo.forwardOperation == 0x10 ||
                    tagFilterInfo.forwardOperation == 0x11 ||
                    tagFilterInfo.forwardOperation == 0x12 ||
                    tagFilterInfo.forwardOperation == 0x13 ||
                    tagFilterInfo.forwardOperation == 0x14 ||
                    tagFilterInfo.forwardOperation == 0x1c ||
                    tagFilterInfo.forwardOperation == 0x1d ||
                    tagFilterInfo.forwardOperation == 0x1e ||
                    tagFilterInfo.forwardOperation == 0x1f ||
                    tagFilterInfo.forwardOperation == 0x20 ||
                    tagFilterInfo.forwardOperation == 0x21    )
                {
                    insertAniMapperFilterTagRules(pbits,
                                                  &dscpToPbitInfo,
                                                  vlanFilterId,
                                                  &tagFilterInfo,
                                                  VLANCTL_DIRECTION_RX,
                                                  &phyInfo, devInterface, vlanInterface);
                }
                else
                {
                    // On rx, default is drop on vlanInterface
                    // only filter packets on GEM-Index when gem is not multicast gem
                    insertDefaultAniTagRules(devInterface, vlanInterface, &phyInfo,
                                          VLANCTL_DIRECTION_RX, VLANCTL_ACTION_DROP);
                }
                // if gemport is not used for multicast traffic
                // nor incident broadcast then filter on TX direction
                insertAniMapperFilterTagRules(pbits,
                                              &dscpToPbitInfo,
                                              vlanFilterId,
                                              &tagFilterInfo,
                                              VLANCTL_DIRECTION_TX,
                                              &phyInfo, devInterface, vlanInterface);
            }
            else
            {
                // On rx, default is drop on vlanInterface
                insertDefaultTagRules(devInterface, vlanInterface, &phyInfo,
                                      VLANCTL_DIRECTION_RX, VLANCTL_ACTION_DROP);
                // if gemport is used for multicast traffic or
                // incident broadcast then filter on RX direction
                insertAniMapperFilterTagRules(pbits,
                                              &dscpToPbitInfo,
                                              vlanFilterId,
                                              &tagFilterInfo,
                                              VLANCTL_DIRECTION_RX,
                                              &phyInfo, devInterface, vlanInterface);
            }
        }
    }
    else
    {
        // On rx, default is drop on vlanInterface
        // always filter packets on GEM-Index since no vlan filter
        insertDefaultAniTagRules(devInterface, vlanInterface, &phyInfo,
                          VLANCTL_DIRECTION_RX, VLANCTL_ACTION_DROP);

        // don't configure tx rule on multicast or broadcast gem
        if (multicast == FALSE && broadcast == FALSE)
            insertAniMapperTagRules(pbits, &dscpToPbitInfo, &phyInfo, devInterface, vlanInterface);
    }

    // interface already exists then remove old tag rules
    if (omci_rule_exist(devInterface, phyId) == CMSRET_SUCCESS)
        omci_rule_remove(devInterface, phyId);

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createAniMapperFilterTagRules, aniMeId=%d, gemPort=%d, vlanFilterId=%d, bridgeMeId=%d, omciBridgeMeId=%d, mapperMeId=%d, phyId=%d, devIfc=%s, vlanIfc=%s, ret=%d\n",
                   ctp->managedEntityId, phyId, vlanFilterId, bridgeMeId, omciBridgeMeId, mapperMeId, phyId,
                   CHECK_INTF_PTR(devInterface), CHECK_INTF_PTR(vlanInterface), ret);

    if (tagFilterInfo.numberOfEntries > 0 && tagFilterInfo.pVlanTciTbl != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(tagFilterInfo.pVlanTciTbl);
    if (devInterface != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(devInterface);

    *aniVirtualIfaceName = vlanInterface;

    return ret;
}

static CmsRet createUniTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32                  reachableOid,
     const UINT32                  reachableMeId,
     const OmciMapFilterModelType  type,
     char                          **uniVirtualIfaceName)
{
    CmsRet ret = CMSRET_SUCCESS;

    switch (type)
    {
        case OMCI_MF_MODEL_1_MAP:
        case OMCI_MF_MODEL_N_MAP:
            createUniMapperTagRules(ctp, reachableOid, reachableMeId, uniVirtualIfaceName);
            break;
        case OMCI_MF_MODEL_1_FILTER:
        case OMCI_MF_MODEL_N_FILTER:
            createUniFilterTagRules(ctp, reachableOid, reachableMeId, uniVirtualIfaceName);
            break;
        case OMCI_MF_MODEL_1_MAP_FILTER:
        case OMCI_MF_MODEL_N_MAP_FILTER:
            createUniFilterTagRules(ctp, reachableOid, reachableMeId, uniVirtualIfaceName);
            break;
        default:
            break;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createUniTagRules, aniMeId=%d, reachableOid=%d, reachableMeId=%d, type=%d, ret=%d\n",
                   ctp->managedEntityId, reachableOid, reachableMeId, type, ret);

    return ret;
}

static CmsRet createIpHostTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32                  reachableOid,
     const UINT32                  reachableMeId,
     const OmciMapFilterModelType  type)
{
    CmsRet ret = CMSRET_SUCCESS;

    switch (type)
    {
        case OMCI_MF_MODEL_1_MAP:
        case OMCI_MF_MODEL_N_MAP:
            createIpHostMapperTagRules(ctp, reachableOid, reachableMeId, type);
            break;
        case OMCI_MF_MODEL_1_FILTER:
        case OMCI_MF_MODEL_N_FILTER:
            createIpHostFilterTagRules(ctp, reachableOid, reachableMeId, type);
            break;
        case OMCI_MF_MODEL_1_MAP_FILTER:
        case OMCI_MF_MODEL_N_MAP_FILTER:
            createIpHostFilterTagRules(ctp, reachableOid, reachableMeId, type);
            break;
        default:
            break;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createIpHostTagRules, aniMeId=%d, reachableOid=%d, reachableMeId=%d, type=%d, ret=%d\n",
                   ctp->managedEntityId, reachableOid, reachableMeId, type, ret);

    return ret;
}

static CmsRet createVeipTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32                  reachableOid,
     const UINT32                  reachableMeId,
     const OmciMapFilterModelType  type)
{
    CmsRet ret = CMSRET_SUCCESS;

    switch (type)
    {
        case OMCI_MF_MODEL_1_MAP:
        case OMCI_MF_MODEL_N_MAP:
            createVeipMapperTagRules(ctp, reachableOid, reachableMeId);
            break;
        case OMCI_MF_MODEL_1_FILTER:
        case OMCI_MF_MODEL_N_FILTER:
            createVeipFilterTagRules(ctp, reachableOid, reachableMeId);
            break;
        case OMCI_MF_MODEL_1_MAP_FILTER:
        case OMCI_MF_MODEL_N_MAP_FILTER:
            createVeipFilterTagRules(ctp, reachableOid, reachableMeId);
            break;
        default:
            break;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createVeipTagRules, aniMeId=%d, reachableOid=%d, reachableMeId=%d, type=%d, ret=%d\n",
                   ctp->managedEntityId, reachableOid, reachableMeId, type, ret);

    return ret;
}

static CmsRet createAniTagRules
    (const GemPortNetworkCtpObject *ctp,
     const UINT32                  reachableOid,
     const UINT32                  reachableMeId,
     const OmciMapFilterModelType  type,
     char                          **aniVirtualIfaceName)
{
    CmsRet ret = CMSRET_SUCCESS;

    switch (type)
    {
        case OMCI_MF_MODEL_1_MAP:
        case OMCI_MF_MODEL_N_MAP:
            createAniMapperTagRules(ctp, reachableOid, reachableMeId, aniVirtualIfaceName);
            break;
        case OMCI_MF_MODEL_1_FILTER:
        case OMCI_MF_MODEL_N_FILTER:
            createAniFilterTagRules(ctp, reachableOid, reachableMeId, aniVirtualIfaceName);
            break;
        case OMCI_MF_MODEL_1_MAP_FILTER:
        case OMCI_MF_MODEL_N_MAP_FILTER:
            createAniMapperFilterTagRules(ctp, reachableOid, reachableMeId, aniVirtualIfaceName);
            break;
        default:
            break;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> createAniTagRules, aniMeId=%d, reachableOid=%d, reachableMeId=%d, type=%d, ret=%d\n",
                   ctp->managedEntityId, reachableOid, reachableMeId, type, ret);

    return ret;
}

//=======================  Public GPON functions ========================

CmsRet rutGpon_getInterfaceName
    (const OmciPhyType phyType,
     const UINT32      phyId,
     char              **interfaceName)
{
    char ifName[CMS_IFNAME_LENGTH];
    CmsRet ret = CMSRET_SUCCESS;

    switch (phyType)
    {
        case OMCI_PHY_ETHERNET:
#ifndef G9991
            snprintf(ifName, sizeof(ifName), "eth%d", phyId);
#else
            snprintf(ifName, sizeof(ifName), "sid%d", phyId);
#endif
            break;
        case OMCI_PHY_MOCA:
            snprintf(ifName, sizeof(ifName), "moca%d", phyId);
            break;
        case OMCI_PHY_WIRELESS:
            snprintf(ifName, sizeof(ifName), "wl%d", phyId);
            break;
        case OMCI_PHY_POTS:
            snprintf(ifName, sizeof(ifName), "voip%d", phyId);
            break;
        case OMCI_PHY_GPON:
            snprintf(ifName, sizeof(ifName), "gpon%d", phyId);
            break;
        default:
            ret = CMSRET_INVALID_ARGUMENTS;
            break;
    }

    if (ret == CMSRET_SUCCESS)
        CMSMEM_REPLACE_STRING_FLAGS(*interfaceName, ifName, 0);

    return ret;
}

CmsRet rutGpon_getVirtualInterfaceName
    (const OmciPhyType phyType,
     const UINT32      phyId,
     const UINT32      devId,
     char              **virtualName)
{
    char ifName[CMS_IFNAME_LENGTH];
    CmsRet ret = CMSRET_SUCCESS;

    memset(ifName, 0, CMS_IFNAME_LENGTH);

    switch (phyType)
    {
        case OMCI_PHY_ETHERNET:
#ifndef G9991
            snprintf(ifName, sizeof(ifName), "eth%d.%d", phyId, devId);
#else
            snprintf(ifName, sizeof(ifName), "sid%d.%d", phyId, devId);
#endif
            break;
        case OMCI_PHY_MOCA:
            snprintf(ifName, sizeof(ifName), "moca%d.%d", phyId, devId);
            break;
        case OMCI_PHY_WIRELESS:
            snprintf(ifName, sizeof(ifName), "wl%d.%d", phyId, devId);
            break;
        case OMCI_PHY_POTS:
            snprintf(ifName, sizeof(ifName), "voip%d.%d", phyId, devId);
            break;
        case OMCI_PHY_GPON:
            snprintf(ifName, sizeof(ifName), "gpon%d.%d", phyId, devId);
            break;
        default:
            ret = CMSRET_INVALID_ARGUMENTS;
            break;
    }

    if (ret == CMSRET_SUCCESS)
        CMSMEM_REPLACE_STRING_FLAGS(*virtualName, ifName, 0);

    return ret;
}

CmsRet rutGpon_getUniVirtualInterfaceName
    (const GemPortNetworkCtpObject *ctp,
     const UINT32                  uniOid,
     const UINT32                  uniMeId,
     const OmciMapFilterModelType  type,
     char                          **uniVirtualIfaceName)
{
    OmciPhyInfo_t phyInfo;
    UINT32 devId = 0;
    UINT8  pbits[OMCI_FILTER_PRIO_NONE];
    CmsRet ret = CMSRET_SUCCESS;

    ret = rutGpon_getUniPhyInfo(uniOid, uniMeId, &phyInfo);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("rutGpon_getUniPhyInfo() failed, UNI (%d:%d)",
          uniOid, uniMeId);
        goto out;
    }

    switch (type)
    {
        case OMCI_MF_MODEL_1_MAP:
        case OMCI_MF_MODEL_N_MAP:
            if ((ret = rutGpon_getPbitsFromGemPortNeworkCtp
                          (ctp, pbits, &devId)) != CMSRET_SUCCESS)
                goto out;

            break;
        case OMCI_MF_MODEL_1_FILTER:
        case OMCI_MF_MODEL_N_FILTER:
        case OMCI_MF_MODEL_1_MAP_FILTER:
        case OMCI_MF_MODEL_N_MAP_FILTER:
            if ((ret = rutGpon_getBridgeMeIdFromUniMeId(uniOid, uniMeId,
                                                        &devId)) != CMSRET_SUCCESS)
                goto out;
            break;
        default:
            goto out;
    }

    rutGpon_getVirtualInterfaceName(phyInfo.phyType, phyInfo.phyId, devId,
      uniVirtualIfaceName);

out:
    return ret;
}

CmsRet rutGpon_createRules
    (const GemPortNetworkCtpObject *ctp,
     const UBOOL8                  isNewUni,
     const UINT32                  checkOid,
     const UINT32                  checkMeId,
     const UINT32                  reachableOid,
     const UINT32                  reachableMeId,
     const OmciMapFilterModelType  type)
{
    UBOOL8 isUniPort = FALSE;
    UBOOL8 isVeipPptpUni = rutGpon_isVeipPptpUni(reachableOid, reachableMeId);
    UINT32 portMeId = 0;
    char *aniVirtualIfaceName = NULL;
    char *uniVirtualIfaceName = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    // determine port at ANI or UNI side
    if (checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA)
    {
        if ((ret = rutGpon_getBridgePortMeIdFromUniMeId(reachableOid, reachableMeId,
                                                        &portMeId)) != CMSRET_SUCCESS)
            goto out;
        if (portMeId == checkMeId)
            isUniPort = TRUE;
    }

    // move createAniTagRules before createUniTagRules
    // since Veip interface uses Ani interface as its devInterface

    ret = createAniTagRules(ctp, reachableOid, reachableMeId, type, &aniVirtualIfaceName);

    if ((reachableOid == MDMOID_PPTP_ETHERNET_UNI && !isVeipPptpUni) ||
        reachableOid == MDMOID_PPTP_MOCA_UNI)
    {
        if (isUniVlanInterfaceExisted(reachableOid, reachableMeId, type) == FALSE)
        {
            ret = createUniTagRules(ctp, reachableOid, reachableMeId, type, &uniVirtualIfaceName);
        }
        // else PPTP or bridge port at UNI side is created then create it
        // since MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA
        // uses MDMOID_PPTP_ETHERNET_UNI or MDMOID_PPTP_MOCA_UNI,
        // and MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA might use itself,
        // and MDMOID_VLAN_TAGGING_FILTER_DATA uses
        // MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA to verify completed path
        // only create UNI rules one when UNI is the new one
        // since multiple CTP can connect to the same UNI
        else if ((isNewUni == TRUE) &&
                 ((checkOid == MDMOID_PPTP_ETHERNET_UNI) ||
                  (checkOid == MDMOID_PPTP_MOCA_UNI) ||
                  (checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) ||
                  ((checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA) && (isUniPort == TRUE))))
        {
            ret = createUniTagRules(ctp, reachableOid, reachableMeId, type, &uniVirtualIfaceName);
        }

        procUniDsIgmpMcastTag(ctp, reachableOid, reachableMeId, FALSE, VLANCTL_DIRECTION_TX);

        if(aniVirtualIfaceName)
        {
            if (uniVirtualIfaceName == NULL &&
                (reachableOid == MDMOID_PPTP_ETHERNET_UNI && !isVeipPptpUni))
            {
                ret = rutGpon_getUniVirtualInterfaceName(ctp, reachableOid, reachableMeId,
                          type, &uniVirtualIfaceName);
            }
            if (uniVirtualIfaceName != NULL)
            {
                saveVlanFlowstoCreate(aniVirtualIfaceName, uniVirtualIfaceName);
            }
        }
    }
    else if (reachableOid == MDMOID_IP_HOST_CONFIG_DATA ||
             reachableOid == MDMOID_IPV6_HOST_CONFIG_DATA)
    {
        if (isIpHostVlanInterfaceExisted(reachableOid, reachableMeId, type) == FALSE)
            ret = createIpHostTagRules(ctp, reachableOid, reachableMeId, type);
        // else if IpHost or bridge port at UNI side is created then create it
        // since MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA
        // uses MDMOID_IP_HOST_CONFIG_DATA, MDMOID_IPV6_HOST_CONFIG_DATA,
        // and MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA might use itself,
        // and MDMOID_VLAN_TAGGING_FILTER_DATA uses
        // MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA to verify completed path
        // only create UNI rules one when UNI is the new one
        // since multiple CTP can connect to the same UNI
        else if ((isNewUni == TRUE) &&
                 ((checkOid == MDMOID_IP_HOST_CONFIG_DATA) ||
                  (checkOid == MDMOID_IPV6_HOST_CONFIG_DATA) ||
                  (checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) ||
                  ((checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA) && (isUniPort == TRUE))))
            ret = createIpHostTagRules(ctp, reachableOid, reachableMeId, type);
    }
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
    else if (reachableOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT || isVeipPptpUni)
    {
        char veipVlanName[CMS_IFNAME_LENGTH] = {0};

        getVeipVlanName(veipVlanName);

        if (rutGpon_isInterfaceExisted(veipVlanName) == FALSE)
        {
            ret = createVeipTagRules(ctp, reachableOid, reachableMeId, type);
        }
        // else VEIP or bridge port at UNI side is created then create it
        // since MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA
        // uses MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
        // and MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA might use itself,
        // and MDMOID_VLAN_TAGGING_FILTER_DATA uses
        // MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA to verify completed path
        // only create UNI rules one when UNI is the new one
        // since multiple CTP can connect to the same UNI
        else if ((isNewUni == TRUE) &&
                 ((checkOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT) ||
                  (rutGpon_isVeipPptpUni(checkOid, checkMeId)) ||
                  (checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) ||
                  ((checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA) && (isUniPort == TRUE))))
        {
            ret = createVeipTagRules(ctp, reachableOid, reachableMeId, type);
        }

        procUniDsIgmpMcastTag(ctp, reachableOid, reachableMeId, TRUE, VLANCTL_DIRECTION_RX);

        /* For GponRg Full Omci only.  Need to create this veip (First parameter set to TRUE) */
        rutRgFull_configVeip(TRUE, reachableOid, reachableMeId,  type, ctp);
    }
#endif /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

    ret = CMSRET_SUCCESS;

out:
    if (aniVirtualIfaceName != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(aniVirtualIfaceName);
    if (uniVirtualIfaceName != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(uniVirtualIfaceName);

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> rutGpon_createRules, aniMeId=%d, isNewUni=%d, checkOid=%d, checkMeId=%d, reachableOid=%d, reachableMeId=%d, type=%d, ret=%d\n",
                   ctp->managedEntityId, isNewUni, checkOid, checkMeId, reachableOid, reachableMeId, type, ret);

    return ret;
}

CmsRet rutGpon_editRules
    (const GemPortNetworkCtpObject *ctp,
     const UBOOL8                  isNewUni,
     const UINT32                  checkOid,
     const UINT32                  checkMeId,
     const UINT32                  reachableOid,
     const UINT32                  reachableMeId,
     const OmciMapFilterModelType  type)
{
    UBOOL8 isUniPort = FALSE;
    UBOOL8 isVeipPptpUni = rutGpon_isVeipPptpUni(reachableOid, reachableMeId);
    UINT32 portMeId = 0;
    char *aniVirtualIfaceName = NULL;
    char *uniVirtualIfaceName = NULL;
    CmsRet ret = CMSRET_INTERNAL_ERROR;

    // For changing existed tag rules, just look for MEs
    // that can effect the VLAN tag rules. They are:
    //    * MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA
    //    * MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA
    //    * MDMOID_VLAN_TAGGING_FILTER_DATA
    //    * MDMOID_MAPPER_SERVICE_PROFILE

    // determine port at ANI or UNI side
    if (checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA)
    {
        if ((ret = rutGpon_getBridgePortMeIdFromUniMeId(reachableOid, reachableMeId,
                                                        &portMeId)) != CMSRET_SUCCESS)
            goto out;
        if (portMeId == checkMeId)
            isUniPort = TRUE;
    }

    // move createAniTagRules before createUniTagRules
    // since Veip interface uses Ani interface as its devInterface

    // if path is completed but vlan interface at ANI does not exist then create it
    if (isAniVlanInterfaceExisted(ctp, reachableOid, reachableMeId, type) == FALSE)
        ret = createAniTagRules(ctp, reachableOid, reachableMeId, type, &aniVirtualIfaceName);
    // else if mapper or bridge port at ANI side is changed then create it
    // since MDMOID_VLAN_TAGGING_FILTER_DATA uses
    // MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA to verify completed path
    else if ((checkOid == MDMOID_MAPPER_SERVICE_PROFILE) ||
             ((checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA) && (isUniPort == FALSE)))
        ret = createAniTagRules(ctp, reachableOid, reachableMeId, type, &aniVirtualIfaceName);

    if ((reachableOid == MDMOID_PPTP_ETHERNET_UNI && !isVeipPptpUni) ||
        reachableOid == MDMOID_PPTP_MOCA_UNI)
    {
        // if path is completed but vlan interface at UNI does not exist then create it
        if (isUniVlanInterfaceExisted(reachableOid, reachableMeId, type) == FALSE)
            ret = createUniTagRules(ctp, reachableOid, reachableMeId, type, &uniVirtualIfaceName);
        // else PPTP or bridge port at UNI side is created then create it
        // since MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA
        // uses MDMOID_PPTP_ETHERNET_UNI or MDMOID_PPTP_MOCA_UNI,
        // and MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA might use itself,
        // and MDMOID_VLAN_TAGGING_FILTER_DATA uses
        // MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA to verify completed path
        // only create UNI rules one when UNI is the new one
        // since multiple CTP can connect to the same UNI
        else if ((isNewUni == TRUE) &&
                 ((checkOid == MDMOID_PPTP_ETHERNET_UNI) ||
                  (checkOid == MDMOID_PPTP_MOCA_UNI) ||
                  (checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) ||
                  ((checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA) && (isUniPort == TRUE))))
            ret = createUniTagRules(ctp, reachableOid, reachableMeId, type, &uniVirtualIfaceName);

        if(aniVirtualIfaceName)
        {
            if (uniVirtualIfaceName == NULL &&
                (reachableOid == MDMOID_PPTP_ETHERNET_UNI && !isVeipPptpUni))
            {
                ret = rutGpon_getUniVirtualInterfaceName(ctp, reachableOid, reachableMeId,
                          type, &uniVirtualIfaceName);
            }
            if (uniVirtualIfaceName != NULL)
            {
                saveVlanFlowstoCreate(aniVirtualIfaceName, uniVirtualIfaceName);
            }
        }
    }
    else if (reachableOid == MDMOID_IP_HOST_CONFIG_DATA ||
             reachableOid == MDMOID_IPV6_HOST_CONFIG_DATA)
    {
        // if path is completed but vlan interface at IpHost does not exist then create it
        if (isIpHostVlanInterfaceExisted(reachableOid, reachableMeId, type) == FALSE)
            ret = createIpHostTagRules(ctp, reachableOid, reachableMeId, type);
        // else if IpHost or bridge port at UNI side is created then create it
        // since MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA
        // uses MDMOID_IP_HOST_CONFIG_DATA, MDMOID_IPV6_HOST_CONFIG_DATA
        // and MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA might use itself,
        // and MDMOID_VLAN_TAGGING_FILTER_DATA uses
        // MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA to verify completed path
        // only create UNI rules one when UNI is the new one
        // since multiple CTP can connect to the same UNI
        else if ((isNewUni == TRUE) &&
                 ((checkOid == MDMOID_IP_HOST_CONFIG_DATA) ||
                  (checkOid == MDMOID_IPV6_HOST_CONFIG_DATA) ||
                  (checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) ||
                  ((checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA) && (isUniPort == TRUE))))
            ret = createIpHostTagRules(ctp, reachableOid, reachableMeId, type);
    }
    else if (reachableOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT || isVeipPptpUni)
    {
        char veipVlanName[CMS_IFNAME_LENGTH] = {0};

        getVeipVlanName(veipVlanName);

        if (rutGpon_isInterfaceExisted(veipVlanName) == FALSE)
            ret = createVeipTagRules(ctp, reachableOid, reachableMeId, type);
        // else VEIP or bridge port at UNI side is created then create it
        // since MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA
        // uses MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
        // and MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA might use itself,
        // and MDMOID_VLAN_TAGGING_FILTER_DATA uses
        // MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA to verify completed path
        // only create UNI rules one when UNI is the new one
        // since multiple CTP can connect to the same UNI
        else if ((isNewUni == TRUE) &&
                 ((checkOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT) ||
                  (rutGpon_isVeipPptpUni(checkOid, checkMeId)) ||
                  (checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) ||
                  ((checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA) && (isUniPort == TRUE))))
            ret = createVeipTagRules(ctp, reachableOid, reachableMeId, type);

        /* For GponRg Full Omci only.  Need to create this veip (First parameter set to TRUE) */
        rutRgFull_configVeip(TRUE, reachableOid, reachableMeId,  type, ctp);
    }

    ret = CMSRET_SUCCESS;

out:
    if (aniVirtualIfaceName != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(aniVirtualIfaceName);
    if (uniVirtualIfaceName != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(uniVirtualIfaceName);

    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> rutGpon_editRules, aniMeId=%d, isNewUni=%d, checkOid=%d, checkMeId=%d, reachableOid=%d, reachableMeId=%d, type=%d, ret=%d\n",
                   ctp->managedEntityId, isNewUni, checkOid, checkMeId, reachableOid, reachableMeId, type, ret);

    return ret;
}

CmsRet rutGpon_deleteRules
    (const GemPortNetworkCtpObject *ctp,
     const UBOOL8                  isNewUni __attribute__((unused)),
     const UINT32                  checkOid,
     const UINT32                  checkMeId,
     const UINT32                  reachableOid,
     const UINT32                  reachableMeId,
     const OmciMapFilterModelType  type)
{
    UBOOL8 isUniPort = FALSE;
    UBOOL8 isVeipPptpUni = rutGpon_isVeipPptpUni(reachableOid, reachableMeId);
    CmsRet ret = CMSRET_INTERNAL_ERROR;
    UINT32 serviceNum;

    // determine port at ANI or UNI side
    if (checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA)
    {
        UINT32 portMeId = 0;
        if ((ret = rutGpon_getBridgePortMeIdFromUniMeId(reachableOid, reachableMeId,
                                                        &portMeId)) != CMSRET_SUCCESS)
            goto out;
        if (portMeId == checkMeId)
            isUniPort = TRUE;
    }

    if ((reachableOid == MDMOID_PPTP_ETHERNET_UNI && !isVeipPptpUni) ||
        reachableOid == MDMOID_PPTP_MOCA_UNI)
    {
        // MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA uses
        // MDMOID_PPTP_ETHERNET_UNI or MDMOID_PPTP_MOCA_UNI,
        // MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA might use itself,
        // and MDMOID_VLAN_TAGGING_FILTER_DATA uses
        // MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA to verify completed path
        if ((checkOid == MDMOID_PPTP_ETHERNET_UNI) ||
            (checkOid == MDMOID_PPTP_MOCA_UNI) ||
            (checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) ||
            ((checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA) && (isUniPort == TRUE)))
        {
            ret = deleteUniVlanInterface(reachableOid, reachableMeId, type);
        }
    }
    else if (reachableOid == MDMOID_IP_HOST_CONFIG_DATA ||
             reachableOid == MDMOID_IPV6_HOST_CONFIG_DATA)
    {
        // MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA uses
        // MDMOID_IP_HOST_CONFIG_DATA or MDMOID_IPV6_HOST_CONFIG_DATA
        // to verify completed path
        // MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA might use itself,
        // and MDMOID_VLAN_TAGGING_FILTER_DATA uses
        // MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA to verify completed path
        if ((checkOid == MDMOID_IP_HOST_CONFIG_DATA) ||
            (checkOid == MDMOID_IPV6_HOST_CONFIG_DATA) ||
            (checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) ||
            ((checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA) && (isUniPort == TRUE)))
        {
            ret = deleteIpHostVlanInterface(reachableOid, reachableMeId, type);
            // remove iptables rule of tcpUdp config data
            rutGpon_configTcpUdpByIpHost(reachableMeId, FALSE);
        }
    }
    else if (reachableOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT || isVeipPptpUni)
    {
        // MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA uses
        // MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT to verify completed path
        // MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA might use itself,
        // and MDMOID_VLAN_TAGGING_FILTER_DATA uses
        // MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA to verify completed path
        if ((checkOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT) ||
            (rutGpon_isVeipPptpUni(checkOid, checkMeId)) ||
            (checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) ||
            ((checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA) && (isUniPort == TRUE)))
        {
            char veipVlanName[CMS_IFNAME_LENGTH] = {0};
            UINT32 omciBridgeMeId;

            getVeipVlanName(veipVlanName);
            ret = rutGpon_getBridgeMeIdFromUniMeId(reachableOid, reachableMeId,
              &omciBridgeMeId);
            if (ret != CMSRET_SUCCESS)
            {
                omciBridgeMeId = 0;
            }
            setIgmpRateLimitOnBridge(omciBridgeMeId, DEFAULT_RG_BRIDGE_NAME,
              veipVlanName, TRUE);

            serviceNum = rutRgFull_configVeip(FALSE, reachableOid, reachableMeId, type, ctp);
            if (serviceNum == 0)
            {
                ret = deleteVeipVlanInterface();
                if (ret != CMSRET_SUCCESS)
                {
                    cmsLog_error("deleteVeipVlanInterface() failed, ret=%d", ret);
                }
            }
        }
    }

    // MDMOID_VLAN_TAGGING_FILTER_DATA uses
    // MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA to verify completed path
    // For service deletion, GEM_INTERWORKING_TP deletion occurs before GEM_PORT_NETWORK_CTP
    if ((checkOid == MDMOID_GEM_PORT_NETWORK_CTP) ||
        (checkOid == MDMOID_MAPPER_SERVICE_PROFILE) ||
        (checkOid == MDMOID_GEM_INTERWORKING_TP) ||
        ((checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA) && (isUniPort == FALSE)))
    {
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
        /* For GponRg Full Omci only. Need to delete this veip (First parameter set to FALSE) */
        if (reachableOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT || isVeipPptpUni)
        {
            serviceNum = rutRgFull_configVeip(FALSE, reachableOid, reachableMeId, type, ctp);
            if (serviceNum == 0)
            {
                ret = deleteVeipVlanInterface();
                if (ret != CMSRET_SUCCESS)
                {
                    cmsLog_error("deleteVeipVlanInterface() failed, ret=%d", ret);
                }

                ret = deleteAniInterface(ctp, reachableOid, reachableMeId);
                if (ret != CMSRET_SUCCESS)
                {
                    cmsLog_error("deleteAniInterface() failed, ret=%d", ret);
                }
            }
        }
        else
#endif /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */
        {
            ret = deleteAniInterface(ctp, reachableOid, reachableMeId);
        }
    }

    if (checkOid == MDMOID_MAC_BRIDGE_SERVICE_PROFILE)
    {
        // do nothing since service bridge is deleted in
        // rcl_bCM_MacBridgeServiceProfileObject() function
    }

    if (checkOid == MDMOID_MAPPER_SERVICE_PROFILE)
    {
        // do nothing since mapper bridge is deleted in
        // rcl_bCM_MapperServiceProfileObject() function
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_RULE,
                   "===> rutGpon_deleteRules, aniMeId=%d, isNewUni=%d, checkOid=%d, checkMeId=%d, reachableOid=%d, reachableMeId=%d, type=%d, ret=%d\n",
                   ctp->managedEntityId, isNewUni, checkOid, checkMeId, reachableOid, reachableMeId, type, ret);

    return ret;
}

void rutGpon_removeVeipRules(OmciDmGemUniPairInfo *infoP)
{
    GemPortNetworkCtpObject *gemCtpObjP = NULL;
    CmsRet ret;


    if ((infoP->uniOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT) ||
      (rutGpon_isVeipPptpUni(infoP->uniOid, infoP->uniMeId) == TRUE))
    {
        ret = get_obj_by_instance(MDMOID_GEM_PORT_NETWORK_CTP,
          (void**)&gemCtpObjP, infoP->gemMeId, 0);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Could not get gemMeId %d, ret=%d",
              infoP->gemMeId, ret);
            return;
        }

        rutRgFull_configVeip(FALSE, infoP->uniOid, infoP->uniMeId,
          infoP->modelType, gemCtpObjP);
        _cmsObj_free((void**)&gemCtpObjP);
    }
}

void rutGpon_removeAniToUniPathRules(OmciDmGemUniPairInfo *infoP)
{
    GemPortNetworkCtpObject *gemCtpObjP = NULL;
    CmsRet ret;

    ret = get_obj_by_instance(MDMOID_GEM_PORT_NETWORK_CTP,
      (void**)&gemCtpObjP, infoP->gemMeId, 0);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Could not get gemMeId %d, ret=%d",
          infoP->gemMeId, ret);
        return;
    }

    ret = rutGpon_deleteRules(gemCtpObjP, infoP->isNewUni,
      infoP->checkOid, infoP->checkMeId,
      infoP->uniOid, infoP->uniMeId,
      infoP->modelType);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("rutGpon_deleteRules() failed");
        cmsLog_error("%d:%d:%d:%d ", infoP->checkOid, infoP->checkMeId,
          infoP->uniOid, infoP->uniMeId);
    }
    _cmsObj_free((void**)&gemCtpObjP);
}

void rutGpon_deleteAllRules(void)
{
    // clean up all existed VLANCTL rules
    omci_rule_exit();

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
    // this function has to be called before deleting veip0
    // since all veip0.1, veip0.2 interfaces have to be deleted first
    omciDm_traverseGemUniLList(rutGpon_removeVeipRules);

    // clean up all existed RG WAN services
    // this function has to called after rutGpon_deleteAllVeipServices()
    omci_service_exit();
#endif   // DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1

    omciDm_traverseGemUniLList(rutGpon_removeAniToUniPathRules);
}

#endif /* DMP_X_ITU_ORG_GPON_1 */

