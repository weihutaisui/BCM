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


#ifndef __CGI_WL_DMX_H__
#define __CGI_WL_DMX_H__
#include <wlcsm_lib_api.h>

#ifdef SUPPORT_UNIFIED_WLMNGR
#undef SUPPORT_DM_LEGACY98
#undef SUPPORT_DM_HYBRID
#ifndef  SUPPORT_DM_PURE181
#define SUPPORT_DM_PURE181
#endif
#endif

/*!\file cgi_wl_dmx.h
 * \brief This file provides the "data model switching" macros for the
 * interface between httpd and the wlan libs.  Httpd code will call function
 * names of the form: funcName_dmx.
 * When we are using the Legacy TR98 (IGD) data model, either at compile time
 * or runtime, these macros will cause the call to go to the original function,
 * funcName.
 * When we are using the new TR181 (dev2) data model, either at compile time
 * or runtime, these macros will cause the call to go to the new function,
 * funcName_dev2.
 *
 */




/** Set a global variable which indicates which adapter/SSID(?) we are
 *  currently operating on
 */
void BcmWl_Switch_instance_dmx(int index);  // macro-ized demux function used in code
void BcmWl_Switch_instance(int index);      // orig way of doing it
void BcmWl_Switch_instance_dev2(int index); // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define BcmWl_Switch_instance_dmx(i)           BcmWl_Switch_instance((i))
#elif defined(SUPPORT_DM_HYBRID)
#define BcmWl_Switch_instance_dmx(i)           BcmWl_Switch_instance((i))
#elif defined(SUPPORT_DM_PURE181)
#define BcmWl_Switch_instance_dmx(i)           BcmWl_Switch_instance_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define BcmWl_Switch_instance_dmx(i)           (cmsMdm_isDataModelDevice2() ? \
                                          BcmWl_Switch_instance_dev2((i)) : \
                                          BcmWl_Switch_instance((i)))
#endif




/** Set a single variable value
 *
 */
void BcmWl_SetVar_dmx(char *varName, char *varValue);  // macro-ized demux function used in code
void BcmWl_SetVar(char *varName, char *varValue);      // orig way of doing it
void BcmWl_SetVar_dev2(char *varName, char *varValue); // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define BcmWl_SetVar_dmx(n, v)           BcmWl_SetVar((n), (v))
#elif defined(SUPPORT_DM_HYBRID)
#define BcmWl_SetVar_dmx(n, v)           BcmWl_SetVar((n), (v))
#elif defined(SUPPORT_DM_PURE181)
#define BcmWl_SetVar_dmx(n, v)           BcmWl_SetVar_dev2((n), (v))
#elif defined(SUPPORT_DM_DETECT)
#define BcmWl_SetVar_dmx(n, v)           (cmsMdm_isDataModelDevice2() ? \
                                          BcmWl_SetVar_dev2((n), (v)) : \
                                          BcmWl_SetVar((n), (v)))
#endif




/** Get a single variable value
 *
 */
void BcmWl_GetVar_dmx(char *varName, char *varValue);  // macro-ized demux function used in code
void BcmWl_GetVar(char *varName, char *varValue);      // orig way of doing it
void BcmWl_GetVar_dev2(char *varName, char *varValue); // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define BcmWl_GetVar_dmx(n, v)           BcmWl_GetVar((n), (v))
#elif defined(SUPPORT_DM_HYBRID)
#define BcmWl_GetVar_dmx(n, v)           BcmWl_GetVar((n), (v))
#elif defined(SUPPORT_DM_PURE181)
#define BcmWl_GetVar_dmx(n, v)           BcmWl_GetVar_dev2((n), (v))
#elif defined(SUPPORT_DM_DETECT)
#define BcmWl_GetVar_dmx(n, v)           (cmsMdm_isDataModelDevice2() ? \
                                          BcmWl_GetVar_dev2((n), (v)) : \
                                          BcmWl_GetVar((n), (v)))
#endif



/** Get multiple values as specified in argc/argv
 *
 */
void BcmWl_GetVarEx_dmx(int argc, char **argv, char *varValue);  // macro-ized demux function used in code
void BcmWl_GetVarEx(int argc, char **argv, char *varValue);      // orig way of doing it
void BcmWl_GetVarEx_dev2(int argc, char **argv, char *varValue); // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define BcmWl_GetVarEx_dmx(c, s, v)      BcmWl_GetVarEx((c), (s), (v))
#elif defined(SUPPORT_DM_HYBRID)
#define BcmWl_GetVarEx_dmx(c, s, v)      BcmWl_GetVarEx((c), (s), (v))
#elif defined(SUPPORT_DM_PURE181)
#define BcmWl_GetVarEx_dmx(c, s, v)      BcmWl_GetVarEx_dev2((c), (s), (v))
#elif defined(SUPPORT_DM_DETECT)
#define BcmWl_GetVarEx_dmx(c, s, v)   (cmsMdm_isDataModelDevice2() ? \
                                       BcmWl_GetVarEx_dev2((c), (s), (v)) : \
                                       BcmWl_GetVarEx((c), (s), (v)))
