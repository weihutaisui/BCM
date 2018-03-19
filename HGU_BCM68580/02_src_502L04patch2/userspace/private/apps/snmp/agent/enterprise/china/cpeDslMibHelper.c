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
#include <string.h>
#include <net/route.h>
#include <signal.h>
#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "asn1.h"
#include "snmp.h"
#include "agtMib.h"
#include "agtEngine.h"
#include "cpeDslMib.h"
#include "cpeDslMibHelper.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

#ifdef LGD_TO_DO
#include "psidefs.h"
#include "psiapi.h"
#include "AdslMibDef.h"
#include "syscall.h"
#include "board_api.h"
#include "sysdiag.h"
#include "adslctlapi.h"
#include "ifcdefs.h"
#include "dbapi.h"
#include "snmpstruc.h"
#include "ifcrtwanapi.h"
#include "atmMibDefs.h"
#include "atmMibHelper.h"
#include "pmapdefs.h"
#else
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#endif

#ifdef BUILD_SNMP_DEBUG
#include "snmpDebug.h"
extern SNMP_DEBUG_FLAG snmpDebug;
#endif

#ifdef LGD_TO_DO
extern pCPE_SYS_INFO pSysInfo;
extern pWAN_INFO_LIST pWanList;
extern pPPP_INFO_LIST pPppList;
extern pCPE_IP_TABLE_LIST pIpList;
extern pCPE_PING_INFO pPingData;
extern pCPE_FTP_INFO pFtpData;
extern pCPE_PVC_ENTRY pPvcParam;
extern pPPP_REFERENCE_LIST pPppReferenceList;
extern pCPE_PPPTEST_INFO pPppTestData;
#else
extern pCPE_FTP_INFO pFtpData;
#endif

#ifdef LGD_TO_DO
extern int setAdslLineOperation;
extern int modTypeAdslLineHandshake;
extern int modTypeAdslLineTrellis;
extern int modTypeAdslLineBitSwapping;
#ifdef CT_SNMP_MIB_VERSION_2

extern pCPE_SERVICETYPE_TABLE_LIST pServiceTypeList;
extern pCPE_ETHERNETPORT_TABLE_LIST pEthernetPortList;
extern ATM_INFO_LIST atmList;

typedef struct cpe_pvc_eth_portmap_nobind 
{
  int vpi;
  int vci;
  int ethnetportmap; 
  int inused;
} CPE_PVC_ETH_PORTMAP_NOBIND, *pCPE_PVC_ETH_PORTMAP_NOBIND;

/*Here is a temp solution, it is very stupid, in the feature if CT use the MIB 2, here need to improve*/

CPE_PVC_ETH_PORTMAP_NOBIND cpePvcEthPortMapNobind[8];

#define RESERVE_PVC_0_16_VPI    0
#define RESERVE_PVC_0_16_VCI    16
#define RESERVE_PVC_1_39_VPI    1
#define RESERVE_PVC_1_39_VCI    39

#endif
// define in ifcwanapi.cpp
extern UINT16 BcmWan_getStatus(PWAN_CON_ID pWanId);
extern void BcmMer_init(PWAN_CON_ID pWanId);

int cpeGetAdslLineModType(void)
{
  PSI_STATUS status;
  long modType = kAdslCfgModAny;
  int adslConfig = CPE_ADSL_AUTOSENSE;
  UINT16 retLen = 0;
  PSI_HANDLE adslPsi = BcmPsi_appOpen(ADSL_APPID);
  
  status = BcmPsi_objRetrieve(adslPsi, ADSL_CONFIG_FLAG, &adslConfig, sizeof(unsigned int), &retLen);
  if (status != PSI_STS_OK) {
    adslConfig = 0xff;
  }
  if (adslConfig&ANNEX_A_MODE_GDMT)
    modType |= kAdslCfgModGdmtOnly;
  if (adslConfig&ANNEX_A_MODE_GLITE)
    modType |= kAdslCfgModGliteOnly;
  if (adslConfig&ANNEX_A_MODE_T1413)
    modType |= kAdslCfgModT1413Only;
  if (adslConfig&ANNEX_A_MODE_ADSL2)
    modType |= kAdslCfgModAdsl2Only;
  if (adslConfig&ANNEX_A_MODE_ANNEXL)
    modType |= kAdslCfgModAdsl2pOnly | kAdslCfgModAdsl2Only;

  switch (modType)
    {
    case (kAdslCfgModGdmtOnly | kAdslCfgModGliteOnly):
      adslConfig = CPE_ADSL_GDMT_OR_LITE;
      break;
    case kAdslCfgModT1413Only:
      adslConfig = CPE_ADSL_T1413;
      break;
    case kAdslCfgModGdmtOnly:
      adslConfig = CPE_ADSL_GDMT;
      break;
    case kAdslCfgModGliteOnly:
      adslConfig = CPE_ADSL_GLITE;
      break;
    default:
      adslConfig = CPE_ADSL_AUTOSENSE;
      break;
    }
  return adslConfig;
}

int cpeGetStoredSnmpParam(pCPE_SYS_INFO pSysinfo)
{
  BCM_SNMP_STRUCT snmpInfo;
  char tmpStr[3];

  BcmDb_getSnmpInfo(&snmpInfo);
  pSysInfo->trap = snmpInfo.trapState;
  cmsUtl_strncpy(pSysInfo->prodId, snmpInfo.prodId, CPE_LEN_32);
  cmsUtl_strncpy(pSysInfo->vendorId, snmpInfo.vendorId, CPE_LEN_32);
  cmsUtl_strncpy(pSysInfo->serialNum, snmpInfo.serialNum, CPE_LEN_32);
  cmsUtl_strncpy(pSysInfo->sysVersion, snmpInfo.sysVersion, CPE_LEN_64);

  tmpStr[2] = '\0';
  cmsUtl_strncpy(tmpStr, snmpInfo.oui, 2);
  pSysInfo->oui[0] = (char) strtol(tmpStr, (char **)NULL, 16);
  cmsUtl_strncpy(tmpStr, (snmpInfo.oui+2), 2);
  pSysInfo->oui[1] = (char) strtol(tmpStr, (char **)NULL, 16);
  cmsUtl_strncpy(tmpStr, (snmpInfo.oui+4), 2);
  pSysInfo->oui[2] = (char) strtol(tmpStr, (char **)NULL, 16);
  
  return 0;
}

void cpeReboot()
{
  sysMipsSoftReset();
}

void cpeSave()
{
#ifdef BUILD_SNMP_DEBUG
  printf("cpeSave: generate new configId, then save configuration... \n");
#endif

#if 0
  /* do not generate config ID unless configuration is saved from LAN */
  /* generate and save new configIf */
  cpeGetConfigId(configId, 1);
#endif

  printf("calling BcmPsi_flush to save the profile\n");
  BcmPsi_flush();
}

void cpeRestoreToNull()
{
  /* whether this is restore to null or default, it's the same right now */
  resetRunningConfig(CPE_SYSTEM_RESTORE_TO_NULL);
}

void cpeRestoreToDefault()
{
  /* whether this is restore to null or default, it's the same right now */
  resetRunningConfig(CPE_SYSTEM_RESTORE_TO_DEFAULT);
}


/* return -1 if there is error */
int cpeSaveTrap(int trapMode)
{
#ifdef TO_BE_DONE
  /* save trapMode to PSI */
#else
  return CPE_AGENT_OK;
#endif

}

/* mode = 1 is on, otherwise off */
int cpeSetAdslTrellisMode(int mode)
{
  int oldMode;

  mode=modTypeAdslLineTrellis;
  setAdslLineOperation=0;      
  signal(SIGALRM,SIG_IGN);
  
  oldMode = getAdslLineTrellisMode();
  /* Adsl driver: 1 = on, 0 = off; SNMP: 1= on, 2 = off */
  if (mode == CPE_ADMIN_DISABLE)
    mode = 0;

  if (oldMode == mode)
    return CPE_AGENT_OK;

  if (mode)
    prctl_runCommandInShellWithTimeout("adslctl configure --trellis on");
  else
    prctl_runCommandInShellWithTimeout("adslctl configure --trellis off");

  return CPE_AGENT_OK;
}

int cpeConvertAdslModLineType(int snmpVar) 
{
  int modType=0;

  switch (snmpVar)
    {
    case CPE_ADSL_GDMT_OR_LITE:
      modType = (ANNEX_A_MODE_GDMT | ANNEX_A_MODE_GLITE);
      break;
    case CPE_ADSL_T1413:
    case CPE_ADSL_T1413_FIRST:
      modType = ANNEX_A_MODE_T1413;
      break;
    case CPE_ADSL_GDMT:
    case CPE_ADSL_GDMT_FIRST:
      modType = ANNEX_A_MODE_GDMT;
      break;
    case CPE_ADSL_GLITE:
      modType = ANNEX_A_MODE_GLITE;
      break;
    default:
      modType = ANNEX_A_MODE_ALL_MOD;
      break;
    }
  return (modType);
}
void cpeSetAdslDefaultValues(int *pAdslFlag)
{
  unsigned int chipId;
  chipId = (unsigned int)sysGetChipId();

#ifdef ANNEX_C
  *pAdslFlag = ANNEX_A_LINE_PAIR_INNER | ANNEX_C_BITMAP_DBM | ANNEX_A_MODE_GDMT | \
               ANNEX_A_MODE_GLITE;
#else  // ANNEX_A
  *pAdslFlag = ANNEX_A_LINE_PAIR_INNER | ANNEX_A_BITSWAP_ENABLE | ANNEX_A_MODE_ALL_MOD;
  if ( chipId != 0x6345)
    *pAdslFlag |= ANNEX_A_MODE_ALL_MOD_48;
#endif
}

/* mode = ANNEX_A_MODE_GDMT, ANNEX_A_MODE_GLITE,... */
int cpeSetAdslModulationMode(int mode)
{
  PSI_HANDLE adslPsi = BcmPsi_appOpen(ADSL_APPID);
  int adslFlag=0;
  UINT16 retLen = 0;
  unsigned int chipId;

  char options[SYS_CMD_LEN];
  
  mode=modTypeAdslLineHandshake;
  setAdslLineOperation=0;
  signal(SIGALRM,SIG_IGN);
 
  chipId = (unsigned int)sysGetChipId();
  BcmPsi_objRetrieve(adslPsi, ADSL_CONFIG_FLAG, &adslFlag, sizeof(adslFlag), &retLen);

  if (retLen == 0)
    cpeSetAdslDefaultValues(&adslFlag);

  /* clear current modulation configuration bits for new ones */
  if (chipId != 0x6345)
    adslFlag &=  (~ANNEX_A_MODE_ALL_MOD_48);
  else
    adslFlag &= (~ANNEX_A_MODE_ALL_MOD);

  strcpy(options, "adsl configure --mod ");
  switch (mode)
    {
    case (ANNEX_A_MODE_GDMT | ANNEX_A_MODE_GLITE):
      if ((adslFlag & ANNEX_A_MODE_GDMT) && (adslFlag & ANNEX_A_MODE_GLITE))
        return CPE_AGENT_OK;
      adslFlag |= (ANNEX_A_MODE_GDMT | ANNEX_A_MODE_GLITE);
#ifdef ANNEX_C  
      strcat(options, "a");
#else
      strcat(options, "dl");
#endif
      break;
    case ANNEX_A_MODE_T1413:
      if ((adslFlag & ANNEX_A_MODE_T1413) == ANNEX_A_MODE_T1413)
        return CPE_AGENT_OK;
      adslFlag |= ANNEX_A_MODE_T1413;
      strcat(options, "t");
      break;
    case ANNEX_A_MODE_GDMT:
      if ((adslFlag & ANNEX_A_MODE_GDMT) == ANNEX_A_MODE_GDMT)
        return CPE_AGENT_OK;
      adslFlag |= ANNEX_A_MODE_GDMT;
      strcat(options, "d");
      break;
    case ANNEX_A_MODE_GLITE:
      if ((adslFlag & ANNEX_A_MODE_GLITE) == ANNEX_A_MODE_GLITE)
        return CPE_AGENT_OK;
      adslFlag |= ANNEX_A_MODE_GLITE;
      strcat(options, "l");
      break;
    default:
      if (chipId != 0x6345) {
        if ((adslFlag &  ANNEX_A_MODE_ALL_MOD_48) == ANNEX_A_MODE_ALL_MOD_48)
          return CPE_AGENT_OK;
        else
          adslFlag |= ANNEX_A_MODE_ALL_MOD_48;
      }
      else {
        if ((adslFlag & ANNEX_A_MODE_ALL_MOD) == ANNEX_A_MODE_ALL_MOD)
          return CPE_AGENT_OK;
        else
          adslFlag |= ANNEX_A_MODE_ALL_MOD;
      }
      strcat(options, "a");
    } /* switch */

  prctl_runCommandInShellWithTimeout(options);

  BcmPsi_objStore(adslPsi, ADSL_CONFIG_FLAG, &adslFlag, sizeof(adslFlag));
  return CPE_AGENT_OK;
}

/* mode = 1 is on, otherwise off */
int cpeSetAdslBitSwapMode(int mode)
{
  PSI_HANDLE adslPsi = BcmPsi_appOpen(ADSL_APPID);
  unsigned int adslFlag=0;
  UINT16 retLen = 0;
  int oldMode = 0;

  mode=modTypeAdslLineBitSwapping;
  setAdslLineOperation=0;
  signal(SIGALRM,SIG_IGN);
 
  BcmPsi_objRetrieve(adslPsi, ADSL_CONFIG_FLAG, &adslFlag, sizeof(ADSL_CONFIG_FLAG), &retLen);

  if (retLen == 0) 
    cpeSetAdslDefaultValues(&adslFlag);

  if (adslFlag & ANNEX_A_BITSWAP_DISENABLE)
    oldMode = 0;
  else 
    oldMode = 1;
  if (mode == oldMode)
    return CPE_AGENT_OK; /* same, do nothing */

  if (mode) {
    prctl_runCommandInShellWithTimeout("adsl configure --bitswap on");
    adslFlag &= (~ANNEX_A_BITSWAP_DISENABLE);
  }
  else {
    prctl_runCommandInShellWithTimeout("adsl configure --bitswap off");
    adslFlag |= ANNEX_A_BITSWAP_DISENABLE;
  }
  BcmPsi_objStore(adslPsi, ADSL_CONFIG_FLAG, &adslFlag, sizeof(adslFlag));
  return CPE_AGENT_OK;
}

int cpeGetAdslBitSwapMode()
{
  PSI_HANDLE adslPsi = BcmPsi_appOpen(ADSL_APPID);
  unsigned int adslFlag=ANNEX_A_BITSWAP_ENABLE;
  UINT16 retLen = 0;

  BcmPsi_objRetrieve(adslPsi, ADSL_CONFIG_FLAG, &adslFlag, sizeof(ADSL_CONFIG_FLAG), &retLen);

  if (retLen == 0) 
    cpeSetAdslDefaultValues(&adslFlag);
    
  if (adslFlag & ANNEX_A_BITSWAP_DISENABLE)
    return 0;
  else 
    return 1;
}

