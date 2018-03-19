/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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


#ifdef DMP_DEVICE2_QOS_1

/*!\file cgi2_qos_policer.c
 * \brief This file contains TR181 QoS functions.  It also uses the
 *        data model independent code in cgi_qos_policer.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "cms.h"
#include "cms_qdm.h"
#include "cms_dal.h"
#include "cms_qos.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_sec.h"
#include "cgi_main.h"
#include "cgi_qos.h"


void cgiQosPolicerViewBody_dev2(FILE *fs)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2QosPolicerObject *pObj = NULL;
   CmsRet ret;

   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_POLICER, &iidStack, (void **)&pObj)) == CMSRET_SUCCESS)
   {
      fprintf(fs, "    <tr align='center'>\n");

      /* policer name */
      fprintf(fs, "      <td>%s</td>\n", pObj->X_BROADCOM_COM_PolicerName);

      /* policer key */
      fprintf(fs, "      <td>%d</td>\n", iidStack.instance[iidStack.currentDepth-1]);

      /* metering type */
      fprintf(fs, "      <td>%s</td>\n", pObj->meterType);

      /* committed rate */
      fprintf(fs, "      <td>%d</td>\n", pObj->committedRate/1000);

      /* committed burst size */
      fprintf(fs, "      <td>%d</td>\n", pObj->committedBurstSize);

      if (cmsUtl_strcmp(MDMVS_SINGLERATETHREECOLOR, pObj->meterType) == 0)
      {
         /* excess burst size */
         fprintf(fs, "      <td>%d</td>\n", pObj->excessBurstSize);
      }
      else
      {
         fprintf(fs, "      <td>-</td>\n");
      }

      if (cmsUtl_strcmp(MDMVS_TWORATETHREECOLOR, pObj->meterType) == 0)
      {
         /* peak rate */
         fprintf(fs, "      <td>%d</td>\n", pObj->peakRate/1000);

         /* peak burst size */
         fprintf(fs, "      <td>%d</td>\n", pObj->peakBurstSize);
      }
      else
      {
         fprintf(fs, "      <td>-</td>\n");
         fprintf(fs, "      <td>-</td>\n");
      }

      /* conforming action */
      if (cmsUtl_strcmp(MDMVS_NULL, pObj->conformingAction) == 0 ||
          cmsUtl_strcmp(MDMVS_DROP, pObj->conformingAction) == 0)
      {
         fprintf(fs, "      <td>%s</td>\n", pObj->conformingAction);
      }
      else
      {
         fprintf(fs, "      <td>%s</td>\n", cgiQosDscpMarkToName(atoi(pObj->conformingAction)));
      }

      if (cmsUtl_strcmp(MDMVS_SIMPLETOKENBUCKET, pObj->meterType) != 0)
      {
         /* partial conforming action */
         if (cmsUtl_strcmp(MDMVS_NULL, pObj->partialConformingAction) == 0 ||
             cmsUtl_strcmp(MDMVS_DROP, pObj->partialConformingAction) == 0)
         {
            fprintf(fs, "      <td>%s</td>\n", pObj->partialConformingAction);
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", cgiQosDscpMarkToName(atoi(pObj->partialConformingAction)));
         }
      }
      else
      {
         fprintf(fs, "      <td>-</td>\n");
      }

      /* non-conforming action */
      if (cmsUtl_strcmp(MDMVS_NULL, pObj->nonConformingAction) == 0 ||
          cmsUtl_strcmp(MDMVS_DROP, pObj->nonConformingAction) == 0)
      {
         fprintf(fs, "      <td>%s</td>\n", pObj->nonConformingAction);
      }
      else
      {
         fprintf(fs, "      <td>%s</td>\n", cgiQosDscpMarkToName(atoi(pObj->nonConformingAction)));
      }

      /* policer enable */
      fprintf(fs, "      <td align='center'><input type='checkbox' name='enbl' %s></td>\n", pObj->enable? "checked" : "");
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml'></td>\n");

      fprintf(fs, "  </tr>\n");
      fflush(fs);
      cmsObj_free((void **)&pObj);
      glbEntryCt++;
   }
}


void cgiQosPolicerRemove_dev2(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   char *check;
   UBOOL8 isRefered;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackPrev=EMPTY_INSTANCE_ID_STACK;
   Dev2QosPolicerObject *pObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName(query, "rmLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the policer table */
   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_POLICER, &iidStack, (void **)&pObj)) == CMSRET_SUCCESS)
   {
      if (last == NULL)
      {
         check = strtok_r(lst, ",", &last);
      }
      else
      {
         check = strtok_r(NULL, ",", &last);
      }
      if (check == NULL)
      {
         cmsObj_free((void **)&pObj);
         break;   /* done */
      }
      if (atoi(check) == 1)
      {
         /* check to see if the policer is being refered by a class rule */
         if ((ret = qdmQos_referenceCheckLocked(CMS_QOS_REF_TARGET_POLICER,
                                          iidStack.instance[iidStack.currentDepth - 1],
                                          &isRefered)) != CMSRET_SUCCESS)
         {
            cmsLog_error("qdmQos_referenceCheck returns error. ret=%d", ret);
            sprintf(cmd, "Reference check failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
            cmsObj_free((void **)&pObj);
            break;
         }
         if (isRefered)
         {
            sprintf(cmd, "Policer can not be deleted. It is being refered by a Class rule.");
            cmsLog_error(cmd);
            cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
            cmsObj_free((void **)&pObj);
            break;
         }

         cmsLog_debug("Deleting policer instance %d", PEEK_INSTANCE_ID(&iidStack));
         if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_QOS_POLICER, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Delete policer returns error, ret = %d", ret);
            sprintf(cmd, "Delete policer failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
            cmsObj_free((void **)&pObj);
            break;
         }
         glbSaveConfigNeeded = TRUE;

         /* since this instance has been deleted, we want to set the iidStack to
          * the previous instance, so that we can continue to do getNext.
          */
         iidStack = iidStackPrev;
      }
      /* save this iidStack in case we want to do a getNext from this instance */
      iidStackPrev = iidStack;
      
      cmsObj_free((void **)&pObj);
   }
}


