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
* :>
*/


#include "cms.h"
#include "cms_util.h"
#include "prctl.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_msg.h"
#include "cms_msg_cellular.h"
#include "ssk.h"
#if defined (DMP_TIME_1) || defined (DMP_DEVICE2_TIME_1)
#include "ssk_time.h"
#endif
#if defined(OMCI_TR69_DUAL_STACK) || defined(SUPPORT_CELLULAR)
#include "../../libs/cms_core/linux/rut_util.h"
#endif
#ifdef DMP_X_ITU_ORG_GPON_1
#include "../../libs/cms_core/linux/rut_omci.h"
#endif
#ifdef BRCM_VOICE_SUPPORT
#include "dal_voice.h"
#endif

#ifdef DMP_X_BROADCOM_COM_L2TPAC_1
#include "../../libs/cms_core/linux/rut_l2tpac.h"
#endif
/*!\file connstatus.c
 * \brief This file receives updates from ppp and dhcpd about WAN
 *        (layer 3) connection status and updates the MDM with that status.
 *
 */


void processGetWanConnStatus_igd(CmsMsgHeader *msg)
{
   char *queryIfName = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject *ipConnObj=NULL;
   WanPppConnObject *pppConnObj=NULL;
   UBOOL8 found=FALSE;
   UBOOL8 status=FALSE;
   CmsRet ret;
#if defined(OMCI_TR69_DUAL_STACK)
   char *ipAddress = NULL;
#endif

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

#if defined(OMCI_TR69_DUAL_STACK)
   /*
    * OMCI TR069, check ipHost, does not support any_wan
    */   
   if (queryIfName)
   {
       if (rut_isGponIpHostInterface(queryIfName, NULL) == CMSRET_SUCCESS)
       {
           UBOOL8 isIPv4 = FALSE;

           found = TRUE;
           if (rutOmci_getIpHostAddress(queryIfName, &ipAddress, &isIPv4) == CMSRET_SUCCESS)
           {
               status = TRUE;
               CMSMEM_FREE_BUF_AND_NULL_PTR(ipAddress);
           }
       }
   }
#endif

   /*
    * Go through all the WANIPConnection and WANPPPConnection objects
    * until I get my answer.  Can't simply look at my ConfiguredWanConnection
    * structures because they don't contain the ifName.
    */
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConnObj)) == CMSRET_SUCCESS)
   {
      if ((queryIfName != NULL) &&
          (cmsUtl_strcmp(ipConnObj->X_BROADCOM_COM_IfName, queryIfName) == 0))
      {
         /* sender is asking about a specific WAN connection */
         found = TRUE;
         if (cmsUtl_strcmp(ipConnObj->connectionStatus, MDMVS_CONNECTED) == 0)
         {
            status = TRUE;
         }
#ifdef DMP_X_BROADCOM_COM_IPV6_1
         if (cmsUtl_strcmp(ipConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED) == 0)
         {
            status = TRUE;
         }
#endif
      }
      else if (queryIfName == NULL)
      {
         /* sender is asking for any WAN connection up, and we found one */
         if (cmsUtl_strcmp(ipConnObj->connectionStatus, MDMVS_CONNECTED) == 0)
         {
            found = TRUE;
            status = TRUE;
         }
#ifdef DMP_X_BROADCOM_COM_IPV6_1
         if (cmsUtl_strcmp(ipConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED) == 0)
         {
            found = TRUE;
            status = TRUE;
         }
#endif
      }

      cmsObj_free((void **) &ipConnObj);
   }


   INIT_INSTANCE_ID_STACK(&iidStack);
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConnObj)) == CMSRET_SUCCESS)
   {
      if ((queryIfName != NULL) &&
          (cmsUtl_strcmp(pppConnObj->X_BROADCOM_COM_IfName, queryIfName) == 0))
      {
         /* sender is asking about a specific WAN connection */
         found = TRUE;
         if (cmsUtl_strcmp(pppConnObj->connectionStatus, MDMVS_CONNECTED) == 0)
         {
            status = TRUE;
         }
#ifdef DMP_X_BROADCOM_COM_IPV6_1
         if (cmsUtl_strcmp(pppConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED) == 0)
         {
            status = TRUE;
         }
#endif
      }
      else if (queryIfName == NULL)
      {
         if (cmsUtl_strcmp(pppConnObj->connectionStatus, MDMVS_CONNECTED) == 0)
         {
            /* sender is asking for any WAN connection up, and we found one */
            found = TRUE;
            status = TRUE;
         }
#ifdef DMP_X_BROADCOM_COM_IPV6_1
         if (cmsUtl_strcmp(pppConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED) == 0)
         {
            found = TRUE;
            status = TRUE;
         }
#endif
      }

      cmsObj_free((void **) &pppConnObj);
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


#ifdef SUPPORT_CELLULAR
typedef enum
{
    CellularIpAddrType_IPV4 = 0x01,                 
    CellularIpAddrType_IPV6 = 0x10
} CellularIpAddrType;

static UINT32 parseDataCallStateMsg(DevCellularDataCallStateChangedBody *cellularInfo, 
                               DevCellularDataCallStateChangedBody *ipv4cellularInfo, 
                               DevCellularDataCallStateChangedBody *ipv6cellularInfo)		 
{
   char *pAddr = NULL;
   UINT32 ipAddrType = 0;
   UBOOL8 isFirstDns4 = TRUE;
   UBOOL8 isFirstDns6 = TRUE;
   
   *ipv4cellularInfo = *cellularInfo;
   ipv4cellularInfo->addresses[0] = '\0';
   ipv4cellularInfo->gateways[0] = '\0';
   ipv4cellularInfo->dnses[0] = '\0';
   
   *ipv6cellularInfo = *ipv4cellularInfo;

   /*parse ip address*/
   for (pAddr = strtok(cellularInfo->addresses, " "); pAddr; pAddr = strtok(NULL, " "))
   {
      char *pPrefix;   
      char addr[BUFLEN_24];	  
	  
      cmsUtl_strcpy(addr, pAddr);
	  pPrefix = strchr(addr, '/');
      if (pPrefix)	  
      {
         *pPrefix = '\0';
      }

      if (cmsUtl_isValidIpAddress(AF_INET, addr))
      {
         cmsUtl_strcpy(ipv4cellularInfo->addresses, pAddr);
         ipAddrType |= CellularIpAddrType_IPV4;
      }
#ifdef SUPPORT_IPV6
      else if (cmsUtl_isValidIpAddress(AF_INET6, addr))
      {
         cmsUtl_strcpy(ipv6cellularInfo->addresses, pAddr);
         ipAddrType |= CellularIpAddrType_IPV6;
      }
#endif	  
   }   	
	  
   if (!ipAddrType)  
   {
      cmsLog_error("no valid ip address, addresses=%s", cellularInfo->addresses);
      return ipAddrType;	  
   }
   
   /*parse gateways*/
   for (pAddr = strtok(cellularInfo->gateways, " "); pAddr; pAddr = strtok(NULL, " "))
   {
      if (cmsUtl_isValidIpAddress(AF_INET, pAddr))
      {
         cmsUtl_strcpy(ipv4cellularInfo->gateways, pAddr);
      }
#ifdef SUPPORT_IPV6
      else if (cmsUtl_isValidIpAddress(AF_INET6, pAddr))
      {
         cmsUtl_strcpy(ipv6cellularInfo->gateways, pAddr);
      }
#endif	  
   }
   
   /*parse dns*/
   for (pAddr = strtok(cellularInfo->dnses, " "); pAddr; pAddr = strtok(NULL, " "))
   {
      if (cmsUtl_isValidIpAddress(AF_INET, pAddr))
      {
         if (!isFirstDns4)  
         {
            cmsUtl_strcat(ipv4cellularInfo->dnses, ",");
         }
		 else
         {
            isFirstDns4 = FALSE;			
         }
		 
         cmsUtl_strcat(ipv4cellularInfo->dnses, pAddr);
      }
#ifdef SUPPORT_IPV6
      else if (cmsUtl_isValidIpAddress(AF_INET6, pAddr))
      {
         if (!isFirstDns6)  
         {
            cmsUtl_strcat(ipv6cellularInfo->dnses, ",");
         }
      else
      {
            isFirstDns6 = FALSE;			
         }
		 
         cmsUtl_strcat(ipv6cellularInfo->dnses, pAddr);
      }
#endif	  
   }

   return ipAddrType;   
}

static void processCellularState4Changed(DevCellularDataCallStateChangedBody *cellularInfo)
{
   WanIpConnObject *ipConn=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE, isChange=FALSE;
   CmsRet ret;
  
   cmsLog_notice("ifname=%s, active=%d, ip=%s, gateway=%s, nameserver=%s", 
                cellularInfo->ifname, cellularInfo->active, cellularInfo->addresses, 
                cellularInfo->gateways, cellularInfo->dnses);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d, cellular status update dropped!", ret);
      return;
   }

   while ((!found) && 
          ((ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConn)) == CMSRET_SUCCESS))
   {		 
      cmsLog_debug("ipConn: X_BROADCOM_COM_IfName=%s, connectionStatus=%s, addresses=%s, dns=%s",
                   ipConn->X_BROADCOM_COM_IfName, ipConn->connectionStatus, ipConn->externalIPAddress, ipConn->DNSServers);
      if (!(cmsUtl_strcmp(ipConn->X_BROADCOM_COM_IfName, cellularInfo->ifname)))
      {
         found = TRUE;

         /*Cellular ip will keep same after registration, so we only monior up and down*/
         if (ipConn->X_BROADCOM_COM_IPv4Enabled &&
             ((cellularInfo->active && (cmsUtl_strcmp(ipConn->connectionStatus, MDMVS_CONNECTED) ||    //active->inactive
               cmsUtl_strcmp(ipConn->externalIPAddress, cellularInfo->addresses) ||          //ip address change
               cmsUtl_strcmp(ipConn->DNSServers, cellularInfo->dnses))) ||          //dns change
             (!cellularInfo->active && cmsUtl_strcmp(ipConn->connectionStatus, MDMVS_DISCONNECTED)))) //inactive->active
         {
           cmsLog_debug("current connectionStatus =%s, addresses=%s, dns=%s", 
                        ipConn->connectionStatus, ipConn->externalIPAddress, ipConn->DNSServers);
		   
           isChange = TRUE;
         }
      }
      else
      {
         cmsObj_free((void **) &ipConn);
      }
   }
 
 
   if (!found)
   {
      cmsLog_error("ipConn not found for %s", cellularInfo->ifname);
   }
   else if (!isChange)
   {
   
      if (cellularInfo->active)
      {	
         char cmd[BUFLEN_64];

         /*In some case like VOICE_NETWORK_STATE_CHANGED, rild will reconfig rmnetX and lost default gw,
           in this case, if ip address keep same, we should add back default gw*/
         snprintf(cmd, sizeof(cmd), "route del default gw %s 2>/dev/null",ipConn->defaultGateway);
         rut_doSystemAction("processCellularState4Changed", cmd);
		 
         snprintf(cmd, sizeof(cmd), "route add default gw %s 2>/dev/null",ipConn->defaultGateway);
         rut_doSystemAction("processCellularState4Changed", cmd);
      }
      else
      {	
         cmsLog_debug("ipConn status not change for %s", cellularInfo->ifname);
      }
	  
      cmsObj_free((void **) &ipConn);
   }
   else
   {	  
      /*
       * Free up as memory as possible before we go into the RCL, which will
       * do lots of insmod and iptables actions.  We need to do this on low
       * memory systems to prevent thrashing of memory.
       */
      if (isLowMemorySystem)
      {
         prctl_runCommandInShellBlocking("echo 3 > /proc/sys/vm/drop_caches");
      }
 
 #ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1
      if (isAutoDetectionEnabled)
      {
         ipConn->X_BROADCOM_COM_LastConnected = TRUE;
      }
      else
      {
         ipConn->X_BROADCOM_COM_LastConnected = FALSE;
      }
 #endif 

      if (!cellularInfo->active || cmsUtl_strcmp(ipConn->externalIPAddress, cellularInfo->addresses))
      {		 
         /* Set the transient layer 2 link status for the CmsObj_set operation which may take some time and
         * the real layer 2 link status could be changed during this time.
         */      
         REPLACE_STRING_IF_NOT_EQUAL(ipConn->X_BROADCOM_COM_TransientLayer2LinkStatus, MDMVS_DOWN);
         REPLACE_STRING_IF_NOT_EQUAL(ipConn->connectionStatus, MDMVS_DISCONNECTED);
		 
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
         if (ipConn->X_BROADCOM_COM_IPv6Enabled)
         {
            REPLACE_STRING_IF_NOT_EQUAL(ipConn->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_DISCONNECTED);
         }
#endif

         if ((ret = cmsObj_set(ipConn, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("set of ipConn failed, ret=%d", ret);
         }
    
         /* Send Wan Connection Down event msg */
         sendStatusMsgToSmd(CMS_MSG_WAN_CONNECTION_DOWN, ipConn->X_BROADCOM_COM_IfName);
      }

      if (cellularInfo->active)
      {
         char *p;    
         UINT8 maskVal;    
		 char maskBuf[BUFLEN_16];
         struct in_addr subnetMask;
		 
         if ((p = strchr(cellularInfo->addresses, '/')) != NULL)
         {
           *p++ = '\0';
         }
         maskVal = (p ? atoi(p) : 24);
         subnetMask.s_addr = htonl(0xffffffff << (32 - maskVal));
         inet_ntop(AF_INET, &subnetMask, maskBuf, sizeof(maskBuf));

         REPLACE_STRING_IF_NOT_EQUAL(ipConn->connectionStatus, MDMVS_CONNECTED);
         REPLACE_STRING_IF_NOT_EQUAL(ipConn->externalIPAddress, cellularInfo->addresses);
         REPLACE_STRING_IF_NOT_EQUAL(ipConn->subnetMask, maskBuf);
         REPLACE_STRING_IF_NOT_EQUAL(ipConn->defaultGateway, cellularInfo->gateways);
         REPLACE_STRING_IF_NOT_EQUAL(ipConn->DNSServers, cellularInfo->dnses);

         if ((ret = cmsObj_set(ipConn, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("set of ipConn failed, ret=%d", ret);
         }
    
         /* Send Wan Connection Up event msg */
         sendStatusMsgToSmd(CMS_MSG_WAN_CONNECTION_UP, ipConn->X_BROADCOM_COM_IfName);		 
      } 
 
      cmsObj_free((void **) &ipConn);
   }     

   cmsLck_releaseLock();

   return;
}

void processCellularStateChanged(CmsMsgHeader *msg)
{  
   cmsLog_notice("msg->wordData = %d", msg->wordData);
   
   switch (msg->wordData)
   {
      case DEVCELL_EVENT_DATACALLSTATECHANGED:
      {
         DevCellularDataCallStateChangedBody *cellularInfo = (DevCellularDataCallStateChangedBody *) (msg + 1);
         DevCellularDataCallStateChangedBody ipv4cellularInfo, ipv6cellularInfo;
         UINT32 ipSupported;
		 
         ipSupported = parseDataCallStateMsg(cellularInfo, &ipv4cellularInfo, &ipv6cellularInfo);	 
         cmsLog_debug("ipSupported = 0x%x", ipSupported);
		 
         if (ipSupported & CellularIpAddrType_IPV4)
         {
            processCellularState4Changed(&ipv4cellularInfo);
         }
		 
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
         if (ipSupported & CellularIpAddrType_IPV6)
         {
            processCellularState6Changed(&ipv6cellularInfo);
         }
#endif
         break;
      }

      case DEVCELL_EVENT_RADIOSTATECHANGED:
      {
         DevCellularRadioState *radioState = (DevCellularRadioState *) (msg + 1);
         WanIpConnObject *ipConn=NULL;
         InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
         CmsRet ret;
		 
         if (*radioState != DEVCELL_RADIO_STATE_ON)
         {
            DevCellularDataCallStateChangedBody cellularInfo;
            //Imply cellularInfo->active = FALSE;
            memset(&cellularInfo, 0, sizeof(cellularInfo));
			
            while (((ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConn)) == CMSRET_SUCCESS))
            {		 
               cmsUtl_strcpy(cellularInfo.ifname, ipConn->X_BROADCOM_COM_IfName);
			   
               if (ipConn->X_BROADCOM_COM_IPv4Enabled)   
               {
                  processCellularState4Changed(&cellularInfo);
               }
			   
#ifdef SUPPORT_IPV6
               if (ipConn->X_BROADCOM_COM_IPv6Enabled)   
               {
                  processCellularState6Changed(&cellularInfo);
               }
#endif			   
               cmsObj_free((void **) &ipConn);
            }
         }
		 
         break;
      }

      default:
         cmsLog_error("cannot handle event type %d from %d (flags=0x%x)",
                      msg->wordData, msg->src, msg->flags);
   }
}

#endif


void processDhcpcStateChanged_igd(CmsMsgHeader *msg)
{
   DhcpcStateChangedMsgBody *dhcpcInfo = (DhcpcStateChangedMsgBody *) (msg + 1);
   WanIpConnObject *ipConn=NULL;
   LanIpIntfObject *lanIpIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   UINT32 pid;
   CmsRet ret;
#ifdef DMP_X_ITU_ORG_GPON_1
   BcmOmciRtdIpHostConfigDataObject *ipHost = NULL;
   BcmOmciRtdIpHostConfigDataExtObject *bcmIpHost = NULL;
#endif
#if defined(BRCM_VOICE_SUPPORT) && VOICE_IPV6_SUPPORT 
   int isIpv6 = 0;
   char ipAddrFamily[BUFLEN_16];
   unsigned int length = 16;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
       cmsLog_error("failed to get lock, ret=%d", ret);
       return;
   }
   
#if defined(BRCM_PKTCBL_SUPPORT)
   ret = dalVoice_SetIpFamily( NULL, MDMVS_IPV4 );
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set the IP address family(IPV4) for voice, ret=%d", ret);
   }
#endif

   ret = dalVoice_GetIpFamily( NULL, ipAddrFamily, length );
   cmsLck_releaseLock();
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get the IP address family for voice, ret=%d", ret);
      return;
   }
   else
   {
      isIpv6 = !(cmsUtl_strcmp( ipAddrFamily, MDMVS_IPV6 ));
   }
