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
//  Filename:       wanpppconnectionhandlers.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "wanpppconnection.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

extern void *msgHandle;

/* use this function from rut_util.c */
extern UINT32 rut_sendMsgToSmd(CmsMsgType msgType, UINT32 wordData, void *msgData, UINT32 msgDataLen);

void SendMessageToChangePPPStateToUnconfigMode(UINT32 specificEid)

{
   char buf[sizeof(CmsMsgHeader) + sizeof(PppoeStateChangeMsgBody)]={0};
   CmsMsgHeader *msg=(CmsMsgHeader *) buf;
   PppoeStateChangeMsgBody *pppoeBody = (PppoeStateChangeMsgBody *) (msg+1);
   CmsRet ret;

   msg->type = CMS_MSG_PPPOE_STATE_CHANGED; 
   msg->src = specificEid;
   msg->dst = EID_SSK;
   msg->flags_event = 1;
   msg->dataLength = sizeof(PppoeStateChangeMsgBody);

   pppoeBody->pppState = BCM_PPPOE_CLIENT_STATE_UNCONFIGURED;
     
   if ((ret = cmsMsg_send(msgHandle, msg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not send out CMS_MSG_PPP_STATE_CHANGED, ret=%d", ret);
   }

}

int StopPPPDaemon(int instanceOfWANDevice, int instanceOfWANConnectionDevice, int instanceOfWANPPPConnection)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   WanPppConnObject  *pppConn = NULL;
   UINT32 specificEid;
   UINT32 pppdPid=0;
#if LGD_TODO
   UINT32 isDisconnected = 1;
#endif
   CmsRet ret;
   
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

   if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) == CMSRET_SUCCESS)
   {
       pppdPid = pppConn->X_BROADCOM_COM_PppdPid;
#if LGD_TODO
       if(strcmp(pppConn->connectionStatus, MDMVS_DISCONNECTED) == 0 )
       {

          isDisconnected = 0;
       }
#endif
       cmsObj_free((void **) &pppConn);
   }
#if LGD_TODO
   if((pppdPid != 0) && (isDisconnected == 0))
#else
   if(pppdPid != 0)
#endif
   {
      specificEid = MAKE_SPECIFIC_EID(pppdPid, EID_PPP); 
      fprintf(stderr, "send message to kill ppp %d\n", specificEid);
      if ((ret = rut_sendMsgToSmd(CMS_MSG_STOP_APP, specificEid, NULL, 0)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to stop pppd");
      }
      else
      {
         cmsLog_debug("pppd stopped");
      }
   }

   return TRUE;
}

void StartPPPDaemon(int instanceOfWANDevice, int instanceOfWANConnectionDevice, int instanceOfWANPPPConnection)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   WanPppConnObject  *pppConn = NULL;
   UINT32 specificEid;
   UINT32 pppdPid=0;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

   if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) == CMSRET_SUCCESS)
   {
       pppdPid = pppConn->X_BROADCOM_COM_PppdPid;
       cmsObj_free((void **) &pppConn);
   }

   if(pppdPid!=0)
   {
      specificEid = MAKE_SPECIFIC_EID(pppdPid, EID_PPP); 
      SendMessageToChangePPPStateToUnconfigMode(specificEid);
   }
}



int WANPPPConnection_GetInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject  *pppConn = NULL;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);


   if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputNumValueToAC(ac, VAR_Enable, pppConn->enable);  
   OutputCharValueToAC(ac, VAR_ConnectionType, pppConn->connectionType);      
   OutputCharValueToAC(ac, VAR_ConnectionStatus, pppConn->connectionStatus); 
   OutputCharValueToAC(ac, VAR_Name, pppConn->name);  
   OutputNumValueToAC(ac, VAR_IdleDisconnectTime, pppConn->idleDisconnectTime);      
   OutputNumValueToAC(ac, VAR_NATEnabled, pppConn->NATEnabled);      
   OutputCharValueToAC(ac, VAR_Username, pppConn->username);      
   OutputCharValueToAC(ac, VAR_ExternalIPAddress, pppConn->externalIPAddress);
   OutputCharValueToAC(ac, VAR_RemoteIPAddress, pppConn->remoteIPAddress);  
   OutputCharValueToAC(ac, VAR_DNSServers, pppConn->DNSServers);      
   OutputCharValueToAC(ac, VAR_PPPoEServiceName, pppConn->PPPoEServiceName);      

   cmsObj_free((void **) &pppConn);
   return TRUE;
}


int WANPPPConnection_GetConnectionTypeInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject  *pppConn = NULL;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);


   if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputCharValueToAC(ac, VAR_ConnectionType, pppConn->connectionType);          

   cmsObj_free((void **) &pppConn);
   return TRUE;
}

