/* 
* <:copyright-BRCM:2007:proprietary:gpon
* 
*    Copyright (c) 2007 Broadcom 
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
#ifndef BCM_PLOAM_ALARM_H
#define BCM_PLOAM_ALARM_H

#include "bcm_ploamEvent.h"
#include <bcmtypes.h>
#include <bcm_ploam_api.h>

#define BCM_PLOAM_HW_ALARM_MASK 0x1F
#define PLOAM_ALARM_UPDATE_PERIOD_MS 10                     /* Alarm updates are checked every 10ms*/
#define BCM_PLOAM_DEFAULT_ALARM_OFF_SOAK_THRESHOLD 100      /* Default Alarm OFF soak time is 1sec */
#define BCM_PLOAM_DEFAULT_ALARM_ON_SOAK_THRESHOLD 0         /* Default Alarm ON soak time is 0sec - immediate */
#define BCM_PLOAM_DEFAULT_LOS_LOL_LOF_OFF_SOAK_THRESHOLD 1  /* Default LOS_LOL_LOF Alarm OFF soak time is 10msec */
#define LOS_LOL_LOF (BCM_PLOAM_ALARM_ID_LOS|BCM_PLOAM_ALARM_ID_LOL|BCM_PLOAM_ALARM_ID_LOF)
#define LOL_LOF (BCM_PLOAM_ALARM_ID_LOL|BCM_PLOAM_ALARM_ID_LOF)

/*returns nonzero if error*/
int bcm_ploamAlarmSetSoakThresholds(uint32_t alarmId,
                                    uint16_t onThreshold, uint16_t offThreshold);

/*returns nonzero if error*/
int bcm_ploamAlarmGetSoakThresholds(uint32_t alarmId,
                                    OUT uint16_t *onThresholdp,
                                    OUT uint16_t *offThresholdp);

/*returns true if LOS/LOL/LOF is set*/
int bcm_ploamAlarmLosLolLof(void);

/*Call every 100ms*/
void bcm_ploamAlarmTick(void);

/*shortOffSoakMap: bitmap of alarmIds for which to shortcut Off soaking.*/
void bcm_ploamAlarmShortSoak(uint32_t shortOffSoakMap);

/*alarmId: One of BCM_PLOAM_ALARM_ID_*/
void bcm_ploamAlarmSet(uint32_t alarmId, int on);

void bcm_ploamAlarmSetMask(uint32_t mask);

/*Note: This will read-reset the alarmEventBitmap*/
void bcm_ploamAlarmStatus(OUT BCM_Ploam_AlarmStatusInfo* alarmStatusInfop);

void bcm_ploamAlarmIrq(uint32_t irqStatus);

/*Note: bcm_ploamAlarm will handle the event masking of losLofLolEventObjp*/
void bcm_ploamAlarmInit(BCM_PloamEventState* usrEventObjp);

#endif /*BCM_PLOAM_ALARM_H*/
