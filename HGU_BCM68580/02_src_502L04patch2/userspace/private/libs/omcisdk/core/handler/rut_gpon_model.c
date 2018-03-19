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

/*****************************************************************************
*    Description:
*
*      OMCI data model parsing module.
*
*****************************************************************************/

#ifdef DMP_X_ITU_ORG_GPON_1

/* ---- Include Files ----------------------------------------------------- */

#include "owsvc_api.h"
#include "owrut_api.h"
#include "me_handlers.h"
#include "ctlutils_api.h"

#ifdef SUPPORT_GPONCTL
//#include "gponctl_api.h"    // for gponCtl_getGemPort() function
#include "gponif_api.h"
#endif
#include "omci_res_dm.h"


/* ---- Private Constants and Types --------------------------------------- */

typedef struct
{
    MdmObjectId _oid;
    UINT32 managedEntityId;
} OMciObjCmnHdr_t;

typedef CmsRet (*uniObject_searchModelPath)(void *uniObj, OmciFLowDirection
  direction, OmciMapFilterModelType *type, UINT32 checkOid, UINT32 checkMeId,
  UINT32 *reachOid, UINT32 *reachMeId, UINT32 depth, UBOOL8 *inPath);


/* ---- Macro API definitions --------------------------------------------- */


/* ---- Private Function Prototypes --------------------------------------- */

CmsRet handleModelPath(const OmciObjectAction action,
  const GemPortNetworkCtpObject *ctp, const UBOOL8 isNewUni,
  const UINT32 checkOid, const UINT32 checkMeId, const UINT32 uniOid,
  const UINT32 uniMeId, const OmciMapFilterModelType type);
#ifdef OMCI_TR69_DUAL_STACK
void rutGpon_checkTr069Managemet(UINT32 ipHostMeId __attribute__((unused)));
#endif /* OMCI_TR69_DUAL_STACK */


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */

static UBOOL8 glbFoundBridge = FALSE;
#if defined(CONFIG_BCM_GPON_802_1Q_ENABLED)
static vlanDomains_t gblCurVlanDomain = {0}; /* Keeps the current switch vlan table snapshot*/
static vlanDomains_t gblNewVlanDomain = {0}; /* Keeps the new vlan table config */
#endif /* CONFIG_BCM_GPON_802_1Q_ENABLED */


//=======================  Private GPON Model functions ========================

/*****************************************************************************
*  FUNCTION:  rutGpon_genericCheckModelPath
*  PURPOSE:   Generic check model path function for all UNI types.
*  PARAMETERS:
*      action - CREATE or DELETE.
*      checkOid - check OID.
*      checkMeId - check ME id.
*      gemType - bi-directional or downstream only.
*      uniCheckOid - UNI check OID.
*      checkInPath - whether or not check "inpath".
*      uniObj - pointer to common UNI object.
*      isNewUni - whether the UNI is a "new "interface.
*      cbFunc - uniObject_searchModelPath() callback function.
*  RETURNS:
*      None.
*  NOTES:
*      This function may be used by both rutGpon_checkModelPath_Us() and
*      rutGpon_configModelPath().
*****************************************************************************/
CmsRet rutGpon_genericCheckModelPath(OmciObjectAction action,
  UINT32 checkOid, UINT32 checkMeId, OmciFLowDirection gemType,
  UINT32 uniCheckOid, UBOOL8 checkInPath, OMciObjCmnHdr_t *uniObj,
  UBOOL8 *isNewUni, uniObject_searchModelPath cbFunc)
{
    UINT32 aniOid = MDMOID_GEM_PORT_NETWORK_CTP;
    GemPortNetworkCtpObject *ctp;
    InstanceIdStack iidAni = EMPTY_INSTANCE_ID_STACK;
    OmciMapFilterModelType type = OMCI_MF_MODEL_NONE;
    UBOOL8 inPath = FALSE;
    UINT32 reachableOid = 0;
    UINT32 reachableMeId = 0;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    INIT_INSTANCE_ID_STACK(&iidAni);
    while (_cmsObj_getNextFlags(aniOid, &iidAni,
      OGF_NO_VALUE_UPDATE, (void**)&ctp) == CMSRET_SUCCESS)
    {
        if ((gemType == OMCI_FLOW_UPSTREAM) &&
          (ctp->direction == OMCI_FLOW_DOWNSTREAM))
        {
            goto next;
        }
        if ((gemType == OMCI_FLOW_DOWNSTREAM) &&
          (ctp->direction != OMCI_FLOW_DOWNSTREAM))
        {
            goto next;
        }

        type = OMCI_MF_MODEL_NONE;
        inPath = FALSE;
        reachableOid = aniOid;
        reachableMeId = ctp->managedEntityId;
        // mark bridge is not found before searching path
        glbFoundBridge = FALSE;

        if ((reachableOid == checkOid) && (reachableMeId != checkMeId))
        {
            goto next;
        }

        // searching path from uniObj to aniObj
        ret = cbFunc(uniObj, OMCI_FLOW_UPSTREAM, &type, checkOid,
          checkMeId, &reachableOid, &reachableMeId, 0,
          &inPath);

        omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
          "===> %s, reachableOid=%d, reachableMeId=%d, "
          "uniMeId=%d, aniMeId=%d, type=%d, "
          "checkOid=%d, checkMeId=%d, inPath=%d, depth=%d, "
          "ret=%d\n\n", __FUNCTION__,
          reachableOid, reachableMeId, uniObj->managedEntityId,
          ctp->managedEntityId, type, checkOid,
          checkMeId, inPath, 0, ret);

        if (ret == CMSRET_SUCCESS)
        {
            if (((checkInPath == TRUE) && (inPath == TRUE)) ||
              (checkInPath == FALSE))
            {
                omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                  "===> ANI to UNI path is completed, and object is in this path\n\n");

                handleModelPath(action,
                  ctp, *isNewUni,
                  checkOid, checkMeId,
                  uniCheckOid, uniObj->managedEntityId,
                  type);

                // if there's FIRST completed path from UNI to ANI then set
                // isNewUni to FALSE to avoid creating filters at UNI
                // side again since 1 UNI can connect to multiple ANIs
                if (rutGpon_isGemPortNetworkCtpMulticast(ctp) == FALSE)
                {
                    *isNewUni = FALSE;
                }
            }
        }
next:
        _cmsObj_free((void**)&ctp);

        // if IpHostConfigDataObject or Ipv6HostConfigDataObject finds its
        // bridge with any GemPortNetworkCtpObject then stop searching for
        // others since ip host (v4 or v6) already created all flows rules
        // that are associated with it
        if (((uniCheckOid == MDMOID_IPV6_HOST_CONFIG_DATA) ||
          (uniCheckOid == MDMOID_IP_HOST_CONFIG_DATA)) &&
          (*isNewUni == FALSE))
        {
            if (action == OMCI_ACTION_DELETE)
            {
                break;
            }
#ifdef OMCI_TR69_DUAL_STACK
            else if (action == OMCI_ACTION_CREATE)
            {
                //iphost path is complete, check tr069 me
                rutGpon_checkTr069Managemet(uniObj->managedEntityId);
            }
#endif
        }
    }

    return ret;
}

