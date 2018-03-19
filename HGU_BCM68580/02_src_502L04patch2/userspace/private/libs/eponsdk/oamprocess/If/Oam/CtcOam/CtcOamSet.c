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
/// \file CtcOamSet.c
/// \brief China Telecom extended OAM set module
/// \author Jason Armstrong
/// \date March 1, 2006
///
///
////////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdlib.h>

#include "Teknovus.h"
#include "Build.h"
#include "Holdover.h"
#include "EponDevInfo.h"
#include "OamSet.h"
#include "UniManager.h"
#include "UniConfigDb.h"
#include "OntConfigDb.h"
#include "OptCtrl.h"
#include "PonManager.h"
#include "CtcOam.h"
#include "CtcOnuOam.h"
#include "CtcOnuUtils.h"
#include "CtcMLlid.h"
#include "CtcVlan.h"
#include "CtcClassification.h"
#include "CtcEthControl.h"
#include "CtcAlarms.h"
#if SFF8472
#include "CtcOptDiag.h"
#endif
#include "Stream.h"
#include "PonManager.h"

#include "CtcStats.h"
#include "CtcMcast.h"
#include "CtcOamDiscovery.h"
#if INCLUDE_PPPOE
#include "PppoE.h"
#endif
#include "OamCtcPowerSave.h"
#include "cms_log.h"
#ifdef BRCM_CMS_BUILD
#include "cms_msg.h"
#include "cms_util.h"
#include "cms_core.h"
#endif

#include "CucOam.h"
#include "MacLearning.h"
#include "ethswctl_api.h"
#include "rdpactl_api.h"

#define CtcHoldoverMinTime            50      // 50ms
#define CtcHoldoverMaxTime            1000    // 1000ms

extern void *msgHandle;

#if LOOP_DETECT
static void LoopDetectSetEnabled(U32 port, U32 mgmt);
static void LoopDetectSetAutoBlock(U32 port, U32 action);
#endif


////////////////////////////////////////////////////////////////////////////////
/// OamCucSetOnuCfgCnt - Set Cuc configuration Counter
///
 // Parameters:
/// \param src      Container to source information
/// \param cont     Container to place information
///
/// \return
/// Pointer to last container
////////////////////////////////////////////////////////////////////////////////
static
void OamCucSetOnuCfgCnt (OamVarContainer BULK* src,
                                       OamVarContainer BULK *cont)
    {
#ifdef BRCM_CMS_BUILD
    U32 *cfgCnt = (U32 *)src->value;
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

    obj->cucCfgCnt = OAM_NTOHL(*cfgCnt);
    
    ret = cmsObj_set(obj, &xponiidStack);
    if( ret != CMSRET_SUCCESS)
        {
        cmsLog_error("set cms error (%d)\n", ret);
        cont->length = OamVarErrActNoResources;
        }

    ret = cmsMgm_saveConfigToFlash();
    if (ret != CMSRET_SUCCESS)
        {
        cmsLog_error("Could not save config to flash, ret=%d\n", ret);
        cont->length = OamVarErrActNoResources;
        }

    cmsObj_free((void **) &obj);
    cmsLck_releaseLock();  
#else
    UNUSED(src);
    UNUSED(cont);
#endif
    }


//##############################################################################
// Extended Attribute Handling Functions
//##############################################################################

#ifdef DMP_TAENDPOINT_1			
////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetVoipPort - Set Voip Pots status
///
 // Parameters:
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetVoipPort (TkOnuEthPort port,
                            const OamVarContainer BULK* src,
                            OamVarContainer BULK *cont)
    {  
#ifdef BRCM_CMS_BUILD
#ifdef DMP_VOICE_SERVICE_1
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    VoiceLineObject* voiceLineObj;

   cmsLog_debug("value:%d", src->value[0]);
   
   PUSH_INSTANCE_ID( &iidStack, 1);
   PUSH_INSTANCE_ID( &iidStack, 1);
   PUSH_INSTANCE_ID( &iidStack, port);
    if((ret = cmsObj_get(MDMOID_VOICE_LINE, &iidStack, 0, (void **) &voiceLineObj)) != CMSRET_SUCCESS)
       {
       cmsLog_error("could not get  MDMOID_VOICE_LINE, ret=%d", ret);
       cont->length = OamVarErrActNoResources;
       return;
        }
	
    CMSMEM_REPLACE_STRING(voiceLineObj->enable, src->value[0] ? MDMVS_ENABLED : MDMVS_DISABLED);
    cmsObj_set(voiceLineObj, &iidStack);
    cmsObj_free((void **) &voiceLineObj);

#else
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack searchIidStack = EMPTY_INSTANCE_ID_STACK;
    void* obj;

    cmsLog_debug("value:%d", src->value[0]);

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
   
    ((SipClientObject *)obj)->enable = (U8)src->value[0];
    cmsObj_set(obj, &searchIidStack);
	
    cmsObj_free((void **) &obj);
#endif // DMP_VOICE_SERVICE_1

#else
    UNUSED(port);
    UNUSED(src);
    UNUSED(cont);
#endif
    } // OamCtcSetEthPortPause
#endif

////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetEthPortPause - Set ONU flow control status
///
 // Parameters:
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetEthPortPause (TkOnuEthPort port,
                            const OamVarContainer BULK* src,
                            OamVarContainer BULK *cont)
    {  
    if (!CtcEthControlPauseSet(port, 0 != (src->value[0])))
		{
		cont->length = OamVarErrActBadParameters;
    	}
    } // OamCtcSetEthPortPause


////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetClassMarking - Set ONU classification provisioning
///
/// Parameters:
/// \param port     ctc port - 1
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetClassMarking (TkOnuEthPort port,
                            OamVarContainer BULK* src,
                            OamVarContainer BULK* cont)
    {
    CtcTlvClassMarking BULK *ctcRule = (CtcTlvClassMarking BULK *)
                                          (src->value);
    cmsLog_debug("action = %d, numRule = 0x%x", ctcRule->action, ctcRule->numRules);
	
    switch (ctcRule->action)
        {
        case CtcRuleActionDel:
            switch(CtcClassificationDel(port,
                              (CtcTlvClassMarkingDelete BULK*)ctcRule))
                {
                case CtcRuleRetCodeBadParameters:
                    cont->length = OamVarErrActBadParameters;
                    break;

                case CtcRuleRetCodeNoResource:
                    cont->length = OamVarErrActNoResources;
                    break;

                default:
                    cont->length = OamVarErrNoError;
                    break;
                }
                break;

        case CtcRuleActionAdd:
            switch (CtcClassificationAdd(port, (OamCtcTlvClassMarking *)ctcRule))
                {
                case CtcRuleRetCodeBadParameters:
                    cont->length = OamVarErrActBadParameters;
                    break;

                case CtcRuleRetCodeNoResource:
                    cont->length = OamVarErrActNoResources;
                    break;

                default:
                    cont->length = OamVarErrNoError;
                    break;
                }

            break;

        case CtcRuleActionClear:
            CtcClassificationClr(port);
            cont->length = OamVarErrNoError;
            break;

        case CtcRuleActionList:
        default:
            cmsLog_error("unknown action = %d", ctcRule->action);
            cont->length =OamVarErrActBadParameters;
        }
    } // OamCtcSetClassMarking


