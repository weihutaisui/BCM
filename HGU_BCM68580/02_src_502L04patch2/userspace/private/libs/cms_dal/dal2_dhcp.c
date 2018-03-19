/*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
:>
*/

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"
#include "dal.h"
#include "dal2_wan.h"

#ifdef DMP_DEVICE2_BASELINE_1    /* this file touches TR181 objects */


#ifdef DMP_DEVICE2_DHCPV4RELAY_1
CmsRet dalDhcpv4Relay_set_dev2(const char *ipIntfFullPath,
                               UBOOL8 enable,
                               const char *relayServerIpAddr)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Dhcpv4RelayForwardingObject *forwardingObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found=FALSE;

   cmsLog_debug("Entered: ipIntfFullPath=%s enable=%d ipAddr=%s",
                ipIntfFullPath, enable, relayServerIpAddr);

   /* See if we can find an existing entry first */
   while (!found &&
          cmsObj_getNext(MDMOID_DEV2_DHCPV4_RELAY_FORWARDING, &iidStack, (void **) &forwardingObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(forwardingObj->interface, ipIntfFullPath) == 0 )
      {
         found=TRUE;
         /* don't free object, exit while loop now */
         break;
      }

      cmsObj_free((void **)&forwardingObj);
   }

   if (!found)
   {
      /* if not enable, then we don't care if there is no entry, we're done */
      if (!enable)
      {
         return CMSRET_SUCCESS;
      }

      /* need to create a new entry */
      INIT_INSTANCE_ID_STACK(&iidStack);
      if ((ret = cmsObj_addInstance(MDMOID_DEV2_DHCPV4_RELAY_FORWARDING, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to add MDMOID_DEV2_DHCPV4_RELAY_FORWARDING, ret = %d", ret);
         return ret;
      }

      if ((ret = cmsObj_get(MDMOID_DEV2_DHCPV4_RELAY_FORWARDING, &iidStack, 0, (void **) &forwardingObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get MDMOID_DEV2_DHCPV4_RELAY_FORWARDING, ret = %d", ret);
         cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_RELAY_FORWARDING, &iidStack);
         return ret;
      }
   }

   /* at this point, we have the correct relay forwarding object */
   forwardingObj->enable = enable;
   CMSMEM_REPLACE_STRING(forwardingObj->interface, ipIntfFullPath);
   if (relayServerIpAddr != NULL)
   {
      CMSMEM_REPLACE_STRING(forwardingObj->DHCPServerIPAddress, relayServerIpAddr);
   }
   else
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(forwardingObj->DHCPServerIPAddress);
   }


   ret = cmsObj_set(forwardingObj, &iidStack);
   cmsObj_free((void **) &forwardingObj);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set DHCPV4_RELAY_FORWARDING obj. ret=%d", ret);
      if (!found)
      {
         cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_RELAY_FORWARDING, &iidStack);
      }
   }

   return ret;
}
#endif  /* DMP_DEVICE2_DHCPV4RELAY_1 */