CmsRet PptpMocaUniObject_searchModelPath
    (PptpMocaUniObject *pptpMoca,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // Since this object is currently in search path,
    // if this object is the one that needs to be checked
    // then specify the checked object is in search path
    if (checkOid == MDMOID_PPTP_MOCA_UNI &&
        checkMeId == pptpMoca->managedEntityId)
        *inPath = TRUE;

    // store information of the reachable object
    // except when reachableMeId has specific
    // GemPortNetworkCtp object to be reached
    // since they're used to search path from UNI to ANI
    if (direction == OMCI_FLOW_DOWNSTREAM ||
        *reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
    {
        *reachableOid = MDMOID_PPTP_MOCA_UNI;
        *reachableMeId = pptpMoca->managedEntityId;
    }

    // if direction is downstream then return success
    // since Mapping Filter model is ended at
    // PptpMocaUniObject for downstream direction
    if (direction == OMCI_FLOW_DOWNSTREAM)
    {
        ret = CMSRET_SUCCESS;
        omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> PptpMocaUniObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", pptpMoca->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath, depth, ret);
        return ret;
    }

    // if direction is upstream then search for either
    // MacBridgePortConfigDataObject or
    // MapperServiceProfileObject or
    // that has its tpPointer match with manged entity Id

    MacBridgePortConfigDataObject *port = NULL;
    MapperServiceProfileObject *mapper = NULL;

    // search MacBridgePortConfigDataObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        found = ((port->tpPointer == pptpMoca->managedEntityId) &&
                 (port->tpType == OMCI_BP_TP_PPTP_MOCA));
        // if MacBridgePortConfigDataObject is found then
        if (found == TRUE)
        {
            // include bridge service to model type
            *type = OMCI_MF_MODEL_1_FILTER;
            // pass the search to MacBridgePortConfigDataObject
            ret = MacBridgePortConfigDataObject_searchModelPath
                      (port, direction, type, checkOid, checkMeId,
                       reachableOid, reachableMeId, depth + 1, inPath);
        }
        _cmsObj_free((void **) &port);
    }
    INIT_INSTANCE_ID_STACK(&iidStack);
    // search MapperServiceProfileObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mapper) == CMSRET_SUCCESS))
    {
        found = ((mapper->tpPointer == pptpMoca->managedEntityId) &&
                 (mapper->tpType == OMCI_MS_TP_PPTP_MOCA));
        // if MapperServiceProfileObject is found then
        if (found == TRUE)
        {
            // include mapper service to model type
            *type = OMCI_MF_MODEL_1_MAP;
            // pass the search to MapperServiceProfileObject
            ret = MapperServiceProfileObject_searchModelPath
                      (mapper, direction, type, checkOid, checkMeId,
                       reachableOid, reachableMeId, depth + 1, inPath);
        }
        _cmsObj_free((void **) &mapper);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> PptpMocaUniObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", pptpMoca->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

CmsRet PptpEthernetUniObject_searchModelPath
    (PptpEthernetUniObject *pptpEth,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // Since this object is currently in search path,
    // if this object is the one that needs to be checked
    // then specify the checked object is in search path
    // VlanTaggingOperationConfigurationDataObject can
    // implicitly link to PptpEthernetUniObject when
    // they have the same managedEntityId
    if ((checkOid == MDMOID_PPTP_ETHERNET_UNI ||
         checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) &&
        checkMeId == pptpEth->managedEntityId)
        *inPath = TRUE;

    // store information of the reachable object
    // except when reachableMeId has specific
    // GemPortNetworkCtp object to be reached
    // since they're used to search path from UNI to ANI
    if (direction == OMCI_FLOW_DOWNSTREAM ||
       *reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
    {
        *reachableOid = MDMOID_PPTP_ETHERNET_UNI;
        *reachableMeId = pptpEth->managedEntityId;
    }

    // if direction is downstream then return success
    // since Mapping Filter model is ended at
    // PptpEthernetUniObject for downstream direction
    if (direction == OMCI_FLOW_DOWNSTREAM)
    {
        ret = CMSRET_SUCCESS;
        omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> PptpEthernetUniObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", pptpEth->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);
        return ret;
    }

    // if direction is upstream then search for either
    // MacBridgePortConfigDataObject or
    // MapperServiceProfileObject or
    // that has its tpPointer match with manged entity Id

    MacBridgePortConfigDataObject *port = NULL;
    MapperServiceProfileObject *mapper = NULL;

    // search MacBridgePortConfigDataObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        found = ((port->tpPointer == pptpEth->managedEntityId) &&
                 (port->tpType == OMCI_BP_TP_PPTP_ETH));
        // if MacBridgePortConfigDataObject is found then
        if (found == TRUE)
        {
            // include bridge service to model type
            *type = OMCI_MF_MODEL_1_FILTER;
            // pass the search to MacBridgePortConfigDataObject
            ret = MacBridgePortConfigDataObject_searchModelPath
                      (port, direction, type, checkOid, checkMeId,
                       reachableOid, reachableMeId, depth + 1, inPath);
        }
        _cmsObj_free((void **) &port);
    }

    INIT_INSTANCE_ID_STACK(&iidStack);
    // search MapperServiceProfileObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mapper) == CMSRET_SUCCESS))
    {
//        found = ((mapper->tpPointer == pptpEth->managedEntityId) &&
//                 (mapper->tpType == OMCI_MS_TP_PPTP_ETH));
        found = (mapper->tpPointer == pptpEth->managedEntityId);
        // if MapperServiceProfileObject is found then
        if (found == TRUE)
        {
            // include mapper service to model type
            *type = OMCI_MF_MODEL_1_MAP;
            // pass the search to MapperServiceProfileObject
            ret = MapperServiceProfileObject_searchModelPath
                      (mapper, direction, type, checkOid, checkMeId,
                       reachableOid, reachableMeId, depth + 1, inPath);
        }
        _cmsObj_free((void **) &mapper);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> PptpEthernetUniObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", pptpEth->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

CmsRet IpHostConfigDataObject_searchModelPath
    (IpHostConfigDataObject *ipHost,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // Since this object is currently in search path,
    // if this object is the one that needs to be checked
    // then specify the checked object is in search path
    // VlanTaggingOperationConfigurationDataObject can
    // implicitly link to IpHostConfigDataObject when
    // they have the same managedEntityId
    if ((checkOid == MDMOID_IP_HOST_CONFIG_DATA ||
         checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) &&
        checkMeId == ipHost->managedEntityId)
        *inPath = TRUE;

    // store information of the reachable object
    // except when reachableMeId has specific
    // GemPortNetworkCtp object to be reached
    // since they're used to search path from UNI to ANI
    if (direction == OMCI_FLOW_DOWNSTREAM ||
       *reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
    {
        *reachableOid = MDMOID_IP_HOST_CONFIG_DATA;
        *reachableMeId = ipHost->managedEntityId;
    }

    // if direction is downstream then return success
    // since Mapping Filter model is ended at
    // IpHostConfigDataObject for downstream direction
    if (direction == OMCI_FLOW_DOWNSTREAM)
    {
        ret = CMSRET_SUCCESS;
        omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> IpHostConfigDataObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", ipHost->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);
        return ret;
    }

    // if direction is upstream then search for either
    // MacBridgePortConfigDataObject or
    // MapperServiceProfileObject
    // that has its tpPointer match with manged entity Id

    MacBridgePortConfigDataObject *port = NULL;
    MapperServiceProfileObject *mapper = NULL;

    // search MacBridgePortConfigDataObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        found = ((port->tpPointer == ipHost->managedEntityId) &&
                 (port->tpType == OMCI_BP_TP_IP_HOST));
        // if MacBridgePortConfigDataObject is found then
        if (found == TRUE)
        {
            // include bridge service to model type
            *type = OMCI_MF_MODEL_1_FILTER;
            // pass the search to MacBridgePortConfigDataObject
            ret = MacBridgePortConfigDataObject_searchModelPath
                      (port, direction, type, checkOid, checkMeId,
                       reachableOid, reachableMeId, depth + 1, inPath);
        }
        _cmsObj_free((void **) &port);
    }

    INIT_INSTANCE_ID_STACK(&iidStack);
    // search MapperServiceProfileObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mapper) == CMSRET_SUCCESS))
    {
        found = ((mapper->tpPointer == ipHost->managedEntityId) &&
                 (mapper->tpType == OMCI_MS_TP_IP_HOST));
        // if MapperServiceProfileObject is found then
        if (found == TRUE)
        {
            // include mapper service to model type
            *type = OMCI_MF_MODEL_1_MAP;
            // pass the search to MapperServiceProfileObject
            ret = MapperServiceProfileObject_searchModelPath
                      (mapper, direction, type, checkOid, checkMeId,
                       reachableOid, reachableMeId, depth + 1, inPath);
        }
        _cmsObj_free((void **) &mapper);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> IpHostConfigDataObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", ipHost->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

CmsRet Ipv6HostConfigDataObject_searchModelPath
    (Ipv6HostConfigDataObject *ipHost,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // Since this object is currently in search path,
    // if this object is the one that needs to be checked
    // then specify the checked object is in search path
    // VlanTaggingOperationConfigurationDataObject can
    // implicitly link to IpHostConfigDataObject when
    // they have the same managedEntityId
    if ((checkOid == MDMOID_IPV6_HOST_CONFIG_DATA ||
         checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) &&
        checkMeId == ipHost->managedEntityId)
        *inPath = TRUE;

    // store information of the reachable object
    // except when reachableMeId has specific
    // GemPortNetworkCtp object to be reached
    // since they're used to search path from UNI to ANI
    if (direction == OMCI_FLOW_DOWNSTREAM ||
       *reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
    {
        *reachableOid = MDMOID_IPV6_HOST_CONFIG_DATA;
        *reachableMeId = ipHost->managedEntityId;
    }

    // if direction is downstream then return success
    // since Mapping Filter model is ended at
    // Ipv6HostConfigDataObject for downstream direction
    if (direction == OMCI_FLOW_DOWNSTREAM)
    {
        ret = CMSRET_SUCCESS;
        omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> Ipv6HostConfigDataObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", ipHost->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);
        return ret;
    }

    // if direction is upstream then search for either
    // MacBridgePortConfigDataObject or
    // MapperServiceProfileObject
    // that has its tpPointer match with manged entity Id

    MacBridgePortConfigDataObject *port = NULL;
    MapperServiceProfileObject *mapper = NULL;

    // search MacBridgePortConfigDataObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        found = ((port->tpPointer == ipHost->managedEntityId) &&
                 (port->tpType == OMCI_BP_TP_IP_HOST));
        // if MacBridgePortConfigDataObject is found then
        if (found == TRUE)
        {
            // include bridge service to model type
            *type = OMCI_MF_MODEL_1_FILTER;
            // pass the search to MacBridgePortConfigDataObject
            ret = MacBridgePortConfigDataObject_searchModelPath
                      (port, direction, type, checkOid, checkMeId,
                       reachableOid, reachableMeId, depth + 1, inPath);
        }
        _cmsObj_free((void **) &port);
    }

    INIT_INSTANCE_ID_STACK(&iidStack);
    // search MapperServiceProfileObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mapper) == CMSRET_SUCCESS))
    {
        found = ((mapper->tpPointer == ipHost->managedEntityId) &&
                 (mapper->tpType == OMCI_MS_TP_IP_HOST));
        // if MapperServiceProfileObject is found then
        if (found == TRUE)
        {
            // include mapper service to model type
            *type = OMCI_MF_MODEL_1_MAP;
            // pass the search to MapperServiceProfileObject
            ret = MapperServiceProfileObject_searchModelPath
                      (mapper, direction, type, checkOid, checkMeId,
                       reachableOid, reachableMeId, depth + 1, inPath);
        }
        _cmsObj_free((void **) &mapper);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> Ipv6HostConfigDataObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", ipHost->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

CmsRet VirtualEthernetInterfacePointObject_searchModelPath
    (VirtualEthernetInterfacePointObject *veip,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // Since this object is currently in search path,
    // if this object is the one that needs to be checked
    // then specify the checked object is in search path
    // VlanTaggingOperationConfigurationDataObject can
    // implicitly link to VirtualEthernetInterfacePointObject when
    // they have the same managedEntityId
    if ((checkOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT ||
         checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) &&
        checkMeId == veip->managedEntityId)
        *inPath = TRUE;

    // store information of the reachable object
    // except when reachableMeId has specific
    // GemPortNetworkCtp object to be reached
    // since they're used to search path from UNI to ANI
    if (direction == OMCI_FLOW_DOWNSTREAM ||
       *reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
    {
        *reachableOid = MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT;
        *reachableMeId = veip->managedEntityId;
    }

    // if direction is downstream then return success
    // since Mapping Filter model is ended at
    // VirtualEthernetInterfacePointObject for downstream direction
    if (direction == OMCI_FLOW_DOWNSTREAM)
    {
        ret = CMSRET_SUCCESS;
        omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> VirtualEthernetInterfacePointObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", veip->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);
        return ret;
    }

    // if direction is upstream then search for either
    // MacBridgePortConfigDataObject or
    // MapperServiceProfileObject or
    // that has its tpPointer match with manged entity Id

    MacBridgePortConfigDataObject *port = NULL;
    MapperServiceProfileObject *mapper = NULL;

    // search MacBridgePortConfigDataObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        found = ((port->tpPointer == veip->managedEntityId) &&
                 (port->tpType == OMCI_BP_TP_VEIP));
        // if MacBridgePortConfigDataObject is found then
        if (found == TRUE)
        {
            // include bridge service to model type
            *type = OMCI_MF_MODEL_1_FILTER;
            // pass the search to MacBridgePortConfigDataObject
            ret = MacBridgePortConfigDataObject_searchModelPath
                      (port, direction, type, checkOid, checkMeId,
                       reachableOid, reachableMeId, depth + 1, inPath);
        }
        _cmsObj_free((void **) &port);
    }

    INIT_INSTANCE_ID_STACK(&iidStack);
    // search MapperServiceProfileObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mapper) == CMSRET_SUCCESS))
    {
//        found = ((mapper->tpPointer == veip->managedEntityId) &&
//                 (mapper->tpType == OMCI_MS_TP_VEIP));
        found = (mapper->tpPointer == veip->managedEntityId);
        // if MapperServiceProfileObject is found then
        if (found == TRUE)
        {
            // include mapper service to model type
            *type = OMCI_MF_MODEL_1_MAP;
            // pass the search to MapperServiceProfileObject
            ret = MapperServiceProfileObject_searchModelPath
                      (mapper, direction, type, checkOid, checkMeId,
                       reachableOid, reachableMeId, depth + 1, inPath);
        }
        _cmsObj_free((void **) &mapper);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> VirtualEthernetInterfacePointObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", veip->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

CmsRet VlanTaggingFilterDataObject_searchModelPath
    (VlanTaggingFilterDataObject *vlanFilter,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // Since this object is currently in search path,
    // if this object is the one that needs to be checked
    // then specify the checked object is in search path
    if (checkOid == MDMOID_VLAN_TAGGING_FILTER_DATA &&
        checkMeId == vlanFilter->managedEntityId)
        *inPath = TRUE;

    // store information of the reachable object
    // except when reachableMeId has specific
    // GemPortNetworkCtp object to be reached
    // since they're used to search path from UNI to ANI
    if (direction == OMCI_FLOW_DOWNSTREAM ||
        *reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
    {
        *reachableOid = MDMOID_VLAN_TAGGING_FILTER_DATA;
        *reachableMeId = vlanFilter->managedEntityId;
    }

    // for both directions VlanTaggingFilterDataObject is
    // implicitly link to MacBridgePortConfigDataObject
    MacBridgePortConfigDataObject *port = NULL;

    // search MacBridgePortConfigDataObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        found = (port->managedEntityId == vlanFilter->managedEntityId);
        // if MacBridgePortConfigDataObject is found then
        if (found == TRUE)
        {
            // pass the search to MacBridgePortConfigDataObject
            ret = MacBridgePortConfigDataObject_searchModelPath
                      (port, direction, type, checkOid, checkMeId,
                       reachableOid, reachableMeId, depth + 1, inPath);
        }
        _cmsObj_free((void **) &port);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> VlanTaggingFilterDataObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", vlanFilter->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

CmsRet VlanTaggingOperationConfigurationDataObject_searchModelPath
    (VlanTaggingOperationConfigurationDataObject *vlanOper,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // Since this object is currently in search path,
    // if this object is the one that needs to be checked
    // then specify the checked object is in search path
    if (checkOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA &&
        checkMeId == vlanOper->managedEntityId)
        *inPath = TRUE;

    // store information of the reachable object
    // except when reachableMeId has specific
    // GemPortNetworkCtp object to be reached
    // since they're used to search path from UNI to ANI
    if (direction == OMCI_FLOW_DOWNSTREAM ||
        *reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
    {
        *reachableOid = MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA;
        *reachableMeId = vlanOper->managedEntityId;
    }

    // for both directions VlanTaggingOperationConfigurationDataObject
    // is either implicitly link to PptpEthernetUniObject
    // when Association Type is zero or
    // explicitly link to Associated ME pointer with
    // specific Association Type

    switch (vlanOper->associationType)
    {
        case OMCI_VLAN_TP_IP_HOST:
        {
            Ipv6HostConfigDataObject *ipv6Host = NULL;
            IpHostConfigDataObject *ipHost = NULL;
            // search for Ipv6HostConfigDataObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // VlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_IPV6_HOST_CONFIG_DATA,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &ipv6Host) == CMSRET_SUCCESS))
            {
                found = (vlanOper->associatedManagedEntityPointer == ipv6Host->managedEntityId);
                // if Ipv6HostConfigDataObject is found then
                // pass the search to this object
                if (found == TRUE)
                {
                    ret = Ipv6HostConfigDataObject_searchModelPath
                              (ipv6Host, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &ipv6Host);
            }
            INIT_INSTANCE_ID_STACK(&iidStack);
            // search for IpHostConfigDataObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // VlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_IP_HOST_CONFIG_DATA,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &ipHost) == CMSRET_SUCCESS))
            {
                found = (vlanOper->associatedManagedEntityPointer == ipHost->managedEntityId);
                // if IpHostConfigDataObject is found then
                // pass the search to this object
                if (found == TRUE)
                {
                    ret = IpHostConfigDataObject_searchModelPath
                              (ipHost, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &ipHost);
            }
            break;
        }
        case OMCI_VLAN_TP_MAPPER_SERVICE:
        {
            MapperServiceProfileObject *mapper = NULL;
            // search for MapperServiceProfileObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // VlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mapper) == CMSRET_SUCCESS))
            {
                found = (vlanOper->associatedManagedEntityPointer == mapper->managedEntityId);
                // if MapperServiceProfileObject is found then
                // pass the search to this object
                if (found == TRUE)
                {
                    ret = MapperServiceProfileObject_searchModelPath
                              (mapper, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &mapper);
            }
            break;
        }
        case OMCI_VLAN_TP_MAC_BRIDGE_PORT:
        {
            MacBridgePortConfigDataObject *port = NULL;
            // search for MacBridgePortConfigDataObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // VlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &port) == CMSRET_SUCCESS))
            {
                found = (vlanOper->associatedManagedEntityPointer == port->managedEntityId);
                // if MacBridgePortConfigDataObject is found then
                // pass the search to this object
                if (found == TRUE)
                {
                    ret = MacBridgePortConfigDataObject_searchModelPath
                              (port, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &port);
            }
            break;
        }
        case OMCI_VLAN_TP_GEM_INTERWORKING:
        {
            GemInterworkingTpObject *tp = NULL;
            // search for GemInterworkingTpObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // VlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_GEM_INTERWORKING_TP,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &tp) == CMSRET_SUCCESS))
            {
                found = (vlanOper->associatedManagedEntityPointer == tp->managedEntityId);
                // if GemInterworkingTpObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = GemInterworkingTpObject_searchModelPath
                              (tp, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &tp);
            }
            break;
        }
        case OMCI_VLAN_TP_MCAST_GEM_INTERWORKING:
        {
            MulticastGemInterworkingTpObject *mcast = NULL;
            // search for MulticastGemInterworkingTpObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // VlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MULTICAST_GEM_INTERWORKING_TP,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mcast) == CMSRET_SUCCESS))
            {
                found = (vlanOper->associatedManagedEntityPointer == mcast->managedEntityId);
                // if MulticastGemInterworkingTpObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = MulticastGemInterworkingTpObject_searchModelPath
                              (mcast, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &mcast);
            }
            break;
        }
        case OMCI_VLAN_TP_PPTP_MOCA:
        {
            PptpMocaUniObject *pptpMoca = NULL;
            // search for PptpMocaUniObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // VlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_PPTP_MOCA_UNI,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &pptpMoca) == CMSRET_SUCCESS))
            {
                found = (vlanOper->associatedManagedEntityPointer == pptpMoca->managedEntityId);
                // if PptpMocaUniObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = PptpMocaUniObject_searchModelPath
                              (pptpMoca, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &pptpMoca);
            }
            break;
        }
        case OMCI_VLAN_TP_PPTP_ETH:
        {
            PptpEthernetUniObject *pptpEth = NULL;
            // search for PptpEthernetUniObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // VlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &pptpEth) == CMSRET_SUCCESS))
            {
                found = (vlanOper->associatedManagedEntityPointer == pptpEth->managedEntityId);
                // if PptpEthernetUniObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = PptpEthernetUniObject_searchModelPath
                              (pptpEth, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &pptpEth);
            }
            break;
        }
        case OMCI_VLAN_TP_VEIP:
        {
            VirtualEthernetInterfacePointObject *veip = NULL;
            // search for VirtualEthernetInterfacePointObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // VlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &veip) == CMSRET_SUCCESS))
            {
                found = (vlanOper->associatedManagedEntityPointer == veip->managedEntityId);
                // if VirtualEthernetInterfacePointObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = VirtualEthernetInterfacePointObject_searchModelPath
                              (veip, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &veip);
            }
            break;
        }
        case OMCI_VLAN_TP_DEFAULT:
        default:
        {
            PptpEthernetUniObject *pptpEth = NULL;
            Ipv6HostConfigDataObject *ipv6Host = NULL;
            IpHostConfigDataObject *ipHost = NULL;
            // search for PptpEthernetUniObject that has its
            // managedEntityId match with managedEntityId of
            // VlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &pptpEth) == CMSRET_SUCCESS))
            {
                found = (vlanOper->managedEntityId == pptpEth->managedEntityId);
                // if PptpEthernetUniObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = PptpEthernetUniObject_searchModelPath
                              (pptpEth, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &pptpEth);
            }
            INIT_INSTANCE_ID_STACK(&iidStack);
            // search for Ipv6HostConfigDataObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // VlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_IPV6_HOST_CONFIG_DATA,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &ipv6Host) == CMSRET_SUCCESS))
            {
                found = (vlanOper->associatedManagedEntityPointer == ipv6Host->managedEntityId);
                // if Ipv6HostConfigDataObject is found then
                // pass the search to this object
                if (found == TRUE)
                {
                    ret = Ipv6HostConfigDataObject_searchModelPath
                              (ipv6Host, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &ipv6Host);
            }
            INIT_INSTANCE_ID_STACK(&iidStack);
            // search for IpHostConfigDataObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // VlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_IP_HOST_CONFIG_DATA,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &ipHost) == CMSRET_SUCCESS))
            {
                found = (vlanOper->associatedManagedEntityPointer == ipHost->managedEntityId);
                // if IpHostConfigDataObject is found then
                // pass the search to this object
                if (found == TRUE)
                {
                    ret = IpHostConfigDataObject_searchModelPath
                              (ipHost, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &ipHost);
            }
            break;
        }
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> VlanTaggingOperationConfigurationDataObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", vlanOper->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

CmsRet ExtendedVlanTaggingOperationConfigurationDataObject_searchModelPath
    (ExtendedVlanTaggingOperationConfigurationDataObject *xVlanOper,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // store information of the reachable object
    // except when reachableMeId has specific
    // GemPortNetworkCtp object to be reached
    // since they're used to search path from UNI to ANI
    if (direction == OMCI_FLOW_DOWNSTREAM ||
        *reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
    {
        *reachableOid = MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA;
        *reachableMeId = xVlanOper->managedEntityId;
    }

    // for both directions ExtendedVlanTaggingOperationConfigurationDataObject
    // is explicitly link to Associated ME pointer with
    // specific Association Type

    switch (xVlanOper->associationType)
    {
        case OMCI_XVLAN_TP_MAC_BRIDGE_PORT:
        {
            MacBridgePortConfigDataObject *port = NULL;
            // search for MacBridgePortConfigDataObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // ExtendedVlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &port) == CMSRET_SUCCESS))
            {
                found = (xVlanOper->associatedManagedEntityPointer == port->managedEntityId);
                // if MacBridgePortConfigDataObject is found then
                // pass the search to this object
                if (found == TRUE)
                {
                    ret = MacBridgePortConfigDataObject_searchModelPath
                              (port, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &port);
            }
            break;
        }
        case OMCI_XVLAN_TP_MAPPER_SERVICE:
        {
            MapperServiceProfileObject *mapper = NULL;
            // search for MapperServiceProfileObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // ExtendedVlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mapper) == CMSRET_SUCCESS))
            {
                found = (xVlanOper->associatedManagedEntityPointer == mapper->managedEntityId);
                // if MapperServiceProfileObject is found then
                // pass the search to this object
                if (found == TRUE)
                {
                    ret = MapperServiceProfileObject_searchModelPath
                              (mapper, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &mapper);
            }
            break;
        }
        case OMCI_XVLAN_TP_PPTP_ETH:
        {
            PptpEthernetUniObject *pptpEth = NULL;
            // search for PptpEthernetUniObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // ExtendedVlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &pptpEth) == CMSRET_SUCCESS))
            {
                found = (xVlanOper->associatedManagedEntityPointer == pptpEth->managedEntityId);
                // if PptpEthernetUniObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = PptpEthernetUniObject_searchModelPath
                              (pptpEth, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &pptpEth);
            }
            break;
        }
        case OMCI_XVLAN_TP_IP_HOST:
        {
            Ipv6HostConfigDataObject *ipv6Host = NULL;
            IpHostConfigDataObject *ipHost = NULL;
            // search for Ipv6HostConfigDataObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // ExtendedVlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_IPV6_HOST_CONFIG_DATA,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &ipv6Host) == CMSRET_SUCCESS))
            {
                found = (xVlanOper->associatedManagedEntityPointer == ipv6Host->managedEntityId);
                // if Ipv6HostConfigDataObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = Ipv6HostConfigDataObject_searchModelPath
                              (ipv6Host, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &ipv6Host);
            }
            INIT_INSTANCE_ID_STACK(&iidStack);
            // search for IpHostConfigDataObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // ExtendedVlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_IP_HOST_CONFIG_DATA,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &ipHost) == CMSRET_SUCCESS))
            {
                found = (xVlanOper->associatedManagedEntityPointer == ipHost->managedEntityId);
                // if IpHostConfigDataObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = IpHostConfigDataObject_searchModelPath
                              (ipHost, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &ipHost);
            }
            break;
        }
        case OMCI_XVLAN_TP_GEM_INTERWORKING:
        {
            GemInterworkingTpObject *tp = NULL;
            // search for GemInterworkingTpObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // ExtendedVlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_GEM_INTERWORKING_TP,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &tp) == CMSRET_SUCCESS))
            {
                found = (xVlanOper->associatedManagedEntityPointer == tp->managedEntityId);
                // if GemInterworkingTpObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = GemInterworkingTpObject_searchModelPath
                              (tp, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &tp);
            }
            break;
        }
        case OMCI_XVLAN_TP_MCAST_GEM_INTERWORKING:
        {
            MulticastGemInterworkingTpObject *mcast = NULL;
            // search for MulticastGemInterworkingTpObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // ExtendedVlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MULTICAST_GEM_INTERWORKING_TP,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mcast) == CMSRET_SUCCESS))
            {
                found = (xVlanOper->associatedManagedEntityPointer == mcast->managedEntityId);
                // if MulticastGemInterworkingTpObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = MulticastGemInterworkingTpObject_searchModelPath
                              (mcast, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &mcast);
            }
            break;
        }
        case OMCI_XVLAN_TP_PPTP_MOCA:
        {
            PptpMocaUniObject *pptpMoca = NULL;
            // search for PptpMocaUniObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // ExtendedVlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_PPTP_MOCA_UNI,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &pptpMoca) == CMSRET_SUCCESS))
            {
                found = (xVlanOper->associatedManagedEntityPointer == pptpMoca->managedEntityId);
                // if PptpMocaUniObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = PptpMocaUniObject_searchModelPath
                              (pptpMoca, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &pptpMoca);
            }
            break;
        }
        case OMCI_XVLAN_TP_VEIP:
        {
            VirtualEthernetInterfacePointObject *veip = NULL;
            // search for VirtualEthernetInterfacePointObject that has its
            // managedEntityId match with associatedManagedEntityPointer of
            // ExtendedVlanTaggingOperationConfigurationDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &veip) == CMSRET_SUCCESS))
            {
                found = (xVlanOper->associatedManagedEntityPointer == veip->managedEntityId);
                // if VirtualEthernetInterfacePointObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = VirtualEthernetInterfacePointObject_searchModelPath
                              (veip, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &veip);
            }
            break;
        }
        default:
            break;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> ExtendedVlanTaggingOperationConfigurationDataObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", xVlanOper->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

CmsRet MacBridgeServiceProfileObject_searchModelPath
    (MacBridgeServiceProfileObject *bridge,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    MacBridgePortConfigDataObject *port = NULL;
    UBOOL8 inPathTmp = *inPath;

    // Since this object is currently in search path,
    // if this object is the one that needs to be checked
    // then specify the checked object is in search path
    if (checkOid == MDMOID_MAC_BRIDGE_SERVICE_PROFILE &&
        checkMeId == bridge->managedEntityId)
        *inPath = TRUE;

    // store information of the reachable object
    // except when reachableMeId has specific
    // GemPortNetworkCtp object to be reached
    // since they're used to search path from UNI to ANI
    if (direction == OMCI_FLOW_DOWNSTREAM ||
        *reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
    {
        *reachableOid = MDMOID_MAC_BRIDGE_SERVICE_PROFILE;
        *reachableMeId = bridge->managedEntityId;
    }

    // mark bridge is found in this search path
    glbFoundBridge = TRUE;

    // search MacBridgePortConfigDataObject that has bridgeId
    // match with managed entity Id of MacBridgeServiceProfileObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        // if direction is upstream then look for
        // MacBridgePortConfigDataObject that is connected to
        // MapperServiceProfileObject or GemInterworkingTpObject
        // or MulticastGemInterworkingTpObject
        if (direction == OMCI_FLOW_UPSTREAM)
        {
            found = ((port->bridgeId == bridge->managedEntityId) &&
                     ((port->tpType == OMCI_BP_TP_MAPPER_SERVICE) ||
                      (port->tpType == OMCI_BP_TP_GEM_INTERWORKING) ||
                      (port->tpType == OMCI_BP_TP_MCAST_GEM_INTERWORKING)));
            // re-set model type to filter when it's upstream
            // to prepare for searching other MacBridgePortConfigDataObject
            *type = OMCI_MF_MODEL_1_FILTER;
        }
        // if direction is downstream then look for
        // MacBridgePortConfigDataObject that is connected to
        // PptpEthernetUniObject or PptpMocaUniObject
        else
        {
            found = ((port->bridgeId == bridge->managedEntityId) &&
                     ((port->tpType == OMCI_BP_TP_PPTP_ETH) ||
                      (port->tpType == OMCI_BP_TP_PPTP_MOCA) ||
                      (port->tpType == OMCI_BP_TP_IP_HOST) ||
                      (port->tpType == OMCI_BP_TP_VEIP)));
        }
        // if MacBridgePortConfigDataObject is found then
        if (found == TRUE)
        {
            // pass the search to MacBridgePortConfigDataObject
            ret = MacBridgePortConfigDataObject_searchModelPath
                      (port, direction, type, checkOid, checkMeId,
                       reachableOid, reachableMeId, depth + 1, inPath);
            // if direction is upstream  and path is not found
            // then reset found = FALSE to search for
            // next MacBridgePortConfigDataObject since
            // multiple MacBridgePortConfigDataObject can be
            // connected to the same MacBridgeServiceProfileObject
            if (direction == OMCI_FLOW_UPSTREAM && ret != CMSRET_SUCCESS)
            {
                found = FALSE;
                /* Remember to restore inPath, since inPath can be
                                polluted by last round search!*/
                *inPath = inPathTmp;
            }
        }
        _cmsObj_free((void **) &port);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> MacBridgeServiceProfileObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", bridge->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

CmsRet MacBridgePortConfigDataObject_searchModelPath
    (MacBridgePortConfigDataObject *port,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8  *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // Since this object is currently in search path,
    // if this object is the one that needs to be checked
    // then specify the checked object is in search path
    // VlanTaggingFilterDataObject can implicitly link
    // to MacBridgePortConfigDataObject when
    // they have the same managedEntityId
    if ((checkOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA ||
         checkOid == MDMOID_VLAN_TAGGING_FILTER_DATA) &&
        checkMeId == port->managedEntityId)
        *inPath = TRUE;

    // store information of the reachable object
    // except when reachableMeId has specific
    // GemPortNetworkCtp object to be reached
    // since they're used to search path from UNI to ANI
    if (direction == OMCI_FLOW_DOWNSTREAM ||
        *reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
    {
        *reachableOid = MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA;
        *reachableMeId = port->managedEntityId;
    }

    // include bridge service to model type
    switch (*type)
    {
        case OMCI_MF_MODEL_NONE:
            *type = OMCI_MF_MODEL_1_FILTER;
            break;
        case OMCI_MF_MODEL_1_MAP:
            *type = OMCI_MF_MODEL_1_MAP_FILTER;
            break;
        case OMCI_MF_MODEL_N_MAP:
            *type = OMCI_MF_MODEL_N_MAP_FILTER;
            break;
        case OMCI_MF_MODEL_1_FILTER:
        case OMCI_MF_MODEL_N_FILTER:
        case OMCI_MF_MODEL_1_MAP_FILTER:
        case OMCI_MF_MODEL_N_MAP_FILTER:
        default:
            break;
    }

    switch (direction)
    {
        // if direction is upstream then search for either
        // GemInterworkingTpObject OR MulticastGemInterworkingTpObject
        // OR MapperServiceProfileObject
        // that has its managed entity Id match with tpPointer
        // OR search for MacBridgeServiceProfileObject
        // that has its managedEntityId match with bridgeId
        case OMCI_FLOW_UPSTREAM:
        {
            GemInterworkingTpObject *tp = NULL;
            MulticastGemInterworkingTpObject *mcast = NULL;
            MapperServiceProfileObject *mapper = NULL;
            MacBridgeServiceProfileObject *bridge = NULL;

            // search for GemInterworkingTpObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_GEM_INTERWORKING_TP,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &tp) == CMSRET_SUCCESS))
            {
                found = (port->tpPointer == tp->managedEntityId &&
                         port->tpType == OMCI_BP_TP_GEM_INTERWORKING);
                // if GemInterworkingTpObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = GemInterworkingTpObject_searchModelPath
                              (tp, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &tp);
            }
            INIT_INSTANCE_ID_STACK(&iidStack);
            // if GemInterworkingTpObject is not found then
            // continue by searching for MulticastGemInterworkingTpObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MULTICAST_GEM_INTERWORKING_TP,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mcast) == CMSRET_SUCCESS))
            {
                found = (port->tpPointer == mcast->managedEntityId &&
                         port->tpType == OMCI_BP_TP_MCAST_GEM_INTERWORKING);
                if (found == TRUE)
                    ret = MulticastGemInterworkingTpObject_searchModelPath
                              (mcast, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &mcast);
            }
            INIT_INSTANCE_ID_STACK(&iidStack);
            // if GemInterworkingTpObject and MulticastGemInterworkingTpObject
            // are both not found then
            // continue by searching for MapperServiceProfileObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mapper) == CMSRET_SUCCESS))
            {
                found = (port->tpPointer == mapper->managedEntityId &&
                         port->tpType == OMCI_BP_TP_MAPPER_SERVICE);
                // if MapperServiceProfileObject is found then
                // include mapper service to model type and
                // pass the search to this object
                if (found == TRUE)
                {
                    switch (*type)
                    {
                        case OMCI_MF_MODEL_NONE:
                            *type = OMCI_MF_MODEL_1_MAP;
                            break;
                        case OMCI_MF_MODEL_1_FILTER:
                            *type = OMCI_MF_MODEL_1_MAP_FILTER;
                            break;
                        case OMCI_MF_MODEL_N_FILTER:
                            *type = OMCI_MF_MODEL_N_MAP_FILTER;
                            break;
                        case OMCI_MF_MODEL_1_MAP:
                        case OMCI_MF_MODEL_N_MAP:
                        case OMCI_MF_MODEL_1_MAP_FILTER:
                        case OMCI_MF_MODEL_N_MAP_FILTER:
                        default:
                            break;
                    }
                    ret = MapperServiceProfileObject_searchModelPath
                              (mapper, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &mapper);
            }
            // if MacBridgePortConfigDataObject is searched BEFORE reaching
            // MacBridgeServiceProfileObject then search MacBridgeServiceProfileObject
            if (found == FALSE && glbFoundBridge == FALSE)
            {
                INIT_INSTANCE_ID_STACK(&iidStack);
                // if neither GemInterworkingTpObject nor
                // MapperServiceProfileObject is found then
                // continue by searching for MacBridgeServiceProfileObject
                while ((!found) &&
                       (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_SERVICE_PROFILE,
                                       &iidStack,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &bridge) == CMSRET_SUCCESS))
                {
                    found = (port->bridgeId == bridge->managedEntityId);
                    // if MacBridgeServiceProfileObject is found then
                    // pass the search to this object
                    if (found == TRUE)
                        ret = MacBridgeServiceProfileObject_searchModelPath
                                  (bridge, direction, type, checkOid, checkMeId,
                                   reachableOid, reachableMeId, depth + 1, inPath);
                    _cmsObj_free((void **) &bridge);
                }
            }
            break;
        }
        case OMCI_FLOW_DOWNSTREAM:
        {
            // if direction is downstream then search for either
            // PptpEthernetUniObject OR PptpMocaUniObject
            // OR IpHostConfigDataObject OR Ipv6HostConfigDataObject
            // OR VirtualEthernetInterfacePointObject
            // that has its managed entity Id match with tpPointer
            // OR search for MacBridgeServiceProfileObject
            // that has its managedEntityId match with bridgeId
            PptpEthernetUniObject *pptpEth = NULL;
            Ipv6HostConfigDataObject *ipv6Host = NULL;
            IpHostConfigDataObject *ipHost = NULL;
            VirtualEthernetInterfacePointObject *veip = NULL;
            PptpMocaUniObject *pptpMoca = NULL;
            MacBridgeServiceProfileObject *bridge = NULL;

            switch (port->tpType)
            {
              // search for PptpEthernetUniObject
              case OMCI_BP_TP_PPTP_ETH:
                while ((!found) &&
                       (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                                       &iidStack,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &pptpEth) == CMSRET_SUCCESS))
                {
                    found = (port->tpPointer == pptpEth->managedEntityId);
                    // if PptpEthernetUniObject is found then
                    // pass the search to this object
                    if (found == TRUE)
                        ret = PptpEthernetUniObject_searchModelPath
                                  (pptpEth, direction, type, checkOid, checkMeId,
                                   reachableOid, reachableMeId, depth + 1, inPath);
                    _cmsObj_free((void **) &pptpEth);
                }
                break;
              // search for Ipv6HostConfigDataObject and IpHostConfigDataObject
              case OMCI_BP_TP_IP_HOST:
                while ((!found) &&
                       (_cmsObj_getNextFlags(MDMOID_IPV6_HOST_CONFIG_DATA,
                                       &iidStack,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &ipv6Host) == CMSRET_SUCCESS))
                {
                    found = (port->tpPointer == ipv6Host->managedEntityId);
                    // if Ipv6HostConfigDataObject is found then
                    // pass the search to this object
                    if (found == TRUE)
                        ret = Ipv6HostConfigDataObject_searchModelPath
                                  (ipv6Host, direction, type, checkOid, checkMeId,
                                   reachableOid, reachableMeId, depth + 1, inPath);
                    _cmsObj_free((void **) &ipv6Host);
                }
                INIT_INSTANCE_ID_STACK(&iidStack);
                while ((!found) &&
                       (_cmsObj_getNextFlags(MDMOID_IP_HOST_CONFIG_DATA,
                                       &iidStack,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &ipHost) == CMSRET_SUCCESS))
                {
                    found = (port->tpPointer == ipHost->managedEntityId);
                    // if IpHostConfigDataObject is found then
                    // pass the search to this object
                    if (found == TRUE)
                        ret = IpHostConfigDataObject_searchModelPath
                                  (ipHost, direction, type, checkOid, checkMeId,
                                   reachableOid, reachableMeId, depth + 1, inPath);
                    _cmsObj_free((void **) &ipHost);
                }
                break;
              // search for VirtualEthernetInterfacePointObject
              case OMCI_BP_TP_VEIP:
                while ((!found) &&
                       (_cmsObj_getNextFlags(MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                                       &iidStack,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &veip) == CMSRET_SUCCESS))
                {
                    found = (port->tpPointer == veip->managedEntityId);
                    // if VirtualEthernetInterfacePointObject is found then
                    // pass the search to this object
                    if (found == TRUE)
                        ret = VirtualEthernetInterfacePointObject_searchModelPath
                                  (veip, direction, type, checkOid, checkMeId,
                                   reachableOid, reachableMeId, depth + 1, inPath);
                    _cmsObj_free((void **) &veip);
                }
                break;
              // search for PptpMocaUniObject
              case OMCI_BP_TP_PPTP_MOCA:
                while ((!found) &&
                       (_cmsObj_getNextFlags(MDMOID_PPTP_MOCA_UNI,
                                       &iidStack,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &pptpMoca) == CMSRET_SUCCESS))
                {
                    found = (port->tpPointer == pptpMoca->managedEntityId);
                    // if PptpMocaUniObject is found then
                    // pass the search to this object
                    if (found == TRUE)
                        ret = PptpMocaUniObject_searchModelPath
                                  (pptpMoca, direction, type, checkOid, checkMeId,
                                   reachableOid, reachableMeId, depth + 1, inPath);
                    _cmsObj_free((void **) &pptpMoca);
                }
                break;
              // search for MacBridgeServiceProfileObject
              default:
                // if MacBridgePortConfigDataObject is searched BEFORE reaching
                // MacBridgeServiceProfileObject then search MacBridgeServiceProfileObject
                if (glbFoundBridge == FALSE)
                {
                    while ((!found) &&
                           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_SERVICE_PROFILE,
                                           &iidStack,
                                           OGF_NO_VALUE_UPDATE,
                                           (void **) &bridge) == CMSRET_SUCCESS))
                    {
                        found = (port->bridgeId == bridge->managedEntityId);
                        // if MacBridgeServiceProfileObject is found then
                        // pass the search to this object
                        if (found == TRUE)
                            ret = MacBridgeServiceProfileObject_searchModelPath
                                      (bridge, direction, type, checkOid, checkMeId,
                                       reachableOid, reachableMeId, depth + 1, inPath);
                        _cmsObj_free((void **) &bridge);
                    }
                }
                break;
            }
        }
        default:
            break;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> MacBridgePortConfigDataObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", port->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

CmsRet MapperServiceProfileObject_searchModelPath
    (MapperServiceProfileObject *mapper,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
    UBOOL8 inPathTmp = *inPath;

    // Since this object is currently in search path,
    // if this object is the one that needs to be checked
    // then specify the checked object is in search path
    if (checkOid == MDMOID_MAPPER_SERVICE_PROFILE &&
        checkMeId == mapper->managedEntityId)
        *inPath = TRUE;

    // store information of the reachable object
    // except when reachableMeId has specific
    // GemPortNetworkCtp object to be reached
    // since they're used to search path from UNI to ANI
    if (direction == OMCI_FLOW_DOWNSTREAM ||
        *reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
    {
        *reachableOid = MDMOID_MAPPER_SERVICE_PROFILE;
        *reachableMeId = mapper->managedEntityId;
    }

    // include mapper service to model type
    switch (*type)
    {
        case OMCI_MF_MODEL_NONE:
            // if tpType is bridging mapping
            if (mapper->tpPointer == 0xFFFF &&
                mapper->tpType == OMCI_MS_TP_BRIDGE)
                *type = OMCI_MF_MODEL_1_MAP_FILTER;
            else
                *type = OMCI_MF_MODEL_1_MAP;
            break;
        case OMCI_MF_MODEL_1_FILTER:
            *type = OMCI_MF_MODEL_1_MAP_FILTER;
            break;
        case OMCI_MF_MODEL_N_FILTER:
            *type = OMCI_MF_MODEL_N_MAP_FILTER;
            break;
        case OMCI_MF_MODEL_1_MAP:
            // if tpType is bridging mapping
            if (mapper->tpPointer == 0xFFFF &&
                mapper->tpType == OMCI_MS_TP_BRIDGE)
                *type = OMCI_MF_MODEL_1_MAP_FILTER;
            break;
        case OMCI_MF_MODEL_N_MAP:
            // if tpType is bridging mapping
            if (mapper->tpPointer == 0xFFFF &&
                mapper->tpType == OMCI_MS_TP_BRIDGE)
                *type = OMCI_MF_MODEL_N_MAP_FILTER;
            break;
        case OMCI_MF_MODEL_1_MAP_FILTER:
        case OMCI_MF_MODEL_N_MAP_FILTER:
        default:
            break;
    }

    switch (direction)
    {
        // if direction is upstream then search for
        // GemInterworkingTpObject that has its managed entity Id
        // match with one of interworkTpPointerPriorityXXX
        case OMCI_FLOW_UPSTREAM:
        {
            GemInterworkingTpObject *tp = NULL;
            MulticastGemInterworkingTpObject *mcast = NULL;
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_GEM_INTERWORKING_TP,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &tp) == CMSRET_SUCCESS))
            {
                found = ((mapper->interworkTpPointerPriority0 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority1 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority2 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority3 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority4 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority5 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority6 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority7 == tp->managedEntityId));
                // if GemInterworkingTpObject is found then
                // pass the search to this object
                if (found == TRUE)
                {
                    ret = GemInterworkingTpObject_searchModelPath
                              (tp, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                    // if cannot find ANI path successfuly then
                    // reset found to FALSE for searching other GemInterworkingTpObject
                    if (ret != CMSRET_SUCCESS)
                    {
                        found = FALSE;
                        /* Remember to restore inPath, since inPath can be
                                            polluted by last round search!*/
                        *inPath = inPathTmp;
                    }
                }
                _cmsObj_free((void **) &tp);
            }
            INIT_INSTANCE_ID_STACK(&iidStack);
            // if GemInterworkingTpObject is not found then
            // search for MulticastGemInterworkingTpObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MULTICAST_GEM_INTERWORKING_TP,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mcast) == CMSRET_SUCCESS))
            {
                found = ((mapper->interworkTpPointerPriority0 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority1 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority2 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority3 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority4 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority5 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority6 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority7 == mcast->managedEntityId));
                // if MulticastGemInterworkingTpObject is found then
                // pass the search to this object
                if (found == TRUE)
                {
                    ret = MulticastGemInterworkingTpObject_searchModelPath
                              (mcast, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                    // if cannot find ANI path successfuly then
                    // reset found to FALSE for searching other MulticastGemInterworkingTpObject
                    if (ret != CMSRET_SUCCESS)
                    {
                        found = FALSE;
                        /* Remember to restore inPath, since inPath can be
                                            polluted by last round search!*/
                        *inPath = inPathTmp;
                    }
                }
                _cmsObj_free((void **) &mcast);
            }
            break;
        }
        case OMCI_FLOW_DOWNSTREAM:
        {
            MacBridgePortConfigDataObject *port = NULL;
            PptpEthernetUniObject *pptpEth = NULL;
            PptpMocaUniObject *pptpMoca = NULL;
            VirtualEthernetInterfacePointObject *veip = NULL;
            switch (mapper->tpType)
            {
                // search MacBridgePortConfigDataObject that has tpPointer
                // match with managed entity Id of MapperServiceProfileObject
                // AND its tpType should be OMCI_BP_TP_MAPPER_SERVICE (3)
                case OMCI_MS_TP_BRIDGE:
                    while ((!found) &&
                           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                                           &iidStack,
                                           OGF_NO_VALUE_UPDATE,
                                           (void **) &port) == CMSRET_SUCCESS))
                    {
                        found = ((port->tpPointer == mapper->managedEntityId) &&
                                 (port->tpType == OMCI_BP_TP_MAPPER_SERVICE));
                        // if MacBridgePortConfigDataObject is found then
                        if (found == TRUE)
                        {
                            // pass the search to MacBridgePortConfigDataObject
                            ret = MacBridgePortConfigDataObject_searchModelPath
                                      (port, direction, type, checkOid, checkMeId,
                                       reachableOid, reachableMeId, depth + 1, inPath);
                        }
                        _cmsObj_free((void **) &port);
                    }
                    break;
                // search PptpEthernetUniObject that has managed entity Id
                // match with tpPointer of MapperServiceProfileObject
                case OMCI_MS_TP_PPTP_ETH:
                    while ((!found) &&
                           (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                                           &iidStack,
                                           OGF_NO_VALUE_UPDATE,
                                           (void **) &pptpEth) == CMSRET_SUCCESS))
                    {
                        found = (mapper->tpPointer == pptpEth->managedEntityId);
                        // if PptpEthernetUniObject is found then
                        if (found == TRUE)
                        {
                            // pass the search to PptpEthernetUniObject
                            ret = PptpEthernetUniObject_searchModelPath
                                      (pptpEth, direction, type, checkOid, checkMeId,
                                       reachableOid, reachableMeId, depth + 1, inPath);
                        }
                        _cmsObj_free((void **) &pptpEth);
                    }
                    break;
                // search PptpMocaUniObject that has managed entity Id
                // match with tpPointer of MapperServiceProfileObject
                case OMCI_MS_TP_PPTP_MOCA:
                    while ((!found) &&
                           (_cmsObj_getNextFlags(MDMOID_PPTP_MOCA_UNI,
                                           &iidStack,
                                           OGF_NO_VALUE_UPDATE,
                                           (void **) &pptpMoca) == CMSRET_SUCCESS))
                    {
                        found = (mapper->tpPointer == pptpMoca->managedEntityId);
                        // if PptpMocaUniObject is found then
                        if (found == TRUE)
                        {
                            // pass the search to PptpMocaUniObject
                            ret = PptpMocaUniObject_searchModelPath
                                      (pptpMoca, direction, type, checkOid, checkMeId,
                                       reachableOid, reachableMeId, depth + 1, inPath);
                        }
                        _cmsObj_free((void **) &pptpMoca);
                    }
                    break;
                // search VirtualEthernetInterfacePointObject that has managed entity Id
                // match with tpPointer of MapperServiceProfileObject
                case OMCI_MS_TP_VEIP:
                    while ((!found) &&
                           (_cmsObj_getNextFlags(MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                                           &iidStack,
                                           OGF_NO_VALUE_UPDATE,
                                           (void **) &veip) == CMSRET_SUCCESS))
                    {
                        found = (mapper->tpPointer == veip->managedEntityId);
                        // if VirtualEthernetInterfacePointObject is found then
                        if (found == TRUE)
                        {
                            // pass the search to VirtualEthernetInterfacePointObject
                            ret = VirtualEthernetInterfacePointObject_searchModelPath
                                      (veip, direction, type, checkOid, checkMeId,
                                       reachableOid, reachableMeId, depth + 1, inPath);
                        }
                        _cmsObj_free((void **) &veip);
                    }
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> MapperServiceProfileObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", mapper->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

CmsRet GemInterworkingTpObject_searchModelPath
    (GemInterworkingTpObject *tp,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // Since this object is currently in search path,
    // if this object is the one that needs to be checked
    // then specify the checked object is in search path
    if (checkOid == MDMOID_GEM_INTERWORKING_TP &&
        checkMeId == tp->managedEntityId)
        *inPath = TRUE;

    // store information of the reachable object
    // except when reachableMeId has specific
    // GemPortNetworkCtp object to be reached
    // since they're used to search path from UNI to ANI
    if (direction == OMCI_FLOW_DOWNSTREAM ||
        *reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
    {
        *reachableOid = MDMOID_GEM_INTERWORKING_TP;
        *reachableMeId = tp->managedEntityId;
    }

    switch (direction)
    {
        // if direction is upstream then search for
        // GemPortNetworkCtpObject that has its managed entity Id
        // match with gemPortNetworkCtpConnPointer
        case OMCI_FLOW_UPSTREAM:
        {
            GemPortNetworkCtpObject *ctp = NULL;
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_GEM_PORT_NETWORK_CTP,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &ctp) == CMSRET_SUCCESS))
            {
                if (*reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
                    found = (tp->gemPortNetworkCtpConnPointer == ctp->managedEntityId);
                // reachableMeId has specific GemPortNetworkCtp object
                // so use it to match the search
                else
                {
                    if (*reachableMeId == ctp->managedEntityId)
                        found = (tp->gemPortNetworkCtpConnPointer == ctp->managedEntityId);
                }
                // if GemPortNetworkCtpObject is found then
                // pass the search to this object
                if (found == TRUE)
                {
                    ret = GemPortNetworkCtpObject_searchModelPath
                              (ctp, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &ctp);
            }
            break;
        }
        // if direction is downstream then search for either
        //    1. MapperServiceProfileObject that has its managed
        //       entity Id match with serviceProfilePointer.
        //       If found then model should have mapper service
        //    2. MacBridgePortConfigDataObject that has tpPointer
        //       match with managed entity Id of GemInterworkingTpObject
        //       AND its tpType should be OMCI_BP_TP_GEM_INTERWORKING (5).
        //       If found then model should have filter service
        case OMCI_FLOW_DOWNSTREAM:
        {
            MapperServiceProfileObject *mapper = NULL;
            MacBridgePortConfigDataObject *filter = NULL;
            // search MapperServiceProfileObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mapper) == CMSRET_SUCCESS))
            {
                // if there is a link from serviceProfilePointer of tp
                // to mapper service profile then it's found
                //if (tp->serviceProfilePointer == mapper->managedEntityId)
                //    found = TRUE;

                // since there is no link from serviceProfilePointer of tp
                // to mapper service profile, try to find any
                // interworkTpPointerPriority that links to tp
                found = ((mapper->interworkTpPointerPriority0 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority1 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority2 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority3 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority4 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority5 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority6 == tp->managedEntityId) ||
                         (mapper->interworkTpPointerPriority7 == tp->managedEntityId));

                // if MapperServiceProfileObject is found then
                if (found == TRUE)
                {
                    // include mapper service to model type
                    switch (*type)
                    {
                        case OMCI_MF_MODEL_NONE:
                            *type = OMCI_MF_MODEL_1_MAP;
                            break;
                        case OMCI_MF_MODEL_1_FILTER:
                            *type = OMCI_MF_MODEL_1_MAP_FILTER;
                            break;
                        case OMCI_MF_MODEL_N_FILTER:
                            *type = OMCI_MF_MODEL_N_MAP_FILTER;
                            break;
                        case OMCI_MF_MODEL_1_MAP:
                        case OMCI_MF_MODEL_1_MAP_FILTER:
                        case OMCI_MF_MODEL_N_MAP:
                        case OMCI_MF_MODEL_N_MAP_FILTER:
                        default:
                            break;
                    }
                    // pass the search to MapperServiceProfileObject
                    ret = MapperServiceProfileObject_searchModelPath
                              (mapper, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &mapper);
            }
            INIT_INSTANCE_ID_STACK(&iidStack);
            // if MapperServiceProfileObject is not found then
            // search MacBridgePortConfigDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &filter) == CMSRET_SUCCESS))
            {
                found = ((filter->tpPointer == tp->managedEntityId) &&
                         (filter->tpType == OMCI_BP_TP_GEM_INTERWORKING));
                // if MacBridgePortConfigDataObject is found then
                if (found == TRUE)
                {
                    // include filter service to model type
                    switch (*type)
                    {
                        case OMCI_MF_MODEL_NONE:
                            *type = OMCI_MF_MODEL_1_FILTER;
                            break;
                        case OMCI_MF_MODEL_1_MAP:
                            *type = OMCI_MF_MODEL_1_MAP_FILTER;
                            break;
                        case OMCI_MF_MODEL_N_MAP:
                            *type = OMCI_MF_MODEL_N_MAP_FILTER;
                            break;
                        case OMCI_MF_MODEL_1_FILTER:
                        case OMCI_MF_MODEL_1_MAP_FILTER:
                        case OMCI_MF_MODEL_N_FILTER:
                        case OMCI_MF_MODEL_N_MAP_FILTER:
                        default:
                            break;
                    }
                    // pass the search to MacBridgePortConfigDataObject
                    ret = MacBridgePortConfigDataObject_searchModelPath
                              (filter, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &filter);
            }
            break;
        }
        default:
            break;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> GemInterworkingTpObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", tp->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

CmsRet MulticastGemInterworkingTpObject_searchModelPath
    (MulticastGemInterworkingTpObject *mcast,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // Since this object is currently in search path,
    // if this object is the one that needs to be checked
    // then specify the checked object is in search path
    if (checkOid == MDMOID_MULTICAST_GEM_INTERWORKING_TP &&
        checkMeId == mcast->managedEntityId)
        *inPath = TRUE;

    // store information of the reachable object
    // except when reachableMeId has specific
    // GemPortNetworkCtp object to be reached
    // since they're used to search path from UNI to ANI
    if (direction == OMCI_FLOW_DOWNSTREAM ||
        *reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
    {
        *reachableOid = MDMOID_MULTICAST_GEM_INTERWORKING_TP;
        *reachableMeId = mcast->managedEntityId;
    }

    switch (direction)
    {
        // if direction is upstream then search for
        // GemPortNetworkCtpObject that has its managed entity Id
        // match with gemPortNetworkCtpConnPointer
        case OMCI_FLOW_UPSTREAM:
        {
            GemPortNetworkCtpObject *ctp = NULL;
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_GEM_PORT_NETWORK_CTP,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &ctp) == CMSRET_SUCCESS))
            {
                if (*reachableOid != MDMOID_GEM_PORT_NETWORK_CTP)
                    found = (mcast->gemPortNetworkCtpConnPointer == ctp->managedEntityId);
                // reachableMeId has specific GemPortNetworkCtp object
                // so use it to match the search
                else
                {
                    if (*reachableMeId == ctp->managedEntityId)
                        found = (mcast->gemPortNetworkCtpConnPointer == ctp->managedEntityId);
                }
                // if GemPortNetworkCtpObject is found then
                // pass the search to this object
                if (found == TRUE)
                {
                    ret = GemPortNetworkCtpObject_searchModelPath
                              (ctp, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &ctp);
            }
            break;
        }
        // if direction is downstream then search for either
        //    1. MapperServiceProfileObject that has its managed
        //       entity Id match with serviceProfilePointer.
        //       If found then model should have mapper service
        //    2. MacBridgePortConfigDataObject that has tpPointer
        //       match with managed entity Id of MulticastGemInterworkingTpObject
        //       AND its tpType should be OMCI_BP_TP_GEM_INTERWORKING (5).
        //       If found then model should have filter service
        case OMCI_FLOW_DOWNSTREAM:
        {
            MapperServiceProfileObject *mapper = NULL;
            MacBridgePortConfigDataObject *filter = NULL;
            // search MapperServiceProfileObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mapper) == CMSRET_SUCCESS))
            {
                // if there is a link from serviceProfilePointer of tp
                // to mapper service profile then it's found
                //if (mcast->serviceProfilePointer == mapper->managedEntityId)
                //    found = TRUE;

                // since there is no link from serviceProfilePointer of tp
                // to mapper service profile, try to find any
                // interworkTpPointerPriority that links to mcast
                found = ((mapper->interworkTpPointerPriority0 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority1 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority2 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority3 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority4 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority5 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority6 == mcast->managedEntityId) ||
                         (mapper->interworkTpPointerPriority7 == mcast->managedEntityId));

                // if MapperServiceProfileObject is found then
                if (found == TRUE)
                {
                    // include mapper service to model type
                    switch (*type)
                    {
                        case OMCI_MF_MODEL_NONE:
                            *type = OMCI_MF_MODEL_1_MAP;
                            break;
                        case OMCI_MF_MODEL_1_FILTER:
                            *type = OMCI_MF_MODEL_1_MAP_FILTER;
                            break;
                        case OMCI_MF_MODEL_N_FILTER:
                            *type = OMCI_MF_MODEL_N_MAP_FILTER;
                            break;
                        case OMCI_MF_MODEL_1_MAP:
                        case OMCI_MF_MODEL_1_MAP_FILTER:
                        case OMCI_MF_MODEL_N_MAP:
                        case OMCI_MF_MODEL_N_MAP_FILTER:
                        default:
                            break;
                    }
                    // pass the search to MapperServiceProfileObject
                    ret = MapperServiceProfileObject_searchModelPath
                              (mapper, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &mapper);
            }
            INIT_INSTANCE_ID_STACK(&iidStack);
            // if MapperServiceProfileObject is not found then
            // search MacBridgePortConfigDataObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &filter) == CMSRET_SUCCESS))
            {
                found = ((filter->tpPointer == mcast->managedEntityId) &&
                         (filter->tpType == OMCI_BP_TP_MCAST_GEM_INTERWORKING));
                // if MacBridgePortConfigDataObject is found then
                if (found == TRUE)
                {
                    // include filter service to model type
                    switch (*type)
                    {
                        case OMCI_MF_MODEL_NONE:
                            *type = OMCI_MF_MODEL_1_FILTER;
                            break;
                        case OMCI_MF_MODEL_1_MAP:
                            *type = OMCI_MF_MODEL_1_MAP_FILTER;
                            break;
                        case OMCI_MF_MODEL_N_MAP:
                            *type = OMCI_MF_MODEL_N_MAP_FILTER;
                            break;
                        case OMCI_MF_MODEL_1_FILTER:
                        case OMCI_MF_MODEL_1_MAP_FILTER:
                        case OMCI_MF_MODEL_N_FILTER:
                        case OMCI_MF_MODEL_N_MAP_FILTER:
                        default:
                            break;
                    }
                    // pass the search to MacBridgePortConfigDataObject
                    ret = MacBridgePortConfigDataObject_searchModelPath
                              (filter, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                }
                _cmsObj_free((void **) &filter);
            }
            break;
        }
        default:
            break;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> MulticastGemInterworkingTpObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", mcast->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

CmsRet GemPortNetworkCtpObject_searchModelPath
    (GemPortNetworkCtpObject *ctp,
     OmciFLowDirection direction,
     OmciMapFilterModelType *type,
     UINT32 checkOid,
     UINT32 checkMeId,
     UINT32 *reachableOid,
     UINT32 *reachableMeId,
     UINT32 depth,
     UBOOL8 *inPath)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // Since this object is currently in search path,
    // if this object is the one that needs to be checked
    // then specify the checked object is in search path
    if (checkOid == MDMOID_GEM_PORT_NETWORK_CTP &&
        checkMeId == ctp->managedEntityId)
        *inPath = TRUE;

    // store information of the reachable object
    *reachableOid = MDMOID_GEM_PORT_NETWORK_CTP;
    *reachableMeId = ctp->managedEntityId;

    switch (direction)
    {
        // if direction is upstream then return success
        // since Mapping Filter model is ended at
        // GemPortNetworkCtpObject for upstream direction
        case OMCI_FLOW_UPSTREAM:
        {
            ret = CMSRET_SUCCESS;
            break;
        }
        case OMCI_FLOW_DOWNSTREAM:
        {
            GemInterworkingTpObject *tp = NULL;
            MulticastGemInterworkingTpObject *mcast = NULL;
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_GEM_INTERWORKING_TP,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &tp) == CMSRET_SUCCESS))
            {
                found = (tp->gemPortNetworkCtpConnPointer == ctp->managedEntityId);
                // if GemInterworkingTpObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = GemInterworkingTpObject_searchModelPath
                              (tp, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &tp);
            }
            INIT_INSTANCE_ID_STACK(&iidStack);
            // if GemInterworkingTpObject is not found then
            // search MulticastGemInterworkingTpObject
            while ((!found) &&
                   (_cmsObj_getNextFlags(MDMOID_MULTICAST_GEM_INTERWORKING_TP,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mcast) == CMSRET_SUCCESS))
            {
                found = (mcast->gemPortNetworkCtpConnPointer == ctp->managedEntityId);
                // if MulticastGemInterworkingTpObject is found then
                // pass the search to this object
                if (found == TRUE)
                    ret = MulticastGemInterworkingTpObject_searchModelPath
                              (mcast, direction, type, checkOid, checkMeId,
                               reachableOid, reachableMeId, depth + 1, inPath);
                _cmsObj_free((void **) &mcast);
            }
            break;
        }
        default:
            break;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> GemPortNetworkCtpObject_searchModelPath, managedEntityId=%d, type=%d, reachableOid=%d, reachableMeId=%d, oid=%d, meId=%d, inPath=%d, depth=%d, ret=%d\n", ctp->managedEntityId, *type, *reachableOid, *reachableMeId, checkOid, checkMeId, *inPath,  depth,ret);

    return ret;
}

#if defined(CONFIG_BCM_GPON_802_1Q_ENABLED)
CmsRet analyzeDsFlows(UINT32 uniPort, OmciPathFlow_t *flowDs_p, vlanDomains_t *vlanDomain_p)
{
    int loopCnt;
    OmciFlowFilter_t *filter_p = &(flowDs_p->filter);
    OmciFlowAction_t *action_p = &(flowDs_p->action);
    OmciVlanAction_t *vlanAction_p;
    BpaEthActionType_t *ethAction_p;
    vlanDomainInfo_t *domainInfo_p;
    UINT32 vlanDomainVid;
    UINT32 bUntaggedIf = 0;
    UINT32 bFound = FALSE;
    UINT32 remainingTags = 0;


    if(OMCI_IS_DONT_CARE(filter_p->vlan.nbrOfTags) ||
       OMCI_IS_DONT_CARE(filter_p->vlan.inner.vid) )
    {
        /* No vlan filters or no inner tag VID filters */
        return CMSRET_SUCCESS;
    }
    remainingTags = filter_p->vlan.nbrOfTags;
    vlanDomainVid = filter_p->vlan.inner.vid;
    bFound = FALSE;
    for (loopCnt=0; bFound == FALSE && loopCnt < action_p->vlanActionIx; ++loopCnt)
    {
        vlanAction_p = &(action_p->vlanAction[loopCnt]);
        switch (vlanAction_p->type)
        {
            case e_VLAN_ACTION_SET_VID:
                if (vlanAction_p->toTag == 0) // TAG0
                {
                    vlanDomainVid = vlanAction_p->val;
                    bFound = TRUE;
                }
                break;

            default:
                break;
        }
    }
    bFound = FALSE;
    bUntaggedIf = 0;
    for (loopCnt=0; bFound == FALSE && loopCnt < action_p->ethActionIx; ++loopCnt)
    {
        ethAction_p = &(action_p->ethAction[loopCnt]);
        switch (*ethAction_p)
        {
            case e_ETH_ACTION_POP_TAG:
                if (remainingTags == OMCI_FILTER_TYPE_SINGLE_TAG)
                {
                    bUntaggedIf = 1;
                    bFound = TRUE;
                }
                else if (remainingTags > OMCI_FILTER_TYPE_SINGLE_TAG)
                {
                    remainingTags--; /* Poping the outer tag from double tagged */
                }
                break;

            default:
                break;
        }
    }
    bFound = FALSE;
    for (loopCnt=0; bFound == FALSE && loopCnt < vlanDomain_p->numDomains; ++loopCnt)
    {
        domainInfo_p = &(vlanDomain_p->domainInfo[loopCnt]);
        if (vlanDomainVid == domainInfo_p->vid)
        {
            bFound = TRUE;
            if (bUntaggedIf)
            {
                domainInfo_p->untaggedMap |= (1<<uniPort);
            }
            else
            {
                domainInfo_p->taggedMap |= (1<<uniPort);
            }
        }
    }
    if (bFound == FALSE && vlanDomain_p->numDomains < MAX_VLAN_DOMAINS)
    {
        domainInfo_p = &(vlanDomain_p->domainInfo[vlanDomain_p->numDomains]);
        domainInfo_p->untaggedMap = 0;
        domainInfo_p->taggedMap = (1<<7); // GPON is always tagged memeber
        domainInfo_p->vid = vlanDomainVid;
        if (bUntaggedIf)
        {
            domainInfo_p->untaggedMap |= (1<<uniPort);
        }
        else
        {
            domainInfo_p->taggedMap |= (1<<uniPort);
        }
        vlanDomain_p->numDomains++;
    }
    return CMSRET_SUCCESS;
}
CmsRet handleMcastDsVlanActions(const GemPortNetworkCtpObject *ctp,
                                const UINT32 uniOid,
                                const UINT32 uniMeId,
                                const OmciMapFilterModelType type)
{
    UBOOL8 isFilterNeeded = FALSE;
    UINT32 bridgeMeId = 0;
    UINT32 extVlanNum = 0;
    UINT32 i, j;
    UINT32 meId, uniPort;
    OmciExtVlanTagOperInfo_t extTagOperInfo;
    OmciExtVlanTagOperInfo_t *pExtTagOperInfo = &extTagOperInfo;
    OmciVlanTagOperInfo_t tagOperInfo;
    OmciVlanTciFilterInfo_t tagFilterInfo;
    OmciVlanTciFilterInfo_t *pTagFilterInfo = &tagFilterInfo;
    OmciPathFlow_t flowUs;
    OmciPathFlow_t flowDs;
    CmsRet ret = CMSRET_INTERNAL_ERROR;

    /* Only support for Ethernet UNI */
    if (uniOid != MDMOID_PPTP_ETHERNET_UNI)
    {
        return CMSRET_SUCCESS;
    }

    if (rutGpon_getFirstEthernetMeId(&meId) != CMSRET_SUCCESS)
    {
        return ret;
    }
    uniPort = uniMeId - meId;
    // initialize local variables
    memset(&extTagOperInfo, 0, sizeof(extTagOperInfo));
    memset(&tagOperInfo, 0, sizeof(tagOperInfo));
    memset(&tagFilterInfo, 0, sizeof(tagFilterInfo));
    memset(&flowUs, 0, sizeof(flowUs));
    memset(&flowDs, 0, sizeof(flowDs));

    // setup Extended VLAN tagging rule operation table
    ret = rutGpon_allocExtTagOperInfo(uniOid, uniMeId, &extTagOperInfo);
    if (ret != CMSRET_SUCCESS)
        goto out;

    // retrieve VLAN tagging operation configuration data if any
    rutGpon_getTagOperInfo(uniOid, uniMeId, &tagOperInfo);
    /* Assumption below is that there can only be either Vlan or xVlan NOT both */
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
    if (isFilterNeeded)
    {
        // get bridge Me ID
        ret = rutGpon_getBridgeMeIdFromUniMeId(uniOid,
                                               uniMeId, &bridgeMeId);
        if (ret != CMSRET_SUCCESS)
            goto out;
        // setup VLAN tagging filter table
        ret = rutGpon_allocTagFilterInfo(ctp->managedEntityId, bridgeMeId,
                                         &tagFilterInfo);
        if (ret != CMSRET_SUCCESS)
            goto out;
    }
    if (pExtTagOperInfo->numberOfEntries > 0 &&
        pTagFilterInfo->numberOfEntries > 0)
    {
        /* Filter and xVLAN */
        for (i = 0; i < pExtTagOperInfo->numberOfEntries; i++)
        {
            if (pExtTagOperInfo->pTagOperTbl[i].removeType == OMCI_TREATMENT_DISCARD_FRAME)
                continue;

            for (j = 0; j < pTagFilterInfo->numberOfEntries; j++)
            {
                ret = rutGpon_extractDsPathFlows(pExtTagOperInfo->inputTpid,
                    pExtTagOperInfo->outputTpid,
                    pExtTagOperInfo->downstreamMode,
                    &pExtTagOperInfo->pTagOperTbl[i],
                    pTagFilterInfo->forwardOperation,
                    &pTagFilterInfo->pVlanTciTbl[j],
                    &flowUs,
                    &flowDs);

                if (ret != CMSRET_SUCCESS)
                    continue;

                analyzeDsFlows(uniPort, &flowDs, &gblNewVlanDomain);
            }
        }
    }
    else if (pExtTagOperInfo->numberOfEntries > 0 &&
             pTagFilterInfo->numberOfEntries == 0)
    {
        /* Only xVLAN */
        for (i = 0; i < pExtTagOperInfo->numberOfEntries; i++)
        {
            if (pExtTagOperInfo->pTagOperTbl[i].removeType == OMCI_TREATMENT_DISCARD_FRAME)
                continue;

            ret = rutGpon_extractDsPathFlows(pExtTagOperInfo->inputTpid,
                pExtTagOperInfo->outputTpid,
                pExtTagOperInfo->downstreamMode,
                &pExtTagOperInfo->pTagOperTbl[i],
                0,
                NULL,
                &flowUs,
                &flowDs);

            if (ret != CMSRET_SUCCESS)
                continue;

            analyzeDsFlows(uniPort, &flowDs, &gblNewVlanDomain);
        }
    }
    else if (pExtTagOperInfo->numberOfEntries == 0 &&
             pTagFilterInfo->numberOfEntries > 0)
    {
        /* Only Filter */
        for (i = 0; i < pTagFilterInfo->numberOfEntries; i++)
        {
            ret = rutGpon_extractDsPathFlows(0,
                0,
                OMCI_XVLAN_DOWNSTREAM_NONE,
                NULL,
                pTagFilterInfo->forwardOperation,
                &pTagFilterInfo->pVlanTciTbl[i],
                &flowUs,
                &flowDs);
            if (ret != CMSRET_SUCCESS)
                continue;                

            analyzeDsFlows(uniPort, &flowDs, &gblNewVlanDomain);
        }
    }
out:
    /* Release the memory if allocated */
    if (extTagOperInfo.numberOfEntries > 0 && extTagOperInfo.pTagOperTbl != NULL)
        CMSMEM_FREE_BUF_AND_NULL_PTR(extTagOperInfo.pTagOperTbl);
    return ret;
}
#else /* !CONFIG_BCM_GPON_802_1Q_ENABLED */
CmsRet handleMcastDsVlanActions(const GemPortNetworkCtpObject *ctp __attribute__((unused)),
                                const UINT32 uniOid __attribute__((unused)),
                                const UINT32 uniMeId __attribute__((unused)),
                                const OmciMapFilterModelType type __attribute__((unused)))
{
    return CMSRET_SUCCESS;
}
#endif /* CONFIG_BCM_GPON_802_1Q_ENABLED */

CmsRet handleModelPath
    (const OmciObjectAction action,
     const GemPortNetworkCtpObject *ctp,
     const UBOOL8 isNewUni,
     const UINT32 checkOid,
     const UINT32 checkMeId,
     const UINT32 uniOid,
     const UINT32 uniMeId,
     const OmciMapFilterModelType type)
{
    CmsRet ret = CMSRET_SUCCESS;
    UBOOL8 newUni;

    if (rutGpon_isGemPortNetworkCtpMulticast(ctp) && isNewUni)
        newUni = FALSE;
    else
        newUni = isNewUni;

    switch (action)
    {
        // create new flows for this path
        case OMCI_ACTION_CREATE:
            ret = rutGpon_createRules(ctp, newUni, checkOid, checkMeId,
                                       uniOid, uniMeId, type);
            if (ret == CMSRET_SUCCESS)
            {
                omciDm_addGemUniPair(ctp->managedEntityId,
                  checkOid, checkMeId, uniOid, uniMeId, newUni, type);
            }
            break;
        // edit current flows that are created for this path
        case OMCI_ACTION_EDIT:
            ret = rutGpon_editRules(ctp, newUni, checkOid, checkMeId,
                                    uniOid, uniMeId, type);
            if (ret == CMSRET_SUCCESS)
            {
                omciDm_addGemUniPair(ctp->managedEntityId,
                  checkOid, checkMeId, uniOid, uniMeId, newUni, type);
            }
            break;
        // delete current flows that are created for this path
        case OMCI_ACTION_DELETE:
            ret = rutGpon_deleteRules(ctp, newUni, checkOid, checkMeId,
                                      uniOid, uniMeId, type);
            if (ret == CMSRET_SUCCESS)
            {
                omciDm_removeGemUniPair(ctp->managedEntityId,
                  checkOid, checkMeId, uniOid, uniMeId);
            }
            break;
        default:
            break;
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  rutGpon_checkModelPath_US
*  PURPOSE:   The checkModelPath function. It is invoked during service
*             deletion.
*  PARAMETERS:
*      checkOid - check OID.
*      checkMeId - check ME id.
*      action - always DELETE.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      The logic in rut_gpon_rule assumes unicast GEM port(s) are processed
*      before multicast/broadcast GEM port(s). However the OMCI sequence from
*      the OLT, and the corresponding GEM port order in the OMCI MIB maybe
*      different. This function first checks unicast GEM ports then others.
*****************************************************************************/
CmsRet rutGpon_checkModelPath_US
    (UINT32 checkOid, UINT32 checkMeId, OmciObjectAction action)
{
    UBOOL8 isNewUni = TRUE;
    UBOOL8 *newUniP = &isNewUni;
    PptpEthernetUniObject *pptpEthernet = NULL;
    PptpMocaUniObject *pptpMoca = NULL;
    Ipv6HostConfigDataObject *ipv6Host = NULL;
    IpHostConfigDataObject *ipHost = NULL;
    VirtualEthernetInterfacePointObject *veip = NULL;
    InstanceIdStack iidEth = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidMoca = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidIpv6Host = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidIpHost = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVeip = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    while ((checkOid == MDMOID_PPTP_ETHERNET_UNI) &&
        _cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
            &iidEth,
            OGF_NO_VALUE_UPDATE,
            (void **) &pptpEthernet) == CMSRET_SUCCESS)
    {
        *newUniP = TRUE;
        ret = rutGpon_genericCheckModelPath(
                 action,
                 checkOid, checkMeId,
                 OMCI_FLOW_UPSTREAM,
                 MDMOID_PPTP_ETHERNET_UNI,
                 TRUE,
                 (OMciObjCmnHdr_t*)pptpEthernet,
                 newUniP,
                 (uniObject_searchModelPath)
                 PptpEthernetUniObject_searchModelPath);
        ret = rutGpon_genericCheckModelPath(
                 action,
                 checkOid, checkMeId,
                 OMCI_FLOW_DOWNSTREAM,
                 MDMOID_PPTP_ETHERNET_UNI,
                 TRUE,
                 (OMciObjCmnHdr_t*)pptpEthernet,
                 newUniP,
                 (uniObject_searchModelPath)
                 PptpEthernetUniObject_searchModelPath);

        _cmsObj_free((void **) &pptpEthernet);
    }

    while ((checkOid == MDMOID_PPTP_MOCA_UNI) &&
        _cmsObj_getNextFlags(MDMOID_PPTP_MOCA_UNI,
            &iidMoca,
            OGF_NO_VALUE_UPDATE,
            (void **) &pptpMoca) == CMSRET_SUCCESS)
    {
        *newUniP = TRUE;
        ret = rutGpon_genericCheckModelPath(
                 action,
                 checkOid, checkMeId,
                 OMCI_FLOW_UPSTREAM,
                 MDMOID_PPTP_MOCA_UNI,
                 TRUE,
                 (OMciObjCmnHdr_t*)pptpMoca,
                 newUniP,
                 (uniObject_searchModelPath)
                 PptpMocaUniObject_searchModelPath);
        ret = rutGpon_genericCheckModelPath(
                 action,
                 checkOid, checkMeId,
                 OMCI_FLOW_DOWNSTREAM,
                 MDMOID_PPTP_MOCA_UNI,
                 TRUE,
                 (OMciObjCmnHdr_t*)pptpMoca,
                 newUniP,
                 (uniObject_searchModelPath)
                 PptpMocaUniObject_searchModelPath);

        _cmsObj_free((void **) &pptpMoca);
    }

    while ((checkOid == MDMOID_IPV6_HOST_CONFIG_DATA) &&
        _cmsObj_getNextFlags(MDMOID_IPV6_HOST_CONFIG_DATA,
            &iidIpv6Host,
            OGF_NO_VALUE_UPDATE,
            (void **) &ipv6Host) == CMSRET_SUCCESS)
    {
        *newUniP = TRUE;
        ret = rutGpon_genericCheckModelPath(
                 action,
                 checkOid, checkMeId,
                 OMCI_FLOW_UPSTREAM,
                 MDMOID_IPV6_HOST_CONFIG_DATA,
                 TRUE,
                 (OMciObjCmnHdr_t*)ipv6Host,
                 newUniP,
                 (uniObject_searchModelPath)
                 Ipv6HostConfigDataObject_searchModelPath);
        ret = rutGpon_genericCheckModelPath(
                 action,
                 checkOid, checkMeId,
                 OMCI_FLOW_DOWNSTREAM,
                 MDMOID_IPV6_HOST_CONFIG_DATA,
                 TRUE,
                 (OMciObjCmnHdr_t*)ipv6Host,
                 newUniP,
                 (uniObject_searchModelPath)
                 Ipv6HostConfigDataObject_searchModelPath);

        _cmsObj_free((void **) &ipv6Host);
    }

    while ((checkOid == MDMOID_IP_HOST_CONFIG_DATA) &&
        _cmsObj_getNextFlags(MDMOID_IP_HOST_CONFIG_DATA,
            &iidIpHost,
            OGF_NO_VALUE_UPDATE,
            (void **) &ipHost) == CMSRET_SUCCESS)
    {
        *newUniP = TRUE;
        ret = rutGpon_genericCheckModelPath(
                 action,
                 checkOid, checkMeId,
                 OMCI_FLOW_UPSTREAM,
                 MDMOID_IP_HOST_CONFIG_DATA,
                 TRUE,
                 (OMciObjCmnHdr_t*)ipHost,
                 newUniP,
                 (uniObject_searchModelPath)
                 IpHostConfigDataObject_searchModelPath);
        ret = rutGpon_genericCheckModelPath(
                 action,
                 checkOid, checkMeId,
                 OMCI_FLOW_DOWNSTREAM,
                 MDMOID_IP_HOST_CONFIG_DATA,
                 TRUE,
                 (OMciObjCmnHdr_t*)ipHost,
                 newUniP,
                 (uniObject_searchModelPath)
                 IpHostConfigDataObject_searchModelPath);

        _cmsObj_free((void **) &ipHost);
    }

    while ((checkOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT) &&
      _cmsObj_getNextFlags(MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
          &iidVeip,
          OGF_NO_VALUE_UPDATE,
          (void **) &veip) == CMSRET_SUCCESS)
    {
        *newUniP = TRUE;
        ret = rutGpon_genericCheckModelPath(
                 action,
                 checkOid, checkMeId,
                 OMCI_FLOW_UPSTREAM,
                 MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                 TRUE,
                 (OMciObjCmnHdr_t*)veip,
                 newUniP,
                 (uniObject_searchModelPath)
                 VirtualEthernetInterfacePointObject_searchModelPath);
        ret = rutGpon_genericCheckModelPath(
                 action,
                 checkOid, checkMeId,
                 OMCI_FLOW_DOWNSTREAM,
                 MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                 TRUE,
                 (OMciObjCmnHdr_t*)veip,
                 newUniP,
                 (uniObject_searchModelPath)
                 VirtualEthernetInterfacePointObject_searchModelPath);

        _cmsObj_free((void **) &veip);
    }

    return ret;
}

CmsRet rutGpon_checkModelPath_DS
    (UINT32 checkOid, UINT32 checkMeId, OmciObjectAction action)
{
    UBOOL8 inPath = FALSE, isNewUni = FALSE;
    UINT32 reachableOid = 0, reachableMeId = 0;
    UINT32 prevReachableOid = 0, prevReachableMeId = 0;
    GemPortNetworkCtpObject *ctp = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciMapFilterModelType type = OMCI_MF_MODEL_NONE;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    while (_cmsObj_getNextFlags(MDMOID_GEM_PORT_NETWORK_CTP,
                          &iidStack,
                          OGF_NO_VALUE_UPDATE,
                          (void **) &ctp) == CMSRET_SUCCESS)
    {
        type = OMCI_MF_MODEL_NONE;
        inPath = FALSE;
        // mark bridge is not found before searching path
        glbFoundBridge = FALSE;

        // searching path from GemPortNetworkCtpObject to PptpXXXUniObject
        ret = GemPortNetworkCtpObject_searchModelPath
                  (ctp, OMCI_FLOW_DOWNSTREAM, &type, checkOid, checkMeId,
                   &reachableOid, &reachableMeId, 0, &inPath);

        omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> rutGpon_checkModelPath_DS, reachableOid=%d, reachableMeId=%d, aniMeId=%d, type=%d, checkOid=%d, checkMeId=%d, inPath=%d, depth=%d, ret=%d\n\n", reachableOid, reachableMeId, ctp->managedEntityId, type, checkOid, checkMeId, inPath, 0, ret);

        if (inPath == TRUE && ret == CMSRET_SUCCESS)
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                           "===> ANI to UNI path is completed, and object is in this path, \n\n");

            // is reachableMeId belonged to NEW UNI
            if (prevReachableOid != reachableOid ||
                prevReachableMeId != reachableMeId)
            {
                prevReachableOid = reachableOid;
                prevReachableMeId = reachableMeId;
                isNewUni = TRUE;
            }
            else
                isNewUni = FALSE;

            handleModelPath(action, ctp, isNewUni,
                            checkOid, checkMeId, reachableOid,
                            reachableMeId, type);
        }

        _cmsObj_free((void **) &ctp);
    }

    return ret;
}
#ifdef OMCI_TR69_DUAL_STACK
void rutGpon_checkTr069Managemet(UINT32 ipHostMeId __attribute__((unused)))
{
    Tr069ManagementServerObject* tr069 = NULL;
    NetworkAddressObject* netAddr = NULL;
    AuthenticationSecurityMethodObject *auth = NULL;
    InstanceIdStack iidTr069 = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidNetAddr = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidAuth = EMPTY_INSTANCE_ID_STACK;
    UBOOL8 foundTr069 = FALSE;
    UBOOL8 foundNetAddr = FALSE;
    UBOOL8 foundAuth = FALSE;
    char *acs = NULL;
    UINT8 *username = NULL;
    UINT8 *password = NULL;

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
    VirtualEthernetInterfacePointObject *veipUni = NULL;
    TcpUdpConfigDataObject* tcpUdp = NULL;
    InstanceIdStack iidVeip = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidTcpUdp = EMPTY_INSTANCE_ID_STACK;
    UINT32 veipMeId = 0;
    UBOOL8 foundTcpUdp;
    UBOOL8 foundVeip = FALSE;
#endif     /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                     "===> rutGpon_checkTr069Managemet enter: ipHostMeId = %d\n", ipHostMeId);

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1

    // check path: veip->tcpUdp->ipHost
    while (!foundVeip &&
           _cmsObj_getNextFlags(MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                          &iidVeip,
                          OGF_NO_VALUE_UPDATE,
                          (void **) &veipUni) == CMSRET_SUCCESS)
    {
        foundTcpUdp = FALSE;
        INIT_INSTANCE_ID_STACK(&iidTcpUdp);
        while (!foundTcpUdp &&
               _cmsObj_getNextFlags(MDMOID_TCP_UDP_CONFIG_DATA,
                              &iidTcpUdp,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &tcpUdp) == CMSRET_SUCCESS)
        {
            foundTcpUdp = (veipUni->tcpUdpPointer == tcpUdp->managedEntityId);
            if (foundTcpUdp && tcpUdp->ipHostPointer == ipHostMeId)
            {
                foundVeip = TRUE;
                veipMeId = veipUni->managedEntityId;
                omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                          "===> rutGpon_checkTr069Managemet foundVeip = TRUE, veipMeId = %d, tcpUdpMeId = %d\n",
                          veipMeId, tcpUdp->managedEntityId);
            }
            _cmsObj_free((void **) &tcpUdp);
        }
        _cmsObj_free((void **) &veipUni);
    }

    if (!foundVeip)
    {
         omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                  "===> rutGpon_checkTr069Managemet foundVeip = FALSE\n");
        return;
    }

    //get tr069 acs, username and password
    while (!foundTr069&&
           _cmsObj_getNextFlags(MDMOID_TR069_MANAGEMENT_SERVER,
                          &iidTr069,
                          OGF_NO_VALUE_UPDATE,
                          (void **) &tr069) == CMSRET_SUCCESS)
    {
        foundTr069 = (tr069->managedEntityId == veipMeId);
        if (foundTr069)
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                      "===> rutGpon_checkTr069Managemet foundTr069, acsNetworkAddress = %d\n",
                      tr069->acsNetworkAddress);
            INIT_INSTANCE_ID_STACK(&iidNetAddr);
            while (!foundNetAddr &&
                   _cmsObj_getNextFlags(MDMOID_NETWORK_ADDRESS,
                                  &iidNetAddr,
                                  OGF_NO_VALUE_UPDATE,
                                  (void**)&netAddr) == CMSRET_SUCCESS)
            {
                foundNetAddr = (tr069->acsNetworkAddress == netAddr->managedEntityId);
                if (foundNetAddr)
                {
                    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                              "===> rutGpon_checkTr069Managemet foundNetAddr, addressPointer = %d, securityPointer = %d\n",
                              netAddr->addressPointer, netAddr->securityPointer);
                    acs = rutGpon_getLargeString(netAddr->addressPointer);
                    if (acs)
                    {
                        omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                                  "===> rutGpon_checkTr069Managemet got ACS = %s\n",acs);
                        while (foundAuth == FALSE &&
                               _cmsObj_getNextFlags(MDMOID_AUTHENTICATION_SECURITY_METHOD,
                                              &iidAuth, OGF_NO_VALUE_UPDATE, (void**)&auth) == CMSRET_SUCCESS)
                       {
                           foundAuth = (netAddr->securityPointer == auth->managedEntityId);
                           if (foundAuth)
                           {
                               UINT32 size;

                               cmsUtl_hexStringToBinaryBuf(auth->username1, &username, &size);
                               cmsUtl_hexStringToBinaryBuf(auth->password, &password, &size);
                               omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                                         "===> rutGpon_checkTr069Managemet foundAuth, username = %s, password = %s\n",
                                         username ? (char *)username : "", password ? (char *)password : "");
                           }
                           _cmsObj_free((void**)&auth);
                       }
                    }
                }
                _cmsObj_free((void**)&netAddr);
            }
        }
        _cmsObj_free((void **) &tr069);
    }

#else     /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

    //check path: tr69ManagementServer->ipHost
    //get tr069 acs, username and password
    while (!foundTr069&&
           _cmsObj_getNextFlags(MDMOID_TR069_MANAGEMENT_SERVER,
                          &iidTr069,
                          OGF_NO_VALUE_UPDATE,
                          (void **) &tr069) == CMSRET_SUCCESS)
    {
        foundTr069 = (tr069->managedEntityId == ipHostMeId);
        if (foundTr069)
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                      "===> rutGpon_checkTr069Managemet foundTr069, acsNetworkAddress = %d\n",
                      tr069->acsNetworkAddress);
            INIT_INSTANCE_ID_STACK(&iidNetAddr);
            while (!foundNetAddr &&
                   _cmsObj_getNextFlags(MDMOID_NETWORK_ADDRESS,
                                  &iidNetAddr,
                                  OGF_NO_VALUE_UPDATE,
                                  (void**)&netAddr) == CMSRET_SUCCESS)
            {
                foundNetAddr = (tr069->acsNetworkAddress == netAddr->managedEntityId);
                if (foundNetAddr)
                {
                    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                              "===> rutGpon_checkTr069Managemet foundNetAddr, addressPointer = %d, securityPointer = %d\n",
                              netAddr->addressPointer, netAddr->securityPointer);
                    acs = rutGpon_getLargeString(netAddr->addressPointer);
                    if (acs)
                    {
                        omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                                  "===> rutGpon_checkTr069Managemet got ACS = %s\n",acs);
                        while (foundAuth == FALSE &&
                               _cmsObj_getNextFlags(MDMOID_AUTHENTICATION_SECURITY_METHOD,
                                              &iidAuth, OGF_NO_VALUE_UPDATE, (void**)&auth) == CMSRET_SUCCESS)
                       {
                           foundAuth = (netAddr->securityPointer == auth->managedEntityId);
                           if (foundAuth)
                           {
                               UINT32 size;

                               cmsUtl_hexStringToBinaryBuf(auth->username1, &username, &size);
                               cmsUtl_hexStringToBinaryBuf(auth->password, &password, &size);
                               omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                                         "===> rutGpon_checkTr069Managemet foundAuth, username = %s, password = %s\n",
                                         username ? (char *)username : "", password ? (char *)password : "");
                           }
                           _cmsObj_free((void**)&auth);
                       }
                    }
                }
                _cmsObj_free((void**)&netAddr);
            }
        }
        _cmsObj_free((void **) &tr069);
    }

#endif     /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

    //update tr069 settings to BCM_Tr069ManagementServerObject
    if (acs && strlen(acs))
    {
        _owapi_rut_updateAcsCfg(ipHostMeId, acs, username, password);
    }

    cmsMem_free(acs);
    cmsMem_free(username);
    cmsMem_free(password);
}
#endif

// get management entity ID of VlanTaggingFilterDataObject
// from the given MacBridgePortConfigDataObject
CmsRet getVlanFilterMeIdFromMacBridgePortConfigDataObject
    (const MacBridgePortConfigDataObject *port,
     UINT32 bridgeMeId,
     UINT32 *pFilterMeId)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    VlanTaggingFilterDataObject *vlan = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search vlan tagging filter data object that has
    // its meId that matches with the given meId
    while ((!found) &&
           (_cmsObj_getNextFlags
                (MDMOID_VLAN_TAGGING_FILTER_DATA,
                 &iidStack,
                 OGF_NO_VALUE_UPDATE,
                 (void **) &vlan) == CMSRET_SUCCESS))
    {
        found = ((port->managedEntityId == vlan->managedEntityId) &&
                 (port->bridgeId == bridgeMeId));

        if (found == TRUE)
        {
            *pFilterMeId = vlan->managedEntityId;
            ret = CMSRET_SUCCESS;
        }

        _cmsObj_free((void **) &vlan);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> getVlanFilterMeIdFromMacBridgePortConfigDataObject, bridgeMeId=%d, filterMeId=%d, ret=%d\n", bridgeMeId, *pFilterMeId, ret);

    return ret;
}

// get management entity ID of VlanTaggingFilterDataObject
// from the given GemInterworkingTpObject
CmsRet getVlanFilterMeIdFromGemInterworkingTpObject
    (const UINT32 tpMeId,
     const UINT32 bridgeMeId,
     const UINT32 multicast,
     UINT32 *pFilterMeId)
{
    UBOOL8 foundMapper = FALSE, foundPort = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MapperServiceProfileObject *mapper = NULL;
    MacBridgePortConfigDataObject *port = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

if(!multicast)
{
    // search MapperServiceProfileObject
    while ((!foundMapper) &&
           (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mapper) == CMSRET_SUCCESS))
    {
        // find any interworkTpPointerPriority that links to tp
        foundMapper = ((mapper->interworkTpPointerPriority0 == tpMeId) ||
                       (mapper->interworkTpPointerPriority1 == tpMeId) ||
                       (mapper->interworkTpPointerPriority2 == tpMeId) ||
                       (mapper->interworkTpPointerPriority3 == tpMeId) ||
                       (mapper->interworkTpPointerPriority4 == tpMeId) ||
                       (mapper->interworkTpPointerPriority5 == tpMeId) ||
                       (mapper->interworkTpPointerPriority6 == tpMeId) ||
                       (mapper->interworkTpPointerPriority7 == tpMeId));

        // if MapperServiceProfileObject is found then
        if (foundMapper == TRUE)
        {
            if (mapper->tpType == OMCI_MS_TP_BRIDGE)
            {
                INIT_INSTANCE_ID_STACK(&iidStack);
                while ((!foundPort) &&
                       (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                                       &iidStack,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &port) == CMSRET_SUCCESS))
                {
                    foundPort = ((port->tpPointer == mapper->managedEntityId) &&
                                 (port->tpType == OMCI_BP_TP_MAPPER_SERVICE));
                    // if MacBridgePortConfigDataObject is found then
                    if (foundPort == TRUE)
                    {
                        // pass the search to MacBridgePortConfigDataObject
                        ret = getVlanFilterMeIdFromMacBridgePortConfigDataObject
                            (port, bridgeMeId, pFilterMeId);
                        // reset found to be FALSE to search for next one
                        if (ret != CMSRET_SUCCESS)
                            foundPort = FALSE;
                    }

                    _cmsObj_free((void **) &port);
                }
            }
        }

        _cmsObj_free((void **) &mapper);
    }

    // if MapperServiceProfileObject is found then exit
    if (foundMapper == TRUE)
        goto out;
}
    INIT_INSTANCE_ID_STACK(&iidStack);
    foundPort = FALSE;
    // if MapperServiceProfileObject is not found then
    // search MacBridgePortConfigDataObject
    while ((!foundPort) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        foundPort = ((port->tpPointer == tpMeId) &&
                     ((!multicast && port->tpType == OMCI_BP_TP_GEM_INTERWORKING) ||
                      (multicast && port->tpType == OMCI_BP_TP_MCAST_GEM_INTERWORKING)));
        // if MacBridgePortConfigDataObject is found then
        if (foundPort == TRUE)
        {
            // pass the search to MacBridgePortConfigDataObject
            ret = getVlanFilterMeIdFromMacBridgePortConfigDataObject
                (port, bridgeMeId, pFilterMeId);
            // reset found to be FALSE to search for next one
            if (ret != CMSRET_SUCCESS)
                foundPort = FALSE;
        }

        _cmsObj_free((void **) &port);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> getVlanFilterMeIdFromGemInterworkingTpObject, bridgeMeId=%d, filterMeId=%d, ret=%d\n", bridgeMeId, *pFilterMeId, ret);

    return ret;
}


// get management entity ID of MacBridgeServiceProfileObject
// from the given MacBridgePortConfigDataObject
CmsRet getBridgeMeIdFromMacBridgePortConfigDataObject
    (const MacBridgePortConfigDataObject *port, UINT32 *pBridgeMeId)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MacBridgeServiceProfileObject *bridge = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search mac bridge service profile object that has
    // its meId that matches with bridgeId of the given port
    while ((!found) &&
           (_cmsObj_getNextFlags
                (MDMOID_MAC_BRIDGE_SERVICE_PROFILE,
                 &iidStack,
                 OGF_NO_VALUE_UPDATE,
                 (void **) &bridge) == CMSRET_SUCCESS))
    {
        found = (port->bridgeId == bridge->managedEntityId);

        if (found == TRUE)
        {
            *pBridgeMeId = bridge->managedEntityId;
            ret = CMSRET_SUCCESS;
        }

        _cmsObj_free((void **) &bridge);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> getBridgeMeIdFromMacBridgePortConfigDataObject, portMeId=%d, bridgeMeId=%d, ret=%d\n",
                   port->managedEntityId, *pBridgeMeId, ret);

    return ret;
}

// get management entity ID of MacBridgePortConfigDataObject
// from the given GemInterworkingTpObject
CmsRet getBridgePortMeIdFromGemInterworkingTpObject
    (const  GemInterworkingTpObject *tp,
     const  UINT32 bridgeMeId,
     UINT32 *pPortMeId)
{
    UBOOL8 foundPort = FALSE, foundMapper = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MacBridgePortConfigDataObject *port = NULL;
    MapperServiceProfileObject *mapper = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search MacBridgePortConfigDataObject
    while ((!foundPort) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        foundPort = ((port->tpPointer == tp->managedEntityId) &&
                     (port->tpType == OMCI_BP_TP_GEM_INTERWORKING) &&
                     (port->bridgeId == bridgeMeId));
        if (foundPort == TRUE)
        {
            *pPortMeId = port->managedEntityId;
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &port);
    }

    if (foundPort == TRUE)
        goto out;

    INIT_INSTANCE_ID_STACK(&iidStack);
    // if not found then search MapperServiceProfileObject
    while ((!foundMapper) &&
           (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mapper) == CMSRET_SUCCESS))
    {
        // find any interworkTpPointerPriority that links to tp
        // find any interworkTpPointerPriority that links to tp
        foundMapper = ((mapper->interworkTpPointerPriority0 == tp->managedEntityId) ||
                       (mapper->interworkTpPointerPriority1 == tp->managedEntityId) ||
                       (mapper->interworkTpPointerPriority2 == tp->managedEntityId) ||
                       (mapper->interworkTpPointerPriority3 == tp->managedEntityId) ||
                       (mapper->interworkTpPointerPriority4 == tp->managedEntityId) ||
                       (mapper->interworkTpPointerPriority5 == tp->managedEntityId) ||
                       (mapper->interworkTpPointerPriority6 == tp->managedEntityId) ||
                       (mapper->interworkTpPointerPriority7 == tp->managedEntityId));

        // if MapperServiceProfileObject is found then
        if (foundMapper == TRUE)
        {
            if (mapper->tpType == OMCI_MS_TP_BRIDGE)
            {
                INIT_INSTANCE_ID_STACK(&iidStack);
                foundPort = FALSE;
                while ((!foundPort) &&
                       (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                                       &iidStack,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &port) == CMSRET_SUCCESS))
                {
                    foundPort = ((port->tpPointer == mapper->managedEntityId) &&
                                 (port->tpType == OMCI_BP_TP_MAPPER_SERVICE) &&
                                 (port->bridgeId == bridgeMeId));
                    // if MacBridgePortConfigDataObject is found then
                    if (foundPort == TRUE)
                    {
                        *pPortMeId = port->managedEntityId;
                        ret = CMSRET_SUCCESS;
                    }
                    _cmsObj_free((void **) &port);
                }
            }
        }

        _cmsObj_free((void **) &mapper);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> getBridgePortMeIdFromGemInterworkingTpObject, bridgeMeId=%d, tpMeId=%d, portMeId=%d, ret=%d\n",
                   bridgeMeId, tp->managedEntityId, *pPortMeId, ret);

    return ret;
}

// get management entity ID of MacBridgePortConfigDataObject
// from the given MulticastGemInterworkingTpObject
CmsRet getBridgePortMeIdFromMulticastGemInterworkingTpObject
    (const  MulticastGemInterworkingTpObject *mcast,
     const  UINT32 bridgeMeId,
     UINT32 *pPortMeId)
{
    UBOOL8 foundPort = FALSE, foundMapper = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MacBridgePortConfigDataObject *port = NULL;
    MapperServiceProfileObject *mapper = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search MacBridgePortConfigDataObject
    while ((!foundPort) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        foundPort = ((port->tpPointer == mcast->managedEntityId) &&
                     (port->tpType == OMCI_BP_TP_MCAST_GEM_INTERWORKING) &&
                     (port->bridgeId == bridgeMeId));
        if (foundPort == TRUE)
        {
            *pPortMeId = port->managedEntityId;
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &port);
    }

    if (foundPort == TRUE)
        goto out;

    INIT_INSTANCE_ID_STACK(&iidStack);
    // if MacBridgePortConfigDataObject is not found then
    // search MapperServiceProfileObject
    while ((!foundMapper) &&
           (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mapper) == CMSRET_SUCCESS))
    {
        // find any interworkTpPointerPriority that links to tp
        foundMapper = ((mapper->interworkTpPointerPriority0 == mcast->managedEntityId) ||
                       (mapper->interworkTpPointerPriority1 == mcast->managedEntityId) ||
                       (mapper->interworkTpPointerPriority2 == mcast->managedEntityId) ||
                       (mapper->interworkTpPointerPriority3 == mcast->managedEntityId) ||
                       (mapper->interworkTpPointerPriority4 == mcast->managedEntityId) ||
                       (mapper->interworkTpPointerPriority5 == mcast->managedEntityId) ||
                       (mapper->interworkTpPointerPriority6 == mcast->managedEntityId) ||
                       (mapper->interworkTpPointerPriority7 == mcast->managedEntityId));

        // if MapperServiceProfileObject is found then
        if (foundMapper == TRUE)
        {
            if (mapper->tpType == OMCI_MS_TP_BRIDGE)
            {
                INIT_INSTANCE_ID_STACK(&iidStack);
                foundPort = FALSE;
                while ((!foundPort) &&
                       (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                                       &iidStack,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &port) == CMSRET_SUCCESS))
                {
                    foundPort = ((port->tpPointer == mapper->managedEntityId) &&
                                 (port->tpType == OMCI_BP_TP_MAPPER_SERVICE) &&
                                 (port->bridgeId == bridgeMeId));
                    // if MacBridgePortConfigDataObject is found then
                    if (foundPort == TRUE)
                    {
                        *pPortMeId = port->managedEntityId;
                        ret = CMSRET_SUCCESS;
                    }
                    _cmsObj_free((void **) &port);
                }
            }
        }

        _cmsObj_free((void **) &mapper);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> getBridgePortMeIdFromMulticastGemInterworkingTpObject, bridgeMeId=%d, mcastMeId=%d, portMeId=%d, ret=%d\n",
                   bridgeMeId, mcast->managedEntityId, *pPortMeId, ret);

    return ret;
}

CmsRet getExtVlanOperMeIdFromUniMeId
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     UINT32 *pOperMeId)
{
    UBOOL8 foundPort = FALSE, foundVlan = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MacBridgePortConfigDataObject *port = NULL;
    ExtendedVlanTaggingOperationConfigurationDataObject *xVlan = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search MacBridgePortConfigDataObject that is
    // associated with the given UNI object
    while ((!foundPort) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        if (uniOid == MDMOID_PPTP_ETHERNET_UNI)
            foundPort = (port->tpPointer == uniMeId &&
                         port->tpType == OMCI_BP_TP_PPTP_ETH);
        else if (uniOid == MDMOID_PPTP_MOCA_UNI)
            foundPort = (port->tpPointer == uniMeId &&
                         port->tpType == OMCI_BP_TP_PPTP_MOCA);
        else if (uniOid == MDMOID_IP_HOST_CONFIG_DATA ||
                 uniOid == MDMOID_IPV6_HOST_CONFIG_DATA)
            foundPort = (port->tpPointer == uniMeId &&
                         port->tpType == OMCI_BP_TP_IP_HOST);
        else if (uniOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT)
            foundPort = (port->tpPointer == uniMeId &&
                         port->tpType == OMCI_BP_TP_VEIP);
        // if MacBridgePortConfigDataObject is found then
        // search ExtendedVlanTaggingOperationConfigurationDataObject that is
        // associated with MacBridgePortConfigDataObject
        if (foundPort == TRUE)
        {
            INIT_INSTANCE_ID_STACK(&iidStack);
            while ((!foundVlan) &&
                   (_cmsObj_getNextFlags
                        (MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA,
                         &iidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **) &xVlan) == CMSRET_SUCCESS))
            {
                foundVlan = (xVlan->associationType == OMCI_XVLAN_TP_MAC_BRIDGE_PORT &&
                             xVlan->associatedManagedEntityPointer == port->managedEntityId);
                // if ExtendedVlanTaggingOperationConfigurationDataObject
                // is found then return its MeId with SUCCESS
                if (foundVlan == TRUE)
                {
                    *pOperMeId = xVlan->managedEntityId;
                    ret = CMSRET_SUCCESS;
                }
                _cmsObj_free((void **) &xVlan);
            }
        }
        _cmsObj_free((void **) &port);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> getExtVlanOperMeIdFromUniMeId, meId=%d, ret=%d\n",
                   *pOperMeId, ret);

    return ret;
}

CmsRet getVlanOperMeIdFromUniMeId
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     UINT32 *pOperMeId)
{
    UBOOL8 foundPort = FALSE, foundVlan = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MacBridgePortConfigDataObject *port = NULL;
    VlanTaggingOperationConfigurationDataObject *vlan = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search MacBridgePortConfigDataObject that is
    // associated with the given UNI object
    while ((!foundPort) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        if (uniOid == MDMOID_PPTP_ETHERNET_UNI)
            foundPort = (port->tpPointer == uniMeId &&
                         port->tpType == OMCI_BP_TP_PPTP_ETH);
        else if (uniOid == MDMOID_PPTP_MOCA_UNI)
            foundPort = (port->tpPointer == uniMeId &&
                         port->tpType == OMCI_BP_TP_PPTP_MOCA);
        else if (uniOid == MDMOID_IP_HOST_CONFIG_DATA ||
                 uniOid == MDMOID_IPV6_HOST_CONFIG_DATA)
            foundPort = (port->tpPointer == uniMeId &&
                         port->tpType == OMCI_BP_TP_IP_HOST);
        else if (uniOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT)
            foundPort = (port->tpPointer == uniMeId &&
                         port->tpType == OMCI_BP_TP_VEIP);
        // if MacBridgePortConfigDataObject is found then
        // search VlanTaggingOperationConfigurationDataObject that is
        // associated with MacBridgePortConfigDataObject
        if (foundPort == TRUE)
        {
            INIT_INSTANCE_ID_STACK(&iidStack);
            while ((!foundVlan) &&
                   (_cmsObj_getNextFlags
                        (MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA,
                         &iidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **) &vlan) == CMSRET_SUCCESS))
            {
                foundVlan = ((vlan->associationType == OMCI_VLAN_TP_MAC_BRIDGE_PORT &&
                              vlan->associatedManagedEntityPointer == port->managedEntityId) ||
                             (vlan->associationType == OMCI_VLAN_TP_DEFAULT &&
                              vlan->managedEntityId == port->managedEntityId));
                // if VlanTaggingOperationConfigurationDataObject
                // is found then return its MeId with SUCCESS
                if (foundVlan == TRUE)
                {
                    *pOperMeId = vlan->managedEntityId;
                    ret = CMSRET_SUCCESS;
                }
                _cmsObj_free((void **) &vlan);
            }
        }
        _cmsObj_free((void **) &port);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> getVlanOperMeIdFromUniMeId, meId=%d, ret=%d\n",
                   *pOperMeId, ret);

    return ret;
}

UBOOL8 getPbitsFromMapperServiceProfile
    (const UINT32 tpMeId,
     const MapperServiceProfileObject *mapper,
     UINT8 *pbits)
{
    UBOOL8 found = FALSE;
    UINT32 i = 0;

    if (mapper->interworkTpPointerPriority0 == tpMeId)
        pbits[0] = 0;
    if (mapper->interworkTpPointerPriority1 == tpMeId)
        pbits[1] = 1;
    if (mapper->interworkTpPointerPriority2 == tpMeId)
        pbits[2] = 2;
    if (mapper->interworkTpPointerPriority3 == tpMeId)
        pbits[3] = 3;
    if (mapper->interworkTpPointerPriority4 == tpMeId)
        pbits[4] = 4;
    if (mapper->interworkTpPointerPriority5 == tpMeId)
        pbits[5] = 5;
    if (mapper->interworkTpPointerPriority6 == tpMeId)
        pbits[6] = 6;
    if (mapper->interworkTpPointerPriority7 == tpMeId)
        pbits[7] = 7;

    for (i = OMCI_FILTER_PRIO_MIN; i <= OMCI_FILTER_PRIO_MAX; i++)
    {
        if (pbits[i] < OMCI_FILTER_PRIO_NONE)
        {
            found = TRUE;
            break;
        }
    }

    return found;
}

//=======================  Public GPON Model functions ========================

CmsRet rutGpon_checkModelPath
    (UINT32 checkOid, UINT32 checkMeId, OmciObjectAction action)
{

    CmsRet ret = CMSRET_SUCCESS;

    if( action == OMCI_ACTION_DELETE )
    {
        ret = rutGpon_checkModelPath_US(checkOid, checkMeId, action);
    }

    return ret;
}

#if defined(CONFIG_BCM_GPON_802_1Q_ENABLED)
CmsRet configSwitchVlanDomains(vlanDomains_t *vlanDomain_p)
{
    int loopCnt, loopCnt1;
    vlanDomainInfo_t *domainInfo_p;
    vlanDomainInfo_t *oldDomainInfo_p;
    char cmd[BUFLEN_1024];
    UINT32 fwdMap, oldFwdMap;
    UBOOL8 bFound;

    /* First remove the unused old vlan domains */
    for (loopCnt=0; loopCnt < gblCurVlanDomain.numDomains; ++loopCnt)
    {
        bFound = FALSE;
        oldDomainInfo_p = &(gblCurVlanDomain.domainInfo[loopCnt]);
        for (loopCnt1=0; loopCnt1 < vlanDomain_p->numDomains; ++loopCnt1)
        {
            domainInfo_p = &(vlanDomain_p->domainInfo[loopCnt1]);
            if (domainInfo_p->vid == oldDomainInfo_p->vid)
            {
                bFound = TRUE; /* Found the vid in new config as well. No need to remove */
                break;
            }
        }
        if (bFound == FALSE)
        { /* Did not find this VID in new config, so remove it now. */
            snprintf(cmd, sizeof(cmd), "ethswctl -c vlan -v %d -f 0 -u 0",
                     oldDomainInfo_p->vid);
            rut_doSystemAction("rcl_gpon", cmd);
            printf("***OMCI:Remove Old vlan config *** %s ***\n",cmd);
        }
    }
    /* Go through new vlan domains and apply new-ones and only those are changed */
    for (loopCnt=0; loopCnt < vlanDomain_p->numDomains; ++loopCnt)
    {
        bFound = FALSE;
        domainInfo_p = &(vlanDomain_p->domainInfo[loopCnt]);
        for (loopCnt1=0; loopCnt1 < gblCurVlanDomain.numDomains; ++loopCnt1)
        {
            oldDomainInfo_p = &(gblCurVlanDomain.domainInfo[loopCnt1]);
            if (domainInfo_p->vid == oldDomainInfo_p->vid)
            { /* This VID is part of new vlan domain map as well. */
                fwdMap = (domainInfo_p->untaggedMap |domainInfo_p->taggedMap);
                oldFwdMap = (oldDomainInfo_p->untaggedMap |oldDomainInfo_p->taggedMap);
                if ((fwdMap == oldFwdMap) && (oldDomainInfo_p->untaggedMap == domainInfo_p->untaggedMap))
                {
                    bFound = TRUE; /* This vlan domain is exactly same as previous one, so no updates */
                    break;
                }
                else
                {
                    break; /* Break out from the loop - we are not going to find the same VID again */
                }
            }
        }
        if (bFound == FALSE)
        { /* Either this VID was not part of earlier vlan table, or
           * earlier vlan table entry is not exactly same as new one */
            domainInfo_p = &(vlanDomain_p->domainInfo[loopCnt]);
            fwdMap = (domainInfo_p->untaggedMap |domainInfo_p->taggedMap);
            snprintf(cmd, sizeof(cmd), "ethswctl -c vlan -v %d -f %d -u %d",
                     domainInfo_p->vid,fwdMap,domainInfo_p->untaggedMap);
            rut_doSystemAction("rcl_gpon", cmd);
            printf("***OMCI:Add vlan config *** %s ***\n",cmd);
        }
    }
    /* Now copy new vlan domains into current */
    memcpy(&gblCurVlanDomain,vlanDomain_p,sizeof(gblCurVlanDomain));
    return CMSRET_SUCCESS;
}
#endif /* CONFIG_BCM_GPON_802_1Q_ENABLED */
CmsRet rutGpon_setMcastGemInGponIf(void)
{
    GemPortNetworkCtpObject *ctp = NULL;
    InstanceIdStack iidGemCtp = EMPTY_INSTANCE_ID_STACK;
    UBOOL8 ctpFound = FALSE;
    UINT32 mcastGemIdx;

    INIT_INSTANCE_ID_STACK(&iidGemCtp);
    while (ctpFound == FALSE &&
           _cmsObj_getNextFlags(MDMOID_GEM_PORT_NETWORK_CTP,
                          &iidGemCtp,
                          OGF_NO_VALUE_UPDATE,
                          (void **) &ctp) == CMSRET_SUCCESS)
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                       "===> Checking Multi GEM for %d, Dir=%d\n",
                       ctp->portIdValue,ctp->direction);
        if (ctp->direction == OMCI_FLOW_DOWNSTREAM)
        {
            if (rutGpon_isGemPortNetworkCtpMulticast(ctp) == TRUE)
            {
                if (rutGpon_getIndexFromGemPortNetworkCtp(ctp, &mcastGemIdx) == CMSRET_SUCCESS)
                {
                    gponif_init();
                    gponif_setmcastgem(mcastGemIdx);
                    gponif_close();
                }
                ctpFound = TRUE;
            }
        }
        _cmsObj_free((void **) &ctp);
    }
    return CMSRET_SUCCESS;
}

/*****************************************************************************
*  FUNCTION:  rutGpon_configModelPath
*  PURPOSE:   The checkModelPath function that configures the whole model.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      The logic in rut_gpon_rule assumes unicast GEM port(s) are processed
*      before multicast/broadcast GEM port(s). However the OMCI sequence from
*      the OLT, and the corresponding GEM port order in the OMCI MIB maybe
*      different. This function first checks unicast GEM ports then others.
*
*      SWBCACPE-7547: add rutGpon_configModelPath() which is almost the same
*      rutGpon_checkModelPath_US() except it does not use any specific ME
*      information since it configures the whole model.
*****************************************************************************/
CmsRet rutGpon_configModelPath(void)
{
    UBOOL8 isNewUni = TRUE;
    UBOOL8 *newUniP = &isNewUni;
    PptpEthernetUniObject *pptpEthernet = NULL;
    PptpMocaUniObject *pptpMoca = NULL;
    Ipv6HostConfigDataObject *ipv6Host = NULL;
    IpHostConfigDataObject *ipHost = NULL;
    VirtualEthernetInterfacePointObject *veip = NULL;
    InstanceIdStack iidEth = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidMoca = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidIpv6Host = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidIpHost = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVeip = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

#if defined(CONFIG_BCM_GPON_802_1Q_ENABLED)
    memset(&gblNewVlanDomain, 0, sizeof(gblNewVlanDomain));
#endif /* CONFIG_BCM_GPON_802_1Q_ENABLED */

    while (_cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                          &iidEth,
                          OGF_NO_VALUE_UPDATE,
                          (void **) &pptpEthernet) == CMSRET_SUCCESS)
    {
        if (rutGpon_getUniDataPathMode() == TRUE)
        {
            if (omciDm_getUniEntryConfigState(MDMOID_PPTP_ETHERNET_UNI,
              pptpEthernet->managedEntityId) == FALSE)
            {
                cmsLog_notice("UNI data path mode TRUE, skip PPTP_ETHERNET_UNI %d",
                  pptpEthernet->managedEntityId);
                _cmsObj_free((void **) &pptpEthernet);
                continue;
            }
        }

        *newUniP = TRUE;
        ret = rutGpon_genericCheckModelPath(
                 OMCI_ACTION_CREATE,
                 MDMOID_PPTP_ETHERNET_UNI,
                 pptpEthernet->managedEntityId,
                 OMCI_FLOW_UPSTREAM,
                 MDMOID_PPTP_ETHERNET_UNI,
                 FALSE,
                 (OMciObjCmnHdr_t*)pptpEthernet,
                 newUniP,
                 (uniObject_searchModelPath)
                 PptpEthernetUniObject_searchModelPath);
        ret = rutGpon_genericCheckModelPath(
                 OMCI_ACTION_CREATE,
                 MDMOID_PPTP_ETHERNET_UNI,
                 pptpEthernet->managedEntityId,
                 OMCI_FLOW_DOWNSTREAM,
                 MDMOID_PPTP_ETHERNET_UNI,
                 FALSE,
                 (OMciObjCmnHdr_t*)pptpEthernet,
                 newUniP,
                 (uniObject_searchModelPath)
                 PptpEthernetUniObject_searchModelPath);

        _cmsObj_free((void **) &pptpEthernet);
    }

    while (_cmsObj_getNextFlags(MDMOID_PPTP_MOCA_UNI,
                          &iidMoca,
                          OGF_NO_VALUE_UPDATE,
                          (void **) &pptpMoca) == CMSRET_SUCCESS)
    {
        *newUniP = TRUE;
        ret = rutGpon_genericCheckModelPath(
                 OMCI_ACTION_CREATE,
                 MDMOID_PPTP_MOCA_UNI,
                 pptpMoca->managedEntityId,
                 OMCI_FLOW_UPSTREAM,
                 MDMOID_PPTP_MOCA_UNI,
                 FALSE,
                 (OMciObjCmnHdr_t*)pptpMoca,
                 newUniP,
                 (uniObject_searchModelPath)
                 PptpMocaUniObject_searchModelPath);
        ret = rutGpon_genericCheckModelPath(
                 OMCI_ACTION_CREATE,
                 MDMOID_PPTP_MOCA_UNI,
                 pptpMoca->managedEntityId,
                 OMCI_FLOW_DOWNSTREAM,
                 MDMOID_PPTP_MOCA_UNI,
                 FALSE,
                 (OMciObjCmnHdr_t*)pptpMoca,
                 newUniP,
                 (uniObject_searchModelPath)
                 PptpMocaUniObject_searchModelPath);

        _cmsObj_free((void **) &pptpMoca);
    }

    while (_cmsObj_getNextFlags(MDMOID_IPV6_HOST_CONFIG_DATA,
                          &iidIpv6Host,
                          OGF_NO_VALUE_UPDATE,
                          (void **) &ipv6Host) == CMSRET_SUCCESS)
    {
        *newUniP = TRUE;
        ret = rutGpon_genericCheckModelPath(
                 OMCI_ACTION_CREATE,
                 MDMOID_IPV6_HOST_CONFIG_DATA,
                 ipv6Host->managedEntityId,
                 OMCI_FLOW_UPSTREAM,
                 MDMOID_IPV6_HOST_CONFIG_DATA,
                 FALSE,
                 (OMciObjCmnHdr_t*)ipv6Host,
                 newUniP,
                 (uniObject_searchModelPath)
                 Ipv6HostConfigDataObject_searchModelPath);
        ret = rutGpon_genericCheckModelPath(
                 OMCI_ACTION_CREATE,
                 MDMOID_IPV6_HOST_CONFIG_DATA,
                 ipv6Host->managedEntityId,
                 OMCI_FLOW_DOWNSTREAM,
                 MDMOID_IPV6_HOST_CONFIG_DATA,
                 FALSE,
                 (OMciObjCmnHdr_t*)ipv6Host,
                 newUniP,
                 (uniObject_searchModelPath)
                 Ipv6HostConfigDataObject_searchModelPath);

        // Ipv6HostConfigDataObject does need to search for all
        // GemPortNetworkCtpObject to create all rules that are
        // associated with it
        _cmsObj_free((void **) &ipv6Host);
    }

    while (_cmsObj_getNextFlags(MDMOID_IP_HOST_CONFIG_DATA,
                          &iidIpHost,
                          OGF_NO_VALUE_UPDATE,
                          (void **) &ipHost) == CMSRET_SUCCESS)
    {
        *newUniP = TRUE;
        ret = rutGpon_genericCheckModelPath(
                 OMCI_ACTION_CREATE,
                 MDMOID_IP_HOST_CONFIG_DATA,
                 ipHost->managedEntityId,
                 OMCI_FLOW_UPSTREAM,
                 MDMOID_IP_HOST_CONFIG_DATA,
                 FALSE,
                 (OMciObjCmnHdr_t*)ipHost,
                 newUniP,
                 (uniObject_searchModelPath)
                 IpHostConfigDataObject_searchModelPath);
        ret = rutGpon_genericCheckModelPath(
                 OMCI_ACTION_CREATE,
                 MDMOID_IP_HOST_CONFIG_DATA,
                 ipHost->managedEntityId,
                 OMCI_FLOW_DOWNSTREAM,
                 MDMOID_IP_HOST_CONFIG_DATA,
                 FALSE,
                 (OMciObjCmnHdr_t*)ipHost,
                 newUniP,
                 (uniObject_searchModelPath)
                 IpHostConfigDataObject_searchModelPath);

        // IpHostConfigDataObject does need to search for all
        // GemPortNetworkCtpObject to create all rules that are
        // associated with it
        _cmsObj_free((void **) &ipHost);
    }

    while (_cmsObj_getNextFlags(MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                          &iidVeip,
                          OGF_NO_VALUE_UPDATE,
                          (void **) &veip) == CMSRET_SUCCESS)
    {
        *newUniP = TRUE;
        ret = rutGpon_genericCheckModelPath(
                 OMCI_ACTION_CREATE,
                 MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                 veip->managedEntityId,
                 OMCI_FLOW_UPSTREAM,
                 MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                 FALSE,
                 (OMciObjCmnHdr_t*)veip,
                 newUniP,
                 (uniObject_searchModelPath)
                 VirtualEthernetInterfacePointObject_searchModelPath);
        ret = rutGpon_genericCheckModelPath(
                 OMCI_ACTION_CREATE,
                 MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                 veip->managedEntityId,
                 OMCI_FLOW_DOWNSTREAM,
                 MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                 FALSE,
                 (OMciObjCmnHdr_t*)veip,
                 newUniP,
                 (uniObject_searchModelPath)
                 VirtualEthernetInterfacePointObject_searchModelPath);

        _cmsObj_free((void **) &veip);
    }
    /* Set the multicast gem port index in gponif */
    rutGpon_setMcastGemInGponIf();
#if defined(CONFIG_BCM_GPON_802_1Q_ENABLED)
    /* Now configure the vlan table in switch */
    configSwitchVlanDomains(&gblNewVlanDomain);
#endif /* CONFIG_BCM_GPON_802_1Q_ENABLED */
    return ret;
}

CmsRet rutGpon_getEthernetLanNum( UINT32 * ethLanNum)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *omciSysObj = NULL;

    if ((ret = _cmsObj_get(MDMOID_OMCI_SYSTEM,
                          &iidStack,
                          OGF_NO_VALUE_UPDATE,
                          (void *) &omciSysObj)) == CMSRET_SUCCESS)
    {
        *ethLanNum= omciSysObj->numberOfEthernetPorts;
        _cmsObj_free((void **) &omciSysObj);
    }

    return ret;
}

CmsRet rutGpon_getFirstEthernetMeId(UINT32 *meId)
{
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *omciSysObj = NULL;

    if ((ret = _cmsObj_get(MDMOID_OMCI_SYSTEM,
                          &iidStack,
                          OGF_NO_VALUE_UPDATE,
                          (void *) &omciSysObj)) == CMSRET_SUCCESS)
    {
        *meId = omciSysObj->ethernetManagedEntityId1;
        _cmsObj_free((void **) &omciSysObj);
    }
#else
    PptpEthernetUniObject *pptpEthernet = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    if ((ret = _cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &pptpEthernet)) == CMSRET_SUCCESS)
    {
        *meId = pptpEthernet->managedEntityId;
        _cmsObj_free((void **) &pptpEthernet);
    }
#endif    // DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1

    return ret;
}

