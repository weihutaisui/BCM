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
/// \file CtcOnuOam.c
/// \brief China Telecom extended OAM generic module
/// \author Jason Armstrong
/// \date February 28, 2006
///
///
////////////////////////////////////////////////////////////////////////////////
#include <string.h>

#include "Teknovus.h"
#include "Build.h"
#include "Oam.h"
#include "OamUtil.h"
#include "CtcOnuUtils.h"
#include "CtcAuthentication.h"
#include "CtcReport.h"
#include "CtcEncryption.h"
#include "CtcMLlid.h"
#include "CtcOamFileTransfer.h"
#include "CtcClassification.h"
#include "CtcEthControl.h"
#include "CtcOam.h"
#include "CtcOamDiscovery.h"
#include "CtcOnuOam.h"
#include "CtcVlan.h"
#include "CtcAlarms.h"
#include "CtcMcast.h"
#include "CtcStats.h"
#include "CtcOptDiag.h"
#include "CtcAlarms.h"
#include "Alarms.h"
#include "OamIeee.h"
#include "OamCtcPowerSave.h"
#include "UserTraffic.h"
#include "OntConfigDb.h"
#include "cms_log.h"
#include "UniConfigDb.h"

CtcLlidMode BULK ctcLlidMode;
CtcAlmMonThd ctc30AlmThd[OamCtc30PonIfAlmNum];
CtcAlmMonThd ctc30EthAlmThd[MAX_UNI_PORTS][OamCtc30PortAlmNum];

//CTC Aging time is tracked in seconds
U32 macLearnCtcAgingTime = 0;
BOOL macCtcAgingTimeSet = FALSE;


//##############################################################################
//                  Ctc Link Oam Record management module
//##############################################################################
CtcUpSllidQueueSizes BULK sLlidUpQueueSizes;
PersCtcUpQueueSizes BULK ctcUpQueueSizes;

////////////////////////////////////////////////////////////////////////////////
/// ctcLinkOamRec - Link provisioning information
///
/// This vector contains the China Telecom extended link provisioning
/// information this information includes OAM version, negotiation state, etc.
////////////////////////////////////////////////////////////////////////////////
static
CtcLinkRecord BULK ctcLinkRecord[TkOnuNumTxLlids];

////////////////////////////////////////////////////////////////////////////////
/// CtcLinkInit - Initialize a link oam management record
///
/// This function initalizes a link management record to the default state.
/// The default state is unnegotiated with version 0 and no extended OUIs.
///
/// \param link  The index of the link to initialize
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcLinkInit(LinkIndex link)
    {
    U8 i;

    ctcLinkRecord[link].state   = OamCtcDiscWait;

    if (link == 0)
        {
        ctcLlidMode = CtcLlidModeNone;
        }
    
    if (EponDevGetOamSel() == OAM_CUC_SUPPORT)
        {
        CucOamVersion supportVer[CucOamVerNum] = {
            CucOamVer03};

        ctcLinkRecord[link].version = CucOamVerDef;
        ctcLinkRecord[link].numExt  = CucOamVerNum;

        for ( i=0; i<CucOamVerNum; i++ )
            {
            if ( supportVer[i] <= CucOamVer03 )
                {
                memcpy ((void *)&(ctcLinkRecord[link].support[i]), 
                    (void *)&CtcOui, sizeof (IeeeOui));
                ctcLinkRecord[link].support[i].version = supportVer[i];
                }
            }
        }
    else
        {
        OamCtcVersion supportVer[CtcOamVerNum] = {
            OamCtcVer01,
            OamCtcVer13,
            OamCtcVer20,
            OamCtcVer21,
            OamCtcVer30};

        ctcLinkRecord[link].version = CtcOamVerDef;
        ctcLinkRecord[link].numExt  = CtcOamVerNum;

        for ( i=0; i<CtcOamVerNum; i++ )
            {
            if ( supportVer[i] <= OamCtcVer30 )
                {
                memcpy ((void *)&(ctcLinkRecord[link].support[i]), 
                    (void *)&CtcOui, sizeof (IeeeOui));
                ctcLinkRecord[link].support[i].version = supportVer[i];
                }
            }
        }
    } // CtcLinkInit


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkVersionGet - Get the negotiated version of the link
///
/// This function returns the negotiated version number of a link.  If
/// negotiation has not completed on this link, the return value is undefined.
///
/// \param link  The index of the link
///
/// \return
/// the version negotiated for the link
////////////////////////////////////////////////////////////////////////////////
//extern
CtcOamVersion CtcLinkVersionGet(LinkIndex link)
    {
    return ctcLinkRecord[link].version;
    } // CtcLinkVersionGet


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkVersionSet - Set the negotiated version of the link
///
/// This function sets the negotiated China Telecom OAM version number of a
/// link.  The value may be queried later with CtcLinkGetVersion.
///
/// \param link    The index of the link
/// \param version Version number to set
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcLinkVersionSet (LinkIndex link, CtcOamVersion version)
    {
    if (EponDevGetOamSel() == OAM_CUC_SUPPORT)
        {
        OamIeeeVendorNegSet(link, OuiCuc);
        }
    else
        {
        OamIeeeVendorNegSet(link, OuiCtc);
        }
        
    ctcLinkRecord[link].version = version;
    ctcLinkRecord[link].state = OamCtcDiscSuccess;
    CtcOamDiscoverySuccess();
    //FifCmdLinkEnable(
    //    (1UL << link) & MpcpActiveLinkMap() & UserTrafficRdyLinkMap(),
    //    FifTypeUser);
    if (link == 0)
        { //if it is the base link,  set it as single LLID mode
        CtcMLlidAdminControl(0);
        }
    else
        {
        CtcClassificationLinkRuleClr(link);
        }	
    AlarmSetCondition(AlmLinkCtcOamDiscComplete, link);
    } // CtcLinkVersionSet


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkStateGet - Get the links version negotiation state
///
/// This function returns the China Telecom extended OAM version negotiation
/// state for this link.
///
/// \param link  The index of the link
///
/// \return
/// Link OAM version negotiation state
////////////////////////////////////////////////////////////////////////////////
//extern
OamCtcDiscoveryState CtcLinkStateGet (LinkIndex link)
    {
    return ctcLinkRecord[link].state;
    } // CtcLinkStateGet


