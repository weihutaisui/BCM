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
//  Filename:       lanhandlers.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "lanhostcfgmgtparams.h"
#include "tr64defs.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

int LANHostConfigManagement_GetVar(struct Service *psvc, int varindex)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanHostCfgObject *lanHostCfg=NULL;
   CmsRet ret;
   
   int instanceOfLANDevice;
   struct StateVar *var;

   var = &(psvc->vars[varindex]);

   instanceOfLANDevice = psvc->device->instance;
   
   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
  
   if ((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, &iidStack, 0, (void *) &lanHostCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);
      return FALSE;
   }
  
   switch (varindex) 
   {
      case VAR_DHCPServerConfigurable:
         if ( lanHostCfg->DHCPServerConfigurable )
         {
            sprintf(var->value, "1");
         }
         else
         {
            sprintf(var->value, "0");
         }
      break;
      
      case VAR_DHCPServerEnable:
         if ( lanHostCfg->DHCPServerEnable )
         {    
            sprintf(var->value, "1");
         }
         else
         {
            sprintf(var->value, "0");
         }
      break;

      case VAR_DHCPRelay:
         if ( lanHostCfg->DHCPRelay )
         {    
            sprintf(var->value, "1");
         }
         else
         {
            sprintf(var->value, "0");
         }
      break;
      
      case VAR_MinAddress:
         if( lanHostCfg->minAddress )
         {
            strcpy(var->value, lanHostCfg->minAddress);
         }
      break;

      case VAR_MaxAddress:
         if( lanHostCfg->maxAddress )
         {
            strcpy(var->value, lanHostCfg->maxAddress);
         }
      break;

      case VAR_ReservedAddresses:
         if( lanHostCfg->reservedAddresses )
         {
            strcpy(var->value, lanHostCfg->reservedAddresses);
         }
      break;

      case VAR_SubnetMask:
         if( lanHostCfg->subnetMask )
         {
            strcpy(var->value, lanHostCfg->subnetMask);
         }
      break;
      
      case VAR_DHCPLeaseTime:
         sprintf(var->value, "%u", lanHostCfg->DHCPLeaseTime);
      break;
   
      case VAR_DNSServers:
         if( lanHostCfg->DNSServers )
         {
            strcpy(var->value, lanHostCfg->DNSServers);
         }
      break;
   
      case VAR_DomainName:
         if( lanHostCfg->domainName )
         {
            strcpy(var->value, lanHostCfg->domainName);
         }
      break;
      
      case VAR_IPRouters:
         if( lanHostCfg->IPRouters )
         {
            strcpy(var->value, lanHostCfg->IPRouters);
         }
       break;   
    }

    cmsObj_free((void **) &lanHostCfg);
    return TRUE;
}

