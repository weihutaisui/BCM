/*
* <:copyright-BRCM:2013:proprietary:standard
* 
*    Copyright (c) 2013 Broadcom 
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

#ifdef DMP_DEVICE2_BASELINE_1    /* this file touches TR181 objects */

CmsRet dalIp_addIntfObject_dev2(UBOOL8 supportIpv4 __attribute((unused)),
                                UBOOL8 supportIpv6 __attribute((unused)),
                                const char *intfGroupName,
                                UBOOL8 isBridgeService,
                                const char *bridgeName,
                                UBOOL8 firewallEnabled,
                                UBOOL8 igmpEnabled __attribute((unused)),
                                UBOOL8 igmpSourceEnabled __attribute((unused)),
                                UBOOL8 mldEnabled __attribute((unused)),
                                UBOOL8 mldSourceEnabled __attribute((unused)),
                                const char *lowerLayerFullPath,
                                char *myPathRef,
                                UINT32 pathLen,
                                MdmPathDescriptor *ipIntfPathDesc)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntf = NULL;
   CmsRet ret;
   

   cmsLog_debug("Enter: lowerLayerFullPath=%s", lowerLayerFullPath);

   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IP_INTERFACE, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add IP Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_IP_INTERFACE, &iidStack, 0, (void **) &ipIntf)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get ipIntf, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IP_INTERFACE, &iidStack);
      return ret;
   }
   
   
   ipIntf->enable = TRUE;

   /* TODO: what about IPv6 only, should we only conditionally enable IPv4?  */
   ipIntf->IPv4Enable = supportIpv4;

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   ipIntf->IPv6Enable = supportIpv6;
#endif

   /* lowLayerStr is ether ethLink object  (IPoE) or ppp interface (PPPoE)  */
   CMSMEM_REPLACE_STRING(ipIntf->lowerLayers, lowerLayerFullPath);


#ifdef later
   /* service name? */
   if (!IS_EMPTY_STRING(serviceName))
   {
      CMSMEM_REPLACE_STRING(ipIntf->X_BROADCOM_COM_servicename, webVar->serviceName);
   }
#endif


   /* For Interface grouping */
   CMSMEM_REPLACE_STRING(ipIntf->X_BROADCOM_COM_GroupName, intfGroupName);

   /* For WAN Bridge service */
   ipIntf->X_BROADCOM_COM_BridgeService = isBridgeService;
   CMSMEM_REPLACE_STRING(ipIntf->X_BROADCOM_COM_BridgeName, bridgeName);

   /* for Firewall */
   ipIntf->X_BROADCOM_COM_FirewallEnabled = firewallEnabled;

#ifdef DMP_X_BROADCOM_COM_IGMP_1
   ipIntf->X_BROADCOM_COM_IGMPEnabled = igmpEnabled;
   ipIntf->X_BROADCOM_COM_IGMP_SOURCEEnabled = igmpSourceEnabled;
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
   ipIntf->X_BROADCOM_COM_MLDEnabled = mldEnabled;
   ipIntf->X_BROADCOM_COM_MLD_SOURCEEnabled = mldSourceEnabled;
#endif

   ret = cmsObj_set(ipIntf, &iidStack);
   cmsObj_free((void **) &ipIntf);
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set ipIntf. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IP_INTERFACE, &iidStack);
      return ret;      
   } 

   
   /* pass back the fullpath and pathdesc of this IP.Interface to caller */
   {
      char *fullPath=NULL;

      memset(ipIntfPathDesc, 0, sizeof(MdmPathDescriptor));
      ipIntfPathDesc->oid = MDMOID_DEV2_IP_INTERFACE;
      ipIntfPathDesc->iidStack = iidStack;
      if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(ipIntfPathDesc, &fullPath)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsMdm_pathDescriptorToFullPathNoEndDot returns error. ret=%d", ret);
         cmsObj_deleteInstance(MDMOID_DEV2_IP_INTERFACE, &iidStack);
         return ret;
      }
      if (cmsUtl_strlen(fullPath)+1  > (SINT32) pathLen)
      {
         cmsLog_error("fullpath %s too long to fit in param", fullPath);
         ret = CMSRET_RESOURCE_EXCEEDED;
         cmsObj_deleteInstance(MDMOID_DEV2_IP_INTERFACE, &iidStack);
      }
      else
      {
         cmsUtl_strncpy(myPathRef, fullPath, pathLen);
      }
	  
