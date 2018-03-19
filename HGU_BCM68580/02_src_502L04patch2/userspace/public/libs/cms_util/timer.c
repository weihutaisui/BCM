/***********************************************************************
 *
 *  Copyright (c) 2006-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:DUAL/GPL:standard

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
 *
************************************************************************/

#include "cms.h"
#include "cms_util.h"



/** Internal event timer structure
 */
typedef struct cms_timer_event
{
   struct cms_timer_event *next;      /**< pointer to the next timer. */
   CmsTimestamp            expireTms; /**< Timestamp (in the future) of when this
                                       *   timer event will expire. */
   CmsEventHandler         func;      /**< handler func to call when event expires. */
   void *                  ctxData;   /**< context data to pass to func */
   char name[CMS_EVENT_TIMER_NAME_LENGTH]; /**< name of this timer */
   CmsTimestamp            pausedTms; /**< Timestamp of when this timer was paused */
} CmsTimerEvent;


/** Internal timer handle. */
typedef struct
{
   CmsTimerEvent *events;     /**< Singly linked list of events */
   UINT32         numEvents;  /**< Number of events in this handle. */
} CmsTimerHandle;


CmsRet cmsTmr_init(void **tmrHandle)
{

   (*tmrHandle) = cmsMem_alloc(sizeof(CmsTimerHandle), ALLOC_ZEROIZE);
   if ((*tmrHandle) == NULL)
   {
      cmsLog_error("could not malloc mem for tmrHandle");
      return CMSRET_RESOURCE_EXCEEDED;
   }

   return CMSRET_SUCCESS;
}


void cmsTmr_cleanup(void **handle)
{
   CmsTimerHandle *tmrHandle = (CmsTimerHandle *) handle;
   CmsTimerEvent *tmrEvent;

   while ((tmrEvent = tmrHandle->events) != NULL)
   {
      tmrHandle->events = tmrEvent->next;
      CMSMEM_FREE_BUF_AND_NULL_PTR(tmrEvent);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR((*handle));

   return;
}

/** This macro will evaluate TRUE if a is earlier than b */
#define IS_EARLIER_THAN(a, b) (((a)->sec < (b)->sec) || \
                               (((a)->sec == (b)->sec) && ((a)->nsec < (b)->nsec)))


void _cmsTmr_insert(CmsTimerHandle *tmrHandle, CmsTimerEvent *newEvent)
{
   CmsTimerEvent *currEvent, *prevEvent;
   /* 
    * Now we just need to insert it in the correct place in the timer handle.
    * We just insert the events in absolute order, i.e. smallest expire timer
    * at the head of the queue, largest at the end of the queue.  If the
    * modem is up long enough where timestamp rollover is an issue (139 years!)
    * cmsTmr_executeExpiredEvents and cmsTmr_getTimeToNextEvent will have to
    * be careful about where they pick the next timer to expire.
    */
   if (tmrHandle->numEvents == 0)
   {
      tmrHandle->events = newEvent;
   }
   else 
   {
      currEvent = tmrHandle->events;

      if (IS_EARLIER_THAN(&(newEvent->expireTms), &(currEvent->expireTms)) ||
         !((currEvent->pausedTms.sec == 0) && (currEvent->pausedTms.nsec == 0)))
      {
         /* queue at the head */
         newEvent->next = currEvent;
         tmrHandle->events = newEvent;
      }
      else
      {
         UBOOL8 done = FALSE;

         while (!done)
         {
            prevEvent = currEvent;
            currEvent = currEvent->next;

            if ((currEvent == NULL) ||
                (IS_EARLIER_THAN(&(newEvent->expireTms), &(currEvent->expireTms))) ||
                !((currEvent->pausedTms.sec == 0) && (currEvent->pausedTms.nsec == 0)))
            {
               newEvent->next = prevEvent->next;
               prevEvent->next = newEvent;
               done = TRUE;
            }
         }
      }
   }

   tmrHandle->numEvents++;

   return;
}


