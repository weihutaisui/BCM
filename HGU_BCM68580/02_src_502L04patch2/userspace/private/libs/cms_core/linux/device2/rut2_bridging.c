/***********************************************************************
 *
 *  Copyright (c) 2006-2011  Broadcom Corporation
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

#ifdef DMP_DEVICE2_BRIDGE_1


/*!\file rut2_bridging.c
 * \brief This file contains helper functions for the Device.Bridging. objects.
 * We may want to move the VLAN related functions out to another file if
 * we want more separation of the code.
 *
 */


#include "odl.h"
#include "cms_core.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_strconv2.h"
#include "rcl.h"
#include "rut_util.h"
#include "rut2_bridging.h"
#ifdef BRCM_WLAN
#include "rut2_wifi.h"
#endif




CmsRet rutBridge_getParentBridgeIntfName_dev2(const InstanceIdStack *iidStack,
                                              char *bridgeIntfName)
{
   InstanceIdStack brIidStack;
   Dev2BridgeObject *brObj=NULL;
   CmsRet ret;

   cmsLog_debug("Entered: iidStack=%s", cmsMdm_dumpIidStack(iidStack));

   /* The parent Bridge object is 1 level above the Port object. */
   brIidStack = *iidStack;
   POP_INSTANCE_ID(&brIidStack);

   ret = cmsObj_get(MDMOID_DEV2_BRIDGE, &brIidStack, 0, (void **)&brObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get parent Bridge obj");
      return ret;
   }

   if (IS_EMPTY_STRING(brObj->X_BROADCOM_COM_IfName))
   {
      cmsLog_error("Parent bridge object is still empty");
   }
   else
   {
      strcpy(bridgeIntfName, brObj->X_BROADCOM_COM_IfName);
      cmsLog_debug("Found parent bridge name=%s", bridgeIntfName);
   }

   cmsObj_free((void **)&brObj);

   return ret;
}


CmsRet rutBridge_addIntfNameToBridge_dev2(const char *intfName,
                                          const char *brIntfName)
{
   char *fullPath=NULL;
   CmsRet ret;

   /*
    * This function might be called from the intf grouping code, so the
    * intfName could be a WAN intfName with a VLANTermination object on top.
    * (Currently, all WAN intf -- including bridge -- has a VLANTermination
    * obj on top).  Check for that first.
    */
   {
      MdmPathDescriptor ethVlanPathDesc = EMPTY_PATH_DESCRIPTOR;
      InstanceIdStack ethVlanIidStack = EMPTY_INSTANCE_ID_STACK;
      Dev2VlanTerminationObject *ethVlanObj = NULL;
      UBOOL8 ethVlanFound=FALSE;

      while (!ethVlanFound &&
             cmsObj_getNextFlags(MDMOID_DEV2_VLAN_TERMINATION,
                                 &ethVlanIidStack,
                                 OGF_NO_VALUE_UPDATE,
                                 (void **) &ethVlanObj) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(ethVlanObj->name, intfName))
         {
            ethVlanFound = TRUE;

            /* get the fullpath to this vlantermination obj */
            ethVlanPathDesc.oid = MDMOID_DEV2_VLAN_TERMINATION;
            ethVlanPathDesc.iidStack = ethVlanIidStack;
            cmsMdm_pathDescriptorToFullPathNoEndDot(&ethVlanPathDesc, &fullPath);
         }

         cmsObj_free((void **) &ethVlanObj);
      }
   }

   if (!fullPath)
   {
      /* this must be a L2 intfName */
      ret = qdmIntf_intfnameToFullPathLocked(intfName, TRUE, &fullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_debug("Could not convert %s to L2 Fullpath, ret=%d", intfName, ret);
         return ret;
      }
   }

   cmsLog_debug("intfName %s ==> fullPath %s", intfName, fullPath);

   ret = rutBridge_addFullPathToBridge_dev2(fullPath, brIntfName);
   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);

#ifdef BRCM_WLAN
   {
           Dev2WifiSsidObject *ssidObj=NULL;
           InstanceIdStack ssidStack= EMPTY_INSTANCE_ID_STACK;
           if(rutWifi_getWlanSsidObjByIfName(intfName, &ssidObj, &ssidStack)) {
                   if(strncmp(ssidObj->X_BROADCOM_COM_WlBrName,brIntfName,strlen(brIntfName))) {
                       CMSMEM_REPLACE_STRING(ssidObj->X_BROADCOM_COM_WlBrName, brIntfName);
                       if( ( ret = cmsObj_set((void *)ssidObj, &ssidStack)) != CMSRET_SUCCESS )
                           cmsLog_error("Setting new bridgeName failed, ret=%d", ret);
                   }
                   cmsObj_free((void **)&ssidObj);
           }
   }
#endif
   return ret;
}


