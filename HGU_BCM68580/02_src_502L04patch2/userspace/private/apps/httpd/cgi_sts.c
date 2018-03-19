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

extern int glbReset;              // flag for atm vcc reset


#define  RX      0
#define  TX      1

extern void reset_qMgmtQueueStats(void);

#ifdef SUPPORT_DSL

// dynamically creates statsxtm.cmd page
//
//**************************************************************************
// Function Name: writeStsXtmScript
// Description  : write the javascript port of the statsxtm.cmd
// Returns      : None.
//**************************************************************************
void writeStsXtmScript(FILE *fs) {
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   fprintf(fs, "function btnReset() {\n");
   fprintf(fs, "   var code = 'location=\"' + 'statsxtmreset.html' + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}

void cgiResetStsXtm(void) 
{
   cgiResetStatsXtm();
}

void cgiResetStatsXtm_igd(void) 
{
   void *dummy;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   while (cmsObj_getNext(MDMOID_XTM_INTERFACE_STATS, &iidStack,(void **)&dummy) == CMSRET_SUCCESS)
   {
      cmsObj_clearStatistics(MDMOID_XTM_INTERFACE_STATS, &iidStack);
      cmsObj_free((void **) &dummy);
   }
}

void cgiResetStsAdsl(void) 
{
   cgiResetStatsXdsl();
}

void cgiResetStatsXdsl_igd()
{
   void *dslIntfObj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   while (cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfObj) == CMSRET_SUCCESS)
   {
      cmsObj_clearStatistics(MDMOID_WAN_DSL_INTF_STATS_TOTAL,&iidStack);
      cmsObj_free((void **) &dslIntfObj);
   }
}


void cgiGetStsAdsl(int argc __attribute((unused)), char **argv __attribute((unused)), char *varValue) 
{
   cgiGetStatsAdsl(varValue);
}

#ifdef DMP_ADSLWAN_1
void cgiGetStatsAdsl_igd(char *varValue) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfObj = NULL;
   WanDslIntfStatsTotalObject *dslTotalStatsObj = NULL;
   int xDsl2Mode = 0, vdslMode = 0;
   UBOOL8 foundLinkUp=FALSE;

   cmsLog_debug("entered, bondingLineNum=%d", glbWebVar.bondingLineNum);

   char *p = varValue;

   /*
    * find the appropriate WANDSLIntfCfg object to print stats from.
    * In the most common case, the bonding line number is 0 (even when bonding
    * is not enabled).  Then find the first WANDSLIntfCfg object that has link up.
    * If bondingLineNumber == 1, then find one of the XTM Bonding WANDslIntfCfg (PTM/ATM based on the availability/activeness).
    */
   if (glbWebVar.bondingLineNum == 0)
   {
      /*
       * On the 6368, there are 2 WAN DSLIntfCfg objects, one for ATM and one for PTM.
       * I don't know which one will have link up.  So if I find one that has link up,
       * use that one.  Otherwise, use the first one, which is the ATM one (and it 
       * will still have link down.)
       */
      while ((cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfObj) == CMSRET_SUCCESS))
      {
         if ((dslIntfObj->enable) &&
             (!cmsUtl_strcmp(dslIntfObj->status, MDMVS_UP)) &&
             (dslIntfObj->X_BROADCOM_COM_BondingLineNumber == 0))
         {
            foundLinkUp = TRUE;
            /* dslIntfObj is still pointing to the object that is up. */
            break;
         }

         cmsObj_free((void **) &dslIntfObj);
      }

      if (!foundLinkUp)
      {
         /*
          * WAN link is not up, so just get the first instance of the DSLIntfObj
          * and report based on that object (which currently has link down anyways).
          */
         INIT_INSTANCE_ID_STACK(&iidStack);
         cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfObj);
      }
   }
   else
   {
#ifdef DMP_X_BROADCOM_COM_DSLBONDING_1
      CmsRet ret;

		/*
       * On the 6368, there are 2 WAN DSL bonding IntfCfg objects, one for ATM and one for PTM.
       * I don't know which one will have link up.  So if I find one that has link up,
       * use that one.  Otherwise, use the PTM bonding instance, which will still have link
		 * down.)
       */
      while ((cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfObj) == CMSRET_SUCCESS))
      {
         if ((dslIntfObj->enable) &&
             (!cmsUtl_strcmp(dslIntfObj->status, MDMVS_UP)) &&
             (dslIntfObj->X_BROADCOM_COM_BondingLineNumber == 1))
         {
            foundLinkUp = TRUE;
            /* dslIntfObj is still pointing to the object that is up. */
            break;
         }

         cmsObj_free((void **) &dslIntfObj);
      }

      if (!foundLinkUp)
      {
			/*
			 * WAN link is not up, so just get the ptm bonding dsl intf instance of the DSLIntfObj
			 * and report based on that object (which currently has link down anyways).
			 * PTM bonding instance followerd by atm bonding, which ever
			 * available first.
			 */

			ret = dalDsl_getBondingPtmDslIntfObject(&iidStack, &dslIntfObj);
			if (ret != CMSRET_SUCCESS)
			{
				cmsLog_error("could not get PTM bonding WANDslIntfCfg object, ret=%d", ret);
				ret = dalDsl_getBondingAtmDslIntfObject(&iidStack, &dslIntfObj);
				if (ret != CMSRET_SUCCESS)
				{
					cmsLog_error("could not get ATM bonding WANDslIntfCfg object, ret=%d", ret);
					return;
				}
			}
      } /* if (!foundLinkUp) */
#else

      cmsLog_error("cannot get PTM bonding WANDslIntfCfg object, bonding not compiled in");
      return;
