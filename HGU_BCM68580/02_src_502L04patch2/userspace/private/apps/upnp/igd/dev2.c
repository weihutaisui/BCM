/***********************************************************************
 *
 * Copyright (c) 2003-2012  Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
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
 *
 ************************************************************************/


#ifdef DMP_DEVICE2_BASELINE_1



#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_qdm.h"

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>


extern int setsvcval_by_name(PService psvc, const char *varname,const char *value);
extern int OutputCharValueToAC(PAction ac, char *varName, char *value);
extern int OutputNumValueToAC(PAction ac, char* varName, int value);




CmsRet getInstanceNumbersForWanIfname_dev2(const char *wan_ifname,
                                          int *wanDevInstanceNum,
                                          int *wanConnInstanceNum,
                                          int *pppipInstanceNum)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   UBOOL8 found = FALSE;

   cmsLog_debug("Entered: wan_ifname=%s", wan_ifname);

   while (!found &&
          (ret = cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                                     OGF_NO_VALUE_UPDATE,
                                     (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (ipIntfObj->X_BROADCOM_COM_Upstream &&
          !cmsUtl_strcmp(ipIntfObj->name, wan_ifname))
      {
         found = TRUE;
      }
      cmsObj_free((void **)&ipIntfObj);
   }

   if (found)
   {
      /*
       * For TR181, the first instance number is the instance number
       * of the Device.IP.Interface.x instance.  The other 2 instance numbers
       * are not relevant (or present) in TR181, so just set to 1 (to make
       * cdrouter happy?)
       */
      if (wanDevInstanceNum)
      {
         *wanDevInstanceNum = (int) iidStack.instance[0];
      }

      if (wanConnInstanceNum)
      {
         *wanConnInstanceNum = (int) 1;
      }

      if (pppipInstanceNum)
      {
         *pppipInstanceNum = (int) 1;
      }
   }

   cmsLog_debug("ret=%d found=%d wanDev=%d", ret, found, *wanDevInstanceNum);

   return ret;
}


static CmsRet getIpIntfFullPathFromInstanceNum(UINT32 instNum, char **ipIntfFullPath)
{
   MdmPathDescriptor pathDesc = EMPTY_PATH_DESCRIPTOR;

   pathDesc.oid = MDMOID_DEV2_IP_INTERFACE;
   PUSH_INSTANCE_ID(&pathDesc.iidStack, instNum);

   return (cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, ipIntfFullPath));
}


void WANConnection_UpdateVars_dev2(struct Service *psvc, int isPpp __attribute__((unused)))
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char tmpbuf[BUFLEN_32]={0};
   char ipAddrBuf[CMS_IPADDR_LENGTH]={0};
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   CmsRet ret;

   PUSH_INSTANCE_ID(&iidStack, psvc->device->parent->instance );/*instanceOfWANDevice */
   cmsLog_debug("Entered: Using instance %d", PEEK_INSTANCE_ID(&iidStack));

   ret = cmsObj_get(MDMOID_DEV2_IP_INTERFACE, &iidStack, 0, (void **)&ipIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get IP.Interface obj at %s, ret=%d",
                   cmsMdm_dumpIidStack(&iidStack), ret);
      return;
   }

   /* set Uptime */
   if (!cmsUtl_strcmp(ipIntfObj->status, MDMVS_UP))
   {
       sprintf(tmpbuf,"%u",
               cmsTms_getSeconds() - ipIntfObj->X_BROADCOM_COM_LastChange);
   }
   else
   {
       sprintf(tmpbuf, "0");
   }
   setsvcval_by_name(psvc,"Uptime",tmpbuf);


   if (mode != PCP_MODE_DSLITE)
   {
      /* set connection status */
      if (!cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv4ServiceStatus, MDMVS_SERVICEUP))
      {
         setsvcval_by_name(psvc,"ConnectionStatus", MDMVS_CONNECTED);
      }
      else if (!cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv4ServiceStatus, MDMVS_SERVICESTARTING))
      {
         setsvcval_by_name(psvc,"ConnectionStatus", MDMVS_CONNECTING);
      }
      else
      {
         setsvcval_by_name(psvc,"ConnectionStatus", MDMVS_DISCONNECTED);
      }

      sprintf(tmpbuf,"%d", qdmIpIntf_isNatEnabledOnIntfNameLocked(ipIntfObj->name));
      setsvcval_by_name(psvc,"NATEnabled",tmpbuf);

      qdmIpIntf_getIpvxAddressByNameLocked(CMS_AF_SELECT_IPV4,
                                           ipIntfObj->name, ipAddrBuf);
      setsvcval_by_name(psvc,"ExternalIPAddress", ipAddrBuf);

      /* count number of portmapping entries pointing to this ip.interface */
      {
         InstanceIdStack pmapIidStack = EMPTY_INSTANCE_ID_STACK;
         Dev2NatPortMappingObject *pmapObj=NULL;
         char *ipIntfFullPath=NULL;
         UINT32 numPortMappings=0;

         getIpIntfFullPathFromInstanceNum(iidStack.instance[0], &ipIntfFullPath);

         while (cmsObj_getNextFlags(MDMOID_DEV2_NAT_PORT_MAPPING, &pmapIidStack,
                                    OGF_NO_VALUE_UPDATE,
                                    (void **) &pmapObj) == CMSRET_SUCCESS)
         {
            if (!cmsUtl_strcmp(pmapObj->interface, ipIntfFullPath))
            {
               numPortMappings++;
            }

            cmsObj_free((void **) &pmapObj);
         }

         sprintf(tmpbuf,"%d", numPortMappings);
         setsvcval_by_name(psvc,"PortMappingNumberOfEntries",tmpbuf);

         CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
      }
   }
   else
   {
      cmsLog_error("mode %d is not yet supported in TR181");
   }

   cmsObj_free((void **) &ipIntfObj);

   return;
}


