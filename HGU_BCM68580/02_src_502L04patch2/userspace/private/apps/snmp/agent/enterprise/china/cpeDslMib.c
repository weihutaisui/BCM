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
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>

/* SNMP includes */
#include "asn1.h"
#include "snmp.h"
#include "agtMib.h"
#include "agtEngine.h"
#include "cpeDslMib.h"

#include "cpeDslMibHelper.h"
#include "cpeDslMib.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

#ifdef LGD_TO_DO
#include "AdslMibDef.h"
#include "adslMibHelper.h"
#include "cpeDslMibHelper.h"
#include "cpeDslMib.h"
#include "ifMib.h"
#endif

#ifdef BUILD_SNMP_DEBUG
#include "snmpDebug.h"
extern SNMP_DEBUG_FLAG snmpDebug;
#endif

#ifdef LGD_TO_DO
#include "board_api.h"
#endif

#ifdef LGD_TO_DO
pCPE_SYS_INFO pSysInfo;
pWAN_INFO_LIST pWanList;
pPPP_INFO_LIST pPppList;
pCPE_IP_TABLE_LIST pIpList;
#ifdef CT_SNMP_MIB_VERSION_2
pCPE_ETHERNETPORT_TABLE_LIST pEthernetPortList;
pCPE_SERVICETYPE_TABLE_LIST pServiceTypeList;
#endif
pCPE_PING_INFO pPingData;
#endif

pCPE_FTP_INFO pFtpData;

#ifdef LGD_TO_DO
static int trapRetry=0;
pCPE_PVC_ENTRY pPvcParam=NULL;
pPPP_REFERENCE_LIST pPppReferenceList;
pCPE_PPPTEST_INFO pPppTestData=NULL;
#endif

#ifdef LGD_TO_DO
int setAdslLineOperation = 0;
int gAdslLinePara = 0;
int modTypeAdslLineHandshake = 0;
int modTypeAdslLineTrellis = 0;
int modTypeAdslLineBitSwapping = 0; 

int cpeGetInt(char *var_val, int len) 
{
  int i;
  int value = *(int*)var_val;

  for (i = 0; i < (sizeof(int)-len); i++)
    value = value >> 8;
  return (value);
}
#endif

void cpeDslMibFree(void)
{
  if (pFtpData)
    free(pFtpData);

#ifdef LGD_TO_DO
  if (pSysInfo)
    free(pSysInfo);
  cpeFreeWanInfo();
  if (pWanList)
    free(pWanList);
  cpeFreePppInfo();
  if (pPppList)
    free(pPppList);
  cpeFreeIpInfo();
  if (pIpList)
    free(pIpList);
  if (pPingData)
    free(pPingData);
  if (pPppReferenceList) {
    cpeFreeReferenceList();
    free(pPppReferenceList);
  }
  if (pPppTestData)
    free(pPppTestData);
#ifdef CT_SNMP_MIB_VERSION_2
  if(pServiceTypeList)
    free(pServiceTypeList);
  if(pEthernetPortList)
    free(pEthernetPortList);
#endif
#endif

}

/* CPE_DSL_MIB initialisation (must also register the MIB module tree) */
void init_cpe_dsl_mib()
{

#ifdef LGD_TO_DO
  adslVersionInfo adslVer;

  register_subtrees_of_cpe_dsl_mib();

  trapRetry = 0;
  pSysInfo = (pCPE_SYS_INFO)malloc(sizeof(CPE_SYS_INFO));
  if (pSysInfo == NULL)
    return;
  memset(pSysInfo,0,sizeof(CPE_SYS_INFO));

  cpeGetConfigId(pSysInfo->configId, 0);

  BcmAdsl_GetVersion(&adslVer);
  sprintf(pSysInfo->version, "BCM%x%X %s-%s", sysGetChipId(), sysGetChipRev(), CMS_RELEASE_VERSION, adslVer.phyVerStr);
  //cmsUtl_strncpy(pSysInfo->sysVersion, pSysInfo->version, strlen(pSysInfo->version));
  strcpy(pSysInfo->mibVersion,CPE_MIB_VERSION);
  if (cpeGetStoredSnmpParam(pSysInfo) == -1) {
      printf("Failed to get snmp params\n");
      exit(0);
  }
  pSysInfo->trapResponseState = CPE_BOOTTRAP_NO_RESPONSE;

#ifdef CT_SNMP_MIB_VERSION_2
        pSysInfo->sysInfoType=sysGetNumEnetPorts()==1?1:3;
#endif

  if ((pWanList = malloc(sizeof(WAN_INFO_LIST))) == NULL) {
    printf("init_cpe_dsl_mib(): unable to allocation WAN INFO LIST\n");
    exit(0);
  } 
  memset(pWanList,0,sizeof(WAN_INFO_LIST));

  if ((pPppList = malloc(sizeof(PPP_INFO_LIST))) == NULL) {
    printf("init_cpe_dsl_mib(): unable to allocation PPP INFO LIST\n");
    exit(0);
  } 
  memset(pPppList,0,sizeof(PPP_INFO_LIST));

  if ((pIpList = (pCPE_IP_TABLE_LIST)malloc(sizeof(CPE_IP_TABLE_LIST))) == NULL) {
    printf("init_cpe_dsl_mib(): unable to allocation IP TABLE LIST\n");
    exit(0);
  } 
  memset(pIpList,0,sizeof(CPE_IP_TABLE_LIST));

#ifdef CT_SNMP_MIB_VERSION_2
  if ((pServiceTypeList = (pCPE_SERVICETYPE_TABLE_LIST)malloc(sizeof(CPE_SERVICETYPE_TABLE_LIST))) == NULL) {
    printf("init_cpe_dsl_mib(): unable to allocation SERVICETYPE TABLE LIST\n");
    exit(0);
  } 
  memset(pServiceTypeList,0,sizeof(CPE_SERVICETYPE_TABLE_LIST));

  if ((pEthernetPortList = (pCPE_ETHERNETPORT_TABLE_LIST)malloc(sizeof(CPE_ETHERNETPORT_TABLE_LIST))) == NULL) {
    printf("init_cpe_dsl_mib(): unable to allocation ETHERNETPORT TABLE LIST\n");
    exit(0);
  } 
  memset(pEthernetPortList,0,sizeof(CPE_ETHERNETPORT_TABLE_LIST));
#endif

  if ((pPingData = (pCPE_PING_INFO)malloc(sizeof(CPE_PING_INFO))) == NULL) {
    printf("init_cpe_dsl_mib(): unable to allocation PING INFO\n");
    exit(0);
  } 
  cpeInitPingData();

  if ((pFtpData = (pCPE_FTP_INFO)malloc(sizeof(CPE_FTP_INFO))) == NULL) {
    printf("init_cpe_dsl_mib(): unable to allocation FTP INFO\n");
    exit(0);
  } 
  cpeInitFtpData();
  if ((pPppReferenceList = (pPPP_REFERENCE_LIST)malloc(sizeof(PPP_REFERENCE_LIST)))
      == NULL) {
    printf("init_cpe_dsl_mib(): unable to allocation PPP reference list\n");
    exit(0);
  } 
  memset(pPppReferenceList,0,sizeof(PPP_REFERENCE_LIST));
  if ((pPppTestData = (pCPE_PPPTEST_INFO)malloc(sizeof(CPE_PPPTEST_INFO))) == NULL) {
    printf("init_cpe_dsl_mib(): unable to allocation PPP Test INFO\n");
    exit(0);
  } 
  memset(pPppTestData,0,sizeof(CPE_PPPTEST_INFO));
  pPppTestData->testResult = CPE_PPP_TEST_RESULT_SUCCESS;

#else

  register_subtrees_of_cpe_dsl_mib();
  if ((pFtpData = (pCPE_FTP_INFO)malloc(sizeof(CPE_FTP_INFO))) == NULL) 
  {
    printf("init_cpe_dsl_mib(): unable to allocation FTP INFO\n");
    exit(0);
  } 
  cpeInitFtpData();

#endif
}

#ifdef LGD_TO_DO

/* 
 * It is the working time since CPE rebooted. 
 */
unsigned char *
var_cpeSysInfoUptime(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  FILE           *fd;
  u_long sec, nsec;
  
  if ((fd = fopen("/proc/uptime", "r")) == NULL) {
    SNMP_DEBUG("Cannot open /proc/uptime in var_cpeSysInfoUptime()\n");
    return (unsigned char *) NO_MIBINSTANCE;
  }
  fscanf(fd,"%ld.%ld",&sec,&nsec);
  fclose(fd);
  
  *var_len = sizeof(long);
  long_return = sec;
  return (unsigned char *)&long_return;
}

#define NMS_ZTE 1

int g_HW_NMS = 0;
/* 
 * for synchronizing configuration by network manager.
 */
