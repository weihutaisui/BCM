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


/** how often do we check CPU utilization, in seconds */
#define TB_POLL_INTERVAL      5

/** how many CPU utilization values we keep per CPU.
 *  So if POLL_INTERVAL is 5 seconds, and number of slots is 12, then
 *  we keep about a 1 minute history of CPU utilization.  The length of
 *  the history provides hysteresis for deciding when to rebind all the
 *  threads.
 */
#define TB_NUM_HISTORY_SLOTS  12


/** The threshold CPU0 utilization value which will trigger unbinding of
 * the threads.  So if this is 95, then we will unbind the threads as soon
 * as we detect CPU0 utilization at or above 95%.
 */
#define TB_UNBIND_THRESH      95


/** If we see that CPU0 utilization is below this number for the entire
 *  history of the utilizations, then bind all the threads to CPU0.
 */
#define TB_BIND_THRESH        40



#define TB_NUM_CPUS           2

#define TB_PROG_NAME          "bcm_thrdBinder"



/* This is an array of pointers to int arrays.  So:
 * [0] -> array of TB_NUM_HISTORY_SLOTS of ints (% cpu busy)
 * [1] -> array of TB_NUM_HISTORY_SLOTS of ints (% cpu busy)
 */
extern int cpu_util_array[TB_NUM_CPUS][TB_NUM_HISTORY_SLOTS];




/* in thrd_utils.c */
void update_cpuUtil(void);
void bind_thread(int pid);
void unbind_thread(int pid);
int get_pidByName(const char *name);
const char *get_timestamp(void);

