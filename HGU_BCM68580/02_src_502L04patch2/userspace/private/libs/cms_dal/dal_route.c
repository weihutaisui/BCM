/*
* <:copyright-BRCM:2012:proprietary:standard
* 
*    Copyright (c) 2012 Broadcom 
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
* :>
*/

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_qdm.h"
#include "cms_util.h"


#ifdef DMP_BASELINE_1

/* local functions */
static CmsRet fillStaticRouteCfg_igd(const char* addr, const char *mask, const char *gateway, const char *wanif, const char *metric, L3ForwardingEntryObject *routeCfg);

#ifdef SUPPORT_POLICYROUTING
static CmsRet fillPolicyRouteCfg_igd(const char* PolicyName, const char *SourceIp, const char *SourceIfName, 
                                                   const char *wanif, const char *defaultGW, L3ForwardingEntryObject *routeCfg);
#endif

#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
static CmsRet fillStaticRoute6Cfg_igd(const char* addr, const char *gateway, const char *wanif, 
                                  const char *metric, IPv6L3ForwardingEntryObject *route6Cfg);
#endif

CmsRet dalStaticRoute_addEntry_igd(const char* addr, const char *mask, const char *gateway, const char *wanif, const char *metric)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   L3ForwardingEntryObject *routeCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* adding a new static route entry */
   cmsLog_debug("Adding new static route with %s/%s/%s/%s/%s", addr, mask, gateway, wanif, metric);

   /* add new instance of Forwarding */
   if ((ret = cmsObj_addInstance(MDMOID_L3_FORWARDING_ENTRY, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new Forwarding, ret=%d", ret);
      return ret;
   }

   /* get the instance of dslLinkConfig in the newly created WanConnectionDevice sub-tree */
   if ((ret = cmsObj_get(MDMOID_L3_FORWARDING_ENTRY, &iidStack, 0, (void **) &routeCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get L3ForwardingEntryObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_L3_FORWARDING_ENTRY, &iidStack);
      return ret;
   }       

   if ((ret = fillStaticRouteCfg_igd(addr, mask, gateway, wanif, metric, routeCfg)) != CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &routeCfg);
      cmsObj_deleteInstance(MDMOID_L3_FORWARDING_ENTRY, &iidStack);
      return ret;
   }

   cmsLog_debug("in routeCfg, addr=%s, mask=%s, gtwy=%s, wanif=%s, metric=%d", 
                 routeCfg->destIPAddress, routeCfg->destSubnetMask, 
                 routeCfg->gatewayIPAddress, routeCfg->interface, routeCfg->forwardingMetric);
   
   /* set and activate L3ForwardingEntryObject */
   ret = cmsObj_set(routeCfg, &iidStack);
   cmsObj_free((void **) &routeCfg);

   if (ret != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("Failed to set L3ForwardingEntryObject, ret = %d", ret);
       
      r2 = cmsObj_deleteInstance(MDMOID_L3_FORWARDING_ENTRY, &iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created L3ForwardingEntryObject, r2=%d", r2);
      }

      cmsLog_debug("Failed to set static route and successfully delete created L3ForwardingEntryObject");

   }

   return ret;

}

/* Fill the given routeCfg object with the input data */

CmsRet fillStaticRouteCfg_igd(const char* addr, const char *mask, const char *gateway, const char *wanif, const char *metric, L3ForwardingEntryObject *routeCfg)
{
   CmsRet ret=CMSRET_SUCCESS;

   routeCfg->enable = TRUE;

   cmsMem_free(routeCfg->destIPAddress);
   routeCfg->destIPAddress = cmsMem_strdup(addr);

   cmsMem_free(routeCfg->destSubnetMask);
   routeCfg->destSubnetMask = cmsMem_strdup(mask);

   cmsMem_free(routeCfg->gatewayIPAddress);
   routeCfg->gatewayIPAddress = cmsMem_strdup(gateway);

   cmsMem_free(routeCfg->interface);
   routeCfg->interface = cmsMem_strdup(wanif);
      
   if ((routeCfg->destIPAddress == NULL) ||(routeCfg->destSubnetMask == NULL) ||
       (routeCfg->gatewayIPAddress == NULL) || (routeCfg->interface == NULL))
   {
      cmsLog_error("malloc failed.");
      cmsObj_free((void **) &routeCfg);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   if ( cmsUtl_strcmp(metric, "") != 0 )
   {
      routeCfg->forwardingMetric = atoi(metric);
   }

   return ret;
}

CmsRet dalStaticRoute_deleteEntry_igd(const char* addr, const char *mask)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   L3ForwardingEntryObject *routeCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   /* deleting a static route entry */
   cmsLog_debug("Deleting a static route with %s/%s", addr, mask);

   while ((ret = cmsObj_getNext(MDMOID_L3_FORWARDING_ENTRY, &iidStack, (void **)&routeCfg)) == CMSRET_SUCCESS)
   {
      if(!strcmp(addr, routeCfg->destIPAddress) && !strcmp(mask, routeCfg->destSubnetMask)) 
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
      ret = cmsObj_deleteInstance(MDMOID_L3_FORWARDING_ENTRY, &iidStack);
      cmsObj_free((void **) &routeCfg);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete L3ForwardingEntryObject, ret = %d", ret);
         return ret;
      }
   }

   return ret;
}
#endif /*DMP_BASELINE_1*/



