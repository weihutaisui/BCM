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

#ifdef DMP_DEVICE2_DSL_1


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

void cgiResetStatsXdsl_dev2(void) 
{
   Dev2DslLineStatsTotalObject *dslLineTotalStatsObj = NULL;
   Dev2DslChannelStatsTotalObject *dslChannelTotalStatsObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   while (cmsObj_getNext(MDMOID_DEV2_DSL_LINE_STATS_TOTAL, &iidStack, (void **) &dslLineTotalStatsObj) == CMSRET_SUCCESS)
   {
      cmsObj_clearStatistics(MDMOID_DEV2_DSL_LINE_STATS_TOTAL,&iidStack);
      cmsObj_free((void **) &dslLineTotalStatsObj);
   }
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_DEV2_DSL_CHANNEL_STATS_TOTAL, &iidStack, (void **) &dslChannelTotalStatsObj) == CMSRET_SUCCESS)
   {
      cmsObj_clearStatistics(MDMOID_DEV2_DSL_CHANNEL_STATS_TOTAL,&iidStack);
      cmsObj_free((void **) &dslChannelTotalStatsObj);
   }
}

/* fast and dsl share a lot of these parameters we display on the web site.
 * A lot of the parameters are proprietary.  But the data model for FAST only have a few.
 * So, I am tweaking this by getting the statistically data to DSL line and channel anyway.  Wherever
 * relevant, I will insert the FAST interface's data.
 */
void cgiGetStatsAdsl_dev2(char *varValue) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslLineObject *dslLineObj = NULL;
   Dev2DslChannelObject *dslChannelObj = NULL;
   Dev2DslLineStatsTotalObject *dslLineTotalStatsObj = NULL;
   Dev2DslChannelStatsTotalObject *dslChannelTotalStatsObj = NULL;
   int xDsl2Mode = 0, vdslMode = 0;
   UBOOL8 foundLinkUp=FALSE;
   UBOOL8 foundChannelUp=FALSE;
   MdmPathDescriptor linePathDesc;
   MdmPathDescriptor channelPathDesc;
   char *lineFullPath, *channelFullPath;
   CmsRet ret;
   char modeStr[32];
#ifdef DMP_DEVICE2_FAST_1
   Dev2FastLineObject *fastLineObj=NULL;
   InstanceIdStack fastIidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 gFast = FALSE;
