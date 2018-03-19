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


#ifdef DMP_DEVICE2_BASELINE_1

#if defined(DMP_DEVICE2_HOMEPLUG_1)

#include "odl.h"
#include "cms_core.h"
#include "cms_util.h"
#include "cms_msg_homeplug.h"
#include "rcl.h"
#include "rut_util.h"
#include "rut2_homeplug.h"
#include "rut2_dhcpv4.h"

static int hpavDriverInitDone = 0; /* HomePlug device control and driver API has been initialized */

/* forward function declarations */
CmsRet rutHomeplug_sendAliasMsg(const char* alias);
CmsRet rutHomeplug_sendPasswordMsg(const char* alias);
CmsRet rutHomeplug_sendLogicalNetworkMsg(const char* alias);
#ifdef DMP_DEVICE2_HOMEPLUG_DEBUG_1
CmsRet rutHomeplug_sendDiagPeriodicIntervalMsg(UINT32 X_BROADCOM_COM_DiagPeriodicInterval);
#endif /* #ifdef DMP_DEVICE2_HOMEPLUG_DEBUG_1 */

void rutHomeplug_checkAPIStatus()
{
   /* Access to HomePlug device will be required at this point, driver API 
      must be initialized before access it. */
   if (hpavDriverInitDone == 0)
   {
     HomePlugDiscoveredLocalPLCDevice discovered;

     devCtrl_homeplugIni();

     /* discover local PLC device */
     if ( devCtrl_homeplugDiscoverLocalPLCDevice(2, &discovered) != 0 )
     {
        /* Terminate device control API */
        devCtrl_homeplugClose();
     }
     else
     {
        devCtrl_homeplugSetLocalPlcDeviceMac(&discovered.MACAddress[0]);
        hpavDriverInitDone = 1;
     }
   } 
}

CmsRet rutHomeplug_setInterfaceStatus(BCMHPAV_STATUS status)
{
  CmsRet ret = CMSRET_SUCCESS;

  rutHomeplug_checkAPIStatus();

  if (devCtl_homeplugSetInterfaceStatus(status) == -1)
  {  
    ret = CMSRET_INTERNAL_ERROR;
  }

  return ret;
}

CmsRet rutHomeplug_setInterfaceLogicalNetwork(const _HomePlugInterfaceObject *newObj,
                                              const _HomePlugInterfaceObject *currObj)
{
   CmsRet ret = CMSRET_SUCCESS;

   rutHomeplug_checkAPIStatus();

   if (cmsUtl_strcmp(newObj->logicalNetwork, currObj->logicalNetwork))
   {
      ret = rutHomeplug_sendLogicalNetworkMsg(newObj->logicalNetwork);
   }

   return ret;
}

CmsRet rutHomeplug_setInterfaceNetworkPassword(_HomePlugInterfaceObject *newObj,
                                               const _HomePlugInterfaceObject *currObj)
{
   CmsRet ret = CMSRET_SUCCESS;

   rutHomeplug_checkAPIStatus();

   if (cmsUtl_strcmp(newObj->networkPassword, currObj->networkPassword))
   {
      ret = rutHomeplug_sendPasswordMsg(newObj->networkPassword);
      if (ret == CMSRET_SUCCESS)
      {
         /* When read, this parameter returns an empty string, regardless of
            the actual value. */
         CMSMEM_REPLACE_STRING_FLAGS(newObj->networkPassword, "", mdmLibCtx.allocFlags);
      }
  }

   return ret;
}

CmsRet rutHomeplug_setInterfaceAlias(const _HomePlugInterfaceObject *newObj,
                                     const _HomePlugInterfaceObject *currObj)
{
   CmsRet ret = CMSRET_SUCCESS;

   rutHomeplug_checkAPIStatus();

   if (cmsUtl_strcmp(newObj->alias, currObj->alias))
   {
      ret = rutHomeplug_sendAliasMsg(newObj->alias);
   }

   return ret;
}

#ifdef DMP_DEVICE2_HOMEPLUG_DEBUG_1
CmsRet rutHomeplug_setInterfaceDiagPeriodicInterval(const _HomePlugInterfaceObject *newObj,
                                                    const _HomePlugInterfaceObject *currObj)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (newObj->X_BROADCOM_COM_DiagPeriodicInterval != currObj->X_BROADCOM_COM_DiagPeriodicInterval)
   {
      ret = rutHomeplug_sendDiagPeriodicIntervalMsg(newObj->X_BROADCOM_COM_DiagPeriodicInterval);
   }

   return ret;
}
#endif /* DMP_DEVICE2_HOMEPLUG_DEBUG_1 */

