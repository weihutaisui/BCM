/* 
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :>
*/

#ifdef DMP_DEVICE2_BASELINE_1

#include "cms.h"
#include "cms_util.h"
#include "cms_qdm.h"
#include "cms_core.h"
#include "ssk.h"
#ifdef DMP_DEVICE2_DSL_1
#include "devctl_xtm.h"
#endif

/*!\file ssk2_intfstack.c
 *
 * This file contains functions which manage the TR181 interface stack.
 * This code is needed in Hybrid TR98+TR181 mode and PURE181 mode.
 */



/* local functions */
static UBOOL8 findIntfStackEntryLocked(const char *higherLayer,
                                       const char *lowerLayer,
                                       Dev2InterfaceStackObject **obj,
                                       InstanceIdStack *iidStack);
static CmsRet addIntfStackEntryLocked(const char *higherLayer, const char *lowerLayer);
static void deleteFullPathFromLowerLayersParam(const char *deletedFullPath,
                                               const char *targetFullPath);
static CmsRet getAliasFromFullPathLocked(const char *fullPath, char *alias);
static CmsRet getAliasFromPathDescLocked(const MdmPathDescriptor *pathDesc, char *alias);

static UBOOL8 isBridgeMgmtPort(const char *higherLayerFullPath);
static UBOOL8 isAnyLowerLayerIntfUpLocked(const char *higherLayerFullPath,
                                       const char *excludeLowerLayerFullPath);
static void writeIntfStackToConfigLocked(void);

#if defined(DMP_BASELINE_1) && defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)
static UBOOL8 isTr98WanIpPppObj(const char *fullpath);
static CmsRet getTr98L2StatusFromL3FullPath(const char *fullPath,
                                     char *statusBuf, UINT32 statusBufLen);
#endif



void processIntfStackLowerLayersChangedMsg(const CmsMsgHeader *msg)
{
   IntfStackLowerLayersChangedMsgBody *llChangedMsg = (IntfStackLowerLayersChangedMsgBody *) (msg+1);
   char *deltaBuf = llChangedMsg->deltaLowerLayers;
   UINT32 end;
   UINT32 idx=0;
   UBOOL8 addOp=FALSE;
   char lowerLayerFullPathBuf[MDM_SINGLE_FULLPATH_BUFLEN+1];
   UINT32 j;
   MdmPathDescriptor pathDesc;
   char *higherLayerFullPath=NULL;
   CmsRet ret;
   char statusBuf[BUFLEN_64] = {0};

   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = llChangedMsg->oid;
   pathDesc.iidStack = llChangedMsg->iidStack;
   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &higherLayerFullPath)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      return;
   }

   end=strlen(llChangedMsg->deltaLowerLayers);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      CMSMEM_FREE_BUF_AND_NULL_PTR(higherLayerFullPath);
      return;
   }

   while (idx < end)
   {
      if (deltaBuf[idx] == '+')
      {
         addOp = TRUE;
      }
      else if (deltaBuf[idx] == '-')
      {
         addOp = FALSE;
      }
      else
      {
         cmsLog_error("unrecogized op char %c at idx %d, stop parsing this msg!", deltaBuf[idx], idx);
         break;
      }
      idx++;
      memset(lowerLayerFullPathBuf, 0, sizeof(lowerLayerFullPathBuf));
      j=0;
      while (deltaBuf[idx] != ',' && idx < end)
      {
         lowerLayerFullPathBuf[j++] = deltaBuf[idx++];
      }
      if (addOp)
      {
         if ((ret = addIntfStackEntryLocked(higherLayerFullPath, lowerLayerFullPathBuf)) != CMSRET_SUCCESS)
         {
            cmsLog_error("addIntfStackEntry failed, ret=%d", ret);
            /* just complain, don't exit the function */
         }
         else
         {
            CmsRet r2;
            /* after successful add of new intf stack entry, we need
             * to propagate up the status from the lower layer.
             */
#if defined(DMP_BASELINE_1) && defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)
            if (isTr98WanIpPppObj(lowerLayerFullPathBuf))
            {
               /* In Hybrid IPv6 case, we will have a lowerLayer pointing to
                * WanIpConn and WanPppConn objects, but what we really
                * want is the status of the underlying Layer 2 link.  So
                * we cannot use qdmIntf_getStatusFromFullPath.  Instead,
                * call special function to get the layer 2 status.
                * XXX fix Hybrid code to correctly point to the layer 2 intf?
                */
               r2 = getTr98L2StatusFromL3FullPath(lowerLayerFullPathBuf,
                                       statusBuf, sizeof(statusBuf));
            }
            else
#endif
            {
               r2 = qdmIntf_getStatusFromFullPathLocked_dev2(lowerLayerFullPathBuf,
                                             statusBuf, sizeof(statusBuf));
            }

            if (r2 != CMSRET_SUCCESS)
            {
               cmsLog_error("getStatusFromFullPath failed for %s, ret=%d",
                            lowerLayerFullPathBuf, r2);
               /* complain but don't exit */
            }
            else
            {
               intfStack_propagateStatusByFullPathLocked(lowerLayerFullPathBuf, statusBuf);
            }
         }
      }
      else
      {
         /*
          * addOp == FALSE, so a fullpath has been removed from the LowerLayers
          * param of the higher layer obj.  Note this is NOT the same as a
          * delete of an object.
          * Propagate a "fake" status of LOWERLAYERDOWN to the higher layer
          * obj.  The status is "fake" because it is not the real status of
          * the lower layer obj.  We only do this because the higher layer
          * obj can no longer derive its status from this lower layer obj.
          * Do not propagate this fake status to any other higher layer objects.
          */
         Dev2InterfaceStackObject *intfStackObj=NULL;
         InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

         cmsLog_debug("delete higher=%s lower=%s",
                       higherLayerFullPath, lowerLayerFullPathBuf);

         /*
          * we might get this message after a whole object delete has been
          * processed.  In this case, propagate status is already done and
          * intfStack entry is already deleted.  So if we don't find an
          * intfStack entry, no big deal.
          */
         if (findIntfStackEntryLocked(higherLayerFullPath, lowerLayerFullPathBuf,
                                      &intfStackObj, &iidStack))
         {
#ifdef DMP_DEVICE2_BONDEDDSL_1
            /* for bonding group, its status doesn't just depend on the
             * lowerLayers's (DSL channel) status of UP/DOWN, it also depends on what 
             * the CO and CPE xDSL dynamically trained mode is.  So, the removal of 
             * channel as lower layer does not necessarily mean the bonding group
             * higher up should always change LOWER_LAYER_DOWN too.    
             * So, propagate the real channel's status up instead; if the channel is UP, 
             * the bonding group should not be brought down.
             */
            if (strstr(deltaBuf,"BondingGroup") != NULL) 
            {
               /* get the real status from the channel */
               ret = qdmIntf_getStatusFromFullPathLocked_dev2(lowerLayerFullPathBuf,
                                                              statusBuf, sizeof(statusBuf));
               if (ret != CMSRET_SUCCESS)
               {
                  intfStack_propagateStatusByFullPathLocked(lowerLayerFullPathBuf,
                                                            MDMVS_LOWERLAYERDOWN);
               }
               else
               {
                  intfStack_propagateStatusByFullPathLocked(lowerLayerFullPathBuf,
                                                            statusBuf);
               }
            }
            else
#endif  /* DMP_DEVICE2_BONDEDDSL_1 */          
            {
               intfStack_propagateStatusOnSingleEntryLocked(intfStackObj,
                                                         MDMVS_LOWERLAYERDOWN);
            }

            cmsObj_free((void **) &intfStackObj);

            /*
             * Now that status propagation is done, delete the intfStack entry
             */
            ret = cmsObj_deleteInstance(MDMOID_DEV2_INTERFACE_STACK, &iidStack);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("delete instance %s failed, ret=%d",
                            cmsMdm_dumpIidStack(&iidStack), ret);
            }
         }
      }

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("add/del op failed, ret=%d", ret);
      }
      idx++;
   }

   writeIntfStackToConfigLocked();

   cmsLck_releaseLock();

   CMSMEM_FREE_BUF_AND_NULL_PTR(higherLayerFullPath);

   return;
}


