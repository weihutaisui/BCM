#ifdef SUPPORT_URLFILTER
/***********************************************************************
 *
 *  Copyright (c) 2006-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"

CmsRet dalUrlFilter_addEntry(const char* url_address, const UINT32 url_port)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UrlFilterListObject *urlListCfg = NULL;
   UrlFilterCfgObject *urlCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* adding a new Url filter list entry */
   cmsLog_debug("Adding new Url filter entry with %s/%u", url_address, url_port);

   if ((ret = cmsObj_get(MDMOID_URL_FILTER_CFG, &iidStack, 0, (void **) &urlCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get UrlFilterCfgObject, ret=%d", ret);
      return ret;
   } 

   if (urlCfg->enable == FALSE ||urlCfg->excludeMode == NULL)
   {
      cmsObj_free((void **) &urlCfg);
      return CMSRET_INVALID_ARGUMENTS;
   }

   cmsObj_free((void **) &urlCfg);
   
   /* add new instance of Url filter list */
   if ((ret = cmsObj_addInstance(MDMOID_URL_FILTER_LIST, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new UrlFilter list, ret=%d", ret);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_URL_FILTER_LIST, &iidStack, 0, (void **) &urlListCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get UrlFilterListObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_URL_FILTER_LIST, &iidStack);
      return ret;
   }

   urlListCfg->enable = TRUE;
   CMSMEM_REPLACE_STRING(urlListCfg->urlAddress, url_address);
   urlListCfg->portNumber = url_port;

   cmsLog_debug("in urlListCfg, urlAddress=%s, portNumber=%u", urlListCfg->urlAddress, urlListCfg->portNumber);
   
   /* set and activate UrlFilterListObject */
   ret = cmsObj_set(urlListCfg, &iidStack);
   cmsObj_free((void **) &urlListCfg);

   if (ret != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("Failed to set UrlFilterListObject, ret = %d", ret);
       
      r2 = cmsObj_deleteInstance(MDMOID_URL_FILTER_LIST, &iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created UrlFilterListObject, r2=%d", r2);
      }

      cmsLog_debug("Failed to set url filter list entry and successfully delete created UrlFilterListObject");
   }

   return ret;
}

CmsRet dalUrlFilter_removeEntry(const char* url_address)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UrlFilterListObject *urlListCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   /* deleting a Url filter list entry */
   cmsLog_debug("Deleting a url filter list entry with %s", url_address);

   while ((ret = cmsObj_getNext
         (MDMOID_URL_FILTER_LIST, &iidStack, (void **) &urlListCfg)) == CMSRET_SUCCESS)
   {
      if(!strcmp(url_address, urlListCfg->urlAddress)) 
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &urlListCfg);
      }
   }

   if(found == FALSE) 
   {
      cmsLog_debug("bad url address: %s   no entry found", url_address);
      return CMSRET_INVALID_ARGUMENTS;
   }
   else 
   {
      ret = cmsObj_deleteInstance(MDMOID_URL_FILTER_LIST, &iidStack);
      cmsObj_free((void **) &urlListCfg);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete UrlFilterListObject, ret = %d", ret);
         return ret;
      }
   }

   return ret;


}


CmsRet dalUrlFilter_setType(const char* type)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UrlFilterCfgObject *urlCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   cmsLog_debug("Activate/Modify url filter feature with mode: %s", type);

   if ((ret = cmsObj_get(MDMOID_URL_FILTER_CFG, &iidStack, 0, (void **) &urlCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get UrlFilterCfgObject, ret=%d", ret);
      return ret;
   } 
   
   urlCfg->enable = TRUE;
   CMSMEM_REPLACE_STRING(urlCfg->excludeMode, type);
   
   /* set and activate UrlFilterCfgObject */
   ret = cmsObj_set(urlCfg, &iidStack);
   cmsObj_free((void **) &urlCfg);

   return ret;

}


//CmsRet dalUrlFilter_getType(const char *type)
CmsRet dalUrlFilter_getType(char *type)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UrlFilterCfgObject *urlCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   if ((ret = cmsObj_get(MDMOID_URL_FILTER_CFG, &iidStack, 0, (void **) &urlCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get UrlFilterCfgObject, ret=%d", ret);
      return ret;
   } 

   if (urlCfg->excludeMode)
   {
      strcpy(type, urlCfg->excludeMode);
   }
   else
   {
      strcpy(type, "\0");
   }
   cmsObj_free((void **) &urlCfg);

   cmsLog_debug("Get url filter mode: %s", type);
   return ret;
}

#endif

