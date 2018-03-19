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
*      OMCI header file for OMCI Wrapper Run-time UTility (RUT) APIs.
*
*****************************************************************************/

#ifndef _OWRUT_API_H_
#define _OWRUT_API_H_

/* ---- Include Files ----------------------------------------------------- */

#include "os_defs.h"
#include "omci_msg.h"
#include "omci_object.h"
#include "omcicmn_defs.h"
#include "tmctl_defs.h"
#include "owrut_defs.h"
#include "ctlutils_api.h"
#include "omcipm_api.h"


/* ---- Constants and Types ----------------------------------------------- */


/* ---- Macro API definitions --------------------------------------------- */


/* ---- Variable Externs -------------------------------------------------- */


/* ---- Function Prototypes ----------------------------------------------- */

/* Configuration database related APIs. */

CmsRet _owapi_rut_initAppCfgData(OmciSystemObject *omciCfgData);
CmsRet owapi_rut_initAppCfgData(OmciSystemObject *omciCfgData);

void _owapi_rut_initLoggingFromConfig(UBOOL8 useConfiguredLogLevel);
void owapi_rut_initLoggingFromConfig(UBOOL8 useConfiguredLogLevel);

/* Miscellaneous. */

CmsRet _owapi_rut_announceMibreset(void);
CmsRet owapi_rut_announceMibreset(void);

void _owapi_rut_doSystemAction(const char *task, char *cmd);
void owapi_rut_doSystemAction(const char *task, char *cmd);

void _owapi_rut_systemReboot(void);
void owapi_rut_systemReboot(void);

/* Multicast related APIs. */

void _owapi_rut_updateIgmpMldSnoopingIntfList(void);
void owapi_rut_updateIgmpMldSnoopingIntfList(void);

CmsRet _owapi_rut_processHostCtrlChange(void);
CmsRet owapi_rut_processHostCtrlChange(void);

UBOOL8 _owapi_mcast_getHostCtrlConfig(void);
UBOOL8 owapi_mcast_getHostCtrlConfig(void);

CmsRet _owapi_rut_updateMcastCtrlIfNames(char *mcastIfNames);
CmsRet owapi_rut_updateMcastCtrlIfNames(char *mcastIfNames);

CmsRet _owapi_rut_reloadMcastCtrl(void);
CmsRet owapi_rut_reloadMcastCtrl(void);

/* TODO: the API should be per UNI port based. */
CmsRet _owapi_rut_getOmciMcastCfgInfo(omciMcastCfgInfo *mcastCfgInfo);
CmsRet owapi_rut_getOmciMcastCfgInfo(omciMcastCfgInfo *mcastCfgInfo);

CmsRet _owapi_rut_setOmciMcastCfgInfo(omciMcastCfgInfo *mcastCfgInfo);
CmsRet owapi_rut_setOmciMcastCfgInfo(omciMcastCfgInfo *mcastCfgInfo);

/* TMCtl related APIs. */

BOOL _owapi_rut_tmctl_isUserTMCtlOwner(eTmctlUser user);
BOOL owapi_rut_tmctl_isUserTMCtlOwner(eTmctlUser user);

UINT32 _owapi_rut_tmctl_getQueueOwner(void);
UINT32 owapi_rut_tmctl_getQueueOwner(void);

UINT32 _owapi_rut_tmctl_getQueueMap(void);
UINT32 owapi_rut_tmctl_getQueueMap(void);

CmsRet _owapi_rut_tmctl_getQueueScheme(UINT32 *owner, UINT32 *scheme);
CmsRet owapi_rut_tmctl_getQueueScheme(UINT32 *owner, UINT32 *scheme);

tmctl_ret_e _owapi_rut_tmctl_portTmInit(tmctl_devType_e devType,
  tmctl_if_t *if_p, uint32_t cfgFlags);
