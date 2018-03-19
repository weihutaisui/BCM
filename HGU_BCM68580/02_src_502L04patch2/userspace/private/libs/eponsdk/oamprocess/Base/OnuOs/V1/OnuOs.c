/*
* <:copyright-BRCM:2012:proprietary:epon
* 
*    Copyright (c) 2012 Broadcom 
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
/// \file OnuOs.c
/// \brief ONU OS misc function implementation
/// \author Jason Armstrong
/// \date November 11, 2010
///
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "Teknovus.h"
#include "OnuOs.h"





//##############################################################################
// Assertion tracking API functions
//##############################################################################
static U64 osAssertDbase[OsAstNums];
static OnuOsAssertHandler osAssertHandle[OsAstNums];


////////////////////////////////////////////////////////////////////////////////
/// \brief Map an assertion id
///
/// This function maps an assertion id to internal store id.
///
/// \param id Assertion to raise
///
/// \return
/// mapped id
////////////////////////////////////////////////////////////////////////////////
U16 OnuOsAssertMap (OnuAssertId id)
    {
    U16 map;

    ////////////////////////////////////////////////////////////////////////////
    /// Os Alarm Assert Id Mapping
    ////////////////////////////////////////////////////////////////////////////
    if (id < OsAstAlmEnd)
        {
        if (id < OsAstAlmOntEnd)
            {
            map = id - OsAstAlmOntBase;
            }
        else if ((id >= OsAstAlmEponBase) && (id < OsAstAlmEponEnd))
            {
            map = (id - OsAstAlmEponBase) + OsAstAlmOntNums;
            }
        else if ((id >= OsAstAlmPortBase) && (id < OsAstAlmPortEnd))
            {
            map = (id - OsAstAlmPortBase) + OsAstAlmOntNums + OsAstAlmEponNums;
            }
        else if ((id >= OsAstAlmLinkBase) && (id < OsAstAlmLinkEnd))
            {
            map = (id - OsAstAlmLinkBase) + OsAstAlmOntNums + OsAstAlmEponNums 
                + OsAstAlmPortNums;
            }
        else if ((id >= OsAstAlmQueueBase) && (id < OsAstAlmQueueEnd))
            {
            map = (id - OsAstAlmQueueBase) + OsAstAlmOntNums + OsAstAlmEponNums 
                + OsAstAlmPortNums + OsAstAlmLinkNums;
            }
        else 
            {
            map = (id - OsAstAlmOtherBase) + OsAstAlmOntNums + OsAstAlmEponNums 
                + OsAstAlmPortNums + OsAstAlmLinkNums + OsAstAlmQueueNums;
            }
        }
    ////////////////////////////////////////////////////////////////////////////
    /// Os Stat Assert Id Mapping
    ////////////////////////////////////////////////////////////////////////////
    else if ((id >= OsAstStatBase) && (id < OsAstStatEnd))
        {
        if ((id >= OsAstStatRxBase) && (id < OsAstStatRxEnd))
            {
            map = (id - OsAstStatRxBase) + OsAstAlmNums;
            }
        else
            {
            map = (id - OsAstStatTxBase) + OsAstAlmNums + OsAstStatRxNums;
            }
        }
    ////////////////////////////////////////////////////////////////////////////
    /// Os Diagnosis Assert Id Mapping
    ////////////////////////////////////////////////////////////////////////////
    else if ((id >= OsAstDiagBase) && (id < OsAstDiagEnd))
        {
        map = (id - OsAstDiagBase) + OsAstAlmNums + OsAstStatNums;
        }
    ////////////////////////////////////////////////////////////////////////////
    /// Os System Assert Id Mapping
    ////////////////////////////////////////////////////////////////////////////
    else if ((id >= OsAstSysBase) && (id < OsAstSysEnd))
        {
        map = (id - OsAstSysBase) + OsAstAlmNums + OsAstStatNums + OsAstDiagNums;
        }
    else
        {
        map = OnuAssertIdInVld;
        }

    return map;
    } // OnuOsAssertMap
static OnuOsAssertHandler BULK osAssertHandleClear[OsAstNums];


////////////////////////////////////////////////////////////////////////////////
/// \brief Set an assertion
///
/// This function sets an assertion state to TRUE.
///
/// \param id Assertion to raise
/// \param inst Instance number
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OnuOsAssertSet (OnuAssertId id, U8 inst)
    {
    U16 FAST idx = OnuOsAssertMap (id);
    U64 FAST old;

    if (idx != OnuAssertIdInVld)
        {
        old = osAssertDbase[idx];

        osAssertDbase[idx] |= (1ULL << inst);
        if ((old != osAssertDbase[idx]) && (osAssertHandle[idx] != NULL))
            {
            (*osAssertHandle[idx])(inst);
            }
        }
    } // OnuOsAssertSet



////////////////////////////////////////////////////////////////////////////////
/// \brief Clean an assertion
///
/// This function sets an assertion state to FALSE.
///
/// \param id Assertion to clear
/// \param inst Instance number
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OnuOsAssertClr (OnuAssertId id, U8 inst)
    {
    U16 FAST idx = OnuOsAssertMap (id);
    U64 FAST old;

    if (idx != OnuAssertIdInVld)
        {
        old = osAssertDbase[idx];

        osAssertDbase[idx] &= ~(1UL << inst);
        if ((old != osAssertDbase[idx]) && (osAssertHandleClear[idx] != NULL))
            {
            (*osAssertHandleClear[idx])(inst);
            }
        }
    } // OnuOsAssertClr



////////////////////////////////////////////////////////////////////////////////
/// \brief Get the current state of an assertion
///
/// This function gets the active state of an assertion.
///
/// \param id Assertion to get
/// \param inst Instance number
///
/// \return
/// TRUE if the assert is active, FALSE if not
////////////////////////////////////////////////////////////////////////////////
BOOL OnuOsAssertGet (OnuAssertId id, U8 inst)
    {
    U16 FAST idx = OnuOsAssertMap (id);

    if (idx != OnuAssertIdInVld)
        {
        return (osAssertDbase[idx] & (1ULL << inst)) != 0;
        }

    return FALSE;
    } // OnuOsAssertGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the current assert value of some id
///
/// This function gets the active state of an assertion.
///
/// \param id Assertion to get
///
/// \return
/// the whole value of this assert
////////////////////////////////////////////////////////////////////////////////
U64 OnuOsAssertDbAll (OnuAssertId id)
    {
    return osAssertDbase[id];
    } // OnuOsAssertDbAll


////////////////////////////////////////////////////////////////////////////////
/// \brief Set a callback handle for assertion
///
/// This function associates a handler function with an assertion.  When an
/// assertion is raised (transitions from clear to set) the handler function
/// will be called.  The instance map is provided to the handler.
///
/// \param id Assertion to get
/// \param handler Callback to register
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OnuOsAssertHandleSet (OnuAssertId id, OnuOsAssertHandler handler)
    {
    U16 FAST idx = OnuOsAssertMap (id);

    if (idx != OnuAssertIdInVld)
        {
        osAssertHandle[idx] = handler;
        }
    } // OnuOsAssertHandleSet



////////////////////////////////////////////////////////////////////////////////
/// \brief Set a callback handle for assertion clear
///
/// This function associates a handler function with an assertion clear.
/// When an assertion is cleared (transitions from set to clear) the handler
/// function will be called.  The instance map is provided to the handler.
///
/// \param id Assertion to get
/// \param handler Callback to register
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OnuOsAssertClearHandleSet (OnuAssertId id, OnuOsAssertHandler handler)
    {
    U16 FAST idx = OnuOsAssertMap (id);

    if (idx != OnuAssertIdInVld)
        {
        osAssertHandleClear[idx] = handler;
        }
    }



////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize assertion tracking database
///
/// This function initializes the assertion tracking database.  The database is
/// initialized with all conditions cleared.
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OnuOsAssertInit (void)
    {
    memset (osAssertDbase, 0, sizeof(osAssertDbase));
    memset (osAssertHandle, 0, sizeof(osAssertHandle));
    memset (osAssertHandleClear, 0, sizeof(osAssertHandleClear));
    } // OnuOsAssertInit




////////////////////////////////////////////////////////////////////////////////
/// \brief Start the operating system
///
/// This functions starts the operating system.  It must be called before using
/// any operating system features.
///
/// \param caching Is data caching enabled
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OnuOsInit (void)
    {
    OnuOsAssertInit ();
    } // OnuOsInit



// End of file OnuOs.c
