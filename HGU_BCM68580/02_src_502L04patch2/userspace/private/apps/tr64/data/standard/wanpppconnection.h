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
//  Filename:       wanpppconnection.h
//
******************************************************************************/
#ifndef _WANPPPCONNECTION_H_
#define _WANPPPCONNECTION_H_

#define VAR_Enable                         0
#define VAR_ConnectionType                 1
#define VAR_PossibleConnectionTypes        2
#define VAR_ConnectionStatus               3
#define VAR_Name                           4
#define VAR_IdleDisconnectTime             5
#define VAR_NATEnabled                     6
#define VAR_Username                       7
#define VAR_Password                       8
#define VAR_ExternalIPAddress              9
#define VAR_RemoteIPAddress                10
#define VAR_DNSServers                     11
#define VAR_PPPoEServiceName               12
#define VAR_ConnectionTrigger              13

#define VAR_PortMappingNumberOfEntries     14
#define VAR_PortMappingEnabled             15
#define VAR_ExternalPort                   16
#define VAR_InternalPort                   17
#define VAR_PortMappingProtocol            18
#define VAR_InternalClient                 19
#define VAR_PortMappingDescription         20

#define VAR_BytesSent                      21
#define VAR_BytesReceived                  22
#define VAR_PacketsSent                    23
#define VAR_PacketsReceived                24

int SetConnectionType(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetWANPPPConnEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetUsername(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetPassword(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetPPPoEService(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetConnectionTrigger(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int ForceTermination(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int RequestTermination(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int RequestConnection(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int WANPPPConnection_GetInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int WANPPPConnection_GetConnectionTypeInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int WANPPPConnection_GetIdleDisconnectTime(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int WANPPPConnection_GetStatusInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int WANPPPConnection_GetUsername(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int WANPPPConnection_GetStatisticsWANPPP(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetIdleDisconnectTime(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int ppp_GetGenericPortMappingEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int ppp_GetSpecificPortMappingEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int ppp_AddPortMappingEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int ppp_GetPortMappingNumberOfEntries(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int ppp_DeletePortMappingEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);

#endif /* _WANPPPCONNECTION_H */

