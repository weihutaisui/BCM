/**********************************************************************************
** <:copyright-BRCM:2017:proprietary:standard
** 
**    Copyright (c) 2017 Broadcom 
**    All Rights Reserved
** 
**  This program is the proprietary software of Broadcom and/or its
**  licensors, and may only be used, duplicated, modified or distributed pursuant
**  to the terms and conditions of a separate, written license agreement executed
**  between you and Broadcom (an "Authorized License").  Except as set forth in
**  an Authorized License, Broadcom grants no license (express or implied), right
**  to use, or waiver of any kind with respect to the Software, and Broadcom
**  expressly reserves all rights in and to the Software and all intellectual
**  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
**  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
**  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
** 
**  Except as expressly set forth in the Authorized License,
** 
**  1. This program, including its structure, sequence and organization,
**     constitutes the valuable trade secrets of Broadcom, and you shall use
**     all reasonable efforts to protect the confidentiality thereof, and to
**     use this information only in connection with your use of Broadcom
**     integrated circuit products.
** 
**  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
**     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
**     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
**     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
**     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
**     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
**     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
**     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
**     PERFORMANCE OF THE SOFTWARE.
** 
**  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
**     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
**     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
**     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
**     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
**     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
**     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
**     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
**     LIMITED REMEDY.
** :>
***********************************************************************************/
/* General includes */
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <fcntl.h>
#include <net/if.h>
#include <unistd.h>
#include <bcm_local_kernel_include/linux/sockios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <linux/if_arp.h>

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

//#include <bcmtypes.h>
//#include <bcmatmapi.h>
#include <bcmnet.h>
#include <bcmxtmcfg.h>
//#include <board_api.h>

/* SNMP includes */
#include "asn1.h"
#include "snmp.h"
#include "agtMib.h"
#include "agtEngine.h"
#include "ifMib.h"
//#include "adslMibHelper.h"
//#include "atmMibHelper.h"

//#define SNMP_DEBUG_FLAG


INTF_INFO_LIST ifInfoList;

void ifAddIfToList(pINTF_INFO pIfNew) {
#ifdef SNMP_DEBUG_FLAG
  printf("ifAddIfToList(): name %s, index %d\n",pIfNew->if_name,pIfNew->if_index);
#endif

  if (ifInfoList.pIf == NULL) {
    ifInfoList.pIf = pIfNew;
    ifInfoList.pIfTail = pIfNew;
  }
  else {
    ifInfoList.pIfTail->next = pIfNew;
    ifInfoList.pIfTail = pIfNew;
  }
  ifInfoList.ifCount++;
}

int if_type_from_name(char *name) {
  if (strcmp(name,IF_BRCM_DSL_DEV_NAME) == 0)
    return IF_IANAIFTYPE_ADSL;
  if (strcmp(name,IF_BRCM_ATM_DEV_NAME) == 0)
    return IF_IANAIFTYPE_ATM;
  if ((strcmp(name,IF_BRCM_ATM_CPCS_DEV_NAME) == 0) ||
      (strncmp(name,IF_BRCM_NAS_DEV_NAME,strlen(IF_BRCM_NAS_DEV_NAME))) == 0)
    return IF_IANAIFTYPE_AAL5;
  if (strncmp(name,IF_BRCM_PPP_DEV_NAME,strlen(IF_BRCM_PPP_DEV_NAME)) == 0)
    return IF_IANAIFTYPE_PPP;
  if (strcmp(name,IF_BRCM_LO_DEV_NAME) == 0)
    return IF_IANAIFTYPE_SOFTWARELOOPBACK;
  else
    return IF_IANAIFTYPE_ETHERNETCSMACD;

}

void ifMibFree(void)
{
  pINTF_INFO pIf;

  while (ifInfoList.pIf) {
    pIf = ifInfoList.pIf;
    ifInfoList.pIf = ifInfoList.pIf->next;
    free(pIf->if_name);
    free(pIf);
  }
  ifInfoList.ifCount = 0;
}

