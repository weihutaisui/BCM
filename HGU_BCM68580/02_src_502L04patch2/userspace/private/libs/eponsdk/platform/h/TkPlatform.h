/*
* <:copyright-BRCM:2013:proprietary:epon
* 
*    Copyright (c) 2013 Broadcom 
*    All Rights Reserved
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
*/


#if !defined(TkPlatform_h)
#define TkPlatform_h

#include "Teknovus.h"
#include "TkOsAlSem.h"
#include "TkOsAlThread.h"
#include "TkOsAlMsg.h"
#include "TkOsAlDataInOut.h"
#include "TkOsAlCommon.h"

#include "TkDebug.h"

#include "vlanctl_api.h"

extern char eponVeipCmsIfIdx;

#define IFNAMESIZ 16
#define MAX_VEIP_If_NUM 8
#define EROUTER_VEIP_IDX eponVeipCmsIfIdx

#if defined(BCM_PON_XRDP)
#define MAX_UNI_PORTS 8
#else
#define MAX_UNI_PORTS 4
#endif

extern char uniRealIfname[MAX_UNI_PORTS][IFNAMESIZ];
extern char eponRealIfname[IFNAMESIZ];
extern char eponVlanIfname[IFNAMESIZ];
extern char eponVeipIfname[MAX_VEIP_If_NUM][IFNAMESIZ];
extern char eponSfuVirtualIfname[IFNAMESIZ];

typedef enum
    {
    OAM_BCM_SUPPORT = 1<<OuiTeknovus,
    OAM_CTC_SUPPORT = 1<<OuiCtc,
    OAM_NTT_SUPPORT = 1<<OuiNtt,
    OAM_DASAN_SUPPORT = 1<<OuiDasan,
    OAM_DPOE_SUPPORT = 1<<OuiDpoe,
    OAM_KT_SUPPORT = 1<<OuiKt,
    OAM_PMC_SUPPORT = 1<<OuiPmc,
    OAM_CUC_SUPPORT = 1<<OuiCuc,
    OAM_MAX_SUPPORT = 1<<OuiOamEnd
    }vendorOamMask;


#define MAX_VLANCTL_TAGS 3
#define MAX_PERMITTED_VLANS 8
#define MAX_VLANCTL_RULE_IDS 64
#define MAX_PERMITTED_AG_GROUP  8

typedef struct {
    U8 valid;
    vlanCtl_direction_t dir;
    unsigned int nbrOfTags;
    unsigned int tagRuleId;
    char devName[IFNAMESIZ];
} PACK vlan_rule_data;


#endif // TkPlatform_h
