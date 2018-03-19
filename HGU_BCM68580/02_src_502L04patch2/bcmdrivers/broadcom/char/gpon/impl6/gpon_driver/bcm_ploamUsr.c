/* 
 * <:copyright-BRCM:2007:proprietary:gpon
 * 
 *    Copyright (c) 2007 Broadcom 
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
 */ /*impl4 API:*/
#include <bcmtypes.h>
#include <bcm_ploam_api.h>
#include "bcm_ploamUsr.h"
#include <linux/bcm_log.h>
#include <rdpa_api.h>
#include <rdpa_gpon.h>
#include <rdpa_ag_gpon.h>
#include "bcm_ploam.h"
#include "bcm_ploamCounterMon.h"
#include "bcm_ploamEvent.h"
#include "bcm_ploamAlarm.h"
#include "wan_drv.h"
#ifdef CONFIG_BCM_GPON_TODD
#include <gpon_tod_gpl.h>
#endif

#ifndef PLOAM_DEVICE_MAJOR
#define PLOAM_DEVICE_MAJOR            3019
#endif /*PLOAM_DEVICE_MAJOR*/
#ifndef USER_PLOAM_DEVICE_MAJOR
#define USER_PLOAM_DEVICE_MAJOR       3017
#endif /*USER_PLOAM_DEVICE_MAJOR*/

#ifndef USER_PLOAM_TOD_DEVICE_MAJOR
#define USER_PLOAM_TOD_DEVICE_MAJOR   3007
#endif /*USER_PLOAM_TOD_DEVICE_MAJOR*/

#define UNCONFIGURE_TCONT_ID 255

/**
 * Local Types
 **/
typedef struct {
  wait_queue_head_t ploamWriteWaitQueue;
  wait_queue_head_t ploamReadWaitQueue;
  wait_queue_head_t eventReadWaitQueue;
  struct semaphore ploamSem;
  struct semaphore usrPloamSem;
  wait_queue_head_t ploamTodReadWaitQueue;
  struct semaphore usrPloamTodSem;
  BCM_PloamEventState *eventObjp;
  bool initialized;
  bool created;
} BCM_PloamUsrState;

/**
 * Local variables
 **/
static int ploamMajor = PLOAM_DEVICE_MAJOR;
static int userPloamMajor = USER_PLOAM_DEVICE_MAJOR;
static BCM_PloamUsrState ploamUsrState;
static BCM_PloamUsrState *ploamUsrStatep = &ploamUsrState;
static int ploamIOCsetSerialPasswd(BCM_Ploam_SerialPasswdInfo* serialPasswdInfop);
static int get_gem_object_by_port_or_flow(UINT16 *gemPortIndex, UINT16 gemPortID, bdmf_object_handle *gem);
extern bdmf_object_handle gponObj;

typedef int (PloamIOCfun)(void* arg);

typedef struct {
    uint8_t cmdId;
    PloamIOCfun* fun;
    int argSize;
    bool checkArg;
} PloamIOCentry;

static int prbs_mode, prbs_enabled;

static int ploamIOCgetEventStatus(BCM_Ploam_EventStatusInfo* eventStatusInfop) {
    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");

  eventStatusInfop->eventBitmap = bcm_ploamEventRead(ploamUsrStatep->eventObjp);
  eventStatusInfop->eventMask = ~bcm_ploamEventGetMask(ploamUsrStatep->eventObjp);

  return 0;
}

static int ploamIOCmaskEvent(BCM_Ploam_EventMaskInfo* eventMaskInfop) {
  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");
  bcm_ploamEventSetMask(ploamUsrStatep->eventObjp, ~eventMaskInfop->eventMask);

  return 0;
}

static int ploamIOCgetAlarmStatus(BCM_Ploam_AlarmStatusInfo* alarmStatusInfop) {
    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");

  bcm_ploamAlarmStatus(alarmStatusInfop);
    alarmStatusInfop->alarmMaskBitmap ^= 0xffffffff;

    return 0;
}

static int ploamIOCmaskAlarm(BCM_Ploam_MaskAlarmInfo* maskAlarmInfop) {
    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");
  bcm_ploamAlarmSetMask(~maskAlarmInfop->alarmMaskBitmap);
    return 0;
}

static int ploamIOCsetSfSdThreshold(BCM_Ploam_SFSDthresholdInfo* sfsdThresholdInfop) {
    rdpa_gpon_link_cfg_t link_cfg;
    bdmf_error_t rc;

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");

    bdmf_lock();
    rc = rdpa_gpon_link_cfg_get(gponObj, &link_cfg);
    if (rc < 0)
        goto exit;

    link_cfg.ber_threshold_for_sf_assertion = sfsdThresholdInfop->sf_exp;
    link_cfg.ber_threshold_for_sd_assertion = sfsdThresholdInfop->sd_exp;
    rc = rdpa_gpon_link_cfg_set(gponObj, &link_cfg);

exit:
    bdmf_unlock();
    BCM_LOG_DEBUG (BCM_LOG_ID_PLOAM,
        "ploamIOCsetSfSdThreshold: sf = %d sd = %d rc = %d \n",
        sfsdThresholdInfop->sf_exp, sfsdThresholdInfop->sd_exp, rc);
    return rc;
}

static int ploamIOCgetSfSdThreshold(BCM_Ploam_SFSDthresholdInfo* sfsdThresholdInfop) {
    bdmf_error_t rc = BDMF_ERR_OK;
    rdpa_gpon_link_cfg_t pon_param;

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");

    rc = rdpa_gpon_link_cfg_get(gponObj, &pon_param);
    if(rc < 0)
        return rc;

    sfsdThresholdInfop->sf_exp = pon_param.ber_threshold_for_sf_assertion;
    sfsdThresholdInfop->sd_exp = pon_param.ber_threshold_for_sd_assertion;

    BCM_LOG_DEBUG (BCM_LOG_ID_PLOAM," get gpon link cfg: sf = %d sd = %d rc = %d\n",
        sfsdThresholdInfop->sf_exp, sfsdThresholdInfop->sd_exp,rc);

    return 0;
}



static int ploamIOCstart(BCM_Ploam_StartInfo* startInfop) {
    return bcm_ploamStart(startInfop->initOperState);
}

static int ploamIOCstop(BCM_Ploam_StopInfo* stopInfop) 
{
    bdmf_error_t rc = BDMF_ERR_OK;
    rdpa_pon_link_state link_state;

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"sendDyingGasp = %d\n", stopInfop->sendDyingGasp);

    if (stopInfop->sendDyingGasp)
    {
        bdmf_lock();
        rc = rdpa_gpon_pon_link_state_get(gponObj, &link_state);
        if (rc != BDMF_ERR_OK)
        {
            BCM_LOG_NOTICE( BCM_LOG_ID_PLOAM,"rdpa_gpon_pon_link_state_get: error = %d", rc); 
            bdmf_unlock();
            return rc;
        }

        if (link_state.link_oper_state == rdpa_pon_operational_o5)
        {
            rc = bcm_ploamSendDyingGasp();

            if (rc != BDMF_ERR_OK)
            {
                BCM_LOG_NOTICE( BCM_LOG_ID_PLOAM,"bcm_ploamSendDyingGasp: error = %d", rc); 
            }
        }
        else
        {
            BCM_LOG_NOTICE( BCM_LOG_ID_PLOAM_FSM, "ONU is not in operation state. Dying Gasp is not sent" );
        }

        bdmf_unlock();
    }

    udelay (1000);

    rc = rdpa_gpon_link_activate_set(gponObj, rdpa_link_deactivate);

    BCM_LOG_NOTICE(BCM_LOG_ID_PLOAM, "\nploamIOCstop: error = %d \n", rc);

    return rc;
}

