/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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

#ifdef DMP_DEVICE2_BASELINE_1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"

#include "cgi_sts.h"



/*  for interface (eth, usb, wlan) statistics */
void cgiGetStsIfc_dev2(char *varValue)
{
   char *p = varValue;
   Dev2EthernetInterfaceObject *ethObj = NULL;
   Dev2EthernetInterfaceStatsObject *ethStatsObj = NULL;
#ifdef DMP_DEVICE2_USBINTERFACE_1
   Dev2UsbInterfaceObject *usbObj = NULL;
   Dev2UsbInterfaceStatsObject *usbStatsObj=NULL;
#endif /* DMP_DEVICE2_USBINTERFACE_1*/
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackChild = EMPTY_INSTANCE_ID_STACK;

   while (cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE,
                         &iidStack, (void **) &ethObj) == CMSRET_SUCCESS)
   {
      p += sprintf(p, "               <tr>");
      p += sprintf(p, "                  <td class='hd'>%s</td>", ethObj->name);
      if (cmsObj_getNextInSubTree(MDMOID_DEV2_ETHERNET_INTERFACE_STATS,
                                  &iidStack, &iidStackChild,
                                  (void **) &ethStatsObj) == CMSRET_SUCCESS)
      {
         p += sprintf(p, "                  <td>%" PRIu64 "</td>",ethStatsObj->bytesReceived);
         p += sprintf(p, "                  <td>%" PRIu64 "</td>",ethStatsObj->packetsReceived);
         p += sprintf(p, "                  <td>%" PRIu32 "</td>",ethStatsObj->errorsReceived);
         p += sprintf(p, "                  <td>%" PRIu32 "</td>",ethStatsObj->discardPacketsReceived);
         p += sprintf(p, "                  <td>%" PRIu64 "</td>",ethStatsObj->X_BROADCOM_COM_MulticastBytesReceived);
         p += sprintf(p, "                  <td>%" PRIu64 "</td>",ethStatsObj->multicastPacketsReceived);
         p += sprintf(p, "                  <td>%" PRIu64 "</td>",ethStatsObj->unicastPacketsReceived);
         p += sprintf(p, "                  <td>%" PRIu64 "</td>",ethStatsObj->broadcastPacketsReceived); 
         p += sprintf(p, "                  <td>%" PRIu64 "</td>",ethStatsObj->bytesSent);
         p += sprintf(p, "                  <td>%" PRIu64 "</td>",ethStatsObj->packetsSent);
         p += sprintf(p, "                  <td>%" PRIu32 "</td>",ethStatsObj->errorsSent);
         p += sprintf(p, "                  <td>%" PRIu32 "</td>",ethStatsObj->discardPacketsSent);
         p += sprintf(p, "                  <td>%" PRIu64 "</td>",ethStatsObj->X_BROADCOM_COM_MulticastBytesSent);
         p += sprintf(p, "                  <td>%" PRIu64 "</td>",ethStatsObj->multicastPacketsSent);
         p += sprintf(p, "                  <td>%" PRIu64 "</td>",ethStatsObj->unicastPacketsSent);
         p += sprintf(p, "                  <td>%" PRIu64 "</td>",ethStatsObj->broadcastPacketsSent);    
         p += sprintf(p, "               </tr>");

         cmsObj_free((void **) &ethStatsObj);
      }

      cmsObj_free((void **) &ethObj);
   }

#ifdef DMP_DEVICE2_USBINTERFACE_1
   INIT_INSTANCE_ID_STACK(&iidStack);
   INIT_INSTANCE_ID_STACK(&iidStackChild);

   while (cmsObj_getNext(MDMOID_DEV2_USB_INTERFACE,
                         &iidStack, (void **) &usbObj) == CMSRET_SUCCESS)
   {
      p += sprintf(p, "               <tr>");
      p += sprintf(p, "                  <td class='hd'>%s</td>", usbObj->name);
      if (cmsObj_getNextInSubTree(MDMOID_DEV2_USB_INTERFACE_STATS,
                                  &iidStack, &iidStackChild,
                                  (void **) &usbStatsObj) == CMSRET_SUCCESS)
      {
         p += sprintf(p, "                  <td>%llu</td>",usbStatsObj->bytesReceived);
         p += sprintf(p, "                  <td>%llu</td>",usbStatsObj->packetsReceived);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->errorsReceived);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->discardPacketsReceived);
         p += sprintf(p, "                  <td>%llu</td>",usbStatsObj->X_BROADCOM_COM_MulticastBytesReceived);
         p += sprintf(p, "                  <td>%llu</td>",usbStatsObj->multicastPacketsReceived);
         p += sprintf(p, "                  <td>%llu</td>",usbStatsObj->unicastPacketsReceived);
         p += sprintf(p, "                  <td>%llu</td>",usbStatsObj->broadcastPacketsReceived); 
         p += sprintf(p, "                  <td>%llu</td>",usbStatsObj->bytesSent);
         p += sprintf(p, "                  <td>%llu</td>",usbStatsObj->packetsSent);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->errorsSent);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->discardPacketsSent);
         p += sprintf(p, "                  <td>%llu</td>",usbStatsObj->X_BROADCOM_COM_MulticastBytesSent);
         p += sprintf(p, "                  <td>%llu</td>",usbStatsObj->multicastPacketsSent);
         p += sprintf(p, "                  <td>%llu</td>",usbStatsObj->unicastPacketsSent);
         p += sprintf(p, "                  <td>%llu</td>",usbStatsObj->broadcastPacketsSent);    
         p += sprintf(p, "               </tr>");

         cmsObj_free((void **) &usbStatsObj);
      }

      cmsObj_free((void **) &usbObj);
   }
#endif /* DMP_DEVICE2_USBINTERFACE_1*/

