/*
 *
 * <:copyright-BRCM:2012:proprietary:standard 
 * 
 *    Copyright (c) 2012 Broadcom 
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
 * :>
 *
 * $Id: igd.c,v 1.12.20.2 2003/10/31 21:31:35 mthawani Exp $
 */

#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "tr64defs.h"
#include "device.h"
#include <signal.h>

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

int igd_config_generation = 0;

extern PDevice root_devices ;

extern DeviceTemplate LANDeviceTemplate;
extern DeviceTemplate WANDeviceTemplate;
extern DeviceTemplate IGDeviceTemplate;
extern DeviceTemplate subdevs_wandevice;

extern PServiceTemplate svcs_wanconnection;

#ifdef INCLUDE_WANDSLINTERFACE
extern ServiceTemplate Template_WANDSLInterfaceConfig;
#endif
#ifdef INCLUDE_DSLLINKCONFIG
extern ServiceTemplate Template_WANDSLLinkConfig;
#endif
#ifdef INCLUDE_IPCONNECTION
extern ServiceTemplate Template_WANIPConnection;
#endif
#ifdef INCLUDE_PPPCONNECTION
extern ServiceTemplate Template_WANPPPConnection;
#endif
#ifdef INCLUDE_ATMF5LOOPBACKDIAG
extern ServiceTemplate Template_WANATMF5LoopbackDiagnostics;
#endif

#ifdef INCLUDE_LANHOSTCONFIGMGMT
extern ServiceTemplate Template_LANHostConfigManagement;
#endif
#ifdef INCLUDE_LANETHINTERFACECONFIG
extern ServiceTemplate Template_LANEthernetInterfaceConfig;
#endif
#ifdef INCLUDE_WLANCONFIG
extern ServiceTemplate Template_WLANConfig;
#endif
#ifdef INCLUDE_LANUSBINTERFACECONFIG
extern ServiceTemplate Template_LANUSBInterfaceConfig;
#endif
#ifdef INCLUDE_LANHOSTS
extern ServiceTemplate Template_LANHosts;
#endif
#ifdef INCLUDE_WANETHERNETCONFIG
#ifdef DMP_ETHERNETWAN_1
extern ServiceTemplate Template_WANETHInterfaceConfig;
#endif
#endif
#ifdef INCLUDE_WANDSLCONNECTIONMGMT
extern ServiceTemplate Template_WANDSLConnMgt;
#endif

#ifdef INCLUDE_WANCOMMONINTERFACE
extern ServiceTemplate Template_WANCommonInterfaceConfig; 
#endif

extern void uuidstr_create(char *str, int len);
extern void destroy_device(PDevice);
extern PDevice init_device(PDevice parent, PDeviceTemplate, ...);
PDevice init_wan_device_and_service(PDevice parent, int instanceOfWANDevice, ...);
PDevice init_lan_device_and_service(PDevice parent, int instanceOfLANDevice);
PDevice wan_devices = NULL;
PDevice lan_device = NULL;

void sigusr1_handler(int i);

Error IGDErrors[] = {
   { SOAP_DISCONNECTINPROGRESS, "DisconnectInProgress" },
   { SOAP_INVALIDCONNECTIONTYPE, "InvalidConnectionType" },
   { SOAP_CONNECTIONALREADYTERMNATED, "ConnectionAlreadyTerminated" },
   { SOAP_CONNECTIONNOTCONFIGURED, "ConnectionNotConfigured" },
   { SOAP_SPECIFIEDARRAYINDEXINVALID, "SpecifiedArrayIndexInvalid" },
   { SOAP_NOSUCHENTRYINARRAY, "NoSuchEntryInArray" },
   { SOAP_CONFLICTINMAPPINGENTRY, "ConflictInMappingEntry" },
   { SOAP_ONLYPERMANENTLEASESSUPPORTED, "OnlyPermanentLeasesSupported" },
   { 0, NULL }
};

const char *itoa(int i)
{
   static char buf[256];
   sprintf(buf, "%d", i);
   return buf;
}

void init_static_igd_devices(void)
{
   /* recursively init all the devices in local memory */
   PDeviceTemplate pdevtmpl = NULL;
   pdevtmpl = (struct DeviceTemplate *) malloc(sizeof(struct DeviceTemplate));
   memset(pdevtmpl, 0, sizeof(DeviceTemplate));

   pdevtmpl->type = strdup(IGDeviceTemplate.type);
   pdevtmpl->udn= strdup(IGDeviceTemplate.udn);
   pdevtmpl->devinit = IGDeviceTemplate.devinit;
   pdevtmpl->devxml = IGDeviceTemplate.devxml;
   pdevtmpl->nservices = IGDeviceTemplate.nservices;
   pdevtmpl->services = IGDeviceTemplate.services;
   pdevtmpl->ndevices = IGDeviceTemplate.ndevices;
   pdevtmpl->devicelist = IGDeviceTemplate.devicelist;
   pdevtmpl->schema = IGDeviceTemplate.schema;
   if (pdevtmpl->schema == NULL)
   {
      pdevtmpl->schema = TR64_DSLFORUM_SCHEMA;
   }

   init_device(NULL, pdevtmpl);
}

/** Initialize IGD, LAN device, WAN device under IGD 
 */