/* this routine get the active default gateway Interface's name */
CmsRet dalRt_getActiveDefaultGateway(char *varValue)
{
   char gwIfName[CMS_IFNAME_LENGTH]={0};
   CmsRet ret;
   
   strcpy(varValue, "&nbsp");

   ret = qdmRt_getActiveDefaultGatewayLocked(gwIfName);

   if (gwIfName[0] != '\0')
   {
      strcpy(varValue, gwIfName);
   }

   return ret;
}

/* this routine get the active default gateway Interface's IP address */
CmsRet dalRt_getDefaultGatewayIP(char *varValue)
{
   char gwIP[CMS_IPADDR_LENGTH]={0};
   CmsRet ret;
   
   strcpy(varValue, "&nbsp");

   ret = qdmRt_getDefaultGatewayIPLocked(gwIP);

   if (gwIP[0] != '\0')
   {
      strcpy(varValue, gwIP);
   }

   return ret;
}


#ifdef DMP_BASELINE_1

CmsRet dalRt_getDefaultGatewayList_igd(char *gwIfNames)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   L3ForwardingObject *L3ForwadingObj=NULL;

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
   
   if ((ret = cmsObj_get(MDMOID_L3_FORWARDING, &iidStack, 0, (void **)&L3ForwadingObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get MDMOID_L3_FORWARDING, ret=%d", ret);
      return ret;
   }

   if (IS_EMPTY_STRING(L3ForwadingObj->X_BROADCOM_COM_DefaultConnectionServices))
   {
      cmsLog_debug("X_BROADCOM_COM_DefaultConnectionServices is NULL");
   }
   else
   {
      strcpy(gwIfNames, L3ForwadingObj->X_BROADCOM_COM_DefaultConnectionServices);
   }
   
   cmsObj_free((void **) &L3ForwadingObj);

   cmsLog_debug("Exit, gwIfNames=%s", gwIfNames);
   
   return ret;
} 



CmsRet dalRt_setDefaultGatewayList_igd(const char *gwList)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   L3ForwardingObject *L3ForwadingObj=NULL;
   char *newDefaultGatewayList=NULL;
   
   cmsLog_debug("Enter: gwList=%s", gwList);

   ret = dalWan_validateIfNameList(gwList, CMS_MAX_DEFAULT_GATEWAY,
                                   &newDefaultGatewayList);
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }


   if ((ret = cmsObj_get(MDMOID_L3_FORWARDING, &iidStack, 0, (void **)&L3ForwadingObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get MDMOID_L3_FORWARDING, ret=%d", ret);
      cmsMem_free(newDefaultGatewayList);      
      return ret;
   }

   CMSMEM_REPLACE_STRING(L3ForwadingObj->X_BROADCOM_COM_DefaultConnectionServices, newDefaultGatewayList);

   CMSMEM_FREE_BUF_AND_NULL_PTR(newDefaultGatewayList);
   
   if ((ret = cmsObj_set(L3ForwadingObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_set <MDMOID_L3_FORWARDING> returns error. ret=%d", ret);
   }

   cmsObj_free((void **) &L3ForwadingObj);

   cmsLog_debug("Exit. ret=%d", ret);
   
   return ret;
}


#ifdef SUPPORT_POLICYROUTING
CmsRet fillPolicyRouteCfg_igd(const char* PolicyName, const char *SourceIp, const char *SourceIfName, 
                                                   const char *wanif, const char *defaultGW, L3ForwardingEntryObject *routeCfg)
{
   CmsRet ret=CMSRET_SUCCESS;

   routeCfg->enable = TRUE;

   CMSMEM_REPLACE_STRING(routeCfg->X_BROADCOM_COM_PolicyRoutingName, PolicyName);

   if(SourceIp)
      CMSMEM_REPLACE_STRING(routeCfg->sourceIPAddress, SourceIp);

   if(SourceIfName)
      CMSMEM_REPLACE_STRING(routeCfg->X_BROADCOM_COM_SourceIfName, SourceIfName);
   
   CMSMEM_REPLACE_STRING(routeCfg->interface, wanif);

   if(defaultGW)
      CMSMEM_REPLACE_STRING(routeCfg->gatewayIPAddress, defaultGW);

   return ret;
}


CmsRet dalPolicyRouting_addEntry_igd(const char* PolicyName, const char *SourceIp, const char *SourceIfName, 
                                                                  const char *wanif, const char *defaultGW)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   L3ForwardingEntryObject *routeCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* adding a new policy route entry */
   cmsLog_debug("Adding new policy route with %s/%s/%s/%s/%s", PolicyName, wanif, SourceIp, SourceIfName, defaultGW);

   /* add new instance of Forwarding */
   if ((ret = cmsObj_addInstance(MDMOID_L3_FORWARDING_ENTRY, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new Forwarding, ret=%d", ret);
      return ret;
   }

   /* get the instance of dslLinkConfig in the newly created WanConnectionDevice sub-tree */
   if ((ret = cmsObj_get(MDMOID_L3_FORWARDING_ENTRY, &iidStack, 0, (void **) &routeCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get L3ForwardingEntryObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_L3_FORWARDING_ENTRY, &iidStack);
      return ret;
   }       

   if ((ret = fillPolicyRouteCfg_igd(PolicyName, SourceIp, SourceIfName, wanif, defaultGW, routeCfg)) != CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &routeCfg);
      cmsObj_deleteInstance(MDMOID_L3_FORWARDING_ENTRY, &iidStack);
      return ret;
   }
   
   /* set and activate L3ForwardingEntryObject */
   ret = cmsObj_set(routeCfg, &iidStack);
   cmsObj_free((void **) &routeCfg);

   if (ret != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("Failed to set L3ForwardingEntryObject, ret = %d", ret);
       
      r2 = cmsObj_deleteInstance(MDMOID_L3_FORWARDING_ENTRY, &iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created L3ForwardingEntryObject, r2=%d", r2);
      }

      cmsLog_debug("Failed to set policy route and successfully delete created L3ForwardingEntryObject");

   }

   return ret;
}


CmsRet dalPolicyRouting_deleteEntry_igd(const char* PolicyName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   L3ForwardingEntryObject *routeCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   /* deleting a policy route entry */
   cmsLog_debug("Deleting a policy route with %s", PolicyName);

   while (cmsObj_getNext(MDMOID_L3_FORWARDING_ENTRY, &iidStack, (void **) &routeCfg) == CMSRET_SUCCESS)
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
      ret = cmsObj_deleteInstance(MDMOID_L3_FORWARDING_ENTRY, &iidStack);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete L3ForwardingEntryObject, ret = %d", ret);
         return ret;
      }
   }

   return ret;
}
#endif /*SUPPORT_POLICYROUTING*/


#ifdef SUPPORT_RIP
CmsRet dalRip_setRipEntry_igd(char *pIfcName, char *pRipVer, char *pOperation, char *pEnabled)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject *ipConn = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   cmsLog_debug("Enter");
   while (!found && cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConn) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(ipConn->X_BROADCOM_COM_IfName, pIfcName) == 0)
      {
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) &ipConn);
      }
   }

   if (found)
   {

      /* default for operation is PASSIVE, (if routeProtocolRx is "Off") */
      cmsMem_free(ipConn->X_BROADCOM_COM_RipOperationMode);
      ipConn->X_BROADCOM_COM_RipOperationMode = cmsMem_strdup(MDMVS_PASSIVE);
      
      cmsMem_free(ipConn->routeProtocolRx);
      if (cmsUtl_strcmp(pEnabled, "0") == 0)
      {
         ipConn->routeProtocolRx = cmsMem_strdup(MDMVS_OFF);
      }
      else
      {
         if (cmsUtl_strcmp(pRipVer, "1") == 0)
         {
            ipConn->routeProtocolRx = cmsMem_strdup(MDMVS_RIPV1);
         }
         else if  (cmsUtl_strcmp(pRipVer, "2") == 0)
         {
            ipConn->routeProtocolRx = cmsMem_strdup(MDMVS_RIPV2);
         }
         else
         {
            ipConn->routeProtocolRx = cmsMem_strdup(MDMVS_RIPV1V2);
         }
         if (cmsUtl_strcmp(pOperation, "0") == 0)
         {
            ipConn->X_BROADCOM_COM_RipOperationMode = cmsMem_strdup(MDMVS_ACTIVE);
         }
      }

      cmsLog_debug("version=%s, operation=%s",  ipConn->routeProtocolRx, ipConn->X_BROADCOM_COM_RipOperationMode);

      ret = cmsObj_set((void *)ipConn, &iidStack);
      cmsObj_free((void **) &ipConn);
   }

   cmsLog_debug("ret =%d", ret);

   return ret;
   
}
#endif /* SUPPORT_RIP */

