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
 * $Id: igd.c,v 1.12.20.2 2003/10/31 21:31:35 mthawani Exp $
 *
 ************************************************************************/

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#define INCLUDE_PPPCONNECTION
#define INCLUDE_IPCONNECTION

extern PDevice root_devices ;

extern DeviceTemplate LANDeviceTemplate;
extern DeviceTemplate WANDeviceTemplate;
extern DeviceTemplate subdevs_wandevice;

extern PServiceTemplate svcs_wanconnection;

#ifdef INCLUDE_IPCONNECTION
extern ServiceTemplate Template_WANIPConnection;
#endif
#ifdef INCLUDE_PPPCONNECTION
extern ServiceTemplate Template_WANPPPConnection;
#endif

extern PDevice init_device(PDevice parent, PDeviceTemplate, ...);
extern PService init_service(PServiceTemplate svctmpl, PDevice pdev,int dynInstId);
extern int setsvcval_by_name(PService psvc, const char *varname,const char *value);
extern int OutputCharValueToAC(PAction ac, char *varName, char *value);
extern int OutputNumValueToAC(PAction ac, char* varName, int value);


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

static PDevice init_wan_device_and_service(PDevice parent, int instanceOfWANDevice, ...);

const char *itoa(int i)
{
   static char buf[256];
   sprintf(buf, "%d", i);
   return buf;
}

int IGDevice_Init(PDevice igdev, device_state_t state, va_list ap)
{
    char *wan_ifname = g_wan_ifname;
#if INCLUDE_LANDEVICE
    char *lan_ifname = g_lan_ifname;
#endif
    PDevice subdev;
    int wanDevInstanceNum=0;

    switch (state) {
    case DEVICE_CREATE:
	soap_register_errors(IGDErrors);

	igdev->friendlyname = cmsMem_alloc(strlen("Broadcom ADSL Router")+1, ALLOC_ZEROIZE);
	if (igdev->friendlyname)
	    strcpy(igdev->friendlyname,"Broadcom ADSL Router");

#if INCLUDE_LANDEVICE
	/*TODO: integrate this part with MDM when LANDEVICE is supported*/

	/* Include this code if you want a LAN device in your IGD.  If
	   you do, make sure that the LANHOSTCONFIG service is also included
	   in igd_desc.c */
	UPNP_TRACE(("Calling init_device for LANDevice\n"));
	subdev = init_device(igdev, &LANDeviceTemplate, lan_ifname);
	subdev->next = igdev->subdevs;
	igdev->subdevs = subdev;
#endif
	
	/* create a WANDevice only for the wan interface sent to upnp
 	* at launch time 
 	* */
   if (getInstanceNumbersForWanIfname(wan_ifname,
                                      &wanDevInstanceNum, NULL, NULL) == CMSRET_SUCCESS)
   {
      UPNP_TRACE(("Calling init_device for WANDevice\n"));
      subdev = init_device(igdev, &WANDeviceTemplate, wanDevInstanceNum, g_wan_l2ifname);
      if(subdev ==NULL){
         goto Exit_fail1;
      }

      subdev->friendlyname = (char *)cmsMem_alloc(BUFLEN_32, ALLOC_ZEROIZE);  // buf must hold WANDevice.xyz
      sprintf(subdev->friendlyname,"WANDevice.%d", wanDevInstanceNum);

      subdev->next = igdev->subdevs;
      igdev->subdevs = subdev;

      subdev = init_wan_device_and_service(subdev , wanDevInstanceNum);
   }else{
		/* interface not found */
		goto Exit_fail1;
	}

	/* install a signal handler for SIGUSR1 */
	signal(SIGUSR1, sigusr1_handler);

	break;

    case DEVICE_DESTROY:
	break;
    }

    return TRUE;

Exit_fail1:
    return FALSE;
}





int WANDevice_Init(PDevice pdev, device_state_t state, va_list ap)
{
    PWANDevicePrivateData pdata;
    char *ifname = NULL;

    switch (state) {
    case DEVICE_CREATE:
	ifname = va_arg( ap, char *);
	if (ifname) {
	    pdata = (PWANDevicePrivateData) cmsMem_alloc(sizeof(WANDevicePrivateData), ALLOC_ZEROIZE);
	    if (pdata) {
		strcpy(pdata->ifname, ifname);
		pdev->opaque = (void *) pdata;
	    }
	}
	break;

    case DEVICE_DESTROY:
	cmsMem_free(pdev->opaque);
	pdev->opaque = NULL;
	break;
    }

    return TRUE;
}

