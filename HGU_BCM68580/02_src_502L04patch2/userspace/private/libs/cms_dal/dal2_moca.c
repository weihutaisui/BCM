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

#ifdef DMP_DEVICE2_MOCA_1

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal.h"
#include "devctl_moca.h"


CmsRet dalMoca_getMocaObjByIntfName_dev2(const char *intfName,
                                         InstanceIdStack *iidStack, void **obj)
{
   CmsRet ret;

   cmsLog_debug("looking for %s", intfName);

   INIT_INSTANCE_ID_STACK(iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_MOCA_INTERFACE, iidStack, obj))  == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(((Dev2MocaInterfaceObject *) (*obj))->name, intfName))
      {
         /* break out of loop and don't free obj */
         cmsLog_debug("Found mocaobj %s", intfName);
         break;
      }
      cmsObj_free(obj);
   }

   /* if we found the intf, ret will be still set to CMSRET_SUCCESS when we
    * break out of the loop*/
   return ret;
}


UBOOL8 dalMoca_getWanMocaIntfByIfName_dev2(const char *intfName,
                                           InstanceIdStack *iidStack,
                                           void **obj)
{
   Dev2MocaInterfaceObject *mocaObj=NULL;
   CmsRet ret;
   UBOOL8 found=FALSE;

   INIT_INSTANCE_ID_STACK(iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_MOCA_INTERFACE, iidStack, (void **) &mocaObj))  == CMSRET_SUCCESS)
   {
      if (mocaObj->upstream)
      {
         /*
          * Consider this a match if:
          * (a) no intfName was provided (NULL), or
          * (b) intfName was provided and it matches
          */
         if ((intfName == NULL) ||
             !cmsUtl_strcmp(mocaObj->name, intfName))
         {
            cmsLog_debug("Found mocaobj %s", mocaObj->name);
            found = TRUE;

            if (obj)
            {
               /* return obj to caller, don't free */
               *obj = mocaObj;
            }
            else
            {
               /* caller only wanted to know if object was there, or only
                * wanted the iidStack.  Free obj.
                */
               cmsObj_free((void **) &mocaObj);
            }

            break;
         }
      }
      cmsObj_free((void **) &mocaObj);
   }

   return found;
}


UBOOL8 dalMoca_getLanMocaIntfByIfName_dev2(const char *intfName,
                                         InstanceIdStack *iidStack,
                                         void **obj)
{
   Dev2MocaInterfaceObject *mocaObj=NULL;
   CmsRet ret;
   UBOOL8 found=FALSE;

   INIT_INSTANCE_ID_STACK(iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_MOCA_INTERFACE, iidStack, (void **) &mocaObj))  == CMSRET_SUCCESS)
   {
      if (mocaObj->upstream == 0)
      {
         /*
          * Consider this a match if:
          * (a) no intfName was provided (NULL), or
          * (b) intfName was provided and it matches
          */
         if ((intfName == NULL) ||
             !cmsUtl_strcmp(mocaObj->name, intfName))
         {
            cmsLog_debug("Found mocaobj %s", mocaObj->name);
            found = TRUE;

            if (obj)
            {
               /* return obj to caller, don't free */
               *obj = mocaObj;
            }
            else
            {
               /* caller only wanted to know if object was there, or only
                * wanted the iidStack.  Free obj.
                */
               cmsObj_free((void **) &mocaObj);
            }

            break;
         }
      }
      cmsObj_free((void **) &mocaObj);
   }

   return found;
}


void dalMoca_getIntfNameList_dev2(char *ifNameListBuf, UINT32 bufLen)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2MocaInterfaceObject *mocaObj = NULL;


   while (CMSRET_SUCCESS == cmsObj_getNextFlags(MDMOID_DEV2_MOCA_INTERFACE,
                                            &iidStack, OGF_NO_VALUE_UPDATE,
                                            (void **)&mocaObj))
   {
      if (cmsUtl_strlen(ifNameListBuf) > 0)
      {
         cmsUtl_strncat(ifNameListBuf, bufLen, ",");
      }
      cmsUtl_strncat(ifNameListBuf, bufLen, mocaObj->name);
      cmsObj_free((void **)&mocaObj);
   }

   return;
}