static int ifGetWanStats(int index)
{

   WanDslLinkCfgObject *dslLinkCfg = NULL;   
   SINT32 vpi;
   SINT32 vci;
   char serviceStr[BUFLEN_32]={0};
   char interfaceStr[BUFLEN_32]={0};
   char protocolStr[BUFLEN_16]={0};
   InstanceIdStack iidStack;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *wanPppConn = NULL;   
   WanIpConnObject  *wanIpConn = NULL;
   UINT32 rxByte, rxPacket, rxErr, rxDrop;
   UINT32 txByte, txPacket, txErr, txDrop;
   struct ifreq    ifrq;
    int             s;
    WanIpConnStatsObject *wanIpConnSts;
    WanPppConnStatsObject *wanPppConnSts;
	
    pINTF_INFO pIfNew;
    char line[IF_LINE_LENGTH], name[64];
    int len=sizeof(ifrq.ifr_name);
    int ifIndex = IF_INDEX_BASE+index;
    int num = 0;
    char *tempPtr = NULL;
   
   if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      SNMP_DEBUG("Socket open failure in ifGetWanStats()\n");
      return -1;
    }
   
     while (cmsObj_getNext(MDMOID_WAN_DSL_LINK_CFG, &parentIidStack,
                              (void **)&dslLinkCfg) == CMSRET_SUCCESS)
   {
      /* get vpi/vci string */
     // cmsUtl_atmVpiVciStrToNum(dslLinkCfg->destinationAddress, &vpi, &vci);
         
      INIT_INSTANCE_ID_STACK(&iidStack);
   
      /* get the related wanIpConn obj */
      while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &parentIidStack, &iidStack,
                                     (void **)&wanIpConn) == CMSRET_SUCCESS)
      {         
         snprintf(serviceStr, sizeof(interfaceStr), wanIpConn->name);
         snprintf(interfaceStr, sizeof(serviceStr), wanIpConn->X_BROADCOM_COM_IfName);
         cmsObj_free((void **) &wanIpConn);
		 
	  if( cmsObj_get(MDMOID_WAN_IP_CONN_STATS,  &iidStack, 0, (void * *)&wanIpConnSts) == CMSRET_SUCCESS)
	  {
              rxByte    =  wanIpConnSts->ethernetBytesReceived;
		rxPacket =  wanIpConnSts->ethernetPacketsReceived;
		rxErr      =  wanIpConnSts->X_BROADCOM_COM_RxErrors;
		rxDrop    = wanIpConnSts->X_BROADCOM_COM_RxDrops;
		txByte     = wanIpConnSts->ethernetBytesSent;
              txPacket  = wanIpConnSts->ethernetPacketsSent;
              txErr       =  wanIpConnSts->X_BROADCOM_COM_TxErrors;
		txDrop    =  wanIpConnSts->X_BROADCOM_COM_TxDrops;
		cmsObj_free((void **) &wanIpConnSts);
	   }
      } /* wanIpConn */

      INIT_INSTANCE_ID_STACK(&iidStack);
      /* get the related wanPppConn obj */
      while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &parentIidStack, &iidStack,
                                     (void **)&wanPppConn) == CMSRET_SUCCESS)
      {
         snprintf(serviceStr, sizeof(interfaceStr), wanPppConn->name);
         snprintf(interfaceStr, sizeof(serviceStr), wanPppConn->X_BROADCOM_COM_IfName);
         cmsObj_free((void **) &wanPppConn);
	  if( cmsObj_get(MDMOID_WAN_PPP_CONN_STATS,  &iidStack, 0, (void * *)&wanPppConnSts) == CMSRET_SUCCESS)
	  {
              rxByte    =  wanPppConnSts->ethernetBytesReceived;
		rxPacket =  wanPppConnSts->ethernetPacketsReceived;
		rxErr      =  wanPppConnSts->X_BROADCOM_COM_RxErrors;
		rxDrop    = wanPppConnSts->X_BROADCOM_COM_RxDrops;
		txByte     = wanPppConnSts->ethernetBytesSent;
              txPacket  = wanPppConnSts->ethernetPacketsSent;
              txErr      =  wanPppConnSts->X_BROADCOM_COM_TxErrors;
		txDrop    = wanPppConnSts->X_BROADCOM_COM_TxDrops;
		cmsObj_free((void **) &wanPppConnSts);
	   }
      } /* wanPppConn */



      /* statistics */
      /* Technically, the httpd code should not call the rut function directly.
       * It should do a cmsObj_get of the object, which will call the stl handler
       * function, which will call the rut_ function.  Then the httpd code should
       * get the stats out of the object and then free it.
       */
      pIfNew = (pINTF_INFO)calloc(1, sizeof(INTF_INFO));
      if (pIfNew == NULL) {
        cmsLog_error("Fail to allocate INTF_INFO in ifGetWanStats()\n");
	 close(s);
        return -1;
      }
      memset(pIfNew,0,sizeof(INTF_INFO));
	  
      memcpy(name,  interfaceStr,sizeof(name));
      pIfNew->if_name = (char *)strdup(name);
      pIfNew->if_ibytes     =  rxByte;
      pIfNew->if_ipackets  =  rxPacket;
      pIfNew->if_ierrors    =  rxErr;
      pIfNew->if_iqdrops   =  rxDrop;
      pIfNew->if_obytes    =  txByte;
      pIfNew->if_opackets =  txPacket;
      pIfNew->if_oerrors   =  txErr;
      pIfNew->if_odrop     =  txDrop; 

     
      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFADDR, &ifrq) >= 0)
        pIfNew->if_addr = ifrq.ifr_addr;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFBRDADDR, &ifrq) >= 0)
        pIfNew->ifu_broadaddr = ifrq.ifr_broadaddr;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFNETMASK, &ifrq) >= 0)
        pIfNew->ia_subnetmask = ifrq.ifr_netmask;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      pIfNew->if_flags = ioctl(s, SIOCGIFFLAGS, &ifrq) < 0
        ? 0 : ifrq.ifr_flags;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFHWADDR, &ifrq) != -1)
        memcpy(pIfNew->if_hwaddr, ifrq.ifr_hwaddr.sa_data, 6);

      pIfNew->if_type = if_type_from_name(pIfNew->if_name);

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      pIfNew->if_metric = ioctl(s, SIOCGIFMETRIC, &ifrq) < 0
        ? 0 : ifrq.ifr_metric;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      pIfNew->if_mtu = (ioctl(s, SIOCGIFMTU, &ifrq) < 0)
        ? 0 : ifrq.ifr_mtu;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      pIfNew->if_lastchange = (ioctl(s, SIOCGIFTRANSSTART, &ifrq) < 0)
        ? 0 : ifrq.ifr_ifru.ifru_ivalue;

       switch (pIfNew->if_type)
       {

        case IF_IANAIFTYPE_PPP:
          memset(pIfNew->if_hwaddr, 0, 6);
          pIfNew->if_speed = 0;
          break;
        default:
           break;
        } /*switch */
     
      pIfNew->if_index = ifIndex;
      ifIndex++;
      num++;

      ifAddIfToList(pIfNew);
      cmsObj_free((void **) &dslLinkCfg);

   } /* dslLinkCfg */
   close(s);

   return num;
}
static int ifGetLanStats(void* obj,void* stsObj,int ifcType,int index)
{

    struct ifreq    ifrq;
    int             s;
    pINTF_INFO pIfNew;;
    char line[IF_LINE_LENGTH], name[64];
    int len=sizeof(ifrq.ifr_name);
     int ifIndex = IF_INDEX_BASE;
    //int i = 0;
    char *tempPtr = NULL;
    char sts[32];
   
   PIOCTL_MIB_INFO pNetDevMibInfo;
   IOCTL_MIB_INFO netDevMibInfo;
   LanEthIntfStatsObject *ethStatsObj=NULL;
   LanEthIntfObject *ethObj = NULL;
#ifdef DMP_USBLAN_1
   LanUsbIntfStatsObject *usbStatsObj=NULL;
   LanUsbIntfObject *usbObj = NULL;
#endif /* DMP_USBLAN_1*/
#ifdef BRCM_WLAN
   LanWlanObject *wlanObj = NULL;
//   LanWlanStatsObject *wlanStatsObj=NULL;
#endif

   if(obj == NULL){
       return -1;
   }
   
   if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      SNMP_DEBUG("Socket open failure in ifScanInterfaces()\n");
      return -1;
    }

   pIfNew = (pINTF_INFO)calloc(1, sizeof(INTF_INFO));
   if (pIfNew == NULL) {
        cmsLog_error("Fail to allocate INTF_INFO in ifGetLanStats()\n");
	 close(s);
        return -1;
     }
     memset(pIfNew,0,sizeof(INTF_INFO));

   switch(ifcType)
   {
    case MDMOID_LAN_ETH_INTF:
          ethStatsObj = (LanEthIntfStatsObject *)stsObj;
          ethObj        =  (LanEthIntfObject*)obj;
          cmsUtl_strncpy(name,  ethObj->X_BROADCOM_COM_IfName,sizeof(name));
          memcpy(sts,ethObj->status,sizeof(sts));
          pIfNew->if_name = (char *)strdup(name);
          pIfNew->if_ibytes     =  ethStatsObj->bytesReceived;
          pIfNew->if_ipackets  =  ethStatsObj->packetsReceived;
          pIfNew->if_ierrors    =  ethStatsObj->X_BROADCOM_COM_RxErrors;
          pIfNew->if_iqdrops   =  ethStatsObj->X_BROADCOM_COM_RxDrops;
          pIfNew->if_obytes    =  ethStatsObj->bytesSent;
          pIfNew->if_opackets =  ethStatsObj->packetsSent;
          pIfNew->if_oerrors   =  ethStatsObj->X_BROADCOM_COM_TxErrors;
          pIfNew->if_odrop     =  ethStatsObj->X_BROADCOM_COM_TxDrops;
          break;
		  
#ifdef DMP_USBLAN_1		  
    case MDMOID_LAN_USB_INTF:
          usbStatsObj = (LanUsbIntfStatsObject *)stsObj;
          usbObj        =  (LanUsbIntfObject*)obj;
          cmsUtl_strncpy(name,  usbObj->X_BROADCOM_COM_IfName,sizeof(name));
          memcpy(sts,usbObj->status,sizeof(sts));
          pIfNew->if_name = (char *)strdup(name);
          pIfNew->if_ibytes     =  usbStatsObj->bytesReceived;
          pIfNew->if_ipackets  =  usbStatsObj->X_BROADCOM_COM_PacketsReceived;
          pIfNew->if_ierrors    =  usbStatsObj->X_BROADCOM_COM_RxErrors;
          pIfNew->if_iqdrops   =  usbStatsObj->X_BROADCOM_COM_RxDrops;
          pIfNew->if_obytes    =  usbStatsObj->bytesSent;
          pIfNew->if_opackets =  usbStatsObj->X_BROADCOM_COM_PacketsSent;
          pIfNew->if_oerrors   =  usbStatsObj->X_BROADCOM_COM_TxErrors;
          pIfNew->if_odrop     =  usbStatsObj->X_BROADCOM_COM_TxDrops;
          break;
#endif

#ifdef BRCM_WLAN
    case  MDMOID_LAN_WLAN:
          wlanObj        =  (LanWlanObject*)obj;
          cmsUtl_strncpy(name,  wlanObj->X_BROADCOM_COM_IfName,sizeof(name));
          pIfNew->if_name = (char *)strdup(name);
          memcpy(sts,wlanObj->status,sizeof(sts));
          pIfNew->if_ibytes     =  wlanObj->totalBytesReceived;
          pIfNew->if_ipackets  =  wlanObj->totalPacketsReceived;
          pIfNew->if_ierrors    =  wlanObj->X_BROADCOM_COM_RxErrors;
          pIfNew->if_iqdrops   =  wlanObj->X_BROADCOM_COM_RxDrops;
          pIfNew->if_obytes    =  wlanObj->totalBytesSent;
          pIfNew->if_opackets =  wlanObj->totalPacketsSent;
          pIfNew->if_oerrors   =  wlanObj->X_BROADCOM_COM_TxErrors;
          pIfNew->if_odrop     =  wlanObj->X_BROADCOM_COM_TxDrops;
          break;
  #endif
  
    default:
          break;
    }


      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFADDR, &ifrq) >= 0)
        pIfNew->if_addr = ifrq.ifr_addr;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFBRDADDR, &ifrq) >= 0)
        pIfNew->ifu_broadaddr = ifrq.ifr_broadaddr;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFNETMASK, &ifrq) >= 0)
        pIfNew->ia_subnetmask = ifrq.ifr_netmask;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      pIfNew->if_flags = ioctl(s, SIOCGIFFLAGS, &ifrq) < 0
        ? 0 : ifrq.ifr_flags;

     if(strcmp(sts,"Up") == 0)
         pIfNew->if_flags |= IFF_RUNNING;
      else
        pIfNew->if_flags &= ~IFF_RUNNING;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFHWADDR, &ifrq) != -1)
        memcpy(pIfNew->if_hwaddr, ifrq.ifr_hwaddr.sa_data, 6);

      pIfNew->if_type = if_type_from_name(pIfNew->if_name);

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      pIfNew->if_metric = ioctl(s, SIOCGIFMETRIC, &ifrq) < 0
        ? 0 : ifrq.ifr_metric;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      pIfNew->if_mtu = (ioctl(s, SIOCGIFMTU, &ifrq) < 0)
        ? 0 : ifrq.ifr_mtu;


      switch (pIfNew->if_type)
        {
        case IF_IANAIFTYPE_ETHERNETCSMACD:
        case IF_IANAIFTYPE_SOFTWARELOOPBACK:
          cmsUtl_strncpy(ifrq.ifr_name, name, len);
          ifrq.ifr_data = (char*)&netDevMibInfo;
          if (ioctl(s, SIOCMIBINFO, &ifrq) >= 0) {
            pNetDevMibInfo = (IOCTL_MIB_INFO *)(ifrq.ifr_data);
            pIfNew->if_speed = pNetDevMibInfo->ulIfSpeed;
            pIfNew->if_lastchange = pNetDevMibInfo->ulIfLastChange;
          }
          break;
		  
        default:
          pIfNew->if_speed = 0;
          memset(pIfNew->if_hwaddr, 0, 6);
	   break;
        } /*switch */
    
      pIfNew->if_index = ifIndex+index;

      /* add interface to link list */
      ifAddIfToList(pIfNew);
      close(s);
      return 1;
}

