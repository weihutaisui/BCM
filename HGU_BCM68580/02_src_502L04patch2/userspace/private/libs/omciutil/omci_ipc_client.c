/***********************************************************************
 *
 *  Copyright (c) 2017 Broadcom
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
*      OMCI Inter Process Communication client API implementation.
*
*****************************************************************************/

/* ---- Include Files ----------------------------------------------------- */

#include "omci_ipc.h"


/* ---- Private Constants and Types --------------------------------------- */

#define MSECS_IN_SEC 1000
#define IPC_RX_TIMEOUT 30000 /* ms */


/* ---- Macro API definitions --------------------------------------------- */

#define _LOG_NAME "omci_ipcc"

#define OMCI_IPC_CLIENT_ERROR
/* #define OMCI_IPC_CLIENT_DEBUG */

#ifdef OMCI_IPC_CLIENT_ERROR
#define omciIpc_logError(fmt, arg...)     \
  printf("[ERROR " "%s" "] %-10s, %d: " fmt "\n",  \
  _LOG_NAME, __FUNCTION__, __LINE__, ##arg);
#else
#define omciIpc_logError(fmt, arg...)
#endif

#ifdef OMCI_IPC_CLIENT_DEBUG
#define omciIpc_logInfo(fmt, arg...)      \
    printf("[INFO " "%s" "] %-10s, %d: " fmt "\n", \
    _LOG_NAME, __FUNCTION__, __LINE__, ##arg);
#else
#define omciIpc_logInfo(fmt, arg...)
#endif


/* ---- Private Function Prototypes --------------------------------------- */


/* ---- Public Variables -------------------------------------------------- */

bcmIpcMsg_t* omciIpc_keepReading(bcmIpcMsgQueue_t *clientQp,
  bcmIpcMsgId_t expectedMsgType);

/* ---- Private Variables ------------------------------------------------- */

static bcmIpcMsgQueue_t clientQ;
static bcmIpcModuleId_t sender = -1;
static char* clientIdToName[] =
{
    "MCPD",
    "HTTPD",
    "CONSOLED"
};


/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  omciIpc_clientInit
*  PURPOSE:   IPC client initialization.
*  PARAMETERS:
*      client - IPC client ID.
*  RETURNS:
*      0 - success.
*      < 0 - failed operation.
*  NOTES:
*      An IPC client must call this function before sending or receiving
*      any IPC messages.
*****************************************************************************/
int omciIpc_clientInit(OMCI_IPC_Clients client)
{
    int ret = 0;
    bcmIpcMsgQueueParam_t clientQueueParam;

    memset(&clientQueueParam, 0x0, sizeof(bcmIpcMsgQueueParam_t));
    sender = client;

    switch (client)
    {
    case MCPD_CLIENT:
         (void)unlink(MCPD_ENDPOINT);
         clientQueueParam.name = MCPD_TX_Q_NAME;
         clientQueueParam.local_ep_address = MCPD_ENDPOINT;
         break;

    case HTTPD_CLIENT:
         (void)unlink(HTTPD_ENDPOINT);
         clientQueueParam.name = HTTPD_TX_Q_NAME;
         clientQueueParam.local_ep_address = HTTPD_ENDPOINT;
         break;

    case CONSOLED_CLIENT:
         (void)unlink(CONSOLED_ENDPOINT);
         clientQueueParam.name = CONSOLED_TX_Q_NAME;
         clientQueueParam.local_ep_address = CONSOLED_ENDPOINT;
         break;

    default:
        omciIpc_logError("Unknown client code %d -- queue not created\n",
          client);
        return -1;
    }

    clientQueueParam.ep_type = BCM_IPC_MSG_QUEUE_EP_DOMAIN_SOCKET;
    clientQueueParam.remote_ep_address = OMCID_ENDPOINT;

    omciIpc_logInfo("Client %s queue initialization",
      clientIdToName[client]);

    ret = bcm_ipc_msg_queue_create(&clientQ, &clientQueueParam);
    if (ret)
    {
        omciIpc_logError("Client %s queue initialization failed, ret=%d",
          clientIdToName[client], ret);
        return ret;
    }

    omciIpc_logInfo("Client %s queue initialization DONE",
      clientIdToName[client]);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_clientShutdown
*  PURPOSE:   IPC client shutdown.
*  PARAMETERS:
*      None.
*  RETURNS:
*      0 - success.
*      < 0 - failed operation.
*  NOTES:
*      An IPC client may call this function when the client process exits.
*****************************************************************************/
int omciIpc_clientShutdown(void)
{
    int ret = 0;

    ret = bcm_ipc_msg_queue_destroy(&clientQ);
    if (ret)
    {
        omciIpc_logError("IPC client queue destruction failed, ret=%d",
          ret);
        return ret;
    }

    omciIpc_logInfo("IPC client queue destroyed");

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_getObject
*  PURPOSE:   Get OMCI object instance of specified oid and instance.
*  PARAMETERS:
*      oid - OMCI object ID.
*      iidStack - pointer to the instance stack.
*      objPP - pointer to the returned object instance.
*  RETURNS:
*      0 - success.
*      < 0 - failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_getObject(MdmObjectId oid, const InstanceIdStack *iidStack,
  void **objPP)
{
    int ret = 0;
    int ipcRc;
    bcmIpcMsg_t *msgReqP;
    bcmIpcMsg_t *msgRspP;
    omciMsgObject_t *reqDataP;
    omciMsgGetObjectReply_t *rspDataP;
    bcmIpcMsgQueue_t *clientQp;
    void *objP = NULL;

    msgReqP = (bcmIpcMsg_t*)malloc(sizeof(bcmIpcMsg_t) +
      sizeof(omciMsgObject_t));
    if (msgReqP == NULL)
    {
        omciIpc_logError("malloc() failed");
        return -1;
    }

    /* Fill the request message. */
    reqDataP = (omciMsgObject_t*)((char*)msgReqP + sizeof(bcmIpcMsg_t));
    reqDataP->oid = oid;
    memcpy(&reqDataP->iidStack, iidStack, sizeof(InstanceIdStack));

    msgReqP->type = OMCI_IPC_GET_OBJ_REQ;
    msgReqP->size = sizeof(omciMsgObject_t);
    msgReqP->send_flags = 0;
    msgReqP->data = (void*)reqDataP;
    msgReqP->sender = sender;

    clientQp = omciIpc_getQ();

    if ((ipcRc = bcm_ipc_msg_send_std(clientQp, msgReqP)) != 0)
    {
        omciIpc_logError("Send OMCI_IPC_GET_OBJ_REQ failed, ret=%d",
          ipcRc);
        return -1;
    }
    else
    {
        omciIpc_logInfo("Sent out OMCI_IPC_GET_OBJ_REQ");
    }

    ipcRc = bcm_ipc_msg_recv(clientQp, IPC_RX_TIMEOUT, &msgRspP);
    if (ipcRc <= 0)
    {
        omciIpc_logInfo("Recv OMCI_IPC_GET_OBJ_RSP failed, ret=%d",
          ipcRc);
        return -1;
    }

    if (msgRspP->type != OMCI_IPC_GET_OBJ_RSP)
    {
        free(msgRspP);
        msgRspP = omciIpc_keepReading(clientQp, OMCI_IPC_GET_OBJ_RSP);
        if (msgRspP == NULL)
        {
            omciIpc_logError("OMCI_IPC_GET_OBJ_RSP failed");
            return -1;
        }
    }

    rspDataP = (omciMsgGetObjectReply_t*)msgRspP->data;
    if (rspDataP->retCode != OMCI_IPC_RET_SUCCESS)
    {
        omciIpc_logError("OMCI_IPC_GET_OBJ_RSP failed, "
          "ret=%d.", rspDataP->retCode);
        free(msgRspP);
        return -1;
    }

    objP = omciIpc_alloc(OMCI_IPC_OBJ_BUF_MAXLEN);
    if (objP == NULL)
    {
        omciIpc_logError("malloc() failed");
        free(msgRspP);
        return -1;
    }

    omciIpc_logInfo("OMCI_IPC_GET_OBJ_RSP OK.");

    memcpy(objP, &rspDataP->obj, OMCI_IPC_OBJ_BUF_MAXLEN);
    *objPP = objP;
    free(msgRspP);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_getSupportedObjectInfo
*  PURPOSE:   Get the list of supported OMCI objects, object instances are
*             excluded.
*  PARAMETERS:
*      objectInfoListPP - pointer to the obtained list of objects.
*      entryNumP - pointer to obtained number of entries.
*  RETURNS:
*      0 - success.
*      < 0 - failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_getSupportedObjectInfo(OmciObjectInfo_t **objectInfoListPP,
  SINT32 *entryNumP)
{
    int ret = 0;
    int ipcRc;
    bcmIpcMsg_t *msgReqP;
    bcmIpcMsg_t *msgRspP;
    bcmIpcMsgQueue_t *clientQp;
    omciIpcArrayReplyHdr_t *rspDataP;
    OmciObjectInfo_t *rspEntryP = NULL;
    OmciObjectInfo_t *clientMeListP = NULL;
    OmciObjectInfo_t *omciEntryP = NULL;
    UINT32 entryI;

    msgReqP = (bcmIpcMsg_t*)malloc(sizeof(bcmIpcMsg_t));
    if (msgReqP == NULL)
    {
        omciIpc_logError("malloc() failed");
        return -1;
    }

    /* Fill the request message. */
    msgReqP->type = OMCI_IPC_GET_SUPP_OBJ_INFO_REQ;
    msgReqP->size = 0;
    msgReqP->send_flags = 0;
    msgReqP->data = NULL;
    msgReqP->sender = sender;

    clientQp = omciIpc_getQ();

    if ((ipcRc = bcm_ipc_msg_send_std(clientQp, msgReqP)) != 0)
    {
        omciIpc_logError("Send OMCI_IPC_GET_SUPP_OBJ_INFO_REQ failed, ret=%d",
          ipcRc);
        return -1;
    }
    else
    {
        omciIpc_logInfo("Sent out OMCI_IPC_GET_SUPP_OBJ_INFO_REQ");
    }

    ipcRc = bcm_ipc_msg_recv(clientQp, IPC_RX_TIMEOUT, &msgRspP);
    if (ipcRc <= 0)
    {
        omciIpc_logInfo("Recv OMCI_IPC_GET_SUPP_OBJ_INFO_RSP failed, ret=%d",
          ipcRc);
        return -1;
    }

    if (msgRspP->type != OMCI_IPC_GET_SUPP_OBJ_INFO_RSP)
    {
        free(msgRspP);
        msgRspP = omciIpc_keepReading(clientQp, OMCI_IPC_GET_SUPP_OBJ_INFO_RSP);
        if (msgRspP == NULL)
        {
            omciIpc_logError("OMCI_IPC_GET_SUPP_OBJ_INFO_RSP failed");
            return -1;
        }
    }

    rspDataP = (omciIpcArrayReplyHdr_t*)msgRspP->data;
    if (rspDataP->entryNum == 0)
    {
        omciIpc_logError("OMCI_IPC_GET_SUPP_OBJ_INFO_RSP failed, "
          "rx msg type=%d, entryNum=%d",
          msgRspP->type, rspDataP->entryNum);
        free(msgRspP);
        return -1;
    }

    if (rspDataP->retCode != OMCI_IPC_RET_SUCCESS)
    {
        omciIpc_logError("OMCI_IPC_GET_SUPP_OBJ_INFO_RSP failed, "
          "ret=%d.", rspDataP->retCode);
        free(msgRspP);
        return -1;
    }

    omciIpc_logInfo("OMCI_IPC_GET_SUPP_OBJ_INFO_RSP OK.");

    clientMeListP = omciIpc_alloc(rspDataP->entryNum * sizeof(OmciObjectInfo_t));
    if (clientMeListP == NULL)
    {
        omciIpc_logError("omciIpc_alloc() failed, num=%d",
          rspDataP->entryNum);
        free(msgRspP);
        return -1;
    }

    omciEntryP = clientMeListP;
    rspEntryP = (OmciObjectInfo_t*)&rspDataP->obj[0];
    for (entryI = 0; entryI < rspDataP->entryNum; entryI++)
    {
        omciEntryP->oid = rspEntryP->oid;
        memcpy(&omciEntryP->iidStack, &rspEntryP->iidStack,
          sizeof(InstanceIdStack));
        omciEntryP++;
        rspEntryP++;
    }

    *objectInfoListPP = clientMeListP;
    *entryNumP = rspDataP->entryNum;

    omciIpc_logInfo("%p, %d\n", objectInfoListPP, *entryNumP);
    free(msgRspP);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_getObjectInfo
*  PURPOSE:   Get the list of OMCI object instances, excluding place holders.
*             Created object instances are included.
*  PARAMETERS:
*      objectInfoListPP - pointer to the obtained list of object instances.
*      entryNumP - pointer to obtained number of entries.
*  RETURNS:
*      0 - success.
*      < 0 - failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_getObjectInfo(OmciObjectInfo_t **objectInfoListPP,
  SINT32 *entryNumP)
{
    int ipcRc;
    bcmIpcMsg_t *msgReqP;
    bcmIpcMsg_t *msgRspP;
    bcmIpcMsgQueue_t *clientQp;
    omciIpcArrayReplyHdr_t *rspDataP;
    OmciObjectInfo_t *clientMeListP = NULL;
    OmciObjectInfo_t *omciEntryP = NULL;
    omciMsgObject_t *rspEntryP = NULL;
    UINT32 entryI;

    msgReqP = (bcmIpcMsg_t*)malloc(sizeof(bcmIpcMsg_t));
    if (msgReqP == NULL)
    {
        omciIpc_logError("malloc() failed");
        return -1;
    }

    /* Fill the request message. */
    msgReqP->type = OMCI_IPC_GET_OBJ_INFO_REQ;
    msgReqP->size = 0;
    msgReqP->send_flags = 0;
    msgReqP->data = NULL;
    msgReqP->sender = sender;

    clientQp = omciIpc_getQ();

    if ((ipcRc = bcm_ipc_msg_send_std(clientQp, msgReqP)) != 0)
    {
        omciIpc_logError("Send OMCI_IPC_GET_OBJ_INFO_REQ failed, ret=%d",
          ipcRc);
        return -1;
    }
    else
    {
        omciIpc_logInfo("Sent out OMCI_IPC_GET_OBJ_INFO_REQ");
    }

    ipcRc = bcm_ipc_msg_recv(clientQp, IPC_RX_TIMEOUT, &msgRspP);
    if (ipcRc <= 0)
    {
        omciIpc_logInfo("Recv OMCI_IPC_GET_OBJ_INFO_RSP failed, ret=%d",
          ipcRc);
        return -1;
    }

    if (msgRspP->type != OMCI_IPC_GET_OBJ_INFO_RSP)
    {
        free(msgRspP);
        msgRspP = omciIpc_keepReading(clientQp, OMCI_IPC_GET_OBJ_INFO_RSP);
        if (msgRspP == NULL)
        {
            omciIpc_logError("OMCI_IPC_GET_OBJ_INFO_RSP failed");
            return -1;
        }
    }

    rspDataP = (omciIpcArrayReplyHdr_t*)msgRspP->data;
    if (rspDataP->entryNum == 0)
    {
        omciIpc_logError("OMCI_IPC_GET_OBJ_INFO_RSP failed, "
          "rx msg type=%d, entryNum=%d",
          msgRspP->type, rspDataP->entryNum);
        free(msgRspP);
        return -1;
    }

    if (rspDataP->retCode != OMCI_IPC_RET_SUCCESS)
    {
        omciIpc_logError("OMCI_IPC_GET_OBJ_INFO_RSP failed, "
          "ret=%d.", rspDataP->retCode);
        free(msgRspP);
        return -1;
    }

    omciIpc_logInfo("OMCI_IPC_GET_OBJ_INFO_RSP OK.");

    clientMeListP = omciIpc_alloc(rspDataP->entryNum * sizeof(OmciObjectInfo_t));
    if (clientMeListP == NULL)
    {
        omciIpc_logError("omciIpc_alloc() failed, num=%d",
          rspDataP->entryNum);
        free(msgRspP);
        return -1;
    }

    omciEntryP = clientMeListP;
    rspEntryP = (omciMsgObject_t*)&rspDataP->obj[0];
    for (entryI = 0; entryI < rspDataP->entryNum; entryI++)
    {
        omciEntryP->oid = rspEntryP->oid;
        memcpy(&omciEntryP->iidStack, &rspEntryP->iidStack,
          sizeof(InstanceIdStack));
        omciEntryP++;
        rspEntryP++;
    }

    *objectInfoListPP = clientMeListP;
    *entryNumP = rspDataP->entryNum;

    omciIpc_logInfo("%p, %d\n", objectInfoListPP, *entryNumP);
    free(msgRspP);

    return 0;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_getParamInfo
*  PURPOSE:   Get the list of OMCI attributes (not the actual values).
*  PARAMETERS:
*      oid - OID.
*      paramInfoListPP - pointer to the obtained list of parameters.
*      entryNumP - pointer to obtained number of entries.
*  RETURNS:
*      0 - success.
*      < 0 - failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_getParamInfo(MdmObjectId oid, OmciParamInfo_t
  **paramInfoListPP, SINT32 *entryNumP)
{
    int ipcRc;
    bcmIpcMsg_t *msgReqP;
    bcmIpcMsg_t *msgRspP;
    bcmIpcMsgQueue_t *clientQp;
    omciMsgGetParamInfo_t *reqDataP;
    omciIpcArrayReplyHdr_t *rspDataP;
    OmciParamInfo_t *clientParamInfoListP = NULL;
    OmciParamInfo_t *omciEntryP = NULL;
    OmciParamInfo_t *rspEntryP = NULL;
    UINT32 entryI;

    msgReqP = (bcmIpcMsg_t*)malloc(sizeof(bcmIpcMsg_t) +
      sizeof(omciMsgGetParamInfo_t));
    if (msgReqP == NULL)
    {
        omciIpc_logError("malloc() failed");
        return -1;
    }

    omciIpc_logInfo("OMCI_IPC_GET_PARAM_INFO_REQ oid=%d.", oid);
    /* Fill the request message. */
    reqDataP = (omciMsgGetParamInfo_t*)((char*)msgReqP + sizeof(bcmIpcMsg_t));
    reqDataP->oid = oid;

    msgReqP->type = OMCI_IPC_GET_PARAM_INFO_REQ;
    msgReqP->size = sizeof(omciMsgGetParamInfo_t);
    msgReqP->send_flags = 0;
    msgReqP->data = reqDataP;
    msgReqP->sender = sender;

    clientQp = omciIpc_getQ();

    if ((ipcRc = bcm_ipc_msg_send_std(clientQp, msgReqP)) != 0)
    {
        omciIpc_logError("Send OMCI_IPC_GET_PARAM_INFO_REQ failed, ret=%d",
          ipcRc);
        return -1;
    }
    else
    {
        omciIpc_logInfo("Sent out OMCI_IPC_GET_PARAM_INFO_REQ");
    }

    ipcRc = bcm_ipc_msg_recv(clientQp, IPC_RX_TIMEOUT, &msgRspP);
    if (ipcRc <= 0)
    {
        omciIpc_logInfo("Recv OMCI_IPC_GET_OBJ_INFO_RSP failed, ret=%d",
          ipcRc);
        return -1;
    }

    if (msgRspP->type != OMCI_IPC_GET_PARAM_INFO_RSP)
    {
        free(msgRspP);
        msgRspP = omciIpc_keepReading(clientQp, OMCI_IPC_GET_PARAM_INFO_RSP);
        if (msgRspP == NULL)
        {
            omciIpc_logError("OMCI_IPC_GET_PARAM_INFO_RSP failed");
            return -1;
        }
    }

    rspDataP = (omciIpcArrayReplyHdr_t*)msgRspP->data;

    if (rspDataP->entryNum == 0)
    {
        omciIpc_logError("OMCI_IPC_GET_PARAM_INFO_RSP failed, "
          "rx msg type=%d, entryNum=%d",
          msgRspP->type, rspDataP->entryNum);
        free(msgRspP);
        return -1;
    }

    if (rspDataP->retCode != OMCI_IPC_RET_SUCCESS)
    {
        omciIpc_logError("OMCI_IPC_GET_PARAM_INFO_RSP failed, "
          "ret=%d.", rspDataP->retCode);
        free(msgRspP);
        return -1;
    }

    omciIpc_logInfo("OMCI_IPC_GET_PARAM_INFO_RSP OK.");

    clientParamInfoListP = omciIpc_alloc(rspDataP->entryNum *
      sizeof(OmciParamInfo_t));
    if (clientParamInfoListP == NULL)
    {
        omciIpc_logError("omciIpc_alloc() failed, num=%d",
          rspDataP->entryNum);
        free(msgRspP);
        return -1;
    }

    omciEntryP = clientParamInfoListP;
    rspEntryP = (OmciParamInfo_t*)&rspDataP->obj[0];
    for (entryI = 0; entryI < rspDataP->entryNum; entryI++)
    {
        memcpy(omciEntryP, rspEntryP, sizeof(OmciParamInfo_t));
        omciEntryP++;
        rspEntryP++;
    }

    *paramInfoListPP = clientParamInfoListP;
    *entryNumP = rspDataP->entryNum;

    omciIpc_logInfo("%p, %d\n", clientParamInfoListP, *entryNumP);
    free(msgRspP);

    return 0;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_fullPathToPathDescriptor
*  PURPOSE:   Convert a full path string to pathDescriptor.
*  PARAMETERS:
*      fullpath - pointer to the full path string.
*      omciMeP - pointer to the path descriptor.
*  RETURNS:
*      0 - success.
*      < 0 - failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_fullPathToPathDescriptor(const char *fullpath,
  OmciObjectInfo_t *pathDesc)
{
    int ipcRc;
    bcmIpcMsg_t *msgReqP;
    omciMsgFullPath2PathDesc_t *reqDataP;
    bcmIpcMsg_t *msgRspP;
    omciMsgFullPath2PathDescReply_t *rspDataP;
    bcmIpcMsgQueue_t *clientQp;

    msgReqP = (bcmIpcMsg_t*)malloc(sizeof(bcmIpcMsg_t) +
      sizeof(omciMsgFullPath2PathDesc_t));
    if (msgReqP == NULL)
    {
        omciIpc_logError("malloc() failed");
        return -1;
    }

    /* Fill the request message. */
    reqDataP = (omciMsgFullPath2PathDesc_t*)((char*)msgReqP + sizeof(bcmIpcMsg_t));
    strncpy(reqDataP->fullPath, fullpath, OMCI_IPC_FULL_PATH_MAXLEN - 1);

    msgReqP->type = OMCI_IPC_CVRT_FP_2_PD_REQ;
    msgReqP->size = sizeof(omciMsgFullPath2PathDesc_t);
    msgReqP->send_flags = 0;
    msgReqP->data = (void*)reqDataP;
    msgReqP->sender = sender;

    clientQp = omciIpc_getQ();

    if ((ipcRc = bcm_ipc_msg_send_std(clientQp, msgReqP)) != 0)
    {
        omciIpc_logError("Send OMCI_IPC_CVRT_FP_2_PD_REQ failed, ret=%d",
          ipcRc);
        return -1;
    }
    else
    {
        omciIpc_logInfo("Sent out OMCI_IPC_CVRT_FP_2_PD_REQ");
    }

    ipcRc = bcm_ipc_msg_recv(clientQp, IPC_RX_TIMEOUT, &msgRspP);
    if (ipcRc <= 0)
    {
        omciIpc_logInfo("OMCI_IPC_CVRT_FP_2_PD_RSP failed, ret=%d",
          ipcRc);
        return -1;
    }

    if (msgRspP->type != OMCI_IPC_CVRT_FP_2_PD_RSP)
    {
        free(msgRspP);
        msgRspP = omciIpc_keepReading(clientQp, OMCI_IPC_CVRT_FP_2_PD_RSP);
        if (msgRspP == NULL)
        {
            omciIpc_logError("OMCI_IPC_CVRT_FP_2_PD_RSP failed");
            return -1;
        }
    }

    rspDataP = (omciMsgFullPath2PathDescReply_t*)msgRspP->data;
    if (rspDataP->retCode != OMCI_IPC_RET_SUCCESS)
    {
        omciIpc_logError("OMCI_IPC_CVRT_FP_2_PD_RSP failed, "
          "ret=%d.", rspDataP->retCode);
        free(msgRspP);
        return -1;
    }

    memcpy(pathDesc, &rspDataP->obj, sizeof(OmciObjectInfo_t));

    omciIpc_logInfo("OMCI_IPC_CVRT_FP_2_PD_RSP OK.");
    free(msgRspP);

    return 0;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_pathDescriptorToFullPath
*  PURPOSE:   Convert pathDescriptor to full path string.
*    Example:
*     Path descriptor:
*       pathDesc.oid = MDMOID_SOFTWARE_IMAGE
*       pathDesc.iidStack.currentDepth = 1
*       pathDesc.iidStack.instance[0] = 2
*       pathDesc.objectName = "SoftwareImage"
*    Full path:
*     InternetGatewayDevice.X_ITU_T_ORG.G_984_4.EquipmentManagement.SoftwareImage.{i}
*  PARAMETERS:
*      pathDesc - pointer to the path descriptor.
*      fullpath - pointer to the full path string.
*  RETURNS:
*      0 - success.
*      < 0 - failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_pathDescriptorToFullPath(const OmciObjectInfo_t *pathDesc,
  char **fullpath)
{
    int ipcRc;
    bcmIpcMsg_t *msgReqP;
    omciMsgPathDescr2FullPath_t *reqDataP;
    bcmIpcMsg_t *msgRspP;
    omciMsgPathDescr2FullPathReply_t *rspDataP;
    bcmIpcMsgQueue_t *clientQp;
    char *fullPathStr = NULL;

    msgReqP = (bcmIpcMsg_t*)malloc(sizeof(bcmIpcMsg_t) +
      sizeof(omciMsgPathDescr2FullPath_t));
    if (msgReqP == NULL)
    {
        omciIpc_logError("malloc() failed");
        return -1;
    }

    /* Fill the request message. */
    reqDataP = (omciMsgPathDescr2FullPath_t*)((char*)msgReqP + sizeof(bcmIpcMsg_t));
    memcpy(&(reqDataP->obj), pathDesc, sizeof(OmciObjectInfo_t));

    msgReqP->type = OMCI_IPC_CVRT_PD_2_FP_REQ;
    msgReqP->size = sizeof(omciMsgPathDescr2FullPath_t);
    msgReqP->send_flags = 0;
    msgReqP->data = (void*)reqDataP;
    msgReqP->sender = sender;

    clientQp = omciIpc_getQ();

    if ((ipcRc = bcm_ipc_msg_send_std(clientQp, msgReqP)) != 0)
    {
        omciIpc_logError("Send OMCI_IPC_CVRT_PD_2_FP_REQ failed, ret=%d.",
          ipcRc);
        return -1;
    }
    else
    {
        omciIpc_logInfo("Sent out OMCI_IPC_CVRT_PD_2_FP_REQ.");
    }

    ipcRc = bcm_ipc_msg_recv(clientQp, IPC_RX_TIMEOUT, &msgRspP);
    if (ipcRc <= 0)
    {
        omciIpc_logError("OMCI_IPC_CVRT_PD_2_FP_RSP failed, ret=%d",
          ipcRc);
        return -1;
    }

    if (msgRspP->type != OMCI_IPC_CVRT_PD_2_FP_RSP)
    {
        free(msgRspP);
        msgRspP = omciIpc_keepReading(clientQp, OMCI_IPC_CVRT_PD_2_FP_RSP);
        if (msgRspP == NULL)
        {
            omciIpc_logError("OMCI_IPC_CVRT_PD_2_FP_RSP failed");
            return -1;
        }
    }

    rspDataP = (omciMsgPathDescr2FullPathReply_t*)msgRspP->data;
    if (rspDataP->retCode != OMCI_IPC_RET_SUCCESS)
    {
        omciIpc_logError("OMCI_IPC_CVRT_PD_2_FP_RSP failed, "
          "ret=%d.", rspDataP->retCode);
        free(msgRspP);
        return -1;
    }

    fullPathStr = omciIpc_alloc(OMCI_IPC_FULL_PATH_MAXLEN);
    if (fullPathStr == NULL)
    {
        omciIpc_logError("malloc() failed");
        free(msgRspP);
        return -1;
    }

    strncpy(fullPathStr, rspDataP->fullPath, OMCI_IPC_FULL_PATH_MAXLEN);
    *fullpath = fullPathStr;

    omciIpc_logInfo("Recv OMCI_IPC_CVRT_PD_2_FP_RSP OK, %s", fullPathStr);
    free(msgRspP);

    return 0;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_sendIgmpAdmissionControl
*  PURPOSE:   Send IGMP admission control request to OMCI.
*  PARAMETERS:
*      igmpReqP - pointer to the IGMP admission control request.
*      retCodeP - pointer to the return code.
*  RETURNS:
*      0 - success.
*      < 0 - failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_sendIgmpAdmissionControl(const OmciIgmpMsgBody *igmpReqP,
  int *retCodeP)
{
    int ipcRc;
    bcmIpcMsg_t *msgReqP;
    bcmIpcMsg_t *msgRspP;
    OmciIgmpMsgBody *reqDataP;
    bcmIpcMsgQueue_t *clientQp;
    omciMsgErrorReply_t *rspDataP;

    msgReqP = (bcmIpcMsg_t*)malloc(sizeof(bcmIpcMsg_t) +
      sizeof(OmciIgmpMsgBody));

    /* Fill the request message. */
    reqDataP = (OmciIgmpMsgBody*)((char*)msgReqP + sizeof(bcmIpcMsg_t));
    memcpy(reqDataP, igmpReqP, sizeof(OmciIgmpMsgBody));

    msgReqP->type = OMCI_IPC_IGMP_ACL_REQ;
    msgReqP->size = sizeof(OmciIgmpMsgBody);
    msgReqP->send_flags = 0;

    msgReqP->data = reqDataP;
    msgReqP->sender = sender;

    clientQp = omciIpc_getQ();

    if ((ipcRc = bcm_ipc_msg_send_std(clientQp, msgReqP)) != 0)
    {
        omciIpc_logError("Send OMCI_IPC_IGMP_ACL_REQ failed, ret=%d.",
          ipcRc);
        return -1;
    }
    else
    {
        omciIpc_logInfo("Sent out OMCI_IPC_IGMP_ACL_REQ.");
    }

    ipcRc = bcm_ipc_msg_recv(clientQp, IPC_RX_TIMEOUT, &msgRspP);
    if (ipcRc <= 0)
    {
        omciIpc_logInfo("Recv OMCI_IPC_IGMP_ACL_RSP failed, ret=%d",
          ipcRc);
        return -1;
    }

    if (msgRspP->type != OMCI_IPC_IGMP_ACL_RSP)
    {
        free(msgRspP);
        msgRspP = omciIpc_keepReading(clientQp, OMCI_IPC_IGMP_ACL_RSP);
        if (msgRspP == NULL)
        {
            omciIpc_logError("OMCI_IPC_IGMP_ACL_RSP failed");
            return -1;
        }
    }

    rspDataP = (omciMsgErrorReply_t*)msgRspP->data;
    *retCodeP = rspDataP->retCode;

    omciIpc_logInfo("OMCI_IPC_IGMP_ACL_RSP OK.");
    free(msgRspP);

    return 0;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_getQ
*  PURPOSE:   Get the IPC client queue handler.
*  PARAMETERS:
*      None.
*  RETURNS:
*      Pointer to the client queue.
*  NOTES:
*      None.
*****************************************************************************/
bcmIpcMsgQueue_t *omciIpc_getQ(void)
{
    return &clientQ;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_getQueueFd
*  PURPOSE:   Get the IPC client queue fd.
*  PARAMETERS:
*      None.
*  RETURNS:
*      Client queue fd.
*  NOTES:
*      None.
*****************************************************************************/
UINT32 omciIpc_getQueueFd(void)
{
    return (UINT32)clientQ.ep;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_keepReading
*  PURPOSE:   Keep reading the queue in case of REQ/RSP mismatch.
*  PARAMETERS:
*      clientQp - pointer to the client queue.
*      expectedMsgType - expected message type.
*  RETURNS:
*      A valid response message or NULL.
*  NOTES:
*      None.
*****************************************************************************/
bcmIpcMsg_t* omciIpc_keepReading(bcmIpcMsgQueue_t *clientQp,
  bcmIpcMsgId_t expectedMsgType)
{
    bcmIpcMsg_t *msgRspP;

    while (bcm_ipc_msg_recv(clientQp, 0, &msgRspP) > 0)
    {
        if (msgRspP->type != expectedMsgType)
        {
            omciIpc_logError("Get expected msg type %d failed, rx msg type=%d.",
              expectedMsgType, msgRspP->type);
            free(msgRspP);
        }
        else
        {
            return msgRspP;
        }
    };

    return NULL;
}
