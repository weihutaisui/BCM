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
//  Filename:       wanipconnectionhandlers.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "wanipconnection.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"


int WANIPConnection_GetVar(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject  *ipConn = NULL;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);

   if (cmsObj_get(MDMOID_WAN_IP_CONN, &iidStack, 0, (void **)&ipConn) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
  
   OutputNumValueToAC(ac, VAR_Enable, ipConn->enable);  
   OutputCharValueToAC(ac, VAR_ConnectionType, ipConn->connectionType);      
   OutputCharValueToAC(ac, VAR_PossibleConnectionTypes, ipConn->possibleConnectionTypes);      
   OutputCharValueToAC(ac, VAR_ConnectionStatus, ipConn->connectionStatus); 
   OutputCharValueToAC(ac, VAR_Name, ipConn->name);  
   OutputNumValueToAC(ac, VAR_Uptime, ipConn->uptime);      
   OutputNumValueToAC(ac, VAR_NATEnabled, ipConn->NATEnabled);      
   OutputCharValueToAC(ac, VAR_ExternalIPAddress, ipConn->externalIPAddress);
   OutputCharValueToAC(ac, VAR_SubnetMask, ipConn->subnetMask);  
   OutputCharValueToAC(ac, VAR_DefaultGateway, ipConn->defaultGateway);      
   OutputCharValueToAC(ac, VAR_DNSServers, ipConn->DNSServers);      
#if 0
   /* cms lib doesn't support this parameter yet */
   OutputCharValueToAC(ac, VAR_ConnectionTrigger, ipConn->connectionTrigger); 
#endif

   cmsObj_free((void **) &ipConn);
   return TRUE;
}

int WANIPConnection_GetConnectionTypeInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject  *ipConn = NULL;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);

   if (cmsObj_get(MDMOID_WAN_IP_CONN, &iidStack, 0, (void **)&ipConn) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
  
   OutputCharValueToAC(ac, VAR_ConnectionType, ipConn->connectionType);      
   OutputCharValueToAC(ac, VAR_PossibleConnectionTypes, ipConn->possibleConnectionTypes);      

   cmsObj_free((void **) &ipConn);
   return TRUE;
}


int WANIPConnection_GetStatusInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject  *ipConn = NULL;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);

   if (cmsObj_get(MDMOID_WAN_IP_CONN, &iidStack, 0, (void **)&ipConn) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
  
   OutputCharValueToAC(ac, VAR_ConnectionStatus, ipConn->connectionStatus); 
   OutputNumValueToAC(ac, VAR_Uptime, ipConn->uptime);     

   cmsObj_free((void **) &ipConn);
   return TRUE;
}

/* Remove since not defined in TR064.pdf
int WANIPConnection_GetStatistics(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnStatsObject *ipConnStats = NULL;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);

   if (cmsObj_get(MDMOID_WAN_IP_CONN_STATS, &iidStack, 0, (void **)&ipConnStats) != CMSRET_SUCCESS)   
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputNumValueToAC(ac, VAR_BytesSent, ipConnStats->ethernetBytesSent);   
   OutputNumValueToAC(ac, VAR_BytesReceived, ipConnStats->ethernetBytesReceived);   
   OutputNumValueToAC(ac, VAR_PacketsSent, ipConnStats->ethernetPacketsSent);   
   OutputNumValueToAC(ac, VAR_PacketsReceived, ipConnStats->ethernetPacketsReceived);   

   cmsObj_free((void **) &ipConnStats);
   return TRUE;
}
*/


int WANIPConnection_GetExternalIPAddress(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject  *ipConn = NULL;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);

   if (cmsObj_get(MDMOID_WAN_IP_CONN, &iidStack, 0, (void **)&ipConn) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
  
   OutputCharValueToAC(ac, VAR_ExternalIPAddress, ipConn->externalIPAddress);   

   cmsObj_free((void **) &ipConn);
   return TRUE;
}

int SetWANIPConnEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject  *ipConn = NULL;

   struct Param *pParams;
   
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);

   pParams = findActionParamByRelatedVar(ac,VAR_Enable);
   if (pParams != NULL)
   {
      if (cmsObj_get(MDMOID_WAN_IP_CONN, &iidStack, 0, (void **)&ipConn) == CMSRET_SUCCESS)
      {
         ipConn->enable = atoi(pParams->value);
         cmsObj_set(ipConn, &iidStack);
               
         cmsObj_free((void **) &ipConn);
      }
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;

}

