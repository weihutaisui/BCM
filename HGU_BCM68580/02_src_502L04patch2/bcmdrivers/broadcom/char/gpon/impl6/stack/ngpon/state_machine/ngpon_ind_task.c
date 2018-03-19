/*
* <:copyright-BRCM:2015:proprietary:gpon
* 
*    Copyright (c) 2015 Broadcom 
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
 
/*****************************************************************************/
/*                                                                           */
/* Include files                                                             */
/*                                                                           */
/*****************************************************************************/
/* general */
#ifndef SIM_ENV
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/interrupt.h>
#include <bcm_ext_timer.h>
#else
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#endif
#include <bdmf_dev.h>

/* driver */
#include "ngpon_db.h"
#include "aes_cmac.h"
#include "pon_sm_common.h"
#include "boardparms.h"
#ifdef CONFIG_BCM_GPON_TODD
#include "ngpon_tod.h"
#endif
#if defined(USE_LOGGER)
#include "gpon_logger.h"
#endif









/******************************************************************************/
/*                                                                            */
/* Types and values definitions                                               */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/* Functions Definitions                                                      */
/*                                                                            */
/******************************************************************************/

void p_handle_oam_message ( NGPON_MESSAGE * xi_message_ptr );  /* Insert it ??? */
void ngpon_pon_task ( void ) ;

static void ngpon_default_panic_callback (void);
static void ngpon_default_event_and_alarm_callback (PON_API_MESSAGE_OPCODES_DTE xi_message_id , PON_USER_INDICATION_DTE xi_user_indication_message);
static void ngpon_default_change_power_level_callback (NGPON_POWER_LEVEL_PARAMETERS param) ;
static void ngpon_default_sc_sc_runner_callback (uint8_t  *xo_buffer,  uint8_t  *xo_flag);
static int  ngpon_default_flush_tcont_callback (uint8_t tcont_id);

extern void pmd_dev_change_tracking_state(uint32_t old_state, uint32_t new_state);

#ifdef SIM_ENV

uint16_t bdmf_rand16(void)
{

}

int bdmf_timer_start(bdmf_timer_t *timer, uint32_t ticks)
{
    return 0;
}

bdmf_error_t bdmf_queue_send(bdmf_queue_t *queue, char *xi_buffer, uint32_t xi_length)
{
  return BDMF_ERR_OK ;
}

uint32_t bdmf_get_cpu_frequency(void)
{
    return 100;
}

bdmf_error_t bdmf_queue_create(bdmf_queue_t *queue, uint32_t xi_number_of_messages, uint32_t xi_max_message_length)
{
  return BDMF_ERR_OK ;    
}

void bdmf_timer_init(bdmf_timer_t *timer, bdmf_timer_cb_t cb, unsigned long priv)
{

}


void bdmf_timer_stop(bdmf_timer_t *timer)
{
    
}

int bdmf_task_create(const char *name, int priority, int stack,
    int (*handler)(void *arg), void *arg, bdmf_task *ptask)
{
  return 0 ;
}

int bdmf_task_destroy(bdmf_task task)
{
  return 0 ;
}

bdmf_error_t bdmf_queue_delete(bdmf_queue_t *queue)
{
  return BDMF_ERR_OK ;
}

void bdmf_timer_delete(bdmf_timer_t *timer)
{

}

bdmf_error_t bdmf_queue_receive(bdmf_queue_t *queue, char *xo_buffer, uint32_t *xio_length)
{
  return BDMF_ERR_OK ;

}


const char *bdmf_strerror(int err)
{
    static char *error[256];
    memset (error, 0x0, sizeof (error));
    sprintf ((char *)error,"BDMF Error Code %d", err) ;

    return (const char *)error;
}


void bdmf_srand(unsigned int seed)
{
  return ;
}


#endif


char * g_link_state_names [ LINK_STATE_ACTIVE + 1 ] =
{
    "INACTIVE",
    "ACTIVE",
} ;


