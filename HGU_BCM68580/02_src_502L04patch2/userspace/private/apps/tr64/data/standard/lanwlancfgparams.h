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
//  Filename:       lanwlancfgparams.h
//
******************************************************************************/
#ifndef _LANWLANCFGPARAMS_H
#define _LANWLANCFGPARAMS_H

#define VAR_Enable                                      0
#define VAR_Status                                      1
#define VAR_BSSID                                       2
#define VAR_MaxBitRate                                  3
#define VAR_Channel                                     4
#define VAR_SSID                                        5
#define VAR_BeaconType                                  6
#define VAR_MACAddressControlEnabled                    7
#define VAR_Standard                                    8
#define VAR_WEPKeyIndex                                 9
#define VAR_KeyPassphrase                               10
#define VAR_WEPEncryptionLevel                          11
#define VAR_BasicEncryptionModes                        12
#define VAR_BasicAuthenticationMode                     13
#define VAR_WPAEncryptionModes                          14
#define VAR_WPAAuthenticationMode                       15
#define VAR_IEEE11iEncryptionModes                      16
#define VAR_IEEE11iAuthenticationMode                   17
#define VAR_PossibleChannels                            18
#define VAR_BasicDataTransmitRates                      19
#define VAR_OperationalDataTransmitRates                20
#define VAR_PossibleDataTransmitRates                   21
#define VAR_InsecureOOBAccessEnabled                    22
#define VAR_BeaconAdvertisementEnabled                  23
#define VAR_RadioEnabled                                24
#define VAR_AutoRateFallBackEnabled                     25
#define VAR_LocationDesccription                        26
#define VAR_RegulatoryDomain                            27
#define VAR_TotalIPSKFailures                           28
#define VAR_TotalIntergrityFailures                     29
#define VAR_ChannelsInUse                               30
#define VAR_DeviceOperationMode                         31
#define VAR_DistanceFromRoot                            32
#define VAR_PeerBSSID                                   33
#define VAR_AuthenticationServiceMode                   34
#define VAR_TotalBytesSent                              35
#define VAR_TotalBytesReceived                          36
#define VAR_TotalPacketsSent                            37
#define VAR_TotalPacketsReceived                        38
#define VAR_TotalAssociations                           39
#define VAR_AssociatedDeviceMACAddress                  40
#define VAR_AssociatedDeviceIPAddress                   41
#define VAR_AssociatedDeviceAuthenticationState         42
#define VAR_WEPKey                                      43
#define VAR_PreSharedKey                                44
#define VAR_PreSharedKeyIndex                           45

int GetWlanInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetConfig(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetSSID(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetSSID(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetBSSID(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetChannel(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetChannelInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetBeaconType(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetBeaconType(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetRadioMode(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetRadioMode(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetTotalAssociations(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetDefaultWEPKeyIndex(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetDefaultWEPKeyIndex(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetBasBeaconSecurityProperties(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetBasBeaconSecurityProperties(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetWPABeaconSecurityProperties(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetWPABeaconSecurityProperties(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetGenericAssociatedDeviceInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetSpecificAssociatedDeviceInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetSecurityKeys(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetSecurityKeys(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetWlanStatistics(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetWlanByteStatistics(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetWlanPacketStatistics(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetPreSharedKey(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetPreSharedKey(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
#endif /* LANWLANCFGPARAMS_H */
