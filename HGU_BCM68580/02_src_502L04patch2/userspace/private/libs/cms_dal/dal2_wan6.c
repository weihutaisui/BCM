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
#include "dal2_wan.h"


#ifdef DMP_DEVICE2_BASELINE_1    /* this file touches TR181 objects */

/* this file touches TR181 IPv6 objects */
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1


static CmsRet updateIpIntfForIPv6(const WEB_NTWK_VAR *webVar,
                                  const char *wanL2IfName,
                                  char *ipIntfPathRefBuf,
                                  UINT32 bufLen,
                                  MdmPathDescriptor *ipIntfPathDesc)
{

   char ipIntfLowerLayer[MDM_SINGLE_FULLPATH_BUFLEN]={0};      
   char *ethLinkPathBuf=NULL;
   char ethVlanPathRef[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   MdmPathDescriptor ethVlanPathDesc = EMPTY_PATH_DESCRIPTOR;
   Dev2VlanTerminationObject *ethVlanObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;
   char ipIntfIfName[CMS_IFNAME_LENGTH];
   UBOOL8 isLayer2=FALSE;
   char *locIpIntfFullPath = NULL;
   
   cmsLog_debug("Enter. wanL2IfName=%s", wanL2IfName);
   
   if ((ret = qdmIntf_intfnameToFullPathLocked_dev2(wanL2IfName,
                                                    isLayer2,
                                                    &ethLinkPathBuf)) != CMSRET_SUCCESS)
   {
      cmsLog_error("qdmIntf_intfnameToFullPathLocked_dev2 failed. ret %d", ret);
      return ret;
   }
   
   ret = qdmEth_getEthernetVlanTermination_dev2(webVar->vlanMuxId, 
                                                ethLinkPathBuf, 
                                                ethVlanPathRef, 
                                                sizeof(ethVlanPathRef));

   CMSMEM_FREE_BUF_AND_NULL_PTR(ethLinkPathBuf);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("qdmEth_getEthernetVlanTermination_dev2 failed. ret %d", ret);
      return ret;
   }
   
   if ((ret = cmsMdm_fullPathToPathDescriptor(ethVlanPathRef, &ethVlanPathDesc)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed. ret %d", ret);
      return ret;
   }

   ret = cmsObj_get(ethVlanPathDesc.oid, &(ethVlanPathDesc.iidStack), OGF_NO_VALUE_UPDATE, (void **)&ethVlanObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get ethVlan Object. ret %d", ret);
      return ret;
   }

  /* Assume it's IPoE first and save ipIntfIfName (ethVlanObj->name is same as ip interface name for IPoE.)
   * and IP interface lower layer path reference
   */
   cmsUtl_strncpy(ipIntfIfName, ethVlanObj->name, sizeof(ipIntfIfName)-1);
   cmsUtl_strncpy(ipIntfLowerLayer, ethVlanPathRef, sizeof(ipIntfLowerLayer));
   cmsObj_free((void **) &ethVlanObj);
   
   if (dalWan_isPPPWanConnection_dev2(webVar))
   {
      UBOOL8 found=FALSE;
      InstanceIdStack pppIntfIidStack=EMPTY_INSTANCE_ID_STACK;
      Dev2PppInterfaceObject *pppIntfObj=NULL;
      
	  /* Need to get ppp object matching ethVlanTermination object 
	  * since PPP path reference is the lowerlayer for IP interface
	  */
      while (!found &&
             ((ret = cmsObj_getNext(MDMOID_DEV2_PPP_INTERFACE,
                                    &pppIntfIidStack,
                                    (void **) &pppIntfObj)) == CMSRET_SUCCESS))
      {
         if (!cmsUtl_strcmp(pppIntfObj->lowerLayers, ethVlanPathRef))
         {
            MdmPathDescriptor pathDesc;
            CmsRet r2;
            char *pppIntfFullPath;

            found = TRUE;
            
            INIT_PATH_DESCRIPTOR(&pathDesc);
            pathDesc.oid = MDMOID_DEV2_PPP_INTERFACE;
            pathDesc.iidStack = pppIntfIidStack;

            r2 = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &pppIntfFullPath);
            if (r2 != CMSRET_SUCCESS)
            {
               cmsLog_error("PathDescToFullPath failed, ret=%d", r2);
            }
            else
            {
               if (cmsUtl_strlen(pppIntfFullPath) > (SINT32) bufLen)
               {
                  cmsLog_error("pppIntfFullPath is too big to fit ipIntfLowerLayer");
                  ret = CMSRET_INTERNAL_ERROR;
               }
               else
               {
                  /* Overwrite the ip interface name and lower layer path reference */
                  cmsUtl_strncpy(ipIntfLowerLayer, pppIntfFullPath, bufLen);
                  cmsUtl_strncpy(ipIntfIfName, pppIntfObj->name, sizeof(ipIntfIfName)-1);
               }

               CMSMEM_FREE_BUF_AND_NULL_PTR(pppIntfFullPath);
            }
         }            
         cmsObj_free((void **) &pppIntfObj);
      }  

      if (ret == CMSRET_INTERNAL_ERROR)
      {
         return ret;
      }
   }

   
   /* Update ip interface for IPv6.   For ppp, the ppp interface object is created by IPv4 so no 
   * need to be modified since pppd might be already started and set ppp object will cause shutting
   * it down and restart it
   */
   if ((ret = qdmIntf_intfnameToFullPathLocked_dev2(ipIntfIfName,
                                                    isLayer2,
                                                    &locIpIntfFullPath)) != CMSRET_SUCCESS)
   {
      cmsLog_error("qdmIntf_intfnameToFullPathLocked_dev2 failed. ret %d", ret);
      return ret;
   }   
   else
   {
      Dev2IpInterfaceObject *ipIntf=NULL;

      cmsUtl_strncpy(ipIntfPathRefBuf, locIpIntfFullPath, bufLen);
      CMSMEM_FREE_BUF_AND_NULL_PTR(locIpIntfFullPath);
      
      if ((ret = cmsMdm_fullPathToPathDescriptor(ipIntfPathRefBuf, ipIntfPathDesc)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsMdm_fullPathToPathDescriptor failed. ret %d", ret);
         return ret;
      }   
      ret = cmsObj_get(ipIntfPathDesc->oid, &(ipIntfPathDesc->iidStack), OGF_NO_VALUE_UPDATE, (void **)&ipIntf);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get ip interface Object. ret %d", ret);
         return ret;
      }      
      
      /* Any IPv6 related additonal setting goes to here ... */
      
      ret =  cmsObj_set(ipIntf, &(ipIntfPathDesc->iidStack));
      cmsObj_free((void **) &ipIntf);      

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to set ipIntf Object. ret %d", ret);
      }
      
   }

   cmsLog_debug("Exit. ret %d, ipIntfPathRefBuf %s", ret, ipIntfPathRefBuf);
   
   return ret;
   
}


