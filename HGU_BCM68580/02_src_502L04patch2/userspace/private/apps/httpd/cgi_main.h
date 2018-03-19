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

#ifndef __CGI_MAIN_H__
#define __CGI_MAIN_H__

#include <stdio.h>
#include <fcntl.h>
#include <netinet/in.h>

#include "os_defs.h"
#include "cms.h"
#include "cms_dal.h"  /* defines WEB_NTWK_VAR */


/********************** Global Types ****************************************/

#define WEB_BUF_SIZE_MAX      6000
#define WEB_BIG_BUF_SIZE_MAX  13000
#define WEB_BIG_BUF_SIZE_MAX_WLAN (13000 * 2)

#define WEB_DIAG_TYPE        0
#define WEB_DIAG_PREV        1
#define WEB_DIAG_CURR        2
#define WEB_DIAG_NEXT        3
#define WEB_DIAG_MAX         4

#define ADSL_BERT_STATE_STOP 0
#define ADSL_BERT_STATE_RUN 1

#ifdef SUPPORT_QUICKSETUP
#define QS_ATM_ADD_ERROR 10 
#define QS_PPP_ADD_ERROR 20 
#define QS_PPP_AUTH_ERROR 21 
#define QS_WLAN_ADD_ERROR 30 
#endif

typedef struct {
   int berState;
   unsigned long berTime;
} WEB_TEST_VAR, *PWEB_TEST_VAR;

typedef void (*CGI_GET_HDLR) (int argc, char **argv, char *varValue);

typedef struct {
   char *cgiGetName;
   CGI_GET_HDLR cgiGetHdlr;
} CGI_GET_VAR, *PCGI_GET_VAR;

typedef void (*CGI_FNC_HDLR) (void);

typedef struct {
   char *cgiFncName;
   CGI_FNC_HDLR cgiFncHdlr;
} CGI_FNC_CMD, *PCGI_FNC_CMD;


typedef enum {
   CGI_STS_OK = 0,
   CGI_STS_ERR_GENERAL,
   CGI_STS_ERR_MEMORY,
   CGI_STS_ERR_FIND
} CGI_STATUS;

typedef enum {
   CGI_TYPE_NONE = 0,
   CGI_TYPE_STR,
   CGI_TYPE_MARK_STR,
   CGI_TYPE_NUM,
   CGI_TYPE_SYS_VERSION,
   CGI_TYPE_CFE_VERSION,
   CGI_TYPE_BOARD_ID,
   CGI_TYPE_ADSL_FLAG,
   CGI_TYPE_EXIST_PROTOCOL,
   CGI_TYPE_NUM_ENET,
   CGI_TYPE_NUM_PVC,
   CGI_TYPE_DHCP_LEASES,
   CGI_TYPE_PORT,
   CGI_TYPE_VPI,
   CGI_TYPE_VCI,
   CGI_TYPE_IPSEC_TABLE,
   CGI_TYPE_IPSEC_SETTINGS,
   CGI_TYPE_CERT_LIST,
   CGI_TYPE_ENET_DIAG,
   CGI_TYPE_VDSL_VERSION,
   CGI_TYPE_BOOL,
   CGI_TYPE_SHORT,
   CGI_TYPE_GROUP_LIST,
   CGI_TYPE_MOCA_LIST,
} CGI_TYPE;


void do_cgi(char *path, FILE *fs);
void cgiFncCmd(int argc, char **argv);
void cgiGetVarOther(int argc, char **argv, char *varValue);
void cgiGetVar(char *varName, char *varValue, FILE *fs __attribute__((unused)));
void cgiSetVar(char *varName, char *varValue);
void cgiGetWanInfo(void);
void cgiReboot(void);
void cgiUrlDecode(char *s);
void cgiParseSet(char *path);

