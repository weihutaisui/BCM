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

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
static CmsRet fillStaticRoute6Cfg_dev2(const char* addr, const char *gateway, const char *wanif, const char *metric, Dev2Ipv6ForwardingObject *routeCfg);
#endif

#ifdef DMP_DEVICE2_ROUTING_1

static CmsRet fillStaticRouteCfg_dev2(const char* addr, const char *mask, const char *gateway, const char *wanif, const char *metric, Dev2Ipv4ForwardingObject *ipv4ForwardingObj);

CmsRet dalRt_addStaticIpv4DefaultGateway_dev2(const char *gwIpAddr, const char *ipIntfFullPath)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2Ipv4ForwardingObject *ipv4ForwardingObj=NULL;
   Dev2RouterObject *routerObj=NULL;
   CmsRet ret;

   cmsLog_debug("Enter: gateway=%s ipIntf=%s", gwIpAddr, ipIntfFullPath);
   /*
    * Don't know why TR181 defines multiple router objects.  Just get the
    * first router object and add the ipv4ForwardingObj entry under that one.
    */
   if ((ret = cmsObj_getNext(MDMOID_DEV2_ROUTER, &iidStack, (void **)&routerObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get MDMOID_DEV2_ROUTER Instance, ret = %d", ret);
      return ret;
   }
   /* we only want the iidStack, not the object */
   cmsObj_free((void *) &routerObj);


   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add MDMOID_DEV2_IPV4_FORWARDING Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_IPV4_FORWARDING, &iidStack, 0, (void **) &ipv4ForwardingObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get ipv4ForwardingObj, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack);
      return ret;
   }

   ipv4ForwardingObj->enable = TRUE;
   /* Just copy webVar->wanIntfGateway over since for static defaultgateway, 
    * gatewayIPAddress will not be "0.0.0.0"
    */
   CMSMEM_REPLACE_STRING(ipv4ForwardingObj->gatewayIPAddress, gwIpAddr);
   CMSMEM_REPLACE_STRING(ipv4ForwardingObj->interface, ipIntfFullPath);

   if ((ret = cmsObj_set(ipv4ForwardingObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set ipv4ForwardingObj. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack);
   }

   cmsObj_free((void **) &ipv4ForwardingObj);

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}


void dalRt_deleteStaticIpv4DefaultGateway_dev2(const char *ipIntfFullPath)
{
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK; 
   Dev2Ipv4ForwardingObject *ipv4ForwardingObj=NULL;

   cmsLog_debug("Enter %s", ipIntfFullPath);

   while(!found &&
         cmsObj_getNextFlags(MDMOID_DEV2_IPV4_FORWARDING,
                             &iidStack,
                             OGF_NO_VALUE_UPDATE,
                             (void **) &ipv4ForwardingObj) == CMSRET_SUCCESS)
   {
      /* delete:
       *  (1) statically configured
       *  (2) default gateway
       *  (3) on this ipIntfFullPath
       * There should only be 1 of these, so after found, we can exit loop
       */

      if (!cmsUtl_strcmp(ipv4ForwardingObj->origin, MDMVS_STATIC) &&
          cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, ipv4ForwardingObj->destIPAddress) &&
          !cmsUtl_strcasecmp(ipv4ForwardingObj->interface, ipIntfFullPath))
      {
         CmsRet r2;

         found = TRUE;
         cmsLog_debug("Deleting instance at %s", cmsMdm_dumpIidStack(&iidStack));
         if ((r2 = cmsObj_deleteInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to delete  MDMOID_DEV2_IPV4_FORWARDING, ret=%d", r2);
         }
      }

      cmsObj_free((void **)&ipv4ForwardingObj);
   }

   return;
}


CmsRet dalRt_getDefaultGatewayList_dev2(char *gwIfNames)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2RouterObject *routerObj=NULL;

   cmsLog_debug("Enter");

   if (gwIfNames)
   {
      gwIfNames[0] = '\0';
   }
   else
   {
      cmsLog_error("gwIfNames is NULL.");
      return CMSRET_INTERNAL_ERROR;
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

   if (IS_EMPTY_STRING(routerObj->X_BROADCOM_COM_DefaultConnectionServices))
   {
      cmsLog_debug("X_BROADCOM_COM_DefaultConnectionServices is NULL");
   }
   else
   {
      strcpy(gwIfNames, routerObj->X_BROADCOM_COM_DefaultConnectionServices);
   }

   cmsObj_free((void **) &routerObj);

   cmsLog_debug("Exit, gwIfNames=%s", gwIfNames);

   return ret;
}


