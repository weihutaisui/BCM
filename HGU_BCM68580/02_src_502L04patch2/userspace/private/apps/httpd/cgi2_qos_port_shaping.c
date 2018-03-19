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

#ifdef DMP_DEVICE2_QOS_1


/*!\file cgi2_qos_port_shaping.c
 * \brief this file contains TR181 specific ethernet port shaping code.
 *        It also uses the data model independent functions in
 *        cgi_qos_port_shaping.c
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



#if 0
void cgiQosPortShapingSavApply_dev2(char *query)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2EthernetInterfaceObject *ethIntfObj = NULL;
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char paraNameBuf[BUFLEN_32];
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 shouldSetObj = TRUE;
   
   cmd[0] = '\0';
   paraNameBuf[0] = '\0';

   /* Set WAN port shaping */
   while (cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      if (ethIntfObj->enable && ethIntfObj->upstream)
      {
         shouldSetObj = TRUE;
         sprintf(paraNameBuf, "%sWanRateCfg", ethIntfObj->name);
         if (cgiGetValueByName(query, paraNameBuf, cmd) == CGI_STS_OK)
         {
            ethIntfObj->X_BROADCOM_COM_ShapingRate = atoi(cmd);
            if(ethIntfObj->X_BROADCOM_COM_ShapingRate != -1)
            {
               ethIntfObj->X_BROADCOM_COM_ShapingRate *= 1000;
            }
         }else{
            shouldSetObj = FALSE;
         }
         sprintf(paraNameBuf, "%sWanSizeCfg", ethIntfObj->name);
         if (cgiGetValueByName(query, paraNameBuf, cmd) == CGI_STS_OK)
         {
            ethIntfObj->X_BROADCOM_COM_ShapingBurstSize = atoi(cmd);
         }
         else
         {
            shouldSetObj = FALSE;
         }
         if(shouldSetObj)
         {
            if ((ret = cmsObj_set(ethIntfObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to set WanEthIntfObject, ret = %d", ret);
            }
            else
            {
               glbSaveConfigNeeded = TRUE;
            }
         }
      }
      cmsObj_free((void **) &ethIntfObj);
   }

   /* Set LAN port shaping */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      if (ethIntfObj->enable && !ethIntfObj->upstream)
      {
         shouldSetObj = TRUE;
         sprintf(paraNameBuf, "%sLanRateCfg", ethIntfObj->name);
         if (cgiGetValueByName(query, paraNameBuf, cmd) == CGI_STS_OK)
         {
            ethIntfObj->X_BROADCOM_COM_ShapingRate = atoi(cmd);
            if(ethIntfObj->X_BROADCOM_COM_ShapingRate != -1)
            {
               ethIntfObj->X_BROADCOM_COM_ShapingRate *= 1000;
            }            
         }
         else
         {
            shouldSetObj = FALSE;
         }
         sprintf(paraNameBuf, "%sLanSizeCfg", ethIntfObj->name);
         if (cgiGetValueByName(query, paraNameBuf, cmd) == CGI_STS_OK)
         {
            ethIntfObj->X_BROADCOM_COM_ShapingBurstSize = atoi(cmd);
         }
         else
         {
            shouldSetObj = FALSE;
         }
         if(shouldSetObj)
         {
            if ((ret = cmsObj_set(ethIntfObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to set LanEthIntfObject, ret = %d", ret);
            }
            else
            {
               glbSaveConfigNeeded = TRUE;
            }
         }
      }
      cmsObj_free((void **) &ethIntfObj);
   }
}



void cgiGetQosPortShapingInfo_dev2(int argc __attribute((unused)),
                              char **argv __attribute((unused)),
                              char *varValue)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2EthernetInterfaceObject *ethIntfObj = NULL;
   SINT32 rateKbps = -1;
   char *p = varValue;

   /* First loop through all ethernet intf looking for WAN interfaces */
   while (cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      if (ethIntfObj->enable && ethIntfObj->upstream)
      {
         if(ethIntfObj->X_BROADCOM_COM_ShapingRate != -1)
         {
            rateKbps = ethIntfObj->X_BROADCOM_COM_ShapingRate / 1000;
         }
         else
         {
            rateKbps = -1;
         }
         p += sprintf(p, "               <tr>\n");
         p += sprintf(p, "                  <td class='hd'>%s</td>\n", ethIntfObj->name);
         p += sprintf(p, "                  <td>WAN</td>\n");
         p += sprintf(p, "                  <td><input type='text' name='%sWanRateCfg' maxlength='16' size='16' value='%d'></td>\n", 
                      ethIntfObj->name, rateKbps);
         p += sprintf(p, "                  <td><input type='text' name='%sWanSizeCfg' maxlength='16' size='16' value='%d'></td>\n", 
                      ethIntfObj->name,
                      ethIntfObj->X_BROADCOM_COM_ShapingBurstSize);
         p += sprintf(p, "               </tr>");
      }
      cmsObj_free((void **) &ethIntfObj);
   }

   /* Now loop through all ethernet intf looking for LAN interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      if (ethIntfObj->enable && !ethIntfObj->upstream)
      {
         if(ethIntfObj->X_BROADCOM_COM_ShapingRate != -1)
         {
            rateKbps = ethIntfObj->X_BROADCOM_COM_ShapingRate / 1000;
         }
         else
         {
            rateKbps = -1;
         }
         p += sprintf(p, "               <tr>\n");
         p += sprintf(p, "                  <td class='hd'>%s</td>\n", ethIntfObj->name);
         p += sprintf(p, "                  <td>LAN</td>\n");
         p += sprintf(p, "                  <td><input type='text' name='%sLanRateCfg' maxlength='16' size='16' value='%d'></td>\n", 
                      ethIntfObj->name, rateKbps);
         p += sprintf(p, "                  <td><input type='text' name='%sLanSizeCfg' maxlength='16' size='16' value='%d'></td>\n", 
                      ethIntfObj->name,
                      ethIntfObj->X_BROADCOM_COM_ShapingBurstSize);
         p += sprintf(p, "               </tr>");
      }
      cmsObj_free((void **) &ethIntfObj);
   }
   
   *p = 0;
   return;
}
#endif


static UBOOL8 isQosShaperExisted
   (const MdmPathDescriptor *pathDesc)
{
   char *ethIntfFullPath = NULL;
   UBOOL8 found = FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2QosShaperObject *shaperObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   ret = cmsMdm_pathDescriptorToFullPath(pathDesc, &ethIntfFullPath);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      return ret;
   }

   // loop through QoS Shaper objects to find
   // one that can match with interface full path
   while (found == FALSE &&
          cmsObj_getNextFlags(MDMOID_DEV2_QOS_SHAPER,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &shaperObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(shaperObj->interface, ethIntfFullPath) == 0)
      {
         found = TRUE;
      }

      cmsObj_free((void **) &shaperObj);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(ethIntfFullPath);

   return found;
}


static CmsRet setQosShaperInfo
   (const MdmPathDescriptor *pathDesc,
    SINT32 shapingRate,
    UINT32 shapingBurstSize)
{
   char *ethIntfFullPath = NULL;
   UBOOL8 found = FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2QosShaperObject *shaperObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   ret = cmsMdm_pathDescriptorToFullPath(pathDesc, &ethIntfFullPath);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      return ret;
   }

   // loop through QoS Shaper objects to find
   // one that can match with interface full path
   while (found == FALSE &&
          cmsObj_getNextFlags(MDMOID_DEV2_QOS_SHAPER,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &shaperObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(shaperObj->interface, ethIntfFullPath) == 0)
      {
         found = TRUE;

         shaperObj->shapingRate = shapingRate;
         shaperObj->shapingBurstSize = shapingBurstSize;

         ret = cmsObj_set(shaperObj, &iidStack);

         if (ret == CMSRET_SUCCESS)
         {
            glbSaveConfigNeeded = TRUE;
         }
         else
         {
            cmsLog_error("Could not set Dev2QosShaperObject, ret = %d", ret);
         }
      }

      cmsObj_free((void **) &shaperObj);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(ethIntfFullPath);

   return ret;
}


static CmsRet addQosShaperInfo
   (const MdmPathDescriptor *pathDesc,
    SINT32 shapingRate,
    UINT32 shapingBurstSize)
{
   char *ethIntfFullPath = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2QosShaperObject *shaperObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   ret = cmsMdm_pathDescriptorToFullPath(pathDesc, &ethIntfFullPath);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      return ret;
   }

   ret = cmsObj_addInstance(MDMOID_DEV2_QOS_SHAPER, &iidStack);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add Dev2QosShaperObject instance, ret=%d", ret);
      return ret;
   } 

   ret = cmsObj_get(MDMOID_DEV2_QOS_SHAPER,
                    &iidStack,
                    0,
                    (void **) &shaperObj);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Dev2QosShaperObject instance, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_QOS_SHAPER, &iidStack);
      return ret;
   }

   shaperObj->enable = TRUE;
   shaperObj->shapingRate = shapingRate;
   shaperObj->shapingBurstSize = shapingBurstSize;
   CMSMEM_REPLACE_STRING(shaperObj->interface, ethIntfFullPath);

   ret = cmsObj_set(shaperObj, &iidStack);

   if (ret == CMSRET_SUCCESS)
   {
      glbSaveConfigNeeded = TRUE;
   }
   else
   {
      cmsLog_error("Failed to set Dev2QosShaperObject, ret=%d", ret);
   }

   cmsObj_free((void **) &shaperObj);

   CMSMEM_FREE_BUF_AND_NULL_PTR(ethIntfFullPath);

   return ret;
}


void cgiQosPortShapingSavApply_dev2(char *query)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char paraNameBuf[BUFLEN_32];
   UBOOL8 shouldSetObj = TRUE;
   SINT32 shapingRate = -1;
   UINT32 shapingBurstSize = 0;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2EthernetInterfaceObject *ethIntfObj = NULL;
   MdmPathDescriptor pathDesc;

   cmd[0] = '\0';
   paraNameBuf[0] = '\0';

   /* Set WAN port shaping */
   while (cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      if (ethIntfObj->enable && ethIntfObj->upstream)
      {
         shouldSetObj = TRUE;

         sprintf(paraNameBuf, "%sWanRateCfg", ethIntfObj->name);
         if (cgiGetValueByName(query, paraNameBuf, cmd) == CGI_STS_OK)
         {
            sscanf(cmd, "%d", &shapingRate);
            if (shapingRate != -1)
            {
               shapingRate *= 1000;
            }
         }
         else
         {
            shouldSetObj = FALSE;
         }

         sprintf(paraNameBuf, "%sWanSizeCfg", ethIntfObj->name);
         if (cgiGetValueByName(query, paraNameBuf, cmd) == CGI_STS_OK)
         {
            sscanf(cmd, "%u", &shapingBurstSize);
         }
         else
         {
            shouldSetObj = FALSE;
         }

         if (shouldSetObj == TRUE)
         {
            INIT_PATH_DESCRIPTOR(&pathDesc);
            pathDesc.oid = MDMOID_DEV2_ETHERNET_INTERFACE;
            pathDesc.iidStack = iidStack;

            if (isQosShaperExisted(&pathDesc) == TRUE)
            {
               setQosShaperInfo(&pathDesc,
                                shapingRate,
                                shapingBurstSize);
            }
            else
            {
               addQosShaperInfo(&pathDesc,
                                shapingRate,
                                shapingBurstSize);
            }
         }
      }

      cmsObj_free((void **) &ethIntfObj);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);

   /* Set LAN port shaping */
   while (cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      if (ethIntfObj->enable && !ethIntfObj->upstream)
      {
         shouldSetObj = TRUE;

         sprintf(paraNameBuf, "%sLanRateCfg", ethIntfObj->name);
         if (cgiGetValueByName(query, paraNameBuf, cmd) == CGI_STS_OK)
         {
            sscanf(cmd, "%d", &shapingRate);
            if (shapingRate != -1)
            {
               shapingRate *= 1000;
            }
         }
         else
         {
            shouldSetObj = FALSE;
         }

         sprintf(paraNameBuf, "%sLanSizeCfg", ethIntfObj->name);
         if (cgiGetValueByName(query, paraNameBuf, cmd) == CGI_STS_OK)
         {
            sscanf(cmd, "%u", &shapingBurstSize);
         }
         else
         {
            shouldSetObj = FALSE;
         }

         if (shouldSetObj == TRUE)
         {
            INIT_PATH_DESCRIPTOR(&pathDesc);
            pathDesc.oid = MDMOID_DEV2_ETHERNET_INTERFACE;
            pathDesc.iidStack = iidStack;

            if (isQosShaperExisted(&pathDesc) == TRUE)
            {
               setQosShaperInfo(&pathDesc,
                                shapingRate,
                                shapingBurstSize);
            }
            else
            {
               addQosShaperInfo(&pathDesc,
                                shapingRate,
                                shapingBurstSize);
            }
         }
      }

      cmsObj_free((void **) &ethIntfObj);
   }
}


static CmsRet getQosShaperInfoByPathDescriptor
   (const MdmPathDescriptor *pathDesc,
    SINT32 *shapingRate,
    UINT32 *shapingBurstSize)
{
   char *ethIntfFullPath = NULL;
   UBOOL8 found = FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2QosShaperObject *shaperObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   ret = cmsMdm_pathDescriptorToFullPath(pathDesc, &ethIntfFullPath);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      return ret;
   }

   // loop through QoS Shaper objects to find
   // one that can match with interface full path
   while (found == FALSE &&
          cmsObj_getNextFlags(MDMOID_DEV2_QOS_SHAPER,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &shaperObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(shaperObj->interface, ethIntfFullPath) == 0)
      {
         found = TRUE;
         *shapingRate = shaperObj->shapingRate;
         *shapingBurstSize = shaperObj->shapingBurstSize;
      }

      cmsObj_free((void **) &shaperObj);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(ethIntfFullPath);

   if (found == FALSE)
   {
      // if shaper is not found then set
      // shapingRate to -1 for no shaper
      *shapingRate = -1;
      ret = CMSRET_OBJECT_NOT_FOUND;
   }

   return ret;
}


void cgiGetQosPortShapingInfo_dev2(int argc __attribute((unused)),
                                   char **argv __attribute((unused)),
                                   char *varValue)
{
   char *p = varValue;
   SINT32 rateKbps = -1, shapingRate = -1;
   UINT32 shapingBurstSize = 0;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2EthernetInterfaceObject *ethIntfObj = NULL;
   MdmPathDescriptor pathDesc;

   /* First loop through all ethernet intf looking for WAN interfaces */
   while (cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      if (ethIntfObj->enable && ethIntfObj->upstream)
      {
         INIT_PATH_DESCRIPTOR(&pathDesc);
         pathDesc.oid = MDMOID_DEV2_ETHERNET_INTERFACE;
         pathDesc.iidStack = iidStack;

         getQosShaperInfoByPathDescriptor(&pathDesc,
                                          &shapingRate,
                                          &shapingBurstSize);

         if (shapingRate != -1)
         {
            rateKbps = shapingRate / 1000;
         }
         else
         {
            rateKbps = -1;
         }

         p += sprintf(p, "               <tr>\n");
         p += sprintf(p, "                  <td class='hd'>%s</td>\n", ethIntfObj->name);
         p += sprintf(p, "                  <td>WAN</td>\n");
         p += sprintf(p, "                  <td><input type='text' name='%sWanRateCfg' maxlength='16' size='16' value='%d'></td>\n", 
                      ethIntfObj->name, rateKbps);
         p += sprintf(p, "                  <td><input type='text' name='%sWanSizeCfg' maxlength='16' size='16' value='%d'></td>\n", 
                      ethIntfObj->name, shapingBurstSize);
         p += sprintf(p, "               </tr>");
      }

      cmsObj_free((void **) &ethIntfObj);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);

   /* Now loop through all ethernet intf looking for LAN interfaces */
   while (cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      if (ethIntfObj->enable && !ethIntfObj->upstream)
      {
         INIT_PATH_DESCRIPTOR(&pathDesc);
         pathDesc.oid = MDMOID_DEV2_ETHERNET_INTERFACE;
         pathDesc.iidStack = iidStack;

         getQosShaperInfoByPathDescriptor(&pathDesc,
                                          &shapingRate,
                                          &shapingBurstSize);

         if (shapingRate != -1)
         {
            rateKbps = shapingRate / 1000;
         }
         else
         {
            rateKbps = -1;
         }

         p += sprintf(p, "               <tr>\n");
         p += sprintf(p, "                  <td class='hd'>%s</td>\n", ethIntfObj->name);
         p += sprintf(p, "                  <td>LAN</td>\n");
         p += sprintf(p, "                  <td><input type='text' name='%sLanRateCfg' maxlength='16' size='16' value='%d'></td>\n", 
                      ethIntfObj->name, rateKbps);
         p += sprintf(p, "                  <td><input type='text' name='%sLanSizeCfg' maxlength='16' size='16' value='%d'></td>\n", 
                      ethIntfObj->name, shapingBurstSize);
         p += sprintf(p, "               </tr>");
      }

      cmsObj_free((void **) &ethIntfObj);
   }
   
   *p = 0;
}

#endif  /* DMP_DEVICE2_QOS_1 */

#endif  /* SUPPORT_QOS */


