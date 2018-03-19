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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <linux/if_ether.h>

#include "cms.h"
#include "cms_dal.h"
#include "cms_qos.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_sec.h"
#include "cgi_main.h"
#include "cgi_qos.h"
#include "secapi.h"
#include "syscall.h"

/* local constants */


/* Local functions */
static void cgiQosMgmtSavApply(char *query, FILE *fs);
static void cgiQosMgmtView(FILE *fs);


void cgiQosMgmt(char *query, FILE *fs)
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);
   if (strcmp(action, "savapply") == 0)
   { 
      cgiQosMgmtSavApply(query, fs);
   }
   else
   {
      cgiQosMgmtView(fs);
   }
}  /* End of cgiQosMgmt() */

void cgiQosMgmtView(FILE *fs)
{
   do_ej("/webs/qosqmgmt.html", fs);

}  /* End of cgiQosMgmtView() */

void cgiQosMgmtSavApply(char *query, FILE *fs)
{
   char cmd[WEB_BUF_SIZE_MAX];
   char enblQos[BUFLEN_8];
   char queue[BUFLEN_8];
   char dscpMark[BUFLEN_8];
   UBOOL8 enable;
   SINT32 defaultDSCPMark;
   UINT32 defaultQueue;
   CmsRet ret;

   cgiGetValueByName(query, "enblQos", enblQos );
   cgiGetValueByName(query, "defaultQueue", queue );
   cgiGetValueByName(query, "defaultDscpMark", dscpMark );

   enable = atoi(enblQos)? TRUE : FALSE;
   defaultDSCPMark = atoi(dscpMark);
   defaultQueue = atoi(queue);


   ret = dalQos_configQosMgmt(enable, defaultDSCPMark, defaultQueue);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("dalQos_configQosMgmt returns error. ret=%d", ret);
      sprintf(cmd, "Queue Management Configuration failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "Queue Management Configuration Error", cmd, "");
   }
   else
   {
      glbSaveConfigNeeded = TRUE;
      cgiQosMgmtView(fs);
   }

}  /* End of cgiQosMgmtSavApply() */


#ifdef DMP_QOS_1