#endif

   cmsLog_debug("entered, bondingLineNum=%d", glbWebVar.bondingLineNum);

   char *p = varValue;

   while ((cmsObj_getNext(MDMOID_DEV2_DSL_LINE, &iidStack, (void **) &dslLineObj) == CMSRET_SUCCESS))
   {
#ifdef DMP_DEVICE2_FAST_1
      if (cmsObj_getNext(MDMOID_DEV2_FAST_LINE, &fastIidStack, (void **) &fastLineObj) != CMSRET_SUCCESS)
      {
         cmsLog_error("Cannot get fast object for this line");
      }
      else
      {
         if (!cmsUtl_strcmp(fastLineObj->status, MDMVS_UP))
         {
            gFast = TRUE;
         }
      }
#endif
      if (((dslLineObj->enable) &&
           (!cmsUtl_strcmp(dslLineObj->status, MDMVS_UP)) &&  
           (dslLineObj->X_BROADCOM_COM_BondingLineNumber == glbWebVar.bondingLineNum)) 
#ifdef DMP_DEVICE2_FAST_1
          ||
          (gFast && (fastLineObj->enable) &&
           (fastLineObj->X_BROADCOM_COM_BondingLineNumber == glbWebVar.bondingLineNum)) 
#endif
          )
      {
         cmsObj_get(MDMOID_DEV2_DSL_LINE_STATS_TOTAL,&iidStack,0,(void **)&dslLineTotalStatsObj);
         /* dslLineObj is still pointing to the object that is up or fastLineObj is up. */
         foundLinkUp = TRUE;

         /* Get the channel that is operational above this line */
         INIT_PATH_DESCRIPTOR(&linePathDesc);
         INIT_PATH_DESCRIPTOR(&channelPathDesc);
         linePathDesc.oid=MDMOID_DEV2_DSL_LINE;
         linePathDesc.iidStack=iidStack;
         cmsMdm_pathDescriptorToFullPathNoEndDot(&linePathDesc,&lineFullPath);

#ifdef DMP_DEVICE2_FAST_1
         if (gFast)
         {
            /* there is no channel in gFast, but we need the data saved in these structures */
            foundChannelUp = qdmDsl_getChannelFullPathFromFastLineFullPathLocked(lineFullPath,&channelFullPath);
         }
         else
         {
            foundChannelUp = qdmDsl_getChannelFullPathFromLineFullPathLocked(lineFullPath,&channelFullPath);
         }
#else
         foundChannelUp = qdmDsl_getChannelFullPathFromLineFullPathLocked(lineFullPath,&channelFullPath);
#endif    

         if (foundChannelUp)
         {
            cmsMdm_fullPathToPathDescriptor(channelFullPath,&channelPathDesc);
            if ((ret = cmsObj_get(channelPathDesc.oid,&(channelPathDesc.iidStack),0,(void **)&dslChannelObj)) 
                != CMSRET_SUCCESS)
            {
               cmsLog_error("Cannot get channel object for this line, ret=%d", ret);
               foundChannelUp=FALSE;
            }
            if ((ret = cmsObj_get(MDMOID_DEV2_DSL_CHANNEL_STATS_TOTAL,&(channelPathDesc.iidStack),0,
                                  (void **)&dslChannelTotalStatsObj))  != CMSRET_SUCCESS)
            {
               if (dslChannelObj != NULL)
               {
                  cmsObj_free((void **) &dslChannelObj);
               }
               cmsLog_error("Cannot get channel object total statistics for this line, ret=%d", ret);
               foundChannelUp=FALSE;
            }
         } /*  channels stats obj retrieved */
         CMSMEM_FREE_BUF_AND_NULL_PTR(lineFullPath);
         CMSMEM_FREE_BUF_AND_NULL_PTR(channelFullPath);
         break;
      } /* line is enabled & up */
      cmsObj_free((void **) &dslLineObj);
#ifdef DMP_DEVICE2_FAST_1
      cmsObj_free((void **) &fastLineObj);
#endif
   } /* while line obj loop */

   /*
    * Now that we have the object, print out the stats.
    */
   if (foundLinkUp && foundChannelUp)
   {
      /* there is AnnexType appended at the end to the standard which we do not care about */
      if ((0 == cmsUtl_strncmp(dslLineObj->standardUsed,"G.992.5",strlen("G.992.5"))) ||
          (0 == cmsUtl_strncmp(dslLineObj->standardUsed,"G.992.3",strlen("G.992.3"))) ||
          (0 == cmsUtl_strncmp(dslLineObj->standardUsed,"G.993.2",strlen("G.993.2"))))
      {
         xDsl2Mode =1;
         vdslMode = (0 == cmsUtl_strncmp(dslLineObj->standardUsed, "G.993.2",strlen("G.993.2"))) ? 1 : 0;
      }
      else
      {
         xDsl2Mode = 0;
      }
      p += sprintf(p, "               <tr>");
      p += sprintf(p, "<td colspan=\"2\" class='hd'>Mode:</td>");
      qdmDsl_getModulationTypeStrLocked(dslLineObj->X_BROADCOM_COM_BondingLineNumber,modeStr);
      p += sprintf(p, "<td>%s</td>",modeStr);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td colspan=\"2\" class='hd'>Traffic Type:</td>");
#ifdef DMP_DEVICE2_FAST_1
      if (gFast)
      {
         p += sprintf(p,"<td>PTM</td>");
      }
      else 
