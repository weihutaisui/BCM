/***********************************************************************
 *
 *  Copyright (c) 2014 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2014:proprietary:omcid

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
*      OMCI Performance Monitoring (PM) thread main loop.
*
*****************************************************************************/


/* ---- Include Files ----------------------------------------------------- */

#include <errno.h>
#include <pthread.h>

#include "cms_utilc.h"

#include "omci_pm.h"
#include "omcid_pm.h"
#include "omcid_pm_timer.h"
#include "omcid_priv.h"
#include "omcipm_ipc_priv.h"
#include "mdmlite_api.h"
#include "owrut_api.h"


/* ---- Private Constants and Types --------------------------------------- */

#define OMCI_PM_REQUEST_TIMEOUT (2 * MSECS_IN_SEC)
#define OMCI_PM_CALIBRATE_CYCLE  120


/* ---- Macro API definitions --------------------------------------------- */


/* ---- Private Function Prototypes --------------------------------------- */

#ifdef DMP_X_ITU_ORG_VOICE_1
static BCM_OMCI_PM_STATUS omci_pm_getRtpStats(UINT16 phyPortId,
  void *rtpStats);
#endif /* DMP_X_ITU_ORG_VOICE_1 */
#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1
BCM_OMCI_PM_STATUS omci_pm_getDnsStats(void *dnsStats);
#endif /* DMP_X_BROADCOM_COM_DNSPROXY_1 */


/* ---- Public Variables -------------------------------------------------- */

void *pmTmrHandle = NULL; /* Should be used in omcipm thread only. */


/* ---- Private Variables ------------------------------------------------- */


/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  omci_pm_release
*  PURPOSE:   Release PM records. This function is called upon PM thread
*             exit or OMCI MIB reset.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_pm_release(void)
{
    /* Release all BCM_OMCIPM_ENTRY records (if any). */
    omci_pm_releaseAll();

    /* Release all threshold records (if any). */
    omci_threshold_releaseAll();

    /* Release all frozen and Non TCA alarm records (if any). */
    omci_alarm_releaseAll();

    /* Release all test records. */
    omci_test_releaseAll();
}

/*****************************************************************************
*  FUNCTION:  omci_pm_init
*  PURPOSE:   Initialize PM internal data.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet omci_pm_init(void)
{
    CmsRet ret = _cmsLck_acquireLockWithTimeout(MSECS_IN_SEC);

    if (ret == CMSRET_SUCCESS)
    {
        omci_pm_initAniGObject();

        omci_pm_initEnetPptpObjects();

#ifdef DMP_X_ITU_ORG_VOICE_1
        omci_pm_initVoipPptpObjects();
#endif /* DMP_X_ITU_ORG_VOICE_1 */

        omci_pm_syncAllAdminStates();

        if (omci_alarm_init() == CMSRET_SUCCESS)
        {
            omci_pm_setTimers();
        }

        _cmsLck_releaseLock();
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omci_pm_stack
*  PURPOSE:   OMCI PM thread main loop.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_pm_stack(void *pData __attribute__((unused)))
{

    SINT32 commFd = 0;
    fd_set readFdsMaster, readFds;
#ifdef BUILD_BCMIPC
    SINT32 imMsgFd = 0;
    int rv;
#endif /* BUILD_BCMIPC */
    struct timeval tm;
    UINT32 nextEventMs;  /* number of milli-seconds until next event */
    UINT32 calibrateCount = 0;
    CmsRet ret = CMSRET_SUCCESS;

    cmsLog_debug("pData=%p", pData);

    /* Set up all the fd stuff for select. */
    FD_ZERO(&readFdsMaster);

#ifdef BUILD_BCMIPC
    omcipm_im_init();
    imMsgFd = omcipm_get_im_queue_fd();

    FD_SET(imMsgFd, &readFdsMaster);
    commFd = imMsgFd;
#endif /* BUILD_BCMIPC */

    /* Initialize timer events. */
    cmsTmr_init(&pmTmrHandle);
    if (cmsTmr_getNumberOfEvents(pmTmrHandle) > 0)
    {
        cmsTmr_dumpEvents(pmTmrHandle);
    }

    /* Initialize PM objects and timers. */
    omci_data_lock();
    ret = omci_pm_init();
    omci_data_unlock();

    /* Exit if cannot initialize PM objects and timers. */
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to intialize PM objects and timers: ret=%d", ret);
        /* Clean up timer. */
        cmsTmr_cleanup(&pmTmrHandle);
        pthread_exit(0); /* exit */
    }