#endif  /* DMP_X_BROADCOM_COM_DSLBONDING_1 */
   }


   /*
    * Now that we have the DSLIntfCfg object, print out the stats.
    */
   {
      cmsLog_debug("found instance %s", cmsMdm_dumpIidStack(&iidStack));

      if ((0 == cmsUtl_strcmp(dslIntfObj->modulationType,MDMVS_ADSL_2PLUS)) ||
          (0 == cmsUtl_strcmp(dslIntfObj->modulationType,MDMVS_ADSL_G_DMT_BIS)) ||
          (0 == cmsUtl_strcmp(dslIntfObj->modulationType,MDMVS_VDSL2)))
      {
         xDsl2Mode =1;
         vdslMode = (0 == cmsUtl_strcmp(dslIntfObj->modulationType, MDMVS_VDSL2)) ? 1 : 0;
      }
      else
      {
         xDsl2Mode = 0;
      }

      p += sprintf(p, "               <tr>");
      p += sprintf(p, "<td colspan=\"2\" class='hd'>Mode:</td>");

      if (cmsUtl_strcmp(dslIntfObj->status,MDMVS_UP) == 0)
      {
         /* mode */
         p += sprintf(p, "<td>%s</td>",dslIntfObj->modulationType);
         p += sprintf(p,"</tr>");

         p += sprintf(p,"<tr>");
         p += sprintf(p,"<td colspan=\"2\" class='hd'>Traffic Type:</td>");
         if (!cmsUtl_strcmp(dslIntfObj->linkEncapsulationUsed, MDMVS_G_992_3_ANNEX_K_ATM) ||  /* adsl+atm */
             !cmsUtl_strcmp(dslIntfObj->linkEncapsulationUsed, MDMVS_G_993_2_ANNEX_K_ATM))    /* adsl+ptm (unlikely) */
         {
            p += sprintf(p,"<td>ATM</td>");
         }
#ifdef SUPPORT_PTM
         else if (!cmsUtl_strcmp(dslIntfObj->linkEncapsulationUsed, MDMVS_G_993_2_ANNEX_K_PTM))    /* vdsl+ptm */
         {
            p += sprintf(p,"<td>PTM</td>");
         }
#endif
         else
         {
            cmsLog_error("unrecognzied linkEncapsulationUsed %s", dslIntfObj->linkEncapsulationUsed);
            p += sprintf(p,"<td>&nbsp&nbsp;</td>");
         }
         p += sprintf(p,"</tr>");

         p += sprintf(p,"<tr>");
         p += sprintf(p,"<td colspan=\"2\" class='hd'>Status:</td>");
         p += sprintf(p,"<td>%s</td>",dslIntfObj->status);
         p += sprintf(p,"</tr>");
         p += sprintf(p,"<tr>");
         p += sprintf(p,"<td colspan=\"2\" class='hd'>Link Power State:</td>");
         p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_LinkPowerState);
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
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_TrellisD);
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_TrellisD);
         }
         else {
            p += sprintf(p,"<td>%s</td>", dslIntfObj->X_BROADCOM_COM_TrellisD);
            p += sprintf(p,"<td>%s</td>", dslIntfObj->X_BROADCOM_COM_TrellisU);
         }
         p += sprintf(p,"</tr>");
         p += sprintf(p,"<tr>");
         p += sprintf(p,"<td class='hd'>SNR Margin (0.1 dB):</td>");
         p += sprintf(p,"<td>%d</td>",dslIntfObj->downstreamNoiseMargin);
         p += sprintf(p,"<td>%d</td>",dslIntfObj->upstreamNoiseMargin);
         p += sprintf(p,"</tr>");
         p += sprintf(p,"<tr>");
         p += sprintf(p,"<td class='hd'>Attenuation (0.1 dB):</td>");
         p += sprintf(p,"<td>%d</td>",dslIntfObj->downstreamAttenuation);
         p += sprintf(p,"<td>%d</td>",dslIntfObj->upstreamAttenuation);
         p += sprintf(p,"</tr>");
         p += sprintf(p,"<tr>");
         p += sprintf(p,"<td class='hd'>Output Power (0.1 dBm):</td>");
         p += sprintf(p,"<td>%d</td>",dslIntfObj->downstreamPower);
         p += sprintf(p,"<td>%d</td>",dslIntfObj->upstreamPower);
         p += sprintf(p,"</tr>");
         p += sprintf(p,"<tr>");
         p += sprintf(p,"<td class='hd'><nobreak>Attainable Rate (Kbps):</nobreak></td>");
         // [JIRA SWBCACPE-10306]: TR-098 requires
         // upstreamMaxRate, downstreamMaxRate
         // in Kbps instead of Bps
         p += sprintf(p,"<td>%d</td>",dslIntfObj->downstreamMaxRate);
         p += sprintf(p,"<td>%d</td>",dslIntfObj->upstreamMaxRate);
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
         // [JIRA SWBCACPE-10306]: TR-098 requires
         // upstreamCurrRate, downstreamCurrRate
         // in Kbps instead of Bps
         p += sprintf(p,"<td>%d</td>",dslIntfObj->downstreamCurrRate);
         p += sprintf(p,"<td>%d</td>",dslIntfObj->upstreamCurrRate);
         p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_DownstreamCurrRate_2);
         p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_UpstreamCurrRate_2);
         p += sprintf(p,"</tr>");
         
         p += sprintf(p,"<tr><td colspan=\"5\">&nbsp;</td></tr>\n");
         
         /* xDSL2 framing */
         if ( xDsl2Mode )
         {
            if (!vdslMode)
            {
               p += sprintf(p,"<tr><td class='hd'><nobreak>MSGc (# of bytes in overhead channel message):</nobreak></td>");
               p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamMSGc);
               p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamMSGc);
               p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamMSGc_2);
               p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamMSGc_2);
               p += sprintf(p,"</tr>");
            }

            p += sprintf(p,"<tr><td class='hd'><nobreak>B (# of bytes in Mux Data Frame):</nobreak></td>");
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamB);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamB);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamB_2);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamB_2);
            p += sprintf(p,"</tr>");

            if (vdslMode)
               p += sprintf(p,"<tr><td class='hd'><nobreak>M (# of Mux Data Frames in an RS codeword):</nobreak></td>");
            else
               p += sprintf(p,"<tr><td class='hd'><nobreak>M (# of Mux Data Frames in FEC Data Frame):</nobreak></td>");
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamM);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamM);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamM_2);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamM_2);
            p += sprintf(p,"</tr>");
            
            if (vdslMode)
               p += sprintf(p,"<tr><td class='hd'><nobreak>T (# of Mux Data Frames in an OH sub-frame):</nobreak></td>");
            else
               p += sprintf(p,"<tr><td class='hd'><nobreak>T (Mux Data Frames over sync bytes):</nobreak></td>");
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamT);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamT);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamT_2);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamT_2);
            p += sprintf(p,"</tr>");

            if (vdslMode)
               p += sprintf(p,"<tr><td class='hd'><nobreak>R (# of redundancy bytes in the RS codeword):</nobreak></td>");
            else
               p += sprintf(p,"<tr><td class='hd'><nobreak>R (# of check bytes in FEC Data Frame):</nobreak></td>");
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamR);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamR);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamR_2);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamR_2);
            p += sprintf(p,"</tr>");

            if (vdslMode)
               p += sprintf(p,"<tr><td class='hd'><nobreak>S (# of data symbols over which the RS code word spans):</nobreak></td>");
            else
               p += sprintf(p,"<tr><td class='hd'><nobreak>S (ratio of FEC over PMD Data Frame length):</nobreak></td>");
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamS);
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamS);
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamS_2);
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamS_2);
            p += sprintf(p,"</tr>");

            if (vdslMode)
               p += sprintf(p,"<tr><td class='hd'><nobreak>L (# of bits transmitted in each data symbol):</nobreak></td>");
            else
               p += sprintf(p,"<tr><td class='hd'><nobreak>L (# of bits in PMD Data Frame):</nobreak></td>");
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamL);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamL);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_DownstreamL_2);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_ADSL2_UpstreamL_2);
            p += sprintf(p,"</tr>");

            p += sprintf(p,"<tr><td class='hd'><nobreak>D (interleaver depth):</nobreak></td>");
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_DownstreamD);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_UpstreamD);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_DownstreamD_2);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_UpstreamD_2);
            p += sprintf(p,"</tr>");

            if (vdslMode)
            {
#ifdef DMP_VDSL2WAN_1
               p += sprintf(p,"<tr><td class='hd'><nobreak>I (interleaver block size in bytes):</nobreak></td>");
               p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_VDSL_DownstreamI);
               p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_VDSL_UpstreamI);
               p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_VDSL_DownstreamI_2);
               p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_VDSL_UpstreamI_2);
               p += sprintf(p,"</tr>");
               p += sprintf(p,"<tr><td class='hd'><nobreak>N (RS codeword size):</nobreak></td>");
               p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_VDSL_DownstreamN);
               p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_VDSL_UpstreamN);
               p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_VDSL_DownstreamN_2);
               p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_VDSL_UpstreamN_2);
               p += sprintf(p,"</tr>");
#endif /* DMP_VDSL2WAN_1 */
               
            }

            p += sprintf(p,"<tr><td class='hd'><nobreak>Delay (msec):</nobreak></td>");  
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_DownstreamDelay);
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_UpstreamDelay);
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_DownstreamDelay_2);
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_UpstreamDelay_2);
            p += sprintf(p,"</tr>");

            p += sprintf(p,"<tr><td class='hd'><nobreak>INP (DMT symbol):</nobreak></td>");  
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_DownstreamINP);
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_UpstreamINP);
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_DownstreamINP_2);
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_UpstreamINP_2);
            p += sprintf(p,"</tr>");
         }
         else
         {
            /* Not xDSL2 */
            p += sprintf(p,"<tr><td class='hd'><nobreak>K (number of bytes in DMT frame):</nobreak></td>");
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_DownstreamK);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_UpstreamK);
            p += sprintf(p,"<td>%d</td>", 0);
            p += sprintf(p,"<td>%d</td>", 0);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr><td class='hd'><nobreak>R (number of check bytes in RS code word):</nobreak></td>");
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_DownstreamR);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_UpstreamR);
            p += sprintf(p,"<td>%d</td>", 0);
            p += sprintf(p,"<td>%d</td>", 0);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr><td class='hd'><nobreak>S (RS code word size in DMT frame):</nobreak></td>");
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_DownstreamS);
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_UpstreamS);
            p += sprintf(p,"<td>%s</td>", "0.0");
            p += sprintf(p,"<td>%s</td>", "0.0");
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr><td class='hd'><nobreak>D (interleaver depth):</nobreak></td>");
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_DownstreamD);
            p += sprintf(p,"<td>%d</td>",dslIntfObj->X_BROADCOM_COM_UpstreamD);
            p += sprintf(p,"<td>%d</td>", 0);
            p += sprintf(p,"<td>%d</td>", 0);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr><td class='hd'><nobreak>Delay (msec):</nobreak></td>");
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_DownstreamDelay);
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_UpstreamDelay);
            p += sprintf(p,"<td>%s</td>", "0.0");
            p += sprintf(p,"<td>%s</td>", "0.0");
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr><td class='hd'><nobreak>INP (DMT symbol):</nobreak></td>");
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_DownstreamINP);
            p += sprintf(p,"<td>%s</td>",dslIntfObj->X_BROADCOM_COM_UpstreamINP);
            p += sprintf(p,"<td>%s</td>", "0.0");
            p += sprintf(p,"<td>%s</td>", "0.0");
            p += sprintf(p,"</tr>");
         }
      } /* link up */
      else 
      {
         /* Link Down; Display empty field values */
         /* mode */
         p += sprintf(p, "<td>&nbsp&nbsp;</td>");
         p += sprintf(p,"</tr>");
         p += sprintf(p,"<tr>");
         p += sprintf(p,"<td colspan=\"2\" class='hd'>Traffic Type:</td>");
         p += sprintf(p,"<td>&nbsp&nbsp;</td>");
         p += sprintf(p,"</tr>");

         p += sprintf(p,"<tr>");
         p += sprintf(p,"<td colspan=\"2\" class='hd'>Status:</td>");
         p += sprintf(p,"<td>%s</td>",dslIntfObj->status);
         p += sprintf(p,"</tr>");
         p += sprintf(p,"<tr>");
         p += sprintf(p,"<td colspan=\"2\" class='hd'>Link Power State:</td>");
         p += sprintf(p,"<td><%s></td>",dslIntfObj->X_BROADCOM_COM_LinkPowerState);
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
      } /* down */

      p += sprintf(p,"<tr>");
      if (cmsUtl_strcmp(dslIntfObj->status,MDMVS_UP) == 0)
         p += sprintf(p,"<td colspan=\"5\">&nbsp;</td>");
      else
         p += sprintf(p,"<td colspan=\"3\">&nbsp;</td>");
      p += sprintf(p,"</tr>");
      
      p += sprintf(p,"<tr>");
      if (vdslMode)
         p += sprintf(p,"<td class='hd'>OH Frames:</td>");
      else
         p += sprintf(p,"<td class='hd'>Super Frames:</td>");

      if (cmsUtl_strcmp(dslIntfObj->status,MDMVS_UP) == 0)
      {
         if (cmsObj_get(MDMOID_WAN_DSL_INTF_STATS_TOTAL, &iidStack, 0, (void **) &dslTotalStatsObj) == CMSRET_SUCCESS)
         {
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->receiveBlocks);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->transmitBlocks);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_ReceiveBlocks_2);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_TransmitBlocks_2);
            p += sprintf(p,"</tr>");
            
            p += sprintf(p,"<tr>");
            if (vdslMode)            
               p += sprintf(p,"<td class='hd'>OH Frame Errors:</td>");            
            else            
               p += sprintf(p,"<td class='hd'>Super Frame Errors:</td>");            
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->CRCErrors);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->ATUCCRCErrors);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_CRCErrors_2);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_ATUCCRCErrors_2);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td class='hd'>RS Words:</td>");
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_RxRsWords);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_TxRsWords);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_RxRsWords_2);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_TxRsWords_2);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td class='hd'>RS Correctable Errors:</td>");
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_RxRsCorrectable);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_TxRsCorrectable);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_RxRsCorrectable_2);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_TxRsCorrectable_2);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td class='hd'>RS Uncorrectable Errors:</td>");
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_RxRsUncorrectable);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_TxRsUncorrectable);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_RxRsUncorrectable_2);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_TxRsUncorrectable_2);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td colspan=\"5\">&nbsp;</td>");
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td class='hd'>HEC Errors:</td>");
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->HECErrors);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->ATUCHECErrors);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_HECErrors_2);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_ATUCHECErrors_2);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td class='hd'>OCD Errors:</td>");
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_DownstreamOCD);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_UpstreamOCD);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_DownstreamOCD_2);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_UpstreamOCD_2);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td class='hd'>LCD Errors:</td>");
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_DownstreamLCD);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_UpstreamLCD);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_DownstreamLCD_2);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_UpstreamLCD_2);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td class='hd'>Total Cells:</td>");
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_DownstreamTotalCells);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_UpstreamTotalCells);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_DownstreamTotalCells_2);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_UpstreamTotalCells_2);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td class='hd'>Data Cells:</td>");
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_DownstreamDataCells);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_UpstreamDataCells);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_DownstreamDataCells_2);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_UpstreamDataCells_2);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td class='hd'>Bit Errors:</td>");
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_DownstreamBitErrors);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_UpstreamBitErrors);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_DownstreamBitErrors_2);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_UpstreamBitErrors_2);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td colspan=\"5\">&nbsp;</td>");
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td class='hd'>Total ES:</td>");
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->erroredSecs);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_UpstreamEs);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td class='hd'>Total SES:</td>");
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->severelyErroredSecs);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_UpstreamSes);
            p += sprintf(p,"</tr>");
            p += sprintf(p,"<tr>");
            p += sprintf(p,"<td class='hd'>Total UAS:</td>");
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_DownstreamUas);
            p += sprintf(p,"<td>%u</td>",dslTotalStatsObj->X_BROADCOM_COM_UpstreamUas);
            p += sprintf(p,"</tr>");
            
            cmsObj_free((void **) &dslTotalStatsObj);
         } /* total Stats obj */
      } /* up */
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
      } /* down */
      cmsObj_free((void **) &dslIntfObj);
   } /* dslIntfObj */

   cmsLog_debug("done");
}

