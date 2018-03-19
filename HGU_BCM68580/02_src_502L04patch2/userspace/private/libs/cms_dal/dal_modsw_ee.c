/***********************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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



#ifdef DMP_DEVICE2_SM_BASELINE_1


#include "cms_util.h"
#include "cms_dal.h"
#include "dal.h"


CmsRet dalModSw_getAvailableEE(NameList **ifList)
{
   CmsRet            ret;
   ExecEnvObject *ExecEnv=NULL;
   InstanceIdStack   iidStack=EMPTY_INSTANCE_ID_STACK;
   NameList          head;
   NameList          *tail, *nameListElement;

   *ifList = NULL;
   head.name = NULL;
   head.next = NULL;
   tail = &head;

   while ((ret = cmsObj_getNext(MDMOID_EXEC_ENV, &iidStack, (void **)&ExecEnv)) == CMSRET_SUCCESS)
   {
      if(ExecEnv->alias == NULL)
      {
         cmsLog_error("EE alias name is NULL!");
         cmsObj_free((void **)&ExecEnv);
         continue;
      }

      /* This function returns the list of Execution Environment names 
       * that are capable for installing Deployment Unit.
       * It should check Execution Environment enable and status.
       */
      if((ExecEnv->enable == FALSE) || (cmsUtl_strcmp(ExecEnv->status, MDMVS_UP) != 0))
      {
         cmsLog_notice("EE is Disable/Down!");
         cmsObj_free((void **)&ExecEnv);
         continue;
      }

      nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
      if (nameListElement == NULL)
      {
         cmsLog_error("could not allocate NameList");
         cmsDal_freeNameList(head.next);
         return CMSRET_RESOURCE_EXCEEDED;
      }

      /* append to name list */
      cmsLog_debug("adding %s to list of available EE Names", ExecEnv->alias);
      nameListElement->name = cmsMem_strdup(ExecEnv->alias);
      nameListElement->next = NULL;
      tail->next = nameListElement;
      tail = nameListElement;

   }


   /* return the name list */
   *ifList = head.next;

   return CMSRET_SUCCESS;

}


#endif /* DMP_DEVICE2_SM_BASELINE_1 */