////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetEthUsPolice  Set ONU Ethernet port upstream policing
///
 // Parameters:
/// \param port     which to set
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetEthUsPolice (TkOnuEthPort port,
                             OamVarContainer BULK* src,
                             OamVarContainer BULK *cont)
    {
    CtcEthPortUsPolice BULK *conf;

    conf = (CtcEthPortUsPolice BULK *)(&src->value[0]);

    if (!CtcEthControlUsPoliceCfgSet (port, conf))
        {
        cont->length = OamVarErrActBadParameters;
        }
    } // OamCtcSetEthUsPolice


////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetEthDsShaper - ONU Ethernet port downstream Shaping
///
 // Parameters:
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetEthDsShaper (TkOnuEthPort port,
                           OamVarContainer BULK* src,
                           OamVarContainer BULK *cont)
    {
    CtcEthPortDsShaper BULK *srcVal;

    srcVal = (CtcEthPortDsShaper BULK *)(&src->value[0]);

    if (!CtcEthControlDsShaperCfgSet (port, srcVal))
        {
        cont->length = OamVarErrActBadParameters;
        }
    } // OamCtcSetEthDsShaper



////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetPortLoopDetect - ONU Ethernet port loop detect
///
 // Parameters:
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetPortLoopDetect (TkOnuEthPort port,
                                    OamVarContainer BULK* src,
                                    OamVarContainer BULK *cont)
    {
    OamCtcTlvPortLoopDetect BULK *srcVal;

    srcVal = (OamCtcTlvPortLoopDetect BULK *)(&src->value[0]);

    if(OAM_NTOHL(srcVal->enabled) == OamCtcActionDisable)
        {
#if LOOP_DETECT
 
			{
			LoopDetectSetEnabled(port, OamCtcActionDisable);
			return ;
			}
		
#endif
#if OAM_RSTP

                        {
                            FILE* f = NULL;
                            f = fopen("/sys/class/net/br0/bridge/stp_state", "w");
                            if (f <= 0)
                            {
                                cont->length = OamVarErrAttrUnsupported;
                                fclose(f);
                                return;
                            }
                            fprintf(f, "%d\n", 0);
                            fclose(f);
                            return ;
                        }
		
#endif
        cont->length = OamVarErrAttrUnsupported;
        }
    else if(OAM_NTOHL(srcVal->enabled) == OamCtcActionEnable)
        {
#if LOOP_DETECT

			{
			LoopDetectSetEnabled(port, OamCtcActionEnable);
			return ;
			}
		
#endif
#if OAM_RSTP

			{
                            FILE* f = NULL;
                            f = fopen("/sys/class/net/br0/bridge/stp_state", "w");
                            if (f <= 0)
                            {
                                cont->length = OamVarErrAttrUnsupported;
                                fclose(f);
                                return;
                            }
                            fprintf(f, "%d\n", 1);
                            fclose(f);
                            return ;
			}
		
#endif
		  cont->length = OamVarErrAttrUnsupported;

        }
    else
        {
        cont->length = OamVarErrActBadParameters;
        }
    } // OamCtcSetPortLoopDetect



////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetLoopedPortDisable - disable looped port or not
///
 // Parameters:
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetLoopedPortDisable (TkOnuEthPort port,
                                    OamVarContainer BULK* src,
                                    OamVarContainer BULK *cont)
    {
    OamCtcTlvLoopedPortDisable BULK *srcVal;

    srcVal = (OamCtcTlvLoopedPortDisable BULK *)(&src->value[0]);

    if(OAM_NTOHL(srcVal->disable) == OamCtcLoopedPortDisable)
        {
#if LOOP_DETECT

			{
			LoopDetectSetAutoBlock(port, TRUE);
			return ;
			}
		
#endif
#if OAM_RSTP

			{
			//RstpSetPortAutoBlock (port, port, TRUE);
			//return ;
			}
		
#endif


		cont->length = OamVarErrAttrUnsupported;
       
        }
    else if(OAM_NTOHL(srcVal->disable) == OamCtcLoopedPortEnable)
        {
#if LOOP_DETECT

			{
			LoopDetectSetAutoBlock(port, FALSE);
			return ;
			}
		
#endif
#if OAM_RSTP

			{
			//RstpSetPortAutoBlock (port, port, FALSE);
			//return ;
			}
		
#endif

		cont->length = OamVarErrAttrUnsupported;

        }
    else
        {
        cont->length = OamVarErrActBadParameters;
        }

    } // OamCtcSetLoopedPortDisable



////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetVlan - Set ONU VLAN provisioning information
///
 // Parameters:
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetVlan (TkOnuEthPort port,
                    OamVarContainer BULK* src,
                    OamVarContainer BULK *cont)
    {
    OamCtcTlvVlan BULK *srcData;
    BOOL FAST ret;

    srcData = (OamCtcTlvVlan BULK *)(&src->value[0]);
    ret = CtcVlanSetMode (port, srcData->mode,
                          ((U8 BULK *)(srcData)) + 1,
                          src->length);
    if (!ret)
        {
        cont->length = OamVarErrActBadParameters;
        }
    } // OamCtcSetVlan



