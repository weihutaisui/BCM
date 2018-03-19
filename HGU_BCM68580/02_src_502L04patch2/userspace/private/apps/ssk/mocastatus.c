/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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

#ifdef SUPPORT_MOCA


#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_msg.h"

#include <fcntl.h>

#include "ssk.h"
#include "ssk_moca.h"
#include "devctl_moca.h"

extern SINT32 mocaMonitorFd; /* defined in ssk.c */


void setLastOperFreq(UINT32 lastOperFreq);

// example code
//static void updateLinkStatus(void *userarg, MoCA_CALLBACK_DATA * pData) 
//{
//   cmsLog_debug("%s: MoCA LINK STATUS message, link is %s\n", __FUNCTION__, pData->data.linkStatus ? "UP" : "DOWN");
//}


#ifndef BRCM_MOCA_DAEMON
static void updateLof (void *userarg, MoCA_CALLBACK_DATA * pData) 
{
   static UINT32 lastOperFreq=0;
   CmsRet nRet;

   cmsLog_debug("%s: MoCA LOF message, %lu MHz\n", __FUNCTION__, pData->data.lof);

   if (lastOperFreq != pData->data.lof) {
      lastOperFreq = pData->data.lof ;
#if 1
      nRet = cmsPsp_set ("MoCALOF", (char *) &lastOperFreq, sizeof (UINT32)) ;
      if (nRet != CMSRET_SUCCESS) {
         cmsLog_error("%s: cmsPsp_set (MoCALOF) ret %lu\n", __FUNCTION__, nRet );
      }
#else
      setLastOperFreq(lastOperFreq);
#endif
   }  
}


void initMocaMonitorFd()
{
   const char *mocaMonitorFile = "/dev/bmoca0";
   CmsRet      nRet;

   mocaMonitorFd = open(mocaMonitorFile, O_RDONLY);

   if (mocaMonitorFd < 0)
   {
      cmsLog_error("Could not open %s.", mocaMonitorFile);
   }
   else 
   {
      cmsLog_debug("MoCA Monitor file opened successfully: 0x%x\n", mocaMonitorFd);
   }

// example code
//   nRet = MoCACtl_RegisterCallback(MoCA_CALLBACK_EVENT_LINK_STATUS, updateLinkStatus, NULL);
//   if (nRet != CMSRET_SUCCESS) {
//      cmsLog_error("%s: MoCACtl_RegisterCallback (LINK STATUS) ret %lu\n", __FUNCTION__, nRet );
//   }

   nRet = MoCACtl_RegisterCallback(MoCA_CALLBACK_EVENT_LOF, updateLof, NULL);
   if (nRet != CMSRET_SUCCESS) {
      cmsLog_error("%s: MoCACtl_RegisterCallback (LOF) ret %lu\n", __FUNCTION__, nRet );
   }
}

void cleanupMocaMonitorFd()
{
   if (mocaMonitorFd != CMS_INVALID_FD)
   {
      close(mocaMonitorFd);
   }
}

void processMocaMonitor()
{
   CmsRet nRet = CMSRET_SUCCESS;
   static UINT32 moca_msg[4];
   UINT32 bytesRead;
   UINT32 msgId;
   UINT32 msgLen;
   MoCA_CALLBACK_DATA cbData;

   /*
    * The moca driver has indicated that it has an event that needs to be processed.
    * So do an ioctl or something to get the event message from the driver.
    */

   bytesRead = read( mocaMonitorFd, moca_msg, sizeof(moca_msg) );

   if (bytesRead != 0) {
      msgId  = moca_msg[0] >> 16;
      msgLen = moca_msg[0] & 0xFFFF; /* length is in bytes */

      switch (msgId) {
         /* Link Status */
         case 0x6003:
            if (msgLen == 4) {
               cbData.data.linkStatus = moca_msg[1];
               nRet = MoCACtl_DispatchCallback(MoCA_CALLBACK_EVENT_LINK_STATUS, &cbData);
            }
            else {
               cmsLog_error("%s: invalid LINK STATUS message length %lu\n",
                  __FUNCTION__, msgLen );
            }            
            break;

         /* Last operating frequency */
         case 0x7001:
            if (msgLen == 4) {
               cbData.data.lof = moca_msg[1];
               nRet = MoCACtl_DispatchCallback(MoCA_CALLBACK_EVENT_LOF, &cbData);
            }
            else {
               cmsLog_error("%s: invalid LOF message length %lu\n",
                  __FUNCTION__, msgLen );
            }
            break;

         default:
            cmsLog_notice("%s: Unrecognized MoCA message 0x%x\n", __FUNCTION__, msgId);
            break;
      }
   }

   if (nRet != CMSRET_SUCCESS) {
      cmsLog_error("%s: ret %lu\n", __FUNCTION__, nRet );
   }
}

