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
#include "cms_qos.h"
#include "cms_phl.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_core.h"

CmsRet qdmIntf_intfnameToFullPathLocked_dev2(const char *intfName,
                                             UBOOL8 layer2,
                                             char **fullPathBuf)
{
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   MdmPathDescriptor pathDesc;
   void *mdmObj = NULL;   
   CmsRet ret = CMSRET_SUCCESS;

   if (IS_EMPTY_STRING(intfName) || fullPathBuf == NULL)
   {
      cmsLog_error("Invalid aruments.");
      return CMSRET_INVALID_ARGUMENTS;
   }

   memset(&pathDesc, 0, sizeof(MdmPathDescriptor));

   if (layer2)
   {
#if defined(DMP_X_BROADCOM_COM_GPONWAN_1) || defined (DMP_X_BROADCOM_COM_EPONWAN_1)
     INIT_INSTANCE_ID_STACK(&iidStack);
     while ((!found) &&
         ((ret = cmsObj_getNextFlags(MDMOID_OPTICAL_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE, &mdmObj)) == CMSRET_SUCCESS))
     {
         found = (0 == cmsUtl_strcmp(((OpticalInterfaceObject*)mdmObj)->name, intfName));
         if (found)
         {
             pathDesc.oid = MDMOID_OPTICAL_INTERFACE;
             pathDesc.iidStack = iidStack;
         }          
         cmsObj_free(&mdmObj);
     }
#endif

#ifdef DMP_DEVICE2_PTMLINK_1
     INIT_INSTANCE_ID_STACK(&iidStack);
      while ((!found) &&
             ((ret = cmsObj_getNextFlags(MDMOID_DEV2_PTM_LINK, &iidStack, OGF_NO_VALUE_UPDATE, &mdmObj)) == CMSRET_SUCCESS))
      {
         found = (0 == cmsUtl_strcmp(((Dev2PtmLinkObject*)mdmObj)->name, intfName));
          if (found)
         {
            pathDesc.oid = MDMOID_DEV2_PTM_LINK;
            pathDesc.iidStack = iidStack;
         }          
         cmsObj_free(&mdmObj);
      }
#endif /* DMP_DEVICE2_PTMLINK_1 */
      
#ifdef DMP_DEVICE2_ATMLINK_1
      INIT_INSTANCE_ID_STACK(&iidStack);
      while ((!found) &&
             ((ret = cmsObj_getNextFlags(MDMOID_DEV2_ATM_LINK, &iidStack, OGF_NO_VALUE_UPDATE, &mdmObj)) == CMSRET_SUCCESS))
      {
         found = (0 == cmsUtl_strcmp(((Dev2AtmLinkObject*)mdmObj)->name, intfName));
         if (found)
         {
            pathDesc.oid = MDMOID_DEV2_ATM_LINK;
            pathDesc.iidStack = iidStack;
         }          
         cmsObj_free(&mdmObj);
      }
#endif /* DMP_DEVICE2_ATMLINK_1 */

      INIT_INSTANCE_ID_STACK(&iidStack);
      while ((!found) &&
             ((ret = cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE, &mdmObj)) == CMSRET_SUCCESS))
      {
         found = (0 == cmsUtl_strcmp(((Dev2EthernetInterfaceObject*)mdmObj)->name, intfName));
         if (found)
         {
            pathDesc.oid = MDMOID_DEV2_ETHERNET_INTERFACE;
            pathDesc.iidStack = iidStack;
         }          
         cmsObj_free(&mdmObj);
      }

#ifdef DMP_DEVICE2_MOCA_1
      INIT_INSTANCE_ID_STACK(&iidStack);
      while ((!found) &&
             ((ret = cmsObj_getNextFlags(MDMOID_DEV2_MOCA_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE, &mdmObj)) == CMSRET_SUCCESS))
      {
         found = (0 == cmsUtl_strcmp(((Dev2MocaInterfaceObject *)mdmObj)->name, intfName));
         if (found)
         {
            pathDesc.oid = MDMOID_DEV2_MOCA_INTERFACE;
            pathDesc.iidStack = iidStack;
         }          
         cmsObj_free(&mdmObj);
      }
#endif

      /*
       * Why is PPP in layer 2?  It is because for Layer 3, we want to find
       * the ppp0.1 interface name in the IP.Interface object?
       */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while ((!found) &&
             ((ret = cmsObj_getNextFlags(MDMOID_DEV2_PPP_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE, &mdmObj)) == CMSRET_SUCCESS))
      {
         found = (0 == cmsUtl_strcmp(((Dev2PppInterfaceObject*)mdmObj)->name, intfName));
         if (found)
         {
            pathDesc.oid = MDMOID_DEV2_PPP_INTERFACE;
            pathDesc.iidStack = iidStack;
         }       
         cmsObj_free(&mdmObj);
      }


#ifdef DMP_DEVICE2_WIFIACCESSPOINT_1
      INIT_INSTANCE_ID_STACK(&iidStack);
      while ((!found) &&
             ((ret = cmsObj_getNextFlags(MDMOID_DEV2_WIFI_SSID, &iidStack, OGF_NO_VALUE_UPDATE, &mdmObj)) == CMSRET_SUCCESS))
      {
         found = (0 == cmsUtl_strcmp(((Dev2WifiSsidObject *)mdmObj)->name, intfName));
         if (found)
         {
            pathDesc.oid = MDMOID_DEV2_WIFI_SSID;
            pathDesc.iidStack = iidStack;
         }
         cmsObj_free(&mdmObj);
      }
#endif

#ifdef DMP_DEVICE2_USBINTERFACE_1
     INIT_INSTANCE_ID_STACK(&iidStack);
      while ((!found) &&
             ((ret = cmsObj_getNextFlags(MDMOID_DEV2_USB_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE, &mdmObj)) == CMSRET_SUCCESS))
      {
         found = (0 == cmsUtl_strcmp(((Dev2UsbInterfaceObject*)mdmObj)->name, intfName));
          if (found)
         {
            pathDesc.oid = MDMOID_DEV2_USB_INTERFACE;
            pathDesc.iidStack = iidStack;
         }          
         cmsObj_free(&mdmObj);
      }
#endif

      /* TODO: add more here */
      
   }
   else
   {
      /* layer3 interface */
      /* Do no use OGF_NO_VALUE_UPDATE for IP.Interface.  It's intfName is
       * dynamically calculated, so we might have to go into the STL handler
       * function to get the correct intfName.
       */
      INIT_INSTANCE_ID_STACK(&iidStack);      
      while ((!found) &&
             ((ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack, &mdmObj)) == CMSRET_SUCCESS))
      {
         found = (0 == cmsUtl_strcmp(((Dev2IpInterfaceObject*)mdmObj)->name, intfName));
         if (found)
         {
            pathDesc.oid = MDMOID_DEV2_IP_INTERFACE;
            pathDesc.iidStack = iidStack;
         }            
         cmsObj_free(&mdmObj);
      }

      /*
       * What interesting layer 3 interface name is in Ethernet.Link?
       * Wouldn't the same interface name be found in the IP.Interface object?
       */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while ((!found) &&
             ((ret = cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_LINK, &iidStack, OGF_NO_VALUE_UPDATE, &mdmObj)) == CMSRET_SUCCESS))
      {
         found = (0 == cmsUtl_strcmp(((Dev2EthernetLinkObject*)mdmObj)->name, intfName));
         if (found)
         {
            pathDesc.oid = MDMOID_DEV2_ETHERNET_LINK;
            pathDesc.iidStack = iidStack;
         }       
         cmsObj_free(&mdmObj);
      }

      INIT_INSTANCE_ID_STACK(&iidStack);
      while ((!found) &&
             ((ret = cmsObj_getNextFlags(MDMOID_DEV2_VLAN_TERMINATION, &iidStack, OGF_NO_VALUE_UPDATE, &mdmObj)) == CMSRET_SUCCESS))
      {
         found = (0 == cmsUtl_strcmp(((Dev2VlanTerminationObject *)mdmObj)->name, intfName));
         if (found)
         {
            pathDesc.oid = MDMOID_DEV2_VLAN_TERMINATION;
            pathDesc.iidStack = iidStack;
         }       
         cmsObj_free(&mdmObj);
      }

      
      /* TODO: add more layer 3 intfName -> mdmPathRef search here */
   }


   if (!found)
   {
      cmsLog_debug("could not find interface %s (layer2=%d)", intfName, layer2);
      return CMSRET_NO_MORE_INSTANCES;
   }
   
   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, fullPathBuf)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor returns error. ret=%d", ret);
      return ret;
   }
   
   cmsLog_debug("fullPathBuf %s", *fullPathBuf);

   return ret;
}


