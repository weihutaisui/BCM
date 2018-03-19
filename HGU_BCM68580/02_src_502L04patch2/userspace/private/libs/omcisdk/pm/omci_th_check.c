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

#include "owsvc_api.h"

#include "omci_th_check.h"
#include "omci_th_llist.h"
#include "omci_arc_llist.h"
#include "omci_pm_alarm.h"
#include "omci_pm_poll.h"
#include "omci_pm_sync.h"
#include "omci_pm_util.h"


extern UINT32 enetPptpAdminState[];    // it is defined in omci_pm_sync.c

/*========================= PRIVATE FUNCTIONS ==========================*/

static void th_check_fec(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const BCM_OMCI_PM_FEC_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr = NULL;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;

    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test pData->correctedBytes threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->correctedBytes) == TRUE)
        {
            // Set 1st bit (which is correctedBytes).
            tcaAlarmBits |= 0x8000;
        }

        // Test pData->correctedCodeWords threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue2, pData->correctedCodeWords) == TRUE)
        {
            // Set 2nd bit (which is correctedCodeWords).
            tcaAlarmBits |= (0x8000 >> 1);
        }

        // Test pData->uncorrectedCodeWords threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue3, pData->uncorrectedCodeWords) == TRUE)
        {
            // Set 3rd bit (which is uncorrectedCodeWords).
            tcaAlarmBits |= (0x8000 >> 2);
        }

        // Test pData->fecSeconds threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue4, pData->fecSeconds) == TRUE)
        {
            // Set 5th bit (which is fecSeconds).  NOTE: 4th bit is reserved.
            tcaAlarmBits |= (0x8000 >> 4);
        }

        // Test for any new alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {

            // Send OLT an FEC TCA alarm message with only new bits & update entry's reportedAlarmBits field.
            omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
        }
    }
}

static void th_check_mac_bridge(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const BCM_OMCI_PM_MAC_BRIDGE_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr = NULL;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;

    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test pData->learningDiscaredEntries threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->learningDiscaredEntries) == TRUE)
        {
            // Set 1st bit (which is learningDiscaredEntries).
            tcaAlarmBits |= 0x8000;
        }

        // Test for any new alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {
            // Send OLT a MAC bridge TCA alarm message with only new bits & update entry's reportedAlarmBits field.
            omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
        }
    }
}

static void th_check_mac_bridge_port(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const BCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr = NULL;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;

    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test pData->delayDiscardedFrames threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->delayDiscardedFrames) == TRUE)
        {
            // Set 2st bit (which is delayDiscardedFrames). (1st is reserved)
            tcaAlarmBits |= (0x8000 >> 1);
        }

        // Test pData->mtuDiscardedFrames threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue2, pData->mtuDiscardedFrames) == TRUE)
        {
            // Set 3rd bit (which is mtuDiscardedFrames).
            tcaAlarmBits |= (0x8000 >> 2);
        }

        // Test pData->receivedDiscardedFrames threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue3, pData->receivedDiscardedFrames) == TRUE)
        {
            // Set 5th bit (which is receivedDiscardedFrames). (4th bit is reserved)
            tcaAlarmBits |= (0x8000 >> 4);
        }

        // Test for any new alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {
            // Test for valid PPTP Administrative State.  NOTE: portID has been verified in Poll_ENET.
            if (enetPptpAdminState[omcipmEntry->omcipm.portID] == ENET_ADMIN_STATE_UNLOCKED)
            {
                // Send OLT a MAC bridge port TCA alarm message with only new bits & update entry's reportedAlarmBits field.
                omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
            }
        }
    }
}


