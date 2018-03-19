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
#include "omciobj_defs.h"
#include "omci_util.h"

#include "omci_pm_llist.h"
#include "omci_pm_sync.h"
#include "omci_pm_poll.h"
#include "omci_th_check.h"

static BCM_COMMON_DECLARE_LL(omcipmLL);


/*========================= PRIVATE FUNCTIONS ==========================*/


static BCM_OMCIPM_ENTRY* pm_findEntryById(UINT16 obj_Type, UINT16 obj_ID)
{
    BCM_OMCIPM_ENTRY* omcipmEntry = BCM_COMMON_LL_GET_HEAD(omcipmLL);

    // Loop until specified entry is found or end-of-list.
    while (omcipmEntry != NULL)
    {
        // Test for specified entry.
        if ((omcipmEntry->omcipm.objType == (UINT16)obj_Type) &&
            (omcipmEntry->omcipm.objID == obj_ID))
        {
            // Specified entry found.
            break;
        }

        // Get list's next entry.
        omcipmEntry = BCM_COMMON_LL_GET_NEXT(omcipmEntry);
    }

    // Return specified entry if found, NULL if not.
    return omcipmEntry;
}


static void pm_freeEntry(BCM_OMCIPM_ENTRY* omcipmEntry)
{
    int loopIndex;

    // Test for valid omcipmEntry pointer.
    if (omcipmEntry != NULL)
    {
        // Remove entry from list.
        BCM_COMMON_LL_REMOVE(&omcipmLL, omcipmEntry);

        // Loop through omcipmEntry's stat rec array.
        for (loopIndex = OMCIPM_COUNTER_TYPE_A;
             loopIndex < OMCIPM_COUNTER_TYPE_MAX;
             loopIndex++)
        {
            // Test for valid pointer.
            if (omcipmEntry->pm[loopIndex] != NULL)
            {
                // Release omcipmEntry's stat rec pointer.
                CMSMEM_FREE_BUF_AND_NULL_PTR(omcipmEntry->pm[loopIndex]);
            }
        }

        // Free memory for this entry
        CMSMEM_FREE_BUF_AND_NULL_PTR(omcipmEntry);
    }
}