#ifdef BUILD_XDSLCTL
static int getAdslOperStatus(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfObj = NULL;
   int xDsl2Mode = 0, vdslMode = 0;
   UBOOL8 foundLinkUp=FALSE;

   cmsLog_debug("entered");

   /*
    * On the 6368, there are 2 WAN DSLIntfCfg objects, one for ATM and one for PTM.
    * I don't know which one will have link up.  So if I find one that has link up,
    * use that one.  Otherwise, use the first one, which is the ATM one (and it 
    * will still have link down.)
    * This algorithm will work even when we have 1 WANDevice.
    */
   while (!foundLinkUp && (cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfObj) == CMSRET_SUCCESS))
   {
      if ((dslIntfObj->enable) &&
          (0 == cmsUtl_strcmp(dslIntfObj->status, MDMVS_UP)))
      {
         foundLinkUp = TRUE;
         /* dslIntfObj is still pointing to the object that is up. */
      }

      cmsObj_free((void **) &dslIntfObj);
   }

   if(foundLinkUp)
   {
        return IF_OPER_STATUS_UP;
   }
   else
   {
        return IF_OPER_STATUS_DOWN;
   }

}

static int adslMibCurrAdminStatus=IF_ADMIN_STATUS_UP;

static int setAdslAdminStatus(int status)
{
  int ret=0;

  if (status != adslMibCurrAdminStatus) {
    if (status == IF_ADMIN_STATUS_UP) {
      if (devCtl_adslConnectionStart() == CMSRET_SUCCESS)
	adslMibCurrAdminStatus = IF_ADMIN_STATUS_UP;
      else
	ret = -1;
    }
    else if (status == IF_ADMIN_STATUS_DOWN) {
      if (devCtl_adslConnectionStop() == CMSRET_SUCCESS)
	adslMibCurrAdminStatus = IF_ADMIN_STATUS_DOWN;
      else
	ret = -1;
    }
    else
      ret =  -1; /* not supporting anything else */
  }
  return (ret);
}

static int getAdslAdminStatus(void)
{
     return adslMibCurrAdminStatus;
}

static int  getAtmIntfOperStatus(void)
{
    int  i;
    XTM_INTERFACE_CFG Cfg;
    UINT32 ulPortId ;
    CmsRet nRet = CMSRET_SUCCESS;

   for(i=0; i< MAX_PHY_PORTS;i++)
   {
      memset( &Cfg, 0x00, sizeof(Cfg) );
      ulPortId=  PORT_TO_PORTID(i);
      /* Read the current interface configuration. */
      nRet = devCtl_xtmGetInterfaceCfg( ulPortId, &Cfg );
      if( nRet == CMSRET_SUCCESS )
      {
           if(Cfg.ulIfOperStatus == ADMSTS_UP)
                return IF_ADMIN_STATUS_UP;
	    else
		  return IF_ADMIN_STATUS_DOWN; 
       }
    }
   
    return IF_ADMIN_STATUS_DOWN;
}


static int getAtmIntfAdminStatus(void)
{
    int  i;
    XTM_INTERFACE_CFG Cfg;
    UINT32 ulPortId ;
    CmsRet nRet = CMSRET_SUCCESS;

   for(i=0; i< MAX_PHY_PORTS;i++)
   {
      memset( &Cfg, 0x00, sizeof(Cfg) );
      ulPortId=  PORT_TO_PORTID(i);
      /* Read the current interface configuration. */
      nRet = devCtl_xtmGetInterfaceCfg( ulPortId, &Cfg );
      if( nRet == CMSRET_SUCCESS )
      {
           if(Cfg.ulIfAdminStatus == ADMSTS_UP)
                return IF_ADMIN_STATUS_UP;
	    else
		  return IF_ADMIN_STATUS_DOWN; 
       }
    }
   
    return IF_ADMIN_STATUS_DOWN;
}


