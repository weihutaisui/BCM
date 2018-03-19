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
//  Filename:       wandsllinkconfig.h
//
******************************************************************************/
#ifndef _WANDSLLINKCONFIG_H_ 
#define _WANDSLLINKCONFIG_H_ 

#define VAR_Enable                   0
#define VAR_LinkStatus               1
#define VAR_LinkType                 2
#define VAR_AutoConfig               3
#define VAR_ModulationType           4
#define VAR_DestinationAddress       5
#define VAR_ATMEncapsulation         6
#define VAR_FCSPreserved             7
#define VAR_VCSearchList             8
#define VAR_ATMAAL                   9
#define VAR_ATMTransmittedBlocks     10
#define VAR_ATMReceivedBlocks        11
#define VAR_ATMQoS                   12
#define VAR_ATMPeakCellRate          13
#define VAR_ATMMaximumBurstSize      14
#define VAR_ATMSustainableCellRate   15
#define VAR_AAL5CRCErrors            16
#define VAR_ATMCRCErrors             17
#define VAR_ATMHECErrors             18

struct iftab {
    char *name;
    int  if_id;
};

typedef enum { 
    DSL_LINK_EoA, 
    DSL_LINK_IPoA, 
    DSL_LINK_PPoA,
    DSL_LINK_PPoE,
    DSL_LINK_CIP,
    DSL_LINK_UNCONFIGURED 
} dsl_link_t;

typedef enum { 
    DSL_LINK_UP, 
    DSL_LINK_DOWN, 
    DSL_LINK_INITIALIZING,
    DSL_LINK_UNAVAILABLE 
} dsl_conn_t;


struct WANDSLLinkConfigData {
    dsl_link_t  link_type;
    dsl_conn_t	link_status;
    int		autoconfig;
    timer_t     eventhandle;
};

typedef struct WANDSLLinkConfigData WANDSLLinkConfigData, *PWANDSLLinkConfigData;

int WANDSLLinkConfig_GetInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int WANDSLLinkConfig_GetDSLLinkInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetATMQoS(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetDestinationAddress(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetDestinationAddress(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetDSLEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetATMEncapsulation(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetATMEncapsulation(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int SetDSLLinkType(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);
int GetStatisticsWANDSL(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);

#endif /* _WANDSLLINKCONFIG_H_  */
