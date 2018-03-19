/***********************************************************************
 *
 *  Copyright (c) 2006  Broadcom Corporation
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <net/route.h>
#include <sys/ioctl.h>

#include "httpd.h"
#include "cgi_main.h"
#include "cms_qdm.h"
#include "cgi_sts.h"
#include "cgi_diag.h"
#include "cgi_util.h"
#include "sysdiag.h"

#ifdef BRCM_WLAN
#ifndef SUPPORT_UNIFIED_WLMNGR 
#include "wlapi.h"
#endif
#endif

#include "cms_dal.h"
#include "cms_msg.h"

#ifdef DMP_X_BROADCOM_COM_ETHERNETOAM_1
#include "prctl.h"
#endif

void cgiGetDiagId_igd(int type, PDIAG_ID pDiagId)
{
   char info[BUFLEN_64];
   char *cp1 = NULL;
   InstanceIdStack iidStack;
   InstanceIdStack   wanDevIid;
   WanDevObject      *wanDev  = NULL;
   WanPppConnObject  *pppCon = NULL;   
   WanIpConnObject   *ipCon = NULL;

   info[0] = '\0';
   CmsRet ret;
   WanLinkType linkType = ATM;

   memset(pDiagId,0,sizeof(DIAG_ID));
   
   cmsLog_debug("type %d, WEB_DIAG_PREV/WEB_DIAG_CURR/WEB_DIAG_NEXT %d/%d/%d\n", type,
                WEB_DIAG_PREV,WEB_DIAG_CURR,WEB_DIAG_NEXT);

   switch ( type ) 
   {
      case WEB_DIAG_PREV:
         strncpy(info, glbDiagInfo[WEB_DIAG_PREV], BUFLEN_64);
         break;
      case WEB_DIAG_CURR:
         strncpy(info, glbDiagInfo[WEB_DIAG_CURR], BUFLEN_64);
         break;
      case WEB_DIAG_NEXT:
         strncpy(info, glbDiagInfo[WEB_DIAG_NEXT], BUFLEN_64);
         break;
   }
   
   if ( info[0] != '\0' ) 
   {
      // info should have the format "protocol/layer3Name"
      cp1 = strstr(info, "/");
      if ( cp1 == NULL ) return;
      *cp1 = '\0';
      strcpy(pDiagId->protocolStr,info);
      strcpy(pDiagId->layer3Name,cp1+1);
   }
   else
   {
      // get the first availabe WAN connection ID
      INIT_INSTANCE_ID_STACK(&wanDevIid);
      while (cmsObj_getNext(MDMOID_WAN_DEV, &wanDevIid, (void **)&wanDev) == CMSRET_SUCCESS)
      {
         WanDslLinkCfgObject     *dslLink = NULL;
         WanCommonIntfCfgObject  *comIntf = NULL;
         InstanceIdStack         conDevIid;
      
         cmsObj_free((void **)&wanDev);  /* no longer needed */

         /* Find out the types of the WANDevice object */
         if (cmsObj_get(MDMOID_WAN_COMMON_INTF_CFG, &wanDevIid, 0, (void **)&comIntf) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_getNext <MDMOID_WAN_COMMON_INTF_CFG> returns error.");
            return;
         }

         if( !cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_DSL) )
         {
            WanDslIntfCfgObject *dslIntfCfg = NULL;
            
            if ((ret = cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &wanDevIid, 0, (void **)&dslIntfCfg)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_getNextInSubTree  <MDMOID_WAN_DSL_INTF_CFG> error. ret=%d", ret);
               /* should never happen */
               cmsObj_free((void **) &comIntf);
               return;
            }
            cmsLog_debug("dslIntfCfg->linkEncapsulationUsed=%s", dslIntfCfg->linkEncapsulationUsed);
            
            if (cmsUtl_strcmp(dslIntfCfg->linkEncapsulationUsed, MDMVS_G_992_3_ANNEX_K_ATM) == 0)
            {
               linkType = ATM;
            }
            else
            {
               linkType = PTM;
               /* skip vpi/vci */
            }
            cmsObj_free((void **)&dslIntfCfg);
         }
         else if( !cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_ETHERNET) )
         {
            WanEthIntfObject *ethIntf = NULL;

            linkType = Ethernet;

            if (cmsObj_get(MDMOID_WAN_ETH_INTF, &wanDevIid, 0, (void **)&ethIntf) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_getNext <MDMOID_WAN_ETH_INTF> returns error.");
               cmsObj_free((void **)&comIntf);
               return;
            }
            cmsObj_free((void **)&ethIntf);
         }
         cmsObj_free((void **)&comIntf);

         INIT_INSTANCE_ID_STACK(&conDevIid);         
         while (cmsObj_getNextInSubTree(MDMOID_WAN_DSL_LINK_CFG, &wanDevIid, &conDevIid, (void **)&dslLink) == CMSRET_SUCCESS)
         {
            /* get the related ipCon obj */
            INIT_INSTANCE_ID_STACK(&iidStack);
            while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &conDevIid, &iidStack, (void **)&ipCon) == CMSRET_SUCCESS)
            {
               strcpy(pDiagId->layer3Name, ipCon->X_BROADCOM_COM_IfName);

               /* form protocolStr */
               if (linkType == ATM)
               {
                  if (!cmsUtl_strcmp(dslLink->linkType, MDMVS_IPOA))
                  {
                     strcpy(pDiagId->protocolStr, IPOA_PROTO_STR);
                  }
                  else if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_BRIDGED))
                  {
                     strcpy(pDiagId->protocolStr,  BRIDGE_PROTO_STR);
                  }
                  else if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_ROUTED))
                  {
                     strcpy(pDiagId->protocolStr, IPOE_PROTO_STR);
                  }
               }
               else if (linkType == PTM)
               {
                  if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_BRIDGED))
                  {
                     strcpy(pDiagId->protocolStr,  BRIDGE_PROTO_STR);
                  }
                  else if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_ROUTED))
                  {
                     strcpy(pDiagId->protocolStr, IPOE_PROTO_STR);
                  }
               }
#ifdef DMP_ETHERNETWAN_1
               else  /* EthWan */
               {
                  if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_ROUTED))
                  {
                     strcpy(pDiagId->protocolStr, IPOE_PROTO_STR);
                  }
                  else
                  {
                     strcpy(pDiagId->protocolStr, BRIDGE_PROTO_STR);
                  }                  
               }
#endif
               cmsObj_free((void **)&ipCon);
            } /* ipConn */
   
            /* get the related pppCon obj */
            INIT_INSTANCE_ID_STACK(&iidStack);
            while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &conDevIid, &iidStack, (void **)&pppCon) == CMSRET_SUCCESS)
            {
               strcpy(pDiagId->layer3Name, pppCon->X_BROADCOM_COM_IfName);
               
               if (linkType == ATM)
               {      
                  if (!cmsUtl_strcmp(dslLink->linkType, MDMVS_EOA))
                  {   
                     strcpy(pDiagId->protocolStr, PPPOE_PROTO_STR);
                  }
                  else if (!cmsUtl_strcmp(dslLink->linkType, MDMVS_PPPOA))
                  {
                     strcpy(pDiagId->protocolStr, PPPOA_PROTO_STR);
                  }
               }
               else if (linkType == PTM)
               {
                  if (!cmsUtl_strcmp(pppCon->connectionType, MDMVS_IP_ROUTED))
                  {   
                     strcpy(pDiagId->protocolStr, PPPOE_PROTO_STR);
                  }
               }
#ifdef DMP_ETHERNETWAN_1
               else  /* EthWan */
               {
                  if (!cmsUtl_strcmp(pppCon->connectionType, MDMVS_IP_ROUTED))
                  {   
                     strcpy(pDiagId->protocolStr, PPPOE_PROTO_STR);
                  }
               }
#endif
               cmsObj_free((void **)&pppCon);
            } /*while ppp Conn */
            cmsObj_free((void **)&dslLink);
         } /* dsl link */
      } /* wan dev */
   } /* info not found */
} /* cgiGetDiagId */