static PDevice init_wan_device_and_service(PDevice parent, int instanceOfWANDevice, ...)
{
   int wanDevInstanceNum=0;
   int wanConnInstanceNum=0;
   int pppipInstanceNum=0;

   PFDEVINIT func;
   PDevice pdev = NULL;
   PService psvc = NULL;
   va_list ap;


   if (getInstanceNumbersForWanIfname(g_wan_ifname,
                                      &wanDevInstanceNum,
                                      &wanConnInstanceNum,
                                      &pppipInstanceNum) == CMSRET_SUCCESS)
   {
      if (instanceOfWANDevice != (int) wanDevInstanceNum)
      {
         UPNP_ERROR(("WANDevice instance mismatch %d %d\n", instanceOfWANDevice, wanDevInstanceNum));
         goto Exit_fail1;
      }

      pdev = (Device *) cmsMem_alloc(sizeof(Device), ALLOC_ZEROIZE);
      if (pdev == NULL)
      {
         UPNP_ERROR(("cmsMem_alloc failed in %s\n", __FUNCTION__));
         goto Exit_fail1;
      }

      pdev->instance = wanConnInstanceNum;
      pdev->friendlyname = (char *)cmsMem_alloc(BUFLEN_64, ALLOC_ZEROIZE);  // buf must hold WanConnectionDevice.xyz
      if (pdev->friendlyname == NULL)
      {
         UPNP_ERROR(("cmsMem_alloc failed in %s\n", __FUNCTION__));
         goto Exit_fail1;
      }
      sprintf(pdev->friendlyname,"WanConnectionDevice.%d", wanConnInstanceNum);

      pdev->parent = parent;
      pdev->template = &subdevs_wandevice;

      pdev->next = parent->subdevs;
      parent->subdevs = pdev;
      /* call the device's intialization function, if defined. */
      if ((func = pdev->template->devinit) != NULL)
      {
         //            va_start( ap, pdev->template);
         va_start( ap, instanceOfWANDevice );
         (*func)(pdev, DEVICE_CREATE, ap);
         va_end( ap);
      }

      /*ADD WANIPCONNECTION or WANPPPCONNECTION service*/

#ifdef INCLUDE_PPPCONNECTION
      if (cmsUtl_strstr(g_wan_ifname, PPP_IFC_STR))
      {
         psvc = init_service(&Template_WANPPPConnection, pdev, pppipInstanceNum);
         psvc->next = pdev->services;
         pdev->services = psvc;
      }
#endif

#ifdef INCLUDE_IPCONNECTION
      if (cmsUtl_strstr(g_wan_ifname, PPP_IFC_STR) == NULL)
      {
         psvc = init_service(&Template_WANIPConnection, pdev, pppipInstanceNum);
         psvc->next = pdev->services;
         pdev->services = psvc;
      }
#endif
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
Exit_fail1:
    exit(1);  
}

int LANDevice_Init(PDevice pdev, device_state_t state, va_list ap)
{
    PLANDevicePrivateData pdata;
    char *ifname = NULL;

    switch (state) {
    case DEVICE_CREATE:
	ifname = va_arg( ap, char *);
	if (ifname) {
	    pdata = (PLANDevicePrivateData) cmsMem_alloc(sizeof(LANDevicePrivateData), ALLOC_ZEROIZE);
	    if (pdata) {
		strcpy(pdata->ifname, ifname);
		pdev->opaque = (void *) pdata;
	    }
	}
	break;

    case DEVICE_DESTROY:
	cmsMem_free(pdev->opaque);
	pdev->opaque = NULL;
	break;
    }

    return TRUE;
}

int igd_config_generation = 0;

void bump_generation()
{
    if (++igd_config_generation > 1000)
	igd_config_generation = 0;
}

void sigusr1_handler(int i)
{
    extern void reinit_lan_interface(struct iface *pif);
    extern struct iface *global_lans;
    struct iface *pif;

    printf("upnp reinitialize.\n");
    bump_generation();

    for (pif = global_lans; pif; pif = pif->next) 
	reinit_lan_interface(pif);

    /* re-install a signal handler for SIGUSR1 */
    signal(SIGUSR1, sigusr1_handler);
}


char *
safe_snprintf(char *str, int *len, const char *fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vsnprintf(str, *len, fmt, ap);
	va_end(ap);

	if (n > 0) {
		str += n;
		*len -= n;
	} else if (n < 0) {
		*len = 0;
	}
	return str;
}




#ifdef DMP_BASELINE_1

CmsRet getInstanceNumbersForWanIfname_igd(const char *wan_ifname,
                                          int *wanDevInstanceNum,
                                          int *wanConnInstanceNum,
                                          int *pppipInstanceNum)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;;
   CmsRet ret;
   UBOOL8 found = FALSE;

   /*first get the MDM WANDevice object corresponding to wan_ifname */
   if (cmsUtl_strstr(wan_ifname, PPP_IFC_STR))
   {
      WanPppConnObject *pppCon = NULL;
      /* this is a PPP connection. We will search all WANPPPConnection objects for
       * the wan_ifname.
       */
      while (!found &&
             (ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **)&pppCon)) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(pppCon->X_BROADCOM_COM_IfName, wan_ifname))
         {
            found = TRUE;
         }
         cmsObj_free((void **)&pppCon);
      }

    }else { /* for IP_ROUTED  */
      WanIpConnObject  *ipCon = NULL;

      /* this is an IP connection. We will search all WANIPConnection objects for
       * the serviceIfcName.
       */
      while (!found &&
             (ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **)&ipCon)) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(ipCon->X_BROADCOM_COM_IfName, wan_ifname))
         {
            found = TRUE;
         }
         cmsObj_free((void **)&ipCon);
      }
    }

   if (found)
   {
      /* The fullpath looks like:
       * WanDevice.x.WanConnectionDevice.y.WanPPPConnection.z   --or--
       * WanDevice.x.WanConnectionDevice.y.WanIPConnection.z
       */
      if (wanDevInstanceNum)
      {
         *wanDevInstanceNum = (int) iidStack.instance[0];
      }

      if (wanConnInstanceNum)
      {
         *wanConnInstanceNum = (int) iidStack.instance[1];
      }

      if (pppipInstanceNum)
      {
         *pppipInstanceNum = (int) iidStack.instance[2];
      }
   }
   else
   {
      cmsLog_error("Could not find %s", wan_ifname);
   }

   return ret;
}


