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
*/
#define UNASSIGNED_GEMPORT_ID    0xffff
#define UNASSIGNED_QUEUE_ID      0xff
#define MAX_ENG_RING_OMCI_CODE   0x3

#include "bcm_ploamPortCtrl.h"
#include "bcm_gponBasicDefs.h"
#include "bcm_omciUsr.h"
#include "bcm_ploamAlarm.h"
#include "bcm_ploam.h"
#include <linux/bcm_log.h>
#include <board.h>
#include <rdpa_ag_gpon.h>
#include <rdpa_mw_cpu_queue_ids.h>
#include "bcm_ploamUsr.h"

extern bdmf_object_handle gponObj;


typedef struct BCM_OmciPortPrivate {
/*private*/
  BCM_OmciPortInfo omciPortInfo;
  bool created;
} BCM_OmciPortPrivate;

typedef struct BCM_GemPortEncryptionInfo {
  uint16_t gemPortId;
  bool encrypted;
} BCM_GemPortEncryptionInfo;

typedef struct BCM_UsQueueInfoPrivate {
/*private*/
  BCM_UsQueueCfgInfo usQueueInfo;
  bool mapped;
} BCM_UsQueueInfoPrivate;

typedef struct BCM_PloamPortCtrlState {
  BCM_GemPortEncryptionInfo encryptionInfo[BCM_PLOAM_PORTCTRL_NUM_GEMPORTS]; /*Not indexed. Search by gemPortID.*/
  BCM_UsQueueInfoPrivate usQueueInfo[BCM_PLOAM_MAX_GTCUS_QUEUES]; /*Indexed by usQueueIdx*/
  BCM_OmciPortPrivate omciPort;
  uint16_t assignedOnuId;
  BCM_PloamEventState* ploamEventObjp;
  bool initialized;
} BCM_PloamPortCtrlState;

/**
 * Local Variables:
 **/
static BCM_PloamPortCtrlState ploamPortCtrlState;
static BCM_PloamPortCtrlState* statep = &ploamPortCtrlState;
static void bcm_ploamlinkStateTransition ( rdpa_pon_link_operational_state old_state,
                                           rdpa_pon_link_operational_state new_state );
static BOARD_LED_STATE curLedState = kLedStateOff;
static void setGponLedState(BOARD_LED_STATE newLedState);

typedef struct
{
    rdpa_pon_link_operational_state state;
    BOARD_LED_STATE ledState;
}OnuLedState;

const OnuLedState ponStateToLedState[] =
{
    {rdpa_pon_init_o1,           kLedStateOff},
    {rdpa_pon_standby_o2,        kLedStateOff},
    {rdpa_pon_serial_num_o3,     kLedStateSlowBlinkContinues},
    {rdpa_pon_ranging_o4,        kLedStateSlowBlinkContinues},
    {rdpa_pon_operational_o5,    kLedStateOn},
    {rdpa_pon_popup_o6,          kLedStateFastBlinkContinues},
    {rdpa_pon_emergency_stop_o7, kLedStateOff},
    {rdpa_pon_ds_tuning_o8,      kLedStateSlowBlinkContinues},
    {rdpa_pon_us_tuning_o9,      kLedStateSlowBlinkContinues},
    {rdpa_pon_serial_num_o2_3,   kLedStateSlowBlinkContinues},
};