static int ploamIOCgetState(BCM_Ploam_StateInfo* stateInfop) {
    bdmf_error_t rc = BDMF_ERR_OK;
    rdpa_pon_link_state pon_link_state;

    rc = rdpa_gpon_pon_link_state_get(gponObj, &pon_link_state);

    switch (pon_link_state.admin_state)
    {
    case rdpa_pon_admin_state_not_ready:
    case rdpa_pon_admin_state_inactive:
        stateInfop->adminState = BCM_PLOAM_ASTATE_OFF ;
        break;
    case rdpa_pon_admin_state_active:
        stateInfop->adminState = BCM_PLOAM_ASTATE_ON ;
        break;
    default:
        return (-1);
    }


    switch (pon_link_state.link_oper_state)
    {
    case rdpa_pon_init_o1:
        stateInfop->operState = BCM_PLOAM_OSTATE_INITIAL_O1 ;
        break;
    case rdpa_pon_standby_o2:
        stateInfop->operState = BCM_PLOAM_OSTATE_STANDBY_O2 ;
        break;
    case rdpa_pon_serial_num_o3:
        stateInfop->operState = BCM_PLOAM_OSTATE_SERIAL_NUMBER_O3 ;
        break;
    case rdpa_pon_ranging_o4:
        stateInfop->operState = BCM_PLOAM_OSTATE_RANGING_O4 ;
        break;
    case rdpa_pon_operational_o5:
        stateInfop->operState = BCM_PLOAM_OSTATE_OPERATION_O5 ;
        break;
    case rdpa_pon_popup_o6:
        stateInfop->operState = BCM_PLOAM_OSTATE_POPUP_O6 ;
        break;
    case rdpa_pon_emergency_stop_o7:
        stateInfop->operState = BCM_PLOAM_OSTATE_EMERGENCY_STOP_O7 ;
        break;
    default:
        return (-1);
    }

    return 0;
}


static int ploamIOCgetMsgCounters(BCM_Ploam_MessageCounters* msgCountersp) {
    BCM_PloamSwCounters swCounters;

    bcm_ploamGetCounters(&swCounters, msgCountersp->reset);

    msgCountersp->crcErrors = bcm_ploamCounterMonGetLinkCounter(BCM_GPON_MACDS_COUNTER_UERR_PLOAM,
        msgCountersp->reset);
    msgCountersp->rxPloamsBcast = swCounters.rxPloamsBcast;
    msgCountersp->rxPloamsDiscarded = swCounters.rxPloamsDiscarded;
    msgCountersp->rxPloamsNonStd = swCounters.rxPloamsNonStd;
    msgCountersp->rxPloamsTotal = swCounters.rxPloamsTotal;
    msgCountersp->rxPloamsUcast = swCounters.rxPloamsUcast;
    msgCountersp->txPloams = swCounters.txPloams;
    msgCountersp->txPloamsNonStd = swCounters.txPloamsNonStd;

    return 0;
}

static int ploamIOCgetGtcCounters(BCM_Ploam_GtcCounters* gtcCountersp) 
{
    gtcCountersp->bipErr = bcm_ploamCounterMonGetLinkCounter(BCM_GPON_MACDS_COUNTER_BIP_ERROR,
        gtcCountersp->reset);

    return 0;
}

static int ploamIOCgetFecCounters(BCM_Ploam_fecCounters* fec_counter) 
{
    return bcm_ploamCounterMonGetFecCounter(fec_counter);
}


static int readGemCounters(int index, BCM_Ploam_GemPortCounters* gemPortCountersp)
{
    int rc;
    BCM_Ploam_GemPortCounters gemCounters;

    rc = bcm_ploamIOCgetGemCounters(gemPortCountersp->reset, index, &gemCounters);

    if (rc == 0) 
    {
        /* convert structures */
        gemPortCountersp->rxBytes += gemCounters.rxBytes;
        gemPortCountersp->rxFragments += gemCounters.rxFragments;
        gemPortCountersp->rxFrames += gemCounters.rxFrames;
        gemPortCountersp->rxDroppedFrames += gemCounters.rxDroppedFrames;
        gemPortCountersp->rxMcastAcceptedFrames += gemCounters.rxMcastAcceptedFrames;
        gemPortCountersp->rxMcastDroppedFrames += gemCounters.rxMcastDroppedFrames;
        gemPortCountersp->txBytes += gemCounters.txBytes;
        gemPortCountersp->txFragments += gemCounters.txFragments;
        gemPortCountersp->txFrames += gemCounters.txFrames;
        gemPortCountersp->txDroppedFrames += gemCounters.txDroppedFrames;
   }

   return rc;
}

static int ploamIOCgetGemPortCounters(BCM_Ploam_GemPortCounters* gemPortCountersp) 
{
    int rc = 0;
    int index;
    UINT16 gemFlow;

    if (INVALID_GEM_INDEX_ETH_ALL(gemPortCountersp->gemPortIndex) ||
        INVALID_GEM_PORT(gemPortCountersp->gemPortID))
    {
        return -EINVAL_PLOAM_GEM_PORT;
    }

    /* Reset counters */
    gemPortCountersp->rxBytes = 0;                      
    gemPortCountersp->txBytes = 0;                      
    gemPortCountersp->rxFragments = 0;                  
    gemPortCountersp->txFragments = 0;                  
    gemPortCountersp->txFrames = 0;                     
    gemPortCountersp->rxFrames = 0;                     
    gemPortCountersp->rxDroppedFrames = 0;              
    gemPortCountersp->txDroppedFrames = 0;              
    gemPortCountersp->rxMcastAcceptedFrames = 0;        
    gemPortCountersp->rxMcastDroppedFrames = 0;         

    /* Read all counters */
    if (gemPortCountersp->gemPortIndex == BCM_PLOAM_GEM_PORT_IDX_ALL) 
    {
        for(index=0; index < RDPA_MAX_GEM_FLOW; index++)
        {
            readGemCounters(index, gemPortCountersp);
        }
    }
    else if (gemPortCountersp->gemPortIndex == BCM_PLOAM_GEM_PORT_IDX_ETH_ALL) 
    {
        BCM_OmciPortInfo omciPortInfo;
		
        bcm_ploamPortCtrlGetOmciPortInfo(&omciPortInfo);

        for (index=0; index < RDPA_MAX_GEM_FLOW; index++)
        {
            if (index != omciPortInfo.portId)
            {
                readGemCounters(index, gemPortCountersp);
            }
        }
    }
    else 
    {
        /* Found specific counter according to Port */
        if (gemPortCountersp->gemPortIndex == BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED) 
        {
            if (gemPortCountersp->gemPortID == BCM_PLOAM_GEM_PORT_ID_UNASSIGNED)
            {
                return -EINVAL_PLOAM_GEM_PORT;
            }
            else 
            {
                rc = get_gem_flow_id_by_gem_port((bdmf_number)gemPortCountersp->gemPortID, &gemFlow);
                if (rc < 0)
                {
                    BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_PORT, 
                        "ploamIOCgetGemPortCounters: failed to find GEM port; rc=%d\n", rc);
                    return rc;
                }
            }
        }
        /* Found specific counter according to flow index */
        else
        {
            gemFlow = gemPortCountersp->gemPortIndex;
        }

        readGemCounters(gemFlow, gemPortCountersp);
    }

    return rc;
}

