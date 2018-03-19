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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cms.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_qdm.h"

#include "cgi_main.h"
#include "cgi_sts.h"
#include "cgi_util.h"
#include "cgi_cmd.h"





#ifdef DMP_DEVICE2_MOCA_1


void cgiResetStsMocaLan_dev2(void)
{
   Dev2MocaInterfaceObject *mocaObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   while (cmsObj_getNext(MDMOID_DEV2_MOCA_INTERFACE, &iidStack,(void **)&mocaObj) == CMSRET_SUCCESS)
   {
      if (!mocaObj->upstream)
      {
         cmsLog_debug("reseting moca lan stats at iidStack=%s", cmsMdm_dumpIidStack(&iidStack));
         /* iidStack is pointing to the correct location, use it to reset stats */
         cmsObj_clearStatistics(MDMOID_DEV2_MOCA_INTERFACE_STATS, &iidStack);
      }
      cmsObj_free((void **) &mocaObj);
   }
}


void cgiResetStsMocaWan_dev2(void)
{
   Dev2MocaInterfaceObject *mocaObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   while (cmsObj_getNext(MDMOID_DEV2_MOCA_INTERFACE, &iidStack,(void **)&mocaObj) == CMSRET_SUCCESS)
   {
      if (mocaObj->upstream)
      {
         cmsLog_debug("reseting moca wan stats at iidStack=%s", cmsMdm_dumpIidStack(&iidStack));
         /* iidStack is pointing to the correct location, use it to reset stats */
         cmsObj_clearStatistics(MDMOID_DEV2_MOCA_INTERFACE_STATS, &iidStack);
      }
      cmsObj_free((void **) &mocaObj);
   }
}


