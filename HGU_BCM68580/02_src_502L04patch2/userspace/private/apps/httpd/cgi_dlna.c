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

#ifdef DMP_X_BROADCOM_COM_DLNA_1

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
#include "cgi_dlna.h"

#include "cms_qdm.h"

/* local constants */


/* Local functions */
static void cgiDlnaCfgSavApply(char *query, FILE *fs);
static void cgiDlnaCfgView(FILE *fs);


void cgiDlnaCfg(char *query, FILE *fs)
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);
   if (strcmp(action, "savapply") == 0)
   { 
      cgiDlnaCfgSavApply(query, fs);
   }
   else
   {
      cgiDlnaCfgView(fs);
   }
}  /* End of cgiDlnaCfg() */

void cgiDlnaCfgView(FILE *fs)
{
   do_ej("/webs/dlnacfg.html", fs);

}  /* End of cgiDlnaCfgView() */

void cgiDlnaCfgSavApply(char *query, FILE *fs)
{
   char cmd[WEB_BUF_SIZE_MAX];
   char dmsEnabled[BUFLEN_8];
   char *newDmsMediaPath;
   UBOOL8 newDmsEnabled = FALSE;
   UBOOL8 changed = FALSE;
   InstanceIdStack iidStack;
   DmsCfgObject *dmsObj = NULL;
   CmsRet ret;

   /* get the current dms configuration */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_DMS_CFG, &iidStack, 0, (void **)&dmsObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_DMS_CFG> returns error. ret=%d", ret);
      return;
   }

   dmsEnabled[0]  = '\0';
   cgiGetValueByName(query, "dmsEnabled", dmsEnabled);
   newDmsEnabled = atoi(dmsEnabled)? TRUE : FALSE;

   if (cgiGetValueByName(query, "dmsMediaPath", cmd) == CGI_STS_OK)
   {
      newDmsMediaPath = cmsMem_strdup(cmd);
      if (cmsUtl_strcmp(newDmsMediaPath, dmsObj->mediaPath) != 0)
      {
         /* overwrite with user's configuration */
         cmsMem_free(dmsObj->mediaPath);
         dmsObj->mediaPath = cmsMem_strdup(newDmsMediaPath);
         changed = TRUE;
      }
      cmsMem_free(newDmsMediaPath);
   }

   if (cgiGetValueByName(query, "dmsBrName", cmd) == CGI_STS_OK)
   {
      SINT32 newDmsBrKey=0;
      qdmIntf_getIntfKeyByGroupName(cmd, (UINT32 *) &newDmsBrKey);
      if (dmsObj->brKey != (UINT32) newDmsBrKey  )
      {
         /* overwrite with user's configuration */
         dmsObj->brKey= newDmsBrKey;
         changed = TRUE;
      }
   }

   /* set object if any of the parameters changes */
   if (newDmsEnabled != dmsObj->enable)
   {
      /* overwrite with user's configuration */
      dmsObj->enable = newDmsEnabled;
      changed = TRUE;
   }

   if (changed)
   {
      if ((ret = cmsObj_set((void *)dmsObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_set returns error. ret=%d", ret);
         sprintf(cmd, "Digital Media Server Configuration failed. ret=%d", ret);
         cgiWriteMessagePage(fs, "Digital Media Server Configuration Error", cmd, "");
      }
      else
      {
         glbSaveConfigNeeded = TRUE;
         cgiDlnaCfgView(fs);
      }
   }
   else
   {
      cgiDlnaCfgView(fs);
      cmsLog_debug("There is no change in DMS server configuration");
   }

   cmsObj_free((void **)&dmsObj);

}  /* End of cgiDlnaCfgSavApply() */

#endif   /* DMP_X_BROADCOM_COM_DLNA_1 */

