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
#include "cms_qos.h"
#include "dal.h"
#include "cms_boardcmds.h"
#include "cms_qdm.h"

#ifdef DMP_X_ITU_ORG_GPON_1
#include "omcicmn_defs.h"
#endif

#ifdef DMP_X_BROADCOM_COM_EPON_1
#include "oam_util.h"
#endif

/* define in rut_upnp.c */
extern UBOOL8 rut_isUpnpEnabled(void);

/* use functions defined in rut_pmap.c */
extern CmsRet rutPMap_getBridgeByKey(UINT32 bridgeKey, InstanceIdStack *iidStack, L2BridgingEntryObject **bridgeObj);

extern CmsRet rutWan_getAvailableConIdForMSC(const char *wanL2IfName, SINT32 *outConId);
extern CmsRet rutWan_getNumUsedQueues(WanLinkType wanType, UINT32 *usedQueues);
extern CmsRet rutWan_getNumUnusedQueues(WanLinkType wanType, UINT32 *unusedQueues);

extern CmsRet rutDpx_updateDnsproxy(void);


/* forward declarations */

#ifdef DMP_X_BROADCOM_COM_MCAST_1
void getDefaultMcastCfg(WEB_NTWK_VAR *webVar);
#endif

#ifdef DMP_X_BROADCOM_COM_IGMP_1
void getDefaultIgmpCfg(WEB_NTWK_VAR *webVar);
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
void getDefaultMldCfg(WEB_NTWK_VAR *webVar);
#endif

#ifdef SUPPORT_MOCA
static void getMocaInfo(WEB_NTWK_VAR *webVar);
#endif

#ifdef SUPPORT_HOMEPLUG
static void getPlcInfo(WEB_NTWK_VAR *webVar);
#endif

#ifdef SUPPORT_UPNP
static UBOOL8 cmsDal_isUpnpEnabled(void);
#endif

#ifdef SUPPORT_STUN
static void getStunCfg(WEB_NTWK_VAR *webVar);
#endif

/* from dbapi.cpp.
 * These default values are here until we can get all of our default values 
 * from the MDM.
 */
typedef struct {
   char *varName;
   char *varValue;
} DB_DEFAULT_ITEM, *PDB_DEFAULT_ITEM;

static DB_DEFAULT_ITEM DbDefaultTable[] = {
   { "wanIfName", "" },
   { "dnsDomainName", "" },
   { "enblDhcpClnt", "1" },
#ifdef SUPPORT_ADVANCED_DMZ
   { "enableAdvancedDmz", "0"},
   { "nonDmzIpAddress", "192.168.2.1" },
   { "nonDmzIpMask", "255.255.255.0" },
#endif      
   { "ripMode", "0" },
   { "ripVersion", "2" },
   { "ripOperation", "1" },
   { "connMode", "0" },
   { "vlanMuxPr", "-1" },
   { "enVlanMux", "0" },
   { "vlanMuxId", "-1" },
   { "vlanTpid", "0" },
   { "snmpStatus", "0" },
   { "snmpRoCommunity", "public" },
   { "snmpRwCommunity", "private" },
   { "snmpSysName", "Broadcom" },
   { "snmpSysLocation", "unknown" },
   { "snmpSysContact", "unknown" },
   { "snmpTrapIp", "0.0.0.0" },
   { "macPolicy", "0" },
   { "encodePassword", "1" },
   { "enblQos", "0" },
   { "enblUpnp", "1" },
#ifdef DMP_X_BROADCOM_COM_IGMPSNOOP_1
   { "enblIgmpSnp", "1" },
   { "enblIgmpMode", "2" },
   { "enblIgmpLanToLanMcast", "-1"},
#endif
   { "enblSiproxd", "1" },
#ifdef ETH_CFG
   { "ethSpeed", "0" },
   { "ethType", "0" },
   { "ethMtu", "1500" },
#endif
#ifdef BRCM_WLAN
   { "wlSsidIdx", "0" },
   { "wlSsid", "Broadcom" },
   { "wlSsid_2", "Guest" },
   { "wlCountry", "US" },
   { "wlWepKey64", "" },
   { "wlWepKey128", "" },
   { "wlPreambleType", "long" },
   { "wlHide", "0" },
   { "wlEnbl", "1" },
   { "wlEnbl_2", "0" },
   { "wlKeyIndex", "1" },
   { "wlChannel", "11" },
   { "wlFrgThrshld", "2346" },
   { "wlRtsThrshld", "2347" },
   { "wlDtmIntvl", "1" },
   { "wlBcnIntvl", "100" },
   { "wlFltMacMode", "disabled" },
   { "wlAuthMode", "open" },
   { "wlKeyBit", "0" },
   { "wlRate", "0" },
   { "wlPhyType", "g" },
   { "wlBasicRate", "default" },
   { "wlgMode", "1" },
   { "wlProtection", "auto" },
   { "wlWpaPsk", "" },
   { "wlWpaGTKRekey", "0" },
   { "wlRadiusServerIP", "0.0.0.0" },
   { "wlRadiusPort", "1812" },
   { "wlRadiusKey", "" },
   { "wlWep", "disabled" },
   { "wlWpa", "tkip" },
   { "wlAuth", "0" },
   { "wlMode", "ap" },
   { "wlLazyWds", "1" },
   { "wlFrameBurst", "off" },
   { "wlAPIsolation", "0"},
   { "wlBand", "0" },
   { "wlMCastRate", "0"},
   { "wlAfterBurnerEn", "off"},
   { "ezc_enable", "1"},
   { "is_default", "1"},
   { "is_modified", "0"},
   { "wlInfra", "1"},
   { "wlAntDiv", "3"},
   { "wlWme", "0"}, 
   { "wlWmeNoAck", "0"},       
   { "wlPreauth", "0"},  
#ifdef SUPPORT_SES    
   { "wlSesEnable", "1"},
   { "wlSesEvent", "2"},
   { "wlSesStates", "01:01"},
   { "wlSesSsid",""}, 
   { "wlSesWpaPsk",""},             
   { "wlSesClosed","0"},   
   { "wlSesAuth","0"}, 
   { "wlSesAuthMode",""},     
   { "wlSesWep",""}, 
   { "wlSesAuth",""},
   { "wlSesWpa",""},            
   { "wlSesWdsMode","2"},   
   { "wlSesClEnable","1"},
   { "wlSesClEvent","0"},
   { "wlWdsWsec",""},                 
#endif   
   { "wlNetReauth","36000"},
   { "wlTxPwrPcnt", "100"},
   { "wlRegMode","0"},
   { "wlDfsPreIsm","60"},
   { "wlDfsPostIsm","60"}, 
   { "wlTpcDb","0"},     	 
   { "wlCsScanTimer","0"},    	 
   { "wlan_ifname", "wl0"},
#ifdef SUPPORT_MIMO
   { "wlNBw", "20"},
   { "wlNCtrlsb", "0"}, 
   { "wlNBand", "2"},
   { "wlNMcsidx", "-1"},
   { "wlNProtection", "auto" },
   { "wlRifs", "auto" }, 
   { "wlAmpdu", "auto" }, 
   { "wlAmsdu", "auto" },
   { "wlNmode", "auto" },   
#endif   
   { "wlan_ifname", "wl0"},
#endif  /* BRCM_WLAN */

#ifdef SUPPORT_IPSEC
   { "ipsConnEn", "0"},    
   { "ipsConnName", "new connection"},
   { "ipsTunMode", "esp"},
   { "ipsIpver", "4"},
   { "ipsLocalGwIf", "None"},
   { "ipsRemoteGWAddr", "0.0.0.0"},
   { "ipsLocalIPMode", "subnet"},
   { "ipsLocalIP", "0.0.0.0"},
   { "ipsLocalMask", "255.255.255.0"},
   { "ipsRemoteIPMode", "subnet"},
   { "ipsRemoteIP", "0.0.0.0"},
   { "ipsRemoteMask", "255.255.255.0"},
   { "ipsKeyExM", "auto"},
   { "ipsAuthM", "pre_shared_key"},
   { "ipsPSK", "key"},
   { "ipsCertificateName", ""},
   { "ipsPerfectFSEn", "disable"},
   { "ipsManualEncryptionAlgo", "3des-cbc"},
   { "ipsManualEncryptionKey", ""},
   { "ipsManualAuthAlgo", "hmac-md5"},
   { "ipsManualAuthKey", ""},
   { "ipsSPI", "101"},
   { "ipsPh1Mode", "main"},
   { "ipsPh1EncryptionAlgo", "3des"},
   { "ipsPh1IntegrityAlgo", "md5"},
   { "ipsPh1DHGroup", "modp1024"},
   { "ipsPh1KeyTime", "3600"},
   { "ipsPh2EncryptionAlgo", "3des"},
   { "ipsPh2IntegrityAlgo", "hmac_md5"},
   { "ipsPh2DHGroup", "modp1024"},
   { "ipsPh2KeyTime", "3600"},
#endif
   { "enblIpVer", "0" },
#ifdef SUPPORT_IPV6
   { "dns6Type", "DHCP" },
   { "dns6Ifc", "" },
   { "dns6Pri", "" },
   { "dns6Sec", "" },
   { "wanAddr6Type", "DHCP" },
   { "dhcp6cForAddr", "0" },
   { "dhcp6cForPd", "1" },
   { "wanAddr6", "" },
   { "wanGtwy6", "" },
   { "dfltGw6Ifc", "" },
#ifdef DMP_X_BROADCOM_COM_MLDSNOOP_1
   { "enblMldSnp", "1" },
   { "enblMldMode", "2" },
   { "enblMldLanToLanMcast", "-1"},
#endif
#endif
#ifdef DMP_X_BROADCOM_COM_PWRMNGT_1
   { "CPUSpeed", "1" },
   { "CPUr4kWaitEn", "1" },
   { "DRAMSelfRefreshEn", "1" },
   { "EthAutoPwrDwnEn","1" },
   { "EthEEE","1" },
   { "EthIfRegisteredCnt", "0" },
   { "EthIfConnectedCnt", "0" },
   { "USBIfRegisteredCnt", "0" },
   { "USBIfConnectedCnt", "0" },
   { "AvsEn","1" },
#endif /* aka SUPPORT_PWRMNGT */
#ifdef DMP_X_BROADCOM_COM_STANDBY_1
   { "StandbyEnable", "0" },
   { "StandbyStatusString", "Disabled" },
   { "StandbyHour", "0" },
   { "StandbyMinutes", "0" },
   { "WakeupHour", "0" },
   { "WakeupMinutes", "0" },
#endif 
#ifdef SUPPORT_LANVLAN
   { "lanName", "\0" },
#endif
   { NULL, NULL }
};


/** Get the default value for the given variable.
 *
 * This function was formerly called BcmDb_getDefaultValue.
 * It is kept here to minimize changes to the web code for now.  It may be
 * eliminated in the future.  Internally, it should get the default value
 * from the MDM, but it may temporarily still get some default values from
 * the old DbDefaultTable.
 *
 * @return default value in string format.  User must not free it.
 */
static const char *cmsDal_getDefaultValue(const char *var) {
   int  i;
   char *ret;

   ret = DbDefaultTable[0].varValue;

   for (i=0; ret != NULL; i++, ret = DbDefaultTable[i].varValue)
   {
      if ( cmsUtl_strcmp(var, DbDefaultTable[i].varName) == 0 )
         break;
   }

   if ( ret == NULL ) {
      cmsLog_error("Could not find default value for %s", var);
      ret = "";
   }

   return ret;
}


void cmsDal_getDefaultGatewayAndDnsCfg(WEB_NTWK_VAR *webVar)
{
   char staticDnsServersBuf[CMS_MAX_ACTIVE_DNS_IP * CMS_IPADDR_LENGTH]={0};
   UBOOL8 found;

   dalRt_getDefaultGatewayList(webVar->defaultGatewayList);

   /* get list of interfaces for system DNS selection */
   dalDns_getIpvxDnsIfNameList(CMS_AF_SELECT_IPV4, webVar->dnsIfcsList);

   /* get static DNS servers */
   found = qdmDns_getStaticIpvxDnsServersLocked(CMS_AF_SELECT_IPV4, staticDnsServersBuf);
   if (!found || IS_EMPTY_STRING(staticDnsServersBuf))
   {
      strcpy(webVar->dnsPrimary, "0.0.0.0");
      strcpy(webVar->dnsSecondary, "0.0.0.0");
   }
   else
   {
      cmsUtl_parseDNS(staticDnsServersBuf,
                      webVar->dnsPrimary, webVar->dnsSecondary, TRUE);
   }
}


void cmsDal_getAllVersionInfo(WEB_NTWK_VAR *webVar)
{
   getDeviceInfo(webVar);

#ifdef SUPPORT_MOCA
   getMocaInfo(webVar);
#endif
  
#ifdef SUPPORT_HOMEPLUG
   getPlcInfo(webVar);
#endif
}

