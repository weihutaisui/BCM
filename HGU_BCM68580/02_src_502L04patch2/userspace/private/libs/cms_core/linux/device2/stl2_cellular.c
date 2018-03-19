/***********************************************************************
 *
 *  Copyright (c) 2006-2011  Broadcom Corporation
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

#ifdef DMP_CELLULARINTERFACEBASE_1


#include "odl.h"
#include "cms_core.h"
#include "cms_util.h"
#include "cms_msg_cellular.h"
#include "rcl.h"
#include "rut_util.h"
#include "rut_wan.h"
#include "rut2_cellular.h"


/*!\file stl2_cellular.c
 * \brief This file contains cellular related functions.
 *
 */


CmsRet stl_dev2CellularObject(_Dev2CellularObject *obj, const InstanceIdStack *iidStack __attribute((unused)))
{
#ifdef DMP_CELLULARINTERFACEEXTENDED_1
   char buf[sizeof(CmsMsgHeader) + BUFLEN_16]={0};
   CmsMsgHeader *replyBuf = (CmsMsgHeader *) buf;
   CmsMsgHeader msg = EMPTY_MSG_HEADER;
   CmsRet ret;
   
   cmsLog_debug("Enter");

   msg.type = CMS_MSG_CELLULARAPP_GET_REQUEST;
   msg.src = mdmLibCtx.eid;
   msg.flags_request = 1;
   msg.dataLength = 0;
   msg.wordData = DEVCELL_ROAMINGSTATUS;
   msg.dst = EID_CELLULAR_APP;

   if ((ret = cmsMsg_sendAndGetReplyBufWithTimeout(mdmLibCtx.msgHandle, &msg, &replyBuf, MSECS_IN_SEC)) == CMSRET_SUCCESS )
   {
      cmsLog_debug("roamingStatus=%s", (char *)(replyBuf + 1));
      CMSMEM_REPLACE_STRING_FLAGS(obj->roamingStatus, (char *)(replyBuf + 1), mdmLibCtx.allocFlags);
   }
   else
   {
      cmsLog_error("send cms msg Get DEVCELL_ROAMINGSTATUS error, ret=%d", ret);
   }
#endif   
   return CMSRET_SUCCESS;
}

CmsRet stl_dev2CellularInterfaceObject(_Dev2CellularInterfaceObject *obj, const InstanceIdStack *iidStack __attribute((unused)))
{
   cmsLog_debug("Enter");

   char buf[sizeof(CmsMsgHeader) + BUFLEN_256]={0};
   CmsMsgHeader *replyBuf = (CmsMsgHeader *) buf;
   CmsMsgHeader msg = EMPTY_MSG_HEADER;
   CmsRet ret;
   
   cmsLog_debug("Enter");

   if (cmsNet_isInterfaceExist("lte0"))
   {
      CMSMEM_REPLACE_STRING_FLAGS(obj->status, MDMVS_UP, mdmLibCtx.allocFlags);
   }
   else
   {
      CMSMEM_REPLACE_STRING_FLAGS(obj->status, MDMVS_DOWN, mdmLibCtx.allocFlags);
   }

   msg.type = CMS_MSG_CELLULARAPP_GET_REQUEST;
   msg.src = mdmLibCtx.eid;
   msg.flags_request = 1;
   msg.dataLength = 0;
   msg.dst = EID_CELLULAR_APP;

#ifdef DMP_CELLULARINTERFACEEXTENDED_1
   msg.wordData = DEVCELL_INT_IMEI;
   if ((ret = cmsMsg_sendAndGetReplyBufWithTimeout(mdmLibCtx.msgHandle, &msg, &replyBuf, MSECS_IN_SEC)) == CMSRET_SUCCESS )
   {
      cmsLog_debug("IMEI=%s", (char *)(replyBuf + 1));
      CMSMEM_REPLACE_STRING_FLAGS(obj->IMEI, (char *)(replyBuf + 1), mdmLibCtx.allocFlags);
   }
   else
   {
      cmsLog_error("send cms msg Get DEVCELL_INT_IMEI error, ret=%d", ret);
   }
#endif

   msg.wordData = DEVCELL_INT_CURRACCESSTECH;
   if ((ret = cmsMsg_sendAndGetReplyBufWithTimeout(mdmLibCtx.msgHandle, &msg, &replyBuf, MSECS_IN_SEC)) == CMSRET_SUCCESS )
   {
      cmsLog_debug("current access technology=%s", (char *)(replyBuf + 1));
      CMSMEM_REPLACE_STRING_FLAGS(obj->currentAccessTechnology, (char *)(replyBuf + 1), mdmLibCtx.allocFlags);
   }
   else
   {
      cmsLog_error("send cms msg Get DEVCELL_INT_CURRACCESSTECH error, ret=%d", ret);
   }

   msg.wordData = DEVCELL_INT_NETWORKINUSE;
   if ((ret = cmsMsg_sendAndGetReplyBufWithTimeout(mdmLibCtx.msgHandle, &msg, &replyBuf, MSECS_IN_SEC)) == CMSRET_SUCCESS )
   {
      cmsLog_debug("network in use=%s", (char *)(replyBuf + 1));
      CMSMEM_REPLACE_STRING_FLAGS(obj->networkInUse, (char *)(replyBuf + 1), mdmLibCtx.allocFlags);
   }
   else
   {
      cmsLog_error("send cms msg Get DEVCELL_INT_NETWORKINUSE error, ret=%d", ret);
   }

   msg.wordData = DEVCELL_INT_RSSI;
   if ((ret = cmsMsg_sendAndGetReplyBufWithTimeout(mdmLibCtx.msgHandle, &msg, &replyBuf, MSECS_IN_SEC)) == CMSRET_SUCCESS )
   {
      cmsLog_debug("RSSI=%d", *((SINT32*)(replyBuf + 1)));
      obj->RSSI = *((SINT32 *)(replyBuf + 1));
   }
   else
   {
      cmsLog_error("send cms msg Get DEVCELL_INT_RSSI error, ret=%d", ret);
   }


   if (cmsUtl_strstr(obj->currentAccessTechnology, "LTE"))
   {
      obj->upstreamMaxBitRate = 51200;
      obj->downstreamMaxBitRate = 307200;
   }
   else if (cmsUtl_strstr(obj->currentAccessTechnology, "GPRS") || cmsUtl_strstr(obj->currentAccessTechnology, "EDGE"))
   {
      obj->upstreamMaxBitRate = 236;
      obj->downstreamMaxBitRate = 296;
   }
   else if (cmsUtl_strstr(obj->currentAccessTechnology, "UMTS"))
   {
      obj->upstreamMaxBitRate = 11776;
      obj->downstreamMaxBitRate = 43008;
   }
   else //unknown
   {
      obj->upstreamMaxBitRate = 0;
      obj->downstreamMaxBitRate = 0;
   }
   
   return CMSRET_SUCCESS;
}
	

