/*
* <:copyright-BRCM:2013:proprietary:epon
* 
*    Copyright (c) 2013 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :> 
*/

////////////////////////////////////////////////////////////////////////////////
/// \file AggregateNumbers.c
/// \author J. Feinstein
/// \date February 2012
/// \brief Implementation of the Aggregate Numbers feature (for 2000 App Code)
///
/// Provides a "number" abstraction to groups of GPIO pins, allowing
/// read/write access

#include "Build.h"
#include "Teknovus.h"
#include "AggregateNumbers.h"
#include <stdio.h>
#include "Cli.h"
#include "Fds.h"
#include "Mpio.h"
#include "Gpio.h"
#include "OnuRegUtil.h"
#include "PbiRegs.h"

static
BOOL AggregateNumberFind(const U8* anStart, U8 anSize, U8 anIndex, U8* numBits,
    U8* startPos, AggregateNumberOperation operation)
    {
    U8 index = 0;
    BOOL success = TRUE;
    U8 offset = 0;

    if (anStart[offset++] != 0)
        {
        printf("Bad AN!\n");
        success = FALSE;
        }
    else {
        while ((offset < anSize) && (index != anIndex))
            {
            offset += 1 + anStart[offset]; // skip this entry
            index++;
            }

        success = success && (((offset < anSize) && (index == anIndex) && (anStart[offset] != 0)));
        if (success)
            { // found the requested AN
            *numBits = anStart[offset++];
            *startPos = offset;
            // now verify that all bits are set for read/write and GPIO as appropriate
            for (index=0; success && (index<(*numBits)); ++index)
                {
                U8 gpio = anStart[offset + index]; // number of the gpio pin
                success = success && (gpio <= LastAggregateNumberGpio);

                // fail if GPIO Bit Direction doesn't match requested operation (0=read)
                success = success && (operation == (AggregateNumberOperation)GpioPinGetDir((GpioPin)gpio));

                // fail if MPIO Mux not configured for GPIO
                success = success && MpioIsGpio(gpio);
                }
            }
        }
    return success;
    }


////////////////////////////////////////////////////////////////////////////////
//extern
BOOL AggregateNumberRead(const U8* anStart, U8 anSize, U8 anIndex, U8* result)
    {
    U8 numBits;
    U8 offset;
    U8 index;
    BOOL success = AggregateNumberFind(anStart, anSize, anIndex, &numBits, &offset, AggregateNumberReadOp);

    *result = 0;
    if (success)
        {
        for (index=0; index<numBits; ++index)
            {
            U8 bitNo = (numBits-(index+1));
            U32 bitVal = (U32)GpioPinGetByPin((GpioPin)(anStart[offset+index]));
            *result |= (U8)(bitVal << bitNo);
            }
        }
    return success;
    }


////////////////////////////////////////////////////////////////////////////////
//extern
BOOL AggregateNumberWrite(const U8* anStart, U8 anSize, U8 anIndex, U8 wrData)
    {
    U8 numBits;
    U8 offset;
    U8 index;
    BOOL success = AggregateNumberFind(anStart, anSize, anIndex, &numBits, &offset, AggregateNumberWriteOp);

    if (success)
        {
        for (index=0; index<numBits; ++index)
            {
            U8 bitNo = (numBits-(index+1));
            GpioPinSetByPin((GpioPin)(anStart[offset+index]), (0 != (wrData >> bitNo)));
            }
        }
    return success;
    }