#endif




/** Add a mac filter
 *
 */
int BcmWl_addFilterMac_dmx(char *mac, char *ssid, char *ifcName);  // macro-ized demux function used in code
// int BcmWl_addFilterMac(char *mac, char *ssid, char *ifcName);      // orig way of doing it; declared in wlapi.h
int BcmWl_addFilterMac_dev2(char *mac, char *ssid, char *ifcName); // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define BcmWl_addFilterMac_dmx(m, s, i)      BcmWl_addFilterMac((m), (s), (i))
#elif defined(SUPPORT_DM_HYBRID)
#define BcmWl_addFilterMac_dmx(m, s, i)      BcmWl_addFilterMac((m), (s), (i))
#elif defined(SUPPORT_DM_PURE181)
#define BcmWl_addFilterMac_dmx(m, s, i)      BcmWl_addFilterMac_dev2((m), (s), (i))
#elif defined(SUPPORT_DM_DETECT)
#define BcmWl_addFilterMac_dmx(m, s, i)   (cmsMdm_isDataModelDevice2() ? \
                                 BcmWl_addFilterMac_dev2((m), (s), (i)) : \
                                 BcmWl_addFilterMac((m), (s), (i)))
#endif



/** Remove a mac filter
 *
 */
int BcmWl_removeFilterMac_dmx(char *mac, char *ifcName);  // macro-ized demux function used in code
// int BcmWl_removeFilterMac(char *mac, char *ifcName);      // orig way of doing it; declared in wlapi.h
int BcmWl_removeFilterMac_dev2(char *mac, char *ifcName); // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define BcmWl_removeFilterMac_dmx(m, i)      BcmWl_removeFilterMac((m), (i))
#elif defined(SUPPORT_DM_HYBRID)
#define BcmWl_removeFilterMac_dmx(m, i)      BcmWl_removeFilterMac((m), (i))
#elif defined(SUPPORT_DM_PURE181)
#define BcmWl_removeFilterMac_dmx(m, i)      BcmWl_removeFilterMac_dev2((m), (i))
#elif defined(SUPPORT_DM_DETECT)
#define BcmWl_removeFilterMac_dmx(m, i)   (cmsMdm_isDataModelDevice2() ? \
                                     BcmWl_removeFilterMac_dev2((m), (i)) : \
                                     BcmWl_removeFilterMac((m), (i)))
#endif



/** Return 1 if there are no mac filters in the current adapter/ssid
 *
 */
int BcmWl_isMacFltEmpty_dmx(void);  // macro-ized demux function used in code
int BcmWl_isMacFltEmpty(void);      // orig way of doing it
int BcmWl_isMacFltEmpty_dev2(void); // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define BcmWl_isMacFltEmpty_dmx()      BcmWl_isMacFltEmpty()
#elif defined(SUPPORT_DM_HYBRID)
#define BcmWl_isMacFltEmpty_dmx()      BcmWl_isMacFltEmpty()
#elif defined(SUPPORT_DM_PURE181)
#define BcmWl_isMacFltEmpty_dmx()      BcmWl_isMacFltEmpty_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define BcmWl_isMacFltEmpty_dmx()      (cmsMdm_isDataModelDevice2() ? \
                                        BcmWl_isMacFltEmpty_dev2() : \
                                        BcmWl_isMacFltEmpty())
#endif



/** Start a WDS mac/ssid scan
 *
 */
void BcmWl_ScanWdsMacStart_dmx(void);  // macro-ized demux function used in code
void BcmWl_ScanWdsMacStart(void);      // orig way of doing it
void BcmWl_ScanWdsMacStart_dev2(void); // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define BcmWl_ScanWdsMacStart_dmx()      BcmWl_ScanWdsMacStart()
#elif defined(SUPPORT_DM_HYBRID)
#define BcmWl_ScanWdsMacStart_dmx()      BcmWl_ScanWdsMacStart()
#elif defined(SUPPORT_DM_PURE181)
#define BcmWl_ScanWdsMacStart_dmx()      BcmWl_ScanWdsMacStart_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define BcmWl_ScanWdsMacStart_dmx()     (cmsMdm_isDataModelDevice2() ? \
                                         BcmWl_ScanWdsMacStart_dev2() : \
                                         BcmWl_ScanWdsMacStart())
#endif



/** Collect the results of a WDS mac/ssid scan
 *
 */
