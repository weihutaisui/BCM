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
#include "bcm_ploamEvent.h"
#include <linux/bcm_log.h>

int bcm_ploamEventAvl(IN BCM_PloamEventState* statep) {
  BCM_ASSERT(statep);
  BCM_ASSERT(statep->initialized);
  /*BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");*/
  return (statep->eventBitmap & statep->eventMask) != 0;
}

void bcm_ploamEventRegister(IN BCM_PloamEventState* statep, IN BCM_PloamEventCallback* callback, IN void* ctxt) {
  BCM_ASSERT(statep);
  BCM_ASSERT(statep->initialized);
  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");

  statep->callback = callback;
  statep->ctxt = ctxt;
}

uint32_t bcm_ploamEventRead(IN BCM_PloamEventState* statep) {
  uint32_t ret;

  BCM_ASSERT(statep);
  BCM_ASSERT(statep->initialized);
  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");

  ret = statep->eventBitmap;
  statep->eventBitmap = 0;

  return ret;
}

uint32_t bcm_ploamEventPeek(IN BCM_PloamEventState* statep) {
  BCM_ASSERT(statep);
  BCM_ASSERT(statep->initialized);
  return statep->eventBitmap;
}

void bcm_ploamEventSetMask(IN BCM_PloamEventState* statep, uint32_t mask) {
  BCM_ASSERT(statep);
  BCM_ASSERT(statep->initialized);
  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"0x%x", mask);

  statep->eventMask = mask;
}

uint32_t bcm_ploamEventGetMask(IN BCM_PloamEventState* statep) {
  BCM_ASSERT(statep);
  BCM_ASSERT(statep->initialized);
  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");

  return statep->eventMask;
}

void bcm_ploamEventSet(IN BCM_PloamEventState* statep, uint32_t event) {
  BCM_ASSERT(statep);
  BCM_ASSERT(statep->initialized);

  statep->eventBitmap |= event;

  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"map: 0x%x, mask: 0x%x",statep->eventBitmap, statep->eventMask);

  if (((statep->eventBitmap & statep->eventMask)!=0) &&
      (statep->callback))
    statep->callback(statep->ctxt);
}


void bcm_ploamEventInit(OUT BCM_PloamEventState* statep) {
  BCM_ASSERT(statep);
  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");

  memset(statep, 0, sizeof(*statep));
  statep->eventMask = ~0;
  statep->initialized = true;
 
}

