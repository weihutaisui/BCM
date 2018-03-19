/***********************************************************************
 *
 *  Copyright (c) 2006-2008  Broadcom Corporation
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

#ifdef DMP_DEVICE2_WIFIRADIO_1

/*!\file rut2_wifi.c
 * \brief This file contains common TR181 Wifi helper functions.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <mdm.h>
#include "cms_core.h"
#include "cms_util.h"
#include "rut2_wifi.h"

#include "wlcsm_lib_api.h"

/* tmp buf used to form a config line */
char wifi_configBuf[BUFLEN_128];

#ifdef DESKTOP_LINUX
static void writeNvram_desktop(const char *configStr);
#endif

UBOOL8 rutWifi_getWlanSsidObjByIfName(const char *ifName,
                Dev2WifiSsidObject **ssidObj,
                InstanceIdStack *iidStack)
{
        UBOOL8 found=FALSE;

        while (!found &&
                        cmsObj_getNextFlags(MDMOID_DEV2_WIFI_SSID, iidStack, OGF_NO_VALUE_UPDATE, (void **) ssidObj) == CMSRET_SUCCESS)
        {
                if ( !cmsUtl_strcmp((*ssidObj)->name, ifName))
                        found = TRUE;
                else
                        cmsObj_free((void **) ssidObj);
        }
        return found;
}

void rutWifi_writeNvram(const char *configStr)
{
#ifdef DESKTOP_LINUX
   writeNvram_desktop(configStr);
   return;
#endif

  // cmsLog_error("not implemented yet, configStr=%s", configStr);
   return;
}

CmsRet rutWifi_updateWlmngr(MdmObjectId oid, UINT32 radioIndex, UINT32 secondIndex, UINT32 thirdIndex, const paramNodeList *changedParams)
{
   paramNodeList *tmpParamNode;
   UINT32 isValid = 0, subIndex=0, ret;

   subIndex = secondIndex + (thirdIndex << 16);
   cmsLog_debug("SUBINDEX=%08x\n", __func__, subIndex);

   for (tmpParamNode = changedParams ; tmpParamNode != NULL ; tmpParamNode = tmpParamNode->nextNode)
   {
      ret = wlcsm_mngr_dm_validate(radioIndex, subIndex, oid, tmpParamNode->offset, tmpParamNode->value);
      isValid = !ret;
      if (ret != 0)
      {
         cmsLog_error("wlcsm_mngr_dm_set error, ret=%d", ret);
         break;
      }
   }

   if (isValid)
   {
   if (mdmLibCtx.eid == EID_TR69C || mdmLibCtx.eid == EID_CWMPD)
      wlcsm_mngr_restart(radioIndex,WLCSM_MNGR_RESTART_TR69C,WLCSM_MNGR_RESTART_NOSAVEDM,0);
   else 
      wlcsm_mngr_restart(radioIndex,WLCSM_MNGR_RESTART_MDM,WLCSM_MNGR_RESTART_NOSAVEDM,0);
      return CMSRET_SUCCESS;
   }
   else
      return CMSRET_INVALID_PARAM_VALUE;
}


CmsRet rutWifi_getRadioCounters(const char *devName, struct RadioCounters *rCounters)
{
   char cmdBuf[BUFLEN_128] = {0};
   FILE *fp = NULL;

   if (!rCounters || !devName || devName[0]!='w')
      return CMSRET_INTERNAL_ERROR;

   sprintf(cmdBuf, "wlctl -i %s counters > /var/%scounters", devName, devName);
   rut_doSystemAction("rutWifi", cmdBuf);
   sprintf(cmdBuf, "/var/%scounters", devName);
   fp = fopen(cmdBuf, "r");
   if (fp)
   {
      char buf[BUFLEN_1024];
      while(fgets(buf, 1024, fp))
      {
         char *ptr;
         if ((ptr = strstr(buf, "rxbadplcp"))!= NULL)
            sscanf(ptr+10, "%d", &(rCounters->PLCPErrorCount));
         else if ((ptr = strstr(buf, "rxbadfcs")) != NULL)
            sscanf(ptr+9, "%d", &(rCounters->FCSErrorCount));
         else if ((ptr = strstr(buf, "rxbadproto")) != NULL)
            sscanf(ptr+11, "%d", &(rCounters->invalidMACCount));
         else if ((ptr = strstr(buf, "rxbadda")) != NULL)
            sscanf(ptr+8, "%d", &(rCounters->packetsOtherReceived));
      }
      fclose(fp);
   }
   unlink(cmdBuf);
   fprintf(stderr, "%s: %s plcp:%d fcs:%d proto:%d da:%d\n", __func__, devName,
         rCounters->PLCPErrorCount,
         rCounters->FCSErrorCount,
         rCounters->invalidMACCount,
         rCounters->packetsOtherReceived);
   return CMSRET_SUCCESS;
}

CmsRet rutWifi_getSSIDCounters(const char *devName, struct SSIDCounters *sCounters)
{
   char cmdBuf[BUFLEN_128] = {0};
   FILE *fp = NULL;

   sprintf(cmdBuf, "wlctl -i %s counters > /var/%scounters", devName, devName);
   rut_doSystemAction("rutWifi", cmdBuf);
   sprintf(cmdBuf, "/var/%scounters", devName);
   fp = fopen(cmdBuf, "r");
   if (fp)
   {
      char buf[BUFLEN_1024];
      while(fgets(buf, 1024, fp))
      {
         char *ptr;
         if ((ptr = strstr(buf, "txretrans"))!= NULL)
            sscanf(ptr+10, "%d", &(sCounters->retransCount));
         if ((ptr = strstr(buf, "txfail")) != NULL)
            sscanf(ptr+7, "%d", &(sCounters->failedRetransCount));
         if ((ptr = strstr(buf, "d11_txretry")) != NULL)
            sscanf(ptr+12, "%d", &(sCounters->retryCount));
         if ((ptr = strstr(buf, "d11_txretrie")) != NULL)
            sscanf(ptr+13, "%d", &(sCounters->multipleRetryCount));
         if ((ptr = strstr(buf, "d11_txnoack")) != NULL)
            sscanf(ptr+12, "%d", &(sCounters->ACKFailureCount));
         if ((ptr = strstr(buf, "txampdu")) != NULL)
            sscanf(ptr+8, "%d", &(sCounters->aggregatedPacketCount));
      }
      fclose(fp);
   }
   unlink(cmdBuf);
   return CMSRET_SUCCESS;
}


#ifdef DESKTOP_LINUX
#define WIFI_DESKTOP_NVRAM  "wifi-nvram.txt"

static void writeNvram_desktop(const char *configStr)
{
   FILE *fp;
   size_t count;
   UINT32 len;

   len = cmsUtl_strlen(configStr);
   if (len == 0)
   {
      cmsLog_error("configStr is NULL or 0 len");
      return;
   }

   fp = fopen(WIFI_DESKTOP_NVRAM, "a+");
   if (fp == NULL)
   {
      cmsLog_error("open of %s failed", WIFI_DESKTOP_NVRAM);
      return;
   }

   count = fwrite(configStr, len, 1, fp);
   if (count != (size_t) 1)
   {
      cmsLog_error("fwrite error, got %d expected %d", (int)count, 1);
   }

   /* for desktop only: write a newline (real nvram does not need it?) */
   count = fwrite("\n", 1, 1, fp);
   if (count != 1)
   {
      cmsLog_error("fwrite of newline failed!");
   }

   fclose(fp);
}
#endif


#endif  /* DMP_DEVICE2_WIFIRADIO_1 */

#endif  /* DMP_DEVICE2_BASELINE_1 */