CmsRet rutBridge_addFullPathToBridge_dev2(const char *fullPath,
                                          const char *brIntfName)
{
   Dev2BridgeObject *brObj=NULL;
   Dev2BridgePortObject *brPortObj=NULL;
   Dev2BridgePortObject *brMgmtPortObj=NULL;
   InstanceIdStack brIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack brPortIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack brMgmtIidStack = EMPTY_INSTANCE_ID_STACK;
   char *brPortFullPath=NULL;
   UBOOL8 found=FALSE;
   CmsRet ret;

   cmsLog_debug("Entered: fullpath=%s brIntfName=%s", fullPath, brIntfName);

   /* First find the bridge */
   while (!found &&
          (ret = cmsObj_getNextFlags(MDMOID_DEV2_BRIDGE, &brIidStack,
                                     OGF_NO_VALUE_UPDATE,
                                     (void **) &brObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(brObj->X_BROADCOM_COM_IfName, brIntfName))
      {
         found = TRUE;
      }
      /* we don't need the bridge object, so always free it */
      cmsObj_free((void **) &brObj);
   }

   if (!found)
   {
      cmsLog_error("Could not find bridge %s", brIntfName);
      return ret;
   }


   /* create a new Bridge.{i}.Port.{i} object under the found bridge */
   brPortIidStack = brIidStack;
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_BRIDGE_PORT, &brPortIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add Bridge.Port Instance, ret = %d", ret);
      return ret;
   }

   /* create fullpath to this new Bridge.Port object */
   {
      MdmPathDescriptor pathDesc = EMPTY_PATH_DESCRIPTOR;
      pathDesc.oid = MDMOID_DEV2_BRIDGE_PORT;
      pathDesc.iidStack = brPortIidStack;
      ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &brPortFullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get fullpath for Bridge.Port. ret=%d", ret);
         cmsObj_deleteInstance(MDMOID_DEV2_BRIDGE_PORT, &brPortIidStack);
         return ret;
      }
   }

   /* Find the management port object for this bridge */
   found = FALSE;
   while (cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_BRIDGE_PORT,
                                       &brIidStack,
                                       &brMgmtIidStack,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &brMgmtPortObj) == CMSRET_SUCCESS)
   {
      if (brMgmtPortObj->managementPort &&
          !cmsUtl_strcmp(brMgmtPortObj->name, brIntfName))
      {
         /* need this obj, so break now and free it later */
         found = TRUE;
         break;
      }
      cmsObj_free((void **) &brMgmtPortObj);
   }

   if (!found)
   {
      cmsLog_error("Could not find management port for bridge %s", brIntfName);
      cmsObj_deleteInstance(MDMOID_DEV2_BRIDGE_PORT, &brPortIidStack);
      CMSMEM_FREE_BUF_AND_NULL_PTR(brPortFullPath);
      return ret;
   }
   else
   {
      /* add the brPortFullPath to lowerLayers param of the Bridge mgmt port */
      char allLowerLayersStringBuf[MDM_MULTI_FULLPATH_BUFLEN]={0};

      if (!IS_EMPTY_STRING(brMgmtPortObj->lowerLayers))
      {
         sprintf(allLowerLayersStringBuf, "%s", brMgmtPortObj->lowerLayers);
      }
      ret = cmsUtl_addFullPathToCSL(brPortFullPath, allLowerLayersStringBuf, sizeof(allLowerLayersStringBuf));
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to add %s to %s", brPortFullPath, allLowerLayersStringBuf);
      }
      else
      {
         CMSMEM_REPLACE_STRING_FLAGS(brMgmtPortObj->lowerLayers,
                                     allLowerLayersStringBuf,
                                     mdmLibCtx.allocFlags);

         cmsLog_debug("new mgmtPortLL(len=%d)=%s",
                      cmsUtl_strlen(brMgmtPortObj->lowerLayers),
                      brMgmtPortObj->lowerLayers);
         ret = cmsObj_set(brMgmtPortObj, &brMgmtIidStack);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set brMgmtPortObj. ret=%d", ret);
            cmsObj_deleteInstance(MDMOID_DEV2_BRIDGE_PORT, &brPortIidStack);
         }
      }

      cmsObj_free((void **) &brMgmtPortObj);
      CMSMEM_FREE_BUF_AND_NULL_PTR(brPortFullPath);
   }

   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }


   /*
    * Finally, now that everything is connected up correctly, get and set the
    * brPort object.
    */
   if ((ret = cmsObj_get(MDMOID_DEV2_BRIDGE_PORT, &brPortIidStack, 0, (void **) &brPortObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Bridge.Port, ret = %d", ret);
      return ret;
   }

   brPortObj->enable = TRUE;
   brPortObj->managementPort = FALSE;
   CMSMEM_REPLACE_STRING_FLAGS(brPortObj->lowerLayers, fullPath, mdmLibCtx.allocFlags);

   ret = cmsObj_set(brPortObj, &brPortIidStack);
   cmsObj_free((void **) &brPortObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set Bridge.Port. ret=%d", ret);
   }

   return ret;
}


void rutBridge_deleteIntfNameFromBridge_dev2(const char *intfName)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2BridgePortObject *portObj=NULL;
   UBOOL8 found=FALSE;

   cmsLog_debug("Entered: intfName=%s", intfName);

   /* find non-mgmt port with specified intfName */
   while (!found &&
          cmsObj_getNextFlags(MDMOID_DEV2_BRIDGE_PORT, &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &portObj) == CMSRET_SUCCESS)
   {
      if (!portObj->managementPort &&
          !cmsUtl_strcmp(portObj->name, intfName))
      {
         found = TRUE;

         /*
          * delete bridge port instance.  When this object is deleted,
          * an INTFSTACK_OBJECT_DELETED msg will get sent to ssk.
          * Ssk will update the lowerlayers param of the mgmt port, as
          * required by TR181 spec.  So no need to do that explictly here.
          */
         cmsObj_deleteInstance(MDMOID_DEV2_BRIDGE_PORT, &iidStack);
      }

      cmsObj_free((void **)&portObj);
   }

   if (!found)
   {
      cmsLog_debug("Could not find Bridge.Port obj %s", intfName);
   }

   return;
}


