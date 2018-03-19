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
*      OMCI initialization and main loop.
*
*****************************************************************************/


/* ---- Include Files ----------------------------------------------------- */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

#include "mdmlite_api.h"
#include "owsvc_api.h"
#include "ctlutils_api.h"
#include "owrut_api.h"
#include "me_handlers.h"

#include "omci_pm.h"
#include "omci_util.h"
#include "omcid.h"
#include "omcid_msg.h"
#include "omcid_capture.h"
#include "omcid_pm.h"
#include "omcid_priv.h"
#include "omcid_helper.h"
#include "omcid_ipc_priv.h"
#include "omcipm_ipc_priv.h"

#ifdef SUPPORT_RDPA
#include "rdpactl_api.h"
#endif


/* ---- Private Constants and Types --------------------------------------- */

#define OMCID_VERSION "2.4"
#define COMMAND_ARG_MAX 4

/*
 * Adjust OMCID_HANDLE_MODELPATH_TIMER value if the scheduleHandleModelPath()
 * execution time > OMCI Tmax.
 */
#ifdef OMCIPROV_WORKAROUND
#define OMCID_HANDLE_MODELPATH_TIMER 5000
#else
#define OMCID_HANDLE_MODELPATH_TIMER 1000
#endif /* OMCIPROV_WORKAROUND */

typedef enum
{
    OMCI_MODEL_STATE_IDLE = 0,
    OMCI_MODEL_STATE_CHANGE,
    OMCI_MODEL_STATE_CHANGE_DONE,
    OMCI_MODEL_STATE_UNCHANGE
} OmciModelState;


/* ---- Macro API definitions --------------------------------------------- */


/* ---- Private Function Prototypes --------------------------------------- */

extern CmsRet initOmciSystemObject();


/* ---- Public Variables -------------------------------------------------- */

gponOmci_t gponOmci; /* main structure */
void *msgHandle = NULL;   /* it's used in omcid.c, omcid_msg.c */
void *tmrHandle = NULL;  /* it's used in omcid.c, omcid_msg.c */


/* ---- Private Variables ------------------------------------------------- */

static omciPacket txBuffer;
static omciPacket rxBuffer;
static OmciModelState glbModelState = OMCI_MODEL_STATE_IDLE;
static UBOOL8 useConfiguredLogLevelB = TRUE;

/* CMS specific. */
static SINT32 cmsShmId = -1;
static CmsEntityId cmsEntityId = EID_OMCID;


/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  expireHandleModelPath
*  PURPOSE:   Timer expiration callback function.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void expireHandleModelPath(void *handle __attribute__((unused)))
{
    if (glbModelState == OMCI_MODEL_STATE_CHANGE)	
    {
        glbModelState = OMCI_MODEL_STATE_IDLE;
    }
    else if (glbModelState == OMCI_MODEL_STATE_UNCHANGE)	
    {
        glbModelState = OMCI_MODEL_STATE_CHANGE_DONE;
    }

    omci_data_lock();
    scheduleHandleModelPath(TRUE);
    omci_data_unlock();
}