#endif
  
   pid = PID_IN_EID(msg->src);

   cmsLog_debug("dhcpc pid=%d dataLength=%d addressAssigned=%d expired=%d", pid, msg->dataLength, dhcpcInfo->addressAssigned, dhcpcInfo->isExpired);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d, dhcp client status update dropped!", ret);
      /* hmm, this is bad, I can't update the MDM with new dhcpc info */
      return;
   }


   if (dhcpcInfo->addressAssigned)
   {
      cmsLog_debug("assigned ip=%s netmask=%s gateway=%s nameserver=%s",
                   dhcpcInfo->ip,
                   dhcpcInfo->mask,
                   dhcpcInfo->gateway,
                   dhcpcInfo->nameserver);

      if (strlen(dhcpcInfo->acsURL) > 0 ||
          strlen(dhcpcInfo->acsProvisioningCode) > 0 ||
          dhcpcInfo->cwmpRetryMinimumWaitInterval != 0 ||
          dhcpcInfo->cwmpRetryIntervalMultiplier != 0)
      {
         /*
          * Don't know whether to do this first or set connection up first.
          * Should be OK either way.  If I do this first, tr69c will wake up
          * but find that wan connection is not up yet, so it won't try to
          * connect to the ACS.
          */
         cmsLog_debug("got acsURL=%s provisioningCode=%s MinWaitInterval=%d intMult=%d",
                      dhcpcInfo->acsURL,
                      dhcpcInfo->acsProvisioningCode,
                      dhcpcInfo->cwmpRetryMinimumWaitInterval,
                      dhcpcInfo->cwmpRetryIntervalMultiplier);
#ifdef SUPPORT_TR69C
         setAcsParams(dhcpcInfo->acsURL, dhcpcInfo->acsProvisioningCode,
                      dhcpcInfo->cwmpRetryMinimumWaitInterval,
                      dhcpcInfo->cwmpRetryIntervalMultiplier);
#endif
      }

      while ((!found) && 
             ((ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConn)) == CMSRET_SUCCESS))
      {
         cmsLog_debug("addressing type=%s", ipConn->addressingType);
         if (!(cmsUtl_strcmp(ipConn->connectionType, MDMVS_IP_ROUTED)) &&
             !(cmsUtl_strcmp(ipConn->addressingType, MDMVS_DHCP)) &&
             (ipConn->X_BROADCOM_COM_DhcpcPid == pid))
         {
            /*
             * We know this is the correct IPConn object because we have
             * matched up the dhcpc pid.
             */
            found = TRUE;

            CMSMEM_REPLACE_STRING(ipConn->externalIPAddress, dhcpcInfo->ip);
            CMSMEM_REPLACE_STRING(ipConn->subnetMask, dhcpcInfo->mask);
            CMSMEM_REPLACE_STRING(ipConn->defaultGateway, dhcpcInfo->gateway);
            CMSMEM_REPLACE_STRING(ipConn->DNSServers, dhcpcInfo->nameserver);

            if (ipConn->externalIPAddress == NULL ||
                ipConn->subnetMask == NULL ||
                ipConn->defaultGateway == NULL ||
                ipConn->DNSServers == NULL)
                {
                   cmsLog_error("one or more allocations in ipConn failed");
                   cmsObj_free((void **) &ipConn);
                }
         }
         else
         {
            /*
             * This instance of the ipConn object is not the one we want.
             * free it and go back up to the while loop to get the next instance.
             */
            cmsObj_free((void **) &ipConn);
         }
      }

      if (!found)
      {
         /* maybe this dhcpc was running on LAN side? */
         INIT_INSTANCE_ID_STACK(&iidStack);
         while ((!found) &&
                ((ret = cmsObj_getNext(MDMOID_LAN_IP_INTF, &iidStack, (void **) &lanIpIntfObj)) == CMSRET_SUCCESS))
         {
            if (!cmsUtl_strcmp(lanIpIntfObj->IPInterfaceAddressingType, MDMVS_DHCP) &&
                lanIpIntfObj->X_BROADCOM_COM_DhcpcPid == pid)
            {
               found = TRUE;

               printf("DHCP acquired LAN side IP address %s (dns=%s gateway=%s)\n",
                       dhcpcInfo->ip, dhcpcInfo->nameserver, dhcpcInfo->gateway);
               CMSMEM_REPLACE_STRING(lanIpIntfObj->X_BROADCOM_COM_DhcpConnectionStatus, MDMVS_CONNECTED);
               CMSMEM_REPLACE_STRING(lanIpIntfObj->IPInterfaceIPAddress, dhcpcInfo->ip);
               CMSMEM_REPLACE_STRING(lanIpIntfObj->IPInterfaceSubnetMask, dhcpcInfo->mask);
               CMSMEM_REPLACE_STRING(lanIpIntfObj->X_BROADCOM_COM_DhcpDefaultGateway, dhcpcInfo->gateway);
               CMSMEM_REPLACE_STRING(lanIpIntfObj->X_BROADCOM_COM_DNSServers, dhcpcInfo->nameserver);

            }
            else
            {
               cmsObj_free((void **) &lanIpIntfObj);
            }
         }
      }

      if (!found)
      {
#ifndef DMP_X_ITU_ORG_GPON_1
         cmsLog_error("No Dynamic IPoE connection found!");
#endif    // DMP_X_ITU_ORG_GPON_1
      }
      else
      {
         if (ipConn != NULL)
         {
#if defined (DMP_TIME_1) || defined (DMP_DEVICE2_TIME_1)
            /* If dhcp server provided ntp servers, set that first */
            if (strlen(dhcpcInfo->ntpserver) > 0)
            {
               cmsLog_debug("Got ntpserver(s)=%s", dhcpcInfo->ntpserver);
               setNtpServers(dhcpcInfo->ntpserver);
            }
#endif /* DMP_TIME_1 or DMP_DEVICE2_TIME_1 */
            /*
             * Free up as memory as possible before we go into the RCL, which will
             * do lots of insmod and iptables actions.  We need to do this on low
             * memory systems to prevent thrashing of memory.
             */
            if (isLowMemorySystem)
            {
               prctl_runCommandInShellBlocking("echo 3 > /proc/sys/vm/drop_caches");
            }

            /* update the 6rd tunnel configuration if there is any */
            dalTunnel_update6rdObject( dhcpcInfo->prefix, dhcpcInfo->brAddr, ipConn->X_BROADCOM_COM_IfName,
                                       (SINT32)dhcpcInfo->ipv4MaskLen, (SINT32)dhcpcInfo->ipv6PrefixLen );

            CMSMEM_REPLACE_STRING(ipConn->connectionStatus, MDMVS_CONNECTED);

#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1
            if (isAutoDetectionEnabled)
            {
               ipConn->X_BROADCOM_COM_LastConnected = TRUE;
            }
            else
            {
               ipConn->X_BROADCOM_COM_LastConnected = FALSE;
            }
#endif 
            if ((ret = cmsObj_set(ipConn, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("set of ipConn failed, ret=%d", ret);
            }

            /* Send Wan Connection Up event msg */
            sendStatusMsgToSmd(CMS_MSG_WAN_CONNECTION_UP, ipConn->X_BROADCOM_COM_IfName);

#if defined(BRCM_VOICE_SUPPORT)
#if VOICE_IPV6_SUPPORT
            if ( !isIpv6 )
#endif
            {
               if ( isVoiceOnAnyWan || voiceWanIfName )
               {
                  initVoiceOnWanIntfLocked(ipConn->X_BROADCOM_COM_IfName, ipConn->externalIPAddress);
               }
            }
#endif
            cmsObj_free((void **) &ipConn);
         }
         else if (lanIpIntfObj != NULL)
         {
            cmsLog_debug("setting new state to %s", lanIpIntfObj->X_BROADCOM_COM_DhcpConnectionStatus);
            if (CMSRET_SUCCESS != (ret = cmsObj_set(lanIpIntfObj, &iidStack)))
            {
               cmsLog_error("set of lanIpIntfObj failed, ret=%d", ret);
            }

            /* XXX Temp: for now, pretend this is a WAN connection.
             * Generate the event msg in case tr69c is waiting for it.
             */
            sendStatusMsgToSmd(CMS_MSG_WAN_CONNECTION_UP, lanIpIntfObj->X_BROADCOM_COM_IfName);

            cmsObj_free((void **) &lanIpIntfObj);
         }
      }

#ifdef DMP_X_ITU_ORG_GPON_1
      INIT_INSTANCE_ID_STACK(&iidStack);

      while ((!found) &&
             ((ret = cmsObj_getNext(MDMOID_BCM_OMCI_RTD_IP_HOST_CONFIG_DATA_EXT,
                                   &iidStack, (void **) &bcmIpHost))
              == CMSRET_SUCCESS))
      {
         // look for Broadcom IP Host that has its
         // dhcpPid match with the pid in the message
         found = (bcmIpHost->dhcpcPid == pid);
         if (found == TRUE)
         {
            InstanceIdStack iidIpHost = EMPTY_INSTANCE_ID_STACK;

            struct in_addr inAddr;
            // look for IP Host that has its managedEntityId
            // match with managedEntityId of Broadcom IP Host
            while ((ret = cmsObj_getNext(MDMOID_BCM_OMCI_RTD_IP_HOST_CONFIG_DATA,
                                         &iidIpHost, (void **) &ipHost))
                    == CMSRET_SUCCESS)
            {
               if (ipHost->managedEntityId == bcmIpHost->managedEntityId)
               {
                  char *hexStr;
                  char *ptr;
                  if (inet_aton(dhcpcInfo->ip, &inAddr) != 0)
                     ipHost->currentAddress = inAddr.s_addr;
                  if (inet_aton(dhcpcInfo->mask, &inAddr) != 0)
                     ipHost->currentMask = inAddr.s_addr;
                  if (inet_aton(dhcpcInfo->gateway, &inAddr) != 0)
                     ipHost->currentGateway = inAddr.s_addr;
                  //more than one name server
                  if ((ptr = strstr(dhcpcInfo->nameserver, ",")))
                  {
                      ptr[0] = '\0';
                      if (inet_aton(dhcpcInfo->nameserver, &inAddr) != 0)
                      {
                          ipHost->currentPrimaryDns = inAddr.s_addr;
                          if (inet_aton(&ptr[1], &inAddr) != 0)
                              ipHost->currentSecondaryDns= inAddr.s_addr;
                      }
                  }
                  else if (inet_aton(dhcpcInfo->nameserver, &inAddr) != 0)
                     ipHost->currentPrimaryDns = inAddr.s_addr;

                  if (cmsUtl_binaryBufToHexString((UINT8 *) dhcpcInfo->hostName, 25, &hexStr) == CMSRET_SUCCESS)
                  {
                    CMSMEM_REPLACE_STRING(ipHost->hostName, hexStr);
                    CMSMEM_FREE_BUF_AND_NULL_PTR(hexStr);
                  }

                  if (cmsUtl_binaryBufToHexString((UINT8 *) dhcpcInfo->domain, 25, &hexStr) == CMSRET_SUCCESS)
                  {
                    CMSMEM_REPLACE_STRING(ipHost->domainName, hexStr);
                    CMSMEM_FREE_BUF_AND_NULL_PTR(hexStr);
                  }

                  cmsObj_set(ipHost, &iidIpHost);
                  sendStatusMsgToSmd(CMS_MSG_WAN_CONNECTION_UP, bcmIpHost->interfaceName);
               }
               cmsObj_free((void **) &ipHost);
            }
            //Need to change bcmIpHost->connectionStatus after set ipHost->currentxxx
            bcmIpHost->connectionStatus = 2;   // Connected
            cmsObj_set(bcmIpHost, &iidStack);
         }
         cmsObj_free((void **) &bcmIpHost);
      }
#endif    // DMP_X_ITU_ORG_GPON_1
   }
   else
   {
      /* When WAN connection lease expires and dhcpc fail to renew the lease, 
       * dhcpc will notify ssk with the following info:
       * 1) addressAssigned == FALSE 
       * 2) isExpired == TRUE
       * Ssk needs to set this WAN connection status to "Connecting" to tear down the layer  3 related ip services
       * but leave the dhcpc running since layer 2 WAN link is still up.
       */
     if(dhcpcInfo->isExpired)
     {
         while ((!found) && 
                ((ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConn)) == CMSRET_SUCCESS))
         {
            cmsLog_debug("addressing type=%s", ipConn->addressingType);
            if (!(cmsUtl_strcmp(ipConn->connectionType, MDMVS_IP_ROUTED)) &&
                !(cmsUtl_strcmp(ipConn->addressingType, MDMVS_DHCP)) &&

                (ipConn->X_BROADCOM_COM_DhcpcPid == pid))
            {
               /*
                * We know this is the correct IPConn object because we have
                * matched up the dhcpc pid.
                */
               found = TRUE;
               sendStatusMsgToSmd(CMS_MSG_WAN_CONNECTION_DOWN, ipConn->X_BROADCOM_COM_IfName);

               /* clean up this WAN ip connection */
               CMSMEM_REPLACE_STRING(ipConn->connectionStatus, MDMVS_CONNECTING);
               CMSMEM_REPLACE_STRING(ipConn->externalIPAddress, "0.0.0.0");
               CMSMEM_REPLACE_STRING(ipConn->subnetMask, "0.0.0.0");
               CMSMEM_REPLACE_STRING(ipConn->defaultGateway, "0.0.0.0");
               CMSMEM_REPLACE_STRING(ipConn->DNSServers, "0.0.0.0");

               if ((ret = cmsObj_set(ipConn, &iidStack)) != CMSRET_SUCCESS)
               {
                   cmsLog_error("set of ipConn failed, ret=%d", ret);
               }
            }
            cmsObj_free((void **) &ipConn);
         }

         /* maybe this dhcpc was running on LAN side? */
         INIT_INSTANCE_ID_STACK(&iidStack);
         while ((!found) &&
                ((ret = cmsObj_getNext(MDMOID_LAN_IP_INTF, &iidStack, (void **) &lanIpIntfObj)) == CMSRET_SUCCESS))
         {
            if (!cmsUtl_strcmp(lanIpIntfObj->IPInterfaceAddressingType, MDMVS_DHCP) &&
                lanIpIntfObj->X_BROADCOM_COM_DhcpcPid == pid)
            {
               found = TRUE;

               // XXX Temp: for consistency with the address acquired case,
               // if we lose the address, generate WAN_CONNECTION_DOWN event.
               sendStatusMsgToSmd(CMS_MSG_WAN_CONNECTION_DOWN, lanIpIntfObj->X_BROADCOM_COM_IfName);

               CMSMEM_REPLACE_STRING(lanIpIntfObj->X_BROADCOM_COM_DhcpConnectionStatus, MDMVS_CONNECTING);
               CMSMEM_REPLACE_STRING(lanIpIntfObj->IPInterfaceIPAddress, "0.0.0.0");
               CMSMEM_REPLACE_STRING(lanIpIntfObj->IPInterfaceSubnetMask, "0.0.0.0");
               CMSMEM_REPLACE_STRING(lanIpIntfObj->X_BROADCOM_COM_DhcpDefaultGateway, "0.0.0.0");
               CMSMEM_REPLACE_STRING(lanIpIntfObj->X_BROADCOM_COM_DNSServers, "0.0.0.0");

               if ((ret = cmsObj_set(lanIpIntfObj, &iidStack)) != CMSRET_SUCCESS)
               {
                   cmsLog_error("set of lanIpIntfObj failed, ret=%d", ret);
               }
            }
            cmsObj_free((void **) &lanIpIntfObj);
         }

#ifdef DMP_X_ITU_ORG_GPON_1
         INIT_INSTANCE_ID_STACK(&iidStack);

         while ((!found) &&
                ((ret = cmsObj_getNext(MDMOID_BCM_OMCI_RTD_IP_HOST_CONFIG_DATA_EXT,
                                      &iidStack, (void **) &bcmIpHost))
                  == CMSRET_SUCCESS))
         {
             // look for Broadcom IP Host that has its
             // dhcpPid match with the pid in the message
             found = (bcmIpHost->dhcpcPid == pid);
             if (found == TRUE)
             {
                 InstanceIdStack iidIpHost = EMPTY_INSTANCE_ID_STACK;

                 // look for IP Host that has its managedEntityId
                 // match with managedEntityId of Broadcom IP Host
                 while ((ret = cmsObj_getNext(MDMOID_BCM_OMCI_RTD_IP_HOST_CONFIG_DATA,
                                              &iidIpHost, (void **) &ipHost))
                         == CMSRET_SUCCESS)
                 {
                     if (ipHost->managedEntityId == bcmIpHost->managedEntityId)
                     {
                         ipHost->currentAddress = 0;
                         ipHost->currentMask = 0;
                         ipHost->currentGateway = 0;
                         ipHost->currentPrimaryDns = 0;
                         ipHost->currentSecondaryDns= 0;
                         CMSMEM_REPLACE_STRING(ipHost->hostName, "00");
                         CMSMEM_REPLACE_STRING(ipHost->domainName, "00");
  
                         cmsObj_set(ipHost, &iidIpHost);
                     }
                     cmsObj_free((void **) &ipHost);
                 }
                 //Need to change bcmIpHost->connectionStatus after set ipHost->currentxxx
                 bcmIpHost->connectionStatus = 5;   // OMCI_CONN_DISCONNECTED
                 cmsObj_set(bcmIpHost, &iidStack);
             }
             cmsObj_free((void **) &bcmIpHost);
         }
#endif    // DMP_X_ITU_ORG_GPON_1
     }
   }  

   cmsLck_releaseLock();


   /*
    * Now that ssk has updated the data model (and the RCL handler functions
    * had a chance to run), send this event to smd for general distribution.
    * Most apps will not need to subscribe to the DHCPC state changed event.
    * It makes more sense for them to subscribe to the WAN_CONNECTION_UP event.
    */
   msg->dst = EID_SMD;
   if ((ret = cmsMsg_send(msgHandle, msg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to forward dhcp state changed message, ret=%d", ret);
   }

   return;
}


#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */

#define IFADDRCONF_ADD     0
#define IFADDRCONF_REMOVE  1

void processDhcp6cStateChanged_igd(const CmsMsgHeader *msg)
{
   Dhcp6cStateChangedMsgBody *dhcp6cInfo = (Dhcp6cStateChangedMsgBody *) (msg + 1);
   WanIpConnObject  *ipConn  = NULL;
   WanPppConnObject *pppConn = NULL;
   char ifName[CMS_IFNAME_LENGTH]={0};
   InstanceIdStack iidStack;
   UBOOL8 found;
   UINT32 pid;
   CmsRet ret;

#if defined(BRCM_VOICE_SUPPORT) && VOICE_IPV6_SUPPORT
   int isIpv6 = 0;
   char ipAddrFamily[BUFLEN_16];
   unsigned int length = 16;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
       cmsLog_error("failed to get lock, ret=%d", ret);
       return;
   }

#if defined(BRCM_PKTCBL_SUPPORT)
   ret = dalVoice_SetIpFamily( NULL, MDMVS_IPV6 );
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set the IP address family(IPV6) for voice, ret=%d", ret);
   }