CmsRet dalRt_setDefaultGatewayList_dev2(const char *gwList)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2RouterObject *routerObj=NULL;
   char *newDefaultGatewayList=NULL;

   cmsLog_debug("Enter: gwList=%s", gwList);

   ret = dalWan_validateIfNameList(gwList, CMS_MAX_DEFAULT_GATEWAY,
                                   &newDefaultGatewayList);
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }


   /*
    * Don't know why TR181 defines multiple router objects.  Just get the
    * first router object.
    */
   if ((ret = cmsObj_getNext(MDMOID_DEV2_ROUTER, &iidStack, (void **)&routerObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get MDMOID_DEV2_ROUTER, ret=%d", ret);
      cmsMem_free(newDefaultGatewayList);
      return ret;
   }

   CMSMEM_REPLACE_STRING(routerObj->X_BROADCOM_COM_DefaultConnectionServices, newDefaultGatewayList);

   CMSMEM_FREE_BUF_AND_NULL_PTR(newDefaultGatewayList);

   if ((ret = cmsObj_set(routerObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_set MDMOID_DEV2_ROUTER returns error. ret=%d", ret);
   }

   cmsObj_free((void **) &routerObj);

   cmsLog_debug("Exit. ret=%d", ret);

   return ret;
}

CmsRet dalStaticRoute_addEntry_dev2(const char* addr, const char *mask, const char *gateway, const char *wanif, const char *metric)
{
   Dev2RouterObject *routerObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Ipv4ForwardingObject *ipv4ForwardingObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* adding a new static route entry */
   cmsLog_debug("Adding new static route with %s/%s/%s/%s/%s", addr, mask, gateway, wanif, metric);

   if ((ret = cmsObj_getNext(MDMOID_DEV2_ROUTER, &iidStack, (void **)&routerObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get MDMOID_DEV2_ROUTER Instance, ret = %d", ret);
      return ret;
   }
   /* we only want the iidStack, not the object */
   cmsObj_free((void *) &routerObj);


   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add MDMOID_DEV2_IPV4_FORWARDING Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_IPV4_FORWARDING, &iidStack, 0, (void **) &ipv4ForwardingObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get ipv4ForwardingObj, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack);
      return ret;
   }

   if ((ret = fillStaticRouteCfg_dev2(addr, mask, gateway, wanif, metric, ipv4ForwardingObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to fill static route cfg, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack);
      cmsObj_free((void **) &ipv4ForwardingObj);
      return ret;
   }

   if ((ret = cmsObj_set(ipv4ForwardingObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set ipv4ForwardingObj. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack);
   }

   cmsObj_free((void **) &ipv4ForwardingObj);

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;


}

/* Fill the given routeCfg object with the input data */

CmsRet fillStaticRouteCfg_dev2(const char* addr, const char *mask, const char *gateway, const char *wanif, const char *metric, Dev2Ipv4ForwardingObject *ipv4ForwardingObj)
{
   CmsRet ret=CMSRET_SUCCESS;
   char *ipIntfFullPath=NULL;

   if(!IS_EMPTY_STRING(wanif) && 
       (ret = qdmIntf_intfnameToFullPathLocked_dev2(wanif,FALSE,&ipIntfFullPath)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to convert ifname %s to fullpath, ret = %d", wanif, ret);
      return ret;
   }

   ipv4ForwardingObj->enable = TRUE;
   
   CMSMEM_REPLACE_STRING(ipv4ForwardingObj->destIPAddress, addr);
   
   CMSMEM_REPLACE_STRING(ipv4ForwardingObj->destSubnetMask, mask);
   
   CMSMEM_REPLACE_STRING(ipv4ForwardingObj->gatewayIPAddress, gateway);
   
   CMSMEM_REPLACE_STRING(ipv4ForwardingObj->interface, ipIntfFullPath);

   if (ipv4ForwardingObj->destIPAddress == NULL ||
       ipv4ForwardingObj->destSubnetMask == NULL || 
       ipv4ForwardingObj->gatewayIPAddress == NULL ||
       (ipIntfFullPath && ipv4ForwardingObj->interface == NULL))      
   {
      cmsLog_error("Failed to set ipv4 forwarding obj, ret = %d", wanif, ret);
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
      return CMSRET_RESOURCE_EXCEEDED;
   }
   
   if ( cmsUtl_strcmp(metric, "") != 0 )
   {
      ipv4ForwardingObj->forwardingMetric = atoi(metric);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

   return ret;
}

CmsRet dalStaticRoute_deleteEntry_dev2(const char* addr, const char *mask)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Ipv4ForwardingObject *routeCfg=NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   /* deleting a static route entry */
   cmsLog_debug("Deleting a static route with %s/%s", addr, mask);

   while ((ret = cmsObj_getNext(MDMOID_DEV2_IPV4_FORWARDING, &iidStack, (void **)&routeCfg)) == CMSRET_SUCCESS)
   {
      if(!cmsUtl_strcmp(addr, routeCfg->destIPAddress) && !cmsUtl_strcmp(mask, routeCfg->destSubnetMask)) 
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &routeCfg);
      }
   }

   if(found == FALSE) 
   {
      cmsLog_debug("bad addr/mask (%s/%s), no static route entry found", addr, mask);
      return CMSRET_INVALID_ARGUMENTS;
   }
   else 
   {
      ret = cmsObj_deleteInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack);
      cmsObj_free((void **) &routeCfg);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete IPv4ForwardingEntryObject, ret = %d", ret);
         return ret;
      }
   }

   return ret;
}

#ifdef SUPPORT_POLICYROUTING
CmsRet fillPolicyRouteCfg_dev2(const char* PolicyName, const char *SourceIp, const char *SourceIfName, 
                                  const char *wanif, const char *defaultGW, Dev2Ipv4ForwardingObject *routeCfg)
{
   CmsRet ret=CMSRET_SUCCESS;
   char *ipIntfFullPath=NULL;

   if(!IS_EMPTY_STRING(wanif) && 
       (ret = qdmIntf_intfnameToFullPathLocked_dev2(wanif,FALSE,&ipIntfFullPath)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to convert ifname %s to fullpath, ret = %d", wanif, ret);
      return ret;
   }

   routeCfg->enable = TRUE;
   REPLACE_STRING_IF_NOT_EQUAL(routeCfg->X_BROADCOM_COM_PolicyRoutingName, PolicyName);
   REPLACE_STRING_IF_NOT_EQUAL(routeCfg->X_BROADCOM_COM_SourceIPAddress, SourceIp);
   REPLACE_STRING_IF_NOT_EQUAL(routeCfg->X_BROADCOM_COM_SourceIfName, SourceIfName);
   REPLACE_STRING_IF_NOT_EQUAL(routeCfg->interface, ipIntfFullPath);
   REPLACE_STRING_IF_NOT_EQUAL(routeCfg->gatewayIPAddress, defaultGW);
   
   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

   return CMSRET_SUCCESS;
}


CmsRet dalPolicyRouting_addEntry_dev2(const char* PolicyName, const char *SourceIp, const char *SourceIfName, 
                                             const char *wanif, const char *defaultGW)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Ipv4ForwardingObject *routeCfg = NULL;
   Dev2RouterObject *routerObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* adding a new policy route entry */
   cmsLog_debug("Adding new policy route with %s/%s/%s/%s/%s", PolicyName, wanif, SourceIp, SourceIfName, defaultGW);

   if ((ret = cmsObj_getNext(MDMOID_DEV2_ROUTER, &iidStack, (void **)&routerObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get MDMOID_DEV2_ROUTER Instance, ret = %d", ret);
      return ret;
   }
   
   /* we only want the iidStack, not the object */
   cmsObj_free((void *) &routerObj);
   
   /* add new instance of Forwarding */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new Forwarding, ret=%d", ret);
      return ret;
   }

   /* get the instance of dslLinkConfig in the newly created WanConnectionDevice sub-tree */
   if ((ret = cmsObj_get(MDMOID_DEV2_IPV4_FORWARDING, &iidStack, 0, (void **) &routeCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get MDMOID_DEV2_IPV4_FORWARDING, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack);
      return ret;
   }       

   if ((ret = fillPolicyRouteCfg_dev2(PolicyName, SourceIp, SourceIfName, wanif, defaultGW, routeCfg)) != CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &routeCfg);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack);
      return ret;
   }
   
   /* set and activate MDMOID_DEV2_IPV4_FORWARDING */
   ret = cmsObj_set(routeCfg, &iidStack);
   cmsObj_free((void **) &routeCfg);

   if (ret != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("Failed to set MDMOID_DEV2_IPV4_FORWARDING, ret = %d", ret);
       
      r2 = cmsObj_deleteInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created MDMOID_DEV2_IPV4_FORWARDING, r2=%d", r2);
      }

      cmsLog_debug("Failed to set policy route and successfully delete created MDMOID_DEV2_IPV4_FORWARDING");

   }

   return ret;
}


CmsRet dalPolicyRouting_deleteEntry_dev2(const char* PolicyName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Ipv4ForwardingObject *routeCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   /* deleting a policy route entry */
   cmsLog_debug("Deleting a policy route with %s", PolicyName);

   while (cmsObj_getNext(MDMOID_DEV2_IPV4_FORWARDING, &iidStack, (void **) &routeCfg) == CMSRET_SUCCESS)
   {
      if(!cmsUtl_strcmp(PolicyName, routeCfg->X_BROADCOM_COM_PolicyRoutingName)) 
      {
         found = TRUE;
         cmsObj_free((void **) &routeCfg);
         break;
      }
      cmsObj_free((void **) &routeCfg);
   }

   if(found == FALSE) 
   {
      cmsLog_debug("bad policy name (%s), no policy route entry found", PolicyName);
      return CMSRET_INVALID_ARGUMENTS;
   }
   else 
   {
      ret = cmsObj_deleteInstance(MDMOID_DEV2_IPV4_FORWARDING, &iidStack);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete MDMOID_DEV2_IPV4_FORWARDING, ret = %d", ret);
         return ret;
      }
   }

   return ret;
}
#endif /*SUPPORT_POLICYROUTING*/
#endif  /* DMP_DEVICE2_ROUTING_1 */

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
CmsRet dalStaticRoute6_addEntry_dev2(const char* addr, const char *gateway, const char *wanif, const char *metric)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2RouterObject *routerObj=NULL;
   Dev2Ipv6ForwardingObject *routeCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* adding a new static route entry */
   cmsLog_debug("Adding new ipv6 static route with %s %s %s %s", addr, gateway, wanif, metric);

   if ((ret = cmsObj_getNext(MDMOID_DEV2_ROUTER, &iidStack, (void **)&routerObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get MDMOID_DEV2_ROUTER Instance, ret = %d", ret);
      return ret;
   }
   /* we only want the iidStack, not the object */
   cmsObj_free((void *) &routerObj);
   
  /* add new instance of Forwarding */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV6_FORWARDING, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new Forwarding, ret=%d", ret);
      return ret;
   }

   /* get the instance of the newly created IPv6L3ForwardingEntryObject */
   if ((ret = cmsObj_get(MDMOID_DEV2_IPV6_FORWARDING, &iidStack, 0, (void **) &routeCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get L3ForwardingEntryObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV6_FORWARDING, &iidStack);
      return ret;
   }       

   if ((ret = fillStaticRoute6Cfg_dev2(addr, gateway, wanif, metric, routeCfg)) != CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &routeCfg);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV6_FORWARDING, &iidStack);
      return ret;
   }

   cmsLog_debug("in routeCfg, addr=%s, gtwy=%s, wanif=%s, metric=%d",
                routeCfg->destIPPrefix,
                routeCfg->nextHop,
                routeCfg->interface,
                routeCfg->forwardingMetric);
   
   /* set and activate IPv6ForwardingEntryObject */
   ret = cmsObj_set(routeCfg, &iidStack);
   cmsObj_free((void **) &routeCfg);

   if (ret != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("Failed to set IPv6ForwardingEntryObject, ret = %d", ret);
       
      r2 = cmsObj_deleteInstance(MDMOID_DEV2_IPV6_FORWARDING, &iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created IPv6ForwardingEntryObject, r2=%d", r2);
      }

      cmsLog_debug("Failed to set static route and successfully delete created IPv6L3ForwardingEntryObject");

   }

   return ret;

}  /* End of dalStaticRoute6_addEntry_dev2() */

