/*
    <:copyright-BRCM:2013:DUAL/GPL:standard
    
       Copyright (c) 2013 Broadcom 
       All Rights Reserved
    
    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:
    
       As a special exception, the copyright holders of this software give
       you permission to link this software with independent modules, and
       to copy and distribute the resulting executable under terms of your
       choice, provided that you also meet, for each linked independent
       module, the terms and conditions of the license of that module.
       An independent module is a module which is not derived from this
       software.  The special exception does not apply to any modifications
       of the software.
    
    Not withstanding the above, under no circumstances may you combine
    this software in any way with any other Broadcom software provided
    under a license other than the GPL, without Broadcom's express prior
    written consent.
    
:>
*/


/*
 * RDPA utilities
 */

#ifndef _RDPA_UTILS_H_
#define _RDPA_UTILS_H_

#include "rdd_defs.h"
#include "rdd_utils.h"

/* 
 * Macro utilities for reading from trace buffer 
 * You pass in a 16-bit or 32-bit pointer to these macros
 */
#ifdef RUNNER_FWTRACE_32BIT
        /* The layout of the buffer in 32-bit mode is:
                 Word 0: bits 15:0 - Event Num index0, bits 31:16 - Thread Num index0
                 Word 1: Time Count for index0
                 Word 2: bits 15:0  - Event Num index1, bits 31:16 - Thread Num index1
                 Word 3: Time Count for index1
                 etc
             */
        #define RDPA_FWTRACE_READ_TIME_CNTR(x) ((ntohl(x[i+1]) & 0x0FFFFFFF))
        #define RDPA_FWTRACE_READ_EVENT(x) (ntohl(x[i]) & 0xFFFF)
        #define RDPA_FWTRACE_READ_THREAD(x) ((ntohl(x[i]) & 0xFF0000) >> 16)
#else
        /* The layout of the buffer in 16-bit mode is:
                 Word 0: bits 15:0 - Time Count for index0.  bits 23:16 Event Num for index 0, bits 31:24 Thread Num for index 0
                 Word 1: bits 15:0 - Time Count for index1.  bits 23:16 Event Num for index 1, bits 31:24 Thread Num for index 1
                 etc
             */
        #define RDPA_FWTRACE_READ_TIME_CNTR(x) (ntohl(x[i]) & 0xFFFF)
        #define RDPA_FWTRACE_READ_EVENT(x) ((ntohl(x[i]) & 0xFF0000) >> 16)
        #define RDPA_FWTRACE_READ_THREAD(x) ((ntohl(x[i]) & 0xFF000000) >> 24)
#endif


/** Clear the FW Trace
 */
void rdpa_fwtrace_clear(void);

/** FW Enable or Disable
 * \param[in]   enable      1 to enable FW Trace, 0 to disable
 * \returnVal    0 on success, -1 on failure
 */
int rdpa_fwtrace_enable_set(uint32_t enable);

/** Read the last FW Trace for a given processor
 * \param[in]   runner_id      Runner processor's FW Trace to read
 * \param[out]   trace_length - function will populate pointer data with trace length
 * \param[out]   trace_buffer - function will populate with trace buffer
 * \returnVal    0 on success, -1 on failure
 */
int rdpa_fwtrace_get(LILAC_RDD_RUNNER_INDEX_DTS runner_id,
                            uint32_t *trace_length,
                            uint32_t *trace_buffer);

/** Get Thread Name from Runner A
 * \param[in]   threadId    Numerical thread ID 
 * \param[out]   pName       pointer to location to return thread name
 * \returnVal    0 on success, -1 on failure
 */
int rdpa_fwtrace_rnr_a_thread_name_get(int thread_id, char *p_mame);

/** Get Thread Name from Runner B
 * \param[in]   threadId    Numerical thread ID 
 * \param[out]   pName       pointer to location to return thread name
 * \returnVal    0 on success, -1 on failure
 */
int rdpa_fwtrace_rnr_b_thread_name_get(int thread_id, char *p_mame);

/** Get FW Trace Event Name from eventId
 * \param[in]   eventId    Numerical event ID
 * \param[out]   pName     pointer to location to return event name
 * \returnVal    0 on success, -1 on failure
 */
int rdpa_fwtrace_event_name_get(int event_id, char *p_name);

#endif