NGPON_CALLBACKS ngpon_callbacks = {

    /* Panic */ 
    ngpon_default_panic_callback,

    /* Events and Alarms */
    ngpon_default_event_and_alarm_callback,

    /* Change power level callback */
    ngpon_default_change_power_level_callback,

    /* Sc SC callback */
    ngpon_default_sc_sc_runner_callback,

    /* Flush FE buffer callback */
    ngpon_default_flush_tcont_callback

} ;




/******************************************************************************/
/* This type defines the reasons for ranging stop types                       */
/******************************************************************************/
#ifdef INCLUDE_LOGS
char * gs_ranging_stop_reasons [ CE_XGPON_RANGING_STOP_REASONS_NUMBER ] =
{
    /* Device deactivate */
    "LINK_DEACTIVATE",
    /* Alarm On */
    "REASON_ALARM_ON",
    /* SN Unmatch */
    "SN_UNMATCH",
    /* Deactivate Pon Id */
    "DEACTIVATE_PON_ID",
    /* Disable SN */
    "DISABLE_SN",
    /* Ranging End */
    "RANGING_END",
    /* Timeout Expired */
    "TIMEOUT_EXPIRED"
};
#endif /* INCLUDE_LOGS */


/* Assign callback routines */
static void handle_assign_callback_routines( NGPON_MESSAGE *message_ptr)
{

    ngpon_callbacks.event_and_alarm_callback = message_ptr->message_data.callback_routines.event_and_alarm_callback;
    /*    ngpon_callbacks.dbr_runner_data_callback = message_ptr->message_data.callback_routines.dbr_runner_data_callback; */
    ngpon_callbacks.sc_sc_runner_callback = message_ptr->message_data.callback_routines.sc_sc_runner_callback;
    /* ngpon_callbacks.flush_tcont_callback = message_ptr->message_data.callback_routines.flush_tcont_callback; */
    ngpon_callbacks.flush_tcont_callback = message_ptr->message_data.callback_routines.flush_tcont_callback;
    return;
}

void change_pmd_tracking_state(uint32_t old_state, uint32_t new_state)
{
    if (g_xgpon_db.optics_type == BP_GPON_OPTICS_TYPE_PMD)
    {
        pmd_dev_change_tracking_state(old_state, new_state);
        p_log(ge_onu_logger.sections.stack.pon_task.unknown_inter_task_messages_id,
            "PMD move tracking from %d state to %d state", old_state, new_state);
    }
}



