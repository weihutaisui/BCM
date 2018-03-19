/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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
 
#ifdef DMP_DEVICE2_BASELINE_1 /* this file touches TR181 objects */

#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_phl.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_core.h"


/* there are 2 two copies of this function: this copy is used only in pure TR181 mode */
/* hopefully all functions in this file will be in this category, but there is no requirement for them to be */

CmsRet qdmEthLink_getEthLinkLowerLayerFullPathByName_dev2(const char *ethLinkName,
                                                          char *lowerLayerBuf, 
                                                          UINT32 bufLen)
{
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   Dev2EthernetLinkObject *ethLinkObj = NULL;
   CmsRet ret;

   cmsLog_debug("lowerLayerBuf %s", lowerLayerBuf);

   if (ethLinkName == NULL || lowerLayerBuf == NULL || bufLen <= 0)
   {
      cmsLog_error("Invalid aruments.");
      return CMSRET_INVALID_ARGUMENTS;
   }

   while ((!found) &&
          ((ret = cmsObj_getNext(MDMOID_DEV2_ETHERNET_LINK, &iidStack, (void **) &ethLinkObj)) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strncmp(ethLinkObj->name, ethLinkName, CMS_IFNAME_LENGTH))
      {
         found = TRUE;
         cmsUtl_strncpy(lowerLayerBuf, ethLinkObj->lowerLayers, bufLen);
      }         
      cmsObj_free((void **) &ethLinkObj);
   }

   return ret;
   
}  /* End of qdmEthLink_getEthLinkLowerLayerFullPathByName_dev2() */


CmsRet qdmEth_getIntfInfoByNameLocked_dev2(const char *ifname,
                                           char       *status,
                                           UBOOL8     *upstream,
                                           SINT32     *shapingRate,
                                           UINT32     *shapingBurstSize)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2EthernetInterfaceObject *ethIntf=NULL;
   UBOOL8 found=FALSE;
   CmsRet ret=CMSRET_INTERNAL_ERROR;
   
   if (cmsUtl_strlen(ifname) == 0)
   {
      cmsLog_error("no Eth ifName, just return");
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   cmsLog_debug("Looking for Eth interface info for %s", ifname);

   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE, 
                                &iidStack,
                                (void **) &ethIntf)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ethIntf->name, ifname))
      {
         found = TRUE;
         
         cmsUtl_strcpy(status, ethIntf->status);
         *upstream         = ethIntf->upstream; 
         *shapingRate      = ethIntf->X_BROADCOM_COM_ShapingRate;
         *shapingBurstSize = ethIntf->X_BROADCOM_COM_ShapingBurstSize;
      }
      cmsObj_free((void **) &ethIntf);
   }

   if (!found)
   {
      cmsLog_error("Cannot find Eth interface %s", ifname);
      return CMSRET_OBJECT_NOT_FOUND;
   }
   
   cmsLog_debug("Exit. ret %d", ret);

   return ret;

} /* End of qdmEth_getIntfInfoByNameLocked_dev2() */

#ifdef DMP_DEVICE2_VLANTERMINATION_1

CmsRet qdmEth_getEthernetVlanTermination_dev2(SINT32 vlanId,
                                              const char *lowerLayer,
                                              char *myPathRef,
                                              UINT32 bufLen)
{
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   Dev2VlanTerminationObject *ethVlanObj = NULL;
   CmsRet ret;

   cmsLog_debug("lowerLayer %s", lowerLayer);
   
   if (lowerLayer == NULL || myPathRef == NULL || bufLen <= 0)
   {
      cmsLog_error("Invalid aruments.");
      return CMSRET_INVALID_ARGUMENTS;
   }


   while ((!found) &&
          ((ret = cmsObj_getNext(MDMOID_DEV2_VLAN_TERMINATION, &iidStack, (void **) &ethVlanObj)) == CMSRET_SUCCESS))
   {
      if (ethVlanObj->VLANID == vlanId && !cmsUtl_strcmp(ethVlanObj->lowerLayers, lowerLayer))
      {
         char *fullPathStringPtr=NULL;
         MdmPathDescriptor ethVlanPathDesc;
         
         found = TRUE;

         /* Need to get the eth.vlan object full path string to be used as ip/ppp Intf lowlayerStr */
         INIT_PATH_DESCRIPTOR(&ethVlanPathDesc);
         ethVlanPathDesc.iidStack = iidStack;
         ethVlanPathDesc.oid = MDMOID_DEV2_VLAN_TERMINATION;
         if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&ethVlanPathDesc, &fullPathStringPtr)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
            return ret;
         }
         if (cmsUtl_strlen(fullPathStringPtr) > (SINT32) bufLen)
         {
            ret = CMSRET_INTERNAL_ERROR;
         }
         else
         {
            cmsUtl_strncpy(myPathRef, fullPathStringPtr, bufLen);
            ret = CMSRET_SUCCESS;
         }
   
         CMSMEM_FREE_BUF_AND_NULL_PTR(fullPathStringPtr);
         
      }         
      cmsObj_free((void **) &ethVlanObj);
   }

   cmsLog_debug("Exit. ret %d", ret);

   return ret;
   
}


#endif /* DMP_DEVICE2_VLANTERMINATION_1 */

#ifdef SUPPORT_LANVLAN

CmsRet qdmEth_getAllL2EthIntfNameLocked_dev2(char *buffer, UINT32 bufSize)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2EthernetInterfaceObject *ethIntfObj=NULL;
   UBOOL8 first = TRUE;

   cmsLog_debug("=====> Entered");

   buffer[0] = '\0';

   while (cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("cmsObj_getNext returns ethIntfObj->name %s",
                   ethIntfObj->name);

      /* filter out the ethernet WAN port */
      if (!ethIntfObj->upstream)
      {
         if (!first)
         {
            cmsUtl_strncat(buffer, bufSize, "|");
         }
         else
         {
            first = FALSE;
         }
         cmsUtl_strncat(buffer, bufSize, ethIntfObj->name);
      }
      cmsObj_free((void **) &ethIntfObj);
   }

   return CMSRET_SUCCESS;
}
#endif /* SUPPORT_LANVLAN */
#endif /* DMP_DEVICE2_BASELINE_1 */

