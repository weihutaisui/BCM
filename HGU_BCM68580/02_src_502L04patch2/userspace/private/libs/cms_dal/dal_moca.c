/***********************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal.h"
#include "devctl_moca.h"




#ifdef DMP_X_BROADCOM_COM_MOCALAN_1
extern CmsRet rutLan_getMocaInterface(const char *ifName, InstanceIdStack *iidStack, LanMocaIntfObject **lanMocaObj);

UBOOL8 dalMoca_getLanMocaIntfByIfName_igd(const char *ifName, InstanceIdStack *iidStack, void **obj)
{
   CmsRet ret;

   ret = rutLan_getMocaInterface(ifName, iidStack, (LanMocaIntfObject **) obj);

   /* convert CmsRet to UBOOL8 */
   return (ret == CMSRET_SUCCESS);
}
#endif


#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
extern UBOOL8 rutMoca_getWanMocaIntfByIfName(const char *ifName, InstanceIdStack *iidStack, WanMocaIntfObject **mocaIntfCfg);

UBOOL8 dalMoca_getWanMocaIntfByIfName_igd(const char *ifName, InstanceIdStack *iidStack, void **obj)
{
   return (rutMoca_getWanMocaIntfByIfName(ifName, iidStack, (WanMocaIntfObject **) obj));
}