void addWanInfoToList(pCPE_PVC_ENTRY p)
{
  pCPE_PVC_ENTRY ptr, ptrNext, ptrHead;

  /* add to list, order is sorted by ifIndex/vpi/vci */
  if (pWanList->head == NULL) {
    pWanList->head = p;
    pWanList->tail = p;
  }
  else {
    ptrHead = ptr = pWanList->head;
    ptrNext = ptr->next;
    while (ptr != NULL) {
      if ((ptrNext) &&
          ((p->ifIndex > ptrNext->ifIndex) || (p->vpi > ptrNext->vpi) ||
           ((p->vpi == ptrNext->vpi) && (p->vci > ptrNext->vci)))) {
        /* move on */
        ptrHead = ptr;
        ptr = ptr->next;
        if (ptr != NULL)
          ptrNext = ptr->next;
        else
          ptrNext = NULL;
      }
      else {
        if ((ptr->ifIndex > p->ifIndex) || (ptr->vpi > p->vpi) ||
            ((ptr->vpi == p->vpi) && (ptr->vci > p->vci))) {
          p->next = ptr;
          //ptrHead = p;
          pWanList->head = p;
          break;
        } /* head */
        else if ((ptrNext == NULL) && 
                 ((ptr->vpi < p->vpi) || 
                  ((ptr->vpi == p->vpi) && (ptr->vci < p->vci)))) {
          ptr->next = p;
          pWanList->tail = p;
          break;
        } /* end */
        else {
          p->next = ptrNext;
          ptr->next = p;
          break;
        } /* middle */
      } /* else, do the adding */
    } /* while */
  } /* list != NULL */
  pWanList->count++;
}

void cpeFreeWanInfo(void) {
  pCPE_PVC_ENTRY ptr;

  while (pWanList->head) {
    ptr = pWanList->head;
    pWanList->head = pWanList->head->next;
    free(ptr);
  }
  pWanList->count = 0;
}

void cpeWanInfoDefault(pCPE_PVC_ENTRY pEntry)
{
  memset(pEntry,0,sizeof(CPE_PVC_ENTRY));
  pEntry->adminStatus = CPE_ADMIN_ENABLE;
  pEntry->operStatus = CPE_ADMIN_ENABLE;
  pEntry->bridgeMode = CPE_ADMIN_ENABLE;
  pEntry->macLearnMode = CPE_ADMIN_ENABLE;
}

void cpeScanWanInfo(void)
{
  WAN_CON_ID wanId;  /* vpi.vci.connId */
  WAN_CON_INFO wanInfo;
  pCPE_PVC_ENTRY pEntry;
  WAN_IP_INFO ipInfo;
  char ifName[IFC_TINY_LEN];
  
#ifdef CT_SNMP_MIB_VERSION_2
  ATM_VCL_TABLE *pVcc;
#endif

  cpeFreeWanInfo();

  memset(&wanId,0,sizeof(wanId));
  while (BcmDb_getWanInfoNext(&wanId,&wanInfo) != DB_WAN_GET_NOT_FOUND) {
    if ((pEntry = (pCPE_PVC_ENTRY)malloc(sizeof(CPE_PVC_ENTRY))) != NULL) {
      cpeWanInfoDefault(pEntry);
      pEntry->vpi = wanId.vpi;
      pEntry->vci = wanId.vci;
      pEntry->pvcId = wanInfo.vccId; 
      pEntry->conId = wanId.conId;
      pEntry->ifIndex = ifGetIfIndexByName(ATM_CELL_LAYER_IFNAME);
      sprintf(ifName,"nas_%d_%d_%d",wanId.portId,wanId.vpi,wanId.vci);
      pEntry->pvcIfIndex = ifGetIfIndexByName(ifName);

      switch (wanInfo.protocol)
        {
        case PROTO_PPPOE:
          pEntry->upperEncap = CPE_PVC_ENCAP_PPPOE;
          break;
        case PROTO_PPPOA:
          pEntry->upperEncap = CPE_PVC_ENCAP_PPPOA;
          break;
        case PROTO_MER:
          pEntry->upperEncap = CPE_PVC_ENCAP_BRIDGE2684IP;
          break;
        case PROTO_BRIDGE:
          pEntry->upperEncap = CPE_PVC_ENCAP_BRIDGE2684;
          break;
        case PROTO_IPOA:
          pEntry->upperEncap = CPE_PVC_ENCAP_ROUTER2684;
          break;
        default:
          break;
        } /* switch */
      if (wanInfo.flag.service)
        pEntry->adminStatus = CPE_ADMIN_ENABLE; 
      else 
        pEntry->adminStatus = CPE_ADMIN_DISABLE; 
      if (wanInfo.flag.igmp)
        pEntry->igmpMode = CPE_ADMIN_ENABLE;
      else
        pEntry->igmpMode = CPE_ADMIN_DISABLE;
      if (wanInfo.flag.nat)
        pEntry->natMode = CPE_ADMIN_ENABLE;
      else
        pEntry->natMode = CPE_ADMIN_DISABLE;
      if (BcmWan_getStatus(&wanId) == IFC_STATUS_UP)
        pEntry->operStatus = CPE_ADMIN_UP;
      else
        pEntry->operStatus = CPE_ADMIN_DOWN;
      if (wanInfo.flag.bridgeMode)
        pEntry->bridgeMode = CPE_ADMIN_ENABLE; 
      else 
        pEntry->bridgeMode = CPE_ADMIN_DISABLE; 
      if (wanInfo.flag.macLearnMode)
        pEntry->macLearnMode = CPE_ADMIN_ENABLE; 
      else 
        pEntry->macLearnMode = CPE_ADMIN_DISABLE; 
      if (wanInfo.flag.bridgeMode)
        pEntry->bridgeMode = CPE_ADMIN_ENABLE; 
      else 
        pEntry->bridgeMode = CPE_ADMIN_DISABLE; 
      if (wanInfo.flag.macLearnMode)
        pEntry->macLearnMode = CPE_ADMIN_ENABLE; 
      else 
        pEntry->macLearnMode = CPE_ADMIN_DISABLE;
      BcmDb_getIpSrvInfo(&wanId, &ipInfo);
      if (ipInfo.enblDhcpClnt)
        pEntry->dhcpcMode = CPE_ADMIN_ENABLE;
      else
        pEntry->dhcpcMode = CPE_ADMIN_DISABLE;
#ifdef CT_SNMP_MIB_VERSION_2
/********************************************************************************/
        pEntry->bindingMode = nmsGetPvcBindMode(pEntry->vpi,pEntry->vci);
/********************************************************************************/
        if(pEntry->bindingMode == CPE_ETHERNETPORT_BINDING_MODE)
        {
            pEntry->ethernetPortMap = nmsGetPvcEthernetPortMap(pEntry->vpi,pEntry->vci);
        }
        else if(pEntry->bindingMode == CPE_ETHERNETPORT_NO_BINDING_MODE)
        {
            pEntry->ethernetPortMap = nmsGetPvcEthernetPortMapNoBindMode(pEntry->vpi,pEntry->vci);
        }
        else
        {
            pEntry->ethernetPortMap = 0;
        }
/**********************************************************************************/
        if ( atmMibHelper_fillAtmVclTable() != 0 ) 
        { 
            pEntry->serviceTypeIndex = 0;
        }
        else
        {
            pVcc = atmList.head;
            while (pVcc) 
            {
                if((pVcc->atmVclVpi==pEntry->vpi)&&(pVcc->atmVclVci==pEntry->vci))
                    break; /* found */
                pVcc = pVcc->next;
            }
            if (pVcc == NULL) 
            {
                pEntry->serviceTypeIndex = 0;
            }
            else
            {
                pEntry->serviceTypeIndex = pVcc->atmVclTransmitTrafficDescrIndex;
            }
        }
/**************************************************************************************/        
#endif

    } /* pEntry != NULL */

    addWanInfoToList(pEntry);
  } /* while */

#ifdef BUILD_SNMP_DEBUG
  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
    if (pWanList->count != 0) {
      printf("************---wanList.count %d---************\n",
             pWanList->count);
      pEntry = pWanList->head;
      while (pEntry) {
        printf("vpi/vci %d/%d\n",pEntry->vpi,pEntry->vci);
        pEntry = pEntry->next;
      }
    }
  }
#endif /* BUILD_SNMP_DEBUG */
}

int cpeAddWanInfo(int vpi, int vci)
{
  WAN_CON_ID wanId;
  IFC_ATM_VCC_INFO vccInfo;
  WAN_CON_INFO wanInfo;

  memset(&wanInfo,0,sizeof(WAN_CON_INFO));

  wanId.vpi = vpi;
  wanId.vci = vci;
  wanId.conId = 0;
  // if there is no available wanId then return error
  if ( BcmDb_getAvailableWanConId(&wanId) == DB_WAN_ADD_OUT_OF_CONNECTION ) {
     printf("Failed to get AvailableWanConId\n");
     return -1;
  }
  wanInfo.conId = wanId.conId;
  vccInfo.vccAddr.ulInterfaceId = ATM_LOGICAL_PORT_NUM;
  vccInfo.vccAddr.usVpi = vpi;
  vccInfo.vccAddr.usVci = vci;
#if 0
  wanInfo.vccId = BcmDb_getVccId(&(vccInfo.vccAddr));
#else
  extern UINT16     AtmVcc_findByAddress(const PATM_VCC_ADDR pVccAddr);
  
  wanInfo.vccId = AtmVcc_findByAddress(&(vccInfo.vccAddr));
#endif

  wanInfo.protocol = PROTO_BRIDGE;
  wanInfo.encapMode = 0; /* LLC */
  wanInfo.flag.igmp = CPE_ADMIN_ENABLE;
  wanInfo.flag.nat = CPE_ADMIN_DISABLE;
  wanInfo.flag.service = CPE_ADMIN_ENABLE;
  sprintf(wanInfo.conName, "bridge_%d_%d", vpi, vci);
  wanInfo.flag.firewall = FALSE;
  wanInfo.flag.nat = FALSE;
  wanInfo.flag.igmp = FALSE;
  wanInfo.vlanId = -1;
  // store WAN connection info
  BcmDb_setWanInfo(&wanId, &wanInfo);
 
  return 0;
}

int cpeConfigWanInfo() {
  WAN_CON_ID wanId;  /* vpi.vci.connId */
  int mode = 1;
  int vpi, vci;

  int ret = 0;
  int encapMode = 0;
  ATM_VCC_ADDR addr;
  ATM_VCC_CFG   atmcfg;
  
  if (pPvcParam == NULL)
    return -1;

  vpi = pPvcParam->vpi;
  vci = pPvcParam->vci;

#ifdef BUILD_SNMP_DEBUG
  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE)
    printf("cpeConfigWanInfo(vpi/vci %d/%d)\n",
           vpi,vci);
#endif  

  wanId.vpi = vpi;
  wanId.vci = vci;
  wanId.conId = pPvcParam->conId;
#ifdef BUILD_SNMP_DEBUG
  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
    printf("====>  cpeConfigWanInfo 1, vpi=%d, vci=%d, pPvcParam->upperEncap=%d, pPvcParam->conId=%d, macLearn %d, igmp %d, dhcpc %d, nat %d\n",
           vpi, vci, pPvcParam->upperEncap, pPvcParam->conId,
	   pPvcParam->macLearnMode,pPvcParam->igmpMode,pPvcParam->dhcpcMode,pPvcParam->natMode); 
  }  
#endif
  switch (pPvcParam->upperEncap) 
    {
    case CPE_PVC_ENCAP_PPPOE:
      mode = PROTO_PPPOE;
#ifdef BUILD_SNMP_DEBUG
      if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
        printf("====>  cpeConfigWanInfo mode = PROTO_PPPOE: NMSbridgeMode %d\n",pPvcParam->bridgeMode); 
      }
#endif
      break;
    case CPE_PVC_ENCAP_PPPOA:
      mode = PROTO_PPPOA;
      break;
    case CPE_PVC_ENCAP_BRIDGE2684IP:
      mode = PROTO_MER;
#ifdef BUILD_SNMP_DEBUG
      if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
        printf("====>  cpeConfigWanInfo mode = PROTO_MER\n"); 
      }
#endif
      break;
    case CPE_PVC_ENCAP_BRIDGE2684:
      mode = PROTO_BRIDGE;
#ifdef BUILD_SNMP_DEBUG
      if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
        printf("====>  cpeConfigWanInfo mode = PROTO_BRIDGE\n"); 
      }
#endif
      break;
    case CPE_PVC_ENCAP_ROUTER2684:
      mode = PROTO_IPOA;
      break;
    }
#ifdef BUILD_SNMP_DEBUG
  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
    printf("encap mode is mode %d, PROTO_MER %d, PROTO_BRIDGE %d\n",
           mode,PROTO_MER,PROTO_BRIDGE);
  }
#endif

  if (BcmDb_configWanMode(&wanId, WAN_PROTOCOL_FIELD, mode) != 0)
    goto cpeConfigWanFail;

  // for now if bridge, kill dhcpd ???
  if (mode == PROTO_BRIDGE)  {
    char cmd[IFC_LARGE_LEN];
    int pid = 0;          
    // kill old dhcp server before start a new one
    if ( (pid = bcmGetPid("dhcpd")) > 0 ) {
      sprintf(cmd, "kill -9 %d", pid);
      prctl_runCommandInShellWithTimeout(cmd);
    }
  }

  memset(&atmcfg, 0, sizeof(atmcfg));
  atmcfg.ulStructureId = ID_ATM_VCC_CFG;
  addr.ulInterfaceId = 0;
  addr.usVpi = vpi;
  addr.usVci = vci;

  ret = (int)BcmAtm_GetVccCfg(&addr, &atmcfg);

  if(0 == ret)
  {
      encapMode = atmcfg.u.Aal5Cfg.ulAtmVccEncapsType;
      if(pPvcParam->upperEncap == PROTO_PPPOA)
      {
           if(encapMode == ET_LLC_ENCAPSULATION)
                encapMode = 1;
           else if(encapMode == ET_VC_MULTIPLEX_ROUTED_PROTOCOL)
                encapMode = 0;
	    else if(encapMode ==ET_VC_MULTIPLEX_BRG_PROTOCOL_8023)
		  encapMode = 0;
           else
                encapMode = 0;
      }
      else
      {
            if(encapMode == ET_LLC_ENCAPSULATION)
                encapMode = 0;
            else if(encapMode == ET_VC_MULTIPLEX_ROUTED_PROTOCOL)
                encapMode = 1;
	     else if(encapMode == ET_VC_MULTIPLEX_BRG_PROTOCOL_8023)
		  encapMode = 1;
            else
                encapMode = 1;
      }
      if(BcmDb_configWanMode(&wanId,WAN_ENCAP_FIELD,encapMode) !=0)
            goto cpeConfigWanFail;
  }

    /* do the conversion between SNMP admin enable and CFM enable/disable (1/0) */
  if ( pPvcParam->adminStatus == CPE_ADMIN_ENABLE)
    mode = 1;
  else 
    mode = 0;
  if (BcmDb_configWanMode(&wanId, WAN_ADMIN_FIELD, mode) != 0)
    goto cpeConfigWanFail;


#if 0
  /* make sure the mode has been changed before restarting or stopping again */
  if (mode) {/* admin UP */
    BcmNtwk_startWan();
  }
  else {
    //BcmNtwk_stopWan();
  }
#endif

  if ( pPvcParam->bridgeMode == CPE_ADMIN_ENABLE)
    mode = 1;
  else 
    mode = 0;

  if (BcmDb_configWanMode(&wanId, WAN_BRIDGE_FIELD, mode) != 0)
    goto cpeConfigWanFail;

  if ( pPvcParam->macLearnMode == CPE_ADMIN_ENABLE)
    mode = 1;
  else 
    mode = 0;
  if (BcmDb_configWanMode(&wanId, WAN_MAC_LEARNING_FIELD, mode) != 0)
    goto cpeConfigWanFail;

  if ( pPvcParam->igmpMode == CPE_ADMIN_ENABLE)
    mode = 1;
  else 
    mode = 0;
  if (BcmDb_configWanMode(&wanId, WAN_IGMP_FIELD, mode) != 0)
    goto cpeConfigWanFail;

  if ( pPvcParam->dhcpcMode == CPE_ADMIN_ENABLE)
    mode = 1;
  else 
    mode = 0;
