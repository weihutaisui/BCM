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

#ifdef DMP_X_ITU_ORG_GPON_1

#include <time.h>
#include <math.h>

#include "owsvc_api.h"

#include "omci_pm_llist.h"
#include "omci_pm_poll.h"
#include "omci_pm_sync.h"
#include "omci_pm_util.h"
#include "omci_th_check.h"

static UINT8 intervalCounter = 0;

static UBOOL8 intervalFlag = FALSE;

/*========================= PRIVATE FUNCTIONS ==========================*/


//#define OMCIPM_TEST

#ifdef OMCIPM_TEST
static int getRandomNumber(int from, int to)
{
    int num = 0;

    srand((unsigned int)time((time_t *)NULL));
    num = (rand() % (to - from)) + from;

    return num;
}
#endif


static void pm_poll_gem_port(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_GEM_PORT_COUNTER_64* activeStatPtr;
    BCM_OMCI_PM_GEM_PORT_COUNTER_64* baseStatPtr;
    BCM_OMCI_PM_GEM_PORT_COUNTER_64 localStatRec_64;
    BCM_OMCI_PM_GEM_PORT_COUNTER localStatRec;
    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;
    UINT32 rxBytesDelta32;
    UINT32 txBytesDelta32;

    memset(&localStatRec_64, 0x0, sizeof(BCM_OMCI_PM_GEM_PORT_COUNTER_64));
    memset(&localStatRec, 0x0, sizeof(BCM_OMCI_PM_GEM_PORT_COUNTER));

    // Test for valid port number.
    if (omcipmEntry->omcipm.portID >= GPON_PORT_MAX)
    {
        // Attempt to find valid port.
        omcipmEntry->omcipm.portID = omci_pm_findGemPort(omcipmEntry->omcipm.objID);
    }
    else
    {
        // Setup current stat buffer pointer.
        activeStatPtr = (BCM_OMCI_PM_GEM_PORT_COUNTER_64*)omcipmEntry->pm[intervalCounter & 1];

        // Test for valid port.
        if (omcipmEntry->omcipm.portID != INVALID_GPON_PORT)
        {
            // Read current GPON stats into local stats rec.
#ifndef OMCIPM_TEST
            result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_GEM_PORT, omcipmEntry->omcipm.portID, &localStatRec);
#endif    // OMCIPM_TEST

            // Test for bcm_omcipm_getCounters success.
            if (result == OMCI_PM_STATUS_SUCCESS)
            {
                // Convert 32-bit stat rec to 64-bit stat rec.
                localStatRec_64.transmittedGEMFrames = localStatRec.transmittedGEMFrames;
                localStatRec_64.receivedGEMFrames = localStatRec.receivedGEMFrames;
                localStatRec_64.receivedPayloadBytes_32 = localStatRec.receivedPayloadBytes;
                localStatRec_64.transmittedPayloadBytes_32 = localStatRec.transmittedPayloadBytes;

                // Setup base stats buffer pointer.
                baseStatPtr = (BCM_OMCI_PM_GEM_PORT_COUNTER_64*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];
#ifdef OMCIPM_TEST
localStatRec_64.transmittedGEMFrames = baseStatPtr->transmittedGEMFrames + getRandomNumber(0, 64);
localStatRec_64.receivedGEMFrames = baseStatPtr->receivedGEMFrames + getRandomNumber(65, 128);
localStatRec_64.transmittedPayloadBytes_32 = baseStatPtr->transmittedPayloadBytes_32 + getRandomNumber(129, 256);
localStatRec_64.receivedPayloadBytes_32 = baseStatPtr->receivedPayloadBytes_32 + getRandomNumber(257, 512);
localStatRec_64.transmittedPayloadBytes_64 = baseStatPtr->transmittedPayloadBytes_64 + getRandomNumber(129, 256);
localStatRec_64.receivedPayloadBytes_64 = baseStatPtr->receivedPayloadBytes_64 + getRandomNumber(65, 128);
#endif    // OMCIPM_TEST

                // Test for operational PM object.
                if (omcipmEntry->initBaseFlag != 0)
                {
                    // Inc GPON stats (ceiling on overflow) with delta between base value and current value.
                    omci_pm_incStatWithCeiling(&activeStatPtr->transmittedGEMFrames, omci_pm_calcStatDelta(baseStatPtr->transmittedGEMFrames, localStatRec.transmittedGEMFrames));
                    omci_pm_incStatWithCeiling(&activeStatPtr->receivedGEMFrames, omci_pm_calcStatDelta(baseStatPtr->receivedGEMFrames, localStatRec.receivedGEMFrames));

                    // Calc 32-bit delta values from current stat & base (since hw register .
                    rxBytesDelta32 = (UINT64)omci_pm_calcStatDelta(baseStatPtr->receivedPayloadBytes_32, localStatRec_64.receivedPayloadBytes_32);
                    txBytesDelta32 = (UINT64)omci_pm_calcStatDelta(baseStatPtr->transmittedPayloadBytes_32, localStatRec_64.transmittedPayloadBytes_32);

                    // Inc 64-bit stats with ceiling.
                    omci_pm_incStatWithCeiling_64(NULL, NULL, &activeStatPtr->receivedPayloadBytes_64, rxBytesDelta32);
                    omci_pm_incStatWithCeiling_64(NULL, NULL, &activeStatPtr->transmittedPayloadBytes_64, txBytesDelta32);

                    // Test for debug.
                    if (omcipmEntry->debugFlag != FALSE)
                    {
                        // Output latest 32-bit GPON stats.
                        omci_pm_debugStatOutput("GPON TX GEM Frames   ", localStatRec.transmittedGEMFrames, baseStatPtr->transmittedGEMFrames, activeStatPtr->transmittedGEMFrames);
                        omci_pm_debugStatOutput("GPON RX GEM Frames   ", localStatRec.receivedGEMFrames, baseStatPtr->receivedGEMFrames, activeStatPtr->receivedGEMFrames);

                        // Output latest 64-bit GPON stats.
                        omci_pm_debugStatOutput_64("GPON RX Payload Bytes  ", localStatRec_64.receivedPayloadBytes_64, baseStatPtr->receivedPayloadBytes_64, rxBytesDelta32);
                        omci_pm_debugStatOutput_64("GPON TX Payload Bytes  ", localStatRec_64.transmittedPayloadBytes_64, baseStatPtr->transmittedPayloadBytes_64, txBytesDelta32);
                    }
                }
                else
                {
                    // Signal omcipmEntry stats initialization has occured.
                    omcipmEntry->initBaseFlag = 1;
                }

                // Reset (or init) omcipmEntry Base stats rec.
                memcpy(baseStatPtr, &localStatRec_64, sizeof(BCM_OMCI_PM_GEM_PORT_COUNTER_64));
            }
            else
            {
                // Signal bcm_omcipm_getCounters error.
                cmsLog_error("bcm_omcipm_getCounters error: %d", result);
            }
        }
    }
}


static void pm_poll_fec(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_FEC_COUNTER* activeStatPtr;
    BCM_OMCI_PM_FEC_COUNTER* baseStatPtr;
    BCM_OMCI_PM_FEC_COUNTER localStatRec;
    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;

    // Setup current stat buffer pointer.
    activeStatPtr = (BCM_OMCI_PM_FEC_COUNTER*)omcipmEntry->pm[intervalCounter & 1];

    // Read current FEC stats into local stats rec.
#ifndef OMCIPM_TEST
    result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_FEC, omcipmEntry->omcipm.portID, &localStatRec);
#endif    // OMCIPM_TEST

    // Test for bcm_omcipm_getCounters success.
    if (result == OMCI_PM_STATUS_SUCCESS)
    {
        // Setup pointer to base stats.
        baseStatPtr = (BCM_OMCI_PM_FEC_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];
#ifdef OMCIPM_TEST
localStatRec.correctedBytes = baseStatPtr->correctedBytes + getRandomNumber(0, 64);
localStatRec.correctedCodeWords = baseStatPtr->correctedCodeWords + getRandomNumber(65, 128);
localStatRec.uncorrectedCodeWords = baseStatPtr->uncorrectedCodeWords + getRandomNumber(129, 256);
localStatRec.totalCodeWords = baseStatPtr->totalCodeWords + getRandomNumber(257, 512);
localStatRec.fecSeconds = baseStatPtr->fecSeconds + getRandomNumber(129, 256);
#endif    // OMCIPM_TEST

        // Test for operational PM object.
        if (omcipmEntry->initBaseFlag != 0)
        {
            // Inc FEC stats (ceiling on overflow) with delta between base value and current value.
            omci_pm_incStatWithCeiling(&activeStatPtr->correctedBytes, omci_pm_calcStatDelta(baseStatPtr->correctedBytes, localStatRec.correctedBytes));
            omci_pm_incStatWithCeiling(&activeStatPtr->correctedCodeWords, omci_pm_calcStatDelta(baseStatPtr->correctedCodeWords, localStatRec.correctedCodeWords));
            omci_pm_incStatWithCeiling(&activeStatPtr->uncorrectedCodeWords, omci_pm_calcStatDelta(baseStatPtr->uncorrectedCodeWords, localStatRec.uncorrectedCodeWords));
            omci_pm_incStatWithCeiling(&activeStatPtr->totalCodeWords, omci_pm_calcStatDelta(baseStatPtr->totalCodeWords, localStatRec.totalCodeWords));
            omci_pm_incStatWithCeiling(&activeStatPtr->fecSeconds, omci_pm_calcStatDelta(baseStatPtr->fecSeconds, localStatRec.fecSeconds));

            // Test for debug.
            if (omcipmEntry->debugFlag != FALSE)
            {
                // Output latest FEC stats.
                omci_pm_debugStatOutput("FEC correctedBytes         ", localStatRec.correctedBytes, baseStatPtr->correctedBytes, activeStatPtr->correctedBytes);
                omci_pm_debugStatOutput("FEC correctedCodeWords     ", localStatRec.correctedCodeWords, baseStatPtr->correctedCodeWords, activeStatPtr->correctedCodeWords);
                omci_pm_debugStatOutput("FEC uncorrectedCodeWords   ", localStatRec.uncorrectedCodeWords, baseStatPtr->uncorrectedCodeWords, activeStatPtr->uncorrectedCodeWords);
                omci_pm_debugStatOutput("FEC totalCodeWords         ", localStatRec.totalCodeWords, baseStatPtr->totalCodeWords, activeStatPtr->totalCodeWords);
                omci_pm_debugStatOutput("FEC fecSeconds             ", localStatRec.fecSeconds, baseStatPtr->fecSeconds, activeStatPtr->fecSeconds);
            }

            // Test for FEC counters that cross over threshold.
            omci_th_check(omcipmEntry, (const void*) activeStatPtr);
        }
        else
        {
            // Signal omcipmEntry stats initialization has occured.
            omcipmEntry->initBaseFlag = 1;
        }

        // Reset (or init) omcipmEntry Base stats rec.
        memcpy(baseStatPtr, &localStatRec, sizeof(BCM_OMCI_PM_FEC_COUNTER));
    }
    else
    {
        // Signal bcm_omcipm_getCounters error.
        cmsLog_error("bcm_omcipm_getCounters error: %d", result);
    }
}

static void pm_poll_mac_bridge(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_MAC_BRIDGE_COUNTER* activeStatPtr;
    BCM_OMCI_PM_MAC_BRIDGE_COUNTER* baseStatPtr;
    BCM_OMCI_PM_MAC_BRIDGE_COUNTER localStatRec;
    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;

    // Setup current stat buffer pointer.
    activeStatPtr = (BCM_OMCI_PM_MAC_BRIDGE_COUNTER*)omcipmEntry->pm[intervalCounter & 1];

    // Read current MAC bridge stats into local stats rec.
#ifndef OMCIPM_TEST
    result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_BRIDGE, omcipmEntry->omcipm.objID, &localStatRec);