static BCM_OMCIPM_ENTRY* pm_allocEntry(
    UINT16 obj_Type,
    UINT16 obj_ID,
    UINT16 port_ID,
    UINT16 threshold_ID)
{
    BCM_OMCIPM_ENTRY* omcipmEntry = NULL;
    int allocLoopIndex = 0;
    int bufSize = 0;

    // Attempt to allocate entry & test result.
    omcipmEntry = (BCM_OMCIPM_ENTRY*)cmsMem_alloc(sizeof(BCM_OMCIPM_ENTRY), ALLOC_ZEROIZE);
    if (omcipmEntry != NULL)
    {
        // Initialize omcipmEntry fields.
        omcipmEntry->omcipm.objType = obj_Type;
        omcipmEntry->omcipm.objID = obj_ID;
        omcipmEntry->omcipm.portID = port_ID;
        omcipmEntry->omcipm.thresholdID = threshold_ID;

        omcipmEntry->reportedAlarmBits = omcipmEntry->initBaseFlag = omcipmEntry->debugFlag = 0;

        // Switch on class type to determine stat buffer size (ENET, GPON, MOCA, ...).
        bufSize = omci_pm_getObjTypeCounterBufSize(obj_Type);

        // Test for valid buffer page size.
        if (bufSize > 0)
        {
            // Test for ENET type objects.
            if ((obj_Type == MDMOID_ETHERNET_PM_HISTORY_DATA) ||
                (obj_Type == MDMOID_ETHERNET_PM_HISTORY_DATA2) ||
                (obj_Type == MDMOID_ETHERNET_PM_HISTORY_DATA3))
            {
                // Find object's corresponding physical port based on match to PPTP object ID.
                omcipmEntry->omcipm.portID = omci_pm_findEnetPort(omcipmEntry->omcipm.objID);
            }

            // Test for ENET Upstream/Downstream type objects.
            if ((obj_Type == MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA) ||
                (obj_Type == MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA))
            {
                // Find object's corresponding physical port based on match to MAC Bridge configuration data object ID.
                omcipmEntry->omcipm.portID = omci_pm_findUpDnEnetPort(omcipmEntry->omcipm.objID);
            }

            // Test for GEM type objects.
            if ((obj_Type == MDMOID_GEM_PORT_PM_HISTORY_DATA) ||
                (obj_Type == MDMOID_GAL_ETHERNET_PM_HISTORY_DATA))
            {
                // Find object's corresponding physical port based on match to CTP object ID.
                omcipmEntry->omcipm.portID = omci_pm_findGemPort(omcipmEntry->omcipm.objID);
            }

#ifdef DMP_X_ITU_ORG_VOICE_1
            // Test for VOIP RTP type objects.
            if (obj_Type == MDMOID_RTP_PM_HISTORY_DATA)
            {
                // Find object's corresponding physical port based on match to CTP object ID.
                omcipmEntry->omcipm.portID = omci_pm_findVoipPort(omcipmEntry->omcipm.objID);
            }
#endif // DMP_X_ITU_ORG_VOICE_1

            // Loop through stat buffer pages.
            for (allocLoopIndex = OMCIPM_COUNTER_TYPE_A;
                 allocLoopIndex < OMCIPM_COUNTER_TYPE_MAX;
                 allocLoopIndex++)
            {
                // Attempt to allocate buffer stat page & test result.
                omcipmEntry->pm[allocLoopIndex] = cmsMem_alloc(bufSize, ALLOC_ZEROIZE);
                if (omcipmEntry->pm[allocLoopIndex] == NULL)
                {
                    // Release OMCI PM memory on error
                    pm_freeEntry(omcipmEntry);
                    omcipmEntry = NULL;

                    // Stop loop.
                    break;
                }
            }
        }
    }

    // Return valid omcipmEntry on success, NULL on failure.
    return omcipmEntry;
}


/*========================= PUBLIC FUNCTIONS ==========================*/

UINT32 omci_pm_getObjTypeCounterBufSize(
    UINT16 objType)
{
    UINT32 bufSize = 0;

    switch (objType)
    {
        case MDMOID_GEM_PORT_PM_HISTORY_DATA:
            bufSize = sizeof(BCM_OMCI_PM_GEM_PORT_COUNTER_64);
            break;
        case MDMOID_FEC_PM_HISTORY_DATA:
            bufSize = sizeof(BCM_OMCI_PM_FEC_COUNTER);
            break;
        case MDMOID_ETHERNET_PM_HISTORY_DATA:
            bufSize = sizeof(BCM_OMCI_PM_ETHERNET_COUNTER);
            break;
        case MDMOID_ETHERNET_PM_HISTORY_DATA2:
            bufSize = sizeof(BCM_OMCI_PM_ETHERNET_2_COUNTER);
            break;
        case MDMOID_ETHERNET_PM_HISTORY_DATA3:
            bufSize = sizeof(BCM_OMCI_PM_ETHERNET_3_COUNTER);
            break;
        case MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA:
            bufSize = sizeof(BCM_OMCI_PM_ETHERNET_UPDN_COUNTER);
            break;
        case MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA:
            bufSize = sizeof(BCM_OMCI_PM_ETHERNET_UPDN_COUNTER);
            break;
        case MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA:
            bufSize = sizeof(BCM_OMCI_PM_MOCA_ETHERNET_COUNTER);
            break;
        case MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA:
            bufSize = sizeof(BCM_OMCI_PM_MOCA_INTERFACE_COUNTER);
            break;
        case MDMOID_GAL_ETHERNET_PM_HISTORY_DATA:
            bufSize = sizeof(BCM_OMCI_PM_GAL_ETHERNET_COUNTER);
            break;
        case MDMOID_RTP_PM_HISTORY_DATA:
            bufSize = sizeof(BCM_OMCI_PM_RTP_COUNTER);
            break;
        case MDMOID_IP_HOST_PM_HISTORY_DATA:
            bufSize = sizeof(BCM_OMCI_PM_IP_HOST_COUNTER);
            break;
        case MDMOID_MAC_BRIDGE_PM_HISTORY_DATA:
            bufSize = sizeof(BCM_OMCI_PM_MAC_BRIDGE_COUNTER);
            break;
        case MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA:
            bufSize = sizeof(BCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER);
            break;
        case MDMOID_ETHERNET_FRAME_EXTENDED_P_M:
            bufSize = sizeof(BCM_OMCI_PM_ETHERNET_UPDN_COUNTER);
            break;
        default:
            cmsLog_error("Invalid objType: %d", objType);
            break;
    }

    return bufSize;
}