void cgiGetDiagInfo(int argc __attribute__((unused)),
                    char **argv, char *varValue)
{
   /* call DM depending function */
   cgiDmGetDiagInfo(argv, varValue);
}
void cgiDmGetDiagInfo_igd(char **argv, char *varValue)
{
   WanDevObject      *wanDev  = NULL;
   InstanceIdStack   wanDevIid;
   char protocolStr[BUFLEN_16]={0};
   WanLinkType linkType = ATM;
   CmsRet ret;
   char info[BUFLEN_40];
   DIAG_ID diagId;
   InstanceIdStack   iidStack;
   WanPppConnObject  *pppCon = NULL;   
   WanIpConnObject   *ipCon = NULL;
   UBOOL8 found = FALSE;
   int len;

   cmsLog_debug("Enter, argv[2] %s",argv[2]);

   if ( strcmp(argv[2], "diag") == 0 ) 
   {   
      /*
       * each PVC's diag info is separated by |, diag info is a string in the following format:
       * protocol1/layer3Ifname1|protocol2/layer3Ifname2|protocol3/layer3Ifname3|
       *
       */

      INIT_INSTANCE_ID_STACK(&wanDevIid);
      while (cmsObj_getNext(MDMOID_WAN_DEV, &wanDevIid, (void **)&wanDev) == CMSRET_SUCCESS)
      {
         WanDslLinkCfgObject     *dslLink = NULL;
         WanCommonIntfCfgObject  *comIntf = NULL;
         InstanceIdStack         conDevIid;
      
         cmsObj_free((void **)&wanDev);  /* no longer needed */

         /* Find out the types of the WANDevice object */
         if (cmsObj_get(MDMOID_WAN_COMMON_INTF_CFG, &wanDevIid, 0, (void **)&comIntf) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_getNext <MDMOID_WAN_COMMON_INTF_CFG> returns error.");
            return;
         }

         if( !cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_DSL) )
         {
            WanDslIntfCfgObject *dslIntfCfg = NULL;
            
            if ((ret = cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &wanDevIid, 0, (void **)&dslIntfCfg)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_getNextInSubTree  <MDMOID_WAN_DSL_INTF_CFG> error. ret=%d", ret);
               cmsObj_free((void **)&comIntf);
               return;
            }
            if (cmsUtl_strcmp(dslIntfCfg->linkEncapsulationUsed, MDMVS_G_992_3_ANNEX_K_ATM) == 0)
            {
               linkType = ATM;
            }
            else
            {
               linkType = PTM;
            }
            cmsObj_free((void **)&dslIntfCfg);
         }
         else if( !cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_ETHERNET) )
         {
            linkType = Ethernet;
         }
         cmsObj_free((void **)&comIntf);
         
         /* Get the PVC information */
         INIT_INSTANCE_ID_STACK(&conDevIid);
         while (cmsObj_getNextInSubTree(MDMOID_WAN_DSL_LINK_CFG, &wanDevIid, &conDevIid, (void **)&dslLink) == CMSRET_SUCCESS)
         {
            info[0] = '\0';

            /* get the related ipCon obj */
            INIT_INSTANCE_ID_STACK(&iidStack);
            while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &conDevIid, &iidStack, (void **)&ipCon) == CMSRET_SUCCESS)
            {
               /* form protocolStr */
               if (linkType == ATM)
               {
                  if (!cmsUtl_strcmp(dslLink->linkType, MDMVS_IPOA))
                  {
                     strcpy(protocolStr, IPOA_PROTO_STR);
                  }
                  else if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_BRIDGED))
                  {
                     strcpy(protocolStr,  BRIDGE_PROTO_STR);
                  }
                  else if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_ROUTED))
                  {
                     strcpy(protocolStr, IPOE_PROTO_STR);
                  }
               }
               else if (linkType == PTM)
               {
                  if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_BRIDGED))
                  {
                     strcpy(protocolStr,  BRIDGE_PROTO_STR);
                  }
                  else if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_ROUTED))
                  {
                     strcpy(protocolStr, IPOE_PROTO_STR);
                  }
               }
#ifdef DMP_ETHERNETWAN_1
               else  /* EthWan */
               {
                  if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_ROUTED))
                  {
                     strcpy(protocolStr, IPOE_PROTO_STR);
                  }
                  else
                  {
                     strcpy(protocolStr, BRIDGE_PROTO_STR);
                  }
               }
#endif

               sprintf(info, "%s/%s|", protocolStr, ipCon->X_BROADCOM_COM_IfName);
               strcat(varValue, info);               
               cmsLog_debug("info %s",info);
               cmsObj_free((void **)&ipCon);
            }
   
            /* get the related pppCon obj */
            INIT_INSTANCE_ID_STACK(&iidStack);
            while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &conDevIid, &iidStack, (void **)&pppCon) == CMSRET_SUCCESS)
            {
               /* layer 2 ifName: service string (pppoe_0_0_35) */
               /*                snprintf(serviceStr, sizeof(serviceStr), pppCon->name); */
               /* layer 3 ifName get interfaceStr (nas_0_0_35) */
               /*                snprintf(interfaceStr, sizeof(interfaceStr), pppCon->X_BROADCOM_COM_IfName); */
               
               if (linkType == ATM)
               {      
                  if (!cmsUtl_strcmp(dslLink->linkType, MDMVS_EOA))
                  {   
                     strcpy(protocolStr, PPPOE_PROTO_STR);
                  }
                  else if (!cmsUtl_strcmp(dslLink->linkType, MDMVS_PPPOA))
                  {
                     strcpy(protocolStr, PPPOA_PROTO_STR);
                  }
               }
               else if (linkType == PTM)
               {
                  if (!cmsUtl_strcmp(pppCon->connectionType, MDMVS_IP_ROUTED))
                  {   
                     strcpy(protocolStr, PPPOE_PROTO_STR);
                  }
               }
#ifdef DMP_ETHERNETWAN_1
               else  /* EthWan */
               {
                  if (!cmsUtl_strcmp(pppCon->connectionType, MDMVS_IP_ROUTED))
                  {   
                     strcpy(protocolStr, PPPOE_PROTO_STR);
                  }
               }
#endif
               sprintf(info, "%s/%s|", protocolStr, pppCon->X_BROADCOM_COM_IfName);
               strcat(varValue, info);               
               cmsLog_debug("info %s",info);
               cmsObj_free((void **)&pppCon);
            }
            cmsObj_free((void **)&dslLink);
         }
      }
      len = strlen(varValue);
      if ( len > 0 )
      {
         varValue[len-1] = '\0';   // remove the last '|' character
      }
   }/* diag is the 2nd arguement */
   else
   {
      /* get diag info by field name */
      varValue[0] = '\0';
      cgiGetDiagId(WEB_DIAG_CURR, &diagId);
         
      if (diagId.layer3Name[0] == '0')
      {
         /* there is no connection */
         return;
      }

      INIT_INSTANCE_ID_STACK(&wanDevIid);
      while (!found && cmsObj_getNext(MDMOID_WAN_DEV, &wanDevIid, (void **)&wanDev) == CMSRET_SUCCESS)
      {
         WanDslLinkCfgObject  *dslLink = NULL;
         InstanceIdStack      conDevIid;

         cmsObj_free((void **)&wanDev);  /* no longer needed */

         INIT_INSTANCE_ID_STACK(&conDevIid);
         while (!found && cmsObj_getNextInSubTree(MDMOID_WAN_DSL_LINK_CFG, &wanDevIid, &conDevIid, (void **)&dslLink) == CMSRET_SUCCESS)
         {
            InstanceIdStack  iidStack;
            WanPppConnObject *pppCon = NULL;
            WanIpConnObject  *ipCon = NULL;

            /* get the related ipCon obj */
            INIT_INSTANCE_ID_STACK(&iidStack);
            while (!found && cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &conDevIid, &iidStack, (void **)&ipCon) == CMSRET_SUCCESS)
            {
               if ((cmsUtl_strcmp(ipCon->X_BROADCOM_COM_IfName, diagId.layer3Name)) == 0)
               {
                  if ( strcmp(argv[2], "serviceName") == 0 )
                  {
                     strcpy(varValue, ipCon->name);
                  }
                  else if ( strcmp(argv[2], "deviceName") == 0 )
                  {
                     strcpy(varValue, ipCon->X_BROADCOM_COM_IfName);
                  }
                  else if ( strcmp(argv[2], "diagType") == 0 )
                  {
                     strcpy(varValue, glbDiagInfo[WEB_DIAG_TYPE]);
                  }
                  else if ( strcmp(argv[2], "diagPrev") == 0 )
                  {
                     strcpy(varValue, glbDiagInfo[WEB_DIAG_PREV]);
                  }
                  else if ( strcmp(argv[2], "diagCurr") == 0 )
                  {
                     strcpy(varValue, glbDiagInfo[WEB_DIAG_CURR]);
                  }
                  else if ( strcmp(argv[2], "diagNext") == 0 )
                  {
                     strcpy(varValue, glbDiagInfo[WEB_DIAG_NEXT]);
                  }                  
                  found = TRUE;            
                  cmsObj_free((void **)&ipCon);
                  break;
               } /* name found */
               cmsObj_free((void **)&ipCon);
            } /* ipconn */
            
            INIT_INSTANCE_ID_STACK(&iidStack);
            while (!found && cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &conDevIid, &iidStack, (void **)&pppCon) == CMSRET_SUCCESS)
            {
               if ((cmsUtl_strcmp(pppCon->X_BROADCOM_COM_IfName, diagId.layer3Name)) == 0)
               {
                  if ( strcmp(argv[2], "serviceName") == 0 )
                  {
                     strcpy(varValue, pppCon->name);
                  }
                  else if ( strcmp(argv[2], "deviceName") == 0 )
                  {
                     strcpy(varValue, pppCon->X_BROADCOM_COM_IfName);
                  }
                  else if ( strcmp(argv[2], "diagType") == 0 )
                  {
                     strcpy(varValue, glbDiagInfo[WEB_DIAG_TYPE]);
                  }
                  else if ( strcmp(argv[2], "diagPrev") == 0 )
                  {
                     strcpy(varValue, glbDiagInfo[WEB_DIAG_PREV]);
                  }
                  else if ( strcmp(argv[2], "diagCurr") == 0 )
                  {
                     strcpy(varValue, glbDiagInfo[WEB_DIAG_CURR]);
                  }
                  else if ( strcmp(argv[2], "diagNext") == 0 )
                  {
                     strcpy(varValue, glbDiagInfo[WEB_DIAG_NEXT]);
                  }                  
                  found = TRUE;            
                  cmsObj_free((void **)&pppCon);
                  break;
               } /* name found */
               cmsObj_free((void **)&pppCon);
            } /* while ppp */
            cmsObj_free((void **) &dslLink);
         } /* while dslLinkCfg */
      } /* wan dev */
   } /* else get by field name*/

   cmsLog_debug("varValue =%s", varValue);
}