#endif
   
   ret = dalVoice_GetIpFamily( NULL, ipAddrFamily, length );
   cmsLck_releaseLock();
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get the IP address family for voice, ret=%d", ret);
      return;
   }
   else
   {
      isIpv6 = !(cmsUtl_strcmp( ipAddrFamily, MDMVS_IPV6 ));
   }  
#endif
   
   pid = PID_IN_EID(msg->src);

   cmsLog_debug("pid=%d dataLength=%d", pid, msg->dataLength);
   cmsLog_debug("prefixAssigned=%d prefixCmd=%d sitePrefix=%s pdIfAddress=%s prefixPltime=%d prefixVltime=%d\n"
                "addrAssigned=%d addrCmd=%d address=%s ifname=%s\n"
                "dnsAssigned=%d nameserver=%s aftrAssigned=%d aftr=%s",
                dhcp6cInfo->prefixAssigned, dhcp6cInfo->prefixCmd, dhcp6cInfo->sitePrefix,
                dhcp6cInfo->pdIfAddress, dhcp6cInfo->prefixPltime, dhcp6cInfo->prefixVltime,
                dhcp6cInfo->addrAssigned, dhcp6cInfo->addrCmd, dhcp6cInfo->address, dhcp6cInfo->ifname, 
                dhcp6cInfo->dnsAssigned, dhcp6cInfo->nameserver, dhcp6cInfo->aftrAssigned, dhcp6cInfo->aftr);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      /* hmm, this is bad, I can't update the MDM with new dhcp6c info */
      return;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   found = FALSE;

   /* Search the WAN connection object */
   while ((!found) && 
          ((ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **)&ipConn)) == CMSRET_SUCCESS))
   {
      if ((strcmp(ipConn->connectionType, MDMVS_IP_ROUTED) == 0) &&
          (ipConn->X_BROADCOM_COM_Dhcp6cPid == pid))
      {
         found = TRUE;
         cmsUtl_strncpy(ifName, ipConn->X_BROADCOM_COM_IfName, sizeof(ifName));
      }
      else
      {
         cmsObj_free((void **) &ipConn);
      }            
   }

   if (!found)
   {
      ipConn = NULL;
      INIT_INSTANCE_ID_STACK(&iidStack);

      while ((!found) && 
             ((ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **)&pppConn)) == CMSRET_SUCCESS))
      {
         if (pppConn->X_BROADCOM_COM_Dhcp6cPid == pid)
         {
            found = TRUE;
            cmsUtl_strncpy(ifName, pppConn->X_BROADCOM_COM_IfName, sizeof(ifName));
         }
         else
         {
            cmsObj_free((void **) &pppConn);
         }            
      }

      if (!found)
      {
         //cmsLog_error("No WAN connection found!");
         pppConn = NULL;
      }
   }

   if (found)
   {
      UBOOL8 connected=FALSE;
      UBOOL8 pdChange = 0;

      cmsLog_debug("found a correct wanObj");

      if (strlen(dhcp6cInfo->acsURL) > 0 ||
          strlen(dhcp6cInfo->acsProvisioningCode) > 0 ||
          dhcp6cInfo->cwmpRetryMinimumWaitInterval != 0 ||
          dhcp6cInfo->cwmpRetryIntervalMultiplier != 0)
      {
         cmsLog_debug("got acsURL=%s provisioningCode=%s MinWaitInterval=%d intMult=%d",
                      dhcp6cInfo->acsURL,
                      dhcp6cInfo->acsProvisioningCode,
                      dhcp6cInfo->cwmpRetryMinimumWaitInterval,
                      dhcp6cInfo->cwmpRetryIntervalMultiplier);
#ifdef SUPPORT_TR69C
         setAcsParams(dhcp6cInfo->acsURL, dhcp6cInfo->acsProvisioningCode,
                      dhcp6cInfo->cwmpRetryMinimumWaitInterval,
                      dhcp6cInfo->cwmpRetryIntervalMultiplier);
#endif
      }

#if defined (DMP_TIME_1) || defined (DMP_DEVICE2_TIME_1)
      if (strlen(dhcp6cInfo->ntpserver) > 0)
      {
         cmsLog_debug("got ntpserver(s)=%s", dhcp6cInfo->ntpserver);
         setNtpServers(dhcp6cInfo->ntpserver);
      }
#endif /* DMP_TIME_1 or DMP_DEVICE2_TIME_1 */

      /* dhcp6c receives an IPv6 address for WAN interface */
      if (dhcp6cInfo->addrAssigned)
      {
         if (dhcp6cInfo->addrCmd == IFADDRCONF_ADD)
         {
            cmsLog_debug("assigned IPv6 address");
            connected = TRUE;

            if (ipConn)
            {
               REPLACE_STRING_IF_NOT_EQUAL(ipConn->X_BROADCOM_COM_ExternalIPv6Address, dhcp6cInfo->address);
               REPLACE_STRING_IF_NOT_EQUAL(ipConn->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED);
            }
            else
            {
               REPLACE_STRING_IF_NOT_EQUAL(pppConn->X_BROADCOM_COM_ExternalIPv6Address, dhcp6cInfo->address);
               REPLACE_STRING_IF_NOT_EQUAL(pppConn->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED);
            }
         }
         else  /* IFADDRCONF_REMOVE */
         {
            cmsLog_debug("remove IPv6 address");

            if (ipConn)
            {
               CMSMEM_FREE_BUF_AND_NULL_PTR(ipConn->X_BROADCOM_COM_ExternalIPv6Address);
            }
            else
            {
               CMSMEM_FREE_BUF_AND_NULL_PTR(pppConn->X_BROADCOM_COM_ExternalIPv6Address);
            }

            cmsLog_debug("FIXME: update connection status for address removal!!");
         }
      }

      /* dhcp6c receives prefix delegation for LAN */
      if (dhcp6cInfo->prefixAssigned)
      {
         if (dhcp6cInfo->prefixCmd == IFADDRCONF_ADD &&
             dhcp6cInfo->prefixPltime != 0 && dhcp6cInfo->prefixVltime != 0)
         {
            cmsLog_debug("assigned prefix");

            connected = TRUE;
            if (ipConn)
            {
               if ( cmsUtl_strcmp(ipConn->X_BROADCOM_COM_IPv6SitePrefix, dhcp6cInfo->sitePrefix) != 0 )
               {
                  CMSMEM_REPLACE_STRING(ipConn->X_BROADCOM_COM_IPv6SitePrefix, dhcp6cInfo->sitePrefix);
                  pdChange = 1;
               }

               if (dhcp6cInfo->prefixVltimeOld != 0)
               {
                  CMSMEM_REPLACE_STRING(ipConn->X_BROADCOM_COM_IPv6SitePrefixOld, dhcp6cInfo->sitePrefixOld);
                  ipConn->X_BROADCOM_COM_IPv6SitePrefixVltimeOld = dhcp6cInfo->prefixVltimeOld;
               }

               REPLACE_STRING_IF_NOT_EQUAL(ipConn->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED);
               ipConn->X_BROADCOM_COM_IPv6SitePrefixPltime = dhcp6cInfo->prefixPltime;
               ipConn->X_BROADCOM_COM_IPv6SitePrefixVltime = dhcp6cInfo->prefixVltime;
               ipConn->X_BROADCOM_COM_IPv6PrefixDelegationEnabled = TRUE;
            }
            else
            {
               if ( cmsUtl_strcmp(pppConn->X_BROADCOM_COM_IPv6SitePrefix, dhcp6cInfo->sitePrefix) != 0 )
               {
                  CMSMEM_REPLACE_STRING(pppConn->X_BROADCOM_COM_IPv6SitePrefix, dhcp6cInfo->sitePrefix);
                  pdChange = 1;
               }

               if (dhcp6cInfo->prefixVltimeOld != 0)
               {
                  CMSMEM_REPLACE_STRING(pppConn->X_BROADCOM_COM_IPv6SitePrefixOld, dhcp6cInfo->sitePrefixOld);
                  pppConn->X_BROADCOM_COM_IPv6SitePrefixVltimeOld = dhcp6cInfo->prefixVltimeOld;
               }

               REPLACE_STRING_IF_NOT_EQUAL(pppConn->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED);
               pppConn->X_BROADCOM_COM_IPv6SitePrefixPltime = dhcp6cInfo->prefixPltime;
               pppConn->X_BROADCOM_COM_IPv6SitePrefixVltime = dhcp6cInfo->prefixVltime;
               pppConn->X_BROADCOM_COM_IPv6PrefixDelegationEnabled = TRUE;
            }
         }
         else  /* IFADDRCONF_REMOVE: prefix remove */
         {
            cmsLog_debug("remove prefix");

            if (ipConn)
            {
               CMSMEM_FREE_BUF_AND_NULL_PTR(ipConn->X_BROADCOM_COM_IPv6SitePrefix);
               CMSMEM_FREE_BUF_AND_NULL_PTR(ipConn->X_BROADCOM_COM_IPv6SitePrefixOld);
               ipConn->X_BROADCOM_COM_IPv6PrefixDelegationEnabled = FALSE;
            }
            else
            {
               CMSMEM_FREE_BUF_AND_NULL_PTR(pppConn->X_BROADCOM_COM_IPv6SitePrefix);
               CMSMEM_FREE_BUF_AND_NULL_PTR(pppConn->X_BROADCOM_COM_IPv6SitePrefixOld);
               pppConn->X_BROADCOM_COM_IPv6PrefixDelegationEnabled = FALSE;
            }
         }
      }

      /* dhcp6c receives dns information */
      if (dhcp6cInfo->dnsAssigned)
      {
         cmsLog_debug("assigned nameserver=%s", dhcp6cInfo->nameserver);
         if (ipConn)
         {
            REPLACE_STRING_IF_NOT_EQUAL(ipConn->X_BROADCOM_COM_IPv6DNSServers, dhcp6cInfo->nameserver);
         }
         else if (pppConn)
         {
            REPLACE_STRING_IF_NOT_EQUAL(pppConn->X_BROADCOM_COM_IPv6DNSServers, dhcp6cInfo->nameserver);
         }
      }

      /* dhcp6c receives domain name information */
      if ( dhcp6cInfo->domainNameAssigned && !IS_EMPTY_STRING(dhcp6cInfo->domainName) )
      {
         cmsLog_debug("assigned domain name=%s", dhcp6cInfo->domainName);
         if (ipConn)
         {
            REPLACE_STRING_IF_NOT_EQUAL(ipConn->X_BROADCOM_COM_IPv6DomainName, dhcp6cInfo->domainName);
         }
         else if (pppConn)
         {
            REPLACE_STRING_IF_NOT_EQUAL(pppConn->X_BROADCOM_COM_IPv6DomainName, dhcp6cInfo->domainName);
         }
      }

      /*
       * Even when dhcp6c is not used to get the WAN interface IP address,
       * use this message as a trigger to check the WAN interface IP.
       * Maybe we've been assigned an address by SLAAC.
       */
      if ((!dhcp6cInfo->addrAssigned) && 
          ((ipConn && !cmsUtl_strcmp(ipConn->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED)) ||
           (pppConn && !cmsUtl_strcmp(pppConn->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED)))
         )
      {
         char guAddr[CMS_IPADDR_LENGTH]={0};
         char guAddrPrefix[CMS_IPADDR_LENGTH]={0};
         UINT32 prefixLen=0;
         CmsRet r2;

         r2 = cmsNet_getGloballyUniqueIfAddr6(ifName, guAddr, &prefixLen);
         if (CMSRET_SUCCESS == r2)
         {
            sprintf(guAddrPrefix, "%s/%d", guAddr, prefixLen);
            cmsLog_debug("Detected GloballyUnique Addr6 %s, mark connection up!", guAddrPrefix);
            connected = TRUE;

            if (ipConn)
            {
               REPLACE_STRING_IF_NOT_EQUAL(ipConn->X_BROADCOM_COM_ExternalIPv6Address, guAddrPrefix);
            }
            else
            {
               REPLACE_STRING_IF_NOT_EQUAL(pppConn->X_BROADCOM_COM_ExternalIPv6Address, guAddrPrefix);
            }
         }
      }

      /* 
       * dhcp6c receives AFTR information
       * TODO: should we update aftr even WAN is not connected?
       */
      if ( connected && dhcp6cInfo->aftrAssigned && !IS_EMPTY_STRING(dhcp6cInfo->aftr) )
      {
         cmsLog_debug("assigned aftr=%s", dhcp6cInfo->aftr);

         /* update the DS-Lite tunnel configuration if there is any */
         if (ipConn)
         {
            dalTunnel_updateDSLiteObject( dhcp6cInfo->aftr, ipConn->X_BROADCOM_COM_IfName );
         }
         else if (pppConn)
         {
            dalTunnel_updateDSLiteObject( dhcp6cInfo->aftr, pppConn->X_BROADCOM_COM_IfName );
         }
      }

      if (ipConn)
      {
         ret = cmsObj_set(ipConn, &iidStack);
      }
      else
      {
         ret = cmsObj_set(pppConn, &iidStack);
      }
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_set returns error. ret=%d", ret);
      }

      if (connected)
      {
         cmsLog_debug("sending WAN_CONNECTION_UP msg for IPv6 connection %s", ifName);

         /* Address assign to PD interface: TODO-> hard coded to br0!!*/
         if ( pdChange && !IS_EMPTY_STRING(dhcp6cInfo->pdIfAddress) )
         {
            cmsLog_debug("delegate LAN IPv6 address: %s", dhcp6cInfo->pdIfAddress);

            if ( (ret = dalWan_addDelegatedAddrEntry(ifName, dhcp6cInfo->pdIfAddress, "br0", MDMVS_WANDELEGATED)) != CMSRET_SUCCESS )
            {
               cmsLog_error("prefix assignment fails at address assignment on PD interface!!");
            }
         }
         else
         {
            if ( pdChange == 0 )
            {
               cmsLog_debug("prefix assignment remains the same");
            }
            else
            {
               cmsLog_error("prefix assignment changes for PD interface without providing pdIfAddr");
            }
         }

         sendStatusMsgToSmd(CMS_MSG_WAN_CONNECTION_UP, ifName);
         
#if defined(BRCM_VOICE_SUPPORT) && VOICE_IPV6_SUPPORT
         if ( isIpv6 && (isVoiceOnAnyWan || voiceWanIfName) )
         {
            if (ipConn)
            {
               initVoiceOnWanIntfLocked(ipConn->X_BROADCOM_COM_IfName, ipConn->X_BROADCOM_COM_ExternalIPv6Address);
            }
            else
            {
               initVoiceOnWanIntfLocked(pppConn->X_BROADCOM_COM_IfName, pppConn->X_BROADCOM_COM_ExternalIPv6Address);
            }
         }
#endif		 
      }
   }

   cmsObj_free((void **) &ipConn);
   cmsObj_free((void **) &pppConn);