#endif    // OMCIPM_TEST

    // Test for bcm_omcipm_getCounters success.
    if (result == OMCI_PM_STATUS_SUCCESS)
    {
        // Setup pointer to base stats.
        baseStatPtr = (BCM_OMCI_PM_MAC_BRIDGE_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];
#ifdef OMCIPM_TEST
localStatRec.learningDiscaredEntries = baseStatPtr->learningDiscaredEntries + getRandomNumber(1, 64);
#endif    // OMCIPM_TEST

        // Test for operational PM object.
        if (omcipmEntry->initBaseFlag != 0)
        {
            // Inc MAC bridge stats (ceiling on overflow) with delta between base value and current value.
            omci_pm_incStatWithCeiling(&activeStatPtr->learningDiscaredEntries, omci_pm_calcStatDelta(baseStatPtr->learningDiscaredEntries, localStatRec.learningDiscaredEntries));

            // Test for debug.
            if (omcipmEntry->debugFlag != FALSE)
            {
                // Output latest MAC bridge stats.
                omci_pm_debugStatOutput("MAC bridge learning discard entries", localStatRec.learningDiscaredEntries, baseStatPtr->learningDiscaredEntries, activeStatPtr->learningDiscaredEntries);
            }

            // Test for MAC bridge counters that cross over threshold.
            omci_th_check(omcipmEntry, (const void*) activeStatPtr);
        }
        else
        {
            // Signal omcipmEntry stats initialization has occured.
            omcipmEntry->initBaseFlag = 1;
        }

        // Reset (or init) omcipmEntry Base stats rec.
        memcpy(baseStatPtr, &localStatRec, sizeof(BCM_OMCI_PM_MAC_BRIDGE_COUNTER));
    }
    else
    {
        // Signal bcm_omcipm_getCounters error.
        cmsLog_error("bcm_omcipm_getCounters error: %d", result);
    }
}

static void pm_poll_mac_bridge_port(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER* activeStatPtr;
    BCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER* baseStatPtr;
    BCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER localStatRec;
    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;

    // Setup current stat buffer pointer.
    activeStatPtr = (BCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER*)omcipmEntry->pm[intervalCounter & 1];

    // Read current MAC bridge port stats into local stats rec.
#ifndef OMCIPM_TEST
    result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_BRIDGE_PORT, omcipmEntry->omcipm.portID, &localStatRec);
#endif    // OMCIPM_TEST

    // Test for bcm_omcipm_getCounters success.
    if (result == OMCI_PM_STATUS_SUCCESS)
    {
        // Setup pointer to base stats.
        baseStatPtr = (BCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];
#ifdef OMCIPM_TEST
        localStatRec.delayDiscardedFrames = baseStatPtr->delayDiscardedFrames + getRandomNumber(1, 64);
        localStatRec.forwardedFrames = baseStatPtr->forwardedFrames + getRandomNumber(1, 64);
        localStatRec.mtuDiscardedFrames = baseStatPtr->mtuDiscardedFrames + getRandomNumber(1, 64);
        localStatRec.receivedDiscardedFrames = baseStatPtr->receivedDiscardedFrames + getRandomNumber(1, 64);
        localStatRec.receivedFrames = baseStatPtr->receivedFrames + getRandomNumber(1, 64);
#endif    // OMCIPM_TEST

        // Test for operational PM object.
        if (omcipmEntry->initBaseFlag != 0)
        {
            // Inc MAC bridge port stats (ceiling on overflow) with delta between base value and current value.
            omci_pm_incStatWithCeiling(&activeStatPtr->delayDiscardedFrames, omci_pm_calcStatDelta(baseStatPtr->delayDiscardedFrames, localStatRec.delayDiscardedFrames));
            omci_pm_incStatWithCeiling(&activeStatPtr->forwardedFrames, omci_pm_calcStatDelta(baseStatPtr->forwardedFrames, localStatRec.forwardedFrames));
            omci_pm_incStatWithCeiling(&activeStatPtr->mtuDiscardedFrames, omci_pm_calcStatDelta(baseStatPtr->mtuDiscardedFrames, localStatRec.mtuDiscardedFrames));
            omci_pm_incStatWithCeiling(&activeStatPtr->receivedDiscardedFrames, omci_pm_calcStatDelta(baseStatPtr->receivedDiscardedFrames, localStatRec.receivedDiscardedFrames));
            omci_pm_incStatWithCeiling(&activeStatPtr->receivedFrames, omci_pm_calcStatDelta(baseStatPtr->receivedFrames, localStatRec.receivedFrames));

            // Test for debug.
            if (omcipmEntry->debugFlag != FALSE)
            {
                // Output latest MAC bridge port stats.
                omci_pm_debugStatOutput("MAC bridge port delay discard frames", localStatRec.delayDiscardedFrames, baseStatPtr->delayDiscardedFrames, activeStatPtr->delayDiscardedFrames);
                omci_pm_debugStatOutput("MAC bridge port forwarded frames", localStatRec.forwardedFrames, baseStatPtr->forwardedFrames, activeStatPtr->forwardedFrames);
                omci_pm_debugStatOutput("MAC bridge port MTU discard frames", localStatRec.mtuDiscardedFrames, baseStatPtr->mtuDiscardedFrames, activeStatPtr->mtuDiscardedFrames);
                omci_pm_debugStatOutput("MAC bridge port received discard frames", localStatRec.receivedDiscardedFrames, baseStatPtr->receivedDiscardedFrames, activeStatPtr->receivedDiscardedFrames);
                omci_pm_debugStatOutput("MAC bridge port received frames", localStatRec.receivedFrames, baseStatPtr->receivedFrames, activeStatPtr->receivedFrames);
            }

            // Test for MAC bridge port counters that cross over threshold.
            omci_th_check(omcipmEntry, (const void*) activeStatPtr);
        }
        else
        {
            // Signal omcipmEntry stats initialization has occured.
            omcipmEntry->initBaseFlag = 1;
        }

        // Reset (or init) omcipmEntry Base stats rec.
        memcpy(baseStatPtr, &localStatRec, sizeof(BCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER));
    }
    else
    {
        // Signal bcm_omcipm_getCounters error.
        cmsLog_error("bcm_omcipm_getCounters error: %d", result);
    }
}


static void pm_poll_enet(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_ETHERNET_COUNTER* activeStatPtr;
    BCM_OMCI_PM_ETHERNET_COUNTER* baseStatPtr;
    BCM_OMCI_PM_ETHERNET_COUNTER localStatRec;
    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;

    // Test for valid port number.
    if (omcipmEntry->omcipm.portID < ethLanNum || omcipmEntry->omcipm.portID == PORT_VEIP)
    {
        // Setup current stat buffer pointer.
        activeStatPtr = (BCM_OMCI_PM_ETHERNET_COUNTER*)omcipmEntry->pm[intervalCounter & 1];

        // Read current ENET stats into local stats rec & test for success.
#ifndef OMCIPM_TEST
        result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENET, omcipmEntry->omcipm.portID, &localStatRec);
#endif    // OMCIPM_TEST

        if (result == OMCI_PM_STATUS_SUCCESS)
        {
            // Setup base stats buffer pointer.
            baseStatPtr = (BCM_OMCI_PM_ETHERNET_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];
#ifdef OMCIPM_TEST
localStatRec.fcsErrors = baseStatPtr->fcsErrors + getRandomNumber(0, 64);
localStatRec.excessiveCollisionCounter = baseStatPtr->excessiveCollisionCounter + getRandomNumber(65, 128);
localStatRec.lateCollisionCounter = baseStatPtr->lateCollisionCounter + getRandomNumber(129, 256);
localStatRec.frameTooLongs = baseStatPtr->frameTooLongs + getRandomNumber(257, 512);
localStatRec.bufferOverflowsOnReceive = baseStatPtr->bufferOverflowsOnReceive + getRandomNumber(129, 256);
localStatRec.bufferOverflowsOnTransmit = baseStatPtr->bufferOverflowsOnTransmit + getRandomNumber(65, 128);
localStatRec.singleCollisionFrameCounter = baseStatPtr->singleCollisionFrameCounter + getRandomNumber(0, 64);
localStatRec.multipleCollisionsFrameCounter = baseStatPtr->multipleCollisionsFrameCounter + getRandomNumber(65, 128);
localStatRec.sqeCounter = baseStatPtr->sqeCounter + getRandomNumber(129, 256);
localStatRec.deferredTransmissionCounter = baseStatPtr->deferredTransmissionCounter + getRandomNumber(257, 512);
localStatRec.internalMacTransmitErrorCounter = baseStatPtr->internalMacTransmitErrorCounter + getRandomNumber(129, 256);
localStatRec.carrierSenseErrorCounter = baseStatPtr->carrierSenseErrorCounter + getRandomNumber(65, 128);
localStatRec.alignmentErrorCounter = baseStatPtr->alignmentErrorCounter + getRandomNumber(0, 64);
localStatRec.internalMacReceiveErrorCounter = baseStatPtr->internalMacReceiveErrorCounter + getRandomNumber(65, 128);
#endif    // OMCIPM_TEST

            // Test if base stats have been initialized.
            if (omcipmEntry->initBaseFlag != 0)
            {
                // Inc ENET stats (ceiling on overflow) with delta between base value and current value.
                omci_pm_incStatWithCeiling(&activeStatPtr->fcsErrors, omci_pm_calcStatDelta(baseStatPtr->fcsErrors, localStatRec.fcsErrors));
                omci_pm_incStatWithCeiling(&activeStatPtr->excessiveCollisionCounter, omci_pm_calcStatDelta(baseStatPtr->excessiveCollisionCounter, localStatRec.excessiveCollisionCounter));
                omci_pm_incStatWithCeiling(&activeStatPtr->lateCollisionCounter, omci_pm_calcStatDelta(baseStatPtr->lateCollisionCounter, localStatRec.lateCollisionCounter));
                omci_pm_incStatWithCeiling(&activeStatPtr->frameTooLongs, omci_pm_calcStatDelta(baseStatPtr->frameTooLongs, localStatRec.frameTooLongs));
                omci_pm_incStatWithCeiling(&activeStatPtr->bufferOverflowsOnReceive, omci_pm_calcStatDelta(baseStatPtr->bufferOverflowsOnReceive, localStatRec.bufferOverflowsOnReceive));
                omci_pm_incStatWithCeiling(&activeStatPtr->bufferOverflowsOnTransmit, omci_pm_calcStatDelta(baseStatPtr->bufferOverflowsOnTransmit, localStatRec.bufferOverflowsOnTransmit));
                omci_pm_incStatWithCeiling(&activeStatPtr->singleCollisionFrameCounter, omci_pm_calcStatDelta(baseStatPtr->singleCollisionFrameCounter, localStatRec.singleCollisionFrameCounter));
                omci_pm_incStatWithCeiling(&activeStatPtr->multipleCollisionsFrameCounter, omci_pm_calcStatDelta(baseStatPtr->multipleCollisionsFrameCounter, localStatRec.multipleCollisionsFrameCounter));
                omci_pm_incStatWithCeiling(&activeStatPtr->sqeCounter, omci_pm_calcStatDelta(baseStatPtr->sqeCounter, localStatRec.sqeCounter));
                omci_pm_incStatWithCeiling(&activeStatPtr->deferredTransmissionCounter, omci_pm_calcStatDelta(baseStatPtr->deferredTransmissionCounter, localStatRec.deferredTransmissionCounter));
                omci_pm_incStatWithCeiling(&activeStatPtr->internalMacTransmitErrorCounter, omci_pm_calcStatDelta(baseStatPtr->internalMacTransmitErrorCounter, localStatRec.internalMacTransmitErrorCounter));
                omci_pm_incStatWithCeiling(&activeStatPtr->carrierSenseErrorCounter, omci_pm_calcStatDelta(baseStatPtr->carrierSenseErrorCounter, localStatRec.carrierSenseErrorCounter));
                omci_pm_incStatWithCeiling(&activeStatPtr->alignmentErrorCounter, omci_pm_calcStatDelta(baseStatPtr->alignmentErrorCounter, localStatRec.alignmentErrorCounter));
                omci_pm_incStatWithCeiling(&activeStatPtr->internalMacReceiveErrorCounter, omci_pm_calcStatDelta(baseStatPtr->internalMacReceiveErrorCounter, localStatRec.internalMacReceiveErrorCounter));

                // Test for debug.
                if (omcipmEntry->debugFlag != FALSE)
                {
                    // Output latest ENET stats.
                    omci_pm_debugStatOutput("ENET fcsErrors\n  ", localStatRec.fcsErrors, baseStatPtr->fcsErrors, activeStatPtr->fcsErrors);
                    omci_pm_debugStatOutput("ENET excessiveCollisionCounter\n  ", localStatRec.excessiveCollisionCounter, baseStatPtr->excessiveCollisionCounter, activeStatPtr->excessiveCollisionCounter);
                    omci_pm_debugStatOutput("ENET lateCollisionCounter\n  ", localStatRec.lateCollisionCounter, baseStatPtr->lateCollisionCounter, activeStatPtr->lateCollisionCounter);
                    omci_pm_debugStatOutput("ENET frameTooLongs\n  ", localStatRec.frameTooLongs, baseStatPtr->frameTooLongs, activeStatPtr->frameTooLongs);
                    omci_pm_debugStatOutput("ENET bufferOverflowsOnReceive\n  ", localStatRec.bufferOverflowsOnReceive, baseStatPtr->bufferOverflowsOnReceive, activeStatPtr->bufferOverflowsOnReceive);
                    omci_pm_debugStatOutput("ENET bufferOverflowsOnTransmit\n  ", localStatRec.bufferOverflowsOnTransmit, baseStatPtr->bufferOverflowsOnTransmit, activeStatPtr->bufferOverflowsOnTransmit);
                    omci_pm_debugStatOutput("ENET singleCollisionFrameCounter\n  ", localStatRec.singleCollisionFrameCounter, baseStatPtr->singleCollisionFrameCounter, activeStatPtr->singleCollisionFrameCounter);
                    omci_pm_debugStatOutput("ENET multipleCollisionsFrameCounter\n  ", localStatRec.multipleCollisionsFrameCounter, baseStatPtr->multipleCollisionsFrameCounter, activeStatPtr->multipleCollisionsFrameCounter);
                    omci_pm_debugStatOutput("ENET sqeCounter\n  ", localStatRec.sqeCounter, baseStatPtr->sqeCounter, activeStatPtr->sqeCounter);
                    omci_pm_debugStatOutput("ENET deferredTransmissionCounter\n  ", localStatRec.deferredTransmissionCounter, baseStatPtr->deferredTransmissionCounter, activeStatPtr->deferredTransmissionCounter);
                    omci_pm_debugStatOutput("ENET internalMacTransmitErrorCounter\n  ", localStatRec.internalMacTransmitErrorCounter, baseStatPtr->internalMacTransmitErrorCounter, activeStatPtr->internalMacTransmitErrorCounter);
                    omci_pm_debugStatOutput("ENET carrierSenseErrorCounter\n  ", localStatRec.carrierSenseErrorCounter, baseStatPtr->carrierSenseErrorCounter, activeStatPtr->carrierSenseErrorCounter);
                    omci_pm_debugStatOutput("ENET alignmentErrorCounter\n  ", localStatRec.alignmentErrorCounter, baseStatPtr->alignmentErrorCounter, activeStatPtr->alignmentErrorCounter);
                    omci_pm_debugStatOutput("ENET internalMacReceiveErrorCounter\n  ", localStatRec.internalMacReceiveErrorCounter, baseStatPtr->internalMacReceiveErrorCounter, activeStatPtr->internalMacReceiveErrorCounter);
                }

                // Test for ENET counters that cross over threshold.
                omci_th_check(omcipmEntry, activeStatPtr);
            }
            else
            {
                // Signal base stats initialization has occured.
                omcipmEntry->initBaseFlag = 1;
            }

            // Reset base stats rec.
            memcpy(baseStatPtr, &localStatRec, sizeof(BCM_OMCI_PM_ETHERNET_COUNTER));
        }
        else
        {
            // Signal bcm_omcipm_getCounters error.
            cmsLog_error("bcm_omcipm_getCounters error");
        }
    }
}


