/*****************************************************************************
//
// Copyright (c) 2005-2012 Broadcom Corporation
// All Rights Reserved
//
// <:label-BRCM:2012:proprietary:standard
// 
//  This program is the proprietary software of Broadcom and/or its
//  licensors, and may only be used, duplicated, modified or distributed pursuant
//  to the terms and conditions of a separate, written license agreement executed
//  between you and Broadcom (an "Authorized License").  Except as set forth in
//  an Authorized License, Broadcom grants no license (express or implied), right
//  to use, or waiver of any kind with respect to the Software, and Broadcom
//  expressly reserves all rights in and to the Software and all intellectual
//  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
//  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
//  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
// 
//  Except as expressly set forth in the Authorized License,
// 
//  1. This program, including its structure, sequence and organization,
//     constitutes the valuable trade secrets of Broadcom, and you shall use
//     all reasonable efforts to protect the confidentiality thereof, and to
//     use this information only in connection with your use of Broadcom
//     integrated circuit products.
// 
//  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
//     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
//     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
//     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
//     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
//     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
//     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
//     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
//     PERFORMANCE OF THE SOFTWARE.
// 
//  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
//     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
//     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
//     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
//     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
//     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
//     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
//     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
//     LIMITED REMEDY.
// :>
//
******************************************************************************
//
//  Filename:       layer3handlers.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "layer3params.h"
#include "tr64defs.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"

//static PDevice find_dev_by_udn(const char *udn);
//static PService find_svc_by_type(PDevice pdev, char *type);
//static PService find_svc_by_name(PDevice pdev, const char *name);

int Layer3Forwarding_Init(PService psvc, service_state_t state)
{
    struct Layer3Forwarding *pdata;
    switch (state)
    {
    case SERVICE_CREATE:
       pdata = (struct Layer3Forwarding *) malloc(sizeof(struct Layer3Forwarding));
       if (pdata) 
       {
          memset(pdata, 0, sizeof(struct Layer3Forwarding));
          pdata->default_svc = NULL;
          psvc->opaque = (void *) pdata;
       }
       break;
    case SERVICE_DESTROY:
       pdata = (struct Layer3Forwarding *) psvc->opaque;
       free(pdata);
       break;
    } 

    return 0;
}

int GetDefaultConnectionService(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)     
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   L3ForwardingObject *L3ForwadingObj=NULL;
   CmsRet ret;
   int errorinfo = 0;

   if ((ret = cmsObj_get(MDMOID_L3_FORWARDING, &iidStack, 0, (void **)&L3ForwadingObj)) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   /*TR098 DataModel for this parameter*/
   /*InternetGatewayDevice.WANDevice.1.WANConnectionDevice.2.WANPPPConnection.1*/
   /*InternetGatewayDevice.WANDevice.1.WANConnectionDevice.2.WANIPConnection.1*/

   if (IS_EMPTY_STRING(L3ForwadingObj->defaultConnectionService))
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_DefaultConnectionService, "");
   }
   else
   {
      if(NULL!=strstr(L3ForwadingObj->defaultConnectionService, "WANPPPConnection"))
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_DefaultConnectionService, L3ForwadingObj->defaultConnectionService);
      }
      else if(NULL!=strstr(L3ForwadingObj->defaultConnectionService, "WANIPConnection"))
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_DefaultConnectionService, L3ForwadingObj->defaultConnectionService);
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_DefaultConnectionService, "");
      }
   }

   cmsObj_free((void **)&L3ForwadingObj);
   
   if(errorinfo)   
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   
   return TRUE;
}

int SetDefaultConnectionService(UFILE *uclient, PService psvc, 
                                PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   L3ForwardingObject *L3ForwadingObj=NULL;
   CmsRet ret;

   struct Param *pParams;
   int result = 0;

   int WANDeviceId = 0;
   int WANConnectionDeviceId = 0;
   int WANConnectionServiceId = 0;

   pParams = findActionParamByRelatedVar(ac,VAR_DefaultConnectionService);
   if (pParams == NULL)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   result = sscanf(pParams->value, "InternetGatewayDevice.WANDevice.%d.WANConnectionDevice.%d.WANPPPConnection.%d", &WANDeviceId, &WANConnectionDeviceId, &WANConnectionServiceId);

   if( result < 0 )
   {
      soap_error( uclient, SOAP_ARGUMENTVALUEINVALID );
      return FALSE;
   }

   if ((ret = cmsObj_get(MDMOID_L3_FORWARDING, &iidStack, 0, (void **)&L3ForwadingObj)) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   /*TR098 DataModel for this parameter*/
   /*InternetGatewayDevice.WANDevice.1.WANConnectionDevice.2.WANPPPConnection.1*/
   /*InternetGatewayDevice.WANDevice.1.WANConnectionDevice.2.WANIPConnection.1*/

   if (cmsUtl_strcmp(pParams->value, L3ForwadingObj->defaultConnectionService))
   {
      CMSMEM_REPLACE_STRING(L3ForwadingObj->defaultConnectionService, pParams->value);
      ret = cmsObj_set(L3ForwadingObj, &iidStack);
   }
 
   cmsObj_free((void **)&L3ForwadingObj);
   
   if(ret != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ARGUMENTVALUEINVALID );
      return FALSE;
   }   
   cmsMgm_saveConfigToFlash();
   return TRUE;
}