CmsRet dalStaticRoute6_deleteEntry_dev2(const char* addr)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Ipv6ForwardingObject *routeCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   /* deleting a static route entry */
   cmsLog_debug("Deleting an ipv6 static route with %s", addr);

   while ((ret = cmsObj_getNext(MDMOID_DEV2_IPV6_FORWARDING, &iidStack, (void **)&routeCfg)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(addr, routeCfg->destIPPrefix) == 0)
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &routeCfg);
      }
   }

   if (found == FALSE) 
   {
      cmsLog_debug("bad addr (%s), no ipv6 static route entry found", addr);
      return CMSRET_INVALID_ARGUMENTS;
   }
   else 
   {
      ret = cmsObj_deleteInstance(MDMOID_DEV2_IPV6_FORWARDING, &iidStack);
      cmsObj_free((void **) &routeCfg);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete IPv6L3ForwardingEntryObject, ret = %d", ret);
         return ret;
      }
   }

   return ret;

}  /* End of dalStaticRoute6_deleteEntry_dev2() */


CmsRet cmsDal_setSysDfltGw6Ifc_dev2(char *gwIfc)
{
   Dev2RouterObject *routerObj=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Enter: gwIfc=%s", gwIfc);
   
   if (gwIfc == NULL)
   {
      cmsLog_debug("gwIfc == NULL.  Do nothing");
      return CMSRET_SUCCESS;
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

   /* ConnectionServices is a LIST of wan intf names, but for now, the WebUI
    * only takes a single wan intfName from the user.  So just stick it in
    * here.  When we do the set, the RCL handler function will update the
    * ActiveIpv6DefaultGateway
    */
   CMSMEM_REPLACE_STRING(routerObj->X_BROADCOM_COM_DefaultIpv6ConnectionServices,
                         gwIfc);

   ret = cmsObj_set(routerObj, &iidStack);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Set of routerObj failed, ret=%d", ret);
   }

   cmsObj_free((void **) &routerObj);

   cmsLog_debug("Exit. ret=%d", ret);

   return ret;
}


