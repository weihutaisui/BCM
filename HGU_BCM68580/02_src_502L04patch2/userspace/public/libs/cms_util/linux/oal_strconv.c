/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "../oal.h"

CmsRet oal_strtol(const char *str, char **endptr, SINT32 base, SINT32 *val)
{
   CmsRet ret=CMSRET_SUCCESS;
   char *localEndPtr=NULL;

   errno = 0;  /* set to 0 so we can detect ERANGE */

   *val = strtol(str, &localEndPtr, base);

   if ((errno != 0) || (*localEndPtr != '\0'))
   {
      *val = 0;
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   if (endptr != NULL)
   {
      *endptr = localEndPtr;
   }

   return ret;
}


CmsRet oal_strtoul(const char *str, char **endptr, SINT32 base, UINT32 *val)
{
   CmsRet ret=CMSRET_SUCCESS;
   char *localEndPtr=NULL;

   /*
    * Linux strtoul allows a minus sign in front of the number.
    * This seems wrong to me.  Specifically check for this and reject
    * such strings.
    */
   while (isspace(*str))
   {
      str++;
   }
   if (*str == '-')
   {
      if (endptr)
      {
         *endptr = (char *) str;
      }
      *val = 0;
      return CMSRET_INVALID_ARGUMENTS;
   }

   errno = 0;  /* set to 0 so we can detect ERANGE */

   *val = strtoul(str, &localEndPtr, base);

   if ((errno != 0) || (*localEndPtr != '\0'))
   {
      *val = 0;
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   if (endptr != NULL)
   {
      *endptr = localEndPtr;
   }

   return ret;
}


CmsRet oal_strtol64(const char *str, char **endptr, SINT32 base, SINT64 *val)
{
   CmsRet ret=CMSRET_SUCCESS;
   char *localEndPtr=NULL;

   errno = 0;  /* set to 0 so we can detect ERANGE */

   *val = strtoll(str, &localEndPtr, base);

   if ((errno != 0) || (*localEndPtr != '\0'))
   {
      *val = 0;
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   if (endptr != NULL)
   {
      *endptr = localEndPtr;
   }

   return ret;
}


CmsRet oal_strtoul64(const char *str, char **endptr, SINT32 base, UINT64 *val)
{
   CmsRet ret=CMSRET_SUCCESS;
   char *localEndPtr=NULL;

   /*
    * Linux strtoul allows a minus sign in front of the number.
    * This seems wrong to me.  Specifically check for this and reject
    * such strings.
    */
   while (isspace(*str))
   {
      str++;
   }
   if (*str == '-')
   {
      if (endptr)
      {
         *endptr = (char *) str;
      }
      *val = 0;
      return CMSRET_INVALID_ARGUMENTS;
   }

   errno = 0;  /* set to 0 so we can detect ERANGE */

   *val = strtoull(str, &localEndPtr, base);

   if ((errno != 0) || (*localEndPtr != '\0'))
   {
      *val = 0;
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   if (endptr != NULL)
   {
      *endptr = localEndPtr;
   }

   return ret;
}

void oal_getRandomBytes(unsigned char *buf __attribute__((unused)), int len __attribute__((unused)))
{
   /* this routine get 16 bytes of random numbers and store it in *buf.
    * In Linux, /dev/urandom is a good source for such random numbers.  A lot of other
    * OS also have this.   Otherwise, get random bytes using another method.
    */
}

/* this function is really linux specific.  A few of other OS also have similar capability.
 * If not, cmUtl_generateUuidStrFromRandom() can call oal_getRandomBytes() above to get 16
 * random bytes, then the bits are being set correctly according to RFC4122 UUID version 4.
 * Alternatively, other versions of UUID can also be generated. 
 */
#define KERNEL_RANDOM_UUID "/proc/sys/kernel/random/uuid"
int oal_getRandomUuid(char *uuidStr, int len)
{
   FILE *fp = NULL;
   int bytesRead = 0;

   if ((fp = fopen(KERNEL_RANDOM_UUID, "r")) == NULL)
   {
      cmsLog_error("failed to open file %s",KERNEL_RANDOM_UUID);
      return (bytesRead);
   }      

   memset(uuidStr, 0, len);
   if( fgets(uuidStr, len, fp) )
   {
      bytesRead = strlen(uuidStr);
      uuidStr[bytesRead-1] = '\0';
   }

   fclose(fp);
   
   return bytesRead;
}