int write_cpeSysInfoConfigId(int action, unsigned char *var_val, unsigned char varval_type, int var_val_len,
                             unsigned char *statP, oid *name, int name_len)
{
  switch (action) 
    {
    case RESERVE1:
      if (varval_type != SNMP_STRING)
        return SNMP_ERROR_WRONGTYPE;
      if (var_val_len > CPE_CONFIG_ID_MAX_LEN) 
        return SNMP_ERROR_WRONGLENGTH;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      memset(pSysInfo->configId,0,CPE_CONFIG_ID_MAX_LEN);
      memcpy(pSysInfo->configId,var_val,CPE_CONFIG_ID_MAX_LEN);

#ifdef BUILD_SNMP_DEBUG
      printf("write_cpeSysInfoConfigId(): var_val_len %d, input %2x%2x%2x%2x\n",
             var_val_len,pSysInfo->configId[0],pSysInfo->configId[1],
             pSysInfo->configId[2],pSysInfo->configId[3]);
#endif
      cpeSetConfigId(pSysInfo->configId);
      g_HW_NMS = NMS_ZTE;
    case FREE:
      break;
    }
    return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpeSysInfoConfigId(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
                       
{
  *write_method = (WRITE_METHOD)write_cpeSysInfoConfigId;

  cpeGetConfigId(pSysInfo->configId, 0);
  *var_len = sizeof(pSysInfo->configId);
  return (unsigned char *)(pSysInfo->configId);
}

/* 
 * Chipset version information.
 * 
 */
unsigned char *
var_cpeSysInfoFirmwareVersion(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *var_len = strlen(pSysInfo->version);
  return (unsigned char *)(pSysInfo->version);
}

/* 
 * CPE product module, For example, the 'MT800' is the product id.
 * 
 */
unsigned char *
var_cpeSysInfoProductId(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *var_len = strlen(pSysInfo->prodId);
  return (unsigned char *)(pSysInfo->prodId);
}

/* 
 * the name of CPE vendor.
 * 
 */
unsigned char *
var_cpeSysInfoVendorId(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
        
{
  *var_len = strlen(pSysInfo->vendorId);
  return (unsigned char *)(pSysInfo->vendorId);
}

/* 
 * CPEs system version
 * 
 */
unsigned char *
var_cpeSysInfoSystemVersion(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *var_len = strlen(pSysInfo->sysVersion);
  return (unsigned char *)(pSysInfo->sysVersion);
}

/* 
 * CPEs work mode. 
 */
unsigned char *
var_cpeSysInfoWorkMode(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  /* Set size (in bytes) and return address of the variable */
  *var_len = sizeof(long);
  long_return = CPE_ROUTER_MODE;
  return (unsigned char *)&long_return;
}

/* 
 * The MIB's version, it would be updated by CCSA  for emendation.
 * Now ,It is the V1.00 
 */
unsigned char *
var_cpeSysInfoMibVersion(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *var_len = strlen(pSysInfo->mibVersion);
  return (unsigned char *)(pSysInfo->mibVersion);
}

/* 
 * The CPE's BAR CODE including serial number to identify itself to each other
 * it is embedded by the equipment provider while manufacturing .
 */
unsigned char *
var_cpeSysInfoSerialNumber(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *var_len = strlen(pSysInfo->serialNum);
  return (unsigned char *)(pSysInfo->serialNum);
}

/* 
 * the OUI Organizationally Unique Identifier of IEEE standard organization  
 * provided 
 */
unsigned char *
var_cpeSysInfoManufacturerOui(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *var_len = sizeof(pSysInfo->oui);
  return (unsigned char *)(pSysInfo->oui);
}

#ifdef CT_SNMP_MIB_VERSION_2
/*
*
*/
unsigned char *
var_cpeSysInfoType(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *var_len = sizeof(long);
  return (unsigned char *)&pSysInfo->sysInfoType;
}

#endif
/* 
 * Rebooting CPEs operation
 */
int write_cpeSystemReboot(int action, unsigned char *var_val, unsigned char varval_type,
                          int var_val_len, unsigned char *statP, oid *name,
                          SNMP_SET_HANDLER *set_handler, unsigned long *param)
{
  switch (action)
    {
    case RESERVE1:
      if (varval_type != SNMP_INTEGER)
        return SNMP_ERROR_WRONGTYPE;
      if ((int)*var_val != 1) 
        return SNMP_ERROR_WRONGVALUE;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      *set_handler = cpeReboot;
      *param = (unsigned long) NULL;
      break;
    case FREE:
      break;
    }
  return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpeSystemReboot(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpeSystemReboot;
  long_return = 0;
  *var_len = sizeof(long);
  return (unsigned char *)&long_return;
}

/* 
 * saving the CPEs configuration to NVRAM
 */
int write_cpeSystemSave(int action, unsigned char *var_val, unsigned char varval_type, int var_val_len,
                        unsigned char *statP, oid *name, SNMP_SET_HANDLER *set_handler,
                        unsigned long *param)
{
  switch (action) 
    {
    case RESERVE1:
      if (varval_type != SNMP_INTEGER)
        return SNMP_ERROR_WRONGTYPE;
      if ((int)*var_val != 1) 
        return SNMP_ERROR_WRONGVALUE;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      if( NMS_ZTE != g_HW_NMS )
            cpeGetConfigId(pSysInfo->configId,1); 
      *set_handler = cpeSave;
      *param = (unsigned long) NULL;
      break;
    case FREE:
      break;
    }
    return SNMP_ERROR_NOERROR;
}

unsigned char *var_cpeSystemSave(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpeSystemSave;
  long_return = 0;
  *var_len = sizeof(long);
  return (unsigned char *)&long_return;
}

/* 
 * Restore CPEs configuration:
 * restoreToNull(1), clear all of the configuration
 * restoreToDefault(2), restore to default configuration
 */
int write_cpeSystemRestore(int action, unsigned char *var_val, unsigned char varval_type, int var_val_len,
                           unsigned char *statP, oid *name, SNMP_SET_HANDLER *set_handler,
                           unsigned long *param)
{
  int value;
	
  value = (int)*var_val;

  switch (action)
    {
    case RESERVE1:
      if (varval_type != SNMP_INTEGER)
        return SNMP_ERROR_WRONGTYPE;
      if (((int)*var_val != CPE_RESTORE_TO_NULL) && ((int)*var_val != CPE_RESTORE_TO_DEFAULT))
        return SNMP_ERROR_WRONGVALUE;
      break;      
    case RESERVE2:
      break;
    case COMMIT:
      switch(value)
      {
        case CPE_SYSTEM_RESTORE_TO_NULL:
        {
          *set_handler = cpeRestoreToNull;
          *param = (unsigned long) NULL;
          break;
        }
        case CPE_SYSTEM_RESTORE_TO_DEFAULT:
        {
          *set_handler = cpeRestoreToDefault;
          *param = (unsigned long) NULL;
          break;
        }
      }
      break;
    case FREE:
      break;
    }
    return SNMP_ERROR_NOERROR;
}

unsigned char *var_cpeSystemRestore(int *var_len, snmp_info_t *mesg,
                                    WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpeSystemRestore; 
  *var_len = sizeof(long);
  long_return = 0;
  return (unsigned char *)&long_return;
}

/* 
 * trap notification switch, if set to Disable
 * CPE will not send trap notification untill it is set to enable       
 */
int write_cpeSystemTrapEnable(int action, unsigned char *var_val, unsigned char varval_type, int var_val_len,
                              unsigned char *statP, oid *name, int name_len)
{
    switch (action)
      {
      case RESERVE1:
        if (varval_type != SNMP_INTEGER)
          return SNMP_ERROR_WRONGTYPE;
        if (((int)*var_val != CPE_ADMIN_ENABLE) && ((int)*var_val != CPE_ADMIN_DISABLE))
          return SNMP_ERROR_WRONGVALUE;
        break;
      case RESERVE2:
        break;
      case COMMIT:
        pSysInfo->trap = *var_val;
        cpeSaveTrap(pSysInfo->trap);
        break;
      case FREE:
        break;
      }
    return SNMP_ERROR_NOERROR;
}


unsigned char *var_cpeSystemTrapEnable(int *var_len, snmp_info_t *mesg,
                                       WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpeSystemTrapEnable;
  *var_len = sizeof(long);
  return (unsigned char *)&pSysInfo->trap;
}

/* 
 * It is the boottrap response identity.
 * Every time,when CPE is rebooted, it is set to noResponse 
 */
int write_cpeBootTrapResponse(int action, unsigned char *var_val, unsigned char varval_type, int var_val_len,
                              unsigned char *statP, oid *name, int name_len)
{
  switch (action)
    {
    case RESERVE1:
      if (varval_type != SNMP_INTEGER)
        return SNMP_ERROR_WRONGTYPE;
      if (((int)*var_val < CPE_BOOTTRAP_NO_RESPONSE) && ((int)*var_val > CPE_BOOTTRAP_RECFG))
        return SNMP_ERROR_WRONGVALUE;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      pSysInfo->trapResponseState = *var_val;
      break;
    case FREE:
      break;
    }
    return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpeBootTrapResponse(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpeBootTrapResponse;
  *var_len = sizeof(long);
  return (unsigned char *)&pSysInfo->trapResponseState;
}

/* 
 * It is the reserved string 
 */
int write_cpeReservedString(int action, unsigned char *var_val, unsigned char varval_type, int var_val_len,
                            unsigned char *statP, oid *name, int name_len)
{
    switch (action) 
      {
      case RESERVE1:
        if (varval_type != SNMP_STRING)
            return SNMP_ERROR_WRONGTYPE;
        if ((var_val_len < 0) ||
            (var_val_len > MAX_OCTSTR_LEN))
          return SNMP_ERROR_WRONGLENGTH;
        break;
      case RESERVE2:
        break;
      case COMMIT:
        memset(return_buf,0,MAX_OCTSTR_LEN);
        cmsUtl_strncpy(return_buf,var_val,var_val_len);
        break;
      case FREE:
        break;
      }
    return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpeReservedString(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpeReservedString;
  *var_len = 0;
  return_buf[0] = '\0';
  return (unsigned char *)return_buf;
}

/* 
 * trellis encoding configuration. 
 */
int write_cpeConfigAdslLineTrellis(int action, unsigned char *var_val, unsigned char varval_type, int var_val_len,
                                   unsigned char *statP, oid *name, int name_len)
{
  int error = -1;
  switch (action)
    {
    case RESERVE1:
      if (varval_type != SNMP_INTEGER)
        return SNMP_ERROR_WRONGTYPE;
      if (((int)*var_val != CPE_ADMIN_ENABLE) && ((int)*var_val != CPE_ADMIN_DISABLE))
        return SNMP_ERROR_WRONGVALUE;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      gAdslLinePara = *var_val;
      setAdslLineOperation=1;        
      if ((int)*var_val == CPE_ADMIN_ENABLE)
      {
            modTypeAdslLineTrellis=1;
            signal(SIGALRM,cpeSetAdslTrellisMode);
            alarm(5);
      }
      else
      {
            modTypeAdslLineTrellis=0;
            signal(SIGALRM,cpeSetAdslTrellisMode);
            alarm(5);
      }
/*
      if (error == -1)
        return SNMP_ERROR_COMMITFAILED;
*/       
      break;
    case FREE:
      break;
    }
    return SNMP_ERROR_NOERROR;
}

unsigned char *var_cpeConfigAdslLineTrellis(int *var_len, snmp_info_t *mesg,
                                            WRITE_METHOD *write_method)
{
  int mode;

  *write_method = (WRITE_METHOD)write_cpeConfigAdslLineTrellis;
  *var_len = sizeof(long);
  mode = getAdslLineTrellisMode();
  if (mode)
    long_return = CPE_ADMIN_ENABLE;
  else
    long_return = CPE_ADMIN_DISABLE;

  if(setAdslLineOperation)
    return (unsigned char*)&gAdslLinePara;  
  else
  return (unsigned char*)&long_return;
}

/* 
 * link handshake negotiation mode configuration
 */
int write_cpeConfigAdslLineHandshake(int action, unsigned char *var_val, unsigned char varval_type, int var_val_len,
                                     unsigned char *statP, oid *name, int name_len)
{
  int error = -1;

  switch (action) 
    {
    case RESERVE1:
      if (varval_type != SNMP_INTEGER)
        return SNMP_ERROR_WRONGTYPE;
      if (((int)*var_val < CPE_ADSL_GDMT_FIRST) && ((int)*var_val > CPE_ADSL_AUTOSENSE_ADSL2PLUS))
        return SNMP_ERROR_WRONGVALUE;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      modTypeAdslLineHandshake = cpeConvertAdslModLineType(*var_val);
/*
      error = cpeSetAdslModulationMode(modType);
      
      if (error == -1)
        return SNMP_ERROR_COMMITFAILED;
*/
      gAdslLinePara = *var_val;
      setAdslLineOperation=1;
      signal(SIGALRM,cpeSetAdslModulationMode);
      alarm(5);  
      break;
    case FREE:
      break;
    }
    return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpeConfigAdslLineHandshake(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  long_return = cpeGetAdslLineModType();
  *write_method = (WRITE_METHOD)write_cpeConfigAdslLineHandshake;
  *var_len = sizeof(long);
  if(setAdslLineOperation)
    return (unsigned char*)&gAdslLinePara;
  else
  return (unsigned char *)&long_return;
}

/* 
 * BITSWAP encoding configuration       
 */
int write_cpeConfigAdslLineBitSwapping(int action,unsigned char *var_val, unsigned char varval_type,
                                       int var_val_len, unsigned char *statP, oid *name, int name_len)
{
  int error = -1;

  switch (action)
    {
    case RESERVE1:
      if (varval_type != SNMP_INTEGER)
        return SNMP_ERROR_WRONGTYPE;
      if (((int)*var_val != CPE_ADMIN_ENABLE) && ((int)*var_val != CPE_ADMIN_DISABLE))
        return SNMP_ERROR_WRONGVALUE;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      gAdslLinePara = *var_val;
      setAdslLineOperation=1;
      if ((int)*var_val == CPE_ADMIN_ENABLE)
      {
            modTypeAdslLineBitSwapping = 1;
            signal(SIGALRM,cpeSetAdslBitSwapMode);
            alarm(5);
      }
      else
      {
            modTypeAdslLineBitSwapping = 0;
            signal(SIGALRM, cpeSetAdslBitSwapMode);   
            alarm(5);
       }
/*      
      if (error == -1)
        return SNMP_ERROR_COMMITFAILED;
*/        
      break;
    case FREE:
      break;
    }
    return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpeConfigAdslLineBitSwapping(int *var_len, snmp_info_t *mesg,
        WRITE_METHOD *write_method)
{
  int mode;
  mode = cpeGetAdslBitSwapMode();
  if (mode)
    long_return = CPE_ADMIN_ENABLE;
  else
    long_return = CPE_ADMIN_DISABLE;
  *write_method = (WRITE_METHOD)write_cpeConfigAdslLineBitSwapping;
  *var_len = sizeof(long);
  if(setAdslLineOperation)
    return (unsigned char *)&gAdslLinePara;
  else
  return (unsigned char *)&long_return;
}

void cpeInitPvcEntry(pCPE_PVC_ENTRY pEntry)
{
  pEntry->upperEncap = CPE_PVC_ENCAP_BRIDGE2684;
  pEntry->adminStatus = CPE_ADMIN_ENABLE;
  pEntry->bridgeMode = CPE_ADMIN_DISABLE;
  pEntry->macLearnMode = CPE_ADMIN_DISABLE;
  pEntry->igmpMode = CPE_ADMIN_DISABLE;
  pEntry->dhcpcMode = CPE_ADMIN_DISABLE;
  pEntry->natMode = CPE_ADMIN_DISABLE;
}

int write_cpePvcEntry(int action,unsigned char *var_val, unsigned char varval_type, 
                      int var_val_len, unsigned char *statP, Oid *name,
                      SNMP_SET_HANDLER *set_handler, unsigned long *param)
{
    int index, value, column;
    int vpi, vci;
    int namelen;
    pCPE_PVC_ENTRY pEntry = NULL;
    static int setFlag = 0, copy = 0;
    static int change = 0;

    namelen = (int)name->namelen;
    /* column, ifIndex, vpiIndex, vciIndex, 0 ==> namelen at 0, column is at (namelen-4)*/
    column =  (int)name->name[namelen-4];
    value = (int)*var_val;

    switch (action) 
      {
      case RESERVE1:
        if (varval_type != SNMP_INTEGER)
          return SNMP_ERROR_WRONGTYPE;
        if (var_val_len > sizeof(int)) 
          return SNMP_ERROR_WRONGLENGTH;
        switch (column) 
          {
          case I_cpePvcId:
          case I_cpePvcIfIndex:
          case I_cpePvcOperStatus:
            return SNMP_ERROR_NOTWRITABLE;
          case I_cpePvcUpperEncapsulation:
            if ((value < CPE_PVC_ENCAP_PPPOA) || (value > CPE_PVC_ENCAP_BRIDGE2684IP))
              return SNMP_ERROR_WRONGVALUE;
            setFlag |= CPE_PVC_ENCAP_SET;
            copy = setFlag;
            break;
          case I_cpePvcAdminStatus:
          case I_cpePvcBridgeMode:
          case I_cpePvcMacLearnMode:
          case I_cpePvcIgmpMode:
          case I_cpePvcDhcpClientMode:
          case I_cpePvcNatMode:
#ifdef CT_SNMP_MIB_VERSION_2
       case I_cpePvcBindingMode:
       case I_cpePvcEthernetPortMap:
       case I_cpePvcServiceTypeIndex:
#endif
        if ((value != CPE_ADMIN_ENABLE) && (value != CPE_ADMIN_DISABLE)) 
        {
           return SNMP_ERROR_WRONGVALUE;
        }
        if (column == I_cpePvcAdminStatus)
          setFlag |= CPE_PVC_ADMIN_SET;
        else if (column == I_cpePvcBridgeMode)
          setFlag |= CPE_PVC_BRIDGE_MODE_SET;
        else if (column == I_cpePvcMacLearnMode)
          setFlag |= CPE_PVC_MAC_LEARN_SET;
        else if (column == I_cpePvcIgmpMode)
          setFlag |= CPE_PVC_IGMP_MODE_SET;
        else if (column == I_cpePvcDhcpClientMode)
          setFlag |= CPE_PVC_DHCP_CLIENT_SET;
        else if (column == I_cpePvcNatMode)
          setFlag |= CPE_PVC_NAT_SET;
#ifdef CT_SNMP_MIB_VERSION_2
        else if (column == I_cpePvcBindingMode)
          setFlag |= CPE_PVC_BINDING_MODE_SET;	
        else if (column == I_cpePvcEthernetPortMap)
          setFlag |= CPE_PVC_ETHERNET_PORTMAP_SET;
        else if (column == I_cpePvcServiceTypeIndex)
          setFlag |= CPE_PVC_SERVICE_TYPEINDEX_SET;
#endif
        copy = setFlag;
            break;

          default:
            return SNMP_ERROR_NOTWRITABLE;
          } /* switch column */
        break;
      case RESERVE2:
        break;
      case COMMIT:
        /* ifIndex, vpiIndex, vciIndex, column, */
        index = (int)name->name[namelen-3];
        vpi = (int)name->name[namelen-2];
        vci = (int)name->name[namelen-1];

        if ((index == 0) || ((vpi < 0) && (vpi > 255)) || ((vci < 32) && (vci > 65535))) {
#ifdef BUILD_SNMP_DEBUG
          if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE)
            printf("write_cpePvcEntry(vpi/vci %d/%d, index %d)\n",vpi,vci,index);
#endif
          return SNMP_ERROR_NOTWRITABLE;
        }

        if (pPvcParam == NULL) {
          if ((pPvcParam = malloc(sizeof(CPE_PVC_ENTRY))) == NULL) {
            setFlag=0;
            change=0;
            return SNMP_ERROR_COMMITFAILED;
      }
    }
    cpeScanWanInfo();
    pEntry = pWanList->head;
    while (pEntry) {
      if ((pEntry->vpi == vpi) && (pEntry->vci == vci)) 
        break;
      else
        pEntry = pEntry->next;
    }
    if (pEntry == NULL) {
      if (cpeAddWanInfo(vpi,vci) != 0)
        printf("cpeAddWanInfo failed.\n");

      /* scan again to see if an entry is created successfully */
      cpeScanWanInfo();
      pEntry = pWanList->head;
      while (pEntry) {
        if ((pEntry->vpi == vpi) && (pEntry->vci == vci)) 
          break;
        else
          pEntry = pEntry->next;
      }
      if (pEntry == NULL) {
        if (pPvcParam)
        {
          free(pPvcParam);
          pPvcParam=NULL;
        }
        copy = 0;
        setFlag = 0;
        return SNMP_ERROR_COMMITFAILED;   
      }
    } /* pEntry == NULL */

    /* very first commit */
    if (setFlag == copy) {
      memcpy(pPvcParam,pEntry,sizeof(CPE_PVC_ENTRY));
#ifdef BUILD_SNMP_DEBUG
      if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
        printf("********************setFlag %x, copy %x, change %d, pEntry->encap %d, pPvcParam->encap %d\n",
               setFlag,copy,change, pEntry->upperEncap, pPvcParam->upperEncap);
      }
#endif
    }

    switch (column) 
    {
      case I_cpePvcUpperEncapsulation:
#ifdef BUILD_SNMP_DEBUG
        if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
             printf("write_cpePvcEntry(): upperEncap from NMS is %d\n",
             value);
        }
#endif
        if (pEntry->upperEncap != value) {
          pPvcParam->upperEncap = value;
          change = 1;
        }
        setFlag &= ~CPE_PVC_ENCAP_SET;
        break;
      case I_cpePvcAdminStatus:
        if (pEntry->adminStatus != value) {
          pPvcParam->adminStatus = value;
          change = 1;
        }
        setFlag &= ~CPE_PVC_ADMIN_SET;
        break;
      case I_cpePvcBridgeMode:
        if (pEntry->bridgeMode != value) {
          pPvcParam->bridgeMode = value;
          change = 1;
        }
        setFlag &= ~CPE_PVC_BRIDGE_MODE_SET;
        break;
      case I_cpePvcMacLearnMode:
        if (pEntry->macLearnMode != value) {
          pPvcParam->macLearnMode = value;
          change = 1;
        }
        setFlag &= ~CPE_PVC_MAC_LEARN_SET;
        break;
      case I_cpePvcIgmpMode:
        if (pEntry->igmpMode != value) {
          pPvcParam->igmpMode = value;
          change = 1;
        }
        setFlag &= ~CPE_PVC_IGMP_MODE_SET;
        break;
      case I_cpePvcNatMode:
        if (pEntry->natMode != value) {
          if(value==CPE_ADMIN_ENABLE)
          {
            if(pPvcParam->upperEncap!=CPE_PVC_ENCAP_BRIDGE2684)
            {
              pPvcParam->natMode=value;
              change=1;
            }
            else
            {
              return SNMP_ERROR_BADVALUE;
            }
          }
          else
          {
            pPvcParam->natMode = value;
            change = 1;
          }
        }
        setFlag &= ~CPE_PVC_NAT_SET;
        break;
      case I_cpePvcDhcpClientMode:
        if (pEntry->dhcpcMode != value) {
          pPvcParam->dhcpcMode = value;
          change = 1;
        }
        setFlag &= ~CPE_PVC_DHCP_CLIENT_SET;
        break;
#ifdef CT_SNMP_MIB_VERSION_2
      case I_cpePvcBindingMode:
        if (pEntry->bindingMode!= value) {
          pPvcParam->bindingMode = value;
          change = 1;
        }
        setFlag &= ~CPE_PVC_BINDING_MODE_SET;
        break;
      case I_cpePvcEthernetPortMap:
        if (pEntry->ethernetPortMap!= value) {
          pPvcParam->ethernetPortMap = value;
          change = 1;
        }
        setFlag &= ~CPE_PVC_ETHERNET_PORTMAP_SET;
        break;
      case I_cpePvcServiceTypeIndex:
        if (pEntry->serviceTypeIndex!= value) {
          pPvcParam->serviceTypeIndex = value;
          change = 1;
        }
        setFlag &= ~CPE_PVC_ETHERNET_PORTMAP_SET;
        break;
#endif
    } /* switch */
    pPvcParam->vpi = vpi;
    pPvcParam->vci = vci;
    pPvcParam->conId = 1;

#ifdef BUILD_SNMP_DEBUG
    printf("********************setFlag %x, copy %x, change %d, pEntry->encap %d, pPvcParam->encap %d\n",
           setFlag,copy,change, pEntry->upperEncap, pPvcParam->upperEncap);
#endif

    if (setFlag == 0) {
      if (change) {
        if (pEntry->upperEncap != pPvcParam->upperEncap) {
          /* set default based on upper encapsulation */
          /* base on the encap mode, call Bcmxxx_createDefault() as in BcmNtwk_startWan */
          cpeProto_createDefaultAndSet(pPvcParam,copy);
        }
#ifdef BUILD_SNMP_DEBUG
        if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
          printf("====>  write_cpePvcEntry (change %d), calling cpeConfigWanInfo, pPvcParam->upperEncap=%d\n",
            change,pPvcParam->upperEncap);
        }
#endif
        cpeConfigWanInfo();
        change = 0;
      } /* change */
    }
    break;
      case FREE:
        /* need to free configured WAN INFO */
    //        vpi = (int)name->name[namelen-2];
    //        vci = (int)name->name[namelen-1];
        //        cpeDeleteWanInfo(vpi,vci);
    if (pPvcParam)
    {
       free(pPvcParam);
       pPvcParam = NULL;
    }
    setFlag = 0;
    copy=0;
    change=0;
    break;
    } /* switch */
    return SNMP_ERROR_NOERROR;
}

/* 
 * PVC VCL configuration entry
 */
unsigned char* var_cpePvcEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType,
                               snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  int column = newoid->name[(newoid->namelen - 1)];
  int result;
  pCPE_PVC_ENTRY pEntry;
  int index, vpiIndex, vciIndex;

  *write_method = (WRITE_METHOD)write_cpePvcEntry;

  index = newoid->namelen++;
  vpiIndex = newoid->namelen++;
  vciIndex = newoid->namelen++;

  if( mesg->pdutype != SNMP_SET_REQ_PDU ) {
        cpeScanWanInfo();
    /* index of this table is ifIndex, vpi, vci */
    if (pWanList->count == 0) { 
      return NO_MIBINSTANCE;
    }
    pEntry = pWanList->head;
    while (pEntry) {
      newoid->name[index] = pEntry->ifIndex;
      newoid->name[vpiIndex] = pEntry->vpi;
      newoid->name[vciIndex] = pEntry->vci;
      result = compare(reqoid, newoid);
      if (((searchType == EXACT) && (result == 0)) ||
          ((searchType == NEXT) && (result < 0)))
        break; /* found */
      pEntry = pEntry->next;
    }
    if (pEntry == NULL) {
      return NO_MIBINSTANCE;
    }
    *var_len = sizeof(long);
    switch (column)
      {
      case I_cpePvcId:
        return (unsigned char*)&pEntry->pvcId;
      case I_cpePvcIfIndex:
        return (unsigned char*)&pEntry->pvcIfIndex;
      case I_cpePvcUpperEncapsulation:
        return (unsigned char*)&pEntry->upperEncap;
      case I_cpePvcAdminStatus:
        return (unsigned char*)&pEntry->adminStatus;
      case I_cpePvcOperStatus:
        return (unsigned char*)&pEntry->operStatus;
      case I_cpePvcBridgeMode:
        return (unsigned char*)&pEntry->bridgeMode;
      case I_cpePvcMacLearnMode:
        return (unsigned char*)&pEntry->macLearnMode;
      case I_cpePvcIgmpMode:
        return (unsigned char*)&pEntry->igmpMode;
      case I_cpePvcDhcpClientMode:
        return (unsigned char*)&pEntry->dhcpcMode;        
      case I_cpePvcNatMode:
        return (unsigned char*)&pEntry->natMode;
#ifdef CT_SNMP_MIB_VERSION_2
      case I_cpePvcBindingMode:
        return (unsigned char*)&pEntry->bindingMode;
      case I_cpePvcEthernetPortMap:
        return (unsigned char*)&pEntry->ethernetPortMap;
      case I_cpePvcServiceTypeIndex:
        return (unsigned char*)&pEntry->serviceTypeIndex;
#endif
      default:
        return NO_MIBINSTANCE;
    }
  } /* if mesg->pdutype == SNMP_GET_REQ_PDU */
  else {
    newoid->name[index] = reqoid->name[index];
    newoid->name[vpiIndex] = reqoid->name[vpiIndex];
    newoid->name[vciIndex] = reqoid->name[vciIndex];
    /* return whatever as long as it's non-zero */
    return (unsigned char*)&long_return;
  }
}

/* 
 * PPP configuration entry
 */
int write_cpePppEntry(int action,unsigned char *var_val, unsigned char varval_type, 
                      int var_val_len, unsigned char *statP, Oid *name,
                      SNMP_SET_HANDLER *set_handler, unsigned long *param)
{
  int column;
  int value;
  int namelen;
  static CPE_PPP_INFO entry;
  pCPE_PPP_INFO pEntry;
  static int setFlag = 0, copy = 0;
  //  int vpi, vci;
  int pppIfIndex;
  //  char ifName[IFC_TINY_LEN];
  int len, i;

  if ((setFlag == 0) && (copy == 0))
    memset(&entry,0,sizeof(CPE_PPP_INFO));

  namelen = (int)name->namelen;
  /* column,  indexId, 0 ==> namelen at 0, indexId is namelen-1, column is at (namelen-2)*/
  column =  (int)name->name[namelen-2];
  pppIfIndex = (int)name->name[namelen-1];
  switch (action) 
    {
    case RESERVE1:
        switch (column) 
          {
          case I_cpePppIfIndex:
          case I_cpePppOperStatus:
          case I_cpePppTestResult:
            return SNMP_ERROR_NOTWRITABLE;
          case I_cpePppPvcIfIndex:
            if (varval_type != SNMP_INTEGER)
              return SNMP_ERROR_WRONGTYPE;
            if (var_val_len > sizeof(int)) 
              return SNMP_ERROR_WRONGLENGTH;
            value = (int)*var_val;
            setFlag |= CPE_PPP_PVC_INDEX_SET;
            copy = setFlag;
            break;
          case I_cpePppServiceName:
            if (varval_type != SNMP_STRING)
              return SNMP_ERROR_WRONGTYPE;
            if (var_val_len > CPE_PPP_NAME_LEN) 
              return SNMP_ERROR_WRONGLENGTH;
            setFlag |= CPE_PPP_SERVICE_NAME_SET;
            copy = setFlag;
            break;
          case I_cpePppDisconnectTimeout:
            if (varval_type != SNMP_INTEGER)
              return SNMP_ERROR_WRONGTYPE;
            if (var_val_len > sizeof(int)) 
              return SNMP_ERROR_WRONGLENGTH;
            value = (int)*var_val;
            if ((value > CPE_PPP_TIMEOUT_MAX_SEC) || (value < 0))
              return SNMP_ERROR_WRONGVALUE;
            setFlag |= CPE_PPP_TIMEOUT_SET;
            copy = setFlag;
            break;
          case I_cpePppMSS:
            if (varval_type != SNMP_INTEGER)
              return SNMP_ERROR_WRONGTYPE;
            if (var_val_len > sizeof(int)) 
              return SNMP_ERROR_WRONGLENGTH;
            break;
          case I_cpePppRowStatus:
            if (varval_type != SNMP_INTEGER)
              return SNMP_ERROR_WRONGTYPE;
            if (var_val_len > sizeof(int)) 
              return SNMP_ERROR_WRONGLENGTH;
            value = (int)*var_val;
            if ((value < CPE_ROW_STATUS_CREATE_AND_GO) || 
                (value > CPE_ROW_STATUS_DESTROY))
              return SNMP_ERROR_WRONGVALUE;
            break;
          case I_cpePppAdminStatus:
            if (varval_type != SNMP_INTEGER)
              return SNMP_ERROR_WRONGTYPE;
            if (var_val_len > sizeof(int)) 
              return SNMP_ERROR_WRONGLENGTH;
            value = (int)*var_val;
            if ((value < CPE_PPP_ADMIN_UP) || 
                (value > CPE_PPP_ADMIN_CANCEL))
              return SNMP_ERROR_WRONGVALUE;
            setFlag |= CPE_PPP_ADMIN_SET;
            copy = setFlag;
            break;
          default:
            return SNMP_ERROR_NOTWRITABLE;
          } /* switch column */
        break;
      case RESERVE2:
        break;
      case COMMIT:

#ifdef BUILD_SNMP_DEBUG
        if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
          printf("writePppTable(): commit::: setFlag %x, copy %x\n",setFlag,copy);
        }
#endif
        /* link, idIndex, column */
        switch (column) 
          {
          case I_cpePppIfIndex:
          case I_cpePppOperStatus:
          case I_cpePppTestResult:
            return SNMP_ERROR_COMMITFAILED;
            break;
          case I_cpePppPvcIfIndex:
            value = *((int*)var_val);
            for (i = 0; i < (sizeof(int)-var_val_len); i++)
              value = value >> 8;
            entry.pvcIfIndex = value;
            setFlag &= ~CPE_PPP_PVC_INDEX_SET;
            break;
          case I_cpePppServiceName:
            memset(entry.serviceName,0,sizeof(entry.serviceName));
            cmsUtl_strncpy(entry.serviceName,var_val,var_val_len);
            setFlag &= ~CPE_PPP_SERVICE_NAME_SET;
            break;
          case I_cpePppDisconnectTimeout:
            /* MIB is in seconds, PSI is in minute */
            value = *((int*)var_val);
            for (i = 0; i < (sizeof(int)-var_val_len); i++)
              value = value >> 8;
            entry.timeout = value;
            setFlag &= ~CPE_PPP_TIMEOUT_SET;
            break;
          case I_cpePppMSS:
            /* ignoring, unclear */
            break;
          case I_cpePppRowStatus:
            entry.rowStatus = *var_val;
            break;
          case I_cpePppAdminStatus:
            entry.adminStatus = *var_val;
            setFlag &= ~CPE_PPP_ADMIN_SET;
            break;
          default:
            return SNMP_ERROR_COMMITFAILED;
          } /* switch */

        if (setFlag == 0) {
          if (copy == 0)
            return SNMP_ERROR_NOERROR;

          /* check to see if a virtual row has been created */
          cpeScanPppInfo();
          pEntry = pPppList->head;
          while (pEntry) {
            if (pEntry->link == pppIfIndex)
              break;
            else
              pEntry = pEntry->next;
          }
          /* no row created, problem, because one is always created for pvcEncap type PPP */
          if (pEntry == NULL) {
#ifdef BUILD_SNMP_DEBUG
            if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
              printf("scanPPPList cannot find pEntry with pppIfIndex %d\n",pppIfIndex);
            }
#endif
            return SNMP_ERROR_COMMITFAILED;
          }
          if ((copy & CPE_PPP_SERVICE_NAME_SET) == CPE_PPP_SERVICE_NAME_SET) {
            len = strlen(entry.serviceName);
            cmsUtl_strncpy(pEntry->serviceName,entry.serviceName,len);
            pEntry->serviceName[len] = '\0';
          }
          if ((copy & CPE_PPP_TIMEOUT_SET) == CPE_PPP_TIMEOUT_SET)
            pEntry->timeout = entry.timeout;
          if ((copy & CPE_PPP_ADMIN_SET) == CPE_PPP_ADMIN_SET)
            pEntry->adminStatus = entry.adminStatus;

#ifdef BUILD_SNMP_DEBUG
          if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
            printf(".......................calling cpeConfigPppInfo(), serviceName %s\n",
                   pEntry->serviceName);
          }
#endif

#if 0
          *set_handler = (SNMP_SET_HANDLER)cpeConfigPppInfo;
          *param = (unsigned long)(pEntry);
#else
          cpeConfigPppInfo((unsigned long) pEntry);
#endif
          setFlag = 0;
          copy = 0;
        } /* setFlag */

        break;
    case FREE:
      setFlag = 0;
      copy = 0;
      memset(&entry,0,sizeof(CPE_PPP_INFO));
        break;
      } /* switch */
    return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpePppEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType,
                snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  int   column = newoid->name[(newoid->namelen - 1)];
  int   result;
  pCPE_PPP_INFO pEntry;

  /* cpePppEntry is extension of ppp secret table.    So, this table is indexed by pppSececuritySecretLink,
     and pppSecuritySecretId, pppSecuritySecretLink is 210000, 210001,... and secretId is 0, 1,2... ; but
     CT specs says there is only one index.. index */
  //  int link, index;
  int index;

  *write_method = (WRITE_METHOD)write_cpePppEntry;

  //link = newoid->namelen++;
  index = newoid->namelen++;

  if( mesg->pdutype != SNMP_SET_REQ_PDU ) {
    /* index of this table is ifIndex, vpi, vci */
    cpeScanPppInfo();
    if (pPppList->count == 0) { 
      return NO_MIBINSTANCE;
    }
    pEntry = pPppList->head;
    while (pEntry) {
      newoid->name[index] = pEntry->link;
      result = compare(reqoid, newoid);
      if (((searchType == EXACT) && (result == 0)) ||
          ((searchType == NEXT) && (result < 0)))
        break; /* found */
      pEntry = pEntry->next;
    }
    if (pEntry == NULL) 
      return NO_MIBINSTANCE;

    *var_len = sizeof(long);
    switch (column) 
      {
      case I_cpePppIfIndex:
        return (unsigned char *)&pEntry->link;
      case I_cpePppPvcIfIndex:
        return (unsigned char *)&pEntry->pvcIfIndex;
      case I_cpePppServiceName:
        *var_len = strlen(pEntry->serviceName);
        return (unsigned char *)pEntry->serviceName;
      case I_cpePppDisconnectTimeout:
        return (unsigned char *)&pEntry->timeout;
      case I_cpePppMSS:
        return (unsigned char *)&pEntry->maxSegmentSize;
      case I_cpePppRowStatus:
        return (unsigned char *)&pEntry->rowStatus;
      case I_cpePppAdminStatus:
        return (unsigned char *)&pEntry->adminStatus;
      case I_cpePppOperStatus:
        return (unsigned char *)&pEntry->operStatus;
      case I_cpePppTestResult:
        return (unsigned char *)&pEntry->testResult;
      default:
        return NO_MIBINSTANCE;
      } /* column */
  } /* != SNMP_SET_REQ_PDU */
  else {
    //newoid->name[link] = reqoid->name[link];
    newoid->name[index] = reqoid->name[index];

    /* return whatever as long as it's non-zero */
    return (unsigned char*)&long_return;
  }
} /* var_cpePppEntry */