////////////////////////////////////////////////////////////////////////////////
/// \brief  Transition the link to the specified discovery state
///
/// \param link  The index of the link
/// \param  next    The new state
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcLinkStateSet(LinkIndex link, OamCtcDiscoveryState next)
    {    
    ctcLinkRecord[link].state = next;
    } // CtcLinkStateNext


////////////////////////////////////////////////////////////////////////////////
//extern
U32 CtcOamDiscCompMap (void)
    {
    U32 map = 0;
    LinkIndex link;
    for (link = 0; link < TkOnuNumTxLlids; ++link)
        {
        if (ctcLinkRecord[link].state == OamCtcDiscSuccess)
            {
            map |= (1UL<<link);
            }
        }
    return map;
    }


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkExtDataGet - Get an extended version record for link
///
/// This function fetches a extended data record from the link management
/// record.  If the index is out of range FALSE is return the extended data
/// field is left alone.
///
 // Parameters:
/// \param link  The index of the link
/// \param index Extended data record to access
/// \param ext   Extended data record to copy to
///
/// \return
/// TRUE if index was in range
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL CtcLinkExtDataGet (LinkIndex link, U8 index,
                        OamCtcInfoTlvExtData BULK *ext)
    {
    if (index < ctcLinkRecord[link].numExt)
    {
        memcpy (ext, &(ctcLinkRecord[link].support[index]),
                                   sizeof(OamCtcInfoTlvExtData));
        return TRUE;
            }
    return FALSE;
    } // CtcLinkExtDataGet


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkExtDataAdd - Add an extended data record to link
///
/// This function adds a new extended OAM support record to the link management
/// record.  If the new data will not fit in the record, FALSE is returned.
///
 // Parameters:
/// \param link  The index of the link
/// \param ext   Extended data record to add
///
/// \return
/// TRUE if the record was added
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL CtcLinkExtDataAdd (LinkIndex link,
                        const OamCtcInfoTlvExtData BULK *ext)
    {
    if (ctcLinkRecord[link].numExt < CtcOamVerNum)
        {
        memcpy (&(ctcLinkRecord[link].support[ctcLinkRecord[link].numExt++]),
                                 ext, sizeof (OamCtcInfoTlvExtData));
        return TRUE;
        }
    return FALSE;
    } // CtcLinkExtDataAdd


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkExtDataCount - Get the number of extended data records
///
/// This function returns the number of extended OAM support records that are
/// currently stored for a link.
///
 // Parameters:
/// \param link  The index of the link
///
/// \return
/// Number of extended OAM support records
////////////////////////////////////////////////////////////////////////////////
//extern
U8 CtcLinkExtDataCount (LinkIndex link)
    {
    return ctcLinkRecord[link].numExt;
    } // CtcLinkExtDataCount


