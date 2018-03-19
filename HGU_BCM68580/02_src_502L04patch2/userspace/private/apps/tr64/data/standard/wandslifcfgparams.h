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
//  Filename:       wandslifcfgparams.h
//
******************************************************************************/
#ifndef _WANDSLIFCFGPARAMS_H_ 
#define _WANDSLIFCFGPARAMS_H_ 

#define VAR_Enable                 0
#define VAR_Status                 1
#define VAR_UpstreamCurrRate       2
#define VAR_DownstreamCurrRate     3
#define VAR_UpstreamMaxRate        4
#define VAR_DownstreamMaxRate      5
#define VAR_UpstreamNoiseMargin    6
#define VAR_DownstreamNoiseMargin  7
#define VAR_UpstreamAttenuation    8
#define VAR_DownstreamAttenuation  9
#define VAR_UpstreamPower          10
#define VAR_DownstreamPower        11
#define VAR_ATURVendor             12
#define VAR_ATURCountry            13
#define VAR_ATURANSIStd            14
#define VAR_ATURANSIRev            15
#define VAR_ATUCVendor             16
#define VAR_ATUCCountry            17
#define VAR_ATUCANSIStd            18
#define VAR_ATUCANSIRev            19
#define VAR_TotalStart             20
#define VAR_ShowTimeStart          21
#define VAR_LastShowTimeStart      22
#define VAR_CurrentDayStart        23
#define VAR_QuarterHourStart       24

#define VAR_ReceiveBlocks          25
#define VAR_TransmitBlocks         26
#define VAR_CellDelin              27
#define VAR_LinkRetrain            28
#define VAR_InitErrors             29
#define VAR_InitTimeouts           30
#define VAR_LossOfFraming          31
#define VAR_ErroredSecs            32
#define VAR_SeverelyErroredSecs    33
#define VAR_FECErrors              34
#define VAR_ATUCFECErrors          35
#define VAR_HECErrors              36
#define VAR_ATUCHECErrors          37
#define VAR_CRCErrors              38
#define VAR_ATUCCRCErrors          39

int WANDSLInterfaceConfig_GetVar(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
#if 0 //remove this action firstly, set atm to disabled will make cpe hang on 6338 chip 
int SetDSLInterfaceEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
#endif
int GetStatisticsTotal(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetStatisticsShowTime(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);

#endif /* _WANDSLLINKCONFIG_H_  */
