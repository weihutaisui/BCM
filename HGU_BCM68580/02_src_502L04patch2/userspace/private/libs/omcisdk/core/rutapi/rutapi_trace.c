/***********************************************************************
 *
 *  Copyright (c) 2017 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2017:proprietary:omcid

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

/*****************************************************************************
*    Description:
*
*      OMCI Wrapper RUT API trace functions.
*
*****************************************************************************/


/* ---- Include Files ----------------------------------------------------- */

#include "owsvc_api.h"
#include "owrut_defs.h"
#include "owrut_api.h"
#include "ctlutils_api.h"
#include "me_handlers.h"


/* ---- Private Constants and Types --------------------------------------- */

#define OMCI_TRACE_PREFIX_NAME    "omci"
#define OMCI_TRACE_IF_NAME_SIZE   16
#define OMCI_TRACE_FILE_NAME      "/var/omcirut.log"


/* ---- Macro API definitions --------------------------------------------- */

//#define CC_OMCI_TRACE_DEFAULT_ON
#define CC_OMCI_TRACE_DEFAULT_PRINT_TO_FILE

#define __error(fmt, arg...) \
  fprintf(stderr, "ERROR[%s, %d]: " fmt, __FUNCTION__, __LINE__, ##arg)

#define __print(fmt, arg...) \
 do { \
         if (omciApiTraceEnable) \
         { \
             if (omciApiTracePrintToFile) \
             { \
                 FILE *fd = fopen(OMCI_TRACE_FILE_NAME, "a+"); \
                 if (fd == NULL) \
                 { \
                     __error("Could not open %s\n", OMCI_TRACE_FILE_NAME); \
                 } \
                 else \
                 { \
                     fprintf(fd, fmt, ##arg); \
                     fclose(fd); \
                 } \
             } \
             else \
             { \
                 fprintf(stdout, fmt, ##arg); \
             } \
        } \
        calLog_library(fmt, ##arg); \
    } while (0)

#define __trace(cmd, fmt, arg...) \
 do { \
      if (omciApiTraceEnable) \
      { \
          __print("["OMCI_TRACE_PREFIX_NAME " " #cmd "] " fmt, ##arg); \
      } \
    } while(0)


#if !defined(BRCM_CMS_BUILD)
#define RUTAPI_TRACE_ONLY 1
#endif

#define dummyFunc(argu, arg1...)

/*
 * RUTAPI_TRACE_ONLY: trace only.
 * RUTAPI_TRACE: trace and execution.
 * None: execution only.
 */
#if defined(RUTAPI_TRACE_ONLY)
#define _owtrace(func) func##Trace
#define _owapi(func) dummyFunc
#elif defined(RUTAPI_TRACE)
#define _owtrace(func) func##Trace
#define _owapi(func) owapi_##func
#else
#define _owtrace(func) dummyFunc
#define _owapi(func) owapi_##func
#endif


/* ---- Private Function Prototypes --------------------------------------- */


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */

#if defined(CC_OMCI_TRACE_DEFAULT_ON)
static int omciApiTraceEnable = 1;
#else
static int omciApiTraceEnable = 0;
#endif

#if defined(CC_OMCI_TRACE_DEFAULT_PRINT_TO_FILE)
static int omciApiTracePrintToFile = 1;
#else
static int omciApiTracePrintToFile = 0;
#endif


/* ---- Functions --------------------------------------------------------- */

/* Trace functions. */

CmsRet owapi_rut_initAppCfgDataTrace(OmciSystemObject *omciCfgData)
{
    __trace(owapi, "%s(%p)\n",
      "rut_initAppCfgData",
      omciCfgData);
    return CMSRET_SUCCESS;
}

void owapi_rut_initLoggingFromConfigTrace(UBOOL8 useConfiguredLogLevel)
{
    __trace(owapi, "%s(%d)\n",
      "rut_initLoggingFromConfig",
      useConfiguredLogLevel);
}

CmsRet owapi_rut_announceMibresetTrace(void)
{
    __trace(owapi, "%s()\n",
      "rut_announceMibreset");
    return CMSRET_SUCCESS;
}

void owapi_rut_doSystemActionTrace(const char *task, char *cmd)
{
    __trace(owapi, "%s(%s, %s)\n",
      "rut_doSystemAction",
      task, cmd);
}

void owapi_rut_systemRebootTrace(void)
{
    __trace(owapi, "%s()\n",
      "rut_systemReboot");
}

BOOL owapi_rut_tmctl_isUserTMCtlOwnerTrace(eTmctlUser user)
{
    __trace(owapi, "%s(%d)\n",
      "rut_tmctl_isUserTMCtlOwner",
      user);
    return FALSE;
}

UINT32 owapi_rut_tmctl_getQueueOwnerTrace(void)
{
    __trace(owapi, "%s()\n",
      "rut_tmctl_getQueueOwner");
    return TMCTL_OWNER_DFLT;
}

UINT32 owapi_rut_tmctl_getQueueMapTrace(void)
{
    __trace(owapi, "%s()\n",
      "rut_tmctl_getQueueMap");
    return QID_PRIO_MAP_Q0P7;
}

CmsRet owapi_rut_tmctl_getQueueSchemeTrace(UINT32 *owner, UINT32 *scheme)
{
    __trace(owapi, "%s(%p, %p)\n",
      "rut_tmctl_getQueueScheme",
      owner, scheme);
    return CMSRET_SUCCESS;
}

tmctl_ret_e owapi_rut_tmctl_portTmInitTrace(tmctl_devType_e devType,
  tmctl_if_t *if_p, uint32_t cfgFlags)
{
    __trace(owapi, "%s(%d, %p, %d)\n",
      "rut_tmctl_portTmInit",
      devType, if_p, cfgFlags);
    return TMCTL_SUCCESS;
}

tmctl_ret_e owapi_rut_tmctl_portTmUninitTrace(tmctl_devType_e devType,
  tmctl_if_t *if_p)
{
    __trace(owapi, "%s(%d, %p)\n",
      "rut_tmctl_portTmUninit",
      devType, if_p);
    return TMCTL_SUCCESS;
}

tmctl_ret_e owapi_rut_tmctl_getQueueCfgTrace(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueCfg_t *qcfg_p)
{
    __trace(owapi, "%s(%d, %p, %d, %p)\n",
      "rut_tmctl_getQueueCfg",
      devType, if_p, queueId, qcfg_p);
    return TMCTL_SUCCESS;
}

tmctl_ret_e owapi_rut_tmctl_delQueueCfgTrace(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId)
{
    __trace(owapi, "%s(%d, %p, %d)\n",
      "rut_tmctl_delQueueCfg",
      devType, if_p, queueId);
    return TMCTL_SUCCESS;
}

tmctl_ret_e owapi_rut_tmctl_getPortShaperTrace(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_shaper_t *shaper_p)
{
    __trace(owapi, "%s(%d, %p, %p)\n",
      "rut_tmctl_getPortShaper",
      devType, if_p, shaper_p);
    return TMCTL_SUCCESS;
}

tmctl_ret_e owapi_rut_tmctl_setPortShaperTrace(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_shaper_t *shaper_p)
{
    __trace(owapi, "%s(%d, %p, %p)\n",
      "rut_tmctl_setPortShaper",
      devType, if_p, shaper_p);
    return TMCTL_SUCCESS;
}

tmctl_ret_e owapi_rut_tmctl_getQueueDropAlgTrace(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p)
{
    __trace(owapi, "%s(%d, %p, %d, %p)\n",
      "rut_tmctl_getQueueDropAlg",
      devType, if_p, queueId, dropAlg_p);
    return TMCTL_SUCCESS;
}

tmctl_ret_e owapi_rut_tmctl_setQueueDropAlgTrace(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p)
{
    __trace(owapi, "%s(%d, %p, %d, %p)\n",
      "rut_tmctl_setQueueDropAlg",
      devType, if_p, queueId, dropAlg_p);
    return TMCTL_SUCCESS;
}

tmctl_ret_e owapi_rut_tmctl_getPortTmParmsTrace(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_portTmParms_t *tmParms_p)
{
    __trace(owapi, "%s(%d, %p, %p)\n",
      "rut_tmctl_getPortTmParms",
      devType, if_p, tmParms_p);
    return TMCTL_SUCCESS;
}

tmctl_ret_e owapi_rut_tmctl_setQueueCfgTrace(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_queueCfg_t *qcfg_p)
{
    __trace(owapi, "%s(%d, %p, %p)\n",
      "rut_tmctl_setQueueCfg",
      devType, if_p, qcfg_p);
    return TMCTL_SUCCESS;
}

tmctl_ret_e owapi_rut_tmctl_getQueueStatsTrace(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueStats_t *stats_p)
{
    __trace(owapi, "%s(%d, %p, %d, %p)\n",
      "rut_tmctl_getQueueStats",
      devType,if_p, queueId, stats_p);
    return TMCTL_SUCCESS;
}

tmctl_ret_e owapi_rut_tmctl_setQueueDropAlgExtTrace(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p)
{
    __trace(owapi, "%s(%d, %p, %d, %p)\n",
     "tmctl_setQueueDropAlgExt",
     devType, if_p, queueId, dropAlg_p);
    return TMCTL_SUCCESS;
}

CmsRet owapi_rut_processHostCtrlChangeTrace(void)
{
    __trace(owapi, "%s()\n",
      "rut_processHostCtrlChange");
    return CMSRET_SUCCESS;
}

UBOOL8 owapi_mcast_getHostCtrlConfigTrace(void)
{
    __trace(owapi, "%s()\n",
      "mcast_getHostCtrlConfig");
    return FALSE;
}

CmsRet owapi_rut_reloadMcastCtrlTrace(void)
{
    __trace(owapi, "%s()\n",
      "rut_reloadMcastCtrl");
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_getOmciMcastCfgInfoTrace(omciMcastCfgInfo *mcastCfgInfo)
{
    __trace(owapi, "%s(%p)\n",
      "rut_getOmciMcastCfgInfo",
      mcastCfgInfo);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_setOmciMcastCfgInfoTrace(omciMcastCfgInfo *mcastCfgInfo)
{
    __trace(owapi, "%s(%p)\n",
      "rut_setOmciMcastCfgInfo",
      mcastCfgInfo);
    return CMSRET_SUCCESS;
}

void owapi_rut_cxtInitTrace(void *msgHandle, const UINT32 eid, SINT32 shmId)
{
    __trace(owapi, "%s(%p, %d, %d)\n",
      "rut_cxtInit",
      msgHandle, eid, shmId);
}


void owapi_rut_updateIgmpMldSnoopingIntfListTrace(void)
{
    __trace(owapi, "%s()\n",
      "rut_updateIgmpMldSnoopingIntfList");
}

UINT32 owapi_rut_startPingTrace(char *cmdLine)
{
    __trace(owapi, "%s(%s)\n",
      "rut_startPing",
      cmdLine);
    return 0;
}

UINT32 owapi_rut_startTracerouteTrace(char *cmdLine)
{
    __trace(owapi, "%s(%s)\n",
      "rut_startTraceroute",
      cmdLine);
    return 0;
}

CmsRet owapi_rut_sendServiceInfoMsgTrace(const OmciServiceMsgBody *pService)
{
    __trace(owapi, "%s(%p)\n",
      "rut_sendServiceInfoMsg",
      pService);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_configDnsTrace(const char *dns1, const char *dns2)
{
    __trace(owapi, "%s(%s, %s)\n",
      "rut_configDns",
      dns1, dns2);
    return CMSRET_SUCCESS;
}

BCM_OMCI_PM_STATUS owapi_rut_getDnsStatsTrace(void *dnsStats)
{
    __trace(owapi, "%s(%p)\n",
      "rut_getDnsStats",
      dnsStats);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_updateMcastCtrlIfNamesTrace(char *mcastIfNames)
{
    __trace(owapi, "%s(%s)\n",
      "rut_updateMcastCtrlIfNames",
      mcastIfNames);
    return CMSRET_SUCCESS;
}

void owapi_rut_configTcpUdpTrace(const TcpUdpConfigDataObject *tcpUdp, UBOOL8 add)
{
    __trace(owapi, "%s(%p, %d)\n",
      "rut_configTcpUdp",
      tcpUdp, add);
}

CmsRet owapi_rut_configIpv4HostTrace(UINT32 meId, char *ifName)
{
    __trace(owapi, "%s(%d, %s)\n",
      "rut_configIpv4Host",
      meId, ifName);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_configIpv6HostTrace(UINT32 meId, char *ifName)
{
    __trace(owapi, "%s(%d, %s)\n",
      "rut_configIpv6Host",
      meId, ifName);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_updateAcsCfgTrace(UINT32 ipHostMeId, char *acs, UINT8 *username,
  UINT8 *password)
{
    __trace(owapi, "%s(%d, %s, %s, %s)\n",
      "rut_updateAcsCfg",
      ipHostMeId, acs, username, password);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_getIpv4HostInfoTrace(UINT32 meId, omciIpv4HostInfo *ipHostInfo)
{
    __trace(owapi, "%s(%d, %p)\n",
      "rut_getIpv4HostInfo",
      meId, ipHostInfo);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_getBrgFwdMaskTrace(UINT32 *brgFwdMask)
{
    __trace(owapi, "%s(%p)\n",
      "rut_getBrgFwdMask",
      brgFwdMask);
    return CMSRET_SUCCESS;
}

void owapi_rut_disablePptpPotsUniTrace(UINT32 lineIdx)
{
    __trace(owapi, "%s(%d)\n",
      "rut_disablePptpPotsUni",
      lineIdx);
}

void owapi_rut_enablePptpPotsUniTrace(UINT32 lineIdx)
{
    __trace(owapi, "%s(%d)\n",
      "rut_disablePptpPotsUni",
      lineIdx);
}

void owapi_rut_disableAllPptpPotsUniTrace(void)
{
    __trace(owapi, "%s()\n",
      "rut_disableAllPptpPotsUni");
}

void owapi_rut_enableAllPptpPotsUniTrace(void)
{
    __trace(owapi, "%s()\n",
      "rut_enableAllPptpPotsUni");
}

CmsRet owapi_rut_setVoipBoundIfNameAddressTrace(UINT32 ipHostMeId,
  char *ipAddr)
{
    __trace(owapi, "%s(%d, %s)\n",
      "rut_setVoipBoundIfNameAddress",
      ipHostMeId, ipAddr);
    return CMSRET_SUCCESS;
}

void owapi_rut_MapVoIpMediaProfileTrace(int lineIdx,
  VoIpMediaProfileObject *omciVoIpMediaProfileObjectPtr)
{
    __trace(owapi, "%s(%d, %p)\n",
      "rut_MapVoIpMediaProfile",
      lineIdx, omciVoIpMediaProfileObjectPtr);
}

void owapi_rut_MapVoiceServiceProfileTrace(int lineIdx,
  VoiceServiceObject *omciVoiceServiceObjectPtr)
{
    __trace(owapi, "%s(%d, %p)\n",
      "rut_MapVoiceServiceProfile",
      lineIdx, omciVoiceServiceObjectPtr);
}

void owapi_rut_MapRtpProfileDataTrace(RtpProfileDataObject *omciRtpProfilePtr)
{
    __trace(owapi, "%s(%p)\n",
      "rut_MapRtpProfileData",
      omciRtpProfilePtr);
}

void owapi_rut_MapPptpPotsUniTrace(int lineIdx,
  PptpPotsUniObject *omciPptpPotsUniObjectPtr)
{
    __trace(owapi, "%s(%d, %p)\n",
      "rut_MapPptpPotsUni",
      lineIdx, omciPptpPotsUniObjectPtr);
}

void owapi_rut_MapVoIpApplServiceProfileTrace(int lineIdx,
  VoIpAppServiceProfileObject *omciVoIpAppServiceProfilePtr)
{
    __trace(owapi, "%s(%d, %p)\n",
      "rut_MapVoIpApplServiceProfile",
      lineIdx, omciVoIpAppServiceProfilePtr);
}

void owapi_rut_MapVoIpConfigDataTrace(VoIpConfigDataObject *voipConfigDataPtr)
{
    __trace(owapi, "%s(%p)\n",
      "rut_MapVoIpConfigData",
      voipConfigDataPtr);
}

UBOOL8 owapi_rut_IsVoiceOmciManagedTrace(void)
{
    __trace(owapi, "%s()\n",
      "rut_IsVoiceOmciManaged");
    return FALSE;
}

BCM_OMCI_PM_STATUS owapi_rut_GetRtpStatsTrace(int lineIdx,
  BCM_OMCI_PM_RTP_COUNTER *rtpStatsRec)
{
    __trace(owapi, "%s(%d, %p)\n",
      "rut_GetRtpStats",
      lineIdx, rtpStatsRec);
    return OMCI_PM_STATUS_SUCCESS;
}

void owapi_rut_MapNetworkDialPlanTrace(UINT8 digitMap[], UINT32 critDigitTimer,
  UINT32 partDigitTimer)
{
    __trace(owapi, "%s(%p, %d, %d)\n",
      "rut_MapNetworkDialPlan",
      digitMap, critDigitTimer, partDigitTimer);
}

void owapi_rut_MapSipAgentConfigDataTrace(int lineIdx,
  SipAgentConfigDataObject *omciSipAgentConfigDataPtr)
{
    __trace(owapi, "%s(%d, %p)\n",
      "rut_MapSipAgentConfigData",
      lineIdx, omciSipAgentConfigDataPtr);
}

CmsRet owapi_rut_setSipReregHeadStartTimeTrace(const UINT32 lineIdx,
  const UINT32 reregVal)
{
    __trace(owapi, "%s(%d, %d)\n",
      "rut_setSipReregHeadStartTime",
      lineIdx, reregVal);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_setSipRegisterExpirationTimeTrace(const UINT32 lineIdx,
  const UINT32 expireVal)
{
    __trace(owapi, "%s(%d, %d)\n",
      "rut_setSipRegisterExpirationTime",
      lineIdx, expireVal);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_setSipOutboundProxyAddressTrace(const UINT32 lineIdx,
  const char *address)
{
    __trace(owapi, "%s(%d, %s)\n",
      "rut_setSipOutboundProxyAddress",
      lineIdx, address);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_setSipAuthUsernameTrace(const UINT32 lineIdx, const char *user)
{
    __trace(owapi, "%s(%d, %s)\n",
      "rut_setSipAuthUsername",
      lineIdx, user);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_setSipAuthPasswordTrace(const UINT32 lineIdx, const char *password)
{
    __trace(owapi, "%s(%d, %s)\n",
      "rut_setSipAuthPassword",
      lineIdx, password);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_setSipUserPartAorTrace(const UINT32 lineIdx,
  const char *userPartAor)
{
    __trace(owapi, "%s(%d, %s)\n",
      "rut_setSipUserPartAor",
      lineIdx, userPartAor);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_setSipProxyServerAddressTrace(const UINT32 lineIdx,
  const char *address)
{
    __trace(owapi, "%s(%d, %s)\n",
      "rut_setSipProxyServerAddress",
      lineIdx, address);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_setSipRegistrarAddressTrace(const UINT32 lineIdx,
  const char *address)
{
    __trace(owapi, "%s(%d, %s)\n",
      "rut_setSipRegistrarAddress",
      lineIdx, address);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_SendUploadCompleteTrace(void)
{
    __trace(owapi, "%s()\n",
      "rut_SendUploadComplete");
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_addAutoObjectTrace(UINT32 oid, UINT32 meId, UBOOL8 persistent)
{
    __trace(owapi, "%s(%d, %d, %d)\n",
      "rut_setAutoObject",
      oid, meId, persistent);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_setAutoObjectTrace(UINT32 oid, UINT32 meId, void *mdmObj)
{
    __trace(owapi, "%s(%d, %d, %p)\n",
      "rut_setAutoObject",
      oid, meId, mdmObj);
    return CMSRET_SUCCESS;
}

CmsRet owapi_rut_delAutoObjectTrace(UINT32 oid, UINT32 meId)
{
    __trace(owapi, "%s(%d, %d)\n",
      "rut_delAutoObject",
      oid, meId);
    return CMSRET_SUCCESS;
}

/* API wrapper functions. */

CmsRet _owapi_rut_initAppCfgData(OmciSystemObject *omciCfgData)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_initAppCfgData)(omciCfgData);
#else
    _owtrace(owapi_rut_initAppCfgData)(omciCfgData);
    return _owapi(rut_initAppCfgData)(omciCfgData);
#endif
}

void _owapi_rut_initLoggingFromConfig(UBOOL8 useConfiguredLogLevel)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_initLoggingFromConfig)(useConfiguredLogLevel);
#else
    _owtrace(owapi_rut_initLoggingFromConfig)(useConfiguredLogLevel);
    return _owapi(rut_initLoggingFromConfig)(useConfiguredLogLevel);
#endif
}

void _owapi_rut_doSystemAction(const char *task, char *cmd)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_doSystemAction)(task, cmd);
#else
    _owtrace(owapi_rut_doSystemAction)(task, cmd);
    return _owapi(rut_doSystemAction)(task, cmd);
#endif
}

CmsRet _owapi_rut_announceMibreset(void)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_announceMibreset)();
#else
    _owtrace(owapi_rut_announceMibreset)();
    return _owapi(rut_announceMibreset)();
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_systemReboot(void)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_systemReboot)();
#else
    _owtrace(owapi_rut_systemReboot)();
    return _owapi(rut_systemReboot)();
#endif /* RUTAPI_TRACE_ONLY */
}

BOOL _owapi_rut_tmctl_isUserTMCtlOwner(eTmctlUser user)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_isUserTMCtlOwner)(user);
#else
    _owtrace(owapi_rut_tmctl_isUserTMCtlOwner)(user);
    return _owapi(rut_tmctl_isUserTMCtlOwner)(user);
#endif
}

UINT32 _owapi_rut_tmctl_getQueueOwner(void)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_getQueueOwner)();
#else
    _owtrace(owapi_rut_tmctl_getQueueOwner)();
    return _owapi(rut_tmctl_getQueueOwner)();
#endif
}

UINT32 _owapi_rut_tmctl_getQueueMap(void)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_getQueueMap)();
#else
    _owtrace(owapi_rut_tmctl_getQueueMap)();
    return _owapi(rut_tmctl_getQueueMap)();
#endif
}

CmsRet _owapi_rut_tmctl_getQueueScheme(UINT32 *owner, UINT32 *scheme)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_getQueueScheme)(owner, scheme);
#else
    _owtrace(owapi_rut_tmctl_getQueueScheme)(owner, scheme);
    return _owapi(rut_tmctl_getQueueScheme)(owner, scheme);
#endif
}

tmctl_ret_e _owapi_rut_tmctl_portTmInit(tmctl_devType_e devType,
  tmctl_if_t *if_p, uint32_t cfgFlags)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_portTmInit)(devType, if_p, cfgFlags);
#else
    _owtrace(owapi_rut_tmctl_portTmInit)(devType, if_p, cfgFlags);
    return _owapi(rut_tmctl_portTmInit)(devType, if_p, cfgFlags);
#endif
}

tmctl_ret_e _owapi_rut_tmctl_portTmUninit(tmctl_devType_e devType,
  tmctl_if_t *if_p)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_portTmUninit)(devType, if_p);
#else
    _owtrace(owapi_rut_tmctl_portTmUninit)(devType, if_p);
    return _owapi(rut_tmctl_portTmUninit)(devType, if_p);
#endif
}

tmctl_ret_e _owapi_rut_tmctl_getQueueCfg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueCfg_t *qcfg_p)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_getQueueCfg)(devType, if_p, queueId, qcfg_p);
#else
    _owtrace(owapi_rut_tmctl_getQueueCfg)(devType, if_p, queueId, qcfg_p);
    return _owapi(rut_tmctl_getQueueCfg)(devType, if_p, queueId, qcfg_p);