#define cgiGetValueByName(query, id, val)    cgiGetValueByNameSafe(query, id, NULL, val, sizeof(val))
/** Get parameter value by identification from httpd input.
 *
 * Usage:
 * 1. Similar to the old cgiGetValueByName(): (val != NULL && buf == NULL && len > 0)
 * Caller pre-allocates a buffer and passes the buffer the this function. 
 * cgiGetValueByNameSafe() gets the parameter value and stores it into the buffer.
 * If the allocated buffer size is not sufficient for the parameter value, error message is printed.
 *
 * 2. New Design with safety: (buf != NULL && val == NULL && len == 0)
 * Caller just passes a pointer by reference to this function.
 * cgiGetValueByNameSafe() allocates correct size for the caller and the caller NEEDS to FREE it.
 *
 * @param query     (IN)  httpd input string
 * @param id        (IN)  identification of the parameter
 * @param buf       (IN/OUT) pointer to the buffer (allocated by this function) with the parameter value. Caller NEEDS TO FREE this buffer.
 * @param val       (IN/OUT) pre-allocated buffer (by caller) to store the parameter value
 * @param len       (IN)  pre-allocated buffer length
 *
 * @return CGI_STATUS enum.
 */
CGI_STATUS cgiGetValueByNameSafe(const char *query, const char *id, char **buf, char *val, int len);

void cgiWriteMessagePage(FILE *fs, char *msg, char *title, char *location);

void do_test_cgi(char *path, FILE *fs);
void cgiGetTestVar(char *varName, char *varValue);
void cgiGetTestVar_igd(char *varName, char *varValue);
void cgiGetTestVar_dev2(char *varName, char *varValue);
#if defined(SUPPORT_DM_LEGACY98)
#define cgiGetTestVar(a,b)   cgiGetTestVar_igd((a),(b))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiGetTestVar(a,b)   cgiGetTestVar_igd((a),(b))
#elif defined(SUPPORT_DM_PURE181)
#define cgiGetTestVar(a,b)   cgiGetTestVar_dev2((a),(b))
#elif defined(SUPPORT_DM_DETECT)
#define cgiGetTestVar(a,b)   (cmsMdm_isDataModelDevice2() ?    \
                              cgiGetTestVar_dev2((a),(b)) :    \
                              cgiGetTestVar_igd((a),(b)))

#endif

void cgiSetTestVar(char *varName, char *varValue);
void cgiSetTestVar_igd(char *varName, char *varValue);
void cgiSetTestVar_dev2(char *varName, char *varValue);
#if defined(SUPPORT_DM_LEGACY98)
#define cgiSetTestVar(a,b)   cgiSetTestVar_igd((a),(b))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiSetTestVar(a,b)   cgiSetTestVar_igd((a),(b))
#elif defined(SUPPORT_DM_PURE181)
#define cgiSetTestVar(a,b)   cgiSetTestVar_dev2((a),(b))
#elif defined(SUPPORT_DM_DETECT)
#define cgiSetTestVar(a,b)   (cmsMdm_isDataModelDevice2() ?    \
                              cgiSetTestVar_dev2((a),(b)) :    \
                              cgiSetTestVar_igd((a),(b)))

#endif

void cgiRunAdslBerTest(void);
void cgiTestParseSet(char *path);
void cgiGetSysInfo(int argc, char **argv, char *varValue);
void cgiGetMemInfo(int argc, char **argv, char *varValue);
void cgiGetWanInterfaceInfo(int argc, char **argv, char *varValue);
void cgiGetTr69cInterfaceList(int argc, char **argv, char *varValue);


/** List all the LAN interfaces that are available for adding to a new bridge
 */
void cgiGetBridgeInterfaceInfo(int argc, char **argv, char *varValue);
void cgiGetBridgeInterfaceInfo_igd(char **argv, char *varValue);
void cgiGetBridgeInterfaceInfo_dev2(char **argv, char *varValue);


void cgiGetPMapGroupName(int argc, char **argv, char *varValue);
void cgiGetConnectionType(int argc, char **argv, char *varValue);
void cgiGetVpiVci(int argc, char **argv, char *varValue);
void cgiGetLineRate(int argc, char **argv, char *varValue);
void cgiGetDiagnostic(int argc, char **argv, char *varValue);
void cgiGetOamF5Diagnostic(int argc, char **argv, char *varValue);
void cgiGetOamF4Diagnostic(int argc, char **argv, char *varValue);
void cgiGetDmzHost(int argc, char **argv, char *varValue);


