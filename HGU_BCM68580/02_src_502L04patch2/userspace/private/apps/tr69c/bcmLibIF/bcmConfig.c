/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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
 



#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_phl.h"
#include "cms_util.h"
#include "cms_core.h"
#include "bcmConfig.h"
#include "bcmWrapper.h"

#include "main/informer_public.h"

extern ACSState acsState;
extern int g_TR069WANIPChanged;
extern UBOOL8 needDisconnect;

extern void changeNameSpaceCwmpVersionURL(int version);
extern void setsaveConfigFlag(UBOOL8 flagh); /* in RPCState.h */
extern void handleNotificationLimit(char *notificationLimitName, int notificationLimitValue, CmsEventHandler limitInformFunc);
extern void manageableDeviceNotificationLimitFunc(void *handle);

/*
 * These functions are called from files which are delivered as binary
 * only objects to customers, so the switching based on Data Model mode
 * must be done here instead of compiled in via header file.
 */
CmsRet getSessionRetryParameters(int *waitInterval, int *multiplier)
{
#if defined(SUPPORT_DM_LEGACY98)
    return (getSessionRetryParameters_igd(waitInterval, multiplier));
#elif defined(SUPPORT_DM_HYBRID)
    return (getSessionRetryParameters_igd(waitInterval, multiplier));
#elif defined(SUPPORT_DM_PURE181)
    return (getSessionRetryParameters_dev2(waitInterval, multiplier));
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
   {
     return (getSessionRetryParameters_dev2(waitInterval, multiplier));
   }
   else
   {
      return (getSessionRetryParameters_igd(waitInterval, multiplier));
   }
#endif
}


CmsRet setMgmtServerParameterKeyLocked(const char *paramKey)
{
#if defined(SUPPORT_DM_LEGACY98)
    return (setMgmtServerParameterKeyLocked_igd(paramKey));
#elif defined(SUPPORT_DM_HYBRID)
    return (setMgmtServerParameterKeyLocked_igd(paramKey));
#elif defined(SUPPORT_DM_PURE181)
    return (setMgmtServerParameterKeyLocked_dev2(paramKey));
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
   {
     return (setMgmtServerParameterKeyLocked_dev2(paramKey));
   }
   else
   {
      return (setMgmtServerParameterKeyLocked_igd(paramKey));
   }
#endif
}


/*
 * TR98 only functions.
 */
#ifdef DMP_BASELINE_1
/* this could be in qdm for LAN IP interface */
CmsRet getLanIPAddressInfo_igd(char **addr, char **subnetmask)
{
   LanIpIntfObject *lanIpCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   ret = cmsObj_getNext(MDMOID_LAN_IP_INTF, &iidStack, (void **) &lanIpCfg);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get LAN_IP_INTF object, ret=%d", ret);
   }
   else
   {
      if (!cmsUtl_strcmp(lanIpCfg->IPInterfaceAddressingType, MDMVS_STATIC) ||
          (!cmsUtl_strcmp(lanIpCfg->IPInterfaceAddressingType, MDMVS_DHCP) &&
           !cmsUtl_strcmp(lanIpCfg->X_BROADCOM_COM_DhcpConnectionStatus, MDMVS_CONNECTED)))
      {
         /* LAN IP Interface is UP, has IP addr and subnetmask */
         if (addr)
         {
            *addr = cmsMem_strdup(lanIpCfg->IPInterfaceIPAddress);
         }

         if (subnetmask)
         {
            *subnetmask = cmsMem_strdup(lanIpCfg->IPInterfaceSubnetMask);
         }
      }
      else
      {
         /* IP Interface not up, set addr and subnetmask to some known/zero value */
         if (addr)
         {
            *addr = cmsMem_strdup("0.0.0.0");
         }

         if (subnetmask)
         {
            *subnetmask = cmsMem_strdup("0.0.0.0");
         }
      }
      cmsObj_free((void **) &lanIpCfg);
   }
   return ret;
}


char *getFullPathToIpvxAddrLocked_igd(UINT32 ipvx, const char *ipAddr)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found = FALSE;
   WanIpConnObject *ipConnObj = NULL;
   WanPppConnObject *pppConnObj = NULL;
   LanIpIntfObject *lanIpIntfObj = NULL;
   MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
   char *fullPath=NULL;
   CmsRet ret = CMSRET_SUCCESS;
#if defined(OMCI_TR69_DUAL_STACK)
   BcmOmciRtdIpHostConfigDataObject *ipHost = NULL;
   struct in_addr inAddr;
