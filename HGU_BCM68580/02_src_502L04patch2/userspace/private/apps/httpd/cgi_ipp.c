/***********************************************************************
 *
 *  Copyright (c) 2000-2010  Broadcom Corporation
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

#ifdef SUPPORT_IPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "cms.h"
#include "cms_dal.h"
#include "cms_net.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_main.h"
#include "cgi_ipp.h"

/* local constants */


/* Local functions */
static void cgiIppCfgSavApply(char *query, FILE *fs);
static void cgiIppCfgView(FILE *fs);


void cgiIppCfg(char *query, FILE *fs)
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);
   if (strcmp(action, "savapply") == 0)
   { 
      cgiIppCfgSavApply(query, fs);
   }
   else
   {
      cgiIppCfgView(fs);
   }
}  /* End of cgiIppCfg() */

void cgiIppCfgView(FILE *fs)
{
   do_ej("/webs/ippcfg.html", fs);

}  /* End of cgiIppCfgView() */

void cgiIppCfgSavApply(char *query, FILE *fs)
{
   char cmd[WEB_BUF_SIZE_MAX];
   char ippEnabled[BUFLEN_8];
   char *newIppMake;
   char *newIppName;
   UBOOL8 newIppEnabled = FALSE;
   UBOOL8 changed = FALSE;
   InstanceIdStack iidStack;
   IppCfgObject *ippObj = NULL;
   CmsRet ret;

   /* get the current lan host configuration */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_IPP_CFG, &iidStack, 0, (void **)&ippObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_IPP_CFG> returns error. ret=%d", ret);
      return;
   }

   ippEnabled[0]  = '\0';
   cgiGetValueByName(query, "ippEnabled", ippEnabled);
   newIppEnabled = atoi(ippEnabled)? TRUE : FALSE;

   if (cgiGetValueByName(query, "ippMake", cmd) == CGI_STS_OK)
   {
      newIppMake = cmsMem_strdup(cmd);
   }
   else
   {
      newIppMake = cmsMem_strdup(ippObj->make);
   }

   if (cgiGetValueByName(query, "ippName", cmd) == CGI_STS_OK)
   {
      newIppName = cmsMem_strdup(cmd);
   }
   else
   {
      newIppName = cmsMem_strdup(ippObj->name);
   }

   /* set object if any of the parameters changes */
   if (newIppEnabled != ippObj->enable)
   {
      /* overwrite with user's configuration */
      ippObj->enable = newIppEnabled;
      changed = TRUE;
   }
   if (cmsUtl_strcmp(newIppMake, ippObj->make) != 0)
   {
      /* overwrite with user's configuration */
      cmsMem_free(ippObj->make);
      ippObj->make = cmsMem_strdup(newIppMake);
      changed = TRUE;
   }
   if (cmsUtl_strcmp(newIppName, ippObj->name) != 0)
   {
      /* overwrite with user's configuration */
      cmsMem_free(ippObj->name);
      ippObj->name = cmsMem_strdup(newIppName);
      changed = TRUE;
   }

   if (changed)
   {
      if ((ret = cmsObj_set((void *)ippObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_set returns error. ret=%d", ret);
         sprintf(cmd, "Print Server Configuration failed. ret=%d", ret);
         cgiWriteMessagePage(fs, "Printer Server Configuration Error", cmd, "");
      }
      else
      {
         glbSaveConfigNeeded = TRUE;
         cgiIppCfgView(fs);
      }
   }
   else
   {
      cmsLog_debug("There is no change in print server configuration");
   }

   cmsMem_free(newIppMake);
   cmsMem_free(newIppName);
   cmsObj_free((void **)&ippObj);

}  /* End of cgiIppCfgSavApply() */

#endif   /* SUPPORT_IPP */