void cgiGetDiagnostic(int argc __attribute__((unused)),
                      char **argv, char *varValue)
{
   int diag = atoi(argv[2]);
   DIAG_ID diagId;

   cmsLog_debug("*********************request from html, cgiGetDiagnostic type %d *****************", diag);

   switch (diag)
   {
#ifdef SUPPORT_USB
   case BCM_DIAG_USB_CONN:
      cgiPrintUsbDiag(varValue);
      break;
#endif /* SUPPORT_USB */

#ifdef BRCM_WLAN
   case BCM_DIAG_WIRELESS_CONN:
      cgiPrintWirelessDiag(varValue);
      break;
#endif

#ifdef SUPPORT_DSL
   case BCM_DIAG_ADSL_CONN:
      cgiPrintAdslConnDiag(varValue);
      break;

   case BCM_DIAG_OAM_LOOPBACK_SEG:
   case BCM_DIAG_OAM_LOOPBACK_END:
   case BCM_DIAG_OAM_F4_LOOPBACK_SEG:
   case BCM_DIAG_OAM_F4_LOOPBACK_END:
      cgiGetDiagId(WEB_DIAG_CURR, &diagId);

      if (diag == BCM_DIAG_OAM_LOOPBACK_SEG)
      {
         cgiPrintOAMLoopbackDiag(varValue, BCM_DIAG_OAM_SEGMENT, &diagId);
      }
      else if (diag == BCM_DIAG_OAM_LOOPBACK_END)
      {
         cgiPrintOAMLoopbackDiag(varValue, BCM_DIAG_OAM_END2END, &diagId);
      }
      else if (diag == BCM_DIAG_OAM_F4_LOOPBACK_SEG)
      {
         cgiPrintOAMLoopbackDiag(varValue, BCM_DIAG_OAM_F4_SEGMENT, &diagId);
      }
      else
      {
         cgiPrintOAMLoopbackDiag(varValue, BCM_DIAG_OAM_F4_END2END, &diagId);
      }
      break;
#endif /* SUPPORT_DSL */

   case BCM_DIAG_PPPOE_CONN:
   case BCM_DIAG_PPPOE_SESSION:
   case BCM_DIAG_PPPOE_AUTH:
   case BCM_DIAG_PPPOE_IP_ADDR:
      cgiGetDiagId(WEB_DIAG_CURR, &diagId);
      cgiPrintPppDiag(varValue,diag,&diagId);
      break;

#if defined(DMP_DEVICE2_IPPING_1) || defined(DMP_IPPING_1)
   case BCM_DIAG_PING_DEF_GW:
      cgiPrintDefaultGatewayDiag(varValue);
      break;

   case BCM_DIAG_PING_DNS:
      cgiPrintDnsDiag(varValue);
      break;
#endif

   case BCM_DIAG_ADSL_BERT:
      break;

   default:
      break;
   } /* switch diag */
}

/* 
 * cgiPrintEnetDiag is replacing BcmLan_printEnetDiag. in utils.c.
 * This routine print ENET diagnostics page.
 */
void cgiPrintEnetDiag_igd(char *varValue)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *ethObj=NULL;
   char *p = varValue;

   cmsLog_debug("Enter");

   p += sprintf(p, "var brdId = '%s';\n", glbWebVar.boardID);
   p += sprintf(p, "var intfDisp = '';\n");
   p += sprintf(p, "var brdIntf = '';\n");

   while (cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj) == CMSRET_SUCCESS)
   {

      cmsLog_debug("cmsObj_getNext returns ethObj->X_BROADCOM_COM_IfName %s, status %s",
                   ethObj->X_BROADCOM_COM_IfName,ethObj->status);
      p += sprintf(p, "brdIntf = brdId + '|' + '%s';\n", ethObj->X_BROADCOM_COM_IfName);
      p += sprintf(p, "intfDisp = getUNameByLName(brdIntf);\n");
      p += sprintf(p, "document.writeln(\"<tr><td class='hd'>Test your \" + intfDisp +  \" Connection: </td>\");\n");

      if (cmsUtl_strcmp(ethObj->status, MDMVS_UP) == 0)
      {
         p += sprintf(p, "document.writeln(\"<td><b> <font color='green'>PASS</font> </b></td>\");\n");
      }
      else
      {
         p += sprintf(p, "document.writeln(\"<td><b> <font color='red'>FAIL</font> </b></td>\");\n");
      }

      p += sprintf(p, "document.writeln(\"<td><A href='hlpethconn.html'>Help</A></td> </tr>\");\n");

      cmsObj_free((void **) &ethObj);
   }

   *p = 0;
   return;
}

#ifdef DMP_USBLAN_1
/* 
 * This routine print USB diagnostic status.
 */
void cgiPrintUsbDiag_igd(char *varValue)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanUsbIntfObject *usbObj=NULL;
   char *p = varValue;

   cmsLog_debug("Enter");

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_LAN_USB_INTF, &iidStack, (void **) &usbObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("cmsObj_getNext returns usbObj->X_BROADCOM_COM_IfName %s, status %s",        
                   usbObj->X_BROADCOM_COM_IfName,usbObj->status);

      if (cmsUtl_strcmp(usbObj->status, MDMVS_UP) == 0)
      {
         p += sprintf(p, "<font color='green'>PASS</font>");
      }
      else
      {
         p += sprintf(p, "<font color='red'>FAIL</font>");
      }

      cmsObj_free((void **) &usbObj);
   }

   *p = 0;
   return;
}
#endif /* DMP_USBLAN_1 */

#ifdef BRCM_WLAN
/* 
 * This routine print Wireless diagnostic status.
 */
