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

#include "omci_alarm_llist.h"

static BCM_COMMON_DECLARE_LL(frozenAlarmLL);
BCM_COMMON_DECLARE_LL(nonTcaAlarmLL);


/*========================= PRIVATE FUNCTIONS ==========================*/

/*========================= PUBLIC FUNCTIONS ==========================*/


UINT32 omci_alarm_create(
    BCM_COMMON_LLIST *pHead,
    UINT16 class_ID,
    UINT16 obj_ID,
    UINT16 alarmBitmap)
{
    UINT32 returnVal = OMCI_PM_STATUS_ERROR;
    BCM_OMCIPM_ALARM_ENTRY* alarmEntryPtr = NULL;

    if (pHead == NULL)
    {
        cmsLog_error("Invalid Head of alarm link-list");
        return returnVal;
    }

    // Attempt to allocate memory for alarm entry.
    alarmEntryPtr = (BCM_OMCIPM_ALARM_ENTRY*)cmsMem_alloc(
      sizeof(BCM_OMCIPM_ALARM_ENTRY), ALLOC_ZEROIZE);

    // Test for success.
    if (alarmEntryPtr != NULL)
    {
        // Init ALARM entry.
        memset(alarmEntryPtr, 0, sizeof(BCM_OMCIPM_ALARM_ENTRY));

        // Setup ALARM entry.
        alarmEntryPtr->objectType = class_ID;
        alarmEntryPtr->objectID = obj_ID;
        alarmEntryPtr->alarmBitmap = alarmBitmap;

        // Add new ALARM entry to list
        BCM_COMMON_LL_APPEND(pHead, alarmEntryPtr);

        returnVal = OMCI_PM_STATUS_SUCCESS;
    }

    // Return OMCI_PM_STATUS_SUCCESS on success, OMCI_PM_STATUS_ALLOC_ERROR on failure.
    return returnVal;
}

UINT32 omci_alarm_delete(
    BCM_COMMON_LLIST *pHead,
    UINT16 class_ID,
    UINT16 obj_ID)
{
    UINT32 returnVal = OMCI_PM_STATUS_ERROR;
    BCM_OMCIPM_ALARM_ENTRY *alarmEntryPtr = NULL;

    if (pHead == NULL)
    {
        cmsLog_error("Invalid Head of alarm link-list");
        return returnVal;
    }

    alarmEntryPtr = omci_alarm_get(pHead, class_ID, obj_ID);

    if (alarmEntryPtr != NULL)
    {
        // Remove entry from list.
        BCM_COMMON_LL_REMOVE(pHead, alarmEntryPtr);

        // Release entry's memory.
        CMSMEM_FREE_BUF_AND_NULL_PTR(alarmEntryPtr);

        returnVal = OMCI_PM_STATUS_SUCCESS;
    }

    // Return OMCI_PM_STATUS_SUCCESS on success, OMCI_PM_STATUS_ALLOC_ERROR on failure.
    return returnVal;
}

BCM_OMCIPM_ALARM_ENTRY* omci_alarm_getByIndex(
    const BCM_COMMON_LLIST *pHead,
    UINT16 alarmIndex)
{
    UINT16 alarmCount = 0;
    BCM_OMCIPM_ALARM_ENTRY *alarmEntryPtr = NULL;

    if (pHead == NULL)
    {
        cmsLog_error("Invalid Head of alarm link-list");
        return alarmEntryPtr;
    }

    // Get 1st entry (if any).
    alarmEntryPtr = BCM_COMMON_LL_GET_HEAD(*pHead);

    // Loop until all entries have been searched or end-of-list.
    while (alarmEntryPtr != NULL)
    {
        // Test for specified alarm.
        if (alarmCount == alarmIndex)
        {
            // Done.
            break;
        }

        // Inc alarm count for next entry.
        alarmCount++;

        // Get list's next entry.
        alarmEntryPtr = BCM_COMMON_LL_GET_NEXT(alarmEntryPtr);
    }

    // Return valid entry if Alarm object found, NULL if not.
    return alarmEntryPtr;
}