static int setAtmIntfAdminStatus(int status)
{
    int  i;
    XTM_INTERFACE_CFG Cfg;
    UINT32 ulPortId ;
    int ret = 0;
    CmsRet nRet = CMSRET_SUCCESS;

     UINT32 ulAdminStatus = (UINT32) -1;

     if( IF_ADMIN_STATUS_UP == status ) 
         ulAdminStatus = ADMSTS_UP;
     else
         ulAdminStatus = ADMSTS_DOWN;

     for(i=0; i< MAX_PHY_PORTS;i++)
     {
         memset( &Cfg, 0x00, sizeof(Cfg) );
         ulPortId=  PORT_TO_PORTID(i);
         /* Read the current interface configuration. */
         nRet = devCtl_xtmGetInterfaceCfg( ulPortId, &Cfg );
         if( nRet == CMSRET_SUCCESS )
         {
                Cfg.ulIfAdminStatus = ulAdminStatus;
                nRet = devCtl_xtmSetInterfaceCfg( ulPortId, &Cfg );
		  if(nRet != CMSRET_SUCCESS)
		     ret = -1;
          }
	   else
	   {
	       ret = -1;
	   }
      }
      
    return( ret );
}

static unsigned long getAdslIfSpeed(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfObj = NULL;
   WanDslIntfStatsTotalObject *dslTotalStatsObj = NULL;
   int xDsl2Mode = 0, vdslMode = 0;
   UBOOL8 foundLinkUp=FALSE;
   unsigned long rate;

   cmsLog_debug("entered");

   /*
    * On the 6368, there are 2 WAN DSLIntfCfg objects, one for ATM and one for PTM.
    * I don't know which one will have link up.  So if I find one that has link up,
    * use that one.  Otherwise, use the first one, which is the ATM one (and it 
    * will still have link down.)
    * This algorithm will work even when we have 1 WANDevice.
    */
   while ((cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfObj) == CMSRET_SUCCESS))
   {
      if ((dslIntfObj->enable) &&
          (0 == cmsUtl_strcmp(dslIntfObj->status, MDMVS_UP)))
      {
         foundLinkUp = TRUE;
         /* dslIntfObj is still pointing to the object that is up. */
         break;
      }

      cmsObj_free((void **) &dslIntfObj);
   }

   if (!foundLinkUp)
   {
      /*
       * WAN link is not up, so just get the first instance of the DSLIntfObj
       * and report based on that object (which currently has link down anyways).
       */
      INIT_INSTANCE_ID_STACK(&iidStack);
      cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfObj);
   }

   // [JIRA SWBCACPE-10306]: TR-098 requires
   // downstreamCurrRate
   // in Kbps instead of Bps
   rate = dslIntfObj->downstreamCurrRate * 1000;

   cmsObj_free((void **) &dslIntfObj);

   if(foundLinkUp)
   {
         return rate;
   }
   else
   {
        return 0;
   }
       
 }

#endif
static int ifGetOtherInfStats(char* ifName,int index)
{
     UINT32 rxByte, rxPacket, rxErr, rxDrop;
     UINT32 txByte, txPacket, txErr, txDrop;
     UINT32 byteMultiRx, packetMulitRx, packetUniRx, packetBcastRx;
     UINT32 byteMultiTx, packetMulitTx, packetUniTx, packetBcastTx;
     
     int ifIndex = IF_INDEX_BASE;
     char interfaceStr[32]={0};
     pINTF_INFO pIfNew;;
     char name[64]; 
     int             s; 
     struct ifreq    ifrq;
     int len=sizeof(ifrq.ifr_name); 
	 
     cmsUtl_strncpy(interfaceStr,ifName,sizeof(interfaceStr));
     rut_getIntfStats(interfaceStr, 
        &rxByte, &rxPacket, &byteMultiRx, &packetMulitRx, &packetUniRx, &packetBcastRx, &rxErr, &rxDrop, 
        &txByte, &txPacket, &byteMultiTx, &packetMulitTx, &packetUniTx, &packetBcastTx, &txErr, &txDrop);
     
     if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      SNMP_DEBUG("Socket open failure in ifGetDsl0Stats()\n");
      return -1;
    }

    pIfNew = (pINTF_INFO)calloc(1, sizeof(INTF_INFO));
    if (pIfNew == NULL) {
        cmsLog_error("Fail to allocate INTF_INFO in ifGetWanStats()\n");
	 close(s);
        return -1;
      }
      memset(pIfNew,0,sizeof(INTF_INFO));
	  
      cmsUtl_strncpy(name,  interfaceStr,sizeof(name));
      pIfNew->if_name = (char *)strdup(name);
      pIfNew->if_ibytes     =  rxByte;
      pIfNew->if_ipackets  =  rxPacket;
      pIfNew->if_ierrors    =  rxErr;
      pIfNew->if_iqdrops   =  rxDrop;
      pIfNew->if_obytes    =  txByte;
      pIfNew->if_opackets =  txPacket;
      pIfNew->if_oerrors   =  txErr;
      pIfNew->if_odrop     =  txDrop; 
      pIfNew->if_index = ifIndex+index;
	  
      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFADDR, &ifrq) >= 0)
        pIfNew->if_addr = ifrq.ifr_addr;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFBRDADDR, &ifrq) >= 0)
        pIfNew->ifu_broadaddr = ifrq.ifr_broadaddr;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFNETMASK, &ifrq) >= 0)
        pIfNew->ia_subnetmask = ifrq.ifr_netmask;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      pIfNew->if_flags = ioctl(s, SIOCGIFFLAGS, &ifrq) < 0
        ? 0 : ifrq.ifr_flags;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFHWADDR, &ifrq) != -1)
        memcpy(pIfNew->if_hwaddr, ifrq.ifr_hwaddr.sa_data, 6);

      pIfNew->if_type = if_type_from_name(pIfNew->if_name);

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      pIfNew->if_metric = ioctl(s, SIOCGIFMETRIC, &ifrq) < 0
        ? 0 : ifrq.ifr_metric;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      pIfNew->if_mtu = (ioctl(s, SIOCGIFMTU, &ifrq) < 0)
        ? 0 : ifrq.ifr_mtu;

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      pIfNew->if_lastchange = (ioctl(s, SIOCGIFTRANSSTART, &ifrq) < 0)
        ? 0 : ifrq.ifr_ifru.ifru_ivalue;
	  
      switch (pIfNew->if_type)
      {
#ifdef BUILD_XDSLCTL
        case IF_IANAIFTYPE_ADSL:
          memset(pIfNew->if_hwaddr, 0, 6);
          pIfNew->if_speed = getAdslIfSpeed();
          if (getAdslOperStatus() == IF_OPER_STATUS_UP)
            pIfNew->if_flags |= IFF_RUNNING;
          else
            pIfNew->if_flags &= ~IFF_RUNNING;
          if (getAdslAdminStatus() == IF_ADMIN_STATUS_UP)
            pIfNew->if_flags |= IFF_UP;
          else
            pIfNew->if_flags &= ~IFF_UP;
          break;
        case IF_IANAIFTYPE_ATM:
        case IF_IANAIFTYPE_AAL5:
          memset(pIfNew->if_hwaddr, 0, 6);
          pIfNew->if_speed = getAdslIfSpeed();
          if (getAtmIntfOperStatus() == IF_OPER_STATUS_UP)
            pIfNew->if_flags |= IFF_RUNNING;
          else
            pIfNew->if_flags &= ~IFF_RUNNING;
          if (getAtmIntfAdminStatus() == IF_ADMIN_STATUS_UP)
            pIfNew->if_flags |= IFF_UP;
          else
            pIfNew->if_flags &= ~IFF_UP;

          break;
#endif
        default:
          pIfNew->if_speed = 0;
          memset(pIfNew->if_hwaddr, 0, 6);
	   break;
        } /*switch */
   
      pIfNew->if_index = ifIndex+index;
      ifIndex++;

     ifAddIfToList(pIfNew);
     close(s);
     return 1;
}



