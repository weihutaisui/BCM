/***********************************************************************
 *
 *  Copyright (c) 2008-2010  Broadcom Corporation
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

#ifdef DMP_X_ITU_ORG_GPON_1 /* aka SUPPORT_OMCI */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>

#include "cms.h"
#include "cms_msg.h"
#include "cms_util.h"
#include "cms_phl.h"
#include "httpd.h"
#include "cgi_main.h"
#include "cgi_omci.h"


// global variables
extern char connIfName[CMS_IFNAME_LENGTH];
extern UINT16 tcIdCur;

char * dlImage = NULL;
UINT32 dlImageLen = 0;
UINT32 dlCurrentImageLen = 0;
UINT8  dlWindowSize = 0xFF;
UINT8  dlSectionNumber = 0;

/* public functions */

extern CmsRet upLoadImage(FILE *stream, UINT32 content_len, int upload_type,
  char **imageBufPtrPtr, UINT32 *imageLen, UBOOL8 flashB);


int cgiOmciDownloadPre(FILE *stream, int downloadLen)
{
   char pathBuf[BUFLEN_1024];
   CmsRet status = upLoadImage(stream, downloadLen, WEB_UPLOAD_IMAGE, &dlImage, &dlImageLen, 0);

   if (status != CMSRET_SUCCESS)
   {
      cmsLog_debug("freeing image buffer %p", dlImage);
      CMSMEM_FREE_BUF_AND_NULL_PTR(dlImage);
      cmsImg_sendLoadDoneMsg(msgHandle);
   }

   /*
    * this web page will tell the user whether we successfully uploaded
    * the image file.  The user will still have to wait a few minutes for
    * the writing of the flash and reboot.
    */
   cmsLog_debug("sending back upload status");
   makePathToWebPage(pathBuf, sizeof(pathBuf), "uploadinfo.html");
   do_ej(pathBuf, stream);

   if (status == CMSRET_SUCCESS)
   {
      glbStsFlag = WEB_STS_OMCI_DOWNLOAD;
      return WEB_STS_OK;
   }
   else
   {
      return WEB_STS_ERROR;
   }
}

void cgiOmciDownloadPost(void)
{
    // send start software download
    cgiOmci_sendRequest(OMCI_MSG_TYPE_STARTSOFTWAREDOWNLOAD);
}

CmsRet cgiOmci_sendSection(omciPacket *packetRes)
{
#define USE_DEV_ID_A 0
   UINT16 dataSize = (USE_DEV_ID_A ? OMCI_PACKET_A_MSG_SIZE : OMCI_PACKET_B_MSG_SIZE_MAX) - 1;
   long sectionLen = 0;
   char buf[sizeof(CmsMsgHeader) + sizeof(omciPacket)];
   CmsMsgHeader *msg=(CmsMsgHeader *) buf;
   omciPacket *packet = (omciPacket *) (msg+1);
   CmsRet ret = CMSRET_SUCCESS;

   memset(buf, 0, sizeof(CmsMsgHeader) + sizeof(omciPacket));

   msg->type = CMS_MSG_OMCI_COMMAND_REQUEST;
   msg->src = EID_HTTPD;
   msg->dst = EID_OMCID;
   msg->flags_event = 1;
   msg->dataLength = sizeof(omciPacket);
   msg->flags_request = 1;
   msg->sequenceNumber = tcIdCur;

   // transaction ID
   OMCI_HTONS(&packet->tcId, tcIdCur);
   tcIdCur++;

   // device ID
   packet->devId = USE_DEV_ID_A ? OMCI_PACKET_DEV_ID_A : OMCI_PACKET_DEV_ID_B;
   // software image class Id is 7, ONT-G is 0, instance is 0 or 1
   packet->classNo[0] = packetRes->classNo[0];
   packet->classNo[1] = packetRes->classNo[1];
   packet->instId[0]  = packetRes->instId[0];
   packet->instId[1]  = packetRes->instId[1];

   sectionLen = dlImageLen - dlCurrentImageLen;
   if (sectionLen < 0)
   {
      return CMSRET_SUCCESS_OBJECT_UNCHANGED;
   }
   if (sectionLen > dataSize)
   {
      sectionLen = dataSize;
   }

   // only request for response when it is the last section in the segment
   // or the last section in the image
   if (dlSectionNumber != dlWindowSize && (sectionLen + dlCurrentImageLen) != dlImageLen)
   {
      packet->msgType = OMCI_MSG_TYPE_DOWNLOADSECTION;
   }
   else
   {
      packet->msgType = OMCI_MSG_TYPE_AR(OMCI_MSG_TYPE_DOWNLOADSECTION);
   }

   // 1st byte in message content is download section number
   OMCI_PACKET_MSG(packet)[0] = dlSectionNumber;
   // the rest in message content are image data
   memcpy(&OMCI_PACKET_MSG(packet)[1], &dlImage[dlCurrentImageLen], sectionLen);
   
   if (!USE_DEV_ID_A)
   {
      OMCI_HTONS(&packet->B.msgLen, sectionLen + 1);
   }

   cgiOmci_addCrc(packet, EID_HTTPD);

   cmsMsg_send(msgHandle, msg);

   // increase current image len
   dlCurrentImageLen += sectionLen;
   // increase section number
   dlSectionNumber++;
   if (dlSectionNumber > dlWindowSize)
   {
      dlSectionNumber = 0;
   }

   if (dlCurrentImageLen >= dlImageLen)
      ret = CMSRET_SUCCESS_OBJECT_UNCHANGED;

   return ret;
}

CmsRet cgiOmci_sendDownloadSection(omciPacket *packetRes)
{
   UINT16 i = 0;
   CmsRet ret = CMSRET_SUCCESS;

   for (i = 0; i <= dlWindowSize; i++)
   {
      ret = cgiOmci_sendSection(packetRes);
      if (ret != CMSRET_SUCCESS)
          break;
      usleep(USECS_IN_MSEC);
   }

   return ret;
}

#endif   //DMP_X_ITU_ORG_GPON_1
