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
//  Filename:       layer2bridgeparams.h
//
******************************************************************************/

#ifndef _LAYER2BRIDGEPARAMS_H
#define _LAYER2BRIDGEPARAMS_H

#define VAR_MaxBridgeEntries                          0
#define VAR_MaxFilterEntries                          1
#define VAR_MaxMarkingEntries                         2
#define VAR_BridgeNumberOfEntries                     3
#define VAR_BridgeKey                                 4
#define VAR_BridgeEnable                              5
#define VAR_BridgeStatus                              6
#define VAR_BridgeName                                7
#define VAR_VLANID                                    8
#define VAR_FilterNumberOfEntries                     9
#define VAR_FilterKey                                 10
#define VAR_FilterEnable                              11
#define VAR_FilterStatus                              12
#define VAR_FilterBridgeReference                     13
#define VAR_ExclusivityOrder                          14
#define VAR_FilterInterface                           15
#define VAR_VLANIDFilter                              16
#define VAR_AdmitOnlyVLANTagged                       17
#define VAR_EthertypeFilterList                       18
#define VAR_EthertypeFilterExclude                    19
#define VAR_SourceMACAddressFilterList                20
#define VAR_SourceMACAddressFilterExclude             21
#define VAR_DestMACAddressFilterList                  22
#define VAR_DestMACAddressFilterExclude               23
#define VAR_SourceMACFromVendorClassIDFilter          24
#define VAR_SourceMACFromVendorClassIDFilterExclude   25
#define VAR_DestMACFromVendorClassIDFilter            26
#define VAR_DestMACFromVendorClassIDFIlterExclude     27
#define VAR_SourceMACFromClientIDFilter               28
#define VAR_SourceMACFromClientIDFilterExclude        29
#define VAR_DestMACFromClientIDFilter                 30
#define VAR_DestMACFromClientIDFilterExclude          31
#define VAR_SourceMACFromUserClassIDFilter            32
#define VAR_SourceMACFromUserClassIDFilterExclude     33
#define VAR_DestMACFromUserClassIDFilter              34
#define VAR_DestMACFromUserClassIDFilterExclude       35
#define VAR_MarkingNumberOfEntries                    36
#define VAR_MarkingKey                                37
#define VAR_MarkingEnable                             38
#define VAR_MarkingStatus                             39
#define VAR_MarkingBridgeReference                    40
#define VAR_MarkingInterface                          41
#define VAR_VLANIDUntag                               42
#define VAR_VLANIDMark                                43
#define VAR_EnternetPriorityMark                      44
#define VAR_EthernetPriorityOverride                  45
#define VAR_AvailableInterfaceNumberOfEntries         46
#define VAR_AvailableInterfaceKey                     47
#define VAR_InterfaceType                             48
#define VAR_InterfaceReference                        49

int GetLayer2Bridge_Info(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int AddBridgeEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int DeleteBridgeEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetSpecificBridgeEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);       
int GetGenericBridgeEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);       
int SetBridgeEntryEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);       
int GetSpecificAvailableInterfaceEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);       
int GetGenericAvailableInterfaceEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);       
#endif /* LAYER2BRIDGEPARAMS_H */
