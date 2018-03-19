/******************************************************************************
 *
 * Copyright (c) 2015   Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2009:proprietary:standard
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
 ************************************************************************/

/***************************************************************************
 *
 *    Copyright (c) 2008-2015, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Description: MoCA userland daemon
 *
 ***************************************************************************/


#include <mocalib.h>
#include <stdio.h>
#ifndef STANDALONE
#include <stdlib.h>
#endif
#include <limits.h>

#include "mocad-timer.h"

#include "mocaint.h"
#include "mocad.h"
#include "mocad-gen.h"
#include "mocad-int.h"

#define TIMER_MAX_INTERVAL ((UINT_MAX >> 1) + 1)

// Wrap safe check (x < y)
#define TIME_WRAP_SAFE_LESS_THAN(x, y) ((x) - (y) >= TIMER_MAX_INTERVAL)
// Wrap safe distance from a to b if TIME_WRAP_SAFE_LESS_THAN(a, b)
#define TIME_WRAP_SAFE_DISTANCE(a, b)  ((a) < (b) ? (b) - (a) : UINT_MAX - (a) + (b) + 1)

#ifdef MOCAD_TIMER_UNITTEST
unsigned int mocad_timer_unittest_time = 0;
#define GET_TIME_SEC() (mocad_timer_unittest_time)
#define TIME_TICK() mocad_timer_unittest_time++
#else
#define GET_TIME_SEC() MoCAOS_GetTimeSec()
#endif

#define TIMER_DBG printf

void mocad_timer_add(struct mocad_ctx *ctx, struct mocad_timer **head, struct mocad_timer *t, unsigned int tmo, mocad_timer_cb cb, void *cb_arg) {
   struct mocad_timer *i;
   struct mocad_timer *prev = NULL;

   unsigned int now = GET_TIME_SEC();
   unsigned int exp = now + tmo;

   for (i = *head; i != NULL; i = i->mt_next) {
      if (i == t) {
         TIMER_DBG("WARNING: Attempt to add t %p when it is already on the timer list\n", t);
         return; // already added, don't touch!
      }
   }
   for (i = *head; i != NULL; i = i->mt_next) {
      if (TIME_WRAP_SAFE_LESS_THAN(exp, i->mt_exp))
         break;
      prev = i;
   }

   t->mt_exp = exp;
   t->mt_cb = cb;
   t->mt_cb_arg = cb_arg;

   if (prev == NULL) { // head insert
      *head = t;
      t->mt_next = i;
   }
   else {
      prev->mt_next = t;
      t->mt_next = i;
   }
}

void mocad_timer_remove(struct mocad_ctx *ctx, struct mocad_timer **head, struct mocad_timer *t) {
   struct mocad_timer *i;
   struct mocad_timer *prev = NULL;

   for (i = *head; i != NULL; i = i->mt_next) {
      if (i == t)
         break;
      prev = i;
   }
   if (i == NULL) // never found it, not an error!
      return;
   if (prev == NULL) { // head remove
      *head = i->mt_next;
   }
   else {
      prev->mt_next = i->mt_next;
   }
   t->mt_next = NULL;
   t->mt_exp = 0;
   t->mt_cb = NULL;
   t->mt_cb_arg = NULL;
}

unsigned int mocad_timer_next(struct mocad_ctx *ctx, struct mocad_timer **head) {
   unsigned int now = GET_TIME_SEC();

   if (*head == NULL)
      return MoCAOS_TIMEOUT_INFINITE;

   if (TIME_WRAP_SAFE_LESS_THAN(now, (*head)->mt_exp))
      return TIME_WRAP_SAFE_DISTANCE(now, (*head)->mt_exp);
   return 0;
}

void mocad_timer_process(struct mocad_ctx *ctx, struct mocad_timer **head) {
   unsigned int now = GET_TIME_SEC();

   while (*head != NULL) {
      struct mocad_timer *t = *head;
      if (TIME_WRAP_SAFE_LESS_THAN(now, t->mt_exp))
         break;

      // Remove entry first, in case callback re-adds same timer!
      *head = (*head)->mt_next;
      t->mt_next = NULL;

      t->mt_cb(ctx, t->mt_cb_arg);
   }
}


#ifdef MOCAD_TIMER_UNITTEST
// UNIT TEST

#define TEST_ASSERT(x) do { if (!(x)) { fprintf(stderr, "mocad_timer UNIT TEST FAIL line %d!\n", __LINE__); exit(1); } } while(0)

int did_cb_run = 0;
void *cb_arg_test = (void *)0xdeadbeef;
void *cb_ctx_test;
struct mocad_timer *cb_timer_to_add = NULL;

void mocad_timer_unittest_cb(struct mocad_ctx *ctx, void *arg) {
   did_cb_run++;
   TEST_ASSERT(ctx == cb_ctx_test);
   TEST_ASSERT(arg == cb_arg_test);
   if (cb_timer_to_add != NULL)
      mocad_timer_add(ctx, &ctx->timer_head, cb_timer_to_add, 1, mocad_timer_unittest_cb, cb_arg_test);
}