int IGDevice_Init(PDevice igdev, device_state_t state, va_list ap)
{
   PDevice subdev;
   PDevice landev;
   PDevice wandev;

   LanDevObject *lanDevObj=NULL;
   InstanceIdStack lanDevIidStack = EMPTY_INSTANCE_ID_STACK;
   WanDevObject *wanDevObj=NULL;
   InstanceIdStack wanDevIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   

   switch (state)
   {
   case DEVICE_CREATE:
      soap_register_errors(IGDErrors);
      igdev->friendlyname = strdup(TR64_ROOT_FRIENDLY_NAME);


      if ((ret = cmsLck_acquireLockWithTimeout(TR64C_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get lock, ret=%d", ret);
         return FALSE;
      }

      cmsLog_debug("Calling init_device for Multi LANDevice\n");
      while (cmsObj_getNext(MDMOID_LAN_DEV, &lanDevIidStack, (void **)&lanDevObj) == CMSRET_SUCCESS)
      {          
         cmsObj_free((void **) &lanDevObj);
         
         char tmpBuf[64];
         memset(tmpBuf, 0, sizeof(tmpBuf));

         PDeviceTemplate pdevtmpl = NULL;
         pdevtmpl = (struct DeviceTemplate *) malloc(sizeof(struct DeviceTemplate));
         memset(pdevtmpl, 0, sizeof(DeviceTemplate));

         pdevtmpl->type = strdup(LANDeviceTemplate.type);
         pdevtmpl->udn= strdup(LANDeviceTemplate.udn);
         pdevtmpl->devinit = LANDeviceTemplate.devinit;
         pdevtmpl->devxml = LANDeviceTemplate.devxml;
         pdevtmpl->nservices = LANDeviceTemplate.nservices;
         pdevtmpl->services = LANDeviceTemplate.services;
         pdevtmpl->ndevices = LANDeviceTemplate.ndevices;
         pdevtmpl->devicelist = LANDeviceTemplate.devicelist;
         pdevtmpl->schema = LANDeviceTemplate.schema;
         if (pdevtmpl->schema == NULL)
         {
            pdevtmpl->schema = TR64_DSLFORUM_SCHEMA;
         }

         lan_device = landev = init_device(igdev, pdevtmpl);

         sprintf(tmpBuf, "LANDevice.%d", lanDevIidStack.instance[lanDevIidStack.currentDepth-1]);
         landev->friendlyname = strdup(tmpBuf);
         landev->next = igdev->subdevs;
         landev->instance = lanDevIidStack.instance[lanDevIidStack.currentDepth-1];
         igdev->subdevs = landev;

         subdev = init_lan_device_and_service(landev, landev->instance);
      } 
  
      cmsLog_debug("Calling init_device for Multi WANDevice\n");
      while (cmsObj_getNext(MDMOID_WAN_DEV, &wanDevIidStack, (void **)&wanDevObj) == CMSRET_SUCCESS)
      {          
         cmsObj_free((void **) &wanDevObj);
         PDevice pdev;
         PDeviceTemplate pdevtmpl = NULL;
         pdevtmpl = (struct DeviceTemplate *) malloc(sizeof(struct DeviceTemplate));
         memset(pdevtmpl, 0, sizeof(DeviceTemplate));

         pdevtmpl->type = (char *)malloc(strlen(WANDeviceTemplate.type) + strlen(itoa(wanDevIidStack.instance[wanDevIidStack.currentDepth-1])) + 1 );
         sprintf(pdevtmpl->type,"%s%d",WANDeviceTemplate.type, wanDevIidStack.instance[wanDevIidStack.currentDepth-1]);

         pdevtmpl->udn = (char *)malloc(strlen(WANDeviceTemplate.udn) + strlen(itoa(wanDevIidStack.instance[wanDevIidStack.currentDepth-1])) + 1);
         sprintf(pdevtmpl->udn,"%s%d",WANDeviceTemplate.udn, wanDevIidStack.instance[wanDevIidStack.currentDepth-1]);

         pdevtmpl->devinit = WANDeviceTemplate.devinit;
         pdevtmpl->devxml = WANDeviceTemplate.devxml;
         pdevtmpl->nservices = WANDeviceTemplate.nservices;
         pdevtmpl->services = WANDeviceTemplate.services;
         pdevtmpl->ndevices = WANDeviceTemplate.ndevices;
         pdevtmpl->devicelist = WANDeviceTemplate.devicelist;
         pdevtmpl->schema = WANDeviceTemplate.schema;
         if (pdevtmpl->schema == NULL)
         {
            pdevtmpl->schema = TR64_DSLFORUM_SCHEMA;
         }
         pdev = (Device *) malloc(sizeof(Device));
         memset(pdev, 0, sizeof(Device));

         pdev->instance = wanDevIidStack.instance[wanDevIidStack.currentDepth-1];            
         /* Add 1 for \0 */
         pdev->friendlyname = (char *)malloc(strlen("WanDevice.") + strlen(itoa(wanDevIidStack.instance[wanDevIidStack.currentDepth-1])) + 1);
         sprintf(pdev->friendlyname,"WanDevice.%d", wanDevIidStack.instance[wanDevIidStack.currentDepth-1]);

         pdev->parent = igdev;
         pdev->template = pdevtmpl;

         pdev->next = igdev->subdevs;
         igdev->subdevs = pdev;
         wandev = pdev;
         wan_devices = pdev;
         subdev = init_wan_device_and_service(wandev , wanDevIidStack.instance[wanDevIidStack.currentDepth-1] );
      } 

      cmsLck_releaseLock();
      break;

   case DEVICE_DESTROY:
      break;
   }

   return TRUE;
}

void regenerate_uuids(void)
{
   char *udn;
   PDevice pdev;

   forall_devices(pdev) 
   {
      if(pdev->udn)
      {
         free(pdev->udn);
      }

      udn = malloc(50);
      strcpy(udn, "uuid:");
      cmsUtl_generateUuidStrFromRandom(udn+5, 50 - 5);
    
      /** the device definition contains a unique string that should be substituted whenever 
       * the http server sends out the XML description document.
       */
      pdev->udn = udn;
   }
}


int WANDevice_ReInit(void)
{

   PDevice subdev;
   PDevice wandev;
   PDevice pdev;
   PDevice pdev_lan;

   WanDevObject *wanDevObj=NULL;
   InstanceIdStack wanDevIidStack = EMPTY_INSTANCE_ID_STACK;
   char *udn = NULL;

   if(root_devices == NULL)
   {
      printf(" WANDevice_ReInit root_devices is null\n");
      return FALSE;
   }

   udn = root_devices->udn;

#if 0
   if(wan_devices != NULL)
   {
      destroy_device(wan_devices);   
      root_devices->subdevs = lan_device;
      wan_devices = NULL;
   }
#else
   while (wan_devices != NULL)
   {
      pdev = wan_devices;
      pdev_lan = lan_device;
      wan_devices = pdev->next;

      while (pdev_lan != NULL && pdev != pdev_lan)
      {
         if(!pdev_lan->next)
            destroy_device(pdev);
         pdev_lan = pdev_lan->next;
      }
   }
   root_devices->subdevs = lan_device;
#endif

   while (cmsObj_getNext(MDMOID_WAN_DEV, &wanDevIidStack, (void **)&wanDevObj) == CMSRET_SUCCESS)
   {          
      cmsObj_free((void **) &wanDevObj);
#if 0
      char tmpBuf[64];
      memset(tmpBuf, 0, sizeof(tmpBuf));

      wan_devices = wandev = init_device(root_devices, &WANDeviceTemplate);
      sprintf(tmpBuf, "WANDevice.%d", wanDevIidStack.instance[wanDevIidStack.currentDepth-1]);
      wandev->friendlyname = strdup(tmpBuf);
      wandev->next = root_devices->subdevs;
      wandev->instance = wanDevIidStack.instance[wanDevIidStack.currentDepth-1];;
      root_devices->subdevs = wandev;
#else
      PDevice pdev;
      PDeviceTemplate pdevtmpl = NULL;
      pdevtmpl = (struct DeviceTemplate *) malloc(sizeof(struct DeviceTemplate));
      memset(pdevtmpl, 0, sizeof(DeviceTemplate));

      pdevtmpl->type = (char *)malloc(strlen(WANDeviceTemplate.type) + strlen(itoa(wanDevIidStack.instance[wanDevIidStack.currentDepth-1])) + 1 );
      sprintf(pdevtmpl->type,"%s%d",WANDeviceTemplate.type, wanDevIidStack.instance[wanDevIidStack.currentDepth-1]);

      pdevtmpl->udn = (char *)malloc(strlen(WANDeviceTemplate.udn) + strlen(itoa(wanDevIidStack.instance[wanDevIidStack.currentDepth-1])) + 1);
      sprintf(pdevtmpl->udn,"%s%d",WANDeviceTemplate.udn, wanDevIidStack.instance[wanDevIidStack.currentDepth-1]);

      pdevtmpl->devinit = WANDeviceTemplate.devinit;
      pdevtmpl->devxml = WANDeviceTemplate.devxml;
      pdevtmpl->nservices = WANDeviceTemplate.nservices;
      pdevtmpl->services = WANDeviceTemplate.services;
      pdevtmpl->ndevices = WANDeviceTemplate.ndevices;
      pdevtmpl->devicelist = WANDeviceTemplate.devicelist;
      pdevtmpl->schema = WANDeviceTemplate.schema;
      if (pdevtmpl->schema == NULL)
      {
         pdevtmpl->schema = TR64_DSLFORUM_SCHEMA;
      }
      pdev = (Device *) malloc(sizeof(Device));
      memset(pdev, 0, sizeof(Device));

      pdev->instance = wanDevIidStack.instance[wanDevIidStack.currentDepth-1];            
      pdev->friendlyname = (char *)malloc(strlen("WanDevice.") + strlen(itoa(wanDevIidStack.instance[wanDevIidStack.currentDepth-1])) + 1);
      sprintf(pdev->friendlyname,"WanDevice.%d", wanDevIidStack.instance[wanDevIidStack.currentDepth-1]);

      pdev->parent = root_devices;
      pdev->template = pdevtmpl;
      pdev->next = root_devices->subdevs;
      root_devices->subdevs = pdev;
      wandev = pdev;
      wan_devices = pdev;

#endif
      subdev = init_wan_device_and_service(wandev , wanDevIidStack.instance[wanDevIidStack.currentDepth-1] );
   }

   regenerate_uuids();

   return TRUE;
}

PDevice init_lan_device_and_service(PDevice parent, int instanceOfLANDevice)
{
   InstanceIdStack lanDevIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   LanDevObject *lanDevObj = NULL; 
   LanHostCfgObject *lanHostCfgObj = NULL;
   LanEthIntfObject *lanEthIntfObj = NULL;
   LanWlanObject *lanWlanObj = NULL;
   LanHostsObject *lanHostsObj = NULL;
#ifdef USB
   LanUsbIntfObject *lanUsbObj = NULL;
#endif


   PDevice pdev = parent;
   PService psvc = NULL;

   while (cmsObj_getNext(MDMOID_LAN_DEV, &lanDevIidStack, (void **)&lanDevObj) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &lanDevObj);

      if(instanceOfLANDevice == lanDevIidStack.instance[lanDevIidStack.currentDepth-1])
      {
         /*ADD InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.*/
#ifdef INCLUDE_LANHOSTCONFIGMGMT
         INIT_INSTANCE_ID_STACK(&iidStack);
         if (cmsObj_get(MDMOID_LAN_HOST_CFG, &lanDevIidStack, 0, (void **)&lanHostCfgObj) == CMSRET_SUCCESS)
         {
            struct ServiceTemplate *pTemplate_LANHostCfg = NULL;
            pTemplate_LANHostCfg= (struct ServiceTemplate *) malloc(sizeof(struct ServiceTemplate));
            if (pTemplate_LANHostCfg == NULL) 
            {
               cmsLog_error("malloc failed");
            }else{
               memset(pTemplate_LANHostCfg, 0, sizeof(ServiceTemplate));
               pTemplate_LANHostCfg->name = (char *)malloc(strlen(Template_LANHostConfigManagement.name)+ strlen(itoa(1)) + 1);
               sprintf(pTemplate_LANHostCfg->name,"%s%d",Template_LANHostConfigManagement.name, 1);
               pTemplate_LANHostCfg->svcinit = Template_LANHostConfigManagement.svcinit;
               pTemplate_LANHostCfg->getvars = Template_LANHostConfigManagement.getvars;
               pTemplate_LANHostCfg->svcxml = Template_LANHostConfigManagement.svcxml;
               pTemplate_LANHostCfg->nvariables = Template_LANHostConfigManagement.nvariables;
               pTemplate_LANHostCfg->variables = Template_LANHostConfigManagement.variables;
               pTemplate_LANHostCfg->actions = Template_LANHostConfigManagement.actions;
               pTemplate_LANHostCfg->count = Template_LANHostConfigManagement.count;
               pTemplate_LANHostCfg->serviceid = Template_LANHostConfigManagement.serviceid; 
               pTemplate_LANHostCfg->schema = Template_LANHostConfigManagement.schema;

               psvc = init_service(pTemplate_LANHostCfg, pdev, 1);

               psvc->next = pdev->services;
               pdev->services = psvc;
            }

            cmsObj_free((void **) &lanHostCfgObj);
         }
#endif
         /*ADD InternetGatewayDevice.LANDevice.{i}.LANEthernetInterfaceConfig.{i}.*/
#ifdef INCLUDE_LANETHINTERFACECONFIG
         INIT_INSTANCE_ID_STACK(&iidStack);
         while (cmsObj_getNextInSubTree(MDMOID_LAN_ETH_INTF, &lanDevIidStack, &iidStack, (void **)&lanEthIntfObj) == CMSRET_SUCCESS)
         {
            struct ServiceTemplate *pTemplate_LANEthIntfCfg = NULL;
            pTemplate_LANEthIntfCfg= (struct ServiceTemplate *) malloc(sizeof(struct ServiceTemplate));

            if (pTemplate_LANEthIntfCfg == NULL) 
            {
               cmsLog_error("malloc failed");
            }else{
               memset(pTemplate_LANEthIntfCfg, 0, sizeof(ServiceTemplate));

               pTemplate_LANEthIntfCfg->name = (char *)malloc(strlen(Template_LANEthernetInterfaceConfig.name)+ strlen(itoa(iidStack.instance[iidStack.currentDepth-1])) + 1);
               sprintf(pTemplate_LANEthIntfCfg->name,"%s%d",Template_LANEthernetInterfaceConfig.name, iidStack.instance[iidStack.currentDepth-1]);
               pTemplate_LANEthIntfCfg->svcinit = Template_LANEthernetInterfaceConfig.svcinit;
               pTemplate_LANEthIntfCfg->getvars = Template_LANEthernetInterfaceConfig.getvars;
               pTemplate_LANEthIntfCfg->svcxml = Template_LANEthernetInterfaceConfig.svcxml;
               pTemplate_LANEthIntfCfg->nvariables = Template_LANEthernetInterfaceConfig.nvariables;
               pTemplate_LANEthIntfCfg->variables = Template_LANEthernetInterfaceConfig.variables;
               pTemplate_LANEthIntfCfg->actions = Template_LANEthernetInterfaceConfig.actions;
               pTemplate_LANEthIntfCfg->count = Template_LANEthernetInterfaceConfig.count;
               pTemplate_LANEthIntfCfg->serviceid = Template_LANEthernetInterfaceConfig.serviceid; 
               pTemplate_LANEthIntfCfg->schema = Template_LANEthernetInterfaceConfig.schema;

               psvc = init_service(pTemplate_LANEthIntfCfg, pdev, iidStack.instance[iidStack.currentDepth-1]);
               psvc->next = pdev->services;
               pdev->services = psvc;
            }
 
            cmsObj_free((void **) &lanEthIntfObj);
      }
#endif
         /*ADD InternetGatewayDevice.LANDevice.{i}.WLANConfiguration.{i}.*/
#ifdef INCLUDE_WLANCONFIG
         INIT_INSTANCE_ID_STACK(&iidStack);
         while (cmsObj_getNextInSubTree(MDMOID_LAN_WLAN, &lanDevIidStack, &iidStack, (void **)&lanWlanObj) == CMSRET_SUCCESS)
         {
            struct ServiceTemplate *pTemplate_LANWlan = NULL;
            pTemplate_LANWlan= (struct ServiceTemplate *) malloc(sizeof(struct ServiceTemplate));

            if (pTemplate_LANWlan == NULL) 
            {
               cmsLog_error("malloc failed");
            }else{
               memset(pTemplate_LANWlan, 0, sizeof(ServiceTemplate));

               pTemplate_LANWlan->name = (char *)malloc(strlen(Template_WLANConfig.name)+ strlen(itoa(iidStack.instance[iidStack.currentDepth-1])) + 1);
               sprintf(pTemplate_LANWlan->name,"%s%d",Template_WLANConfig.name, iidStack.instance[iidStack.currentDepth-1]);
               pTemplate_LANWlan->svcinit = Template_WLANConfig.svcinit;
               pTemplate_LANWlan->getvars = Template_WLANConfig.getvars;
               pTemplate_LANWlan->svcxml = Template_WLANConfig.svcxml;
               pTemplate_LANWlan->nvariables = Template_WLANConfig.nvariables;
               pTemplate_LANWlan->variables = Template_WLANConfig.variables;
               pTemplate_LANWlan->actions = Template_WLANConfig.actions;
               pTemplate_LANWlan->count = Template_WLANConfig.count;
               pTemplate_LANWlan->serviceid = Template_WLANConfig.serviceid; 
               pTemplate_LANWlan->schema = Template_WLANConfig.schema;

               psvc = init_service(pTemplate_LANWlan, pdev, iidStack.instance[iidStack.currentDepth-1]);
               psvc->next = pdev->services;
               pdev->services = psvc;
            }
 
            cmsObj_free((void **) &lanWlanObj);
         }
#endif
         /* ADD InternetGatewayDevice.LANDevice.{i}.LANUSBInterfaceConfig.{i}. */
#ifdef INCLUDE_LANUSBINTERFACECONFIG
#ifdef USB
         INIT_INSTANCE_ID_STACK(&iidStack);
         while (cmsObj_getNextInSubTree(MDMOID_LAN_USB_INTF, &lanDevIidStack, &iidStack, (void **)&lanUsbObj) == CMSRET_SUCCESS)
         {
            struct ServiceTemplate *pTemplate_LANUSB = NULL;
            pTemplate_LANUSB= (struct ServiceTemplate *) malloc(sizeof(struct ServiceTemplate));
 
            if (pTemplate_LANUSB == NULL) 
            {
               cmsLog_error("malloc failed");
            }else{
               memset(pTemplate_LANUSB, 0, sizeof(ServiceTemplate));

               pTemplate_LANUSB->name = (char *)malloc(strlen(Template_LANUSBInterfaceConfig.name)+ strlen(itoa(iidStack.instance[iidStack.currentDepth-1])) + 1);
               sprintf(pTemplate_LANUSB->name,"%s%d",Template_LANUSBInterfaceConfig.name, iidStack.instance[iidStack.currentDepth-1]);
               pTemplate_LANUSB->svcinit = Template_LANUSBInterfaceConfig.svcinit;
               pTemplate_LANUSB->getvars = Template_LANUSBInterfaceConfig.getvars;
               pTemplate_LANUSB->svcxml = Template_LANUSBInterfaceConfig.svcxml;
               pTemplate_LANUSB->nvariables = Template_LANUSBInterfaceConfig.nvariables;
               pTemplate_LANUSB->variables = Template_LANUSBInterfaceConfig.variables;
               pTemplate_LANUSB->actions = Template_LANUSBInterfaceConfig.actions;
               pTemplate_LANUSB->count = Template_LANUSBInterfaceConfig.count;
               pTemplate_LANUSB->serviceid = Template_LANUSBInterfaceConfig.serviceid; 
               pTemplate_LANUSB->schema = Template_LANUSBInterfaceConfig.schema;

               psvc = init_service(pTemplate_LANUSB, pdev, iidStack.instance[iidStack.currentDepth-1]);
               psvc->next = pdev->services;
               pdev->services = psvc;
            }
 
            cmsObj_free((void **) &lanUsbObj);
         }
#endif
#endif
         /* ADD InternetGatewayDevice.LANDevice.{i}.Hosts.*/
#ifdef INCLUDE_LANHOSTS
         INIT_INSTANCE_ID_STACK(&iidStack);
         if (cmsObj_get(MDMOID_LAN_HOSTS, &lanDevIidStack, 0, (void **)&lanHostsObj) == CMSRET_SUCCESS)
         {
            struct ServiceTemplate *pTemplate_LANHosts = NULL;
            pTemplate_LANHosts= (struct ServiceTemplate *) malloc(sizeof(struct ServiceTemplate));

            if (pTemplate_LANHosts == NULL) 
            {
               cmsLog_error("malloc failed");
            }else{
               memset(pTemplate_LANHosts, 0, sizeof(ServiceTemplate));

               pTemplate_LANHosts->name = (char *)malloc(strlen(Template_LANHosts.name)+ strlen(itoa(1)) + 1);
               sprintf(pTemplate_LANHosts->name,"%s%d",Template_LANHosts.name, 1);
               pTemplate_LANHosts->svcinit = Template_LANHosts.svcinit;
               pTemplate_LANHosts->getvars = Template_LANHosts.getvars;
               pTemplate_LANHosts->svcxml = Template_LANHosts.svcxml;
               pTemplate_LANHosts->nvariables = Template_LANHosts.nvariables;
               pTemplate_LANHosts->variables = Template_LANHosts.variables;
               pTemplate_LANHosts->actions = Template_LANHosts.actions;
               pTemplate_LANHosts->count = Template_LANHosts.count;
               pTemplate_LANHosts->serviceid = Template_LANHosts.serviceid; 
               pTemplate_LANHosts->schema = Template_LANHosts.schema;

               psvc = init_service(pTemplate_LANHosts, pdev, 1);
               psvc->next = pdev->services;
               pdev->services = psvc;
            }
 
            cmsObj_free((void **) &lanHostsObj);
         }
#endif
       }
    }

    if(pdev!=NULL)
    {
       if (ISROOT(pdev)) 
       {
          pdev->next = root_devices;
          root_devices = pdev;
       }
    }
    return pdev;
}

PDevice init_wan_device_and_service(PDevice parent, int instanceOfWANDevice, ...)
{

   InstanceIdStack iidStack;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;

   WanConnDeviceObject *wanconCfg=NULL;

   WanCommonIntfCfgObject  *comIntf = NULL;

#if defined (INCLUDE_WANDSLINTERFACE) || defined (INCLUDE_WANDSLCONNECTIONMGMT)
   WanDslIntfCfgObject *dslIntfCfg = NULL;
#endif

#ifdef INCLUDE_WANETHERNETCONFIG
#ifdef DMP_ETHERNETWAN_1
   WanEthIntfObject *ethIntf = NULL;
#endif
#endif

#ifdef INCLUDE_DSLLINKCONFIG
   WanDslLinkCfgObject *dslLinkCfg = NULL;   
#endif
#ifdef INCLUDE_ATMF5LOOPBACKDIAG
   WanAtm5LoopbackDiagObject *atm5LoopbackCfg = NULL;
#endif
   WanPppConnObject *wanPppConn = NULL;   
   WanIpConnObject  *wanIpConn = NULL;

   PFDEVINIT func;
   PDevice pdev = NULL;
   PService psvc = NULL;
   va_list ap;

#ifdef INCLUDE_WANCOMMONINTERFACE
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_WAN_COMMON_INTF_CFG, &iidStack, (void **)&comIntf) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &comIntf);
      if(instanceOfWANDevice == iidStack.instance[iidStack.currentDepth-1])
      {
         struct ServiceTemplate *pTemplate_WANCommonInterfaceConfig= NULL;
         pTemplate_WANCommonInterfaceConfig= (struct ServiceTemplate *) malloc(sizeof(struct ServiceTemplate));
  
         if (pTemplate_WANCommonInterfaceConfig == NULL) 
         {
            UPNP_ERROR(("malloc failed in %s\n", __FUNCTION__));
         }
         memset(pTemplate_WANCommonInterfaceConfig, 0, sizeof(ServiceTemplate));

         pTemplate_WANCommonInterfaceConfig->name = (char *)malloc(strlen(Template_WANCommonInterfaceConfig.name)+1);
         sprintf(pTemplate_WANCommonInterfaceConfig->name,"%s",Template_WANCommonInterfaceConfig.name);
         pTemplate_WANCommonInterfaceConfig->svcinit = Template_WANCommonInterfaceConfig.svcinit;
         pTemplate_WANCommonInterfaceConfig->getvars = Template_WANCommonInterfaceConfig.getvars;
         pTemplate_WANCommonInterfaceConfig->svcxml = Template_WANCommonInterfaceConfig.svcxml;
         pTemplate_WANCommonInterfaceConfig->nvariables = Template_WANCommonInterfaceConfig.nvariables;
         pTemplate_WANCommonInterfaceConfig->variables = Template_WANCommonInterfaceConfig.variables;
         pTemplate_WANCommonInterfaceConfig->actions = Template_WANCommonInterfaceConfig.actions;
         pTemplate_WANCommonInterfaceConfig->count = Template_WANCommonInterfaceConfig.count;
         pTemplate_WANCommonInterfaceConfig->serviceid = Template_WANCommonInterfaceConfig.serviceid; 
         pTemplate_WANCommonInterfaceConfig->schema = Template_WANCommonInterfaceConfig.schema;
         psvc = init_service(pTemplate_WANCommonInterfaceConfig, parent, iidStack.instance[iidStack.currentDepth-1]);
         psvc->next = parent->services;
         parent->services = psvc;
      }
   }