CmsRet qdmIntf_fullPathToIntfnameLocked_dev2(const char *mdmPath,
                                             char *intfname)
{
   if (IS_EMPTY_STRING(mdmPath) || intfname == NULL)
   {
      cmsLog_error("invalid argument. mdmPath=0x%x intfname=0x%x", (uintptr_t)mdmPath, (uintptr_t)intfname);
      return CMSRET_INVALID_ARGUMENTS;
   }

   return (qdmIntf_getIntfnameFromFullPathLocked_dev2(mdmPath, intfname, CMS_IFNAME_LENGTH));
}


CmsRet qdmIntf_getIntfnameFromFullPathLocked_dev2(const char *fullPath,
                                                  char *intfname, UINT32 intfnameLen)
{
   MdmPathDescriptor pathDesc;
   CmsRet ret = CMSRET_SUCCESS;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", fullPath, ret);
      return ret;
   }

   return (qdmIntf_getIntfnameFromPathDescLocked_dev2(&pathDesc, intfname, intfnameLen));
}


CmsRet qdmIntf_getIntfnameFromPathDescLocked_dev2(const MdmPathDescriptor *pathDescIn,
                                                  char *intfname, UINT32 intfnameLen)
{
   MdmPathDescriptor pathDesc;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   memset(intfname, 0, intfnameLen);

   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = pathDescIn->oid;
   pathDesc.iidStack = pathDescIn->iidStack;
   sprintf(pathDesc.paramName, "Name");

   ret = cmsPhl_getParamValue(&pathDesc, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getParamValue error: %d", ret);
   }
   else
   {
      if (cmsUtl_strlen(pParamValue->pValue) > 0)
      {
         snprintf(intfname, intfnameLen, "%s", pParamValue->pValue);
      }
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }

   return ret;
}


