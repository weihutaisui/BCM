/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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

#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal.h"


UBOOL8 dalAutoDetect_isAutoDetectEnabled(void)
{
   UBOOL8 isAutoDetectEnabled = FALSE;   
   CmsRet ret = CMSRET_SUCCESS;
   WanCommonIntfCfgObject *wanCommIntf = NULL;
   UINT32 flags = OGF_NO_VALUE_UPDATE;
   UBOOL8 found = FALSE;   
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   InstanceIdStack savedIidStack;   

   while (!found &&
          ((ret = cmsObj_getNextFlags(MDMOID_WAN_COMMON_INTF_CFG, &iidStack, flags, (void **) &wanCommIntf)) == CMSRET_SUCCESS))
   {

      savedIidStack = iidStack;

#ifdef DMP_ETHERNETWAN_1   
      if (!cmsUtl_strcmp(wanCommIntf->WANAccessType, MDMVS_ETHERNET))
      {
         WanEthIntfObject *wanEthIntfObj =NULL;    
         if ((ret = cmsObj_get(MDMOID_WAN_ETH_INTF, &iidStack, 0, (void **) &wanEthIntfObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not get ethIntfCfg object, ret=%d", ret);
         }      
         else
         {  
            isAutoDetectEnabled = wanEthIntfObj->X_BROADCOM_COM_LimitedConnections;
            found = TRUE;
            cmsObj_free((void **) &wanEthIntfObj);
         }
      }
#endif /* DMP_ETHERNETWAN_1 */

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1      
      if (!cmsUtl_strcmp(wanCommIntf->WANAccessType, MDMVS_X_BROADCOM_COM_MOCA))
      {
         WanMocaIntfObject *wanMocaIntfObj = NULL;         
         if ((ret = cmsObj_get(MDMOID_WAN_MOCA_INTF, &iidStack, 0, (void **) &wanMocaIntfObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not get wanMocaIntfObj object, ret=%d", ret);
         }      
         else
         {  
            isAutoDetectEnabled =wanMocaIntfObj->limitedConnections;
            found = TRUE;
            cmsObj_free((void **) &wanMocaIntfObj);            
         }
      }
#endif /* #ifdef DMP_X_BROADCOM_COM_MOCAWAN_1 */

/* if xDSL need auto detection, add code here */
#ifdef DMP_ADSLWAN_1
#ifdef DMP_PTMWAN_1
#endif
#endif

      cmsObj_free((void **) &wanCommIntf);
      iidStack = savedIidStack;
   }

   cmsLog_debug("isAutoDetectEnabled = %d", isAutoDetectEnabled);

   return isAutoDetectEnabled;
   

}


CmsRet dalAutoDetect_enableAllWanConn(void)
{
   CmsRet ret=CMSRET_SUCCESS;
   WanDevObject *wanDev=NULL;
   InstanceIdStack wanDevIid=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *pppCon=NULL;   
   WanIpConnObject *ipCon=NULL;
   UBOOL8 done=FALSE;
   
   while (cmsObj_getNextFlags(MDMOID_WAN_DEV, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&wanDev) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **)&wanDev);  /* no longer needed */
      /* get the related ipCon obj */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (!done &&      
         cmsObj_getNextInSubTreeFlags(MDMOID_WAN_IP_CONN, &wanDevIid, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ipCon) == CMSRET_SUCCESS)      
      {
         /* enable the IpWanConn object if it is disabled */    
         if (!ipCon->enable)
         {
            ipCon->enable = TRUE;
            if ((ret = cmsObj_set(ipCon, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to set ipCon, ret = %d", ret);
               done = TRUE;
            }   
            else
            {
               cmsLog_debug("done setting ipConn enable");
            }
         }
         else
         {
            cmsLog_debug("IpCon is already enabled. Do nothing");
         }
         cmsObj_free((void **)&ipCon);
      }
   
      /* get the related pppCon obj */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (!done &&
         cmsObj_getNextInSubTreeFlags(MDMOID_WAN_PPP_CONN, &wanDevIid, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&pppCon) == CMSRET_SUCCESS)
      {
         /* enable the PppWanConn object if it is disabled */    
         if (!pppCon->enable)
         {
            pppCon->enable = TRUE;
            if ((ret = cmsObj_set(pppCon, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to set pppCon, ret = %d", ret);
               done = TRUE;
            }   
            else
            {
               cmsLog_debug("done setting pppCon enable");
            }
         }
         else
         {
            cmsLog_debug("pppCon is already enabled. Do nothing");
         }

         cmsObj_free((void **)&pppCon);
      }
   }

   return ret;
   
}


CmsRet dalAutoDetect_setAutoDetectionFlag(UBOOL8 isEnabled)
{
   CmsRet ret=CMSRET_SUCCESS;

   cmsLog_debug("isEnabled = %d", isEnabled);
   
#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   {
      WanMocaIntfObject *wanMocaIntf=NULL;
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      
      if (dalWan_getWanMocaObject(&iidStack, &wanMocaIntf) == CMSRET_SUCCESS)
      {
         wanMocaIntf->limitedConnections = isEnabled;
         if ((ret = cmsObj_set(wanMocaIntf, &iidStack)) != CMSRET_SUCCESS)
         {
           cmsLog_error("Failed to set wanMocaIntf, ret = %d", ret);
         }
         else
         {
           cmsLog_debug("done set auto to %d", isEnabled);
         }
         cmsObj_free((void **) &wanMocaIntf);
      }
      else
      {
         cmsLog_error("Failed to get wanMocaIntf, ret = %d", ret);
      }
   }
#endif


#ifdef DMP_ETHERNETWAN_1
   {
      WanEthIntfObject *wanEthIntfObj=NULL;
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
         
      if (dalWan_getWanEthObject(&iidStack, &wanEthIntfObj) == CMSRET_SUCCESS)
      {
         wanEthIntfObj->X_BROADCOM_COM_LimitedConnections = isEnabled;
         if ((ret = cmsObj_set(wanEthIntfObj, &iidStack)) != CMSRET_SUCCESS)
         {
           cmsLog_error("Failed to set wanEthIntfObj, ret = %d", ret);
         }
         else
         {
           cmsLog_debug("done set auto to %d", isEnabled);
         }
         cmsObj_free((void **) &wanEthIntfObj);
      }
      else
      {
         cmsLog_error("Failed to get wanEthIntfObj, ret = %d", ret);
      }
   }   
#endif

   return ret;
}


#endif /* DMP_X_BROADCOM_COM_AUTODETECTION_1 */