#endif
      {
         if (!cmsUtl_strcmp(dslChannelObj->linkEncapsulationUsed, MDMVS_G_992_3_ANNEX_K_ATM) ||  /* adsl+atm */
             !cmsUtl_strcmp(dslChannelObj->linkEncapsulationUsed, MDMVS_G_993_2_ANNEX_K_ATM))    /* adsl+ptm (unlikely) */
         {
            p += sprintf(p,"<td>ATM</td>");
         }
#ifdef SUPPORT_PTM
         else if (!cmsUtl_strcmp(dslChannelObj->linkEncapsulationUsed, MDMVS_G_993_2_ANNEX_K_PTM))    /* vdsl+ptm */
         {
            p += sprintf(p,"<td>PTM</td>");
         }
#endif
         else
         {
            cmsLog_error("unrecognzied linkEncapsulationUsed %s", dslChannelObj->linkEncapsulationUsed);
            p += sprintf(p,"<td>&nbsp&nbsp;</td>");
         }
      }

      p += sprintf(p,"</tr>");

      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td colspan=\"2\" class='hd'>Status:</td>");
#ifdef DMP_DEVICE2_FAST_1
      if (gFast)
      {
         p += sprintf(p,"<td>%s</td>",fastLineObj->status);
      }
      else
#endif
      {      
         p += sprintf(p,"<td>%s</td>",dslLineObj->status);
      }
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td colspan=\"2\" class='hd'>Link Power State:</td>");
      p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_LinkPowerState);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td colspan=\"3\">&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>&nbsp;</td>");
      p += sprintf(p,"<td class='hd'>Downstream</td>");
      p += sprintf(p,"<td class='hd'>Upstream</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'><nobreak>Line Coding(Trellis):</nobreak></td>");
      if( !xDsl2Mode ) {
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_TrellisD);
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_TrellisD);
      }
      else {
         p += sprintf(p,"<td>%s</td>", dslLineObj->X_BROADCOM_COM_TrellisD);
         p += sprintf(p,"<td>%s</td>", dslLineObj->X_BROADCOM_COM_TrellisU);
      }
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>SNR Margin (0.1 dB):</td>");
      p += sprintf(p,"<td>%d</td>",dslLineObj->downstreamNoiseMargin);
      p += sprintf(p,"<td>%d</td>",dslLineObj->upstreamNoiseMargin);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Attenuation (0.1 dB):</td>");
#ifdef DMP_DEVICE2_VDSL2_1
      p += sprintf(p,"<td>%d</td>",dslLineObj->downstreamAttenuation);
      p += sprintf(p,"<td>%d</td>",dslLineObj->upstreamAttenuation);
#else
      p += sprintf(p,"<td>0</td>");
      p += sprintf(p,"<td>0</td>");
