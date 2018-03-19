/***********************************************************************
 *
 *  Copyright (c) 2006-2012 Broadcom Corporation
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


#include "cms.h"
#include "cms_core.h"
#include "cms_util.h"
#include "mdm.h"
#include "mdm_private.h"
#include "cms_qdm.h"

#ifdef DMP_DEVICE2_HOMEPLUG_1


#if defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)
CmsRet mdmInit_addDhcpv6ClientObject_dev2(const char * ipIntfPathRef)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2Dhcpv6ClientObject *dhcp6ClientObj=NULL;
   CmsRet ret;

   cmsLog_debug("mdmInit_addDhcpv6ClientObject_dev2() enter Path=%s", ipIntfPathRef);
 
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_DHCPV6_CLIENT, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add MDMOID_DEV2_DHCPV6_CLIENT Instance, ret = %d", ret);
      return ret;
   } 
   
   if ((ret = cmsObj_get(MDMOID_DEV2_DHCPV6_CLIENT, &iidStack, 0, (void **) &dhcp6ClientObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get dhcp6ClientObj, ret = %d", ret);
      return ret;
   }
      
   dhcp6ClientObj->enable = TRUE;
   dhcp6ClientObj->requestAddresses = TRUE;
   dhcp6ClientObj->requestPrefixes = FALSE;
   dhcp6ClientObj->X_BROADCOM_COM_UnnumberedModel = FALSE;

   cmsLog_debug("mdmInit_addDhcpv6ClientObject_dev2() enable %d requestAddresses %d requestPrefixes %d", dhcp6ClientObj->enable,dhcp6ClientObj->requestAddresses,dhcp6ClientObj->requestPrefixes);
   
   CMSMEM_REPLACE_STRING(dhcp6ClientObj->interface, ipIntfPathRef);
   
   ret = cmsObj_set(dhcp6ClientObj, &iidStack);
   cmsObj_free((void **) &dhcp6ClientObj); 
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set dhcp6ClientObj. ret=%d", ret);
      mdm_freeObject((void **)&dhcp6ClientObj);
      return ret;
   }    
#if 0
   /* need to manually update the count when adding objects during mdm_init */
   {  
      InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
      Dev2Dhcpv6Object *dhcpv6Obj=NULL;
   
      if ((ret = cmsObj_get(MDMOID_DEV2_DHCPV6, &iidStack, 0, (void **) &dhcpv6Obj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get dhcpv6Obj, ret = %d", ret);
         return ret;
      }

      dhcpv6Obj->clientNumberOfEntries++;
      
      ret = mdm_setObject((void **) &dhcpv6Obj, &iidStack, FALSE);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to set dhcpv6Obj. ret=%d", ret);
         mdm_freeObject((void **)&dhcpv6Obj);
      }
   } 
#endif
   
   cmsLog_debug("mdmInit_addDhcpv6ClientObject_dev2() Exit, ret=%d", ret);
   
   return ret;
}
#endif

UBOOL8 isPlcInterfaceExist(void)
{
    char *ifNames=NULL;
    UBOOL8 found=FALSE;
    
   /*
    * Need to get lan port interface name from the kernel and see if plc is there or not
    */
   cmsNet_getIfNameList(&ifNames);
   if (ifNames == NULL)
   {
      cmsLog_error("no interfaces found during initialization!");
      return found;
   }

   if (cmsUtl_strstr(ifNames, "plc0") != NULL)
   {
      found = TRUE;
   }

   cmsMem_free(ifNames);

   return found;
   
   
}