#endif

#ifdef INCLUDE_WANDSLINTERFACE
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **)&dslIntfCfg) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &dslIntfCfg);
      if(instanceOfWANDevice == iidStack.instance[iidStack.currentDepth-1])
      {
         struct ServiceTemplate *pTemplate_WANDSLInterfaceConfig= NULL;
         pTemplate_WANDSLInterfaceConfig= (struct ServiceTemplate *) malloc(sizeof(ServiceTemplate));
  
         if (pTemplate_WANDSLInterfaceConfig == NULL) 
         {
            UPNP_ERROR(("malloc failed in %s\n", __FUNCTION__));
         }
         memset(pTemplate_WANDSLInterfaceConfig, 0, sizeof(ServiceTemplate));

         pTemplate_WANDSLInterfaceConfig->name = (char *)malloc(strlen(Template_WANDSLInterfaceConfig.name)+1);
         sprintf(pTemplate_WANDSLInterfaceConfig->name,"%s",Template_WANDSLInterfaceConfig.name);
         pTemplate_WANDSLInterfaceConfig->svcinit = Template_WANDSLInterfaceConfig.svcinit;
         pTemplate_WANDSLInterfaceConfig->getvars = Template_WANDSLInterfaceConfig.getvars;
         pTemplate_WANDSLInterfaceConfig->svcxml = Template_WANDSLInterfaceConfig.svcxml;
         pTemplate_WANDSLInterfaceConfig->nvariables = Template_WANDSLInterfaceConfig.nvariables;
         pTemplate_WANDSLInterfaceConfig->variables = Template_WANDSLInterfaceConfig.variables;
         pTemplate_WANDSLInterfaceConfig->actions = Template_WANDSLInterfaceConfig.actions;
         pTemplate_WANDSLInterfaceConfig->count = Template_WANDSLInterfaceConfig.count;
         pTemplate_WANDSLInterfaceConfig->serviceid = Template_WANDSLInterfaceConfig.serviceid; 
         pTemplate_WANDSLInterfaceConfig->schema = Template_WANDSLInterfaceConfig.schema;
         psvc = init_service(pTemplate_WANDSLInterfaceConfig, parent, iidStack.instance[iidStack.currentDepth-1]);
         psvc->next = parent->services;
         parent->services = psvc;
      }
   }