void processIntfStackObjectDeletedMsg(const CmsMsgHeader *msg)
{
   IntfStackObjectDeletedMsgBody *objDeletedMsg = (IntfStackObjectDeletedMsgBody *) (msg+1);
   MdmPathDescriptor pathDesc;
   char *deletedFullPath=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2InterfaceStackObject *intfStackObj=NULL;
   UINT32 lowerEntriesDeleted=0;
   UINT32 higherEntriesDeleted=0;
   CmsRet ret;

   cmsLog_debug("oid=%d iidStack=%s",
          objDeletedMsg->oid, cmsMdm_dumpIidStack(&objDeletedMsg->iidStack));

   /* form the fullpath of the deleted object */
   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = objDeletedMsg->oid;
   pathDesc.iidStack = objDeletedMsg->iidStack;

   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &deletedFullPath)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      return;
   }


   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      CMSMEM_FREE_BUF_AND_NULL_PTR(deletedFullPath);
      return;
   }

   /*
    * First, find all entries where lowerLayer == deletedFullPath and
    * update the lowerLayers param on the upper object.
    */
   while (findIntfStackEntryLocked(NULL, deletedFullPath, &intfStackObj, &iidStack))
   {
      /* remove the deletedFullPath from the LowerLayers param on the
       * higher object.  This will(may?) generate LowerLayersChanged messages
       * but ssk will have to ignore these.
       */
      deleteFullPathFromLowerLayersParam(deletedFullPath, intfStackObj->higherLayer);
      cmsObj_free((void **) &intfStackObj);

      /*
       * What status should this object have if the LowerLayers param is NULL?
       * Keep things simple and use LowerLayerDown in this case.
       * Propagate LowerLayerDown status to the higher layer object.  Must do
       * this before deleting the intf stack entry because propagateStatus
       * matches against the lowerLayer fullpath.
       */
      cmsLog_debug("propagate LowerLayerDown from %s", deletedFullPath);
      intfStack_propagateStatusByFullPathLocked(deletedFullPath, MDMVS_LOWERLAYERDOWN);

      ret = cmsObj_deleteInstance(MDMOID_DEV2_INTERFACE_STACK, &iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("delete instance %s failed, ret=%d",
                      cmsMdm_dumpIidStack(&iidStack), ret);
      }
      else
      {
         lowerEntriesDeleted++;
      }
   }

   /*
    * Final cleanup: delete all entries where higherLayer == deletedFullPath.
    * Don't have to worry about propagate status in this step.
    */
   while (findIntfStackEntryLocked(deletedFullPath, NULL, NULL, &iidStack))
   {
      ret = cmsObj_deleteInstance(MDMOID_DEV2_INTERFACE_STACK, &iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("delete instance %s failed, ret=%d",
                      cmsMdm_dumpIidStack(&iidStack), ret);
      }
      else
      {
         higherEntriesDeleted++;
      }
   }

   cmsLog_debug("LowerDeleted=%d higherDeleted=%d",
                lowerEntriesDeleted, higherEntriesDeleted);

   if (lowerEntriesDeleted || higherEntriesDeleted)
   {
      writeIntfStackToConfigLocked();
   }

   cmsLck_releaseLock();

   CMSMEM_FREE_BUF_AND_NULL_PTR(deletedFullPath);

   return;
}


/** Find an interface stack entry.
 *
 * @param (IN) Higher Layer fullpath to match, or NULL for wildcard
 * @param (IN) Lower Layer fullpath to match, or NULL for wildcard
 * @param (OUT) if ptr is not NULL, will point to found IntfStackObj.
 *              Caller is responsible for freeing it.
 * @param (OUT) if ptr is not NULL, will contain iidStack of found entry
 *
 * @return TRUE if an entry is found
 */
static UBOOL8 findIntfStackEntryLocked(const char *higherLayer,
                                       const char *lowerLayer,
                                       Dev2InterfaceStackObject **obj,
                                       InstanceIdStack *iidStack)
{
   Dev2InterfaceStackObject *intfStackObj=NULL;
   InstanceIdStack localIidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret;

   while (!found &&
         (ret = cmsObj_getNext(MDMOID_DEV2_INTERFACE_STACK,
                               &localIidStack,
                               (void **)&intfStackObj)) == CMSRET_SUCCESS)
   {

      if (((higherLayer == NULL) ||
           (!cmsUtl_strcmp(intfStackObj->higherLayer, higherLayer))) &&
          ((lowerLayer == NULL) ||
           (!cmsUtl_strcmp(intfStackObj->lowerLayer, lowerLayer))))
      {
         found = TRUE;
         if (iidStack)
         {
            *iidStack = localIidStack;
         }
         if (obj != NULL)
         {
            *obj = intfStackObj;
            /* return immediately so we do not free obj in this function.
             * caller has it now and he is responsible for freeing.
             */
            return found;
         }
      }
      cmsObj_free((void **) &intfStackObj);
   }

   return found;
}


CmsRet addIntfStackEntryLocked(const char *higherLayer, const char *lowerLayer)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2InterfaceStackObject *intfStackObj=NULL;
   CmsRet ret;

   cmsLog_debug("higher=%s lower=%s", higherLayer, lowerLayer);

   if (findIntfStackEntryLocked(higherLayer, lowerLayer, NULL, NULL))
   {
      /*
       * The interface stack entries are always saved to the config file.
       * So when MDM initializes during bootup, we will get LowerLayersChanged
       * messages, but the entries are already in the MDM.
       */
      cmsLog_debug("ignore dup intfStack entry, higher=%s lower=%s",
                   higherLayer, lowerLayer);
      return CMSRET_SUCCESS;
   }


   ret = cmsObj_addInstance(MDMOID_DEV2_INTERFACE_STACK, &iidStack);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("addInstance failed, ret=%d", ret);
      return ret;
   }

   ret = cmsObj_get(MDMOID_DEV2_INTERFACE_STACK, &iidStack, 0, (void **)&intfStackObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get new IntfStack instance, ret=%d", ret);
      return ret;
   }

   CMSMEM_REPLACE_STRING(intfStackObj->higherLayer, higherLayer);
   CMSMEM_REPLACE_STRING(intfStackObj->lowerLayer, lowerLayer);

   /* also fill in the aliases when this entry is created */
   {
      char aliasBuf[MDM_ALIAS_BUFLEN]={0};
      memset(aliasBuf, 0, sizeof(aliasBuf));
      if ((CMSRET_SUCCESS == getAliasFromFullPathLocked(higherLayer, aliasBuf)) &&
            (cmsUtl_strlen(aliasBuf) > 0))
      {
         CMSMEM_REPLACE_STRING(intfStackObj->higherAlias, aliasBuf);
      }
      memset(aliasBuf, 0, sizeof(aliasBuf));
      if ((CMSRET_SUCCESS == getAliasFromFullPathLocked(lowerLayer, aliasBuf)) &&
            (cmsUtl_strlen(aliasBuf) > 0))
      {
         CMSMEM_REPLACE_STRING(intfStackObj->lowerAlias, aliasBuf);
      }
   }


   ret = cmsObj_set((void *) intfStackObj, &iidStack);
   cmsObj_free((void **) &intfStackObj);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of IntfStackObj failed, ret=%d", ret);
   }

   return ret;
}