/** Get the DHCP vendorId's assigned to the current intf group.
 *
 */
void cgiGetDhcpVendorId(int argc, char **argv, char *varValue);
void cgiGetDhcpVendorId_igd(char *varValue);
void cgiGetDhcpVendorId_dev2(char *varValue);


void cgiGetDiagInfo(int argc, char **argv, char *varValue);
void cgiDmGetDiagInfo(char **argv, char *varValue);
void cgiDmGetDiagInfo_igd(char **argv, char *varValue);
void cgiDmGetDiagInfo_dev2(char **argv, char *varValue);
#if defined(SUPPORT_DM_LEGACY98)
#define cgiDmGetDiagInfo(a,b)   cgiDmGetDiagInfo_igd((a),(b))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiDmGetDiagInfo(a,b)   cgiDmGetDiagInfo_igd((a),(b))
#elif defined(SUPPORT_DM_PURE181)
#define cgiDmGetDiagInfo(a,b)   cgiDmGetDiagInfo_dev2((a),(b))
#elif defined(SUPPORT_DM_DETECT)
#define cgiDmGetDiagInfo(a,b)   (cmsMdm_isDataModelDevice2() ?    \
                                 cgiDmGetDiagInfo_dev2((a),(b)) : \
                                 cgiDmGetDiagInfo_igd((a),(b)))

#endif

/* in cgi_sts.c and cgi_sts_xyz.c */
/* see also cgi_sts.h */
void cgiResetStsIfc(void);
void cgiGetStsIfc(int argc, char **argv, char *varValue);
void cgiResetStsWan(void);

#if defined(DMP_X_BROADCOM_COM_RDPA_1)
/* wan / lan port per queue stats*/
void cgiGetStsQueue(int argc, char **argv, char *varValue);
void cgiResetStsQueue(char *query, FILE *fs);
#endif

/* moca stats reset functions */
void cgiResetStsMocaLan(void);
void cgiResetStsMocaLan_igd(void);
void cgiResetStsMocaLan_dev2(void);

void cgiResetStsMocaWan(void);
void cgiResetStsMocaWan_igd(void);
void cgiResetStsMocaWan_dev2(void);

/* Reset ITU (GPON) Moca stats */
void cgiResetStsMoca(void);




#ifdef SUPPORT_DSL
void cgiGetVdslLineRate(int argc, char **argv, char *varValue);
void cgiIsVdslSupported (int argc, char **argv, char *varValue);
void cgiGetTrafficType(int argc, char **argv, char *varValue);

void cgiUpdateStsAtm(void);
void cgiUpdateStsAdsl(void);
void cgiResetStsAdsl(void);
void cgiResetStsVdsl(void);
void cgiUpdateStsVdsl(void);
void cgiGetStsVdsl(int argc, char **argv, char *varValue);
void cgiGetStsAdsl(int argc, char **argv, char *varValue);
void cgiGetStatsAdsl(char *varValue);
void cgiGetStatsAdsl_igd(char *varValue);
void cgiGetStatsAdsl_dev2(char *varValue);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiGetStatsAdsl(p)   cgiGetStatsAdsl_igd((p))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiGetStatsAdsl(p)   cgiGetStatsAdsl_igd((p))
#elif defined(SUPPORT_DM_PURE181)
#define cgiGetStatsAdsl(p)   cgiGetStatsAdsl_dev2((p))
#elif defined(SUPPORT_DM_DETECT)
#define cgiGetStatsAdsl(p)   (cmsMdm_isDataModelDevice2() ? \
                              cgiGetStatsAdsl_dev2((p)) :   \
                              cgiGetStatsAdsl_igd((p)))
#endif

void cgiResetStatsXdsl_igd();
void cgiResetStatsXdsl_dev2();
#if defined(SUPPORT_DM_LEGACY98)
#define cgiResetStatsXdsl()   cgiResetStatsXdsl_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define cgiResetStatsXdsl()   cgiResetStatsXdsl_igd()
#elif defined(SUPPORT_DM_PURE181)
#define cgiResetStatsXdsl()   cgiResetStatsXdsl_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define cgiResetStatsXdsl()   (cmsMdm_isDataModelDevice2() ? \
                              cgiResetStatsXdsl_dev2() :   \
                              cgiResetStatsXdsl_igd())
