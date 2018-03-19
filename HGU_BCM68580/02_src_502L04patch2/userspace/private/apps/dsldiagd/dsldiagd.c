/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef ECOS
#include "bcmThreads.h"

/* globals */
static cyg_handle_t dsldiagd_thread_handle;
static char dsldiagd_stack[THREAD_DSLDIAGD_STACK_SIZE];
static cyg_thread dsldiagd_thread;
#else
#include "bcmnet.h"
#endif

extern void XdslConnectionCheck(void);
extern int  BcmAdslCtl_GetDiagsSocket(void);
extern void BcmAdslCtl_ProcessDiagsFrames(void);
extern int  BcmAdslCtl_GetGuiSocket(void);
extern void BcmAdslCtl_ProcessGuiFrames(void);
extern int  BcmAdslCtl_GetGdbSocket(void);
extern void BcmAdslCtl_ProcessGdbFrames(void);

//#define EOC_MSG_RD_WR_TEST
#ifdef EOC_MSG_RD_WR_TEST
extern CmsRet xdslCtl_OpenEocIntf(unsigned char lineId, int eocMsgType, char *pIfName);
#include <fcntl.h>      /* open */
#include "bcmadsl.h"

#define XDSL_NSF_EOC_FILE_NAME			"xdslNsfEocMsg0"
#define XDSL_NSF_EOC_PROC_FILE_NAME	"/proc/xdslNsfEocMsg0"
#define EOC_BUF_SIZE					1020
unsigned char nsfEocBuf[EOC_BUF_SIZE];
SINT32 gNsfEocFd = -1;
CmsRet nsfEocIntfOpened = CMSRET_INTERNAL_ERROR;

int BcmAdslCtl_GetNsfEocSocket(void)
{
	if(CMSRET_INTERNAL_ERROR == nsfEocIntfOpened)
		nsfEocIntfOpened = xdslCtl_OpenEocIntf(0, BCM_XDSL_NSF_EOC_MSG, XDSL_NSF_EOC_FILE_NAME);
	else {
		if(-1 == gNsfEocFd) {
			gNsfEocFd = open(XDSL_NSF_EOC_PROC_FILE_NAME, O_RDWR);
		}
	}
	return gNsfEocFd;
}

void BcmAdslCtl_ProcessNsfEocFrames(void)
{
	int byteWrite, bytesRead = read(gNsfEocFd,(void*)nsfEocBuf, EOC_BUF_SIZE);
	if(0 != bytesRead) {
		byteWrite = write(gNsfEocFd,(void*)nsfEocBuf, bytesRead);
	}
}

#define XDSL_DATAGRAM_EOC_FILE_NAME		"xdslDatagramEocMsg0"
#define XDSL_DATAGRAM_EOC_PROC_FILE_NAME	"/proc/xdslDatagramEocMsg0"
#define EOC_BUF_SIZE					1020
unsigned char datagramEocBuf[EOC_BUF_SIZE];
SINT32 gDatagramEocFd = -1;
CmsRet datagramEocIntfOpened = CMSRET_INTERNAL_ERROR;

int BcmAdslCtl_GetDatagramEocSocket(void)
{
	if(CMSRET_INTERNAL_ERROR == datagramEocIntfOpened)
		datagramEocIntfOpened = xdslCtl_OpenEocIntf(0, BCM_XDSL_DATAGRAM_EOC_MSG, XDSL_DATAGRAM_EOC_FILE_NAME);
	else {
		if(-1 == gDatagramEocFd) {
			gDatagramEocFd = open(XDSL_DATAGRAM_EOC_PROC_FILE_NAME, O_RDWR);
		}
	}
	return gDatagramEocFd;
}

void BcmAdslCtl_ProcessDatagramEocFrames(void)
{
	int byteWrite, bytesRead = read(gDatagramEocFd,(void*)datagramEocBuf, EOC_BUF_SIZE);
	if(0 != bytesRead) {
		byteWrite = write(gDatagramEocFd,(void*)datagramEocBuf, bytesRead);
	}
}
#endif  /* EOC_MSG_RD_WR_TEST */

