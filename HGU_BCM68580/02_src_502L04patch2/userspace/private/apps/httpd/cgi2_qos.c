/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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

#ifdef SUPPORT_QOS

#ifdef DMP_DEVICE2_QOS_1


#include "cms.h"
#include "cms_obj.h"
#include "cms_dal.h"
#include "cms_qos.h"
#include "httpd.h"
#include "cgi_cmd.h"




void cgiGetQosIntf_dev2(int argc __attribute__((unused)),
                   char **argv __attribute__((unused)),
                   char *varValue)
{
   InstanceIdStack iidStack;
   void *mdmObj  = NULL;
   CmsRet ret;

   varValue[0] = '\0';

#ifdef DMP_DEVICE2_ETHERNETINTERFACE_1
   /* get LAN Ethernet interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      if (((Dev2EthernetInterfaceObject *)mdmObj)->enable &&
          ((Dev2EthernetInterfaceObject *)mdmObj)->upstream == FALSE)
      {
         strcat(varValue, ((Dev2EthernetInterfaceObject *)mdmObj)->name);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
#endif

#ifdef DMP_DEVICE2_WIFIACCESSPOINT_1
   /* get (LAN side) WLAN interfaces (XXX what about Wifi as WAN?) */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_WIFI_SSID, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      /* XXX for now assume all wifi ssid's are on LAN side */
      if (((Dev2WifiSsidObject *)mdmObj)->enable)
 // && !qdmWifi_isIntfUpstreamLocked((Dev2WifiSsidObject *)mdmObj)->name)
      {
         strcat(varValue, ((Dev2WifiSsidObject *)mdmObj)->name);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
#endif  /* DMP_DEVICE2_WIFIACCESSPOINT_1 */

#ifdef todo_later
#ifdef DMP_USBLAN_1
   /* get USB interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_LAN_USB_INTF, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      if (((LanUsbIntfObject *)mdmObj)->enable)
      {
         strcat(varValue, ((LanUsbIntfObject *)mdmObj)->X_BROADCOM_COM_IfName);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_LAN_USB_INTF> returns error. ret=%d", ret);
      return;
   }
#endif

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   INIT_INSTANCE_ID_STACK(&iidStack);
   /* Get Gpon WanDevice iidStack first */
   if (dalGpon_getGponWanIidStatck(&iidStack) == CMSRET_SUCCESS)
   {
      InstanceIdStack iidStack2;
      InstanceIdStack gponLinkIid = EMPTY_INSTANCE_ID_STACK;
      WanGponLinkCfgObject *gponLinkCfg = NULL;
      char gpondesc[CMS_IFNAME_LENGTH];

      /* go over all the wan gpon link config objects to find the enabled gpon interface and add it to the list  */
      while  ((ret = cmsObj_getNextInSubTreeFlags
         (MDMOID_WAN_GPON_LINK_CFG, &iidStack, &gponLinkIid, OGF_NO_VALUE_UPDATE, (void **)&gponLinkCfg)) == CMSRET_SUCCESS)
      {
         if (gponLinkCfg->enable)
         {
            sprintf(gpondesc, "/%s", gponLinkCfg->ifName);
         }
         cmsObj_free((void **)&gponLinkCfg);


         INIT_INSTANCE_ID_STACK(&iidStack2);
         while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &gponLinkIid, &iidStack2, &mdmObj)) == CMSRET_SUCCESS)
         {
            strcat(varValue, ((WanIpConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
            strcat(varValue, gpondesc);
            if (cmsUtl_strcmp(((WanIpConnObject *)mdmObj)->connectionType, MDMVS_IP_BRIDGED) == 0)
            {
               strcat(varValue, "(bridged)");
            }
            else
            {
               strcat(varValue, "(routed)");
            }
            strcat(varValue, "|");
            cmsObj_free(&mdmObj);
         }
         if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_getNextInSubTree <MDMOID_WAN_IP_CONN> returns error. ret=%d", ret);
            return;
         }

         INIT_INSTANCE_ID_STACK(&iidStack2);
         while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &gponLinkIid, &iidStack2, &mdmObj)) == CMSRET_SUCCESS)
         {
            strcat(varValue, ((WanPppConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
            strcat(varValue, gpondesc);
            strcat(varValue, "(routed)");
            strcat(varValue, "|");
            cmsObj_free(&mdmObj);
         }
         if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_getNextInSubTree <MDMOID_WAN_PPP_CONN> returns error. ret=%d", ret);
            return;
         }
      }

   }
#endif

#ifdef SUPPORT_MOCA
#ifdef DMP_X_BROADCOM_COM_MOCALAN_1
   /* get LAN Moca interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_LAN_MOCA_INTF, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      if (((LanMocaIntfObject *)mdmObj)->enable)
      {
         strcat(varValue, ((LanMocaIntfObject *)mdmObj)->ifName);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_LAN_MOCA_INTF> returns error. ret=%d", ret);
      return;
   }
#endif
#endif

#endif  /* todo_later */

   /*
    * All Layer 3 interfaces can be obtained by simply walking through
    * the IP.Interface table.  Does not matter what the underlying layer 2
    * interface type is.
    */
   {
      Dev2IpInterfaceObject *ipIntfObj = NULL;

      INIT_INSTANCE_ID_STACK(&iidStack);
      while (cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                            (void **)&ipIntfObj) == CMSRET_SUCCESS)
      {
         if (ipIntfObj->X_BROADCOM_COM_Upstream)
         {
            strcat(varValue, ipIntfObj->name);

            if (ipIntfObj->X_BROADCOM_COM_BridgeService)
            {
               strcat(varValue, "(bridged)");
            }
            else
            {
               strcat(varValue, "(routed)");
            }
            strcat(varValue, "|");
         }

         cmsObj_free((void **)&ipIntfObj);
      }
   }
}


void cgiGetQosMgmtEnbl_dev2(int argc __attribute__((unused)),
                       char **argv __attribute__((unused)),
                       char *varValue)
{
   /*
    * In TR181, the top level QoS object (equivalent to TR98 Q_MGMT) does
    * not have an enable parameter.  So it is always enabled.
    */
   sprintf(varValue, "1");
}


void cgiGetQosMgmtDefMark_dev2(int argc __attribute__((unused)),
                          char **argv __attribute__((unused)),
                          char *varValue)
{
   InstanceIdStack iidStack;
   Dev2QosObject *qMgmtObj = NULL;
   CmsRet ret;

   varValue[0] = '\0';

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_DEV2_QOS, &iidStack, 0, (void **)&qMgmtObj)) == CMSRET_SUCCESS)
   {
      sprintf(varValue, "%d", qMgmtObj->defaultDSCPMark);
      cmsObj_free((void **)&qMgmtObj);
   }
   else
   {
      cmsLog_error("cmsObj_get returns error. ret=%d", ret);
   }
}


void cgiGetQosMgmtDefQueue_dev2(int argc __attribute__((unused)),
                           char **argv __attribute__((unused)),
                           char *varValue)
{
   /*
    * DefaultQueue is not displayed or configurable on web page
    * (qosqmgmt.html), so just return same dummy value as TR98 code.
    */
   sprintf(varValue, "-1");
}



#endif  /* DMP_DEVICE2_QOS_1 */

#endif  /* SUPPORT_QOS */

