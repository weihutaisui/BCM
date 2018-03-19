/* 
* <:copyright-BRCM:2013:proprietary:standard
* 
*    Copyright (c) 2013 Broadcom 
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
* :>
*/

#ifdef DMP_DEVICE2_BASELINE_1

#include "cms.h"
#include "cms_util.h"
#include "prctl.h"
#include "cms_core.h"
#include "ssk.h"
#if defined (DMP_TIME_1) || defined (DMP_DEVICE2_TIME_1)
#include "ssk_time.h"
#endif


/*!\file ssk2_connstatus.c
 *
 * This file deals with status of ppp client, dhcp client, service state
 * machines for IPv4.  This is the TR181 equivalent of connstatus.c
 */


static UBOOL8 getDhcpClientObjByPid(UINT32 pid,
                                 InstanceIdStack *dhcpClientIidStack,
                                 Dev2Dhcpv4ClientObject **dhcpClientObj);
static UBOOL8 getPppIntfByPid(UINT32 pid,
                       char **pppIntfFullPath,
                       InstanceIdStack *pppIntfIidStack,
                       Dev2PppInterfaceObject **pppIntfObj);
static void fillInPppIpcpObject(const InstanceIdStack *pppIntfIidStack,
                      const char *localIPAddress,
                      const char *remoteIPAddress,
                      const char *DNSServers);
static void blankOutPppIpcpObject(const InstanceIdStack *pppIntfIidStack);



void processGetWanConnStatus_dev2(CmsMsgHeader *msg)
{
   char *queryIfName = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   UBOOL8 found=FALSE;
   UBOOL8 status=FALSE;
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d, respond with status down", ret);
      return;
   }

   if (msg->dataLength > 0)
   {
      /*
       * Sender is asking about a specific WAN interface.
       * Otherwise, queryIfName is NULL, which means is any WAN interface up.
       */
      queryIfName = (char *) (msg + 1);
      cmsLog_debug("queryIfName=%s", queryIfName);
   }
   else
   {
      cmsLog_debug("no ifName specified by caller");
   }


   while (!found &&
          ((ret = cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                                  OGF_NO_VALUE_UPDATE,
                                  (void **) &ipIntfObj)) == CMSRET_SUCCESS))
   {
      if (ipIntfObj->X_BROADCOM_COM_Upstream)
      {
         if ((queryIfName != NULL) &&
             (cmsUtl_strcmp(ipIntfObj->name, queryIfName) == 0))
         {
            /* sender is asking about a specific WAN connection */
            found = TRUE;
            if (!cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv4ServiceStatus,
                                                         MDMVS_SERVICEUP))
            {
               status = TRUE;
            }
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
            if (!cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv6ServiceStatus,
                                                         MDMVS_SERVICEUP))
            {
               status = TRUE;
            }
#endif
         }
         else if (queryIfName == NULL)
         {
            /* sender is asking for any WAN connection up, and we found one */
            if (!cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv4ServiceStatus,
                                                         MDMVS_SERVICEUP))
            {
               found = TRUE;
               status = TRUE;
            }
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
            if (!cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv6ServiceStatus,
                                                         MDMVS_SERVICEUP))
            {
               found = TRUE;
               status = TRUE;
            }