CmsRet dalWan_addIPv6Service_dev2(const WEB_NTWK_VAR *webVar)
{
    /* add TR-181 IPv6 objects here.
    * If there is any need to look up (get) objects, that should be done in a QDM
    * function, which will have multiple copies to find objects in the right
    * place.
    */

   char *layer2FullPath=NULL;
   char IpIntfPathRef[MDM_SINGLE_FULLPATH_BUFLEN] = {0};
   char ipIntfLowerLayer[MDM_SINGLE_FULLPATH_BUFLEN]={0};      
   MdmPathDescriptor ipIntfPathDesc;
   char ethLinkPathRef[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   char ethVlanPathRef[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   char pppPathRef[MDM_SINGLE_FULLPATH_BUFLEN]={0};   
   UBOOL8 isPPP = dalWan_isPPPWanConnection_dev2(webVar);
   UBOOL8 supportIpv4=FALSE;
   UBOOL8 supportIpv6=FALSE;
   UBOOL8 enableIgmp = FALSE;
   UBOOL8 enableIgmpSource = FALSE;
   UBOOL8 enableMld = FALSE;
   UBOOL8 enableMldSource = FALSE;
   UBOOL8 ipInterfaceExisted=FALSE;
   CmsRet ret = CMSRET_SUCCESS;
   char wanL2IfName[BUFLEN_32];
   char *p;
   
   cmsLog_debug("ifName=%s, protocol=%d", webVar->wanL2IfName, webVar->ntwkPrtcl);
   strncpy(wanL2IfName, webVar->wanL2IfName, sizeof(wanL2IfName)-1);
   
   if ((p = strchr(wanL2IfName, '/')))
   {
      /* now wanL2IfName is "ptm0", "atm0" etc. */
      *p = '\0';
   }
   else if (cmsUtl_strstr(wanL2IfName, ETH_IFC_STR) ||
      cmsUtl_strstr(wanL2IfName, MOCA_IFC_STR) ||
#ifdef SUPPORT_CELLULAR
      cmsUtl_strstr(wanL2IfName, CELLULAR_IFC_STR) ||
#endif      
      cmsUtl_strstr(wanL2IfName, GPON_IFC_STR) ||
      cmsUtl_strstr(wanL2IfName, EPON_IFC_STR) ||
      cmsUtl_strstr(wanL2IfName, WLAN_IFC_STR))
   {
      /* do nothing for now */
   }
   else
   {
      cmsLog_error("wanL2IfName %s - wrong format", wanL2IfName);
      return ret;
   }

   cmsLog_debug("wanL2IfName %s", wanL2IfName);

   /* First get the fullpath to the layer 2 interface name. */
   /* Tricky Hack Alert: In Hybrid IPv6 mode, the "layer 2" fullpath
    * is actually the fullpath the layer 3 object in the TR98 data model.
    * So if we are in Hybrid mode, get the intfName from webVar->wanIfName
    * and get the fullpath to the layer 3 intfName.
    * In Pure181 mode, everything works as expected.
    */
   if (cmsMdm_isDataModelDevice2())
   {
      /* we are in Pure181 mode, do the normal thing */
      UBOOL8 layer2 = TRUE;
      ret = qdmIntf_intfnameToFullPathLocked(wanL2IfName, layer2, &layer2FullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get fullpath of %s, ret=%d", wanL2IfName, ret);
         return ret;
      }
      cmsLog_debug("Pure181 wanL2IfName %s ==> L2FullPath %s",
                    wanL2IfName, layer2FullPath);
   }
   else
   {
      /* we are in Hybrid mode, do tricky hack */
      UBOOL8 layer2 = FALSE;
      ret = qdmIntf_intfnameToFullPathLocked(webVar->wanIfName, layer2, &layer2FullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get fullpath of %s, ret=%d", webVar->wanIfName, ret);
         return ret;
      }

      /* qdmIntf_intfnameToFullPathLocked_igd returns fullpath with end dot, so strip it off */
      if (layer2FullPath[strlen(layer2FullPath)-1] == '.')
      {
         layer2FullPath[strlen(layer2FullPath)-1] = '\0';
      }

      cmsLog_debug("Hybrid wanIfName %s ==> FullPath %s",
                   webVar->wanIfName, layer2FullPath);
   }


   if (webVar->enblIpVer == ENBL_IPV4_ONLY)
   {
      supportIpv4 = TRUE;
      supportIpv6 = FALSE;
   }
   else if (webVar->enblIpVer == ENBL_IPV6_ONLY)
   {
      supportIpv4 = FALSE;
      supportIpv6 = TRUE;
   }
   else if (webVar->enblIpVer == ENBL_IPV4_IPV6)
   {
      /*
       * Enable both IPv4 and IPv6, but.....
       * This function may be called when we are in Hybrid mode.
       * In Hybrid mode, we use the IP.Interface only for IPv6.  So if we
       * are in Hybrid mode, do not enable IPv4.
       */
      if (cmsMdm_isDataModelDevice2())
      {
         supportIpv4 = TRUE;
         /* For Pure 181 with dual stack wan Connection, IP.interface is already existed
         * since ipv4 WAN connection is created first. So do not need to create
         * Ethernet.Link and Ethernet.vlantermination objects.
         */
         ipInterfaceExisted = TRUE;
         
      }
      else
      {
         supportIpv4 = FALSE;
      }
      supportIpv6 = TRUE;
   }

   

   if (ipInterfaceExisted)
   {
      if ((ret =updateIpIntfForIPv6(webVar,  
                                    wanL2IfName,
                                    IpIntfPathRef,
                                    sizeof(IpIntfPathRef),
                                    &ipIntfPathDesc))!= CMSRET_SUCCESS)
      {
         cmsLog_error("updateIpIntfForIpv6 failed, ret = %d", ret);
         dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
         CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);
         return ret;             
      }
   }
   else
   {
#ifdef SUPPORT_CELLULAR
      /* Cellular WAN will not have EthernetLink and Vlan*/
      if (cmsUtl_strstr(wanL2IfName, CELLULAR_IFC_STR) == NULL)
      {
#endif
      /* No IP interface existed. Need to create the Ethernet.Link first and then Ethernet.vlantermination objects */
      ret = dalEth_addEthernetLink_dev2(layer2FullPath, ethLinkPathRef, sizeof(ethLinkPathRef));


      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("dalEth_addEthernetLink_dev2 failed, ret = %d", ret);
         dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
         CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);
         return ret;      
      }

      /* For now, always create ethernet.vlantermination object since our TR98
      * always has this even there is no vlan id to set.
      */
      if ((ret = dalEth_addEthernetVlanTermination_dev2(webVar,
                                                        ethLinkPathRef, 
                                                        ethVlanPathRef, 
                                                        sizeof(ethVlanPathRef))) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalEth_addEthernetVlanTermination_dev2 failed, ret = %d", ret);
         dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
         CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);
         return ret;    
      }
   
      /* save ethVlanPathRef in ipIntfLowerLayer for non ppp model */
      strncpy(ipIntfLowerLayer, ethVlanPathRef, sizeof(ipIntfLowerLayer));

      /*
      *  PPP interface is between Ethernet.vlantermination and ip interface.
      */
      if (isPPP)
      {
         if ((ret = dalPpp_addIntfObject_dev2(webVar, 
                                              supportIpv4,
                                              supportIpv6,
                                              ethVlanPathRef,
                                              pppPathRef,
                                              sizeof(pppPathRef))) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalPpp_addIntfObject_dev2 failed, ret %d", ret);
            dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
            CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);
            return ret;
         }
         /* Use pppPathRef as ipIntfLowerLayer now for ppp WAN */
         strncpy(ipIntfLowerLayer, pppPathRef, sizeof(ipIntfLowerLayer));
      }
      
