/***********************************************************************
 * <:copyright-BRCM:2007:DUAL/GPL:standard
 * 
 *    Copyright (c) 2007 Broadcom 
 *    All Rights Reserved
 * 
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed
 * to you under the terms of the GNU General Public License version 2
 * (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
 * with the following added to such license:
 * 
 *    As a special exception, the copyright holders of this software give
 *    you permission to link this software with independent modules, and
 *    to copy and distribute the resulting executable under terms of your
 *    choice, provided that you also meet, for each linked independent
 *    module, the terms and conditions of the license of that module.
 *    An independent module is a module which is not derived from this
 *    software.  The special exception does not apply to any modifications
 *    of the software.
 * 
 * Not withstanding the above, under no circumstances may you combine
 * this software in any way with any other Broadcom software provided
 * under a license other than the GPL, without Broadcom's express prior
 * written consent.
 * 
 * :>
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cms.h"
#include "cms_util.h"
#include "cms_msg.h"
#include "cms_msg_modsw.h"
#include "image_modsw_linux.h"
#include "cms_params_modsw.h"


#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_LINUXPFP_1

#ifdef SUPPORT_LINMOSD
static void sendLinmosdNewfileMsg(void *msgHandle, const char *filename)
{
   CmsMsgHeader *msg;
   CmsRet ret;

   cmsLog_debug("sending NEWFILE msg to linmosd %s", filename);

   msg = (CmsMsgHeader *) cmsMem_alloc(sizeof(CmsMsgHeader)+strlen(filename)+1,
                                       ALLOC_ZEROIZE);
   if (msg == NULL)
   {
      cmsLog_error("cmsMem_alloc failed, dropping msg (%s), filename");
      return;
   }

   msg->type = (CmsMsgType) CMS_MSG_LINMOSD_NEWFILE;
   msg->src = cmsMsg_getHandleEid(msgHandle);
   msg->dst = EID_LINMOSD;
   msg->wordData = 0;
   msg->flags_event = 1;
   msg->dataLength = strlen(filename)+1;
   strcpy((char *)(msg+1), filename);

   ret = cmsMsg_send(msgHandle, msg);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("msg send failed, ret=%d", ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
}
#endif  /* SUPPORT_LINMOSD */


UBOOL8 cmsImg_isModSwLinuxPfp(const unsigned char *buf, UINT32 len)
{
   if ((len > CMS_MODSW_LINUX_HEADER1_LEN + CMS_MODSW_LINUX_HEADER2_LEN) &&
       (0 == memcmp(buf, CMS_MODSW_LINUXPFP_HEADER, CMS_MODSW_LINUX_HEADER1_LEN)))
   {
      return TRUE;
   }

   return FALSE;
}


/* this function is just responsible for writing the buffer to the tmp
 * directory and starting modupdated (telling it the location+filename).
 */
CmsRet cmsImg_writeValidatedLinuxPfp(const char *imagePtr, UINT32 imageLen, void *msgHandle)
{
   char filename[CMS_MODSW_LINUX_HEADER2_LEN+1]={0};
   char fullpath[CMS_MAX_FULLPATH_LENGTH]={0};
   CmsRet ret;

   /*
    * Extract the name from the package.  (name is in the second header.)
    * Then put it in the modsw tmp dir.
    */
   memcpy(filename, &imagePtr[CMS_MODSW_LINUX_HEADER1_LEN], CMS_MODSW_LINUX_HEADER2_LEN);

   if ((ret = cmsUtl_getRunTimePath(CMS_MODSW_DIR, fullpath, sizeof(fullpath))) != CMSRET_SUCCESS)
   {
      return ret;
   }

   if ((ret = cmsFil_makeDir(fullpath)) != CMSRET_SUCCESS)
   {
      return ret;
   }

   if ((ret = cmsUtl_getRunTimePath(CMS_MODSW_TMP_DIR, fullpath, sizeof(fullpath))) != CMSRET_SUCCESS)
   {
      return ret;
   }

   if ((ret = cmsFil_makeDir(fullpath)) != CMSRET_SUCCESS)
   {
      return ret;
   }

   if ((ret = cmsUtl_strncat(fullpath, sizeof(fullpath), "/")) != CMSRET_SUCCESS)
   {
      return ret;
   }

   if ((ret = cmsUtl_strncat(fullpath, sizeof(fullpath), filename)) != CMSRET_SUCCESS)
   {
      return ret;
   }

   ret = cmsFil_writeBufferToFile(fullpath, (UINT8 *)imagePtr, imageLen);

   /*
    * Send a message to linmosd with the filename, and it will
    * do the rest
    * --verify signature/checksum
    * --move to correct directory
    * --update MDM
    * --start shutdown sequence
    */
   sendLinmosdNewfileMsg(msgHandle, filename);

   return ret;

}
#endif  /* DMP_DEVICE2_X_BROADCOM_COM_MODSW_LINUXPFP_1 */



