/***********************************************************************
 *
 *  Copyright (c) 2008  Broadcom Corporation
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>                                                              
                                                                                
#include "cms.h"
#include "cms_mem.h"
#include "cms_log.h"


// chec if string should contains XML Unicode characters
// &#xhhhh; or &#dddd, where h is any hex character [0-9,a-f]
// and d is any decimal digit [0-9]
// such as &#x4A; or &#67;
// This function will look for any number of digits, but only 8 bit unicode
// is supported right now.
UBOOL8 cmsUnicode_isUnescapeNeeded
   (const char *string)
{
   char *pStart = NULL, *pChar = NULL;
   UBOOL8 found = FALSE;

   if (string == NULL)
      return found;

   pStart = (char *)string;

   while (found == FALSE)
   {
      if ((pChar = strstr(pStart, "&#")) != NULL)
      {
         for (pChar += 2;
              found == FALSE && *pChar != '&' && *pChar != '\0';
              pChar++)
         {
            if (*pChar == ';')
               found = TRUE;
         }

         pStart = pChar;
      }
      else
      {
         break;
      }
   }

   return found;
}

static CmsRet consumeUnicode
   (const char *string, unsigned char *pConvertedChar, UINT32 *offset)
{
   char *tmpStr = NULL;
   char *pSemi = NULL, *pAmp = NULL;
   UINT16 num = 0;
   UINT32 i = 0;
   CmsRet ret = CMSRET_SUCCESS;
   
   if (string == NULL)
   {
      return CMSRET_INVALID_PARAM_VALUE;
   }

   // See valid format strings above
   tmpStr = cmsMem_strdup(string);

   *pConvertedChar = tmpStr[i];
   *offset = 1;

   if (tmpStr[i] == '&' && tmpStr[i+1] == '#')
   {
      // tmpStr = '&#x;' ==>  invalid number to convert
      if (tmpStr[i+2] == 'x' && tmpStr[i+3] == ';')
      {
         ret = CMSRET_INVALID_PARAM_VALUE;
         cmsLog_error("invalid format, skipping data");
         *offset = 4;
      }
      // tmpStr = '&#;' ==> invalid number to convert
      else if (tmpStr[i+2] == ';')
      {
         ret = CMSRET_INVALID_PARAM_VALUE;
         cmsLog_error("invalid format, skipping data");
         *offset = 3;
      }
      else
      {
         pSemi = strstr(&tmpStr[i+2], ";");
         if (pSemi != NULL)
         {
            pAmp = strstr(&tmpStr[i+2], "&");
            if (pAmp == NULL || pAmp > pSemi)
            {
               *pSemi = '\0';
               if (tmpStr[i+2] == 'x')
                  num = strtoul(&tmpStr[i+3], (char **)NULL, 16);
               else
                  num = strtoul(&tmpStr[i+2], (char **)NULL, 10);
               // TO-DO: need to take care unicode 16 (2 bytes)
               // right now only take care unicode 8 (1 byte)
               if (num > 127)
               {
                  cmsLog_error("multi-byte unicode not supported. "
                              "expected num <= 127, got num %d", num);
               }
               *pConvertedChar = (unsigned char) btowc(num);
               *offset = (pSemi - tmpStr) + 1;
            }
         }
      }
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(tmpStr);

   return ret;
}

CmsRet cmsUnicode_unescapeString
   (const char *string, char **unicodedString)
{
   char *tmpStr = NULL;
   unsigned char convertedChar;
   UINT32 len = 0, i = 0, j = 0, offset = 0;
   CmsRet ret = CMSRET_SUCCESS;

   if (string == NULL)
   {
      return ret;
   }

   len = strlen(string);

   if ((tmpStr = cmsMem_alloc(len, ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("failed to allocate %d bytes", len);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   while (i < len)
   {
      ret = consumeUnicode(&string[i], &convertedChar, &offset);
      // note, only 8 bit unicode is suported right now, so we can only
      // get at most 1 byte convertedChar
      if (ret == CMSRET_SUCCESS)
         tmpStr[j++] = convertedChar;

      i += offset;
   }
   
   *unicodedString = tmpStr;

   return ret;
}