#ifdef CPE_DSL_MIB
  /* Huawei NMS send command to disable dhcpc for bridge only PVC;
     this causes error since PSI doesn't have associate dhcpc field
     for bridge PVC mode.   Do not check for error, just ignore this command
     if error occurs */
  BcmDb_configWanMode(&wanId, WAN_DHCPC_FIELD, mode);
#else
  if (BcmDb_configWanMode(&wanId, WAN_DHCPC_FIELD, mode) != 0)
    goto cpeConfigWanFail;
#endif

  if ( pPvcParam->natMode == CPE_ADMIN_ENABLE)
    mode = 1;
  else 
    mode = 0;
  if (BcmDb_configWanMode(&wanId, WAN_NAT_FIELD, mode) != 0)
    goto cpeConfigWanFail;

#ifdef CT_SNMP_MIB_VERSION_2
/***********************************************************************************/
    if(pPvcParam->bindingMode == CPE_ETHERNETPORT_BINDING_MODE)
    {
        if(0 != pPvcParam->ethernetPortMap)
        {
            char grpName[1024];
            char grepdIntf[1024];
            switch (pPvcParam->ethernetPortMap)
            {
                case 1:
                    sprintf(grpName,"nas_%d_%d_ENET1",vpi,vci);
                    sprintf(grepdIntf,"nas_%d_%d|eth1.2|",vpi,vci);
                    break;
                    
                case 2:
                    sprintf(grpName,"nas_%d_%d_ENET2",vpi,vci);
                    sprintf(grepdIntf,"nas_%d_%d|eth1.3|",vpi,vci);                    
                    break;
                case 3:
                    sprintf(grpName,"nas_%d_%d_ENET3",vpi,vci);
                    sprintf(grepdIntf,"nas_%d_%d|eth1.4|",vpi,vci);                    
                    break;
                case 4:
                    sprintf(grpName,"nas_%d_%d_ENET4",vpi,vci);
                    sprintf(grepdIntf,"nas_%d_%d|eth0|",vpi,vci);                    
                    break;
                default:
                    goto cpeConfigWanFail;
            }
            
            if(0 == nmsPortMapAdd(grpName, grepdIntf))
            {
                if (BcmDb_applyBridgeCfg() == 0) 
                {
                    BcmPsi_flush();
                }
                else
                    goto cpeConfigWanFail;
            }
            else
                goto cpeConfigWanFail;
        }
    }
    else if(pPvcParam->bindingMode == CPE_ETHERNETPORT_NO_BINDING_MODE)
    {
            if(nmsGetPvcBindMode(pPvcParam->vpi,pPvcParam->vci) == CPE_ETHERNETPORT_BINDING_MODE)
            {
                char groupName[IFC_TINY_LEN];
                void *node = NULL;
                char grpedIntf[IFC_LARGE_LEN];
                char dynIntf[IFC_LARGE_LEN];
                char intfList[IFC_LARGE_LEN];
                char pvcName[IFC_LARGE_LEN];
                int ret = CPE_ETHERNETPORT_NO_BINDING_MODE;
                sprintf(pvcName,"nas_%d_%d",vpi,vci);
                node = BcmPMcm_getPMapCfg(NULL, groupName, grpedIntf);
                BcmDb_getDynIntf(groupName, dynIntf, IFC_LARGE_LEN);
                memset(intfList, 0, IFC_LARGE_LEN);
                cmsUtl_strncpy(intfList, grpedIntf, IFC_LARGE_LEN);
                while(node != NULL)
                {
                    if (strlen(dynIntf) != 0) 
                    {
                        strcat(intfList, "|");
                        strncat(intfList, dynIntf, (IFC_LARGE_LEN - strlen(intfList)));
                    }
                    if (strcmp(groupName, "Default") != 0) 
                    {
                        if(strstr(intfList,pvcName)!=0)
                        {  
                            if(nmsPortMapRemove(groupName)==PORT_MAP_STS_OK)
                            {
                                  if (BcmDb_applyBridgeCfg() == PORT_MAP_STS_OK) 
                                  {
                                        BcmPsi_flush();
                                  }
                                  break;
                             }
                        }              
                    }                
                    node = BcmPMcm_getPMapCfg(node, groupName, grpedIntf);
                    memset(dynIntf, 0, IFC_LARGE_LEN);
                    BcmDb_getDynIntf(groupName, dynIntf, IFC_LARGE_LEN);
                    memset(intfList, 0, IFC_LARGE_LEN);
                    cmsUtl_strncpy(intfList, grpedIntf, IFC_LARGE_LEN);          
                }      
                
            }
    }
/****************************************************************************************/    
    if(pPvcParam->ethernetPortMap!=0)
    {
        if(pPvcParam->bindingMode == CPE_ETHERNETPORT_NO_BINDING_MODE)
        {
            ret = nmsSetPvcEthernetPortMapNoBindMode(vpi,vci,pPvcParam->ethernetPortMap);
            if (ret == -1)
                goto cpeConfigWanFail;
            
        }
    }
    
    if(pPvcParam->serviceTypeIndex != 0)
    {
        ret = vclSetAal5EncapType(ATM_PORT_NUM,vpi,vci,pPvcParam->serviceTypeIndex);
        if (ret == -1)
            goto cpeConfigWanFail;
    }
/****************************************************************************************/   

#endif


  if (pPvcParam->adminStatus) { /* admin UP */
    BcmNtwk_startWan(&wanId);
  }
#ifdef BUILD_SNMP_DEBUG
  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
    printf("------------ cpeConfigWan OK\n");
  }
#endif
  free(pPvcParam);
  pPvcParam = NULL;
  return 0;
 cpeConfigWanFail:
#ifdef BUILD_SNMP_DEBUG
  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
    printf("!!!!!!!!!!  cpeConfigWanFail\n");
  }
#endif
if(pPvcParam)
{
  free(pPvcParam);
  pPvcParam = NULL;
}
  return -1;
}


void cpeFreePppInfo(void) {
  pCPE_PPP_INFO ptr;

  while (pPppList->head) {
    ptr = pPppList->head;
    pPppList->head = pPppList->head->next;
    free(ptr);
  }
  pPppList->count = 0;
}

void cpeAddPppToList(pCPE_PPP_INFO p)
{
  pCPE_PPP_INFO ptr, ptrNext, ptrHead;

  if (pPppList->head == NULL) {
    pPppList->head = p;
    pPppList->tail = p;
  }
  else {
    ptrHead = ptr = pPppList->head;
    ptrNext = ptr->next;
    while (ptr != NULL) {
      if ((ptrNext) && (p->link > ptrNext->link)) {
        /* move on */
        ptrHead = ptr;
        ptr = ptr->next;
        if (ptr != NULL)
          ptrNext = ptr->next;
        else
          ptrNext = NULL;
      }
      else {
        if (ptr->link > p->link) {
          p->next = ptr;
          //ptrHead = p;
          pPppList->head = p;
          break;
        } /* head */
        else if ((ptrNext == NULL) && (ptr->link < p->link)) {
          ptr->next = p;
          pPppList->tail = p;
          break;
        } /* end */
        else {
          p->next = ptrNext;
          ptr->next = p;
          break;
        } /* middle */
      } /* else, do the adding */
    } /* while */
  } /* list != NULL */
  pPppList->count++;
}

void cpePppInitDefault(int index, int link, pCPE_PPP_INFO pEntry)
{
  char name[IFC_TINY_LEN];

  memset(pEntry,0,sizeof(CPE_PPP_INFO));
  pEntry->indexId = index;
  pEntry->link = link;
  pEntry->pppIfIndex = link;
  pEntry->timeout = 0;
  pEntry->maxSegmentSize = CPE_PPP_DEFAULT_MSS;
  pEntry->adminStatus = CPE_PPP_ADMIN_UP;
  pEntry->operStatus = CPE_PPP_ADMIN_DOWN;
  /* figure out the vpi/vci/conId from link */
  if (ifGetNameByIfIndex(link,name) == 0) {
    sscanf(name,"ppp_%d_%d_%d",&pEntry->vpi,&pEntry->vci,&pEntry->conId);
  }
}

//int cpeReadPppStatus(char *name)
int cpeReadPppStatus(WAN_CON_ID *pWanId)
{
  if ((pPppTestData->vpi == pWanId->vpi) &&
      (pPppTestData->vci == pWanId->vci)) {
    return (pPppTestData->testResult);
  }
  else { 
    return CPE_PPP_TEST_RESULT_TESTING;
  }
}

void cpeScanPppInfo(void)
{
  WAN_CON_ID wanId;  /* vpi.vci.connId */
  WAN_CON_INFO wanInfo;
  WAN_PPP_INFO pppInfo;
  pCPE_PPP_INFO pEntry;
  char ifName[IFC_TINY_LEN];
  int i = 0;
  int status;

  cpeFreePppInfo();

#ifdef BUILD_SNMP_DEBUG
  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
    printf("cpeScanPppInfo(entry)\n");
  }
#endif

  memset(&wanId,0,sizeof(wanId));
  while (BcmDb_getWanInfoNext(&wanId,&wanInfo) != DB_WAN_GET_NOT_FOUND) {

#ifdef BUILD_SNMP_DEBUG
    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
      printf("cpeScanPppInfo(get wan next ok): wan.protocol %d, PROTO_PPPOE %d, PROTO_PPPOA %d\n",
             wanInfo.protocol,PROTO_PPPOE,PROTO_PPPOA);
    }
#endif

    if ((wanInfo.protocol != PROTO_PPPOE) && (wanInfo.protocol != PROTO_PPPOA))
      continue;

#ifdef BUILD_SNMP_DEBUG
    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
      printf("cpeScanPppInfo(get wan next ok)\n");
    }
#endif

    if ((pEntry= (pCPE_PPP_INFO)malloc(sizeof(CPE_PPP_INFO))) != NULL) {
      memset(pEntry,0,sizeof(CPE_PPP_INFO));
      if (BcmDb_getPppSrvInfo(&wanId,&pppInfo) == DB_WAN_GET_NOT_FOUND)
        continue;

#ifdef BUILD_SNMP_DEBUG
      if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
        printf("cpeScanPppInfo(get PPPsrv ok): wanId.vpi/vci %d/%d\n",wanId.vpi, wanId.vci);
      }
#endif
      pEntry->vpi = wanId.vpi;
      pEntry->vci = wanId.vci;
      pEntry->conId = wanId.conId;
      pEntry->adminStatus = pppInfo.adminStatus;

      status = BcmWan_getStatus(&wanId);
      switch (status)
        {
        case IFC_STATUS_UP:
          pEntry->operStatus = CPE_PPP_ADMIN_UP;
	  pEntry->testResult = CPE_PPP_TEST_RESULT_SUCCESS;
          break;
        case IFC_STATUS_AUTH_ERR:
          pEntry->operStatus = CPE_PPP_ADMIN_DOWN;
	  pEntry->testResult = CPE_PPP_TEST_RESULT_AUTHFAIL;
          break;
        case IFC_STATUS_NEW:
          pEntry->operStatus = CPE_PPP_ADMIN_DOWN;
	  pEntry->testResult = cpeReadPppStatus(&wanId);
	  break;
        case IFC_STATUS_DOWN:
	default:
          pEntry->operStatus = CPE_PPP_ADMIN_DOWN;
	  pEntry->testResult = cpeReadPppStatus(&wanId);
	  break;
        }
      pEntry->indexId = i;     
      cmsUtl_strncpy(pEntry->serviceName,pppInfo.serverName,strlen(pppInfo.serverName));
      pEntry->timeout = (pppInfo.idleTimeout) * 60;  /* PSI's is in minutes */
      pEntry->maxSegmentSize = CPE_PPP_DEFAULT_MSS;
      pEntry->adminStatus = pppInfo.adminStatus;
      cmsUtl_strncpy(pEntry->secretId,pppInfo.userName,strlen(pppInfo.userName));
      cmsUtl_strncpy(pEntry->secret,pppInfo.password,strlen(pppInfo.password));

      memset(ifName,0,IFC_TINY_LEN);
      /* note that when ppp fails to connect, it kills the interface, and recreate the
         interface again for retry; so, we have to ignore this whole entry at this
         particular instance. */
      sprintf(ifName,"ppp_%d_%d_%d",wanId.vpi,wanId.vci,wanId.conId);
      pEntry->link = ifGetIfIndexByName(ifName);
      if (pEntry->link <= 0)
        pEntry->link = IF_INDEX_PPP_BASE + i;
      i++;

      memset(ifName,0,IFC_TINY_LEN);
      sprintf(ifName,"nas_%d_%d",wanId.vpi,wanId.vci);
      pEntry->pvcIfIndex = ifGetIfIndexByName(ifName);
      pEntry->rowStatus = CPE_ROW_STATUS_ACTIVE;
      cpeAddPppToList(pEntry);
    } /* pEntry != NULL */
  } /* while */

#ifdef SNMP_DEBUG_FLAG
  if (pPppList->count != 0) {
    printf("************---pppList.count %d---************\n",
           pPppList->count);
    pEntry = pPppList->head;
    while (pEntry) {
      printf("link/Id %d/%d vpi/vci %d/%d\n",pEntry->link,pEntry->indexId,
             pEntry->vpi,pEntry->vci);
      pEntry = pEntry->next;
    }
  }
#endif
}

int cpeConfigPppInfo(unsigned long param) {
  pCPE_PPP_INFO pPpp = (pCPE_PPP_INFO)param;
  WAN_CON_ID wanId;  /* vpi.vci.connId */
  WAN_PPP_INFO pppInfo;
  int change = 0;

#ifdef BUILD_SNMP_DEBUG
  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
    printf("cpeConfigPppInfo(entry): vpi/vci/conId %d/%d/%d, serviceName %s\n",
           pPpp->vpi,pPpp->vci,pPpp->conId,pPpp->serviceName);
    printf("cpeConfigPppInfo(entry): timeout %d rowStatus %d, adminStatus %d\n",
           pPpp->timeout,pPpp->rowStatus,pPpp->adminStatus);
  }