bdmf_error_t send_message_to_pon_task ( PON_EVENTS_DTE xi_msg_opcode, ...)
{
    bdmf_error_t                          msg_error ;
    va_list                               parameters ;
    NGPON_MESSAGE                         pon_message ;

    /* Init the va database */
    va_start ( parameters, xi_msg_opcode );

    /* For all messages - set opcode */
    pon_message.message_id = xi_msg_opcode ;

    switch ( xi_msg_opcode )
    {
    case PON_EVENT_CHANGE_POWER_LEVEL :
        pon_message.message_data.power_level = va_arg(parameters, NGPON_POWER_LEVEL_PARAMETERS);
        break;

    case PON_EVENT_ASSIGN_CALLBACKS :
        pon_message.message_data.callback_routines = va_arg(parameters, NGPON_CALLBACKS);
        break;

    case PON_EVENT_QUEUE_EVENT:
        pon_message.message_data.fifo_block = va_arg(parameters, uint32_t);
        break ;

    case PON_EVENT_KEEPALIVE_TIMEOUT:
        pon_message.message_data.keepalive_timeout = va_arg(parameters, uint32_t);
        break ;

    case PON_EVENT_LINK_STATE_TRANSITION:
        pon_message.message_data.user_indication.indication_data.onu_link_state_transition.onu_old_link_state = va_arg(parameters, LINK_OPERATION_STATES_DTE);
        pon_message.message_data.user_indication.indication_data.onu_link_state_transition.onu_new_link_state = va_arg (parameters, LINK_OPERATION_STATES_DTE);
        change_pmd_tracking_state(pon_message.message_data.user_indication.indication_data.onu_link_state_transition.onu_old_link_state,
            pon_message.message_data.user_indication.indication_data.onu_link_state_transition.onu_new_link_state);
        break;


    case PON_EVENT_OAM:
        pon_message.message_data.oam.oam_alarm_id = va_arg ( parameters, NGPON_OAM_ALARM_ID_DTE ) ;
        pon_message.message_data.oam.oam_alarm_status = va_arg ( parameters, uint32_t ) ;

        switch ( pon_message.message_data.oam.oam_alarm_id )
        {
            case NGPON_INDICATION_MEM :
                pon_message.message_data.user_indication.indication_data.ngpon_mem.unknown_msg = va_arg (parameters, NGPON_DS_PLOAM);
                break;
        }

        break;

    case PON_EVENT_USER_INDICATION:
        pon_message.message_data.user_indication.indication_id = va_arg(parameters, PON_INDICATIONS_DTE);
        switch (pon_message.message_data.user_indication.indication_id)
        {
            case PON_INDICATION_RANGING_START:
            {
                break;
            }
            case PON_INDICATION_RANGING_STOP:
            {
                pon_message.message_data.user_indication.indication_data.ranging_stop.ranging_stop_reason = va_arg(parameters, NGPON_RANGING_STOP_REASONS_INDICATIONS);
                break;
            }
            case PON_INDICATION_ASSIGN_ALLOC_ID:
            {
                pon_message.message_data.user_indication.indication_data.assign_alloc_id = va_arg(parameters, PON_ASSIGN_ALLOC_ID_INDICATION);
                break;
            }
            case PON_INDICATION_ASSIGN_ONU_ID:
            {
                pon_message.message_data.user_indication.indication_data.assign_onu_id = va_arg(parameters, PON_ASSIGN_ONU_ID_INDICATION);
                break;
            }
            case PON_INDICATION_CONFIGURE_OMCI_PORT_ID_FILTER:
            {
                 pon_message.message_data.user_indication.indication_data.configure_omci_port = va_arg(parameters, PON_CONFIGURE_OMCI_PORT_ID_INDICATION);
                 break;
            }
            case PON_INDICATION_MODIFY_ENCRYPTION_PORT_ID_FILTER:
            {
                pon_message.message_data.user_indication.indication_data.configure_port = va_arg(parameters, PON_CONFIGURE_FILTER_INDICATION);
                break;
            }
            case PON_INDICATION_LCDG:
            {
                pon_message.message_data.user_indication.indication_data.lcdg = va_arg(parameters, PON_ALARM_INDICATION);
                break;
            }
            case PON_INDICATION_LOF:
            {
                pon_message.message_data.user_indication.indication_data.lof = va_arg(parameters, PON_ALARM_INDICATION);
                break;
            }
            case PON_INDICATION_AES_ERROR:
            case PON_INDICATION_AES_KEY_SWITCHING_TIME_MESSAGE:
            {
                pon_message.message_data.user_indication.indication_data.aes = va_arg(parameters, PON_AES_INDICATION);
                break;
            }
            case PON_INDICATION_ERR:
            {
                pon_message.message_data.user_indication.indication_data.bip_error = va_arg(parameters, PON_ERROR_INDICATION);
                break;
            }
            case PON_INDICATION_ROGUE_ONU:
            {
                pon_message.message_data.user_indication.indication_data.rogue_onu = va_arg(parameters, PON_ROGUE_ONU_INDICATION);
                break;
            }
            case PON_INDICATION_PMD_ALARM:
            {
                pon_message.message_data.user_indication.indication_data.pmd_alarm = va_arg(parameters, PMD_ALARM_INDICATION_PARAMETERS_DTE);
                break;
            }
            default:
            {
                break;
            }
        }

        break;

    case PON_EVENT_CRITICAL_BER_TIMEOUT:
        break;

    case PON_EVENT_BER_TIMEOUT:
        break ;

    case PON_EVENT_PEE_TIMEOUT:
        break;

    case PON_EVENT_CTRL_BER_TIMER:
        pon_message.message_data.timer_control.interval = va_arg(parameters, uint32_t);
        pon_message.message_data.timer_control.command = va_arg(parameters,  uint32_t);
        break;

    case PON_EVENT_CTRL_PEE_TIMER:
        break;

    case PON_EVENT_PST_PLOAM:
        break;

    default:
        return (NGPON_ERROR_OS_ERROR) ;/* TBD -fix error code */
        break ;
    }

    /* Since the parameter/buffer we send is not the message itself but a pointer to the */
    /* message, we have to send the pointer's address  ( & message_ptr ) */

    /* Send the message */
    msg_error = bdmf_queue_send(&g_xgpon_db.os_resources.pon_messages_queue_id, 
                   (char *) (&pon_message), sizeof(NGPON_MESSAGE));
 
    if ( msg_error != BDMF_ERR_OK )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Send PON message failed due to stt_error: %d ",msg_error ) ;
        return (msg_error);/* TBD -fix error code */
    }

    va_end (parameters);
    return BDMF_ERR_OK ;
}

