/***********************************************************************
 *
 *  Copyright (c) 2006-2009  Broadcom Corporation
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


#ifdef DMP_DEVICE2_BASELINE_1

#ifdef DMP_CELLULARINTERFACEBASE_1

#include "odl.h"
#include "cms_core.h"
#include "cms_util.h"
#include "cms_msg_cellular.h"
#include "rcl.h"
#include "rut_util.h"
#include "rut2_util.h"
#include "rut_wan.h"
#include "rut2_cellular.h"


/*!\file rcl2_cellular.c
 * \brief This file contains cellular WAN related functions.
 *
 */
#ifdef LATER
static void modifyCellularIntfNumEntries(const InstanceIdStack *iidStack, SINT32 delta)
{
   rutUtil_modifyNumGeneric_dev2(MDMOID_DEV2_CELLULAR,
                                 MDMOID_DEV2_CELLULAR_INTERFACE, iidStack, delta);
}
#endif


static void modifyCellularApnNumEntries(const InstanceIdStack *iidStack, SINT32 delta)
{
   rutUtil_modifyNumGeneric_dev2(MDMOID_DEV2_CELLULAR,
                                 MDMOID_DEV2_CELLULAR_ACCESS_POINT, iidStack, delta);
}

CmsRet rcl_dev2CellularObject( _Dev2CellularObject *newObj,
                const _Dev2CellularObject *currObj __attribute((unused)),
                const InstanceIdStack *iidStack __attribute((unused)),
                char **errorParam __attribute((unused)),
                CmsRet *errorCode __attribute((unused)))
{
   cmsLog_debug("Enter: roamingEnabled=%d", newObj->roamingEnabled);
   
#ifdef DMP_CELLULARINTERFACEEXTENDED_1
   
#endif   

   return CMSRET_SUCCESS;
}
                

CmsRet rcl_dev2CellularInterfaceObject( _Dev2CellularInterfaceObject *newObj,
                const _Dev2CellularInterfaceObject *currObj,
                const InstanceIdStack *iidStack __attribute((unused)),
                char **errorParam __attribute((unused)),
                CmsRet *errorCode __attribute((unused)))
{
   char buf[sizeof(CmsMsgHeader) + BUFLEN_16]={0};
   CmsMsgHeader *msg = (CmsMsgHeader *) buf;
   CmsRet ret;
   
   cmsLog_debug("Enter");

   msg->type = CMS_MSG_CELLULARAPP_SET_REQUEST;
   msg->src = mdmLibCtx.eid;
   msg->dst = EID_CELLULAR_APP;
   msg->flags_request = 1;


   if (ADD_NEW(newObj, currObj) || newObj->enable != currObj->enable)   
   {
      msg->wordData = DEVCELL_INT_ENABLE;
      *((UBOOL8 *)(msg+1)) = newObj->enable;
      msg->dataLength = sizeof(CmsMsgHeader) + sizeof(UBOOL8);
	  
      if ((ret = cmsMsg_send(mdmLibCtx.msgHandle, msg)) == CMSRET_SUCCESS)
      {
         cmsLog_debug("sent cms msg Set DEVCELL_INT_ENABLE, enable=%d", newObj->enable);
      }
      else
      {
         cmsLog_error("send cms msg Set DEVCELL_INT_ENABLE error, ret=%d", ret);
      }
   }  
//Sarah: preferredAccessTechnology not supported in samos   
#if 0
   if (ADD_NEW(newObj, currObj) || cmsUtl_strcmp(newObj->preferredAccessTechnology, currObj->preferredAccessTechnology))   
   {
      msg->wordData = DEVCELL_INT_PREFERREDACCESSTECH;
      cmsUtl_strcpy((char *)(msg+1), newObj->preferredAccessTechnology);
      msg->dataLength = sizeof(CmsMsgHeader) + cmsUtl_strlen(newObj->preferredAccessTechnology);
	  
      if ((ret = cmsMsg_send(mdmLibCtx.msgHandle, msg)) == CMSRET_SUCCESS)
      {
         cmsLog_debug("sent cms msg Set DEVCELL_INT_PREFERREDACCESSTECH");
      }
      else
      {
         cmsLog_error("send cms msg Set DEVCELL_INT_PREFERREDACCESSTECH error, ret=%d", ret);
      }
   }   
#endif   
   return CMSRET_SUCCESS;
}
                

