/*
 * Copyright (c) 2003-2012  Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
 * 
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 * 
 *  Except as expressly set forth in the Authorized License,
 * 
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 * 
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 * 
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
 * :>
 *
 * $Id: wanipc.c,v 1.33.2.2 2003/10/31 21:31:35 mthawani Exp $
 */

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "wanipc.h"
#include "pcpiwf.h"

#include "cms_log.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_util.h"
#include "mdm_object.h"


/*
  Code to support the WAN IPConnection service.
*/


extern struct Param *findActionParamByName(PAction ac, char *varName);

extern void igd_restart_helper();
extern void igd_restart(int secs);

static int ipGetGenericPortMappingEntry(UFILE *, PService , PAction, pvar_entry_t , int );
static int ipGetSpecificPortMappingEntry(UFILE *, PService , PAction, pvar_entry_t , int);
static int ipAddPortMapping(UFILE *, PService, PAction, pvar_entry_t, int);
static int ipDeletePortMapping(UFILE *, PService, PAction, pvar_entry_t, int);
static int ipSetConnectionType(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);

//static void ipDelete_AllPortMappings(PService psvc);

extern int igd_config_generation;
extern char * safe_snprintf(char *str, int *len, const char *fmt, ...);

static int WANIPConnectionInit(struct Service *psvc, service_state_t state);
static int WANIPConnection_GetVar(struct Service *psvc, int varindex);

#define ipGetConnectionTypeInfo		DefaultAction
#define ipGetStatusInfo			DefaultAction
#define ipGetNATRSIPStatus		DefaultAction
#define ipGetExternalIPAddress		DefaultAction
#define ipGetIdleDisconnectTime		DefaultAction
#define ipGetWarnDisconnectDelay		DefaultAction
#define ipGetAutoDisconnectTime		DefaultAction

#define ipForceTermination NotImplemented
#define ipRequestConnection NotImplemented

static char *PossibleConnectionTypes_allowedValueList[] = { "Unconfigured",
                                                            "IP_Routed",
                                                            "IP_Bridged",
                                                            NULL };

static char *ConnectionStatus_allowedValueList[] = { "Unconfigured",
                                                     "Connecting",
                                                     "Authenticating",
                                                     "Connected",
                                                     "PendingDisconnect",
                                                     "Disconnecting",
                                                     "Disconnected",
                                                     NULL };
static char *LastConnectionError_allowedValueList[] = { "ERROR_NONE", "ERROR_UNKNOWN", NULL };
static char *PortMappingProtocol_allowedValueList[] = { "TCP", "UDP", NULL };

static VarTemplate StateVariables[] = {
    { "ConnectionType", "IP_Routed", VAR_STRING|VAR_LIST,
      (allowedValue) { PossibleConnectionTypes_allowedValueList }  },
    { "PossibleConnectionTypes", "", VAR_EVENTED|VAR_STRING|VAR_LIST,
      (allowedValue) { PossibleConnectionTypes_allowedValueList } },
    { "ConnectionStatus", "", VAR_EVENTED|VAR_STRING|VAR_LIST,
      (allowedValue) { ConnectionStatus_allowedValueList } },
    { "Uptime", "", VAR_ULONG },
    { "LastConnectionError", "", VAR_STRING|VAR_LIST,
      (allowedValue) { LastConnectionError_allowedValueList } },
    { "RSIPAvailable", "1", VAR_BOOL },
    { "NATEnabled", "1", VAR_BOOL },
    { "ExternalIPAddress", "", VAR_EVENTED|VAR_STRING },
    { "PortMappingNumberOfEntries", "", VAR_EVENTED|VAR_USHORT },
    { "PortMappingEnabled", "", VAR_BOOL },
    { "PortMappingLeaseDuration", "", VAR_ULONG },
    { "RemoteHost", "", VAR_STRING },
    { "ExternalPort", "", VAR_USHORT },
    { "InternalPort", "", VAR_USHORT },
    { "PortMappingProtocol", "", VAR_STRING|VAR_LIST,
      (allowedValue) { PortMappingProtocol_allowedValueList } },
    { "InternalClient", "", VAR_STRING },
    { "PortMappingDescription", "", VAR_STRING },
    { NULL }
};

