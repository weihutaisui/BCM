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

#if !defined(CtcOnuUtils_h)
#define CtcOnuUtils_h
////////////////////////////////////////////////////////////////////////////////
/// \file CtcOnuUtils.c
/// \brief China Telecom utility functions
/// \author Jason Armstrong
/// \author Joshua Melcon
/// \date March 2, 2006
///
/// \todo
/// Finish this header.
////////////////////////////////////////////////////////////////////////////////
#include "Teknovus.h"
#include "Build.h"
#include "Oui.h"
#include "Oam.h"
#include "OamUtil.h"
#include "CtcOam.h"



//##############################################################################
//    Ctc Onu misc information
//##############################################################################




#if OAM_SWITCH
#define CtcNumEthPorts      SwitchUserPortCount
#else
#define CtcNumEthPorts      1 //TODO: Update for real #define value
                              // this may need to be provisionable
#endif



//##############################################################################
//                     Ctc Onu utility functions
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// OamCtcEthToPort - Convert ethernet port number
///
/// This function converts a zero indexed ethernet port number to an absolute
/// index including the EPON port.
///
 // Parameters:
/// \param port     Ethernet port number
///
/// \return
/// Absolute port index
////////////////////////////////////////////////////////////////////////////////
#define OamCtcEthToPort(port)       ((port) + 1U)



////////////////////////////////////////////////////////////////////////////////
/// OamCtcEthToPort - Convert OAM port number to ethernet port number
///
/// This function converts a CTC OAM port number to a zero indexed ethernet port
/// number that excludes the EPON port.
///
 // Parameters:
/// \param port     Ethernet port number
///
/// \return
/// Absolute port index
////////////////////////////////////////////////////////////////////////////////
#define OamCtcPortToEth(port)       ((port) - 1U)


////////////////////////////////////////////////////////////////////////////////
/// OamCtcNextVarRequest - Get the next variable request
///
/// This function returns the next variable descriptor.  It may not be nessasay
/// however one workaround for the lack of object identification involves
/// sending a port number as part of the variable request.  It is better to have
/// this function in place in case that happens.
///
 // Parameters:
/// \param src      Variable descriptor
///
/// \return
/// Pointer to the next variable descriptor
////////////////////////////////////////////////////////////////////////////////
extern
OamVarDesc BULK *OamCtcNextVarRequest (OamVarDesc BULK* src);


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetExtCount - Get the number of extended OUIs in an info TLV
///
/// \param tlv TLV to check
///
/// \return Number of extended entries to process
////////////////////////////////////////////////////////////////////////////////
extern
U8 OamCtcInfoTlvGetExtCount (const OamCtcInfoTlvHeader BULK* tlv);


////////////////////////////////////////////////////////////////////////////////
/// OamCtcNextCont - Setup a next container
///
/// This function starts up a container with the same branch and leaf codes and
/// following the supplied context.  This should be used in variable responses
/// where more than one TLV must be returned from the lowest level handler
/// function.
///
 // Parameters:
/// \param cont Current OAM variable container
///
/// \return
/// Pointer to next container that was created
////////////////////////////////////////////////////////////////////////////////
extern
OamVarContainer BULK *OamCtcNextCont (const OamVarContainer BULK *cont);


////////////////////////////////////////////////////////////////////////////////
/// \brief  This function terminates a response set
///
/// This function sets the terminal container value and returns a pointer to
/// the end of the final container
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcTerminateBranch(void);


//##############################################################################
// CTC OAM Object Process Utilities
//##############################################################################

typedef U8 OamCtcObj;


////////////////////////////////////////////////////////////////////////////////
/// \brief  Is the current object the all ports object?
///
/// \return TRUE if the current object is the all ports object
////////////////////////////////////////////////////////////////////////////////
extern
BOOL OamCtcIsObjAllPorts(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Reset the context for the following TLVs to default
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcClearObject(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the context for the following TLVs
///
/// This function sets the current object instance ID from a object instance
/// TLV.  The reply container is also set.  Calling this function with NULL
/// parameters resets the current instance to the default which is the EPON
/// port.
///
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcSetObject (const OamVarContainer BULK *src,
                      OamVarContainer BULK *cont);



////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetObject - Get the current object ID
///
/// This function returns an object identification based on the last object
/// instance TLV that was received.  The returned object ID may not be required
/// for all messages.
///
 // Parameters:
/// \param src      Variable descriptor
///
/// \return
/// Pointer to corresponding object ID
////////////////////////////////////////////////////////////////////////////////
extern
OamCtcObject BULK * OamCtcGetObject (OamVarDesc BULK* src);



////////////////////////////////////////////////////////////////////////////////
/// OamCtcCreateObject - Create a object instance TLV
///
/// This function creates an object identification based on the CTC
/// standard version,
/// If the CTC standard is CTC2.1, creates the an object identificatio in
/// new management
/// format, else, creates the an object identificatio in old management format.
///
/// Parameters:
/// \param cont     pointer to var containers in reply
/// \param type     object type
/// \param obj      port number
///
/// \return
/// Pointer to the next container
///////////////////////////////////////////////////////////////////////////////
extern
OamVarContainer BULK * OamCtcCreateObject (OamVarContainer BULK *cont,
                                           OamCtcObjType type,
                                           OamCtcObj obj);

////////////////////////////////////////////////////////////////////////////////
/// CtcStringSwap:  Ctc string swap
///
 // Parameters:
/// \param str     string for swap
/// \param len     string max length
///
/// \return
///         swapped string address
////////////////////////////////////////////////////////////////////////////////
extern
U8 *CtcStringSwap(const U8 *str, U8 len);

#endif // End of File CtcOnuUtils.h