#endif

   if (IS_EMPTY_STRING(ipAddr))
   {
      cmsLog_error("ipAddr is empty, just return");
      return NULL;
   }

   cmsLog_notice("Looking for fullpath to %s", ipAddr);

   /*
    * Handle special case of loopback addr first.  This is only used by
    * tr69c unittest code.  Since we don't create an object in the
    * Data Model for the loopback interface, just use the first LAN (br0)
    * interface.  The unittest code does not check if it points to a
    * correct location.  It is only looking for a string in the response.
    */
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      if (!cmsUtl_strcmp(ipAddr, "::1"))
      {
         /* Hybrid IPv6 using _dev2 path */
         fullPath = cmsMem_strdup("InternetGatewayDevice.Device.IP.Interface.1.IPv6Address.1");
      }
      else
      {
         fullPath = getFullPathToIpvxAddrLocked_dev2(CMS_AF_SELECT_IPV6, ipAddr);
      }

      if (fullPath)
      {
         return fullPath;
      }
   }
#endif

#ifdef DMP_X_BROADCOM_COM_UPNPIGDHTTPCONNREQ_1
   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      fullPath = cmsMem_strdup("InternetGatewayDevice.ManagementServer.X_BROADCOM_COM_UPNPC_IGD_WAN_ADDRESS");
      return fullPath;
   }
#endif

#ifdef DMP_X_BROADCOM_COM_IPV6_1
   /* deprecated legacy IPv6 */
   if ((ipvx & CMS_AF_SELECT_IPV6) &&
       !cmsUtl_strcmp(ipAddr, "::1"))
   {
      fullPath = cmsMem_strdup("InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.IPInterface.1.IPInterfaceIPAddress");
      return fullPath;
   }
#endif

   if ((ipvx & CMS_AF_SELECT_IPV4) &&
       !cmsUtl_strcmp(ipAddr, "127.0.0.1"))
   {
      fullPath = cmsMem_strdup("InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.IPInterface.1.IPInterfaceIPAddress");
      return fullPath;
   }


   /* XXX TODO: the OMCI related code has not been modified to handle ipvx.
    */
#if defined(OMCI_TR69_DUAL_STACK)
#ifdef SUPPORT_IPV6
   BcmOmciRtdIpv6HostConfigDataObject *ipv6Host = NULL;

   while (!found && 
          cmsObj_getNext(MDMOID_BCM_OMCI_RTD_IPV6_HOST_CONFIG_DATA,
                          &iidStack, (void **) &ipv6Host) == CMSRET_SUCCESS)
   {
      char ipv6Address[CMS_IPADDR_LENGTH];
      UINT32 size = 0;
      UINT8 *buf = NULL;

      memset(ipv6Address, 0, CMS_IPADDR_LENGTH);

      // convert ipv6Address hexString (32 bytes) to struct in6_addr (16 bytes) buf
      cmsUtl_hexStringToBinaryBuf(ipv6Host->currentAddressTable, &buf, &size);
      // struct in6_addr (16 bytes) buf to ipv6 string format (48 bytes)
      inet_ntop(AF_INET6, buf, ipv6Address, BUFLEN_48);
      // free temporary memory
      cmsMem_free(buf);

      if (cmsUtl_strcmp(ipv6Address, ipAddr) == 0)
      {
         found = TRUE;

         pathDesc.oid = MDMOID_BCM_OMCI_RTD_IPV6_HOST_CONFIG_DATA;
         pathDesc.iidStack = iidStack;
         sprintf(pathDesc.paramName, "CurrentAddressTable");
      }
      cmsObj_free((void **) &ipv6Host);
   }
   INIT_INSTANCE_ID_STACK(&iidStack);
#endif    // SUPPORT_IPV6
   while (!found && 
          cmsObj_getNext(MDMOID_BCM_OMCI_RTD_IP_HOST_CONFIG_DATA,
                          &iidStack, (void **) &ipHost) == CMSRET_SUCCESS)
   {
      inAddr.s_addr = ipHost->currentAddress;
      if (cmsUtl_strcmp(inet_ntoa(inAddr), ipAddr) == 0)
      {
         found = TRUE;

         pathDesc.oid = MDMOID_BCM_OMCI_RTD_IP_HOST_CONFIG_DATA;
         pathDesc.iidStack = iidStack;
         sprintf(pathDesc.paramName, "CurrentAddress");
      }
      cmsObj_free((void **) &ipHost);
   }
   INIT_INSTANCE_ID_STACK(&iidStack);