void deleteFullPathFromLowerLayersParam(const char *deletedFullPath,
                                        const char *targetFullPath)
{
   MdmPathDescriptor pathDesc;
   PhlSetParamValue_t paramValue;
   char lowerLayersBuf[MDM_MULTI_FULLPATH_BUFLEN]={0};
   CmsRet ret;

   cmsLog_debug("delete %s from %s", deletedFullPath, targetFullPath);

   ret = qdmIntf_getLowerLayersFromFullPathLocked_dev2(targetFullPath,
                                 lowerLayersBuf, sizeof(lowerLayersBuf));
   if (ret != CMSRET_SUCCESS)
   {
      /*
       * If delete from WebUI, by the time we get here, the higher layer
       * object may be already deleted, so there is nothing to update.
       */
      cmsLog_debug("could not get LowerLayers param for %s, ret=%d",
                   targetFullPath, ret);
      return;
   }

   /* remove the deletedFullPath from lowerLayers param buf */
   cmsUtl_deleteFullPathFromCSL(deletedFullPath, lowerLayersBuf);

   /* set the updated LowerLayers param value */
   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(targetFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                   targetFullPath, ret);
      return;
   }

   memset(&paramValue, 0, sizeof(paramValue));
   paramValue.pathDesc.oid = pathDesc.oid;
   paramValue.pathDesc.iidStack = pathDesc.iidStack;
   sprintf(paramValue.pathDesc.paramName, "LowerLayers");
   paramValue.pParamType = "string";
   paramValue.pValue = lowerLayersBuf;
   paramValue.status = CMSRET_SUCCESS;

   ret = cmsPhl_setParameterValues(&paramValue, 1);
   if (ret != CMSRET_SUCCESS || paramValue.status != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getParamValue error: %d %d", ret, paramValue.status);
   }
}


void processIntfStackAliasChangedMsg(const CmsMsgHeader *msg)
{
   IntfStackAliasChangedMsgBody *aliasChangedMsg = (IntfStackAliasChangedMsgBody *) (msg+1);
   char aliasBuf[MDM_ALIAS_BUFLEN]={0};
   MdmPathDescriptor pathDesc;
   char *fullPath=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2InterfaceStackObject *intfStackObj=NULL;
   UBOOL8 doSet;
   CmsRet ret;

   /* gather info needed for this operation */
   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = aliasChangedMsg->oid;
   pathDesc.iidStack = aliasChangedMsg->iidStack;

   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullPath)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      return;
   }


   /*
    * loop through all intfstack table entries, find matching path, and
    * update the alias.
    */
   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);
      return;
   }

   ret = getAliasFromPathDescLocked(&pathDesc, aliasBuf);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get alias, ignore msg");
      cmsLck_releaseLock();
      CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);
      return;
   }

   cmsLog_debug("update %s alias to %s", fullPath, aliasBuf);

   while ((ret = cmsObj_getNext(MDMOID_DEV2_INTERFACE_STACK, &iidStack, (void **)&intfStackObj)) == CMSRET_SUCCESS)
   {
      doSet = FALSE;
      if (!cmsUtl_strcmp(intfStackObj->higherLayer, fullPath))
      {
         CMSMEM_REPLACE_STRING(intfStackObj->higherAlias, aliasBuf);
         doSet = TRUE;
      }
      if (!cmsUtl_strcmp(intfStackObj->lowerLayer, fullPath))
      {
         CMSMEM_REPLACE_STRING(intfStackObj->lowerAlias, aliasBuf);
         doSet = TRUE;
      }
      if (doSet)
      {
         ret = cmsObj_set((void *) intfStackObj, &iidStack);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("set of IntfStackObj failed, ret=%d", ret);
         }
      }
      cmsObj_free((void **) &intfStackObj);
   }

   writeIntfStackToConfigLocked();

   cmsLck_releaseLock();

   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);
   return;
}