BCM_OMCIPM_ALARM_ENTRY* omci_alarm_get(
    const BCM_COMMON_LLIST *pHead,
    UINT16 class_ID,
    UINT16 obj_ID)
{
    BCM_OMCIPM_ALARM_ENTRY* alarmEntryPtr = NULL;

    if (pHead == NULL)
    {
        cmsLog_error("Invalid Head of alarm link-list");
        return alarmEntryPtr;
    }

    // Get 1st entry (if any).
    alarmEntryPtr = BCM_COMMON_LL_GET_HEAD(*pHead);

    // Loop until all entries have been searched or end-of-list.
    while (alarmEntryPtr != NULL)
    {
        // Test for specified alarm.
        if ((alarmEntryPtr->objectType == class_ID) &&
            (alarmEntryPtr->objectID == obj_ID))
        {
            // Done.
            break;
        }

        // Get list's next entry.
        alarmEntryPtr = BCM_COMMON_LL_GET_NEXT(alarmEntryPtr);
    }

    // Return valid entry if Alarm object found, NULL if not.
    return alarmEntryPtr;
}

UINT32 omci_alarm_set(
    const BCM_COMMON_LLIST *pHead,
    UINT16 class_ID,
    UINT16 obj_ID,
    UINT16 alarmBitmap)
{
    UINT32 returnVal = OMCI_PM_STATUS_ERROR;
    BCM_OMCIPM_ALARM_ENTRY* alarmEntryPtr = NULL;

    if (pHead == NULL)
    {
        cmsLog_error("Invalid Head of alarm link-list");
        return returnVal;
    }

    // Attempt to find existing alarm entry.
    alarmEntryPtr = omci_alarm_get(pHead, class_ID, obj_ID);

    if (alarmEntryPtr != NULL)
    {
        // Setup alarm bitmap.
        alarmEntryPtr->alarmBitmap = alarmBitmap;

        returnVal = OMCI_PM_STATUS_SUCCESS;
    }

    // Return OMCI_PM_STATUS_SUCCESS on success, OMCI_PM_STATUS_ALLOC_ERROR on failure.
    return returnVal;
}

void omci_alarm_releaseByHeadList(
    BCM_COMMON_LLIST *pHead)
{
    BCM_OMCIPM_ALARM_ENTRY* alarmEntry = NULL;
    BCM_OMCIPM_ALARM_ENTRY* nextAlarmEntry = NULL;

    if (pHead == NULL)
    {
        cmsLog_error("Invalid Head of alarm link-list");
        return;
    }

    // Get 1st entry (if any).
    alarmEntry = BCM_COMMON_LL_GET_HEAD(*pHead);

    // Loop until all entries are released or end-of-list.
    while (alarmEntry != NULL)
    {
        // Get list's next entry.
        nextAlarmEntry = BCM_COMMON_LL_GET_NEXT(alarmEntry);

        // Free current entry.
        CMSMEM_FREE_BUF_AND_NULL_PTR(alarmEntry);

        // Setup list's next entry.
        alarmEntry = nextAlarmEntry;
    }

    // Re-initialize head and tail to NULL
    BCM_COMMON_LL_INIT(pHead);
}

BCM_COMMON_LLIST* omci_alarm_getFrozenListPtr(void)
{
    // Return Head of frozen alarm linked-list.
    return &frozenAlarmLL;
}

BCM_COMMON_LLIST* omci_alarm_getNonTcaListPtr(void)
{
    // Return Head of none TCA alarm linked-list.
    return &nonTcaAlarmLL;
}

void omci_alarm_releaseAll(void)
{
    // Release existing frozen alarm records (if any).
    omci_alarm_releaseByHeadList(&frozenAlarmLL);

    // Release existing frozen alarm records (if any).
    omci_alarm_releaseByHeadList(&nonTcaAlarmLL);

    // Release existing ARC alarm records (if any).
    omci_arc_releaseAll();

    omci_alarm_data_reset();
}

#endif    // DMP_X_ITU_ORG_GPON_1