CmsRet mdmInit_addHomePlugInterfaceObject(const char *ifName,
                                          UBOOL8 isUpstream,
                                          char **homePlugFullPath)
{
    CmsRet ret = CMSRET_SUCCESS;
   _HomePlugInterfaceObject *homePlugIntf=NULL;
   MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
   
   cmsLog_debug("Enter: ifName=%s isUpstream=%d", ifName, isUpstream);
   
   /* Only plc0 is supported */
   if (cmsUtl_strncmp(ifName, "plc0", 4))
   {
      cmsLog_error("Only plc0 is supported, got %s", ifName);
      return CMSRET_INTERNAL_ERROR;
   }

   /* see if we can find an existing one, if not, create it */
   pathDesc.oid = MDMOID_HOME_PLUG_INTERFACE;
   if (mdm_getNextObject(pathDesc.oid, &pathDesc.iidStack, (void **) &homePlugIntf) != CMSRET_SUCCESS)
   {
      cmsLog_notice("Adding homeplug interface %s", ifName);
      if ((ret = mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not add homePlugIntf ret=%d", ret);
         return ret;
      }
      cmsLog_debug("homePlugIntf added");

      /* get the object we just created */
      if ((ret = mdm_getObject(pathDesc.oid, &pathDesc.iidStack, (void **) &homePlugIntf)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not get homeplug intf object, ret=%d", ret);
         return ret;
      }
   }

   /*
    * Regardless of whether we created a new homeplug obj or found an
    * existing one, set the parameters.  Is this to compensate for an old
    * bug where the ifName was not written to config file.
    */
   homePlugIntf->enable = TRUE;
   homePlugIntf->upstream = isUpstream;
   CMSMEM_REPLACE_STRING_FLAGS(homePlugIntf->name, ifName, mdmLibCtx.allocFlags);

   if ((ret = mdm_setObject((void **) &homePlugIntf, &pathDesc.iidStack, FALSE)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set homeplug intf object, ret=%d", ret);
      mdm_freeObject((void **) &homePlugIntf);
      return ret;
   }
   cmsLog_debug("Done setting ifName=%s", ifName);


   /*
    * To compensate for an old bug where interfaceNumberOfEntries was not
    * written to config file, always set it to the correct count, which should
    * be 1
    */
   {
      DeviceHomePlugObject *homePlugDevice = NULL;
      InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;

      if ((ret = mdm_getObject(MDMOID_DEVICE_HOME_PLUG, &iidStack, (void **) &homePlugDevice)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get homePlugDevice, ret=%d", ret);
         return ret;
      }

      homePlugDevice->interfaceNumberOfEntries = 1;

      ret = mdm_setObject((void **) &homePlugDevice, &iidStack, FALSE);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("could not set homePlugDevice, ret=%d", ret);
         mdm_freeObject((void **) &homePlugDevice);
         return ret;
      }
   }


   /* return fullpath to caller if requested */
   if (homePlugFullPath)
   {
      ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, homePlugFullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not convert homeplug pathDesc to fullPath,ret=%d", ret);
         return ret;
      }
   }

   cmsLog_debug("exit ret  =%d", ret);

   return ret;
}


/* this function is used only in PURE181 mode.  Add entire Bridged WAN
 * service stack for the plc0 interface.
 */
CmsRet mdmInit_addUpstreamHomePlugWanService(const char *intfName)
{
   char *homePlugFullPath=NULL;

   CmsRet ret;

   cmsLog_debug("Entered: intfName=%s", intfName);

   /*
    * First check if there is already an IP.Interface obj for homeplug
    */
   {
      Dev2IpInterfaceObject *ipIntfObj=NULL;
      InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
      UBOOL8 found=FALSE;

      while (!found &&
             mdm_getNextObject(MDMOID_DEV2_IP_INTERFACE, &iidStack, (void **)&ipIntfObj) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(ipIntfObj->name, intfName))
         {
            found = TRUE;
         }
         mdm_freeObject((void **)&ipIntfObj);
      }

      if (found)
      {
         cmsLog_debug("IP.Interface for %s already exists", intfName);
         return CMSRET_SUCCESS;
      }
   }


   ret = mdmInit_addHomePlugInterfaceObject(intfName, TRUE, &homePlugFullPath);
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }


   /*
    * Create a WAN Bridge Service IP.Interface object for homeplug
    */
   {
      MdmPathDescriptor ipIntfPathDesc=EMPTY_PATH_DESCRIPTOR;
      char *ipIntfFullPath=NULL;
      UBOOL8 supportIpv4=TRUE;
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
      UBOOL8 supportIpv6=TRUE;  
#else
      UBOOL8 supportIpv6=FALSE;  
#endif

      ret = mdmInit_addIpInterfaceObject_dev2(intfName, NULL,
                                          supportIpv4, supportIpv6,
                                          TRUE, TRUE, TRUE, "br0",
                                          homePlugFullPath, &ipIntfPathDesc);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to create IP.Interface, ret=%d", ret);
         CMSMEM_FREE_BUF_AND_NULL_PTR(homePlugFullPath);
         return ret;
      }

      ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&ipIntfPathDesc, &ipIntfFullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not convert IP.Intf pathDesc to fullPath,ret=%d", ret);
         CMSMEM_FREE_BUF_AND_NULL_PTR(homePlugFullPath);
         return ret;
      }