/*****************************************************************************
*  FUNCTION:  scheduleHandleModelPath
*  PURPOSE:   Check whether the OMCI MIB has a complete ANI-UNI path to trigger
*             the device configuration.
*  PARAMETERS:
*      callFromExpiredTimer - TRUE: called from timer.
          FALSE: called when a ME instance is added, set, or deleted.
*  RETURNS:
*      None.
*  NOTES:
*      About createVlanFlowsAction():
*      The execution time of vlanCtl_createVlanFlows() increases along with
*      the total number of flows (a VLANCtl driver behavior).
*      In the original implementation, vlanCtl_createVlanFlows() was called
*      during the creation of every flow. When the OMCI configuration
*      triggered the creation of N flows, vlanCtl_createVlanFlows() was
*      called N times, and the total execution time became O(N**2).
*      To improve the performance, invoke vlanCtl_createVlanFlows() only
*      once per rx/tx interface pair, at the end of the service
*      configuration.
*****************************************************************************/
void scheduleHandleModelPath(UBOOL8 callFromExpiredTimer)
{
    static UINT32 glbMibDataSync = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OntDataObject *obj = NULL;
    CmsRet ret = CMSRET_SUCCESS;
    CmsTimestamp start_tms, end_tms;
    UINT32 deltaMs = 0;

    if (glbModelState == OMCI_MODEL_STATE_CHANGE ||
      glbModelState == OMCI_MODEL_STATE_UNCHANGE)
    {
        return;
    }

    cmsTms_get(&start_tms);

    if (glbModelState == OMCI_MODEL_STATE_IDLE)	
    {
        glbModelState = OMCI_MODEL_STATE_CHANGE;
    }
    else if (glbModelState == OMCI_MODEL_STATE_CHANGE_DONE)	
    {
        glbModelState = OMCI_MODEL_STATE_UNCHANGE;
    }

    cmsTmr_cancel(tmrHandle, expireHandleModelPath, NULL);

    /*
     * If this function was called from an expired timer, we must acquire
     * OMCI MIB lock. Otherwise, the OMCI MIB lock was already acquired
     * by the caller.
     */
    if (callFromExpiredTimer == TRUE)
    {
        ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Could not acquire OMCIMIB lock, ret=%d", ret);
            return;
        }
    }

    if ((ret = _cmsObj_get(MDMOID_ONT_DATA, &iidStack, 0, (void*)&obj))
      == CMSRET_SUCCESS)
    {
        if (glbMibDataSync != obj->mibDataSync)
        {
            glbMibDataSync = obj->mibDataSync;
            glbModelState = OMCI_MODEL_STATE_CHANGE;
            ret = cmsTmr_set(tmrHandle, expireHandleModelPath, NULL,
              OMCID_HANDLE_MODELPATH_TIMER, "omci_handle_model");
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("cmsTmr_set() failed, ret=%d", ret);
            }
        }
        else
        {
            if (glbModelState == OMCI_MODEL_STATE_CHANGE)
            {
                if (callFromExpiredTimer == TRUE)
                {
                    (void)rutGpon_configModelPath();
                    if (omciDm_getVlanFlowIfPairCount() != 0)
                    {
                        omciDm_traverseSavedVlanFlowIfPair(
                          createVlanFlowsAction);
                    }
#if defined(DMP_X_ITU_ORG_VOICE_1)
                    rutGpon_setupOmciVoipObjects();
#endif /* #defined(DMP_X_ITU_ORG_VOICE_1) */
                    omciDm_clearUniListConfigState();
                }

                glbModelState = OMCI_MODEL_STATE_UNCHANGE;
                ret = cmsTmr_set(tmrHandle, expireHandleModelPath, NULL,
                  OMCID_HANDLE_MODELPATH_TIMER, "omci_handle_model");
                if (ret != CMSRET_SUCCESS)
                {
                    cmsLog_error("cmsTmr_set() failed, ret=%d", ret);
                }
            }
            else if (glbModelState == OMCI_MODEL_STATE_UNCHANGE)
            {
                glbModelState = OMCI_MODEL_STATE_IDLE;
            }
        }

        _cmsObj_free((void**)&obj);
    }

    /* If the lock was acquired in this function, then release it here too. */
    if (callFromExpiredTimer == TRUE)
    {
        (void)_cmsLck_releaseLock();
    }

    cmsTms_get(&end_tms);
    deltaMs = cmsTms_deltaInMilliSeconds(&end_tms, &start_tms);
    if (deltaMs >= OMCID_HANDLE_MODELPATH_TIMER)
    {
        cmsLog_error("Long OMCI processing time %d ms", deltaMs);
    }
}

/*****************************************************************************
*  FUNCTION:  omci_init_system
*  PURPOSE:   Initialization related to the MDMOID_OMCI_SYSTEM object.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet omci_init_system(void)
{
    omciDebug_t omciDebug;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *obj = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get OMCIMIB lock, ret=%d", ret);
        return ret;
    }

    if ((ret = _cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, 0, (void*)
      &obj)) == CMSRET_SUCCESS)
    {
        omciDebug.flags.all = obj->debugFlags;
        if (omciDebug.flags.bits.file == OMCID_DEBUG_ON)
        {
            ret = rutGpon_openDebugFile();
        }

        /* Init tag rules link list. */
        (void)omci_rule_init();
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
        /* Init RG WAN services link list. */
        omci_service_init();
#endif /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1

        /* Fixed SWBCACPE-8775
         * set MIPS rate limiter to make MIPS be able handle other tasks
         * beside of receiving data traffic. It's helpful when all flows
         * are removed and all data traffic are forwared to MIPS.
         */