int SetDHCPLeaseTime(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   LanHostCfgObject *lanHostCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   int instanceOfLANDevice;
   struct Param *pParams;

   instanceOfLANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);

   if ((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, &iidStack, 0, (void *) &lanHostCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   
   pParams = findActionParamByRelatedVar(ac,VAR_DHCPLeaseTime);
   if (pParams != NULL)
   {
      lanHostCfg->DHCPLeaseTime = atoi(pParams->value);   
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      cmsObj_free((void **) &lanHostCfg);
      return FALSE;
   }      
  
   ret = cmsObj_set(lanHostCfg, &iidStack);
   cmsObj_free((void **) &lanHostCfg);

   if (ret != CMSRET_SUCCESS)
   {
       cmsLog_error("set of MDMOID_LAN_HOST_CFG failed, ret=%d", ret);
       soap_error( uclient, SOAP_ACTIONFAILED );
       return FALSE;
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;
}

int SetDHCPServerConfigurable(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   LanHostCfgObject *lanHostCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   
   int instanceOfLANDevice;
   struct Param *pParams;

   instanceOfLANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);

   if ((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, &iidStack, 0, (void *) &lanHostCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   
   pParams = findActionParamByRelatedVar(ac,VAR_DHCPServerConfigurable);
   if (pParams != NULL)
   {
      lanHostCfg->DHCPServerConfigurable = atoi(pParams->value);   
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      cmsObj_free((void **) &lanHostCfg);
      return FALSE;
   }      
  
   ret = cmsObj_set(lanHostCfg, &iidStack);
   cmsObj_free((void **) &lanHostCfg);

   if (ret != CMSRET_SUCCESS)
   {
       cmsLog_error("set of MDMOID_LAN_HOST_CFG failed, ret=%d", ret);
       soap_error( uclient, SOAP_ACTIONFAILED );
       return FALSE;
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;
}

int SetIPRouter(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   LanHostCfgObject *lanHostCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   int instanceOfLANDevice;
   struct Param *pParams;

   instanceOfLANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);

   if ((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, &iidStack, 0, (void *) &lanHostCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   
   pParams = findActionParamByRelatedVar(ac,VAR_IPRouters);
   if (pParams != NULL)
   {
      cmsMem_free(lanHostCfg->IPRouters);
      lanHostCfg->IPRouters = cmsMem_strdup(pParams->value);   
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      cmsObj_free((void **) &lanHostCfg);
      return FALSE;
   }      
  
   ret = cmsObj_set(lanHostCfg, &iidStack);
   cmsObj_free((void **) &lanHostCfg);

   if (ret != CMSRET_SUCCESS)
   {
       cmsLog_error("set of MDMOID_LAN_HOST_CFG failed, ret=%d", ret);
       soap_error( uclient, SOAP_ACTIONFAILED );
       return FALSE;
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;
}

int SetDomainName(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   LanHostCfgObject *lanHostCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   int instanceOfLANDevice;
   struct Param *pParams;

   instanceOfLANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);

   if ((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, &iidStack, 0, (void *) &lanHostCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   
   pParams = findActionParamByRelatedVar(ac,VAR_DomainName);
   if (pParams != NULL)
   {
      cmsMem_free(lanHostCfg->domainName);
      lanHostCfg->domainName = cmsMem_strdup(pParams->value);   
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      cmsObj_free((void **) &lanHostCfg);
      return FALSE;
   }      
  
   ret = cmsObj_set(lanHostCfg, &iidStack);
   cmsObj_free((void **) &lanHostCfg);

   if (ret != CMSRET_SUCCESS)
   {
       cmsLog_error("set of MDMOID_LAN_HOST_CFG failed, ret=%d", ret);
       soap_error( uclient, SOAP_ACTIONFAILED );
       return FALSE;
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;
}

int SetAddressRange(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   LanHostCfgObject *lanHostCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   int instanceOfLANDevice;
   struct Param *pParams;

   instanceOfLANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);

   if ((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, &iidStack, 0, (void *) &lanHostCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   
   pParams = findActionParamByRelatedVar(ac,VAR_MinAddress);
   if (pParams != NULL)
   {
      cmsMem_free(lanHostCfg->minAddress);
      lanHostCfg->minAddress= cmsMem_strdup(pParams->value);   
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      cmsObj_free((void **) &lanHostCfg);
      return FALSE;
   }      

   pParams = findActionParamByRelatedVar(ac,VAR_MaxAddress);
   if (pParams != NULL)
   {
      cmsMem_free(lanHostCfg->maxAddress);
      lanHostCfg->maxAddress= cmsMem_strdup(pParams->value);   
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      cmsObj_free((void **) &lanHostCfg);
      return FALSE;
   }   
  
   ret = cmsObj_set(lanHostCfg, &iidStack);
   cmsObj_free((void **) &lanHostCfg);

   if (ret != CMSRET_SUCCESS)
   {
       cmsLog_error("set of MDMOID_LAN_HOST_CFG failed, ret=%d", ret);
       soap_error( uclient, SOAP_ACTIONFAILED );
       return FALSE;
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;
}

int GetAddressRange(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   LanHostCfgObject *lanHostCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   int instanceOfLANDevice;

   instanceOfLANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);

   if ((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, &iidStack, 0, (void *) &lanHostCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputCharValueToAC(ac, VAR_MaxAddress, lanHostCfg->maxAddress);  
   OutputCharValueToAC(ac, VAR_MinAddress, lanHostCfg->minAddress);      

   cmsObj_free((void **) &lanHostCfg);

   return TRUE;
}

int SetDNSServer(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   LanHostCfgObject *lanHostCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   int instanceOfLANDevice;
   struct Param *pParams;

   instanceOfLANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);

   if ((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, &iidStack, 0, (void *) &lanHostCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_DNSServers);
   if (pParams != NULL)
   {
      cmsMem_free(lanHostCfg->DNSServers);
      lanHostCfg->DNSServers= cmsMem_strdup(pParams->value);   
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      cmsObj_free((void **) &lanHostCfg);
      return FALSE;
   }   
  
   ret = cmsObj_set(lanHostCfg, &iidStack);
   cmsObj_free((void **) &lanHostCfg);

   if (ret != CMSRET_SUCCESS)
   {
       cmsLog_error("set of MDMOID_LAN_HOST_CFG failed, ret=%d", ret);
       soap_error( uclient, SOAP_ACTIONFAILED );
       return FALSE;
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;
}

int DeleteDNSServer(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   LanHostCfgObject *lanHostCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   int instanceOfLANDevice;
   struct Param *pParams;

   int writedns1 = 0;
   int writedns2 = 0;

   char dns1[BUFLEN_32]={0}; 
   char dns2[BUFLEN_32]={0};
   char tmpbuf[BUFLEN_64]={0};   

   strcpy(dns1, "0.0.0.0");
   strcpy(dns2, "0.0.0.0");

   instanceOfLANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);

   if ((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, &iidStack, 0, (void *) &lanHostCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);
   soap_error( uclient, SOAP_ACTIONFAILED );
   return FALSE;
}

   pParams = findActionParamByRelatedVar(ac,VAR_DNSServers);
   if (pParams != NULL)
   {

      cmsUtl_parseDNS(lanHostCfg->DNSServers, dns1, dns2, TRUE);

      if (cmsUtl_strcmp(dns1, "0.0.0.0"))
      {
         if(cmsUtl_strcmp(dns1, pParams->value))
         {
            writedns1 = 1;
         }
      } 
      if (cmsUtl_strcmp(dns2, "0.0.0.0"))
      {
         if(cmsUtl_strcmp(dns2, pParams->value))
         {
            writedns2 = 1;
         }
      } 

      if((writedns1 ==1) &&(writedns2==1))
      {
         sprintf(tmpbuf, "%s;%s", dns1, dns2);
         cmsMem_free(lanHostCfg->DNSServers);
         lanHostCfg->DNSServers= cmsMem_strdup(tmpbuf);   
      }
      else if(writedns1==1)
      {
         sprintf(tmpbuf, "%s", dns1);  
         cmsMem_free(lanHostCfg->DNSServers);
         lanHostCfg->DNSServers= cmsMem_strdup(tmpbuf);   
      }
      else if(writedns2==1)
      {
         sprintf(tmpbuf, "%s", dns2);  
         cmsMem_free(lanHostCfg->DNSServers);
         lanHostCfg->DNSServers= cmsMem_strdup(tmpbuf);   
      }
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      cmsObj_free((void **) &lanHostCfg);
      return FALSE;
   }   
  
   ret = cmsObj_set(lanHostCfg, &iidStack);
   cmsObj_free((void **) &lanHostCfg);

   if (ret != CMSRET_SUCCESS)
   {
       cmsLog_error("set of MDMOID_LAN_HOST_CFG failed, ret=%d", ret);
       soap_error( uclient, SOAP_ACTIONFAILED );
       return FALSE;
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;
}

int SetDHCPServerEnable(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   LanHostCfgObject *lanHostCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   int instanceOfLANDevice;
   struct Param *pParams;

   instanceOfLANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);

   if ((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, &iidStack, 0, (void *) &lanHostCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_DHCPServerEnable);
   if (pParams != NULL)
   {
      lanHostCfg->DHCPServerEnable = atoi(pParams->value);   
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      cmsObj_free((void **) &lanHostCfg);
      return FALSE;
   }   
  
   ret = cmsObj_set(lanHostCfg, &iidStack);
   cmsObj_free((void **) &lanHostCfg);

   if (ret != CMSRET_SUCCESS)
   {
       cmsLog_error("set of MDMOID_LAN_HOST_CFG failed, ret=%d", ret);
       soap_error( uclient, SOAP_ACTIONFAILED );
       return FALSE;
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;
}

int SetSubnetMask(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   LanHostCfgObject *lanHostCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   int instanceOfLANDevice;
   struct Param *pParams;

   instanceOfLANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);

   if ((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, &iidStack, 0, (void *) &lanHostCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_SubnetMask);
   if (pParams != NULL)
   {
      cmsMem_free(lanHostCfg->subnetMask);
      lanHostCfg->subnetMask = cmsMem_strdup(pParams->value);   
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      cmsObj_free((void **) &lanHostCfg);
      return FALSE;
   }   
  
   ret = cmsObj_set(lanHostCfg, &iidStack);
   cmsObj_free((void **) &lanHostCfg);

   if (ret != CMSRET_SUCCESS)
   {
       cmsLog_error("set of MDMOID_LAN_HOST_CFG failed, ret=%d", ret);
       soap_error( uclient, SOAP_ACTIONFAILED );
       return FALSE;
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;
}

int GetIPInterfaceNumberOfEntries(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   LanHostCfgObject *lanHostCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   
   int instanceOfLANDevice;
   int cnt = 0;

   instanceOfLANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);

   if ((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, &iidStack, 0, (void *) &lanHostCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cnt = lanHostCfg->IPInterfaceNumberOfEntries;

   cmsObj_free((void **) &lanHostCfg);

   if(!OutputNumValueToAC(ac, VAR_IPInterfaceNumberOfEntries, cnt))
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
  
   return TRUE;
}

int GetIPInterfaceGenericEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack lanDevIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack lanIptfIidStack = EMPTY_INSTANCE_ID_STACK;
   LanIpIntfObject *lanIpObj = NULL;
   CmsRet ret;
   int errorinfo = 0;    
   int instanceOfLANDevice;
   int index = 0;
   int found = 0;
   struct Param *pParams;

   instanceOfLANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&lanDevIidStack, instanceOfLANDevice);

   pParams = findActionParamByRelatedVar(ac, VAR_IPInterfaceNumberOfEntries);
   if (pParams != NULL)
   {
      index = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   while ((ret = cmsObj_getNextInSubTree(MDMOID_LAN_IP_INTF, &lanDevIidStack, &lanIptfIidStack, (void **) &lanIpObj)) == CMSRET_SUCCESS)
   {
      /* Assume that bridge name associated with major ip address does not have ":" */
      if(lanIptfIidStack.instance[lanIptfIidStack.currentDepth-1] == index)
      {
         /* do not free lanIpObj */
         found = TRUE;
         break;
      }
   
      cmsObj_free((void **) &lanIpObj);
   }

   if (found == FALSE)
   {
      cmsLog_error("Failed to get IPInterface, ret = %d", ret);
      return FALSE;
   }

   if(lanIpObj->enable)
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "1");      
   }
   else
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "0");      
   }

   errorinfo |= OutputCharValueToAC(ac, VAR_IPInterfaceIPAddress, lanIpObj->IPInterfaceIPAddress);      
   errorinfo |= OutputCharValueToAC(ac, VAR_IPInterfaceSubnetMask, lanIpObj->IPInterfaceSubnetMask);      
   errorinfo |= OutputCharValueToAC(ac, VAR_IPInterfaceAddressingType, lanIpObj->IPInterfaceAddressingType);      

   cmsObj_free((void **) &lanIpObj);

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 

   return TRUE;
}

int SetIPInterface(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack lanDevIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack lanIptfIidStack = EMPTY_INSTANCE_ID_STACK;
   LanIpIntfObject *lanIpObj = NULL;
   CmsRet ret;

   int instanceOfLANDevice;
   struct Param *pParams;
   char Enable[4];
   char IPAddress[32];
   char SubnetMask[32];
   char IPAddressingType[32];   

   instanceOfLANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&lanDevIidStack, instanceOfLANDevice);

   pParams = findActionParamByRelatedVar(ac, VAR_Enable);
   if (pParams != NULL)
   {
      strcpy(Enable, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   pParams = findActionParamByRelatedVar(ac, VAR_IPInterfaceIPAddress);
   if (pParams != NULL)
   {
      strcpy(IPAddress, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   pParams = findActionParamByRelatedVar(ac, VAR_IPInterfaceSubnetMask);
   if (pParams != NULL)
   {
      strcpy(SubnetMask, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   pParams = findActionParamByRelatedVar(ac, VAR_IPInterfaceAddressingType);
   if (pParams != NULL)
   {
      strcpy(IPAddressingType, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   
   
   if ((ret = cmsObj_getNextInSubTree(MDMOID_LAN_IP_INTF, &lanDevIidStack, &lanIptfIidStack, (void **) &lanIpObj)) == CMSRET_SUCCESS)
   {
      lanIpObj->enable = atoi(Enable);
      cmsMem_free(lanIpObj->IPInterfaceIPAddress);
      lanIpObj->IPInterfaceIPAddress = cmsMem_strdup(IPAddress);
      cmsMem_free(lanIpObj->IPInterfaceSubnetMask);
      lanIpObj->IPInterfaceSubnetMask = cmsMem_strdup(SubnetMask);
      cmsMem_free(lanIpObj->IPInterfaceAddressingType);
      lanIpObj->IPInterfaceAddressingType = cmsMem_strdup(IPAddressingType);

      ret = cmsObj_set(lanIpObj, &lanIptfIidStack);
      cmsObj_free((void **) &lanIpObj);
   } 
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;    
   }


   if(ret != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;    
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;
}
