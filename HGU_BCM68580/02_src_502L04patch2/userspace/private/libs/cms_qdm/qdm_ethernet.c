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
 
#ifdef DMP_BASELINE_1  /* this file touches TR98 objects */


#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_phl.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_core.h"

/** there are 2 two copies of this function: this copy is used in legacy TR98 and hybrid mode 
 * hopefully all functions in this file will be in this category, but there is no requirement for them to be
 * Will return the path to the TR98 LANDevice with the TR98 IP Intf name 
 */


CmsRet qdmEthLink_getEthLinkLowerLayerFullPathByName_igd(const char *ethLinkName,
                                                         char *lowerLayerBuf, 
                                                         UINT32 bufLen)
{
   UBOOL8 found=FALSE;
   LanIpIntfObject *ipIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   char *fullPathString=NULL;   
   MdmPathDescriptor pathDesc;
   CmsRet ret;

   if (ethLinkName == NULL || lowerLayerBuf == NULL)
   {
      cmsLog_error("Invalid aruments.");
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* It's call in mdm_initxxx and since cmsObj_xxx need lock here.  
   * Alternative is to use mdm functions and not putting in qdm_xxx ? 
   */
   if ((ret = cmsLck_acquireLockWithTimeout(6)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return ret;
   }

   while ((!found) &&
          ((ret = cmsObj_getNext(MDMOID_LAN_IP_INTF, &iidStack, (void **) &ipIntfObj)) == CMSRET_SUCCESS))
   {
      found = (0 == cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IfName, ethLinkName));
      cmsObj_free((void **) &ipIntfObj);
   }
   cmsLck_releaseLock();

   if (!found)
   {
      cmsLog_error("could not find bridge %s", ethLinkName);
      return CMSRET_NO_MORE_INSTANCES;
   }

   memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
   pathDesc.oid = MDMOID_LAN_IP_INTF;
   pathDesc.iidStack = iidStack;
   
   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullPathString)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor returns error. ret=%d", ret);
      return ret;
   }

   if (cmsUtl_strlen(fullPathString)+1 > (SINT32) bufLen)
   {
      cmsLog_error("fullpath %s too long to fit in param", fullPathString);
      ret =  CMSRET_INTERNAL_ERROR;
   }
   else
   {
      strcpy(lowerLayerBuf, fullPathString);
      ret = CMSRET_SUCCESS;
   }
         
   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPathString);

      
   cmsLog_debug("lowerLayerBuf %s", lowerLayerBuf);

   return ret;
   
}  /* End of qdmEthLink_getEthLinkLowerLayerFullPathByName_igd() */
 

/* Defined in rut_lan.c */
extern CmsRet rutLan_getEthInterface(const char       *ifName,
                                     InstanceIdStack  *iidStack,
                                     LanEthIntfObject **lanEthObj);
/* Defined in rut_wanlayer2.c */
extern CmsRet rutWl2_getWanEthObject(InstanceIdStack  *iidStack,
                                     WanEthIntfObject **wanEthIntfObj);

CmsRet qdmEth_getIntfInfoByNameLocked_igd(const char  *ifname,
                                          char        *status,
                                          UBOOL8      *upstream,
                                          SINT32      *shapingRate,
                                          UINT32      *shapingBurstSize)
{
   UBOOL8 found = FALSE;
   LanEthIntfObject *lanEthObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   CmsRet ret;
   
   if (cmsUtl_strlen(ifname) == 0)
   {
      cmsLog_error("no Eth ifName, just return");
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   cmsLog_debug("Looking for Eth interface info for %s", ifname);

   if ((ret = rutLan_getEthInterface(ifname, &iidStack, &lanEthObj)) == CMSRET_SUCCESS)
   {
      found = TRUE;
         
      cmsUtl_strcpy(status, lanEthObj->status);
      *upstream         = FALSE; /* LAN */ 
      *shapingRate      = lanEthObj->X_BROADCOM_COM_ShapingRate;
      *shapingBurstSize = lanEthObj->X_BROADCOM_COM_ShapingBurstSize;

      cmsObj_free((void **)&lanEthObj);
   }
#ifdef DMP_ETHERNETWAN_1
   else
   {
      WanEthIntfObject *wanEthObj = NULL;
      
      INIT_INSTANCE_ID_STACK(&iidStack);
      if ((ret = rutWl2_getWanEthObject(&iidStack, &wanEthObj)) == CMSRET_SUCCESS)
      {
         found = TRUE;
         
         cmsUtl_strcpy(status, wanEthObj->status);
         *upstream         = TRUE;  /* WAN */ 
         *shapingRate      = wanEthObj->shapingRate;
         *shapingBurstSize = wanEthObj->shapingBurstSize;

         cmsObj_free((void **)&wanEthObj);
      }
   }
#endif

   if (!found)
   {
      cmsLog_error("Cannot find Eth interface %s", ifname);
      return CMSRET_OBJECT_NOT_FOUND;
   }
   
   cmsLog_debug("Exit. ret %d", ret);

   return ret;

} /* End of qdmEth_getIntfInfoByNameLocked_igd() */


#ifdef SUPPORT_LANVLAN

CmsRet qdmEth_getAllL2EthIntfNameLocked_igd(char *buffer, UINT32 bufSize)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *ethObj=NULL;
   UBOOL8 first = TRUE;

   cmsLog_debug("Enter");

   buffer[0] = '\0';

   while (cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("cmsObj_getNext returns ethObj->X_BROADCOM_COM_IfName %s",
                   ethObj->X_BROADCOM_COM_IfName);

      if (!first)
      {
         strncat(buffer,"|", bufSize);
      }
      else
      {
         first = FALSE;
      }
      cmsUtl_strncat(buffer, bufSize, ethObj->X_BROADCOM_COM_IfName);
      cmsObj_free((void **) &ethObj);
   }

   return CMSRET_SUCCESS;
}

#endif /* SUPPORT_LANVLAN */

#endif /* DMP_BASELINE_1 */

