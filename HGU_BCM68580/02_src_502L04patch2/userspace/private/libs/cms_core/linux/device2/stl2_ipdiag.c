/***********************************************************************
 *
 *  Copyright (c) 2006-2011  Broadcom Corporation
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

#include "stl.h"

/*!\file stl2_ipdiag.c
 * \brief This file contains device 2 device.ip.diagnostics. objects related functions.
 *
 */
#ifdef DMP_DEVICE2_IPPING_1
CmsRet stl_dev2IpDiagnosticsObject(_Dev2IpDiagnosticsObject *obj __attribute__((unused)),
                                       const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}

CmsRet stl_dev2IpPingDiagObject(_Dev2IpPingDiagObject *obj, const InstanceIdStack *iidStack)
{
   /* no change is needed since SSK is updating MDM with proper stats from PING */
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
#endif /* DMP_DEVICE2_IPPING_1 */

#endif    /* DMP_DEVICE2_BASELINE_1 */

#if defined(DMP_DEVICE2_DOWNLOAD_1) || defined(DMP_DEVICE2_UPLOAD_1)
/* this header file is installed by userspace/private/libs/tr143_utils */
#include "tr143_defs.h"
#endif

#ifdef DMP_DEVICE2_DOWNLOAD_1
CmsRet stl_dev2IpDiagDownloadObject(_Dev2IpDiagDownloadObject *obj, const InstanceIdStack *iidStack)
{
	FILE *fp;
	struct tr143diagstats_t diagstats;
	size_t rc;
		
	fp=fopen(TR143_DOWNLOAD_RESULT_FILE,"r");
	if (fp == NULL)  return CMSRET_SUCCESS_OBJECT_UNCHANGED;
	memset(&diagstats, 0, sizeof(diagstats));
	rc = fread(&diagstats,1,sizeof(diagstats),fp);
	fclose(fp);

	if (rc < 1)
   {
      /* fread returns number of elements read (1) */
      cmsLog_error("fread failed, rc=%d", rc);
      return CMSRET_SUCCESS_OBJECT_UNCHANGED;
   }

   CMSMEM_REPLACE_STRING_FLAGS(obj->diagnosticsState, diagstats.DiagnosticsState, mdmLibCtx.allocFlags);
   CMSMEM_REPLACE_STRING_FLAGS(obj->ROMTime, diagstats.ROMTime, mdmLibCtx.allocFlags);
   CMSMEM_REPLACE_STRING_FLAGS(obj->BOMTime, diagstats.BOMTime, mdmLibCtx.allocFlags);
   CMSMEM_REPLACE_STRING_FLAGS(obj->EOMTime, diagstats.EOMTime, mdmLibCtx.allocFlags);
   obj->testBytesReceived = diagstats.TestBytesReceived;
   obj->totalBytesReceived = diagstats.TotalBytesReceived;
#ifdef DMP_DOWNLOADTCP_1
   CMSMEM_REPLACE_STRING_FLAGS(obj->TCPOpenRequestTime, diagstats.TCPOpenRequestTime, mdmLibCtx.allocFlags);
   CMSMEM_REPLACE_STRING_FLAGS(obj->TCPOpenResponseTime, diagstats.TCPOpenResponseTime, mdmLibCtx.allocFlags);
#endif

   return CMSRET_SUCCESS;
}
#endif /* DMP_DEVICE2_DOWNLOAD_1 */

#ifdef DMP_DEVICE2_UPLOAD_1
CmsRet stl_dev2IpDiagUploadObject(_Dev2IpDiagUploadObject *obj, const InstanceIdStack *iidStack)
{
   FILE *fp;
   struct tr143diagstats_t diagstats;
   size_t rc;

   fp=fopen(TR143_UPLOAD_RESULT_FILE,"r");
   if (fp == NULL)  return CMSRET_SUCCESS_OBJECT_UNCHANGED;
   memset(&diagstats, 0, sizeof(diagstats));
   rc = fread(&diagstats,1,sizeof(diagstats),fp);
   fclose(fp);

   if (rc < 1)
   {
      /* fread returns number of elements read (1) */
      cmsLog_error("fread failed, rc=%d", rc);
      return CMSRET_SUCCESS_OBJECT_UNCHANGED;
   }

   CMSMEM_REPLACE_STRING_FLAGS(obj->diagnosticsState, diagstats.DiagnosticsState, mdmLibCtx.allocFlags);
   CMSMEM_REPLACE_STRING_FLAGS(obj->ROMTime, diagstats.ROMTime, mdmLibCtx.allocFlags);
   CMSMEM_REPLACE_STRING_FLAGS(obj->BOMTime, diagstats.BOMTime, mdmLibCtx.allocFlags);
   CMSMEM_REPLACE_STRING_FLAGS(obj->EOMTime, diagstats.EOMTime, mdmLibCtx.allocFlags);
   obj->totalBytesSent = diagstats.TotalBytesSent;
#ifdef DMP_UPLOADTCP_1
   CMSMEM_REPLACE_STRING_FLAGS(obj->TCPOpenRequestTime, diagstats.TCPOpenRequestTime, mdmLibCtx.allocFlags);
   CMSMEM_REPLACE_STRING_FLAGS(obj->TCPOpenResponseTime, diagstats.TCPOpenResponseTime, mdmLibCtx.allocFlags);
#endif

   return CMSRET_SUCCESS;
}
#endif /* DMP_DEVICE2_UPLOAD_1 */

#ifdef DMP_DEVICE2_UDPECHO_1
CmsRet stl_dev2IpDiagUDPEchoConfigObject(_Dev2IpDiagUDPEchoConfigObject *obj, const InstanceIdStack *iidStack)
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
#endif /* DMP_DEVICE2_UDPECHO_1 */

#ifdef DMP_DEVICE2_TRACEROUTE_1
CmsRet stl_dev2IpDiagTraceRouteObject(_Dev2IpDiagTraceRouteObject *obj, const InstanceIdStack *iidStack)
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
};

CmsRet stl_dev2IpDiagTraceRouteRouteHopsObject(_Dev2IpDiagTraceRouteRouteHopsObject *obj, const InstanceIdStack *iidStack)
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
};

#endif /* DMP_DEVICE2_TRACEROUTE_1 */
