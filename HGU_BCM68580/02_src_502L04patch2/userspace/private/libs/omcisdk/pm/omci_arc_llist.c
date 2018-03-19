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

#include "owsvc_api.h"

#include "omci_alarm_llist.h"
#include "omci_arc_llist.h"
#include "omci_pm.h"
#include "omci_pm_sync.h"
#include "omci_util.h"

#include "bcm_ploam_api.h"
#include "gponctl_api.h"
#include "omciobj_defs.h"
#include "mdmlite_api.h"


static BCM_COMMON_DECLARE_LL(arcEntryLL);


/*========================= PRIVATE FUNCTIONS ==========================*/


static UINT8 arc_test_enet(UINT32 cbParam)
{
    UINT8 returnResult = FALSE;

    // Test for valid ethernet connection.
    if (omci_pm_getEnetPptpInfo((UINT16)cbParam) != 0)
    {
        // Signal connection is valid.
        returnResult = TRUE;
    }

    // Return TRUE if connection is problem-free, FALSE if not.
    return returnResult;
}


static UINT8 arc_test_ani_g(UINT32 cbParam __attribute__((unused)))
{
    BCM_Ploam_AlarmStatusInfo alarmStatusInfo;
    UINT8 returnResult = FALSE;

    // Attempt to get GPON alarm status.
    if (gponCtl_getAlarmStatus(&alarmStatusInfo) == 0)
    {
        // Test for SF and/or SD alarm.
        if ((alarmStatusInfo.alarmEventBitmap &
          (BCM_PLOAM_ALARM_ID_SF | BCM_PLOAM_ALARM_ID_SD)) == 0)
        {
            // Signal connection is valid and problem-free.
            returnResult = TRUE;
        }
    }

    // Return TRUE if connection is problem-free, FALSE if not.
    return returnResult;
}


static UBOOL8 arc_disable_enet(BCM_OMCIPM_ARC_ENTRY* arcEntryPtr)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet cmsResult = CMSRET_SUCCESS;
    PptpEthernetUniObject* objPtr = NULL;
    UBOOL8 found = FALSE, result = FALSE;
    UINT8 arcAttr = 0;

    // Loop until MDMOID_PPTP_ETHERNET_UNI has been found or no more objects to compare.
    while ((found == FALSE) &&
      (_cmsObj_getNext(MDMOID_PPTP_ETHERNET_UNI, &iidStack, (void**)&objPtr))
      == CMSRET_SUCCESS)
    {
        // Test for specified object.
        if (objPtr->managedEntityId == arcEntryPtr->objectID)
        {
            found = TRUE;

            // Remove ENET PPTP ARC entry.
            omci_arc_delete(arcEntryPtr->objectType, arcEntryPtr->objectID);

            // Set ARC field to disabled.
            objPtr->alarmReportingControl = FALSE;

            // Attempt to write edited object back to CMS.
            cmsResult = _cmsObj_set(objPtr, &iidStack);

            // Test write for success.
            if (cmsResult == CMSRET_SUCCESS)
            {
                // Signal success.
                result = TRUE;

                // Send AVC to OLT to signal MDMOID_PPTP_ETHERNET_UNI ARC disable.
                omciUtl_sendAttributeValueChange(arcEntryPtr->objectType,
                                                 arcEntryPtr->objectID,
                                                 BCM_AVC_ENET_ARC_VAL,
                                                 &arcAttr, sizeof(arcAttr));
            }
            else
            {
                // Report object write error.
                cmsLog_error("CMS object cannot be written, cmsResult: %d", cmsResult);
            }
        }

        // Free non-specified object and continue looking for specified object.
        _cmsObj_free((void**)&objPtr);
    }

    // Return TRUE if CMS object's ARC disabled, FALSE if not.
    return result;
}

