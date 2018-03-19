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
//  Filename:        wanipconnectionparams.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "wanipconnection.h"

char *IPPossibleConnectionTypes_allowedValueList[] = { "Unconfigured",
                                                       "IP_Routed",
                                                       "IP_Bridged",
                                                       NULL };

char *IPConnectionStatus_allowedValueList[] = { "Unconfigured",
                                              "Connecting",
                                              "Authenticating",
                                              "Connected",
                                              "PendingDisconnect",
                                              "Disconnecting",
                                              "Disconnected",
                                              NULL };

char *IPConnectionTrigger_allowedValueList[] = { "OnDemand",
                                              "AlwaysOn",
                                              "Manual",
                                              NULL };

char *IPLastConnectionError_allowedValueList[] = { "ERROR_NONE", "ERROR_UNKNOWN", NULL };
char *IPPortMappingProtocol_allowedValueList[] = { "TCP", "UDP", NULL };
char *IPAddressingType_allowedValueList[] = { "DHCP", "Static", NULL };

static VarTemplate StateVariables[] = {
       { "Enable", "1", VAR_BOOL },
    { "ConnectionType", "IP_Routed", VAR_STRING|VAR_LIST,
        (allowedValue) { IPPossibleConnectionTypes_allowedValueList }  },      
    { "PossibleConnectionTypes", "", VAR_EVENTED|VAR_STRING|VAR_LIST,
        (allowedValue) { IPPossibleConnectionTypes_allowedValueList } },
    { "ConnectionStatus", "", VAR_EVENTED|VAR_STRING|VAR_LIST,
        (allowedValue) { IPConnectionStatus_allowedValueList } },        
    { "Name", "", VAR_STRING },
    { "Uptime", "", VAR_ULONG },
    { "NATEnabled", "1", VAR_BOOL },
    { "AddressingType", "", VAR_STRING|VAR_LIST, (allowedValue) { IPAddressingType_allowedValueList } },
    { "ExternalIPAddress", "", VAR_EVENTED|VAR_STRING },
    { "SubnetMask", "", VAR_STRING },
    { "DefaultGateway", "", VAR_STRING },    
    { "DNSEnabled", "", VAR_BOOL },
    { "DNSServers", "", VAR_STRING },
#if 0
       /* this parameter is not supported in cms lib core */
    { "ConnectionTrigger", "", VAR_STRING|VAR_LIST, (allowedValue) { IPConnectionTrigger_allowedValueList } },
#endif
    { "PortMappingNumberOfEntries", "", VAR_EVENTED|VAR_USHORT },
    { "PortMappingEnabled", "", VAR_BOOL },
    { "ExternalPort", "", VAR_USHORT },
    { "InternalPort", "", VAR_USHORT },
    { "PortMappingProtocol", "", VAR_STRING|VAR_LIST,
        (allowedValue) { IPPortMappingProtocol_allowedValueList } },
    { "InternalClient", "", VAR_STRING },
    { "PortMappingDescription", "", VAR_STRING },
/* Remove since not defined in TR064.pdf
    { "BytesSent", "", VAR_ULONG },
    { "BytesReceived", "", VAR_ULONG },
    { "PacketsSent", "", VAR_ULONG },
    { "PacketsReceived", "", VAR_ULONG }, */
    { NULL }
};

static Action _SetEnable = {
    "SetEnable", SetWANIPConnEnable,
    (Param []) {
       {"NewEnable", VAR_Enable, VAR_IN},
       { 0 }
    }
};
static Action _GetInfo = {
    "GetInfo", WANIPConnection_GetVar,
   (Param []) {
       {"NewEnable", VAR_Enable, VAR_OUT},   
       {"NewConnectionType", VAR_ConnectionType, VAR_OUT},
       {"NewPossibleConnectionTypes", VAR_PossibleConnectionTypes, VAR_OUT},
       {"NewConnectionStatus", VAR_ConnectionStatus, VAR_OUT},       
       {"NewName", VAR_Name, VAR_OUT},       
       {"NewUptime", VAR_Uptime, VAR_OUT},              
       {"NewNATEnabled", VAR_NATEnabled, VAR_OUT},       
       {"NewExternalIPAddress", VAR_ExternalIPAddress, VAR_OUT},  
       {"NewSubnetMask", VAR_SubnetMask, VAR_OUT},  
       {"NewDefaultGateway", VAR_DefaultGateway, VAR_OUT},  
  //     {"NewDNSEnabled", VAR_DNSEnabled, VAR_OUT},        
       {"NewDNSServers", VAR_DNSServers, VAR_OUT},   
#if 0
       /* this parameter is not supported */
       {"NewConnectionTrigger", VAR_ConnectionTrigger, VAR_OUT},   
#endif
       { 0 }
    }
};

static Action _GetConnectionTypeInfo = {
    "GetConnectionTypeInfo", WANIPConnection_GetConnectionTypeInfo,
   (Param []) {
       {"NewConnectionType", VAR_ConnectionType, VAR_OUT},
       {"NewPossibleConnectionTypes", VAR_PossibleConnectionTypes, VAR_OUT},
       { 0 }
    }
};

static Action _SetIPConnectionType = {
    "SetConnectionType", SetIPConnectionType,
    (Param []) {
       {"NewConnectionType", VAR_ConnectionType, VAR_IN},
       { 0 }
    }
};

static Action _RequestConnection = {
    "RequestConnection", RequestConnectionTR64,
   (Param []) {
       { 0 }
    }
};

static Action _RequestTermination = {
    "RequestTermination", RequestTerminationTR64,
   (Param []) {
       { 0 }
    }
};

