/***********************************************************************
 *
 *  Copyright (c) 2006-2009  Broadcom Corporation
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

#ifdef SUPPORT_DEBUG_TOOLS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "syscall.h"
#include "httpd.h"
#include "cgi_main.h"
#include "cgi_eng.h"
#include "prctl.h"

void engDumpFile(FILE *fs, char *path)
{
   char buffer[1024];
   int rsize;

   FILE *fi = fopen(path, "r");
   if (!fi) {
      return;
   }

   for(;;) {
      rsize = fread(buffer, 1, sizeof(buffer), fi);
      if (!rsize) {
         break;
      }
      fwrite(buffer, 1, rsize, fs);
   }
   fclose(fi);
}

void cgiEngInfo(char *query __attribute__((unused)), FILE *fs)
{
   fprintf(fs, "<html>\n");
   fprintf(fs, "<head>\n");
   fprintf(fs, "<title>Engineering Information</title>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "</head>\n");
   fprintf(fs, "<body>\n");

   fprintf(fs, "<h1>/proc/version</h1>\n<pre>\n");
   engDumpFile(fs, "/proc/version");
   fprintf(fs, "</pre>\n");
   
   fprintf(fs, "<h1>sysinfo</h1>\n<pre>\n");
   prctl_runCommandInShellBlocking("sysinfo > /var/enginfo");
   engDumpFile(fs, "/var/enginfo");
   fprintf(fs, "</pre>\n");

   fprintf(fs, "<h1>/proc/meminfo</h1>\n<pre>\n");
   engDumpFile(fs, "/proc/meminfo");
   fprintf(fs, "</pre>\n");

   fprintf(fs, "<h1>/proc/cpuinfo</h1>\n<pre>\n");
   engDumpFile(fs, "/proc/cpuinfo");
   fprintf(fs, "</pre>\n");

   fprintf(fs, "<h1>ps</h1>\n<pre>\n");
   prctl_runCommandInShellBlocking("ps > /var/enginfo");
   engDumpFile(fs, "/var/enginfo");
   fprintf(fs, "</pre>\n");

   fprintf(fs, "<h1>/proc/slabinfo</h1>\n<pre>\n");
   engDumpFile(fs, "/proc/slabinfo");
   fprintf(fs, "</pre>\n");

   fprintf(fs, "<h1>/proc/filesystems</h1>\n<pre>\n");
   engDumpFile(fs, "/proc/filesystems");
   fprintf(fs, "</pre>\n");

   fprintf(fs, "<h1>/proc/devices</h1>\n<pre>\n");
   engDumpFile(fs, "/proc/devices");
   fprintf(fs, "</pre>\n");

   fprintf(fs, "<h1>ifconfig</h1>\n<pre>\n");
   prctl_runCommandInShellBlocking("ifconfig > /var/enginfo");
   engDumpFile(fs, "/var/enginfo");
   fprintf(fs, "</pre>\n");

   fprintf(fs, "<h1>route</h1>\n<pre>\n");
   prctl_runCommandInShellBlocking("route > /var/enginfo");
   engDumpFile(fs, "/var/enginfo");
   fprintf(fs, "</pre>\n");

   fprintf(fs, "<h1>/proc/net/wireless</h1>\n<pre>\n");
   engDumpFile(fs, "/proc/net/wireless");
   fprintf(fs, "</pre>\n");

   fprintf(fs, "<h1>/proc/net/wl0</h1>\n<pre>\n");
   engDumpFile(fs, "/proc/net/wl0");
   fprintf(fs, "</pre>\n");

   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}


void cgiEngDebug (char *query, FILE *fs)
{
    char action[BUFLEN_256] ;
    char lst[BUFLEN_1024];

    cgiGetValueByName (query, "action", action) ;

    if (strcmp(action, "apply") == 0) 
    {
        cgiGetValueByName(query, "mirrorLst", lst);
        dalPMirror_configPortMirrors(lst);
        glbSaveConfigNeeded = TRUE;
    }

    do_ej("/webs/engdebug.html", fs);
} /* cgiEngDebug */


#endif /* SUPPORT_DEBUG_TOOLS */
