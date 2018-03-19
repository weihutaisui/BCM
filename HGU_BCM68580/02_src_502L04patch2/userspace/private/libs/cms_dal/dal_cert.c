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

#ifdef SUPPORT_CERT

#include "cms.h"
#include "cms_util.h"
#include "cms_obj.h"
#include "cms_dal.h"

UINT32 dalCert_getNumberOfExistedCert(SINT32 type)
{
   UINT32 num = 0;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CertificateCfgObject *obj = NULL;

   while (cmsObj_getNext(MDMOID_CERTIFICATE_CFG, &iidStack, (void **) &obj) == CMSRET_SUCCESS)
   {
      switch (type)
      {
         case CERT_CA:
            if (cmsUtl_strcmp(obj->type, CERT_TYPE_CA) == 0)
            {
                num++;
            }
            break;
         case CERT_LOCAL:
            if (cmsUtl_strcmp(obj->type, CERT_TYPE_CA) != 0)
            {
                num++;
            }
            break;
      }
      /* free certCfg object */
      cmsObj_free((void **) &obj);
   }

   return (num);
}

UBOOL8 dalCert_findCert(char *name, SINT32 type)
{
   UBOOL8 ret = FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CertificateCfgObject *obj = NULL;

   while (ret == FALSE && 
              cmsObj_getNext(MDMOID_CERTIFICATE_CFG, &iidStack, (void **) &obj) == CMSRET_SUCCESS)
   {
      switch (type)
      {
         case CERT_CA:
            if (cmsUtl_strcmp(obj->type, CERT_TYPE_CA) == 0 &&
                 cmsUtl_strcmp(obj->name, name) == 0)
            {
                ret = TRUE;
            }
            break;
         case CERT_LOCAL:
            if (cmsUtl_strcmp(obj->type, CERT_TYPE_CA) != 0 &&
                 cmsUtl_strcmp(obj->name, name) == 0)
            {
                ret = TRUE;
            }
            break;
      }
      /* free certCfg object */
      cmsObj_free((void **) &obj);
   }

   return (ret);
}

/* copy the object with the input certiticate object */
CmsRet dalCert_copyCert(CertificateCfgObject *obj, CertificateCfgObject *certCfg)
{
   CmsRet ret = CMSRET_SUCCESS;

   CMSMEM_FREE_BUF_AND_NULL_PTR(obj->name);
   obj->name = cmsMem_strdup(certCfg->name);

   CMSMEM_FREE_BUF_AND_NULL_PTR(obj->subject);
   obj->subject = cmsMem_strdup(certCfg->subject);

   CMSMEM_FREE_BUF_AND_NULL_PTR(obj->type);
   obj->type = cmsMem_strdup(certCfg->type);

   CMSMEM_FREE_BUF_AND_NULL_PTR(obj->password);
   obj->password = cmsMem_strdup(certCfg->password);
      
   CMSMEM_FREE_BUF_AND_NULL_PTR(obj->content);
   obj->content = cmsMem_strdup(certCfg->content);

   CMSMEM_FREE_BUF_AND_NULL_PTR(obj->privKey);
   obj->privKey = cmsMem_strdup(certCfg->privKey);

   CMSMEM_FREE_BUF_AND_NULL_PTR(obj->reqPub);
   obj->reqPub = cmsMem_strdup(certCfg->reqPub);

   obj->refCount = certCfg->refCount;
 
   return (ret);
}

