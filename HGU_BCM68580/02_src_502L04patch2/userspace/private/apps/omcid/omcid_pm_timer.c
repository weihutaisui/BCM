/***********************************************************************
 *
 *  Copyright (c) 2014 Broadcom
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
*      OMCI Performance Monitoring (PM) thread timer related functions.
*
*****************************************************************************/


/* ---- Include Files ----------------------------------------------------- */

#include <pthread.h>

#include "cms_utilc.h"
#include "omci_pm.h"
#include "omcid_priv.h"
#include "omciobj_defs.h"
#include "omcid_pm_timer.h"
#include "omcipm_ipc_priv.h"


/* ---- Private Constants and Types --------------------------------------- */

#define MDMOID_DONT_CARE 0

typedef void (*OMCI_TMR_FNC)(void *data);

typedef struct
{
    UINT32 mdmOid;
    UINT32 expiredTime;
    char   tmrName[BUFLEN_128];
    OMCI_TMR_FNC tmrFunc;
} OmciTmrInfo_t;


/* ---- Macro API definitions --------------------------------------------- */

#define OMCI_TMR_MAX    (sizeof(omciTmrInfo)/sizeof(OmciTmrInfo_t))


/* ---- Private Function Prototypes --------------------------------------- */

static void tmrFunc_pm_poll(void *data);
static void tmrFunc_15min(void *data);
static void tmrFunc_multicast(void *data);
static void tmrFunc_test(void *data);
static void tmrFunc_alarm_enet(void *data);
static void tmrFunc_alarm_gpon(void *data);
static void tmrFunc_alarm_optic(void *data);
static void tmrFunc_alarm_arc(void *data);

static const OmciTmrInfo_t omciTmrInfo[] =
{
    {MDMOID_GEM_PORT_PM_HISTORY_DATA, WAIT_GPON_TIME,
      "omcipmd_GPON", tmrFunc_pm_poll},
    {MDMOID_FEC_PM_HISTORY_DATA, WAIT_FEC_TIME,
      "omcipmd_FEC", tmrFunc_pm_poll},
    {MDMOID_ETHERNET_PM_HISTORY_DATA, WAIT_ENET_TIME,
      "omcipmd_ENET", tmrFunc_pm_poll},
    {MDMOID_ETHERNET_PM_HISTORY_DATA2, WAIT_ENET2_TIME,
      "omcipmd_ENET2", tmrFunc_pm_poll},
    {MDMOID_ETHERNET_PM_HISTORY_DATA3, WAIT_ENET3_TIME,
      "omcipmd_ENET3", tmrFunc_pm_poll},
    {MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA, WAIT_ENETDN_TIME,
      "omcipmd_ENETDN", tmrFunc_pm_poll},
    {MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA, WAIT_ENETUP_TIME,
      "omcipmd_ENETUP", tmrFunc_pm_poll},
    {MDMOID_GAL_ETHERNET_PM_HISTORY_DATA,WAIT_GAL_ENET_TIME,
      "omcipmd_GAL_ENET", tmrFunc_pm_poll},
    {MDMOID_RTP_PM_HISTORY_DATA, WAIT_VOIP_TIME,
      "omcipmd_VoIP", tmrFunc_pm_poll},
    {MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA, WAIT_MOCA_ENET_TIME,
      "omcipmd_MOCA_ENET", tmrFunc_pm_poll},
    {MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA, WAIT_MOCA_INTF_TIME,
      "omcipmd_MOCA_INTF", tmrFunc_pm_poll},
    {MDMOID_IP_HOST_PM_HISTORY_DATA, WAIT_IP_HOST_TIME,
      "omcipmd_IP_HOST", tmrFunc_pm_poll},
    {MDMOID_ETHERNET_FRAME_EXTENDED_P_M, WAIT_EXTPM_TIME,
      "omcipmd_extPM", tmrFunc_pm_poll},
    {MDMOID_DONT_CARE, WAIT_15_MIN,
      "omcipmd_15", tmrFunc_15min},
    {MDMOID_DONT_CARE, WAIT_MULTICAST_TIME,
      "omcipmd_MULTICAST", tmrFunc_multicast},
    {MDMOID_DONT_CARE, WAIT_TEST_TIME,
      "omcipmd_TEST", tmrFunc_test},
    {MDMOID_DONT_CARE, WAIT_ALARM_TEST_TIME,
      "omcipmd_ALARM_ENET", tmrFunc_alarm_enet},
    {MDMOID_DONT_CARE, WAIT_ALARM_TEST_TIME,
      "omcipmd_ALARM_GPON", tmrFunc_alarm_gpon},
    {MDMOID_DONT_CARE, WAIT_ALARM_ARC_TIME,
      "omcipmd_ALARM_OPTIC", tmrFunc_alarm_optic},
    {MDMOID_DONT_CARE, WAIT_ALARM_ARC_TIME,
      "omcipmd_ALARM_ARC", tmrFunc_alarm_arc},
    {MDMOID_MAC_BRIDGE_PM_HISTORY_DATA, WAIT_MAC_BRIDGE_TIME,
      "omcipmd_MAC_BRIDGE", tmrFunc_pm_poll},
    {MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA, WAIT_ENET_TIME,
      "omcipmd_MAC_BRIDGE_PORT", tmrFunc_pm_poll},
};


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */

static CmsTimestamp omci_pm_beginTms;
static CmsTimestamp omci_pm_endTms;
static UINT32 omci_pm_remainInMs;


/* Timestamps for calibration. */
static CmsTimestamp omci_pm_beginTms;
static CmsTimestamp omci_pm_endTms;
static UINT32 omci_pm_remainInMs;


/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  tmrFunc_pm_poll
*  PURPOSE:   OMCI PM timer callback function.
*  PARAMETERS:
*      data - pointer to the callback context data.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void tmrFunc_pm_poll(void *data)
{
    CmsRet ret = CMSRET_SUCCESS;
    const OmciTmrInfo_t *pInfo = (OmciTmrInfo_t *)data;

    if (pInfo == NULL)
    {
        cmsLog_error("Cannot find timer information");
        return;
    }

    // Restart PM timer.
    ret = cmsTmr_set(pmTmrHandle, pInfo->tmrFunc, data,
      pInfo->expiredTime - omci_pm_remainInMs, pInfo->tmrName);
    // Test for failure.
    if (ret != CMSRET_SUCCESS)
    {
        // Log error.
        cmsLog_error("Timer restart failed, ret: %d", ret);
    }

    if (omci_data_trylock() == 0)
    {
        // Read & update PM stats.
        omci_pm_pollByObjectClass(pInfo->mdmOid);

        omci_data_unlock();
    }
}