static void th_check_enet(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const BCM_OMCI_PM_ETHERNET_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr = NULL;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;

    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test pData->fcsErrors threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->fcsErrors) == TRUE)
        {
            // Set 1st alarm bit (which is fcsErrors).
            tcaAlarmBits |= 0x8000;
        }

        // Test pData->excessiveCollisionCounter threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue2, pData->excessiveCollisionCounter) == TRUE)
        {
            // Set 2nd alarm bit (which is excessiveCollisionCounter).
            tcaAlarmBits |= (0x8000 >> 1);
        }

        // Test pData->lateCollisionCounter threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue3, pData->lateCollisionCounter) == TRUE)
        {
            // Set 3rd alarm bit (which is impairedBlocks).
            tcaAlarmBits |= (0x8000 >> 2);
        }

        // Test pData->frameTooLongs threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue4, pData->frameTooLongs) == TRUE)
        {
            // Set 4th alarm bit (which is frameTooLongs).
            tcaAlarmBits |= (0x8000 >> 3);
        }

        // Test pData->bufferOverflowsOnReceive threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue5, pData->bufferOverflowsOnReceive) == TRUE)
        {
            // Set 5th alarm bit (which is bufferOverflowsOnReceive).
            tcaAlarmBits |= (0x8000 >> 4);
        }

        // Test pData->bufferOverflowsOnTransmit threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue6, pData->bufferOverflowsOnTransmit) == TRUE)
        {
            // Set 6th alarm bit (which is bufferOverflowsOnTransmit).
            tcaAlarmBits |= (0x8000 >> 5);
        }

        // Test pData->singleCollisionFrameCounter threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue7, pData->singleCollisionFrameCounter) == TRUE)
        {
            // Set 7th alarm bit (which is singleCollisionFrameCounter).
            tcaAlarmBits |= (0x8000 >> 6);
        }

        // Test pData->multipleCollisionsFrameCounter threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue8, pData->multipleCollisionsFrameCounter) == TRUE)
        {
            // Set 8th alarm bit (which is multipleCollisionsFrameCounter).
            tcaAlarmBits |= (0x8000 >> 7);
        }

        // Test pData->sqeCounter threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue9, pData->sqeCounter) == TRUE)
        {
            // Set 9th alarm bit (which is sqeCounter).
            tcaAlarmBits |= (0x8000 >> 8);
        }

        // Test pData->deferredTransmissionCounter threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue10, pData->deferredTransmissionCounter) == TRUE)
        {
            // Set 10th alarm bit (which is deferredTransmissionCounter).
            tcaAlarmBits |= (0x8000 >> 9);
        }

        // Test pData->internalMacTransmitErrorCounter threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue11, pData->internalMacTransmitErrorCounter) == TRUE)
        {
            // Set 11th alarm bit (which is internalMacTransmitErrorCounter).
            tcaAlarmBits |= (0x8000 >> 10);
        }

        // Test pData->carrierSenseErrorCounter threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue12, pData->carrierSenseErrorCounter) == TRUE)
        {
            // Set 12th alarm bit (which is carrierSenseErrorCounter).
            tcaAlarmBits |= (0x8000 >> 11);
        }

        // Test pData->alignmentErrorCounter threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue13, pData->alignmentErrorCounter) == TRUE)
        {
            // Set 13th alarm bit (which is alignmentErrorCounter).
            tcaAlarmBits |= (0x8000 >> 12);
        }

        // Test pData->internalMacReceiveErrorCounter threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue14, pData->internalMacReceiveErrorCounter) == TRUE)
        {
            // Set 14th alarm bit (which is internalMacReceiveErrorCounter).
            tcaAlarmBits |= (0x8000 >> 13);
        }

        // Test for any new alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {
            // Test for valid PPTP Administrative State.  NOTE: portID has been verified in Poll_ENET.
            if (enetPptpAdminState[omcipmEntry->omcipm.portID] == ENET_ADMIN_STATE_UNLOCKED)
            {
                // Send OLT an ENET TCA alarm message with only new bits & update entry's reportedAlarmBits field.
                omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
            }
        }
    }
}