// Fills in all the interfaces that supports QoS on the LAN and the WAN side.
// This function is used by the classification page.
void cgiGetQosIntf_igd(int argc __attribute__((unused)),
                   char **argv __attribute__((unused)),
                   char *varValue)
{
   InstanceIdStack iidStack;
   void *mdmObj  = NULL;
#ifdef DMP_ADSLWAN_1
   void *linkCfg = NULL;
#endif
   CmsRet ret;

   varValue[0] = '\0';

   /* get LAN Ethernet interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      if (((LanEthIntfObject *)mdmObj)->enable &&
          cmsUtl_strcmp(((LanEthIntfObject *)mdmObj)->X_BROADCOM_COM_WanLan_Attribute,
                        MDMVS_WANONLY))
      {
         strcat(varValue, ((LanEthIntfObject *)mdmObj)->X_BROADCOM_COM_IfName);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_LAN_ETH_INTF> returns error. ret=%d", ret);
      return;
   }

#ifdef BRCM_WLAN
   /* get WLAN interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_WL_VIRT_INTF_CFG, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      InstanceIdStack iidStack1;

      if (((WlVirtIntfCfgObject *)mdmObj)->wlEnblSsid)
      {
         _WlBaseCfgObject *wlBaseCfgObj=NULL;
         int wlEnbl =0;
   
         INIT_INSTANCE_ID_STACK(&iidStack1);
         iidStack1.instance[0] = iidStack.instance[0];
         iidStack1.instance[1] = iidStack.instance[1];
         iidStack1.instance[1] = iidStack.instance[1];

         iidStack1.currentDepth=2;
         if ((ret = cmsObj_get(MDMOID_WL_BASE_CFG, &iidStack1, 0, (void **)&wlBaseCfgObj)) != CMSRET_SUCCESS) {
	      
            cmsLog_error("%s@%d MDM MDMOID_WL_BASE_CFG failure\n", __FUNCTION__, __LINE__);
            cmsObj_free(&mdmObj);
            return;
         }
         wlEnbl = wlBaseCfgObj->wlEnbl;
         cmsObj_free((void **) &wlBaseCfgObj);
         if  (wlEnbl) {
           strcat(varValue, ((WlVirtIntfCfgObject *)mdmObj)->wlIfcname);
           strcat(varValue, "|");
         }
      }
      cmsObj_free(&mdmObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_WL_VIRT_INTF_CFG> returns error. ret=%d", ret);
      return;
   }
#endif

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

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
    INIT_INSTANCE_ID_STACK(&iidStack);
    /* Get Epon WanDevice iidStack */      
    if (dalEpon_getEponWanIidStatck(&iidStack) == CMSRET_SUCCESS)
    {
        InstanceIdStack iidStack2;
        InstanceIdStack eponIid = EMPTY_INSTANCE_ID_STACK;   
        WanEponLinkCfgObject *eponLinkCfg = NULL;
        char epondesc[CMS_IFNAME_LENGTH];

        epondesc[0] = '\0';

        /* Run over all WAN Epon link configuration objects to find the enabled Epon interfaces and add it to the list. */
        while  ((ret = cmsObj_getNextInSubTreeFlags
           (MDMOID_WAN_EPON_LINK_CFG, &iidStack, &eponIid, OGF_NO_VALUE_UPDATE, (void **)&eponLinkCfg)) == CMSRET_SUCCESS)
        {
            if (eponLinkCfg->enable)
            {
                sprintf(epondesc, "/%s", eponLinkCfg->ifName);
            }
            cmsObj_free((void **)&eponLinkCfg);
                     
            INIT_INSTANCE_ID_STACK(&iidStack2);
            /* Find out & print Interface type (WAN bridge/WAN IP) */
            while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &eponIid, &iidStack2, &mdmObj)) == CMSRET_SUCCESS)
            {
                strcat(varValue, ((WanIpConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
                strcat(varValue, epondesc);
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
            /* Wan PPPoE */
            while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &eponIid, &iidStack2, &mdmObj)) == CMSRET_SUCCESS)
            {
                strcat(varValue, ((WanPppConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
                strcat(varValue, epondesc);
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
#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   /* get WAN Moca interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_WAN_MOCA_INTF, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      InstanceIdStack iidStack1;
      InstanceIdStack iidStack2;
      char mocadesc[CMS_IFNAME_LENGTH];

      if (((WanMocaIntfObject *)mdmObj)->enable)
      {
         sprintf(mocadesc, "/%s", ((WanMocaIntfObject *)mdmObj)->ifName);
         cmsObj_free(&mdmObj);   /* no longer needed */
      }
      else
      {
         cmsObj_free(&mdmObj);   /* no longer needed */
         continue;
      }

      /* get iidstack of WANConnectionDevice */
      INIT_INSTANCE_ID_STACK(&iidStack1);
      if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &iidStack, &iidStack1, &mdmObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanConnDevice, ret=%d", ret);
         return;
      }
      cmsObj_free(&mdmObj);   /* no longer needed */

      INIT_INSTANCE_ID_STACK(&iidStack2);
      while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &iidStack1, &iidStack2, &mdmObj)) == CMSRET_SUCCESS)
      {
         strcat(varValue, ((WanIpConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
         strcat(varValue, mocadesc);
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
      while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &iidStack1, &iidStack2, &mdmObj)) == CMSRET_SUCCESS)
      {
         strcat(varValue, ((WanPppConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
         strcat(varValue, mocadesc);
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
#endif
#endif

#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1 
{
   InstanceIdStack cellularLinkIid = EMPTY_INSTANCE_ID_STACK;
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   PUSH_INSTANCE_ID(&cellularLinkIid, CMS_WANDEVICE_CELLULAR);

   while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &cellularLinkIid, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
         strcat(varValue, ((WanIpConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
         strcat(varValue, "(routed)");
         strcat(varValue, "|");
         cmsObj_free(&mdmObj);
   }
		 
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNextInSubTree <MDMOID_WAN_IP_CONN> returns error. ret=%d", ret);
      return;
   }
}
#endif

#ifdef DMP_ETHERNETWAN_1 
   /* get WAN Ethernet interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_WAN_ETH_INTF, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      InstanceIdStack iidStack1;
      InstanceIdStack iidStack2;

      if (((WanEthIntfObject *)mdmObj)->enable)
      {
         cmsObj_free(&mdmObj);   /* no longer needed */
      }
      else
      {
         cmsObj_free(&mdmObj);   /* no longer needed */
         continue;
      }

      /* get iidstack of WANConnectionDevice */
      INIT_INSTANCE_ID_STACK(&iidStack1);
      if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &iidStack, &iidStack1, &mdmObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanConnDevice, ret=%d", ret);
         return;
      }
      cmsObj_free(&mdmObj);   /* no longer needed */

      INIT_INSTANCE_ID_STACK(&iidStack2);
      while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &iidStack1, &iidStack2, &mdmObj)) == CMSRET_SUCCESS)
      {
         strcat(varValue, ((WanIpConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
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
      while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &iidStack1, &iidStack2, &mdmObj)) == CMSRET_SUCCESS)
      {
         strcat(varValue, ((WanPppConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
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
#endif

#ifdef DMP_ADSLWAN_1
   /* get WAN ATM and PTM interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_WAN_DSL_LINK_CFG, &iidStack, (void **)&linkCfg)) == CMSRET_SUCCESS)
   {
      InstanceIdStack iidStack2;

      if (!((WanDslLinkCfgObject *)linkCfg)->enable)
      {
         cmsObj_free(&linkCfg);
#ifdef DMP_PTMWAN_1
         if ((ret = cmsObj_get(MDMOID_WAN_PTM_LINK_CFG, &iidStack, 0, &linkCfg)) != CMSRET_SUCCESS)
         {
            break;
         }
         if (!(((WanPtmLinkCfgObject *)linkCfg)->enable &&
               ((WanPtmLinkCfgObject *)linkCfg)->X_BROADCOM_COM_PTMEnbQos))
         {
            cmsObj_free(&linkCfg);
            continue;
         }
         else
         {
            cmsObj_free(&linkCfg);
         }
#else
         continue;
#endif
      }
      else if (!((WanDslLinkCfgObject *)linkCfg)->X_BROADCOM_COM_ATMEnbQos)
      {
         cmsObj_free(&linkCfg);
         continue;
      }
      else
      {
         SINT32 vpi, vci;

         cmsUtl_atmVpiVciStrToNum(((WanDslLinkCfgObject *)linkCfg)->destinationAddress, &vpi, &vci);
         cmsObj_free(&linkCfg);
      }

      INIT_INSTANCE_ID_STACK(&iidStack2);
      while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &iidStack, &iidStack2, &mdmObj)) == CMSRET_SUCCESS)
      {
         strcat(varValue, ((WanIpConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
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
      while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &iidStack, &iidStack2, &mdmObj)) == CMSRET_SUCCESS)
      {
         strcat(varValue, ((WanPppConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
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
#endif /* DMP_ADSLWAN_1 */

}  /* End of cgiGetQosIntf() */

#endif  /* DMP_QOS_1 */


void cgiGetQosIntf(int argc, char **argv, char *varValue)
{
#if defined(SUPPORT_DM_LEGACY98)

   cgiGetQosIntf_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_HYBRID)

   cgiGetQosIntf_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_PURE181)

   cgiGetQosIntf_dev2(argc, argv, varValue);

#elif defined(SUPPORT_DM_DETECT)

   if (cmsMdm_isDataModelDevice2())
   {
      cgiGetQosIntf_dev2(argc, argv, varValue);
   }
   else
   {
      cgiGetQosIntf_dev2(argc, argv, varValue);
   }

#endif
}


void cgiGetQosMgmtEnbl(int argc, char **argv, char *varValue)
{
#if defined(SUPPORT_DM_LEGACY98)

   cgiGetQosMgmtEnbl_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_HYBRID)

   cgiGetQosMgmtEnbl_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_PURE181)

   cgiGetQosMgmtEnbl_dev2(argc, argv, varValue);

#elif defined(SUPPORT_DM_DETECT)

   if (cmsMdm_isDataModelDevice2())
   {
      cgiGetQosMgmtEnbl_dev2(argc, argv, varValue);
   }
   else
   {
      cgiGetQosMgmtEnbl_dev2(argc, argv, varValue);
   }

#endif
}


void cgiGetQosMgmtDefMark(int argc, char **argv, char *varValue)
{
#if defined(SUPPORT_DM_LEGACY98)

   cgiGetQosMgmtDefMark_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_HYBRID)

   cgiGetQosMgmtDefMark_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_PURE181)

   cgiGetQosMgmtDefMark_dev2(argc, argv, varValue);

#elif defined(SUPPORT_DM_DETECT)

   if (cmsMdm_isDataModelDevice2())
   {
      cgiGetQosMgmtDefMark_dev2(argc, argv, varValue);
   }
   else
   {
      cgiGetQosMgmtDefMark_dev2(argc, argv, varValue);
   }

#endif
}