#endif

#ifdef INCLUDE_WANDSLCONNECTIONMGMT
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **)&dslIntfCfg) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &dslIntfCfg);

      if(instanceOfWANDevice == iidStack.instance[iidStack.currentDepth-1])
      {
         struct ServiceTemplate *pTemplate_WANDSLConnMgt= NULL;
         pTemplate_WANDSLConnMgt= (struct ServiceTemplate *) malloc(sizeof(struct ServiceTemplate));
  
         if (pTemplate_WANDSLConnMgt == NULL) 
         {
            UPNP_ERROR(("malloc failed in %s\n", __FUNCTION__));
         }
         memset(pTemplate_WANDSLConnMgt, 0, sizeof(ServiceTemplate));

         pTemplate_WANDSLConnMgt->name = (char *)malloc(strlen(Template_WANDSLConnMgt.name)+1);
         sprintf(pTemplate_WANDSLConnMgt->name,"%s",Template_WANDSLConnMgt.name);
         pTemplate_WANDSLConnMgt->svcinit = Template_WANDSLConnMgt.svcinit;
         pTemplate_WANDSLConnMgt->getvars = Template_WANDSLConnMgt.getvars;
         pTemplate_WANDSLConnMgt->svcxml = Template_WANDSLConnMgt.svcxml;
         pTemplate_WANDSLConnMgt->nvariables = Template_WANDSLConnMgt.nvariables;
         pTemplate_WANDSLConnMgt->variables = Template_WANDSLConnMgt.variables;
         pTemplate_WANDSLConnMgt->actions = Template_WANDSLConnMgt.actions;
         pTemplate_WANDSLConnMgt->count = Template_WANDSLConnMgt.count;
         pTemplate_WANDSLConnMgt->serviceid = Template_WANDSLConnMgt.serviceid; 
         pTemplate_WANDSLConnMgt->schema = Template_WANDSLConnMgt.schema;
         psvc = init_service(pTemplate_WANDSLConnMgt, parent, iidStack.instance[iidStack.currentDepth-1]);
         psvc->next = parent->services;
         parent->services = psvc;
      }
   }