static Action _SetConnectionType = {
    "SetConnectionType", ipSetConnectionType,
   (Param []) {
       {"NewConnectionType", VAR_ConnectionType, VAR_IN},
       { 0 }
    }
};

static Action _GetConnectionTypeInfo = {
    "GetConnectionTypeInfo", ipGetConnectionTypeInfo,
   (Param []) {
       {"NewConnectionType", VAR_ConnectionType, VAR_OUT},
       {"NewPossibleConnectionTypes", VAR_PossibleConnectionTypes, VAR_OUT},
       { 0 }
    }
};

static Action _RequestConnection = {
    "RequestConnection", ipRequestConnection,
   (Param []) {
       { 0 }
    }
};

static Action _ForceTermination = {
    "ForceTermination", ipForceTermination,
   (Param []) {
       { 0 }
    }
};

static Action _GetStatusInfo = {
    "GetStatusInfo", ipGetStatusInfo,
   (Param []) {
       {"NewConnectionStatus", VAR_ConnectionStatus, VAR_OUT},
       {"NewLastConnectionError", VAR_LastConnectionError, VAR_OUT},
       {"NewUptime", VAR_Uptime, VAR_OUT},
       { 0 }
    }
};

static Action _GetNATRSIPStatus = {
    "GetNATRSIPStatus", ipGetNATRSIPStatus,
   (Param []) {
       {"NewRSIPAvailable", VAR_RSIPAvailable, VAR_OUT},
       {"NewNATEnabled", VAR_NATEnabled, VAR_OUT},
       { 0 }
    }
};

static Action _GetGenericPortMappingEntry = {
    "GetGenericPortMappingEntry", ipGetGenericPortMappingEntry,
   (Param []) {
       {"NewPortMappingIndex", VAR_PortMappingNumberOfEntries, VAR_IN},
       {"NewRemoteHost", VAR_RemoteHost, VAR_OUT},
       {"NewExternalPort", VAR_ExternalPort, VAR_OUT},
       {"NewProtocol", VAR_PortMappingProtocol, VAR_OUT},
       {"NewInternalPort", VAR_InternalPort, VAR_OUT},
       {"NewInternalClient", VAR_InternalClient, VAR_OUT},
       {"NewEnabled", VAR_PortMappingEnabled, VAR_OUT},
       {"NewPortMappingDescription", VAR_PortMappingDescription, VAR_OUT},
       {"NewLeaseDuration", VAR_PortMappingLeaseDuration, VAR_OUT},
       { 0 }
    }
};

static Action _GetSpecificPortMappingEntry = {
    "GetSpecificPortMappingEntry", ipGetSpecificPortMappingEntry,
   (Param []) {
       {"NewRemoteHost", VAR_RemoteHost, VAR_IN},
       {"NewExternalPort", VAR_ExternalPort, VAR_IN},
       {"NewProtocol", VAR_PortMappingProtocol, VAR_IN},
       {"NewInternalPort", VAR_InternalPort, VAR_OUT},
       {"NewInternalClient", VAR_InternalClient, VAR_OUT},
       {"NewEnabled", VAR_PortMappingEnabled, VAR_OUT},
       {"NewPortMappingDescription", VAR_PortMappingDescription, VAR_OUT},
       {"NewLeaseDuration", VAR_PortMappingLeaseDuration, VAR_OUT},
       { 0 }
    }
};

static Action _AddPortMapping = {
    "AddPortMapping", ipAddPortMapping,
   (Param []) {
       {"NewRemoteHost", VAR_RemoteHost, VAR_IN},
       {"NewExternalPort", VAR_ExternalPort, VAR_IN},
       {"NewProtocol", VAR_PortMappingProtocol, VAR_IN},
       {"NewInternalPort", VAR_InternalPort, VAR_IN},
       {"NewInternalClient", VAR_InternalClient, VAR_IN},
       {"NewEnabled", VAR_PortMappingEnabled, VAR_IN},
       {"NewPortMappingDescription", VAR_PortMappingDescription, VAR_IN},
       {"NewLeaseDuration", VAR_PortMappingLeaseDuration, VAR_IN},
       { 0 }
    }
};

