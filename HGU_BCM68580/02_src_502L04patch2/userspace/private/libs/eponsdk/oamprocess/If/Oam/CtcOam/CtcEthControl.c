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
/// \file CtcEthControl.c
/// \brief China Telecom ether port control module
///
////////////////////////////////////////////////////////////////////////////////
#include <string.h>

#include "Teknovus.h"
#include "Build.h"
#include "CtcEthControl.h"
#include "MultiByte.h"
#include "TkDebug.h"
#include "PonManager.h"
#include "CtcOnuOam.h"
#include "CtcClassification.h"
#include "CtcMLlid.h"
#include "ethswctl_api.h"
#include "EponDevInfo.h"

U8  pausePortEn;
static CtcEthPortUsPolice policer[MAX_UNI_PORTS] = {0};

//##############################################################################
// E1 and VOIP Stubs
//##############################################################################

// now ONU doesn't support this


//##############################################################################
// Ether port Rate Limit
// Ether port US Policing & DS Rate Limiting(Shaper)
//##############################################################################


// The database of Ctc ether port upstream police configuration
static
CtcEthPortUsPolice BULK ethPortUsPolice[MAX_UNI_PORTS];

// The database of Ctc ether port downstream shaper configuration
static
CtcEthPortDsShaper BULK ethPortDsShaper[MAX_UNI_PORTS];

static
U8 BULK ctcFcConfigDirty;


////////////////////////////////////////////////////////////////////////////////
/// CtcFCReserveQueueSize  the reserve queue size for ctc flow control function.
///
 // Parameters:
/// \param port     The ethernet port number
///////////////////////////////////////////////////////
/////////////////////////
U16 CtcFCReserveQueueSize(TkOnuEthPort port)
	{
	return 0;
	}



////////////////////////////////////////////////////////////////////////////////
/// CtcFlowControlSupport  check if support ctc flow control function.
///
 // Parameters:
/// \return
/// TRUE if support, otherwise FALSE
///////////////////////////////////////////////////////
/////////////////////////
BOOL CtcFlowControlSupport(void)
	{
	return FALSE;
	}



////////////////////////////////////////////////////////////////////////////////
/// CtcFlowControlCheck  check if enable ctc flow control function.
///
 // Parameters:
/// \return
/// TRUE if enable flow control, otherwise FALSE
///////////////////////////////////////////////////////
/////////////////////////
BOOL CtcFlowControlCheck(TkOnuEthPort port)
	{
	return FALSE;
	}




////////////////////////////////////////////////////////////////////////////////
/// CtcFlowControlTimer  detect the flow control if hung.
///
/// Parameters:
/// \param port     The ethernet port number
/// \return
///////////////////////////////////////////////////////
/////////////////////////
void CtcFlowControlTimer(void)
	{
    return;	
	}



////////////////////////////////////////////////////////////////////////////////
/// CtcFlowControlUpdate  update the flow control configuration to UNI port.
///
 // Parameters:
/// \param port     The ethernet port number
/// \param enable  enable/disable pause frame transmit to UNI port.
/// \param flag       flag for OntDirWarmInit.
///
/// \return
/// TRUE if install successfully, otherwise FALSE
///////////////////////////////////////////////////////
/////////////////////////
BOOL CtcFlowControlUpdate(TkOnuEthPort port,BOOL enable)
	{
	return TRUE;
	}

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
BOOL CtcEthControlPauseGet(TkOnuEthPort port,BOOL* enable)
    {  
    U8 pause;
    
    bcm_port_pause_capability_get(0, bcm_enet_map_oam_idx_to_phys_port(port), (char *)&pause);
    *enable = (pause == PAUSE_FLOW_CTRL_NONE) ? FALSE:TRUE;
    return TRUE;
    }



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
BOOL CtcEthControlPauseSet(TkOnuEthPort port,BOOL enable)
    {  
    U8 pause = (enable == FALSE) ? PAUSE_FLOW_CTRL_NONE:PAUSE_FLOW_CTRL_BOTH;
    
    bcm_port_pause_capability_set(0, bcm_enet_map_oam_idx_to_phys_port(port), pause);
    return TRUE;
    }

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
//extern
void CtcFlowControlCommitConfig (void)
    {
    return;    
    } // CtcFlowControlCommitConfig


