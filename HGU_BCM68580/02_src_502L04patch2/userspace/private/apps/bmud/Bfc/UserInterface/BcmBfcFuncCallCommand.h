//****************************************************************************
//
// Copyright (c) 2009 Broadcom Corporation
//
// This program is the proprietary software of Broadcom Corporation and/or
// its licensors, and may only be used, duplicated, modified or distributed
// pursuant to the terms and conditions of a separate, written license
// agreement executed between you and Broadcom (an "Authorized License").
// Except as set forth in an Authorized License, Broadcom grants no license
// (express or implied), right to use, or waiver of any kind with respect to
// the Software, and Broadcom expressly reserves all rights in and to the
// Software and all intellectual property rights therein.  IF YOU HAVE NO
// AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
// AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE
// SOFTWARE.  
//
// Except as expressly set forth in the Authorized License,
//
// 1.     This program, including its structure, sequence and organization,
// constitutes the valuable trade secrets of Broadcom, and you shall use all
// reasonable efforts to protect the confidentiality thereof, and to use this
// information only in connection with your use of Broadcom integrated circuit
// products.
//
// 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
// "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
// OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
// RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL
// IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR
// A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
// ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME
// THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
//
// 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
// OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
// INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY
// RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM
// HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN
// EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
// WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY
// FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
//
//****************************************************************************
//
//  Filename:       BcmBfcFuncCallCommand.h
//  Author:         dpullen
//  Creation Date:  9/25/2009
//
//****************************************************************************

#ifndef BcmBfcFuncCallCommand_H
#define BcmBfcFuncCallCommand_H

//********************** Include Files ***************************************

// My base class.
#include "Command.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

//********************** Class Declaration ***********************************


/** \ingroup BfcConsole
 *
 *   This class implements support for a specific command ("call") which is used
 *   in multiple command tables.  Putting this code in a derived class
 *   eliminates code/text duplication and minimizes debug and maintenance
 *   hassles.
 */
class BcmBfcFuncCallCommand : public BcmCommand
{
public:

    /** Default Constructor.  Initializes the state of the object...
     */
    BcmBfcFuncCallCommand(void);

    /** Destructor.  Frees up any memory/objects allocated, cleans up internal
     *  state.
     */
    virtual ~BcmBfcFuncCallCommand();

private:

    /** This is the entrypoint that is called whenever this command is entered.
    *
    * \param
    *      pInstanceValue - the instance value to which the command should be
    *                       applied.  Instances must be registered with the
    *                       command table in order for them to receive the
    *                       command.
    * \param
    *      command - the command (with parameters) that was entered by the user
    */
    static void CommandHandler(void *pInstanceValue, const BcmCommand &command);

private:

    /** Copy Constructor.  Not supported...
     */
    BcmBfcFuncCallCommand(const BcmBfcFuncCallCommand &otherInstance);

    /** Assignment operator.  Not supported...
     */
    BcmBfcFuncCallCommand & operator = (const BcmBfcFuncCallCommand &otherInstance);

};


//********************** Inline Method Implementations ***********************

#endif