static void th_check_enet2(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const BCM_OMCI_PM_ETHERNET_2_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr = NULL;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;

    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test pData->pppoeFilterFrameCounter threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->pppoeFilterFrameCounter) == TRUE)
        {
            // Set 1st alarm bit (which is pppoeFilterFrameCounter).
            tcaAlarmBits |= 0x8000;
        }

        // Test for any new ENET2 TCA alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {
            // Test for valid PPTP Administrative State.  NOTE: portID has been verified in Poll_ENET2.
            if (enetPptpAdminState[omcipmEntry->omcipm.portID] == ENET_ADMIN_STATE_UNLOCKED)
            {
                // Send OLT an ENET2 TCA alarm message (if appropriate) with only new bits & update entry's reportedAlarmBits field.
                omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
            }
        }
    }
}


static void th_check_enet3(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const BCM_OMCI_PM_ETHERNET_3_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr = NULL;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;

    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test pData->dropEvents threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->dropEvents) == TRUE)
        {
            // Set 1st alarm bit (which is dropEvents).
            tcaAlarmBits |= 0x8000;
        }

        // Test pData->undersizePackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue2, pData->undersizePackets) == TRUE)
        {
            // Set 2nd alarm bit (which is undersizePackets).
            tcaAlarmBits |= (0x8000 >> 1);
        }

        // Test pData->fragments threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue3, pData->fragments) == TRUE)
        {
            // Set 3rd alarm bit (which is fragments).
            tcaAlarmBits |= (0x8000 >> 2);
        }

        // Test pData->jabbers threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue4, pData->jabbers) == TRUE)
        {
            // Set 4th alarm bit (which is jabbers).
            tcaAlarmBits |= (0x8000 >> 3);
        }

        // Test for any new ENET3 TCA alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {
            // Test for valid PPTP Administrative State.  NOTE: portID has been verified in Poll_ENET3.
            if (enetPptpAdminState[omcipmEntry->omcipm.portID] == ENET_ADMIN_STATE_UNLOCKED)
            {
                // Send OLT an ENET3 TCA alarm message (if appropriate) with only new bits & update entry's reportedAlarmBits field.
                omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
            }
        }
    }
}


static void th_check_enetdn(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const BCM_OMCI_PM_ETHERNET_UPDN_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr = NULL;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;

    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test pData->dropEvents threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->dropEvents) == TRUE)
        {
            // Set 1st bit (which is dropEvents).
            tcaAlarmBits |= 0x8000;
        }

        // Test pData->crcErroredPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue2, pData->crcErroredPackets) == TRUE)
        {
            // Set 2nd bit (which is crcErroredPackets).
            tcaAlarmBits |= (0x8000 >> 1);
        }

        // Test pData->undersizePackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue3, pData->undersizePackets) == TRUE)
        {
            // Set 3rd bit (which is undersizePackets).
            tcaAlarmBits |= (0x8000 >> 2);
        }

        // Test pData->oversizePackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue4, pData->oversizePackets) == TRUE)
        {
            // Set 4th bit (which is oversizePackets).
            tcaAlarmBits |= (0x8000 >> 3);
        }

        // Test for any new alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {
            // Test for valid PPTP Administrative State.  NOTE: portID has been verified in Poll_ENETDN.
            if (enetPptpAdminState[omcipmEntry->omcipm.portID] == ENET_ADMIN_STATE_UNLOCKED)
            {
                // Send OLT an ENETDN TCA alarm message with only new bits & update entry's reportedAlarmBits field.
                omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
            }
        }
    }
}