int WANPPPConnection_GetIdleDisconnectTime(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject  *pppConn = NULL;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);


   if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputNumValueToAC(ac, VAR_IdleDisconnectTime, pppConn->idleDisconnectTime);      
  
   cmsObj_free((void **) &pppConn);
   return TRUE;
}

int WANPPPConnection_GetStatusInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject  *pppConn = NULL;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);


   if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   
   OutputCharValueToAC(ac, VAR_ConnectionStatus, pppConn->connectionStatus);  

   cmsObj_free((void **) &pppConn);
   return TRUE;
}


int WANPPPConnection_GetUsername(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject  *pppConn = NULL;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);


   if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   
   OutputCharValueToAC(ac, VAR_Username, pppConn->username);        

   cmsObj_free((void **) &pppConn);
   return TRUE;
}

/* Remove since not defined in TR064.pdf
int WANPPPConnection_GetStatisticsWANPPP(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnStatsObject *pppConnStats = NULL; 
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);


   if (cmsObj_get(MDMOID_WAN_PPP_CONN_STATS, &iidStack, 0, (void **)&pppConnStats) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
  
   OutputNumValueToAC(ac, VAR_BytesSent, pppConnStats->ethernetBytesSent);        
   OutputNumValueToAC(ac, VAR_BytesReceived, pppConnStats->ethernetBytesReceived);        
   OutputNumValueToAC(ac, VAR_PacketsSent, pppConnStats->ethernetPacketsSent);        
   OutputNumValueToAC(ac, VAR_PacketsReceived, pppConnStats->ethernetPacketsReceived);        

   cmsObj_free((void **) &pppConnStats);
   return TRUE;
}
*/

int ForceTermination(UFILE *uclient, PService psvc, PAction ac,
                     pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   WanPppConnObject  *pppConn = NULL;
   UINT32 specificEid;
   UINT32 pppdPid=0;
   CmsRet ret;
   
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

   if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) == CMSRET_SUCCESS)
   {
       pppdPid = pppConn->X_BROADCOM_COM_PppdPid;
       cmsObj_free((void **) &pppConn);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }        

   if(pppdPid!=0)
   {
      specificEid = MAKE_SPECIFIC_EID(pppdPid, EID_PPP); 
      if ((ret = rut_sendMsgToSmd(CMS_MSG_STOP_APP, specificEid, NULL, 0)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to stop pppd");
      }
      else
      {
         cmsLog_debug("pppd stopped");
      }
   }

   return TRUE;
}

int RequestTermination(UFILE *uclient, PService psvc, PAction ac,
                     pvar_entry_t args, int nargs)
{
   return ForceTermination(uclient, psvc, ac, args, nargs);
}

int RequestConnection(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   WanPppConnObject  *pppConn = NULL;
   UINT32 specificEid;
   UINT32 pppdPid=0;
   
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

   if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) == CMSRET_SUCCESS)
   {
       pppdPid = pppConn->X_BROADCOM_COM_PppdPid;
       cmsObj_free((void **) &pppConn);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }        

   if(pppdPid!=0)
   {
      specificEid = MAKE_SPECIFIC_EID(pppdPid, EID_PPP); 
      SendMessageToChangePPPStateToUnconfigMode(specificEid);
   }

   return TRUE;
}

int SetConnectionType(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   WanPppConnObject  *pppConn = NULL;

   struct Param *pParams;
   
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

   pParams = findActionParamByRelatedVar(ac,VAR_ConnectionType);
   if (pParams != NULL)
   {
      if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) == CMSRET_SUCCESS)
      {
         cmsMem_free(pppConn->connectionType);
         pppConn->connectionType= cmsMem_strdup(pParams->value);           
         cmsObj_set(pppConn, &iidStack);
         cmsObj_free((void **) &pppConn);
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

   StopPPPDaemon(instanceOfWANDevice, instanceOfWANConnectionDevice, instanceOfWANPPPConnection);
   StartPPPDaemon(instanceOfWANDevice,instanceOfWANConnectionDevice, instanceOfWANPPPConnection);

   return TRUE;
}

int SetWANPPPConnEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject  *pppConn = NULL;

   struct Param *pParams;
   
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

   pParams = findActionParamByRelatedVar(ac,VAR_Enable);
   if (pParams != NULL)
   {
      if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) == CMSRET_SUCCESS)
      {
         pppConn->enable = atoi(pParams->value);
         cmsObj_set(pppConn, &iidStack);
         cmsObj_free((void **) &pppConn);
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

   StopPPPDaemon(instanceOfWANDevice, instanceOfWANConnectionDevice, instanceOfWANPPPConnection);
   StartPPPDaemon(instanceOfWANDevice,instanceOfWANConnectionDevice, instanceOfWANPPPConnection);

   return TRUE;

}

