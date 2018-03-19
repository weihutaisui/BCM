/***********************************************************************
 *
 *  Copyright (c) 2008 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:omcid

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
*      OMCID CMS-specific functions.
*
*****************************************************************************/

#if defined(BRCM_CMS_BUILD)

/* ---- Include Files ----------------------------------------------------- */

#include "mdmlite_api.h"
#include "owsvc_api.h"
#include "me_handlers.h"
#include "ctlutils_api.h"
#include "owrut_api.h"

#include "omciutl_cmn.h"
#include "omci_util.h"
#include "omci_pm.h"
#include "omcid_pm.h"
#include "omcid_msg.h"
#include "omcid.h"
#include "omcid_capture.h"
#include "omcid_priv.h"
#include "omcid_helper.h"
#include "omcipm_ipc_priv.h"

#include "cms_lck.h"
#include "cms_mdm.h"


/* ---- Private Constants and Types --------------------------------------- */


/* ---- Macro API definitions --------------------------------------------- */


/* ---- Private Function Prototypes --------------------------------------- */


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */


/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  omci_cmsInit
*  PURPOSE:   Initialize CMS related context.
*  PARAMETERS:
*      entityId - OMCI entity id.
*      cmsShmId - OMCI share memory id.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet omci_cmsInit(const CmsEntityId entityId, SINT32 cmsShmId)
{
    SINT32 sessionPid = 0;
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader *buf = NULL;

    /*
    * Detach myself from the terminal so I don't get any control-c/sigint.
    * On the desktop, it is smd's job to catch control-c and exit.
    * When omcid detects that smd has exited, omcid will also exit.
    */
    if ((sessionPid = setsid()) == -1)
    {
        cmsLog_error("Could not detach from terminal");
    }
    else
    {
        cmsLog_debug("Detached from terminal");
    }

    if ((ret = cmsMsg_init(entityId, &msgHandle)) != CMSRET_SUCCESS)
    {
        cmsLog_error("msg initialization failed, ret=%d", ret);
        return ret;
    }

    ret = cmsMsg_receiveWithTimeout(msgHandle, &buf, 100);
    if (ret == CMSRET_SUCCESS)
    {
        cmsMem_free(buf);
    }

    rutcmn_cxtInit(msgHandle, entityId);
    _owapi_rut_cxtInit(msgHandle, entityId, cmsShmId);

    return CMSRET_SUCCESS;
}

