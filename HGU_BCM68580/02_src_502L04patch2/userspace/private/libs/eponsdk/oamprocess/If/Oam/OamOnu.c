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
/// \file OamOnu.c
/// \brief Main interface to the ONU OAM module
///
////////////////////////////////////////////////////////////////////////////////
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "OamOnu.h"
#include "Alarms.h"
#include "ApiResLayer.h"
#include "Oam.h"
#include "OamProcessInit.h"
#include "Sff8472.h"
#include "laser.h"

#include "cms_log.h"

#include "ethswctl_api.h"

typedef enum
    {
    PortLoopbackTypeNone,
    PortLoopbackTypeFw,  /**< loopback in firmware */
    PortLoopbackTypeMac, /**< loopback in mac */
    PortLoopbackTypePhy,  /**< loopback in phy */
    }PortLoopbackType;

typedef enum
    {
    LoopBackOpNone = 0,
    LoopBackOpLocal,
    LoopBackOpRemote
    }LoopBackOp;


static int LaserFd = 0;

extern OamHandlerCB *oamVenderHandlerHead;


////////////////////////////////////////////////////////////////////////////////
/// \brief  Send a key exchange OAM to the OLT
///
/// \param link     Link index for key
/// \param keyNum   Key index to exchange
/// \param oam      Type of OAM to send
/// \param KeyGen     flag indicates whether generate a new key
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OamSendKeyExchange(OuiVendor oam, LinkIndex link, U8 keyNum, BOOL keyGen)
    {
    OamHeaderFill(link);
    
    VENDOR_ENC_KEY_EXCHANGE(oam, link, keyNum, keyGen);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Enable extended loopback
///
/// \param port     Port to loopback
/// \param loop     Loopback action parameters
/// \param reply    Location for action reply
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void ActLoopEnable (TkOnuEthPort port,
                    OamExtLoopback const BULK* loop,
                    OamVarContainer BULK* reply)
    {
    S32 val;
    
    reply->length = sizeof(OamExtLoopback);
    ((OamExtLoopback BULK*)reply->value)->loc = loop->loc;

    if (loop->loc == OamLoopLocUniMac)
        {
        val = (PortLoopbackTypeMac << 8) | LoopBackOpLocal;
        if(bcm_port_loopback_set(0, bcm_enet_map_oam_idx_to_phys_port(port), val) != 0)
            {
            reply->length = OamVarErrAttrUndetermined;
			return;
            }
        else
            {
            reply->length = OamVarErrNoError;
            }
        }
    else if(loop->loc == OamLoopLocUniPhy)
        {
        val = (PortLoopbackTypePhy << 8) | LoopBackOpLocal;        
        if(bcm_port_loopback_set(0, bcm_enet_map_oam_idx_to_phys_port(port), val) != 0)
            {
            reply->length = OamVarErrAttrUndetermined;
			return;
            }
        else
            {
            reply->length = OamVarErrNoError;
            }
        }
    else
        {
        reply->length = OamVarErrActBadParameters;
		return;
        }    
    

    AlarmSetCondition (AlmPortLoopback, port);
    } // ActLoopEnable


////////////////////////////////////////////////////////////////////////////////
/// \brief  Disable extended loopback
///
/// \param port     Port to loopback
/// \param loop     Loopback action parameters
/// \param reply    Location for action reply
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void ActLoopDisable (TkOnuEthPort port,
                     OamExtLoopback const BULK* loop,
                     OamVarContainer BULK* reply)
    {
    S32 val;
    
    ((OamExtLoopback BULK*)reply->value)->loc = loop->loc;
    reply->length = sizeof(OamExtLoopback);

    val = (PortLoopbackTypeNone << 8)| LoopBackOpLocal;
        if ((loop->loc == OamLoopLocUniMac) || (loop->loc == OamLoopLocUniPhy))
            {
            if(bcm_port_loopback_set(0, bcm_enet_map_oam_idx_to_phys_port(port), val) != 0)
                {
                reply->length = OamVarErrAttrUndetermined;
				return;
                }
            else
                {
                reply->length = OamVarErrNoError;
                }        
            }
        else
            {
            reply->length = OamVarErrActBadParameters;
			return;
            }    

    AlarmClearCondition (AlmPortLoopback, port);
    } // ActLoopDisable

////////////////////////////////////////////////////////////////////////////////
//extern
void OamOnuPoll(void)
    {
    VENDOR_OAM_POLL();
    }


static BOOL optical_para_get(U16 *value, Sff8472RegAddr addr)
    {
    U32 val;
    int ret;
    char objInfo[50]={0};


    if(LaserFd == 0)
        {
        LaserFd = open("/dev/laser_dev", O_RDWR);

        if (LaserFd < 0)
            {
            cmsLog_error("Laser driver open error");
            }
        }
    
    if((LaserFd <= 0) || (value == 0))
        {
        return FALSE;
        }    

    switch(addr)
        {
        case Sff8472Temp:
            ret = ioctl(LaserFd, LASER_IOCTL_GET_TEMPTURE, &val);
            sprintf(objInfo, "LASER_IOCTL_GET_TEMPTURE");
            break;
            
        case Sff8472Vcc:
            ret = ioctl(LaserFd, LASER_IOCTL_GET_VOTAGE, &val);
            sprintf(objInfo, "LASER_IOCTL_GET_TEMPTURE");
            break;
            
        case Sff8472TxBias:
            ret = ioctl(LaserFd, LASER_IOCTL_GET_BIAS_CURRENT, &val);
            sprintf(objInfo, "LASER_IOCTL_GET_TEMPTURE");
            break;
            
        case Sff8472TxPower:
            ret = ioctl(LaserFd, LASER_IOCTL_GET_TX_PWR, &val);
            sprintf(objInfo, "LASER_IOCTL_GET_TEMPTURE");           
            break;
            
        case Sff8472RxPower:
            ret = ioctl(LaserFd, LASER_IOCTL_GET_RX_PWR, &val);
            sprintf(objInfo, "LASER_IOCTL_GET_TEMPTURE");            
            break;
            
        default:
            ret = -1;
            sprintf(objInfo, "Unknow Obj");            
            break;
        }
    if(ret == 0)
        {
        *value = (U16)(val & 0xFFFF);
        }    
    else
        {
        *value = 0;
        cmsLog_error("Laser driver IOCTL error on %s", objInfo);
        }
    return TRUE;
    }

////////////////////////////////////////////////////////////////////////////////
//extern
BOOL optical_temp(U16 *value)
    {
    return optical_para_get(value, Sff8472Temp);
    }


////////////////////////////////////////////////////////////////////////////////
//extern
BOOL optical_vcc(U16 *value)
    {
    return optical_para_get(value, Sff8472Vcc);
    }


////////////////////////////////////////////////////////////////////////////////
//extern
BOOL optical_bias(U16 *value)
    {
    return optical_para_get(value, Sff8472TxBias);
    }


////////////////////////////////////////////////////////////////////////////////
//extern
BOOL optical_rxpower(U16 *value)
    {
    return optical_para_get(value, Sff8472RxPower);
    }


////////////////////////////////////////////////////////////////////////////////
//extern
BOOL optical_txpower(U16 *value)
    {
    return optical_para_get(value, Sff8472TxPower);
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamOnuInit(void)
    {
    OamIeeeInit();
    }

// end of OamOnu.c