#endif
         }
      }

      cmsObj_free((void **)&ipIntfObj);
   }

   cmsLck_releaseLock();

   /* reuse the msg to send the reply */
   msg->dst = msg->src;
   msg->src = EID_SSK;
   msg->flags_request = 0;
   msg->flags_response = 1;
   msg->wordData = status;

   cmsLog_debug("wan conn ifName=%s, dstEid=%d, status=%d",
                 queryIfName, msg->dst, status);

   if ((ret = cmsMsg_send(msgHandle, msg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg send failed, ret=%d",ret);
   }

   return;
}


void processDhcpcStateChanged_dev2(CmsMsgHeader *msg)
{
   DhcpcStateChangedMsgBody *dhcpcInfo = (DhcpcStateChangedMsgBody *) (msg + 1);
   Dev2Dhcpv4ClientObject *dhcpClientObj=NULL;
   InstanceIdStack dhcpClientIidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   InstanceIdStack ipIntfIidStack=EMPTY_INSTANCE_ID_STACK;
   UINT32 pid;
   UBOOL8 found=FALSE;
   CmsRet ret;

   pid = PID_IN_EID(msg->src);

   cmsLog_debug("dhcpc pid=%d dataLength=%d addressAssigned=%d expired=%d", pid, msg->dataLength, dhcpcInfo->addressAssigned, dhcpcInfo->isExpired);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d, dhcp client status update dropped!", ret);
      /* hmm, this is bad, I can't update the MDM with new dhcpc info */
      return;
   }

   /* find the right dhcpv4 client object */
   found = getDhcpClientObjByPid(pid, &dhcpClientIidStack, &dhcpClientObj);
   if (!found)
   {
      cmsLog_error("Could not find dhcpv4 client object with pid=%d", pid);
      cmsLck_releaseLock();
      return;
   }

   /* Get the IP.Interface object which this dhcp client runs on */
   if ((ret = getIpIntfByFullPath(dhcpClientObj->interface,
                           &ipIntfIidStack, &ipIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get IP.Interface from dhcp client object!");
      cmsObj_free((void **) &dhcpClientObj);
      cmsLck_releaseLock();
      return;
   }

   if (dhcpcInfo->addressAssigned)
   {
      if (!cmsUtl_strcmp(dhcpClientObj->DHCPStatus, MDMVS_BOUND))
      {
         /*
          * We already have an address but now we get another msg from
          * DHCP client saying we got address.  Address or some other
          * "important" param may have changed.  If so, we need to:
          * set IPv4 service status to SERVICESTARTING
          * modify contents of dhcpClient object.
          * continue on to the code below.
          */
         if (cmsUtl_strcmp(dhcpClientObj->IPAddress, dhcpcInfo->ip) ||
             cmsUtl_strcmp(dhcpClientObj->subnetMask, dhcpcInfo->mask) ||
             cmsUtl_strcmp(dhcpClientObj->IPRouters, dhcpcInfo->gateway) ||
             cmsUtl_strcmp(dhcpClientObj->DNSServers, dhcpcInfo->nameserver))
         {
            /*
             * Update IP.Interface.Status before IPv4ServiceStatus,
             * but IP.Interface.Status needs to consider the new
             * IPv4ServiceStatus.
             */
            intfStack_updateIpIntfStatusLocked(&ipIntfIidStack,
                                               MDMVS_SERVICESTARTING, NULL);
            setIpv4ServiceStatusByFullPathLocked(dhcpClientObj->interface,
                                                 MDMVS_SERVICESTARTING);

            CMSMEM_REPLACE_STRING(dhcpClientObj->DHCPStatus, MDMVS_RENEWING);
            if ((ret = cmsObj_set(dhcpClientObj, &dhcpClientIidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("set of dhcpv4clientObj to RENEWING failed, ret=%d", ret);
            }
         }
      }

      cmsLog_debug("Assigned ip=%s netmask=%s gateway=%s nameserver=%s",
                   dhcpcInfo->ip,
                   dhcpcInfo->mask,
                   dhcpcInfo->gateway,
                   dhcpcInfo->nameserver);


      /*
       * set the standard DHCP client stuff.
       * The RCL handler function for the dhcpClientObj will create and fill
       * in the IPv4AddressObject, the DNS.Client.Server object, and
       * IPv4ForwardingObject.
       */
      CMSMEM_REPLACE_STRING(dhcpClientObj->IPAddress, dhcpcInfo->ip);
      CMSMEM_REPLACE_STRING(dhcpClientObj->subnetMask, dhcpcInfo->mask);
      CMSMEM_REPLACE_STRING(dhcpClientObj->IPRouters, dhcpcInfo->gateway);
      CMSMEM_REPLACE_STRING(dhcpClientObj->DNSServers, dhcpcInfo->nameserver);
      CMSMEM_REPLACE_STRING(dhcpClientObj->DHCPStatus, MDMVS_BOUND);
      if ((ret = cmsObj_set(dhcpClientObj, &dhcpClientIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("set of dhcpv4clientObj failed, ret=%d", ret);
      }

      /*
       * Since DHCP client is not part of the interface stack, it cannot
       * use propagateStatus to cause the interface stack to set the state.
       * So do it directly here.
       */
      intfStack_setStatusByFullPathLocked(dhcpClientObj->interface, MDMVS_UP);
      setIpv4ServiceStatusByFullPathLocked(dhcpClientObj->interface, MDMVS_SERVICEUP);


      /*
       * These other services rely on the layer 3 service, so do them after
       * the layer 3 is fully up.
       */

      /* XXX set 6rd tunnel object.  Do we still need to do this in TR181?
       * Should be after layer 3 or before?
       */

#if defined (DMP_TIME_1) || defined (DMP_DEVICE2_TIME_1)
      /* dhcp client msg may contain NTP server info */
      if (strlen(dhcpcInfo->ntpserver) > 0)
      {
         cmsLog_debug("Got ntpserver(s)=%s", dhcpcInfo->ntpserver);
         setNtpServers(dhcpcInfo->ntpserver);
      }
#endif /* DMP_TIME_1 or DMP_DEVICE2_TIME_1 */

#ifdef SUPPORT_TR69C 
      /* dhcp client msg may contain TR69 mgmt server params */
      if (strlen(dhcpcInfo->acsURL) > 0 ||
          strlen(dhcpcInfo->acsProvisioningCode) > 0 ||
          dhcpcInfo->cwmpRetryMinimumWaitInterval != 0 ||
          dhcpcInfo->cwmpRetryIntervalMultiplier != 0)
      {
         cmsLog_debug("got acsURL=%s provisioningCode=%s MinWaitInterval=%d intMult=%d",
                      dhcpcInfo->acsURL,
                      dhcpcInfo->acsProvisioningCode,
                      dhcpcInfo->cwmpRetryMinimumWaitInterval,
                      dhcpcInfo->cwmpRetryIntervalMultiplier);
         setAcsParams(dhcpcInfo->acsURL, dhcpcInfo->acsProvisioningCode,
                      dhcpcInfo->cwmpRetryMinimumWaitInterval,
                      dhcpcInfo->cwmpRetryIntervalMultiplier);
      }
#endif /* SUPPORT_TR69C */ 
   }
   else
   {
      /* DHCP client could not renew lease */

      /* NULL out the info we stored in the dhcp client object */
      CMSMEM_FREE_BUF_AND_NULL_PTR(dhcpClientObj->IPAddress);
      CMSMEM_FREE_BUF_AND_NULL_PTR(dhcpClientObj->subnetMask);
      CMSMEM_FREE_BUF_AND_NULL_PTR(dhcpClientObj->IPRouters);
      CMSMEM_FREE_BUF_AND_NULL_PTR(dhcpClientObj->DNSServers);
      CMSMEM_REPLACE_STRING(dhcpClientObj->DHCPStatus, MDMVS_REQUESTING);
      if ((ret = cmsObj_set(dhcpClientObj, &dhcpClientIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("set of dhcpv4clientObj failed, ret=%d", ret);
      }

      /*
       * Update IP.Interface.Status before IPv4ServiceStatus,
       * but IP.Interface.Status needs to consider the new
       * IPv4ServiceStatus.
       */
      intfStack_updateIpIntfStatusLocked(&ipIntfIidStack,
                                         MDMVS_SERVICESTARTING, NULL);
     /*
      * When we go from SERVICEUP to SERVICESTARTING, the dynamically created
      * IPv4Address object will be deleted in the teardown step.
      */
      setIpv4ServiceStatusByFullPathLocked(dhcpClientObj->interface,
                                           MDMVS_SERVICESTARTING);
   }

   cmsLck_releaseLock();

   cmsObj_free((void **) &ipIntfObj);
   cmsObj_free((void **) &dhcpClientObj);

   return;
}


#ifdef DMP_DEVICE2_DHCPV4CLIENT_1
void processDhcpcReqOptionReport_dev2(CmsMsgHeader *msg)
{

   char buf[BUFLEN_16];
   UINT32 pid = 0;
   DhcpcReqOptionReportMsgBody *reqOptionBody = (DhcpcReqOptionReportMsgBody *) (msg + 1);
   Dev2Dhcpv4ClientObject *dhcpClientObj = NULL;
   InstanceIdStack dhcpClientIidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Dhcpv4ClientReqOptionObject *reqOptionObj = NULL;
   InstanceIdStack reqOptionIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   pid = PID_IN_EID(msg->src);

   cmsLog_debug("dhcpc pid=%d, leasedTime=%d, clientAddr=%s, serverAddr=%s",
      pid, reqOptionBody->leasedTime, reqOptionBody->clientAddress, reqOptionBody->serverAddress);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d, dhcp client report request option dropped!", ret);
      return;
   }

   /* find the right dhcpv4 client object */
   if (getDhcpClientObjByPid(pid, &dhcpClientIidStack, &dhcpClientObj) == FALSE)
   {
      cmsLog_error("Could not find dhcpv4 client object with pid=%d", pid);
      cmsLck_releaseLock();
      return;
   }

   /* just need dhcpv4 client iidStack, so free dhcpv4 client object */
   cmsObj_free((void **) &dhcpClientObj);

   /* Loop through Dev2Dhcpv4ClientReqOptionObject
    * that has this dhcp client as its parent */
   while (cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_DHCPV4_CLIENT_REQ_OPTION,
                                       &dhcpClientIidStack, 
                                       &reqOptionIidStack, 
                                       OGF_NO_VALUE_UPDATE,
                                       (void **)&reqOptionObj) == CMSRET_SUCCESS)
   {
      if (reqOptionObj->enable == TRUE)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(reqOptionObj->value);

         switch (reqOptionObj->tag)
         {
            case 50:
               cmsUtl_binaryBufToHexString((const UINT8 *)reqOptionBody->clientAddress,
                                           cmsUtl_strlen((const char *)reqOptionBody->clientAddress),
                                           &(reqOptionObj->value));
               break;
            case 51:
               memset(buf, 0, BUFLEN_16);
               snprintf(buf, BUFLEN_16, "%d", reqOptionBody->leasedTime);
               cmsUtl_binaryBufToHexString((const UINT8 *)buf,
                                           cmsUtl_strlen((const char *)buf),
                                           &(reqOptionObj->value));
               break;
            case 54:
               cmsUtl_binaryBufToHexString((const UINT8 *)reqOptionBody->serverAddress,
                                           cmsUtl_strlen((const char *)reqOptionBody->serverAddress),
                                           &(reqOptionObj->value));
               CMSMEM_REPLACE_STRING(reqOptionObj->X_BROADCOM_COM_Value, reqOptionBody->serverAddress);
               break;
         }

         ret = cmsObj_set(reqOptionObj, &reqOptionIidStack);

         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set Dev2Dhcpv4ClientReqOptionObject tag=%d. ret=%d",
                         reqOptionObj->tag, ret);
         }     
      }

      cmsObj_free((void **)&reqOptionObj);
   }     

   cmsLck_releaseLock();

}
#else
void processDhcpcReqOptionReport_dev2(CmsMsgHeader *msg  __attribute__((unused)))
{
}
#endif   /* DMP_DEVICE2_DHCPV4CLIENT_1 */


void processPppStateChanged_dev2(CmsMsgHeader *msg)
{
   PppoeStateChangeMsgBody *pppInfo = (PppoeStateChangeMsgBody *) (msg + 1);
   InstanceIdStack pppIntfIidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2PppInterfaceObject *pppIntfObj=NULL;
   char *pppIntfFullPath=NULL;
   UINT32 pid;
   UBOOL8 found=FALSE;
   CmsRet ret;

   pid = PID_IN_EID(msg->src);

   cmsLog_debug("ppp pid=%d dataLength=%d state=%d, lastConnectionError=%s",
      pid, msg->dataLength, pppInfo->pppState, pppInfo->ppplastconnecterror);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d, ppp connection state update dropped!", ret);
      /* hmm, this is bad, I can't update the MDM with new pppd info */
      return;
   }

   /* Get the PPP.Interface object */
   found = getPppIntfByPid(pid, &pppIntfFullPath, &pppIntfIidStack, &pppIntfObj);
   if (!found)
   {
      cmsLog_error("Could not find PPP.Interface object with pid=%d", pid);
      cmsLck_releaseLock();
      return;
   }
   
   cmsLog_debug("found %d, ppp state %d", found, pppInfo->pppState);

   switch(pppInfo->pppState)
   {

   case BCM_PPPOE_CLIENT_STATE_PADO:
   case BCM_PPPOE_CLIENT_STATE_PADS:
   case BCM_PPPOE_CLIENT_STATE_CONFIRMED:
   case BCM_PPPOE_SERVICE_AVAILABLE:
      cmsLog_debug("waiting for ppp to come up; current state=%d", pppInfo->pppState);
      break;



   case BCM_PPPOE_CLIENT_STATE_UP:
     cmsLog_debug("assigned ip=%s gateway=%s nameserver=%s lastConnectionError=%s servicename=%s",
                        pppInfo->ip,
                        pppInfo->gateway,
                        pppInfo->nameserver,
                        pppInfo->ppplastconnecterror,
                        pppInfo->servicename);

     /*
      * Free up as memory as possible before we go into the RCL, which will
      * do lots of insmod and iptables actions.  We need to do this on low
      * memory systems to prevent thrashing of memory.
      */
      if (isLowMemorySystem)
      {
         prctl_runCommandInShellBlocking("echo 3 > /proc/sys/vm/drop_caches");
      }

      /* Record the info from ppp server in PPP.Interface.{i}.IPCP obj */
      fillInPppIpcpObject(&pppIntfIidStack, pppInfo->ip, pppInfo->gateway,
                          pppInfo->nameserver);

      /* IPv4AddressObject is created by pppIntf object where, it calls the same functions as IPoE
      * to set ip interface's  IPv4Address, DNS server and gateway with the information from
      * ipcp object in the previous function addPppIpcpObject
      */
      sskConn_setPppConnStatusByObjLocked(&pppIntfIidStack, pppIntfObj, MDMVS_CONNECTED);

      /* Set PPP interface Status to UP and propagate */
      intfStack_setStatusByFullPathLocked(pppIntfFullPath, MDMVS_UP);
      intfStack_propagateStatusByFullPathLocked(pppIntfFullPath, MDMVS_UP);

      break;

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   case BCM_PPPOE_CLIENT_IPV6_STATE_UP:
      cmsLog_debug("IPv6CP is UP!");

      /*
       * Free up as memory as possible before we go into the RCL, which will
       * do lots of insmod and iptables actions.  We need to do this on low
       * memory systems to prevent thrashing of memory.
       */
      if (isLowMemorySystem)
      {
         prctl_runCommandInShellBlocking("echo 3 > /proc/sys/vm/drop_caches");
      }

      /* A single ppp daemon can do ipv4 and ipv6 negotiation at the
       * same time.  If we get this message, call out to ppp handler function
       * in v6 file.
       */
      processPppv6StateUp_dev2(&pppIntfIidStack, pppInfo->localIntfId, pppInfo->remoteIntfId);
      break;

   case BCM_PPPOE_CLIENT_IPV6_STATE_DOWN:
      cmsLog_debug("IPv6CP is down!");
      /* A single ppp daemon can do ipv4 and ipv6 negotiation at the
       * same time.  If we get this message, call out to ppp handler function
       * in v6 file.
       */
      processPppv6StateDown_dev2(&pppIntfIidStack);
      break;
#endif /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */

   case BCM_PPPOE_CLIENT_STATE_DOWN:

      /* Delete the now-invalid info in the PPP.Interface.{i}.IPCP. */
      blankOutPppIpcpObject(&pppIntfIidStack);

      sskConn_setPppConnStatusByObjLocked(&pppIntfIidStack, pppIntfObj, MDMVS_DISCONNECTED);

      /*
       * If PPP goes down, we assume it is because the lower layer link is
       * down.  So we let the LOWERLAYERDOWN propagate up to the IP.Interface
       * object, where it will set the service state machines to SERVICEDOWN
       * When we go to SERVICEDOWN state, the dynamically created
       * IPv4Address object will be deleted in the teardown step.
       */
      intfStack_setStatusByFullPathLocked(pppIntfFullPath, MDMVS_LOWERLAYERDOWN);
      intfStack_propagateStatusByFullPathLocked(pppIntfFullPath, MDMVS_LOWERLAYERDOWN);
      break;

   case BCM_PPPOE_AUTH_FAILED:
     /* Set the lastConnectionError field first, then call the helper
      * function sskConn_setPppConnStatus to do the set.  The set will
      * set the main connectionStatus param as well as our lastConnectionError
      * param.
      */
      CMSMEM_REPLACE_STRING(pppIntfObj->lastConnectionError, MDMVS_ERROR_AUTHENTICATION_FAILURE);
      sskConn_setPppConnStatusByObjLocked(&pppIntfIidStack, pppIntfObj, MDMVS_DISCONNECTED);
      break;

   /*
   * Just update the lastConnectionError field - used by tr69 and ppp on demand.
   */
   case BCM_PPPOE_REPORT_LASTCONNECTERROR:

      CMSMEM_REPLACE_STRING(pppIntfObj->lastConnectionError, pppInfo->ppplastconnecterror);
      if ((ret = cmsObj_set(pppIntfObj, &pppIntfIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not set pppIntf lastConnectionError, ret=%d", ret);
      }
      break;

   default:
      cmsLog_error("Unsupported state=%d", pppInfo->pppState);
      break;
   }

   cmsLck_releaseLock();

   CMSMEM_FREE_BUF_AND_NULL_PTR(pppIntfFullPath);
   cmsObj_free((void **) &pppIntfObj);

   return;
}


void setIpv4ServiceStatusByFullPathLocked(const char *ipIntfFullPath, const char *serviceStatus)
{
   MdmPathDescriptor pathDesc;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(ipIntfFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                   ipIntfFullPath, ret);
      return;
   }

   if (pathDesc.oid != MDMOID_DEV2_IP_INTERFACE)
   {
      cmsLog_error("fullPath (%s) must point to IP.Interface", ipIntfFullPath);
      return;
   }

   setIpv4ServiceStatusByIidLocked(&pathDesc.iidStack, serviceStatus);

   return;
}


void setIpv4ServiceStatusByIidLocked(const InstanceIdStack *ipIntfIidStack, const char *serviceStatus)
{
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_DEV2_IP_INTERFACE, ipIntfIidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **)&ipIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get IP.Interface object, ret=%d", ret);
      return;
   }

   if (ipIntfObj->IPv4Enable)
   {
      CMSMEM_REPLACE_STRING(ipIntfObj->X_BROADCOM_COM_IPv4ServiceStatus, serviceStatus);

      if ((ret = cmsObj_set(ipIntfObj, ipIntfIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Set of IP.Interface object failed, ret=%d", ret);
      }
   }

   cmsObj_free((void **) &ipIntfObj);

   return;
}


void getIpv4ServiceStatusByIidLocked(const InstanceIdStack *ipIntfIidStack,
                                     char *serviceStatus, UINT32 bufLen)
{
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_DEV2_IP_INTERFACE, ipIntfIidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **)&ipIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_debug("Could not get IP.Interface object at %s, ret=%d",
                   cmsMdm_dumpIidStack(ipIntfIidStack), ret);
      /* For intf grouping, the entire IP.Interface subtree could get
       * deleted, so we cannot see the IP.Interface status.  Just report a
       * DOWN status.
       */
      cmsUtl_strncpy(serviceStatus, MDMVS_DOWN, bufLen);
      return;
   }

   if (cmsUtl_strlen(ipIntfObj->X_BROADCOM_COM_IPv4ServiceStatus) >= (SINT32) bufLen)
   {
      cmsLog_error("status %s cannot fit into bufLen %d",
            ipIntfObj->X_BROADCOM_COM_IPv4ServiceStatus, bufLen);
   }
   else
   {
      strcpy(serviceStatus, ipIntfObj->X_BROADCOM_COM_IPv4ServiceStatus);
   }

   cmsObj_free((void **) &ipIntfObj);

   return;
}


UBOOL8 getDhcpClientObjByPid(UINT32 pid,
                          InstanceIdStack *dhcpClientIidStack,
                          Dev2Dhcpv4ClientObject **dhcpClientObj)
{
   UBOOL8 found = FALSE;
   CmsRet ret;

   while (!found &&
          ((ret = cmsObj_getNext(MDMOID_DEV2_DHCPV4_CLIENT,
                                 dhcpClientIidStack,
                                 (void **) dhcpClientObj)) == CMSRET_SUCCESS))
   {
      if ((*dhcpClientObj)->X_BROADCOM_COM_Pid == pid)
      {
         /* found it, do not free it */
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) dhcpClientObj);
      }
   }

   return found;
}


