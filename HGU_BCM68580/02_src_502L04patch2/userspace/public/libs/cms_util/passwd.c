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

#include "cms.h"
#include "cms_util.h"
#include "oal.h"
#include <time.h>

int i64c(int i)
{
    if (i <= 0)
        return ('.');
    if (i == 1)
        return ('/');
    if (i >= 2 && i < 12)
        return ('0' - 2 + i);
    if (i >= 12 && i < 38)
        return ('A' - 12 + i);
    if (i >= 38 && i < 63)
        return ('a' - 38 + i);
    return ('z');
}


char *cmsUtil_cryptMakeSalt(void)
{
    time_t now;
    static unsigned long x;
    static char result[]="$1$................";
    int i;

    for (i = 0; i < 16; i++)
    {
        time(&now);
        x += now + oal_getPid() + clock();
        result[3+i] = i64c(((x >> 18) ^ (x >> 6)) & 077);
        i++;
        result[3+i] = i64c(((x >> 12) ^ x) & 077);
    }
    
    return result;
}


// function to support passowrd hashing
char *cmsUtil_pwEncrypt(const char *clear, const char *salt)
{
    static char cipher[BUFLEN_48];
    char *cp;

#ifdef CONFIG_FEATURE_SHA1_PASSWORDS
    if (strncmp(salt, "$2$", 3) == 0) {
        return sha1_crypt(clear);
    }
#endif

    cp = oalPass_crypt(clear, salt);
    /* if crypt (a nonstandard crypt) returns a string too large,
       truncate it so we don't overrun buffers and hope there is
       enough security in what's left */
    if (strlen(cp) > sizeof(cipher)-1)
    {
        cp[sizeof(cipher)-1] = 0;
    }
    strcpy(cipher, cp);

    return cipher;
}