int write_cpeIpEntry(int action,unsigned char *var_val, unsigned char varval_type, 
                      int var_val_len, unsigned char *statP, Oid *name, int name_len)
{
    int routeIndex, column, value;
    int namelen;
    int intValue;
    pCPE_IP_TABLE_INFO pEntry= NULL;
    int i, ret;
    static int set=0, copy=0;
    static CPE_IP_TABLE_INFO input;

    namelen = (int)name->namelen;
    /* column, ifIndex, 0 ==> namelen at 0, column is at (namelen-2)*/
    column =  (int)name->name[namelen-2];
    
    if ((set==0) && (copy==0))
      memset(&input,0,sizeof(CPE_IP_TABLE_INFO));

#ifdef BUILD_SNMP_DEBUG
    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
      printf("write_cpeIpEntry(entry): column %d\n",column);
    }
#endif

    switch (action) 
      {
      case RESERVE1:
        switch (column) 
          {
          case I_cpeIpIndex:
            return SNMP_ERROR_NOTWRITABLE;
          case I_cpeIpLowerIfIndex:
            if (varval_type != SNMP_INTEGER) {
#ifdef BUILD_SNMP_DEBUG
              if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
                printf("check: lowerIfIndex, invalid var_type %d (SNMP_INTEGER %d)\n",
                       varval_type,SNMP_INTEGER);
              }
#endif
              return SNMP_ERROR_WRONGTYPE;
            }

            if (var_val_len > sizeof(int)) {
#ifdef BUILD_SNMP_DEBUG
              if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
                printf("check: lowerIfIndex, invalid val_len %d\n",var_val_len);
              }
#endif
              return SNMP_ERROR_WRONGLENGTH;
            }
            set |= CPE_IP_TABLE_LOWER_INDEX_SET;
            copy = set;
            break;
          case I_cpeIpAddress:
          case I_cpeIpNetmask:
          case I_cpeIpGateway:
            if (varval_type != SNMP_IPADDRESS) {
#ifdef BUILD_SNMP_DEBUG
              if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
                printf("check: IpAddr,Netmask or Gateway, invalid var_type %d (SNMP_IPADDRESS %d)\n",
                       varval_type,SNMP_IPADDRESS);
              }
#endif
              return SNMP_ERROR_WRONGTYPE;
            }
            if (var_val_len != sizeof(int)) {
#ifdef BUILD_SNMP_DEBUG
              if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
                printf("check: cpeIpRowStatus, invalid val_len %d\n",var_val_len);
              }
#endif
              return SNMP_ERROR_WRONGLENGTH;
            }
            if (column == I_cpeIpAddress)
              set |= CPE_IP_TABLE_IP_SET;
            else if (column == I_cpeIpNetmask)
              set |= CPE_IP_TABLE_MASK_SET;
            else
              set |= CPE_IP_TABLE_GW_SET;
            copy = set;
            break;
          case I_cpeIpRowStatus:
            value = (int)*var_val;
            if ((value != CPE_ROW_STATUS_ACTIVE) &&
                (value != CPE_ROW_STATUS_CREATE_AND_GO) &&
                (value != CPE_ROW_STATUS_CREATE_AND_WAIT) &&
                (value != CPE_ROW_STATUS_DESTROY)) {
#ifdef BUILD_SNMP_DEBUG
              if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
                printf("check: cpeIpRowStatus, invalid value %d\n",value);
              }
#endif
              return SNMP_ERROR_WRONGVALUE;
              set |= CPE_IP_TABLE_ROW_STATUS_SET;
              copy = set;
            }
            break;
          case I_cpeIpDefaultRowOnOff:
            value = (int)*var_val;
            if ((value != CPE_ADMIN_ENABLE) && (value != CPE_ADMIN_DISABLE)) 
              return SNMP_ERROR_WRONGVALUE;
            
            set |= CPE_IP_TABLE_GW_ONOFF_SET;
            copy = set;
            break;
          default:
            return SNMP_ERROR_NOTWRITABLE;
          } /* switch column */
        break;
      case RESERVE2:
        break;
      case COMMIT:
        switch (column) 
          {
          case I_cpeIpIndex:
            return SNMP_ERROR_NOTWRITABLE;
          case I_cpeIpLowerIfIndex:
            intValue = *((int*)var_val);
            for (i = 0; i < (sizeof(int)-var_val_len); i++)
              intValue = intValue >> 8;
            //            pEntry->lowerIfIndex = intValue;
            input.lowerIfIndex = intValue;
            set &= ~CPE_IP_TABLE_LOWER_INDEX_SET;
            break;
          case I_cpeIpAddress:
            //            pEntry->addr.s_addr = *(int*)var_val;
            input.addr.s_addr = *(int*)var_val;
            set &= ~CPE_IP_TABLE_IP_SET;
            break;
          case I_cpeIpNetmask:
            //            pEntry->mask.s_addr = *(int*)var_val;
            input.mask.s_addr = *(int*)var_val;
            set &= ~CPE_IP_TABLE_MASK_SET;
            break;
          case I_cpeIpGateway:
            //pEntry->gw.s_addr = *(int*)var_val;
            input.gw.s_addr = *(int*)var_val;
            set &= ~CPE_IP_TABLE_GW_SET;
            break;
          case I_cpeIpRowStatus:
            input.status = (int)*var_val;
            set &= ~CPE_IP_TABLE_ROW_STATUS_SET;
            break;
          case I_cpeIpDefaultRowOnOff:
            value = (int)*var_val;
            if (value == CPE_ADMIN_ENABLE)
              input.gwOnOff = 1;
            else
              input.gwOnOff = 0;
            set &= ~CPE_IP_TABLE_GW_ONOFF_SET;
            break;
          default:
            break;
          } /* switch */

        if (set == 0) {
          /* index, column, */
          routeIndex = (int)name->name[namelen-1];
        
#ifdef BUILD_SNMP_DEBUG
          if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
            printf("commit: routeIndex %d\n",routeIndex);
          }
