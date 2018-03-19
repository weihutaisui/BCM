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
*  \file OamProcessInit.h
*  \brief TK file transfer via OAM protocol
* 
*/
#ifndef OAMPROCESSINIT_H
#define OAMPROCESSINIT_H

#include "Teknovus.h"
#include "OntmTimer.h"
#include "EponDevInfo.h"
#include "cms_msg.h"
#include "AlarmCodes.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct
{
    BOOL isActive; //flag indicate is this OAM active
    
    //oam callback
    void (*VendorOamInit)(void);
    void (*VendorLinkInit)(LinkIndex link);
    void (*VendorOamInfoTlvGet)(OuiVendor oam, LinkIndex link, U8 BULK *msg, U8* len);
    void (*VendorOamInfoTlvHandle)(OuiVendor oam, LinkIndex link, const OamInfoTlv BULK* tlv);
    void (*VendorOamVendorHandle)(OuiVendor oam, LinkIndex link, U8 BULK* msg);
    void (*VendorLinkResetHandle)(U8 inst, U8 stat);
    void (*VendorAlarmNotify)(AlmCondition cond, U8 inst, StatId stat);
    void (*VendorEncKeyExchange)(OuiVendor oam, LinkIndex link, U8 keyNum, Bool keyGen);
    void (*VendorOamPoll)(void);

    //timer callback
    void (*Timer100msHandler)(void);
    void (*Timer1sHandler)(void);
    void (*PollTimerHandler)(OuiVendor oam, OntmTimerId timerId);
    
    void (*CmsMsgEthLinkStHandler)(CmsMsgType type, U8 inst);
    BOOL (*NLMsgNewLinkHandler)(U8 state, U8 port);
    void *next;
} PACK OamHandlerCB;

#define VENDOR_OAM_PRE_INIT() \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->VendorOamInit)) \
                                (tmpPtr->VendorOamInit)(); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)
            
#define VENDOR_LINK_INIT(link) \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->VendorLinkInit)) \
                                (tmpPtr->VendorLinkInit)(link); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)
            
#define VENDOR_OAM_INFO_TLV_GET(oam, link, msg, len) \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->VendorOamInfoTlvGet)) \
                                (tmpPtr->VendorOamInfoTlvGet)(oam, link, msg, len); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)
            
#define VENDOR_OAM_INFO_TLV_HANDLE(oam, link, tlv) \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->VendorOamInfoTlvHandle)) \
                                (tmpPtr->VendorOamInfoTlvHandle)(oam, link, tlv); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)

#define VENDOR_OAM_HANDLE(oam, link, tlv) \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->VendorOamVendorHandle)) \
                                (tmpPtr->VendorOamVendorHandle)(oam, link, tlv); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)
            
#define VENDOR_ALARM_NOTIFY_HANDLE(cond, inst, stat) \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->VendorAlarmNotify)) \
                                (tmpPtr->VendorAlarmNotify)(cond, inst, stat); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)
            
#define VENDOR_LINK_RESET_HANDLE(inst, stat) \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->VendorLinkResetHandle)) \
                                (tmpPtr->VendorLinkResetHandle)(inst, stat); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)
            
#define VENDOR_ENC_KEY_EXCHANGE(oam, link, keyNum, keyGen) \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->VendorEncKeyExchange)) \
                                (tmpPtr->VendorEncKeyExchange)(oam, link, keyNum, keyGen); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)
            
#define VENDOR_OAM_POLL() \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->VendorOamPoll)) \
                                (tmpPtr->VendorOamPoll)(); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)
            
#define TIMER_100MS_HANDLE() \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->Timer100msHandler)) \
                                (tmpPtr->Timer100msHandler)(); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)
            
#define TIMER_1S_HANDLE() \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->Timer1sHandler)) \
                                (tmpPtr->Timer1sHandler)(); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)
            
#define POLL_TIMER_HANDLE(oam, timerId) \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->PollTimerHandler)) \
                                (tmpPtr->PollTimerHandler)(oam, timerId); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)
            
#define CMS_ETH_LINK_ST_HANDLE(type, inst) \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->CmsMsgEthLinkStHandler)) \
                                (tmpPtr->CmsMsgEthLinkStHandler)(type, inst); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)
            
#define NL_NEW_LINK_HANDLE(state, port) \
                    do{ \
                        OamHandlerCB * tmpPtr = oamVenderHandlerHead; \
                        while(tmpPtr) { \
                            if ((tmpPtr->isActive) && (tmpPtr->NLMsgNewLinkHandler)) \
                                (tmpPtr->NLMsgNewLinkHandler)(state, port); \
                            tmpPtr = tmpPtr->next;  \
                            } \
                    }while(0)

/**********************************************************/
extern
OuiVendor oamValToOamVendor(U32 oamVal);


extern
OamHandlerCB *OamProcessAllocCB(void);

void OamProcessInit(U8 vendorMask);

#if defined(__cplusplus)
}
#endif

#endif /* OamFileTransfer.h */