CmsRet dalCert_addCert(CertificateCfgObject *certCfg)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CertificateCfgObject *obj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Adding new certificate name=%s, subject=%s, type=%s, refCount=%d",
              certCfg->name, certCfg->subject, certCfg->type, certCfg->refCount);

   /* add new instance of certificate */
   if ((ret = cmsObj_addInstance(MDMOID_CERTIFICATE_CFG, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new certificate, ret=%d", ret);
      return (ret);
   }

   /* get the instance of object in the newly created 
      InternetGatewayDevice.X_BROADCOM_COM_CertificateCfg.{i}. sub-tree */
   if ((ret = cmsObj_get(MDMOID_CERTIFICATE_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get CertificateCfgObject, ret=%d", ret);
      return (ret);
   }       

   /* Fill the object with the input data */
   if ((ret = dalCert_copyCert(obj, certCfg)) != CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &obj);
      return (ret);
   }

   cmsLog_debug("Adding new certificate name=%s, subject=%s, type=%s, refCount=%d",
              obj->name, obj->subject, obj->type, obj->refCount);

   /* set X_BROADCOM_COM_CertificateCfg */
   ret = cmsObj_set(obj, &iidStack);
   /* free certCfg object */
   cmsObj_free((void **) &obj);

   if (ret != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("Failed to set X_BROADCOM_COM_CertificateCfg, ret = %d", ret);
       
      r2 = cmsObj_deleteInstance(MDMOID_CERTIFICATE_CFG, &iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created X_BROADCOM_COM_CertificateCfg, r2=%d", r2);
      }
      cmsLog_debug("Failed to set created X_BROADCOM_COM_CertificateCfg");
   }

   return (ret);
}

CmsRet dalCert_getCert(char *name, SINT32 type, CertificateCfgObject *certCfg)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CertificateCfgObject *obj = NULL;
   UBOOL8 found = FALSE;
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

   while (found == FALSE && 
              cmsObj_getNext(MDMOID_CERTIFICATE_CFG, &iidStack, (void **) &obj) == CMSRET_SUCCESS)
   {
      switch (type)
      {
         case CERT_CA:
            if (cmsUtl_strcmp(obj->type, CERT_TYPE_CA) == 0 &&
                 cmsUtl_strcmp(obj->name, name) == 0)
            {
               found = TRUE;
            }
            break;
         case CERT_LOCAL:
            if (cmsUtl_strcmp(obj->type, CERT_TYPE_CA) != 0 &&
                 cmsUtl_strcmp(obj->name, name) == 0)
            {
               found = TRUE;
            }
            break;
      }
      if (found == FALSE)
      {
         /* free certCfg object */
         cmsObj_free((void **) &obj);
      }
   }

   /* retrieve certCfg object */
   if (found == TRUE)
   {
      ret = dalCert_copyCert(certCfg, obj); 
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get X_BROADCOM_COM_CertificateCfg, ret=%d", ret);
      }
      /* free certCfg object */
      cmsObj_free((void **) &obj);
   }

   return (ret);
}

CmsRet dalCert_setCert(char *name, SINT32 type, CertificateCfgObject *certCfg)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CertificateCfgObject *obj = NULL;
   UBOOL8 found = FALSE;
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

   while (found == FALSE && 
              cmsObj_getNext(MDMOID_CERTIFICATE_CFG, &iidStack, (void **) &obj) == CMSRET_SUCCESS)
   {
      switch (type)
      {
         case CERT_CA:
           if (cmsUtl_strcmp(obj->type, CERT_TYPE_CA) == 0 &&
                cmsUtl_strcmp(obj->name, name) == 0)
           {
               found = TRUE;
           }
           break;
         case CERT_LOCAL:
           if (cmsUtl_strcmp(obj->type, CERT_TYPE_CA) != 0 &&
                cmsUtl_strcmp(obj->name, name) == 0)
           {
               found = TRUE;
           }
           break;
      }
      if (found == FALSE)
      {
         /* free certCfg object */
         cmsObj_free((void **) &obj);
      }
   }

   if (found == TRUE)
   {
      ret = dalCert_copyCert(obj, certCfg); 
      /* set X_BROADCOM_COM_CertificateCfg */
      ret = cmsObj_set(obj, &iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to set X_BROADCOM_COM_CertificateCfg, ret=%d", ret);
      }
      /* free certCfg object */
      cmsObj_free((void **) &obj);
   }

   return (ret);
}

CmsRet dalCert_delCert(char *name, SINT32 type)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CertificateCfgObject *obj = NULL;
   UBOOL8 found = FALSE;
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

   while (found == FALSE && 
              cmsObj_getNext(MDMOID_CERTIFICATE_CFG, &iidStack, (void **) &obj) == CMSRET_SUCCESS)
   {
      switch (type)
      {
         case CERT_CA:
            if (cmsUtl_strcmp(obj->type, CERT_TYPE_CA) == 0 &&
                 cmsUtl_strcmp(obj->name, name) == 0)
            {
               found = TRUE;
            }
            break;
         case CERT_LOCAL:
            if (cmsUtl_strcmp(obj->type, CERT_TYPE_CA) != 0 &&
                 cmsUtl_strcmp(obj->name, name) == 0)
            {
               found = TRUE;
            }
            break;
      }
      /* free certCfg object */
      cmsObj_free((void **) &obj);
   }

   if (found == TRUE)
   {
      ret = cmsObj_deleteInstance(MDMOID_CERTIFICATE_CFG, &iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created X_BROADCOM_COM_CertificateCfg, ret=%d", ret);
      }
   }

   return (ret);
}

CmsRet dalCert_resetRefCount(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CertificateCfgObject *obj = NULL;
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

   while (cmsObj_getNext(MDMOID_CERTIFICATE_CFG, &iidStack, (void **) &obj) == CMSRET_SUCCESS)
   {
      /* only reset reference count for signed or signed request certifcates but not CA certificates */
      if (cmsUtl_strcmp(obj->type, CERT_TYPE_CA) != 0)
      {
         /* reset reference count of X_BROADCOM_COM_CertificateCfg */
         obj->refCount = 0;
         /* set X_BROADCOM_COM_CertificateCfg */
         ret = cmsObj_set(obj, &iidStack);
      }
      /* free certCfg object */
      cmsObj_free((void **) &obj);
   }

   return (ret);
}

#endif    // SUPPORT_CERT

