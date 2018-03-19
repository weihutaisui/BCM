/*
<:copyright-BRCM:2012:proprietary:standard

   Copyright (c) 2012 Broadcom 
   All Rights Reserved

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
*/

#ifdef SUPPORT_QOS

/*!\file cgi_qos_port_shaping.c
 * \brief this file contains the data model independent code
 *        and TR98 specific code for ethernet port shaping.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <linux/if_ether.h>

#include "cms.h"
#include "cms_dal.h"
#include "cms_qos.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_main.h"
#include "cgi_qos.h"



static void cgiQosPortShapingView( FILE *fs )
{
   do_ej("/webs/qosportshaping.html", fs);
}  /* End of cgiQosPortShapingView() */


void cgiQosPortShaping(char *query, FILE *fs)
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);
   if (strcmp(action, "savapply") == 0)
   { 
      cgiQosPortShapingSavApply(query);
      cgiQosPortShapingView(fs);
   }
   else
   {
      cgiQosPortShapingView(fs);
   }
}  /* End of cgiQosPortShaping() */


#ifdef DMP_QOS_1
void cgiQosPortShapingSavApply_igd(char *query)
{

   InstanceIdStack wanIidStack = EMPTY_INSTANCE_ID_STACK;
   WanEthIntfObject *wanEthObj = NULL;
   InstanceIdStack lanIidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *lanEthObj = NULL;
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char paraNameBuf[BUFLEN_32];
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 shouldSetObj = TRUE;
   
   cmd[0] = '\0';
   paraNameBuf[0] = '\0';

   /* Set WAN port shaping */
   while (cmsObj_getNext(MDMOID_WAN_ETH_INTF, &wanIidStack, (void **) &wanEthObj) == CMSRET_SUCCESS)
   {
      if (wanEthObj->X_BROADCOM_COM_IfName != NULL && wanEthObj->enable)
      {
         shouldSetObj = TRUE;
         sprintf(paraNameBuf, "%sWanRateCfg", wanEthObj->X_BROADCOM_COM_IfName);
         if (cgiGetValueByName(query, paraNameBuf, cmd) == CGI_STS_OK)
         {
            wanEthObj->shapingRate = atoi(cmd);
            if(wanEthObj->shapingRate != -1)
            {
               wanEthObj->shapingRate = wanEthObj->shapingRate * 1000;
            }
         }else{
            shouldSetObj = FALSE;
         }
         sprintf(paraNameBuf, "%sWanSizeCfg", wanEthObj->X_BROADCOM_COM_IfName);
         if (cgiGetValueByName(query, paraNameBuf, cmd) == CGI_STS_OK)
         {
            wanEthObj->shapingBurstSize = atoi(cmd);
         }
         else
         {
            shouldSetObj = FALSE;
         }
         if(shouldSetObj)
         {
            if ((ret = cmsObj_set(wanEthObj, &wanIidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to set WanEthIntfObject, ret = %d", ret);
            }
            else
            {
               glbSaveConfigNeeded = TRUE;
            }
         }
      }
      cmsObj_free((void **) &wanEthObj);
   }

   /* Set LAN port shaping */
   while (cmsObj_getNext(MDMOID_LAN_ETH_INTF, &lanIidStack, (void **) &lanEthObj) == CMSRET_SUCCESS)
   {
      if (lanEthObj->X_BROADCOM_COM_IfName != NULL && lanEthObj->enable)
      {
         shouldSetObj = TRUE;
         sprintf(paraNameBuf, "%sLanRateCfg", lanEthObj->X_BROADCOM_COM_IfName);
         if (cgiGetValueByName(query, paraNameBuf, cmd) == CGI_STS_OK)
         {
            lanEthObj->X_BROADCOM_COM_ShapingRate = atoi(cmd);
            if(lanEthObj->X_BROADCOM_COM_ShapingRate != -1)
            {
               lanEthObj->X_BROADCOM_COM_ShapingRate = lanEthObj->X_BROADCOM_COM_ShapingRate * 1000;
            }            
         }
         else
         {
            shouldSetObj = FALSE;
         }
         sprintf(paraNameBuf, "%sLanSizeCfg", lanEthObj->X_BROADCOM_COM_IfName);
         if (cgiGetValueByName(query, paraNameBuf, cmd) == CGI_STS_OK)
         {
            lanEthObj->X_BROADCOM_COM_ShapingBurstSize = atoi(cmd);
         }
         else
         {
            shouldSetObj = FALSE;
         }
         if(shouldSetObj)
         {
            if ((ret = cmsObj_set(lanEthObj, &lanIidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to set LanEthIntfObject, ret = %d", ret);
            }
            else
            {
               glbSaveConfigNeeded = TRUE;
            }
         }
      }
      cmsObj_free((void **) &lanEthObj);
   }
}  /* End of cgiQosPortShapingSavApply() */


void cgiGetQosPortShapingInfo_igd(int argc __attribute((unused)),
                              char **argv __attribute((unused)),
                              char *varValue)
{
   InstanceIdStack wanIidStack = EMPTY_INSTANCE_ID_STACK;
   WanEthIntfObject *wanEthObj = NULL;

#if !defined(BCM_PON)
   InstanceIdStack lanIidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *lanEthObj = NULL;
#endif

   SINT32 rateKbps = -1;
   char *p = varValue;

   /* Get QoS Port Shaping information from Ethernet WAN */
   while (cmsObj_getNext(MDMOID_WAN_ETH_INTF, &wanIidStack, (void **) &wanEthObj) == CMSRET_SUCCESS)
   {
      if (wanEthObj->X_BROADCOM_COM_IfName != NULL && wanEthObj->enable)
      {
         if(wanEthObj->shapingRate != -1)
         {
            rateKbps = wanEthObj->shapingRate / 1000;
         }
         else
         {
            rateKbps = -1;
         }
         p += sprintf(p, "               <tr>\n");
         p += sprintf(p, "                  <td class='hd'>%s</td>\n", wanEthObj->X_BROADCOM_COM_IfName);
         p += sprintf(p, "                  <td>WAN</td>\n");
         p += sprintf(p, "                  <td><input type='text' name='%sWanRateCfg' maxlength='16' size='16' value='%d'></td>\n", 
            wanEthObj->X_BROADCOM_COM_IfName, rateKbps);
         p += sprintf(p, "                  <td><input type='text' name='%sWanSizeCfg' maxlength='16' size='16' value='%d'></td>\n", 
            wanEthObj->X_BROADCOM_COM_IfName, wanEthObj->shapingBurstSize);
         p += sprintf(p, "               </tr>");
      }
      cmsObj_free((void **) &wanEthObj);
   }

#if !defined(BCM_PON)
   /* Get QoS Port Shaping information from Ethernet LAN */
   while (cmsObj_getNext(MDMOID_LAN_ETH_INTF, &lanIidStack, (void **) &lanEthObj) == CMSRET_SUCCESS)
   {
      if (lanEthObj->X_BROADCOM_COM_IfName != NULL && lanEthObj->enable)
      {
         if(lanEthObj->X_BROADCOM_COM_ShapingRate != -1)
         {
            rateKbps = lanEthObj->X_BROADCOM_COM_ShapingRate / 1000;
         }
         else
         {
            rateKbps = -1;
         }
         p += sprintf(p, "               <tr>\n");
         p += sprintf(p, "                  <td class='hd'>%s</td>\n", lanEthObj->X_BROADCOM_COM_IfName);
         p += sprintf(p, "                  <td>LAN</td>\n");
         p += sprintf(p, "                  <td><input type='text' name='%sLanRateCfg' maxlength='16' size='16' value='%d'></td>\n", 
            lanEthObj->X_BROADCOM_COM_IfName, rateKbps);
         p += sprintf(p, "                  <td><input type='text' name='%sLanSizeCfg' maxlength='16' size='16' value='%d'></td>\n", 
            lanEthObj->X_BROADCOM_COM_IfName, lanEthObj->X_BROADCOM_COM_ShapingBurstSize);
         p += sprintf(p, "               </tr>");
      }
      cmsObj_free((void **) &lanEthObj);
   }
#endif
   
   *p = 0;
   return;
}

#endif  /* DMP_QOS_1 */




void cgiGetQosPortShapingInfo(int argc, char **argv, char *varValue)
{
#if defined(SUPPORT_DM_LEGACY98)
   cgiGetQosPortShapingInfo_igd(argc, argv, varValue);
#elif defined(SUPPORT_DM_HYBRID)
   cgiGetQosPortShapingInfo_igd(argc, argv, varValue);
#elif defined(SUPPORT_DM_PURE181)
   cgiGetQosPortShapingInfo_dev2(argc, argv, varValue);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
      cgiGetQosPortShapingInfo_dev2(argc, argv, varValue);
   else
      cgiGetQosPortShapingInfo_igd(argc, argv, varValue);
#endif
}

#endif  /* SUPPORT_QOS */