CmsRet getIpIntfByFullPath(const char *ipIntfFullPath,
                           InstanceIdStack *ipIntfIidStack,
                           Dev2IpInterfaceObject **ipIntfObj)
{
   MdmPathDescriptor pathDesc;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(ipIntfFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                   ipIntfFullPath, ret);
      return ret;
   }

   if (pathDesc.oid != MDMOID_DEV2_IP_INTERFACE)
   {
      cmsLog_error("fullPath (%s) must point to IP.Interface", ipIntfFullPath);
      return CMSRET_INVALID_PARAM_NAME;
   }

   *ipIntfIidStack = pathDesc.iidStack;

   if ((ret = cmsObj_get(MDMOID_DEV2_IP_INTERFACE, ipIntfIidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **) ipIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get IP.Interface object, ret=%d", ret);
      return ret;
   }

   return ret;
}

#ifdef SUPPORT_TR69C
void setAcsParams_dev2(const char *acsURL, const char *provisioningCode,
                  UINT32 minWaitInterval, UINT32 intervalMultiplier)
{
   Dev2ManagementServerObject *mgmtObj=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 doSet=FALSE;
   CmsRet ret;


   if (CMSRET_SUCCESS != (ret = cmsObj_get(MDMOID_DEV2_MANAGEMENT_SERVER,
                                    &iidStack, 0, (void **) &mgmtObj)))
   {
      cmsLog_error("Could not get DEV2_MANAGMENT_SERVER obj, ret=%d", ret);
      return;
   }

   if (cmsUtl_strlen(acsURL) > 0 && cmsUtl_strcmp(mgmtObj->URL, acsURL))
   {
      CMSMEM_REPLACE_STRING(mgmtObj->URL, acsURL);
      doSet = TRUE;
   }

   if (minWaitInterval != 0)
   {
      mgmtObj->CWMPRetryMinimumWaitInterval = minWaitInterval;
      doSet = TRUE;
   }

   if (intervalMultiplier != 0)
   {
      mgmtObj->CWMPRetryIntervalMultiplier = intervalMultiplier;
      doSet = TRUE;
   }

   if (doSet)
   {
      cmsLog_debug("Setting new value(s) in ManagementServer Object");
      if (CMSRET_SUCCESS != (ret = cmsObj_set(mgmtObj, &iidStack)))
      {
         cmsLog_error("Could not set mgmtObj, ret=%d", ret);
      }
   }
   else
   {
      cmsLog_debug("no change in ManagementServerObject, do not set");
   }

   cmsObj_free((void **) &mgmtObj);

   /* now do the same for DeviceInfo.ProvisioningCode */
   if (cmsUtl_strlen(provisioningCode) > 0)
   {
      Dev2DeviceInfoObject *deviceInfoObj=NULL;

      // Technically do not need to init iidStack since both the
      // previous object (MDMOID_DEV2_MANAGEMENT_SERVER) and this object
      // (MDMOID_DEV2_DEVICE_INFO) are Type 0 objects, so iidStack is always
      // empty.  But do it anyways just to show good form.
      INIT_INSTANCE_ID_STACK(&iidStack);
      if (CMSRET_SUCCESS != (ret = cmsObj_get(MDMOID_DEV2_DEVICE_INFO,
                                   &iidStack, 0, (void **) &deviceInfoObj)))
      {
         cmsLog_error("Could not get DEV2_DEVICE_INFO obj, ret=%d", ret);
         return;
      }

      CMSMEM_REPLACE_STRING(deviceInfoObj->provisioningCode, provisioningCode);

      cmsLog_debug("Setting provisioning code in deviceInfo Object");
      if (CMSRET_SUCCESS != (ret = cmsObj_set(deviceInfoObj, &iidStack)))
      {
         cmsLog_error("Could not set deviceInfoObj, ret=%d", ret);
      }

      cmsObj_free((void **) &deviceInfoObj);
   }

   return;
}
#endif /* SUPPORT_TR69C */