#endif

          /* check to see if a virtual row has been created */
          cpeScanIpTable();
          pEntry = pIpList->head;
          while (pEntry) {
            if (pEntry->index == routeIndex) 
              break;
            else
              pEntry = pEntry->next;
          } /* while */
          if (pEntry == NULL) 
            return SNMP_ERROR_COMMITFAILED;

          if ((copy & CPE_IP_TABLE_IP_SET) == CPE_IP_TABLE_IP_SET)
            memcpy(&pEntry->addr,&input.addr,sizeof(input.addr));
          if ((copy & CPE_IP_TABLE_MASK_SET) == CPE_IP_TABLE_MASK_SET)
            memcpy(&pEntry->mask,&input.mask,sizeof(input.mask));
          if ((copy & CPE_IP_TABLE_GW_SET) == CPE_IP_TABLE_GW_SET)
            memcpy(&pEntry->gw,&input.gw,sizeof(input.gw));
          if ((copy & CPE_IP_TABLE_GW_ONOFF_SET) == CPE_IP_TABLE_GW_ONOFF_SET)
            pEntry->gwOnOff = input.gwOnOff;
          else
            pEntry->gwOnOff = 1; /* default is enable */
          if ((copy & CPE_IP_TABLE_ROW_STATUS_SET) == CPE_IP_TABLE_ROW_STATUS_SET)
            pEntry->status = input.status;
          else
            pEntry->status = CPE_ROW_STATUS_ACTIVE;

#ifdef BUILD_SNMP_DEBUG
          if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
            char ip[IFC_TINY_LEN];
            char mask[IFC_TINY_LEN];
            char gw[IFC_TINY_LEN];
            cmsUtl_strncpy(ip,inet_ntoa(pEntry->addr),IFC_TINY_LEN);
            cmsUtl_strncpy(mask,inet_ntoa(pEntry->mask),IFC_TINY_LEN);
            cmsUtl_strncpy(gw,inet_ntoa(pEntry->gw),IFC_TINY_LEN);
            
            printf("before calling cpeConfigIpTable(): index %d, lowerIndex %d, gwOnOff %d\n",
                   pEntry->index,pEntry->lowerIfIndex,pEntry->gwOnOff);
            printf("   ip %s, mask %s, gw %s, status %d\n",ip,mask,gw,pEntry->status);
          }
#endif
          
          ret = cpeConfigIpTable(pEntry);
          if (ret == -1) {
            return SNMP_ERROR_COMMITFAILED;
          }
        }
        break;
      case FREE:
        set = 0;
        memset(&input,0,sizeof(CPE_IP_TABLE_INFO));
        break;
      } /* switch */
    return SNMP_ERROR_NOERROR;
}

/* 
 * Interfaces IP address entry
 */
unsigned char *
var_cpeIpEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType,
               snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  /* Last subOID of COLUMNAR OID is column */
  int   column = newoid->name[(newoid->namelen - 1)];
  int   result;
  pCPE_IP_TABLE_INFO pEntry;
  int index;

  *write_method = (WRITE_METHOD)write_cpeIpEntry;
  index = newoid->namelen;
  newoid->namelen++;

  if( mesg->pdutype != SNMP_SET_REQ_PDU ) {
    cpeScanIpTable();

    if (pIpList->count == 0)
      return NO_MIBINSTANCE;

    pEntry = pIpList->head;

    while (pEntry) {
      newoid->name[index] = pEntry->index;
      /* Determine whether it is the requested OID  */
      result = compare(reqoid, newoid);
      if (((searchType == EXACT) && (result == 0)) ||
          ((searchType == NEXT) && (result < 0)))
        break; /* found */
      pEntry = pEntry->next;
    } /* for IP routes */

    if (pEntry == NULL)
      return NO_MIBINSTANCE;

    *var_len = sizeof(long);

    switch (column) 
      {
      case I_cpeIpIndex:
        return (unsigned char *)&pEntry->index;
      case I_cpeIpLowerIfIndex:
        return (unsigned char *)&pEntry->lowerIfIndex;
      case I_cpeIpAddress:
        return (unsigned char *)&pEntry->addr;
      case I_cpeIpNetmask:
        return (unsigned char *)&pEntry->mask;
      case I_cpeIpGateway:
        return (unsigned char *)&pEntry->gw;
      case I_cpeIpRowStatus:
        long_return = CPE_ROW_STATUS_ACTIVE;
        return (unsigned char *)&long_return;
      case I_cpeIpDefaultRowOnOff:
        /* default should be enable */
        if (pEntry->gwOnOff)
          long_return = CPE_ADMIN_ENABLE;
        else
          long_return = CPE_ADMIN_DISABLE;
        return (unsigned char *)&long_return;
      default:
        return NO_MIBINSTANCE;
      } /* switch */
  } /* != SNMP_SET_REQ_PDU */
  else {
    newoid->name[index] = reqoid->name[index];
    /* return whatever as long as it's non-zero */
    return (unsigned char*)&long_return;
  }
}

/* 
 * dhcp mode configuration:
 * noSupport(1),no support dhcp 
 * dhcpServer(2),support dhcp server 
 * 
 */
int     write_cpeLanDhcpAdminStatus(int action,
        unsigned char *var_val, unsigned char varval_type, int var_val_len,
        unsigned char *statP, oid *name, int name_len)
{
  int error = 0;
  switch (action)
    {
    case RESERVE1:
      if (varval_type != SNMP_INTEGER)
        return SNMP_ERROR_WRONGTYPE;
      if (((int)*var_val < CPE_DHCP_SERVER_NO_SUPPORT) ||
          ((int)*var_val > CPE_DHCP_RELAY_ENABLE))
        return SNMP_ERROR_WRONGVALUE;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      if ((int)*var_val == CPE_DHCP_SERVER_NO_SUPPORT)
        error = cpeSetDhcpMode(DHCP_SRV_DISABLE);
      else if ((int)*var_val == CPE_DHCP_SERVER_ENABLE)
        error = cpeSetDhcpMode(DHCP_SRV_ENABLE);
      else if ((int)*var_val == CPE_DHCP_RELAY_ENABLE)
        error = cpeSetDhcpMode(DHCP_SRV_RELAY);

      if (error == -1)
        return SNMP_ERROR_COMMITFAILED;
      break;
    case FREE:
      break;
    }
  return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpeLanDhcpAdminStatus(int *var_len, snmp_info_t *mesg,
        WRITE_METHOD *write_method)
{
  int mode;
  mode = cpeGetDhcpMode();
  /* modem never returns client enable mode */
  switch (mode)
    {
    case DHCP_SRV_ENABLE:
      long_return = CPE_DHCP_SERVER_ENABLE;
      break;
    case DHCP_SRV_RELAY:
      long_return = CPE_DHCP_RELAY_ENABLE;
      break;
    case DHCP_SRV_DISABLE:
    default:
      long_return = CPE_DHCP_SERVER_NO_SUPPORT;
      break;
    }
  *write_method = (WRITE_METHOD)write_cpeLanDhcpAdminStatus;
  *var_len = sizeof(long);
  return (unsigned char *)&long_return;
}

#ifdef CT_SNMP_MIB_VERSION_2

/* 
 * the DHCP Relay IP address
 */
int write_cpeDhcpReplayIpAddress(int action,
        unsigned char *var_val, unsigned char varval_type, int var_val_len,
        unsigned char *statP, oid *name, int name_len)
{
        int error;
        struct in_addr ip;

        switch (action) 
        {
            case RESERVE1:
                if (varval_type != SNMP_IPADDRESS)
                return SNMP_ERROR_WRONGTYPE;
                if (var_val_len != sizeof(int)) 
                return SNMP_ERROR_WRONGLENGTH;
                break;
            case RESERVE2:
                break;
            case COMMIT:
                ip.s_addr = *(int*)var_val;
                cpeSetDhcpRelayAddr(&ip);
                if (error == -1)
                return SNMP_ERROR_COMMITFAILED;
                break;
            case FREE:
                break;
        }
        return SNMP_ERROR_NOERROR;
}

/* 
 * the DHCP Relay IP address
 */

unsigned char *
var_cpeDhcpRelayIpAddress(int *var_len, snmp_info_t *mesg,
         WRITE_METHOD *write_method)
{
    IFC_LAN_INFO lanInfo;
    
    *write_method = (WRITE_METHOD)write_cpeDhcpReplayIpAddress;
    *var_len = sizeof(long);
    BcmDb_getLanInfo(IFC_ENET_ID, &lanInfo);
    long_return = lanInfo.dhcpSrv.startAddress.s_addr;
    return (unsigned char *)&long_return;

}

/* 
 * the Qos Tag Mode Set
 */
int  write_cpeQosTagMode(int action,
        unsigned char *var_val, unsigned char varval_type, int var_val_len,
        unsigned char *statP, oid *name, int name_len)
{
       int EnableQoS = 0;
       int ret = -1;
       switch (action)
      {
      case RESERVE1:
        if (varval_type != SNMP_INTEGER)
          return SNMP_ERROR_WRONGTYPE;
        if (((int)*var_val != CPE_ADMIN_ENABLE) && ((int)*var_val != CPE_ADMIN_DISABLE))
          return SNMP_ERROR_WRONGVALUE;
        break;
      case RESERVE2:
        break;
      case COMMIT:
        EnableQoS = *var_val;
// 1: noQos 2:EnableQos        
        if(EnableQoS==2)
            ret = cpeQosTagModeConfig(1);
        else if(EnableQoS==1)
            ret = cpeQosTagModeConfig(0);
        if(ret ==-1)
            return SNMP_ERROR_COMMITFAILED;
        break;
      case FREE:
        break;
      }
    
    return SNMP_ERROR_NOERROR;
}

/* 
 * the Qos Tag Mode Get 
 */

unsigned char *
var_cpeQosTagMode(int *var_len, snmp_info_t *mesg,
        WRITE_METHOD *write_method)
{
    
  *write_method = (WRITE_METHOD)write_cpeQosTagMode;
  *var_len = sizeof(long);
  if(BcmDb_isQosEnabled())
    long_return = 2;
  else
    long_return = 1;
  return (unsigned char *)&long_return;
}

int write_cpeEthernetPortEntry(int action,unsigned char *var_val, unsigned char varval_type, 
                 int var_val_len, unsigned char *statP, Oid *name, int name_len)
{
    int ethernetPortIndex, column;
    int namelen;
    int intValue;
    pCPE_ETHERNETPORT_TABLE_INFO pEntry= NULL;
    int i, ret;
    static int set=0, copy=0;
    static CPE_ETHERNETPORT_TABLE_INFO input;

    namelen = (int)name->namelen;
    /* column, ifIndex, 0 ==> namelen at 0, column is at (namelen-2)*/
    column =  (int)name->name[namelen-2];

    if ((set==0) && (copy==0))
    memset(&input,0,sizeof(CPE_ETHERNETPORT_TABLE_INFO));

#ifdef BUILD_SNMP_DEBUG
    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
    {
        printf("write_cpeEthernetPortEntry(entry): column %d\n",column);
    }
#endif

    switch (action) 
    {
        case RESERVE1:
        switch (column) 
        {
            case I_cpeEthernetPortIfIndex:
            return SNMP_ERROR_NOTWRITABLE;
            break;

            case I_cpeEthernetPortSerialNumber:
            if (varval_type != SNMP_INTEGER) 
            {
#ifdef BUILD_SNMP_DEBUG
                if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                {
                    printf("check: EthernetPortSerialNumber, invalid var_type %d (SNMP_INTEGER %d)\n",
                    varval_type,SNMP_INTEGER);
                }
#endif
                return SNMP_ERROR_WRONGTYPE;
            }

            if (var_val_len > sizeof(int)) 
            {
#ifdef BUILD_SNMP_DEBUG
                if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                {
                    printf("check: EthernetPortSerialNumber, invalid val_len %d\n",var_val_len);
                }
#endif
                return SNMP_ERROR_WRONGLENGTH;
            }
            set |= CPE_ETHERNETPORT_SERIALNUMBER_SET;
            copy = set;
            break;

            case I_cpeEthernetPortBindingMode:
            if (varval_type != SNMP_INTEGER) 
            {
#ifdef BUILD_SNMP_DEBUG
                if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                {
                    printf("check: EthernetPortBindingMode, invalid var_type %d (SNMP_INTEGER %d)\n",
                    varval_type,SNMP_INTEGER);
                }
#endif
                return SNMP_ERROR_WRONGTYPE;
            }

            if (var_val_len > sizeof(int)) 
            {
#ifdef BUILD_SNMP_DEBUG
                if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                {
                    printf("check: EthernetPortBindingMode, invalid val_len %d\n",var_val_len);
                }
#endif
                return SNMP_ERROR_WRONGLENGTH;
            }
            set |= CPE_ETHERNETPORT_SERIALNUMBER_SET;
            copy = set;
            break;

            case I_cpeEthernetPortIgmpOnOff:
            if (varval_type != SNMP_INTEGER) 
            {
#ifdef BUILD_SNMP_DEBUG
                if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                {
                    printf("check: EthernetPortIgmpOnOff, invalid var_type %d (SNMP_INTEGER %d)\n",
                    varval_type,SNMP_INTEGER);
                }
#endif
                return SNMP_ERROR_WRONGTYPE;
            }

            if (var_val_len > sizeof(int)) 
            {
#ifdef BUILD_SNMP_DEBUG
                if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                {
                    printf("check: EthernetPortIgmpOnOff, invalid val_len %d\n",var_val_len);
                }
#endif
                return SNMP_ERROR_WRONGLENGTH;
            }
            set |= CPE_ETHERNETPORT_IGMPONOFF_SET;
            copy = set;
            break;

            case I_cpeEthernetPortI8021dParameter:
            if (varval_type != SNMP_INTEGER) 
            {
#ifdef BUILD_SNMP_DEBUG
                if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                {
                    printf("check: cpeEthernetPortI8021dParameter, invalid var_type %d (SNMP_INTEGER %d)\n",
                    varval_type,SNMP_INTEGER);
                }
#endif
                return SNMP_ERROR_WRONGTYPE;
            }

            if (var_val_len > sizeof(int)) 
            {
#ifdef BUILD_SNMP_DEBUG
                if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                {
                    printf("check: cpeEthernetPortI8021dParameter, invalid val_len %d\n",var_val_len);
                }
#endif
                return SNMP_ERROR_WRONGLENGTH;
            }
            set |= CPE_ETHERNETPORT_I8021DPARAMETER_SET;
            copy = set;
            break;          

            default:
            return SNMP_ERROR_NOTWRITABLE;
        } /* switch column */
        break;

        case RESERVE2:
        break;

        case COMMIT:
        switch (column) 
        {
            case I_cpeEthernetPortIfIndex:
            intValue = *((int*)var_val);
            for (i = 0; i < (sizeof(int)-var_val_len); i++)
            {
                intValue = intValue >> 8;
            }
            input.ethernetPortIfIndex= intValue;
            set &= ~CPE_ETHERNETPORT_IFINDEX_SET;
            break;

            case I_cpeEthernetPortSerialNumber:
            intValue = *((int*)var_val);
            for (i = 0; i < (sizeof(int)-var_val_len); i++)
            {
                intValue = intValue >> 8;
            }
            input.ethernetPortSerialNumber= intValue;
            set &= ~CPE_ETHERNETPORT_SERIALNUMBER_SET;
            break;

            case I_cpeEthernetPortBindingMode:
            intValue = *((int*)var_val);
            for (i = 0; i < (sizeof(int)-var_val_len); i++)
            {
                intValue = intValue >> 8;
            }
            input.ethernetPortBindingMode= intValue;
            set &= ~CPE_ETHERNETPORT_BINDINGMODE_SET;   
            break;

            case I_cpeEthernetPortIgmpOnOff:
            intValue = *((int*)var_val);
            for (i = 0; i < (sizeof(int)-var_val_len); i++)
            {
                intValue = intValue >> 8;
            }
            input.ethernetPortIgmpOnOff= intValue;
            set &= ~CPE_ETHERNETPORT_IGMPONOFF_SET;   
            break;

            case I_cpeEthernetPortI8021dParameter:
            intValue = *((int*)var_val);
            for (i = 0; i < (sizeof(int)-var_val_len); i++)
            {
                intValue = intValue >> 8;
            }
            input.ethernetPortI8021dParameter= intValue;
            set &= ~CPE_ETHERNETPORT_I8021DPARAMETER_SET;	
            break;

            default:
            break;
        } /* switch */

        if (set == 0) 
        {
            /* index, column, */
            ethernetPortIndex = (int)name->name[namelen-1];

#ifdef BUILD_SNMP_DEBUG
            if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
            {
                printf("commit: ethernetPortIndex %d\n",ethernetPortIndex);
            }
#endif

            /* Add by Alex Liu , Here need to add the implement function in the future,check to see if a virtual row has been created */
            /*
            cpeScanIpTable();
            */
            pEntry = pEthernetPortList->head;

            while (pEntry) 
            {
                if (pEntry->ethernetPortIfIndex== ethernetPortIndex) 
                    break;
                else
                    pEntry = pEntry->next;
            } 
            if (pEntry == NULL) 
                return SNMP_ERROR_COMMITFAILED;
/*            
            if ((copy & CPE_ETHERNETPORT_SERIALNUMBER_SET) == CPE_ETHERNETPORT_SERIALNUMBER_SET)
                pEntry->ethernetPortSerialNumber=input.ethernetPortSerialNumber;
            if ((copy & CPE_ETHERNETPORT_BINDINGMODE_SET) == CPE_ETHERNETPORT_BINDINGMODE_SET)
                pEntry->ethernetPortBindingMode=input.ethernetPortBindingMode;
            if ((copy & CPE_ETHERNETPORT_IGMPONOFF_SET) == CPE_ETHERNETPORT_IGMPONOFF_SET)
                pEntry->ethernetPortIgmpOnOff= input.ethernetPortIgmpOnOff;
            if ((copy & CPE_ETHERNETPORT_I8021DPARAMETER_SET) == CPE_ETHERNETPORT_I8021DPARAMETER_SET)
                pEntry->ethernetPortI8021dParameter= input.ethernetPortI8021dParameter;
*/       
            input.ethernetPortIfIndex = pEntry->ethernetPortIfIndex;
            if ((copy & CPE_ETHERNETPORT_SERIALNUMBER_SET) != CPE_ETHERNETPORT_SERIALNUMBER_SET)
                    input.ethernetPortSerialNumber=pEntry->ethernetPortSerialNumber;
            if ((copy & CPE_ETHERNETPORT_BINDINGMODE_SET) != CPE_ETHERNETPORT_BINDINGMODE_SET)
                    input.ethernetPortBindingMode=pEntry->ethernetPortBindingMode;
            if ((copy & CPE_ETHERNETPORT_IGMPONOFF_SET) != CPE_ETHERNETPORT_IGMPONOFF_SET)
                    input.ethernetPortIgmpOnOff=pEntry->ethernetPortIgmpOnOff;
            if ((copy & CPE_ETHERNETPORT_I8021DPARAMETER_SET) != CPE_ETHERNETPORT_I8021DPARAMETER_SET)
                    input.ethernetPortI8021dParameter=pEntry->ethernetPortI8021dParameter ;

            ret = cpeConfigEthernetPortTable(&input);

            if (ret == -1) 
            {
                return SNMP_ERROR_COMMITFAILED;
            }
        }
        break;

        case FREE:
        set = 0;
        memset(&input,0,sizeof(CPE_ETHERNETPORT_TABLE_INFO));
        break;
    } /* switch */
    return SNMP_ERROR_NOERROR;

}

/* 
 * Interfaces IP address entry
 */
unsigned char *
var_cpeEthernetPortEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType,
               snmp_info_t *mesg, WRITE_METHOD *write_method)
{
    int column = newoid->name[(newoid->namelen - 1)];
    int result;
    pCPE_ETHERNETPORT_TABLE_INFO pEntry=NULL;
    int index;

    *write_method = (WRITE_METHOD)write_cpeEthernetPortEntry;
    index = newoid->namelen;
    newoid->namelen++;
    if( mesg->pdutype != SNMP_SET_REQ_PDU ) 
    {
        cpeScanEthernetPortTable();
        pEntry = pEthernetPortList->head;
        while (pEntry) 
        {
            newoid->name[index] = pEntry->ethernetPortIfIndex;
            /* Determine whether it is the requested OID	*/
            result = compare(reqoid, newoid);
            if (((searchType == EXACT) && (result == 0)) ||((searchType == NEXT) && (result < 0)))
                break; /* found */
            pEntry = pEntry->next;
        } 
        if (pEntry == NULL)
            return NO_MIBINSTANCE;

        *var_len = sizeof(long);

        switch (column) 
        {
            case I_cpeEthernetPortIfIndex:
                return (unsigned char *)&pEntry->ethernetPortIfIndex;
            case I_cpeEthernetPortSerialNumber:
                return (unsigned char *)&pEntry->ethernetPortSerialNumber;
            case I_cpeEthernetPortBindingMode:
                return (unsigned char *)&pEntry->ethernetPortBindingMode;
            case I_cpeEthernetPortIgmpOnOff:
                return (unsigned char *)&pEntry->ethernetPortIgmpOnOff;
            case I_cpeEthernetPortI8021dParameter:
                return (unsigned char *)&pEntry->ethernetPortI8021dParameter;
            default:
                return NO_MIBINSTANCE;
         } /* switch */
    } /* != SNMP_SET_REQ_PDU */
    else 
    {
        newoid->name[index] = reqoid->name[index];
        /* return whatever as long as it's non-zero */
        return (unsigned char*)&long_return;
    }
}

