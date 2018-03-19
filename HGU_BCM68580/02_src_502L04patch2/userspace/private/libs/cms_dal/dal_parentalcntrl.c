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


/* local functions */
static CmsRet fillAccessTimeRestriction(const char *username, const char *mac, const unsigned char days, const unsigned short int starttime, const unsigned short int endtime, AccessTimeRestrictionObject *todCfg);

CmsRet dalAccessTimeRestriction_addEntry(const char *username, const char *mac, const unsigned char days, const unsigned short int starttime, const unsigned short int endtime)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   AccessTimeRestrictionObject *todCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* adding a new access time restriction entry */
   cmsLog_debug("Adding new access time restriction entry with %s/%s/%c/%u/%u", username, mac, days, starttime, endtime);

   /* add new instance */
   if ((ret = cmsObj_addInstance(MDMOID_ACCESS_TIME_RESTRICTION, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new Forwarding, ret=%d", ret);
      return ret;
   }

   /* get the instance of dslLinkConfig in the newly created WanConnectionDevice sub-tree */
   if ((ret = cmsObj_get(MDMOID_ACCESS_TIME_RESTRICTION, &iidStack, 0, (void **) &todCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get AccessTimeRestrictionObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_ACCESS_TIME_RESTRICTION, &iidStack);
      return ret;
   }       

   if ((ret = fillAccessTimeRestriction(username, mac, days, starttime, endtime, todCfg)) != CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &todCfg);
      cmsObj_deleteInstance(MDMOID_ACCESS_TIME_RESTRICTION, &iidStack);
      return ret;
   }

   cmsLog_debug("in todCfg, %s/%s/%s/%s/%s", todCfg->username, todCfg->MACAddress, todCfg->days, todCfg->startTime, todCfg->endTime);
   
   /* set and activate AccessTimeRestrictionObject */
   ret = cmsObj_set(todCfg, &iidStack);
   cmsObj_free((void **) &todCfg);

   if (ret != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("Failed to set AccessTimeRestrictionObject, ret = %d", ret);
       
      r2 = cmsObj_deleteInstance(MDMOID_ACCESS_TIME_RESTRICTION, &iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created AccessTimeRestrictionObject, r2=%d", r2);
      }

      cmsLog_debug("Failed to set access time restriction entry and successfully delete created AccessTimeRestrictionObject");

   }

   return ret;

}

/* Fill the given todCfg object with the input data */

CmsRet fillAccessTimeRestriction(const char *username, const char *mac, const unsigned char days, const unsigned short int starttime, const unsigned short int endtime, AccessTimeRestrictionObject *todCfg)
{
   CmsRet ret=CMSRET_SUCCESS;
   char start_time[BUFLEN_8], end_time[BUFLEN_8];
   char days_str[BUFLEN_32];

   days_str[0] = start_time[0] = end_time[0] = '\0';

   todCfg->enable = TRUE;

   cmsMem_free(todCfg->username);
   todCfg->username = cmsMem_strdup(username);

   cmsMem_free(todCfg->MACAddress);
   todCfg->MACAddress = cmsMem_strdup(mac);

   cmsMem_free(todCfg->days);

   if( days != 127 ) 
   {
      if( days & 1 )
      {
         strncat( days_str, "Mon,", sizeof( days_str ) );
      }
      if( days & 2 )
      {
         strncat( days_str, "Tue,", sizeof( days_str ) );
      }
      if( days & 4 )
      {
         strncat( days_str, "Wed,", sizeof( days_str ) );
      }
      if( days & 8 )
      {
         strncat( days_str, "Thu,", sizeof( days_str ) );
      }
      if( days & 16 )
      {
         strncat( days_str, "Fri,", sizeof( days_str ) );
      }
      if( days & 32 )
      {
         strncat( days_str, "Sat,", sizeof( days_str ) );
      }
      if( days & 64 )
      {
         strncat( days_str, "Sun ", sizeof( days_str ) );	 
      }
   }
   else
   {
      strncat( days_str, "Mon,Tue,Wed,Thu,Fri,Sat,Sun ", sizeof( days_str ) );	
   }

   days_str[strlen(days_str) - 1] = '\0';   
   todCfg->days= cmsMem_strdup(days_str);

   cmsMem_free(todCfg->startTime);
   snprintf(start_time, sizeof(start_time), "%d:%d", starttime / 60, starttime % 60);
   todCfg->startTime = cmsMem_strdup(start_time);

   cmsMem_free(todCfg->endTime);
   snprintf(end_time, sizeof(end_time), "%d:%d", endtime / 60, endtime % 60);   
   todCfg->endTime = cmsMem_strdup(end_time);

      
   if ((todCfg->username == NULL) ||(todCfg->MACAddress == NULL) ||
       (todCfg->days == NULL) || (todCfg->endTime == NULL) || (todCfg->startTime == NULL))
   {
      cmsLog_error("malloc failed.");
      cmsObj_free((void **) &todCfg);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   return ret;
}

CmsRet dalAccessTimeRestriction_deleteEntry(const char *username)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   AccessTimeRestrictionObject *todCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   /* deleting a static route entry */
   cmsLog_debug("Deleting a access time restriction entry with %s", username);

   while ((ret = cmsObj_getNext
         (MDMOID_ACCESS_TIME_RESTRICTION, &iidStack, (void **) &todCfg)) == CMSRET_SUCCESS)
   {
      if( !strcmp(username, todCfg->username) ) 
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &todCfg);
      }
   }

   if(found == FALSE) 
   {
      cmsLog_debug("bad username (%s), no access time restriction entry found", username);
      return CMSRET_INVALID_ARGUMENTS;
   }
   else 
   {
      ret = cmsObj_deleteInstance(MDMOID_ACCESS_TIME_RESTRICTION, &iidStack);
      cmsObj_free((void **) &todCfg);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete AccessTimeRestrictionObject, ret = %d", ret);
         return ret;
      }
   }

   return ret;

}