/* Fill the given routeCfg object with the input data */
CmsRet fillStaticRoute6Cfg_dev2(const char* addr, const char *gateway, const char *wanif, const char *metric, Dev2Ipv6ForwardingObject *routeCfg)
{
   CmsRet ret = CMSRET_SUCCESS;
   char *fullPath=NULL;

   if (!IS_EMPTY_STRING(wanif)  && (ret = qdmIntf_intfnameToFullPathLocked_dev2(wanif, FALSE, &fullPath)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get fullPath on %s, ret=%d", wanif, ret);
      return CMSRET_INVALID_ARGUMENTS;
   }

   routeCfg->enable = TRUE;

   CMSMEM_REPLACE_STRING(routeCfg->destIPPrefix, addr);
   CMSMEM_REPLACE_STRING(routeCfg->nextHop, gateway);
   CMSMEM_REPLACE_STRING(routeCfg->interface, fullPath);
      
   if ((routeCfg->destIPPrefix == NULL) ||
       (routeCfg->nextHop == NULL) ||
       (!IS_EMPTY_STRING(wanif) && (routeCfg->interface == NULL)))
   {
      cmsLog_error("malloc failed.");
      ret = CMSRET_RESOURCE_EXCEEDED;
   }

   if ( cmsUtl_strcmp(metric, ""))
   {
      routeCfg->forwardingMetric = atoi(metric);
   }

   if (fullPath)
      CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);

   return ret;

}  /* End of fillStaticRoute6Cfg() */