CmsRet omci_pm_findEntryById(
    UINT16 obj_Type,
    UINT16 obj_ID)
{
    CmsRet result = CMSRET_OBJECT_NOT_FOUND;
    BCM_OMCIPM_ENTRY* omcipmEntry = BCM_COMMON_LL_GET_HEAD(omcipmLL);

    // Loop until specified entry is found or end-of-list.
    while (omcipmEntry != NULL)
    {
        // Test for specified entry.
        if ((omcipmEntry->omcipm.objType == (UINT16)obj_Type) &&
            (omcipmEntry->omcipm.objID == obj_ID))
        {
            result = CMSRET_SUCCESS;
            // Specified entry found.
            break;
        }

        // Get list's next entry.
        omcipmEntry = BCM_COMMON_LL_GET_NEXT(omcipmEntry);
    }

    // Return specified entry if found, NULL if not.
    return result;
}

UINT32 omci_pm_create(
    UINT16 obj_Type,
    UINT16 obj_ID,
    UINT16 port_ID,
    UINT16 threshold_ID)
{
    UINT32 result = OMCI_PM_STATUS_SUCCESS;
    BCM_OMCIPM_ENTRY* omcipmEntryPtr = NULL;

    // Allocate a new OMCI PM entry & test result.
    omcipmEntryPtr = pm_allocEntry(obj_Type, obj_ID, port_ID, threshold_ID);

    if (omcipmEntryPtr != NULL)
    {
        // Add new omcipmEntry to linked-list.
        BCM_COMMON_LL_INSERT(&omcipmLL, omcipmEntryPtr, LL_POSITION_BEFORE,
          (BCM_OMCIPM_ENTRY*)BCM_COMMON_LL_GET_HEAD(omcipmLL));
    }
    else
    {
        // Signal entry-not-found error.
        cmsLog_error("omci_pm_create: Could not allocate OMCI PM Entry memory for Type: %d  ID: %d  Port: %d", obj_Type, obj_ID, port_ID);
        result = OMCI_PM_STATUS_ERROR;
    }

    // Return CMSRET_SUCCESS if object allocated, CMSRET_RESOURCE_EXCEEDED if not.
    return result;
}


UINT32 omci_pm_delete(
    BCM_OMCI_PM_CLASS_ID obj_Class,
    UINT16 obj_ID)
{
    UINT32 result = OMCI_PM_STATUS_SUCCESS;
    BCM_OMCIPM_ENTRY* omcipmEntry = NULL;

    // Find specified omcipmEntry by class and object ID & test result.
    omcipmEntry = pm_findEntryById(obj_Class, obj_ID);
    if (omcipmEntry != NULL)
    {
        // Release allocated memory.
        pm_freeEntry(omcipmEntry);
    }
    else
    {
        // Signal entry-not-found error.
        cmsLog_error("omci_pm_delete: Could not find OMCI PM Entry Class: %d  Object: %d",
          obj_Class, obj_ID);
        result = OMCI_PM_STATUS_NOT_FOUND;
    }

    // Return OMCI_PM_STATUS_SUCCESS if object removed, OMCI_PM_STATUS_NOT_FOUND if not.
    return result;
}

