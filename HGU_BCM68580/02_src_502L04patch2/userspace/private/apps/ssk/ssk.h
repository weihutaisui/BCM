/*
#
#  Copyright 2012, Broadcom Corporation
#
# <:label-BRCM:2012:proprietary:standard
# 
#  This program is the proprietary software of Broadcom and/or its
#  licensors, and may only be used, duplicated, modified or distributed pursuant
#  to the terms and conditions of a separate, written license agreement executed
#  between you and Broadcom (an "Authorized License").  Except as set forth in
#  an Authorized License, Broadcom grants no license (express or implied), right
#  to use, or waiver of any kind with respect to the Software, and Broadcom
#  expressly reserves all rights in and to the Software and all intellectual
#  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
#  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
#  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
# 
#  Except as expressly set forth in the Authorized License,
# 
#  1. This program, including its structure, sequence and organization,
#     constitutes the valuable trade secrets of Broadcom, and you shall use
#     all reasonable efforts to protect the confidentiality thereof, and to
#     use this information only in connection with your use of Broadcom
#     integrated circuit products.
# 
#  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
#     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
#     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
#     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
#     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
#     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
#     PERFORMANCE OF THE SOFTWARE.
# 
#  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
#     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
#     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
#     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
#     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
#     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
#     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
#     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
#     LIMITED REMEDY.
# :>
*/

#ifndef _SSK_H_
#define _SSK_H_

#include "cms.h"
#include "cms_msg.h"
#include "cms_msg_cellular.h"

/** Need to keep track of the previous link status for each WAN link
 *  so I know whether their state has changed or not.
 *  This is used by TR98 WAN side code only.
 */
typedef struct
{
   DlistNode dlist;   
   UBOOL8 isUp;                /**< Is the wan layer 2 link up */
   UBOOL8 isATM;               /**< Is this an ATM wan link */
   UBOOL8 isPTM;               /**< Is this a PTM wan link */
   UBOOL8 isEth;               /**< Is this an ethernet wan link */
   UBOOL8 isMoca;              /**< Is this a moca wan link */
   UBOOL8 isGpon;              /**< Is this a gpon wan link */
   UBOOL8 isEpon;              /**< Is this a epon wan link */   
   UBOOL8 isWifi;              /**< Is this an wifi wan link */   
   InstanceIdStack iidStack;   /**< Instance Id Stack for this wan link */
} WanLinkInfo;

/** This is a state description uesd by interface temporarily moved to another bridge*/
typedef enum
{
   tmpMoveNo,      /**< no tmpmove action on this intreface */
   tmpMoveDoing,   /**< intreface is moving to another bridge, but have't been up*/
   tmpMoveDone     /**< intreface have moved to another bridge, and have been up*/
}TmpMoveState;

/** This struct is used by the TR98 LAN side code and TR181 (WAN and LAN side)
 *  for link status tracking.
 */
typedef struct
{
   DlistNode dlist;        /**< linked list header */
   char ifName[CMS_IFNAME_LENGTH]; /**< ifname */
   UBOOL8 isWan;    /**< Is this a WAN interface */
   UBOOL8 isLinkUp; /**< Is link up */
   TmpMoveState tmpMoved; /**< LAN only: is interface temporarily moved to another bridge */
   UBOOL8 moveBack; /**< LAN only: Does this interface need to be moved back to original bridge */
   char tmpBridgeIfName[CMS_IFNAME_LENGTH];  /**< LAN only: Name of bridge where this is temporarily moved to */
} SskLinkStatusRecord;


extern struct dlist_node sskLinkStatusRecordHead;




/** Max number of milliseconds to wait for a MDM read lock.
 *
 * Some apps, most notably wireless and voice, sometimes hold the lock for 
 * a lot longer than CMSLCK_MAX_HOLDTIME.  ssk runs in the background,
 * so it can afford to wait longer for the lock.
 */
#define SSK_LOCK_TIMEOUT  (15*MSECS_IN_SEC)


/** Interval (in seconds) between periodic task execution.
 *
 * Hopefully we will not need to run more than once every 60 seconds to
 * keep data model stuff up-to-date.
 */
#define PERIODIC_TASK_INTERVAL 15

/** For auto detection, each WAN connection (IPoE/PPPoE) will be tried for the following seconds
  * and this value can be changed accordingly for getting an external IP in dhcp and pppoe.
  */
#define AUTO_DETECT_TASK_INTERVAL          20

#define MAX_NUMBER_OF_VENDOR_CONFIG_RECORD 4

/* in ssk.c */
void sendStatusMsgToSmd(CmsMsgType msgType, const char *ifName);

extern void *msgHandle;
extern UBOOL8 isLowMemorySystem;
extern CmsTimestamp bootTimestamp;



/*
 * Answer a query msg about whether any WAN connection is up or
 * if a specific WAN connection is up.  "UP" is defined as having an IPv4
 * or IPv6 address.
 */
void processGetWanConnStatus(CmsMsgHeader *msg);

void processGetWanConnStatus_igd(CmsMsgHeader *msg);

void processGetWanConnStatus_dev2(CmsMsgHeader *msg);

