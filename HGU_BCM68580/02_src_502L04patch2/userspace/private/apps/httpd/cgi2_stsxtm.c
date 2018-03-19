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
#ifdef DMP_DEVICE2_BASELINE_1

#if defined(DMP_DEVICE2_ATMLINK_1) || defined(DMP_DEVICE2_PTMLINK_1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>
#include <bcm_local_kernel_include/linux/sockios.h>
#include "mdm_validstrings.h"
#include "cms_util.h"

#include "cgi_main.h"
#include "cgi_sts.h"
#include "cgi_util.h"
#include "syscall.h"
#include "cms_boardcmds.h"
#include "cms_dal.h"
#include "cms_qdm.h"
#include "cgi_cmd.h"



/* this one can be used to display PTM and ATM traffic */
void cgiDisplayStsXtm_dev2(FILE *fs)
{
	InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2AtmLinkStatsObject *atmStats = NULL;
   Dev2AtmLinkObject *atmLinkObj;
   Dev2PtmLinkStatsObject *ptmStats = NULL;
   Dev2PtmLinkObject *ptmLinkObj;
   UBOOL8 isAtm = FALSE;

	// write html header
	fprintf(fs, "<html><head>\n");
	fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
	fprintf(fs, "<meta http-equiv='refresh' content='10'>\n");
	fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");

	// write Java Script
	writeStsXtmScript(fs);

	// write body
	fprintf(fs, "</head>\n<body>\n<blockquote>\n<center>\n<form>\n");

   /* in the new xtm driver, errors such as Hec, PTI, invalid VPI/VCI, GFC errors are
    * no longer available for display.  They are not available in 6368.
    * If necessary, SAR registers can be dumped to see these errors.  WEBUI is not
    * displaying these anymore, unless xtm drivers distinguish the different chips and
    * return different stats available on them. 
    */

	// write table 
   
   fprintf(fs, "<b><strong>Interface Statistics</strong></b><br>\n");

   fprintf(fs, "<table border='1' cellpadding='3' cellspacing='0'>\n");

   // write table header
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Port Number</td>\n");
   fprintf(fs, "      <td class='hd'>In Octets</td>\n");
   fprintf(fs, "      <td class='hd'>Out Octets</td>\n");
   fprintf(fs, "      <td class='hd'>In Packets</td>\n");
   fprintf(fs, "      <td class='hd'>Out Packets</td>\n");
   fprintf(fs, "      <td class='hd'>In OAM Cells</td>\n");
   fprintf(fs, "      <td class='hd'>Out OAM Cells</td>\n");
   fprintf(fs, "      <td class='hd'>In ASM Cells</td>\n");
   fprintf(fs, "      <td class='hd'>Out ASM Cells</td>\n");
   fprintf(fs, "      <td class='hd'>In Packet Errors</td></td>\n");
   fprintf(fs, "      <td class='hd'>In Cell Errors</td>\n");
   fprintf(fs, "   </tr>\n");


   dalDsl_displayXtmStatsType_dev2(&isAtm);

   if (isAtm)
   {
      while (cmsObj_getNext(MDMOID_DEV2_ATM_LINK, &iidStack,(void **)&atmLinkObj) == CMSRET_SUCCESS)
      {
         if (cmsObj_get(MDMOID_DEV2_ATM_LINK_STATS, &iidStack,0,(void **)&atmStats) == CMSRET_SUCCESS)
         {
            /* There is only an ATM interface stats, and not VCC (ATMLINK stats).
               So, xtm interface stats is not per VCC but per ATM port 
            */
            fprintf(fs, "   <tr align='center'>\n");
            fprintf(fs, "      <td>%u</td>\n", atmLinkObj->X_BROADCOM_COM_ATMInterfaceId);
            fprintf(fs, "      <td>%lu</td>\n", (unsigned long)atmStats->bytesReceived);
            fprintf(fs, "      <td>%lu</td>\n", (unsigned long)atmStats->bytesSent);
            fprintf(fs, "      <td>%lu</td>\n", (unsigned long)atmStats->packetsReceived);
            fprintf(fs, "      <td>%lu</td>\n", (unsigned long)atmStats->packetsSent);
            fprintf(fs, "      <td>%u</td>\n", atmStats->X_BROADCOM_COM_InOAMCells);
            fprintf(fs, "      <td>%u</td>\n", atmStats->X_BROADCOM_COM_OutOAMCells);
            fprintf(fs, "      <td>%u</td>\n", atmStats->X_BROADCOM_COM_InASMCells);
            fprintf(fs, "      <td>%u</td>\n", atmStats->X_BROADCOM_COM_OutASMCells);
            fprintf(fs, "      <td>%u</td>\n", atmStats->discardPacketsReceived);
            fprintf(fs, "      <td>%u</td>\n", atmStats->X_BROADCOM_COM_InCellErrors);
            fprintf(fs, "   </tr>\n");
            cmsObj_free((void **) &atmStats);
         }
         cmsObj_free((void **) &atmLinkObj);
      }
   }
#ifdef DMP_DEVICE2_PTMLINK_1
   else 
   {
      while (cmsObj_getNext(MDMOID_DEV2_PTM_LINK, &iidStack,(void **)&ptmLinkObj) == CMSRET_SUCCESS)
      {
         if (cmsObj_get(MDMOID_DEV2_PTM_LINK_STATS, &iidStack,0,(void **)&ptmStats) == CMSRET_SUCCESS)
         {
            fprintf(fs, "   <tr align='center'>\n");
            fprintf(fs, "      <td>%u</td>\n", ptmLinkObj->X_BROADCOM_COM_PTMPortId);
            fprintf(fs, "      <td>%u</td>\n", (UINT32)ptmStats->bytesReceived);
            fprintf(fs, "      <td>%u</td>\n", (UINT32)ptmStats->bytesSent);
            fprintf(fs, "      <td>%u</td>\n", (UINT32)ptmStats->packetsReceived);
            fprintf(fs, "      <td>%u</td>\n", (UINT32)ptmStats->packetsSent);
            fprintf(fs, "      <td>%u</td>\n", ptmStats->X_BROADCOM_COM_InOAMCells);
            fprintf(fs, "      <td>%u</td>\n", ptmStats->X_BROADCOM_COM_OutOAMCells);
            fprintf(fs, "      <td>%u</td>\n", ptmStats->X_BROADCOM_COM_InASMCells);
            fprintf(fs, "      <td>%u</td>\n", ptmStats->X_BROADCOM_COM_OutASMCells);
            fprintf(fs, "      <td>%u</td>\n", ptmStats->discardPacketsReceived);
            fprintf(fs, "      <td>%u</td>\n", ptmStats->X_BROADCOM_COM_InCellErrors);
            fprintf(fs, "   </tr>\n");
            cmsObj_free((void **) &ptmStats);
         }
         cmsObj_free((void **) &ptmLinkObj);
      }
   }
#endif /* #ifdef DMP_DEVICE2_PTMLINK_1 */
   fprintf(fs, "</table>\n");
   fprintf(fs, "<br><br>\n");

	fprintf(fs, "<p align='center'>\n");
	fprintf(fs, "<input type='button' onClick='btnReset()' value='Reset'> \n");
	fprintf(fs, "</form>\n</center>\n</blockquote>\n</body>\n</html>\n");

	fflush(fs);
}

void cgiResetStatsXtm_dev2(void) 
{
   void *dummy;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 isAtm=FALSE;
   
   dalDsl_displayXtmStatsType_dev2(&isAtm);
   
   if (isAtm)
   {
      while (cmsObj_getNext(MDMOID_DEV2_ATM_LINK_STATS, &iidStack,(void **)&dummy) == CMSRET_SUCCESS)
      {
         cmsObj_clearStatistics(MDMOID_DEV2_ATM_LINK_STATS, &iidStack);
         cmsObj_free((void **) &dummy);
      }
   }
#ifdef DMP_DEVICE2_PTMLINK_1
   else
   {
      while (cmsObj_getNext(MDMOID_DEV2_PTM_LINK_STATS, &iidStack,(void **)&dummy) == CMSRET_SUCCESS)
      {
         cmsObj_clearStatistics(MDMOID_DEV2_PTM_LINK_STATS, &iidStack);
         cmsObj_free((void **) &dummy);
      }
   }
#endif /* DMP_DEVICE2_PTMLINK_1 */
}


#endif /* defined(DMP_DEVICE2_ATMLINK_1) || defined(DMP_DEVICE2_PTMLINK_1) */ 

#endif /* DMP_DEVICE2_BASELINE_1 */