static void pm_poll_enet2(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_ETHERNET_2_COUNTER* activeStatPtr;
    BCM_OMCI_PM_ETHERNET_2_COUNTER* baseStatPtr;
    BCM_OMCI_PM_ETHERNET_2_COUNTER localStatRec;
    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;

    // Test for valid port number.
    if (omcipmEntry->omcipm.portID < ethLanNum || omcipmEntry->omcipm.portID == PORT_VEIP)
    {
        // Setup current stat buffer pointer.
        activeStatPtr = (BCM_OMCI_PM_ETHERNET_2_COUNTER*)omcipmEntry->pm[intervalCounter & 1];

        // Read current ENET2 stats into local stats rec.
#ifndef OMCIPM_TEST
        result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENET2, omcipmEntry->omcipm.portID, &localStatRec);
#endif    // OMCIPM_TEST

        // Test for bcm_omcipm_getCounters success.
        if (result == OMCI_PM_STATUS_SUCCESS)
        {
            // Setup base stats buffer pointer.
            baseStatPtr = (BCM_OMCI_PM_ETHERNET_2_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];
#ifdef OMCIPM_TEST
localStatRec.pppoeFilterFrameCounter = baseStatPtr->pppoeFilterFrameCounter + getRandomNumber(0, 255);
#endif    // OMCIPM_TEST

            // Test for operational PM object.
            if (omcipmEntry->initBaseFlag != 0)
            {
                // Inc ENET2 stats (ceiling on overflow) with delta between base value and current value.
                omci_pm_incStatWithCeiling(&activeStatPtr->pppoeFilterFrameCounter, omci_pm_calcStatDelta(baseStatPtr->pppoeFilterFrameCounter, localStatRec.pppoeFilterFrameCounter));

                // Test for debug.
                if (omcipmEntry->debugFlag != FALSE)
                {
                    // Output latest ENET2 stats.
                    omci_pm_debugStatOutput("ENET2 pppoeFilterFrameCounter", localStatRec.pppoeFilterFrameCounter, baseStatPtr->pppoeFilterFrameCounter, activeStatPtr->pppoeFilterFrameCounter);
                }

                // Test for ENET2 counters that cross over threshold.
                omci_th_check(omcipmEntry, activeStatPtr);
            }
            else
            {
                // Signal base stats initialization has occured.
                omcipmEntry->initBaseFlag = 1;
            }

            // Reset omcipmEntry Base stats rec.
            memcpy(baseStatPtr, &localStatRec, sizeof(BCM_OMCI_PM_ETHERNET_2_COUNTER));
        }
        else
        {
            // Signal bcm_omcipm_getCounters error.
            cmsLog_error("bcm_omcipm_getCounters error: %d", result);
        }
    }
}