////////////////////////////////////////////////////////////////////////////////
/// \brief  Is queue configured to be strict priority?
///
/// \param sch      Scheduler type
/// \param U8       high priority bound
/// \param pri      Priority of queue
///
/// \return
/// TRUE if queue is configured to be strict priority
////////////////////////////////////////////////////////////////////////////////
static
BOOL IsQueueSp(OamCtcScheduling sch,
               U8 highPriBound,
               U8 pri)
    {
    return
        (sch == OamCtcScheduleSp)
    ||
        ((sch == OamCtcScheduleSpWrr)
         &&
         (pri >= highPriBound));
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Validate queue number
///
/// \param src      Variable information source
///
/// \return
/// TRUE if weight is valid for given priority and SLA
////////////////////////////////////////////////////////////////////////////////
static
BOOL ValidateOamQueue(OamVarContainer BULK * src)
    {
    U8 FAST i;
    U8 FAST j;
    OamCtcServiceSla BULK * sla = (OamCtcServiceSla BULK *)src->value;
    OamCtcService BULK * ser = (OamCtcService BULK *)sla->service;

    //lint --e{661} access of out-of-bound pointer
    for (i = 0; i < sla->numServices; i++)
        {
        for (j = i + 1; j < sla->numServices; j++)
            {
            if ((ser + i)->qNum == (ser + j)->qNum)
                {
                return FALSE;
                }
            }
        }

    return TRUE;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Validate queue weight
///
/// \param sch      Scheduler type
/// \param U8       high priority bound
/// \param pri      Priority of queue
/// \param weight   weight of queue
///
/// \return
/// TRUE if weight is valid for given priority and SLA
////////////////////////////////////////////////////////////////////////////////
static
BOOL ValidateWeight(OamCtcScheduling sch,
                    U8 highPriBound,
                    U8 pri,
                    U16 weight)
    {
    if (IsQueueSp(sch, highPriBound, pri))
        {
        return weight == 0;
        }

    return (weight > 0) && (weight <= 100);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set CTC service SLA
///
/// \note   Per discussion with Drew we are ignoring the cycle length
///         (when non-zero), the fixed packet size and all the bandwidth
///         parameters
///
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcServiceSlaSet(OamVarContainer BULK * src,
                         OamVarContainer BULK * cont)
    {
    U8 priorities;
    OamCtcServiceSla BULK * sla = (OamCtcServiceSla BULK *)src->value;
    BOOL FAST valid = TRUE;
    Stream input;
    OamCtcScheduling sch;
    OamCtcServiceDbaOp dbaOp;
    U8 highPriBound;
    U32 cycleLen;
    U8 numServices;
    CtcLlidQueueCfgLocal BULK queueCfg;
    memset(&queueCfg, 0, sizeof(CtcLlidQueueCfgLocal));

    if (ctcLlidMode == CtcLlidModeMllid)
        {
        cont->length = OamVarErrActNoResources;
        return;
        }

    CtcInitQueueSizes(); // force queue init from CTC Default Queue Sizes

    if (!ValidateOamQueue(src))
        {
        cont->length = OamVarErrActBadParameters;
        return;
        }

    StreamInit(&input, (U8 BULK *)src);
    StreamSkip(&input, 4); // skip OamVarContainer overhead

    priorities = PonMgrPriCountGet();

    dbaOp = (OamCtcServiceDbaOp)StreamReadU8(&input);
    if (OamCtcServiceDbaOpActivate == dbaOp)
        {
        U8 FAST i;
        U8 FAST j;
        U16 FAST totalWeight = 0;
        U8 qId;
        U8 weight;
        sch = (OamCtcScheduling)StreamReadU8(&input);
        highPriBound = StreamReadU8(&input);
        cycleLen = StreamReadU32(&input);
        numServices = StreamReadU8(&input);

        for (i = 0; i < Min(numServices, priorities); i++)
            {
            //if this is the last available priority we
            //pontentially want to add more than one queue.
            if (i < (priorities-1))
                {
                qId = StreamReadU8(&input);
                queueCfg.qCfg[queueCfg.queues++].queueID = qId;
                //skipping pktSize, fixedBw, guarBw, BestefforBw each 2 byte fields
                StreamSkip(&input, 4*2);
                weight = StreamReadU8(&input);
                if (!ValidateWeight(sch, highPriBound, qId, weight))
                    {
                    valid = FALSE;
                    break;
                    }

                totalWeight += weight;

                if (cycleLen == 0)
                    {
                    weight = 0;
                    }

                }
            else
                {
                //note that we should not reach here unless
                //we have one or more services remaining.
                //therefore sla->numServices >= priorities aka no negatives.
                U8 index;
                U8 lastPriWeight = 0;
                U8 cnt = (numServices - priorities)+1;

                //get the sum of all remaining weights
                if (cycleLen == 0)
                    {
                    weight = 0;
                    }
                else

                    {
                    for (index = i; index < numServices;index++)
                        {
                        qId = StreamReadU8(&input);
                        //skipping pktSize, fixedBw, guarBw,
                        //BestefforBw each 2 byte fields
                        //also skipping queue ID 1 byte field
                        StreamSkip(&input, (4*2));
                        weight = StreamReadU8(&input);
                        if (cycleLen == 0)
                            {
                            totalWeight = 0;
                            }
                        lastPriWeight += weight;
                        totalWeight += weight;
                        if (!ValidateWeight(sch, highPriBound, qId, weight))
                            {
                            valid = FALSE;
                            }
                        }
                    //rewind
                    StreamRewind(&input, ((4*2)+1+1)*cnt);
                    }
                for(j = 0; j < cnt; j++)
                    {
                    qId = StreamReadU8(&input);
                    queueCfg.qCfg[queueCfg.queues++].queueID = qId;
                    //skipping pktSize, fixedBw, guarBw,
                    //BestefforBw each 2 byte fields
                    //also skipping weight 1 byte field
                    StreamSkip(&input, (4*2)+1);
                    }
                }
            }

        // go back and write length

        if ((totalWeight != 100) && (sla->sched != OamCtcScheduleSp))
            {
            valid = FALSE;
            }
        }
    else if (OamCtcServiceDbaOpDeactivate == dbaOp)
        {
        ;    // Do nothing here!
        }
    else
        {
        valid = FALSE;
        }


    if (valid)
        {
        if(OamCtcServiceDbaOpActivate == dbaOp)
            {
            }
        }
    else
        {
        cont->length = OamVarErrActBadParameters;
        }
    } // OamCtcServiceSlaSet


////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetHoldover - Set holdover params
///
/// Parameters:
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetHoldover (OamVarContainer BULK * src,
                        OamVarContainer BULK * cont)
    {
    OamCtcHoldover BULK * pHoldover = (OamCtcHoldover BULK *)src->value;
    if(OAM_NTOHL(pHoldover->holdoverFlag) == OamCtcActionEnable)
        {
        U16 FAST holdoverTime = (U16)OAM_NTOHL(pHoldover->holdoverTime);
        if( (holdoverTime >= CtcHoldoverMinTime) &&
            (holdoverTime <= CtcHoldoverMaxTime) )
            {
            HoldoverSetParams(holdoverTime, rdpa_epon_holdover_noflags);
            }
        else
            {
            cont->length = OamVarErrActBadParameters;
            }
        }
    else
        {
        HoldoverSetParams(0, rdpa_epon_holdover_noflags);
        }
    } // OamCtcSetHoldover


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set active PON_IF admin state
///
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetActPonIfAdmin(const OamVarContainer BULK * src,
                            OamVarContainer BULK * cont)
    {
        cont->length = OamVarErrActNoResources;
    } // OamCtcSetHoldover


////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetTxPower - Disables the laser transmitter for a specified length of time
///
/// Parameters:
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetTxPower (OamVarContainer BULK * src,
                                OamVarContainer BULK * cont)
    {
    Stream s;
    MacAddr onuId;
    U32 action;
    OamCtcOnuTxPowerCtrlOpId        optId;

    cmsLog_notice("enter:");

    //The OAM expects no response.
    oamParser.dest.pift = PortIfNone;    

    StreamInit(&s, (U8 BULK *)(&src->value[0]));

    //warning: because of CTC2.1 spec error, the length is not match as variable structure.
    //some customers used the length the same as CTC2.1 spec definition.
    //but some customers used the fixed length the same as variable structure.
    //firmware cover the two case when onu run in CTC2.1 version.
    if (src->length == 0x0c)
        {
        action = StreamReadU16(&s);
        }
    else
        {
        action = StreamReadU32(&s);
        }

    onuId.mac[0] = StreamReadU8(&s);
    onuId.mac[1] = StreamReadU8(&s);
    onuId.mac[2] = StreamReadU8(&s);
    onuId.mac[3] = StreamReadU8(&s);
    onuId.mac[4] = StreamReadU8(&s);
    onuId.mac[5] = StreamReadU8(&s);

    optId = (OamCtcOnuTxPowerCtrlOpId)StreamReadU32(&s);

    if ((memcmp(&onuId, (U8 *)&EponDevBaseMacAddr, sizeof(MacAddr)) == 0) ||
        ((onuId.lowHi.hi == 0xFFFF)&&(onuId.lowHi.low == 0xFFFFFFFFUL)))
        {
        // Active optical module
        if((optId == OamCtcMainOptical) ||
                              (optId == OamCtcBothOptical))
            {
            if (eponStack_CtlSetTxPower(TRUE, (U16)(action & 0xFFFF)))
                cont->length = OamVarErrUnknow;
            else
                cont->length = OamVarErrNoError;
            }

        // Standby optical module
        if((optId == OamCtcStandbyOptical) ||
                              (optId == OamCtcBothOptical))
            {
            if (eponStack_CtlSetTxPower(FALSE, (U16)(action & 0xFFFF)))
                cont->length = OamVarErrUnknow;
            else
                cont->length = OamVarErrNoError;
            }
        }

    } // OamCtcSetTxPower


////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetAlarmAdminState - Set Alarm Admin State
///
/// Parameters:
///\param port      port number
/// \param src      pointer to var descriptors in request
/// \param cont     Container to place information
///
/// \return
/// Pointer to last container
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetAlarmAdminState (OamCtcObjType obj,
                                TkOnuEthPort port,
                                const OamVarContainer BULK *src,
                                OamVarContainer BULK *cont)
    {
    OamCtcTlvAlarmAdminState BULK *value;

    value = (OamCtcTlvAlarmAdminState BULK *)(&src->value[0]);
    cont->length = OamVarErrActBadParameters;
    if(src->length != sizeof(OamCtcTlvAlarmAdminState))
        {
        return;
        }

    if(CtcAlmIdValid(obj, OAM_NTOHS(value->alarmId)))
        {
        if(CtcAlmAdminStateSet(port, OAM_NTOHS((OamCtcAlarmId)value->alarmId),
                 OAM_NTOHL(value->config)))
            {
            cont->length = OamVarErrNoError;
            }
        }
    } // OamCtcSetAlarmAdminState


#if SFF8472
////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetPowerMonThd - Set power monitor threshold value
///
/// Parameters:
/// \param src      pointer to var descriptors in request
/// \param cont     Container to place information
///
/// \return
/// Pointer to last container
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetPowerMonThd (const OamVarContainer BULK *src,
                                OamVarContainer BULK *cont)
    {
    OamCtcTlvPowerMonThd BULK * FAST value;
    OamCtcTlvPowerMonThd thd;

    value = (OamCtcTlvPowerMonThd BULK *)(&src->value[0]);

    thd.alarmId  = OAM_NTOHS(value->alarmId);
    thd.thdRaise = OAM_NTOHL(value->thdRaise);
    thd.thdClear = OAM_NTOHL(value->thdClear);
    
    if ((src->length != sizeof(OamCtcTlvPowerMonThd)) ||
        (OAM_NTOHS(value->alarmId) > OamCtcAttrPowerMonTempWarnLow))
        {
        cont->length = OamVarErrActBadParameters;
        }
    else
        {
        CtcOptDiagSetThd(&thd);
        cont->length = OamVarErrNoError;
        }
    } // OamCtcSetPowerMonThd
#endif


////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetMcastSwitch - Set ONU IGMP snooping status
///
 // Parameters:
/// \param obj      Object to be get information from
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetMcastSwitch (const OamVarContainer BULK* src,
                                             OamVarContainer BULK *cont)
{
    if((CtcMcastSwitchSetMode((OamCtcMcastMode)src->value[0]) != 0))
    {
        cont->length = OamVarErrActBadParameters;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetMcastVlan - Set multicast VLAN for the port
///
 // Parameters:
/// \param obj      Object to be get information from
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetMcastVlan (TkOnuEthPort port,
                          OamVarContainer BULK* src,
                          OamVarContainer BULK *cont)
{
    U8 FAST len;
    CtcMcastVlanOpCfg BULK * cfg;
    len = src->length;
    cfg = (CtcMcastVlanOpCfg BULK *)(&src->value[0]);

    if( CtcMcastVlanSet(port, len, cfg, FALSE) != 0)
    {
        cont->length = OamVarErrActBadParameters;
    }
} 

////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetMcastStrip - Set multicast VLAN tag stripping configuration
///
 // Parameters:
/// \param obj      Object to be get information from
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetMcastStrip (TkOnuEthPort port,
                          OamVarContainer BULK* src,
                          OamVarContainer BULK *cont)
{
    CtcMcastTagOpCfg BULK * cfg;
    cfg = (CtcMcastTagOpCfg BULK *)(&src->value[0]);

    if( CtcMcastTagOpSet(port, cfg) != 0)
    {
        cont->length = OamVarErrActBadParameters;
    }
} 

////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetGroupMax - Set the maximum number of groups for the port
///
 // Parameters:
/// \param port     Port to Set
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetGroupMax (TkOnuEthPort port,
                          OamVarContainer BULK* src,
                          OamVarContainer BULK *cont)
{
    OamCtcTlvGroupMax BULK *value;
    value = (OamCtcTlvGroupMax BULK *)(&src->value[0]);

    if((port >= UniCfgDbGetActivePortCount())
       || (!CtcMcastIsSnoopingMode())   //FIXME: only valid in snooping mode
       || (CtcMcastGroupMaxSet(port, value->maxGroup) != 0))
    {
        cont->length = OamVarErrActBadParameters;
    }
} 

////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetFastLeaveAdmin - Set the fast leave admin control
///
 // Parameters:
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetFastLeaveAdmin (const OamVarContainer BULK* src,
                              OamVarContainer BULK *cont)
{
    U32 FAST admin;
    admin = ContGetU32((OamVarContainer BULK *)src);

    if((admin == CtcMcastFastLeaveDisable) || (admin == CtcMcastFastLeaveEnable))
    {
        if(CtcMcastFastLeaveStateSet(admin) != 0)
        cmsLog_error("Set mcast fast leave failed\n");
    }
    else
    {
        cont->length = OamVarErrActBadParameters;
    }
} 

////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetMcastCtrl - Set multicast control information
///
 // Parameters:
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetMcastCtrl (OamVarContainer BULK* src,
                         OamVarContainer BULK *cont)
{

    //FIXME: dont apply this in host control mode? 
    if((!CtcMcastIsHostCtrlMode()) ||   (  src->length >= OamVarErrNoError))
    {
        cont->length = OamVarErrActBadParameters;
        cmsLog_error("OamVarErrActBadParameters return, IsHostCtrlMode = %d, length = %d", 
        CtcMcastIsHostCtrlMode(), src->length);
        return;
    }

     if (CtcMcastCtrlGrpSet(src, cont) != 0)
        cont->length = OamVarErrActBadParameters;
    else
        cont->length = OamVarErrNoError;
}

////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetOnuProtParam - Set Ctc Protection parameters
///
 // Parameters:
/// \param src      Container to source information
/// \param cont     Container to place information
///
/// \return
/// Pointer to last container
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetOnuProtParam (OamVarContainer BULK* src,
                                       OamVarContainer BULK *cont)
    {
    LosCheckPara los_para;

    OamCtcTlvOnuProtParam BULK *para =
                    (OamCtcTlvOnuProtParam BULK *)src->value;

    cont->length = (src->length == sizeof(OamCtcTlvOnuProtParam))?
                        OamVarErrNoError : OamVarErrActBadParameters;
    
    if(OamVarErrNoError == cont->length)
        {
        los_para.losopttime = OAM_NTOHS(para->tLosOptical);
        los_para.losmpcptime = OAM_NTOHS(para->tLosMpcp);    
        eponStack_CtlCfgLosCheckTime(EponSetOpe, &los_para); 
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetMacAgingTime - Set Ctc mac aging time
///
 // Parameters:
/// \param src      Container to source information
/// \param cont     Container to place information
///
/// \return
/// Pointer to last container
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetMacAgingTime (const OamVarContainer BULK *src,
                                       OamVarContainer BULK *cont)
    {
	U32 macAgingTime = ContGetU32(src);
	U64 macAgingTimeUl = macAgingTime*100/2;// system will check ageing_time twice, so use half of it to set driver.  

    if (macAgingTime == 0)
        {//set macAgingTimeUI to longest value
        //but the data value doesn't change for get response
        macAgingTimeUl = 0xFF0CCCCCCC;
        }
	
	FILE* f = NULL;
	f = fopen("/sys/class/net/br0/bridge/ageing_time", "w");
	if (f <= 0)
		{
		cont->length = OamVarErrAttrUnsupported;
		fclose(f);
		return;
		}

	fprintf(f, "%llu\n", macAgingTimeUl);

	fclose(f);
	
	macLearnCtcAgingTime = macAgingTime;

    if(!macCtcAgingTimeSet)
        macCtcAgingTimeSet = TRUE;
    
	cont->length = OamVarErrNoError;
    return ;
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
void OamCucSetPhyType(TkOnuEthPort port,
                          const OamVarContainer BULK* src,
                          OamVarContainer BULK* cont)
    {
    U8 portMode = ContGetU8(src);
    int speed, duplex;
    
    switch(portMode)
        {
        case OamCucPhy10MHalf:
            speed  = 10;
            duplex = 0;
            break;
        case OamCucPhy10MFull:
            speed  = 10;
            duplex = 1;
            break;
        case OamCucPhy100MHalf:
            speed  = 100;
            duplex = 0;
            break;
        case OamCucPhy100MFull:
            speed  = 100;
            duplex = 1;
            break;
        case OamCucPhy1G:
            speed  = 1000;
            duplex = 1;
            break;
        default:
            cont->length = OamVarErrAttrUnsupported;
            return;
            break;
        }

    if (bcm_phy_mode_set(0, bcm_enet_map_oam_idx_to_phys_port(port), speed, duplex))
        {
        cont->length = OamVarErrAttrHwFailure;
        }

    cont->length = OamVarErrNoError;
    }


////////////////////////////////////////////////////////////////////////////////
/// OamCucSetMacLimit - Set Cuc mac aging time
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
void OamCucSetMacLimit (TkOnuEthPort port,
                          const OamVarContainer BULK* src,
                          OamVarContainer BULK* cont)
    {
    U16 tableSize = ContGetU16(src);
    char devName[IFNAMSIZ];    
    
    if(tableSize == 0)
        {
        bcm_ifname_get(0, bcm_enet_map_oam_idx_to_phys_port(port), devName);
        MacLearnClearAll("br0", devName, 1);
        MacLearnClearAll("br0", devName, 0);
        }
    else
        {
        if (tableSize == 0xFFFF)
            tableSize = 0;
        
        if(!MacLearnSetMaxLimit("br0", port, tableSize))
            {
            cont->length = OamVarErrActNoResources;
            }
        else
            {
            cont->length = OamVarErrNoError;
            if(rdpaCtl_set_port_sa_limit(bcm_enet_map_oam_idx_to_phys_port(port), FALSE, tableSize))
                {
                cmsLog_error("set rdpa port %d sa limit (%d) fail\n", port, tableSize);
                cont->length = OamVarErrAttrHwFailure;
                }
            }
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetPfmStatus - Set Performance Monitor Status
///
 // Parameters:
/// \param port     port number
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetPfmStatus (TkOnuEthPort port,
                        OamVarContainer BULK * src,
                        OamVarContainer BULK * cont)
    {
    OamCtcTlvPfmStatus BULK *value =
        (OamCtcTlvPfmStatus BULK *)(&src->value[0]);

    if (OAM_NTOHS(value->enable) == OamCtcActionU16Enable)
        {
        CtcStatsSetEnable (port, TRUE);
        }
    else
        {
        CtcStatsSetEnable (port, FALSE);
        }

    if (value->period != 0)
        {
        CtcStatsSetPeriod (port, OAM_NTOHL(value->period));
        }
    else
        {
        cont->length = OamVarErrActBadParameters;
        }
    } // OamCtcSetPfmStatus


////////////////////////////////////////////////////////////////////////////////
/// OamCtcSetPfmCurData - Set Performance Monitor Current Data
///
 // Parameters:
/// \param port     port number
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetPfmCurData (TkOnuEthPort port,
                        OamVarContainer BULK * src,
                        OamVarContainer BULK * cont)
    {
    UNUSED(src);
    UNUSED(cont);
    if (!CtcStatsGetEnable (port))
        {
        cont->length = OamVarErrActNoResources;
        }
    else
        {
        CtcStatsClear (port);
       }
    }//OamCtcSetPfmCurData



////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleSetOnuAttr - Handle a set extended attribute branch
///
/// This function handles extended attribute requests.
///
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleSetOnuAttr (OamVarContainer BULK* src,
                             OamVarContainer BULK* cont)
    {
    cmsLog_debug("leaf = 0x%x", OAM_NTOHS(src->leaf));
    switch (OAM_NTOHS(src->leaf))
        {
        case OamCtcAttrMcastSwitch:
            OamCtcSetMcastSwitch (src, cont);
            break;

        case OamCtcAttrMcastCtrl:
            OamCtcSetMcastCtrl(src, cont);
            break;

        case OamCtcAttrServiceSla:
            OamCtcServiceSlaSet(src, cont);
            break;

        case OamCtcAttrOnuHoldover:
            OamCtcSetHoldover(src, cont);
            break;

        case OamCtcAttrOnuTxPower:
            OamCtcSetTxPower(src, cont);
            break;

        case OamCtcAttrAlarmAdminState:
            OamCtcSetAlarmAdminState(OamCtcObjOnu, TkOnuPortNotUsed, src, cont);
            break;

        case OamCtcAttrActPonIFAdmin:
            OamCtcSetActPonIfAdmin(src, cont);
            break;

        case OamCtcAttrOnuProtParam:
        // case OamCucAttrOnuCfgCnt:
            if (OamIeeeIsVendorNeg(0, OuiCuc))
                {
                OamCucSetOnuCfgCnt(src, cont);
                }
            else
                {
                OamCtcSetOnuProtParam(src, cont);
                }
            break;

        case OamCtcAttrOnuMacAgingTime:
            OamCtcSetMacAgingTime(src, cont);
            break;

        case OamCtcAttrPowerSavingCfg:   //OamCucAttrOnuMacAgingTime:
            if (OamIeeeIsVendorNeg(0, OuiCtc))
                {
                OamCtcPowerSaveCfgSet(src, cont);
                }
            else
                {
                OamCtcSetMacAgingTime(src, cont);
                }
            break;

        case OamCucAttrOnuProtParam:
            if (OamIeeeIsVendorNeg(0, OuiCuc))
                {
                OamCtcSetOnuProtParam(src, cont);
                }
            else
                {
                cont->length = OamVarErrAttrUnsupported;
                }
            break;

        default:
            cont->length = OamVarErrAttrUnsupported;
            break;
        }
    } // OamCtcHandleSetOnuAttr



////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleSetEthAttr - Handle a set extended attribute branch
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
void OamCtcHandleSetEthAttr (TkOnuEthPort port,
                             OamVarContainer BULK* src,
                             OamVarContainer BULK* cont)
    {
    cmsLog_debug("leaf = 0x%x", OAM_NTOHS(src->leaf));

    switch (OAM_NTOHS(src->leaf))
        {
        case OamCtcAttrEthPortPause:
            OamCtcSetEthPortPause (port, src, cont);
            break;

        case OamCtcAttrClassMarking:
            OamCtcSetClassMarking (port, src, cont);
            break;

        case OamCtcAttrEthPortPolice:
            OamCtcSetEthUsPolice (port, src, cont);
            break;

        case OamCtcAttrEthPortDsRateLimit:
            OamCtcSetEthDsShaper (port, src, cont);
            break;


        case OamCtcAttrPortLoopDetect:
            OamCtcSetPortLoopDetect(port, src, cont);
            break;

        case OamCtcAttrPortDisableLooped:
            OamCtcSetLoopedPortDisable(port, src, cont);
            break;

#ifdef DMP_TAENDPOINT_1			
        case OamCtcAttrVoipPort:
            OamCtcSetVoipPort(port, src, cont);
            break;
#endif

        case OamCtcAttrE1Port:
             cont->length = OamVarErrAttrUnsupported;
            break;

        case OamCucAttrOnuMacLimit:
            if (OamIeeeIsVendorNeg(0, OuiCuc))
                {
                OamCucSetMacLimit(port, src, cont);
                }
            else
                {
                cont->length = OamVarErrAttrUnsupported;
                } 
            break;

        case OamCucAttrOnuPortModeConfig:
            if (OamIeeeIsVendorNeg(0, OuiCuc))
                {
                OamCucSetPhyType(port, src, cont);
                }
            else
                {
                cont->length = OamVarErrAttrUnsupported;
                } 
            break;

        case OamCtcAttrVlan:
            OamCtcSetVlan (port, src, cont);
            break;

        case OamCtcAttrMcastVlan:
            OamCtcSetMcastVlan (port, src, cont);
            break;

        case OamCtcAttrMcastStrip:
            OamCtcSetMcastStrip (port, src, cont);
            break;

        case OamCtcAttrGroupMax:
            OamCtcSetGroupMax (port, src, cont);
            break;

        case OamCtcAttrAlarmAdminState:
            OamCtcSetAlarmAdminState(OamCtcObjPort, port, src, cont);
            break;

        case OamCtcAttrPrfrmMontrStatus:
            OamCtcSetPfmStatus (port+1, src, cont);;
            break;

        case OamCtcAttrPrfrmCurrtData:
            OamCtcSetPfmCurData (port+1, src, cont);
            break;
#if INCLUDE_PPPOE
        case OamZteAttrPppoEAuthenReq :
            {
            OamPPPoEAuthStruct BULK* pppoeAuth;

            pppoeAuth = (OamPPPoEAuthStruct BULK*)(src->value);

            if ((pppoeAuth->command == PPPOE_AUTH_START) ||
                (pppoeAuth->command == PPPOE_AUTH_STOP))
                {
                (void)OnuPPPoeSessionOamHandle(port, pppoeAuth);
                }
            else
                {
                cont->length = OamVarErrActBadParameters;
                printf("OAM: pppoe-auth err cmd %02bx\n", pppoeAuth->command);
                }
            break;
            }
#endif // INCLUDE_PPPOE
        default:
            cont->length = OamVarErrAttrUnsupported;
            break;
        }
    } // OamCtcHandleSetEthAttr



////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleSetPonIfAttr - Handle a set pon interface branch
///
/// This function handles pon interface requests.
///
 // Parameters:
///
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleSetPonIfAttr (const OamVarContainer BULK* src,
                               OamVarContainer BULK* cont)
    {
    cmsLog_debug("leaf = 0x%x", OAM_NTOHS(src->leaf));
    switch (OAM_NTOHS(src->leaf))
        {
        case OamCtcAttrAlarmAdminState:
            OamCtcSetAlarmAdminState(OamCtcObjPon, TkOnuPonNumIndex, src, cont);
            break;

#if SFF8472
        case OamCtcAttrPowerMonThreshold:
            OamCtcSetPowerMonThd(src, cont);
            break;
#endif
        case OamCtcAttrPrfrmMontrStatus:
            OamCtcSetPfmStatus (0, (OamVarContainer *)src, cont);
            break;

        case OamCtcAttrPrfrmCurrtData:
            OamCtcSetPfmCurData (0, (OamVarContainer *)src, cont);
            break;

        default:
            cont->length = OamVarErrAttrUnsupported;
            break;
        }
    } // OamCtcHandleSetPonIfAttr


////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleSetLlidAttr - Handle a set LLID branch
///
/// This function handles LLID interface requests.
///
 // Parameters:
///
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleSetLlidAttr (const OamVarContainer BULK* src,
                              OamVarContainer BULK* cont)
    {
    cmsLog_debug("leaf = 0x%x", OAM_NTOHS(src->leaf));
    switch (OAM_NTOHS(src->leaf))
        {
        case OamCtcAttrLlidQueueConfig :
            //CtcMLlidQueueConfigSet(OamContextLink(), src, cont);
            break;

        default:
            cont->length = OamVarErrAttrUnsupported;
            break;
        }
    } // OamCtcHandleSetLlidAttr



////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleSetExtAttr - Handle a set extended attribute branch
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
void OamCtcHandleSetExtAttr (OamVarContainer BULK* src,
                             OamVarContainer BULK* cont)
    {
    OamCtcObject BULK * obj;

    obj = OamCtcGetObject ((OamVarDesc BULK*)src);
	
    cmsLog_debug("objType = 0x%x", obj->objType);

    switch (obj->objType)
        {
        case OamCtcObjOnu:
            OamCtcHandleSetOnuAttr (src, cont);
            break;

        case OamCtcObjPort:
            if (obj->instNum.sNum.portType == OamCtcPortEth)
                {
                if (obj->instNum.sNum.portNum == OamCtcAllUniPorts)
                    {
                    TkOnuEthPort FAST port;

                    cont->length = OamVarErrNoError;
                    for (port= 0; port < UniCfgDbGetActivePortCount(); ++port)
                        {
                        OamCtcHandleSetEthAttr(port, src, cont);
                        }
                    }
                else if (OamCtcPortToEth (obj->instNum.sNum.portNum) <
                         UniCfgDbGetActivePortCount ())
                    {
                    OamCtcHandleSetEthAttr(
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
                    return;
                    }
				
                cmsLog_debug("maxLineCount: %d\n", voiceCapObj->maxLineCount);     
				
                if (obj->instNum.sNum.portNum == OamCtcAllUniPorts)
                    {
                    TkOnuEthPort FAST port;
                    cont->length = OamVarErrNoError;
                    for (port= 1; port <= voiceCapObj->maxLineCount; ++port)
                        {
                        OamCtcHandleSetEthAttr(port, src, cont);
                        }
                    }
               else if (obj->instNum.sNum.portNum <= voiceCapObj->maxLineCount)
                    {
                    OamCtcHandleSetEthAttr(obj->instNum.sNum.portNum, src, cont);
                    }
				
                else
                    {
                    cont->length = OamVarErrActBadParameters;
                    }
				
                cmsObj_free((void **)&voiceCapObj);
                cmsLck_releaseLock();
#endif
                }
#endif			
            else
                {
                cont->length = OamVarErrObjUnsupported;
                }
            break;

        case OamCtcObjLlid:
            if (OamContextLink() < 0x08)
                {
                OamCtcHandleSetLlidAttr (src, cont);
                }
            else
                {
                cont->length = OamVarErrActNoResources;
                }
            break;

        case OamCtcObjPon:
            if (obj->instNum.sNum.portNum < EponDevCtcPonIfNum())
                {
                OamCtcHandleSetPonIfAttr (src, cont);
                }
            else
                {
                cont->length = OamVarErrActNoResources;
                }
            break;

        case OamCtcObjCard:
        default:
            cont->length = OamVarErrObjUnsupported;
            break;
        }
    } // OamCtcHandleSetExtAttr


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle a set action branch
///
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply
///
/// \return
/// Reply container
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleSetAct(const OamVarContainer BULK* src,
                        OamVarContainer BULK* cont)
    {
    OamCtcObject BULK * obj;

    obj = OamCtcGetObject ((OamVarDesc BULK*)src);

    cmsLog_debug("objType = 0x%x", obj->objType);
    switch (obj->objType)
        {
        case OamCtcObjPort:
            if(obj->instNum.sNum.portType == OamCtcPortEth)
                {
                if (obj->instNum.sNum.portNum == OamCtcAllUniPorts)
                    {
                    TkOnuEthPort FAST port;

                    cont->length = OamVarErrNoError;
                    for (port = 0; port < UniCfgDbGetActivePortCount(); ++port)
                        {
                        OamContextReset(port + 1, 0);
                        OamAct(src, cont);
                        }
                    }
                else if (OamCtcPortToEth (obj->instNum.sNum.portNum) <
                         UniCfgDbGetActivePortCount ())
                    {
                    OamAct(src, cont);
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
            OamAct(src, cont);
            break;

        default:
            cont->length = OamVarErrObjUnsupported;
            break;
        }
    } // OamCtcHandleSetAct


//##############################################################################
// Extended Action Handling Functions
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle Extended Action Branch
///
/// \param src      pointer to var containers in request
/// \param cont     pointer to var containers in reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleExtAction (const OamVarContainer BULK* src,
                            OamVarContainer BULK* cont)
    {
/*dont know why to add these lines. comment out them, 
    otherwise, OamCtcAttrFastLeaveAdminCtl will have no 
    chance to set
*/
#if 0   
    if (OAM_NTOHS(src->leaf) != OamCtcActReset)
    {
        cont->length = OamVarErrAttrSystemBusy;
        return;
    }
    else
    {
        cont->length = OamVarErrNoError;
    }
#else
    if(OAM_NTOHS(src->leaf) == OamCtcActReset)
        cont->length = OamVarErrNoError;
#endif

    switch (OAM_NTOHS(src->leaf))
        {
        case OamCtcActReset:
            /* Save reboot reason to NVS? */
            system("reboot \n");
            exit(0); 
            break;

        case OamCtcActSleepControl:
            OamCtcSleepCtrlAct(src, cont);
            oamParser.dest.pift = PortIfNone;
            break;

        case OamCtcAttrFastLeaveAdminCtl: // 0x0048,
            OamCtcSetFastLeaveAdmin((OamVarContainer BULK *)src, cont);
            break;

        case OamCtcActMultiLlidAdminCtl:
            //if (!CtcMLlidAdminControl((U8)ContGetU32(src)))
                {
                cont->length = OamVarErrActBadParameters;
                }
            break;

        default:
            cont->length = OamVarErrActBadParameters;
            break;
        }
    } // OamCtcHandleExtAction


//##############################################################################
// Primary Variable Request Handling Functions
//##############################################################################


////////////////////////////////////////////////////////////////////////////////
/// \brief  Check registration on a variable container
///
/// \param  src     Pointer to var descriptors in request
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetVarRegCheck(const OamVarContainer BULK* src)
    {
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Process a variable container
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
void OamCtcSetVarProcess(const OamVarContainer BULK* src)
    {
    OamVarContainer BULK * cont = (OamVarContainer BULK *)oamParser.reply.cur;

    cont->branch = src->branch;
    cont->leaf = src->leaf;
    cont->length = OamVarErrNoError;
	
    cmsLog_notice("branch = 0x%x, leaf = 0x%x", src->branch, OAM_NTOHS(src->leaf));

    switch (src->branch)
        {
        case OamCtcBranchAttribute:
            OamSetAttr ((OamVarContainer *)src, cont);
            break;

        case OamCtcBranchAction:
            OamCtcHandleSetAct(src, cont);
            break;

        case OamCtcBranchObjInst:
        case OamCtcBranchObjInst21:
            OamCtcSetObject (src, cont);
            break;

        case OamCtcBranchExtAttribute:
            OamCtcHandleSetExtAttr ((OamVarContainer *)src, cont);
            break;

        case OamCtcBranchExtAction:
            OamCtcHandleExtAction (src, cont);
            break;

        default:
            cmsLog_notice("return act bad params!");
            cont->length = OamVarErrActBadParameters;
            break;
        }

    StreamSkip(&oamParser.reply, OamContSize(cont));
    } // OamCtcHandleSet


////////////////////////////////////////////////////////////////////////////////
/// \brief  Parse variable containers in a CTC OAM set request
///
/// \param  varAction   Action to perform for each variable container
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetParse(void (*varAction)(const OamVarContainer BULK *))
    {
    OamVarContainer BULK * src = (OamVarContainer BULK *)oamParser.src.cur;

    OamCtcClearObject();

    while (src->branch != OamCtcBranchTermination)
        {
        if ((U8*)src >= PointerAdd(RxFrame, rxFrameLength))
            { /* incoming packet used up */
            cmsLog_notice("OAM: HandleSet src frame unterminated");
            break;
            }
        
        varAction(src);

        // According to CTC spec, on OamActAutoRenegotiate, the OAM from the
        // OLT only contains Branch (0x09) Leaf(0x000B) fields, and DOES NOT
        // contain the Length indication(0x80) field.
        if ((src->branch == OamCtcBranchAction) &&
            (OAM_NTOHS(src->leaf) == OamActAutoRenegotiate) &&
            (src->length != OamVarErrNoError))
            {
            src = (OamVarContainer BULK *) &(src->length);
            }
        else
            {
            src = NextCont(src);
            }
        }
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcHandleSetRequest(void)
    {
    OamCtcSetParse(OamCtcSetVarRegCheck);
    StreamWriteU8(&oamParser.reply, OamCtcSetResponse);
    OamCtcSetParse(OamCtcSetVarProcess);
    OamCtcTerminateBranch();
    OamTransmit();
    } // OamCtcHandleSetRequest

#if LOOP_DETECT
void LoopDetectSetEnabled(U32 port, U32 mgmt)
{
#ifdef BRCM_CMS_BUILD
   char msg[sizeof(CmsMsgHeader) + sizeof(U32)] = {0};
   CmsMsgHeader *hdr = (CmsMsgHeader *)msg;
   char *body = &msg[sizeof(CmsMsgHeader)];

   hdr->type          = CMS_MSG_PORT_LOOP_DETECT;
   hdr->src           = EID_EPON_APP;
   hdr->dst           = EID_EPON_OAM_PORT_LOOP_DETECT;
   hdr->flags_request = 1;
   hdr->wordData      = port;
   hdr->dataLength    = sizeof(U32);
   *(U32 *)body       = mgmt;

   cmsMsg_send(msgHandle, hdr);
#else
   UNUSED(port);
   UNUSED(mgmt);
#endif
}  /* End of LoopDetectSetEnabled() */

void LoopDetectSetAutoBlock(U32 port, U32 action)
{
#ifdef BRCM_CMS_BUILD
   char msg[sizeof(CmsMsgHeader) + sizeof(U32)] = {0};
   CmsMsgHeader *hdr = (CmsMsgHeader *)msg;
   char *body = &msg[sizeof(CmsMsgHeader)];

   hdr->type          = CMS_MSG_PORT_DISABLE_LOOPED;
   hdr->src           = EID_EPON_APP;
   hdr->dst           = EID_EPON_OAM_PORT_LOOP_DETECT;
   hdr->flags_request = 1;
   hdr->wordData      = port;
   hdr->dataLength    = sizeof(U32);
   *(U32 *)body       = action;

   cmsMsg_send(msgHandle, hdr);
#else
   UNUSED(port);
   UNUSED(action);
#endif
}  /* End of LoopDetectSetAutoBlock() */
#endif

// End of File CtcOamSet.c
