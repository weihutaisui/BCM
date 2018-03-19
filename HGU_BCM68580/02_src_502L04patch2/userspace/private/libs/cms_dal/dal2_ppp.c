/*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
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

#ifdef  DMP_DEVICE2_PPPINTERFACE_1 

UBOOL8 dalWan_isPPPWanConnection_dev2(const WEB_NTWK_VAR *webVar)
{
   UBOOL8 isPPP = FALSE;
   
   if (webVar->ntwkPrtcl == CMS_WAN_TYPE_PPPOE ||
       webVar->ntwkPrtcl == CMS_WAN_TYPE_ETHERNET_PPPOE ||
       webVar->ntwkPrtcl == CMS_WAN_TYPE_PPPOA)
   {
      isPPP = TRUE;
   }

   return isPPP;
}

CmsRet dalPpp_addIntfObject_dev2(const WEB_NTWK_VAR *webVar,  
                                 UBOOL8 supportIpv4,
                                 UBOOL8 supportIpv6,
                                 const char *lowLayerStr,
                                 char *myPathRef,
                                 UINT32 bufLen)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2PppInterfaceObject *pppIntf=NULL;
   char wanL2IfName[CMS_IFNAME_LENGTH];
   char *p;
   char *fullPathStringPtr=NULL;
   MdmPathDescriptor pppPathDesc;
   CmsRet ret;
   
   /* Add/set ppp interface
   */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_PPP_INTERFACE, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add PPP intf Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_PPP_INTERFACE, &iidStack, 0, (void **) &pppIntf)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get pppIntfObj, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_PPP_INTERFACE, &iidStack);
      return ret;
   }


   /* extract layer2 interface name, eg. atm0, ptm0 */
   strncpy(wanL2IfName, webVar->wanL2IfName, sizeof(wanL2IfName)-1);
   p = strchr(wanL2IfName, '/');
   if (p)
   {
      *p = '\0';
   }    

   if ((webVar->ntwkPrtcl == CMS_WAN_TYPE_PPPOE) || (webVar->ntwkPrtcl == CMS_WAN_TYPE_ETHERNET_PPPOE))
   {
      /* This part is only for PPPoE */
      if (cmsUtl_strlen(webVar->pppServerName) > 0 ||
          webVar->pppToBridge ||
          webVar->pppIpExtension)
      {
         Dev2PppInterfacePpoeObject *pppoeObj=NULL;
      
         /* Get/set PPPoE object */
         if ((ret = cmsObj_get(MDMOID_DEV2_PPP_INTERFACE_PPOE, &iidStack, 0, (void **) &pppoeObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to get pppoeObj, ret = %d", ret);
            cmsObj_deleteInstance(MDMOID_DEV2_PPP_INTERFACE, &iidStack);
            cmsObj_free((void **) &pppIntf);    
            return ret;
         }
   
         CMSMEM_REPLACE_STRING(pppoeObj->serviceName, webVar->pppServerName);

         if (webVar->pppToBridge)
         {
            pppoeObj->X_BROADCOM_COM_AddPppToBridge = TRUE;
         }
         
         /* ppp ip extension, if TRUE, no NAT/Firewall */
         if (webVar->pppIpExtension)
         {
            pppoeObj->X_BROADCOM_COM_IPExtension = TRUE;
         }
         
         ret = cmsObj_set(pppoeObj,  &iidStack);
         cmsObj_free((void **) &pppoeObj);    
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set pppoeObj. ret=%d", ret);
            cmsObj_deleteInstance(MDMOID_DEV2_PPP_INTERFACE, &iidStack);
            cmsObj_free((void **) &pppIntf);    
            return ret;      
         } 
         
      }
   }
   else if (webVar->ntwkPrtcl == CMS_WAN_TYPE_PPPOA)
   {
#if 0 /* For later, set something in PPPoA obj, current PPPoA obj in TR181 is an empty object. */
      Dev2PppInterfacePpoaObject *pppoaObj=NULL;      
      
      /* Get/set PPPoE object */
      if ((ret = cmsObj_get(MDMOID_DEV2_PPP_INTERFACE_PPOA, &iidStack, 0, (void **) &pppoaObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get pppoeObj, ret = %d", ret);
         cmsObj_deleteInstance(MDMOID_DEV2_PPP_INTERFACE, &iidStack);
         return ret;
      }

      ret = cmsObj_set(pppoaObj,  &iidStack);
      cmsObj_free((void **) &pppoaObj);    
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to set pppoaObj. ret=%d", ret);
         cmsObj_deleteInstance(MDMOID_DEV2_PPP_INTERFACE, &iidStack);
         return ret;      
      } 
#endif
   }
   
   CMSMEM_REPLACE_STRING(pppIntf->username, webVar->pppUserName);
   CMSMEM_REPLACE_STRING(pppIntf->password, webVar->pppPassword);
   pppIntf->X_BROADCOM_COM_Enable_Debug = webVar->enblPppDebug;
  /* Convert the number to string for auth protocol (method) */
   CMSMEM_REPLACE_STRING(pppIntf->authenticationProtocol, cmsUtl_numToPppAuthString(webVar->pppAuthMethod)); 
   
  /* Get on demand ideltime out in seconds if it is enabled  (enblOnDemand > 0) */
  if (webVar->enblOnDemand)
  {
     pppIntf->idleDisconnectTime = webVar->pppTimeOut * 60;
  }
  else
  {
     /* 0 is no on demand feature */
     pppIntf->idleDisconnectTime = 0;
  }

   /* if static ip is used, save it */
   if (webVar->useStaticIpAddress)
   {
      pppIntf->X_BROADCOM_COM_UseStaticIPAddress = TRUE;
      CMSMEM_REPLACE_STRING(pppIntf->X_BROADCOM_COM_LocalIPAddress , webVar->pppLocalIpAddress);
   }
   else
   {
      CMSMEM_REPLACE_STRING(pppIntf->X_BROADCOM_COM_LocalIPAddress , "0.0.0.0");
   }


   /* Use Ethernet.valtermination as the lowLayerStr passed in */
   CMSMEM_REPLACE_STRING(pppIntf->lowerLayers, lowLayerStr);
   pppIntf->enable = TRUE;

   pppIntf->IPCPEnable = supportIpv4;
   pppIntf->IPv6CPEnable = supportIpv6;

   ret = cmsObj_set(pppIntf,  &iidStack);
   cmsObj_free((void **) &pppIntf);    
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set pppIntf. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_PPP_INTERFACE, &iidStack);
      return ret;      
   } 


   /* Need to fill up myPathRef (pppFullPath) and  pppPathDesc as output  */
   
   INIT_PATH_DESCRIPTOR(&pppPathDesc);
   pppPathDesc.iidStack = iidStack;
   pppPathDesc.oid = MDMOID_DEV2_PPP_INTERFACE;
   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pppPathDesc, &fullPathStringPtr)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_PPP_INTERFACE, &iidStack);
      return ret;
   }
   if (cmsUtl_strlen(fullPathStringPtr) > (SINT32) bufLen)
   {
      ret = CMSRET_INTERNAL_ERROR;
   }
   else
   {
      strncpy(myPathRef, fullPathStringPtr, bufLen);
      ret = CMSRET_SUCCESS;
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPathStringPtr);
   
   cmsLog_debug("Exit. ret %d", ret);
   
   return ret;
   
}


