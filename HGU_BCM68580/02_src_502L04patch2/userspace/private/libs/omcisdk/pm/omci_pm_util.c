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

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#include "owsvc_api.h"

#include "omci_arc_llist.h"
#include "omci_pm_util.h"

#include "bcm_ploam_api.h"
#include "gponctl_api.h"
#include "omci_api.h"
#include "omci_util.h"
#include "rut_gpon.h"


/*========================= PUBLIC FUNCTIONS ==========================*/


void omci_msg_send(
    char* msgTitle __attribute__((unused)),
    omciPacket *pPacket)
{
    int gponFd = 0;
    int txCount = 0;
    int msgLen = OMCI_PACKET_A_SIZE - OMCI_PACKET_MIC_SIZE;

    OMCI_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    // BCMGPON_FILE is defined in omci_api.h
    cmsLog_notice("Opening %s...", BCMGPON_FILE);

    gponFd = open(BCMGPON_FILE, O_WRONLY);
    if (gponFd < 0)
    {
        cmsLog_error("Cannot open gpon file descriptor to write: %s", strerror(errno));
        return;
    }

    /* transmit response packet */
    txCount = write(gponFd, (unsigned char *)pPacket, msgLen);

    cmsLog_notice("Closing %s", BCMGPON_FILE);
    close(gponFd);

    if ((txCount == -1) && (errno == ENOLINK))
    {
        /* Return success: the OMCI Port has been deleted */
        cmsLog_error("OMCI Port has been deleted");
    }
    else if (txCount != msgLen)
    {
        cmsLog_error("Failed to transmit Response Packet (count = %d)",
                     txCount);
    }
}

UINT8 omci_pm_testThreshholdValue(UINT32 thresholdValue, UINT32 testValue)
{
    UINT8 result = FALSE;

    // Test for valid threshold value.
    if ((thresholdValue != 0) && (thresholdValue != MAX_THRESHOLD_VALUE))
    {
        // Test for threshold crossing alert (TCA).
        if (testValue >= thresholdValue)
        {
            // Signal threshold crossing alert (TCA).
            result = TRUE;
        }
    }

    // Return TRUE if threshold value is valid, FALSE if not.
    return result;
}

UINT8 omci_pm_testThreshholdValue_64(UINT64 thresholdValue, UINT64 testValue)
{
    UINT8 result = FALSE;

    // Test for valid threshold value.
    if ((thresholdValue != 0) && (thresholdValue != MAX_THRESHOLD_VALUE))
    {
        // Test for threshold crossing alert (TCA).
        if (testValue >= thresholdValue)
        {
            // Signal threshold crossing alert (TCA).
            result = TRUE;
        }
    }

    // Return TRUE if threshold value is valid, FALSE if not.
    return result;
}

void omci_pm_form64BitValue(UINT64* destPtr, UINT32 hiVal, UINT32 loVal,
  UINT32 validByteCount)
{
    UINT64 dataVal;
    UINT32 bitShiftCount = (8 - validByteCount) * 8;

    // Form 64-bit value.
    dataVal = hiVal;
    dataVal <<= 32;
    dataVal |= loVal;

    // Test for mask.
    if (bitShiftCount > 0)
    {
        // Shift off invalid most-significant bytes.
        dataVal <<= bitShiftCount;

        // Shift remaining bytes into least-significant position.
        dataVal >>= bitShiftCount;
    }

    // Write 64-bit value to destination.
    *destPtr = dataVal;
}

UINT32 omci_pm_calcStatDelta(UINT32 baseValue, UINT32 currentValue)
{
    UINT32 deltaValue;

    // Test for register rollover.
    if (baseValue > currentValue)
    {
        // Calc delta when register rolls over.
        deltaValue = ~baseValue + currentValue + 1;
    }
    else
    {
        // Calc delta when register doesn't roll over.
        deltaValue = currentValue - baseValue;
    }

    // Return delta value accounting for register rollover.
    return deltaValue;
}

UINT64 omci_pm_calcStatDelta_64(UINT64 baseValue, UINT64 currentValue)
{
    UINT64 deltaValue;

    // Test for register wrap.
    if (baseValue > currentValue)
    {
        // Calc delta when register rolls over.
        deltaValue = ~baseValue + currentValue + 1;
    }
    else
    {
        // Calc delta when register doesn't roll over.
        deltaValue = currentValue - baseValue;
    }

    // Return delta value accounting for register rollover.
    return deltaValue;
}

void omci_pm_incStatWithCeiling(UINT32* statPtr, UINT32 statIncVal)
{
    // Test for stat wrap.
    if ((*statPtr + statIncVal) < *statPtr)
    {
        // Setup 0xFFFFFFFF ceiling value on overflow detected.
        *statPtr = MAX_UINT32;
    }
    else
    {
        // Inc non-overflow stat.
        *statPtr += statIncVal;
    }
}

