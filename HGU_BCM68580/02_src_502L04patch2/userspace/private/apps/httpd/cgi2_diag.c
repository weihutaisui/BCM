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

#ifdef DMP_DEVICE2_BASELINE_1

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

#if defined(BRCM_WLAN) && !defined(SUPPORT_UNIFIED_WLMNGR)
#include "wlapi.h"
#endif

#include "cms_dal.h"
#include "cms_msg.h"

#ifdef DMP_X_BROADCOM_COM_ETHERNETOAM_1
#include "prctl.h"
#endif

extern CmsRet dalRt_getDefaultGatewayIP(char *varValue);

void cgiGetDiagId_dev2(int type, PDIAG_ID pDiagId)
{
   char info[BUFLEN_64];
   char *cp1 = NULL;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

   info[0] = '\0';

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
      /* get the first availabe WAN connection ID */
      while ((ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                                   (void **)&ipIntfObj)) == CMSRET_SUCCESS)
      {
         if ((ipIntfObj->X_BROADCOM_COM_Upstream == TRUE) &&
             (ipIntfObj->enable == TRUE) &&
             ((cmsUtl_strcmp(ipIntfObj->status,MDMVS_UP) == 0) ||
              (cmsUtl_strcmp(ipIntfObj->status,MDMVS_DOWN) == 0)))
         {
            /* this is a WAN, but the services are not necessarily UP, but diag does not care */
            /* one would need to traverse all the way down to physical layer to know if this is an IPoE or IPoA.
             * I will assume we are not over A now.  All PPP have ifname prefix of PPP.
             */
            strcpy(pDiagId->layer3Name, ipIntfObj->name);
            if (cmsUtl_strncmp(ipIntfObj->name,PPP_IFC_STR,strlen(PPP_IFC_STR)) == 0)
            {
               strcpy(pDiagId->protocolStr, PPPOE_PROTO_STR);
            }
            else
            {
               /* Is this a routed connection or bridge?  If it has the IPv4AddressNumberofEntries, then it is routed.
                * Shouldn't it be the "router" parameter that we should check?
                */
               if ((ipIntfObj->IPv4AddressNumberOfEntries != 0)
#ifdef DMP_DEVICE2_IPV6INTERFACE_1
                   || (ipIntfObj->IPv6AddressNumberOfEntries != 0)
#endif
                   )
               {
                  strcpy(pDiagId->protocolStr, IPOE_PROTO_STR);               
               }
               else
               {
                  strcpy(pDiagId->protocolStr,  BRIDGE_PROTO_STR);
               }
            } /* a non-ppp connection */
            cmsObj_free((void **) &ipIntfObj);
            return;
         }   /* a WAN interface */
         cmsObj_free((void **) &ipIntfObj);
      } /* while IP_INTERFACE */
   } /* info not found */
} /* cgiGetDiagId_dev2 */

/*
 * This routine is to return a list of wan connection with protocolStrType/layer3Ifname seperated by |:
 * protocol1/layer3Ifname1|protocol2/layer3Ifname2|protocol3/layer3Ifname3|
 *
 */
void cgiDmGetDiagInfo_dev2(char **argv, char *varValue)
{
   DIAG_ID diagId;
   char info[BUFLEN_64];
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char protocolStr[BUFLEN_16]={0};
   info[0] = '\0';
   int len;

   cmsLog_debug("Enter, argv[2] %s",argv[2]);

   if ( strcmp(argv[2], "diag") == 0 ) 
   {   
      while ((ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                                   (void **)&ipIntfObj)) == CMSRET_SUCCESS)
      {
         if (ipIntfObj->X_BROADCOM_COM_Upstream == TRUE)
         {
            /* this is a WAN, but the services are not necessarily UP, but diag does not care */
            /* one would need to traverse all the way down to physical layer to know if this is an IPoE or IPoA.
             * I will assume we are not over A now.  All PPP have ifname prefix of PPP.
             */
            if (cmsUtl_strncmp(ipIntfObj->name,PPP_IFC_STR,strlen(PPP_IFC_STR)) == 0)
            {
               strcpy(protocolStr, PPPOE_PROTO_STR);
            }
            else
            {
               /* Is this a routed connection or bridge?  If it has the IPv4AddressNumberofEntries, then it is routed.
                * Shouldn't it be the "router" parameter that we should check?
                */
               if ((ipIntfObj->IPv4AddressNumberOfEntries != 0)
#ifdef DMP_DEVICE2_IPV6INTERFACE_1
                   || (ipIntfObj->IPv6AddressNumberOfEntries != 0)
#endif
                   )
               {
                  strcpy(protocolStr, IPOE_PROTO_STR);               
               }
               else
               {
                  strcpy(protocolStr,  BRIDGE_PROTO_STR);
               }
            } /* a non-ppp connection */
            sprintf(info, "%s/%s|", protocolStr, ipIntfObj->name);
            strcat(varValue, info);               
            cmsLog_debug("info %s",info);
         }   /* a WAN interface */
         cmsObj_free((void **) &ipIntfObj);
      } /* while IP_INTERFACE */
      len = strlen(varValue);
      if ( len > 0 )
      {
         varValue[len-1] = '\0';   // remove the last '|' character
      }
   } /* diag is the 2nd arguement */ 
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

      while ((ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                                   (void **)&ipIntfObj)) == CMSRET_SUCCESS)
      {
         if (ipIntfObj->X_BROADCOM_COM_Upstream == TRUE &&
             cmsUtl_strcmp(ipIntfObj->status, MDMVS_UP) == 0)
         {
            if (cmsUtl_strcmp(ipIntfObj->name,diagId.layer3Name) == 0)
            {
               if ( strcmp(argv[2], "serviceName") == 0 )
               {
                  strcpy(varValue, ipIntfObj->name);
               }
               else if ( strcmp(argv[2], "deviceName") == 0 )
               {
                  strcpy(varValue, ipIntfObj->name);
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
               cmsObj_free((void **)&ipIntfObj);
               break;
            } /* name found */
            cmsObj_free((void **)&ipIntfObj);
         } /* wan interface */
      } /* ip interface */
   } /* else get by field name*/

   cmsLog_debug("varValue =%s", varValue);
}

