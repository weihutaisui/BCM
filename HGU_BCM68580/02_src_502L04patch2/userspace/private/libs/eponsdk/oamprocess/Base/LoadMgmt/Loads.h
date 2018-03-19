//
// <:copyright-BRCM:2014:proprietary:epon
// 
//    Copyright (c) 2014 Broadcom 
//    All Rights Reserved
// 
//  This program is the proprietary software of Broadcom and/or its
//  licensors, and may only be used, duplicated, modified or distributed pursuant
//  to the terms and conditions of a separate, written license agreement executed
//  between you and Broadcom (an "Authorized License").  Except as set forth in
//  an Authorized License, Broadcom grants no license (express or implied), right
//  to use, or waiver of any kind with respect to the Software, and Broadcom
//  expressly reserves all rights in and to the Software and all intellectual
//  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
//  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
//  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
// 
//  Except as expressly set forth in the Authorized License,
// 
//  1. This program, including its structure, sequence and organization,
//     constitutes the valuable trade secrets of Broadcom, and you shall use
//     all reasonable efforts to protect the confidentiality thereof, and to
//     use this information only in connection with your use of Broadcom
//     integrated circuit products.
// 
//  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
//     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
//     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
//     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
//     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
//     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
//     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
//     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
//     PERFORMANCE OF THE SOFTWARE.
// 
//  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
//     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
//     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
//     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
//     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
//     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
//     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
//     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
//     LIMITED REMEDY.
// :>
//

#if !defined(Loads_h)
#define Loads_h
////////////////////////////////////////////////////////////////////////////////
/// \file Loads.h
/// \brief
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"


#if defined(__cplusplus)
extern "C" {
#endif

typedef U32 LoadSize;

typedef U8  DataWord;

typedef enum
    {
    OamFileIdle,
    OamFileReadPrep,
    OamFileReading,
    OamFileReadDone,
    OamFileWritePrep,
    OamFileWriting,
    OamFileWriteDone,
    OamFileFilenameSave,
    OamFileStateCount
    } OamFileState;

#define OamFileStateStrings {  \
    "Idle",           \
    "ReadPrep",        \
    "Reading",              \
    "ReadDone",           \
    "WritePrep",            \
    "Writing",        \
    "WriteDone",       \
    "FilenameSave"              \
    }

typedef enum
    {
    LoadLocRunning = 0,
    LoadLocBoot    = 1,
    LoadLocApp0    = 2,
    LoadLocApp1    = 3,
    LoadLocApp2    = 4,
    LoadLocPers    = 5,
    LoadLocPers0   = 6,
    LoadLocPers1   = 7,
    LoadLocNvs0    = 8,
    LoadLocNvs1    = 9,
    LoadLocDiag    = 10,
    LoadLocBuffer  = 11,
    LoadLocEpld    = 12,            // EPLD load
    LoadLocNumLocations
    } LoadLocation;

typedef enum
    {
    LoadTypeUnknown = 0, // Unknown (assumed to be legacy app)
    LoadTypeBoot    = 1, // Boot code
    LoadTypeApp     = 2, // Application code
    LoadTypePers    = 3, // Personality load
    LoadTypeDiag    = 4, // Diagnostic load (used for ONU web server FS)
    LoadTypePersExt = 5, // Personality extension
    LoadTypeEpld    = 6, // EPLD load
    LoadTypeNumTypes
    } LoadType;

typedef union
    {
    //U8 FAR* buffer;
    LoadLocation loc;
    } OamFileInfo;

typedef struct
    {
    U16     year;
    U8      month;
    U8      day;
    U8      hour;
    U8      min;
    U8      sec;
    } PACK Tk2Time;

#if defined(__cplusplus)
}
#endif


#endif // Loads.h


