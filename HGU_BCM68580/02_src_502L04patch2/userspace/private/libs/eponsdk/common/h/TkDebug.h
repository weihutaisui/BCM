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


#if !defined(TkDebug_h)
#define TkDebug_h
////////////////////////////////////////////////////////////////////////////////
/// \file TkDebug.h
/// \brief Debug macros
///
/// This module provides a facility for adding debug output, which can
/// be turned on and off at runtime.
///
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

// debug levels range from least debug output (0 == off) through
// increasing levels of verbosity.  Thus, higher priority debug
// output is a lower verbosity number.
#define TkDebugOff			0	
#define TkDebugPkg  		1	// print in/out data frame
#define TkDebugErr  	    2
#define TkDebugTrace  	    3
/* BCA_EPON_SDK_CHANGES_BEGIN */
#define TkDebugInfo  	    4
/* BCA_EPON_SDK_CHANGES_END */
#define TkDebugAll  	    10

#define TkDebugHi			TkDebugErr	
#define TkDebugStd			TkDebugTrace
#define TkDebugLo			TkDebugTrace

/// Sets debug control variable
#define TkDebugSetVar(var, val)  ((var) = (val))
/// Emit debug output
//#define TkDebug(lvl, var, args) TkDebug##lvl (var, args)

extern U8 dbgType;

#define TkSdkDebug(lvl, args) 		if ( ( (lvl==dbgType) || (dbgType == TkDebugAll) ) )  {printf args ;}

/* BCA_EPON_SDK_CHANGES_BEGIN */
#define TkSdkDbg(lvl, args) 		if ( ( (lvl<=dbgType) || (dbgType == TkDebugAll) ) )  {printf args ;}
/* BCA_EPON_SDK_CHANGES_END */


////////////////////////////////////////////////////////////////////////////////
// Implementation - do not call these routines directly
////////////////////////////////////////////////////////////////////////////////
//	#define TkDebugBody(lvl, var, args)	if (var >= lvl)  {printf args ;}
//	
//	
//	#define TkDebug1(var, args)  TkDebugBody(TkDebugHi, var, args)
//	
//	#define TkDebug2(var, args)  TkDebugBody(TkDebugStd, var, args)
//	
//	#define TkDebug3(var, args)  TkDebugBody(TkDebugLo, var, args)

#define TkDbgPrintf(x) 	printf x
#define TkPrint			printf

#define TkDbgLogTrace() TkSdkDebug( TkDebugTrace, ("%s,%s,%d\n",__FILE__,__FUNCTION__,__LINE__) )

//void TkDbgDataDump(U8 *p,U16 len,U16 width);

extern U8 sdkDbgTrace;

void TkSdkSetDebugLvl(U8 lvl);
void DumpRawFrame( void *buf, U32 len );
void DumpEthFrame(void *buf, U32 len);

void TkDbgDataDump(void *p,U16 len,U16 width);



#if defined(__cplusplus)
}
#endif

#endif // TkDebug.h