CmsRet qdmIntf_getPathDescFromIntfnameLocked_dev2(const char *intfname,
									 UBOOL8 layer2,
                                     MdmPathDescriptor *pathDesc)
{
   char *fullPath=NULL;
   CmsRet ret = CMSRET_SUCCESS;	

   ret = qdmIntf_intfnameToFullPathLocked_dev2(intfname,layer2,&fullPath);

   if (ret == CMSRET_SUCCESS)
   {
      ret = cmsMdm_fullPathToPathDescriptor(fullPath,pathDesc);
      CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);
   }

   return ret;

}


CmsRet qdmIntf_getStatusFromFullPathLocked_dev2(const char *fullPath,
                                    char *statusBuf, UINT32 statusBufLen)
{
   MdmPathDescriptor pathDesc;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", fullPath, ret);
      return ret;
   }

   return (qdmIntf_getStatusFromPathDescLocked_dev2(&pathDesc, statusBuf, statusBufLen));
}


CmsRet qdmIntf_getStatusFromPathDescLocked_dev2(const MdmPathDescriptor *pathDescIn,
                                    char *statusBuf, UINT32 statusBufLen)
{
   MdmPathDescriptor pathDesc;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet ret;

   memset(statusBuf, 0, statusBufLen);

#ifdef DMP_BASELINE_1
   /* this func may get called in Hybrid mode, and as a special case,
    * check for TR98 LAN IP INTF object only. */
   if (pathDescIn->oid == MDMOID_LAN_IP_INTF)
   {
      /* brX is always up */
      snprintf(statusBuf, statusBufLen, "%s", MDMVS_UP);

      cmsLog_debug("TR98 oid %d %s status %s",
                   pathDescIn->oid, cmsMdm_dumpIidStack(&pathDescIn->iidStack),
                   statusBuf);
      return CMSRET_SUCCESS;
   }
#endif

   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = pathDescIn->oid;
   pathDesc.iidStack = pathDescIn->iidStack;
   sprintf(pathDesc.paramName, "Status");

   ret = cmsPhl_getParamValue(&pathDesc, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      /*
       * ssk intfstack code may try to get Status on a deleted object,
       * so just return an error code and let the caller deal with it.
       */
      cmsLog_debug("cmsPhl_getParamValue error: %d", ret);
   }
   else
   {
      if (cmsUtl_strlen(pParamValue->pValue) > 0)
      {
         snprintf(statusBuf, statusBufLen, "%s", pParamValue->pValue);
         cmsLog_debug("TR181 oid %d %s status %s",
                      pathDesc.oid, cmsMdm_dumpIidStack(&pathDesc.iidStack),
                      statusBuf);
      }
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }

   return ret;
}


