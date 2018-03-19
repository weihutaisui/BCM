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
#include "cms_phl.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_core.h"


CmsRet qdmRt_getActiveDefaultGatewayLocked_dev2(char *gwIfName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2RouterObject *routerObj=NULL;
   CmsRet ret;
   
   cmsLog_debug("Enter");

   gwIfName[0] = '\0';
   
   /*
    * Don't know why TR181 defines multiple router objects.  Just get the
    * first router object.
    */
   if ((ret = cmsObj_getNext(MDMOID_DEV2_ROUTER, &iidStack, (void **)&routerObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get MDMOID_DEV2_ROUTER, ret=%d", ret);
      return ret;
   }

   if (IS_EMPTY_STRING(routerObj->X_BROADCOM_COM_ActiveDefaultGateway))
   {
      cmsLog_debug("X_BROADCOM_COM_ActiveDefaultGateway is NULL");
   }
   else
   {
      strcpy(gwIfName, routerObj->X_BROADCOM_COM_ActiveDefaultGateway);
   }

   cmsObj_free((void **) &routerObj);

   cmsLog_debug("Exit ret %d,  gwIfName %s", ret, gwIfName);
   
   return ret;
}


CmsRet qdmRt_getDefaultGatewayIPLocked_dev2(char *gwIpAddr)
{
   char *ipIntfFullPath=NULL;
   char gwIntfNameBuf[CMS_IFNAME_LENGTH]={0};
   CmsRet ret;
   
   cmsLog_debug("Enter");

   gwIpAddr[0] = '\0';

   ret = qdmRt_getActiveDefaultGatewayLocked_dev2(gwIntfNameBuf);
   if (ret != CMSRET_SUCCESS)
   {
      /* no default gateway set, just return SUCCESS but empty gwIpAddr */
      return CMSRET_SUCCESS;
   }
   
   ret = qdmIntf_intfnameToFullPathLocked_dev2(gwIntfNameBuf, FALSE, &ipIntfFullPath);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get L3 fullpath for %s", gwIntfNameBuf);
      return ret;
   }

   ret = qdmRt_getGatewayIpv4AddrByFullPathLocked_dev2(ipIntfFullPath, gwIpAddr);

   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

   cmsLog_debug("Exit ret %d,  gwIpAddr %s", ret, gwIpAddr);

   return ret;
}


CmsRet qdmRt_getGatewayIpv4AddrByFullPathLocked_dev2(const char *ipIntfFullPath,
                                                     char *gwIpAddr)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Ipv4ForwardingObject *ipv4Forwarding=NULL;
   UBOOL8 found=FALSE;
   CmsRet ret;

   if (ipIntfFullPath == NULL)
   {
      cmsLog_error("NULL string.");
      return CMSRET_INVALID_ARGUMENTS;
   }
   cmsLog_debug("Enter ipIntffullPath %s", ipIntfFullPath);
   
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DEV2_IPV4_FORWARDING, &iidStack,
                                (void **) &ipv4Forwarding)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipv4Forwarding->interface, ipIntfFullPath) &&
          cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, ipv4Forwarding->destIPAddress))
      {
         found = TRUE;

         strcpy(gwIpAddr, ipv4Forwarding->gatewayIPAddress);
         cmsLog_debug("Found: %s => gwIpAddr %s (origin=%s)",
                      ipIntfFullPath, gwIpAddr, ipv4Forwarding->origin);

      }
      cmsObj_free((void **)&ipv4Forwarding);
   }

   return ret;
}




#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1