void WANConnection_UpdateVars_igd(struct Service *psvc, int isPpp)
{
   WanPppConnObject *pppConn = NULL;
   WanIpConnObject  *ipConn = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char tmpbuf[BUFLEN_32]={0};
   CmsRet ret;


   PUSH_INSTANCE_ID(&iidStack, psvc->device->parent->instance );/*instanceOfWANDevice */
   PUSH_INSTANCE_ID(&iidStack, psvc->device->instance);/*instanceOfWANConnectionDevice */
   PUSH_INSTANCE_ID(&iidStack, psvc->instance);/*instanceOfWANPPPConnection */

   if (isPpp)
   {
      if ((ret=cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **)&pppConn)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WANPPPconn object , ret=%d", ret);
         return;
      }
   }
   else
   {
      if ((ret=cmsObj_get(MDMOID_WAN_IP_CONN, &iidStack, 0, (void **)&ipConn)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WANIPconn object , ret=%d", ret);
         return;
      }
   }

    if (mode != PCP_MODE_DSLITE)
    {
        setsvcval_by_name(psvc,"ConnectionStatus",
                                 (isPpp ? pppConn->connectionStatus :
                                           ipConn->connectionStatus));

        sprintf(tmpbuf,"%d", (isPpp ? pppConn->NATEnabled:
                                       ipConn->NATEnabled));
        setsvcval_by_name(psvc,"NATEnabled",tmpbuf);

        setsvcval_by_name(psvc,"ExternalIPAddress",
                                  (isPpp ? pppConn->externalIPAddress :
                                            ipConn->externalIPAddress));

        sprintf(tmpbuf,"%d", (isPpp ? pppConn->portMappingNumberOfEntries :
                                       ipConn->portMappingNumberOfEntries));
        setsvcval_by_name(psvc,"PortMappingNumberOfEntries",tmpbuf);
    }
#ifdef SUPPORT_IPV6
    else
    {
#ifdef DMP_DEVICE2_BASELINE_1
       /* Hybrid IPv6, get params from _dev2 side */
       WANConnection_UpdateVars_dev2(psvc, isPpp);
#else
       /* Legacy Broadcom Proprietary IPv6 */
        char externalAddr[INET6_ADDRSTRLEN]={0};
        char *ptr;

        setsvcval_by_name(psvc,"ConnectionStatus", pppConn->X_BROADCOM_COM_IPv6ConnStatus);

        //Always set NATEnabled to 1 for PCP mode
        setsvcval_by_name(psvc,"NATEnabled","1");

        if (!inet_ntop(AF_INET6, pcpextaddr, externalAddr, INET6_ADDRSTRLEN))
        {
           cmsLog_error("inet_ntop AF_INET6 failed for %s", pcpextaddr);
        }

        // fetch external IPv4 address from ::ffff:a.b.c.d
        ptr = strstr(externalAddr, "::");
        setsvcval_by_name(psvc,"ExternalIPAddress", (ptr+7));

        sprintf(tmpbuf,"%d", (isPpp ? pppConn->portMappingNumberOfEntries :
                                       ipConn->portMappingNumberOfEntries));
        setsvcval_by_name(psvc,"PortMappingNumberOfEntries",tmpbuf);
#endif  /* DMP_DEVICE2_BASELINE_1 */
    }
#endif  /* SUPPORT_IPV6 */

    sprintf(tmpbuf,"%u", (isPpp ? pppConn->uptime :
                                   ipConn->uptime));
    setsvcval_by_name(psvc,"Uptime",tmpbuf);

   /*TODO:suresh check if other variables need to be added in
    * data model */
#if 0
    //setsvcval_by_name(psvc,"LastConnectionError", pppConn->lastConnectionError);
    //setsvcval_by_name(psvc,"ConnectionType", pppConn->connectionType);
    //setsvcval_by_name(psvc,"PossibleConnectionTypes", pppConn->possibleConnectionTypes);

    sprintf(tmpbuf,"%lu",pppConn->upStreamMaxBitRate);
    setsvcval_by_name(psvc,"UpstreamMaxBitRate",tmpbuf);

    sprintf(tmpbuf,"%lu",pppConn->downStreamMaxBitRate);
    setsvcval_by_name(psvc,"DownstreamMaxBitRate",tmpbuf);

    sprintf(tmpbuf,"%d",pppConn->Rsip);
    setsvcval_by_name(psvc,"RSIPAvailable",tmpbuf);

    sprintf(tmpbuf,"%d",pppConn->portMappingEnabled);
    setsvcval_by_name(psvc,"PortMappingEnabled",tmpbuf);

    sprintf(tmpbuf,"%lu",pppConn->PortMappingLeaseDuration);
    setsvcval_by_name(psvc,"PortMappingLeaseDuration",tmpbuf);

    setsvcval_by_name(psvc,"RemoteHost", pppConn->remoteHost);
    sprintf(tmpbuf,"%d",pppConn->ExternalPort);
    setsvcval_by_name(psvc,"ExternalPort",tmpbuf);
    sprintf(tmpbuf,"%d",pppConn->InternalPort);
    setsvcval_by_name(psvc,"InternalPort",tmpbuf);
    setsvcval_by_name(psvc,"InternalClient", pppConn->internalClient);
    setsvcval_by_name(psvc,"PortMappingDescription", pppConn->portMappingDescription);
    setsvcval_by_name(psvc,"PortMappingProtocol", pppConn->PortMappingProtocol);
#endif

   if (isPpp)
   {
      cmsObj_free((void **) &pppConn);
   }
   else
   {
      cmsObj_free((void **) &ipConn);
   }

   return;
}