#ifdef CONFIG_MIPS_RATE_LIMIT
        rutGpon_configMipsRateLimit();
#endif

        /* Enable flowcache for RG OMCI. */
        (void)prctl_runCommandInShellWithTimeout("fc enable");

        /* if ethernet port type is RG then
         * create virtual interface (eth0.0, eth1.0, etc...)
         * remove device interface (eth0, eth1, etc...) out default bridge br0
         * and add virtual interface to default bridge br0
         */
        rutRgFull_configDefaultLanInterfaces();

#else /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

#if defined(SUPPORT_IPV6)
        /*
         * ONU/SFU has to make it become host to receive router
         * addresses from RA
         */
        (void)prctl_runCommandInShellWithTimeout(
          "echo 0 > /proc/sys/net/ipv6/conf/all/forwarding");
#endif /* SUPPORT_IPV6 */

        /* Disable flowcache for ONT. */
        (void)prctl_runCommandInShellWithTimeout("fc disable");

#endif /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */
        _cmsObj_free((void**)&obj);
    }

    if (_owapi_rut_tmctl_getQueueOwner() == TMCTL_OWNER_BH)
    {
        rutGpon_pptpEthUniTmInit();
    }

    (void)_cmsLck_releaseLock();

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omci_omcimsg_handler
*  PURPOSE:   Initialization related to the MDMOID_OMCI_SYSTEM object.
*  PARAMETERS:
*      None.
*  RETURNS:
*      < 0 - failure. 0 - successful operation.
*  NOTES:
*      None.
*****************************************************************************/
static int omci_omcimsg_handler(void)
{
    int len;
    int rv = 0;
    CmsTimestamp begin_tms, end_tms;
    UINT32 deltaMs;

    /* Replay capture. */
    if (TRUE == OmciCapture.Flags.Replaying)
    {
        omci_capture_get_msg_to_replay(&rxBuffer);
        len = OMCI_PACKET_A_SIZE;
    }
    else
    {
        len = gpon_omci_api_receive(&rxBuffer, (int)sizeof(rxBuffer));
    }

    /* Only handle message that passes CRC checking in OMCI driver. */
    if (errno == EBADMSG)
    {
        return 0;
    }

    cmsTms_get(&begin_tms);

    /* Handle the OMCI message from the OLT. */
    omci_data_lock();
    rv = omci_msg_handler(&rxBuffer, len);
    omci_data_unlock();

    cmsTms_get(&end_tms);
    deltaMs = cmsTms_deltaInMilliSeconds(&end_tms, &begin_tms);

    omciDebugPrint(OMCI_DEBUG_MODULE_OMCI,
    //printf(
      "\n========================================================================\n");
    omciDebugPrint(OMCI_DEBUG_MODULE_OMCI,
    //printf(
      "OMCI message response time in %d milli-seconds\n",
      deltaMs);
    omciDebugPrint(OMCI_DEBUG_MODULE_OMCI,
      //printf(
      "========================================================================\n\n");
    if (deltaMs >= MSECS_PER_SEC)
    {
        cmsLog_error("Long OMCI message response time in %d ms", deltaMs);
    }
    return rv;
}