int ifScanInterfaces(void)
{
    int rel = 0;
    int index = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    LanEthIntfStatsObject *ethStatsObj=NULL;
    LanEthIntfObject *ethObj = NULL;
#ifdef DMP_USBLAN_1
    LanUsbIntfStatsObject *usbStatsObj=NULL;
    LanUsbIntfObject *usbObj = NULL;
#endif /* DMP_USBLAN_1*/
#ifdef BRCM_WLAN
    LanWlanObject *wlanObj = NULL;
//   LanWlanStatsObject *wlanStatsObj=NULL;
#endif
    char name[32];


    /* Remove interfaces in current list and create a new one */
    ifMibFree();
   
   while (cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj) == CMSRET_SUCCESS)
   {
      if (cmsObj_get(MDMOID_LAN_ETH_INTF_STATS, &iidStack, 0, (void **) &ethStatsObj) == CMSRET_SUCCESS)
      {
          rel = ifGetLanStats(ethObj,ethStatsObj,MDMOID_LAN_ETH_INTF,index);
          cmsObj_free((void **) &ethStatsObj);
	   if(rel < 0)
	   {
	        cmsObj_free((void **) &ethObj);
	        return -1;
	   }
	   index++;
      } /* if eth_itf_stats */
      cmsObj_free((void **) &ethObj);
   }

#ifdef DMP_USBLAN_1
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_LAN_USB_INTF, &iidStack, (void **) &usbObj) == CMSRET_SUCCESS)
   {
      if (cmsObj_get(MDMOID_LAN_USB_INTF_STATS, &iidStack, 0, (void **) &usbStatsObj) == CMSRET_SUCCESS)
      {
        rel = ifGetLanStats(usbObj,usbStatsObj,MDMOID_LAN_USB_INTF,index);
         cmsObj_free((void **) &usbStatsObj);
	  if(rel < 0)
	   {
	        cmsObj_free((void **) &usbObj);
	        return -1;
	   }
	  index++;
      } /* if usb_itf_stats */
      cmsObj_free((void **) &usbObj);
   }
#endif

#ifdef BRCM_WLAN
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_LAN_WLAN, &iidStack, (void **) &wlanObj) == CMSRET_SUCCESS)
   {
      rel = ifGetLanStats(wlanObj,NULL,MDMOID_LAN_WLAN,index);
      cmsObj_free((void **) &wlanObj);
      if(rel < 0)
      {
	    return -1;
      }
      index++;
   }
#endif

   
#ifdef BUILD_XDSLCTL
  rel =  ifGetWanStats( index);

  if(rel < 0)
  	return -1;

  index += rel;
  memcpy(name,IF_BRCM_DSL_DEV_NAME,sizeof(name));
  rel =ifGetOtherInfStats(name,index);

  if(rel < 0)
  	return -1;


  index++;
  memcpy(name,IF_BRCM_ATM_DEV_NAME,sizeof(name));
  rel =ifGetOtherInfStats(name,index); 

  if(rel < 0)
  	return -1;

  index++;
  memcpy(name,IF_BRCM_ATM_CPCS_DEV_NAME,sizeof(name));
  rel =ifGetOtherInfStats(name,index);
  index++;

   if(rel < 0)
  	return -1;
#endif

 
#ifdef SNMP_DEBUG_FLAG
    pINTF_INFO pIf;
    pIf = ifInfoList.pIf;
    if (ifInfoList.ifCount != 0) {
      printf("************---ifInfoList.ifCount %d---************\n",ifInfoList.ifCount);
      while (pIf) {
        printf("ifName %s, ifIndex %d\n",pIf->if_name,pIf->if_index);
        pIf = pIf->next;
      }
    }
#endif

    return 1;
}

int ifGetIfIndexByName(char *name)
{
  pINTF_INFO pIf;

  ifScanInterfaces();
  pIf = ifInfoList.pIf;
  if (ifInfoList.ifCount != 0) {
    while (pIf) {
      if (strcmp(pIf->if_name,name) == 0)
        return (pIf->if_index);
      pIf = pIf->next;
    } /* while */
  } /* ifCount */
  return -1;
}

int ifGetNameByIfIndex(int index, char *name)
{
  pINTF_INFO pIf;

  ifScanInterfaces();
  pIf = ifInfoList.pIf;
  if (ifInfoList.ifCount != 0) {
    while (pIf) {
      if (pIf->if_index == index) {
        strcpy(name,pIf->if_name);
        return 0;
      }
      pIf = pIf->next;
    } /* while */
  } /* ifCount */
  return -1;
}

/* return interface count */
int ifCount()
{
  ifScanInterfaces();
  return ifInfoList.ifCount;
}

/* The number of network interfaces (regardless of their
 * current state) present on this system.
 */
unsigned char *
var_ifNumber(int *var_len, snmp_info_t *mesg,
        int (**write_method)())
{
   long_return = ifCount();
   *var_len = sizeof(long);
   
   return (unsigned char *)&long_return;;

}

/* The value of sysUpTime at the time of the last creation or
 * deletion of an entry in the ifTable.  If the number of
 * entries has been unchanged since the last re-initialization
 * of the local network management subsystem, then this object
 * contains a zero value.
 */
unsigned char *
var_ifTableLastChange(int *var_len, snmp_info_t *mesg,
        int (**write_method)())
{
    /* Add value computations */

    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    return (unsigned char *) NO_MIBINSTANCE;
}

#ifdef BUILD_SNMP_SET
int setPppConnAdminDown(char *devName) 
{

#ifdef NEED_CHANGE

  FILE* fs = NULL;
  char path[100];
  int status = -1;
  int fd;

  sprintf(path,"/proc/var/fyi/wan/%s/wanup",devName);

  fs = fopen(path, "w");
  if (fs != NULL) {
    rewind(fs);
    fputs("0\n",fs);
    fputs("\0",fs);
    fclose(fs);
    status = 0;
  }

  if( (fd = open( "/dev/brcmboard", O_RDWR )) != -1 ) {
    ioctl( fd, BOARD_IOCTL_WAKEUP_MONITOR_TASK, NULL);
    close(fd);
  }

  return status;
#else
  return -1;
#endif
}