static void pm_poll_enet3(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_ETHERNET_3_COUNTER* activeStatPtr;
    BCM_OMCI_PM_ETHERNET_3_COUNTER* baseStatPtr;
    BCM_OMCI_PM_ETHERNET_3_COUNTER localStatRec;
    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;

    // Test for valid port number.
    if (omcipmEntry->omcipm.portID < ethLanNum || omcipmEntry->omcipm.portID == PORT_VEIP)
    {
        // Setup current stat buffer pointer.
        activeStatPtr = (BCM_OMCI_PM_ETHERNET_3_COUNTER*)omcipmEntry->pm[intervalCounter & 1];

        // Read current ENET3 stats into local stats rec.
#ifndef OMCIPM_TEST
        result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENET3, omcipmEntry->omcipm.portID, &localStatRec);
#endif    // OMCIPM_TEST

        // Test for bcm_omcipm_getCounters success.
        if (result == OMCI_PM_STATUS_SUCCESS)
        {
            // Setup base stats buffer pointer.
            baseStatPtr = (BCM_OMCI_PM_ETHERNET_3_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];
#ifdef OMCIPM_TEST
localStatRec.dropEvents = baseStatPtr->dropEvents + getRandomNumber(0, 64);
localStatRec.octets = baseStatPtr->octets + getRandomNumber(65, 128);
localStatRec.packets = baseStatPtr->packets + getRandomNumber(129, 256);
localStatRec.broadcastPackets = baseStatPtr->broadcastPackets + getRandomNumber(257, 512);
localStatRec.multicastPackets = baseStatPtr->multicastPackets + getRandomNumber(129, 256);
localStatRec.undersizePackets = baseStatPtr->undersizePackets + getRandomNumber(65, 128);
localStatRec.fragments = baseStatPtr->fragments + getRandomNumber(0, 64);
localStatRec.jabbers = baseStatPtr->jabbers + getRandomNumber(65, 128);
localStatRec.packets64Octets = baseStatPtr->packets64Octets + getRandomNumber(129, 256);
localStatRec.packets127Octets = baseStatPtr->packets127Octets + getRandomNumber(257, 512);
localStatRec.packets255Octets = baseStatPtr->packets255Octets + getRandomNumber(129, 256);
localStatRec.packets511Octets = baseStatPtr->packets511Octets + getRandomNumber(65, 128);
localStatRec.packets1023Octets = baseStatPtr->packets1023Octets + getRandomNumber(0, 64);
localStatRec.packets1518Octets = baseStatPtr->packets1518Octets + getRandomNumber(65, 128);
#endif    // OMCIPM_TEST

            // Test if base stats have been initialized.
            if (omcipmEntry->initBaseFlag != 0)
            {
                // Inc ENET3 stats (ceiling on overflow) with delta between base value and current value.
                omci_pm_incStatWithCeiling(&activeStatPtr->dropEvents, omci_pm_calcStatDelta(baseStatPtr->dropEvents, localStatRec.dropEvents));
                omci_pm_incStatWithCeiling(&activeStatPtr->octets, omci_pm_calcStatDelta(baseStatPtr->octets, localStatRec.octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets, omci_pm_calcStatDelta(baseStatPtr->packets, localStatRec.packets));
                omci_pm_incStatWithCeiling(&activeStatPtr->broadcastPackets, omci_pm_calcStatDelta(baseStatPtr->broadcastPackets, localStatRec.broadcastPackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->multicastPackets, omci_pm_calcStatDelta(baseStatPtr->multicastPackets, localStatRec.multicastPackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->undersizePackets, omci_pm_calcStatDelta(baseStatPtr->undersizePackets, localStatRec.undersizePackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->fragments, omci_pm_calcStatDelta(baseStatPtr->fragments, localStatRec.fragments));
                omci_pm_incStatWithCeiling(&activeStatPtr->jabbers, omci_pm_calcStatDelta(baseStatPtr->jabbers, localStatRec.jabbers));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets64Octets, omci_pm_calcStatDelta(baseStatPtr->packets64Octets, localStatRec.packets64Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets127Octets, omci_pm_calcStatDelta(baseStatPtr->packets127Octets, localStatRec.packets127Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets255Octets, omci_pm_calcStatDelta(baseStatPtr->packets255Octets, localStatRec.packets255Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets511Octets, omci_pm_calcStatDelta(baseStatPtr->packets511Octets, localStatRec.packets511Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets1023Octets, omci_pm_calcStatDelta(baseStatPtr->packets1023Octets, localStatRec.packets1023Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets1518Octets, omci_pm_calcStatDelta(baseStatPtr->packets1518Octets, localStatRec.packets1518Octets));

                // Test for debug.
                if (omcipmEntry->debugFlag != FALSE)
                {
                    // Output latest ENET3 stats.
                    omci_pm_debugStatOutput("ENET3 dropEvents       ", localStatRec.dropEvents, baseStatPtr->dropEvents, activeStatPtr->dropEvents);
                    omci_pm_debugStatOutput("ENET3 octets           ", localStatRec.octets, baseStatPtr->octets, activeStatPtr->octets);
                    omci_pm_debugStatOutput("ENET3 packets          ", localStatRec.packets, baseStatPtr->packets, activeStatPtr->packets);
                    omci_pm_debugStatOutput("ENET3 broadcastPackets ", localStatRec.broadcastPackets, baseStatPtr->broadcastPackets, activeStatPtr->broadcastPackets);
                    omci_pm_debugStatOutput("ENET3 multicastPackets ", localStatRec.multicastPackets, baseStatPtr->multicastPackets, activeStatPtr->multicastPackets);
                    omci_pm_debugStatOutput("ENET3 undersizePackets ", localStatRec.undersizePackets, baseStatPtr->undersizePackets, activeStatPtr->undersizePackets);
                    omci_pm_debugStatOutput("ENET3 fragments        ", localStatRec.fragments, baseStatPtr->fragments, activeStatPtr->fragments);
                    omci_pm_debugStatOutput("ENET3 jabbers          ", localStatRec.jabbers, baseStatPtr->jabbers, activeStatPtr->jabbers);
                    omci_pm_debugStatOutput("ENET3 packets64Octets  ", localStatRec.packets64Octets, baseStatPtr->packets64Octets, activeStatPtr->packets64Octets);
                    omci_pm_debugStatOutput("ENET3 packets127Octets ", localStatRec.packets127Octets, baseStatPtr->packets127Octets, activeStatPtr->packets127Octets);
                    omci_pm_debugStatOutput("ENET3 packets255Octets ", localStatRec.packets255Octets, baseStatPtr->packets255Octets, activeStatPtr->packets255Octets);
                    omci_pm_debugStatOutput("ENET3 packets511Octets ", localStatRec.packets511Octets, baseStatPtr->packets511Octets, activeStatPtr->packets511Octets);
                    omci_pm_debugStatOutput("ENET3 packets1023Octets", localStatRec.packets1023Octets, baseStatPtr->packets1023Octets, activeStatPtr->packets1023Octets);
                    omci_pm_debugStatOutput("ENET3 packets1518Octets", localStatRec.packets1518Octets, baseStatPtr->packets1518Octets, activeStatPtr->packets1518Octets);
                }

                // Test for ENET3 counters that cross over threshold.
                omci_th_check(omcipmEntry, activeStatPtr);
            }
            else
            {
                // Signal base stats initialization has occured.
                omcipmEntry->initBaseFlag = 1;
            }

            // Reset (or init) omcipmEntry Base stats rec.
            memcpy(baseStatPtr, &localStatRec, sizeof(BCM_OMCI_PM_ETHERNET_3_COUNTER));
        }
        else
        {
            // Signal bcm_omcipm_getCounters error.
            cmsLog_error("bcm_omcipm_getCounters error: %d", result);
        }
    }
}


static void pm_poll_enetdn(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER* activeStatPtr;
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER* baseStatPtr;
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER localStatRec;
    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;

    // Test for valid port number.
    if (omcipmEntry->omcipm.portID < ethLanNum || omcipmEntry->omcipm.portID == PORT_VEIP)
    {
        // Setup current stat buffer pointer.
        activeStatPtr = (BCM_OMCI_PM_ETHERNET_UPDN_COUNTER*)omcipmEntry->pm[intervalCounter & 1];

        // Read current ENETDN stats into local stats rec.
#ifndef OMCIPM_TEST
        result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENETDN, omcipmEntry->omcipm.portID, &localStatRec);
#endif    // OMCIPM_TEST

        // Test for bcm_omcipm_getCounters success.
        if (result == OMCI_PM_STATUS_SUCCESS)
        {
            // Setup base stats buffer pointer.
            baseStatPtr = (BCM_OMCI_PM_ETHERNET_UPDN_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];
#ifdef OMCIPM_TEST
localStatRec.dropEvents = baseStatPtr->dropEvents + getRandomNumber(0, 64);
localStatRec.octets = baseStatPtr->octets + getRandomNumber(65, 128);
localStatRec.packets = baseStatPtr->packets + getRandomNumber(129, 256);
localStatRec.broadcastPackets = baseStatPtr->broadcastPackets + getRandomNumber(257, 512);
localStatRec.multicastPackets = baseStatPtr->multicastPackets + getRandomNumber(129, 256);
localStatRec.crcErroredPackets = baseStatPtr->crcErroredPackets + getRandomNumber(65, 128);
localStatRec.undersizePackets = baseStatPtr->undersizePackets + getRandomNumber(0, 64);
localStatRec.oversizePackets = baseStatPtr->oversizePackets + getRandomNumber(65, 128);
localStatRec.packets64Octets = baseStatPtr->packets64Octets + getRandomNumber(129, 256);
localStatRec.packets127Octets = baseStatPtr->packets127Octets + getRandomNumber(257, 512);
localStatRec.packets255Octets = baseStatPtr->packets255Octets + getRandomNumber(129, 256);
localStatRec.packets511Octets = baseStatPtr->packets511Octets + getRandomNumber(65, 128);
localStatRec.packets1023Octets = baseStatPtr->packets1023Octets + getRandomNumber(0, 64);
localStatRec.packets1518Octets = baseStatPtr->packets1518Octets + getRandomNumber(65, 128);
#endif    // OMCIPM_TEST

            // Test if base stats have been initialized.
            if (omcipmEntry->initBaseFlag != 0)
            {
                // Inc ENET stats (ceiling on overflow) with delta between base value and current value.
                omci_pm_incStatWithCeiling(&activeStatPtr->dropEvents, omci_pm_calcStatDelta(baseStatPtr->dropEvents, localStatRec.dropEvents));
                omci_pm_incStatWithCeiling(&activeStatPtr->octets, omci_pm_calcStatDelta(baseStatPtr->octets, localStatRec.octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets, omci_pm_calcStatDelta(baseStatPtr->packets, localStatRec.packets));
                omci_pm_incStatWithCeiling(&activeStatPtr->broadcastPackets, omci_pm_calcStatDelta(baseStatPtr->broadcastPackets, localStatRec.broadcastPackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->multicastPackets, omci_pm_calcStatDelta(baseStatPtr->multicastPackets, localStatRec.multicastPackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->crcErroredPackets, omci_pm_calcStatDelta(baseStatPtr->crcErroredPackets, localStatRec.crcErroredPackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->undersizePackets, omci_pm_calcStatDelta(baseStatPtr->undersizePackets, localStatRec.undersizePackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->oversizePackets, omci_pm_calcStatDelta(baseStatPtr->oversizePackets, localStatRec.oversizePackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets64Octets, omci_pm_calcStatDelta(baseStatPtr->packets64Octets, localStatRec.packets64Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets127Octets, omci_pm_calcStatDelta(baseStatPtr->packets127Octets, localStatRec.packets127Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets255Octets, omci_pm_calcStatDelta(baseStatPtr->packets255Octets, localStatRec.packets255Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets511Octets, omci_pm_calcStatDelta(baseStatPtr->packets511Octets, localStatRec.packets511Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets1023Octets, omci_pm_calcStatDelta(baseStatPtr->packets1023Octets, localStatRec.packets1023Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets1518Octets, omci_pm_calcStatDelta(baseStatPtr->packets1518Octets, localStatRec.packets1518Octets));

                // Test for debug.
                if (omcipmEntry->debugFlag != FALSE)
                {
                    // Output latest ENETDN stats.
                    omci_pm_debugStatOutput("ENETDN dropEvents\n        ", localStatRec.dropEvents, baseStatPtr->dropEvents, activeStatPtr->dropEvents);
                    omci_pm_debugStatOutput("ENETDN octets\n            ", localStatRec.octets, baseStatPtr->octets, activeStatPtr->octets);
                    omci_pm_debugStatOutput("ENETDN packets\n           ", localStatRec.packets, baseStatPtr->packets, activeStatPtr->packets);
                    omci_pm_debugStatOutput("ENETDN broadcastPackets\n  ", localStatRec.broadcastPackets, baseStatPtr->broadcastPackets, activeStatPtr->broadcastPackets);
                    omci_pm_debugStatOutput("ENETDN multicastPackets\n  ", localStatRec.multicastPackets, baseStatPtr->multicastPackets, activeStatPtr->multicastPackets);
                    omci_pm_debugStatOutput("ENETDN crcErroredPackets\n ", localStatRec.crcErroredPackets, baseStatPtr->crcErroredPackets, activeStatPtr->crcErroredPackets);
                    omci_pm_debugStatOutput("ENETDN undersizePackets\n  ", localStatRec.undersizePackets, baseStatPtr->undersizePackets, activeStatPtr->undersizePackets);
                    omci_pm_debugStatOutput("ENETDN oversizePackets\n   ", localStatRec.oversizePackets, baseStatPtr->oversizePackets, activeStatPtr->oversizePackets);
                    omci_pm_debugStatOutput("ENETDN packets64Octets     ", localStatRec.packets64Octets, baseStatPtr->packets64Octets, activeStatPtr->packets64Octets);
                    omci_pm_debugStatOutput("ENETDN packets127Octets    ", localStatRec.packets127Octets, baseStatPtr->packets127Octets, activeStatPtr->packets127Octets);
                    omci_pm_debugStatOutput("ENETDN packets255Octets    ", localStatRec.packets255Octets, baseStatPtr->packets255Octets, activeStatPtr->packets255Octets);
                    omci_pm_debugStatOutput("ENETDN packets511Octets    ", localStatRec.packets511Octets, baseStatPtr->packets511Octets, activeStatPtr->packets511Octets);
                    omci_pm_debugStatOutput("ENETDN packets1023Octets   ", localStatRec.packets1023Octets, baseStatPtr->packets1023Octets, activeStatPtr->packets1023Octets);
                    omci_pm_debugStatOutput("ENETDN packets1518Octets   ", localStatRec.packets1518Octets, baseStatPtr->packets1518Octets, activeStatPtr->packets1518Octets);
                }

                // Test for ENETDN counters that cross over threshold.
                omci_th_check(omcipmEntry, activeStatPtr);
            }
            else
            {
                // Signal base stats initialization has occured.
                omcipmEntry->initBaseFlag = 1;
            }

            // Reset base stats rec.
            memcpy(baseStatPtr, &localStatRec, sizeof(BCM_OMCI_PM_ETHERNET_UPDN_COUNTER));
        }
        else
        {
            // Signal bcm_omcipm_getCounters error.
            cmsLog_error("bcm_omcipm_getCounters error: %d", result);
        }
    }
}


static void pm_poll_enetup(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER* activeStatPtr;
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER* baseStatPtr;
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER localStatRec;
    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;

    // Test for valid port number.
    if (omcipmEntry->omcipm.portID < ethLanNum || omcipmEntry->omcipm.portID == PORT_VEIP)
    {
        // Setup current stat buffer pointer.
        activeStatPtr = (BCM_OMCI_PM_ETHERNET_UPDN_COUNTER*)omcipmEntry->pm[intervalCounter & 1];

        // Read current ENET stats into local stats rec.
#ifndef OMCIPM_TEST
        result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENETUP, omcipmEntry->omcipm.portID, &localStatRec);
#endif    // OMCIPM_TEST

        // Test for bcm_omcipm_getCounters success.
        if (result == OMCI_PM_STATUS_SUCCESS)
        {
            // Setup base stats buffer pointer.
            baseStatPtr = (BCM_OMCI_PM_ETHERNET_UPDN_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];
#ifdef OMCIPM_TEST
localStatRec.dropEvents = baseStatPtr->dropEvents + getRandomNumber(0, 64);
localStatRec.octets = baseStatPtr->octets + getRandomNumber(65, 128);
localStatRec.packets = baseStatPtr->packets + getRandomNumber(129, 256);
localStatRec.broadcastPackets = baseStatPtr->broadcastPackets + getRandomNumber(257, 512);
localStatRec.multicastPackets = baseStatPtr->multicastPackets + getRandomNumber(129, 256);
localStatRec.crcErroredPackets = baseStatPtr->crcErroredPackets + getRandomNumber(65, 128);
localStatRec.undersizePackets = baseStatPtr->undersizePackets + getRandomNumber(0, 64);
localStatRec.oversizePackets = baseStatPtr->oversizePackets + getRandomNumber(65, 128);
localStatRec.packets64Octets = baseStatPtr->packets64Octets + getRandomNumber(129, 256);
localStatRec.packets127Octets = baseStatPtr->packets127Octets + getRandomNumber(257, 512);
localStatRec.packets255Octets = baseStatPtr->packets255Octets + getRandomNumber(129, 256);
localStatRec.packets511Octets = baseStatPtr->packets511Octets + getRandomNumber(65, 128);
localStatRec.packets1023Octets = baseStatPtr->packets1023Octets + getRandomNumber(0, 64);
localStatRec.packets1518Octets = baseStatPtr->packets1518Octets + getRandomNumber(65, 128);
#endif    // OMCIPM_TEST

            // Test if base stats have been initialized.
            if (omcipmEntry->initBaseFlag != 0)
            {
                // Inc ENETUP stats (ceiling on overflow) with delta between base value and current value.
                omci_pm_incStatWithCeiling(&activeStatPtr->dropEvents, omci_pm_calcStatDelta(baseStatPtr->dropEvents, localStatRec.dropEvents));
                omci_pm_incStatWithCeiling(&activeStatPtr->octets, omci_pm_calcStatDelta(baseStatPtr->octets, localStatRec.octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets, omci_pm_calcStatDelta(baseStatPtr->packets, localStatRec.packets));
                omci_pm_incStatWithCeiling(&activeStatPtr->broadcastPackets, omci_pm_calcStatDelta(baseStatPtr->broadcastPackets, localStatRec.broadcastPackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->multicastPackets, omci_pm_calcStatDelta(baseStatPtr->multicastPackets, localStatRec.multicastPackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->crcErroredPackets, omci_pm_calcStatDelta(baseStatPtr->crcErroredPackets, localStatRec.crcErroredPackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->undersizePackets, omci_pm_calcStatDelta(baseStatPtr->undersizePackets, localStatRec.undersizePackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->oversizePackets, omci_pm_calcStatDelta(baseStatPtr->oversizePackets, localStatRec.oversizePackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets64Octets, omci_pm_calcStatDelta(baseStatPtr->packets64Octets, localStatRec.packets64Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets127Octets, omci_pm_calcStatDelta(baseStatPtr->packets127Octets, localStatRec.packets127Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets255Octets, omci_pm_calcStatDelta(baseStatPtr->packets255Octets, localStatRec.packets255Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets511Octets, omci_pm_calcStatDelta(baseStatPtr->packets511Octets, localStatRec.packets511Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets1023Octets, omci_pm_calcStatDelta(baseStatPtr->packets1023Octets, localStatRec.packets1023Octets));
                omci_pm_incStatWithCeiling(&activeStatPtr->packets1518Octets, omci_pm_calcStatDelta(baseStatPtr->packets1518Octets, localStatRec.packets1518Octets));

                // Test for debug.
                if (omcipmEntry->debugFlag != FALSE)
                {
                    // Output latest ENETUP stats.
                    omci_pm_debugStatOutput("ENETUP dropEvents\n        ", localStatRec.dropEvents, baseStatPtr->dropEvents, activeStatPtr->dropEvents);
                    omci_pm_debugStatOutput("ENETUP octets\n            ", localStatRec.octets, baseStatPtr->octets, activeStatPtr->octets);
                    omci_pm_debugStatOutput("ENETUP packets\n           ", localStatRec.packets, baseStatPtr->packets, activeStatPtr->packets);
                    omci_pm_debugStatOutput("ENETUP broadcastPackets\n  ", localStatRec.broadcastPackets, baseStatPtr->broadcastPackets, activeStatPtr->broadcastPackets);
                    omci_pm_debugStatOutput("ENETUP multicastPackets\n  ", localStatRec.multicastPackets, baseStatPtr->multicastPackets, activeStatPtr->multicastPackets);
                    omci_pm_debugStatOutput("ENETUP crcErroredPackets\n ", localStatRec.crcErroredPackets, baseStatPtr->crcErroredPackets, activeStatPtr->crcErroredPackets);
                    omci_pm_debugStatOutput("ENETUP undersizePackets\n  ", localStatRec.undersizePackets, baseStatPtr->undersizePackets, activeStatPtr->undersizePackets);
                    omci_pm_debugStatOutput("ENETUP oversizePackets\n   ", localStatRec.oversizePackets, baseStatPtr->oversizePackets, activeStatPtr->oversizePackets);
                    omci_pm_debugStatOutput("ENETUP packets64Octets     ", localStatRec.packets64Octets, baseStatPtr->packets64Octets, activeStatPtr->packets64Octets);
                    omci_pm_debugStatOutput("ENETUP packets127Octets    ", localStatRec.packets127Octets, baseStatPtr->packets127Octets, activeStatPtr->packets127Octets);
                    omci_pm_debugStatOutput("ENETUP packets255Octets    ", localStatRec.packets255Octets, baseStatPtr->packets255Octets, activeStatPtr->packets255Octets);
                    omci_pm_debugStatOutput("ENETUP packets511Octets    ", localStatRec.packets511Octets, baseStatPtr->packets511Octets, activeStatPtr->packets511Octets);
                    omci_pm_debugStatOutput("ENETUP packets1023Octets   ", localStatRec.packets1023Octets, baseStatPtr->packets1023Octets, activeStatPtr->packets1023Octets);
                    omci_pm_debugStatOutput("ENETUP packets1518Octets   ", localStatRec.packets1518Octets, baseStatPtr->packets1518Octets, activeStatPtr->packets1518Octets);
                }

                // Test for ENETUP counters that cross over threshold.
                omci_th_check(omcipmEntry, activeStatPtr);
            }
            else
            {
                // Signal base stats initialization has occured.
                omcipmEntry->initBaseFlag = 1;
            }

            // Reset base stats rec.
            memcpy(baseStatPtr, &localStatRec, sizeof(BCM_OMCI_PM_ETHERNET_UPDN_COUNTER));
        }
        else
        {
            // Signal bcm_omcipm_getCounters error.
            cmsLog_error("bcm_omcipm_getCounters error: %d", result);
        }
    }
}


static void pm_poll_moca_enet(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_MOCA_ETHERNET_COUNTER* activeStatPtr;
    BCM_OMCI_PM_MOCA_ETHERNET_COUNTER* baseStatPtr;
    BCM_OMCI_PM_MOCA_ETHERNET_COUNTER localStatRec;
    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;
    UINT64 inStat64;
    UINT64 inBase64;
    UINT64 inDelta64;
    UINT64 outStat64;
    UINT64 outBase64;
    UINT64 outDelta64;

    // Setup current stat buffer pointer.
    activeStatPtr = (BCM_OMCI_PM_MOCA_ETHERNET_COUNTER*)omcipmEntry->pm[intervalCounter & 1];

    // Read current MOCA ENET stats into local stats rec.
    result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_MOCA_ENET, omcipmEntry->omcipm.portID, &localStatRec);

    // Test for bcm_omcipm_getCounters success.
    if (result == OMCI_PM_STATUS_SUCCESS)
    {
        // Setup base stats buffer pointer.
        baseStatPtr = (BCM_OMCI_PM_MOCA_ETHERNET_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];

        // Test for operational PM object.
        if (omcipmEntry->initBaseFlag != 0)
        {
            // Inc MOCA ENET stats (ceiling on overflow) with delta between base value and current value.
            omci_pm_incStatWithCeiling(&activeStatPtr->incomingUnicastPackets, omci_pm_calcStatDelta(baseStatPtr->incomingUnicastPackets, localStatRec.incomingUnicastPackets));
            omci_pm_incStatWithCeiling(&activeStatPtr->incomingDiscardedPackets, omci_pm_calcStatDelta(baseStatPtr->incomingDiscardedPackets, localStatRec.incomingDiscardedPackets));
            omci_pm_incStatWithCeiling(&activeStatPtr->incomingErroredPackets, omci_pm_calcStatDelta(baseStatPtr->incomingErroredPackets, localStatRec.incomingErroredPackets));
            omci_pm_incStatWithCeiling(&activeStatPtr->incomingUnknownPackets, omci_pm_calcStatDelta(baseStatPtr->incomingUnknownPackets, localStatRec.incomingUnknownPackets));
            omci_pm_incStatWithCeiling(&activeStatPtr->incomingMulticastPackets, omci_pm_calcStatDelta(baseStatPtr->incomingMulticastPackets, localStatRec.incomingMulticastPackets));
            omci_pm_incStatWithCeiling(&activeStatPtr->incomingBroadcastPackets, omci_pm_calcStatDelta(baseStatPtr->incomingBroadcastPackets, localStatRec.incomingBroadcastPackets));

            // Setup 64-bit values for current stat & base.
            inStat64 = ((UINT64)localStatRec.incomingOctets_hi << 32) | localStatRec.incomingOctets_low;
            inBase64 = ((UINT64)baseStatPtr->incomingOctets_hi << 32) | baseStatPtr->incomingOctets_low;
            inDelta64 = omci_pm_calcStatDelta_64(inBase64, inStat64);
            omci_pm_incStatWithCeiling_64(&activeStatPtr->incomingOctets_hi, &activeStatPtr->incomingOctets_low, NULL, inDelta64);

            omci_pm_incStatWithCeiling(&activeStatPtr->outgoingUnicastPackets, omci_pm_calcStatDelta(baseStatPtr->outgoingUnicastPackets, localStatRec.outgoingUnicastPackets));
            omci_pm_incStatWithCeiling(&activeStatPtr->outgoingDiscardedPackets, omci_pm_calcStatDelta(baseStatPtr->outgoingDiscardedPackets, localStatRec.outgoingDiscardedPackets));
            omci_pm_incStatWithCeiling(&activeStatPtr->outgoingErroredPackets, omci_pm_calcStatDelta(baseStatPtr->outgoingErroredPackets, localStatRec.outgoingErroredPackets));
            omci_pm_incStatWithCeiling(&activeStatPtr->outgoingUnknownPackets, omci_pm_calcStatDelta(baseStatPtr->outgoingUnknownPackets, localStatRec.outgoingUnknownPackets));
            omci_pm_incStatWithCeiling(&activeStatPtr->outgoingMulticastPackets, omci_pm_calcStatDelta(baseStatPtr->outgoingMulticastPackets, localStatRec.outgoingMulticastPackets));
            omci_pm_incStatWithCeiling(&activeStatPtr->outgoingBroadcastPackets, omci_pm_calcStatDelta(baseStatPtr->outgoingBroadcastPackets, localStatRec.outgoingBroadcastPackets));

            // Setup 64-bit values for current stat & base.
            outStat64 = ((UINT64)localStatRec.outgoingOctets_hi << 32) | localStatRec.outgoingOctets_low;
            outBase64 = ((UINT64)baseStatPtr->outgoingOctets_hi << 32) | baseStatPtr->outgoingOctets_low;
            outDelta64 = omci_pm_calcStatDelta_64(outBase64, outStat64);
            omci_pm_incStatWithCeiling_64(&activeStatPtr->outgoingOctets_hi, &activeStatPtr->outgoingOctets_low, NULL, outDelta64);

            // Test for debug.
            if (omcipmEntry->debugFlag != FALSE)
            {
                // Output latest FEC stats.
                omci_pm_debugStatOutput("MOCA_ENET incomingUnicastPackets\n  ", localStatRec.incomingUnicastPackets, baseStatPtr->incomingUnicastPackets, activeStatPtr->incomingUnicastPackets);
                omci_pm_debugStatOutput("MOCA_ENET incomingDiscardedPackets\n  ", localStatRec.incomingDiscardedPackets, baseStatPtr->incomingDiscardedPackets, activeStatPtr->incomingDiscardedPackets);
                omci_pm_debugStatOutput("MOCA_ENET incomingErroredPackets\n  ", localStatRec.incomingErroredPackets, baseStatPtr->incomingErroredPackets, activeStatPtr->incomingErroredPackets);
                omci_pm_debugStatOutput("MOCA_ENET incomingUnknownPackets\n  ", localStatRec.incomingUnknownPackets, baseStatPtr->incomingUnknownPackets, activeStatPtr->incomingUnknownPackets);
                omci_pm_debugStatOutput("MOCA_ENET incomingMulticastPackets\n  ", localStatRec.incomingMulticastPackets, baseStatPtr->incomingMulticastPackets, activeStatPtr->incomingMulticastPackets);
                omci_pm_debugStatOutput("MOCA_ENET incomingBroadcastPackets\n  ", localStatRec.incomingBroadcastPackets, baseStatPtr->incomingBroadcastPackets, activeStatPtr->incomingBroadcastPackets);
                omci_pm_debugStatOutput("MOCA_ENET incomingOctets_hi\n  ", localStatRec.incomingOctets_hi, baseStatPtr->incomingOctets_hi, activeStatPtr->incomingOctets_hi);
                omci_pm_debugStatOutput("MOCA_ENET incomingOctets_low\n  ", localStatRec.incomingOctets_low, baseStatPtr->incomingOctets_low, activeStatPtr->incomingOctets_low);
                omci_pm_debugStatOutput("MOCA_ENET outgoingUnicastPackets\n  ", localStatRec.outgoingUnicastPackets, baseStatPtr->outgoingUnicastPackets, activeStatPtr->outgoingUnicastPackets);
                omci_pm_debugStatOutput("MOCA_ENET outgoingDiscardedPackets\n  ", localStatRec.outgoingDiscardedPackets, baseStatPtr->outgoingDiscardedPackets, activeStatPtr->outgoingDiscardedPackets);
                omci_pm_debugStatOutput("MOCA_ENET outgoingErroredPackets\n  ", localStatRec.outgoingErroredPackets, baseStatPtr->outgoingErroredPackets, activeStatPtr->outgoingErroredPackets);
                omci_pm_debugStatOutput("MOCA_ENET outgoingUnknownPackets\n  ", localStatRec.outgoingUnknownPackets, baseStatPtr->outgoingUnknownPackets, activeStatPtr->outgoingUnknownPackets);
                omci_pm_debugStatOutput("MOCA_ENET outgoingMulticastPackets\n  ", localStatRec.outgoingMulticastPackets, baseStatPtr->outgoingMulticastPackets, activeStatPtr->outgoingMulticastPackets);
                omci_pm_debugStatOutput("MOCA_ENET outgoingBroadcastPackets\n  ", localStatRec.outgoingBroadcastPackets, baseStatPtr->outgoingBroadcastPackets, activeStatPtr->outgoingBroadcastPackets);
                omci_pm_debugStatOutput("MOCA_ENET outgoingOctets_hi\n  ", localStatRec.outgoingOctets_hi, baseStatPtr->outgoingOctets_hi, activeStatPtr->outgoingOctets_hi);
                omci_pm_debugStatOutput("MOCA_ENET outgoingOctets_low\n  ", localStatRec.outgoingOctets_low, baseStatPtr->outgoingOctets_low, activeStatPtr->outgoingOctets_low);

                omci_pm_debugStatOutput_64("MOCA_ENET incomingOctets       ", inStat64, inBase64, inDelta64);
                omci_pm_debugStatOutput_64("MOCA_ENET outgoingOctets       ", outStat64, outBase64, outDelta64);
            }

            // Test for MOCA ENET counters that cross over threshold.
            omci_th_check(omcipmEntry, activeStatPtr);
        }
        else
        {
            // Signal omcipmEntry stats initialization has occured.
            omcipmEntry->initBaseFlag = 1;
        }

        // Reset (or init) omcipmEntry Base stats rec.
        memcpy(baseStatPtr, &localStatRec, sizeof(BCM_OMCI_PM_MOCA_ETHERNET_COUNTER));
    }
    else
    {
        // Signal bcm_omcipm_getCounters error.
        cmsLog_error("bcm_omcipm_getCounters error: %d", result);
    }
}


static void pm_poll_moca_intf(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_MOCA_INTERFACE_COUNTER* activeStatPtr;
    BCM_OMCI_PM_MOCA_INTERFACE_COUNTER* baseStatPtr;
    BCM_OMCI_PM_MOCA_INTERFACE_COUNTER localStatRec;
    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;

    // Setup current stat buffer pointer.
    activeStatPtr = (BCM_OMCI_PM_MOCA_INTERFACE_COUNTER*)omcipmEntry->pm[intervalCounter & 1];

    // Read current MOCA INTF stats into local stats rec.
    result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_MOCA_INTF, omcipmEntry->omcipm.portID, &localStatRec);

    // Test for bcm_omcipm_getCounters success.
    if (result == OMCI_PM_STATUS_SUCCESS)
    {
        // Setup base stats buffer pointer.
        baseStatPtr = (BCM_OMCI_PM_MOCA_INTERFACE_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];

        // Test for operational PM object.
        if (omcipmEntry->initBaseFlag != 0)
        {
            // Inc MOCA INTF stats (ceiling on overflow) with delta between base value and current value.
            omci_pm_incStatWithCeiling(&activeStatPtr->erroredMissedRxPackets, omci_pm_calcStatDelta(baseStatPtr->erroredMissedRxPackets, localStatRec.erroredMissedRxPackets));
            omci_pm_incStatWithCeiling(&activeStatPtr->erroredRxPackets, omci_pm_calcStatDelta(baseStatPtr->erroredRxPackets, localStatRec.erroredRxPackets));

            // Test for debug.
            if (omcipmEntry->debugFlag != FALSE)
            {
                // Output latest MOCA_INTF stats.
                omci_pm_debugStatOutput("MOCA_INTF phyTxBroadcastRate\n  ", localStatRec.phyTxBroadcastRate, baseStatPtr->phyTxBroadcastRate, activeStatPtr->phyTxBroadcastRate);
                omci_pm_debugStatOutput("MOCA_INTF phyTxRate\n  ", localStatRec.phyTxRate, baseStatPtr->phyTxRate, activeStatPtr->phyTxRate);
                omci_pm_debugStatOutput("MOCA_INTF txPowerControlReduction\n  ", localStatRec.txPowerControlReduction, baseStatPtr->txPowerControlReduction, activeStatPtr->txPowerControlReduction);
                omci_pm_debugStatOutput("MOCA_INTF phyRxRate\n  ", localStatRec.phyRxRate, baseStatPtr->phyRxRate, activeStatPtr->phyRxRate);
                omci_pm_debugStatOutput("MOCA_INTF rxPowerLevel\n  ", localStatRec.rxPowerLevel, baseStatPtr->rxPowerLevel, activeStatPtr->rxPowerLevel);
                omci_pm_debugStatOutput("MOCA_INTF phyRxBroadcastRate\n  ", localStatRec.phyRxBroadcastRate, baseStatPtr->phyRxBroadcastRate, activeStatPtr->phyRxBroadcastRate);
                omci_pm_debugStatOutput("MOCA_INTF rxBroadcastPowerLevel\n  ", localStatRec.rxBroadcastPowerLevel, baseStatPtr->rxBroadcastPowerLevel, activeStatPtr->rxBroadcastPowerLevel);
                omci_pm_debugStatOutput("MOCA_INTF txPackets\n  ", localStatRec.txPackets, baseStatPtr->txPackets, activeStatPtr->txPackets);
                omci_pm_debugStatOutput("MOCA_INTF rxPackets\n  ", localStatRec.rxPackets, baseStatPtr->rxPackets, activeStatPtr->rxPackets);
                omci_pm_debugOutput_MAC("MOCA_INTF mac\n  ", activeStatPtr->mac, localStatRec.mac);
            }

            // Copy MOCA INTF current values.
            activeStatPtr->phyTxBroadcastRate = localStatRec.phyTxBroadcastRate;
            activeStatPtr->phyTxRate = localStatRec.phyTxRate;
            activeStatPtr->txPowerControlReduction = localStatRec.txPowerControlReduction;
            activeStatPtr->phyRxRate = localStatRec.phyRxRate;
            activeStatPtr->rxPowerLevel = localStatRec.rxPowerLevel;
            activeStatPtr->phyRxBroadcastRate = localStatRec.phyRxBroadcastRate;
            activeStatPtr->rxBroadcastPowerLevel = localStatRec.rxBroadcastPowerLevel;
            activeStatPtr->txPackets = localStatRec.txPackets;
            activeStatPtr->rxPackets = localStatRec.rxPackets;
            memcpy(&activeStatPtr->mac, localStatRec.mac, BCM_OMCI_MAC_STR_SIZE);

            // Test for MOCA INTF counters that cross over threshold.
            omci_th_check(omcipmEntry, activeStatPtr);
        }
        else
        {
            // Signal omcipmEntry stats initialization has occured.
            omcipmEntry->initBaseFlag = 1;
        }

        // Reset (or init) omcipmEntry Base stats rec.
        baseStatPtr->erroredMissedRxPackets = localStatRec.erroredMissedRxPackets;
        baseStatPtr->erroredRxPackets = localStatRec.erroredRxPackets;
    }
    else
    {
        // Signal bcm_omcipm_getCounters error.
        cmsLog_error("bcm_omcipm_getCounters error: %d", result);
    }
}


static void pm_poll_gal_enet(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_GAL_ETHERNET_COUNTER* activeStatPtr;
    BCM_OMCI_PM_GAL_ETHERNET_COUNTER* baseStatPtr;
    BCM_OMCI_PM_GAL_ETHERNET_COUNTER localStatRec;
    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;

    // Test for valid port number.
    if (omcipmEntry->omcipm.portID >= GPON_PORT_MAX)
    {
        // Attempt to find valid port.
        omcipmEntry->omcipm.portID = omci_pm_findGemPort(omcipmEntry->omcipm.objID);
    }
    else
    {
        // Setup current stat buffer pointer.
        activeStatPtr = (BCM_OMCI_PM_GAL_ETHERNET_COUNTER*)omcipmEntry->pm[intervalCounter & 1];

        // Read current GAL_ENET stats into local stats rec.
#ifndef OMCIPM_TEST
        result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_GAL_ENET, omcipmEntry->omcipm.portID, &localStatRec);
#endif    // OMCIPM_TEST

        // Test for bcm_omcipm_getCounters success.
        if (result == OMCI_PM_STATUS_SUCCESS)
        {
            // Setup pointer to base stats.
            baseStatPtr = (BCM_OMCI_PM_GAL_ETHERNET_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];
#ifdef OMCIPM_TEST
localStatRec.discardedFrames = baseStatPtr->discardedFrames + getRandomNumber(0, 64);
localStatRec.transmittedFrames = baseStatPtr->transmittedFrames + getRandomNumber(65, 128);
localStatRec.receivedFrames = baseStatPtr->receivedFrames + getRandomNumber(129, 256);
#endif    // OMCIPM_TEST

            // Test for operational PM object.
            if (omcipmEntry->initBaseFlag != 0)
            {
                // Inc GAL_ENET stats (ceiling on overflow) with delta between base value and current value.
                omci_pm_incStatWithCeiling(&activeStatPtr->discardedFrames, omci_pm_calcStatDelta(baseStatPtr->discardedFrames, localStatRec.discardedFrames));
                omci_pm_incStatWithCeiling(&activeStatPtr->transmittedFrames, omci_pm_calcStatDelta(baseStatPtr->transmittedFrames, localStatRec.transmittedFrames));
                omci_pm_incStatWithCeiling(&activeStatPtr->receivedFrames, omci_pm_calcStatDelta(baseStatPtr->receivedFrames, localStatRec.receivedFrames));

                // Test for debug.
                if (omcipmEntry->debugFlag != FALSE)
                {
                    // Output latest GAL_ENET stats.
                    omci_pm_debugStatOutput("GAL_ENET discardedFrames\n  ", localStatRec.discardedFrames, baseStatPtr->discardedFrames, activeStatPtr->discardedFrames);
                    omci_pm_debugStatOutput("GAL_ENET transmittedFrames\n  ", localStatRec.transmittedFrames, baseStatPtr->transmittedFrames, activeStatPtr->transmittedFrames);
                    omci_pm_debugStatOutput("GAL_ENET receivedFrames\n  ", localStatRec.receivedFrames, baseStatPtr->receivedFrames, activeStatPtr->receivedFrames);
                }

                // Test for GAL_ENET counters that cross over threshold.
                omci_th_check(omcipmEntry, activeStatPtr);
            }
            else
            {
                // Signal omcipmEntry stats initialization has occured.
                omcipmEntry->initBaseFlag = 1;
            }

            // Reset (or init) omcipmEntry Base stats rec.
            memcpy(baseStatPtr, &localStatRec, sizeof(BCM_OMCI_PM_GAL_ETHERNET_COUNTER));
        }
        else
        {
            // Signal bcm_omcipm_getCounters error.
            cmsLog_error("bcm_omcipm_getCounters error: %d", result);
        }
    }
}


#ifdef DMP_X_ITU_ORG_VOICE_1

static void pm_poll_rtp(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_RTP_COUNTER* activeStatPtr;
    BCM_OMCI_PM_RTP_COUNTER* baseStatPtr;
    BCM_OMCI_PM_RTP_COUNTER localStatRec;

    // Test for valid port number.
    if (omcipmEntry->omcipm.portID >= VOIP_PORT_MAX)
    {
        // Attempt to find valid port.
        omcipmEntry->omcipm.portID = omci_pm_findVoipPort(omcipmEntry->omcipm.objID);
    }

    // Test for valid port number.
    if (omcipmEntry->omcipm.portID < VOIP_PORT_MAX)
    {
        // Setup current stat buffer pointer.
        activeStatPtr = (BCM_OMCI_PM_RTP_COUNTER*)omcipmEntry->pm[intervalCounter & 1];

        // Read current RTP_PM stats into local stats rec & test for success.
        if (bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_RTP, omcipmEntry->omcipm.portID, &localStatRec) == OMCI_PM_STATUS_SUCCESS)
        {
            // Setup base stats buffer pointer.
            baseStatPtr = (BCM_OMCI_PM_RTP_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];

            // Test if base stats have been initialized.
            if (omcipmEntry->initBaseFlag != 0)
            {
                // Inc RTP_PM stats (ceiling on overflow) with delta between base value and current value.
                omci_pm_incStatWithCeiling(&activeStatPtr->rtpErrors, omci_pm_calcStatDelta(baseStatPtr->rtpErrors, localStatRec.rtpErrors));
                omci_pm_incStatWithCeiling(&activeStatPtr->maxJitter, omci_pm_calcStatDelta(baseStatPtr->maxJitter, localStatRec.maxJitter));
                omci_pm_incStatWithCeiling(&activeStatPtr->maxTimeBetweenRtcpPackets, omci_pm_calcStatDelta(baseStatPtr->maxTimeBetweenRtcpPackets, localStatRec.maxTimeBetweenRtcpPackets));
                omci_pm_incStatWithCeiling(&activeStatPtr->bufferUnderflows, omci_pm_calcStatDelta(baseStatPtr->bufferUnderflows, localStatRec.bufferUnderflows));
                omci_pm_incStatWithCeiling(&activeStatPtr->bufferOverflows, omci_pm_calcStatDelta(baseStatPtr->bufferOverflows, localStatRec.bufferOverflows));

                // Test for interval end.
                if (intervalFlag == TRUE)
                {
                    // Calc RTP packet loss at end of interval only.
                    activeStatPtr->packetLoss = 123456; /* CalcRtpPacketLoss(); */
                }
                else
                {
                    // Clear RTP packet loss until end of interval.
                    activeStatPtr->packetLoss = 0;
                }

                // Test for debug.
                if (omcipmEntry->debugFlag != FALSE)
                {
                    // Output latest RTP_PM stats.
                    omci_pm_debugStatOutput("RTP_PM rtpErrors\n  ", localStatRec.rtpErrors, baseStatPtr->rtpErrors, activeStatPtr->rtpErrors);
                    omci_pm_debugStatOutput("RTP_PM maxJitter\n  ", localStatRec.maxJitter, baseStatPtr->maxJitter, activeStatPtr->maxJitter);
                    omci_pm_debugStatOutput("RTP_PM maxTimeBetweenRtcpPackets\n  ", localStatRec.maxTimeBetweenRtcpPackets, baseStatPtr->maxTimeBetweenRtcpPackets, activeStatPtr->maxTimeBetweenRtcpPackets);
                    omci_pm_debugStatOutput("RTP_PM bufferUnderflows\n  ", localStatRec.bufferUnderflows, baseStatPtr->bufferUnderflows, activeStatPtr->bufferUnderflows);
                    omci_pm_debugStatOutput("RTP_PM bufferOverflows\n  ", localStatRec.bufferOverflows, baseStatPtr->bufferOverflows, activeStatPtr->bufferOverflows);
                }

                // Test for RTP_PM counters that cross over threshold.
                omci_th_check(omcipmEntry, activeStatPtr);
            }
            else
            {
                // Signal base stats initialization has occured.
                omcipmEntry->initBaseFlag = 1;
            }

            // Reset base stats rec.
            memcpy(baseStatPtr, &localStatRec, sizeof(BCM_OMCI_PM_RTP_COUNTER));
        }
        else
        {
            // Signal bcm_omcipm_getCounters error.
            cmsLog_error("bcm_omcipm_getCounters error");
        }
    }
}

#endif // DMP_X_ITU_ORG_VOICE_1


static void pm_poll_ipHost(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_IP_HOST_COUNTER* activeStatPtr;
    BCM_OMCI_PM_IP_HOST_COUNTER* baseStatPtr;
    BCM_OMCI_PM_IP_HOST_COUNTER localStatRec;
    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;

    // Setup current stat buffer pointer.
    activeStatPtr = (BCM_OMCI_PM_IP_HOST_COUNTER*)omcipmEntry->pm[intervalCounter & 1];

    // Read current FEC stats into local stats rec.
#ifndef OMCIPM_TEST
    result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_IPHOST, omcipmEntry->omcipm.portID, &localStatRec);
#endif    // OMCIPM_TEST

    // Test for bcm_omcipm_getCounters success.
    if (result == OMCI_PM_STATUS_SUCCESS)
    {
        // Setup pointer to base stats.
        baseStatPtr = (BCM_OMCI_PM_IP_HOST_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];
#ifdef OMCIPM_TEST
localStatRec.icmpErrors = baseStatPtr->icmpErrors + getRandomNumber(129, 256);
localStatRec.dnsErrors = baseStatPtr->dnsErrors + getRandomNumber(257, 512);
#endif    // OMCIPM_TEST

        // Test for operational PM object.
        if (omcipmEntry->initBaseFlag != 0)
        {
            // Inc ipHost stats (ceiling on overflow) with delta between base value and current value.
            omci_pm_incStatWithCeiling(&activeStatPtr->icmpErrors, omci_pm_calcStatDelta(baseStatPtr->icmpErrors, localStatRec.icmpErrors));
            omci_pm_incStatWithCeiling(&activeStatPtr->dnsErrors, omci_pm_calcStatDelta(baseStatPtr->dnsErrors, localStatRec.dnsErrors));

            // Test for debug.
            if (omcipmEntry->debugFlag != FALSE)
            {
                // Output latest FEC stats.
                omci_pm_debugStatOutput("ICMP errors         ", localStatRec.icmpErrors, baseStatPtr->icmpErrors, activeStatPtr->icmpErrors);
                omci_pm_debugStatOutput("DNS errors          ", localStatRec.dnsErrors, baseStatPtr->dnsErrors, activeStatPtr->dnsErrors);
            }

            // Test for ipHost counters that cross over threshold.
            omci_th_check(omcipmEntry, activeStatPtr);
        }
        else
        {
            // Signal omcipmEntry stats initialization has occured.
            omcipmEntry->initBaseFlag = 1;
        }

        // Reset (or init) omcipmEntry Base stats rec.
        memcpy(baseStatPtr, &localStatRec, sizeof(BCM_OMCI_PM_IP_HOST_COUNTER));
    }
    else
    {
        // Signal bcm_omcipm_getCounters error.
        cmsLog_error("bcm_omcipm_getCounters error: %d", result);
    }
}

static void pm_poll_extPm(BCM_OMCIPM_ENTRY* omcipmEntry, UINT8 intervalCounter)
{
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER* activeStatPtr;
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER* baseStatPtr;
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER localStatRec;
    BCM_OMCI_PM_MOCA_ETHERNET_COUNTER mocaLocal;
    BCM_OMCI_PM_GEM_PORT_COUNTER gemLocal;
    UINT8 interval = intervalCounter;

    BCM_OMCI_PM_STATUS result = OMCI_PM_STATUS_SUCCESS;
    UINT32 validPort = 1;

    // check continuous accumulation
    if (omcipmEntry->omcipm.controlFields & 0x1)
      interval = 0;

    // check global disable
    if (!(omcipmEntry->omcipm.accumulationDisable & 0x4000))
    {
        // Test for valid port number.
        switch(omcipmEntry->omcipm.tpType)
        {
            case MAC_BRIDGE_ETHERNET_PPTP_TYPE:
                if ((omcipmEntry->omcipm.portID >= ETHERNET_PORT_MAX) && 
                  (omcipmEntry->omcipm.portID != PORT_VEIP))
                    validPort = 0;
                break;
            case MAC_BRIDGE_1P_MAPPER_TYPE:
                if (omcipmEntry->omcipm.portID == 0)
                    validPort = 0;
                break;
            case MAC_BRIDGE_IWTP_TYPE:
            case MAC_BRIDGE_MCAST_IWTP_TYPE:
                if (omcipmEntry->omcipm.portID >= GPON_PORT_MAX)
                    validPort = 0;
                break;
            case MAC_BRIDGE_VEIP_TYPE:
            case MAC_BRIDGE_MOCA_TYPE:
                break;
            default:
                validPort = 0;
                break;
        }

        if (!validPort)
        {
            switch (omcipmEntry->omcipm.parentMeClass)
            {
                case 47: /* MAC bridge port configuration data */
                    omcipmEntry->omcipm.portID = omci_pm_findPhysicalPort(omcipmEntry->omcipm.parentMeInstance, &omcipmEntry->omcipm.tpType);
                    break;
                case 11: /* Physical path termination point Ethernet UNI */
                    omcipmEntry->omcipm.portID = omci_pm_findEnetPort(omcipmEntry->omcipm.parentMeInstance);
                    omcipmEntry->omcipm.tpType = MAC_BRIDGE_ETHERNET_PPTP_TYPE;
                    break;
                case 266: /* GEM interworking termination point */
                    omcipmEntry->omcipm.portID = omci_pm_findIwtpGemPort(omcipmEntry->omcipm.parentMeInstance);
                    omcipmEntry->omcipm.tpType = MAC_BRIDGE_IWTP_TYPE;
                    break;
                case 281: /* Multicast GEM interworking termination point */
                    omcipmEntry->omcipm.portID = omci_pm_findMcastIwtpGemPort(omcipmEntry->omcipm.parentMeInstance);
                    omcipmEntry->omcipm.tpType = MAC_BRIDGE_MCAST_IWTP_TYPE;
                    break;
                case 329: /* Virtual Ethernet interface point */
                    omcipmEntry->omcipm.portID = PORT_VEIP; // veip port
                    omcipmEntry->omcipm.tpType = MAC_BRIDGE_VEIP_TYPE;
                    break;
                case 162: /* Physical path termination point MoCA UNI */
                    omcipmEntry->omcipm.tpType = MAC_BRIDGE_MOCA_TYPE;
                    break;
            }
            validPort = 1;
            // Test for valid port number.
            switch(omcipmEntry->omcipm.tpType)
            {
                case MAC_BRIDGE_ETHERNET_PPTP_TYPE:
                    if ((omcipmEntry->omcipm.portID >= ETHERNET_PORT_MAX) && 
                      (omcipmEntry->omcipm.portID != PORT_VEIP))
                        validPort = 0;
                    break;
                case MAC_BRIDGE_1P_MAPPER_TYPE:
                    if (omcipmEntry->omcipm.portID == 0)
                        validPort = 0;
                    break;
                case MAC_BRIDGE_IWTP_TYPE:
                case MAC_BRIDGE_MCAST_IWTP_TYPE:
                    if (omcipmEntry->omcipm.portID >= GPON_PORT_MAX)
                        validPort = 0;
                    break;
                case MAC_BRIDGE_VEIP_TYPE:
                case MAC_BRIDGE_MOCA_TYPE:
                    break;
                default:
                    validPort = 0;
                    break;
            }
            if (!validPort)
            {
                cmsLog_error("poll extPM: not a valid port, instance=%d parentClass=%d parentInstance=%d\n",
                     omcipmEntry->omcipm.objID, omcipmEntry->omcipm.parentMeClass, omcipmEntry->omcipm.tpType);
                return;
            }
        }

        memset(&localStatRec, 0, sizeof(localStatRec));

        // Read current stats into local stats rec.
        switch(omcipmEntry->omcipm.tpType)
        {
            case MAC_BRIDGE_ETHERNET_PPTP_TYPE:
            case MAC_BRIDGE_VEIP_TYPE:
                if (omcipmEntry->omcipm.controlFields & 0x2) // downstream traffic
                    result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENETDN, omcipmEntry->omcipm.portID, &localStatRec);
                else
                    result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENETUP, omcipmEntry->omcipm.portID, &localStatRec);
                break;
            case MAC_BRIDGE_1P_MAPPER_TYPE:
                {
                    int i;
                    for (i = 0; i < GPON_PORT_MAX; i++)
                    {
                        if (omcipmEntry->omcipm.portID & (1 << i))
                        {
                            result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_GEM_PORT, i, &gemLocal);
                            if (result == OMCI_PM_STATUS_SUCCESS)
                            {
                                if (omcipmEntry->omcipm.controlFields & 0x2) // downstream traffic
                                {
                                    localStatRec.octets += gemLocal.receivedPayloadBytes;
                                    localStatRec.packets += gemLocal.receivedGEMFrames;
                                }
                                else
                                {
                                    localStatRec.octets += gemLocal.transmittedPayloadBytes;
                                    localStatRec.packets += gemLocal.transmittedGEMFrames;
                                }
                            }
                            else
                                break;
                        }
                    }
                }
                break;
            case MAC_BRIDGE_IWTP_TYPE:
            case MAC_BRIDGE_MCAST_IWTP_TYPE:
                result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_GEM_PORT, omcipmEntry->omcipm.portID, &gemLocal);
                if (result == OMCI_PM_STATUS_SUCCESS)
                {
                    if (omcipmEntry->omcipm.controlFields & 0x2) // downstream traffic
                    {
                        localStatRec.octets = gemLocal.receivedPayloadBytes;
                        localStatRec.packets = gemLocal.receivedGEMFrames;
                    }
                    else
                    {
                        localStatRec.octets = gemLocal.transmittedPayloadBytes;
                        localStatRec.packets = gemLocal.transmittedGEMFrames;
                    }
                }
                break;
            case MAC_BRIDGE_MOCA_TYPE:
                result = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_MOCA_ENET, omcipmEntry->omcipm.portID, &mocaLocal);
                if (result == OMCI_PM_STATUS_SUCCESS)
                {
                    if (omcipmEntry->omcipm.controlFields & 0x2) // downstream traffic
                    {
                        localStatRec.octets = mocaLocal.outgoingOctets_low;
                        localStatRec.broadcastPackets = mocaLocal.outgoingBroadcastPackets;
                        localStatRec.multicastPackets = mocaLocal.outgoingMulticastPackets;
                        localStatRec.packets = mocaLocal.outgoingUnicastPackets +
                                               mocaLocal.outgoingBroadcastPackets +
                                               mocaLocal.outgoingMulticastPackets;
                    }
                    else
                    {
                        localStatRec.octets = mocaLocal.incomingOctets_low;
                        localStatRec.broadcastPackets = mocaLocal.incomingBroadcastPackets;
                        localStatRec.multicastPackets = mocaLocal.incomingMulticastPackets;
                        localStatRec.packets = mocaLocal.incomingUnicastPackets +
                                               mocaLocal.incomingBroadcastPackets +
                                               mocaLocal.incomingMulticastPackets;
                    }
                }
                break;
            default:
                result = OMCI_PM_STATUS_NOT_SUPPORTED;

        }

        // Test for bcm_omcipm_getCounters success.
        if (result == OMCI_PM_STATUS_SUCCESS)
        {

            // Setup current stat buffer pointer.
            activeStatPtr = (BCM_OMCI_PM_ETHERNET_UPDN_COUNTER*)omcipmEntry->pm[interval & 1];
            // Setup base stats buffer pointer.
            baseStatPtr = (BCM_OMCI_PM_ETHERNET_UPDN_COUNTER*)omcipmEntry->pm[OMCIPM_COUNTER_TYPE_BASE];

            // Test if base stats have been initialized.
            if (omcipmEntry->initBaseFlag != 0)
            {
                // Inc ENET stats (ceiling on overflow) with delta between base value and current value.
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 0)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->dropEvents, omci_pm_calcStatDelta(baseStatPtr->dropEvents, localStatRec.dropEvents));
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 1)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->octets, omci_pm_calcStatDelta(baseStatPtr->octets, localStatRec.octets));
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 2)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->packets, omci_pm_calcStatDelta(baseStatPtr->packets, localStatRec.packets));
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 3)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->broadcastPackets, omci_pm_calcStatDelta(baseStatPtr->broadcastPackets, localStatRec.broadcastPackets));
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 4)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->multicastPackets, omci_pm_calcStatDelta(baseStatPtr->multicastPackets, localStatRec.multicastPackets));
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 5)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->crcErroredPackets, omci_pm_calcStatDelta(baseStatPtr->crcErroredPackets, localStatRec.crcErroredPackets));
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 6)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->undersizePackets, omci_pm_calcStatDelta(baseStatPtr->undersizePackets, localStatRec.undersizePackets));
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 7)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->oversizePackets, omci_pm_calcStatDelta(baseStatPtr->oversizePackets, localStatRec.oversizePackets));
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 8)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->packets64Octets, omci_pm_calcStatDelta(baseStatPtr->packets64Octets, localStatRec.packets64Octets));
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 9)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->packets127Octets, omci_pm_calcStatDelta(baseStatPtr->packets127Octets, localStatRec.packets127Octets));
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 10)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->packets255Octets, omci_pm_calcStatDelta(baseStatPtr->packets255Octets, localStatRec.packets255Octets));
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 11)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->packets511Octets, omci_pm_calcStatDelta(baseStatPtr->packets511Octets, localStatRec.packets511Octets));
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 12)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->packets1023Octets, omci_pm_calcStatDelta(baseStatPtr->packets1023Octets, localStatRec.packets1023Octets));
                if ( !(omcipmEntry->omcipm.accumulationDisable & (1 << 13)) )
                    omci_pm_incStatWithCeiling(&activeStatPtr->packets1518Octets, omci_pm_calcStatDelta(baseStatPtr->packets1518Octets, localStatRec.packets1518Octets));
                // Test for counters that cross over threshold. don't need send TCA if continous accumulation
                if ( !(omcipmEntry->omcipm.tcaDisable & 0x4000) && !(omcipmEntry->omcipm.controlFields & 0x1))
                    // Test for counters that cross over threshold.
                    omci_th_check(omcipmEntry, (const void*) activeStatPtr);
            }
            else
            {
                // Signal base stats initialization has occured.
                omcipmEntry->initBaseFlag = 1;
            }

            // Reset base stats rec.
            memcpy(baseStatPtr, &localStatRec, sizeof(BCM_OMCI_PM_ETHERNET_UPDN_COUNTER));
        }
        else
        {
            // Signal bcm_omcipm_getCounters error.
            cmsLog_error("bcm_omcipm_getCounters error: %d", result);
        }
    }

}