/*****************************************************************************
*  FUNCTION:  omci_stack
*  PURPOSE:   OMCI process main loop.
*  PARAMETERS:
*      None.
*  RETURNS:
*      < 0 - fatal error. Otherwise never returns.
*  NOTES:
*      None.
*****************************************************************************/
static int omci_stack(void)
{
    int rv = 0;
    SINT32 omciFd = 0;
#if defined (BRCM_CMS_BUILD)
    SINT32 cmsMsgFd = 0;
#endif /* BRCM_CMS_BUILD */
#if defined (BUILD_BCMIPC)
    SINT32 ipcMsgFd = 0;
    SINT32 imMsgFd = 0;
#endif /* BUILD_BCMIPC */
    SINT32 maxFd = 0;
    fd_set readFdsMaster, readFds;
    struct timeval tm;
    UINT32 nextEventMs = 0;  /* number of milli-seconds until next event */

    /* Get the OMCI handle. */
    gpon_omci_api_get_handle(&omciFd);

#if defined (BRCM_CMS_BUILD)
    /* Get the CMS messaging handle. */
    cmsMsg_getEventHandle(msgHandle, &cmsMsgFd);
#endif /* BRCM_CMS_BUILD */

#if defined (BUILD_BCMIPC)
    /* Get the IPC messaging handle. */
    ipcMsgFd = omcid_get_queue_fd();
    imMsgFd = omcid_get_im_queue_fd();
#endif /* BUILD_BCMIPC */

    /* Set up all the fd stuff for select. */
    FD_ZERO(&readFdsMaster);
    FD_SET(omciFd, &readFdsMaster);
#if defined (BRCM_CMS_BUILD)
    FD_SET(cmsMsgFd, &readFdsMaster);
#endif /* BRCM_CMS_BUILD */
#if defined (BUILD_BCMIPC)
    FD_SET(ipcMsgFd, &readFdsMaster);
    FD_SET(imMsgFd, &readFdsMaster);
#endif /* BUILD_BCMIPC */

    maxFd = omciFd;
#if defined (BRCM_CMS_BUILD)
    maxFd = (omciFd > cmsMsgFd) ? omciFd : cmsMsgFd;
#endif /* BRCM_CMS_BUILD */
#if defined (BUILD_BCMIPC)
    maxFd = (maxFd > ipcMsgFd)? maxFd : ipcMsgFd;
    maxFd = (maxFd > imMsgFd)? maxFd : imMsgFd;
#endif /* BUILD_BCMIPC */

#if defined (BRCM_CMS_BUILD)
    omci_app_send_postmdm_cmsmsg();
#endif /* BRCM_CMS_BUILD */

    while (1)
    {
        /*
        * Set our select timeout based on time to next event
        * in our timer handle.
        */
        const UINT32 sched_lag_time = 10; /* ms */

        (void)cmsTmr_getTimeToNextEvent(tmrHandle, &nextEventMs);
        if (nextEventMs < UINT32_MAX - sched_lag_time)
        {
            nextEventMs += sched_lag_time;
        }

        tm.tv_sec = nextEventMs / MSECS_IN_SEC;
        tm.tv_usec = (nextEventMs % MSECS_IN_SEC) * USECS_IN_MSEC;
        cmsLog_debug("set select timeout to %ld.%ld secs",
          tm.tv_sec, (tm.tv_usec / USECS_IN_MSEC));

        /* Disable the select timeout if we are doing replay. */
        if (TRUE == OmciCapture.Flags.Replaying)
        {
            tm.tv_sec = 0;
            tm.tv_usec = 0;
        }

        readFds = readFdsMaster;

        rv = select(maxFd+1, &readFds, NULL, NULL, &tm);
        if (rv < 0)
        {
            /* interrupted by signal or something, continue */
            rv = 0;
            continue;
        }

        /*
         * service all timer events that are due (there may be no events due
         * if we woke up from select because of activity on the fds).
         */
        cmsTmr_executeExpiredEvents(tmrHandle);

#ifdef DESKTOP_LINUX
        if (TRUE == OmciCapture.Flags.Replaying)
#else /* DESKTOP_LINUX */
        if (FD_ISSET(omciFd, &readFds) || (TRUE == OmciCapture.Flags.Replaying))
#endif /* DESKTOP_LINUX */
        {
           /* OMCI packet or replaying a capture file. */
            rv = omci_omcimsg_handler();
        }
#if defined (BRCM_CMS_BUILD)
        if (FD_ISSET(cmsMsgFd, &readFds))
        {
            rv = omci_cmsmsg_handler();
        }
#endif /* BRCM_CMS_BUILD */
#if defined (BUILD_BCMIPC)
        if (FD_ISSET(imMsgFd, &readFds))
        {
            rv = omcid_im_msg_handler();
        }
        if (FD_ISSET(ipcMsgFd, &readFds))
        {
            rv = omcid_ipc_msg_handler();
        }

#endif /* BUILD_BCMIPC */
    }

    printf("\n%s: EXITING (%d)\n\n", __FUNCTION__, rv);

    return rv;
}

