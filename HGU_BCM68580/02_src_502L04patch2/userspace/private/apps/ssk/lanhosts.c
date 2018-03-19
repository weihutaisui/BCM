/***********************************************************************
 *
 *  Copyright (c) 2008  Broadcom Corporation
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

#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_msg.h"
#include "ssk.h"
#include "oal.h"


/** This file contains functions to update the LANDevice.{i}.Hosts.Host.{i} object
 */


static CmsRet getHostEntryByMacAddr(const InstanceIdStack *parentIidStack, const char *macAddr, InstanceIdStack *iidStack, LanHostEntryObject **hostEntry);
static CmsRet getManageableDeviceEntry(const char *oui, const char *serialNum, const char *productClass, InstanceIdStack *iidStack, ManageableDeviceObject **manageableObj);


void processLanHostInfoMsg_igd(CmsMsgHeader *msg)
{
   LanDevObject *lanObj=NULL;
   LanHostEntryObject *hostEntry=NULL;
   DhcpdHostInfoMsgBody *body = (DhcpdHostInfoMsgBody *) (msg + 1);
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack searchIidStack = EMPTY_INSTANCE_ID_STACK;
   ManageableDeviceObject *manageableObj = NULL;
   CmsRet ret;

   if (msg->dataLength != sizeof(DhcpdHostInfoMsgBody))
   {
      cmsLog_error("bad data length, got %d expected %d, drop msg",
                   msg->dataLength, sizeof(DhcpdHostInfoMsgBody));
      return;
   }

   cmsLog_debug("Entered: delete=%d ifName=%s ipAddr=%s macAddr=%s interfaceType=%s remaining=%d",
                body->deleteHost, body->ifName, body->ipAddr, body->macAddr, body->interfaceType, body->leaseTimeRemaining);
   cmsLog_debug("oui=%s serialNum=%s productClass=%s", 
                body->oui,body->serialNum,body->productClass);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d, drop message", ret);
      return;
   }

   ret = dalLan_getLanDevByBridgeIfName(body->ifName, &iidStack, &lanObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not find bridge ifname %s", body->ifName);
      cmsLck_releaseLock();
      return;
   }

   /* we don't need the lanObj, just its iidStack */
   cmsObj_free((void **) &lanObj);


   if (body->deleteHost)
   {
      /* this is the delete case */
      if ((ret = getHostEntryByMacAddr(&iidStack, body->macAddr, &searchIidStack, &hostEntry)) == CMSRET_SUCCESS)
      {
         cmsLog_debug("deleteing host entry");
         cmsObj_deleteInstance(MDMOID_LAN_HOST_ENTRY, &searchIidStack);
         cmsObj_free((void **) &hostEntry);
      }
      else
      {
         cmsLog_notice("could not find host entry for mac Addr %s", body->macAddr);
      }

      INIT_INSTANCE_ID_STACK(&iidStack);
      if ((strlen(body->oui)) > 0)
      {
         if (CMSRET_SUCCESS == getManageableDeviceEntry(body->oui, body->serialNum, body->productClass, &iidStack, &manageableObj))
         {
            cmsObj_free((void **) &manageableObj);
            cmsLog_debug("deleteing manageable entry");
            cmsObj_deleteInstance(MDMOID_MANAGEABLE_DEVICE, &iidStack);               
         }
      } /* body->oui */
   } /* deleteHost */
   else
   {
      if ((ret = getHostEntryByMacAddr(&iidStack, body->macAddr, &searchIidStack, &hostEntry)) == CMSRET_SUCCESS)
      {
         /* this is edit case, only a few fields can change */
         cmsLog_debug("edit existing host entry for mac addr %s", body->macAddr);
         CMSMEM_REPLACE_STRING(hostEntry->IPAddress, body->ipAddr);
         hostEntry->leaseTimeRemaining = body->leaseTimeRemaining;

         iidStack = searchIidStack; /* the cmsObj_set below uses iidStack */
      }
      else
      {
         /* this is the new host detected case */
         cmsLog_debug("adding new host entry instance for mac addr %s", body->macAddr);

         if ((ret = cmsObj_addInstance(MDMOID_LAN_HOST_ENTRY, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("add instance of lan host returned %d", ret);
         }
         else
         {
            cmsObj_get(MDMOID_LAN_HOST_ENTRY, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &hostEntry);

            CMSMEM_REPLACE_STRING(hostEntry->IPAddress, body->ipAddr);
            CMSMEM_REPLACE_STRING(hostEntry->MACAddress, body->macAddr);
            CMSMEM_REPLACE_STRING(hostEntry->addressSource, body->addressSource);
            CMSMEM_REPLACE_STRING(hostEntry->interfaceType, body->interfaceType);
            CMSMEM_REPLACE_STRING(hostEntry->hostName, body->hostName);

#ifdef DMP_BRIDGING_1
            UINT8 macNum[6];
            char portName[CMS_IFNAME_LENGTH]={0};

            cmsUtl_macStrToNum(body->macAddr, macNum);
            if (oal_getPortNameFromMacAddr(body->ifName, macNum, portName) == CMSRET_SUCCESS)
            {
               CMSMEM_REPLACE_STRING(hostEntry->X_BROADCOM_COM_IfName, portName);
            }
            else
            {
               cmsLog_error("oal_getPortnameFromMacAddr failed, ret=%d", ret);
            }
#endif

            hostEntry->leaseTimeRemaining = body->leaseTimeRemaining;
            hostEntry->active = TRUE;
         }
      }

      /* both edit and add new will need to do a set and free */
      if ((ret = cmsObj_set(hostEntry, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("set of host entry failed, ret=%d (leasetimeremaining=%d)", ret, hostEntry->leaseTimeRemaining);
      }
      else
      {
         cmsLog_debug("host entry added/updated");
      }
      cmsObj_free((void **) &hostEntry);


      /* Begin processing manageable device option */
      /* do we have this entry already? if yes, don't do anything */

      if (strlen(body->oui) > 0)
      {
         INIT_INSTANCE_ID_STACK(&iidStack);
         if (CMSRET_SUCCESS == getManageableDeviceEntry(body->oui, body->serialNum, body->productClass, &iidStack, &manageableObj))
         {
            /* found the exact entry */
            cmsLck_releaseLock();
            cmsObj_free((void **) &manageableObj);
            return;
         }

         /* an entry needs to be added */
         cmsLog_debug("adding new manageable device entry instance oui %s", body->oui);

         INIT_INSTANCE_ID_STACK(&iidStack);
         if ((ret = cmsObj_addInstance(MDMOID_MANAGEABLE_DEVICE, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("add instance of lan host returned %d", ret);
         }
         else
         {
            cmsObj_get(MDMOID_MANAGEABLE_DEVICE, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &manageableObj);
            
            CMSMEM_REPLACE_STRING(manageableObj->manufacturerOUI,body->oui);
            CMSMEM_REPLACE_STRING(manageableObj->serialNumber,body->serialNum);
            CMSMEM_REPLACE_STRING(manageableObj->productClass,body->productClass);
            
            if ((ret = cmsObj_set(manageableObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("set of manageable entry failed, ret=%d", ret);
            }
            else
            {
               cmsLog_debug("manageable entry added");
            }
            
            cmsObj_free((void **) &manageableObj);
            /* end of manageableDevice option */
         } /* add instance, ok */
      } /* if body->oui exists */
   } /* add or edit */

   cmsLck_releaseLock();

}


CmsRet getHostEntryByMacAddr(const InstanceIdStack *parentIidStack, const char *macAddr, InstanceIdStack *iidStack, LanHostEntryObject **hostEntry)
{
   UBOOL8 found=FALSE;
   CmsRet ret;

   INIT_INSTANCE_ID_STACK(iidStack);

   while (!found &&
          ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_LAN_HOST_ENTRY, parentIidStack, iidStack, OGF_NO_VALUE_UPDATE, (void **) hostEntry)) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp((*hostEntry)->MACAddress, macAddr))
      {
         cmsLog_debug("found host entry with mac=%s", (*hostEntry)->MACAddress);
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) hostEntry);
      }
   }

   /*
    * If the desired host entry was found, then ret will equal CMSRET_SUCCESS
    * iidStack will point to the desired entry, and hostEntry will be the
    * desired object.
    */

   return ret;
}



CmsRet getManageableDeviceEntry(const char *oui, const char *serialNum, const char *productClass, InstanceIdStack *iidStack, ManageableDeviceObject **manageableObj)
{
   UBOOL8 found=FALSE;
   CmsRet ret;

   INIT_INSTANCE_ID_STACK(iidStack);

   while (!found &&
          ((ret = cmsObj_getNextFlags(MDMOID_MANAGEABLE_DEVICE, iidStack, OGF_NO_VALUE_UPDATE, (void **) manageableObj)) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp((*manageableObj)->manufacturerOUI, oui) &&
          !cmsUtl_strcmp((*manageableObj)->serialNumber, serialNum) &&
          !cmsUtl_strcmp((*manageableObj)->productClass, productClass))
      {
         cmsLog_debug("found manageable entry with oui=%s serialNum=%s productClass=%s",
                      oui, serialNum, productClass);
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) manageableObj);
      }
   }

   /*
    * If the desired object was found, then ret will equal CMSRET_SUCCESS
    * iidStack will point to the desired entry, and manageableObj will be the
    * desired object.
    */

   return ret;
}