void bcm_ploamPortCtrlInit(bool keepConfig, BCM_PloamEventState* ploamEventObjp) {
  int i;
  BCM_ASSERT(statep);
  /*OMCI port info is always returned, even if created=0
   *=> initialize the entire object*/
  statep->omciPort.created = 0;
  statep->omciPort.omciPortInfo.enabled = 0;
  statep->omciPort.omciPortInfo.encrypted = 0;
  statep->omciPort.omciPortInfo.portId = BCM_PLOAM_GEM_PORT_ID_UNASSIGNED;
  statep->assignedOnuId = BCM_PLOAM_ONU_ID_UNASSIGNED;
  statep->initialized=1;
  for (i=0; i<BCM_PLOAM_PORTCTRL_NUM_GEMPORTS; ++i) {
      /*always disable encryption. This info should come from ploam*/
      statep->encryptionInfo[i].gemPortId = UNASSIGNED_GEMPORT_ID;
      statep->encryptionInfo[i].encrypted = 0;
  }

  /* Initialize US Queue Info */
  for (i=0; i<BCM_PLOAM_MAX_GTCUS_QUEUES; ++i) {
      statep->usQueueInfo[i].mapped = 0;
      statep->usQueueInfo[i].usQueueInfo.tcontIdx = UNASSIGNED_QUEUE_ID;
      statep->usQueueInfo[i].usQueueInfo.usQueueIdx = UNASSIGNED_QUEUE_ID;
      statep->usQueueInfo[i].usQueueInfo.priority = 0;
  }
  statep->ploamEventObjp = ploamEventObjp;
  BCM_LOG_INFO(BCM_LOG_ID_PLOAM_PORT,"ploamPortCtrl Initialized. keepConfig %d", keepConfig);
}

extern void enet_pon_drv_link_change(int link);

