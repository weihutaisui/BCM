/***********************************************************************
 *
 *  Copyright (c) 2006-2010  Broadcom Corporation
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

#ifdef SUPPORT_IPV6

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "cms.h"
#include "cms_dal.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_main.h"
#include "cgi_lan6.h"
#include "syscall.h"
#include "qdm_ipv6.h"


/* Local functions */
static void cgiIPv6LanCfgSavApply(char *query, FILE *fs);
static void cgiIPv6LanCfgView(FILE *fs);
static void cgi_generateRandomUla(char *prefix);

void cgiIPv6LanCfg(char *query, FILE *fs)
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);
   if (strcmp(action, "savapply") == 0)
   { 
      cgiIPv6LanCfgSavApply(query, fs);
   }
   else
   {
      cgiIPv6LanCfgView(fs);
   }
}  /* End of cgiIPv6LanCfg() */

void cgiIPv6LanCfgView(FILE *fs)
{
   do_ej("/webs/ipv6lancfg.html", fs);

}  /* End of cgiIPv6LanCfgView() */

void cgi_generateRandomUla(char *prefix)
{
   unsigned char rand[5];
   int len = 5;
   int dev_random_fd;
   ssize_t rcount;


   dev_random_fd = open("/dev/urandom", O_RDONLY|O_NONBLOCK);
   if ( dev_random_fd < 0 ) {
      cmsLog_error("Could not open /dev/urandom");
      return;
   }

   rcount = read(dev_random_fd, (void *)rand, len);
   close(dev_random_fd);

   if (rcount != (ssize_t)len)
   {
      cmsLog_error("unexpected read count, got %d wanted %d", (int)rcount, len);
   }

   sprintf(prefix, "fd%02x:%02x%02x:%02x%02x::/64", rand[0], rand[1], rand[2], rand[3], rand[4]);
}