static int ploamIOCcfgGemPort(BCM_Ploam_CfgGemPortInfo* cfgGemPortInfop)
{
    return bcm_cfgGemPort (cfgGemPortInfop);
}


static int ploamIOCsetGemPortEncryption(BCM_Ploam_GemPortEncryption* dsGemEncription)
{
    int rc = 0;

    if (INVALID_GEM_INDEX(dsGemEncription->gemIndex))  /* Configure by port */
    {
       rc = bcm_ploamPortCtrlEncryptGemPortById(dsGemEncription->gemPortId, dsGemEncription->encryption);
       if (rc == -ENOENT) 
           BCM_LOG_ERROR (BCM_LOG_ID_PLOAM, "Gem port doesn't exist: GEM PORT ID %d error %d\n", dsGemEncription->gemPortId, rc);
    }
    else
    {
       rc = bcm_ploamPortCtrlEncryptGemPortByIdx(dsGemEncription->gemIndex, dsGemEncription->encryption);
       if (rc == -ENOENT) 
           BCM_LOG_ERROR (BCM_LOG_ID_PLOAM, "Gem port doesn't exist : Index %d error %d\n", dsGemEncription->gemIndex, rc);
    }
    return rc;
}

static int ploamIOCgetTcontCfg(BCM_Ploam_TcontInfo* tcontInfop) {
    bdmf_number alloc_id;
    bdmf_boolean ploam_flag;
    bdmf_error_t rc = BDMF_ERR_OK;
    bdmf_object_handle tcont;
    bdmf_object_handle scheduler = NULL;
    rdpa_tm_sched_mode policy=rdpa_tm_sched_disabled;
    int tcontID = 0;
  
    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM_PORT,"");
    BCM_ASSERT(tcontInfop);
  
    if (INVALID_TCONT_INDEX(tcontInfop->tcontIdx))
        return -EINVAL_PLOAM_ARG;
    
    tcontID = tcontInfop->tcontIdx;
    SET_TCONT_IDX(tcontID);

    memset(tcontInfop, 0, sizeof (BCM_Ploam_TcontInfo));

    rc = rdpa_tcont_get(tcontID, &tcont);
    if(rc < 0)
    {
        BCM_LOG_ERROR (BCM_LOG_ID_PLOAM_PORT, "ploamIOCgetTcontCfg: can't get tcont error %d\n",rc);
        return EINVAL_PLOAM_RESOURCE_UNAVAIL;
    }
  
    rc = rdpa_gpon_tcont_alloc_id_get(gponObj, tcont, &alloc_id);
    if(rc < 0)
    {
         BCM_LOG_ERROR (BCM_LOG_ID_PLOAM_PORT, "ploamIOCgetTcontCfg: can't get tcont alloc error %d\n",rc);
        goto exit;
    }

    tcontInfop->enabled = true;
    tcontInfop->allocID = alloc_id;

    rc = rdpa_gpon_tcont_assign_ploam_flag_get(gponObj, tcont, &ploam_flag);
    if(rc < 0)
    {
        BCM_LOG_ERROR (BCM_LOG_ID_PLOAM_PORT, "ploamIOCgetTcontCfg: can't get tcont ploam flag error %d\n",rc);
        goto exit;
    }
    if (ploam_flag)
        tcontInfop->ploamCreated = true;

    rc = rdpa_tcont_egress_tm_get(tcont, &scheduler );    
    if(rc < 0)
    {
        if (scheduler==NULL)
            tcontInfop->schdPolicy = TCONT_SCHD_POLICY_NONE;
        else
        {
            BCM_LOG_ERROR (BCM_LOG_ID_PLOAM_PORT, "ploamIOCgetTcontCfg: can't get tcont scheduler %d\n",rc);
        goto exit;
        }
    }
  
    tcontInfop->refCount = 1;
    if (scheduler==NULL)
        tcontInfop->schdPolicy = TCONT_SCHD_POLICY_NONE;
    else
    {
        rc = rdpa_egress_tm_mode_get(scheduler,&policy);
        if(rc < 0)
        {
            BCM_LOG_ERROR (BCM_LOG_ID_PLOAM_PORT, "ploamIOCgetTcontCfg: can't get tcont policy %d\n",rc);
            goto exit;
        }
     }
     if ( policy == rdpa_tm_sched_sp )
         tcontInfop->schdPolicy = TCONT_SCHD_POLICY_SP;
     else if ( policy == rdpa_tm_sched_wrr )
         tcontInfop->schdPolicy = TCONT_SCHD_POLICY_WRR;
     else
         tcontInfop->schdPolicy = TCONT_SCHD_POLICY_NONE;
  
     BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"tcontIdx %d created by omci and enabled, ploam arrived %u allocId %d ",
         tcontInfop->tcontIdx,tcontInfop->ploamCreated, tcontInfop->allocID);  

exit:
    bdmf_put(tcont);
    return rc;
}

static int ploamIOCcfgTcontAllocId(BCM_Ploam_TcontAllocIdInfo* cfgTcontp) 
{
    bdmf_error_t rc = BDMF_ERR_OK, rc_exist = BDMF_ERR_OK;
    bdmf_object_handle tcont = NULL;        
    BDMF_MATTR(tcont_attrs, rdpa_tcont_drv());
    int tcontID = 0;

    BCM_LOG_DEBUG( BCM_LOG_ID_PLOAM, "ploamIOCcfgTcontAllocId: tcontIdx=0x%x, allocID=0x%x\n", 
        cfgTcontp->tcontIdx, cfgTcontp->allocID);

    if (INVALID_TCONT_INDEX(cfgTcontp->tcontIdx) || INVALID_ALLOC(cfgTcontp->allocID))
        return -EINVAL_PLOAM_ARG;

    tcontID = cfgTcontp->tcontIdx;
    SET_TCONT_IDX(tcontID);

    if ( cfgTcontp->allocID != BCM_PLOAM_ALLOC_ID_UNASSIGNED ) 	
    {
        /* configure tcont */
        rc_exist = rdpa_tcont_get(tcontID, &tcont);
        if (rc_exist != BDMF_ERR_OK) /* New TCONT Object */
        {
            rc = rdpa_tcont_index_set(tcont_attrs, tcontID);
            rc = rc ? rc : bdmf_new_and_set(rdpa_tcont_drv(), NULL, tcont_attrs, &tcont);
            if (rc < 0) 
            {
                BCM_LOG_NOTICE(BCM_LOG_ID_PLOAM, "Create Object TCONT=%d, RDPA TCONT=%d, error = %d",
                    cfgTcontp->tcontIdx, tcontID, rc);
                return rc;
            }
        }

        /* Configure / Modify Alloc ID */
        rc = rdpa_gpon_tcont_alloc_id_set(gponObj, tcont, cfgTcontp->allocID);
        if (rc < 0) 
        {
            BCM_LOG_NOTICE(BCM_LOG_ID_PLOAM, "Config GPON allocID=%d failed, error = %d",
                cfgTcontp->allocID, rc);
        }
        
        /* Modify or New TCONT create - GPON configuration failed */ 
        if (rc_exist == BDMF_ERR_OK || rc < 0) 
            bdmf_put(tcont);
    }

    return rc;
}