/**************************************************************************
**  Function Name: GponDrvIndicationCallbac
**  Purpose:
**    This function is used as a callback function for ploams indication
**  Parameters:
**    indication_type        - the types of indication.
**    indParam  - parameters of the indication.
**  Return:
**    None
**  Globals:
**    None
**  Special Function Usage Requirements:
**    the function is called in ISR.
**************************************************************************/
void GponDrvIndicationCallback(rdpa_pon_indication xi_indication_type,
                               rdpa_callback_indication indParam)
{
    /*switch begin*/
    switch (xi_indication_type)
    {
    /*CONFIGURE PORT ID Ploam message*/
    case rdpa_indication_cfg_port_id_msg:
    {
        int ret;
        BCM_OmciPortInfo omciPortInfo;

        uint32_t portId = indParam.configure_port_id_parameters.port_id;
        uint8_t status = indParam.configure_port_id_parameters.status;

        switch (status)
        {
            case rdpa_indication_on:
                ret = bcm_ploamPortCtrlGetOmciPortInfo(&omciPortInfo);
                if (ret == 0) {
                    /*OMCI port already exists. Only tear down and recreate if the portId has changed*/
                    if (omciPortInfo.portId != portId) {
                            ret = bcm_ploamPortCtrlRemoveOmciPort();
                            BCM_ASSERT(ret==0);
                            ret = bcm_ploamPortCtrlCreateOmciPort(portId);
                            BCM_ASSERT(ret==0);
                    }
                    else /* Reset encryption status on OMCI GEM */
                    {
                        int i;

                        bcm_ploamPortCtrlEncryptGemPortByIdx(RDPA_OMCI_FLOW_ID, 0);
                        BCM_ASSERT(ret==0);

                        /* Update DB */
                        for (i=0; i<BCM_PLOAM_PORTCTRL_NUM_GEMPORTS; ++i)
                        {
                            if (statep->encryptionInfo[i].gemPortId == portId)
                            {
                                statep->encryptionInfo[i].encrypted = 0;
                                break;
                            }
                        }
                    }
               }
                else {
                    /*OMCI port does not exist yet -> create it*/
                    ret = bcm_ploamPortCtrlCreateOmciPort(portId);
                    BCM_ASSERT(ret==0);
                }
                break;

            case rdpa_indication_off:

                ret = bcm_ploamPortCtrlGetOmciPortInfo(&omciPortInfo);
                if (ret != 0) 
                {
                    /* Silently return if OMCI port does no exist */
                    BCM_LOG_NOTICE(BCM_LOG_ID_PLOAM_FSM,
                        "ConfigPortID deactivate failed. No OMCI port configured.ret = %d", ret);
                    return;
                }
                if (omciPortInfo.portId != portId) 
                {
                    BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_FSM,
                        "ConfigPortID deactivate failed. OMCI port ID: %d, given portID: %d",
                        omciPortInfo.portId, portId);
                    return;
                }
                ret = bcm_ploamPortCtrlRemoveOmciPort();
                BCM_ASSERT(ret == 0);

                break;

            default:
                BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_FSM,"ConfigPortID. Unknow action: 0x%x", status);
                return;

            }//switch status
         }//case CE_BL_INDICATIONS_CFG_PORT_ID_MESSAGE
        break;

        case rdpa_indication_assign_onu_id_msg:
        {
            if (statep->assignedOnuId != BCM_PLOAM_ONU_ID_UNASSIGNED && statep->assignedOnuId != indParam.onu_id)
            {
                bcm_ploamPortCtrlRemoveOmciPort();
            }
            statep->assignedOnuId = indParam.onu_id;
            BCM_LOG_INFO(BCM_LOG_ID_PLOAM_FSM, "Assigned onuID: %d",statep->assignedOnuId);

        }
        break;
        case rdpa_indication_encrypted_port_id_msg:
        {
            int i, ret;
            uint16_t gemPortId = indParam.encrypted_port_id_parameters.port_id;
            bool encrypt = indParam.encrypted_port_id_parameters.encryption;

            /* Update DB */
            for (i=0; i<BCM_PLOAM_PORTCTRL_NUM_GEMPORTS; ++i)
            {
                if (statep->encryptionInfo[i].gemPortId == gemPortId)
                {
                    /* gem was already indicated*/
                    if (statep->encryptionInfo[i].encrypted == encrypt)
                    {
                        /*nothing changed*/
                        return;
                    }
                    else
                    {
                        statep->encryptionInfo[i].encrypted = encrypt;
                        goto new_changed;
                    }
                }
            }

            /* New GEM port */
            for (i=0; i<BCM_PLOAM_PORTCTRL_NUM_GEMPORTS; ++i)
            {
                if (statep->encryptionInfo[i].gemPortId == UNASSIGNED_GEMPORT_ID)
                {
                    statep->encryptionInfo[i].gemPortId = gemPortId;
                    statep->encryptionInfo[i].encrypted = encrypt;
                    goto new_changed;
                }
            }

            return;

 new_changed:
            ret = bcm_ploamPortCtrlEncryptGemPortById(gemPortId, encrypt);
            if(ret < 0 && ret != -ENOENT)     
                BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_PORT, "Failed to modify port encription mode: ret=%d", ret);
            return;
            break;

        }//case CE_BL_INDICATIONS_ASSIGN_ONU_ID_MESSAGE
           break;
    

        /*LOF (and LODS) Alarm*/
        case rdpa_indication_lof:
            bcm_ploamAlarmSet(BCM_PLOAM_ALARM_ID_LOF, indParam.sd_status==rdpa_indication_on);
            break;

        /*LCDG Alarm*/
        case rdpa_indication_lcdg:
            bcm_ploamAlarmSet(BCM_PLOAM_ALARM_ID_LCDG, indParam.sd_status==rdpa_indication_on);
            break;

        /*SD Alarm*/
        case rdpa_indication_sd:
            bcm_ploamAlarmSet(BCM_PLOAM_ALARM_ID_SD, indParam.sd_status==rdpa_indication_on);
            break;

        /*SF Alarm*/
        case rdpa_indication_sf:
            bcm_ploamAlarmSet(BCM_PLOAM_ALARM_ID_SF, indParam.sf_status==rdpa_indication_on);
            break;

        /*DACT Alarm*/
        case rdpa_indication_dact:
            bcm_ploamAlarmSet(BCM_PLOAM_ALARM_ID_DACT, indParam.sd_status==rdpa_indication_on);
            break;

        /*DACT Alarm*/
        case rdpa_indication_dis:
            bcm_ploamAlarmSet(BCM_PLOAM_ALARM_ID_DIS, indParam.sd_status==rdpa_indication_on);
            break;

        /*PEE Alarm*/
        case rdpa_indication_pee:
            bcm_ploamAlarmSet(BCM_PLOAM_ALARM_ID_PEE, indParam.sd_status==rdpa_indication_on);
            break;

        /*MEM Alarm*/
        case rdpa_indication_mem:
            bcm_ploamAlarmSet(BCM_PLOAM_ALARM_ID_MEM, indParam.sd_status==rdpa_indication_on);
            break;


        /* Transition to O1/O7 link state */
        case rdpa_indication_link_state_transition:
        {
            rdpa_pon_link_operational_state old_state = indParam.link_state_transition_parameters.old_state;
            rdpa_pon_link_operational_state new_state = indParam.link_state_transition_parameters.new_state;

            if ( ( new_state == rdpa_pon_emergency_stop_o7 ) ||
                 ( old_state == rdpa_pon_emergency_stop_o7 ) )
            {
                bcm_ploamlinkStateTransition ( old_state,
                                               new_state );
            }

            /* GPON LED control */
            setGponLedState(ponStateToLedState[new_state].ledState);
            
          bcm_ploamEventSet(statep->ploamEventObjp, BCM_PLOAM_EVENT_GEM_STATE_CHANGE);

          /* check if the state is o5 than return 1 to enetx driver */
          if (new_state == rdpa_pon_operational_o5 || old_state == rdpa_pon_operational_o5)
              enet_pon_drv_link_change(new_state == rdpa_pon_operational_o5);
        }
        break;

        default:
            break;
    }//switch xi_indication_type
}/* End of GponDrvIndicationCallback() */

