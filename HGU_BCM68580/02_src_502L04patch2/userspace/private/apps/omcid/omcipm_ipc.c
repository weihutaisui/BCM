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
*      Inter Process Communication utilities between OMCID and PM.
*
*****************************************************************************/

/* ---- Include Files ----------------------------------------------------- */

#include "owsvc_api.h"
#include "omci_ipc.h"
#include "omcipm_ipc_priv.h"
#include "omcid_helper.h"
#include "omcid_pm_timer.h"


/* ---- Private Constants and Types --------------------------------------- */

#define OMCI_LOCK_BUSY_MAX 10


/* ---- Macro API definitions --------------------------------------------- */


/* ---- Private Function Prototypes --------------------------------------- */


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */

/* Internal queues. */
static bcmIpcMsgQueue_t omcidIntQ;
static bcmIpcMsgQueue_t omcipmIntQ;

/* mutex, cond for synchronization between omcid main and omcipm thread. */
static pthread_mutex_t comm_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t comm_cond = PTHREAD_COND_INITIALIZER;

/*
 * mutex, cond, signal data for synchronization between
 * omcid main and omcipm thread. They're used in
 * omcid_pm.c, omcid_msg.c, omcid_pm_timer.c
 */
static pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;

static UINT32 omciDataMutexLockBusyCnt = 0;