static int ploamIOCdecfgTcontAllocId(BCM_Ploam_TcontAllocIdInfo* cfgTcontp) 
{
    bdmf_error_t rc = BDMF_ERR_OK;
    bdmf_object_handle tcont = NULL;        
    int tcontID = 0;

    BCM_LOG_DEBUG( BCM_LOG_ID_PLOAM, "ploamIOCdecfgTcontAllocId: tcontIdx=0x%x, allocID=0x%x\n", 
        cfgTcontp->tcontIdx, cfgTcontp->allocID);

    if (INVALID_TCONT_INDEX(cfgTcontp->tcontIdx))
        return -EINVAL_PLOAM_ARG;

    tcontID = cfgTcontp->tcontIdx;
    SET_TCONT_IDX(tcontID) ;
    
    rc = rdpa_tcont_get(tcontID, &tcont);
    if (rc)
    {
        BCM_LOG_NOTICE(BCM_LOG_ID_PLOAM, "Remove failed OMCI TCONT=%d, RDPA TCONT=%d, allocID=%d, error = %d",
            cfgTcontp->tcontIdx, tcontID, cfgTcontp->allocID, rc);
        return rc;
    }
    
    rc = rdpa_gpon_tcont_alloc_id_delete(gponObj, tcont);
    bdmf_put(tcont);
    if (rc)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM, "Remove TCONT=%d from GPON failed, error = %d",
            cfgTcontp->tcontIdx, rc);
        return rc;
    }
    
    bdmf_destroy(tcont);
    
    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM, "removed OMCI TCONT=%d, RDPA TCONT=%d, allocID=%d ",
        cfgTcontp->tcontIdx, tcontID, cfgTcontp->allocID);
    
    return rc;
}




int is_xgpon(void)
{
    bdmf_object_handle port_obj;
    static rdpa_wan_type wan_type = rdpa_wan_none;
    rdpa_if wan_if = rdpa_if_none;
    int rc;

    if (wan_type != rdpa_wan_none)
        return wan_type == rdpa_wan_xgpon;
    wan_if = rdpa_wan_type_to_if(rdpa_wan_xgpon);
    rc = rdpa_port_get(wan_if, &port_obj);
    if (rc)
    {
        BCM_LOG_INFO(BCM_LOG_ID_PLOAM_PORT, "RDPA WAN type not configured!?");
        return -1;
    }

    rdpa_port_wan_type_get(port_obj, &wan_type);
    bdmf_put(port_obj);

    return wan_type == rdpa_wan_xgpon;
}

static int ploamIOCdecfgGemPort(BCM_Ploam_DecfgGemPortInfo* decfgGemPortInfop) 
{
    bdmf_object_handle gem = NULL;
    int rc;

    if (INVALID_GEM_INDEX_ALL(decfgGemPortInfop->gemPortIndex) ||
        INVALID_GEM_PORT(decfgGemPortInfop->gemPortID))
    {
        return -EINVAL_PLOAM_ARG;
    }

    /* Remove all GEM flows */
    if (decfgGemPortInfop->gemPortIndex == BCM_PLOAM_GEM_PORT_IDX_ALL) 
    {
        while ((gem = bdmf_get_next(rdpa_gem_drv(), gem, NULL)))
        {
            /* Remove from GPON MAC */
            rdpa_gpon_gem_enable_set(gponObj, gem, BDMF_FALSE);
            /* Remove Object */
            bdmf_destroy(gem);
        }

        return 0;
    }

    rc = get_gem_object_by_port_or_flow(&decfgGemPortInfop->gemPortIndex, decfgGemPortInfop->gemPortID, &gem);
    if (rc < 0)
    {
        BCM_LOG_NOTICE(BCM_LOG_ID_PLOAM,
          "decfgGemPort: not found, index=%d, portID=%d\n",
          decfgGemPortInfop->gemPortIndex, decfgGemPortInfop->gemPortID);
        return -EINVAL_PLOAM_GEM_PORT;
    }

    rdpa_gpon_gem_enable_set(gponObj, gem, BDMF_FALSE);
    bdmf_put(gem);            
    bdmf_destroy(gem);

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,
      "decfgGemPort: success, index=%d, portID=%d\n",
      decfgGemPortInfop->gemPortIndex, decfgGemPortInfop->gemPortID);

    return 0;
}

static int ploamIOCenableGemPort(BCM_Ploam_EnableGemPortInfo* enableGemPortInfop) 
{
    bdmf_error_t rc = BDMF_ERR_OK;
    bdmf_object_handle gem = NULL;
    int error = 0;

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM, "gemPortIndex=0x%x, gemPortId=0x%x, enable=%d",
        enableGemPortInfop->gemPortIndex, enableGemPortInfop->gemPortID,
        enableGemPortInfop->enable);

    if (enableGemPortInfop->gemPortIndex != enableGemPortInfop->gemPortID &&
        (INVALID_GEM_INDEX(enableGemPortInfop->gemPortIndex) ||
        INVALID_GEM_PORT(enableGemPortInfop->gemPortID)))
    {
        return -EINVAL_PLOAM_ARG;
    }

    bdmf_lock();
    rc = get_gem_object_by_port_or_flow(&enableGemPortInfop->gemPortIndex, enableGemPortInfop->gemPortID, &gem);
    rc = rc ? rc : rdpa_gpon_gem_enable_set(gponObj, gem, enableGemPortInfop->enable);
    bdmf_unlock();

    if (rc < 0)
    {
        BCM_LOG_ERROR (BCM_LOG_ID_PLOAM_PORT, "ploamIOCenableGemPort: GEM %d, E %d, error %d\n", enableGemPortInfop->gemPortID, enableGemPortInfop->enable, rc);
        error = -EINVAL_PLOAM_GEM_PORT;
    }

    if (gem)
        bdmf_put(gem);            

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM, "enableGem(), gemPortIndex=0x%x, gemPortId=0x%x, enable=%d, ret %d",
        enableGemPortInfop->gemPortIndex, enableGemPortInfop->gemPortID,
        enableGemPortInfop->enable, error);
    return error;
}