#if defined(SUPPORT_DM_LEGACY98)
#define processGetWanConnStatus(m)  processGetWanConnStatus_igd((m))
#elif defined(SUPPORT_DM_HYBRID)
#define processGetWanConnStatus(m)  processGetWanConnStatus_igd((m))
#elif defined(SUPPORT_DM_PURE181)
#define processGetWanConnStatus(m)  processGetWanConnStatus_dev2((m))
#elif defined(SUPPORT_DM_DETECT)
#define processGetWanConnStatus(m)  (cmsMdm_isDataModelDevice2() ? \
                                     processGetWanConnStatus_dev2((m)) : \
                                     processGetWanConnStatus_igd((m)))
#endif




/*
 * Process a msg from DHCPv4 client.
 */
void processDhcpcStateChanged(CmsMsgHeader *msg);

void processDhcpcStateChanged_igd(CmsMsgHeader *msg);

void processDhcpcStateChanged_dev2(CmsMsgHeader *msg);

#if defined(SUPPORT_DM_LEGACY98)
#define processDhcpcStateChanged(m)  processDhcpcStateChanged_igd((m))
#elif defined(SUPPORT_DM_HYBRID)
#define processDhcpcStateChanged(m)  processDhcpcStateChanged_igd((m))
#elif defined(SUPPORT_DM_PURE181)
#define processDhcpcStateChanged(m)  processDhcpcStateChanged_dev2((m))
#elif defined(SUPPORT_DM_DETECT)
#define processDhcpcStateChanged(m)  (cmsMdm_isDataModelDevice2() ? \
                                      processDhcpcStateChanged_dev2((m)) : \
                                      processDhcpcStateChanged_igd((m)))
#endif


/*
 * Process CMS_MSG_DHCPC_REQ_OPTION_REPORT msg from DHCPv4 client.
 */
void processDhcpcReqOptionReport(CmsMsgHeader *msg);

void processDhcpcReqOptionReport_igd(CmsMsgHeader *msg);

void processDhcpcReqOptionReport_dev2(CmsMsgHeader *msg);

#if defined(SUPPORT_DM_LEGACY98)
#define processDhcpcReqOptionReport(m)  processDhcpcReqOptionReport_igd((m))
#elif defined(SUPPORT_DM_HYBRID)
#define processDhcpcReqOptionReport(m)  processDhcpcReqOptionReport_igd((m))
#elif defined(SUPPORT_DM_PURE181)
#define processDhcpcReqOptionReport(m)  processDhcpcReqOptionReport_dev2((m))
#elif defined(SUPPORT_DM_DETECT)
#define processDhcpcReqOptionReport(m)  (cmsMdm_isDataModelDevice2() ? \
                                      processDhcpcReqOptionReport_dev2((m)) : \
                                      processDhcpcReqOptionReport_igd((m)))
#endif




/*
 * Process a msg from PPP client.
 */
void processPppStateChanged(CmsMsgHeader *msg);

void processPppStateChanged_igd(CmsMsgHeader *msg);

void processPppStateChanged_dev2(CmsMsgHeader *msg);

#if defined(SUPPORT_DM_LEGACY98)
#define processPppStateChanged(m)  processPppStateChanged_igd((m))
#elif defined(SUPPORT_DM_HYBRID)
#define processPppStateChanged(m)  processPppStateChanged_igd((m))
#elif defined(SUPPORT_DM_PURE181)
#define processPppStateChanged(m)  processPppStateChanged_dev2((m))
#elif defined(SUPPORT_DM_DETECT)
#define processPppStateChanged(m)  (cmsMdm_isDataModelDevice2() ? \
                                    processPppStateChanged_dev2((m)) : \
                                    processPppStateChanged_igd((m)))
#endif


#ifdef SUPPORT_CELLULAR
void processCellularStateChanged(CmsMsgHeader *msg);
#endif


/** Top level function to force ssk to update its knowledge of all WAN and
 *  LAN link statuses.
 *
 * @param intfName (IN) If NULL, then ssk must query/update all link statuses.
 *                      If not NULL, then ssk only needs to query/update the
 *                      link status of the specified intfName.
 */
void updateLinkStatus(const char *intfName);
void updateLinkStatus_igd(const char *intfName);
void updateLinkStatus_dev2(const char *intfName);

#if defined(SUPPORT_DM_LEGACY98)
#define updateLinkStatus(i)             updateLinkStatus_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define updateLinkStatus(i)             updateLinkStatus_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define updateLinkStatus(i)             updateLinkStatus_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define updateLinkStatus(i)             (cmsMdm_isDataModelDevice2() ? \
                                         updateLinkStatus_dev2((i)) : \
                                         updateLinkStatus_igd((i)))
#endif




/* in linkstatus_wan.c */

/** process message to get WAN link status
 *
 */
void processGetWanLinkStatus(CmsMsgHeader *msg);
void processGetWanLinkStatus_igd(CmsMsgHeader *msg);
void processGetWanLinkStatus_dev2(CmsMsgHeader *msg);

#if defined(SUPPORT_DM_LEGACY98)
#define processGetWanLinkStatus(m)  processGetWanLinkStatus_igd(m)
#elif defined(SUPPORT_DM_HYBRID)
#define processGetWanLinkStatus(m)  processGetWanLinkStatus_igd(m)
#elif defined(SUPPORT_DM_PURE181)
#define processGetWanLinkStatus(m)  processGetWanLinkStatus_dev2(m)
#elif defined(SUPPORT_DM_DETECT)
#define processGetWanLinkStatus(m)  (cmsMdm_isDataModelDevice2() ? \
                                     processGetWanLinkStatus_dev2(m) : \
                                     processGetWanLinkStatus_igd(m))
