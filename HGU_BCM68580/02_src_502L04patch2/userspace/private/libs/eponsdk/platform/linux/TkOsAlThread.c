/*
* <:copyright-BRCM:2013:proprietary:epon
* 
*    Copyright (c) 2013 Broadcom 
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

#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "TkOsAlThread.h"

////////////////////////////////////////////////////////////////////////////////
/// TkPrintf: system-dependent print function
///
/// This calls the underlying system "printf" type function.
///
/// \param tabs		the number of tabs to preceed the print
/// \param format	generic formatting string, followed by appropriate values
/// \return			the number of characters written, or negative if error
////////////////////////////////////////////////////////////////////////////////
int TkPrintf(U8 tabs, const char *format, ...)
	{
	int ret;
	va_list param;
	va_start(param, format);    //lint !e40 (confused by werid macro)
	for(ret = 0; ret < tabs; ret++)
		{
		printf("    ");
		}

	ret = vprintf(format, param);
	(void)fflush(stdout); //required by UNIX sometimes to actually display
	return ret;
	}

////////////////////////////////////////////////////////////////////////////////
/// TkOsGetCurrentTime: get the current system time in milliseconds
///
/// This gets the current system time.  For convenience, this value should be
/// returned in milliseconds, rather than in system ticks.  This value is used
/// to determine the amount of time that has passed since a message was sent,
/// so the system can retry the transmission.
///
/// \return The current system time in milliseconds
////////////////////////////////////////////////////////////////////////////////
U64 TkOsGetCurrentTimeMs(void)
	{
	struct timeval tv;
	gettimeofday(&tv, NULL); 
	// overflow is OK here
	return (U64)(tv.tv_sec * 1000) + (U64)(tv.tv_usec / 1000);
	}

TkOsThread TkOsThreadCreate(TkThreadPriority priority,
							TkThreadBody body, 
							void *stack,
							unsigned short stackSize,
							const unsigned char *threadName,
							unsigned int p1,
							unsigned short P2,
							unsigned short P3,
							unsigned short P4,
							unsigned short P5)
	{
	TkOsThread threadId;

	(void)stack;
	(void)stackSize;
	(void)threadName;
	(void)priority;

	if ( pthread_create(&threadId, NULL, (threadStartRoutine)body, (void *)p1) != 0 )
		{
		printf( "Thread %s create failed!", threadName );
		}
	return threadId;
	}

////////////////////////////////////////////////////////////////////////////////
/// TkOsThreadDestory:  destory a thread
///
/// Destory a thread that is already executing.
///
/// \param id   thread id. b
////////////////////////////////////////////////////////////////////////////////
void TkOsThreadDestroy(TkOsThread id)
	{
	pthread_kill(id, SIGKILL);
	}


//lint -e{818} thread should not be passed in as const
////////////////////////////////////////////////////////////////////////////////
/// TkOsThreadIdCheck: Checks if the executing thread is the given thread
///
/// \param thread Thread to check
///
/// \return TRUE if thread is currently executing
////////////////////////////////////////////////////////////////////////////////
BOOL TkOsThreadIdCheck(TkOsThread thread)
	{
	return (pthread_self() == thread) ? TRUE : FALSE;
	}


////////////////////////////////////////////////////////////////////////////////
/// TkOsThreadYield: Causes this thread to give other threads at the same 
/// priority a chance to run. 
////////////////////////////////////////////////////////////////////////////////
void TkOsThreadYield(void)
	{
	// do nothing on POSIX
	}

void TkOsTaskDelay(int ms)
	{
	usleep(1000*ms);
	}

