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

//##############################################################################
//                      Multicast Protocol Funtcions
//##############################################################################

#ifndef McastCommon_h
#define McastCommon_h

#include "Build.h"
#include "Teknovus.h"
#include "OntmTimer.h"
#include "McastTypes.h"

#define IgmpV3TimeMinNonFloatValue         0x80     // 128
#define IgmpV3TimeExpBase                  3
#define IgmpV3TimeExpOffset                4
#define IgmpV3TimeExpMask                  0x70
#define IgmpV3TimeMantMask                 0x0F
#define IgmpV3TimeMantBase                 0x10
#define IgmpV3TimeMantMax                  0x1F
#define MldV2TimeMinNonFloatValue          0x8000  // 32768
#define MldV2TimeExpBase                   3
#define MldV2TimeExpOffset                 12
#define MldV3TimeExpMask                   0x7000
#define MldV2TimeMantMask                  0x0FFF
#define MldV2TimeMantBase                  0x1000
#define MldV2TimeMantMax                   0x1FFF


////////////////////////////////////////////////////////////////////////////////
/// \brief  Register multicast FDS group
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtFdsGroupReg(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the multicast module.
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtInit(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Handle the downstream multicast frame
///
/// \param link Link from which the frame is recived
/// \param vid  Vid
/// \param frm  Pointer to the multicast frame
/// \param len  The length of the multicast frame
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDnHandle(LinkIndex link, U16 vid,
                         const McastFrame BULK * frm, U16 len);


////////////////////////////////////////////////////////////////////////////////
/// \brief Handle the upstream multicast frame
///
/// \param port Port from which the frame is recived
/// \param vid  Vid
/// \param frm  Pointer to the multicast frame
/// \param len  The length of the multicast frame
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastUpHandle(TkOnuEthPort port, U16 vid,
                        const McastFrame BULK * frm, U16 len);


////////////////////////////////////////////////////////////////////////////////
/// \brief Handle Timer expiry events
///
/// \param timerId Timer that has expired
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastHandleTimer (OntmTimerId timerId);


////////////////////////////////////////////////////////////////////////////////
/// \brief Handle Port LoS
///
/// \param port Port that encountered LoS
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastHandlePortLoS (Port p);

#endif
