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
#include "bcm_OS_Deps.h"
#include <linux/bcm_log.h>
#include "bcm_ploamAlarm.h"

typedef struct {
  uint16_t alarmOnSoakCount;
  uint16_t alarmOnSoakThreshold;
  uint16_t alarmOffSoakCount;
  uint16_t alarmOffSoakThreshold;

} BCM_PloamAlarmSoakInfo;

typedef struct {
  uint32_t alarmStatus;

  /*Although this table is dimensioned for all alarms, it's only used for
   *'HW' alarms: LOS/LOF/LOL/LCDG/APC*/
  BCM_PloamAlarmSoakInfo soakTable[BCM_PLOAM_NUM_ALARMS];
  BCM_PloamEventState *usrEventObj;
  BCM_PloamEventState alarmEventObj;
  uint32_t uerrHecCount;
  bool initialized;
} BCM_PloamAlarmState;

static BCM_PloamAlarmState ploamAlarmState;
static BCM_PloamAlarmState *statep = &ploamAlarmState;

/*Returns -1, if alarmId not found*/
static int32_t log2AlarmId(uint32_t alarmId) {
  int i;

  for (i=0; i<BCM_PLOAM_NUM_ALARMS; ++i) {
    if ((1<<i) == alarmId)
      return i;
  }

  return -1;
}


int bcm_ploamAlarmSetSoakThresholds(uint32_t alarmId,
                                    uint16_t onThreshold, uint16_t offThreshold) {
  int32_t idx = log2AlarmId(alarmId);

  if ((alarmId & BCM_PLOAM_HW_ALARM_MASK) == 0)
    return -1;

  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM_ALARM,
                "alarmId=0x%x, onThreshold=%d, offThreshold=%d.",
                alarmId, onThreshold, offThreshold);

  BCM_ASSERT(statep->initialized);
  BCM_ASSERT(idx >= 0);

  statep->soakTable[idx].alarmOnSoakThreshold = onThreshold;
  statep->soakTable[idx].alarmOffSoakThreshold = offThreshold;

  return 0;
}

int bcm_ploamAlarmGetSoakThresholds(uint32_t alarmId,
                                    OUT uint16_t *onThresholdp,
                                    OUT uint16_t *offThresholdp) {
  int32_t idx = log2AlarmId(alarmId);

  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM_ALARM, "alarmId=0x%x.", alarmId);
  if ((alarmId & BCM_PLOAM_HW_ALARM_MASK) == 0)
    return -1;

  BCM_ASSERT(statep->initialized);
  BCM_ASSERT(onThresholdp);
  BCM_ASSERT(offThresholdp);
  BCM_ASSERT(idx >= 0);

  *onThresholdp = statep->soakTable[idx].alarmOnSoakThreshold;
  *offThresholdp = statep->soakTable[idx].alarmOffSoakThreshold;

  return 0;
}

static void logAlarm(uint32_t alarmEventBitmap,
                     uint32_t alarmStatusBitmap) {
  return ;
}

#define SOAK_FLAG_SHORT_ON (1<<0)
#define SOAK_FLAG_SHORT_OFF (1<<1)
#define SOAK_FLAG_TICK (1<<2)

/*returns alarmEvent bitmap*/
static uint32_t soakAlarm(uint32_t soakFlags,
                          uint32_t alarmIdx, int alarmOn,
                          INOUT uint32_t *alarmSoakedStatusBitmapp) {
  uint32_t alarmEventBitmap=0;
  uint32_t alarmId = (1<<alarmIdx);
  BCM_PloamAlarmSoakInfo *soakTablep = &statep->soakTable[alarmIdx];

  BCM_ASSERT(alarmSoakedStatusBitmapp);
  if (alarmOn) {
if (soakFlags & SOAK_FLAG_TICK) {
      /*bump On soak count, reset off soak count*/
      if (soakTablep->alarmOnSoakCount < soakTablep->alarmOnSoakThreshold)
        ++soakTablep->alarmOnSoakCount;
      soakTablep->alarmOffSoakCount = 0;
    }

    /*Do we need to shortcut On Soaking for this alarm?*/
    if (soakFlags & SOAK_FLAG_SHORT_ON) {
      /*Do so by forcing soak count equal to threshold*/
      soakTablep->alarmOnSoakCount = soakTablep->alarmOnSoakThreshold;
    }

    if ((soakTablep->alarmOnSoakCount >= soakTablep->alarmOnSoakThreshold) && /*soaked*/
        ((*alarmSoakedStatusBitmapp & alarmId)==0)) { /*not On yet*/
      BCM_LOG_INFO(BCM_LOG_ID_PLOAM_ALARM,"Alarm 0x%x On (soaked).", alarmId);

      /*Soaked long enough -> enable alarm*/
      *alarmSoakedStatusBitmapp |= alarmId;
      alarmEventBitmap |= alarmId;
    }
  }
  else {
    if (soakFlags & SOAK_FLAG_TICK) {
      /*alarm off -> bump Off soak count, reset on soak count*/
      if (soakTablep->alarmOffSoakCount < soakTablep->alarmOffSoakThreshold)
        ++soakTablep->alarmOffSoakCount;
      soakTablep->alarmOnSoakCount = 0;
    }

    /*Do we need to shortcut Off Soaking for this alarm?*/
    if (soakFlags & SOAK_FLAG_SHORT_OFF) {
      /*Do so by forcing soak count equal to threshold*/
      soakTablep->alarmOffSoakCount = soakTablep->alarmOffSoakThreshold;
    }

    if ((soakTablep->alarmOffSoakCount >= soakTablep->alarmOffSoakThreshold) && /*soaked*/
        ((*alarmSoakedStatusBitmapp & alarmId)!=0)) { /*not Off yet*/
      BCM_LOG_INFO(BCM_LOG_ID_PLOAM_ALARM,"Alarm 0x%x Off (soaked).", alarmId);

      /*Soaked long enough -> disable alarm*/
      *alarmSoakedStatusBitmapp &= ~alarmId;
      alarmEventBitmap |= alarmId;
    }
  }

  return alarmEventBitmap;
}

