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

#if !defined(Sff8472_h)
#define Sff8472_h
////////////////////////////////////////////////////////////////////////////////
/// \file Sff8472ModuleIf.h
/// \brief Sff8472 interface
/// \author Chen Lingyong
/// \date June 8, 2009
///
/// This module supports optical power monitor as defined in SFF-8472 Rev9.5
/// criterion.  Power levels can be read from supported lasers and high/low
/// alarm thresholds may be set.  
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"

#if defined(__cplusplus)
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
// Sff8472 Device Address
////////////////////////////////////////////////////////////////////////////////
#define Sff8472DevAddr		0x51


////////////////////////////////////////////////////////////////////////////////
// Sff8472 Register Address
////////////////////////////////////////////////////////////////////////////////
typedef enum
	{
	// Alarm and Warn threshold.
	Sff8472TempAlmHi 	= 0x00,
	Sff8472TempAlmLo 	= 0x02,
	Sff8472TempWrnHi 	= 0x04,
	Sff8472TempWrnLo 	= 0x06,
	Sff8472VccAlmHi 	= 0x08,
	Sff8472VccAlmLo		= 0x0A,
	Sff8472VccWrnHi		= 0x0C,
	Sff8472VccWrnLo		= 0x0E,
	Sff8472TxBiasAlmHi	= 0x10,
	Sff8472TxBiasAlmLo	= 0x12,
	Sff8472TxBiasWrnHi	= 0x14,
	Sff8472TxBiasWrnLo	= 0x16,
	Sff8472TxPowerAlmHi	= 0x18,
	Sff8472TxPowerAlmLo	= 0x1A,
	Sff8472TxPowerWrnHi	= 0x1C,
	Sff8472TxPowerWrnLo	= 0x1E,
	Sff8472RxPowerAlmHi	= 0x20,
	Sff8472RxPowerAlmLo	= 0x22,
	Sff8472RxPowerWrnHi	= 0x24,
	Sff8472RxPowerWrnLo	= 0x26,

	//external calibration Address. 
	RxPowerCalib4 		= 0x38,
	RxPowerCalib3		= 0x3C,
	RxPowerCalib2		= 0x40,
	RxPowerCalib1		= 0x44,
	RxPowerCalib0 		= 0x48,
	TxBiasSlope			= 0x4C,
	TxBiasOffset		= 0x4E,
	TxPowerSlope		= 0x50,
	TxPowerOffset 		= 0x52,
	TxTempSlope			= 0x54,
	TxTempOffset 		= 0x56,
	TxVccSlope			= 0x58,
	TxVccOffset 		= 0x5A,
	
	// Measure Value.
	Sff8472Temp 		= 0x60,
	Sff8472Vcc			= 0x62,
	Sff8472TxBias		= 0x64,
	Sff8472TxPower		= 0x66,
	Sff8472RxPower 		= 0x68,

	Sff8472Force8		= 0x7F
	}Sff8472RegAddr;


#if defined(__cplusplus)
}
#endif

#endif // Sff8472.h