#endif




/** Free all wanLinkInfo structs (free allocations for mem leak checker).
 *  This is not needed in Pure181 mode.
 */
void freeWanLinkInfoList(void);
void freeWanLinkInfoList_igd(void);

#if defined(SUPPORT_DM_LEGACY98)
#define freeWanLinkInfoList()           freeWanLinkInfoList_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define freeWanLinkInfoList()           freeWanLinkInfoList_igd()
#elif defined(SUPPORT_DM_PURE181)
#define freeWanLinkInfoList()
#elif defined(SUPPORT_DM_DETECT)
#define freeWanLinkInfoList()      do { if (!cmsMdm_isDataModelDevice2()) \
                                          freeWanLinkInfoList_igd(); } while(0)
#endif




void processWatchDslLoopDiag(CmsMsgHeader *msg);
void getDslLoopDiagResults(void);
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1   
void processGponWanServiceStatusChange(CmsMsgHeader *msg);
void processGponWanServiceStatusChange_igd(CmsMsgHeader *msg);
void processGponWanServiceStatusChange_dev2(CmsMsgHeader *msg);

#if defined(SUPPORT_DM_LEGACY98)
#define processGponWanServiceStatusChange(m)  processGponWanServiceStatusChange_igd(m)
#elif defined(SUPPORT_DM_HYBRID)
#define processGponWanServiceStatusChange(m)  processGponWanServiceStatusChange_igd(m)
#elif defined(SUPPORT_DM_PURE181)
#define processGponWanServiceStatusChange(m)  processGponWanServiceStatusChange_dev2(m)
#elif defined(SUPPORT_DM_DETECT)
#define processGponWanServiceStatusChange(m)  (cmsMdm_isDataModelDevice2() ? \
                                     processGponWanServiceStatusChange_igd(m) : \
                                     processGponWanServiceStatusChange_dev2(m))
#endif
#endif

#ifdef SUPPORT_DSL_BONDING
void setWanDslTrafficTypeAndRestart (void);
void setWanDslTrafficType (void);
#endif /* support_dsl_bonding */

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1   
void processEponWanLinkChange(CmsMsgHeader *msg);
void processEponWanLinkChange_igd(CmsMsgHeader *msg);
void processEponWanLinkChange_dev2(CmsMsgHeader *msg);

#if defined(SUPPORT_DM_LEGACY98)
#define processEponWanLinkChange(m)  processEponWanLinkChange_igd(m)
#elif defined(SUPPORT_DM_HYBRID)
#define processEponWanLinkChange(m)  processEponWanLinkChange_igd(m)
#elif defined(SUPPORT_DM_PURE181)
#define processEponWanLinkChange(m)  processEponWanLinkChange_dev2(m)
#elif defined(SUPPORT_DM_DETECT)
#define processEponWanLinkChange(m)  (cmsMdm_isDataModelDevice2() ? \
                                     processEponWanLinkChange_dev2(m) : \
                                     processEponWanLinkChange_igd(m))
#endif
#endif

#if defined (DMP_X_BROADCOM_COM_GPONWAN_1) || defined (DMP_X_BROADCOM_COM_EPONWAN_1)
void processWanLinkOpState(CmsMsgHeader *msg);
#endif /* defined (DMP_X_BROADCOM_COM_GPONWAN_1) || defined (DMP_X_BROADCOM_COM_EPONWAN_1) */

/** Refresh our info about wan link status
 */
UBOOL8 checkWanLinkStatusLocked_igd(const char *intfName);


/* in connstatus.c  and ssk2_connstatus.c
 *
 * process ppp request change message 
 *
 */
void processRequestPppChange(const CmsMsgHeader *msg);
void processRequestPppChange_igd(const CmsMsgHeader *msg);
void processRequestPppChange_dev2(const CmsMsgHeader *msg);

#if defined(SUPPORT_DM_LEGACY98)
#define processRequestPppChange(m)  processRequestPppChange_igd(m)
#elif defined(SUPPORT_DM_HYBRID)
#define processRequestPppChange(m)  processRequestPppChange_igd(m)
#elif defined(SUPPORT_DM_PURE181)
#define processRequestPppChange(m)  processRequestPppChange_dev2(m)
#elif defined(SUPPORT_DM_DETECT)
#define processRequestPppChange(m)  (cmsMdm_isDataModelDevice2() ? \
                                     processRequestPppChange_dev2(m) : \
                                     processRequestPppChange_igd(m))
#endif



CmsRet processWatchWanConnectionMsg(CmsMsgHeader *msg);
void updateWanConnStatusInSubtreeLocked(const InstanceIdStack *iidStack, UBOOL8 isLinkUp);
void updateSingleWanConnStatusLocked(const InstanceIdStack *iidStack, void *wanConnObj, UBOOL8 wanLinkUp);

/* in connstatus-n.c */
void processAutoDetectTask(void);
void updateWanConnStatusInSubtreeLocked_n(const InstanceIdStack *parentIidStack, 
                                          UBOOL8  isLinkUp, 
                                          UBOOL8 startNewConnection);
