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
/// \file   OamIeeePhy.c
/// \brief  PHY support for IEEE standard OAM
///
////////////////////////////////////////////////////////////////////////////////


#include "OamIeeePhy.h"
#include "OamIeee.h"
#include "Teknovus.h"
#include "Oam.h"
#include "OamUtil.h"

#include "ApiResLayer.h"
#include "UniConfigDb.h"
#include "ethswctl_api.h"


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get UNI PHY type
///
/// \param port     Port instance
/// \param reply    Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeUniPhyTypeGet(TkOnuEthPort port, OamVarContainer BULK * reply)
    {
    int speed, duplex;

    if (bcm_phy_mode_get(0, bcm_enet_map_oam_idx_to_phys_port(port), &speed, &duplex))
        ContPutU8(reply, OamPhyTypeUnknown);
    else
        {
        if (speed == 1000)
            ContPutU8(reply, (duplex == 0) ? OamPhyType1000T : OamPhyType1000X);
        else if (speed == 100)
            ContPutU8(reply, (duplex == 0) ? OamPhyType100T2 : OamPhyType100X);
        else if (speed == 10)
            ContPutU8(reply, OamPhyType10);
        else
            ContPutU8(reply, OamPhyTypeNone);
        } 
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  Get MAU media available
///
/// \param port     Port instance
/// \param reply    Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeMauMediaAvailGet(TkOnuEthPort port, OamVarContainer BULK * reply)
    {
    int linkstatus;
    bcm_get_linkstatus(0, bcm_enet_map_oam_idx_to_phys_port(port), &linkstatus);
    if (linkstatus == STATUS_ENABLED)
        {
        ContPutU8(reply, OamMauMediaAvailAvailable);
        }
    else
        {
        ContPutU8(reply, OamMauMediaAvailNotAvailable);
        }
    }

// end of OamIeeePhy.c
