/***********************************************************************
 *
 *  Copyright (c) 2017  Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2017:proprietary:standard

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

/*****************************************************************************
*    Description:
*
*      OMCI Wrapper (OW) Inter Process Communication (IPC) definitions and
*        client APIs.
*
*****************************************************************************/

#ifndef _OWIPC_API_H_
#define _OWIPC_API_H_

/* ---- Include Files ----------------------------------------------------- */

#include "omcimib_dts.h"
#include "bcm_ipc_api.h"
#include "omci_msg.h"
#ifdef BRCM_CMS_BUILD
#include "cms_mem.h"
#else /* BRCM_CMS_BUILD */
#include <stdlib.h>
#endif /* BRCM_CMS_BUILD */


/* ---- Constants and Types ----------------------------------------------- */

/* IPC endpoint and queue definitions. */
#define OMCID_ENDPOINT            "/var/tmp/omcid_ep"
#define MCPD_ENDPOINT             "/var/tmp/mcpd_ep"
#define HTTPD_ENDPOINT            "/var/tmp/httpd_ep"
#define CONSOLED_ENDPOINT         "/var/tmp/consoled_ep"

#define OMCI_RX_Q_NAME            "omcid_rx_q"
#define MCPD_TX_Q_NAME            "mcpd_tx_q"
#define HTTPD_TX_Q_NAME           "httpd_tx_q"
#define CONSOLED_TX_Q_NAME        "consoled_tx_q"

/* IPC message related definitions. */
#define OMCI_IPC_OBJ_BUF_MAXLEN      512
#define OMCI_IPC_OBJ_MAXOIDS         128
#define OMCI_IPC_OBJ_MAXNUM          2000
#define OMCI_IPC_OBJ_MAXPARAMS       16
#define OMCI_IPC_FULL_PATH_MAXLEN    256

typedef enum
{
    MCPD_CLIENT,
    HTTPD_CLIENT,
    CONSOLED_CLIENT
} OMCI_IPC_Clients;

typedef enum
{
   OMCI_IPC_RET_SUCCESS = 0,
   OMCI_IPC_RET_ERROR   = -1,
   OMCI_IPC_RET_NOT_FOUND = -2,
   OMCI_IPC_RET_UNSUPPORTED = -3
} OMCI_IPC_RetCode_e;

typedef enum
{
    OMCI_IPC_GPON_LINK_STATUS_CHANGE = 1,
    OMCI_IPC_PM_SYNC_TIME,
    /* Object access. */
    OMCI_IPC_GET_OBJ_REQ = 21,
    OMCI_IPC_GET_OBJ_RSP,
    OMCI_IPC_GET_SUPP_OBJ_INFO_REQ,
    OMCI_IPC_GET_SUPP_OBJ_INFO_RSP,
    OMCI_IPC_GET_OBJ_INFO_REQ,
    OMCI_IPC_GET_OBJ_INFO_RSP,
    OMCI_IPC_GET_PARAM_INFO_REQ,
    OMCI_IPC_GET_PARAM_INFO_RSP,
    OMCI_IPC_CVRT_FP_2_PD_REQ,
    OMCI_IPC_CVRT_FP_2_PD_RSP,
    OMCI_IPC_CVRT_PD_2_FP_REQ,        /* 31 */
    OMCI_IPC_CVRT_PD_2_FP_RSP,
    /* IGMP admission control (IGMP->OMCI). */
    OMCI_IPC_IGMP_ACL_REQ = 41,
    OMCI_IPC_IGMP_ACL_RSP,
    /* MIB reset (OMCI->IGMP). */
    OMCI_IPC_MCPD_MIB_RESET,
} OMCI_IPC_MsgType_e;

typedef struct
{
    UINT32 retCode;
    UINT32 entryNum;
    void   *obj[0];
} omciIpcArrayReplyHdr_t;

typedef struct
{
    MdmObjectId     oid;
    InstanceIdStack iidStack;
} omciMsgObject_t;

typedef struct
{
    UINT32 retCode;
    char   obj[OMCI_IPC_OBJ_BUF_MAXLEN];
} omciMsgGetObjectReply_t;

typedef struct
{
    MdmObjectId oid;
} omciMsgGetParamInfo_t;

typedef struct
{
    char fullPath[OMCI_IPC_FULL_PATH_MAXLEN];
} omciMsgFullPath2PathDesc_t;

typedef struct
{
    UINT32           retCode;
    OmciObjectInfo_t obj;
} omciMsgFullPath2PathDescReply_t;

typedef struct
{
    OmciObjectInfo_t obj;
} omciMsgPathDescr2FullPath_t;

typedef struct
{
    UINT32 retCode;
    char   fullPath[OMCI_IPC_FULL_PATH_MAXLEN];
} omciMsgPathDescr2FullPathReply_t;

typedef struct
{
    UINT32 retCode;
} omciMsgErrorReply_t;

typedef struct
{
    bcmIpcMsg_t ipcMsg;
    UINT32 retCode;
} omciIpcErrRsp_t;


/* ---- Macro API definitions --------------------------------------------- */

/* Utilize the CMS MEM debugging feature when BRCM_CMS_BUILD is enabled. */
#ifdef BRCM_CMS_BUILD
#define omciIpc_alloc(s)  cmsMem_alloc(s, ALLOC_ZEROIZE)
#define omciIpc_free(buf) cmsMem_free(buf)
#else /* BRCM_CMS_BUILD */
#define omciIpc_alloc(s)  malloc(s)
#define omciIpc_free(buf) free(buf)
#endif /* BRCM_CMS_BUILD */


/* ---- Variable Externs -------------------------------------------------- */


/* ---- Function Prototypes ----------------------------------------------- */

/* Client-side APIs. */
int omciIpc_clientInit(OMCI_IPC_Clients Client);
int omciIpc_clientShutdown(void);

int omciIpc_getObject(MdmObjectId oid, const InstanceIdStack *iidStack,
  void **obj);
int omciIpc_getSupportedObjectInfo(OmciObjectInfo_t **objectInfoListPP,
  SINT32 *entryNumP);
int omciIpc_getObjectInfo(OmciObjectInfo_t **objectInfoListPP,
  SINT32 *entryNumP);
int omciIpc_getParamInfo(MdmObjectId oid, OmciParamInfo_t **paramInfoListPP,
  SINT32 *entryNumP);
int omciIpc_pathDescriptorToFullPath(const OmciObjectInfo_t *pathDescP,
  char **fullpathP);
int omciIpc_fullPathToPathDescriptor(const char *fullpathP,
  OmciObjectInfo_t *pathDescP);
int omciIpc_sendIgmpAdmissionControl(const OmciIgmpMsgBody *igmpReqP,
  int *retCodeP);
bcmIpcMsgQueue_t *omciIpc_getQ(void);
UINT32 omciIpc_getQueueFd(void);


#endif /* OWIPC_API_H_*/
