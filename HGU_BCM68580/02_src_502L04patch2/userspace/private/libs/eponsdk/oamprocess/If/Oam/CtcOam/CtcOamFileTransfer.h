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


#ifndef CtcOamFileTransfer_h
#define CtcOamFileTransfer_h
////////////////////////////////////////////////////////////////////////////////
/// \file CtcOamFileTransfer.h
/// \brief Ctc2.1 firmware upgrade based TFTP and Oam
/// \new feature
///
/// Added by nking
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Platform.h"
#include "Oam.h"

#include "bcm_imgutil_api.h"

#if defined(__cplusplus)
extern "C" {
#endif
// the Maximum bytes in the transmission package
#define CtcFileMaxPacketSize 1400
// the flag of activating and commiting mesage
#define ActComFlags             0x00
#define FwUpgradeOamCreditsRate         10
// Temp value, may change in future

/////////////////////////////////////////////////////////
///
///  CTC2.1 firmware upgrade operation  code
///
/////////////////////////////////////////////////////////
typedef enum
    {
    CtcOpFileWriteReq     = 2,
    CtcOpFileSendData,
    CtcOpFileSendAck,
    CtcOpFileError,
    CtcOpEndDnldReq,
    CtcOpEndDnldRes,

    CtcOpActImgReq,
    CtcOpActImgRes,

    CtcOpCmtImgReq,
    CtcOpCmtImgRes,

    CtcOpForceToU16  = 0x7fff
    } CtcFwUpgradeOpcode;

/////////////////////////////////////////////////////////
///
///  CTC2.1 firmware upgrade error  code
///
/////////////////////////////////////////////////////////
typedef enum
    {
    CtcErrUndefined,
    CtcErrFileNotFound,
    CtcErrAccessViolation,
    CtcErrDiskFull,
    CtcErrIllegalOp,
    CtcErrUnknownTid,
    CtcErrFileExists,
    CtcErrNoSuchUser,
    ctcErrCodeNums,

    CtcErrNoError,         // non-standard; internal use only

    CtcErrForceToU16 = 0x7fff
    } CtcFwUpgradeErr;

/////////////////////////////////////////////////////////
///
///  the response code for End Download request
///
/////////////////////////////////////////////////////////
typedef enum
    {
    CtcResEndDnldNoErr,
    CtcResEndDnldBusy,
    CtcResEndDnldCrcError,
    CtcResEndDnldParaError,
    CtcResEndDnldUnsupport,

    CtcResEndDnldNums
    }CtcEndDnldRes;

/////////////////////////////////////////////////////////
///
///  the response code for activating and comiting message
///
/////////////////////////////////////////////////////////
typedef enum
    {
    CtcAckActComOk,
    CtcAckActComParaError,
    CtcAckActComUnsupport,
    CtcAckActComLoadError,

    CtcAckActComNums
    }CtcActComAck;

/////////////////////////////////////////////////////////
///
///  Data Type of the upgrade package
///
/////////////////////////////////////////////////////////
typedef enum
    {
    CtcTftpProData          =1,
    CtcCheckFileData,
    CtcLoadRunImageData,
    CtcCommitImageData,

    CtcNumDataTypes
    }CtcPayloadType;


/////////////////////////////////////////////////////////
///
///  Payload structure
///
/////////////////////////////////////////////////////////
typedef struct
    {
    U8                  dataType;    // enum CtcPayloadType
    U16                 length;
    U16                 tid;
    } PACK OamCtcPayloadHead;

/////////////////////////////////////////////////////////
///
///  File Write Request structure
///
/////////////////////////////////////////////////////////
typedef struct
    {
    U16   opcode; // enum CtcFwUpgradeOpcode
    char  str[1];
    } PACK CtcFwUpgradeFileRequest;

/////////////////////////////////////////////////////////
///
///  File Transfer Data Header structure
///
/////////////////////////////////////////////////////////
typedef struct
    {
    U16 opcode;  // enum CtcFwUpgradeOpcode
    U16 num;
    } PACK CtcFwUpgradeFileData;

/////////////////////////////////////////////////////////
///
///  File Transfer ACK structure
///
/////////////////////////////////////////////////////////
typedef struct
    {
    U16 opcode;  // enum CtcFwUpgradeOpcode
    U16 num;
    } PACK CtcFwUpgradeFileAck;

/////////////////////////////////////////////////////////
///
///  File Transfer Error structure
///
/////////////////////////////////////////////////////////
typedef struct
    {
    U16      opcode;   // enum CtcFwUpgradeOpcode
    U16      errcode;  // enum CtcFwUpgradeErr
    char     errMsg[1];
    } PACK CtcFwUpgradeFileError;

/////////////////////////////////////////////////////////
///
///  End Download Request structure
///
/////////////////////////////////////////////////////////
typedef struct
    {
    U16 opcode;  // enum CtcFwUpgradeOpcode
    U32 fileSize;
    } PACK CtcFwUpgradeEndDnld;

/////////////////////////////////////////////////////////
///
///  Response package structure for End Download Request,
///              Activating, and Commiting message
/////////////////////////////////////////////////////////
typedef struct
    {
    U16      opcode;  // enum CtcFwUpgradeOpcode
    U8       para;
    } PACK CtcFwUpgradeComm;


#define CtcFwUpgradeMaxWait     300 // 30s

/////////////////////////////////////////////////////////
///
///  File Download State
///
/////////////////////////////////////////////////////////
typedef enum
    {
    CtcUpgradeIdle,
    CtcUpgradeWritePrep,
    CtcUpgradeWriting,
    CtcUpgradeWriteCopy
    } CtcUpgradeState;


typedef struct
    {
    CtcUpgradeState     state;
    U16                 blockNum;
    CtcFwUpgradeErr     lastErr;
    LoadSize            size;
    LoadSize            maxSize;
    U16                 timetick;
    IMGUTIL_HANDLE      imgifHandle;
    }CtcFwUpgradeSessionInfo;


////////////////////////////////////////////////////////////////////////////////
/// \brief  The file tranfer timer process
///
/// \return:
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcOamFile100MsTimer(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle a CTC FileUpgrade PDUs
///
/// \param pdu  Pointer to received CTC FileUpgrade PDU
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcHandleFileUpgrade(OamCtcPayloadHead BULK *pdu);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Run CTC firmware upgrade state machine
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcFwUpgradePoll(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Initialize file transfer internals
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcFwUpgradeInit (void);


#if defined(__cplusplus)
}
#endif

#endif // end CtcOamFileTransfer.h
