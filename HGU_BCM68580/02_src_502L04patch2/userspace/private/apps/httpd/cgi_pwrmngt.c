#ifdef DMP_X_BROADCOM_COM_PWRMNGT_1 /* aka SUPPORT_PWRMNGT */
/***********************************************************************
 *
 *  Copyright (c) 2008-2010  Broadcom Corporation
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
#include "cgi_pwrmngt.h"

void cgiPowerManagement(char *query, FILE *fs) 
{
   CmsRet ret = CMSRET_SUCCESS;
   char path[BUFLEN_1024];
   char cgiVal[4];

   cmsLog_debug("cgiPowerManagement");
   if (cgiGetValueByName(query, "PmCPUSpeed", cgiVal) == CGI_STS_OK)
      glbWebVar.pmCPUSpeed = atoi(cgiVal);
   if (cgiGetValueByName(query, "PmCPUr4kWaitEn", cgiVal) == CGI_STS_OK)
      glbWebVar.pmCPUr4kWaitEn = atoi(cgiVal);
   if (cgiGetValueByName(query, "PmDRAMSelfRefreshEn", cgiVal) == CGI_STS_OK)
      glbWebVar.pmDRAMSelfRefreshEn = atoi(cgiVal);
   if (cgiGetValueByName(query, "PmEthAutoPwrDwnEn", cgiVal) == CGI_STS_OK)
      glbWebVar.pmEthAutoPwrDwnEn = atoi(cgiVal);
   if (cgiGetValueByName(query, "PmEthEEE", cgiVal) == CGI_STS_OK)
      glbWebVar.pmEthEEE = atoi(cgiVal);
   if (cgiGetValueByName(query, "PmAvsEn", cgiVal) == CGI_STS_OK)
      glbWebVar.pmAvsEn = atoi(cgiVal);

   if ((ret = dalPowerManagement(&glbWebVar)) != CMSRET_SUCCESS) 
   {
      cmsLog_error("dalPowerManagement failed, ret=%d", ret);
      return;
   }
   else 
   {
      glbSaveConfigNeeded = TRUE;
   }
   makePathToWebPage(path, sizeof(path), "pwrmngt.html");
   do_ej(path, fs);

#ifdef BRCM_WLAN
#if defined(SUPPORT_DM_DETECT)
   if(cmsMdm_isDataModelDevice2())
   {
#endif
#if defined(SUPPORT_TR181_WLMNGR)
	   cmsLck_releaseLock();
	   /* dm power save will accss mdm with lock, so it should unlock it */
	   wlcsm_mngr_dm_pwr_reset();
	   if ((cmsLck_acquireLockWithTimeout(HTTPD_LOCK_TIMEOUT) != CMSRET_SUCCESS)) {
		   cmsLog_error("could not send lock back\n");
	   }

#endif
#if defined(SUPPORT_DM_DETECT)
   } else 
#endif
#if !defined(SUPPORT_TR181_WLMNGR) || defined(SUPPORT_DM_DETECT)
   {
   static char buf[sizeof(CmsMsgHeader) + 32]={0};
   /* send a changed notification to wlmngr */
   CmsMsgHeader *msg=(CmsMsgHeader *) buf;
   sprintf((char *)(msg + 1), "PwrMngt");

   msg->dataLength = 32;
   msg->type = CMS_MSG_WLAN_CHANGED;
   msg->src = EID_HTTPD;
   msg->dst = EID_WLMNGR;
   msg->flags_event = 1;
   msg->flags_request = 0;

   if ((ret = cmsMsg_send(msgHandle, msg)) != CMSRET_SUCCESS)
          cmsLog_error("could not send CMS_MSG_WLAN_CHANGED msg to wlmngr, ret=%d", ret);
   else
          cmsLog_debug("message CMS_MSG_WLAN_CHANGED sent successfully");
   }
#endif
#endif

   return;

} /* cgiPowerManagementCfg */

void cgiGetPowerManagement(int argc __attribute__((unused)),
                           char **argv __attribute__((unused)),
                           char *varValue __attribute__((unused)))
{
   cmsLog_debug("cgiGetPowerManagement");
   dalGetPowerManagement(&glbWebVar);

   return;
}
#endif /* aka SUPPORT_PWRMNGT */
