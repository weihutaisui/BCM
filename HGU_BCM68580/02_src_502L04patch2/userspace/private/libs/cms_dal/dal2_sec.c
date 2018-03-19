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

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"
#include "dal.h"
#include "rut_iptables.h"

#ifdef DMP_DEVICE2_BASELINE_1    /* this file touches TR181 objects */


CmsRet dalSec_addIpFilterCommon_dev2(const char *name, const char *ipver, const char *protocol,
                            const char *srcAddr, const char *srcMask,  const char *srcPort, 
                            const char *dstAddr, const char *dstMask, const char *dstPort,
                            const char *ifName,const char *action)
{
   char *ipIntfFullPath=NULL;
   Dev2FirewallExceptionObject *fwExObj=NULL;
   Dev2FirewallExceptionRuleObject *fwExRuleObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   int found = FALSE;
   char port_start[BUFLEN_8], *port_end;
   CmsRet ret = CMSRET_SUCCESS;

   /* Find Dev2FirewallExceptionObject for Interface . 
        If there no Dev2FirewallExceptionObject for interface ,  create one .
   */

   cmsLog_debug("entered, ifName=%s", ifName);

   if(qdmIntf_intfnameToFullPathLocked_dev2(ifName,FALSE,&ipIntfFullPath) == CMSRET_SUCCESS)
   {
         if(rutIpt_GetfwExceptionbyFullPath_dev2(ipIntfFullPath,fwExObj,&iidStack) == CMSRET_SUCCESS)
         {
             found = TRUE;
         }

         if(!found)
         {
             rutIpt_AddfwExceptionforIPDevice_dev2(ipIntfFullPath);
             if((ret = rutIpt_GetfwExceptionbyFullPath_dev2(ipIntfFullPath,fwExObj,&iidStack)) != CMSRET_SUCCESS)
             {
                 cmsLog_error("Failed to add MDMOID_DEV2_FIREWALL_EXCEPTION_RULE on %s, ret = %d", ipIntfFullPath,ret);
                 CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
                 return ret;
             }
         }

         cmsObj_free((void **)&fwExObj);

         cmsObj_addInstance(MDMOID_DEV2_FIREWALL_EXCEPTION_RULE,&iidStack);
         if ((ret = cmsObj_get(MDMOID_DEV2_FIREWALL_EXCEPTION_RULE, &iidStack, 0, (void **) &fwExRuleObj)) != CMSRET_SUCCESS)
         {
             cmsLog_error("Failed to add MDMOID_DEV2_FIREWALL_EXCEPTION_RULE on %s, ret = %d", ipIntfFullPath,ret);
             CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
             cmsObj_deleteInstance(MDMOID_DEV2_FIREWALL_EXCEPTION_RULE, &iidStack);
             return ret;
         }

         fwExRuleObj->enable = TRUE;
         CMSMEM_REPLACE_STRING(fwExRuleObj->filterName, name);
         CMSMEM_REPLACE_STRING(fwExRuleObj->IPVersion, ipver);
         CMSMEM_REPLACE_STRING(fwExRuleObj->sourceIPAddress, srcAddr);
         CMSMEM_REPLACE_STRING(fwExRuleObj->sourceNetMask, srcMask);

         if( (port_end = strchr(srcPort, ':')) == NULL)
         {
              fwExRuleObj->sourcePortStart = atoi(srcPort);
         }
         else
         {
              strncpy(port_start, srcPort, (port_end - srcPort));
              fwExRuleObj->sourcePortStart = atoi(port_start);
              fwExRuleObj->sourcePortEnd= atoi(port_end + 1);
         }

         CMSMEM_REPLACE_STRING(fwExRuleObj->destinationIPAddress, dstAddr);
         CMSMEM_REPLACE_STRING(fwExRuleObj->destinationNetMask, dstMask);

         if( (port_end = strchr(dstPort, ':')) == NULL)
         {
                fwExRuleObj->destinationPortStart = atoi(dstPort);
         }
         else
         {
                strncpy(port_start, dstPort, (port_end - dstPort));
                fwExRuleObj->destinationPortStart = atoi(port_start);
                fwExRuleObj->destinationPortEnd= atoi(port_end + 1);
         }

         switch ( atoi(protocol) ) 
         {
         case 0: // TCP or UDP
                CMSMEM_REPLACE_STRING(fwExRuleObj->protocol, MDMVS_TCP_OR_UDP);
                break;
         case 1: // TCP
                CMSMEM_REPLACE_STRING(fwExRuleObj->protocol, MDMVS_TCP);
                break;
         case 2: // UDP
                CMSMEM_REPLACE_STRING(fwExRuleObj->protocol, MDMVS_UDP);
                break;
         case 3: // ICMP
                if (atoi(ipver) == 4)
                {
                   CMSMEM_REPLACE_STRING(fwExRuleObj->protocol, MDMVS_ICMP);
                }
                else
                {
                   CMSMEM_REPLACE_STRING(fwExRuleObj->protocol, MDMVS_ICMPV6);
                }
                break;
         default: // None
                CMSMEM_REPLACE_STRING(fwExRuleObj->protocol, MDMVS_NONE);
                break;
         }

         if((cmsUtl_strcmp(action, MDMVS_ACCEPT) == 0 ))
            CMSMEM_REPLACE_STRING(fwExRuleObj->target,MDMVS_ACCEPT);
         else
            CMSMEM_REPLACE_STRING(fwExRuleObj->target,MDMVS_DROP); 
		 
         if ((cmsObj_set((void *)fwExRuleObj,&iidStack)) != CMSRET_SUCCESS)
             cmsObj_deleteInstance(MDMOID_DEV2_FIREWALL_EXCEPTION_RULE, &iidStack);

         cmsObj_free((void **)&fwExRuleObj);
         CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
   }
 
   return ret; 
}