#ifdef SUPPORT_RIP
      if ((ret = cmsObj_get(MDMOID_DEV2_IP_INTERFACE, &iidStack, 0, (void **) &ipIntf)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get ipIntf, ret = %d", ret);
         CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);
         return ret;
      }

      if (!cmsUtl_strstr(lowerLayerFullPath, "PPP") && 
          !isBridgeService && 
           ipIntf->X_BROADCOM_COM_Upstream)
      {
         dalRip_addRipInterfaceSetting_dev2(fullPath);
      }
	  
      cmsObj_free((void **) &ipIntf);
#endif

      CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);
   }

   cmsLog_debug("Exit: ret=%d myPathRef=%s", ret, myPathRef);
   return ret;
}


CmsRet dalIp_addIpIntfIpv4Address_dev2(const InstanceIdStack *ipIntfIidStack, 
                                       const char *ipAddr,
                                       const char *subnetMask)
{

   InstanceIdStack iidStack = *ipIntfIidStack;
   Dev2Ipv4AddressObject *ipv4AddrObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   cmsLog_debug("Entered: iidStack=%s ipAddr=%s subnet=%s",
                cmsMdm_dumpIidStack(ipIntfIidStack),
                ipAddr, subnetMask);

   /* For static ipaddress, need to create the ipv4Address object and fill it in. */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV4_ADDRESS, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add MDMOID_DEV2_IPV4_ADDRESS Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_IPV4_ADDRESS, &iidStack, 0, (void **) &ipv4AddrObj)) != CMSRET_SUCCESS)
   {
      cmsObj_deleteInstance(MDMOID_DEV2_IPV4_ADDRESS, &iidStack);
      cmsLog_error("Failed to get ipv4AddrObj, ret = %d", ret);
      return ret;
   }

   ipv4AddrObj->enable = TRUE;
   ipv4AddrObj->addressingType = cmsMem_strdup(MDMVS_STATIC);
   CMSMEM_REPLACE_STRING(ipv4AddrObj->IPAddress, ipAddr);
   CMSMEM_REPLACE_STRING(ipv4AddrObj->subnetMask, subnetMask);

   if ((ret = cmsObj_set(ipv4AddrObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsObj_deleteInstance(MDMOID_DEV2_IPV4_ADDRESS, &iidStack);
      cmsLog_error("Failed to set ipv4AddrObj. ret=%d", ret);
   } 
   cmsObj_free((void **) &ipv4AddrObj); 

   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}


CmsRet dalIp_deleteIntfObject_dev2(const char *ipLowerlayerFullPath, char *ipFullPath)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   char *objPath = NULL;
   Dev2IpInterfaceObject *ipObj=NULL;
   CmsRet ret=CMSRET_INVALID_ARGUMENTS;
   
   if (ipLowerlayerFullPath == NULL)
   {
      cmsLog_error("NULL string.");
      return ret;
   }
   cmsLog_debug("Enter ipLowerlayerFullPath %s.", ipLowerlayerFullPath);

   while(!found && 
         (ret = cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE,
                                    &iidStack, 
                                    OGF_NO_VALUE_UPDATE,
                                    (void **)&ipObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcasecmp(ipObj->lowerLayers, ipLowerlayerFullPath))
      {
         MdmPathDescriptor pathDesc = EMPTY_PATH_DESCRIPTOR;
            
         pathDesc.oid = MDMOID_DEV2_IP_INTERFACE;
         pathDesc.iidStack = iidStack;
         if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &objPath)) == CMSRET_SUCCESS)
         {
            /* save the lowerlayer pointer */
            if (objPath != NULL)
            {
               cmsLog_debug("Got ip intf full path %s and deleting ip intf obj...", objPath);
               cmsUtl_strncpy(ipFullPath, objPath, MDM_SINGLE_FULLPATH_BUFLEN);
               CMSMEM_FREE_BUF_AND_NULL_PTR(objPath);
#ifdef SUPPORT_RIP
               /* delete rip interface setting (if present) associated with this IP.Interface */
               ret = dalRip_deleteRipInterfaceSetting_dev2(ipFullPath);
               if (ret != CMSRET_SUCCESS && ret != CMSRET_NO_MORE_INSTANCES)
               {
                  cmsLog_error("Error while deleting rip interface setting for %s, ret=%d", ipFullPath, ret);
                  /* complain, but keep going */
               }
#endif
            }

            cmsObj_deleteInstance(MDMOID_DEV2_IP_INTERFACE, &iidStack);
         }
         else
         {
            cmsLog_error("Could not form fullpath to %d %s", pathDesc.oid, cmsMdm_dumpIidStack(&pathDesc.iidStack));
         }

         found = TRUE;
      }

      cmsObj_free((void **)&ipObj);
   }    
   
   cmsLog_debug("Exit, ret=%d", ret);

   return ret;

}

#endif /* DMP_DEVICE2_BASELINE_1 */