#endif /*  DMP_ADSLWAN_1 */

#endif /* SUPPORT_DSL */

void cgiDisplayWanStatusEntry(FILE *fs, char *srvcStr,
             char *ifcStr, char *protoStr, SINT32 vlanId __attribute((unused)),
             UBOOL8 ipv6Enabled __attribute__((unused)),
             UBOOL8 igmpEnabled, UBOOL8 igmpSourceEnabled, UBOOL8 natEnabled, UBOOL8 fwEnabled,
             char *status, char *ipAddr,
             UBOOL8 mldEnabled __attribute__((unused)),
             UBOOL8 mldSourceEnabled __attribute__((unused)),
             char *ipv6status, char *ipv6Addr __attribute__((unused)))
{
   fprintf(fs, "   <tr align='center'>\n");

   /* interface name */
   fprintf(fs, "      <td>%s</td>\n", ifcStr);

   /* service name */
   if (srvcStr[0] != '\0')
      fprintf(fs, "      <td>%s</td>\n", srvcStr);
   else
      fprintf(fs, "      <td>&nbsp;</td>\n");

   /* protocal */
   fprintf(fs, "      <td>%s</td>\n", protoStr);
   if (!cmsUtl_strcmp(protoStr, MDMVS_IP_BRIDGED))
   {
      strncpy(protoStr,  BRIDGE_PROTO_STR, sizeof(protoStr)-1);
   }

#ifdef SUPPORT_WANVLANMUX
   if (vlanId == VLANMUX_DISABLE)
   {
      fprintf(fs, "      <td>%s</td>\n", MDMVS_DISABLED);
   }
   else
   {
      fprintf(fs, "      <td>%d</td>\n", vlanId);
   }
#endif
#ifdef SUPPORT_IPV6
   fprintf(fs, "      <td>%s</td>\n", ipv6Enabled? MDMVS_ENABLED : MDMVS_DISABLED);
#endif

   fprintf(fs, "      <td>%s</td>\n", igmpEnabled? MDMVS_ENABLED : MDMVS_DISABLED);
   fprintf(fs, "      <td>%s</td>\n", igmpSourceEnabled? MDMVS_ENABLED : MDMVS_DISABLED);
#ifdef SUPPORT_IPV6
   fprintf(fs, "      <td>%s</td>\n", mldEnabled? MDMVS_ENABLED : MDMVS_DISABLED);
   fprintf(fs, "      <td>%s</td>\n", mldSourceEnabled? MDMVS_ENABLED : MDMVS_DISABLED);
#endif
   fprintf(fs, "      <td>%s</td>\n", natEnabled? MDMVS_ENABLED : MDMVS_DISABLED);
   fprintf(fs, "      <td>%s</td>\n", fwEnabled? MDMVS_ENABLED : MDMVS_DISABLED);
   fprintf(fs, "      <td>%s</td>\n", status);
   fprintf(fs, "      <td>%s</td>\n", ipAddr);
#ifdef SUPPORT_IPV6
   fprintf(fs, "      <td>%s</td>\n", ipv6status);
   fprintf(fs, "      <td>%s</td>\n", ipv6Addr);
#endif
   fprintf(fs, "   </tr>\n");

}


#ifdef DMP_BASELINE_1

void cgiDeviceInfoWanStatusBody_igd(FILE *fs)
{
   WanDevObject *wanDev=NULL;
   InstanceIdStack wanDevIid=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *pppCon = NULL;
   WanIpConnObject *ipCon = NULL;
   char serviceStr[BUFLEN_32];
   char interfaceStr[BUFLEN_32];
   char protocalStr[BUFLEN_16];
   UBOOL8 ipv6Enabled=FALSE;
   UBOOL8 igmpProxyEnabled=FALSE;
   UBOOL8 igmpSourceEnabled=FALSE;
   UBOOL8 mldProxyEnabled=FALSE;
   UBOOL8 mldSourceEnabled=FALSE;
   char ipv6AddrStrBuf[CMS_IPADDR_LENGTH]={0};
   char ipv6StatusBuf[BUFLEN_32]={0};
   SINT32 vlanId;

   while (cmsObj_getNextFlags(MDMOID_WAN_DEV, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&wanDev) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **)&wanDev);  /* no longer needed */


      /* get the related ipCon obj */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &wanDevIid, &iidStack, (void **)&ipCon) == CMSRET_SUCCESS)
      {
         /* form serviceStr (br_0_2_35) string */
         snprintf(serviceStr, sizeof(serviceStr), "%s", ipCon->name);
         /* form interfaceStr (nas_0_2_35) */
         snprintf(interfaceStr, sizeof(interfaceStr), "%s",
                                       ipCon->X_BROADCOM_COM_IfName);

         /* form protocolStr */
         if (dalWan_isIPoA(&iidStack))
         {
            strcpy(protocalStr, IPOA_PROTO_STR);
         }
         else
         {
            if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_BRIDGED))
            {
               strcpy(protocalStr, BRIDGE_PROTO_STR);
            }
            else if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_ROUTED))
            {
               strcpy(protocalStr, IPOE_PROTO_STR);
            }
         }

         vlanId = dalWan_isVlanMuxEnabled(MDMOID_WAN_IP_CONN, &iidStack) ? ipCon->X_BROADCOM_COM_VlanMuxID : VLANMUX_DISABLE;

         /* Note IPv6 is tricky, this function is called in Hybrid mode,
          * but we still need to look in TR181 tree for IPv6 params.
          * Fortunately, this is all hidden by QDM.
          */
         ipv6Enabled = qdmIpIntf_isIpv6EnabledOnIntfNameLocked(ipCon->X_BROADCOM_COM_IfName);
         mldProxyEnabled = qdmMulti_isMldProxyEnabledOnIntfNameLocked(ipCon->X_BROADCOM_COM_IfName);
         mldSourceEnabled = qdmMulti_isMldSourceEnabledOnIntfNameLocked(ipCon->X_BROADCOM_COM_IfName);
#ifdef DMP_X_BROADCOM_COM_IPV6_1
         if (ipCon->X_BROADCOM_COM_IPv6ConnStatus)
            strncpy(ipv6StatusBuf, ipCon->X_BROADCOM_COM_IPv6ConnStatus, sizeof(ipv6StatusBuf) - 1);
