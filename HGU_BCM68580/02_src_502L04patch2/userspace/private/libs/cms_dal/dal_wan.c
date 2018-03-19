/*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
:>
*/

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"
#include "dal.h"
#include "dal_network.h"
#include "adslctlapi.h" /* for ANNEX_A_LINE_PAIR_INNER/OUTER */

/* use this function from rut_wan.h */
extern UBOOL8 rut_isUpnpEnabled(void);

extern CmsRet rutWan_fillPppIfName(UBOOL8 isPPPoE, char *pppName);
extern CmsRet rutWan_getErrorSamples(void* obj,long *len,int lineId);
extern CmsRet rutWan_getVceMacAddress(void* vceMacAddress, int lineId);
extern UBOOL8 rutPMap_isBridgedInterfaceGroup(UINT32 BridgeKey);
extern UBOOL8 rutWl2_isWanLayer2LinkUp(MdmObjectId wanConnId, const InstanceIdStack *iidStack);
extern UBOOL8 rutWl2_isVlanMuxEnabled(MdmObjectId wanConnId, const InstanceIdStack *iidStack);
/*rut_util.h*/
extern UBOOL8 rut_isWanTypeEpon(void);


#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
extern CmsRet rutWan_addDelegatedAddrEntry(const char *srvName, const char *ipv6str, const char *lanIntf, const char * mode);
#endif
#ifdef DMP_ETHERNETWAN_1
extern CmsRet rutWl2_getWanEthObject(InstanceIdStack *iidStack, WanEthIntfObject **wanEthIntfObj);
#endif
#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
CmsRet rutWl2_getWanMocaObject(InstanceIdStack *iidStack, WanMocaIntfObject **wanMocaIntfObj);
#endif
#ifdef DMP_X_BROADCOM_COM_L2TPAC_1
CmsRet rutWl2_getWanL2tpAcObject(InstanceIdStack *iidStack, L2tpAcIntfConfigObject **wanL2tpAcIntfObj);
#endif
#ifdef DMP_X_BROADCOM_COM_PPTPAC_1
CmsRet rutWl2_getWanPptpAcObject(InstanceIdStack *iidStack, PptpAcIntfConfigObject **wanPptpAcIntfObj);
#endif

/* local functions */
static CmsRet addWanIpConnection(const WEB_NTWK_VAR *webVar, InstanceIdStack *iidStack);
static CmsRet deleteIpOrPppConnection(char *serviceIfcName, InstanceIdStack *iidStack);
static UBOOL8 getIpConn(UINT32 portId, const char *destAddr, SINT32 connectionId, InstanceIdStack *iidStack, WanIpConnObject **ipConn);
static UBOOL8 getPppConn(UINT32 portId, const char *destAddr, SINT32 connectionId, InstanceIdStack *iidStack, WanPppConnObject **pppConn);
static CmsRet addWanPppConnection(const WEB_NTWK_VAR *webVar, InstanceIdStack *iidStack);
static CmsRet dalWan_getIpOrPppObjectByIfname(const char *ifName, InstanceIdStack *iidStack, void **obj, UBOOL8 *isPpp);

UBOOL8 rutWl2_isIPoA(const InstanceIdStack *iidStack);
UBOOL8 rutWl2_isPPPoA(const InstanceIdStack *iidStack);

UBOOL8 dalWan_isIPoA(const InstanceIdStack *iidStack)
{
   return rutWl2_isIPoA(iidStack);
}

UBOOL8 dalWan_isPPPoA(const InstanceIdStack *iidStack)
{
   return rutWl2_isPPPoA(iidStack);
}


UBOOL8 dalWan_isVlanMuxEnabled(MdmObjectId wanConnOid, const InstanceIdStack *iidStack)
{
   return rutWl2_isVlanMuxEnabled(wanConnOid, iidStack);
}


#ifdef DMP_ADSLWAN_1

UBOOL8 dalWan_getDslLinkCfg(const WEB_NTWK_VAR *webVar, InstanceIdStack *iidStack,
                            WanDslLinkCfgObject **dslLinkCfg)
{
   char destAddr[BUFLEN_128];   
   
   if (cmsUtl_atmVpiVciNumToStr(webVar->atmVpi, webVar->atmVci, destAddr) != CMSRET_SUCCESS)
   {
      cmsLog_debug("bad vpi/vci (%d/%d).", webVar->atmVpi, webVar->atmVci);
      return FALSE;
   }

   return getDslLinkCfg(webVar->portId, destAddr, iidStack, dslLinkCfg);

}  /* End of dalWan_getDslLinkCfg() */ 

#endif

#ifdef DMP_VDSL2WAN_1
UBOOL8 dalWan_getErrorSamples(void *obj,long *len, int lineId) {
  if (rutWan_getErrorSamples(obj,len,lineId))
    return FALSE;
  return TRUE;
}

UBOOL8 dalWan_getVceMacAddress(void *vceMacAddress, int lineId) {
  if (rutWan_getVceMacAddress(vceMacAddress,lineId))
    return FALSE;
  return TRUE;
}
#endif

UBOOL8 dalWan_getIpConn(const WEB_NTWK_VAR *webVar, InstanceIdStack *iidStack,
                        WanIpConnObject **ipConn)
{
   char destAddr[BUFLEN_128];   
   
   if (cmsUtl_atmVpiVciNumToStr(webVar->atmVpi, webVar->atmVci, destAddr) != CMSRET_SUCCESS)
   {
      cmsLog_debug("bad vpi/vci (%d/%d).", webVar->atmVpi, webVar->atmVci);
      return FALSE;
   }

   return getIpConn(webVar->portId, destAddr, webVar->serviceId, iidStack, ipConn);

}  /* End of dalWan_getIpConn() */ 

UBOOL8 dalWan_getPppConn(const WEB_NTWK_VAR *webVar, InstanceIdStack *iidStack,
                         WanPppConnObject **pppConn)
{
   char destAddr[BUFLEN_128];   
   
   if (cmsUtl_atmVpiVciNumToStr(webVar->atmVpi, webVar->atmVci, destAddr) != CMSRET_SUCCESS)
   {
      cmsLog_debug("bad vpi/vci (%d/%d).", webVar->atmVpi, webVar->atmVci);
      return FALSE;
   }

   return getPppConn(webVar->portId, destAddr, webVar->serviceId, iidStack, pppConn);

}  /* End of dalWan_getPppConn() */ 

UBOOL8 delWan_getAnotherIpConn(SINT32 connIdExcluded, InstanceIdStack *parentIidStack,
                               InstanceIdStack *iidStack, WanIpConnObject **ipConn)
{
   WanIpConnObject *obj;
   UBOOL8 found=FALSE;
   CmsRet ret;

   /*
    * search the sub-tree rooted at the parentIidStack
    * for an IpConn object with the given connectionId.
    */
   INIT_INSTANCE_ID_STACK(iidStack);
   while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, parentIidStack, iidStack, (void **)&obj)) == CMSRET_SUCCESS)
   {
      if (obj->X_BROADCOM_COM_ConnectionId != connIdExcluded)
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &obj);
      }
   }

   if (found)
   {
      if (ipConn != NULL)
      {
         *ipConn = obj;
      }
      else
      {
         cmsObj_free((void **) &obj);
      }
   }

   return found;

}  /* End of delWan_getAnotherIpConn() */

UBOOL8 delWan_getAnotherPppConn(SINT32 connIdExcluded, InstanceIdStack *parentIidStack,
                                InstanceIdStack *iidStack, WanPppConnObject **pppConn)
{
   WanPppConnObject *obj;
   UBOOL8 found=FALSE;
   CmsRet ret;

   /*
    * search the sub-tree rooted at the parentIidStack
    * for a PppConn object with the given connectionId.
    */
   INIT_INSTANCE_ID_STACK(iidStack);
   while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, parentIidStack, iidStack, (void **)&obj)) == CMSRET_SUCCESS)
   {
      if (obj->X_BROADCOM_COM_ConnectionId != connIdExcluded)
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &obj);
      }
   }

   if (found)
   {
      if (pppConn != NULL)
      {
         *pppConn = obj;
      }
      else
      {
         cmsObj_free((void **) &obj);
      }
   }

   return found;

}  /* End of delWan_getAnotherPppConn() */


UBOOL8 dalWan_isBridgeInInterfaceGroup(const char *brName __attribute((unused)))
{
   UBOOL8 isBridgedIntfGroup = FALSE;

#ifdef DMP_BRIDGING_1  /* aka SUPPORT_PMAP */
   UINT32 bridgeKey=0;
   
   if (dalPMap_getBridgeKey(brName, &bridgeKey) != CMSRET_SUCCESS)
   {
      cmsLog_error("cannot find bridge key for bridge %s", brName);
      return isBridgedIntfGroup;
   }
   
   isBridgedIntfGroup = rutPMap_isBridgedInterfaceGroup(bridgeKey);

   cmsLog_debug("Bridged Interface group %s  = %d", brName, isBridgedIntfGroup);
#endif


   return isBridgedIntfGroup;
}


void dalWan_allBridgePrtcl(const char *brName, char *allBridge)
{
   if (allBridge != NULL)
   {
      if (qdmIpIntf_isAllBridgeWanServiceLocked() ||
          dalWan_isBridgeInInterfaceGroup(brName))
      {
         strcpy(allBridge, "Bridge");
      }
      else
      {
         strcpy(allBridge, "MER");     /* TODO: Need to return WEB_PROTOCOL ?  */
      }
   }
}


/* same idea as BcmDb_getNoBridgeNtwkPrtcl in dbapi.cpp */
CmsWanConnectionType dalWan_getNoBridgeNtwkPrtcl(void)
{
   CmsWanConnectionType proto = CMS_WAN_TYPE_UNDEFINED;
   UBOOL8 isBridgeFound=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *pppConn=NULL;
   WanIpConnObject *ipConn=NULL;
   CmsRet ret;
   
   if ((ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConn)) == CMSRET_SUCCESS)
   {
      InstanceIdStack tmpIidStack = iidStack;
      WanDslLinkCfgObject *dslLinkCfg = NULL;
      CmsRet ret2;
      
      if ((ret2 = cmsObj_getAncestor(MDMOID_WAN_DSL_LINK_CFG, MDMOID_WAN_PPP_CONN, &tmpIidStack, (void **)&dslLinkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not find ancestor dslLinkCfg obj, ret=%d", ret2);
      }
      else
      {
         // rcl_wanPppConnObject checks for dslLinkCfg->linktype, but
         // what if there are multiple services on a PVC?  PPPoA not allowed?
         // In addWanPPPConnectionObject, wanPPPConn->connectionType is also set 
         // to IP_ROUTED, can we check that instead?
         proto = (!cmsUtl_strcmp(dslLinkCfg->linkType, MDMVS_PPPOA)) ? CMS_WAN_TYPE_PPPOA : CMS_WAN_TYPE_PPPOE;
         cmsObj_free((void **) &dslLinkCfg);
      }
      
      cmsObj_free((void **) &pppConn);
      
      /* we found a non-bridge wan proto, so we can return now */
      return proto;
   }
   
   
   /*
    * No PPP connection.  Look through IP connections find a non-bridge one.
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConn)) == CMSRET_SUCCESS)
   {
      InstanceIdStack tmpIidStack = iidStack;
      WanDslLinkCfgObject *dslLinkCfg = NULL;
      CmsRet ret2;
      
      if ((ret2 = cmsObj_getAncestor(MDMOID_WAN_DSL_LINK_CFG, MDMOID_WAN_PPP_CONN, &tmpIidStack, (void **)&dslLinkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not find ancestor dslLinkCfg obj, ret=%d", ret2);
      }
      else
      {
         if (!cmsUtl_strcmp(dslLinkCfg->linkType, MDMVS_IPOA))
         {
            proto = CMS_WAN_TYPE_IPOA;
         }
         else if (!cmsUtl_strcmp(ipConn->connectionType, MDMVS_IP_BRIDGED))
         {
            proto = CMS_WAN_TYPE_BRIDGE;
            isBridgeFound = TRUE;
         }
         else
         {
            proto = CMS_WAN_TYPE_DYNAMIC_IPOE;
         }
         cmsObj_free((void **) &dslLinkCfg);
      }
      
      cmsObj_free((void **) &pppConn);
      
      /* we found a non-bridge wan proto, so we can return now */
      if (proto != CMS_WAN_TYPE_BRIDGE)
      {
         return proto;
      }
   }

   /*
    * If we get here, then we have either no WAN connections on the system
    * or only bridge connections.  If we only have bridge connections, return
    * CMS_WAN_TYPE_BRIDGE (don't know why we need to do that, but the original
    * cfm code did this).
    */   
   return (isBridgeFound ? CMS_WAN_TYPE_BRIDGE : CMS_WAN_TYPE_UNDEFINED);
   
}


#ifdef notUsed
/* This function finds out if there is any firewall enable WAN service.
 * Currently, it is not used
 */
UBOOL8 dalWan_isAnyFirewallEnabled(void)
{
   UBOOL8 enabled = FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *pppConn=NULL;
   WanIpConnObject *ipConn=NULL;
   CmsRet ret;
   
   while ((enabled == FALSE) &&
          (ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConn)) == CMSRET_SUCCESS)
   {
      enabled = pppConn->X_BROADCOM_COM_FirewallEnabled;
      cmsObj_free((void **) &pppConn);
   }
   
   /* already found an interface with firewall enabled, can return true now */
   if (enabled)
   {
      return TRUE;
   }
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((enabled == FALSE) &&
          (ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConn)) == CMSRET_SUCCESS)
   {
      enabled = ipConn->X_BROADCOM_COM_FirewallEnabled;
      cmsObj_free((void **) &ipConn);
   }
      
   return enabled;
}
#endif /* notUsed */


#ifdef DMP_BASELINE_1

