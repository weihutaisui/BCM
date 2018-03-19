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
//  Filename:       lanparams.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "lanhostcfgmgtparams.h"

static VarTemplate StateVariables[] = {
    { "DHCPServerConfigurable", "1", VAR_EVENTED|VAR_BOOL},
    { "DHCPServerEnable", "1", VAR_EVENTED|VAR_BOOL},    
    { "DHCPRelay", "0", VAR_EVENTED|VAR_BOOL},            
    { "MinAddress",  "", VAR_EVENTED|VAR_STRING },
    { "MaxAddress", "", VAR_EVENTED|VAR_STRING },
    { "ReservedAddresses", "", VAR_EVENTED|VAR_STRING },
    { "SubnetMask", "", VAR_EVENTED|VAR_STRING },
    { "DNSServers", "", VAR_EVENTED|VAR_STRING },
    { "DomainName", "", VAR_EVENTED|VAR_STRING },
    { "IPRouters", "", VAR_EVENTED|VAR_STRING },    
    { "DHCPLeaseTime", "", VAR_ULONG },
    { "IPInterfaceNumberOfEntries", "", VAR_LONG },  
    { "Enable", "", VAR_BOOL },    
    { "IPInterfaceIPAddress", "", VAR_STRING },    
    { "IPInterfaceSubnetMask", "", VAR_STRING },    
    { "IPInterfaceAddressingType", "", VAR_STRING },    
    { NULL }
};

static Action _GetInfo = {
    "GetInfo", GetInfo,
   (Param []) {
        { "NewDHCPServerConfigurable", VAR_DHCPServerConfigurable, VAR_OUT},
    { "NewDHCPServerEnable", VAR_DHCPServerEnable, VAR_OUT},    
        { "NewDHCPRelay", VAR_DHCPRelay, VAR_OUT},            
    { "NewMinAddress",  VAR_MinAddress, VAR_OUT },
    { "NewMaxAddress", VAR_MaxAddress, VAR_OUT },
        { "NewReservedAddresses", VAR_ReservedAddresses, VAR_OUT },
    { "NewSubnetMask", VAR_SubnetMask, VAR_OUT },
        { "NewDNSServers", VAR_DNSServers, VAR_OUT },
        { "NewDomainName", VAR_DomainName, VAR_OUT },
        { "NewIPRouters", VAR_IPRouters, VAR_OUT },    
    { "NewDHCPLeaseTime", VAR_DHCPLeaseTime, VAR_OUT },
        { 0 }
    }
};

static Action _SetDHCPServerConfigurable = { 
    "SetDHCPServerConfigurable", SetDHCPServerConfigurable,
        (Param []) {
            {"NewDHCPServerConfigurable", VAR_DHCPServerConfigurable, VAR_IN},
            { 0 }
        }
};


static Action _GetDHCPServerConfigurable = { 
    "GetDHCPServerConfigurable", GetDHCPServerConfigurable,
        (Param []) {
            {"NewDHCPServerConfigurable", VAR_DHCPServerConfigurable, VAR_OUT},
            { 0 }
        }
};

static Action _GetDHCPRelay = { 
    "GetDHCPRelay", GetDHCPRelay,
        (Param []) {
            {"NewDHCPRelay", VAR_DHCPRelay, VAR_OUT},
            { 0 }
        }
};

static Action _SetSubnetMask = { 
    "SetSubnetMask", SetSubnetMask,
        (Param []) {
            {"NewSubnetMask", VAR_SubnetMask, VAR_IN},
            { 0 }
        }
};

static Action _GetSubnetMask = { 
    "GetSubnetMask", GetSubnetMask,
        (Param []) {
            {"NewSubnetMask", VAR_SubnetMask, VAR_OUT},
            { 0 }
        }
};

static Action _SetIPRouter = { 
    "SetIPRouter", SetIPRouter,
        (Param []) {
            {"NewIPRouters", VAR_IPRouters, VAR_IN},
            { 0 }
        }
};
/*
static Action _DeleteIPRouter = { 
    "DeleteIPRouter", DeleteIPRouter,
        (Param []) {
            {"NewIPRouters", VAR_IPRouters, VAR_IN},
            { 0 }
        }
};
*/
static Action _GetIPRoutersList = { 
    "GetIPRoutersList", GetIPRoutersList,
        (Param []) {
            {"NewIPRouters", VAR_IPRouters, VAR_OUT},
            { 0 }
        }
};

static Action _SetDomainName = { 
    "SetDomainName", SetDomainName,
        (Param []) {
            {"NewDomainName", VAR_DomainName, VAR_IN},
            { 0 }
        }
};

static Action _GetDomainName = { 
    "GetDomainName", GetDomainName,
        (Param []) {
            {"NewDomainName", VAR_DomainName, VAR_OUT},
            { 0 }
        }
};

static Action _SetAddressRange = { 
    "SetAddressRange", SetAddressRange,
        (Param []) {
            {"NewMinAddress", VAR_MinAddress, VAR_IN},
            {"NewMaxAddress", VAR_MaxAddress, VAR_IN},                
            { 0 }
        }
};