void addNewWanConnObj(const InstanceIdStack *iidStack, void *wanConnObj);
void updateAutoDetectWanConnListForDeletion(const InstanceIdStack *iidStack, MdmObjectId wanConnOid);
void  stopAllWanConn(const InstanceIdStack *parentIidStack, UBOOL8 newAutoDetectEnabled);
extern UBOOL8 isAutoDetectionEnabled;




/* in linkstatus_lan.c */

/** Return TRUE if the link status of the given ifName has changed.
 *  If the given ifName is unknown, a linkStatusRecord will be created
 *  with initial state of link down.
 *  This function is used by the TR98 LAN side code and the TR181 LAN and
 *  WAN side code for tracking link status.
 *
 *  @param ifName (IN) intf name being queried
 *  @param isWan  (IN) TRUE if this is a Upstream/WAN interface
 *  @param status (IN) new link status: MDMVS_UP if UP, any other string is
 *                     considered not up.
 *
 *  @return TRUE if the link status has changed.
 */
UBOOL8 comparePreviousLinkStatus(const char *ifName, UBOOL8 isWan, const char *status);



SskLinkStatusRecord *getLinkStatusRecord(const char *ifName);

void cleanupLinkStatusRecords(void);


/** process message to get LAN link status
 */
void processGetLanLinkStatus(CmsMsgHeader *msg);
void processGetLanLinkStatus_igd(CmsMsgHeader *msg);
void processGetLanLinkStatus_dev2(CmsMsgHeader *msg);
#ifdef SUPPORT_HOMEPLUG
void processGetIFLinkStatus(CmsMsgHeader *msg);
#endif

#if defined(SUPPORT_DM_LEGACY98)
#define processGetLanLinkStatus(m)  processGetLanLinkStatus_igd(m)
#elif defined(SUPPORT_DM_HYBRID)
#define processGetLanLinkStatus(m)  processGetLanLinkStatus_igd(m)
#elif defined(SUPPORT_DM_PURE181)
#define processGetLanLinkStatus(m)  processGetLanLinkStatus_dev2(m)
#elif defined(SUPPORT_DM_DETECT)
#define processGetLanLinkStatus(m)  (cmsMdm_isDataModelDevice2() ? \
                                     processGetLanLinkStatus_dev2(m) : \
                                     processGetLanLinkStatus_igd(m))
#endif


/** Top level function for ssk to trigger a check of the link statuses of
 * the LAN side interfaces.
 */
UBOOL8 checkLanLinkStatusLocked_igd(const char *intfName);


/** check homeplug link status.
 *  This function is used by Hybrid TR98+181 and Pure181.
 */
void checkHomePlugLinkStatusLocked(const char *intfName);


#ifdef DMP_BRIDGING_1
void processDhcpdDenyVendorId(CmsMsgHeader *msg);
#endif


/*
 * Process a msg from DHCPv6 client.
 */
void processDhcp6cStateChanged(const CmsMsgHeader *msg);
void processDhcp6cStateChanged_igd(const CmsMsgHeader *msg);
void processDhcp6cStateChanged_dev2(const CmsMsgHeader *msg);

#if defined(SUPPORT_DM_LEGACY98)
#define processDhcp6cStateChanged(m)  processDhcp6cStateChanged_igd((m))
#elif defined(SUPPORT_DM_HYBRID)
#define processDhcp6cStateChanged(m)  processDhcp6cStateChanged_dev2((m))
#elif defined(SUPPORT_DM_PURE181)
#define processDhcp6cStateChanged(m)  processDhcp6cStateChanged_dev2((m))
#elif defined(SUPPORT_DM_DETECT)
#define processDhcp6cStateChanged(m)  processDhcpcStateChanged_dev2((m))
#endif


/*
 * Process a msg from Router Advertisement.
 */
void processRAStatus6Info(const CmsMsgHeader *msg);
void processRAStatus6Info_igd(const CmsMsgHeader *msg);
void processRAStatus6Info_dev2(const CmsMsgHeader *msg);

#if defined(SUPPORT_DM_LEGACY98)
#define processRAStatus6Info(m)  processRAStatus6Info_igd((m))
#elif defined(SUPPORT_DM_HYBRID)
#define processRAStatus6Info(m)  processRAStatus6Info_dev2((m))
#elif defined(SUPPORT_DM_PURE181)
#define processRAStatus6Info(m)  processRAStatus6Info_dev2((m))
#elif defined(SUPPORT_DM_DETECT)
#define processRAStatus6Info(m)  processRAStatus6Info_dev2((m))
#endif


#ifdef SUPPORT_MOCA

/* in mocastatus.c */
void initMocaMonitorFd(void);
void cleanupMocaMonitorFd(void);
void processMocaMonitor(void);

#ifdef BRCM_MOCA_DAEMON
void processMoCAWriteLof(CmsMsgHeader *);
void processMoCAReadLof(CmsMsgHeader *);
void processMoCAWriteMRNonDefSeqNum(CmsMsgHeader *pMsg);
void processMoCAReadMRNonDefSeqNum(CmsMsgHeader *pMsg);
void processMoCANotification(CmsMsgHeader *pMsg);
#endif
#endif