void* omci_pm_get(
    BCM_OMCI_PM_CLASS_ID obj_Class,
    UINT16 obj_ID,
    UINT16 currentDataFlag)
{
    void* dataPtr = NULL;
    BCM_OMCIPM_ENTRY* omcipmEntry = NULL;
    UINT8 dataArrayIndex = 0;

    // Find specified omcipmEntry by class and object ID & test result.
    omcipmEntry = pm_findEntryById(obj_Class, obj_ID);
    if (omcipmEntry != NULL)
    {
        // Test for get/get_current request.
        // continuous accumulation always return current
        if (omcipmEntry->omcipm.objType == MDMOID_ETHERNET_FRAME_EXTENDED_P_M &&
            omcipmEntry->omcipm.controlFields & 0x1)
        {
            // Read current entry's counters.
            omci_pm_poll(omcipmEntry);

            // Setup array index for current stats.
            dataArrayIndex = 0;
        }
        else if (currentDataFlag == TRUE)
        {
            // Read current entry's counters.
            //Poll_Entry(omcipmEntry);
            omci_pm_poll(omcipmEntry);

            // Setup array index for current stats.
            dataArrayIndex = omci_pm_getIntervalCounter() & 1;
        }
        else
        {
            // Setup array index for saved stats.
            dataArrayIndex = (omci_pm_getIntervalCounter() + 1) & 1;
        }

        // Setup appropriate data array pointer.
        dataPtr = omcipmEntry->pm[dataArrayIndex];
    }
    else
    {
        // Signal entry-not-found error.
        dataPtr = NULL;
    }

    // Return valid data pointer (current or saved) on success, NULL on failure.
    return dataPtr;
}

UINT32 omci_pm_set(
    UINT16 obj_Class,
    UINT16 obj_ID,
    UINT16 threshold_ID)
{
    UINT32 result = OMCI_PM_STATUS_SUCCESS;
    BCM_OMCIPM_ENTRY* omcipmEntry = NULL;

    // Find specified omcipmEntry by class and object ID & test result.
    omcipmEntry = pm_findEntryById(obj_Class, obj_ID);

    // Test for valid entry.
    if (omcipmEntry != NULL)
    {
        // Set threshold ID in entry.
        omcipmEntry->omcipm.thresholdID = threshold_ID;

        // Set managed entity's ID to new ID.
        omcipmEntry->omcipm.objID = obj_ID;

        // Test for ENET type objects.
        if ((obj_Class == MDMOID_ETHERNET_PM_HISTORY_DATA) ||
            (obj_Class == MDMOID_ETHERNET_PM_HISTORY_DATA2) ||
            (obj_Class == MDMOID_ETHERNET_PM_HISTORY_DATA3))
        {
            // Find object's corresponding physical port based on match to PPTP object ID.
            omcipmEntry->omcipm.portID = omci_pm_findEnetPort(omcipmEntry->omcipm.objID);
        }

        // Test for ENET Upstream/Downstream type objects.
        if ((obj_Class == MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA) ||
            (obj_Class == MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA))
        {
            // Find object's corresponding physical port based on match to MAC Bridge configuration data object ID.
            omcipmEntry->omcipm.portID = omci_pm_findUpDnEnetPort(omcipmEntry->omcipm.objID);
        }

        // Test for GPON type objects.
        if ((obj_Class == MDMOID_GEM_PORT_PM_HISTORY_DATA) ||
            (obj_Class == MDMOID_GAL_ETHERNET_PM_HISTORY_DATA))
        {
            // Find object's corresponding physical port based on match to CTP object ID.
            omcipmEntry->omcipm.portID = omci_pm_findGemPort(omcipmEntry->omcipm.objID);
        }

        // Signal success.
        result = OMCI_PM_STATUS_SUCCESS;
    }
    else
    {
        // Signal entry-not-found error.
        cmsLog_error("omci_pm_set: Could not find OMCI PM Entry Class: %d  Object: %d",
          obj_Class, obj_ID);
        result = OMCI_PM_STATUS_NOT_FOUND;
    }

    // Return OMCI_PM_STATUS_SUCCESS if object set, OMCI_PM_STATUS_NOT_FOUND if not.
    return result;
}