int SetUsername(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject  *pppConn = NULL;

   struct Param *pParams;
   
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

   pParams = findActionParamByRelatedVar(ac,VAR_Username);
   if (pParams != NULL)
   {
      if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) == CMSRET_SUCCESS)
      {
         cmsMem_free(pppConn->username);
         pppConn->username = cmsMem_strdup(pParams->value);
         cmsObj_set(pppConn, &iidStack);
         cmsObj_free((void **) &pppConn);
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

   StopPPPDaemon(instanceOfWANDevice, instanceOfWANConnectionDevice, instanceOfWANPPPConnection);
   StartPPPDaemon(instanceOfWANDevice,instanceOfWANConnectionDevice, instanceOfWANPPPConnection);

   return TRUE;

}

int SetPassword(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject  *pppConn = NULL;
   struct Param *pParams;
   
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

   pParams = findActionParamByRelatedVar(ac,VAR_Password);
   if (pParams != NULL)
   {
      if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) == CMSRET_SUCCESS)
      {
         cmsMem_free(pppConn->password);
         pppConn->password= cmsMem_strdup(pParams->value);
         cmsObj_set(pppConn, &iidStack);
         cmsObj_free((void **) &pppConn);
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

   StopPPPDaemon(instanceOfWANDevice, instanceOfWANConnectionDevice, instanceOfWANPPPConnection);
   StartPPPDaemon(instanceOfWANDevice,instanceOfWANConnectionDevice, instanceOfWANPPPConnection);

   return TRUE;

}

int SetPPPoEService(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject  *pppConn = NULL;

   struct Param *pParams;
   
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

   pParams = findActionParamByRelatedVar(ac,VAR_PPPoEServiceName);
   if (pParams != NULL)
   {
      if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) == CMSRET_SUCCESS)
      {
         cmsMem_free(pppConn->PPPoEServiceName);
         pppConn->PPPoEServiceName = cmsMem_strdup(pParams->value);
         cmsObj_set(pppConn, &iidStack);
         cmsObj_free((void **) &pppConn);
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

   StopPPPDaemon(instanceOfWANDevice, instanceOfWANConnectionDevice, instanceOfWANPPPConnection);
   StartPPPDaemon(instanceOfWANDevice,instanceOfWANConnectionDevice, instanceOfWANPPPConnection);

   return TRUE;

}

int SetConnectionTrigger(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject  *pppConn = NULL;

   struct Param *pParams;
   
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

   pParams = findActionParamByRelatedVar(ac,VAR_ConnectionTrigger);
   if (pParams != NULL)
   {
      if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) == CMSRET_SUCCESS)
      {
         cmsMem_free(pppConn->connectionTrigger);
         pppConn->connectionTrigger = cmsMem_strdup(pParams->value);
         cmsObj_set(pppConn, &iidStack);
         cmsObj_free((void **) &pppConn);
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

   StopPPPDaemon(instanceOfWANDevice, instanceOfWANConnectionDevice, instanceOfWANPPPConnection);
   StartPPPDaemon(instanceOfWANDevice,instanceOfWANConnectionDevice, instanceOfWANPPPConnection);

   return TRUE;
}

int SetIdleDisconnectTime(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject  *pppConn = NULL;

   struct Param *pParams;
   
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

   pParams = findActionParamByRelatedVar(ac,VAR_IdleDisconnectTime);
   if (pParams != NULL)
   {
      if (cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn) == CMSRET_SUCCESS)
      {
         pppConn->idleDisconnectTime = atoi(pParams->value);
         cmsObj_set(pppConn, &iidStack);
         cmsObj_free((void **) &pppConn);
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

   StopPPPDaemon(instanceOfWANDevice, instanceOfWANConnectionDevice, instanceOfWANPPPConnection);
   StartPPPDaemon(instanceOfWANDevice,instanceOfWANConnectionDevice, instanceOfWANPPPConnection);

   return TRUE;
}

int ppp_GetPortMappingNumberOfEntries(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack sonIidStack = EMPTY_INSTANCE_ID_STACK;

   WanPppConnPortmappingObject *port_mapping = NULL;

   int  cnt = 0;
   int  errorinfo = 0;
   

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

   INIT_INSTANCE_ID_STACK(&sonIidStack);
   while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN_PORTMAPPING, &iidStack, &sonIidStack,(void **)&port_mapping) == CMSRET_SUCCESS)
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

