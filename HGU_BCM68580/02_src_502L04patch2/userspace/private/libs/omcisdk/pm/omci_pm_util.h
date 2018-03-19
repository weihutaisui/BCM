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

#ifndef __OMCI_PM_UTIL_H__
#define __OMCI_PM_UTIL_H__

/*!\file omci_pm_util.h
 * Util functions for GPON Performance Monitoring functionality.
 *
 */


#include "os_defs.h"
#include "omci_api.h"
#include "omci_pm.h"
#include "omciobj_defs.h"

#define OMCI_DUMP_PACKET(...)                                    \
do                                                               \
{                                                                \
    if (rutGpon_getOmciDebugModule(OMCI_DEBUG_MODULE_OMCI) == 1) \
    {                                                            \
        FILE *fs = rutGpon_getOmciDebugFile();                   \
        if (fs != NULL)                                          \
            omciUtl_dumpPacketToFile(fs, __VA_ARGS__);           \
        else                                                     \
            omciUtl_dumpPacket(__VA_ARGS__);                     \
    }                                                            \
} while (0)


void omci_msg_send(
    char* msgTitle,
    omciPacket *pPacket);

UINT8 omci_pm_testThreshholdValue(
    UINT32 thresholdValue,
    UINT32 testValue);

UINT8 omci_pm_testThreshholdValue_64(
    UINT64 thresholdValue,
    UINT64 testValue);

void omci_pm_form64BitValue(
    UINT64* destPtr,
    UINT32 hiVal,
    UINT32 loVal,
    UINT32 validByteCount);

UINT32 omci_pm_calcStatDelta(
    UINT32 baseValue,
    UINT32 currentValue);

UINT64 omci_pm_calcStatDelta_64(
    UINT64 baseValue,
    UINT64 currentValue);

void omci_pm_incStatWithCeiling(
    UINT32* statPtr,
    UINT32 statIncVal);

void omci_pm_incStatWithCeiling_64(
    UINT32* statPtr_H,
    UINT32* statPtr_L,
    UINT64* statPtr_64,
    UINT64 statIncVal);

void omci_pm_debugStatOutput(
    char* hdrStrPtr,
    UINT32 statRawVal,
    UINT32 statBaseVal,
    UINT32 statDeltaVal);

void omci_pm_debugOutput_MAC(
    char* hdrStrPtr,
    UINT8* currentMacArrayPtr,
    UINT8* newMacArrayPtr);

void omci_pm_debugStatOutput_64(
    char* hdrStrPtr,
    UINT64 statRawVal,
    UINT64 statBaseVal,
    UINT64 statDeltaVal);

void omci_pm_dumpArray(
    char* nameStr,
    void* inputPtr,
    int arrayLen);

void omci_pm_dumpArray_Word(
    char* nameStr,
    void* inputPtr,
    int arrayLen);


#endif  // __OMCI_PM_UTIL_H__