void processIntfStackStaticAddressConfigdMsg(const CmsMsgHeader *msg)
{
   IntfStackStaticAddressConfig *staticAddrMsg = (IntfStackStaticAddressConfig *) (msg+1);
   char *ipIntfFullPath=NULL;
   MdmPathDescriptor ipIntfPathDesc=EMPTY_PATH_DESCRIPTOR;
   CmsRet ret;

   cmsLog_debug("Enter: ifName=%s isIPv4=%d isAdd=%d isMod=%d isDel=%d",
             staticAddrMsg->ifName, staticAddrMsg->isIPv4,
             staticAddrMsg->isAdd, staticAddrMsg->isMod, staticAddrMsg->isDel);


   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   /*
    * Get all the info we need before really starting to do the work.
    */

   if(qdmIntf_intfnameToFullPathLocked_dev2(staticAddrMsg->ifName, FALSE, &ipIntfFullPath)!=CMSRET_SUCCESS)
   {
      cmsLog_error("cannot get ipIntfFullPath of ifName<%s>", staticAddrMsg->ifName);
      cmsLck_releaseLock();
      return;
   }

   cmsLog_debug("%s ==> %s", staticAddrMsg->ifName, ipIntfFullPath);

   ret = cmsMdm_fullPathToPathDescriptor(ipIntfFullPath, &ipIntfPathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                   ipIntfFullPath, ret);
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
      cmsLck_releaseLock();
      return;
   }


   /*
    * If delete or modify, and if service state is in SERVICEUP,
    * bring the service status down to SERVICESTARTING.  Also update the
    * IP.Interface.Status (if necessary).
    * If service state is not SERVICEDOWN, that means lower layer link is
    * down.  Don't need to do anything here.  When link comes up, state
    * machine will be updated via propagateStatus.
    */
   if (staticAddrMsg->isDel || staticAddrMsg->isMod)
   {
      if (staticAddrMsg->isIPv4)
      {
         char ipv4ServiceStatusBuf[BUFLEN_64]={0};

         getIpv4ServiceStatusByIidLocked(&ipIntfPathDesc.iidStack,
                                         ipv4ServiceStatusBuf,
                                         sizeof(ipv4ServiceStatusBuf));
         cmsLog_debug("current IPv4ServiceStatus=%s", ipv4ServiceStatusBuf);

         if (!strcmp(ipv4ServiceStatusBuf, MDMVS_SERVICEUP))
         {
            /* update IP.Interface.Status first based on new IPv4ServiceStatus */
            intfStack_updateIpIntfStatusLocked(&ipIntfPathDesc.iidStack,
                                               MDMVS_SERVICESTARTING,
                                               NULL);

            setIpv4ServiceStatusByIidLocked(&ipIntfPathDesc.iidStack,
                                            MDMVS_SERVICESTARTING);
         }
      }
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
      else
      {
         char ipv6ServiceStatusBuf[BUFLEN_64]={0};

         getIpv6ServiceStatusByIidLocked(&ipIntfPathDesc.iidStack,
                                         ipv6ServiceStatusBuf,
                                         sizeof(ipv6ServiceStatusBuf));
         cmsLog_debug("current IPv6ServiceStatus=%s", ipv6ServiceStatusBuf);

         if (!strcmp(ipv6ServiceStatusBuf, MDMVS_SERVICEUP))
         {
            /* update IP.Interface.Status first based on new IPv6ServiceStatus */
            intfStack_updateIpIntfStatusLocked(&ipIntfPathDesc.iidStack,
                                               NULL,
                                               MDMVS_SERVICESTARTING);

            setIpv6ServiceStatusByIidLocked(&ipIntfPathDesc.iidStack,
                                            MDMVS_SERVICESTARTING);
         }
      }
#endif  /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */
   }


   /*
    * If add or modify, and if service state is in SERVICESTARTING,
    * we can bring the service state up to SERVICEUP.  Also update
    * IP.Interface.Status (if necessary).
    * If service state is not SERVICESTARTING, that means lower layer link is
    * down.  Don't need to do anything here.  When link comes up, state
    * machine will be updated via propagateStatus.
    */
   if (staticAddrMsg->isAdd || staticAddrMsg->isMod)
   {
      if (staticAddrMsg->isIPv4)
      {
         char ipv4ServiceStatusBuf[BUFLEN_64]={0};

         getIpv4ServiceStatusByIidLocked(&ipIntfPathDesc.iidStack,
                                         ipv4ServiceStatusBuf,
                                         sizeof(ipv4ServiceStatusBuf));
         cmsLog_debug("current IPv4ServiceStatus=%s", ipv4ServiceStatusBuf);

         if (!strcmp(ipv4ServiceStatusBuf, MDMVS_SERVICESTARTING))
         {
            /* update IP.Interface.Status first based on new IPv4ServiceStatus */
            intfStack_updateIpIntfStatusLocked(&ipIntfPathDesc.iidStack,
                                               MDMVS_SERVICEUP,
                                               NULL);

            setIpv4ServiceStatusByIidLocked(&ipIntfPathDesc.iidStack,
                                            MDMVS_SERVICEUP);
         }
      }
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
      else
      {
         char ipv6ServiceStatusBuf[BUFLEN_64]={0};

         getIpv6ServiceStatusByIidLocked(&ipIntfPathDesc.iidStack,
                                         ipv6ServiceStatusBuf,
                                         sizeof(ipv6ServiceStatusBuf));
         cmsLog_debug("current IPv6ServiceStatus=%s", ipv6ServiceStatusBuf);

         if (!strcmp(ipv6ServiceStatusBuf, MDMVS_SERVICESTARTING))
         {
            /* update IP.Interface.Status first based on new IPv6ServiceStatus */
            intfStack_updateIpIntfStatusLocked(&ipIntfPathDesc.iidStack,
                                               NULL,
                                               MDMVS_SERVICEUP);

            setIpv6ServiceStatusByIidLocked(&ipIntfPathDesc.iidStack,
                                            MDMVS_SERVICEUP);
         }
      }
#endif  /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */
   }

   cmsLck_releaseLock();

   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

   return;
}


void processIntfStackPropagateMsg(const CmsMsgHeader *msg)
{
   IntfStackPropagateStaus *propagaeStatusMsg = (IntfStackPropagateStaus *) (msg+1);
   char statusBuf[BUFLEN_64] = {0};
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   if ((ret = qdmIntf_getStatusFromFullPathLocked_dev2(propagaeStatusMsg->ipLowerLayerFullPath,
                                                       statusBuf, 
                                                       sizeof(statusBuf))) != CMSRET_SUCCESS)
   {
      cmsLog_error("getStatusFromFullPath failed for %s, ret=%d", propagaeStatusMsg->ipLowerLayerFullPath, ret);
      /* complain but don't exit */
   }
   else
   {
      intfStack_propagateStatusByFullPathLocked(propagaeStatusMsg->ipLowerLayerFullPath, statusBuf);
   }

   cmsLck_releaseLock();
}


void intfStack_updateIpIntfStatusLocked(const InstanceIdStack *ipIntfIidStack,
                                        const char *newIpv4ServiceStatus,
                                        const char *newIpv6ServiceStatus)
{
   char ipv4ServiceStatusBuf[BUFLEN_64]={0};
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   char ipv6ServiceStatusBuf[BUFLEN_64]={0};
#endif
   char newIpIntfStatusBuf[BUFLEN_64]={0};

   cmsLog_debug("Entered: iidStack=%s newIpv4ServiceStatus=%s newIPv6ServiceStatus=%s",
                cmsMdm_dumpIidStack(ipIntfIidStack),
                newIpv4ServiceStatus,
                newIpv6ServiceStatus);

   if (newIpv4ServiceStatus)
   {
      strcpy(ipv4ServiceStatusBuf, newIpv4ServiceStatus);
   }
   else
   {
      getIpv4ServiceStatusByIidLocked(ipIntfIidStack,
                                      ipv4ServiceStatusBuf,
                                      sizeof(ipv4ServiceStatusBuf));
      cmsLog_debug("current IPv4ServiceStatus=%s", ipv4ServiceStatusBuf);
   }

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (newIpv6ServiceStatus)
   {
      strcpy(ipv6ServiceStatusBuf, newIpv6ServiceStatus);
   }
   else
   {
      getIpv6ServiceStatusByIidLocked(ipIntfIidStack,
                                      ipv6ServiceStatusBuf,
                                      sizeof(ipv6ServiceStatusBuf));
      cmsLog_debug("current IPv6ServiceStatus=%s", ipv6ServiceStatusBuf);
   }
#endif


   /* if either IPv4 or IPv6 state machine is SERVICEUP, then
    * IP.Interface.Status=UP */
   if (!strcmp(ipv4ServiceStatusBuf, MDMVS_SERVICEUP))
   {
      strcpy(newIpIntfStatusBuf, MDMVS_UP);
   }
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   else if (!strcmp(ipv6ServiceStatusBuf, MDMVS_SERVICEUP))
   {
      strcpy(newIpIntfStatusBuf, MDMVS_UP);
   }
#endif
   /* else if either IPv4 or IPv6 state machine is SERVICESTARTING, then
    * IP.Interface.Status=DORMANT */
   else if (!strcmp(ipv4ServiceStatusBuf, MDMVS_SERVICESTARTING))
   {
      strcpy(newIpIntfStatusBuf, MDMVS_DORMANT);
   }
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   else if (!strcmp(ipv6ServiceStatusBuf, MDMVS_SERVICESTARTING))
   {
      strcpy(newIpIntfStatusBuf, MDMVS_DORMANT);
   }
#endif
   /* else, IP.Interface.Status must be LOWERLAYERDOWN */
   else
   {
      strcpy(newIpIntfStatusBuf, MDMVS_LOWERLAYERDOWN);
   }

   /* now set to new status (its OK if it is the same as current status) */
   {
      MdmPathDescriptor ipIntfPathDesc=EMPTY_PATH_DESCRIPTOR;

      ipIntfPathDesc.oid = MDMOID_DEV2_IP_INTERFACE;
      ipIntfPathDesc.iidStack = *ipIntfIidStack;

      intfStack_setStatusByPathDescLocked(&ipIntfPathDesc, newIpIntfStatusBuf);
   }

   return;
}


