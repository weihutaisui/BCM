/*
 *
 * Copyright (c) 2003-2012  Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
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
 *
 * $Id: linux_igd.c,v 1.10.2.2 2003/10/31 21:31:36 mthawani Exp $
 */

#include <sys/ioctl.h>
#include <net/if.h>
//#include <linux/sockios.h>
#include <linux/socket.h>
#include <signal.h>
#include <signal.h>

#define __KERNEL__
#include <asm/types.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>

#include <string.h>         // for memset, strncasecmp, etc.
#include <stdio.h>          // for printf, perror, fopen, fclose, etc.
#include <arpa/inet.h>      // for inet_aton, etc.
#include <time.h>           // for time, strftime, gmtime, etc.
#include <stdlib.h>         // for malloc, free, atoi, atol, etc.

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "../igd/igd.h"
#include "shutils.h"

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "cms_log.h"
#include "cms_mem.h"
#include "cms_msg.h"
#include "bcmatmapi.h"
#include "atmapidrv.h"
#include "bcmadsl.h"
#include "adsldrv.h"

//~~~ not used in cms #include "board_api.h"


#define _PATH_PROCNET_DEV           "/proc/net/dev"
#define CMD_BUF_SZ                  256
#define IFC_STATUS_SZ               16
#define IFC_PATH_SZ                 256

//~~~ not used in cms  extern int bcmSystemEx ( char *cmd, int printFlag );

extern void *g_msgHandle;

/* Return OSL_LINK_UP if the link status given interface is UP, OSL_LINK_DOWN otherwise. */
osl_link_t osl_link_status(char *devname)
{
   CmsMsgHeader *msg;
   char *data;
   void *msgBuf;
   UINT32 msgDataLen = 0;
   CmsRet ret;
   void *msgHandle = g_msgHandle;

   cmsLog_debug("====>Enter. devname=%s", devname);

   msgDataLen = strlen(devname) + 1;

   msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE);

   if(msgBuf == NULL)
   {
      cmsLog_error(" cmsMem_alloc failed to allocate %d bytes", sizeof(CmsMsgHeader) + msgDataLen);
      return FALSE;
   }

   msg = (CmsMsgHeader *)msgBuf;

   msg->type = CMS_MSG_GET_WAN_LINK_STATUS;
   msg->src = EID_UPNP;
   msg->dst = EID_SSK;
   msg->flags_request = 1;

   data = (char *) (msg + 1);
   msg->dataLength = msgDataLen;
   strcpy(data, devname);

   /*
    * ssk will reply with TRUE or FALSE for wan connection up.
    * Need to check for that instead of the usual CMSRET enum.
    */
   ret = cmsMsg_sendAndGetReply(msgHandle, msg);
   cmsMem_free(msgBuf);

   if (ret == WAN_LINK_UP)
   {
      cmsLog_debug("ssk says %s is up. ret=%d", devname, ret);
      return OSL_LINK_UP;
   }
   else
   {
      cmsLog_debug("ssk says %s is not up. ret=%d", devname, ret);
      return OSL_LINK_DOWN;
   }

}


void osl_sys_restart()
{
   cmsLog_debug("====>kill SIGHUP pid=1");

   kill(1, SIGHUP);
}

#if 0
void osl_sys_reboot()
{
   cmsLog_debug("====>kill SIGTERM pid=1");

   kill(1, SIGTERM);
}
#endif


bool osl_wan_isup(char *devname)
{
   CmsMsgHeader *msg;
   char *data;
   void *msgBuf;
   UINT32 msgDataLen = 0;
   CmsRet ret;
   void *msgHandle = g_msgHandle;

   cmsLog_debug("====>Enter. devname=%s", devname);

   msgDataLen = strlen(devname) + 1;

   msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE);

   if(msgBuf == NULL)
   {
      cmsLog_error(" cmsMem_alloc failed to allocate %d bytes", sizeof(CmsMsgHeader) + msgDataLen);
      return FALSE;
   }

   msg = (CmsMsgHeader *)msgBuf;

   msg->type = CMS_MSG_GET_WAN_CONN_STATUS;
   msg->src = EID_UPNP;
   msg->dst = EID_SSK;
   msg->flags_request = 1;

   data = (char *) (msg + 1);
   msg->dataLength = msgDataLen;
   strcpy(data, devname);

   /*
    * ssk will reply with TRUE or FALSE for wan connection up.
    * Need to check for that instead of the usual CMSRET enum.
    */
   ret = cmsMsg_sendAndGetReply(msgHandle, msg);
   cmsMem_free(msgBuf);

   if (ret == TRUE)
   {
      cmsLog_debug("ssk says %s is up. ret=%d", devname, ret);
      return TRUE;
   }
   else
   {
      cmsLog_debug("ssk says %s is not up. ret=%d", devname, ret);
      return FALSE;
   }
}