#ifdef DMP_X_ITU_ORG_GPON_1
   if (!found)
   {
      BcmOmciRtdIpv6HostConfigDataObject *ipHost = NULL;
      BcmOmciRtdIpv6HostConfigDataExtObject *bcmIpHost = NULL;

      INIT_INSTANCE_ID_STACK(&iidStack);

      while ((!found) &&
             ((ret = cmsObj_getNext(MDMOID_BCM_OMCI_RTD_IPV6_HOST_CONFIG_DATA_EXT,
                                   &iidStack, (void **) &bcmIpHost))
              == CMSRET_SUCCESS))
      {
        // look for Broadcom IPv6 Host that has its
        // dhcpPid match with the pid in the message
        found = (bcmIpHost->dhcpcPid == pid);
        if (found == TRUE)
        {
          InstanceIdStack iidIpHost = EMPTY_INSTANCE_ID_STACK;
          UBOOL8 found_iphost = FALSE;

          // look for IP Host that has its managedEntityId
          // match with managedEntityId of Broadcom IP Host
          while (!found_iphost &&
                 cmsObj_getNext(MDMOID_BCM_OMCI_RTD_IPV6_HOST_CONFIG_DATA,
                                &iidIpHost, (void **) &ipHost) == CMSRET_SUCCESS)
          {
            if (ipHost->managedEntityId == bcmIpHost->managedEntityId)
            {
               found_iphost = TRUE;
            }

            cmsLog_debug("found_iphost<%d>", found_iphost);

            if (found_iphost)
            {
               if (dhcp6cInfo->addrAssigned)
               {
                  if (dhcp6cInfo->addrCmd == IFADDRCONF_ADD)
                  {
                     struct in6_addr in6;
                     char *tmpStr = NULL;
                     char *separator = NULL;
                     UINT8 addrInfo[24];
                     UINT8 lifeTime[8] = {0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F};

                     cmsLog_debug("assigned IPv6 address");

                     separator = strstr(dhcp6cInfo->address, "/");
                     if (separator)
                     {
                        *separator = '\0';
                     }

                     inet_pton(AF_INET6, dhcp6cInfo->address, &in6);
                     memcpy(&addrInfo[0], (void *)&in6, 16);
                     // TODO: plt and vlt
                     memcpy(&addrInfo[16], lifeTime, 8);

                     cmsUtl_binaryBufToHexString(addrInfo, 24, &tmpStr);
                     rutOmci_addIpv6TableAttribute(MDMOID_BCM_OMCI_RTD_IPV6_CURRENT_ADDRESS_TABLE, &iidIpHost, tmpStr);
                     CMSMEM_FREE_BUF_AND_NULL_PTR(tmpStr);
                  }
                  else  /* IFADDRCONF_REMOVE */
                  {
                     cmsLog_debug("remove IPv6 address");
                     // TODO: only delete the correct address entry, now we flush all addr
                     rutOmci_clearTableAttribute(MDMOID_BCM_OMCI_RTD_IPV6_CURRENT_ADDRESS_TABLE, &iidIpHost);
                  }
               }

               /* dhcp6c receives dns information */
               if (dhcp6cInfo->dnsAssigned)
               {
                  char dnsPri[CMS_IPADDR_LENGTH];
                  char dnsSec[CMS_IPADDR_LENGTH];

                  dnsPri[0] = '\0';
                  dnsSec[0] = '\0';

                  cmsLog_debug("assigned nameserver=%s", dhcp6cInfo->nameserver);
                  if (cmsUtl_parseDNS(dhcp6cInfo->nameserver, dnsPri, dnsSec, FALSE) 
                                      == CMSRET_SUCCESS)
                  {
                     struct in6_addr in6;
                     char *tmpStr = NULL;

                     cmsLog_debug("dns servers, dns1=%s, dns2=%s", dnsPri, dnsSec);
//                     rutGpon_configDns(dnsPri, dnsSec);  //TODO: for router case??

                     inet_pton(AF_INET6, dnsPri, &in6);
                     cmsUtl_binaryBufToHexString((UINT8 *)&in6, 16, &tmpStr);
                     rutOmci_addIpv6TableAttribute(MDMOID_BCM_OMCI_RTD_IPV6_CURRENT_DNS_TABLE, &iidIpHost, tmpStr);
                     CMSMEM_FREE_BUF_AND_NULL_PTR(tmpStr);

                     if (dnsSec[0] != '\0')
                     {
                        inet_pton(AF_INET6, dnsSec, &in6);
                        cmsUtl_binaryBufToHexString((UINT8 *)&in6, 16, &tmpStr);
                        rutOmci_addIpv6TableAttribute(MDMOID_BCM_OMCI_RTD_IPV6_CURRENT_DNS_TABLE, &iidIpHost, tmpStr);
                        CMSMEM_FREE_BUF_AND_NULL_PTR(tmpStr);
                     }
                  }
                  else
                  {
                     cmsLog_debug("failed parsing dns server");
                  }
               }

               /* update ipHost->currentXXXTables */
               cmsObj_set(ipHost, &iidIpHost);
               cmsObj_free((void **)&ipHost);

               /* Send Wan Connection Up event msg */
               if (dhcp6cInfo->addrAssigned && (dhcp6cInfo->addrCmd == IFADDRCONF_ADD))
               {
                   ManagementServerObject *acsCfg = NULL;
                   InstanceIdStack iidManServer = EMPTY_INSTANCE_ID_STACK;
                   if (cmsObj_get(MDMOID_MANAGEMENT_SERVER, 
                                  &iidManServer, 0, (void *) &acsCfg) == CMSRET_SUCCESS)
                   {
                       if(cmsUtl_strcmp(acsCfg->X_BROADCOM_COM_BoundIfName, bcmIpHost->interfaceName) != 0)
                       {
                           CMSMEM_REPLACE_STRING(acsCfg->X_BROADCOM_COM_BoundIfName, bcmIpHost->interfaceName);

                           cmsObj_set(acsCfg, &iidManServer);
                       }
                       cmsObj_free((void **) &acsCfg);
                   }

                   //Need to change bcmIpHost->connectionStatus after set ipHost->currentxxx
                   bcmIpHost->connectionStatus = 2;   // Connected
                   cmsObj_set(bcmIpHost, &iidStack);

                   cmsLog_debug("Send CMS_MSG_WAN_CONNECTION_UP, ifName=%s", bcmIpHost->interfaceName);
                   sendStatusMsgToSmd(CMS_MSG_WAN_CONNECTION_UP, bcmIpHost->interfaceName);
               }
            }
          }
        }
        cmsObj_free((void **)&bcmIpHost);
      }

      if (strlen(dhcp6cInfo->acsURL) > 0 ||
          strlen(dhcp6cInfo->acsProvisioningCode) > 0 ||
          dhcp6cInfo->cwmpRetryMinimumWaitInterval != 0 ||
          dhcp6cInfo->cwmpRetryIntervalMultiplier != 0)
      {
         cmsLog_debug("got acsURL=%s provisioningCode=%s MinWaitInterval=%d intMult=%d",
                      dhcp6cInfo->acsURL,
                      dhcp6cInfo->acsProvisioningCode,
                      dhcp6cInfo->cwmpRetryMinimumWaitInterval,
                      dhcp6cInfo->cwmpRetryIntervalMultiplier);
#ifdef SUPPORT_TR69C
         setAcsParams(dhcp6cInfo->acsURL, dhcp6cInfo->acsProvisioningCode,
                      dhcp6cInfo->cwmpRetryMinimumWaitInterval,
                      dhcp6cInfo->cwmpRetryIntervalMultiplier);
#endif
      }

#if defined (DMP_TIME_1) || defined (DMP_DEVICE2_TIME_1)
      if (strlen(dhcp6cInfo->ntpserver) > 0)
      {
         cmsLog_debug("got ntpserver(s)=%s", dhcp6cInfo->ntpserver);
         setNtpServers(dhcp6cInfo->ntpserver);
      }
#endif /* DMP_TIME_1 or DMP_DEVICE2_TIME_1 */
   }