void sskConn_setPppConnStatusByFullPathLocked(const char *pppIntfFullPath, const char *connStatus)
{
   MdmPathDescriptor pathDesc;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(pppIntfFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                   pppIntfFullPath, ret);
      return;
   }

   if (pathDesc.oid != MDMOID_DEV2_PPP_INTERFACE)
   {
      cmsLog_error("fullPath %s must point to PPP.Interface", pppIntfFullPath);
      return;
   }

   sskConn_setPppConnStatusByIidLocked(&pathDesc.iidStack, connStatus);

   return;
}


void sskConn_setPppConnStatusByIidLocked(const InstanceIdStack *pppIntfIidStack, const char *connStatus)
{
   Dev2PppInterfaceObject *pppIntfObj=NULL;
   CmsRet ret;

   cmsLog_debug("Entered: iidStack=%s connStatus=%s",
                cmsMdm_dumpIidStack(pppIntfIidStack), connStatus);

   if ((ret = cmsObj_get(MDMOID_DEV2_PPP_INTERFACE, pppIntfIidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **)&pppIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get PPP.Interface object, ret=%d", ret);
      return;
   }

   sskConn_setPppConnStatusByObjLocked(pppIntfIidStack, pppIntfObj, connStatus);

   cmsObj_free((void **) &pppIntfObj);

   return;
}


void sskConn_setPppConnStatusByObjLocked(const InstanceIdStack *iidStack,
                      Dev2PppInterfaceObject *pppIntfObj,
                      const char *connStatus)
{
   CmsRet ret;

   CMSMEM_REPLACE_STRING(pppIntfObj->connectionStatus, connStatus);

   if ((ret = cmsObj_set(pppIntfObj, iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Set of PPP.Interface object failed, ret=%d", ret);
   }

   return;
}


UBOOL8 getPppIntfByPid(UINT32 pid,
                       char **pppIntfFullPath,
                       InstanceIdStack *pppIntfIidStack,
                       Dev2PppInterfaceObject **pppIntfObj)
{
   UBOOL8 found = FALSE;
   CmsRet ret;

   while (!found &&
          ((ret = cmsObj_getNext(MDMOID_DEV2_PPP_INTERFACE,
                                 pppIntfIidStack,
                                 (void **) pppIntfObj)) == CMSRET_SUCCESS))
   {
      if ((*pppIntfObj)->X_BROADCOM_COM_Pid == pid)
      {
         /* found it, do not free it */
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) pppIntfObj);
      }
   }

   if (found)
   {
      MdmPathDescriptor pathDesc;
      CmsRet r2;

      INIT_PATH_DESCRIPTOR(&pathDesc);
      pathDesc.oid = MDMOID_DEV2_PPP_INTERFACE;
      pathDesc.iidStack = *pppIntfIidStack;

      r2 = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, pppIntfFullPath);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("PathDescToFullPath failed, ret=%d", ret);
         cmsObj_free((void **) pppIntfObj);
         found = FALSE;
      }
   }

   return found;
}