void omci_pm_incStatWithCeiling_64(UINT32* statPtr_H, UINT32* statPtr_L,
  UINT64* statPtr_64, UINT64 statIncVal)
{
    UINT64 statValue = 0;
    UINT32 Hi_32;
    UINT32 Lo_32;

    // Test for valid 64-bit pointer.
    if (statPtr_64 != NULL)
    {
        // Form 64-bit current stat value.
        statValue = *statPtr_64;
    }
    else
    {
        // Test for valid 32-bit pointers.
        if ((statPtr_H != NULL) && (statPtr_L != NULL))
        {
            // Form 64-bit current stat value from two 32-bit pointers.
            statValue = ((UINT64)*statPtr_H << 32) | *statPtr_L;
        }
    }

    // Test for stat wrap.
    if ((statValue + statIncVal) < statValue)
    {
        // Setup 0xFFFFFFFF ceiling value on overflow detected.
        Hi_32 = Lo_32 = MAX_UINT32;

        // Setup 64-bit ceiling value on overflow detected.
        statValue = MAX_UINT64;
    }
    else
    {
        // Inc non-overflow stat.
        statValue += statIncVal;

        // Setup most significant 32-bit value.
        Hi_32 = (UINT32)(statValue >> 32);

        // Setup least significant 32-bit value.
        Lo_32 = (UINT32)(statValue & MAX_UINT32);
    }

    // Test for valid UINT32* for most significant 32-bits.
    if (statPtr_H != NULL)
    {
        // Write most significant 32-bits.
        *statPtr_H = Hi_32;
    }

    // Test for valid UINT32* for least significant 32-bits.
    if (statPtr_L != NULL)
    {
        // Write least significant 32-bits.
        *statPtr_L = Lo_32;
    }

    // Test for valid UINT64* for 64-bit result.
    if (statPtr_64 != NULL)
    {
        // Write 64-bit result.
        *statPtr_64 = statValue;
    }
}

void omci_pm_debugStatOutput(char* hdrStrPtr, UINT32 statRawVal,
  UINT32 statBaseVal, UINT32 statDeltaVal)
{
    printf("%s  Raw: %10d  Base: %10d  Delta: %10d\n",
      hdrStrPtr, statRawVal, statBaseVal, statDeltaVal);
}

void omci_pm_debugOutput_MAC(char* hdrStrPtr, UINT8* currentMacArrayPtr,
  UINT8* newMacArrayPtr)
{
    printf("%s  Current: %02X-%02X-%02X-%02X-%02X-%02X  New: %02X-%02X-%02X-%02X-%02X-%02X\n",
      hdrStrPtr, currentMacArrayPtr[0], currentMacArrayPtr[1], currentMacArrayPtr[2],
      currentMacArrayPtr[3], currentMacArrayPtr[4], currentMacArrayPtr[5],
      newMacArrayPtr[0], newMacArrayPtr[1], newMacArrayPtr[2],
      newMacArrayPtr[3], newMacArrayPtr[4], newMacArrayPtr[5]);
}

void omci_pm_debugStatOutput_64(char* hdrStrPtr, UINT64 statRawVal,
  UINT64 statBaseVal, UINT64 statDeltaVal)
{
    printf("%s  R:%-10llu  B:%-10llu  D:%-10llu\n",
       hdrStrPtr, statRawVal, statBaseVal, statDeltaVal);
}

void omci_pm_dumpArray(char* nameStr, void* inputPtr, int arrayLen)
{
    int offset = 0;
    unsigned char* arrayPtr = (unsigned char*)inputPtr;

    printf("** DUMP  '%s'  arrayPtr: 0x%08X  arrayLen: 0x%04X  time: %d",
      nameStr, (int)arrayPtr, arrayLen, (int)time(NULL));

    // Loop until done.
    while (offset < arrayLen)
    {
        // Test for line start.
        if ((offset & 0x7) == 0)
        {
            // Output pointer & offset.
            printf("\n** 0x%08X (0x%04X) ", (int)arrayPtr, offset);
        }

        // Inc offset.
        offset++;

        // Output data & inc offset.
        printf(" 0x%02X", *arrayPtr++);
    }

    // Output CRLF.
    printf("\n");
}

void omci_pm_dumpArray_Word(char* nameStr, void* inputPtr, int arrayLen)
{
    int offset = 0;
    UINT32* wordPtr = (UINT32*)inputPtr;

    printf("** DUMP  '%s'  arrayPtr: 0x%08X  arrayLen: 0x%04X",
      nameStr, (int)inputPtr, arrayLen);

    // Align to word length.
    arrayLen += 3;
    arrayLen &= ~3;

    // Loop until done.
    while (offset < arrayLen)
    {
        // Test for line start.
        if ((offset & 0xF) == 0)
        {
            // Output pointer & offset.
            printf("\n** 0x%08X (0x%04X) ", (int)wordPtr, offset);
        }

        offset += 4;

        // Output data & inc offset.
        printf(" 0x%08X", *wordPtr++);
    }

    // Output CRLF.
    printf("\n");
}

#endif    // DMP_X_ITU_ORG_GPON_1

