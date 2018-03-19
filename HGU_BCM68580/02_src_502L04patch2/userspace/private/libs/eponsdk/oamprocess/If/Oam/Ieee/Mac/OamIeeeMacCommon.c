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
/// \file   OamIeeeMacCommon.c
/// \brief  MAC support for IEEE standard OAM
///
////////////////////////////////////////////////////////////////////////////////


#include <string.h>
#include "OamIeeeMac.h"
#include "Teknovus.h"
#include "Oam.h"
#include "OamUtil.h"
#include "EponDevInfo.h"
#include "Stream.h"
#include "Alarms.h"
#include "ApiResLayer.h"
#include "ethswctl_api.h"
#include "cms_log.h"


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get MAC enable status
///
/// \param port     Port instance
/// \param reply    Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeMacEnableStatusGet(TkOnuEthPort port, OamVarContainer BULK * reply)
    {
      //janeqiu: to be implemented  
      int status = EponDevPortConfig(port) & TkOnuEthPortCfgEnable;
      if( status != 0 )
        {
        ContPutU8(reply, OamTrue);
        }
    else
        {
        ContPutU8(reply, OamFalse);
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set MAC enable status
///
/// \param port     Port instance
/// \param src      Request parameters
///
/// \return
/// TRUE if succeeded, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL OamIeeeMacEnableStatusSet(TkOnuEthPort port,
                               const OamVarContainer BULK * src)
    {
#if OAM_FULLSUPPORT
    // We cannot disable the interface connecting to the OAM Master
    if (IsOamMaster (port))
        {
        return FALSE;
        }
#endif

    if (ContGetU8(src) != 0)
        {
        //UniMgrPortEnable(port);
        EponDevPortConfig(port) |= TkOnuEthPortCfgEnable;
        bcm_port_traffic_control_set(0, bcm_enet_map_oam_idx_to_phys_port(port), 0);
        AlarmClearCondition(AlmPortDisabled, port);
        }
    else
        {
        //UniMgrPortDisable(port);
        EponDevPortConfig(port) &= (~TkOnuEthPortCfgEnable);
        bcm_port_traffic_control_set(0, bcm_enet_map_oam_idx_to_phys_port(port), PORT_RXDISABLE | PORT_TXDISABLE);
        AlarmSetCondition(AlmPortDisabled, port);
        }

    return TRUE;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get MAC address for port
///
/// \param port     Port instance
/// \param reply    Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeMacAddrGet(TkOnuEthPort port, OamVarContainer BULK * reply)
    {
    MacAddr BULK* mac = (MacAddr BULK*)reply->value;
    struct ifreq req;
	int sd; 

    reply->length = OamVarErrAttrHwFailure;
		
	/* get socket descriptor */ 
    sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP); 
    if(sd == -1) 
        { 
        return;
        }

    sprintf(req.ifr_name, "eth%d", (int)port);
    
    if (ioctl(sd, SIOCGIFHWADDR, &req) < 0)
        {
        cmsLog_error("failed to get %s hw address!", req.ifr_name);
        close(sd);
        return;            
        }

    reply->length = sizeof(MacAddr);
    memcpy (mac, req.ifr_hwaddr.sa_data, sizeof(MacAddr));
    close(sd);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get MAC duplex status for PON
///
/// \param reply    Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OamIeeePonMacDuplexStatusGet(OamVarContainer BULK* reply)
    {
    ContPutU8 (reply, OamMacDuplexFull);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get MAC duplex status for port
///
/// \param port     Port instance
/// \param reply    Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeUniMacDuplexStatusGet(TkOnuEthPort port,
                                  OamVarContainer BULK * reply)
    {
    int duplex = 0;
    int speed = 0;
    
    bcm_phy_mode_get(0, bcm_enet_map_oam_idx_to_phys_port(port), &speed, &duplex);

    if( duplex )
        {
        ContPutU8(reply, OamMacDuplexFull);
        }
    else
        {
        ContPutU8(reply, OamMacDuplexHalf);
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set MAC duplex status for port
///
/// \param port     Port instance
/// \param src      Request parameters
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeUniMacDuplexStatusSet(TkOnuEthPort port,
                                  const OamVarContainer BULK * src)
    {
    switch (ContGetU8(src))
        {
        case OamMacDuplexHalf :
            //UniMgrDuplexSetHalf(port);
            break;

        case OamMacDuplexFull :
            //UniMgrDuplexSetFull(port);
            break;

        default : // shouldn't happen
            break;
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get supported MAC control functions
///
/// \param port     Port instance
/// \param reply    Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeMacCtrlFuncsGet(TkOnuEthPort port, OamVarContainer BULK * reply)
    {
    //janeqiu: to be implemented
    //if (UniMgrIsFlowCtrlEnabled(port))
    if (TRUE) 
        {
        ContPutU16 (reply, OamMacCtrlFuncPause);
        }
    else
        { // zero-length list, no functions in it
        reply->length = OamVarErrNoError;
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Detects Pause function in functions supported sequence
///
/// \param src      Source container holding MAC Ctrl funcs list
///
/// \return
/// TRUE if MAC Control Pause function is in the list
////////////////////////////////////////////////////////////////////////////////
static
BOOL PauseInMacCtrl (OamVarContainer BULK* src)
    {
    U8 FAST i = OamContValLen(src->length) / sizeof(OamMacCtrlFuncs);
    Stream FAST strm;

    StreamInit(&strm, src->value);
    while (i-- != 0)
        {
        if (StreamReadU16(&strm) == OamMacCtrlFuncPause)
            {
            return TRUE;
            }
        }

    return FALSE;
    } // PauseInMacCtrl


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set supported MAC control functions
///
/// \param port     Port instance
/// \param src      Request parameters
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeMacCtrlFuncsSet(TkOnuEthPort port,
                            const OamVarContainer BULK * src)
    {
    if (PauseInMacCtrl((OamVarContainer *)src))
        {
        //UniMgrFlowCtrlEnable(port);
        }
    else
        {
        //UniMgrFlowCtrlDisable(port);
        }
    }


// end of OamIeeeMacCommon.c