#endif

  wanId.vpi = pPpp->vpi;
  wanId.vci = pPpp->vci;
  wanId.conId = pPpp->conId;
  /* get current configuration, and compare with new ones to see if a
     restart PPP action is needed */
  BcmDb_getPppSrvInfo(&wanId,&pppInfo);

  if (pPpp->rowStatus == CPE_ROW_STATUS_DESTROY) {
    memset(&pppInfo, 0, sizeof(WAN_PPP_INFO));
    pppInfo.authMethod = PPP_AUTH_METHOD_AUTO;
    pppInfo.adminStatus = PPP_ADMIN_STATUS_UP;
    BcmDb_setPppSrvInfo(&wanId, &pppInfo);
  }
  else {
    if ((pppInfo.idleTimeout * 60)!= pPpp->timeout) {
      pppInfo.idleTimeout = pPpp->timeout/60;
      change = 1;
    }
    if (strcmp(pppInfo.serverName,pPpp->serviceName) != 0) {
      strcpy(pppInfo.serverName,pPpp->serviceName);
      change = 1;
    }
    if (pppInfo.adminStatus != pPpp->adminStatus) {
      pppInfo.adminStatus = pPpp->adminStatus;
      change = 1;
    }
    if (change) {
      BcmDb_setPppSrvInfo(&wanId, &pppInfo);
    
#ifdef BUILD_SNMP_DEBUG
      printf("cpeConfigPppInfo(): pppInfo.password %s\n",pppInfo.password);
#endif 
    
      if (pppInfo.password[0] != '\0') {
	/*************** continue here *************************/
	/* start PPP only when admin status is UP or testing and that
	   ppp username and password are ready */
      
#ifdef BUILD_SNMP_DEBUG
	printf("cpeConfigPppInfo(): password set, adminsStatus %d\n",pppInfo.adminStatus);
#endif
	switch (pppInfo.adminStatus) 
	  {
	  case PPP_ADMIN_STATUS_UP:
	    if (!BcmPpp_isRunning(&wanId))
	      BcmPppoe_startPppd(&wanId);
	    break;
	  case PPP_ADMIN_STATUS_DOWN:
	    if (BcmPpp_isRunning(&wanId)) 
	      BcmPppoe_stopPppd(&wanId);
	    break;
	  case PPP_ADMIN_STATUS_TESTING:
	    /* do status polling and bring connection down after a certain time */
	    /* the polling routine sets operStatus to UP or DOWN */
	    /* ZTE NMS sends an ADMIN down, then admin up, and then admin testing;
	       so, when admin up is received, pppd is started; and while starting it,
	       there is no need to stop and restart the test if ppp is running when
	       admin test is received */
	    if (!BcmPpp_isRunning(&wanId)) 
	      BcmPppoe_startPppd(&wanId);
	  
	    cpeStartPppMonitor(&wanId);
	    break;
	  case PPP_ADMIN_STATUS_CANCEL:
	    /* stop status polling and bring connection down */
	    /* the polling routine sets operStatus to DOWN */
	    if (BcmPpp_isRunning(&wanId))
	      BcmPppoe_stopPppd(&wanId);
	    cpeStopPppMonitor(&wanId);
	    break;
	  } /* switch */
      }/* ppp password ok */
    } /* change */
  } /* not destroy */
  return 0;
}

void cpeFreeReferenceList(void)
{
  pCPE_PPP_REFERENCE ptr;

  while (pPppReferenceList->head) {
    ptr = pPppReferenceList->head;
    pPppReferenceList->head = pPppReferenceList->head->next;
    free(ptr);
  }
  memset(pPppReferenceList,0,sizeof(PPP_REFERENCE_LIST));
}

void cpeAddPppRefernceToList(pCPE_PPP_REFERENCE p)
{
  if (pPppReferenceList->head == NULL) {
    pPppReferenceList->head = p;
    pPppReferenceList->tail = p;
  }
  else {
    pPppReferenceList->tail->next = p;
    pPppReferenceList->tail = p;
  }
  pPppReferenceList->count++;
}

void cpeAddRouteToList(pCPE_IP_TABLE_INFO p)
{
#ifdef NOT_SORTING
  if (pIpList->head == NULL) {
    pIpList->head = p;
    pIpList->tail = p;
  }
  else {
    pIpList->tail->next = p;
    pIpList->tail = p;
  }
#else
  pCPE_IP_TABLE_INFO ptr, ptrNext, ptrHead;

  if (pIpList->head == NULL) {
    pIpList->head = p;
    pIpList->tail = p;
  }
  else {
    ptrHead = ptr = pIpList->head;
    ptrNext = ptr->next;
    while (ptr != NULL) {
      if ((ptrNext) && (p->index > ptrNext->index)) {
        /* move on */
        ptrHead = ptr;
        ptr = ptr->next;
        if (ptr != NULL)
          ptrNext = ptr->next;
        else
          ptrNext = NULL;
      }
      else {
        if (ptr->index > p->index) {
          p->next = ptr;
          //ptrHead = p;
          pIpList->head = p;
          break;
        } /* head */
        else if ((ptrNext == NULL) && (ptr->index < p->index)) {
          ptr->next = p;
          pIpList->tail = p;
          break;
        } /* end */
        else {
          p->next = ptrNext;
          ptr->next = p;
          break;
        } /* middle */
      } /* else, do the adding */
    } /* while */
  } /* list != NULL */
#endif

  pIpList->count++;
}

#ifdef CT_SNMP_MIB_VERSION_2

int cpeQosTagModeConfig(int value)
{
        int ret = 0;
        int port =ATM_PORT_NUM;
        IFC_ATM_VCC_INFO vccInfo;
        ATM_VCC_ADDR addr;
        ATM_VCC_CFG atmcfg;
        UINT16 vccId = 0;   // set to 0 to search vcc from beginning

        while ( BcmDb_getVccInfoNext(&vccId, &vccInfo) == DB_VCC_GET_OK ) 
        {
            if((vccInfo.vccAddr.usVpi!=RESERVE_PVC_1_39_VPI)||(vccInfo.vccAddr.usVci!=RESERVE_PVC_1_39_VCI))
            { 
                if((vccInfo.vccAddr.usVpi!=RESERVE_PVC_0_16_VPI)||(vccInfo.vccAddr.usVci!=RESERVE_PVC_0_16_VCI))
                {
                    if(value)
                        vccInfo.enblQos = 1;
                    else
                        vccInfo.enblQos = 0;
                    
                    ret=BcmDb_setVccInfo(vccId,&vccInfo);
                    
                    if(ret ==-1)
                    {
                        return ret;
                    }
                }
            }
        }
        AtmVcc_init();     
        return ret;
}


int nmsGetPvcBindMode(int vpi, int vci)
{
    void *node = NULL;
    char groupName[IFC_TINY_LEN];
    char grpedIntf[IFC_LARGE_LEN];
    char dynIntf[IFC_LARGE_LEN];
    char intfList[IFC_LARGE_LEN];
    char pvcName[IFC_LARGE_LEN];
    int ret = CPE_ETHERNETPORT_NO_BINDING_MODE;


    if((vpi==RESERVE_PVC_1_39_VPI)&&(vci==RESERVE_PVC_1_39_VCI))
        return CPE_ETHERNETPORT_NO_BINDING_MODE;
    else if((vpi==RESERVE_PVC_0_16_VPI)&&(vci==RESERVE_PVC_0_16_VCI))
        return CPE_ETHERNETPORT_NO_BINDING_MODE;
    
    sprintf(pvcName,"nas_%d_%d",vpi,vci);
    node = BcmPMcm_getPMapCfg(NULL, groupName, grpedIntf);
    BcmDb_getDynIntf(groupName, dynIntf, IFC_LARGE_LEN);
    memset(intfList, 0, IFC_LARGE_LEN);
    cmsUtl_strncpy(intfList, grpedIntf, IFC_LARGE_LEN);
    while(node != NULL)
    {
        if (strlen(dynIntf) != 0) 
        {
            strcat(intfList, "|");
            strncat(intfList, dynIntf, (IFC_LARGE_LEN - strlen(intfList)));
        }
        if (strcmp(groupName, "Default") == 0) 
        {
            if(strstr(intfList,pvcName)!=NULL)
            {                                    
                return CPE_ETHERNETPORT_NO_BINDING_MODE;
            }
            else
            {
                return  CPE_ETHERNETPORT_BINDING_MODE;                              
            }
        }
    
        node = BcmPMcm_getPMapCfg(node, groupName, grpedIntf);
        memset(dynIntf, 0, IFC_LARGE_LEN);
        BcmDb_getDynIntf(groupName, dynIntf, IFC_LARGE_LEN);
        memset(intfList, 0, IFC_LARGE_LEN);
        cmsUtl_strncpy(intfList, grpedIntf, IFC_LARGE_LEN);          
    }

    return ret;
}


int nmsGetPvcEthernetPortMap(int vpi, int vci)
{
    void *node = NULL;
    char groupName[IFC_TINY_LEN];
    char grpedIntf[IFC_LARGE_LEN];
    char dynIntf[IFC_LARGE_LEN];
    char intfList[IFC_LARGE_LEN];
    char pvcName[IFC_LARGE_LEN];
    int ret = 0;
    int i=0;
    static char* ethIfName[] = {"eth1.2", "eth1.3", "eth1.4", "eth0"};       

    if((vpi==RESERVE_PVC_1_39_VPI)&&(vci==RESERVE_PVC_1_39_VCI))
        return 0;
    else if((vpi==RESERVE_PVC_0_16_VPI)&&(vci==RESERVE_PVC_0_16_VCI))
        return 0;

    sprintf(pvcName,"nas_%d_%d",vpi,vci);
    node = BcmPMcm_getPMapCfg(NULL, groupName, grpedIntf);
    BcmDb_getDynIntf(groupName, dynIntf, IFC_LARGE_LEN);
    memset(intfList, 0, IFC_LARGE_LEN);
    cmsUtl_strncpy(intfList, grpedIntf, IFC_LARGE_LEN);
    while(node != NULL)
    {
        if (strlen(dynIntf) != 0) 
        {
            strcat(intfList, "|");
            strncat(intfList, dynIntf, (IFC_LARGE_LEN - strlen(intfList)));
        }
        if (strcmp(groupName, "Default") == 0) 
        {
            if(strstr(intfList,pvcName)!=NULL)
            {  
                ret = 0;
                return ret;
            }
        }
        else
        {
            if(strstr(intfList,pvcName)!=NULL)
            {
                for(i=0;i<4;i++)
                {
                    if(strstr(intfList,ethIfName[i])!=NULL)
                    {
                        return i+1;
                    }
                }
            }
        }
    
        node = BcmPMcm_getPMapCfg(node, groupName, grpedIntf);
        memset(dynIntf, 0, IFC_LARGE_LEN);
        BcmDb_getDynIntf(groupName, dynIntf, IFC_LARGE_LEN);
        memset(intfList, 0, IFC_LARGE_LEN);
        cmsUtl_strncpy(intfList, grpedIntf, IFC_LARGE_LEN);          
    }

    return ret;
}

int nmsGetPvcEthernetPortMapNoBindMode(int vpi, int vci)
{
    int ret = 0;
    int i = 0;
    for(i=0;i<8;i++)
    {
       if((cpePvcEthPortMapNobind[i].vpi==vpi)&&(cpePvcEthPortMapNobind[i].vci==vci))
       {
            return cpePvcEthPortMapNobind[i].ethnetportmap;
       }
    }

    return ret;
}

int nmsSetPvcEthernetPortMapNoBindMode(int vpi, int vci, int value)
{
    int ret = 0;
    int i =0;
    int find =0;
    int save = 0;
    for(i=0;i<8;i++)
    {
        if((cpePvcEthPortMapNobind[i].vpi==vpi)&&(cpePvcEthPortMapNobind[i].vci==vci))
        {
            cpePvcEthPortMapNobind[i].ethnetportmap = value;
            save = 1;
            find = 1;
            break;
        }
    }
    
    if(find==0)
    {
        for(i=0;i<8;i++)
        {
            if(cpePvcEthPortMapNobind[i].inused==0)
            {
                cpePvcEthPortMapNobind[i].vpi = vpi;
                cpePvcEthPortMapNobind[i].vci = vci;
                cpePvcEthPortMapNobind[i].ethnetportmap= value;
                cpePvcEthPortMapNobind[i].inused = 1;
                save = 1;
                break;
            }
        }
    }
    if(!save)
        return -1;
    return ret;
}

void  nmsGetPvcGroupName(int vpi, int vci, char *groupName)
{
    void *node = NULL;
    char grpedIntf[IFC_LARGE_LEN];
    char dynIntf[IFC_LARGE_LEN];
    char intfList[IFC_LARGE_LEN];
    char pvcName[IFC_LARGE_LEN];
    int ret = CPE_ETHERNETPORT_NO_BINDING_MODE;

    sprintf(pvcName,"nas_%d_%d",vpi,vci);
    node = BcmPMcm_getPMapCfg(NULL, groupName, grpedIntf);
    BcmDb_getDynIntf(groupName, dynIntf, IFC_LARGE_LEN);
    memset(intfList, 0, IFC_LARGE_LEN);
    cmsUtl_strncpy(intfList, grpedIntf, IFC_LARGE_LEN);
    while(node != NULL)
    {
        if (strlen(dynIntf) != 0) 
        {
            strcat(intfList, "|");
            strncat(intfList, dynIntf, (IFC_LARGE_LEN - strlen(intfList)));
        }
        if (strcmp(groupName, "Default") != 0) 
        {
            if(strstr(intfList,pvcName)!=0)
            {                                    
                return ;
            }
            else
            {
                groupName = NULL;
                return ;                              
            }
        }
    
        node = BcmPMcm_getPMapCfg(node, groupName, grpedIntf);
        memset(dynIntf, 0, IFC_LARGE_LEN);
        BcmDb_getDynIntf(groupName, dynIntf, IFC_LARGE_LEN);
        memset(intfList, 0, IFC_LARGE_LEN);
        cmsUtl_strncpy(intfList, grpedIntf, IFC_LARGE_LEN);          
    }

}

int nmsPortMapAdd(char *grpName, char *grpedIntf)
{
   int sts = 0;
   void *node = NULL;
   char brIfName[IFC_SMALL_LEN];
   char cfgGrpedIntf[IFC_GIANT_LEN];
   char *pIfName = NULL;
   char *pIfNameEnd = NULL;
   char ifName[IFC_TINY_LEN];
   char cmd[1024];
   char str[IFC_LARGE_LEN];
   FILE *errFs = NULL;

   cmd[0]          = '\0';
   str[0]          = '\0';

   // if route cfg mngr is not initialized yet then return NULL.
   // it can happen when system is changed from other mode to
   // bridge mode only but user does not allow system to update the
   // tree menu yet so that the tree menu still has the old link
   // to configure static route, rip, or default gateway
   if ( BcmPMcm_isInitialized() == FALSE ) 
   {
      node = NULL;
   } 
   else 
   {
      node = BcmPMcm_getPMapCfg(NULL, brIfName, cfgGrpedIntf);
   }

   // Check if this group is already in the configuration. Initially when there is
   // no configuration just accept any name.
   while ( node != NULL ) 
   {
      if (strcmp(brIfName, grpName) == 0) 
      {
         return sts = -1;
      }
      node = BcmPMcm_getPMapCfg(node, brIfName, cfgGrpedIntf);
   }

   // Add the group only if all the interfaces are enabled
   pIfName = grpedIntf;
   while (pIfName <= (grpedIntf + strlen(grpedIntf))) 
    {
      if ((pIfNameEnd = strstr(pIfName, "|")) != NULL) 
      {
         memset(ifName, '\0', IFC_TINY_LEN);
         memcpy(ifName, pIfName, (pIfNameEnd - pIfName));
         if (*ifName != '\0') 
         {
            sprintf(cmd, "ifconfig %s 2>/var/ifcfgerr", ifName);
            prctl_runCommandInShellWithTimeout(cmd);

            str[0] = '\0';
            errFs = fopen("/var/ifcfgerr", "r");
            if (errFs != NULL ) {
               fgets(str, IFC_LARGE_LEN, errFs);
               fclose(errFs);
               prctl_runCommandInShellWithTimeout("rm /var/ifcfgerr");
               if (strstr(str, "error fetching interface information") != NULL) 
               {
                  return sts = -1;
               }
            }
         }
      }
      else 
      {
         break;
      }
      pIfName = pIfNameEnd + 1;
   }
   
   // Allocate a new bridge entry in configuration and get its interface name.
   if (BcmDb_allocBridge(grpName, brIfName) == -1) 
   {
      return sts = -1;
   }

   // Add the bridge and set its configuration.
   sprintf(cmd, "brctl addbr %s 2>/dev/null", brIfName);
   prctl_runCommandInShellWithTimeout(cmd);
   sprintf(cmd, "brctl stp %s disable 2>/dev/null", brIfName);
   prctl_runCommandInShellWithTimeout(cmd);
   sprintf(cmd, "brctl setfd %s 0 2>/dev/null", brIfName);
   prctl_runCommandInShellWithTimeout(cmd);
   // ifconfig up the bridge
   sprintf(cmd, "ifconfig %s up 2>/dev/null", brIfName);
   prctl_runCommandInShellWithTimeout(cmd);

   // Get the selected interfaces for this group and parse them to
   // remove the | from the list.
   pIfName = grpedIntf;
   while (pIfName <= (grpedIntf + strlen(grpedIntf))) 
   {
      if ((pIfNameEnd = strstr(pIfName, "|")) != NULL) 
      {
         memset(ifName, '\0', IFC_TINY_LEN);
         memcpy(ifName, pIfName, (pIfNameEnd - pIfName));
         if (*ifName != '\0') 
         {
            BcmDb_delIfFromBridge(ifName);
            BcmDb_addBridgeIf(grpName, ifName);
            sprintf(cmd, "brctl addif %s %s 2>/dev/null", brIfName, ifName);
            prctl_runCommandInShellWithTimeout(cmd);
            // If we add an interface to br0 which is hooked to the IP stack, when
            // the MAC address changes (as we add or delete interfaces), we must
            // send an arp to all devices attached to it to propagate the change.
            if (strcmp(brIfName, IFC_BRIDGE_NAME) == 0) 
            {
               sprintf(cmd, "sendarp -s %s -d %s", IFC_BRIDGE_NAME, IFC_BRIDGE_NAME);
               prctl_runCommandInShellWithTimeout(cmd);
            }
         }
      } 
      else 
      {
         break;
      }
      pIfName = pIfNameEnd + 1;
   }
   return sts;
}