void cmsDal_getAllInfo(WEB_NTWK_VAR *webVar)
{
   
#ifdef SUPPORT_DSL
   /* get default DSL Configuration */
   getDefaultWanDslLinkCfg(webVar);
#endif

   cmsDal_getCurrentSyslogCfg(webVar);
   cmsDal_getCurrentLoginCfg(webVar);

   cmsDal_getAllVersionInfo(webVar);

   /* get info needed for the LAN page (lancfg2). */
   dalLan_getFirstIntfGroupName(webVar->brName);
   dalLan_getLanPageInfo(webVar);

   cmsLog_notice("ethIpAddress=%s/%s", webVar->ethIpAddress, webVar->ethSubnetMask);

   getDefaultWanConnParams(webVar);

   strcpy(webVar->wanIfName, cmsDal_getDefaultValue("wanIfName"));

   cmsDal_getDefaultGatewayAndDnsCfg(webVar);

   webVar->enblDhcpClnt = atoi(cmsDal_getDefaultValue("enblDhcpClnt"));

   webVar->dhcpcOp60VID[0] = 0;
   webVar->dhcpcOp61DUID[0] = 0;
   webVar->dhcpcOp61IAID[0] = 0;
   webVar->dhcpcOp77UID[0] = 0;
   webVar->dhcpcOp125Enabled = 0;
   webVar->dhcpcOp50IpAddress[0] = 0;
   webVar->dhcpcOp54ServerIpAddress[0] = 0;
   webVar->dhcpcOp51LeasedTime = 0;
   webVar->cfgL2tpAc = 0;
   webVar->cfgPptpAc = 0;
   
#ifdef SUPPORT_ADVANCED_DMZ
   if (dalWan_isAdvancedDmzEnabled() == TRUE)
   {
      webVar->enableAdvancedDmz = TRUE;
      dalNtwk_fillInNonDMZIpAndMask(webVar->nonDmzIpAddress, webVar->nonDmzIpMask);
   }
   else
   {
      webVar->enableAdvancedDmz = atoi(cmsDal_getDefaultValue("enableAdvancedDmz"));
      strcpy(webVar->nonDmzIpAddress, cmsDal_getDefaultValue("nonDmzIpAddress"));
      strcpy(webVar->nonDmzIpMask, cmsDal_getDefaultValue("nonDmzIpMask"));      
   }

#endif /* SUPPORT_ADVANCED_DMZ */

   webVar->enableEthWan = FALSE;


#ifdef SUPPORT_DSL
   // ADSL Configuration
   if (dalWan_getAdslFlags(&(webVar->adslFlag)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get adslFlags, continuing...");
   }
#endif

#ifdef SUPPORT_CELLULAR
   // Cellular Configuration
   dalCellular_getCellularCfg(webVar);
#endif

#ifdef SUPPORT_SNMP
   // SNMP Configuration
   webVar->snmpStatus = atoi(cmsDal_getDefaultValue("snmpStatus"));
   strcpy(webVar->snmpRoCommunity, cmsDal_getDefaultValue("snmpRoCommunity"));
   strcpy(webVar->snmpRwCommunity, cmsDal_getDefaultValue("snmpRwCommunity"));
   strcpy(webVar->snmpSysName, cmsDal_getDefaultValue("snmpSysName"));
   strcpy(webVar->snmpSysLocation, cmsDal_getDefaultValue("snmpSysLocation"));
   strcpy(webVar->snmpSysContact, cmsDal_getDefaultValue("snmpSysContact"));
   strcpy(webVar->snmpTrapIp, cmsDal_getDefaultValue("snmpTrapIp"));
#endif

#ifdef SUPPORT_TR69C
   // TR-069 Client Configuration
   cmsDal_getTr69cCfg(webVar);
#endif

#ifdef DMP_X_ITU_ORG_GPON_1
   // OMCI System Configuration
   cmsDal_getOmciSystem(webVar);
#endif

#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1) 
   // SNTP Configuration
   cmsDal_getNtpCfg(webVar);
#endif

#ifdef DMP_X_BROADCOM_COM_MCAST_1
   getDefaultMcastCfg(webVar);
#endif

#ifdef DMP_X_BROADCOM_COM_IGMP_1
   getDefaultIgmpCfg(webVar);
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
   getDefaultMldCfg(webVar);
#endif

#ifdef DMP_X_BROADCOM_COM_EPON_1
   cmsDal_getOamLoidPwCfg(webVar);
#endif

#ifdef SUPPORT_STUN
   getStunCfg(webVar);
#endif

   // MAC global policy
   webVar->macPolicy = atoi(cmsDal_getDefaultValue("macPolicy"));
#ifdef SUPPORT_UPNP
   // enable/disable UPnP
   webVar->enblUpnp = cmsDal_isUpnpEnabled();
#endif

#ifdef DMP_X_BROADCOM_COM_DBUSREMOTE_1
   cmsDal_getDbusRemoteCfg(webVar);
#endif

#ifdef DMP_X_BROADCOM_COM_IGMPSNOOP_1
   // enable/disable IGMP Snooping
   webVar->enblIgmpSnp = atoi(cmsDal_getDefaultValue("enblIgmpSnp"));
   webVar->enblIgmpMode = atoi(cmsDal_getDefaultValue("enblIgmpMode"));
   webVar->enblIgmpLanToLanMcast = atoi(cmsDal_getDefaultValue("enblIgmpLanToLanMcast"));
#endif

#ifdef ETH_CFG
   // Ethernet Configuration
   webVar->ethSpeed = atoi(cmsDal_getDefaultValue("ethSpeed"));
   webVar->ethType = atoi(cmsDal_getDefaultValue("ethType"));
   webVar->ethMtu = atoi(cmsDal_getDefaultValue("ethMtu"));
#endif

   webVar->connMode = atoi(cmsDal_getDefaultValue("connMode"));
   webVar->vlanMuxPr = atoi(cmsDal_getDefaultValue("vlanMuxPr"));
   webVar->vlanMuxId = atoi(cmsDal_getDefaultValue("vlanMuxId"));
   webVar->vlanTpid = atoi(cmsDal_getDefaultValue("vlanTpid"));

   webVar->enblIpVer = atoi(cmsDal_getDefaultValue("enblIpVer"));
#ifdef SUPPORT_IPV6
   qdmIpv6_getDhcp6sInfo(&webVar->enblDhcp6sStateful, webVar->ipv6IntfIDStart,
                          webVar->ipv6IntfIDEnd, &webVar->dhcp6LeasedTime);
   qdmIpv6_getRadvdUlaInfo(&webVar->enblRadvdUla, &webVar->enblRandomULA, 
                           webVar->ipv6UlaPrefix, &webVar->ipv6UlaPlt, &webVar->ipv6UlaVlt);
   /* IPv6 only handles a single LAN interface, so hardcode to br0 for now */
   qdmIpIntf_getIpv6AddressByNameLocked("br0", webVar->lanIntfAddr6);
   strcpy(webVar->dns6Type, cmsDal_getDefaultValue("dns6Type"));
   strcpy(webVar->dns6Ifc, cmsDal_getDefaultValue("dns6Ifc"));
   strcpy(webVar->dns6Pri, cmsDal_getDefaultValue("dns6Pri"));
   strcpy(webVar->dns6Sec, cmsDal_getDefaultValue("dns6Sec"));
   strcpy(webVar->wanAddr6Type, cmsDal_getDefaultValue("wanAddr6Type"));
   webVar->dhcp6cForAddr =atoi(cmsDal_getDefaultValue("dhcp6cForAddr"));
   webVar->dhcp6cForPd =atoi(cmsDal_getDefaultValue("dhcp6cForPd"));
   strcpy(webVar->wanAddr6, cmsDal_getDefaultValue("wanAddr6"));
   strcpy(webVar->wanGtwy6, cmsDal_getDefaultValue("wanGtwy6"));
   strcpy(webVar->dfltGw6Ifc, cmsDal_getDefaultValue("dfltGw6Ifc"));
#ifdef DMP_X_BROADCOM_COM_MLDSNOOP_1
   webVar->enblMldSnp = atoi(cmsDal_getDefaultValue("enblMldSnp"));
   webVar->enblMldMode = atoi(cmsDal_getDefaultValue("enblMldMode"));
   webVar->enblMldLanToLanMcast = atoi(cmsDal_getDefaultValue("enblMldLanToLanMcast"));
#endif
#endif

#ifdef SUPPORT_IPSEC
   // IPSec Configuration
    strcpy(webVar->ipsConnName, cmsDal_getDefaultValue("ipsConnName"));
    strcpy(webVar->ipsTunMode, cmsDal_getDefaultValue("ipsTunMode"));
    strcpy(webVar->ipsIpver, cmsDal_getDefaultValue("ipsIpver"));
    strcpy(webVar->ipsLocalGwIf, cmsDal_getDefaultValue("ipsLocalGwIf"));
    strcpy(webVar->ipsRemoteGWAddr, cmsDal_getDefaultValue("ipsRemoteGWAddr"));
    strcpy(webVar->ipsLocalIPMode, cmsDal_getDefaultValue("ipsLocalIPMode"));
    strcpy(webVar->ipsLocalIP, cmsDal_getDefaultValue("ipsLocalIP"));
    strcpy(webVar->ipsLocalMask, cmsDal_getDefaultValue("ipsLocalMask"));
    strcpy(webVar->ipsRemoteIPMode, cmsDal_getDefaultValue("ipsRemoteIPMode"));
    strcpy(webVar->ipsRemoteIP, cmsDal_getDefaultValue("ipsRemoteIP"));
    strcpy(webVar->ipsRemoteMask, cmsDal_getDefaultValue("ipsRemoteMask"));
    strcpy(webVar->ipsKeyExM, cmsDal_getDefaultValue("ipsKeyExM"));
    strcpy(webVar->ipsAuthM, cmsDal_getDefaultValue("ipsAuthM"));
    strcpy(webVar->ipsPSK, cmsDal_getDefaultValue("ipsPSK"));
    strcpy(webVar->ipsCertificateName, cmsDal_getDefaultValue("ipsCertificateName"));
    strcpy(webVar->ipsPerfectFSEn, cmsDal_getDefaultValue("ipsPerfectFSEn"));
    strcpy(webVar->ipsManualEncryptionAlgo, cmsDal_getDefaultValue("ipsManualEncryptionAlgo"));
    strcpy(webVar->ipsManualEncryptionKey, cmsDal_getDefaultValue("ipsManualEncryptionKey"));
    strcpy(webVar->ipsManualAuthAlgo, cmsDal_getDefaultValue("ipsManualAuthAlgo"));
    strcpy(webVar->ipsManualAuthKey, cmsDal_getDefaultValue("ipsManualAuthKey"));
    strcpy(webVar->ipsSPI, cmsDal_getDefaultValue("ipsSPI"));
    strcpy(webVar->ipsPh1Mode, cmsDal_getDefaultValue("ipsPh1Mode"));
    strcpy(webVar->ipsPh1EncryptionAlgo, cmsDal_getDefaultValue("ipsPh1EncryptionAlgo"));
    strcpy(webVar->ipsPh1IntegrityAlgo, cmsDal_getDefaultValue("ipsPh1IntegrityAlgo"));
    strcpy(webVar->ipsPh1DHGroup, cmsDal_getDefaultValue("ipsPh1DHGroup"));
    webVar->ipsPh1KeyTime = atoi(cmsDal_getDefaultValue("ipsPh1KeyTime"));
    strcpy(webVar->ipsPh2EncryptionAlgo, cmsDal_getDefaultValue("ipsPh2EncryptionAlgo"));
    strcpy(webVar->ipsPh2IntegrityAlgo, cmsDal_getDefaultValue("ipsPh2IntegrityAlgo"));
    strcpy(webVar->ipsPh2DHGroup, cmsDal_getDefaultValue("ipsPh2DHGroup"));
    webVar->ipsPh2KeyTime = atoi(cmsDal_getDefaultValue("ipsPh2KeyTime"));

#endif

#ifdef DMP_X_BROADCOM_COM_PWRMNGT_1
   webVar->pmCPUSpeed= atoi(cmsDal_getDefaultValue("CPUSpeed"));
   webVar->pmCPUr4kWaitEn = atoi(cmsDal_getDefaultValue("CPUr4kWaitEn"));
   webVar->pmDRAMSelfRefreshEn = atoi(cmsDal_getDefaultValue("DRAMSelfRefreshEn"));
   webVar->pmEthAutoPwrDwnEn   = atoi(cmsDal_getDefaultValue("EthAutoPwrDwnEn"));
   webVar->pmEthEEE   = atoi(cmsDal_getDefaultValue("EthEEE"));
   webVar->pmAvsEn               = atoi(cmsDal_getDefaultValue("AvsEn"));
#endif /* aka SUPPORT_PWRMNGT */

#ifdef DMP_X_BROADCOM_COM_STANDBY_1
   webVar->pmStandbyEnable = atoi(cmsDal_getDefaultValue("StandbyEnable"));
   strcpy(webVar->pmStandbyStatusString, cmsDal_getDefaultValue("StandbyStatusString"));
   webVar->pmStandbyHour = atoi(cmsDal_getDefaultValue("StandbyHour"));
   webVar->pmStandbyMinutes = atoi(cmsDal_getDefaultValue("StandbyMinutes"));
   webVar->pmWakeupHour = atoi(cmsDal_getDefaultValue("WakeupHour"));
   webVar->pmWakeupMinutes = atoi(cmsDal_getDefaultValue("WakeupMinutes"));
#endif 

#ifdef SUPPORT_LANVLAN
   strcpy(webVar->lanName, cmsDal_getDefaultValue("lanName"));
#endif
   return;
}


CmsRet cmsDal_getCurrentSyslogCfg(WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   SyslogCfgObject *syslogCfg=NULL;
   CmsRet ret;

   cmsLog_debug("Entered");

   if ((ret = cmsObj_get(MDMOID_SYSLOG_CFG, &iidStack, 0, (void *) &syslogCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get SYSLOG_CFG, ret=%d", ret);
      return ret;
   }

   /* if status is the DISABLED string, then strcmp returns 0, which means status=0/off */
   webVar->logStatus = cmsUtl_strcmp(syslogCfg->status, MDMVS_DISABLED);

   webVar->logMode = cmsUtl_syslogModeToNum(syslogCfg->option);
   webVar->logDisplay = cmsUtl_syslogLevelToNum(syslogCfg->localDisplayLevel);
   /* glbwebvar only has logLevel, not local and remote log level.  So if
    * our syslogd only supports unified local/remote log level, and this data
    * model is broadcom proprietary, why not just unify them in the data model? */
   webVar->logLevel = cmsUtl_syslogLevelToNum(syslogCfg->localLogLevel);

   // don't see where remoteloglevel is used
   //   webVar->logLevel = cmsUtl_syslogLevelToNum(syslogCfg->remoteLogLevel);

   strcpy(webVar->logIpAddress, syslogCfg->serverIPAddress);
   webVar->logPort = syslogCfg->serverPortNumber;

   cmsObj_free((void **) &syslogCfg);

   return ret;
}


CmsRet cmsDal_getCurrentLoginCfg(WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LoginCfgObject *loginCfg=NULL;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_LOGIN_CFG, &iidStack, 0, (void *) &loginCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get LOGIN_CFG, ret=%d", ret);
      return ret;
   }

   strcpy(webVar->adminUserName, loginCfg->adminUserName);
   strcpy(webVar->sptUserName, loginCfg->supportUserName);
   strcpy(webVar->usrUserName, loginCfg->userUserName);
#ifdef SUPPORT_HASHED_PASSWORDS
   strcpy(webVar->adminPassword, loginCfg->adminPasswordHash);
   strcpy(webVar->sptPassword, loginCfg->supportPasswordHash);
   strcpy(webVar->usrPassword, loginCfg->userPasswordHash);
#else
   strcpy(webVar->adminPassword, loginCfg->adminPassword);
   strcpy(webVar->sptPassword, loginCfg->supportPassword);
   strcpy(webVar->usrPassword, loginCfg->userPassword);
#endif

   cmsObj_free((void **) &loginCfg);

   return ret;
}


#if defined(DMP_BASELINE_1)
void getDeviceInfo_igd(WEB_NTWK_VAR *webVar)
{
   char *strPtr;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IGDDeviceInfoObject *deviceInfo=NULL;
   CmsRet ret;

   memset(webVar->boardID, 0, sizeof(webVar->boardID));
   memset(webVar->swVers, 0, sizeof(webVar->swVers));
   memset(webVar->cfeVers, 0, sizeof(webVar->cfeVers));
   memset(webVar->cmsBuildTimestamp, 0, sizeof(webVar->cmsBuildTimestamp));

   if ((ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void *) &deviceInfo)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get DEVICE_INFO, ret=%d", ret);
      return;
   }

   if ((strPtr = strstr(deviceInfo->additionalHardwareVersion, "BoardId=")) != NULL)
   {
      strncpy(webVar->boardID, &(strPtr[8]), sizeof(webVar->boardID)-1);
   }

   strncpy(webVar->swVers, deviceInfo->softwareVersion, sizeof(webVar->swVers)-1);

   if ((strPtr = strstr(deviceInfo->additionalSoftwareVersion, "CFE=")) != NULL)
   {
      strncpy(webVar->cfeVers, &(strPtr[4]), sizeof(webVar->cfeVers)-1);
   }

   if (deviceInfo->X_BROADCOM_COM_SwBuildTimestamp != NULL)
   {
      strncpy(webVar->cmsBuildTimestamp, deviceInfo->X_BROADCOM_COM_SwBuildTimestamp, sizeof(webVar->cmsBuildTimestamp)-1);
   }

   webVar->numCpuThreads = deviceInfo->X_BROADCOM_COM_NumberOfCpuThreads;

#ifdef DMP_X_BROADCOM_COM_ADSLWAN_1
   memset(webVar->dslPhyDrvVersion, 0, sizeof(webVar->dslPhyDrvVersion));
   if (deviceInfo->X_BROADCOM_COM_DslPhyDrvVersion != NULL)
   {
      strncpy(webVar->dslPhyDrvVersion, deviceInfo->X_BROADCOM_COM_DslPhyDrvVersion, sizeof(webVar->dslPhyDrvVersion)-1);
   }
#endif


#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   memset(webVar->voiceServiceVersion, 0, sizeof(webVar->voiceServiceVersion));
   if (deviceInfo->X_BROADCOM_COM_VoiceServiceVersion != NULL)
   {
      strncpy(webVar->voiceServiceVersion, deviceInfo->X_BROADCOM_COM_VoiceServiceVersion, sizeof(webVar->voiceServiceVersion)-1);
   }
#endif

   cmsObj_free((void **) &deviceInfo);
}
#endif  /* DMP_BASELINE_1 */

#ifdef SUPPORT_HOMEPLUG
static void getPlcInfo(WEB_NTWK_VAR *webVar)
{
   /* update global web var with HomePlug info */
   CmsRet ret = dalHomeplug_getCurrentCfg(webVar);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get HOMEPLUG DAL object");
   }
}
#endif // SUPPORT_HOMEPLUG