CmsRet getAliasFromFullPathLocked(const char *fullPath, char *alias)
{
   MdmPathDescriptor pathDesc;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", fullPath, ret);
      return ret;
   }

   return (getAliasFromPathDescLocked(&pathDesc, alias));
}


CmsRet getAliasFromPathDescLocked(const MdmPathDescriptor *pathDescIn, char *alias)
{
   MdmPathDescriptor pathDesc;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = pathDescIn->oid;
   pathDesc.iidStack = pathDescIn->iidStack;
   sprintf(pathDesc.paramName, "Alias");

   ret = cmsPhl_getParamValue(&pathDesc, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_notice("cmsPhl_getParamValue error: %d", ret);
   }
   else
   {
      if (cmsUtl_strlen(pParamValue->pValue) > 0)
      {
         sprintf(alias, "%s", pParamValue->pValue);
      }
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }

   return ret;
}


void intfStack_setStatusByFullPathLocked(const char *fullPath, const char *status)
{
   MdmPathDescriptor pathDesc;
   CmsRet ret;

   cmsLog_debug("set %s to status %s", fullPath, status);

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", fullPath, ret);
      return;
   }

   intfStack_setStatusByPathDescLocked(&pathDesc, status);
}


void intfStack_setStatusByPathDescLocked(const MdmPathDescriptor *pathDescIn, const char *status)
{
   PhlSetParamValue_t paramValue;
   CmsRet ret;

   cmsLog_debug("Entered: oid=%d iidStack=%s status=%s",
                pathDescIn->oid,
                cmsMdm_dumpIidStack(&pathDescIn->iidStack),
                status);

   memset(&paramValue, 0, sizeof(paramValue));
   paramValue.pathDesc.oid = pathDescIn->oid;
   paramValue.pathDesc.iidStack = pathDescIn->iidStack;
   paramValue.pParamType = "string";
   sprintf(paramValue.pathDesc.paramName, "Status");
   paramValue.pValue = (char *) status;
   paramValue.status = CMSRET_SUCCESS;

   ret = cmsPhl_setParameterValues(&paramValue, 1);
   if (ret != CMSRET_SUCCESS || paramValue.status != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getParamValue error: %d %d", ret, paramValue.status);
   }

   return;
}


void intfStack_propagateStatusByIidLocked(MdmObjectId oid, const InstanceIdStack *iidStack, const char *status)
{
   MdmPathDescriptor pathDesc;
   char *fullPath=NULL;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = oid;
   pathDesc.iidStack = *iidStack;

   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullPath)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      return;
   }

   intfStack_propagateStatusByFullPathLocked(fullPath, status);

   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);

}


void intfStack_propagateStatusByFullPathLocked(const char *lowerLayerFullPath,
                                               const char *newStatus)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2InterfaceStackObject *intfStackObj=NULL;

   if (IS_EMPTY_STRING(lowerLayerFullPath))
   {
      cmsLog_error("called with NULL or empty lowerLayerFullPath -- just return");
      return;
   }

   cmsLog_debug("Enter: lowerLayerFullPath=%s newStatus=%s",
                lowerLayerFullPath, newStatus);

   /*
    * Walk through the entire interface stack table looking for entries
    * that match our lowerLayerFullPath.
    */
   while (cmsObj_getNextFlags(MDMOID_DEV2_INTERFACE_STACK, &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&intfStackObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(intfStackObj->lowerLayer, lowerLayerFullPath))
      {
         intfStack_propagateStatusOnSingleEntryLocked(intfStackObj, newStatus);
      }

      cmsObj_free((void **) &intfStackObj);
   }

   cmsLog_debug("Exit: lowerLayerFullPath=%s newStatus=%s",
                lowerLayerFullPath, newStatus);

   return;
}


