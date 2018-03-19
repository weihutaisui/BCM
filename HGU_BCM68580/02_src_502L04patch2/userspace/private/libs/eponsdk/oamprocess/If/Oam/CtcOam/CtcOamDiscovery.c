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
/// \file CtcOamDiscovery.c
/// \brief China Telecom extended OAM discovery module
/// \author Jason Armstrong
/// \date February 28, 2006
///
///
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "Teknovus.h"
#include "Build.h"
#include "Alarms.h"
#include "PonManager.h"
#include "CtcOam.h"
#include "CtcOnuOam.h"
#include "CtcOnuUtils.h"
#include "CtcMLlid.h"
#include "CtcEthControl.h"

#if MCAST
#include "CtcMcast.h"
#endif
#include "CtcClassification.h"
#include "CtcOamDiscovery.h"
#include "OntConfigDb.h"
#include "UniConfigDb.h"

#include "UserTraffic.h"
#include "eponctl_api.h"
#include "cms_log.h"


#define CtcOamMaxVersionSupport        CtcOamVer30
#define CtcOamDefaultPonLosTime        0x2
#define CtcOamDefaultGateLosTime       0x37
#define CtcOamDefaultMacAgingTime      0x0000012c


static IeeeOui BULK responseOui;


///////////////////////////////////////////////////////////////////////////////
/// CtcOamNegotiatedVersion - Return the negotiated version
///
/// This function returns the negotiated version number of a the ONU.
///
/// \param None
///
/// \return
/// The negotiated version of the ONU
///////////////////////////////////////////////////////////////////////////////
//extern
CtcOamVersion CtcOamNegotiatedVersion (void)
    {
    return CtcLinkVersionGet(0);
    } // CtcOamNegotiatedVersion


////////////////////////////////////////////////////////////////////////////////
/// CtcOamVersionSupported - Is a version of CTC OAM supported by firmware
///
/// This function will return TRUE if the version number passed is supported
/// by the current firmware.  A version is considered supported if supplied
/// version number is less than or equal to the firmware supported version.
///
/// \note
/// New versions of OAM may undergo changes that will make them incompatible
/// with previous versions.  This function will have to be updated if that
/// situation arises.
///
 // Parameters:
/// \param version OAM version to check
///
/// \return
/// TRUE if version is supported
////////////////////////////////////////////////////////////////////////////////
static
BOOL CtcOamVersionSupported (OamCtcOamVersion version)
    {
    if (EponDevGetOamSel() == OAM_CUC_SUPPORT)
        {
        return (version == CucOamVer03);
        }
    else
        {
        return (version <= CtcOamMaxVersionSupport);
        }
    } // CtcOamVersionSupported


////////////////////////////////////////////////////////////////////////////////
/// CtcOamDiscoverySuccess - Further process after CTC discovery
///
/// Erase NVS first, because CTC reqire no save to NVS
/// Do CTC IGMP default setting here
///
/// Parameters:
/// None
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
void CtcOamDiscoverySuccess(void)
    {
    static BOOL firstTimes = TRUE;
    LosCheckPara los_para;
    
    if (firstTimes)
        {
         if (CtcOamNegotiatedVersion() > CtcOamVer21)
            {
            los_para.losopttime = CtcOamDefaultPonLosTime;
            los_para.losmpcptime = CtcOamDefaultGateLosTime;    
            eponStack_CtlCfgLosCheckTime(EponSetOpe, &los_para);     
            }
        firstTimes = FALSE;
        }
    }// CtcOamDiscoverySuccess


////////////////////////////////////////////////////////////////////////////////
/// CtcOamInfoTlvHandle:  Process CTC-specific TLV types
///
/// This function processes an incoming China Telecom specific OAM info TLV,
/// including parsing the TLV and driving the version negotiation state machine.
/// If the TLV is not an actual CTC info TLV the function will return FALSE.
///
 // Parameters:
