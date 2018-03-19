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
#include "omci_th_llist.h"

static BCM_COMMON_DECLARE_LL(thresholdLL);


/*========================= PRIVATE FUNCTIONS ==========================*/

//#define THRESHOLD_TEST

static UINT32 threshold_set(
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdEntryPtr,
    UINT16 obj_Type,
    UINT16 obj_ID __attribute__((unused)),
    const void *pData)
{
    UINT32 returnVal = OMCI_PM_STATUS_ERROR;
    ThresholdData1Object* pData1Object = NULL;
    ThresholdData2Object* pData2Object = NULL;

    // Test for valid data.
    if (pData == NULL)
    {
        return returnVal;
    }

    // Test for valid threshold record entry.
    if (thresholdEntryPtr != NULL)
    {
        switch (obj_Type)
        {
            case MDMOID_THRESHOLD_DATA1:
                pData1Object = (ThresholdData1Object *)pData;

                thresholdEntryPtr->thresholdValue1 = pData1Object->thresholdValue1;
                thresholdEntryPtr->thresholdValue2 = pData1Object->thresholdValue2;
                thresholdEntryPtr->thresholdValue3 = pData1Object->thresholdValue3;
                thresholdEntryPtr->thresholdValue4 = pData1Object->thresholdValue4;
                thresholdEntryPtr->thresholdValue5 = pData1Object->thresholdValue5;
                thresholdEntryPtr->thresholdValue6 = pData1Object->thresholdValue6;
                thresholdEntryPtr->thresholdValue7 = pData1Object->thresholdValue7;

                returnVal = OMCI_PM_STATUS_SUCCESS;
                break;
            case MDMOID_THRESHOLD_DATA2:
                pData2Object = (ThresholdData2Object *)pData;

                thresholdEntryPtr->thresholdValue8  = pData2Object->thresholdValue8;
                thresholdEntryPtr->thresholdValue9  = pData2Object->thresholdValue9;
                thresholdEntryPtr->thresholdValue10 = pData2Object->thresholdValue10;
                thresholdEntryPtr->thresholdValue11 = pData2Object->thresholdValue11;
                thresholdEntryPtr->thresholdValue12 = pData2Object->thresholdValue12;
                thresholdEntryPtr->thresholdValue13 = pData2Object->thresholdValue13;
                thresholdEntryPtr->thresholdValue14 = pData2Object->thresholdValue14;

                returnVal = OMCI_PM_STATUS_SUCCESS;
                break;
        }
    }

#ifdef THRESHOLD_TEST
    omci_threshold_dumpAll();
#endif // THRESHOLD_TEST

    // Return OMCI_PM_STATUS_SUCCESS on success, OMCI_PM_STATUS_ERROR on failure.
    return returnVal;
}

/*========================= PUBLIC FUNCTIONS ==========================*/

UINT32 omci_threshold_create(
    UINT16 obj_Type,
    UINT16 obj_ID,
    const void* pData)
{
    UINT32 returnVal = OMCI_PM_STATUS_ERROR;
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdEntryPtr;

    // Attempt to find already created entry.
    thresholdEntryPtr = omci_threshold_get(obj_ID);

    // Test if entry has not yet been created since it is two parts and either part can come first.
    if (thresholdEntryPtr == NULL)
    {
        // Allocate a new threshold entry.
        thresholdEntryPtr = (BCM_OMCIPM_THRESHOLD_ENTRY*)cmsMem_alloc(sizeof(BCM_OMCIPM_THRESHOLD_ENTRY), ALLOC_ZEROIZE);

        // Test for valid threshold entry.
        if (thresholdEntryPtr != NULL)
        {
            // Setup object ID.
            thresholdEntryPtr->objectID = obj_ID;

            // Add new threshold entry to linked-list.
            BCM_COMMON_LL_APPEND(&thresholdLL, thresholdEntryPtr);

            // Signal success.
            returnVal = OMCI_PM_STATUS_SUCCESS;
        }
        else
        {
            // Signal error.
            cmsLog_error("Could not allocate Threshold entry memory.  objType: %d  objID: %d", obj_Type, obj_ID);
            returnVal = OMCI_PM_STATUS_ALLOC_ERROR;
        }
    }

    // Test for valid threshold entry.
    if (thresholdEntryPtr != NULL)
    {
        // Re-assign managed entity's ID to new ID.
        thresholdEntryPtr->objectID = obj_ID;

        // Set threshold values from message's data fields (if any).
        returnVal = threshold_set(thresholdEntryPtr, obj_Type, obj_ID, pData);
    }

#ifdef THRESHOLD_TEST
    omci_threshold_dumpAll();
#endif // THRESHOLD_TEST

    // Return OMCI_PM_STATUS_SUCCESS on success, OMCI_PM_STATUS_ALLOC_ERROR on failure.
    return returnVal;
}

