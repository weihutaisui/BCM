/***********************************************************************
 *
 *  Copyright (c) 2007-2010  Broadcom Corporation
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


/* local functions */
static CmsRet fillDDnsCfg(const char *fullyQualifiedDomainName, const char *userName, const char *password, const char *interface, unsigned short int providerName, DDnsCfgObject *ddnsCfg);

CmsRet dalDDns_addEntry(const char *fullyQualifiedDomainName, const char *userName, const char *password, const char *interface, unsigned short int providerName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
   DDnsCfgObject *ddnsCfg = NULL;
   DDnsCfgObject *ddnsCfg1 = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* adding a new dynamic dns entry */
   cmsLog_debug("Adding new dynamic dns entry with %s/%s/%s/%s/%u", fullyQualifiedDomainName, userName, password, interface, providerName);

   while((ret=cmsObj_getNext( MDMOID_D_DNS_CFG, &iidStack1, (void **) &ddnsCfg1))==CMSRET_SUCCESS){
	  if (!cmsUtl_strcmp(fullyQualifiedDomainName, ddnsCfg1->fullyQualifiedDomainName)){
	  ret=CMSRET_INVALID_ARGUMENTS;
	  cmsObj_free((void **) &ddnsCfg1);
	  return ret;
	  }	 
         cmsObj_free((void **) &ddnsCfg1);	
   }
   
   /* add new instance */
   if ((ret = cmsObj_addInstance(MDMOID_D_DNS_CFG, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new Forwarding, ret=%d", ret);
      return ret;
   }

   /* get the instance of dslLinkConfig in the newly created WanConnectionDevice sub-tree */
   if ((ret = cmsObj_get(MDMOID_D_DNS_CFG, &iidStack, 0, (void **) &ddnsCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get DDnsCfgObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_D_DNS_CFG, &iidStack);
      return ret;
   }       
   if ((ret = fillDDnsCfg(fullyQualifiedDomainName, userName, password, interface, providerName, ddnsCfg)) != CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &ddnsCfg);
      cmsObj_deleteInstance(MDMOID_D_DNS_CFG, &iidStack);
      return ret;
   }
   cmsLog_debug("in ddnsCfg, %s/%s/%s/%s/%s", ddnsCfg->fullyQualifiedDomainName, ddnsCfg->userName, ddnsCfg->password, ddnsCfg->ifName, ddnsCfg->providerName);


   /* set and activate DDnsCfgObject */
   ret = cmsObj_set(ddnsCfg, &iidStack);
   cmsObj_free((void **) &ddnsCfg);

   if (ret != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("Failed to set DDnsCfgObject, ret = %d", ret);
       
      r2 = cmsObj_deleteInstance(MDMOID_D_DNS_CFG, &iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created DDnsCfgObject, r2=%d", r2);
      }

      cmsLog_debug("Failed to set dynamic dns entry and successfully delete created DDnsCfgObject");

   }

   return ret;

}

/* Fill the given ddnsCfg object with the input data */

CmsRet fillDDnsCfg(const char *fullyQualifiedDomainName, const char *userName, const char *password, const char *interface, unsigned short int providerName, DDnsCfgObject *ddnsCfg)
{
   char providerName_str[BUFLEN_32] = {0};
   CmsRet ret=CMSRET_SUCCESS;

   ddnsCfg->enable = TRUE;
   CMSMEM_REPLACE_STRING(ddnsCfg->fullyQualifiedDomainName, fullyQualifiedDomainName);
   CMSMEM_REPLACE_STRING(ddnsCfg->userName, userName);
   CMSMEM_REPLACE_STRING(ddnsCfg->password, password);
   CMSMEM_REPLACE_STRING(ddnsCfg->ifName, interface);

   switch ( providerName )
   {
   case 0:
      snprintf(providerName_str, sizeof(providerName_str), "%s", MDMVS_TZO);   
      break;
   case 1:
      snprintf(providerName_str, sizeof(providerName_str), "%s", MDMVS_DYNDNS);   
      break;
   case 2:
      snprintf(providerName_str, sizeof(providerName_str), "%s", MDMVS_NOIP);   
      break;
   }
   CMSMEM_REPLACE_STRING(ddnsCfg->providerName, providerName_str);

   if ((ddnsCfg->fullyQualifiedDomainName == NULL) ||(ddnsCfg->userName == NULL) ||
       (ddnsCfg->password == NULL) || (ddnsCfg->ifName == NULL) || (ddnsCfg->providerName == NULL))
   {
      cmsLog_error("malloc failed.");
      ret = CMSRET_RESOURCE_EXCEEDED;
   }
   return ret;
}

CmsRet dalDDns_deleteEntry(const char *fullyQualifiedDomainName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   DDnsCfgObject *ddnsCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   /* deleting a dynamic dns entry */
   cmsLog_debug("Deleting a dynamic DNS entry with %s", fullyQualifiedDomainName);

   while (!found &&
          (ret = cmsObj_getNext(MDMOID_D_DNS_CFG, &iidStack, (void **) &ddnsCfg)) == CMSRET_SUCCESS)
   {
      found = (0 == strcmp(fullyQualifiedDomainName, ddnsCfg->fullyQualifiedDomainName)); 
      cmsObj_free((void **) &ddnsCfg);
   }

   if(found == FALSE) 
   {
      cmsLog_debug("bad hostname (%s), no dynamic DNS entry found", fullyQualifiedDomainName);
      return CMSRET_INVALID_ARGUMENTS;
   }
   else 
   {
      ret = cmsObj_deleteInstance(MDMOID_D_DNS_CFG, &iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete DDNSCfgObject, ret = %d", ret);
         return ret;
      }
   }

   return ret;

}

 