#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */

CmsRet cmsDal_setSysDfltGw6Ifc_igd(char *gwIfc)
{
   InstanceIdStack iidStack;
   IPv6L3ForwardingObject *ipv6Obj = NULL;
   CmsRet ret;

   cmsLog_debug("Enter");
   
   if (gwIfc == NULL)
   {
      cmsLog_debug("gwIfc == NULL.  Do nothing");
      return CMSRET_SUCCESS;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_I_PV6_L3_FORWARDING, &iidStack, 0, (void **)&ipv6Obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_I_PV6_L3_FORWARDING> returns error. ret=%d", ret);
      return ret;
   }   
   cmsLog_debug("Current defaultConnectionService=%s", ipv6Obj->defaultConnectionService);

   if (*gwIfc == '\0')
   {
      if (IS_EMPTY_STRING(ipv6Obj->defaultConnectionService))
      {
         cmsObj_free((void **)&ipv6Obj);
         return ret;
      }

      CMSMEM_FREE_BUF_AND_NULL_PTR(ipv6Obj->defaultConnectionService);
   }
   else
   {
      if (cmsUtl_strcmp(gwIfc, ipv6Obj->defaultConnectionService) == 0)
      {
         cmsObj_free((void **)&ipv6Obj);
         return ret;
      }

      CMSMEM_REPLACE_STRING(ipv6Obj->defaultConnectionService, gwIfc);
   }

   if ((ret = cmsObj_set(ipv6Obj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_set <MDMOID_I_PV6_L3_FORWARDING> returns error. ret=%d", ret);
   }
      
   cmsObj_free((void **) &ipv6Obj);   

   cmsLog_debug("Exit. ret=%d", ret);
   return ret;
   
}  /* End of cmsDal_setSysDfltGw6Ifc_igd() */

CmsRet dalStaticRoute6_addEntry_igd(const char* addr, const char *gateway, const char *wanif, const char *metric)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPv6L3ForwardingEntryObject *routeCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* adding a new static route entry */
   cmsLog_debug("Adding new ipv6 static route with %s %s %s %s", addr, gateway, wanif, metric);

   /* add new instance of Forwarding */
   if ((ret = cmsObj_addInstance(MDMOID_I_PV6_L3_FORWARDING_ENTRY, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new Forwarding, ret=%d", ret);
      return ret;
   }

   /* get the instance of the newly created IPv6L3ForwardingEntryObject */
   if ((ret = cmsObj_get(MDMOID_I_PV6_L3_FORWARDING_ENTRY, &iidStack, 0, (void **) &routeCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get L3ForwardingEntryObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_I_PV6_L3_FORWARDING_ENTRY, &iidStack);
      return ret;
   }       

   if ((ret = fillStaticRoute6Cfg_igd(addr, gateway, wanif, metric, routeCfg)) != CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &routeCfg);
      cmsObj_deleteInstance(MDMOID_I_PV6_L3_FORWARDING_ENTRY, &iidStack);
      return ret;
   }

   cmsLog_debug("in routeCfg, addr=%s, gtwy=%s, wanif=%s, metric=%d",
                routeCfg->destIPv6Address,
                routeCfg->gatewayIPv6Address,
                routeCfg->interface,
                routeCfg->forwardingMetric);
   
   /* set and activate IPv6L3ForwardingEntryObject */
   ret = cmsObj_set(routeCfg, &iidStack);
   cmsObj_free((void **) &routeCfg);

   if (ret != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("Failed to set IPv6L3ForwardingEntryObject, ret = %d", ret);
       
      r2 = cmsObj_deleteInstance(MDMOID_I_PV6_L3_FORWARDING_ENTRY, &iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created IPv6L3ForwardingEntryObject, r2=%d", r2);
      }

      cmsLog_debug("Failed to set static route and successfully delete created IPv6L3ForwardingEntryObject");

   }

   return ret;

}  /* End of dalStaticRoute6_addEntry_igd() */

/* Fill the given routeCfg object with the input data */
CmsRet fillStaticRoute6Cfg_igd(const char* addr, const char *gateway, const char *wanif, const char *metric, IPv6L3ForwardingEntryObject *routeCfg)
{
   CmsRet ret = CMSRET_SUCCESS;

   routeCfg->enable = TRUE;

   CMSMEM_REPLACE_STRING(routeCfg->destIPv6Address, addr);
   CMSMEM_REPLACE_STRING(routeCfg->gatewayIPv6Address, gateway);
   CMSMEM_REPLACE_STRING(routeCfg->interface, wanif);
      
   if ((routeCfg->destIPv6Address == NULL) ||
       (routeCfg->gatewayIPv6Address == NULL) ||
       (routeCfg->interface == NULL))
   {
      cmsLog_error("malloc failed.");
      ret = CMSRET_RESOURCE_EXCEEDED;
   }

   if ( cmsUtl_strcmp(metric, "") != 0 )
   {
      routeCfg->forwardingMetric = atoi(metric);
   }

   return ret;

}  /* End of fillStaticRoute6Cfg() */

CmsRet dalStaticRoute6_deleteEntry_igd(const char* addr)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPv6L3ForwardingEntryObject *routeCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   /* deleting a static route entry */
   cmsLog_debug("Deleting an ipv6 static route with %s", addr);

   while ((ret = cmsObj_getNext(MDMOID_I_PV6_L3_FORWARDING_ENTRY, &iidStack, (void **)&routeCfg)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(addr, routeCfg->destIPv6Address) == 0)
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
      ret = cmsObj_deleteInstance(MDMOID_I_PV6_L3_FORWARDING_ENTRY, &iidStack);
      cmsObj_free((void **) &routeCfg);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete IPv6L3ForwardingEntryObject, ret = %d", ret);
         return ret;
      }
   }

   return ret;

}  /* End of dalStaticRoute6_deleteEntry_igd() */

#endif
#endif  /* DMP_BASELINE_1 */