void omci_pm_initStatBuffers(UINT8 intervalCounter)
{
    BCM_OMCIPM_ENTRY* omcipmEntry = NULL;
    UINT32 bufSize;

    // Get omcipmEntry linked-list 1st item.
    omcipmEntry = BCM_COMMON_LL_GET_HEAD(omcipmLL);

    // Loop until every linked-list item examined.
    while (omcipmEntry != NULL)
    {
        bufSize = omci_pm_getObjTypeCounterBufSize(omcipmEntry->omcipm.objType);
        if (bufSize != 0)
        {
            if (omcipmEntry->omcipm.objType == MDMOID_ETHERNET_FRAME_EXTENDED_P_M)
            {
                /* Skip memset() if continuous accumulation enabled. */
                if ((omcipmEntry->omcipm.controlFields & 0x1) == 0)
                {
                    memset(omcipmEntry->pm[intervalCounter & 1], 0x0, bufSize);
                }
            }
            else
            {
                memset(omcipmEntry->pm[intervalCounter & 1], 0x0, bufSize);
            }

            if (omcipmEntry->reportedAlarmBits != 0)
            {
                // Send OLT an alarm clear message with all reported alarm bits.
                omci_th_send_alarm(omcipmEntry, 0, FALSE);
            }
        }

        // Get next linked-list item (if any).
        omcipmEntry = BCM_COMMON_LL_GET_NEXT(omcipmEntry);
    }
}

void omci_pm_releaseAll(void)
{
    BCM_OMCIPM_ENTRY* omcipmEntry = NULL;
    BCM_OMCIPM_ENTRY* nextOmcipmEntry = NULL;

    // Get 1st entry (if any).
    omcipmEntry = BCM_COMMON_LL_GET_HEAD(omcipmLL);

    // Loop until all entries are released (including stat buffers) or end-of-list.
    while (omcipmEntry != NULL)
    {
        // Get list's next entry.
        nextOmcipmEntry = BCM_COMMON_LL_GET_NEXT(omcipmEntry);

        // Free current entry.
        pm_freeEntry(omcipmEntry);

        // Setup list's next entry.
        omcipmEntry = nextOmcipmEntry;
    }

    // Re-initialize head and tail to NULL
    BCM_COMMON_LL_INIT(&omcipmLL);
}


BCM_COMMON_LLIST* omci_pm_getListPtr(void)
{
    // Return OMCIPMD master entry linked-list.
    return &omcipmLL;
}