#endif
         qdmIpIntf_getIpv6AddressByNameLocked(ipCon->X_BROADCOM_COM_IfName, ipv6AddrStrBuf);
         igmpProxyEnabled = qdmMulti_isIgmpProxyEnabledOnIntfNameLocked(ipCon->X_BROADCOM_COM_IfName);
         igmpSourceEnabled = qdmMulti_isIgmpSourceEnabledOnIntfNameLocked(ipCon->X_BROADCOM_COM_IfName);

         cgiDisplayWanStatusEntry(fs, serviceStr, interfaceStr, protocalStr, vlanId,
                  ipv6Enabled,
                  igmpProxyEnabled, igmpSourceEnabled,
                  ipCon->NATEnabled, ipCon->X_BROADCOM_COM_FirewallEnabled, ipCon->connectionStatus,
                  ipCon->externalIPAddress,
                  mldProxyEnabled, mldSourceEnabled, ipv6StatusBuf, ipv6AddrStrBuf);

         cmsObj_free((void **)&ipCon);
      }

      /* get the related pppCon obj */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &wanDevIid, &iidStack, (void **)&pppCon) == CMSRET_SUCCESS)
      {
       /* service string (pppoe_0_0_35) */
       snprintf(serviceStr, sizeof(serviceStr), "%s", pppCon->name);
       /* get interfaceStr (nas_0_0_35) */
       snprintf(interfaceStr, sizeof(interfaceStr), "%s",
                                     pppCon->X_BROADCOM_COM_IfName);

       if (dalWan_isPPPoA(&iidStack))
       {
          strcpy(protocalStr, PPPOA_PROTO_STR);
       }
       else
       {
          strcpy(protocalStr, PPPOE_PROTO_STR);
       }

       vlanId = dalWan_isVlanMuxEnabled(MDMOID_WAN_PPP_CONN, &iidStack) ? pppCon->X_BROADCOM_COM_VlanMuxID : VLANMUX_DISABLE;

       /* Note IPv6 is tricky, this function is called in Hybrid mode,
        * but we still need to look in TR181 tree for IPv6 params.
        * Fortunately, this is all hidden by QDM.
        */
       ipv6Enabled = qdmIpIntf_isIpv6EnabledOnIntfNameLocked(pppCon->X_BROADCOM_COM_IfName);
       mldProxyEnabled = qdmMulti_isMldProxyEnabledOnIntfNameLocked(pppCon->X_BROADCOM_COM_IfName);
       mldSourceEnabled = qdmMulti_isMldSourceEnabledOnIntfNameLocked(pppCon->X_BROADCOM_COM_IfName);
#ifdef DMP_X_BROADCOM_COM_IPV6_1
       if (pppCon->X_BROADCOM_COM_IPv6ConnStatus)
          strncpy(ipv6StatusBuf, pppCon->X_BROADCOM_COM_IPv6ConnStatus, sizeof(ipv6StatusBuf) - 1);
#endif
       qdmIpIntf_getIpv6AddressByNameLocked(pppCon->X_BROADCOM_COM_IfName, ipv6AddrStrBuf);
       igmpProxyEnabled = qdmMulti_isIgmpProxyEnabledOnIntfNameLocked(pppCon->X_BROADCOM_COM_IfName);
       igmpSourceEnabled = qdmMulti_isIgmpSourceEnabledOnIntfNameLocked(pppCon->X_BROADCOM_COM_IfName);

       cgiDisplayWanStatusEntry(fs, serviceStr, interfaceStr, protocalStr, vlanId,
             ipv6Enabled,
             igmpProxyEnabled, igmpSourceEnabled,
             pppCon->NATEnabled, pppCon->X_BROADCOM_COM_FirewallEnabled,
             pppCon->connectionStatus, pppCon->externalIPAddress,
             mldProxyEnabled, mldSourceEnabled, ipv6StatusBuf, ipv6AddrStrBuf);

       cmsObj_free((void **)&pppCon);
      }

   }  /* loop over while (WAN_DEV) */
}

#endif  /* DMP_BASELINE_1 */


/*
 * This function displays Device info WAN status 
 */
void cgiDeviceInfoWanStatus(FILE *fs) 
{


   
   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   /* write body */
   fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");

   /* write table */
   fprintf(fs, "<center>\n");
   fprintf(fs, "<b>WAN Info</b><br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");

   /* write table header */
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "      <td class='hd'>Description</td>\n");
   fprintf(fs, "      <td class='hd'>Type</td>\n");
#ifdef SUPPORT_WANVLANMUX
   fprintf(fs, "      <td class='hd'>VlanMuxId</td>\n");
#endif
#ifdef SUPPORT_IPV6
   fprintf(fs, "      <td class='hd'>IPv6</td>\n");
#endif
   fprintf(fs, "      <td class='hd'>Igmp Pxy</td>\n");
   fprintf(fs, "      <td class='hd'>Igmp Src Enbl</td>\n");
#ifdef SUPPORT_IPV6
   fprintf(fs, "      <td class='hd'>MLD Pxy</td>\n");
   fprintf(fs, "      <td class='hd'>MLD Src Enbl</td>\n");
#endif
   fprintf(fs, "      <td class='hd'>NAT</td>\n");
   fprintf(fs, "      <td class='hd'>Firewall</td>\n");
   fprintf(fs, "      <td class='hd'>IPv4 Status</td>\n");
   fprintf(fs, "      <td class='hd'>IPv4 Address</td>\n");
#ifdef SUPPORT_IPV6
   fprintf(fs, "      <td class='hd'>IPv6 Status</td>\n");
   fprintf(fs, "      <td class='hd'>IPv6 Address</td>\n");
#endif
   fprintf(fs, "   </tr>\n");


   /* do data model dependent traversal to dump out the body */
   cgiDeviceInfoWanStatusBody(fs);

   fprintf(fs, "</table><br><br>\n");
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");

   fflush(fs);
}


/*  for interface (eth, usb, wlan) statistics */
void cgiGetStsIfc_igd(char *varValue)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfStatsObject *ethStatsObj=NULL;
   LanEthIntfObject *ethObj = NULL;
#ifdef DMP_USBLAN_1
   LanUsbIntfStatsObject *usbStatsObj=NULL;
   LanUsbIntfObject *usbObj = NULL;
#endif /* DMP_USBLAN_1*/
   char *p = varValue;

   while (cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj) == CMSRET_SUCCESS)
   {
      p += sprintf(p, "               <tr>");
      p += sprintf(p, "                  <td class='hd'>%s</td>", ethObj->X_BROADCOM_COM_IfName);
      if (cmsObj_get(MDMOID_LAN_ETH_INTF_STATS, &iidStack, 0, (void **) &ethStatsObj) == CMSRET_SUCCESS)
      {
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->bytesReceived);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->packetsReceived);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->X_BROADCOM_COM_RxErrors);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->X_BROADCOM_COM_RxDrops);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->X_BROADCOM_COM_MulticastBytesReceived);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->X_BROADCOM_COM_MulticastPacketsReceived);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->X_BROADCOM_COM_UnicastPacketsReceived);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->X_BROADCOM_COM_BroadcastPacketsReceived); 
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->bytesSent);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->packetsSent);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->X_BROADCOM_COM_TxErrors);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->X_BROADCOM_COM_TxDrops);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->X_BROADCOM_COM_MulticastBytesSent);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->X_BROADCOM_COM_MulticastPacketsSent);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->X_BROADCOM_COM_UnicastPacketsSent);
         p += sprintf(p, "                  <td>%u</td>",ethStatsObj->X_BROADCOM_COM_BroadcastPacketsSent);    
         p += sprintf(p, "               </tr>");
         cmsObj_free((void **) &ethStatsObj);
      } /* if eth_itf_stats */
      cmsObj_free((void **) &ethObj);
   }


#ifdef DMP_USBLAN_1
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_LAN_USB_INTF, &iidStack, (void **) &usbObj) == CMSRET_SUCCESS)
   {
      p += sprintf(p, "               <tr>");
      p += sprintf(p, "                  <td class='hd'>%s</td>", usbObj->X_BROADCOM_COM_IfName);
      if (cmsObj_get(MDMOID_LAN_USB_INTF_STATS, &iidStack, 0, (void **) &usbStatsObj) == CMSRET_SUCCESS)
      {
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->bytesReceived);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_PacketsReceived);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_RxErrors);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_RxDrops);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_MulticastBytesReceived);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_MulticastPacketsReceived);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_UnicastPacketsReceived);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_BroadcastPacketsReceived);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->bytesSent);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_PacketsSent);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_TxErrors);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_TxDrops);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_MulticastBytesSent);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_MulticastPacketsSent);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_UnicastPacketsSent);
         p += sprintf(p, "                  <td>%u</td>",usbStatsObj->X_BROADCOM_COM_BroadcastPacketsSent);
         p += sprintf(p, "               </tr>");
         cmsObj_free((void **) &usbStatsObj);
      } /* if usb_itf_stats */
      cmsObj_free((void **) &usbObj);
   }
#endif

#ifdef BRCM_WLAN
   cgiGetStsWifiIfc(&p);
#endif

   *p = 0;

   /* this is potentially a big page.  Buffer is WEB_BUF_SIZE_MAX bytes */
   cmsLog_debug("total bytes written=%d", p-varValue+1);

   return;
}

void cgiGetStsIfc(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue)
{
#if defined(SUPPORT_DM_LEGACY98)
   cgiGetStsIfc_igd(varValue);
#elif defined(SUPPORT_DM_HYBRID)
   cgiGetStsIfc_igd(varValue);
#elif defined(SUPPORT_DM_PURE181)
   cgiGetStsIfc_dev2(varValue);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2() == TRUE)
   {
      cgiGetStsIfc_dev2(varValue);
   }
   else
   {
      cgiGetStsIfc_igd(varValue);
   }
#endif
}

#if defined(DMP_X_BROADCOM_COM_RDPA_1)
/*  for interface (eth, wan) per queue statistics */
void cgiGetStsQueue_igd(char *varValue)
{
   char intfname[CMS_IFNAME_LENGTH];
   InstanceIdStack iidStack;
   QMgmtQueueStatsObject *qStatsObj = NULL;
   CmsRet ret;
   char *p = varValue;

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_QUEUE_STATS, &iidStack, (void **)&qStatsObj)) == CMSRET_SUCCESS)
   {
      memset(intfname, 0, sizeof(intfname));
      if ((ret = qdmIntf_fullPathToIntfnameLocked(qStatsObj->queueInterface, intfname)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmIntf_fullPathToIntfnameLocked returns error. ret=%d", ret);
         cmsObj_free((void **)&qStatsObj);
         continue;
      }
      p += sprintf(p, "<td class='hd'>%s</td>", intfname);
      p += sprintf(p, "<td class='hd'>%d</td>", qStatsObj->X_BROADCOM_COM_QueueId);
      p += sprintf(p, "<td>%u</td>",qStatsObj->outputPackets);
      p += sprintf(p, "<td>%u</td>",qStatsObj->outputBytes);
      p += sprintf(p, "<td>%u</td>",qStatsObj->droppedPackets);
      p += sprintf(p, "<td>%u</td>",qStatsObj->droppedBytes);
      p += sprintf(p, "</tr>");
      cmsObj_free((void **)&qStatsObj);
   }
   *p = 0;

   /* this is potentially a big page.  Buffer is WEB_BUF_SIZE_MAX bytes */
   cmsLog_debug("total bytes written=%d", p-varValue+1);

   return;
}