#endif
}

tmctl_ret_e _owapi_rut_tmctl_delQueueCfg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_delQueueCfg)(devType, if_p, queueId);
#else
    _owtrace(owapi_rut_tmctl_delQueueCfg)(devType, if_p, queueId);
    return _owapi(rut_tmctl_delQueueCfg)(devType, if_p, queueId);
#endif
}

tmctl_ret_e _owapi_rut_tmctl_getPortShaper(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_shaper_t *shaper_p)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_getPortShaper)(devType, if_p, shaper_p);
#else
    _owtrace(owapi_rut_tmctl_getPortShaper)(devType, if_p, shaper_p);
    return _owapi(rut_tmctl_getPortShaper)(devType, if_p, shaper_p);
#endif /* RUTAPI_TRACE_ONLY */
}

tmctl_ret_e _owapi_rut_tmctl_setPortShaper(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_shaper_t *shaper_p)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_setPortShaper)(devType, if_p, shaper_p);
#else
    _owtrace(owapi_rut_tmctl_setPortShaper)(devType, if_p, shaper_p);
    return _owapi(rut_tmctl_setPortShaper)(devType, if_p, shaper_p);
#endif /* RUTAPI_TRACE_ONLY */
    return TMCTL_SUCCESS;
}

tmctl_ret_e _owapi_rut_tmctl_getQueueDropAlg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_getQueueDropAlg)(devType, if_p, queueId, dropAlg_p);
#else
    _owtrace(owapi_rut_tmctl_getQueueDropAlg)(devType, if_p, queueId, dropAlg_p);
    return _owapi(rut_tmctl_getQueueDropAlg)(devType, if_p, queueId, dropAlg_p);