static int ploamIOCgetGemPortCfg(BCM_Ploam_GemPortInfo* gemPortInfop) 
{
    rdpa_gem_flow_ds_cfg_t ds_cfg;
    rdpa_gem_flow_us_cfg_t us_cfg;
    bdmf_object_handle gem = NULL;
    bdmf_error_t rc;
    bdmf_number allocID;
    int error = 0;

    if (INVALID_GEM_INDEX(gemPortInfop->gemPortIndex) ||
        INVALID_GEM_PORT(gemPortInfop->gemPortID))
    {
        BCM_LOG_ERROR (BCM_LOG_ID_PLOAM, "Parameter check failed for gemPortIdx %d gemPortId %d",
            gemPortInfop->gemPortIndex, gemPortInfop->gemPortID);
        return -EINVAL_PLOAM_ARG ;
    }

    gemPortInfop->allocID = BCM_PLOAM_ALLOC_ID_UNASSIGNED;

    rc = get_gem_object_by_port_or_flow(&gemPortInfop->gemPortIndex, gemPortInfop->gemPortID, &gem);
    rc = rc ? rc : rdpa_gem_ds_cfg_get(gem, &ds_cfg);
    if (rc < 0)
        goto out;

    /* Upstream Configuration */
    if (ds_cfg.destination != rdpa_flow_dest_iptv)
    {
        /* It's possible that gem object will have no upstream configuration */
        if (rdpa_gem_us_cfg_get(gem, &us_cfg) == BDMF_ERR_OK)
        {
            if (rdpa_gpon_tcont_alloc_id_get(gponObj, us_cfg.tcont, &allocID) == BDMF_ERR_OK)
                gemPortInfop->allocID = (UINT16)allocID;
        }
        gemPortInfop->isMcast = BDMF_FALSE;
    }
    else
    {
        gemPortInfop->isMcast = BDMF_TRUE;
    }

    gemPortInfop->flags = 0;
    gemPortInfop->weight = 0;    /* TBD: When WRR implemented: gem->RC->weight */
    gemPortInfop->dsQueueIdx = 0xFF;
    gemPortInfop->mcastFilterMode = BCM_PLOAM_GTCDS_MCAST_FILTER_OFF;

out:
    if (rc < 0)
    {
        BCM_LOG_ERROR (BCM_LOG_ID_PLOAM_PORT, "ploamIOCgetGemPortCfg: GEM %d, error %d\n", gemPortInfop->gemPortID, rc);
        error = -EINVAL_PLOAM_GEM_PORT;
    }

    if (gem)
        bdmf_put(gem);            

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,
      "getGemPortCfg: success, index=%d, portID=%d\n",
      gemPortInfop->gemPortIndex, gemPortInfop->gemPortID);

    return error;
}

static int ploamIOCgetAllocIds(BCM_Ploam_AllocIDs* allocIds) {

    bdmf_number alloc_id;
    bdmf_object_handle tcont = NULL;
    UINT16 numAllocIDs = 0;
    bdmf_error_t rc = BDMF_ERR_OK;

    while ((tcont = bdmf_get_next(rdpa_tcont_drv(), tcont, NULL)))
    {
        rc = rdpa_gpon_tcont_alloc_id_get(gponObj, tcont, &alloc_id);
        if (rc == BDMF_ERR_OK)
        {
            allocIds->allocIDs[numAllocIDs] = alloc_id;
            numAllocIDs++;
        } 
    }
    allocIds->numAllocIDs = numAllocIDs ;

    return 0;
}

static int ploamIOCgetOmciPortInfo(BCM_Ploam_OmciPortInfo* omciPortInfop) {

    BCM_OmciPortInfo omciPortInfo;

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");

    bcm_ploamPortCtrlGetOmciPortInfo(&omciPortInfo);

    omciPortInfop->encrypted = omciPortInfo.encrypted;
    omciPortInfop->omciGemPortActivated = omciPortInfo.enabled;
    omciPortInfop->omciGemPortID = omciPortInfo.portId;

    return 0;
}





static int ploamIOCgetOnuId(BCM_Ploam_GetOnuIdInfo* getOnuIdInfop) 
{    
    bdmf_number onu_id;
    bdmf_error_t rc;

    rc = rdpa_gpon_onu_id_get(gponObj, &onu_id);
    if (rc < 0)
        return -ENOENT;

    getOnuIdInfop->onuId = (UINT16)onu_id;

    return 0;
}

static int ploamIOCsetGemBlockLength(BCM_Ploam_GemBlkLenInfo* gemBlkLenInfop)
{
    bdmf_error_t rc;

    rc = rdpa_gpon_gem_block_size_set(gponObj, gemBlkLenInfop->gemBlkLen);
    if (rc != BDMF_ERR_OK)
        return -1;

    return 0;
}

static int ploamIOCgetGemBlockLength(BCM_Ploam_GemBlkLenInfo* gemBlkLenInfop) 
{
    bdmf_error_t rc = BDMF_ERR_OK;
    BCM_ASSERT(gemBlkLenInfop);
    rc = rdpa_gpon_gem_block_size_get(gponObj, (bdmf_number*)&(gemBlkLenInfop->gemBlkLen));
    if ( rc != BDMF_ERR_OK ){   
        return -1;
    }   
    return 0;
}

static int ploamIOCsetTodInfo(BCM_Ploam_TimeOfDayInfo* todInfop) 
{

#ifdef CONFIG_BCM_GPON_TODD
    uint32_t sframe_ls;
    uint32_t sframe_ms;
    gpon_todd_tstamp_t tstamp_n;

    /* Read Super Frame counter [SFC]*/
    gpon_tod_get_sfc(&sframe_ls, &sframe_ms);

    sframe_ls = todInfop->superframe;
    tstamp_n.sec_ms = todInfop->tStampN.secondsMSB;
    tstamp_n.sec_ls = todInfop->tStampN.secondsLSB;
    tstamp_n.nsec = todInfop->tStampN.nanoSeconds;

    gpon_todd_set_tod_info(sframe_ls, sframe_ms, &tstamp_n);
#endif 

    return 0;
}

static int ploamIOCgetTodInfo(BCM_Ploam_TimeOfDayInfo* todInfop) 
{
#ifdef CONFIG_BCM_GPON_TODD
    uint32_t sframe_ls;
    uint32_t sframe_ms;
    gpon_todd_tstamp_t tstamp_n;

    gpon_todd_get_tod_info(&sframe_ls, &sframe_ms, &tstamp_n);

    todInfop->superframe = sframe_ls;

    todInfop->tStampN.secondsMSB = tstamp_n.sec_ms;
    todInfop->tStampN.secondsLSB = tstamp_n.sec_ls;
    todInfop->tStampN.nanoSeconds = tstamp_n.nsec;
#endif 
    return 0;
}



static int ploamIOCgetFecMode(BCM_Ploam_GetFecModeInfo* getFecModeInfop) 
{
    bdmf_boolean   fec_state;


    getFecModeInfop->fecMode = 0;

    rdpa_gpon_ds_fec_state_get(gponObj, &fec_state);

    if (fec_state) 
    {
        getFecModeInfop->fecMode |=  BCM_PLOAM_FEC_MODE_FLAG_DS_FEC_ON;  
    }

    return 0;
}

static int ploamIOCgetEncryptionKey(BCM_Ploam_GetEncryptionKeyInfo* getEncryptionKeyInfop) 
{
    int rc;
    
    rc = rdpa_gpon_encryption_key_get(gponObj, &getEncryptionKeyInfop->key, 
        BCM_PLOAM_ENCRYPTION_KEY_SIZE_BYTES*sizeof(UINT8));

    if (rc < 0)
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM, "failed to get encription key: rc=%d", rc);

    return rc;
    }


