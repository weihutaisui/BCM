/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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

#ifndef BCMCONFIG_H
#define BCMCONFIG_H
 
#include "cms.h"
#include "inc/appdefs.h"

CmsRet getLanIPAddressInfo(char **addr, char **subnetmask);
void clearModemConnectionURL(void);
UBOOL8 isTimeSynchronized(void);
void updateTr69cCfgInfo(void);
void saveLastConnectedURL(void);

/** Get params related to session retry
*
*/
CmsRet getSessionRetryParameters(int *waitInterval, int *multiplier);
CmsRet getSessionRetryParameters_igd(int *waitInterval, int *multiplier);
CmsRet getSessionRetryParameters_dev2(int *waitInterval, int *multiplier);


/** Set paramkey
*
*/
CmsRet setMgmtServerParameterKeyLocked(const char *paramKey);
CmsRet setMgmtServerParameterKeyLocked_igd(const char *paramKey);
CmsRet setMgmtServerParameterKeyLocked_dev2(const char *paramKey);

#ifndef BUILD_TR69C_BEEP
CmsRet getLanIPAddressInfo_igd(char **addr, char **subnetmask);
CmsRet getLanIPAddressInfo_dev2(char **addr, char **subnetmask);

#if defined(SUPPORT_DM_LEGACY98)
#define getLanIPAddressInfo(v, a)  getLanIPAddressInfo_igd((v), (a))
#elif defined(SUPPORT_DM_HYBRID)
#define getLanIPAddressInfo(v, a)  getLanIPAddressInfo_igd((v), (a))
#elif defined(SUPPORT_DM_PURE181)
#define getLanIPAddressInfo(v, a)  getLanIPAddressInfo_dev2((v), (a))
#elif defined(SUPPORT_DM_DETECT)
#define getLanIPAddressInfo(v, a)  (cmsMdm_isDataModelDevice2() ? \
                               getLanIPAddressInfo_dev2((v), (a)) : \
                               getLanIPAddressInfo_igd((v), (a)))
#endif


void clearModemConnectionURL_igd(void);
void clearModemConnectionURL_dev2(void);

#if defined(SUPPORT_DM_LEGACY98)
#define clearModemConnectionURL()  clearModemConnectionURL_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define clearModemConnectionURL()  clearModemConnectionURL_igd()
#elif defined(SUPPORT_DM_PURE181)
#define clearModemConnectionURL()  clearModemConnectionURL_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define clearModemConnectionURL()  (cmsMdm_isDataModelDevice2() ? \
                               clearModemConnectionURL_dev2() : \
                               clearModemConnectionURL_igd())
#endif


/**
*
*/
UBOOL8 isTimeSynchronized_igd(void);
UBOOL8 isTimeSynchronized_dev2(void);

#if defined(SUPPORT_DM_LEGACY98)
#define isTimeSynchronized()  isTimeSynchronized_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define isTimeSynchronized()  isTimeSynchronized_igd()
#elif defined(SUPPORT_DM_PURE181)
#define isTimeSynchronized()  isTimeSynchronized_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define isTimeSynchronized()  (cmsMdm_isDataModelDevice2() ? \
                               isTimeSynchronized_dev2() : \
                               isTimeSynchronized_igd())
#endif


/**
*
*/
void setDefaultActiveNotificationLocked(PhlSetParamAttr_t **pSetParmAttrList, int *numEntries);
void setDefaultActiveNotificationLocked_igd(PhlSetParamAttr_t **pSetParmAttrList, int *numEntries);
void setDefaultActiveNotificationLocked_dev2(PhlSetParamAttr_t **pSetParmAttrList, int *numEntries);

#if defined(SUPPORT_DM_LEGACY98)
#define setDefaultActiveNotificationLocked(v, a)  setDefaultActiveNotificationLocked_igd((v), (a))
#elif defined(SUPPORT_DM_HYBRID)
#define setDefaultActiveNotificationLocked(v, a)  setDefaultActiveNotificationLocked_igd((v), (a))
#elif defined(SUPPORT_DM_PURE181)
#define setDefaultActiveNotificationLocked(v, a)  setDefaultActiveNotificationLocked_dev2((v), (a))
#elif defined(SUPPORT_DM_DETECT)
#define setDefaultActiveNotificationLocked(v, a)  (cmsMdm_isDataModelDevice2() ? \
                               setDefaultActiveNotificationLocked_dev2((v), (a)) : \
                               setDefaultActiveNotificationLocked_igd((v), (a)))
#endif


/**
*
*/
UBOOL8 checkWanDslDiagnosticObjID(MdmPathDescriptor *pathDesc);
UBOOL8 checkWanDslDiagnosticObjID_igd(MdmPathDescriptor *pathDesc);
UBOOL8 checkWanDslDiagnosticObjID_dev2(MdmPathDescriptor *pathDesc);

#if defined(SUPPORT_DM_LEGACY98)
#define checkWanDslDiagnosticObjID(a)  checkWanDslDiagnosticObjID_igd((a))
#elif defined(SUPPORT_DM_HYBRID)
#define checkWanDslDiagnosticObjID(a)  checkWanDslDiagnosticObjID_igd((a))
#elif defined(SUPPORT_DM_PURE181)
#define checkWanDslDiagnosticObjID(a)  checkWanDslDiagnosticObjID_dev2((a))
#elif defined(SUPPORT_DM_DETECT)
#define checkWanDslDiagnosticObjID(a)  (cmsMdm_isDataModelDevice2() ? \
                               checkWanDslDiagnosticObjID_dev2((a)) : \
                               checkWanDslDiagnosticObjID_igd((a)))
#endif


/**
*
*/
void updateTr69cCfgInfo_igd(void);
void updateTr69cCfgInfo_dev2(void);

#if defined(SUPPORT_DM_LEGACY98)
#define updateTr69cCfgInfo()  updateTr69cCfgInfo_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define updateTr69cCfgInfo()  updateTr69cCfgInfo_igd()
#elif defined(SUPPORT_DM_PURE181)
#define updateTr69cCfgInfo()  updateTr69cCfgInfo_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define updateTr69cCfgInfo()  (cmsMdm_isDataModelDevice2() ? \
                               updateTr69cCfgInfo_dev2() : \
                               updateTr69cCfgInfo_igd())
#endif



/**
*
*/
void saveLastConnectedURL_igd(void);
void saveLastConnectedURL_dev2(void);


#endif // not BUILD_TR69C_BEEP

#endif /* BCMCONFIG_H */