static void th_check_enetup(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const BCM_OMCI_PM_ETHERNET_UPDN_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr = NULL;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;

    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test pData->dropEvents threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->dropEvents) == TRUE)
        {
            // Set 1st bit (which is dropEvents).
            tcaAlarmBits |= 0x8000;
        }

        // Test pData->crcErroredPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue2, pData->crcErroredPackets) == TRUE)
        {
            // Set 2nd bit (which is crcErroredPackets).
            tcaAlarmBits |= (0x8000 >> 1);
        }

        // Test pData->undersizePackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue3, pData->undersizePackets) == TRUE)
        {
            // Set 3rd bit (which is undersizePackets).
            tcaAlarmBits |= (0x8000 >> 2);
        }

        // Test pData->oversizePackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue4, pData->oversizePackets) == TRUE)
        {
            // Set 4th bit (which is oversizePackets).
            tcaAlarmBits |= (0x8000 >> 3);
        }

        // Test for any new alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {
            // Test for valid PPTP Administrative State.  NOTE: portID has been verified in Poll_ENETUP.
            if (enetPptpAdminState[omcipmEntry->omcipm.portID] == ENET_ADMIN_STATE_UNLOCKED)
            {
                // Send OLT an ENETUP TCA alarm message with only new bits & update entry's reportedAlarmBits field.
                omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
            }
        }
    }
}


static void th_check_moca_enet(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const BCM_OMCI_PM_MOCA_ETHERNET_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr = NULL;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;
    UINT64 data64 = 0;
    UINT64 threshold64 = 0;

    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test pData->incomingUnicastPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->incomingUnicastPackets) == TRUE)
        {
            // Set 1st bit (which is incomingUnicastPackets).
            tcaAlarmBits |= 0x8000;
        }

        // Test pData->incomingDiscardedPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue2, pData->incomingDiscardedPackets) == TRUE)
        {
            // Set 2nd bit (which is incomingDiscardedPackets).
            tcaAlarmBits |= (0x8000 >> 1);
        }

        // Test pData->incomingErroredPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue3, pData->incomingErroredPackets) == TRUE)
        {
            // Set 3rd bit (which is incomingErroredPackets).
            tcaAlarmBits |= (0x8000 >> 2);
        }

        // Test pData->incomingUnknownPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue4, pData->incomingUnknownPackets) == TRUE)
        {
            // Set 4th bit (which is incomingUnknownPackets).
            tcaAlarmBits |= (0x8000 >> 3);
        }

        // Test pData->incomingMulticastPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue5, pData->incomingMulticastPackets) == TRUE)
        {
            // Set 5th bit (which is incomingMulticastPackets).
            tcaAlarmBits |= (0x8000 >> 4);
        }

        // Test pData->incomingBroadcastPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue6, pData->incomingBroadcastPackets) == TRUE)
        {
            // Set 6th bit (which is incomingBroadcastPackets).
            tcaAlarmBits |= (0x8000 >> 5);
        }

        // Setup 64-bit values for stat & threshold incomingOctets.
        omci_pm_form64BitValue(&data64, pData->incomingOctets_hi, pData->incomingOctets_low, 8);
        threshold64 = (UINT64)thresholdPtr->thresholdValue7;

        // Test pData->incomingOctets threshold.
        if (omci_pm_testThreshholdValue_64(threshold64, data64) == TRUE)
        {
            // Set 7th bit (which is incomingOctets).
            tcaAlarmBits |= (0x8000 >> 6);
        }

        // Test pData->outgoingUnicastPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue8, pData->outgoingUnicastPackets) == TRUE)
        {
            // Set 8th bit (which is outgoingUnicastPackets).
            tcaAlarmBits |= (0x8000 >> 7);
        }

        // Test pData->outgoingDiscardedPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue9, pData->outgoingDiscardedPackets) == TRUE)
        {
            // Set 9th bit (which is outgoingDiscardedPackets).
            tcaAlarmBits |= (0x8000 >> 8);
        }

        // Test pData->outgoingErroredPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue10, pData->outgoingErroredPackets) == TRUE)
        {
            // Set 10th bit (which is outgoingErroredPackets).
            tcaAlarmBits |= (0x8000 >> 9);
        }

        // Test pData->outgoingUnknownPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue11, pData->outgoingUnknownPackets) == TRUE)
        {
            // Set 11th bit (which is outgoingUnknownPackets).
            tcaAlarmBits |= (0x8000 >> 10);
        }

        // Test pData->outgoingMulticastPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue12, pData->outgoingMulticastPackets) == TRUE)
        {
            // Set 12th bit (which is outgoingMulticastPackets).
            tcaAlarmBits |= (0x8000 >> 11);
        }

        // Test pData->outgoingBroadcastPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue13, pData->outgoingBroadcastPackets) == TRUE)
        {
            // Set 13th bit (which is outgoingBroadcastPackets).
            tcaAlarmBits |= (0x8000 >> 12);
        }

        // Setup 64-bit values for stat & threshold outgoingOctets.
        omci_pm_form64BitValue(&data64, pData->outgoingOctets_hi, pData->outgoingOctets_low, 8);
        threshold64 = (UINT64)thresholdPtr->thresholdValue14;

        // Test pData->outgoingOctets threshold outgoingOctets.
        if (omci_pm_testThreshholdValue_64(threshold64, data64) == TRUE)
        {
            // Set 14th bit (which is outgoingOctets).
            tcaAlarmBits |= (0x8000 >> 13);
        }

        // Test for any new alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {
            // Send OLT an MOCA_ENET TCA alarm message with only new bits & update entry's reportedAlarmBits field.
            omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
        }
    }
}