static Action _DeletePortMapping = {
    "DeletePortMapping", ipDeletePortMapping,
   (Param []) {
       {"NewRemoteHost", VAR_RemoteHost, VAR_IN},
       {"NewExternalPort", VAR_ExternalPort, VAR_IN},
       {"NewProtocol", VAR_PortMappingProtocol, VAR_IN},
       { 0 }
    }
};

static Action _GetExternalIPAddress = {
    "GetExternalIPAddress", ipGetExternalIPAddress,
   (Param []) {
       {"NewExternalIPAddress", VAR_ExternalIPAddress, VAR_OUT},
       { 0 }
    }
};



static PAction Actions[] = {
    &_SetConnectionType,
    &_GetConnectionTypeInfo,
    &_RequestConnection,
    &_ForceTermination,
    &_GetStatusInfo,
    &_GetNATRSIPStatus,
    &_GetGenericPortMappingEntry,
    &_GetSpecificPortMappingEntry,
    &_AddPortMapping,
    &_DeletePortMapping,
    &_GetExternalIPAddress,
    NULL
};

static int WANIPConnectionInit(struct Service *psvc, service_state_t state);

ServiceTemplate Template_WANIPConnection = {
    "WANIPConnection:1",
    WANIPConnectionInit,	/* service initialization */
    WANIPConnection_GetVar,/* state variable handler */
    NULL,			/* xml generator */
    ARRAYSIZE(StateVariables)-1, StateVariables,
    Actions, 0,
    "urn:upnp-org:serviceId:WANIPConnection."
};

static int WANIPConnectionInit(struct Service *psvc, service_state_t state)
{
//    struct  itimerspec  timer;
    PWANIPConnectionData pdata;
    // PWANDevicePrivateData pdevdata;
    char *wanif;

    /*
     * If the WAN interface is PPP then this should not be initialized.
     * I am a little paranoid so I'll check and return.
     */
    wanif = g_wan_ifname;
    if ( strncasecmp(wanif, "ppp", 3) == 0 )  {
        return 0;
    }
    switch (state) {
    case SERVICE_CREATE:
	// pdevdata = (PWANDevicePrivateData) psvc->device->parent->opaque;
	pdata = (PWANIPConnectionData) cmsMem_alloc(sizeof(WANIPConnectionData), ALLOC_ZEROIZE);
	if (pdata) {
	    pdata->connection_status = IP_CONNECTING;
	    pdata->connected_time = time(NULL);
	    pdata->igd_generation = igd_config_generation;

	    psvc->opaque = (void *) pdata;

	    /*initialize state variables here */	
	     WANConnection_UpdateVars(psvc, FALSE);

         /*Suresh:TODO resgister with ssk to get notification about
          *  eveneted variables of this service
          *  this is not needed for now as we restart upnp if any of evenetd
          *  variables are changed(just a co-incidence) 
          */
	}
	break;

    case SERVICE_DESTROY:
      /*suresh TODO: delete all portmappings of this service instance*/
	/*delete all portmappings */
        //ipDelete_AllPortMappings(psvc);

	pdata = (PWANIPConnectionData) psvc->opaque;
	cmsMem_free(pdata);
	break;
    } /* end switch */
    return 0;
}


static int WANIPConnection_GetVar(struct Service *psvc, int varindex)
{
   struct StateVar *var;
   static time_t then;
   time_t now;
   CmsRet ret;

   var = &(psvc->vars[varindex]);

   time(&now);/*to avoid too frequent queries to cms*/
   if (now != then) 
   {

      if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get lock, ret=%d", ret);
         return FALSE;
      } 	

      WANConnection_UpdateVars(psvc, FALSE);

      cmsLck_releaseLock();

      then = now;
/*    if ((psvc->flags & VAR_CHANGED) == VAR_CHANGED) 
	update_all_subscriptions(psvc);*/
   }

   return TRUE;
}


