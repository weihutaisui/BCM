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
#if !defined(CtcEthControl_h)
#define CtcEthControl_h
////////////////////////////////////////////////////////////////////////////////
/// \file CtcEthControl.h
/// \brief China Telecom ether port control module
///
////////////////////////////////////////////////////////////////////////////////
#include "Teknovus.h"


#if defined(__cplusplus)
extern "C" {
#endif
#define FcDebug(lvl, args)    CliDebug(DebugFc, lvl, args)
#define FcDebugLevel          TkDebugLo
#define FcDebugLevelLo        TkDebugStd

#define CtcPortPolicerPrecLo          3
#define CtcPortPolicerPrecHi          2
#define CtcFlowControlPrecHi          12

// Return the bit mask for n queue, one bit per queue. The bit mask starts from
// the lowest bit(bit 0)

#define CtcPortShaperBurstSizeDefault        0x8000


////////////////////////////////////////////////////////////////////////////////
// CTC rate limit Fds Record
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    CtcUpRateFdsRec,
    CtcDnRateFdsRec,
    CtcRateLmtFdsRecNums
    }  CtcRateLmtFdsRec;


typedef struct
    {
    BOOL                enabled;
    U8                  cir[3];
    U8                  cbs[3];
    U8                  ebs[3];
    } PACK CtcEthPortUsPolice;

typedef struct
    {
    BOOL                enabled;
    U8                  cir[3];
    U8                  pir[3];
    } PACK CtcEthPortDsShaper;


////////////////////////////////////////////////////////////////////////////////
/// CtcFCReserveQueueSize  the reserve queue size for ctc flow control function.
///
 // Parameters:
/// \param port     The ethernet port number
///////////////////////////////////////////////////////
/////////////////////////
extern
U16 CtcFCReserveQueueSize(TkOnuEthPort port);



////////////////////////////////////////////////////////////////////////////////
/// CtcFlowControlSupport  check if support ctc flow control function.
///
 // Parameters:
/// \return
/// TRUE if support, otherwise FALSE
///////////////////////////////////////////////////////
/////////////////////////
extern
BOOL CtcFlowControlSupport(void);



////////////////////////////////////////////////////////////////////////////////
/// CtcFlowControlCheck  check if enable ctc flow control function.
///
 // Parameters:
/// \return
/// TRUE if enable flow control, otherwise FALSE
///////////////////////////////////////////////////////
/////////////////////////
extern
BOOL CtcFlowControlCheck(TkOnuEthPort port);



////////////////////////////////////////////////////////////////////////////////
/// CtcFlowControlTimer  detect the flow control work status.
///
 // Parameters:
///
/// \return
///////////////////////////////////////////////////////
/////////////////////////
extern
void CtcFlowControlTimer(void);



////////////////////////////////////////////////////////////////////////////////
/// CtcFlowControlUpdate  update the flow control configuration to UNI port.
///
 // Parameters:
/// \param port     The ethernet port number
/// \param enable  enable/disable pause frame transmit to UNI port.
///
/// \return
/// TRUE if install successfully, otherwise FALSE
///////////////////////////////////////////////////////
/////////////////////////
extern
BOOL CtcFlowControlUpdate(TkOnuEthPort port,BOOL enable);


////////////////////////////////////////////////////////////////////////////////
/// CtcFlowControlCommitConfig - 
///
/// This function configure the port flow control after OAM response
///
 // Parameters:
///         None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcFlowControlCommitConfig (void);



////////////////////////////////////////////////////////////////////////////////
/// CtcEthControlPauseGet  Get the pause transmit ability to UNI port.
///
 // Parameters:
/// \param port     The ethernet port number
/// \param enable  enable/disable pause frame transmit to UNI port.
///
/// \return
/// TRUE if install successfully, otherwise FALSE
////////////////////////////////////////////////////////////////////////////////
extern
BOOL CtcEthControlPauseGet(TkOnuEthPort port,BOOL* enable);


////////////////////////////////////////////////////////////////////////////////
/// CtcEthControlPauseSet  configure the pause transmit ability to UNI port.
///
 // Parameters:
/// \param port     The ethernet port number
/// \param enable  enable/disable pause frame transmit to UNI port.
///
/// \return
/// TRUE if install successfully, otherwise FALSE
////////////////////////////////////////////////////////////////////////////////
extern
BOOL CtcEthControlPauseSet(TkOnuEthPort port,BOOL enable);



////////////////////////////////////////////////////////////////////////////////
/// CtcEthControlUSPoliceCfgSet  Set the current Upsteam policing configuration
///                              for the port.
///
/// Now it is just for OAM opercation, not for real function
///
 // Parameters:
/// \param port    The ethernet port number
/// \param cfg     Rate configuration
///
/// \return
/// TRUE if successfule, otherwise FALSE
////////////////////////////////////////////////////////////////////////////////
extern
BOOL CtcEthControlUsPoliceCfgSet(TkOnuEthPort port,
                      const CtcEthPortUsPolice BULK *cfg);


////////////////////////////////////////////////////////////////////////////////
/// CtcEthControlUsPoliceCfgGet  Get the current Upsteam policing configuration
///                              for the port.
///
 // Parameters:
/// \param port    The ethernet port number
/// \param cfg     Pointer to rate configuration buffer
///
/// \return
/// Size of configuration fetched
////////////////////////////////////////////////////////////////////////////////
extern
U8 CtcEthControlUsPoliceCfgGet(TkOnuEthPort port, CtcEthPortUsPolice BULK *cfg);


////////////////////////////////////////////////////////////////////////////////
/// CtcEthControlDsShaperCfgSet  Set the current downsteam shaper configuration
///                              for the port.
///
 // Parameters:
/// \param port    The ethernet port number
/// \param cfg     Rate configuration
///
/// \return
/// TRUE if successfully, otherwise FALSE
////////////////////////////////////////////////////////////////////////////////
extern
BOOL CtcEthControlDsShaperCfgSet(TkOnuEthPort port,
                      const CtcEthPortDsShaper BULK *cfg);


////////////////////////////////////////////////////////////////////////////////
/// CtcEthControlDsShaperCfgGet  Get the downsteam shaper configuration
///                              for the port.
///
 // Parameters:
/// \param port    The ethernet port number
/// \param cfg     Pointer to rate configuration buffer
///
/// \return
/// Size of configuration fetched
////////////////////////////////////////////////////////////////////////////////
extern
U8 CtcEthControlDsShaperCfgGet(TkOnuEthPort port, CtcEthPortDsShaper BULK *cfg);



////////////////////////////////////////////////////////////////////////////////
/// CtcEthControlDsShaperResume  resume the downsteam shaper configuration
///                              for the port.
///
// Parameters:
/// \param port    The ethernet port number
/// \param cfg     Pointer to rate configuration buffer
///
/// \return
/// Size of configuration fetched
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcEthControlDsShaperResume(void);


////////////////////////////////////////////////////////////////////////////////
/// CtcUsPolicerCommit  save ctc Upstream policer info to FDS
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcUsPolicerCommit(void);


////////////////////////////////////////////////////////////////////////////////
/// CtcDsShaperCommit   save ctc Downstream shaper info to FDS
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcDsShaperCommit(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Register CTC ethernet port control FDS group
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcEthControlFdsGroupReg(void);


////////////////////////////////////////////////////////////////////////////////
/// CtcEthControlInit  Initialize the Ctc Ether Port Control module
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcEthControlInit(void);



#if defined(__cplusplus)
}
#endif

#endif

// End of File CtcEthControl.h