UBOOL8 rutHomeplug_isValuesChanged(const _HomePlugInterfaceObject *newObj,
                                   const _HomePlugInterfaceObject *currObj)
{
   if (cmsUtl_strcmp(newObj->alias, currObj->alias) 
       || cmsUtl_strcmp(newObj->lowerLayers, currObj->lowerLayers) 
       || cmsUtl_strcmp(newObj->logicalNetwork, currObj->logicalNetwork) 
       || cmsUtl_strcmp(newObj->networkPassword, currObj->networkPassword) 
#ifdef DMP_DEVICE2_HOMEPLUG_DEBUG_1
       || (newObj->X_BROADCOM_COM_DiagPeriodicInterval != currObj->X_BROADCOM_COM_DiagPeriodicInterval))
#else 
       )
#endif /* DMP_DEVICE2_HOMEPLUG_DEBUG_1 */
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}


/* This function can be merged with rut_modifyNumGeneric,
 * but since this is a device2, does it need to be in rux?
 * but rux should not know about objects, does that include OID's?
 */
void rutHomeplug_modifyNumAssocHomeplugDevices(const InstanceIdStack *iidStack, SINT32 delta)
{
   HomePlugInterfaceObject *hplugIntfObj=NULL;
   InstanceIdStack ancestorIidStack = *iidStack;
   CmsRet ret;

   cmsLog_debug("delta=%d", delta);

   ret = cmsObj_getAncestor(MDMOID_HOME_PLUG_INTERFACE,
                         MDMOID_HOME_PLUG_INTERFACE_ASSOCIATED_DEVICE,
                         &ancestorIidStack, (void **) &hplugIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get ancestor intf object, ret=%d", ret);
      return;
   }

   if (delta < 0)
   {
      UINT32 decreaseBy = (UINT32) (delta * -1);
      if (decreaseBy > hplugIntfObj->associatedDeviceNumberOfEntries)
      {
         cmsLog_error("underflow detected for home plug num assoc devices!");
         ret = CMSRET_INTERNAL_ERROR;
      }
   }
   else if (delta > 0)
   {
      UINT32 result = hplugIntfObj->associatedDeviceNumberOfEntries + delta;
      if (result < hplugIntfObj->associatedDeviceNumberOfEntries)
      {
         cmsLog_error("overflow detected for home plug num assoc devices!");
         ret = CMSRET_INTERNAL_ERROR;
      }
   }

   if (ret == CMSRET_SUCCESS)
   {
      hplugIntfObj->associatedDeviceNumberOfEntries += delta;

      ret = cmsObj_set(hplugIntfObj, &ancestorIidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of home plug intf obj failed, ret=%d", ret);
      }
   }

   cmsObj_free((void **)&hplugIntfObj);

   return;
}

/*
 * Fill in homeplug generic message data and send the message to homeplugd 
 * through CMS messaging.
 * Pre: CmsMsgHeader already contains specific data size and data content.
 */