#ifdef SUPPORT_MOCA

/** Get Moca version string.
 *
 * This function is complicated by the fact the moca could be on just the LAN side,
 * or it may be on the WAN side, or there may be one on each side.  We just need to
 * find one.  I assume that the version numbers will be the same.
 */
void getMocaInfo(WEB_NTWK_VAR *webVar)
{
   char ifNameListBuf[CMS_IFNAME_LENGTH*2]={0};
   char *pToken;

   cmsLog_debug("Entered");

   memset(webVar->mocaVersion, 0, sizeof(webVar->mocaVersion));
   memset(webVar->mocaIfName, 0, sizeof(webVar->mocaIfName));

   /*
    * Set the initial moca intf name.  When we go to the Moca page for the
    * first time, the page will be filled in with the info from the
    * initial moca intf name (should match the first intf name in the
    * drop down menu.)
    */
   dalMoca_getIntfNameList(ifNameListBuf, sizeof(ifNameListBuf));
   cmsLog_debug("got intfNameList=%s", ifNameListBuf);

   if (!IS_EMPTY_STRING(ifNameListBuf))
   {
      pToken = strtok(ifNameListBuf, ",");
      sprintf(webVar->mocaIfName, "%s", pToken);

      /* set version string */
      dalMoca_setVersionString(webVar->mocaIfName, webVar->mocaVersion, sizeof(webVar->mocaVersion));
   }
   else
   {
      cmsLog_error("could not find any Moca device, set version to not found");
      sprintf(webVar->mocaVersion, "Not Found");
   }

   return;
}

#endif /* SUPPORT_MOCA */


#ifdef DMP_ADSLWAN_1

void getDefaultAtmLinkCfg_igd(WEB_NTWK_VAR *webVar)
{
   WanDslLinkCfgObject *dslLinkCfg=NULL;
   CmsRet ret;

   /*
    * Note we are getting the default values for this object.
    * This call should always work, and does not need an
    * iidStack pointer.
    */
   if ((ret = cmsObj_get(MDMOID_WAN_DSL_LINK_CFG,
                         NULL,
                         OGF_DEFAULT_VALUES,
                         (void *) &dslLinkCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get default DSL_LINK_CFG, ret=%d", ret);
      return;
   }

   webVar->atmPeakCellRate = dslLinkCfg->ATMPeakCellRate;
   webVar->atmSustainedCellRate = dslLinkCfg->ATMSustainableCellRate;
   webVar->atmMaxBurstSize = dslLinkCfg->ATMMaximumBurstSize;
   webVar->atmMinCellRate = dslLinkCfg->X_BROADCOM_COM_ATMMinimumCellRate;
   STR_COPY_OR_NULL_TERMINATE(webVar->atmServiceCategory, dslLinkCfg->ATMQoS);
   STR_COPY_OR_NULL_TERMINATE(webVar->linkType, dslLinkCfg->linkType);
   cmsUtl_atmVpiVciStrToNum(dslLinkCfg->destinationAddress,
                            &(webVar->atmVpi),
                            &(webVar->atmVci));

   STR_COPY_OR_NULL_TERMINATE(webVar->grpScheduler,
                              dslLinkCfg->X_BROADCOM_COM_GrpScheduler);
   webVar->grpWeight     = dslLinkCfg->X_BROADCOM_COM_GrpWeight;
   webVar->grpPrecedence = dslLinkCfg->X_BROADCOM_COM_GrpPrecedence;

   STR_COPY_OR_NULL_TERMINATE(webVar->schedulerAlgorithm,
                              dslLinkCfg->X_BROADCOM_COM_SchedulerAlgorithm);
   webVar->queueWeight     = dslLinkCfg->X_BROADCOM_COM_QueueWeight;
   webVar->queuePrecedence = dslLinkCfg->X_BROADCOM_COM_QueuePrecedence;

   cmsObj_free((void **) &dslLinkCfg);
}

#endif /* DMP_ADSLWAN_1 */   


#ifdef DMP_PTMWAN_1
   
void getDefaultPtmLinkCfg_igd(WEB_NTWK_VAR *webVar)
{
   WanPtmLinkCfgObject *ptmLinkCfg=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * Note we are getting the default values for this object.
    * This call should always work, and does not need an
    * iidStack pointer.
    */
   if ((ret = cmsObj_get(MDMOID_WAN_PTM_LINK_CFG,
                         NULL,
                         OGF_DEFAULT_VALUES,
                         (void *) &ptmLinkCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get default PTM_LINK_CFG, ret=%d", ret);
   return;
   }

   webVar->portId     = ptmLinkCfg->X_BROADCOM_COM_PTMPortId;
   webVar->ptmPriorityNorm = ptmLinkCfg->X_BROADCOM_COM_PTMPriorityLow;
   webVar->ptmPriorityHigh = ptmLinkCfg->X_BROADCOM_COM_PTMPriorityHigh;

   webVar->enblQos = ptmLinkCfg->X_BROADCOM_COM_PTMEnbQos;

   STR_COPY_OR_NULL_TERMINATE(webVar->grpScheduler,
                              ptmLinkCfg->X_BROADCOM_COM_GrpScheduler);
   webVar->grpWeight     = ptmLinkCfg->X_BROADCOM_COM_GrpWeight;
   webVar->grpPrecedence = ptmLinkCfg->X_BROADCOM_COM_GrpPrecedence;

   STR_COPY_OR_NULL_TERMINATE(webVar->schedulerAlgorithm,
                              ptmLinkCfg->X_BROADCOM_COM_SchedulerAlgorithm);
   webVar->queueWeight     = ptmLinkCfg->X_BROADCOM_COM_QueueWeight;
   webVar->queuePrecedence = ptmLinkCfg->X_BROADCOM_COM_QueuePrecedence;

   cmsObj_free((void **) &ptmLinkCfg);
}

#endif /* DMP_PTMWAN_1 */   


void getDefaultWanDslLinkCfg_igd(WEB_NTWK_VAR *webVar __attribute__((unused)))
{
#ifdef DMP_ADSLWAN_1
   getDefaultAtmLinkCfg_igd(webVar);
#endif /* DMP_ADSLWAN_1 */   

#ifdef DMP_PTMWAN_1
   getDefaultPtmLinkCfg_igd(webVar);
#endif /* DMP_PTMWAN_1 */   
}


void getDefaultWanConnParams_igd(WEB_NTWK_VAR *webVar)
{
   WanPppConnObject *pppConn=NULL;
   WanIpConnObject *ipConn=NULL;
   CmsRet ret;

   cmsLog_debug("loading defaults from pppConn object");

   /*
    * Note we are getting the default values for this object.
    * This call should always work, and does not need an
    * iidStack pointer.
    */
   if ((ret = cmsObj_get(MDMOID_WAN_IP_CONN,
                         NULL,
                         OGF_DEFAULT_VALUES,
                         (void *) &ipConn)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get default WAN_IP_CONN, ret=%d", ret);
      return;
   }


   if (ipConn->externalIPAddress != NULL)
   {
      strncpy(webVar->wanIpAddress, ipConn->externalIPAddress, sizeof(webVar->wanIpAddress));
   }
   else
   {
      strncpy(webVar->wanIpAddress, "0.0.0.0", sizeof(webVar->wanIpAddress));
   }
   if (ipConn->subnetMask != NULL)
   {
      strncpy(webVar->wanSubnetMask, ipConn->subnetMask, sizeof(webVar->wanSubnetMask));
   }
   else
   {
      strncpy(webVar->wanSubnetMask, "0.0.0.0", sizeof(webVar->wanSubnetMask));
   }
   if (ipConn->defaultGateway != NULL)
   {
      strncpy(webVar->wanIntfGateway, ipConn->defaultGateway, sizeof(webVar->wanIntfGateway));
   }
   else
   {
      strncpy(webVar->wanIntfGateway, "0.0.0.0", sizeof(webVar->wanIntfGateway));
   }

   cmsObj_free((void **) &ipConn);

   cmsLog_debug("loading defaults from pppConn object");

   if ((ret = cmsObj_get(MDMOID_WAN_PPP_CONN,
                         NULL,
                         OGF_DEFAULT_VALUES,
                         (void *) &pppConn)) != CMSRET_SUCCESS)

   if (pppConn->username != NULL)
   {
      strcpy(webVar->pppUserName, pppConn->username);
   }
   if (pppConn->password != NULL)
   {
      strcpy(webVar->pppPassword, pppConn->password);
   }
   if (pppConn->name != NULL)
   {
      strcpy(webVar->serviceName, pppConn->name);
   }
   if (pppConn->PPPoEServiceName != NULL)
   {
      strcpy(webVar->pppServerName, pppConn->PPPoEServiceName);
   }
   webVar->serviceId = pppConn->X_BROADCOM_COM_ConnectionId;
#ifdef DMP_X_BROADCOM_COM_IGMP_1
   webVar->enblIgmp = pppConn->X_BROADCOM_COM_IGMPEnabled;
   webVar->enblIgmpMcastSource = pppConn->X_BROADCOM_COM_IGMP_SOURCEEnabled;
#endif
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   webVar->noMcastVlanFilter  = pppConn->X_BROADCOM_COM_NoMcastVlanFilter;
#endif
   webVar->enblService = 1;  /* always enable service even though data model default is false */
   webVar->ntwkPrtcl = 3;  /* magic number, does it mean IPoE? */
   webVar->encapMode = 0;

   webVar->enblAutoScan = 0; /* don't support autoscan anymore, delete? */

   webVar->enblNat = pppConn->NATEnabled;
   webVar->enblFirewall = pppConn->X_BROADCOM_COM_FirewallEnabled;
   webVar->enblOnDemand = 0; /* which means onDemand */
   webVar->pppTimeOut = 0; /* not in data model */
   webVar->pppIpExtension = pppConn->X_BROADCOM_COM_IPExtension;
   webVar->pppAuthMethod = 0; /* which means AUTO_AUTH */
   webVar->pppToBridge = pppConn->X_BROADCOM_COM_AddPppToBridge;
   webVar->useStaticIpAddress = pppConn->X_BROADCOM_COM_UseStaticIPAddress;
   if (pppConn->X_BROADCOM_COM_LocalIPAddress != NULL)
   {
      strcpy(webVar->pppLocalIpAddress, pppConn->X_BROADCOM_COM_LocalIPAddress);
   }
   else
   {
      strcpy(webVar->pppLocalIpAddress, "0.0.0.0");
   }
   webVar->enblPppDebug = pppConn->X_BROADCOM_COM_Enable_Debug;
#ifdef DMP_X_BROADCOM_COM_MLD_1
   webVar->enblMld = pppConn->X_BROADCOM_COM_MLDEnabled;
   webVar->enblMldMcastSource = pppConn->X_BROADCOM_COM_MLD_SOURCEEnabled;
#endif   /* DMP_X_BROADCOM_COM_MLD_1 */
   webVar->pcpMode = 0;
   strcpy(webVar->pcpServer, "");

   cmsObj_free((void **) &pppConn);

   return;
}

#ifdef DMP_X_BROADCOM_COM_MCAST_1
void getDefaultMcastCfg(WEB_NTWK_VAR *webVar)
{
   McastCfgObject *mcastCfgObj = NULL;
   CmsRet ret;

   /*
    * Note we are getting the default values for this object.
    * This call should always work, and does not need an
    * iidStack pointer.
    */
   if ((ret = cmsObj_get(MDMOID_MCAST_CFG,
                         NULL,
                         OGF_DEFAULT_VALUES,
                         (void *) &mcastCfgObj)) != CMSRET_SUCCESS)
   {
      printf("Could not get default MDMOID_MCAST_CFG, ret=%d", ret);
      return;
   }

   webVar->mcastPrecedence = mcastCfgObj->mcastPrecedence;
   webVar->mcastStrictWan = mcastCfgObj->mcastStrictWan;
#ifdef DMP_X_BROADCOM_COM_DCSP_MCAST_REMARK_1
   webVar->mcastDscpRemarkEnable = mcastCfgObj->mcastDscpRemarkEnable;
   webVar->mcastDscpRemarkVal = mcastCfgObj->mcastDscpRemarkVal;
#endif

   cmsObj_free((void **) &mcastCfgObj);

   return;
} /* getDefaultMcastCfg */
#endif

#ifdef DMP_X_BROADCOM_COM_IGMP_1
void getDefaultIgmpCfg(WEB_NTWK_VAR *webVar)
{
   IgmpCfgObject *igmpCfgObj = NULL;
   CmsRet ret;

   /*
    * Note we are getting the default values for this object.
    * This call should always work, and does not need an
    * iidStack pointer.
    */
   if ((ret = cmsObj_get(MDMOID_IGMP_CFG,
                         NULL,
                         OGF_DEFAULT_VALUES,
                         (void *) &igmpCfgObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get default MDMOID_IGMP_CFG, ret=%d", ret);
      return;
   }

   webVar->igmpVer = igmpCfgObj->igmpVer;
   webVar->igmpQI = igmpCfgObj->igmpQI;
   webVar->igmpQRI = igmpCfgObj->igmpQRI;
   webVar->igmpLMQI = igmpCfgObj->igmpLMQI;
   webVar->igmpRV = igmpCfgObj->igmpRV;
   webVar->igmpMaxGroups = igmpCfgObj->igmpMaxGroups;
   webVar->igmpMaxSources = igmpCfgObj->igmpMaxSources;
   webVar->igmpMaxMembers = igmpCfgObj->igmpMaxMembers;
   webVar->igmpFastLeaveEnable = igmpCfgObj->igmpFastLeaveEnable;

   cmsObj_free((void **) &igmpCfgObj);

   return;
} /* getDefaultIgmpCfg */
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
void getDefaultMldCfg(WEB_NTWK_VAR *webVar)
{
   MldCfgObject *mldCfgObj = NULL;
   CmsRet ret;

   /*
    * Note we are getting the default values for this object.
    * This call should always work, and does not need an
    * iidStack pointer.
    */
   if ((ret = cmsObj_get(MDMOID_MLD_CFG,
                         NULL,
                         OGF_DEFAULT_VALUES,
                         (void *) &mldCfgObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get default MLD_CFG, ret=%d", ret);
      return;
   }

   webVar->mldVer = mldCfgObj->mldVer;
   webVar->mldQI = mldCfgObj->mldQI;
   webVar->mldQRI = mldCfgObj->mldQRI;
   webVar->mldLMQI = mldCfgObj->mldLMQI;
   webVar->mldRV = mldCfgObj->mldRV;
   webVar->mldMaxGroups = mldCfgObj->mldMaxGroups;
   webVar->mldMaxSources = mldCfgObj->mldMaxSources;
   webVar->mldMaxMembers = mldCfgObj->mldMaxMembers;
   webVar->mldFastLeaveEnable = mldCfgObj->mldFastLeaveEnable;

   cmsObj_free((void **) &mldCfgObj);

   return;
} /* getDefaultMldCfg */
#endif

CmsRet cmsDal_setSyslogCfg(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   SyslogCfgObject *syslogCfg=NULL;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_SYSLOG_CFG, &iidStack, 0, (void *) &syslogCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get SYSLOG_CFG, ret=%d", ret);
      return ret;
   }

   /* take values from webVar and stick them into the syslogCfg object */
   cmsMem_free(syslogCfg->status);
   if (webVar->logStatus)
   {
      syslogCfg->status = cmsMem_strdup(MDMVS_ENABLED);
   }
   else
   {
      syslogCfg->status = cmsMem_strdup(MDMVS_DISABLED);
   }

   cmsMem_free(syslogCfg->option);
   syslogCfg->option = cmsMem_strdup(cmsUtl_numToSyslogModeString(webVar->logMode));

   cmsMem_free(syslogCfg->localDisplayLevel);
   syslogCfg->localDisplayLevel = cmsMem_strdup(cmsUtl_numToSyslogLevelString(webVar->logDisplay));

   /* glbwebvar only has logLevel, not local and remote log level */
   cmsMem_free(syslogCfg->localLogLevel);
   syslogCfg->localLogLevel = cmsMem_strdup(cmsUtl_numToSyslogLevelString(webVar->logLevel));

   cmsMem_free(syslogCfg->remoteLogLevel);
   syslogCfg->remoteLogLevel = cmsMem_strdup(cmsUtl_numToSyslogLevelString(webVar->logLevel));


   cmsMem_free(syslogCfg->serverIPAddress);
   syslogCfg->serverIPAddress = cmsMem_strdup(webVar->logIpAddress);

   syslogCfg->serverPortNumber = webVar->logPort;

   ret = cmsObj_set(syslogCfg, &iidStack);
   cmsObj_free((void **) &syslogCfg);

   return ret;
}



CmsRet cmsDal_setLoginCfg(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LoginCfgObject *loginCfg=NULL;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_LOGIN_CFG, &iidStack, 0, (void *) &loginCfg)) != CMSRET_SUCCESS)
   {
      return ret;
   }

#ifdef SUPPORT_HASHED_PASSWORDS
   cmsMem_free(loginCfg->adminPasswordHash);
   loginCfg->adminPasswordHash = cmsMem_strdup(webVar->adminPassword);

   cmsMem_free(loginCfg->supportPasswordHash);
   loginCfg->supportPasswordHash = cmsMem_strdup(webVar->sptPassword);

   cmsMem_free(loginCfg->userPasswordHash);
   loginCfg->userPasswordHash = cmsMem_strdup(webVar->usrPassword);
#else
   cmsMem_free(loginCfg->adminPassword);
   loginCfg->adminPassword = cmsMem_strdup(webVar->adminPassword);

   cmsMem_free(loginCfg->supportPassword);
   loginCfg->supportPassword = cmsMem_strdup(webVar->sptPassword);

   cmsMem_free(loginCfg->userPassword);
   loginCfg->userPassword = cmsMem_strdup(webVar->usrPassword);
#endif

   ret = cmsObj_set(loginCfg, &iidStack);
   cmsObj_free((void **) &loginCfg);

   return ret;
}


CmsRet cmsDal_getEnblNatForWeb_igd(char *varValue)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack2 = EMPTY_INSTANCE_ID_STACK;   
   WanPppConnObject *wan_ppp_con = NULL;
   WanIpConnObject *wan_ip_con = NULL; 

   /* getting NAT information */
   cmsLog_debug("Getting NAT info");

   while ( cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack1, (void **) &wan_ip_con) == CMSRET_SUCCESS)
   {
      if ( wan_ip_con->NATEnabled == TRUE) 
      {
         strcpy(varValue, "1");
         cmsObj_free((void **) &wan_ip_con);
         return ret;
      }
      cmsObj_free((void **) &wan_ip_con);
   }

   while ((ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack2, (void **) &wan_ppp_con)) == CMSRET_SUCCESS)
   {
      if ( wan_ppp_con->NATEnabled == TRUE ) 
      {
         strcpy(varValue, "1");
         cmsObj_free((void **) &wan_ppp_con);
         return ret;
      }
      cmsObj_free((void **) &wan_ppp_con);	  	  
   }

   strcpy(varValue, "0");

   if (ret == CMSRET_NO_MORE_INSTANCES)
	   ret = CMSRET_SUCCESS;

   return ret;

}

CmsRet cmsDal_getEnblFullconeForWeb(char *varValue)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack2 = EMPTY_INSTANCE_ID_STACK;   
   WanPppConnObject *wan_ppp_con = NULL;
   WanIpConnObject *wan_ip_con = NULL; 

   /* getting NAT information */
   cmsLog_debug("Getting Fullcone NAT info");

   while ( cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack1, (void **) &wan_ip_con) == CMSRET_SUCCESS)
   {
      if ( wan_ip_con->X_BROADCOM_COM_FullconeNATEnabled == TRUE) 
      {
         strcpy(varValue, "1");
         cmsObj_free((void **) &wan_ip_con);
         return ret;
      }
      cmsObj_free((void **) &wan_ip_con);
   }

   while ((ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack2, (void **) &wan_ppp_con)) == CMSRET_SUCCESS)
   {
      if ( wan_ppp_con->X_BROADCOM_COM_FullconeNATEnabled == TRUE ) 
      {
         strcpy(varValue, "1");
         cmsObj_free((void **) &wan_ppp_con);
         return ret;
      }
      cmsObj_free((void **) &wan_ppp_con);	  	  
   }

   strcpy(varValue, "0");

   if (ret == CMSRET_NO_MORE_INSTANCES)
	   ret = CMSRET_SUCCESS;

   return ret;
}