/*****************************************************************************
*  FUNCTION:  tmrFunc_15min
*  PURPOSE:   OMCI PM timer callback function.
*  PARAMETERS:
*      data - pointer to the callback context data.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void tmrFunc_15min(void *data)
{
    CmsRet ret = CMSRET_SUCCESS;
    const OmciTmrInfo_t *pInfo = (OmciTmrInfo_t *)data;

    if (pInfo == NULL)
    {
        cmsLog_error("Cannot find timer information");
        return;
    }

    // Restart 15-minute timer.
    ret = cmsTmr_set(pmTmrHandle, pInfo->tmrFunc, data,
      pInfo->expiredTime - omci_pm_remainInMs, pInfo->tmrName);
    // Test for failure.
    if (ret != CMSRET_SUCCESS)
    {
        // Log error.
        cmsLog_error("Timer restart failed, ret: %d", ret);
    }

    if (omci_data_trylock() == 0)
    {
        // Signal 1st interval has occurred.
        omci_pm_setIntervalFlag(TRUE);

        // Process all objects that need special interval-end handling.
        omci_pm_setIntervalFlag(FALSE);

        // Inc Interval End value (0 - 0xFF).
        omci_pm_increaseIntervalCounter();

        // Init all stat buffers on interval (15-minute) switch.
        omci_pm_initStatBuffers(omci_pm_getIntervalCounter());

        omci_data_unlock();
    }
}

/*****************************************************************************
*  FUNCTION:  tmrFunc_multicast
*  PURPOSE:   OMCI PM timer callback function.
*  PARAMETERS:
*      data - pointer to the callback context data.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void tmrFunc_multicast(void *data)
{
    CmsRet ret = CMSRET_SUCCESS;
    const OmciTmrInfo_t *pInfo = (OmciTmrInfo_t *)data;

    if (pInfo == NULL)
    {
        cmsLog_error("Cannot find timer information");
        return;
    }

    // Restart Multicast timer.
    ret = cmsTmr_set(pmTmrHandle, pInfo->tmrFunc, data,
      pInfo->expiredTime - omci_pm_remainInMs, pInfo->tmrName);
    // Test for failure.
    if (ret != CMSRET_SUCCESS)
    {
        // Log error.
        cmsLog_error("Timer restart failed, ret: %d", ret);
    }

    if (omci_data_trylock() == 0)
    {
        // Read current Multicast stats from ENET driver.
        omci_pm_mcast_poll();

        omci_data_unlock();
    }
}

/*****************************************************************************
*  FUNCTION:  tmrFunc_test
*  PURPOSE:   OMCI PM timer callback function.
*  PARAMETERS:
*      data - pointer to the callback context data.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void tmrFunc_test(void *data)
{
    CmsRet ret = CMSRET_SUCCESS;
    const OmciTmrInfo_t *pInfo = (OmciTmrInfo_t *)data;

    if (pInfo == NULL)
    {
        cmsLog_error("Cannot find timer information");
        return;
    }

    // Restart TEST timer.
    ret = cmsTmr_set(pmTmrHandle, pInfo->tmrFunc, data,
      pInfo->expiredTime - omci_pm_remainInMs, pInfo->tmrName);
    // Test for failure.
    if (ret != CMSRET_SUCCESS)
    {
        // Log error.
        cmsLog_error("Timer restart failed, ret: %d", ret);
    }

    if (omci_data_trylock() == 0)
    {
        // poll TEST timer expired.
        omci_poll_test();

        omci_data_unlock();
    }
}

/*****************************************************************************
*  FUNCTION:  tmrFunc_alarm_enet
*  PURPOSE:   OMCI PM timer callback function.
*  PARAMETERS:
*      data - pointer to the callback context data.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void tmrFunc_alarm_enet(void *data)
{
    CmsRet ret = CMSRET_SUCCESS;
    const OmciTmrInfo_t *pInfo = (OmciTmrInfo_t *)data;

    if (pInfo == NULL)
    {
        cmsLog_error("Cannot find timer information");
        return;
    }

    // Restart ALARM_ENET timer.
    ret = cmsTmr_set(pmTmrHandle, pInfo->tmrFunc, data,
      pInfo->expiredTime - omci_pm_remainInMs, pInfo->tmrName);
    // Test for failure.
    if (ret != CMSRET_SUCCESS)
    {
        // Log error.
        cmsLog_error("Timer restart failed, ret: %d", ret);
    }

    if (omci_data_trylock() == 0)
    {
        omci_alarm_poll_enet();

        omci_data_unlock();
    }
}

/*****************************************************************************
*  FUNCTION:  tmrFunc_alarm_gpon
*  PURPOSE:   OMCI PM timer callback function.
*  PARAMETERS:
*      data - pointer to the callback context data.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void tmrFunc_alarm_gpon(void *data)
{
    CmsRet ret = CMSRET_SUCCESS;
    const OmciTmrInfo_t *pInfo = (OmciTmrInfo_t *)data;

    if (pInfo == NULL)
    {
        cmsLog_error("Cannot find timer information");
        return;
    }

    // Restart ALARM_GPON timer.
    ret = cmsTmr_set(pmTmrHandle, pInfo->tmrFunc, data,
      pInfo->expiredTime - omci_pm_remainInMs, pInfo->tmrName);
    // Test for failure.
    if (ret != CMSRET_SUCCESS)
    {
        // Log error.
        cmsLog_error("Timer restart failed, ret: %d", ret);
    }

    if (omci_data_trylock() == 0)
    {
        omci_alarm_poll_gpon();

        omci_data_unlock();
    }
}

/*****************************************************************************
*  FUNCTION:  tmrFunc_alarm_optic
*  PURPOSE:   OMCI PM timer callback function.
*  PARAMETERS:
*      data - pointer to the callback context data.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void tmrFunc_alarm_optic(void *data)
{
    CmsRet ret = CMSRET_SUCCESS;
    const OmciTmrInfo_t *pInfo = (OmciTmrInfo_t *)data;

    if (pInfo == NULL)
    {
        cmsLog_error("Cannot find timer information");
        return;
    }

    // Restart timer.
    ret = cmsTmr_set(pmTmrHandle, pInfo->tmrFunc, data,
      pInfo->expiredTime - omci_pm_remainInMs, pInfo->tmrName);
    // Test for failure.
    if (ret != CMSRET_SUCCESS)
    {
        // Log error.
        cmsLog_error("Timer restart failed, ret: %d", ret);
    }

    if (omci_data_trylock() == 0)
    {
        omci_alarm_poll_optical_signals();

        omci_data_unlock();
    }
}

/*****************************************************************************
*  FUNCTION:  tmrFunc_alarm_arc
*  PURPOSE:   OMCI PM timer callback function.
*  PARAMETERS:
*      data - pointer to the callback context data.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void tmrFunc_alarm_arc(void *data)
{
    CmsRet ret = CMSRET_SUCCESS;
    const OmciTmrInfo_t *pInfo = (OmciTmrInfo_t *)data;

    if (pInfo == NULL)
    {
        cmsLog_error("Cannot find timer information");
        return;
    }

    // Restart timer.
    ret = cmsTmr_set(pmTmrHandle, pInfo->tmrFunc, data,
                     pInfo->expiredTime - omci_pm_remainInMs, pInfo->tmrName);

    // Test for failure.
    if (ret != CMSRET_SUCCESS)
    {
        // Log error.
        cmsLog_error("Timer restart failed, ret: %d", ret);
    }

    if (omci_data_trylock() == 0)
    {
        omci_arc_poll();

        omci_data_unlock();
    }
}

/*****************************************************************************
*  FUNCTION:  omci_pm_setTimers
*  PURPOSE:   Start OMCI PM timers.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet omci_pm_setTimers(void)
{
    UINT32 i = 0;
    CmsRet ret = CMSRET_SUCCESS;

    // Test for timers already active (as in SYNC).
    if (pmTmrHandle != NULL)
    {
        // Stop ALL CMS timers.
        cmsTmr_cleanup(&pmTmrHandle);

        // Clear global CMS timer handle.
        pmTmrHandle = NULL;
    }

    // Init CMS timer handle.
    if ((ret = cmsTmr_init(&pmTmrHandle)) == CMSRET_SUCCESS)
    {
        for (i = 0; i < OMCI_TMR_MAX; i++)
        {
            ret  = cmsTmr_set(pmTmrHandle,
                              omciTmrInfo[i].tmrFunc,
                              (void *)&omciTmrInfo[i],
                              omciTmrInfo[i].expiredTime,
                              omciTmrInfo[i].tmrName);
            if (ret != CMSRET_SUCCESS)
                break;
        }
    }

    // Test for CMS timer failure.
    if (ret != CMSRET_SUCCESS)
    {
        // Signal error.
        cmsLog_error("CMS timer failure, ret: %d", ret);
    }

    // Return cummulative success or 1st failure result.
    return ret;
}

/*****************************************************************************
*  FUNCTION:  omci_pm_syncTimers
*  PURPOSE:   Actions for the OMCI Synchronize time command. Refer G.988
*             Clause 9.1.1.
*             - Synchronizes the start time.
*             - Clear all PM ME counters to 0.
*             - Set its PM interval counter to 0.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet omci_pm_syncTimers(void)
{
    CmsRet ret = CMSRET_SUCCESS;

    // Restart timers.
    ret = omci_pm_setTimers();

    if (ret == CMSRET_SUCCESS)
    {
        // Reset Interval Counter
        omci_pm_resetIntervalCounter();

        // Init PM Counters
        omci_pm_initStatBuffers(0);
        omci_pm_initStatBuffers(1);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omci_pm_calibratetimerInit
*  PURPOSE:   Init PM calibration timer omci_pm_beginTms. This timer is used
*             as base.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      To avoid overflow, reset omci_pm_beginTms.sec periodically. For
*      accuracy, always keep the usec from the base calibration timer.
*****************************************************************************/
void omci_pm_calibratetimerInit(void)
{
    static UBOOL8 initDone = FALSE;
    CmsTimestamp new_tms;

    cmsTms_get(&new_tms);
    omci_pm_beginTms.sec = new_tms.sec;
    if (initDone == FALSE)
    {
        omci_pm_beginTms.nsec = new_tms.nsec;
        initDone = TRUE;
    }
    omci_pm_remainInMs = 0;
}

/*****************************************************************************
*  FUNCTION:  omci_pm_calibratetimerCheck
*  PURPOSE:   Calculate PM calibration result.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_pm_calibratetimerCheck(void)
{
    UINT32 remainInMs;

    cmsTms_get(&omci_pm_endTms);
    remainInMs = cmsTms_deltaInMilliSeconds(&omci_pm_endTms,
      &omci_pm_beginTms) % MSECS_IN_SEC;
    omci_pm_remainInMs = remainInMs;
}

/*****************************************************************************
*  FUNCTION:  omci_pm_calibratetimerReset
*  PURPOSE:   Reset PM calibration result.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_pm_calibratetimerReset(void)
{
    omci_pm_remainInMs = 0;
}