void cgiPrintWirelessDiag_igd(char *varValue)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanWlanObject *wirelessObj=NULL;
   char *p = varValue;

   cmsLog_debug("Enter");

   while (cmsObj_getNext(MDMOID_LAN_WLAN, &iidStack, (void **)&wirelessObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("cmsObj_getNext returns wirelessObj->X_BROADCOM_COM_IfName %s, status %s",        
                   wirelessObj->X_BROADCOM_COM_IfName,wirelessObj->status);

      if (cmsUtl_strcmp(wirelessObj->status, MDMVS_UP) == 0)
      {
         p += sprintf(p, "<font color='green'>PASS</font>");
      }
      else
      {
         p += sprintf(p, "<font color='red'>FAIL</font>");
      }

      cmsObj_free((void **) &wirelessObj);
   }

   *p = 0;
   return;
}
#endif 


UBOOL8 isXdslConnUp(WanLinkType *pLinkType)
{
   UBOOL8 linkup = FALSE;
   UBOOL8 found = FALSE;
   WanDevObject      *wanDev  = NULL;
   InstanceIdStack   wanDevIid;
   CmsRet ret;

   cmsLog_debug("Enter");
   /* two WAN device now, loop through it and return */
   // get the first availabe WAN connection ID
   INIT_INSTANCE_ID_STACK(&wanDevIid);
   while (!found && cmsObj_getNext(MDMOID_WAN_DEV, &wanDevIid, (void **)&wanDev) == CMSRET_SUCCESS)
   {
      WanCommonIntfCfgObject  *comIntf = NULL;

      
      cmsObj_free((void **)&wanDev);  /* no longer needed */

      /* Find out the types of the WANDevice object */
      if ((cmsObj_get(MDMOID_WAN_COMMON_INTF_CFG, &wanDevIid, 0, (void **)&comIntf) != CMSRET_SUCCESS))
      {
         cmsLog_error("cmsObj_getNext <MDMOID_WAN_COMMON_INTF_CFG> returns error.");
         return (linkup);
      }

      if( !cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_DSL) )
      {
         WanDslIntfCfgObject *dslIntfCfg = NULL;
         
         if ((ret = cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &wanDevIid, 0, (void **)&dslIntfCfg)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_getNextInSubTree  <MDMOID_WAN_DSL_INTF_CFG> error. ret=%d", ret);
            cmsObj_free((void **)&comIntf);
            return (linkup);
         }
         if (!cmsUtl_strcmp(dslIntfCfg->status, MDMVS_UP))
         {
            if (cmsUtl_strcmp(dslIntfCfg->linkEncapsulationUsed, MDMVS_G_992_3_ANNEX_K_ATM) == 0)
            {
               *pLinkType = ATM;
            }
            else
            {
               *pLinkType = PTM;
            }
            linkup = TRUE;
            found = TRUE;
         }
         cmsObj_free((void **)&comIntf);
         cmsObj_free((void **)&dslIntfCfg);
      } /* dsl type */
   } /*wan Dev */
   cmsLog_debug("returns linkup %d", linkup);

   return (linkup);
}

/* 
 * This routine print xDSL link diagnostic status.
 */
void cgiPrintAdslConnDiag_igd(char *varValue)
{
   char *p = varValue;
   WanLinkType linkType;

   cmsLog_debug("Enter");

   if (isXdslConnUp(&linkType) == TRUE)
   {
      p += sprintf(p, "<font color='green'>PASS</font>");
   }
   else
   {
      p += sprintf(p, "<font color='red'>FAIL</font>");
   }
   *p = 0;
   return;
}

#ifdef DMP_ADSLWAN_1
/* This routines calls driver to performs OAM loopback test, and print the result string to *varValue.
 * diag specifies the type of test: f4/f5 end-to-end or segment OAM test.
 * diagId contains port/vpi/vci/conId info of PVC to do looback test on.
 */
void cgiPrintOAMLoopbackDiag_igd(char *varValue __attribute((unused)),
                                 int type __attribute((unused)),
                                 PDIAG_ID pDiagId __attribute((unused)))
{
   char *p = varValue;
   PORT_VPI_VCI_TUPLE vpiVciTuple;
   WanDslLinkCfgObject  *dslLink = NULL;
   UBOOL8 found = FALSE;
   InstanceIdStack   wanDevIid;
   InstanceIdStack iidStack;
   WanDevObject      *wanDev  = NULL;
   WanPppConnObject  *pppCon = NULL;   
   WanIpConnObject   *ipCon = NULL;
   InstanceIdStack         conDevIid;
   WanLinkType linkType;

   cmsLog_debug("Enter: type %d, pDiagId->protocol/name %s/%s",type,pDiagId->protocolStr,pDiagId->layer3Name);


   /* before doing the test, first check to see if ADSL link is up; if not up, don't even do
    *    these tests.   For this OAM test, it only applies when DSL link is ADSL.
    */
   if ((isXdslConnUp(&linkType) == TRUE) && (linkType == ATM))
   {
      cmsLog_debug("XdslConnUp \n");

      /* figure out port/vpi/vci info first from pDiagId */
      INIT_INSTANCE_ID_STACK(&wanDevIid);
      while (!found && ((cmsObj_getNext(MDMOID_WAN_DEV, &wanDevIid, (void **)&wanDev) == CMSRET_SUCCESS)))
      {
         cmsObj_free((void **)&wanDev);  /* no longer needed */
         
         INIT_INSTANCE_ID_STACK(&conDevIid);
         while (!found && 
                (cmsObj_getNextInSubTree(MDMOID_WAN_DSL_LINK_CFG, &wanDevIid, &conDevIid, (void **)&dslLink) == CMSRET_SUCCESS))
         {
            /* get the related ipCon obj */
            INIT_INSTANCE_ID_STACK(&iidStack);
            while (!found && 
                   (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &conDevIid, &iidStack, (void **)&ipCon) == CMSRET_SUCCESS))
            {
               if ((cmsUtl_strcmp(ipCon->X_BROADCOM_COM_IfName, pDiagId->layer3Name)) == 0)
               {
                  found = TRUE;
                  vpiVciTuple.conId = ipCon->X_BROADCOM_COM_ConnectionId;
                  cmsObj_free((void **)&ipCon);
                  break;
               }
               cmsObj_free((void **)&ipCon);
            } /* ipCon */

            INIT_INSTANCE_ID_STACK(&iidStack);
            while (!found && cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &conDevIid, &iidStack, (void **)&pppCon) == CMSRET_SUCCESS)
            {
               if ((cmsUtl_strcmp(pppCon->X_BROADCOM_COM_IfName, pDiagId->layer3Name)) == 0)
               {
                  found = TRUE;
                  vpiVciTuple.conId = pppCon->X_BROADCOM_COM_ConnectionId;
                  cmsObj_free((void **)&pppCon);
                  break;
               }
               cmsObj_free((void **)&pppCon);
            } /* while ppp */
            if (found)
            {
               break;
            }
            cmsObj_free((void **)&dslLink);
         } /* dslLink loop */
      } /* while WAN device */

      if (found)
      {
         cmsUtl_atmVpiVciStrToNum(dslLink->destinationAddress, &vpiVciTuple.vpi, &vpiVciTuple.vci);
         vpiVciTuple.port = dslLink->X_BROADCOM_COM_ATMInterfaceId;
         cmsObj_free((void **)&dslLink);
         if (dalDiag_doOamLoopback(type,&vpiVciTuple,0,0) != CMSRET_SUCCESS)
         {
            if ((type == BCM_DIAG_OAM_F4_SEGMENT) || (type == BCM_DIAG_OAM_F4_END2END))
            {
               p += sprintf(p, "<font color='red'>F4 FAIL</font>");
            }
            else
            {
               p += sprintf(p, "<font color='red'>FAIL</font>");
            }
         }
         else
         {
            p += sprintf(p, "<font color='green'>PASS</font>");
            cmsLog_debug("(port/vpi/vci: %d/%d/%d): returns CMSRET_SUCCESS",
                         vpiVciTuple.port,vpiVciTuple.vpi,vpiVciTuple.vci);
         }
      } /* found */
      else
      {
         p += sprintf(p, "<font color='#ff9900'>SKIP</font>");
      }
   } /* adsl is UP */
   else
   {
      cmsLog_debug("ADSL link is down or PTM mode, tests not done");
      p += sprintf(p, "<font color='#ff9900'>DISABLED</font>");
   }
   *p = 0;

   return;
}
#endif /* #ifdef DMP_ADSLWAN_1 */