void intfStack_propagateStatusOnSingleEntryLocked(const Dev2InterfaceStackObject *intfStackObj,
                                                  const char *newStatus)
{
   char higherLayerStatusBuf[BUFLEN_64]={0};
   MdmPathDescriptor higherLayerPathDesc;
   CmsRet ret;

   cmsLog_debug("Enter: %s->%s (%s)",
                intfStackObj->lowerLayer, intfStackObj->higherLayer,
                newStatus);


   ret = qdmIntf_getStatusFromFullPathLocked_dev2(intfStackObj->higherLayer,
                                      higherLayerStatusBuf,
                                      sizeof(higherLayerStatusBuf));
   if (ret != CMSRET_SUCCESS)
   {
      /*
       * When we delete a whole WAN service from WebUI, by the time
       * we get here, the higher layer object is already deleted.
       * Do not complain loudly about the error, just return.  There is
       * nothing to be done.
       */
      cmsLog_debug("qdmIntf_getStatusFromFullPath for %s failed, ret=%d",
                   intfStackObj->higherLayer, ret);
      return;
   }

   /*
    * Convert higherLayerFullPath to pathDesc to check for special cases
    */
   INIT_PATH_DESCRIPTOR(&higherLayerPathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(intfStackObj->higherLayer, &higherLayerPathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                    intfStackObj->higherLayer, ret);
      return;
   }

   /*
    * If higher layer status is already equal to newStatus, then we
    * don't need to do process further.
    * Special case: for IP.Interface, for the UP status, even when
    * higher layer status is equal to newStatus continue processing
    * because the IP.Interface could be UP and IPv4ServiceStatus is
    * SERVICEUP, but the IPv6ServiceStatus is still in SERVICESTARTING.
    * So continue processing so IPv6ServiceStatus can get moved to
    * SERVICEUP.  (Actually, this is only needed for PPP, not for dhcpc,
    * but I don't bother to check for ppp specifically.)
    */
   if (((higherLayerPathDesc.oid != MDMOID_DEV2_IP_INTERFACE) &&
        !cmsUtl_strcmp(newStatus, higherLayerStatusBuf))            ||
       ((higherLayerPathDesc.oid == MDMOID_DEV2_IP_INTERFACE) &&
        !cmsUtl_strcmp(newStatus, higherLayerStatusBuf) &&
        cmsUtl_strcmp(newStatus, MDMVS_UP)))

   {
      cmsLog_debug("higherLayer %s is already %s -- "
                   "no more processing on this branch of the stack",
                   intfStackObj->higherLayer, newStatus);
      return;
   }

   cmsLog_debug("higherLayer %s (status=%s) ==> %s",
           intfStackObj->higherLayer, higherLayerStatusBuf, newStatus);

   /*
    * If we get here, we need to set new status on the higherLayer.
    * But first check for special case situations, e.g.
    * ppp state machine, IP layer service state machine,
    * DSL channel types, etc.
    */
   if ((higherLayerPathDesc.oid == MDMOID_DEV2_PPP_INTERFACE) &&
       (!strcmp(newStatus, MDMVS_UP)))
   {
      cmsLog_debug("in ppp special case");
      /* Set PPP.Interface status to DORMANT, meaning it is waiting
       * for the ppp client to connect to the server.
       */
      intfStack_setStatusByFullPathLocked(intfStackObj->higherLayer,
                                          MDMVS_DORMANT);

      /* Set ConnectionStatus on the ppp object so it can start the
       * ppp client */
      sskConn_setPppConnStatusByIidLocked(&higherLayerPathDesc.iidStack,
                                          MDMVS_CONNECTING);

      /*
       * Do not propagate status up.  PPP will send messages back
       * to ssk.  The ssk2_connstatus.c code will call
       * propagateStatus when it is fully connected.
       */
   }
   else if ((higherLayerPathDesc.oid == MDMOID_DEV2_IP_INTERFACE) &&
           (!strcmp(newStatus, MDMVS_UP)))
   {
      Dev2IpInterfaceObject *ipIntfObj=NULL;
      CmsRet r2;

      cmsLog_debug("in IP.Interface special case");

      if ((r2 = cmsObj_get(higherLayerPathDesc.oid,
                           &higherLayerPathDesc.iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **)&ipIntfObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get IP.Interface object, ret=%d", ret);
      }
      else
      {
         /*
          * DORMANT means IP.Interface is about to come up, but still
          * waiting for some external events, e.g. dhcpc or ppp to
          * get the IP address.
          */
         if (strcmp(higherLayerStatusBuf, MDMVS_DORMANT) &&
             strcmp(higherLayerStatusBuf, MDMVS_UP))
         {
            intfStack_setStatusByFullPathLocked(intfStackObj->higherLayer,
                                                MDMVS_DORMANT);
         }

         if (ipIntfObj->IPv4Enable)
         {
            char ipv4ServiceStatusBuf[BUFLEN_64]={0};

            getIpv4ServiceStatusByIidLocked(&higherLayerPathDesc.iidStack,
                                            ipv4ServiceStatusBuf,
                                     sizeof(ipv4ServiceStatusBuf));
            cmsLog_debug("%s current IPv4ServiceStatus=%s",
                         ipIntfObj->name, ipv4ServiceStatusBuf);

            /*
             * If we are currently in SERVICEDOWN, advance to
             * SERVICESTARTING.  If we are not in SERVICEUP, check if
             * we can go to SERVICEUP.  We may end up doing nothing on
             * the IPv4 side because this UP status is being propagated
             * up the intfStack for IPv6.
             */
            if (!strcmp(ipv4ServiceStatusBuf, MDMVS_SERVICEDOWN))
            {
               setIpv4ServiceStatusByIidLocked(&higherLayerPathDesc.iidStack,
                                               MDMVS_SERVICESTARTING);
               strcpy(ipv4ServiceStatusBuf, MDMVS_SERVICESTARTING);
            }

            if (strcmp(ipv4ServiceStatusBuf, MDMVS_SERVICEUP))
            {
               if (sskConn_hasAnyIpv4AddressLocked(&higherLayerPathDesc.iidStack) ||
                   (ipIntfObj->X_BROADCOM_COM_BridgeService == TRUE &&
                    ipIntfObj->X_BROADCOM_COM_BridgeNeedsIpAddr == FALSE))
               {
                  /*
                   * IP.Interface has an address (it may be static, or
                   * it may have been set already by ppp or dhcpc).
                   * Or it is a bridge service, which normally does not
                   * need IP addr. It can now go into the UP state.
                   */
                  intfStack_setStatusByFullPathLocked(intfStackObj->higherLayer,
                                                      MDMVS_UP);
                  setIpv4ServiceStatusByIidLocked(&higherLayerPathDesc.iidStack,
                                                  MDMVS_SERVICEUP);
#ifdef BRCM_VOICE_SUPPORT
                  if (!ipIntfObj->X_BROADCOM_COM_BridgeService)
                  {
                     initVoiceOnIntfUpLocked_dev2(CMS_AF_SELECT_IPV4,
                                          ipIntfObj->name,
                                          ipIntfObj->X_BROADCOM_COM_Upstream);
                  }
#endif
               }
               /* else {
                *   this IP.Interface does not have any IP address
                *   configured, so nothing more to do here.  dhcpc will
                *   send messages back to ssk.  The code in
                *   ssk2_connstatus.c will advance the interface stack
                *   status and service state machines.
                * }
                */
            }
         }

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
         /* see comments for the IPv4 case */
         if (ipIntfObj->IPv6Enable)
         {
            char ipv6ServiceStatusBuf[BUFLEN_64]={0};

            getIpv6ServiceStatusByIidLocked(&higherLayerPathDesc.iidStack,
                                            ipv6ServiceStatusBuf,
                                     sizeof(ipv6ServiceStatusBuf));

            cmsLog_debug("%s current IPv6ServiceStatus=%s",
                         ipIntfObj->name, ipv6ServiceStatusBuf);

            if (!strcmp(ipv6ServiceStatusBuf, MDMVS_SERVICEDOWN))
            {
               setIpv6ServiceStatusByIidLocked(&higherLayerPathDesc.iidStack,
                                               MDMVS_SERVICESTARTING);
               strcpy(ipv6ServiceStatusBuf, MDMVS_SERVICESTARTING);
            }

            if (strcmp(ipv6ServiceStatusBuf, MDMVS_SERVICEUP))
            {
               /* For IPv6, "any" IPv6 address may be too relaxed.
                * Maybe we can only move to UP and SERVICEUP if we have
                * a globally unique address (not link local).
                */
               if (sskConn_hasAnyIpv6AddressLocked(&higherLayerPathDesc.iidStack))
               {
                  intfStack_setStatusByFullPathLocked(intfStackObj->higherLayer,
                                                      MDMVS_UP);
                  setIpv6ServiceStatusByIidLocked(&higherLayerPathDesc.iidStack,
                                                  MDMVS_SERVICEUP);
#ifdef BRCM_VOICE_SUPPORT
                  if (!ipIntfObj->X_BROADCOM_COM_BridgeService)
                  {
                     initVoiceOnIntfUpLocked_dev2(CMS_AF_SELECT_IPV6,
                                          ipIntfObj->name,
                                          ipIntfObj->X_BROADCOM_COM_Upstream);
                  }
#endif
               }
            }
         }
#endif  /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */
         cmsObj_free((void **) &ipIntfObj);
      }

      /* Since IP.Interface is the highest interface in the intf stack,
       * no need to propagate up.
       */
   }
#ifdef DMP_DEVICE2_DSL_1
   else if ((higherLayerPathDesc.oid == MDMOID_DEV2_DSL_CHANNEL) &&
            (strcmp(newStatus, MDMVS_DOWN) != 0))
   {
      /* if DSL line is trained to use ATM encapsulation, then propagate UP status to ATM channel,
       * and set the PTM channel to disable.  And the same would apply to PTM encapsulation.
       */
      Dev2DslChannelObject *channelObj=NULL;
      CmsRet rc;
      UBOOL8 encapNotTrained = FALSE;

      if ((rc = cmsObj_get(higherLayerPathDesc.oid,
                           &higherLayerPathDesc.iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **)&channelObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get DSL.Channel object, ret=%d", rc);
      }
      else
      {
         if (qdmDsl_isAtmConnectionLocked(channelObj->lowerLayers))
         {
            if (!strcmp(channelObj->linkEncapsulationUsed,MDMVS_G_993_2_ANNEX_K_PTM))
            {
               /* channel is an ATM channel, but link is trained PTM */
               encapNotTrained = TRUE;
            }
         }
         else
         {
            if (!strcmp(channelObj->linkEncapsulationUsed,MDMVS_G_992_3_ANNEX_K_ATM))
            {
               /* channel is PTM, but link is trained ATM */
               encapNotTrained = TRUE;
            }
         }
         if (encapNotTrained == TRUE)
         {
            intfStack_setStatusByFullPathLocked(intfStackObj->higherLayer,
                                                MDMVS_NOTPRESENT);
            /* propagate status Link NotPresent to upper layers on the interface stack */
            intfStack_propagateStatusByFullPathLocked(intfStackObj->higherLayer,
                                                      MDMVS_NOTPRESENT);
         }
         else
         {
            /* propagate status UP to upper layers on the interface stack */
            intfStack_setStatusByFullPathLocked(intfStackObj->higherLayer,
                                                newStatus);
            intfStack_propagateStatusByFullPathLocked(intfStackObj->higherLayer,
                                                      newStatus);
         }
         cmsObj_free((void **) &channelObj);
      } /* channelObj */
   } /* DSL channel */
#ifdef DMP_DEVICE2_BONDEDDSL_1
   /* same for bonding, the status of a bonding group is not just dependent on the channels,
      bonding status needs to be retrieved from the driver even when the channels are up */
   else if ((higherLayerPathDesc.oid == MDMOID_DEV2_DSL_BONDING_GROUP) &&
            (strcmp(newStatus,MDMVS_UP) == 0))
   {
      Dev2DslBondingGroupObject *bondingGroupObj=NULL;
      CmsRet rc;

      if ((rc = cmsObj_get(higherLayerPathDesc.oid,
                           &higherLayerPathDesc.iidStack,
                           OGF_NO_VALUE_UPDATE,
                           (void **)&bondingGroupObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get DSL.BondingGroup object, ret=%d", rc);
         intfStack_setStatusByFullPathLocked(intfStackObj->higherLayer,
                                             MDMVS_UNKNOWN);
         /* propagate status Link NotPresent to upper layers on the interface stack */
         intfStack_propagateStatusByFullPathLocked(intfStackObj->higherLayer,
                                                   MDMVS_UNKNOWN);
      }
      else
      {
         cmsObj_free((void **) &bondingGroupObj);
         intfStack_setStatusByFullPathLocked(intfStackObj->higherLayer,
                                             newStatus);
         /* need to check to see if we need to change the lower layer of
          * PTM/ATM links because there may be traffic type mismatch for bonding
         */
         updateXtmLowerLayerLocked();
         intfStack_propagateStatusByFullPathLocked(intfStackObj->higherLayer,
                                                   newStatus);
      }
   } /* DSL bonding group OID */
#endif /* DMP_DEVICE2_BONDEDDSL_1 */
#endif /* DMP_DEVICE2_DSL_1 */
   else if (!strcmp(newStatus, MDMVS_DOWN) ||
            !strcmp(newStatus, MDMVS_UNKNOWN) ||
            !strcmp(newStatus, MDMVS_DORMANT) ||
            !strcmp(newStatus, MDMVS_NOTPRESENT) ||
            !strcmp(newStatus, MDMVS_LOWERLAYERDOWN) ||
            !strcmp(newStatus, MDMVS_ERROR))
   {
      cmsLog_debug("in newStatus=%s special case", newStatus);

      if (isBridgeMgmtPort(intfStackObj->higherLayer))
      {
         cmsLog_debug("do not propagate non-UP status through bridge mgmt port %s",
                      intfStackObj->higherLayer);
      }
      else if (isAnyLowerLayerIntfUpLocked(intfStackObj->higherLayer,
                                           intfStackObj->lowerLayer))
      {
         cmsLog_debug("one or more lowerLayer interfaces is still UP -- do nothing");
      }
      else
      {
         /*
          * xlate lowerLayer status to appropriate higherLayer status.
          * Seems like in all cases, the appropriate higherLayer status
          * is MDMVS_LOWERLAYERDOWN:
          * LowerLayer Status            HigherLayer Status
          * DOWN                   ->    LOWERLAYERDOWN
          * UNKNOWN                ->    LOWERLAYERDOWN
          * DORMANT                ->    LOWERLAYERDOWN
          * NOTPRESENT             ->    LOWERLAYERDOWN
          * LOWERLAYERDOWN         ->    LOWERLAYERDOWN
          * ERROR                  ->    LOWERLAYERDOWN
          */

         intfStack_setStatusByFullPathLocked(intfStackObj->higherLayer,
                                             MDMVS_LOWERLAYERDOWN);

         if (higherLayerPathDesc.oid == MDMOID_DEV2_IP_INTERFACE)
         {
            /*
             * we have just marked the IP.Interface LOWERLAYERDOWN,
             * so change IPv4 and IPv6 service states to SERVICEDOWN.
             */
            Dev2IpInterfaceObject *ipIntfObj=NULL;
            CmsRet r2;

            if ((r2 = cmsObj_get(higherLayerPathDesc.oid,
                                 &higherLayerPathDesc.iidStack,
                                 OGF_NO_VALUE_UPDATE,
                                 (void **)&ipIntfObj)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Could not get IP.Interface object, ret=%d", r2);
            }
            else
            {
               if (ipIntfObj->IPv4Enable)
               {
                  setIpv4ServiceStatusByIidLocked(&higherLayerPathDesc.iidStack,
                                                  MDMVS_SERVICEDOWN);
               }

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
               if (ipIntfObj->IPv6Enable)
               {
                  setIpv6ServiceStatusByIidLocked(&higherLayerPathDesc.iidStack,
                                                  MDMVS_SERVICEDOWN);
               }
#endif
               cmsObj_free((void **) &ipIntfObj);
            }

            /* IP.Interface is highest layer in the stack, so no need
             * to propagate further.
             */
         }
         else
         {
            if (higherLayerPathDesc.oid == MDMOID_DEV2_PPP_INTERFACE)
            {
               /*
                * When link goes down, we also need to set ppp
                * connStatus to DISCONNECTED in addition to all the
                * normal interface stack propagation.
                */
               sskConn_setPppConnStatusByIidLocked(&higherLayerPathDesc.iidStack,
                                                   MDMVS_DISCONNECTED);
            }

            /* propagate up the interface stack */
            cmsLog_debug("down case: set %s ==> %s and propagate",
                         intfStackObj->higherLayer, newStatus);
            intfStack_propagateStatusByFullPathLocked(intfStackObj->higherLayer,
                                               MDMVS_LOWERLAYERDOWN);
         }
      }
   }
   else
   {
      /*
       * no special case handling, so just set the current higher layer
       * object status and propagate up the interface stack.
       */
      cmsLog_debug("normal: set %s ==> %s and propagate",
                    intfStackObj->higherLayer, newStatus);
      intfStack_setStatusByFullPathLocked(intfStackObj->higherLayer,
                                          newStatus);

      intfStack_propagateStatusByFullPathLocked(intfStackObj->higherLayer,
                                                newStatus);
   }

   cmsLog_debug("Exit: %s->%s (%s)",
                intfStackObj->lowerLayer, intfStackObj->higherLayer,
                newStatus);

   return;
}


UBOOL8 isBridgeMgmtPort(const char *higherLayerFullPath)
{
   Dev2BridgePortObject *brPortObj=NULL;
   MdmPathDescriptor pathDesc;
   UBOOL8 rv=FALSE;

   cmsMdm_fullPathToPathDescriptor(higherLayerFullPath, &pathDesc);
   if (pathDesc.oid == MDMOID_DEV2_BRIDGE_PORT)
   {
      if (cmsObj_get(pathDesc.oid, &pathDesc.iidStack, 0, (void **) &brPortObj) == CMSRET_SUCCESS)
      {
         rv = brPortObj->managementPort;
         cmsObj_free((void **) &brPortObj);
      }
   }

   return rv;
}


/** check if the given higherLayerFullPath has any lowerLayers which is
 *  still up, but do not consider the excludeLowerlayerFullPath.  The
 *  excludeLowerLayerFullPath logic is needed in the delete a fullpath from
 *  the LowerLayers param case because we propagate status before deleting
 *  the intf stack entry.
 *
 *  @param higherLayerFullPath (IN) higher layer to check
 *  @param excludeLowerLayerFullPath (IN) exclude this lower layer
 *
 *  @return TRUE if there is a lower layer with status UP
 */
UBOOL8 isAnyLowerLayerIntfUpLocked(const char *higherLayerFullPath,
                                   const char *excludeLowerLayerFullPath)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2InterfaceStackObject *intfStackObj=NULL;
   UBOOL8 isUp=FALSE;
   CmsRet ret;

   while (!isUp &&
          (ret = cmsObj_getNext(MDMOID_DEV2_INTERFACE_STACK, &iidStack, (void **)&intfStackObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(intfStackObj->higherLayer, higherLayerFullPath) &&
           cmsUtl_strcmp(intfStackObj->lowerLayer, excludeLowerLayerFullPath))
      {
         isUp = qdmIntf_isStatusUpOnFullPathLocked_dev2(intfStackObj->lowerLayer);
      }

      cmsObj_free((void **) &intfStackObj);
   }

   cmsLog_debug("higherLayer=%s isUp=%d", higherLayerFullPath, isUp);
   return isUp;
}


UBOOL8 getUpperLayerPathDescFromLowerLayerLocked(const char *lowerLayerFullPath,
                                       MdmObjectId oid,
                                       MdmPathDescriptor *upperLayerPathDesc)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2InterfaceStackObject *intfStackObj=NULL;
   MdmPathDescriptor higherLayerPathDesc;
   CmsRet ret;
   UBOOL8 found=FALSE;

   INIT_PATH_DESCRIPTOR(upperLayerPathDesc);

   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DEV2_INTERFACE_STACK, &iidStack,
                                 (void **)&intfStackObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(intfStackObj->lowerLayer, lowerLayerFullPath))
      {
         INIT_PATH_DESCRIPTOR(&higherLayerPathDesc);
         ret = cmsMdm_fullPathToPathDescriptor(intfStackObj->higherLayer, &higherLayerPathDesc);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                          intfStackObj->higherLayer, ret);
            cmsObj_free((void **) &intfStackObj);
            return FALSE;
         }

         if (higherLayerPathDesc.oid == oid)
         {
            *upperLayerPathDesc = higherLayerPathDesc;
            found = TRUE;
         }
         else
         {
            /* recurse */
            return (getUpperLayerPathDescFromLowerLayerLocked(
                                               intfStackObj->higherLayer,
                                               oid,
                                               upperLayerPathDesc));
         }
      }

      cmsObj_free((void **) &intfStackObj);
   }

   return found;
}