#if defined(SUPPORT_DM_PURE181)
void cgiGetStsQueue(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue __attribute__((unused)))
#else
void cgiGetStsQueue(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue)
#endif
{
#if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID)
   cgiGetStsQueue_igd(varValue);
#elif defined(SUPPORT_DM_DETECT)
   if (!cmsMdm_isDataModelDevice2())
   {
      cgiGetStsQueue_igd(varValue);
   }
#endif
}

#ifdef SUPPORT_QOS
/* reseting/clearing wan / lan port queue stats */
void cgiResetStsQueue_igd(void)
{
#if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID)
   reset_qMgmtQueueStats();
#elif defined(SUPPORT_DM_DETECT)
   if (!cmsMdm_isDataModelDevice2())
   {
      reset_qMgmtQueueStats();
   }
#endif
}

void cgiResetStsQueue(char *query __attribute__((unused)), FILE *fs)
{
   char path[BUFLEN_1024];
   
#if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID)
   cgiResetStsQueue_igd();
#elif defined(SUPPORT_DM_DETECT)
   if (!cmsMdm_isDataModelDevice2())
   {
      cgiResetStsIfc_igd();
   }
#endif
   makePathToWebPage(path, sizeof(path), "statsqueue.html");
   do_ej(path, fs);
}
#endif
#endif

/* reseting/clearing LAN interfaces' stats */
void cgiResetStsIfc_igd(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *ethObj = NULL;
#ifdef DMP_USBLAN_1
   LanUsbIntfObject *usbObj = NULL;
#endif /* DMP_USBLAN_1 */


   while ((cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj)) == CMSRET_SUCCESS)
   {
      cmsObj_clearStatistics(MDMOID_LAN_ETH_INTF_STATS, &iidStack);
      cmsObj_free((void **) &ethObj);
   }

#ifdef DMP_USBLAN_1
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((cmsObj_getNext(MDMOID_LAN_USB_INTF, &iidStack, (void **) &usbObj)) == CMSRET_SUCCESS)
   {
      cmsObj_clearStatistics(MDMOID_LAN_USB_INTF_STATS, &iidStack);
      cmsObj_free((void **) &usbObj);
   } 
#endif

#ifdef BRCM_WLAN
   cgiResetStsWifiLan();
#endif
}


void cgiResetStsIfc(void)
{
#if defined(SUPPORT_DM_LEGACY98)
   cgiResetStsIfc_igd();
#elif defined(SUPPORT_DM_HYBRID)
   cgiResetStsIfc_igd();
#elif defined(SUPPORT_DM_PURE181)
   cgiResetStsIfc_dev2();
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2() == TRUE)
   {
      cgiResetStsIfc_dev2();
   }
   else
   {
      cgiResetStsIfc_igd();
   }
#endif
}


void cgiResetStsWan_igd(void)
{
   InstanceIdStack commIntfIid = EMPTY_INSTANCE_ID_STACK;
   WanCommonIntfCfgObject *comIntf = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *wanPppConn = NULL;   
   WanIpConnObject  *wanIpConn = NULL;

   /* Loop through all WanDevice.WanCommonInterfaceConfig object to find the EthWan Interface stats to clear */
   while (cmsObj_getNext(MDMOID_WAN_COMMON_INTF_CFG, &commIntfIid, (void **)&comIntf) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_ETHERNET))
      {

         WanEthIntfObject *wanEthIntf=NULL;
         if (cmsObj_get(MDMOID_WAN_ETH_INTF, &commIntfIid, 0, (void **)&wanEthIntf) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_get <MDMOID_WAN_ETH_INTF> returns error.");
            cmsObj_free((void **)&comIntf);
            return;
         }            
         else 
         {
            /* Need to check if this ethWan is configured or not.  If not configured, just skip it */
            if (IS_EMPTY_STRING(wanEthIntf->X_BROADCOM_COM_IfName))
            {
               cmsObj_free((void **)&comIntf);
               cmsObj_free((void **)&wanEthIntf);
               continue;
            }
            else
            {
               WanEthIntfStatsObject *wanEthIntfStat=NULL;

               cmsObj_free((void **)&wanEthIntf);
               if (cmsObj_get(MDMOID_WAN_ETH_INTF_STATS, &commIntfIid, 0, (void **)&wanEthIntfStat) != CMSRET_SUCCESS)
               {
                  cmsLog_error("cmsObj_get <MDMOID_WAN_ETH_INTF_STATS> returns error.");
                  return;
               }  
               cmsObj_clearStatistics(MDMOID_WAN_ETH_INTF_STATS, &commIntfIid);
               cmsObj_free((void **)&wanEthIntfStat);
            }
         }
      }

      cmsObj_free((void **)&comIntf);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   
   /* get the related wanIpConn obj */
   while (cmsObj_getNextFlags(MDMOID_WAN_IP_CONN, &iidStack, OGF_NO_VALUE_UPDATE,
                                  (void **)&wanIpConn) == CMSRET_SUCCESS)
   {
      cmsObj_clearStatistics(MDMOID_WAN_IP_CONN_STATS, &iidStack);
      cmsObj_free((void **) &wanIpConn);
   }
  
   INIT_INSTANCE_ID_STACK(&iidStack);
   /* get the related wanPppConn obj */
   while (cmsObj_getNextFlags(MDMOID_WAN_PPP_CONN, &iidStack, OGF_NO_VALUE_UPDATE,
                                  (void **)&wanPppConn) == CMSRET_SUCCESS)
   {
      cmsObj_clearStatistics(MDMOID_WAN_PPP_CONN_STATS, &iidStack);
      cmsObj_free((void **) &wanPppConn);
   }
   
}

void cgiResetStsWan(void)
{
#if defined(SUPPORT_DM_LEGACY98)
   cgiResetStsWan_igd();
#elif defined(SUPPORT_DM_HYBRID)
   cgiResetStsWan_igd();
#elif defined(SUPPORT_DM_PURE181)
   cgiResetStsWan_dev2();
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2() == TRUE)
   {
      cgiResetStsWan_dev2();
   }
   else
   {
      cgiResetStsWan_igd();
   }
#endif
}



void cgiWriteWanSts(FILE *fs, const char *interfaceDisplayStr, const char *serviceStr,
                      UINT32 rxTotalByte, UINT32 rxTotalPacket, 
                      UINT32 rxTotalErr, UINT32 rxTotalDrop,
                      UINT32 rxMulticastByte, UINT32 rxMulticastPacket,
                      UINT32 rxUnicastPacket, UINT32 rxBroadcastPacket,
                      UINT32 txTotalByte, UINT32 txTotalPacket, 
                      UINT32 txTotalErr, UINT32 txTotalDrop,
                      UINT32 txMulticastByte, UINT32 txMulticastPacket,
                      UINT32 txUnicastPacket, UINT32 txBroadcastPacket)
{
   
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>%s</td>\n", interfaceDisplayStr);
   fprintf(fs, "      <td class='hd'>%s</td>\n", serviceStr);
 
   fprintf(fs, "      <td>%u</td>\n", rxTotalByte);
   fprintf(fs, "      <td>%u</td>\n", rxTotalPacket);
   fprintf(fs, "      <td>%u</td>\n", rxTotalErr);
   fprintf(fs, "      <td>%u</td>\n", rxTotalDrop);
   fprintf(fs, "      <td>%u</td>\n", rxMulticastByte);
   fprintf(fs, "      <td>%u</td>\n", rxMulticastPacket);
   fprintf(fs, "      <td>%u</td>\n", rxUnicastPacket);
   fprintf(fs, "      <td>%u</td>\n", rxBroadcastPacket);
   
   
   fprintf(fs, "      <td>%u</td>\n", txTotalByte);
   fprintf(fs, "      <td>%u</td>\n", txTotalPacket);
   fprintf(fs, "      <td>%u</td>\n", txTotalErr);
   fprintf(fs, "      <td>%u</td>\n", txTotalDrop);
   fprintf(fs, "      <td>%u</td>\n", txMulticastByte);
   fprintf(fs, "      <td>%u</td>\n", txMulticastPacket);
   fprintf(fs, "      <td>%u</td>\n", txUnicastPacket);
   fprintf(fs, "      <td>%u</td>\n", txBroadcastPacket);
   
   
   fprintf(fs, "   </tr>\n");   
}


