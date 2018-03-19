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

#if !defined(MacLearning_h)
#define MacLearning_h
////////////////////////////////////////////////////////////////////////////////
/// \file MacLearning.h
/// \brief Generic interface for MAC address learning configuration
/// \author Jason Armstrong
/// \date January 16, 2007
///
/// This module is a generic interface for configuration of MAC address
/// learning.  Any module implementing this interface needs to implement every
/// function defined within.  Storage of provisioned values is up to the
/// implementor.
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Oam.h"

#define BR_MAX_PORTS	1024

#define BR_SYSFS_CLASS_NET "/sys/class/net/"
#define BR_SYSFS_PATH_MAX	256
#define MacEntryInvalidIndex            0xFFFF
#define MacAddrStrSize  18
#define MAX_MAC_ENTRIES_NUM 1024

////////////////////////////////////////////////////////////////////////////////
///               Stubs to remove learning function not desired
////////////////////////////////////////////////////////////////////////////////

typedef enum
    {
    EntryTypeDynamic,
    EntryTypeStatic
    } EntryType;

typedef enum
    {
    MacLearnDisable,
    MacLearnAuto,
    MacLearnHostCtrl,
    MacLearnNums
    }MacLearnMode;

////////////////////////////////////////////////////////////////////////////////
/// MacLearnCtcSetMacAgingTime - Set Mac Aging TIme
/// \param aging Aging time
/// \return
/// TRUE if index was in range
////////////////////////////////////////////////////////////////////////////////
extern
BOOL MacLearnCtcSetMacAgingTime (U32 aging);


////////////////////////////////////////////////////////////////////////////////
/// MacLearnCtcGetMacAgingTime - Get Mac Aging TIme
/// \return
/// TRUE if index was in range
////////////////////////////////////////////////////////////////////////////////
extern
U32 MacLearnCtcGetMacAgingTime (void);


////////////////////////////////////////////////////////////////////////////////
/// MacLearnSetAgingTime - Set Mac Aging TIme
/// \param exponent to set to ASIC register
/// \return
/// TRUE if index was in range
////////////////////////////////////////////////////////////////////////////////
extern
BOOL MacLearnSetAgeingTime (char *brname, U32 ageingTime);


////////////////////////////////////////////////////////////////////////////////
/// MacLearnGetAgingTime - Get Mac Aging TIme
/// \param exponent to set to ASIC register
/// \return
/// exponent 
////////////////////////////////////////////////////////////////////////////////
extern
BOOL MacLearnGetAgeingTime (char *brname, U32 *ageingTime);


////////////////////////////////////////////////////////////////////////////////
/// MacLearnSetOnuLimit - Set to ONU aggregate MAC limit
///
/// This function sets the ONU aggregate dynamic MAC address limit, that is the
/// number of addresses that can be learned by all ports combined.  For the set
/// to be successful the limit must be less than the ONU dynamic learning table
/// size and greater than or equal to the sum of minimum guarenteed limits for
/// all ports.  A successful set operation will clear the learning table on all
/// ports.
///
 // Parameters:
/// \param limit Total ONU limit
///
/// \return
/// TRUE if operation was successful
////////////////////////////////////////////////////////////////////////////////
extern
BOOL MacLearnSetOnuLimit (char *brname, U16 limit);


////////////////////////////////////////////////////////////////////////////////
/// MacLearnGetOnuLimit - Get to ONU aggregate MAC limit
///
/// This function gets the ONU aggregate dynamic MAC address limit, that is the
/// number of addresses that can be learned by all ports combined.
///
 // Parameters:
/// \param limit Total ONU limit
///
/// \return
/// Aggregate ONU MAC limit
////////////////////////////////////////////////////////////////////////////////
extern
BOOL MacLearnGetOnuLimit (char *brname, U16 *limit);


////////////////////////////////////////////////////////////////////////////////
/// MacLearnTotalMinLimit - Get the sum of minimum guarenteed addresses
///
/// This function returns the total sum of minimum guarenteed address limits
/// for all configured ports.
///
 // Parameters:
///
/// \return
/// Sum of minimum guarenteed address limits
////////////////////////////////////////////////////////////////////////////////
extern
U16 MacLearnTotalMinLimit (void);


////////////////////////////////////////////////////////////////////////////////
/// MacLearnTotalMinLimit - Get the sum of maximum allowed addresses
///
/// This function returns the total sum of maximum allowed address limits for
/// all configured ports.
///
 // Parameters:
///
/// \return
/// Sum of maximum allowed address limits
////////////////////////////////////////////////////////////////////////////////
extern
U16 MacLearnTotalMaxLimit (void);


////////////////////////////////////////////////////////////////////////////////
/// MacLearnSetMinLimit - Set minimum guarenteed limit on a port
///
/// This function sets the minimum guarenteed address limit on a port.  For this
/// operation to be successful the total limit for all ports must be less than
/// or equal to the total ONU limit and the given minimum must be less than or
/// equal to the maximum allowed on this port.  A successful set operation will
/// clear the learning table for this port.
///
 // Parameters:
/// \param port User port number
/// \param min Minimum guarenteed limit
///
/// \return
/// TRUE if operation was successful
////////////////////////////////////////////////////////////////////////////////
extern
BOOL MacLearnSetMinLimit (char *brname, TkOnuEthPort port, U16 min);


////////////////////////////////////////////////////////////////////////////////
/// MacLearnGetMinLimit - Get minimum guarenteed limit on a port
///
/// This function gets the minimum guarenteed address limit on a port.
///
 // Parameters:
/// \param port User port number
///
/// \return
/// Number addresses guarenteed to be learned
////////////////////////////////////////////////////////////////////////////////
extern
BOOL MacLearnGetMinLimit (char *brname, TkOnuEthPort port, U16 *min);