/* in lanhosts.c */
void processLanHostInfoMsg(CmsMsgHeader *msg);
void processLanHostInfoMsg_igd(CmsMsgHeader *msg);
void processLanHostInfoMsg_dev2(CmsMsgHeader *msg);

#if defined(SUPPORT_DM_LEGACY98)
#define processLanHostInfoMsg(m)  processLanHostInfoMsg_igd((m))
#elif defined(SUPPORT_DM_HYBRID)
#define processLanHostInfoMsg(m)  processLanHostInfoMsg_igd((m))
#elif defined(SUPPORT_DM_PURE181)
#define processLanHostInfoMsg(m)  processLanHostInfoMsg_dev2((m))
#elif defined(SUPPORT_DM_DETECT)
#define processLanHostInfoMsg(m)  (cmsMdm_isDataModelDevice2() ? \
                                     processLanHostInfoMsg_dev2((m)) : \
                                     processLanHostInfoMsg_igd((m)))
#endif

void getDslDiagResults(void);

#ifdef DMP_DSLDIAGNOSTICS_1
typedef struct
{
   UBOOL8 dslLoopDiagInProgress;
   InstanceIdStack iidStack;
   int pollRetries;
   CmsEntityId src;
} dslLoopDiag;

#define DIAG_DSL_LOOPBACK_TIMEOUT_PERIOD 150 /* maximum 2.5 minutes */

/** Function to check for DSL loop diag completion
 *
 */
void processWatchDslLoopDiag(CmsMsgHeader *msg);
void processWatchDslLoopDiag_igd(CmsMsgHeader *msg);
void processWatchDslLoopDiag_dev2(CmsMsgHeader *msg);

#if defined(SUPPORT_DM_LEGACY98)
#define processWatchDslLoopDiag(m)  processWatchDslLoopDiag_igd((m))
#elif defined(SUPPORT_DM_HYBRID)
#define processWatchDslLoopDiag(m)  processWatchDslLoopDiag_igd((m))
#elif defined(SUPPORT_DM_PURE181)
#define processWatchDslLoopDiag(m)  processWatchDslLoopDiag_dev2((m))
#elif defined(SUPPORT_DM_DETECT)
#define processWatchDslLoopDiag(m)  (cmsMdm_isDataModelDevice2() ? \
                                     processWatchDslLoopDiag_dev2((m)) : \
                                     processWatchDslLoopDiag_igd((m)))
#endif


/** Report DSL Loop diag results
 *
 */
void getDslLoopDiagResults(void);
void getDslLoopDiagResults_igd(void);
void getDslLoopDiagResults_dev2(void);

#if defined(SUPPORT_DM_LEGACY98)
#define getDslLoopDiagResults(void)    getDslLoopDiagResults_igd(void)
#elif defined(SUPPORT_DM_HYBRID)
#define getDslLoopDiagResults(void)    getDslLoopDiagResults_igd(void)
#elif defined(SUPPORT_DM_PURE181)
#define getDslLoopDiagResults(void)    getDslLoopDiagResults_dev2(void)
#elif defined(SUPPORT_DM_DETECT)
#define getDslLoopDiagResults(void)    (cmsMdm_isDataModelDevice2(void) ? \
                                    getDslLoopDiagResults_dev2(void) : \
                                    getDslLoopDiagResults_igd(void))
#endif


#endif  /* DMP_DSLDIAGNOSTICS_1 */


#ifdef DMP_X_BROADCOM_COM_SELT_1
typedef struct
{
   UBOOL8 dslDiagInProgress;
   UINT32 testType;   /* SELT or whatever test */
   int pollRetries;
   CmsEntityId src;
   time_t testStartTime;
   time_t testEndTime;
} dslDiag;

void processWatchDslSeltDiag(CmsMsgHeader *msg);
void getDslSeltDiagResults(void);
#endif /* DMP_X_BROADCOM_COM_SELT_1*/



/** Push the various ACS related params that we received via DHCP into
 * the data model object.
 */
void setAcsParams(const char *acsURL, const char *provisioningCode,
                       UINT32 minWaitInterval, UINT32 intervalMultiplier);
void setAcsParams_igd(const char *acsURL, const char *provisioningCode,
                       UINT32 minWaitInterval, UINT32 intervalMultiplier);
void setAcsParams_dev2(const char *acsURL, const char *provisioningCode,
                       UINT32 minWaitInterval, UINT32 intervalMultiplier);

#if defined(SUPPORT_DM_LEGACY98)
#define setAcsParams(u, p, w, m)    setAcsParams_igd((u), (p), (w), (m))
#elif defined(SUPPORT_DM_HYBRID)
#define setAcsParams(u, p, w, m)    setAcsParams_igd((u), (p), (w), (m))
#elif defined(SUPPORT_DM_PURE181)
#define setAcsParams(u, p, w, m)    setAcsParams_dev2((u), (p), (w), (m))
#elif defined(SUPPORT_DM_DETECT)
#define setAcsParams(u, p, w, m)    (cmsMdm_isDataModelDevice2() ? \
                                    setAcsParams_dev2((u), (p), (w), (m)) : \
                                    setAcsParams_igd((u), (p), (w), (m)))
#endif




CmsRet getIpIntfByFullPath(const char *ipIntfFullPath,
                           InstanceIdStack *ipIntfIidStack,
                           Dev2IpInterfaceObject **ipIntfObj);