#endif /* RUTAPI_TRACE_ONLY */
}

tmctl_ret_e _owapi_rut_tmctl_setQueueDropAlg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_setQueueDropAlg)(devType, if_p, queueId, dropAlg_p);
#else
    _owtrace(owapi_rut_tmctl_setQueueDropAlg)(devType, if_p, queueId, dropAlg_p);
    return _owapi(rut_tmctl_setQueueDropAlg)(devType, if_p, queueId, dropAlg_p);
#endif /* RUTAPI_TRACE_ONLY */
}

tmctl_ret_e _owapi_rut_tmctl_getPortTmParms(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_portTmParms_t *tmParms_p)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_getPortTmParms)(devType, if_p, tmParms_p);
#else
    _owtrace(owapi_rut_tmctl_getPortTmParms)(devType, if_p, tmParms_p);
    return _owapi(rut_tmctl_getPortTmParms)(devType, if_p, tmParms_p);
#endif /* RUTAPI_TRACE_ONLY */
}

tmctl_ret_e _owapi_rut_tmctl_setQueueCfg(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_queueCfg_t *qcfg_p)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_setQueueCfg)(devType, if_p, qcfg_p);
#else
    _owtrace(owapi_rut_tmctl_setQueueCfg)(devType, if_p, qcfg_p);
    return _owapi(rut_tmctl_setQueueCfg)(devType, if_p, qcfg_p);