#endif
   cmsLck_releaseLock();
   return;

}  /* End of processDhcp6cStateChanged() */

void processRAStatus6Info_igd(const CmsMsgHeader *msg)
{
   CmsRet ret;
   RAStatus6MsgBody *raInfo = (RAStatus6MsgBody *) (msg + 1);
   WanIpConnObject  *ipConn  = NULL;
   WanPppConnObject *pppConn = NULL;
   char ifName[CMS_IFNAME_LENGTH]={0};
   InstanceIdStack iidStack;
   UBOOL8 found;

   cmsLog_debug("PIO with L<%d> A<%d> plt<%u> vlt<%u> prefix<%s/%u>", 
                raInfo->pio_L_flag, raInfo->pio_A_flag, raInfo->pio_plt, 
                raInfo->pio_vlt, raInfo->pio_prefix, raInfo->pio_prefixLen);

   cmsLog_debug("dns with servers<%s>, lifetime<%d>", 
                raInfo->dns_servers, raInfo->dns_lifetime);

   cmsLog_debug("domain with domain<%s>, lifetime<%d>", 
                raInfo->domainName, raInfo->domainName_lifetime);

   cmsLog_debug("gwAddr<%s> rtrLife<%d> M<%d> O<%d> ifName<%s>", 
                raInfo->router, raInfo->router_lifetime, raInfo->router_M_flags, 
                raInfo->router_O_flags, raInfo->ifName);
#ifdef DMP_DEVICE2_HOMEPLUG_1
   /* HomePlug devices needs to monitor RA at LAN network */
#else
   /* No need to monitor RA at LAN network */
   if (cmsUtl_strcmp(raInfo->ifName, "br0") == 0)
   {
      return;
   }
#endif
   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d, ramonitor update dropped!", ret);
      return;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   found = FALSE;

   /* Search the WAN connection object */
   while ((!found) && 
          ((ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **)&ipConn)) == CMSRET_SUCCESS))
   {
      if ((strcmp(ipConn->connectionType, MDMVS_IP_ROUTED) == 0) &&
          (!cmsUtl_strcmp(ipConn->X_BROADCOM_COM_IfName, raInfo->ifName)))
      {
         found = TRUE;
         cmsUtl_strncpy(ifName, ipConn->X_BROADCOM_COM_IfName, sizeof(ifName));
      }
      else
      {
         cmsObj_free((void **) &ipConn);
      }            
   }

   if (!found)
   {
      ipConn = NULL;
      INIT_INSTANCE_ID_STACK(&iidStack);

      while ((!found) && 
             ((ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **)&pppConn)) == CMSRET_SUCCESS))
      {
         if (!cmsUtl_strcmp(pppConn->X_BROADCOM_COM_IfName, raInfo->ifName))
         {
            found = TRUE;
            cmsUtl_strncpy(ifName, pppConn->X_BROADCOM_COM_IfName, sizeof(ifName));
         }
         else
         {
            cmsObj_free((void **) &pppConn);
         }            
      }

      if (!found)
      {
         pppConn = NULL;
      }
   }

   if (found)
   {
      if (ipConn)
      {
         ipConn->X_BROADCOM_COM_MFlag = (raInfo->router_M_flags?1:0);
         ipConn->X_BROADCOM_COM_OFlag = (raInfo->router_O_flags?1:0);
         ipConn->X_BROADCOM_COM_AFlag = (raInfo->pio_A_flag?1:0);
         ipConn->X_BROADCOM_COM_LFlag = (raInfo->pio_L_flag?1:0);

         if (raInfo->router_lifetime)
         {
            REPLACE_STRING_IF_NOT_EQUAL(ipConn->X_BROADCOM_COM_DefaultIPv6Gateway, raInfo->router);
         }
         else
         {
            CMSMEM_FREE_BUF_AND_NULL_PTR(ipConn->X_BROADCOM_COM_DefaultIPv6Gateway);
         }

         if (raInfo->dns_lifetime)
         {
            REPLACE_STRING_IF_NOT_EQUAL(ipConn->X_BROADCOM_COM_IPv6DNSServers, raInfo->dns_servers);
         }

         ret = cmsObj_set(ipConn, &iidStack);
      }
      else
      {
         pppConn->X_BROADCOM_COM_MFlag = (raInfo->router_M_flags?1:0);
         pppConn->X_BROADCOM_COM_OFlag = (raInfo->router_O_flags?1:0);
         pppConn->X_BROADCOM_COM_AFlag = (raInfo->pio_A_flag?1:0);
         pppConn->X_BROADCOM_COM_LFlag = (raInfo->pio_L_flag?1:0);

         if (raInfo->router_lifetime)
         {
            REPLACE_STRING_IF_NOT_EQUAL(pppConn->X_BROADCOM_COM_DefaultIPv6Gateway, raInfo->router);
         }
         else
         {
            CMSMEM_FREE_BUF_AND_NULL_PTR(pppConn->X_BROADCOM_COM_DefaultIPv6Gateway);
         }

         if (raInfo->dns_lifetime)
         {
            REPLACE_STRING_IF_NOT_EQUAL(pppConn->X_BROADCOM_COM_IPv6DNSServers, raInfo->dns_servers);
         }

         ret = cmsObj_set(pppConn, &iidStack);
      }
   }

   cmsObj_free((void **) &ipConn);
   cmsObj_free((void **) &pppConn);

#ifdef DMP_X_ITU_ORG_GPON_1
   if (!found)
   {
      UBOOL8 found = FALSE;
      BcmOmciRtdIpv6HostConfigDataObject *ipHost = NULL;
      BcmOmciRtdIpv6HostConfigDataExtObject *bcmIpHost = NULL;

      INIT_INSTANCE_ID_STACK(&iidStack);

      while ((!found) &&
             (cmsObj_getNext(MDMOID_BCM_OMCI_RTD_IPV6_HOST_CONFIG_DATA_EXT,
                             &iidStack, (void **) &bcmIpHost) == CMSRET_SUCCESS))
      {
         found = (cmsUtl_strncmp(raInfo->ifName, bcmIpHost->interfaceName, BUFLEN_32) == 0);
         if (found == TRUE)
         {
            InstanceIdStack iidIpHost = EMPTY_INSTANCE_ID_STACK;
            UBOOL8 found_iphost = FALSE;
            UBOOL8 getAddr = FALSE;

            // look for IP Host that has its managedEntityId
            // match with managedEntityId of Broadcom IP Host
            while (!found_iphost &&
                   cmsObj_getNext(MDMOID_BCM_OMCI_RTD_IPV6_HOST_CONFIG_DATA,
                                  &iidIpHost, (void **) &ipHost) == CMSRET_SUCCESS)
            {
               if (ipHost->managedEntityId == bcmIpHost->managedEntityId)
               {
                  found_iphost = TRUE;
               }
               else
               {
                  cmsObj_free((void **)&ipHost);
               }
            }

            // set m_flag and o_flag
            bcmIpHost->MFlag = raInfo->router_M_flags;
            bcmIpHost->OFlag = raInfo->router_O_flags;

            if (found_iphost)
            {
               // must add gateway info
               struct in6_addr in6;
               char *tmpStr = NULL;

               cmsLog_debug("gateway info");

               inet_pton(AF_INET6, raInfo->router, &in6);
               cmsUtl_binaryBufToHexString((UINT8 *)&in6, 16, &tmpStr);
               rutOmci_addIpv6TableAttribute(MDMOID_BCM_OMCI_RTD_IPV6_CURRENT_DEFAULT_ROUTER_TABLE, &iidIpHost, tmpStr);
               CMSMEM_FREE_BUF_AND_NULL_PTR(tmpStr);

               // set prefix info
               if (raInfo->pio_plt != 0)
               {
                  struct in6_addr in6;
                  char *tmpStr = NULL;
                  UINT8 prefixInfo[26];
                  UINT8 lifeTime[8] = {0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F};

                  cmsLog_debug("assigned IPv6 prefix info");

                  inet_pton(AF_INET6, raInfo->pio_prefix, &in6);

                  prefixInfo[0] = raInfo->pio_prefixLen;
                  prefixInfo[1] = raInfo->pio_A_flag;
                  memcpy(&prefixInfo[2], (void *)&in6, 16);
                  // TODO: plt and vlt
                  memcpy(&prefixInfo[2+16], lifeTime, 8);

                  cmsUtl_binaryBufToHexString(prefixInfo, 26, &tmpStr);
                  rutOmci_addIpv6TableAttribute(MDMOID_BCM_OMCI_RTD_IPV6_CURRENT_ONLINK_PREFIX_TABLE, &iidIpHost, tmpStr);
                  CMSMEM_FREE_BUF_AND_NULL_PTR(tmpStr);
               }

               if (ipHost->ipOptions & 0x02)
               {
                  // set address info
                  if (raInfo->pio_A_flag != 0)
                  {
                     UINT8 macAddr[6];
                     UINT8 *buf = NULL;
                     UINT32 size = 0;
                     char addr[CMS_IPADDR_LENGTH];

                     cmsLog_debug("assigned IPv6 addr");

                     cmsUtl_hexStringToBinaryBuf(ipHost->macAddress, &buf, &size);

                     if (size != 6)
                     {
                        cmsLog_debug("macAddress is not 6 byte??");
                     }
                     else
                     {
                        memcpy(&macAddr[0], &buf[0], 6);
                     }
                     cmsMem_free(buf);

                     if (cmsUtl_prefixMacToAddress(raInfo->pio_prefix, macAddr, addr) 
                                                  == CMSRET_SUCCESS)
                     {
                        struct in6_addr in6;
                        char *tmpStr = NULL;
                        UINT8 addrInfo[24];
                        UINT8 lifeTime[8] = {0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F};

                        inet_pton(AF_INET6, addr, &in6);
                        memcpy(&addrInfo[0], (void *)&in6, 16);
                        // TODO: plt and vlt
                        memcpy(&addrInfo[16], lifeTime, 8);

                        cmsUtl_binaryBufToHexString(addrInfo, 24, &tmpStr);
                        rutOmci_addIpv6TableAttribute(MDMOID_BCM_OMCI_RTD_IPV6_CURRENT_ADDRESS_TABLE, &iidIpHost, tmpStr);
                        CMSMEM_FREE_BUF_AND_NULL_PTR(tmpStr);

                        getAddr = TRUE;
                     }
                     else
                     {
                        cmsLog_debug("cannot generate address with ra");
                     }
                  }

                  // set dns info
                  if (raInfo->dns_lifetime != 0)
                  {
                     cmsLog_debug("assigned IPv6 dns");
 
                     // TODO: update currentDnsTable obj
                  }

                  // set domainName info
                  if (raInfo->domainName_lifetime != 0)
                  {
                     cmsLog_debug("assigned IPv6 domainName");
 
                     // TODO: update currentDomainNameTable obj
                  }
               }

               /* update ipHost->currentXXXTables */
               cmsObj_set(ipHost, &iidIpHost);
               cmsObj_free((void **)&ipHost);

               if (getAddr)
               {
                  //Need to change bcmIpHost->connectionStatus after set ipHost->currentxxx
                  bcmIpHost->connectionStatus = 2;   // Connected
                  cmsObj_set(bcmIpHost, &iidStack);

                  sendStatusMsgToSmd(CMS_MSG_WAN_CONNECTION_UP, bcmIpHost->interfaceName);
               }
            }
            else
            {
               cmsLog_error("cannot find ipv6hostObj");
            }

            if (!getAddr)
            {
               cmsObj_set(bcmIpHost, &iidStack);
            }
         }
         cmsObj_free((void **)&bcmIpHost);
      }
   }
#endif
   cmsLck_releaseLock();
}
#endif  /* DMP_X_BROADCOM_COM_IPV6_1 */



#if defined(DMP_DEVICE2_BASELINE_1) && defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)

