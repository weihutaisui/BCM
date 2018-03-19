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

#ifndef __CGI_WIRELESS_H__
#define __CGI_WIRELESS_H__

#include <stdio.h>
#include <fcntl.h>

/********************** Global Types ****************************************/

#ifdef SUPPORT_UNIFIED_WLMNGR
#undef SUPPORT_DM_LEGACY98
#undef SUPPORT_DM_HYBRID
#ifndef  SUPPORT_DM_PURE181
#define SUPPORT_DM_PURE181
#endif
#endif

void wlParseSetUrl(char *path);
#ifdef SES
void do_ezconfig_asp(char *path, FILE *fs);
#endif
void do_wl_cgi(char *path, FILE *fs);
void cgiWlMacFlt(char *query, FILE *fs);
void cgiWlMacFltAdd(char *query, FILE *fs);
void cgiWlMacFltRemove(char *query, FILE *fs);
void cgiWlMacFltSave(char *query, FILE *fs);
void cgiWlMacFltView(FILE *fs);
void writeWlMacFltScript(FILE *fs);
void cgiWlWds(char *query, FILE *fs);
void cgiWlWdsSave(char *query, FILE *fs);
void cgiWlWdsView(char *query, FILE *fs);
void writeWlWdsScript(char *query, FILE *fs);
void cgiWlStationList(char *query, FILE *fs);
void wlSetTargetPage(char *page);
#ifdef BCMWAPI_WAI
void cgiWlWapiAs(char *query, FILE *fs);
int cgiWlWapiApCertUpload(FILE * stream, int upload_len);
#endif
#endif