static CmsRet addIpIntfClientSentOption(UINT32 tagNum,
                                    const void *value,
                                    const char *ipIntfFullPath)
{
   char alias[BUFLEN_48];
   UBOOL8 found = FALSE;
   Dev2Dhcpv4ClientObject *dhcp4ClientObj = NULL;
   Dev2Dhcpv4ClientSentOptionObject *sentOption = NULL;
   InstanceIdStack clientIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack sentOptionIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("enter");

   if (ipIntfFullPath == NULL)
   {
      cmsLog_error("Full path of DHCPv4 client interface is NULL");
      return CMSRET_INVALID_ARGUMENTS;
   }

   if (value == NULL)
   {
      cmsLog_error("Option value is NULL");
      return CMSRET_INVALID_ARGUMENTS;
   }

   while(found == FALSE && 
         (ret = cmsObj_getNextFlags(MDMOID_DEV2_DHCPV4_CLIENT,
                                    &clientIidStack, 
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

   /* Loop through Dev2Dhcpv4ClientSentOptionObject
    * to find object that has the same tag */
   while (found == FALSE &&
          cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_DHCPV4_CLIENT_SENT_OPTION,
                                       &clientIidStack, 
                                       &sentOptionIidStack, 
                                       OGF_NO_VALUE_UPDATE,
                                       (void **)&sentOption) == CMSRET_SUCCESS)
   {
      if (sentOption->tag == tagNum)
      {
         found = TRUE;
         /* do not free sentOption when it's found
            since it's used and freed later */
         break;
      }

      cmsObj_free((void **)&sentOption);
   }

   /* Dev2Dhcpv4ClientReqOptionObject with the
      same tag does not exist, so add new object */
   if (found == FALSE)
   {
      /* need to create a new entry */
      memcpy(&sentOptionIidStack, &clientIidStack, sizeof(InstanceIdStack));

      if ((ret = cmsObj_addInstance(MDMOID_DEV2_DHCPV4_CLIENT_SENT_OPTION,
                                    &sentOptionIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to add Dev2Dhcpv4ClientSentOptionObject instance, ret = %d", ret);
         return ret;
      } 

      if ((ret = cmsObj_get(MDMOID_DEV2_DHCPV4_CLIENT_SENT_OPTION,
                            &sentOptionIidStack,
                            0,
                            (void **) &sentOption)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get Dev2Dhcpv4ClientSentOptionObject instance, ret = %d", ret);
         cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_CLIENT_SENT_OPTION, &sentOptionIidStack);
         return ret;
      }

      memset(alias, 0, BUFLEN_48);
      sprintf(alias, "cpe-dhcp-option-%d", tagNum);

      CMSMEM_REPLACE_STRING(sentOption->alias, alias);
      sentOption->tag = tagNum;
   }

   switch (tagNum)
   {
      case 125:
         sentOption->enable = (UBOOL8)(*(UBOOL8 *)value);
         break;
      case 61:
         sentOption->enable = TRUE;
         CMSMEM_REPLACE_STRING(sentOption->value, value);
         break;
      case 60:
      case 77:
         sentOption->enable = TRUE;
         if (sentOption->value != NULL)
         {
            cmsMem_free(sentOption->value);
            sentOption->value = NULL;
         }
         cmsUtl_binaryBufToHexString((const UINT8 *)value,
                                     cmsUtl_strlen((const char *)value),
                                     &(sentOption->value));
         break;
   }

   /* modify entry */
   ret = cmsObj_set(sentOption, &sentOptionIidStack);

   /* free entry */
   cmsObj_free((void **) &sentOption); 

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set Dev2Dhcpv4ClientSentOptionObject instance. ret=%d", ret);
      if (found == FALSE)
      {
         /* only delete a new entry */
         cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_CLIENT_SENT_OPTION, &sentOptionIidStack);
      }
   }     

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}


static CmsRet addIpIntfClientReqOption(UINT32 tagNum,
                                       const void *value,
                                       const char *ipIntfFullPath)
{
   char alias[BUFLEN_48];
   char buf[BUFLEN_16];
   UBOOL8 found = FALSE;
   Dev2Dhcpv4ClientObject *dhcp4Client = NULL;
   Dev2Dhcpv4ClientReqOptionObject *reqOption = NULL;
   InstanceIdStack clientIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack reqOptionIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("enter");

   if (tagNum != 50 && tagNum != 51 && tagNum != 54)
   {
      cmsLog_error("Invalid tag number %d.", tagNum);
      return CMSRET_INVALID_ARGUMENTS;
   }

   if (ipIntfFullPath == NULL)
   {
      cmsLog_error("Full path of DHCPv4 client interface is NULL");
      return CMSRET_INVALID_ARGUMENTS;
   }

   if (value == NULL)
   {
      cmsLog_error("Option value is NULL");
      return CMSRET_INVALID_ARGUMENTS;
   }

   while(found == FALSE && 
         (ret = cmsObj_getNextFlags(MDMOID_DEV2_DHCPV4_CLIENT,
                                    &clientIidStack, 
                                    OGF_NO_VALUE_UPDATE,
                                    (void **)&dhcp4Client)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(dhcp4Client->interface, ipIntfFullPath))
      {
         found = TRUE;
      }

      cmsObj_free((void **)&dhcp4Client);
   }     
 
   if (found == FALSE)
   {
      cmsLog_error("Fail to find the dhcp client info for %s", ipIntfFullPath);
      return CMSRET_INTERNAL_ERROR;
   }

   found = FALSE;

   /* Loop through Dev2Dhcpv4ClientReqOptionObject
    * to find object that has the same tag */
   while (found == FALSE &&
          cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_DHCPV4_CLIENT_REQ_OPTION,
                                       &clientIidStack, 
                                       &reqOptionIidStack, 
                                       OGF_NO_VALUE_UPDATE,
                                       (void **)&reqOption) == CMSRET_SUCCESS)
   {
      if (reqOption->tag == tagNum)
      {
         found = TRUE;
         /* do not free reqOption when it's found
            since it's used and freed later */
         break;
      }

      cmsObj_free((void **)&reqOption);
   }

   /* Dev2Dhcpv4ClientReqOptionObject with the
      same tag does not exist, so add new object */
   if (found == FALSE)
   {
      /* need to create a new entry */
      memcpy(&reqOptionIidStack, &clientIidStack, sizeof(InstanceIdStack));

      if ((ret = cmsObj_addInstance(MDMOID_DEV2_DHCPV4_CLIENT_REQ_OPTION,
                                    &reqOptionIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to add Dev2Dhcpv4ClientReqOptionObject instance, ret = %d", ret);
         return ret;
      } 

      if ((ret = cmsObj_get(MDMOID_DEV2_DHCPV4_CLIENT_REQ_OPTION,
                            &reqOptionIidStack,
                            0,
                            (void **) &reqOption)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get Dev2Dhcpv4ClientReqOptionObject instance, ret = %d", ret);
         cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_CLIENT_REQ_OPTION, &reqOptionIidStack);
         return ret;
      }

      memset(alias, 0, BUFLEN_48);
      sprintf(alias, "cpe-dhcp-option-%d", tagNum);

      CMSMEM_REPLACE_STRING(reqOption->alias, alias);
      reqOption->tag = tagNum;
      reqOption->enable = TRUE;
   }

   switch (tagNum)
   {
      case 50:
      case 54:
         CMSMEM_REPLACE_STRING(reqOption->X_BROADCOM_COM_Value, (char *)value);
         break;
      case 51:
         memset(buf, 0, BUFLEN_16);
         snprintf(buf, BUFLEN_16, "%d", (UINT32)(*(UINT32 *)value));
         CMSMEM_REPLACE_STRING(reqOption->X_BROADCOM_COM_Value, buf);
         break;
   }

   /* modify entry */
   ret = cmsObj_set(reqOption, &reqOptionIidStack);

   /* free entry */
   cmsObj_free((void **) &reqOption); 

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set Dev2Dhcpv4ClientReqOptionObject instance. ret=%d", ret);
      if (found == FALSE)
      {
         /* only delete a new entry */
         cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_CLIENT_REQ_OPTION, &reqOptionIidStack);
      }
   }     

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}


static CmsRet addIpIntfClientOptions(const char *ipIntfFullPath,
                                     const char *dhcpcOp60VID,
                                     const char *dhcpcOp61DUID,
                                     const char *dhcpcOp61IAID,
                                     const char *dhcpcOp77UID,
                                     UBOOL8 dhcpcOp125Enabled,
                                     const char *dhcpcOp50IpAddress,
                                     const char *dhcpcOp54ServerIpAddress,
                                     UINT32 dhcpcOp51LeasedTime)
{
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("enter");

   if (!IS_EMPTY_STRING(dhcpcOp60VID))
      ret = addIpIntfClientSentOption(60,
                                      (const void *)dhcpcOp60VID,
                                      ipIntfFullPath);

   if (!IS_EMPTY_STRING(dhcpcOp61DUID))
      ret = addIpIntfClientSentOption(61,
                                      (const void *)dhcpcOp61DUID,
                                      ipIntfFullPath);

   if (!IS_EMPTY_STRING(dhcpcOp61IAID))
      ret = addIpIntfClientSentOption(61,
                                      (const void *)dhcpcOp61IAID,
                                      ipIntfFullPath);

   if (!IS_EMPTY_STRING(dhcpcOp77UID))
      ret = addIpIntfClientSentOption(77,
                                      (const void *)dhcpcOp77UID,
                                      ipIntfFullPath);

   ret = addIpIntfClientSentOption(125,
                                   (const void *)&dhcpcOp125Enabled,
                                   ipIntfFullPath);

   if (!IS_EMPTY_STRING(dhcpcOp50IpAddress))
      ret = addIpIntfClientReqOption(50,
                                     (const void *)dhcpcOp50IpAddress,
                                     ipIntfFullPath);

   if (dhcpcOp51LeasedTime != 0)
      ret = addIpIntfClientReqOption(51,
                                     (const void *)&dhcpcOp51LeasedTime,
                                     ipIntfFullPath);

   if (!IS_EMPTY_STRING(dhcpcOp54ServerIpAddress))
      ret = addIpIntfClientReqOption(54,
                                     (const void *)dhcpcOp54ServerIpAddress,
                                     ipIntfFullPath);

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}


CmsRet dalDhcp_addIpIntfClient_dev2(const char * ipIntfPathRef,
                                    const char * dhcpcOp60VID,
                                    const char * dhcpcOp61DUID,
                                    const char * dhcpcOp61IAID,
                                    const char * dhcpcOp77UID,
                                    UBOOL8 dhcpcOp125Enabled,
                                    const char *dhcpcOp50IpAddress,
                                    const char *dhcpcOp54ServerIpAddress,
                                    UINT32 dhcpcOp51LeasedTime)
{                                        
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2Dhcpv4ClientObject *dhcp4ClientObj=NULL;
   CmsRet ret;
   
   cmsLog_debug("enter");
   
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_DHCPV4_CLIENT, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add MDMOID_DEV2_DHCPV4_CLIENT Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_DHCPV4_CLIENT, &iidStack, 0, (void **) &dhcp4ClientObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get dhcp4ClientObj, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_CLIENT, &iidStack);
      return ret;
   }

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to addIpIntfClientSentOptions. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_CLIENT, &iidStack);
      cmsObj_free((void **) &dhcp4ClientObj); 
      return ret;
   }     

   // enable to start dhcpc
   dhcp4ClientObj->enable = TRUE;

   CMSMEM_REPLACE_STRING(dhcp4ClientObj->interface, ipIntfPathRef);

   ret = cmsObj_set(dhcp4ClientObj, &iidStack);
   cmsObj_free((void **) &dhcp4ClientObj); 

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set dhcp4ClientObj. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_CLIENT, &iidStack);
      return ret;
   }     
   
   // add client options if any
   ret = addIpIntfClientOptions(ipIntfPathRef,
                                dhcpcOp60VID,
                                dhcpcOp61DUID,
                                dhcpcOp61IAID,
                                dhcpcOp77UID,
                                dhcpcOp125Enabled,
                                dhcpcOp50IpAddress,
                                dhcpcOp54ServerIpAddress,
                                dhcpcOp51LeasedTime);

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;

}


CmsRet dalDhcp_deleteIpIntfClient_dev2(const char * ipIntffullPath)
{                                        
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   Dev2Dhcpv4ClientObject *dhcp4ClientObj=NULL;
   CmsRet ret=CMSRET_INVALID_ARGUMENTS;
   
   if (ipIntffullPath == NULL)
   {
      cmsLog_error("NULL string.");
      return ret;
   }
   cmsLog_debug("Enter ipIntffullPath %s.", ipIntffullPath);
   
   while(!found && 
         (ret = cmsObj_getNextFlags(MDMOID_DEV2_DHCPV4_CLIENT,
                             &iidStack, 
                             OGF_NO_VALUE_UPDATE,
                             (void **)&dhcp4ClientObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcasecmp(dhcp4ClientObj->interface, ipIntffullPath))
      {
         cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_CLIENT, &iidStack);
         found = TRUE;
      }
      cmsObj_free((void **)&dhcp4ClientObj);
   }    
   
   cmsLog_debug("Exit, ret=%d", ret);

   return ret;

}

#endif /* DMP_DEVICE2_BASELINE_1 */