static UBOOL8 getDev2PppFromIgdPpp(const InstanceIdStack *igdPppIidStack,
                                   InstanceIdStack *dev2PppIidStack)
{
   MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
   char *igdPppFullPath=NULL;
   UBOOL8 found;
   CmsRet ret;

   /* convert the TR98/IGD PPP object to fullpath */
   pathDesc.oid = MDMOID_WAN_PPP_CONN;
   pathDesc.iidStack = *igdPppIidStack;
   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &igdPppFullPath)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      return FALSE;
   }


   /* Use the interface stack to find the DEV2 PPP object above the
    * TR98/IGD PPP object.
    */
   INIT_PATH_DESCRIPTOR(&pathDesc);
   found = getUpperLayerPathDescFromLowerLayerLocked(igdPppFullPath,
                                                     MDMOID_DEV2_PPP_INTERFACE,
                                                     &pathDesc);
   if (!found)
   {
      cmsLog_error("Could not find DEV2_PPP object above %s", igdPppFullPath);
   }
   else
   {
      *dev2PppIidStack = pathDesc.iidStack;
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(igdPppFullPath);

   return found;
}
#endif  /* DMP_DEVICE2_BASELINE_1 && DMP_X_BROADCOM_COM_DEV2_IPV6_1 */


void processPppStateChanged_igd(CmsMsgHeader *msg)
{
   PppoeStateChangeMsgBody *pppInfo = (PppoeStateChangeMsgBody *) (msg + 1);
   WanPppConnObject *pppConn=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   UINT32 pid;
   CmsRet ret;
#ifdef DMP_X_BROADCOM_COM_L2TPAC_1    
   char cmd[BUFLEN_128];
#endif   
#if defined(BRCM_VOICE_SUPPORT) && VOICE_IPV6_SUPPORT
   int isIpv6 = 0;
   char ipAddrFamily[BUFLEN_16];
   unsigned int length = 16;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
       cmsLog_error("failed to get lock, ret=%d", ret);
       return;
   }
   ret = dalVoice_GetIpFamily( NULL, ipAddrFamily, length );
   cmsLck_releaseLock();
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get the IP address family for voice, ret=%d", ret);
      return;
   }
   else
   {
      isIpv6 = !(cmsUtl_strcmp( ipAddrFamily, MDMVS_IPV6 ));
   }   