/// \param link     Link on which info frame arrived
/// \param tlv      TLV to process
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcOamInfoTlvHandle(LinkIndex link,
                          const OamCtcInfoTlvHeader BULK* tlv)
    {
    U8 FAST numExt;


    memcpy(&responseOui, &tlv->oui, sizeof(IeeeOui));
    numExt = (tlv->length - CtcInfoTvlHdrSize) / sizeof (OamCtcInfoTlvExtData);

    if (numExt > 0)
        {
        //Containing version list. The first discovery info packet arrived.
        CtcLinkInit(link);
        }

    switch (CtcLinkStateGet(link))
        {
        case OamCtcDiscWait:
            CtcLinkStateSet(link, OamCtcDiscNegotiating);
            break;

        case OamCtcDiscNegotiating:
        case OamCtcDiscNegotiated:
            // One CTC Info TVL has been received and we have successfully
            // acknowledged that message.  we must now negotiate on an OAM
            // version number to use.  We save the OAM version that the OLT
            // is using just in case we need it later.
	    if (CtcOamVersionSupported (tlv->version))
	        {
		    cmsLog_debug("set oam version: %x", (CtcOamVersion)tlv->version);
		    CtcLinkVersionSet(link, (CtcOamVersion)tlv->version);
		    CtcLinkStateSet(link, OamCtcDiscNegotiated);
		    AlarmSetCondition (AlmLinkCtcOamDiscComplete, link);
	        }
	    else
	        {
		    CtcLinkStateSet(link, OamCtcDiscFailed);
	        }
	    break;

        case OamCtcDiscSuccess:
        case OamCtcDiscFailed:
            CtcLinkStateSet(link, OamCtcDiscFailed);
            break;
        } // end switch
    } // CtcOamInfoTlvHandle



////////////////////////////////////////////////////////////////////////////////
/// CtcOamInfoTlvGet - Load a CTC info TLV
///
/// This function loads a CTC Organization Specific Information TVL into the
/// supplied buffer.  The size of the TLV is returned.
///
 // Parameters:
/// \param link Logical link ID
/// \param msg Buffer to load into
///
/// \return
/// Size of the info message
////////////////////////////////////////////////////////////////////////////////
//extern
U16 CtcOamInfoTlvGet(LinkIndex link, U8 BULK *msg)
    {
    OamCtcInfoTlvHeader BULK *tlv = (OamCtcInfoTlvHeader BULK *)msg;

    tlv->type = OamTlvOrgSpecific;
    tlv->length = 0;
    memcpy(&tlv->oui, &responseOui, sizeof(IeeeOui));

    switch (CtcLinkStateGet(link))
        {
        case OamCtcDiscWait:
        case OamCtcDiscSuccess:
            break;

        case OamCtcDiscNegotiating:
            {
            U8 FAST i;

            // The first stage of negotiation has completed.  We now need to
            // inform the OLT of the extended OAM version we support.
            tlv->support = TRUE;
            tlv->version = 0;

            for (i = 0; i < CtcLinkExtDataCount(link); i++)
                {
                (void)CtcLinkExtDataGet(link, i, (&tlv->ext[i]));
                }
            tlv->length = CtcInfoTvlHdrSize + (sizeof (OamCtcInfoTlvExtData) *
                 CtcLinkExtDataCount(link));
            }
            break;

        case OamCtcDiscNegotiated:
            // The final stage of negotiation has completed.  An ACK must now be
            // sent back to the OLT with the negotiated version number.
            tlv->support = TRUE;
            tlv->version = CtcLinkVersionGet (link);
            tlv->length = CtcInfoTvlHdrSize;
            CtcLinkStateSet(link, OamCtcDiscSuccess);
            break;

        case OamCtcDiscFailed:
            // Something has gone wrong in the negotiation phase and the links
            // negotiation state has been reset.  This means the ONU does not
            // support any of the OLTs OAM versions.  We need to NACK the OLT to
            // inform the world all is not well.
            tlv->support = FALSE;
            tlv->version = 0;
            tlv->length = CtcInfoTvlHdrSize;
            CtcLinkInit(link);
            break;
            }

    return tlv->length;
    } // CtcOamInfoTlvGet



// End of File CtcOamDiscovery.c