#endif
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Output Power (0.1 dBm):</td>");
      p += sprintf(p,"<td>%d</td>",dslLineObj->downstreamPower);
      p += sprintf(p,"<td>%d</td>",dslLineObj->upstreamPower);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'><nobreak>Attainable Rate (Kbps):</nobreak></td>");
      p += sprintf(p,"<td>%d</td>",dslLineObj->downstreamMaxBitRate);
      p += sprintf(p,"<td>%d</td>",dslLineObj->upstreamMaxBitRate);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr><td colspan=\"5\">&nbsp;</td></tr>\n");
         
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>&nbsp;</td>");
      p += sprintf(p,"<td class='hd'>Path 0</td>");
      p += sprintf(p,"<td class='hd'>&nbsp;</td>");
      p += sprintf(p,"<td class='hd'>Path 1</td>");
      p += sprintf(p,"<td class='hd'>&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>&nbsp;</td>");
      p += sprintf(p,"<td class='hd'>Downstream</td>");
      p += sprintf(p,"<td class='hd'>Upstream</td>");
      p += sprintf(p,"<td class='hd'>Downstream</td>");
      p += sprintf(p,"<td class='hd'>Upstream</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Rate (Kbps):</td>");
      p += sprintf(p,"<td>%d</td>",dslChannelObj->downstreamCurrRate);
      p += sprintf(p,"<td>%d</td>",dslChannelObj->upstreamCurrRate);
      p += sprintf(p,"<td>%d</td>",dslChannelObj->X_BROADCOM_COM_DownstreamCurrRate_2);
      p += sprintf(p,"<td>%d</td>",dslChannelObj->X_BROADCOM_COM_UpstreamCurrRate_2);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr><td colspan=\"5\">&nbsp;</td></tr>\n");
         
      /* xDSL2 framing */
      if ( xDsl2Mode )
      {
         if (!vdslMode)
         {
            p += sprintf(p,"<tr><td class='hd'><nobreak>MSGc (# of bytes in overhead channel message):</nobreak></td>");
            p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamMSGc);
            p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamMSGc);
            p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamMSGc_2);
            p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamMSGc_2);
            p += sprintf(p,"</tr>");
         }
         
         p += sprintf(p,"<tr><td class='hd'><nobreak>B (# of bytes in Mux Data Frame):</nobreak></td>");
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamB);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamB);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamB_2);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamB_2);
         p += sprintf(p,"</tr>");
         
         if (vdslMode)
            p += sprintf(p,"<tr><td class='hd'><nobreak>M (# of Mux Data Frames in an RS codeword):</nobreak></td>");
         else
            p += sprintf(p,"<tr><td class='hd'><nobreak>M (# of Mux Data Frames in FEC Data Frame):</nobreak></td>");
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamM);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamM);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamM_2);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamM_2);
         p += sprintf(p,"</tr>");
         
         if (vdslMode)
            p += sprintf(p,"<tr><td class='hd'><nobreak>T (# of Mux Data Frames in an OH sub-frame):</nobreak></td>");
         else
            p += sprintf(p,"<tr><td class='hd'><nobreak>T (Mux Data Frames over sync bytes):</nobreak></td>");
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamT);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamT);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamT_2);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamT_2);
         p += sprintf(p,"</tr>");
         
         if (vdslMode)
            p += sprintf(p,"<tr><td class='hd'><nobreak>R (# of redundancy bytes in the RS codeword):</nobreak></td>");
         else
            p += sprintf(p,"<tr><td class='hd'><nobreak>R (# of check bytes in FEC Data Frame):</nobreak></td>");
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamR);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamR);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamR_2);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamR_2);
         p += sprintf(p,"</tr>");
         
         if (vdslMode)
            p += sprintf(p,"<tr><td class='hd'><nobreak>S (# of data symbols over which the RS code word spans):</nobreak></td>");
         else
            p += sprintf(p,"<tr><td class='hd'><nobreak>S (ratio of FEC over PMD Data Frame length):</nobreak></td>");
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamS);
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamS);
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamS_2);
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamS_2);
         p += sprintf(p,"</tr>");
         
         if (vdslMode)
            p += sprintf(p,"<tr><td class='hd'><nobreak>L (# of bits transmitted in each data symbol):</nobreak></td>");
         else
            p += sprintf(p,"<tr><td class='hd'><nobreak>L (# of bits in PMD Data Frame):</nobreak></td>");
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamL);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamL);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_DownstreamL_2);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_ADSL2_UpstreamL_2);
         p += sprintf(p,"</tr>");
         
         p += sprintf(p,"<tr><td class='hd'><nobreak>D (interleaver depth):</nobreak></td>");
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_DownstreamD);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_UpstreamD);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_DownstreamD_2);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_UpstreamD_2);
         p += sprintf(p,"</tr>");

         if (vdslMode)
         {
#ifdef DMP_VDSL2WAN_1
            p += sprintf(p,"<tr><td class='hd'><nobreak>I (interleaver block size in bytes):</nobreak></td>");
            p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_VDSL_DownstreamI);
            p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_VDSL_UpstreamI);
            p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_VDSL_DownstreamI_2);
            p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_VDSL_UpstreamI_2);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr><td class='hd'><nobreak>N (RS codeword size):</nobreak></td>");
            p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_VDSL_DownstreamN);
            p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_VDSL_UpstreamN);
            p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_VDSL_DownstreamN_2);
            p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_VDSL_UpstreamN_2);
            p += sprintf(p,"</tr>");