#endif  /* ifndef MOCA_DAEMON */


#ifdef DMP_X_ITU_ORG_GPON_1
void setLastOperFreq(UINT32 lastOperFreq)
{
   PptpMocaUniObject *mocaObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;


   if ((ret = cmsObj_getNext(MDMOID_PPTP_MOCA_UNI, &iidStack, (void **) &mocaObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not find PPTP_MOCA_UNI object, ret=%d", ret);
      return;
   }

   /* assume there is exactly one PPTP_MOCA_UNI object. */

   /*
    * now set the frequency.
    * ssk is allowed to write to this parameter even though it is read-only.
    */
   
   if (mocaObj->lastOperationalFrequency != lastOperFreq) {
      mocaObj->lastOperationalFrequency = lastOperFreq;

      if ((ret = cmsObj_set(mocaObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not set PPTP_MOCA_UNI object, ret=%d", ret);
      }
   }
   

   cmsObj_free((void **) &mocaObj);
      
   return;
}
#endif


#ifdef X_BROADCOM_COM_MOCA_1

void setLastOperFreq(UINT32 lastOperFreq)
{
   cmsLog_error("cannot set freq=%d because no broadcom BHR and ONT+RG moca objects have been defined", lastOperFreq);
}

#endif




#ifdef BRCM_MOCA_DAEMON

void processMoCAWriteLof(CmsMsgHeader *pMsg)
{
   CmsRet              cmsRet;
   void              * obj = NULL;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   char              * pIfName  = (char *)(pMsg + 1);

   cmsLog_debug("ifName = %s, lof %d", pIfName, pMsg->wordData);

   if( (cmsRet = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
   {
      cmsLog_error("could not acquire lock, ret=%d", cmsRet);
      return;
   }

   cmsRet = dalMoca_getMocaObjByIntfName( pIfName, &iidStack, &obj);
   if ( CMSRET_SUCCESS != cmsRet )
   {
      cmsLog_error("Could not find MoCA object for %s", pIfName);
      cmsLck_releaseLock();
      return;
   }

   sskMoca_writeLof(obj, pMsg->wordData);

   cmsRet = cmsObj_set(obj, &iidStack);
   if ( CMSRET_SUCCESS != cmsRet )
   {
      cmsLog_error("Could not set MoCA object for %s", pIfName);
   }

   cmsObj_free(&obj);
   cmsMgm_saveConfigToFlash();

   cmsLck_releaseLock();
   
}

void processMoCAReadLof(CmsMsgHeader *pMsg)
{
   CmsRet              cmsRet;
   void              * obj = NULL;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   char              * pIfName  = (char *)(pMsg + 1);

   if( (cmsRet = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
   {
      cmsLog_error("could not acquire lock, ret=%d", cmsRet);
      return;
   }

   pMsg->src            = EID_SSK;
   pMsg->dst            = pMsg->src;
   pMsg->flags_request  = 0;
   pMsg->flags_response = 1;

   cmsRet = dalMoca_getMocaObjByIntfName( pIfName, &iidStack, &obj);

   if ( CMSRET_SUCCESS != cmsRet )
   {
      cmsLog_error("Could not find MoCA object for %s", pIfName);
      pMsg->wordData = 0xffffffff;  // send back -1 to indicate error
   }
   else
   {
      pMsg->wordData = sskMoca_readLof(obj);

      cmsLog_debug("ifName = %s, lof %d\n", pIfName, pMsg->wordData);

      cmsObj_free(&obj);
   }

   cmsLck_releaseLock();

   cmsRet = cmsMsg_send(msgHandle, pMsg);
   if (CMSRET_SUCCESS != cmsRet )
   {
      cmsLog_error("Could not send message for %s", pIfName);
   }
}


void processMoCAWriteMRNonDefSeqNum(CmsMsgHeader *pMsg)
{
   CmsRet              cmsRet;
   void              * obj = NULL;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   char              * pIfName  = (char *)(pMsg + 1);

   cmsLog_debug("ifName = %s, mrNonDefSeqNum %d", pIfName, pMsg->wordData);

   if( (cmsRet = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
   {
      cmsLog_error("could not acquire lock, ret=%d", cmsRet);
      return;
   }

   cmsRet = dalMoca_getMocaObjByIntfName( pIfName, &iidStack, &obj);
   if ( CMSRET_SUCCESS != cmsRet )
   {
      cmsLog_error("Could not find MoCA object for %s", pIfName);
      cmsLck_releaseLock();
      return;
   }

   sskMoca_writeNonDefSeqNum(obj, pMsg->wordData);

   cmsRet = cmsObj_set(obj, &iidStack);
   if ( CMSRET_SUCCESS != cmsRet )
   {
      cmsLog_error("Could not set MoCA object for %s", pIfName);
   }

   cmsObj_free(&obj);
   cmsMgm_saveConfigToFlash();

   cmsLck_releaseLock();
   
}

void processMoCAReadMRNonDefSeqNum(CmsMsgHeader *pMsg)
{
   CmsRet              cmsRet;
   void              * obj = NULL;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   char              * pIfName  = (char *)(pMsg + 1);

   if( (cmsRet = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
   {
      cmsLog_error("could not acquire lock, ret=%d", cmsRet);
      return;
   }

   pMsg->src            = EID_SSK;
   pMsg->dst            = pMsg->src;
   pMsg->flags_request  = 0;
   pMsg->flags_response = 1;

   cmsRet = dalMoca_getMocaObjByIntfName( pIfName, &iidStack, &obj);

   if ( CMSRET_SUCCESS != cmsRet )
   {
      cmsLog_error("Could not find MoCA object for %s", pIfName);
      pMsg->wordData = 0xffffffff;  // send back -1 to indicate error
   }
   else
   {
      pMsg->wordData = sskMoca_readNonDefSeqNum(obj);

      cmsLog_debug("ifName = %s, mrNonDefSeqNum 0x%x\n", pIfName, pMsg->wordData);

      cmsObj_free(&obj);
   }

   cmsLck_releaseLock();

   cmsRet = cmsMsg_send(msgHandle, pMsg);
   if (CMSRET_SUCCESS != cmsRet )
   {
      cmsLog_error("Could not send message for %s", pIfName);
   }
}

void processMoCANotification(CmsMsgHeader *pMsg)
{
   CmsRet              cmsRet;
   void              * obj = NULL;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   char              * pIfName  = (char *)(pMsg + 1);

   cmsLog_debug("ifName = %s, notification %d", pIfName, pMsg->wordData);

   if( (cmsRet = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
   {
      cmsLog_error("could not acquire lock, ret=%d", cmsRet);
      return;
   }

   cmsRet = dalMoca_getMocaObjByIntfName( pIfName, &iidStack, (void **)&obj);
   if ( CMSRET_SUCCESS != cmsRet )
   {
      cmsLog_error("Could not find MoCA object for %s", pIfName);
      cmsLck_releaseLock();
      return;
   }

   switch(pMsg->wordData)
   {
      case MOCA_UPDATE_FLASH_INIT:
         sskMoca_writeMocaControl(obj, MDMVS_UPDATEINIT);
         break;

      case MOCA_UPDATE_FLASH_CONFIG:
         sskMoca_writeMocaControl(obj, MDMVS_UPDATECONFIG);
         break;

      case MOCA_UPDATE_FLASH_TRACE:
         sskMoca_writeMocaControl(obj, MDMVS_UPDATETRACE);
         break;

      default:
         cmsLog_error("Unsupported MoCA notification (%lu) for %s", 
                      pMsg->wordData, pIfName);
         cmsObj_free(&obj);
         cmsLck_releaseLock();
         return;
   }

   cmsRet = cmsObj_set(obj, &iidStack);
   if ( CMSRET_SUCCESS != cmsRet )
   {
      cmsLog_error("Could not set MoCA object for %s", pIfName);
   }
   else
   {
      cmsMgm_saveConfigToFlash();
   }

   cmsObj_free(&obj);
   cmsLck_releaseLock();
   
}


#if defined(DMP_X_BROADCOM_COM_MOCAWAN_1) || defined(DMP_X_BROADCOM_COM_MOCALAN_1)

void sskMoca_writeLof_igd(void *obj, UINT32 lof)
{
   LanMocaIntfObject *mocaObj = (LanMocaIntfObject *) obj;

   mocaObj->lastOperationalFrequency = lof;
}

UINT32 sskMoca_readLof_igd(const void *obj)
{
   LanMocaIntfObject *mocaObj = (LanMocaIntfObject *) obj;

   return (mocaObj->lastOperationalFrequency);
}

void sskMoca_writeNonDefSeqNum_igd(void *obj, UINT32 num)
{
   LanMocaIntfObject *mocaObj = (LanMocaIntfObject *) obj;

   mocaObj->mrNonDefSeqNumber = num;
}

UINT32 sskMoca_readNonDefSeqNum_igd(const void *obj)
{
   LanMocaIntfObject *mocaObj = (LanMocaIntfObject *) obj;

   return (mocaObj->mrNonDefSeqNumber);
}

void sskMoca_writeMocaControl_igd(void *obj, const char *controlStr)
{
   LanMocaIntfObject *mocaObj = (LanMocaIntfObject *) obj;

   CMSMEM_REPLACE_STRING(mocaObj->mocaControl, controlStr);
}

#endif /* TR98 MOCA WAN || MOCA_LAN */


#endif  /* BRCM_MOCA_DAEMON */

#endif /* SUPPORT_MOCA */