CmsRet stl_dev2CellularInterfaceUsimObject(_Dev2CellularInterfaceUsimObject *obj, const InstanceIdStack *iidStack __attribute((unused)))
{
   char buf[sizeof(CmsMsgHeader) + BUFLEN_16]={0};
   CmsMsgHeader *replyBuf = (CmsMsgHeader *) buf;
   CmsMsgHeader msg = EMPTY_MSG_HEADER;
   CmsRet ret;
   
   cmsLog_debug("Enter");

   msg.type = CMS_MSG_CELLULARAPP_GET_REQUEST;
   msg.src = mdmLibCtx.eid;
   msg.flags_request = 1;
   msg.dataLength = 0;
   msg.dst = EID_CELLULAR_APP;

   msg.wordData = DEVCELL_INT_USIMSTATUS;
   if ((ret = cmsMsg_sendAndGetReplyBufWithTimeout(mdmLibCtx.msgHandle, &msg, &replyBuf, MSECS_IN_SEC)) == CMSRET_SUCCESS )
   {
      cmsLog_debug("sim status=%s", (char *)(replyBuf + 1));
      CMSMEM_REPLACE_STRING_FLAGS(obj->status, (char *)(replyBuf + 1), mdmLibCtx.allocFlags);
   }
   else
   {
      cmsLog_error("send cms msg Get DEVCELL_INT_USIMSTATUS error, ret=%d", ret);
   }
   
#ifdef DMP_CELLULARINTERFACEEXTENDED_1
   msg.wordData = DEVCELL_INT_USIMIMSI;
   if ((ret = cmsMsg_sendAndGetReplyBufWithTimeout(mdmLibCtx.msgHandle, &msg, &replyBuf, MSECS_IN_SEC)) == CMSRET_SUCCESS )
   {
      cmsLog_debug("sim IMSI=%s", (char *)(replyBuf + 1));
      CMSMEM_REPLACE_STRING_FLAGS(obj->IMSI, (char *)(replyBuf + 1), mdmLibCtx.allocFlags);
   }
   else
   {
      cmsLog_error("send cms msg Get DEVCELL_INT_USIMIMSI error, ret=%d", ret);
   }
   
   msg.wordData = DEVCELL_INT_USIMICCID;
   if ((ret = cmsMsg_sendAndGetReplyBufWithTimeout(mdmLibCtx.msgHandle, &msg, &replyBuf, MSECS_IN_SEC)) == CMSRET_SUCCESS )
   {
      cmsLog_debug("sim ICCID=%s", (char *)(replyBuf + 1));
      CMSMEM_REPLACE_STRING_FLAGS(obj->ICCID, (char *)(replyBuf + 1), mdmLibCtx.allocFlags);
   }
   else
   {
      cmsLog_error("send cms msg Get DEVCELL_INT_USIMICCID error, ret=%d", ret);
   }
#endif

   msg.wordData = DEVCELL_INT_USIMMSISDN;
   if ((ret = cmsMsg_sendAndGetReplyBufWithTimeout(mdmLibCtx.msgHandle, &msg, &replyBuf, MSECS_IN_SEC)) == CMSRET_SUCCESS )
   {
      cmsLog_debug("sim MSISDN=%s", (char *)(replyBuf + 1));
      CMSMEM_REPLACE_STRING_FLAGS(obj->MSISDN, (char *)(replyBuf + 1), mdmLibCtx.allocFlags);
   }
   else
   {
      cmsLog_error("send cms msg Get DEVCELL_INT_USIMMSISDN error, ret=%d", ret);
   }
   return CMSRET_SUCCESS;
}
	

#ifdef DMP_CELLULARINTERFACESTATS_1
CmsRet stl_dev2CellularInterfaceStatsObject(_Dev2CellularInterfaceStatsObject *obj, const InstanceIdStack *iidStack)
{
   cmsLog_debug("Enter");
   
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
#endif

CmsRet stl_dev2CellularAccessPointObject(_Dev2CellularAccessPointObject *obj, const InstanceIdStack *iidStack)
{
   cmsLog_debug("Enter");

   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_dev2CellularLinkObject(_Dev2CellularLinkObject *obj, const InstanceIdStack *iidStack)
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

#endif /* DMP_CELLULARINTERFACEBASE_1 */

#else
/* DMP_DEVICE2_BASELINE_1 is not defined */

#ifdef DMP_CELLULARINTERFACEBASE_1
#error "cellular objects incompatible with current Data Model mode(need DMP_DEVICE2_BASELINE_1), go to make menuconfig to fix"
#endif

#endif  /* DMP_DEVICE2_BASELINE_1 */