static int ploamIOCsetMcastEncryptionKeys(BCM_Ploam_McastEncryptionKeysInfo* mcastEncryptionKeysInfop) 
{
    int rc;
    rdpa_gpon_mcast_enc_key_param_t mcastKeys;
    int key_idx = mcastEncryptionKeysInfop->key_idx;

    if (key_idx != 1 && key_idx != 2)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM, "unsupported multicast encryption key index: key_idx=%d", key_idx);
        return -1;
    }

    rc = rdpa_gpon_mcast_enc_key_parameters_get(gponObj, &mcastKeys);
    if (rc)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM, "failed to get encryption keys: rc=%d", rc);
        return rc;
    }

    memcpy(key_idx == 2 ? mcastKeys.key_2 : mcastKeys.key_1, mcastEncryptionKeysInfop->key, BCM_PLOAM_ENCRYPTION_KEY_SIZE_BYTES);
    rc = rdpa_gpon_mcast_enc_key_parameters_set(gponObj, &mcastKeys); 
    if (rc < 0)
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM, "failed to set encryption keys: rc=%d", rc);

    return rc;
    }






static int ploamIOCsetSerialPasswd(BCM_Ploam_SerialPasswdInfo* serialPasswdInfop) {
    
    /* keep SN and password */
    bcm_ploamSetNewSerialPasswd(serialPasswdInfop) ;

    /* Configure new serial number and password */
    bcm_ploamSetSerialPasswd(serialPasswdInfop) ;
  
    return 0;
}



static int ploamIOCgetSerialPasswd(BCM_Ploam_SerialPasswdInfo* serialPasswdInfop) {

    bcm_ploamGetSerialPasswd(serialPasswdInfop);

    return 0;
}


static int ploamIOCgenPrbs(BCM_Ploam_GenPrbsInfo* genPrbsInfop) {

    bdmf_boolean dummy;
    char wan_type_buf[PSP_BUFLEN_16] = {};
    int  count;
    serdes_wan_type_t wan_type = SERDES_WAN_TYPE_GPON;

    count = kerSysScratchPadGet((char*)RDPA_WAN_TYPE_PSP_KEY, (char*)wan_type_buf, (int)sizeof(wan_type_buf));
    if (count == 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_GPON, "ScratchPad does not contain RdpaWanType");
        return -1;
    }
    
    if (!strcasecmp(wan_type_buf, "GPON"))
    {
        wan_type = SERDES_WAN_TYPE_GPON;
    }
    else if (!strcasecmp(wan_type_buf, "XGPON1"))
    {
        wan_type = SERDES_WAN_TYPE_XGPON_10G_2_5G;
    } 
    else if (!strcasecmp(wan_type_buf, "XGS"))
    {
        wan_type = SERDES_WAN_TYPE_NGPON_10G_10G;
    }


    if (genPrbsInfop->mode < 0 || genPrbsInfop->mode >= BCM_GPON_SERDES_WRAPPER_PSEUDO_RANDOM ||
        genPrbsInfop->mode == BCM_GPON_SERDES_PRBS15)
    {
        BCM_LOG_ERROR ( BCM_LOG_ID_PLOAM,
                "Parameter check failed for genPrbsInfop->mode - %d not supported",
                genPrbsInfop->mode);
        return ( -EINVAL_PLOAM_ARG );
    }

    /* genPrbsInfop->mode same value is hardcoded in wan_drv.. */
    wan_prbs_gen(genPrbsInfop->enable, 1, genPrbsInfop->mode, wan_type, &dummy);
    prbs_enabled = genPrbsInfop->enable;
    prbs_mode = genPrbsInfop->mode;

    return 0;
}

/* This will return prbs enabled state by ploamIOCgenPrbs, not other means */
static int ploamIOCgetPrbsState(BCM_Ploam_GenPrbsInfo* genPrbsInfop) {

    genPrbsInfop->enable = prbs_enabled;
    genPrbsInfop->mode = prbs_mode;

    return BDMF_ERR_OK;
}



static int ploamIOCgetDriverVersion(BCM_Gpon_DriverVersionInfo* driverVersionInfop) {
  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");

  driverVersionInfop->apiMajor = BCM_PLOAM_VERSION_API_MAJOR;
  driverVersionInfop->apiMinor = BCM_PLOAM_VERSION_API_MINOR;
  driverVersionInfop->driverMajor = BCM_PLOAM_VERSION_DRIVER_MAJOR;
  driverVersionInfop->driverMinor = BCM_PLOAM_VERSION_DRIVER_MINOR;
  driverVersionInfop->driverFix = BCM_PLOAM_VERSION_DRIVER_FIX;

  return 0;
}

static int ploamIOCsetTO1TO2(BCM_Ploam_TO1TO2Info* to1to2Infop) {
    rdpa_gpon_link_cfg_t link_configuration;
    bdmf_error_t rc;

    if ((to1to2Infop->to1 > BCM_PLOAM_MAX_TO1_MS) ||
        (to1to2Infop->to2 > BCM_PLOAM_MAX_TO2_MS))
        return -EINVAL_PLOAM_ARG;

    bdmf_lock();
    rc = rdpa_gpon_link_cfg_get(gponObj, &link_configuration);
    if (rc < 0)
        goto exit;

    link_configuration.to1_timeout = to1to2Infop->to1;
    link_configuration.to2_timeout = to1to2Infop->to2;

    rc = rdpa_gpon_link_cfg_set(gponObj, &link_configuration );

exit:
    bdmf_unlock();
    return rc;
}

static int ploamIOCgetTO1TO2(BCM_Ploam_TO1TO2Info* to1to2Infop) {
    rdpa_gpon_link_cfg_t link_configuration;
    bdmf_error_t rc;

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");
    rc = rdpa_gpon_link_cfg_get(gponObj, &link_configuration);
    if (rc < 0)
        return rc;

    to1to2Infop->to1 = link_configuration.to1_timeout;
    to1to2Infop->to2 = link_configuration.to2_timeout; 

    return 0;
}


int ploamIOCgetSRIndication(BCM_Ploam_SRIndInfo *srIndication)   
{
    int rc=0;
    rdpa_dba_interval_t dba_interval;    
    rc = rdpa_gpon_dba_interval_get(gponObj, &dba_interval);
    if (!rc)
        srIndication->srIndication = dba_interval ? true : false;
    else
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM, "failed to get dba interval");
    return rc;
}