void cgiGetQosMgmtDefQueue(int argc, char **argv, char *varValue)
{
#if defined(SUPPORT_DM_LEGACY98)

   cgiGetQosMgmtDefQueue_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_HYBRID)

   cgiGetQosMgmtDefQueue_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_PURE181)

   cgiGetQosMgmtDefQueue_dev2(argc, argv, varValue);

#elif defined(SUPPORT_DM_DETECT)

   if (cmsMdm_isDataModelDevice2())
   {
      cgiGetQosMgmtDefQueue_dev2(argc, argv, varValue);
   }
   else
   {
      cgiGetQosMgmtDefQueue_dev2(argc, argv, varValue);
   }

#endif
}


#ifdef DMP_QOS_1

void cgiGetQosMgmtEnbl_igd(int argc __attribute__((unused)),
                       char **argv __attribute__((unused)),
                       char *varValue)
{
   InstanceIdStack iidStack;
   QMgmtObject *qMgmtObj = NULL;
   CmsRet ret;

   varValue[0] = '\0';

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_Q_MGMT, &iidStack, 0, (void **)&qMgmtObj)) == CMSRET_SUCCESS)
   {
      sprintf(varValue, "%d", qMgmtObj->enable);
      cmsObj_free((void **)&qMgmtObj);
   }
   else
   {
      cmsLog_error("cmsObj_get returns error. ret=%d", ret);
   }
}  /* End of cgiGetQosMgmtEnbl() */

