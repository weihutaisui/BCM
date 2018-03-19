/*----------------------------------------------------------------------*
 *
 * Copyright (c) 2005-2012 Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
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
 * :>
 *
 *----------------------------------------------------------------------*
 * File Name  : tr64defs.h
 *
 *----------------------------------------------------------------------*/

#ifndef TR64_DEFS_H
#define TR64_DEFS_H

//#include "bcmcfm.h"

#if defined(__cplusplus)
extern "C" {
#endif
extern void tr64_start(void);
#if defined(__cplusplus)
}
#endif

#include "typedefs.h"

#define TR64_DSLFORUM_SCHEMA      "dslforum-org"
#define TR64_NUMBER_EVENT_QUEUE   40
#define TR64_LAN_INTF_NAME        "br0"
#define TR64_PID_FILE             "/var/run/tr64.pid"
#define TR64_STATE_PERSISTENT_TOKEN   "tr64_state"
#define TR64_DSLF_CONFIG          "dslf-config"
#define TR64_DSLF_RESET           "dslf-reset"
#define TR64_DSLF_RESET_PWD       "admin"
#define TR64_REALM                "IgdAuthentication"
#define TR64_DOMAIN               "/"
#define TR64_ROOT_FRIENDLY_NAME   "Broadcom ADSL Router"
#define TR64_ROOTDEV_XML_NAME     "devicedesc.xml"

#define DEV_MFR   	          "Broadcom"
#define DEV_MFR_URL            "http://www.broadcom.com/"
#define DEV_MODEL_DESCRIPTION  "Broadcom single-chip ADSL router"
#define DEV_MODEL              "BRCM963xx"
#define DEV_MODEL_NO           "1.0"
#define DEV_MODEL_URL          "http://www.broadcom.com/"

typedef enum {
   FACTORY,
   NORMAL
} sysPasswordState;

typedef enum {
   IDLE,
   AUTHENTICATING,
   AUTHENTICATED,
   ACCESS_NOT_ALLOW
} authState;

typedef struct tr64PersistentData
{
   sysPasswordState passwordState;
   char password[64];    
   char provisioningCode[64];
} *pTr64PersistentData, tr64PersistentData;


/* TODO: the following could be shared with TR69 */
typedef enum {
   eNone,
   eDigest,
   eBasic
} eAuthentication;

typedef enum {
   eNoQop,
   eAuth,
   eAuthInt
} eQop;

typedef struct sessionAuth {
   eQop  qopType;
   int      nonceCnt;
   char  *nc;     /* str of nonceCnt */
   char    *nonce;
   char    *orignonce;
   char    *realm;
   char    *domain;
   char    *method;
   char    *cnonce;
   char  *opaque;
   char  *qop;
   char    *user;
   char    *uri;
   char  *algorithm;
   char    *response;
   char    *basic;
   unsigned    char    requestDigest[33];
} sessionAuth;
/* TODO: the above could be shared with TR69 */

/** Macro to push another instance id onto the instance stack. */
#define PUSH_INSTANCE_ID(s, id) \
   do {if ((s)->currentDepth < MAX_MDM_INSTANCE_DEPTH ) { \
         (s)->instance[(s)->currentDepth] = (id);    \
         (s)->currentDepth++; }                       \
   } while(0)

#define TR64C_LOCK_TIMEOUT  (60 * MSECS_IN_SEC)
#endif   // TR64_DEFS_H