#endif /* DMP_VDSL2WAN_1 */
               
         }

         p += sprintf(p,"<tr><td class='hd'><nobreak>Delay (msec):</nobreak></td>");  
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_DownstreamDelay);
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_UpstreamDelay);
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_DownstreamDelay_2);
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_UpstreamDelay_2);
         p += sprintf(p,"</tr>");
            
         p += sprintf(p,"<tr><td class='hd'><nobreak>INP (DMT symbol):</nobreak></td>");  
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_DownstreamINP);
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_UpstreamINP);
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_DownstreamINP_2);
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_UpstreamINP_2);
         p += sprintf(p,"</tr>");
      } 
      else
      {
         /* Not xDSL2 */
         p += sprintf(p,"<tr><td class='hd'><nobreak>K (number of bytes in DMT frame):</nobreak></td>");
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_DownstreamK);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_UpstreamK);
         p += sprintf(p,"<td>%d</td>", 0);
         p += sprintf(p,"<td>%d</td>", 0);
         p += sprintf(p,"</tr>");
         p += sprintf(p,"<tr><td class='hd'><nobreak>R (number of check bytes in RS code word):</nobreak></td>");
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_DownstreamR);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_UpstreamR);
         p += sprintf(p,"<td>%d</td>", 0);
         p += sprintf(p,"<td>%d</td>", 0);
         p += sprintf(p,"</tr>");
         p += sprintf(p,"<tr><td class='hd'><nobreak>S (RS code word size in DMT frame):</nobreak></td>");
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_DownstreamS);
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_UpstreamS);
         p += sprintf(p,"<td>%s</td>", "0.0");
         p += sprintf(p,"<td>%s</td>", "0.0");
         p += sprintf(p,"</tr>");
         p += sprintf(p,"<tr><td class='hd'><nobreak>D (interleaver depth):</nobreak></td>");
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_DownstreamD);
         p += sprintf(p,"<td>%d</td>",dslLineObj->X_BROADCOM_COM_UpstreamD);
         p += sprintf(p,"<td>%d</td>", 0);
         p += sprintf(p,"<td>%d</td>", 0);
         p += sprintf(p,"</tr>");
         p += sprintf(p,"<tr><td class='hd'><nobreak>Delay (msec):</nobreak></td>");
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_DownstreamDelay);
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_UpstreamDelay);
         p += sprintf(p,"<td>%s</td>", "0.0");
         p += sprintf(p,"<td>%s</td>", "0.0");
         p += sprintf(p,"</tr>");
         p += sprintf(p,"<tr><td class='hd'><nobreak>INP (DMT symbol):</nobreak></td>");
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_DownstreamINP);
         p += sprintf(p,"<td>%s</td>",dslLineObj->X_BROADCOM_COM_UpstreamINP);
         p += sprintf(p,"<td>%s</td>", "0.0");
         p += sprintf(p,"<td>%s</td>", "0.0");
         p += sprintf(p,"</tr>");
      } /* not xdsl2 */
   } /* link and channel not up */
   else 
   {
      /* Link Down or channel down; Display empty field values */
      /* mode */
      p += sprintf(p, "<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td colspan=\"2\" class='hd'>Traffic Type:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td colspan=\"2\" class='hd'>Status:</td>");
      p += sprintf(p,"<td>%s</td>",MDMVS_DOWN);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td colspan=\"2\" class='hd'>Link Power State:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td colspan=\"3\">&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>&nbsp;</td>");
      p += sprintf(p,"<td class='hd'>Downstream</td>");
      p += sprintf(p,"<td class='hd'>Upstream</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'><nobreak>Line Coding(Trellis):</nobreak></td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>SNR Margin (0.1 dB):</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Attenuation (0.1 dB):</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Output Power (0.1 dBm):</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'><nobreak>Attainable Rate (Kbps):</nobreak></td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Rate (Kbps):</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
   } /* link and channel not down */

   /* the second half of statistics */
   p += sprintf(p,"<tr>");
   if (foundLinkUp)
   {
      p += sprintf(p,"<td colspan=\"5\">&nbsp;</td>");
   }
   else
      p += sprintf(p,"<td colspan=\"3\">&nbsp;</td>");
   p += sprintf(p,"</tr>");
   p += sprintf(p,"<tr>");

   if (vdslMode)
      p += sprintf(p,"<td class='hd'>OH Frames:</td>");
   else
      p += sprintf(p,"<td class='hd'>Super Frames:</td>");
   
   cmsObj_free((void **) &dslLineObj);
#ifdef DMP_DEVICE2_FAST_1
   cmsObj_free((void **) &fastLineObj);