int nmsPortMapRemove(char *lst)
{
    
       PORT_MAP_STATUS sts = PORT_MAP_STS_OK;
       char cmd[IFC_LARGE_LEN];
       char groupName[IFC_TINY_LEN];
       char grpedIntf[IFC_LARGE_LEN];
       char dynIntf[IFC_LARGE_LEN];
       char *pIfName = NULL;
       char *pIfNameEnd = NULL;
       char ifName[IFC_TINY_LEN];
       char *pToken = NULL;
       char *pLast = NULL;
       int  idx = 0;
    
       memset(groupName, 0, IFC_TINY_LEN);
       memset(grpedIntf, 0, IFC_LARGE_LEN);
    
       pToken = strtok_r(lst, ", ", &pLast);
       while ( pToken != NULL ) 
       {
          int index;
          int removedFlag;
          // Get the bridge configuration from the database and first remove all
          // interfaces mapped to this bridge.
          index = removedFlag = 0;
          while (index < PORT_MAP_MAX_ENTRY) 
          {
              int procDynIntf = 0;
              if (BcmDb_getBridgeInfoByIndex(index, groupName, grpedIntf) == NULL) 
              {
                 index++;
                 continue;
              }
    
              // We do not allow users to delete the default group.
              if (strcmp(pToken, "Default") == 0) 
              {
                 index++;
                 continue;
              }
    
              if (strcmp(groupName, pToken) == 0) 
              {
                 // Clear all the vendor ID strings.
                 for (index = 0; index < DHCP_MAX_VENDOR_IDS; index++) 
                 {
                    BcmDb_setDHCPVendorId(idx, "", groupName);
                 }
    
                 // Get the list of dynamically added interfaces fo this group.
                 memset(dynIntf, 0, IFC_LARGE_LEN);
                 BcmDb_getDynIntf(groupName, dynIntf, IFC_LARGE_LEN);
    
                 // This is the correct group. Now remove all associated
                 // interfaces that are configured statically in PSI.
    remove_intf:    pIfName = grpedIntf;
                    while (pIfName <= (grpedIntf + strlen(grpedIntf))) 
                    {
                        if ((pIfNameEnd = strstr(pIfName, "|")) != NULL) 
                        {
                           memset(ifName, '\0', IFC_TINY_LEN);
                           memcpy(ifName, pIfName, (pIfNameEnd - pIfName));
                           if (*ifName == '\0') {
                              pIfName = pIfNameEnd + 1;
                              continue;
                           }
                           // Delete this interface from the temp store also.
                           BcmDb_delIfFromBridge(ifName);
                           // Add this newly free interface to the default bridge.
                           BcmDb_addBridgeIf("Default", ifName);
                           sprintf(cmd, "brctl addif %s %s 2>/dev/null", IFC_BRIDGE_NAME, ifName);
                           prctl_runCommandInShellWithTimeout(cmd);
                           // MAC address of IFC_BRIDGE_NAME could have changed with this addition,
                           // so send an arp to all its attached devices and propagate the change.
                           sprintf(cmd, "sendarp -s %s -d %s", IFC_BRIDGE_NAME, IFC_BRIDGE_NAME);
                           prctl_runCommandInShellWithTimeout(cmd);
                           pIfName = pIfNameEnd + 1;
                        } 
                        else 
                        {
                           memset(ifName, '\0', IFC_TINY_LEN);
                           strcpy(ifName, pIfName);
                           // Delete this interface from the temp store also.
                           BcmDb_delIfFromBridge(ifName);
                           // Add this newly free interface to the default bridge.
                           BcmDb_addBridgeIf("Default", ifName);
                           sprintf(cmd, "brctl addif %s %s 2>/dev/null", IFC_BRIDGE_NAME, ifName);
                           prctl_runCommandInShellWithTimeout(cmd);
                           // MAC address of IFC_BRIDGE_NAME could have changed with this addition,
                           // so send an arp to all its attached devices and propagate the change.
                           sprintf(cmd, "sendarp -s %s -d %s", IFC_BRIDGE_NAME, IFC_BRIDGE_NAME);
                           prctl_runCommandInShellWithTimeout(cmd);
                           break;
                        }
                     }
                     if (!procDynIntf) 
                     {
                        if (strlen(dynIntf) > 0) 
                        {
                           procDynIntf = 1;
                           memset(grpedIntf, 0, IFC_LARGE_LEN);
                           cmsUtl_strncpy(grpedIntf, dynIntf, IFC_LARGE_LEN);
                           goto remove_intf;
                        }
                     }
                     // Finally remove the bridge from the list of interfaces
                     BcmDb_delBridge(groupName);
                     // Remove it from the PSI
                     sts = BcmPMcm_removePMapCfg(groupName);
                     memset(groupName, 0, IFC_TINY_LEN);
                     memset(grpedIntf, 0, IFC_LARGE_LEN);
                     removedFlag = 1;
                     break;
              }
              if (!removedFlag)
                 index++;
              else
                 break;
          }
         if ( sts != PORT_MAP_STS_OK ) break;
          pToken = strtok_r(NULL, ", ", &pLast);
       }
       return sts;
    
}

int nmsGetEthernetPortBindMode(int ethIfIndex)
{
    void *node = NULL;
    char groupName[IFC_TINY_LEN];
    char grpedIntf[IFC_LARGE_LEN];
    char dynIntf[IFC_LARGE_LEN];
    char intfList[IFC_LARGE_LEN];
    char pvcName[IFC_LARGE_LEN];
    int ret = CPE_ETHERNETPORT_NO_BINDING_MODE;
    static char* ethIfName[] = {"eth1.2", "eth1.3", "eth1.4", "eth0"};  
    
    node = BcmPMcm_getPMapCfg(NULL, groupName, grpedIntf);
    BcmDb_getDynIntf(groupName, dynIntf, IFC_LARGE_LEN);
    memset(intfList, 0, IFC_LARGE_LEN);
    cmsUtl_strncpy(intfList, grpedIntf, IFC_LARGE_LEN);
    while(node != NULL)
    {
        if (strlen(dynIntf) != 0) 
        {
            strcat(intfList, "|");
            strncat(intfList, dynIntf, (IFC_LARGE_LEN - strlen(intfList)));
        }
        if (strcmp(groupName, "Default") == 0) 
        {
            if(strstr(intfList,ethIfName[ethIfIndex])!=NULL)
            {                                    
                return CPE_ETHERNETPORT_NO_BINDING_MODE;
            }
            else
            {
                return  CPE_ETHERNETPORT_BINDING_MODE;                              
            }
        }
    
        node = BcmPMcm_getPMapCfg(node, groupName, grpedIntf);
        memset(dynIntf, 0, IFC_LARGE_LEN);
        BcmDb_getDynIntf(groupName, dynIntf, IFC_LARGE_LEN);
        memset(intfList, 0, IFC_LARGE_LEN);
        cmsUtl_strncpy(intfList, grpedIntf, IFC_LARGE_LEN);          
    }

    return ret;
}

int nmsGetEthernetPortI8021dPara(int ethIfIndex)
{
    int ret = 0;
    void *node, *prevnode;
    QOS_CLASS_ENTRY entry;
    node = NULL;
    prevnode = NULL;
    static char* ethIfName[] = {"eth1.2", "eth1.3", "eth1.4", "eth0"};  
    
    node = BcmQos_getQosClass(NULL, &entry);
    for (; node != NULL; node = BcmQos_getQosClass(node, &entry)) 
    {
        if (strcmp(entry.lanIfcName,ethIfName[ethIfIndex]) == 0) 
        {
            break;
        }
        prevnode = node;
    }

    if (node != NULL) 
    {
        return entry.priority;
    } 
    else
    {
        return 0;
    }
    return ret;
}

int nmsGetEthernetPortIgmpOnOff(int ethIfIndex)
{
    int ret = 2;
    SEC_MAC_FLT_ENTRY entry;
    void *node = NULL;
    static char* ethIfName[] = {"eth1.2", "eth1.3", "eth1.4", "eth0"};  
    
    node = BcmScm_getMacFilter(NULL, &entry);
    while ( node != NULL ) 
    {
        if(entry.flag.protocol== SEC_PROTO_IGMP_NMS)
        {
            if(strcmp(entry.wanIf,ethIfName[ethIfIndex])==0)
            {
                ret = 1;
                break;
            }
        }
        node = BcmScm_getMacFilter(node, &entry);
    }

    return ret;
}



void cpeAddEthernetPortInfoToList(pCPE_ETHERNETPORT_TABLE_INFO p)
{
    if (pEthernetPortList->head == NULL) 
    {
        pEthernetPortList->head = p;
        pEthernetPortList->tail = p;
    }
    else 
    {
        pEthernetPortList->tail->next = p;
        pEthernetPortList->tail = p;
    }
    pEthernetPortList->count++;
}

void cpeFreeEthernetPortInfo(void) {
  pCPE_ETHERNETPORT_TABLE_INFO ptr = NULL;

  while (pEthernetPortList->head) {
    ptr = pEthernetPortList->head;
    pEthernetPortList->head = pEthernetPortList->head->next;
    if(ptr)
        free(ptr);
  }
  pEthernetPortList->count = 0;
  pEthernetPortList->head = NULL;
}

int cpeScanEthernetPortTable(void)
{
       /******************************************************

               |----1----|----2----|----3----|----4----|
               |-eth1.2---|--eth1.3--|--eth1.4--|----eth0--|
               |--ENET1--|--ENET2--|--ENET3--|--ENET4--|
               
       *****************************************************/       
       UINT16 retLen = 0;
       int i = 0;
	PORT_MAP_IFC_STATUS_INFO pmapIfcStatus;
	PSI_HANDLE pmapPsi = BcmPsi_appOpen(PMAP_APPID);
	pCPE_ETHERNETPORT_TABLE_INFO pEntry;
        
       cpeFreeEthernetPortInfo();

       memset(&pmapIfcStatus, 0, sizeof(PORT_MAP_IFC_STATUS_INFO));
       BcmPsi_objRetrieve(pmapPsi, PMCM_PMAP_IFC_STS_ID,
                          (PSI_VALUE)&pmapIfcStatus,
                          sizeof(PORT_MAP_IFC_STATUS_INFO),
                          &retLen);

       if(pmapIfcStatus.status)
       {
           for(i=0;i<4;i++)
           {
                 if ((pEntry= (pCPE_ETHERNETPORT_TABLE_INFO)malloc(sizeof(CPE_ETHERNETPORT_TABLE_INFO))) != NULL) 
		   {
                        memset(pEntry,0,sizeof(CPE_ETHERNETPORT_TABLE_INFO));

                        pEntry->ethernetPortIfIndex = i + 1;
                        pEntry->ethernetPortSerialNumber = i + 1 ;
                        pEntry->ethernetPortBindingMode = 0;
                        pEntry->ethernetPortI8021dParameter = 0;
                        pEntry->ethernetPortIgmpOnOff = 0 ;

                        pEntry->ethernetPortBindingMode = nmsGetEthernetPortBindMode(i);
                        pEntry->ethernetPortIgmpOnOff = nmsGetEthernetPortIgmpOnOff(i);
                        pEntry->ethernetPortI8021dParameter = nmsGetEthernetPortI8021dPara(i);

                        cpeAddEthernetPortInfoToList(pEntry);
                 }
           }
       }
	return 1;
}


