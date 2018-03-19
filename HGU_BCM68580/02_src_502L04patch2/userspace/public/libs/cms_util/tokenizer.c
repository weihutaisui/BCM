/***********************************************************************
 *
 *  Copyright (c) 2012  Broadcom Corporation
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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
// extern ssize_t getline(char **lineptr, size_t *n, FILE *stream);

#include "cms.h"
#include "cms_util.h"
#include "oal.h"





static UBOOL8 is_terminal_char(UBOOL8 is_quoted, char c)
{
   if (is_quoted && c == '"')
      return TRUE;
   else if (!is_quoted && isspace(c))
      return TRUE;
   return FALSE;
}

UBOOL8 cmsTok_tokenizeLine(const char *line, UINT32 len,
                           const struct cms_token_map_entry *token_map,
                           struct cms_token_value_pair *pair)
{
   UBOOL8 found=FALSE;
   UINT32 i=0;

   /* skip leading whitespace */
   while (i < len && isspace(line[i]))
         i++;

   /* grab symbol */
   if (i < len)
   {
      UINT32 j=0;
      char tmpfield[128]={0};
      while (!isspace(line[i]) && line[i] != '=' && i < len && j < sizeof(tmpfield)-1)
      {
         tmpfield[j] = line[i];
         j++;
         i++;
      }
      for (j=0; token_map[j].keyword; j++)
      {
         if (!strcasecmp(tmpfield, token_map[j].keyword))
         {
            pair->token = token_map[j].token;
            // printf("field %s => token %d\n", tmpfield, pair->token);
            found = TRUE;
            break;
         }
      }

      if (!found)
      {
         cmsLog_error("Unrecognized token %s", tmpfield);
         return found;
      }
   }


   /* skip middle whitespace, including = sign */
   while (i < len && (isspace(line[i]) || line[i] == '='))
      i++;

   /* grab value string */
   if (i < len)
   {
      UINT32 j=0;
      UBOOL8 is_quoted=FALSE;
      if (line[i] == '"')
      {
         is_quoted = TRUE;
         i++;
      }

      while (i < len && j < sizeof(pair->valuebuf) &&
             !is_terminal_char(is_quoted, line[i]))
      {
         pair->valuebuf[j] = line[i];
         j++;
         i++;
      }
      // printf("valuebuf(%d)==>%s<==\n", j, pair->valuebuf);
   }

   return found;
}


UBOOL8 cmsTok_isDataLine(const char *buf, int len)
{
   int i=0;

   if (len > 0 && buf[0] == '#')
   {
      return FALSE;
   }

   for (i=0; i < len; i++)
   {
      if (!isspace(buf[i]))
         return TRUE;
   }

   return FALSE;
}


UBOOL8 cmsTok_getNextDataLine(FILE *fp, char *line, UINT32 *len)
{
   char *buf=NULL;
   ssize_t cnt=0;
   size_t n=0;

   while ((cnt = getline(&buf, &n, fp)) != -1)
   {
      if (cmsTok_isDataLine(buf, (int) cnt))
      {
         cmsUtl_strncpy(line, buf, (SINT32) *len);
         *len = (UINT32) n;
         free(buf);
         return 1;
      }
      free(buf);
      buf = NULL;
      n = 0;
   }

   return 0;
}