void cgiGetQosMgmtDefMark_igd(int argc __attribute__((unused)),
                          char **argv __attribute__((unused)),
                          char *varValue)
{
   InstanceIdStack iidStack;
   QMgmtObject *qMgmtObj = NULL;
   CmsRet ret;

   varValue[0] = '\0';

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_Q_MGMT, &iidStack, 0, (void **)&qMgmtObj)) == CMSRET_SUCCESS)
   {
      sprintf(varValue, "%d", qMgmtObj->defaultDSCPMark);
      cmsObj_free((void **)&qMgmtObj);        
   }
   else
   {
      cmsLog_error("cmsObj_get returns error. ret=%d", ret);
   }
}  /* End of cgiGetQosMgmtDefMark() */

void cgiGetQosMgmtDefQueue_igd(int argc __attribute__((unused)),
                           char **argv __attribute__((unused)),
                           char *varValue)
{
   InstanceIdStack iidStack;
   QMgmtObject *qMgmtObj = NULL;
   CmsRet ret;

   varValue[0] = '\0';

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_Q_MGMT, &iidStack, 0, (void **)&qMgmtObj)) == CMSRET_SUCCESS)
   {
      sprintf(varValue, "%d", qMgmtObj->defaultQueue);
      cmsObj_free((void **)&qMgmtObj);
   }
   else
   {
      cmsLog_error("cmsObj_get returns error. ret=%d", ret);
   }
}  /* End of cgiGetQosMgmtDefQueue() */

#endif  /* DMP_QOS_1 */



char *cgiQosDscpMarkToName(UINT8 mark)
{
   int i;
   char* dscpMarkDesc[] = {"auto", "nochange", "default",
                           "AF13", "AF12", "AF11", "CS1",
                           "AF23", "AF22", "AF21", "CS2",
                           "AF33", "AF32", "AF31", "CS3",
                           "AF43", "AF42", "AF41", "CS4",
                           "EF", "CS5", "CS6", "CS7", NULL};
   UINT8 dscpMarkValues[] = {DSCP_AUTO, DSCP_NO_CHANGE, DSCP_DEFAULT,
                             DSCP_AF13, DSCP_AF12, DSCP_AF11, DSCP_CS1,
                             DSCP_AF23, DSCP_AF22, DSCP_AF21, DSCP_CS2,
                             DSCP_AF33, DSCP_AF32, DSCP_AF31, DSCP_CS3,
                             DSCP_AF43, DSCP_AF42, DSCP_AF41, DSCP_CS4,
                             DSCP_EF, DSCP_CS5, DSCP_CS6, DSCP_CS7};

   if (mark == (UINT8)QOS_RESULT_NO_CHANGE)
   {
      return "&nbsp";
   }

   for (i = 0; dscpMarkDesc[i] != NULL; i++)
   {
      if (mark == dscpMarkValues[i])
      {
         return dscpMarkDesc[i];
      }
   }
   return dscpMarkDesc[0];

}  /* End of cgiQosDscpMarkToName() */

#endif  /* SUPPORT_QOS */

