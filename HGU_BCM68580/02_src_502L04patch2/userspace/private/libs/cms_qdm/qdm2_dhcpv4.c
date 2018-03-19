/***********************************************************************
 *
 *  Copyright (c) 2012  Broadcom Corporation
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

#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_qdm.h"
#include "cms_util.h"


#ifdef DMP_DEVICE2_DHCPV4RELAY_1
CmsRet qdmDhcpv4Relay_getInfo_dev2(const char *ipIntfFullPath,
                                   UBOOL8 *enabled,
                                   char *relayServerIpAddr)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Dhcpv4RelayForwardingObject *forwardingObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found=FALSE;

   cmsLog_debug("Entered: ipIntfFullPath=%s", ipIntfFullPath);

   while (!found &&
          cmsObj_getNext(MDMOID_DEV2_DHCPV4_RELAY_FORWARDING, &iidStack, (void **) &forwardingObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(forwardingObj->interface, ipIntfFullPath) == 0)
      {
         found=TRUE;
         if (enabled)
         {
            *enabled = forwardingObj->enable;
         }
         if (relayServerIpAddr)
         {
            cmsUtl_strcpy(relayServerIpAddr, forwardingObj->DHCPServerIPAddress);
         }
      }

      cmsObj_free((void **)&forwardingObj);
   }

   if (!found)
   {
      ret = CMSRET_OBJECT_NOT_FOUND;
   }

   return ret;
}

#endif  /* DMP_DEVICE2_DHCPV4RELAY_1 */


CmsRet qdmDhcpv4Client_getSentOption_dev2(const char *ipIntfFullPath,
                                          UINT32 tagNum,
                                          char *option,
                                          UINT32 optionLen)
{
   UBOOL8 found = FALSE;
   UINT8 *buf = NULL;
   UINT32 len = 0;
   Dev2Dhcpv4ClientObject *dhcp4ClientObj = NULL;
   Dev2Dhcpv4ClientSentOptionObject *sentOption = NULL;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   if (option == NULL || optionLen == 0)
   {
      cmsLog_error("option cannot be NULL");
      return CMSRET_INVALID_ARGUMENTS;
   }

   while (found == FALSE && 
          (ret = cmsObj_getNextFlags(MDMOID_DEV2_DHCPV4_CLIENT,
                                     &parentIidStack, 
                                     OGF_NO_VALUE_UPDATE,
                                     (void **)&dhcp4ClientObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(dhcp4ClientObj->interface, ipIntfFullPath))
      {
         found = TRUE;
      }

      cmsObj_free((void **)&dhcp4ClientObj);
   }     
 
   if (found == FALSE)
   {
      cmsLog_error("Fail to find the dhcp client info for %s", ipIntfFullPath);
      return CMSRET_INTERNAL_ERROR;
   }

   found = FALSE;

   while (found == FALSE && 
          cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_DHCPV4_CLIENT_SENT_OPTION,
                                       &parentIidStack, 
                                       &iidStack, 
                                       OGF_NO_VALUE_UPDATE,
                                       (void **)&sentOption) == CMSRET_SUCCESS)
   {
      if (sentOption->tag == tagNum)
      {
         switch (tagNum)
         {
            case 125:
               found = TRUE;
               if (sentOption->enable == TRUE)
               {
                  cmsUtl_strcpy(option, "1");
               }
	       else
               {
                  cmsUtl_strcpy(option, "0");
               }
               break;
            case 61:
               if (sentOption->enable == TRUE && sentOption->value != NULL)
               {
                  if (optionLen == BUFLEN_8)
                  {
                     if ( (UINT32) cmsUtl_strlen(sentOption->value) == optionLen)
                     {
                        found = TRUE;
                        cmsUtl_strncpy(option, sentOption->value, optionLen);
                     }
                  }
                  else
                  {
                     found = TRUE;
                     cmsUtl_strcpy(option, sentOption->value);
                  }
               }
               break;
            case 60:
            case 77:
               found = TRUE;
               if (sentOption->enable == TRUE && sentOption->value != NULL)
               {
                  cmsUtl_hexStringToBinaryBuf(sentOption->value, &buf, &len);
                  if (buf != NULL)
                  {
                     if (len > optionLen)
                        len = optionLen;
                     memcpy(option, buf, len);
                     CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
                  }
               }
               break;
         }
      }

      cmsObj_free((void **)&sentOption);
   }     

   if (found == FALSE)
   {
      ret = CMSRET_OBJECT_NOT_FOUND;
   }

   cmsLog_debug("tag=%d, option='%s', ret=%d", tagNum, option, ret);

   return ret;
}


CmsRet qdmDhcpv4Client_getReqOption_dev2(const char *ipIntfFullPath,
                                         UINT32 tagNum,
                                         char *option,
                                         UINT32 optionLen)
{
   UBOOL8 found = FALSE;
   Dev2Dhcpv4ClientObject *dhcp4ClientObj = NULL;
   Dev2Dhcpv4ClientReqOptionObject *reqOption = NULL;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   if (option == NULL || optionLen == 0)
   {
      cmsLog_error("option cannot be NULL");
      return CMSRET_INVALID_ARGUMENTS;
   }

   while (found == FALSE && 
          cmsObj_getNextFlags(MDMOID_DEV2_DHCPV4_CLIENT,
                              &parentIidStack, 
                              OGF_NO_VALUE_UPDATE,
                              (void **)&dhcp4ClientObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(dhcp4ClientObj->interface, ipIntfFullPath))
      {
         found = TRUE;
      }

      cmsObj_free((void **)&dhcp4ClientObj);
   }     
 
   if (found == FALSE)
   {
      cmsLog_error("Fail to find the dhcp client info for %s", ipIntfFullPath);
      return CMSRET_INTERNAL_ERROR;
   }

   found = FALSE;

   while (found == FALSE && 
          cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_DHCPV4_CLIENT_REQ_OPTION,
                                       &parentIidStack, 
                                       &iidStack, 
                                       OGF_NO_VALUE_UPDATE,
                                       (void **)&reqOption) == CMSRET_SUCCESS)
   {
      if (reqOption->tag == tagNum)
      {
         switch (tagNum)
         {
            case 50:
            case 51:
            case 54:
               if (reqOption->enable == TRUE &&
                   reqOption->X_BROADCOM_COM_Value != NULL)
               {
                  found = TRUE;

                  cmsUtl_strncpy(option,
                                 reqOption->X_BROADCOM_COM_Value,
                                 optionLen);
               }
               break;
         }
      }

      cmsObj_free((void **)&reqOption);
   }     

   if (found == FALSE)
   {
      ret = CMSRET_OBJECT_NOT_FOUND;
   }

   cmsLog_debug("tag=%d, option='%s', ret=%d", tagNum, option, ret);

   return ret;
}


#endif /* DMP_DEVICE2_BASELINE_1 */