void p_handle_oam_message (NGPON_MESSAGE * xi_message_ptr)
{
    PON_USER_INDICATION_DTE user_indication_message;

    /* Init oam indications */
    user_indication_message.indication_id = xi_message_ptr->message_data.oam.oam_alarm_id;

    switch ( xi_message_ptr -> message_data.oam.oam_alarm_id )
    {
    case PON_INDICATION_LOF:
        p_log (ge_onu_logger.sections.stack.pon_task.general_id, "Loss of Frame (LOF) Alaram Status %s ",
            xi_message_ptr->message_data.oam.oam_alarm_status ? "ON" :"OFF"); 
        user_indication_message.indication_data.lof.alarm = xi_message_ptr->message_data.oam.oam_alarm_status;
        break;

    case PON_INDICATION_LCDG:
        p_log ( ge_onu_logger.sections.stack.pon_task.general_id, 
            "Loss of Channel Deliniation for GEM (LCDG) Alaram Status %s " ,
            xi_message_ptr->message_data.oam.oam_alarm_status ? "ON" :"OFF");
        user_indication_message.indication_data.lcdg.alarm = xi_message_ptr->message_data.oam.oam_alarm_status;
        break;

    default:
        p_log (ge_onu_logger.sections.stack.pon_task.unknown_inter_task_messages_id, 
            "Unknown ONU alarm %d ", xi_message_ptr->message_data.oam.oam_alarm_id);
        return ;
    }
    ngpon_callbacks.event_and_alarm_callback(PON_API_MESSAGE_OPCODE_OAM_EVENT, user_indication_message);

}