UBOOL8 dalWan_isInterfaceVlanEnabled_igd(const char *ifcName)
{
   UBOOL8 enable = FALSE;
   UBOOL8 found  = FALSE;
   InstanceIdStack iidStack;
   WanPppConnObject *pppConn = NULL;
   WanIpConnObject *ipConn = NULL;

   cmsLog_debug("Enter, ifcName=%s", ifcName);
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((found == FALSE) &&
          (cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConn) == CMSRET_SUCCESS))
   {
      if (cmsUtl_strcmp(ifcName, pppConn->X_BROADCOM_COM_IfName) == 0)
      {
         enable = ((pppConn->X_BROADCOM_COM_VlanMux8021p != VLANMUX_DISABLE) ||
                   (pppConn->X_BROADCOM_COM_VlanMuxID    != VLANMUX_DISABLE));
         found = TRUE;
      }
      cmsObj_free((void **) &pppConn);
   }
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((found == FALSE) &&
          (cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConn) == CMSRET_SUCCESS))
   {
      if (cmsUtl_strcmp(ifcName, ipConn->X_BROADCOM_COM_IfName) == 0)
      {
         enable = ((ipConn->X_BROADCOM_COM_VlanMux8021p != VLANMUX_DISABLE) ||
                   (ipConn->X_BROADCOM_COM_VlanMuxID    != VLANMUX_DISABLE));
         found = TRUE;
      }
      cmsObj_free((void **) &ipConn);
   }
      
   return enable;
   
}  /* End of dalWan_isInterfaceVlanEnable() */

#endif  /* DMP_BASELINE_1 */


UINT32 dalWan_getNumberOfUsedPvc(void)
{
   WanDslLinkCfgObject *dslLinkCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UINT32 num = 0;

   while ( cmsObj_getNext(MDMOID_WAN_DSL_LINK_CFG, &iidStack, (void **) &dslLinkCfg) == CMSRET_SUCCESS )
   {
      num++;
      cmsObj_free((void **) &dslLinkCfg);
   }

   return num;
}



CmsRet dalWan_addService_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
   char wanL2IfName[BUFLEN_32];
   char *p;
#ifndef CMS_LOG0
   MdmObjectId wanConnOid __attribute__((unused)) = MDMOID_WAN_IP_CONN;
#endif
   
#ifdef DMP_ADSLWAN_1
   UBOOL8 matched;
   /* Initialization split into two lines for the case where matched is not used.
      Normally, Werror would complain about an unused variable, but splitting
      initialization will not trigger that warning. */
   matched = FALSE;
#endif
   
   cmsLog_debug("ifName=%s, protocol=%d, wanConnOid=%d", webVar->wanL2IfName, webVar->ntwkPrtcl, wanConnOid);
   strncpy(wanL2IfName, webVar->wanL2IfName, sizeof(wanL2IfName)-1);
   
   if ((p = strchr(wanL2IfName, '/')))
   {
      /* now wanL2IfName is "ptm0", "atm0" etc. */
      *p = '\0';
   }
   else if (cmsUtl_strstr(wanL2IfName, ETH_IFC_STR) || 
      cmsUtl_strstr(wanL2IfName, MOCA_IFC_STR) ||
      cmsUtl_strstr(wanL2IfName, GPON_IFC_STR) ||
      cmsUtl_strstr(wanL2IfName, EPON_IFC_STR) ||
      cmsUtl_strstr(wanL2IfName, WLAN_IFC_STR))
   {
      /* do nothing for now */
   }
   else
   {
      cmsLog_error("wanL2IfName %s - wrong format", wanL2IfName);
      return ret;
   }

#ifdef DMP_BASELINE_1
   /* this is a very TR98 specific way of doing it.  Hopefully when we
    * are doing a Pure TR181 impl. we can check if an interface is a wan
    * interface in a generic way--not per interface type.
    */
#ifdef DMP_ETHERNETWAN_1  /* aka SUPPORT_ETHWAN */
   if (cmsUtl_strstr(wanL2IfName, ETH_IFC_STR))
   {
      WanEthIntfObject *ethIntfCfg=NULL;

      if (dalEth_getEthIntfByIfName(wanL2IfName, &iidStack, &ethIntfCfg) == FALSE)
      {
         cmsLog_error("Failed to get ethIntfCfg iidStack");
         return CMSRET_INTERNAL_ERROR;
      }
      cmsObj_free((void **) &ethIntfCfg);      
#ifdef DMP_ADSLWAN_1
      matched=TRUE;
#endif
   }
#endif

#endif /* DMP_BASELINE_1 */

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   if (cmsUtl_strstr(wanL2IfName, MOCA_IFC_STR))
   {
      WanMocaIntfObject *mocaIntfCfg;

      if (dalMoca_getWanMocaIntfByIfName(wanL2IfName, &iidStack, (void **)&mocaIntfCfg) == FALSE)
      {
         cmsLog_error("Failed to get mocaIntfCfg iidStack");
         return CMSRET_INTERNAL_ERROR;
      }
      cmsObj_free((void **) &mocaIntfCfg);      
#ifdef DMP_ADSLWAN_1
      matched=TRUE;
#endif
   }
#endif

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   if (cmsUtl_strstr(wanL2IfName, GPON_IFC_STR) && !rut_isWanTypeEpon())
   {
      WanGponLinkCfgObject *gponLinkCfg;

      if (dalGpon_getGponLinkByIfName(wanL2IfName, &iidStack, &gponLinkCfg) == FALSE)
      {
         cmsLog_error("Failed to get gponLinkCfg");
         return CMSRET_INTERNAL_ERROR;
      }
      cmsObj_free((void **) &gponLinkCfg);      
#ifdef DMP_ADSLWAN_1
      matched=TRUE;
#endif
   }
#endif

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
   if (cmsUtl_strstr(wanL2IfName, EPON_IFC_STR) && rut_isWanTypeEpon())
   {
#ifdef EPON_SFU
      WanEponIntfObject *eponCfg;

      if (dalEpon_getEponIntfByIfName(wanL2IfName, &iidStack, &eponCfg) == FALSE)
      {
         cmsLog_error("Failed to get eponCfg");
         return CMSRET_INTERNAL_ERROR;
      }
      cmsObj_free((void **) &eponCfg);
#else
      WanEponLinkCfgObject *eponLinkCfg;

      if (dalEpon_getEponLinkByIfName(wanL2IfName, &iidStack, &eponLinkCfg) == FALSE)
      {
         cmsLog_error("Failed to get eponLinkCfg");
         return CMSRET_INTERNAL_ERROR;
      }
      cmsObj_free((void **) &eponLinkCfg);
#endif
#ifdef DMP_ADSLWAN_1
      matched=TRUE;
#endif
   }
#endif

#ifdef DMP_ADSLWAN_1
#ifdef DMP_PTMWAN_1
   if (cmsUtl_strstr(wanL2IfName, PTM_IFC_STR) != NULL)
   {
      WanPtmLinkCfgObject *ptmLinkCfg = NULL;

      if (dalDsl_getPtmLinkByIfName(wanL2IfName, &iidStack, &ptmLinkCfg) == FALSE)
      {
         cmsLog_error("Failed to get ptmLinkCfg iidStack");
         return CMSRET_INTERNAL_ERROR;
      }
      matched=TRUE;

      cmsObj_free((void **) &ptmLinkCfg);
   }
#endif

#ifdef DMP_X_BROADCOM_COM_WIFIWAN_1
   if (cmsUtl_strstr(wanL2IfName, WLAN_IFC_STR))
   {
      WanWifiIntfObject *wifiIntfCfg=NULL;

      if (dalWifiWan_getWlIntfByIfName(wanL2IfName, &iidStack, &wifiIntfCfg) == FALSE)
      {
         cmsLog_error("Failed to get wifiIntfCfg iidStack");
         return CMSRET_INTERNAL_ERROR;
      }
      cmsObj_free((void **) &wifiIntfCfg);      
      matched=TRUE;
   }
#endif

   if (!matched) {
      /*
       * The WanConnectionDevice object should be already created by DSL Interface 
       * and need to get that iidStack to add either a WanPppConnection or WanIpConnection.
       */
      WanDslLinkCfgObject *dslLinkCfg = NULL;
      
      if (dalDsl_getDslLinkByIfName(wanL2IfName, &iidStack, &dslLinkCfg) == FALSE)
      {
         cmsLog_error("Failed to dslLinkCfg iidStack");
         return CMSRET_INTERNAL_ERROR;
      }

      cmsObj_free((void **) &dslLinkCfg);
   }