CmsRet rcl_dev2CellularInterfaceUsimObject( _Dev2CellularInterfaceUsimObject *newObj,
                const _Dev2CellularInterfaceUsimObject *currObj,
                const InstanceIdStack *iidStack __attribute((unused)),
                char **errorParam __attribute((unused)),
                CmsRet *errorCode __attribute((unused)))
{
   cmsLog_debug("Enter");
   
   return CMSRET_SUCCESS;
}

				
#ifdef DMP_CELLULARINTERFACESTATS_1
CmsRet rcl_dev2CellularInterfaceStatsObject( _Dev2CellularInterfaceStatsObject *newObj __attribute((unused)),
                const _Dev2CellularInterfaceStatsObject *currObj __attribute((unused)),
                const InstanceIdStack *iidStack __attribute((unused)),
                char **errorParam __attribute((unused)),
                CmsRet *errorCode __attribute((unused)))
{
   return CMSRET_SUCCESS;
}
#endif

CmsRet rcl_dev2CellularAccessPointObject( _Dev2CellularAccessPointObject *newObj,
                const _Dev2CellularAccessPointObject *currObj,
                const InstanceIdStack *iidStack __attribute((unused)),
                char **errorParam __attribute((unused)),
                CmsRet *errorCode __attribute((unused)))
{
   const Dev2CellularAccessPointObject *msgObj;
   char buf[sizeof(CmsMsgHeader) + sizeof(DevCellularAccessPointMsgBody)]={0};
   CmsMsgHeader *msg = (CmsMsgHeader *) buf;
   DevCellularAccessPointMsgBody *cellularApnMsg = (DevCellularAccessPointMsgBody *)(msg + 1);
   CmsRet ret;
   
   cmsLog_debug("Enter");

   if (ADD_NEW(newObj, currObj))
   {
      modifyCellularApnNumEntries(iidStack, 1);
   }

   if (DELETE_EXISTING(newObj, currObj))
   {
      modifyCellularApnNumEntries(iidStack, -1);
   }

   if ((ADD_NEW(newObj, currObj) && newObj->enable && !cmsUtl_strcmp(newObj->X_BROADCOM_COM_Type, CELLULAR_APN_TYPE_DEFAULT)) || 
       (DELETE_EXISTING(newObj, currObj) && currObj->enable && !cmsUtl_strcmp(currObj->X_BROADCOM_COM_Type, CELLULAR_APN_TYPE_DEFAULT))||
       (newObj != NULL && currObj != NULL && newObj->enable != currObj->enable && !cmsUtl_strcmp(newObj->X_BROADCOM_COM_Type, CELLULAR_APN_TYPE_DEFAULT)))   
   {
      msgObj = newObj ? newObj : currObj;

//Sarah: TODO: implement QDM function to support pure 181   
#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1
      if (!rutCellular_getIfnameByApnInstId(iidStack->instance[0], cellularApnMsg->ifname))
      {
         cmsLog_error("interface name not found");
         return CMSRET_INTERNAL_ERROR;
      }
#endif
	  
      /* Set rest msg parameter */	  
      msg->type = CMS_MSG_CELLULARAPP_SET_REQUEST;
      msg->src = mdmLibCtx.eid;
      msg->dst = EID_CELLULAR_APP;
      msg->flags_request = 1;
      msg->wordData = DEVCELL_ACCESSPOINTENABLE;

      cellularApnMsg->enable = newObj ? newObj->enable : FALSE;
      cellularApnMsg->ipProtocalMode = msgObj->X_BROADCOM_COM_IpVer;
      cmsUtl_strcpy(cellularApnMsg->APN, msgObj->APN);
      msg->dataLength = sizeof(buf);
	  
      if ((ret = cmsMsg_send(mdmLibCtx.msgHandle, msg)) == CMSRET_SUCCESS)
      {
         cmsLog_debug("sent cms msg Set DEVCELL_ACCESSPOINTENABLE, enable=%d, ifname=%s, APN=%s, ipProtocalMode=0x%x", 
                  cellularApnMsg->enable, cellularApnMsg->ifname, cellularApnMsg->APN, cellularApnMsg->ipProtocalMode);
      }
      else
      {
         cmsLog_error("send cms msg Set DEVCELL_ACCESSPOINTENABLE error, ret=%d", ret);
      }
   }   

   if (newObj && cmsUtl_strcmp(newObj->APN, CELLULAR_AUTO_SEL_APN_NAME) &&      //ignore default APN
       ((ENABLE_EXISTING(newObj, currObj) && 
        newObj->X_BROADCOM_COM_Id >= CELLULAR_APN_USER_BASE_ID) ||              //Re-enabled APN
       (ENABLE_NEW_OR_ENABLE_EXISTING(newObj, currObj) && newObj->X_BROADCOM_COM_Id == 0)))         //Newly add APN
   {
      if (newObj->X_BROADCOM_COM_Id == 0)   
      {
         newObj->X_BROADCOM_COM_Id = CELLULAR_APN_USER_BASE_ID + iidStack->instance[0];
      }

      cmsLog_debug("Add to telephony.db, id = %d", newObj->X_BROADCOM_COM_Id);
      rutCellularDB_addApn(newObj);
      if (!cmsUtl_strcmp(newObj->X_BROADCOM_COM_Type, CELLULAR_APN_TYPE_DEFAULT))
      {
         rutCellular_setPrefdApn(newObj->X_BROADCOM_COM_Id, NULL);
      }
   }
   else if (DELETE_OR_DISABLE_EXISTING(newObj, currObj) &&
            currObj->X_BROADCOM_COM_Id >= CELLULAR_APN_USER_BASE_ID)
   {
      cmsLog_debug("Delete from telephony.db, id = %d", currObj->X_BROADCOM_COM_Id);
      rutCellularDB_DelApn(currObj->X_BROADCOM_COM_Id);
      if (!cmsUtl_strcmp(currObj->X_BROADCOM_COM_Type, CELLULAR_APN_TYPE_DEFAULT))
      {
         rutCellular_setPrefdApn(0, iidStack);
      }
   }

   cmsLog_debug("Exit");
   
   return CMSRET_SUCCESS;
}