void writeIntfStackToConfigLocked()
{
   CmsRet ret;
   CmsTimestamp nowTms;
   UINT32 deltaMs;

   cmsTms_get(&nowTms);
   deltaMs = cmsTms_deltaInMilliSeconds(&nowTms, &bootTimestamp);

   cmsLog_debug("deltaMs=%d", deltaMs);

   if (deltaMs < 5000)
   {
      /*
       * When the system boots, we will get about 20 LowerLayersChanged
       * messages as the MDM is initializing itself.  Do not write these
       * to the flash.  Either, they are duplicate entries, or
       * they will be regenerated on the next boot.  However, after the
       * first 5 seconds, any changes to the Interface stack table are
       * probably done by the ACS or a human, so they must be saved.
       */
      return;
   }

   if ((ret = cmsMgm_saveConfigToFlash()) != CMSRET_SUCCESS)
   {
      cmsLog_error("write to config file failed, ret=%d", ret);
   }
}


#if defined(DMP_BASELINE_1) && defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)

UBOOL8 isTr98WanIpPppObj(const char *fullPath)
{
   MdmPathDescriptor pathDesc = EMPTY_PATH_DESCRIPTOR;
   CmsRet ret;

   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", fullPath, ret);
      return FALSE;
   }

   if ((pathDesc.oid == MDMOID_WAN_IP_CONN) ||
       (pathDesc.oid == MDMOID_WAN_PPP_CONN))
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}