/*****************************************************************************
*  FUNCTION:  omci_monitor
*  PURPOSE:   Debug function to read received OMCI messages.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void omci_monitor(void)
{
    int count = 1;
    int size;

    printf("\nStarting OMCI Monitor...\n");

    while (1)
    {
        printf("\n%s: Waiting for OMCI packet...\n", __FUNCTION__);

        /* gpon_omci_api_receive blocks if no packets are available */
        size = gpon_omci_api_receive(&rxBuffer, OMCI_PACKET_A_SIZE);
        if ((size <= 0) || (size > OMCI_PACKET_A_SIZE))
        {
            cmsLog_error("Invalid OMCI packet size: %d", size);
            break;
        }

        printf("\n%s: New OMCI packet:\n", __FUNCTION__);
        OMCID_DUMP_PACKET(&rxBuffer, size);

        printf("%s: Total OMCI packets = %d\n", __FUNCTION__, count++);
    }
}

/*****************************************************************************
*  FUNCTION:  dbg_create_packet
*  PURPOSE:   Debug function to create an OMCI message.
*  PARAMETERS:
*      pPacket - pointer to OMCI packet buffer.
*      seed - packet content seed.
*      size - packet size.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void dbg_create_packet(omciPacket *pPacket, int seed, int size)
{
    int i;
    unsigned char *pBuf = (unsigned char *)pPacket;

    for (i = 0; i < size; i++)
    {
        *pBuf++ = (unsigned char)(seed + i);
    }
}

/*****************************************************************************
*  FUNCTION:  send_test_packet
*  PURPOSE:   Debug function to send a test OMCI packet.
*  PARAMETERS:
*      arg - argument array with num_of_packets and msgType information.
*  RETURNS:
*      < 0 - failure. 0 - successful operation.
*  NOTES:
*      None.
*****************************************************************************/
static int send_test_packet(int *arg)
{
    int num_of_packets = arg[0];
    int msgType = arg[1];
    int size;
    int count = 0;
    omciPacket *p = &txBuffer;

    while (num_of_packets--)
    {
        cmsLog_notice("Transmitting OMCI packet %d...", count);

        dbg_create_packet(&txBuffer, count, OMCI_PACKET_A_SIZE);

        p->tcId[0] = 0;
        p->tcId[1] = 0;
        p->msgType = 0x40 | msgType; /* ack request */
        p->devId   = OMCI_PACKET_DEV_ID_A;

        count++;

        size = gpon_omci_api_transmit(&txBuffer, OMCI_PACKET_A_SIZE);
        if ((size == -1) && (errno == ENOLINK))
        {
            /* Return success: the OMCI Port has been deleted */
            cmsLog_notice("OMCI Port has been deleted, continue");
        }
        else if (size != OMCI_PACKET_A_SIZE)
        {
            cmsLog_error("Failed to transmit packet (%d)", size);
            return -1;
        }
    }

    return 0;
}

/*****************************************************************************
*  FUNCTION:  set_log_level
*  PURPOSE:   Helper function to set log level.
*  PARAMETERS:
*      arg - argument array with log level information.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void set_log_level(int *arg)
{
    CmsLogLevel logLevel;
    DECLARE_PGPONOMCI();

    /* Save log level. */
    pGponOmci->logLevel = arg[0];

    if (arg[0] == 0)
    {
        logLevel = LOG_LEVEL_ERR;
    }
    else if (arg[0] == 1)
    {
        logLevel = LOG_LEVEL_NOTICE;
    }
    else
    {
        logLevel = LOG_LEVEL_DEBUG;
    }

    cmsLog_setLevel(logLevel);
}