static void th_check_moca_intf(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const BCM_OMCI_PM_MOCA_INTERFACE_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr = NULL;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;

    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test pData->erroredMissedRxPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->erroredMissedRxPackets) == TRUE)
        {
            // Set 1st bit (which is erroredMissedRxPackets).
            tcaAlarmBits |= 0x8000;
        }

        // Test pData->erroredRxPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue2, pData->erroredRxPackets) == TRUE)
        {
            // Set 2nd bit (which is erroredRxPackets).
            tcaAlarmBits |= (0x8000 >> 1);
        }

        // Test for any new alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {
            // Send OLT a MOCA_INTF TCA alarm message with only new bits & update entry's reportedAlarmBits field.
            omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
        }
    }
}


static void th_check_gal_enet(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const BCM_OMCI_PM_GAL_ETHERNET_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr = NULL;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;

    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test pData->discardedFrames threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->discardedFrames) == TRUE)
        {
            // Set 1st bit (which is discardedFrames).
            tcaAlarmBits |= 0x8000;
        }

        // Test for any new alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {
            // Send OLT a GAL_ENET TCA alarm message with only new bits & update entry's reportedAlarmBits field.
            omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
        }
    }
}


static void th_check_ipHost(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const BCM_OMCI_PM_IP_HOST_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr = NULL;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;

    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test pData->icmpErrors threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->icmpErrors) == TRUE)
        {
            // Set 1st bit (which is icmpErrors).
            tcaAlarmBits |= 0x8000;
        }

        // Test pData->dnsErrors threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue2, pData->dnsErrors) == TRUE)
        {
            // Set 2nd bit (which is dnsErrors).
            tcaAlarmBits |= (0x8000 >> 1);
        }

        // Test for any new alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {
            // Send OLT an ipHost TCA alarm message with only new bits & update entry's reportedAlarmBits field.
            omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
        }
    }
}


#ifdef DMP_X_ITU_ORG_VOICE_1