static void bcm_ploamlinkStateTransition ( rdpa_pon_link_operational_state old_state,
                                           rdpa_pon_link_operational_state new_state )
{
    int error;
    char tokBuf [SP_LINK_STATE_BUFFER_SIZE];

    /* ONU receives Disable Request -> Operation State Stop */
    if ( new_state == rdpa_pon_emergency_stop_o7 )
    {
        memcpy ( &tokBuf, "stop   ", SP_LINK_STATE_BUFFER_SIZE );
    }

    /* ONU receives Enable Request -> Operation State Standby */
    else if ( old_state == rdpa_pon_emergency_stop_o7 )
    {
        memcpy ( &tokBuf, "standby", SP_LINK_STATE_BUFFER_SIZE );
    }

    error =  kerSysScratchPadSet("linkState", tokBuf, SP_LINK_STATE_BUFFER_SIZE );
    if ( error != 0 )
    {
        BCM_LOG_ERROR ( BCM_LOG_ID_PLOAM_PORT,
                        "Failed to keep initial state in data partition. error: %d.",
                        error);
    }
}

/* Set GPON Led state */
static void setGponLedState(BOARD_LED_STATE newLedState)
{
    if (curLedState != newLedState)
    {
        kerSysLedCtrl(kLedGpon, newLedState);
        curLedState = newLedState;
    }
}

/*Returns gemPort's encryption Cfg. Returns NULL if portId not found*/
static BCM_GemPortEncryptionInfo* findGemPortEncryptionInfo(uint16_t gemPortId) {
  int i;
  for (i=0; i<BCM_PLOAM_PORTCTRL_NUM_GEMPORTS; ++i) {
    if (statep->encryptionInfo[i].gemPortId == gemPortId)
      return &statep->encryptionInfo[i];
  }

  return NULL;
}


int bcm_ploamPortCtrlGetOmciPortInfo(OUT BCM_OmciPortInfo* omciPortInfo) {
  int rc=0;

  BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM_PORT,"");
  BCM_ASSERT(statep);
  BCM_ASSERT(statep->initialized);

  if (!statep->omciPort.created)
    rc = -ENOENT;

  *omciPortInfo = statep->omciPort.omciPortInfo;
  return rc;
}

