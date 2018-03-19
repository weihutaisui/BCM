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


#include "hotplug.h"

#ifdef DMP_DEVICE2_USBHOSTSBASIC_1

extern void *g_msgHandle;
void notify_ssk_usb_state_change(char *action, char *usbDevName, int msgType)
{
   CmsMsgHeader *msg;
   char *data;
   void *msgBuf;
   UINT32 msgDataLen = 0;
   CmsRet ret;
   void *msgHandle = g_msgHandle ;
   int maxRetries = 5;

   if(msgHandle == NULL)
   {
      do
      {
         if ((ret = cmsMsg_initWithFlags(EID_HOTPLUG, EIF_MULTIPLE_INSTANCES, &g_msgHandle)) == CMSRET_SUCCESS)
         {
            msgHandle = g_msgHandle;
            break; 
         }
         else
         {
            sleep(30);
            g_msgHandle = NULL;
         }

      } while(maxRetries--);

   
      if(ret != CMSRET_SUCCESS)
      {
          cprintf("cmsMsg_init error ret=%d", ret);
          return;
      }
   }
    

   msgDataLen =  strlen(usbDevName) + 1;

   msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE);

   if (msgBuf != NULL) {
      msg = (CmsMsgHeader *)msgBuf;

      msg->type = msgType;
      msg->src = MAKE_SPECIFIC_EID(getpid(), EID_HOTPLUG);
      msg->dst = EID_SSK;
      msg->wordData = !strcmp(action, "add");
      msg->flags_event = 1;
     
       
      data = (char *) (msg + 1);
      msg->dataLength = msgDataLen;
      strcpy(data, usbDevName);
       
      ret = cmsMsg_send(msgHandle, msg);
      /*ignore if send fails */
      cmsMem_free(msgBuf);
   }

   cmsMsg_cleanup(&msgHandle);
}

int hotplug_usb_handler(void)
{

   char *action = NULL;
   char *usbDevName = NULL;
   
   /* USBDEVNAME format: hh-bb[.pp]:cc-ff (example: 1-1:1.0, 1-1.4:1.0)
   * hh: host controller bus number
   * bb: parent hub number    
   * pp: port number
   * cc: configuration number
   * ff: interface number    
   */
   usbDevName = getenv("USBDEVNAME");
   if(!usbDevName)
   {
       return 0;
   }
   
   action = getenv("ACTION");
   if(!action)
   {
       HOTPLUG_ERROR("getenv(ACTION) failed");
       return EINVAL;
   }
   
   HOTPLUG_DEBUG("usbDevName=%s, action=%s", usbDevName, action);
  
   if(strcmp(action,"add") && strcmp(action,"remove")) 
   {
      HOTPLUG_DEBUG("action ignored");
      return 0;
   }
   
   notify_ssk_usb_state_change(action, usbDevName, CMS_MSG_USB_DEVICE_STATE_CHANGE);
   
   return 0;
}

#endif

