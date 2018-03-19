/***********************************************************************
 *
 *  Copyright (c) 2000-2010  Broadcom Corporation
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

#ifndef __CGI_NTWK_H__
#define __CGI_NTWK_H__

#include <stdio.h>
#include <fcntl.h>

/********************** Global Types ****************************************/

// System Log fields
#define CGI_LOG_DATE_TIME 0
#define CGI_LOG_FACILITY  1
#define CGI_LOG_SEVERITY  2
#define CGI_LOG_MESSAGE   3

void writeNtwkHeader(FILE *fs);

void cgiGetLogData(char *line, char *data, int field);
int cgiGetLogSeverityLevel(char *severity);

int  cgiGetWanLinkStatus(void);
void cgiGetNtwkPrtclForWeb(char *varValue);
void cgiGetNoBridgeNtwkPrtclForWeb(char *varValue);
void cgiGetDns1(char *varValue);
void cgiGetDns2(char *varValue);
void cgiGetDnsIfc(char *varValue);
void cgiGetLanAddrForWeb(char *varValue);
void cgiGetDfltGtwyForWeb(char *varValue);
void cgiGetWanLinkStatusForWeb(char *varValue);
void cgiGetEnblNatForWeb(char *varValue);
void cgiGetEnblFirewallForWeb(char *varValue);
void cgiGetPppIpExtensionForWeb(char *varValue);




/*
 * ======= Interface Grouping related functions, in cgi_pmap.c =======
 */

void cgiPortMapAdd(char *query, FILE *fs);
void cgiPortMapRemove(char *query, FILE *fs);
void cgiPortMapEdit(char *query, FILE *fs);
void cgiPortMapView(FILE *fs);
void cgiPortMapConfig(char *query, FILE *fs);

/** Return the number of interface groups.
 *
 */
UINT32 cgiGetNumberOfIntfGroups(void);
UINT32 cgiGetNumberOfIntfGroups_igd(void);
UINT32 cgiGetNumberOfIntfGroups_dev2(void);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiGetNumberOfIntfGroups()  cgiGetNumberOfIntfGroups_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define cgiGetNumberOfIntfGroups()  cgiGetNumberOfIntfGroups_igd()
#elif defined(SUPPORT_DM_PURE181)
#define cgiGetNumberOfIntfGroups()  cgiGetNumberOfIntfGroups_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define cgiGetNumberOfIntfGroups()  (cmsMdm_isDataModelDevice2() ? \
                          cgiGetNumberOfIntfGroups_dev2() : \
                          cgiGetNumberOfIntfGroups_igd())
#endif


/** Get info associated with this interface group index for display.
 *
 * @param brIdx (IN) The n-th interface group in the system, counting from 0.
 */
void cgiGetIntfGroupInfo(UINT32 brIdx,
             char *groupName, UINT32 groupNameLen,
             char *intfList, UINT32 intfListLen, UINT32 *numIntf,
             char *wanIntfList, UINT32 wanIntfListLen, UINT32 *numWanIntf, char *wanIfName,
             char *vendorIdList, UINT32 vendorIdListLen, UINT32 *numVendorIds);

void cgiGetIntfGroupInfo_igd(UINT32 brIdx,
             char *groupName, UINT32 groupNameLen,
             char *intfList, UINT32 intfListLen, UINT32 *numIntf,
             char *wanIntfList, UINT32 wanIntfListLen, UINT32 *numWanIntf, char *wanIfName,
             char *vendorIdList, UINT32 vendorIdListLen, UINT32 *numVendorIds);

void cgiGetIntfGroupInfo_dev2(UINT32 brIdx,
             char *groupName, UINT32 groupNameLen,
             char *intfList, UINT32 intfListLen, UINT32 *numIntf,
             char *wanIntfList, UINT32 wanIntfListLen, UINT32 *numWanIntf, char *wanIfName,
             char *vendorIdList, UINT32 vendorIdListLen, UINT32 *numVendorIds);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiGetIntfGroupInfo(a, b, c, d, e, f, g, h, i, j, k, l, m)  cgiGetIntfGroupInfo_igd((a), (b), (c), (d), (e), (f), (g), (h), (i), (j), (k), (l), (m))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiGetIntfGroupInfo(a, b, c, d, e, f, g, h, i, j, k, l, m)  cgiGetIntfGroupInfo_igd((a), (b), (c), (d), (e), (f), (g), (h), (i), (j), (k), (l), (m))
#elif defined(SUPPORT_DM_PURE181)
#define cgiGetIntfGroupInfo(a, b, c, d, e, f, g, h, i, j, k, l, m)  cgiGetIntfGroupInfo_dev2((a), (b), (c), (d), (e), (f), (g), (h), (i), (j), (k), (l), (m))
#elif defined(SUPPORT_DM_DETECT)
#define cgiGetIntfGroupInfo(a, b, c, d, e, f, g, h, i, j, k, l, m)  (cmsMdm_isDataModelDevice2() ? \
                  cgiGetIntfGroupInfo_dev2((a), (b), (c), (d), (e), (f), (g), (h), (i), (j), (k), (l), (m)) : \
                  cgiGetIntfGroupInfo_igd((a), (b), (c), (d), (e), (f), (g), (h), (i), (j), (k), (l), (m)))
#endif






void cgiConfigDns(char *filename);

#ifdef PORT_MIRRORING
void cgiEngDebug (char *query, FILE *fs) ;
#endif

void cgiGetActiveDnsIp(char *outDns1, char *outDns2);
void cgiGetDnsInterfacesList(char *varValue);
void cgiGetDnsStaticIp(char *staticDns1, char *staticDns2, UBOOL8 isIPv4);

#endif