SINT32 cmsDal_getWANProtocolFromIpConn(_WanDslLinkCfgObject *dslLinkCfg, _WanIpConnObject *ipConn)
{
   SINT32 protocol = CMS_WAN_TYPE_UNDEFINED;
   if (!cmsUtl_strcmp(ipConn->connectionType, MDMVS_IP_BRIDGED))
   {
      protocol = CMS_WAN_TYPE_BRIDGE;
   }
   else if (!cmsUtl_strcmp(dslLinkCfg->linkType, MDMVS_IPOA))
   {
      protocol = CMS_WAN_TYPE_IPOA;
   }                  
   else                  
   {
      protocol = CMS_WAN_TYPE_DYNAMIC_IPOE;
   }  

   return protocol;
}


SINT32 cmsDal_getWANProtocolFromPppConn(_WanDslLinkCfgObject *dslLinkCfg)
{
   SINT32 protocol = CMS_WAN_TYPE_UNDEFINED;
   
   if (!cmsUtl_strcmp(dslLinkCfg->linkType, MDMVS_PPPOA))
   {
      protocol = CMS_WAN_TYPE_PPPOA;
   }
   else
   {
      protocol = CMS_WAN_TYPE_PPPOE;
   }  

   return protocol;
   
}


CmsRet cmsDal_getAvailableConIdForMSC(const char *wanL2IfName, SINT32 *outConId)
{
   return (rutWan_getAvailableConIdForMSC(wanL2IfName, outConId));
}

/*
 * This function replaces BcmDb_getWanProtocolName(): converts data model's protocol name convention
 * to HTTP protocol name.   Shorter names are used when composing parameters for http requests.
 */
void cmsDal_getWanProtocolName(UINT8 protocol, char *name)
{
    if ( name == NULL ) return;

    name[0] = '\0';
       
    switch ( protocol )
    {
    case CMS_WAN_TYPE_PPPOE:
    case CMS_WAN_TYPE_ETHERNET_PPPOE:
       strcpy(name, PPPOE_PROTO_STR);
       break;
    case CMS_WAN_TYPE_PPPOA:
       strcpy(name, PPPOA_PROTO_STR);
       break;
    case CMS_WAN_TYPE_DYNAMIC_IPOE:
    case CMS_WAN_TYPE_DYNAMIC_ETHERNET_IP:
       strcpy(name, IPOE_PROTO_STR);
       break;
    case CMS_WAN_TYPE_IPOA:
       strcpy(name, IPOA_PROTO_STR);
       break;
    case CMS_WAN_TYPE_BRIDGE:
    case CMS_WAN_TYPE_ETHERNET_BRIDGE:
       strcpy(name, BRIDGE_PROTO_STR);
       break;
    default:
       strcpy(name, "Not Applicable");
       break;
    }
}



CmsRet cmsDalDpx_updateDnsproxy(void)
{
   return(rutDpx_updateDnsproxy());
}

#ifdef SUPPORT_UPNP

CmsRet dalUpnp_config(UBOOL8 enableUpnp, UBOOL8 *outNeedSave)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UpnpCfgObject *upnpCfg=NULL;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_UPNP_CFG, &iidStack, 0, (void **) &upnpCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get UPNP_CFG, ret=%d", ret);
   }
   else
   {
      if (upnpCfg->enable != enableUpnp)
      {
         upnpCfg->enable = enableUpnp;

         ret = cmsObj_set(upnpCfg, &iidStack);
         cmsObj_free((void **) &upnpCfg);
         
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("set upnpcfg failed, ret=%d", ret);
         }
         else
         {
            cmsLog_debug("set upnpcfg done.");
            *outNeedSave = TRUE;
         }
      }
      else
      {
         cmsLog_debug("No change, enblUpnp = %d", enableUpnp);
      }
   }

   return ret;
   
}

UBOOL8 cmsDal_isUpnpEnabled(void)
{
   return (rut_isUpnpEnabled());
}

#endif  /* SUPPORT_UPNP */


#if defined(DMP_BASELINE_1)
#ifdef SUPPORT_TR69C
CmsRet cmsDal_setTr69cCfg_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   ManagementServerObject *acsCfg=NULL;
   Tr69cCfgObject *tr69cCfg = NULL;
   CmsRet ret;

   cmsLog_debug("get tr69c cfg obj");
   if ((ret = cmsObj_get(MDMOID_TR69C_CFG, &iidStack, 0, (void *) &tr69cCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TR69C_CFG failed, ret=%d", ret);
      return ret;
   }

   tr69cCfg->loggingSOAP = (webVar->tr69cDebugEnable == FALSE) ? FALSE : TRUE;
   tr69cCfg->connectionRequestAuthentication = (webVar->tr69cNoneConnReqAuth == TRUE) ? FALSE : TRUE;

   cmsLog_debug("set tr69c cfg obj");
   ret = cmsObj_set(tr69cCfg, &iidStack);
   cmsObj_free((void **) &tr69cCfg);
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of TR69C_CFG failed, ret=%d", ret);
   }


   cmsLog_debug("Get management server obj");
   if ((ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void *) &acsCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of MANAGEMENT_SERVER failed, ret=%d", ret);
      return ret;
   }

   acsCfg->periodicInformEnable = (webVar->tr69cInformEnable == FALSE) ? FALSE : TRUE;
   acsCfg->periodicInformInterval = webVar->tr69cInformInterval;

   REPLACE_STRING_IF_NOT_EQUAL(acsCfg->URL, webVar->tr69cAcsURL);
   REPLACE_STRING_IF_NOT_EQUAL(acsCfg->username, webVar->tr69cAcsUser);
   REPLACE_STRING_IF_NOT_EQUAL(acsCfg->password, webVar->tr69cAcsPwd);

   REPLACE_STRING_IF_NOT_EQUAL(acsCfg->connectionRequestUsername, webVar->tr69cConnReqUser);
   REPLACE_STRING_IF_NOT_EQUAL(acsCfg->connectionRequestPassword, webVar->tr69cConnReqPwd);
   REPLACE_STRING_IF_NOT_EQUAL(acsCfg->X_BROADCOM_COM_BoundIfName, webVar->tr69cBoundIfName);

   cmsLog_debug("set management server obj");
   ret = cmsObj_set(acsCfg, &iidStack);
   cmsObj_free((void **) &acsCfg);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of MANAGEMENT_SERVER failed, ret=%d", ret);
      return ret;
   }
   else
   {
      /* recreate /var/dnsinfo for updating the specific wan interface dns  for resolve needs */
      cmsDalDpx_updateDnsproxy();
   }

   cmsLog_debug("done, ret=%d", ret);

   return ret;
}


void cmsDal_getTr69cCfg_igd(WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   ManagementServerObject *acsCfg=NULL;
   Tr69cCfgObject *tr69cCfg = NULL;
   CmsRet ret;
   
   cmsLog_debug("update glbWebVar tr69c info from MDM");

   if ((ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void *) &acsCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of MANAGEMENT_SERVER failed, ret=%d", ret);
      return;
   }

   webVar->tr69cInformEnable = acsCfg->periodicInformEnable;
   webVar->tr69cInformInterval = acsCfg->periodicInformInterval;

   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cAcsURL, acsCfg->URL);
   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cAcsUser, acsCfg->username);
   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cAcsPwd, acsCfg->password);

   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cConnReqURL, acsCfg->connectionRequestURL);
   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cConnReqUser, acsCfg->connectionRequestUsername);
   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cConnReqPwd, acsCfg->connectionRequestPassword);

   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cBoundIfName, acsCfg->X_BROADCOM_COM_BoundIfName);

   cmsObj_free((void **) &acsCfg);


   if ((ret = cmsObj_get(MDMOID_TR69C_CFG, &iidStack, 0, (void *) &tr69cCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TR69C_CFG failed, ret=%d", ret);
      return;
   }

   webVar->tr69cDebugEnable = (tr69cCfg->loggingSOAP == FALSE) ? FALSE : TRUE;
   webVar->tr69cNoneConnReqAuth = (tr69cCfg->connectionRequestAuthentication == TRUE) ? FALSE : TRUE;

   cmsObj_free((void **) &tr69cCfg);
}


#endif  /* SUPPORT_TR69C */
#endif  /* DMP_BASELINE_1 */




#ifdef DMP_X_ITU_ORG_GPON_1
CmsRet cmsDal_setOmciSystem(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   BcmOmciConfigSystemObject *omciSys = NULL;
   omciDebug_t omciDebug;
   CmsRet ret;

   cmsLog_debug("get OMCI system obj");
   if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void *) &omciSys)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of OMCI system object failed, ret=%d", ret);
      return ret;
   }

   omciSys->numberOfTConts = webVar->omciTcontNum;
   omciSys->tcontManagedEntityId = webVar->omciTcontMeId;
   omciSys->numberOfEthernetPorts = webVar->omciEthNum;
   omciSys->ethernetManagedEntityId1 = webVar->omciEthMeId1;
   omciSys->ethernetManagedEntityId2 = webVar->omciEthMeId2;
   omciSys->ethernetManagedEntityId3 = webVar->omciEthMeId3;
   omciSys->ethernetManagedEntityId4 = webVar->omciEthMeId4;
   omciSys->numberOfMocaPorts = webVar->omciMocaNum;
   omciSys->mocaManagedEntityId1 = webVar->omciMocaMeId1;
   omciSys->numberOfDownstreamPriorityQueues = webVar->omciDsPrioQueueNum;

   omciDebug.flags.bits.omci = webVar->omciDbgOmciEnable;
   omciDebug.flags.bits.model = webVar->omciDbgModelEnable;
   omciDebug.flags.bits.vlan = webVar->omciDbgVlanEnable;
   omciDebug.flags.bits.cmf = webVar->omciDbgCmfEnable;
   omciDebug.flags.bits.flow = webVar->omciDbgFlowEnable;
   omciDebug.flags.bits.rule = webVar->omciDbgRuleEnable;
   omciDebug.flags.bits.mcast = webVar->omciDbgMcastEnable;
   omciDebug.flags.bits.file = webVar->omciDbgFileEnable;
   omciSys->debugFlags = omciDebug.flags.all;

   omciSys->omciRawEnable = webVar->omciRawEnable;

   cmsLog_debug("set OMCI system obj");
   ret = cmsObj_set(omciSys, &iidStack);
   cmsObj_free((void **) &omciSys);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of OMCI system object failed, ret=%d", ret);
      return ret;
   }

   return ret;
}