int write_ifEntry(int action,unsigned char *var_val, unsigned char varval_type, int var_val_len,
                  unsigned char *statP, Oid *name, int name_len)
{
    int fd;
    struct ifreq   ifrq;
    int index, value, column;
    int namelen;

    namelen = (int)name->namelen;
    column = (int)name->name[namelen-2];

    switch (action) 
      {
      case RESERVE1:
        if (varval_type != SNMP_INTEGER)
          return SNMP_ERROR_WRONGTYPE;
        if (var_val_len > sizeof(int)) 
          return SNMP_ERROR_WRONGLENGTH;
        if ((*var_val < IF_ADMIN_STATUS_UP) || (*var_val > IF_ADMIN_STATUS_TESTING))
          return SNMP_ERROR_WRONGVALUE;

        if (column != I_ifAdminStatus)
          return SNMP_ERROR_NOTWRITABLE;

        break;
      case RESERVE2:
        break;
      case COMMIT:
        index = (int)name->name[namelen-1];
        value = *var_val;

        if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
          SNMP_DEBUG("socket open failure in Write_ifEntry\n");
          return SNMP_ERROR_COMMITFAILED;
        }

        /* get ifName first */
        ifrq.ifr_ifindex = index;
        if (ioctl(fd, SIOCGIFNAME, &ifrq) < 0) {
          SNMP_DEBUG("WriteIfEntry unable to locate device name\n");
          close(fd);	  
          return SNMP_ERROR_COMMITFAILED;
        }

#ifdef BUILD_XDSLCTL
        if (strcmp(ifrq.ifr_name,IF_BRCM_DSL_DEV_NAME) == 0) {
          if (setAdslAdminStatus(value) < 0) {
            close(fd);	  
            return SNMP_ERROR_COMMITFAILED;
          }
        }
        else if ((strcmp(ifrq.ifr_name,"atm0") == 0) ||
                 (strcmp(ifrq.ifr_name,"cpcs0") == 0)) {
          if (setAtmIntfAdminStatus(value) < 0) {
            close(fd);	  
            return SNMP_ERROR_COMMITFAILED;
          }
        }
        else if ((strncmp(ifrq.ifr_name,"ppp",strlen("ppp"))) == 0) {
          /* value is always down since after ppp is admin down the first time, dev is gone */
          if (setPppConnAdminDown(ifrq.ifr_name) < 0) {
            close(fd);	  
            return SNMP_ERROR_COMMITFAILED;
          }
        }
#else
        if ((strncmp(ifrq.ifr_name,"ppp",strlen("ppp"))) == 0) {
          /* value is always down since after ppp is admin down the first time, dev is gone */
          if (setPppConnAdminDown(ifrq.ifr_name) < 0) {
            close(fd);	  
            return SNMP_ERROR_COMMITFAILED;
          }
        }
#endif
        if (value == IF_ADMIN_STATUS_UP) 
          ifrq.ifr_flags |= IFF_UP;
        else 
          ifrq.ifr_flags &= ~IFF_UP;
        if (ioctl(fd, SIOCSIFFLAGS,  &ifrq) < 0) {
          close(fd);	  
          return SNMP_ERROR_COMMITFAILED;
        }
        close(fd);
        break;
      case FREE:
        break;
      }
    return SNMP_ERROR_NOERROR;
}
#endif /* BUILD_SNMP_SET */

/* An entry containing management information applicable to a
 * particular interface.
 */
unsigned char *
var_ifEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType,
            snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  /* Last subOID of COLUMNAR OID is column */
  int	column = newoid->name[(newoid->namelen - 1)];
  int	result;
  pINTF_INFO pIf;
  int index;

  cmsLog_debug("newoid(%p), reqoid(%p), searchType(%d), column(%d)",
               newoid, reqoid, searchType, column);

  ifScanInterfaces();

  /* Add indexes for the entry to OID newname */
  /*     ifIndex =  derived from ifInfoList somehow */
  pIf = ifInfoList.pIf;
  /* newoid: column (namelen-1), ifIndex (name_len) , value */
  index = newoid->namelen;
  newoid->namelen++;
  while (pIf) {
    newoid->name[index] = pIf->if_index;
    /* Determine whether it is the requested OID  */

    result = compare(reqoid, newoid);

    if (((searchType == EXACT) && (result == 0)) ||
        ((searchType == NEXT) && (result < 0)))
      break; /* found */
    pIf = pIf->next;
  } /* for all interfaces */

  /* should return end-mibinstance, so next can return the next index of the next table; shouldn't be nomibinstance */
  if (pIf == NULL)
    return NO_MIBINSTANCE;
    
  *var_len = sizeof(long);    

#ifdef BUILD_SNMP_SET
  /* Set write-function */
  *write_method = (WRITE_METHOD)write_ifEntry;
#else
  *write_method = 0;
#endif
    /* Set size (in bytes) and return address of the variable */
    switch (column)
      {
      case I_ifIndex:
        long_return = pIf->if_index;
        return (unsigned char *)&long_return;
      case I_ifDescr:
        *var_len = strlen(pIf->if_name);
        return (unsigned char *)pIf->if_name;
      case I_ifType:
        return (unsigned char *)&pIf->if_type;
      case I_ifMtu:
        return (unsigned char *)&pIf->if_mtu;
      case I_ifSpeed:
        return (unsigned char *)&pIf->if_speed;
      case I_ifPhysAddress:
        if ((pIf->if_hwaddr[0] == 0) && (pIf->if_hwaddr[1] == 0) &&
            (pIf->if_hwaddr[2] == 0) && (pIf->if_hwaddr[3] == 0) &&
            (pIf->if_hwaddr[4] == 0) && (pIf->if_hwaddr[5] == 0))
          *var_len = 0;
        else
          *var_len = sizeof(pIf->if_hwaddr);
        return (unsigned char *)pIf->if_hwaddr;
      case I_ifAdminStatus:
        if (pIf->if_flags & IFF_UP)
          long_return = IF_ADMIN_STATUS_UP;
        else 
          long_return = IF_ADMIN_STATUS_DOWN;
        return (unsigned char *)&long_return;
      case I_ifOperStatus:
        if (pIf->if_flags & IFF_RUNNING)
          long_return = IF_OPER_STATUS_UP;
        else 
          long_return = IF_OPER_STATUS_DOWN;

        return (unsigned char *)&long_return;
      case I_ifLastChange:
        return (unsigned char *)&pIf->if_lastchange;
      case I_ifInOctets:
        return (unsigned char *)&pIf->if_ibytes;
      case I_ifInUcastPkts:
        return (unsigned char *)&pIf->if_ipackets;
      case I_ifInNUcastPkts:
        /* deprecated */
        long_return = 0;
        return (unsigned char *)&long_return;
      case I_ifInDiscards:
        return (unsigned char *)&pIf->if_iqdrops;
      case I_ifInErrors:
        return (unsigned char *)&pIf->if_ierrors;
      case I_ifInUnknownProtos:
        /* don't have this info, returning 0 instead */
        long_return = 0;
        return (unsigned char *)&long_return;
      case I_ifOutOctets:
        return (unsigned char *)&pIf->if_obytes;
      case I_ifOutUcastPkts:
        return (unsigned char *)&pIf->if_opackets;
      case I_ifOutNUcastPkts:
        /* deprecated */
        long_return = 0;
        return (unsigned char *)&long_return;
      case I_ifOutDiscards:
        return (unsigned char *)&pIf->if_odrop;
      case I_ifOutErrors:
        return (unsigned char *)&pIf->if_oerrors;
      case I_ifOutQLen:
        /* deprecated */
        long_return = 0;
        return (unsigned char *)&long_return;
      case I_ifSpecific:
        /* deprecated */
        long_return = 0;
        return (unsigned char *)&long_return;
      default:
        return NO_MIBINSTANCE;
      }
} 

/* An entry containing additional management information
 * applicable to a particular interface.
 */

unsigned char *
var_ifXEntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        snmp_info_t *mesg, int (**write_method)())
{
/* Last subOID of COLUMNAR OID is column */
int	column = newoid->name[(newoid->namelen - 1)];
int	result;

   cmsLog_debug("newoid(%p), reqoid(%p), searchType(%d), column(%d)",
                newoid, reqoid, searchType, column);

    while (0 /* Test whether the entry has been found */) {
        /* Add indexes for the entry to OID newname */
        /* Determine whether it is the requested OID  */
        result = compare(reqoid, newoid);
        if (((searchType == EXACT) && (result != 0)) ||
                ((searchType == NEXT) && (result >= 0))) {
            return NO_MIBINSTANCE;
        }
    }
    /* Place here the computation functionality */

    /* Set write-function */
    *write_method = 0;
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    switch (column) {
	case I_ifName:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifInMulticastPkts:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifInBroadcastPkts:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifOutMulticastPkts:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifOutBroadcastPkts:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifHCInOctets:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifHCInUcastPkts:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifHCInMulticastPkts:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifHCInBroadcastPkts:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifHCOutOctets:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifHCOutUcastPkts:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifHCOutMulticastPkts:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifHCOutBroadcastPkts:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifLinkUpDownTrapEnable:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifHighSpeed:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifPromiscuousMode:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifConnectorPresent:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifAlias:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifCounterDiscontinuityTime:
	    return (unsigned char *) NO_MIBINSTANCE;
        default:
            return NO_MIBINSTANCE;
    }
}