void WANCommonInterface_UpdateVars_dev2(PService psvc)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   CmsRet ret;
   char tmpbuf[BUFLEN_32]={0};
   char l2IntfNameBuf[BUFLEN_32]={0};
   char *l2FullPath=NULL;
   MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
   UBOOL8 found = FALSE;

   PUSH_INSTANCE_ID(&iidStack, psvc->device->instance );/*instanceOfWANDevice */
   cmsLog_debug("Entered: Using instance %d", psvc->device->instance );

   ret = cmsObj_get(MDMOID_DEV2_IP_INTERFACE, &iidStack, 0, (void **)&ipIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get IP.Interface obj at %s, ret=%d",
                   cmsMdm_dumpIidStack(&iidStack), ret);
      return;
   }

   /* In TR181, all the required variables are in the Layer 2 object */
   ret = qdmIpIntf_getLayer2IntfNameByLayer3IntfNameLocked(ipIntfObj->name, l2IntfNameBuf);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not convert %s to L2 IntfName", ipIntfObj->name);
      cmsObj_free((void **) &ipIntfObj);
      return;
   }
   else
   {
      cmsLog_debug("%s => %s", ipIntfObj->name, l2IntfNameBuf);
   }
   cmsObj_free((void **) &ipIntfObj);


   /* convert L2IntfNameBuf to fullpath */
   ret = qdmIntf_intfnameToFullPathLocked(l2IntfNameBuf, TRUE, &l2FullPath);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get fullpath for %s", l2IntfNameBuf);
      return;
   }

   /* convert fullpath to pathdesc for the OID */
   cmsMdm_fullPathToPathDescriptor(l2FullPath, &pathDesc);

   cmsLog_debug("got OID=%d", pathDesc.oid);
