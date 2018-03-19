
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

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"
#include "dal.h"


CmsRet fillVrtSrvCfg_dev2(const char * ipIntfFullPath, const char *srvName, const char *srvAddr, const char *protocol, const UINT16 EPS, 
                              const UINT16 EPE, const UINT16 IPS, const UINT16 IPE, void * obj)
{
   Dev2NatPortMappingObject *portmappingObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("%s/%s/%s/%s/%u/%u/%u/%u", ipIntfFullPath, srvName, srvAddr, protocol, EPS, EPE, IPS, IPE);
   if (TRUE)
   {
      portmappingObj = (Dev2NatPortMappingObject *) obj;
      portmappingObj->enable= TRUE;

      CMSMEM_REPLACE_STRING(portmappingObj->description, srvName);
      CMSMEM_REPLACE_STRING(portmappingObj->internalClient, srvAddr);
      CMSMEM_REPLACE_STRING(portmappingObj->remoteHost,"");

      if (cmsUtl_strcmp(protocol, "1") == 0)
      {
         CMSMEM_REPLACE_STRING(portmappingObj->protocol, MDMVS_TCP);
      }
      else if (cmsUtl_strcmp(protocol, "2") == 0)
      {
         CMSMEM_REPLACE_STRING(portmappingObj->protocol, MDMVS_UDP);
      }
      else
      { //TR181 doesn't define MDMVS_TCP_OR_UDP ..
         CMSMEM_REPLACE_STRING(portmappingObj->protocol, MDMVS_TCP_OR_UDP);
      }

      CMSMEM_REPLACE_STRING(portmappingObj->interface,ipIntfFullPath);
      if (!cmsUtl_strcmp(portmappingObj->interface,"allintf"))
      {
         CMSMEM_REPLACE_STRING(portmappingObj->interface,"");
      }
      if(!cmsUtl_strcmp(portmappingObj->interface,""))
      {
          portmappingObj->allInterfaces = TRUE;
      }

      portmappingObj->externalPort = EPS;
      portmappingObj->externalPortEndRange= EPE;
      portmappingObj->internalPort = IPS;
      portmappingObj->X_BROADCOM_COM_InternalPortEndRange = IPE;

      if( portmappingObj->externalPort == 0 && portmappingObj->internalPort == 0)
      { //DMZ , set Allinterface to true
         portmappingObj->allInterfaces =TRUE;
      }

      if ((portmappingObj->description == NULL) ||(portmappingObj->internalClient == NULL) ||(portmappingObj->protocol== NULL))
      {
         cmsLog_error("malloc failed.");
         cmsObj_free((void **) &portmappingObj);
         return CMSRET_RESOURCE_EXCEEDED;
      }
   }

   return ret;
}


CmsRet dalVirtualServer_addEntry_app_dev2(const char *dstWanIf, const char *srvName, const char *srvAddr, const char *protocol, 
                                                         const UINT16 EPS, const UINT16 EPE, const UINT16 IPS, const UINT16 IPE,const char *appName)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   Dev2NatPortMappingObject *portmappingObj=NULL;
   char *ipIntfFullPath=NULL;


   /* adding a new virtual server entry */
   cmsLog_debug("Adding new virtual server with %s/%s/%s/%s/%u/%u/%u/%u", dstWanIf, srvName, srvAddr, protocol, EPS, EPE, IPS, IPE);

   /* add new instance */
   if (!cmsUtl_strcmp(dstWanIf,"allintf"))
   {
      CMSMEM_REPLACE_STRING(ipIntfFullPath, dstWanIf);
   }
   else if(cmsUtl_strcmp(dstWanIf,""))
   {
       if (qdmIpIntf_isNatEnabledOnIntfNameLocked(dstWanIf)==TRUE)
       {
          if(qdmIntf_intfnameToFullPathLocked_dev2(dstWanIf,FALSE,&ipIntfFullPath) != CMSRET_SUCCESS)
          {
             return ret;
          }
       }
   }else
   {
      //no specific interface
      CMSMEM_REPLACE_STRING(ipIntfFullPath,"");
   }

   if(ipIntfFullPath)
   {
      if ((ret = cmsObj_addInstance(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not create new virtual server entry, ret=%d", ret);
         CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
         return ret;
      }

      if ((ret = cmsObj_get(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack, 0, (void **) &portmappingObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get Dev2NatPortMappingObject, ret=%d", ret);
         CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
         cmsObj_deleteInstance(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack);
         return ret;
      }

      if ((ret = fillVrtSrvCfg_dev2(ipIntfFullPath, srvName, srvAddr, protocol, EPS, EPE, IPS, IPE, portmappingObj)) != CMSRET_SUCCESS)
      {
         cmsObj_free((void **) &portmappingObj);
         CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
         cmsObj_deleteInstance(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack);
         return ret;
      }

      REPLACE_STRING_IF_NOT_EQUAL(portmappingObj->X_BROADCOM_COM_AppName,appName);

      cmsLog_debug("in portmappingObj, %s/%s/%s/%u/%u/%u/%u", 
      portmappingObj->description, portmappingObj->internalClient, portmappingObj->protocol, portmappingObj->externalPort, 
      portmappingObj->externalPortEndRange, portmappingObj->internalPort, portmappingObj->X_BROADCOM_COM_InternalPortEndRange);

      /* set and activate Object */
      ret = cmsObj_set(portmappingObj, &iidStack);
      cmsObj_free((void **) &portmappingObj);

      if (ret != CMSRET_SUCCESS)
      {
         CmsRet r2;
         cmsLog_error("Failed to set Dev2NatPortMappingObject, ret = %d", ret);

         r2 = cmsObj_deleteInstance(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack);
         if (r2 != CMSRET_SUCCESS)
         {
            cmsLog_error("failed to delete created Dev2NatPortMappingObject, r2=%d", r2);
         }

         cmsLog_debug("Failed to set virtual server and successfully delete created Dev2NatPortMappingObject");

      }
   }
   else
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
   return ret;
}

CmsRet dalVirtualServer_addEntry_dev2(const char *dstWanIf, const char *srvName, const char *srvAddr, const char *protocol,
                                                         const UINT16 EPS, const UINT16 EPE, const UINT16 IPS, const UINT16 IPE)
{
    return dalVirtualServer_addEntry_app_dev2(dstWanIf,srvName,srvAddr,protocol,EPS,EPE,IPS,IPE,"User");
}

CmsRet dalVirtualServer_deleteEntry_dev2(const char * srvAddr, const char * protocol, const UINT16 EPS, const UINT16 EPE, const UINT16 IPS, const UINT16 IPE)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found = FALSE;
   Dev2NatPortMappingObject *portmappingObj=NULL;

   /* deleting a new virtual server entry */
   cmsLog_debug("Deleting virtual server with %s/%s/%u/%u/%u/%u", srvAddr, protocol, EPS, EPE, IPS, IPE);

   while ((ret = cmsObj_getNext(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack, (void **) &portmappingObj)) == CMSRET_SUCCESS)
   {
      if(!cmsUtl_strcmp(srvAddr, portmappingObj->internalClient) && !cmsUtl_strcmp(protocol, portmappingObj->protocol) &&
         EPS == portmappingObj->externalPort&& EPE == portmappingObj->externalPortEndRange &&
         IPS == portmappingObj->internalPort&& IPE == portmappingObj->X_BROADCOM_COM_InternalPortEndRange)
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &portmappingObj);
      }
   }

   if(found == TRUE)
   {
      ret = cmsObj_deleteInstance(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack);
      cmsObj_free((void **) &portmappingObj);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete WanPppConnPortmappingObject, ret = %d", ret);
         return ret;
      }
   }
   else
   {
      cmsLog_debug("bad %s/%s/%u/%u/%u/%u, no virtual server entry found", srvAddr, protocol, EPS, EPE, IPS, IPE);
      return CMSRET_INVALID_ARGUMENTS;
   }

   return ret;

}