#ifdef SUPPORT_CELLULAR
      }//if (cmsUtl_strstr(wanL2IfName, CELLULAR_IFC_STR) == NULL)
      else      
      {
         strncpy(ipIntfLowerLayer, layer2FullPath, sizeof(ipIntfLowerLayer));
      }
#endif

#ifdef DMP_X_BROADCOM_COM_IGMP_1
      enableIgmp = (UBOOL8) webVar->enblIgmp;
      enableIgmpSource = (UBOOL8) webVar->enblIgmpMcastSource;
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
      enableMld = (UBOOL8) webVar->enblMld;
      enableMldSource = (UBOOL8) webVar->enblMldMcastSource;
#endif

      /* Add an ip interface object */
      if ((ret = dalIp_addIntfObject_dev2(supportIpv4,
                                          supportIpv6, 
                                          NULL,  /* intfGroupName */
                                          FALSE, NULL,  /* isBridgeService, bridgeName */
                                          webVar->enblFirewall,
                                          enableIgmp, enableIgmpSource,
                                          enableMld, enableMldSource,
                                          ipIntfLowerLayer,
                                          IpIntfPathRef,
                                          sizeof(IpIntfPathRef), 
                                          &ipIntfPathDesc)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalIp_addIntfObject_dev2 failed. ret=%d", ret);
         dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
         CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);
         return ret;
      }
   }

   
   /* Now need add/set objects related to this ip interface based on 
   * IpIntfPathRef and ipIntfPathDesc (from either updated existing or newly created ip interface)
   */
   if (cmsUtl_strcmp(webVar->wanAddr6Type, MDMVS_STATIC) == 0)
   {
      char prefixPathRef[MDM_SINGLE_FULLPATH_BUFLEN]={0};      
         
      /* static Ip interface  
      * Static IP interface needs prefix/address objects 
      */
      
      /*  Add and set the ipv6 prefix object */
      if ((ret = dalIp_addIpIntfIPv6Prefix_dev2(&(ipIntfPathDesc.iidStack), 
                                                webVar, 
                                                prefixPathRef, 
                                                sizeof(prefixPathRef))) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalIp_addIpIntfIPv6Prefix_dev2 failed. ret=%d", ret);
      }
      else
      {
         /* Add and set the ipv6 address object */   
         if ((ret = dalIp_addIpIntfIpv6Address_dev2(&(ipIntfPathDesc.iidStack), webVar, prefixPathRef)) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalIp_addIpIntfIpv6Address_dev2 failed. ret=%d", ret);
         }
      }

      if (ret != CMSRET_SUCCESS)
      {
         dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
      }
      
      //FIXME IPV6:  optional for v6 if  webVar->wanGtwy6 is not NULL ?     
      //CMSMEM_REPLACE_STRING(wanIpConn->X_BROADCOM_COM_DefaultIPv6Gateway, webVar->wanGtwy6);
      
   }
