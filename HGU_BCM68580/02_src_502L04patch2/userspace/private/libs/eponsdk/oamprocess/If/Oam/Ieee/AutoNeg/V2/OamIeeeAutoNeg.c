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
/// \file   OamIeeeAutoNeg.c
/// \brief  Autonegotiation support for IEEE standard OAM
///
////////////////////////////////////////////////////////////////////////////////


#include "OamIeeeAutoNeg.h"
#include "Teknovus.h"
#include "Oam.h"
#include "OamIeee.h"
#include "OamUtil.h"

#include "Stream.h"
#include "UniConfigDb.h"

#include "bcmnet.h"
#include "ethswctl_api.h"

#if OAM_FULLSUPPORT
////////////////////////////////////////////////////////////////////////////////
/// \brief  Get auto-negotiation remote signal
///
/// \param port     Port instance
/// \param reply    Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeRemoteSigGet(TkOnuEthPort port, OamVarContainer BULK* reply)
    {
#warning "No OAM support"
    }
#endif


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get auto-negotiation configuration
///
/// \param port     Port instance
/// \param reply    Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeAutoCfgGet(TkOnuEthPort port, OamVarContainer BULK* reply)
    {
#if OAM_FULLSUPPORT
#warning "No OAM support"
#else
    UNUSED(port);
    ContPutU8 (reply, OamAutoRemoteNotDetected);
#endif
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  Make up the 32 bit with capability
///
/// \param cont     Location for response
/// \param techs    phy cap to advertise
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static void AutoNegTechToList32(OamVarContainer BULK* cont, U16 techs)
    {
    
    U32 FAST count = 0;
    Stream strm;

    cont->length = sizeof(count);
    StreamInit(&strm, cont->value);
    StreamSkip(&strm, sizeof(count));

    if (techs & AN_1000M_HALF)
        {
        StreamWriteU16(&strm, OamAutoCapGlobal);
        StreamWriteU16(&strm, OamAutoCap1000T);
        count++;
        }

    if (techs & AN_1000M_FULL)
        {
        StreamWriteU16(&strm, OamAutoCapGlobal);
        StreamWriteU16(&strm, OamAutoCap1000TFD);
        count++;
        }

    if (techs & AN_100M_HALF)
        {
        StreamWriteU16(&strm, OamAutoCapGlobal);
        StreamWriteU16(&strm, OamAutoCap100TX);
        count++;
        }

    if (techs & AN_100M_FULL)
        {
        StreamWriteU16(&strm, OamAutoCapGlobal);
        StreamWriteU16(&strm, OamAutoCap100TXFD);
        count++;
        }

    if (techs & AN_10M_HALF)
        {
        StreamWriteU16(&strm, OamAutoCapGlobal);
        StreamWriteU16(&strm, OamAutoCap10T);
        count++;
        }

    if (techs & AN_10M_FULL)
        {
        StreamWriteU16(&strm, OamAutoCapGlobal);
        StreamWriteU16(&strm, OamAutoCap10TFD);
        count++;
        }

    cont->length += (U8)count * sizeof(U32);
    StreamRewind(&strm, cont->length);
    StreamWriteU32(&strm, count);
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  Make up the 16 bit with capability
///
/// \param cont     Location for response
/// \param techs    phy cap to advertise
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void AutoNegTechToList16 (OamVarContainer BULK* cont, U16 techs)
    {
    Stream strm;

    cont->length = 0;
    StreamInit(&strm, cont->value);

    if (techs & AN_1000M_HALF)
        {
        StreamWriteU16(&strm, OamAutoCap1000T);
        cont->length += sizeof(OamAutoNegCapability);
        }

    if (techs & AN_1000M_FULL)
        {
        StreamWriteU16(&strm, OamAutoCap1000TFD);
        cont->length += sizeof(OamAutoNegCapability);
        }

    if (techs & AN_100M_HALF)
        {
        StreamWriteU16(&strm, OamAutoCap100TX);
        cont->length += sizeof(OamAutoNegCapability);
        }

    if (techs & AN_100M_FULL)
        {
        StreamWriteU16(&strm, OamAutoCap100TXFD);
        cont->length += sizeof(OamAutoNegCapability);
        }

    if (techs & AN_10M_HALF)
        {
        StreamWriteU16(&strm, OamAutoCap10T);
        cont->length += sizeof(OamAutoNegCapability);
        }

    if (techs & AN_10M_FULL)
        {
        StreamWriteU16(&strm, OamAutoCap10TFD);
        cont->length += sizeof(OamAutoNegCapability);
        }
    } // AutoNegTechMdioToList16


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get auto-negotiation local tech
///
/// \param port     Port instance
/// \param reply    Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeLocalTechGet(TkOnuEthPort port, OamVarContainer BULK * reply)
    {
    unsigned char autoneg;
    unsigned short local_cap, ad_cap;

    bcm_phy_autoneg_info_get(0, bcm_enet_map_oam_idx_to_phys_port(port), &autoneg, &local_cap, &ad_cap);

    if ((EponDevGetOamSel() == 0) || 
        (EponDevGetOamSel() == OAM_CTC_SUPPORT) || 
        (EponDevGetOamSel() == OAM_CUC_SUPPORT))
        {
        AutoNegTechToList32(reply, local_cap);
        }
    else
        {
        AutoNegTechToList16(reply, local_cap);
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get auto-negotiation advertised tech
///
/// \param port     Port instance
/// \param reply    Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeAdTechGet (TkOnuEthPort port, OamVarContainer BULK* reply)
    {
    unsigned char autoneg;
    unsigned short local_cap, ad_cap;

    bcm_phy_autoneg_info_get(0, bcm_enet_map_oam_idx_to_phys_port(port), &autoneg, &local_cap, &ad_cap);

    if ((EponDevGetOamSel() == 0) || 
        (EponDevGetOamSel() == OAM_CTC_SUPPORT) || 
        (EponDevGetOamSel() == OAM_CUC_SUPPORT))
        {
        AutoNegTechToList32(reply, local_cap);
        }
    else
        {
        AutoNegTechToList16(reply, local_cap);
        }
    } // OamHdlAutoNegAdTech



////////////////////////////////////////////////////////////////////////////////
/// \brief  Set auto-negotiation advertised tech
///
/// \param port     Port instance
/// \param src      Request parameters
/// \param reply    Location for response
///
/// \return
/// TRUE if set succeeded, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL OamIeeeAdTechSet (TkOnuEthPort port,
                       const OamVarContainer BULK* src,
                       OamVarContainer BULK* reply)
    {
    U8 autoneg;
    unsigned short ad_cap = 0;
    Stream FAST strm;
    U8 FAST len = src->length;

    if (len == OamVarErrNoError)
        {
        return FALSE;
        }
    if ((len % sizeof(OamAutoNegCapability)) != 0)
        {
        return FALSE;
        }

    StreamInit(&strm, (U8 *)src->value);
    for (; len != 0; len -= sizeof(OamAutoNegCapability))
        {
        switch (StreamReadU16(&strm))
            {
            //10 Half
            case OamAutoCap10T :
                ad_cap |= AN_10M_HALF;
                break;

            //10 full
            case OamAutoCap10TFD :
                ad_cap |= AN_10M_FULL;
                break;
            //100 FULL duplex
            case OamAutoCap100TXFD :
                ad_cap |= AN_100M_FULL;
                break;
            //100 HALF duplex
            case OamAutoCap100TX :
                ad_cap |= AN_100M_HALF;
                break;

            //1000 Full
            case OamAutoCap1000XFD :
            case OamAutoCap1000TFD :
                ad_cap |= AN_1000M_FULL;
                break;
            case OamAutoCap1000X :
            case OamAutoCap1000T  :
                ad_cap |= AN_1000M_HALF;
                break;
            case OamAutoCapFdxPause:
                ad_cap |= AN_FLOW_CONTROL;
                break;
            case OamAutoCap10GBaseX:
            case OamAutoCap10GBaseLX4:
            case OamAutoCap10GBaseR:
            case OamAutoCap10GBaseER:
            case OamAutoCap10GBaseLR:
            case OamAutoCap10GBaseSR:
            case OamAutoCap10GBaseW:
            case OamAutoCap10GBaseEW:
            case OamAutoCap10GBaseLW:
            case OamAutoCap10GBaseSW:
            default :
                reply->length = OamVarErrAttrUnsupported;
                break;
            }
        }
    
    autoneg = AUTONEG_RESTART_MASK;

    (void)bcm_phy_autoneg_cap_adv_set(0, bcm_enet_map_oam_idx_to_phys_port(port), autoneg, &ad_cap);

    return TRUE;
    } // OamHdlAutoNegTechSet


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get auto-negotiation receive tech
///
/// \param port     Port instance
/// \param reply    Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeRxTechGet(TkOnuEthPort port, OamVarContainer BULK * reply)
    {
#if OAM_FULLSUPPORT
#warning "No OAM support"
#else
    UNUSED(port);
    UNUSED(reply);
#endif
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Auto-negotiation re-negotiate
///
/// \param port     Port instance
/// \param src      Request parameters
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeAutoRenegotiate(TkOnuEthPort port,
                            const OamVarContainer BULK * src)
    {
    unsigned char autoneg = AUTONEG_CTRL_MASK|AUTONEG_RESTART_MASK;

    bcm_phy_autoneg_info_set(0, bcm_enet_map_oam_idx_to_phys_port(port), autoneg);
    }


// end of OamIeeeAutoNeg.c