void cgiIPv6LanCfgSavApply(char *query, FILE *fs)
{
   char cmd[WEB_BUF_SIZE_MAX];
   char enblDhcp6s[BUFLEN_8];
   char enblRadvd[BUFLEN_8];
   char enblDhcp6sStateful[BUFLEN_8];
   char InterfaceIDBgn[CMS_IPADDR_LENGTH];
   char InterfaceIDEnd[CMS_IPADDR_LENGTH];
   char LanIntfAddr6[CMS_IPADDR_LENGTH];
   char LeasedTimeStr[BUFLEN_8];
   char enblRadvdUla[BUFLEN_8];
   char enblRandomUla[BUFLEN_8];
   char ulaPrefix[CMS_IPADDR_LENGTH];
   char ulaPlt[BUFLEN_8];
   char ulaVlt[BUFLEN_8];
   CmsRet ret;
#ifdef DMP_X_BROADCOM_COM_MLDSNOOP_1
   char mldVar[BUFLEN_8];
#endif

   cmsLog_debug("Enter");

   /* initialize new settings to current settings */
   enblDhcp6s[0] = enblRadvd[0] = cmd[0] = enblDhcp6sStateful[0] = '\0';
   enblRadvdUla[0] = enblRandomUla[0] = ulaPrefix[0] = ulaPlt[0] = ulaVlt[0] = '\0';
   InterfaceIDBgn[0] = InterfaceIDEnd[0] = LanIntfAddr6[0] = LeasedTimeStr[0] = '\0';

   cgiGetValueByName(query, "enblDhcp6s", enblDhcp6s);
   cgiGetValueByName(query, "enblRadvd", enblRadvd);
   cgiGetValueByName(query, "enblDhcp6sStateful", enblDhcp6sStateful);
   cgiGetValueByName(query, "dhcp6LeasedTime", LeasedTimeStr);
   cgiGetValueByName(query, "ipv6IntfIDStart", InterfaceIDBgn);
   cgiGetValueByName(query, "ipv6IntfIDEnd", InterfaceIDEnd);
   cgiGetValueByName(query, "lanIntfAddr6", glbWebVar.lanIntfAddr6);
   cgiGetValueByName(query, "enblRadvdUla", enblRadvdUla);
   cgiGetValueByName(query, "ipv6UlaRandom", enblRandomUla);
   cgiGetValueByName(query, "ipv6UlaPrefix", ulaPrefix);
   cgiGetValueByName(query, "ipv6UlaPlt", ulaPlt);
   cgiGetValueByName(query, "ipv6UlaVlt", ulaVlt);

   glbWebVar.enblDhcp6s = atoi(enblDhcp6s)? TRUE : FALSE;
   glbWebVar.enblRadvd  = atoi(enblRadvd)?  TRUE : FALSE;

   if ( InterfaceIDBgn[0] != '\0' )
   {
      cmsUtl_strncpy(glbWebVar.ipv6IntfIDStart, InterfaceIDBgn, CMS_IPADDR_LENGTH);
   }

   if ( InterfaceIDEnd[0] != '\0' )
   {
      cmsUtl_strncpy(glbWebVar.ipv6IntfIDEnd, InterfaceIDEnd, CMS_IPADDR_LENGTH);
   }

   if ( LeasedTimeStr[0] != '\0' )
   {
       glbWebVar.dhcp6LeasedTime = atoi(LeasedTimeStr);
   }

   if ( enblDhcp6sStateful[0] != '\0' )
   {
       glbWebVar.enblDhcp6sStateful = atoi(enblDhcp6sStateful)? TRUE : FALSE;
   }

   if ( enblRadvdUla[0] != '\0' )
   {
       glbWebVar.enblRadvdUla = atoi(enblRadvdUla)? TRUE : FALSE;
   }

   if ( enblRandomUla[0] != '\0' )
   {
       glbWebVar.enblRandomULA = atoi(enblRandomUla)? TRUE : FALSE;
   }

   cmsUtl_strncpy(glbWebVar.ipv6UlaPrefix, ulaPrefix, CMS_IPADDR_LENGTH);

   if ( ulaPlt[0] != '\0' )
   {
       glbWebVar.ipv6UlaPlt= atoi(ulaPlt);
   }

   if ( ulaVlt[0] != '\0' )
   {
       glbWebVar.ipv6UlaVlt= atoi(ulaVlt);
   }

#ifdef DMP_X_BROADCOM_COM_MLDSNOOP_1
   /* MLD snooping */
   cgiGetValueByName(query, "enblMldSnp", mldVar);
   glbWebVar.enblMldSnp = atoi(mldVar);
   cgiGetValueByName(query, "enblMldMode", mldVar);
   glbWebVar.enblMldMode = atoi(mldVar);
   cgiGetValueByName(query, "enblMldLanToLanMcast", mldVar);
   glbWebVar.enblMldLanToLanMcast = atoi(mldVar);
#endif

   if ( glbWebVar.enblRadvdUla )
   {
      if ( glbWebVar.enblRandomULA )
      {
         if ( IS_EMPTY_STRING(glbWebVar.ipv6UlaPrefix) )
         {
            cgi_generateRandomUla(glbWebVar.ipv6UlaPrefix);
         }
         else
         {
            cmsLog_debug("ULA prefix is generated before");
         }
      }
      else
      {
         if ( IS_EMPTY_STRING(glbWebVar.ipv6UlaPrefix) )
         {
            cmsLog_error("ULA prefix is not configured");
         }
      }
   }

   if ((ret = dalLan_setLan6Cfg(&glbWebVar)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_set returns error. ret=%d", ret);
      sprintf(cmd, "IPv6 LAN Host Configuration failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "IPv6 LAN Host Configuration Error", cmd, "");
      return;
   }

   glbSaveConfigNeeded = TRUE;

   cgiIPv6LanCfgView(fs);
   
}  /* End of cgiIPv6LanCfgSavApply() */


void cgiGetEnblRadvd(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue)
{
   UBOOL8 enabled;

   cmsLog_debug("Enter");
   varValue[0] = '\0';

   enabled = qdmIpv6_isRadvdEnabled();
   sprintf(varValue, "%d", enabled);
}  /* End of cgiGetEnblRadvd() */

void cgiGetEnblDhcp6s(int argc __attribute__((unused)),  char **argv __attribute__((unused)), char *varValue)
{
   UBOOL8 enabled;

   cmsLog_debug("Enter");
   varValue[0] = '\0';

   enabled = qdmIpv6_isDhcp6sEnabled();
   sprintf(varValue, "%d", enabled);
}  /* End of cgiGetEnblDhcp6s() */

#endif   /* SUPPORT_IPV6 */