void ngpon_pon_task(void)
{
    uint32_t length;
    bdmf_error_t bdmf_error;
    NGPON_MESSAGE indication_message;
    LINK_OPERATION_STATES_DTE old_link_state;
    LINK_OPERATION_STATES_DTE new_link_state;
    
    for (;;)
    {
        length = sizeof(NGPON_MESSAGE);
        
        bdmf_error = bdmf_queue_receive(&g_xgpon_db.os_resources.pon_messages_queue_id,
            (char *)(&indication_message), (uint32_t *)&length);
        if (bdmf_error != BDMF_ERR_OK)
        {
            p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
                "NGPON task Error receving message %d\n", bdmf_error);
            continue;
        }
        if (length != sizeof(NGPON_MESSAGE))
        {
            p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
                "Invalid message length %d in NGPON task %d\n", length);
            continue;
        }
        
        switch (indication_message.message_id)
        {
        case PON_EVENT_OAM:
            p_handle_oam_message(& indication_message);
            break ;
        
        case PON_EVENT_LINK_STATE_TRANSITION:
            old_link_state = 
                indication_message.message_data.user_indication.indication_data.onu_link_state_transition.onu_old_link_state;
            new_link_state = 
                indication_message.message_data.user_indication.indication_data.onu_link_state_transition.onu_new_link_state;
            p_log(ge_onu_logger.sections.stack.operating.state_change_id,
                "Link SM: Old State:%s -> New State:%s", p_get_operation_state_name(old_link_state),
                p_get_operation_state_name(new_link_state));
        
            /* Call to user callback with link State event */
            ngpon_callbacks.event_and_alarm_callback(PON_API_MESSAGE_OPCODE_GPON_LINK_STATE_TRANSITION,
                indication_message.message_data.user_indication);

#ifdef CONFIG_BCM_GPON_TODD
            tod_handle_onu_state_transition(old_link_state, new_link_state);
#endif 
            break;
        
        case PON_EVENT_ASSIGN_CALLBACKS:
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
            "Assign callback routines event indication arrived");
            handle_assign_callback_routines(&indication_message);
            break;
        
        case PON_EVENT_USER_INDICATION:
            switch (indication_message.message_data.user_indication.indication_id)
            {
            case PON_INDICATION_RANGING_START:
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id,
                    "Ranging Start indication arrived");

                ngpon_callbacks.event_and_alarm_callback(PON_API_MESSAGE_OPCODE_RANGING_START,
                   indication_message.message_data.user_indication);
                break;
        
            case PON_INDICATION_RANGING_STOP:
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id,
                    "Ranging Stop indication arrived, reason %s !!!",
                    gs_ranging_stop_reasons[indication_message.message_data.user_indication.indication_data.ranging_stop.ranging_stop_reason]);

                ngpon_callbacks.event_and_alarm_callback(PON_API_MESSAGE_OPCODE_RANGING_STOP,
                   indication_message.message_data.user_indication);
                break;
        
            case PON_INDICATION_ASSIGN_ALLOC_ID:
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id,
                    "%s Alloc-id %lu (0x%X) indication arrived",
                indication_message.message_data.user_indication.indication_data.assign_alloc_id.assign_flag == BDMF_TRUE ?
                    "Assign" : "Deassign",
                indication_message.message_data.user_indication.indication_data.assign_alloc_id.alloc_id,
                indication_message.message_data.user_indication.indication_data.assign_alloc_id.alloc_id);

                ngpon_callbacks.event_and_alarm_callback(PON_API_MESSAGE_OPCODE_ASSIGN_ALLOC_ID_MESSAGE,
                   indication_message.message_data.user_indication);
                break;
        
            case PON_INDICATION_ASSIGN_ONU_ID:
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id,
                     "Assign ONU-ID %lu (0x%X) Indication Arrived",
                     indication_message.message_data.user_indication.indication_data.assign_onu_id.onu_id,
                     indication_message.message_data.user_indication.indication_data.assign_onu_id.onu_id);

                ngpon_callbacks.event_and_alarm_callback(PON_API_MESSAGE_OPCODE_ASSIGN_ONU_ID_MESSAGE,
                    indication_message.message_data.user_indication);
                break;
        
            case PON_INDICATION_LCDG:
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id,
                    "LCDG Indication Arrived",
                    indication_message.message_data.user_indication.indication_data.lcdg.alarm == BDMF_TRUE ? 
                        "ON" : "OFF");

                ngpon_callbacks.event_and_alarm_callback(PON_INDICATION_LCDG,
                   indication_message.message_data.user_indication);
                break;

            case PON_INDICATION_CONFIGURE_OMCI_PORT_ID_FILTER:
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id,
                    "Configure OMCI Port %lu (0x%X) Indication Arrived",
                    indication_message.message_data.user_indication.indication_data.configure_omci_port.port_id,
                    indication_message.message_data.user_indication.indication_data.configure_omci_port.port_id);

                ngpon_callbacks.event_and_alarm_callback(PON_API_MESSAGE_OPCODE_CONFIGURE_OMCI_PORT_ID,
                   indication_message.message_data.user_indication);
                break;

            case PON_INDICATION_ROGUE_ONU:
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id,
                     "ROGUE_ONU error indication arrived");

                /* Call to user callback with Rogue indication indication */
                ngpon_callbacks.event_and_alarm_callback(PON_API_MESSAGE_OPCODE_ROGUE_ONU,
                    indication_message.message_data.user_indication);
                break;

            case PON_INDICATION_PMD_ALARM:
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id,
                     "PMD_ALARM indication arrived");

                /* Call to user callback with Rogue indication indication */
                ngpon_callbacks.event_and_alarm_callback(PON_API_MESSAGE_OPCODE_PMD_ALARM,
                        indication_message.message_data.user_indication);
                break;
            
            default:
                p_log(ge_onu_logger.sections.stack.pon_task.unknown_inter_task_messages_id, 
                    "Unknown indication arrived %d", 
                    indication_message.message_data.user_indication.indication_id);
                break;
        }
        break;
        
        default:
            p_log(ge_onu_logger.sections.stack.pon_task.unknown_inter_task_messages_id, 
                "Unknown pon message (%d)", indication_message.message_id);
            break;
        }
    }
}