int SetIPConnectionType(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject  *ipConn = NULL;

   struct Param *pParams;
   
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);

   pParams = findActionParamByRelatedVar(ac,VAR_ConnectionType);
   if (pParams != NULL)
   {
      if (cmsObj_get(MDMOID_WAN_IP_CONN, &iidStack, 0, (void **)&ipConn) == CMSRET_SUCCESS)
      {
         cmsMem_free(ipConn->connectionType);
         ipConn->connectionType = cmsMem_strdup(pParams->value);
         cmsObj_set(ipConn, &iidStack);
       
         cmsObj_free((void **) &ipConn);
      }
      else
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMgm_saveConfigToFlash();
   return TRUE;
}
       
int SetIPInterfaceInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject  *ipConn = NULL;

   struct Param *pParams;
   char AddressingType[32];
   char ExtIPAddress[32];
   char SubnetMask[32];
   char DefaultGateway[32];

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);
   
   pParams = findActionParamByRelatedVar(ac,VAR_AddressingType);
   if (pParams != NULL)
   {
      strcpy(AddressingType, pParams->value); 
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_ExternalIPAddress);
   if (pParams != NULL)
   {
      strcpy(ExtIPAddress, pParams->value); 
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_SubnetMask);
   if (pParams != NULL)
   {
      strcpy(SubnetMask, pParams->value); 
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   

   pParams = findActionParamByRelatedVar(ac,VAR_DefaultGateway);
   if (pParams != NULL)
   {
      strcpy(DefaultGateway, pParams->value); 
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }  

   if (cmsObj_get(MDMOID_WAN_IP_CONN, &iidStack, 0, (void **)&ipConn) == CMSRET_SUCCESS)
   {
      cmsMem_free(ipConn->addressingType);
      ipConn->addressingType = cmsMem_strdup(AddressingType);
      cmsMem_free(ipConn->externalIPAddress);
      ipConn->externalIPAddress = cmsMem_strdup(ExtIPAddress);
      cmsMem_free(ipConn->subnetMask);
      ipConn->subnetMask = cmsMem_strdup(SubnetMask);
      cmsMem_free(ipConn->defaultGateway);
      ipConn->defaultGateway = cmsMem_strdup(DefaultGateway);
      cmsObj_set(ipConn, &iidStack);
      cmsObj_free((void **) &ipConn);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMgm_saveConfigToFlash();
   return TRUE;

}

int SetIPConnectionTrigger(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
#if 0
   /* cms lib doesn't support this parameter */
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;  
   WanIpConnObject  *ipConn = NULL;

   struct Param *pParams;
   
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);

   pParams = findActionParamByRelatedVar(ac,VAR_ConnectionTrigger);
   if (pParams != NULL)
   {
      if (cmsObj_get(MDMOID_WAN_IP_CONN, &iidStack, 0, (void **)&ipConn) == CMSRET_SUCCESS)
      {
         cmsMem_free(ipConn->connectionTrigger);
         ipConn->connectionTrigger = cmsMem_strdup(pParams->value);
         cmsObj_set(ipConn, &iidStack);
         cmsObj_free((void **) &ipConn);
      }    
      else
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMgm_saveConfigToFlash();
#endif /* 0 */
   return TRUE;

}

int ForceTerminationTR64(UFILE *uclient, PService psvc, PAction ac,
                     pvar_entry_t args, int nargs)
{
#ifdef LGD_TODO
#endif    
   return TRUE;
}

int RequestTerminationTR64(UFILE *uclient, PService psvc, PAction ac,
                     pvar_entry_t args, int nargs)
{
#ifdef LGD_TODO
#endif    
   return TRUE;
}

int RequestConnectionTR64(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
#ifdef LGD_TODO
#endif
   return TRUE;
}

int GetPortMappingNumberOfEntries(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack sonIidStack = EMPTY_INSTANCE_ID_STACK;

   WanIpConnPortmappingObject *port_mapping = NULL;

   int  cnt = 0;
   int  errorinfo = 0;
   
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);

   INIT_INSTANCE_ID_STACK(&sonIidStack);
   while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN_PORTMAPPING, &iidStack, &sonIidStack,(void **)&port_mapping) == CMSRET_SUCCESS)
   {
      cnt++;
      cmsObj_free((void **) &port_mapping);
   }
 
   errorinfo |= OutputNumValueToAC(ac, VAR_PortMappingNumberOfEntries, cnt);  

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   return TRUE;
}

int GetGenericPortMappingEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack sonIidStack = EMPTY_INSTANCE_ID_STACK;

   WanIpConnPortmappingObject *port_mapping = NULL;

   int  PortMappingIndex =0;
   int  errorinfo = 0;
   int  found = 0;

   struct Param *pParams;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);

   pParams = findActionParamByRelatedVar(ac,VAR_PortMappingNumberOfEntries);
   if (pParams != NULL)
   {
      PortMappingIndex = atoi(pParams->value); 
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   INIT_INSTANCE_ID_STACK(&sonIidStack);
   while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN_PORTMAPPING, &iidStack, &sonIidStack,(void **)&port_mapping) == CMSRET_SUCCESS)
   {
      if(sonIidStack.instance[sonIidStack.currentDepth-1] == PortMappingIndex)
      {
         found = 1;

         errorinfo |= OutputNumValueToAC(ac, VAR_ExternalPort, port_mapping->externalPort);
         errorinfo |= OutputNumValueToAC(ac, VAR_InternalPort, port_mapping->internalPort);
         errorinfo |= OutputCharValueToAC(ac, VAR_InternalClient, port_mapping->internalClient);
         errorinfo |= OutputCharValueToAC(ac, VAR_PortMappingProtocol,  port_mapping->portMappingProtocol);
         errorinfo |= OutputCharValueToAC(ac, VAR_PortMappingDescription, port_mapping->portMappingDescription);
         if(port_mapping->portMappingEnabled)
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_PortMappingEnabled, "1");
         }
         else
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_PortMappingEnabled, "0");
         }
      }
      cmsObj_free((void **) &port_mapping);
   }
   
   if(found ==0)
   {
      soap_error( uclient, SOAP_NOSUCHENTRYINARRAY );
      return FALSE;
   }

   if(errorinfo)
   {
      soap_error( uclient, errorinfo);
      return FALSE;
   }

   return TRUE;
}


int GetSpecificPortMappingEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack sonIidStack = EMPTY_INSTANCE_ID_STACK;

   WanIpConnPortmappingObject *port_mapping = NULL;

   int  errorinfo = 0;
   int  found = 0;

   struct Param *pParams;

   uint32 ExternalPort;
   char PortMappingProtocol[32];

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);

   pParams = findActionParamByRelatedVar(ac,VAR_ExternalPort);
   if (pParams != NULL)
   {
      ExternalPort = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_PortMappingProtocol);
   if (pParams != NULL)
   {
      strcpy(PortMappingProtocol, pParams->value);   
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;      
   } 

   while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN_PORTMAPPING, &iidStack, &sonIidStack,(void **)&port_mapping) == CMSRET_SUCCESS)
   {
      if((ExternalPort == port_mapping->externalPort) 
           &&(0==strcmp(PortMappingProtocol, port_mapping->portMappingProtocol)))
      {
         found = 1;

         errorinfo |= OutputNumValueToAC(ac, VAR_InternalPort, port_mapping->internalPort);
         errorinfo |= OutputCharValueToAC(ac, VAR_InternalClient, port_mapping->internalClient);
         errorinfo |= OutputCharValueToAC(ac, VAR_PortMappingDescription, port_mapping->portMappingDescription);
         if(port_mapping->portMappingEnabled)
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_PortMappingEnabled, "1");
         }
         else
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_PortMappingEnabled, "0");
         }
      }
      cmsObj_free((void **) &port_mapping);
   }

   if(found ==0)
   {
      soap_error( uclient, SOAP_NOSUCHENTRYINARRAY );
      return FALSE;
   }

   if(errorinfo)
   {
      soap_error( uclient, errorinfo);
      return FALSE;
   }

   return TRUE;

}

int AddPortMappingEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack sonIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   WanIpConnPortmappingObject *port_mapping = NULL;

   int  errorinfo = 0;

   struct Param *pParams;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);

   sonIidStack = iidStack;
   if ((ret = cmsObj_addInstance(MDMOID_WAN_IP_CONN_PORTMAPPING, &sonIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new virtual server entry, ret=%d", ret);
      soap_error( uclient, errorinfo );
      return FALSE;                 
   }

   if ((ret = cmsObj_get(MDMOID_WAN_IP_CONN_PORTMAPPING, &sonIidStack, 0, (void **) &port_mapping)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get VirtualServerCfgObject, ret=%d", ret);
      soap_error( uclient, errorinfo );
      return FALSE;  
   }       
   
   pParams = findActionParamByRelatedVar(ac,VAR_ExternalPort);
   if (pParams != NULL)
   {
      port_mapping->externalPort = atoi(pParams->value);
      port_mapping->X_BROADCOM_COM_ExternalPortEnd = atoi(pParams->value);
   }
   else
   {
      errorinfo = SOAP_ARGUMENTVALUEINVALID;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_InternalPort);
   if (pParams != NULL)
   {
      port_mapping->internalPort= atoi(pParams->value);
      port_mapping->X_BROADCOM_COM_InternalPortEnd = atoi(pParams->value);
   }
   else
   {
      errorinfo = SOAP_ARGUMENTVALUEINVALID;
   }       

   pParams = findActionParamByRelatedVar(ac,VAR_PortMappingProtocol);
   if (pParams != NULL)
   {
      cmsMem_free(port_mapping->portMappingProtocol);
      port_mapping->portMappingProtocol = cmsMem_strdup(pParams->value);
   }
   else
   {
      errorinfo = SOAP_ARGUMENTVALUEINVALID;
   }            
   pParams = findActionParamByRelatedVar(ac,VAR_InternalClient);
   if (pParams != NULL)
   {
      cmsMem_free(port_mapping->internalClient);
      port_mapping->internalClient= cmsMem_strdup(pParams->value);
   }
   else
   {
      errorinfo = SOAP_ARGUMENTVALUEINVALID;
   }   

   pParams = findActionParamByRelatedVar(ac,VAR_PortMappingEnabled);
   if (pParams != NULL)
   {
      port_mapping->portMappingEnabled = atoi(pParams->value);
   }
   else
   {
      errorinfo = SOAP_ARGUMENTVALUEINVALID;
   }
   pParams = findActionParamByRelatedVar(ac,VAR_PortMappingDescription);
   if (pParams != NULL)
   {
      cmsMem_free(port_mapping->portMappingDescription);
      port_mapping->portMappingDescription= cmsMem_strdup(pParams->value);
   }
   else
   {
      errorinfo = SOAP_ARGUMENTVALUEINVALID;
   }
   ret = cmsObj_set(port_mapping, &sonIidStack);
   cmsObj_free((void **) &port_mapping);               
 

   if (errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }

   if (ret!=CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   cmsMgm_saveConfigToFlash();
   return TRUE;  
}

int DeletePortMappingEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack sonIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   WanIpConnPortmappingObject *port_mapping = NULL;

   struct Param *pParams;

   int  ExternalPort;
   char PortMappingProtocol[32];
   
   int found = 0;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANIPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANIPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANIPConnection);

   pParams = findActionParamByRelatedVar(ac,VAR_ExternalPort);
   if (pParams != NULL)
   {
      ExternalPort = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ARGUMENTVALUEINVALID );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_PortMappingProtocol);
   if (pParams != NULL)
   {
      strcpy(PortMappingProtocol, pParams->value);   
   }
   else
   {
      soap_error( uclient, SOAP_ARGUMENTVALUEINVALID );
      return FALSE;      
   }   

   while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN_PORTMAPPING, &iidStack, &sonIidStack,(void **)&port_mapping) == CMSRET_SUCCESS)
   {
      if((ExternalPort == port_mapping->externalPort) 
          &&(0==strcmp(PortMappingProtocol, port_mapping->portMappingProtocol)))
      {
         found = 1;
         ret = cmsObj_deleteInstance(MDMOID_WAN_IP_CONN_PORTMAPPING, &sonIidStack);
      }  
      cmsObj_free((void **) &port_mapping);
   }                    

   if (found == 0)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;  
   }

   if (ret != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;  
   }
   cmsMgm_saveConfigToFlash();
   return TRUE;
}
