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


////////////////////////////////////////////////////////////////////////////////
/// \file PON Manager
/// \brief Manages PON specific Configuration and detection of PON status
///
///
////////////////////////////////////////////////////////////////////////////////

// generic includes
#include "Build.h"
#include <stdio.h>
#include "Teknovus.h"
#include "bcm_epon_cfg.h"
#include "OamIeee.h"
#include "EponDevInfo.h"

// base includes
#include "PonManager.h"

#define PonApiShaperRateUnit    2000

LaserTxMode laserTxMode;

//To burst or not to burst that is the question.
//We need to remember the previous burstiness accross
//laser TX power ON / OFF:
LaserTxMode laserTxBurstMode = LaserTxModeBurst;

static LinkIndex currentLink;
static PonMgrRptInfo curRptInfo;

#define PriCount()      PonMgrRptModeToPri(curRptInfo.mode)

static BOOL shaperInUse[EponUpstreamShaperCount];


//Minimum burst cap.  This is what we want to use to minimise
//the delay to MPCP/OAM frame FIFO when attempting to register.
#define DefaultHardCodedBurstCap  0x400


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get reporting mode currently provisioned in HW
///
/// \return Report mode currently setup.
////////////////////////////////////////////////////////////////////////////////
//extern
PonMgrRptMode PonMgrReportModeGet (void)
    {
    return curRptInfo.mode;
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void PonMgrGetMacForLink(LinkIndex link, MacAddr * mac)
    {
    //EponGetMac(link, mac);
    }



////////////////////////////////////////////////////////////////////////////////
/// \brief Applies a new burst cap to a link on the fly
///
/// \param link     Link index for burst cap
/// \param bcap     New burst cap values in a array of 16 bytes values
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void PonMgrSetBurstCap(LinkIndex link, U16 *bcap, U8 count)
    {
    if(bcap != NULL)    
        {
        eponStack_CtCfgBurstCap(EponSetOpe, link, bcap, &count);
        }
    } // PonMgrSetBurstCap


void PonMgrGetBurstCap(LinkIndex link, U16 *bcap, U8 *count)
    {
    if(bcap != NULL && count != NULL)    
        {
        eponStack_CtCfgBurstCap(EponGetOpe, link, bcap, count);
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Add shapers for queue map
///
/// \param The map of port queues to associate with the shaper
/// \param The shaper rate
/// \param The shaper max burst size
///
/// \return
/// A handle to the shaper element or PonShaperBad if failed
////////////////////////////////////////////////////////////////////////////////
//extern
PonShaper PonMgrAddShaper (U32 shapedQueueMap,
                              PonShaperRate rate,
                              PonMaxBurstSize mbs)
    {
    EponShpElement shp;
    for (shp = 0; shp < EponUpstreamShaperCount; ++shp)
        {
        if (!shaperInUse[shp])
            {
            shaperInUse[shp] = TRUE;
            /*
             *int eponStackCtlCfgShaper (EponCtlOpe ope ,U32 *shaperL1Map, U32 *rate,
             *U16 *size,U8 *shp) 
             */
            return (PonShaper)shp;
            }
        }
    return PonShaperBad;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Delete a shaper
///
/// \param Shaper to deactivate
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void PonMgrDelChannelShaper (PonShaper shp)
    {
    if (shp < EponUpstreamShaperCount)
        {
        shaperInUse[shp] = FALSE;
        /*
         * call eponStackCtlCfgShaper(...)
         * */
        }
    }



////////////////////////////////////////////////////////////////////////////////
/// \brief Set the status of the Tx Laser(normal, always on, or disable)
///
/// \param mode New mode to set the laser to
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void PonMgrSetLaserStatus (LaserTxMode mode)
    {
    /*    
     *ioctl-->BOOL OntDirLaserStatusSet(Direction dir, LaserTxMode mode)
    */
    } // PonMgrSetLaserStatus


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the status of the Tx Laser(normal, always on, or disable)
///
/// \param none
///
/// \return
/// status of the Tx Laser
////////////////////////////////////////////////////////////////////////////////
//extern
LaserTxMode PonMgrGetLaserStatus (void)
    {
    return laserTxMode;
    } // PonMgrGetLaserStatus


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the last known burst status of the Tx Laser
///
/// \param none
///
/// \return
/// Burst mode of TX laser
////////////////////////////////////////////////////////////////////////////////
//extern
LaserTxMode PonMgrGetLaserBurstStatus (void)
    {
    return laserTxBurstMode;
    } // PonMgrGetLaserStatus



////////////////////////////////////////////////////////////////////////////////
/// \brief Start definition of a link
///
/// \param link link index of the link to be created
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void PonMgrLinkCmdStart (LinkIndex link)
    {
    currentLink = link;
    } // PonMgrLinkCmdStart


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
//extern
void PonMgrLinkCmdAddQueue(U8 pri, U8 qId, U32 size)
    {
    /*
     *call ioctl --> OntDirLinkQueueSet(LinkIndex link,LinkQueueCnt *dst,U8 count)
     */
    } // PonMgrLinkCmdAddQueue



////////////////////////////////////////////////////////////////////////////////
//extern
void PonMgrSetReporting (PonMgrRptMode rptMode, BOOL force)
    {
    if(!curRptInfo.forced)
		{
	    curRptInfo.mode = rptMode;
		curRptInfo.forced = force;
		}
    }


////////////////////////////////////////////////////////////////////////////////
//extern
BOOL PonMgrIsPmcOlt (void)
    {
    if((EponDevGetSchMode() == PonSchMode3715CompatabilityMode) && 
            (EponDevGetIdleTimeOffset() == 16))
        {
        return TRUE;
        }
    else
        {
        return FALSE;
        }
    }

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
//extern
void PonMgrSyncReportOptions (IeeeOui BULK *vendor)
    {
    if (memcmp (vendor, &PmcOui, sizeof(IeeeOui)) == 0)
        {
        // PMC has some interesting granting behavior that needs to be
        // worked around depending on the chip type.  The best way to pick
        // off the difference is off of the vendor info message which has
        // the chip and version number.
        U16 BULK *chipId = (U16 BULK *)(vendor + 1);
        if(PonMgrIsPmcOlt())
            {
            return;
            }

        switch (*chipId)
            {
            case 0x5001:
            case 0x5201:
                EponDevSetSchMode(PonSchMode3715CompatabilityMode);
                EponDevSetIdleTimeOffset(16);
                system("reboot \n");
                exit(0); 
                break;

            default:
                break;
            }
        }
    else
        {
        if(PonMgrIsPmcOlt())
            {
            EponDevSetSchMode(PonSchModePriorityBased);
            EponDevSetIdleTimeOffset(0);
            system("reboot \n");
            exit(0); 
            }
        }
    } // PonMgrSyncReportOptions