static PloamIOCentry ploamIOCfunTable[] = {
    {BCM_PLOAM_IOC_GET_EVENT_STATUS, (PloamIOCfun*)ploamIOCgetEventStatus, sizeof(BCM_Ploam_EventStatusInfo), 1},
    {BCM_PLOAM_IOC_MASK_EVENT, (PloamIOCfun*)ploamIOCmaskEvent, sizeof(BCM_Ploam_EventMaskInfo), 1},
    {BCM_PLOAM_IOC_GET_ALARM_STATUS, (PloamIOCfun*)ploamIOCgetAlarmStatus, sizeof(BCM_Ploam_AlarmStatusInfo), 1},
    {BCM_PLOAM_IOC_MASK_ALARM, (PloamIOCfun*)ploamIOCmaskAlarm, sizeof(BCM_Ploam_MaskAlarmInfo), 1},
    {BCM_PLOAM_IOC_SET_SF_SD_THRESHOLD, (PloamIOCfun*)ploamIOCsetSfSdThreshold, sizeof(BCM_Ploam_SFSDthresholdInfo), 1},
    {BCM_PLOAM_IOC_GET_SF_SD_THRESHOLD, (PloamIOCfun*)ploamIOCgetSfSdThreshold, sizeof(BCM_Ploam_SFSDthresholdInfo), 1},
    {BCM_PLOAM_IOC_START, (PloamIOCfun*)ploamIOCstart, sizeof(BCM_Ploam_StartInfo), 1},
    {BCM_PLOAM_IOC_STOP, (PloamIOCfun*)ploamIOCstop, sizeof(BCM_Ploam_StopInfo), 1},
    {BCM_PLOAM_IOC_GET_STATE, (PloamIOCfun*)ploamIOCgetState, sizeof(BCM_Ploam_StateInfo), 1},
    {BCM_PLOAM_IOC_GET_MESSAGE_COUNTERS, (PloamIOCfun*)ploamIOCgetMsgCounters, sizeof(BCM_Ploam_MessageCounters), 1},
    {BCM_PLOAM_IOC_GET_GTC_COUNTERS, (PloamIOCfun*)ploamIOCgetGtcCounters, sizeof(BCM_Ploam_GtcCounters), 1},
    {BCM_PLOAM_IOC_GET_FEC_COUNTERS, (PloamIOCfun*)ploamIOCgetFecCounters, sizeof(BCM_Ploam_fecCounters), 1},
    {BCM_PLOAM_IOC_GET_GEM_PORT_COUNTERS, (PloamIOCfun*)ploamIOCgetGemPortCounters, sizeof(BCM_Ploam_GemPortCounters), 1},
    {BCM_PLOAM_IOC_CFG_GEM_PORT, (PloamIOCfun*)ploamIOCcfgGemPort, sizeof(BCM_Ploam_CfgGemPortInfo), 1},
    {BCM_PLOAM_IOC_CFG_DS_GEM_PORT_ENCRYPTION, (PloamIOCfun*)ploamIOCsetGemPortEncryption, sizeof(BCM_Ploam_GemPortEncryption), 1},
    {BCM_PLOAM_IOC_GET_TCONT_CFG, (PloamIOCfun*)ploamIOCgetTcontCfg, sizeof(BCM_Ploam_TcontInfo), 1},

    {BCM_PLOAM_IOC_CFG_TCONT_ALLOCID, (PloamIOCfun*)ploamIOCcfgTcontAllocId, sizeof(BCM_Ploam_TcontAllocIdInfo), 1},
    {BCM_PLOAM_IOC_DECFG_TCONT_ALLOCID, (PloamIOCfun*)ploamIOCdecfgTcontAllocId, sizeof(BCM_Ploam_TcontAllocIdInfo), 1},

    {BCM_PLOAM_IOC_DECFG_GEM_PORT, (PloamIOCfun*)ploamIOCdecfgGemPort, sizeof(BCM_Ploam_DecfgGemPortInfo), 1},
    {BCM_PLOAM_IOC_ENABLE_GEM_PORT, (PloamIOCfun*)ploamIOCenableGemPort, sizeof(BCM_Ploam_EnableGemPortInfo), 1},
    {BCM_PLOAM_IOC_GET_GEM_PORT_CFG, (PloamIOCfun*)ploamIOCgetGemPortCfg, sizeof(BCM_Ploam_GemPortInfo), 1},
    {BCM_PLOAM_IOC_GET_ALLOC_IDS, (PloamIOCfun*)ploamIOCgetAllocIds, sizeof(BCM_Ploam_AllocIDs), 1},
    {BCM_PLOAM_IOC_GET_OMCI_PORT_INFO, (PloamIOCfun*)ploamIOCgetOmciPortInfo, sizeof(BCM_Ploam_OmciPortInfo), 1},
    {BCM_PLOAM_IOC_GET_ONU_ID, (PloamIOCfun*)ploamIOCgetOnuId, sizeof(BCM_Ploam_GetOnuIdInfo), 1},
    {BCM_PLOAM_IOC_GET_FEC_MODE, (PloamIOCfun*)ploamIOCgetFecMode, sizeof(BCM_Ploam_GetFecModeInfo), 1},
    {BCM_PLOAM_IOC_GET_ENCRYPTION_KEY, (PloamIOCfun*)ploamIOCgetEncryptionKey, sizeof(BCM_Ploam_GetEncryptionKeyInfo), 1},

    {BCM_PLOAM_IOC_SET_SERIAL_PASSWD, (PloamIOCfun*)ploamIOCsetSerialPasswd, sizeof(BCM_Ploam_SerialPasswdInfo), 1},
    {BCM_PLOAM_IOC_GET_SERIAL_PASSWD, (PloamIOCfun*)ploamIOCgetSerialPasswd, sizeof(BCM_Ploam_SerialPasswdInfo), 1},
    {BCM_PLOAM_IOC_GEN_PRBS, (PloamIOCfun*)ploamIOCgenPrbs, sizeof(BCM_Ploam_GenPrbsInfo), 1},
    {BCM_PLOAM_IOC_GET_PRBS_STATE, (PloamIOCfun*)ploamIOCgetPrbsState, sizeof(BCM_Ploam_GenPrbsInfo), 1},
    {BCM_PLOAM_IOC_GET_DRIVER_VERSION, (PloamIOCfun*)ploamIOCgetDriverVersion, sizeof(BCM_Gpon_DriverVersionInfo), 1},
    {BCM_PLOAM_IOC_SET_TO1_TO2, (PloamIOCfun*)ploamIOCsetTO1TO2, sizeof(BCM_Ploam_TO1TO2Info), 1},
    {BCM_PLOAM_IOC_GET_TO1_TO2, (PloamIOCfun*)ploamIOCgetTO1TO2, sizeof(BCM_Ploam_TO1TO2Info), 1},

    {BCM_PLOAM_IOC_SET_GEM_BLOCK_LENGTH, (PloamIOCfun*)ploamIOCsetGemBlockLength, sizeof(BCM_Ploam_GemBlkLenInfo), 1},
    {BCM_PLOAM_IOC_GET_GEM_BLOCK_LENGTH, (PloamIOCfun*)ploamIOCgetGemBlockLength, sizeof(BCM_Ploam_GemBlkLenInfo), 1},
    {BCM_PLOAM_IOC_SET_TIME_OF_DAY, (PloamIOCfun*)ploamIOCsetTodInfo, sizeof(BCM_Ploam_TimeOfDayInfo), 1},
    {BCM_PLOAM_IOC_GET_TIME_OF_DAY, (PloamIOCfun*)ploamIOCgetTodInfo, sizeof(BCM_Ploam_TimeOfDayInfo), 1},
   {BCM_PLOAM_IOC_GET_SR_IND, (PloamIOCfun*)ploamIOCgetSRIndication, sizeof(BCM_Ploam_SRIndInfo),1},
   {BCM_PLOAM_IOC_SET_MCAST_ENCRYPTION_KEY, (PloamIOCfun*)ploamIOCsetMcastEncryptionKeys, sizeof(BCM_Ploam_McastEncryptionKeysInfo), 1}
};

/**
 * ploam fops
 **/





 