#ifdef MDMOID_DEV2_ETHERNET_INTERFACE
void cgiPrintEnetDiag_dev2(char *varValue)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2EthernetInterfaceObject *ethIntfObj=NULL;
   char *p = varValue;

   cmsLog_debug("Enter");

   p += sprintf(p, "var brdId = '%s';\n", glbWebVar.boardID);
   p += sprintf(p, "var intfDisp = '';\n");
   p += sprintf(p, "var brdIntf = '';\n");

   while (cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {

      cmsLog_debug("cmsObj_getNext returns ethIntfObj->name %s, upstream=%d status %s",
                   ethIntfObj->name, ethIntfObj->upstream, ethIntfObj->status);

      /* only print out LAN eth interfaces */
      if (ethIntfObj->upstream == FALSE)
      {
         p += sprintf(p, "brdIntf = brdId + '|' + '%s';\n", ethIntfObj->name);
         p += sprintf(p, "intfDisp = getUNameByLName(brdIntf);\n");
         p += sprintf(p, "document.writeln(\"<tr><td class='hd'>Test your \" + intfDisp +  \" Connection: </td>\");\n");

         if (cmsUtl_strcmp(ethIntfObj->status, MDMVS_UP) == 0)
         {
            p += sprintf(p, "document.writeln(\"<td><b> <font color='green'>PASS</font> </b></td>\");\n");
         }
         else
         {
            p += sprintf(p, "document.writeln(\"<td><b> <font color='red'>FAIL</font> </b></td>\");\n");
         }

         p += sprintf(p, "document.writeln(\"<td><A href='hlpethconn.html'>Help</A></td> </tr>\");\n");
      }

      cmsObj_free((void **) &ethIntfObj);
   }

   *p = 0;
   return;
}
#endif /* MDMOID_DEV2_ETHERNET_INTERFACE */

#ifdef MDMOID_DEV2_USB_INTERFACE
/* 
 * This routine print USB diagnostic status.
 */
void cgiPrintUsbDiag_dev2(char *varValue)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2UsbInterfaceObject *usbObj=NULL;
   char *p = varValue;

   cmsLog_debug("Enter");

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_DEV2_USB_INTERFACE, &iidStack, (void **) &usbObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("cmsObj_getNext returns usbObj->name %s, status %s",        
                   usbObj->name,usbObj->status);

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
#endif /* MDMOID_DEV2_ETHERNET_INTERFACE */

#ifdef MDMOID_DEV2_WIFI_RADIO
/* 
 * This routine print Wireless diagnostic status.
 */
