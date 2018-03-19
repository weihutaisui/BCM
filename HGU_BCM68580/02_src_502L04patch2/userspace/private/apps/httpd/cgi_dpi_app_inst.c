/***********************************************************************
 *
 *  Copyright (c) 2005-2014  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2014:proprietary:standard

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


#ifdef SUPPORT_DPI


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "cms.h"
#include "cms_util.h"
#include "cgi_main.h"
#include "dpictl_api.h"

#define DPI_APP_INST_FILE "/var/tmp/appinst"


void writeDpiAppInstHeader(FILE *fs, UBOOL8 isSorted) 
{
   fprintf(fs, "<!doctype html>\n<html>\n<head>\n\n");
   if (isSorted)
   {
      fprintf(fs, "<meta charset='utf-8'>\n");
      fprintf(fs, "<link class='include' rel='stylesheet' type='text/css' href='tablesorter/themes/blue/style.css'/>\n");
      fprintf(fs, "<script type='text/javascript' src='jquery.js'></script>\n");
      fprintf(fs, "<script type='text/javascript' src='tablesorter/jquery.tablesorter.min.js'></script>\n\n");
   }
   else
   {
      fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
      fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
      fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   }
}


static void writeDpiAppInstScript(FILE *fs) 
{
   fprintf(fs, "<script class='code' type='text/javascript'>\n\n");

   fprintf(fs, "$(document).ready(function(){\n");
   fprintf(fs, "  {\n");
   fprintf(fs, "    $('#AppInstTable').tablesorter( {sortList: [[1,0], [2,0]]} );\n");
   fprintf(fs, "  }\n");
   fprintf(fs, "});\n\n");

   fprintf(fs, "</script>\n\n");
}


static void cgiDpiAppInstView(FILE *fs, UBOOL8 isSorted) 
{
   size_t len = 0;
   UINT32 argc = 0, id = 0;
   unsigned long upPkg=0, dwnPkg = 0;
   unsigned long long upByte = 0, dwnByte = 0;
   char cmd[BUFLEN_64], mac[BUFLEN_32];
   char devName[DPI_MAX_DEV_NAME_LEN], appName[DPI_MAX_NAME_LEN];
   char *line = NULL;
   FILE *fp = NULL;

   memset(cmd, 0, BUFLEN_64);
   memset(mac, 0, BUFLEN_32);
   memset(devName, 0, DPI_MAX_DEV_NAME_LEN);
   memset(appName, 0, DPI_MAX_NAME_LEN);

   sprintf(cmd, "dpi appinst > %s", DPI_APP_INST_FILE);
#ifndef DESKTOP_LINUX   
   system(cmd);
#endif

   writeDpiAppInstHeader(fs, isSorted);
   if (isSorted)
      writeDpiAppInstScript(fs);

   // write page body
   if (isSorted)
      fprintf(fs, "</head>\n<body>\n");
   else
      fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n<center>\n");

   fprintf(fs, "<b>DPI -- Devices & Applications</b><br><br>\n");

   // write table
   if (isSorted)
   {
      fprintf(fs, "<table id='AppInstTable' class='tablesorter'>\n");
      fprintf(fs, "  <thead>\n");
   }
   else
      fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");

   // write table header
   fprintf(fs, "    <tr>\n");
   if (isSorted)
   {
      fprintf(fs, "      <th>MAC Address</th>\n");
      fprintf(fs, "      <th>Device Name</th>\n");
      fprintf(fs, "      <th>Application Name</th>\n");
      fprintf(fs, "      <th>Application ID</th>\n");
      fprintf(fs, "      <th>Upstream (Byte)</th>\n");
      fprintf(fs, "      <th>Downstream (Byte)</th>\n");
   }
   else
   {
      fprintf(fs, "      <td class='hd'>MAC Address</td>\n");   
      fprintf(fs, "      <td class='hd'>Device Name</td>\n");
      fprintf(fs, "      <td class='hd'>Application Name</td>\n");   
      fprintf(fs, "      <td class='hd'>Application ID</td>\n");   
      fprintf(fs, "      <td class='hd'>Upstream (Byte)</th>\n");
      fprintf(fs, "      <td class='hd'>Downstream (Byte)</th>\n");
   }
   fprintf(fs, "    </tr>\n");

   if (isSorted)
   {
      fprintf(fs, "  </thead>\n");
      fprintf(fs, "  <tbody>\n");
   }

   // write table body
   fp = fopen(DPI_APP_INST_FILE, "r");
   if (fp != NULL)
   {
      while (getline(&line, &len, fp) != -1)
      {
         argc = sscanf(line,
                       "%u,%[^,],%[^,],%[^,],%lu,%llu,%lu,%llu",
                       &id, appName, mac, devName,
                       &upPkg, &upByte, &dwnPkg, &dwnByte);
         if (argc == 8)
         {
            fprintf(fs, "   <tr>\n");
            if (isSorted)
            {
               fprintf(fs, "      <td>%s</td>\n", mac);
               fprintf(fs, "      <td>%s</td>\n", devName);
               fprintf(fs, "      <td>%s</td>\n", appName);
               fprintf(fs, "      <td>%u</td>\n", id);
               fprintf(fs, "      <td>%llu</td>\n", upByte);
               fprintf(fs, "      <td>%llu</td>\n", dwnByte);
            }
            else
            {
               fprintf(fs, "      <td align='center'>%s</td>\n", mac);
               fprintf(fs, "      <td align='center'>%s</td>\n", devName);
               fprintf(fs, "      <td align='center'>%s</td>\n", appName);
               fprintf(fs, "      <td align='center'>%u</td>\n", id);
               fprintf(fs, "      <td align='center'>%llu</td>\n", upByte);
               fprintf(fs, "      <td align='center'>%llu</td>\n", dwnByte);
            }
            fprintf(fs, "   </tr>\n");
         }
      }

      // free line memory outside of while loop
      if (line != NULL)
      {
         free(line);
         line = NULL;
      }

      // close file descriptor
      fclose(fp);

      // remove temp file
      unlink(DPI_APP_INST_FILE);
   }

   if (isSorted)
      fprintf(fs, "  </tbody>\n");

   // write table end
   fprintf(fs, "</table>\n");

   // write page end
   if (isSorted)
      fprintf(fs, "</body>\n</html>\n");
   else
      fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");

   // flush output
   fflush(fs);
}


void cgiDpiAppInst(char *query, FILE *fs) 
{
   char sort[BUFLEN_32];

   cgiGetValueByName(query, "sort", sort);

   if ( strcmp(sort, "true") == 0 )
   {
      cgiDpiAppInstView(fs, TRUE);
   }
   else
   {
      cgiDpiAppInstView(fs, FALSE);
   }
}


#endif   /* SUPPORT_DPI */