#ifdef DMP_X_ITU_ORG_VOICE_1
    bcm_omcipm_usrRtpStatsCbRegister(omci_pm_getRtpStats);
#endif /* DMP_X_ITU_ORG_VOICE_1 */

#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1
    bcm_omcipm_usrDnsStatsCbRegister(omci_pm_getDnsStats);
#endif /* DMP_X_BROADCOM_COM_DNSPROXY_1 */

    omci_pm_up();

    while (TRUE)
    {
        /*
        * Set our select timeout based on time to next event
        * in our timer handle.
        */

        /* Init PM calibration timer. */
        if (calibrateCount == 0)
        {
            omci_pm_calibratetimerInit();
        }

        (void)cmsTmr_getTimeToNextEvent(pmTmrHandle, &nextEventMs);

        /* 
         * Add SCHED_LAG_TIME to the next scheduled event so that by the time
         * select() returns, it will definitely be time for that event to 
         * execute. 
         */
        if (nextEventMs < SCHED_LAG_TIME)
        {
            nextEventMs = SCHED_LAG_TIME; 
        }

        tm.tv_sec = nextEventMs / MSECS_IN_SEC;
        tm.tv_usec = (nextEventMs % MSECS_IN_SEC) * USECS_IN_MSEC;

        readFds = readFdsMaster;

        if (select(commFd + 1, &readFds, NULL, NULL, &tm) < 0)
        {
            /* Interrupted by signal or something, continue. */
            continue;
        }

        /* Calculate PM calibration result. */
        if (calibrateCount != 0)
        {
            omci_pm_calibratetimerCheck();
        }
        calibrateCount++;
        if (calibrateCount >= OMCI_PM_CALIBRATE_CYCLE)
        {
            calibrateCount = 0;
        }

        /* Service all timer events that are due. */
        cmsTmr_executeExpiredEvents(pmTmrHandle);

#ifdef BUILD_BCMIPC
        if (FD_ISSET(imMsgFd, &readFds))
        {

            rv = omcipm_im_msg_handler();
            if (rv < 0)
            {
                cmsLog_error("omcipm_im_msg_handler() failed");
            }
        }
#endif /* BUILD_BCMIPC */

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
        pthread_mutex_lock(&gpon_link_mutex);
        if (gponLinkChange == TRUE)
        {
            /* Handle gpon link state change. */
            omci_pm_send_link_change();

            /* Resest gpon link state change. */
            gponLinkChange = FALSE;
        }
        pthread_mutex_unlock(&gpon_link_mutex);
#endif /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

        /* Reset PM calibration result. */
        omci_pm_calibratetimerReset();
    }

    /* Release all PM and alarm link lists. */
    omci_pm_release();

    /* Clean up timer. */
    cmsTmr_cleanup(&pmTmrHandle);

    pthread_exit(0); /* exit */
}

/*****************************************************************************
*  FUNCTION:  omci_pm_getRtpStats
*  PURPOSE:   Callback function to get RTP statistics.
*  PARAMETERS:
*      phyPortId - POTS port ID.
*      rtpStats - pointer to RTP statistics.
*  RETURNS:
*      BCM_OMCI_PM_STATUS.
*  NOTES:
*      None.
*****************************************************************************/
#ifdef DMP_X_ITU_ORG_VOICE_1
BCM_OMCI_PM_STATUS omci_pm_getRtpStats(UINT16 phyPortId, void *rtpStats)
{
    return _owapi_rut_GetRtpStats(phyPortId, rtpStats);
}
#endif /* DMP_X_ITU_ORG_VOICE_1 */

/*****************************************************************************
*  FUNCTION:  omci_pm_getDnsStats
*  PURPOSE:   Callback function to get DNS statistics.
*  PARAMETERS:
*      dnsStats - pointer to DNS staticstics.
*  RETURNS:
*      BCM_OMCI_PM_STATUS.
*  NOTES:
*      None.
*****************************************************************************/
#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1
BCM_OMCI_PM_STATUS omci_pm_getDnsStats(void *dnsStats)
{
    return _owapi_rut_getDnsStats(dnsStats);
}
#endif /* DMP_X_BROADCOM_COM_DNSPROXY_1 */