void cgiStsWanHeadHtml(char *query __attribute__((unused)), FILE *fs) 
{
   
   // write html header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<meta http-equiv='refresh' content='15'>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");
   fprintf(fs, "function resetClick() {\n");
   fprintf(fs, "   var loc = 'statswanreset.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

   // write body
   fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");

   // write table
   fprintf(fs, "<b>Statistics -- WAN</b><br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='0' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd' rowspan='3'>Interface</td>\n");
   fprintf(fs, "      <td class='hd' rowspan='3'>Description</td>\n");
   fprintf(fs, "      <td class='hd' colspan='8'>Received</td>\n");
   fprintf(fs, "      <td class='hd' colspan='8'>Transmitted</td>\n");
   fprintf(fs, "   </tr>\n");
   
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd' colspan='4'>Total</td>\n");
   fprintf(fs, "      <td class='hd' colspan='2'>Multicast</td>\n");
   fprintf(fs, "      <td class='hd' colspan='1'>Unicast</td>\n");
   fprintf(fs, "      <td class='hd' colspan='1'>Broadcast</td>\n");
   fprintf(fs, "      <td class='hd' colspan='4'>Total</td>\n");
   fprintf(fs, "      <td class='hd' colspan='2'>Multicast</td>\n");
   fprintf(fs, "      <td class='hd' colspan='1'>Unicast</td>\n");
   fprintf(fs, "      <td class='hd' colspan='1'>Broadcast</td>\n");
   fprintf(fs, "   </tr>\n");
   
   
   
   
   
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Bytes</td>\n");
   fprintf(fs, "      <td class='hd'>Pkts</td>\n");
   fprintf(fs, "      <td class='hd'>Errs</td>\n");
   fprintf(fs, "      <td class='hd'>Drops</td>\n");
   fprintf(fs, "      <td class='hd'>Bytes</td>\n");
   fprintf(fs, "      <td class='hd'>Pkts</td>\n");
   fprintf(fs, "      <td class='hd'>Pkts</td>\n");
   fprintf(fs, "      <td class='hd'>Pkts</td>\n");
   
   
   fprintf(fs, "      <td class='hd'>Bytes</td>\n");
   fprintf(fs, "      <td class='hd'>Pkts</td>\n");
   fprintf(fs, "      <td class='hd'>Errs</td>\n");
   fprintf(fs, "      <td class='hd'>Drops</td>\n");
   fprintf(fs, "      <td class='hd'>Bytes</td>\n");
   fprintf(fs, "      <td class='hd'>Pkts</td>\n");
   fprintf(fs, "      <td class='hd'>Pkts</td>\n");
   fprintf(fs, "      <td class='hd'>Pkts</td>\n");
   
   
   fprintf(fs, "   </tr>\n");

}



void cgiStsWanFooterHtml(char *query __attribute__((unused)), FILE *fs) 
{
   fprintf(fs, "</table>\n");
   fprintf(fs, "<br><br>\n");
   fprintf(fs, "<input type='button' onClick='resetClick()' value='Reset Statistics'>\n");
   fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");

   fflush(fs);
}



//**************************************************************************
// Function Name: cgiStsWan
// Description  : creates statswan.cmd according to defined pvc in psi
// Returns      : None.
//**************************************************************************
void cgiStsWanBody_igd(char *query __attribute__((unused)), FILE *fs) 
{
   WanDevObject *wanDev;
   InstanceIdStack wanDevIid = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *pppCon = NULL;   
   WanIpConnObject  *ipCon = NULL;
   WanPppConnStatsObject *pppConnStats = NULL; 
   WanIpConnStatsObject *ipConnStats = NULL; 
   CmsRet ret = CMSRET_SUCCESS;


   while (cmsObj_getNext(MDMOID_WAN_DEV, &wanDevIid, (void **)&wanDev) == CMSRET_SUCCESS)
   {
      WanCommonIntfCfgObject *comIntf = NULL;
      
      cmsObj_free((void **)&wanDev);  /* no longer needed */

      /* Find out the types of the WANDevice object */
      if (cmsObj_get(MDMOID_WAN_COMMON_INTF_CFG, &wanDevIid, 0, (void **)&comIntf) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_get <MDMOID_WAN_COMMON_INTF_CFG> returns error.");
         return;
      }

      if (!cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_ETHERNET))
      {
         WanEthIntfObject *wanEthIntf=NULL;
         if (cmsObj_get(MDMOID_WAN_ETH_INTF, &wanDevIid, 0, (void **)&wanEthIntf) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_get <MDMOID_WAN_ETH_INTF> returns error.");
            cmsObj_free((void **)&comIntf);
            return;
         }            
         else 
         {
            /* Need to check if this ethWan is configured or not.  If not configured, just skip it */
            if (IS_EMPTY_STRING(wanEthIntf->X_BROADCOM_COM_IfName))
            {
               cmsObj_free((void **)&comIntf);
               cmsObj_free((void **)&wanEthIntf);
               continue;
            }
            else
            {
               WanEthIntfStatsObject *wanEthIntfStat=NULL;
               
               /* Get the stats from this ethWAN */
               if (cmsObj_get(MDMOID_WAN_ETH_INTF_STATS, &wanDevIid, 0, (void **)&wanEthIntfStat) != CMSRET_SUCCESS)
               {
                  cmsLog_error("cmsObj_get <MDMOID_WAN_ETH_INTF_STATS> returns error.");
                  cmsObj_free((void **)&wanEthIntf);
                  return;
               }  
               cgiWriteWanSts(fs, wanEthIntf->X_BROADCOM_COM_IfName, wanEthIntf->X_BROADCOM_COM_IfName,
                   wanEthIntfStat->bytesReceived, wanEthIntfStat->packetsReceived,
                   wanEthIntfStat->X_BROADCOM_COM_RxErrors, wanEthIntfStat->X_BROADCOM_COM_RxDrops,
                   wanEthIntfStat->X_BROADCOM_COM_MulticastBytesReceived, wanEthIntfStat->X_BROADCOM_COM_MulticastPacketsReceived, wanEthIntfStat->X_BROADCOM_COM_UnicastPacketsReceived, wanEthIntfStat->X_BROADCOM_COM_BroadcastPacketsReceived, 
                   wanEthIntfStat->bytesSent, wanEthIntfStat->packetsSent,
                   wanEthIntfStat->X_BROADCOM_COM_TxErrors, wanEthIntfStat->X_BROADCOM_COM_TxDrops,
                   wanEthIntfStat->X_BROADCOM_COM_MulticastBytesSent, wanEthIntfStat->X_BROADCOM_COM_MulticastPacketsSent, wanEthIntfStat->X_BROADCOM_COM_UnicastPacketsSent, wanEthIntfStat->X_BROADCOM_COM_BroadcastPacketsSent);    
               cmsObj_free((void **)&wanEthIntf);
               cmsObj_free((void **)&wanEthIntfStat);
            }    
         }            
      }

      cmsObj_free((void **)&comIntf);

      /* get the related ipCon obj */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &wanDevIid, &iidStack, (void **)&ipCon) == CMSRET_SUCCESS)
      {
         /* get the statistics*/
         if ((ret = cmsObj_get(MDMOID_WAN_IP_CONN_STATS, &iidStack, 0, (void **)&ipConnStats)) == CMSRET_SUCCESS)
         {
            cgiWriteWanSts(fs, ipCon->X_BROADCOM_COM_IfName, ipCon->name,
                  ipConnStats->ethernetBytesReceived, ipConnStats->ethernetPacketsReceived,
                  ipConnStats->X_BROADCOM_COM_RxErrors, ipConnStats->X_BROADCOM_COM_RxDrops,
                  ipConnStats->X_BROADCOM_COM_MulticastBytesReceived, ipConnStats->X_BROADCOM_COM_MulticastPacketsReceived, ipConnStats->X_BROADCOM_COM_UnicastPacketsReceived, ipConnStats->X_BROADCOM_COM_BroadcastPacketsReceived, 
                  ipConnStats->ethernetBytesSent, ipConnStats->ethernetPacketsSent,
                  ipConnStats->X_BROADCOM_COM_TxErrors, ipConnStats->X_BROADCOM_COM_TxDrops,
                  ipConnStats->X_BROADCOM_COM_MulticastBytesSent, ipConnStats->X_BROADCOM_COM_MulticastPacketsSent, ipConnStats->X_BROADCOM_COM_UnicastPacketsSent, ipConnStats->X_BROADCOM_COM_BroadcastPacketsSent);    
            cmsObj_free((void **)&ipConnStats);
         }
         else
         {
            cgiWriteWanSts(fs, ipCon->X_BROADCOM_COM_IfName, ipCon->name, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);    

            cmsLog_error("Failed to get WAN_IP_CONN_STATS object, ret=%d", ret);
         }
         cmsObj_free((void **)&ipCon);
      }

      INIT_INSTANCE_ID_STACK(&iidStack);
      while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &wanDevIid, &iidStack, (void **)&pppCon) == CMSRET_SUCCESS)
      {

         /* get the statistics*/
         if ((ret = cmsObj_get(MDMOID_WAN_PPP_CONN_STATS, &iidStack, 0, (void **)&pppConnStats)) == CMSRET_SUCCESS)
         {
            cgiWriteWanSts(fs, pppCon->X_BROADCOM_COM_IfName, pppCon->name, 
                  pppConnStats->ethernetBytesReceived, pppConnStats->ethernetPacketsReceived,
                  pppConnStats->X_BROADCOM_COM_RxErrors, pppConnStats->X_BROADCOM_COM_RxDrops,
                  pppConnStats->X_BROADCOM_COM_MulticastBytesReceived, pppConnStats->X_BROADCOM_COM_MulticastPacketsReceived, pppConnStats->X_BROADCOM_COM_UnicastPacketsReceived, pppConnStats->X_BROADCOM_COM_BroadcastPacketsReceived, 
                  pppConnStats->ethernetBytesSent, pppConnStats->ethernetPacketsSent,
                  pppConnStats->X_BROADCOM_COM_TxErrors, pppConnStats->X_BROADCOM_COM_TxDrops,
                  pppConnStats->X_BROADCOM_COM_MulticastBytesSent, pppConnStats->X_BROADCOM_COM_MulticastPacketsSent, pppConnStats->X_BROADCOM_COM_UnicastPacketsSent, pppConnStats->X_BROADCOM_COM_BroadcastPacketsSent);    
            cmsObj_free((void **)&pppConnStats);
         }
         else
         {
            cgiWriteWanSts(fs, pppCon->X_BROADCOM_COM_IfName, pppCon->name, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);    
            cmsLog_error("Failed to get WAN_PPP_CONN_STATS object, ret=%d", ret);

         }
         cmsObj_free((void **)&pppCon);
      }

   }
   
}