#endif

void cgiResetStsXtm(void);
void cgiResetStatsXtm(void);
void cgiResetStatsXtm_igd(void);
void cgiResetStatsXtm_dev2(void);
#if defined(SUPPORT_DM_LEGACY98)
#define cgiResetStatsXtm()   cgiResetStatsXtm_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define cgiResetStatsXtm()   cgiResetStatsXtm_igd()
#elif defined(SUPPORT_DM_PURE181)
#define cgiResetStatsXtm()   cgiResetStatsXtm_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define cgiResetStatsXtm()   (cmsMdm_isDataModelDevice2() ? \
                               cgiResetStatsXtm_dev2() :  \
                               cgiResetStatsXtm_igd())

#endif


#endif /*SUPPORT_DSL */

void cgiGetCurrSessionKey(int argc, char **argv, char *varValue);
CmsRet cgiValidateSessionKey(FILE *fs);

CmsRet cgiConfigSysLog(void);

CmsRet cgiConfigSnmp(void);
#ifdef DMP_X_BROADCOM_COM_DBUSREMOTE_1
CmsRet cgiConfigDbusremote(void);
#endif

CmsRet cgiConfigPassword(void);
void cgiConfigSiproxd(void);
void cgiConfigPortMap(void);
void cgiSetPppUserNameAndPassword(void);

#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1) 
CmsRet cgiConfigNtp(void);
#endif

#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1 /* aka SUPPORT_DNSPROXY */
void cgiGetDnsProxy(int argc, char **argv, char *varValue);
#endif

#ifdef SUPPORT_TR69C
CmsRet cgiTr69cConfig(void);
void cgiTr69cGetRpcMethods(void);
#endif

#ifdef DMP_X_ITU_ORG_GPON_1 /* aka SUPPORT_OMCI */
CmsRet cgiOmciSystem(void);
#endif

void cgiGetVdslType(int argc, char **argv, char *varValue);


void cgiGetAvailableL2EthIntf(int argc, char **argv, char *varValue);
void cgiGetGMACEthIntf(int argc, char **argv, char *varValue);
void cgiGetWanOnlyEthIntf(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue) ;

/** Helper function to display configured ethWan interfaces.
 *
 */
UBOOL8 cgiEthWanCfgViewBody(FILE *fs);
UBOOL8 cgiEthWanCfgViewBody_igd(FILE *fs);
UBOOL8 cgiEthWanCfgViewBody_dev2(FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiEthWanCfgViewBody(p)   cgiEthWanCfgViewBody_igd((p))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiEthWanCfgViewBody(p)   cgiEthWanCfgViewBody_igd((p))
#elif defined(SUPPORT_DM_PURE181)
#define cgiEthWanCfgViewBody(p)   cgiEthWanCfgViewBody_dev2((p))
#elif defined(SUPPORT_DM_DETECT)
#define cgiEthWanCfgViewBody(p)   (cmsMdm_isDataModelDevice2() ? \
                              cgiEthWanCfgViewBody_dev2((p)) :   \
                              cgiEthWanCfgViewBody_igd((p)))
#endif




#if defined(DMP_X_BROADCOM_COM_MOCAWAN_1) || defined(DMP_DEVICE2_MOCA_1)
void cgiGetAvailableL2MocaIntf(int argc, char **argv, char *varValue);
#endif

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
void cgiGetAvailableL2GponIntf(int argc, char **argv, char *varValue);
#endif

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
void cgiGetAvailableL2EponIntf(int argc, char **argv, char *varValue);
#endif




/** Helper function used by cgiWanSrvc
 *
 */
UBOOL8 cgiWan_getInfoFromWanL2IfName(const char *wanL2IfName,
                                     WanIfcType *iftype, SINT32 *connMode);
UBOOL8 cgiWan_getInfoFromWanL2IfName_igd(const char *wanL2IfName,
                                     WanIfcType *iftype, SINT32 *connMode);
