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

#ifdef DMP_BASELINE_1  


#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_dal.h"

/* this is how it was done in dal_route.c, which calls the function in
 * rut_route.c to do the get in the IGD data model. Preserve the existing
 * code, even though it would be better to just implement the function here.
 */

extern CmsRet rutRt_getActiveDefaultGateway_igd(char *gwIfName);

CmsRet qdmRt_getActiveDefaultGatewayLocked_igd(char *gwIfName)
{
   return (rutRt_getActiveDefaultGateway_igd(gwIfName));
}


CmsRet qdmRt_getDefaultGatewayIPLocked_igd(char *gwIP)
{
   /* code from cgi_diag.c which takes an ifName of gateway, and find out the IP address */
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_INTERNAL_ERROR;
   char gwIfc[BUFLEN_32]={0};
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslLinkCfgObject *dslLinkCfg = NULL;
   WanIpConnObject *ipConn=NULL;
   WanPppConnObject *pppConn=NULL;
   UBOOL8 found = FALSE;

   cmsLog_debug("Enter");

   if ((ret = qdmRt_getActiveDefaultGatewayLocked(gwIfc)) == CMSRET_SUCCESS)
   {
      if ((strcmp(gwIfc,"&nbsp") != 0) && (gwIfc[0] != '\0'))
      {
         // get the first availabe WAN connection ID
         if (cmsObj_getNext(MDMOID_WAN_DSL_LINK_CFG, &parentIidStack, (void **)&dslLinkCfg) == CMSRET_SUCCESS)
         {
            INIT_INSTANCE_ID_STACK(&iidStack);
            /* get the related ipConn obj */
            while (!found && cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &parentIidStack, &iidStack,
                                                     (void **)&ipConn) == CMSRET_SUCCESS)
            {
               cmsLog_debug("ipConn->X_BROADCOM_COM_IfName %s, gwIfc %s",ipConn->X_BROADCOM_COM_IfName,gwIfc);
               
               if ((cmsUtl_strcmp(ipConn->X_BROADCOM_COM_IfName,gwIfc)) == 0)
               {
                  found = TRUE;
                  if (ipConn->defaultGateway)
                  {
                     strcpy(gwIP,ipConn->defaultGateway);
                  }
               }
               cmsObj_free((void **) &ipConn);
            } /* while ipconn */

            INIT_INSTANCE_ID_STACK(&iidStack);
            while (!found && cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &parentIidStack, &iidStack,
                                                     (void **)&pppConn) == CMSRET_SUCCESS)
            {
               cmsLog_debug("pppConn->X_BROADCOM_COM_IfName %s, gwIfc %s",pppConn->X_BROADCOM_COM_IfName,gwIfc);
               
               if ((cmsUtl_strcmp(pppConn->X_BROADCOM_COM_IfName,gwIfc)) == 0)
               {
                  found = TRUE;
                  if (pppConn->X_BROADCOM_COM_DefaultGateway)
                  {
                     cmsLog_debug("pppConn->X_BROADCOM_COM_DefaultGateway %s",pppConn->X_BROADCOM_COM_DefaultGateway);
                     strcpy(gwIP,pppConn->X_BROADCOM_COM_DefaultGateway);
                  }
               }
               cmsObj_free((void **) &pppConn);
            } /* while pppConn */
            cmsObj_free((void **) &dslLinkCfg);
         } /* dslLinkCfg */

         cmsLog_debug("gwIp %s",gwIP);
      } /* gwifc found */
   }
   return ret;
}


#ifdef DMP_X_BROADCOM_COM_IPV6_1
CmsRet qdmRt_getSysDfltGw6_igd(char *gwIfc, char *gwIpAddr)
{
   InstanceIdStack iidStack;
   IPv6L3ForwardingObject *ipv6Obj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Enter");
   if (gwIfc == NULL && gwIpAddr == NULL)
   {
      cmsLog_error("NULL string.");
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

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_I_PV6_L3_FORWARDING, &iidStack, 0, (void **)&ipv6Obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get MDMOID_I_PV6_L3_FORWARDING, ret=%d", ret);
      return ret;
   }

   if (gwIfc != NULL)
   {
      if (!IS_EMPTY_STRING(ipv6Obj->defaultConnectionService))
      {
         strcpy(gwIfc, ipv6Obj->defaultConnectionService);
      }
   }

   if (gwIpAddr != NULL)
   {
      /* XXX TODO: fill in gwIpAddr  */
   }

   cmsObj_free((void **)&ipv6Obj);
   return ret;        
    
}  /* End of qdmRt_getSysDfltGw6() */
#endif  /* DMP_X_BROADCOM_COM_IPV6_1 */


#endif /* DMP_BASELINE_1  */