void cgiPrintWirelessDiag_dev2(char *varValue)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2WifiRadioObject *wirelessObj;
   char *p = varValue;

   cmsLog_debug("Enter");

   while (cmsObj_getNext(MDMOID_DEV2_WIFI_RADIO, &iidStack, (void **)&wirelessObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("cmsObj_getNext returns wirelessObj->name %s, status %s",        
                   wirelessObj->name,wirelessObj->status);

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
#endif /* MDMOID_DEV2_WIFI_RADIO */

#ifdef DMP_DEVICE2_DSL_1
/* 
 * This routine print xDSL link diagnostic status.
 */
void cgiPrintAdslConnDiag_dev2(char *varValue)
{
   char *p = varValue;

   cmsLog_debug("Enter");

   if (qdmDsl_isXdslLinkUpLocked_dev2() == TRUE)
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
#endif

#ifdef DMP_DEVICE2_ATMLOOPBACK_1
/* This routines calls driver to performs OAM loopback test, and print the result string to *varValue.
 * diag specifies the type of test: f4/f5 end-to-end or segment OAM test.
 * diagId contains port/vpi/vci/conId info of PVC to do looback test on.
 */
void cgiPrintOAMLoopbackDiag_dev2(char *varValue, int type, PDIAG_ID pDiagId)
{
   char *p = varValue;
   PORT_VPI_VCI_TUPLE vpiVciTuple;
   UBOOL8 isVdsl, isAtm;
   UBOOL8 found = FALSE;
   UBOOL8 atmLinkFound = FALSE;
   InstanceIdStack iidStack;
   InstanceIdStack atmIidStack;
   Dev2AtmLinkObject *atmLinkObj = NULL;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   CmsRet ret;

   cmsLog_debug("Enter: type %d, pDiagId->protocol/name %s/%s",type,pDiagId->protocolStr,pDiagId->layer3Name);


   /* before doing the test, first check to see if ADSL link is up; if not up, don't even do
    *    these tests.   For this OAM test, it only applies when DSL link is ADSL.
    */
   qdmDsl_getDSLTrainedModeLocked_dev2(&isVdsl,&isAtm);
   if ((qdmDsl_isXdslLinkUpLocked_dev2() == TRUE) && (isAtm == TRUE))
   {
      cmsLog_debug("Xdsl Up, and ATM mode \n");
      INIT_INSTANCE_ID_STACK(&iidStack);
      /* figure out port/vpi/vci info first from pDiagId */
      while ((!found) &&(ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                                              (void **)&ipIntfObj)) == CMSRET_SUCCESS)
      {
         char currL2IntfNameBuf[CMS_IFNAME_LENGTH]={0};
         /* Get the L2Intf */
         ret = qdmIpIntf_getLayer2IntfNameByLayer3IntfNameLocked_dev2(ipIntfObj->name, currL2IntfNameBuf);
         if ( (ret == CMSRET_SUCCESS) && (ipIntfObj->X_BROADCOM_COM_Upstream == TRUE) &&
             (cmsUtl_strcmp(ipIntfObj->name, pDiagId->layer3Name) == 0))
         {
            found = TRUE;
            INIT_INSTANCE_ID_STACK(&atmIidStack);
            while ((!atmLinkFound) &&(ret = cmsObj_getNext(MDMOID_DEV2_ATM_LINK, &atmIidStack,
                                                    (void **)&atmLinkObj)) == CMSRET_SUCCESS)
            {
               if ((cmsUtl_strcmp(currL2IntfNameBuf, atmLinkObj->name) == 0))
               {
                  atmLinkFound = 1;
                  cmsUtl_atmVpiVciStrToNum_dev2(atmLinkObj->destinationAddress, &vpiVciTuple.vpi, &vpiVciTuple.vci);
                  vpiVciTuple.port = atmLinkObj->X_BROADCOM_COM_ATMInterfaceId;                  
               }
               cmsObj_free((void **)&atmLinkObj);
            }
         } 
         cmsObj_free((void **)&ipIntfObj);
      }
      if (found && atmLinkFound)
      {
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
#endif /* DMP_DEVICE2_ATMLOOPBACK_1  */

void cgiPrintPppDiag_dev2(char *varValue __attribute((unused)), int type __attribute((unused)), 
                          PDIAG_ID pDiagId __attribute((unused)))
{
   char *p = varValue;
   int ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2PppInterfaceObject *pppObj=NULL;
   Dev2PppInterfaceIpcpObject *pppIpcpObj=NULL;
#ifdef SUPPORT_IPV6
   Dev2PppInterfaceIpv6cpObject *pppIpcpV6Obj=NULL;
#endif
   UBOOL8 found=FALSE;
   UBOOL8 passed=FALSE;

   cmsLog_debug("Enter: type %d, pDiagId->protocolStr/name %s/%s",
                type,pDiagId->protocolStr,pDiagId->layer3Name);

   while ((!found) &&
          ((ret = cmsObj_getNext(MDMOID_DEV2_PPP_INTERFACE, &iidStack, (void **) &pppObj)) == CMSRET_SUCCESS))
   {
      if (cmsUtl_strcmp(pppObj->name, pDiagId->layer3Name) == 0)
      {
         found = TRUE;
         break;
      }
      cmsObj_free((void **) &pppObj);
   }
   if (found)
   {     
      /* all tests passed, there is no need to check anything */
      if (cmsUtl_strcmp(pppObj->connectionStatus, MDMVS_CONNECTED) == 0)
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
             */
            if ((cmsUtl_strcmp(pppObj->connectionStatus, MDMVS_CONNECTING) == 0) ||
                (cmsUtl_strcmp(pppObj->connectionStatus, MDMVS_DISCONNECTED) == 0 &&
                 cmsUtl_strcmp(pppObj->lastConnectionError, MDMVS_ERROR_AUTHENTICATION_FAILURE) == 0))
            {
               passed = TRUE;
            }
            break;
         case BCM_DIAG_PPPOE_AUTH:
            if ((cmsUtl_strcmp(pppObj->connectionStatus, MDMVS_DISCONNECTED) == 0) &&
                (cmsUtl_strcmp(pppObj->lastConnectionError, MDMVS_ERROR_AUTHENTICATION_FAILURE) == 0))
            {
               passed = FALSE;
            }
            else
            {
               passed = TRUE;
            }
            break;
         case BCM_DIAG_PPPOE_IP_ADDR:
            if ((ret = cmsObj_get(MDMOID_DEV2_PPP_INTERFACE_IPCP, &iidStack, OGF_NO_VALUE_UPDATE,
                                  (void **) &pppIpcpObj)) == CMSRET_SUCCESS)
            {
               if (pppIpcpObj->localIPAddress != NULL)
               {
                  passed = TRUE;
               }
               cmsObj_free((void **) &pppIpcpObj);
            }
#ifdef SUPPORT_IPV6
            if (!passed)
            {
               if ((ret = cmsObj_get(MDMOID_DEV2_PPP_INTERFACE_IPV6CP, &iidStack, OGF_NO_VALUE_UPDATE,
                                     (void **) &pppIpcpV6Obj)) == CMSRET_SUCCESS)
               {
                  if (pppIpcpV6Obj->localInterfaceIdentifier != NULL)
                  {
                     passed = TRUE;
                  }
                  cmsObj_free((void **) &pppIpcpV6Obj);
               }            
            }
#endif         
            break;
         } /* switch */
      }
      if (passed)
      {
         p += sprintf(p, "<font color='green'>PASS</font>");
      }
      else
      {
         p += sprintf(p, "<font color='red'>FAIL</font>");
      }
      cmsObj_free((void **) &pppObj);
   } /* found */
   else
   {
      cmsLog_debug("PPP connection is not found");
      p += sprintf(p, "<font color='#ff9900'>DISABLED</font>");
   }
   *p = 0;
   return;
}

#ifdef DMP_DEVICE2_IPPING_1
void cgiPrintDefaultGatewayDiag_dev2(char *varValue)
{
   char *p = varValue;
   CmsRet result = CMSRET_INTERNAL_ERROR;
   CmsRet ret = CMSRET_INTERNAL_ERROR;
   char gwIfc[CMS_IFNAME_LENGTH]={0};
   char gwIp[CMS_IPADDR_LENGTH]={0};

   Dev2IpPingDiagObject pingParms, pingResult;

   cmsLog_debug("Enter");

   /* dalRt_getActiveDefaultGateway is actually returning the gw IP address, not ifname */
   if ((ret = dalRt_getDefaultGatewayIP(gwIp)) == CMSRET_SUCCESS)
   {
      if ((gwIp[0] != '\0'))
      {
         cmsLog_debug("gwIp %s",gwIp);
         if ((ret = dalRt_getActiveDefaultGateway(gwIfc)) == CMSRET_SUCCESS)
         {
            /* set the IPPing object */
            pingParms.diagnosticsState = cmsMem_strdup(MDMVS_REQUESTED);
            pingParms.interface  = cmsMem_strdup(gwIfc);

            cmsLog_debug("gwIfc %s, pingParms.interface %s",pingParms.interface);

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
         } /* gwIfc */
         else
         {
            cmsLog_debug("dalRt_getActiveDefaultGateway(gwIfc %s) returns error");
         }
      } /* gwIP */
   } /* default gateway */
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
#endif /* DMP_DEVICE2_IPPING_1 */

#ifdef DMP_DEVICE2_IPPING_1
void cgiPrintDnsDiag_dev2(char *varValue)
{
   char *p = varValue;
   CmsRet ret = CMSRET_INTERNAL_ERROR;
   CmsRet result = CMSRET_INTERNAL_ERROR;
   char dnsPrimary[CMS_IPADDR_LENGTH] = {0};
   char dnsSecondary[CMS_IPADDR_LENGTH] = {0};
   Dev2IpPingDiagObject pingParms, pingResult;

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
#endif /* DMP_DEVICE2_IPPING_1 */

#endif /* `DMP_DEVICE2_BASELINE_1 */