/*========================= PUBLIC FUNCTIONS ==========================*/


void omci_pm_poll(BCM_OMCIPM_ENTRY* omcipmEntry)
{
    // Poll current counters for specific OMCI PM entry.
    switch (omcipmEntry->omcipm.objType)
    {
        case MDMOID_GEM_PORT_PM_HISTORY_DATA:
            pm_poll_gem_port(omcipmEntry, intervalCounter);
            break;
        case MDMOID_FEC_PM_HISTORY_DATA:
            pm_poll_fec(omcipmEntry, intervalCounter);
            break;
        case MDMOID_ETHERNET_PM_HISTORY_DATA:
            pm_poll_enet(omcipmEntry, intervalCounter);
            break;
        case MDMOID_ETHERNET_PM_HISTORY_DATA2:
            pm_poll_enet2(omcipmEntry, intervalCounter);
            break;
        case MDMOID_ETHERNET_PM_HISTORY_DATA3:
            pm_poll_enet3(omcipmEntry, intervalCounter);
            break;
        case MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA:
            pm_poll_enetdn(omcipmEntry, intervalCounter);
            break;
        case MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA:
            pm_poll_enetup(omcipmEntry, intervalCounter);
            break;
        case MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA:
            pm_poll_moca_enet(omcipmEntry, intervalCounter);
            break;
        case MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA:
            pm_poll_moca_intf(omcipmEntry, intervalCounter);
            break;
        case MDMOID_GAL_ETHERNET_PM_HISTORY_DATA:
            pm_poll_gal_enet(omcipmEntry, intervalCounter);
            break;
        case MDMOID_RTP_PM_HISTORY_DATA:
#ifdef DMP_X_ITU_ORG_VOICE_1
            pm_poll_rtp(omcipmEntry, intervalCounter);
#endif  // #ifdef DMP_X_ITU_ORG_VOICE_1
            break;
        case MDMOID_IP_HOST_PM_HISTORY_DATA:
            pm_poll_ipHost(omcipmEntry, intervalCounter);
            break;
        case MDMOID_MAC_BRIDGE_PM_HISTORY_DATA:
            pm_poll_mac_bridge(omcipmEntry, intervalCounter);
            break;
        case MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA:
            pm_poll_mac_bridge_port(omcipmEntry, intervalCounter);
            break;
        case MDMOID_ETHERNET_FRAME_EXTENDED_P_M:
            pm_poll_extPm(omcipmEntry, intervalCounter);
            break;
        default:
            cmsLog_error("omci_pm_poll: Invalid objType: %d", omcipmEntry->omcipm.objType);
        break;
    }
}