void cmsDal_getOmciSystem(WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   BcmOmciConfigSystemObject *omciSys = NULL;
   omciDebug_t omciDebug;
   CmsRet ret;

   cmsLog_debug("get OMCI system obj");
   if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void *) &omciSys)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of OMCI system object failed, ret=%d", ret);
      return;
   }

   webVar->omciTcontNum = omciSys->numberOfTConts;
   webVar->omciTcontMeId = omciSys->tcontManagedEntityId;
   webVar->omciEthNum = omciSys->numberOfEthernetPorts;
   webVar->omciEthMeId1 = omciSys->ethernetManagedEntityId1;
   webVar->omciEthMeId2 = omciSys->ethernetManagedEntityId2;
   webVar->omciEthMeId3 = omciSys->ethernetManagedEntityId3;
   webVar->omciEthMeId4 = omciSys->ethernetManagedEntityId4;
   webVar->omciMocaNum = omciSys->numberOfMocaPorts;
   webVar->omciMocaMeId1 = omciSys->mocaManagedEntityId1;
   webVar->omciDsPrioQueueNum = omciSys->numberOfDownstreamPriorityQueues;

   omciDebug.flags.all = omciSys->debugFlags;
   webVar->omciDbgOmciEnable = omciDebug.flags.bits.omci;
   webVar->omciDbgModelEnable = omciDebug.flags.bits.model;
   webVar->omciDbgVlanEnable = omciDebug.flags.bits.vlan;
   webVar->omciDbgCmfEnable = omciDebug.flags.bits.cmf;
   webVar->omciDbgFlowEnable = omciDebug.flags.bits.flow;
   webVar->omciDbgRuleEnable = omciDebug.flags.bits.rule;
   webVar->omciDbgMcastEnable = omciDebug.flags.bits.mcast;
   webVar->omciDbgFileEnable = omciDebug.flags.bits.file;

   webVar->omciRawEnable = omciSys->omciRawEnable;

   cmsObj_free((void **) &omciSys);
}
#endif  /* DMP_X_ITU_ORG_GPON_1 */

#ifdef DMP_X_BROADCOM_COM_IPV6_1
/* This is the deprecated proprietary Broadcom IPv6 impl */
CmsRet cmsDal_setDns6Info_igd(char *dnsType, char *dnsIfc, char *dnsPri, char *dnsSec)
{
   InstanceIdStack iidStack;
   IPv6LanHostCfgObject *ipv6Obj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Enter");
   
   if (IS_EMPTY_STRING(dnsType))
   {
      cmsLog_error("dnsType is empty");
      return CMSRET_INVALID_ARGUMENTS;
   }
   if (cmsUtl_strcmp(dnsType, MDMVS_STATIC) == 0 && IS_EMPTY_STRING(dnsPri))
   {
      cmsLog_notice("dnsType==MDMVS_STATIC without primary DNS server");
      return ret;
   }
   if (cmsUtl_strcmp(dnsType, MDMVS_DHCP) == 0 && IS_EMPTY_STRING(dnsIfc))
   {
      cmsLog_error("dnsType==DHCP but dnsIfc is empty.");
      return CMSRET_INVALID_ARGUMENTS;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_getNext(MDMOID_I_PV6_LAN_HOST_CFG, &iidStack, (void **)&ipv6Obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get MDMOID_I_PV6_LAN_HOST_CFG, ret=%d", ret);
      return ret;
   }   

   if (cmsUtl_strcmp(dnsType, MDMVS_STATIC) == 0)
   {
      char dnsServers[BUFLEN_128];

      *dnsServers = '\0';

      if (!IS_EMPTY_STRING(dnsPri))
      {
         strcat(dnsServers, dnsPri);
      }
      if (!IS_EMPTY_STRING(dnsSec))
      {
         strcat(dnsServers, ",");
         strcat(dnsServers, dnsSec);
      }
      if (cmsUtl_strcmp(ipv6Obj->IPv6DNSConfigType, dnsType) == 0 &&
          cmsUtl_strcmp(ipv6Obj->IPv6DNSServers, dnsServers) == 0)
      {
         /* Nothing changed */
         cmsObj_free((void **)&ipv6Obj);
         return ret;
      }

      CMSMEM_REPLACE_STRING(ipv6Obj->IPv6DNSConfigType, dnsType);
      CMSMEM_REPLACE_STRING(ipv6Obj->IPv6DNSServers, dnsServers);

      CMSMEM_FREE_BUF_AND_NULL_PTR(ipv6Obj->IPv6DNSWANConnection);
   }
   else
   {
      if (cmsUtl_strcmp(ipv6Obj->IPv6DNSConfigType, dnsType) == 0 &&
          cmsUtl_strcmp(ipv6Obj->IPv6DNSWANConnection, dnsIfc) == 0)
      {
         /* Nothing changed */
         cmsObj_free((void **)&ipv6Obj);

         return ret;
      }

      CMSMEM_REPLACE_STRING(ipv6Obj->IPv6DNSConfigType, dnsType);
      CMSMEM_REPLACE_STRING(ipv6Obj->IPv6DNSWANConnection, dnsIfc);

      CMSMEM_FREE_BUF_AND_NULL_PTR(ipv6Obj->IPv6DNSServers);
   }

   if ((ret = cmsObj_set(ipv6Obj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_set <MDMOID_I_PV6_LAN_HOST_CFG> returns error, ret=%d", ret);
   }   
   
   cmsObj_free((void **)&ipv6Obj);
   cmsLog_debug("Exit. ret=%d", ret);

   return ret;
   
}  /* End of cmsDal_setDns6Info_igd() */

#endif   /* DMP_X_BROADCOM_COM_IPV6_1 */

#ifdef SUPPORT_IPP
CmsRet cmsDal_getPrintServerInfo(char *ippEnabled, char *ippMake, char *ippName)
{
   InstanceIdStack iidStack;
   IppCfgObject *ippObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   if (ippEnabled == NULL && ippMake == NULL && ippName == NULL)
   {
      cmsLog_error("NULL string.");
      return CMSRET_INVALID_ARGUMENTS;
   }
   if (ippEnabled != NULL)
   {
      *ippEnabled = '\0';
   }
   if (ippMake != NULL)
   {
      *ippMake = '\0';
   }
   if (ippName != NULL)
   {
      *ippName = '\0';
   }
         
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_IPP_CFG, &iidStack, 0, (void **)&ippObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_IPP_CFG> returns error. ret=%d", ret);
      return ret;
   }   

   if (ippEnabled != NULL)
   {
      sprintf(ippEnabled, "%d", ippObj->enable);
   }
   
   if (ippMake != NULL && !IS_EMPTY_STRING(ippObj->make))
   {
      strcpy(ippMake, ippObj->make);
   }
   
   if (ippName != NULL && !IS_EMPTY_STRING(ippObj->name))
   {
      strcpy(ippName, ippObj->name);
   }

   cmsObj_free((void **)&ippObj);
   return ret;        
    
}  /* End of cmsDal_getPrintServerInfo() */
#endif

#ifdef DMP_X_BROADCOM_COM_DLNA_1
CmsRet cmsDal_getDigitalMediaServerInfo(char *dmsEnabled, char *dmsMediaPath, char *dmsBrName)
{
   InstanceIdStack iidStack;
   DmsCfgObject *dmsObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   if (dmsEnabled == NULL && dmsMediaPath == NULL && dmsBrName == NULL)
   {
      cmsLog_error("NULL string.");
      return CMSRET_INVALID_ARGUMENTS;
   }
   if (dmsEnabled != NULL)
   {
      *dmsEnabled = '\0';
   }
   if (dmsMediaPath != NULL)
   {
      *dmsMediaPath = '\0';
   }
   if (dmsBrName != NULL)
   {
      *dmsBrName = '\0';
   }
         
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_DMS_CFG, &iidStack, 0, (void **)&dmsObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_DMS_CFG> returns error. ret=%d", ret);
      return ret;
   }   

   if (dmsEnabled != NULL)
   {
      sprintf(dmsEnabled, "%d", dmsObj->enable);
   }
   
   if (dmsMediaPath != NULL && !IS_EMPTY_STRING(dmsObj->mediaPath))
   {
      strcpy(dmsMediaPath, dmsObj->mediaPath);
   }

   if (dmsBrName != NULL)
   {
      /* Get the InterfaceGroupName by using the bridge key. 
       * Use the br0 (Default) when there is error.
       */
      if ((ret =  qdmIntf_getIntfGroupNameByBrKey(dmsBrName, (UINT32) dmsObj->brKey)) != CMSRET_SUCCESS)
      {
          strcpy(dmsBrName, "Default");
      }
   }
   
   cmsObj_free((void **)&dmsObj);
   return ret;        
    
}  /* End of cmsDal_getDigitalMediaServerInfo() */
#endif


#ifdef DMP_TIME_1

CmsRet cmsDal_setNtpCfg_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   TimeServerCfgObject *ntpCfg=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_get(MDMOID_TIME_SERVER_CFG, &iidStack, 0, (void *) &ntpCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TIME_SERVER_CFG failed, ret=%d", ret);
      return ret;
   }

   ntpCfg->enable = webVar->NTPEnable;
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer1, webVar->NTPServer1);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer2, webVar->NTPServer2);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer3, webVar->NTPServer3);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer4, webVar->NTPServer4);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer5, webVar->NTPServer5);
   CMSMEM_REPLACE_STRING(ntpCfg->localTimeZone, webVar->localTimeZone);
   CMSMEM_REPLACE_STRING(ntpCfg->localTimeZoneName, webVar->localTimeZoneName);
   ntpCfg->daylightSavingsUsed= (webVar->daylightSavingsUsed== FALSE) ? FALSE : TRUE;
   CMSMEM_REPLACE_STRING(ntpCfg->daylightSavingsStart, webVar->daylightSavingsStart);
   CMSMEM_REPLACE_STRING(ntpCfg->daylightSavingsEnd, webVar->daylightSavingsEnd);

   ret = cmsObj_set(ntpCfg, &iidStack);
   cmsObj_free((void **) &ntpCfg);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of TIME_SERVER_CFG failed, ret=%d", ret);
   }

   return ret;
}

void cmsDal_getNtpCfg_igd(WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   TimeServerCfgObject *ntpCfg=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_get(MDMOID_TIME_SERVER_CFG, &iidStack, 0, (void *) &ntpCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TIME_SERVER_CFG failed, ret=%d", ret);
      return;
   }

   webVar->NTPEnable = ntpCfg->enable;
   STR_COPY_OR_NULL_TERMINATE(webVar->NTPServer1, ntpCfg->NTPServer1);
   STR_COPY_OR_NULL_TERMINATE(webVar->NTPServer2, ntpCfg->NTPServer2);
   STR_COPY_OR_NULL_TERMINATE(webVar->NTPServer3, ntpCfg->NTPServer3);
   STR_COPY_OR_NULL_TERMINATE(webVar->NTPServer4, ntpCfg->NTPServer4);
   STR_COPY_OR_NULL_TERMINATE(webVar->NTPServer5, ntpCfg->NTPServer5);
   STR_COPY_OR_NULL_TERMINATE(webVar->localTimeZone, ntpCfg->localTimeZone);
   STR_COPY_OR_NULL_TERMINATE(webVar->localTimeZoneName, ntpCfg->localTimeZoneName);
   webVar->daylightSavingsUsed = (ntpCfg->daylightSavingsUsed == FALSE) ? FALSE : TRUE;
   STR_COPY_OR_NULL_TERMINATE(webVar->daylightSavingsStart, ntpCfg->daylightSavingsStart);
   STR_COPY_OR_NULL_TERMINATE(webVar->daylightSavingsEnd, ntpCfg->daylightSavingsEnd);

   cmsObj_free((void **) &ntpCfg);
}

#endif   /* DMP_TIME_1 */


CmsRet cmsDal_getAvailableIfForWanService_igd(NameList **ifList,
                                    UBOOL8 skipUsed __attribute__((unused)))
{
   CmsRet            ret;
   WanDevObject      *wanDev  = NULL;
   WanCommonIntfCfgObject *comIntf = NULL;
   WanDslIntfCfgObject  *dslIntf = NULL; 
   WanConnDeviceObject *wanCon;
   InstanceIdStack   wanDevIid;
   InstanceIdStack   conDevIid;

   NameList          head;
   NameList          *tail;
   NameList          *nameListElement  __attribute__ ((unused));
   char              name[CMS_IFNAME_LENGTH+BUFLEN_32];

#ifdef DMP_X_BROADCOM_COM_ATMWAN_1
   UBOOL8 isATM;
#endif
#ifdef DMP_PTMWAN_1
   UBOOL8 isPTM;
#endif
#ifdef DMP_ETHERNETWAN_1
   UBOOL8 isEth;
#endif
#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   UBOOL8 isMoca;
#endif
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   UBOOL8 isGpon = FALSE;
#endif
#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
   UBOOL8 isEpon = FALSE;
#endif
#ifdef DMP_X_BROADCOM_COM_WIFIWAN_1
   UBOOL8 isWifi;
#endif
#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1
   UBOOL8 isCellular;
#endif

   //supress some warnings where various locals are not used due to compile issues:
   (void)tail;
   (void)wanCon;

   *ifList = NULL;
   head.name = NULL;
   head.next = NULL;
   tail = &head;

   /* Initialization split into two lines for the case where these variables are not used.
      Normally, Werror would complain about unused variables, but splitting initialization
      will not trigger that warning. */
   wanCon = NULL; 
   nameListElement = NULL;
   *name = '\0';
   INIT_INSTANCE_ID_STACK(&conDevIid);

   /* Look for DSL WAN connection devices */      
   INIT_INSTANCE_ID_STACK(&wanDevIid);
   while ((ret = cmsObj_getNext(MDMOID_WAN_DEV, &wanDevIid, (void **)&wanDev)) == CMSRET_SUCCESS)
   {

      cmsObj_free((void **)&wanDev);  /* no longer needed */

      if ((ret = cmsObj_get(MDMOID_WAN_COMMON_INTF_CFG, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&comIntf)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_getNext <MDMOID_WAN_COMMON_INTF_CFG> returns error. ret=%d", ret);
         cmsDal_freeNameList(head.next);
         return ret;
      }
      if ((ret = cmsObj_get(MDMOID_WAN_COMMON_INTF_CFG, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&comIntf)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_getNext <MDMOID_WAN_COMMON_INTF_CFG> returns error. ret=%d", ret);
         cmsDal_freeNameList(head.next);
         return ret;
      }

      if (!cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_DSL))
      {
         if ((ret = cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&dslIntf)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_getNextInSubTree  <MDMOID_WAN_DSL_INTF_CFG> error. ret=%d", ret);
            cmsDal_freeNameList(head.next);
            cmsObj_free((void **) &comIntf);
            return ret;
         }
      }

#ifdef DMP_X_BROADCOM_COM_ATMWAN_1
      isATM = (!cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_DSL) &&
               !cmsUtl_strcmp(dslIntf->linkEncapsulationUsed, MDMVS_G_992_3_ANNEX_K_ATM));
#endif

#ifdef DMP_PTMWAN_1
      isPTM = (!cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_DSL) &&
               !cmsUtl_strcmp(dslIntf->linkEncapsulationUsed, MDMVS_G_993_2_ANNEX_K_PTM));
#endif

#ifdef DMP_ADSLWAN_1
      cmsObj_free((void **)&dslIntf);
#endif

#ifdef DMP_ETHERNETWAN_1
      isEth = (!cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_ETHERNET));