#endif
   if (!foundChannelUp)
   {
      cmsObj_free((void **) &dslLineTotalStatsObj); 
   }
   /* these are channel Stats Total */
   if (foundChannelUp)
   {
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_ReceiveBlocks);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_TransmitBlocks);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_ReceiveBlocks_2);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_TransmitBlocks_2);
      p += sprintf(p,"</tr>");
      
      p += sprintf(p,"<tr>");
      if (vdslMode)            
         p += sprintf(p,"<td class='hd'>OH Frame Errors:</td>");            
      else            
         p += sprintf(p,"<td class='hd'>Super Frame Errors:</td>");            
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->XTURCRCErrors);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->XTUCCRCErrors);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_XTURCRCErrors_2);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_XTUCCRCErrors_2);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>RS Words:</td>");
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_RxRsWords);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_TxRsWords);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_RxRsWords_2);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_TxRsWords_2);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>RS Correctable Errors:</td>");
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_RxRsCorrectable);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_TxRsCorrectable);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_RxRsCorrectable_2);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_TxRsCorrectable_2);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>RS Uncorrectable Errors:</td>");
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_RxRsUncorrectable);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_TxRsUncorrectable);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_RxRsUncorrectable_2);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_TxRsUncorrectable_2);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td colspan=\"5\">&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>HEC Errors:</td>");
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->XTURHECErrors);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->XTUCHECErrors);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_XTURHECErrors_2);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_XTUCHECErrors_2);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>OCD Errors:</td>");
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_DownstreamOCD);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_UpstreamOCD);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_DownstreamOCD_2);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_UpstreamOCD_2);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>LCD Errors:</td>");
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_DownstreamLCD);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_UpstreamLCD);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_DownstreamLCD_2);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_UpstreamLCD_2);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Total Cells:</td>");
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_DownstreamTotalCells);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_UpstreamTotalCells);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_DownstreamTotalCells_2);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_UpstreamTotalCells_2);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Data Cells:</td>");
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_DownstreamDataCells);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_UpstreamDataCells);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_DownstreamDataCells_2);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_UpstreamDataCells_2);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Bit Errors:</td>");
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_DownstreamBitErrors);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_UpstreamBitErrors);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_DownstreamBitErrors_2);
      p += sprintf(p,"<td>%u</td>",dslChannelTotalStatsObj->X_BROADCOM_COM_UpstreamBitErrors_2);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td colspan=\"5\">&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Total ES:</td>");
      p += sprintf(p,"<td>%u</td>",dslLineTotalStatsObj->erroredSecs);
      p += sprintf(p,"<td>%u</td>",dslLineTotalStatsObj->X_BROADCOM_COM_UpstreamEs);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Total SES:</td>");
      p += sprintf(p,"<td>%u</td>",dslLineTotalStatsObj->severelyErroredSecs);
      p += sprintf(p,"<td>%u</td>",dslLineTotalStatsObj->X_BROADCOM_COM_UpstreamSes);
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Total UAS:</td>");
      p += sprintf(p,"<td>%u</td>",dslLineTotalStatsObj->X_BROADCOM_COM_DownstreamUas);
      p += sprintf(p,"<td>%u</td>",dslLineTotalStatsObj->X_BROADCOM_COM_UpstreamUas);
      p += sprintf(p,"</tr>");
      
      cmsObj_free((void **) &dslChannelTotalStatsObj);
      cmsObj_free((void **) &dslChannelObj);
      cmsObj_free((void **) &dslLineTotalStatsObj); 
   } /* found channel up */
   else 
   {
      /* down */
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Super Frame Errors:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>RS Words:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>RS Correctable Errors:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>RS Uncorrectable Errors:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td colspan=\"3\">&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<td class='hd'>HEC Errors:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>OCD Errors:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>LCD Errors:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Total Cells:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Data Cells:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Bit Errors:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td colspan=\"3\">&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Total ES:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Total SES:</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
      p += sprintf(p,"<tr>");
      p += sprintf(p,"<td class='hd'>Total UAS:</td>");
      /* need to add to data model */
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"<td>&nbsp&nbsp;</td>");
      p += sprintf(p,"</tr>");
   } /* channel down */

   cmsLog_debug("done");
}

#endif /* DMP_DEVICE2_BASELINE_1 */

#endif /* DMP_DEVICE2_DSL_1 */