void cgiQosPolicerEnable_dev2(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   char *check;
   UBOOL8 isRefered;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2QosPolicerObject *pObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName(query, "eblLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the policer table */
   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_POLICER, &iidStack, (void **)&pObj)) == CMSRET_SUCCESS)
   {
      if (last == NULL)
      {
         check = strtok_r(lst, ",", &last);
      }
      else
      {
         check = strtok_r(NULL, ",", &last);
      }
   
      if (check == NULL)
      {
         cmsObj_free((void **)&pObj);
         break;   /* done */
      }
   
      if (atoi(check) == 1)
      {
         if (!pObj->enable)
         {
            /* enable policer */
            pObj->enable = TRUE;
            if ((ret = cmsObj_set(pObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_set returns error. ret=%d", ret);
               sprintf(cmd, "Enable policer failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
               cmsObj_free((void **)&pObj);
               break;
            }
            else
            {
               glbSaveConfigNeeded = TRUE;
            }
         }
      }
      else
      {
         if (pObj->enable)
         {
            /* disable policer */
            /* check to see if the policer is being refered by a class rule */
            if ((ret = qdmQos_referenceCheckLocked(CMS_QOS_REF_TARGET_POLICER,
                                             iidStack.instance[iidStack.currentDepth - 1],
                                             &isRefered)) != CMSRET_SUCCESS)
            {
               cmsLog_error("qdmQos_referenceCheck returns error. ret=%d", ret);
               sprintf(cmd, "Reference check failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
               cmsObj_free((void **)&pObj);
               break;
            }
            if (isRefered)
            {
               sprintf(cmd, "Policer can not be disabled. It is being refered by a Class rule.");
               cmsLog_error(cmd);
               cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
               cmsObj_free((void **)&pObj);
               break;
            }

            pObj->enable = FALSE;
            if ((ret = cmsObj_set(pObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_set returns error. ret=%d", ret);
               sprintf(cmd, "Disable policer failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
               cmsObj_free((void **)&pObj);
               break;
            }
            else
            {
               glbSaveConfigNeeded = TRUE;
            }
         }
      }
      cmsObj_free((void **)&pObj);
   }
}


void cgiGetQosPolicerInfo_dev2(int argc __attribute__((unused)),
                          char **argv __attribute__((unused)),
                          char *varValue)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2QosPolicerObject *pObj = NULL;
   char varValue1[BUFLEN_512];
   char meterType[BUFLEN_4];

   varValue[0]  = '\0';

   /* loop through the policer table */
   while (cmsObj_getNext(MDMOID_DEV2_QOS_POLICER, &iidStack,
                         (void **) &pObj) == CMSRET_SUCCESS)
   {
      if (pObj->enable)
      {
         if (cmsUtl_strcmp(MDMVS_SIMPLETOKENBUCKET, pObj->meterType) == 0)
         {
            strcpy(meterType, "tb");
         }
         else if (cmsUtl_strcmp(MDMVS_SINGLERATETHREECOLOR, pObj->meterType) == 0)
         {
            strcpy(meterType, "1r");
         }
         else
         {
            strcpy(meterType, "2r");
         }

         varValue1[0] = '\0';
         sprintf(varValue1, "%d:Key%d&%s,cir%dcbs%debs%dpir%dpbs%d,%s-%s-%s|",
                 iidStack.instance[iidStack.currentDepth-1],
                 iidStack.instance[iidStack.currentDepth-1],
                 meterType,
                 pObj->committedRate/1000,
                 pObj->committedBurstSize,
                 pObj->excessBurstSize,
                 pObj->peakRate/1000,
                 pObj->peakBurstSize,
                 (cmsUtl_strcmp(MDMVS_NULL, pObj->conformingAction) == 0 ||
                  cmsUtl_strcmp(MDMVS_DROP, pObj->conformingAction) == 0)? 
                     pObj->conformingAction :
                     cgiQosDscpMarkToName(atoi(pObj->conformingAction)),
                 (cmsUtl_strcmp(MDMVS_NULL, pObj->partialConformingAction) == 0 ||
                  cmsUtl_strcmp(MDMVS_DROP, pObj->partialConformingAction) == 0)? 
                     pObj->partialConformingAction :
                     cgiQosDscpMarkToName(atoi(pObj->partialConformingAction)),
                 (cmsUtl_strcmp(MDMVS_NULL, pObj->nonConformingAction) == 0 ||
                  cmsUtl_strcmp(MDMVS_DROP, pObj->nonConformingAction) == 0)? 
                     pObj->nonConformingAction :
                     cgiQosDscpMarkToName(atoi(pObj->nonConformingAction)));
         strcat(varValue, varValue1);
      }

      cmsObj_free((void **) &pObj);
   }
}

#endif  /* DMP_DEVICE2_QOS_1 */