CmsRet rutGpon_getFirstMocaMeId(UINT32 *meId)
{
    PptpMocaUniObject *pptpMoca = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    if (_cmsObj_getNextFlags(MDMOID_PPTP_MOCA_UNI,
                       &iidStack,
                       OGF_NO_VALUE_UPDATE,
                       (void **) &pptpMoca) == CMSRET_SUCCESS)
    {
        *meId = pptpMoca->managedEntityId;
        ret = CMSRET_SUCCESS;
        _cmsObj_free((void **) &pptpMoca);
    }

    return ret;
}

#ifdef SUPPORT_GPONCTL

CmsRet rutGpon_getIndexFromGemPortNetworkCtp
    (const GemPortNetworkCtpObject *ctp, UINT32 *gemPortIndex)
{
    CmsRet ret = CMSRET_SUCCESS;

    UINT16 drvIdx;
    ret = omciDm_FindGemEntry(ctp->managedEntityId, &drvIdx);
    if (ret == CMSRET_SUCCESS)
    {
        *gemPortIndex = drvIdx;
    }
    else
    {
        cmsLog_error("Cannot get GEM Port Index for portId %d, ret=%d", ctp->portIdValue, ret);
        ret = CMSRET_INTERNAL_ERROR;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL, "===> rutGpon_getIndexFromGemPortNetworkCtp, portIdValue %d index=%d, ret=%d\n",
                    ctp->portIdValue, *gemPortIndex, ret);

    return ret;
}