#endif

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
      isMoca = (!cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_X_BROADCOM_COM_MOCA));
#endif

#ifdef DMP_X_BROADCOM_COM_PONWAN_1
      if (!cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_X_BROADCOM_COM_PON))
      {
         WanPonIntfObject *ponObj=NULL;

         if ((ret = cmsObj_get(MDMOID_WAN_PON_INTF, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&ponObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_get  <MDMOID_WAN_PON_INTF> error. ret=%d", ret);
            cmsObj_free((void **) &comIntf);                  
            cmsDal_freeNameList(head.next);
            return ret;
         }     
         
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1      
         isGpon = (!cmsUtl_strcmp(ponObj->ponType, MDMVS_GPON));
#endif
#ifdef DMP_X_BROADCOM_COM_EPONWAN_1      
         isEpon = (!cmsUtl_strcmp(ponObj->ponType, MDMVS_EPON));
#endif

         cmsObj_free((void **) &ponObj);
      }
#endif /* DMP_X_BROADCOM_COM_PONWAN_1 */

#ifdef DMP_X_BROADCOM_COM_WIFIWAN_1
      isWifi = (!cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_X_BROADCOM_COM_WIFI));
#endif

#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1
      isCellular = (!cmsUtl_strcmp(comIntf->WANAccessType, MDMVS_X_BROADCOM_COM_CELLULAR));
#endif

      /* no longer needed */
      cmsObj_free((void **)&comIntf);

#ifdef DMP_X_BROADCOM_COM_ATMWAN_1
      if (isATM)
      {
         WanDslLinkCfgObject *dslLink=NULL;
         SINT32 vpi, vci;

         cmsLog_debug("searching for L2 interfaces in ATM WANDevice");

         INIT_INSTANCE_ID_STACK(&conDevIid);
         while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &wanDevIid, &conDevIid,
                                               (void **)&wanCon)) == CMSRET_SUCCESS)
         {
            if ((ret = cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &conDevIid, OGF_NO_VALUE_UPDATE, (void **)&dslLink)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_get<MDMOID_DSL_LINK_CFG> returns error, ret=%d", ret);
               cmsObj_free((void **)&wanCon);
               return ret;
            }


            cmsLog_debug("%s skipUsed=%d connectionMode=%s, IpConnections=%d pppConnections=%d",
                         dslLink->X_BROADCOM_COM_IfName,
                         skipUsed,
                         dslLink->X_BROADCOM_COM_ConnectionMode,
                         wanCon->WANIPConnectionNumberOfEntries,
                         wanCon->WANPPPConnectionNumberOfEntries);
                         
            /*
             * If skipUsed and if link is in default connection mode, and is already 
             * used by one ip/ppp connection, it is not available.
             * Note: IPoA and PPPoA is default connection mode only 
             */
            if (skipUsed &&
                !cmsUtl_strcmp(dslLink->X_BROADCOM_COM_ConnectionMode, MDMVS_DEFAULTMODE) &&
                (wanCon->WANIPConnectionNumberOfEntries != 0 || wanCon->WANPPPConnectionNumberOfEntries != 0))
            {
               cmsObj_free((void **)&wanCon);
               cmsObj_free((void **)&dslLink);
               continue;
            }

            cmsObj_free((void **)&wanCon);


            /* get portId, vpi and vci */
            cmsUtl_atmVpiVciStrToNum(dslLink->destinationAddress, &vpi, &vci);
            
            /* format interface name */
            sprintf(name, "%s/(%d_%d_%d)", dslLink->X_BROADCOM_COM_IfName, dslLink->X_BROADCOM_COM_ATMInterfaceId, vpi, vci);
   
            cmsObj_free((void **)&dslLink);  /* no longer needed */
   
            nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
            if (nameListElement == NULL)
            {
               cmsLog_error("could not allocate NameList");
               cmsDal_freeNameList(head.next);
               return CMSRET_RESOURCE_EXCEEDED;
            }

            /* append to name list */
            cmsLog_debug("adding %s to list of available ifNames", name);
            nameListElement->name = cmsMem_strdup(name);
            nameListElement->next = NULL;
            tail->next = nameListElement;
            tail = nameListElement;
         }  /* while */
      }
#endif /* DMP_X_BROADCOM_COM_ATMWAN_1 */

#ifdef DMP_PTMWAN_1
      if (isPTM)
      {
         WanPtmLinkCfgObject *ptmLink = NULL;
   
         cmsLog_debug("searching for L2 interface in PTM WANDevices");
         
         INIT_INSTANCE_ID_STACK(&conDevIid);
         while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &wanDevIid, &conDevIid,
                                               (void **)&wanCon)) == CMSRET_SUCCESS)
         {

            if ((ret = cmsObj_get(MDMOID_WAN_PTM_LINK_CFG, &conDevIid, OGF_NO_VALUE_UPDATE, (void **)&ptmLink)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_get<MDMOID_PTM_LINK_CFG> returns error, ret=%d", ret);
               cmsObj_free((void **)&wanCon);
               return ret;
            }

            cmsLog_debug("%s skipUsed=%d connectionMode=%s ipConnections=%d pppConnections=%d",
                         ptmLink->X_BROADCOM_COM_IfName,
                         skipUsed,
                         ptmLink->X_BROADCOM_COM_ConnectionMode,
                         wanCon->WANIPConnectionNumberOfEntries, wanCon->WANPPPConnectionNumberOfEntries);


            /*
             * If skipUsed and if link is in default connection mode, and is already 
             * used by one ip/ppp connection, it is not available.
             */
            if (skipUsed &&
                !cmsUtl_strcmp(ptmLink->X_BROADCOM_COM_ConnectionMode, MDMVS_DEFAULTMODE) &&
                (wanCon->WANIPConnectionNumberOfEntries != 0 || wanCon->WANPPPConnectionNumberOfEntries != 0))
            {
               cmsObj_free((void **)&ptmLink);
               cmsObj_free((void **)&wanCon);
               continue;
            }
   
            cmsObj_free((void **)&wanCon);

            /* format interface name */
            sprintf(name, "%s/(%d_%d_%d)", ptmLink->X_BROADCOM_COM_IfName, ptmLink->X_BROADCOM_COM_PTMPortId, 
                    ptmLink->X_BROADCOM_COM_PTMPriorityHigh, ptmLink->X_BROADCOM_COM_PTMPriorityLow);
            cmsObj_free((void **)&ptmLink);  /* no longer needed */
   
            nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
            if (nameListElement == NULL)
            {
               cmsLog_error("could not allocate NameList");
               cmsDal_freeNameList(head.next);
               return CMSRET_RESOURCE_EXCEEDED;
            }

            /* append to name list */
            cmsLog_debug("adding %s to list of available ifNames", name);
            nameListElement->name = cmsMem_strdup(name);
            nameListElement->next = NULL;
            tail->next = nameListElement;
            tail = nameListElement;
         }  /* while */
      }
#endif /* DMP_PTMWAN_1 */

#ifdef DMP_ETHERNETWAN_1
      if (isEth)
      {
         WanEthIntfObject  *ethIntf = NULL;

         cmsLog_debug("searching for L2 interface in Eth WANDevices");

         if( (ret = cmsObj_get(MDMOID_WAN_ETH_INTF, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&ethIntf)) != CMSRET_SUCCESS )
         {
            cmsLog_error("Cannot get WanEthIntfObject, ret = %d", ret);
            return ret;
         }

         if (ethIntf->enable)
         {
            /*
             * there should only be 1 WanConnectionDevice under this sub-tree.
             * Get that and do the single service in default mode check.
             */
            INIT_INSTANCE_ID_STACK(&conDevIid);
            if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &wanDevIid, &conDevIid, (void **) &wanCon)) != CMSRET_SUCCESS)
            {
               cmsLog_error("could not get WanConnectionDevice, ret=%d", ret);
               cmsObj_free((void **) &ethIntf);
               continue;
            }

            cmsLog_debug("wanEth skipUsed=%d connectionMode=%s ipConnections=%d pppConnections=%d",
                         skipUsed,
                         ethIntf->X_BROADCOM_COM_ConnectionMode,
                         wanCon->WANIPConnectionNumberOfEntries, wanCon->WANPPPConnectionNumberOfEntries);

            /*
             * If skipUsed and if link is in default connection mode, and is already 
             * used by one ip/ppp connection, it is not available.
             */
            if (skipUsed &&
                !cmsUtl_strcmp(ethIntf->X_BROADCOM_COM_ConnectionMode, MDMVS_DEFAULTMODE) &&
                (wanCon->WANIPConnectionNumberOfEntries != 0 || wanCon->WANPPPConnectionNumberOfEntries != 0))
            {
               cmsObj_free((void **)&wanCon);
               cmsObj_free((void **)&ethIntf);
               continue;
            }
   
            /* format interface name */
            strcpy(name, ethIntf->X_BROADCOM_COM_IfName);
            cmsObj_free((void **)&ethIntf);

            nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
            if (nameListElement == NULL)
            {
               cmsLog_error("could not allocate NameList");
               cmsDal_freeNameList(head.next);
               return CMSRET_RESOURCE_EXCEEDED;
            }

            /* append to name list */
            cmsLog_debug("adding %s to list of available ifNames", name);
            nameListElement->name = cmsMem_strdup(name);
            nameListElement->next = NULL;
            tail->next = nameListElement;
            tail = nameListElement;
         }
      }
#endif /* DMP_ETHERNETWAN_1 */

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
      if (isMoca)
      {
         WanMocaIntfObject  *mocaIntf = NULL;

         cmsLog_debug("searching for L2 interface in Moca WANDevices");

         if( (ret = cmsObj_get(MDMOID_WAN_MOCA_INTF, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&mocaIntf)) != CMSRET_SUCCESS )
         {
            cmsLog_error("Cannot get WanMocaIntfObject, ret = %d", ret);
            return ret;
         }

         if (mocaIntf->enable)
         {
            /*
             * there should only be 1 WanConnectionDevice under this sub-tree.
             * Get that and do the single service in default mode check.
             */
            INIT_INSTANCE_ID_STACK(&conDevIid);
            if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &wanDevIid, &conDevIid, (void **) &wanCon)) != CMSRET_SUCCESS)
            {
               cmsLog_error("could not get WanConnectionDevice, ret=%d", ret);
               cmsObj_free((void **) &mocaIntf);
               continue;
            }

            cmsLog_debug("mocaWan skipUsed=%d connectionMode=%s ipConnections=%d pppConnections=%d",
                         skipUsed,
                         mocaIntf->connectionMode,
                         wanCon->WANIPConnectionNumberOfEntries, wanCon->WANPPPConnectionNumberOfEntries);

            /*
             * If skipUsed and if link is in default connection mode, and is already 
             * used by one ip/ppp connection, it is not available.
             */
            if (skipUsed &&
                !cmsUtl_strcmp(mocaIntf->connectionMode, MDMVS_DEFAULTMODE) &&
                (wanCon->WANIPConnectionNumberOfEntries != 0 || wanCon->WANPPPConnectionNumberOfEntries != 0))
            {
               cmsObj_free((void **)&wanCon);
               cmsObj_free((void **)&mocaIntf);
               continue;
            }
   
            /* format interface name */
            strcpy(name, mocaIntf->ifName);
            cmsObj_free((void **)&mocaIntf);
   
            nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
            if (nameListElement == NULL)
            {
               cmsLog_error("could not allocate NameList");
               cmsDal_freeNameList(head.next);
               return CMSRET_RESOURCE_EXCEEDED;
            }

            /* append to name list */
            cmsLog_debug("adding %s to list of available ifNames", name);
            nameListElement->name = cmsMem_strdup(name);
            nameListElement->next = NULL;
            tail->next = nameListElement;
            tail = nameListElement;
         }
      }
#endif /* DMP_X_BROADCOM_COM_MOCAWAN_1 */

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
      if (isGpon)
      {
         WanGponLinkCfgObject *gponLink = NULL;
         
         cmsLog_debug("searching for L2 interface in Gpon WANConnDevices");
         
         INIT_INSTANCE_ID_STACK(&conDevIid);
         while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &wanDevIid, &conDevIid,
                                               (void **)&wanCon)) == CMSRET_SUCCESS)
         {
            if ((ret = cmsObj_get(MDMOID_WAN_GPON_LINK_CFG, &conDevIid, OGF_NO_VALUE_UPDATE, (void **)&gponLink)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_get<MDMOID_WAN_GPON_LINK_CFG> returns error, ret=%d", ret);
               cmsObj_free((void **)&wanCon);
               return ret;
            }

            cmsLog_debug("%s skipUsed=%d connectionMode=%s ipConnections=%d pppConnections=%d gponlink->enable=%d",
                         gponLink->ifName,
                         skipUsed,
                         gponLink->connectionMode,
                         wanCon->WANIPConnectionNumberOfEntries, wanCon->WANPPPConnectionNumberOfEntries,
                         gponLink->enable);

            /*
             * skip this wanconn
             * If gponLinkCfg->enable is FALSE, or
             * skipUsed == TRUE (skip this wan conn object) AND it is already used as a WAN connection and is in default connection mode
             */
            if (!gponLink->enable ||
                 (skipUsed &&  
                 !cmsUtl_strcmp(gponLink->connectionMode, MDMVS_DEFAULTMODE) &&
                 (wanCon->WANIPConnectionNumberOfEntries != 0 || wanCon->WANPPPConnectionNumberOfEntries != 0)))
            {
               cmsObj_free((void **)&gponLink);
               cmsObj_free((void **)&wanCon);
               continue;
            }
   
            cmsObj_free((void **)&wanCon);

            /* format interface name */
            strcpy(name, gponLink->ifName);
            cmsObj_free((void **)&gponLink);  /* no longer needed */
   
            nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
            if (nameListElement == NULL)
            {
               cmsLog_error("could not allocate NameList");
               cmsDal_freeNameList(head.next);
               return CMSRET_RESOURCE_EXCEEDED;
            }

            /* append to name list */
            cmsLog_debug("adding %s to list of available ifNames", name);
            nameListElement->name = cmsMem_strdup(name);
            nameListElement->next = NULL;
            tail->next = nameListElement;
            tail = nameListElement;
         }  /* while */
      }
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
      if (isEpon)
      {
#ifdef EPON_SFU
         WanEponIntfObject  *eponIntf = NULL;

         cmsLog_debug("searching for L2 interface in Epon WANDevices");

         if( (ret = cmsObj_get(MDMOID_WAN_EPON_INTF, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&eponIntf)) != CMSRET_SUCCESS )
         {
            cmsLog_error("Cannot get WanEponIntfObject, ret = %d", ret);
            return ret;
         }

         if (eponIntf->enable)
         {
            /*
             * there should only be 1 WanConnectionDevice under this sub-tree.
             * Get that and do the single service in default mode check.
             */
            INIT_INSTANCE_ID_STACK(&conDevIid);
            if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &wanDevIid, &conDevIid, (void **) &wanCon)) != CMSRET_SUCCESS)
            {
               cmsLog_error("could not get WanConnectionDevice, ret=%d", ret);
               cmsObj_free((void **) &eponIntf);
               continue;
            }

            cmsLog_debug("eponWan skipUsed=%d connectionMode=%s ipConnections=%d pppConnections=%d",
                         skipUsed,
                         eponIntf->connectionMode,
                         wanCon->WANIPConnectionNumberOfEntries, wanCon->WANPPPConnectionNumberOfEntries);

            /*
             * If skipUsed and if link is in default connection mode, and is already 
             * used by one ip/ppp connection, it is not available.
             */
            if (skipUsed &&
                !cmsUtl_strcmp(eponIntf->connectionMode, MDMVS_DEFAULTMODE) &&
                (wanCon->WANIPConnectionNumberOfEntries != 0 || wanCon->WANPPPConnectionNumberOfEntries != 0))
            {
               cmsObj_free((void **)&wanCon);
               cmsObj_free((void **)&eponIntf);
               continue;
            }
   
            /* format interface name */
            strcpy(name, eponIntf->ifName);
            cmsObj_free((void **)&eponIntf);
   
            nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
            if (nameListElement == NULL)
            {
               cmsLog_error("could not allocate NameList");
               cmsDal_freeNameList(head.next);
               return CMSRET_RESOURCE_EXCEEDED;
            }

            /* append to name list */
            cmsLog_debug("adding %s to list of available ifNames", name);
            nameListElement->name = cmsMem_strdup(name);
            nameListElement->next = NULL;
            tail->next = nameListElement;
            tail = nameListElement;
         }
