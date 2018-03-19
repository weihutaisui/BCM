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


#ifdef DMP_DEVICE2_BASELINE_1


#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal.h"



CmsRet dalEth_addEthernetLink_dev2(const char *lowerLayer,
                                   char *myPathRef,
                                   UINT32 bufLen)
{
   char *fullPathStringPtr=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   Dev2EthernetLinkObject *ethLinkObj = NULL;
   MdmPathDescriptor ethLinkPathDesc;
   CmsRet ret;

   cmsLog_debug("lowerLayer %s", lowerLayer);
   
   /* Create an Eth.link object first for ip/ppp interface objects
   */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_ETHERNET_LINK, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add ethLink Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_ETHERNET_LINK, &iidStack, 0, (void **) &ethLinkObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get ethLinkObj, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_ETHERNET_LINK, &iidStack);
      return ret;
   }

   CMSMEM_REPLACE_STRING(ethLinkObj->lowerLayers, lowerLayer);
   ethLinkObj->enable = TRUE;

   /* MacAddress need to be set when the layer 2 link is up in rcl/rut */
   ret =  cmsObj_set(ethLinkObj, &iidStack);
   cmsObj_free((void **) &ethLinkObj); 

   if (ret  != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set ethLinkObj. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_ETHERNET_LINK, &iidStack);
      return ret;
   } 

   /* Need to get the eth.link object full path string to be used as ip/ppp Intf lowlayerStr */
   INIT_PATH_DESCRIPTOR(&ethLinkPathDesc);
   ethLinkPathDesc.iidStack = iidStack;
   ethLinkPathDesc.oid = MDMOID_DEV2_ETHERNET_LINK;
   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&ethLinkPathDesc, &fullPathStringPtr)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_ETHERNET_LINK, &iidStack);
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

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;

}


CmsRet dalEth_deleteEthernetLinkByName_dev2(const char *ethLinkName)
{                                        
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   Dev2EthernetLinkObject *ethLinkObj = NULL;
   CmsRet ret=CMSRET_INVALID_ARGUMENTS;
   
   cmsLog_debug("Enter ethLinkName %s.", ethLinkName);

   while(!found && 
         (ret = cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_LINK,
                                    &iidStack, 
                                    OGF_NO_VALUE_UPDATE,
                                    (void **)&ethLinkObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ethLinkObj->name, ethLinkName))
      {
         cmsObj_deleteInstance(MDMOID_DEV2_ETHERNET_LINK, &iidStack);
         found = TRUE;
      }

      cmsObj_free((void **)&ethLinkObj);
   }    
   
   cmsLog_debug("Exit, ret=%d", ret);

   return ret;

}


CmsRet dalEth_addEthernetVlanTermination_dev2(const WEB_NTWK_VAR *webVar,
                                              const char *lowerLayer,
                                              char *myPathRef,
                                              UINT32 bufLen)
{
   char *fullPathStringPtr=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   Dev2VlanTerminationObject *ethVlanObj = NULL;
   MdmPathDescriptor ethVlanTerminationPathDesc;
   CmsRet ret;

   cmsLog_debug("lowerLayer %s", lowerLayer);
   
   /* Create an Eth.VlanTermination object first for ip/ppp interface objects
   */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_VLAN_TERMINATION, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add VLAN_TERMINATION Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_VLAN_TERMINATION, &iidStack, 0, (void **) &ethVlanObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get VLAN_TERMINATION object, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_VLAN_TERMINATION, &iidStack);
      return ret;
   }

   CMSMEM_REPLACE_STRING(ethVlanObj->lowerLayers, lowerLayer);
   ethVlanObj->VLANID = webVar->vlanMuxId;
   ethVlanObj->X_BROADCOM_COM_Vlan8021p = webVar->vlanMuxPr;
   ethVlanObj->TPID = webVar->vlanTpid;

   ethVlanObj->enable = TRUE;

   ret =  cmsObj_set(ethVlanObj, &iidStack);
   cmsObj_free((void **) &ethVlanObj); 

   if (ret  != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set ethVlanObj. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_VLAN_TERMINATION, &iidStack);
      return ret;
   } 

      /* Need to get the eth.vlantermination object full path string to be used as ip/ppp Intf lowlayerStr */
   INIT_PATH_DESCRIPTOR(&ethVlanTerminationPathDesc);
   ethVlanTerminationPathDesc.iidStack = iidStack;
   ethVlanTerminationPathDesc.oid = MDMOID_DEV2_VLAN_TERMINATION;
   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&ethVlanTerminationPathDesc, &fullPathStringPtr)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_VLAN_TERMINATION, &iidStack);
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


   cmsLog_debug("Exit, ret=%d", ret);

   return ret;

}


CmsRet dalEth_deleteEthernetVlanTermination_dev2(char *ethLinkFullPath)
{                                        
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   char *objPath = NULL;
   Dev2VlanTerminationObject *ethVlanObj = NULL;
   CmsRet ret=CMSRET_INVALID_ARGUMENTS;
   
   if (ethLinkFullPath == NULL)
   {
      cmsLog_error("NULL string.");
      return ret;
   }
   cmsLog_debug("Enter ethLinkFullPath %s.", ethLinkFullPath);

   while(!found && 
         (ret = cmsObj_getNextFlags(MDMOID_DEV2_VLAN_TERMINATION,
                                    &iidStack, 
                                    OGF_NO_VALUE_UPDATE,
                                    (void **)&ethVlanObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcasecmp(ethVlanObj->lowerLayers, ethLinkFullPath))
      {
         MdmPathDescriptor pathDesc = EMPTY_PATH_DESCRIPTOR;
            
         pathDesc.oid = MDMOID_DEV2_VLAN_TERMINATION;
         pathDesc.iidStack = iidStack;
         if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &objPath)) == CMSRET_SUCCESS)
         {
            /* save the lowerlayer pointer */
            if (objPath != NULL)
            {
               cmsLog_debug("Got eth.vlan full path %s and deleting eth.vlan obj...", objPath);
               cmsUtl_strncpy(ethLinkFullPath, objPath, MDM_SINGLE_FULLPATH_BUFLEN);
               CMSMEM_FREE_BUF_AND_NULL_PTR(objPath);
            }

            cmsObj_deleteInstance(MDMOID_DEV2_VLAN_TERMINATION, &iidStack);
         }
         else
         {
            cmsLog_error("Could not form fullpath to %d %s", pathDesc.oid, cmsMdm_dumpIidStack(&pathDesc.iidStack));
         }

         found = TRUE;
      }

      cmsObj_free((void **)&ethVlanObj);
   }    
   
   cmsLog_debug("Exit, ret=%d", ret);

   return ret;

}

#endif  /* DMP_DEVICE2_BASELINE_1 */

