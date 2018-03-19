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


#ifdef DMP_DEVICE2_BASELINE_1

#ifdef DMP_DEVICE2_ETHERNETINTERFACE_1


#include "cms_core.h"
#include "cms_qdm.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal.h"



CmsRet dalEth_getAvailableL2EthIntf_dev2(NameList **ifList)
{
   Dev2EthernetInterfaceObject *ethIntfObj=NULL;
   InstanceIdStack   iidStack = EMPTY_INSTANCE_ID_STACK;

   cmsLog_debug("Entered:");

   while (cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      /* Two cases for available eth WAN port:
      *  1). It is not LANONLY and upstream is FALSE and it is enable is TRUE 
      *  2). It is WANONLY and upstream is TRUE and enable is FALSE (wan only port is not configured).
      */
      if ((ethIntfObj->enable &&
           ethIntfObj->upstream == FALSE &&
           cmsUtl_strcmp(ethIntfObj->X_BROADCOM_COM_WanLan_Attribute, MDMVS_LANONLY)) ||
          (!ethIntfObj->enable &&
           ethIntfObj->upstream &&
           !cmsUtl_strcmp(ethIntfObj->X_BROADCOM_COM_WanLan_Attribute, MDMVS_WANONLY)))
           
      {
         if (cmsDal_addNameToNameList(ethIntfObj->name, ifList) == NULL)
         {
            cmsDal_freeNameList(*ifList);
            cmsObj_free((void **)&ethIntfObj);
            return CMSRET_RESOURCE_EXCEEDED;
         }
      }

      cmsObj_free((void **)&ethIntfObj);
   }

   return CMSRET_SUCCESS;
}


CmsRet dalEth_getGMACEthIntf_dev2(char *gMACEthIntf)
{
   Dev2EthernetInterfaceObject *ethIntfObj=NULL;
   InstanceIdStack   iidStack = EMPTY_INSTANCE_ID_STACK;

   while (cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      if (ethIntfObj->enable &&
          ethIntfObj->upstream == FALSE &&
          (!cmsUtl_strcmp(ethIntfObj->X_BROADCOM_COM_WanLan_Attribute, MDMVS_WANPREPERRED) ||
           ethIntfObj->X_BROADCOM_COM_GMAC_Enabled))    /* For backward compatabilty and  will be depreciated later on */
      {
         if (cmsUtl_strlen(gMACEthIntf) > 0)
         {
            cmsUtl_strcat(gMACEthIntf, ",");
         }
         cmsUtl_strcat(gMACEthIntf, ethIntfObj->name);
      }
      cmsObj_free((void **)&ethIntfObj);
   }

   cmsLog_debug("gMACEthIntf=%s", gMACEthIntf);

   return CMSRET_SUCCESS;
}


CmsRet cmsDal_getWANOnlyEthIntf_dev2(char *gWanOnlyEthIntf)
{
   Dev2EthernetInterfaceObject *ethIntfObj=NULL;
   InstanceIdStack   iidStack = EMPTY_INSTANCE_ID_STACK;

   while (cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      if ( ethIntfObj->upstream &&
         !cmsUtl_strcmp(ethIntfObj->X_BROADCOM_COM_WanLan_Attribute, MDMVS_WANONLY))
      {
         if (cmsUtl_strlen(gWanOnlyEthIntf) > 0)
         {
            cmsUtl_strcat(gWanOnlyEthIntf, ",");
         }
         cmsUtl_strcat(gWanOnlyEthIntf, ethIntfObj->name);
      }
      cmsObj_free((void **)&ethIntfObj);
   }

   cmsLog_debug("gWanOnlyEthIntf=%s", gWanOnlyEthIntf);

   return CMSRET_SUCCESS;
}




static CmsRet getConfiguredEthWanObj_dev2(InstanceIdStack *iidStack, Dev2EthernetInterfaceObject **ethIntfObj)
{
   while (cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE,
                              iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) ethIntfObj) == CMSRET_SUCCESS)
   {
      /* For a configured WAN layer 2, upstream and enable flag HAS TO be TRUE
      * since when WANONLY eth port is always initialized to FALSE by default
      */
      if ((*ethIntfObj)->upstream && (*ethIntfObj)->enable)
      {
         /* caller is responsible for freeing ethIntfObj */
         return CMSRET_SUCCESS;
      }

      cmsObj_free((void **) ethIntfObj);
   }

   return CMSRET_OBJECT_NOT_FOUND;
}


static CmsRet getEthObjByName_dev2(const char *intfName,
                                   InstanceIdStack *iidStack,
                                   Dev2EthernetInterfaceObject **ethIntfObj)
{
   while (cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE,
                              iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) ethIntfObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp((*ethIntfObj)->name, intfName))
      {
         /* caller is responsible for freeing ethIntfObj */
         return CMSRET_SUCCESS;
      }

      cmsObj_free((void **) ethIntfObj);
   }

   return CMSRET_OBJECT_NOT_FOUND;
}


void cmsDal_releaseAndRelock()
{
   cmsLck_releaseLock();
   usleep(100*1000); // 100ms
   if (cmsLck_acquireLockWithTimeout(5000) != CMSRET_SUCCESS)  // 5 sec
   {
      cmsLog_error("failed to re-acquirelock");
      cmsLck_dumpInfo();
   }
}