static void th_check_rtp(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const BCM_OMCI_PM_RTP_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr = NULL;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;

    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test pData->rtpErrors threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->rtpErrors) == TRUE)
        {
            // Set 1st alarm bit (which is rtpErrors).
            tcaAlarmBits |= 0x8000;
        }

        // Test for interval end (time to test packet loss).
        if (omci_pm_getIntervalFlag() == TRUE)
        {
            // Test pData->maxJitter threshold.
            if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue2, pData->packetLoss) == TRUE)
            {
                // Set 2nd alarm bit (which is packetLoss).
                tcaAlarmBits |= (0x8000 >> 1);
            }
        }

        // Test pData->maxJitter threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue3, pData->maxJitter) == TRUE)
        {
            // Set 3rd alarm bit (which is maxJitter).
            tcaAlarmBits |= (0x8000 >> 2);
        }

        // Test pData->maxTimeBetweenRtcpPackets threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue4, pData->maxTimeBetweenRtcpPackets) == TRUE)
        {
            // Set 4th alarm bit (which is maxTimeBetweenRtcpPackets).
            tcaAlarmBits |= (0x8000 >> 3);
        }

        // Test pData->bufferUnderflows threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue5, pData->bufferUnderflows) == TRUE)
        {
            // Set 5th alarm bit (which is bufferUnderflows).
            tcaAlarmBits |= (0x8000 >> 4);
        }

        // Test pData->bufferOverflows threshold.
        if (omci_pm_testThreshholdValue(thresholdPtr->thresholdValue6, pData->bufferOverflows) == TRUE)
        {
            // Set 6th alarm bit (which is bufferOverflows).
            tcaAlarmBits |= (0x8000 >> 5);
        }

        // Test for any new alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {
            if(omci_pm_isAdminStateEnabled(MDMOID_PPTP_POTS_UNI, omcipmEntry->omcipm.portID))
            {
                // Send OLT an RTP_PM TCA alarm message with only new bits & update entry's reportedAlarmBits field.
                omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
            }
        }
    }
}

#endif  // #ifdef DMP_X_ITU_ORG_VOICE_1

static void th_check_extPm(BCM_OMCIPM_ENTRY* omcipmEntry, BCM_OMCI_PM_ETHERNET_UPDN_COUNTER* pData)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdPtr;
    UINT16 tcaAlarmBits = omcipmEntry->reportedAlarmBits;


    // Get threshold record from omcipmEntry ID.
    thresholdPtr = omci_threshold_get(omcipmEntry->omcipm.thresholdID);

    // Test for valid stats pointer.
    if (thresholdPtr != NULL)
    {
        // Test activeStatPtr->dropEvents threshold.
        if (!(omcipmEntry->omcipm.tcaDisable & (1 << 0)) &&
            omci_pm_testThreshholdValue(thresholdPtr->thresholdValue1, pData->dropEvents) == TRUE)
        {
            // Set 1st bit (which is dropEvents).
            tcaAlarmBits |= 0x8000;
        }

        // Test activeStatPtr->crcErroredPackets threshold.
        if (!(omcipmEntry->omcipm.tcaDisable & (1 << 1)) &&
            omci_pm_testThreshholdValue(thresholdPtr->thresholdValue2, pData->crcErroredPackets) == TRUE)
        {
            // Set 2nd bit (which is crcErroredPackets).
            tcaAlarmBits |= (0x8000 >> 1);
        }

        // Test activeStatPtr->undersizePackets threshold.
        if (!(omcipmEntry->omcipm.tcaDisable & (1 << 2)) &&
            omci_pm_testThreshholdValue(thresholdPtr->thresholdValue3, pData->undersizePackets) == TRUE)
        {
            // Set 3rd bit (which is undersizePackets).
            tcaAlarmBits |= (0x8000 >> 2);
        }

        // Test activeStatPtr->oversizePackets threshold.
        if (!(omcipmEntry->omcipm.tcaDisable & (1 << 3)) &&
            omci_pm_testThreshholdValue(thresholdPtr->thresholdValue4, pData->oversizePackets) == TRUE)
        {
            // Set 4th bit (which is oversizePackets).
            tcaAlarmBits |= (0x8000 >> 3);
        }

        // Test for any new alarm bits.
        if (omcipmEntry->reportedAlarmBits != tcaAlarmBits)
        {
            // Send OLT an ENETDN TCA alarm message with only new bits & update entry's reportedAlarmBits field.
            omci_th_send_alarm(omcipmEntry, tcaAlarmBits, TRUE);
        }
    }
}


/*========================= PUBLIC FUNCTIONS ==========================*/