#endif    //#ifdef SUPPORT_GPONCTL

CmsRet rutGpon_getPbitsFromGemPortNeworkCtp
    (const  GemPortNetworkCtpObject *ctp,
     UINT8  *pbits,
     UINT32 *mapperMeId)
{
    UBOOL8 foundTp = FALSE, foundMcast = FALSE, foundMapper = FALSE;
    GemInterworkingTpObject *tp = NULL;
    MulticastGemInterworkingTpObject *mtp = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    while ((!foundTp) &&
           (_cmsObj_getNextFlags(MDMOID_GEM_INTERWORKING_TP,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &tp) == CMSRET_SUCCESS))
    {
        foundTp = (tp->gemPortNetworkCtpConnPointer == ctp->managedEntityId);
        // GemInterworkingTpObject is found
        if (foundTp)
        {
            MapperServiceProfileObject *mapper = NULL;
            INIT_INSTANCE_ID_STACK(&iidStack);
            // search MapperServiceProfileObject
            while ((!foundMapper) &&
                   (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mapper) == CMSRET_SUCCESS))
            {
                foundMapper = (tp->serviceProfilePointer == mapper->managedEntityId);
                // if MapperServiceProfileObject is found then
                if (foundMapper == TRUE)
                {
                    getPbitsFromMapperServiceProfile(tp->managedEntityId, mapper, pbits);
                }
                // since there is no link from serviceProfilePointer of tp
                // to mapper service profile, try to find any
                // interworkTpPointerPriority that links to tp
                else
                {
                    foundMapper = getPbitsFromMapperServiceProfile(tp->managedEntityId, mapper, pbits);
                }
                if (foundMapper == TRUE)
                {
                    *mapperMeId = mapper->managedEntityId;
                    ret = CMSRET_SUCCESS;
                }

                _cmsObj_free((void **) &mapper);
            }
        }
        _cmsObj_free((void **) &tp);
    }

    if (ret == CMSRET_SUCCESS)
        goto out;

    // if cannot find GemInterworkingTpObject then
    // try to find MulticastGemInterworkingTpObject
    INIT_INSTANCE_ID_STACK(&iidStack);
    while ((!foundMcast) &&
           (_cmsObj_getNextFlags(MDMOID_MULTICAST_GEM_INTERWORKING_TP,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mtp) == CMSRET_SUCCESS))
    {
        foundMcast = (mtp->gemPortNetworkCtpConnPointer == ctp->managedEntityId);
        // MulticastGemInterworkingTpObject is found
        if (foundMcast == TRUE)
        {
            MapperServiceProfileObject *mapper = NULL;
            INIT_INSTANCE_ID_STACK(&iidStack);
            foundMapper = FALSE;
            // search MapperServiceProfileObject
            while ((!foundMapper) &&
                   (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mapper) == CMSRET_SUCCESS))
            {
                foundMapper = (mtp->serviceProfilePointer == mapper->managedEntityId);
                // if MapperServiceProfileObject is found then
                if (foundMapper == TRUE)
                {
                    getPbitsFromMapperServiceProfile(mtp->managedEntityId, mapper, pbits);
                }
                // since there is no link from serviceProfilePointer of mtp
                // to mapper service profile, try to find any
                // interworkTpPointerPriority that links to mtp
                else
                {
                    foundMapper = getPbitsFromMapperServiceProfile(mtp->managedEntityId, mapper, pbits);
                }
                if (foundMapper == TRUE)
                {
                    *mapperMeId = mapper->managedEntityId;
                    ret = CMSRET_SUCCESS;
                }

                _cmsObj_free((void **) &mapper);
            }
        }
        _cmsObj_free((void **) &mtp);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getPbitsFromGemPortNeworkCtp, ctpMeId=%d, pbits=[%d|%d|%d|%d|%d|%d|%d,%d], mapperMeId=%d, ret=%d\n",
                   ctp->managedEntityId, pbits[0], pbits[1], pbits[2], pbits[3], pbits[4], pbits[5], pbits[6], pbits[7], *mapperMeId, ret);

    return ret;
}