#endif /* RUTAPI_TRACE_ONLY */
}

tmctl_ret_e _owapi_rut_tmctl_getQueueStats(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueStats_t *stats_p)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_getQueueStats)(devType,if_p, queueId, stats_p);
#else
    _owtrace(owapi_rut_tmctl_getQueueStats)(devType,if_p, queueId, stats_p);
    return _owapi(rut_tmctl_getQueueStats)(devType,if_p, queueId, stats_p);
#endif /* RUTAPI_TRACE_ONLY */
}

tmctl_ret_e _owapi_rut_tmctl_setQueueDropAlgExt(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_tmctl_setQueueDropAlgExt)(devType, if_p, queueId,
      dropAlg_p);
#else
    _owtrace(owapi_rut_tmctl_setQueueDropAlgExt)(devType, if_p, queueId,
      dropAlg_p);
    return _owapi(rut_tmctl_setQueueDropAlgExt)(devType, if_p, queueId,
      dropAlg_p);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_processHostCtrlChange(void)
{
#if defined(RUTAPI_TRACE_ONLY)
   return _owtrace(owapi_rut_processHostCtrlChange)();
#else
   _owtrace(owapi_rut_processHostCtrlChange)();
   return _owapi(rut_processHostCtrlChange)();
#endif /* RUTAPI_TRACE_ONLY */
}