void mocad_timer_unittest(struct mocad_ctx *ctx, struct mocad_timer **head) {
   struct mocad_timer timer1;
   struct mocad_timer timer2;
   struct mocad_timer timer3;
   unsigned int next;
   unsigned int ticks_until_wrap = 12;
   int test_rounds = ticks_until_wrap + 1;

   while (test_rounds--) {
      // Empty
      TEST_ASSERT(mocad_timer_next(ctx, head) == MoCAOS_TIMEOUT_INFINITE);
      mocad_timer_process(ctx, head);

      // Insert first
      cb_ctx_test = ctx;
      mocad_timer_add(ctx, head, &timer1, 1, mocad_timer_unittest_cb, cb_arg_test);
      // double add
      mocad_timer_add(ctx, head, &timer1, 1, mocad_timer_unittest_cb, cb_arg_test);
      mocad_timer_add(ctx, head, &timer1, 0, mocad_timer_unittest_cb, cb_arg_test);
      mocad_timer_add(ctx, head, &timer1, 2, mocad_timer_unittest_cb, cb_arg_test);

      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 1);

      // Expire none
      did_cb_run = 0;
      mocad_timer_process(ctx, head);
      TEST_ASSERT(did_cb_run == 0);
      TEST_ASSERT(*head == &timer1);

      // Expire single
      TIME_TICK();
      mocad_timer_process(ctx, head);
      TEST_ASSERT(did_cb_run == 1);
      TEST_ASSERT(*head == NULL);

      // Insert first (timer1)
      mocad_timer_add(ctx, head, &timer1, 3, mocad_timer_unittest_cb, cb_arg_test);
      TEST_ASSERT(*head == &timer1);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 3);
      // Insert at front (timer2->timer1)
      mocad_timer_add(ctx, head, &timer2, 1, mocad_timer_unittest_cb, cb_arg_test);
      TEST_ASSERT(*head == &timer2);
      // double add
      mocad_timer_add(ctx, head, &timer1, 0, mocad_timer_unittest_cb, cb_arg_test);

      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 1);
      // Insert middle (timer2->timer3->timer1)
      mocad_timer_add(ctx, head, &timer3, 2, mocad_timer_unittest_cb, cb_arg_test);
      TEST_ASSERT(*head == &timer2);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 1);
      // Delete last (timer2->timer3)
      mocad_timer_remove(ctx, head, &timer1);
      TEST_ASSERT(*head == &timer2);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 1);
      // Insert last (timer2->timer3->timer1)
      mocad_timer_add(ctx, head, &timer1, 3, mocad_timer_unittest_cb, cb_arg_test);
      TEST_ASSERT(*head == &timer2);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 1);
      // Delete front (timer3->timer1)
      mocad_timer_remove(ctx, head, &timer2);
      TEST_ASSERT(*head == &timer3);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 2);
      // Insert front (timer2->timer3->timer1)
      mocad_timer_add(ctx, head, &timer2, 1, mocad_timer_unittest_cb, cb_arg_test);
      TEST_ASSERT(*head == &timer2);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 1);
      // Delete middle (timer2->timer1)
      mocad_timer_remove(ctx, head, &timer3);
      TEST_ASSERT(*head == &timer2);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 1);
      // remove nonexisting!!!
      mocad_timer_remove(ctx, head, &timer3);
      TEST_ASSERT(*head == &timer2);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 1);
      // create full list again (timer2->timer3->timer1)
      mocad_timer_add(ctx, head, &timer3, 2, mocad_timer_unittest_cb, cb_arg_test);
      TEST_ASSERT(*head == &timer2);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 1);

      // Expire none
      mocad_timer_process(ctx, head);
      TEST_ASSERT(did_cb_run == 1);
      TEST_ASSERT(*head == &timer2);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 1);

      // Expire one
      TIME_TICK();
      mocad_timer_process(ctx, head);
      TEST_ASSERT(did_cb_run == 2);
      TEST_ASSERT(*head == &timer3);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 1);

      // Expire multiple and all
      TIME_TICK();
      TIME_TICK();
      mocad_timer_process(ctx, head);
      TEST_ASSERT(did_cb_run == 4);
      TEST_ASSERT(*head == NULL);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == MoCAOS_TIMEOUT_INFINITE);

      // Expire none
      TIME_TICK();
      mocad_timer_process(ctx, head);
      TEST_ASSERT(did_cb_run == 4);
      TEST_ASSERT(*head == NULL);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == MoCAOS_TIMEOUT_INFINITE);

      // re-add during expiry
      mocad_timer_add(ctx, head, &timer2, 1, mocad_timer_unittest_cb, cb_arg_test);
      TEST_ASSERT(*head == &timer2);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 1);

      cb_timer_to_add = &timer2;
      TIME_TICK();
      mocad_timer_process(ctx, head);
      TEST_ASSERT(did_cb_run == 5);
      TEST_ASSERT(*head == &timer2);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == 1);

      cb_timer_to_add = NULL;
      TIME_TICK();
      mocad_timer_process(ctx, head);
      TEST_ASSERT(did_cb_run == 6);
      TEST_ASSERT(*head == NULL);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == MoCAOS_TIMEOUT_INFINITE);

      // Expire none
      TIME_TICK();
      mocad_timer_process(ctx, head);
      TEST_ASSERT(did_cb_run == 6);
      TEST_ASSERT(*head == NULL);
      next = mocad_timer_next(ctx, head);
      TEST_ASSERT(next == MoCAOS_TIMEOUT_INFINITE);

      // Wrap tests
      mocad_timer_unittest_time = 0 - ticks_until_wrap;
      ticks_until_wrap--;
   }

   TIMER_DBG("\nMOCAD_TIMER_UNITTEST finished!\n");
   exit(0);
}
#endif