static int ipSetConnectionType(UFILE *uclient, PService psvc, PAction ac,
                             pvar_entry_t args, int nargs)
{
   struct Param *pParams;
   int errorInfo=0;


   pParams = findActionParamByName(ac,"NewConnectionType");
   if (pParams == NULL)
   {
      soap_error( uclient, SOAP_INVALID_ARGS);
      return FALSE;
   }


   /* Call separate function to do CMS MDM operation */
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

      PUSH_INSTANCE_ID(&iidStack, psvc->device->parent->instance);/*instanceOfWANDevice */
      PUSH_INSTANCE_ID(&iidStack, psvc->device->instance);/*instanceOfWANConnectionDevice */
      PUSH_INSTANCE_ID(&iidStack,psvc->instance );/*instanceOfWANIPConnection */

      errorInfo = upnp_setConnectionType(FALSE, &iidStack, pParams->value);
   }

   /*update state variable */
   {
      int strLen = 0;
      strLen = sizeof(psvc->vars[VAR_ConnectionType].value) - 1;
      strncpy(psvc->vars[VAR_ConnectionType].value, ac->params[0].value, strLen);
      psvc->vars[VAR_ConnectionType].value[strLen] = '\0';
   }
   
   if (errorInfo)
   {
      soap_error( uclient, errorInfo);
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

static int ipAddPortMapping( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
/* {"NewRemoteHost", VAR_RemoteHost, VAR_IN},				*/
/* {"NewExternalPort", VAR_ExternalPort, VAR_IN},			*/
/* {"NewProtocol", VAR_PortMappingProtocol, VAR_IN},			*/
/* {"NewInternalPort", VAR_InternalPort, VAR_IN},			*/
/* {"NewInternalClient", VAR_InternalClient, VAR_IN},			*/
/* {"NewEnabled", VAR_PortMappingEnabled, VAR_IN},			*/
/* {"NewPortMappingDescription", VAR_PortMappingDescription, VAR_IN},	*/
/* {"NewLeaseDuration", VAR_PortMappingLeaseDuration, VAR_IN},		*/
{

   UBOOL8    portMappingEnabled = FALSE;
   UINT32    leaseDuration = 0;
   char *    remoteHost = NULL;	
   UINT16    externalPort = 0;
   UINT16    internalPort = 0;	
   char *    protocol = NULL;
   char *    internalClient = NULL;	
   char *    description = NULL;

   struct Param *pParams;
   int errorInfo = 0;


   /* parse the action parameters */   

   pParams = findActionParamByName(ac,"NewRemoteHost");
   if(pParams!=NULL){
      remoteHost = pParams->value; 
   } else{
      errorInfo = SOAP_INVALID_ARGS; 
   }  

   pParams = findActionParamByName(ac,"NewExternalPort");
   if(pParams!=NULL){
      externalPort = (atoi(pParams->value)); 
   } else{
      errorInfo = SOAP_INVALID_ARGS; 
   }  

   pParams = findActionParamByName(ac,"NewProtocol");
   if(pParams!=NULL){
      protocol = pParams->value;
   } else{
      errorInfo = SOAP_INVALID_ARGS; 
   }  

   pParams = findActionParamByName(ac,"NewInternalClient");
   if(pParams!=NULL){
      internalClient = pParams->value; 
   } else{
      errorInfo = SOAP_INVALID_ARGS; 
   }  

   pParams = findActionParamByName(ac,"NewInternalPort");
   if(pParams!=NULL){
      internalPort = (atoi(pParams->value)); 
   } else{
      errorInfo = SOAP_INVALID_ARGS; 
   }  

   pParams = findActionParamByName(ac,"NewEnabled");
   if(pParams!=NULL){
      portMappingEnabled= atoi(pParams->value); 
   } else{
      errorInfo = SOAP_INVALID_ARGS; 
   }  

   pParams = findActionParamByName(ac,"NewPortMappingDescription");
   if(pParams!=NULL){
      description = pParams->value;
   } else{
      errorInfo = SOAP_INVALID_ARGS; 
   }  

   pParams = findActionParamByName(ac,"NewLeaseDuration");
   if(pParams!=NULL){
      leaseDuration= (atoi(pParams->value));
   } else{
      errorInfo = SOAP_INVALID_ARGS; 
   }  

   if(errorInfo){
      soap_error( uclient, errorInfo);
      return FALSE;
   }

   /* Call separate function to do CMS MDM operation */
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

      PUSH_INSTANCE_ID(&iidStack, psvc->device->parent->instance);/*instanceOfWANDevice */
      PUSH_INSTANCE_ID(&iidStack, psvc->device->instance);/*instanceOfWANConnectionDevice */
      PUSH_INSTANCE_ID(&iidStack,psvc->instance );/*instanceOfWANIPConnection */

      errorInfo = upnp_updateOrAddPortMapping(FALSE, &iidStack,
                                 portMappingEnabled, leaseDuration,
                                 protocol, externalPort, internalPort,
                                 remoteHost, internalClient, description);
   }

      /*suresh TODO: update local state variables */

   if (errorInfo)
   {
      soap_error( uclient, errorInfo);
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

static int ipDeletePortMapping( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
/*  {"NewRemoteHost", VAR_RemoteHost, VAR_IN},		*/
/*  {"NewExternalPort", VAR_ExternalPort, VAR_IN},	*/
/*  {"NewProtocol", VAR_PortMappingProtocol, VAR_IN},	*/
{

   char *    remoteHost = NULL;	
   UINT16    externalPort = 0;
   char *    protocol = NULL;
   struct Param *pParams;
   char iaddr[CMS_IPADDR_LENGTH]={0};
   int errorInfo = 0;


   /* parse the action parameters */   

   pParams = findActionParamByName(ac,"NewRemoteHost");
   if(pParams!=NULL){
      remoteHost = pParams->value; 
   } else{
      errorInfo = SOAP_INVALID_ARGS; 
   }  

   pParams = findActionParamByName(ac,"NewExternalPort");
   if(pParams!=NULL){
      externalPort = (atoi(pParams->value)); 
   } else{
      errorInfo = SOAP_INVALID_ARGS; 
   }  

   pParams = findActionParamByName(ac,"NewProtocol");
   if(pParams!=NULL){
      protocol = pParams->value;
   } else{
      errorInfo = SOAP_INVALID_ARGS; 
   }  

   if(errorInfo){
      soap_error( uclient, errorInfo);
      return FALSE;
   }


   /* Call separate function to do CMS MDM operation */
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

      PUSH_INSTANCE_ID(&iidStack, psvc->device->parent->instance);/*instanceOfWANDevice */
      PUSH_INSTANCE_ID(&iidStack, psvc->device->instance);/*instanceOfWANConnectionDevice */
      PUSH_INSTANCE_ID(&iidStack,psvc->instance );/*instanceOfWANIPConnection */

      errorInfo = upnp_deletePortMapping(FALSE, &iidStack,
                                         protocol, externalPort, remoteHost,
                                         iaddr);
   }

   //TODO: PCP: should call pcp_invoke() only for successful case??
   if (mode != PCP_MODE_DISABLE)
   {
       pcp_invoke(uclient, psvc, ac, args, nargs, iaddr, 0);
   }

   if (errorInfo)
   {
      soap_error( uclient, errorInfo);
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

static int ipGetGenericPortMappingEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
/* {"NewPortMappingIndex", VAR_PortMappingNumberOfEntries, VAR_IN}, 	*/
/* {"NewRemoteHost", VAR_RemoteHost, VAR_OUT}, 				*/
/* {"NewExternalPort", VAR_ExternalPort, VAR_OUT}, 			*/
/* {"NewProtocol", VAR_PortMappingProtocol, VAR_OUT}, 			*/
/* {"NewInternalPort", VAR_InternalPort, VAR_OUT}, 			*/
/* {"NewInternalClient", VAR_InternalClient, VAR_OUT}, 			*/
/* {"NewEnabled", VAR_PortMappingEnabled, VAR_OUT}, 			*/
/* {"NewPortMappingDescription", VAR_PortMappingDescription, VAR_OUT},	*/
/* {"NewLeaseDuration", VAR_PortMappingLeaseDuration, VAR_OUT}, 	*/
{
   int  portMappingIndex =0;
   int  errorinfo = 0;
   struct Param *pParams;


   pParams = findActionParamByName(ac,"NewPortMappingIndex");
   if (pParams != NULL)
   {
      portMappingIndex = (atoi(pParams->value)); 
   }
   else
   {
      soap_error( uclient, SOAP_INVALID_ARGS);
      return FALSE;
   } 


   /* Call separate function to do CMS MDM operation */
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

      PUSH_INSTANCE_ID(&iidStack, psvc->device->parent->instance);/*instanceOfWANDevice */
      PUSH_INSTANCE_ID(&iidStack, psvc->device->instance);/*instanceOfWANConnectionDevice */
      PUSH_INSTANCE_ID(&iidStack,psvc->instance );/*instanceOfWANIPConnection */

      errorinfo = upnp_getGenericPortMapping(FALSE, &iidStack,
                                             portMappingIndex, ac);
   }

   if (errorinfo)
   {
      soap_error( uclient, errorinfo);
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}


static int ipGetSpecificPortMappingEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
/* {"NewRemoteHost", VAR_RemoteHost, VAR_IN},				*/
/* {"NewExternalPort", VAR_ExternalPort, VAR_IN},			*/
/* {"NewProtocol", VAR_PortMappingProtocol, VAR_IN},			*/
/* {"NewInternalPort", VAR_InternalPort, VAR_OUT},			*/
/* {"NewInternalClient", VAR_InternalClient, VAR_OUT},			*/
/* {"NewEnabled", VAR_PortMappingEnabled, VAR_OUT},			*/
/* {"NewPortMappingDescription", VAR_PortMappingDescription, VAR_OUT},	*/
/* {"NewLeaseDuration", VAR_PortMappingLeaseDuration, VAR_OUT},		*/
{

   int  errorinfo = 0;
   struct Param *pParams;
   char *    remoteHost = NULL;	
   UINT16    externalPort = 0;
   char *    protocol = NULL;


   /* parse the action parameters */   

   pParams = findActionParamByName(ac,"NewRemoteHost");
   if(pParams!=NULL){
      remoteHost = pParams->value; 
   } else{
      errorinfo = SOAP_INVALID_ARGS; 
   }  

   pParams = findActionParamByName(ac,"NewExternalPort");
   if(pParams!=NULL){
      externalPort = (atoi(pParams->value)); 
   } else{
      errorinfo = SOAP_INVALID_ARGS; 
   }  

   pParams = findActionParamByName(ac,"NewProtocol");
   if(pParams!=NULL){
      protocol = pParams->value;
   } else{
      errorinfo = SOAP_INVALID_ARGS; 
   }  

   if(errorinfo)
   {
      soap_error( uclient, errorinfo);
      return FALSE;
   } 


   /* Call separate function to do CMS MDM operation */
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

      PUSH_INSTANCE_ID(&iidStack, psvc->device->parent->instance);/*instanceOfWANDevice */
      PUSH_INSTANCE_ID(&iidStack, psvc->device->instance);/*instanceOfWANConnectionDevice */
      PUSH_INSTANCE_ID(&iidStack,psvc->instance );/*instanceOfWANIPConnection */

      errorinfo = upnp_getSpecificPortMapping(FALSE, &iidStack,
                                          protocol, externalPort, remoteHost,
                                          ac);
   }

   if (errorinfo)
   {
      soap_error( uclient, errorinfo);
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

/* this function deletes all portmappings added by upnp
 * for WANIPConnection, when upnp is exiting */
#if 0 /* not used */
static void ipDelete_AllPortMappings(PService psvc)
{

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack sonIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   WanIpConnPortmappingObject *port_mapping = NULL;

   PUSH_INSTANCE_ID(&iidStack, psvc->device->parent->instance);/*instanceOfWANDevice */
   PUSH_INSTANCE_ID(&iidStack, psvc->device->instance);/*instanceOfWANConnectionDevice */
   PUSH_INSTANCE_ID(&iidStack,psvc->instance );/*instanceOfWANIPConnection */

      if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
      {
   while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN_PORTMAPPING, &iidStack, &sonIidStack,(void **)&port_mapping) == CMSRET_SUCCESS)
   {
	/*checkif the entry is added by upnp */
      if(0 == cmsUtl_strcmp(port_mapping->X_BROADCOM_COM_AppName,g_upnpAppName))
      {
         if((ret = cmsObj_deleteInstance(MDMOID_WAN_IP_CONN_PORTMAPPING, &sonIidStack)) ==CMSRET_SUCCESS){
      		cmsObj_free((void **) &port_mapping);
	} else {
      		cmsLog_error("could not delete virtual server entry, ret=%d", ret);
      		cmsObj_free((void **) &port_mapping);
	}
	 		
      }  
      cmsObj_free((void **) &port_mapping);
   }                    

   cmsLck_releaseLock();
   } else{
         cmsLog_error("failed to get lock, ret=%d", ret);
   }                   

   		//cmsMgm_saveConfigToFlash();
}
#endif
