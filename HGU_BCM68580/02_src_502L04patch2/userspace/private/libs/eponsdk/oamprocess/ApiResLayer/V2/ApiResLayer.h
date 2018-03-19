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
/// \file   ApiResLayer.h
/// \brief  API resolution layer for aliasing differently named functions
///
/// API resolutions for 10G ONU
///
////////////////////////////////////////////////////////////////////////////////


#if !defined(ApiResLayer_h)
#define ApiResLayer_h


#include "Teknovus.h"
#include "OamIeee.h"

/*******************************************************************************
 * janeqiu: to be implemented **************************************************
 * *****************************************************************************/
#define Rand32()    0
#define Rand16()    0

#define UniMgrAutoNegEnable(port)       //UniMgrApplyAnState(port, TRUE)
#define UniMgrAutoNegDisable(port)      //UniMgrApplyAnState(port, FALSE)

#define UniMgrAutoNegCapsGet(port)      0 //UniCfgDbGetAnModeCaps(port)
#define UniMgrPhysicalCapsGet(port)     0 //UniCfgDbGetPhysicalCapabilities(port)

#define UniMgrPortIsEnabled(port)       //(UniCfgDbGetPortMode(port) == Enabled)

#define UniMgrPortEnable(port)          //OntDirApplyPortMode(port, Enabled)
#define UniMgrPortDisable(port)         //OntDirApplyPortMode(port, Disabled)

#define UniMgrPortIsFullDuplex(port)    
     //TestBitsSet(UniCfgDbFindCurrentCaps(port), EthPortFullDuplex)

#define UniMgrDuplexSetFull(port)       //UniMgrApplyCap(port, EthPortFullDuplex)
#define UniMgrDuplexSetHalf(port)       //UniMgrApplyCap(port, EthPortHalfDuplex)

#define UniMgrIsFlowCtrlEnabled(port)   
    //TestBitsSet(UniCfgDbGetForceModeCaps(port), EthPortFlowControl)

#define UniMgrFlowCtrlEnable(port)      //UniMgrApplyCap(port, EthPortFlowControl)
#define UniMgrFlowCtrlDisable(port)     //UniMgrApplyCap(port, EthPortFlowControl)

#define UniMgrMediaAvail(port)          TRUE //UniMgrFindPortStatus(port)

#define UniMgrIsLenErrDisEnabled(port)    TRUE  //UniCfgDbGetLenError(port)

#define UniMgrLenErrDisApply(port, enable)  
    //UniMgrApplyLengthErrorDiscard(port, enable)

#define UserPortIsValid(port)   TRUE //OntDirValidateUserPort(port)

#define PortIsValid(port)               TRUE  //OntDirValidatePort(port)
#define PortQueueIsValid(port, queue)   TRUE  //OntDirValidatePortQueue((port), (queue))
#define LinkQueueIsValid(link, queue)   TRUE  //OntDirValidateLinkQueue((link), (queue))

#define UniMgrLoopbackEnable(port, loc)     
    //OntDirEnableLoopback(port, (UniLpbkType)(PhyLpbk + (loc)))

#define UniMgrLoopbackDisable(port)     
    //OntDirDisableLoopback(port)

#define GpioAlarmSize       sizeof(OamEventTkGpioAlarm10G)

#define TkOnuQUpIncrement   0 //FifCmdQueueSizeKbytesPerInc
#define TkOnuQDnIncrement   0 //FifCmdQueueSizeKbytesPerInc

#define GetCurrentMaxLinks()     0 //PonMgrGetCurrentMaxLinks()
#define GetNumUpQueues()         0
#define GetMaxUpQueuesPerLink()  0
#define GetNumDnQueues()         0
#define GetMaxDnQueuesPerLink()  0
#define GetUpBuffer()            0
#define GetDnBuffer()            0

#define RsaKeySize 256
// RSA key size in bytes

#endif // ApiResLayer.h