CmsRet dalPpp_deleteIntfObject_dev2(char *pppLowerlayerFullPath)
{                                        
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   char *objPath = NULL;
   Dev2PppInterfaceObject *pppObj=NULL;
   CmsRet ret=CMSRET_INVALID_ARGUMENTS;
   
   if (pppLowerlayerFullPath == NULL)
   {
      cmsLog_error("NULL string.");
      return ret;
   }
   cmsLog_debug("Enter pppLowerlayerFullPath %s.", pppLowerlayerFullPath);

   while(!found && 
         (ret = cmsObj_getNextFlags(MDMOID_DEV2_PPP_INTERFACE,
                                    &iidStack, 
                                    OGF_NO_VALUE_UPDATE,
                                    (void **)&pppObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcasecmp(pppObj->lowerLayers, pppLowerlayerFullPath))
      {
         MdmPathDescriptor pathDesc = EMPTY_PATH_DESCRIPTOR;
            
         pathDesc.oid = MDMOID_DEV2_PPP_INTERFACE;
         pathDesc.iidStack = iidStack;
         if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &objPath)) == CMSRET_SUCCESS)
         {
            /* save the lowerlayer pointer */
            if (objPath != NULL)
            {
               cmsLog_debug("Got ppp intf full path %s and deleting ppp intf obj..", objPath);
               cmsUtl_strncpy(pppLowerlayerFullPath, objPath, MDM_SINGLE_FULLPATH_BUFLEN);
               CMSMEM_FREE_BUF_AND_NULL_PTR(objPath);
            }

            cmsObj_deleteInstance(MDMOID_DEV2_PPP_INTERFACE, &iidStack);
         }
         else
         {
            cmsLog_error("Could not form fullpath to %d %s", pathDesc.oid, cmsMdm_dumpIidStack(&pathDesc.iidStack));
         }

         found = TRUE;
      }

      cmsObj_free((void **)&pppObj);
   }    
   
   cmsLog_debug("Exit, ret=%d", ret);

   return ret;

}


#endif /*  DMP_DEVICE2_PPPINTERFACE_1 */

#endif /* DMP_DEVICE2_BASELINE_1 */