int write_cpeServiceTypeEntry(int action,unsigned char *var_val, unsigned char varval_type, 
            int var_val_len, unsigned char *statP, Oid *name, int name_len)
{
    int serviceTypeIndex, column, value;
    int namelen;
    int intValue;
    pCPE_SERVICETYPE_TABLE_INFO pEntry= NULL;
    static CPE_SERVICETYPE_TABLE_INFO serviceTypeEntry;
    int i, ret;
    static int set=0, copy=0;
    static int willCreate = 0; 
    static int created = 0;
    static int start=1;
    static CPE_SERVICETYPE_TABLE_INFO input;

    namelen = (int)name->namelen;
    column =  (int)name->name[namelen-2];
    serviceTypeIndex = (int)name->name[namelen-1];

    if (start) 
    {
        input.pvcMcr=0;
        input.pvcPcr=0;
        input.pvcScr=0;
        input.pvcServiceType=CPE_SERVICETYPE_UBR;
        start = 0;
    }

    if ((set==0) && (copy==0))
        memset(&input,0,sizeof(CPE_SERVICETYPE_TABLE_INFO));

#ifdef BUILD_SNMP_DEBUG
    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
    {
        printf("write_cpeServiceTypeEntry(entry): column %d\n",column);
    }
#endif

    switch (action) 
    {
        case RESERVE1:
        switch (column) 
        {
            case I_cpeServiceTypeIndex:
                return SNMP_ERROR_NOTWRITABLE;
                break;

            case I_cpePvcServiceType:
                if (varval_type != SNMP_INTEGER) 
                {
#ifdef BUILD_SNMP_DEBUG
                    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                    {
                        printf("check: PvcServiceType, invalid var_type %d (SNMP_INTEGER %d)\n",
                               varval_type,SNMP_INTEGER);
                    }
#endif
                    return SNMP_ERROR_WRONGTYPE;
                }

                if (var_val_len > sizeof(int)) 
                {
#ifdef BUILD_SNMP_DEBUG
                    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                    {
                        printf("check: PvcServiceType, invalid val_len %d\n",var_val_len);
                    }
#endif
                    return SNMP_ERROR_WRONGLENGTH;
                }
                set |= CPE_PVCSERVICETYPE_SET;
                copy = set;
                break;

             case I_cpePvcPcr:
                if (varval_type != SNMP_INTEGER) 
                {
#ifdef BUILD_SNMP_DEBUG
                    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                    {
                        printf("check: PvcPcr, invalid var_type %d (SNMP_INTEGER %d)\n",
                               varval_type,SNMP_INTEGER);
                    }
#endif
                    return SNMP_ERROR_WRONGTYPE;
                }

                if (var_val_len > sizeof(int)) 
                {
#ifdef BUILD_SNMP_DEBUG
                    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                    {
                        printf("check: PvcPcr, invalid val_len %d\n",var_val_len);
                    }
#endif
                    return SNMP_ERROR_WRONGLENGTH;
                }
                set |= CPE_PVC_PCR_SET;
                copy = set;
                break;

            case I_cpePvcMcr:
                if (varval_type != SNMP_INTEGER) 
                {
#ifdef BUILD_SNMP_DEBUG
                    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                    {
                        printf("check: PvcMcr, invalid var_type %d (SNMP_INTEGER %d)\n",
                                varval_type,SNMP_INTEGER);
                    }
#endif
                    return SNMP_ERROR_WRONGTYPE;
                }

                if (var_val_len > sizeof(int)) 
                {
#ifdef BUILD_SNMP_DEBUG
                    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                    {
                        printf("check: PvcMcr, invalid val_len %d\n",var_val_len);
                    }
#endif
                    return SNMP_ERROR_WRONGLENGTH;
                }
                set |= CPE_PVC_MCR_SET;
                copy = set;
                break;

            case I_cpePvcScr:
                if (varval_type != SNMP_INTEGER) 
                {
#ifdef BUILD_SNMP_DEBUG
                    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                    {
                        printf("check: cpePvcScr, invalid var_type %d (SNMP_INTEGER %d)\n",
                                varval_type,SNMP_INTEGER);
                    }
#endif
                    return SNMP_ERROR_WRONGTYPE;
                }

                if (var_val_len > sizeof(int)) 
                {
#ifdef BUILD_SNMP_DEBUG
                    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
                    {
                        printf("check: cpePvcScr, invalid val_len %d\n",var_val_len);
                    }
#endif
                    return SNMP_ERROR_WRONGLENGTH;
                }
                set |= CPE_PVC_SCR_SET;
                copy = set;
                break;

            case I_cpeServiceTypeRowStatus:
                if (varval_type != SNMP_INTEGER)
                    return SNMP_ERROR_WRONGTYPE;
                if (var_val_len > sizeof(int)) 
                    return SNMP_ERROR_WRONGLENGTH;
                value = (int)*var_val;
                if ((value < CPE_ROW_STATUS_CREATE_AND_GO) || 
                    (value > CPE_ROW_STATUS_DESTROY))
                    return SNMP_ERROR_WRONGVALUE;
                if ((value == CPE_ROW_STATUS_CREATE_AND_GO) ||
                    (value == CPE_ROW_STATUS_CREATE_AND_WAIT)) 
                {
                    willCreate = value;
                }
                break;

            default:
            return SNMP_ERROR_NOTWRITABLE;
        } /* switch column */
        break;

        case RESERVE2:
        break;

        case COMMIT:

        switch (column) 
        {
            case I_cpePvcServiceTypeIndex:
                return SNMP_ERROR_NOTWRITABLE;
                break;

            case I_cpePvcServiceType:
                intValue = *((int*)var_val);
                for (i = 0; i < (sizeof(int)-var_val_len); i++)
                {
                    intValue = intValue >> 8;
                }
                input.pvcServiceType= intValue;
                set &= ~CPE_PVCSERVICETYPE_SET;
                break;

            case I_cpePvcPcr:
                intValue = *((int*)var_val);
                for (i = 0; i < (sizeof(int)-var_val_len); i++)
                {
                    intValue = intValue >> 8;
                }
                input.pvcPcr= intValue;
                set &= ~CPE_PVC_PCR_SET;   
                break;

            case I_cpePvcMcr:
                intValue = *((int*)var_val);
                for (i = 0; i < (sizeof(int)-var_val_len); i++)
                {
                    intValue = intValue >> 8;
                }
                input.pvcMcr= intValue;
                set &= ~CPE_PVC_MCR_SET;	
                break;

            case I_cpePvcScr:
                intValue = *((int*)var_val);
                for (i = 0; i < (sizeof(int)-var_val_len); i++)
                {
                    intValue = intValue >> 8;
                }
                input.pvcScr= intValue;
                set &= ~CPE_PVC_SCR_SET; 
                break;

            case I_cpeServiceTypeRowStatus:
                start=1;
                intValue = *((int*)var_val);
                for (i = 0; i < (sizeof(int)-var_val_len); i++)
                {
                    intValue = intValue >> 8;
                }
                input.serviceTypeRowStatus= intValue;
                set &= ~CPE_SERVICETYPE_ROWSTATUS_SET; 
                break;

            default:
                break;
        }       /* switch */

        if (set == 0) 
        {
            /* index, column, */
            serviceTypeIndex = (int)name->name[namelen-1];

#ifdef BUILD_SNMP_DEBUG
            if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) 
            {
                printf("commit: serviceTypeIndex %d\n",serviceTypeIndex);
            }
#endif

            cpeScanServiceTypeTable();

            pEntry = pServiceTypeList->head;

            while (pEntry) 
            {
                if (pEntry->serviceTypeIndex== serviceTypeIndex) 
                    break;
                else
                    pEntry = pEntry->next;
            } 
            if (pEntry == NULL) 
            {
                if (willCreate) 
                {
                    input.serviceTypeRowStatus=willCreate;
                    willCreate = 0;
                    if ((ret = TdAddEntry(&input)) == -1) 
                    {
                        return SNMP_ERROR_COMMITFAILED;
                    }
                    else if (ret == 0) 
                    {
                         created = 1;
                    }
                  }

                  /* scan again to see if an entry is created successfully */
                  cpeScanServiceTypeTable();
                  pEntry = pWanList->head;
                  while (pEntry) 
                  {
                    if (pEntry->serviceTypeIndex== serviceTypeIndex) 
                      break;
                    else
                      pEntry = pEntry->next;
                  }
                  if (pEntry == NULL) 
                  {
                    copy = 0;
                    set = 0;
                    willCreate=0;
                    created=0;
                    return SNMP_ERROR_COMMITFAILED;   
                  }
            }/* pEntry == NULL */
            else
            {

                if ((copy & CPE_PVCSERVICETYPE_SET) == CPE_PVCSERVICETYPE_SET)
                    pEntry->pvcServiceType=input.pvcServiceType;
                if ((copy & CPE_PVC_PCR_SET) == CPE_PVC_PCR_SET)
                    pEntry->pvcPcr=input.pvcPcr;
                if ((copy & CPE_PVC_MCR_SET) == CPE_PVC_MCR_SET)
                    pEntry->pvcMcr= input.pvcMcr;
                if ((copy & CPE_PVC_SCR_SET) == CPE_PVC_SCR_SET)
                    pEntry->pvcScr= input.pvcScr;
                if ((copy & CPE_SERVICETYPE_ROWSTATUS_SET) == CPE_SERVICETYPE_ROWSTATUS_SET)
                    pEntry->serviceTypeRowStatus= input.serviceTypeRowStatus;

                ret = cpeConfigServiceTypeTable(pEntry);

                if (ret == -1) 
                {
                    return SNMP_ERROR_COMMITFAILED;
                }
            }
        }
        break;

        case FREE:
            set = 0;
            memset(&input,0,sizeof(CPE_SERVICETYPE_TABLE_INFO));
            break;
    } /* switch */
    return SNMP_ERROR_NOERROR;

}

/* 
 * Interfaces IP address entry
 */
unsigned char *
var_cpeServiceTypeEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType,
               snmp_info_t *mesg, WRITE_METHOD *write_method)
{
    int column = newoid->name[(newoid->namelen - 1)];
    int result;
    pCPE_SERVICETYPE_TABLE_INFO pEntry;
    int index;

    *write_method = (WRITE_METHOD)write_cpeServiceTypeEntry;
    index = newoid->namelen;
    newoid->namelen++;
    if( mesg->pdutype != SNMP_SET_REQ_PDU ) 
    {

        cpeScanServiceTypeTable();
        pEntry = pServiceTypeList->head;

        while (pEntry) 
        {
            newoid->name[index] = pEntry->serviceTypeIndex;
            /* Determine whether it is the requested OID	*/
            result = compare(reqoid, newoid);
            if (((searchType == EXACT) && (result == 0)) ||((searchType == NEXT) && (result < 0)))
                break; /* found */
            pEntry = pEntry->next;
        } 

        if (pEntry == NULL)
            return NO_MIBINSTANCE;

        *var_len = sizeof(long);

        switch (column) 
        {
            case I_cpeServiceTypeIndex:
                return (unsigned char *)&pEntry->serviceTypeIndex;
            case I_cpePvcServiceType:
                return (unsigned char *)&pEntry->pvcServiceType;
            case I_cpePvcPcr:
                return (unsigned char *)&pEntry->pvcPcr;
            case I_cpePvcMcr:
                return (unsigned char *)&pEntry->pvcMcr;
            case I_cpePvcScr:
                return (unsigned char *)&pEntry->pvcScr;
            case I_cpeServiceTypeRowStatus:
                return (unsigned char *)&pEntry->serviceTypeRowStatus;

            default:
            return NO_MIBINSTANCE;
        } /* switch */
    } /* != SNMP_SET_REQ_PDU */
    else 
    {
        newoid->name[index] = reqoid->name[index];
        /* return whatever as long as it's non-zero */
        return (unsigned char*)&long_return;
    }

}


int write_cpeAdminIdentity(int action,
    unsigned char *var_val, unsigned char varval_type, int var_val_len,
    unsigned char *statP, oid *name, int name_len)
{
    char userName[IFC_TINY_LEN];

    switch (action) 
    {
    case RESERVE1:
      if (varval_type != SNMP_STRING)
        return SNMP_ERROR_WRONGTYPE;
      if (var_val_len > CPE_CONFIG_ID_MAX_LEN) 
        return SNMP_ERROR_WRONGLENGTH;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      memset(userName,0,IFC_TINY_LEN);
      memcpy(userName,var_val,IFC_TINY_LEN);
      
      BcmDb_setSysUserName(userName, IFC_TINY_LEN) ;
      break;
    case FREE:
      break;
    }
    return SNMP_ERROR_NOERROR;
}


unsigned char *
var_cpeAdminIdentity(int *var_len, snmp_info_t *mesg,
                     WRITE_METHOD *write_method)
{
    char userName[IFC_TINY_LEN];
    *write_method = (WRITE_METHOD)write_cpeAdminIdentity;
    // if user name does not exist
    BcmDb_getSysUserName(userName, IFC_TINY_LEN) ;
    *var_len = strlen(userName);
    return (unsigned char *)(userName);
}

int write_cpeAdminPassword(int action,
        unsigned char *var_val, unsigned char varval_type, int var_val_len,
        unsigned char *statP, oid *name, int name_len)
{
        char Password[IFC_TINY_LEN];
        char sptPassword[IFC_TINY_LEN];
        char usrPassword[IFC_TINY_LEN];
        switch (action) 
        {
            case RESERVE1:
                if (varval_type != SNMP_STRING)
                return SNMP_ERROR_WRONGTYPE;
                if (var_val_len > CPE_CONFIG_ID_MAX_LEN) 
                return SNMP_ERROR_WRONGLENGTH;
                break;
            case RESERVE2:
                break;
            case COMMIT:
                memset(Password,0,IFC_TINY_LEN);
                memcpy(Password,var_val,IFC_TINY_LEN);
                BcmDb_setSysPassword(Password, IFC_TINY_LEN) ;
                BcmDb_getSptPassword(sptPassword,IFC_TINY_LEN);
                BcmDb_getUsrPassword(usrPassword, IFC_TINY_LEN);
                bcmCreateLoginCfg(Password, sptPassword, usrPassword);
                break;

            case FREE:
            break;
        }
        return SNMP_ERROR_NOERROR;
}


unsigned char *
var_cpeAdminPassword(int *var_len, snmp_info_t *mesg,
        WRITE_METHOD *write_method)
{
	
     char *Password = "****";
     *write_method = (WRITE_METHOD)write_cpeAdminPassword;
     *var_len = strlen(Password);
     return (unsigned char *)(Password);

}

#endif
/* 
 * the primary DNS IP address
 */