void cgiPrintPppDiag_igd(char *varValue, int type, PDIAG_ID pDiagId)
{
   char *p = varValue;
   int ret;
   WanPppConnObject *pppCon=NULL;
   WanDevObject      *wanDev  = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   UBOOL8 passed=FALSE;
   UBOOL8 linkup=FALSE; 
   InstanceIdStack   wanDevIid;

   cmsLog_debug("*************Enter: type %d, pDiagId->protocolStr/name %s/%s",
                type,pDiagId->protocolStr,pDiagId->layer3Name);

   /* before doing the test, first check to see if WAN link is up; if not up, don't even do
    *    these tests. 
    */

   INIT_INSTANCE_ID_STACK(&wanDevIid);
   while (!found && (cmsObj_getNext(MDMOID_WAN_DEV, &wanDevIid, (void **)&wanDev) == CMSRET_SUCCESS))
   {
      WanCommonIntfCfgObject  *comIntf = NULL;
      cmsObj_free((void **)&wanDev);  /* no longer needed */
      
      /* Find out the types of the WANDevice object */
      if (cmsObj_get(MDMOID_WAN_COMMON_INTF_CFG, &wanDevIid, 0, (void **)&comIntf) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_getNext <MDMOID_WAN_COMMON_INTF_CFG> returns error.");
         return;
      }
      
      if( !cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_DSL) )
      {
         WanDslIntfCfgObject *dslIntfCfg = NULL;
         
         if ((ret = cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &wanDevIid, 0, (void **)&dslIntfCfg)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_getNextInSubTree  <MDMOID_WAN_DSL_INTF_CFG> error. ret=%d", ret);
            cmsObj_free((void **)&comIntf);
            return;
         }
         cmsLog_debug("dslIntfCfg->linkEncapsulationUsed=%s", dslIntfCfg->linkEncapsulationUsed);
         if (cmsUtl_strcmp(dslIntfCfg->status, MDMVS_UP) == 0)
         {
            linkup = TRUE;
         }         
         cmsObj_free((void **)&dslIntfCfg);
      }
      else if( !cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_ETHERNET) )
      {
         WanEthIntfObject *ethIntf = NULL;
         
         if (cmsObj_get(MDMOID_WAN_ETH_INTF, &wanDevIid, 0, (void **)&ethIntf) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_getNext <MDMOID_WAN_ETH_INTF> returns error.");
            cmsObj_free((void **)&comIntf);
            return;
         }
         if (cmsUtl_strcmp(ethIntf->status, MDMVS_UP) == 0 )
         {
            linkup = TRUE;
         }
         cmsObj_free((void **)&ethIntf);
      }
      cmsObj_free((void **)&comIntf);
      
      if (linkup)
      {
         /* get the related pppCon obj */
         INIT_INSTANCE_ID_STACK(&iidStack);
         while (!found && 
                (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &wanDevIid, &iidStack, (void **)&pppCon) == CMSRET_SUCCESS))
         {
            if (cmsUtl_strcmp(pppCon->X_BROADCOM_COM_IfName, pDiagId->layer3Name) == 0)
            {
               found = TRUE;

               /* all tests passed, there is no need to check anything */
               if (cmsUtl_strcmp(pppCon->connectionStatus, MDMVS_CONNECTED) == 0)
               {
                  passed = TRUE;
               }
               else
               {
                  switch (type)
                  {
                  case BCM_DIAG_PPPOE_SESSION:
                     /* after session is established, authentication is checked. 
                      * MDMVS_CONNECTING needs to be devided into 2 types of connecting: PADS AND CONFIRMED,
                      * CONNECTING_PADS means PADO has been received, but session has not been confirmed.
                      * CONNECTING_CONFIRMED means PADS has been received. 
                      * TO DO - yen, change MDMVS_CONNECTING to MDMVS_CONNECTING_CONFIRMED.
                      */
                     if ((cmsUtl_strcmp(pppCon->connectionStatus, MDMVS_CONNECTING) == 0) ||
                         (cmsUtl_strcmp(pppCon->connectionStatus, MDMVS_DISCONNECTED) == 0 &&
                          cmsUtl_strcmp(pppCon->lastConnectionError, MDMVS_ERROR_AUTHENTICATION_FAILURE) == 0))
                     {
                        passed = TRUE;
                     }
                     break;
                  case BCM_DIAG_PPPOE_AUTH:
                     /* if not connected-- it pretty much means this test has failed. */
                     break;
                  case BCM_DIAG_PPPOE_IP_ADDR:
                     if ((pppCon->externalIPAddress != NULL) && (pppCon->externalIPAddress[0] != '\0'))
                     {
                        passed = TRUE;
                     }
                     break;
                  } /* case */
               } /* connection is not connected yet */
               cmsObj_free((void **) &pppCon);
               break;
            } /* found ppp connection */
            cmsObj_free((void **) &pppCon);
         } /* while ppp conn */

         if (passed)
         {
            p += sprintf(p, "<font color='green'>PASS</font>");
         }
         else
         {
            p += sprintf(p, "<font color='red'>FAIL</font>");
         }
      }/* link up **/
   } /* wan dev */
   if (!linkup)
   {
      cmsLog_debug("WAN link is down, tests not done");
      p += sprintf(p, "<font color='#ff9900'>DISABLED</font>");
   }
   *p = 0;
   return;
}

#ifdef DMP_IPPING_1
void cgiPrintDefaultGatewayDiag_igd(char *varValue)
{
   char *p = varValue;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet result = CMSRET_INTERNAL_ERROR;
   CmsRet ret = CMSRET_INTERNAL_ERROR;
   char gwIfc[BUFLEN_32]={0};
   char gwIp[BUFLEN_32]={0};
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslLinkCfgObject *dslLinkCfg = NULL;
   WanIpConnObject *ipConn=NULL;
   WanPppConnObject *pppConn=NULL;
   UBOOL8 found = FALSE;
   IPPingDiagObject pingParms, pingResult;

   cmsLog_debug("Enter");

   if ((ret = dalRt_getActiveDefaultGateway(gwIfc)) == CMSRET_SUCCESS)
   {
      if ((strcmp(gwIfc,"&nbsp") != 0) && (gwIfc[0] != '\0'))
      {
         // get the first availabe WAN connection ID
         if (cmsObj_getNext(MDMOID_WAN_DSL_LINK_CFG, &parentIidStack, (void **)&dslLinkCfg) == CMSRET_SUCCESS)
         {
            INIT_INSTANCE_ID_STACK(&iidStack);
            /* get the related ipConn obj */
            while (!found && cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &parentIidStack, &iidStack,
                                                     (void **)&ipConn) == CMSRET_SUCCESS)
            {
               cmsLog_debug("ipConn->X_BROADCOM_COM_IfName %s, gwIfc %s",ipConn->X_BROADCOM_COM_IfName,gwIfc);
               
               if ((cmsUtl_strcmp(ipConn->X_BROADCOM_COM_IfName,gwIfc)) == 0)
               {
                  found = TRUE;
                  if (ipConn->defaultGateway)
                  {
                     strcpy(gwIp,ipConn->defaultGateway);
                  }
               }
               cmsObj_free((void **) &ipConn);
            } /* while ipconn */

            INIT_INSTANCE_ID_STACK(&iidStack);
            while (!found && cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &parentIidStack, &iidStack,
                                                     (void **)&pppConn) == CMSRET_SUCCESS)
            {
               cmsLog_debug("pppConn->X_BROADCOM_COM_IfName %s, gwIfc %s",pppConn->X_BROADCOM_COM_IfName,gwIfc);
               
               if ((cmsUtl_strcmp(pppConn->X_BROADCOM_COM_IfName,gwIfc)) == 0)
               {
                  found = TRUE;
                  if (pppConn->X_BROADCOM_COM_DefaultGateway)
                  {
                     cmsLog_debug("pppConn->X_BROADCOM_COM_DefaultGateway %s",pppConn->X_BROADCOM_COM_DefaultGateway);
                     strcpy(gwIp,pppConn->X_BROADCOM_COM_DefaultGateway);
                  }
               }
               cmsObj_free((void **) &pppConn);
            } /* while pppConn */
            cmsObj_free((void **) &dslLinkCfg);
         } /* dslLinkCfg */
      

         cmsLog_debug("gwIp %s",gwIp);

         /* set the IPPing object */
         if (gwIp[0] != '\0')
         {
            char *fullPathName;
            
            pingParms.diagnosticsState = cmsMem_strdup(MDMVS_REQUESTED);
            if (gwIfc[0] != 0)
            {
               if (qdmIntf_intfnameToFullPathLocked(gwIfc, FALSE, &fullPathName) == CMSRET_SUCCESS)
               {
                  /* strip the ending '.' */
                  fullPathName[strlen(fullPathName)-1] = '\0';
                  pingParms.interface  = fullPathName;
               }
            }

            pingParms.host = cmsMem_strdup(gwIp);
            pingParms.numberOfRepetitions = 1;
            pingParms.timeout = 1; /* 1 second */
            pingParms.dataBlockSize = 64;
            pingParms.DSCP = 0;
            
            ret = dalDiag_startStopPing((void*)&pingParms);
            if (ret == CMSRET_SUCCESS)
            {
               /* release lock for SSK to update statistics */
               cmsLck_releaseLock();
            }
            cmsMem_free(pingParms.diagnosticsState);
            cmsMem_free(pingParms.interface);
            cmsMem_free(pingParms.host);
            
            /* and then read the object after timeout period */
            if (ret == CMSRET_SUCCESS)
            {
               sleep(pingParms.timeout);
               cmsLck_acquireLock();
               
               if ((ret = dalDiag_getPingResult(&pingResult)) == CMSRET_SUCCESS)
               {
                  cmsLog_debug("got PingResult state %s",pingResult.diagnosticsState);
                  
                  if ((cmsUtl_strcmp(pingResult.diagnosticsState,MDMVS_COMPLETE) == 0) &&
                      (pingResult.successCount == pingParms.numberOfRepetitions))
                  {
                     result = CMSRET_SUCCESS;
                  } /* ping result */
                  cmsMem_free(pingResult.diagnosticsState);
               } /* get ping result */
            } /* ping OK */
         } /* gwIP */
      } /* gwifc */
   } /* active default gateway */

   if (result == CMSRET_SUCCESS)
   {
      p += sprintf(p, "<font color='green'>PASS</font>");
   }
   else
   {
      p += sprintf(p, "<font color='red'>FAIL</font>");
   }
   *p = 0;

   return;
}
#endif /* DMP_IPPING_1 */

