
/***********************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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

#ifdef DMP_STORAGESERVICE_1


#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal.h"


#ifdef SUPPORT_SAMBA
extern void rut_doSystemAction(const char* from, char *cmd);

CmsRet dalStorage_addUserAccount(const WEB_NTWK_VAR *webVar, char**errorStr)
{
   StorageServiceObject *storageServiceObj=NULL;
   UserAccountObject *userAccountObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack childIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   char cmd[BUFLEN_256];

   const char *userName = &(webVar->storageuserName[0]);

   if(( ret =cmsObj_getNext(MDMOID_STORAGE_SERVICE, &iidStack, (void **) &storageServiceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get StorageService object, ret=%d", ret);
      return ret ;
   }

   /* check for unique username and also invalid usernames
    * (admin,user,support,nobody) */   
   while(( ret =cmsObj_getNextInSubTree(MDMOID_USER_ACCOUNT, &iidStack, &childIidStack, (void **) &userAccountObj)) == CMSRET_SUCCESS)
   {

      if(cmsUtl_strcmp(userName,userAccountObj->username) ==0)
      {
         *errorStr = "username already exists\n";
         cmsObj_free((void **) &storageServiceObj);
         cmsObj_free((void **) &userAccountObj);
         return CMSRET_INVALID_ARGUMENTS;
      }
      else if(!cmsUtl_strcmp(userName,"admin")|| !cmsUtl_strcmp(userName,"support")
            || !cmsUtl_strcmp(userName,"user")|| !cmsUtl_strcmp(userName,"nobody"))
      {
         *errorStr = "username cannot be admin,support,user or nobody\n";
         cmsObj_free((void **) &storageServiceObj);
         cmsObj_free((void **) &userAccountObj);
         return CMSRET_INVALID_ARGUMENTS;

      }
      cmsObj_free((void **) &userAccountObj);
   }

   /*username validated,add to MDM */  
   childIidStack = iidStack;

   if((ret=cmsObj_addInstance(MDMOID_USER_ACCOUNT, &childIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not add Storage UserAccount object, ret=%d", ret);
      cmsObj_free((void **) &storageServiceObj);
      return ret;
   }

   if((ret =cmsObj_get(MDMOID_USER_ACCOUNT, &childIidStack, OGF_NO_VALUE_UPDATE,(void **) &userAccountObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get UserAccount object, ret=%d", ret);
      cmsObj_free((void **) &storageServiceObj);
      return ret;
   } 

   CMSMEM_REPLACE_STRING(userAccountObj->username,webVar->storageuserName);
   CMSMEM_REPLACE_STRING(userAccountObj->password,webVar->storagePassword);
   CMSMEM_REPLACE_STRING(userAccountObj->X_BROADCOM_volumeName,webVar->storagevolumeName);

   userAccountObj->enable = TRUE;


   if(( ret =cmsObj_set(userAccountObj,&childIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not set UserAccount object, ret=%d", ret);
      cmsObj_free((void **) &userAccountObj);
      cmsObj_free((void **) &storageServiceObj);
      return ret;
   } 

   /* create a home directory for this user */
   sprintf(cmd,"mkdir /mnt/%s/%s",userAccountObj->X_BROADCOM_volumeName,userAccountObj->username);
   rut_doSystemAction("storage:useraccount", cmd);

   cmsObj_free((void **) &userAccountObj);

   storageServiceObj->userAccountNumberOfEntries++;

   if((ret =cmsObj_set(storageServiceObj,&iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not set StorageServic object, ret=%d", ret);
      cmsObj_free((void **) &storageServiceObj);
      return ret;
   } 

   cmsObj_free((void **) &storageServiceObj);

   return CMSRET_SUCCESS;
}

CmsRet dalStorage_deleteUserAccount(const char *userName)
{
   StorageServiceObject *storageServiceObj=NULL;
   UserAccountObject *userAccountObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack childIidStack = EMPTY_INSTANCE_ID_STACK;

   CmsRet ret;

   if(( ret =cmsObj_getNext(MDMOID_STORAGE_SERVICE, &iidStack, (void **) &storageServiceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get StorageService object, ret=%d", ret);
      return ret ;
   }

   while(( ret =cmsObj_getNextInSubTree(MDMOID_USER_ACCOUNT, &iidStack, &childIidStack, (void **) &userAccountObj)) == CMSRET_SUCCESS)
   {

      if(cmsUtl_strcmp(userName,userAccountObj->username) ==0)
      {
         if((ret = cmsObj_deleteInstance(MDMOID_USER_ACCOUNT, &childIidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not delete UserAccount Obj  ret=%d", ret);
            cmsObj_free((void **) &userAccountObj);
            cmsObj_free((void **) &storageServiceObj);
            return ret;

         }
         else
         {
            cmsObj_free((void **) &userAccountObj);
            storageServiceObj->userAccountNumberOfEntries--;
            if((ret =cmsObj_set(storageServiceObj,&iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Could not set StorageServic object, ret=%d", ret);
               cmsObj_free((void **) &storageServiceObj);
               return ret;
            } 

            cmsObj_free((void **) &storageServiceObj);
            return CMSRET_SUCCESS;
         } 

      }

      cmsObj_free((void **) &userAccountObj);
   }

   /*username not found */
   return CMSRET_INVALID_ARGUMENTS;

}

#endif /* SUPPORT_SAMBA */

#endif  /* DMP_STORAGESERVICE_1 */
