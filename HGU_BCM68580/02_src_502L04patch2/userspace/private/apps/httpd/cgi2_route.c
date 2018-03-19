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

#include "cms.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/syslog.h>
#include <net/if_arp.h>
#include <net/route.h>
#include "cgi_cmd.h"
#include "cgi_ntwk.h"
#include "cgi_route.h"
#include "cgi_main.h"
#include "syscall.h"
#include "sysdiag.h"
#include "httpd.h"


#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_qdm.h"


#ifdef DMP_DEVICE2_ROUTING_1
void cgiRtRouteViewBody_dev2(FILE *fs) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   void *obj = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   Dev2DnsServerObject *dnsServerObj=NULL;
   char l3IntfNameBuf[CMS_IFNAME_LENGTH]={0};
   
   /* Get the objects one after another till we fail. */
   while ((ret = cmsObj_getNextFlags(MDMOID_DEV2_IPV4_FORWARDING, &iidStack, OGF_NO_VALUE_UPDATE, &obj)) == CMSRET_SUCCESS)
   {
      Dev2Ipv4ForwardingObject *routeCfg = (Dev2Ipv4ForwardingObject *)obj;

     if (IS_EMPTY_STRING(routeCfg->X_BROADCOM_COM_PolicyRoutingName) && !IS_EMPTY_STRING(routeCfg->destIPAddress))
     {
         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>4</td>\n");

         if (IS_EMPTY_STRING(routeCfg->destIPAddress))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            if (IS_EMPTY_STRING(routeCfg->destSubnetMask))
            {
               fprintf(fs, "      <td>%s</td>\n", routeCfg->destIPAddress);
            } 
            else
            {
               fprintf(fs, "      <td>%s/%d</td>\n", routeCfg->destIPAddress,
                       cmsNet_getLeftMostOneBitsInMask(routeCfg->destSubnetMask));
            }
         } 

         if (IS_EMPTY_STRING(routeCfg->gatewayIPAddress))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", routeCfg->gatewayIPAddress);
         }

         if (IS_EMPTY_STRING(routeCfg->interface))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            char l3IntfNameBuf[CMS_IFNAME_LENGTH]={0};

            if ((ret = qdmIntf_fullPathToIntfnameLocked(routeCfg->interface, l3IntfNameBuf)) != CMSRET_SUCCESS)
            {
               cmsLog_error("qdmIntf_fullPathToIntfnameLocked on %s ret=%d",
                             routeCfg->interface, ret);
            }
            else
               fprintf(fs, "      <td>%s</td>\n", l3IntfNameBuf);
         }

         if (routeCfg->forwardingMetric < 0)
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%d</td>\n", routeCfg->forwardingMetric);
         }

         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s|%s'></td>\n",
                    routeCfg->destIPAddress, routeCfg->destSubnetMask);
         fprintf(fs, "   </tr>\n");

         glbEntryCt++;
      }

      /* free routeCfg */
      cmsObj_free((void **) &routeCfg);
   }

#ifdef SUPPORT_IPV6
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNextFlags(MDMOID_DEV2_IPV6_FORWARDING, &iidStack, OGF_NO_VALUE_UPDATE, &obj)) == CMSRET_SUCCESS)
   {
      Dev2Ipv6ForwardingObject *route6Cfg = (Dev2Ipv6ForwardingObject *)obj;
	  
      if (!IS_EMPTY_STRING(route6Cfg->destIPPrefix))
      {
         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>6</td>\n");

         if (IS_EMPTY_STRING(route6Cfg->destIPPrefix))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", route6Cfg->destIPPrefix);
         } 

         if (IS_EMPTY_STRING(route6Cfg->nextHop))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", route6Cfg->nextHop);
         }

         if (IS_EMPTY_STRING(route6Cfg->interface))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            char l3IntfNameBuf[CMS_IFNAME_LENGTH]={0};
            /* convert mdm full path string to queue interface name */
            if ((ret = qdmIntf_fullPathToIntfnameLocked(route6Cfg->interface, l3IntfNameBuf)) != CMSRET_SUCCESS)
            {
               cmsLog_error("qdmIntf_fullPathToIntfnameLocked on %s ret=%d",
                             route6Cfg->interface, ret);
            }
            else
               fprintf(fs, "      <td>%s</td>\n", l3IntfNameBuf);
         }

         if (route6Cfg->forwardingMetric < 0)
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%d</td>\n", route6Cfg->forwardingMetric);
         }

         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n",
                 route6Cfg->destIPPrefix);
         fprintf(fs, "   </tr>\n");

         glbEntryCt++;
      }

      /* free routeCfg */
      cmsObj_free((void **) &route6Cfg);
   }