int     write_cpeDnsPrimaryAddress(int action,
        unsigned char *var_val, unsigned char varval_type, int var_val_len,
        unsigned char *statP, oid *name, int name_len)
{
  int error;
  struct in_addr ip;

  switch (action) 
    {
    case RESERVE1:
      if (varval_type != SNMP_IPADDRESS)
        return SNMP_ERROR_WRONGTYPE;
      if (var_val_len != sizeof(int)) 
        return SNMP_ERROR_WRONGLENGTH;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      ip.s_addr = *(int*)var_val;
      error = cpeSetDnsAddr(CPE_DNS_PRIMARY,&ip);
      if (error == -1)
        return SNMP_ERROR_COMMITFAILED;
      break;
    case FREE:
      break;
    }
  return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpeDnsPrimaryAddress(int *var_len, snmp_info_t *mesg,
        WRITE_METHOD *write_method)
{
  struct in_addr ip;

  *write_method = (WRITE_METHOD)write_cpeDnsPrimaryAddress;
  *var_len = sizeof(long);

  cpeGetDnsAddr(CPE_DNS_PRIMARY,&ip);
  long_return = ip.s_addr;
  return (unsigned char *)&long_return;
}

/* 
 * the secondary DNS IP address
 */
int     write_cpeDnsSecondAddress(int action,
        unsigned char *var_val, unsigned char varval_type, int var_val_len,
        unsigned char *statP, oid *name, int name_len)
{
  int error;
  struct in_addr ip;
   
  switch (action) 
    {
    case RESERVE1:
      if (varval_type != SNMP_IPADDRESS)
        return SNMP_ERROR_WRONGTYPE;
      if (var_val_len != sizeof(int)) 
        return SNMP_ERROR_WRONGLENGTH;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      ip.s_addr = *(int*)var_val;
      error = cpeSetDnsAddr(CPE_DNS_SECONDARY,&ip);
      if (error == -1)
        return SNMP_ERROR_COMMITFAILED;
      break;
    case FREE:
      break;
    }
  return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpeDnsSecondAddress(int *var_len, snmp_info_t *mesg,
        WRITE_METHOD *write_method)
{
  struct in_addr ip;

  *write_method = (WRITE_METHOD)write_cpeDnsSecondAddress; 
  *var_len = sizeof(long);

  cpeGetDnsAddr(CPE_DNS_SECONDARY,&ip);
  long_return = ip.s_addr;
  return (unsigned char *)&long_return;
}

/* 
 * syslog requery begin time
 */
/*
int     write_cpeSysLogBeginTime(int action,
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
var_cpeSysLogBeginTime(int *var_len, snmp_info_t *mesg,
        WRITE_METHOD *write_method)
{
  /* Add value computations */

  /* Set write-function (uncomment if you want to implement it)  */
  /* *write_method = &write_cpeSysLogBeginTime(); */
  /* Set size (in bytes) and return address of the variable */

  /* time is in the format specified in snmpv2-tc.txt :
     DateAndTime ::= TEXTUAL-CONVENTION
              DISPLAY-HINT "2d-1d-1d,1d:1d:1d.1d,1a1d:1d"
              STATUS       current
              DESCRIPTION
                      "A date-time specification.

                       field  octets  contents                  range
                       -----  ------  --------                  -----
                         1      1-2   year                      0..65536
                         2       3    month                     1..12
                         3       4    day                       1..31
                         4       5    hour                      0..23
                         5       6    minutes                   0..59
                         6       7    seconds                   0..60
                                      (use 60 for leap-second)
                         7       8    deci-seconds              0..9
                         8       9    direction from UTC        '+' / '-'
                         9      10    hours from UTC            0..11
                        10      11    minutes from UTC          0..59

                      For example, Tuesday May 26, 1992 at 1:30:15 PM
                      EDT would be displayed as:

                                  1992-5-26,13:30:15.0,-4:0
  */
  *var_len = sizeof(long);
  return (unsigned char *) NO_MIBINSTANCE;
}

/* 
 * syslog requerys end time
 */
/*
int     write_cpeSysLogEndTime(int action,
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
var_cpeSysLogEndTime(int *var_len, snmp_info_t *mesg,
        WRITE_METHOD *write_method)
{
    /* Add value computations */

    /* Set write-function (uncomment if you want to implement it)  */
    /* *write_method = &write_cpeSysLogEndTime(); */
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    return (unsigned char *) NO_MIBINSTANCE;
}

/* 
 * requery syslog operation 
 * start(1), start requery syslog 
 * clear(2), clear CPEs syslog record
 */
/*
int     write_cpeSysLogAdminStatus(int action,
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
var_cpeSysLogAdminStatus(int *var_len, snmp_info_t *mesg,
        WRITE_METHOD *write_method)
{
    /* Add value computations */

    /* Set write-function (uncomment if you want to implement it)  */
    /* *write_method = &write_cpeSysLogAdminStatus(); */
    /* Set size (in bytes) and return address of the variable */
    *var_len = sizeof(long);
    return (unsigned char *) NO_MIBINSTANCE;
}

/* 
 * syslog records entry 
 */

unsigned char *
var_cpeSysLogEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType,
                   snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  /* Last subOID of COLUMNAR OID is column */
  int   column = newoid->name[(newoid->namelen - 1)];
  int   result;

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
        case I_cpeSysLogIndex:
            return (unsigned char *) NO_MIBINSTANCE;
        case I_cpeSysLogContent:
            return (unsigned char *) NO_MIBINSTANCE;
        default:
            return NO_MIBINSTANCE;
    }
}
#endif
void cpeInitFtpData(void)
{
  memset(pFtpData,0,sizeof(CPE_FTP_INFO));
  strcpy(pFtpData->filename,CPE_FTP_DEFAULT_FILE);
  pFtpData->operStatus = CPE_FTP_OPER_STATUS_NORMAL;
}

/* 
 * the FTP Servers IP address
 */
int write_cpeFtpIp(int action, unsigned char *var_val, unsigned char varval_type, int var_val_len,
                   unsigned char *statP, oid *name, int name_len)
{
  switch (action)
  {
    case RESERVE1:
      if (varval_type != SNMP_IPADDRESS)
        return SNMP_ERROR_WRONGTYPE;
      if (var_val_len != sizeof(int)) 
        return SNMP_ERROR_WRONGLENGTH;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      pFtpData->addr.s_addr = *(int*)var_val;
      break;
    case FREE:
      break;
  }
  return SNMP_ERROR_NOERROR;
}


unsigned char *
var_cpeFtpIp(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpeFtpIp;
  *var_len = sizeof(long);
  return (unsigned char *)&pFtpData->addr;
}

/* 
 * the user name for FTP
 */
int write_cpeFtpUser(int action, unsigned char *var_val, unsigned char varval_type, 
                     int var_val_len, unsigned char *statP, oid *name, int name_len)
{
  int value;
  switch (action) 
    {
    case RESERVE1:
      if (varval_type != SNMP_STRING)
        return SNMP_ERROR_WRONGTYPE;
      if (var_val_len > CPE_FTP_NAME_MAX_LEN)
        return SNMP_ERROR_WRONGLENGTH;
      value = (int)*var_val;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      memset(pFtpData->user,0,CPE_FTP_NAME_MAX_LEN);
      cmsUtl_strncpy(pFtpData->user,var_val,var_val_len);
      break;
    case FREE:
      break;
    }
  return SNMP_ERROR_NOERROR;
}

unsigned char *var_cpeFtpUser(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpeFtpUser;
  *var_len = strlen(pFtpData->user);
  return (unsigned char *)(pFtpData->user);
}

/* 
 * the access password for FTP
 */
int write_cpeFtpPasswd(int action, unsigned char *var_val, unsigned char varval_type,
                       int var_val_len, unsigned char *statP, oid *name, int name_len)
{
    switch (action) 
      {
      case RESERVE1:
        if (varval_type != SNMP_STRING)
          return SNMP_ERROR_WRONGTYPE;
        if (var_val_len > CPE_FTP_PASSWORD_MAX_LEN) 
          return SNMP_ERROR_WRONGLENGTH;
        break;
      case RESERVE2:
        break;
      case COMMIT:
        memset(pFtpData->password,0,CPE_FTP_PASSWORD_MAX_LEN);
        cmsUtl_strncpy(pFtpData->password,var_val,var_val_len);
        break;
      case FREE:
        break;
      }
    return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpeFtpPasswd(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpeFtpPasswd;
  *var_len = strlen(pFtpData->password);
  return (unsigned char *)(pFtpData->password);
}

/* 
 * the file name for ftp download which is saved in FTP server.
 */
int write_cpeFtpFileName(int action, unsigned char *var_val, unsigned char varval_type,
                         int var_val_len, unsigned char *statP, oid *name, int name_len)
{
    switch (action) 
      {
      case RESERVE1:
        if (varval_type != SNMP_STRING)
          return SNMP_ERROR_WRONGTYPE;
        if ((var_val_len < CPE_FTP_FILE_NAME_MIN_LEN) ||
            (var_val_len > CPE_FTP_FILE_NAME_MAX_LEN))
          return SNMP_ERROR_WRONGLENGTH;
        break;
      case RESERVE2:
        break;
      case COMMIT:
        memset(pFtpData->filename,0,CPE_FTP_FILE_NAME_MAX_LEN);
        cmsUtl_strncpy(pFtpData->filename,var_val,var_val_len);        
        break;
      case FREE:
        break;
      }
    return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpeFtpFileName(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpeFtpFileName;
  *var_len = strlen(pFtpData->filename);
  return (unsigned char *)(pFtpData->filename);
}

/* 
 * FTP administration operation
 */
int write_cpeFtpAdminStatus(int action, unsigned char *var_val, unsigned char varval_type, 
                            int var_val_len, unsigned char *statP, oid *name, 
                            SNMP_SET_HANDLER *set_handler, unsigned long *param)
{
  int value;

  value = (int)*var_val;

  switch (action) 
    {
    case RESERVE1:
      if (varval_type != SNMP_INTEGER)
        return SNMP_ERROR_WRONGTYPE;
      if (var_val_len > sizeof(int)) 
        return SNMP_ERROR_WRONGLENGTH;
      if ((value < CPE_FTP_ADMIN_STATUS_STOP) ||
          (value > CPE_FTP_OPER_STATUS_UPGRADE_FAIL))
        return SNMP_ERROR_WRONGVALUE;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      if ((pFtpData->adminStatus == value) && 
          ((pFtpData->operStatus == CPE_FTP_OPER_STATUS_DOWNLOADING) || 
           (pFtpData->operStatus == CPE_FTP_OPER_STATUS_SAVING)))
        break;
      switch (value)
        {
        case CPE_FTP_ADMIN_STATUS_STOP:
          cpeFtpStop();
          break;
        case CPE_FTP_ADMIN_STATUS_UPGRADE:
          if ((pFtpData->filename[0] != '\0') && (pFtpData->addr.s_addr != 0)) {
            *set_handler = (SNMP_SET_HANDLER)cpeFtpUpgrade;
            *param = (unsigned long) pFtpData;
          }
          break;
        case CPE_FTP_ADMIN_STATUS_FTPTEST:
          if ((pFtpData->filename[0] != '\0') && (pFtpData->addr.s_addr != 0)) {
            *set_handler = (SNMP_SET_HANDLER)cpeFtpTest;
            *param = (unsigned long) pFtpData;
            pFtpData->adminStatus = CPE_FTP_ADMIN_STATUS_FTPTEST;
            pFtpData->totalSize = 0;
            pFtpData->doneSize = 0;
            pFtpData->elapseTime = 0;
            pFtpData->operStatus = CPE_FTP_OPER_STATUS_DOWNLOADING;
          }
          break;
        }
      pFtpData->adminStatus = value;
      break;
    case FREE:
      break;
    }
  return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpeFtpAdminStatus(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpeFtpAdminStatus;
  *var_len = sizeof(long);
  return (unsigned char *)&(pFtpData->adminStatus);
}

/* 
 * FTP administration status
 */
unsigned char *var_cpeFtpOperStatus(int *var_len, snmp_info_t *mesg,
                                    WRITE_METHOD *write_method)
{
    *var_len = sizeof(long);
    return (unsigned char *)&(pFtpData->operStatus);
}

/* 
 * the files total size for download?unit is Kbyte 
 */
unsigned char *
var_cpeFtpTotalSize(int *var_len, snmp_info_t *mesg,
        WRITE_METHOD *write_method)
{
    *var_len = sizeof(long);
    return (unsigned char *)&(pFtpData->totalSize);
}

/* 
 * the files size which is already download,unit is Kbyte
 */
unsigned char *var_cpeFtpDoneSize(int *var_len, snmp_info_t *mesg,
                                  WRITE_METHOD *write_method)
{
  *var_len = sizeof(long);
  return (unsigned char *)&(pFtpData->doneSize);
}

/* 
 * the elapse time for FTP download 
 */
unsigned char *
var_cpeFtpElapseTime(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *var_len = sizeof(long);
  return (unsigned char *)&(pFtpData->elapseTime);
}


#ifdef LGD_TO_DO
void cpeInitPingData(void)
{
  memset(pPingData,0,sizeof(CPE_PING_INFO));
  cmsUtl_strncpy(pPingData->addr,CPE_PING_ADDR_DEFAULT,strlen(CPE_PING_ADDR_DEFAULT));
  pPingData->addrLen = strlen(CPE_PING_ADDR_DEFAULT);
  pPingData->size = CPE_PING_PKT_SIZE_DEFAULT;
  pPingData->count = CPE_PING_COUNT_DEFAULT;
  pPingData->adminStatus = CPE_PING_ADMIN_STOP;
  pPingData->operStatus = CPE_PING_OPER_NORMAL;
  sprintf(pPingData->result,
          "Ping test result: IP = %s Sent = %d Received = %d Lost = %d Min = %d ms Max = %d ms Average = %d ms", "0.0.0.0",0,0,0,0,0,0);
}

/* 
 * the destination IP address for ping test,either IP or Domain name
 */
int write_cpePingAddr(int action, unsigned char *var_val, unsigned char varval_type,
                      int var_val_len, unsigned char *statP, oid *name, int name_len)
{
  switch (action)
  {
    case RESERVE1:
      if (varval_type != SNMP_STRING)
        return SNMP_ERROR_WRONGTYPE;
      if ((var_val_len >= CPE_PING_ADDR_LEN_MAX) || (var_val_len < CPE_PING_ADDR_LEN_MIN))
        return SNMP_ERROR_WRONGLENGTH;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      memset(pPingData->addr, 0, sizeof(pPingData->addr));
      cmsUtl_strncpy(pPingData->addr,var_val,var_val_len);
      pPingData->addrLen = var_val_len;
      break;
    case FREE:
      break;
  }
  return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpePingAddr(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpePingAddr;
  
  *var_len = pPingData->addrLen;
  return (unsigned char *)pPingData->addr;
}

/* 
 * the packet size for ping test 
 */
int write_cpePingPkgSize(int action, unsigned char *var_val, unsigned char varval_type,
                         int var_val_len, unsigned char *statP, oid *name, int name_len)
{
  int value;

  value = cpeGetInt(var_val,var_val_len);

  switch (action)
    {
    case RESERVE1:
      if (varval_type != SNMP_INTEGER)
        return SNMP_ERROR_WRONGTYPE;
      if ((value < CPE_PING_PKT_SIZE_DEFAULT) ||
          (value > CPE_PING_PKT_SIZE_MAX)) {
        return SNMP_ERROR_WRONGVALUE;
      }
      break;
    case RESERVE2:
      break;
    case COMMIT:
      pPingData->size = value;
      break;
    case FREE:
      break;
    }
  return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpePingPkgSize(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpePingPkgSize;

  *var_len = sizeof(long);
  return (unsigned char *)&pPingData->size;
}

/* 
 * the count of sending packets for ping test
 */
int write_cpePingCount(int action, unsigned char *var_val, unsigned char varval_type,
                       int var_val_len, unsigned char *statP, oid *name, int name_len)
{
  int value;

  value = cpeGetInt(var_val,var_val_len);
  switch (action) 
    {
    case RESERVE1:
      if (varval_type != SNMP_INTEGER)
        return SNMP_ERROR_WRONGTYPE;
      if ((value < CPE_PING_COUNT_MIN) ||
          (value > CPE_PING_COUNT_MAX))
        return SNMP_ERROR_WRONGVALUE;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      pPingData->count = value;
      break;
    case FREE:
      break;
    }
  return SNMP_ERROR_NOERROR;
}


unsigned char *
var_cpePingCount(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpePingCount;
  *var_len = sizeof(long);
  return (unsigned char *)&pPingData->count;
}

/* 
 * ping test operation
 * stop(1), start ping test
 * start(2) stop ping test
 * 
 */
int write_cpePingAdminStatus(int action, unsigned char *var_val, unsigned char varval_type,
                             int var_val_len, unsigned char *statP, oid *name, int name_len)
{
  int value;
  static int oldAdminStatus;

  value = (int)*var_val;

  switch (action) 
    {
    case RESERVE1:
      if (varval_type != SNMP_INTEGER)
        return SNMP_ERROR_WRONGTYPE;
      if ((value != CPE_PING_ADMIN_STOP) && (value != CPE_PING_ADMIN_START))
        return SNMP_ERROR_WRONGVALUE;
      break;
      oldAdminStatus = pPingData->adminStatus;
    case RESERVE2:
      break;
    case COMMIT:
      if (value == pPingData->adminStatus)
        break;
      pPingData->adminStatus = value;
      if (value == CPE_PING_ADMIN_STOP)
        cpeStopPing();
      else
        /* go start the ping test */
        if (cpeStartPing() == -1)
          return SNMP_ERROR_COMMITFAILED;
      break;
    case FREE:
      pPingData->adminStatus = oldAdminStatus;
      break;
    }
  return SNMP_ERROR_NOERROR;
}

unsigned char *
var_cpePingAdminStatus(int *var_len, snmp_info_t *mesg,
        WRITE_METHOD *write_method)
{
  *write_method = (WRITE_METHOD)write_cpePingAdminStatus;
  *var_len = sizeof(long);
  return (unsigned char *)&pPingData->adminStatus;
}

/* 
 * ping test operation status   
 */
unsigned char *
var_cpePingOperStatus(int *var_len, snmp_info_t *mesg,
        WRITE_METHOD *write_method)
{
  *var_len = sizeof(long);
  return (unsigned char *)&pPingData->operStatus;
}

/* 
 * ping test result:
 * IP = 10.1.1.100
 * Sent     = xx 
 * Received = xx 
 * Lost     = xx 
 * Min      = xx ms 
 * Max      = xx ms 
 * Average  = xx ms
 * 
 */
unsigned char *
var_cpePingResult(int *var_len, snmp_info_t *mesg,
        WRITE_METHOD *write_method)
{
  *var_len = strlen(pPingData->result); /* take out the return char */
  return (unsigned char *)pPingData->result;
}
#endif

#ifdef LGD_TO_DO
static oid cpeSysInfo_oid[] = { O_cpeSysInfo };
static Object cpeSysInfo_variables[] = {
    { SNMP_INTEGER, (RONLY| SCALAR), (void*)var_cpeSysInfoUptime,
                 {2, { I_cpeSysInfoUptime, 0 }}},
    { SNMP_STRING, (RWRITE| SCALAR), (void*)var_cpeSysInfoConfigId,
                 {2, { I_cpeSysInfoConfigId, 0 }}},
    { SNMP_STRING, (RONLY| SCALAR), (void*)var_cpeSysInfoFirmwareVersion,
                 {2, { I_cpeSysInfoFirmwareVersion, 0 }}},
    { SNMP_STRING, (RONLY| SCALAR), (void*)var_cpeSysInfoProductId,
                 {2, { I_cpeSysInfoProductId, 0 }}},
    { SNMP_STRING, (RONLY| SCALAR), (void*)var_cpeSysInfoVendorId,
                 {2, { I_cpeSysInfoVendorId, 0 }}},
    { SNMP_STRING, (RONLY| SCALAR), (void*)var_cpeSysInfoSystemVersion,
                 {2, { I_cpeSysInfoSystemVersion, 0 }}},
    { SNMP_INTEGER, (RONLY| SCALAR), (void*)var_cpeSysInfoWorkMode,
                 {2, { I_cpeSysInfoWorkMode, 0 }}},
    { SNMP_STRING, (RONLY| SCALAR), (void*)var_cpeSysInfoMibVersion,
                 {2, { I_cpeSysInfoMibVersion, 0 }}},
    { SNMP_STRING, (RONLY| SCALAR), (void*)var_cpeSysInfoSerialNumber,
                 {2, { I_cpeSysInfoSerialNumber, 0 }}},
    { SNMP_STRING, (RONLY| SCALAR), (void*)var_cpeSysInfoManufacturerOui,
                 {2, { I_cpeSysInfoManufacturerOui, 0 }}},
#ifdef CT_SNMP_MIB_VERSION_2
    { SNMP_INTEGER, (RONLY| SCALAR), (void*)var_cpeSysInfoType,
                 {2, { I_cpeSysInfoType, 0 }}},
#endif
    { (int)NULL }
    };
static SubTree cpeSysInfo_tree =  { NULL, cpeSysInfo_variables,
                (sizeof(cpeSysInfo_oid)/sizeof(oid)), cpeSysInfo_oid};

static oid cpeSystem_oid[] = { O_cpeSystem };
static Object cpeSystem_variables[] = {
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpeSystemReboot,
                 {2, { I_cpeSystemReboot, 0 }}},
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpeSystemSave,
                 {2, { I_cpeSystemSave, 0 }}},
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpeSystemRestore,
                 {2, { I_cpeSystemRestore, 0 }}},
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpeSystemTrapEnable,
                 {2, { I_cpeSystemTrapEnable, 0 }}},
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpeBootTrapResponse,
                 {2, { I_cpeBootTrapResponse, 0 }}},
    { SNMP_STRING, (RWRITE| SCALAR), (void*)var_cpeReservedString,
                 {2, { I_cpeReservedString, 0 }}},
    { (int)NULL }
    };