#endif /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */

#ifdef SUPPORT_RIP
CmsRet dalRip_setRipEnableByIntfSettingEntries()
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2RipObject *ripObj;
   CmsRet ret;
   UBOOL8 ripEnable;
   Dev2RipIntfSettingObject *ripIntfSetting = NULL;

   if ((ret = cmsObj_get(MDMOID_DEV2_RIP, &iidStack, 0, (void **) &ripObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get MDMOID_DEV2_RIP, ret=%d", ret);
      return ret;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   ripEnable = FALSE;
   while (!ripEnable && cmsObj_getNext(MDMOID_DEV2_RIP_INTF_SETTING, &iidStack, (void **) &ripIntfSetting) == CMSRET_SUCCESS)
   {
      if (ripIntfSetting->enable) ripEnable = TRUE;
      cmsObj_free((void **) &ripIntfSetting);
   }
   
   cmsLog_debug("ripEnable=%d, ripObj->enable=%d", ripEnable, ripObj->enable);
   
   if ((ripEnable && !ripObj->enable) || (!ripEnable && ripObj->enable))
   {
      ripObj->enable = ripEnable;
      INIT_INSTANCE_ID_STACK(&iidStack);
      if ((ret = cmsObj_set((void *)ripObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not set MDMOID_DEV2_RIP, ret=%d", ret);
      }
   }
   
   cmsObj_free((void **) &ripObj);

   return ret;
}



CmsRet dalRip_setRipEntry_dev2(char *pIfcName, char *pRipVer, char *pOperation, char *pEnabled)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2RipIntfSettingObject *ripIntfSetting = NULL;
   char l3IntfNameBuf[CMS_IFNAME_LENGTH]={0};
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   cmsLog_debug("Enter");
   
   while (!found && cmsObj_getNext(MDMOID_DEV2_RIP_INTF_SETTING, &iidStack, (void **) &ripIntfSetting) == CMSRET_SUCCESS)
   {
      if (!ripIntfSetting->interface ||
         (ret = qdmIntf_fullPathToIntfnameLocked(ripIntfSetting->interface, l3IntfNameBuf)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmIntf_fullPathToIntfnameLocked on %s ret=%d", ripIntfSetting->interface, ret);
         cmsObj_free((void **) &ripIntfSetting);
      }
      else if (cmsUtl_strcmp(l3IntfNameBuf, pIfcName) == 0)
      {
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) &ripIntfSetting);
      }
   }

   if (found)
   {
      ripIntfSetting->enable = cmsUtl_strcmp(pEnabled, "0");
      ripIntfSetting->X_BROADCOM_COM_Version = atoi(pRipVer);
      
      if (cmsUtl_strcmp(pOperation, "0") == 0)
      {
         ripIntfSetting->acceptRA = TRUE;
         ripIntfSetting->sendRA = TRUE; 	 
      }
      else
      {
         ripIntfSetting->acceptRA = TRUE;
         ripIntfSetting->sendRA = FALSE;		 
      }
      
      cmsLog_debug("ifcName=%s, enabled=%s, version=%s, operation=%s", pIfcName, pEnabled, pRipVer, pOperation);
      
      if ((ret =  cmsObj_set((void *)ripIntfSetting, &iidStack))!= CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_set MDMOID_DEV2_RIP_INTF_SETTING on %s ret=%d", ripIntfSetting->interface, ret);
      }
      else
      {
         /* Also enable or disable Device.Routing.RIP.enable as GUI will not config this parameter */
         dalRip_setRipEnableByIntfSettingEntries();
      }
      
      cmsObj_free((void **) &ripIntfSetting);
   }
   else
   {
      cmsLog_error("Rip interface entry %s not found", ripIntfSetting->interface);
   }
   

   cmsLog_debug("ret =%d", ret);

   return ret;
   
}


CmsRet dalRip_addRipInterfaceSetting_dev2(const char *ipIntfFullPath)
{
   Dev2RipIntfSettingObject *ripIntfSettingObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
   
   cmsLog_debug("Enter, interface full path = %s", ipIntfFullPath);
   
   /* RIP object is only available for pure181 data model */
   if (!cmsMdm_isDataModelDevice2())
   {
      return ret;
   }

   if ((ret = cmsObj_addInstance(MDMOID_DEV2_RIP_INTF_SETTING, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add MDMOID_DEV2_RIP_INTF_SETTING, ret = %d", ret);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_RIP_INTF_SETTING, &iidStack, 0, (void **) &ripIntfSettingObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get MDMOID_DEV2_RIP_INTF_SETTING, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_RIP_INTF_SETTING, &iidStack);
      return ret;
   }

   CMSMEM_REPLACE_STRING(ripIntfSettingObj->interface, ipIntfFullPath);

   if( (ret = cmsObj_set(ripIntfSettingObj, &iidStack)) != CMSRET_SUCCESS)
      cmsObj_deleteInstance(MDMOID_DEV2_RIP_INTF_SETTING, &iidStack);

   cmsObj_free((void **) &ripIntfSettingObj);
   
   cmsLog_debug("Exit, ret = %d", ret);
   
   return ret;
}


CmsRet dalRip_deleteRipInterfaceSetting_dev2(const char *ipIntfFullPath)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack2=EMPTY_INSTANCE_ID_STACK;
   Dev2RipIntfSettingObject *ripIntfSetting = NULL;
   CmsRet ret;

   cmsLog_debug("enter: ipIntf %s", ipIntfFullPath);

   while ((ret = cmsObj_getNextFlags(MDMOID_DEV2_RIP_INTF_SETTING,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&ripIntfSetting)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ripIntfSetting->interface, ipIntfFullPath))
      {
         cmsObj_deleteInstance(MDMOID_DEV2_RIP_INTF_SETTING, &iidStack);
         iidStack = iidStack2;
      }
      else
      {
         iidStack2 = iidStack;
      }
	  
      cmsObj_free((void **)&ripIntfSetting);
   }

   dalRip_setRipEnableByIntfSettingEntries();

   cmsLog_debug("Exit: ret=%d", ret);

   return ret;
}

#endif /* SUPPORT_RIP */

#endif /* DMP_DEVICE2_BASELINE_1 */