int bcm_ploamPortCtrlRemoveOmciPort(void) {

    bdmf_error_t rc = BDMF_ERR_OK;
    bdmf_object_handle tcont = NULL;
    bdmf_object_handle gem=NULL;

    BCM_ASSERT(statep);
    BCM_ASSERT(statep->initialized);

    if (!statep->omciPort.created)
        return -ENOENT;

    /*OMCI Port info can still be read after it's been removed.
     *=>Set enabled status to 0.
     */
    statep->omciPort.created=0;
    statep->omciPort.omciPortInfo.enabled=0;

    /* Remove omci gem flow */
    rc = rdpa_gem_get(RDPA_OMCI_FLOW_ID, &gem);
    if (rc < 0)
        return rc;
    rdpa_gpon_gem_enable_set(gponObj, gem, BDMF_FALSE);
    bdmf_put(gem);
    bdmf_destroy(gem);

    /* Tcont must be configured prior to rate controller API */
    rc = rdpa_tcont_get(RDPA_OMCI_TCONT_ID, &tcont);
    if (rc < 0)
        return rc;

    rc = rdpa_gpon_tcont_alloc_id_delete(gponObj, tcont);
    bdmf_put(tcont);
    if (rc)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM,
            "Remove TCONT from GPON failed, error = %d", (int)rc);
        return rc;
    }
    bdmf_destroy(tcont);

    /*disable the cpu queue*/
    rdpa_cpu_int_disable(rdpa_cpu_host, RDPA_OMCI_CPU_RX_QUEUE_ID);
    rdpa_cpu_int_clear(rdpa_cpu_host, RDPA_OMCI_CPU_RX_QUEUE_ID);

    BCM_LOG_NOTICE(BCM_LOG_ID_PLOAM_PORT,"Removed OmciPort.");
    return 0;
}

int bcm_ploamPortCtrlCreateOmciPort(uint16_t gemPortId)
{
    BCM_GemPortEncryptionInfo* encryptionInfop;
    bdmf_error_t rc = BDMF_ERR_OK;
    bdmf_object_handle tcont;
    bdmf_object_handle gem;
    bdmf_object_handle sched;
    rdpa_gem_flow_ds_cfg_t ds_cfg = {};
    rdpa_gem_flow_us_cfg_t us_cfg = {};
    rdpa_tm_queue_cfg_t queue_cfg = {};
    rdpa_gpon_gem_ds_cfg_t gem_ds_cfg = {};
    BDMF_MATTR(tcont_attrs, rdpa_tcont_drv());
    BDMF_MATTR(gem_attrs, rdpa_gem_drv());
    BDMF_MATTR(sched_attrs, rdpa_egress_tm_drv());


    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM_PORT,"");
    BCM_ASSERT(statep);
    BCM_ASSERT(statep->initialized);
    if (statep->omciPort.created)
    {
      BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_PORT, "OMCI port already created, using gemPortId %d.", statep->omciPort.omciPortInfo.portId);
      return -EPERM;
    }

    encryptionInfop = findGemPortEncryptionInfo(gemPortId);

    statep->omciPort.created = true;
    statep->omciPort.omciPortInfo.enabled = true;
    statep->omciPort.omciPortInfo.encrypted = encryptionInfop ? encryptionInfop->encrypted : 0;
    statep->omciPort.omciPortInfo.portId = gemPortId;

    rc = rdpa_tcont_index_set(tcont_attrs, RDPA_OMCI_TCONT_ID);
    rc = rc ? rc : rdpa_tcont_management_set(tcont_attrs, 1);

    rc = rc ? rc : bdmf_new_and_set(rdpa_tcont_drv(), NULL, tcont_attrs, &tcont);

    if (rc < 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_PORT, "OMCI TCONT create object failed: rc = %d.", rc);
        return rc;
    }

    rc = rdpa_gpon_tcont_alloc_id_set(gponObj, tcont, statep->assignedOnuId);
    if (rc < 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_PORT,
            "OMCI TCONT creation - set Alloc ID failed: rc = %d.", rc);
        return rc;
    }

    rc = rdpa_gem_index_set(gem_attrs, RDPA_OMCI_FLOW_ID);
    rc = rc ? rc : rdpa_gem_gem_port_set(gem_attrs, gemPortId);
    rc = rc ? rc : rdpa_gem_flow_type_set(gem_attrs, rdpa_gem_flow_omci);

    ds_cfg.discard_prty = rdpa_discard_prty_high;
    ds_cfg.destination = rdpa_flow_dest_omci;
    rc = rc ? rc : rdpa_gem_ds_cfg_set(gem_attrs, &ds_cfg);

    us_cfg.tcont = tcont;
    rc = rc ? rc : rdpa_gem_us_cfg_set(gem_attrs, &us_cfg);

    rc = rc ? rc : bdmf_new_and_set(rdpa_gem_drv(), NULL, gem_attrs, &gem);

    if (rc < 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_PORT, "OMCI GEM creation failed: rc = %d.", rc);
        return rc;
    }

    /* Configure and enable GEM in GPON MAC */
    gem_ds_cfg.encryption = statep->omciPort.omciPortInfo.encrypted;
    gem_ds_cfg.port = gemPortId;
    gem_ds_cfg.discard_prty = rdpa_discard_prty_high;
    gem_ds_cfg.crc = BDMF_FALSE;
    gem_ds_cfg.enc_ring = 0;  /* Otherwise OMCI MIC calculation fails */
    gem_ds_cfg.encryption = BDMF_FALSE;
    gem_ds_cfg.flow_type = rdpa_gpon_flow_type_omci;

    rc = rdpa_gpon_gem_ds_cfg_set(gponObj, gem, &gem_ds_cfg);
    if (rc < 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_PORT, "Configure OMCI GEM in GPON MAC failed: rc = %d.", rc);
        return rc;
    }
    
    rdpa_gpon_gem_enable_set(gponObj, gem, BDMF_TRUE);

    rc = rdpa_egress_tm_dir_set(sched_attrs, rdpa_dir_us);
    rc = rc ? rc : rdpa_egress_tm_level_set(sched_attrs, rdpa_tm_level_queue);
    rc = rc ? rc : rdpa_egress_tm_mode_set(sched_attrs, rdpa_tm_sched_sp);