UBOOL8 cgiWan_getInfoFromWanL2IfName_dev2(const char *wanL2IfName,
                                     WanIfcType *iftype, SINT32 *connMode);
#if defined(SUPPORT_DM_LEGACY98)
#define cgiWan_getInfoFromWanL2IfName(i, t, c)   cgiWan_getInfoFromWanL2IfName_igd((i), (t), (c))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiWan_getInfoFromWanL2IfName(i, t, c)   cgiWan_getInfoFromWanL2IfName_igd((i), (t), (c))
#elif defined(SUPPORT_DM_PURE181)
#define cgiWan_getInfoFromWanL2IfName(i, t, c)   cgiWan_getInfoFromWanL2IfName_dev2((i), (t), (c))
#elif defined(SUPPORT_DM_DETECT)
#define cgiWan_getInfoFromWanL2IfName(i, t, c)   (cmsMdm_isDataModelDevice2() ? \
                          cgiWan_getInfoFromWanL2IfName_dev2((i), (t), (c)) : \
                          cgiWan_getInfoFromWanL2IfName_igd((i), (t), (c)))
#endif




/** Get a list of Moca interface names (both WAN and LAN).
 *
 */
void cgiPrintMocaList(char *varValue);





#ifdef DMP_X_BROADCOM_COM_PWRMNGT_1 
void cgiGetPowerManagement(int argc, char **argv, char *varValue);
#endif /* aka SUPPORT_PWRMNGT */

#ifdef SUPPORT_LANVLAN
void cgiGetAllL2EthIntf(int argc, char **argv, char *varValue); 
#endif

#ifdef DMP_DEVICE2_OPTICAL_1

#ifdef DMP_X_ITU_ORG_GPON_1
void cgiResetStsOptical(void);
void cgiGetStsOptical(int argc, char **argv, char *varValue);
#endif    // DMP_X_ITU_ORG_GPON_1

#endif    // DMP_DEVICE2_OPTICAL_1

void cgiGetAvailableL2WlIntf(int argc, char **argv, char *varValue);

#if defined(DMP_X_BROADCOM_COM_SPDSVC_1)
void cgiGetSpdsvcInfo(int argc, char **argv, char *varValue);
#endif

#ifdef DMP_X_BROADCOM_COM_CONTAINER_1
void cgiGetContainerTable(int argc, char **argv, char *varValue);
void cgiGetContainerDetail(int argc, char **argv, char *varValue);
#endif  /* DMP_X_BROADCOM_COM_CONTAINER_1 */

// API return status
typedef enum {
    BcmCfm_Ok = 0,
    BcmCfm_Fail
} BcmCfm_Status;
extern WEB_NTWK_VAR glbWebVar; // this global var is defined in cgimain.c
extern int glbRefresh;  // used for refresh service control web page when ftp or tftp is enabled
extern int glbDiagInfoClear; // use to indicate clean glbDiagInfo or not.
extern char glbDiagInfo[WEB_DIAG_MAX][BUFLEN_64]; // use to transfer diagnostics info between WEB client & server
extern int glbExistedProtocol;   // used for validate pvc configuration 
extern int glbOldPort; // it is defined in cgimain.c   
extern int glbOldVpi; // it is defined in cgimain.c
extern int glbOldVci; // it is defined in cgimain.c
extern char glbErrMsg[]; // global glbErrMsg is declared in cgimain.c
extern int glbEntryCt;  // used for web page (port trigger) to know how many entries can be added
extern int glbQosPrioLevel;

extern SINT32 glbCurrSessionKey;  // The current valid session key
extern CmsTimestamp glbSessionKeyIssuedTms;  // The timestamp when the current session key was created.

#ifdef SUPPORT_MODSW_WEBUI
extern void cgiModSw_handleResponse(CmsMsgHeader *msgRes);
extern void cgiModSw_handleResponseEE(CmsMsgHeader *msgRes);
#endif

#ifdef SUPPORT_HOMEPLUG
void cgiHomePlugAssoc(int argc, char **argv, char *varValue);
CmsRet cgiConfigHomePlugPassword(void);
CmsRet cgiConfigHomePlug(void);
#endif