CmsRet dalSec_deleteIpFilterCommon_dev2(const char* filtername)
{
	InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
	Dev2FirewallExceptionRuleObject *fwRuleObj=NULL;
	UBOOL8 found=FALSE;
	CmsRet ret = CMSRET_SUCCESS;

	cmsLog_debug("entered, filtername:%s", filtername);
	
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DEV2_FIREWALL_EXCEPTION_RULE,
                                &iidStack,
                                (void **) &fwRuleObj)) == CMSRET_SUCCESS)
	{
	
	   if(!cmsUtl_strcmp(filtername, fwRuleObj->filterName) ) 
	   {
	      found = TRUE;

	      if((ret = cmsObj_deleteInstance(MDMOID_DEV2_FIREWALL_EXCEPTION_RULE, &iidStack)) != CMSRET_SUCCESS)
	      {
	         cmsLog_error("delete filtername:%s fail, ret=%d", filtername, ret);
	      }
	   }
	
	   cmsObj_free((void **) &fwRuleObj);
	}

	return CMSRET_SUCCESS;
}


CmsRet dalSec_addIpFilterOut_dev2(const char *name, const char *ipver, const char *protocol,
              const char *srcAddr, const char *srcMask,  const char *srcPort,
              const char *dstAddr, const char *dstMask, const char *dstPort,
              const char *ifName __attribute__((unused)))
{
   return dalSec_addIpFilterCommon_dev2(name,ipver,protocol,srcAddr,srcMask,srcPort,dstAddr,dstMask,dstPort,"br0",MDMVS_DROP);
}


CmsRet dalSec_deleteIpFilterOut_dev2(const char* filtername)
{
   cmsLog_debug("entered, filtername:%s", filtername);

   return dalSec_deleteIpFilterCommon_dev2(filtername);
}


CmsRet dalSec_addIpFilterIn_dev2(const char *name, const char *ipver, const char *protocol,
                            const char *srcAddr, const char *srcMask,  const char *srcPort, 
                            const char *dstAddr, const char *dstMask, const char *dstPort,
                            const char *ifName)
{
   return dalSec_addIpFilterCommon_dev2(name,ipver,protocol,srcAddr,srcMask,srcPort,dstAddr,dstMask,dstPort,ifName,MDMVS_ACCEPT);
}


CmsRet dalSec_deleteIpFilterIn_dev2(const char* filtername __attribute__((unused)))
{
   cmsLog_debug("entered, filtername:%s", filtername);

   return dalSec_deleteIpFilterCommon_dev2(filtername);
}


