/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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

#ifndef BCMWRAPPER_H
#define BCMWRAPPER_H
 
#include "cms.h"
#include "cms_core.h"
#include "cms_util.h"
#include "inc/tr69cdefs.h"


typedef struct gwStateData
{
   char     rebootCommandKey[33];
   char     downloadCommandKey[33];
   char     newParameterKey_OBSOLETE[33];  /* saved in MDM, do not use this field */
   int      contactedState;
   int      dlFaultStatus;
   int      retryCount;
   time_t   startDLTime;
   time_t   endDLTime;
   char     dlFaultMsg[129];
   int      informEvCount;
   unsigned char informEvList[64];
   int      dataModel;
   int      cwmpVersion;
   UBOOL8   doSendAutonTransferComplete;
   char     padforexpansion[315];             
} GWStateData, *PGWStateData;


/* general system status and configruation retrieval */
/* Returns state of WAN interface to be used by tr69 client */
typedef enum
{
   eWAN_INACTIVE,
   eWAN_ACTIVE
} eWanState;

eWanState getWanState(void);
eWanState getRealWanState(const char *ifName);

const char *getInformEvtString(UINT32 evt);
void saveTR69StatusItems(void);
void retrieveTR69StatusItems(void);
void wrapperSaveConfigurations(void);
void wrapperReboot(eInformState rebootContactValue);
UBOOL8 wrapperFactoryReset(void);
void wrapperReset(void);
CmsRet downloadComplete(DownloadReq *r, char *buf);
void setInformState(eInformState state);
CmsRet tr69RetrieveTransferListFromStore(DownloadReqInfo *list, UINT16 *size);
CmsRet tr69SaveTransferList(void);
UBOOL8 matchAddrOnLanSide(const char *urlAddr);

/*************** BEGIN FUNCTIONS MUST ALREADY HAVE MDM LOCK ********************/


/** Return the full MDM path to the specified IP addr.
 *
 * @param ipvx  (IN) If CMS_AF_SELECT_IPV4, return path to IPv4 addr.
 *                   If CMS_AF_SELECT_IPV6, return path to IPv6 addr.
 *                   If CMS_AF_SELECT_IPVX, try to return path to IPv6 addr
 *                   first, but if not found, then try to return path to
 *                   IPv6 addr.
 * @param ipAddr (IN) The IP address to search for.
 * 
 * @return the MDM fullPath to the ipAddr, or NULL if the ipAddr was not
 *         found.  Caller is responsible for freeing the fullPath buffer
 *         using cmsMem_free.
 */
char *getFullPathToIpvxAddrLocked(UINT32 ipvx, const char *ipAddr);
#ifndef BUILD_TR69C_BEEP
char *getFullPathToIpvxAddrLocked_igd(UINT32 ipvx, const char *ipAddr);
char *getFullPathToIpvxAddrLocked_dev2(UINT32 ipvx, const char *ipAddr);

#if defined(SUPPORT_DM_LEGACY98)
#define getFullPathToIpvxAddrLocked(v, a)  getFullPathToIpvxAddrLocked_igd((v), (a))
#elif defined(SUPPORT_DM_HYBRID)
#define getFullPathToIpvxAddrLocked(v, a)  getFullPathToIpvxAddrLocked_igd((v), (a))
#elif defined(SUPPORT_DM_PURE181)
#define getFullPathToIpvxAddrLocked(v, a)  getFullPathToIpvxAddrLocked_dev2((v), (a))
#elif defined(SUPPORT_DM_DETECT)
#define getFullPathToIpvxAddrLocked(v, a)  (cmsMdm_isDataModelDevice2() ? \
                               getFullPathToIpvxAddrLocked_dev2((v), (a)) : \
                               getFullPathToIpvxAddrLocked_igd((v), (a)))
#endif

#endif // not BUILD_TR69C_BEEP



// updateCredentialsInfo should be called from function that
// is already get MDM lock such as doSetParameterValues
void updateCredentialsInfo(void);
CmsRet setMSrvrURL(const char *value);
CmsRet setMSrvrInformInterval(UINT32 interval);
CmsRet setMSrvrInformEnable(UBOOL8 enable);
CmsRet setMSrvrBoundIfName(const char *boundIfName);

/*************** END FUNCTIONS MUST ALREADY HAVE MDM LOCK ********************/

CmsRet tr69SaveConfigFileInfo(DownloadReq *r);
void setVendorConfigObj(DownloadVendorConfigInfo *vendorConfig);
void retrieveClearTR69VendorConfigInfo(void);

/* binary helper function */
int get_max_image_buf_size(void);

void setDefaultActiveNotification(void);



#ifdef DMP_DSLDIAGNOSTICS_1
void getDslLoopDiagResultsAndLinkUp(void);
CmsRet dslLoopDiag_getParamValue(char *paramName, char **paramValue);
void freeDslLoopDiagResults(void);
#endif /* DMP_DSLDIAGNOSTICS_1 */

int getAutonXferCompletePolicyLocked(UBOOL8 *enable, char *fileTypeFilter, char *resultTypeFilter, char *transferTypeFilter);
int getDuStateChangePolicyLocked(UBOOL8 *enable, char *opTypeFilter, char *resultTypeFilter, char *faultCodeFilter);

#endif /*BCMWRAPPER_H*/
