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

#include <string.h>
#include "cms.h"
#include "cms_mem.h"
#include "cms_log.h"
#include "cms_strconv.h"


struct xml_esc_entry {
   char esc;  /**< character that needs to be escaped */
   char *seq; /**< escape sequence */
   UINT32 len;  /**< length of escape sequence */
};

struct xml_esc_entry xml_esc_table[] = {
      {'<', "&lt;", 4},
      {'>', "&gt;", 4},
      {'&', "&amp;", 5},
      {'%', "&#37;", 5},
      {' ', "&#32;", 5},
      {'\t', "&#09;", 5},
      {'\n', "&#10;", 5},
      {'\r', "&#13;", 5},
      {'"', "&quot;", 6},
};

#define NUM_XML_ESC_ENTRIES (sizeof(xml_esc_table)/sizeof(struct xml_esc_entry))



UBOOL8 cmsXml_isEscapeNeeded(const char *string)
{
   UINT32 len, i=0, e=0;
   UBOOL8 escapeNeeded = FALSE;

   if (string == NULL)
   {
      return FALSE;
   }

   len = strlen(string);

   /* look for characters which need to be escaped. */
   while (escapeNeeded == FALSE && i < len)
   {
      for (e=0; e < NUM_XML_ESC_ENTRIES; e++)
      {
         if (string[i] == xml_esc_table[e].esc)
         {
            escapeNeeded = TRUE;
            break;
         }
      }
      i++;
   }

   return escapeNeeded;
}


CmsRet cmsXml_escapeString(const char *string, char **escapedString)
{
   UINT32 len, len2, i=0, j=0, e=0, f=0;
   char *tmpStr;

   if (string == NULL)
   {
      return CMSRET_SUCCESS;
   }

   len = strlen(string);
   len2 = len;

   /* see how many characters need to be escaped and what the new length is */
   while (i < len)
   {
      for (e=0; e < NUM_XML_ESC_ENTRIES; e++)
      {
         if (string[i] == xml_esc_table[e].esc)
         {
            len2 += (xml_esc_table[e].len - 1);
            break;
         }
      }
      i++;
   }

   if ((tmpStr = cmsMem_alloc(len2+1, ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("failed to allocate %d bytes", len+1);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   i=0;
   while (i < len)
   {
      UBOOL8 found;

      found = FALSE;
      /* see if we need to replace any characters with esc sequences */
      for (e=0; e < NUM_XML_ESC_ENTRIES; e++)
      {
         if (string[i] == xml_esc_table[e].esc)
         {
            for (f=0; f<xml_esc_table[e].len; f++)
            {
               tmpStr[j++] = xml_esc_table[e].seq[f];
               found = TRUE;
            }
            break;
         }
      }

      /* no replacement, then just copy over the original string */
      if (!found)
      {
         tmpStr[j++] = string[i];
      }

      i++;
   }

   *escapedString = tmpStr;

   return CMSRET_SUCCESS;
}


UBOOL8 cmsXml_isUnescapeNeeded(const char *escapedString)
{
   UINT32 len, i=0, e=0, f=0;
   UBOOL8 unescapeNeeded = FALSE;
   UBOOL8 matched=FALSE;

   if (escapedString == NULL)
   {
      return FALSE;
   }

   len = strlen(escapedString);

   while (unescapeNeeded == FALSE && i < len)
   {
      /* all esc sequences begin with &, so look for that first */
      if (escapedString[i] == '&')
      {
         for (e=0; e < NUM_XML_ESC_ENTRIES && !matched; e++)
         {
            if (i+xml_esc_table[e].len-1 < len)
            {
               /* check for match against an individual sequence */
               matched = TRUE;
               for (f=1; f < xml_esc_table[e].len; f++)
               {
                  if (escapedString[i+f] != xml_esc_table[e].seq[f])
                  {
                     matched = FALSE;
                     break;
                  }
               }
            }
         }
      }

      i++;

      /* if we saw a match, then unescape is needed */
      unescapeNeeded = matched;
   }

   return unescapeNeeded;
}


CmsRet cmsXml_unescapeString(const char *escapedString, char **string)
{
   UINT32 len, i=0, j=0, e=0, f=0;
   char *tmpStr;
   UBOOL8 matched=FALSE;

   if (escapedString == NULL)
   {
      return CMSRET_SUCCESS;
   }

   len = strlen(escapedString);

   if ((tmpStr = cmsMem_alloc(len+1, ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("failed to allocate %d bytes", len+1);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   while (i < len)
   {
      /* all esc sequences begin with &, so look for that first */
      if (escapedString[i] == '&')
      {
         for (e=0; e < NUM_XML_ESC_ENTRIES && !matched; e++)
         {
            if (i+xml_esc_table[e].len-1 < len)
            {
               /* check for match against an individual sequence */
               matched = TRUE;
               for (f=1; f < xml_esc_table[e].len; f++)
               {
                  if (escapedString[i+f] != xml_esc_table[e].seq[f])
                  {
                     matched = FALSE;
                     break;
                  }
               }
            }

            if (matched)
            {
               tmpStr[j++] = xml_esc_table[e].esc;
               i += xml_esc_table[e].len;
            }
         }
      }

      if (!matched)
      {
         /* not a valid escape sequence, just copy it */
         tmpStr[j++] = escapedString[i++];
      }

      /* going on to next character, so reset matched */
      matched = FALSE;
   }

   *string = tmpStr;

   return CMSRET_SUCCESS;
}



void cmsXml_escapeStringEx(const char * orignalString, char * outputEscapedString, SINT32 outputEscapedStringLen)
{
   /* Always copy the orignalString to the outputEscapedString since there might
   * not any escape characters in the orignalString
   */
   strncpy(outputEscapedString, orignalString, outputEscapedStringLen);

   if (cmsXml_isEscapeNeeded(orignalString))             
   {
      char *escapedString=NULL;            
 
      cmsXml_escapeString(orignalString, &escapedString);
      cmsUtl_strncpy(outputEscapedString, escapedString, outputEscapedStringLen);
      cmsMem_free(escapedString);
   }     
}