#ifdef XRDP
    rc = rc ? rc : rdpa_egress_tm_num_queues_set(sched_attrs, 1); /* Set egress_tm type complex */
#endif
    rc = rc ? rc : bdmf_new_and_set(rdpa_egress_tm_drv(), tcont, sched_attrs, &sched);
    if (rc < 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_PORT, "Configure OMCI Egress TM failed: rc = %d.", rc);
        return rc;
    }

    queue_cfg.drop_alg = rdpa_tm_drop_alg_dt;
    queue_cfg.drop_threshold = RDPA_OMCI_US_PRIQ_PACKET_THRESHOLD;

    /* Configure queue 0  in egress tm */
    rc = rdpa_egress_tm_queue_cfg_set(sched, 0, &queue_cfg);

    if (rc < 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_PORT, "Configure OMCI Queues failed: rc = %d.", rc);
        return rc;
    }

    rdpa_cpu_int_enable(rdpa_cpu_host, RDPA_OMCI_CPU_RX_QUEUE_ID);

    BCM_LOG_INFO(BCM_LOG_ID_PLOAM_PORT,"Created OmciPort, GemPortId=0x%4.4X", gemPortId);

    bcm_omciUsrWakeupWriters(0);

    return rc;
}

int bcm_cfgGemPort(BCM_Ploam_CfgGemPortInfo* cfgGemPortInfop)
{
    bdmf_object_handle gem = NULL;
    BDMF_MATTR(gem_attrs, rdpa_gem_drv());
    rdpa_gem_flow_ds_cfg_t ds_cfg = {};
    rdpa_gem_flow_us_cfg_t us_cfg = {};
    rdpa_flow_destination dest_flow = rdpa_flow_dest_eth;
    rdpa_classify_mode classify_type = rdpa_classify_mode_pkt;
    bdmf_number allocID = BDMF_INDEX_UNASSIGNED;
    BCM_GemPortEncryptionInfo *encryptionInfop;
    bool encrypted = false;
    int i = 0;
    bdmf_error_t rc = BDMF_ERR_OK;
    bdmf_number gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
    rdpa_gpon_gem_ds_cfg_t gem_ds_cfg = {};
    UINT16 gemFlowId = 0;
#if 1
    /*
     * Since the encryption ring configuration is provided in the MIB format:
     *     0 - unencrypted
     *     1 - unicast encryption (bidirectional)
     *     2 - multicast encryption at this GEM
     *     3 - unicast encryption (downstream only)
     *  while upstream encryption for RDPA is a binary variable (true/false)
     *  the mapping is provided by the following table
     */
    static bdmf_boolean enc_ring_to_us_enc[] = {
      BDMF_FALSE, /* unencrypted */
      BDMF_TRUE,  /* unicast encryption, we _always_ use keys of ONU #1 */
      BDMF_FALSE, /* multicast key set */
      BDMF_FALSE  /* unicast encryption, downstream only - but here the downstream is configured , we _always_ use keys of ONU #1 */
    } ;
#endif

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"gemPortID=0x%x MaxGems %d",
        cfgGemPortInfop->gemPortID,
        BCM_PLOAM_NUM_DATA_GEM_PORTS);

    if (INVALID_GEM_INDEX(cfgGemPortInfop->gemPortIndex) ||
        INVALID_GEM_PORT(cfgGemPortInfop->gemPortID) ||
        INVALID_ALLOC(cfgGemPortInfop->allocID))
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM,
          "bcm_cfgGemPort: invalid params, index=%d, portID=%d, allocID=%d\n",
          cfgGemPortInfop->gemPortIndex, cfgGemPortInfop->gemPortID,
          cfgGemPortInfop->allocID);
        return -EINVAL_PLOAM_ARG;
    }

    /* Check whether the GEM port has been configured. */
    if (get_gem_flow_id_by_gem_port(cfgGemPortInfop->gemPortID, &gemFlowId) == 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM,
          "dup: gem/index=%d, portID=%d, flowid %d\n",
          cfgGemPortInfop->gemPortIndex, cfgGemPortInfop->gemPortID, gemFlowId);
        return -EINVAL_PLOAM_DUPLICATE;
    }

    encryptionInfop = findGemPortEncryptionInfo(cfgGemPortInfop->gemPortID);
    if (encryptionInfop)
    {
        encrypted = encryptionInfop->encrypted;
    }
    else
    {
        for (i = 0; i < BCM_PLOAM_PORTCTRL_NUM_GEMPORTS; ++i)
        {
            if ( statep->encryptionInfo[i].gemPortId == UNASSIGNED_GEMPORT_ID )
            {
                statep->encryptionInfo[i].gemPortId = cfgGemPortInfop->gemPortID;
                break;
            }
        }
    }

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM, "GEM port=%d, encrypt=%d", 
      cfgGemPortInfop->gemPortID, encrypted);

    /* iptv channel */
    if ( cfgGemPortInfop->isMcast == BDMF_TRUE )
    {
        classify_type = rdpa_classify_mode_flow;
        dest_flow = rdpa_flow_dest_iptv;
    }

    rc = rdpa_gem_gem_port_set(gem_attrs, cfgGemPortInfop->gemPortID);
    if ( cfgGemPortInfop->gemPortIndex != BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED )
        rc = rc ? rc : rdpa_gem_index_set(gem_attrs, cfgGemPortInfop->gemPortIndex);

    rc = rc ? rc : rdpa_gem_flow_type_set(gem_attrs, rdpa_gem_flow_ethernet);

    ds_cfg.discard_prty = rdpa_discard_prty_low;
    ds_cfg.destination = dest_flow;
    rc = rc ? rc : rdpa_gem_ds_cfg_set(gem_attrs, &ds_cfg);

    us_cfg.tcont = NULL;
    if ( cfgGemPortInfop->allocID != BCM_PLOAM_ALLOC_ID_UNASSIGNED )
    {
        while ((us_cfg.tcont = bdmf_get_next(rdpa_tcont_drv(), us_cfg.tcont, NULL)))
        {
            rc = rc ? rc : rdpa_gpon_tcont_alloc_id_get(gponObj, us_cfg.tcont, &allocID);
            if ((rc == BDMF_ERR_OK) && ((UINT16)allocID == cfgGemPortInfop->allocID))
                break;
        }

        if (((UINT16)allocID != cfgGemPortInfop->allocID) || (us_cfg.tcont == NULL))
        {
            BCM_LOG_ERROR(BCM_LOG_ID_PLOAM,
                "bcm_cfgGemPort: can't find tcont's allocId. error = %d, "
                "index=%d, portID=%d, allocID=%d\n",
                rc, cfgGemPortInfop->gemPortIndex, cfgGemPortInfop->gemPortID,
                cfgGemPortInfop->allocID);
            rc = -ENOENT;
            goto exit;
        }

        if (cfgGemPortInfop->encRing <= MAX_ENG_RING_OMCI_CODE)
        {
           us_cfg.enc = enc_ring_to_us_enc[cfgGemPortInfop->encRing];
        }
        else
        {
           us_cfg.enc = 0;
        }

        rc = rc ? rc : rdpa_gem_us_cfg_set(gem_attrs, &us_cfg);
    }

    rc = rc ? rc : bdmf_new_and_set(rdpa_gem_drv(), NULL, gem_attrs, &gem);
    if (rc < 0)
    {
        goto exit;
    }

    rc = rc ? rc : rdpa_gem_index_get(gem, &gemPortIndex);
    if (rc < 0)
    {
        goto exit;
    }

    cfgGemPortInfop->gemPortIndex = (UINT16)gemPortIndex;

    /* GPON MAC configuration */
    gem_ds_cfg.encryption = encrypted;
    gem_ds_cfg.port = cfgGemPortInfop->gemPortID;
    if (cfgGemPortInfop->encRing <= MAX_ENG_RING_OMCI_CODE)
    {
        gem_ds_cfg.enc_ring =  cfgGemPortInfop->encRing;    /* GPON stack ignores this parameter */
    }
    else
    {
      gem_ds_cfg.enc_ring =  0;
    }
    gem_ds_cfg.discard_prty = rdpa_discard_prty_low;
    gem_ds_cfg.crc = BDMF_TRUE;
    gem_ds_cfg.flow_type = rdpa_gpon_flow_type_ethernet;

    /* Configure and enable GEM in GPON MAC */
    rc = rdpa_gpon_gem_ds_cfg_set(gponObj, gem, &gem_ds_cfg);
    if (rc < 0)
    {
        /* MAC configuration failed, remove RDPA GEM object */
        goto exit;
    }