UBOOL8 qdmIntf_isStatusUpOnFullPathLocked_dev2(const char *fullPath)
{
   char statusBuf[BUFLEN_32]={0};

   if (qdmIntf_getStatusFromFullPathLocked_dev2(fullPath,
                          statusBuf, sizeof(statusBuf)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(statusBuf, MDMVS_UP))
      {
         return TRUE;
      }
   }

   return FALSE;
}


CmsRet qdmIntf_getLowerLayersFromFullPathLocked_dev2(const char *fullPath,
                                                     char *lowerLayersBuf,
                                                     UINT32 llBufLen)
{
   MdmPathDescriptor pathDesc;
   CmsRet ret = CMSRET_SUCCESS;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", fullPath, ret);
      return ret;
   }

   return (qdmIntf_getLowerLayersFromPathDescLocked_dev2(&pathDesc,
                                                 lowerLayersBuf, llBufLen));
}


CmsRet qdmIntf_getLowerLayersFromPathDescLocked_dev2(const MdmPathDescriptor *pathDescIn,
                                                     char *lowerLayersBuf,
                                                     UINT32 llBufLen)
{
   MdmPathDescriptor pathDesc;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   memset(lowerLayersBuf, 0, llBufLen);

   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = pathDescIn->oid;
   pathDesc.iidStack = pathDescIn->iidStack;
   sprintf(pathDesc.paramName, "LowerLayers");

   ret = cmsPhl_getParamValue(&pathDesc, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      /*
       * ssk intfstack code may try to get LowerLayers on a deleted object,
       * so just return an error code and let the caller deal with it.
       */
      cmsLog_debug("cmsPhl_getParamValue error: %d", ret);
   }
   else
   {
      if (cmsUtl_strlen(pParamValue->pValue) > 0)
      {
         snprintf(lowerLayersBuf, llBufLen, "%s", pParamValue->pValue);
      }
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }

   return ret;
}


CmsRet qdmIntf_getFirstLowerLayerFromFullPathLocked_dev2(const char *fullPath,
                                                 char *lowerLayerFullPathBuf,
                                                 UINT32 bufLen)
{
   MdmPathDescriptor pathDesc;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet ret;

   memset((void *) lowerLayerFullPathBuf, 0, bufLen);

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", fullPath, ret);
      return ret;
   }

   sprintf(pathDesc.paramName, "LowerLayers");

   ret = cmsPhl_getParamValue(&pathDesc, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_notice("cmsPhl_getParamValue error: %d (fullpath=%s)", ret, fullPath);
   }
   else
   {
      if (cmsUtl_strlen(pParamValue->pValue) > 0)
      {
         UINT32 i=0;
         /* we got a lowerLayers param.  This may be a comma separated list
          * of fullpaths, so just get the first one.
          */
         while ((pParamValue->pValue[i] != 0) &&
                (pParamValue->pValue[i] != ',') &&
                (pParamValue->pValue[i] != ' ') && (i < bufLen-1))
         {
            lowerLayerFullPathBuf[i] = pParamValue->pValue[i];
            i++;
         }
         cmsLog_debug("lowerLayerFullPath=%s", lowerLayerFullPathBuf);
      }
      else
      {
         /* should we return an error code if lowerlayers is empty? */
         cmsLog_debug("LowerLayers of %s is empty", fullPath);
      }
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }

   return ret;
}