#if defined(SUPPORT_IEEE1905) && defined(DMP_DEVICE2_IEEE1905BASELINE_1)
CmsRet cgiDoIeee1905Config(void);
#endif

extern void cgiGetSeltVar(char *varName, char *varValue);
void cgiSeltParseSet(char *path, FILE *fs);
CmsRet getSeltResultAndLinkUp(void);
void cgiPrintSeltResult(char *query __attribute__((unused)),FILE *fs);
int calcLenOfMappingTxtToHtml(char* txtStr);
int mappingTxtToHtml(char* txtStr, char* htmlStr);


#if defined(DMP_X_BROADCOM_COM_SPDSVC_1)
void cgiSpeedTestCmd(char *path, FILE *fs);
#endif

#if defined(DMP_X_BROADCOM_COM_NFC_1)
void cgiGetNfc(int argc, char **argv, char *varValue);
#endif

#if defined(DMP_X_BROADCOM_COM_OPENVSWITCH_1)
void cgiGetOpenVSCfg(int argc, char **argv, char *varValue);
#endif
//TODO extern int glbPvcCreation;    /* flag for in pvc creation or not */

//~ temp stuff from dbapi.h

#define DB_WAN_ADD_OK                     0
#define DB_WAN_ADD_OUT_OF_PVC             1
#define DB_WAN_ADD_OUT_OF_MEMORY          2
#define DB_WAN_NOT_FOUND                  3
#define DB_WAN_ADD_INVALID_QOS            4
#define DB_WAN_ADD_OUT_OF_QUEUES          5
#define DB_WAN_ADD_OUT_OF_CONNECTION      6
#define DB_WAN_ADD_EXISTED_PVC            7
#define DB_WAN_ADD_PPPOE_ONLY             8
#define DB_WAN_ADD_PPPOE_OR_MER_ONLY      9
#define DB_WAN_REMOVE_OK                  10

#define DB_WAN_GET_OK                     1
#define DB_WAN_GET_NOT_FOUND              2
#define DB_VCC_GET_OK                     3
#define DB_VCC_GET_NOT_FOUND              4

#define DB_GET_OK                         5
#define DB_GET_NOT_FOUND                  6

#define WAN_PROTOCOL_FIELD                1
#define WAN_ADMIN_FIELD                   2
#define WAN_BRIDGE_FIELD                  3
#define WAN_MAC_LEARNING_FIELD            4
#define WAN_IGMP_FIELD                    5
#define WAN_NAT_FIELD                     6
#define WAN_DHCPC_FIELD                   7

//~ temp end


//~ temp begin .. from objects.h

// wan statistic fields
#define STS_FROM_LAN                  0
#define STS_FROM_WAN                  1
#define OBJ_STS_IFC_COLS              8
#define OBJ_STS_RX_BYTE               0
#define OBJ_STS_RX_PACKET             1
#define OBJ_STS_RX_ERR                2
#define OBJ_STS_RX_DROP               3
#define OBJ_STS_TX_BYTE               4
#define OBJ_STS_TX_PACKET             5
#define OBJ_STS_TX_ERR                6
#define OBJ_STS_TX_DROP               7
#define OBJ_STS_ALL                   8

//~ temp end


#define WEB_SPACE               "&nbsp"

#define IFC_STATUS_UNKNOWN          0
#define IFC_STATUS_NEW              1
#define IFC_STATUS_AUTH_ERR         2
#define IFC_STATUS_DOWN             3
#define IFC_STATUS_DHCPC_PROGRESS   4
#define IFC_STATUS_DHCPC_FAIL       5
#define IFC_STATUS_UP               6

#define ATM_VCC_APPID 			  "AtmCfgVcc"
#define ATM_TD_APPID 			  "AtmCfgTd"
#define ATM_CFG_APPID 			  "AtmCfg"
#define PPP_APPID 			     "PppInfo"
#define DHCP_CLT_APPID		     "DhcpClntInfo"