PON_ERROR_DTE ngpon_get_all_tx_counters (XGPON_TXPON_PM_COUNTERS * xi_pm_tx)
{
    PON_TCONT_ID_DTE           tcont_id  ;
    NGPON_PLOAM_TYPE         ploam_buf = NGPON_PLOAM_TYPE_LOW ;

    /* Define parameters handling variables */
    PON_ERROR_DTE             error ;

    for ( tcont_id = 0 ; tcont_id < NGPON_NUM_OF_TCONT_AT_ALLOC_INDEX_ARRAY ; tcont_id++  )  
    {
        error = ag_drv_ngpon_tx_perf_mon_rx_accs_cnt_get ( tcont_id ,&xi_pm_tx->received_access[tcont_id] );
        error = ag_drv_ngpon_tx_perf_mon_req_dbr_cnt_get ( tcont_id ,&xi_pm_tx->requested_dbr_counter[tcont_id] );
        error = ag_drv_ngpon_tx_perf_mon_tx_pckt_cnt_get( tcont_id ,&xi_pm_tx->transmitted_packet_counter[tcont_id] );
        error = ag_drv_ngpon_tx_perf_mon_tx_xgem_frm_cnt_get ( tcont_id ,&xi_pm_tx->transmitted_xgem_frames_counter[tcont_id] );
        error = ag_drv_ngpon_tx_perf_mon_tx_xgem_idle_cnt_get ( tcont_id ,&xi_pm_tx->transmitted_xgem_idle_frames_counter[tcont_id] );
    }
    /*
    for ( ploam_buf = NGPON_PLOAM_TYPE_LOW ; ploam_buf <= NGPON_PLOAM_TYPE_HIGH ; ploam_buf++ )  
    {
        error = ag_drv_ngpon_tx_perf_mon_req_plm_get (ploam_buf, &xi_pm_tx->requested_ploam_counter[ploam_buf] );
    }
    */
    for (ploam_buf = NGPON_PLOAM_TYPE_LOW; ploam_buf <= NGPON_PLOAM_TYPE_HIGH; ploam_buf ++)
    {
       error = ag_drv_ngpon_tx_perf_mon_req_plm_get (ploam_buf, &xi_pm_tx->requested_ploam_counter[ploam_buf] );
    }
    return ( BDMF_ERR_OK ) ;
}