#endif  /* OMCI_TR69_DUAL_STACK */


   while (!found &&
          (ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConnObj)) == CMSRET_SUCCESS)
   {

#ifdef DMP_X_BROADCOM_COM_IPV6_1
      /* check IPv6 first */
      /* deprecated legacy IPv6 */
      if ((ipvx & CMS_AF_SELECT_IPV6) &&
          cmsUtl_strstr(ipConnObj->X_BROADCOM_COM_ExternalIPv6Address, ipAddr))
      {
         found = TRUE;

         pathDesc.oid = MDMOID_WAN_IP_CONN;
         pathDesc.iidStack = iidStack;
         sprintf(pathDesc.paramName, "X_BROADCOM_COM_ExternalIPv6Address");
      }
#endif

      if (!found &&
          (ipvx & CMS_AF_SELECT_IPV4) &&
          cmsUtl_strcmp(ipConnObj->externalIPAddress, ipAddr) == 0)
      {
         found = TRUE;

         pathDesc.oid = MDMOID_WAN_IP_CONN;
         pathDesc.iidStack = iidStack;
         sprintf(pathDesc.paramName, "ExternalIPAddress");
      }

      cmsObj_free((void **) &ipConnObj);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConnObj)) == CMSRET_SUCCESS)
   {

#ifdef DMP_X_BROADCOM_COM_IPV6_1
      /* check IPv6 first */
      /* deprecated legacy IPv6 */
      if ((ipvx & CMS_AF_SELECT_IPV6) &&
          cmsUtl_strstr(pppConnObj->X_BROADCOM_COM_ExternalIPv6Address, ipAddr))
      {
         found = TRUE;

         pathDesc.oid = MDMOID_WAN_PPP_CONN;
         pathDesc.iidStack = iidStack;
         sprintf(pathDesc.paramName, "X_BROADCOM_COM_ExternalIPv6Address");
      }
#endif

      if (!found &&
          (ipvx & CMS_AF_SELECT_IPV4) &&
          (cmsUtl_strcmp(pppConnObj->externalIPAddress, ipAddr) == 0))
      {
         found = TRUE;

         pathDesc.oid = MDMOID_WAN_PPP_CONN;
         pathDesc.iidStack = iidStack;
         sprintf(pathDesc.paramName, "ExternalIPAddress");
      }

      cmsObj_free((void **) &pppConnObj);
   }


   INIT_INSTANCE_ID_STACK(&iidStack);
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_LAN_IP_INTF, &iidStack, (void **) &lanIpIntfObj)) == CMSRET_SUCCESS)
   {
      /* what about IPv6 on LAN side? */

      if ((ipvx & CMS_AF_SELECT_IPV4) &&
           cmsUtl_strcmp(lanIpIntfObj->IPInterfaceIPAddress, ipAddr) == 0)
      {
         found = TRUE;

         pathDesc.oid = MDMOID_LAN_IP_INTF;
         pathDesc.iidStack = iidStack;
         sprintf(pathDesc.paramName, "IPInterfaceIPAddress");
      }
      cmsObj_free((void **) &lanIpIntfObj);
   }


   if (found)
   {
      ret = cmsMdm_pathDescriptorToFullPath(&pathDesc, &fullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("could not convert pathDesc to fullPath");
      }
      else
      {
         cmsLog_debug("mapped %s to %s", ipAddr, fullPath);
      }
   }
   else
   {
      cmsLog_error("Could not find %s in data model", ipAddr);
   }

   return fullPath;
}

void clearModemConnectionURL_igd(void)
{
   CmsRet ret;
   ManagementServerObject *obj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   if ((ret = cmsLck_acquireLockWithTimeout(TR69C_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void **) &obj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get management server object!");
   }
   else 
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(obj->connectionRequestURL);

      if ((ret = cmsObj_setFlags(obj, &iidStack, OSF_NO_RCL_CALLBACK|OSF_NO_ACCESSPERM_CHECK)) != CMSRET_SUCCESS)
      {
         cmsLog_error("set of connectionRequestURL to NULL failed, ret=%d", ret);
      }

      cmsObj_free((void **) &obj);
   }
   cmsLck_releaseLock();
}