CmsRet dalSec_addMacFilter_dev2(const char *protocol, const char *srcMac, const char *dstMac, const char *direction, const char *ifName)
{
    CmsRet ret = CMSRET_SUCCESS;

    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    Dev2MacFilterCfgObject *macFilterCfg=NULL;
    char *ipIntfFullPath=NULL;
    int found = FALSE;
    MdmPathDescriptor pathDesc;

    /* Get wan interface path */
    cmsLog_debug("entered, ifName=%s", ifName);

    if(qdmIntf_intfnameToFullPathLocked_dev2(ifName,FALSE,&ipIntfFullPath) == CMSRET_SUCCESS)
    {
        /* Get macFilterObj iid */
        INIT_PATH_DESCRIPTOR(&pathDesc);
        if(qdmIpIntf_getMacFilterByFullPathLocked_dev2(ipIntfFullPath, &pathDesc) == CMSRET_SUCCESS)
        {
           iidStack = pathDesc.iidStack;         
           found = TRUE;
        }

        if(!found)
        {
           Dev2MacFilterObject *macFilterObj=NULL;        
           INIT_INSTANCE_ID_STACK(&iidStack);
     
           cmsObj_addInstance(MDMOID_DEV2_MAC_FILTER,&iidStack);
           if ((ret = cmsObj_get(MDMOID_DEV2_MAC_FILTER, &iidStack, 0, (void **) &macFilterObj)) != CMSRET_SUCCESS)
           {
              cmsLog_error("Failed to add Dev2MacFilterObject on %s, ret = %d", ipIntfFullPath,ret);
              CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
              cmsObj_deleteInstance(MDMOID_DEV2_MAC_FILTER, &iidStack);
              return ret;
           }
     
           CMSMEM_REPLACE_STRING(macFilterObj->IPInterface, ipIntfFullPath);
     
           cmsObj_set((void *)macFilterObj,&iidStack);
           cmsObj_free((void **)&macFilterObj);
        }

        /* Add new instance of mac filter */
        if ((ret = cmsObj_addInstance(MDMOID_DEV2_MAC_FILTER_CFG, &iidStack)) != CMSRET_SUCCESS)
        {
           cmsLog_error("could not create new MacFilterCfgObject, ret=%d", ret);
           CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
           return ret;
        }

        if ((ret = cmsObj_get(MDMOID_DEV2_MAC_FILTER_CFG, &iidStack, OGF_NORMAL_UPDATE, (void **) &macFilterCfg)) != CMSRET_SUCCESS)
        {
           cmsLog_error("Failed to get MacFilterCfgObject, ret=%d", ret);
           cmsObj_deleteInstance(MDMOID_DEV2_MAC_FILTER_CFG, &iidStack);
           CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
           return ret;
        }

        macFilterCfg->enable = TRUE;
        CMSMEM_REPLACE_STRING(macFilterCfg->sourceMAC, srcMac);
        CMSMEM_REPLACE_STRING(macFilterCfg->destinationMAC, dstMac);
        CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

        switch(atoi(protocol))
        {
           case 1:  // PPPoE
              CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_PPPOE);
              break;
           case 2: // IPv4
              CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_IPV4);
              break;
           case 3: // IPv6
              CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_IPV6);
              break;
           case 4: // AppleTalk
              CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_APPLETALK);
              break;
           case 5: // IPX
              CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_IPX);
              break;
           case 6: // NetBEUI
              CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_NETBEUI);
              break;
           case 7: // IGMP
              CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_IGMP);
              break;
           default: // None
              CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_NONE);
              break;
        }

        switch(atoi(direction))
        {
           case 0:  // LAN to WAN
              CMSMEM_REPLACE_STRING(macFilterCfg->direction, MDMVS_LAN_TO_WAN);
              break;
           case 1: // WAN to LAN
              CMSMEM_REPLACE_STRING(macFilterCfg->direction, MDMVS_WAN_TO_LAN);
              break;
           default: // BOTH
              CMSMEM_REPLACE_STRING(macFilterCfg->direction, MDMVS_BOTH);
              break;
        }
        
        /* set and activate WanIpConnFirewallExceptionObject */
        ret = cmsObj_set(macFilterCfg, &iidStack);
        cmsObj_free((void **) &macFilterCfg);

        if (ret != CMSRET_SUCCESS)
        {
           CmsRet r2;
           cmsLog_error("Failed to set MacFilterCfgObject, ret = %d", ret);
                  
           r2 = cmsObj_deleteInstance(MDMOID_MAC_FILTER_CFG, &iidStack);
           if (r2 != CMSRET_SUCCESS)
           {
              cmsLog_error("failed to delete created MacFilterCfgObject, r2=%d", r2);
           }
           
           cmsLog_debug("Failed to set IP filter entry and successfully delete created MacFilterCfgObject");
        }
    }

   return ret;
}