CmsRet dalMoca_addMocaInterface_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 existingMocaWanFound;
   InterfaceControlObject *ifcObj = NULL;
   WanMocaIntfObject *wanMocaObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("wanL2IfName=%s", webVar->wanL2IfName);
   
   existingMocaWanFound = dalMoca_getWanMocaIntfByIfName((char *)webVar->wanL2IfName, &iidStack, NULL);
   if (existingMocaWanFound)
   {
      cmsLog_error("A Moca interface is already in use as WAN Interface");
      return CMSRET_INTERNAL_ERROR;
   }


   /*
    * Use the Interface Control object to move the eth interface from LAN side
    * to WAN side.
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_INTERFACE_CONTROL, &iidStack, 0, (void **) &ifcObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get InterfaceControlObject, ret=%d", ret);
      return ret;
   }
   
   CMSMEM_REPLACE_STRING(ifcObj->ifName, (char *)webVar->wanL2IfName);
   ifcObj->moveToLANSide = FALSE;
   ifcObj->moveToWANSide = TRUE;

   /* set InterfaceControlObject */
   if ((ret = cmsObj_set(ifcObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set InterfaceControlObject, ret = %d", ret);
      cmsObj_free((void **) &ifcObj);
      return ret;
   }		 

   cmsObj_free((void **) &ifcObj);

   /*
    * Find the WANDevice that our eth interface was created under.
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   existingMocaWanFound = dalMoca_getWanMocaIntfByIfName((char *) webVar->wanL2IfName, &iidStack, (void **)&wanMocaObj);
   if (!existingMocaWanFound)
   {
      cmsLog_error("could not find the mocaWan object!");
      return CMSRET_INTERNAL_ERROR;
   }

   cmsLog_debug("WAN %s created at %s (wanMocaObj=%p)", (char *) webVar->wanL2IfName, cmsMdm_dumpIidStack(&iidStack), wanMocaObj);


   /* set connection mode value */
   if (webVar->connMode == CMS_CONNECTION_MODE_VLANMUX)
   {
      CMSMEM_REPLACE_STRING(wanMocaObj->connectionMode, MDMVS_VLANMUXMODE);
   }
   else
   {
      CMSMEM_REPLACE_STRING(wanMocaObj->connectionMode, MDMVS_DEFAULTMODE);
   }

   wanMocaObj->enable = TRUE;
              
   if ((ret = cmsObj_set(wanMocaObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set WanMocaIntfObject, ret = %d", ret);
   }
   cmsObj_free((void **) &wanMocaObj);


   /*
    * Also create a single WANConnectionDevice in this WANDevice.
    */
   cmsLog_debug("create a single WANConnectionDevice in this MocaWAN device");
   if ((ret = cmsObj_addInstance(MDMOID_WAN_CONN_DEVICE, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new WanConnectionDevice, ret=%d", ret);
      return ret;
   }


   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}


CmsRet dalMoca_deleteMocaInterface_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack wanConnDevdStack = EMPTY_INSTANCE_ID_STACK;
   WanConnDeviceObject *wanConn=NULL;
   InterfaceControlObject *ifcObj = NULL;
   LanMocaIntfObject *lanMocaObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found;
   
   cmsLog_debug("Deleting %s", webVar->wanL2IfName);

   if (dalMoca_getWanMocaIntfByIfName((char *)webVar->wanL2IfName, &iidStack, NULL) == FALSE)
   {
      cmsLog_debug("Interface %s not found.", webVar->wanL2IfName);
      return CMSRET_INTERNAL_ERROR;
   }

   /*
    * Finally, check if there are any services left on the WanConnectionDevice.
    * If not, delete the entire WanConnectionDevice sub-tree, which includes
    * the DslLinkCfg instance.
    */
   if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &iidStack, &wanConnDevdStack, (void **) &wanConn)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get WanConnDev, ret=%d", ret);
      return ret;
   }
   
   /* we don't need to del  obj MDMOID_WAN_CONN_DEVICE here,
    * it be deleted in rcl_interfaceControlObject when eth move to lan.
    */
   if (wanConn->WANIPConnectionNumberOfEntries != 0 ||
       wanConn->WANPPPConnectionNumberOfEntries != 0)
   {
      cmsLog_debug("WanConnDevice is still in use and cannot be removed");
      cmsObj_free((void **) &wanConn);
      return CMSRET_REQUEST_DENIED;
   }

   cmsObj_free((void **) &wanConn);

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_INTERFACE_CONTROL, &iidStack, 0, (void **) &ifcObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get InterfaceControlObject, ret=%d", ret);
      return ret;
   }
   
   CMSMEM_REPLACE_STRING(ifcObj->ifName, webVar->wanL2IfName);
   ifcObj->moveToLANSide = TRUE;
   ifcObj->moveToWANSide = FALSE;

   /* set InterfaceControlObject */
   if ((ret = cmsObj_set(ifcObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set InterfaceControlObject, ret = %d", ret);
      cmsObj_free((void **) &ifcObj);
      return ret;
   }		 

   cmsObj_free((void **) &ifcObj);

   /*
    * To be symetrical with the move to WAN side case, I also need to enable the
    * moca interface that was just moved back to the LAN side.
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   found = dalMoca_getLanMocaIntfByIfName(webVar->wanL2IfName, &iidStack, (void **)&lanMocaObj);
   if (!found)
   {
      cmsLog_error("could not get the LAN eth obj that was moved back");
   }
   else
   {
      lanMocaObj->enable = TRUE;
      cmsObj_set(lanMocaObj, &iidStack);
      cmsObj_free((void **) &lanMocaObj);
   }

   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}


CmsRet dalMoca_getWanIntfInfo_igd(char *intfName, char *connMode)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanMocaIntfObject *mocaIntf = NULL;
   CmsRet ret;

   intfName[0] = '\0';

    if ((ret = dalWan_getWanMocaObject(&iidStack, &mocaIntf)) != CMSRET_SUCCESS)
    {
       return ret;
    }

    if (!IS_EMPTY_STRING(mocaIntf->ifName))
    {
       sprintf(intfName, "%s", mocaIntf->ifName);
    }
    sprintf(connMode, "%s", mocaIntf->connectionMode);

    cmsObj_free((void **) &mocaIntf);

    return ret;
}

#endif /* DMP_X_BROADCOM_COM_MOCAWAN_1 */


#if defined(DMP_X_BROADCOM_COM_MOCALAN_1) || defined(DMP_X_BROADCOM_COM_MOCAWAN_1)


CmsRet dalMoca_getMocaObjByIntfName_igd(const char *ifName,
                                        InstanceIdStack *iidStack, void **obj)
{
   UBOOL8 found;
   
   found = dalMoca_getLanMocaIntfByIfName( ifName, iidStack, obj );
   if (!found)
   {
      cmsLog_debug("could not get the LAN MoCA obj");
#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
      found = dalMoca_getWanMocaIntfByIfName( ifName, iidStack, obj );
#endif         
   }

   return (found ? CMSRET_SUCCESS : CMSRET_OBJECT_NOT_FOUND);
}


void dalMoca_getIntfNameList_igd(char *ifNameListBuf, UINT32 bufLen)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanMocaIntfObject *pMocaObj = NULL;

   while (CMSRET_SUCCESS == cmsObj_getNextFlags(MDMOID_LAN_MOCA_INTF, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&pMocaObj))
   {
      if (cmsUtl_strlen(ifNameListBuf) > 0)
      {
         cmsUtl_strncat(ifNameListBuf, bufLen, ",");
      }
      cmsUtl_strncat(ifNameListBuf, bufLen, pMocaObj->ifName);
      cmsObj_free((void **)&pMocaObj);
   }

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   {
      WanMocaIntfObject *mocaIntf = NULL;

      INIT_INSTANCE_ID_STACK(&iidStack);
      if ((dalWan_getWanMocaObject(&iidStack, &mocaIntf)) == CMSRET_SUCCESS)
      {
         /* Only add to the list if the interface name is not NULL */
         if (mocaIntf->ifName != NULL)
         {
            if (cmsUtl_strlen(ifNameListBuf) > 0)
            {
               cmsUtl_strncat(ifNameListBuf, bufLen, ",");
            }
            cmsUtl_strncat(ifNameListBuf, bufLen, mocaIntf->ifName);
         }
         cmsObj_free((void **)&mocaIntf);
      }
   }
#endif

   return;
}


static void dalMoca_copyInitParms_igd(LanMocaIntfObject * prevObj, UINT64 initMask,
                                      UBOOL8 autoNwSearch, UBOOL8 privacy,
                                      UINT32 lastOperationalFrequency,
                                      const char *password,
                                      const char *initParmsString)
{
   cmsLog_debug("entered");
   if (initMask & MoCA_INIT_PARAM_AUTO_NETWORK_SEARCH_EN_MASK)
      prevObj->autoNwSearch = autoNwSearch;
   if (initMask & MoCA_INIT_PARAM_PRIVACY_MASK)
      prevObj->privacy = privacy;
   if (initMask & MoCA_INIT_PARAM_NV_PARAMS_LOF_MASK)
      prevObj->lastOperationalFrequency = lastOperationalFrequency;
   if ((initMask & MoCA_INIT_PARAM_PASSWORD_SIZE_MASK) ||
       (initMask & MoCA_INIT_PARAM_PASSWORD_MASK)) {
       if (cmsUtl_strlen( password ) <= MoCA_MAX_PASSWORD_LEN) {
         CMSMEM_REPLACE_STRING( prevObj->password, password);
       }
   }

   CMSMEM_REPLACE_STRING( prevObj->initParmsString, initParmsString);
}


CmsRet dalMoca_start_igd(const char *ifName, UINT64 initMask,
                         UBOOL8 autoNwSearch, UBOOL8 privacy,
                         UINT32 lastOperationalFrequency,
                         const char *password, const char *initParmsString)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanMocaIntfObject *lanMocaObj=NULL;

   cmsLog_debug("entered ifName '%s'", ifName);
   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = dalMoca_getMocaObjByIntfName( ifName, &iidStack, (void **)&lanMocaObj );

   if (ret == CMSRET_SUCCESS) {

      dalMoca_copyInitParms_igd(lanMocaObj, initMask,
                                autoNwSearch, privacy,
                                lastOperationalFrequency,
                                password, initParmsString);

      CMSMEM_REPLACE_STRING(lanMocaObj->mocaControl, MDMVS_START);

      ret = cmsObj_set( lanMocaObj, &iidStack );

      cmsObj_free( (void **)&lanMocaObj );
   }
   else {
      cmsLog_error("moca interface '%s' not found", ifName);
   }
      
   return (ret);
   
}

CmsRet dalMoca_stop_igd(const char *ifName)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanMocaIntfObject *lanMocaObj=NULL;

   cmsLog_debug("entered ifName '%s'", ifName);
   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = dalMoca_getMocaObjByIntfName( ifName, &iidStack, (void **)&lanMocaObj );

   if (ret == CMSRET_SUCCESS) {
         
      CMSMEM_REPLACE_STRING(lanMocaObj->mocaControl, MDMVS_STOP);

      ret = cmsObj_set( lanMocaObj, &iidStack );

      cmsObj_free( (void **)&lanMocaObj );
   }
   else {
      cmsLog_error("moca interface '%s' not found", ifName);
   }
      
   return (ret);
   
}


CmsRet dalMoca_restart_igd(const char *ifName, UINT64 reinitMask,
                           UBOOL8 autoNwSearch, UBOOL8 privacy,
                           UINT32 lastOperationalFrequency,
                           const char *password, const char *initParmsString)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanMocaIntfObject *lanMocaObj=NULL;

   cmsLog_debug("entered ifName '%s'", ifName);
   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = dalMoca_getMocaObjByIntfName( ifName, &iidStack, (void **)&lanMocaObj );

   if (ret == CMSRET_SUCCESS) {
      dalMoca_copyInitParms_igd(lanMocaObj, reinitMask,
                                autoNwSearch, privacy,
                                lastOperationalFrequency,
                                password, initParmsString);

      CMSMEM_REPLACE_STRING(lanMocaObj->mocaControl, MDMVS_RESTART);

      ret = cmsObj_set( lanMocaObj, &iidStack );

      cmsObj_free( (void **)&lanMocaObj );
   }
   else {
      cmsLog_error("moca interface '%s' not found", ifName);
   }
      
   return (ret);
}