int cpeConfigEthernetPortTable(pCPE_ETHERNETPORT_TABLE_INFO pEthernetPort) 
{
    static char* ethIfName[] = {"eth1.2", "eth1.3", "eth1.4", "eth0"}; 
    int ethernetPortIfIndex = 0;
    pCPE_ETHERNETPORT_TABLE_INFO pEntry;
    ethernetPortIfIndex = pEthernetPort->ethernetPortIfIndex;
    
    pEntry = pEthernetPortList->head;
    while (pEntry) 
    {
        if(pEntry->ethernetPortIfIndex ==pEthernetPort->ethernetPortIfIndex)
            break; /* found */
        pEntry = pEntry->next;
    }
    
    if (pEntry == NULL) 
    {
        printf("\r\n can't find return -1\r\n");
        return -1;
    }
    
    if(pEntry->ethernetPortBindingMode != pEthernetPort->ethernetPortBindingMode)
    {
        
    }
    if(pEntry->ethernetPortIgmpOnOff != pEthernetPort->ethernetPortIgmpOnOff)
    {
        int sts = 0;
        SEC_MAC_FLT_ENTRY entry;
        memset(&entry,0,sizeof(SEC_MAC_FLT_ENTRY));
        entry.flag.protocol = SEC_PROTO_IGMP_NMS;
        entry.flag.direction = 2;
        strcpy(entry.destMac, WEB_SPACE);
        strcpy(entry.srcMac, WEB_SPACE);
        strcpy(entry.wanIf,ethIfName[ethernetPortIfIndex-1]);
        if(pEthernetPort->ethernetPortIgmpOnOff==1)
        {
            sts = BcmScm_addMacFilter(&entry);
            if(sts != SEC_STS_OK)
                return -1;
        }
        else if(pEthernetPort->ethernetPortIgmpOnOff == 2)
        {
            sts = BcmScm_removeMacFilter(&entry);
            if(sts !=SEC_STS_OK)
                return -1;
        }
        else
            return -1;
    }
    
    if(pEntry->ethernetPortI8021dParameter != pEthernetPort->ethernetPortI8021dParameter)
    {
           if(pEthernetPort->ethernetPortI8021dParameter!=0)
          {
              if(pEntry->ethernetPortI8021dParameter ==0)
              {
                  QOS_CLASS_ENTRY entry;
                  int sts = 0;
                  memset(&entry,0,sizeof(QOS_CLASS_ENTRY));
                  entry.type = QOS_TYPE_GENERAL;
                  entry.priority = pEthernetPort->ethernetPortI8021dParameter;
                  entry.wlpriority = 0;
                  entry.precedence = -1;
                  entry.wanVlan8021p = -1;
                  entry.tos = -1;
                  entry.vlan8021p = -1;   
                  entry.protocol = -1;
                  strcpy(entry.clsName,ethIfName[ethernetPortIfIndex-1]);
                  strcpy(entry.lanIfcName,ethIfName[ethernetPortIfIndex-1]);
                  sts = BcmQos_addQosClass(&entry);
                  if(sts !=SEC_STS_OK)
                    return -1;
              }
              else
              {
                    void *node, *prevnode;
                    QOS_CLASS_ENTRY qosentry;
                    node = NULL;
                    prevnode = NULL;
                    node = BcmQos_getQosClass(NULL, &qosentry);
                    for (; node != NULL; node = BcmQos_getQosClass(node, &qosentry)) 
                    {
                        if (qosentry.type != QOS_TYPE_GENERAL) 
                        {
                            continue;
                        }
                        if (strcmp(qosentry.lanIfcName,ethIfName[ethernetPortIfIndex-1]) == 0) 
                        {
                            break;
                        }
                        prevnode = node;
                    }

                    if (node != NULL) 
                    {
                          QOS_CLASS_ENTRY entry;
                          int sts = 0;
                          memset(&entry,0,sizeof(QOS_CLASS_ENTRY));
                          entry.type = QOS_TYPE_GENERAL;
                          entry.priority = pEthernetPort->ethernetPortI8021dParameter;
                          entry.wlpriority = 0;
                          entry.precedence = -1;
                          entry.wanVlan8021p = -1;
                          entry.tos = -1;
                          entry.vlan8021p = -1;   
                          entry.protocol = -1;
                          strcpy(entry.clsName,ethIfName[ethernetPortIfIndex-1]);
                          strcpy(entry.lanIfcName,ethIfName[ethernetPortIfIndex-1]);
                          sts = BcmQos_removeQosClass(qosentry.clsName, QOS_TYPE_GENERAL);
                          if(sts !=SEC_STS_OK)
                            return -1;            
                          sts = BcmQos_addQosClass(&entry);
                          if(sts !=SEC_STS_OK)
                            return -1;                          
                    } 
              }
          }
          else
          {               
                void *node, *prevnode;
                QOS_CLASS_ENTRY qosentry;
                node = NULL;
                prevnode = NULL;
                int sts = 0;
                node = BcmQos_getQosClass(NULL, &qosentry);
                for (; node != NULL; node = BcmQos_getQosClass(node, &qosentry)) 
                {
                    if (qosentry.type != QOS_TYPE_GENERAL) 
                    {
                        continue;
                    }
                    if (strcmp(qosentry.lanIfcName,ethIfName[ethernetPortIfIndex-1]) == 0) 
                    {
                        break;
                    }
                    prevnode = node;
                }

                if (node != NULL) 
                {
                    sts = BcmQos_removeQosClass(qosentry.clsName, QOS_TYPE_GENERAL);
                    if(sts !=SEC_STS_OK)
                      return -1;                      
                }                 
          }
    }

    return 0;
}   

void cpeAddServiceTypeInfoToList(pCPE_SERVICETYPE_TABLE_INFO p)
{
	if (pServiceTypeList->head == NULL) 
	{
		pServiceTypeList->head = p;
		pServiceTypeList->tail = p;
	}
	else 
	{
		pServiceTypeList->tail->next = p;
		pServiceTypeList->tail = p;
	}
	pServiceTypeList->count++;
}

void cpeFreeServiceTypeInfo(void) {
  pCPE_SERVICETYPE_TABLE_INFO ptr;

  while (pServiceTypeList->head) {
	ptr = pServiceTypeList->head;
	pServiceTypeList->head = pServiceTypeList->head->next;
	free(ptr);
  }
  pServiceTypeList->count = 0;
}

int cpeScanServiceTypeTable(void)
{
	  UINT16 tdId = 0;
	  IFC_ATM_TD_INFO TdInfo;
	  pCPE_SERVICETYPE_TABLE_INFO pEntry;
	  cpeFreeServiceTypeInfo();
	  while (BcmDb_getTdInfoNext(&tdId,&TdInfo)!= DB_GET_NOT_FOUND) 
	  {
		   if ((pEntry= (pCPE_SERVICETYPE_TABLE_INFO)malloc(sizeof(CPE_SERVICETYPE_TABLE_INFO))) != NULL) 
		   {
      			memset(pEntry,0,sizeof(CPE_SERVICETYPE_TABLE_INFO));
      			if (BcmDb_getTdInfoByTdId(tdId,&TdInfo) == DB_GET_NOT_FOUND)
        			continue;
			      pEntry->serviceTypeIndex= tdId; 
			      if(strcmp(TdInfo.serviceCategory,"UBR")==0)
			      		pEntry->pvcServiceType= CPE_SERVICETYPE_UBR;
			      else if(strcmp(TdInfo.serviceCategory,"RT_VBR")==0)
				  	pEntry->pvcServiceType=CPE_SERVICETYPE_RT_VBR;
  			      else if(strcmp(TdInfo.serviceCategory,"NRT_VBR")==0)
				  	pEntry->pvcServiceType=CPE_SERVICETYPE_NRT_VBR;
			      else if(strcmp(TdInfo.serviceCategory,"CBR")==0)
				  	pEntry->pvcServiceType=CPE_SERVICETYPE_CBR;				  
			      pEntry->pvcMcr=TdInfo.maxBurstSize;
			      pEntry->pvcPcr=TdInfo.peakCellRate;
			      pEntry->pvcScr=TdInfo.sustainedCellRate;
			      pEntry->serviceTypeRowStatus=CPE_ROW_STATUS_ACTIVE;
      			      cpeAddServiceTypeInfoToList(pEntry);
    		   }
	  }
	  return 1;
}

int cpeConfigServiceTypeTable(pCPE_SERVICETYPE_TABLE_INFO pServiceType) 
{
	int tdId=0;
  	int change = 0;
	IFC_ATM_TD_INFO tdInfo;
	char serviceCategory[IFC_TINY_LEN];
	
	tdId=pServiceType->serviceTypeIndex;
	
  	BcmDb_getTdInfoByTdId(tdId,&tdInfo);

  	if (pServiceType->serviceTypeRowStatus == CPE_ROW_STATUS_DESTROY) 
  	{
  		AtmTd_removeFromPsiOnly(tdId);
  	}
	else
	{
		if (tdInfo.peakCellRate!=pServiceType->pvcPcr) 
		{
			tdInfo.peakCellRate = pServiceType->pvcPcr;
			change = 1;
		}
		if (tdInfo.sustainedCellRate!=pServiceType->pvcScr) 
		{
			tdInfo.sustainedCellRate = pServiceType->pvcScr;
			change = 1;
		}
		if (tdInfo.maxBurstSize!=pServiceType->pvcMcr) 
		{
			tdInfo.maxBurstSize = pServiceType->pvcMcr;
			change = 1;
		}
		
		switch(pServiceType->pvcServiceType)
		{
			case  CPE_SERVICETYPE_UBR:
			strcpy(serviceCategory,"UBR");
			break;
			case CPE_SERVICETYPE_RT_VBR:
			strcpy(serviceCategory,"RT_VBR");
			break;
			case CPE_SERVICETYPE_NRT_VBR:
			strcpy(serviceCategory,"NRT_VBR");
			break;
			case CPE_SERVICETYPE_CBR:
			strcpy(serviceCategory,"CBR");
			break;
			default:
			strcpy(serviceCategory,"UBR");
			break;
		}

		if(strcmp(serviceCategory,tdInfo.serviceCategory)!=0)
		{
			strcpy(tdInfo.serviceCategory,serviceCategory);
			change = 1;
		}

		if (change) 
		{
			AtmTd_setTdInfoFromObjectId(tdId,&tdInfo);
			if(tdId==0)
			{
				return -1;
			}
			return 0;
		} 
	} 
  	return 0;
}	

int TdAddEntry(CPE_SERVICETYPE_TABLE_INFO* tableRow)
{
      int tdId;
      IFC_ATM_TD_INFO tdInfo;
      
      memset(&tdInfo, 0, sizeof(tdInfo));

      switch(tableRow->pvcServiceType)
      {
          	case  CPE_SERVICETYPE_UBR:
        		strcpy(tdInfo.serviceCategory,"UBR");
        		break;
        	case CPE_SERVICETYPE_RT_VBR:
        		strcpy(tdInfo.serviceCategory,"RT_VBR");
        		break;
        	case CPE_SERVICETYPE_NRT_VBR:
        		strcpy(tdInfo.serviceCategory,"NRT_VBR");
        		break;
        	case CPE_SERVICETYPE_CBR:
        		strcpy(tdInfo.serviceCategory,"CBR");
        		break;
        	default:
        		strcpy(tdInfo.serviceCategory,"UBR");
        		break;
      }
      tdInfo.peakCellRate=tableRow->pvcPcr;
      tdInfo.sustainedCellRate=tableRow->pvcScr;
      tdInfo.maxBurstSize=tableRow->pvcMcr;

      tdId=AtmTd_setToPsiOnly(&tdInfo);

      if(tdId==0)
      {
      	    return -1;
      }

      return 0;
}

int cpeSetDhcpRelayAddr(struct in_addr *ip)
{
    IFC_LAN_INFO lanInfo;

    BcmDb_getLanInfo(IFC_ENET_ID, &lanInfo);
    lanInfo.dhcpSrv.startAddress = *ip;
    BcmDb_setLanInfo(IFC_ENET_ID, &lanInfo);

    return 0;
}



#endif
void cpeFreeIpInfo(void) {
  pCPE_IP_TABLE_INFO ptr;

  while (pIpList->head) {
    ptr = pIpList->head;
    pIpList->head = pIpList->head->next;
    free(ptr);
  }
  pIpList->count = 0;
}

/* scan IP route by reading /proc/net/route */
int cpeScanIpTable(void)
{
    IFC_DEF_GW_INFO gtwyInfo;
    int             s;
    struct ifreq    ifrq;
    FILE           *fd;
    pCPE_IP_TABLE_INFO pEntry;
    char line[IF_LINE_LENGTH], name[64];
    int i = 1;
    u_long dummy;
    int ifType;
    int pvcIfIndex = IF_INDEX_PVC_BASE;
    int pppIfIndex = IF_INDEX_PPP_BASE;
    int ethIfIndex = IF_INDEX_ETH_BASE;
    int usbIfIndex = IF_INDEX_USB_BASE;
    int ieeeIfIndex = IF_INDEX_IEEE_802DOT1_BASE;
    int adslIfIndex = IF_INDEX_ADSL_BASE;
    int bridgeIfIndex = IF_INDEX_BRIDGE_BASE;
    int atmIfIndex = IF_INDEX_ATM_BASE;
    int ifIndex = IF_INDEX_BASE;

    WAN_CON_ID wanId;  
    WAN_CON_INFO wanInfo;
    WAN_IP_INFO ipInfo;

    int getDefaultGw = 0;
   
    /* Remove routes in current list and create a new one */
    cpeFreeIpInfo();
    int len=sizeof(ifrq.ifr_name);

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      SNMP_DEBUG("Socket open failure in ifScanInterfaces()\n");
      return -1;
    }
    if ((fd = fopen("/proc/net/dev", "r")) == NULL) {
        SNMP_DEBUG("Cannot open /proc/net/dev in ifScanInterfaces()\n");
        close(s);
        return -1; 
    }

    /*
      the file (/proc/net/dev) looks like this:
Inter-|   Receive                                                |  Transmit
 face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
    lo:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
  atm0:   93216       0    0    0    0     0          0         0    84960       0    0    0    0     0       0          0
 cpcs0:   93216       0    0    0    0     0          0         0    84960       0    0    0    0     0       0          0
  dsl0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
  eth0:   19746     163    0    0    0     0          0         0    58354     158    0    0    0     0       0          0
     */
    /* skip header 2 header lines */
    fgets(line, IF_LINE_LENGTH, fd);
    fgets(line, IF_LINE_LENGTH, fd);

    while (fgets(line, IF_LINE_LENGTH, fd)) {
      pEntry = (pCPE_IP_TABLE_INFO)calloc(1, sizeof(CPE_IP_TABLE_INFO));
      if (pEntry == NULL) {
        SNMP_DEBUG("Fail to allocate INTF_INFO in ifScanInterfaces()\n");
        close(s);
        fclose(fd);
        return -1;
      }
      memset(pEntry,0,sizeof(CPE_IP_TABLE_INFO));
      sscanf(line, "%s %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
             name, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
             &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy);

      /* name has : (i.e. br0:, eth0:) at the end, get rid of it */
      name[strlen(name)-1] = '\0'; 

      /* skipping lo and cpcs */
      if ((strcmp(name, IF_BRCM_LO_DEV_NAME) == 0)  ||
	  (strcmp(name,IF_BRCM_ATM_CPCS_DEV_NAME) == 0) ||
	  (strcmp(name,IF_BRCM_DSL_DEV_NAME) == 0) ||
	  (strcmp(name,IF_BRCM_ATM_DEV_NAME) == 0)) {
        free(pEntry);
        continue;
      }

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFADDR, &ifrq) >= 0)
        pEntry->addr = ((struct sockaddr_in*)(&ifrq.ifr_addr))->sin_addr;

#if 0
      /* incremental index starting from 0 */
      //#ifdef IP_ENTRY_INDEX_IS_INDEX
      pEntry->index = i;
      i++;
      //#endif
#else
      /* this is something we do for Huawei/Thailand-TRUE NMS;
	 the NMS expects lan IP address to be at index 0 */
      if (strncmp(name,IF_BRCM_BR_DEV_NAME,2) == 0) 
	pEntry->index = 0;
      else {
	pEntry->index = i;
	i++;
      }