#endif


   /*  Need to display dns system static route at the end of user defined static route
    * those system static route cannot be removed and is for display only.
    * Find any connected routed Layer 3 interface and search the proc/net with interface name
    * primary dns ip and gateway to find a match and then display.
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNextFlags(MDMOID_DEV2_DNS_SERVER,
                             &iidStack, OGF_NO_VALUE_UPDATE,
                             (void **) &dnsServerObj)) == CMSRET_SUCCESS)
   {
      if (!IS_EMPTY_STRING(dnsServerObj->interface) &&
         (ret = qdmIntf_fullPathToIntfnameLocked(dnsServerObj->interface, l3IntfNameBuf)) == CMSRET_SUCCESS)
      {
         Dev2Ipv4ForwardingObject *routeCfg;
         UBOOL8 found = FALSE;
		 
         while (!found && 
                   (ret = cmsObj_getNextFlags(MDMOID_DEV2_IPV4_FORWARDING, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&routeCfg)) == CMSRET_SUCCESS)
         {
            /* get interface default gateway */         
            if (routeCfg->destIPAddress == NULL && cmsUtl_strcmp(routeCfg->interface, dnsServerObj->interface) == 0)
            {
               cmsLog_debug("found dns, l3ifname=%s, DNSServer=%s, gatewayIPAddress=%s",
                  l3IntfNameBuf,dnsServerObj->DNSServer, routeCfg->gatewayIPAddress);
               displaySystemRouteIfMatched(fs, l3IntfNameBuf, dnsServerObj->DNSServer, routeCfg->gatewayIPAddress);
               found = TRUE;
            }
            cmsObj_free((void **) &routeCfg);
          }
       }
       else
       {
           cmsLog_notice("invalid dnsServer interface %s ret=%d",
                         dnsServerObj->interface, ret);
       }
      cmsObj_free((void **) &dnsServerObj);
   }
}


void cgiRtRouteViewGetServiceName_dev2(const char *ifName, char *serviceName)
{
   Dev2IpInterfaceObject *ipIntf=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;

   if (ifName == NULL || serviceName == NULL)
   {
      cmsLog_error("input args is NULL");
      return;
   }

   while (!found && cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack, (void **)&ipIntf) == CMSRET_SUCCESS)
   {
      cmsLog_debug(" ipIntf->name %s, ipIntf->alias %s", ipIntf->name, ipIntf->alias);
	  
      if (cmsUtl_strcmp(ipIntf->name, ifName) == 0)
      {
         strcpy(serviceName, ipIntf->alias);
         found = TRUE;
      }
	  
      cmsObj_free((void **)&ipIntf);  
   }
   
   if (found == TRUE)
   {
      return;
   }


   /* if not found, just add blank.  The ifName could be br0 */
   strcpy(serviceName, "&nbsp;");
}