#endif

#ifdef INCLUDE_WANETHERNETCONFIG
#ifdef DMP_ETHERNETWAN_1
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_WAN_ETH_INTF, &iidStack, (void **)&ethIntf) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &ethIntf);
      if(instanceOfWANDevice == iidStack.instance[iidStack.currentDepth-1])
      {

         struct ServiceTemplate *pTemplate_WANETHInterfaceConfig= NULL;
         pTemplate_WANETHInterfaceConfig= (struct ServiceTemplate *) malloc(sizeof(struct ServiceTemplate));
  
         if (pTemplate_WANETHInterfaceConfig == NULL) 
         {
            UPNP_ERROR(("malloc failed in %s\n", __FUNCTION__));
         }
         memset(pTemplate_WANETHInterfaceConfig, 0, sizeof(ServiceTemplate));

         pTemplate_WANETHInterfaceConfig->name = (char *)malloc(strlen(Template_WANETHInterfaceConfig.name)+1);
         sprintf(pTemplate_WANETHInterfaceConfig->name,"%s",Template_WANETHInterfaceConfig.name);
         pTemplate_WANETHInterfaceConfig->svcinit = Template_WANETHInterfaceConfig.svcinit;
         pTemplate_WANETHInterfaceConfig->getvars = Template_WANETHInterfaceConfig.getvars;
         pTemplate_WANETHInterfaceConfig->svcxml = Template_WANETHInterfaceConfig.svcxml;
         pTemplate_WANETHInterfaceConfig->nvariables = Template_WANETHInterfaceConfig.nvariables;
         pTemplate_WANETHInterfaceConfig->variables = Template_WANETHInterfaceConfig.variables;
         pTemplate_WANETHInterfaceConfig->actions = Template_WANETHInterfaceConfig.actions;
         pTemplate_WANETHInterfaceConfig->count = Template_WANETHInterfaceConfig.count;
         pTemplate_WANETHInterfaceConfig->serviceid = Template_WANETHInterfaceConfig.serviceid; 
         pTemplate_WANETHInterfaceConfig->schema = Template_WANETHInterfaceConfig.schema;
 
         psvc = init_service(pTemplate_WANETHInterfaceConfig, parent, iidStack.instance[iidStack.currentDepth-1]);
         psvc->next = parent->services;
         parent->services = psvc;
      }
   }