void WANCommonInterface_UpdateVars_igd(PService psvc)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanCommonIntfCfgObject *wanCommObj=NULL;

   CmsRet ret;
   char tmpbuf[BUFLEN_32]={0};


   PUSH_INSTANCE_ID(&iidStack, psvc->device->instance );/*instanceOfWANDevice */
   cmsLog_debug("Entered: Using instance %d", psvc->device->instance );

   if ((ret = cmsObj_get(MDMOID_WAN_COMMON_INTF_CFG, &iidStack, 0, (void **)&wanCommObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get WANCommonInterface object , ret=%d", ret);
      return;
   }

    setsvcval_by_name(psvc,"WANAccessType", wanCommObj->WANAccessType);

    sprintf(tmpbuf,"%lu",(ulong)wanCommObj->layer1UpstreamMaxBitRate);
    setsvcval_by_name(psvc,"Layer1UpstreamMaxBitRate",tmpbuf);
    sprintf(tmpbuf,"%lu",(ulong)wanCommObj->layer1DownstreamMaxBitRate);
    setsvcval_by_name(psvc,"Layer1DownstreamMaxBitRate",tmpbuf);

    setsvcval_by_name(psvc,"PhysicalLinkStatus", wanCommObj->physicalLinkStatus);

   /*TODO: use this once EnabledforInternet flag is set proeprly in CMS*/
    /*sprintf(tmpbuf,"%c",wanCommObj->enabledForInternet);
    setsvcval_by_name(psvc,"EnabledForInternet",tmpbuf);*/

    if (!cmsUtl_strcmp(wanCommObj->WANAccessType, MDMVS_ETHERNET))
    {
         WanEthIntfObject *wanEthIntf=NULL;
         if (cmsObj_get(MDMOID_WAN_ETH_INTF, &iidStack, 0, (void **)&wanEthIntf) != CMSRET_SUCCESS)
         {
            sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalBytesSent);
            setsvcval_by_name(psvc,"TotalBytesSent",tmpbuf);
            sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalBytesReceived);
            setsvcval_by_name(psvc,"TotalBytesReceived",tmpbuf);
            sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalPacketsSent);
            setsvcval_by_name(psvc,"TotalPacketsSent",tmpbuf);
            sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalPacketsReceived);
            setsvcval_by_name(psvc,"TotalPacketsReceived",tmpbuf);
            cmsLog_error("cmsObj_get <MDMOID_WAN_ETH_INTF> returns error.");
            cmsObj_free((void **)&wanCommObj);
            return;
         }            
         else 
         {
            /* Need to check if this ethWan is configured or not.  If not configured, just skip it */
            if (IS_EMPTY_STRING(wanEthIntf->X_BROADCOM_COM_IfName))
            {
               sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalBytesSent);
               setsvcval_by_name(psvc,"TotalBytesSent",tmpbuf);
               sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalBytesReceived);
               setsvcval_by_name(psvc,"TotalBytesReceived",tmpbuf);
               sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalPacketsSent);
               setsvcval_by_name(psvc,"TotalPacketsSent",tmpbuf);
               sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalPacketsReceived);
               setsvcval_by_name(psvc,"TotalPacketsReceived",tmpbuf);
               cmsObj_free((void **)&wanCommObj);
               cmsObj_free((void **)&wanEthIntf);
               return;
            }
            else
            {
               WanEthIntfStatsObject *wanEthIntfStat=NULL;
               
               /* Get the stats from this ethWAN */
               if (cmsObj_get(MDMOID_WAN_ETH_INTF_STATS, &iidStack, 0, (void **)&wanEthIntfStat) != CMSRET_SUCCESS)
               {
                  sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalBytesSent);
                  setsvcval_by_name(psvc,"TotalBytesSent",tmpbuf);
                  sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalBytesReceived);
                  setsvcval_by_name(psvc,"TotalBytesReceived",tmpbuf);
                  sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalPacketsSent);
                  setsvcval_by_name(psvc,"TotalPacketsSent",tmpbuf);
                  sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalPacketsReceived);
                  setsvcval_by_name(psvc,"TotalPacketsReceived",tmpbuf);
                  cmsLog_error("cmsObj_get <MDMOID_WAN_ETH_INTF_STATS> returns error.");
                  cmsObj_free((void **)&wanEthIntf);
                  return;
               }  
               sprintf(tmpbuf,"%lu",wanEthIntfStat->bytesSent);
               setsvcval_by_name(psvc,"TotalBytesSent",tmpbuf);
               sprintf(tmpbuf,"%lu",wanEthIntfStat->bytesReceived);
               setsvcval_by_name(psvc,"TotalBytesReceived",tmpbuf);
               sprintf(tmpbuf,"%lu",wanEthIntfStat->packetsSent);
               setsvcval_by_name(psvc,"TotalPacketsSent",tmpbuf);
               sprintf(tmpbuf,"%lu",wanEthIntfStat->packetsReceived);
               setsvcval_by_name(psvc,"TotalPacketsReceived",tmpbuf);
               cmsObj_free((void **)&wanEthIntf);
               cmsObj_free((void **)&wanEthIntfStat);
               return;
            }    
         }            
    }
    else if (!cmsUtl_strcmp(wanCommObj->WANAccessType, MDMVS_DSL))
    {
	/* copy from cgi_sts.c::cgiDisplayStsXtm_igd() */
	InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
        XtmInterfaceStatsObject *xtmStats = NULL;
        while (cmsObj_getNext(MDMOID_XTM_INTERFACE_STATS, &iidStack,(void **)&xtmStats) == CMSRET_SUCCESS)
        {
           if (!cmsUtl_strcmp(xtmStats->status,MDMVS_ENABLED))
           {
              sprintf(tmpbuf,"%lu",xtmStats->outOctets);
              setsvcval_by_name(psvc,"TotalBytesSent",tmpbuf);
              sprintf(tmpbuf,"%lu",xtmStats->inOctets);
              setsvcval_by_name(psvc,"TotalBytesReceived",tmpbuf);
              sprintf(tmpbuf,"%lu",xtmStats->outPackets);
              setsvcval_by_name(psvc,"TotalPacketsSent",tmpbuf);
              sprintf(tmpbuf,"%lu",xtmStats->inPackets);
              setsvcval_by_name(psvc,"TotalPacketsReceived",tmpbuf);
              cmsObj_free((void **) &xtmStats);
              cmsObj_free((void **) &wanCommObj);
              return;
           }
           cmsObj_free((void **) &xtmStats);
        } /* while xtmStats */
    }
    sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalBytesSent);
    setsvcval_by_name(psvc,"TotalBytesSent",tmpbuf);
    sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalBytesReceived);
    setsvcval_by_name(psvc,"TotalBytesReceived",tmpbuf);
    sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalPacketsSent);
    setsvcval_by_name(psvc,"TotalPacketsSent",tmpbuf);
    sprintf(tmpbuf,"%lu",(ulong)wanCommObj->totalPacketsReceived);
    setsvcval_by_name(psvc,"TotalPacketsReceived",tmpbuf);

   cmsObj_free((void **) &wanCommObj);

   return;
}



