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
/// \file CtcOamGet.c
/// \brief China Telecom extended OAM get module
/// \author Jason Armstrong
/// \date March 1, 2006
///
///
////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include "Teknovus.h"
#include "Build.h"
#include "EponDevInfo.h"
#include "Holdover.h"
#include "Stream.h"
#include "UniConfigDb.h"
#include "OntConfigDb.h"
#include "OntmMpcp.h"
#include "PonManager.h"
#include "Oam.h"
#include "OamGet.h"
#include "OamSet.h"
#include "OamOnu.h"
#include "CtcOam.h"
#include "OamUtil.h"
#include "CtcOnuUtils.h"
#include "CtcOnuOam.h"
#include "CtcVlan.h"
#include "CtcMLlid.h"
#include "CtcClassification.h"
#include "CtcEthControl.h"
#include "CtcAlarms.h"
#include "CtcOamDiscovery.h"
#include "OnuOs.h"
#include "CtcOptDiag.h"
#include "CtcStats.h"
#include "CtcMcast.h"

#include "OamCtcPowerSave.h"
#include "eponctl_api.h"
#include "ethswctl_api.h"

#include "CucOam.h"
#include "MacLearning.h"

#ifdef BRCM_CMS_BUILD
#include "cms_util.h"
#include "cms_core.h"
#endif

#include "cms_log.h"