CmsRet rcl_dev2CellularLinkObject( _Dev2CellularLinkObject *newObj,
                const _Dev2CellularLinkObject *currObj __attribute((unused)),
                const InstanceIdStack *iidStack __attribute((unused)),
                char **errorParam __attribute((unused)),
                CmsRet *errorCode __attribute((unused)))
{
   CmsRet ret;
   
   if ((newObj != NULL) &&
       IS_EMPTY_STRING(newObj->name) &&
       !IS_EMPTY_STRING(newObj->lowerLayers))
   {
      char ifName[CMS_IFNAME_LENGTH]={0};

      cmsLog_debug("newObj->lowerLayers = %s", newObj->lowerLayers);

      /* get the ifName of lowerlayer interfaces.
      * For hybrid mode where ethernet.link points to
      * ether WANIP/WANPPP objects instead of TR181 layer2 objects
      */
      if (!cmsMdm_isDataModelDevice2())
      {
         if ((ret = rut_fullPathToIntfname(newObj->lowerLayers, ifName)) != CMSRET_SUCCESS)
         {
            cmsLog_error("qdmIntf_getIntfnameFromFullPathLocked failed. ret %d", ret);
            return ret;
         }
      }

      CMSMEM_REPLACE_STRING_FLAGS(newObj->name, ifName, mdmLibCtx.allocFlags);
   }
   
   return CMSRET_SUCCESS;
}

#endif /* DMP_CELLULARINTERFACEBASE_1 */

#else
/* DMP_DEVICE2_BASELINE_1 is not defined */

#ifdef DMP_CELLULARINTERFACEBASE_1
#error "cellular objects incompatible with current Data Model mode(need DMP_DEVICE2_BASELINE_1), go to make menuconfig to fix"
#endif

#endif  /* DMP_DEVICE2_BASELINE_1 */