#ifdef DMP_DEVICE2_ATMLINK_1
   if (pathDesc.oid == MDMOID_DEV2_ATM_LINK)
   {
      Dev2AtmLinkStatsObject *atmStatsObj=NULL;
      CmsRet r2;

      found = TRUE;
      setsvcval_by_name(psvc,"WANAccessType", "DSL");

      r2 = cmsObj_get(MDMOID_DEV2_ATM_LINK_STATS, &pathDesc.iidStack, 0, (void **)&atmStatsObj);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get ATM stats obj, r2=%d", r2);
      }
      else
      {
         MdmPathDescriptor l1PathDesc=EMPTY_PATH_DESCRIPTOR;
         Dev2DslLineObject *dslLineObj = NULL;

         ret = qdmIntf_getBottomLayerPathDescLocked_dev2(l2FullPath, &l1PathDesc, MDMOID_DEV2_DSL_LINE);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not get DSLLine object from %s, ret=%d", l2FullPath, ret);
            return;
         }
         else
         {
            ret = cmsObj_get(MDMOID_DEV2_DSL_LINE, &l1PathDesc.iidStack, OGF_NO_VALUE_UPDATE, (void **)&dslLineObj);
            if ( ret != CMSRET_SUCCESS)
            {
               setsvcval_by_name(psvc,"Layer1UpstreamMaxBitRate", "1500000");
               setsvcval_by_name(psvc,"Layer1DownstreamMaxBitRate", "20000000");
            }
            else
            {
               sprintf(tmpbuf, "%lu", (ulong)dslLineObj->upstreamMaxBitRate * 1000);
               setsvcval_by_name(psvc,"Layer1UpstreamMaxBitRate", tmpbuf);
               sprintf(tmpbuf, "%lu", (ulong)dslLineObj->downstreamMaxBitRate * 1000);
               setsvcval_by_name(psvc,"Layer1DownstreamMaxBitRate", tmpbuf);
               cmsObj_free((void **)&dslLineObj);
            }
         }

         /* assume it is UP, otherwise, UPnP will not be running on it */
         setsvcval_by_name(psvc,"PhysicalLinkStatus", MDMVS_UP);

         sprintf(tmpbuf,"%lu",(ulong)atmStatsObj->bytesSent);
         setsvcval_by_name(psvc,"TotalBytesSent",tmpbuf);

         sprintf(tmpbuf,"%lu",(ulong)atmStatsObj->bytesReceived);
         setsvcval_by_name(psvc,"TotalBytesReceived",tmpbuf);

         sprintf(tmpbuf,"%lu",(ulong)atmStatsObj->packetsSent);
         setsvcval_by_name(psvc,"TotalPacketsSent",tmpbuf);

         sprintf(tmpbuf,"%lu",(ulong)atmStatsObj->packetsReceived);
         setsvcval_by_name(psvc,"TotalPacketsReceived",tmpbuf);

         cmsObj_free((void **) &atmStatsObj);
      }
   }
#endif /* DMP_DEVICE2_ATMLINK_1 */
#ifdef DMP_DEVICE2_PTMLINK_1
   if (!found && (pathDesc.oid == MDMOID_DEV2_PTM_LINK))
   {
      Dev2PtmLinkStatsObject *ptmStatsObj=NULL;
      CmsRet r2;

      found = TRUE;
      setsvcval_by_name(psvc,"WANAccessType", "DSL");

      r2 = cmsObj_get(MDMOID_DEV2_PTM_LINK_STATS, &pathDesc.iidStack, 0, (void **)&ptmStatsObj);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get Ptm stats obj, r2=%d", r2);
      }
      else
      {
         MdmPathDescriptor l1PathDesc=EMPTY_PATH_DESCRIPTOR;
         Dev2DslLineObject *dslLineObj = NULL;

         ret = qdmIntf_getBottomLayerPathDescLocked_dev2(l2FullPath, &l1PathDesc, MDMOID_DEV2_DSL_LINE);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not get DSLLine object from %s, ret=%d", l2FullPath, ret);
            return;
         }
         else
         {
            ret = cmsObj_get(MDMOID_DEV2_DSL_LINE, &l1PathDesc.iidStack, OGF_NO_VALUE_UPDATE, (void **)&dslLineObj);
            if ( ret != CMSRET_SUCCESS)
            {
               setsvcval_by_name(psvc,"Layer1UpstreamMaxBitRate", "1500000");
               setsvcval_by_name(psvc,"Layer1DownstreamMaxBitRate", "20000000");
            }
            else
            {
               sprintf(tmpbuf, "%lu", (ulong)dslLineObj->upstreamMaxBitRate*1000);
               setsvcval_by_name(psvc,"Layer1UpstreamMaxBitRate", tmpbuf);
               sprintf(tmpbuf, "%lu", (ulong)dslLineObj->downstreamMaxBitRate*1000);
               setsvcval_by_name(psvc,"Layer1DownstreamMaxBitRate", tmpbuf);
               cmsObj_free((void **)&dslLineObj);
            }
         }

         /* assume it is UP, otherwise, UPnP will not be running on it */
         setsvcval_by_name(psvc,"PhysicalLinkStatus", MDMVS_UP);

         sprintf(tmpbuf,"%lu",(ulong)ptmStatsObj->bytesSent);
         setsvcval_by_name(psvc,"TotalBytesSent",tmpbuf);

         sprintf(tmpbuf,"%lu",(ulong)ptmStatsObj->bytesReceived);
         setsvcval_by_name(psvc,"TotalBytesReceived",tmpbuf);

         sprintf(tmpbuf,"%lu",(ulong)ptmStatsObj->packetsSent);
         setsvcval_by_name(psvc,"TotalPacketsSent",tmpbuf);

         sprintf(tmpbuf,"%lu",(ulong)ptmStatsObj->packetsReceived);
         setsvcval_by_name(psvc,"TotalPacketsReceived",tmpbuf);

         cmsObj_free((void **) &ptmStatsObj);
      }
   }