exit:
    if (us_cfg.tcont)
        bdmf_put(us_cfg.tcont);

    if (rc < 0)
    {
        if (gem != NULL)
        {
            bdmf_destroy(gem);
        }
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM,
          "bcm_cfgGemPort: failed rc=%d, index=%d, portID=%d, allocID=%d\n",
          rc, cfgGemPortInfop->gemPortIndex, cfgGemPortInfop->gemPortID,
          cfgGemPortInfop->allocID);
    }
    else
    {
        BCM_LOG_NOTICE(BCM_LOG_ID_PLOAM,
          "bcm_cfgGemPort: success, index=%d, portID=%d, mcast=%d, allocID=%d, encRing=%d, encryption=%d\n",
          cfgGemPortInfop->gemPortIndex, cfgGemPortInfop->gemPortID,
          cfgGemPortInfop->isMcast, cfgGemPortInfop->allocID,
          cfgGemPortInfop->encRing, gem_ds_cfg.encryption);
     }

    return rc;
}

int bcm_ploamPortCtrlEncryptGemPortById(uint16_t gemPortId, bool encrypt)
{
    UINT16 gem_index = 0;

    if (get_gem_flow_id_by_gem_port(gemPortId, &gem_index) < 0)
        return -ENOENT;

    return (bcm_ploamPortCtrlEncryptGemPortByIdx(gem_index, encrypt));
}

int bcm_ploamPortCtrlEncryptGemPortByIdx(uint16_t gemPortIdx, bool encrypt)
{
    bdmf_error_t rc = BDMF_ERR_OK;
    bdmf_object_handle gem = NULL;

    bdmf_lock();
    rc = rdpa_gem_get(gemPortIdx, &gem);
    rc = rc ? rc : rdpa_gpon_gem_encryption_set(gponObj, gem, encrypt);
    bdmf_unlock();
    if (rc < 0)
        BCM_LOG_ERROR (BCM_LOG_ID_PLOAM, "bcm_ploamPortCtrlEncryptGemPortByIdx: error %d\n", rc);

    if (gem)
        bdmf_put(gem);

    return rc;
}









