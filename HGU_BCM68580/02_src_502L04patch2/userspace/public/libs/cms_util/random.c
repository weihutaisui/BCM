/***********************************************************************
 *
<:copyright-BRCM:2011:DUAL/GPL:standard

   Copyright (c) 2011 Broadcom 
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

#include <stdlib.h>
#include <string.h>

#include "cms.h"
#include "cms_util.h"

static char _randstrbuf[MAX_RANDSTRING_LEN+1];
static char *randspace="abcdefghijklmnopqurstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";


char *cmsRand_getRandString(UINT32 minchars, UINT32 maxchars)
{
   UINT32 i;
   int mod;

   if (maxchars > MAX_RANDSTRING_LEN)
      return NULL;

   if (maxchars < minchars)
      return NULL;

   if (minchars < 1)
      return NULL;

   memset(_randstrbuf, 0, sizeof(_randstrbuf));
   for (i=0; i < maxchars; i++)
   {
      // first char is a-z, rest of chars can be anything
      mod = (0==i) ? 26 : strlen(randspace);
      _randstrbuf[i] = randspace[rand() % mod];

      if (i+1 >= minchars)
      {
         // we've satisfied the minchars requirement, see if we break
         if (0 == (rand() % 2))
            break;
      }
   }

   return _randstrbuf;
}