#endif

      cmsUtl_strncpy(ifrq.ifr_name, name, len);
      if (ioctl(s, SIOCGIFNETMASK, &ifrq) >= 0)
        pEntry->mask = ((struct sockaddr_in*)(&ifrq.ifr_netmask))->sin_addr;

      ifType = if_type_from_name(name);

      switch (ifType)
        {
        case IF_IANAIFTYPE_ETHERNETCSMACD:
        case IF_IANAIFTYPE_SOFTWARELOOPBACK:
          if (ifType == IF_IANAIFTYPE_ETHERNETCSMACD) {
            pEntry->lowerIfIndex = ethIfIndex;
            ethIfIndex++;
          }
          break;
        case IF_IANAIFTYPE_ADSL:
          pEntry->lowerIfIndex = adslIfIndex;
          adslIfIndex++;
          break;
        case IF_IANAIFTYPE_ATM:
        case IF_IANAIFTYPE_AAL5:
          if (ifType == IF_IANAIFTYPE_ATM) {
            pEntry->lowerIfIndex = atmIfIndex;
            atmIfIndex++;
          }
          else {
            pEntry->lowerIfIndex = pvcIfIndex;
            pvcIfIndex++;
          }
          break;
        case IF_IANAIFTYPE_BRIDGE:
          pEntry->lowerIfIndex = bridgeIfIndex;
          bridgeIfIndex++;
          break;
        case IF_IANAIFTYPE_PPP:
          pEntry->lowerIfIndex = pppIfIndex;
          pppIfIndex++;
          break;
        case IF_IANAIFTYPE_USB:
          pEntry->lowerIfIndex = usbIfIndex;
          usbIfIndex++;
          break;
        case IF_IANAIFTYPE_IEEE80211:
          pEntry->lowerIfIndex = ieeeIfIndex;
          ieeeIfIndex++;
          break;
        default:
          pEntry->lowerIfIndex = ifIndex;
          ifIndex++;
        } /*switch */
      /* add interface to link list */


        memset(&wanId,0,sizeof(wanId));
        while (BcmDb_getWanInfoNext(&wanId,&wanInfo) != DB_WAN_GET_NOT_FOUND) 
        {
            BcmDb_getIpSrvInfo(&wanId, &ipInfo);
            if((ipInfo.wanAddress.s_addr ==pEntry->addr.s_addr ))
            {
                pEntry->gw.s_addr = ipInfo.wanDefaultGw.s_addr;
                getDefaultGw = 1;
                break;
            }
        } 

      if(0==getDefaultGw)
      {
      BcmDb_getDefaultGatewayInfo(&gtwyInfo);
      pEntry->gw.s_addr = gtwyInfo.defaultGateway.s_addr;
      }
      
      if(bcmIsDefaultGatewayIfName(name))
      {
        pEntry->gwOnOff= 1;
      }
      else
      {
        pEntry->gwOnOff=0;
      }
      cpeAddRouteToList(pEntry);
    }   /* while */

    fclose(fd);
    close(s);

#ifdef SNMP_DEBUG_FLAG
    pEntry = pIpList->head;
    printf("pIpList(after scan): count %d\n",pIpList->count);
    while (pEntry) {
      printf("index %d, ifIndex %d\n",pEntry->index,pEntry->lowerIfIndex);
      pEntry = pEntry->next;
    }
#endif
    return 1;
}

int cpeConfigIpTable(pCPE_IP_TABLE_INFO pIp) {
  char ifName[IFC_TINY_LEN];
  char addr[IFC_TINY_LEN], mask[IFC_TINY_LEN], gw[IFC_TINY_LEN];
#ifdef REMOVE_OLD_IPTABLE_ENTRY   
  char oldAddr[IFC_TINY_LEN], oldMask[IFC_TINY_LEN];
  char oldIfName[IFC_TINY_LEN];
#endif /* REMOVE_OLD_IPTABLE_ENTRY */
  int ifType;
  char cmdName[32];
  IFC_LAN_INFO lanInfo;
  WAN_PPP_INFO pppInfo;
  WAN_IP_INFO ipInfo;
  WAN_CON_ID wanId;
  int vpi,vci, conId;
  IFC_DEF_GW_INFO gwInfo;

  ifName[0] = '\0';

  if (ifGetNameByIfIndex(pIp->lowerIfIndex,ifName) == -1)
    /* index/intergace is not found */
    return -1;

#ifdef BUILD_SNMP_DEBUG
    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
      printf("cpeConfigIpTable(): calling cfm to configure an IP address for an interface:\n");
      printf("  index %d, lowIndex %d (ifname %s), gwOnOff %d\n",
             pIp->index, pIp->lowerIfIndex, ifName, pIp->gwOnOff);
    }
#endif

    memcpy(addr,inet_ntoa(pIp->addr),IFC_TINY_LEN);
    memcpy(mask,inet_ntoa(pIp->mask),IFC_TINY_LEN);

    if (pIp->status == CPE_ROW_STATUS_DESTROY) {
      /* remove IP information */
      ;
    }
    else 
    {
      /* ifconfig ifName ipaddr netmask mask */

      /* if gateway is nonzero, add a static route to this subnet to go to default gateway */
      sprintf(cmdName, "ifconfig %s %s netmask %s up", ifName,addr,mask);
      prctl_runCommandInShellWithTimeout(cmdName);
      gw[0]='\0';
      if (pIp->gw.s_addr != 0 ) 
      {

        memcpy(gw,inet_ntoa(pIp->gw),IFC_TINY_LEN);
        memset(&gwInfo,0,sizeof(IFC_DEF_GW_INFO));
        if (pIp->gwOnOff == 1) 
        {
        gwInfo.defaultGateway.s_addr = pIp->gw.s_addr;
        BcmDb_setDefaultGatewayInfo(&gwInfo);
          sprintf(cmdName,"route delete default");
          prctl_runCommandInShellWithTimeout(cmdName);
          sprintf(cmdName, "route add default gw %s 2>dev/null", gw);
          prctl_runCommandInShellWithTimeout(cmdName);
        }
        else 
        {
          BcmDb_getDefaultGatewayInfo(&gwInfo);
          if(gwInfo.defaultGateway.s_addr == pIp->gw.s_addr)
          {
          sprintf(cmdName, "route delete default");
          prctl_runCommandInShellWithTimeout(cmdName);
          BcmDb_removeDefaultGatewayInfo();
        }
        }
      } /* gw is set */

      /* need to call CFM to store the IP information to PSI, WAN INFO */
      /* if ifName is LAN type, store to <lan>.  If ifName is a WAN type, store
         it in the WAN_IP_INFO data structure */
      ifType = if_type_from_name(ifName);
      switch (ifType)
        {
        case IF_IANAIFTYPE_ETHERNETCSMACD:
        case IF_IANAIFTYPE_IEEE80211:
        case IF_IANAIFTYPE_USB:
          /* eth, lan  */
          BcmDb_getLanInfo(IFC_ENET_ID, &lanInfo);
          memcpy(&lanInfo.lanAddress,&pIp->addr,sizeof(lanInfo.lanAddress));
          memcpy(&lanInfo.lanMask,&pIp->mask,sizeof(lanInfo.lanMask));
          BcmDb_setLanInfo(IFC_ENET_ID, &lanInfo);
          break;
        case IF_IANAIFTYPE_PPP:
          /* ppp */
          sscanf(ifName,"ppp_%d_%d_%d",&vpi,&vci,&conId);
          BcmDb_getPppSrvInfo(&wanId, &pppInfo);
          pppInfo.useStaticIpAddress = 1;
          memcpy(&pppInfo.pppLocalIpAddress,&pIp->addr,sizeof(pppInfo.pppLocalIpAddress));
          BcmDb_setPppSrvInfo(&wanId, &pppInfo);
          break;
        default:
          /* either mer or ipoa */
          if(strstr(ifName,"nas"))
          sscanf(ifName,"nas_%d_%d",&vpi, &vci);
          else if(strstr(ifName,"ipa"))
            sscanf(ifName,"ipa_%d_%d",&vpi, &vci);
          else if(strstr(ifName,"ipoa"))
            sscanf(ifName,"ipoa_%d_%d",&vpi, &vci);
          /* TODO: for 1 protocol/PVC, connId is always 1; however, there needs to be a way to get the connId */
          wanId.vpi = vpi;
          wanId.vci = vci;
          wanId.conId = 1; 
          if (BcmDb_getIpSrvInfo(&wanId, &ipInfo) != DB_WAN_GET_OK) 
         {
            BcmMer_createDefault(&wanId);
          if (BcmDb_getIpSrvInfo(&wanId, &ipInfo) != DB_WAN_GET_OK) {
            printf("--------- cpeConfigIpTable, Failed to get BcmDb_getIpSrvInfo vpi=%d vci=%d?\n",vpi,vci);
            break;
          }
	  }
          memcpy(&ipInfo.wanAddress,&pIp->addr,sizeof(ipInfo.wanAddress));
          memcpy(&ipInfo.wanMask,&pIp->mask,sizeof(ipInfo.wanMask));
          memcpy(&ipInfo.wanDefaultGw,&pIp->gw,sizeof(ipInfo.wanDefaultGw));
          BcmDb_setIpSrvInfo(&wanId, &ipInfo);
#if 0 /* yen remove */
          // for some reason, old nas has to be removed and restarted before ping to NMS works.
          BcmMer_unInit(&wanId);
          BcmMer_init(&wanId);
          // added back default gw if exist.
          if ((pIp->gwOnOff == 1) && (pIp->gw.s_addr != 0)){
            sprintf(cmdName, "route add default gw %s 2>dev/null", gw);
            prctl_runCommandInShellWithTimeout(cmdName);
          }
#endif
          break;
        }
    } /* not destroy */
    return 1;
}   /* IP  */

int cpeGetDhcpMode(void)
{  
    IFC_LAN_INFO lanInfo;  
    BcmDb_getLanInfo(IFC_ENET_ID, &lanInfo);  
    return(lanInfo.dhcpSrv.enblDhcpSrv);
}

int cpeSetDhcpMode(int mode)
{
  IFC_LAN_INFO lanInfo;

  BcmDb_getLanInfo(IFC_ENET_ID, &lanInfo);
  
  if (mode ==  lanInfo.dhcpSrv.enblDhcpSrv)
    /* no change, do nothing */
    return 0;
  
  lanInfo.dhcpSrv.enblDhcpSrv = mode;
  BcmDb_setLanInfo(IFC_ENET_ID, &lanInfo);

  if (mode) {
    /* enable, either DHCP_SRV_ENABLE or DHCP_SRV_RELAY */
    BcmNtwk_startDhcpSrv();
  }
  else {
    /* disable, DHCP_SRV_DISABLE */
    BcmNtwk_stopDefDhcpSrv();
  }
  return 0;
}

int cpeGetDnsAddr(int type, struct in_addr *ip)
{
  char dns[IFC_TINY_LEN];

  if (type == CPE_DNS_PRIMARY)
    bcmGetDns(dns);
  else 
    bcmGetDns2(dns);
  inet_aton(dns,ip);
  return 0;
}

int cpeSetDnsAddr(int type, struct in_addr *ip)
{
  char primary[IFC_TINY_LEN], secondary[IFC_TINY_LEN];
  int mode;
  struct in_addr p, s;

  bcmGetDnsSettings(&mode,primary,secondary);

  inet_aton(primary,&p);
  inet_aton(secondary,&s);

  if (type == CPE_DNS_PRIMARY) {
    if (ip->s_addr == p.s_addr)
      /* no change, do nothing */
      return 0;
    else
      cmsUtl_strncpy(primary,inet_ntoa(*ip),IFC_TINY_LEN);
  }
  else {
    if (ip->s_addr == s.s_addr)
      /* no change, do nothing */
      return 0;
    else
      cmsUtl_strncpy(secondary,inet_ntoa(*ip),IFC_TINY_LEN);
  }
  /* mode is dynamic or static; since DNS is assigned here, mode must be static which is 0 */
  bcmConfigDns(primary,secondary,0);

  return 0;
}

int cpeStartPing(void)
{
  char cmdName[100];
  char *name;
  int len = pPingData->addrLen;

  name = (char*)malloc(len+1);
  memset(pPingData->result,0,sizeof(pPingData->result));
  cmsUtl_strncpy(name,pPingData->addr,len);
  name[len] = '\0';
  sprintf(cmdName, "ping -c %d -s %d -q -l %s &", pPingData->count,pPingData->size,name);
  prctl_runCommandInShellWithTimeout(cmdName);
  pPingData->operStatus = CPE_PING_OPER_PINGING;
  signal(SIGALRM,cpePollPingResult);
  alarm(CPE_POLL_INTERVAL);
  free(name);
  return 0;
}

int cpeGetPid(char *pidFile)
{
  int pid;
  FILE *fd;
  char line[IF_LINE_LENGTH];

  if ((fd = fopen(pidFile, "r")) == NULL) {
    return -1;
  }
  fgets(line, IF_LINE_LENGTH, fd);
  sscanf(line,"%d",&pid);
  fclose(fd);
  return pid;
}



int cpeStopPing(void)
{
  char cmdName[32];
  int pid;

  pPingData->operStatus = CPE_PING_OPER_SUCCESS;

  pid = cpeGetPid("/var/pingPid");
  if (pid != -1) {
    sprintf(cmdName, "kill -9 %d",pid);
    prctl_runCommandInShellWithTimeout(cmdName);
    return 0;
  }
  else
    return -1;
}

void cpePollPingResult(int junk)
{
  FILE           *fd;
  char line[IF_LINE_LENGTH];
  int finish=0, len;
  long receive;
  unsigned long dummy;
  char ip[16];
  int pid, dead = 0;
  /* first check to see if the process dies */
  pid = cpeGetPid("/var/pingPid");

  /* when pid is -1, it means ping process exited after it finishes ping job,
     when dead pid is not -1 because pidFile still exists */  
  if (pid != -1) {
    if ((getpgid(pid)) && (errno == ESRCH)) {
      /* the process is dead */
      dead = 1;
    }
  }

  /* ping results are written by ping_main everytime it gathers ping statistics */
  /* it will not be available if there's problem with the host address, or there is
   some problem with creating the pingStats file */
  if ((fd = fopen("/var/pingStats", "r")) == NULL) {
#if 0
    SNMP_DEBUG("Cannot open /var/pingResult in cpePollPingResult()\n");
      pPingData->adminStatus = CPE_PING_ADMIN_STOP;
      pPingData->operStatus = CPE_PING_OPER_FAILURE;
      sprintf(pPingData->result,
              "Ping test result: IP = %s Sent = %d Received = %d Lost = %d Min = %d ms Max = %d ms Average = %d ms", pPingData->addr,0,0,0,0,0,0);
      cpeSendPingResult();
#endif
      if (access("/var/pingPid",F_OK) == 0)
        unlink("/var/pingPid");
      if (dead) {
        /* process is dead and pingStats is not available, something is really wrong */
        if (access("/var/pingStats",F_OK) == 0)
          unlink("/var/pingStats");
      }
      else
        alarm(CPE_POLL_INTERVAL);
  }
  else {
    /* first line is 0 or 1; 0=incomplete, 1= complete */
    if (fgets(line, IF_LINE_LENGTH, fd) == NULL) {
      fclose(fd);
      unlink("/var/pingStats");
      sprintf(pPingData->result,
              "Ping test result: IP = %s Sent = %d Received = %d Lost = %d Min = %d ms Max = %d ms Average = %d ms", pPingData->addr,0,0,0,0,0,0);
      pPingData->operStatus = CPE_PING_OPER_FAILURE;
      cpeSendPingResult();
      return;
    }
    sscanf(line,"%d",&finish);
    
    /* IP, sent, receive, lost, min, max, average */
    fgets(line, IF_LINE_LENGTH, fd);
    fclose(fd);
    len = strlen(line);
    cmsUtl_strncpy(pPingData->result,line,len);
    sscanf(line,"IP = %s Sent = %ld Receive = %ld Lost = %ld Min = %lu.%lu ms Max =  %lu.%lu Average = %lu.%lu\n",
           ip, &dummy, &receive, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy);
    if (finish == 0) {
      if (dead) {
        /* user stop this, do cleanup */
        if (access("/var/pingStats",F_OK) == 0)
          unlink("/var/pingStats");
        if (access("/var/pingPid",F_OK) == 0)
          unlink("/var/pingPid");
      }
      else
        alarm(CPE_POLL_INTERVAL);
    }
    else {
      pPingData->adminStatus = CPE_PING_ADMIN_STOP;
      if ((receive == 0) || (finish == -1))
        pPingData->operStatus = CPE_PING_OPER_FAILURE;
      else
        pPingData->operStatus = CPE_PING_OPER_SUCCESS;
      cpeSendPingResult();
      
      if (access("/var/pingStats",F_OK) == 0)
        unlink("/var/pingStats");
      if (access("/var/pingPid",F_OK) == 0) 
        unlink("/var/pingPid");
    }
  } /* pingStat available */
}