void bcm_ploamAlarmShortSoak(uint32_t shortOffSoakMap) {
  /*irqStatus=0, otherwise we'd be in an isr now*/
}

void bcm_ploamAlarmTick() {
  /*irqStatus=0, otherwise we'd be in an isr now*/
}

void bcm_ploamAlarmSetMask(uint32_t mask) {
  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM_ALARM,"");
  BCM_ASSERT(statep->initialized);

  bcm_ploamEventSetMask(&statep->alarmEventObj, mask);
}

void bcm_ploamAlarmSet(uint32_t alarmId, int on) {

  uint32_t alarmEventBitmap;
  uint32_t alarmIdx = log2AlarmId(alarmId);

  /*BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM_ALARM,"");*/
  BCM_ASSERT(alarmIdx >= 0);
  BCM_ASSERT(statep->initialized);

  if (alarmId == BCM_PLOAM_ALARM_ID_MEM) {
    alarmEventBitmap = BCM_PLOAM_ALARM_ID_MEM;
  }
  else {
    alarmEventBitmap = soakAlarm(SOAK_FLAG_SHORT_ON|SOAK_FLAG_SHORT_OFF,
                                 alarmIdx,
                                 on,
                                 &statep->alarmStatus);
  }

  if (alarmEventBitmap) {
    logAlarm(alarmEventBitmap, statep->alarmStatus);
    bcm_ploamEventSet(&statep->alarmEventObj, alarmEventBitmap);
  }
}

void bcm_ploamAlarmStatus(BCM_Ploam_AlarmStatusInfo* alarmStatusInfop) {
  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM_ALARM,"");
  BCM_ASSERT(statep->initialized);

  alarmStatusInfop->alarmEventBitmap = bcm_ploamEventRead(&statep->alarmEventObj);
  alarmStatusInfop->alarmMaskBitmap = bcm_ploamEventGetMask(&statep->alarmEventObj);
  alarmStatusInfop->alarmStatusBitmap = statep->alarmStatus;
}


void bcm_ploamAlarmIrq(uint32_t irqStatus) {
}

static void ploamAlarmCallback(void *arg) {
  bcm_ploamEventSet(statep->usrEventObj, BCM_PLOAM_EVENT_ALARM);
}

void bcm_ploamAlarmInit(BCM_PloamEventState* usrEventObjp) {
  int alamrIdx;

  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM_ALARM,"");

  BCM_ASSERT(usrEventObjp);

  memset(statep, 0, sizeof(*statep));

  for (alamrIdx=0; alamrIdx<BCM_PLOAM_NUM_ALARMS; alamrIdx++) {
    BCM_PloamAlarmSoakInfo *soakTablep = &statep->soakTable[alamrIdx];
    if ((1<<alamrIdx) & LOS_LOL_LOF) {
      soakTablep->alarmOffSoakThreshold = BCM_PLOAM_DEFAULT_LOS_LOL_LOF_OFF_SOAK_THRESHOLD;
      soakTablep->alarmOnSoakThreshold = BCM_PLOAM_DEFAULT_ALARM_ON_SOAK_THRESHOLD;
    }
    else {
      soakTablep->alarmOffSoakThreshold = BCM_PLOAM_DEFAULT_ALARM_OFF_SOAK_THRESHOLD;
      soakTablep->alarmOnSoakThreshold = BCM_PLOAM_DEFAULT_ALARM_ON_SOAK_THRESHOLD;
    }
  }

  statep->usrEventObj = usrEventObjp;

  bcm_ploamEventInit(&statep->alarmEventObj);

  bcm_ploamEventRegister(&statep->alarmEventObj, ploamAlarmCallback, 0 /*ctxt*/);

  statep->initialized = 1;
}