static SubTree cpeSystem_tree =  { NULL, cpeSystem_variables,
                (sizeof(cpeSystem_oid)/sizeof(oid)), cpeSystem_oid};

static oid cpeConfigAdslLine_oid[] = { O_cpeConfigAdslLine };
static Object cpeConfigAdslLine_variables[] = {
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpeConfigAdslLineTrellis,
                 {2, { I_cpeConfigAdslLineTrellis, 0 }}},
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpeConfigAdslLineHandshake,
                 {2, { I_cpeConfigAdslLineHandshake, 0 }}},
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpeConfigAdslLineBitSwapping,
                 {2, { I_cpeConfigAdslLineBitSwapping, 0 }}},
    { (int)NULL }
    };
static SubTree cpeConfigAdslLine_tree =  { NULL, cpeConfigAdslLine_variables,
                (sizeof(cpeConfigAdslLine_oid)/sizeof(oid)), cpeConfigAdslLine_oid};

static oid cpePvcEntry_oid[] = { O_cpePvcEntry };
static Object cpePvcEntry_variables[] = {
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_cpePvcEntry,
                {1, { I_cpePvcId }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_cpePvcEntry,
                {1, { I_cpePvcIfIndex }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePvcEntry,
                {1, { I_cpePvcUpperEncapsulation }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePvcEntry,
                {1, { I_cpePvcAdminStatus }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_cpePvcEntry,
                {1, { I_cpePvcOperStatus }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePvcEntry,
                {1, { I_cpePvcBridgeMode }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePvcEntry,
                {1, { I_cpePvcMacLearnMode }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePvcEntry,
                {1, { I_cpePvcIgmpMode }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePvcEntry,
                {1, { I_cpePvcDhcpClientMode }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePvcEntry,
                {1, { I_cpePvcNatMode }}},
#ifdef CT_SNMP_MIB_VERSION_2
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePvcEntry,
                {1, { I_cpePvcBindingMode }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePvcEntry,
                {1, { I_cpePvcEthernetPortMap }}}, 
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePvcEntry,
                {1, { I_cpePvcServiceTypeIndex }}},                         
#endif             
    { (int)NULL }
    };
static SubTree cpePvcEntry_tree =  { NULL, cpePvcEntry_variables,
                (sizeof(cpePvcEntry_oid)/sizeof(oid)), cpePvcEntry_oid};

static oid cpePppEntry_oid[] = { O_cpePppEntry };
static Object cpePppEntry_variables[] = {
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_cpePppEntry,
                {1, { I_cpePppIfIndex }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePppEntry,
                {1, { I_cpePppPvcIfIndex }}},
    { SNMP_STRING, (RWRITE| COLUMN), (void*)var_cpePppEntry,
                {1, { I_cpePppServiceName }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePppEntry,
                {1, { I_cpePppDisconnectTimeout }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePppEntry,
                {1, { I_cpePppMSS }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePppEntry,
                {1, { I_cpePppRowStatus }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpePppEntry,
                {1, { I_cpePppAdminStatus }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_cpePppEntry,
                {1, { I_cpePppOperStatus }}},
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_cpePppEntry,
                {1, { I_cpePppTestResult }}},
    { (int)NULL }
    };
static SubTree cpePppEntry_tree =  { NULL, cpePppEntry_variables,
                (sizeof(cpePppEntry_oid)/sizeof(oid)), cpePppEntry_oid};

static oid cpeIpEntry_oid[] = { O_cpeIpEntry };
static Object cpeIpEntry_variables[] = {
    { SNMP_UINTEGER, (RONLY| COLUMN), (void*)var_cpeIpEntry,
                {1, { I_cpeIpIndex }}},
    { SNMP_UINTEGER, (RWRITE| COLUMN), (void*)var_cpeIpEntry,
                {1, { I_cpeIpLowerIfIndex }}},
    { SNMP_IPADDRESS, (RWRITE| COLUMN), (void*)var_cpeIpEntry,
                {1, { I_cpeIpAddress }}},
    { SNMP_IPADDRESS, (RWRITE| COLUMN), (void*)var_cpeIpEntry,
                {1, { I_cpeIpNetmask }}},
    { SNMP_IPADDRESS, (RWRITE| COLUMN), (void*)var_cpeIpEntry,
                {1, { I_cpeIpGateway }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpeIpEntry,
                {1, { I_cpeIpRowStatus }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpeIpEntry,
                {1, { I_cpeIpDefaultRowOnOff }}},
    { (int)NULL }
    };
static SubTree cpeIpEntry_tree =  { NULL, cpeIpEntry_variables,
                (sizeof(cpeIpEntry_oid)/sizeof(oid)), cpeIpEntry_oid};

static oid cpeLanDhcp_oid[] = { O_cpeLanDhcp };
static Object cpeLanDhcp_variables[] = {
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpeLanDhcpAdminStatus,
                 {2, { I_cpeLanDhcpAdminStatus, 0 }}},
#ifdef CT_SNMP_MIB_VERSION_2
    { SNMP_IPADDRESS, (RWRITE| SCALAR), (void*)var_cpeDhcpRelayIpAddress,
                 {2, { I_cpeDhcpRelayIpAddress, 0 }}},
#endif
    { (int)NULL }
    };
static SubTree cpeLanDhcp_tree =  { NULL, cpeLanDhcp_variables,
                (sizeof(cpeLanDhcp_oid)/sizeof(oid)), cpeLanDhcp_oid};

static oid cpeWanDns_oid[] = { O_cpeWanDns };
static Object cpeWanDns_variables[] = {
    { SNMP_IPADDRESS, (RWRITE| SCALAR), (void*)var_cpeDnsPrimaryAddress,
                 {2, { I_cpeDnsPrimaryAddress, 0 }}},
    { SNMP_IPADDRESS, (RWRITE| SCALAR), (void*)var_cpeDnsSecondAddress,
                 {2, { I_cpeDnsSecondAddress, 0 }}},
    { (int)NULL }
    };
static SubTree cpeWanDns_tree =  { NULL, cpeWanDns_variables,
                (sizeof(cpeWanDns_oid)/sizeof(oid)), cpeWanDns_oid};

#ifdef CT_SNMP_MIB_VERSION_2
/*************************QoS***********************/
static oid cpeQos_oid[] = { O_cpeQos };
static Object cpeQos_variables[] = {
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpeQosTagMode,
                 {2, { I_cpeQosTagMode, 0 }}},
    { (int)NULL }
    };
static SubTree cpeQos_tree =  { NULL, cpeQos_variables,
                (sizeof(cpeQos_oid)/sizeof(oid)), cpeQos_oid};
/*************************EthernetPortMap*************/
static oid cpeEthernetPortEntry_oid[] = { O_cpeEthernetPortEntry };
static Object cpeEthernetPortEntry_variables[] = {
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_cpeEthernetPortEntry,
                {1, { I_cpeEthernetPortIfIndex }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpeEthernetPortEntry,
                {1, { I_cpeEthernetPortSerialNumber }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpeEthernetPortEntry,
                {1, { I_cpeEthernetPortBindingMode }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpeEthernetPortEntry,
                {1, { I_cpeEthernetPortIgmpOnOff }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpeEthernetPortEntry,
                {1, { I_cpeEthernetPortI8021dParameter }}},
    { (int)NULL }
    };
static SubTree cpeEthernetPortEntry_tree =  { NULL, cpeEthernetPortEntry_variables,
               (sizeof(cpeEthernetPortEntry_oid)/sizeof(oid)), cpeEthernetPortEntry_oid};
/**************************ServiceType*******************/
static oid cpeServiceTypeEntry_oid[] = { O_cpeServiceTypeEntry };
static Object cpeServiceTypeEntry_variables[] = {
    { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_cpeServiceTypeEntry,
                {1, { I_cpeServiceTypeIndex}}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpeServiceTypeEntry,
                {1, { I_cpePvcServiceType}}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpeServiceTypeEntry,
                {1, { I_cpePvcPcr }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpeServiceTypeEntry,
                {1, { I_cpePvcMcr }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpeServiceTypeEntry,
                {1, { I_cpePvcScr }}},
    { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_cpeServiceTypeEntry,
                {1, { I_cpeServiceTypeRowStatus}}},
    { (int)NULL }
    };
static SubTree cpeServiceTypeEntry_tree = { NULL, cpeServiceTypeEntry_variables,
               (sizeof(cpeServiceTypeEntry_oid)/sizeof(oid)), cpeServiceTypeEntry_oid};
/***************************Admin*************************/
static oid cpeAdmin_oid[] = { O_cpeAdmin };
static Object cpeAdmin_variables[] = {
    { SNMP_STRING, (RONLY| SCALAR), (void*)var_cpeAdminIdentity,
                {2, { I_cpeAdminIdentity, 0 }}},
       { SNMP_STRING, (RWRITE| SCALAR), (void*)var_cpeAdminPassword,
                {2, { I_cpeAdminPassword, 0 }}},
    { (int)NULL }
    };
static SubTree cpeAdmin_tree =  { NULL, cpeAdmin_variables,
               (sizeof(cpeAdmin_oid)/sizeof(oid)), cpeAdmin_oid};

#endif

static oid cpeSysLog_oid[] = { O_cpeSysLog };
static Object cpeSysLog_variables[] = {
    { SNMP_STRING, (RWRITE| SCALAR), (void*)var_cpeSysLogBeginTime,
                 {2, { I_cpeSysLogBeginTime, 0 }}},
    { SNMP_STRING, (RWRITE| SCALAR), (void*)var_cpeSysLogEndTime,
                 {2, { I_cpeSysLogEndTime, 0 }}},
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpeSysLogAdminStatus,
                 {2, { I_cpeSysLogAdminStatus, 0 }}},
    { SNMP_UINTEGER, (RONLY| COLUMN), (void*)var_cpeSysLogEntry,
                {3, { I_cpeSysLogTable, I_cpeSysLogEntry, I_cpeSysLogIndex }}},
    { SNMP_STRING, (RONLY| COLUMN), (void*)var_cpeSysLogEntry,
                {3, { I_cpeSysLogTable, I_cpeSysLogEntry, I_cpeSysLogContent }}},
    { (int)NULL }
    };
static SubTree cpeSysLog_tree =  { NULL, cpeSysLog_variables,
                (sizeof(cpeSysLog_oid)/sizeof(oid)), cpeSysLog_oid};
#endif

static oid cpeFtp_oid[] = { O_cpeFtp };
static Object cpeFtp_variables[] = {
    { SNMP_IPADDRESS, (RWRITE| SCALAR), (void*)var_cpeFtpIp,
                 {2, { I_cpeFtpIp, 0 }}},
    { SNMP_STRING, (RWRITE| SCALAR), (void*)var_cpeFtpUser,
                 {2, { I_cpeFtpUser, 0 }}},
    { SNMP_STRING, (RWRITE| SCALAR), (void*)var_cpeFtpPasswd,
                 {2, { I_cpeFtpPasswd, 0 }}},
    { SNMP_STRING, (RWRITE| SCALAR), (void*)var_cpeFtpFileName,
                 {2, { I_cpeFtpFileName, 0 }}},
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpeFtpAdminStatus,
                 {2, { I_cpeFtpAdminStatus, 0 }}},
    { SNMP_INTEGER, (RONLY| SCALAR), (void*)var_cpeFtpOperStatus,
                 {2, { I_cpeFtpOperStatus, 0 }}},
    { SNMP_INTEGER, (RONLY| SCALAR), (void*)var_cpeFtpTotalSize,
                 {2, { I_cpeFtpTotalSize, 0 }}},
    { SNMP_INTEGER, (RONLY| SCALAR), (void*)var_cpeFtpDoneSize,
                 {2, { I_cpeFtpDoneSize, 0 }}},
    { SNMP_INTEGER, (RONLY| SCALAR), (void*)var_cpeFtpElapseTime,
                 {2, { I_cpeFtpElapseTime, 0 }}},
    { (int)NULL }
    };
static SubTree cpeFtp_tree =  { NULL, cpeFtp_variables,
                (sizeof(cpeFtp_oid)/sizeof(oid)), cpeFtp_oid};

#ifdef LGD_TO_DO
static oid cpePing_oid[] = { O_cpePing };
static Object cpePing_variables[] = {
    { SNMP_STRING, (RWRITE| SCALAR), (void*)var_cpePingAddr,
                 {2, { I_cpePingAddr, 0 }}},
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpePingPkgSize,
                 {2, { I_cpePingPkgSize, 0 }}},
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpePingCount,
                 {2, { I_cpePingCount, 0 }}},
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_cpePingAdminStatus,
                 {2, { I_cpePingAdminStatus, 0 }}},
    { SNMP_INTEGER, (RONLY| SCALAR), (void*)var_cpePingOperStatus,
                 {2, { I_cpePingOperStatus, 0 }}},
    { SNMP_STRING, (RONLY| SCALAR), (void*)var_cpePingResult,
                 {2, { I_cpePingResult, 0 }}},
    { (int)NULL }
    };
static SubTree cpePing_tree =  { NULL, cpePing_variables,
                (sizeof(cpePing_oid)/sizeof(oid)), cpePing_oid};
#endif
/* This is the MIB registration function. This should be called */
/* within the init_cpe_dsl_mib-function */
void register_subtrees_of_cpe_dsl_mib()
{
#ifdef LGD_TO_DO
    insert_group_in_mib(&cpeSysInfo_tree);
    insert_group_in_mib(&cpeSystem_tree);
    insert_group_in_mib(&cpeConfigAdslLine_tree);
    insert_group_in_mib(&cpePvcEntry_tree);
    insert_group_in_mib(&cpePppEntry_tree);
    insert_group_in_mib(&cpeIpEntry_tree);
    insert_group_in_mib(&cpeLanDhcp_tree);
    insert_group_in_mib(&cpeWanDns_tree);
#ifdef CT_SNMP_MIB_VERSION_2
    insert_group_in_mib(&cpeQos_tree);
    insert_group_in_mib(&cpeEthernetPortEntry_tree);
    insert_group_in_mib(&cpeServiceTypeEntry_tree);
    insert_group_in_mib(&cpeAdmin_tree);
#endif
    insert_group_in_mib(&cpeSysLog_tree);
    insert_group_in_mib(&cpePing_tree);
#endif
    insert_group_in_mib(&cpeFtp_tree);
}

#ifdef LGD_TO_DO
/* min is 1, max is passed in*/
int cpeGenerateRandomNum(unsigned int max)
{
  struct timeval  now;
  int seed, num;
  
  gettimeofday(&now, (struct timezone *)0);
  seed = now.tv_sec + now.tv_usec;

  /* init seed */
  srand(seed);
  num = (rand() % max) + 1;
  return (num);
}

void cpeSendBootTrap(int junk)
{
    oid oid_enterprise[] = {O_snmpTrapEnterprise }; 
    oid val_enterprise[] = {SYS_OBJ, 6 }; 
    oid oid_sysConfigId[] = { O_cpeSysInfoConfigId };
    oid oid_sysFirmwareVersion[] = { O_cpeSysInfoFirmwareVersion };
    oid oid_sysProductId[] = { O_cpeSysInfoProductId };
    oid oid_sysVendorId[] = { O_cpeSysInfoVendorId };
    oid oid_sysManufacturerOui[] = { O_cpeSysInfoManufacturerOui };
    oid oid_sysInfoSystemVersion[] = { O_cpeSysInfoSystemVersion };
    oid oid_sysInfoWorkMode[] = { O_cpeSysInfoWorkMode};
    oid oid_sysInfoMibVersion[] = { O_cpeSysInfoMibVersion};
    oid oid_sysInfoSerialNumber[] = { O_cpeSysInfoSerialNumber};
#ifdef CT_SNMP_MIB_VERSION_2
    oid oid_sysInfoType[]={ O_cpeSysInfoType };
#endif
    Oid oid_obj;
    unsigned char *out_data;
    int out_length = SNMP_MAX_MSG_LENGTH;
    raw_snmp_info_t message;
    int ret;
#ifdef CT_SNMP_MIB_VERSION_2
    int return_int=0;
#endif
    //    time_t now;

    if (pSysInfo->trap != CPE_ADMIN_ENABLE)
      return;

    /* trap response has been received, then send anymore */
    if (pSysInfo->trapResponseState != CPE_BOOTTRAP_NO_RESPONSE) 
      return;

#if 0
    if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
      time(&now);
      printf("cpeSendBootTrap(entry): trap retry %d at %s\n",trapRetry,ctime(&now));
    }
#endif

    memset((unsigned char *)&message, 0x00, sizeof(message));
    message.mesg.version = SNMP_VERSION_1;
    message.mesg.pdutype = SNMP_TRP_REQ_PDU;

    cpeGetSnmpRoCommunity(message.mesg.community);
    message.mesg.community_length = strlen(message.mesg.community);
    message.mesg.community_id = 0;
    if (message.mesg.version == SNMP_VERSION_1) {
      message.trap_enterprise_oid.namelen =
        sizeof(val_enterprise) / sizeof(oid);
      memcpy(message.trap_enterprise_oid.name, val_enterprise,
             sizeof(oid_enterprise));

      message.trap_specific_type = I_cpeBootTrap;
      message.trap_type = SNMP_TRAP_ENTERPRISESPECIFIC;
      message.trap_timestamp = (long)current_sysUpTime();
      out_data = (unsigned char*)build_snmp_trapv1_without_list_of_varbind(&message);
    }
    else
    {
        message.mesg.request_id = (long)current_sysUpTime();
        out_data = (unsigned char*)build_snmp_response_without_list_of_varbind(&message);
    }
    
    /* varbind values follows: configId, firwareVersion, productId,
       vendorId, manufacturerOui, systemVersion, workMode, mibVersion, serialNumber */
    oid_obj.namelen = sizeof(oid_sysConfigId) / sizeof(oid);
    memcpy( oid_obj.name, oid_sysConfigId, sizeof(oid_sysConfigId) );
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_STRING,
                                   sizeof(pSysInfo->configId), (unsigned char *)pSysInfo->configId,
                                   &out_length);

    RETURN_VOID_ON_ERROR(out_data, "build trap sysConfigId");

    oid_obj.namelen = sizeof(oid_sysFirmwareVersion) / sizeof(oid);
    memcpy( oid_obj.name, oid_sysFirmwareVersion, sizeof(oid_sysFirmwareVersion) );
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_STRING,
                                   strlen(pSysInfo->version), (unsigned char *)pSysInfo->version,
                                   &out_length);
    RETURN_VOID_ON_ERROR(out_data, "build trap sysFirmwareVersion");

    oid_obj.namelen = sizeof(oid_sysProductId) / sizeof(oid);
    memcpy( oid_obj.name, oid_sysProductId, sizeof(oid_sysProductId) );
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_STRING,
                                   strlen(pSysInfo->prodId), (unsigned char *)pSysInfo->prodId,
                                   &out_length);
    RETURN_VOID_ON_ERROR(out_data, "build trap sysProdId");

    oid_obj.namelen = sizeof(oid_sysVendorId) / sizeof(oid);
    memcpy( oid_obj.name, oid_sysVendorId, sizeof(oid_sysVendorId) );
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_STRING,
                                   strlen(pSysInfo->vendorId), (unsigned char *)pSysInfo->vendorId,
                                   &out_length);
    RETURN_VOID_ON_ERROR(out_data, "build trap sysVendorId");

    oid_obj.namelen = sizeof(oid_sysManufacturerOui) / sizeof(oid);
    memcpy( oid_obj.name, oid_sysManufacturerOui, sizeof(oid_sysManufacturerOui) );
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_STRING,
                                   sizeof(pSysInfo->oui), (unsigned char *)pSysInfo->oui,
                                   &out_length);
    RETURN_VOID_ON_ERROR(out_data, "build trap sysManufacturerOui");

    oid_obj.namelen = sizeof(oid_sysInfoSystemVersion) / sizeof(oid);
    memcpy( oid_obj.name, oid_sysInfoSystemVersion, sizeof(oid_sysInfoSystemVersion) );
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_STRING,
                                   strlen(pSysInfo->sysVersion), 
                                   (unsigned char *)pSysInfo->sysVersion, &out_length);
    RETURN_VOID_ON_ERROR(out_data, "build trap sysSystemVersion");

    oid_obj.namelen = sizeof(oid_sysInfoWorkMode) / sizeof(oid);
    memcpy( oid_obj.name, oid_sysInfoWorkMode, sizeof(oid_sysInfoWorkMode) );
    long_return = CPE_ROUTER_MODE;
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_INTEGER,
                                   sizeof(int), (unsigned char *)&long_return,
                                   &out_length);
    RETURN_VOID_ON_ERROR(out_data, "build trap sysInfoWorkMode");

    oid_obj.namelen = sizeof(oid_sysInfoMibVersion) / sizeof(oid);
    memcpy( oid_obj.name, oid_sysInfoMibVersion, sizeof(oid_sysInfoMibVersion) );
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_STRING,
                                   strlen(pSysInfo->mibVersion), 
                                   (unsigned char *)pSysInfo->mibVersion,&out_length);
    RETURN_VOID_ON_ERROR(out_data, "build trap sysInfoMibVersion");

    oid_obj.namelen = sizeof(oid_sysInfoSerialNumber) / sizeof(oid);
    memcpy( oid_obj.name, oid_sysInfoSerialNumber, sizeof(oid_sysInfoSerialNumber) );
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_STRING,
                                   strlen(pSysInfo->serialNum), 
                                   (unsigned char *)pSysInfo->serialNum,&out_length);
    RETURN_VOID_ON_ERROR(out_data, "build trap sysInfoSerialNumber");


#ifdef CT_SNMP_MIB_VERSION_2
    oid_obj.namelen = sizeof(oid_sysInfoType) / sizeof(oid);
    memcpy( oid_obj.name, oid_sysInfoType, sizeof(oid_sysInfoType) );
    return_int=pSysInfo->sysInfoType;
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_INTEGER,
                      sizeof(int), (unsigned char *)&return_int,
                      &out_length);
    RETURN_VOID_ON_ERROR(out_data, "build trap sysInfoType");
#endif

    message.response_packet_end = out_data;
    out_length = correct_snmp_response_with_lengths( &message, 0, 0 );
    out_data = asn_build_sequence(message.response_pdu, &out_length,
                                  message.mesg.pdutype,
                                  message.response_packet_end - message.response_request_id);
    RETURN_VOID_ON_ERROR(out_data, "build trap pdu type");

    if ((trapRetry < CPE_TRAP_RETRY_COUNT) &&
        (pSysInfo->trapResponseState == CPE_BOOTTRAP_NO_RESPONSE)) {
      trapRetry++;
      alarm(0);

#if 0
      if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_CPE) == SNMP_DEBUG_LEVEL_MIB_CPE) {
        time(&now);      
        printf("setting alarm for next retry, retry %d, time %s, retry_time %d\n",trapRetry,ctime(&now),
               CPE_TRAP_RETRY_INTERVAL);
  }