#if 0 /* not used */
static PDevice find_dev_by_udn(const char *udn)
{
   PDevice pdev = NULL;
    
   forall_devices(pdev) 
   {
      if (strcmp(pdev->udn, udn) == 0) 
      {
         break;
      }
   }
    
   return pdev;
}


static PService find_svc_by_type(PDevice pdev, char *type)
{
   char *name = NULL, *p;
   PService psvc = NULL;
   
   p = rindex(type, ':');
   if (p != 0) 
   {
      *p = '\0';
      p = rindex(type, ':');
      if (p != 0)
         name = p+1;
   }

   if (name) 
      psvc = find_svc_by_name(pdev, name);

   return psvc;
}

static PService find_svc_by_name(PDevice pdev, const char *name)
{
   PService psvc;

   forall_services(pdev, psvc) 
   {
      if (strcmp(psvc->template->name, name) == 0) 
      {
         break;
      }
   }

   return psvc;
}
#endif

CmsRet fullPathToIntfname(const char *mdmPath, char *intfname)
{
   MdmPathDescriptor pathDesc;
   void *mdmObj = NULL;
   CmsRet ret;

   if (IS_EMPTY_STRING(mdmPath) || intfname == NULL)
   {
      cmsLog_error("invalid argument. mdmPath=0x%x intfname=0x%x", (uintptr_t)mdmPath, (uintptr_t)intfname);
      return CMSRET_INVALID_ARGUMENTS;
   }

   intfname[0] = '\0';

   if ((ret = cmsMdm_fullPathToPathDescriptor(mdmPath, &pathDesc)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor returns error. ret=%d", ret);
      return ret;
   }

   /* get the interface object */
   if ((ret = cmsObj_get(pathDesc.oid, &pathDesc.iidStack, 0, &mdmObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get returns error. ret=%d", ret);
      return ret;
   }

   switch (pathDesc.oid)
   {
   case MDMOID_LAN_ETH_INTF:
      strcpy(intfname, ((LanEthIntfObject *)mdmObj)->X_BROADCOM_COM_IfName);
      break;
#ifdef BRCM_WLAN
   case MDMOID_WL_VIRT_INTF_CFG:
      strcpy(intfname, ((WlVirtIntfCfgObject *)mdmObj)->wlIfcname);
      break;
#endif   
   case MDMOID_LAN_USB_INTF:
      strcpy(intfname, ((LanUsbIntfObject *)mdmObj)->X_BROADCOM_COM_IfName);
      break;
   case MDMOID_WAN_IP_CONN:
      strcpy(intfname, ((WanIpConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
      break;
   case MDMOID_WAN_PPP_CONN:
      strcpy(intfname, ((WanPppConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
      break;
   default:
      cmsLog_error("Invalid interface: %s", mdmPath);
      return CMSRET_INVALID_ARGUMENTS;
   }
   cmsObj_free(&mdmObj);

   return ret;

} 
 
int AddForwardingEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   struct Param *pParams;
   int errorinfo = 0;
   CmsRet ret = CMSRET_SUCCESS;

   char addr[BUFLEN_32];
   char mask[BUFLEN_32];
   char gtwy[BUFLEN_32];
   char wanIf[BUFLEN_32];
   char fullPath[BUFLEN_128];

   addr[0] = mask[0] = gtwy[0] = wanIf[0] = fullPath[0] = '\0';

   pParams = findActionParamByRelatedVar(ac,VAR_DestIPAddress);
   if (pParams != NULL)
   {
      strcpy(addr, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_DestSubnetMask);
   if (pParams != NULL)
   {
      strcpy(mask, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_GatewayIPAddress);
   if (pParams != NULL)
   {
      strcpy(gtwy, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_Interface);
   if (pParams != NULL)
   {
      sprintf(fullPath,"%s.", pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   if(fullPathToIntfname(fullPath, wanIf)!=CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_INVALIDARGS );
      return FALSE;
   }

   cmsLog_debug("addr=%s, mask=%s, gtwy=%s, wanIf=%s",addr, mask, gtwy, wanIf);

   if (cmsUtl_isValidIpAddress(AF_INET, addr) == FALSE || !strcmp(addr, "0.0.0.0"))
   {
      cmsLog_error("Invalid destination IP address");
      errorinfo = SOAP_INVALIDARGS;
      soap_error( uclient, errorinfo );
      return FALSE;
   }

   if (cmsUtl_isValidIpAddress(AF_INET, mask) == FALSE || !strcmp(mask, "0.0.0.0"))
   {
      cmsLog_error("Invalid destination subnet mask");
      errorinfo = SOAP_INVALIDARGS;
      soap_error( uclient, errorinfo );
      return FALSE;
   }

   if (cmsUtl_isValidIpAddress(AF_INET, gtwy) == FALSE)
   {
      cmsLog_error("Invalid gateway IP address");
      errorinfo = SOAP_INVALIDARGS;
      soap_error( uclient, errorinfo );
      return FALSE;
   }

   if ((ret = dalStaticRoute_addEntry(addr, mask, gtwy, wanIf, "")) != CMSRET_SUCCESS) 
   {
      cmsLog_error("dalStaticRoute_addEntry failed, ret=%d", ret);
      errorinfo = SOAP_ACTIONFAILED;
   }   
   
   if (errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }
   cmsMgm_saveConfigToFlash();
   return TRUE;    
}

int DeleteForwardingEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
/*
   Index Support 
    0     Y        {"NewDestIPAddress", VAR_DestIPAddress, VAR_IN},
    1     Y        {"NewDestSubnetMask", VAR_DestSubnetMask, VAR_IN},
    2     N        {"NewSourceIPAddress", VAR_SourceIPAddress, VAR_IN},
    3     N        {"NewSourceSubnetMask", VAR_SourceSubnetMask, VAR_IN},
 */     
{
   CmsRet ret = CMSRET_SUCCESS;
   char addr[BUFLEN_40], mask[BUFLEN_40];
   
   struct Param *pParams;

   pParams = findActionParamByRelatedVar(ac,VAR_DestIPAddress);
   if (pParams != NULL)
   {
      strcpy(addr, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_DestSubnetMask);
   if (pParams != NULL)
   {
      strcpy(mask, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;      
   }

   if ((ret = dalStaticRoute_deleteEntry(addr, mask)) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalStaticRoute_deleteInterface failed for %s/%s (ret=%d)", addr, mask, ret);
      soap_error( uclient, SOAP_NOSUCHENTRYINARRAY );
      return FALSE; 
   } 
   cmsMgm_saveConfigToFlash();
   return TRUE;    
}

int GetGenericForwardingEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   L3ForwardingEntryObject *routeCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   int errorinfo = 0;   
   int index = 0;
   struct Param *pParams;

   pParams = findActionParamByRelatedVar(ac,VAR_ForwardNumberOfEntries);
   if (pParams != NULL)
   {
      index = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   while ((ret = cmsObj_getNext(MDMOID_L3_FORWARDING_ENTRY, &iidStack, (void **) &routeCfg)) == CMSRET_SUCCESS)
   {
      if(iidStack.instance[iidStack.currentDepth-1] == index)
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &routeCfg);
      }
   }

   if(found)
   {
      if ( routeCfg->enable )
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "1");
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "0");
      }

      errorinfo |= OutputCharValueToAC(ac, VAR_Status, routeCfg->status);
      errorinfo |= OutputCharValueToAC(ac, VAR_DestIPAddress, routeCfg->destIPAddress);
      errorinfo |= OutputCharValueToAC(ac, VAR_DestSubnetMask, routeCfg->destSubnetMask);
      errorinfo |= OutputCharValueToAC(ac, VAR_GatewayIPAddress, routeCfg->gatewayIPAddress);
      errorinfo |= OutputCharValueToAC(ac, VAR_Interface, routeCfg->interface);

      cmsObj_free((void **) &routeCfg);
   }
   else
   {
      soap_error( uclient, SOAP_SPECIFIEDARRAYINDEXINVALID );
      return FALSE;
   }



   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }

   return TRUE;
}

int GetSpecificForwardingEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   L3ForwardingEntryObject *routeCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   int errorinfo = 0;   
   char DestIPAddress[32];
   char DestSubnetMask[32]; 
   struct Param *pParams;

   //DestIPAddress
   pParams = findActionParamByRelatedVar(ac,VAR_DestIPAddress);
   if (pParams != NULL)
   {
      strcpy(DestIPAddress, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
    //DestSubnetMask
   pParams = findActionParamByRelatedVar(ac,VAR_DestSubnetMask);
   if (pParams != NULL)
   {
      strcpy(DestSubnetMask, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;      
   } 

   while ((ret = cmsObj_getNext(MDMOID_L3_FORWARDING_ENTRY, &iidStack, (void **) &routeCfg)) == CMSRET_SUCCESS)
   {
      if((0==strcmp(routeCfg->destIPAddress, DestIPAddress))
          &&(0==strcmp(routeCfg->destSubnetMask, DestSubnetMask)))
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &routeCfg);
      }
   }

   if(found)
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_Status, routeCfg->status);
      errorinfo |= OutputCharValueToAC(ac, VAR_DestIPAddress, routeCfg->destIPAddress);
      errorinfo |= OutputCharValueToAC(ac, VAR_DestSubnetMask, routeCfg->destSubnetMask);
      errorinfo |= OutputCharValueToAC(ac, VAR_GatewayIPAddress, routeCfg->gatewayIPAddress);
      errorinfo |= OutputCharValueToAC(ac, VAR_Interface, routeCfg->interface);
      if ( routeCfg->enable )
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "1");
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "0");
      }
   
      cmsObj_free((void **) &routeCfg);
   }
   else
   {
      soap_error( uclient, SOAP_SPECIFIEDARRAYINDEXINVALID );
      return FALSE;
   }

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }

   return TRUE;
}

int SetForwardingEntryEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
/*
Index Support
0      Y      {"NewDestIPAddress", VAR_DestIPAddress, VAR_IN},
1      Y       {"NewDestSubnetMask", VAR_DestSubnetMask, VAR_IN},
2      N       {"NewSourceIPAddress", VAR_SourceIPAddress, VAR_IN},
3      N       {"NewSourceSubnetMask", VAR_SourceSubnetMask, VAR_IN},  
4      Y      {"NewEnable", VAR_Enable, VAR_IN},
*/
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   L3ForwardingEntryObject *routeCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   char DestIPAddress[32];
   char DestSubnetMask[32];
   struct Param *pParams;
   
   pParams = findActionParamByRelatedVar(ac,VAR_DestIPAddress);
   if (pParams != NULL)
   {
      strcpy(DestIPAddress, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_DestSubnetMask);
   if (pParams != NULL)
   {
      strcpy(DestSubnetMask, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;      
   }      

   while ((ret = cmsObj_getNext(MDMOID_L3_FORWARDING_ENTRY, &iidStack, (void **) &routeCfg)) == CMSRET_SUCCESS)
   {
      if((0==strcmp(routeCfg->destIPAddress, DestIPAddress))
          &&(0==strcmp(routeCfg->destSubnetMask, DestSubnetMask)))
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &routeCfg);
      }
   }

   if(found)
   {
      pParams = findActionParamByRelatedVar(ac,VAR_Enable);
      if (pParams != NULL)
      {
         if(strcmp(pParams->value,"0")==0)
         {
            routeCfg->enable = 0;
         }
         else
         {
            routeCfg->enable = 1;
         }
         ret = cmsObj_set(routeCfg, &iidStack);
         cmsObj_free((void **) &routeCfg);

         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("set of MDMOID_L3_FORWARDING_ENTRY failed, ret=%d", ret);
            soap_error( uclient, SOAP_ACTIONFAILED );
            return FALSE;
         } 
      }
      else
      {
         cmsObj_free((void **) &routeCfg);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;      
      }      

   }
   else
   {
      soap_error( uclient, SOAP_NOSUCHENTRYINARRAY );
      return FALSE;
   }
   cmsMgm_saveConfigToFlash();
   return TRUE;
}

int GetForwardNumberOfEntries(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
/*
       {"NewForwardNumberOfEntries", VAR_ForwardNumberOfEntries, VAR_OUT}, 
*/       
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   L3ForwardingObject *L3ForwadingObj=NULL;
   CmsRet ret;
   int errorinfo = 0;

   if ((ret = cmsObj_get(MDMOID_L3_FORWARDING, &iidStack, 0, (void **)&L3ForwadingObj)) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   errorinfo |= OutputNumValueToAC(ac, VAR_ForwardNumberOfEntries, L3ForwadingObj->forwardNumberOfEntries);

   cmsObj_free((void **)&L3ForwadingObj);
   
   if(errorinfo)   
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   
   return TRUE;
}