#else
         WanEponLinkCfgObject *eponLink = NULL;
         
         cmsLog_debug("searching for L2 interface in Epon WANConnDevices");
         
         INIT_INSTANCE_ID_STACK(&conDevIid);
         while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &wanDevIid, &conDevIid,
                                               (void **)&wanCon)) == CMSRET_SUCCESS)
         {
            if ((ret = cmsObj_get(MDMOID_WAN_EPON_LINK_CFG, &conDevIid, OGF_NO_VALUE_UPDATE, (void **)&eponLink)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_get<MDMOID_WAN_EPON_LINK_CFG> returns error, ret=%d", ret);
               cmsObj_free((void **)&wanCon);
               return ret;
            }

            cmsLog_debug("%s skipUsed=%d connectionMode=%s ipConnections=%d pppConnections=%d eponlink->enable=%d",
                         eponLink->ifName,
                         skipUsed,
                         eponLink->connectionMode,
                         wanCon->WANIPConnectionNumberOfEntries, wanCon->WANPPPConnectionNumberOfEntries,
                         eponLink->enable);

            /*
             * skip this wanconn
             * If eponLinkCfg->enable is FALSE, or
             * skipUsed == TRUE (skip this wan conn object) AND it is already used as a WAN connection and is in default connection mode
             */
            if (!eponLink->enable ||
                 (skipUsed &&  
                 !cmsUtl_strcmp(eponLink->connectionMode, MDMVS_DEFAULTMODE) &&
                 (wanCon->WANIPConnectionNumberOfEntries != 0 || wanCon->WANPPPConnectionNumberOfEntries != 0)))
            {
               cmsObj_free((void **)&eponLink);
               cmsObj_free((void **)&wanCon);
               continue;
            }
   
            cmsObj_free((void **)&wanCon);

            /* format interface name */
            strcpy(name, eponLink->ifName);
            cmsObj_free((void **)&eponLink);  /* no longer needed */
   
            nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
            if (nameListElement == NULL)
            {
               cmsLog_error("could not allocate NameList");
               cmsDal_freeNameList(head.next);
               return CMSRET_RESOURCE_EXCEEDED;
            }

            /* append to name list */
            cmsLog_debug("adding %s to list of available ifNames", name);
            nameListElement->name = cmsMem_strdup(name);
            nameListElement->next = NULL;
            tail->next = nameListElement;
            tail = nameListElement;
         }  /* while */
#endif
      }

      #endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */

#ifdef DMP_X_BROADCOM_COM_WIFIWAN_1
      if (isWifi)
      {
         WanWifiIntfObject  *wifiIntf = NULL;

         cmsLog_debug("searching for L2 interface in Wifi WANDevices");

         if( (ret = cmsObj_get(MDMOID_WAN_WIFI_INTF, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&wifiIntf)) != CMSRET_SUCCESS )
         {
            cmsLog_error("Cannot get WanWifiIntfObject, ret = %d", ret);
            return ret;
         }

         if (wifiIntf->enable)
         {
            /*
             * there should only be 1 WanConnectionDevice under this sub-tree.
             * Get that and do the single service in default mode check.
             */
            INIT_INSTANCE_ID_STACK(&conDevIid);
            if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &wanDevIid, &conDevIid, (void **) &wanCon)) != CMSRET_SUCCESS)
            {
               cmsLog_error("could not get WanConnectionDevice, ret=%d", ret);
               cmsObj_free((void **) &wifiIntf);
               continue;
            }

            cmsLog_debug("wanWifi skipUsed=%d connectionMode=%s ipConnections=%d pppConnections=%d",
                         skipUsed,
                         wifiIntf->connectionMode,
                         wanCon->WANIPConnectionNumberOfEntries, wanCon->WANPPPConnectionNumberOfEntries);

            /*
             * If skipUsed and if link is in default connection mode, and is already 
             * used by one ip/ppp connection, it is not available.
             */
            if (skipUsed &&
                !cmsUtl_strcmp(wifiIntf->connectionMode, MDMVS_DEFAULTMODE) &&
                (wanCon->WANIPConnectionNumberOfEntries != 0 || wanCon->WANPPPConnectionNumberOfEntries != 0))
            {
               cmsObj_free((void **)&wanCon);
               cmsObj_free((void **)&wifiIntf);
               continue;
            }
   
            /* format interface name */
            strcpy(name, wifiIntf->ifName);
            cmsObj_free((void **)&wifiIntf);
   
            nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
            if (nameListElement == NULL)
            {
               cmsLog_error("could not allocate NameList");
               cmsDal_freeNameList(head.next);
               return CMSRET_RESOURCE_EXCEEDED;
            }

            /* append to name list */
            cmsLog_debug("adding %s to list of available ifNames", name);
            nameListElement->name = cmsMem_strdup(name);
            nameListElement->next = NULL;
            tail->next = nameListElement;
            tail = nameListElement;
         }
      }
#endif /* DMP_X_BROADCOM_COM_WIFIWAN_1 */

#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1
      /* Cellular wan interface is created when APN is enable. 
          there's 1 WANIPConnection instance for WANConnectionDevice*/
      if (isCellular && !skipUsed)
      {
         WanIpConnObject *wanIpConn;
      
         INIT_INSTANCE_ID_STACK(&conDevIid);
         while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &wanDevIid, &conDevIid,
                                               (void **)&wanIpConn)) == CMSRET_SUCCESS)
         { 
            nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
            if (nameListElement == NULL)
            {
               cmsLog_error("could not allocate NameList");
               cmsDal_freeNameList(head.next);
               cmsObj_free((void **)&wanIpConn);
               return CMSRET_RESOURCE_EXCEEDED;
            }
    
            /* append to name list */
            cmsLog_debug("adding %s to list of available ifNames", name);
            nameListElement->name = cmsMem_strdup(wanIpConn->X_BROADCOM_COM_IfName);
            nameListElement->next = NULL;
            tail->next = nameListElement;
            tail = nameListElement;

            cmsObj_free((void **)&wanIpConn);
         }
      }
#endif /* DMP_X_BROADCOM_COM_WIFIWAN_1 */

   } /* while loop over all WANDevices */

   
   /* return the name list */
   *ifList = head.next;

   return CMSRET_SUCCESS;

}  /* End of cmsDal_getAvailableIfForWanService() */