#ifdef ECOS
SINT32 dsldiagd_main( void * optArgs )
#else
SINT32 main(SINT32 argc, char *argv[])
#endif
{
   SINT32 exitCode=0;
   SINT32 n, maxFd=0;
   fd_set readFdsMaster, readFds;
   fd_set errorFdsMaster, errorFds;
   UBOOL8 keepLooping=TRUE;
   int pid;
   struct timeval tv;
   SINT32 diagFd, guiFd, gdbFd;
#ifdef EOC_MSG_RD_WR_TEST
   SINT32 nsfEocFd, datagramEocFd;
#endif
   pid = fork();
   if (pid < 0)
   {
      printf("cannot fork dsldiagd! \n");
      return -1;
   }
   else if (pid > 0)
   {
      /* parent process */
      exit(0);
   }
   setsid();

   /* set up all the fd stuff for select */
   FD_ZERO(&readFdsMaster);
   FD_ZERO(&errorFdsMaster);

   tv.tv_sec = 1;
   tv.tv_usec = 0;
   
   while (keepLooping)
   {
      readFds = readFdsMaster;
      errorFds = errorFdsMaster;
#ifdef EOC_MSG_RD_WR_TEST
      if ((nsfEocFd = BcmAdslCtl_GetNsfEocSocket()) != -1) {
         FD_SET(nsfEocFd, &readFds);
         maxFd = (nsfEocFd > maxFd) ? nsfEocFd: maxFd;
      }
      if ((datagramEocFd = BcmAdslCtl_GetDatagramEocSocket()) != -1) {
         FD_SET(datagramEocFd, &readFds);
         maxFd = (datagramEocFd > maxFd) ? datagramEocFd: maxFd;
      }
#endif
      if ((diagFd = BcmAdslCtl_GetDiagsSocket()) != -1) {
         FD_SET(diagFd, &readFds);
         maxFd = (diagFd > maxFd) ? diagFd: maxFd;
      }
      if ((guiFd = BcmAdslCtl_GetGuiSocket()) != -1) {
         FD_SET(guiFd, &readFds);
         maxFd = (guiFd > maxFd) ? guiFd: maxFd;
      }
      if ((gdbFd = BcmAdslCtl_GetGdbSocket()) != -1) {
         FD_SET(gdbFd, &readFds);
         maxFd = (gdbFd > maxFd) ? gdbFd: maxFd;
      }

      n = select(maxFd+1, &readFds, NULL, &errorFds, &tv);
      if (n < 0)
      {
         /* interrupted by signal or something, continue */
         continue;
      }

      if ( 0 == n )
      {
         tv.tv_sec = 1;
         tv.tv_usec = 0;

         XdslConnectionCheck();

      }
#ifdef EOC_MSG_RD_WR_TEST
      if ((nsfEocFd != -1) && (FD_ISSET(nsfEocFd,&readFds)))
         BcmAdslCtl_ProcessNsfEocFrames();
      if ((datagramEocFd != -1) && (FD_ISSET(datagramEocFd,&readFds)))
         BcmAdslCtl_ProcessDatagramEocFrames();
#endif
      if ((diagFd != -1) && (FD_ISSET(diagFd,&readFds)))
         BcmAdslCtl_ProcessDiagsFrames();
      if ((guiFd != -1) && (FD_ISSET(guiFd,&readFds)))
         BcmAdslCtl_ProcessGuiFrames();
      if ((gdbFd != -1) && (FD_ISSET(gdbFd,&readFds)))
         BcmAdslCtl_ProcessGdbFrames();
   }

   return exitCode;
}

#ifdef ECOS
int dsldiagd_start(void *arg)
{
   /* priority, 0 being the highest, 31 being the lowest */

	cyg_thread_create(THREAD_DSLDIAGD_PRIORITY,
                     (cyg_thread_entry_t *)dsldiagd_main,
                     (cyg_addrword_t)arg,
                     THREAD_DSLDIAGD_NAME,
                     (void *)&dsldiagd_stack[0],
                     THREAD_DSLDIAGD_STACK_SIZE,
                     &dsldiagd_thread_handle,
                     &dsldiagd_thread);
	cyg_thread_resume(dsldiagd_thread_handle);
	/* Make sure param is copied */
	cyg_thread_delay(2);
   return 0;
}
#endif
