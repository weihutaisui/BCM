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

#include <net/if.h>
#include <sys/ioctl.h>

#include "owsvc_api.h"

#include "omci_pm.h"
#include "omci_pm_sync.h"
#include "omci_pm_util.h"

//#define OMCI_PM_TEST_MCAST

#define MULTICAST_ENET_ID 0
#define MULTICAST_MOCA_ID 1

/*
 * ONU's best effort estimate of "current multicast bandwidth" attribute
 * in "Multicast subscriber monitor" ME.
 * This implementation reads octets. The alternative approach is to read
 * multicastPackets and multiply with the estimated packet size.
 */ 
static UINT32 baseStatArray_ETH[ETHERNET_PORT_MAX] = {0};
static UINT32 multicastStatArray_ETH[ETHERNET_PORT_MAX] = {0};


/*========================= PRIVATE FUNCTIONS ==========================*/

static void pm_mcast_enet_read(void)
{
    int returnVal = 0;
    UINT16 portIndex = 0;
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER localStatArray;

    // Loop thru ETHERNET ports.
    for (portIndex = 0; portIndex < ethLanNum; portIndex++)
    {
        // Test portIndex is associated with valid PptpEthernetUni.
        if (omci_pm_getEnetPptpObjIDs(portIndex) != INVALID_OBJ_ID)
        {
            // Get current ETHERNET port counters.
            returnVal = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENETDN,
                                               portIndex,
                                              (void *)&localStatArray);

            // Test for success.
            if (returnVal == OMCI_PM_STATUS_SUCCESS)
            {
                // Calc one-second delta and copy to output rec, accounting for register wrap.
                multicastStatArray_ETH[portIndex] = omci_pm_calcStatDelta(
                  baseStatArray_ETH[portIndex],
                  localStatArray.octets) / (WAIT_MULTICAST_TIME / MSECS_PER_SEC);

#ifdef OMCI_PM_TEST_MCAST
                // Output multicast stats.
                printf("Multicast txOctets   Raw: %10u  Base: %10u  Delta: %10u\n",
                  localStatArray.octets, baseStatArray_ETH[portIndex],
                  multicastStatArray_ETH[portIndex]);
#endif
                // Set base value for next call.
                baseStatArray_ETH[portIndex] = localStatArray.octets;
            }
            else
            {
                // Log error.
                cmsLog_error("bcm_omcipm_getCounters failed, returnVal = %d", returnVal);
            }
        }
    }
}

#ifdef SUPPORT_MOCA

static UINT32 baseStatArray_MoCA[MOCA_PORT_MAX] = {0};
static UINT32 multicastStatArray_MoCA[MOCA_PORT_MAX] = {0};
static UINT32 errArray[MOCA_PORT_MAX] = {0};

static void pm_mcast_moca_read(void)
{
    int returnVal = 0;
    UINT16 portIndex = 0;
    BCM_OMCI_PM_MOCA_ETHERNET_COUNTER localStatRec;

    // Loop thru MOCA ports.
    for (portIndex = 0; portIndex < MOCA_PORT_MAX; portIndex++)
    {
        // Get current MOCA port counters.
        returnVal = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_MOCA_ENET, portIndex,
          (void *)&localStatRec);

        // Test for success.
        if (returnVal == OMCI_PM_STATUS_SUCCESS)
        {
            // Calc one-second delta and copy to output rec accounting for register wrap.
            multicastStatArray_MoCA[portIndex] = omci_pm_calcStatDelta(
              baseStatArray_MoCA[portIndex], localStatRec.incomingOctets_low);

            // Set base value for next call.
            baseStatArray_MoCA[portIndex] = localStatRec.incomingOctets_low;
            errArray[portIndex] = 0;
        }
        else
        {
            // avoid flooding msgs by printing the error msg only if it hasn't been reported
            if (errArray[portIndex] == 0)
            {
                // Log error.
                cmsLog_error("bcm_omcipm_getCounters failed, returnVal = %d", returnVal);
                errArray[portIndex] = 1;
            }
        }
    }
}

#endif // SUPPORT_MOCA


/*========================= PUBLIC FUNCTIONS ==========================*/


void omci_pm_mcast_poll(void)
{
    // Read ETHERNET stats.
    pm_mcast_enet_read();

#ifdef SUPPORT_MOCA
    // Read MOCA stats.
    pm_mcast_moca_read();
#endif // SUPPORT_MOCA
}

CmsRet omci_pm_mcast_get(
    UINT16 phyId,
    UINT16 portId,
    UINT32 *pCounter)
{
    CmsRet ret = CMSRET_SUCCESS;

    switch (phyId)
    {
        case MULTICAST_ENET_ID:
            // Test portId is associated with valid PptpEthernetUni.
            if (omci_pm_getEnetPptpObjIDs(portId) != INVALID_OBJ_ID)
            {
                *pCounter = multicastStatArray_ETH[portId];
            }
            else
            {
                ret = CMSRET_INVALID_ARGUMENTS;
                cmsLog_error("omci_pm_mcast_get() failed for invalid eth port index %d",
                  portId);
            }
            break;
#ifdef SUPPORT_MOCA
        case MULTICAST_MOCA_ID:
            if (portId < MOCA_PORT_MAX)
            {
                *pCounter = multicastStatArray_MoCA[portId];
            }
            else
            {
                ret = CMSRET_INVALID_ARGUMENTS;
                cmsLog_error("omci_pm_mcast_get() failed for invalid MoCA port index %d",
                  portId);
            }
            break;
#endif // SUPPORT_MOCA
        default:
            ret = CMSRET_INVALID_ARGUMENTS;
            cmsLog_error("omci_pm_mcast_get() failed for invalid physical port type %d",
              phyId);
            break;
    }

    return ret;
}

#endif // DMP_X_ITU_ORG_GPON_1