CmsRet cmsTmr_set(void *handle, CmsEventHandler func, void *ctxData, UINT32 ms, const char *name)
{
   CmsTimerHandle *tmrHandle = (CmsTimerHandle *) handle;
   CmsTimerEvent *newEvent;

   /*
    * First verify there is not a duplicate event.
    * (The original code first deleted any existing timer,
    * which is a "side-effect", bad style, but maybe tr69c requires
    * that functionality?)
    */
   if (cmsTmr_isEventPresent(handle, func, ctxData))
   {
      cmsLog_error("There is already an event func 0x%p ctxData 0x%p",
                   func, ctxData);
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* make sure name is not too long */
   if ((name != NULL) && (strlen(name) >= CMS_EVENT_TIMER_NAME_LENGTH))
   {
      cmsLog_error("name of timer event is too long, max %d", CMS_EVENT_TIMER_NAME_LENGTH);
      return CMSRET_INVALID_ARGUMENTS;
   }


   /*
    * Allocate a structure for the timer event.
    */
   newEvent = cmsMem_alloc(sizeof(CmsTimerEvent), ALLOC_ZEROIZE);
   if (newEvent == NULL)
   {
      cmsLog_error("malloc of new timer event failed");
      return CMSRET_RESOURCE_EXCEEDED;
   }

   /* fill in fields of new event timer structure. */
   newEvent->func = func;
   newEvent->ctxData = ctxData;

   cmsTms_get(&(newEvent->expireTms));
   cmsTms_addMilliSeconds(&(newEvent->expireTms), ms);

   if (name != NULL)
   {
      sprintf(newEvent->name, "%s", name);
   }

   /* Insert the new event timer in the timer handle */
   _cmsTmr_insert(tmrHandle, newEvent);

   cmsLog_debug("added event %s, expires in %ums (at %u.%03u), func=0x%x data=%p count=%d",
                newEvent->name,
                ms,
                newEvent->expireTms.sec,
                newEvent->expireTms.nsec/NSECS_IN_MSEC,
                func,
                ctxData,
                tmrHandle->numEvents);

   return CMSRET_SUCCESS;
}  


CmsTimerEvent * _cmsTmr_remove(CmsTimerHandle *tmrHandle, CmsEventHandler func, void *ctxData)
{
   CmsTimerEvent *currEvent, *prevEvent;

   if ((currEvent = tmrHandle->events) == NULL)
   {
      return NULL;
   }

   if (currEvent->func == func && currEvent->ctxData == ctxData)
   {
      /* remove from head of the queue */
      tmrHandle->events = currEvent->next;
      currEvent->next = NULL;
   }
   else
   {
      UBOOL8 done = FALSE;

      while ((currEvent != NULL) && (!done))
      {
         prevEvent = currEvent;
         currEvent = currEvent->next;

         if (currEvent != NULL && currEvent->func == func && currEvent->ctxData == ctxData)
         {
            prevEvent->next = currEvent->next;
            currEvent->next = NULL;
            done = TRUE;
         }
      }
   }

   if (currEvent != NULL)
   {
      tmrHandle->numEvents--;
   }

   return currEvent;
}


void cmsTmr_cancel(void *handle, CmsEventHandler func, void *ctxData)
{
   CmsTimerHandle *tmrHandle = (CmsTimerHandle *) handle;
   CmsTimerEvent *currEvent;

   if ((currEvent = _cmsTmr_remove(tmrHandle, func, ctxData)) == NULL)
   {
      cmsLog_debug("no events to cancel (func=0x%x data=%p)", func, ctxData);
      return;
   }

   cmsLog_debug("canceled event %s, count=%d", currEvent->name, tmrHandle->numEvents);

   CMSMEM_FREE_BUF_AND_NULL_PTR(currEvent);

   return;
}


CmsRet cmsTmr_pause(void *handle, CmsEventHandler func, void *ctxData)
{
   CmsTimerHandle *tmrHandle = (CmsTimerHandle *) handle;
   CmsTimerEvent *currEvent, *newEvent;

   /* First take the event out of the queue */
   if ((newEvent = _cmsTmr_remove(tmrHandle, func, ctxData)) == NULL)
   {
      cmsLog_debug("no events to pause (func=0x%x data=%p)", func, ctxData);
      return CMSRET_RESOURCE_NOT_CONFIGURED;
   }

   /* Pause it if it was not already paused */
   if ((newEvent->pausedTms.sec == 0) && (newEvent->pausedTms.nsec == 0))
   {
      cmsTms_get(&(newEvent->pausedTms));
   }

   /* Add it to the end of the queue because it is paused */
   if (tmrHandle->numEvents == 0)
   {
      tmrHandle->events = newEvent;
   }
   else 
   {
      currEvent = tmrHandle->events;

      while (currEvent->next != NULL)
      {
         currEvent = currEvent->next;
      }
      currEvent->next = newEvent;
   }
   tmrHandle->numEvents++;

   cmsLog_debug("paused event %s, count=%d", newEvent->name, tmrHandle->numEvents);

   return CMSRET_SUCCESS;
}


CmsRet cmsTmr_resume(void *handle, CmsEventHandler func, void *ctxData)
{
   CmsTimerHandle *tmrHandle = (CmsTimerHandle *) handle;
   CmsTimerEvent *currEvent;
   UINT32 ms;

   /* First take the event out of the queue */
   if ((currEvent = _cmsTmr_remove(tmrHandle, func, ctxData)) == NULL)
   {
      cmsLog_debug("no events to pause (func=0x%x data=%p)", func, ctxData);
      return CMSRET_RESOURCE_NOT_CONFIGURED;
   }

   /* Calculate the new expiry delay */
   if ((currEvent->pausedTms.sec != 0) || (currEvent->pausedTms.nsec != 0))
   {
      if (IS_EARLIER_THAN(&(currEvent->expireTms), &(currEvent->pausedTms)))
      {
         cmsTms_get(&(currEvent->expireTms));
      }
      else
      {
         ms = cmsTms_deltaInMilliSeconds(&currEvent->expireTms, &currEvent->pausedTms);
         cmsTms_get(&(currEvent->expireTms));
         cmsTms_addMilliSeconds(&(currEvent->expireTms), ms);
      }
      currEvent->pausedTms.sec = currEvent->pausedTms.nsec = 0;
   }

   /* Resume it */
   _cmsTmr_insert(tmrHandle, currEvent);
   cmsLog_debug("resumed event %s, count=%d", currEvent->name, tmrHandle->numEvents);

   return CMSRET_SUCCESS;
}


CmsRet cmsTmr_getTimeToNextEvent(const void *handle, UINT32 *ms)
{
   CmsTimerHandle *tmrHandle = (CmsTimerHandle *) handle;
   CmsTimerEvent *currEvent;
   CmsTimestamp nowTms;

   cmsTms_get(&nowTms);
   currEvent = tmrHandle->events;

   if (currEvent == NULL)
   {
      *ms = MAX_UINT32;
      return CMSRET_NO_MORE_INSTANCES;
   }

   /* this is the same code as in dumpEvents, integrate? */
   if (IS_EARLIER_THAN(&(currEvent->expireTms), &nowTms))
   {
      /*
       * the next event is past due (nowTms is later than currEvent),
       * so time to next event is 0.
       */
      *ms = 0;
   }
   else
   {
      /*
       * nowTms is earlier than currEvent, so currEvent is still in
       * the future.  
       */
      (*ms) = cmsTms_deltaInMilliSeconds(&(currEvent->expireTms), &nowTms);
   }

   return CMSRET_SUCCESS;
}


UINT32 cmsTmr_getNumberOfEvents(const void *handle)
{
   const CmsTimerHandle *tmrHandle = (const CmsTimerHandle *) handle;

   return (tmrHandle->numEvents);
}


void cmsTmr_executeExpiredEvents(void *handle)
{
   CmsTimerHandle *tmrHandle = (CmsTimerHandle *) handle;
   CmsTimerEvent *currEvent;
   CmsTimestamp nowTms;

   cmsTms_get(&nowTms);
   currEvent = tmrHandle->events;

   while ((currEvent != NULL) && (IS_EARLIER_THAN(&(currEvent->expireTms), &nowTms)) &&
          (currEvent->pausedTms.sec == 0) && (currEvent->pausedTms.nsec == 0))
   {
      /*
       * first remove the currEvent from the tmrHandle because
       * when we execute the callback function, it might call the
       * cmsTmr API again.
       */
      tmrHandle->events = currEvent->next;
      currEvent->next = NULL;
      tmrHandle->numEvents--;

      cmsLog_debug("executing timer event %s func 0x%x data 0x%x",
                   currEvent->name, currEvent->func, currEvent->ctxData);

      /* call the function */
      (*currEvent->func)(currEvent->ctxData);

      /* free the event struct */
      cmsMem_free(currEvent);

      currEvent = tmrHandle->events;
   }

   return;
}

CmsRet cmsTmr_getTimeRemaining(const void *handle, CmsEventHandler func, void *ctxData, UINT32 *ms)
{
   const CmsTimerHandle *tmrHandle = (const CmsTimerHandle *)handle;
   CmsTimerEvent *tmrEvent;
   CmsTimestamp   nowTms;
   UINT32 msRem = 0;
   CmsRet retVal = CMSRET_OBJECT_NOT_FOUND;

   tmrEvent = tmrHandle->events;
   while (tmrEvent != NULL)
   {
      if ((tmrEvent->func == func) && (tmrEvent->ctxData == ctxData))
      {
          retVal = CMSRET_SUCCESS;
          cmsTms_get(&nowTms);
          if ( IS_EARLIER_THAN(&nowTms, &tmrEvent->expireTms) )
          {
              msRem = cmsTms_deltaInMilliSeconds(&tmrEvent->expireTms, &nowTms);
          }
          else
          {
              msRem = 0;
          }
          break;
      }
      else
      {
         tmrEvent = tmrEvent->next;
      }
   }

   *ms = msRem;

   return retVal;
}

UBOOL8 cmsTmr_isEventPresent(const void *handle, CmsEventHandler func, void *ctxData)
{
   const CmsTimerHandle *tmrHandle = (const CmsTimerHandle *) handle;
   CmsTimerEvent *tmrEvent;
   UBOOL8 found=FALSE;

   tmrEvent = tmrHandle->events;

   while ((tmrEvent != NULL) && (!found))
   {
      if (tmrEvent->func == func && tmrEvent->ctxData == ctxData)
      {
         found = TRUE;
      }
      else
      {
         tmrEvent = tmrEvent->next;
      }
   }

   return found;
}

void cmsTmr_dumpEvents(const void *handle __attribute__((unused)))
{
#ifdef CMS_LOG3
   const CmsTimerHandle *tmrHandle = (const CmsTimerHandle *) handle;
   CmsTimerEvent *currEvent;
   CmsTimestamp nowTms;
   UINT32 expires;

   cmsLog_debug("dumping %d events", tmrHandle->numEvents);
   cmsTms_get(&nowTms);

   currEvent = tmrHandle->events;

   while (currEvent != NULL)
   {

      /* this is the same code as in getTimeToNextEvent, integrate? */
      if (IS_EARLIER_THAN(&(currEvent->expireTms), &nowTms))
      {
         /*
          * the currentevent is past due (nowTms is later than currEvent),
          * so expiry time is 0.
          */
         expires = 0;
      }
      else
      {
         /*
          * nowTms is earlier than currEvent, so currEvent is still in
          * the future.  
          */
         expires = cmsTms_deltaInMilliSeconds(&(currEvent->expireTms), &nowTms);
      }


      cmsLog_debug("event %s expires in %ums (at %u.%03u) func=0x%x data=%p",
                   currEvent->name,
                   expires,
                   currEvent->expireTms.sec,
                   currEvent->expireTms.nsec/NSECS_IN_MSEC,
                   currEvent->func,
                   currEvent->ctxData);

      currEvent = currEvent->next;
   }
#endif
   return;
}


CmsRet cmsTmr_replaceIfSooner(void *handle, CmsEventHandler func, void *ctxData, UINT32 ms, const char *name)
{
   CmsTimestamp nowTms;
   const CmsTimerHandle *tmrHandle = (const CmsTimerHandle *) handle;
   CmsTimerEvent *tmrEvent;
   UBOOL8 found=FALSE;

   tmrEvent = tmrHandle->events;

   while ((tmrEvent != NULL) && (!found))
   {
      if (tmrEvent->func == func && tmrEvent->ctxData == ctxData)
      {
         found = TRUE;
      }
      else
      {
         tmrEvent = tmrEvent->next;
      }
   }
   if (found)
   {
      /* find out the expire time of this event.  If it's sooner then the one in the 
       * timer list, then replace the one in list with this one.
       */
      cmsTms_get(&nowTms);
      cmsTms_addMilliSeconds(&nowTms, ms);
      if (IS_EARLIER_THAN(&nowTms, &(tmrEvent->expireTms)))
      {
         cmsTmr_cancel((void*)tmrHandle, func, (void*)NULL);
      }
      else
      {
         return CMSRET_SUCCESS;
      }
   } /* found */
   return(cmsTmr_set(handle, func, ctxData, ms, name));
}
