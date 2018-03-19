/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2013:proprietary:standard
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
 *
 * $Change: 160447 $
 ***********************************************************************/

/*
 * Timers
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "ieee1905_timer.h"
#include "ieee1905_linkedlist.h"
#include "ieee1905_trace.h"

#define I5_TRACE_MODULE i5TraceTimer

timer_elem_type i5_timer_list = {};

#if 0
static inline void i5TimerDump()
{
    if (i5TraceGet(i5TraceTimer) >= i5TraceLevelInfo) {
        timer_elem_type *ptmr = i5_timer_list.ll.next;
        while(ptmr != NULL){
            printf("[%p %p(%p) %ld.%04ld]->", ptmr, ptmr->process, ptmr->arg, ptmr->tv.tv_sec, ptmr->tv.tv_usec/1000);
            ptmr = ptmr->ll.next;
        }
        printf("\n");
    }
}
#endif


timer_elem_type *i5TimerNew(int msecs, void (*process)(void *arg), void *arg)
{
  struct timespec now_ts;
  timer_elem_type *ptmr;
  timer_elem_type *newtmr = (timer_elem_type *)malloc(sizeof(timer_elem_type));
  struct timeval  now_tv, add_tv;

  if (newtmr != NULL) {
    add_tv.tv_sec = msecs/1000;
    add_tv.tv_usec = (msecs%1000)*1000;

    clock_gettime(CLOCK_MONOTONIC, &now_ts);
    now_tv.tv_sec = now_ts.tv_sec;
    now_tv.tv_usec = now_ts.tv_nsec/1000;

    timeradd(&now_tv, &add_tv, &newtmr->tv);
    newtmr->process = process;
    if ( NULL == arg) {
      newtmr->arg = newtmr;
    }
    else {
      newtmr->arg = arg;
    }

    ptmr = &i5_timer_list;
    while ((ptmr->ll.next != NULL) && timercmp(&((timer_elem_type *)(ptmr->ll.next))->tv, &newtmr->tv, <)) {
      ptmr = ptmr->ll.next;
    }
    i5LlItemAdd(NULL, ptmr, newtmr);
  }
  return newtmr;
}

int i5TimerFree(timer_elem_type *ptmr)
{
  timer_elem_type *prevItem;

  prevItem = &i5_timer_list;
  while ((prevItem->ll.next != NULL) && ((timer_elem_type *)(prevItem->ll.next) != ptmr)) {
    prevItem = prevItem->ll.next;
  }
  if (prevItem->ll.next != NULL) {
      return i5LlItemFree(prevItem, prevItem->ll.next);
  } else {
      i5TraceError("could not delete item (%p) from list...\n", ptmr);      
      return -1;
  }
}

struct timeval *i5TimerExpires(struct timeval *ptv) {
  struct timespec now_ts;
  struct timeval  now_tv;
  timer_elem_type *ptmr = i5_timer_list.ll.next;

  while (ptmr != NULL) {
    clock_gettime(CLOCK_MONOTONIC, &now_ts);
    now_tv.tv_sec = now_ts.tv_sec;
    now_tv.tv_usec = now_ts.tv_nsec/1000;

    if (timercmp(&ptmr->tv, &now_tv, <)) {
      /* Expired */
      (ptmr->process)(ptmr->arg);
      ptmr = i5_timer_list.ll.next;
    } else {
      timersub(&ptmr->tv, &now_tv, ptv);
      return ptv;
    }
  }
  /* No timeout */
  return NULL;
}