void fillInPppIpcpObject(const InstanceIdStack *pppIntfIidStack,
                         const char *localIPAddress,
                         const char *remoteIPAddress,
                         const char *DNSServers)
{
   InstanceIdStack iidStack = *pppIntfIidStack;
   Dev2PppInterfaceIpcpObject *ipcpObj=NULL;
   CmsRet ret;

   ret = cmsObj_get(MDMOID_DEV2_PPP_INTERFACE_IPCP, &iidStack,
                    OGF_NO_VALUE_UPDATE, (void **) &ipcpObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get IPCP object at iidStack %d, ret=%d",
                    cmsMdm_dumpIidStack(pppIntfIidStack), ret);
      return;
   }

   CMSMEM_REPLACE_STRING(ipcpObj->localIPAddress, localIPAddress);
   CMSMEM_REPLACE_STRING(ipcpObj->remoteIPAddress, remoteIPAddress);
   CMSMEM_REPLACE_STRING(ipcpObj->DNSServers, DNSServers);


   if ((ret = cmsObj_set(ipcpObj, &iidStack)) != CMSRET_SUCCESS)
    {
       cmsLog_error("Set of IPV4Address object failed, ret=%d", ret);
    }

   cmsObj_free((void **)&ipcpObj);

   return;
}


void blankOutPppIpcpObject(const InstanceIdStack *pppIntfIidStack)
{
   Dev2PppInterfaceIpcpObject *ipcpObj=NULL;
   CmsRet ret;

   /* Since there is exactly 1 IPCP object associated with this PPP
    * interface object, and we have the iidStack already, we can get
    * the object directly.
    */
   ret = cmsObj_get(MDMOID_DEV2_PPP_INTERFACE_IPCP, pppIntfIidStack,
                    OGF_NO_VALUE_UPDATE, (void **) &ipcpObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get IPCP object at iidStack %d, ret=%d",
                    cmsMdm_dumpIidStack(pppIntfIidStack), ret);
      return;
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(ipcpObj->localIPAddress);
   CMSMEM_FREE_BUF_AND_NULL_PTR(ipcpObj->remoteIPAddress);
   CMSMEM_FREE_BUF_AND_NULL_PTR(ipcpObj->DNSServers);

      /*
       * XXX TODO: do a set again, but that needs to be tested....
       */

   cmsObj_free((void **) &ipcpObj);

   return;
}




