/***********************************************************************
 *
 *  Copyright (c) 2006-2008  Broadcom Corporation
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

#ifndef __CGI_ROUTE_H__
#define __CGI_ROUTE_H__

#include <stdio.h>
#include <fcntl.h>

#define RT_MAX_STATIC_ENTRY     32       // max static route entries.

#ifdef SUPPORT_RIP
void cgiConfigRip(char *path, FILE *fs);

UBOOL8 cgiRtRipViewBody(FILE *fs);
UBOOL8 cgiRtRipViewBody_igd(FILE *fs);
UBOOL8 cgiRtRipViewBody_dev2(FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiRtRipViewBody(fs)   cgiRtRipViewBody_igd((fs))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiRtRipViewBody(fs)   cgiRtRipViewBody_igd((fs))
#elif defined(SUPPORT_DM_PURE181)
#define cgiRtRipViewBody(fs)   cgiRtRipViewBody_dev2((fs))
#elif defined(SUPPORT_DM_DETECT)
#define cgiRtRipViewBody(fs)   (cmsMdm_isDataModelDevice2() ? \
                              cgiRtRipViewBody_dev2((fs)) :   \
                              cgiRtRipViewBody_igd((fs)))
#endif
void writeRtRipScript(FILE *fs);
#endif /*SUPPORT_RIP*/

void cgiRtRouteAdd(char *path, FILE *fs);
void cgiRtRouteRemove(char *path, FILE *fs);

void cgiRtRouteView(FILE *fs);

void cgiRtRouteViewBody(FILE *fs);
void cgiRtRouteViewBody_igd(FILE *fs);
void cgiRtRouteViewBody_dev2(FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiRtRouteViewBody(fs)   cgiRtRouteViewBody_igd((fs))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiRtRouteViewBody(fs)   cgiRtRouteViewBody_igd((fs))
#elif defined(SUPPORT_DM_PURE181)
#define cgiRtRouteViewBody(fs)   cgiRtRouteViewBody_dev2((fs))
#elif defined(SUPPORT_DM_DETECT)
#define cgiRtRouteViewBody(fs)   (cmsMdm_isDataModelDevice2() ? \
                              cgiRtRouteViewBody_dev2((fs)) :   \
                              cgiRtRouteViewBody_igd((fs)))
#endif

void cgiRtRouteViewGetServiceName(const char *ifName, char *serviceName);
void cgiRtRouteViewGetServiceName_igd(const char *ifName, char *serviceName);
void cgiRtRouteViewGetServiceName_dev2(const char *ifName, char *serviceName);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiRtRouteViewGetServiceName(ifName,serviceName)   cgiRtRouteViewGetServiceName_igd((ifName),(serviceName))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiRtRouteViewGetServiceName(ifName,serviceName)   cgiRtRouteViewGetServiceName_igd((ifName),(serviceName))
#elif defined(SUPPORT_DM_PURE181)
#define cgiRtRouteViewGetServiceName(ifName,serviceName)   cgiRtRouteViewGetServiceName_dev2((ifName),(serviceName))
#elif defined(SUPPORT_DM_DETECT)
#define cgiRtRouteViewGetServiceName(ifName,serviceName)   (cmsMdm_isDataModelDevice2() ? \
                              cgiRtRouteViewGetServiceName_dev2((ifName),(serviceName)) :   \
                              cgiRtRouteViewGetServiceName_igd((ifName),(serviceName)))
#endif

void cgiRouteView(FILE *fs);
void writeRtRouteScript(FILE *fs);
void cgiConfigDefaultGatewayList(char *filename);
void cgiGetDefaultGatewList(char *varValue);
void displaySystemRouteIfMatched(FILE *fs, const char *wanIfName, const char *DNSServers, const char *defaultGateway);


#ifdef SUPPORT_POLICYROUTING

void cgiPrViewBody( FILE *fs );
void cgiPrViewBody_igd( FILE *fs );
void cgiPrViewBody_dev2( FILE *fs );

#if defined(SUPPORT_DM_LEGACY98)
#define cgiPrViewBody(fs)   cgiPrViewBody_igd((fs))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiPrViewBody(fs)   cgiPrViewBody_igd((fs))
#elif defined(SUPPORT_DM_PURE181)
#define cgiPrViewBody(fs)   cgiPrViewBody_dev2((fs))
#elif defined(SUPPORT_DM_DETECT)
#define cgiPrViewBody(fs)   (cmsMdm_isDataModelDevice2() ? \
                              cgiPrViewBody_dev2((fs)) :   \
                              cgiPrViewBody_igd((fs)))
#endif

#endif /*SUPPORT_POLICYROUTING*/

#endif // __CGI_ROUTE_H__