PON_ERROR_DTE ngpon_get_all_rx_counters (XGPON_RXPON_PM_COUNTERS * xi_pm_rx)
{
        NGPON_ONU_INDEX      onu_index ; 
        PON_TCONT_ID_DTE     tcont_id  ; 
        PON_FLOW_ID_DTE      xgem_flow ; 
        PON_ERROR_DTE        error ;

        error = ag_drv_ngpon_rx_pm_sync_lof_get ( &xi_pm_rx->lof_num_counter ) ;
        error = ag_drv_ngpon_rx_pm_bwmap_hec_err_get ( &xi_pm_rx->hec_err_counter ) ;
        error = ag_drv_ngpon_rx_pm_bwmap_invalid_burst_series_get ( &xi_pm_rx->bwmap_invalid_burst_series_counter ) ;

        
        error = ag_drv_ngpon_rx_pm_hlend_hec_err_get ( &xi_pm_rx->hlend_hec_err_counter ) ;
        error = ag_drv_ngpon_rx_pm_del_lcdg_get ( &xi_pm_rx->lcdg_counter ) ;
        error = ag_drv_ngpon_rx_pm_sync_ponid_hec_err_get ( &xi_pm_rx->sync_ponid_hec_err_counter ) ;
        error = ag_drv_ngpon_rx_pm_del_pass_pkt_get ( &xi_pm_rx->del_pass_pkt_counter ) ;
        error = ag_drv_ngpon_rx_pm_bwmap_correct_get ( &xi_pm_rx->bwmap_correct_counter ) ;
        error = ag_drv_ngpon_rx_pm_bwmap_bursts_get ( &xi_pm_rx->bwmap_bursts_counter ) ;
        error = ag_drv_ngpon_rx_pm_xhp_pass_pkt_get /* ngpon_drv_rx_get_pm_pass_xgem_filter_pkt */ ( &xi_pm_rx->pass_xgem_filter_pkt_counter ) ;
        error = ag_drv_ngpon_rx_pm_dec_cant_allocate_get ( &xi_pm_rx->cipher_cant_allocate_counter ) ;
        error = ag_drv_ngpon_rx_pm_dec_invalid_key_idx_get ( &xi_pm_rx->xgem_frame_invalid_key_idx_counter ) ;
        for ( onu_index = NGPON_ONU_INDEX_UNICAST_ONU_ID_LOW ; onu_index <= NGPON_ONU_INDEX_UNICAST_ONU_ID_HIGH ; onu_index ++  ) 
        {
            error = ag_drv_ngpon_rx_pm_plp_valid_onuid_get ( onu_index, &xi_pm_rx->valid_unicast_ploams_counter[onu_index] );
         }
        error = ag_drv_ngpon_rx_pm_plp_valid_bcst_get ( &xi_pm_rx->valid_broadcast_ploams_counter ) ;
        error = ag_drv_ngpon_rx_pm_plp_mic_err_get ( &xi_pm_rx->ploam_mic_err_counter ) ;
        error = ag_drv_ngpon_rx_pm_bwmap_hec_fix_get ( &xi_pm_rx->bwmap_hec_fix_counter ) ;
        error = ag_drv_ngpon_rx_pm_hlend_hec_fix_get ( &xi_pm_rx->hlend_hec_fix_counter) ;
        error = ag_drv_ngpon_rx_pm_xhp_hec_fix_get /* ngpon_drv_rx_get_pm_xgem_header_hec_fix */ ( &xi_pm_rx->xgem_header_hec_fix_counter ) ;
        error = ag_drv_ngpon_rx_pm_sync_sfc_hec_fix_get ( &xi_pm_rx->superframe_hec_fix_counter ) ;
        error = ag_drv_ngpon_rx_pm_sync_ponid_hec_fix_get ( &xi_pm_rx->pon_id_hec_fix_counter ) ;
        error = ag_drv_ngpon_rx_pm_xgem_overrun_get ( &xi_pm_rx->xgem_overrun_counter ) ;
        error = ag_drv_ngpon_rx_pm_bwmap_discard_dis_tx_get ( &xi_pm_rx->bwmap_discard_dis_tx_counter ) ;
        error = ag_drv_ngpon_rx_pm_fec_bit_err_get ( &xi_pm_rx->corrected_fec_bits_counter ) ;
        error = ag_drv_ngpon_rx_pm_fec_sym_err_get ( &xi_pm_rx->corrected_fec_symbols_counter ) ;
        error = ag_drv_ngpon_rx_pm_fec_cw_err_get ( &xi_pm_rx->corrected_fec_words_counter ) ;
        error = ag_drv_ngpon_rx_pm_fec_uc_cw_get ( &xi_pm_rx->uncorrected_fec_words_counter ) ;

        /*
         * Collected for TCONTs 0-7 only
         */
        for ( tcont_id = 0 ; tcont_id <= 7  ; tcont_id ++  ) 
        {
           error = ag_drv_ngpon_rx_pm_bwmap_num_alloc_tcont_get ( tcont_id, &xi_pm_rx->bwmap_alloc_per_tcont_counter[tcont_id] ) ;
           error = ag_drv_ngpon_rx_pm_bwmap_tot_bw_tcont_get ( tcont_id, &xi_pm_rx->total_bwmap_alloc_per_tcont_counter[tcont_id] ) ;
        }
        for ( xgem_flow = NGPON_XGEM_FLOW_ID_LOW ; xgem_flow <= NGPON_XGEM_FLOW_ID_HIGH ; xgem_flow ++  ) 
        {
           error = ag_drv_ngpon_rx_pm_xhp_xgem_per_flow_get ( xgem_flow, &xi_pm_rx->xgem_frames_per_flow_counter[xgem_flow] ) ;
           error = ag_drv_ngpon_rx_pm_xhp_bytes_per_flow_get ( xgem_flow, &xi_pm_rx->received_bytes_per_xgem_flow_counter[xgem_flow] ) ;
        }
        return ( BDMF_ERR_OK ) ;
}

