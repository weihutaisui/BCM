//<:copyright-BRCM:2002:proprietary:epon 
//
//   Copyright (c) 2002 Broadcom 
//   All Rights Reserved
//
// This program is the proprietary software of Broadcom and/or its
// licensors, and may only be used, duplicated, modified or distributed pursuant
// to the terms and conditions of a separate, written license agreement executed
// between you and Broadcom (an "Authorized License").  Except as set forth in
// an Authorized License, Broadcom grants no license (express or implied), right
// to use, or waiver of any kind with respect to the Software, and Broadcom
// expressly reserves all rights in and to the Software and all intellectual
// property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
// NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
// BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
//
// Except as expressly set forth in the Authorized License,
//
// 1. This program, including its structure, sequence and organization,
//    constitutes the valuable trade secrets of Broadcom, and you shall use
//    all reasonable efforts to protect the confidentiality thereof, and to
//    use this information only in connection with your use of Broadcom
//    integrated circuit products.
//
// 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
//    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
//    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
//    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
//    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
//    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
//    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
//    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
//    PERFORMANCE OF THE SOFTWARE.
//
// 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
//    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
//    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
//    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
//    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
//    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
//    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
//    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
//    LIMITED REMEDY.
//:>

#include "CtcOnuOam.h"
#include "CtcOamDiscovery.h"
#include "CtcAlarms.h"
#include "CtcOamIf.h"
#include "CtcVlan.h"
#include "CtcOamFileTransfer.h"
#include "CtcEncryption.h"
#include "CtcStats.h"
#include "CtcOptDiag.h"

#include "OamOnu.h"
#include "OnuOs.h"
#include "OamIeee.h"
#include "OsAstMsgQ.h"
#include "OntmTimer.h"
#include "PonMgrEncrypt.h"

#include "eponctl_api.h"

#include "TkMsgProcess.h"
#include "OamProcessInit.h"
#include "rdpa_types.h"
#include "rdpactl_api.h"
#include "Alarms.h"

#include <linux/if_bridge.h>

#ifdef BRCM_CMS_BUILD
#include "rut_tmctl_wrap.h"
#include "cms_msg.h"
#endif

#include "cms_log.h"

extern BOOL BULK ctcIeeeOamProcessing;

#ifdef EPON_SFU	
static unsigned int cpuTowWanRuleId = 0;

////////////////////////////////////////////////////////////////////////////////
/// SetCpuToWanService - enable/disable EPON cpu to wan service
/// 
 // Parameters:
/// \param enable/disable
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
static 
void SetCpuToWanService(Bool enable)
{
    vlanCtl_init();
    if(enable)
    {
    /*enable cpu->wan traffic*/
        vlanCtl_initTagRule();
        vlanCtl_filterOnRxRealDevice("lo");
        vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
        vlanCtl_cmdSetSkbMarkPort(0);
        vlanCtl_insertTagRule(eponRealIfname, VLANCTL_DIRECTION_TX, 0, 
            VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &cpuTowWanRuleId);   
    }
	else
    {
    /*disable cpu->wan traffic*/
        vlanCtl_removeTagRule(
            eponRealIfname, VLANCTL_DIRECTION_TX, 0, cpuTowWanRuleId);
    }
    vlanCtl_cleanup();
}
#endif


void CtcDisableWanService(U8 link)
{
    cmsLog_notice("link=%d", link);
	
#ifdef EPON_SFU
    {
    int i;
    for (i = 0; i < UniCfgDbGetActivePortCount(); i++)
       CtcVlanSetMode(i, OamCtcVlanInit, NULL, 1);
    }

    SetCpuToWanService(TRUE);
#endif
    
    if(TRUE != send_notification_to_cms(link, FALSE))
    {
	cmsLog_error("stop wan_service fail");
    }
}

