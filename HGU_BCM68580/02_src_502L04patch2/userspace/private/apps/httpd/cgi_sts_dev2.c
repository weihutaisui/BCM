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

#ifdef DMP_DEVICE2_OPTICAL_1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "cms_dal.h"
#include "cms_util.h"

#ifdef DMP_X_ITU_ORG_GPON_1

/*  for optical interface statistics */
void cgiGetStsOptical(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   OpticalInterfaceStatsObject *opticalStatsObj=NULL;
   OpticalInterfaceObject *opticalObj = NULL;
   char *p = varValue;

   while (cmsObj_getNext(MDMOID_OPTICAL_INTERFACE, &iidStack, (void **) &opticalObj) == CMSRET_SUCCESS)
   {
      if (cmsObj_get(MDMOID_OPTICAL_INTERFACE_STATS, &iidStack, 0, (void **) &opticalStatsObj) == CMSRET_SUCCESS)
      {
         p += sprintf(p, "               <tr>\n");
         p += sprintf(p, "                  <td class='hd'>%s</td>\n", opticalObj->name);
         p += sprintf(p, "                  <td>%u</td>\n", (UINT32) opticalStatsObj->bytesReceived);
         p += sprintf(p, "                  <td>%u</td>\n", (UINT32) opticalStatsObj->packetsReceived);
         p += sprintf(p, "                  <td>%u</td>\n", opticalStatsObj->errorsReceived);
         p += sprintf(p, "                  <td>%u</td>\n", opticalStatsObj->discardPacketsReceived);
         p += sprintf(p, "                  <td>%u</td>\n", (UINT32) opticalStatsObj->bytesSent);
         p += sprintf(p, "                  <td>%u</td>\n", (UINT32) opticalStatsObj->packetsSent);
         p += sprintf(p, "                  <td>%u</td>\n", opticalStatsObj->errorsSent);
         p += sprintf(p, "                  <td>%u</td>\n", opticalStatsObj->discardPacketsSent);
         p += sprintf(p, "               </tr>\n");

         cmsObj_free((void **) &opticalStatsObj);
      }

      p += sprintf(p, "            </table>\n");
      p += sprintf(p, "            <br><br>\n");
      p += sprintf(p, "            <input type='button' onClick='resetClick()' value='Reset Statistics'>\n");
      p += sprintf(p, "            <br><br><br>\n");
      p += sprintf(p, "            <table border='1' cellpadding='2' cellspacing='0'>\n");
      p += sprintf(p, "               <tr>\n");
      p += sprintf(p, "                  <td class='hd'>Link Status</td>\n");
      p += sprintf(p, "                  <td class='hd'>Optical Signal Level</td>\n");
      p += sprintf(p, "                  <td class='hd'>Transmit Optical Level</td>\n");
      p += sprintf(p, "               </tr>\n");
      p += sprintf(p, "               <tr>\n");
      p += sprintf(p, "                  <td><center>%s</center></td>\n", opticalObj->status);
      p += sprintf(p, "                  <td><center>%u</center></td>\n", opticalObj->opticalSignalLevel);
      p += sprintf(p, "                  <td><center>%u</center></td>\n", opticalObj->transmitOpticalLevel);
      p += sprintf(p, "               </tr>\n");

      cmsObj_free((void **) &opticalObj);
   }

   *p = 0;

   return;
}

/* reseting/clearing Optic interfaces' stats */
void cgiResetStsOptical(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   OpticalInterfaceObject *opticalObj = NULL;

   while ((cmsObj_getNext(MDMOID_OPTICAL_INTERFACE, &iidStack, (void **) &opticalObj)) == CMSRET_SUCCESS)
   {
      cmsObj_clearStatistics(MDMOID_OPTICAL_INTERFACE_STATS, &iidStack);
      cmsObj_free((void **) &opticalObj);
   }
}

#endif    // DMP_X_ITU_ORG_GPON_1

#endif    // DMP_DEVICE2_OPTICAL_1

