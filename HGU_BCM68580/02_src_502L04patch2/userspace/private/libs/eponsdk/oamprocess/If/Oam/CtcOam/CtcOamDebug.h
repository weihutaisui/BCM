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

#if !defined(CtcOamDebug_h)
#define CtcOamDebug_h
////////////////////////////////////////////////////////////////////////////////
/// \file CtcOamDebug.h
/// \brief China Telecom debug module
/// \author Jason Armstrong
/// \author Joshua Melcon
/// \date March 2, 2006
///
/// \todo
/// Finish this header.
////////////////////////////////////////////////////////////////////////////////


#include "Teknovus.h"
#include "Oam.h"
#include "CtcOam.h"
#include "Mpcp.h"
#include "TkDebug.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct
    {
    U8 level;
    char description[32];
    } CtcDebugLevel;


#define NumCtcDebugAreas 6
extern CtcDebugLevel ctcDebugAreas[NumCtcDebugAreas];
#define CtcDebugBody(idx,lvl,args) TkDebug(lvl,ctcDebugAreas[idx].level,args)

#define CtcDebugDiscOamFunc(lvl,func) TkDebugFunction(lvl,ctcDebugAreas[0].level,func)
#define CtcDebugDiscOam(lvl,args) CtcDebugBody(0,lvl,args)


#define CtcDebugPostDiscHeartBeatFunc(lvl,func) TkDebugFunction(lvl,ctcDebugAreas[1].level,func)
#define CtcDebugPostDiscHeartBeat(lvl,args) CtcDebugBody(1,lvl,args)

#define CtcDebugPrePostDisc(lvl,args) CtcDebugDiscOam(lvl,args) CtcDebugPostDiscHeartBeat(lvl,args)
#define CtcDebugPrePostDiscFunc(lvl,args) CtcDebugDiscOamFunc(lvl,args) CtcDebugPostDiscHeartBeatFunc(lvl,args)

#define CtcDebugKeyExchange(lvl,args) CtcDebugBody(2,lvl,args)
#define CtcDebugKeyExchangeFunc(lvl,args) TkDebugFunction(lvl,ctcDebugAreas[2].level,args)

#define CtcEncryptFpgaDebugFunc(lvl,func) TkDebugFunction(lvl,ctcDebugAreas[3].level,func)
#define CtcEncryptFpgaDebug(lvl,args) CtcDebugBody(3,lvl,args)

#define CtcDebugOamDbaFunc(lvl,func) TkDebugFunction(lvl,ctcDebugAreas[4].level,func)
#define CtcDebugOamDba(lvl,args) CtcDebugBody(4,lvl,args)


#define CtcDebugHostIfFunc(lvl,func) TkDebugFunction(lvl,ctcDebugAreas[5].level,func)
#define CtcDebugHostIf(lvl,args) CtcDebugBody(5,lvl,args)

////////////////////////////////////////////////////////////////////////////////
/// CtcDebugPrintOui - Print Organizationally Unique Identifier
///
/// This function parses and prints the contents of a Organizationally Unique
/// Identifier.
///
/// \param oui  Organizationally Unique Identifier
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void CtcDebugPrintOui (const IeeeOui BULK *oui, U8 indent);


////////////////////////////////////////////////////////////////////////////////
/// CtcDebugPrintChurningOam: Prints a OamCtcChurningPdu to the CLI in a human
/// readable format.
///
/// \param *msg message to print.
///
/// \return No return value.
////////////////////////////////////////////////////////////////////////////////
void CtcDebugPrintChurningOam(const OamCtcChurningPdu BULK *msg);


////////////////////////////////////////////////////////////////////////////////
/// CtcDebugPrintInfoTvl - Print a CTC information TVL
///
/// This function parses and dumps the contents of a China Telecom Information
/// TLV.
///
/// \param tlv Information TVL
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void CtcDebugPrintInfoTvl (const OamCtcInfoTlvHeader BULK *tlv);


#if defined(__cplusplus)
}
#endif


#endif // End of File CtcOamDebug.h