////////////////////////////////////////////////////////////////////////////////
/// OamCucGetOnuCfgCnt - Get CUC ONU configuration Counter
///
 // Parameters:
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCucGetOnuCfgCnt (OamVarContainer BULK *cont)
    {
#ifdef BRCM_CMS_BUILD
    U32 *cfgCnt = (U32 *)cont->value;
    XponObject *obj = NULL;   
    InstanceIdStack xponiidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;

    if ((ret = cmsLck_acquireLock()) != CMSRET_SUCCESS)
        {
        cmsLog_error("failed to get lock, ret=%d", ret);
        cont->length = OamVarErrActNoResources;
        return;
        }
    
    if ((ret = cmsObj_get(MDMOID_XPON, &xponiidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
        {
        cmsLog_error("get of MDMOID_IGD_DEVICE_INFO object failed, ret=%d", ret);
        cmsLck_releaseLock();
        cont->length = OamVarErrActNoResources;
        return;
        }
	
    *cfgCnt = OAM_HTONL(obj->cucCfgCnt);
    cont->length = sizeof(U32);

    cmsObj_free((void **) &obj);
    cmsLck_releaseLock();  
#else
    cont->length = OamVarErrActNoResources;
#endif
    }


#ifdef DMP_TAENDPOINT_1			
////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetVoipPort - Get ONU Voip Pots status
///
 // Parameters:
/// \param port
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetVoipPort  (TkOnuEthPort port, OamVarContainer BULK *cont)
    {
#ifdef BRCM_CMS_BUILD
#ifdef DMP_VOICE_SERVICE_1
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    VoiceLineObject* voiceLineObj;

    cmsLog_debug("port:%d", port);

   PUSH_INSTANCE_ID( &iidStack, 1);
   PUSH_INSTANCE_ID( &iidStack, 1);
   PUSH_INSTANCE_ID( &iidStack, port);
    if((ret = cmsObj_get(MDMOID_VOICE_LINE, &iidStack, 0, (void **) &voiceLineObj)) != CMSRET_SUCCESS)
       {
       cmsLog_error("could not get  MDMOID_VOICE_LINE, ret=%d", ret);
       cont->length = OamVarErrActNoResources;
       return;
        }
    
   cmsLog_debug("enable:%s", voiceLineObj->enable);
   
    ContPutU8 (cont,
        (U8)(strcmp(voiceLineObj->enable, MDMVS_ENABLED) ? 0 : 1));
	
    cmsObj_free((void **) &voiceLineObj);
#else
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack searchIidStack = EMPTY_INSTANCE_ID_STACK;
    void* obj;

    cmsLog_debug("port:%d", port);

    if((ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
    {
       cmsLog_error("could not get MDMOID_VOICE, ret=%d", ret);
       cont->length = OamVarErrActNoResources;
       return;
    }
    cmsObj_free(&obj);
   
    for( int i = port; i >= 0 ; i-- )
    {
        ret = cmsObj_getNextInSubTreeFlags(MDMOID_SIP_CLIENT, &iidStack, &searchIidStack, OGF_NO_VALUE_UPDATE, (void **)&obj);
        if( ret == CMSRET_SUCCESS ){
            cmsObj_free((void **)&obj );
        }
        else{
            break;
        }
    }

    if( ret != CMSRET_SUCCESS )
    {
       cmsLog_error("could not get MDMOID_SIP_CLIENT, ret=%d", ret);
       cont->length = OamVarErrActNoResources;
       return;
    }

    cmsObj_get(MDMOID_SIP_CLIENT, &searchIidStack, OGF_NO_VALUE_UPDATE, (void **)&obj);

    cmsLog_debug("enable:%s", ((SipClientObject *)obj)->enable? "enabled":"disabled");
   
    ContPutU8 (cont, (U8)(((SipClientObject *)obj)->enable? 0 : 1));
	
    cmsObj_free((void **) &obj);
#endif /* DMP_VOICE_SERVICE_1 */
#else
    cont->length = OamVarErrActNoResources;
#endif
    } // OamCtcGetEthLinkState
#endif    

////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetOnuSn - Get ONU Serial Number
///
/// \param obj      Object to be get information from
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetOnuSn (OamVarContainer BULK *cont)
    {
    OamCtcTlvOnuSn BULK *value;
    U8 const * vendor = (U8 *)EponDevInfoExtendedId();

    cont->length = sizeof (OamCtcTlvOnuSn);
    value = (OamCtcTlvOnuSn BULK *)(&cont->value[0]);

    memcpy(&value->vendorId, &vendor[CtcOamVendorIdIndex], 4);
    memcpy(&value->onuModel, &vendor[CtcOamOnuModelIndex], 4);


    memcpy(&value->onuId, (U8 *)&EponDevBaseMacAddr, sizeof(MacAddr));
    memcpy(value->hardVersion,
           CtcStringSwap(&vendor[CtcOamHardwareVerIndex], 8), 8);
    memcpy(value->softVersion,
           CtcStringSwap(&vendor[CtcOamSoftwareVerIndex], 16), 16);

    if (OamIeeeIsVendorNeg(0, OuiCtc) && 
        CtcOamNegotiatedVersion() > CtcOamVer21)
        {
        memcpy(value->ExtOnuModel,
           CtcStringSwap(&vendor[CtcOamOnuExtOnuModel], 16), 16);
        }
    else
        {
        //before CTC 3.0 or under CUC, do not include this field.
        cont->length -= sizeof(value->ExtOnuModel);
        }
    } // OamCtcGetOnuSn


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetFirmwareVer - Get ONU firmware version information
///
 // Parameters:
/// \param obj      Object to be get information from
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetFirmwareVer (OamVarContainer BULK *cont)
    {
    OamCtcTlvFirmwareVer *value;

    cont->length = sizeof(EponDevFirmwareVer);
    value = (OamCtcTlvFirmwareVer BULK *)(&cont->value[0]);

    value->version = OAM_HTONS(EponDevFirmwareVer);
    } // OamCtcGetFirmwareVer


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetChipsetId - Get ONU chipset ID
///
 // Parameters:
/// \param obj      Object to be get information from
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetChipsetId (OamVarContainer BULK *cont)
    {
    OamCtcTlvChipsetId *value;

    cont->length = sizeof(*value);
    value = (OamCtcTlvChipsetId *)(&cont->value[0]);

    value->vendorId = OAM_HTONS((TkJedecId)EponDevChipId.vendorId);
    value->chipModel = OAM_HTONS((TkChipId)EponDevChipId.chipModel);
    value->revisionDate = OAM_HTONL((TkChipVersion)EponDevChipId.revision);
    } // OamCtcGetChipsetId


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetOnuCap - Get ONU capabilities
///
 // Parameters:
/// \param obj      Object to be get information from
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetOnuCap (OamVarContainer BULK *cont)
    {
    OamCtcTlvOnuCap BULK *value;
    TkOnuEthPort FAST port;
    Port BULK p;
    U8 const FAR* vendor = EponDevInfoExtendedId();

    cont->length = sizeof (OamCtcTlvOnuCap);
    value = (OamCtcTlvOnuCap BULK *)(&cont->value[0]);
    memset(value, 0 , sizeof(OamCtcTlvOnuCap));

    for (port = 0; port < UniCfgDbGetActivePortCount(); port++)
        {
            if ((EponDevPortConfig(port) & TkOnuEthPortCfgEnable) != 0)
            {
            if ((EponDevPortCapability(port) & EthPort1GSpeed) != 0)
                {
                value->services |= OamCtcServSupportGe;
                value->numGePorts++;
                value->geBitmap[7] |= (1 << bcm_enet_map_unit_port_to_oam_idx(0, port));
                }
            else
                {
                value->services |= OamCtcServSupportFe;
                value->numFePorts++;
                value->feBitmap[7] |= (1 << bcm_enet_map_unit_port_to_oam_idx(0, port));
                }
            }
        }

    value->numPotsPorts = vendor[CtcOamPotsPortCountIndex];
    if (value->numPotsPorts != 0)
        {
        value->services |= OamCtcServSupportVoip;
        }

    value->numE1Ports = vendor[CtcOamE1PortCountIndex];
    if (value->numE1Ports != 0)
        {
        value->services |= OamCtcServSupportTdm;
        }

    p.pift = PortIfLink;
    for (p.inst = 0; p.inst < OntCfgDbGetNumUserLinks(); ++p.inst)
        {
        value->numUpQueue++;
        value->numDnQueue++;
        }

    value->upQueueMax  = TkOnuNumUpQueues;
	value->dnQueueMax  = TkOnuNumDnQueues;
   
    value->batteryBack = 0;

    } // OamCtcGetOnuCap


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetOnuCap2 - Get ONU capabilities2
///
 // Parameters:
/// \param obj      Object to be get information from
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetOnuCap2 (OamVarContainer BULK *cont)
    {
    OamCtcTlvOnuCap2  *value;
    OamCtcTlvOnuIf    * FAST pIfs;
    U8 gePort = 0;
    U8 fePort = 0;

    // This is the super lightweight version of cap2 that can only work on the
    // 10G FPGA boards.  Most of the information in the structure is 0 so only
    // non-zero values are set here.  We don't have CTC multiple LLIDs yet so we
    // have to report as a single LLID system.  There is no 10G port option in
    // the CTC spec so we will report the UNI as a 1G.
    value = (OamCtcTlvOnuCap2 BULK *)(&cont->value[0]);
    memset(value, 0x0, sizeof(OamCtcTlvOnuCap2));
    pIfs = value->ifs;
    
    value->onuType = OAM_HTONL(EponDevOnuType); 
    value->numPonIf = 1;
    value->multiLlid = CtcMLlidSupport;

    gePort = EponDevGePortNum;
    fePort = EponDevFePortNum;

    if (gePort > 0)
        {
        pIfs->ifType = OAM_HTONL(OamCtcOnuIfGe);
        pIfs->numPort = OAM_HTONS(gePort);
        pIfs++;
        value->numIfType++;
        }
    if(fePort > 0)
        {
        pIfs->ifType = OAM_HTONL(OamCtcOnuIfFe);
        pIfs->numPort = OAM_HTONS(fePort);
        pIfs++;
        value->numIfType++;
        }

    *(U8 BULK *)pIfs = 0; // Battery Backup not support
    
    cont->length = (U8)(PointerDiff(pIfs, value) + 1);
    } // OamCtcGetOnuCap2


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetOnuCap3 - Get ONU capabilities3
///
 // Parameters:
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetOnuCap3 (OamVarContainer BULK *cont)
    {
    OamCtcTlvOnuCap3 BULK *value;

    cont->length = sizeof(OamCtcTlvOnuCap3);
    value = (OamCtcTlvOnuCap3 BULK *)(&cont->value[0]);
    value->ip6Supported = FALSE;
    value->powerCtrl = OamCtcPowerIndepentCtrl;

    if (OamIeeeIsVendorNeg(0, OuiCuc))
        {
        cont->length = sizeof(OamCtcTlvOnuCap3old);
        }
    else
        {
        value->ServiceSla = PonMgrPriCountGet();
        } 
    } // OamCtcGetOnuCap3



////////////////////////////////////////////////////////////////////////////////
/// \brief  Get CTC service SLA
///
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcServiceSlaGet(OamVarContainer BULK * cont)
    {
    OamCtcServiceSla BULK * sla = (OamCtcServiceSla BULK *)cont->value;

    //NvsRead(NvsRecIdCtcServiceSla, sla, sizeof(OamCtcServiceSla) +
    //        (sizeof(OamCtcService) * (8 - 1)));
    switch (sla->op)
        {
        case OamCtcServiceDbaOpActivate:
            cont->length = sizeof(OamCtcServiceSla) +
                (U8)(sizeof(OamCtcService) * (sla->numServices - 1));
            break;

        case OamCtcServiceDbaOpDeactivate:
            cont->length = 1;
            break;

        default:
            cont->length = OamVarErrActBadParameters;
        }
    } // OamCtcServiceSlaGet


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetHoldover - Get CTC holdover params
///
/// Parameters:
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetHoldover (OamVarContainer BULK *cont)
    {
    OamCtcHoldover BULK * pHoldover = (OamCtcHoldover BULK *)(cont->value);

    cont->length = sizeof (OamCtcHoldover);
    // Set the CTC disable/enable flags based on whether we have a
    // non-zero time provisioned
    pHoldover->holdoverFlag = (HoldoverIsEnabled() ?
                               OAM_HTONL(OamCtcActionEnable) : OAM_HTONL(OamCtcActionDisable));
    pHoldover->holdoverTime = OAM_HTONL(HoldoverGetTime());
    } // OamCtcGetHoldover


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get active PON_IF admin state
///
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetActPonIfAdmin (OamVarContainer BULK *cont)
    {
        cont->length = OamVarErrActNoResources;
    } // OamCtcGetActPonIfAdmin


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetAlarmAdminState - Get Ctc2.1 alarm admin state
///
/// Parameters:
/// \param src      Container to place receive information
/// \param cont     Container to place reply information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetAlarmAdminState (OamCtcObjType obj,
                               TkOnuEthPort port,
                               const OamVarContainer BULK* src,
                               OamVarContainer BULK *cont)
    {
    OamCtcTlvAlarmAdminState BULK *value;
    Stream FAST strm;
    U16 alarmId = 0;

    cont->length = OamVarErrActBadParameters;
    value = (OamCtcTlvAlarmAdminState BULK *)(&cont->value[0]);
    if(src->length != sizeof(U16))
        {
        return;
        }

    memset(value, 0 ,sizeof(OamCtcTlvAlarmAdminState));
    StreamInit(&strm, (U8 *)src->value);
    alarmId = StreamPeekU16(&strm);
    value->alarmId = OAM_HTONS(alarmId);

    if(CtcAlmIdValid(obj, alarmId))
        {
        value->config = (CtcAlmAdminStateGet(port,
            (OamCtcAlarmId)alarmId) ? OAM_HTONL(OamCtcAlarmEnable)
            : OAM_HTONL(OamCtcAlarmDisable));
        cont->length = sizeof(OamCtcTlvAlarmAdminState);
        }
    } // OamCtcGetAlarmAdminState


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetPowerMonDiag - Get power monitor diagnosis value
///
/// Parameters:
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetPowerMonDiag (OamVarContainer *cont)
    {
    OamCtcTlvPowerMonDiag * ctcDiagVal =
        (OamCtcTlvPowerMonDiag BULK *)&cont->value[0];

    U16    temp     = 0;
    U16    vcc      = 0;
    U16    txBias   = 0;
    U16    txPower  = 0;
    U16    rxPower  = 0;
    
    if (optical_temp(&temp) &&
        optical_vcc(&vcc) &&
        optical_bias(&txBias) &&
        optical_txpower(&txPower) &&
        optical_rxpower(&rxPower))
        {
        ctcDiagVal->temp = OAM_HTONS(temp);
        ctcDiagVal->vcc  = OAM_HTONS(vcc);
        ctcDiagVal->txBias = OAM_HTONS(txBias);
        ctcDiagVal->txPower = OAM_HTONS(txPower);
        ctcDiagVal->rxPower = OAM_HTONS(rxPower);
        
        cont->length = sizeof(OamCtcTlvPowerMonDiag);
        }
    else
        {
        cont->length = OamVarErrActBadParameters;
        }

    } // OamCtcGetPowerMonDiag


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetPowerMonThd - Get power monitor threshold value
///
/// Parameters:
/// \param src      pointer to var descriptors in request
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetPowerMonThd (const OamVarContainer * src,
                           OamVarContainer *cont)
    {
    OamCtcTlvPowerMonThd * value;
    OamCtcTlvPowerMonThd   thd;
    Stream strm;
    U16 alarmId = 0;    

    value = (OamCtcTlvPowerMonThd BULK *)(&cont->value[0]);
    memset(value, 0 ,sizeof(OamCtcTlvPowerMonThd));
    StreamInit(&strm, (U8 *)src->value);
    alarmId = StreamPeekU16(&strm);
    value->alarmId = OAM_HTONS(alarmId);

    cont->length = OamVarErrActBadParameters;

    if ((src->length != sizeof(U16)) ||
        (alarmId > OamCtcAttrPowerMonTempWarnLow))
        {
        cont->length = OamVarErrActBadParameters;
        }
    else
        {
        CtcOptDiagGetThd(alarmId, &thd);
        value->thdRaise = OAM_HTONL(thd.thdRaise);
        value->thdClear = OAM_HTONL(thd.thdClear);
        cont->length = sizeof(OamCtcTlvPowerMonThd);
        }
    } // OamCtcGetPowerMonThd


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetEthLinkState - Get ONU ethernet port link status
///
 // Parameters:
/// \param port
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetEthLinkState (TkOnuEthPort port, OamVarContainer BULK *cont)
    {
#if !OAM_SWITCH
	int linkStatus;
	bcm_get_linkstatus(0, bcm_enet_map_oam_idx_to_phys_port(port), &linkStatus);
    ContPutU8(cont, (U8)linkStatus);
#else
    ContPutU8 (cont,
        (U8)TkOnuUserPortGetLinkStatus (port));
#endif
    } // OamCtcGetEthLinkState


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetEthPortPause - Get ONU flow control status
///
 // Parameters:
/// \param obj      Object to be get information from
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetEthPortPause (TkOnuEthPort port, OamVarContainer BULK *cont)
    {
    BOOL pause = FALSE;
    cont->length = sizeof (OamCtcTlvEthPortPause);

    CtcEthControlPauseGet(port, &pause);
    ((OamCtcTlvEthPortPause BULK *)(&cont->value[0]))->enabled = pause;
    } // OamCtcGetEthPortPause


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetClassMarking - Get ONU classification provisioning
///
 // Parameters:
/// \param obj      Object to be get information from
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetClassMarking (TkOnuEthPort port, OamVarContainer BULK *cont)
    {
    if(port >= UniCfgDbGetActivePortCount())
        {
        cont->length = OamVarErrActNoResources;
        }
    else
        {
        cont = CtcClassificationGet(port, cont);
        }
    } // OamCtcGetClassMarking


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetEthUsPolice - ONU Ethernet port upstream policing
///
 // Parameters:
/// \param port     Which ether port to be get
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetEthUsPolice (TkOnuEthPort port, OamVarContainer BULK *cont)
    {
    CtcEthPortUsPolice BULK *conf;

    conf = (CtcEthPortUsPolice BULK *)(&cont->value[0]);
    cont->length = CtcEthControlUsPoliceCfgGet (port, conf);
    } // OamCtcGetEthUsPolice


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetEthDsShaper - ONU Ethernet port downstream shaping
///
 // Parameters:
/// \param port     Which ether port to be get
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetEthDsShaper (TkOnuEthPort port, OamVarContainer BULK *cont)
    {
    CtcEthPortDsShaper BULK *conf;

    conf = (CtcEthPortDsShaper BULK *)(&cont->value[0]);
    cont->length = CtcEthControlDsShaperCfgGet (port, conf);
    } // OamCtcGetEthDsShaper



////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetEthLoopDetect - ONU Ethernet port loop detect
///
 // Parameters:
/// \param port     Which ether port to be get
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetEthLoopDetect (TkOnuEthPort port, OamVarContainer BULK *cont)
    {
#if OAM_RSTP
    {
            int  state = 0;

            FILE* f = fopen("/sys/class/net/br0/bridge/stp_state", "r");
            if(f){
                fscanf(f, "%d", &state);
                fclose(f);
                ContPutU32(cont, state ?OamCtcActionEnable : OamCtcActionDisable);
                return ;
            }
    }		
#endif
	cont->length = OamVarErrAttrUnsupported;
    } // OamCtcGetEthLoopDetect


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetEthDisableLooped - ONU Ethernet port disable looped
///
 // Parameters:
/// \param port     Which ether port to be get
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetEthDisableLooped (TkOnuEthPort port, OamVarContainer BULK *cont)
    {
	cont->length = OamVarErrAttrUnsupported;
    } // OamCtcGetEthDisableLooped


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetVlan - Get ONU VLAN provisioning information
///
 // Parameters:
/// \param obj      Object to be get information from
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetVlan (TkOnuEthPort port, OamVarContainer BULK *cont)
    {
    OamCtcTlvVlan BULK *value;

    value = (OamCtcTlvVlan BULK *)(&cont->value[0]);
    value->mode = CtcVlanGetMode (port);
    cont->length = (U8)(CtcVlanGetData (port, ((U8 BULK *)(value)) + 1));
    } // OamCtcGetVlan



////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetMcastSwitch - Get ONU IGMP snooping status
///
 // Parameters:
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetMcastSwitch (OamVarContainer BULK *cont)
{
    //if(CtcMcastAdminState())
    if(1)
    {
        ContPutU8(cont, CtcMcastSwitchGetMode());
    }
    else
    {
        cont->length = OamVarErrActNoResources;
    }
} 

////////////////////////////////////////////////////////////////////////////////
/// Get multicast VLAN for the port
///
 // Parameters:
/// \param port     Port to get
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetMcastVlan(U8 port, OamVarContainer BULK *cont)
{
    if(port < UniCfgDbGetActivePortCount())
    {
        CtcMcastVlanOpCfg  * cfg;
        cfg = (CtcMcastVlanOpCfg  *)(&cont->value[0]);
        cont->length = CtcMcastVlanGet(port, cfg);
    }
    else
    {
        cont->length = OamVarErrActBadParameters;
    }
} 

////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetMcastStrip - Get multicast vlan tag process for the port
///
 // Parameters:
/// \param port     Port to get
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetMcastStrip(U8 port, OamVarContainer BULK *cont)
{
    if(port < UniCfgDbGetActivePortCount())
    {
        CtcMcastTagOpCfg  * cfg;
        cfg = (CtcMcastTagOpCfg  *)(&cont->value[0]);
        cont->length = CtcMcastTagOpGet(port, cfg);
    }
    else
    {
        cont->length = OamVarErrActBadParameters;
    }
} 

////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetGroupMax - Get the maximum group number for the port
///
 // Parameters:
/// \param port     Port to get
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetGroupMax(U8 port, OamVarContainer BULK *cont)
{
    if(port < UniCfgDbGetActivePortCount())
    {
        OamCtcTlvGroupMax  *value;
        value = (OamCtcTlvGroupMax *)(&cont->value[0]);
        value->maxGroup = CtcMcastGroupMaxGet(port);
        cont->length = sizeof(value->maxGroup);
    }
    else
    {
        cont->length = OamVarErrActBadParameters;
    }
} 

////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetMcastCtrl - Get multicast control information
///
 // Parameters:
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static void OamCtcGetMcastCtrl (OamVarContainer *cont)
{
    if(!CtcMcastIsHostCtrlMode())
    {
        cont->length = OamVarErrActBadParameters;
        return;
    }

     CtcMcastCtrlGrpGet(cont);  //return len saved in cont->length
}

////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetFastLeaveAbility - Get the Fast-Leave ability
///
 // Parameters:
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetFastLeaveAbility(OamVarContainer BULK *cont)
{
    #if 0
    if(CtcMcastAdminState())
    #else
    if(1)
    #endif
    {
        Stream  ability;
        CtcOamVersion FAST ctcVer = CtcOamNegotiatedVersion();
        StreamInit(&ability, cont->value);
        StreamWriteU32(&ability, (U32)(
                                 (ctcVer >= CtcOamVer30)?
                                  Ctc30McastFastModeNums:
                                  Ctc21McastFastModeNums));
        StreamWriteU32(&ability, CtcMcastIgmpSnoopNoFast);      //FIXME: do we support these?
        StreamWriteU32(&ability, CtcMcastIgmpSnoopFastLeave);
        StreamWriteU32(&ability, CtcMcastHostCtlNoFast);
        StreamWriteU32(&ability, CtcMcastHostCtlFastLeave);
        if(ctcVer >= CtcOamVer30)
        {
            StreamWriteU32(&ability, CtcMcastMldSnoopNoFast);
            StreamWriteU32(&ability, CtcMcastMldSnoopFastLeave);
        }
        cont->length = (U8)StreamLengthInBytes(&ability);
    }
    else
    {
        cont->length = OamVarErrActBadParameters;
    }
}

////////////////////////////////////////////////////////////////////////////////
///Get the Fast-Leave ability
///
 // Parameters:
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetFastLeaveState(OamVarContainer BULK *cont)
{
    //if(CtcMcastAdminState())
    if(1)
    {
        ContPutU32(cont, (U32)CtcMcastFastLeaveStateGet());
    }
    else
    {
        cont->length = OamVarErrActBadParameters;
    }
}


////////////////////////////////////////////////////////////////////////////////
/// OamCucGetPhyType - Get Cuc Uni port PHY mode
///
 // Parameters:
/// \param port    port number
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// Pointer to last container
////////////////////////////////////////////////////////////////////////////////
static
void OamCucGetPhyType(TkOnuEthPort port, OamVarContainer BULK* cont)
    {
    int speed, duplex;

    if (bcm_phy_mode_get(0, bcm_enet_map_oam_idx_to_phys_port(port), &speed, &duplex))
        {
        cont->length = OamVarErrAttrHwFailure;
        }
    else
        {
        if (speed == 1000)
            {
            ContPutU8(cont, OamCucPhy1G);
            }
        else if (speed == 100)
            {
            ContPutU8(cont, (duplex == 0) ? OamCucPhy100MHalf : OamCucPhy100MFull);
            }
        else if (speed == 10)
            {
            ContPutU8(cont, (duplex == 0) ? OamCucPhy10MHalf : OamCucPhy10MFull);
            }
        else
            {
            cont->length = OamVarErrAttrUndetermined;
            }
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// OamCucGetMacLimit - Get Cuc mac aging time
///
 // Parameters:
/// \param port    port number
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// Pointer to last container
////////////////////////////////////////////////////////////////////////////////
static
void OamCucGetMacLimit (TkOnuEthPort port, OamVarContainer BULK* cont)
    {
    U16 tableSize;
   
        
    if(MacLearnGetMaxLimit("br0", port, &tableSize))
        {
        /* 0 means no limit, translate to 0xffff according to spec */
        if (tableSize == 0)
            tableSize = 0xFFFF;
        
        ContPutU16 (cont, tableSize);
        }
    else
        {
        cont->length = OamVarErrActBadParameters;
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetOnuProtParam - Get ONU Protection Parameters
///
 // Parameters:
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetOnuProtParam (OamVarContainer BULK *cont)
    {
    LosCheckPara los_para;
    OamCtcTlvOnuProtParam BULK *value =
        (OamCtcTlvOnuProtParam BULK *)(&cont->value[0]);
     
    eponStack_CtlCfgLosCheckTime(EponGetOpe, &los_para);    
    
    value->tLosOptical = OAM_HTONS(los_para.losopttime);
    value->tLosMpcp = OAM_HTONS(los_para.losmpcptime);
    cont->length = sizeof(OamCtcTlvOnuProtParam);
    }



////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetPfmStatus - Get Performance Monitor Status
///
 // Parameters:
/// \param port     port number
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetPfmStatus (TkOnuEthPort port, OamVarContainer BULK *cont)
    {
    OamCtcTlvPfmStatus BULK *value =
        (OamCtcTlvPfmStatus BULK *)(&cont->value[0]);

    cont->length = sizeof(OamCtcTlvPfmStatus);
   
    value->enable = (CtcStatsGetEnable (port) ?
        OAM_HTONS(OamCtcActionU16Enable) : OAM_HTONS(OamCtcActionU16Disable));
    value->period = OAM_HTONL(CtcStatsGetPeriod(port));
    
    } // OamCtcGetPfmStatus



////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetPfmData - Get Performance Monitor Data
///
 // Parameters:
/// \param his      history or current data flag
/// \param port     port number
/// \param cont     Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetPfmData (BOOL his,
                       TkOnuEthPort port,
                       OamVarContainer BULK *cont)
    {
    CtcStatId  id;
    Stream  strm;
    U64  statVal;

    cont->length = OamVarErrActNoResources;
    
    if (!CtcStatsGetEnable (port))
        {
        cmsLog_notice("stats gathering not enabled on port %d", port);    
        cont->length = OamVarErrActNoResources;
        }
    else
        {
        cont->length = 0;
        StreamInit(&strm, cont->value);
        CtcStatsGather(port);
        for (id = CtcStatIdFirst; id < CtcStatIdNums; id++)
            {
            statVal = 0;
            if (his)
                {
                CtcStatsGetHisStats (port, id, &statVal);
                }
            else
                {
                CtcStatsGetStats (port, id, &statVal);
                }
            StreamWriteU64(&strm, statVal);
            cont->length += sizeof(U64);
            if ((cont->length + sizeof(U64)) > 0x80)
                {
                cont->length = ((cont->length == 0x80) ? 0x00 : cont->length);
                StreamSkip(&oamParser.reply, OamContSize(cont));
                cont = OamCtcNextCont (cont);
                cont->length = 0x0;
                StreamInit(&strm, cont->value);
                }
            }
        }
        
    }//OamCtcGetPfmData

////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetMacAgingTime - Get Ctc mac aging time
///
 // Parameters:
/// \param src      Container to source information
/// \param cont     Container to place information
///
/// \return
/// Pointer to last container
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetMacAgingTime (OamVarContainer BULK *cont)
    {
    if (!macCtcAgingTimeSet)
    	{
        	//invalid value, just read the applied value
    	U64 macAgingTimeUl = 0;
    	
    	FILE* f = fopen("/sys/class/net/br0/bridge/ageing_time", "r");
    	fscanf(f, "%llu", &macAgingTimeUl);
    	fclose(f);

    	ContPutU32(cont, (U32)((float)macAgingTimeUl*2/100.0 + 0.5));
    	}
    else
    	{
    	ContPutU32(cont, macLearnCtcAgingTime);
    	}
    return ;
    }

////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleGetOnuAttr - Handle a get extended attribute branch
///
/// This function handles extended attribute requests.
///
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleGetOnuAttr (const OamVarDesc BULK* src,
                             OamVarContainer BULK* cont)
    {
    cmsLog_notice("leaf = 0x%x", OAM_NTOHS(src->leaf));
    switch (OAM_NTOHS(src->leaf))
        {
        case OamCtcAttrOnuSn:
            OamCtcGetOnuSn (cont);
            break;

        case OamCtcAttrFirmwareVer:
            OamCtcGetFirmwareVer (cont);
            break;

        case OamCtcAttrChipsetId:
            OamCtcGetChipsetId (cont);
            break;

        case OamCtcAttrOnuCap:
            OamCtcGetOnuCap (cont);
            break;

        case OamCtcAttrOnuCap2:
            OamCtcGetOnuCap2 (cont);
            break;

        case OamCtcAttrOnuCap3:
            OamCtcGetOnuCap3 (cont);
            break;

        case OamCtcAttrPowerMonDiagnosis:
            OamCtcGetPowerMonDiag(cont);
            break;
        
        case OamCtcAttrMcastSwitch:
            OamCtcGetMcastSwitch (cont);
            break;

        case OamCtcAttrMcastCtrl:
            OamCtcGetMcastCtrl (cont);
            break;

        case OamCtcAttrFastLeaveAbility:
            OamCtcGetFastLeaveAbility(cont);
            break;

        case OamCtcAttrFastLeaveState:
            OamCtcGetFastLeaveState(cont);
            break;

        case OamCtcAttrServiceSla:
            OamCtcServiceSlaGet(cont);
            break;

        case OamCtcAttrOnuHoldover:
            OamCtcGetHoldover(cont);
            break;

        case OamCtcAttrAlarmAdminState:
            OamCtcGetAlarmAdminState(OamCtcObjOnu,
                                     TkOnuPortNotUsed,
                                     (OamVarContainer BULK*)src,
                                     cont);
            break;

        case OamCtcAttrActPonIFAdmin:
            OamCtcGetActPonIfAdmin(cont);
            break;

        case OamCtcAttrOnuProtParam:
        // case OamCucAttrOnuCfgCnt:
            if (OamIeeeIsVendorNeg(0, OuiCuc))
                {
                OamCucGetOnuCfgCnt (cont);
                }
            else
                {
                OamCtcGetOnuProtParam (cont);
                }
            break;

        case OamCtcAttrPowerSavingCap:
            OamCtcPowerSaveCapGet(cont);
            break;

        case OamCtcAttrPowerSavingCfg:   //OamCucAttrOnuMacAgingTime:
            if (OamIeeeIsVendorNeg(0, OuiCtc))
                {
                OamCtcPowerSaveCfgGet(cont);
                }
            else
                {
                OamCtcGetMacAgingTime(cont);
                }
            break;

        case OamCtcAttrOnuMacAgingTime:
            OamCtcGetMacAgingTime(cont);
            break;
            
        case OamCucAttrOnuProtParam:
            if (OamIeeeIsVendorNeg(0, OuiCuc))
                {
                OamCtcGetOnuProtParam (cont);
                }
            else
                {
                cont->length = OamVarErrAttrUnsupported;
                }
            break;
            
        case OamCucAttrOnuCap4:
            if (OamIeeeIsVendorNeg(0, OuiCuc))
                {
                cont->length = OamVarErrAttrUnsupported;
                }
            else
                {
                cont->length = OamVarErrActBadParameters;
                }
            break;
            
        default:
            cont->length = OamVarErrActBadParameters;
            break;
        }
    } // OamCtcHandleGetOnuAttr


////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleGetEthAttr - Handle a get extended attribute branch
///
/// This function handles extended attribute requests.
///
 // Parameters:
/// \param link     link for message
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleGetEthAttr (TkOnuEthPort port,
                             const OamVarDesc BULK* src,
                             OamVarContainer BULK* cont)
    {
    cmsLog_notice("leaf = 0x%x", OAM_NTOHS(src->leaf));
    switch (OAM_NTOHS(src->leaf))
        {
        case OamCtcAttrEthLinkState:
            OamCtcGetEthLinkState (port, cont);
            break;

        case OamCtcAttrEthPortPause:
            OamCtcGetEthPortPause (port, cont);
            break;

        case OamCtcAttrClassMarking:
            OamCtcGetClassMarking (port, cont);
            break;

        case OamCtcAttrEthPortPolice:
            OamCtcGetEthUsPolice (port, cont);
            break;

        case OamCtcAttrEthPortDsRateLimit:
            OamCtcGetEthDsShaper (port, cont);
            break;


        case OamCtcAttrPortLoopDetect:
            OamCtcGetEthLoopDetect(port, cont);
            break;

        case OamCtcAttrPortDisableLooped:
            OamCtcGetEthDisableLooped(port, cont);
            break;
#ifdef DMP_TAENDPOINT_1						
        case OamCtcAttrVoipPort:
            OamCtcGetVoipPort(port, cont);
            break;
#endif
        case OamCtcAttrE1Port:
            cont->length = OamVarErrAttrUnsupported;
            break;

        case OamCucAttrOnuMacLimit:
            if (OamIeeeIsVendorNeg(0, OuiCuc))
                {
                OamCucGetMacLimit(port,cont);
                }
            else
                {
                cont->length = OamVarErrAttrUnsupported;
                } 
            break;

        case OamCucAttrOnuPortModeStatus:
            if (OamIeeeIsVendorNeg(0, OuiCuc))
                {
                OamCucGetPhyType(port, cont);
                }
            else
                {
                cont->length = OamVarErrAttrUnsupported;
                } 
            break;

        case OamCtcAttrVlan:
            OamCtcGetVlan (port, cont);
            break;

        case OamCtcAttrMcastVlan:
            OamCtcGetMcastVlan (port, cont);
            break;

        case OamCtcAttrMcastStrip:
            OamCtcGetMcastStrip (port, cont);
            break;

        case OamCtcAttrGroupMax:
            OamCtcGetGroupMax (port, cont);
            break;

        case OamCtcAttrAlarmAdminState:
            OamCtcGetAlarmAdminState(OamCtcObjPort,
                                     port,
                                     (OamVarContainer BULK*)src,
                                     cont);
            break;

        case OamCtcAttrPrfrmMontrStatus:
            OamCtcGetPfmStatus (port+1, cont);
            break;

        case OamCtcAttrPrfrmCurrtData:
            OamCtcGetPfmData (FALSE, port+1, cont);
            break;

        case OamCtcAttrPrfrmHistrData:
            OamCtcGetPfmData (TRUE, port+1, cont);
            break;
#if INCLUDE_PPPOE
        case OamZteAttrPppoEAuthenResult :
            {
            OnuPPPoeTestResultGet(cont);
            break;
            }
#endif // INCLUDE_PPPOE
        default:
            cont->length = OamVarErrAttrUnsupported;
            break;
        }
    } // OamCtcHandleGetEthAttr


////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleGetPonIfAttr - Handle a get pon interface branch
///
/// This function handles pon interface requests.
///
 // Parameters:
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleGetPonIfAttr (const OamVarDesc BULK* src,
                               OamVarContainer BULK* cont)
    {
    cmsLog_notice("leaf = 0x%x", OAM_NTOHS(src->leaf));
    switch (OAM_NTOHS(src->leaf))
        {
        case OamCtcAttrAlarmAdminState:
            OamCtcGetAlarmAdminState(OamCtcObjPon,
                                     TkOnuPonNumIndex,
                                     (OamVarContainer BULK*)src,
                                     cont);
            break;

        case OamCtcAttrPowerMonThreshold:
            OamCtcGetPowerMonThd((OamVarContainer BULK*)src, cont);
            break;

        case OamCtcAttrPrfrmMontrStatus:
            OamCtcGetPfmStatus (0, cont);
            break;

        case OamCtcAttrPrfrmCurrtData:
            OamCtcGetPfmData (FALSE, 0, cont);
            break;

        case OamCtcAttrPrfrmHistrData:
            OamCtcGetPfmData (TRUE, 0, cont);
            break;

        default:
            cont->length = OamVarErrAttrUnsupported;
            break;
        }
    } // OamCtcHandleGetPonIfAttr


////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleGetLlidAttr - Handle a get LLID branch
///
/// This function handles LLID interface requests.
///
 // Parameters:
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleGetLlidAttr (const OamVarDesc BULK* src,
                              OamVarContainer BULK* cont)
    {
    cmsLog_notice("leaf = 0x%x", OAM_NTOHS(src->leaf));
    switch (OAM_NTOHS(src->leaf))
        {
        case OamCtcAttrLlidQueueConfig :
            if(ctcLlidMode != CtcLlidModeMllid)
                {
                cont->length = OamVarErrActBadParameters;
                }
              //else
              //  {
                //cont->length = CtcMLlidQueueConfigGet(OamContextLink(),
                //                    (CtcLlidQueueCfg BULK*)cont->value);
              //  }
            break;

        default:
            cont->length = OamVarErrAttrUnsupported;
            break;
        }
    } // OamCtcHandleGetLlidAttr


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle a get extended attribute branch
///
/// This function handles extended attribute requests.
///
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleGetExtAttr(OamVarDesc BULK* src, OamVarContainer BULK* cont)
    {
    OamCtcObject BULK * obj;

    obj = OamCtcGetObject (src);

    cmsLog_notice("objType = 0x%x", obj->objType);
    switch (obj->objType)
        {
        case OamCtcObjPort:
            if (obj->instNum.sNum.portType == OamCtcPortEth)
                {
                cmsLog_notice("portNum=0x%x", obj->instNum.sNum.portNum);

                if (obj->instNum.sNum.portNum == OamCtcAllUniPorts)
                    {
                    TkOnuEthPort FAST port;
                    cmsLog_notice("from all uni ports");
                    for (port = 0; port < UniCfgDbGetActivePortCount(); ++port)
                        {
                        OamContextReset (port+1,0);
                        if (port != 0)
                            {
                            StreamSkip(
                                &oamParser.reply,
                                OamContSize(
                                    (OamVarContainer *)oamParser.reply.cur));
                            }
                        cont = (OamVarContainer *)oamParser.reply.cur;
                        cont = OamCtcCreateObject (cont,
                                                   OamCtcObjPort,
                                                   OamCtcEthToPort (port));
                        cont->branch = src->branch;
                        cont->leaf = src->leaf;
                        OamCtcHandleGetEthAttr (port, src, cont);
                        }
                    }
                else if (OamCtcPortToEth(obj->instNum.sNum.portNum) <
                         UniCfgDbGetActivePortCount())
                    {
                    cmsLog_notice("from uni port %d", 
                            (TkOnuEthPort)OamCtcPortToEth(obj->instNum.sNum.portNum));    
                   
                    OamCtcHandleGetEthAttr(
                        (TkOnuEthPort)OamCtcPortToEth(
                            obj->instNum.sNum.portNum),
                        src,
                        cont);
                    }
                }
#ifdef DMP_TAENDPOINT_1			
            else if (obj->instNum.sNum.portType == OamCtcPortVoIP)
                {
#ifdef BRCM_CMS_BUILD
                CmsRet ret = CMSRET_SUCCESS;
                InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
                VoiceCapObject* voiceCapObj;
            	
                if( (ret = cmsLck_acquireLockWithTimeout(50000)) != CMSRET_SUCCESS )
                    {
                    cmsLog_error("could not acquire lock, ret=%d", ret);
                    return;
                    }
            	
                if((ret = cmsObj_getNext(MDMOID_VOICE_CAP, &iidStack, (void **) &voiceCapObj)) != CMSRET_SUCCESS)
                    {
                    cmsLog_error("could not get  MDMOID_VOICE_CAP, ret=%d", ret);
                    cmsLck_releaseLock();
                    return OamVarErrActNoResources;
                    }
				
                cmsLog_debug("maxLineCount: %d\n", voiceCapObj->maxLineCount);                

                if (obj->instNum.sNum.portNum == OamCtcAllUniPorts)
                    {
                    TkOnuEthPort FAST port;
                    for (port = 1; port <= voiceCapObj->maxLineCount; ++port)
                        {
                        OamContextReset (port,0);
                        if (port != 1)
                            {
                            StreamSkip(
                                &oamParser.reply,
                                OamContSize(
                                    (OamVarContainer *)oamParser.reply.cur));
                            }
                        cont = (OamVarContainer *)oamParser.reply.cur;
                        cont = OamCtcCreateObject (cont,
                                                   OamCtcObjPort,
                                                   port);
                        cont->branch = src->branch;
                        cont->leaf = src->leaf;
                        OamCtcHandleGetEthAttr (port, src, cont);
                        }
                    }
                else if (obj->instNum.sNum.portNum <= voiceCapObj->maxLineCount)
                    {                   
                    OamCtcHandleGetEthAttr(obj->instNum.sNum.portNum, src, cont);
                    }
                else
                    {
                    cmsLog_notice("invalid port number");                       
                    cont->length = OamVarErrActBadParameters;
                    }
				
                cmsObj_free((void **)&voiceCapObj);
                cmsLck_releaseLock();
#else
                return OamVarErrActNoResources;
#endif
                }
#endif			
            else
                {
                cmsLog_notice("obj not supported");    
                cont->length = OamVarErrObjUnsupported;
                }
            break;

        case OamCtcObjCard:
            cont->length = OamVarErrObjUnsupported;
            break;

        case OamCtcObjLlid:
            if (OamContextLink() < 0x08)
                {
                OamCtcHandleGetLlidAttr (src, cont);
                }
            else
                {
                cont->length = OamVarErrActNoResources;
                }
            break;

        case OamCtcObjPon:
            if (obj->instNum.sNum.portNum < EponDevCtcPonIfNum())
                {
                OamCtcHandleGetPonIfAttr (src, cont);
                }
            else
                {
                cmsLog_notice("act no resource");    
                cont->length = OamVarErrActNoResources;
                }
            break;

        case OamCtcObjOnu:
            OamCtcHandleGetOnuAttr (src, cont);
            break;

        default:
            cont->length = OamVarErrObjUnsupported;
            break;
        }

    StreamSkip(&oamParser.reply,
               OamContSize((OamVarContainer *)oamParser.reply.cur));
    } // OamCtcHandleGetExtAttr


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle a get extended attribute branch
///
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleGetAttr(OamVarDesc BULK* src, OamVarContainer BULK* cont)
    {
    OamCtcObject BULK * obj;

    obj = OamCtcGetObject (src);

    cmsLog_notice("objType = 0x%x", obj->objType);
    switch (obj->objType)
        {
        case OamCtcObjPort:
            if (obj->instNum.sNum.portType == OamCtcPortEth)
                {
                if (obj->instNum.sNum.portNum == OamCtcAllUniPorts)
                    {
                    TkOnuEthPort FAST port;

                    for (port= 0; port < UniCfgDbGetActivePortCount (); ++port)
                        {
                        OamContextReset (port+1,0);
                        if (port != 0)
                            {
                            StreamSkip(
                                &oamParser.reply,
                                OamContSize(
                                    (OamVarContainer *)oamParser.reply.cur));
                            }
                        cont = (OamVarContainer *)oamParser.reply.cur;
                        cont = OamCtcCreateObject (cont,
                                                   OamCtcObjPort,
                                                   OamCtcEthToPort (port));
                        cont->branch = src->branch;
                        cont->leaf = src->leaf;
                        OamGetAttrEth (port, cont, src);
                        }
                    }
                else if (OamCtcPortToEth(obj->instNum.sNum.portNum) <
                         UniCfgDbGetActivePortCount())
                    {
                    OamGetAttrEth (
                        (TkOnuEthPort)OamCtcPortToEth(
                            obj->instNum.sNum.portNum),
                        cont,
                        src);
                    }
                else
                    {
                    cont->length = OamVarErrActNoResources;
                    }
                }
            else
                {
                cont->length = OamVarErrObjUnsupported;
                }
            break;

        case OamCtcObjOnu:
            OamGetAttrEpon (cont, src);
            break;

        default:
            cont->length = OamVarErrObjUnsupported;
            break;
        }

    StreamSkip(&oamParser.reply,
               OamContSize((OamVarContainer *)oamParser.reply.cur));
    } // OamCtcHandleGetAttr


//##############################################################################
// Primary Variable Request Handling Functions
//##############################################################################


////////////////////////////////////////////////////////////////////////////////
/// \brief  Check registration on a variable descriptor
///
/// \param  src     Pointer to var descriptors in request
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetVarRegCheck(const OamVarDesc BULK* src)
    {
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Process a variable descriptor
///
/// \param  src     Pointer to var descriptors in request
///
/// \pre    oamParser.reply MUST point to the next variable container in the
///         response frame
/// \post   oamParser.reply WILL point to the end of the last variable
///         container written to the response frame
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetVarProcess(const OamVarDesc BULK* src)
    {
    OamVarContainer BULK * cont = (OamVarContainer BULK *)oamParser.reply.cur;

    cont->branch = src->branch;
    cont->leaf = src->leaf;
	
    cmsLog_notice("branch = 0x%x, leaf = 0x%x", src->branch, OAM_NTOHS(src->leaf));

    switch (src->branch)
        {
        case OamCtcBranchAttribute:
            OamCtcHandleGetAttr ((OamVarDesc *)src, cont);
            break;

        case OamCtcBranchAction:
            OamAct((OamVarContainer BULK *)src, cont);
            StreamSkip(&oamParser.reply, OamContSize(cont));
            break;

        case OamCtcBranchObjInst:
        case OamCtcBranchObjInst21:
            OamCtcSetObject((OamVarContainer BULK *)src, cont);
            if (!OamCtcIsObjAllPorts())
                {
                StreamSkip(&oamParser.reply, OamContSize(cont));
                }
            break;

        case OamCtcBranchExtAttribute:
            OamCtcHandleGetExtAttr ((OamVarDesc *)src, cont);
            break;

        default:
            cmsLog_notice("return attr unsupported!");
            cont->length = OamVarErrAttrUnsupported;
            StreamSkip(&oamParser.reply, OamContSize(cont));
            break;
        }
    } // OamCtcGetVarProcess


////////////////////////////////////////////////////////////////////////////////
/// \brief  Parse variable descriptors in a CTC OAM get request
///
/// \param  varAction   Action to perform for each variable container
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGetParse(void (*varAction)(const OamVarDesc BULK *))
    {
    OamVarDesc BULK * src = (OamVarDesc BULK *)oamParser.src.cur;

    OamCtcClearObject();

    while (src->branch != OamCtcBranchTermination)
        {
        if ((U8*)src >= PointerAdd(RxFrame, rxFrameLength))
            { /* incoming packet used up */
            cmsLog_notice("OAM: HandleGet src frame unterminated");
            break;
            }
            
        varAction(src);
        src = OamCtcNextVarRequest(src);
        }
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcHandleExtVarReq(void)
    {
    OamCtcGetParse(OamCtcGetVarRegCheck);
    StreamWriteU8(&oamParser.reply, OamCtcExtVarResponse);
    OamCtcGetParse(OamCtcGetVarProcess);
    OamCtcTerminateBranch();
    OamTransmit();
    } // OamCtcHandleExtVarReq


// End of File CtcOamGet.c