void CtcEnableWanService(U8 link)
{
    WanStatePara para;
    para.index = link;
    para.enable = TRUE;
    
    cmsLog_notice("link=%d", link);

    if(eponStack_CfgL2PonState(EponSetOpe, &para) == 0)
    {
#ifdef EPON_SFU
        if(TRUE != send_notification_to_cms(link, TRUE))
        {
           cmsLog_error("start wan_service fail");
        }
        else
        {
            int i;
            for (i = 0; i < UniCfgDbGetActivePortCount(); i++)
            {
                CtcVlanSetMode(i, OamCtcVlanTransparent, NULL, 1);
            }
	        SetCpuToWanService(TRUE);
        }
#else
        static BOOL isFirst = TRUE;

        if(isFirst)
        {
            char cmdStr[64];
            U8 i;
            U16 phyLlid = 0;
            U16 idxMcast = 16;
            UINT32 tagRuleId = VLANCTL_DONT_CARE;

            vlanCtl_init();
            vlanCtl_createVlanInterfaceByName(eponRealIfname, eponVeipIfname[EROUTER_VEIP_IDX], 1, 1);
            vlanCtl_setRealDevMode(eponRealIfname, BCM_VLAN_MODE_ONT);
            eponStack_CtlGetMcast (0, &phyLlid, &idxMcast, ActGetMcastLlidAndIdxByUcastIdx);
            
            for (i = 0; i < 3; i ++)
            {
            
                vlanCtl_setDefaultAction(eponRealIfname, VLANCTL_DIRECTION_TX, i, VLANCTL_ACTION_DROP, NULL);

                vlanCtl_initTagRule();
                vlanCtl_filterOnTxVlanDevice(eponVeipIfname[EROUTER_VEIP_IDX]);
                vlanCtl_cmdSetSkbMarkPort(0);
                vlanCtl_insertTagRule(eponRealIfname, VLANCTL_DIRECTION_TX, i,
                    VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE,
                    (unsigned int *)&tagRuleId);
                
                vlanCtl_initTagRule();
                vlanCtl_filterOnSkbMarkPort(0);
                vlanCtl_setReceiveVlanDevice(eponVeipIfname[EROUTER_VEIP_IDX]);
                vlanCtl_insertTagRule(eponRealIfname, VLANCTL_DIRECTION_RX, i,
                    VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE,
                    (unsigned int *)&tagRuleId);

                /*setup vlan rule for broadcast llid*/
                vlanCtl_initTagRule();
                vlanCtl_filterOnSkbMarkPort(idxMcast);
                vlanCtl_setReceiveVlanDevice(eponVeipIfname[EROUTER_VEIP_IDX]);
                vlanCtl_insertTagRule(eponRealIfname, VLANCTL_DIRECTION_RX, i,
                    VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE,
                    (unsigned int *)&tagRuleId);
            }
            vlanCtl_cleanup();

            memset(cmdStr, 0,sizeof(cmdStr));
            snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s up", eponVeipIfname[EROUTER_VEIP_IDX]);
            system(cmdStr);
        }
        if(TRUE != send_notification_to_cms(link, TRUE))
        {
           cmsLog_error("start wan_service fail");
        }
        isFirst = FALSE;
#endif
    }
    else
    {
        cmsLog_error("failed to set L2 Pon state in epon stack");
    } 
}

BOOL CtcNLMsgNewLinkHandleIf(U8 state, U8 port)
{
    if( state == BR_STATE_BLOCKING) 
    {
        CtcAlmSetCond(OamCtcAttrEthPortFailure,port, AlmInfoLen0, 0ULL);
        CtcAlmSetCond(OamCtcAttrEthPortLoopback,port, AlmInfoLen0, 0ULL);
        return TRUE;
    }
    else  if( state == BR_STATE_FORWARDING) 
    {
        CtcAlmClrCond(OamCtcAttrEthPortLoopback,port, AlmInfoLen0, 0ULL);
        CtcAlmClrCond(OamCtcAttrEthPortFailure,port, AlmInfoLen0, 0ULL);
        return TRUE;
    }

    return FALSE;
}


void CtcCmsMsgEthHandleIf(CmsMsgType type, U8 inst)
{    
    if(type == CMS_MSG_ETH_LINK_DOWN) 
    {
        CtcStatsSetPortChg (inst, FALSE);
        OsAstMsgQSet (OsAstAlmPortLinkDown, inst, 0);
    }
    else
    {
        CtcStatsSetPortChg (inst, TRUE);
        OsAstMsgQClr (OsAstAlmPortLinkDown, inst, 0);
    }
}

void CtcLinkResetHandleIf(U8 inst, U8 stat)
{
    OsAstMsgQClr (OsAstAlmLinkCtcOamDiscComplete, inst, stat);

    //the link down, just reset encryption information
    OamCtcEncryptResetLink(inst);
}

void CtcAlarmNotifyHandleIf(AlmCondition cond, U8 inst, StatId stat)
{        
    CtcAlmCont BULK ctcAlmCont;
    if(CtcAlmCondCreate(&ctcAlmCont, cond, inst, stat))
        {
        CtcAlmTlvAdd(&ctcAlmCont);
        }
}

void Ctc100msHandleIf(void)
{
    OamCtcProcess100MsTimer(); 
    CtcOamFile100MsTimer();
    CtcOptDiagPoll();
}

void Ctc1sHandleIf(void)
{
    CtcStats1sTimer();
    CtcAlm1sTimer();
    AlarmLedPoll();
}

void VendorCtcOamInit(void)
{
    OamCtcInit();

    CtcOptDiagInit();
    CtcStatsInit();

    OnuOsAssertHandleSet(OsAstAlmLinkCtcOamDiscComplete, CtcEnableWanService);
    OnuOsAssertClearHandleSet(OsAstAlmLinkCtcOamDiscComplete, CtcDisableWanService);
}

