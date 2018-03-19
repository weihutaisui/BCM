/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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

#ifdef BRCM_WLAN


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cms.h"
#include "cms_obj.h"
#include "cms_util.h"

#include "cgi_main.h"
#include "cgi_sts.h"
#include "cgi_util.h"
#include "cgi_cmd.h"


#ifdef DMP_WIFILAN_1

void cgiGetStsWifiIfc_igd(char **pp)
{
   char *p = *pp;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanWlanObject *wlanObj = NULL;

   while (cmsObj_getNext(MDMOID_LAN_WLAN, &iidStack, (void **) &wlanObj) == CMSRET_SUCCESS)
   {
      p += sprintf(p, "               <tr>");
      p += sprintf(p, "                  <td class='hd'>%s</td>", wlanObj->X_BROADCOM_COM_IfName);
//      if (cmsObj_get(MDMOID_LAN_WLAN_STATS, &iidStack, 0, (void **) &wlanStatsObj) == CMSRET_SUCCESS)
      {
         p += sprintf(p, "                  <td>%u</td>",wlanObj->totalBytesReceived);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->totalPacketsReceived);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->X_BROADCOM_COM_RxErrors);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->X_BROADCOM_COM_RxDrops);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->X_BROADCOM_COM_MulticastBytesReceived);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->X_BROADCOM_COM_MulticastPacketsReceived);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->X_BROADCOM_COM_UnicastPacketsReceived);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->X_BROADCOM_COM_BroadcastPacketsReceived);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->totalBytesSent);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->totalPacketsSent);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->X_BROADCOM_COM_TxErrors);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->X_BROADCOM_COM_TxDrops);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->X_BROADCOM_COM_MulticastBytesSent);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->X_BROADCOM_COM_MulticastPacketsSent);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->X_BROADCOM_COM_UnicastPacketsSent);
         p += sprintf(p, "                  <td>%u</td>",wlanObj->X_BROADCOM_COM_BroadcastPacketsSent);
         p += sprintf(p, "               </tr>");
//         cmsObj_free((void **) &wlanStatsObj);
      } /* if wlan_itf_stats */
      cmsObj_free((void **) &wlanObj);
   }

   *pp = p;
}

extern CmsRet stl_lanWlanClearStats(_LanWlanObject *obj);

void cgiResetStsWifiLan_igd(void)
{
   LanWlanObject *wlanObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   while ((cmsObj_getNext(MDMOID_LAN_WLAN, &iidStack, (void **) &wlanObj)) == CMSRET_SUCCESS)
   {
      /* Really should not call stl from here, does not conform to CMS architecture! */
      stl_lanWlanClearStats(wlanObj);
      cmsObj_free((void **) &wlanObj);
   }
}
#endif  /* DMP_WIFILAN_1 */


#ifdef DMP_DEVICE2_WIFIACCESSPOINT_1

void cgiGetStsWifiIfc_dev2(char **pp)
{
   char *p = *pp;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2WifiSsidObject *ssid = NULL;

   while (cmsObj_getNextFlags(MDMOID_DEV2_WIFI_SSID, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &ssid) == CMSRET_SUCCESS)
   {
      if ( ssid->enable )
      {
         Dev2WifiSsidStatsObject *ssidStatsObj = NULL;

         if (cmsObj_get(MDMOID_DEV2_WIFI_SSID_STATS, &iidStack, 0, (void **) &ssidStatsObj) == CMSRET_SUCCESS)
         {
            p += sprintf(p, "               <tr>");
            p += sprintf(p, "                  <td class='hd'>%s</td>", ssid->name);
            {
               p += sprintf(p, "                  <td>%" PRIu64 "</td>",ssidStatsObj->bytesReceived);
               p += sprintf(p, "                  <td>%" PRIu64 "</td>",ssidStatsObj->packetsReceived);
               p += sprintf(p, "                  <td>%" PRIu32 "</td>",0);  // RxErrors
               p += sprintf(p, "                  <td>%" PRIu32 "</td>",ssidStatsObj->discardPacketsReceived);
               p += sprintf(p, "                  <td>%" PRIu32 "</td>",0); // MulticastBytesReceived);
               p += sprintf(p, "                  <td>%" PRIu64 "</td>",ssidStatsObj->multicastPacketsReceived);
               p += sprintf(p, "                  <td>%" PRIu64 "</td>",ssidStatsObj->packetsReceived);
               p += sprintf(p, "                  <td>%" PRIu64 "</td>",ssidStatsObj->broadcastPacketsReceived);
               p += sprintf(p, "                  <td>%" PRIu64 "</td>",ssidStatsObj->bytesSent);
               p += sprintf(p, "                  <td>%" PRIu64 "</td>",ssidStatsObj->packetsSent);
               p += sprintf(p, "                  <td>%" PRIu32 "</td>",0);  // error tx packets
               p += sprintf(p, "                  <td>%" PRIu32 "</td>",ssidStatsObj->discardPacketsSent);
               p += sprintf(p, "                  <td>%" PRIu32 "</td>",0);  // multicastBytesSent
               p += sprintf(p, "                  <td>%" PRIu64 "</td>",ssidStatsObj->multicastPacketsSent);
               p += sprintf(p, "                  <td>%" PRIu64 "</td>",ssidStatsObj->packetsSent);
               p += sprintf(p, "                  <td>%" PRIu64 "</td>",ssidStatsObj->broadcastPacketsSent);
               p += sprintf(p, "               </tr>");
            }
            cmsObj_free((void **) &ssidStatsObj);
         }
      }
      cmsObj_free((void **) &ssid);
   }

   *pp = p;
}

void cgiResetStsWifiLan_dev2(void)
{
   Dev2WifiRadioObject *radioObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   while ((cmsObj_getNext(MDMOID_DEV2_WIFI_RADIO, &iidStack, (void **) &radioObj)) == CMSRET_SUCCESS)
   {
      /* If upstream == FALSE, then this is a LAN facing Wifi radio */
      if (radioObj->upstream == FALSE)
      {
         /* this is the standard CMS way, use cmsObj_clearStatistics.
          * Note that even though I am clearing the RADIO_STATS object,
          * I am using the iidStack from the Radio object.  This is OK because
          * there is exactly one RADIO_STATS object associated with each
          * RADIO object so their iidStacks are the same.
          */
         cmsObj_clearStatistics(MDMOID_DEV2_WIFI_RADIO_STATS, &iidStack);
      }
      cmsObj_free((void **) &radioObj);
   }
}
#endif /* DMP_DEVICE2_WIFIACCESSPOINT_1 */

#endif /* BRCM_WLAN */