/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  omcid_im_init
*  PURPOSE:   Initialize OMCID-side queue for OMCID/PM internal messaging.
*  PARAMETERS:
*      None.
*  RETURNS:
*      < 0 for failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omcid_im_init(void)
{
    int ret = 0;
    bcmIpcMsgQueueParam_t omciQueueParam;

    (void)unlink(OMCIDINT_ENDPOINT);
    (void)unlink(OMCIPMINT_ENDPOINT);
    memset(&omciQueueParam, 0, sizeof(omciQueueParam));

    omciQueueParam.name = OMCIDINT_RX_Q_NAME;
    omciQueueParam.ep_type = BCM_IPC_MSG_QUEUE_EP_DOMAIN_SOCKET;
    omciQueueParam.local_ep_address = OMCIDINT_ENDPOINT;
    omciQueueParam.remote_ep_address = OMCIPMINT_ENDPOINT;

    ret = bcm_ipc_msg_queue_create(&omcidIntQ, &omciQueueParam);
    if (ret)
    {
        cmsLog_error("OMCID queue initialization failed, ret=%d", ret);
        return ret;
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omcipm_im_init
*  PURPOSE:   Initialize PM-side queue for OMCID/PM internal messaging.
*  PARAMETERS:
*      None.
*  RETURNS:
*      < 0 for failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omcipm_im_init(void)
{
    int ret = 0;
    bcmIpcMsgQueueParam_t omciQueueParam;


    memset(&omciQueueParam, 0, sizeof(omciQueueParam));

    omciQueueParam.name = OMCIPMINT_RX_Q_NAME;
    omciQueueParam.ep_type = BCM_IPC_MSG_QUEUE_EP_DOMAIN_SOCKET;
    omciQueueParam.local_ep_address = OMCIPMINT_ENDPOINT;
    omciQueueParam.remote_ep_address = OMCIDINT_ENDPOINT;

    ret = bcm_ipc_msg_queue_create(&omcipmIntQ, &omciQueueParam);
    if (ret)
    {
        cmsLog_error("OMCID queue initialization failed, ret=%d", ret);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omci_pm_send_link_change
*  PURPOSE:   OMCI PM sends PON link status change message.
*  PARAMETERS:
*      None.
*  RETURNS:
*      < 0 for failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omci_pm_send_link_change(void)
{
    int ret = 0;
    bcmIpcMsg_t *msgP = (bcmIpcMsg_t*)calloc(1, sizeof(bcmIpcMsg_t));

    msgP->type = OMCI_IPC_GPON_LINK_STATUS_CHANGE;
    msgP->size = 0;
    msgP->send_flags = 0;
    msgP->data = NULL;

    if ((ret = bcm_ipc_msg_send_std(&omcipmIntQ, msgP)) != 0)
    {
        cmsLog_error("could not send out OMCI_IPC_GPON_LINK_STATUS_CHANGE, ret=%d",
          ret);
    }
    else
    {
        cmsLog_notice("sent out OMCI_IPC_GPON_LINK_STATUS_CHANGE");
    }

    printf("\n%s: ret=%d\n", __FUNCTION__, ret);
    return ret;
}

/*****************************************************************************
*  FUNCTION:  omcid_im_msg_handler
*  PURPOSE:   OMCID internal messaging handler.
*  PARAMETERS:
*      None.
*  RETURNS:
*      < 0 for failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omcid_im_msg_handler(void)
{
    bcmIpcMsg_t *pMsg;
    int rv;

    rv = bcm_ipc_msg_recv(&omcidIntQ, 0, &pMsg);
    if (rv > 0)
    {
        switch (pMsg->type)
        {
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
        case OMCI_IPC_GPON_LINK_STATUS_CHANGE:
            omci_gpon_link_status_change_handler();
            rv = 0;
            break;
#endif
        default:
            cmsLog_error("Invalid message type %d", pMsg->type);
            rv = -1;
            break;
        }

        free(pMsg);
    }
    return rv;
}

/*****************************************************************************
*  FUNCTION:  omcipm_im_msg_handler
*  PURPOSE:   OMCIPM internal messaging handler.
*  PARAMETERS:
*      None.
*  RETURNS:
*      < 0 for failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omcipm_im_msg_handler(void)
{
    bcmIpcMsg_t *pMsg;
    int rv;

    rv = bcm_ipc_msg_recv(&omcipmIntQ, 0, &pMsg);
    if (rv > 0)
    {
        switch (pMsg->type)
        {
        case OMCI_IPC_PM_SYNC_TIME:
            omci_data_lock();
            /* Reset timers, init. PM counters. */
            omci_pm_syncTimers();
            omci_data_unlock();
            rv = 0;
            break;

        default:
            cmsLog_error("Invalid message type %d", pMsg->type);
            rv = -1;
            break;
        }
        free(pMsg);
    }
    return rv;
}

/*****************************************************************************
*  FUNCTION:  omcid_get_im_queue_fd
*  PURPOSE:   Get OMCID-side internal messaging queue ID.
*  PARAMETERS:
*      None.
*  RETURNS:
*      OMCID queue ID.
*  NOTES:
*      None.
*****************************************************************************/
UINT32 omcid_get_im_queue_fd(void)
{
    return (UINT32)omcidIntQ.ep;
}

/*****************************************************************************
*  FUNCTION:  omcipm_get_im_queue_fd
*  PURPOSE:   Get OMCIPM-side internal messaging queue ID.
*  PARAMETERS:
*      None.
*  RETURNS:
*      OMCIPM queue ID.
*  NOTES:
*      None.
*****************************************************************************/
UINT32 omcipm_get_im_queue_fd(void)
{
    return (UINT32)omcipmIntQ.ep;
}

/*****************************************************************************
*  FUNCTION:  omcid_pm_send_sync_time
*  PURPOSE:   OMCID sends SYNC TIME command to OMCI PM.
*  PARAMETERS:
*      None.
*  RETURNS:
*      < 0 for failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int omcid_pm_send_sync_time(void)
{
    int ret = 0;
    bcmIpcMsg_t *msgP = (bcmIpcMsg_t*)calloc(1, sizeof(bcmIpcMsg_t));

    msgP->type = OMCI_IPC_PM_SYNC_TIME;
    msgP->size = 0;
    msgP->send_flags = 0;
    msgP->data = NULL;

    if ((ret = bcm_ipc_msg_send_std(&omcidIntQ, msgP)) != 0)
    {
        cmsLog_error("could not send out OMCI_IPC_PM_SYNC_TIME, ret=%d", ret);
    }
    else
    {
        cmsLog_notice("sent out OMCI_IPC_PM_SYNC_TIME");
    }

    printf("\n%s: ret=%d\n", __FUNCTION__, ret);
    return ret;
}

/*****************************************************************************
*  FUNCTION:  omci_pm_wait_up
*  PURPOSE:   Wait OMCIPM UP indication (completes initialization).
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_pm_wait_up(void)
{
    pthread_mutex_lock(&comm_mutex);
    pthread_cond_wait(&comm_cond, &comm_mutex);
    pthread_mutex_unlock(&comm_mutex);
}

/*****************************************************************************
*  FUNCTION:  omci_pm_up
*  PURPOSE:   Send OMCIPM UP indication.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_pm_up(void)
{
    pthread_mutex_lock(&comm_mutex);
    pthread_cond_signal(&comm_cond);
    pthread_mutex_unlock(&comm_mutex);
}

/*****************************************************************************
*  FUNCTION:  omci_data_lock
*  PURPOSE:   Lock the data_mutex.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_data_lock(void)
{
    int rc;

    rc = pthread_mutex_lock(&data_mutex);
    if (rc != 0)
    {
        cmsLog_error("pthread_mutex_lock(data) failed, rc=%d\n", rc);
    }
}

/*****************************************************************************
*  FUNCTION:  omci_data_trylock
*  PURPOSE:   Try to lock the data_mutex.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
int omci_data_trylock(void)
{
    int rc;

    rc = pthread_mutex_trylock(&data_mutex);
    if (rc != 0)
    {
        if (rc == EBUSY)
        {
            omciDataMutexLockBusyCnt++;
            if (omciDataMutexLockBusyCnt >= OMCI_LOCK_BUSY_MAX)
            {
                cmsLog_debug("pthread_mutex_trylock(data) failed, "
                  "cnt=%d, rc=%d\n", omciDataMutexLockBusyCnt, rc);
                omciDataMutexLockBusyCnt = 0;
            }
        }
        else
        {
            cmsLog_error("pthread_mutex_trylock(data) failed, rc=%d\n", rc);
        }
    }

    return rc;
}

/*****************************************************************************
*  FUNCTION:  omci_data_unlock
*  PURPOSE:   Unlock the data_mutex.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_data_unlock(void)
{
    int rc;

    rc = pthread_mutex_unlock(&data_mutex);
    if (rc != 0)
    {
        cmsLog_error("pthread_mutex_unlock(data) failed, rc=%d\n", rc);
    }
}