UINT32 omci_threshold_delete(UINT16 obj_ID)
{
    UINT32 returnVal = OMCI_PM_STATUS_ERROR;
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdEntryPtr = NULL;

    // Attempt to find already created entry.
    thresholdEntryPtr = omci_threshold_get(obj_ID);

    // Test for valid threshold entry.
    while (thresholdEntryPtr != NULL)
    {
        // Test for specified threshold entry.
        if (thresholdEntryPtr->objectID == obj_ID)
        {
            // Remove entry from list.
            BCM_COMMON_LL_REMOVE(&thresholdLL, thresholdEntryPtr);

            // Free current entry.
            CMSMEM_FREE_BUF_AND_NULL_PTR(thresholdEntryPtr);

            // Signal success.
            returnVal = OMCI_PM_STATUS_SUCCESS;

            // Done.
            break;
        }

        // Get next threshold entry.
        thresholdEntryPtr = BCM_COMMON_LL_GET_NEXT(thresholdEntryPtr);
    }

#ifdef THRESHOLD_TEST
    omci_threshold_dumpAll();
#endif // THRESHOLD_TEST

    // Return OMCI_PM_STATUS_SUCCESS on success, OMCI_PM_STATUS_ERROR on failure.
    return returnVal;
}

BCM_OMCIPM_THRESHOLD_ENTRY* omci_threshold_get(UINT16 obj_ID)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdEntryPtr;

    // Get 1st threshold entry (if any).
    thresholdEntryPtr = BCM_COMMON_LL_GET_HEAD(thresholdLL);

    // Loop until specified threshold entry is found or end-of-list.
    while (thresholdEntryPtr != NULL)
    {
        // Test for specified threshold entry.
        if (thresholdEntryPtr->objectID == obj_ID)
        {
            // Done, specified threshold entry found.
            break;
        }

        // Get next threshold entry.
        thresholdEntryPtr = BCM_COMMON_LL_GET_NEXT(thresholdEntryPtr);
    }

    // Return valid threshold entry on success, NULL on failure.
    return thresholdEntryPtr;
}

UINT32 omci_threshold_set(
    UINT16 obj_Type,
    UINT16 obj_ID,
    const void* pData)
{
    UINT32 returnVal = OMCI_PM_STATUS_ERROR;
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdEntryPtr;

    // Attempt to find already created entry.
    thresholdEntryPtr = omci_threshold_get(obj_ID);

    // Test if entry has not yet been created since it is two parts and either part can come first.
    if (thresholdEntryPtr != NULL)
    {
        // Re-assign managed entity's ID to new ID.
        thresholdEntryPtr->objectID = obj_ID;

        // Set threshold values from message's data fields.
        returnVal = threshold_set(thresholdEntryPtr, obj_Type, obj_ID, pData);
    }

#ifdef THRESHOLD_TEST
    omci_threshold_dumpAll();
#endif // THRESHOLD_TEST

    // Return OMCI_PM_STATUS_SUCCESS on success, OMCI_PM_STATUS_ERROR on failure.
    return returnVal;
}