UBOOL8 getUpperLayerPathDescFromLowerLayerLocked(const char *lowerLayerFullPath,
                                      MdmObjectId upperLayerOid,
                                      MdmPathDescriptor *upperlayerPathDesc);


#ifdef BRCM_VOICE_SUPPORT

/* in ssk_voice.c */

void processVoiceShutdown(void);
void processVoiceStart(void);
void processVoiceRestart(void);
void processConfigUploadComplete(void);
void processMibReset(void);




CmsRet setVoiceBoundIp ( const char * ipAddr );

/** General purpose entry point for TR181 interface stack when a Layer 3
 *  service comes up.  Similar to initVoiceOnLanSideLocked and initVoiceOnWanIntfLocked
 *  on the TR98 side.
 */
void initVoiceOnIntfUpLocked_dev2(UINT32 ipvx, const char *intfName, UBOOL8 isWan);

void initVoiceOnLanSideLocked(const char *ifName);
void initVoiceOnWanIntfLocked(const char *ifName, const char *ipAddr);


/** Check if LAN side br0 is up.  If so, set the boundIpAddr
 *
 */
void initVoiceBoundIpIfLanSideUpLocked(void);
void initVoiceBoundIpIfLanSideUpLocked_igd(void);
void initVoiceBoundIpIfLanSideUpLocked_dev2(void);

#if defined(SUPPORT_DM_LEGACY98)
#define initVoiceBoundIpIfLanSideUpLocked()   initVoiceBoundIpIfLanSideUpLocked_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define initVoiceBoundIpIfLanSideUpLocked()   initVoiceBoundIpIfLanSideUpLocked_igd()
#elif defined(SUPPORT_DM_PURE181)
#define initVoiceBoundIpIfLanSideUpLocked()   initVoiceBoundIpIfLanSideUpLocked_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define initVoiceBoundIpIfLanSideUpLocked()   (cmsMdm_isDataModelDevice2() ? \
                             initVoiceBoundIpIfLanSideUpLocked_dev2() : \
                             initVoiceBoundIpIfLanSideUpLocked_igd())
#endif


/** Check if specified wan ifName is UP, and if so, set the boundIpAddr
 *
 * @param ifName (IN) wan side boundIfName, note: could be MDMVS_ANY_WAN
 *
 */
void initVoiceBoundIpIfWanSideUpLocked(const char *ifName);
void initVoiceBoundIpIfWanSideUpLocked_igd(const char *ifName);
void initVoiceBoundIpIfWanSideUpLocked_dev2(const char *ifName);

#if defined(SUPPORT_DM_LEGACY98)
#define initVoiceBoundIpIfWanSideUpLocked(i)   initVoiceBoundIpIfWanSideUpLocked_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define initVoiceBoundIpIfWanSideUpLocked(i)   initVoiceBoundIpIfWanSideUpLocked_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define initVoiceBoundIpIfWanSideUpLocked(i)   initVoiceBoundIpIfWanSideUpLocked_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define initVoiceBoundIpIfWanSideUpLocked(i)   (cmsMdm_isDataModelDevice2() ? \
                             initVoiceBoundIpIfWanSideUpLocked_dev2((i)) : \
                             initVoiceBoundIpIfWanSideUpLocked_igd((i)))
#endif


void initializeVoice(void);
void deInitializeVoice(void);
void defaultVoice(void);
#if DMP_X_BROADCOM_COM_EPON_1
void processEponMacBootInd();
#endif /* DMP_X_BROADCOM_COM_EPON_1 */

extern UBOOL8 isVoiceOnLanSide;
extern UBOOL8 isVoiceOnAnyWan;
extern char * voiceWanIfName;

#endif /* BRCM_VOICE_SUPPORT */

#ifdef DMP_STORAGESERVICE_1
/** Validates the recived message and on success adds MDMOID_PHYSICAL_MEDIUM
 * object
 *
 * @param msg (IN) Msg with details about physical storage device to be added 
 *
 * @return none
 */
void processAddPhysicalMediumMsg(CmsMsgHeader *msg);

/** Validates the recived message and on success removes a MDMOID_PHYSICAL_MEDIUM
 * object 
 *
 * @param msg (IN) Msg with details about physical storage device to be deleted 
 *
 * @return none
 */
void processRemovePhysicalMediumMsg(CmsMsgHeader *msg);

/** Validates the recived message and on success adds MDMOID_LOGICAL_VOLUME
 * object
 *
 * @param msg (IN) Msg with  physical device name and partition number to be added 
 *
 * @return none
 */
void processAddLogicalVolumeMsg(CmsMsgHeader *msg);

/** Validates the recived message and on success removes MDMOID_LOGICAL_VOLUME
 * object
 *
 * @param msg (IN) Msg with  physical device name and partition number to be deleted 
 *
 * @return none
 */
void processRemoveLogicalVolumeMsg(CmsMsgHeader *msg);

/** Initialize storage service.
 *
 * @return none
 */
void initStorageService(void);
#endif


#ifdef DMP_DEVICE2_USBHOSTSBASIC_1
void processUsbDeviceStateChange(CmsMsgHeader *msg);
#endif

/*
 * periodic functions, in ssk_periodic.c and ssk2_periodic.c
 */