tmctl_ret_e owapi_rut_tmctl_portTmInit(tmctl_devType_e devType,
  tmctl_if_t *if_p, uint32_t cfgFlags);

tmctl_ret_e _owapi_rut_tmctl_portTmUninit(tmctl_devType_e devType,
  tmctl_if_t *if_p);
tmctl_ret_e owapi_rut_tmctl_portTmUninit(tmctl_devType_e devType,
  tmctl_if_t *if_p);

tmctl_ret_e _owapi_rut_tmctl_getQueueCfg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueCfg_t *qcfg_p);
tmctl_ret_e owapi_rut_tmctl_getQueueCfg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueCfg_t *qcfg_p);

tmctl_ret_e _owapi_rut_tmctl_delQueueCfg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId);
tmctl_ret_e owapi_rut_tmctl_delQueueCfg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId);

tmctl_ret_e _owapi_rut_tmctl_getPortShaper(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_shaper_t *shaper_p);
tmctl_ret_e owapi_rut_tmctl_getPortShaper(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_shaper_t *shaper_p);

tmctl_ret_e _owapi_rut_tmctl_setPortShaper(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_shaper_t *shaper_p);
tmctl_ret_e owapi_rut_tmctl_setPortShaper(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_shaper_t *shaper_p);

tmctl_ret_e _owapi_rut_tmctl_getQueueDropAlg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p);
tmctl_ret_e owapi_rut_tmctl_getQueueDropAlg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p);

tmctl_ret_e _owapi_rut_tmctl_setQueueDropAlg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p);
tmctl_ret_e owapi_rut_tmctl_setQueueDropAlg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p);

tmctl_ret_e _owapi_rut_tmctl_getQueueStats(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueStats_t *stats_p);
tmctl_ret_e owapi_rut_tmctl_getQueueStats(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueStats_t *stats_p);

tmctl_ret_e _owapi_rut_tmctl_getPortTmParms(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_portTmParms_t *tmParms_p);
tmctl_ret_e owapi_rut_tmctl_getPortTmParms(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_portTmParms_t *tmParms_p);

tmctl_ret_e _owapi_rut_tmctl_setQueueCfg(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_queueCfg_t *qcfg_p);
tmctl_ret_e owapi_rut_tmctl_setQueueCfg(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_queueCfg_t *qcfg_p);

tmctl_ret_e _owapi_rut_tmctl_getQueueStats(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueStats_t *stats_p);
tmctl_ret_e owapi_rut_tmctl_getQueueStats(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueStats_t *stats_p);

tmctl_ret_e _owapi_rut_tmctl_setQueueDropAlgExt(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p);
tmctl_ret_e owapi_rut_tmctl_setQueueDropAlgExt(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p);

void _owapi_rut_cxtInit(void *msgHandle, const UINT32 eid, SINT32 shmId);
void owapi_rut_cxtInit(void *msgHandle, const UINT32 eid, SINT32 shmId);

/* Interface related APIs. */

CmsRet _owapi_rut_getBrgFwdMask(UINT32 *brgFwdMask);
CmsRet owapi_rut_getBrgFwdMask(UINT32 *brgFwdMask);

/* Layer3 (IP host) function related APIs. */

UINT32 _owapi_rut_startPing(char *cmd);
UINT32 owapi_rut_startPing(char *cmd);

UINT32 _owapi_rut_startTraceroute(char *cmd);
UINT32 owapi_rut_startTraceroute(char *cmd);

CmsRet _owapi_rut_configDns(const char *dns1, const char *dns2);
CmsRet owapi_rut_configDns(const char *dns1, const char *dns2);

BCM_OMCI_PM_STATUS _owapi_rut_getDnsStats(void *dnsStats);
BCM_OMCI_PM_STATUS owapi_rut_getDnsStats(void *dnsStats);

void _owapi_rut_configTcpUdp(const TcpUdpConfigDataObject *tcpUdp, UBOOL8 add);
void owapi_rut_configTcpUdp(const TcpUdpConfigDataObject *tcpUdp, UBOOL8 add);