CmsRet rutGpon_getDscpToPbitFromGemPortNeworkCtp
    (const  GemPortNetworkCtpObject *ctp,
     OmciDscpPbitMappingInfo_t *dscpToPbit)
{
    UBOOL8 foundTp = FALSE, foundMcast = FALSE, foundMapper = FALSE;
    GemInterworkingTpObject *tp = NULL;
    MulticastGemInterworkingTpObject *mtp = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    while ((!foundTp) &&
           (_cmsObj_getNextFlags(MDMOID_GEM_INTERWORKING_TP,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &tp) == CMSRET_SUCCESS))
    {
        foundTp = (tp->gemPortNetworkCtpConnPointer == ctp->managedEntityId);
        // GemInterworkingTpObject is found
        if (foundTp)
        {
            MapperServiceProfileObject *mapper = NULL;
            INIT_INSTANCE_ID_STACK(&iidStack);
            // search MapperServiceProfileObject
            while ((!foundMapper) &&
                   (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mapper) == CMSRET_SUCCESS))
            {
                foundMapper = (tp->serviceProfilePointer == mapper->managedEntityId);
                // if MapperServiceProfileObject is found then
                if (foundMapper == TRUE)
                {
                    ret = rutGpon_getDscpToPbitFromMapperServiceProfile(mapper, dscpToPbit);
                }

                _cmsObj_free((void **) &mapper);
            }
        }
        _cmsObj_free((void **) &tp);
    }

    if (foundMapper == TRUE)
        goto out;

    // if cannot find GemInterworkingTpObject then
    // try to find MulticastGemInterworkingTpObject
    INIT_INSTANCE_ID_STACK(&iidStack);
    while ((!foundMcast) &&
           (_cmsObj_getNextFlags(MDMOID_MULTICAST_GEM_INTERWORKING_TP,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mtp) == CMSRET_SUCCESS))
    {
        foundMcast = (mtp->gemPortNetworkCtpConnPointer == ctp->managedEntityId);
        // MulticastGemInterworkingTpObject is found
        if (foundMcast == TRUE)
        {
            MapperServiceProfileObject *mapper = NULL;
            INIT_INSTANCE_ID_STACK(&iidStack);
            foundMapper = FALSE;
            // search MapperServiceProfileObject
            while ((!foundMapper) &&
                   (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                                   &iidStack,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &mapper) == CMSRET_SUCCESS))
            {
                foundMapper = (mtp->serviceProfilePointer == mapper->managedEntityId);
                // if MapperServiceProfileObject is found then
                if (foundMapper == TRUE)
                {
                    ret = rutGpon_getDscpToPbitFromMapperServiceProfile(mapper, dscpToPbit);
                }

                _cmsObj_free((void **) &mapper);
            }
        }
        _cmsObj_free((void **) &mtp);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getDscpToPbitFromGemPortNeworkCtp, ctpMeId=%d, option=%d, defaultPbit=%d, ret=%d\n",
                   ctp->managedEntityId, dscpToPbit->unmarkedFrameOption, dscpToPbit->defaultPBitMarking, ret);

    return ret;
}

