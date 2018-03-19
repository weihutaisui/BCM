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
//  Filename:       lanparams.h
//
******************************************************************************/
#ifndef _LANPARAMS_H
#define _LANPARAMS_H

#define VAR_DefaultConnectionService	0
#define GetDefaultConnectionService		DefaultAction

#define GetInfo 			DefaultAction
#define GetDHCPServerConfigurable	DefaultAction
#define GetDHCPRelay			DefaultAction
#define GetSubnetMask			DefaultAction
#define GetIPRoutersList		DefaultAction
#define GetDomainName			DefaultAction
#define GetReservedAddresses		DefaultAction
#define GetDNSServers			DefaultAction

#define VAR_DHCPServerConfigurable          0
#define VAR_DHCPServerEnable                1
#define VAR_DHCPRelay                       2
#define VAR_MinAddress                      3
#define VAR_MaxAddress                      4
#define VAR_ReservedAddresses               5
#define VAR_SubnetMask                      6
#define VAR_DNSServers                      7
#define VAR_DomainName                      8
#define VAR_IPRouters                       9
#define VAR_DHCPLeaseTime                   10
#define VAR_IPInterfaceNumberOfEntries      11
#define VAR_Enable                          12
#define VAR_IPInterfaceIPAddress            13
#define VAR_IPInterfaceSubnetMask           14
#define VAR_IPInterfaceAddressingType       15

int LANHostConfigManagement_Init(PService, service_state_t);
int LANHostConfigManagement_GetVar(struct Service *psvc, int varindex);
void LANHostConfigManagement_UpdateStats(timer_t t, PService psvc);

int SetDHCPLeaseTime(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetDHCPServerConfigurable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetDHCPServerEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetIPRouter(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetSubnetMask(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetDomainName(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetAddressRange(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetDNSServer(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int DeleteDNSServer(UFILE *uclient, PService psvc, PAction ac,pvar_entry_t args, int nargs);
int GetAddressRange(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetIPInterfaceNumberOfEntries(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetIPInterfaceGenericEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetIPInterface(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
#endif /* LANPARAMS_H */