#endif /* #ifdef DMP_DEVICE2_ATMLINK_1*/
#ifdef DMP_DEVICE2_OPTICAL_1
   if (!found && (pathDesc.oid == MDMOID_OPTICAL_INTERFACE))
   {
      OpticalInterfaceObject *optIntfObj = NULL;
      OpticalInterfaceStatsObject *optStatsObj = NULL;
      CmsRet r2;

      found = TRUE;
      setsvcval_by_name(psvc,"WANAccessType", "Optical");

      r2 = cmsObj_get(MDMOID_OPTICAL_INTERFACE, &pathDesc.iidStack, 0, (void **)&optIntfObj);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get optical obj, r2=%d", r2);
      }
      else
      {
         setsvcval_by_name(psvc,"PhysicalLinkStatus", optIntfObj->status);

         cmsObj_free((void **) &optIntfObj);
      }

      r2 = cmsObj_get(MDMOID_OPTICAL_INTERFACE_STATS, &pathDesc.iidStack, 0, (void **)&optStatsObj);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get optical stats obj, r2=%d", r2);
      }
      else
      {
         sprintf(tmpbuf,"%lu",(ulong)optStatsObj->bytesSent);
         setsvcval_by_name(psvc,"TotalBytesSent",tmpbuf);

         sprintf(tmpbuf,"%lu",(ulong)optStatsObj->bytesReceived);
         setsvcval_by_name(psvc,"TotalBytesReceived",tmpbuf);

         sprintf(tmpbuf,"%lu",(ulong)optStatsObj->packetsSent);
         setsvcval_by_name(psvc,"TotalPacketsSent",tmpbuf);

         sprintf(tmpbuf,"%lu",(ulong)optStatsObj->packetsReceived);
         setsvcval_by_name(psvc,"TotalPacketsReceived",tmpbuf);

         cmsObj_free((void **) &optStatsObj);
      }
   }
#endif /* #ifdef DMP_DEVICE2_OPTICAL_1 */
   if (!found && (pathDesc.oid == MDMOID_DEV2_ETHERNET_INTERFACE))
   {
      Dev2EthernetInterfaceObject *ethIntfObj=NULL;
      Dev2EthernetInterfaceStatsObject *ethStatsObj=NULL;
      CmsRet r2;

      found = TRUE;
      setsvcval_by_name(psvc,"WANAccessType", "Ethernet");

      r2 = cmsObj_get(MDMOID_DEV2_ETHERNET_INTERFACE, &pathDesc.iidStack, 0, (void **)&ethIntfObj);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get eth stats obj, r2=%d", r2);
      }
      else
      {
         sprintf(tmpbuf,"%lu",(ulong)ethIntfObj->maxBitRate);
         setsvcval_by_name(psvc,"Layer1UpstreamMaxBitRate", tmpbuf);
         setsvcval_by_name(psvc,"Layer1DownstreamMaxBitRate", tmpbuf);

         setsvcval_by_name(psvc,"PhysicalLinkStatus", ethIntfObj->status);

         cmsObj_free((void **) &ethIntfObj);
      }

      r2 = cmsObj_get(MDMOID_DEV2_ETHERNET_INTERFACE_STATS, &pathDesc.iidStack, 0, (void **)&ethStatsObj);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get eth stats obj, r2=%d", r2);
      }
      else
      {
         sprintf(tmpbuf,"%lu",(ulong)ethStatsObj->bytesSent);
         setsvcval_by_name(psvc,"TotalBytesSent",tmpbuf);

         sprintf(tmpbuf,"%lu",(ulong)ethStatsObj->bytesReceived);
         setsvcval_by_name(psvc,"TotalBytesReceived",tmpbuf);

         sprintf(tmpbuf,"%lu",(ulong)ethStatsObj->packetsSent);
         setsvcval_by_name(psvc,"TotalPacketsSent",tmpbuf);

         sprintf(tmpbuf,"%lu",(ulong)ethStatsObj->packetsReceived);
         setsvcval_by_name(psvc,"TotalPacketsReceived",tmpbuf);

         cmsObj_free((void **) &ethStatsObj);
      }
   }

   if (!found)
   {
      cmsLog_error("Unsupported oid %d (%s)", pathDesc.oid, l2FullPath);
   }


   CMSMEM_FREE_BUF_AND_NULL_PTR(l2FullPath);
   return;
}