void BcmWl_ScanWdsMacResult_dmx(void);  // macro-ized demux function used in code
void BcmWl_ScanWdsMacResult(void);      // orig way of doing it
void BcmWl_ScanWdsMacResult_dev2(void); // new way of doing it (and using TR181)
void BcmWl_print_stalist(FILE *fs);
void BcmWl_print_wdsscanlist(FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define BcmWl_ScanWdsMacResult_dmx()      BcmWl_ScanWdsMacResult()
#elif defined(SUPPORT_DM_HYBRID)
#define BcmWl_ScanWdsMacResult_dmx()      BcmWl_ScanWdsMacResult()
#elif defined(SUPPORT_DM_PURE181)
#define BcmWl_ScanWdsMacResult_dmx()      BcmWl_ScanWdsMacResult_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define BcmWl_ScanWdsMacResult_dmx()     (cmsMdm_isDataModelDevice2() ? \
                                         BcmWl_ScanWdsMacResult_dev2() : \
                                         BcmWl_ScanWdsMacResult())
#endif


/** Add a WDS mac
 *
 */
int BcmWl_addWdsMac_dmx(char *mac);  // macro-ized demux function used in code
// int BcmWl_addWdsMac(char *mac);   // orig way of doing it; defined in wlapi.h
int BcmWl_addWdsMac_dev2(char *mac); // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define BcmWl_addWdsMac_dmx(m)           BcmWl_addWdsMac_tr98((m))
#elif defined(SUPPORT_DM_HYBRID)
#define BcmWl_addWdsMac_dmx(m)           BcmWl_addWdsMac_tr98((m))
#elif defined(SUPPORT_DM_PURE181)
#define BcmWl_addWdsMac_dmx(m)           BcmWl_addWdsMac_dev2((m))
#elif defined(SUPPORT_DM_DETECT)
#define BcmWl_addWdsMac_dmx(m)           (cmsMdm_isDataModelDevice2() ? \
                                          BcmWl_addWdsMac_dev2((m)) : \
                                          BcmWl_addWdsMac_tr98((m)))
#endif



/** Remove all WDS macs
 *
 */
int BcmWl_removeAllWdsMac_dmx(void); // macro-ized demux function used in code
// int BcmWl_removeAllWdsMac(void);  // orig way of doing it; defined in wlapi.h
int BcmWl_removeAllWdsMac_dev2(void);// new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define BcmWl_removeAllWdsMac_dmx()      BcmWl_removeAllWdsMac()
#elif defined(SUPPORT_DM_HYBRID)
#define BcmWl_removeAllWdsMac_dmx()      BcmWl_removeAllWdsMac()
#elif defined(SUPPORT_DM_PURE181)
#define BcmWl_removeAllWdsMac_dmx()      BcmWl_removeAllWdsMac_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define BcmWl_removeAllWdsMac_dmx()      (cmsMdm_isDataModelDevice2() ? \
                                          BcmWl_removeAllWdsMac_dev2() : \
                                          BcmWl_removeAllWdsMac())
#endif



/** Allocate wlmngr adapter storage for specified number of adapters
 *
 */
int wlmngr_alloc_dmx(int adapter_cnt);    // macro-ized demux function used in code
int wlmngr_alloc(int adapter_cnt);        // orig way of doing it
int wlmngr_alloc_dev2(int adapter_cnt);   // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define wlmngr_alloc_dmx(a)               wlmngr_alloc((a))
#elif defined(SUPPORT_DM_HYBRID)
#define wlmngr_alloc_dmx(a)               wlmngr_alloc((a))
#elif defined(SUPPORT_DM_PURE181)
#define wlmngr_alloc_dmx(a)               wlmngr_alloc_dev2((a))
#elif defined(SUPPORT_DM_DETECT)
#define wlmngr_alloc_dmx(a)              (cmsMdm_isDataModelDevice2() ? \
                                          wlmngr_alloc_dev2((a)) : \
                                          wlmngr_alloc((a)))
#endif



/** Free wlmngr adapter storage
 *
 */
void wlmngr_free_dmx(void );       // macro-ized demux function used in code
void wlmngr_free(void );           // orig way of doing it
void wlmngr_free_dev2(void );      // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define wlmngr_free_dmx()               wlmngr_free()
#elif defined(SUPPORT_DM_HYBRID)
#define wlmngr_free_dmx()               wlmngr_free()
#elif defined(SUPPORT_DM_PURE181)
#define wlmngr_free_dmx()               wlmngr_free_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define wlmngr_free_dmx()              (cmsMdm_isDataModelDevice2() ? \
                                        wlmngr_free_dev2() : \
                                        wlmngr_free())
#endif



/** Allocate internal structure for storing TR-98 params (wl_dsl_tr_struct)
 *  for specified number of adapters
 */