UBOOL8 qdmIntf_isFullPathLayer2Locked_dev2(const char *fullPath)
{
   MdmPathDescriptor pathDesc = EMPTY_PATH_DESCRIPTOR;
   CmsRet ret;

   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not convert fullpath %s to pathDesc, ret=%d", fullPath, ret);
      return FALSE;
   }

   /*
    * Now compare the OID to the list of Layer 2 interfaces defined in TR181.
    * Special case 1: these do not have the upstream param, but they do have
    *                 the correct layer 2 intfNames.  We know they are always
    *                 upstream anyways.
    * Special case 2: Broadcom Wifi allows you to have 1 SSID upstream, and
    *                 the other 3 SSID downstream, all on the same radio, so
    *                 we cannot use the upstream param in the radio object.
    *                 Also, the correctl layer 2 intfName is stored in the
    *                 SSID object.
    */
   if (pathDesc.oid == MDMOID_DEV2_ETHERNET_INTERFACE ||
#ifdef DMP_DEVICE2_ATMLINK_1
       pathDesc.oid == MDMOID_DEV2_ATM_LINK ||  /* special case 1 */
#endif
#ifdef DMP_DEVICE2_PTMLINK_1
       pathDesc.oid == MDMOID_DEV2_PTM_LINK ||  /* special case 1 */
#endif
#ifdef DMP_CELLULARINTERFACEBASE_1
       pathDesc.oid == MDMOID_DEV2_CELLULAR_LINK ||
#endif       
#ifdef DMP_DEVICE2_HOMEPLUG_1       
       pathDesc.oid == MDMOID_HOME_PLUG_INTERFACE ||
#endif
#ifdef DMP_DEVICE2_MOCA_1
       pathDesc.oid == MDMOID_DEV2_MOCA_INTERFACE ||
#endif
#ifdef DMP_DEVICE2_OPTICAL_1
       pathDesc.oid == MDMOID_OPTICAL_INTERFACE || /* XXX may need to get one level higher */
#endif
       pathDesc.oid == MDMOID_DEV2_WIFI_SSID ||  /* special case 2 */
       pathDesc.oid == MDMOID_DEV2_USB_INTERFACE)
   {
      return TRUE;
   }
#ifdef DMP_BASELINE_1
   /*
    * In Hybrid mode, we might get a fullpath that points to a TR98 WAN / LAN
    * object, also consider these "layer2".
    */
   else if ((pathDesc.oid == MDMOID_WAN_IP_CONN) ||
            (pathDesc.oid == MDMOID_WAN_PPP_CONN) ||
            (pathDesc.oid == MDMOID_LAN_IP_INTF))

   {
      cmsLog_debug("hit TR98 obj %d, return TRUE", pathDesc.oid);
      return TRUE;
   }
#endif
   else
   {
      return FALSE;
   }
}


UBOOL8 qdmIntf_isLayer2IntfNameUpstreamLocked_dev2(const char *l2IntfName)
{
   char *fullPath=NULL;
   UBOOL8 isLayer2 = TRUE;
   UBOOL8 isUpstream;
   CmsRet ret;

   if ((ret = qdmIntf_intfnameToFullPathLocked(l2IntfName, isLayer2, &fullPath)) != CMSRET_SUCCESS)
   {
      cmsLog_error("intfnameToFullPath on %s returned %d", l2IntfName, ret);
      return FALSE;
   }

   isUpstream = qdmIntf_isLayer2FullPathUpstreamLocked_dev2(fullPath);

   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);

   return isUpstream;
}


UBOOL8 qdmIntf_isLayer2FullPathUpstreamLocked_dev2(const char *l2FullPath)
{
   MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
   PhlGetParamValue_t *pParamValue = NULL;
   UBOOL8 isUpstream=FALSE;
   CmsRet ret;

   cmsLog_debug("Entered: l2FullPath=%s", l2FullPath);

   ret = cmsMdm_fullPathToPathDescriptor(l2FullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", l2FullPath, ret);
      return ret;
   }

#if defined(DMP_X_BROADCOM_COM_GPONWAN_1) || defined(DMP_X_BROADCOM_COM_EPONWAN_1)
   if (pathDesc.oid == MDMOID_OPTICAL_INTERFACE)
   {
      /* these objects do not have an Upstream param, but we know they are upstream */
      isUpstream = TRUE;
   }
#endif
#ifdef DMP_DEVICE2_ATMLINK_1 
   if (pathDesc.oid == MDMOID_DEV2_ATM_LINK)
   {
      /* these objects do not have an Upstream param, but we know they are upstream */
      isUpstream = TRUE;
   }
#endif
#ifdef DMP_DEVICE2_PTMLINK_1 
   else if (pathDesc.oid == MDMOID_DEV2_PTM_LINK)
   {
      /* these objects do not have an Upstream param, but we know they are upstream */
      isUpstream = TRUE;
   }
#endif
#ifdef DMP_CELLULARINTERFACEBASE_1
   else if (pathDesc.oid == MDMOID_DEV2_CELLULAR_LINK)
   {
      isUpstream = TRUE;
   }
#endif          
   else if (pathDesc.oid == MDMOID_DEV2_WIFI_SSID)
   {
      Dev2WifiSsidObject *ssidObj=NULL;

      ret = cmsObj_get(MDMOID_DEV2_WIFI_SSID, &pathDesc.iidStack,
                       OGF_NO_VALUE_UPDATE, (void **)&ssidObj);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get SSID obj at %s, ret=%d", l2FullPath, ret);
      }
      else
      {
         isUpstream = ssidObj->X_BROADCOM_COM_Upstream;
         cmsObj_free((void **) &ssidObj);
      }
   }