UBOOL8 _owapi_mcast_getHostCtrlConfig(void)
{
#if defined(RUTAPI_TRACE_ONLY)
   return _owtrace(owapi_mcast_getHostCtrlConfig)();
#else
   _owtrace(owapi_mcast_getHostCtrlConfig)();
   return _owapi(mcast_getHostCtrlConfig)();
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_reloadMcastCtrl(void)
{
#if defined(RUTAPI_TRACE_ONLY)
   return _owtrace(owapi_rut_reloadMcastCtrl)();
#else
   _owtrace(owapi_rut_reloadMcastCtrl)();
   return _owapi(rut_reloadMcastCtrl)();
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_getOmciMcastCfgInfo(omciMcastCfgInfo *mcastCfgInfo)
{
#if defined(RUTAPI_TRACE_ONLY)
   return _owtrace(owapi_rut_getOmciMcastCfgInfo)(mcastCfgInfo);
#else
   _owtrace(owapi_rut_getOmciMcastCfgInfo)(mcastCfgInfo);
   return _owapi(rut_getOmciMcastCfgInfo)(mcastCfgInfo);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_setOmciMcastCfgInfo(omciMcastCfgInfo *mcastCfgInfo)
{
#if defined(RUTAPI_TRACE_ONLY)
   return _owtrace(owapi_rut_setOmciMcastCfgInfo)(mcastCfgInfo);
#else
   _owtrace(owapi_rut_setOmciMcastCfgInfo)(mcastCfgInfo);
   return _owapi(rut_setOmciMcastCfgInfo)(mcastCfgInfo);
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_cxtInit(void *msgHandle, const UINT32 eid, SINT32 shmId)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_cxtInit)(msgHandle, eid, shmId);
#else
    _owtrace(owapi_rut_cxtInit)(msgHandle, eid, shmId);
    return _owapi(rut_cxtInit)(msgHandle, eid, shmId);
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_updateIgmpMldSnoopingIntfList(void)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_updateIgmpMldSnoopingIntfList)();
#else
    _owtrace(owapi_rut_updateIgmpMldSnoopingIntfList)();
    return _owapi(rut_updateIgmpMldSnoopingIntfList)();
#endif /* RUTAPI_TRACE_ONLY */
}

UINT32 _owapi_rut_startPing(char *cmdLine)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_startPing)(cmdLine);
#else
    _owtrace(owapi_rut_startPing)(cmdLine);
    return _owapi(rut_startPing)(cmdLine);