/* in rut_wanlayer2.c */
extern UBOOL8 rutWl2_isWanLayer2LinkUp(MdmObjectId wanConnOid,
                                       const InstanceIdStack *iidStack);

CmsRet getTr98L2StatusFromL3FullPath(const char *fullPath,
                                     char *statusBuf, UINT32 statusBufLen)
{
   MdmPathDescriptor pathDesc = EMPTY_PATH_DESCRIPTOR;
   CmsRet ret;

   memset(statusBuf, 0, statusBufLen);

   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", fullPath, ret);
      return ret;
   }

   if ((pathDesc.oid != MDMOID_WAN_IP_CONN) &&
       (pathDesc.oid != MDMOID_WAN_PPP_CONN))
   {
      cmsLog_error("OID %d not supported", pathDesc.oid);
      return CMSRET_INVALID_ARGUMENTS;
   }

   if (rutWl2_isWanLayer2LinkUp(pathDesc.oid, &pathDesc.iidStack))
   {
      snprintf(statusBuf, statusBufLen, "%s", MDMVS_UP);
   }
   else
   {
      snprintf(statusBuf, statusBufLen, "%s", MDMVS_DOWN);
   }

   cmsLog_debug("%s status %s", fullPath, statusBuf);
   return CMSRET_SUCCESS;
}
#endif /* defined(DMP_BASELINE_1) && defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1) */

#endif  /* DMP_DEVICE2_BASELINE_1 */