#ifdef SUPPORT_RIP
UBOOL8 cgiRtRipViewBody_dev2(FILE *fs)
{
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2RipIntfSettingObject *ripIntfSetting = NULL;
   UBOOL8 foundIfc = FALSE;
   
   /* add lan interface as well for CDRoute tests ? or just enable that in cli?  TODO*/
   /* cgi_ripdLanInterface(fs); */
   
   while ((ret = cmsObj_getNext(MDMOID_DEV2_RIP_INTF_SETTING, &iidStack, (void **) &ripIntfSetting)) == CMSRET_SUCCESS)
   {
      /* only routed connections (not bridged) can have RIP running on it */
      char l3IntfNameBuf[CMS_IFNAME_LENGTH]={0};
      if ((ret = qdmIntf_fullPathToIntfnameLocked(ripIntfSetting->interface, l3IntfNameBuf)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmIntf_fullPathToIntfnameLocked on %s ret=%d",
                      ripIntfSetting->interface, ret);
      }
      else
      {
         fprintf(fs, "    <tr align='center'>\n");
         fprintf(fs, "      <td>%s</td>\n", l3IntfNameBuf);
         fprintf(fs, "      <td><select name='ripVer'>\n");
         /* if "Off", default to version 2 */
         if (ripIntfSetting->X_BROADCOM_COM_Version == 1)
         {
            fprintf(fs, "            <option value='1' selected>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1&nbsp;&nbsp;\n");
            fprintf(fs, "            <option value='2'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2&nbsp;&nbsp;\n");
            fprintf(fs, "            <option value='3'>&nbsp;&nbsp;Both&nbsp;&nbsp;</option>\n");
         }
         else if (ripIntfSetting->X_BROADCOM_COM_Version == 2)
         {
            fprintf(fs, "            <option value='1'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1&nbsp;&nbsp;\n");
            fprintf(fs, "            <option value='2' selected>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2&nbsp;&nbsp;\n");
            fprintf(fs, "            <option value='3'>&nbsp;&nbsp;Both&nbsp;&nbsp;</option>\n");
         }
         else if (ripIntfSetting->X_BROADCOM_COM_Version == 3)
         {
            fprintf(fs, "            <option value='1'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1&nbsp;&nbsp;\n");
            fprintf(fs, "            <option value='2'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2&nbsp;&nbsp;\n");
            fprintf(fs, "            <option value='3' selected>&nbsp;&nbsp;Both&nbsp;&nbsp;</option>\n");
         }
         else
         {
            cmsLog_error("Error rip version");
            cmsObj_free((void **)&ripIntfSetting);
         }
         
         fprintf(fs, "         </select></td>\n");
         fprintf(fs, "      <td>&nbsp;&nbsp;&nbsp;<select name='ripOp'>\n");
    
         if (qdmIpIntf_isNatEnabledOnIntfNameLocked_dev2(l3IntfNameBuf))
         {
            /* if NAT enabled, only passive is allowed */
            fprintf(fs, "         <option value='1' selected>Passive</option>\n");
         }
         else
         {
            if (ripIntfSetting->sendRA)
            {
               fprintf(fs, "         <option value='0' selected>Active\n");
               fprintf(fs, "         <option value='1'>Passive</option>\n");
            }
            else
            {
               fprintf(fs, "         <option value='0'>Active\n");
               fprintf(fs, "         <option value='1' selected>Passive</option>\n");
            }
         }
    	 
         fprintf(fs, "         </select></td>\n");
    
         if (!ripIntfSetting->enable)
         {
            fprintf(fs, "      <td align='center'><input type='checkbox' name='riplist' value='%s'></td>\n",l3IntfNameBuf);
         }
         else
         {
            fprintf(fs, "      <td align='center'><input type='checkbox' name='riplist' value='%s' \
                        checked></td>\n", l3IntfNameBuf);
         }
         fprintf(fs, "    </tr>\n");
         
         foundIfc = TRUE;
      }
      cmsObj_free((void **) &ripIntfSetting);
   }
   
   return foundIfc;   
}
#endif


#ifdef SUPPORT_POLICYROUTING
void cgiPrViewBody_dev2( FILE *fs ) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Ipv4ForwardingObject *routeCfg = NULL;
   char l3IntfNameBuf[CMS_IFNAME_LENGTH]={0};

   /* Get the objects one after another till we fail. */
   while (cmsObj_getNext(MDMOID_DEV2_IPV4_FORWARDING, &iidStack, (void **) &routeCfg) == CMSRET_SUCCESS)
   {
      if (!IS_EMPTY_STRING(routeCfg->X_BROADCOM_COM_PolicyRoutingName))
      {
         fprintf(fs, "   <tr>\n");

         fprintf(fs, "      <td>%s</td>\n", routeCfg->X_BROADCOM_COM_PolicyRoutingName);

         if (IS_EMPTY_STRING(routeCfg->X_BROADCOM_COM_SourceIPAddress))
         {
            fprintf(fs, "      <td>%s</td>\n", "&nbsp");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", routeCfg->X_BROADCOM_COM_SourceIPAddress);
         }

         if (IS_EMPTY_STRING(routeCfg->X_BROADCOM_COM_SourceIfName))
         {
            fprintf(fs, "      <td>%s</td>\n", "&nbsp");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", routeCfg->X_BROADCOM_COM_SourceIfName);
         }

         if (qdmIntf_fullPathToIntfnameLocked(routeCfg->interface, l3IntfNameBuf) != CMSRET_SUCCESS)
         {
            cmsLog_error("qdmIntf_fullPathToIntfnameLocked on %s", routeCfg->interface);
            fprintf(fs, "      <td>%s</td>\n", routeCfg->interface);
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", l3IntfNameBuf);
         }

         if (IS_EMPTY_STRING(routeCfg->gatewayIPAddress))
         {
            fprintf(fs, "      <td>%s</td>\n", "&nbsp");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", routeCfg->gatewayIPAddress);
         }

         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", routeCfg->X_BROADCOM_COM_PolicyRoutingName);
         fprintf(fs, "   </tr>\n");
   
         glbEntryCt++;
      }
      /* free routeCfg */
      cmsObj_free((void **) &routeCfg);
   }
}
#endif /* SUPPORT_POLICYROUTING */

#endif /* DMP_DEVICE2_ROUTING_1 */

#endif /* DMP_DEVICE2_BASELINE_1 */