CmsRet rutHomeplug_sendMsgToHomeplugd(CmsMsgHeader* msg, CmsMsgType msgType)
{
   CmsRet ret = CMSRET_SUCCESS;
   void* msgHandle = mdmLibCtx.msgHandle;
   const CmsEntityId myEid = mdmLibCtx.eid;

   if (msg == NULL)
   {
      cmsLog_error("cannot send null cms message type = %d, ret=%d", msgType);
      ret = CMSRET_INTERNAL_ERROR;
   }
   else
   {
      /* fill in msg header */
      msg->type = msgType;
      msg->src = myEid;
      msg->dst = EID_HOMEPLUGD;
      msg->flags_event = 1;

      /*
       * We do not receive a response from homeplugd by the moment, just send the 
       * message.
       */   
      if ((ret = cmsMsg_send(msgHandle, msg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("send of cms message type %d failed, ret=%d", msgType, ret);
      }
      else
      {
         cmsLog_debug("cms message type %d sent", msgType);
      }
   }

   return ret;
}

/*
 * Fill data message for homeplug network password and send the message.
 */
CmsRet rutHomeplug_sendPasswordMsg(const char* password)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (password == NULL)
   {
      cmsLog_error("cannot send null password cms message");
      ret = CMSRET_INTERNAL_ERROR;
   }
   else
   {
      /* create the message to be sent */
      char buf[sizeof(CmsMsgHeader) + sizeof(HomePlugPasswordSetMsgBody)] = {0};
      CmsMsgHeader* msgHdr = (CmsMsgHeader*) buf;
      HomePlugPasswordSetMsgBody* msgData =  
         (HomePlugPasswordSetMsgBody*) &(buf[sizeof(CmsMsgHeader)]);
      msgHdr->dataLength = sizeof(HomePlugPasswordSetMsgBody);

      /* copy the network password data */
      cmsUtl_strncpy(msgData->password, password, CMS_MSG_HOMEPLUG_PASSWORD_LENGTH);

      /* send the message */
      ret = rutHomeplug_sendMsgToHomeplugd(msgHdr, CMS_MSG_HOMEPLUG_PASSWORD_SET);
   }

   return ret;
}

/*
 * Fill data message homeplug alias and send the message.
 */
CmsRet rutHomeplug_sendAliasMsg(const char* alias)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (alias == NULL)
   {
      cmsLog_error("cannot send null alias cms message");
      ret = CMSRET_INTERNAL_ERROR;
   }
   else
   {
      /* create the message to be sent */
      char buf[sizeof(CmsMsgHeader) + sizeof(HomePlugAliasSetMsgBody)] = {0};
      CmsMsgHeader* msgHdr = (CmsMsgHeader*) buf;
      HomePlugAliasSetMsgBody* msgData =  
         (HomePlugAliasSetMsgBody*) &(buf[sizeof(CmsMsgHeader)]);
      msgHdr->dataLength = sizeof(HomePlugAliasSetMsgBody);

      /* copy the alias data */
      cmsUtl_strncpy(msgData->alias, alias, CMS_MSG_HOMEPLUG_ALIAS_LENGTH);

      /* send the message */
      ret = rutHomeplug_sendMsgToHomeplugd(msgHdr, CMS_MSG_HOMEPLUG_ALIAS_SET);
   }

   return ret;
}

/*
 * Fill data message logicalNetwork and send the message.
 */
CmsRet rutHomeplug_sendLogicalNetworkMsg(const char* logicalNetwork)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (logicalNetwork == NULL)
   {
      cmsLog_error("cannot send null logicalNetwork cms message");
      ret = CMSRET_INTERNAL_ERROR;
   }
   else
   {
      /* create the message to be sent */
      char buf[sizeof(CmsMsgHeader) + sizeof(HomePlugLogicalNetworkSetMsgBody)] = {0};
      CmsMsgHeader* msgHdr = (CmsMsgHeader*) buf;
      HomePlugLogicalNetworkSetMsgBody* msgData =  
         (HomePlugLogicalNetworkSetMsgBody*) &(buf[sizeof(CmsMsgHeader)]);
      msgHdr->dataLength = sizeof(HomePlugLogicalNetworkSetMsgBody);

      /* copy the logicalNetwork data */
      cmsUtl_strncpy(msgData->logicalNetwork, logicalNetwork, CMS_MSG_HOMEPLUG_LOGICALNET_LENGTH);

      /* send the message */
      ret = rutHomeplug_sendMsgToHomeplugd(msgHdr, CMS_MSG_HOMEPLUG_LOGICALNET_SET);
   }

   return ret;
}

#ifdef DMP_DEVICE2_HOMEPLUG_DEBUG_1
/*
 * Fill data message PeriodicInterval and send the message.
 */
CmsRet rutHomeplug_sendDiagPeriodicIntervalMsg(UINT32 X_BROADCOM_COM_DiagPeriodicInterval)
{
   CmsRet ret = CMSRET_SUCCESS;

   /* create the message to be sent */
   char buf[sizeof(CmsMsgHeader) + sizeof(HomePlugDiagPeriodicIntervalSetMsgBody)] = {0};
   CmsMsgHeader* msgHdr = (CmsMsgHeader*) buf;
   HomePlugDiagPeriodicIntervalSetMsgBody* msgData = 
      (HomePlugDiagPeriodicIntervalSetMsgBody*) &(buf[sizeof(CmsMsgHeader)]);
   msgHdr->dataLength = sizeof(HomePlugDiagPeriodicIntervalSetMsgBody);

   /* copy the msg data */
   msgData->X_BROADCOM_COM_DiagPeriodicInterval = X_BROADCOM_COM_DiagPeriodicInterval;

   /* send the message */
   ret = rutHomeplug_sendMsgToHomeplugd(msgHdr, CMS_MSG_HOMEPLUG_DIAGINTERVAL_SET);

   return ret;
}
#endif /* DMP_DEVICE2_HOMEPLUG_DEBUG_1 */

#endif  /* DMP_DEVICE2_HOMEPLUG_1 */

#endif  /* DMP_DEVICE2_BASELINE_1 */