#ifdef DMP_BASELINE_1
   /*
    * In Hybrid mode, we might get a fullpath that points to a TR98 WAN / LAN
    * object, which do not have an Upstream param.  But we know whether
    * these objects are upstream or not.
    */
   else if ((pathDesc.oid == MDMOID_WAN_IP_CONN) ||
            (pathDesc.oid == MDMOID_WAN_PPP_CONN))
   {
      cmsLog_debug("hit TR98 WAN obj %d, return TRUE", pathDesc.oid);
      isUpstream = TRUE;
   }
   else if (pathDesc.oid == MDMOID_LAN_IP_INTF)
   {
      cmsLog_debug("hit TR98 LAN obj %d, return FALSE", pathDesc.oid);
      isUpstream = FALSE;
   }
#endif
   else
   {
      sprintf(pathDesc.paramName, "Upstream");
      ret = cmsPhl_getParamValue(&pathDesc, &pParamValue);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsPhl_getParamValue for Upstream param failed on %s, ret=%d",
                      l2FullPath, ret);
      }
      else
      {
         if (!cmsUtl_strcmp(pParamValue->pValue, "1") ||
             !cmsUtl_strcasecmp(pParamValue->pValue, "true"))
         {
            isUpstream = TRUE;
         }
         cmsPhl_freeGetParamValueBuf(pParamValue, 1);
      }
   }

   cmsLog_debug("returning isUpstream=%d", isUpstream);
   return isUpstream;
}


void getLowestLayerPathDescFromUpperLayerPathDescLocked(const MdmPathDescriptor *pathDescUp,
                                           MdmPathDescriptor *lowerLayerPathDesc,
                                           UBOOL8 *found, MdmObjectId oid)
{
   MdmPathDescriptor pathDescLower;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   char *pLowerLayers;
   char *pNext=NULL;
   char *pToken;
   char *pLastToken;

   INIT_PATH_DESCRIPTOR(lowerLayerPathDesc);

   INIT_PATH_DESCRIPTOR(&pathDescLower);
   pathDescLower.oid = pathDescUp->oid;
   pathDescLower.iidStack = pathDescUp->iidStack;
   sprintf(pathDescLower.paramName, "LowerLayers");

   ret = cmsPhl_getParamValue(&pathDescLower, &pParamValue);
   if (ret != CMSRET_SUCCESS || IS_EMPTY_STRING(pParamValue->pValue))
   {
      cmsLog_debug("reach lowest layer");

      if (pathDescUp->oid == oid)
      {
         *found = TRUE;
         *lowerLayerPathDesc = *pathDescUp;
      }

      if (pParamValue)
      {
         cmsPhl_freeGetParamValueBuf(pParamValue, 1);
      }
   }
   else
   {
      cmsLog_debug("found %s", pParamValue->pValue);

      /* The lowerLayers is a comma-separated string, so the token needs to be parsed. 
       * If the interface is up, pick that, and continue.  If the last one is reached and it's down, use that.
       */
      pLowerLayers = cmsMem_strdup(pParamValue->pValue);
      pToken = strtok_r(pLowerLayers,",",&pNext);
      pLastToken = pLowerLayers;
      while (pToken)
      {
         pLastToken = pToken;
         if (qdmIntf_isStatusUpOnFullPathLocked_dev2(pToken))
         {
            break;
         }
         else
         {
            pToken = strtok_r(NULL,",",&pNext);
         }
      }
      INIT_PATH_DESCRIPTOR(&pathDescLower);

      ret = cmsMdm_fullPathToPathDescriptor(pLastToken, &pathDescLower);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", pLastToken, ret);
         return;
      }
      cmsMem_free(pLowerLayers);

      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
      getLowestLayerPathDescFromUpperLayerPathDescLocked(&pathDescLower, lowerLayerPathDesc, found, oid);
   }

   return;
}