#ifdef SUPPORT_CELLULAR
   else if (cmsUtl_strcmp(webVar->wanAddr6Type, MDMVS_X_BROADCOM_COM_NAS) == 0)
   {
      strncpy(ipIntfLowerLayer, layer2FullPath, sizeof(ipIntfLowerLayer));
   }
#endif   
   else 
   {
      /** Dynamic Ip interface
      *
      * Need to add/set the DHCPv6 client object
      */

      if ((ret = dalIp_addIpIntfDhcpv6Client_dev2(webVar, IpIntfPathRef)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalIp_addIpIntfDhcpv6Client_dev2 failed. ret=%d", ret);
         dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
         CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);
         return ret;
      }

      if (webVar->dhcp6cForPd)
      {
         char prefixPathRef[MDM_SINGLE_FULLPATH_BUFLEN]={0};

         if ((ret = dalIp_addIPv6DelegatedPrefix_dev2(&(ipIntfPathDesc.iidStack), 
                      prefixPathRef, sizeof(prefixPathRef))) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalIp_addIPv6DelegatedPrefix_dev2 failed. ret=%d", ret);
         }
         else
         {
            /* Add and set the child prefix object, we choose the first available prefix */   
            if ((ret = dalIp_addIPv6ChildPrefix_dev2("br0", prefixPathRef, "::/64")) != CMSRET_SUCCESS)
            {
               cmsLog_error("dalIp_addIPv6ChildPrefix_dev2 failed. ret=%d", ret);
            }
         }

         if (ret != CMSRET_SUCCESS)
         {
            dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
            CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);
            return ret;
         }
      }
   }

   /* Add this intfName to list of potential IPv6 default gateways */
   if ((ret = cmsDal_setSysDfltGw6Ifc_dev2((char *) webVar->dfltGw6Ifc)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsDal_setSysDfltGw6Ifc_dev2 returns error. ret=%d", ret);
      dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
      CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);
      return ret;
   }

   if ((ret = dalDns_setIpv6DnsInfo_dev2(webVar)) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalDns_setIpv6DnsInfo_dev2 failed. ret=%d", ret);
      dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);   
      CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);
      return ret;
   }
   
   if ((ret = dalPcp_set_dev2(IpIntfPathRef, webVar->pcpMode,
                              webVar->pcpServer)) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalPcp_set_dev2 failed. ret=%d", ret);
      dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);   
      CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);
      return ret;
   }
   
   CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);
   cmsLog_debug("Exit: ret %d", ret);
   
   return ret;
}


#endif  /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */

#endif /* DMP_DEVICE2_BASELINE_1 */

