/*
 Copyright 2011, Broadcom Corporation
 <:label-BRCM:2015:proprietary:standard
 
  This program is the proprietary software of Broadcom and/or its
  licensors, and may only be used, duplicated, modified or distributed pursuant
  to the terms and conditions of a separate, written license agreement executed
  between you and Broadcom (an "Authorized License").  Except as set forth in
  an Authorized License, Broadcom grants no license (express or implied), right
  to use, or waiver of any kind with respect to the Software, and Broadcom
  expressly reserves all rights in and to the Software and all intellectual
  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 
  Except as expressly set forth in the Authorized License,
 
  1. This program, including its structure, sequence and organization,
     constitutes the valuable trade secrets of Broadcom, and you shall use
     all reasonable efforts to protect the confidentiality thereof, and to
     use this information only in connection with your use of Broadcom
     integrated circuit products.
 
  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
     PERFORMANCE OF THE SOFTWARE.
 
  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
     LIMITED REMEDY.
 :>
 */


#include "timer_manager.h"
#include "cms_log.h"
#include <errno.h>
#include <string.h>
#include <sys/timerfd.h>

static UINT32 timer_manager_systick();
static void timer_enqueue(struct timer_agent *timer);
static void timer_dequeue(struct timer_agent *timer);

static struct timer_agent *timer_list_head;

void timer_manager_init()
{
    timer_list_head = NULL;
}

/* The interval is in milli seconds. 1 second == 1000 milli seconds. */
UBOOL8 timer_manager_add(int interval, struct internal_message* result)
{
    struct timer_agent *new_timer;
    struct itimerspec newtimespec;
    int fd;

    if(interval == 0)
    {
        cmsLog_error("Invalid interval!");
        return FALSE;
    }

    new_timer = malloc(sizeof(struct timer_agent));
    if(new_timer == NULL)
    {
        cmsLog_error("Fatal error when creating timer_agent!");
        return FALSE;
    }
    new_timer->expiry = timer_manager_systick() + interval;
    new_timer->interval = interval;
    new_timer->result = result;
    
    fd = timerfd_create(CLOCK_MONOTONIC, 0);
    
    if(fd != -1)
    {
        newtimespec.it_interval.tv_sec = 0;
        newtimespec.it_interval.tv_nsec = 0;
        /* conversion from ms to s */
        newtimespec.it_value.tv_sec = interval / 1000;
        /* conversion from ms to ns */
        newtimespec.it_value.tv_nsec = (interval % 1000) * 1000 * 1000;    
        if(0 == timerfd_settime(fd, 0, &newtimespec, NULL))
        { 
            cmsLog_debug("new fd = %d", fd);
            cmsLog_debug("Created a new timer with interval %d msec", interval);
            new_timer->my_fd = fd;
            timer_enqueue(new_timer);   
            return TRUE;
        }
        
    } 
    
   /* 
    * We are here because of some failures. Need to free the allocated data
    * and return FALSE.
    */ 

    if(new_timer != NULL)
    {
        cmsLog_error("Failed to create timer for client!");
        timer_manager_delete(new_timer);
        new_timer = NULL;
    }

    return FALSE;
}

UBOOL8 timer_manager_delete(struct timer_agent* timer)
{
    struct itimerspec oldtimespec;
    struct itimerspec newtimespec;

    if(0 == timerfd_gettime(timer->my_fd, &oldtimespec))
    {
        newtimespec.it_interval.tv_sec = 0;
        newtimespec.it_interval.tv_nsec = 0;
        newtimespec.it_value.tv_sec = 0;
        newtimespec.it_value.tv_nsec = 0;
        if(0 == timerfd_settime(timer->my_fd, TFD_TIMER_ABSTIME, 
                                &newtimespec, NULL))
        {
            timer_dequeue(timer);
            close(timer->my_fd); 
            free(timer); 
        }    
        else
        {
            cmsLog_error("failed to delete timer!");
            return FALSE;
        }
    } 

    return TRUE;
}


static UBOOL8 timer_less_than(UINT32 t1, UINT32 t2)
{
    return (t1-t2)>=0x80000000;
}

static void timer_enqueue(struct timer_agent *timer)
{
    struct timer_agent* p;
    struct timer_agent* q;

    q = NULL;
    p = timer_list_head;
    while ((p != NULL) && timer_less_than(p->expiry, timer->expiry))
    {
        q = p;
        p = p->next;
    }

    // insert in front of p
    timer->next = p;
    if (q != NULL)
    {
        q->next = timer;
    }
    else
    {
        timer_list_head = timer;
    }
}

static void timer_dequeue(struct timer_agent *timer)
{
    struct timer_agent *p;
    struct timer_agent *q;

    q = NULL;
    p = timer_list_head;
    while((p != NULL) && (p != timer))
    {
        q = p;
        p = p->next;
    }

    if(p != NULL)
    {
        /* Found it */
        if(q != NULL)
        {
            q->next = timer->next;
        } 
        else
        {
            timer_list_head = timer->next;
        }
    }
}

static UINT32 timer_manager_systick()
{
    UINT32 ms;
    struct timespec current;
    if(clock_gettime(CLOCK_MONOTONIC, &current) == -1)
    {
        cmsLog_error("Failed to get current time!");
        return 0;
    } 
    ms = (current.tv_sec*1000) + (current.tv_nsec/1000000);
    return ms;
}

/* Calculate the fd set for timers in the locally maintained list. */
int timer_manager_init_fdset(fd_set *pReadFds)
{
    struct timer_agent *q;
    struct timer_agent *p = timer_list_head;
    int max_fd = 0;

    while (p != NULL)
    {
        q = p;
        p = p->next;
        
        if(q->my_fd > max_fd)
        {
            max_fd = q->my_fd;
        }

        FD_SET(q->my_fd, pReadFds);
    }
    return max_fd;
}

/*
 * Return the first found expired timer's internal_message for further 
 * processing. The timer found would be removed after the operation.
 */
struct internal_message* timer_manager_check_expiry(fd_set *pReadFds)
{
    struct timer_agent *p, *q;
    UINT64 expire_count;

    q = timer_list_head;
    
    while(q != NULL)
    {
        p = NULL;
        if(FD_ISSET(q->my_fd, pReadFds))
        {
            read(q->my_fd, &expire_count, sizeof(UINT64)); 
            p = q;               
        }
        
        if(p != NULL)
        {
            struct internal_message *result = p->result;
            timer_manager_delete(p);
            p = NULL;
            return result;
        }
        else
        {
            q = q->next;       
        }
    } 
    return NULL;
}