/* Information on a particular relationship between two sub-
 * layers, specifying that one sub-layer runs on 'top' of the
 * other sub-layer.  Each sub-layer corresponds to a conceptual
 * row in the ifTable.
 */

unsigned char *
var_ifStackEntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        snmp_info_t *mesg, int (**write_method)())
{
/* Last subOID of COLUMNAR OID is column */
int	column = newoid->name[(newoid->namelen - 1)];
int	result;

    while (0 /* Test whether the entry has been found */) {
        /* Add indexes for the entry to OID newname */
        /* Determine whether it is the requested OID  */
        result = compare(reqoid, newoid);
        if (((searchType == EXACT) && (result != 0)) ||
                ((searchType == NEXT) && (result >= 0))) {
            return NO_MIBINSTANCE;
        }
    }
    /* Place here the computation functionality */

    /* Set write-function */
    *write_method = 0;
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    switch (column) {
	case I_ifStackHigherLayer:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifStackLowerLayer:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifStackStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
        default:
            return NO_MIBINSTANCE;
    }
}

/* The value of sysUpTime at the time of the last change of
 * the (whole) interface stack.  A change of the interface
 * stack is defined to be any creation, deletion, or change in
 * value of any instance of ifStackStatus.  If the interface
 * stack has been unchanged since the last re-initialization of
 * the local network management subsystem, then this object
 * contains a zero value.
 */

unsigned char *
var_ifStackLastChange(int *var_len, snmp_info_t *mesg,
        int (**write_method)())
{
    /* Add value computations */

    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    return (unsigned char *) NO_MIBINSTANCE;
}

/* A list of objects identifying an address for which the
 * system will accept packets/frames on the particular
 * interface identified by the index value ifIndex.
 */

unsigned char *
var_ifRcvAddressEntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        snmp_info_t *mesg, int (**write_method)())
{
/* Last subOID of COLUMNAR OID is column */
int	column = newoid->name[(newoid->namelen - 1)];
int	result;

    while (0 /* Test whether the entry has been found */) {
        /* Add indexes for the entry to OID newname */
        /* Determine whether it is the requested OID  */
        result = compare(reqoid, newoid);
        if (((searchType == EXACT) && (result != 0)) ||
                ((searchType == NEXT) && (result >= 0))) {
            return NO_MIBINSTANCE;
        }
    }
    /* Place here the computation functionality */

    /* Set write-function */
    *write_method = 0;
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    switch (column) {
	case I_ifRcvAddressAddress:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifRcvAddressStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifRcvAddressType:
	    return (unsigned char *) NO_MIBINSTANCE;
        default:
            return NO_MIBINSTANCE;
    }
}

/* An entry containing objects for invoking tests on an
 * interface.
 */
/*
int	write_ifTestId(int action,
	unsigned char *var_val, unsigned char varval_type, int var_val_len,
	unsigned char *statP, oid *name, int name_len)
{
    switch (action) {
    case RESERVE1:
    case RESERVE2:
    case COMMIT:
    case FREE:
    }
}
*/

unsigned char *
var_ifTestEntry(int *var_len,
        Oid *newoid, Oid *reqoid, int searchType,
        snmp_info_t *mesg, int (**write_method)())
{
/* Last subOID of COLUMNAR OID is column */
int	column = newoid->name[(newoid->namelen - 1)];
int	result;

    while (0 /* Test whether the entry has been found */) {
        /* Add indexes for the entry to OID newname */
        /* Determine whether it is the requested OID  */
        result = compare(reqoid, newoid);
        if (((searchType == EXACT) && (result != 0)) ||
                ((searchType == NEXT) && (result >= 0))) {
            return NO_MIBINSTANCE;
        }
    }
    /* Place here the computation functionality */

    /* Set write-function */
    *write_method = 0;
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    switch (column) {
	case I_ifTestId:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifTestStatus:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifTestType:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifTestResult:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifTestCode:
	    return (unsigned char *) NO_MIBINSTANCE;
	case I_ifTestOwner:
	    return (unsigned char *) NO_MIBINSTANCE;
        default:
            return NO_MIBINSTANCE;
    }
}

static oid ifMIBObjects_oid[] = { O_ifMIBObjects };
static Object ifMIBObjects_variables[] = {
    { SNMP_STRING, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifName }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifInMulticastPkts }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifInBroadcastPkts }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifOutMulticastPkts }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifOutBroadcastPkts }}},
    { SNMP_COUNTER64, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifHCInOctets }}},
    { SNMP_COUNTER64, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifHCInUcastPkts }}},
    { SNMP_COUNTER64, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifHCInMulticastPkts }}},
    { SNMP_COUNTER64, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifHCInBroadcastPkts }}},
    { SNMP_COUNTER64, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifHCOutOctets }}},
    { SNMP_COUNTER64, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifHCOutUcastPkts }}},
    { SNMP_COUNTER64, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifHCOutMulticastPkts }}},
    { SNMP_COUNTER64, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifHCOutBroadcastPkts }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifLinkUpDownTrapEnable }}},
    { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifHighSpeed }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifPromiscuousMode }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifConnectorPresent }}},
    { SNMP_STRING, (RWRITE| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifAlias }}},
    { SNMP_TIMETICKS, (RONLY| COLUMN), (void*)var_ifXEntry,
                {3, { I_ifXTable, I_ifXEntry, I_ifCounterDiscontinuityTime }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_ifStackEntry,
                {3, { I_ifStackTable, I_ifStackEntry, I_ifStackStatus }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_ifTestEntry,
                {3, { I_ifTestTable, I_ifTestEntry, I_ifTestId }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_ifTestEntry,
                {3, { I_ifTestTable, I_ifTestEntry, I_ifTestStatus }}},
    { SNMP_OBJID, (RWRITE| COLUMN), (void*)var_ifTestEntry,
                {3, { I_ifTestTable, I_ifTestEntry, I_ifTestType }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_ifTestEntry,
                {3, { I_ifTestTable, I_ifTestEntry, I_ifTestResult }}},
    { SNMP_OBJID, (RONLY| COLUMN), (void*)var_ifTestEntry,
                {3, { I_ifTestTable, I_ifTestEntry, I_ifTestCode }}},
    { SNMP_STRING, (RWRITE| COLUMN), (void*)var_ifTestEntry,
                {3, { I_ifTestTable, I_ifTestEntry, I_ifTestOwner }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_ifRcvAddressEntry,
                {3, { I_ifRcvAddressTable, I_ifRcvAddressEntry, I_ifRcvAddressStatus }}},
    { SNMP_INTEGER, (NOACCESS| COLUMN), (void*)var_ifRcvAddressEntry,
                {3, { I_ifRcvAddressTable, I_ifRcvAddressEntry, I_ifRcvAddressType }}},
    { SNMP_TIMETICKS, (RONLY| SCALAR), (void*)var_ifTableLastChange,
                 {2, { I_ifTableLastChange, 0 }}},
    { SNMP_TIMETICKS, (RONLY| SCALAR), (void*)var_ifStackLastChange,
                 {2, { I_ifStackLastChange, 0 }}},
    { (int)NULL }
    };
static SubTree ifMIBObjects_tree =  { NULL, ifMIBObjects_variables,
	        (sizeof(ifMIBObjects_oid)/sizeof(oid)), ifMIBObjects_oid};

static oid interfaces_oid[] = { O_interfaces };
static Object interfaces_variables[] = {
    { SNMP_INTEGER, (RONLY| SCALAR), (void*)var_ifNumber,
                 {2, { I_ifNumber, 0 }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifIndex }}},
    { SNMP_STRING, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifDescr }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifType }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifMtu }}},
    { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifSpeed }}},
    { SNMP_STRING, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifPhysAddress }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifAdminStatus }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifOperStatus }}},
    { SNMP_TIMETICKS, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifLastChange }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifInOctets }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifInUcastPkts }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifInNUcastPkts }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifInDiscards }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifInErrors }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifInUnknownProtos }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifOutOctets }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifOutUcastPkts }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifOutNUcastPkts }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifOutDiscards }}},
    { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifOutErrors }}},
    { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifOutQLen }}},
    { SNMP_OBJID, (RONLY| COLUMN), (void*)var_ifEntry,
                {3, { I_ifTable, I_ifEntry, I_ifSpecific }}},
    { (int)NULL }
    };
