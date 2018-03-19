/*
#
#  Copyright 2011, Broadcom Corporation
#
# <:label-BRCM:2011:proprietary:standard
# 
#  This program is the proprietary software of Broadcom and/or its
#  licensors, and may only be used, duplicated, modified or distributed pursuant
#  to the terms and conditions of a separate, written license agreement executed
#  between you and Broadcom (an "Authorized License").  Except as set forth in
#  an Authorized License, Broadcom grants no license (express or implied), right
#  to use, or waiver of any kind with respect to the Software, and Broadcom
#  expressly reserves all rights in and to the Software and all intellectual
#  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
#  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
#  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
# 
#  Except as expressly set forth in the Authorized License,
# 
#  1. This program, including its structure, sequence and organization,
#     constitutes the valuable trade secrets of Broadcom, and you shall use
#     all reasonable efforts to protect the confidentiality thereof, and to
#     use this information only in connection with your use of Broadcom
#     integrated circuit products.
# 
#  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
#     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
#     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
#     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
#     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
#     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
#     PERFORMANCE OF THE SOFTWARE.
# 
#  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
#     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
#     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
#     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
#     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
#     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
#     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
#     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
#     LIMITED REMEDY.
# :>
*/

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <signal.h>


#include "thrd_binder.h"



struct thread_info
{
   char *name;
   int pid;
};

/** these are the threads which we will dynamically bind or unbind to CPU0 */
struct thread_info thread_array[] = {{"wl0-kthrd", 0},
                                     {"skbFreeTask", 0},
                                     {NULL, 0}
                                    };




#define TB_STATE_ALLUNBOUND    0
#define TB_STATE_ALLBOUND      1

/** our current state.  Currently only 2 states are defined, but the
 * structure of this program allows for many more states.
 * All state numbers must be sequential and starting from 0 because they
 * will be put into an array (see sm_node_array below).
 */
int curr_state=TB_STATE_ALLBOUND;


/** each state in the state machine must implement a "check" and "enter"
 * function.
 * The "check" function decides whether to stay in the current state or
 * enter another state.  It always returns a state value (which could be
 * the same as the current state.)
 * The "enter" function configures the system for the new state.
 */
static void enter_allUnbound(void);
static int  check_allUnbound(void);

static void enter_allBound(void);
static int  check_allBound(void);


struct tb_sm_node {
   void (*enter_func)();
   int (*check_func)();
};

struct tb_sm_node sm_node_array[] = {
                       {enter_allUnbound, check_allUnbound},
                       {enter_allBound,   check_allBound},
                                    };




/* See comments in thrd_binder.h */
int cpu_util_array[TB_NUM_CPUS][TB_NUM_HISTORY_SLOTS];

static void init()
{
   int i=0;

   /* Run in background and ignore SIGINT.  To kill, type:
    * killall bcm_thrdBinder
    */
   daemon(1, 1);
   signal(SIGINT, SIG_IGN);

   memset(cpu_util_array, 0, sizeof(cpu_util_array));

   /* get pids for the threads we want to bind/unbind
    * (also good place for startup msg)
    */
   printf("%s starting: managing { ", TB_PROG_NAME);
   while (thread_array[i].name != NULL)
   {
      thread_array[i].pid = get_pidByName(thread_array[i].name);
      if (thread_array[i].pid > 0)
      {
         printf("%s(%d) ", thread_array[i].name, thread_array[i].pid);
      }
      i++;
   }
   printf("}\n");

   /* increase my own prio so I can always run even when system is very busy */
   {
      struct sched_param sp;
      memset(&sp, 0, sizeof(sp));
      sp.sched_priority = 99;

      if (sched_setscheduler(getpid(), SCHED_RR, &sp) == -1)
      {
         printf("%s: could not increase my own priority\n", TB_PROG_NAME);
      }
   }

   /* enter initial state */
   sm_node_array[curr_state].enter_func();
}


int main()
{

   init();

   while(1)
   {
      int new_state;

      /*
       * Always update our snapshot of CPU util so the check functions below
       * are using the latest info to make their decisions.
       */
      update_cpuUtil();

      /* Check if we should remain in our current state */
      new_state = sm_node_array[curr_state].check_func();

      if (new_state != curr_state)
      {
         sm_node_array[new_state].enter_func();
         curr_state = new_state;
      }

      sleep(TB_POLL_INTERVAL);
   }
}



static void enter_allUnbound(void)
{
   int i=0;

   printf("[%s] %s Enter all UNBOUND state { ", get_timestamp(), TB_PROG_NAME);

   while (thread_array[i].name != NULL)
   {
      if (thread_array[i].pid > 0)
      {
         unbind_thread(thread_array[i].pid);
         printf("%s(%d) ", thread_array[i].name, thread_array[i].pid);
      }
      i++;
   }

   printf("} \n");
}

static int  check_allUnbound(void)
{
   int i;

#ifdef DEBUG
   printf("currently in all UNBOUND, checking.....\n");
#endif

   for (i=0; i < TB_NUM_HISTORY_SLOTS; i++)
   {
      /* if any CPU0 history slot is above the TB_BIND_THRESH, then CPU0 is
       * still probably too busy to bind all threads to it.  Remain in the
       * UNBOUND STATE.
       */
      if (cpu_util_array[0][i] >= TB_BIND_THRESH)
      {
         return TB_STATE_ALLUNBOUND;
      }
   }

   /* if we get here, then all CPU0 history slots are below the TB_BIND_THRESH,
    * so that means CPU0 is relatively idle, so bind all threads to CPU0
    */
   return TB_STATE_ALLBOUND;
}

static void enter_allBound(void)
{
   int i=0;

   printf("[%s] %s Enter all BOUND state { ", get_timestamp(), TB_PROG_NAME);

   while (thread_array[i].name != NULL)
   {
      if (thread_array[i].pid > 0)
      {
         bind_thread(thread_array[i].pid);
         printf("%s(%d) ", thread_array[i].name, thread_array[i].pid);
      }
      i++;
   }

   printf("} \n");
}

static int  check_allBound(void)
{
#ifdef DEBUG
   printf("currently in all BOUND, checking.....\n");
#endif

   /* When in the ALLBOUND state, we only need to check the most recent
    * CPU util.  If at or above the UNBIND_THRESH, then unbind all threads.
    */
   if (cpu_util_array[0][0] >= TB_UNBIND_THRESH)
   {
      return TB_STATE_ALLUNBOUND;
   }

   /* if we get here, then CPU0 is not close to fully busy, so stay all bound */
   return TB_STATE_ALLBOUND;
}