static UBOOL8 arc_disable_ani_g(BCM_OMCIPM_ARC_ENTRY* arcEntryPtr)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet cmsResult = CMSRET_SUCCESS;
    AniGObject* objPtr = NULL;
    UBOOL8 found = FALSE, result = FALSE;
    UINT8 arcAttr = 0;

    // Loop until MDMOID_ANI_G has been found or no more objects to compare.
    while ((found == FALSE) &&
           (_cmsObj_getNext(MDMOID_ANI_G, &iidStack, (void**)&objPtr)) == CMSRET_SUCCESS)
    {
        // Test for specified object.
        if (objPtr->managedEntityId == arcEntryPtr->objectID)
        {
            found = TRUE;

            // Remove MDMOID_ANI_G ARC entry.
            omci_arc_delete(arcEntryPtr->objectType, arcEntryPtr->objectID);

            // Set ARC field to disabled.
            objPtr->alarmReportingControl = FALSE;

            // Attempt to write edited object back to CMS.
            cmsResult = _cmsObj_set(objPtr, &iidStack);

            // Test write for success.
            if (cmsResult == CMSRET_SUCCESS)
            {
                // Signal success.
                result = TRUE;

                // Send AVC to OLT to signal MDMOID_PPTP_ETHERNET_UNI ARC disable.
                omciUtl_sendAttributeValueChange(arcEntryPtr->objectType,
                                                 arcEntryPtr->objectID,
                                                 BCM_AVC_ANIG_ARC_VAL,
                                                 &arcAttr, sizeof(arcAttr));
            }
            else
            {
                // Report object write error.
                cmsLog_error("CMS object cannot be written, cmsResult: %d", cmsResult);
            }
        }

        // Free non-specified object and continue looking for specified object.
        _cmsObj_free((void**)&objPtr);
    }

    // Return TRUE if CMS object's ARC disabled, FALSE if not.
    return result;
}

static UBOOL8 arc_disable(BCM_OMCIPM_ARC_ENTRY* arcEntryPtr)
{
    UBOOL8 result = FALSE;

    // Switch on object type.
    switch (arcEntryPtr->objectType)
    {
        case MDMOID_ANI_G:
            // Edit CMS MDMOID_ANI_G object
            result = arc_disable_ani_g(arcEntryPtr);
            break;
        case MDMOID_PPTP_ETHERNET_UNI:
            // Edit CMS MDMOID_PPTP_ETHERNET_UNI object
            result = arc_disable_enet(arcEntryPtr);
            break;
        default:
            // Report invalid object error.
            cmsLog_error("Invalid object type: %d", arcEntryPtr->objectType);
            break;
    }

    // Return TRUE if CMS object was found, edited, and AVC was sent, FALSE if not.
    return result;
}

/*========================= PUBLIC FUNCTIONS ==========================*/

UINT32 omci_arc_create(
    UINT16 class_ID,
    UINT16 obj_ID)
{
    UINT32 returnVal = OMCI_PM_STATUS_ERROR;
    BCM_OMCIPM_ARC_ENTRY* arcEntryPtr = NULL;

    // Test on object type.
    if (class_ID != MDMOID_ANI_G &&
        class_ID != MDMOID_PPTP_ETHERNET_UNI)
    {
        // Report invalid object error.
        cmsLog_error("Invalid object type: %d", class_ID);
        return returnVal;
    }

    // Attempt to allocate memory for ARC record.
    arcEntryPtr = (BCM_OMCIPM_ARC_ENTRY*)cmsMem_alloc(
      sizeof(BCM_OMCIPM_ARC_ENTRY), ALLOC_ZEROIZE);

    // Test for success.
    if (arcEntryPtr != NULL)
    {
        // Init ARC entry.
        memset(arcEntryPtr, 0, sizeof(BCM_OMCIPM_ARC_ENTRY));

        // Setup ARC entry.
        arcEntryPtr->objectType = class_ID;
        arcEntryPtr->objectID = obj_ID;

        // Setup ARC callback function.
        switch (class_ID)
        {
            case MDMOID_ANI_G:
                arcEntryPtr->cbFunction = arc_test_ani_g;
                break;
            case MDMOID_PPTP_ETHERNET_UNI:
                arcEntryPtr->cbFunction = arc_test_enet;
                break;
        }

        // Add new ARC entry to list
        BCM_COMMON_LL_APPEND(&arcEntryLL, arcEntryPtr);

        returnVal = OMCI_PM_STATUS_SUCCESS;
    }

    // Return OMCI_PM_STATUS_SUCCESS on success, OMCI_PM_STATUS_ALLOC_ERROR on failure.
    return returnVal;
}