CmsRet dalMoca_enable ( char * ifName )
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanMocaIntfObject *lanMocaObj=NULL;

   cmsLog_debug("entered ifName '%s'", ifName);
   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = dalMoca_getMocaObjByIntfName( ifName, &iidStack, (void **)&lanMocaObj );

   if (ret == CMSRET_SUCCESS) {
      lanMocaObj->enable = TRUE;

      ret = cmsObj_set( lanMocaObj, &iidStack );

      cmsObj_free( (void **)&lanMocaObj );
   }
   else {
      cmsLog_error("moca interface '%s' not found", ifName);
   }
      
   return (ret);
   
}

CmsRet dalMoca_disable ( char * ifName )
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanMocaIntfObject *lanMocaObj=NULL;

   cmsLog_debug("entered ifName '%s'", ifName);
   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = dalMoca_getMocaObjByIntfName( ifName, &iidStack, (void **)&lanMocaObj );

   if (ret == CMSRET_SUCCESS) {
      lanMocaObj->enable = FALSE;

      ret = cmsObj_set( lanMocaObj, &iidStack );

      cmsObj_free( (void **)&lanMocaObj );
   }
   else {
      cmsLog_error("moca interface '%s' not found", ifName);
   }
      
   return (ret);
   
}

CmsRet dalMoca_setConfig (
   char * ifName, 
   LanMocaIntfObject *newObj)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanMocaIntfObject *lanMocaObj=NULL;

   cmsLog_debug("entered ifName '%s'", ifName);
   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = dalMoca_getMocaObjByIntfName( ifName, &iidStack, (void **)&lanMocaObj );

   if (ret == CMSRET_SUCCESS) {
      CMSMEM_REPLACE_STRING_FLAGS( lanMocaObj->configParmsString, newObj->configParmsString, ALLOC_ZEROIZE );            

      ret = cmsObj_set( lanMocaObj, &iidStack );

      cmsObj_free( (void **)&lanMocaObj );
   }
   else {
      cmsLog_error("moca interface '%s' not found", ifName);
   }
      
   return (ret);
   
}