extern char *g_devMfr;
extern char *g_devMdlDesc;
extern char *g_devMdlName;
extern char *g_devSerialNum;

int upnp_getDeviceInfo_igd()
{
   int errorInfo = 0;
   CmsRet ret;
   IGDDeviceInfoObject *deviceInfoObj= NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* caller has already acquired the CMS lock */

   if ((ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void **) &deviceInfoObj)) == CMSRET_SUCCESS)
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


int upnp_setConnectionType_igd(UBOOL8 isPpp, const InstanceIdStack *iidStack,
                               const char *connectionType)
{
   CmsRet ret;
   int errorInfo = 0;


   if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return SOAP_ACTION_FAILED;
   }

   if (isPpp)
   {
      WanPppConnObject *pppConn = NULL;

      ret = cmsObj_get(MDMOID_WAN_PPP_CONN, iidStack, 0, (void **)&pppConn);
      if (ret == CMSRET_SUCCESS)
      {
         /* XXX Really?  We allow UPnP to set the connectionType?!? */
         REPLACE_STRING_IF_NOT_EQUAL(pppConn->connectionType, connectionType);

         ret = cmsObj_set(pppConn, iidStack);

         cmsObj_free((void **) &pppConn);
      }
    }
    else
    {
       WanIpConnObject  *ipConn = NULL;

       ret = cmsObj_get(MDMOID_WAN_IP_CONN, iidStack, 0, (void **)&ipConn);
       if (ret == CMSRET_SUCCESS)
       {
          REPLACE_STRING_IF_NOT_EQUAL(ipConn->connectionType, connectionType);

          ret = cmsObj_set(ipConn, iidStack);

          cmsObj_free((void **) &ipConn);
       }
    }

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get WANIPconn/WANPPPconn object , ret=%d", ret);
      errorInfo = SOAP_ACTION_FAILED;
   }
   else
   {
      /* save config  */
      cmsMgm_saveConfigToFlash();
   }

   cmsLck_releaseLock();

   return errorInfo;
}