UINT32 omci_arc_delete(
    UINT16 class_ID,
    UINT16 obj_ID)
{
    UINT32 returnVal = OMCI_PM_STATUS_ERROR;
    BCM_OMCIPM_ARC_ENTRY* arcEntryPtr = NULL;

    // Get 1st entry (if any).
    arcEntryPtr = BCM_COMMON_LL_GET_HEAD(arcEntryLL);

    // Loop until all entries hace been searched or end-of-list.
    while (arcEntryPtr != NULL)
    {
        // Test for specified ARC object.
        if ((arcEntryPtr->objectType == class_ID) &&
            (arcEntryPtr->objectID == obj_ID))
        {
            // Remove entry from list.
            BCM_COMMON_LL_REMOVE(&arcEntryLL, arcEntryPtr);

            // Release entry's memory.
            CMSMEM_FREE_BUF_AND_NULL_PTR(arcEntryPtr);

            returnVal = OMCI_PM_STATUS_SUCCESS;

            // Done.
            break;
        }

        // Get list's next entry.
        arcEntryPtr = BCM_COMMON_LL_GET_NEXT(arcEntryPtr);
    }

    // Return OMCI_PM_STATUS_SUCCESS on success, OMCI_PM_STATUS_ALLOC_ERROR on failure.
    return returnVal;
}

BCM_OMCIPM_ARC_ENTRY* omci_arc_get(
    UINT16 class_ID,
    UINT16 obj_ID)
{
    BCM_OMCIPM_ARC_ENTRY* arcEntryPtr = NULL;

    // Get 1st entry (if any).
    arcEntryPtr = BCM_COMMON_LL_GET_HEAD(arcEntryLL);

    // Loop until all entries have been searched or end-of-list.
    while (arcEntryPtr != NULL)
    {
        // Test for specified ARC object.
        if ((arcEntryPtr->objectType == class_ID) &&
            (arcEntryPtr->objectID == obj_ID))
        {
            // Done.
            break;
        }

        // Get list's next entry.
        arcEntryPtr = BCM_COMMON_LL_GET_NEXT(arcEntryPtr);
    }

    // Return valid entry if ARC object found, NULL if not.
    return arcEntryPtr;
}

UINT32 omci_arc_set(
    UINT16 class_ID,
    UINT16 obj_ID,
    UINT16 arc_Interval)
{
    UINT32 returnVal = OMCI_PM_STATUS_ERROR;
    BCM_OMCIPM_ARC_ENTRY* arcEntryPtr = NULL;

    // Test on object type.
    if (class_ID != MDMOID_ANI_G &&
        class_ID != MDMOID_PPTP_ETHERNET_UNI)
    {
        // Report invalid object error.
        cmsLog_error("Invalid object type: %d", class_ID);
        return returnVal;
    }

    // Attempt to find existing ARC entry.
    arcEntryPtr = omci_arc_get(class_ID, obj_ID);

    if (arcEntryPtr != NULL)
    {
        // Setup ARC rec fields.
        arcEntryPtr->arcInterval = arc_Interval;
        arcEntryPtr->expireTime = time(NULL) + (arc_Interval * SECS_PER_MINUTE);

        returnVal = OMCI_PM_STATUS_SUCCESS;
    }

    // Return OMCI_PM_STATUS_SUCCESS on success, OMCI_PM_STATUS_ALLOC_ERROR on failure.
    return returnVal;
}