/*****************************************************************************
*  FUNCTION:  omci_cmsCleanup
*  PURPOSE:   Clean up CMS related context.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
void omci_cmsCleanup(void)
{
    cmsMdm_cleanup();
    cmsMsg_cleanup(&msgHandle);
}

/*****************************************************************************
*  FUNCTION:  omci_app_send_postmdm_cmsmsg
*  PURPOSE:   Inform SSK to perform MDM post-activation.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_app_send_postmdm_cmsmsg(void)
{
    char buf[sizeof(CmsMsgHeader) + sizeof(MdmPostActNodeInfo)] = {0};
    CmsRet cmsReturn;
    CmsMsgHeader *msgHdr = (CmsMsgHeader *)buf;
    MdmPostActNodeInfo *msgBody = (MdmPostActNodeInfo *)(msgHdr + 1);

    msgHdr->dst = EID_SSK;
    msgHdr->src = EID_OMCID;
    msgHdr->type = CMS_MSG_MDM_POST_ACTIVATING;
    msgHdr->flags_event = 1;
    msgHdr->dataLength = sizeof(MdmPostActNodeInfo);
    msgBody->subType = MDM_POST_ACT_TYPE_FILTER;

    // Attempt to send CMS response message & test result.
    cmsReturn = cmsMsg_send(msgHandle, msgHdr);
    if (cmsReturn != CMSRET_SUCCESS)
    {
        // Log error.
        cmsLog_error("Send message failure, cmsResult: %d", cmsReturn);
    }
    else
    {
        cmsLog_debug("Sent OMCI App Indication to SSK");
    }
}

/*****************************************************************************
*  FUNCTION:  omci_stats_msg_send
*  PURPOSE:   Request OMCI statistics.
*  PARAMETERS:
*      reqMsg - pointer to the CMS message.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet omci_stats_msg_send(CmsMsgHeader *reqMsg)
{
    CmsRet ret;
    void *msgBuf;
    CmsMsgHeader *msg;
    _GponOmciStatsObject *msgStats;
    DECLARE_PGPONOMCI();

    msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) +
      sizeof(_GponOmciStatsObject), ALLOC_ZEROIZE);
    if (msgBuf == NULL)
    {
        cmsLog_error("Memory allocation failed");

        return CMSRET_INTERNAL_ERROR;
    }

    msg = (CmsMsgHeader *)msgBuf;

    /* initialize some common fields */
    msg->type = CMS_MSG_GET_GPON_OMCI_STATS;
    msg->src = EID_OMCID;
    msg->dst = reqMsg->src;
    msg->flags_response = 1;
    msg->sequenceNumber = reqMsg->sequenceNumber;
    msg->dataLength = sizeof(_GponOmciStatsObject);

    msgStats = (_GponOmciStatsObject*)(msg + 1);

    memcpy((char*)msgStats, (char*)&pGponOmci->stats,
      sizeof(_GponOmciStatsObject));

    ret = cmsMsg_send(msgHandle, msg);

    CMSMEM_FREE_BUF_AND_NULL_PTR(msg);

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omci_request_handler
*  PURPOSE:   Handle received CMS message - OMCI command request.
*  PARAMETERS:
*      msgReq - pointer to the CMS message.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet omci_request_handler(const CmsMsgHeader *msgReq)
{
    CmsTimestamp start_tms, end_tms;
    UINT32 startTime = 0, endTime = 0;
    UINT32 deltaMs = 0;
    CmsRet ret = CMSRET_SUCCESS;
    omciPacket *packetReq = (omciPacket *) (msgReq+1);
    UINT32 crc32;
    char buf[sizeof(CmsMsgHeader) + sizeof(omciPacket)];
    CmsMsgHeader *msgRes=(CmsMsgHeader *) buf;
    omciPacket *packetRes = (omciPacket *) (msgRes+1);
    UINT8 msgType = OMCI_PACKET_MT(packetReq);
    UINT8 request = OMCI_PACKET_AR(packetReq);

    cmsTms_get(&start_tms);
    startTime = start_tms.sec;

    if (msgType != OMCI_MSG_TYPE_DOWNLOADSECTION)
    {
        cmsLog_notice(" Request Message:\n");
    }

    (void)omci_msg_handler(packetReq, OMCI_PACKET_SIZE(packetReq));

    if (request != 0)
    {
        memset(buf, 0, sizeof(CmsMsgHeader) + sizeof(omciPacket));

        msgRes->type = CMS_MSG_OMCI_COMMAND_RESPONSE;
        msgRes->src = EID_OMCID;
        msgRes->dst = msgReq->src;
        msgRes->flags_event = 1;
        msgRes->dataLength = sizeof(omciPacket);
        msgRes->flags_response = 1;
        msgRes->flags_request = 0;
        msgRes->sequenceNumber = msgReq->sequenceNumber;

        memcpy(packetRes, packetReq, sizeof(omciPacket));
        packetRes->msgType = OMCI_MSG_TYPE_AK(OMCI_PACKET_MT(packetReq));

        if (OMCI_CHECK_DEV_ID_A(packetRes))
        {
            packetRes->A.trailer[3] = OMCI_PACKET_CPCS_SDU_LEN;
        }

        crc32 = omciUtl_getCrc32(-1, (char *) packetRes,
                OMCI_PACKET_SIZE(packetRes) - OMCI_PACKET_MIC_SIZE);
        OMCI_HTONL(OMCI_PACKET_CRC(packetRes), crc32);
        packetRes->src_eid = EID_OMCID;

        if ((ret = cmsMsg_send(msgHandle, msgRes)) != CMSRET_SUCCESS)
        {
            cmsLog_error("could not send out CMS_MSG_OMCI_COMMAND_RESPONSE, ret=%d", ret);
        }
        else
        {
            if (msgType != OMCI_MSG_TYPE_DOWNLOADSECTION)
            {
                cmsLog_notice("sent out CMS_MSG_OMCI_COMMAND_RESPONSE with message type = %d",
                  OMCI_PACKET_MT(packetRes));
            }
        }

        cmsTms_get(&end_tms);
        endTime = end_tms.sec;
        deltaMs = cmsTms_deltaInMilliSeconds(&end_tms, &start_tms);

        omciDebugPrint(OMCI_DEBUG_MODULE_OMCI,
            //printf(
            "\n========================================================================\n");
            omciDebugPrint(OMCI_DEBUG_MODULE_OMCI,
            //printf(
            "OMCI message response time: %d ms, receive: %d sec., send: %d sec.\n",
            deltaMs, startTime, endTime);
            omciDebugPrint(OMCI_DEBUG_MODULE_OMCI,
            //printf(
            "========================================================================\n\n");

        if (deltaMs >= MSECS_PER_SEC)
        {
            cmsLog_error("Long OMCI message response time %d ms, receive: %d sec., send: %d sec",
              deltaMs, startTime, endTime);
        }
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omci_promisc_set_request_handler
*  PURPOSE:   Handle received CMS message - set promisc mode.
*  PARAMETERS:
*      msgReq - pointer to the CMS message.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet omci_promisc_set_request_handler(const CmsMsgHeader *msgReq)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader msgRes;

    rutGpon_setOmciPromiscInfo(msgReq->wordData);

    if (msgReq->wordData == 1)
    {
        gponOmci.flags |= OMCI_FLAG_PROMISC;
    }
    else if (msgReq->wordData == 0)
    {
        gponOmci.flags &= ~OMCI_FLAG_PROMISC;
    }
    else
    {
        cmsLog_error("the input promisc value is wrong, value=%d", msgReq->wordData);
    }

    memset(&msgRes, 0, sizeof(CmsMsgHeader));
    msgRes.type = CMS_MSG_OMCI_PROMISC_SET_RESPONSE;
    msgRes.src = cmsMsg_getHandleEid(msgHandle);
    msgRes.dst = msgReq->src;
    msgRes.flags_response = 1;
    msgRes.dataLength = 0;
    msgRes.sequenceNumber = msgReq->sequenceNumber;
    msgRes.wordData = rutGpon_getOmciPromiscInfo();

    if ((ret = cmsMsg_send(msgHandle, &msgRes)) != CMSRET_SUCCESS)
    {
        cmsLog_error("could not send out CMS_MSG_OMCI_PROMISC_SET_RESPONSE, ret=%d", ret);
    }
    else
    {
        cmsLog_notice("sent out CMS_MSG_OMCI_PROMISC_SET_RESPONSE from OMCID");
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omci_debug_get_request_handler
*  PURPOSE:   Handle received CMS message - get debug mode.
*  PARAMETERS:
*      msgReq - pointer to the CMS message.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet omci_debug_get_request_handler(const CmsMsgHeader *msgReq)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader msgRes;

    memset(&msgRes, 0, sizeof(CmsMsgHeader));
    msgRes.type = CMS_MSG_OMCI_DEBUG_GET_RESPONSE;
    msgRes.src = cmsMsg_getHandleEid(msgHandle);
    msgRes.dst = msgReq->src;
    msgRes.flags_response = 1;
    msgRes.dataLength = 0;
    msgRes.sequenceNumber = msgReq->sequenceNumber;
    msgRes.wordData = rutGpon_getOmciDebugInfo();

    if ((ret = cmsMsg_send(msgHandle, &msgRes)) != CMSRET_SUCCESS)
    {
        cmsLog_error("could not send out CMS_MSG_OMCI_DEBUG_GET_RESPONSE, ret=%d", ret);
    }
    else
    {
        cmsLog_notice("sent out CMS_MSG_OMCI_DEBUG_GET_RESPONSE from OMCID");
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omci_debug_set_request_handler
*  PURPOSE:   Handle received CMS message - set debug mode.
*  PARAMETERS:
*      msgReq - pointer to the CMS message.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet omci_debug_set_request_handler(const CmsMsgHeader *msgReq)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader msgRes;

    rutGpon_setOmciDebugInfo(msgReq->wordData);

    memset(&msgRes, 0, sizeof(CmsMsgHeader));
    msgRes.type = CMS_MSG_OMCI_DEBUG_SET_RESPONSE;
    msgRes.src = cmsMsg_getHandleEid(msgHandle);
    msgRes.dst = msgReq->src;
    msgRes.flags_response = 1;
    msgRes.dataLength = 0;
    msgRes.sequenceNumber = msgReq->sequenceNumber;
    msgRes.wordData = rutGpon_getOmciDebugInfo();

    if ((ret = cmsMsg_send(msgHandle, &msgRes)) != CMSRET_SUCCESS)
    {
        cmsLog_error("could not send out CMS_MSG_OMCI_DEBUG_SET_RESPONSE, ret=%d", ret);
    }
    else
    {
        cmsLog_notice("sent out CMS_MSG_OMCI_DEBUG_SET_RESPONSE from OMCID");
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omci_mcpd_send_reply_handler
*  PURPOSE:   Perform OMCI admission control and send response back to MCPD.
*  PARAMETERS:
*      msgReq - pointer to the received CMS message.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet omci_mcpd_send_reply_handler(CmsMsgHeader *msgReq)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader msgRes;
    OmciIgmpMsgBody *msgBody = (OmciIgmpMsgBody *) (msgReq+1);

    memset(&msgRes, 0, sizeof(CmsMsgHeader));
    msgRes.type = CMS_MSG_OMCI_IGMP_ADMISSION_CONTROL;
    msgRes.src = cmsMsg_getHandleEid(msgHandle);
    msgRes.dst = msgReq->src;
    msgRes.flags_response = 1;
    msgRes.wordData = rutGpon_requestIgmpMessage(msgBody);

    if ((ret = cmsMsg_send(msgHandle, &msgRes)) != CMSRET_SUCCESS)
    {
        cmsLog_error("could not send out CMS_MSG_OMCI_DEBUG_SET_RESPONSE, ret=%d", ret);
    }
    else
    {
        cmsLog_notice("sent out CMS_MSG_OMCI_DEBUG_SET_RESPONSE from OMCID");
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omci_cmsmsg_handler
*  PURPOSE:   Receive and process CMS messages.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
int omci_cmsmsg_handler(void)
{
    int rv = 0;
    CmsRet ret;
    char *pData;
    CmsMsgHeader *msg = NULL;

    if ((ret = cmsMsg_receiveWithTimeout(msgHandle, &msg, 500)) != CMSRET_SUCCESS)
    {
        if (cmsFil_isFilePresent(SMD_SHUTDOWN_IN_PROGRESS) == 0)
        {
            cmsLog_error("Failed to receive message (ret=%d)", ret);
        }
        rv = -1;
    }
    else
    {
        pData = ((char *) msg) + sizeof (CmsMsgHeader);
        omci_data_lock();

        switch(msg->type)
        {
        case CMS_MSG_GET_GPON_OMCI_STATS:
            if ((ret = omci_stats_msg_send(msg)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to transmit message (ret=%d)", ret);
                rv = -1;
            }
            else
            {
                rv = 0;
            }
            break;
        case CMS_MSG_OMCI_COMMAND_REQUEST:
            if ((ret = omci_request_handler(msg)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to transmit message (ret=%d)", ret);
                rv = -1;
            }
            else
            {
                rv = 0;
            }
            break;
        case CMS_MSG_OMCI_PROMISC_SET_REQUEST:
            if ((ret = omci_promisc_set_request_handler(msg)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to transmit message (ret=%d)", ret);
                rv = -1;
            }
            else
            {
                rv = 0;
            }
            break;
        case CMS_MSG_OMCI_DEBUG_GET_REQUEST:
            if ((ret = omci_debug_get_request_handler(msg)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to transmit message (ret=%d)", ret);
                rv = -1;
            }
            else
            {
                rv = 0;
            }
            break;
        case CMS_MSG_OMCI_DEBUG_SET_REQUEST:
            if ((ret = omci_debug_set_request_handler(msg)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to transmit message (ret=%d)", ret);
                rv = -1;
            }
            else
            {
                rv = 0;
            }
            break;
        case CMS_MSG_OMCI_IGMP_ADMISSION_CONTROL:
            if ((ret = omci_mcpd_send_reply_handler(msg)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to transmit message (ret=%d)", ret);
                rv = -1;
            }
            else
            {
                rv = 0;
            }
            break;

        case CMS_MSG_OMCI_CAPTURE_STATE_ON:
            {
                omci_capture_control(msg->wordData, RESTART);
                rv = 0;
                break;
            }
        case CMS_MSG_OMCI_CAPTURE_STATE_OFF:
            {
                omci_capture_control(0, STOP);
                rv = 0;
                break;
            }
        case CMS_MSG_OMCI_CAPTURE_REPLAY_ON:
            {
                omci_capture_replay_control(msg->wordData, pData, START);
                rv = 0;
                break;
            }
        case CMS_MSG_OMCI_CAPTURE_REPLAY_OFF:
            {
                omci_capture_replay_control(0, NULL, STOP);
                rv = 0;
                break;
            }
        case CMS_MSG_OMCI_CAPTURE_VIEW:
            {
                omci_capture_view(msg->wordData, pData);
                rv = 0;
                break;
            }
        case CMS_MSG_OMCI_CAPTURE_DOWNLOAD:  // SAVE file to USB stick
            {
                omci_capture_save(msg->wordData, pData);
                rv = 0;
                break;
            }
        case CMS_MSG_OMCI_CAPTURE_UPLOAD://restore data to internal capture file location
            {
                omci_capture_restore(msg->wordData, pData);
                rv = 0;
                break;
            }

       case CMS_MSG_SET_LOG_LEVEL:
            cmsLog_debug("got set log level to %d", msg->wordData);
            cmsLog_setLevel(msg->wordData);
            if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
            {
               cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
            }
            rv = 0;
            break;

       case CMS_MSG_SET_LOG_DESTINATION:
            cmsLog_debug("got set log destination to %d", msg->wordData);
            cmsLog_setDestination(msg->wordData);
            if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
            {
               cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
            }
            rv = 0;
            break;

        // Test for CLI Alarm Sequence Number SET request.
        case CMS_MSG_OMCIPMD_ALARM_SEQ_SET:
            // Test for valid OMCI Alarm Sequence Number.
            if ((msg->wordData > 0) && (msg->wordData < 256))
            {
                // Setup new OMCI Alarm Sequence Number.
                omci_alarm_setSequenceNumber((UINT8)msg->wordData);

                // Output new OMCI Alarm Sequence Number.
                cmsLog_notice("New OMCI Alarm Sequence Number: %d\n", (UINT8)msg->wordData);
            }
            else
            {
                // Output new OMCI Alarm Sequence Number.
                cmsLog_error("Invalid OMCI Alarm Sequence Number: %d", msg->wordData);
            }
            break;

        // Test for CLI Alarm Sequence Number GET request.
        case CMS_MSG_OMCIPMD_ALARM_SEQ_GET:
            // Output current OMCI Alarm Sequence Number
            printf("Current OMCI Alarm Sequence Number: %d\n", omci_alarm_getSequenceNumber());
            break;

        case CMS_MSG_OMCI_DEBUG_OMCI_MSG_GEN:
            omci_msg_gen_handler((OmciMsgGenCmd*)(msg + 1));
            break;

        // Test for ping result
        case CMS_MSG_PING_STATE_CHANGED:
            omci_test_iphost_ping_result((OmciPingDataMsgBody*)(msg + 1));
            break;

        // Test for traceroute result
        case CMS_MSG_TRACERT_STATE_CHANGED:
            omci_test_iphost_trace_route_result((OmciTracertDataMsgBody*)(msg + 1));
            break;

#ifdef SUPPORT_DEBUG_TOOLS
       case CMS_MSG_MEM_DUMP_STATS:
            if ((ret = cmsLck_acquireLockWithTimeout(6*MSECS_IN_SEC)) == CMSRET_SUCCESS)
            {
               cmsMem_dumpMemStats();
               cmsLck_releaseLock();
            }
            rv = 0;
            break;
#endif

#ifdef CMS_MEM_LEAK_TRACING
       case CMS_MSG_MEM_DUMP_TRACEALL:
            cmsMem_dumpTraceAll();
            rv = 0;
            break;

       case CMS_MSG_MEM_DUMP_TRACE50:
            cmsMem_dumpTrace50();
            rv = 0;
            break;

       case CMS_MSG_MEM_DUMP_TRACECLONES:
            cmsMem_dumpTraceClones();
            rv = 0;
            break;
#endif

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
        case CMS_MSG_OMCI_RG_WAN_SERVICE_STAUTS_CHANGE:
            {
                OmciServiceMsgBody *pService = (OmciServiceMsgBody*)(msg + 1);
                omci_gpon_wan_service_status_change_handler(pService);
                rv = 0;
                break;
            }
#endif   // DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1

        case CMS_MSG_OMCI_DEBUG_MKERR_SWDLERR1:
            {
                OmciDbgErr = OMCI_ERR_SWDL_SECTION_HOLE;
                rv = 0;
                break;
            }
        case CMS_MSG_OMCI_DEBUG_MKERR_SWDLERR2:
            {
                OmciDbgErr = OMCI_ERR_SWDL_SECTION_RSP;
                rv = 0;
                break;
            }
        case CMS_MSG_OMCI_DEBUG_MKERR_SWDLERR3:
            {
                OmciDbgErr = OMCI_ERR_SWDL_IMAGE_CRC;
                rv = 0;
                break;
            }
        case CMS_MSG_OMCI_DUMP_INFO_REQ:
            {
                omciDebug_dumpInfo((UINT32)msg->wordData);
                break;
            }

        default:
            {
                cmsLog_notice("Invalid message type (%x)", (unsigned int)msg->type);

                // make sure we don't bomb out for this reason.
                rv = 0;
            }
        }
        omci_data_unlock();

        CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
    }
    return rv;
}
#endif /* BRCM_CMS_BUILD */