#ifdef DMP_IPPING_1
void cgiPrintDnsDiag_igd(char *varValue)
{
   char *p = varValue;
   char dnsPrimary[CMS_IPADDR_LENGTH] = {0};
   char dnsSecondary[CMS_IPADDR_LENGTH] = {0};
   CmsRet result = CMSRET_INTERNAL_ERROR;
   IPPingDiagObject pingParms, pingResult;
   CmsRet ret = CMSRET_INTERNAL_ERROR;

   cmsLog_debug("Enter");

   qdmDns_getActiveIpvxDnsIpLocked(CMS_AF_SELECT_IPV4, dnsPrimary, dnsSecondary);

   cmsLog_debug("dnsPrimary %s",dnsPrimary);

   /* set the IPPing object */
   if ((dnsPrimary[0] != '\0') && (strcmp(dnsPrimary,"0.0.0.0") != 0))
   {
      pingParms.diagnosticsState = cmsMem_strdup(MDMVS_REQUESTED);
      pingParms.host = cmsMem_strdup(dnsPrimary);
      pingParms.numberOfRepetitions = 1;
      pingParms.timeout = 1; /* 1 second */
      pingParms.dataBlockSize = 64;
      pingParms.DSCP = 0;
      cmsLog_debug("dnsPrimary %s, ping DnsPrimary",dnsPrimary);
      if ((ret = dalDiag_startStopPing((void*)&pingParms)) == CMSRET_SUCCESS)
      {
         /* release lock for SSK to update statistics */
         cmsLck_releaseLock();
      }
      cmsMem_free(pingParms.diagnosticsState);
      cmsMem_free(pingParms.host);

      /* and then read the object after timeout period */
      if (ret == CMSRET_SUCCESS)
      {
         sleep(pingParms.timeout);
         cmsLck_acquireLock();
         
         if ((ret = dalDiag_getPingResult(&pingResult)) == CMSRET_SUCCESS)
         {
            cmsLog_debug("got PingResult state %s",pingResult.diagnosticsState);
            
            if ((cmsUtl_strcmp(pingResult.diagnosticsState,MDMVS_COMPLETE) == 0) &&
                (pingResult.successCount == pingParms.numberOfRepetitions))
            {
               result = CMSRET_SUCCESS;
            } /* ping result */
            cmsMem_free(pingResult.diagnosticsState);
         } /* get ping result */
      } /* ping start OK */
   }

   if (result == CMSRET_SUCCESS)
   {
      p += sprintf(p, "<font color='green'>PASS</font>");
   }
   else
   {
      p += sprintf(p, "<font color='red'>FAIL</font>");
   }
   *p = 0;
   return;
}
#endif /* DMP_IPPING_1 */