/*****************************************************************************
*  FUNCTION:  get_numeric_arg
*  PURPOSE:   Helper function to parse command line arguments.
*  PARAMETERS:
*      argc - number of command line arguments.
*      argv - array of command line argument strings.
*      arg - parsed numeric argument array.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static int get_numeric_arg(int *argc, char ***argv, int *arg)
{
    if (--(*argc))
    {
        (*argv)++;
        errno = 0;
        *arg = strtol(**argv, NULL, 0);
        if (errno != 0)
        {
            cmsLog_notice("%s: %d", strerror(errno), *arg);
            return -1;
        }

        return 0;
    }

    cmsLog_error("Missing argument");
    return -1;
}

/*****************************************************************************
*  FUNCTION:  get_cmd_args
*  PURPOSE:   Helper function to parse command line arguments.
*  PARAMETERS:
*      argc - number of command line arguments.
*      argv - array of command line argument strings.
*      arg - parsed numeric argument array.
*      count - number of arguments.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static int get_cmd_args(int *argc, char ***argv, int *arg, int count)
{
    int i;
    int rv = 0;

    if (count > COMMAND_ARG_MAX)
    {
        cmsLog_error("Internal Error");
        return -1;
    }

    for(i = 0; (rv == 0) && (i < count); i++)
    {
        rv = get_numeric_arg(argc, argv, &arg[i]);
    }

    return rv;
}

#if (!defined(DESKTOP_LINUX))
/*****************************************************************************
*  FUNCTION:  omci_getPonMacInitState
*  PURPOSE:   Get MAC driver init state from scratch pad.
*  PARAMETERS:
*      None.
*  RETURNS:
*      Initial (O1) or Emergency stop (O7).
*  NOTES:
*      None.
*****************************************************************************/
static UINT32 omci_getPonMacInitState(void)
{
    SINT32 rv = 0;
    char buf[SP_LINK_STATE_BUFFER_SIZE] = "\0";
    UINT32 initMacState;

    /* Read initial state from Scratch-Pad. */
    rv = cmsPsp_get("linkState", &buf, SP_LINK_STATE_BUFFER_SIZE);

    /* The kernel will either return the number of bytes read,
     * or if a user provided buffer was not big enough,
     * a negative number indicating the number of bytes needed.
     */
    if (rv < 0)
    {
        cmsLog_error("Load initial state from SP failed."
          "Provided buffer was not big enough, number of bytes needed: %d).",
          rv);
    }
    else
    {
        cmsLog_debug("Load initial state from SP succeed, number of bytes "
          "read: %d).", rv);
    }

    /* In case this is the first board initialisation - previous link state
     * is empty.
     */
    if (buf[0] == '\0')
    {
        memcpy(&buf, "standby", SP_LINK_STATE_BUFFER_SIZE);
        (void)cmsPsp_set("linkState", &buf, SP_LINK_STATE_BUFFER_SIZE);
    }

    initMacState = memcmp(&buf, "stop   ", SP_LINK_STATE_BUFFER_SIZE) == 0 ?
      BCM_PLOAM_OSTATE_EMERGENCY_STOP_O7 : BCM_PLOAM_OSTATE_INITIAL_O1;

    return initMacState;
}
#endif /* !defined(DESKTOP_LINUX) */

/*****************************************************************************
*  FUNCTION:  omci_pm_main
*  PURPOSE:   To start OMCI PM thread.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      This function is blocked until the PM thread completes initialization.
*****************************************************************************/
static void omci_pm_main(void)
{
    struct sched_param omcipmtSched;
    pthread_t omcipmt;
    pthread_attr_t omcipmtAttr;
    THREAD_DATA omcipmThData;

    (void)pthread_attr_init(&omcipmtAttr);
    omcipmtSched.sched_priority = 1;  /* low priority */
    (void)pthread_attr_setschedparam(&omcipmtAttr, &omcipmtSched);
    (void)pthread_attr_setdetachstate(&omcipmtAttr, PTHREAD_CREATE_DETACHED);

    omcipmThData.entityId = (cmsEntityId + 1);
    omcipmThData.pMsgHandle = msgHandle;
    omcipmThData.pTmrHandle = tmrHandle;

    /* Create omcipmt thread. */
    (void)pthread_create(&omcipmt, &omcipmtAttr, (void*)&omci_pm_stack,
      (void*)&omcipmThData);
    (void)pthread_attr_destroy(&omcipmtAttr);

    omci_pm_wait_up();
}