#define LAN_APPID 			     "Lan"
#define IFC_NAME_LEN		        16
#define IFC_IP_ADDR_SIZE   	  16
#define IFC_LAN_MAX        	  4
#define IFC_UNKNOWN_ID     	  0
#define IFC_ENET_ID	          1
#define IFC_ENET1_ID            (IFC_ENET_ID+1)
#define IFC_USB_ID		        (IFC_ENET_ID + IFC_LAN_MAX)
#define IFC_HPNA_ID		        (IFC_USB_ID + IFC_LAN_MAX)
#define IFC_WIRELESS_ID		    (IFC_HPNA_ID + IFC_LAN_MAX)
#define IFC_SUBNET_ID           (IFC_WIRELESS_ID + IFC_LAN_MAX) // for sencond LAN subnet
#define IFC_ENET0_VNET_ID       (IFC_SUBNET_ID + IFC_LAN_MAX)
#define IFC_ENET1_VNET_ID       (IFC_ENET0_VNET_ID + IFC_LAN_MAX)

#define WAN_APPID 			     "Wan"

#define BR2684_APPID 			  "BR2684"
#define IFC_BR2684_ID		     16
#define IFC_BRIDGE_NAME			  "br0"

#define MER2684_APPID 			  "MER2684"
#define IFC_RFC2684_MAX    	  8
#define IFC_MER2684_ID		     (IFC_BR2684_ID + IFC_RFC2684_MAX)

#define PPPOE_APPID 			     "PPPOE"
#define IFC_PPPOE2684_ID   	  (IFC_MER2684_ID + IFC_RFC2684_MAX)

#define PPPOA_APPID 			     "PPPOA"
#define IFC_PPPOA2684_ID   	  (IFC_PPPOE2684_ID + IFC_RFC2684_MAX)

#define IPOA_APPID 			     "IPOA"
#define IFC_IPOA2684_ID   	     (IFC_PPPOA2684_ID + IFC_RFC2684_MAX)

#define PPP_AUTH_METHOD_AUTO    0
#define PPP_AUTH_METHOD_PAP     1
#define PPP_AUTH_METHOD_CHAP    2
#define PPP_AUTH_METHOD_MSCHAP  3

#define ATM_AUX_CHANNEL_VPI     1
#define ATM_AUX_CHANNEL_VCI     39

#define INSTID_OBJID            9999
#define INSTID_VALID_VALUE      0x5a000000
#define INSTID_VALID_MASK       0xff000000

typedef struct IfcAtmTdInfo {
   UINT32 peakCellRate;
   UINT32 sustainedCellRate;
   UINT32 maxBurstSize;
   char serviceCategory[BUFLEN_16];
} IFC_ATM_TD_INFO, *PIFC_ATM_TD_INFO;

//mwang- not used
#ifdef not_used
typedef struct IfcAtmVccInfo {
   UINT32 id;
   UINT16 tdId;
   UINT32 aalType;
   UINT32 adminStatus;
   UINT16 encapMode;
   UINT16 enblQos;
   ATM_VCC_ADDR vccAddr;
} IFC_ATM_VCC_INFO, *PIFC_ATM_VCC_INFO;
#endif

#define DHCP_SRV_DISABLE   0
#define DHCP_SRV_ENABLE    1
#define DHCP_SRV_RELAY     2

typedef struct IfcDhcpSrvInfo {
   UINT16 enblDhcpSrv;
   UINT16 leasedTime;
   struct in_addr startAddress;
   struct in_addr endAddress;
} IFC_DHCP_SRV_INFO, *PIFC_DHCP_SRV_INFO;

typedef struct IfcLanInfo {
   UINT32 id;
   struct in_addr lanAddress;
   struct in_addr lanMask;
   IFC_DHCP_SRV_INFO dhcpSrv;
} IFC_LAN_INFO, *PIFC_LAN_INFO;

typedef struct IfcDnsInfo {
   struct in_addr preferredDns;
   struct in_addr alternateDns;
   UINT16 dynamic;
   char domainName[BUFLEN_64];
} IFC_DNS_INFO, *PIFC_DNS_INFO;

typedef struct IfcDefGWInfo {
   UINT16 enblGwAutoAssign;     // ==1 use auto assigned def. gw
   char ifName[BUFLEN_16];
   struct in_addr defaultGateway;
} IFC_DEF_GW_INFO, *PIFC_DEF_GW_INFO;

