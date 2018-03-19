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


/*
* \file TkOamProcess.h
* \brief the data process handler
*
*/

#ifndef TKOAMPROCESS_h
#define TKOAMPROCESS_h

#include "TkPlatform.h"
#include "Oam.h"
#include "TkSdkInitApi.h"
#ifdef BRCM_CMS_BUILD
#include "cms_msg.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    NlMsgBase                    = 0x00, //change to 0x00 might break other alarms especially power loss
    // per-ONT alarms  
    NlMsgOnt1GDnRegistered = NlMsgBase, 
    NlMsgOnt1GDnActivity,                //0x01
    NlMsgOnt10GDnRegistered,             //0x02
    NlMsgOnt10GDnActivity,               //0x03
    NlMsgOnt1GUpRegistered,              //0x04
    NlMsgOnt1GUpActivity,                //0x05
    NlMsgOnt10GUpRegistered,             //0x06
    NlMsgOnt10GUpActivity,               //0x07 
    NlMsgOntSingleLinkReg,               //0x08
    NlMsgOntMultiLinkReg,                //0x09

    NlMsgOntFecUp,                       //0x0A
    NlMsgOntFecDn,                       //0x0B
    NlMsgOntProtSwitch,                  //0x0C
    NlMsgOntRogueOnu,                    //0x0D

    // EPON port alarms             
    NlMsgEponLos,                        //0x0E
    NlMsgEponLostSync,                   //0x0F
    NlMsgEponStatThreshold,              //0x10 
    NlMsgEponActLed,                     //0x11
    NlMsgEponStandbyLos,                 //0x12
    NlMsgEponLaserShutdownTemp,          //0x13
    NlMsgEponLaserShutdownPerm,          //0x14
    NlMsgEponLaserOn,                    //0x15

    // per-LLID alarms
    NlMsgLinkNoGates,                    //0x16
    NlMsgLinkUnregistered,               //0x17
    NlMsgLinkOamTimeout,                 //0x18
    NlMsgLinkOamDiscComplete,            //0x19
    NlMsgLinkStatThreshold,              //0x1A
    NlMsgLinkKeyExchange,                //0x1B
    NlMsgLinkLoopback,                   //0x1C
    NlMsgLinkAllDown,                    //0x1D
    NlMsgLinkDisabled,                   //0x1E 
    NlMsgLinkRegStart,                   //0x1F 
    NlMsgLinkRegSucc,                    //0x20
    NlMsgLinkRegFail,                    //0x21
    NlMsgLinkRegTimeOut,                 //0x22
    NlMsgNums,

    //extend besides previous alarm set
    NlMsgLinkFailSafeReset = 0xFE,                 //0xFE
    NlMsgInvalid                    = 0xFF
}NlMsg_e;

typedef enum{
    nl_type_brcm = 1,
    nl_type_ctc,
    nl_type_max
}NlMsg_Type_e;

typedef struct{
    U8   type;     
    U16  almid;
    BOOL raise;
    U8   inst;
    U8   infoLen;
    U64  value;
} PACK NlMsgData;

typedef struct{
    U8   oui[3];
    U8   count;
    U32  crc;
} PACK NlMsgHeader;


typedef struct{
    NlMsgHeader hd;
    /* multiple msg data may reside in one netlink message */
    NlMsgData   *data;
} PACK NlMsg;

BOOL TkDataProcessHandle( TkMsgBuff *pSrcMsg, TkMsgBuff *pResMsg, U16 len );
BOOL TkDataProcessHandle_NetlinkEponMsg(struct msghdr *msg, U16 len);
BOOL send_notification_to_cms(LinkIndex link, Bool in_service);
BOOL send_bcm_hgu_notification_to_cms(LinkIndex link, Bool in_service, S32 vlan_id, S32 pbits);
BOOL TkDataProcessHandle_NetlinkRouteMsg(struct msghdr *msg, U16 len);
#ifdef BRCM_CMS_BUILD
void TkDataProcessHandle_CmsMsg( CmsMsgHeader *pMsg );
#endif
#ifdef BRCM_PKTCBL_SUPPORT
int epon_initialize_voice_virtual_interface(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