CmsRet _owapi_rut_configIpv4Host(UINT32 meId, char *ifName);
CmsRet owapi_rut_configIpv4Host(UINT32 meId, char *ifName);

CmsRet _owapi_rut_configIpv6Host(UINT32 meId, char *ifName);
CmsRet owapi_rut_configIpv6Host(UINT32 meId, char *ifName);

CmsRet _owapi_rut_getIpv4HostInfo(UINT32 meId, omciIpv4HostInfo *hostInfo);
CmsRet owapi_rut_getIpv4HostInfo(UINT32 meId, omciIpv4HostInfo *hostInfo);

/* Gateway function related APIs. */

CmsRet _owapi_rut_sendServiceInfoMsg(const OmciServiceMsgBody *pService);
CmsRet owapi_rut_sendServiceInfoMsg(const OmciServiceMsgBody *pService);

/* Voice service related APIs. */

void owapi_rut_disablePptpPotsUni(UINT32 lineIdx);
void _owapi_rut_disablePptpPotsUni(UINT32 lineIdx);

void owapi_rut_enablePptpPotsUni(UINT32 lineIdx);
void _owapi_rut_enablePptpPotsUni(UINT32 lineIdx);

void owapi_rut_disableAllPptpPotsUni(void);
void _owapi_rut_disableAllPptpPotsUni(void);

void owapi_rut_enableAllPptpPotsUni(void);
void _owapi_rut_enableAllPptpPotsUni(void);

UBOOL8 _owapi_rut_IsVoiceOmciManaged(void);
UBOOL8 owapi_rut_IsVoiceOmciManaged(void);

CmsRet _owapi_rut_setVoipBoundIfNameAddress(UINT32 ipHostMeId,
  char *ipAddr);
CmsRet owapi_rut_setVoipBoundIfNameAddress(UINT32 ipHostMeId,
  char *ipAddr);

/* Mapping APIs: map OMCI parameters into the voice stack maintained objects. */
void _owapi_rut_MapVoIpMediaProfile(int lineIdx,
  VoIpMediaProfileObject *omciVoIpMediaProfileObjectPtr);
void owapi_rut_MapVoIpMediaProfile(int lineIdx,
  VoIpMediaProfileObject *omciVoIpMediaProfileObjectPtr);

void _owapi_rut_MapVoiceServiceProfile(int lineIdx,
  VoiceServiceObject *omciVoiceServiceObjectPtr);
void owapi_rut_MapVoiceServiceProfile(int lineIdx,
  VoiceServiceObject *omciVoiceServiceObjectPtr);

void _owapi_rut_MapRtpProfileData(RtpProfileDataObject *omciRtpProfilePtr);
void owapi_rut_MapRtpProfileData(RtpProfileDataObject *omciRtpProfilePtr);

void _owapi_rut_MapPptpPotsUni(int lineIdx,
  PptpPotsUniObject *omciPptpPotsUniObjectPtr);
void owapi_rut_MapPptpPotsUni(int lineIdx,
  PptpPotsUniObject *omciPptpPotsUniObjectPtr);

void _owapi_rut_MapVoIpApplServiceProfile(int lineIdx,
  VoIpAppServiceProfileObject *omciVoIpAppServiceProfilePtr);
void owapi_rut_MapVoIpApplServiceProfile(int lineIdx,
  VoIpAppServiceProfileObject *omciVoIpAppServiceProfilePtr);

void _owapi_rut_MapVoIpConfigData(VoIpConfigDataObject *voipConfigDataPtr);
void owapi_rut_MapVoIpConfigData(VoIpConfigDataObject *voipConfigDataPtr);

void _owapi_rut_MapSipAgentConfigData(int lineIdx,
  SipAgentConfigDataObject *omciSipAgentConfigDataPtr);