CmsRet qdmRt_getGatewayIpv6AddrByFullPathLocked_dev2(const char *ipIntfFullPath,
                                                     char *gwIpAddr,
                                                     char *origin)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Ipv6ForwardingObject *ipv6Forwarding=NULL;
   UBOOL8 found=FALSE;
   CmsRet ret;

   if (ipIntfFullPath == NULL)
   {
      cmsLog_error("NULL string.");
      return CMSRET_INVALID_ARGUMENTS;
   }
   cmsLog_debug("Enter ipIntffullPath %s", ipIntfFullPath);

   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DEV2_IPV6_FORWARDING, &iidStack,
                                (void **) &ipv6Forwarding)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipv6Forwarding->interface, ipIntfFullPath) &&
          cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, ipv6Forwarding->destIPPrefix))
      {
         found = TRUE;

         strcpy(gwIpAddr, ipv6Forwarding->nextHop);
         cmsLog_debug("Found: %s => gwIpAddr %s (origin=%s)",
                      ipIntfFullPath, gwIpAddr, ipv6Forwarding->origin);

         if (origin)
         {
            strcpy(origin, ipv6Forwarding->origin);
         }
      }
      cmsObj_free((void **)&ipv6Forwarding);
   }

   return ret;
}


CmsRet qdmRt_getGatewayIpv6AddrByIfNameLocked_dev2(const char *ifname,
                                                   char *gwIpAddr,
                                                   char *origin)
{
   char *ipIntfFullPath=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("ifname<%s>", ifname);

   ret = qdmIntf_intfnameToFullPathLocked_dev2(ifname, FALSE, &ipIntfFullPath);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get L3 fullpath for %s", ifname);
   }
   else
   {
      ret = qdmRt_getGatewayIpv6AddrByFullPathLocked_dev2(ipIntfFullPath,
                                                         gwIpAddr,
                                                         origin);
      if (ret != CMSRET_SUCCESS)
      {
         /*
          * Sometimes, there seems to be a timing issue where the
          * IPv6ForwardingObject from RA is not written into the
          * MDM by the time we get here.
          */
         cmsLog_notice("Could not get gwIpAddr for %s", ipIntfFullPath);
      }

      CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
   }

   cmsLog_debug("gwip<%s> origin<%s>", gwIpAddr, origin);

   return ret;
}


CmsRet qdmRt_getSysDfltGw6_dev2(char *gwIfc, char *gwIpAddr)
{
   Dev2RouterObject *routerObj=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Enter");
   if (gwIfc == NULL && gwIpAddr == NULL)
   {
      cmsLog_error("both input ptrs are NULL!");
      return CMSRET_INVALID_ARGUMENTS;
   }
   if (gwIfc != NULL)
   {
      *gwIfc = '\0';
   }
   if (gwIpAddr != NULL)
   {
      *gwIpAddr = '\0';
   }

   /*
    * Don't know why TR181 defines multiple router objects.  Just get the
    * first router object.
    */
   if ((ret = cmsObj_getNext(MDMOID_DEV2_ROUTER, &iidStack, (void **)&routerObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get MDMOID_DEV2_ROUTER, ret=%d", ret);
      return ret;
   }

   if (gwIfc != NULL)
   {
      if (!IS_EMPTY_STRING(routerObj->X_BROADCOM_COM_ActiveIpv6DefaultGateway))
      {
         strcpy(gwIfc, routerObj->X_BROADCOM_COM_ActiveIpv6DefaultGateway);
      }
   }

   if (gwIpAddr != NULL)
   {
      if (!IS_EMPTY_STRING(routerObj->X_BROADCOM_COM_ActiveIpv6DefaultGateway))
      {
         char *ipIntfFullPath=NULL;

         ret = qdmIntf_intfnameToFullPathLocked_dev2(routerObj->X_BROADCOM_COM_ActiveIpv6DefaultGateway,
                                                     FALSE, &ipIntfFullPath);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not get L3 fullpath for %s",
                       routerObj->X_BROADCOM_COM_ActiveIpv6DefaultGateway);
         }
         else
         {
            qdmRt_getGatewayIpv6AddrByFullPathLocked_dev2(ipIntfFullPath,
                                                          gwIpAddr, NULL);

            CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
         }
      }
   }

   cmsObj_free((void **) &routerObj);
   return ret;
}

#endif  /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */

#endif /* DMP_DEVICE2_BASELINE_1 */