void omci_pm_pollByObjectClass(BCM_OMCI_PM_CLASS_ID obj_Class)
{
    BCM_OMCIPM_ENTRY* omcipmEntry = NULL;
    BCM_COMMON_LLIST* pHead = omci_pm_getListPtr();

    if (pHead == NULL)
    {
        cmsLog_error("Invalid Head of PM link-list");
        return;
    }

    // Get omcipmEntry linked-list 1st item.
    omcipmEntry = BCM_COMMON_LL_GET_HEAD(*pHead);

    // Loop until every linked-list item examined.
    while (omcipmEntry != NULL)
    {
        // Poll current counters for specific object class.
        if (omcipmEntry->omcipm.objType == obj_Class)
        {
            omci_pm_poll(omcipmEntry);
        }

        // Get next linked-list item (if any).
        omcipmEntry = BCM_COMMON_LL_GET_NEXT(omcipmEntry);
    }
}


void omci_pm_increaseIntervalCounter(void)
{
    intervalCounter++;
}


void omci_pm_resetIntervalCounter(void)
{
    intervalCounter = 0;
}


UINT8 omci_pm_getIntervalCounter(void)
{
    return intervalCounter;
}


void omci_pm_setIntervalFlag(UBOOL8 intervalEndFlag)
{
    intervalFlag = intervalEndFlag;
}


UBOOL8 omci_pm_getIntervalFlag(void)
{
    return intervalFlag;
}


#endif    // DMP_X_ITU_ORG_GPON_1