#ifdef BRCM_WLAN
   cgiGetStsWifiIfc(&p);
#endif
   
   *p = 0;

   /* this is potentially a big page.  Buffer is WEB_BUF_SIZE_MAX bytes */
   cmsLog_debug("total bytes written=%d", p-varValue+1);

   return;
}

/* reseting/clearing LAN interfaces' stats */
void cgiResetStsIfc_dev2(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2EthernetInterfaceObject *ethObj = NULL;
#ifdef DMP_DEVICE2_USBINTERFACE_1
   Dev2UsbInterfaceObject *usbObj = NULL;
#endif /* DMP_DEVICE2_USBINTERFACE_1*/

   while ((cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, (void **) &ethObj)) == CMSRET_SUCCESS)
   {
      cmsObj_clearStatistics(MDMOID_DEV2_ETHERNET_INTERFACE_STATS, &iidStack);
      cmsObj_free((void **) &ethObj);
   }

#ifdef DMP_DEVICE2_USBINTERFACE_1
   while ((cmsObj_getNext(MDMOID_DEV2_USB_INTERFACE, &iidStack, (void **) &usbObj)) == CMSRET_SUCCESS)
   {
      cmsObj_clearStatistics(MDMOID_DEV2_USB_INTERFACE_STATS, &iidStack);
      cmsObj_free((void **) &usbObj);
   }
#endif /* DMP_DEVICE2_USBINTERFACE_1*/
}


void cgiDeviceInfoWanStatusBody_dev2(FILE *fs)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   char serviceStr[BUFLEN_32];
   char interfaceStr[BUFLEN_32];
   char protocalStr[BUFLEN_16]={0};
   SINT32 vlanId=0;
   UBOOL8 natEnabled;
   UBOOL8 firewallEnabled;
   UBOOL8 ipv6Enabled=FALSE;
   UBOOL8 igmpProxyEnabled=FALSE;
   UBOOL8 igmpSourceEnabled=FALSE;
   UBOOL8 mldProxyEnabled=FALSE;
   UBOOL8 mldSourceEnabled=FALSE;
   char ipv4AddrStrBuf[CMS_IPADDR_LENGTH]={0};
   char ipv6AddrStrBuf[CMS_IPADDR_LENGTH]={0};
   char ipv6StatusBuf[BUFLEN_32]={0};


   while (cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                                 (void **)&ipIntfObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("ipIntf->name %s, ipIntf->X_BROADCOM_COM_Upstream %d",
                    ipIntfObj->name, ipIntfObj->X_BROADCOM_COM_Upstream);

      if (!ipIntfObj->X_BROADCOM_COM_Upstream)
      {
         /* skip non wan ip interfaces */
         cmsObj_free((void **)&ipIntfObj);
         continue;
      }

      /* form serviceStr (br_0_2_35) string XXX should not use alias*/
      snprintf(serviceStr, sizeof(serviceStr), "%s",  ipIntfObj->alias? ipIntfObj->alias: "(null)");

      /* form interfaceStr (ptm0) */
      snprintf(interfaceStr, sizeof(interfaceStr), "%s", (ipIntfObj->name ? ipIntfObj->name : "(null)"));

      /* form protocolStr */
      if (ipIntfObj->X_BROADCOM_COM_BridgeService)
      {
         strcpy(protocalStr, BRIDGE_PROTO_STR);
      }
      else if (strstr(ipIntfObj->lowerLayers,"Device.PPP."))  // a bit of a hack, but it works
      {
         if(strstr(ipIntfObj->name,PPPOA_IFC_STR))
         {
             strcpy(protocalStr, PPPOA_PROTO_STR);
         }
         else
             strcpy(protocalStr, PPPOE_PROTO_STR);
      }
      else
      {
          if (strstr(ipIntfObj->lowerLayers,"Device.ATM."))
          {
             strcpy(protocalStr, IPOA_PROTO_STR);
          }else
             strcpy(protocalStr, IPOE_PROTO_STR);
      }

      natEnabled = qdmIpIntf_isNatEnabledOnIntfNameLocked(ipIntfObj->name);
      firewallEnabled = qdmIpIntf_isFirewallEnabledOnIntfnameLocked(ipIntfObj->name);
      qdmIpIntf_getIpv4AddressByNameLocked_dev2(ipIntfObj->name, ipv4AddrStrBuf);

#ifdef SUPPORT_IPV6
      ipv6Enabled = ipIntfObj->IPv6Enable;
      qdmIpIntf_getIpv6AddressByNameLocked_dev2(ipIntfObj->name, ipv6AddrStrBuf);
#endif

#ifdef DMP_X_BROADCOM_COM_IGMP_1
      igmpProxyEnabled = ipIntfObj->X_BROADCOM_COM_IGMPEnabled;
      igmpSourceEnabled = ipIntfObj->X_BROADCOM_COM_IGMP_SOURCEEnabled;
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
      mldProxyEnabled = ipIntfObj->X_BROADCOM_COM_MLDEnabled;
      mldSourceEnabled = ipIntfObj->X_BROADCOM_COM_MLD_SOURCEEnabled;
#endif

      /* at the end we need to be able to display this */
      cgiDisplayWanStatusEntry(fs, serviceStr, interfaceStr, protocalStr, vlanId,
                    ipv6Enabled,
                    igmpProxyEnabled, igmpSourceEnabled,
                    natEnabled, firewallEnabled,
                    ipIntfObj->status, ipv4AddrStrBuf,
                    mldProxyEnabled, mldSourceEnabled, ipv6StatusBuf, ipv6AddrStrBuf);

      cmsObj_free((void **)&ipIntfObj);
   }

   return;
}

#endif    // DMP_DEVICE2_BASELINE_1