static long ploam_ioctl(struct file *fp, unsigned int cmd, unsigned long arg) {
    int i;
    int ret=-EINVAL;
    int iocFunFound=0;
    static uint32 buf[512];

    BCM_ASSERT(ploamUsrStatep->initialized);
 
    /* ploamSem is protecting against concurrent ioctl calls to the GPON driver */
    if (down_interruptible(&ploamUsrStatep->ploamSem))
        return -ERESTARTSYS;
  

    for (i=0; i<sizeof(ploamIOCfunTable)/sizeof(ploamIOCfunTable[0]); i++) {
        if (cmd == ploamIOCfunTable[i].cmdId) {

            BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"ploamIOCfunTable[i = %d ].cmdId = %d \n",
	       	i,  ploamIOCfunTable[i].cmdId );


            iocFunFound=true;

            if (ploamIOCfunTable[i].checkArg && (arg==0)) {
                BCM_LOG_ERROR(BCM_LOG_ID_PLOAM,"invalid argument (1), 0x%x", (unsigned)arg);
                ret = -EINVAL;
                break;
            }

            if (arg) {
                BCM_ASSERT(ploamIOCfunTable[i].argSize < sizeof(buf));
                ret = copy_from_user(buf, (const void*)arg, ploamIOCfunTable[i].argSize);
                if (ret) {
                    BCM_LOG_ERROR(BCM_LOG_ID_PLOAM,"invalid argument (2), 0x%x", (unsigned)arg);
                    ret = -EINVAL;
                    break;
                }
            }

            /*RL FIXME: delegate isr and timers to a tasklet running a task queue,
             *so we don't need to globally disable interrupts*/

            // Save current PloamRx interrupt state and disable the GPON interrupt.
            /* Anil - Do we really need to do all the work with GPON_LOCK ??
             * Can we not reduce the scope and just protect the critical section ??
             * This seems bad coding !! */

            /*  BCM_GPON_LOCK(flags); */
            /* should call BL interrupt */

            ret = ploamIOCfunTable[i].fun((void*)buf);

            // Restore PloamRx interrupt to its previous enable/disable state.
            /*  BCM_GPON_UNLOCK(flags); */


            if (arg) {
                copy_to_user((void*)arg, buf, ploamIOCfunTable[i].argSize);
            }

            break;
        }
    }

    if (!iocFunFound) {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM,"invalid command, %d", cmd);
    }

    up(&ploamUsrStatep->ploamSem);

    return ret;
}

static unsigned int ploam_poll(struct file *filp, poll_table *wait) {

    unsigned int mask = 0;

  poll_wait(filp, &ploamUsrStatep->eventReadWaitQueue,  wait);

  if (bcm_ploamEventAvl(ploamUsrStatep->eventObjp)) {
    mask |= (POLLIN | POLLRDNORM);  /* readable */
  }

  BCM_LOG_INFO(BCM_LOG_ID_PLOAM, "mask=0x%x\n", mask);

    return mask;
}

static int ploam_open(struct inode *ip, struct file *fp) {
    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");
    return 0;
}

static int ploam_close(struct inode *ip, struct file *fp) {
    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");
    return 0;
}

static int user_ploam_open(struct inode *ip, struct file *fp) {
    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");

    return 0;
}

static int user_ploam_close(struct inode *ip, struct file *fp) {
    BCM_LOG_INFO(BCM_LOG_ID_PLOAM,"");
    return 0;
}

static struct file_operations ploam_fops = {
    .owner = THIS_MODULE,
    .llseek = no_llseek,
    .unlocked_ioctl = ploam_ioctl,
#if defined(CONFIG_COMPAT)
    .compat_ioctl = ploam_ioctl,
#endif
    .poll = ploam_poll,
    .open = ploam_open,
    .release = ploam_close,
};

static struct file_operations user_ploam_fops = {
    .owner =  THIS_MODULE,
    .llseek = no_llseek,
    .open = user_ploam_open,
    .release =  user_ploam_close,
};

int __devinit bcm_ploamUsrCreate(void) {

    int result;

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");
    BCM_ASSERT(ploamUsrStatep->created==0);
  
    /* Register the driver and link it to our fops */ 
    result = register_chrdev(userPloamMajor, BCM_PLOAM_USR_DEVICE_NAME, &user_ploam_fops);

    if (result < 0) {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM,"unable to register %s device (major=%d)", 
            BCM_PLOAM_USR_DEVICE_NAME, userPloamMajor);
    }

    if (userPloamMajor==0)
        userPloamMajor = result;

    result = register_chrdev(ploamMajor, BCM_PLOAM_DEVICE_NAME, &ploam_fops);

    if (result < 0) {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM,"unable to register %s device (major=%d)", 
            BCM_PLOAM_DEVICE_NAME, ploamMajor);

        unregister_chrdev(userPloamMajor, BCM_PLOAM_USR_DEVICE_NAME);
        userPloamMajor=USER_PLOAM_DEVICE_MAJOR;
    }

    if (ploamMajor==0)
        ploamMajor = result;

    /* Init semaphores */
    init_MUTEX(&ploamUsrStatep->ploamSem);
    init_MUTEX(&ploamUsrStatep->usrPloamSem);

    /* for gpond/omci implementation: init read from fifo events */
    init_waitqueue_head(&(ploamUsrStatep->eventReadWaitQueue));
    init_waitqueue_head(&(ploamUsrStatep->ploamReadWaitQueue));


    ploamUsrStatep->created = 1;

    return ( result );
}

static void ploamEventCallback(void *arg) {
    BCM_LOG_INFO(BCM_LOG_ID_PLOAM,"");
    wake_up_interruptible(&ploamUsrStatep->eventReadWaitQueue);
}


void bcm_ploamUsrInit(BCM_PloamEventState* eventObjp) {
  
    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");

    BCM_ASSERT(ploamUsrStatep->created);
    BCM_ASSERT(eventObjp);
    
    ploamUsrStatep->eventObjp = eventObjp;

    bcm_ploamEventRegister(ploamUsrStatep->eventObjp, ploamEventCallback, 0 /*ctxt*/);
 
    ploamUsrStatep->initialized = 1;
}


void bcm_ploamUsrDelete(void) {

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM, "");

    if (ploamUsrStatep->created) {
        unregister_chrdev(userPloamMajor, BCM_PLOAM_USR_DEVICE_NAME);
        userPloamMajor=USER_PLOAM_DEVICE_MAJOR;

        unregister_chrdev(ploamMajor, BCM_PLOAM_DEVICE_NAME);
        ploamMajor=PLOAM_DEVICE_MAJOR;

        ploamUsrStatep->created=0;
    }
}

static int get_gem_object_by_port_or_flow(UINT16 *gemPortIndex, UINT16 gemPortID, bdmf_object_handle *gem)
{
    int rc;

    /* Find GEM by Port */
    if (*gemPortIndex == BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED) 
    {
        rc = get_gem_flow_id_by_gem_port((bdmf_number)gemPortID, gemPortIndex);
        if (rc < 0) 
        {
            BCM_LOG_ERROR (BCM_LOG_ID_PLOAM_PORT, "Failed to get GEM flow ID by GEM port %d: error %d\n", gemPortID, rc);
            return -ENOENT;
        }
    }

    rc = rdpa_gem_get((bdmf_number)(*gemPortIndex), gem);
    if (rc < 0) 
    {
        BCM_LOG_ERROR (BCM_LOG_ID_PLOAM_PORT, "Failed to get GEM object: error %d\n", rc);
        return -ENOENT;
    }

    return 0;
}