/*****************************************************************************
*  FUNCTION:  omci_init
*  PURPOSE:   OMCI process initialization.
*  PARAMETERS:
*      None.
*  RETURNS:
*      < 0 if failed.
*  NOTES:
*      gponCtl_startAdminState() will trigger the ranging process if the ONU
*      is connected with the OLT. It should be called in the end to ensure
*      the OMCI initialization (including PM thread) is completed before the
*      ranging process and OMCI message exchanges.
*****************************************************************************/
static int omci_init(void)
{
    SINT32 rv = 0, i = 0;
    CmsRet ret = CMSRET_SUCCESS;
    CmsLogLevel logLevel;
    DECLARE_PGPONOMCI();
#if (!defined(DESKTOP_LINUX))
    BCM_Ploam_StartInfo info;
#endif

    /* Ignore broken pipes. */
    (void)signal(SIGINT, SIG_IGN);
    (void)signal(SIGPIPE, SIG_IGN);

#if defined (BUILD_BCMIPC)
    /* Initialize the IPC framework. */
    omci_ipc_init();
    omcid_im_init();
#endif /* BUILD_BCMIPC */

#if defined (BRCM_CMS_BUILD)
    /* Initialize the CMS framework. */
    if ((ret = omci_cmsInit(cmsEntityId, cmsShmId)) != CMSRET_SUCCESS)
    {
        cmsLog_error("omci_cmsInit() failed (%d), exit.", ret);
        return -1;
    }
#endif /* BRCM_CMS_BUILD */

    /* Initialize OMCI MIB schema. */
    if ((ret = omcimib_init()) != CMSRET_SUCCESS)
    {
        cmsLog_error("omcimib_init failed, ret=%d", ret);
        return ret;
    }

    if ((ret = cmsTmr_init(&tmrHandle)) != CMSRET_SUCCESS)
    {
        cmsLog_error("cmsTmr_init failed, ret=%d", ret);
        return -1;
    }

    /* Initialize the OMCI message handler control structure. */
    memset(pGponOmci, 0, sizeof(gponOmci_t));
    for (i = 0; i < OMCI_PACKET_PRIORITY_MAX; i++)
    {
        pGponOmci->prevTcId[i] = OMCI_PACKET_TC_ID_INIT;
    }

    /* Initialize MDMOID_OMCI_SYSTEM data. */
    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT))
      == CMSRET_SUCCESS)
    {
        (void)initOmciSystemObject();
        (void)_cmsLck_releaseLock();
    }
    else
    {
        cmsLog_error("Failed to get OMCIMIB lock, ret=%d, exit.", ret);
        return -1;
    }

    if (rutGpon_getOmciPromiscInfo() != 0)
    {
        pGponOmci->flags |= OMCI_FLAG_PROMISC;
    }

    /* OMCI resource data init. */
    omciDm_init();

    /* Initialize default OMCI MIB. */
    if ((ret = omciMibDataReset()) != CMSRET_SUCCESS)
    {
        cmsLog_error("omciMibDataReset() failed, ret=%d, exit.", ret);
        return -1;
    }

    /* Create and start OMCI PM thread. */
    omci_pm_main();

    /* Create and start Remote Debug thread. */
    (void)debugCbInit();

    /* Initialize platform parameters. */
    if ((ret = omci_init_system()) != CMSRET_SUCCESS)
    {
        cmsLog_error("omci_init_system() failed, ret=%d, exit.", ret);
        return -1;
    }

    cmsLog_setHeaderMask(0);
    _owapi_rut_initLoggingFromConfig(useConfiguredLogLevelB);
    logLevel = cmsLog_getLevel();

    cmsLog_setLevel(LOG_LEVEL_NOTICE);
    cmsLog_notice("Broadcom OMCI Stack v%s (%s, %s)", OMCID_VERSION,
        __DATE__, __TIME__);
    cmsLog_setLevel(logLevel);

    cmsLog_setHeaderMask(DEFAULT_LOG_HEADER_MASK);

    /* Initialize timer events. */
    if (cmsTmr_getNumberOfEvents(tmrHandle) > 0)
    {
        cmsTmr_dumpEvents(tmrHandle);
    }

    /* Initialize the OMCI message handler control structure. */
    if ((ret = omci_omcimsg_init()) != CMSRET_SUCCESS)
    {
        return -1;
    }

    omci_capture_init();

#if (!defined(DESKTOP_LINUX))
    /* Start xPON driver. */
    info.initOperState = omci_getPonMacInitState();
    rv = gponCtl_startAdminState(&info);
    if (rv != 0)
    {
        cmsLog_error("Start GPON driver failed (%d), exit.", rv);
        return (int)rv;
    }

    /* Initialize the OMCI API. Ready to Rx/Tx OMCI messages. */
    rv = gpon_omci_api_init(OMCI_ACCESS_TYPE_BLOCKING);
    if (rv != 0)
    {
        cmsLog_error("gpon_omci_api_init() failed (%d), exit.", rv);
    }
#endif /* !defined(DESKTOP_LINUX) */

    return (int)rv;
}

/*****************************************************************************
*  FUNCTION:  omci_exit
*  PURPOSE:   Clean up functions when OMCID exits.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void omci_exit(void)
{
    gpon_omci_api_exit();
#if defined (BRCM_CMS_BUILD)
    omci_cmsCleanup();
#endif /* BRCM_CMS_BUILD */
    cmsTmr_cleanup(&tmrHandle);

    omcmib_cleanup();
    printf("omci_exit()\n");
}

