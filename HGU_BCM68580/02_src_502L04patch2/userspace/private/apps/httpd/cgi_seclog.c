/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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

#include "cgi_main.h"
#include "cms_log.h"
#include "cms_seclog.h"

void cgiSecLogView(char *query __attribute__((unused)), FILE *fs) {
   CmsSecurityLogFile log_data;
   CmsRet ret;   
   UINT32 rd, end;
   UINT32 new_log = TRUE;

   if ( (0 == strcmp(glbWebVar.curUserName, glbWebVar.adminUserName)) ||
        (0 == strcmp(glbWebVar.curUserName, glbWebVar.sptUserName)) )
   {
      fprintf(fs, "<html><head>\n");
      fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
      fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
      fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
      fprintf(fs, "<title></title>\n");
      fprintf(fs, "</head>\n");
      fprintf(fs, "<center>\n<form>\n");
      fprintf(fs, "<b>Security Log</b><br><br>\n");
      fprintf(fs, "<table border='1' cellpadding='1' cellspacing='0'>\n");
      // write headers
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>Message</td>\n");
      fprintf(fs, "   </tr>\n");


      // retrieve log
      ret = cmsLog_getSecurityLog(&log_data);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to read security log into buf, ret=%d", ret);

         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>Error reading security log (%d)</td>\n", ret);
         fprintf(fs, "   </tr>\n");
      }
      else
      {
         rd = log_data.read_offset;
         if (rd > log_data.write_offset)
            end = log_data.write_offset + SECURITY_LOG_DATA_SIZE;
         else
            end = log_data.write_offset;

         while (rd < end)
         {
            if (log_data.log[rd % SECURITY_LOG_DATA_SIZE] != 0x0)
            {
               if (new_log)
               {
                  fprintf(fs, "   <tr>\n");
                  fprintf(fs, "      <td>");
                  new_log = FALSE;
               }
               putc(log_data.log[rd % SECURITY_LOG_DATA_SIZE], fs);
            }
            else
            {
               fprintf(fs, "</td>\n");
               fprintf(fs, "   </tr>\n");

               new_log = TRUE;
            }
            rd++;
         };
      }

      fprintf(fs, "</table>\n");
      fprintf(fs, "<p align='center'>\n");
      fprintf(fs, "<input type='button' onClick='location.href=\"seclogview.cmd\"' value='Refresh'>\n");
      fprintf(fs, "<input type='button' onClick='window.close()' value='Close'>\n");
      fprintf(fs, "</form>\n</center>\n</body>\n</html>\n");
   }
   fflush(fs);
}


void cgiSecLogText(char *query __attribute__((unused)), FILE *fs) {
   CmsSecurityLogFile log_data;
   CmsRet ret;   
   UINT32 rd, end;

   if ( (0 == strcmp(glbWebVar.curUserName, glbWebVar.adminUserName)) ||
        (0 == strcmp(glbWebVar.curUserName, glbWebVar.sptUserName)) )
   {
      fprintf(fs, "Broadcom Security Log\n");
      fprintf(fs, "------------------------\n");

      // retrieve log
      ret = cmsLog_getSecurityLog(&log_data);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to read security log into buf, ret=%d", ret);

         fprintf(fs, "\nError reading security log (%d)\n", ret);
      }
      else
      {
         rd = log_data.read_offset;
         if (rd > log_data.write_offset)
            end = log_data.write_offset + SECURITY_LOG_DATA_SIZE;
         else
            end = log_data.write_offset;

         while (rd < end)
         {
            if (log_data.log[rd % SECURITY_LOG_DATA_SIZE] != 0x0)
            {
               putc(log_data.log[rd % SECURITY_LOG_DATA_SIZE], fs);
            }
            else
            {
               fprintf(fs, "\n");
            }
            rd++;
         };
      }
   }
   fflush(fs);
}

void cgiSecLogReset(char *query __attribute__((unused)), FILE *fs) {
   CmsRet ret;   

   if ( (0 == strcmp(glbWebVar.curUserName, glbWebVar.adminUserName)) ||
        (0 == strcmp(glbWebVar.curUserName, glbWebVar.sptUserName)) )
   {
      // reset log
      ret = cmsLog_resetSecurityLog();

      fprintf(fs, "<html><head>\n");
      fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
      fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
      fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
      fprintf(fs, "<title></title>\n");
      fprintf(fs, "</head>\n");
      fprintf(fs, "<center>\n<form>\n");

      if (ret == CMSRET_SUCCESS)
         fprintf(fs, "<b>Security Log Successfully Reset</b><br><br>\n");
      else
         fprintf(fs, "<b>Error Resetting Security Log (%d)</b><br><br>\n", ret);

      fprintf(fs, "<p align='center'>\n");
      fprintf(fs, "<input type='button' onClick='window.close()' value='Close'>\n");
      fprintf(fs, "</form>\n</center>\n</body>\n</html>\n");
   }
   fflush(fs);
}