static Action _GetAddressRange = { 
    "GetAddressRange", GetAddressRange,
        (Param []) {
            {"NewMinAddress", VAR_MinAddress, VAR_OUT},
            {"NewMaxAddress", VAR_MaxAddress, VAR_OUT},    
            { 0 }
        }
};

/*
static Action _SetReservedAddress = { 
    "SetReservedAddress", SetReservedAddress,
        (Param []) {
            {"NewReservedAddresses", VAR_ReservedAddresses, VAR_IN},
            { 0 }
        }
};

static Action _DeleteReservedAddress = { 
    "DeleteReservedAddress", DeleteReservedAddress,
        (Param []) {
            {"NewReservedAddresses", VAR_ReservedAddresses, VAR_IN},
            { 0 }
        }
};
*/
static Action _SetDNSServer = { 
    "SetDNSServer", SetDNSServer,
        (Param []) {
            {"NewDNSServers", VAR_DNSServers, VAR_IN},
            { 0 }
        }
};

static Action _DeleteDNSServer = { 
    "DeleteDNSServer", DeleteDNSServer,
        (Param []) {
            {"NewDNSServers", VAR_DNSServers, VAR_IN},
            { 0 }
        }
};

static Action _GetDNSServers = { 
    "GetDNSServer", GetDNSServers,
        (Param []) {
            {"NewDNSServers", VAR_DNSServers, VAR_OUT},
            { 0 }
        }
};


static Action _SetDHCPLeaseTime = {
    "SetDHCPLeaseTime", SetDHCPLeaseTime,
   (Param []) {
       {"NewDHCPLeaseTime", VAR_DHCPLeaseTime, VAR_IN},
       { 0 }
    }
};

static Action _SetDHCPServerEnable = {
    "SetDHCPServerEnable", SetDHCPServerEnable,
   (Param []) {
       {"NewDHCPServerEnable", VAR_DHCPServerEnable, VAR_IN},
       { 0 }
    }
};

static Action _GetIPInterfaceNumberOfEntries = {
    "GetIPInterfaceNumberOfEntries", GetIPInterfaceNumberOfEntries,
   (Param []) {
       {"NewIPInterfaceNumberOfEntries", VAR_IPInterfaceNumberOfEntries, VAR_OUT},
       { 0 }
    }
};

static Action _SetIPInterface = {
    "SetIPInterface", SetIPInterface,
   (Param []) {
       {"NewEnable", VAR_Enable, VAR_IN},
       {"NewIPAddress", VAR_IPInterfaceIPAddress, VAR_IN},
       {"NewSubnetMask", VAR_IPInterfaceSubnetMask, VAR_IN},
       {"NewIPAddressingType", VAR_IPInterfaceAddressingType, VAR_IN},       
       { 0 }
    }
};

static Action _GetIPInterfaceGenericEntry = {
    "GetIPInterfaceGenericEntry", GetIPInterfaceGenericEntry,
   (Param []) {
       {"NewIPInterfaceIndex", VAR_IPInterfaceNumberOfEntries, VAR_IN},
       {"NewEnable", VAR_Enable, VAR_OUT},
       {"NewIPAddress", VAR_IPInterfaceIPAddress, VAR_OUT},
       {"NewSubnetMask", VAR_IPInterfaceSubnetMask, VAR_OUT},
       {"NewIPAddressingType", VAR_IPInterfaceAddressingType, VAR_OUT},
    //   {"NewIPInterfaceNumberOfEntries", VAR_IPInterfaceNumberOfEntries, VAR_OUT},
       { 0 }
    }
};

static PAction Actions[] = {
   &_GetInfo,
   &_SetDHCPServerConfigurable,
   &_GetDHCPServerConfigurable,
   &_GetDHCPRelay,
   &_SetSubnetMask,
   &_GetSubnetMask,
   &_SetIPRouter,
// &_DeleteIPRouter,
   &_GetIPRoutersList,
   &_SetDomainName,
   &_GetDomainName,
   &_SetAddressRange,
   &_GetAddressRange,
// &_SetReservedAddress,
// &_DeleteReservedAddress,
   &_SetDNSServer,
   &_DeleteDNSServer,
   &_GetDNSServers,
   &_SetDHCPLeaseTime,
   &_SetDHCPServerEnable,
   &_GetIPInterfaceNumberOfEntries,
   &_SetIPInterface,
//  &_AddIPInterface,
//  &_DeleteIPInterface,
   &_GetIPInterfaceGenericEntry,
    NULL
};

ServiceTemplate Template_LANHostConfigManagement = {
    "LANHostConfigManagement:",
    NULL,
    LANHostConfigManagement_GetVar,
    NULL,   /* SVCXML */
    ARRAYSIZE(StateVariables)-1, StateVariables,
    Actions,
    0,
    "urn:dslforum-org:serviceId:LANHostConfigManagement",
    NULL
};