////////////////////////////////////////////////////////////////////////////////
/// MacLearnSetMaxLimit - Set maximum allowed limit on a port
///
/// This function sets the maximum allowed address limit on a port.  For this
/// operation to be successful the limit for  this port must be less than or
/// equal to the total ONU limit and the given minimum must be greater than or
/// equal to the minimum allowed on this port.  A successful set operation
/// will clear the learning table for this port.
///
 // Parameters:
/// \param port User port number
/// \param max limit
///
/// \return
/// TRUE if operation was successful
////////////////////////////////////////////////////////////////////////////////
extern
BOOL MacLearnSetMaxLimit (char *brname, TkOnuEthPort port, U16 max);


////////////////////////////////////////////////////////////////////////////////
/// MacLearnGetMaxLimit - Get maximum allowed limit on a port
///
/// This function gets the maximum allowed address limit on a port.
///
 // Parameters:
/// \param port User port number
/// \param max limit
/// \return
/// TRUE if operation was successful
/// Number addresses allowed to be learned
////////////////////////////////////////////////////////////////////////////////
extern
BOOL MacLearnGetMaxLimit (char *brname, TkOnuEthPort port, U16 *max);

////////////////////////////////////////////////////////////////////////////////
/// MacLearnGetCurLearned - Get the current number of learned addresses
///
/// This function returns the current number of learned addresses on a port.
///
 // Parameters:
/// \param port User port number
///
/// \return
/// Number of MAC addresses that have been learned on this port
////////////////////////////////////////////////////////////////////////////////
extern
U16 MacLearnGetCurLearned (TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// MacLearnGetPortLearn - Get the ONU port learn state
///
/// This function returns the ONU port learn state.
///
 // Parameters:
/// \param port to get
///
/// \return
/// TRUE if port learning is enabled, otherwise FALSE
////////////////////////////////////////////////////////////////////////////////
extern
BOOL MacLearnGetPortLearn (TkOnuEthPort port);

////////////////////////////////////////////////////////////////////////////////
/// MacLearnGetSwitchMode - Get MAC switch mode
///
/// This function gets the MAC switch mode.
///
 // Parameters:
/// \param None
///
/// \return
/// BOOL    switch mode
////////////////////////////////////////////////////////////////////////////////
extern
BOOL MacLearnGetSwitchMode (void);


////////////////////////////////////////////////////////////////////////////////
/// MacLearnSetSwitchMode - Set MAC switch mode
///
/// This function gets the MAC switch mode.
///
 // Parameters:
/// BOOL    switch mode to set
///
/// \return
///none
////////////////////////////////////////////////////////////////////////////////
extern
void MacLearnSetSwitchMode (BOOL mode);

////////////////////////////////////////////////////////////////////////////////
/// MacLearnClearAllDyn - Clear all dynamic entries for a port
///
/// This function deletes all dynamically learned entries for a port.  Static or
/// blocked entries will be left alone.
///
 // Parameters:
/// \param port User port number
///
/// \return
/// Error code
////////////////////////////////////////////////////////////////////////////////
extern
BOOL MacLearnClearAll (const char *bridge, 
                           const char *ifName,
                           BOOL isStatic);


////////////////////////////////////////////////////////////////////////////////
/// \brief Add entry to learning/forwarding tables
///
///
/// \param port
/// \param entry 
/// \param type
///
/// \return
/// TRUE if add successful
////////////////////////////////////////////////////////////////////////////////
extern
BOOL MacLearnAddEntry (const char *bridge, 
                             const char *ifName,
                             const MacAddr *pMac,
                             EntryType et);

////////////////////////////////////////////////////////////////////////////////
/// \brief Delete an entry from the learning table
///
///
/// \param port
/// \param entry 
/// \param type
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void MacLearnDelEntry (const char *bridge, 
                            const char *ifName,
                            const MacAddr *pMac);

////////////////////////////////////////////////////////////////////////////////
/// MacLearnDisableLearning - Disable learning at a hardware level
///
/// This function disables the learning of MAC addresses at a hardware level.
/// This will prevent the hardware to generate violations when unlearned source
/// addresses arrive upstream.
///
 // Parameters:
/// \param port User port number
/// \param modifyFlg is this change from host
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void MacLearnDisableLearning (TkOnuEthPort port, BOOL modifyFlg);

////////////////////////////////////////////////////////////////////////////////
/// MacLearnEnableLearning - Enable learning at a hardware level
///
/// This function enables the learning of MAC addresses at a hardware level.
/// This will allow the hardware to generate violations when unlearned source
/// addresses arrive upstream.
///
 // Parameters:
/// \param port User port number
/// \param modifyFlg is this change from host
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void MacLearnEnableLearning (TkOnuEthPort port, BOOL modifyFlg);


////////////////////////////////////////////////////////////////////////////////
///           Functions needed for TEK / CTC learning related items
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// MacLearnInit - Initialize the MAC learning module
///
/// This function sets up the MAC learning module.
///
 // Parameters:
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void MacLearnInit (void);



////////////////////////////////////////////////////////////////////////////////
/// MacLearnLrnTblGetByType - Get the MAC learning table
///
/// This function return entry in MAC learning table.
///
/// input
/// \param port User port number
/// \param curOffset entry index
/// \param type dynamic or static
///
/// output
/// \param mac mac address
///
/// \return next entry index
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
U16 MacLearnLrnTblGetByType(char *brname,
                            Port port,
                            U16 curOffset, 
                            MacAddr *mac, 
                            EntryType type);


extern
Bool MacLearnSearchSaPort (char *brname, const MacAddr * mac, U8 * port);
#endif /* MacLearning_h */