void CtcOamInfoTlvGetIf(OuiVendor oam, LinkIndex link, U8 BULK* msg, U8 *len)
{
	*len = 0;
    if (oam != OuiCtc)
        return;
    
    *len = CtcOamInfoTlvGet(link, msg);
}


void CtcOamInfoTlvHandleIf(OuiVendor oam, LinkIndex link, const OamInfoTlv BULK* tlv)
{
    if (oam != OuiCtc)
        return;

    CtcOamInfoTlvHandle(link, (OamCtcInfoTlvHeader *)tlv);
}

void OamCtcHandleIf(OuiVendor oam, LinkIndex link, U8 BULK* msg)
{
    if (oam != OuiCtc)
        return;

    ctcIeeeOamProcessing = TRUE;
    OamCtcHandle(link, msg);
    ctcIeeeOamProcessing = FALSE;
}

void OamCtcTimerHandleIf(OuiVendor oam, OntmTimerId timerId)
{
    if (oam != OuiCtc)
        return;
    
    OamCtcTimerHandle(timerId);
}

int VendorCtcDataPathInit(void)
{
    int rc = 0;
    U8 link_id;
    
#ifdef BRCM_CMS_BUILD
    tmctl_if_t tm_if;
    U8 qIndex;
#endif
    
    for (link_id = 0; link_id < DEF_CTC_LINK_NUM; link_id++)
    {
        if ((rc = rdpaCtl_LlidCreate(link_id)))
        {
            cmsLog_error("rdpaCtl_LlidCreate() failed: link(%u) rc(%d)\n", link_id, rc);  
            break;
        }
        
#ifdef BRCM_CMS_BUILD
        tm_if.eponIf.llid = link_id;
        if (rutoam_tmctl_portTmInit(TMCTL_DEV_EPON, &tm_if, TMCTL_SCHED_TYPE_SP) != TMCTL_SUCCESS)
        {
            cmsLog_error("Failed to setup tm for llid %d\n", link_id);
            break;
        }
        
        /* create sub_tm and add queue */
        for (qIndex = DEF_CTC_SLLID_Q_NUM; qIndex > 0; qIndex--)
        {
            tmctl_queueCfg_t tmctl_queueCfg;
            
            memset(&tmctl_queueCfg, 0, sizeof(tmctl_queueCfg));
            tmctl_queueCfg.qid = qIndex - 1;
            tmctl_queueCfg.priority = qIndex - 1;
            tmctl_queueCfg.qsize = TMCTL_DEF_ETH_Q_SZ_US;
            tmctl_queueCfg.weight = 0;
            tmctl_queueCfg.schedMode = TMCTL_SCHED_SP;
            if (rutoam_tmctl_setQueueCfg(TMCTL_DEV_EPON, &tm_if, &tmctl_queueCfg) != TMCTL_SUCCESS)
            {
                cmsLog_error("rutoam_tmctl_setQueueCfg failed :llid=%d, qid=%d\n",
                    link_id, qIndex);
                break;
            }
        }
#endif
    }
    return rc;
}


void VendorCtcInit(U8 VendorMask)
{
    // create a CB structure
    OamHandlerCB *ctcCbPtr;

    ctcCbPtr = OamProcessAllocCB();
    if (ctcCbPtr == NULL)
        return;

    if ((!VendorMask) || 
        (VendorMask&OAM_CTC_SUPPORT) || (VendorMask&OAM_CUC_SUPPORT))
        {
        ctcCbPtr->isActive = TRUE;
        printf("CTC OAM stack registered\n");
        }
    
    // register feature callback
    ctcCbPtr->VendorOamInit = VendorCtcOamInit;
    ctcCbPtr->VendorLinkInit = CtcLinkInit;
    ctcCbPtr->VendorOamInfoTlvGet = CtcOamInfoTlvGetIf;
    ctcCbPtr->VendorOamInfoTlvHandle= CtcOamInfoTlvHandleIf;
    ctcCbPtr->VendorOamVendorHandle= OamCtcHandleIf;
    ctcCbPtr->VendorLinkResetHandle = CtcLinkResetHandleIf;
    ctcCbPtr->VendorAlarmNotify = CtcAlarmNotifyHandleIf;
    //ctcCbPtr->VendorEncKeyExchange;
    ctcCbPtr->VendorOamPoll = OamCtcPoll;

    // register timer callback
    ctcCbPtr->Timer100msHandler = Ctc100msHandleIf;
    ctcCbPtr->Timer1sHandler = Ctc1sHandleIf;
    ctcCbPtr->PollTimerHandler = OamCtcTimerHandleIf;

    ctcCbPtr->CmsMsgEthLinkStHandler = CtcCmsMsgEthHandleIf;
    ctcCbPtr->NLMsgNewLinkHandler = CtcNLMsgNewLinkHandleIf;
}

// end CtcOamIf.c