UBOOL8 isTimeSynchronized_igd(void)
{
   UBOOL8 synchronized = FALSE;

#ifdef DMP_TIME_1
   TimeServerCfgObject *timeObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsLck_acquireLockWithTimeout(TR69C_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return synchronized;
   }

   if ((ret = cmsObj_get(MDMOID_TIME_SERVER_CFG, &iidStack, 0, (void **) &timeObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TIME_SERVER_CFG object failed, ret=%d", ret);
   }
   else
   {
      if (timeObj->enable == TRUE &&
	  cmsUtl_strcmp(timeObj->status, MDMVS_SYNCHRONIZED) == 0)
      {
         synchronized = TRUE;
      }

      cmsObj_free((void **) &timeObj);
   }

   cmsLck_releaseLock();
#endif /* DMP_TIME_1 */
   return synchronized;
}

/* Build a list of parameters that need to be set defaultActiveNotification */
void setDefaultActiveNotificationLocked_igd(PhlSetParamAttr_t **pSetParmAttrList, int *numEntries)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject *ipConnObj = NULL;
   WanPppConnObject *pppConnObj = NULL;
   PhlSetParamAttr_t *pList, *pSetParamAttr;
   int num = 1;   /* ConnectionRequestURL counted */


   /* There are 2 parameters that needs to be forced to Active Notification
    * TR98 specification, Table 5, Default Active Notification parameters.
    * IGD.ManagementServer.ConnectionRequestURL
    * IGD.WANDevice{i}.WanConnectionDevice.{j}.WAN***Connection.{k}.ExternalIPAddress.
    */
   while (cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConnObj) == CMSRET_SUCCESS)
   {
      num++;
      cmsObj_free((void **) &ipConnObj);
   }
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConnObj) == CMSRET_SUCCESS)
   {
      num++;
      cmsObj_free((void **) &pppConnObj);      
   }
   
   pList = cmsMem_alloc(num * sizeof(PhlSetParamAttr_t),ALLOC_ZEROIZE);
   if (pList == NULL)
   {
      cmsLck_releaseLock();
      return;
   }
   *numEntries = num;
   pSetParamAttr = pList;

   /* first, ConnectionRequestURL */
   cmsMdm_fullPathToPathDescriptor("InternetGatewayDevice.ManagementServer.ConnectionRequestURL",
                                   &(pSetParamAttr->pathDesc));
   pSetParamAttr->attributes.notificationChange = 1;
   pSetParamAttr->attributes.notification = NDA_TR69_ACTIVE_NOTIFICATION;
   pSetParamAttr->attributes.accessBitMask = NDA_ACCESS_TR69C;
   pSetParamAttr++;

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConnObj)) == CMSRET_SUCCESS)
   {
      pSetParamAttr->pathDesc.oid = MDMOID_WAN_IP_CONN;
      pSetParamAttr->pathDesc.iidStack = iidStack;
      sprintf(pSetParamAttr->pathDesc.paramName, "ExternalIPAddress");
      pSetParamAttr->attributes.notificationChange = 1;
      pSetParamAttr->attributes.notification = NDA_TR69_ACTIVE_NOTIFICATION;
      pSetParamAttr->attributes.accessBitMask = NDA_ACCESS_TR69C;
      pSetParamAttr++;
      cmsObj_free((void **) &ipConnObj);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConnObj)) == CMSRET_SUCCESS)
   {
      pSetParamAttr->pathDesc.oid = MDMOID_WAN_PPP_CONN;
      pSetParamAttr->pathDesc.iidStack = iidStack;
      sprintf(pSetParamAttr->pathDesc.paramName, "ExternalIPAddress");
      pSetParamAttr->attributes.notificationChange = 1;
      pSetParamAttr->attributes.notification = NDA_TR69_ACTIVE_NOTIFICATION;
      pSetParamAttr->attributes.accessBitMask = NDA_ACCESS_TR69C;
      pSetParamAttr++;
      cmsObj_free((void **) &pppConnObj);
   }
   *pSetParmAttrList = pList;
}

UBOOL8 checkWanDslDiagnosticObjID_igd(MdmPathDescriptor *pathDesc)
{
   if (pathDesc->oid == MDMOID_WAN_DSL_DIAG)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

CmsRet getSessionRetryParameters_igd(int *waitInterval, int *multiplier)
{
   ManagementServerObject *obj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsLck_acquireLockWithTimeout(TR69C_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      return (CMSRET_INTERNAL_ERROR);
   }

   ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void **) &obj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get management server object!");
   }
   else 
   {
      *waitInterval = obj->CWMPRetryMinimumWaitInterval;
      *multiplier = obj->CWMPRetryIntervalMultiplier;
      cmsObj_free((void **) &obj);
   }
   cmsLck_releaseLock();
   return (ret);
}

CmsRet setMgmtServerParameterKeyLocked_igd(const char *paramKey)
{
   ManagementServerObject *obj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void **) &obj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get management server object!");
      return ret;
   }

   CMSMEM_REPLACE_STRING(obj->parameterKey, paramKey);

   if ((ret = cmsObj_setFlags(obj, &iidStack, OSF_NO_RCL_CALLBACK|OSF_NO_ACCESSPERM_CHECK)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of paramKey %s failed, ret=%d", paramKey, ret);
   }

   cmsObj_free((void **) &obj);
   return (ret);
}


#endif /* (DMP_BASELINE_1) */
