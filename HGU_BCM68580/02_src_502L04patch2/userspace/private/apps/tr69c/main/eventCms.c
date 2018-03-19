/***********************************************************************
 *
 *  Copyright (c) 2017  Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2017:proprietary:standard

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
 *
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
/*#ifdef linux */
#include <sys/select.h>
/*#endif  */
/* #ifdef __NetBSD__  */
#include <sys/types.h>
#include <unistd.h>
/* #endif   */
#include <sys/time.h>
#include <string.h>

/* #define USE_SYSINFO */
#if defined(linux) && defined(USE_SYSINFO)
#include <sys/sysinfo.h>
#endif
#include <syslog.h>

#include "cms_util.h"
#include "inc/utils.h"
#include "event.h"
#include "informer_public.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

extern ACSState       acsState;

extern int tr69cTerm;            /* defined in main.c */
extern void main_cleanup(SINT32 code);  /* defined in main.c */


/*#define DEBUG*/
#undef DEBUG
/*#define DEBUG_TIMER */
/*#define DEBUG_LISTENER*/
/*#define DEBUG_IDLE*/
/*#define DEBUG_CALLBACK */

#ifdef DEBUG
#ifndef DEBUG_TIMER
#define DEBUG_TIMER
#endif
#ifndef DEBUG_LISTENER
#define DEBUG_LISTENER
#endif
#ifndef DEBUG_CALLBACK
#define DEBUG_CALLBACK
#endif
#ifndef DEBUG_IDLE
#define DEBUG_IDLE
#endif
#endif

extern Listener      *listeners;