#ifdef DMP_X_BROADCOM_COM_ETHERNETOAM_1
void cgiEthOamSavApply(char *query, FILE *fs __attribute__((unused)))
{
   char eoam3ahEnbl[BUFLEN_8];
   char eoam3ahIntf[CMS_IFNAME_LENGTH];
   char eoam3ahOamId[BUFLEN_16];
   char eoam3ahAeEnbl[BUFLEN_8];
   char eoam3ahVrEnbl[BUFLEN_8];
   char eoam3ahLeEnbl[BUFLEN_8];
   char eoam3ahRlEnbl[BUFLEN_8];
   char eoam3ahAmEnbl[BUFLEN_8];
   char eoam1agEnbl[BUFLEN_8];
   char eoam1731Enbl[BUFLEN_8];
   char eoam1agMdId1[BUFLEN_48];
   char eoam1agMdId2[BUFLEN_48];
   char eoam1agMdLvl1[BUFLEN_32];
   char eoam1agMdLvl2[BUFLEN_32];
   char eoam1agMaId[BUFLEN_48];
   char eoam1agCcmInterval[BUFLEN_8];
   char eoam1agLocIntf[CMS_IFNAME_LENGTH];
   char eoam1agLocMepId[BUFLEN_8];
   char eoam1agLocVlanId[BUFLEN_8];
   char eoam1agLocCcmEnbl[BUFLEN_8];
   char eoam1agRemMepId[BUFLEN_8];
   InstanceIdStack ieee8023ahCfgIidStack;
   Ieee8023ahCfgObject *ieee8023ahCfg = NULL;
   InstanceIdStack ieee8021agCfgIidStack;
   Ieee8021agCfgObject *ieee8021agCfg = NULL;
   InstanceIdStack localMepIidStack;
   LocalMepObject *localMep = NULL;
   InstanceIdStack remoteMepIidStack;
   RemoteMepObject *remoteMep = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   eoam3ahEnbl[0] = eoam3ahIntf[0] = eoam3ahOamId[0] = eoam3ahAeEnbl[0] = '\0';
   eoam3ahVrEnbl[0] = eoam3ahLeEnbl[0] = eoam3ahRlEnbl[0] = eoam3ahAmEnbl[0] = '\0';
   eoam1agEnbl[0] = eoam1731Enbl[0] = eoam1agMdId1[0] = eoam1agMdId2[0] = eoam1agMdLvl1[0]= eoam1agMdLvl2[0] = '\0';
   eoam1agMaId[0] = eoam1agCcmInterval[0] = eoam1agLocIntf[0] = eoam1agLocMepId[0] = '\0';
   eoam1agLocVlanId[0] = eoam1agLocCcmEnbl[0] = eoam1agRemMepId[0] = '\0';

   cgiGetValueByName(query, "oam3ahEnbl", eoam3ahEnbl);
   cgiGetValueByName(query, "oam3ahIntf", eoam3ahIntf);
   cgiGetValueByName(query, "oam3ahOamId", eoam3ahOamId);
   cgiGetValueByName(query, "oam3ahAeEnbl", eoam3ahAeEnbl);
   cgiGetValueByName(query, "oam3ahVrEnbl", eoam3ahVrEnbl);
   cgiGetValueByName(query, "oam3ahLeEnbl", eoam3ahLeEnbl);
   cgiGetValueByName(query, "oam3ahRlEnbl", eoam3ahRlEnbl);
   cgiGetValueByName(query, "oam3ahAmEnbl", eoam3ahAmEnbl);
   cgiGetValueByName(query, "oam1agEnbl", eoam1agEnbl);
   cgiGetValueByName(query, "oam1731Enbl", eoam1731Enbl);
   cgiGetValueByName(query, "oam1agMdId1", eoam1agMdId1);
   cgiGetValueByName(query, "oam1agMdId2", eoam1agMdId2);
   cgiGetValueByName(query, "oam1agMdLvl1", eoam1agMdLvl1);
   cgiGetValueByName(query, "oam1agMdLvl2", eoam1agMdLvl2);
   cgiGetValueByName(query, "oam1agMaId", eoam1agMaId);
   cgiGetValueByName(query, "oam1agCcmInterval", eoam1agCcmInterval);
   cgiGetValueByName(query, "oam1agLocIntf", eoam1agLocIntf);
   cgiGetValueByName(query, "oam1agLocMepId", eoam1agLocMepId);
   cgiGetValueByName(query, "oam1agLocVlanId", eoam1agLocVlanId);
   cgiGetValueByName(query, "oam1agLocCcmEnbl", eoam1agLocCcmEnbl);
   cgiGetValueByName(query, "oam1agRemMepId", eoam1agRemMepId);

   /* MDMOID_IEEE8023AH_CFG */
   INIT_INSTANCE_ID_STACK(&ieee8023ahCfgIidStack);
   if ((ret = cmsObj_get(MDMOID_IEEE8023AH_CFG, &ieee8023ahCfgIidStack, 0, (void **)&ieee8023ahCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_IEEE8023AH_CFG> returns error. ret=%d", ret);
      return;
   }
   
   ieee8023ahCfg->enabled = atoi(eoam3ahEnbl)? TRUE : FALSE;
   CMSMEM_REPLACE_STRING(ieee8023ahCfg->ifName, eoam3ahIntf);
   ieee8023ahCfg->oamId = atoi(eoam3ahOamId);
   ieee8023ahCfg->autoEventEnabled = atoi(eoam3ahAeEnbl)? TRUE : FALSE;
   ieee8023ahCfg->variableRetrievalEnabled = atoi(eoam3ahVrEnbl)? TRUE : FALSE;
   ieee8023ahCfg->linkEventsEnabled = atoi(eoam3ahLeEnbl)? TRUE : FALSE;
   ieee8023ahCfg->remoteLoopbackEnabled = atoi(eoam3ahRlEnbl)? TRUE : FALSE;
   ieee8023ahCfg->activeModeEnabled = atoi(eoam3ahAmEnbl)? TRUE : FALSE;
   if ((ret = cmsObj_set((void *)ieee8023ahCfg, &ieee8023ahCfgIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_set <MDMOID_IEEE8023AH_CFG> returns error. ret=%d", ret);
      cmsObj_free((void **) &ieee8023ahCfg);
      return;
   }
   else
   {
      glbSaveConfigNeeded = TRUE;
   }
   cmsObj_free((void **) &ieee8023ahCfg);

   /* MDMOID_LOCAL_MEP */
   INIT_INSTANCE_ID_STACK(&localMepIidStack);
   if ((ret = cmsObj_getNext(MDMOID_LOCAL_MEP, &localMepIidStack, (void **)&localMep)) != CMSRET_SUCCESS)
   {
      if ((ret = cmsObj_addInstance(MDMOID_LOCAL_MEP, &localMepIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_addInstance <MDMOID_LOCAL_MEP> returns error. ret=%d", ret);
         return;
      }
      if ((ret = cmsObj_get(MDMOID_LOCAL_MEP, &localMepIidStack, 0, (void **)&localMep)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_get <MDMOID_LOCAL_MEP> returns error. ret=%d", ret);
         return;
      }
   }
   CMSMEM_REPLACE_STRING(localMep->ifName, eoam1agLocIntf);
   localMep->mepId = atoi(eoam1agLocMepId);
   if(atoi(eoam1agLocVlanId) <= 0)
   {
      localMep->vlanId = -1;
   }
   else
   {
      localMep->vlanId = atoi(eoam1agLocVlanId);
   }
   localMep->ccmEnabled = atoi(eoam1agLocCcmEnbl)? TRUE : FALSE;
   if ((ret = cmsObj_set((void *)localMep, &localMepIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_set <MDMOID_LOCAL_MEP> returns error. ret=%d", ret);
      cmsObj_free((void **) &localMep);
      return;
   }
   else
   {
      glbSaveConfigNeeded = TRUE;
   }
   cmsObj_free((void **) &localMep);

   /* MDMOID_REMOTE_MEP */
   INIT_INSTANCE_ID_STACK(&remoteMepIidStack);
   if ((ret = cmsObj_getNext(MDMOID_REMOTE_MEP, &remoteMepIidStack, (void **)&remoteMep)) != CMSRET_SUCCESS)
   {
      if ((ret = cmsObj_addInstance(MDMOID_REMOTE_MEP, &remoteMepIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_addInstance <MDMOID_REMOTE_MEP> returns error. ret=%d", ret);
         return;
      }
      if ((ret = cmsObj_get(MDMOID_REMOTE_MEP, &remoteMepIidStack, 0, (void **)&remoteMep)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_get <MDMOID_REMOTE_MEP> returns error. ret=%d", ret);
         return;
      }
   }
   if(atoi(eoam1agRemMepId) <= 0)
   {
      remoteMep->mepId = -1;
   }
   else
   {
      remoteMep->mepId = atoi(eoam1agRemMepId);
   }
   if ((ret = cmsObj_set((void *)remoteMep, &remoteMepIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_set <MDMOID_REMOTE_MEP> returns error. ret=%d", ret);
      cmsObj_free((void **) &remoteMep);
      return;
   }
   else
   {
      glbSaveConfigNeeded = TRUE;
   }
   cmsObj_free((void **) &remoteMep);

   /*
      MDMOID_IEEE8021AG_CFG
      Set 802.1ag object after local and remote MEP information is set.
   */
   INIT_INSTANCE_ID_STACK(&ieee8021agCfgIidStack);
   if ((ret = cmsObj_get(MDMOID_IEEE8021AG_CFG, &ieee8021agCfgIidStack, 0, (void **)&ieee8021agCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_IEEE8021AG_CFG> returns error. ret=%d", ret);
      return;
   }
   ieee8021agCfg->enabled = atoi(eoam1agEnbl)? TRUE : FALSE;
   ieee8021agCfg->Y1731Enabled = atoi(eoam1731Enbl)? TRUE : FALSE;
   if(ieee8021agCfg->Y1731Enabled == FALSE){
      CMSMEM_REPLACE_STRING(ieee8021agCfg->mdId, eoam1agMdId1);
      ieee8021agCfg->mdLevel = atoi(eoam1agMdLvl1);
   }else{
      CMSMEM_REPLACE_STRING(ieee8021agCfg->mdId, eoam1agMdId2);
      ieee8021agCfg->mdLevel = atoi(eoam1agMdLvl2);
   }
   CMSMEM_REPLACE_STRING(ieee8021agCfg->maId, eoam1agMaId);
   ieee8021agCfg->ccmInterval = atoi(eoam1agCcmInterval);

   if ((ret = cmsObj_set((void *)ieee8021agCfg, &ieee8021agCfgIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_set <MDMOID_IEEE8021AG_CFG> returns error. ret=%d", ret);
      cmsObj_free((void **) &ieee8021agCfg);
      return;
   }
   else
   {
      glbSaveConfigNeeded = TRUE;
   }
   cmsObj_free((void **) &ieee8021agCfg);

}  /* End of cgiEthOamSavApply() */

void cgiEthOam1agLbm(char *query, FILE *fs __attribute__((unused)))
{
   char eoam1agLocMepId[BUFLEN_8];
   char eoam1agTargetMac[MAC_STR_LEN+1];
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack eoam1agLoopbackIidStack;
   Eoam1agLoopbackDiagObject *eoam1agLoopback = NULL;

   eoam1agLocMepId[0] = eoam1agTargetMac[0] = '\0';
   cgiGetValueByName(query, "oam1agLocMepId", eoam1agLocMepId);
   cgiGetValueByName(query, "oam1agTargetMac", eoam1agTargetMac);

   INIT_INSTANCE_ID_STACK(&eoam1agLoopbackIidStack);
   if ((ret = cmsObj_get(MDMOID_EOAM1AG_LOOPBACK_DIAG, &eoam1agLoopbackIidStack, 0, (void **)&eoam1agLoopback)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_EOAM1AG_LOOPBACK_DIAG> returns error. ret=%d", ret);
      return;
   }
   CMSMEM_REPLACE_STRING(eoam1agLoopback->diagnosticsState, MDMVS_REQUESTED);
   CMSMEM_REPLACE_STRING(eoam1agLoopback->targetMacAddress, eoam1agTargetMac);
   eoam1agLoopback->localMepId = atoi(eoam1agLocMepId);
   eoam1agLoopback->numberOfRepetitions = 1;

   if ((ret = cmsObj_set((void *)eoam1agLoopback, &eoam1agLoopbackIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_set <MDMOID_EOAM1AG_LOOPBACK_DIAG> returns error. ret=%d", ret);
   }
   cmsObj_free((void **) &eoam1agLoopback);
   
}  /* End of cgiEthOam1agLbm() */

void cgiEthOam1agLtm(char *query, FILE *fs __attribute__((unused)))
{
   char eoam1agLocMepId[BUFLEN_8];
   char eoam1agTargetMac[MAC_STR_LEN+1];
   char eoam1agLtmTtl[BUFLEN_8];
  
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack eoam1agLinktraceIidStack;
   Eoam1agLinktraceDiagObject* eoam1agLinktrace = NULL;

   eoam1agLocMepId[0] = eoam1agTargetMac[0] = eoam1agLtmTtl[0] = '\0';

   cgiGetValueByName(query, "oam1agLocMepId", eoam1agLocMepId);
   cgiGetValueByName(query, "oam1agTargetMac", eoam1agTargetMac);
   cgiGetValueByName(query, "oam1agLtmTtl", eoam1agLtmTtl);

   INIT_INSTANCE_ID_STACK(&eoam1agLinktraceIidStack);
   if ((ret = cmsObj_get(MDMOID_EOAM1AG_LINKTRACE_DIAG, &eoam1agLinktraceIidStack, 0, (void **)&eoam1agLinktrace)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_EOAM1AG_LINKTRACE_DIAG> returns error. ret=%d", ret);
      return;
   }
   CMSMEM_REPLACE_STRING(eoam1agLinktrace->diagnosticsState, MDMVS_REQUESTED);
   CMSMEM_REPLACE_STRING(eoam1agLinktrace->targetMacAddress, eoam1agTargetMac);
   eoam1agLinktrace->localMepId = atoi(eoam1agLocMepId);
   if(atoi(eoam1agLtmTtl) <= 0)
   {
      eoam1agLinktrace->maxHopCount = -1;
   }
   else
   {
      eoam1agLinktrace->maxHopCount = atoi(eoam1agLtmTtl);
   }

   if ((ret = cmsObj_set((void *)eoam1agLinktrace, &eoam1agLinktraceIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_set <MDMOID_EOAM1AG_LINKTRACE_DIAG> returns error. ret=%d", ret);
   }
   cmsObj_free((void **) &eoam1agLinktrace);
  
}  /* End of cgiEthOam1agLtm() */

void cgiEthOam(char *query, FILE *fs)
{
   char action[BUFLEN_264];
   char path[BUFLEN_1024];
   cgiGetValueByName(query, "action", action);
   if (strcmp(action, "savapply") == 0)
   { 
      cgiEthOamSavApply(query, fs);
   }
   else if (strcmp(action, "oam1agLbm") == 0)
   { 
      cgiEthOam1agLbm(query, fs);
   }
   else if (strcmp(action, "oam1agLtm") == 0)
   { 
      cgiEthOam1agLtm(query, fs);
   }

   makePathToWebPage(path, sizeof(path), "diagethoam.html");
   do_ej(path, fs);
   
}  /* End of cgiEthOam() */


void cgiGetEoamIntfList(int argc __attribute__((unused)),
                        char **argv __attribute__((unused)), char *varValue)
{
   NameList *nl, *ifList = NULL;
   varValue[0] = '\0';

   if (cmsDal_getAvailableIfForWanService(&ifList, FALSE) != CMSRET_SUCCESS)
   {
      cmsLog_error("Can not get available interfaces.");
      return;
   }

   if (ifList == NULL)
   {
      cmsLog_error("No available interfaces.");
      return;
   }
 
   nl = ifList;
   while (nl != NULL)
   {
      char *pp;
      pp = strchr(nl->name, '/');
      if (pp)
      {
         *pp = '\0';
      }
      pp = strchr(nl->name, '(');
      if (pp)
      {
         *pp = '\0';
      }
      strcat(varValue, nl->name);
      strcat(varValue, "|");
      nl = nl->next;
   }
   cmsDal_freeNameList(ifList);
}


void cgiGetEoam1agLbmResult(int argc __attribute__((unused)),
                        char **argv __attribute__((unused)), char *varValue)
{
   CmsRet ret = CMSRET_SUCCESS;
   varValue[0] = '\0';

   InstanceIdStack eoam1agLoopbackIidStack;
   Eoam1agLoopbackDiagObject *eoam1agLoopback = NULL;

   strcpy(varValue, "N/A");

   INIT_INSTANCE_ID_STACK(&eoam1agLoopbackIidStack);
   if ((ret = cmsObj_get(MDMOID_EOAM1AG_LOOPBACK_DIAG, &eoam1agLoopbackIidStack, 0, (void **)&eoam1agLoopback)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_EOAM1AG_LOOPBACK_DIAG> returns error. ret=%d", ret);
      return;
   }

   if(!strcmp(eoam1agLoopback->diagnosticsState, MDMVS_ERROR))
   {
      strcpy(varValue, "<B><font color='red'>ERROR</font></B>");
   }
   else if(!strcmp(eoam1agLoopback->diagnosticsState, MDMVS_COMPLETE))
   {
      if(eoam1agLoopback->successCount)
      {
         strcpy(varValue, "<B><font color='green'>PASS</font></B>");
      }
      else
      {
         strcpy(varValue, "<B><font color='red'>FAIL</font></B>");
      }
   }
   cmsObj_free((void **) &eoam1agLoopback);

}  /* End of cgiGetEoam1agLbmResult() */


void cgiGetEoam1agLtmResult(int argc __attribute__((unused)),
                            char **argv, char *varValue)
{

   CmsRet ret = CMSRET_SUCCESS;
   int getMacIndex = 0;
   int ltmReplyIndex = 0;
   char *delim = ",";
   char *mep = NULL;
   char mepList[BUFLEN_1024];
   
   varValue[0] = '\0';
   
   getMacIndex = atoi(argv[2]);

   InstanceIdStack eoam1agLinktraceIidStack;
   Eoam1agLinktraceDiagObject *eoam1agLinktrace = NULL;

   if(getMacIndex <= 1)
   {
      strcpy(varValue, "N/A");
   }else
   {
      strcpy(varValue, "");
   }

   INIT_INSTANCE_ID_STACK(&eoam1agLinktraceIidStack);
   if ((ret = cmsObj_get(MDMOID_EOAM1AG_LINKTRACE_DIAG, &eoam1agLinktraceIidStack, 0, (void **)&eoam1agLinktrace)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_EOAM1AG_LINKTRACE_DIAG> returns error. ret=%d", ret);
      return;
   }

   if(!strcmp(eoam1agLinktrace->diagnosticsState, MDMVS_ERROR))
   {
      if(getMacIndex == 1)
      {
         strcpy(varValue, "<B><font color='red'>ERROR</font></B>");
      }
   }
   else if(!strcmp(eoam1agLinktrace->diagnosticsState, MDMVS_COMPLETE))
   {
      strncpy(mepList, eoam1agLinktrace->mepList, BUFLEN_1024);
      mep = strtok(mepList, delim);
      for (ltmReplyIndex = 0; ltmReplyIndex < getMacIndex; ltmReplyIndex++)
      {
         if(mep) 
         {
            strcpy(varValue, mep);
         }
         else
         {
            strcpy(varValue, "");
         }
         mep = strtok(NULL, delim);
      }
   }
   cmsObj_free((void **) &eoam1agLinktrace);

} /* End of cgiGetEoam1agLtmResult() */

#endif