void omci_threshold_releaseAll(void)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdEntryPtr;
    BCM_OMCIPM_THRESHOLD_ENTRY* nextThresholdEntryPtr;

    // Get 1st threshold entry (if any).
    thresholdEntryPtr = BCM_COMMON_LL_GET_HEAD(thresholdLL);

    // Loop until all threshold entries are released as defined by end-of-list.
    while (thresholdEntryPtr != NULL)
    {
        // Get list's next entry.
        nextThresholdEntryPtr = BCM_COMMON_LL_GET_NEXT(thresholdEntryPtr);

        // Remove entry from list.
        BCM_COMMON_LL_REMOVE(&thresholdLL, thresholdEntryPtr);

        // Free current entry.
        CMSMEM_FREE_BUF_AND_NULL_PTR(thresholdEntryPtr);

        // Setup list's next entry,
        thresholdEntryPtr = nextThresholdEntryPtr;
    }

    // Re-initialize head and tail to NULL
    BCM_COMMON_LL_INIT(&thresholdLL);
}

BCM_COMMON_LLIST* omci_threshold_getEntryListPtr(void)
{
    // Return OMCIPM threshold master entry linked-list.
    return &thresholdLL;
}

#ifdef THRESHOLD_TEST

void omci_threshold_dump(BCM_OMCIPM_THRESHOLD_ENTRY* thresholdEntryPtr)
{
    printf("*** omci_threshold_dump thresholdEntryPtr: 0x%08X\n", (int)thresholdEntryPtr);

    printf("***              objectID: 0x%04X\n", thresholdEntryPtr->objectID);
    printf("***       thresholdValue1: 0x%08X\n", thresholdEntryPtr->thresholdValue1);
    printf("***       thresholdValue2: 0x%08X\n", thresholdEntryPtr->thresholdValue2);
    printf("***       thresholdValue3: 0x%08X\n", thresholdEntryPtr->thresholdValue3);
    printf("***       thresholdValue4: 0x%08X\n", thresholdEntryPtr->thresholdValue4);
    printf("***       thresholdValue5: 0x%08X\n", thresholdEntryPtr->thresholdValue5);
    printf("***       thresholdValue6: 0x%08X\n", thresholdEntryPtr->thresholdValue6);
    printf("***       thresholdValue7: 0x%08X\n", thresholdEntryPtr->thresholdValue7);
    printf("***       thresholdValue8: 0x%08X\n", thresholdEntryPtr->thresholdValue8);
    printf("***       thresholdValue9: 0x%08X\n", thresholdEntryPtr->thresholdValue9);

    printf("***      thresholdValue10: 0x%08X\n", thresholdEntryPtr->thresholdValue10);
    printf("***      thresholdValue11: 0x%08X\n", thresholdEntryPtr->thresholdValue11);
    printf("***      thresholdValue12: 0x%08X\n", thresholdEntryPtr->thresholdValue12);
    printf("***      thresholdValue13: 0x%08X\n", thresholdEntryPtr->thresholdValue13);
    printf("***      thresholdValue14: 0x%08X\n", thresholdEntryPtr->thresholdValue14);
}


void omci_threshold_dumpAll(void)
{
    BCM_OMCIPM_THRESHOLD_ENTRY* thresholdEntryPtr;

    printf("*** omci_threshold_dumpAll Start ***\n");

    // Get 1st entry (if any).
    thresholdEntryPtr = BCM_COMMON_LL_GET_HEAD(thresholdLL);

    // Loop until all entries are output or end-of-list.
    while (thresholdEntryPtr != NULL)
    {
        // Dump threshold contents.
        omci_threshold_dump(thresholdEntryPtr);

        // Get list's next entry.
        thresholdEntryPtr = BCM_COMMON_LL_GET_NEXT(thresholdEntryPtr);
    }

    printf("*** omci_threshold_dumpAll Done ***\n\n\n\n");
}

#endif // THRESHOLD_TEST

#endif // DMP_X_ITU_ORG_GPON_1