void eventLoop(void)
{
   int            i, n;
   int            res;
   fd_set         rfds, wfds, efds;
   struct timeval tm;
   Listener       *l;
   UINT32         numEvents;
   UINT32         nextEventMs;  /* number of milli-seconds until next event */
   UINT32         lastSelectTimeout;  /* remember the approx timeout val. in seconds */
   UBOOL8         keepLooping=TRUE;
   CmsRet         ret;

   numEvents = cmsTmr_getNumberOfEvents(tmrHandle);
   cmsLog_notice("entering eventLoop with %u events, listeners=%p", numEvents, listeners);

   if (numEvents > 0)
   {
      cmsTmr_dumpEvents(tmrHandle);
   }

   
   while (keepLooping)
   {
      /*
       * Set the select fds structures based on the registered listeners.
       */
      n = 0;
      FD_ZERO(&rfds);
      FD_ZERO(&wfds);
      FD_ZERO(&efds);
      for (l = listeners; l; l = l->next)
      {
         /* paranoid check */
         if ((l->fd < MIN_LISTENER_FD) || (l->fd > MAX_LISTENER_FD))
         {
            cmsLog_error("Don't set fd=%d", l->fd);
            continue;
         }

         switch (l->type)
         {
            case iListener_Read:
               FD_SET(l->fd, &rfds);
               break;
            case iListener_Write:
               FD_SET(l->fd, &wfds);
               break;
            case iListener_ReadWrite:
               FD_SET(l->fd, &rfds);
               FD_SET(l->fd, &wfds);
               break;
            case iListener_Except:
               FD_SET(l->fd, &efds);
               break;
            default:
               cmsLog_error("Impossible error: illegal listener type (%d)", l->type);
               break;
         }
         if (n <= l->fd)
         {
            n = l->fd + 1;
         }
      }


      /*
       * Set our select timeout based on time to next event 
       * in our timer handle.
       */
      if ((ret = cmsTmr_getTimeToNextEvent(tmrHandle, &nextEventMs)) == CMSRET_NO_MORE_INSTANCES)
      {
         cmsLog_debug("no more events, set timeout to %d (MAX)", TR69C_EXIT_ON_IDLE_TIMEOUT);
         tm.tv_sec  = TR69C_EXIT_ON_IDLE_TIMEOUT;
         tm.tv_usec = 0;

         if (tr69cTerm)
         {
            cmsLog_debug("no more events and tr69cTerm is set, calling main_cleanup");
            main_cleanup(0);
         }
      }
      else
      {
         const UINT32 sched_lag_time=10;

         /* stole this little piece of code from oal_event.c */
         /* add another 10ms to the next scheduled event so that by the time
          * we wake up, it will definately be time for that event to execute. */
         if (nextEventMs < UINT32_MAX - sched_lag_time)
         {
            nextEventMs += sched_lag_time;
         }

         tm.tv_sec = nextEventMs / MSECS_IN_SEC;
         tm.tv_sec = (tm.tv_sec > TR69C_EXIT_ON_IDLE_TIMEOUT) ? TR69C_EXIT_ON_IDLE_TIMEOUT : tm.tv_sec;
         tm.tv_usec = (nextEventMs % MSECS_IN_SEC) * USECS_IN_MSEC;

         cmsLog_debug("set select timeout to %ld.%ld secs",
                      tm.tv_sec, (tm.tv_usec / USECS_IN_MSEC));
      }

      lastSelectTimeout = tm.tv_sec;


      /*
       * Now call select() using the info we constructed in the two
       * previous blocks of code.
       */
      res = select(n, &rfds, &wfds, &efds, &tm);

      if (res < 0 && errno != EINTR)
      {
         cmsLog_error("Error in select (%d=%s)", errno, strerror(errno));
         continue;
      }
      else if (res < 0 && errno == EINTR)
      {
         /* interrupted by some signal, just go do select again. */
         continue;
      }


      /*
       * service all timer events that are due (there may be no events due
       * if we woke up from select because of activity on the fds).
       */
      cmsTmr_executeExpiredEvents(tmrHandle);


      /* Now service the listener events, if any (res may be 0) */
      for (i = 0; i < res; ++i)
      {
         /* this is to keep trying until the number of fd returned from*/
         /* the select are processed */
         for (l = listeners; l; l = l->next)
         {
            if (FD_ISSET(l->fd, &rfds) || FD_ISSET(l->fd, &wfds) || FD_ISSET(l->fd, &efds))
            {
               /* Clear the fd in the fdset so we don't call the func again */
               FD_CLR(l->fd, &rfds);
               FD_CLR(l->fd, &wfds);
               FD_CLR(l->fd, &efds);

               l->func(l->handle);    /* l is invalid following this call */
               break;
            }
         }
      }

     /*
      * Decide whether to exit this loop, and as a result tr69c,
      * based on the reasoning below:
      *     (1) If the next timer is due before TR69C_EXIT_ON_IDLE_TIMEOUT
      *         then either this session is not complete or the next periodic
      *         inform timer is about to due in the near future. For this case,
      *         we want to keep looping.
      *     (2) If the next timer is not due within TR69C_EXIT_ON_IDLE_TIMEOUT,
      *         we would assume that all the transactions with ACS in this
      *         session are complete, and the next timer should be a periodic
      *         inform timer which is due in the far future. For this case,
      *         we would request for a wake-up call from SMD and exit tr69c.
      * (mwang: what if a timer event in the tmrHandle is not for periodic 
      *  inform?  I think a better algorithm for this case would be, if 
      *  the only event in the tmrHandle is a periodic inform and it is due
      *  at a time in the future that is greater than TR69C_EXIT_ON_IDLE_TIMEOUT,
      *  then request a delayed msg from smd and exit.  Otherwise, do not exit and 
      *  wait around long enough to service this other event.
      *
      *     (3) If there is no timer in the timer queue, we would assume that
      *         periodic inform is probably disabled. For this case, we would
      *         exit tr69c.
      */
      if ((res == 0) && (lastSelectTimeout == TR69C_EXIT_ON_IDLE_TIMEOUT))
      {
         cmsLog_debug("select timed out using max value (%d secs)", lastSelectTimeout);
         /*
          * Original code also had some consideration of listeners.
          * We might want to go through the listener array and verify that
          * we don't have any connections other than our listening
          * server socket.
          */

         ret = cmsTmr_getTimeToNextEvent(tmrHandle, &nextEventMs);
         if (ret == CMSRET_NO_MORE_INSTANCES)
         {
        
            /* there is a small window when retry timer is off the event list
               for a brief time, we exit... */
            if (acsState.retryCount == 0)
            {
                /* this is case 3 in the comments above. */
               cmsLog_debug("no more timer events, prepare to exit (on nothing to do)");
               keepLooping = FALSE;
            }
	    else
            {
               cmsLog_debug("retryCount != 0, continue in loop");
            }
         }
         else
         {
            cmsTmr_dumpEvents(tmrHandle);

            numEvents = cmsTmr_getNumberOfEvents(tmrHandle);
            if ((numEvents == 1) &&
                (cmsTmr_isEventPresent(tmrHandle, periodicInformTimeout, NULL)) &&
                (nextEventMs > TR69C_EXIT_ON_IDLE_TIMEOUT))
            {
               /* this is case 2 in the comments above. */
               cmsLog_debug("only 1 event left and it is periodic inform, exit-on-idle");
               requestPeriodicInform(nextEventMs / MSECS_IN_SEC);
               keepLooping = FALSE;
            }
            else
            {
               /* case 1 in the comments above. */
               cmsLog_debug("more work to do, continue in loop");
            }
         }
      }

   }   /* end of while (keepLooping) */

   cmsLog_notice("exiting from bottom of function");

   return;
}