/** Force update of the Uptime param in the MDM.
 *
 */
void sskPeriodic_updateUptime(void);
void sskPeriodic_updateUptime_igd(void);
void sskPeriodic_updateUptime_dev2(void);

#if defined(SUPPORT_DM_LEGACY98)
#define sskPeriodic_updateUptime()  sskPeriodic_updateUptime_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define sskPeriodic_updateUptime()  sskPeriodic_updateUptime_igd()
#elif defined(SUPPORT_DM_PURE181)
#define sskPeriodic_updateUptime()  sskPeriodic_updateUptime_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define sskPeriodic_updateUptime()  (cmsMdm_isDataModelDevice2() ? \
                                     sskPeriodic_updateUptime_dev2() : \
                                     sskPeriodic_updateUptime_igd())
#endif




/** Age and delete expired portmappings.
 *
 */
void sskPeriodic_expirePortMappings(void);
void sskPeriodic_expirePortMappings_igd(void);
void sskPeriodic_expirePortMappings_dev2(void);

#if defined(SUPPORT_DM_LEGACY98)
#define sskPeriodic_expirePortMappings()  sskPeriodic_expirePortMappings_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define sskPeriodic_expirePortMappings()  sskPeriodic_expirePortMappings_igd()
#elif defined(SUPPORT_DM_PURE181)
#define sskPeriodic_expirePortMappings()  sskPeriodic_expirePortMappings_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define sskPeriodic_expirePortMappings()  (cmsMdm_isDataModelDevice2() ? \
                                     sskPeriodic_expirePortMappings_dev2() : \
                                     sskPeriodic_expirePortMappings_igd())
#endif




/*
 * TR181 Interface Stack functions, in ssk2_intfstack.c
 */
void processIntfStackLowerLayersChangedMsg(const CmsMsgHeader *msg);
void processIntfStackObjectDeletedMsg(const CmsMsgHeader *msg);
void processIntfStackAliasChangedMsg(const CmsMsgHeader *msg);
void processIntfStackStaticAddressConfigdMsg(const CmsMsgHeader *msg);
void processIntfStackPropagateMsg(const CmsMsgHeader *msg);
void intfStack_propagateStatusByIidLocked(MdmObjectId oid, const InstanceIdStack *iidStack, const char *status);
void intfStack_propagateStatusByFullPathLocked(const char *lowerLayerFullPath, const char *status);
void intfStack_propagateStatusOnSingleEntryLocked(const Dev2InterfaceStackObject *intfStackObj, const char *status);
void intfStack_setStatusByFullPathLocked(const char *fullPath, const char *status);
void intfStack_setStatusByPathDescLocked(const MdmPathDescriptor *pathDescIn, const char *status);
void intfStack_updateIpIntfStatusLocked(const InstanceIdStack *ipIntfIidStack,
                                        const char *newIpv4ServiceStatus,
                                        const char *newIpv6ServiceStatus);

#ifdef DMP_DEVICE2_BONDEDDSL_1
void updateXtmLowerLayerLocked(void);
void updateAtmLowerLayerLocked(UBOOL8 trainBonded);
void updatePtmLowerLayerLocked(UBOOL8 trainBonded);
#endif

/*
 * TR181 Connection status functions (IPv4), in ssk2_connstatus.c
 */
void setIpv4ServiceStatusByFullPathLocked(const char *ipIntfFullPath, const char *serviceStatus);
void setIpv4ServiceStatusByIidLocked(const InstanceIdStack *ipIntfIidStack, const char *serviceStatus);
void getIpv4ServiceStatusByIidLocked(const InstanceIdStack *ipIntfIidStack,
                                     char *serviceStatus, UINT32 bufLen);


void sskConn_setPppConnStatusByFullPathLocked(const char *pppIntfFullPath, const char *connStatus);
void sskConn_setPppConnStatusByIidLocked(const InstanceIdStack *pppIntfIidStack, const char *connStatus);
void sskConn_setPppConnStatusByObjLocked(const InstanceIdStack *iidStack,
                      Dev2PppInterfaceObject *pppIntfObj,
                      const char *connStatus);

UBOOL8 sskConn_hasStaticIpv4AddressLocked(const InstanceIdStack *ipIntfIidStack);
UBOOL8 sskConn_hasAnyIpv4AddressLocked(const InstanceIdStack *ipIntfIidStack);

/*
 * TR181 Connection status functions (IPv6), in ssk2_connstatus6.c
 */
void setIpv6ServiceStatusByFullPathLocked(const char *ipIntfFullPath, const char *serviceStatus);
void setIpv6ServiceStatusByIidLocked(const InstanceIdStack *ipIntfIidStack, const char *serviceStatus);
void getIpv6ServiceStatusByIidLocked(const InstanceIdStack *ipIntfIidStack,
                                     char *serviceStatus, UINT32 bufLen);

void processPppv6StateUp_dev2(const InstanceIdStack *pppIntfIidStack, const char *localIntfId, const char *remoteIntfId);
void processPppv6StateDown_dev2(const InstanceIdStack *pppIntfIidStack);
UBOOL8 sskConn_hasStaticIpv6AddressLocked(const InstanceIdStack *ipIntfIidStack);
UBOOL8 sskConn_hasAnyIpv6AddressLocked(const InstanceIdStack *ipIntfIidStack);

