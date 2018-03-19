/******************************************************************************
 *  
<:copyright-BRCM:2015:DUAL/GPL:standard

   Copyright (c) 2015 Broadcom 
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
 *
******************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include "cms_actionlog.h"

#define CAL_LOG_FILE    "/var/cal.log"

inline void printlog( const char *buf )
{
   FILE *fd = fopen(CAL_LOG_FILE, "a+");

   if (fd == NULL)
   {
      fprintf(stderr, "callog error: cannot open %s", CAL_LOG_FILE);
      fflush(stderr);
   }
   else
   {
      fprintf(fd, "%s", buf);
      fclose(fd);
   }
}

void callog_log(CmsActionLogSrc src __attribute__((unused)), const char *pFmt, ... )
{
   va_list		ap;
   char buf[MAX_ACTIONLOG_LINE_LENGTH] = {0};
   int len=0, maxLen;

   maxLen = sizeof(buf);

   va_start(ap, pFmt);

#if 0
   {
      CmsTimestamp ts;

      cmsTms_get(&ts);
   }
#endif

   if (len < maxLen)
   {
      maxLen -= len;
      vsnprintf(&buf[len], maxLen, pFmt, ap);
   }

   printlog(buf);

   va_end(ap);
}  /* End of callog_log() */