#endif
#endif

   INIT_INSTANCE_ID_STACK(&parentIidStack);
   while (cmsObj_getNext(MDMOID_WAN_CONN_DEVICE, &parentIidStack, (void **)&wanconCfg) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &wanconCfg);
  
      if(instanceOfWANDevice == parentIidStack.instance[parentIidStack.currentDepth-2])
      {
         /*Here Create one New WANConnectionDevice Template and Add it to the WANDevice*/
         PDeviceTemplate pdevtmpl = NULL;
         pdevtmpl = (struct DeviceTemplate *) malloc(sizeof(struct DeviceTemplate));
         memset(pdevtmpl, 0, sizeof(DeviceTemplate));
         pdevtmpl->type = (char *)malloc(strlen(subdevs_wandevice.type) + strlen(itoa(parentIidStack.instance[parentIidStack.currentDepth-1])) + 1 );
         sprintf(pdevtmpl->type,"%s%d",subdevs_wandevice.type, parentIidStack.instance[parentIidStack.currentDepth-1]);

         pdevtmpl->udn = (char *)malloc(strlen(subdevs_wandevice.udn) + strlen(itoa(parentIidStack.instance[parentIidStack.currentDepth-1])) + 1);
         sprintf(pdevtmpl->udn,"%s%d",subdevs_wandevice.udn, parentIidStack.instance[parentIidStack.currentDepth-1]);

         pdevtmpl->devinit = subdevs_wandevice.devinit;
         pdevtmpl->devxml = subdevs_wandevice.devxml;
         pdevtmpl->nservices = subdevs_wandevice.nservices;
         pdevtmpl->services = subdevs_wandevice.services;
         pdevtmpl->ndevices = subdevs_wandevice.ndevices;
         pdevtmpl->devicelist = subdevs_wandevice.devicelist;
         pdevtmpl->schema = subdevs_wandevice.schema;
         if (pdevtmpl->schema == NULL)
         {
            pdevtmpl->schema = TR64_DSLFORUM_SCHEMA;
         }
         pdev = (Device *) malloc(sizeof(Device));
         memset(pdev, 0, sizeof(Device));

         pdev->instance = parentIidStack.instance[parentIidStack.currentDepth-1];            
         /* Add 1 for \0 */
         pdev->friendlyname = (char *)malloc(strlen("WanConnectionDevice.") + strlen(itoa(parentIidStack.instance[parentIidStack.currentDepth-1])) + 1);
         sprintf(pdev->friendlyname,"WanConnectionDevice.%d", parentIidStack.instance[parentIidStack.currentDepth-1]);

         pdev->parent = parent;
         pdev->template = pdevtmpl;

         pdev->next = parent->subdevs;
         parent->subdevs = pdev;
         /* call the device's intialization function, if defined. */
         if ((func = pdevtmpl->devinit) != NULL) 
         {
//            va_start( ap, pdevtmpl);
            va_start( ap, instanceOfWANDevice );
            (*func)(pdev, DEVICE_CREATE, ap);
            va_end( ap);
         }
         /*ADD WANDSLLINKCONFIG*/
#ifdef INCLUDE_DSLLINKCONFIG
         INIT_INSTANCE_ID_STACK(&iidStack);

         /* get the related wanIpConn obj */
         while (cmsObj_getNextInSubTree(MDMOID_WAN_DSL_LINK_CFG, &parentIidStack, 
                                        &iidStack, (void **)&dslLinkCfg) == CMSRET_SUCCESS)
         {
            struct ServiceTemplate *pTemplate_WANDslLinkConfig= NULL;
            pTemplate_WANDslLinkConfig= (struct ServiceTemplate *) malloc(sizeof(struct ServiceTemplate));

            if (pTemplate_WANDslLinkConfig == NULL) 
            {
               UPNP_ERROR(("malloc failed in %s\n", __FUNCTION__));
            }
            memset(pTemplate_WANDslLinkConfig, 0, sizeof(ServiceTemplate));

            pTemplate_WANDslLinkConfig->name = (char *)malloc(strlen(Template_WANDSLLinkConfig.name)+1);
            sprintf(pTemplate_WANDslLinkConfig->name,"%s",Template_WANDSLLinkConfig.name);
            pTemplate_WANDslLinkConfig->svcinit = Template_WANDSLLinkConfig.svcinit;
            pTemplate_WANDslLinkConfig->getvars = Template_WANDSLLinkConfig.getvars;
            pTemplate_WANDslLinkConfig->svcxml = Template_WANDSLLinkConfig.svcxml;
            pTemplate_WANDslLinkConfig->nvariables = Template_WANDSLLinkConfig.nvariables;
            pTemplate_WANDslLinkConfig->variables = Template_WANDSLLinkConfig.variables;
            pTemplate_WANDslLinkConfig->actions = Template_WANDSLLinkConfig.actions;
            pTemplate_WANDslLinkConfig->count = Template_WANDSLLinkConfig.count;
            pTemplate_WANDslLinkConfig->serviceid = Template_WANDSLLinkConfig.serviceid; 
            pTemplate_WANDslLinkConfig->schema = Template_WANDSLLinkConfig.schema;
 
            psvc = init_service(pTemplate_WANDslLinkConfig, pdev, iidStack.instance[iidStack.currentDepth-1]);
            psvc->next = pdev->services;
            pdev->services = psvc;
 
            cmsObj_free((void **) &dslLinkCfg);
         }
#endif
         /*ADD ATMF5LOOPBACKDIAGNOSTICS*/
#ifdef INCLUDE_ATMF5LOOPBACKDIAG
         INIT_INSTANCE_ID_STACK(&iidStack);
         /* get the related wanIpConn obj */
         while (cmsObj_getNextInSubTree(MDMOID_WAN_ATM5_LOOPBACK_DIAG, &parentIidStack, &iidStack,
                                                              (void **)&atm5LoopbackCfg) == CMSRET_SUCCESS)
         {
            struct ServiceTemplate *pTemplate_WANATMF5LoopbackDiagnostics= NULL;
            pTemplate_WANATMF5LoopbackDiagnostics= (struct ServiceTemplate *) malloc(sizeof(struct ServiceTemplate));

            if (pTemplate_WANATMF5LoopbackDiagnostics == NULL) 
            {
               UPNP_ERROR(("malloc failed in %s\n", __FUNCTION__));
            }
            memset(pTemplate_WANATMF5LoopbackDiagnostics, 0, sizeof(ServiceTemplate));
  
            pTemplate_WANATMF5LoopbackDiagnostics->name = (char *)malloc(strlen(Template_WANATMF5LoopbackDiagnostics.name)+1);
            sprintf(pTemplate_WANATMF5LoopbackDiagnostics->name,"%s",Template_WANATMF5LoopbackDiagnostics.name);
            pTemplate_WANATMF5LoopbackDiagnostics->svcinit = Template_WANATMF5LoopbackDiagnostics.svcinit;
            pTemplate_WANATMF5LoopbackDiagnostics->getvars = Template_WANATMF5LoopbackDiagnostics.getvars;
            pTemplate_WANATMF5LoopbackDiagnostics->svcxml = Template_WANATMF5LoopbackDiagnostics.svcxml;
            pTemplate_WANATMF5LoopbackDiagnostics->nvariables = Template_WANATMF5LoopbackDiagnostics.nvariables;
            pTemplate_WANATMF5LoopbackDiagnostics->variables = Template_WANATMF5LoopbackDiagnostics.variables;
            pTemplate_WANATMF5LoopbackDiagnostics->actions = Template_WANATMF5LoopbackDiagnostics.actions;
            pTemplate_WANATMF5LoopbackDiagnostics->count = Template_WANATMF5LoopbackDiagnostics.count;
            pTemplate_WANATMF5LoopbackDiagnostics->serviceid = Template_WANATMF5LoopbackDiagnostics.serviceid; 
            pTemplate_WANATMF5LoopbackDiagnostics->schema = Template_WANATMF5LoopbackDiagnostics.schema;

            psvc = init_service(pTemplate_WANATMF5LoopbackDiagnostics, pdev, iidStack.instance[iidStack.currentDepth-1]);
            psvc->next = pdev->services;
            pdev->services = psvc;
            cmsObj_free((void **) &atm5LoopbackCfg);
         }
#endif
         /*ADD WANIPCONNECTION or WANPPPCONNECTION service*/

#ifdef INCLUDE_PPPCONNECTION
         INIT_INSTANCE_ID_STACK(&iidStack);
         /* get the related wanIpConn obj */
         while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &parentIidStack, &iidStack,
                                                            (void **)&wanPppConn) == CMSRET_SUCCESS)
         {
            struct ServiceTemplate *pTemplate_WANPPPConnection = NULL;
            pTemplate_WANPPPConnection= (struct ServiceTemplate *) malloc(sizeof(struct ServiceTemplate));

            if (pTemplate_WANPPPConnection == NULL) 
            {
               UPNP_ERROR(("malloc failed in %s\n", __FUNCTION__));
            }
            memset(pTemplate_WANPPPConnection, 0, sizeof(ServiceTemplate));
#if 0
            pTemplate_WANPPPConnection->name = (char *)malloc(strlen(Template_WANPPPConnection.name));
            sprintf(pTemplate_WANPPPConnection->name,"%s",Template_WANPPPConnection.name);
#else
            pTemplate_WANPPPConnection->name = malloc(strlen(Template_WANPPPConnection.name) + strlen(itoa(iidStack.instance[iidStack.currentDepth-1])) + 1);
            sprintf(pTemplate_WANPPPConnection->name,"%s%d",Template_WANPPPConnection.name, iidStack.instance[iidStack.currentDepth-1]);
#endif

            pTemplate_WANPPPConnection->svcinit = Template_WANPPPConnection.svcinit;
            pTemplate_WANPPPConnection->getvars = Template_WANPPPConnection.getvars;
            pTemplate_WANPPPConnection->svcxml = Template_WANPPPConnection.svcxml;
            pTemplate_WANPPPConnection->nvariables = Template_WANPPPConnection.nvariables;
            pTemplate_WANPPPConnection->variables = Template_WANPPPConnection.variables;
            pTemplate_WANPPPConnection->actions = Template_WANPPPConnection.actions;
            pTemplate_WANPPPConnection->count = Template_WANPPPConnection.count;
            pTemplate_WANPPPConnection->serviceid = Template_WANPPPConnection.serviceid; 
            pTemplate_WANPPPConnection->schema = Template_WANPPPConnection.schema;

            psvc = init_service(pTemplate_WANPPPConnection, pdev ,iidStack.instance[iidStack.currentDepth-1]);
            psvc->next = pdev->services;
            pdev->services = psvc;

            cmsObj_free((void **) &wanPppConn);
         }