CmsRet dalEth_addEthInterface_dev2(const WEB_NTWK_VAR *webVar)
{
   Dev2EthernetInterfaceObject *ethIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   cmsLog_debug("Entered: wanl2IfName=%s", webVar->wanL2IfName);

   /*
    * Make sure ethWan does not already exist.
    */
   if ((ret = getConfiguredEthWanObj_dev2(&iidStack, &ethIntfObj)) == CMSRET_SUCCESS)
   {
      cmsLog_error("Cannot add another ethWan, current ethWan %s", ethIntfObj->name);
      cmsObj_free((void **) &ethIntfObj);
      return CMSRET_INTERNAL_ERROR;
   }


   INIT_INSTANCE_ID_STACK(&iidStack);
   if (getEthObjByName_dev2(webVar->wanL2IfName, &iidStack, &ethIntfObj) != CMSRET_SUCCESS)
   {
      cmsLog_error("%s does not exist!", webVar->wanL2IfName);
      return CMSRET_INVALID_ARGUMENTS;
   }

   if (cmsUtl_strcmp(ethIntfObj->X_BROADCOM_COM_WanLan_Attribute, MDMVS_WANONLY))
   {
      /* For NOT WANONLY eth port, need to move from LAN to WAN
        * by disabling it first
        */
      
      ethIntfObj->enable = FALSE;
      cmsObj_set(ethIntfObj, &iidStack);
      cmsObj_free((void **) &ethIntfObj);

      /* Release lock and sleep to give ssk a chance to process the link down status msg */
      cmsDal_releaseAndRelock();

      /* Remove eth from bridge and set Upstream to TRUE */
      {
         UBOOL8 moveToLan = FALSE;
         UBOOL8 moveToWan = TRUE;

         ret = cmsDal_moveIntfLanWan(webVar->wanL2IfName, moveToLan, moveToWan);
         if (ret != CMSRET_SUCCESS)
         {
            return ret;
         }
      }

      /* Release the lock and sleep to give ssk a chance to process delete port obj msg */
      cmsDal_releaseAndRelock();

      /* Find the eth object again and later re-enable  */
      INIT_INSTANCE_ID_STACK(&iidStack);
      if (getEthObjByName_dev2(webVar->wanL2IfName, &iidStack, &ethIntfObj) != CMSRET_SUCCESS)
      {
         cmsLog_error("%s does not exist!", webVar->wanL2IfName);
         return CMSRET_INVALID_ARGUMENTS;
      }
      
   }

   /* This eth WAN port is configured now by setting enable to TRUE */
   ethIntfObj->enable = TRUE;
   
   ret = cmsObj_set(ethIntfObj, &iidStack);
   cmsObj_free((void **) &ethIntfObj);

   return ret;
}


CmsRet dalEth_deleteEthInterface_dev2(const WEB_NTWK_VAR *webVar)
{
   Dev2EthernetInterfaceObject *ethIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   cmsLog_debug("Entered: wanl2IfName=%s", webVar->wanL2IfName);

   /*
    * Check if there is any ethWAN interface in the first place.
    */
   ret = getConfiguredEthWanObj_dev2(&iidStack, &ethIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("No ethWan found, looking for %s", webVar->wanL2IfName);
      return ret;
   }

   /*
    * No wan services are allowed to exist on top of this layer 2 intf
    * before we move it back to LAN.
    */
   if (qdmIpIntf_getNumberOfWanServicesOnLayer2IntfNameLocked_dev2(webVar->wanL2IfName) != 0)
   {
      cmsLog_error("All WAN services on %s must be removed prior to move", webVar->wanL2IfName);
      cmsObj_free((void **) &ethIntfObj);
      return CMSRET_REQUEST_DENIED;;
   }

   if (cmsUtl_strcmp(ethIntfObj->X_BROADCOM_COM_WanLan_Attribute, MDMVS_WANONLY))
   {
      /* For not WANONLY port, need to move from WAN to LAN
      * by first disabling the interface
      */
      
      ethIntfObj->enable = FALSE;
      cmsObj_set(ethIntfObj, &iidStack);
      cmsObj_free((void **) &ethIntfObj);

      /* Release lock and sleep to give ssk a chance to process the link down status msg */
      cmsDal_releaseAndRelock();


      /* Set upstream to FALSE and add to default LAN side bridge br0 */
      {
         UBOOL8 moveToLan = TRUE;
         UBOOL8 moveToWan = FALSE;

         ret = cmsDal_moveIntfLanWan(webVar->wanL2IfName, moveToLan, moveToWan);
         if (ret != CMSRET_SUCCESS)
         {
            cmsObj_free((void **) &ethIntfObj);
            return ret;
         }
      }

      /* Release the lock and sleep to give ssk a chance to process add port obj msg */
      cmsDal_releaseAndRelock();

   }
   else
   {
      /* For WANONLY, just free it for now
      */
      cmsObj_free((void **) &ethIntfObj);
   }
   
   /* Find the object again and re-enable */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if (getEthObjByName_dev2(webVar->wanL2IfName, &iidStack, &ethIntfObj) != CMSRET_SUCCESS)
   {
      cmsLog_error("%s does not exist!", webVar->wanL2IfName);
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* For WANONLY port, if removed, need to set enable to FALSE to indicate that this WANONLY 
   * enet port is not configured (not in br0 also)
   */
   if (!cmsUtl_strcmp(ethIntfObj->X_BROADCOM_COM_WanLan_Attribute, MDMVS_WANONLY))
   {
      ethIntfObj->enable = FALSE;
   }
   else
   {
      ethIntfObj->enable = TRUE;
   }
   
   ret = cmsObj_set(ethIntfObj, &iidStack);
   cmsObj_free((void **) &ethIntfObj);

   return ret;
}


#endif  /* DMP_DEVICE2_ETHERNETINTERFACE_1 */

#endif  /* DMP_DEVICE2_BASELINE_1 */