CmsRet dalDmzHost_addEntry_dev2(const char *srvAddr)
{
   /* DMZ is a special case in Virtual Server with external port = 0 and bind on allInterface ! */
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2NatPortMappingObject *portmappingObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   //Try to get perious one 
   while ( (ret = cmsObj_getNext
             (MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack, (void **) &portmappingObj)) == CMSRET_SUCCESS) 
   {
          if (!cmsUtl_strncmp(portmappingObj->X_BROADCOM_COM_AppName,"DMZ",3)) 
          {
             found=TRUE;
             break;
          }
          // Free the mem allocated this object by the get API.
          cmsObj_free((void **) &portmappingObj);
   }

   if(found == TRUE)
   {
          ret = cmsObj_deleteInstance(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack);
          cmsObj_free((void **) &portmappingObj);

          if (ret != CMSRET_SUCCESS)
          {
             cmsLog_error("Failed to delete DMZ, ret = %d", ret);
             return ret;
          }
   }

   if(cmsUtl_strcmp(srvAddr,""))
       ret = dalVirtualServer_addEntry_app_dev2("","DMZ",cmsMem_strdup(srvAddr),"0",0,0,0,0,"DMZ");

   return ret;
}

CmsRet dalGetDmzHost_dev2(char *address)
{
    /* DMZ is a special case in Virtual Server with external port = 0 and bind on allInterface ! */
    UBOOL8 found=FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    Dev2NatPortMappingObject *portmappingObj=NULL;
    CmsRet ret = CMSRET_SUCCESS;

    //Try to get perious one 
    while ( (ret = cmsObj_getNext
            (MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack, (void **) &portmappingObj)) == CMSRET_SUCCESS)
    {
       if (!cmsUtl_strncmp(portmappingObj->X_BROADCOM_COM_AppName,"DMZ",3)) 
       {
       found=TRUE;
       break;
       }
       // Free the mem allocated this object by the get API.
       cmsObj_free((void **) &portmappingObj);
    }

    if(found == TRUE)
    {
       strcpy(address, portmappingObj->internalClient);
       cmsObj_free((void **) &portmappingObj);
    }else
    {
       strcpy(address,"");
       cmsLog_debug("Failed to get DMZ host, ret=%d", ret);
    }

    return CMSRET_SUCCESS;
}


CmsRet dalNat_deletePortMapping_dev2(const char * ipIntffullPath)
{                                        
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackPrev = EMPTY_INSTANCE_ID_STACK;
   Dev2NatPortMappingObject *portmappingObj = NULL;
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;
   
   if (ipIntffullPath == NULL)
   {
      cmsLog_error("NULL string.");
      return ret;
   }
   cmsLog_debug("Enter ipIntffullPath %s.", ipIntffullPath);
   
   while((ret = cmsObj_getNextFlags(MDMOID_DEV2_NAT_PORT_MAPPING,
                                    &iidStack, 
                                    OGF_NO_VALUE_UPDATE,
                                    (void **)&portmappingObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcasecmp(portmappingObj->interface, ipIntffullPath))
      {
         cmsObj_deleteInstance(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack);
         iidStack = iidStackPrev;
      }
      else
      {
         iidStackPrev = iidStack;
      }

      cmsObj_free((void **)&portmappingObj);
   }    
   
   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}


#endif  /* DMP_DEVICE2_BASELINE_1 */