//**************************************************************************
// Function Name: cgiStsWan
// Description  : creates statswan.cmd according to defined pvc in psi
// Returns      : None.
//**************************************************************************
void cgiStsWan(char *query __attribute__((unused)), FILE *fs) 
{
   cgiStsWanHeadHtml(query, fs);
   
#if defined(SUPPORT_DM_LEGACY98)
   cgiStsWanBody_igd(query, fs);
#elif defined(SUPPORT_DM_HYBRID)
   cgiStsWanBody_igd(query, fs);
#elif defined(SUPPORT_DM_PURE181)
   cgiStsWanBody_dev2(query, fs);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
   {
      cgiStsWanBody_dev2(query, fs);
   }
   else
   {
      cgiStsWanBody_igd(query, fs);
   }
#endif

   cgiStsWanFooterHtml(query, fs);

}

#ifdef SUPPORT_DSL
void cgiStsXtm(char *query __attribute__((unused)), FILE *fs)
{
   cgiDisplayStsXtm(fs);
}

void cgiDisplayStsXtm_igd(FILE *fs)
{
	InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   XtmInterfaceStatsObject *xtmStats = NULL;
   UINT32 nextPort=1;

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


   while (cmsObj_getNext(MDMOID_XTM_INTERFACE_STATS, &iidStack,(void **)&xtmStats) == CMSRET_SUCCESS)
   {
      /* there are maximum 4 ports (6368, and 2 are used) under a WANDevice.  We have 2 instances of
       * WANDevice (one is used to represent ATM mode, and another is PTM mode).   The SAR interfaces
       * are added under each WANDevice, port numbers are incremental.
       * When the interfaces (ports) are down, all the MIBs counters are 0.  The XTM driver right now doesn't
       * accumulate the counters.
       */
      if (xtmStats->port < nextPort)
      {
         cmsObj_free((void **) &xtmStats);
         break;
      }
      else
      {
         nextPort = xtmStats->port + 1;
      }

      if (!cmsUtl_strcmp(xtmStats->status,MDMVS_ENABLED))
      {
         fprintf(fs, "   <tr align='center'>\n");
         fprintf(fs, "      <td>%u</td>\n", xtmStats->port);
         fprintf(fs, "      <td>%u</td>\n", xtmStats->inOctets);
         fprintf(fs, "      <td>%u</td>\n", xtmStats->outOctets);
         fprintf(fs, "      <td>%u</td>\n", xtmStats->inPackets);
         fprintf(fs, "      <td>%u</td>\n", xtmStats->outPackets);
         fprintf(fs, "      <td>%u</td>\n", xtmStats->inOAMCells);
         fprintf(fs, "      <td>%u</td>\n", xtmStats->outOAMCells);
         fprintf(fs, "      <td>%u</td>\n", xtmStats->inASMCells);
         fprintf(fs, "      <td>%u</td>\n", xtmStats->outASMCells);
         fprintf(fs, "      <td>%u</td>\n", xtmStats->inPacketErrors);
         fprintf(fs, "      <td>%u</td>\n", xtmStats->inCellErrors);
         fprintf(fs, "   </tr>\n");
      }
      cmsObj_free((void **) &xtmStats);
   } /* while xtmStats */

   fprintf(fs, "</table>\n");
   fprintf(fs, "<br><br>\n");

	fprintf(fs, "<p align='center'>\n");
	fprintf(fs, "<input type='button' onClick='btnReset()' value='Reset'> \n");
	fprintf(fs, "</form>\n</center>\n</blockquote>\n</body>\n</html>\n");

	fflush(fs);
}


/** This function gets called for the html ejGetOther(trafficType
 *
 */
void cgiGetVdslType(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue)
{
   int rate = 0;

   if (argc > 0)
   {
      cmsLog_debug("argc=%d argv[%d]=%s", argc, argc-1, argv[argc-1]);
   }

   /* for now, just report status for path 0/bearer 0 */
   if (argc == 3 && !cmsUtl_strcmp(argv[2], "0"))
   {
      if (isXdslLinkUp())
      {
         if (isVdslPtmMode())
         {
            strcpy(varValue, "PTM");
         }
         else
         {
            strcpy(varValue, "ATM");
         }
      }
      else
      {
         strcpy(varValue, "Inactive");
      }
   }
   else
   {
      /* path 1/bearer 1 */
      if (isXdslLinkUp())
      {
         qdmDsl_getPath1LineRateLocked(&rate);
         if (rate != 0)
         {
            if (isVdslPtmMode())
            {
               strcpy(varValue, "PTM");
            }
            else
            {
               strcpy(varValue, "ATM");
            }
         }
         else
         {
            strcpy(varValue, "Inactive");
         }
      }
      else
      {
         strcpy(varValue, "Inactive");
      }
   }
   
   cmsLog_debug("linkstatus=%s", varValue);
}
#endif /* SUPPORT_DSL */

#ifdef SUPPORT_MOCA


void cgiStsMoca(char *query, FILE *fs)
{
#if defined(SUPPORT_DM_LEGACY98)
   cgiStsMoca_igd(query, fs);
#elif defined(SUPPORT_DM_HYBRID)
   cgiStsMoca_igd(query, fs);
#elif defined(SUPPORT_DM_PURE181)
   cgiStsMoca_dev2(query, fs);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
   {
      cgiStsMoca_dev2(query, fs);
   }
   else
   {
      cgiStsMoca_igd(query, fs);
   }
#endif
}


void cgiResetStsMocaLan(void)
{
#ifdef DMP_X_BROADCOM_COM_MOCALAN_1
#if defined(SUPPORT_DM_LEGACY98)
   cgiResetStsMocaLan_igd();
#elif defined(SUPPORT_DM_HYBRID)
   cgiResetStsMocaLan_igd();
#elif defined(SUPPORT_DM_PURE181)
   cgiResetStsMocaLan_dev2();
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
   {
      cgiResetStsMocaLan_dev2();
   }
   else
   {
      cgiResetStsMocaLan_igd();
   }
#endif
#endif
}


void cgiResetStsMocaWan(void)
{
#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
#if defined(SUPPORT_DM_LEGACY98)
   cgiResetStsMocaWan_igd();
#elif defined(SUPPORT_DM_HYBRID)
   cgiResetStsMocaWan_igd();
#elif defined(SUPPORT_DM_PURE181)
   cgiResetStsMocaWan_dev2();
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
   {
      cgiResetStsMocaWan_dev2();
   }
   else
   {
      cgiResetStsMocaWan_igd();
   }
#endif
#endif
}


/* Reset ITU (GPON) Moca stats */
void cgiResetStsMoca(void)
{
   void *dummy=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   while (cmsObj_getNext(MDMOID_MOCA_STATS, &iidStack,(void **)&dummy) == CMSRET_SUCCESS)
   {
      cmsLog_debug("reseting moca stats at iidStack=%s", cmsMdm_dumpIidStack(&iidStack));
      cmsObj_clearStatistics(MDMOID_MOCA_STATS, &iidStack);
      cmsObj_free((void **) &dummy);
   }

}

#ifdef DMP_X_BROADCOM_COM_MOCALAN_1
void cgiResetStsMocaLan_igd(void)
{
   void *dummy=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   while (cmsObj_getNext(MDMOID_LAN_MOCA_INTF_STATS, &iidStack,(void **)&dummy) == CMSRET_SUCCESS)
   {
      cmsLog_debug("reseting moca lan stats at iidStack=%s", cmsMdm_dumpIidStack(&iidStack));
      cmsObj_clearStatistics(MDMOID_LAN_MOCA_INTF_STATS, &iidStack);
      cmsObj_free((void **) &dummy);
   }
}
#endif

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
void cgiResetStsMocaWan_igd(void)
{
   void *dummy=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   while (cmsObj_getNext(MDMOID_WAN_MOCA_INTF_STATS, &iidStack,(void **)&dummy) == CMSRET_SUCCESS)
   {
      cmsLog_debug("reseting moca wan stats at iidStack=%s", cmsMdm_dumpIidStack(&iidStack));
      cmsObj_clearStatistics(MDMOID_WAN_MOCA_INTF_STATS, &iidStack);
      cmsObj_free((void **) &dummy);
   }
}
#endif