UBOOL8 omci_arc_exist(
    UINT16 class_ID,
    UINT16 obj_ID)
{
    BCM_OMCIPM_ARC_ENTRY* arcEntryPtr = NULL;

    // Attempt to find existing ARC entry.
    arcEntryPtr = omci_arc_get(class_ID, obj_ID);

    // Return TRUE if ARC entry exists and alarm supressed, FALSE if not.
    return (arcEntryPtr != NULL);
}

void omci_arc_refresh_timer(
    UINT16 class_ID,
    UINT16 obj_ID)
{
    BCM_OMCIPM_ARC_ENTRY* arcEntryPtr = NULL;

    // Attempt to find existing ARC entry.
    arcEntryPtr = omci_arc_get(class_ID, obj_ID);

    // Test if ARC entry has been found.
    if (arcEntryPtr != NULL)
    {
        // Reset ARC timeout.
        arcEntryPtr->expireTime = time(NULL) + (arcEntryPtr->arcInterval * SECS_PER_MINUTE);
    }
}

void omci_arc_poll(void)
{
    BCM_OMCIPM_ARC_ENTRY* arcEntryPtr = NULL;
    time_t currentTime = time(NULL);
    UBOOL8 disableARC = FALSE;

    // Get 1st entry (if any).
    arcEntryPtr = BCM_COMMON_LL_GET_HEAD(arcEntryLL);

    // Loop until all entries are processed or end-of-list.
    while (arcEntryPtr != NULL)
    {
        // Setup flag.
        disableARC = FALSE;

        // Test for not indefinite ARC interval.
        if (arcEntryPtr->arcInterval != ARC_INTERVAL_INDEF)
        {
            // Test if ARC entry has expired.
            if (currentTime >= arcEntryPtr->expireTime)
            {
                // Test for valid callback.  NOTE Could be NULL if DisableARC() fails.
                if (arcEntryPtr->cbFunction != NULL)
                {
                    // Execute ARC callback function to determine if system is in problem-free state.
                    disableARC = arcEntryPtr->cbFunction(arcEntryPtr->objectID);

                    // Test if ARC callback function determines system is not in problem-free state.
                    if (disableARC == FALSE)
                    {
                        // System is NOT in problem-free state,
                        // Reset ARC timeout.
                        arcEntryPtr->expireTime = currentTime +
                          (arcEntryPtr->arcInterval * SECS_PER_MINUTE);
                    }
                    else
                    {
                        // System is in problem-free state,
                        // Attempt to disable ARC on timeout & test result,
                        // And remove entry from linked-list.
                        arc_disable(arcEntryPtr);
                    }
                }
            }
        }

        // Get list's next entry.
        arcEntryPtr = BCM_COMMON_LL_GET_NEXT(arcEntryPtr);
    }
}

void omci_arc_releaseAll(void)
{
    BCM_OMCIPM_ARC_ENTRY* arcEntry = NULL;
    BCM_OMCIPM_ARC_ENTRY* nextArcEntry = NULL;

    // Get 1st entry (if any).
    arcEntry = BCM_COMMON_LL_GET_HEAD(arcEntryLL);

    // Loop until all entries are released (including stat buffers) or end-of-list.
    while (arcEntry != NULL)
    {
        // Get list's next entry.
        nextArcEntry = BCM_COMMON_LL_GET_NEXT(arcEntry);

        // Free current entry.
        CMSMEM_FREE_BUF_AND_NULL_PTR(arcEntry);

        // Setup list's next entry.
        arcEntry = nextArcEntry;
    }

    // Re-initialize head and tail to NULL
    BCM_COMMON_LL_INIT(&arcEntryLL);
}

#endif // DMP_X_ITU_ORG_GPON_1