PON_ERROR_DTE ngpon_assign_user_callback (EVENT_AND_ALARM_CALLBACK_DTE  xi_event_and_alarm_callback,
                 DBR_PROCESS_RUNNER_DATA_CALLBACK_DTE  xi_dbr_runner_data_callback,
                 SC_SC_RUNNER_DATA_CALLBACK_DTE        xi_sc_sc_runner_callback,
                 FLUSH_TCONT_CALLBACK_DTE              xi_flush_tcont_callback)
{
    PON_ERROR_DTE xgpon_error ;
    NGPON_CALLBACKS callback_struct ;


    /* Verify that the stack is active */
    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return (PON_ERROR_INVALID_STATE) ;
    }
    /* Check for initial pointer for callback function */
    if (xi_event_and_alarm_callback == 0)    
    {
        return (PON_ERROR_INVALID_STATE) ;
    }

    /* Init the callback struct */
    callback_struct.event_and_alarm_callback    = xi_event_and_alarm_callback ;
    /*    callback_struct.dbr_runner_data_callback    = xi_dbr_runner_data_callback ; */
    callback_struct.sc_sc_runner_callback       = xi_sc_sc_runner_callback ;
    callback_struct.flush_tcont_callback        = xi_flush_tcont_callback ;


    /* Send indication to Pon task to assign callback functions */
    xgpon_error = send_message_to_pon_task (PON_EVENT_ASSIGN_CALLBACKS, callback_struct);
    if (xgpon_error != PON_NO_ERROR)
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send assign callbacks request: error %d !", xgpon_error ) ;
        return (xgpon_error) ;
    }
    return (PON_NO_ERROR) ;
}

void ngpon_default_panic_callback (void)
{
  return ;
}

void ngpon_default_event_and_alarm_callback (PON_API_MESSAGE_OPCODES_DTE xi_message_id , PON_USER_INDICATION_DTE xi_user_indication_message)
{
  p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Default event_and_alarm_callback activated fro message %d !", xi_message_id) ;
  return ;
}

void ngpon_default_change_power_level_callback (NGPON_POWER_LEVEL_PARAMETERS param)
{
  return ;
}

void ngpon_default_sc_sc_runner_callback (uint8_t  *xo_buffer,  uint8_t  *xo_flag)
{
  return ;
}

int ngpon_default_flush_tcont_callback (uint8_t tcont_id)
{
  return 0;
}