int upnp_updateOrAddPortMapping_igd(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
              UBOOL8 portMappingEnabled, UINT32 leaseDuration,
              const char *protocol, UINT16 externalPort, UINT16 internalPort,
              const char *remoteHost, const char *internalClient, char *description)
{
   int errorInfo = 0;
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   // We use pppConnPortMapping object even when dealing with IpConnPortmapping.
   // This is OK because these two objects have the exact same layout.  Only
   // the oid is different.
   WanPppConnPortmappingObject *port_mapping = NULL;
   MdmObjectId oid;

   oid = (isPpp ? MDMOID_WAN_PPP_CONN_PORTMAPPING :
                  MDMOID_WAN_IP_CONN_PORTMAPPING);


   if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return SOAP_ACTION_FAILED;
   }

   /* check if there is an existing entry for same Ip/port mapping */
   while (!found &&
          (cmsObj_getNextInSubTree(oid, parentIidStack, &iidStack,
                                  (void **)&port_mapping) == CMSRET_SUCCESS))
   {
      if((externalPort == port_mapping->externalPort)
           && (0 == cmsUtl_strcmp(protocol, port_mapping->portMappingProtocol)))
      {
         found = TRUE;

         /*check if it is for same internal client and
          * and also update entry only if is aaded by upnp previously */
         if((internalPort == port_mapping->internalPort)
               &&(0 == cmsUtl_strcmp(internalClient,port_mapping->internalClient) )
               &&(0 == cmsUtl_strcmp(remoteHost,port_mapping->remoteHost) )
               &&(0 == cmsUtl_strcmp(port_mapping->X_BROADCOM_COM_AppName,g_upnpAppName)))
         {
            /* update the entry */
            port_mapping->portMappingLeaseDuration = leaseDuration;
            port_mapping->portMappingEnabled = portMappingEnabled;
            REPLACE_STRING_IF_NOT_EQUAL(port_mapping->portMappingDescription,description);

            ret = cmsObj_set(port_mapping, &iidStack);
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
      cmsObj_free((void **) &port_mapping);
   }

   if (errorInfo)
   {
      cmsLck_releaseLock();
      return errorInfo;
   }

   if (found)
   {
      cmsLck_releaseLock();
      return 0;
   }


   /*
    * Existing entry not found, create new entry
    */
   iidStack = *parentIidStack;
   if ((ret = cmsObj_addInstance(oid, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new virtual server entry, ret=%d", ret);
      errorInfo = SOAP_ACTION_FAILED;
      goto Exit_1;
   }

   if((ret =cmsObj_setNonpersistentInstance(oid, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set Non persistent flag of new virtual server entry, ret=%d", ret);
      errorInfo = SOAP_ACTION_FAILED;
      goto Exit_1;
   }

   if ((ret = cmsObj_get(oid, &iidStack, 0, (void **) &port_mapping)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get VirtualServerCfgObject, ret=%d", ret);
      errorInfo =SOAP_ACTION_FAILED ;
      goto Exit_1;
   }

   REPLACE_STRING_IF_NOT_EQUAL(port_mapping->remoteHost,remoteHost);
   port_mapping->externalPort = externalPort;
   port_mapping->X_BROADCOM_COM_ExternalPortEnd = externalPort;
   REPLACE_STRING_IF_NOT_EQUAL(port_mapping->internalClient,internalClient);
   port_mapping->internalPort = internalPort;
   port_mapping->X_BROADCOM_COM_InternalPortEnd = internalPort;
   REPLACE_STRING_IF_NOT_EQUAL(port_mapping->portMappingProtocol,protocol);
   port_mapping->portMappingEnabled = portMappingEnabled;
   port_mapping->portMappingLeaseDuration = leaseDuration;
   REPLACE_STRING_IF_NOT_EQUAL(port_mapping->portMappingDescription,description);
   REPLACE_STRING_IF_NOT_EQUAL(port_mapping->X_BROADCOM_COM_AppName,g_upnpAppName);

   if((ret =cmsObj_set(port_mapping, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set new virtual server entry, ret=%d", ret);
      errorInfo = SOAP_ACTION_FAILED;
      goto Exit_1;
   }


Exit_1:
   cmsObj_free((void **) &port_mapping);
   cmsLck_releaseLock();

   return errorInfo;
}


int upnp_deletePortMapping_igd(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
            const char *protocol, UINT16 externalPort, const char *remoteHost,
            char *iaddr)
{
   int errorInfo = 0;
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   // We use pppConnPortMapping object even when dealing with IpConnPortmapping.
   // This is OK because these two objects have the exact same layout.  Only
   // the oid is different.
   WanPppConnPortmappingObject *port_mapping = NULL;
   MdmObjectId oid;

   oid = (isPpp ? MDMOID_WAN_PPP_CONN_PORTMAPPING :
                  MDMOID_WAN_IP_CONN_PORTMAPPING);

   if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return SOAP_ACTION_FAILED;
   }

   while (!found &&
          (cmsObj_getNextInSubTree(oid, parentIidStack, &iidStack,
                                   (void **)&port_mapping) == CMSRET_SUCCESS))
   {
      if((port_mapping->externalPort == externalPort)
            &&(0== cmsUtl_strcmp(port_mapping->portMappingProtocol, protocol))
            &&(0 == cmsUtl_strcmp(port_mapping->remoteHost,remoteHost))
            &&(0 == cmsUtl_strcmp(port_mapping->X_BROADCOM_COM_AppName,g_upnpAppName)))
      {
         found = TRUE;

         /* pass back internal client addr to caller (assume caller has
          * passed in a buffer that is big enough) */
         cmsUtl_strcpy(iaddr, port_mapping->internalClient);

         if ((ret = cmsObj_deleteInstance(oid, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not delete virtual server entry, ret=%d", ret);
            errorInfo = SOAP_ACTION_FAILED;
         }

         /*suresh TODO: update local state variables */
      }
      cmsObj_free((void **) &port_mapping);
   }

   if (!found)
   {
      cmsLog_error("Could not find portmapping proto=%s extPort=%d remoteHost=%s",
                   protocol, externalPort, remoteHost);
      errorInfo = SOAP_NOSUCHENTRYINARRAY;
   }

    cmsLck_releaseLock();
    return errorInfo;
}


int upnp_getGenericPortMapping_igd(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
                                   int portMappingIndex, PAction ac)
{
   int errorinfo = 0;
   int localIndex =0;
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   // We use pppConnPortMapping object even when dealing with IpConnPortmapping.
   // This is OK because these two objects have the exact same layout.  Only
   // the oid is different.
   WanPppConnPortmappingObject *port_mapping = NULL;
   MdmObjectId oid;

   oid = (isPpp ? MDMOID_WAN_PPP_CONN_PORTMAPPING :
                  MDMOID_WAN_IP_CONN_PORTMAPPING);

   if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return SOAP_ACTION_FAILED;
   }

    cmsLog_debug(" requested portMappingIndex=%d",portMappingIndex);

    while (!found &&
           (cmsObj_getNextInSubTree(oid, parentIidStack, &iidStack,
                                    (void **)&port_mapping) == CMSRET_SUCCESS))
    {
       /* As instance Id need not be sequential(due to add/delete)
        * we maintain our own index incrementing by 1 for each portmapping found */
       if(localIndex == portMappingIndex)
       {
          found = TRUE;

          errorinfo |= OutputCharValueToAC(ac, "NewRemoteHost", port_mapping->remoteHost);
          errorinfo |= OutputNumValueToAC(ac, "NewExternalPort", port_mapping->externalPort);
          errorinfo |= OutputNumValueToAC(ac, "NewInternalPort", port_mapping->internalPort);
          errorinfo |= OutputCharValueToAC(ac, "NewInternalClient", port_mapping->internalClient);
          errorinfo |= OutputCharValueToAC(ac, "NewProtocol",  port_mapping->portMappingProtocol);
          errorinfo |= OutputCharValueToAC(ac, "NewPortMappingDescription", port_mapping->portMappingDescription);
          errorinfo |= OutputNumValueToAC(ac, "NewLeaseDuration", port_mapping->portMappingLeaseDuration);
          errorinfo |= OutputCharValueToAC(ac, "NewEnabled",
                              (port_mapping->portMappingEnabled ? "1" : "0"));
       }
       localIndex++;
       cmsObj_free((void **) &port_mapping);
    }

    cmsLck_releaseLock();

    if (errorinfo) {
       return errorinfo;
    }

    if(!found) {
       return SOAP_SPECIFIEDARRAYINDEXINVALID;
    }

    return 0;
}


int upnp_getSpecificPortMapping_igd(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
            const char *protocol, UINT16 externalPort, const char *remoteHost,
            PAction ac)
{
   int errorinfo = 0;
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   // We use pppConnPortMapping object even when dealing with IpConnPortmapping.
   // This is OK because these two objects have the exact same layout.  Only
   // the oid is different.
   WanPppConnPortmappingObject *port_mapping = NULL;
   MdmObjectId oid;

   oid = (isPpp ? MDMOID_WAN_PPP_CONN_PORTMAPPING :
                  MDMOID_WAN_IP_CONN_PORTMAPPING);

   if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return SOAP_ACTION_FAILED;
   }

   while (!found &&
          (cmsObj_getNextInSubTree(oid, parentIidStack, &iidStack,
                                   (void **)&port_mapping) == CMSRET_SUCCESS))
   {
      if((port_mapping->externalPort == externalPort)
            &&(0 == cmsUtl_strcmp(port_mapping->portMappingProtocol, protocol))
            &&(0 == cmsUtl_strcmp(port_mapping->remoteHost,remoteHost)))
      {
         found = TRUE;

         errorinfo |= OutputNumValueToAC(ac, "NewInternalPort", port_mapping->internalPort);
         errorinfo |= OutputCharValueToAC(ac, "NewInternalClient", port_mapping->internalClient);
         errorinfo |= OutputCharValueToAC(ac, "NewPortMappingDescription", port_mapping->portMappingDescription);
         errorinfo |= OutputNumValueToAC(ac, "NewLeaseDuration", port_mapping->portMappingLeaseDuration);
         errorinfo |= OutputCharValueToAC(ac, "NewEnabled",
                             (port_mapping->portMappingEnabled ? "1" : "0"));
      }
      cmsObj_free((void **) &port_mapping);
   }

   cmsLck_releaseLock();

   if (errorinfo) {
      return errorinfo;
   }

   if(!found) {
      return SOAP_NOSUCHENTRYINARRAY;
   }

   return 0;
}

#endif  /* DMP_BASELINE_1 */