extern char *g_devMfr;
extern char *g_devMdlDesc;
extern char *g_devMdlName;
extern char *g_devSerialNum;

int upnp_getDeviceInfo_dev2()
{
   int errorInfo = 0;
   CmsRet ret;
   Dev2DeviceInfoObject *deviceInfoObj= NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* caller has already acquired the CMS lock */

   if ((ret = cmsObj_get(MDMOID_DEV2_DEVICE_INFO, &iidStack, 0, (void **) &deviceInfoObj)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(g_devMfr,deviceInfoObj->manufacturer);
      CMSMEM_REPLACE_STRING(g_devMdlName,deviceInfoObj->modelName);
      CMSMEM_REPLACE_STRING(g_devMdlDesc,deviceInfoObj->description);
      CMSMEM_REPLACE_STRING(g_devSerialNum,deviceInfoObj->serialNumber);

      cmsObj_free((void **) &deviceInfoObj);
   }
   else
   {
      cmsLog_error("Could not get DeviceInfo, ret=%d", ret);
      errorInfo = SOAP_ACTION_FAILED;
   }

   return errorInfo;
}


int upnp_setConnectionType_dev2(UBOOL8 isPpp __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                const char *connectionType __attribute__((unused)))
{
   cmsLog_error("Setting connectionType is not supported in TR181 mode");
   return 0;
}


int upnp_updateOrAddPortMapping_dev2(UBOOL8 isPpp __attribute__((unused)),
              const InstanceIdStack *parentIidStack,
              UBOOL8 portMappingEnabled, UINT32 leaseDuration,
              const char *protocol, UINT16 externalPort, UINT16 internalPort,
              const char *remoteHost, const char *internalClient, char *description)
{
   int errorInfo=0;
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2NatPortMappingObject *pmapObj=NULL;
   CmsRet ret;
   char *ipIntfFullPath=NULL;

   if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return SOAP_ACTION_FAILED;
   }

   cmsLog_debug("Entered: Using %d", parentIidStack->instance[0]);

   if (getIpIntfFullPathFromInstanceNum(parentIidStack->instance[0], &ipIntfFullPath) != CMSRET_SUCCESS)
   {
      cmsLck_releaseLock();
      return SOAP_ACTION_FAILED;
   }

   /* Look for existing portmapping entry */
   while (!found &&
          (cmsObj_getNext(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack,
                          (void **) &pmapObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(pmapObj->interface, ipIntfFullPath) &&
          (pmapObj->externalPort == externalPort) &&
          !cmsUtl_strcmp(pmapObj->protocol, protocol))
      {
          found = TRUE;

          /*check if it is for same internal client and
           * and also update entry only if is aaded by upnp previously */
          if ((pmapObj->internalPort == internalPort) &&
              !cmsUtl_strcmp(pmapObj->internalClient, internalClient) &&
              !cmsUtl_strcmp(pmapObj->remoteHost, remoteHost) &&
              !cmsUtl_strcmp(pmapObj->X_BROADCOM_COM_AppName, g_upnpAppName))
          {
             /* update existing entry */
             pmapObj->leaseDuration = leaseDuration;
             pmapObj->enable = portMappingEnabled;
             REPLACE_STRING_IF_NOT_EQUAL(pmapObj->description, description);

             ret = cmsObj_set(pmapObj, &iidStack);
             if (ret != CMSRET_SUCCESS)
             {
                cmsLog_error("could not set new virtual server entry, ret=%d", ret);
                errorInfo = SOAP_ACTION_FAILED;
             }
          }
          else
          {
             cmsLog_error("Conflicting entry detected for external port %d proto %s",
                          externalPort, protocol);
             errorInfo = SOAP_CONFLICTINMAPPINGENTRY;
          }
      }

      cmsObj_free((void **) &pmapObj);
   }

   if (errorInfo)
   {
      cmsLck_releaseLock();
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
      return errorInfo;
   }

   if (found)
   {
      cmsLck_releaseLock();
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
      return 0;
   }


   /*
    * Existing entry not found, create new entry
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new virtual server entry, ret=%d", ret);
      errorInfo = SOAP_ACTION_FAILED;
      goto Exit_2;
   }

   if((ret =cmsObj_setNonpersistentInstance(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set Non persistent flag of new virtual server entry, ret=%d", ret);
      errorInfo = SOAP_ACTION_FAILED;
      goto Exit_2;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack, 0,
                         (void **) &pmapObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get VirtualServerCfgObject, ret=%d", ret);
      errorInfo = SOAP_ACTION_FAILED;
      goto Exit_2;
   }

   REPLACE_STRING_IF_NOT_EQUAL(pmapObj->remoteHost,remoteHost);
   pmapObj->externalPort = externalPort;
   pmapObj->externalPortEndRange = externalPort;
   REPLACE_STRING_IF_NOT_EQUAL(pmapObj->internalClient,internalClient);
   pmapObj->internalPort = internalPort;
   pmapObj->X_BROADCOM_COM_InternalPortEndRange = internalPort;
   REPLACE_STRING_IF_NOT_EQUAL(pmapObj->protocol,protocol);
   pmapObj->enable = portMappingEnabled;
   pmapObj->leaseDuration = leaseDuration;
   REPLACE_STRING_IF_NOT_EQUAL(pmapObj->description,description);
   REPLACE_STRING_IF_NOT_EQUAL(pmapObj->X_BROADCOM_COM_AppName,g_upnpAppName);
   REPLACE_STRING_IF_NOT_EQUAL(pmapObj->interface, ipIntfFullPath);

   if((ret =cmsObj_set(pmapObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set new virtual server entry, ret=%d", ret);
      errorInfo = SOAP_ACTION_FAILED;
      goto Exit_2;
   }

Exit_2:
   cmsObj_free((void **) &pmapObj);
   cmsLck_releaseLock();
   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

   return errorInfo;
}


int upnp_deletePortMapping_dev2(UBOOL8 isPpp __attribute__((unused)),
            const InstanceIdStack *parentIidStack,
            const char *protocol, UINT16 externalPort, const char *remoteHost,
            char *iaddr)
{
   int errorInfo=0;
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2NatPortMappingObject *pmapObj=NULL;
   CmsRet ret;
   char *ipIntfFullPath=NULL;

   if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return SOAP_ACTION_FAILED;
   }

   cmsLog_debug("Entered: Using %d", parentIidStack->instance[0]);

   if (getIpIntfFullPathFromInstanceNum(parentIidStack->instance[0], &ipIntfFullPath) != CMSRET_SUCCESS)
   {
      cmsLck_releaseLock();
      return SOAP_ACTION_FAILED;
   }

   /* Look for portmapping entry */
   while (!found &&
          (cmsObj_getNext(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack,
                          (void **) &pmapObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(pmapObj->interface, ipIntfFullPath) &&
          (pmapObj->externalPort == externalPort) &&
          !cmsUtl_strcmp(pmapObj->protocol, protocol) &&
          !cmsUtl_strcmp(pmapObj->remoteHost, remoteHost))
      {
         found = TRUE;

         if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not delete virtual server entry, ret=%d", ret);
            errorInfo = SOAP_ACTION_FAILED;
         }

         /*suresh TODO: update local state variables */
      }
      cmsObj_free((void **) &pmapObj);
   }

   cmsLck_releaseLock();
   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

   return errorInfo;
}


int upnp_getGenericPortMapping_dev2(UBOOL8 isPpp __attribute__((unused)),
                                    const InstanceIdStack *parentIidStack,
                                    int portMappingIndex, PAction ac)
{
   int errorinfo=0;
   int localIndex =0;
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2NatPortMappingObject *pmapObj=NULL;
   CmsRet ret;
   char *ipIntfFullPath=NULL;

   if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return SOAP_ACTION_FAILED;
   }

   cmsLog_debug("Entered: Using %d", parentIidStack->instance[0]);

   if (getIpIntfFullPathFromInstanceNum(parentIidStack->instance[0], &ipIntfFullPath) != CMSRET_SUCCESS)
   {
      cmsLck_releaseLock();
      return SOAP_ACTION_FAILED;
   }

   /* Look for portmapping entry */
   while (!found &&
          (cmsObj_getNext(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack,
                          (void **) &pmapObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(pmapObj->interface, ipIntfFullPath))
      {
         if (localIndex == portMappingIndex)
         {
            found = TRUE;

            errorinfo |= OutputCharValueToAC(ac, "NewRemoteHost", pmapObj->remoteHost);
            errorinfo |= OutputNumValueToAC(ac, "NewExternalPort", pmapObj->externalPort);
            errorinfo |= OutputNumValueToAC(ac, "NewInternalPort", pmapObj->internalPort);
            errorinfo |= OutputCharValueToAC(ac, "NewInternalClient", pmapObj->internalClient);
            errorinfo |= OutputCharValueToAC(ac, "NewProtocol",  pmapObj->protocol);
            errorinfo |= OutputCharValueToAC(ac, "NewPortMappingDescription", pmapObj->description);
            errorinfo |= OutputNumValueToAC(ac, "NewLeaseDuration", pmapObj->leaseDuration);
            errorinfo |= OutputCharValueToAC(ac, "NewEnabled",
                                               (pmapObj->enable ? "1" : "0"));
         }

         /* As instance Id need not be sequential(due to add/delete)
          * we maintain our own index incrementing by 1 for each portmapping
          * found for this particular WAN serivce
          */
         localIndex++;
      }
      cmsObj_free((void **) &pmapObj);
   }

   cmsLck_releaseLock();
   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

   if(!found) {
      return SOAP_SPECIFIEDARRAYINDEXINVALID;
   }

   return errorinfo;
}


int upnp_getSpecificPortMapping_dev2(UBOOL8 isPpp __attribute__((unused)),
            const InstanceIdStack *parentIidStack,
            const char *protocol, UINT16 externalPort, const char *remoteHost,
            PAction ac)
{
   int errorinfo=0;
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2NatPortMappingObject *pmapObj=NULL;
   CmsRet ret;
   char *ipIntfFullPath=NULL;

   if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return SOAP_ACTION_FAILED;
   }

   cmsLog_debug("Entered: Using %d", parentIidStack->instance[0]);

   if (getIpIntfFullPathFromInstanceNum(parentIidStack->instance[0], &ipIntfFullPath) != CMSRET_SUCCESS)
   {
      cmsLck_releaseLock();
      return SOAP_ACTION_FAILED;
   }

   /* Look for portmapping entry */
   while (!found &&
          (cmsObj_getNext(MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack,
                          (void **) &pmapObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(pmapObj->interface, ipIntfFullPath) &&
          (pmapObj->externalPort == externalPort) &&
          !cmsUtl_strcmp(pmapObj->protocol, protocol) &&
          !cmsUtl_strcmp(pmapObj->remoteHost, remoteHost))
      {
         found = TRUE;

         errorinfo |= OutputNumValueToAC(ac, "NewInternalPort", pmapObj->internalPort);
         errorinfo |= OutputCharValueToAC(ac, "NewInternalClient", pmapObj->internalClient);
         errorinfo |= OutputCharValueToAC(ac, "NewPortMappingDescription", pmapObj->description);
         errorinfo |= OutputNumValueToAC(ac, "NewLeaseDuration", pmapObj->leaseDuration);
         errorinfo |= OutputCharValueToAC(ac, "NewEnabled",
                                              (pmapObj->enable ? "1" : "0"));
      }
      cmsObj_free((void **) &pmapObj);
   }

   cmsLck_releaseLock();
   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

   return errorinfo;
}


#endif  /* DMP_DEVICE2_BASELINE_1 */
