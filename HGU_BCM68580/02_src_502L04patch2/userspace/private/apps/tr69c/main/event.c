/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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

/**********************************************************************
 * Globals
 *********************************************************************/
Listener      *listeners = NULL;


/**********************************************************************
 * Public functions
 *********************************************************************/
void stopListener(int fd)
{
   Listener* curr = NULL;
   Listener* prev = NULL;

   // find Listener that matches fd
   for (prev = curr = listeners; curr != NULL && curr->fd != fd; curr = curr->next)
   {
      prev = curr;
   }

   if (curr != NULL)
   {
      // remove the match Listener
      prev->next = curr->next;
      /* move head pointer to the next one
         if head node is removed */
      if (listeners == curr)
      {
         listeners = curr->next;
      }
      // don't close fd since it's closed by caller
      // close(fd);         
      curr->func = NULL;
      curr->handle = NULL;
      curr->next = NULL;
      CMSMEM_FREE_BUF_AND_NULL_PTR(curr);
      cmsLog_debug("removed listener on fd=%d", fd);
   }
}

void closeAllListeners(void)
{
   Listener* l;
   for (l = listeners; l; l = l->next)
   {
      close(l->fd);
   }
}

void freeAllListeners(void)
{
   Listener *l=listeners;
   Listener *tmp;

   while (l)
   {
      tmp = l->next;
      cmsMem_free(l);
      l = tmp;
   }
}

void setListener(int fd, CmsEventHandler func, void* handle)
{
   setListenerType(fd, func, handle, iListener_Read);
}

void setListenerType(int fd, CmsEventHandler func, void* handle, tListenerType type)
{
   Listener* l;
   char *listenerType __attribute__ ((unused)) = "invalid";

   stopListener(fd);

   switch(type)
   {
   case iListener_Read:
      listenerType = "read";
      break;

   case iListener_Write:
      listenerType = "write";
      break;

   case iListener_Except:
      listenerType = "exception";
      break;

   case iListener_ReadWrite:
      listenerType = "read-write";
      break;
   }

   if (fd < MIN_LISTENER_FD || fd > MAX_LISTENER_FD)
   {
      cmsLog_error("Suspicious fd=%d handle=%p type=%s", fd, handle, listenerType);
      cmsAst_assert((fd >= MIN_LISTENER_FD) && (fd <= MAX_LISTENER_FD));
      /* let it go on for now, we won't actually set the fd in eventLoop */
   }


   cmsLog_debug("Adding listener on fd=%d type=%s handle=%p", fd, listenerType, handle);


   l = cmsMem_alloc((sizeof *l), 0);
   l->fd     = fd;
   l->func   = func;
   l->handle = handle;
   l->next   = listeners;
   l->type   = type;
   listeners = l;
}


