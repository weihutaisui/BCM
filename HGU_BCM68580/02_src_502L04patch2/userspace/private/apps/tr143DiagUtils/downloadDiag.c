/***********************************************************************
 *
 *  Copyright (c) 2012-2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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


#include <stdio.h>

#include "tr143_defs.h"
#include "cms_msg.h"



int main (int argc, char *argv[])
{
   int ret, ch;

   tr143_result_fname=TR143_DOWNLOAD_RESULT_FILE;

   cmsLog_init(EID_DOWNLOAD_DIAG);
   cmsLog_setLevel(DEFAULT_LOG_LEVEL);

   cmsLog_debug("download diag start ......\n");

   if ((ret = cmsMsg_init(EID_DOWNLOAD_DIAG, &msgHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMsg_init failed, ret=%d", ret);
      cleanup_and_notify(Error_InitConnectionFailed, EID_DOWNLOAD_DIAG);
      return 0;
   }

   memset(if_name, 0, sizeof(if_name));
   while ((ch = getopt(argc, argv, "i:d:u:D:")) != -1)
      switch (ch) {
      case 'i':
         strncpy(if_name, optarg, sizeof(if_name)-1);
         break;
      case 'd':
         dscp = atoi(optarg);
         break;
      case 'u':
         url = optarg;
         break;
      case 'D':
         cmsLog_setLevel(atoi(optarg));
         break;
      }

   if (setupconfig() <0 )
   {
      cmsLog_error("setup config error\n");
      cleanup_and_notify(Error_InitConnectionFailed, EID_DOWNLOAD_DIAG);
      return 0;
   }

   if (strstr(proto, "http"))
      ret = httpDownloadDiag();
   else
      ret = ftpDownloadDiag();

   cleanup_and_notify(ret, EID_DOWNLOAD_DIAG);
   return 0;
}



