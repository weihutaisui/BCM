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
#ifndef _BCM_PLOAM_H_
#define _BCM_PLOAM_H_

#include "bcm_OS_Deps.h"
#include "board.h"
#include "boardparms.h"
#include "bcm_intr.h"
#include "bcm_gponBasicDefs.h"
#include <linux/bcm_log.h>
#include "bcm_ploamPortCtrl.h"
#include "bcm_omci.h"
#include "bcm_ploamUsr.h"

#define PLOAM_COUNTER_UPDATE_PERIOD_MS BCM_SFSD_UPDATE_PERIOD_MS

#define SECS_IN_MSECS 1000
#define MINS_IN_MSECS (SECS_IN_MSECS * 60)
#define HOURS_IN_MSECS (MINS_IN_MSECS * 60)
#define DAYS_IN_MSECS (HOURS_IN_MSECS * 60)
#define YEARS_IN_MSECS (DAYS_IN_MSECS * 365)

/* On bcm96838, index 0 is reserved for Default ALLOC Id so we
 * need to increment by 1 the index of the TCONT that comes from OMCI
 * userspace application */
#define SET_TCONT_IDX( tcont ) ( tcont++ )
#define GET_TCONT_IDX( tcont ) ( tcont-- )

typedef struct {
  uint32_t irqCounter;
  uint32_t rxPloamsTotal;
  uint32_t rxPloamsUcast;
  uint32_t rxPloamsBcast;
  uint32_t rxPloamsDiscarded;
  uint32_t rxPloamsNonStd;
  uint32_t txPloams;
  uint32_t txPloamsNonStd;
  uint32_t rxDeactivates;
  uint32_t linkUpStartTime;

} BCM_PloamSwCounters;

typedef struct 
{
    BCM_Ploam_AdminState adminState;
    BCM_Ploam_OperState onuState;
    BCM_Ploam_SerialPasswdInfo newSerialPasswd; /*Will become current at O2 entry*/
    BCM_Ploam_SerialPasswdInfo curSerialPasswd;
    BCM_PloamSwCounters swCounters;
    BCM_PloamEventState usrEventObj;
    bool initialized;
    bool created;
}PloamState;

void bcm_ploamGetCounters(OUT BCM_PloamSwCounters* ploamCounters, int reset);

const char* bcm_ploamGetStateString(BCM_Ploam_OperState onuState);

BCM_Ploam_OperState bcm_ploamGetOnuState(void);

void bcm_ploamGetGtcParams(OUT BCM_Ploam_GtcParamInfo* gtcParamInfop);

/*Returns nonzero if encryption is not enabled*/
int bcm_ploamGetAesKey(OUT uint8_t key[BCM_PLOAM_ENCRYPTION_KEY_SIZE_BYTES]);

void bcm_ploamSet1stSnPreamble(IN BCM_Ploam_1stSnPreambleInfo* firstSnPreambleInfo);

int bcm_ploamStart(BCM_Ploam_OperState initOperState);
int bcm_ploamStop(bool sendDyingGasp);
void bcm_ploamGetState(OUT BCM_Ploam_StateInfo* stateInfop);
BCM_Ploam_AdminState bcm_ploamGetAdminState(void);

void bcm_ploamSetTO1(int to1);
void bcm_ploamSetTO2(int to2);

int bcm_ploamGetTO1(void);
int bcm_ploamGetTO2(void);

/*Equivalent of a constructor. Performs one-time setup actions.*/
int __devinit bcm_ploamCreate(void);

/*(Re)Initialize this module*/
void bcm_ploamInit(bool keepConfig);

void bcm_ploamLoadNewSerialPasswd(void);
int bcm_ploamConnectInterrupt(void);
void bcm_ploamSetSerialPasswd(IN BCM_Ploam_SerialPasswdInfo *serialPasswdInfo);
void bcm_ploamGetSerialPasswd(OUT BCM_Ploam_SerialPasswdInfo *serialPasswdInfo);
void bcm_ploamSetNewSerialPasswd(BCM_Ploam_SerialPasswdInfo* serialPasswdInfop);
/*Equivalent of a destructor. Performs clean-up actions.*/
void bcm_ploamDelete(void);

/**
 * 'Public' Global variables
 **/
extern int PLOAMGEMPORTMAP;

/**
 *Debug API for ploamProc (gpondbg)
 **/
void bcm_ploamDumpOnDeact(int enable);
void bcm_ploamSetRangingTimeOffset(int rangingTimeOffset);
/* Function to set offset for Pre/Post Ranged state */
void bcm_ploamSetPreambleOffset(int preambleOffset, int bPreOrPostRanged);
/* Function to get the current preamble settings */
int bcm_ploamGetCurPreambleLen(OUT int *pOnuState, OUT int *pCurPreamblePreRangeOffset,
                               OUT int *pCurPreambleRangedOffset);
void bcm_ploamSetEQ(int eq);
void bcm_ploamRandomizeSN(int enable);
void bcm_ploamGemPortMap(int enable);
int bcm_ploamFsm(BCM_Ploam_OperState newState);
/*returns nonzero if offset out of range*/
int bcm_ploamSetAckOffset(int offset);
int bcm_ploamGetAckOffset(int *pOffset);
void startDbgDataPollTmr(void);
unsigned long bcm_ploamCheckFeCfg(void);
int bcm_ploamGetAesKey(OUT uint8_t key[BCM_PLOAM_ENCRYPTION_KEY_SIZE_BYTES]);
int bcm_ploamSetNewKey(uint32_t  timeSec, uint8_t key[BCM_PLOAM_ENCRYPTION_KEY_SIZE_BYTES], uint32_t *sfCnt);
int bcm_ploamSendDyingGasp(void) ;
int get_gem_flow_id_by_gem_port(bdmf_number gemPortID, UINT16 *gemPortIdx);

#endif /*_BCM_PLOAM_H_*/

