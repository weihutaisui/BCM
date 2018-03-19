/***********************************************************************
 *
 *  Copyright (c) 2005-2007  Broadcom Corporation
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

#include "cms.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_ipsec.h"
#include "cgi_main.h"
#include "secapi.h"
#include "syscall.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"

#ifdef DMP_DEVICE2_IPSEC_1

extern WEB_NTWK_VAR glbWebVar;

#if defined(SUPPORT_DM_PURE181) || defined(SUPPORT_DM_DETECT)
void fillIPSecInfo_dev2(FILE *fs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;    
   Dev2IpsecFilterObject *filterObj = NULL;

   while ( cmsObj_getNext(MDMOID_DEV2_IPSEC_FILTER, &iidStack,
                          (void **) &filterObj) == CMSRET_SUCCESS )
   {
      MdmPathDescriptor pathDesc;
      Dev2IpsecProfileObject *profileObj = NULL;

      fprintf(fs, "   <tr>\n");
      fprintf(fs, "      <td>%s</td>\n", filterObj->X_BROADCOM_COM_TunnelName);

      /* Get remoteGWAddress from associated profile object */
      INIT_PATH_DESCRIPTOR(&pathDesc);
      cmsMdm_fullPathToPathDescriptor(filterObj->profile, &pathDesc);
      if (cmsObj_get(MDMOID_DEV2_IPSEC_PROFILE, &pathDesc.iidStack, 0,
                            (void **) &profileObj) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get profileObj");
      }

      fprintf(fs, "      <td>%s</td>\n", profileObj->remoteEndpoints);
      fprintf(fs, "      <td>%s</td>\n", filterObj->sourceIP);
      fprintf(fs, "      <td>%s</td>\n", filterObj->destIP);
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", filterObj->X_BROADCOM_COM_TunnelName);
      fprintf(fs, "   </tr>\n");

      cmsObj_free((void **) &profileObj);
      cmsObj_free((void **) &filterObj);
   }

   return;
}

int bcmIPSecNumEntries_dev2()
{
   int numTunCfg = 0;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpsecFilterObject *filterObj = NULL;

   while (cmsObj_getNextFlags(MDMOID_DEV2_IPSEC_FILTER, &iidStack, 
                    OGF_NO_VALUE_UPDATE, (void **)&filterObj) == CMSRET_SUCCESS)
   {
      if (filterObj->enable)
      {
         numTunCfg++;
      }
      cmsObj_free((void **)&filterObj);
   }

   return numTunCfg;
}
#endif


/*
 * cgiPrintEnetDiag is replacing BcmLan_printEnetDiag. in utils.c.
 * This routine print ENET diagnostics page.
 */
void cgiPrintIPSecTable_dev2(char *varValue __attribute__((unused)))
{
#if 0
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPSecCfgObject *ipsecObj=NULL;
   char *prtloc = varValue;
   char chkstr[10];
   int  wsize;

   cmsLog_debug("Enter");

   while (cmsObj_getNext(MDMOID_IP_SEC_CFG, &iidStack,
                         (void **) &ipsecObj) == CMSRET_SUCCESS)
   {
      UINT32 inst = iidStack.instance[iidStack.currentDepth-1];
      if (ipsecObj->enable) {
         strcpy(chkstr, "checked");
      } else {
         strcpy(chkstr, "");
      }
      sprintf(prtloc, "<tr>\n%n", &wsize); prtloc +=wsize;
      sprintf(prtloc, "<td align='center'><input type=checkbox name='connEn%d' %s onClick='enClick(%d, document.forms[0].connEn%d);'></td>\n%n", 
      inst, chkstr, inst, inst, &wsize); prtloc +=wsize;
      sprintf(prtloc, "<td align='center'>%s</td>\n%n", ipsecObj->connName, &wsize); prtloc +=wsize;
      sprintf(prtloc, "<td align='center'>%s</td>\n%n", ipsecObj->remoteGWAddress, &wsize); prtloc +=wsize;
      sprintf(prtloc, "<td align='center'>%s</td>\n%n", ipsecObj->localIPAddress, &wsize); prtloc +=wsize;
      sprintf(prtloc, "<td align='center'>%s</td>\n%n", ipsecObj->remoteIPAddress, &wsize); prtloc +=wsize;
      sprintf(prtloc, "</tr>\n%n", &wsize); prtloc +=wsize;

      cmsObj_free((void **) &ipsecObj);
    }
    *prtloc = 0;
#endif
}

#endif /* DMP_DEVICE2_IPSEC_1 */
