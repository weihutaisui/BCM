/***********************************************************************
 *
 *  Copyright (c) 2006-2010  Broadcom Corporation
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

#ifndef __CGI_DIAG_H__
#define __CGI_DIAG_H__

#include <stdio.h>
#include <fcntl.h>

void cgiGetDiagId(int type, PDIAG_ID pDiagId);
void cgiGetDiagId_igd(int type, PDIAG_ID pDiagId);
void cgiGetDiagId_dev2(int type, PDIAG_ID pDiagId);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiGetDiagId(a,b)   cgiGetDiagId_igd((a),(b))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiGetDiagId(a,b)   cgiGetDiagId_igd((a),(b))
#elif defined(SUPPORT_DM_PURE181)
#define cgiGetDiagId(a,b)   cgiGetDiagId_dev2((a),(b))
#elif defined(SUPPORT_DM_DETECT)
#define cgiGetDiagId(a,b)   (cmsMdm_isDataModelDevice2() ? \
                             cgiGetDiagId_dev2((a),(b)) :   \
                             cgiGetDiagId_igd((a),(b)))

#endif

void cgiGetDiagInfo(int argc, char **argv, char *varValue);
void cgiGetDiagnostic(int argc, char **argv, char *varValue);

void cgiPrintEnetDiag(char *varValue);
void cgiPrintEnetDiag_igd(char *varValue);
void cgiPrintEnetDiag_dev2(char *varValue);
#if defined(SUPPORT_DM_LEGACY98)
#define cgiPrintEnetDiag(a)   cgiPrintEnetDiag_igd((a))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiPrintEnetDiag(a)   cgiPrintEnetDiag_igd((a))
#elif defined(SUPPORT_DM_PURE181)
#define cgiPrintEnetDiag(a)   cgiPrintEnetDiag_dev2((a))
#elif defined(SUPPORT_DM_DETECT)
#define cgiPrintEnetDiag(a)   (cmsMdm_isDataModelDevice2() ? \
                               cgiPrintEnetDiag_dev2((a)) :  \
                               cgiPrintEnetDiag_igd((a)))
#endif

void cgiPrintUsbDiag(char *varValue);
void cgiPrintUsbDiag_igd(char *varValue);
void cgiPrintUsbDiag_dev2(char *varValue);
#if defined(SUPPORT_DM_LEGACY98)
#define cgiPrintUsbDiag(a)   cgiPrintUsbDiag_igd((a))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiPrintUsbDiag(a)   cgiPrintUsbDiag_igd((a))
#elif defined(SUPPORT_DM_PURE181)
#define cgiPrintUsbDiag(a)   cgiPrintUsbDiag_dev2((a))
#elif defined(SUPPORT_DM_DETECT)
#define cgiPrintUsbDiag(a)   (cmsMdm_isDataModelDevice2() ? \
                              cgiPrintUsbDiag_dev2((a)) :   \
                              cgiPrintUsbDiag_igd((a)))

#endif

void cgiPrintWirelessDiag(char *varValue);
void cgiPrintWirelessDiag_igd(char *varValue);
void cgiPrintWirelessDiag_dev2(char *varValue);
#if defined(SUPPORT_DM_LEGACY98)
#define cgiPrintWirelessDiag(a)   cgiPrintWirelessDiag_igd((a))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiPrintWirelessDiag(a)   cgiPrintWirelessDiag_igd((a))
#elif defined(SUPPORT_DM_PURE181)
#define cgiPrintWirelessDiag(a)   cgiPrintWirelessDiag_dev2((a))
#elif defined(SUPPORT_DM_DETECT)
#define cgiPrintWirelessDiag(a)   (cmsMdm_isDataModelDevice2() ? \
                                   cgiPrintWirelessDiag_dev2((a)) :                  \
                                   cgiPrintWirelessDiag_igd((a)))

#endif

void cgiPrintAdslConnDiag(char *varValue);
void cgiPrintAdslConnDiag_igd(char *varValue);
void cgiPrintAdslConnDiag_dev2(char *varValue);
#if defined(SUPPORT_DM_LEGACY98)
#define cgiPrintAdslConnDiag(a)   cgiPrintAdslConnDiag_igd((a))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiPrintAdslConnDiag(a)   cgiPrintAdslConnDiag_igd((a))
#elif defined(SUPPORT_DM_PURE181)
#define cgiPrintAdslConnDiag(a)   cgiPrintAdslConnDiag_dev2((a))
#elif defined(SUPPORT_DM_DETECT)
#define cgiPrintAdslConnDiag(a)   (cmsMdm_isDataModelDevice2() ? \
                                   cgiPrintAdslConnDiag_dev2((a)) :                  \
                                   cgiPrintAdslConnDiag_igd((a)))

#endif


void cgiPrintOAMLoopbackDiag(char *varValue, int type, PDIAG_ID pDiagId);
void cgiPrintOAMLoopbackDiag_igd(char *varValue, int type, PDIAG_ID pDiagId);
void cgiPrintOAMLoopbackDiag_dev2(char *varValue, int type, PDIAG_ID pDiagId);
#if defined(SUPPORT_DM_LEGACY98)
#define cgiPrintOAMLoopbackDiag(a,b,c)   cgiPrintOAMLoopbackDiag_igd((a),(b),(c))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiPrintOAMLoopbackDiag(a,b,c)   cgiPrintOAMLoopbackDiag_igd((a),(b),(c))
#elif defined(SUPPORT_DM_PURE181)
#define cgiPrintOAMLoopbackDiag(a,b,c)   cgiPrintOAMLoopbackDiag_dev2((a),(b),(c))
#elif defined(SUPPORT_DM_DETECT)
#define cgiPrintOAMLoopbackDiag(a,b,c)   (cmsMdm_isDataModelDevice2() ?  \
                                          cgiPrintOAMLoopbackDiag_dev2((a),(b),(c)) : \
                                          cgiPrintOAMLoopbackDiag_igd((a),(b),(c)))

#endif

void cgiPrintPppDiag(char *varValue, int type, PDIAG_ID pDiagId);
void cgiPrintPppDiag_igd(char *varValue, int type, PDIAG_ID pDiagId);
void cgiPrintPppDiag_dev2(char *varValue, int type, PDIAG_ID pDiagId);
#if defined(SUPPORT_DM_LEGACY98)
#define cgiPrintPppDiag(a,b,c)   cgiPrintPppDiag_igd((a),(b),(c))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiPrintPppDiag(a,b,c)   cgiPrintPppDiag_igd((a),(b),(c))
#elif defined(SUPPORT_DM_PURE181)
#define cgiPrintPppDiag(a,b,c)   cgiPrintPppDiag_dev2((a),(b),(c))
#elif defined(SUPPORT_DM_DETECT)
#define cgiPrintPppDiag(a,b,c)   (cmsMdm_isDataModelDevice2() ?  \
                                  cgiPrintPppDiag_dev2((a),(b),(c)) :   \
                                  cgiPrintPppDiag_igd((a),(b),(c)))

#endif


void cgiPrintDefaultGatewayDiag(char *varValue);
void cgiPrintDefaultGatewayDiag_igd(char *varValue);
void cgiPrintDefaultGatewayDiag_dev2(char *varValue);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiPrintDefaultGatewayDiag(a)   cgiPrintDefaultGatewayDiag_igd((a))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiPrintDefaultGatewayDiag(a)   cgiPrintDefaultGatewayDiag_igd((a))
#elif defined(SUPPORT_DM_PURE181)
#define cgiPrintDefaultGatewayDiag(a)   cgiPrintDefaultGatewayDiag_dev2((a))
#elif defined(SUPPORT_DM_DETECT)
#define cgiPrintDefaultGatewayDiag(a)   (cmsMdm_isDataModelDevice2() ? \
                                         cgiPrintDefaultGatewayDiag_dev2((a)) : \
                                         cgiPrintDefaultGatewayDiag_igd((a)))

#endif


void cgiPrintDnsDiag(char *varValue);
void cgiPrintDnsDiag_igd(char *varValue);
void cgiPrintDnsDiag_dev2(char *varValue);
#if defined(SUPPORT_DM_LEGACY98)
#define cgiPrintDnsDiag(a)   cgiPrintDnsDiag_igd((a))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiPrintDnsDiag(a)   cgiPrintDnsDiag_igd((a))
#elif defined(SUPPORT_DM_PURE181)
#define cgiPrintDnsDiag(a)   cgiPrintDnsDiag_dev2((a))
#elif defined(SUPPORT_DM_DETECT)
#define cgiPrintDnsDiag(a)   (cmsMdm_isDataModelDevice2() ? \
                              cgiPrintDnsDiag_dev2((a)) :            \
                              cgiPrintDnsDiag_igd((a)))

#endif

#ifdef DMP_X_BROADCOM_COM_ETHERNETOAM_1
void cgiEthOam(char *query, FILE *fs);
void cgiGetEoamIntfList(int argc, char **argv, char *varValue);
void cgiGetEoam1agLbmResult(int argc, char **argv, char *varValue);
void cgiGetEoam1agLtmResult(int argc, char **argv, char *varValue);
#endif

#endif /* __CGI_DIAG_H__ */