void processPingStateChanged(CmsMsgHeader *msg);
void processPingStateChanged_igd(CmsMsgHeader *msg);
void processPingStateChanged_dev2(CmsMsgHeader *msg);
#if defined(SUPPORT_DM_LEGACY98)
#define processPingStateChanged(m)  processPingStateChanged_igd((m))
#elif defined(SUPPORT_DM_HYBRID)
#define processPingStateChanged(m)  processPingStateChanged_igd((m))
#elif defined(SUPPORT_DM_PURE181)
#define processPingStateChanged(m)  processPingStateChanged_dev2((m))
#elif defined(SUPPORT_DM_DETECT)
#define processPingStateChanged(m)  (cmsMdm_isDataModelDevice2() ? \
                                     processPingStateChanged_dev2((m)) : \
                                     processPingStateChanged_igd((m)))
#endif

#ifdef DMP_DEVICE2_TRACEROUTE_1
void processTracertStateChanged_dev2(CmsMsgHeader *msg);
#endif
#ifdef DMP_DEVICE2_WIFIACCESSPOINT_1
void processAssociatedDeviceUpdated_dev2(CmsMsgHeader *msg);
#endif

void processSampleSetStatusChanged(CmsMsgHeader *msg);
void processSampleSetStatusChanged_igd(CmsMsgHeader *msg);
void processSampleSetStatusChanged_dev2(CmsMsgHeader *msg);
#if defined(SUPPORT_DM_LEGACY98)
#define processSampleSetStatusChanged(m)  processSampleSetStatusChanged_igd((m))
#elif defined(SUPPORT_DM_HYBRID)
#define processSampleSetStatusChanged(m)  processSampleSetStatusChanged_igd((m))
#elif defined(SUPPORT_DM_PURE181)
#define processSampleSetStatusChanged(m)  processSampleSetStatusChanged_dev2((m))
#elif defined(SUPPORT_DM_DETECT)
#define processSampleSetStatusChanged(m)  (cmsMdm_isDataModelDevice2() ? \
                                     processSampleSetStatusChanged_dev2((m)) : \
                                     processSampleSetStatusChanged_igd((m)))
#endif

void processSpeedServiceComplete(CmsMsgHeader *msg);


void processXdslCfgSaveMessage(UINT32 msgId);
void processXdslCfgSaveMessage_dev2(UINT32 msgId);
void processXdslCfgSaveMessage_igd(UINT32 msgId);
#if defined(SUPPORT_DM_LEGACY98)
#define processXdslCfgSaveMessage(m)  processXdslCfgSaveMessage_igd(m)
#elif defined(SUPPORT_DM_HYBRID)
#define processXdslCfgSaveMessage(m)  processXdslCfgSaveMessage_igd(m)
#elif defined(SUPPORT_DM_PURE181)
#define processXdslCfgSaveMessage(m)  processXdslCfgSaveMessage_dev2(m)
#elif defined(SUPPORT_DM_DETECT)
#define processXdslCfgSaveMessage(m)  (cmsMdm_isDataModelDevice2() ? \
                                      processXdslCfgSaveMessage_dev2(m) : \
                                      processXdslCfgSaveMessage_igd(m))
#endif


void checkDslLinkStatusLocked_dev2(void);

#if defined(DMP_X_BROADCOM_COM_IEEE1905_1)
void processIeee1905GetCfgMsg(const CmsMsgHeader *msg);
void processIeee1905SetCfgMsg(const CmsMsgHeader *msg);
#endif


/** Persistent Scrach Pad (psp) entry name (key) which records which WAN type
 *  the RDPA was initialized with.  Supported contents (WAN types) are
 *  "GBE"
 *  "DSL"
 */
#define RDPA_WAN_TYPE_PSP_KEY     "RdpaWanType"
#define RDPA_WAN_OEMAC_PSP_KEY    "WanOEMac"


/** When DSL or EthWan link comes up, ssk will call this function with the
 *  wan type that came up.  This function will match against what RDPA was
 *  initialized with.  If there is a mismatch, ssk will set the correct type
 *  and reboot.
 */
void matchRdpaWanType(const char *wanTypeUp);

void processVendorConfigUpdate(CmsMsgHeader *msg);
void processVendorConfigUpdate_igd(CmsMsgHeader *msg);
void processVendorConfigUpdate_dev2(CmsMsgHeader *msg);

#if defined(SUPPORT_DM_LEGACY98)
#define processVendorConfigUpdate(m)   processVendorConfigUpdate_igd((m))
#elif defined(SUPPORT_DM_HYBRID)
#define processVendorConfigUpdate(m)   processVendorConfigUpdate_igd((m))
#elif defined(SUPPORT_DM_PURE181)
#define processVendorConfigUpdate(m)   processVendorConfigUpdate_dev2((m))
#elif defined(SUPPORT_DM_DETECT)
#define processVendorConfigUpdate(m)   (cmsMdm_isDataModelDevice2() ? \
                                        processVendorConfigUpdate_dev2((m)) : \
                                        processVendorConfigUpdate_igd((m)))
#endif


#ifdef SUPPORT_CELLULAR
void processCellularState6Changed(DevCellularDataCallStateChangedBody *cellularInfo);
#endif

#endif /* _SSK_H_ */