CmsRet dalMoca_setTrace (
   char * ifName, 
   LanMocaIntfObject *newObj)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanMocaIntfObject *lanMocaObj=NULL;

   cmsLog_debug("entered ifName '%s'", ifName);
   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = dalMoca_getMocaObjByIntfName( ifName, &iidStack, (void **)&lanMocaObj );

   if (ret == CMSRET_SUCCESS) {
      CMSMEM_REPLACE_STRING_FLAGS( lanMocaObj->traceParmsString, newObj->traceParmsString, ALLOC_ZEROIZE );            

      ret = cmsObj_set( lanMocaObj, &iidStack );

      cmsObj_free( (void **)&lanMocaObj );
   }
   else {
      cmsLog_error("moca interface '%s' not found", ifName);
   }
      
   return (ret);
   
}

#endif /* TR98 MOCALAN || MOCAWAN */


#ifdef SUPPORT_MOCA

/* this is a data model independent function */
CmsRet dalMoca_setWebParams(const WEB_NTWK_VAR * webData )
{
   CmsRet ret = CMSRET_SUCCESS;
   UINT64 mask = 0;

   cmsLog_debug("entered: ifName '%s'", webData->mocaIfName);
   cmsLog_debug("privacy enable = %u, password = %s",
                 webData->enblMocaPrivacy, webData->mocaPassword);
   cmsLog_debug("autoScan = %u, frequency = %u",
                 webData->enblMocaAutoScan, webData->mocaFrequency);


   mask = (MoCA_INIT_PARAM_PASSWORD_MASK |
           MoCA_INIT_PARAM_PRIVACY_MASK  |
           MoCA_INIT_PARAM_AUTO_NETWORK_SEARCH_EN_MASK |
           MoCA_INIT_PARAM_NV_PARAMS_LOF_MASK );


   ret = dalMoca_restart(webData->mocaIfName, mask,
                         webData->enblMocaAutoScan, webData->enblMocaPrivacy,
                         webData->mocaFrequency, webData->mocaPassword, NULL);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("restart of MoCA failed, ret=%d", ret);
   }

   return ret;
}
#endif  /* SUPPORT_MOCA */


#if defined(DMP_X_BROADCOM_COM_MOCALAN_1) || defined(DMP_X_BROADCOM_COM_MOCAWAN_1)