void omci_th_send_alarm(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    UINT16 tcaAlarmBits,
    UINT8 alarmSetFlag)
{
    UINT16 alarm_Mask = 0;

    // Test for alarm announce message.
    if (alarmSetFlag == TRUE)
    {
        // Send only new alarms to notify OMCID of new event when it occurs.
        alarm_Mask = (tcaAlarmBits & ~omcipmEntry->reportedAlarmBits);

        // Update reported alarm bits.
        omcipmEntry->reportedAlarmBits |= tcaAlarmBits;
    }
    else
    {
        // Send all reported alarms to clear at the end of each 15-minute interval.
        alarm_Mask = omcipmEntry->reportedAlarmBits;

        // Clear reported alarms for next 15-minute interval.
        omcipmEntry->reportedAlarmBits = 0;
    }

    // Send TCA as alarm message.
    omci_alarm_send(0, omcipmEntry->omcipm.objType, omcipmEntry->omcipm.objID, alarm_Mask);
}


void omci_th_check(
    BCM_OMCIPM_ENTRY* omcipmEntry,
    const void* pData)
{
    // Check alarm for specific OMCI PM entry.
    switch (omcipmEntry->omcipm.objType)
    {
        case MDMOID_FEC_PM_HISTORY_DATA:
            th_check_fec(omcipmEntry, (BCM_OMCI_PM_FEC_COUNTER*)pData);
            break;
        case MDMOID_ETHERNET_PM_HISTORY_DATA:
            th_check_enet(omcipmEntry, (BCM_OMCI_PM_ETHERNET_COUNTER*)pData);
            break;
        case MDMOID_ETHERNET_PM_HISTORY_DATA2:
            th_check_enet2(omcipmEntry, (BCM_OMCI_PM_ETHERNET_2_COUNTER*)pData);
            break;
        case MDMOID_ETHERNET_PM_HISTORY_DATA3:
            th_check_enet3(omcipmEntry, (BCM_OMCI_PM_ETHERNET_3_COUNTER*)pData);
            break;
        case MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA:
            th_check_enetdn(omcipmEntry, (BCM_OMCI_PM_ETHERNET_UPDN_COUNTER*)pData);
            break;
        case MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA:
            th_check_enetup(omcipmEntry, (BCM_OMCI_PM_ETHERNET_UPDN_COUNTER*)pData);
            break;
        case MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA:
            th_check_moca_enet(omcipmEntry, (BCM_OMCI_PM_MOCA_ETHERNET_COUNTER*)pData);
            break;
        case MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA:
            th_check_moca_intf(omcipmEntry, (BCM_OMCI_PM_MOCA_INTERFACE_COUNTER*)pData);
            break;
        case MDMOID_GAL_ETHERNET_PM_HISTORY_DATA:
            th_check_gal_enet(omcipmEntry, (BCM_OMCI_PM_GAL_ETHERNET_COUNTER*)pData);
            break;
        case MDMOID_RTP_PM_HISTORY_DATA:
#ifdef DMP_X_ITU_ORG_VOICE_1
            th_check_rtp(omcipmEntry, (BCM_OMCI_PM_RTP_COUNTER*)pData);
#endif  // #ifdef DMP_X_ITU_ORG_VOICE_1
            break;
        case MDMOID_IP_HOST_PM_HISTORY_DATA:
            th_check_ipHost(omcipmEntry, (BCM_OMCI_PM_IP_HOST_COUNTER*)pData);
            break;
        case MDMOID_MAC_BRIDGE_PM_HISTORY_DATA:
            th_check_mac_bridge(omcipmEntry, (BCM_OMCI_PM_MAC_BRIDGE_COUNTER*)pData);
            break;
        case MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA:
            th_check_mac_bridge_port(omcipmEntry, (BCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER*)pData);
            break;
       case MDMOID_ETHERNET_FRAME_EXTENDED_P_M:
            th_check_extPm(omcipmEntry, (BCM_OMCI_PM_ETHERNET_UPDN_COUNTER*)pData);
            break;
        default:
            cmsLog_error("th_check: Invalid objType: %d", omcipmEntry->omcipm.objType);
        break;
    }
}

#endif    // DMP_X_ITU_ORG_GPON_1