#endif

#ifdef INCLUDE_IPCONNECTION
         INIT_INSTANCE_ID_STACK(&iidStack);
         /* get the related wanIpConn obj */
         while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &parentIidStack, &iidStack,
                                                         (void **)&wanIpConn) == CMSRET_SUCCESS)
         {
            struct ServiceTemplate *pTemplate_WANIPConnection = NULL;
            pTemplate_WANIPConnection= (struct ServiceTemplate *) malloc(sizeof(struct ServiceTemplate));

            if (pTemplate_WANIPConnection == NULL) 
            {
               UPNP_ERROR(("malloc failed in %s\n", __FUNCTION__));
            }
            memset(pTemplate_WANIPConnection, 0, sizeof(ServiceTemplate));
#if 0
            pTemplate_WANIPConnection->name = (char *)malloc(strlen(Template_WANIPConnection.name));
            sprintf(pTemplate_WANIPConnection->name,"%s",Template_WANIPConnection.name);
#else
            pTemplate_WANIPConnection->name = (char *)malloc(strlen(Template_WANIPConnection.name) + strlen(itoa(iidStack.instance[iidStack.currentDepth-1]))+1);
            sprintf(pTemplate_WANIPConnection->name,"%s%d",Template_WANIPConnection.name, iidStack.instance[iidStack.currentDepth-1]);
#endif
            pTemplate_WANIPConnection->svcinit = Template_WANIPConnection.svcinit;
            pTemplate_WANIPConnection->getvars = Template_WANIPConnection.getvars;
            pTemplate_WANIPConnection->svcxml = Template_WANIPConnection.svcxml;
            pTemplate_WANIPConnection->nvariables = Template_WANIPConnection.nvariables;
            pTemplate_WANIPConnection->variables = Template_WANIPConnection.variables;
            pTemplate_WANIPConnection->actions = Template_WANIPConnection.actions;
            pTemplate_WANIPConnection->count = Template_WANIPConnection.count;
            pTemplate_WANIPConnection->serviceid = Template_WANIPConnection.serviceid; 
            pTemplate_WANIPConnection->schema = Template_WANIPConnection.schema;

            psvc = init_service(pTemplate_WANIPConnection, pdev ,iidStack.instance[iidStack.currentDepth-1]);
            psvc->next = pdev->services;
            pdev->services = psvc;
            cmsObj_free((void **) &wanIpConn);
         }