#endif /* DMP_ADSLWAN_1 */


   if (webVar->ntwkPrtcl == CMS_WAN_TYPE_BRIDGE ||
       webVar->ntwkPrtcl == CMS_WAN_TYPE_DYNAMIC_IPOE ||
       webVar->ntwkPrtcl == CMS_WAN_TYPE_STATIC_IPOE ||
       /* next 3 used by ethWan and mocaWan, no need to put in ifdef */
       webVar->ntwkPrtcl == CMS_WAN_TYPE_DYNAMIC_ETHERNET_IP ||
       webVar->ntwkPrtcl == CMS_WAN_TYPE_STATIC_ETHERNET_IP ||
       webVar->ntwkPrtcl == CMS_WAN_TYPE_ETHERNET_BRIDGE ||
       webVar->ntwkPrtcl == CMS_WAN_TYPE_IPOA)
       
   {         
      if ((ret = addWanIpConnection(webVar, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("addWanIpConnection failed. ret=%d", ret);
      }
   }
   /* Add WAN PPP CONN OBJ */
   else if (webVar->ntwkPrtcl == CMS_WAN_TYPE_PPPOE ||
            webVar->ntwkPrtcl == CMS_WAN_TYPE_ETHERNET_PPPOE ||
            webVar->ntwkPrtcl == CMS_WAN_TYPE_PPPOA)
   {         
      if ((ret = addWanPppConnection(webVar, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("addWanPppConnection failed. ret=%d", ret);
      }
#ifndef CMS_LOG0
      wanConnOid = MDMOID_WAN_PPP_CONN;
#endif
   }

#if defined(DMP_X_BROADCOM_COM_IPV6_1)
      if (ret != CMSRET_SUCCESS)
      {
         return ret;
      }
    
   if (((webVar->enblIpVer != ENBL_IPV4_ONLY)) &&
       (webVar->ntwkPrtcl == CMS_WAN_TYPE_DYNAMIC_IPOE ||
        webVar->ntwkPrtcl == CMS_WAN_TYPE_STATIC_IPOE ||
#ifdef DMP_ETHERNETWAN_1  /* aka SUPPORT_ETHWAN */
        webVar->ntwkPrtcl == CMS_WAN_TYPE_DYNAMIC_ETHERNET_IP ||
        webVar->ntwkPrtcl == CMS_WAN_TYPE_STATIC_ETHERNET_IP ||
        webVar->ntwkPrtcl == CMS_WAN_TYPE_ETHERNET_PPPOE ||
#endif 
        webVar->ntwkPrtcl == CMS_WAN_TYPE_PPPOE))
   {
      if ((ret = cmsDal_setSysDfltGw6Ifc_igd((char *)(webVar->dfltGw6Ifc))) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsDal_setSysDfltGw6Ifc_igd returns error. ret=%d", ret);
         return ret;
      }

      if ((ret = cmsDal_setDns6Info_igd((char *)(webVar->dns6Type),
                                    (char *)(webVar->dns6Ifc),
                                    (char *)(webVar->dns6Pri),
                                    (char *)(webVar->dns6Sec))) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsDal_setDns6Info_igd returns error. ret=%d", ret);
      }
   }
#endif


#if defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1) && defined(DMP_DEVICE2_BASELINE_1)
   {
      /*
       * Tricky Hack Alert: In Hybrid IPv6 mode, this code will pass the
       * Layer 3 intf name (PPP or IP conn) over to the TR181 IPv6 code
       * for use in the lowerlayer param of the Ethernet.Link object.
       */
      if (ret == CMSRET_SUCCESS && webVar->enblIpVer != ENBL_IPV4_ONLY)
      {
         void *wanConnObj=NULL;
         if ((ret = cmsObj_get(wanConnOid, &iidStack, 0, (void **) &wanConnObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to get WanIP/PPP Conn, ret = %d", ret);
            return ret;
         } 
         if (wanConnOid == MDMOID_WAN_IP_CONN)
         {
            strncpy((char *) webVar->wanIfName, ((WanIpConnObject *) wanConnObj)->X_BROADCOM_COM_IfName, sizeof(webVar->wanIfName)-1);
         }
         else
         {
            strncpy((char *)webVar->wanIfName, ((WanPppConnObject *) wanConnObj)->X_BROADCOM_COM_IfName, sizeof(webVar->wanIfName)-1);
         }
         cmsObj_free((void **) &wanConnObj);    
      }
   }
#endif /* hybrid TR98 and TR181 */

   return ret;

}


CmsRet addWanPppConnection(const WEB_NTWK_VAR *webVar, InstanceIdStack *iidStack)
{
   void *linkCfg = NULL;
   WanPppConnObject    *wanPppConn = NULL; 
   UBOOL8 vlanMuxEnab = FALSE;
   CmsRet ret;

   // get rid of warnings when linkCfg is not used due to compile options:
   (void)linkCfg;

   cmsLog_debug("adding %s at %s", webVar->wanL2IfName, cmsMdm_dumpIidStack(iidStack));

#ifdef DMP_ADSLWAN_1
   if (cmsUtl_strstr(webVar->wanL2IfName, ATM_IFC_STR))
   {
      if ((ret = cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, iidStack, 0, &linkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanDslLinkCfgObject, ret=%d", ret);
         return ret;
      }
      vlanMuxEnab =  !cmsUtl_strcmp(((WanDslLinkCfgObject *)linkCfg)->X_BROADCOM_COM_ConnectionMode, MDMVS_VLANMUXMODE);
      cmsObj_free(&linkCfg);   /* no longer needed */
   }

#ifdef DMP_PTMWAN_1
   else if (cmsUtl_strstr(webVar->wanL2IfName, PTM_IFC_STR))
   {
      if ((ret = cmsObj_get(MDMOID_WAN_PTM_LINK_CFG, iidStack, 0, &linkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanPtmLinkCfgObject, ret=%d", ret);
         return ret;
      }
      vlanMuxEnab =  !cmsUtl_strcmp(((WanPtmLinkCfgObject *)linkCfg)->X_BROADCOM_COM_ConnectionMode, MDMVS_VLANMUXMODE);
      cmsObj_free(&linkCfg);   /* no longer needed */
   }
#endif

#endif /* DMP_ADSLWAN_1 */    

#ifdef DMP_ETHERNETWAN_1  /* aka SUPPORT_ETHWAN */
   if (cmsUtl_strstr(webVar->wanL2IfName, ETH_IFC_STR))
   {
      void *obj;
      InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
      if ((ret = cmsObj_get(MDMOID_WAN_ETH_INTF, iidStack, 0, &linkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanEthIntfObject, ret=%d", ret);
         return ret;
      }
      vlanMuxEnab =  !cmsUtl_strcmp(((WanEthIntfObject *)linkCfg)->X_BROADCOM_COM_ConnectionMode, MDMVS_VLANMUXMODE);
      cmsObj_free(&linkCfg);   /* no longer needed */
      
      /* get iidstack of WANConnectionDevice */
      if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, iidStack, &iidStack1, (void **)&obj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanConnDevice, ret=%d", ret);
         return ret;
      }
      cmsObj_free(&obj);   /* no longer needed */
      *iidStack = iidStack1;
   }
#endif

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   if (cmsUtl_strstr(webVar->wanL2IfName, MOCA_IFC_STR))
   {
      void *obj = NULL;
      WanMocaIntfObject *wanMocaIntf=NULL;
      InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;

      if ((ret = cmsObj_get(MDMOID_WAN_MOCA_INTF, iidStack, OGF_NO_VALUE_UPDATE, (void **)&wanMocaIntf)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanMocaIntfObject, ret=%d", ret);
         return ret;
      }
      vlanMuxEnab =  !cmsUtl_strcmp(wanMocaIntf->connectionMode, MDMVS_VLANMUXMODE);
      cmsObj_free((void **) &wanMocaIntf);
      
      /* get iidstack of WANConnectionDevice */
      if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, iidStack, &iidStack1, (void **)&obj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanConnDevice, ret=%d", ret);
         return ret;
      }
      cmsObj_free(&obj);   /* no longer needed */
      *iidStack = iidStack1;
   }
#endif


#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   if (cmsUtl_strstr(webVar->wanL2IfName, GPON_IFC_STR) && !rut_isWanTypeEpon())
   {
      WanGponLinkCfgObject *gponLinkCfg=NULL;

      if ((ret = cmsObj_get(MDMOID_WAN_GPON_LINK_CFG, iidStack, OGF_NO_VALUE_UPDATE, (void **)&gponLinkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanGponLinkCfgObject, ret=%d", ret);
         return ret;
      }
      vlanMuxEnab =  !cmsUtl_strcmp(gponLinkCfg->connectionMode, MDMVS_VLANMUXMODE);
      cmsObj_free((void **) &gponLinkCfg);
   }
#endif


#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
   if (cmsUtl_strstr(webVar->wanL2IfName, EPON_IFC_STR) && rut_isWanTypeEpon())
   {
      WanEponLinkCfgObject *eponLinkCfg=NULL;
      
      if ((ret = cmsObj_get(MDMOID_WAN_EPON_LINK_CFG, iidStack, OGF_NO_VALUE_UPDATE, (void **)&eponLinkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanEponLinkCfgObject, ret=%d", ret);
         return ret;
      }
      vlanMuxEnab =  !cmsUtl_strcmp(eponLinkCfg->connectionMode, MDMVS_VLANMUXMODE);
      cmsObj_free((void **) &eponLinkCfg);
   }
#endif

#ifdef DMP_X_BROADCOM_COM_WIFIWAN_1
   if (cmsUtl_strstr(webVar->wanL2IfName, WLAN_IFC_STR))
   {
      void *obj;
      InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
      if ((ret = cmsObj_get(MDMOID_WAN_WIFI_INTF, iidStack, 0, &linkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanWifiIntfObject, ret=%d", ret);
         return ret;
      }
      cmsObj_free(&linkCfg);   /* no longer needed */
      
      /* get iidstack of WANConnectionDevice */
      if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, iidStack, &iidStack1, (void **)&obj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanConnDevice, ret=%d", ret);
         return ret;
      }
      cmsObj_free(&obj);   /* no longer needed */
      *iidStack = iidStack1;
   }
#endif

   cmsLog_debug("before add  WAN_PPP_CONN, iidStack=%s", cmsMdm_dumpIidStack(iidStack));

   /* add WAN_PPP_CONN instance */
   if ((ret = cmsObj_addInstance(MDMOID_WAN_PPP_CONN, iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add wanPppConnInstance, ret = %d", ret);
      return ret;
   }
   if ((ret = cmsObj_get(MDMOID_WAN_PPP_CONN, iidStack, 0, (void **) &wanPppConn)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get wanPppConn, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN, iidStack);
      return ret;
   }

   switch (webVar->ntwkPrtcl)
   {
   case CMS_WAN_TYPE_PPPOE:
   case CMS_WAN_TYPE_ETHERNET_PPPOE:
      /* get sevice name */
      if (strlen(webVar->pppServerName) > 0)
      {
         CMSMEM_REPLACE_STRING(wanPppConn->PPPoEServiceName, webVar->pppServerName);
      }

      /* Set connectionId for PPPoE only */
      if (webVar->connMode == CMS_CONNECTION_MODE_VLANMUX)
      {
         SINT32 conId;
         char *p;
         char wanL2IfName[BUFLEN_32];

         /* extract layer2 interface name, eg. atm0, ptm0 */
         strncpy(wanL2IfName, webVar->wanL2IfName, sizeof(wanL2IfName)-1);
         p = strchr(wanL2IfName, '/');
         if (p)
         {
            *p = '\0';
         }            
         if ((ret = cmsDal_getAvailableConIdForMSC(wanL2IfName, &conId)) == CMSRET_SUCCESS)
         {
            wanPppConn->X_BROADCOM_COM_ConnectionId = conId;
         }
         else
         {
            cmsLog_error("cmsDal_getAvailableConIdForMSC failed on %s. ret=%d", webVar->wanL2IfName, ret);
         }
      }         
      
      /* Continue to the next case PPPoA since rest of parameters are same as PPPoE */

   case CMS_WAN_TYPE_PPPOA:
      cmsMem_free(wanPppConn->connectionType);
      wanPppConn->connectionType = cmsMem_strdup(MDMVS_IP_ROUTED);
      cmsMem_free(wanPppConn->username);
      wanPppConn->username = cmsMem_strndup(webVar->pppUserName, BUFLEN_256);
      cmsMem_free(wanPppConn->password);
      wanPppConn->password = cmsMem_strndup(webVar->pppPassword, BUFLEN_256);


      /* Convert the number to string for auth protocol (method) */
      cmsMem_free(wanPppConn->PPPAuthenticationProtocol);
      wanPppConn->PPPAuthenticationProtocol = cmsMem_strdup(cmsUtl_numToPppAuthString(webVar->pppAuthMethod)); 
      
      /* get on demand ideltime out in seconds if it is enabled */
      if (webVar->enblOnDemand)
      {
         wanPppConn->idleDisconnectTime = webVar->pppTimeOut * 60;
      }
      else
      {
         /* 0 is no on demand feature */
         wanPppConn->idleDisconnectTime = 0;
      }

      /* If IPv4 is enabled, do IPv4 related stuff */
      if (webVar->enblIpVer != ENBL_IPV6_ONLY)
      {
         /* ppp ip extension, if TRUE, no NAT/Firewall */
         if (webVar->pppIpExtension)
         {
            wanPppConn->X_BROADCOM_COM_IPExtension = TRUE;
         }
         else
         {
            /* by default NAT/Firewall are enabled in ppp if ipExtension is FALSE */
            wanPppConn->NATEnabled = TRUE;
            wanPppConn->X_BROADCOM_COM_FullconeNATEnabled = webVar->enblFullcone;
            wanPppConn->X_BROADCOM_COM_FirewallEnabled = TRUE;
         }

         /* if static ip is used, save it */
         if (webVar->useStaticIpAddress)
         {
            wanPppConn->X_BROADCOM_COM_UseStaticIPAddress = TRUE;
            CMSMEM_REPLACE_STRING(wanPppConn->X_BROADCOM_COM_LocalIPAddress , webVar->pppLocalIpAddress);
         }

         wanPppConn->X_BROADCOM_COM_IPv4Enabled = TRUE;
      }
      else
      {
         /* by default firewall is enabled in PPPv6oE */
         wanPppConn->X_BROADCOM_COM_FirewallEnabled = TRUE;
         wanPppConn->X_BROADCOM_COM_IPv4Enabled = FALSE;
      }

      /* get ppp debug flag */
      wanPppConn->X_BROADCOM_COM_Enable_Debug = webVar->enblPppDebug;
      wanPppConn->X_BROADCOM_COM_AddPppToBridge = webVar->pppToBridge;

      wanPppConn->X_BROADCOM_COM_PCPMode = webVar->pcpMode;
      CMSMEM_REPLACE_STRING(wanPppConn->X_BROADCOM_COM_PCPServer, webVar->pcpServer);
      
#ifdef SUPPORT_IPV6
      if (webVar->enblIpVer != ENBL_IPV4_ONLY)
      {
         wanPppConn->X_BROADCOM_COM_IPv6Enabled = TRUE;
#if defined(DMP_X_BROADCOM_COM_IPV6_1)
         wanPppConn->X_BROADCOM_COM_UnnumberedModel = webVar->unnumberedModel;
         CMSMEM_REPLACE_STRING(wanPppConn->X_BROADCOM_COM_IPv6AddressingType, webVar->wanAddr6Type);
         if (cmsUtl_strcmp(webVar->wanAddr6Type, MDMVS_STATIC) == 0)
         {
            CMSMEM_REPLACE_STRING(wanPppConn->X_BROADCOM_COM_ExternalIPv6Address, webVar->wanAddr6);
            wanPppConn->X_BROADCOM_COM_IPv6PrefixDelegationEnabled = FALSE;
            CMSMEM_REPLACE_STRING(wanPppConn->X_BROADCOM_COM_DefaultIPv6Gateway, webVar->wanGtwy6);
         }
         else
         {
            wanPppConn->X_BROADCOM_COM_Dhcp6cForAddress = webVar->dhcp6cForAddr;
            wanPppConn->X_BROADCOM_COM_Dhcp6cForPrefixDelegation = webVar->dhcp6cForPd;
         }
#endif
      }
#endif
      break;
      
   default:
      cmsLog_error("only pppoe, pppoa are supported, ntwkPrctl=", webVar->ntwkPrtcl);
      ret = CMSRET_INTERNAL_ERROR;
      break;
   }

   if (ret == CMSRET_SUCCESS)
   {
      wanPppConn->enable = TRUE;

      wanPppConn->X_BROADCOM_COM_VlanMuxID    = vlanMuxEnab? webVar->vlanMuxId : VLANMUX_DISABLE;
      wanPppConn->X_BROADCOM_COM_VlanMux8021p = vlanMuxEnab? webVar->vlanMuxPr : VLANMUX_DISABLE;
      wanPppConn->X_BROADCOM_COM_VlanTpid     = vlanMuxEnab? webVar->vlanTpid : (UINT32)VLANMUX_DISABLE;
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      wanPppConn->X_BROADCOM_COM_IGMPEnabled  = webVar->enblIgmp;
      wanPppConn->X_BROADCOM_COM_IGMP_SOURCEEnabled  = webVar->enblIgmpMcastSource;
#endif
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
      wanPppConn->X_BROADCOM_COM_NoMcastVlanFilter  = webVar->noMcastVlanFilter;
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
      wanPppConn->X_BROADCOM_COM_MLDEnabled  = webVar->enblMld;
      wanPppConn->X_BROADCOM_COM_MLD_SOURCEEnabled  = webVar->enblMldMcastSource;
#endif

      /* fill in service name */
      if (strlen(webVar->serviceName) > 0)
      {
         CMSMEM_REPLACE_STRING(wanPppConn->name, webVar->serviceName);
      }
      /* set and activate wanPppConnObj */
      if ((ret = cmsObj_set(wanPppConn, iidStack)) == CMSRET_SUCCESS)
      {
         /* TODO: revisit here for IPv6 */
         if (webVar->enblIpVer != ENBL_IPV6_ONLY)
         {
#ifdef SUPPORT_ADVANCED_DMZ
            if ((wanPppConn->X_BROADCOM_COM_IPExtension == TRUE) &&
               (webVar->enableAdvancedDmz == TRUE))
            {
               /* Enable Eth Switch virtual ports */
               dalEsw_enableVirtualPorts(TRUE);
                  
               if ((ret = dalWan_addNonAdvancedDmzLan(webVar->nonDmzIpAddress,  webVar->nonDmzIpMask)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed to add non DMZ Lan, error=%d", ret);
                  return ret;
               }
            }
#endif /* SUPPORT_ADVANCED_DMZ */
         
            cmsLog_debug("cmsObj_set wanPppConn ok. Setting IPv4 gw,dns ifc...");
            /* set up gateway and dns info */
            ret = dalWan_setDefaultGatewayAndDns((char *)webVar->defaultGatewayList, (char *)webVar->dnsIfcsList, 
               (char *)webVar->dnsPrimary, (char *)webVar->dnsSecondary);
         }
      }
      else
      {
         cmsLog_error("set of wanPppConn failed");
      }
   }
   
   cmsObj_free((void **) &wanPppConn);   
   
   /* remove the ppp conn if there is a problem */
   if (ret != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("Failed to set wanPppConnObj, ret = %d, delete the instance", ret);
      if ((r2 = cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN, iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not delete WanPppConn instance, r2=%d", r2);
      }
   }
   
   return ret;
}


CmsRet addWanIpConnection(const WEB_NTWK_VAR *webVar, InstanceIdStack *iidStack)
{
   void *linkCfg = NULL;
   WanIpConnObject *wanIpConn = NULL; 
   UBOOL8 vlanMuxEnab = FALSE;
   CmsRet ret;

   // supress warnings for compile configurations where linkCfg is not used:
   (void)linkCfg;

   /*
    * mwang: there is a lot of code repetition among dalWan_addInterface,
    * addWanIpConnection, and addWanPppConnection.  Need to consolidate.
    */
   cmsLog_debug("adding %s at %s", webVar->wanL2IfName, cmsMdm_dumpIidStack(iidStack));

#ifdef DMP_ADSLWAN_1
   if (cmsUtl_strstr(webVar->wanL2IfName, ATM_IFC_STR))
   {
      if ((ret = cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, iidStack, 0, &linkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanDslLinkCfgObject, ret=%d", ret);
         return ret;
      }
      vlanMuxEnab =  !cmsUtl_strcmp(((WanDslLinkCfgObject *)linkCfg)->X_BROADCOM_COM_ConnectionMode, MDMVS_VLANMUXMODE);
      cmsObj_free(&linkCfg);   /* no longer needed */
   }
   
#ifdef DMP_PTMWAN_1
   if (cmsUtl_strstr(webVar->wanL2IfName, PTM_IFC_STR))
   {
      if ((ret = cmsObj_get(MDMOID_WAN_PTM_LINK_CFG, iidStack, 0, &linkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanPtmLinkCfgObject, ret=%d", ret);
         return ret;
      }
      vlanMuxEnab =  !cmsUtl_strcmp(((WanPtmLinkCfgObject *)linkCfg)->X_BROADCOM_COM_ConnectionMode, MDMVS_VLANMUXMODE);
      cmsObj_free(&linkCfg);   /* no longer needed */
   }
#endif

#endif /* DMP_ADSLWAN_1 */

#ifdef DMP_ETHERNETWAN_1  /* aka SUPPORT_ETHWAN */
   if (cmsUtl_strstr(webVar->wanL2IfName, ETH_IFC_STR))
   {
      void *obj = NULL;
      InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
      if ((ret = cmsObj_get(MDMOID_WAN_ETH_INTF, iidStack, 0, &linkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanEthIntfObject, ret=%d", ret);
         return ret;
      }
      vlanMuxEnab =  !cmsUtl_strcmp(((WanEthIntfObject *)linkCfg)->X_BROADCOM_COM_ConnectionMode, MDMVS_VLANMUXMODE);
      cmsObj_free(&linkCfg);   /* no longer needed */
      
      /* get iidstack of WANConnectionDevice */
      if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, iidStack, &iidStack1, (void **)&obj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanConnDevice, ret=%d", ret);
         return ret;
      }
      cmsObj_free(&obj);   /* no longer needed */
      *iidStack = iidStack1;
   }
#endif

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   if (cmsUtl_strstr(webVar->wanL2IfName, MOCA_IFC_STR))
   {
      void *obj = NULL;
      WanMocaIntfObject *wanMocaIntf=NULL;
      InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;

      if ((ret = cmsObj_get(MDMOID_WAN_MOCA_INTF, iidStack, OGF_NO_VALUE_UPDATE, (void **)&wanMocaIntf)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanMocaIntfObject, ret=%d", ret);
         return ret;
      }
      vlanMuxEnab =  !cmsUtl_strcmp(wanMocaIntf->connectionMode, MDMVS_VLANMUXMODE);
      cmsObj_free((void **) &wanMocaIntf);
      
      /* get iidstack of WANConnectionDevice */
      if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, iidStack, &iidStack1, (void **)&obj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanConnDevice, ret=%d", ret);
         return ret;
      }
      cmsObj_free(&obj);   /* no longer needed */
      *iidStack = iidStack1;
   }
#endif


#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   if (cmsUtl_strstr(webVar->wanL2IfName, GPON_IFC_STR) && !rut_isWanTypeEpon())
   {
      WanGponLinkCfgObject *gponLinkCfg=NULL;

      if ((ret = cmsObj_get(MDMOID_WAN_GPON_LINK_CFG, iidStack, OGF_NO_VALUE_UPDATE, (void **)&gponLinkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanGponLinkCfgObject, ret=%d", ret);
         return ret;
      }
      vlanMuxEnab =  !cmsUtl_strcmp(gponLinkCfg->connectionMode, MDMVS_VLANMUXMODE);
      cmsObj_free((void **) &gponLinkCfg);
   }
#endif

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
   if (cmsUtl_strstr(webVar->wanL2IfName, EPON_IFC_STR) && rut_isWanTypeEpon())
   {
      WanEponLinkCfgObject *eponLinkCfg=NULL;
      
      if ((ret = cmsObj_get(MDMOID_WAN_EPON_LINK_CFG, iidStack, OGF_NO_VALUE_UPDATE, (void **)&eponLinkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get MDMOID_WAN_EPON_INTF, ret=%d", ret);
         return ret;
      }
      vlanMuxEnab =  !cmsUtl_strcmp(eponLinkCfg->connectionMode, MDMVS_VLANMUXMODE);
      cmsObj_free((void **) &eponLinkCfg);
   }
#endif

#ifdef DMP_X_BROADCOM_COM_WIFIWAN_1 
   if (cmsUtl_strstr(webVar->wanL2IfName, WLAN_IFC_STR))
   {
      void *obj = NULL;
      InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
      if ((ret = cmsObj_get(MDMOID_WAN_WIFI_INTF, iidStack, 0, &linkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanWifiIntfObject, ret=%d", ret);
         return ret;
      }
      vlanMuxEnab = cmsUtl_strcmp(((WanWifiIntfObject *)linkCfg)->connectionMode, MDMVS_VLANMUXMODE);
      cmsObj_free(&linkCfg);   /* no longer needed */
      
      /* get iidstack of WANConnectionDevice */
      if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, iidStack, &iidStack1, (void **)&obj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanConnDevice, ret=%d", ret);
         return ret;
      }
      cmsObj_free(&obj);   /* no longer needed */
      *iidStack = iidStack1;

      /*
       * Set URE Mode: 1: Range Extender; 2: Travel Router
       */
      {
         SINT32 ureMode=0;

          switch (webVar->ntwkPrtcl)
          {
          case CMS_WAN_TYPE_BRIDGE:
          case CMS_WAN_TYPE_ETHERNET_BRIDGE:
             ureMode = 1;  /* Bridged mode: Range Extender */
             break;

          case CMS_WAN_TYPE_DYNAMIC_IPOE:
          case CMS_WAN_TYPE_DYNAMIC_ETHERNET_IP:
             ureMode = 2;  /* Routed mode: travel router */
             break;

          default:
             cmsLog_error("Unsupported ntwkPrtcl %d", webVar->ntwkPrtcl);
             return CMSRET_INVALID_ARGUMENTS;

          }

          if (dalWifiWan_setUreMode(ureMode))
          {
             cmsLog_error("could not set ureMode to %d", ureMode);
             return CMSRET_INVALID_ARGUMENTS;
          }
      }
   }
#endif

   cmsLog_debug("before add of WAN_IP_CONN, iidStack=%s", cmsMdm_dumpIidStack(iidStack));

   /* add WAN_IP_CONN instance */
   if ((ret = cmsObj_addInstance(MDMOID_WAN_IP_CONN, iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add wanIpConnInstance, ret = %d", ret);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_WAN_IP_CONN, iidStack, 0, (void **) &wanIpConn)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get wanIpConn, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_WAN_IP_CONN, iidStack);
      return ret;
   }

   switch (webVar->ntwkPrtcl)
   {
   case CMS_WAN_TYPE_BRIDGE:
   case CMS_WAN_TYPE_ETHERNET_BRIDGE:
      cmsMem_free(wanIpConn->connectionType);
      wanIpConn->connectionType = cmsMem_strdup(MDMVS_IP_BRIDGED);
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      wanIpConn->X_BROADCOM_COM_IGMP_SOURCEEnabled = webVar->enblIgmpMcastSource;
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
      wanIpConn->X_BROADCOM_COM_MLD_SOURCEEnabled = webVar->enblMldMcastSource;
#endif
      break;
      
   case CMS_WAN_TYPE_DYNAMIC_IPOE:
      /* used by both ethWan and mocaWan, no need to put in ifdef */      
   case CMS_WAN_TYPE_DYNAMIC_ETHERNET_IP:
      cmsMem_free(wanIpConn->connectionType);
      wanIpConn->connectionType = cmsMem_strdup(MDMVS_IP_ROUTED);
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      wanIpConn->X_BROADCOM_COM_IGMPEnabled = webVar->enblIgmp;
      wanIpConn->X_BROADCOM_COM_IGMP_SOURCEEnabled = webVar->enblIgmpMcastSource;
#endif
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
      wanIpConn->X_BROADCOM_COM_NoMcastVlanFilter = webVar->noMcastVlanFilter;
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
      wanIpConn->X_BROADCOM_COM_MLDEnabled = webVar->enblMld;
      wanIpConn->X_BROADCOM_COM_MLD_SOURCEEnabled = webVar->enblMldMcastSource;
#endif
      wanIpConn->X_BROADCOM_COM_FirewallEnabled = webVar->enblFirewall;
      wanIpConn->X_BROADCOM_COM_PCPMode = webVar->pcpMode;
      CMSMEM_REPLACE_STRING(wanIpConn->X_BROADCOM_COM_PCPServer, webVar->pcpServer);      

      /* If IPv4 is enabled, do the IPv4 stuff */
      if (webVar->enblIpVer != ENBL_IPV6_ONLY)
      {
         cmsMem_free(wanIpConn->externalIPAddress);
         wanIpConn->externalIPAddress = cmsMem_strndup(webVar->wanIpAddress, BUFLEN_16);
         cmsMem_free(wanIpConn->subnetMask);
         wanIpConn->subnetMask = cmsMem_strndup(webVar->wanSubnetMask, BUFLEN_16);
         cmsMem_free(wanIpConn->defaultGateway);
         wanIpConn->defaultGateway = cmsMem_strndup(webVar->wanIntfGateway, BUFLEN_16);
         wanIpConn->NATEnabled = webVar->enblNat;
         wanIpConn->X_BROADCOM_COM_FullconeNATEnabled = webVar->enblFullcone;

         cmsMem_free(wanIpConn->addressingType);
         /* if wanIpConn->externalIPAddress is "0.0.0.0", it is dynamic MER */
         if (cmsUtl_strcmp(wanIpConn->externalIPAddress, "0.0.0.0") == 0)
         {
            wanIpConn->addressingType = cmsMem_strdup(MDMVS_DHCP);
         }
         else
         {
            wanIpConn->addressingType = cmsMem_strdup(MDMVS_STATIC);
         }
         cmsMem_free(wanIpConn->X_BROADCOM_COM_Op60VenderID);
         if (strlen(webVar->dhcpcOp60VID))
         {
            wanIpConn->X_BROADCOM_COM_Op60VenderID = cmsMem_strdup(webVar->dhcpcOp60VID);
         }
         else
         {
            wanIpConn->X_BROADCOM_COM_Op60VenderID = NULL;
         }
         if (strlen(webVar->dhcpcOp61DUID))
         {
            wanIpConn->X_BROADCOM_COM_Op61DUID = cmsMem_strdup(webVar->dhcpcOp61DUID);
         }
         else
         {
            wanIpConn->X_BROADCOM_COM_Op61DUID = NULL;
         }
         if (strlen(webVar->dhcpcOp61IAID))
         {
            wanIpConn->X_BROADCOM_COM_Op61IAID = cmsMem_strdup(webVar->dhcpcOp61IAID);
         }
         else
         {
            wanIpConn->X_BROADCOM_COM_Op61IAID = NULL;
         }
         cmsMem_free(wanIpConn->X_BROADCOM_COM_Op77UserID);
         if (strlen(webVar->dhcpcOp77UID))
         {
            wanIpConn->X_BROADCOM_COM_Op77UserID = cmsMem_strdup(webVar->dhcpcOp77UID);
         }
         else
         {
            wanIpConn->X_BROADCOM_COM_Op77UserID = NULL;
         }
         wanIpConn->X_BROADCOM_COM_Op125Enabled = webVar->dhcpcOp125Enabled;
         cmsMem_free(wanIpConn->X_BROADCOM_COM_Op50IpAddress);
         if (strlen(webVar->dhcpcOp50IpAddress))
         {
            wanIpConn->X_BROADCOM_COM_Op50IpAddress = cmsMem_strdup(webVar->dhcpcOp50IpAddress);
         }
         else
         {
            wanIpConn->X_BROADCOM_COM_Op50IpAddress = NULL;
         }
         wanIpConn->X_BROADCOM_COM_Op51LeasedTime = webVar->dhcpcOp51LeasedTime;
         cmsMem_free(wanIpConn->X_BROADCOM_COM_Op54ServerIpAddress);
         if (strlen(webVar->dhcpcOp54ServerIpAddress))
         {
            wanIpConn->X_BROADCOM_COM_Op54ServerIpAddress = cmsMem_strdup(webVar->dhcpcOp54ServerIpAddress);
         }
         else
         {
            wanIpConn->X_BROADCOM_COM_Op54ServerIpAddress = NULL;
         }

         wanIpConn->X_BROADCOM_COM_IPv4Enabled = TRUE;
      }
      else
      {
         wanIpConn->X_BROADCOM_COM_IPv4Enabled = FALSE;
      }
#ifdef SUPPORT_IPV6
      if (webVar->enblIpVer != ENBL_IPV4_ONLY)
      {
         wanIpConn->X_BROADCOM_COM_IPv6Enabled = TRUE;
#if defined(DMP_X_BROADCOM_COM_IPV6_1)
         CMSMEM_REPLACE_STRING(wanIpConn->X_BROADCOM_COM_IPv6AddressingType, webVar->wanAddr6Type);
         if (cmsUtl_strcmp(webVar->wanAddr6Type, MDMVS_STATIC) == 0)
         {
            CMSMEM_REPLACE_STRING(wanIpConn->X_BROADCOM_COM_ExternalIPv6Address, webVar->wanAddr6);
            wanIpConn->X_BROADCOM_COM_IPv6PrefixDelegationEnabled = FALSE;
            CMSMEM_REPLACE_STRING(wanIpConn->X_BROADCOM_COM_DefaultIPv6Gateway, webVar->wanGtwy6);
         }
         else
         {
            wanIpConn->X_BROADCOM_COM_Dhcp6cForAddress = webVar->dhcp6cForAddr;
            wanIpConn->X_BROADCOM_COM_Dhcp6cForPrefixDelegation = webVar->dhcp6cForPd;
         }
#endif
      }
#endif
      break;
      
   case CMS_WAN_TYPE_IPOA:
      cmsMem_free(wanIpConn->connectionType);
      wanIpConn->connectionType = cmsMem_strdup(MDMVS_IP_ROUTED);

      /*
       * TODO: For IPv6 project, we don't have IPv6 over ATM now.
       */
      if (webVar->enblIpVer != ENBL_IPV6_ONLY)
      {
         cmsMem_free(wanIpConn->addressingType);
         wanIpConn->addressingType = cmsMem_strdup(MDMVS_STATIC);
         cmsMem_free(wanIpConn->externalIPAddress);
         wanIpConn->externalIPAddress = cmsMem_strndup(webVar->wanIpAddress, BUFLEN_16);
         cmsMem_free(wanIpConn->subnetMask);
         wanIpConn->subnetMask = cmsMem_strndup(webVar->wanSubnetMask, BUFLEN_16);
         wanIpConn->NATEnabled = webVar->enblNat;
         wanIpConn->X_BROADCOM_COM_FullconeNATEnabled = webVar->enblFullcone;
         wanIpConn->X_BROADCOM_COM_IPv4Enabled = TRUE;
      }
      else
      {
         wanIpConn->X_BROADCOM_COM_IPv4Enabled = FALSE;
      }
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      wanIpConn->X_BROADCOM_COM_IGMPEnabled = webVar->enblIgmp;
      wanIpConn->X_BROADCOM_COM_IGMP_SOURCEEnabled = webVar->enblIgmpMcastSource;
#endif
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
      wanIpConn->X_BROADCOM_COM_NoMcastVlanFilter = webVar->noMcastVlanFilter;
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
      wanIpConn->X_BROADCOM_COM_MLDEnabled = webVar->enblMld;
      wanIpConn->X_BROADCOM_COM_MLD_SOURCEEnabled = webVar->enblMldMcastSource;
#endif
      wanIpConn->X_BROADCOM_COM_FirewallEnabled = webVar->enblFirewall;
      wanIpConn->X_BROADCOM_COM_PCPMode = webVar->pcpMode;
      CMSMEM_REPLACE_STRING(wanIpConn->X_BROADCOM_COM_PCPServer, webVar->pcpServer);      
      break;
      
   default:
      cmsLog_error("unrecognized/unsupported ntwkPrtcl type %d", webVar->ntwkPrtcl);
      ret = CMSRET_INTERNAL_ERROR;
      break;
   }

   if (ret == CMSRET_SUCCESS)
   {
      wanIpConn->enable = TRUE;

      wanIpConn->X_BROADCOM_COM_VlanMuxID    = vlanMuxEnab? webVar->vlanMuxId : VLANMUX_DISABLE;
      wanIpConn->X_BROADCOM_COM_VlanMux8021p = vlanMuxEnab? webVar->vlanMuxPr : VLANMUX_DISABLE;
      wanIpConn->X_BROADCOM_COM_VlanTpid     = vlanMuxEnab? webVar->vlanTpid : (UINT32)VLANMUX_DISABLE;


      /* Set connectionId if connMode is MSC */
      if (webVar->connMode == CMS_CONNECTION_MODE_VLANMUX)
      {
         SINT32 conId;
         char *p;
         char wanL2IfName[BUFLEN_32];

         /* extract layer2 interface name, eg. atm0, ptm0 */
         strncpy(wanL2IfName, webVar->wanL2IfName, sizeof(wanL2IfName)-1);

         p = strchr(wanL2IfName, '/');
         if (p)
         {
            *p = '\0';
         }            
         if ((ret = cmsDal_getAvailableConIdForMSC(wanL2IfName, &conId)) == CMSRET_SUCCESS)
         {
            wanIpConn->X_BROADCOM_COM_ConnectionId = conId;
         }
         else
         {
            cmsLog_error("cmsDal_getAvailableConIdForMSC failed on %s. ret=%d", webVar->wanL2IfName, ret);
         }

      }         

      /* fill in service name */
      if (strlen(webVar->serviceName) > 0)
      {
         CMSMEM_REPLACE_STRING(wanIpConn->name, webVar->serviceName);
      }

      /* set and activate wanIpConnObj */
      if (cmsObj_set(wanIpConn, iidStack) == CMSRET_SUCCESS)
      {
         /* TODO: revisit here for IPv6 */
         if (webVar->enblIpVer != ENBL_IPV6_ONLY)
         {
#ifdef SUPPORT_ADVANCED_DMZ
            /*
             * Advanced DMZ for MER only in IPConnObject
             */
            cmsLog_debug("bef Added a non dmz lan for MER.");

            if ((webVar->ntwkPrtcl == CMS_WAN_TYPE_DYNAMIC_IPOE) &&              
                  (webVar->enableAdvancedDmz == TRUE))
            {
               /* Enable Eth Switch virtual ports */
               dalEsw_enableVirtualPorts(TRUE);
                  
               if ((ret = dalWan_addNonAdvancedDmzLan(webVar->nonDmzIpAddress,  webVar->nonDmzIpMask)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed to add non DMZ Lan, error=%d", ret);
                  return ret;
               }
               cmsLog_debug("Added a non dmz lan for MER.");
            }
#endif /* SUPPORT_ADVANCED_DMZ */

            cmsLog_debug("cmsObj_set wanIpConn ok. Now setting default gateway");

            /* set up gateway and dns info */
            ret = dalWan_setDefaultGatewayAndDns((char *)webVar->defaultGatewayList, (char *)webVar->dnsIfcsList, 
               (char *)webVar->dnsPrimary, (char *)webVar->dnsSecondary);
         }
      }

      /* remove the ip conn if there is a problem */
      if (ret != CMSRET_SUCCESS)
      {
         CmsRet r2;
         cmsLog_error("Failed to set wanIpConnObj, ret = %d, delete the instance", ret);
         if ((r2 = cmsObj_deleteInstance(MDMOID_WAN_IP_CONN, iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not delete WanIpConn instance, r2=%d", r2);
         }
      }
   }

   cmsObj_free((void **) &wanIpConn);
   
   return ret;
}


CmsRet dalWan_deleteService_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack wanConIid;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("delete serviceIfcName=%s", webVar->wanIfName);

   /*
    * delete either a WanIpConnection or WanPppConnection based on wanIfName match.
    */
   INIT_INSTANCE_ID_STACK(&wanConIid);
   if ((ret = deleteIpOrPppConnection((char*)webVar->wanIfName, &wanConIid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("deleteIpOrPppConnection return error. ret=%d", ret);
      return ret;
   }


#ifdef SUPPORT_ADVANCED_DMZ
    /* always delete Non Advanced Dmz LAN if it is enabled -- ONLY 1 pvc allowed for advanced DMZ */
    if (dalWan_isAdvancedDmzEnabled() == TRUE)
    {
        /* disable Eth Switch virtual ports */
        dalEsw_enableVirtualPorts(FALSE);    
        
        ret = dalWan_deleteNonAdvancedDmzLan();
    }
#endif /* SUPPORT_ADVANCED_DMZ */

#if defined(DMP_DEVICE2_BASELINE_1) && defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)
    /*
     * This function may be called in Hybrid mode, so we might have to
     * delete the TR181 IPv6 IP.Interface and related objects.
     */
    dalWan_deleteService_dev2(webVar);
#endif

   return ret;

}


CmsRet dalWan_editInterface_igd(const WEB_NTWK_VAR *webVar)
{

   UBOOL8 isPpp = FALSE;
   void *wanConnObj=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 change = FALSE;
   CmsRet ret = CMSRET_SUCCESS;
   
   if (CMSRET_SUCCESS != dalWan_getIpOrPppObjectByIfname(webVar->wanIfName, &iidStack, &wanConnObj, &isPpp))
   {
      cmsLog_error("cannot find obj for %s", webVar->wanIfName);
      return CMSRET_INTERNAL_ERROR;
   }

   if (isPpp)
   {
      /* For pppoe and pppoa */
      WanPppConnObject *pppConn = (WanPppConnObject *) wanConnObj;

      /* PPPoE service name is for PPPoE only, not PPPoA */
      if (!dalWan_isPPPoA(&iidStack) && cmsUtl_strcmp(pppConn->PPPoEServiceName, webVar->pppServerName))
      {
         CMSMEM_REPLACE_STRING(pppConn->PPPoEServiceName, webVar->pppServerName); 
         change = TRUE;
      }      
      if (cmsUtl_strcmp(pppConn->username, webVar->pppUserName))
      {
         CMSMEM_REPLACE_STRING(pppConn->username, webVar->pppUserName);
         change = TRUE;
      }      
      if (cmsUtl_strcmp(pppConn->password, webVar->pppPassword))
      {
         CMSMEM_REPLACE_STRING(pppConn->password, webVar->pppPassword);
         change = TRUE;
      }      
      if (cmsUtl_pppAuthToNum(pppConn->PPPAuthenticationProtocol) != webVar->pppAuthMethod)
      {
         cmsMem_free(pppConn->PPPAuthenticationProtocol);
         pppConn->PPPAuthenticationProtocol = cmsMem_strdup(cmsUtl_numToPppAuthString(webVar->pppAuthMethod)); 
         change = TRUE;         
      }
      if (pppConn->X_BROADCOM_COM_FullconeNATEnabled != webVar->enblFullcone) 
      {
         pppConn->X_BROADCOM_COM_FullconeNATEnabled = webVar->enblFullcone;
         change = TRUE;
      }
      /* pppConn->idleDisconnectTime == 0 ==> NOT enabling On Demand */
      if (pppConn->idleDisconnectTime == 0 && webVar->enblOnDemand)
      {
         pppConn->idleDisconnectTime = webVar->pppTimeOut * 60;
         change = TRUE;
      }
      if (pppConn->idleDisconnectTime != 0 && !webVar->enblOnDemand)
      {
          /* 0 is no OnDemand feature.  Change from OnDemand to not OnDemand */
         pppConn->idleDisconnectTime = 0;
         change = TRUE;         
      }
      if (pppConn->idleDisconnectTime != 0 && webVar->enblOnDemand)
      {
         /* Need to set change to TRUE if the idleDisconnectTim differs */
         if (pppConn->idleDisconnectTime != (UINT32) (webVar->pppTimeOut * 60))
         {
            pppConn->idleDisconnectTime = (UINT32) webVar->pppTimeOut * 60;
            change = TRUE;
         }            
      }
      if (pppConn->X_BROADCOM_COM_UseStaticIPAddress != webVar->useStaticIpAddress)
      {
         pppConn->X_BROADCOM_COM_UseStaticIPAddress = webVar->useStaticIpAddress;
         CMSMEM_REPLACE_STRING(pppConn->X_BROADCOM_COM_LocalIPAddress , webVar->pppLocalIpAddress);
         change = TRUE;         
      }    
      
      if (pppConn->X_BROADCOM_COM_UseStaticIPAddress &&  webVar->useStaticIpAddress)
      {
      
         if (cmsUtl_strcmp(pppConn->X_BROADCOM_COM_LocalIPAddress, webVar->pppLocalIpAddress))
         {
            /* Need to set change to TRUE if the locaIPAddress differs */
            CMSMEM_REPLACE_STRING(pppConn->X_BROADCOM_COM_LocalIPAddress , webVar->pppLocalIpAddress);
            change = TRUE;             
         }
      }
      
      if (pppConn->X_BROADCOM_COM_Enable_Debug != webVar->enblPppDebug)
      {
         pppConn->X_BROADCOM_COM_Enable_Debug = webVar->enblPppDebug;
         change = TRUE;         
      }
      if (pppConn->X_BROADCOM_COM_AddPppToBridge != webVar->pppToBridge)
      {
         pppConn->X_BROADCOM_COM_AddPppToBridge = webVar->pppToBridge;
         change = TRUE;
      }
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      if (pppConn->X_BROADCOM_COM_IGMPEnabled != webVar->enblIgmp)
      {
         pppConn->X_BROADCOM_COM_IGMPEnabled = webVar->enblIgmp;
         change = TRUE;
      } 
      if (pppConn->X_BROADCOM_COM_IGMP_SOURCEEnabled != webVar->enblIgmpMcastSource)
      {
         pppConn->X_BROADCOM_COM_IGMP_SOURCEEnabled = webVar->enblIgmpMcastSource;
         change = TRUE;
      } 
#endif
      if (pppConn->X_BROADCOM_COM_PCPMode != webVar->pcpMode) 
      {
         pppConn->X_BROADCOM_COM_PCPMode = webVar->pcpMode;
         change = TRUE;
      }        

#ifdef DMP_X_BROADCOM_COM_MLD_1
      if (pppConn->X_BROADCOM_COM_MLDEnabled != webVar->enblMld)
      {
         pppConn->X_BROADCOM_COM_MLDEnabled = webVar->enblMld;
         change = TRUE;
      } 
      if (pppConn->X_BROADCOM_COM_MLD_SOURCEEnabled != webVar->enblMldMcastSource)
      {
         pppConn->X_BROADCOM_COM_MLD_SOURCEEnabled = webVar->enblMldMcastSource;
         change = TRUE;
      } 
#endif
      
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1      
      if (pppConn->X_BROADCOM_COM_NoMcastVlanFilter != webVar->noMcastVlanFilter)
      {
         pppConn->X_BROADCOM_COM_NoMcastVlanFilter = webVar->noMcastVlanFilter;
         change = TRUE;
      }
#endif  /* DMP_X_BROADCOM_COM_GPONWAN_1 */

      if (change)
      {
         /* Need to disable the WAN connection first in the cmsObj_set below */
         pppConn->enable = FALSE;
      }      
   }
   else
   {
      /* For IPoE, IPoA */
      WanIpConnObject *ipConn = (WanIpConnObject *) wanConnObj;

      if (ipConn->NATEnabled != webVar->enblNat) 
      {
         ipConn->NATEnabled = webVar->enblNat;
         change = TRUE;
      }
      if (ipConn->X_BROADCOM_COM_FullconeNATEnabled != webVar->enblFullcone) 
      {
         ipConn->X_BROADCOM_COM_FullconeNATEnabled = webVar->enblFullcone;
         change = TRUE;
      }    
      if (ipConn->X_BROADCOM_COM_FirewallEnabled != webVar->enblFirewall) 
      {
         ipConn->X_BROADCOM_COM_FirewallEnabled = webVar->enblFirewall;
         change = TRUE;
      }  
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      if (ipConn->X_BROADCOM_COM_IGMPEnabled != webVar->enblIgmp)
      {
         ipConn->X_BROADCOM_COM_IGMPEnabled = webVar->enblIgmp;
         change = TRUE;
      } 
      if (ipConn->X_BROADCOM_COM_IGMP_SOURCEEnabled != webVar->enblIgmpMcastSource)
      {
         ipConn->X_BROADCOM_COM_IGMP_SOURCEEnabled = webVar->enblIgmpMcastSource;
         change = TRUE;
      } 
#endif
      if (ipConn->X_BROADCOM_COM_PCPMode != webVar->pcpMode) 
      {
         ipConn->X_BROADCOM_COM_PCPMode = webVar->pcpMode;
         change = TRUE;
      }
#ifdef DMP_X_BROADCOM_COM_MLD_1
      if (ipConn->X_BROADCOM_COM_MLDEnabled != webVar->enblMld)
      {
         ipConn->X_BROADCOM_COM_MLDEnabled = webVar->enblMld;
         change = TRUE;
      } 
      if (ipConn->X_BROADCOM_COM_MLD_SOURCEEnabled != webVar->enblMldMcastSource)
      {
         ipConn->X_BROADCOM_COM_MLD_SOURCEEnabled = webVar->enblMldMcastSource;
         change = TRUE;
      } 
#endif
      
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1      
      if (ipConn->X_BROADCOM_COM_NoMcastVlanFilter != webVar->noMcastVlanFilter)
      {
         ipConn->X_BROADCOM_COM_NoMcastVlanFilter = webVar->noMcastVlanFilter;
         change = TRUE;
      }
#endif  /* DMP_X_BROADCOM_COM_GPONWAN_1 */

      if (change)
      {
         /* Need to disable the WAN connection first in the cmsObj_set below */
         ipConn->enable = FALSE;
      }      
   }

   if (change)
   {
      /* Need to bring the WAN down first and reactivate in 
      * dalWan_enableDisableWanConnObj(webVar->wanIfName, TRUE) below.
      */

      ret = cmsObj_set(wanConnObj, &iidStack);
      
      cmsObj_free(&wanConnObj);
      
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to set ip/pppCon, ret = %d", ret);
      }      
      else
      {
         /* Reactivate this WAN connection */
         ret = dalWan_enableDisableWanConnObj(webVar->wanIfName, TRUE);
      }
   }
   else
   {
      cmsObj_free(&wanConnObj);
   }      
   
#if defined(DMP_DEVICE2_BASELINE_1) && defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)
    /*
     * This function may be called in Hybrid mode, so we might have to
     * update the TR181 IPv6 IP.Interface and related objects if ipv6 is enabled.
     */
   if (qdmIpIntf_isIpv6EnabledOnIntfNameLocked(webVar->wanIfName))
   {
      ret = dalWan_editInterface_dev2(webVar);
   }
#endif
   return ret;
   
}


CmsRet restoreDhcpInfo(void)
{
   CmsRet ret = CMSRET_SUCCESS;
   LanHostCfgObject *lanHostCfg = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanHostCfgObject *lanHostCfgDefault = NULL;
   InstanceIdStack iidStackDefault  = EMPTY_INSTANCE_ID_STACK;
      
   cmsLog_debug("Entered");

   /* need to restore default dhcp info */
   if ((ret = cmsObj_getNext(MDMOID_LAN_HOST_CFG, 
                                &iidStack, 
                                (void **) &lanHostCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get LanHostCfgObject, ret=%d", ret);
      return ret;
   }

   /* get default values back */
   if ((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, 
                                       &iidStackDefault,  
                                       OGF_DEFAULT_VALUES,
                                       (void **) &lanHostCfgDefault)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get default lan host cfg obj, ret=%d", ret);
      return ret;
   }

   /* put back the default values since ip extension had changed that */
   cmsMem_free(lanHostCfg->minAddress);
   lanHostCfg->minAddress = cmsMem_strdup(lanHostCfgDefault->minAddress);
   cmsMem_free(lanHostCfg->maxAddress);
   lanHostCfg->maxAddress = cmsMem_strdup(lanHostCfgDefault->maxAddress);
   cmsMem_free(lanHostCfg->DNSServers);
   lanHostCfg->DNSServers = cmsMem_strdup(lanHostCfgDefault->DNSServers);

   if ((ret = cmsObj_set(lanHostCfg,  &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set lan host cfg obj, ret=%d", ret);
      return ret;
   }

   return ret;

}



/** find the specified connection id, either in the WanIpConnection or
 * WanPppConnection object in the specified sub-tree and delete the object.
 */
static CmsRet deleteIpOrPppConnection(char *serviceIfcName, InstanceIdStack *wanConIid)
{
   CmsRet ret=CMSRET_SUCCESS;
   void *wanCon = NULL;
   InstanceIdStack conIid;
   char currIfc[BUFLEN_32];
   UBOOL8 found = FALSE;
   
   if (cmsUtl_strstr(serviceIfcName, PPP_IFC_STR))
   {
      WanPppConnObject *pppCon = NULL;   

      /* this is a PPP connection. We will search all WANPPPConnection objects for
       * the serviceIfcName.
       */
      INIT_INSTANCE_ID_STACK(&conIid);
      while (!found &&
             (ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &conIid, (void **)&pppCon)) == CMSRET_SUCCESS)
      {
         if (cmsUtl_strcmp(pppCon->X_BROADCOM_COM_IfName, serviceIfcName) == 0)
         {
            UBOOL8 isPPPIpExtension = pppCon->X_BROADCOM_COM_IPExtension;

            /* we need to return the parent WANConnectionDevice object of this deleted instance. */
            *wanConIid = conIid;
            if ((ret = cmsObj_getAncestor(MDMOID_WAN_CONN_DEVICE, MDMOID_WAN_PPP_CONN, wanConIid, &wanCon)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Could not find ancestor dslLinkCfg obj, ret=%d", ret);
               cmsObj_free((void **)&pppCon);
               return ret;
            }
            cmsObj_free(&wanCon);   /* no longer needed */

#if defined(DMP_X_BROADCOM_COM_IPV6_1) && defined(SUPPORT_DM_LEGACY98)
            if (pppCon->X_BROADCOM_COM_IPv6Enabled)
            {
               removeIfcFromNetworkCfg6(MDMOID_WAN_PPP_CONN, &conIid);
            }
#endif
            if ((ret = cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN, &conIid)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_deleteInstance<MDMOID_WAN_PPP_CONN> returns error. ret=%d", ret);
               cmsObj_free((void **)&pppCon);
               return ret;
            }

            /* need to restore dhcp info to default if it was ppp ip extension */
            if (isPPPIpExtension)
            {
               CmsRet r2;

               if ((r2 = restoreDhcpInfo()) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Could not restore dhcp info, ret=%d", r2);
               }
            }

            strncpy(currIfc, pppCon->X_BROADCOM_COM_IfName, sizeof(currIfc));
            found = TRUE;
         }
         cmsObj_free((void **)&pppCon);
      }
   }
   else
   {
      WanIpConnObject  *ipCon = NULL;

      /* this is an IP connection. We will search all WANIPConnection objects for
       * the serviceIfcName.
       */
      INIT_INSTANCE_ID_STACK(&conIid);
      while (!found &&
             (ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &conIid, (void **)&ipCon)) == CMSRET_SUCCESS)
      {
         if (cmsUtl_strcmp(ipCon->X_BROADCOM_COM_IfName, serviceIfcName) == 0)
         {
            /* we need to return the parent WANConnectionDevice object of this deleted instance. */
            *wanConIid = conIid;
            if ((ret = cmsObj_getAncestor(MDMOID_WAN_CONN_DEVICE, MDMOID_WAN_IP_CONN, wanConIid, &wanCon)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Could not find ancestor dslLinkCfg obj, ret=%d", ret);
               cmsObj_free((void **)&ipCon);
               return ret;
            }
            cmsObj_free(&wanCon);   /* no longer needed */

#if defined(DMP_X_BROADCOM_COM_IPV6_1) && defined(SUPPORT_DM_LEGACY98)
            if (ipCon->X_BROADCOM_COM_IPv6Enabled)
            {
               removeIfcFromNetworkCfg6(MDMOID_WAN_IP_CONN, &conIid);
            }
#endif
            if ((ret = cmsObj_deleteInstance(MDMOID_WAN_IP_CONN, &conIid)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_deleteInstance<MDMOID_WAN_IP_CONN> returns error. ret=%d", ret);
               cmsObj_free((void **)&ipCon);
               return ret;
            }

            strncpy(currIfc, ipCon->X_BROADCOM_COM_IfName, sizeof(currIfc));
            found = TRUE;
         }
         cmsObj_free((void **)&ipCon);
      }
   }

   if (!found)
   {
      cmsLog_error("Could not find connection to delete. serviceIfcName=%s", serviceIfcName);
   }

   return ret;

}  /* End of deleteIpOrPppConnection() */


UBOOL8 getIpConn(UINT32 portId __attribute((unused)),
                 const char *destAddr __attribute((unused)),
                 SINT32 connectionId,
                 InstanceIdStack *iidStack,
                 WanIpConnObject **ipConn)
{
   InstanceIdStack parentIidStack=EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject *obj;
   UBOOL8 found=FALSE;
   CmsRet ret;

#ifdef DMP_ADSLWAN_1

   /* First find the DslLinkCfg corresponding to the destAddr. */
   if (!getDslLinkCfg(portId, destAddr, &parentIidStack, NULL))
   {
      return FALSE;
   }
#else
   cmsLog_error("cannot get DslLinkCfg on non-DSL system!");
#endif


   /*
    * Found correct DslLinkCfg, now search the sub-tree rooted at the parentIidStack
    * for an IpConn object with the given connectionId.
    */
   INIT_INSTANCE_ID_STACK(iidStack);
   while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &parentIidStack, iidStack, (void **) &obj)) == CMSRET_SUCCESS)
   {
      if (obj->X_BROADCOM_COM_ConnectionId == connectionId)
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &obj);
      }
   }

   if (found)
   {
      if (ipConn != NULL)
      {
         *ipConn = obj;
      }
      else
      {
         cmsObj_free((void **) &obj);
      }
   }

   return found;
}


UBOOL8 getPppConn(UINT32 portId __attribute((unused)),
                  const char *destAddr __attribute((unused)),
                  SINT32 connectionId,
                  InstanceIdStack *iidStack, WanPppConnObject **pppConn)
{
   InstanceIdStack parentIidStack=EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *obj;
   UBOOL8 found=FALSE;
   CmsRet ret;


#ifdef DMP_ADSLWAN_1

   /* First find the DslLinkCfg corresponding to the destAddr. */
   if (!getDslLinkCfg(portId, destAddr, &parentIidStack, NULL))
   {
      return FALSE;
   }
#else
   cmsLog_error("cannot get DslLinkCfg on non-DSL system!");
#endif


   /*
    * Found correct DslLinkCfg, now search the sub-tree rooted at the parentIidStack
    * for a PppConn object with the given connectionId.
    */
   INIT_INSTANCE_ID_STACK(iidStack);
   while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &parentIidStack, iidStack, (void **) &obj)) == CMSRET_SUCCESS)
   {
      if (obj->X_BROADCOM_COM_ConnectionId == connectionId)
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &obj);
      }
   }

   if (found)
   {
      if (pppConn != NULL)
      {
         *pppConn = obj;
      }
      else
      {
         cmsObj_free((void **) &obj);
      }
   }

   return found;
}


CmsRet dalWan_enableDisableWanConnObj(const char *ifName, UBOOL8 isEnabled)
{
   CmsRet ret=CMSRET_SUCCESS;
   WanDevObject *wanDev=NULL;
   InstanceIdStack wanDevIid=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *pppCon=NULL;   
   WanIpConnObject *ipCon=NULL;
   UBOOL8 done=FALSE;

   cmsLog_debug("ifName<%s> isEnabled<%d>", ifName, isEnabled);
   while (cmsObj_getNextFlags(MDMOID_WAN_DEV, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&wanDev) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **)&wanDev);  /* no longer needed */

      /* get the related ipCon obj */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (!done &&
         cmsObj_getNextInSubTreeFlags(MDMOID_WAN_IP_CONN, &wanDevIid, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ipCon) == CMSRET_SUCCESS)      
      {
         if (!cmsUtl_strcmp(ipCon->X_BROADCOM_COM_IfName, ifName))
         {
            if (ipCon->enable != isEnabled)
            {
               ipCon->enable = isEnabled;
               if ((ret = cmsObj_set(ipCon, &iidStack)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed to set ipCon, ret = %d", ret);
                  done = TRUE;
               }      
               else
               {
                  cmsLog_debug("Done set ipCon");
               }
            }
            else
            {
               cmsLog_debug("Same. No change");
            }
         }
         cmsObj_free((void **)&ipCon);
      }
   
      /* get the related pppCon obj */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (!done &&
         cmsObj_getNextInSubTreeFlags(MDMOID_WAN_PPP_CONN, &wanDevIid, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&pppCon) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(pppCon->X_BROADCOM_COM_IfName, ifName))
         {

            if (pppCon->enable != isEnabled)
            {
               pppCon->enable = isEnabled;

               if ((ret = cmsObj_set(pppCon, &iidStack)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed to set pppCon, ret = %d", ret);
                  done = TRUE;
               }      
               else
               {
                  cmsLog_debug("Done set pppCon");
               }
            }
            else
            {
               cmsLog_debug("Same. No change");
            }            
         }
         cmsObj_free((void **)&pppCon);
      }
      
   }

   return ret;

}


UBOOL8 dalWan_isWanLayer2LinkUp(MdmObjectId wanConnId, const InstanceIdStack *iidStack)
{
   return (rutWl2_isWanLayer2LinkUp(wanConnId, iidStack));
}


UBOOL8 dalWan_isWanLayer3ServiceUp(const char *ifName, UBOOL8 isIPv4)
{
   return (qdmIpIntf_isWanInterfaceUpLocked(ifName, isIPv4));
}


#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
CmsRet dalWan_addDelegatedAddrEntry(const char *srvName, const char *ipv6str, const char *lanIntf, const char * mode)
{
   return (rutWan_addDelegatedAddrEntry(srvName, ipv6str, lanIntf, mode));
}
#endif


#ifdef DMP_ETHERNETWAN_1
CmsRet dalWan_getWanEthObject(InstanceIdStack *iidStack,
                              WanEthIntfObject **wanEthIntfObj)
{
   return (rutWl2_getWanEthObject(iidStack, wanEthIntfObj));
}
#endif


#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
CmsRet dalWan_getWanMocaObject(InstanceIdStack *iidStack,
                               WanMocaIntfObject **wanMocaIntfObj)
{
   return (rutWl2_getWanMocaObject(iidStack, wanMocaIntfObj));
}
#endif

#ifdef DMP_X_BROADCOM_COM_L2TPAC_1
CmsRet dalWan_getWanL2tpAcObject(InstanceIdStack *iidStack,
                                 L2tpAcIntfConfigObject **wanL2tpAcIntfObj)
{
   return (rutWl2_getWanL2tpAcObject(iidStack, wanL2tpAcIntfObj));
}
#endif

#ifdef DMP_X_BROADCOM_COM_PPTPAC_1
CmsRet dalWan_getWanPptpAcObject(InstanceIdStack *iidStack,
                                 PptpAcIntfConfigObject **wanPptpAcIntfObj)
{
   return (rutWl2_getWanPptpAcObject(iidStack, wanPptpAcIntfObj));
}
#endif

#ifdef DMP_ADSLWAN_1

CmsRet dalWan_getAdslFlags_igd(UINT32 *adslFlags)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfCfg = NULL;
   CmsRet ret;
   char cfgModType[BUFLEN_128];
   char *pToken = NULL;
   char *pLast = NULL;

   cmsLog_debug("Enter");

   /* we are assuming there is only 1 WANDevice */
   ret = cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfCfg);
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }

   *adslFlags = 0;
   strncpy(cfgModType,dslIntfCfg->X_BROADCOM_COM_AdslModulationCfg,BUFLEN_128);
   pToken = strtok_r(cfgModType, ", ", &pLast);
   while (pToken != NULL)
   {
      if (cmsUtl_strcmp(pToken,MDMVS_ADSL_MODULATION_ALL) == 0)
      {
#ifdef DMP_VDSL2WAN_1
        *adslFlags |= ANNEX_A_MODE_ALL_MOD_68;
#else
         *adslFlags |= ANNEX_A_MODE_ALL_MOD_48;
#endif
         break;
      }
      else 
      {
         if (cmsUtl_strcmp(pToken,MDMVS_ADSL_G_DMT) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_GDMT;
         }
         if (cmsUtl_strcmp(pToken,MDMVS_ADSL_2PLUS) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_ADSL2PLUS;
         }
         if (cmsUtl_strcmp(pToken,MDMVS_ADSL_G_DMT_BIS) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_ADSL2;
         }
         if (cmsUtl_strcmp(pToken,MDMVS_ADSL_RE_ADSL) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_ANNEXL;
         }
         if (cmsUtl_strcmp(pToken,MDMVS_ADSL_G_LITE) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_GLITE;
         }
         if (cmsUtl_strcmp(pToken,MDMVS_ADSL_ANSI_T1_413) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_T1413;
         }
         if (cmsUtl_strcmp(pToken,MDMVS_ANNEXM) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_ANNEXM;
         }         
#ifdef DMP_VDSL2WAN_1
        if (cmsUtl_strcmp(pToken,MDMVS_VDSL2) == 0)
            *adslFlags |=  ANNEX_A_MODE_VDSL2;
#ifdef SUPPORT_DSL_GFAST
        if (cmsUtl_strcmp(pToken,MDMVS_G_FAST) == 0)
            *adslFlags |=  ANNEX_A_MODE_GFAST;
#endif
#endif
      } /* else */
      pToken = strtok_r(NULL, ", ", &pLast);
   } /* while pToken */

   if (dslIntfCfg->X_BROADCOM_COM_ADSL2_AnnexM == TRUE)
   {
      *adslFlags |= ANNEX_A_MODE_ANNEXM;
   }
   
   if (cmsUtl_strcmp(dslIntfCfg->X_BROADCOM_COM_PhoneLinePair,MDMVS_OUTER_PAIR) == 0)
   {
      *adslFlags |= ANNEX_A_LINE_PAIR_OUTER;
   }
   else
      *adslFlags |= ANNEX_A_LINE_PAIR_INNER;

   if (cmsUtl_strcmp(dslIntfCfg->X_BROADCOM_COM_Bitswap,MDMVS_OFF) == 0)
   {
      *adslFlags |= ANNEX_A_BITSWAP_DISENABLE;
   }
   if (cmsUtl_strcmp(dslIntfCfg->X_BROADCOM_COM_SRA,MDMVS_ON) == 0)
   {
      *adslFlags |=  ANNEX_A_SRA_ENABLE;
   }

#ifdef DMP_VDSL2WAN_1
   *adslFlags |= (dslIntfCfg->X_BROADCOM_COM_VDSL_8a) ? VDSL_PROFILE_8a : 0;
   *adslFlags |= (dslIntfCfg->X_BROADCOM_COM_VDSL_8b) ? VDSL_PROFILE_8b : 0;
   *adslFlags |= (dslIntfCfg->X_BROADCOM_COM_VDSL_8c) ? VDSL_PROFILE_8c : 0;
   *adslFlags |= (dslIntfCfg->X_BROADCOM_COM_VDSL_8d) ? VDSL_PROFILE_8d : 0;
   *adslFlags |= (dslIntfCfg->X_BROADCOM_COM_VDSL_12a) ? VDSL_PROFILE_12a : 0;
   *adslFlags |= (dslIntfCfg->X_BROADCOM_COM_VDSL_12b) ? VDSL_PROFILE_12b : 0;
   *adslFlags |= (dslIntfCfg->X_BROADCOM_COM_VDSL_17a) ? VDSL_PROFILE_17a : 0;
   *adslFlags |= (dslIntfCfg->X_BROADCOM_COM_VDSL_30a) ? VDSL_PROFILE_30a : 0;
   *adslFlags |= (dslIntfCfg->X_BROADCOM_COM_VDSL_US0_8a) ? VDSL_US0_8a : 0;

   cmsLog_debug("VDSL2 profiles 8a=%d 8b=%d 8c=%d 8d=%d",
                dslIntfCfg->X_BROADCOM_COM_VDSL_8a,
                dslIntfCfg->X_BROADCOM_COM_VDSL_8b,
                dslIntfCfg->X_BROADCOM_COM_VDSL_8c,
                dslIntfCfg->X_BROADCOM_COM_VDSL_8d);
#endif

   cmsObj_free((void **) &dslIntfCfg);

   cmsLog_debug("End: ret %d, adslFlags 0x%x", ret, *adslFlags);
   return ret;
}


CmsRet dalWan_setAdslFlags_igd(UINT32 adslFlags)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfCfg = NULL;
   CmsRet ret;
   UINT32 modMask = 0;
   char cfgModType[BUFLEN_128];
   int len;

   cmsLog_debug("Enter: adslFlags 0x%x",adslFlags);

   /* we are assuming there is only 1 WANDevice */
   ret = cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfCfg);
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }
#ifdef DMP_VDSL2WAN_1
   modMask = ANNEX_A_MODE_ALL_MOD_68;
#else
   modMask = ANNEX_A_MODE_ALL_MOD_48;
#endif
   if ((adslFlags & modMask) == modMask)
   {
      cmsMem_free(dslIntfCfg->X_BROADCOM_COM_AdslModulationCfg);
      dslIntfCfg->X_BROADCOM_COM_AdslModulationCfg = cmsMem_strdup(MDMVS_ADSL_MODULATION_ALL);
   }
   else
   {
      memset(cfgModType,0,BUFLEN_128);
      if ((adslFlags & ANNEX_A_MODE_GDMT) == ANNEX_A_MODE_GDMT)
      {
         strcat(cfgModType,MDMVS_ADSL_G_DMT);
         strcat(cfgModType,", ");
      }
      if ((adslFlags & ANNEX_A_MODE_ADSL2PLUS) == ANNEX_A_MODE_ADSL2PLUS)
      {
         strcat(cfgModType,MDMVS_ADSL_2PLUS);
         strcat(cfgModType,", ");
      }
      if ((adslFlags & ANNEX_A_MODE_ADSL2) == ANNEX_A_MODE_ADSL2)
      {
         strcat(cfgModType,MDMVS_ADSL_G_DMT_BIS);
         strcat(cfgModType,", ");
      }
      if ((adslFlags & ANNEX_A_MODE_ANNEXL) == ANNEX_A_MODE_ANNEXL)
      {
         strcat(cfgModType,MDMVS_ADSL_RE_ADSL);
         strcat(cfgModType,", ");
      }
      if ((adslFlags &  ANNEX_A_MODE_GLITE) ==  ANNEX_A_MODE_GLITE)
      {
         strcat(cfgModType,MDMVS_ADSL_G_LITE);
         strcat(cfgModType,", ");
      }
      if ((adslFlags &  ANNEX_A_MODE_T1413) ==  ANNEX_A_MODE_T1413)
      {
         strcat(cfgModType,MDMVS_ADSL_ANSI_T1_413);
         strcat(cfgModType,", ");
      }
      if ((adslFlags & ANNEX_A_MODE_ANNEXM) ==  ANNEX_A_MODE_ANNEXM)
      {
         strcat(cfgModType, MDMVS_ANNEXM);
         strcat(cfgModType,", ");
      }
#ifdef DMP_VDSL2WAN_1
      if ((adslFlags &  ANNEX_A_MODE_VDSL2) ==  ANNEX_A_MODE_VDSL2)
      {
         strcat(cfgModType,MDMVS_VDSL2);
         strcat(cfgModType,", ");
      }
#ifdef SUPPORT_DSL_GFAST
      if ((adslFlags &  ANNEX_A_MODE_GFAST) ==  ANNEX_A_MODE_GFAST)
      {
         strcat(cfgModType,MDMVS_G_FAST);
         strcat(cfgModType,", ");
      }
#endif
#endif

      /* take out the last ", " */
      len = strlen(cfgModType);
      if (len > 2)
      {
         cfgModType[len-2] = '\0';
         cmsMem_free(dslIntfCfg->X_BROADCOM_COM_AdslModulationCfg);
         dslIntfCfg->X_BROADCOM_COM_AdslModulationCfg = cmsMem_strdup(cfgModType);
      }
      else
      {
         /* default will be all */
         cmsMem_free(dslIntfCfg->X_BROADCOM_COM_AdslModulationCfg);
         dslIntfCfg->X_BROADCOM_COM_AdslModulationCfg = cmsMem_strdup(MDMVS_ADSL_MODULATION_ALL);
      }
   } /* not MODALL*/

   /* We now have MDMVS_ANNEXM, so this is only for backward compatibility. */
   dslIntfCfg->X_BROADCOM_COM_ADSL2_AnnexM = ((adslFlags & ANNEX_A_MODE_ANNEXM) != 0);

   cmsMem_free(dslIntfCfg->X_BROADCOM_COM_PhoneLinePair);   
   if ((adslFlags & ANNEX_A_LINE_PAIR_OUTER) == ANNEX_A_LINE_PAIR_OUTER)
   {
      dslIntfCfg->X_BROADCOM_COM_PhoneLinePair = cmsMem_strdup(MDMVS_OUTER_PAIR);
   }
   else
   {
      dslIntfCfg->X_BROADCOM_COM_PhoneLinePair = cmsMem_strdup(MDMVS_INNER_PAIR);
   }

   /* bitswap */
   cmsMem_free(dslIntfCfg->X_BROADCOM_COM_Bitswap);
   if ((adslFlags & ANNEX_A_BITSWAP_DISENABLE) == ANNEX_A_BITSWAP_DISENABLE)
   {
      dslIntfCfg->X_BROADCOM_COM_Bitswap = cmsMem_strdup(MDMVS_OFF);
   }
   else
   {
      dslIntfCfg->X_BROADCOM_COM_Bitswap = cmsMem_strdup(MDMVS_ON);
   }

   cmsMem_free(dslIntfCfg->X_BROADCOM_COM_SRA);
   if ((adslFlags & ANNEX_A_SRA_ENABLE) == ANNEX_A_SRA_ENABLE)
   {
      dslIntfCfg->X_BROADCOM_COM_SRA = cmsMem_strdup(MDMVS_ON);
   }
   else
   {
      dslIntfCfg->X_BROADCOM_COM_SRA = cmsMem_strdup(MDMVS_OFF);
   }

#ifdef DMP_VDSL2WAN_1
   /* set the various VDSL profile bits, defined in adslctlapi.h */
   dslIntfCfg->X_BROADCOM_COM_VDSL_8a = ((adslFlags & VDSL_PROFILE_8a) != 0);
   dslIntfCfg->X_BROADCOM_COM_VDSL_8b = ((adslFlags & VDSL_PROFILE_8b) != 0);
   dslIntfCfg->X_BROADCOM_COM_VDSL_8c = ((adslFlags & VDSL_PROFILE_8c) != 0);
   dslIntfCfg->X_BROADCOM_COM_VDSL_8d = ((adslFlags & VDSL_PROFILE_8d) != 0);
   dslIntfCfg->X_BROADCOM_COM_VDSL_12a = ((adslFlags & VDSL_PROFILE_12a) != 0);
   dslIntfCfg->X_BROADCOM_COM_VDSL_12b = ((adslFlags & VDSL_PROFILE_12b) != 0);
   dslIntfCfg->X_BROADCOM_COM_VDSL_17a = ((adslFlags & VDSL_PROFILE_17a) != 0);
   dslIntfCfg->X_BROADCOM_COM_VDSL_30a = ((adslFlags & VDSL_PROFILE_30a) != 0);

   dslIntfCfg->X_BROADCOM_COM_VDSL_US0_8a = ((adslFlags & VDSL_US0_8a) != 0);
#endif

   ret = cmsObj_set((void *) dslIntfCfg,&iidStack );

   cmsObj_free((void **) &dslIntfCfg);

   cmsLog_debug("End: ret %d",ret);

   return ret;
}


#endif /* DMP_ADSLWAN_1 */


CmsRet  dalWan_fillPppIfName(UBOOL8 isPPPoE, char *pppName)
{
    return (rutWan_fillPppIfName(isPPPoE, pppName));
}


UBOOL8 dalWan_isValidWanInterface_igd(const char *ifName)
{
   InstanceIdStack iidStack;
   _WanPppConnObject *pppConn = NULL;
   _WanIpConnObject  *ipConn = NULL;
   UBOOL8 found = FALSE;

   cmsLog_debug("Enter: ifName=%s", ifName);
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   /* get the related ipConn obj */
   while (!found &&
          (cmsObj_getNextFlags(MDMOID_WAN_IP_CONN, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &ipConn)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ifName, ipConn->X_BROADCOM_COM_IfName))
      {
         found = TRUE;            
      }
      cmsObj_free((void **) &ipConn);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (!found &&
          (cmsObj_getNextFlags(MDMOID_WAN_PPP_CONN, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &pppConn)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ifName, pppConn->X_BROADCOM_COM_IfName))
      {
         found = TRUE;            
      }         
      cmsObj_free((void **) &pppConn);
   }
   
   cmsLog_debug("Exit: ifName=%s found=%d", ifName, found);

   return found;
}


CmsRet dalWan_validateIfNameList(const char *ifNameList, UINT32 maxCount,
                                 char **returnNewList)
{
   char *tmpList, *newList;
   char *currIfName, *ptr, *savePtr=NULL;
   UINT32 count=0;

   tmpList = cmsMem_strdup(ifNameList);
   newList = cmsMem_strdup(ifNameList);
   if (!tmpList || !newList)
   {
      cmsLog_error("Memory allocation failure");
      cmsMem_free(tmpList);
      cmsMem_free(newList);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   memset(newList, 0, strlen(newList));
   ptr = strtok_r(tmpList, ",", &savePtr);

   /* Verify each interface names ifNameList is valid */
   while (ptr && (count < maxCount))
   {
      currIfName=ptr;
      while ((isspace(*currIfName)) && (*currIfName != 0))
      {
         /* skip white space after comma */
         currIfName++;
      }

      if (dalWan_isValidWanInterface(currIfName))
      {
         if (newList[0] != '\0')
         {
            strcat(newList, ",");
         }
         strcat(newList, currIfName);
      }
      else
      {
         cmsLog_error("WAN interface name %s does not exist.", currIfName);
         cmsMem_free(tmpList);
         cmsMem_free(newList);
         return CMSRET_INVALID_ARGUMENTS;
      }

      count++;
      ptr = strtok_r(NULL, ",", &savePtr);
   }

   if (count >= maxCount)
   {
      cmsLog_error("Count exceeded maxCount %d, results truncated", maxCount);
   }

   cmsMem_free(tmpList);

   *returnNewList = newList;

   return CMSRET_SUCCESS;
}


extern CmsRet rutWl2_getL2IfnameFromL3Ifname(const char *l3Ifname, char *l2Ifname);
extern CmsRet rutWan_getIpOrPppObjectByIfname(const char *ifName, InstanceIdStack *iidStack, void **obj, UBOOL8 *isPpp);

CmsRet dalWan_getL2IfnameFromL3Ifname(const char *l3Ifname, char *l2Ifname)
{
   return (rutWl2_getL2IfnameFromL3Ifname(l3Ifname, l2Ifname));
}

CmsRet dalWan_getIpOrPppObjectByIfname(const char *ifName, InstanceIdStack *iidStack, void **obj, UBOOL8 *isPpp)
{
   return (rutWan_getIpOrPppObjectByIfname(ifName, iidStack, obj, isPpp));
}


/** Given a L3 ifname, get the descriptive L2 ifname
 *
 * The descriptive L2 ifname has the form atm0/(0_2_35)
 *
 * @param l3Ifname (IN) The L3 IfName
 * @param descriptiveL2Ifname (OUT) the descriptive L2 ifname.  This buffer
 *                                  must be at leaset 2*CMS_IFNAME_LENGTH because it can
 *                                  contain an ifname plus some numbers.
 *
 * @return TRUE if the L3 ifname was found and a descriptive L2 Ifname was
 *         put inside the descriptiveL2Ifname buffer that was passed in.
 */
static UBOOL8 dalWan_getDescriptiveL2IfnameForEdit(const char *l3Ifname, char *descriptiveL2Ifname)
{
   char l2Ifname[CMS_IFNAME_LENGTH]={0};   
   NameList *nl, *ifList = NULL;
   UBOOL8 found=FALSE;

   descriptiveL2Ifname[0] = '\0';

   if ((dalWan_getL2IfnameFromL3Ifname(l3Ifname, l2Ifname)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Fail to get L2 ifName from L3 ifName %s", l3Ifname);
      return FALSE;
   }         

   /* Need to find the wanL2IfName in the format of "atm0/(0_0_35)" for later Edit processing */
   if (cmsDal_getAvailableIfForWanService(&ifList, FALSE) != CMSRET_SUCCESS || ifList == NULL)
   {
      cmsLog_error("cmsDal_getAvailableIfForWanService failed.");
      return FALSE;
   }

   nl = ifList;
   while (!found && nl)
   {
      if (!cmsUtl_strncmp(l2Ifname, nl->name, strlen(l2Ifname)))
      {
         strcpy(descriptiveL2Ifname, nl->name);
         found = TRUE;
      }
      nl = nl->next;
   }

   cmsDal_freeNameList(ifList);
      
   
   cmsLog_debug("found=%d, l3ifname %s -> descriptiveL2IfName=%s",
                found, l3Ifname, descriptiveL2Ifname);
   
   return found;
}   
   



CmsRet dalWan_getWanConInfoForEdit_igd(WEB_NTWK_VAR *webVar)
{
   void *wanConnObj=NULL;
   UBOOL8 isPpp;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
   
      
   if (!dalWan_getDescriptiveL2IfnameForEdit(webVar->wanIfName, webVar->wanL2IfName))
   {
      cmsLog_error("Cannot get friendly L2ifname for %s.", webVar->wanIfName);
      return CMSRET_INTERNAL_ERROR;
   }

   if (CMSRET_SUCCESS != dalWan_getIpOrPppObjectByIfname(webVar->wanIfName, &iidStack, &wanConnObj, &isPpp))
   {
      cmsLog_error("cannot find obj for %s", webVar->wanIfName);
      return CMSRET_INTERNAL_ERROR;
   }

   if (!isPpp)
   {
      WanIpConnObject *ipConn = (WanIpConnObject *) wanConnObj;

      if (ipConn->name != NULL)
      {
         strcpy(webVar->serviceName, ipConn->name);
      }         
      webVar->enblNat = ipConn->NATEnabled;
      webVar->enblFullcone = ipConn->X_BROADCOM_COM_FullconeNATEnabled;
      webVar->enblFirewall = ipConn->X_BROADCOM_COM_FirewallEnabled;
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      webVar->enblIgmp = ipConn->X_BROADCOM_COM_IGMPEnabled;
      webVar->enblIgmpMcastSource = ipConn->X_BROADCOM_COM_IGMP_SOURCEEnabled;
#endif
      webVar->pcpMode = ipConn->X_BROADCOM_COM_PCPMode;      
      webVar->enblIpVer = 1 - (ipConn->X_BROADCOM_COM_IPv6Enabled ? 0 : 1) + (ipConn->X_BROADCOM_COM_IPv4Enabled ? 0 : 1);
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
      webVar->noMcastVlanFilter = ipConn->X_BROADCOM_COM_NoMcastVlanFilter;
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
      webVar->enblMld = ipConn->X_BROADCOM_COM_MLDEnabled;
      webVar->enblMldMcastSource= ipConn->X_BROADCOM_COM_MLD_SOURCEEnabled;
#endif

      /* if wanIpConn->externalIPAddress (webVar->wanIpAddress,) is "0.0.0.0", it is dynamic MER */
      if (cmsUtl_strcmp(ipConn->addressingType, MDMVS_DHCP) == 0)
      {
         webVar->enblDhcpClnt = TRUE;
         strcpy(webVar->wanIpAddress, "0.0.0.0");
      }
      else
      {
         webVar->enblDhcpClnt = FALSE;                  
         cmsUtl_strcpy(webVar->wanIntfGateway, ipConn->defaultGateway);
      }
   }
   else 
   {
      WanPppConnObject *pppConn = (WanPppConnObject *) wanConnObj;

      if (pppConn->username != NULL)
      {
         strcpy(webVar->pppUserName, pppConn->username);
      }
      if (pppConn->password != NULL)
      {
         strcpy(webVar->pppPassword, pppConn->password);
      }
      if (pppConn->name != NULL)
      {
         strcpy(webVar->serviceName, pppConn->name);
      }
      if (pppConn->PPPoEServiceName != NULL)
      {
         strcpy(webVar->pppServerName, pppConn->PPPoEServiceName);
      }
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      webVar->enblIgmp = pppConn->X_BROADCOM_COM_IGMPEnabled;
      webVar->enblIgmpMcastSource = pppConn->X_BROADCOM_COM_IGMP_SOURCEEnabled;
#endif
      webVar->enblIpVer = 1 - (pppConn->X_BROADCOM_COM_IPv6Enabled ? 0 : 1) + (pppConn->X_BROADCOM_COM_IPv4Enabled ? 0 : 1);
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
      webVar->noMcastVlanFilter = pppConn->X_BROADCOM_COM_NoMcastVlanFilter;
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
      webVar->enblMld = pppConn->X_BROADCOM_COM_MLDEnabled;
      webVar->enblMldMcastSource = pppConn->X_BROADCOM_COM_MLD_SOURCEEnabled;
#endif
      webVar->enblService = 1;  /* always enable service even though data model default is false */

      if (pppConn->X_BROADCOM_COM_IPExtension)
      {
        webVar->pppIpExtension = TRUE;
      }
      else
      {
         webVar->enblNat = pppConn->NATEnabled;
         webVar->enblFullcone = pppConn->X_BROADCOM_COM_FullconeNATEnabled;         
         webVar->enblFirewall = pppConn->X_BROADCOM_COM_FirewallEnabled;
         webVar->pcpMode = pppConn->X_BROADCOM_COM_PCPMode;         
      }
      
      webVar->pppToBridge = pppConn->X_BROADCOM_COM_AddPppToBridge;
      if (pppConn->X_BROADCOM_COM_LocalIPAddress != NULL)
      {
         strcpy(webVar->pppLocalIpAddress, pppConn->X_BROADCOM_COM_LocalIPAddress);
      }
      else
      {
         strcpy(webVar->pppLocalIpAddress, "0.0.0.0");
      }
      
      webVar->pppAuthMethod = cmsUtl_pppAuthToNum(pppConn->PPPAuthenticationProtocol);
      
      if (pppConn->idleDisconnectTime != 0)
      {
         webVar->enblOnDemand = TRUE;
         /* get on demand ideltime out (seconds in mdm) */
         webVar->pppTimeOut = pppConn->idleDisconnectTime / 60;
      }
      else
      {
         webVar->enblOnDemand = FALSE;
         webVar->pppTimeOut = 0;
      }

      if (pppConn->X_BROADCOM_COM_UseStaticIPAddress)
      {
         webVar->useStaticIpAddress = TRUE;
         strcpy(webVar->pppLocalIpAddress, pppConn->X_BROADCOM_COM_LocalIPAddress);
      }
      else
      {
         webVar->useStaticIpAddress = FALSE;
      }
      webVar->enblPppDebug = pppConn->X_BROADCOM_COM_Enable_Debug;
   }

   cmsObj_free(&wanConnObj);

   cmsLog_debug("Exit Edit. ret=%d", ret);

   return ret;

}


UBOOL8 dalWan_isEoAInterface(const InstanceIdStack *iidStack __attribute((unused)))
{
   UBOOL8 isEoAIntf = FALSE;

#ifdef DMP_X_BROADCOM_COM_ATMWAN_1
   if ((isEoAIntf = dalWan_isIPoA(iidStack))  == TRUE || 
       (isEoAIntf = dalWan_isPPPoA(iidStack)) == TRUE)
   {
      isEoAIntf = TRUE;
   }   
#endif /* DMP_X_BROADCOM_COM_ATMWAN_1 */

   return isEoAIntf;
}


CmsRet dalWan_addIPv4Service_igd(const WEB_NTWK_VAR *webVar)
{
   /* this function is only called in hybrid mode.
    * In hybrid mode, the caller just wants to create the legacy TR98 IPv4
    * objects for the new TR-181 based IPv6 objects he wants to create later.
    * That whole thing is already done in dalWan_addService_igd, so just
    * call that.
    */

   return (dalWan_addService_igd(webVar));
}