CmsRet rutGpon_getDscpToPbitFromUniMeId
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     OmciDscpPbitMappingInfo_t *dscpToPbit)
{
    UBOOL8 found = FALSE, isPointerUsed = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    ExtendedVlanTaggingOperationConfigurationDataObject *xVlan = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // checking any extended vlan tagging operation data
    // that is attached to the given Uni MeId
    // by searching vlan object that has
    // its meId or associatedManagedEntityPointer
    // that matches with the given uniMeId
    while ((!found) &&
           (_cmsObj_getNextFlags
                (MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA,
                 &iidStack,
                 OGF_NO_VALUE_UPDATE,
                 (void **) &xVlan) == CMSRET_SUCCESS))
    {
        isPointerUsed = ((xVlan->associationType == OMCI_XVLAN_TP_PPTP_ETH) ||
                         (xVlan->associationType == OMCI_XVLAN_TP_PPTP_MOCA) ||
                         (xVlan->associationType == OMCI_XVLAN_TP_IP_HOST) ||
                         (xVlan->associationType == OMCI_XVLAN_TP_VEIP));

        if (isPointerUsed == TRUE)
            found = (uniMeId == xVlan->associatedManagedEntityPointer);
        else
            found = (uniMeId == xVlan->managedEntityId);

        if (found == TRUE)
        {
            ret = rutGpon_getDscpToPbitFromExtendedVlanTaggingOperationConfigurationData
                      (xVlan, dscpToPbit);
        }
        _cmsObj_free((void **) &xVlan);
    }

    // if ExtendedVlanTaggingOperationConfigurationDataObject is not
    // associated with UNI object directly then
    // search it from MacBridgePortConfigDataObject
    if (found == FALSE)
    {
        UINT32 operMeId = 0;

        ret = getExtVlanOperMeIdFromUniMeId(uniOid, uniMeId, &operMeId);

        if (ret == CMSRET_SUCCESS)
        {
            // reset ret to continue searching
            ret = CMSRET_OBJECT_NOT_FOUND;
            // search xVlan that has managedEntityId match with operMeId
            while ((!found) &&
                   (_cmsObj_getNextFlags
                        (MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA,
                         &iidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **) &xVlan) == CMSRET_SUCCESS))
            {
                found = (operMeId == xVlan->managedEntityId);
                if (found == TRUE)
                {
                    ret = rutGpon_getDscpToPbitFromExtendedVlanTaggingOperationConfigurationData
                              (xVlan, dscpToPbit);
                }
                _cmsObj_free((void **) &xVlan);
            }
        }
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getDscpToPbitFromUniMeId, uniOid=%d, uniMeId=%d, ret=%d\n",
                   uniOid, uniMeId, ret);

    return ret;
}