#endif
       }
    }

    if(pdev!=NULL)
    {
       if (ISROOT(pdev)) 
       {
          pdev->next = root_devices;
          root_devices = pdev;
       }
    }
    return pdev;
}

int WANDevice_Init(PDevice pdev, device_state_t state, va_list ap)
{
   PWANDevicePrivateData pdata;

   switch (state) 
   {
   case DEVICE_CREATE:
      pdata = (PWANDevicePrivateData) malloc(sizeof(WANDevicePrivateData));
      /* we only have one WAN device */
      if (pdata)
      {
         strcpy(pdata->ifname, "WAN DEVICE");
         pdev->opaque = (void *) pdata;
      }
      break;
       
    case DEVICE_DESTROY:
       free(pdev->opaque);
       pdev->opaque = NULL;
       break;
    }

    return TRUE;
}


int LANDevice_Init(PDevice pdev, device_state_t state, va_list ap)
{
    PLANDevicePrivateData pdata;

    switch (state) 
    {
    case DEVICE_CREATE:
       pdata = (PLANDevicePrivateData) malloc(sizeof(LANDevicePrivateData));
       /* we only have one LAN device */
       if (pdata)
       {
          strcpy(pdata->ifname, "LAN DEVICE");
          pdev->opaque = (void *) pdata;
       }
       break;

    case DEVICE_DESTROY:
       free(pdev->opaque);
       pdev->opaque = NULL;
       break;
    }

    return TRUE;
}

/** findActionParamByRelatedVar
 *  input parameter: Action pointer, and relatedVar is the var index such as VAR_Interface.
 *  output: returns pointer to struct Param of this variable. NULL if not found.
 */
struct Param *findActionParamByRelatedVar(PAction ac, int relatedVar)
{
   struct Param *ptr;
   int index = 0;

   while ((ptr = &ac->params[index]) != NULL)
   {
      if (ptr->related == relatedVar)
      {
         return ptr;
      }
      index++;
   } /* while */
   return ((struct Param*)NULL);
}

int OutputCharValueToAC(PAction ac, int varIndex, char *value)
{
   int errorinfo = 0;
   struct Param *pParams;

   pParams = findActionParamByRelatedVar(ac,varIndex);
   if (pParams != NULL)
   {
      if( value != NULL )
      {
         pParams->value = strdup(value);
      }
      else
      {
         pParams->value = strdup("");
      }
   }
   else
   {
      errorinfo = SOAP_ACTIONFAILED;
   }
   return errorinfo;
}

int OutputNumValueToAC(PAction ac, int varIndex, int value)
{
   int errorinfo = 0;
   struct Param *pParams;
   char tmp[BUFLEN_32];

   memset(tmp, 0, BUFLEN_32);
   sprintf(tmp, "%d", value);

   pParams = findActionParamByRelatedVar(ac,varIndex);
   if (pParams != NULL)
   {
      pParams->value = strdup(tmp);
   }
   else
   {
      errorinfo = SOAP_ACTIONFAILED;
   }
   return errorinfo;
}