CmsRet qdmIntf_getIfNameFromBottomLayer(UBOOL8 isPPP, const char *fullPath, char *ifName)
{
   MdmPathDescriptor pathDesc, pathDescIn;
   UBOOL8 found=FALSE;
   CmsRet ret=CMSRET_SUCCESS;

   *ifName = '\0';

   cmsLog_debug("fullPath<%s> ppp<%d>", fullPath, isPPP);

   INIT_PATH_DESCRIPTOR(&pathDescIn);
   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDescIn);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", fullPath, ret);
      return ret;
   }

   INIT_PATH_DESCRIPTOR(&pathDesc);
   getLowestLayerPathDescFromUpperLayerPathDescLocked(&pathDescIn, &pathDesc, &found, 
                              isPPP?MDMOID_WAN_PPP_CONN:MDMOID_WAN_IP_CONN);

   if (found)
   {
      void *wanConnObj=NULL;

      ret = cmsObj_get(pathDesc.oid, &(pathDesc.iidStack), OGF_NO_VALUE_UPDATE, &wanConnObj);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("could not get wanConn object , ret=%d", ret);
         return ret;
      }

      if (isPPP)
      {
         WanPppConnObject *obj = (WanPppConnObject *)wanConnObj;
         cmsUtl_strcpy(ifName, obj->X_BROADCOM_COM_IfName);
      }
      else
      {
         WanIpConnObject *obj = (WanIpConnObject *)wanConnObj;
         cmsUtl_strcpy(ifName, obj->X_BROADCOM_COM_IfName);
      }

      cmsObj_free(&wanConnObj);
   }

   cmsLog_debug("ifName<%s>", ifName);
   return ret;
}


CmsRet qdmIntf_getIntfGroupNameByBrKey_dev2(char *bridgeName, UINT32 bridgeKey)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   UBOOL8 found=FALSE;
   CmsRet ret = CMSRET_SUCCESS;

   /* find the InterfaceGroupName by matching the bridge key */
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                               (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strstr(ipIntfObj->name,BRIDGE_IFC_STR) &&
          bridgeKey == (UINT32) atoi(&ipIntfObj->name[2]))
      {
         found = TRUE;
         strcpy(bridgeName, ipIntfObj->X_BROADCOM_COM_GroupName);
      }
      cmsObj_free((void **) &ipIntfObj);
   }
   return ret;
}


CmsRet qdmIntf_getIntfKeyByGroupName_dev2(char *bridgeName, UINT32 *bridgeKey)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   UBOOL8 found=FALSE;
   SINT32 foundBrKey=0;
   CmsRet ret = CMSRET_SUCCESS;

   /* find the matched InterfaceGroup and get the bridge key */
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                               (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if ((cmsUtl_strstr(ipIntfObj->name,BRIDGE_IFC_STR) &&
           !cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_GroupName, bridgeName)))
      {
         found = TRUE;
         foundBrKey = atoi(&ipIntfObj->name[2]);
      }
      cmsObj_free((void **) &ipIntfObj);
   }
   /* Update the bridge key if found */
   if(found)
   {
      *bridgeKey = foundBrKey;
   }
   return ret;
}

CmsRet qdmIntf_getBottomLayerPathDescLocked_dev2(const char *fullPath,
                                 MdmPathDescriptor *bottomLayerPathDesc,
                                 MdmObjectId oid)
{
   UBOOL8 found=FALSE;
   MdmPathDescriptor pathDescIn;
   CmsRet ret=CMSRET_SUCCESS;

   if (bottomLayerPathDesc == NULL)
      return CMSRET_INVALID_PARAM_VALUE;
   
   INIT_PATH_DESCRIPTOR(&pathDescIn);
   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDescIn);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", fullPath, ret);
      return ret;
   }

   INIT_PATH_DESCRIPTOR(bottomLayerPathDesc);
   getLowestLayerPathDescFromUpperLayerPathDescLocked(&pathDescIn, bottomLayerPathDesc, &found, oid); 
   if (!found)
      ret = CMSRET_INTERNAL_ERROR;

   return CMSRET_SUCCESS;
}



UBOOL8 qdmIntf_isInterfaceWANOnly_dev2(const char *ifName)
{
   UBOOL8 found = FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2EthernetInterfaceObject *ethIntfObj=NULL;
   
   while (!found &&
          (cmsObj_getNext(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack, (void **) &ethIntfObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(ethIntfObj->name, ifName) && 
          !cmsUtl_strcmp(ethIntfObj->X_BROADCOM_COM_WanLan_Attribute, MDMVS_WANONLY))
      {      
         found = TRUE;
      }      
      cmsObj_free((void **) &ethIntfObj);
   }
   
   return found;

}

#endif /* DMP_DEVICE2_BASELINE_1 */