#endif

   pid = PID_IN_EID(msg->src);

   cmsLog_debug("ppp pid=%d dataLength=%d state=%d, lastConnectionError=%s", 
      pid, msg->dataLength, pppInfo->pppState, pppInfo->ppplastconnecterror);
      
   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d, ppp connection state update dropped!", ret);
      /* hmm, this is bad, I can't update the MDM with new pppd info */
      return;
   }

   while ((!found) && 
          ((ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConn)) == CMSRET_SUCCESS))
   {
#ifdef DMP_X_BROADCOM_COM_L2TPAC_1      
      //find out the pid of the pppd which was launched by openl2tpd
      if(!cmsUtl_strcmp(pppConn->name, "PPPoL2tpAc") && pid > 0)
      {
         sprintf(cmd, "echo %d > %s", pid, PPPoL2TPAC_PID_FILENAME);
         prctl_runCommandInShellBlocking(cmd);
      }	
#endif      	  
      if ((!cmsUtl_strcmp(pppConn->connectionType, MDMVS_IP_ROUTED)) &&
          (pppConn->X_BROADCOM_COM_PppdPid == pid))
      {
         /*
          * We know this this the correct PPPConn object because we've
          * matched up the pid.
          */
         found = TRUE;
      }
      else
      {
         /*
          * This instance of the pppConn object is not the one we want.
          * free it and go back up to the while loop to get the next instance.
          */
         cmsObj_free((void **) &pppConn);
      }            
   }

   if (!found)
   {
      cmsLog_debug("No pppoe connection found!");
      cmsLck_releaseLock();
      return;
   }

   /* lastConnectionError is alway set even there is no error */
   CMSMEM_REPLACE_STRING(pppConn->lastConnectionError, pppInfo->ppplastconnecterror);    

   switch(pppInfo->pppState) 
   {

   case BCM_PPPOE_CLIENT_STATE_PADO:
   case BCM_PPPOE_CLIENT_STATE_PADS:
   case BCM_PPPOE_CLIENT_STATE_CONFIRMED:
   case BCM_PPPOE_SERVICE_AVAILABLE:
      cmsLog_debug("waiting for ppp to come up; current state=%d", pppInfo->pppState);
      break;

   case BCM_PPPOE_CLIENT_STATE_DOWN:

      /*
       * Free up as memory as possible before we go into the RCL, which will
       * do lots of insmod and iptables actions.  We need to do this on low
       * memory systems to prevent thrashing of memory.
       */
      if (isLowMemorySystem)
      {
         prctl_runCommandInShellBlocking("echo 3 > /proc/sys/vm/drop_caches");
      }

      CMSMEM_REPLACE_STRING(pppConn->connectionStatus, MDMVS_DISCONNECTED);
      if ((ret = cmsObj_set(pppConn, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not set pppConn to DISCONNECTED state, ret=%d", ret);
      }
      break;

   case BCM_PPPOE_CLIENT_STATE_UP:
      {
         /* record the Connection Establish time */
         pppConn->X_BROADCOM_COM_ConnectionEstablishedTime = cmsTms_getSeconds();

         CMSMEM_REPLACE_STRING(pppConn->externalIPAddress, pppInfo->ip);
         CMSMEM_REPLACE_STRING(pppConn->X_BROADCOM_COM_DefaultGateway, pppInfo->gateway);
         CMSMEM_REPLACE_STRING(pppConn->remoteIPAddress, pppInfo->gateway);
         CMSMEM_REPLACE_STRING(pppConn->DNSServers, pppInfo->nameserver);
         
         if (pppConn->externalIPAddress == NULL ||
            pppConn->X_BROADCOM_COM_DefaultGateway == NULL ||
            pppConn->DNSServers == NULL ||
            pppConn->lastConnectionError == NULL)
    
         {
            cmsLog_error("one or more allocations in pppConn failed");
            cmsObj_free((void **) &pppConn);
            cmsLck_releaseLock();
            return;
         }
      
         cmsLog_debug("assigned ip=%s gateway=%s nameserver=%s lastConnectionError=%s servicename=%s",
                     pppConn->externalIPAddress,
                     pppConn->X_BROADCOM_COM_DefaultGateway,
                     pppConn->DNSServers,
                     pppConn->lastConnectionError,
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

         /* set the ppp object to CONNECTED status */
         CMSMEM_REPLACE_STRING(pppConn->connectionStatus, MDMVS_CONNECTED);
         
         
#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1
            if (isAutoDetectionEnabled)
            {
               pppConn->X_BROADCOM_COM_LastConnected = TRUE;
            }
            else
            {
               pppConn->X_BROADCOM_COM_LastConnected = FALSE;
            }
#endif 

         if ((ret = cmsObj_set(pppConn, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("set of pppConn failed, ret=%d", ret);
         }

         /* Send Wan Connection Up event msg */
         sendStatusMsgToSmd(CMS_MSG_WAN_CONNECTION_UP, pppConn->X_BROADCOM_COM_IfName);

#ifdef BRCM_VOICE_SUPPORT
#if VOICE_IPV6_SUPPORT
         if ( !isIpv6 )
#endif
         {
            if ( isVoiceOnAnyWan || voiceWanIfName )
            {
               initVoiceOnWanIntfLocked(pppConn->X_BROADCOM_COM_IfName, pppConn->externalIPAddress);
            }
         }
#endif
      }
          
      break;

#ifdef SUPPORT_IPV6
   case BCM_PPPOE_CLIENT_IPV6_STATE_UP:
      {
         /* TODO: record the Connection Establish time only for IPv4 */
         cmsLog_debug("IPv6CP is up: local<%s> remote<%s>", pppInfo->localIntfId, pppInfo->remoteIntfId);

         /*
          * Free up as memory as possible before we go into the RCL, which will
          * do lots of insmod and iptables actions.  We need to do this on low
          * memory systems to prevent thrashing of memory.
          */
         if (isLowMemorySystem)
         {
            prctl_runCommandInShellBlocking("echo 3 > /proc/sys/vm/drop_caches");
         }

#ifdef DMP_X_BROADCOM_COM_IPV6_1
         /*
          * For static connections, connection status is directly set to "Connected" state.
         */
         if (!cmsUtl_strcmp(pppConn->X_BROADCOM_COM_IPv6AddressingType, MDMVS_STATIC))
         {
            cmsLog_debug("PPP in static mode");
                  
            CMSMEM_REPLACE_STRING(pppConn->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED);

#if defined(BRCM_VOICE_SUPPORT) && VOICE_IPV6_SUPPORT
            if ( isIpv6 && (isVoiceOnAnyWan || voiceWanIfName) )
            {
               initVoiceOnWanIntfLocked(pppConn->X_BROADCOM_COM_IfName, pppConn->X_BROADCOM_COM_ExternalIPv6Address);
            }
#endif		 
         }
         else
         {
            CMSMEM_REPLACE_STRING(pppConn->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTING);
         }
         /* set the ppp X_BROADCOM_COM_IPv6PppUp to true */
         pppConn->X_BROADCOM_COM_IPv6PppUp = TRUE;

         if ((ret = cmsObj_set(pppConn, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("set of pppConn failed, ret=%d", ret);
         }

         /* TODO: update WAN IPv6 Connection  */
#endif //DMP_X_BROADCOM_COM_IPV6_1

#if defined(DMP_DEVICE2_BASELINE_1) && defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)
         {
            InstanceIdStack dev2PppIidStack = EMPTY_INSTANCE_ID_STACK;
            UBOOL8 found;

            found = getDev2PppFromIgdPpp(&iidStack, &dev2PppIidStack);
            if (!found)
            {
               cmsLog_error("Could not find DEV2_PPP object above WANPPPConn");
            }
            else
            {
               /* do the actual work in a common function */
               processPppv6StateUp_dev2(&dev2PppIidStack, pppInfo->localIntfId, pppInfo->remoteIntfId);
            }
         }
#endif  /* DMP_DEVICE2_BASELINE_1 && DMP_X_BROADCOM_COM_DEV2_IPV6_1 */

      }
      break;


   case BCM_PPPOE_CLIENT_IPV6_STATE_DOWN:
      cmsLog_debug("IPv6CP is down!");

#ifdef DMP_X_BROADCOM_COM_IPV6_1
      CMSMEM_REPLACE_STRING(pppConn->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_DISCONNECTED);
      pppConn->X_BROADCOM_COM_IPv6PppUp = FALSE;

      if ((ret = cmsObj_set(pppConn, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not set pppConn to DISCONNECTED state, ret=%d", ret);
      }
#endif

#if defined(DMP_DEVICE2_BASELINE_1) && defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)
      {
         InstanceIdStack dev2PppIidStack = EMPTY_INSTANCE_ID_STACK;
         UBOOL8 found;

         found = getDev2PppFromIgdPpp(&iidStack, &dev2PppIidStack);
         if (!found)
         {
            cmsLog_error("Could not find DEV2_PPP object above WANPPPConn");
         }
         else
         {
            /* do the actual work in a common function */
            processPppv6StateDown_dev2(&dev2PppIidStack);


#if 0  //FIXME: how to stop pppd for IPv6 only case? cannot call rut function here (maybe set connectionstatus in wanpppconn obj?)
            if (!rutWan_isTransientLayer2LinkUp(pppConn->X_BROADCOM_COM_TransientLayer2LinkStatus) || STOP_PPPD(pppConn))
            {
               if ((ret = rutCfg_stopWanPppConnection(iidStack, pppConn, TRUE)) == CMSRET_SUCCESS)
               {
                  cmsLog_debug("rutCfg_stopWanPppConnection ok.");
               }
               else
               {
                  cmsLog_error("rutCfg_stopWanPppConnection failed, error %d", ret);
               }

#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1          
               /* reset it to FALSE */
               pppConn->X_BROADCOM_COM_StopPppD = FALSE;
#endif /* DMP_X_BROADCOM_COM_AUTODETECTION_1 */   
            }
#endif  /* 0 */
         }
      }
#endif /* DMP_DEVICE2_BASELINE_1 && DMP_X_BROADCOM_COM_DEV2_IPV6_1 */

      break;

#endif  //SUPPORT_IPV6


   case BCM_PPPOE_AUTH_FAILED:
   
     /* For authentification failed, connectionStatus is DISCONNECED and lastConnectionError is already
      * set by pppd to MDMVS_ERROR_AUTHENTICATION_FAILURE
      */
      CMSMEM_REPLACE_STRING(pppConn->connectionStatus, MDMVS_DISCONNECTED);
      
      if ((ret = cmsObj_set(pppConn, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not set pppConn to MDMVS_DISCONNECTED state, ret=%d", ret);
      }
      break;

   /*
   * Just update the lastConnectionError field - used by tr69 and ppp on demand.
   */
   case BCM_PPPOE_REPORT_LASTCONNECTERROR:
   
      CMSMEM_REPLACE_STRING(pppConn->lastConnectionError, pppInfo->ppplastconnecterror);
      if ((ret = cmsObj_set(pppConn, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not set pppConn lastConnectionError, ret=%d", ret);
      }
      break;
      
   default:
      cmsLog_error("Unsupported state=%d", pppInfo->pppState);
      break;
   }

   cmsLog_debug("Current state: %s", pppConn->connectionStatus);

   cmsObj_free((void **) &pppConn);
 
   cmsLck_releaseLock();

   /*
    * Now that ssk has updated the data model (and the RCL handler functions
    * had a chance to run), send this event to smd for general distribution.
    * Most apps will not need to subscribe to the PPP state changed event.
    * It makes more sense for them to subscribe to the WAN_CONNECTION_UP event.
    */
   msg->dst = EID_SMD;
   if ((ret = cmsMsg_send(msgHandle, msg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to forward ppp state changed message, ret=%d", ret);
   }
   
   return;
}


void processRequestPppChange_igd(const CmsMsgHeader *msg)
{
   char *pppIfName = (char *) (msg + 1);
   WanPppConnObject *pppConn=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   UBOOL8 change = FALSE;
   CmsRet ret;


   cmsLog_debug("ppp request state =%d dataLength=%d intfName =%s", 
      msg->wordData, msg->dataLength, pppIfName);
      
   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   while (!found && 
      cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **)&pppConn) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(pppConn->X_BROADCOM_COM_IfName, pppIfName))
   	{
   	   found = TRUE;
   	   break;
      }
      cmsObj_free((void **)&pppConn);
   }

   if (!found)
   {
      cmsLog_error("No ppp connection object found!");
      cmsLck_releaseLock();
      return;
   }

   /* for user request pppd down if in  the "Connected" connectionStatus */
   if (msg->wordData == USER_REQUEST_CONNECTION_DOWN && 
      !cmsUtl_strcmp(pppConn->connectionStatus, MDMVS_CONNECTED))
   {
   
      CMSMEM_REPLACE_STRING(pppConn->X_BROADCOM_COM_UserRequest, MDMVS_DOWN);
      change = TRUE;
   }
   else if (msg->wordData == USER_REQUEST_CONNECTION_UP && 
      !cmsUtl_strcmp(pppConn->connectionStatus, MDMVS_DISCONNECTED))
   {
      /* for user request pppd up if  in the "Disconnected" connectionStatus */
      CMSMEM_REPLACE_STRING(pppConn->X_BROADCOM_COM_UserRequest, MDMVS_UP);
      change = TRUE;
   }
   
   if (change)
   {
      if ((ret = cmsObj_set(pppConn, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not set pppConn for user request. ret=%d", ret);
      }
   }
   else
   {
      cmsLog_debug("No need to change. user request=%d, connectStatus=%s", 
         msg->wordData, pppConn->X_BROADCOM_COM_UserRequest);
   }      

   cmsObj_free((void **)&pppConn);
   
   cmsLck_releaseLock();

   return;
   
}


CmsRet processWatchWanConnectionMsg(CmsMsgHeader *msg)
{
   CmsRet ret = CMSRET_SUCCESS;
   WatchedWanConnection *info = (WatchedWanConnection *) (msg + 1);
   void *wanConnObj=NULL;
   UBOOL8 wanLinkUp;
   
   if (msg->dataLength != sizeof(WatchedWanConnection))
   {
      cmsLog_error("Wrong msg! dataLength=%d expected=%d", msg->dataLength, sizeof(WatchedWanConnection));
      return CMSRET_INTERNAL_ERROR;
   }

   cmsLog_debug("dataLen=%d, oid=%d, info->iidStack=%s, isAdd=%d, isStatic=%d, isDeleted=%d",
      msg->dataLength, info->oid, cmsMdm_dumpIidStack(&(info->iidStack)), info->isAdd, info->isStatic, info->isDeleted);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT*2)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d, unable to watch wan connection!", ret);
      return ret;
   }

   if (info->isAdd == TRUE && (info->oid == MDMOID_WAN_IP_CONN || info->oid == MDMOID_WAN_PPP_CONN))
   {
      wanLinkUp = dalWan_isWanLayer2LinkUp(info->oid, &(info->iidStack));

      /*
       * if WAN link is already up, move the state machine along for this
       * WANIP or WANPPP connection object.  If wan link is not up yet, no
       * action is needed now.  All existing WANIP/PPP connection objects will be updated
       * when the wan link comes up.
       */
      if (wanLinkUp == TRUE)
      {
         cmsLog_debug("Layer 2 link already up");

         ret = cmsObj_get(info->oid, &(info->iidStack), OGF_NO_VALUE_UPDATE, &wanConnObj);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("could not get wanConn object to do update on, ret=%d", ret);
         }
         else
         {
#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1
            if (isAutoDetectionEnabled)
            {
               addNewWanConnObj(&(info->iidStack), wanConnObj);
            }
            else
            {
               updateSingleWanConnStatusLocked(&(info->iidStack), wanConnObj, wanLinkUp);
               cmsObj_free(&wanConnObj);
            }                     
#else
            {
               updateSingleWanConnStatusLocked(&(info->iidStack), wanConnObj, wanLinkUp);
               cmsObj_free(&wanConnObj);
            }               
#endif
         }
      }
   }

#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1
    if (!info->isAdd && (info->oid == MDMOID_WAN_IP_CONN || info->oid == MDMOID_WAN_PPP_CONN))
   {
      /* Remove case */
      if (isAutoDetectionEnabled)
      {
         cmsLog_debug("Do updateAutoDetectWanConnListForDeletion on: info->oid=%d, iidstack=%s",
            info->oid, cmsMdm_dumpIidStack(&(info->iidStack)));
         updateAutoDetectWanConnListForDeletion(&(info->iidStack), info->oid);
      }
      else
      {  
         cmsLog_debug("Ignore msg in auto detect profile without auto detect enabled");
      }
   } 
   else  if (info->isAutoDetectChange && info->oid != MDMOID_WAN_IP_CONN && info->oid != MDMOID_WAN_PPP_CONN)
   {
      
      wanLinkUp = dalWan_isWanLayer2LinkUp(info->oid, &(info->iidStack));
      cmsLog_debug("OLD auto detect flag=%d: info->oid=%d, iidstack=%s, wanLinkUp=%d", 
         isAutoDetectionEnabled, info->oid, cmsMdm_dumpIidStack(&(info->iidStack)), wanLinkUp);
      
      if (wanLinkUp)
      {
         /* Need to stop all connected WAN connection first
         */
         stopAllWanConn(&(info->iidStack), isAutoDetectionEnabled);
         
         /* get the new auto detection flag */
         isAutoDetectionEnabled = dalAutoDetect_isAutoDetectEnabled();
         cmsLog_debug("New auto detect flag=%d",  isAutoDetectionEnabled);
         
         if (isAutoDetectionEnabled)
         {
            UBOOL8 startNewConnection = FALSE;
            /* will start the auto detection wan connection process in processAutoDetectTask */
            cmsLog_debug("Auto detect: Disable -> Enable..");
            updateWanConnStatusInSubtreeLocked_n(&(info->iidStack), wanLinkUp, startNewConnection);
         }
         else
         {
            cmsLog_debug("Auto detect: Enable -> Disable. Update with regular linkup process.");
            updateWanConnStatusInSubtreeLocked(&(info->iidStack), wanLinkUp);         
         }
      } 
      else
      {
         cmsLog_error("Should not come to here.  The message should be sent only when the link is up.");
      }
      
   }
#endif /* DMP_X_BROADCOM_COM_AUTODETECTION_1 */

   cmsLck_releaseLock();   

   return ret;
}


void processDhcpcReqOptionReport_igd(CmsMsgHeader *msg)
{
   UBOOL8 found = FALSE;
   UINT32 pid = 0;
   DhcpcReqOptionReportMsgBody *reqOptionBody = (DhcpcReqOptionReportMsgBody *) (msg + 1);
   WanIpConnObject *ipConn = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
  
   pid = PID_IN_EID(msg->src);

   cmsLog_debug("dhcpc pid=%d, leasedTime=%d, clientAddr=%s, serverAddr=%s",
      pid, reqOptionBody->leasedTime, reqOptionBody->clientAddress, reqOptionBody->serverAddress);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d, dhcp client report request option dropped!", ret);
      return;
   }

   while (found == FALSE && 
          cmsObj_getNextFlags(MDMOID_WAN_IP_CONN,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ipConn) == CMSRET_SUCCESS)
   {
      cmsLog_debug("addressing type=%s", ipConn->addressingType);

      if (!(cmsUtl_strcmp(ipConn->connectionType, MDMVS_IP_ROUTED)) &&
          !(cmsUtl_strcmp(ipConn->addressingType, MDMVS_DHCP)) &&
          (ipConn->X_BROADCOM_COM_DhcpcPid == pid))
      {
         found = TRUE;

         CMSMEM_REPLACE_STRING(ipConn->X_BROADCOM_COM_Op50IpAddress, reqOptionBody->clientAddress);
         CMSMEM_REPLACE_STRING(ipConn->X_BROADCOM_COM_Op54ServerIpAddress, reqOptionBody->serverAddress);
         ipConn->X_BROADCOM_COM_Op51LeasedTime = reqOptionBody->leasedTime;

         if ((ret = cmsObj_set(ipConn, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set WanIpConnObject, ret=%d", ret);
         }
      }

      cmsObj_free((void **) &ipConn);
   }

   cmsLck_releaseLock();
}


void updateWanConnStatusInSubtreeLocked(const InstanceIdStack *parentIidStack, UBOOL8 isLinkUp)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   void *wanConnObj=NULL;
   CmsRet ret;

   cmsLog_debug("isLinkUp=%d iidStack=%s", isLinkUp, cmsMdm_dumpIidStack(parentIidStack));


#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1
   isAutoDetectionEnabled = dalAutoDetect_isAutoDetectEnabled();
   if (isAutoDetectionEnabled )
   {
      UBOOL8  startWanConnection = TRUE;
      
      updateWanConnStatusInSubtreeLocked_n(parentIidStack, isLinkUp, startWanConnection);
      return;
   }
#endif

   while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, parentIidStack, &iidStack, &wanConnObj)) == CMSRET_SUCCESS)
   {
      updateSingleWanConnStatusLocked(&iidStack, wanConnObj, isLinkUp);
      cmsObj_free(&wanConnObj);
   }


   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, parentIidStack, &iidStack, &wanConnObj)) == CMSRET_SUCCESS)
   {
      updateSingleWanConnStatusLocked(&iidStack, wanConnObj, isLinkUp);
      cmsObj_free(&wanConnObj);
   }

   return;
}


void updateSingleWanConnStatusLocked(const InstanceIdStack *iidStack, void *wanConnObj, UBOOL8 wanLinkUp)
{
   char ifName[CMS_IFNAME_LENGTH]={0};
   UBOOL8 change = FALSE;
   CmsMsgType connMsg=0;
   CmsRet ret = CMSRET_SUCCESS;   
   MdmObjectId oid = GET_MDM_OBJECT_ID(wanConnObj);

#ifdef BRCM_VOICE_SUPPORT
   char ipAddrBuf[CMS_IPADDR_LENGTH]={0};
#if VOICE_IPV6_SUPPORT
   int isIpv6 = 0;
   char ipAddrFamily[BUFLEN_16];
   unsigned int length = 16;

   ret = dalVoice_GetIpFamily( NULL, ipAddrFamily, length );
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get the IP address family for voice, ret=%d", ret);
      return;
   }
   else
   {
      isIpv6 = !(cmsUtl_strcmp( ipAddrFamily, MDMVS_IPV6 ));
   }
#endif   
#endif

   cmsLog_debug("oid=%d, iidstack: %s, wanLinkUp=%d", 
                 oid, cmsMdm_dumpIidStack(iidStack), wanLinkUp);

   if (oid == MDMOID_WAN_IP_CONN)
   {
      WanIpConnObject *wanIpConnObj = (WanIpConnObject *) wanConnObj;

#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
      cmsLog_debug("wanLinkUp=%d, connectionType=%s connectionStatus=%s IPv6ConnStatus=%s",
                   wanLinkUp, wanIpConnObj->connectionType, wanIpConnObj->connectionStatus,
                   wanIpConnObj->X_BROADCOM_COM_IPv6ConnStatus);
#else
      cmsLog_debug("wanLinkUp=%d, connectionType=%s connectionStatus=%s",
                   wanLinkUp, wanIpConnObj->connectionType, wanIpConnObj->connectionStatus);
#endif

      if (wanLinkUp == FALSE) 
      {
         /* Set the transient layer 2 link status for the CmsObj_set operation which may take some time and
         * the real layer 2 link status could be changed during this time.
         */      
         CMSMEM_REPLACE_STRING(wanIpConnObj->X_BROADCOM_COM_TransientLayer2LinkStatus, MDMVS_DOWN);
      
         if (!cmsUtl_strcmp(wanIpConnObj->connectionStatus, MDMVS_CONNECTED) ||
             !cmsUtl_strcmp(wanIpConnObj->connectionStatus, MDMVS_CONNECTING))
         {
            cmsLog_debug("layer 2 link is down, so set ifName=%s from %s to Disconnected.", 
                         wanIpConnObj->X_BROADCOM_COM_IfName, wanIpConnObj->connectionStatus);
               
            CMSMEM_REPLACE_STRING(wanIpConnObj->connectionStatus, MDMVS_DISCONNECTED);
            snprintf(ifName, sizeof(ifName), "%s", wanIpConnObj->X_BROADCOM_COM_IfName);
            connMsg = CMS_MSG_WAN_CONNECTION_DOWN;
            change = TRUE;
         }
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
         if ( (wanIpConnObj->X_BROADCOM_COM_IPv6Enabled) &&
              ( !cmsUtl_strcmp(wanIpConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED) ||
                !cmsUtl_strcmp(wanIpConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTING)) )
         {
            cmsLog_debug("layer 2 link is down, so set ifName=%s from %s to Disconnected.", 
                         wanIpConnObj->X_BROADCOM_COM_IfName,
                         wanIpConnObj->X_BROADCOM_COM_IPv6ConnStatus);
               
            CMSMEM_REPLACE_STRING(wanIpConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_DISCONNECTED);
            snprintf(ifName, sizeof(ifName), "%s", wanIpConnObj->X_BROADCOM_COM_IfName);
            connMsg = CMS_MSG_WAN_CONNECTION_DOWN;
            change = TRUE;
         }
#endif
      }
      else if (wanLinkUp == TRUE) 
      {
         /* Set the transient layer 2 link status for the CmsObj_set operation which may take some time and
         * the real layer 2 link status could be changed during this time.
         */      
         CMSMEM_REPLACE_STRING(wanIpConnObj->X_BROADCOM_COM_TransientLayer2LinkStatus, MDMVS_UP);
      
         if ((cmsUtl_strcmp(wanIpConnObj->connectionStatus, MDMVS_UNCONFIGURED) == 0) ||
             (cmsUtl_strcmp(wanIpConnObj->connectionStatus, MDMVS_DISCONNECTED) == 0))
         {
            cmsLog_debug("wanIpConnObj->connectionStatus=%s", wanIpConnObj->connectionStatus);

            /* If it is disabled, do nothing on the wanIpConnObj */
            if (!wanIpConnObj->enable)
            {
               cmsLog_debug("wan conn object is disabled.");
            }
            else
            {
               /*
                * For static or bridged connections, when layer 2 link goes up,
                * move the WAN connection status directly to "Connected" state.
                */
               if (!cmsUtl_strcmp(wanIpConnObj->addressingType, MDMVS_STATIC) ||
#ifdef SUPPORT_CELLULAR
                   /* Consider layer3 up if layer2 is up and already get address */
                   (!cmsUtl_strcmp(wanIpConnObj->addressingType, MDMVS_X_BROADCOM_COM_NAS) &&
                     cmsUtl_strcmp(wanIpConnObj->externalIPAddress, "0.0.0.0")) ||
#endif
                   cmsUtl_strcmp(wanIpConnObj->connectionType, MDMVS_IP_ROUTED))     
               {
                  cmsLog_debug("Layer 2 link up in static connection (bridge/IPoA/static IPoE): Wan connectionStatus=%s; Set ifName=%s to Connected.",
                     wanIpConnObj->connectionStatus, wanIpConnObj->X_BROADCOM_COM_IfName);
                     

                  /* record the Connection Establish time */
                  wanIpConnObj->X_BROADCOM_COM_ConnectionEstablishedTime = cmsTms_getSeconds();
                  CMSMEM_REPLACE_STRING(wanIpConnObj->connectionStatus, MDMVS_CONNECTED);
                  snprintf(ifName, sizeof(ifName), "%s", wanIpConnObj->X_BROADCOM_COM_IfName);
#ifdef BRCM_VOICE_SUPPORT
#if VOICE_IPV6_SUPPORT
                  if ( !isIpv6 )
#endif
                  {
                     snprintf(ipAddrBuf, sizeof(ipAddrBuf), "%s", wanIpConnObj->externalIPAddress);
                  }
#endif
                  connMsg = CMS_MSG_WAN_CONNECTION_UP;
                  change = TRUE;
               }
               else
               {
                  /* dynamic IPoE, need to start dhcpc first: set the connectionStatus="Connecting"
                  * and processDhcpcStateChanged will change that to "Connected" if dhcpc gets the 
                  * external IP, etc.
                  */

                  cmsLog_debug("Layer 2 link up in dynamic IPoE: %s is %s, so set to CONNECTING", 
                     wanIpConnObj->X_BROADCOM_COM_IfName,  wanIpConnObj->connectionStatus);
                     
                  CMSMEM_REPLACE_STRING(wanIpConnObj->connectionStatus, MDMVS_CONNECTING);
                  change = TRUE;
               }
            }
         }
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
         if ((wanIpConnObj->X_BROADCOM_COM_IPv6Enabled) &&
             ((cmsUtl_strcmp(wanIpConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_UNCONFIGURED) == 0) ||
              (cmsUtl_strcmp(wanIpConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_DISCONNECTED) == 0)) )
         {
            cmsLog_debug("wanIpConnObj->X_BROADCOM_COM_IPv6ConnStatus=%s",
                          wanIpConnObj->X_BROADCOM_COM_IPv6ConnStatus);

            /*
             * For static connections, when layer 2 link goes up,
             * move the WAN connection status directly to "Connected" state.
            */
            if (!cmsUtl_strcmp(wanIpConnObj->X_BROADCOM_COM_IPv6AddressingType, MDMVS_STATIC))
            {
               cmsLog_debug("Layer 2 link up in static mode: Wan IPv6ConnStatus=%s; Set ifName=%s to Connected.",
                            wanIpConnObj->X_BROADCOM_COM_IPv6ConnStatus,
                            wanIpConnObj->X_BROADCOM_COM_IfName);
                  
               CMSMEM_REPLACE_STRING(wanIpConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED);
               snprintf(ifName, sizeof(ifName), "%s", wanIpConnObj->X_BROADCOM_COM_IfName);
               connMsg = CMS_MSG_WAN_CONNECTION_UP;
#if defined(BRCM_VOICE_SUPPORT) && VOICE_IPV6_SUPPORT
               if ( isIpv6 )
               {
                  snprintf(ipAddrBuf, sizeof(ipAddrBuf), "%s", wanIpConnObj->X_BROADCOM_COM_ExternalIPv6Address);
               }
#endif
            }
            else
            {
               /*
                * dynamic IPoE, need to start dhcp6c first: set the IPv6ConnStatus="Connecting"
                * and processDhcpcStateChanged will change that to "Connected" if dhcp6c gets the 
                * external IP, etc.
               */

               cmsLog_debug("Layer 2 link up in dynamic IPoE: %s is %s, so set to CONNECTING", 
                            wanIpConnObj->X_BROADCOM_COM_IfName, 
                            wanIpConnObj->X_BROADCOM_COM_IPv6ConnStatus);
                  
               CMSMEM_REPLACE_STRING(wanIpConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTING);
            }

            wanIpConnObj->X_BROADCOM_COM_IPv6PrefixDelegationEnabled = FALSE;
            change = TRUE;
         }
#endif
      }
   }

   else /* must be a pppConn Object */
   {
      WanPppConnObject *wanPppConnObj = (WanPppConnObject *) wanConnObj;

#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
      cmsLog_debug("wanLinkUp=%d, connectionStatus=%s IPv6ConnStatus=%s", 
                    wanLinkUp, wanPppConnObj->connectionStatus,
                    wanPppConnObj->X_BROADCOM_COM_IPv6ConnStatus);
#else
      cmsLog_debug("wanLinkUp=%d, connectionStatus=%s", wanLinkUp, wanPppConnObj->connectionStatus);
#endif
      
      if (wanLinkUp == FALSE)
      {
         /* Set the transient layer 2 link status for the CmsObj_set operation which may take some time and
         * the real layer 2 link status could be changed during this time.
         */      
         CMSMEM_REPLACE_STRING(wanPppConnObj->X_BROADCOM_COM_TransientLayer2LinkStatus, MDMVS_DOWN);
         if (!cmsUtl_strcmp(wanPppConnObj->connectionStatus, MDMVS_CONNECTED) ||
             !cmsUtl_strcmp(wanPppConnObj->connectionStatus, MDMVS_CONNECTING))
         {
            cmsLog_debug("layer 2 link is down so Set ifName=%s from %s to Disconnected.", 
                         wanPppConnObj->X_BROADCOM_COM_IfName, wanPppConnObj->connectionStatus);
               
            CMSMEM_REPLACE_STRING(wanPppConnObj->connectionStatus, MDMVS_DISCONNECTED);

#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1
            {
               UBOOL8 realWanLinkUp = dalWan_isWanLayer2LinkUp(MDMOID_WAN_PPP_CONN, iidStack);
               
               cmsLog_debug("realWanLinkUp=%d, fakelinkUp=%d", realWanLinkUp, wanLinkUp);
               if (!realWanLinkUp)
               {
                  /*  X_BROADCOM_COM_StopPppD was set to TRUE for stopping pppd in the manual
                  * selection (change on ppp setting).  Only reset to FALSE if phyiscal link and
                  * fakelinkUp both are down. 
                  */
                   wanPppConnObj->X_BROADCOM_COM_StopPppD = FALSE;
               }
               else
               {
                   wanPppConnObj->X_BROADCOM_COM_StopPppD = TRUE;
               }
            }               
#endif      
            snprintf(ifName, sizeof(ifName), "%s", wanPppConnObj->X_BROADCOM_COM_IfName);
            connMsg = CMS_MSG_WAN_CONNECTION_DOWN;
            change = TRUE;
         }
         else if (!cmsUtl_strcmp(wanPppConnObj->connectionStatus, MDMVS_DISCONNECTED) ||
             !cmsUtl_strcmp(wanPppConnObj->lastConnectionError, MDMVS_ERROR_UNKNOWN))
         {
            /* This is needed because if layer 2 link is up but ppp server is down (connectionStatus is "Disconnected")
            *  and then layer 2 link goes down, the pppd in the memory need to be stopped in the rcl_wan.c
            */
            CMSMEM_REPLACE_STRING(wanPppConnObj->lastConnectionError, MDMVS_ERROR_FORCED_DISCONNECT);
            snprintf(ifName, sizeof(ifName), "%s", wanPppConnObj->X_BROADCOM_COM_IfName);
            connMsg = CMS_MSG_WAN_CONNECTION_DOWN;
            change = TRUE;
         }
  
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
         if ((wanPppConnObj->X_BROADCOM_COM_IPv6Enabled) &&
             (!cmsUtl_strcmp(wanPppConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED) ||
              !cmsUtl_strcmp(wanPppConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTING)) )
         {
            cmsLog_debug("layer 2 link is down so Set ifName=%s from %s to Disconnected.", 
                         wanPppConnObj->X_BROADCOM_COM_IfName,
                         wanPppConnObj->X_BROADCOM_COM_IPv6ConnStatus);
               
            CMSMEM_REPLACE_STRING(wanPppConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_DISCONNECTED);
            wanPppConnObj->X_BROADCOM_COM_IPv6PppUp = FALSE;
            snprintf(ifName, sizeof(ifName), "%s", wanPppConnObj->X_BROADCOM_COM_IfName);
            connMsg = CMS_MSG_WAN_CONNECTION_DOWN;
            change = TRUE;
         }
#endif
      }
      else if (wanLinkUp == TRUE)
      {
         /* Set the transient layer 2 link status for the CmsObj_set operation which may take some time and
         * the real layer 2 link status could be changed during this time.
         */
         CMSMEM_REPLACE_STRING(wanPppConnObj->X_BROADCOM_COM_TransientLayer2LinkStatus, MDMVS_UP);
#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1
         if (!cmsUtl_strcmp(wanPppConnObj->connectionStatus, MDMVS_CONNECTED) && 
            !wanPppConnObj->enable)
         {
            /* For auto detection profile, if ppp is connected and is disabled in the manual selection,
            *  need to set X_BROADCOM_COM_StopPppD to TRUE for rcl handler to stop pppd
            */
            wanPppConnObj->X_BROADCOM_COM_StopPppD = TRUE;
         }
         else 
#endif
         if (!cmsUtl_strcmp(wanPppConnObj->connectionStatus, MDMVS_UNCONFIGURED) ||
             !cmsUtl_strcmp(wanPppConnObj->connectionStatus, MDMVS_DISCONNECTED))
         {
            /* Need to start pppd: set the connectionStatus="Connecting"
            * and processPppStateChanged will change that to "Connected" if pppd gets the 
            * external IP, etc. If it is disabled, do nothing on the wanPppConnObj
            */

            if (!wanPppConnObj->enable)
            {
               cmsLog_debug("wan conn object is disabled.");
            }
            else
            {
               cmsLog_debug("dsl link went from down to up, set %s to Connecting", wanPppConnObj->X_BROADCOM_COM_IfName);
               CMSMEM_REPLACE_STRING(wanPppConnObj->connectionStatus, MDMVS_CONNECTING);
#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1
               if (isAutoDetectionEnabled)
               {
                  /* if auto detection is enabled and this ppp is enabled, but it can not connected 
                  * in time (AUTO_DETECT_TASK_INTERVAL), this pppd needs to be stop in rcl
                  */
                  wanPppConnObj->X_BROADCOM_COM_StopPppD = TRUE;
               }
               else
               {
                  wanPppConnObj->X_BROADCOM_COM_StopPppD = FALSE;
               }
#endif            

               change = TRUE;
            }               
         }         
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
         if ((wanPppConnObj->X_BROADCOM_COM_IPv6Enabled) &&
             (!cmsUtl_strcmp(wanPppConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_UNCONFIGURED) ||
              !cmsUtl_strcmp(wanPppConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_DISCONNECTED)) )
         {
           /* 
            * Set the IPv6ConnStatus="Connecting"
            * and processPppStateChanged will change PppUp to true if pppd gets the 
            * IPv6CP up information.
            */
          
            cmsLog_debug("dsl link went from down to up, set %s to Connecting",
                         wanPppConnObj->X_BROADCOM_COM_IfName);
            CMSMEM_REPLACE_STRING(wanPppConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTING);
            wanPppConnObj->X_BROADCOM_COM_IPv6PppUp = FALSE;
            change = TRUE;
         }
#endif
      }
   }
     
   if ( change )
   {
      if ((ret = cmsObj_set(wanConnObj, iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Fail to set wanConnObj. ret=%d", ret);
      }
      else
      {
         cmsLog_debug("Done setting the connectionStatus to wanConnObj.");
      }
      
      /* Only send WAN connection event message if connection is up or down */
      if (connMsg == CMS_MSG_WAN_CONNECTION_DOWN ||
          connMsg == CMS_MSG_WAN_CONNECTION_UP)
      {
         sendStatusMsgToSmd(connMsg, ifName);
      }

#ifdef BRCM_VOICE_SUPPORT
      /*
       * The only time we should call this function is when we have a static IPoE
       * connection and the link goes up.  For all the other link up cases, we have
       * not acquired the IP address yet. So we should not call this function yet.
       * For the link down case, I don't see any code that handles switching from
       * one ip address to another, and it does not seem like the code wants 
       * voice to go down if the link goes down.  (The boundIfName implemenation for
       * voice needs to be revisited and cleaned up a bit more... mwang).
       */
      if (wanLinkUp && ipAddrBuf[0] != '\0' && (isVoiceOnAnyWan || voiceWanIfName))
      {
         initVoiceOnWanIntfLocked(ifName, ipAddrBuf);
      }
#endif

   }
   else
   {
      /* do nothing */
      cmsLog_debug("layer 2 link changed but no action taken."); 
   }

#if defined(DMP_DEVICE2_BASELINE_1) && defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)
   /* In Hybrid IPv6 case, we need to inject WAN Layer 2 link status into
    * the TR181 interface stack.
    */
   cmsLog_debug("injecting status for oid %d %s, wanLinkUp=%d",
                oid, cmsMdm_dumpIidStack(iidStack), wanLinkUp);

   intfStack_propagateStatusByIidLocked(oid, iidStack,
                                        (wanLinkUp ? MDMVS_UP : MDMVS_DOWN));
#endif

   return;
}

#ifdef SUPPORT_TR69C
void setAcsParams_igd(const char *acsURL, const char *provisioningCode,
                  UINT32 minWaitInterval, UINT32 intervalMultiplier)
{
   ManagementServerObject *mgmtObj=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 doSet=FALSE;
   CmsRet ret;

   if (CMSRET_SUCCESS != (ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack,
                                           0, (void **) &mgmtObj)))
   {
      cmsLog_error("Could not get obj, ret=%d", ret);
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
      IGDDeviceInfoObject *deviceInfoObj=NULL;

      // Technically do not need to init iidStack since both the
      // previous object (MDMOID_MANAGEMENT_SERVER) and this object
      // (MDMOID_IGD_DEVICE_INFO) are Type 0 objects, so iidStack is always
      // empty.  But do it anyways just to show good form.
      INIT_INSTANCE_ID_STACK(&iidStack);
      if (CMSRET_SUCCESS != (ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO,
                                   &iidStack, 0, (void **) &deviceInfoObj)))
      {
         cmsLog_error("Could not get obj, ret=%d", ret);
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