void cgiStsMoca_dev2(char *query __attribute__((unused)), FILE *fs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2MocaInterfaceObject *mocaObj=NULL;
   Dev2MocaInterfaceStatsObject *mocaStatsObj=NULL;
   UBOOL8 found=FALSE;
   static UBOOL8 lanStat = TRUE;
   char choice[BUFLEN_32];


   /* The lanStat is defined static since refresh will not pass in the "LAN" or "WAN" parameter */
   cgiGetValueByName(query, "choice", choice );   

   if (!cmsUtl_strcmp(choice, MDMVS_WAN))
   {
      lanStat = FALSE;
   }
   else if (!cmsUtl_strcmp(choice, MDMVS_LAN))
   {
      lanStat = TRUE;
   }
   else if (!IS_EMPTY_STRING(choice))
   {
      cmsLog_error("Unexpected moca intf choice %s", choice);
   }

   if (lanStat)
   {
      found = dalMoca_getLanMocaIntfByIfName(NULL, &iidStack, (void **)&mocaObj);
   }
   else
   {
      found = dalMoca_getWanMocaIntfByIfName(NULL, &iidStack, (void **) &mocaObj);
   }

   if (found)
   {
      CmsRet ret;
      /* iidStack contains info to allow us to get mocaIntfStatsObj */
      ret = cmsObj_get(MDMOID_DEV2_MOCA_INTERFACE_STATS, &iidStack, 0,
                       (void **) &mocaStatsObj);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get mocaStats obj at %s (ret=%d)",
                      cmsMdm_dumpIidStack(&iidStack), ret);
      }
   }


   // write Java Script
   writeStsMocaScript(fs);

   // write body
   fprintf(fs, "</head>\n<body>\n<blockquote>\n<center>\n<form>\n");
   /* write table */
   fprintf(fs, "<center>\n");
   fprintf(fs, "<b>Moca Status and Statistics</b><br><br>\n");
   fprintf(fs, "<table border='0' cellpadding='0' cellspacing='0'>\n");
   fprintf(fs, "  <tr>\n");
   fprintf(fs, "  <td><select name='mocaSel' size='1' onChange='onMocaSel()'>\n");
   if (lanStat)
   {
      fprintf(fs, "    <option value='%s' selected>%s\n", MDMVS_LAN, MDMVS_LAN);
      fprintf(fs, "    <option value='%s'>%s\n", MDMVS_WAN, MDMVS_WAN);
   }
   else
   {
      fprintf(fs, "    <option value='%s'>%s\n", MDMVS_LAN, MDMVS_LAN);
      fprintf(fs, "    <option value='%s' selected>%s\n", MDMVS_WAN, MDMVS_WAN);
   }
   fprintf(fs, "  </select></td>\n");
   fprintf(fs, "  </tr>\n");
   fprintf(fs, "</table><br><br>\n");


   if (mocaObj != NULL && mocaStatsObj != NULL)
   {
      fprintf(fs, "<b>Status</b><br>\n");
      fprintf(fs, "<table border='1' cellpadding='3' cellspacing='0'>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>Interface</td>\n");
      fprintf(fs, "      <td>%s</td>\n", mocaObj->name);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>VendorId</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaObj->X_BROADCOM_COM_VendorId);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>HWVersion</td>\n");
      fprintf(fs, "      <td>%X</td>\n", mocaObj->X_BROADCOM_COM_HwVersion);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>SoftwareVersion</td>\n");
      fprintf(fs, "      <td>%s</td>\n", mocaObj->firmwareVersion);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>SelfMoCAVersion</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaObj->X_BROADCOM_COM_SelfMoCAVersion);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>NetworkVersionNumber</td>\n");
      fprintf(fs, "      <td>%s</td>\n", mocaObj->currentVersion);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>Qam256Support</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaObj->QAM256Capable);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>Enable</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaObj->enable);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>LinkStatus</td>\n");
      fprintf(fs, "      <td>%s</td>\n", mocaObj->status);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>LastChange</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaObj->lastChange);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>ConnectedNodes</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaObj->associatedDeviceNumberOfEntries);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>NodeId</td></td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaObj->nodeID);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>NetworkControllerNodeId</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaObj->networkCoordinator);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>BackupNetworkControllerNodeId</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaObj->backupNC);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>RfChannel</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaObj->currentOperFreq);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>BwStatus</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaObj->X_BROADCOM_COM_BwStatus);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "</table>\n");
      fprintf(fs, "<br><br>\n");

      fprintf(fs, "<b>Statistics</b><br>\n");
      fprintf(fs, "<table border='1' cellpadding='3' cellspacing='0'>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>Interface</td>\n");
      fprintf(fs, "      <td>%s</td>\n", mocaObj->name);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>OutUcPkts</td>\n");
      fprintf(fs, "      <td>%llu</td>\n", mocaStatsObj->unicastPacketsSent);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>InUcPkts</td>\n");
      fprintf(fs, "      <td>%llu</td>\n", mocaStatsObj->unicastPacketsReceived);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>OutMcPkts</td>\n");
      fprintf(fs, "      <td>%llu</td>\n", mocaStatsObj->multicastPacketsSent);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>InMcPkts</td>\n");
      fprintf(fs, "      <td>%llu</td>\n", mocaStatsObj->multicastPacketsReceived);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>OutBcPkts</td>\n");
      fprintf(fs, "      <td>%llu</td>\n", mocaStatsObj->broadcastPacketsSent);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>InBcPkts</td>\n");
      fprintf(fs, "      <td>%llu</td>\n", mocaStatsObj->broadcastPacketsReceived);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>InOctets</td>\n");
      fprintf(fs, "      <td>%llu</td>\n", mocaStatsObj->bytesReceived);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>OutOctets</td>\n");
      fprintf(fs, "      <td>%llu</td>\n", mocaStatsObj->bytesSent);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>InDiscardPkts</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatsObj->discardPacketsReceived);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>OutDiscardPkts</td></td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatsObj->discardPacketsSent);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>InUnKnownPkts</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatsObj->unknownProtoPacketsReceived);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>NetworkControllerHandOffs</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatsObj->X_BROADCOM_COM_NCHandOffs);
      fprintf(fs, "   </tr>\n");

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>NetworkControllerBackups</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatsObj->X_BROADCOM_COM_NCBackups);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "</table>\n");
      fprintf(fs, "<br><br>\n");

      cmsObj_free((void **) &mocaObj);
      cmsObj_free((void **) &mocaStatsObj);
   }
   else
   {
      fprintf(fs, "<b>No Moca interface found.</b><br>\n");
   }
   
   fprintf(fs, "<p align='center'>\n");
   fprintf(fs, "<input type='button' onClick='btnReset()' value='Reset'>\n");
   fprintf(fs, "</form>\n</center>\n</blockquote>\n</body>\n</html>\n");

   fflush(fs);
}

#endif /* DMP_DEVICE2_MOCA_1 */