int ppp_GetGenericPortMappingEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack sonIidStack = EMPTY_INSTANCE_ID_STACK;

   WanPppConnPortmappingObject *port_mapping = NULL;

   int  PortMappingIndex =0;
   int  errorinfo = 0;
   int  found = 0;

   struct Param *pParams;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

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
   while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN_PORTMAPPING, &iidStack, &sonIidStack,(void **)&port_mapping) == CMSRET_SUCCESS)
   {
      if(sonIidStack.instance[sonIidStack.currentDepth-1] == PortMappingIndex)
      {
         found = 1;
         if(port_mapping->portMappingEnabled)
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_PortMappingEnabled, "1");
         }
         else
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_PortMappingEnabled, "0");
         }
         errorinfo |= OutputNumValueToAC(ac, VAR_ExternalPort, port_mapping->externalPort);
         errorinfo |= OutputNumValueToAC(ac, VAR_InternalPort, port_mapping->internalPort);
         errorinfo |= OutputCharValueToAC(ac, VAR_InternalClient, port_mapping->internalClient);
         errorinfo |= OutputCharValueToAC(ac, VAR_PortMappingProtocol, port_mapping->portMappingProtocol);
         errorinfo |= OutputCharValueToAC(ac, VAR_PortMappingDescription, port_mapping->portMappingDescription);
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


int ppp_GetSpecificPortMappingEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack sonIidStack = EMPTY_INSTANCE_ID_STACK;

   WanPppConnPortmappingObject *port_mapping = NULL;

   int  errorinfo = 0;
   int  found = 0;

   struct Param *pParams;

   uint32 ExternalPort;
   char PortMappingProtocol[32];

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

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

   while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN_PORTMAPPING, &iidStack, &sonIidStack,(void **)&port_mapping) == CMSRET_SUCCESS)
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

int ppp_AddPortMappingEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack sonIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   WanPppConnPortmappingObject *port_mapping = NULL;

   int  errorinfo = 0;

   struct Param *pParams;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

   sonIidStack = iidStack;
   if ((ret = cmsObj_addInstance(MDMOID_WAN_PPP_CONN_PORTMAPPING, &sonIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new virtual server entry, ret=%d", ret);
      soap_error( uclient, errorinfo );
      return FALSE;                 
   }

   if ((ret = cmsObj_get(MDMOID_WAN_PPP_CONN_PORTMAPPING, &sonIidStack, 0, (void **) &port_mapping)) != CMSRET_SUCCESS)
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
      errorinfo = SOAP_ACTIONFAILED;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_InternalPort);
   if (pParams != NULL)
   {
      port_mapping->internalPort= atoi(pParams->value);
      port_mapping->X_BROADCOM_COM_InternalPortEnd = atoi(pParams->value);
   }
   else
   {
      errorinfo = SOAP_ACTIONFAILED;
   }       

   pParams = findActionParamByRelatedVar(ac,VAR_PortMappingProtocol);
   if (pParams != NULL)
   {
      cmsMem_free(port_mapping->portMappingProtocol);
      port_mapping->portMappingProtocol = cmsMem_strdup(pParams->value);
   }
   else
   {
      errorinfo = SOAP_ACTIONFAILED;
   }            
   pParams = findActionParamByRelatedVar(ac,VAR_InternalClient);
   if (pParams != NULL)
   {
      cmsMem_free(port_mapping->internalClient);
      port_mapping->internalClient= cmsMem_strdup(pParams->value);
   }
   else
   {
      errorinfo = SOAP_ACTIONFAILED;
   }   

   pParams = findActionParamByRelatedVar(ac,VAR_PortMappingEnabled);
   if (pParams != NULL)
   {
      port_mapping->portMappingEnabled = atoi(pParams->value);
   }
   else
   {
      errorinfo = SOAP_ACTIONFAILED;
   }
   pParams = findActionParamByRelatedVar(ac,VAR_PortMappingDescription);
   if (pParams != NULL)
   {
      cmsMem_free(port_mapping->portMappingDescription);
      port_mapping->portMappingDescription= cmsMem_strdup(pParams->value);
   }
   else
   {
      errorinfo = SOAP_ACTIONFAILED;
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

int ppp_DeletePortMappingEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack sonIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   WanPppConnPortmappingObject *port_mapping = NULL;

   struct Param *pParams;

   int  ExternalPort;
   char PortMappingProtocol[32];
   
   int found = 0;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   int instanceOfWANPPPConnection;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;
   instanceOfWANPPPConnection = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANPPPConnection);

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

   while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN_PORTMAPPING, &iidStack, &sonIidStack,(void **)&port_mapping) == CMSRET_SUCCESS)
   {
      if((ExternalPort == port_mapping->externalPort) 
          &&(0==strcmp(PortMappingProtocol, port_mapping->portMappingProtocol)))
      {
         found = 1;
         ret = cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN_PORTMAPPING, &sonIidStack);
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