CmsRet cmsDal_getAvailableL2MocaIntf_dev2(NameList **ifList)
{
   Dev2MocaInterfaceObject *mocaObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   NameList          head;
   NameList          *tail, *nameListElement;

   *ifList = NULL;
   head.name = NULL;
   head.next = NULL;
   tail = &head;

   while (CMSRET_SUCCESS == cmsObj_getNextFlags(MDMOID_DEV2_MOCA_INTERFACE,
                                            &iidStack, OGF_NO_VALUE_UPDATE,
                                            (void **)&mocaObj))
   {
      /* only add LAN side moca intfs to the list */
      if (!mocaObj->upstream)
      {
         nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
         if (nameListElement == NULL)
         {
            cmsLog_error("could not allocate nameListElement");
            cmsObj_free((void **)&mocaObj);
            cmsDal_freeNameList(head.next);
            return CMSRET_RESOURCE_EXCEEDED;
         }

         /* append to name list */
         cmsLog_debug("adding %s to list", mocaObj->name);
         nameListElement->name = cmsMem_strdup(mocaObj->name);
         nameListElement->next = NULL;
         tail->next = nameListElement;
         tail = nameListElement;
      }

      cmsObj_free((void **)&mocaObj);
   }

   /* return the name list */
   *ifList = head.next;

   return CMSRET_SUCCESS;
}


static void dalMoca_copyInitParms_dev2(Dev2MocaInterfaceObject *mocaObj,
                                       UINT64 initMask,
                                       UBOOL8 autoNwSearch, UBOOL8 privacy,
                                       UINT32 lastOperationalFrequency,
                                       const char *password,
                                       const char *initParmsString)
{
   cmsLog_debug("entered");

   if (initMask & MoCA_INIT_PARAM_AUTO_NETWORK_SEARCH_EN_MASK)
      mocaObj->X_BROADCOM_COM_AutoNwSearch = autoNwSearch;

   if (initMask & MoCA_INIT_PARAM_PRIVACY_MASK)
   {
      if (mocaObj->privacyEnabled != privacy)
      {
         cmsLog_error("TR181 does not allow setting of Privacy, ignored");
      }
      // mocaObj->privacyEnabled = privacy;
   }

   if (initMask & MoCA_INIT_PARAM_NV_PARAMS_LOF_MASK)
   {
      if (mocaObj->lastOperFreq != lastOperationalFrequency)
      {
         cmsLog_error("TR181 does not allow setting of lastOperFreq, ignored");
      }
      // mocaObj->lastOperFreq = lastOperationalFrequency;
   }

   if ((initMask & MoCA_INIT_PARAM_PASSWORD_SIZE_MASK) ||
       (initMask & MoCA_INIT_PARAM_PASSWORD_MASK)) {
       if (cmsUtl_strlen(mocaObj->keyPassphrase ) <= MoCA_MAX_PASSWORD_LEN) {
         CMSMEM_REPLACE_STRING(mocaObj->keyPassphrase, password);
       }
   }

   CMSMEM_REPLACE_STRING(mocaObj->X_BROADCOM_COM_InitParmsString, initParmsString);
}


CmsRet dalMoca_start_dev2(const char *ifName, UINT64 initMask,
                          UBOOL8 autoNwSearch, UBOOL8 privacy,
                          UINT32 lastOperationalFrequency,
                          const char *password, const char *initParmsString)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2MocaInterfaceObject *mocaObj=NULL;

   cmsLog_debug("entered ifName '%s'", ifName);
   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = dalMoca_getMocaObjByIntfName(ifName, &iidStack, (void **)&mocaObj );

   if (ret == CMSRET_SUCCESS) {

      dalMoca_copyInitParms_dev2(mocaObj, initMask,
                                 autoNwSearch, privacy,
                                 lastOperationalFrequency,
                                 password, initParmsString);

      CMSMEM_REPLACE_STRING(mocaObj->X_BROADCOM_COM_MocaControl, MDMVS_START);

      ret = cmsObj_set( mocaObj, &iidStack );

      cmsObj_free( (void **)&mocaObj );
   }
   else {
      cmsLog_error("moca interface '%s' not found", ifName);
   }

   return (ret);
}

CmsRet dalMoca_stop_dev2(const char *ifName)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2MocaInterfaceObject *mocaObj=NULL;

   cmsLog_debug("entered ifName '%s'", ifName);
   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = dalMoca_getMocaObjByIntfName( ifName, &iidStack, (void **)&mocaObj );

   if (ret == CMSRET_SUCCESS) {

      CMSMEM_REPLACE_STRING(mocaObj->X_BROADCOM_COM_MocaControl, MDMVS_STOP);

      ret = cmsObj_set( mocaObj, &iidStack );

      cmsObj_free( (void **)&mocaObj );
   }
   else {
      cmsLog_error("moca interface '%s' not found", ifName);
   }

   return (ret);
}