static Action _ForceTermination = {
    "ForceTermination", ForceTerminationTR64,
   (Param []) {
       { 0 }
    }
};

static Action _GetStatusInfo = {
    "GetStatusInfo", WANIPConnection_GetStatusInfo,
   (Param []) {
       {"NewConnectionStatus", VAR_ConnectionStatus, VAR_OUT},
       {"NewUptime", VAR_Uptime, VAR_OUT},
       { 0 }
    }
};

static Action _GetExternalIPAddress = {
    "GetExternalIPAddress", WANIPConnection_GetExternalIPAddress,
   (Param []) {
       {"NewExternalIPAddress", VAR_ExternalIPAddress, VAR_OUT},
       { 0 }
    }
};

static Action _SetIPInterfaceInfo = {
    "SetIPInterfaceInfo", SetIPInterfaceInfo,
    (Param []) {
       {"NewAddressingType", VAR_AddressingType, VAR_IN},
       {"NewExternalIPAddress", VAR_ExternalIPAddress, VAR_IN},           
       {"NewSubnetMask", VAR_SubnetMask, VAR_IN},       
       {"NewDefaultGateway", VAR_DefaultGateway, VAR_IN},              
       { 0 }
    }
};

static Action _SetConnectionTrigger = {
    "SetConnectionTrigger", SetIPConnectionTrigger,
    (Param []) {
       {"NewConnectionTrigger", VAR_ConnectionTrigger, VAR_IN},          
       { 0 }
    }
};

static Action _GetPortMappingNumberOfEntries = {
    "GetPortMappingNumberOfEntries", GetPortMappingNumberOfEntries,
    (Param []) {
       {"NewPortMappingNumberOfEntries", VAR_PortMappingNumberOfEntries, VAR_OUT},          
       { 0 }
    }
};
              
static Action _GetGenericPortMappingEntry = {
    "GetGenericPortMappingEntry", GetGenericPortMappingEntry,
   (Param []) {
       {"NewPortMappingIndex", VAR_PortMappingNumberOfEntries, VAR_IN},
       {"NewExternalPort", VAR_ExternalPort, VAR_OUT},
       {"NewProtocol", VAR_PortMappingProtocol, VAR_OUT},
       {"NewInternalPort", VAR_InternalPort, VAR_OUT},
       {"NewInternalClient", VAR_InternalClient, VAR_OUT},
       {"NewEnabled", VAR_PortMappingEnabled, VAR_OUT},
       {"NewPortMappingDescription", VAR_PortMappingDescription, VAR_OUT},
       { 0 }
    }
};

static Action _GetSpecificPortMappingEntry = {
    "GetSpecificPortMappingEntry", GetSpecificPortMappingEntry,
   (Param []) {
       {"NewExternalPort", VAR_ExternalPort, VAR_IN},
       {"NewProtocol", VAR_PortMappingProtocol, VAR_IN},
       {"NewInternalPort", VAR_InternalPort, VAR_OUT},
       {"NewInternalClient", VAR_InternalClient, VAR_OUT},
       {"NewEnabled", VAR_PortMappingEnabled, VAR_OUT},
       {"NewPortMappingDescription", VAR_PortMappingDescription, VAR_OUT},
       { 0 }
    }
};

static Action _AddPortMapping = {
    "AddPortMapping", AddPortMappingEntry,
   (Param []) {
       {"NewExternalPort", VAR_ExternalPort, VAR_IN},
       {"NewProtocol", VAR_PortMappingProtocol, VAR_IN},
       {"NewInternalPort", VAR_InternalPort, VAR_IN},
       {"NewInternalClient", VAR_InternalClient, VAR_IN},
       {"NewEnabled", VAR_PortMappingEnabled, VAR_IN},
       {"NewPortMappingDescription", VAR_PortMappingDescription, VAR_IN},
       { 0 }
    }
};
static Action _DeletePortMapping = {
    "DeletePortMapping", DeletePortMappingEntry,
   (Param []) {
       {"NewExternalPort", VAR_ExternalPort, VAR_IN},
       {"NewProtocol", VAR_PortMappingProtocol, VAR_IN},
       { 0 }
    }
};

/* Remove since not defined in TR064.pdf
static Action _GetStatistics = {
    "GetStatistics", WANIPConnection_GetStatistics,
   (Param []) {
       {"NewBytesSent", VAR_BytesSent, VAR_OUT},
       {"NewBytesReceived", VAR_BytesReceived, VAR_OUT},
       {"NewPacketsSent", VAR_PacketsSent, VAR_OUT},
       {"NewPacketsReceived", VAR_PacketsReceived, VAR_OUT},
       { 0 }
    }
};
*/

static PAction Actions[] = {
   &_SetEnable,
   &_GetInfo,
   &_GetConnectionTypeInfo,
   &_GetStatusInfo,
   &_GetExternalIPAddress,
   &_RequestConnection,
   &_RequestTermination,
   &_ForceTermination,
   &_SetIPInterfaceInfo,
#if 0
   /* cms lib doesn't support this parameter */
   &_SetConnectionTrigger,
#endif
   &_SetIPConnectionType,
   &_GetPortMappingNumberOfEntries,
   &_GetGenericPortMappingEntry,
   &_GetSpecificPortMappingEntry,
   &_AddPortMapping,
   &_DeletePortMapping,
   /* &_GetStatistics, */
    NULL
};

ServiceTemplate Template_WANIPConnection = {
    "WANIPConnection:",
    NULL,    /* service initialization */
    NULL,    /* state variable handler */
    NULL,            /* xml generator */
    ARRAYSIZE(StateVariables)-1, 
    StateVariables,
    Actions, 0,
    "urn:dslforum-org:serviceId:WANIPConnection"
};