SINT32 rutLan_getNextAvailableBridgeNumber_dev2()
{
   UBOOL8 inUseArray[MAX_LAYER2_BRIDGES] = {FALSE};
   UINT32 i;
   Dev2BridgePortObject *brPortObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   /*
    * Enumerate through all of our bridges and for each bridge, mark that
    * bridge's slot in the inUseArray as TRUE;
    */
   while ((ret = cmsObj_getNextFlags(MDMOID_DEV2_BRIDGE_PORT, &iidStack,
                                     OGF_NO_VALUE_UPDATE,
                                     (void **) &brPortObj)) == CMSRET_SUCCESS)
   {
      if (brPortObj->managementPort &&
          !IS_EMPTY_STRING(brPortObj->name))
      {
         UINT32 bridgeNumber;
         bridgeNumber = atoi(&(brPortObj->name[2]));
         cmsAst_assert(bridgeNumber < MAX_LAYER2_BRIDGES);
         inUseArray[bridgeNumber] = TRUE;
      }
      cmsObj_free((void **) &brPortObj);
   }

   /*
    * Now go through the array and return the first available.
    */
   for (i=0; i < MAX_LAYER2_BRIDGES; i++)
   {
      if (inUseArray[i] == FALSE)
      {
         cmsLog_debug("returning %d", (SINT32) i);
         return ((SINT32) i);
      }
   }


   cmsLog_error("All %d bridges in use!", MAX_LAYER2_BRIDGES);
   return (-1);
}


#ifdef DMP_X_BROADCOM_COM_DLNA_1
CmsRet rutLan_updateDlna_dev2(void)
{
   InstanceIdStack dmsIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack ipIidStack = EMPTY_INSTANCE_ID_STACK;
   DmsCfgObject *dmsObj = NULL;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   char brIntfName[BUFLEN_32];
   UBOOL8 found=FALSE;
   CmsRet ret = CMSRET_SUCCESS;
   if ((ret = cmsObj_get(MDMOID_DMS_CFG, &dmsIidStack, 0, (void **)&dmsObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_DMS_CFG> returns error. ret=%d", ret);
      return ret;
   }   

   /* Generate the bridge interface name */
   sprintf(brIntfName, "br%d", dmsObj->brKey);

   /* First find the bridge */
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &ipIidStack,
                               (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipIntfObj->name, brIntfName))
      {
         found = TRUE;
      }
      cmsObj_free((void **) &ipIntfObj);
   }

   if (!found)
   {
       cmsLog_error("Could not find bridge %s", brIntfName);
       /* disable the DLNA due to the bridge interface isn't existed. */
       dmsObj->brKey = 0 ; 
       dmsObj->enable = 0 ; 
       ret = cmsObj_set(dmsObj, &dmsIidStack);
       cmsObj_free((void **) &dmsObj);
       if (ret != CMSRET_SUCCESS)
       {
           cmsLog_error("could not set DLNA cfg, ret=%d", ret);
           return ret;
       }
   }
   else
   {
       cmsObj_free((void **)&dmsObj);
   }

   cmsLog_debug("Exit with ret=%d", ret);

   return ret;
}
#endif

#endif  /* DMP_DEVICE2_BRIDGE_1 */

#endif    /* DMP_DEVICE2_BASELINE_1 */