void cpeSendPingResult(void)
{
  /* send trap to managment */
  printf("cpeSendPingResult(entry):\n");

  cpeSendPingDoneTrap(1);
  return;
}
#endif

int cpeGetPid(char *pidFile)
{
  int pid;
  FILE *fd;
  char line[IF_LINE_LENGTH];

  if ((fd = fopen(pidFile, "r")) == NULL) {
    return -1;
  }
  fgets(line, IF_LINE_LENGTH, fd);
  sscanf(line,"%d",&pid);
  fclose(fd);
  return pid;
}

void cpeFtpStop(void)
{
  /* stop whatever we are doing, either upgrade or test */
#ifdef BUILD_SNMP_DEBUG
  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
    printf("cpeFtpStop()\n");
  }
#endif
  pFtpData->adminStatus = CPE_FTP_ADMIN_STATUS_STOP;
}

void cpeFtpUpgrade(pCPE_FTP_INFO pFtpData)
{
  /* do image upgrade, when this is called, FileName and server IP address are specified */
  /* receive the file, save the image to flash  */
  /* send the FTP result and then reboot */
  char cmd[512];
#ifdef BUILD_SNMP_DEBUG
  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
    printf("cpeFtpUpgrade()\n");
  }
#endif
  pFtpData->adminStatus = CPE_FTP_ADMIN_STATUS_UPGRADE;
  sprintf(cmd, "ftpget -v -f -u %s -p %s %s %s %s &", \
      pFtpData->user, pFtpData->password, inet_ntoa(pFtpData->addr), pFtpData->filename, pFtpData->filename);
  prctl_runCommandInShellWithTimeout(cmd);
  signal(SIGALRM,cpePollFtpResult);
  alarm(CPE_POLL_INTERVAL);
}

void cpeFtpTest(pCPE_FTP_INFO pFtpData)
{
  /* do test, when this is call, FileName and server IP address are specified */
  /* just receive the file, and dump all the data, just maintain all the statistics */
  char cmd[512];

#ifdef BUILD_SNMP_DEBUG
  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
    printf("======>  cpeFtpTest pFtpData->filename=%s, pFtpData->user=%s, pFtpData->password=%s, pFtpData->ip=%s\n",
           pFtpData->filename, pFtpData->user, pFtpData->password, inet_ntoa(pFtpData->addr));
  }
#endif
  pFtpData->adminStatus = CPE_FTP_ADMIN_STATUS_FTPTEST;
  pFtpData->totalSize = 0;
  pFtpData->doneSize = 0;
  pFtpData->elapseTime = 0;
  pFtpData->operStatus = CPE_FTP_OPER_STATUS_DOWNLOADING;
  sprintf(cmd, "ftpget -v -u %s -p %s %s %s %s &", \
      pFtpData->user, pFtpData->password, inet_ntoa(pFtpData->addr), pFtpData->filename, pFtpData->filename);
  prctl_runCommandInShellWithTimeout(cmd);
  signal(SIGALRM,cpePollFtpResult);
  alarm(CPE_POLL_INTERVAL);
}

void cpePollFtpResult(int junk)
{
  FILE *fd;
  char line[IF_LINE_LENGTH];
  int pid, dead = 0;

  /* first check to see if the process dies */
  pid = cpeGetPid("/var/ftpPid");

  /* when pid is -1, it means ftp process exited after it finishes the job,
     when dead pid is not -1 because pidFile still exists */  
  if (pid != -1) {
    if ((getpgid(pid)) && (errno == ESRCH)) {
      /* the process is dead */
      dead = 1;
    }
  }

  if ((fd = fopen("/var/ftpStats", "r")) == NULL) {
    SNMP_DEBUG("Cannot open /var/ftpStats in cpePollFtpResult()\n");
    pFtpData->operStatus = CPE_FTP_OPER_STATUS_DOWNLOAD_FAILURE;
    cpeSendFtpResult();
    if (access("/var/ftpPid",F_OK) == 0)
      unlink("/var/ftpPid");
    if (dead) {
      /* process is dead and pingStats is not available, something is really wrong */
      if (access("/var/ftpStats",F_OK) == 0)
        unlink("/var/ftpStats");
    }
  }
  else {
    // one line:  operStatus = 5 doneSize = 18375720 elapseTime = 4
    fgets(line, IF_LINE_LENGTH, fd);
    fclose(fd);
    sscanf(line, "operStatus = %d totalSize = %d doneSize = %d elapseTime = %d\n",
        &(pFtpData->operStatus), &(pFtpData->totalSize), &(pFtpData->doneSize), &(pFtpData->elapseTime));
    //printf("operStatu=%d, totalSize=%d, doneSize=%d, elapseTime=%d\n", pFtpData->operStatus, pFtpData->totalSize, //pFtpData->doneSize, pFtpData->elapseTime);
    // if not done, set alarm again until operStatus is not one following
    if (pFtpData->operStatus == CPE_FTP_OPER_STATUS_NORMAL ||
        pFtpData->operStatus == CPE_FTP_OPER_STATUS_DOWNLOADING ||
        pFtpData->operStatus == CPE_FTP_OPER_STATUS_SAVING) {
      if (dead) {
        /* user stop this, do cleanup */
        if (access("/var/ftpStats",F_OK) == 0)
          unlink("/var/ftpStats");
        if (access("/var/ftpPid",F_OK) == 0)
          unlink("/var/ftpPid");
      }
      else
        alarm(CPE_POLL_INTERVAL);
        //printf("operstat not ready = %d, dead=%d\n", pFtpData->operStatus, dead);
    }
    else { // done ftp
      //printf("operstat ready = %d, dead=%d\n", pFtpData->operStatus, dead);
      cpeSendFtpResult();
      if (access("/var/ftpPid",F_OK) == 0)
        unlink("/var/ftpPid");
      if (access("/var/ftpStats",F_OK) == 0)
        unlink("/var/ftpStats");
    }
  }
}

void cpeSendFtpResult(void)
{
  /* send trap to managment */
#ifdef BUILD_SNMP_DEBUG
  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
    printf("cpeSendFtpResult(entry):\n");
  }
#endif
  cpeSendFtpDoneTrap(1);

  return;
}

void cpeGetSnmpRoCommunity(char *com) 
{
   SnmpCfgObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;


   if (cmsObj_get(MDMOID_SNMP_CFG, &iidStack, 0, (void **) &obj) == CMSRET_SUCCESS)
   {
      cmsUtl_strncpy(com, obj->ROCommunity, strlen(obj->ROCommunity));
      cmsObj_free((void **) &obj);
   }
   else
   {
      cmsLog_error("Unable to get MDMOID_SNMP_CFG");
   }
}

#ifdef LGD_TO_DO
void cpeSendPppResult(int result)
{
#ifdef BUILD_SNMP_DEBUG
  if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
    /* send trap to managment */
    printf("cpeSendPppResult(entry):\n");
  }
#endif
  cpeSendPppTestDoneTrap(result);
  return;
}

void cpeGetSnmpRoCommunity(char *com) 
{
   SnmpCfgObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;


   if (cmsObj_get(MDMOID_SNMP_CFG, &iidStack, 0, (void **) &obj) == CMSRET_SUCCESS)
   
      cmsUtl_strncpy(com, obj->ROCommunity, strlen(obj->ROCommunity));
      cmsObj_free((void **) &obj);
   }
   else
   {
      cmsLog_error("Unable to get MDMOID_SNMP_CFG");
   }
}


/* this routine is called when PPP Test is initiated.   It polls the status of the 
PPP connection, every p=10 seconds interval. 
It would declare the test failed if PPP connection is not established
after X=5 minutes. */
void cpePollPppTestResult(int junk) {
  WAN_CON_ID wanId;
  int status;
  int testResult = CPE_PPP_TEST_RESULT_TESTING;

  wanId.vpi = pPppTestData->vpi;
  wanId.vci = pPppTestData->vci;
  wanId.conId = pPppTestData->conId;

  status = BcmWan_getStatus(&wanId);

#ifdef BUILD_SNMP_DEBUG
  printf("cpePollPppTestResult(): status %d, IFC_STATUS_UP %d, IFC_STATUS_AUTH_ERR %d, IFC_STATUS_NEW %d, IFC_STATUS_DOWN %d\n", 
	 status, IFC_STATUS_UP,IFC_STATUS_AUTH_ERR,IFC_STATUS_NEW,IFC_STATUS_DOWN);
#endif

  switch (status)
    {
    case IFC_STATUS_UP:
      testResult = CPE_PPP_TEST_RESULT_SUCCESS;
      break;
    case IFC_STATUS_AUTH_ERR:
      testResult = CPE_PPP_TEST_RESULT_AUTHFAIL;
      break;
    case IFC_STATUS_NEW:
    case IFC_STATUS_DOWN:
    default:
      testResult = CPE_PPP_TEST_RESULT_TESTING;
      break;
    }
  if ((testResult == CPE_PPP_TEST_RESULT_SUCCESS) ||
      (pPppTestData->pollCount >= CPE_PPP_TEST_TIMEOUT_PERIOD) ||
      (pPppTestData->stop)) {
    if (testResult != CPE_PPP_TEST_RESULT_SUCCESS)
      testResult = CPE_PPP_TEST_RESULT_FAILURE;
    cpeSendPppResult(testResult);
    pPppTestData->testResult = testResult;
#ifdef ZTE_NMS /* Huawei NMS doesn't want it down */
    /* after sending trap, bring connection down */
    BcmPppoe_stopPppd(&wanId);
#endif 
  }
  else {
    pPppTestData->pollCount++;
    signal(SIGALRM,cpePollPppTestResult);
    alarm(CPE_PPP_TEST_POLL_INTERVAL);
  }
}

int cpeStopPppMonitor(WAN_CON_ID *wanId)
{
  pPppTestData->stop = 1;
  return 0;
}

int cpeStartPppMonitor(WAN_CON_ID *wanId)
{
#ifdef BUILD_SNMP_DEBUG
  printf("cpeStartPppMonitor(): wanId->vpi/vci %d/%d\n",wanId->vpi,wanId->vci);
#endif

  pPppTestData->pollCount = 0;
  pPppTestData->stop = 0;
  pPppTestData->vpi = wanId->vpi;
  pPppTestData->vci = wanId->vci;
  pPppTestData->conId = wanId->conId;
  pPppTestData->testResult = CPE_PPP_TEST_RESULT_TESTING;
  signal(SIGALRM,cpePollPppTestResult);
  alarm(CPE_PPP_TEST_POLL_INTERVAL);
  return 0;
}

/* input: pPvcParam contains values set by user, pEntry is current parameter in PSI,
   output: param is the default based on protocol mode + value sets by user */
void cpeProto_createDefaultAndSet(pCPE_PVC_ENTRY param, int copy)
{
  WAN_CON_ID wanId;
  WAN_CON_INFO wanInfo;
  WAN_IP_INFO ipInfo;
  char ifName[IFC_TINY_LEN];
  CPE_PVC_ENTRY entry;

  wanId.vpi = param->vpi;
  wanId.vci = param->vci;
  wanId.conId = param->conId;

  switch (param->upperEncap)
    {
    case CPE_PVC_ENCAP_PPPOE:
      BcmPppoe_createDefault(&wanId);
      break;
    case CPE_PVC_ENCAP_PPPOA:
      BcmPppoa_createDefault(&wanId);
      break;
    case CPE_PVC_ENCAP_BRIDGE2684IP:
      BcmMer_createDefault(&wanId);
      break;
    case CPE_PVC_ENCAP_ROUTER2684:
      BcmIpoa_createDefault(&wanId);
      break;
    default:
      break;
      /* do nothing, already created as bridge by default */
    }

#ifdef BUILD_SNMP_DEBUG
  printf("cpeProto_createDefaultAndSet(): wanId vpi/vci/conId %d/%d/%d\n",
	 wanId.vpi, wanId.vci,wanId.conId);
#endif

  BcmDb_getWanInfo(&wanId,&wanInfo);
  entry.ifIndex = ifGetIfIndexByName(ATM_CELL_LAYER_IFNAME);
  sprintf(ifName,"nas_%d_%d",wanId.vpi,wanId.vci);
  entry.pvcIfIndex = ifGetIfIndexByName(ifName);
  entry.vpi = wanId.vpi;
  entry.vci = wanId.vci;
  entry.conId = wanId.conId;
  entry.pvcId = wanInfo.vccId;
  if (wanInfo.flag.service)
    entry.adminStatus = CPE_ADMIN_ENABLE; 
  else 
    entry.adminStatus = CPE_ADMIN_DISABLE; 
  if (wanInfo.flag.igmp)
    entry.igmpMode = CPE_ADMIN_ENABLE;
  else
    entry.igmpMode = CPE_ADMIN_DISABLE;
  if (wanInfo.flag.nat)
    entry.natMode = CPE_ADMIN_ENABLE;
  else
    entry.natMode = CPE_ADMIN_DISABLE;
  if (wanInfo.flag.bridgeMode)
    entry.bridgeMode = CPE_ADMIN_ENABLE; 
  else 
    entry.bridgeMode = CPE_ADMIN_DISABLE; 
  if (wanInfo.flag.macLearnMode)
    entry.macLearnMode = CPE_ADMIN_ENABLE; 
  else 
    entry.macLearnMode = CPE_ADMIN_DISABLE; 
  if (wanInfo.flag.bridgeMode)
    entry.bridgeMode = CPE_ADMIN_ENABLE; 
  else 
    entry.bridgeMode = CPE_ADMIN_DISABLE; 
  if (wanInfo.flag.macLearnMode)
    entry.macLearnMode = CPE_ADMIN_ENABLE; 
  else 
    entry.macLearnMode = CPE_ADMIN_DISABLE;
  BcmDb_getIpSrvInfo(&wanId, &ipInfo);
  if (ipInfo.enblDhcpClnt)
    entry.dhcpcMode = CPE_ADMIN_ENABLE;
  else
    entry.dhcpcMode = CPE_ADMIN_DISABLE;

  /* now copy the set parameter over */
  if ((copy & CPE_PVC_ADMIN_SET) == CPE_PVC_ADMIN_SET)
    entry.adminStatus = param->adminStatus;
  if ((copy & CPE_PVC_BRIDGE_MODE_SET) == CPE_PVC_BRIDGE_MODE_SET)
    entry.bridgeMode = param->bridgeMode;
  if ((copy & CPE_PVC_MAC_LEARN_SET) == CPE_PVC_MAC_LEARN_SET)
    entry.macLearnMode = param->macLearnMode;
  if ((copy & CPE_PVC_IGMP_MODE_SET) == CPE_PVC_IGMP_MODE_SET)
    entry.igmpMode = param->igmpMode;
  if ((copy & CPE_PVC_NAT_SET) == CPE_PVC_NAT_SET)
    entry.natMode = param->natMode;
  if ((copy & CPE_PVC_DHCP_CLIENT_SET) == CPE_PVC_DHCP_CLIENT_SET)
    entry.dhcpcMode = param->dhcpcMode;
  if ((copy & CPE_PVC_ENCAP_SET) == CPE_PVC_ENCAP_SET)
    entry.upperEncap = param->upperEncap;

  memcpy(param,&entry,sizeof(CPE_PVC_ENTRY));
}
#endif