static void set_extPm_entry(
    BCM_OMCIPM_ENTRY *omcipmEntry,
    UINT8 *control)
{
    UINT32 bufSize;

    omcipmEntry->omcipm.thresholdID = (control[0] << 8) | control[1];
    omcipmEntry->omcipm.parentMeClass = (control[2] << 8) | control[3];
    omcipmEntry->omcipm.parentMeInstance = (control[4] << 8) | control[5];
    omcipmEntry->omcipm.accumulationDisable = (control[6] << 8) | control[7];
    omcipmEntry->omcipm.tcaDisable = (control[8] << 8) | control[9];
    omcipmEntry->omcipm.controlFields = (control[10] << 8) | control[11];
    omcipmEntry->omcipm.tci = (control[12] << 8) | control[13];

    cmsLog_notice("EXT PM: thrID=0x%02x, PClass=0x%02x, PInst=0x%02x, "
      "accDis=0x%02x, tcaDis=0x%02x, ctrl=0x%02x, tci=0x%02x\n",
      omcipmEntry->omcipm.thresholdID,
      omcipmEntry->omcipm.parentMeClass,
      omcipmEntry->omcipm.parentMeInstance,
      omcipmEntry->omcipm.accumulationDisable,
      omcipmEntry->omcipm.tcaDisable,
      omcipmEntry->omcipm.controlFields,
      omcipmEntry->omcipm.tci);

    switch (omcipmEntry->omcipm.parentMeClass)
    {
        case 47: /* MAC bridge port configuration data */
            omcipmEntry->omcipm.portID = omci_pm_findPhysicalPort(
              omcipmEntry->omcipm.parentMeInstance, &omcipmEntry->omcipm.tpType);
            break;
        case 11: /* Physical path termination point Ethernet UNI */
            omcipmEntry->omcipm.portID = omci_pm_findEnetPort(
              omcipmEntry->omcipm.parentMeInstance);
            omcipmEntry->omcipm.tpType = MAC_BRIDGE_ETHERNET_PPTP_TYPE;
            break;
        case 266: /* GEM interworking termination point */
            omcipmEntry->omcipm.portID = omci_pm_findIwtpGemPort(
              omcipmEntry->omcipm.parentMeInstance);
            omcipmEntry->omcipm.tpType = MAC_BRIDGE_IWTP_TYPE;
            break;
        case 281: /* Multicast GEM interworking termination point */
            omcipmEntry->omcipm.portID = omci_pm_findMcastIwtpGemPort(
              omcipmEntry->omcipm.parentMeInstance);
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

    /* Global clear. */
    if (omcipmEntry->omcipm.accumulationDisable & (1 << 15))
    {
        bufSize = omci_pm_getObjTypeCounterBufSize(omcipmEntry->omcipm.objType);
        memset(omcipmEntry->pm[0], 0x0, bufSize);
        memset(omcipmEntry->pm[1], 0x0, bufSize);
        omci_th_send_alarm(omcipmEntry, 0, FALSE);
    }
}

UINT32 omci_extPm_create(
    UINT16 obj_Type,
    UINT16 obj_ID,
    UINT8 *control)
{
    UINT32 result = OMCI_PM_STATUS_SUCCESS;
    BCM_OMCIPM_ENTRY* omcipmEntry = NULL;

    // Allocate a new OMCI PM entry & test result.
    omcipmEntry = pm_allocEntry(obj_Type, obj_ID, 0, 0);

    if (omcipmEntry != NULL)
    {
        set_extPm_entry(omcipmEntry, control);
        // Add new omcipmEntry to linked-list.
        BCM_COMMON_LL_INSERT(&omcipmLL, omcipmEntry, LL_POSITION_BEFORE,
          (BCM_OMCIPM_ENTRY*)BCM_COMMON_LL_GET_HEAD(omcipmLL));
    }
    else
    {
        // Signal entry-not-found error.
        cmsLog_error("omci_pm_create: pm_allocEntry() failed for Type: %d  ID: %d",
          obj_Type, obj_ID);
        result = OMCI_PM_STATUS_ERROR;
    }

    // Return CMSRET_SUCCESS if object allocated, CMSRET_RESOURCE_EXCEEDED if not.
    return result;
}

UINT32 omci_extPm_set(
    UINT16 obj_Class,
    UINT16 obj_ID,
    UINT8 *control)
{
    UINT32 result = OMCI_PM_STATUS_SUCCESS;
    BCM_OMCIPM_ENTRY* omcipmEntry = NULL;

    // Find specified omcipmEntry by class and object ID & test result.
    omcipmEntry = pm_findEntryById(obj_Class, obj_ID);

    // Test for valid entry.
    if (omcipmEntry != NULL)
    {
        set_extPm_entry(omcipmEntry, control);
        // Signal success.
        result = OMCI_PM_STATUS_SUCCESS;
    }
    else
    {
        // Signal entry-not-found error.
        cmsLog_error("omci_pm_set: Could not find OMCI PM Entry Class: %d  Object: %d",
          obj_Class, obj_ID);
        result = OMCI_PM_STATUS_NOT_FOUND;
    }

    // Return OMCI_PM_STATUS_SUCCESS if object set, OMCI_PM_STATUS_NOT_FOUND if not.
    return result;
}

#endif // DMP_X_ITU_ORG_GPON_1

