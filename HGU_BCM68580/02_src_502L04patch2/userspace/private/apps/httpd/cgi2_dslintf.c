/***********************************************************************
 *
 *  Copyright (c) 2008  Broadcom Corporation
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
#include <errno.h>
#include <unistd.h>
#include "cgi_cmd.h"
#include "cgi_main.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "devctl_xtm.h"

/*
 * ATM configuration
 */

void cgiDslAtmCfgView_dev2(FILE *fs) 
{
   UINT32 portId = 0;
   SINT32 vpi = 0;
   SINT32 vci = 0;
   SINT32 layer2Count = 0;
   Dev2AtmLinkObject *atmLink = NULL;
   Dev2AtmLinkQosObject *atmLinkQos = NULL;
   InstanceIdStack atmLinkIid = EMPTY_INSTANCE_ID_STACK;
   
   /* write Java Script */
   writeDslXtmCfgScript(fs, "cfgatm", "dslatm");

   /* write table */
   fprintf(fs, "<b>DSL ATM Interface Configuration</b><br><br>\n");
   fprintf(fs, "Choose Add, or Remove to configure DSL ATM interfaces.<br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "      <td class='hd'>Vpi</td>\n");
   fprintf(fs, "      <td class='hd'>Vci</td>\n");
   fprintf(fs, "      <td class='hd'>DSL Latency</td>\n");
   fprintf(fs, "      <td class='hd'>Category</td>\n");
   fprintf(fs, "      <td class='hd'>Peak Cell Rate(cells/s)</td>\n");
   fprintf(fs, "      <td class='hd'>Sustainable Cell Rate(cells/s)</td>\n");
   fprintf(fs, "      <td class='hd'>Max Burst Size(bytes)</td>\n");
#if defined(CHIP_63268) || defined(CHIP_63138) || defined(CHIP_63381) || defined(CHIP_63148) || defined(CHIP_63158)
   fprintf(fs, "      <td class='hd'>Min Cell Rate(cells/s)</td>\n");
#endif   
   fprintf(fs, "      <td class='hd'>Link Type</td>\n");
   fprintf(fs, "      <td class='hd'>Conn Mode</td>\n");
   fprintf(fs, "      <td class='hd'>IP QoS</td>\n");
#if defined(CHIP_63268) || defined(CHIP_63138) || defined(CHIP_63381) || defined(CHIP_63148) || defined(CHIP_63158)
   fprintf(fs, "      <td class='hd'>MPAAL Prec/Alg/Wght</td>\n");
#endif
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   /* Normal ATM PVC */
   while (layer2Count < IFC_WAN_MAX &&
          cmsObj_getNext(MDMOID_DEV2_ATM_LINK, &atmLinkIid, (void **) &atmLink) == CMSRET_SUCCESS)
   {
      /* get vpi/vci string */
      cmsUtl_atmVpiVciStrToNum_dev2(atmLink->destinationAddress, &vpi, &vci);
      portId = atmLink->X_BROADCOM_COM_ATMInterfaceId;

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td>%s</td>\n", atmLink->name);
      fprintf(fs, "      <td>%d</td>\n", vpi);
      fprintf(fs, "      <td>%d</td>\n", vci);
      fprintf(fs, "      <td>%s</td>\n", (portId == PHY0_PATH0)? "Path0" :
                                         (portId == PHY0_PATH1)? "Path1" :
                                         (portId == PHY0_PATH0_PATH1)? "Path0&1" : "");

      if (cmsObj_get(MDMOID_DEV2_ATM_LINK_QOS, &atmLinkIid, 0, (void **)&atmLinkQos) == CMSRET_SUCCESS)
      {  
         fprintf(fs, "      <td>%s</td>\n", atmLinkQos->qoSClass);

         if (cmsUtl_strcmp(atmLinkQos->qoSClass, MDMVS_UBR) == 0)
         {
            fprintf(fs, "      <td>&nbsp;</td>\n");
            fprintf(fs, "      <td>&nbsp;</td>\n");
            fprintf(fs, "      <td>&nbsp;</td>\n");
         }
         else if (cmsUtl_strcmp(atmLinkQos->qoSClass, MDMVS_UBRWPCR) == 0 ||
                  cmsUtl_strcmp(atmLinkQos->qoSClass, MDMVS_CBR)     == 0)
         {
            fprintf(fs, "      <td>%d</td>\n", atmLinkQos->peakCellRate);
            fprintf(fs, "      <td>&nbsp;</td>\n");
            fprintf(fs, "      <td>&nbsp;</td>\n");
         }
         else if (cmsUtl_strcmp(atmLinkQos->qoSClass, MDMVS_VBR_NRT) == 0 ||
                  cmsUtl_strcmp(atmLinkQos->qoSClass, MDMVS_VBR_RT)  == 0)
         {
            fprintf(fs, "      <td>%d</td>\n", atmLinkQos->peakCellRate);
            fprintf(fs, "      <td>%d</td>\n", atmLinkQos->sustainableCellRate);
            fprintf(fs, "      <td>%d</td>\n", atmLinkQos->maximumBurstSize);
         }

#if defined(CHIP_63268) || defined(CHIP_63138) || defined(CHIP_63381) || defined(CHIP_63148) || defined(CHIP_63158)
         if ((atmLinkQos->X_BROADCOM_COM_MinimumCellRate > 0) &&
             (cmsUtl_strcmp(atmLinkQos->qoSClass, MDMVS_UBR)     == 0 ||
              cmsUtl_strcmp(atmLinkQos->qoSClass, MDMVS_UBRWPCR) == 0))
         {
            fprintf(fs, "      <td>%d</td>\n", atmLinkQos->X_BROADCOM_COM_MinimumCellRate);
         }
         else
         {
            fprintf(fs, "      <td>&nbsp;</td>\n");
         }
#endif      

         /* free atmLinkQos */
         cmsObj_free((void **) &atmLinkQos);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp;</td>\n");
         fprintf(fs, "      <td>&nbsp;</td>\n");
         fprintf(fs, "      <td>&nbsp;</td>\n");
         fprintf(fs, "      <td>&nbsp;</td>\n");
#if defined(CHIP_63268) || defined(CHIP_63138) || defined(CHIP_63381) || defined(CHIP_63148) || defined(CHIP_63158)
         fprintf(fs, "      <td>&nbsp;</td>\n");
#endif      
      }

      fprintf(fs, "      <td>%s</td>\n", atmLink->linkType);
      fprintf(fs, "      <td>%s</td>\n", atmLink->X_BROADCOM_COM_ConnectionMode);
      fprintf(fs, "      <td>%s</td>\n", atmLink->X_BROADCOM_COM_ATMEnbQos ? "Support" : "NoSupport");
#if defined(CHIP_63268) || defined(CHIP_63138) || defined(CHIP_63381) || defined(CHIP_63148) || defined(CHIP_63158)
      if (strcmp(atmLink->X_BROADCOM_COM_GrpScheduler, MDMVS_SP) == 0)
      {
         fprintf(fs, "      <td>&nbsp;</td>\n");
      }
      else
      {
         fprintf(fs, "      <td>%d/WRR/%d</td>\n",
                 atmLink->X_BROADCOM_COM_GrpPrecedence,
                 atmLink->X_BROADCOM_COM_GrpWeight);
      }
#endif

      /* remove check box */
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", atmLink->name);
      fprintf(fs, "   </tr>\n");

      /* free atmLink */
      cmsObj_free((void **) &atmLink);

      layer2Count++;
   }

   writeXtmFooter(fs, layer2Count);
   
   /* reset atm to default value for next add  */
   getDefaultAtmLinkCfg(&glbWebVar);
}

#ifdef DMP_DEVICE2_PTMLINK_1
/* 
 * PTM configuration
 */

void cgiDslPtmCfgView_dev2(FILE *fs) 
{
   UINT32 portId;
   Dev2PtmLinkObject *ptmLink = NULL;
   InstanceIdStack ptmLinkIid = EMPTY_INSTANCE_ID_STACK;

   /* For PTM layer 2 interface, the max layer 2 count is 4 and
   * since writeXtmFooter function check layer2Count against 8,
   * just have ptm layer2Count starts with 4.
   */
   SINT32 layer2Count = 4;
   
   /* write Java Script */
   writeDslXtmCfgScript(fs, "cfgptm", "dslptm");

   /* write table */
   fprintf(fs, "<b>DSL PTM Interface Configuration</b><br><br>\n");
   fprintf(fs, "Choose Add, or Remove to configure DSL PTM interfaces.<br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "      <td class='hd'>DSL Latency</td>\n");
   fprintf(fs, "      <td class='hd'>PTM Priority</td>\n");
   fprintf(fs, "      <td class='hd'>Conn Mode</td>\n");
   fprintf(fs, "      <td class='hd'>IP QoS</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   while (layer2Count < IFC_WAN_MAX &&
          cmsObj_getNext(MDMOID_DEV2_PTM_LINK, &ptmLinkIid, (void **) &ptmLink) == CMSRET_SUCCESS)
   {
      portId = ptmLink->X_BROADCOM_COM_PTMPortId;

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td>%s</td>\n", ptmLink->name);
      fprintf(fs, "      <td>%s</td>\n", (portId == PHY0_PATH0)? "Path0" :
                                         (portId == PHY0_PATH1)? "Path1" :
                                         (portId == PHY0_PATH0_PATH1)? "Path0&1" : "");
      if (ptmLink->X_BROADCOM_COM_PTMPriorityLow && ptmLink->X_BROADCOM_COM_PTMPriorityHigh)
      {
         fprintf(fs, "      <td>%s</td>\n", "Normal&High");
      }
      else if (ptmLink->X_BROADCOM_COM_PTMPriorityLow)
      {
         fprintf(fs, "      <td>%s</td>\n", "Normal");
      }
      else if (ptmLink->X_BROADCOM_COM_PTMPriorityHigh)
      {
         fprintf(fs, "      <td>%s</td>\n", "High");
      }
      else
      {
         fprintf(fs, "      <td>%s</td>\n", "");
      }
      fprintf(fs, "      <td>%s</td>\n", ptmLink->X_BROADCOM_COM_ConnectionMode);
      fprintf(fs, "      <td>%s</td>\n", ptmLink->X_BROADCOM_COM_PTMEnbQos ? "Support" : "NoSupport");
      /* remove check box */
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", ptmLink->name);
      fprintf(fs, "   </tr>\n");
         
      /* free ptmLink */
      cmsObj_free((void **) &ptmLink);
      layer2Count++;
   }
   
   writeXtmFooter(fs, layer2Count);

   /* reset ptm to default value for next add  */
   getDefaultPtmLinkCfg(&glbWebVar);
}

#endif  /* DMP_DEVICE2_PTMLINK_1 */

#endif  /* DMP_DEVICE2_DSL_1 */

#endif  /* DMP_DEVICE2_BASELINE_1 */