CmsRet rutGpon_getQidFromGemPortIndex
    (const UINT32 gemPortIndex, const OmciFLowDirection dir, UINT16 *pQid)
{
    UBOOL8 found = FALSE;

    if (dir == OMCI_FLOW_UPSTREAM)
    {
        found = omciDm_getGemPortIdxUsQid(gemPortIndex, pQid);
    }
    else
    {
        found = omciDm_getGemPortIdxDsQid(gemPortIndex, pQid);
    }

    return ((found == TRUE) && (*pQid != OMCI_QID_NOT_EXIST))?
      CMSRET_SUCCESS: CMSRET_OBJECT_NOT_FOUND;
}

UBOOL8 rutGpon_isGemPortNetworkCtpMulticast
    (const GemPortNetworkCtpObject *ctp)
{
    UBOOL8 found = FALSE;
    MulticastGemInterworkingTpObject *mcast = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MULTICAST_GEM_INTERWORKING_TP,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mcast) == CMSRET_SUCCESS))
    {
        found = (mcast->gemPortNetworkCtpConnPointer == ctp->managedEntityId);
        _cmsObj_free((void **) &mcast);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_isGemPortNetworkCtpMulticast, ctpMeId=%d, ret=%d\n",
                   ctp->managedEntityId, found);

    return found;
}

UBOOL8 rutGpon_isGemPortNetworkCtpIncidentBroadcast
    (const GemPortNetworkCtpObject *ctp)
{
    UBOOL8 found = FALSE;
    GemInterworkingTpObject *tp = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    if (ctp->direction == OMCI_FLOW_DOWNSTREAM)
    {
        // search GemInterworkingTpObject
        while ((!found) &&
               (_cmsObj_getNextFlags(MDMOID_GEM_INTERWORKING_TP,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &tp) == CMSRET_SUCCESS))
        {
            found = (tp->gemPortNetworkCtpConnPointer == ctp->managedEntityId);
            _cmsObj_free((void **) &tp);
        }
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_isGemPortNetworkCtpIncidentBroadcast, ctpMeId=%d, ret=%d\n",
                   ctp->managedEntityId, found);

    return found;
}

CmsRet rutGpon_getVlanFilterMeIdFromGemPortNetworkCtpMeId
    (const UINT32 ctpMeId,
     const UINT32 bridgeMeId,
     UINT32 *pFilterMeId)
{
    UBOOL8 foundTp = FALSE, foundMcast = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    GemInterworkingTpObject *tp = NULL;
    MulticastGemInterworkingTpObject *mcast = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search GemInterworkingTpObject
    while ((!foundTp) &&
           (_cmsObj_getNextFlags(MDMOID_GEM_INTERWORKING_TP,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &tp) == CMSRET_SUCCESS))
    {
        foundTp = (tp->gemPortNetworkCtpConnPointer == ctpMeId);
        // if GemInterworkingTpObject is found then
        // pass the search to this object
        if (foundTp == TRUE)
            ret = getVlanFilterMeIdFromGemInterworkingTpObject
                      (tp->managedEntityId, bridgeMeId, 0, pFilterMeId);
        _cmsObj_free((void **) &tp);
    }

    if (ret == CMSRET_SUCCESS)
        goto out;

    INIT_INSTANCE_ID_STACK(&iidStack);
    // if GemInterworkingTpObject is not found then
    // search MulticastGemInterworkingTpObject
    while ((!foundMcast) &&
           (_cmsObj_getNextFlags(MDMOID_MULTICAST_GEM_INTERWORKING_TP,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mcast) == CMSRET_SUCCESS))
    {
        foundMcast = (mcast->gemPortNetworkCtpConnPointer == ctpMeId);
        // if MulticastGemInterworkingTpObject is found then
        // pass the search to this object
        if (foundMcast == TRUE)
            ret = getVlanFilterMeIdFromGemInterworkingTpObject
                      (mcast->managedEntityId, bridgeMeId, 1, pFilterMeId);
        _cmsObj_free((void **) &mcast);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getVlanFilterMeIdFromGemPortNetworkCtpMeId, ctpMeId=%d, bridgeMeId=%d, filterMeId=%d, ret=%d\n",
                   ctpMeId, bridgeMeId, *pFilterMeId, ret);

    return ret;
}

CmsRet rutGpon_getBridgeMeIdFromUniMeId
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     UINT32 *pBridgeMeId)
{
    UBOOL8 foundPort = FALSE, foundMapper = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MacBridgePortConfigDataObject *port = NULL;
    MapperServiceProfileObject *mapper = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search MacBridgePortConfigDataObject
    while ((!foundPort) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        switch (uniOid)
        {
            case MDMOID_PPTP_ETHERNET_UNI:
                foundPort = ((port->tpType == OMCI_BP_TP_PPTP_ETH) && (port->tpPointer == uniMeId));
                break;
            case MDMOID_PPTP_MOCA_UNI:
                foundPort = ((port->tpType == OMCI_BP_TP_PPTP_MOCA) && (port->tpPointer == uniMeId));
                break;
            case MDMOID_IP_HOST_CONFIG_DATA:
            case MDMOID_IPV6_HOST_CONFIG_DATA:
                foundPort = ((port->tpType == OMCI_BP_TP_IP_HOST) && (port->tpPointer == uniMeId));
                break;
            case MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT:
                foundPort = ((port->tpType == OMCI_BP_TP_VEIP) && (port->tpPointer == uniMeId));
                break;
            default:
                foundPort = FALSE;
                break;
        }
        // if MacBridgePortConfigDataObject is found then
        // return Bridge Id of MacBridgePortConfigDataObject
        if (foundPort == TRUE)
        {
            *pBridgeMeId = port->bridgeId;
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &port);
    }

    if (ret == CMSRET_SUCCESS)
        goto out;

    INIT_INSTANCE_ID_STACK(&iidStack);
    // if not found then search MapperServiceProfileObject
    while ((!foundMapper) &&
           (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mapper) == CMSRET_SUCCESS))
    {
        foundMapper = (mapper->tpPointer == uniMeId);
        // if MapperServiceProfileObject is found then
        // return MeId of MapperServiceProfileObject
        if (foundMapper == TRUE)
        {
            *pBridgeMeId = mapper->managedEntityId;
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &mapper);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getBridgeMeIdFromUniMeId, uniOid=%d, uniMeId=%d, bridgeMeId=%d, ret=%d\n",
                   uniOid, uniMeId, *pBridgeMeId, ret);

    return ret;
}

CmsRet rutGpon_getBridgePortMeIdFromGemPortNetworkCtpMeId
    (const UINT32 ctpMeId,
     const UINT32 bridgeMeId,
     UINT32 *pPortMeId)
{
    UBOOL8 foundTp = FALSE, foundMcast = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    GemInterworkingTpObject *tp = NULL;
    MulticastGemInterworkingTpObject *mcast = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search GemInterworkingTpObject
    while ((!foundTp) &&
           (_cmsObj_getNextFlags(MDMOID_GEM_INTERWORKING_TP,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &tp) == CMSRET_SUCCESS))
    {
        foundTp = (tp->gemPortNetworkCtpConnPointer == ctpMeId);
        // if GemInterworkingTpObject is found then
        // pass the search to this object
        if (foundTp == TRUE)
            ret = getBridgePortMeIdFromGemInterworkingTpObject
                       (tp, bridgeMeId, pPortMeId);
        _cmsObj_free((void **) &tp);
    }

    if (ret == CMSRET_SUCCESS)
        goto out;

    INIT_INSTANCE_ID_STACK(&iidStack);
    // if not found then search MulticastGemInterworkingTpObject
    while ((!foundMcast) &&
           (_cmsObj_getNextFlags(MDMOID_MULTICAST_GEM_INTERWORKING_TP,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &mcast) == CMSRET_SUCCESS))
    {
        foundMcast = (mcast->gemPortNetworkCtpConnPointer == ctpMeId);
        // if MulticastGemInterworkingTpObject is found then
        // pass the search to this object
        if (foundMcast == TRUE)
            ret = getBridgePortMeIdFromMulticastGemInterworkingTpObject
                       (mcast, bridgeMeId, pPortMeId);
        _cmsObj_free((void **) &mcast);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getBridgePortMeIdFromGemPortNetworkCtpMeId, ctpMeId=%d, bridgeMeId=%d, portMeId=%d, ret=%d\n",
                   ctpMeId, bridgeMeId, *pPortMeId, ret);

    return ret;
}

UBOOL8 rutGpon_isBridgeConnectedToAnyGemPorts
    (const UINT32 ctpMeId,
     const UINT32 bridgeMeId)
{
    UBOOL8 found = FALSE;
    UINT32 portMeId = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    GemPortNetworkCtpObject *ctp = NULL;

    // search GemPortNetworkCtpObject
    while ((found == FALSE) &&
           (_cmsObj_getNextFlags(MDMOID_GEM_PORT_NETWORK_CTP,
                          &iidStack,
                          OGF_NO_VALUE_UPDATE,
                          (void **) &ctp) == CMSRET_SUCCESS))
    {
        // only find ctp that does not match with the given ctp
        if (ctp->managedEntityId != ctpMeId)
        {
            // find bridge port that connects bridge to gem port
            if (rutGpon_getBridgePortMeIdFromGemPortNetworkCtpMeId
                        (ctp->managedEntityId, bridgeMeId, &portMeId) == CMSRET_SUCCESS)
                found = TRUE;
        }
        _cmsObj_free((void **) &ctp);
    }

    return found;
}

CmsRet rutGpon_getBridgePortMeIdFromUniMeId
    (const  UINT32 uniOid,
     const  UINT32 uniMeId,
     UINT32 *pPortMeId)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    MacBridgePortConfigDataObject *port = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search MacBridgePortConfigDataObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &port) == CMSRET_SUCCESS))
    {
        switch (uniOid)
        {
            case MDMOID_PPTP_ETHERNET_UNI:
                found = ((port->tpType == OMCI_BP_TP_PPTP_ETH) && (port->tpPointer == uniMeId));
                break;
            case MDMOID_PPTP_MOCA_UNI:
                found = ((port->tpType == OMCI_BP_TP_PPTP_MOCA) && (port->tpPointer == uniMeId));
                break;
            case MDMOID_IP_HOST_CONFIG_DATA:
            case MDMOID_IPV6_HOST_CONFIG_DATA:
                found = ((port->tpType == OMCI_BP_TP_IP_HOST) && (port->tpPointer == uniMeId));
                break;
            case MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT:
                found = ((port->tpType == OMCI_BP_TP_VEIP) && (port->tpPointer == uniMeId));
                break;
            default:
                found = FALSE;
                break;
        }
        // if MacBridgePortConfigDataObject is found then
        // return managedEntityId of MacBridgePortConfigDataObject
        if (found == TRUE)
        {
            *pPortMeId = port->managedEntityId;
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &port);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getBridgePortMeIdFromUniMeId, uniOid=%d, uniMeId=%d, portMeId=%d, ret=%d\n",
                   uniOid, uniMeId, *pPortMeId, ret);

    return ret;
}

CmsRet rutGpon_getExtVlanOperMeIdFromUniMeId
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     UINT32 *pOperMeId)
{
    UBOOL8 found = FALSE, isPointerUsed = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    ExtendedVlanTaggingOperationConfigurationDataObject *xVlan = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // checking any extended vlan tagging operation data
    // that is attached to the given Uni MeId
    // by searching vlan object that has
    // its meId or associatedManagedEntityPointer
    // that matches with the given uniMeId
    while ((!found) &&
           (_cmsObj_getNextFlags
                (MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA,
                 &iidStack,
                 OGF_NO_VALUE_UPDATE,
                 (void **) &xVlan) == CMSRET_SUCCESS))
    {
        isPointerUsed = ((xVlan->associationType == OMCI_XVLAN_TP_PPTP_ETH) ||
                         (xVlan->associationType == OMCI_XVLAN_TP_PPTP_MOCA) ||
                         (xVlan->associationType == OMCI_XVLAN_TP_IP_HOST) ||
                         (xVlan->associationType == OMCI_XVLAN_TP_VEIP));

        if (isPointerUsed == TRUE)
            found = (uniMeId == xVlan->associatedManagedEntityPointer);
        else
            found = (uniMeId == xVlan->managedEntityId);

        if (found == TRUE)
        {
            *pOperMeId = xVlan->managedEntityId;
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &xVlan);
    }

    // if ExtendedVlanTaggingOperationConfigurationDataObject is not
    // associated with UNI object directly then
    // search it from MacBridgePortConfigDataObject
    if (found == FALSE)
    {
        ret = getExtVlanOperMeIdFromUniMeId(uniOid, uniMeId, pOperMeId);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getExtVlanOperMeIdFromUniMeId, meId=%d, ret=%d\n",
                   *pOperMeId, ret);

    return ret;
}

CmsRet rutGpon_getVlanOperMeIdFromUniMeId
    (const UINT32 uniOid,
     const UINT32 uniMeId,
     UINT32 *pOperMeId)
{
    UBOOL8 found = FALSE, isPointerUsed = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    VlanTaggingOperationConfigurationDataObject *vlan = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // checking any vlan tagging operation data
    // that is attached to the given Uni MeId
    // by searching vlan object that has
    // its meId or associatedManagedEntityPointer
    // that matches with the given uniMeId
    while ((!found) &&
           (_cmsObj_getNextFlags
                (MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA,
                 &iidStack,
                 OGF_NO_VALUE_UPDATE,
                 (void **) &vlan) == CMSRET_SUCCESS))
    {
        isPointerUsed = ((vlan->associationType == OMCI_VLAN_TP_PPTP_ETH) ||
                         (vlan->associationType == OMCI_VLAN_TP_PPTP_MOCA) ||
                         (vlan->associationType == OMCI_VLAN_TP_IP_HOST) ||
                         (vlan->associationType == OMCI_VLAN_TP_VEIP));

        if (isPointerUsed == TRUE)
            found = (uniMeId == vlan->associatedManagedEntityPointer);
        else
            found = (uniMeId == vlan->managedEntityId);

        if (found == TRUE)
        {
            *pOperMeId = vlan->managedEntityId;
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &vlan);
    }

    // if VlanTaggingOperationConfigurationDataObject is not
    // associated with UNI object directly then
    // search it from MacBridgePortConfigDataObject
    if (found == FALSE)
        ret = getVlanOperMeIdFromUniMeId(uniOid, uniMeId, pOperMeId);

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getVlanOperMeIdFromUniMeId, meId=%d, ret=%d\n",
                   *pOperMeId, ret);

    return ret;
}