void writeStsMocaScript(FILE *fs) {
   // write html header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<meta http-equiv='refresh' content='10'>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<script language=\"javascript\" src=\"util.js\"></script>\n");

   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");
   fprintf(fs, "function btnReset() {\n");
   fprintf(fs, "   var code = 'location=\"' + 'statsmocareset.html' + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   fprintf(fs, "      var idx = mocaSel.selectedIndex;\n");
   fprintf(fs, "      var val = mocaSel.options[idx].value;\n");
   fprintf(fs, "      var code = 'location=\"' + 'statsmoca' + val.toLowerCase() + 'reset.html' + '\"';\n");
   fprintf(fs, "      eval(code);\n");
   fprintf(fs, "   }\n");
   fprintf(fs, "}\n\n");

   /* Action on dropdown box selection */
   fprintf(fs, "function onMocaSel() {\n");
   fprintf(fs, "   var loc = 'statsmoca.cmd?';\n\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   fprintf(fs, "      var idx = mocaSel.selectedIndex;\n");
   fprintf(fs, "      var val = mocaSel.options[idx].value;\n");
   fprintf(fs, "      loc += 'choice=' + val;\n");
   fprintf(fs, "   }\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
   
}


#if defined(DMP_X_BROADCOM_COM_MOCALAN_1) || defined(DMP_X_BROADCOM_COM_MOCAWAN_1)

void cgiStsMoca_igd(char *query __attribute__((unused)), FILE *fs) {
   WanMocaIntfStatsObject  *mocaStats[2] = {NULL, NULL};
   WanMocaIntfStatusObject *mocaStatus[2] = {NULL, NULL};
   WanMocaIntfObject       *mocaIntf[2] = {NULL, NULL};
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   static UBOOL8 lanStat = TRUE;
   char choice[BUFLEN_32];
   int index = 0;

   /* The lanStat is defined static since refresh will not pass in the "LAN" or "WAN" parameter */
   cgiGetValueByName(query, "choice", choice );   

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   if (!cmsUtl_strcmp(choice, MDMVS_WAN))
      lanStat = FALSE;
   else if (!cmsUtl_strcmp(choice, MDMVS_LAN))
#endif
      lanStat = TRUE;

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
#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
      fprintf(fs, "    <option value='%s'>%s\n", MDMVS_WAN, MDMVS_WAN);
#endif
   }
#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   else
   {
      fprintf(fs, "    <option value='%s'>%s\n", MDMVS_LAN, MDMVS_LAN);
      fprintf(fs, "    <option value='%s' selected>%s\n", MDMVS_WAN, MDMVS_WAN);
   }
#endif
   fprintf(fs, "  </select></td>\n");
   fprintf(fs, "  </tr>\n");
   fprintf(fs, "</table><br><br>\n");

   if (lanStat)
   {
      do
      {
         if (cmsObj_getNext(MDMOID_LAN_MOCA_INTF, &iidStack, (void **) &mocaIntf[index]) != CMSRET_SUCCESS)
         {
            cmsLog_debug("LAN moca interface not found.");
            break;
         }
         else
         {
            if (mocaIntf[index]->enable)
            {
               if ((ret = cmsObj_get(MDMOID_LAN_MOCA_INTF_STATUS, &iidStack, 0, (void **) &mocaStatus[index])) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Fail to get LAN moca status, ret=%d", ret);
                  cmsObj_free((void **) &mocaIntf[index]);
                  break;
               }
               else
               {
                  if ((ret = cmsObj_get(MDMOID_LAN_MOCA_INTF_STATS, &iidStack, 0, (void **) &mocaStats[index])) != CMSRET_SUCCESS)
                  {
                     cmsLog_error("Fail to get LAN stats, ret=%d", ret);
                     cmsObj_free((void **) &mocaIntf[index]); 
                     cmsObj_free((void **) &mocaStatus[index]);
                     break;
                  }
               }
            }
            else
            {
               cmsObj_free((void **) &mocaIntf[index]);
               break;
            }
         }
         index++;
      } while (index < 2);
   }
#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   else
   {
      if (dalWan_getWanMocaObject(&iidStack, &mocaIntf[0]) != CMSRET_SUCCESS)
      {
         cmsLog_debug("WAN moca interface not found.");
      }
      else
      {
         if (mocaIntf[0]->enable)
         {
            if ((ret = cmsObj_get(MDMOID_WAN_MOCA_INTF_STATUS, &iidStack, 0, (void **)&mocaStatus[0])) != CMSRET_SUCCESS)
            {
               cmsLog_error("Fail to get WAN moca status, ret=%d", ret);
               cmsObj_free((void **) &mocaIntf[0]); 
            }
            else
            {
               if ((ret = cmsObj_get(MDMOID_WAN_MOCA_INTF_STATS, &iidStack, 0, (void **)&mocaStats[0])) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Fail to get WAN stats, ret=%d", ret);
                  cmsObj_free((void **) &mocaIntf[0]); 
                  cmsObj_free((void **) &mocaStatus[0]); 
               }
            }
         }
         else
         {
            cmsObj_free((void **) &mocaIntf[0]);
         }
      }
   }
#endif /* DMP_X_BROADCOM_COM_MOCAWAN_1 */      

   if ((NULL != mocaIntf[0]) && (NULL != mocaStats[0]) && (NULL != mocaStatus[0]))
   {
      fprintf(fs, "<b>Status</b><br>\n");
      fprintf(fs, "<table border='1' cellpadding='3' cellspacing='0'>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>Interface</td>\n");
      fprintf(fs, "      <td>%s</td>\n", mocaIntf[0]->ifName);
      if (NULL != mocaIntf[1]) fprintf(fs, "      <td>%s</td>\n", mocaIntf[1]->ifName);
      fprintf(fs, "   </tr>\n");      
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>VendorId</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->vendorId);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->vendorId);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>HWVersion</td>\n");
      fprintf(fs, "      <td>%X</td>\n", mocaStatus[0]->hwVersion);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%X</td>\n", mocaStatus[1]->hwVersion);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>SoftwareVersion</td>\n");
      fprintf(fs, "      <td>%u.%u.%u</td>\n", 
          (mocaStatus[0]->softwareVersion >> 28), 
          ((mocaStatus[0]->softwareVersion >> 24) & 0xf), 
          (mocaStatus[0]->softwareVersion & 0xffffff));
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u.%u.%u</td>\n", 
          (mocaStatus[1]->softwareVersion >> 28), 
          ((mocaStatus[1]->softwareVersion >> 24) & 0xf), 
          (mocaStatus[1]->softwareVersion & 0xffffff));
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>SelfMoCAVersion</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->selfMoCAVersion);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->selfMoCAVersion);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>NetworkVersionNumber</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->networkVersionNumber);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->networkVersionNumber);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>Qam256Support</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->qam256Support);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->qam256Support);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>OperationalStatus</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->operationalStatus);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->operationalStatus);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>LinkStatus</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->linkStatus);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->linkStatus);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>ConnectedNodes</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->connectedNodes);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->connectedNodes);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>NodeId</td></td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->nodeId);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->nodeId);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>NetworkControllerNodeId</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->networkControllerNodeId);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->networkControllerNodeId);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>MoCA UpTime</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->upTime);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->upTime);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>LinkUpTime</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->linkUpTime);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->linkUpTime);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>BackupNetworkControllerNodeId</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->backupNetworkControllerNodeId);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->backupNetworkControllerNodeId);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>RfChannel</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->rfChannel);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->rfChannel);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>BwStatus</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStatus[0]->bwStatus);
      if (NULL != mocaStatus[1]) fprintf(fs, "      <td>%u</td>\n", mocaStatus[1]->bwStatus);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "</table>\n");
      fprintf(fs, "<br><br>\n");

      fprintf(fs, "<b>Statistics</b><br>\n");
      fprintf(fs, "<table border='1' cellpadding='3' cellspacing='0'>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>Interface</td>\n");
      fprintf(fs, "      <td>%s</td>\n", mocaIntf[0]->ifName);
      if (NULL != mocaIntf[1]) fprintf(fs, "      <td>%s</td>\n", mocaIntf[1]->ifName);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>InUcPkts</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStats[0]->inUcPkts);
      if (NULL != mocaStats[1]) fprintf(fs, "      <td>%u</td>\n", mocaStats[1]->inUcPkts);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>InDiscardPktsEcl</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStats[0]->inDiscardPktsEcl);
      if (NULL != mocaStats[1]) fprintf(fs, "      <td>%u</td>\n", mocaStats[1]->inDiscardPktsEcl);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>InDiscardPktsMac</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStats[0]->inDiscardPktsMac);
      if (NULL != mocaStats[1]) fprintf(fs, "      <td>%u</td>\n", mocaStats[1]->inDiscardPktsMac);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>InUnKnownPkts</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStats[0]->inUnKnownPkts);
      if (NULL != mocaStats[1]) fprintf(fs, "      <td>%u</td>\n", mocaStats[1]->inUnKnownPkts);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>InMcPkts</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStats[0]->inMcPkts);
      if (NULL != mocaStats[1]) fprintf(fs, "      <td>%u</td>\n", mocaStats[1]->inMcPkts);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>InBcPkts</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStats[0]->inBcPkts);
      if (NULL != mocaStats[1]) fprintf(fs, "      <td>%u</td>\n", mocaStats[1]->inBcPkts);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>InOctetsLow</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStats[0]->inOctetsLow);
      if (NULL != mocaStats[1]) fprintf(fs, "      <td>%u</td>\n", mocaStats[1]->inOctetsLow);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>OutUcPkts</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStats[0]->outUcPkts);
      if (NULL != mocaStats[1]) fprintf(fs, "      <td>%u</td>\n", mocaStats[1]->outUcPkts);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>OutDiscardPkts</td></td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStats[0]->outDiscardPkts);
      if (NULL != mocaStats[1]) fprintf(fs, "      <td>%u</td>\n", mocaStats[1]->outDiscardPkts);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>OutBcPkts</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStats[0]->outBcPkts);
      if (NULL != mocaStats[1]) fprintf(fs, "      <td>%u</td>\n", mocaStats[1]->outBcPkts);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>OutOctetsLow</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStats[0]->outOctetsLow);
      if (NULL != mocaStats[1]) fprintf(fs, "      <td>%u</td>\n", mocaStats[1]->outOctetsLow);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>NetworkControllerHandOffs</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStats[0]->networkControllerHandOffs);
      if (NULL != mocaStats[1]) fprintf(fs, "      <td>%u</td>\n", mocaStats[1]->networkControllerHandOffs);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td class='hd'>NetworkControllerBackups</td>\n");
      fprintf(fs, "      <td>%u</td>\n", mocaStats[0]->networkControllerBackups);
      if (NULL != mocaStats[1]) fprintf(fs, "      <td>%u</td>\n", mocaStats[1]->networkControllerBackups);
      fprintf(fs, "   </tr>\n");
      fprintf(fs, "</table>\n");
      fprintf(fs, "<br><br>\n");

      cmsObj_free((void **) &mocaIntf[0]);
      cmsObj_free((void **) &mocaStatus[0]);
      cmsObj_free((void **) &mocaStats[0]);
      if ( NULL != mocaIntf[1] ) cmsObj_free((void **) &mocaIntf[1]);
      if ( NULL != mocaStatus[1] ) cmsObj_free((void **) &mocaStatus[1]);
      if ( NULL != mocaStats[1] ) cmsObj_free((void **) &mocaStats[1]);
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

#endif /* TR98 MOCALAN || MOCAWAN */

#endif /* SUPPORT_MOCA */