CmsRet dalSec_deleteMacFilter_dev2(const char *protocol, const char *srcMac, const char *dstMac, const char *direction, const char *ifName)
{
   CmsRet ret = CMSRET_SUCCESS;    
   Dev2MacFilterCfgObject *macFilterCfg = NULL;
   Dev2MacFilterObject *macFilterObj=NULL;       
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   char Wan_ifName[BUFLEN_32];
   UBOOL8 found = FALSE;

   /* deleting a mac filter entry */
   cmsLog_debug("Deleting a mac filter entry with %s/%s/%s/%s/%s", protocol, srcMac, dstMac, direction, ifName);

   while (cmsObj_getNext(MDMOID_DEV2_MAC_FILTER_CFG, &iidStack, (void **) &macFilterCfg) == CMSRET_SUCCESS)
   {
      if(!cmsUtl_strcmp(protocol, macFilterCfg->protocol) && !cmsUtl_strcmp(srcMac, macFilterCfg->sourceMAC) &&\
          !cmsUtl_strcmp(dstMac, macFilterCfg->destinationMAC) && !cmsUtl_strcmp(direction, macFilterCfg->direction) ) 
      {

         parentIidStack = iidStack;
         if ((ret = cmsObj_getAncestor(MDMOID_DEV2_MAC_FILTER, MDMOID_DEV2_MAC_FILTER_CFG, 
                                                           &parentIidStack, (void **) &macFilterObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to get Dev2MacFilterObject. ret=%d", ret);
            cmsObj_free((void **) &macFilterCfg);
            return ret;
         }
         
         Wan_ifName[0]='\0';
         
         qdmIntf_getIntfnameFromFullPathLocked_dev2(macFilterObj->IPInterface, Wan_ifName, sizeof(Wan_ifName));
         
         cmsObj_free((void **) &macFilterObj);   
         cmsObj_free((void **) &macFilterCfg);

         if (!cmsUtl_strcmp(ifName, Wan_ifName))
         {
            found = TRUE;
            break;
         }
      }
      
      cmsObj_free((void **) &macFilterCfg);
   }

   if(found == FALSE) 
   {
      cmsLog_debug("no mac filter entry found: %s/%s/%s/%s/%s", protocol, srcMac, dstMac, direction, ifName);
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else 
   {
      ret = cmsObj_deleteInstance(MDMOID_DEV2_MAC_FILTER_CFG, &iidStack);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete MacFilterCfgObject, ret = %d", ret);
      }
   }

   return ret;
}


CmsRet dalSec_ChangeMacFilterPolicy_dev2(const char *ifName)
{
   CmsRet ret = CMSRET_SUCCESS;


   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   char *ipIntfFullPath=NULL;
   Dev2MacFilterCfgObject *macFilterCfg=NULL;
   Dev2MacFilterObject *macFilterObj=NULL;
   MdmPathDescriptor pathDesc;

   cmsLog_debug("Enter dalSec_ChangeMacFilterPolicy() with ifName = %s", ifName);

   if(qdmIntf_intfnameToFullPathLocked_dev2(ifName,FALSE,&ipIntfFullPath) == CMSRET_SUCCESS)
   {
      /* Get macFilterObj iid */
      INIT_PATH_DESCRIPTOR(&pathDesc);
      if((ret = qdmIpIntf_getMacFilterByFullPathLocked_dev2(ipIntfFullPath, &pathDesc)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get IPInterface FullPath, ret=%d", ret);         
         CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
         return CMSRET_INTERNAL_ERROR;
      }else
      {
         parentIidStack = pathDesc.iidStack;
      } 
   }
   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

   if ((ret = cmsObj_get(MDMOID_DEV2_MAC_FILTER, &parentIidStack, OGF_NORMAL_UPDATE, (void **) &macFilterObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get MacFilterCfgObject, ret=%d", ret);
      return ret;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while(cmsObj_getNextInSubTree(MDMOID_DEV2_MAC_FILTER_CFG, &parentIidStack, &iidStack, (void **)&macFilterCfg) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &macFilterCfg);
	  
      ret = cmsObj_deleteInstance(MDMOID_DEV2_MAC_FILTER_CFG, &iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Fail to delete MacFilterCfgObject in dalSec_ChangeMacFilterPolicy()");
         cmsObj_free((void **) &macFilterObj);         
         return CMSRET_INTERNAL_ERROR;
      }
      INIT_INSTANCE_ID_STACK(&iidStack);
   }

   macFilterObj->enable = TRUE;
   
   if (cmsUtl_strcmp(macFilterObj->policy, MDMVS_FORWARD) == 0)
   {
      CMSMEM_REPLACE_STRING(macFilterObj->policy, MDMVS_BLOCKED);
   }
   else if (cmsUtl_strcmp(macFilterObj->policy, MDMVS_BLOCKED) == 0)
   {
      CMSMEM_REPLACE_STRING(macFilterObj->policy, MDMVS_FORWARD);
   }

   /* set and activate MacFilterCfgObject */
   ret = cmsObj_set(macFilterObj, &parentIidStack);
   cmsObj_free((void **) &macFilterObj);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set macFilterObj, ret = %d", ret);
   }

   return ret;
}

#endif /* DMP_DEVICE2_BASELINE_1 */