CmsRet dalMoca_getCurrentCfg_igd(WEB_NTWK_VAR * webData )
{
   LanMocaIntfObject *mocaObj = NULL; 
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("entered ifName '%s'", webData->mocaIfName);

   if ((ret = dalMoca_getMocaObjByIntfName(webData->mocaIfName, &iidStack, (void **)&mocaObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("unable to get MoCA obj '%s', ret=%d", webData->mocaIfName, ret);
   }
   else 
   {
      webData->enblMocaPrivacy = mocaObj->privacy;
      cmsUtl_strncpy(webData->mocaPassword, mocaObj->password, sizeof(webData->mocaPassword) );
      webData->enblMocaAutoScan = mocaObj->autoNwSearch;
      webData->mocaFrequency = mocaObj->lastOperationalFrequency;
      
      cmsLog_debug("enblMocaPrivacy = %d  passwd = %s (%u) autoScan %u  lof %u", 
         webData->enblMocaPrivacy, webData->mocaPassword, sizeof(webData->mocaPassword),
         webData->enblMocaAutoScan, webData->mocaFrequency);
      cmsObj_free((void **) &mocaObj);
   }

   return ret;
}


static void formMocaVersionString(char *s, UINT32 len, UINT32 hw, UINT32 sw, UINT32 self, UINT32 net,
                                  UINT32 drvMaj, UINT32 drvMin, UINT32 drvBuild)
{
   snprintf(s, len, "HW=%x SW=%d.%d.%d DRV=%d.%d.%x SELF=0x%x NET=0x%x",
            hw,
            (sw >> 28),
            ((sw << 4) >> 28),
            ((sw << 8) >> 8),
            drvMaj, drvMin, drvBuild, self, net);
}

void dalMoca_setVersionString_igd(const char *intfName, char *versionBuf, UINT32 bufLen)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   void *obj = NULL;
   CmsRet ret;


   ret = dalMoca_getMocaObjByIntfName(intfName, &iidStack, &obj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_debug("Could not find moca intf %s, maybe no Moca devices?");
      snprintf(versionBuf, bufLen, "Not Found");
      return;
   }

   if (GET_MDM_OBJECT_ID(obj) == MDMOID_LAN_MOCA_INTF)
   {
      LanMocaIntfStatusObject *lanMocaStatus=NULL;

      /* reuse the iidStack to get the LanMocaStatus obj associated with this
       * LanMocaInterface.
       */
      ret = cmsObj_get(MDMOID_LAN_MOCA_INTF_STATUS, &iidStack, 0, (void **)&lanMocaStatus);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get Moca Status obj at %s (ret=%d)",
                      cmsMdm_dumpIidStack(&iidStack), ret);
      }
      else
      {
         formMocaVersionString(versionBuf, bufLen,
                              lanMocaStatus->hwVersion,
                              lanMocaStatus->softwareVersion,
                              lanMocaStatus->selfMoCAVersion,
                              lanMocaStatus->networkVersionNumber,
                              lanMocaStatus->driverMajorVersion,
                              lanMocaStatus->driverMinorVersion,
                              lanMocaStatus->driverBuildVersion);

         cmsObj_free((void **)&lanMocaStatus);
      }
   }
#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   else if (GET_MDM_OBJECT_ID(obj) == MDMOID_WAN_MOCA_INTF)
   {
      WanMocaIntfStatusObject *wanMocaStatus=NULL;

      /* reuse the iidStack to get the WanMocaStatus obj associated with this
       * WanMocaInterface.
       */
      ret = cmsObj_get(MDMOID_WAN_MOCA_INTF_STATUS, &iidStack, 0, (void **)&wanMocaStatus);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get Moca Status obj at %s (ret=%d)",
                      cmsMdm_dumpIidStack(&iidStack), ret);
      }
      else
      {
         formMocaVersionString(versionBuf, bufLen,
                               wanMocaStatus->hwVersion,
                               wanMocaStatus->softwareVersion,
                               wanMocaStatus->selfMoCAVersion,
                               wanMocaStatus->networkVersionNumber,
                               wanMocaStatus->driverMajorVersion,
                               wanMocaStatus->driverMinorVersion,
                               wanMocaStatus->driverBuildVersion);

         cmsObj_free((void **) &wanMocaStatus);
      }
   }
#endif  /* DMP_X_BROADCOM_COM_MOCAWAN_1 */
   else
   {
      cmsLog_error("Unsupported Moca Intf oid %d", GET_MDM_OBJECT_ID(obj));
   }

   cmsObj_free(&obj);

   return;
}


#endif /* MOCALAN || MOCAWAN */