#endif /* RUTAPI_TRACE_ONLY */
}

UINT32 _owapi_rut_startTraceroute(char *cmdLine)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_startTraceroute)(cmdLine);
#else
    _owtrace(owapi_rut_startTraceroute)(cmdLine);
    return _owapi(rut_startTraceroute)(cmdLine);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_sendServiceInfoMsg(const OmciServiceMsgBody *pService)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_sendServiceInfoMsg)(pService);
#else
    _owtrace(owapi_rut_sendServiceInfoMsg)(pService);
    return _owapi(rut_sendServiceInfoMsg)(pService);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_configDns(const char *dns1, const char *dns2)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_configDns)(dns1, dns2);
#else
    _owtrace(owapi_rut_configDns)(dns1, dns2);
    return _owapi(rut_configDns)(dns1, dns2);
#endif /* RUTAPI_TRACE_ONLY */
}

#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1
BCM_OMCI_PM_STATUS _owapi_rut_getDnsStats(void *dnsStats)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_getDnsStats)(dnsStats);
#else
    _owtrace(owapi_rut_getDnsStats)(dnsStats);
    return _owapi(rut_getDnsStats)(dnsStats);
#endif /* RUTAPI_TRACE_ONLY */
}
#endif /* DMP_X_BROADCOM_COM_DNSPROXY_1 */

CmsRet _owapi_rut_updateMcastCtrlIfNames(char *mcastIfNames)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_updateMcastCtrlIfNames)(mcastIfNames);
#else
    _owtrace(owapi_rut_updateMcastCtrlIfNames)(mcastIfNames);
    return _owapi(rut_updateMcastCtrlIfNames)(mcastIfNames);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_getBrgFwdMask(UINT32 *brgFwdMask)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_getBrgFwdMask)(brgFwdMask);
#else
    _owtrace(owapi_rut_getBrgFwdMask)(brgFwdMask);
    return _owapi(rut_getBrgFwdMask)(brgFwdMask);
#endif /* RUTAPI_TRACE_ONLY */
}