CmsRet rutGpon_getUniMeIdFromMcastSubsMeId
    (const UINT32 subsMeId, const UINT32 meType,
     UINT32 *pUniOid, UINT32 *pUniMeId)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    if (meType == OMCI_MCAST_METYPE_MAC_BPCD)
    {
        MacBridgePortConfigDataObject *port = NULL;
        while ((!found) &&
               (_cmsObj_getNextFlags(MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &port) == CMSRET_SUCCESS))
        {
            found = (port->managedEntityId == subsMeId);
            if (found == TRUE)
            {
                if (port->tpType == OMCI_BP_TP_PPTP_ETH)
                    *pUniOid = MDMOID_PPTP_ETHERNET_UNI;
                else if (port->tpType == OMCI_BP_TP_PPTP_MOCA)
                    *pUniOid = MDMOID_PPTP_MOCA_UNI;
                else if (port->tpType == OMCI_BP_TP_VEIP)
                    *pUniOid = MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT;
                *pUniMeId = port->tpPointer;
                ret = CMSRET_SUCCESS;
            }
            _cmsObj_free((void **) &port);
        }
    }
    else if (meType == OMCI_MCAST_METYPE_MAPPER_SVC)
    {
        MapperServiceProfileObject *mapper = NULL;
        while ((!found) &&
               (_cmsObj_getNextFlags(MDMOID_MAPPER_SERVICE_PROFILE,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &mapper) == CMSRET_SUCCESS))
        {
            found = (mapper->managedEntityId == subsMeId);
            if (found == TRUE)
            {
                if (mapper->tpType == OMCI_MS_TP_PPTP_ETH)
                    *pUniOid = MDMOID_PPTP_ETHERNET_UNI;
                else if (mapper->tpType == OMCI_MS_TP_PPTP_MOCA)
                    *pUniOid = MDMOID_PPTP_MOCA_UNI;
                else if (mapper->tpType == OMCI_MS_TP_VEIP)
                    *pUniOid = MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT;
                *pUniMeId = mapper->tpPointer;
                ret = CMSRET_SUCCESS;
            }
            _cmsObj_free((void **) &mapper);
        }
    }

    // check if the UNI port exists.
    if (found)
    {
        INIT_INSTANCE_ID_STACK(&iidStack);
        ret = CMSRET_OBJECT_NOT_FOUND;
        found = FALSE;

        if (*pUniOid  ==  MDMOID_PPTP_ETHERNET_UNI)
        {
            PptpEthernetUniObject *pptpEthernet = NULL;

            while ((!found) && _cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &pptpEthernet) == CMSRET_SUCCESS)
            {
                found = (pptpEthernet->managedEntityId == *pUniMeId);
                if (found)
                    ret = CMSRET_SUCCESS;
                _cmsObj_free((void **) &pptpEthernet);
            }
        }
        else if (*pUniOid  ==  MDMOID_PPTP_MOCA_UNI)
        {
            PptpMocaUniObject *pptpMoca = NULL;

            while ((!found) && _cmsObj_getNextFlags(MDMOID_PPTP_MOCA_UNI,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &pptpMoca) == CMSRET_SUCCESS)
            {
                found = (pptpMoca->managedEntityId == *pUniMeId);
                if (found)
                    ret = CMSRET_SUCCESS;
                _cmsObj_free((void **) &pptpMoca);
            }
        }
        else if (*pUniOid  ==  MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT)
        {
            VirtualEthernetInterfacePointObject *pptpVeip = NULL;

            while ((!found) && _cmsObj_getNextFlags(MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &pptpVeip) == CMSRET_SUCCESS)
            {
                found = (pptpVeip->managedEntityId == *pUniMeId);
                if (found)
                    ret = CMSRET_SUCCESS;
                _cmsObj_free((void **) &pptpVeip);
            }
        }
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getUniMeIdFromMcastSubsMeId, subsMeId=%d, meType=%d, uniOid=%d, uniMeId=%d, ret=%d\n",
                   subsMeId, meType, *pUniOid, *pUniMeId, ret);

    return ret;
}

UBOOL8 rutGpon_isInterfaceAniMulticast(const char *interfaceName)
{
    char *pc = NULL;
    UBOOL8 ret = FALSE, found = FALSE;
    UINT32 gemPortIndex = 0, phyId = 0;
    GemPortNetworkCtpObject *ctp = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    pc = strchr(interfaceName, '.');
    if (pc == NULL)
        return ret;

    // parse gemportIndex from interface name
    gemPortIndex = strtoul(pc+1, (char**)NULL, 10);

    while (!found &&
           _cmsObj_getNextFlags(MDMOID_GEM_PORT_NETWORK_CTP,
                          &iidStack,
                          OGF_NO_VALUE_UPDATE,
                          (void **) &ctp) == CMSRET_SUCCESS)
    {
        if (rutGpon_getIndexFromGemPortNetworkCtp(ctp, &phyId) == CMSRET_SUCCESS)
        {
            if (gemPortIndex == phyId)
            {
                ret = rutGpon_isGemPortNetworkCtpMulticast(ctp);
                found = TRUE;
            }
        }
        _cmsObj_free((void **) &ctp);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_isInterfaceAniMulticast, interfaceName=%s, ret=%d\n",
                   interfaceName, ret);

    return ret;
}

CmsRet rutGpon_getBridgeNameFromBridgeMeId
    (const UINT32 bridgeMeId, char *bridgeName)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BCM_MacBridgeServiceProfileObject *bcmBridge = NULL;
    BCM_MapperServiceProfileObject *bcmMapper = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    if (bridgeName == NULL)
    {
        cmsLog_error("bridgeMeId=%d, NULL bridgeName pointer", bridgeMeId);
        goto out2;
    }

    // search BCM_MacBridgeServiceProfileObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_BC_MAC_BRIDGE_SERVICE_PROFILE,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &bcmBridge) == CMSRET_SUCCESS))
    {
        found = (bcmBridge->managedEntityId == bridgeMeId);
        // if BCM_MacBridgeServiceProfileObject is found then
        // get name of the bridge
        if (found == TRUE && bcmBridge->bridgeName != NULL)
        {
            strcpy(bridgeName, bcmBridge->bridgeName);
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &bcmBridge);
    }

    if (ret == CMSRET_SUCCESS)
        goto out;

    found = FALSE;
    INIT_INSTANCE_ID_STACK(&iidStack);

    // search BCM_MapperServiceProfileObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_BC_MAPPER_SERVICE_PROFILE,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &bcmMapper) == CMSRET_SUCCESS))
    {
        found = (bcmMapper->managedEntityId == bridgeMeId);
        // if BCM_MapperServiceProfileObject is found then
        // get bridge name of mapper
        if (found == TRUE && bcmMapper->bridgeName != NULL)
        {
            strcpy(bridgeName, bcmMapper->bridgeName);
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &bcmMapper);
    }

out:
    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getBridgeNameFromBridgeMeId, bridgeMeId=%d, bridgeName=%s, ret=%d\n",
                   bridgeMeId, bridgeName, ret);
out2:
    return ret;
}

CmsRet rutGpon_getPrioFromPriorityQueue
    (PriorityQueueGObject *pQInfoP,
     const OmciFLowDirection dir,
     UINT16 *pPrio)
{
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
    UINT16 prio = GetRelatedPortPrio(pQInfoP->relatedPort);

    if (((dir == OMCI_FLOW_DOWNSTREAM) && (prio < GPON_PHY_DS_PQ_MAX)) ||
      ((dir == OMCI_FLOW_UPSTREAM) && (prio < GPON_PHY_US_PQ_MAX)))
    {
        *pPrio = prio;
        ret = CMSRET_SUCCESS;
    }
    else
    {
        cmsLog_error("Invalid Q prio %d from PQ 0x%04x, related port 0x%08x",
          prio, pQInfoP->managedEntityId, pQInfoP->relatedPort);
        *pPrio = 0;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getPrioFromPriorityQueue, dir=%d, prio=%d, ret=%d\n",
                   dir, *pPrio, ret);

    return ret;
}

/* Look up both PQ ID and PQ informaiton for better efficiency. */
CmsRet rutGpon_getQidFromPriorityQueueV2
    (const UINT16 prioQMeId,
     UINT16 *pQid,
     PriorityQueueGObject *pQInfoP)
{
    UBOOL8 foundPrioQ = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    PriorityQueueGObject *prioQ = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
    UINT32 curPort = 0;
    UINT32 lastPort = 0;
    UINT16 firstQMeIdCurPort = 0xFFFF;
    UBOOL8 finishCurPort = FALSE;
    UINT16 tmpQid = 0;
    UINT16 foundQMeId = 0;
#ifdef SUPPORT_QOS_CAR
    OntGObject *ontGObj = NULL;
    UINT32 tmOption;

    if (_cmsObj_get(MDMOID_ONT_G,
                   &iidStack,
                   OGF_NO_VALUE_UPDATE,
                   (void **)&ontGObj) == CMSRET_SUCCESS)
    {
        tmOption = ontGObj->trafficManagementOption;
        _cmsObj_free((void**)&ontGObj);
        if (tmOption == OMCI_TRAFFIC_MANAGEMENT_RATE)
        {
            *pQid = 0;
            memset(pQInfoP, 0x0, sizeof(PriorityQueueGObject));
            ret = CMSRET_SUCCESS;
            return ret;
        }
    }
#endif /* SUPPORT_QOS_CAR */

    *pQid = OMCI_QID_NOT_EXIST;
    memset(pQInfoP, 0x0, sizeof(PriorityQueueGObject));
    // search PriorityQueueGObject
    while (!foundPrioQ && !finishCurPort && //havent finish cur port
           (_cmsObj_getNext(MDMOID_PRIORITY_QUEUE_G,
                           &iidStack,
                           (void **) &prioQ) == CMSRET_SUCCESS))
    {
        //find out the first Q Meid for curr port
        curPort = prioQ->relatedPort;
        if ((curPort & 0xFFFF0000) == (lastPort & 0xFFFF0000))
        {
            finishCurPort = FALSE;
            if (prioQ->managedEntityId < firstQMeIdCurPort)
            {
                firstQMeIdCurPort = prioQ->managedEntityId;
            }
        }
        else // iterate on a new port, means last port is finished
        {
            if (lastPort != 0 && foundPrioQ) //last port is initialized to 0
            {
                finishCurPort = TRUE;
                tmpQid = foundQMeId - firstQMeIdCurPort;
            }
            lastPort = curPort;
            firstQMeIdCurPort = prioQ->managedEntityId;
        }

        if (foundPrioQ != TRUE)
        {
            foundPrioQ = (prioQ->managedEntityId == prioQMeId);

            if (foundPrioQ)
            {
                foundQMeId = prioQ->managedEntityId;
            }
        }

        if (foundPrioQ == TRUE)
        {
            memcpy(pQInfoP, prioQ, sizeof(PriorityQueueGObject));
        }

        _cmsObj_free((void **) &prioQ);
    }

    if (foundPrioQ == TRUE)
    {
        if (!finishCurPort)
        {
            tmpQid = foundQMeId - firstQMeIdCurPort;
        }
        *pQid = tmpQid;
        ret = CMSRET_SUCCESS;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getQidFromPriorityQueue, priOMeId=%d, qid=%d, ret=%d\n",
                   prioQMeId, *pQid, ret);

    return ret;
}

CmsRet rutGpon_getPrioFromTrafficScheduler
    (const GemPortNetworkCtpObject *ctp,
     const OmciFLowDirection dir,
     UINT16 *pPrio)
{
    UBOOL8 foundPrioQ = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    PriorityQueueGObject *prioQ = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search PriorityQueueGObject
    while ((!foundPrioQ) &&
           (_cmsObj_getNextFlags(MDMOID_PRIORITY_QUEUE_G,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &prioQ) == CMSRET_SUCCESS))
    {
        if (dir == OMCI_FLOW_DOWNSTREAM)
            foundPrioQ = (prioQ->managedEntityId == ctp->downstreamPriorityQueuePointer);
        else
            foundPrioQ = (prioQ->managedEntityId == ctp->upstreamTrafficManagementPointer);

        // if PriorityQueueGObject is found
        if (foundPrioQ == TRUE)
        {
            UBOOL8 foundTrSch = FALSE;
            InstanceIdStack iidTrffSch = EMPTY_INSTANCE_ID_STACK;
            TrafficSchedulerGObject *trffSch = NULL;

            // search TrafficSchedulerGObject
            while ((!foundTrSch) &&
                   (_cmsObj_getNextFlags(MDMOID_TRAFFIC_SCHEDULER_G,
                                   &iidTrffSch,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &trffSch) == CMSRET_SUCCESS))
            {
                foundTrSch = (prioQ->trafficSchedulerGPointer == trffSch->managedEntityId);
                // Does TrafficSchedulerGObject point to the same
                // TCont that GemPortNetworkCtpObject points to ?
                if (dir == OMCI_FLOW_UPSTREAM)
                    foundTrSch = (foundTrSch &&  (ctp->TContPointer == trffSch->TContPointer));

                // if TrafficSchedulerGObject is found then get priority from priority field
                // if policy of TrafficSchedulerGObject is OMCI_SCHEDULE_POLICY_HOL
                if (foundTrSch == TRUE && trffSch->policy == OMCI_SCHEDULE_POLICY_HOL)
                {
                    *pPrio = trffSch->priority;
                    ret = CMSRET_SUCCESS;
                }

                _cmsObj_free((void **) &trffSch);
            }
        }

        _cmsObj_free((void **) &prioQ);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getPrioFromTrafficScheduler, ctpMeId=%d, dir=%d, weight=%d, ret=%d\n",
                   ctp->managedEntityId, dir, *pPrio, ret);

    return ret;
}

CmsRet rutGpon_getWeightFromPriorityQueue
    (PriorityQueueGObject *pQInfo,
     const OmciFLowDirection dir,
     UINT16 *pWeight)
{
    CmsRet ret = CMSRET_SUCCESS;

    *pWeight = pQInfo->weight;
    if (*pWeight == 0)
    {
        /* weight can not be 0. */
        *pWeight = 1;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getWeightFromPriorityQueue, dir=%d, weight=%d, ret=%d\n",
                   dir, *pWeight, ret);

    return ret;
}

CmsRet rutGpon_getWeightFromTrafficScheduler
    (const GemPortNetworkCtpObject *ctp,
     const OmciFLowDirection dir,
     UINT16 *pWeight)
{
    UBOOL8 foundPrioQ = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    PriorityQueueGObject *prioQ = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search PriorityQueueGObject
    while ((!foundPrioQ) &&
           (_cmsObj_getNextFlags(MDMOID_PRIORITY_QUEUE_G,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &prioQ) == CMSRET_SUCCESS))
    {
        if (dir == OMCI_FLOW_DOWNSTREAM)
            foundPrioQ = (prioQ->managedEntityId == ctp->downstreamPriorityQueuePointer);
        else
            foundPrioQ = (prioQ->managedEntityId == ctp->upstreamTrafficManagementPointer);

        // if PriorityQueueGObject is found
        if (foundPrioQ == TRUE)
        {
            UBOOL8 foundTrSch = FALSE;
            InstanceIdStack iidTrffSch = EMPTY_INSTANCE_ID_STACK;
            TrafficSchedulerGObject *trffSch = NULL;

            // search TrafficSchedulerGObject
            while ((!foundTrSch) &&
                   (_cmsObj_getNextFlags(MDMOID_TRAFFIC_SCHEDULER_G,
                                   &iidTrffSch,
                                   OGF_NO_VALUE_UPDATE,
                                   (void **) &trffSch) == CMSRET_SUCCESS))
            {
                foundTrSch = (prioQ->trafficSchedulerGPointer == trffSch->managedEntityId);
                // Does TrafficSchedulerGObject point to the same
                // TCont that GemPortNetworkCtpObject points to ?
                if (dir == OMCI_FLOW_UPSTREAM)
                    foundTrSch = (foundTrSch &&  (ctp->TContPointer == trffSch->TContPointer));

                // if TrafficSchedulerGObject is found then get weight from priority field
                // if policy of TrafficSchedulerGObject is OMCI_SCHEDULE_POLICY_WRR
                if (foundTrSch == TRUE && trffSch->policy == OMCI_SCHEDULE_POLICY_WRR)
                {
                    *pWeight = (UINT16)(trffSch->priority * 127 / 255);
                    ret = CMSRET_SUCCESS;
                }

                _cmsObj_free((void **) &trffSch);
            }
        }

        _cmsObj_free((void **) &prioQ);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getWeightFromTrafficScheduler, ctpMeId=%d, dir=%d, weight=%d, ret=%d\n",
                   ctp->managedEntityId, dir, *pWeight, ret);

    return ret;
}

CmsRet rutGpon_getTContPolicyFromTContMeId
    (const  UINT32 tContMeId,
     OmciSchedulePolicy *pPolicy)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    TContObject *tCont = NULL;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

    // search TContObject
    while ((!found) &&
           (_cmsObj_getNextFlags(MDMOID_T_CONT,
                           &iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **) &tCont) == CMSRET_SUCCESS))
    {
        found = (tCont->managedEntityId == tContMeId);

        // if TContObject is found then return policy of TContObject
        if (found == TRUE)
        {
            *pPolicy = (OmciSchedulePolicy) tCont->policy;
            ret = CMSRET_SUCCESS;
        }
        _cmsObj_free((void **) &tCont);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                   "===> rutGpon_getTContPolicyFromTContMeId, tContMeId=%d, policy=%d, ret=%d\n",
                   tContMeId, *pPolicy, ret);

    return ret;
}

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1

void rutGpon_deleteAllVeipServices(void)
{
    UBOOL8 inPath = FALSE;
    UINT32 reachableOid = 0, reachableMeId = 0;
    GemPortNetworkCtpObject *ctp = NULL;
    VirtualEthernetInterfacePointObject *veip = NULL;
    PptpEthernetUniObject    *eth = NULL;
    OmciSystemObject *omci = NULL;
    InstanceIdStack iidCtp = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidVeip = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidEth = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidOmci = EMPTY_INSTANCE_ID_STACK;
    OmciMapFilterModelType type = OMCI_MF_MODEL_NONE;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
    UBOOL8 found = FALSE;

    while (_cmsObj_getNextFlags(MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                          &iidVeip,
                          OGF_NO_VALUE_UPDATE,
                          (void **) &veip) == CMSRET_SUCCESS)
    {
        INIT_INSTANCE_ID_STACK(&iidCtp);
        while (_cmsObj_getNextFlags(MDMOID_GEM_PORT_NETWORK_CTP,
                              &iidCtp,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ctp) == CMSRET_SUCCESS)
        {
            type = OMCI_MF_MODEL_NONE;
            inPath = FALSE;
            reachableOid = MDMOID_GEM_PORT_NETWORK_CTP;
            reachableMeId = ctp->managedEntityId;
            // mark bridge is not found before searching path
            glbFoundBridge = FALSE;

            // searching path from VirtualEthernetInterfacePointObject to GemPortNetworkCtpObject
            ret = VirtualEthernetInterfacePointObject_searchModelPath
                      (veip, OMCI_FLOW_UPSTREAM, &type, MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,
                       veip->managedEntityId, &reachableOid, &reachableMeId, 0, &inPath);

            omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                           "===> rutGpon_deleteAllVeipServices, reachableOid=%d, reachableMeId=%d, uniMeId=%d, aniMeId=%d, type=%d, checkOid=%d, checkMeId=%d, inPath=%d, depth=%d, ret=%d\n\n",
                           reachableOid, reachableMeId, veip->managedEntityId, ctp->managedEntityId, type, MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT, veip->managedEntityId, inPath, 0, ret);

            if (ret == CMSRET_SUCCESS)
            {
                /* For GponRg Full Omci only.  Need to delete this veip (First parameter set to FALSE) */
                rutRgFull_configVeip(FALSE, MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT, veip->managedEntityId, type, ctp);
            }
            _cmsObj_free((void **) &ctp);
        }
        _cmsObj_free((void **) &veip);
    }

    //check pptp eth uni as veip
    if (_cmsObj_get(MDMOID_OMCI_SYSTEM, &iidOmci, 0, (void *) &omci) == CMSRET_SUCCESS)
    {
        if (omci->veipPptpUni0)
        {
            while (!found && _cmsObj_getNextFlags(MDMOID_PPTP_ETHERNET_UNI,
                                    &iidEth,
                                    OGF_NO_VALUE_UPDATE,
                                    (void **) &eth) == CMSRET_SUCCESS)
            {
                found = (eth->managedEntityId == omci->veipManagedEntityId1);
                if (found)
                {

                INIT_INSTANCE_ID_STACK(&iidCtp);
                while (_cmsObj_getNextFlags(MDMOID_GEM_PORT_NETWORK_CTP,
                                      &iidCtp,
                                      OGF_NO_VALUE_UPDATE,
                                      (void **) &ctp) == CMSRET_SUCCESS)
                {
                    type = OMCI_MF_MODEL_NONE;
                    inPath = FALSE;
                    reachableOid = MDMOID_GEM_PORT_NETWORK_CTP;
                    reachableMeId = ctp->managedEntityId;
                    // mark bridge is not found before searching path
                    glbFoundBridge = FALSE;

                    // searching path from VirtualEthernetInterfacePointObject to GemPortNetworkCtpObject
                    ret = PptpEthernetUniObject_searchModelPath
                              (eth, OMCI_FLOW_UPSTREAM, &type, MDMOID_PPTP_ETHERNET_UNI,
                               eth->managedEntityId, &reachableOid, &reachableMeId, 0, &inPath);

                    omciDebugPrint(OMCI_DEBUG_MODULE_MODEL,
                                   "===> rutGpon_deleteAllVeipServices, reachableOid=%d, reachableMeId=%d, uniMeId=%d, aniMeId=%d, type=%d, checkOid=%d, checkMeId=%d, inPath=%d, depth=%d, ret=%d\n\n",
                                   reachableOid, reachableMeId, eth->managedEntityId, ctp->managedEntityId, type, MDMOID_PPTP_ETHERNET_UNI, eth->managedEntityId, inPath, 0, ret);

                    if (ret == CMSRET_SUCCESS)
                    {
                        /* For GponRg Full Omci only.  Need to delete this veip (First parameter set to FALSE) */
                        rutRgFull_configVeip(FALSE, MDMOID_PPTP_ETHERNET_UNI, eth->managedEntityId, type, ctp);
                    }
                    _cmsObj_free((void **) &ctp);
                }

                }
                _cmsObj_free((void **) &eth);
            }
        }
        _cmsObj_free((void **) &omci);
    }
}

#endif /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

#endif /* DMP_X_ITU_ORG_GPON_1 */
