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


#if !defined(PonManager_h)
#define PonManager_h
////////////////////////////////////////////////////////////////////////////////
/// \file PonManager.h
/// \brief Manages PON specific Configuration and detection of PON status
///
///
////////////////////////////////////////////////////////////////////////////////

#include "Laser.h"
#include "Oam.h"
#include "eponctl_api.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define PonMgrOptToEpnOptMsk 0xC0


#define PriorityMask 0xF
#define PonMgrRptModeToPri(rptMode)     ((rptMode) & PriorityMask)

//Shaper types
typedef U8  PonShaper;
typedef U64 PonShaperRate;
typedef U16 PonMaxBurstSize;
//The value representing an unused shaper.
#define PonShaperBad                              0xFF

typedef struct
    {
    PonMgrRptMode mode;
    BOOL          forced;
    BOOL 		  byTkOltPrefer;
    } PACK PonMgrRptInfo;

//The personality and firmware are being tricky here.
//We want a SharedL2mode implemented but we do NOT want to
//expose it to the end users.  If you want to test SharedL2 mode
//Use a command line or non-pers editor formatted record to do so.
typedef enum
    {
    PonSchModePriorityBased = 0,
    PonSchMode3715CompatabilityMode = 1,
    PonSchModeSharedL2Mode = 2,
    } PACK PonSchMode;


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get reporting mode currently provisioned in HW
///
/// \return Report mode currently setup.
////////////////////////////////////////////////////////////////////////////////
extern
PonMgrRptMode PonMgrReportModeGet (void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get number of priority in the reporting mode
///
/// \return How many priority
////////////////////////////////////////////////////////////////////////////////
#define PonMgrPriCountGet()     PonMgrRptModeToPri(PonMgrReportModeGet())


////////////////////////////////////////////////////////////////////////////////
/// \brief  Retrieve the MAC address of a link
///
/// \param  link    Index of the link
/// \param  mac     Where to store the MAC
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void PonMgrGetMacForLink(LinkIndex link, MacAddr * mac);


////////////////////////////////////////////////////////////////////////////////
/// \brief Applies a new burst cap to a link on the fly
///
/// \param link     Link index for burst cap
/// \param bcap     New burst cap values in a array of 16 bytes values
/// \param count    Burst cap array item count
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void PonMgrSetBurstCap(LinkIndex link, U16 * bcap, U8 count);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get burst cap to a link on the fly
///
/// \param link     Link index for burst cap
/// \param bcap   place to store burst cap
/// \count           
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void PonMgrGetBurstCap(LinkIndex link, U16 *bcap, U8 *count);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Add shapers for queue map
///
/// \param  shapedQueueMap  The map of port queues to associate with the shaper
/// \param  rate            The shaper rate
/// \param  mbs             The shaper max burst size
///
/// \return A handle to the shaper element or PonShaperBad if failed
////////////////////////////////////////////////////////////////////////////////
extern
PonShaper PonMgrAddShaper (U32 shapedQueueMap,
                              PonShaperRate rate,
                              PonMaxBurstSize mbs);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Delete a shaper
///
/// \param  shp     Shaper to deactivate
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void PonMgrDelChannelShaper (PonShaper shp);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Return a PhyLlid associated to a link index
///
/// \param link     Logical link number
///
/// \return
/// Physical Llid
////////////////////////////////////////////////////////////////////////////////
extern
PhyLlid PonMgrGetPhyLlid (LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the status of the Tx Laser(normal, always on, or disable)
///
/// \param mode New mode to set the laser to
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void PonMgrSetLaserStatus (LaserTxMode mode);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the status of the Tx Laser(normal, always on, or disable)
///
/// \param none
///
/// \return
/// status of the Tx Laser
////////////////////////////////////////////////////////////////////////////////
extern
LaserTxMode PonMgrGetLaserStatus (void);



////////////////////////////////////////////////////////////////////////////////
/// \brief Get the last known burst status of the Tx Laser
///
/// \param none
///
/// \return
/// Burst mode of TX laser
////////////////////////////////////////////////////////////////////////////////
extern
LaserTxMode PonMgrGetLaserBurstStatus (void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Start definition of a link
///
/// \param link link index of the link to be created
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void PonMgrLinkCmdStart (LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Add a queue to the current link being configured
///
/// \param pri  Priority to add queue to
/// \param qId  Absolute queue number within EPON chanel
/// \param size How many frame the queue can hold
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void PonMgrLinkCmdAddQueue (U8 pri, U8 qId, U32 size);



////////////////////////////////////////////////////////////////////////////////
/// \brief Setup PON reporting mode
///
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void PonMgrSetReporting (PonMgrRptMode rptMode, BOOL force);


////////////////////////////////////////////////////////////////////////////////
extern
BOOL PonMgrIsPmcOlt (void);


////////////////////////////////////////////////////////////////////////////////
/// PonMgrSyncReportOptions - Set options for different vendors
///
/// This function sets extension options for different vendors.  It differs
/// slightly from the report configuration function in that options are
/// cumalative, not overriding.
///
 // Parameters:
/// \param vendor Vendor/carrier OUI to sync
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void PonMgrSyncReportOptions (IeeeOui BULK *vendor);


#if defined(__cplusplus)
}
#endif

#endif // PonManager.h