#define ETH_SPEED_AUTO           0
#define ETH_SPEED_100            1
#define ETH_SPEED_10             2
#define ETH_TYPE_AUTO            0
#define ETH_TYPE_FULL_DUPLEX     1
#define ETH_TYPE_HALF_DUPLEX     2
#define ETH_CFG_ID               1024
#define ETH_CFG_MTU_MIN          46
#define ETH_CFG_MTU_MAX          1500
typedef struct EthCfgInfo {
   UINT16 ethSpeed;
   UINT16 ethType;
   UINT16 ethMtu;
   UINT16 ethIfcDisabled;
} ETH_CFG_INFO, *PETH_CFG_INFO;

// multiple protocols changes
// new database structures for multiple protocols
// over single PVC behind this line

// WAN flag define
// 8 bit
// bit 0 : enable firewall
// bit 1 : enable nat
// bit 2 : enable igmp
// bit 3 : enable service
// bit 4-7: reserve
typedef struct WanFlagInfo {
   UINT8
   firewall:1,
   nat:1,
   igmp:1,
   service:1,
   reserved:4;
} WAN_FLAG_INFO;

typedef struct WanPppInfo {
   UINT16 idleTimeout;  // 0 disable onDemand, > 0 timeout in second
   UINT8 enblIpExtension;
   UINT8 authMethod;
   char userName[BUFLEN_256];
   char password[BUFLEN_40];
   char serverName[BUFLEN_40];
   UINT16 useStaticIpAddress;
   struct in_addr pppLocalIpAddress;
   UINT8 enblDebug;    // 0 disable debugging, 1 enable debugging
} WAN_PPP_INFO, *PWAN_PPP_INFO;

typedef struct WanIpInfo {
   UINT8 enblDhcpClnt;
   struct in_addr wanAddress;
   struct in_addr wanMask;
   UBOOL8 dhcpcOp125Enabled;
   UINT32 dhcpcOp51LeasedTime;
   char dhcpcOp50IpAddress[BUFLEN_16];
   char dhcpcOp54ServerIpAddress[BUFLEN_16];
   char dhcpcOp61IAID[BUFLEN_16];
   char dhcpcOp60VID[BUFLEN_256];
   char dhcpcOp61DUID[BUFLEN_256];
   char dhcpcOp77UID[BUFLEN_256];
} WAN_IP_INFO, *PWAN_IP_INFO;

typedef struct WanConInfo {
   UINT32 id;
   UINT16 vccId;
   UINT16 conId;
   UINT8 protocol;
   UINT8 encapMode;
   SINT16 vlanId;          // 0 - 4095, -1 -> vlan is not enabled
   WAN_FLAG_INFO flag;
   char conName[BUFLEN_40];
} WAN_CON_INFO, *PWAN_CON_INFO;

typedef struct WanConId {
   UINT16 portId;
   UINT16 vpi;
   UINT16 vci;
   UINT16 conId;
} WAN_CON_ID, *PWAN_CON_ID;


// System flag define
#define SYS_FLAG_AUTO_SCAN           0
#define SYS_FLAG_UPNP                1
#define SYS_FLAG_MAC_POLICY          2
#define SYS_FLAG_SIPROXD             3
#define SYS_FLAG_ENCODE_PASSWORD     4
#define SYS_FLAG_IGMP_SNP            5
#define SYS_FLAG_IGMP_MODE           6
#define SYS_FLAG_ENET_WAN            7
typedef struct SysFlagInfo {
   UINT8
   autoScan:1,
   upnp:1,
   macPolicy:1,
   siproxd:1,
   encodePassword:1,
   igmpSnp:1,
   igmpMode:1,
#if SUPPORT_ETHWAN
   enetWan:1;
#else
   reserved:1;
#endif
} SYS_FLAG_INFO, *PSYS_FLAG_INFO;

typedef struct SysMgmtLockInfo {
   UINT8 action;
   UINT8 type;
} SYS_MGMT_LOCK_INFO, *PSYS_MGMT_LOCK_INFO;

#endif