#ifdef DMP_X_ITU_ORG_VOICE_1
void _owapi_rut_disablePptpPotsUni(UINT32 lineIdx)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_disablePptpPotsUni)(lineIdx);
#else
    _owtrace(owapi_rut_disablePptpPotsUni)(lineIdx);
    return _owapi(rut_disablePptpPotsUni)(lineIdx);
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_enablePptpPotsUni(UINT32 lineIdx)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_enablePptpPotsUni)(lineIdx);
#else
    _owtrace(owapi_rut_enablePptpPotsUni)(lineIdx);
    return _owapi(rut_enablePptpPotsUni)(lineIdx);
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_disableAllPptpPotsUni(void)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_disableAllPptpPotsUni)();
#else
    _owtrace(owapi_rut_disableAllPptpPotsUni)();
    return _owapi(rut_disableAllPptpPotsUni)();
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_enableAllPptpPotsUni(void)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_enableAllPptpPotsUni)();
#else
    _owtrace(owapi_rut_enableAllPptpPotsUni)();
    return _owapi(rut_enableAllPptpPotsUni)();
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_setVoipBoundIfNameAddress(UINT32 ipHostMeId,
  char *ipAddr)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_setVoipBoundIfNameAddress)(ipHostMeId, ipAddr);
#else
    _owtrace(owapi_rut_setVoipBoundIfNameAddress)(ipHostMeId, ipAddr);
    return _owapi(rut_setVoipBoundIfNameAddress)(ipHostMeId, ipAddr);
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_MapVoIpMediaProfile(int lineIdx,
  VoIpMediaProfileObject *omciVoIpMediaProfileObjectPtr)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_MapVoIpMediaProfile)(lineIdx, omciVoIpMediaProfileObjectPtr);
#else
    _owtrace(owapi_rut_MapVoIpMediaProfile)(lineIdx, omciVoIpMediaProfileObjectPtr);
    return _owapi(rut_MapVoIpMediaProfile)(lineIdx, omciVoIpMediaProfileObjectPtr);
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_MapVoiceServiceProfile(int lineIdx,
  VoiceServiceObject *omciVoiceServiceObjectPtr)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_MapVoiceServiceProfile)(lineIdx, omciVoiceServiceObjectPtr);
#else
    _owtrace(owapi_rut_MapVoiceServiceProfile)(lineIdx, omciVoiceServiceObjectPtr);
    return _owapi(rut_MapVoiceServiceProfile)(lineIdx, omciVoiceServiceObjectPtr);
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_MapRtpProfileData(RtpProfileDataObject *omciRtpProfilePtr)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_MapRtpProfileData)(omciRtpProfilePtr);
#else
    _owtrace(owapi_rut_MapRtpProfileData)(omciRtpProfilePtr);
    return _owapi(rut_MapRtpProfileData)(omciRtpProfilePtr);
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_MapPptpPotsUni(int lineIdx,
  PptpPotsUniObject *omciPptpPotsUniObjectPtr)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_MapPptpPotsUni)(lineIdx, omciPptpPotsUniObjectPtr);
#else
    _owtrace(owapi_rut_MapPptpPotsUni)(lineIdx, omciPptpPotsUniObjectPtr);
    return _owapi(rut_MapPptpPotsUni)(lineIdx, omciPptpPotsUniObjectPtr);
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_MapVoIpConfigData(VoIpConfigDataObject *voipConfigDataPtr)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_MapVoIpConfigData)(voipConfigDataPtr);
#else
    _owtrace(owapi_rut_MapVoIpConfigData)(voipConfigDataPtr);
    return _owapi(rut_MapVoIpConfigData)(voipConfigDataPtr);
#endif /* RUTAPI_TRACE_ONLY */
}

UBOOL8 _owapi_rut_IsVoiceOmciManaged(void)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_IsVoiceOmciManaged)();
#else
    _owtrace(owapi_rut_IsVoiceOmciManaged)();
    return _owapi(rut_IsVoiceOmciManaged)();
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_SendUploadComplete(void)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_SendUploadComplete)();
#else
    _owtrace(owapi_rut_SendUploadComplete)();
    return _owapi(rut_SendUploadComplete)();
#endif /* RUTAPI_TRACE_ONLY */
}

BCM_OMCI_PM_STATUS _owapi_rut_GetRtpStats(int lineIdx,
  BCM_OMCI_PM_RTP_COUNTER *rtpStatsRec)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_GetRtpStats)(lineIdx, rtpStatsRec);
#else
    _owtrace(owapi_rut_GetRtpStats)(lineIdx, rtpStatsRec);
    return _owapi(rut_GetRtpStats)(lineIdx, rtpStatsRec);
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_MapNetworkDialPlan(UINT8 digitMap[], UINT32 critDigitTimer,
  UINT32 partDigitTimer)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_MapNetworkDialPlan)(digitMap, critDigitTimer, partDigitTimer);
#else
    _owtrace(owapi_rut_MapNetworkDialPlan)(digitMap, critDigitTimer, partDigitTimer);
    return _owapi(rut_MapNetworkDialPlan)(digitMap, critDigitTimer, partDigitTimer);
#endif /* RUTAPI_TRACE_ONLY */
}

#ifdef DMP_X_ITU_ORG_VOICE_SIP_1

void _owapi_rut_MapVoIpApplServiceProfile(int lineIdx,
  VoIpAppServiceProfileObject *omciVoIpAppServiceProfilePtr)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_MapVoIpApplServiceProfile)(lineIdx, omciVoIpAppServiceProfilePtr);
#else
    _owtrace(owapi_rut_MapVoIpApplServiceProfile)(lineIdx, omciVoIpAppServiceProfilePtr);
    return _owapi(rut_MapVoIpApplServiceProfile)(lineIdx, omciVoIpAppServiceProfilePtr);
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_MapSipAgentConfigData(int lineIdx,
  SipAgentConfigDataObject *omciSipAgentConfigDataPtr)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_MapSipAgentConfigData)(lineIdx, omciSipAgentConfigDataPtr);
#else
    _owtrace(owapi_rut_MapSipAgentConfigData)(lineIdx, omciSipAgentConfigDataPtr);
    return _owapi(rut_MapSipAgentConfigData)(lineIdx, omciSipAgentConfigDataPtr);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_setSipReregHeadStartTime(const UINT32 lineIdx,
  const UINT32 reregVal)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_setSipReregHeadStartTime)(lineIdx, reregVal);
