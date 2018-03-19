/***********************************************************************
 *
 *  Copyright (c) 2005-2010  Broadcom Corporation
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

#ifndef __CGI_SECURITY_H__
#define __CGI_SECURITY_H__

#include <stdio.h>
#include <fcntl.h>

#define SEC_PTR_TRG_MAX          32       // max port triggering entries.
#define SEC_VIR_SRV_MAX          32       // max virtual server entries.
#define SEC_ADD_REMOVE_ROW       6        // display add remove button if row > 6

#define SEC_MAX_PORT_MIRRORS     2       // max virtual server entries.

void cgiScVrtSrvAdd(char *path, FILE *fs);
void cgiScVrtSrvRemove(char *path, FILE *fs);
void cgiScVrtSrvSave(FILE *fs);
void cgiScVrtSrvView(FILE *fs);
void writeScVrtSrvScript(FILE *fs);
void cgiScPrtTrgAdd(char *path, FILE *fs);
void cgiScPrtTrgRemove(char *path, FILE *fs);
void cgiScPrtTrgSave(FILE *fs);
void cgiScPrtTrgView(FILE *fs);
void writeScPrtTrgScript(FILE *fs);
void cgiScOutFltAdd(char *path, FILE *fs);
void cgiScOutFltRemove(char *path, FILE *fs);
void cgiScOutFltSave(FILE *fs);
void cgiScOutFltView(FILE *fs);
void writeScOutFltScript(FILE *fs);
void cgiScInFltAdd(char *path, FILE *fs);

CmsRet addIpFilterInAll(const char *name, const char *ipver, const char *protocol,
                            const char *srcAddr, const char *srcMask, const char *srcPort, 
                            const char *dstAddr, const char *dstMask, const char *dstPort);
CmsRet addIpFilterInAll_igd(const char *name, const char *ipver, const char *protocol,
                            const char *srcAddr, const char *srcMask, const char *srcPort, 
                            const char *dstAddr, const char *dstMask, const char *dstPort);
CmsRet addIpFilterInAll_dev2(const char *name, const char *ipver, const char *protocol,
                            const char *srcAddr, const char *srcMask, const char *srcPort, 
                            const char *dstAddr, const char *dstMask, const char *dstPort);


#if defined(SUPPORT_DM_LEGACY98)
#define addIpFilterInAll(n,i,p,sa,sm,sp,da,dm,dp)  addIpFilterInAll_igd((n),(i),(p),(sa),(sm),(sp),(da),(dm),(dp))
#elif defined(SUPPORT_DM_HYBRID)
#define addIpFilterInAll(n,i,p,sa,sm,sp,da,dm,dp)  addIpFilterInAll_igd((n),(i),(p),(sa),(sm),(sp),(da),(dm),(dp))
#elif defined(SUPPORT_DM_PURE181)
#define addIpFilterInAll(n,i,p,sa,sm,sp,da,dm,dp)  addIpFilterInAll_dev2((n),(i),(p),(sa),(sm),(sp),(da),(dm),(dp))
#elif defined(SUPPORT_DM_DETECT)
#define addIpFilterInAll(n,i,p,sa,sm,sp,da,dm,dp)  (cmsMdm_isDataModelDevice2() ? \
                                                    addIpFilterInAll_dev2((n),(i),(p),(sa),(sm),(sp),(da),(dm),(dp)) : \
                                                    addIpFilterInAll_igd((n),(i),(p),(sa),(sm),(sp),(da),(dm),(dp)))
#endif


void cgiScInFltRemove(char *path, FILE *fs);
void cgiScInFltSave(FILE *fs);
void cgiScInFltView(FILE *fs);
void writeScInFltScript(FILE *fs);
void cgiScAccCntrAdd(char *path, FILE *fs);
void cgiScAccCntrRemove(char *path, FILE *fs);
void cgiScAccCntrSave(char *path, FILE *fs);
void cgiScAccCntrView(FILE *fs);
void writeScAccCntrScript(FILE *fs);
void cgiScSrvCntrSave(char *path, FILE *fs);
void cgiScSrvCntrView(FILE *fs);
void writeScSrvCntrScript(FILE *fs);
void writeSecurityHeader(FILE *fs);
void writeSecurityBody(FILE *fs);
void cgiScMacFltAdd(char *path, FILE *fs);
void cgiScMacFltRemove(char *path, FILE *fs);
void cgiScMacFltChangePolicy(char *query, FILE *fs);
void cgiScMacFltSave(FILE *fs);
void cgiScMacFltView(FILE *fs);
void writeScMacFltScript(FILE *fs);

void cgiScInFltViewbody(FILE * fs);
void cgiScInFltViewbody_igd(FILE * fs);
void cgiScInFltViewbody_dev2(FILE * fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiScInFltViewbody(i)  cgiScInFltViewbody_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiScInFltViewbody(i)  cgiScInFltViewbody_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define cgiScInFltViewbody(i)  cgiScInFltViewbody_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define cgiScInFltViewbody(i)  (cmsMdm_isDataModelDevice2() ? \
                               cgiScInFltViewbody_dev2((i)) : \
                               cgiScInFltViewbody_igd((i)))
#endif

void cgiScOutFltViewbody(FILE *fs); 
void cgiScOutFltViewbody_igd(FILE *fs); 
void cgiScOutFltViewbody_dev2(FILE *fs); 

#if defined(SUPPORT_DM_LEGACY98)
#define cgiScOutFltViewbody(i)  cgiScOutFltViewbody_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiScOutFltViewbody(i)  cgiScOutFltViewbody_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define cgiScOutFltViewbody(i)  cgiScOutFltViewbody_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define cgiScOutFltViewbody(i)  (cmsMdm_isDataModelDevice2() ? \
                               cgiScOutFltViewbody_dev2((i)) : \
                               cgiScOutFltViewbody_igd((i)))
#endif

CmsRet cgiScMacFltViewBrWanbody(FILE *fs); 
CmsRet cgiScMacFltViewBrWanbody_igd(FILE *fs); 
CmsRet cgiScMacFltViewBrWanbody_dev2(FILE *fs); 

#if defined(SUPPORT_DM_LEGACY98)
#define cgiScMacFltViewBrWanbody(i)  cgiScMacFltViewBrWanbody_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiScMacFltViewBrWanbody(i)  cgiScMacFltViewBrWanbody_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define cgiScMacFltViewBrWanbody(i)  cgiScMacFltViewBrWanbody_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define cgiScMacFltViewBrWanbody(i)  (cmsMdm_isDataModelDevice2() ? \
                               cgiScMacFltViewBrWanbody_dev2((i)) : \
                               cgiScMacFltViewBrWanbody_igd((i)))
#endif

CmsRet cgiScMacFltViewCfgbody(FILE *fs); 
CmsRet cgiScMacFltViewCfgbody_igd(FILE *fs);
CmsRet cgiScMacFltViewCfgbody_dev2(FILE *fs); 

#if defined(SUPPORT_DM_LEGACY98)
#define cgiScMacFltViewCfgbody(i)  cgiScMacFltViewCfgbody_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiScMacFltViewCfgbody(i)  cgiScMacFltViewCfgbody_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define cgiScMacFltViewCfgbody(i)  cgiScMacFltViewCfgbody_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define cgiScMacFltViewCfgbody(i)  (cmsMdm_isDataModelDevice2() ? \
                               cgiScMacFltViewCfgbody_dev2((i)) : \
                               cgiScMacFltViewCfgbody_igd((i)))
#endif


void cgiScPrtTrgViewBody(FILE *fs); 
void cgiScPrtTrgViewBody_igd(FILE *fs);
void cgiScPrtTrgViewBody_dev2(FILE *fs); 

#if defined(SUPPORT_DM_LEGACY98)
#define cgiScPrtTrgViewBody(i)  cgiScPrtTrgViewBody_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiScPrtTrgViewBody(i)  cgiScPrtTrgViewBody_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define cgiScPrtTrgViewBody(i)  cgiScPrtTrgViewBody_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define cgiScPrtTrgViewBody(i)  (cmsMdm_isDataModelDevice2() ? \
                               cgiScPrtTrgViewBody_dev2((i)) : \
                               cgiScPrtTrgViewBody_igd((i)))
#endif

#endif