void owapi_rut_MapSipAgentConfigData(int lineIdx,
  SipAgentConfigDataObject *omciSipAgentConfigDataPtr);

BCM_OMCI_PM_STATUS _owapi_rut_GetRtpStats(int physPortId,
  BCM_OMCI_PM_RTP_COUNTER *rtpStatsRec);
BCM_OMCI_PM_STATUS owapi_rut_GetRtpStats(int physPortId,
  BCM_OMCI_PM_RTP_COUNTER *rtpStatsRec);

void _owapi_rut_MapNetworkDialPlan(UINT8 digitMap[], UINT32 critDigitTimer,
  UINT32 partDigitTimer);
void owapi_rut_MapNetworkDialPlan(UINT8 digitMap[], UINT32 critDigitTimer,
  UINT32 partDigitTimer);

CmsRet _owapi_rut_setSipReregHeadStartTime(const UINT32 lineIdx,
  const UINT32 reregVal);
CmsRet owapi_rut_setSipReregHeadStartTime(const UINT32 lineIdx,
  const UINT32 reregVal);

CmsRet _owapi_rut_setSipRegisterExpirationTime(const UINT32 lineIdx,
  const UINT32 expireVal);
CmsRet owapi_rut_setSipRegisterExpirationTime(const UINT32 lineIdx,
  const UINT32 expireVal);

CmsRet _owapi_rut_setSipOutboundProxyAddress(const UINT32 lineIdx,
  const char *address);
CmsRet owapi_rut_setSipOutboundProxyAddress(const UINT32 lineIdx,
  const char *address);

CmsRet _owapi_rut_setSipAuthUsername(const UINT32 lineIdx, const char *user);
CmsRet owapi_rut_setSipAuthUsername(const UINT32 lineIdx, const char *user);

CmsRet _owapi_rut_setSipAuthPassword(const UINT32 lineIdx, const char *password);
CmsRet owapi_rut_setSipAuthPassword(const UINT32 lineIdx, const char *password);

CmsRet _owapi_rut_setSipUserPartAor(const UINT32 lineIdx,
  const char *userPartAor);
CmsRet owapi_rut_setSipUserPartAor(const UINT32 lineIdx,
  const char *userPartAor);

CmsRet _owapi_rut_setSipProxyServerAddress(const UINT32 lineIdx,
  const char *address);
CmsRet owapi_rut_setSipProxyServerAddress(const UINT32 lineIdx,
  const char *address);

CmsRet _owapi_rut_setSipRegistrarAddress(const UINT32 lineIdx,
  const char *address);
CmsRet owapi_rut_setSipRegistrarAddress(const UINT32 lineIdx,
  const char *address);

CmsRet _owapi_rut_SendUploadComplete(void);
CmsRet owapi_rut_SendUploadComplete(void);

/* Dual-management related APIs. */

CmsRet _owapi_rut_updateAcsCfg(UINT32 ipHostMeId, char *acs, UINT8 *username,
  UINT8 *password);
CmsRet owapi_rut_updateAcsCfg(UINT32 ipHostMeId, char *acs, UINT8 *username,
  UINT8 *password);

/* Auto-object related APIs. */

CmsRet _owapi_rut_addAutoObject(UINT32 omciOid, UINT32 meId, UBOOL8 persistent);
CmsRet owapi_rut_addAutoObject(UINT32 omciOid, UINT32 meId, UBOOL8 persistent);

CmsRet _owapi_rut_setAutoObject(UINT32 omciOid, UINT32 meId, void *mdmObj);
CmsRet owapi_rut_setAutoObject(UINT32 omciOid, UINT32 meId, void *mdmObj);

CmsRet _owapi_rut_delAutoObject(UINT32 omciOid, UINT32 meId);
CmsRet owapi_rut_delAutoObject(UINT32 omciOid, UINT32 meId);


#endif /* _OWRUT_API_H_ */