CmsRet dalMoca_restart_dev2(const char *ifName, UINT64 reinitMask,
                            UBOOL8 autoNwSearch, UBOOL8 privacy,
                            UINT32 lastOperationalFrequency,
                            const char *password, const char *initParmsString)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2MocaInterfaceObject *mocaObj=NULL;

   cmsLog_debug("entered ifName '%s'", ifName);
   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = dalMoca_getMocaObjByIntfName( ifName, &iidStack, (void **)&mocaObj );

   if (ret == CMSRET_SUCCESS) {
      dalMoca_copyInitParms_dev2(mocaObj, reinitMask,
                                 autoNwSearch, privacy,
                                 lastOperationalFrequency,
                                 password, initParmsString);

      CMSMEM_REPLACE_STRING(mocaObj->X_BROADCOM_COM_MocaControl, MDMVS_RESTART);

      ret = cmsObj_set( mocaObj, &iidStack );
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of mocaObj failed, ret=%d", ret);
      }

      cmsObj_free( (void **)&mocaObj );
   }
   else {
      cmsLog_error("moca interface '%s' not found", ifName);
   }

   return (ret);
}


CmsRet dalMoca_getCurrentCfg_dev2(WEB_NTWK_VAR * webData )
{
   Dev2MocaInterfaceObject *mocaObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("entered ifName '%s'", webData->mocaIfName);

   if ((ret = dalMoca_getMocaObjByIntfName(webData->mocaIfName, &iidStack,
                                      (void**) &mocaObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("unable to get MoCA obj '%s', ret=%d", webData->mocaIfName, ret);
   }
   else
   {
      webData->enblMocaPrivacy = mocaObj->privacyEnabled;
      webData->mocaPassword[0] = '\0';
      cmsUtl_strncpy(webData->mocaPassword, mocaObj->keyPassphrase, sizeof(webData->mocaPassword) );
      webData->enblMocaAutoScan = mocaObj->X_BROADCOM_COM_AutoNwSearch;
      webData->mocaFrequency = mocaObj->lastOperFreq;

      cmsLog_debug("enblMocaPrivacy = %d  passwd = %s (%u) autoScan %u  lof %u",
         webData->enblMocaPrivacy, webData->mocaPassword, sizeof(webData->mocaPassword),
         webData->enblMocaAutoScan, webData->mocaFrequency);
      cmsObj_free((void **) &mocaObj);
   }

   return ret;
}


CmsRet dalMoca_getWanIntfInfo_dev2(char *intfName, char *connMode)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2MocaInterfaceObject *mocaObj = NULL;
   UBOOL8 found=FALSE;
   CmsRet ret;

   while (!found &&
          ((ret = cmsObj_getNextFlags(MDMOID_DEV2_MOCA_INTERFACE,
                                      &iidStack, OGF_NO_VALUE_UPDATE,
                                      (void **)&mocaObj)) == CMSRET_SUCCESS))
   {
      if (mocaObj->upstream)
      {
         found = TRUE;

         sprintf(intfName, "%s", mocaObj->name);
         /* for now, all TR181 WAN connections are vlan mux mode */
         sprintf(connMode, "%s", MDMVS_VLANMUXMODE);
      }
      cmsObj_free((void **)&mocaObj);
   }

   return ret;
}


void dalMoca_setVersionString_dev2(const char *intfName, char *versionBuf, UINT32 bufLen)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2MocaInterfaceObject *mocaObj= NULL;
   CmsRet ret;


   ret = dalMoca_getMocaObjByIntfName(intfName, &iidStack, (void **)&mocaObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_debug("Could not find moca intf %s, maybe no Moca devices?");
      snprintf(versionBuf, bufLen, "Not Found");
      return;
   }

   /*
    * The TR98 code treats SW version and Driver version as different
    * things, but they seem to come from the same place in the driver.
    */
   snprintf(versionBuf, bufLen, "HW=%x SW=%s DRV=%s SELF=0x%x NET=%s",
           mocaObj->X_BROADCOM_COM_HwVersion,
           mocaObj->firmwareVersion,
           mocaObj->firmwareVersion,
           mocaObj->X_BROADCOM_COM_SelfMoCAVersion,
           mocaObj->currentVersion);

   cmsObj_free((void **)&mocaObj);

   return;
}


CmsRet dalMoca_addMocaInterface_dev2(const WEB_NTWK_VAR *webVar)
{
   cmsLog_debug("Enter: ifName=%s", webVar->wanL2IfName);
   cmsLog_error("moving Moca intf from LAN to WAN not supported yet");

   return CMSRET_INTERNAL_ERROR;
}


CmsRet dalMoca_deleteMocaInterface_dev2(const WEB_NTWK_VAR *webVar)
{
   cmsLog_debug("Enter: ifName=%s", webVar->wanL2IfName);
   cmsLog_error("moving Moca intf from WAN to LAN not supported yet");

   return CMSRET_INTERNAL_ERROR;
}



#endif /* DMP_DEVICE2_MOCA_1 */




