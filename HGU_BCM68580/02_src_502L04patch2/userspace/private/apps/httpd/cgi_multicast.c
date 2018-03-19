/***********************************************************************
 *
 *  Copyright (c) 2006  Broadcom Corporation
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
#if defined(DMP_X_BROADCOM_COM_IGMP_1) || defined(DMP_X_BROADCOM_COM_MLD_1) || defined(DMP_X_BROADCOM_COM_MCAST_1)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "httpd.h"
#include "cgi_main.h"
#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cgi_multicast.h"

#if defined(DMP_X_BROADCOM_COM_MCAST_1)
void cgiMulticastExceptionAdd(char *query, FILE *fs)
{
  CmsRet sts = CMSRET_SUCCESS;
  char ip[16]={0};
  char mask[16]={0};
  char path[BUFLEN_1024];

  cgiGetValueByName(query, "ip", ip);
  cgiGetValueByName(query, "mask", mask);

  cmsLog_debug("cgiMulticastException Add (%s)(%s)", ip, mask);

  sts = dalMulticast_AddException(ip, mask);

  if ( sts == CMSRET_SUCCESS )
  {
    glbSaveConfigNeeded = TRUE;
    makePathToWebPage(path, sizeof(path), "multicast.html");
    do_ej(path, fs);
  }
}

void cgiMulticastExceptionRemove(char *query, FILE *fs)
{
  CmsRet sts = CMSRET_SUCCESS;
  char* rmList = NULL;
  char path[BUFLEN_1024];

  cgiGetValueByNameSafe(query, "rmLst", &rmList, NULL, 0);  // it will allocate memory for me.
  cmsLog_debug("cgiMulticastException Remove List (%s)", rmList);
  sts = dalMulticast_RemoveException(rmList);
  cmsMem_free(rmList);  // thanks for the memory

  if ( sts == CMSRET_SUCCESS )
  {
    glbSaveConfigNeeded = TRUE;
    makePathToWebPage(path, sizeof(path), "multicast.html");
    do_ej(path, fs);
  }
}

void cgiMulticastExceptionAddMld(char *query, FILE *fs)
{
  CmsRet sts = CMSRET_SUCCESS;
  char ipv6[41]={0};
  char mask[41]={0};
  char path[BUFLEN_1024];

  cgiGetValueByName(query, "ipv6", ipv6);
  cgiGetValueByName(query, "mask", mask);

  cmsLog_debug("cgiMulticastExceptionMld Add (%s)(%s)", ipv6, mask);

  sts = dalMulticast_AddExceptionMld(ipv6, mask);

  if ( sts == CMSRET_SUCCESS )
  {
    glbSaveConfigNeeded = TRUE;
    makePathToWebPage(path, sizeof(path), "multicast.html");
    do_ej(path, fs);
  }
}

void cgiMulticastExceptionRemoveMld(char *query, FILE *fs)
{
  CmsRet sts = CMSRET_SUCCESS;
  char* rmList = NULL;
  char path[BUFLEN_1024];

  cgiGetValueByNameSafe(query, "rmLst", &rmList, NULL, 0);  // it will allocate memory for me.
  cmsLog_debug("cgiMulticastExceptionMld Remove List (%s)", rmList);
  sts = dalMulticast_RemoveExceptionMld(rmList);
  cmsMem_free(rmList);  // thanks for the memory

  if ( sts == CMSRET_SUCCESS )
  {
    glbSaveConfigNeeded = TRUE;
    makePathToWebPage(path, sizeof(path), "multicast.html");
    do_ej(path, fs);
  }
}

// Handle a set command of the Multicast Exception
void cgiMulticastException(char *query, FILE *fs)
{
  char action[BUFLEN_264];

  cgiGetValueByName(query, "action", action );

  if ( strcmp(action, "add") == 0 )
  {
    cgiMulticastExceptionAdd( query, fs );
  }
  else if ( strcmp(action, "remove") == 0 )
  {
    cgiMulticastExceptionRemove( query, fs );
  }
  else if ( strcmp(action, "addMld") == 0 )
  {
    cgiMulticastExceptionAddMld( query, fs );
  }
  else if ( strcmp(action, "removeMld") == 0 )
  {
    cgiMulticastExceptionRemoveMld( query, fs );
  }
}
#endif

void cgiMulticast(char *query, FILE *fs) 
{
   CmsRet ret = CMSRET_SUCCESS;
   char path[BUFLEN_1024];
   char cgiVal[5];

   cmsLog_debug("cgiMulticast");

#ifdef DMP_X_BROADCOM_COM_MCAST_1
   cgiGetValueByName(query, "mcastPrecedence", cgiVal);
   glbWebVar.mcastPrecedence = atoi(cgiVal);
   cgiGetValueByName(query, "mcastStrictWan", cgiVal);
   glbWebVar.mcastStrictWan = atoi(cgiVal);
#ifdef DMP_X_BROADCOM_COM_DCSP_MCAST_REMARK_1
   cgiGetValueByName(query, "mcastDscpRemarkEnable", cgiVal);
   glbWebVar.mcastDscpRemarkEnable= atoi(cgiVal);
   cgiGetValueByName(query, "mcastDscpRemarkVal", cgiVal);
   glbWebVar.mcastDscpRemarkVal = atoi(cgiVal);
#endif
#endif   
 
#ifdef DMP_X_BROADCOM_COM_IGMP_1
   cgiGetValueByName(query, "igmpVer", cgiVal);
   glbWebVar.igmpVer = atoi(cgiVal);
   cgiGetValueByName(query, "igmpQI", cgiVal);
   glbWebVar.igmpQI = atoi(cgiVal);
   cgiGetValueByName(query, "igmpQRI", cgiVal);
   glbWebVar.igmpQRI = atoi(cgiVal);
   cgiGetValueByName(query, "igmpLMQI", cgiVal);
   glbWebVar.igmpLMQI = atoi(cgiVal);
   cgiGetValueByName(query, "igmpRV", cgiVal);
   glbWebVar.igmpRV = atoi(cgiVal);
   cgiGetValueByName(query, "igmpMaxGroups", cgiVal);
   glbWebVar.igmpMaxGroups = atoi(cgiVal);
   cgiGetValueByName(query, "igmpMaxSources", cgiVal);
   glbWebVar.igmpMaxSources = atoi(cgiVal);
   cgiGetValueByName(query, "igmpMaxMembers", cgiVal);
   glbWebVar.igmpMaxMembers = atoi(cgiVal);
   cgiGetValueByName(query, "igmpFastLeaveEnable", cgiVal);
   glbWebVar.igmpFastLeaveEnable = atoi(cgiVal);
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
   cgiGetValueByName(query, "mldVer", cgiVal);
   glbWebVar.mldVer = atoi(cgiVal);
   cgiGetValueByName(query, "mldQI", cgiVal);
   glbWebVar.mldQI = atoi(cgiVal);
   cgiGetValueByName(query, "mldQRI", cgiVal);
   glbWebVar.mldQRI = atoi(cgiVal);
   cgiGetValueByName(query, "mldLMQI", cgiVal);
   glbWebVar.mldLMQI = atoi(cgiVal);
   cgiGetValueByName(query, "mldRV", cgiVal);
   glbWebVar.mldRV = atoi(cgiVal);
   cgiGetValueByName(query, "mldMaxGroups", cgiVal);
   glbWebVar.mldMaxGroups = atoi(cgiVal);
   cgiGetValueByName(query, "mldMaxSources", cgiVal);
   glbWebVar.mldMaxSources = atoi(cgiVal);
   cgiGetValueByName(query, "mldMaxMembers", cgiVal);
   glbWebVar.mldMaxMembers = atoi(cgiVal);
   cgiGetValueByName(query, "mldFastLeaveEnable", cgiVal);
   glbWebVar.mldFastLeaveEnable = atoi(cgiVal);
#endif

   if ((ret = dalSetMulticastCfg(&glbWebVar)) != CMSRET_SUCCESS) 
   {
      cmsLog_error("dalMulticast failed, ret=%d", ret);
      return;
   }
   else 
   {
      glbSaveConfigNeeded = TRUE;
   }
   makePathToWebPage(path, sizeof(path), "multicast.html");
   do_ej(path, fs);

   return;

} /* cgiMulticast */

void cgiGetMulticastInfo(int argc __attribute__((unused)),
                         char **argv __attribute__((unused)),
                         char *varValue __attribute__((unused)))
{
   cmsLog_debug("cgiGetMulticastInfo");

   dalGetMulticastCfg(&glbWebVar);

   return;
}
#endif 