static SubTree interfaces_tree =  { NULL, interfaces_variables,
	        (sizeof(interfaces_oid)/sizeof(oid)), interfaces_oid};

/* This is the MIB registration function. This should be called */
/* within the init_ifmib-function */
void register_subtrees_of_ifmib()
{
    insert_group_in_mib(&ifMIBObjects_tree);
    insert_group_in_mib(&interfaces_tree);
}

/* ifmib initialisation (must also register the MIB module tree) */
void init_ifmib()
{
    cmsLog_debug("Entry");

    register_subtrees_of_ifmib();
    memset(&ifInfoList,0,sizeof(ifInfoList));
}

int sendLinkStatusTrap(int ifIndex, int adminStatus, int operStatus)
{

    oid oid_sysUpTime[] = { O_sysUpTime, 0 };
    oid oid_trapOid[] = {  O_snmpTrapOID, 0 };
    oid val_trapLinkDownOid[] = { O_linkDown };
    oid val_trapLinkUpOid[] = { O_linkUp };
    oid oid_enterprise[] = {O_snmpTrapEnterprise, 0 }; 
    oid val_enterprise[] = {SYS_OBJ, 2, 10 }; 
    Oid oid_obj;
    struct timeval  now;
    unsigned char *out_data;
    int out_length = SNMP_MAX_MSG_LENGTH;
    raw_snmp_info_t message;
    CmsRet      ret;
    SnmpCfgObject *obj;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    oid oid_ifIndex[] = { O_ifIndex, 0 };
    oid oid_adminStatus[] = { O_ifAdminStatus, 0 };
    oid oid_operStatus[] = { O_ifOperStatus, 0 };

    cmsLog_debug("Sending link Up/Down trap: ifIndex %d, adminStatus %d, operStatus %d\n",
                 ifIndex,adminStatus,operStatus);


    memset( (unsigned char *) &message, 0x00, sizeof(message) );
    
    if ((ret = cmsObj_get(MDMOID_SNMP_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
    {
       cmsLog_error("get of SNMP_CFG object failed, ret=%d", ret);
    }
    else
    {
       message.mesg.version = SNMP_VERSION_2C;
       message.mesg.pdutype = SNMP_TRP2_REQ_PDU;
       strcpy( message.mesg.community, obj->ROCommunity);
       message.mesg.community_length = strlen(obj->ROCommunity);
       message.mesg.community_id = 0;

         cmsObj_free((void **) &obj);

       if (message.mesg.version == SNMP_VERSION_1)
       {
          message.trap_enterprise_oid.namelen =
             sizeof(oid_enterprise) / sizeof(oid);
          memcpy(message.trap_enterprise_oid.name, oid_enterprise,
                 sizeof(oid_enterprise));
          memcpy(message.trap_agent_ip_addr, "0.0.0.0", sizeof("0.0.0.0"));
          if (operStatus == IF_OPER_STATUS_UP)
          {
             message.trap_specific_type = SNMP_TRAP_LINKUP;
          }
          else
          {
             message.trap_specific_type = SNMP_TRAP_LINKDOWN;
          }
          message.trap_timestamp = (long)(gettimeofday(&now, (struct timezone *)0));
          out_data = (unsigned char*)build_snmp_trapv1_without_list_of_varbind(&message);
       }
       else
       {
          message.mesg.request_id = (long)current_sysUpTime();
          out_data = (unsigned char*)build_snmp_response_without_list_of_varbind(&message);
       }

       oid_obj.namelen = sizeof(oid_trapOid) / sizeof(oid);
       memcpy( oid_obj.name, oid_trapOid, sizeof(oid_trapOid) );
       if (operStatus == IF_OPER_STATUS_UP)
       {
          out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_OBJID,
                                         sizeof(val_trapLinkUpOid),
                                         (unsigned char *)val_trapLinkUpOid,
                                         &out_length);
          RETURN_ON_BUILD_ERROR(out_data, "build trap trapOid");
       }
       else 
       {
          out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_OBJID,
                                         sizeof(val_trapLinkDownOid),
                                         (unsigned char *)val_trapLinkDownOid,
                                         &out_length);
          RETURN_ON_BUILD_ERROR(out_data, "build trap trapOid");
       }

       oid_obj.namelen = sizeof(oid_ifIndex) / sizeof(oid);
       memcpy( oid_obj.name, oid_ifIndex, sizeof(oid_ifIndex) );
       long_return = (long)ifIndex;
       out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_INTEGER,
                                      sizeof(long), (unsigned char *)&long_return,
                                      &out_length);
       RETURN_ON_BUILD_ERROR(out_data, "build trap ifIndexOid");

       oid_obj.namelen = sizeof(oid_adminStatus) / sizeof(oid);
       memcpy( oid_obj.name, oid_adminStatus, sizeof(oid_adminStatus) );
       long_return = (long)adminStatus;
       out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_INTEGER,
                                      sizeof(long), (unsigned char *)&long_return,
                                      &out_length);
       RETURN_ON_BUILD_ERROR(out_data, "build trap adminStatusOid");

       oid_obj.namelen = sizeof(oid_operStatus) / sizeof(oid);
       memcpy( oid_obj.name, oid_operStatus, sizeof(oid_operStatus) );
       long_return = (long)operStatus;
       out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_INTEGER,
                                      sizeof(long), (unsigned char *)&long_return,
                                      &out_length);
       RETURN_ON_BUILD_ERROR(out_data, "build trap operStatusOid");


       message.response_packet_end = out_data;
       out_length = correct_snmp_response_with_lengths( &message, 0, 0 );
       out_data = asn_build_sequence(message.response_pdu, &out_length,
                                     message.mesg.pdutype,
                                     message.response_packet_end - message.response_request_id);
       RETURN_ON_BUILD_ERROR(out_data, "build trap pdu type");

       return( snmp_send_trap( &message ) );
    } /* get snmp config ok */
    return 1;
}