#else
    _owtrace(owapi_rut_setSipReregHeadStartTime)(lineIdx, reregVal);
    return _owapi(rut_setSipReregHeadStartTime)(lineIdx, reregVal);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_setSipRegisterExpirationTime(const UINT32 lineIdx,
  const UINT32 expireVal)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_setSipRegisterExpirationTime)(lineIdx, expireVal);
#else
    _owtrace(owapi_rut_setSipRegisterExpirationTime)(lineIdx, expireVal);
    return _owapi(rut_setSipRegisterExpirationTime)(lineIdx, expireVal);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_setSipOutboundProxyAddress(const UINT32 lineIdx,
  const char *address)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_setSipOutboundProxyAddress)(lineIdx, address);
#else
    _owtrace(owapi_rut_setSipOutboundProxyAddress)(lineIdx, address);
    return _owapi(rut_setSipOutboundProxyAddress)(lineIdx, address);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_setSipAuthUsername(const UINT32 lineIdx, const char *user)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_setSipAuthUsername)(lineIdx, user);
#else
    _owtrace(owapi_rut_setSipAuthUsername)(lineIdx, user);
    return _owapi(rut_setSipAuthUsername)(lineIdx, user);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_setSipAuthPassword(const UINT32 lineIdx, const char *password)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_setSipAuthPassword)(lineIdx, password);
#else
    _owtrace(owapi_rut_setSipAuthPassword)(lineIdx, password);
    return _owapi(rut_setSipAuthPassword)(lineIdx, password);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_setSipUserPartAor(const UINT32 lineIdx,
  const char *userPartAor)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_setSipUserPartAor)(lineIdx, userPartAor);
#else
    _owtrace(owapi_rut_setSipUserPartAor)(lineIdx, userPartAor);
    return _owapi(rut_setSipUserPartAor)(lineIdx, userPartAor);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_setSipProxyServerAddress(const UINT32 lineIdx,
  const char *address)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_setSipProxyServerAddress)(lineIdx, address);
#else
    _owtrace(owapi_rut_setSipProxyServerAddress)(lineIdx, address);
    return _owapi(rut_setSipProxyServerAddress)(lineIdx, address);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_setSipRegistrarAddress(const UINT32 lineIdx,
  const char *address)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_setSipRegistrarAddress)(lineIdx, address);
#else
    _owtrace(owapi_rut_setSipRegistrarAddress)(lineIdx, address);
    return _owapi(rut_setSipRegistrarAddress)(lineIdx, address);
#endif /* RUTAPI_TRACE_ONLY */
}

#endif /* DMP_X_ITU_ORG_VOICE_SIP_1 */

#endif /* DMP_X_ITU_ORG_VOICE_1 */

CmsRet _owapi_rut_addAutoObject(UINT32 oid, UINT32 meId, UBOOL8 persistent)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_addAutoObject)(oid, meId, persistent);
#else
    _owtrace(owapi_rut_addAutoObject)(oid, meId, persistent);
    return _owapi(rut_addAutoObject)(oid, meId, persistent);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_setAutoObject(UINT32 oid, UINT32 meId, void *mdmObj)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_setAutoObject)(oid, meId, mdmObj);
#else
    _owtrace(owapi_rut_setAutoObject)(oid, meId, mdmObj);
    return _owapi(rut_setAutoObject)(oid, meId, mdmObj);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_delAutoObject(UINT32 oid, UINT32 meId)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_delAutoObject)(oid, meId);
#else
    _owtrace(owapi_rut_delAutoObject)(oid, meId);
    return _owapi(rut_delAutoObject)(oid, meId);
#endif /* RUTAPI_TRACE_ONLY */
}

void _owapi_rut_configTcpUdp(const TcpUdpConfigDataObject *tcpUdp, UBOOL8 add)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_configTcpUdp)(tcpUdp, add);
#else
    _owtrace(owapi_rut_configTcpUdp)(tcpUdp, add);
    return _owapi(rut_configTcpUdp)(tcpUdp, add);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_configIpv4Host(UINT32 meId, char *ifName)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_configIpv4Host)(meId, ifName);
#else
    _owtrace(owapi_rut_configIpv4Host)(meId, ifName);
    return _owapi(rut_configIpv4Host)(meId, ifName);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_configIpv6Host(UINT32 meId, char *ifName)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_configIpv6Host)(meId, ifName);
#else
    _owtrace(owapi_rut_configIpv6Host)(meId, ifName);
    return _owapi(rut_configIpv6Host)(meId, ifName);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_updateAcsCfg(UINT32 ipHostMeId, char *acs, UINT8 *username,
  UINT8 *password)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_updateAcsCfg)(ipHostMeId, acs,
      username, password);
#else
    _owtrace(owapi_rut_updateAcsCfg)(ipHostMeId, acs,
      username, password);
    return _owapi(rut_updateAcsCfg)(ipHostMeId, acs, username, password);
#endif /* RUTAPI_TRACE_ONLY */
}

CmsRet _owapi_rut_getIpv4HostInfo(UINT32 meId, omciIpv4HostInfo *ipHostInfo)
{
#if defined(RUTAPI_TRACE_ONLY)
    return _owtrace(owapi_rut_getIpv4HostInfo)(meId, ipHostInfo);
#else
    _owtrace(owapi_rut_getIpv4HostInfo)(meId, ipHostInfo);
    return _owapi(rut_getIpv4HostInfo)(meId, ipHostInfo);
#endif /* RUTAPI_TRACE_ONLY */
}