#ifdef DMP_ETHERNETWAN_1
CmsRet cmsDal_getAvailableL2EthIntf_igd(NameList **ifList)
{
   CmsRet ret;
   LanDevObject *lanDev  = NULL;
   LanEthIntfObject *ethIntf = NULL;
   InstanceIdStack ethIid=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack lanDevIid=EMPTY_INSTANCE_ID_STACK;

   /* Look for Ethernet interface under LANDev.1. */      
   INIT_INSTANCE_ID_STACK(&lanDevIid);
   if ((ret = cmsObj_getNext(MDMOID_LAN_DEV, &lanDevIid, (void **)&lanDev)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_LAN_DEV> returns error. ret=%d", ret);
      return ret;
   }
   cmsObj_free((void **)&lanDev);   /* no longer needed */

   while (cmsObj_getNextInSubTree(MDMOID_LAN_ETH_INTF, &lanDevIid, &ethIid,
                                  (void **)&ethIntf) == CMSRET_SUCCESS)
   {
      if (!ethIntf->enable)
      {
         cmsObj_free((void **)&ethIntf);
         continue;
      }

      /* if ethIntf is NOT LANONLY, add to WAN capable list */
      if (cmsUtl_strcmp(ethIntf->X_BROADCOM_COM_WanLan_Attribute, MDMVS_LANONLY))
      {
         if (cmsDal_addNameToNameList(ethIntf->X_BROADCOM_COM_IfName, ifList) == NULL)
         {
            cmsDal_freeNameList(*ifList);
            cmsObj_free((void **)&ethIntf);
            return CMSRET_RESOURCE_EXCEEDED;
         }
      }
      else
      {
         cmsLog_debug("skip LANONLY ethIntf %s", ethIntf->X_BROADCOM_COM_IfName);
      }
   
      cmsObj_free((void **)&ethIntf);
   }  /* while */

   return CMSRET_SUCCESS;

}  /* End of cmsDal_getAvailableL2EthIntf() */



CmsRet cmsDal_getWANOnlyEthIntf_igd(char *gWanOnlyEthIntf)
{
   CmsRet ret = CMSRET_SUCCESS;
   LanDevObject *lanDev  = NULL;
   LanEthIntfObject *ethIntf = NULL;
   InstanceIdStack ethIid=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack lanDevIid=EMPTY_INSTANCE_ID_STACK;

   /* Look for Ethernet interface under LANDev.1. */      
   if ((ret = cmsObj_getNext(MDMOID_LAN_DEV, &lanDevIid, (void **)&lanDev)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_LAN_DEV> returns error. ret=%d", ret);
      return ret;
   }
   cmsObj_free((void **)&lanDev);   /* no longer needed */

   while (cmsObj_getNextInSubTree(MDMOID_LAN_ETH_INTF, &lanDevIid, &ethIid, (void **)&ethIntf) == CMSRET_SUCCESS)
   {
      /* Get WAN Only Eth interface */
      if (ethIntf->enable && !cmsUtl_strcmp(ethIntf->X_BROADCOM_COM_WanLan_Attribute, MDMVS_WANONLY))
      {
         if (cmsUtl_strlen(gWanOnlyEthIntf) > 0)
         {
            cmsUtl_strcat(gWanOnlyEthIntf, ",");
         }
         cmsUtl_strcat(gWanOnlyEthIntf, ethIntf->X_BROADCOM_COM_IfName);
      }
      cmsObj_free((void **)&ethIntf);
   }

   cmsLog_debug("WanOnlyEthIntf=%s", gWanOnlyEthIntf);

   return ret;
   
}


CmsRet cmsDal_getGMACEthIntf_igd(char *gMACEthIntf)
{
   CmsRet ret = CMSRET_SUCCESS;
   LanDevObject *lanDev  = NULL;
   LanEthIntfObject *lanEthIntf = NULL;
   InstanceIdStack  ethIid;
   InstanceIdStack  lanDevIid;

   /* Look for Ethernet interface under LANDev.1. */      
   INIT_INSTANCE_ID_STACK(&lanDevIid);
   if ((ret = cmsObj_getNext(MDMOID_LAN_DEV, &lanDevIid, (void **)&lanDev)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_LAN_DEV> returns error. ret=%d", ret);
      return ret;
   }

   cmsObj_free((void **)&lanDev);   /* no longer needed */

   INIT_INSTANCE_ID_STACK(&ethIid);
   while (cmsObj_getNextInSubTree(MDMOID_LAN_ETH_INTF, &lanDevIid, &ethIid, (void **)&lanEthIntf) == CMSRET_SUCCESS)
   {
      if (lanEthIntf->enable && 
         (!cmsUtl_strcmp(lanEthIntf->X_BROADCOM_COM_WanLan_Attribute, MDMVS_WANPREPERRED) ||
          lanEthIntf->X_BROADCOM_COM_GMAC_Enabled))    /* For backward compatabilty and  will be depreciated later on */
      {
         if (cmsUtl_strlen(gMACEthIntf) > 0)
         {
            cmsUtl_strcat(gMACEthIntf, ",");
         }
         cmsUtl_strcat(gMACEthIntf, lanEthIntf->X_BROADCOM_COM_IfName);
      }
      cmsObj_free((void **)&lanEthIntf);
   }

   cmsLog_debug("gMACEthIntf=%s", gMACEthIntf);

   return ret;
   
}


#endif  /* DMP_ETHERNETWAN_1 */


#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
CmsRet cmsDal_getAvailableL2MocaIntf_igd(NameList **ifList)
{
   CmsRet            ret;
   LanDevObject      *lanDev  = NULL;
   LanMocaIntfObject *mocaIntf = NULL;
   InstanceIdStack   mocaIid;
   InstanceIdStack   lanDevIid;
   NameList          head;
   NameList          *tail, *nameListElement;
   UINT32 flags=OGF_NO_VALUE_UPDATE;
   
   *ifList = NULL;
   head.name = NULL;
   head.next = NULL;
   tail = &head;

   /* Look for Moca interface under LANDev.1. only */      
   INIT_INSTANCE_ID_STACK(&lanDevIid);
   if ((ret = cmsObj_getNext(MDMOID_LAN_DEV, &lanDevIid, (void **)&lanDev)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_LAN_DEV> returns error. ret=%d", ret);
      return ret;
   }

   cmsObj_free((void **)&lanDev);   /* no longer needed */

   INIT_INSTANCE_ID_STACK(&mocaIid);
   while ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_LAN_MOCA_INTF, &lanDevIid, &mocaIid, flags, (void **)&mocaIntf)) == CMSRET_SUCCESS)
   {
      if (mocaIntf->enable)
      {
         nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
         if (nameListElement == NULL)
         {
            cmsLog_error("could not allocate nameListElement");
            cmsObj_free((void **)&mocaIntf);
            cmsDal_freeNameList(head.next);
            return CMSRET_RESOURCE_EXCEEDED;
         }

         /* append to name list */
         cmsLog_debug("adding %s to list", mocaIntf->ifName);
         nameListElement->name = cmsMem_strdup(mocaIntf->ifName);
         nameListElement->next = NULL;
         tail->next = nameListElement;
         tail = nameListElement;
      }

      cmsObj_free((void **)&mocaIntf);
   }

   /* return the name list */
   *ifList = head.next;

   return CMSRET_SUCCESS;

} 
#endif  /* DMP_X_BROADCOM_COM_MOCAWAN_1 */


#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
CmsRet cmsDal_getAvailableL2GponIntf_igd(NameList **ifList)
{

   NameList head;
   NameList *tail, *nameListElement;
   InstanceIdStack  wanDevIid = EMPTY_INSTANCE_ID_STACK;  
   InstanceIdStack  gponLinkIid = EMPTY_INSTANCE_ID_STACK;   
   WanGponLinkCfgObject *gponLinkCfg = NULL;
   CmsRet  ret;

   *ifList = NULL;
   head.name = NULL;
   head.next = NULL;
   tail = &head;

   /* Get Gpon WanDevice iidStack first */      
   if ((ret = dalGpon_getGponWanIidStatck(&wanDevIid)) != CMSRET_SUCCESS)
   {
      return ret;
   }
   cmsLog_debug("wanDevIid=%s",  cmsMdm_dumpIidStack(&wanDevIid));
   while ((ret = cmsObj_getNextInSubTreeFlags
      (MDMOID_WAN_GPON_LINK_CFG, &wanDevIid,  &gponLinkIid, OGF_NO_VALUE_UPDATE, (void **)&gponLinkCfg)) == CMSRET_SUCCESS)
   {
      cmsLog_debug("found %s, enable=%d", gponLinkCfg->ifName, gponLinkCfg->enable);   
      if (!gponLinkCfg->enable)
      {
         nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
         if (nameListElement == NULL)
         {
            cmsLog_error("could not allocate nameListElement");
            cmsObj_free((void **)&gponLinkCfg);
            cmsDal_freeNameList(head.next);
            return CMSRET_RESOURCE_EXCEEDED;
         }

         /* append to name list */
         cmsLog_debug("adding %s to list", gponLinkCfg->ifName);
         nameListElement->name = cmsMem_strdup(gponLinkCfg->ifName);
         nameListElement->next = NULL;
         tail->next = nameListElement;
         tail = nameListElement;
      }

      cmsObj_free((void **)&gponLinkCfg);
   }

   /* return the name list */
   *ifList = head.next;

   return CMSRET_SUCCESS;

} 
#endif  /* DMP_X_BROADCOM_COM_GPONWAN_1 */


#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
CmsRet cmsDal_getAvailableL2EponIntf_igd(NameList **ifList)
{

   NameList head;
   NameList *tail, *nameListElement;
   InstanceIdStack  wanDevIid = EMPTY_INSTANCE_ID_STACK;  
   CmsRet  ret;

   *ifList = NULL;
   head.name = NULL;
   head.next = NULL;
   tail = &head;

   /* Get Epon WanDevice iidStack first */      
   if ((ret = dalEpon_getEponWanIidStatck(&wanDevIid)) != CMSRET_SUCCESS)
   {
      return ret;
   }
   cmsLog_debug("wanDevIid=%s",  cmsMdm_dumpIidStack(&wanDevIid));
#ifdef EPON_SFU
   InstanceIdStack  eponCfgIid = EMPTY_INSTANCE_ID_STACK;   
   WanEponIntfObject *eponCfg = NULL;
   while ((ret = cmsObj_getNextInSubTreeFlags 
      (MDMOID_WAN_EPON_INTF, &wanDevIid,  &eponCfgIid, OGF_NO_VALUE_UPDATE, (void **)&eponCfg)) == CMSRET_SUCCESS)
   {
      cmsLog_debug("found %s, enable=%d", eponCfg->ifName, eponCfg->enable);   
      if (eponCfg->enable == FALSE && eponCfg->ifName != NULL)
      {
         nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
         if (nameListElement == NULL)
         {
            cmsLog_error("could not allocate nameListElement");
            cmsObj_free((void **)&eponCfg);
            cmsDal_freeNameList(head.next);
            return CMSRET_RESOURCE_EXCEEDED;
         }

         /* append to name list */
         cmsLog_debug("adding %s to list", eponCfg->ifName);
         nameListElement->name = cmsMem_strdup(eponCfg->ifName);
         nameListElement->next = NULL;
         tail->next = nameListElement;
         tail = nameListElement;
      }

      cmsObj_free((void **)&eponCfg);
   }
#else
   InstanceIdStack  eponLinkIid = EMPTY_INSTANCE_ID_STACK;   
   WanEponLinkCfgObject *eponLinkCfg = NULL;
   while ((ret = cmsObj_getNextInSubTreeFlags
      (MDMOID_WAN_EPON_LINK_CFG, &wanDevIid,  &eponLinkIid, OGF_NO_VALUE_UPDATE, (void **)&eponLinkCfg)) == CMSRET_SUCCESS)
   {
      cmsLog_debug("found %s, enable=%d", eponLinkCfg->ifName, eponLinkCfg->enable);   
      if (!eponLinkCfg->enable)
      {
         nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
         if (nameListElement == NULL)
         {
            cmsLog_error("could not allocate nameListElement");
            cmsObj_free((void **)&eponLinkCfg);
            cmsDal_freeNameList(head.next);
            return CMSRET_RESOURCE_EXCEEDED;
         }

         /* append to name list */
         cmsLog_debug("adding %s to list", eponLinkCfg->ifName);
         nameListElement->name = cmsMem_strdup(eponLinkCfg->ifName);
         nameListElement->next = NULL;
         tail->next = nameListElement;
         tail = nameListElement;
      }

      cmsObj_free((void **)&eponLinkCfg);
   }
#endif

   /* return the name list */
   *ifList = head.next;

   return CMSRET_SUCCESS;

} 
#endif  /* DMP_X_BROADCOM_COM_EPONWAN_1 */




CmsRet cmsDal_moveIntfLanWan(const char *l2IntfName,
                             UBOOL8 moveToLan, UBOOL8 moveToWan)
{
   InterfaceControlObject *ifcObj = NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Entered: l2IntfName=%s", l2IntfName);

   /*
    * tell the Interface Control object what to do.
    */
   if ((ret = cmsObj_get(MDMOID_INTERFACE_CONTROL, &iidStack, 0, (void **) &ifcObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get InterfaceControlObject, ret=%d", ret);
      return ret;
   }

   CMSMEM_REPLACE_STRING(ifcObj->ifName, l2IntfName);
   ifcObj->moveToLANSide = moveToLan;
   ifcObj->moveToWANSide = moveToWan;

   /* set InterfaceControlObject */
   if ((ret = cmsObj_set(ifcObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set InterfaceControlObject, ret = %d", ret);
   }

   cmsObj_free((void **) &ifcObj);

   return ret;
}




NameList *cmsDal_addNameToNameList(const char *name, NameList **ifList)
{
   NameList *newElement;
   NameList *tmpElement;

   if (IS_EMPTY_STRING(name))
   {
      cmsLog_error("Empty name?!?");
      return NULL;
   }

   if (ifList == NULL)
   {
      cmsLog_error("NULL ifList!");
      return NULL;
   }

   cmsLog_debug("Adding %s to list", name);

   newElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
   if (newElement == NULL)
   {
      cmsLog_error("could not allocate nameListElement");
      return NULL;
   }

   newElement->name = cmsMem_strdup(name);
   if (newElement->name == NULL)
   {
      cmsLog_error("strdup of name %s failed", name);
      cmsMem_free(newElement);
      return NULL;
   }

   /* append new element to the end of the singly linked list */
   if (*ifList == NULL)
   {
      *ifList = newElement;
   }
   else
   {
      tmpElement = *ifList;
      while (tmpElement->next != NULL)
      {
         tmpElement = tmpElement->next;
      }
      /* hit the end of the list */
      tmpElement->next = newElement;
   }

   return newElement;
}




void cmsDal_freeNameList(NameList *nl)
{
   while (nl != NULL)
   {
      NameList *p = nl;

      cmsMem_free(nl->name);
      nl = nl->next;
      cmsMem_free(p);
   }
}  /* End of cmsDal_freeNameList() */



CmsRet cmsDal_getNumUsedQueues(WanLinkType wanType, UINT32 *usedQueues)
{
   return (rutWan_getNumUsedQueues(wanType, usedQueues));
} 

CmsRet cmsDal_getNumUnusedQueues(WanLinkType wanType, UINT32 *unusedQueues)
{
   return (rutWan_getNumUnusedQueues(wanType, unusedQueues));
      
}

#ifdef DMP_X_BROADCOM_COM_SNMP_1
CmsRet cmsDal_getCurrentSnmpCfg(WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   SnmpCfgObject *snmpCfg=NULL;
   CmsRet ret;

   cmsLog_debug("Entered");

   if ((ret = cmsObj_get(MDMOID_SNMP_CFG, &iidStack, 0, (void *) &snmpCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get SNMP_CFG, ret=%d", ret);
      return ret;
   }

   /* if status is the DISABLED string, then strcmp returns 0, which means status=0/off */
   webVar->snmpStatus = cmsUtl_strcmp(snmpCfg->status, MDMVS_DISABLED);

   if (snmpCfg->ROCommunity)
   {
   strcpy(webVar->snmpRoCommunity,snmpCfg->ROCommunity);
   }

   if (snmpCfg->RWCommunity)
   {
   strcpy(webVar->snmpRwCommunity,snmpCfg->RWCommunity);
   }

   if (snmpCfg->sysName)
   {
   strcpy(webVar->snmpSysName,snmpCfg->sysName);
   }

   if (snmpCfg->sysLocation)
   {
   strcpy(webVar->snmpSysLocation,snmpCfg->sysLocation);
   }

   if (snmpCfg->sysContact)
   {
   strcpy(webVar->snmpSysContact,snmpCfg->sysContact);
   }

   if (snmpCfg->trapIPAddress)
   {
   strcpy(webVar->snmpTrapIp,snmpCfg->trapIPAddress);
   }

   cmsObj_free((void **) &snmpCfg);
   return ret;
}

CmsRet cmsDal_setSnmpCfg(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   SnmpCfgObject *snmpCfg=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Entered");

   if ((ret = cmsObj_get(MDMOID_SNMP_CFG, &iidStack, 0, (void *) &snmpCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of SNMP_CFG failed, ret=%d", ret);
      return ret;
   }

   if (webVar->snmpStatus)
   {
      CMSMEM_REPLACE_STRING(snmpCfg->status, MDMVS_ENABLED);
   }
   else
   {
      CMSMEM_REPLACE_STRING(snmpCfg->status, MDMVS_DISABLED);
   }
   CMSMEM_REPLACE_STRING(snmpCfg->ROCommunity, webVar->snmpRoCommunity);
   CMSMEM_REPLACE_STRING(snmpCfg->RWCommunity, webVar->snmpRwCommunity);
   CMSMEM_REPLACE_STRING(snmpCfg->sysName, webVar->snmpSysName);
   CMSMEM_REPLACE_STRING(snmpCfg->sysLocation, webVar->snmpSysLocation);
   CMSMEM_REPLACE_STRING(snmpCfg->sysContact, webVar->snmpSysContact);
   CMSMEM_REPLACE_STRING(snmpCfg->trapIPAddress, webVar->snmpTrapIp);

   cmsLog_debug("snmpCfg->sysContact %s",snmpCfg->sysContact);

   ret = cmsObj_set(snmpCfg, &iidStack);
   cmsObj_free((void **) &snmpCfg);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of SNMP_CFG failed, ret=%d", ret);
   }
   return ret;
}

#endif /* DMP_X_BROADCOM_COM_SNMP_1 */

#ifdef DMP_X_BROADCOM_COM_DBUSREMOTE_1  
void cmsDal_getDbusRemoteCfg(WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   DbusRemoteCfgObject *dbusCfg=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_get(MDMOID_DBUS_REMOTE_CFG, &iidStack, 0, (void *) &dbusCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of DBUS_CFG failed, ret=%d", ret);
      return;
   }

   webVar->dbusRemoteEnable = dbusCfg->enable;
   webVar->dbusPort = dbusCfg->port;

   cmsObj_free((void **) &dbusCfg);
}

CmsRet cmsDal_setDbusremoteCfg(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   DbusRemoteCfgObject *dbusCfg=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Entered cmsDal_setDbusremoteCfg");

   if ((ret = cmsObj_get(MDMOID_DBUS_REMOTE_CFG, &iidStack, 0, (void *) &dbusCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of DBUS_CFG failed, ret=%d", ret);
      return ret;
   }

   dbusCfg->enable = webVar->dbusRemoteEnable;
   dbusCfg->port = webVar->dbusPort;

   cmsLog_debug("dbusCfg->enable=%d, port=%d", dbusCfg->enable, dbusCfg->port);

   ret = cmsObj_set(dbusCfg, &iidStack);
   cmsObj_free((void **) &dbusCfg);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of DBUS_CFG failed, ret=%d", ret);
   }

   return ret;
}
#endif /* DMP_X_BROADCOM_COM_DBUSREMOTE_1 */

#ifdef DMP_X_BROADCOM_COM_EPON_1
void cmsDal_getOamLoidPwCfg(WEB_NTWK_VAR *webVar)
{
   XponObject *obj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   UINT8 len;

   memset(webVar->oamAuthLoid, 0, sizeof(webVar->oamAuthLoid));     
   memset(webVar->oamAuthPass, 0, sizeof(webVar->oamAuthPass)); 

   if ((ret = 
    cmsObj_get(MDMOID_XPON, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
       cmsLog_error("get of MDMOID_XPON object failed, ret=%d", ret);
       return;
   }
   
   len = strlen(obj->authenticationId);
   memcpy(webVar->oamAuthLoid, obj->authenticationId, 
      (len > MaxLoidLen)? MaxLoidLen : len );

   len = strlen(obj->authenticationPassword);
   memcpy(webVar->oamAuthPass, obj->authenticationPassword, 
      (len > MaxAuthPassLen)? MaxAuthPassLen : len );
   
   cmsObj_free((void **) &obj);
} 


CmsRet cmsDal_setEponLoidPw(char *eponLoid, char *eponLoidPassword)
{
   CmsRet ret = CMSRET_SUCCESS;
   XponObject *obj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UINT8 len;

   if ((ret = 
    cmsObj_get(MDMOID_XPON, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
       cmsLog_error("get of MDMOID_XPON object failed, ret=%d", ret);
       return ret;
   }
   
   len = strlen(eponLoid);
   CMSMEM_FREE_BUF_AND_NULL_PTR(obj->authenticationId);
   obj->authenticationId = cmsMem_alloc(len+1, ALLOC_ZEROIZE);
   memcpy(obj->authenticationId, eponLoid, len);

   len = strlen(eponLoidPassword);
   CMSMEM_FREE_BUF_AND_NULL_PTR(obj->authenticationPassword);
   obj->authenticationPassword = cmsMem_alloc(len+1, ALLOC_ZEROIZE);
   memcpy(obj->authenticationPassword, eponLoidPassword, len);   

   ret = cmsObj_set(obj, &iidStack);
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of MDMOID_XPON object failed, ret=%d", ret);
      cmsObj_free((void **) &obj);
      return ret;
   }

   ret = cmsMgm_saveConfigToFlash();
   if (ret != CMSRET_SUCCESS)
   {
       printf("Could not epon loid to flash, ret=%d\n", ret);
   }
   
   cmsObj_free((void **) &obj);
   
   return ret;
}
#endif

#ifdef SUPPORT_STUN
void getStunCfg(WEB_NTWK_VAR *webVar)
{
   /* update global web var with STUN configuration */
   CmsRet ret = dalStun_getStunCfg(webVar);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get STUN DAL object");
   }
}
#endif