//////////////////////////
static void OamCtcEvtInit(void)
    {
    U16 i;
    for (i = 0;i < OamCtc30PonIfAlmNum;i++)
        {
        ctc30AlmThd[i].CtcAlmRaise = 0xFFFFFFFFUL;
        ctc30AlmThd[i].CtcAlmClear = 0xFFFFFFFFUL;
        ctc30AlmThd[i].CtcAlmFlag  = FALSE;
        }
    for (i = 0;i < OamCtc30PortAlmNum;i++)
        {
        U8 port;
        for (port = 0;port < MAX_UNI_PORTS; port++)
            {
            ctc30EthAlmThd[port][i].CtcAlmRaise = 0xFFFFFFFFUL;
            ctc30EthAlmThd[port][i].CtcAlmClear = 0xFFFFFFFFUL;
            ctc30EthAlmThd[port][i].CtcAlmFlag  = FALSE;
            }
        }
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  Process extended OAM for  EVENT_STATUS
///        SUBTYPE = 0x01
///
/// \param msg      pointer to the Event status OAM message Header.
///
/// +------+------+-----+-----+-------+-----+-----+-----+---+----+-----+
/// |      |      |     |     |       |     |     |     |   |    |     |
/// | DA   | SA   | 8809|0x03 | FLAGS |FE   |OUI  |FF   |SUB|ENT |     |
/// |      |      |     |     |       |     |     |     |TYP|NUM |     |
/// +------+------+-----+-----+-------+-----+-----+-----+---+----+-----+
/// \return  None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleEvtExtStat (OamCtcExtEvtPdu BULK *msg)
    {
    U8              FAST i;
    OamCtcObjType   obj;
    U16             alarmId;
    TkOnuEthPort    port = TkOnuPortNotUsed;
    OamCtcExtEvtPdu BULK * FAST tx = (OamCtcExtEvtPdu BULK*)oamParser.reply.cur;
    OamCtcAlarmStat BULK * FAST almStatRep = (OamCtcAlarmStat BULK *)(tx + 1);
    OamCtcAlarmObj  BULK * FAST almStatReq = (OamCtcAlarmObj BULK *)(msg + 1);
    OamCtcAlarmStat BULK * FAST almStatSet = (OamCtcAlarmStat BULK *)(msg + 1);
    TkOnuEthPort       startPort = TkOnuPortNotUsed;
    TkOnuEthPort       endPort = TkOnuPortNotUsed;
    OamCtcObjType   tmpObjType;
    OamCtcInstNumU tmpInstNum;
    U8 entryCount = 0;

    tx->ext     = OamCtcEvent;
    tx->subtype = OamCtcExtEvtStatRep;
    tx->num     = msg->num;

    for (i = 0; i < OAM_NTOHS(msg->num); i++)
        {
        if (msg->subtype == OamCtcExtEvtStatReq)
            {
            tmpObjType = (OamCtcObjType)almStatReq->objType;
            tmpInstNum = almStatReq->instNum;
            }
        else
            {
            tmpObjType = (OamCtcObjType)almStatSet->almObj.objType;
            tmpInstNum = almStatSet->almObj.instNum;
            }

        switch(OAM_NTOHS(tmpObjType))
            {
            case OamCtcObjPort:
                if((tmpInstNum.sNum.portType == OamCtcPortEth) &&
                    (OAM_NTOHS(tmpInstNum.sNum.portNum) == OamCtcAllUniPorts))
                    {
                    startPort = 0;
                    endPort = UniCfgDbGetActivePortCount() ;
                    }
                else
                    {
                    startPort  =(U8)(OAM_NTOHS(tmpInstNum.sNum.portNum)) -  (U8)(1U);
                    //Spec doesn't define how to handle this, just avoid overflow.
                    if(startPort >=  UniCfgDbGetActivePortCount())
                        {
                        startPort = UniCfgDbGetActivePortCount() - 1;
                        }
                    endPort = startPort +1;
                    }
                break;
            default:
                startPort  =0 ;
                endPort = startPort + 1;
            break;
            }

        for (port = startPort; port < endPort; port++  )
            {
            if (msg->subtype == OamCtcExtEvtStatReq)
                {
                memcpy(&(almStatRep->almObj), almStatReq, sizeof(OamCtcAlarmObj));
                obj = (OamCtcObjType)OAM_NTOHS(almStatReq->objType);
                alarmId = OAM_NTOHS(almStatReq->almID);
                }
            else
                {
                memcpy(&(almStatRep->almObj), &(almStatSet->almObj),
                sizeof(OamCtcAlarmObj));
                obj = (OamCtcObjType)OAM_NTOHS(almStatSet->almObj.objType);
                alarmId = OAM_NTOHS(almStatSet->almObj.almID);
                }

            if((tmpInstNum.sNum.portType == OamCtcPortEth) &&
                (OAM_NTOHS(tmpInstNum.sNum.portNum) == OamCtcAllUniPorts))
                {
                almStatRep->almObj.instNum.sNum.portNum = OAM_HTONS(port + 1); 
                }

            if (CtcAlmIdValid(obj, alarmId))
                {
                if (msg->subtype == OamCtcExtEvtStatReq)
                    {
                    almStatRep->stat =
                        (CtcAlmAdminStateGet(port, (OamCtcAlarmId)alarmId) ?
                        OAM_HTONL(CtcAlmEnable) : OAM_HTONL(CtcAlmDisable));
                    }
                else
                    {
                    U16 index;
                    U32 stat = (CtcAlmAdminStateGet(port, (OamCtcAlarmId)alarmId) ?
                        CtcAlmEnable : CtcAlmDisable);

                    if (stat != OAM_NTOHL(almStatSet->stat))
                        {
                        switch(obj)
                            {
                            case OamCtcObjPort:
                                if (alarmId > OamCtcPortAlarmEnd)
                                    {
                                    index = (alarmId - OamCtcPortAlarmEnd);
                                    ctc30EthAlmThd[port][index].CtcAlmFlag = FALSE;
                                    }
                                break;
                            case OamCtcObjPon:
                                if (alarmId > OamCtcPonIfAlarmEnd)
                                    {
                                    index = (alarmId - OamCtcPonIfAlarmEnd);
                                    ctc30AlmThd[index].CtcAlmFlag = FALSE;
                                    }
                                break;
                            default:
                                break;
                            }

                        }
                    CtcAlmAdminStateSet(port,(OamCtcAlarmId)alarmId,
                        (OAM_NTOHL(almStatSet->stat) == CtcAlmEnable ? OamCtcActionEnable:OamCtcActionDisable));
                        almStatRep->stat= almStatSet->stat;
                    }

                }
            else
                {
                almStatRep->stat = OAM_HTONL(CtcAlmInValid);
                }

            almStatRep++;
            entryCount++;
            }

        if (msg->subtype == OamCtcExtEvtStatReq)
            {
            almStatReq++;
            }
        else
            {
            almStatSet++;
            }
        }

    tx->num = OAM_HTONS(entryCount);
    StreamSkip(&oamParser.reply,(sizeof(OamCtcExtEvtPdu)+ (sizeof(OamCtcAlarmStat)*(entryCount))));
    OamTransmit();
    } // OamCtcHandleEvtExtStatReq

////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleEvtExtThrdReq:Process extended OAM for request message for alarm
///   threshold SUBTYPE = 0x04
 // Parameters:
/// \param source   source port
/// \param link     Link on which frame arrived
/// \param msg      pointer to the Event status OAM message Header.
///
/// +------+------+-----+-----+-------+-----+-----+-----+---+----+-----+
/// |      |      |     |     |       |     |     |     |   |    |     |
/// | DA   | SA   | 8809|0x03 | FLAGS |FE   |OUI  |FF   |SUB|ENT |     |
/// |      |      |     |     |       |     |     |     |TYP|NUM |     |
/// +------+------+-----+-----+-------+-----+-----+-----+---+----+-----+
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleEvtExtThrdReq (OamCtcExtEvtPdu BULK *msg)
    {

    U8              FAST i;
    TkOnuEthPort    port= TkOnuPortNotUsed;
    OamCtcExtEvtPdu BULK* FAST tx = (OamCtcExtEvtPdu BULK*)oamParser.reply.cur;
    OamCtcAlarmThrd BULK * FAST almThrdRep = (OamCtcAlarmThrd BULK *)(tx + 1);
    OamCtcAlarmObj BULK * FAST almThrdReq = (OamCtcAlarmObj BULK *)(msg + 1);
    TkOnuEthPort            startPort = TkOnuPortNotUsed;
    TkOnuEthPort            endPort = TkOnuPortNotUsed;
    OamCtcInstNumU      tmpInstNum;
    U8 entryCount = 0;

    tx->ext     = OamCtcEvent;
    tx->subtype = OamCtcExtEvtThrdRep;
    tx->num     = msg->num;

    for (i = 0; i < OAM_NTOHS(msg->num); i++)
        {
        tmpInstNum = almThrdReq->instNum;
        switch( OAM_NTOHS(almThrdReq->objType))
            {
            case OamCtcObjPort:
            if((tmpInstNum.sNum.portType == OamCtcPortEth) &&
                (OAM_NTOHS(tmpInstNum.sNum.portNum) == OamCtcAllUniPorts))
                {
                startPort = 0;
                endPort = UniCfgDbGetActivePortCount() ;
                }
            else
                {
                startPort  =(U8)(OAM_NTOHS(tmpInstNum.sNum.portNum)) -  (U8)(1U);
                if(startPort >=  UniCfgDbGetActivePortCount())
                    {
                    startPort = UniCfgDbGetActivePortCount() - 1 ;
                    }
                endPort = startPort +1;
                }
                break;
            default:
                startPort  =0 ;
                endPort = startPort + 1;
                break;
            }

        for (port = startPort; port < endPort; port++  )
            {
            memcpy(&almThrdRep->almObj, (almThrdReq), sizeof(OamCtcAlarmObj));

            if (CtcAlmIdValid((OamCtcObjType)OAM_NTOHS(almThrdReq->objType), OAM_NTOHS(almThrdReq->almID)))
                {
                switch(OAM_NTOHS(almThrdReq->objType))
                    {
                    case OamCtcObjPort:
                        if (OAM_NTOHS(almThrdReq->almID) > OamCtcPortAlarmEnd)
                            {
                            almThrdRep->raiseVal =
                                OAM_HTONL(ctc30EthAlmThd[port][OAM_NTOHS(almThrdReq->almID) - OamCtcPortAlarmEnd].CtcAlmRaise);
                            almThrdRep->clearVal =
                                OAM_HTONL(ctc30EthAlmThd[port][OAM_NTOHS(almThrdReq->almID) - OamCtcPortAlarmEnd].CtcAlmClear);
                            }
                        else
                            {
                            almThrdRep->raiseVal = OAM_HTONL(CtcAlmInValid);
                            almThrdRep->clearVal = OAM_HTONL(CtcAlmInValid);
                            }

               
                        if((tmpInstNum.sNum.portType == OamCtcPortEth) &&
                            (OAM_NTOHS(tmpInstNum.sNum.portNum) == OamCtcAllUniPorts))
                            {
                            almThrdRep->almObj.instNum.sNum.portNum = OAM_HTONS(port + 1); 
                            }
                        break;
                    case OamCtcObjPon:
                        if (OAM_NTOHS(almThrdReq->almID) <= OamCtcPonIfAlarmEnd)
                            {
                            OamCtcTlvPowerMonThd BULK thd;
                            CtcOptDiagGetThd(OAM_NTOHS(almThrdReq->almID),&thd);
                            almThrdRep->raiseVal = OAM_HTONL(thd.thdRaise);
                            almThrdRep->clearVal = OAM_HTONL(thd.thdClear);
                            }
                        else
                            {
                            almThrdRep->raiseVal = 
                                OAM_HTONL(ctc30AlmThd[OAM_NTOHS(almThrdReq->almID) - OamCtcPonIfAlarmEnd].CtcAlmRaise);
                            almThrdRep->clearVal = 
                                OAM_HTONL(ctc30AlmThd[OAM_NTOHS(almThrdReq->almID) - OamCtcPonIfAlarmEnd].CtcAlmClear);
                            }
                        break;
                    case OamCtcObjOnu:
                        if ((OAM_NTOHS(almThrdReq->almID) >= OamCtcPonIfAlarmBase) &&
                            (OAM_NTOHS(almThrdReq->almID) <= OamCtcPonIfAlarmEnd))
                            {
                            OamCtcTlvPowerMonThd BULK thd;
                            CtcOptDiagGetThd(OAM_NTOHS(almThrdReq->almID),&thd);
                            almThrdRep->raiseVal = OAM_HTONL(thd.thdRaise);
                            almThrdRep->clearVal = OAM_HTONL(thd.thdClear);
                            }
                        else if ((almThrdReq->almID >= OamCtcAttrOnuTempHighAlarm) &&
                            (almThrdReq->almID <= OamCtcAttrOnuTempLowAlarm))
                            {
                            U32 thdRaise;
                            U32 thdClear;
                            GetOnuTempThd(OAM_NTOHS(almThrdReq->almID),&thdRaise,&thdClear);
                            almThrdRep->raiseVal = OAM_HTONL(thdRaise);
                            almThrdRep->clearVal = OAM_HTONL(thdClear);
                            }	
                        else
                            {
                            almThrdRep->raiseVal = OAM_HTONL(CtcAlmInValid);
                            almThrdRep->clearVal = OAM_HTONL(CtcAlmInValid);
                            }
                        break;
                    case OamCtcObjCard:
                    case OamCtcObjLlid:
                    default:
                        {
                        almThrdRep->raiseVal = OAM_HTONL(CtcAlmInValid);
                        almThrdRep->clearVal = OAM_HTONL(CtcAlmInValid);
                        }
                    }
                }
            else
                {
                if((tmpInstNum.sNum.portType == OamCtcPortEth) &&
                    (OAM_NTOHS(tmpInstNum.sNum.portNum) == OamCtcAllUniPorts))
                    {
                    almThrdRep->almObj.instNum.sNum.portNum = OAM_HTONS(port + 1); 
                    }
                entryCount ++;
                almThrdRep->raiseVal = OAM_HTONL(CtcAlmInValid);
                almThrdRep->clearVal = OAM_HTONL(CtcAlmInValid);
                break;
                }

            entryCount++;
            almThrdRep++;
            }

         almThrdReq++;
        }

    tx->num     = OAM_HTONS(entryCount);
    StreamSkip(&oamParser.reply,(sizeof(OamCtcExtEvtPdu)+ (sizeof(OamCtcAlarmThrd)*(entryCount))));
    OamTransmit();
    } // OamCtcHandleEvtExtThrdReq

////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleEvtExtThrdSet:  Process extended OAM for set message for alarm
///   threshold SUBTYPE = 0x05
 // Parameters:
/// \param source   source port
/// \param link     Link on which frame arrived
/// \param msg      pointer to the Event status OAM message Header.
///
/// +------+------+-----+-----+-------+-----+-----+-----+---+----+-----+
/// |      |      |     |     |       |     |     |     |   |    |     |
/// | DA   | SA   | 8809|0x03 | FLAGS |FE   |OUI  |FF   |SUB|ENT |     |
/// |      |      |     |     |       |     |     |     |TYP|NUM |     |
/// +------+------+-----+-----+-------+-----+-----+-----+---+----+-----+
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleEvtExtThrdSet (OamCtcExtEvtPdu BULK *msg)
    {
    U8              i;
    TkOnuEthPort    port = TkOnuPortNotUsed;
    OamCtcExtEvtPdu * tx = (OamCtcExtEvtPdu *)oamParser.reply.cur;
    OamCtcAlarmThrd * almThrdRep = (OamCtcAlarmThrd *)(tx + 1);
    OamCtcAlarmThrd * almThrdSet = (OamCtcAlarmThrd *)(msg + 1);
    TkOnuEthPort            startPort = TkOnuPortNotUsed;
    TkOnuEthPort            endPort = TkOnuPortNotUsed;
    OamCtcInstNumU      tmpInstNum;
    U8 entryCount = 0;

    tx->ext     = OamCtcEvent;
    tx->subtype = OamCtcExtEvtThrdRep;
    tx->num     = msg->num;

    cmsLog_notice("enter");

    for (i = 0; i < OAM_NTOHS(msg->num); i++)
    {
        tmpInstNum = almThrdSet->almObj.instNum;

        switch( OAM_NTOHS(almThrdSet->almObj.objType))
            {
            case OamCtcObjPort:
                if((tmpInstNum.sNum.portType == OamCtcPortEth) &&
                    (OAM_NTOHS(tmpInstNum.sNum.portNum) == OamCtcAllUniPorts))
                    {
                    startPort = 0;
                    endPort = UniCfgDbGetActivePortCount() ;
                    }
                else
                    {
                    startPort  =(U8)(OAM_NTOHS(tmpInstNum.sNum.portNum)) -  (U8)(1U);
                    if(startPort >=  UniCfgDbGetActivePortCount())
                        {
                        startPort = UniCfgDbGetActivePortCount() - 1;
                        }
                    endPort = startPort + 1;
                    }
                break;
            default:
                startPort  =0 ;
                endPort = startPort + 1;
                break;
            }

        for (port = startPort; port < endPort; port++  )
        {
            memcpy(&almThrdRep->almObj, &almThrdSet->almObj,
            sizeof(OamCtcAlarmObj));
            almThrdRep->clearVal = OAM_HTONL(CtcAlmInValid);
            almThrdRep->raiseVal = OAM_HTONL(CtcAlmInValid);
            if (CtcAlmIdValid((OamCtcObjType)OAM_NTOHS(almThrdSet->almObj.objType),
                OAM_NTOHS(almThrdSet->almObj.almID)))
                {
                switch(OAM_NTOHS(almThrdSet->almObj.objType))
                    {
                        case OamCtcObjPort:
                            if (OAM_NTOHS(almThrdSet->almObj.almID) > OamCtcPortAlarmEnd)
                                {
                                if ((U32)0 == OAM_NTOHL(almThrdSet->raiseVal))
                                    {
                                    almThrdSet->raiseVal = OAM_NTOHL(CtcAlmInValid);
                                    }
                                ctc30EthAlmThd[port][OAM_NTOHS(almThrdSet->almObj.almID) - OamCtcPortAlarmEnd].CtcAlmRaise
                                    = OAM_NTOHL(almThrdSet->raiseVal);
                                ctc30EthAlmThd[port][OAM_NTOHS(almThrdSet->almObj.almID) - OamCtcPortAlarmEnd].CtcAlmClear
                                    = OAM_NTOHL(almThrdSet->clearVal);
                                memcpy(almThrdRep, almThrdSet, sizeof(OamCtcAlarmThrd));
                                if((tmpInstNum.sNum.portType == OamCtcPortEth) &&
                                    (OAM_NTOHS(tmpInstNum.sNum.portNum) == OamCtcAllUniPorts))
                                    {
                                    almThrdRep->almObj.instNum.sNum.portNum = OAM_HTONS(port + 1); 
                                    }
                                }
                        break;
                        case OamCtcObjPon:
                            if (OAM_NTOHS(almThrdSet->almObj.almID) <= OamCtcPonIfAlarmEnd)
                                {
                                OamCtcTlvPowerMonThd thd;
                                thd.alarmId = OAM_NTOHS(almThrdSet->almObj.almID);
                                thd.thdRaise = OAM_NTOHL(almThrdSet->raiseVal);
                                thd.thdClear = OAM_NTOHL(almThrdSet->clearVal);
                                CtcOptDiagSetThd(&thd);
                                memcpy(almThrdRep, almThrdSet, sizeof(OamCtcAlarmThrd));
                                }
                            else
                                {
                                if (0 == OAM_NTOHL(almThrdSet->raiseVal))
                                    {
                                    almThrdSet->raiseVal = OAM_HTONL(CtcAlmInValid);
                                    }
                                ctc30AlmThd[OAM_NTOHS(almThrdSet->almObj.almID) - OamCtcPonIfAlarmEnd].CtcAlmRaise
                                    = OAM_NTOHL(almThrdSet->raiseVal);
                                ctc30AlmThd[OAM_NTOHS(almThrdSet->almObj.almID) - OamCtcPonIfAlarmEnd].CtcAlmClear
                                    = OAM_NTOHL(almThrdSet->clearVal);
                                memcpy(almThrdRep, almThrdSet, sizeof(OamCtcAlarmThrd));
                                }
                            break;
                        case OamCtcObjOnu:
                            if ((OAM_NTOHS(almThrdSet->almObj.almID) >= OamCtcPonIfAlarmBase) &&
                                (OAM_NTOHS(almThrdSet->almObj.almID) <= OamCtcPonIfAlarmEnd))
                                {
                                    OamCtcTlvPowerMonThd thd;
                                    thd.alarmId = OAM_NTOHS(almThrdSet->almObj.almID);
                                    thd.thdRaise = OAM_NTOHL(almThrdSet->raiseVal);
                                    thd.thdClear = OAM_NTOHL(almThrdSet->clearVal);
                                    CtcOptDiagSetThd(&thd);
                                    memcpy(almThrdRep, almThrdSet, sizeof(OamCtcAlarmThrd));
                                }
                            else  if ((OAM_NTOHS(almThrdSet->almObj.almID) >= OamCtcAttrOnuTempHighAlarm) &&
                                (OAM_NTOHS(almThrdSet->almObj.almID) <= OamCtcAttrOnuTempLowAlarm))
                                {
                                    SetOnuTempThd(OAM_NTOHS(almThrdSet->almObj.almID),OAM_NTOHL(almThrdSet->raiseVal),OAM_NTOHL(almThrdSet->clearVal));
                                    memcpy(almThrdRep, almThrdSet, sizeof(OamCtcAlarmThrd));
                                }
                            else
                                {
                                }
                            break;
                    case OamCtcObjCard:
                    case OamCtcObjLlid:
                    default:
                        break;
                    }
                }
            else
                {
                entryCount++;
                break;
                }
            entryCount++;
            almThrdRep++;
            }

        almThrdSet++;
    }

    tx->num     = OAM_HTONS(entryCount);
    StreamSkip(&oamParser.reply,(sizeof(OamCtcExtEvtPdu)+ (sizeof(OamCtcAlarmThrd)*(entryCount))));
    OamTransmit();
    } // OamCtcHandleEvtExtThrdSet


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle CTC Event extending OAM
///
/// This function handles all incoming China Telecom extended OAM messages that
/// is for CTC Event OAM. The message type is 0xFF. Subtype is 0x01 to 0x06
///
/// \param msg CTC extension body from message
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcHandleEventExt (OamCtcExtEvtPdu BULK *msg)
    {
    cmsLog_notice("subtype=0x%x", msg->subtype);

    switch (msg->subtype)
        {
        case OamCtcExtEvtStatReq:
        case OamCtcExtEvtStatSet:
            OamCtcHandleEvtExtStat(msg);
            break;

        case OamCtcExtEvtThrdReq:
            OamCtcHandleEvtExtThrdReq(msg);
            break;

        case OamCtcExtEvtThrdSet:
            OamCtcHandleEvtExtThrdSet(msg);
            break;

        default:
            break;
        }

    return;
    }


//##############################################################################
// CTC OAM Module Entry
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// CtcLinksInit - Initialize all the links oam management record
///
/// This function initalizes all the links management record to the default
//  state.The default state is unnegotiated with version 0 and no extended OUIs.
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CtcLinksInit(void)
    {
    U8 FAST i;
    for (i = 0; i < TkOnuNumTxLlids; ++i)
        {
        CtcLinkInit(i);
        }
    } // CtcLinksInit


////////////////////////////////////////////////////////////////////////////////
/// \brief  Load default CTC queue sizes from FDS
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcInitQueueSizes(void)
    {
    memset(&ctcUpQueueSizes, 0x0, sizeof(ctcUpQueueSizes));
    //get default queue config to ctcUpQueueSizes    
        {
        U8 FAST i;
        ctcUpQueueSizes.queueCount = MaxCtcQueueSizes;
        for(i = 0; i < MaxCtcQueueSizes; i++)
            {
            ctcUpQueueSizes.ctcUpQueueSize[i] =
                                   (CtcQueueSize)CtcUpQueueSizeDefault;
            }

        }
    } // CtcInitQueueSizes


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcAllLinksDown(void)
    {  
    //OntDirFifoDefaultSet();
    //OntDirWarmInit();
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcTimerHandle(OntmTimerId timerId)
    {
    switch (timerId & OntmTimerIdMsk)
        {
        case OamCtcPowerSaveTimer:
            OamCtcPowerSaveTimerExpire();
            break;

        default :
            break;
        }
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcPoll(void)
    {
    CtcFwUpgradePoll();
    OamCtcPowerSavePoll();
    }

#ifdef EPON_SFU
////////////////////////////////////////////////////////////////////////////////
/// CtcMacAgeTimeInit - set a default Mac_Age_Time under EPON SFU
///
/// defaultAgeTime should be 300s.
/// but system will check ageing_time twice,so we use half of it to set driver.  
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////

static void CtcMacAgeTimeInit(void)
    {
    U32 defaultAgeTime = 150*100;
    FILE* f = NULL;

    f = fopen("/sys/class/net/br0/bridge/ageing_time", "w");
    if (f <= 0)
        {
        fclose(f);
        return;
        }

    fprintf(f, "%u\n", defaultAgeTime);
    
    fclose(f);

    return;
    }
#endif
////////////////////////////////////////////////////////////////////////////////
/// OamCtcInit - Initialize the CTC module
///
/// This function intializes all of the modules needed for China Telecom OAM
/// operation.
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcInit (void)
    {
#if OAM_FULLSUPPORT
    U8 FAST passthruPort;
#endif
    OamCtcAllLinksDown();
    CtcLinksInit();
    OamCtcEncryptionInit ();
    CtcFwUpgradeInit();
    CtcMLlidInit();
    CtcClassificationInit();
    CtcEthControlInit();
    CtcVlanInit ();
    CtcMcastInit ();
    OamCtcPowerSaveInit();
    AlarmInit();
#if OAM_FULLSUPPORT
    if (passthruPort && passthruPort <= MAX_UNI_PORTS)
        {
        SetUpOamCaptureRule (passthruPort - 1);
        }
#endif
    OamCtcEvtInit();
#ifdef EPON_SFU  
    CtcMacAgeTimeInit();
#endif
    } // OamCtcInit


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcHandle (LinkIndex link, U8 BULK* msg)
    {
    OamCtcOpcode opcode = (OamCtcOpcode)StreamReadU8(&oamParser.src);

    cmsLog_notice("opcode = %d", opcode);

    switch (opcode)
        {
        case OamCtcExtVarRequest:
            OamCtcHandleExtVarReq();
            break;

        case OamCtcSetRequest:
            OamCtcHandleSetRequest();
            break;

        case OamCtcAuthentication:
            OamCtcHandleAuth ((OamCtcAuthHead BULK *)msg);
            break;

        case OamCtcChurning:
            OamCtcHandleChurning (link, (OamCtcChurningPdu BULK *)msg);
            break;

        case OamCtcDba:
            OamCtcHandleDba (link, (OamCtcDbaPdu BULK *)msg);
            break;

        case OamCtcFileUpgrade:
            OamCtcHandleFileUpgrade((OamCtcPayloadHead BULK *)(msg+1));
            break;

        case OamCtcEvent:
            OamCtcHandleEventExt((OamCtcExtEvtPdu BULK *)msg);
            break;

        case OamCtcReserved:
        default:
            cmsLog_notice("unknown opcode!");
            break;
        }
    } // OamCtcHandle


// End of File CtcOnuOam.c

