/***********************************************************************
 *
<:copyright-BRCM:2013:DUAL/GPL:standard

   Copyright (c) 2013 Broadcom 
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
 ************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>     /* for isDigit, really should be in oal_strconv.c */
#include <sys/stat.h>  /* this really should be in oal_strconv.c */
#include <arpa/inet.h> /* for inet_aton */
#include <sys/time.h> /* for inet_aton */

#include "cms_util.h"
#include "mdm_params.h"  /* for MDM_SINGLE_FULLPATH_LEN */
#include "oal.h"



#ifdef DMP_DEVICE2_BASELINE_1

extern UBOOL8 cmsUtl_isValidVpiVci(SINT32 vpi, SINT32 vci);

/* The vpiVciStr is in this format: VPI/VCI (e.g. 0/35) */
CmsRet cmsUtl_atmVpiVciStrToNum_dev2(const char *vpiVciStr, SINT32 *vpi, SINT32 *vci)
{
   char *pSlash;
   char vpiStr[BUFLEN_256];
   char vciStr[BUFLEN_256];
   
   *vpi = *vci = -1;   
   if (vpiVciStr == NULL)
   {
      cmsLog_error("vpiVciStr is NULL");
      return CMSRET_INVALID_ARGUMENTS;
   }      

   strncpy(vpiStr, vpiVciStr, sizeof(vpiStr));

   pSlash = (char *) strchr(vpiStr, '/');
   if (pSlash == NULL)
   {
      cmsLog_error("vpiVciStr %s is invalid", vpiVciStr);
      return CMSRET_INVALID_ARGUMENTS;
   }
   strncpy(vciStr, (pSlash + 1), sizeof(vciStr));
   *pSlash = '\0';       
   *vpi = atoi(vpiStr);
   *vci = atoi(vciStr);
   if (cmsUtl_isValidVpiVci(*vpi, *vci) == FALSE)
   {
      return CMSRET_INVALID_PARAM_VALUE;
   }     

   return CMSRET_SUCCESS;
}


CmsRet cmsUtl_atmVpiVciNumToStr_dev2(const SINT32 vpi, const SINT32 vci, char *vpiVciStr)
{
   if (vpiVciStr == NULL)
   {
      cmsLog_error("vpiVciStr is NULL");
      return CMSRET_INVALID_ARGUMENTS;
   }         
   if (cmsUtl_isValidVpiVci(vpi, vci) == FALSE)
   {
      return CMSRET_INVALID_PARAM_VALUE;
   }     

   sprintf(vpiVciStr, "%d/%d", vpi, vci);

   return CMSRET_SUCCESS;
   
}


#endif  /* DMP_DEVICE2_BASELINE_1 */


/*
 * The next couple were orginally intended for manipulating fullpaths in
 * the TR181 LowerLayers param, but turns out they can be used for
 * manipulating intfNames in a comma separated list or anything in
 * a comma separated list.
 * XXX TOOD: many of thse funcs can use strtok_r.
 * XXX TODO: consolidate with cmsUtl_isSubOptionPresent
 */
