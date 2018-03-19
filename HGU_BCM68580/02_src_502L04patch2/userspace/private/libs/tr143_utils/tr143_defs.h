/***********************************************************************
 *
 *  Copyright (c) 2012-2013  Broadcom Corporation
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

#ifndef _TR143_DEFS_H_
#define _TR143_DEFS_H_

#include <sys/socket.h>
#include <netinet/in.h>

#include "cms.h"
#include "cms_util.h"




#define TR143_AGENT_NAME        "BCM_TR143_DIAG_04_06"
#define TR143_SESSION_TIMEOUT   10
#define TR143_LOCK_TIMEOUT      (5*1000)
#define TR143_BUF_SIZE_MAX      4096

#define TR143_UPLOAD_RESULT_FILE    "/tmp/ulDiagResult"
#define TR143_DOWNLOAD_RESULT_FILE  "/tmp/dlDiagResult"


/*
 * These variables are used to pass args between app and tr143_utils lib.
 * All variables are declared in the tr143_utils lib, common.c
 */
extern char *tr143_result_fname;
extern  struct tr143diagstats_t diagstats;
extern const char *DiagnosticsState[];
extern char server_addr[16];
extern int server_port;
extern char uri[256], proto[256];
extern char *url, if_name[32];
extern unsigned char dscp;
extern unsigned int testFileLength;
extern void *msgHandle;



struct tr143diagstats_t
{
   char DiagnosticsState[64];
   char ROMTime[64];
   char BOMTime[64];
   char EOMTime[64];
   unsigned int TestBytesReceived;
   unsigned int TotalBytesReceived;
   unsigned int TotalBytesSent;
   char TCPOpenRequestTime[64];
   char TCPOpenResponseTime[64];
};


typedef enum{
   Completed=2,
   Error_InitConnectionFailed,
   Error_NoResponse,
   Error_TransferFailed,
   Error_PasswordRequestFailed,
   Error_LoginFailed,
   Error_NoTransferMode,
   Error_NoPASV,
   Error_IncorrectSize,
   Error_Timeout,
   Error_NoCWD,
   Error_NoSTOR,
   Error_Internal
} Tr143DiagState;



/*
 * These are the publicly available functions from libtr143_utils
 */
int setupconfig(void);

int ftpUploadDiag (void);
int ftpDownloadDiag (void);

int httpDownloadDiag (void);
int httpUploadDiag (void);

void cleanup_and_notify(Tr143DiagState state, CmsEntityId eid);



#endif /* _TR143_DEFS_H_ */