#ifdef DMP_DEVICE2_DHCPV4_1
      /*
       * Create a dhcp client to point to IP.Interface.
       * (Need ifdef DEVICE2_DHCPV4_1 around this call to make compiler happy,
       * but in practice, this function is only called in PURE181 mode.)
       */
      ret = mdmInit_addDhcpv4ClientObject_dev2(ipIntfFullPath);
#endif
      
      if (ret != CMSRET_SUCCESS)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
         CMSMEM_FREE_BUF_AND_NULL_PTR(homePlugFullPath);
         cmsLog_error("Could not create dhcp client object for br0, ret=%d", ret);
         return ret;
      }

#if defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)
      /*
       * Create a dhcp client to point to br0 IP.Interface.
       */
      ret = mdmInit_addDhcpv6ClientObject_dev2(ipIntfFullPath);
      if (ret != CMSRET_SUCCESS)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
         CMSMEM_FREE_BUF_AND_NULL_PTR(homePlugFullPath);
         cmsLog_error("Could not create dhcp client object for br0, ret=%d", ret);
         return ret;
      }
#endif
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
   }

   /*
    * Because homeplug is a WAN Bridge service, also connect the homeplug
    * interface obj to br0.
    */
   ret = mdmInit_addFullPathToBridge_dev2("br0", intfName, homePlugFullPath);
   CMSMEM_FREE_BUF_AND_NULL_PTR(homePlugFullPath);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not create bridge port for homeplug, ret=%d", ret);
      return ret;
   }

   cmsLog_debug("Exit: ret=%d", ret);

   return ret;
}



/* this function is used only in PURE181 mode.  Add a plc0 interface
 * to the LAN side br0
 */
CmsRet mdmInit_addDownstreamHomePlugPort(const char *intfName)
{
   char *homePlugFullPath=NULL;
   CmsRet ret;

   cmsLog_debug("Entered: intfName=%s", intfName);

   /*
    * First check if there is already a Bridge.Port obj for homeplug
    */
   {
      Dev2BridgePortObject *brPortObj=NULL;
      InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
      UBOOL8 found=FALSE;

      while (!found &&
             mdm_getNextObject(MDMOID_DEV2_BRIDGE_PORT, &iidStack, (void **)&brPortObj) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(brPortObj->name, intfName))
         {
            found = TRUE;
         }
         mdm_freeObject((void **)&brPortObj);
      }

      if (found)
      {
         cmsLog_debug("Bridge.Port for %s already exists", intfName);
         return CMSRET_SUCCESS;
      }
   }


   ret = mdmInit_addHomePlugInterfaceObject(intfName, FALSE, &homePlugFullPath);
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }


   /*
    * add this homeplug interface to the LAN side br0
    */
   ret = mdmInit_addFullPathToBridge_dev2("br0", intfName, homePlugFullPath);
   CMSMEM_FREE_BUF_AND_NULL_PTR(homePlugFullPath);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not create bridge port for homeplug, ret=%d", ret);
      return ret;
   }
     
#ifdef DMP_DEVICE2_DHCPV4_1
   /*
    * Create a dhcp client to point to br0 IP.Interface since TR181 will behave same
    * as Hybrid homeplug implementation.
    *
    * (Need ifdef DEVICE2_DHCPV4_1 around this call to make compiler happy,
    * but in practice, this function is only called in PURE181 mode.)
    */
   {       
      char *BrIpIntfFullPath=NULL;  
      qdmIntf_intfnameToFullPathLocked("br0", FALSE, &BrIpIntfFullPath);
   
      ret = mdmInit_addDhcpv4ClientObject_dev2(BrIpIntfFullPath);

      CMSMEM_FREE_BUF_AND_NULL_PTR(BrIpIntfFullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not create dhcp client object for br0, ret=%d", ret);
         return ret;
      }
   }
#endif      

#if defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)
   /*
    * Create a dhcp client to point to br0 IP.Interface.
    */
   {
      char *BrIpIntfFullPath=NULL;  
      qdmIntf_intfnameToFullPathLocked("br0", FALSE, &BrIpIntfFullPath);

      ret = mdmInit_addDhcpv6ClientObject_dev2(BrIpIntfFullPath);

      CMSMEM_FREE_BUF_AND_NULL_PTR(BrIpIntfFullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not create dhcp client object for br0, ret=%d", ret);
         return ret;
      }
   }
#endif

   cmsLog_debug("Exit: ret=%d", ret);

   return ret;
}


#endif /* DMP_DEVICE2_HOMEPLUG_1 */