int wldsltr_alloc_dmx(int adapter_cnt);    // macro-ized demux function used in code
int wldsltr_alloc(int adapter_cnt);        // orig way of doing it
int wldsltr_alloc_dev2(int adapter_cnt);   // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define wldsltr_alloc_dmx(a)               wldsltr_alloc((a))
#elif defined(SUPPORT_DM_HYBRID)
#define wldsltr_alloc_dmx(a)               wldsltr_alloc((a))
#elif defined(SUPPORT_DM_PURE181)
#define wldsltr_alloc_dmx(a)               wldsltr_alloc_dev2((a))
#elif defined(SUPPORT_DM_DETECT)
#define wldsltr_alloc_dmx(a)              (cmsMdm_isDataModelDevice2() ? \
                                           wldsltr_alloc_dev2((a)) : \
                                           wldsltr_alloc((a)))
#endif



/** Free internal structure for storing TR-98 params (wl_dsl_tr_struct)
 *
 */
void wldsltr_free_dmx(void );       // macro-ized demux function used in code
void wldsltr_free(void );           // orig way of doing it
void wldsltr_free_dev2(void );      // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define wldsltr_free_dmx()               wldsltr_free()
#elif defined(SUPPORT_DM_HYBRID)
#define wldsltr_free_dmx()               wldsltr_free()
#elif defined(SUPPORT_DM_PURE181)
#define wldsltr_free_dmx()               wldsltr_free_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define wldsltr_free_dmx()              (cmsMdm_isDataModelDevice2() ? \
                                         wldsltr_free_dev2() : \
                                         wldsltr_free())
#endif



/** Read all configurations from MDM into local memory.
 *
 */
CmsRet wlUnlockReadMdm_dmx(void );    // macro-ized demux function used in code
CmsRet wlUnlockReadMdm(void );        // orig way of doing it
CmsRet wlUnlockReadMdm_dev2(void );   // new way of doing it (and using TR181)


#if defined(SUPPORT_DM_LEGACY98)
#define wlUnlockReadMdm_dmx()          wlUnlockReadMdm()
#elif defined(SUPPORT_DM_HYBRID)
#define wlUnlockReadMdm_dmx()          wlUnlockReadMdm()
#elif defined(SUPPORT_DM_PURE181)
#define wlUnlockReadMdm_dmx()          wlUnlockReadMdm_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define wlUnlockReadMdm_dmx()         (cmsMdm_isDataModelDevice2() ? \
                                       wlUnlockReadMdm_dev2() : \
                                       wlUnlockReadMdm())
#endif



/** Write configuration of specified adapter into MDM.
 *
 */
CmsRet wlUnlockWriteMdmOne_dmx(int idx);    // macro-ized demux function used in code
CmsRet wlUnlockWriteMdmOne(int idx);        // orig way of doing it
CmsRet wlUnlockWriteMdmOne_dev2(int idx);   // new way of doing it (and using TR181)
void BcmWl_setWds_dev2(int i, char *mac,char *buf);


#if defined(SUPPORT_DM_LEGACY98)
#define wlUnlockWriteMdmOne_dmx(i)       wlUnlockWriteMdmOne(REAL_IDX(i))
#define BcmWl_setWds_dmx(i,lst,wdsbuf)
#define GLBCONFIGNEEDED_DMX      (1)
#elif defined(SUPPORT_DM_HYBRID)
#define wlUnlockWriteMdmOne_dmx(i)       wlUnlockWriteMdmOne(REAL_IDX(i))
#define BcmWl_setWds_dmx(i,lst,wdsbuf)
#define GLBCONFIGNEEDED_DMX      (1)
#elif defined(SUPPORT_DM_PURE181)
#define BcmWl_setWds_dmx(i,lst,wdsbuf)   BcmWl_setWds_dev2(i,lst,wdsbuf)
#define wlUnlockWriteMdmOne_dmx(i)       wlUnlockWriteMdmOne_dev2((i))
#define GLBCONFIGNEEDED_DMX      (0)
#elif defined(SUPPORT_DM_DETECT)
#define wlUnlockWriteMdmOne_dmx(i)      (cmsMdm_isDataModelDevice2() ? \
                                         wlUnlockWriteMdmOne_dev2((i)) : \
                                         wlUnlockWriteMdmOne(REAL_IDX(i)))
#define GLBCONFIGNEEDED_DMX     (!cmsMdm_isDataModelDevice2())
#define BcmWl_setWds_dmx(i,lst,wdsbuf)   if(cmsMdm_isDataModelDevice2()) \
						BcmWl_setWds_dev2(i,lst,wdsbuf)
#endif

#endif  /* __CGI_WL_DMX_H__ */