UBOOL8 sskConn_hasStaticIpv4AddressLocked(const InstanceIdStack *ipIntfIidStack)
{
   MdmPathDescriptor pathDesc;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Ipv4AddressObject *ipv4AddrObj=NULL;
   UBOOL8 hasAddr=FALSE;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   while (!hasAddr &&
          ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_IPV4_ADDRESS,
                              ipIntfIidStack,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&ipv4AddrObj)) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(ipv4AddrObj->addressingType, MDMVS_STATIC) &&
          !cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, ipv4AddrObj->IPAddress))
      {
         hasAddr = TRUE;
      }
      cmsObj_free((void **) &ipv4AddrObj);
   }

   return hasAddr;
}


UBOOL8 sskConn_hasAnyIpv4AddressLocked(const InstanceIdStack *ipIntfIidStack)
{
   MdmPathDescriptor pathDesc;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Ipv4AddressObject *ipv4AddrObj=NULL;
   UBOOL8 hasAddr=FALSE;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   while (!hasAddr &&
          ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_IPV4_ADDRESS,
                              ipIntfIidStack,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&ipv4AddrObj)) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, ipv4AddrObj->IPAddress))
      {
         hasAddr = TRUE;
      }
      cmsObj_free((void **) &ipv4AddrObj);
   }

   return hasAddr;
}
#endif  /* DMP_DEVICE2_BASELINE_1 */
