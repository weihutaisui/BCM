/***********************************************************************
 *
 *  Copyright (c) 2017 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2017:proprietary:omcid

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
*      Inter Process Communication utilities for OMCI daemon.
*
*****************************************************************************/

/* ---- Include Files ----------------------------------------------------- */

#include "mdmlite_api.h"
#include "owsvc_api.h"
#include "omcid_ipc_priv.h"
#include "omci_ipc.h"
#include "omcid_priv.h"
#include "rut_gpon_mcast.h"


/* ---- Private Constants and Types --------------------------------------- */


/* ---- Macro API definitions --------------------------------------------- */


/* ---- Private Function Prototypes --------------------------------------- */

static bcmIpcMsgQueue_t *omciIpc_getReplyQ(bcmIpcMsg_t *msgReq);
static void omciIpc_sendErrorRsp(bcmIpcMsgQueue_t *replyQ, bcmIpcMsgId_t
  msgId, SINT32 retCode);


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */

static bcmIpcMsgQueue_t omcidQ;
static bcmIpcMsgQueue_t mcpdQ;
static bcmIpcMsgQueue_t httpdQ;
static bcmIpcMsgQueue_t consoledQ;


/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  omci_ipc_init
*  PURPOSE:   OMCI IPC initialization.
*  PARAMETERS:
*      None.
*  RETURNS:
*      < 0 for failed operation.
*  NOTES:
*      None.
*      The IPC is based on the queue abstraction where queue is an entity
*      connecting two endpoints: local and remote.
*      Since OMCID is built around "many-to-one" communication paradigm, IPC
*      queues are used in this way:
*        (RX) queue with valid local endpoint and NULL remote endpoint is used
*          to receive messages addressed to OMCID.
*        N (TX) queues with NULL local endpoint and valid remote endpoint will
*          be used to communicate with MCPD, CLID, HTTPD etc.
*      Addition of the client will require definition of yet another queue,
*      but since the number of clients is limited this would not be a
*      serious disadvantage.
*
*      Each client defines a queue where:
*        Local endpoint matches that used by OMCId Tx queue; remote endpoint
*        is that of OMCId Rx queue.
*****************************************************************************/
int omci_ipc_init(void)
{
    int ret = 0;
    bcmIpcMsgQueueParam_t omciQueueParam;
    bcmIpcMsgQueueParam_t clientQueueParam;

    (void)unlink(OMCID_ENDPOINT);
    memset(&omciQueueParam, 0, sizeof(omciQueueParam));
    memset(&clientQueueParam, 0, sizeof(clientQueueParam));

    omciQueueParam.name = OMCI_RX_Q_NAME;
    omciQueueParam.ep_type = BCM_IPC_MSG_QUEUE_EP_DOMAIN_SOCKET;
    omciQueueParam.local_ep_address = OMCID_ENDPOINT;
    omciQueueParam.remote_ep_address = NULL;

    ret = bcm_ipc_msg_queue_create(&omcidQ, &omciQueueParam);
    if (ret)
    {
        cmsLog_error("OMCID queue initialization failed, ret=%d", ret);
        return ret;
    }

    clientQueueParam.name = MCPD_TX_Q_NAME;
    clientQueueParam.ep_type = BCM_IPC_MSG_QUEUE_EP_DOMAIN_SOCKET;
    clientQueueParam.local_ep_address = NULL;
    clientQueueParam.remote_ep_address = MCPD_ENDPOINT;

    ret = bcm_ipc_msg_queue_create(&mcpdQ, &clientQueueParam);
    if (ret)
    {
        cmsLog_error("MCPD-OMCID queue initialization failed, ret=%d", ret);
        return ret;
    }

    clientQueueParam.name = HTTPD_TX_Q_NAME;
    clientQueueParam.ep_type = BCM_IPC_MSG_QUEUE_EP_DOMAIN_SOCKET;
    clientQueueParam.local_ep_address = NULL;
    clientQueueParam.remote_ep_address = HTTPD_ENDPOINT;

    ret = bcm_ipc_msg_queue_create(&httpdQ, &clientQueueParam);
    if (ret)
    {
        cmsLog_error("HTTPD-OMCID queue initialization failed, ret=%d", ret);
        return ret;
    }

    clientQueueParam.name = CONSOLED_TX_Q_NAME;
    clientQueueParam.ep_type = BCM_IPC_MSG_QUEUE_EP_DOMAIN_SOCKET;
    clientQueueParam.local_ep_address = NULL;
    clientQueueParam.remote_ep_address = CONSOLED_ENDPOINT;

    ret = bcm_ipc_msg_queue_create(&consoledQ, &clientQueueParam);
    if (ret)
    {
        cmsLog_error("CONSOLED-OMCID queue initialization failed, ret=%d", ret);
        return ret;
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_mcpd_send_reply_handler
*  PURPOSE:   Perform OMCI admission control and send response back to MCPD.
*  PARAMETERS:
*      msgReq - pointer to the received IPC message.
*  RETURNS:
*      < 0 for failed operation.
*  NOTES:
*      None.
*****************************************************************************/
static int omciIpc_mcpd_send_reply_handler(bcmIpcMsg_t *msgReq)
{
    bcmIpcMsgQueue_t *replyQ;
    CmsRet cmsRet = CMSRET_SUCCESS;
    OmciIgmpMsgBody *msgBody = (OmciIgmpMsgBody*)(msgReq->data);

    replyQ = omciIpc_getReplyQ(msgReq);
    if (replyQ == NULL)
    {
        cmsLog_error("omciIpc_getReplyQ() failed");
        return -1;
    }

    cmsRet = rutGpon_requestIgmpMessage(msgBody);
    if (cmsRet == CMSRET_SUCCESS)
    {
        omciIpc_sendErrorRsp(replyQ, OMCI_IPC_IGMP_ACL_RSP,
          OMCI_IPC_RET_SUCCESS);
    }
    else
    {
        omciIpc_sendErrorRsp(replyQ, OMCI_IPC_IGMP_ACL_RSP,
          OMCI_IPC_RET_NOT_FOUND);
    }

    return 0;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_mcpd_send_mib_reset
*  PURPOSE:   Send OMCI MIB reset command to MCPD.
*  PARAMETERS:
*      None.
*  RETURNS:
*      < 0 for failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_mcpd_send_mib_reset(void)
{
    int ret = 0;
    bcmIpcMsg_t *msgP = (bcmIpcMsg_t*)calloc(1, sizeof(bcmIpcMsg_t));

    msgP->type = OMCI_IPC_MCPD_MIB_RESET;
    msgP->size = 0;
    msgP->send_flags = 0;
    msgP->data = NULL;

    if ((ret = bcm_ipc_msg_send_std(&mcpdQ, msgP)) != 0)
    {
        cmsLog_error("could not send out OMCI_IPC_MCPD_MIB_RESET, ret=%d", ret);
    }
    else
    {
        cmsLog_notice("sent out OMCI_IPC_MCPD_MIB_RESET from OMCID");
    }
    return ret;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_getReplyQ
*  PURPOSE:   Get OMCI response queue ID.
*  PARAMETERS:
*      msgReq - pointer to the received IPC message.
*  RETURNS:
*      Pointer to the Tx queue.
*  NOTES:
*      None.
*****************************************************************************/
static bcmIpcMsgQueue_t *omciIpc_getReplyQ(bcmIpcMsg_t *msgReq)
{
    bcmIpcMsgQueue_t *replyQ = (bcmIpcMsgQueue_t*)NULL;

    if (msgReq != NULL)
    {
        switch (msgReq->sender)
        {
           case HTTPD_CLIENT:
               replyQ = &httpdQ;
               break;
           case CONSOLED_CLIENT:
               replyQ = &consoledQ;
               break;
           case MCPD_CLIENT:
               replyQ = &mcpdQ;
               break;
           default:
               cmsLog_error("Message type=%d illegal sender=%d\n",
                 msgReq->type, msgReq->sender);
               break;
        }
    }

    return replyQ;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_sendErrorRsp
*  PURPOSE:   Generic error response function.
*  PARAMETERS:
*      replyQ - pointer to reply queue.
*      msgId - IPC message id.
*      retCode - error code.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void omciIpc_sendErrorRsp(bcmIpcMsgQueue_t *replyQ, bcmIpcMsgId_t msgId,
  SINT32 retCode)
{
    UINT32 rspPayloadSize;
    char *bufP;
    omciMsgErrorReply_t *rspDataP;
    bcmIpcMsg_t *msgRsp;
    int ret = 0;

    rspPayloadSize = sizeof(omciMsgErrorReply_t);

    bufP = (char*)malloc(sizeof(bcmIpcMsg_t) + rspPayloadSize);
    if (bufP == NULL)
    {
        cmsLog_error("malloc() failed");
        return;
    }

    msgRsp = (bcmIpcMsg_t*)bufP;

    rspDataP = (omciMsgErrorReply_t*)((char*)msgRsp + sizeof(bcmIpcMsg_t));
    rspDataP->retCode = retCode;

    /* Header. */
    msgRsp->type = msgId;
    msgRsp->send_flags = 0;
    msgRsp->data = (void*)rspDataP;
    msgRsp->size = rspPayloadSize;

    if ((ret = bcm_ipc_msg_send_std(replyQ, msgRsp)) != 0)
    {
        cmsLog_error("bcm_ipc_msg_send_std() failed, ret=%d", ret);
    }
    else
    {
        cmsLog_notice("bcm_ipc_msg_send_std() OK");
    }
}

/*****************************************************************************
*  FUNCTION:  omciIpc_getObjectHandler
*  PURPOSE:   OMCI server-side implementation to get the OMCI object instance
*             of specified oid and instance id.
*  PARAMETERS:
*      msgReq - pointer to the IPC message.
*  RETURNS:
*      0 - success.
*      < 0 - failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_getObjectHandler(bcmIpcMsg_t *msgReq)
{
    bcmIpcMsgQueue_t *replyQ;
    int ret = 0;
    CmsRet cmsRet;
    omciMsgObject_t *reqDataP;
    omciMsgGetObjectReply_t *rspDataP;
    void *objP = NULL;
    char *bufP;
    bcmIpcMsg_t *msgRsp;
    UINT32 rspPayloadSize;

    replyQ = omciIpc_getReplyQ(msgReq);
    if (replyQ == NULL)
    {
        cmsLog_error("omciIpc_getReplyQ() failed");
        return -1;
    }

    reqDataP = (omciMsgObject_t*)(msgReq->data);

    cmsRet = _cmsObj_get(reqDataP->oid, &reqDataP->iidStack,
      OGF_NO_VALUE_UPDATE, &objP);
    if (cmsRet != CMSRET_SUCCESS)
    {
        cmsLog_error("_cmsObj_get() failed, ret=%d", cmsRet);
        omciIpc_sendErrorRsp(replyQ, OMCI_IPC_GET_OBJ_RSP,
          OMCI_IPC_RET_ERROR);
        return -1;
    }

    rspPayloadSize = sizeof(omciMsgGetObjectReply_t);

    bufP = (char*)malloc(sizeof(bcmIpcMsg_t) + rspPayloadSize);
    if (bufP == NULL)
    {
        cmsLog_error("malloc() failed");
        ret = -1;
        goto out;
    }

    msgRsp = (bcmIpcMsg_t*)bufP;

    rspDataP = (omciMsgGetObjectReply_t*)((char*)msgRsp +
      sizeof(bcmIpcMsg_t));
    rspDataP->retCode = OMCI_IPC_RET_SUCCESS;
    /* Payload. */
    memcpy(&rspDataP->obj, objP, OMCI_IPC_OBJ_BUF_MAXLEN);

    /* Header. */
    msgRsp->type = OMCI_IPC_GET_OBJ_RSP;
    msgRsp->send_flags = 0;
    msgRsp->data = (void*)rspDataP;
    msgRsp->size = rspPayloadSize;

    if ((ret = bcm_ipc_msg_send_std(replyQ, msgRsp)) != 0)
    {
        cmsLog_error("bcm_ipc_msg_send_std() failed, ret=%d", ret);
    }
    else
    {
        cmsLog_notice("bcm_ipc_msg_send_std() OK");
    }

out:
    _cmsObj_free(&objP);
    return ret;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_getSupportedObjectInfoHandler
*  PURPOSE:   OMCI server-side implementation to get the list of supported
*             OMCI objects, object instances are excluded.
*  PARAMETERS:
*      msgReq - pointer to the IPC message.
*  RETURNS:
*      0 - success.
*      < 0 - failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_getSupportedObjectInfoHandler(bcmIpcMsg_t *msgReq)
{
    bcmIpcMsgQueue_t *replyQ;
    int ret = 0;
    CmsRet cmsRet;
    OmciObjectInfo_t *omciMeListP = NULL;
    OmciObjectInfo_t *omciEntryP;
    SINT32 entryNum = 0;
    char *bufP;
    bcmIpcMsg_t *msgRsp;
    omciIpcArrayReplyHdr_t *rspDataP;
    OmciObjectInfo_t *rspEntryP;
    SINT32 entryI;
    UINT32 rspPayloadSize = 0;

    replyQ = omciIpc_getReplyQ(msgReq);
    if (replyQ == NULL)
    {
        cmsLog_error("omciIpc_getReplyQ() failed");
        return -1;
    }

    cmsRet = omcimib_getSupportedObjectInfo(&omciMeListP, &entryNum);
    if (cmsRet != CMSRET_SUCCESS)
    {
        cmsLog_error("omcimib_getSupportedObjectInfo() failed, ret=%d",
          cmsRet);
        omciIpc_sendErrorRsp(replyQ, OMCI_IPC_GET_SUPP_OBJ_INFO_RSP,
          OMCI_IPC_RET_ERROR);
        return -1;
    }

    rspPayloadSize = entryNum * sizeof(OmciObjectInfo_t);
    rspPayloadSize += sizeof(omciIpcArrayReplyHdr_t);

    bufP = (char*)malloc(sizeof(bcmIpcMsg_t) + rspPayloadSize);
    if (bufP == NULL)
    {
        cmsLog_error("malloc() failed");
        ret = -1;
        goto out;
    }

    msgRsp = (bcmIpcMsg_t*)bufP;

    rspDataP = (omciIpcArrayReplyHdr_t*)((char*)msgRsp +
      sizeof(bcmIpcMsg_t));
    rspDataP->retCode = OMCI_IPC_RET_SUCCESS;
    /* Payload. */
    rspDataP->entryNum = entryNum;
    rspEntryP = (OmciObjectInfo_t*)&rspDataP->obj[0];
    omciEntryP = omciMeListP;
    for (entryI = 0; entryI < entryNum; entryI++)
    {
        rspEntryP->oid = omciEntryP->oid;
        strncpy(rspEntryP->objectName, omciEntryP->objectName,
          MAX_OMCI_PARAM_NAME_LENGTH);
        rspEntryP++;
        omciEntryP++;
    }

    /* Header. */
    msgRsp->type = OMCI_IPC_GET_SUPP_OBJ_INFO_RSP;
    msgRsp->send_flags = 0;
    msgRsp->data = (void*)rspDataP;
    msgRsp->size = rspPayloadSize;

    if ((ret = bcm_ipc_msg_send_std(replyQ, msgRsp)) != 0)
    {
        cmsLog_error("bcm_ipc_msg_send_std() failed, ret=%d", ret);
    }
    else
    {
        cmsLog_notice("bcm_ipc_msg_send_std() OK");
    }

out:
    cmsMem_free(omciMeListP);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_getObjectInfoHandler
*  PURPOSE:   OMCI server-side implementation to get the list of OMCI
*             object instances, excluding place holders.
*  PARAMETERS:
*      msgReq - pointer to the IPC message.
*  RETURNS:
*      0 - success.
*      < 0 - failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_getObjectInfoHandler(bcmIpcMsg_t *msgReq)
{
    bcmIpcMsgQueue_t *replyQ;
    int ret = 0;
    CmsRet cmsRet;
    OmciObjectInfo_t *omciMeListP = NULL;
    OmciObjectInfo_t *omciEntryP;
    SINT32 entryNum;
    char *bufP;
    bcmIpcMsg_t *msgRsp;
    omciIpcArrayReplyHdr_t *rspDataP;
    omciMsgObject_t *rspEntryP;
    SINT32 entryI;
    UINT32 rspPayloadSize;

    replyQ = omciIpc_getReplyQ(msgReq);
    if (replyQ == NULL)
    {
        cmsLog_error("omciIpc_getReplyQ() failed");
        return -1;
    }

    cmsRet = omcimib_getObjectInfo(&omciMeListP, &entryNum);
    if (cmsRet != CMSRET_SUCCESS)
    {
        cmsLog_error("omcimib_getObjectInfo() failed, ret=%d",
          cmsRet);
        omciIpc_sendErrorRsp(replyQ, OMCI_IPC_GET_OBJ_INFO_RSP,
          OMCI_IPC_RET_ERROR);
        return -1;
    }

    rspPayloadSize = entryNum * sizeof(omciMsgObject_t);
    rspPayloadSize += sizeof(omciIpcArrayReplyHdr_t);
    bufP = (char*)malloc(sizeof(bcmIpcMsg_t) + rspPayloadSize);
    if (bufP == NULL)
    {
        cmsLog_error("malloc() failed");
        ret = -1;
        goto out;
    }

    msgRsp = (bcmIpcMsg_t*)bufP;

    rspDataP = (omciIpcArrayReplyHdr_t*)((char*)msgRsp + sizeof(bcmIpcMsg_t));
    rspDataP->retCode = OMCI_IPC_RET_SUCCESS;
    /* Payload. */
    rspDataP->entryNum = entryNum;
    rspEntryP = (omciMsgObject_t*)&rspDataP->obj[0];
    omciEntryP = omciMeListP;
    for (entryI = 0; entryI < entryNum; entryI++)
    {
        rspEntryP->oid = omciEntryP->oid;
        memcpy(&rspEntryP->iidStack, &omciEntryP->iidStack,
          sizeof(InstanceIdStack));
        rspEntryP++;
        omciEntryP++;
    }

    /* Header. */
    msgRsp->type = OMCI_IPC_GET_OBJ_INFO_RSP;
    msgRsp->send_flags = 0;
    msgRsp->data = (void*)rspDataP;
    msgRsp->size = rspPayloadSize;

    if ((ret = bcm_ipc_msg_send_std(replyQ, msgRsp)) != 0)
    {
        cmsLog_error("bcm_ipc_msg_send_std() failed, ret=%d", ret);
    }
    else
    {
        cmsLog_notice("bcm_ipc_msg_send_std() OK");
    }

out:
    cmsMem_free(omciMeListP);
    return ret;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_getParamInfoHandler
*  PURPOSE:   OMCI server-side implementation to get the list of OMCI
*             attributes (not the actual values).
*  PARAMETERS:
*      msgReq - pointer to the IPC message.
*  RETURNS:
*      0 - success.
*      < 0 - failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_getParamInfoHandler(bcmIpcMsg_t *msgReq)
{
    bcmIpcMsgQueue_t *replyQ;
    int ret = 0;
    CmsRet cmsRet;
    SINT32 entryNum = 0;
    char *bufP;
    bcmIpcMsg_t *msgRsp;
    omciMsgGetParamInfo_t *reqDataP;
    omciIpcArrayReplyHdr_t *rspDataP;
    OmciParamInfo_t *rspEntryP;
    OmciParamInfo_t *omciParamInfoListP = NULL;
    OmciParamInfo_t *omciEntryP;
    SINT32 entryI;
    UINT32 rspPayloadSize;

    replyQ = omciIpc_getReplyQ(msgReq);
    if (replyQ == NULL)
    {
        cmsLog_error("omciIpc_getReplyQ() failed");
        return -1;
    }

    reqDataP = (omciMsgGetParamInfo_t*)(msgReq->data);
    cmsRet = omcimib_getParamInfo(reqDataP->oid, &omciParamInfoListP,
      &entryNum);
    if (cmsRet != CMSRET_SUCCESS)
    {
        cmsLog_error("omcimib_getParamInfo() failed, ret=%d",
          cmsRet);
        omciIpc_sendErrorRsp(replyQ, OMCI_IPC_GET_PARAM_INFO_RSP,
          OMCI_IPC_RET_ERROR);
        return -1;
    }

    rspPayloadSize = entryNum * sizeof(OmciParamInfo_t);
    rspPayloadSize += sizeof(omciIpcArrayReplyHdr_t);
    bufP = (char*)malloc(sizeof(bcmIpcMsg_t) + rspPayloadSize);
    if (bufP == NULL)
    {
        cmsLog_error("malloc() failed");
        ret = -1;
        goto out;
    }

    msgRsp = (bcmIpcMsg_t*)bufP;

    rspDataP = (omciIpcArrayReplyHdr_t*)((char*)msgRsp +
      sizeof(bcmIpcMsg_t));
    rspDataP->retCode = OMCI_IPC_RET_SUCCESS;
    /* Payload. */
    rspDataP->entryNum = entryNum;
    rspEntryP = (OmciParamInfo_t*)&rspDataP->obj[0];
    omciEntryP = omciParamInfoListP;
    for (entryI = 0; entryI < entryNum; entryI++)
    {
        memcpy(rspEntryP, omciEntryP, sizeof(OmciParamInfo_t));
        rspEntryP++;
        omciEntryP++;
    }

    /* Header. */
    msgRsp->type = OMCI_IPC_GET_PARAM_INFO_RSP;
    msgRsp->send_flags = 0;
    msgRsp->data = (void*)rspDataP;
    msgRsp->size = rspPayloadSize;

    if ((ret = bcm_ipc_msg_send_std(replyQ, msgRsp)) != 0)
    {
        cmsLog_error("bcm_ipc_msg_send_std() failed, ret=%d", ret);
    }
    else
    {
        cmsLog_notice("bcm_ipc_msg_send_std() OK");
    }

out:
    cmsMem_free(omciParamInfoListP);
    return ret;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_fullPathToPathDescriptorHandler
*  PURPOSE:   OMCI server-side implementation to convert full path string to
*             pathDescriptor.
*  PARAMETERS:
*      msgReq - pointer to the IPC message.
*  RETURNS:
*      < 0 for failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_fullPathToPathDescriptorHandler(bcmIpcMsg_t *msgReq)
{
    bcmIpcMsgQueue_t *replyQ;
    int ret = 0;
    CmsRet cmsRet;
    omciMsgFullPath2PathDesc_t *reqDataP;
    char *bufP;
    bcmIpcMsg_t *msgRsp;
    omciMsgFullPath2PathDescReply_t *rspDataP;
    _MdmPathDescriptor mdmPathDesc;
    UINT32 rspPayloadSize;

    replyQ = omciIpc_getReplyQ(msgReq);
    if (replyQ == NULL)
    {
        cmsLog_error("omciIpc_getReplyQ() failed");
        return -1;
    }

    reqDataP = (omciMsgFullPath2PathDesc_t*)(msgReq->data);
    cmsRet = mdmlite_fullPathToPathDescriptor(reqDataP->fullPath,
      &mdmPathDesc);
    if (cmsRet != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to convert full path %s to object info\n",
          reqDataP->fullPath);
        omciIpc_sendErrorRsp(replyQ, OMCI_IPC_CVRT_FP_2_PD_RSP,
          OMCI_IPC_RET_ERROR);
        return -1;
    }

    rspPayloadSize = sizeof(omciMsgFullPath2PathDescReply_t);

    bufP = (char*)malloc(sizeof(bcmIpcMsg_t) + rspPayloadSize);
    if (bufP == NULL)
    {
        cmsLog_error("malloc() failed");
        ret = -1;
        goto out;
    }

    msgRsp = (bcmIpcMsg_t*)bufP;

    rspDataP = (omciMsgFullPath2PathDescReply_t*)((char*)msgRsp +
      sizeof(bcmIpcMsg_t));
    rspDataP->retCode = OMCI_IPC_RET_SUCCESS;
    /* Payload. */
    rspDataP->obj.oid = mdmPathDesc.oid;
    memcpy(&(rspDataP->obj.iidStack), &(mdmPathDesc.iidStack),
      sizeof(InstanceIdStack));
    strncpy(rspDataP->obj.objectName, mdmPathDesc.paramName,
      MAX_MDM_PARAM_NAME_LENGTH);

    /* Header. */
    msgRsp->type = OMCI_IPC_CVRT_FP_2_PD_RSP;
    msgRsp->send_flags = 0;
    msgRsp->data = (void *)rspDataP;
    msgRsp->size = rspPayloadSize;

    if ((ret = bcm_ipc_msg_send_std(replyQ, msgRsp)) != 0)
    {
        cmsLog_error("bcm_ipc_msg_send_std() failed, ret=%d", ret);
    }
    else
    {
        cmsLog_notice("bcm_ipc_msg_send_std() OK");
    }

out:
    return ret;
}

/*****************************************************************************
*  FUNCTION:  omciIpc_pathDescriptorToFullPathHandler
*  PURPOSE:   OMCI server-side implementation to convert pathDescriptor to
*             full path string.
*  PARAMETERS:
*      msgReq - pointer to the IPC message.
*  RETURNS:
*      < 0 for failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omciIpc_pathDescriptorToFullPathHandler(bcmIpcMsg_t *msgReq)
{
    int ret = 0;
    char *genericStr = NULL;
    omciMsgPathDescr2FullPath_t *reqDataP;
    bcmIpcMsg_t *msgRsp;
    omciMsgPathDescr2FullPathReply_t *rspDataP;
    bcmIpcMsgQueue_t *replyQ;
    _MdmPathDescriptor mdmPathDesc;
    UINT32 rspPayloadSize;

    replyQ = omciIpc_getReplyQ(msgReq);
    if (replyQ == NULL)
    {
        cmsLog_error("omciIpc_getReplyQ() failed");
        return -1;
    }

    reqDataP = (omciMsgPathDescr2FullPath_t*)(msgReq->data);

    mdmPathDesc.oid = reqDataP->obj.oid;
    memcpy(&(mdmPathDesc.iidStack), &(reqDataP->obj.iidStack),
      sizeof(InstanceIdStack));
    strncpy(mdmPathDesc.paramName, reqDataP->obj.objectName,
      MAX_MDM_PARAM_NAME_LENGTH);
    if (mdmlite_pathDescriptorToFullPath(&mdmPathDesc, &genericStr)
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Invalid MIB OID %d\n", mdmPathDesc.oid);
        omciIpc_sendErrorRsp(replyQ, OMCI_IPC_CVRT_PD_2_FP_RSP,
          OMCI_IPC_RET_ERROR);
        return -1;
    }

    rspPayloadSize = sizeof(omciMsgPathDescr2FullPathReply_t);
    msgRsp = (bcmIpcMsg_t*)(bcmIpcMsg_t*)malloc(sizeof(bcmIpcMsg_t)
      + rspPayloadSize);
    if (msgRsp == NULL)
    {
        cmsLog_error("malloc() failed");
        ret = -1;
        goto out;
    }

    rspDataP = (omciMsgPathDescr2FullPathReply_t*)((char*)msgRsp +
      sizeof(bcmIpcMsg_t));
    rspDataP->retCode = OMCI_IPC_RET_SUCCESS;

    /* Payload. */
    strncpy(rspDataP->fullPath, genericStr, sizeof(rspDataP->fullPath) - 1);

    msgRsp->type = OMCI_IPC_CVRT_PD_2_FP_RSP;
    msgRsp->send_flags = 0;
    msgRsp->data = (void*)rspDataP;
    msgRsp->size = rspPayloadSize;

    if ((ret = bcm_ipc_msg_send_std(replyQ, msgRsp)) != 0)
    {
        cmsLog_error("bcm_ipc_msg_send_std() failed, ret=%d.", ret);
    }
    else
    {
        cmsLog_notice("bcm_ipc_msg_send_std() OK.");
    }

out:
    cmsMem_free(genericStr);
    return ret;
}

/*****************************************************************************
*  FUNCTION:  omcid_ipc_msg_handler
*  PURPOSE:   Read and process messages on the Rx queue.
*  PARAMETERS:
*      None.
*  RETURNS:
*      < 0 for failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omcid_ipc_msg_handler(void)
{
    bcmIpcMsg_t *pMsg;
    int rv;

    rv = bcm_ipc_msg_recv(&omcidQ, 0, &pMsg);
    if (rv > 0)
    {
        switch (pMsg->type)
        {
        case OMCI_IPC_IGMP_ACL_REQ:
            rv = omciIpc_mcpd_send_reply_handler(pMsg);
            break;

        case OMCI_IPC_GET_OBJ_REQ:
            rv = omciIpc_getObjectHandler(pMsg);
            break;

        case OMCI_IPC_GET_SUPP_OBJ_INFO_REQ:
            rv = omciIpc_getSupportedObjectInfoHandler(pMsg);
            break;

        case OMCI_IPC_GET_OBJ_INFO_REQ:
            rv = omciIpc_getObjectInfoHandler(pMsg);
            break;

        case OMCI_IPC_GET_PARAM_INFO_REQ:
            rv = omciIpc_getParamInfoHandler(pMsg);
            break;

        case OMCI_IPC_CVRT_FP_2_PD_REQ:
            rv = omciIpc_fullPathToPathDescriptorHandler(pMsg);
            break;

        case OMCI_IPC_CVRT_PD_2_FP_REQ:
            rv = omciIpc_pathDescriptorToFullPathHandler(pMsg);
            break;

        default:
            cmsLog_error("Message type %d handling not implemented yet",
              pMsg->type);
            rv = -1;
        }

        free(pMsg);
    }
    return rv;
}

/*****************************************************************************
*  FUNCTION:  omcid_get_queue_fd
*  PURPOSE:   Get OMCI Rx queue ID.
*  PARAMETERS:
*      None.
*  RETURNS:
*      OMCI Rx queue ID.
*  NOTES:
*      None.
*****************************************************************************/
UINT32 omcid_get_queue_fd(void)
{
    return (UINT32)omcidQ.ep;
}