#ifdef SUPPORT_RDPA
/*****************************************************************************
*  FUNCTION:  isGponLinkActive
*  PURPOSE:   Check RDPA PON link type.
*  PARAMETERS:
*      None.
*  RETURNS:
*      TRUE if the WAN is an ITU-T PON link type.
*  NOTES:
*      None.
*****************************************************************************/
static UBOOL8 isGponLinkActive(void)
{
#ifdef DESKTOP_LINUX
    return TRUE;
#else /* DESKTOP_LINUX */

    UBOOL8 active = TRUE;
    rdpa_wan_type wan_type = rdpa_wan_none;
    int ret;

    ret = rdpaCtl_get_wan_type(rdpa_wan_type_to_if(rdpa_wan_gpon), &wan_type);
    if (ret)
    {
        printf("rdpaCtl_get_wan_type: FAILED wan_type[%d] ret[%d]",
          wan_type, ret);
        active = FALSE;
    }
    else
    {
        if ((wan_type != rdpa_wan_gpon) &&
          (wan_type != rdpa_wan_xgpon))
        {
            active = FALSE;
        }
    }

    return active;
#endif /* DESKTOP_LINUX */
}
#endif   /* SUPPORT_RDPA */

/*****************************************************************************
*  FUNCTION:  main
*  PURPOSE:   OMCID process main entry function.
*  PARAMETERS:
*      argc - number of command line arguments.
*      argv - array of command line argument strings.
*  RETURNS:
*      < 0 - failed operation.
*  NOTES:
*      None.
*****************************************************************************/
int main(int argc, char **argv)
{
    int rv = 0;
    int arg[COMMAND_ARG_MAX];
    UINT32 commandCount = 0;
    DECLARE_PGPONOMCI();

    /* Only start omcid when gpon link is active. */
    if (isGponLinkActive() == FALSE)
    {
        printf("omcid can only be started when xPON link is active, exit");
        return -1;
    }

    /* Initialize CMS logging. */
    cmsLog_init(cmsEntityId);

    cmsLog_debug("initializing %s...", *argv);

    /* Parse commands. */
    argc--, argv++;
    while ((rv == 0) && (argc > 0))
    {
        if (strcmp(*argv, "start") == 0)
        {
            /* Normal OMCI start up routine. */
            rv = omci_init();
            if (rv == 0)
            {
                commandCount++;
                rv = omci_stack();
            }
            /* Never reach here in normal conditions. */
            omci_exit();
        }
        else if (strcmp(*argv, "monitor") == 0)
        {
            /* Debug function to check Rx packets. */
            commandCount++;
            omci_monitor();
        }
        else if (strcmp(*argv, "send") == 0)
        {
            /* Debug function to send Tx packets. */
            commandCount++;
            rv = get_cmd_args(&argc, &argv, arg, 2);
            if (rv == 0)
            {
                rv = send_test_packet(arg);
            }
        }
        else if (strcmp(*argv, "-v") == 0)
        {
            /* Change default CMS log level. */
            rv = get_cmd_args(&argc, &argv, arg, 1);
            if (rv == 0)
            {
                set_log_level(arg);
            }
            useConfiguredLogLevelB = FALSE;
        }
        else if (strcmp(*argv, "-m") == 0)
        {
            /* Change CMS share memory ID. */
            rv = get_cmd_args(&argc, &argv, arg, 1);
            if (rv == 0)
            {
                cmsShmId = (SINT32)arg[0];
                cmsLog_notice("Setting shmId = %d", cmsShmId);
            }
        }
        else if (strcmp(*argv, "-promisc") == 0)
        {
            commandCount++;
            pGponOmci->flags |= OMCI_FLAG_PROMISC;
        }
        else
        {
            cmsLog_error("Invalid command");
            rv = -1;
        }

        if (argc > 0)
        {
            argc--, argv++;
        }
    }

    if ((rv == 0) && (commandCount == 0))
    {
        cmsLog_error("missing command");
        rv = -1;
    }

    cmsLog_notice("exiting with code %d", rv);

    /* Cleanup CMS logging. */
    cmsLog_cleanup();

    return (int)rv;
}