////////////////////////////////////////////////////////////////////////////////
/// CtcEthControlUsPoliceCfgSet  Set the current Upsteam policing configuration
///                              for the port.
///
 // Parameters:
/// \param port    The ethernet port number
/// \param cfg     Rate configuration
///
/// \return
/// TRUE if successfully, otherwise FALSE
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL CtcEthControlUsPoliceCfgSet(TkOnuEthPort port,
                      const CtcEthPortUsPolice BULK *cfg)
    {
    U32 rate = 0;
    U32 burst = 0;

    if (memcmp((U8 *)cfg, (U8 *)&policer[port], sizeof(CtcEthPortUsPolice))== 0)
        {
        // nothing change
        return TRUE;
        }
    if (cfg->enabled)
        {
        rate = cfg->cir[0] << 16 | cfg->cir[1] << 8 | cfg->cir[2];
        burst = cfg->cbs[0] << 16 | cfg->cbs[1] << 8 | cfg->cbs[2];
        if (bcm_port_rate_ingress_set(0, bcm_enet_map_oam_idx_to_phys_port(port), rate, burst/1000) != 0)
            {
            return FALSE;
            }
        }
    memcpy (&policer[port], cfg, sizeof(CtcEthPortUsPolice));
    return TRUE;
    } // CtcEthControlUsPoliceCfgSet


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
//extern
U8 CtcEthControlUsPoliceCfgGet(TkOnuEthPort port, CtcEthPortUsPolice BULK *cfg)
    {
    memcpy ((U8 *)cfg, (U8 *)&policer[port], sizeof(CtcEthPortUsPolice));
    return sizeof(CtcEthPortUsPolice);
    } // CtcEthControlUsPoliceCfgGet

BOOL CtcEthControlDsShaperCfgSet(TkOnuEthPort port,
                      const CtcEthPortDsShaper BULK *cfg)
{
    MultiByte32  rate;
    MultiByte32  burst;

    rate.u32 = 0;
    burst.u32 = 0;
    if (cfg->enabled)
    {
        rate.array[1] = cfg->cir[0];
        rate.array[2] = cfg->cir[1];
        rate.array[3] = cfg->cir[2];
    }

    if (0 != bcm_port_rate_egress_set(0,bcm_enet_map_oam_idx_to_phys_port(port), rate.u32,burst.u32))
        return FALSE;

    memcpy (&ethPortDsShaper[port], cfg, sizeof(CtcEthPortDsShaper));
    return TRUE;
} 

U8 CtcEthControlDsShaperCfgGet(TkOnuEthPort port, CtcEthPortDsShaper BULK *cfg)
{
    U8  size = (ethPortDsShaper[port].enabled?
        sizeof(CtcEthPortDsShaper) :
        sizeof(ethPortDsShaper[port].enabled));
        
    memcpy ((U8 *)cfg, (U8 *)&ethPortDsShaper[port], size);
    return size;
} 


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
void CtcEthControlDsShaperResume(void)
    {
    return;
    } // CtcEthControlDsShaperResume


////////////////////////////////////////////////////////////////////////////////
/// CtcEthControlInit  Initialize the Ctc Ether Port Control module
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcEthControlInit(void)
    {
    U8 i;    
    char cmdStr[64];
    
     for(i = 0; i < UniCfgDbGetActivePortCount(); i++)
        {
        memset(cmdStr, 0,sizeof(cmdStr));
        snprintf(cmdStr, sizeof(cmdStr), "ifconfig eth%d mtu %d", i, ETH_DATA_LEN);
        system(cmdStr);            
        memset(cmdStr, 0,sizeof(cmdStr));
        snprintf(cmdStr, sizeof(cmdStr), "ifconfig eth%d.0 mtu %d", i, ETH_DATA_LEN);
        system(cmdStr);            
        }
    memset(cmdStr, 0,sizeof(cmdStr));
    snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s mtu %d", eponRealIfname, ETH_DATA_LEN);
    system(cmdStr);           

    return;
    } // CtcEthControlInit


// End of File CtcEthControl.c