CmsRet cmsUtl_addFullPathToCSL(const char *fullPath, char *CSLBuf, UINT32 CSLlen)
{
   /* fully implemented and unit tested (suite_str.c:addFullPathToCSL1) */

   if (fullPath == NULL || CSLBuf == NULL)
   {
      cmsLog_error("NULL input params");
      return CMSRET_INVALID_PARAM_VALUE;
   }

   if (cmsUtl_isFullPathInCSL(fullPath, CSLBuf))
   {
      return CMSRET_SUCCESS;
   }

   if (cmsUtl_strlen(fullPath)+cmsUtl_strlen(CSLBuf)+2 > (SINT32) CSLlen)
   {
      cmsLog_error("CSLBuf len %d too small to add %s (currently %s)",
                    CSLlen, fullPath, CSLBuf);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   if (cmsUtl_strlen(CSLBuf) > 0)
   {
      /* there are already elements in the list, so add a comma first */
      strcat(CSLBuf, ",");
   }

   strcat(CSLBuf, fullPath);

   return CMSRET_SUCCESS;
}


void cmsUtl_deleteFullPathFromCSL(const char *fullPath, char *CSLBuf)
{
   UINT32 CSLlen;
   UINT32 fpLen;
   char *start;
   char *end;

   /* fully implemented and unit tested (suite_str.c:deleteFullPathFromCSL1) */

   fpLen = cmsUtl_strlen(fullPath);
   CSLlen = cmsUtl_strlen(CSLBuf);
   if ((fpLen == 0) || (CSLlen == 0))
   {
      /* nothing to delete, return success anyways. */
      return;
   }

   if (!cmsUtl_isFullPathInCSL(fullPath, CSLBuf))
   {
      /* fullpath is not in CSLBuf, so nothing to do */
      return;
   }

   start = strstr(CSLBuf, fullPath);
   end = start + fpLen;

   /* find the start of next fullpath, or end of CSLBuf */
   while (*end != '\0' && ((*end == ',') ||
                           (isspace(*end))))
   {
      end++;
   }

   if (*end == '\0')
   {
      /* we are deleting the last fullpath in CSLBuf, back up to delete
       * the last comma.
       */
      if (start > CSLBuf)
      {
         start--;
      }
   }

   /* pull up all the characters in CSLBuf */
   while (*end != '\0')
   {
      *start = *end;
      start++;
      end++;
   }

   /* zero out the rest of the buffer */
   while (start < (CSLBuf + CSLlen))
   {
      *start = '\0';
      start++;
   }

   return;
}


UBOOL8 cmsUtl_isFullPathInCSL(const char *fullPath, const char *CSLBuf)
{
   UINT32 i=0;
   UINT32 n;
   UINT32 CSLlen;

   /* fully implemented and unit tested (suite_str.c:isFullPathInCSL1) */

   CSLlen = cmsUtl_strlen(CSLBuf);
   if (fullPath == NULL || CSLlen == 0)
   {
      return FALSE;
   }

   n = strlen(fullPath);
   while (i < CSLlen)
   {
      /* advance past any leading white space.  This seems overly
       * paranoid.  Our code controls how this field is written, and we
       * never put any white space between comma and next fullpath.
       */
      while (i < CSLlen && isspace(CSLBuf[i]))
      {
         i++;
      }
      if (i >= CSLlen)
         break;

      /* a match is a match on the length of the fullpath string,
       * and the fullpath in the CSLBuf must be terminated with comma,
       * space, or end of string.
       */
      if (!strncmp(fullPath, &CSLBuf[i], n) &&
                 ((CSLBuf[i+n] == ',') ||
                  (isspace(CSLBuf[i+n]) ||
                  (CSLBuf[i+n] == '\0'))))
      {
         return TRUE;
      }

      /* advance i past next comma */
      while (i < CSLlen)
      {
         if (CSLBuf[i] == ',')
         {
            i++;
            break;
         }
         i++;
      }
   }

   return FALSE;
}



#ifdef DMP_DEVICE2_BASELINE_1

/** Look for fullpaths that are in srcBuf and not in dstBuf.
 *
 */
static CmsRet calcDiffFullPathCSLs(const char op,
                                   const char *srcBuf, const char *dstBuf,
                                   char *diffCSLBuf, UINT32 diffCSLBufLen)
{
   UINT32 srcIdx=0;
   CmsRet ret=CMSRET_SUCCESS;


   /* First iterate through the srcBuf and process fullpath one by one */
   while (srcBuf[srcIdx] != '\0')
   {
      char tmpBuf[MDM_SINGLE_FULLPATH_BUFLEN+1];
      UINT32 tmpIdx;

      /* suck in the first fullpath from the src buf */
      memset(tmpBuf, 0, sizeof(tmpBuf));
      tmpBuf[0] = op;
      tmpIdx = 1;
      while (srcBuf[srcIdx] != ',' && srcBuf[srcIdx] != '\0')
      {
         tmpBuf[tmpIdx++] = srcBuf[srcIdx++];
         if (tmpIdx >= sizeof(tmpBuf)-1)
         {
            cmsLog_error("fullpath too long (len=%d, tmpBuf=%s)", tmpIdx, tmpBuf);
            return CMSRET_RESOURCE_EXCEEDED;
         }
      }

      if (!cmsUtl_isFullPathInCSL(&tmpBuf[1], dstBuf))
      {
         /*
          * This fullpath in the srcBuf is not in the dstBuf, so add it to
          * the diffCSLBuf.
          */
         ret = cmsUtl_addFullPathToCSL(tmpBuf, diffCSLBuf, diffCSLBufLen);
         if (ret != CMSRET_SUCCESS)
         {
            return ret;
         }
      }

      /* now advance past the comma and any white space */
      while ((srcBuf[srcIdx] == ',') ||
              isspace(srcBuf[srcIdx]))
      {
         srcIdx++;
      }
   }

   return ret;
}


CmsRet cmsUtl_diffFullPathCSLs(const char *newLowerLayerBuf,
                               const char *currLowerLayerBuf,
                               char *diffCSLBuf,
                               UINT32 diffCSLBufLen)
{
   CmsRet ret=CMSRET_SUCCESS;

   /* Fully implemented and unit tested (suite_str.c:diffFullPathToCSL1,2,3) */

   memset(diffCSLBuf, 0, diffCSLBufLen);

   /* Look for fullpaths that were deleted from newLowerLayerBuf.
    * Do the delete first so interface stack processing can delete
    * these entries before adding new ones.
    */
   if ((ret == CMSRET_SUCCESS) && (cmsUtl_strlen(currLowerLayerBuf) > 0))
   {
      ret = calcDiffFullPathCSLs('-', currLowerLayerBuf, newLowerLayerBuf,
                                      diffCSLBuf, diffCSLBufLen);
   }

   /* Look for new fullpaths added in newLowerLayerBuf */
   if (cmsUtl_strlen(newLowerLayerBuf) > 0)
   {
      ret = calcDiffFullPathCSLs('+', newLowerLayerBuf, currLowerLayerBuf,
                                      diffCSLBuf, diffCSLBufLen);
   }

   return ret;
}


#endif /* DMP_DEVICE2_BASELINE_1 */