#endif    

      signal(SIGALRM,cpeSendBootTrap);
      alarm(CPE_TRAP_RETRY_INTERVAL);
    }

    /* this trap expects a response, and a retry is to be sent every 30 seconds
       for up to 3 times */
    ret = snmp_send_trap(&message);
}
#endif
void cpeSendFtpDoneTrap(int junk)
{
    oid oid_enterprise[] = {O_snmpTrapEnterprise }; 
    oid val_enterprise[] = {SYS_OBJ, 6 }; 
    oid oid_ftpAdminStatus[] = { O_cpeFtpAdminStatus, 0 };
    oid oid_ftpOperStatus[] = { O_cpeFtpOperStatus, 0 };
    oid oid_ftpTotalSize[] = { O_cpeFtpTotalSize, 0 };
    oid oid_ftpDoneSize[] = { O_cpeFtpDoneSize, 0 };
    oid oid_ftpElapseTime[] = { O_cpeFtpElapseTime, 0 };
    Oid oid_obj;
    unsigned char *out_data;
    int out_length = SNMP_MAX_MSG_LENGTH;
    raw_snmp_info_t message;

    memset( (unsigned char *) &message, 0x00, sizeof(message) );

    message.mesg.version = SNMP_VERSION_1;
    message.mesg.pdutype = SNMP_TRP_REQ_PDU;
    cpeGetSnmpRoCommunity(message.mesg.community);
    message.mesg.community_length = strlen(message.mesg.community);
    message.mesg.community_id = 0;

    if (message.mesg.version == SNMP_VERSION_1) {
      message.trap_enterprise_oid.namelen =
        sizeof(val_enterprise) / sizeof(oid);
      memcpy(message.trap_enterprise_oid.name, val_enterprise,
             sizeof(oid_enterprise));

      message.trap_specific_type = I_cpeFtpDone;
      message.trap_type = SNMP_TRAP_ENTERPRISESPECIFIC;
      message.trap_timestamp = (long)current_sysUpTime();
      out_data = (unsigned char*)build_snmp_trapv1_without_list_of_varbind(&message);
    }
    else
    {
        message.mesg.request_id = (long)current_sysUpTime();
        out_data = (unsigned char*)build_snmp_response_without_list_of_varbind(&message);
    }

    /* varbind values follows: ftpAdminStatus, ftpOperStatus, ftpTotalSize,
       ftpDoneSize, ftpElapseTime */
    oid_obj.namelen = sizeof(oid_ftpAdminStatus) / sizeof(oid);
    memcpy( oid_obj.name, oid_ftpAdminStatus, sizeof(oid_ftpAdminStatus) );
    long_return = pFtpData->adminStatus;
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_INTEGER,
                                   sizeof(int), (unsigned char *)&long_return,
                                   &out_length);

    oid_obj.namelen = sizeof(oid_ftpOperStatus) / sizeof(oid);
    memcpy( oid_obj.name, oid_ftpOperStatus, sizeof(oid_ftpOperStatus) );
    long_return = pFtpData->operStatus;
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_INTEGER,
                                   sizeof(int), (unsigned char *)&long_return,
                                   &out_length);
    

    oid_obj.namelen = sizeof(oid_ftpTotalSize) / sizeof(oid);
    memcpy( oid_obj.name, oid_ftpTotalSize, sizeof(oid_ftpTotalSize) );
    long_return = pFtpData->totalSize;
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_INTEGER,
                                   sizeof(int), (unsigned char *)&long_return,
                                   &out_length);
    
    oid_obj.namelen = sizeof(oid_ftpDoneSize) / sizeof(oid);
    memcpy( oid_obj.name, oid_ftpDoneSize, sizeof(oid_ftpDoneSize) );
    long_return = pFtpData->doneSize;
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_INTEGER,
                                   sizeof(int), (unsigned char *)&long_return,
                                   &out_length);

    oid_obj.namelen = sizeof(oid_ftpElapseTime) / sizeof(oid);
    memcpy( oid_obj.name, oid_ftpElapseTime, sizeof(oid_ftpElapseTime) );
    long_return = pFtpData->elapseTime;
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_INTEGER,
                                   sizeof(int), (unsigned char *)&long_return,
                                   &out_length);


    message.response_packet_end = out_data;
    out_length = correct_snmp_response_with_lengths( &message, 0, 0 );
    out_data = asn_build_sequence(message.response_pdu, &out_length,
                                  message.mesg.pdutype,
                                  message.response_packet_end - message.response_request_id);

    snmp_send_trap(&message);
}

#ifdef LGD_TO_DO
void cpeSendPingDoneTrap(int junk)
{
    oid oid_enterprise[] = {O_snmpTrapEnterprise }; 
    oid val_enterprise[] = {SYS_OBJ, 6 }; 
    oid oid_pingOperStatus[] = { O_cpePingOperStatus };
    oid oid_pingResult[] = { O_cpePingResult };
    Oid oid_obj;
    unsigned char *out_data;
    int out_length = SNMP_MAX_MSG_LENGTH;
    raw_snmp_info_t message;

    memset( (unsigned char *) &message, 0x00, sizeof(message) );

    message.mesg.version = SNMP_VERSION_1;
    message.mesg.pdutype = SNMP_TRP_REQ_PDU;
    cpeGetSnmpRoCommunity(message.mesg.community);
    message.mesg.community_length = strlen(message.mesg.community);
    message.mesg.community_id = 0;

    if (message.mesg.version == SNMP_VERSION_1) {
      message.trap_enterprise_oid.namelen =
        sizeof(val_enterprise) / sizeof(oid);
      memcpy(message.trap_enterprise_oid.name, val_enterprise,
             sizeof(oid_enterprise));

      message.trap_specific_type = I_cpePingDone;
      message.trap_type = SNMP_TRAP_ENTERPRISESPECIFIC;
      message.trap_timestamp = (long)current_sysUpTime();
      out_data = (unsigned char*)build_snmp_trapv1_without_list_of_varbind(&message);
    }
    else
    {
        message.mesg.request_id = (long)current_sysUpTime();
        out_data = (unsigned char*)build_snmp_response_without_list_of_varbind(&message);
    }

    /* varbind values follows: pingOperStatus and pingResult */
    oid_obj.namelen = sizeof(oid_pingOperStatus) / sizeof(oid);
    memcpy( oid_obj.name, oid_pingOperStatus, sizeof(oid_pingOperStatus) );
    long_return = pPingData->operStatus;
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_INTEGER,
                                   sizeof(int), (unsigned char *)&long_return,
                                   &out_length);
    
    oid_obj.namelen = sizeof(oid_pingResult) / sizeof(oid);
    memcpy( oid_obj.name, oid_pingResult, sizeof(oid_pingResult) );
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_STRING,
                                   strlen(pPingData->result), 
                                   (unsigned char *)pPingData->result,&out_length);

    message.response_packet_end = out_data;
    out_length = correct_snmp_response_with_lengths( &message, 0, 0 );
    out_data = asn_build_sequence(message.response_pdu, &out_length,
                                  message.mesg.pdutype,
                                  message.response_packet_end - message.response_request_id);

    snmp_send_trap(&message);
}

void cpeSendPppTestDoneTrap(int result)
{
    oid oid_enterprise[] = {O_snmpTrapEnterprise }; 
    oid val_enterprise[] = {SYS_OBJ, 6 }; 
    oid oid_pppTestResult[] = { O_cpePppTestResult };
    Oid oid_obj;
    unsigned char *out_data;
    int out_length = SNMP_MAX_MSG_LENGTH;
    raw_snmp_info_t message;

    memset( (unsigned char *) &message, 0x00, sizeof(message) );
    message.mesg.version = SNMP_VERSION_1;
    message.mesg.pdutype = SNMP_TRP_REQ_PDU;
    cpeGetSnmpRoCommunity(message.mesg.community);
    message.mesg.community_length = strlen(message.mesg.community);
    message.mesg.community_id = 0;

    if (message.mesg.version == SNMP_VERSION_1) {
      message.trap_enterprise_oid.namelen =
        sizeof(val_enterprise) / sizeof(oid);
      memcpy(message.trap_enterprise_oid.name, val_enterprise,
             sizeof(oid_enterprise));

      message.trap_specific_type = I_cpePppTestDone;
      message.trap_type = SNMP_TRAP_ENTERPRISESPECIFIC;
      message.trap_timestamp = (long)current_sysUpTime();
      out_data = (unsigned char*)build_snmp_trapv1_without_list_of_varbind(&message);
    }
    else
    {
        message.mesg.request_id = (long)current_sysUpTime();
        out_data = (unsigned char*)build_snmp_response_without_list_of_varbind(&message);
    }

    /* varbind values follows: PPP test result */
    oid_obj.namelen = sizeof(oid_pppTestResult) / sizeof(oid);
    memcpy( oid_obj.name, oid_pppTestResult, sizeof(oid_pppTestResult) );
    long_return = result;
    out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_INTEGER,
                                   sizeof(int), (unsigned char *)&long_return,
                                   &out_length);

    message.response_packet_end = out_data;
    out_length = correct_snmp_response_with_lengths( &message, 0, 0 );
    out_data = asn_build_sequence(message.response_pdu, &out_length,
                                  message.mesg.pdutype,
                                  message.response_packet_end - message.response_request_id);

    snmp_send_trap(&message);
}

#endif
