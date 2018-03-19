/*
* <:copyright-BRCM:2016:proprietary:gpon
* 
*    Copyright (c) 2016 Broadcom 
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

#include <boardparms.h>
#include "gpon_ind_task.h"
#include "gpon_db.h"    
#include "gpon_stat.h"    
#include "gpon_logger.h"
#include "gpon_general.h"
#ifdef CONFIG_BCM_GPON_TODD
#include "gpon_tod_common.h"
#endif

extern GPON_DATABASE_DTS gs_gpon_database;

#define CE_DS_RATE 2488320 
#define CS_MAX_REI_SEQUENCE_NUMBER 16 
#define CS_MAX_BER_INTERVAL 10000 
/* maximal BER interval is limited to 10000 milliseconds in order to avoid the counter being saturated */
#define CS_MAX_POWER_OF_10 9 
static  uint32_t gs_10_to_the_power_of[CS_MAX_POWER_OF_10 + 1] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};
#define MS_10_TO_THE_POWER_OF( p )((((p) < 1 ) ||((p) > CS_MAX_POWER_OF_10 ) ) ? 1 : gs_10_to_the_power_of[(p)])

typedef void(* GPON_EVENTS_ALARMS_HOOK_DTE)(uint32_t  xi_event_opcode, PON_USER_INDICATION_DTE xi_user_indication_message);
#define p_gpon_events_callback(message_id, user_indication_message) ((GPON_EVENTS_ALARMS_HOOK_DTE) (gs_callbacks.event_and_alarm_callback))(message_id, user_indication_message)

/* Define default callback functions */
static void p_default_event_and_alarm_callback(PON_API_MESSAGE_OPCODES_DTE xi_message_id,
    PON_USER_INDICATION_DTE xi_user_indication_message);
static void p_gpon_dbr_runner_data_callback(uint32_t xi_runner_queue_id, 
    uint32_t * xo_runner_queue_size);
static void p_gpon_sc_sc_runner_data_callback(uint8_t *xo_buffer, uint8_t *xo_flag);
static int p_gpon_flush_tcont_callback(uint8_t tcont_id);

/* Define callback hooks */
GPON_CALLBACKS_DTE gs_callbacks =
{
    /* Event and alarm */
    p_default_event_and_alarm_callback,
    /* DBR status report get runner data callback */
    p_gpon_dbr_runner_data_callback,
    /* SC SC callback */
    p_gpon_sc_sc_runner_data_callback,
    /* Flush callback */
    p_gpon_flush_tcont_callback
};

#ifdef INCLUDE_LOGS
static char *gs_ranging_stop_reasons[PON_RANGING_STOP_REASONS_NUMBER]=
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
#endif 

extern void pmd_dev_change_tracking_state(uint32_t old_state, uint32_t new_state);

/* Assign callback routines */
static void p_handle_assign_callback_routines(GPON_MESSAGE_DTE *xi_message_ptr)
{
    gs_callbacks.event_and_alarm_callback = xi_message_ptr->message_data.callback_routines.event_and_alarm_callback;
    gs_callbacks.dbr_runner_data_callback = xi_message_ptr->message_data.callback_routines.dbr_runner_data_callback;
    gs_callbacks.sc_sc_runner_callback = xi_message_ptr->message_data.callback_routines.sc_sc_runner_callback;
    gs_callbacks.flush_tcont_callback = xi_message_ptr->message_data.callback_routines.flush_tcont_callback;
    return;
}


/* Keepalive Timeout  */
static void p_handle_keepalive_message(GPON_MESSAGE_DTE *xi_message_ptr)
{
    PON_USER_INDICATION_DTE user_indication_message;

    user_indication_message.indication_data.keep_alive.keepalive_time_value = 
        xi_message_ptr->message_data.keepalive_timeout;

    /** Call to user callback with OAM events mask */
#ifdef TBD
    p_gpon_events_callback(PON_API_MESSAGE_OPCODE_KEEPALIVE_TIMEOUT,
        user_indication_message);
#endif
}


/* OAM Event */
static void p_handle_oam_message(GPON_MESSAGE_DTE *xi_message_ptr)
{
    PON_USER_INDICATION_DTE user_indication_message;

    /* Init oam indications */
    user_indication_message.indication_data.oaml.oaml_alarm_status = gs_gpon_database.last_alarm_status;
    user_indication_message.indication_id = xi_message_ptr->message_data.oam.oam_alarm_id;

    switch(xi_message_ptr->message_data.oam.oam_alarm_id )
    {
    case PON_INDICATION_PEE_OLT:
        user_indication_message.indication_data.pee_olt.alarm = xi_message_ptr->message_data.oam.oam_alarm_status;
        /* Call to user callback with OAM events mask */
        p_gpon_events_callback(PON_API_MESSAGE_OPCODE_OAM_EVENT,
            user_indication_message);
        break;

    case GPON_INDICATION_MEM:
        user_indication_message.indication_data.gpon_mem.alarm = xi_message_ptr->message_data.oam.oam_alarm_status;
        /* Call to user callback with OAM events mask */
        memcpy(&user_indication_message.indication_data.gpon_mem.unknown_msg, 
           &(xi_message_ptr->message_data.oam.oam_payload.ploam_message), sizeof(GPON_DS_PLOAM));
        p_gpon_events_callback(PON_API_MESSAGE_OPCODE_OAM_EVENT,
            user_indication_message);
        break;

    case PON_INDICATION_DACT:
        user_indication_message.indication_data.dact.alarm = xi_message_ptr->message_data.oam.oam_alarm_status;
        /* Call to user callback with OAM events mask */
        p_gpon_events_callback(PON_API_MESSAGE_OPCODE_OAM_EVENT,
            user_indication_message);
        break;

    case PON_INDICATION_DIS:
        user_indication_message.indication_data.dis.alarm = xi_message_ptr->message_data.oam.oam_alarm_status;
        /* Call to user callback with OAM events mask */
        p_gpon_events_callback(PON_API_MESSAGE_OPCODE_OAM_EVENT,
            user_indication_message);
        break;

    case PON_INDICATION_LOF:
        user_indication_message.indication_data.lof.alarm = xi_message_ptr->message_data.oam.oam_alarm_status;
        p_gpon_events_callback(PON_API_MESSAGE_OPCODE_OAM_EVENT,
            user_indication_message);
        break;

    case PON_INDICATION_LCDG:
        if (gs_gpon_database.last_alarm_status.lcd != xi_message_ptr->message_data.oam.oam_alarm_status )
        {
            user_indication_message.indication_data.lcdg.alarm = xi_message_ptr->message_data.oam.oam_alarm_status;
            gs_gpon_database.last_alarm_status.lcd = xi_message_ptr->message_data.oam.oam_alarm_status;
            p_gpon_events_callback(PON_API_MESSAGE_OPCODE_OAM_EVENT, user_indication_message);
        }
        break;

    case PON_INDICATION_SD:
        user_indication_message.indication_data.sd.alarm = xi_message_ptr->message_data.oam.oam_alarm_status;
        p_gpon_events_callback(PON_API_MESSAGE_OPCODE_OAM_EVENT, user_indication_message);
        break;

    case PON_INDICATION_SF:
        user_indication_message.indication_data.sf.alarm = xi_message_ptr->message_data.oam.oam_alarm_status;
        p_gpon_events_callback(PON_API_MESSAGE_OPCODE_OAM_EVENT,
            user_indication_message);
        break;

    case PON_INDICATION_OPERATIONAL_LOF:
        p_gpon_events_callback(PON_API_MESSAGE_OPCODE_OAM_EVENT, user_indication_message);
        break;

    default:
        p_log(ge_onu_logger.sections.stack.pon_task.unknown_inter_task_messages_id, 
            "unknown alarm %d ", xi_message_ptr->message_data.oam.oam_alarm_id);
        return;
    }
}


/* PEE control message */
static void p_handle_pee_control_message(GPON_MESSAGE_DTE * xi_message_ptr)
{
    bdmf_error_t bdmf_error = BDMF_ERR_OK;

    if (xi_message_ptr->message_data.timer_control.command == BDMF_FALSE)
    {
       /* stop PEE interval  */
       f_pon_stop_pee_interval();
    }
    else
    {
        /* Restart the PEE timer */
        bdmf_error = bdmf_timer_start(&gs_gpon_database.os_resources.pee_timer_id, 
            GPON_MILISEC_TO_TICKS(GPON_PON_PEE_TIMEOUT));
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start PEE timer");
    }

    if (bdmf_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Fail to control PEE timer. error = %d ", bdmf_error);
    }

    return;
}


/* BER control message */
static void p_handle_ber_control_message(GPON_MESSAGE_DTE * xi_message_ptr)
{
    if (xi_message_ptr->message_data.timer_control.command == BDMF_FALSE)
    {
        f_pon_stop_ber_interval();
    }
    else
    {
        p_pon_start_ber_interval(xi_message_ptr->message_data.timer_control.interval );

        /* Keep the ber interval */
        gs_gpon_database.onu_parameters.ber_interval = xi_message_ptr->message_data.timer_control.interval;

        if (gs_gpon_database.onu_parameters.ber_interval > CS_MAX_BER_INTERVAL)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "BER interval changed to %d", gs_gpon_database.onu_parameters.ber_interval);
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "BER is bigger then 10000, It will result in bip8 error");
        }
        else
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.ber_interval_message_id, 
                "BER interval changed to %d", gs_gpon_database.onu_parameters.ber_interval);
        }
    }

    return;
}


/* BIP error message */
static void p_handle_bip8_timeout_message(GPON_MESSAGE_DTE *xi_message_ptr)
{
    PON_USER_INDICATION_DTE user_indication_message;
    GPON_US_PLOAM rei_message_ptr;
    PON_ERROR_INDICATION bip_error;
    bdmf_boolean onu_id_disable;
    PON_ERROR_DTE gpon_error;
    int drv_error;
    uint32_t bip_count;
    uint32_t bip_rate;
    uint8_t onu_id;
    bool bip_rate_devide_by_10 ;
    uint32_t ber_threshold_for_sf_assertion;
    uint32_t ber_threshold_for_sd_assertion;
    gpon_tx_general_configuration_ind ind;
    uint32_t i = 0;

    /* Init oam indications */
    user_indication_message.indication_data.oaml.oaml_alarm_status = gs_gpon_database.last_alarm_status;

    /* get pon id for REI message */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_get(0, &onu_id, &onu_id_disable);
    if (onu_id_disable)
    {
        p_log(ge_onu_logger.sections.stack.debug.general_id, 
            "BIP8 timeout for not operational pon id");
        return;
    }

    /* Read BIP8 counter */
    p_read_bip8_error_counters(&bip_count, CS_BER_INTERVAL_TYPE);

    /* Send REI ploam message is required. */
        rei_message_ptr.onu_id = onu_id;
    rei_message_ptr.message_id = CE_US_REMOTE_ERROR_INDICATION_MESSAGE_ID;

    for (i = 0; i < US_PLOAM_MESSAGE_REI_UNSPECIFIED_FIELD_SIZE; i++)
    {
        rei_message_ptr.message.remote_error_indication.unspecified[i]= 0;
    }

    memcpy(rei_message_ptr.message.remote_error_indication.error_count, 
        &bip_count, US_PLOAM_MESSAGE_REI_ERROR_COUNT_FIELD_SIZE);
    rei_message_ptr.message.remote_error_indication.sequence_number = 
        (gs_gpon_database.link_parameters.rei_sequence_number % CS_MAX_REI_SEQUENCE_NUMBER);
        gs_gpon_database.link_parameters.rei_sequence_number ++;

        /* Send the REI message to the Tx PLOAM handler. */
    if (f_gpon_txpon_send_base_level_ploam_message(&rei_message_ptr, 
        GPON_REI_TRANSMISSION_NUMBER) != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Fail to send normal PLOAM message");
        }

    if (gs_gpon_database.onu_parameters.ber_interval > CS_MAX_BER_INTERVAL)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Ber Interval %d msec is out of range for bip8 error collection", 
            gs_gpon_database.onu_parameters.ber_interval);
        return;
    }

    /* Avoid division by zero */
    if (bip_count == 0)
    {
        /* p_log(ge_onu_logger.sections.stack.debug.general_id, "No bip8 errors"); */
        /* if last SF state was 'on', send user indication of 'SF off' */
        if (gs_gpon_database.last_alarm_status.sf == BDMF_TRUE)
        {
            gs_gpon_database.last_alarm_status.sf = BDMF_FALSE;
            p_log(ge_onu_logger.sections.stack.pon_task.general_id, "SF status changed from ON to OFF");
            /* Send user indication "SF" - Off */
            gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM, PON_INDICATION_SF,
                CE_OAM_ALARM_STATUS_OFF);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to send oam indication: error %d !", gpon_error);
            }
         }

        /* if last SD state was 'on', send user indication of 'SD off' */
        if (gs_gpon_database.last_alarm_status.sd == BDMF_TRUE)
        {
            gs_gpon_database.last_alarm_status.sd = BDMF_FALSE;
            p_log(ge_onu_logger.sections.stack.pon_task.general_id, "SD status changed from ON to OFF");

            /* Send user indication "SD" - Off */
            gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM, PON_INDICATION_SD,
                CE_OAM_ALARM_STATUS_OFF);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to send oam indication: error %d !", gpon_error);
            }
        }

        if (gs_gpon_database.last_alarm_status.bip_error == BDMF_TRUE)
        {

            gs_gpon_database.last_alarm_status.bip_error = BDMF_FALSE;
            p_log(ge_onu_logger.sections.stack.pon_task.general_id, "ERR status changed from ON to OFF");

            drv_error = ag_drv_gpon_tx_general_configuration_ind_get(&ind);
            if (drv_error > 0)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Driver error %d, unable to get TX indications !", drv_error);
            }

            ind.rdi = BDMF_FALSE;

            drv_error = ag_drv_gpon_tx_general_configuration_ind_set(&ind);
            if (drv_error != PON_NO_ERROR )
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Driver error %d, unable to set TX indications !", drv_error);
            }
        }

        return;
    }

    /* For debug only */
    p_log(ge_onu_logger.sections.stack.pon_task.general_id,
            "BER interval(%d ms ), bip count %d",
            gs_gpon_database.onu_parameters.ber_interval,
            bip_count);

    /* check if there is situation for bit_rate overflow */
    if ((((gs_gpon_database.onu_parameters.ber_interval + (bip_count - 1)) / bip_count) > (0xFFFFFFFF / CE_DS_RATE)) ||
        (gs_gpon_database.onu_parameters.ber_threshold_for_sd_assertion >= CS_MAX_POWER_OF_10))
    {
        bip_rate_devide_by_10 = BDMF_TRUE;
        ber_threshold_for_sd_assertion = (gs_gpon_database.onu_parameters.ber_threshold_for_sd_assertion - 1);
        ber_threshold_for_sf_assertion = (gs_gpon_database.onu_parameters.ber_threshold_for_sf_assertion - 1);
        bip_rate = (CE_DS_RATE / bip_count)*(gs_gpon_database.onu_parameters.ber_interval / 10);

        /* For debug only */
        p_log(ge_onu_logger.sections.stack.pon_task.general_id,
            "bip_rate devided by 10 %u", bip_rate);
    }
    else
    {
        bip_rate_devide_by_10 = BDMF_FALSE;
        ber_threshold_for_sd_assertion = gs_gpon_database.onu_parameters.ber_threshold_for_sd_assertion;
        ber_threshold_for_sf_assertion = gs_gpon_database.onu_parameters.ber_threshold_for_sf_assertion;
        bip_rate = (CE_DS_RATE / bip_count)*gs_gpon_database.onu_parameters.ber_interval;

        /* For debug only */
        p_log(ge_onu_logger.sections.stack.pon_task.general_id, 
            "bip_rate  %u", bip_rate);
    }

    if (gs_gpon_database.last_alarm_status.bip_error == BDMF_FALSE)
    {
        gs_gpon_database.last_alarm_status.bip_error = BDMF_TRUE;
        p_log(ge_onu_logger.sections.stack.pon_task.general_id, 
            "ERR status changed from OFF to ON");

        drv_error = ag_drv_gpon_tx_general_configuration_ind_get(&ind);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Driver error %d, unable to get TX indications !", drv_error);
        }

        ind.rdi = BDMF_TRUE;

        drv_error = ag_drv_gpon_tx_general_configuration_ind_set(&ind);
        if (drv_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Driver error %d, unable to set TX indications !", drv_error);
        }
    }

    bip_error.alarm = CE_OAM_ALARM_STATUS_ON;
    bip_error.bip_error = bip_count;
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION ,
        PON_INDICATION_ERR, bip_error);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
    }

    if (bip_rate < MS_10_TO_THE_POWER_OF(ber_threshold_for_sf_assertion))
    {
        if (gs_gpon_database.last_alarm_status.sf != BDMF_TRUE)
        {
            gs_gpon_database.last_alarm_status.sf = BDMF_TRUE;
            p_log(ge_onu_logger.sections.stack.debug.general_id, "SF on");

            /* Send user indication "SF" - On */
            gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM ,
                PON_INDICATION_SF, CE_OAM_ALARM_STATUS_ON);
            if (gpon_error != PON_NO_ERROR )
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to send oam indication: error %d !", gpon_error);
            }
        }
    }
    else if (bip_rate > MS_10_TO_THE_POWER_OF( ber_threshold_for_sf_assertion + 1))
    {
        /* if last SF state was 'on', send user indication of 'SF off' */
        if (gs_gpon_database.last_alarm_status.sf == BDMF_TRUE)
        {
            gs_gpon_database.last_alarm_status.sf = BDMF_FALSE;
            p_log(ge_onu_logger.sections.stack.pon_task.general_id, 
                "SF status changed from ON to OFF");
            /* Send user indication "SF" - Off */
            gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM,
                PON_INDICATION_SF, CE_OAM_ALARM_STATUS_OFF);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to send oam indication: error %d !", gpon_error);
            }
        }
    }

    /* if BERR than handle SD alarm. the BER threashhold is 10^-X by standard.
       But we divided both sides on 10 in order to allow using uint32_t  */
    if (bip_rate < MS_10_TO_THE_POWER_OF(ber_threshold_for_sd_assertion))
    {
        if (gs_gpon_database.last_alarm_status.sd != BDMF_TRUE )
        {
            p_log(ge_onu_logger.sections.stack.pon_task.general_id, 
                "bip8 rate reached SD threshold of 10^-5");

            gs_gpon_database.last_alarm_status.sd = BDMF_TRUE;
            p_log(ge_onu_logger.sections.stack.debug.general_id, "SD on");

            /* Send user indication "SD" - On */
            gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM,
                PON_INDICATION_SD, CE_OAM_ALARM_STATUS_ON);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to send oam indication: error %d !", gpon_error);
            }
        }
    }
    else if (bip_rate > MS_10_TO_THE_POWER_OF(ber_threshold_for_sd_assertion + 1))
    {
        /* if last SD state was 'on', send user indication of 'SD off' */
        if (gs_gpon_database.last_alarm_status.sd == BDMF_TRUE)
        {
            gs_gpon_database.last_alarm_status.sd = BDMF_FALSE;
            p_log(ge_onu_logger.sections.stack.pon_task.general_id, 
                "SD status changed from ON to OFF");

            /* Send user indication "SD" - Off */
            gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM ,
                PON_INDICATION_SD, CE_OAM_ALARM_STATUS_OFF);
            if (gpon_error != PON_NO_ERROR )
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to send oam indication: error %d !", gpon_error);
            }
        }
    }
}

/* Hnadle state transition  */
static void p_handle_pon_state_transition(GPON_MESSAGE_DTE *xi_message_ptr)
{
    PON_USER_INDICATION_DTE user_indication_message;

    /* If old sub state not Operational and new state is Operational then start timers: Ber and Keep alive */
    if ((xi_message_ptr->message_data.user_indication.indication_data.onu_state_transition.onu_old_sub_state != LINK_SUB_STATE_OPERATIONAL)
        && (xi_message_ptr->message_data.user_indication.indication_data.onu_state_transition.onu_new_sub_state == LINK_SUB_STATE_OPERATIONAL))
    {
        /* Start ploam keep alive timer */
        bdmf_timer_start(& gs_gpon_database.os_resources.keep_alive_timer_id, 
            GPON_MILISEC_TO_TICKS(gs_gpon_database.physical_parameters.upstream_ploam_rate));
        p_log(ge_onu_logger.sections.stack.debug.timers_id, 
            "Keep Alive timer started for %d ms", 
            gs_gpon_database.physical_parameters.upstream_ploam_rate);

        /* Start BER interval default timeout value */
        p_pon_start_ber_interval( gs_gpon_database.onu_parameters.ber_interval);
    }
    else if ((xi_message_ptr->message_data.user_indication.indication_data.onu_state_transition.onu_old_sub_state == LINK_SUB_STATE_OPERATIONAL)
        && (xi_message_ptr->message_data.user_indication.indication_data.onu_state_transition.onu_new_sub_state != LINK_SUB_STATE_OPERATIONAL))
    {
       /* If old state was Operational then Stop all timers:
          Pee, Ber and Keep alive -  Stop Keep Alive interval */
	bdmf_timer_stop(& gs_gpon_database.os_resources.keep_alive_timer_id);

        /* Stop PEE interval */
        f_pon_stop_pee_interval();

        /* Stop BER interval */
        f_pon_stop_ber_interval();
    }

    /* Set link sub state in the data base */
    gs_gpon_database.link_parameters.operation_state_machine.link_sub_state = 
        xi_message_ptr->message_data.user_indication.indication_data.onu_state_transition.onu_new_sub_state;
    user_indication_message.indication_data.onu_state_transition.onu_old_sub_state = 
        xi_message_ptr->message_data.user_indication.indication_data.onu_state_transition.onu_old_sub_state;
    user_indication_message.indication_data.onu_state_transition.onu_new_sub_state = 
        xi_message_ptr->message_data.user_indication.indication_data.onu_state_transition.onu_new_sub_state;

    /* Call to user callback with OAM events mask */
    p_gpon_events_callback(PON_API_MESSAGE_OPCODE_GPON_SM_TRANSITION,
        user_indication_message);
}


/* Handle pon link state transition */
static void p_handle_pon_link_state_transition(GPON_MESSAGE_DTE *xi_message_ptr)
{
    PON_USER_INDICATION_DTE user_indication_message;

    /* Set link sub state in the data base */
    user_indication_message.indication_data.onu_link_state_transition.onu_old_link_state = 
        xi_message_ptr->message_data.user_indication.indication_data.onu_link_state_transition.onu_old_link_state;
    user_indication_message.indication_data.onu_link_state_transition.onu_new_link_state = 
        xi_message_ptr->message_data.user_indication.indication_data.onu_link_state_transition.onu_new_link_state;

    /* Call to user callback with OAM events mask */
    p_gpon_events_callback(PON_API_MESSAGE_OPCODE_GPON_LINK_STATE_TRANSITION,
        user_indication_message);

#ifdef CONFIG_BCM_GPON_TODD
    tod_handle_onu_state_transition(
        user_indication_message.indication_data.onu_link_state_transition.onu_old_link_state ,
        user_indication_message.indication_data.onu_link_state_transition.onu_new_link_state);
#endif 
}

/* PEE error message */
static void p_handle_pee_timeout_message(GPON_MESSAGE_DTE *xi_message_ptr)
{
    PON_ERROR_DTE api_error;
    int drv_error;
    uint8_t onu_id, message_opcode;
    bdmf_boolean onu_id_status;
    GPON_US_PLOAM pee_alarm_message;

    message_opcode = CE_US_PHYSICAL_EQUIPMENT_ERROR_MESSAGE_ID;

    drv_error = ag_drv_gpon_rx_ploam_onu_id_get(0, &onu_id, &onu_id_status);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,  
            "Fail to get pon id. error = %d", drv_error);
        return;
    }

    pee_alarm_message.onu_id = onu_id;
    pee_alarm_message.message_id = message_opcode;

    memset((void*) pee_alarm_message.message.physical_equipment_error.unspecified, 0, US_PLOAM_PEE_UNSPECIFIED_FIELD_SIZE);

    api_error = f_gpon_txpon_send_base_level_ploam_message(&pee_alarm_message, GPON_PEE_ALARM_TRANSMISSION_NUMBER);
    if (api_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Fail to send normal message. error = %d " , api_error);
        return;
    }

    return;
}

/* Vendor Specific  message */
static void p_handle_vendor_specific(GPON_MESSAGE_DTE *xi_message_ptr)
{
#ifdef TBD
    PON_USER_INDICATION_DTE user_indication_message;

    user_indication_message.indication_data.vendor_specific.vendor_specific_msg.type =
        ((DS_PLOAM_VENDOR_SPECIFIC_MESSAGE_DTE *)(&(xi_message_ptr->message_data.ploam_message)))->message_id  & CE_VENDOR_SPECIFIC_MESSAGE_MASK;
    memcpy(user_indication_message.indication_data.vendor_specific.vendor_specific_msg.message_payload,
        ((DS_PLOAM_VENDOR_SPECIFIC_MESSAGE_DTE *) (&(xi_message_ptr->message_data.ploam_message)))->unspecified, 
        CE_DS_PLOAM_VENDOR_SPECIFIC_MESSAGE_UNSPECIFIED_FIELD_SIZE);

    /* Call to user callback with pst parameters */
    p_gpon_events_callback(PON_API_MESSAGE_OPCODE_VENDOR_SPECIFIC_MESSAGE,
        user_indication_message);
#endif
}

/* PST  message */
static void p_handle_pst_message(GPON_MESSAGE_DTE *xi_message_ptr)
{
    PON_USER_INDICATION_DTE user_indication_message;

    memcpy(& user_indication_message.indication_data.pst.pst_msg, 
        &(xi_message_ptr->message_data.ploam_message ), sizeof(GPON_DS_PLOAM));

    /* Call to user callback with pst parameters */
    p_gpon_events_callback(PON_API_MESSAGE_OPCODE_PST_MESSAGE,
        user_indication_message);
}

int p_pon_task(void *xi_parameter)
{
    int32_t length;
    bdmf_error_t bdmf_error;
    GPON_MESSAGE_DTE indication_message;
    PON_API_MESSAGE_OPCODES_DTE message_opcode;

    for (;;)
    {
        length = sizeof(GPON_MESSAGE_DTE);
        bdmf_error = bdmf_queue_receive(& gs_gpon_database.os_resources.pon_messages_queue_id ,
            (char * )(& indication_message ), (uint32_t *)& length);
        if (bdmf_error != BDMF_ERR_OK)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "PON task Error receving message %d \n", bdmf_error);
            continue;
        }

        message_opcode = indication_message.message_id;

        switch (message_opcode)
        {
        case PON_EVENT_CHANGE_POWER_LEVEL:
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                "Change power level event indication arrived");
            break;

        case PON_EVENT_ASSIGN_CALLBACKS :
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                "Assign callback routines event indication arrived");
            p_handle_assign_callback_routines(& indication_message);
            break;

        case PON_EVENT_QUEUE_EVENT:
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                "Queue event indication arrived");
            break;

        case PON_EVENT_KEEPALIVE_TIMEOUT:
#ifdef TBD
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                "Keep alive indication arrived");
#endif
            p_handle_keepalive_message(&indication_message);
            break;

        case PON_EVENT_STATE_TRANSITION:
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                "State transition indication arrived");
            p_handle_pon_state_transition(&indication_message);
            break;

        case PON_EVENT_LINK_STATE_TRANSITION:
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                "Link Operation state transition indication arrived");
            p_handle_pon_link_state_transition(&indication_message);
            break;

        case PON_EVENT_OAM:
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id,
                    "OAM indication arrived ");
            p_handle_oam_message(& indication_message);
            break;

        case PON_EVENT_USER_INDICATION:
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                "User indication arrived");

            switch (indication_message.message_data.user_indication.indication_id)
            {
            case PON_INDICATION_RANGING_START:
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id,
                        "Ranging Start indication arrived");
                /* Call to user callback with Ranging start indication */
                p_gpon_events_callback(PON_API_MESSAGE_OPCODE_RANGING_START,
                    indication_message.message_data.user_indication);
                break;

            case PON_INDICATION_RANGING_STOP :
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id,
                    "Ranging Stop indication arrived, reason %s !!!",
                    gs_ranging_stop_reasons[indication_message.message_data.user_indication.indication_data.ranging_stop.ranging_stop_reason]);
                /* Call to user callback with Ranging stop indication */
                p_gpon_events_callback(PON_API_MESSAGE_OPCODE_RANGING_STOP,
                    indication_message.message_data.user_indication);
                break;

            case PON_INDICATION_ASSIGN_ALLOC_ID :
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                    "Assign / Deassign alloc-id indication arrived");

                /* Call to user callback with Assign alloc-id indication */
                p_gpon_events_callback(PON_API_MESSAGE_OPCODE_ASSIGN_ALLOC_ID_MESSAGE,
                    indication_message.message_data.user_indication);
                break;

            case PON_INDICATION_ASSIGN_ONU_ID :
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                    "Assign ONU-ID indication arrived");

                /* Call to user callback with Assign ONU-id indication */
                p_gpon_events_callback(PON_API_MESSAGE_OPCODE_ASSIGN_ONU_ID_MESSAGE,
                    indication_message.message_data.user_indication);
                break;

            case PON_INDICATION_CONFIGURE_OMCI_PORT_ID_FILTER :
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                    "Configure OMCI port id filter indication arrived ");
                /* Call to user callback with Configure OMCI port id indication */
                p_gpon_events_callback(PON_API_MESSAGE_OPCODE_CONFIGURE_OMCI_PORT_ID ,
                    indication_message.message_data.user_indication);
                break;

            case PON_INDICATION_MODIFY_ENCRYPTION_PORT_ID_FILTER :
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                    "Enable / Disable Encryption port id filter indication arrived");
                /* Call to user callback with modify encryption port id filter indication */
                p_gpon_events_callback(PON_API_MESSAGE_OPCODE_ENCRYPT_PORT_ID ,
                    indication_message.message_data.user_indication);
                break;

            case PON_INDICATION_AES_KEY_SWITCHING_TIME_MESSAGE:
                p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                    "AES message indication arrived");
                /* Call to user callback withkey switching time indication indication */
                p_gpon_events_callback(PON_API_MESSAGE_OPCODE_AES_KEY_SWITCHING_TIME ,
                    indication_message.message_data.user_indication);
                break;

            case PON_INDICATION_AES_ERROR:
                 p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                     "AES error indication arrived");
                 /* Call to user callback with error indication indication */
                 p_gpon_events_callback(PON_API_MESSAGE_OPCODE_AES_ERROR ,
                     indication_message.message_data.user_indication);
                 break;
            case PON_INDICATION_ERR :
                 p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                     "ERR error indication arrived");
                 /* Call to user callback with error indication indication */
                 p_gpon_events_callback(PON_API_MESSAGE_OPCODE_BIP_ERROR ,
                     indication_message.message_data.user_indication);
                 break;
            case PON_INDICATION_ROGUE_ONU :
                 p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                     "ROGUE_ONU error indication arrived");
                 /* Call to user callback with error indication indication */
                 p_gpon_events_callback(PON_API_MESSAGE_OPCODE_ROGUE_ONU ,
                     indication_message.message_data.user_indication);
                 break;
            case PON_INDICATION_PMD_ALARM:
                 p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                     "PMD ESC error indication arrived");
                 /* Call to user callback with error indication indication */
                 p_gpon_events_callback(PON_API_MESSAGE_OPCODE_PMD_ALARM ,
                     indication_message.message_data.user_indication);
                 break;
            default :
                p_log(ge_onu_logger.sections.stack.pon_task.unknown_inter_task_messages_id, 
                     "Unknown indication arrived %d", indication_message.message_data.user_indication.indication_id);
                break;
            }

            break;

        case PON_EVENT_BER_TIMEOUT:
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                "BER timeout indication arrived");
            p_handle_bip8_timeout_message(&indication_message);
            break;

        case PON_EVENT_PEE_TIMEOUT:
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                "PEE timeout indication arrived");
            p_handle_pee_timeout_message(&indication_message);
            break;

        case PON_EVENT_PST_PLOAM:
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                "PST ploam indication arrived");
            p_handle_pst_message(&indication_message);
            break;

        case PON_EVENT_VENDOR_SPECIFIC:
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                "Vendor specific indication arrived");
            p_handle_vendor_specific(&indication_message);
            break;

        case PON_EVENT_CTRL_BER_TIMER:
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                "BER Interval indication arrived");
            p_handle_ber_control_message(&indication_message);
            break;

        case PON_EVENT_CTRL_PEE_TIMER:
            p_log(ge_onu_logger.sections.stack.pon_task.inter_task_messages_id, 
                "PEE timer expired indication arrived");
            p_handle_pee_control_message(&indication_message);
            break;

        default:
            p_log(ge_onu_logger.sections.stack.pon_task.unknown_inter_task_messages_id, "Unknown pon message.(%d )", message_opcode);
            break;
        }
    }

    return 0;
}

void change_pmd_tracking_state(uint32_t old_state, uint32_t new_state)
{
    if (gs_gpon_database.OpticsType == BP_GPON_OPTICS_TYPE_PMD)
    {
        pmd_dev_change_tracking_state(old_state, new_state);
        p_log(ge_onu_logger.sections.stack.pon_task.unknown_inter_task_messages_id, 
            "PMD move tracking from %d state to %d state", old_state, new_state);
    }
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   f_gpon_send_message_to_pon_task                                          */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Send message to pon handler .                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   Send a message to the pon handler                                        */
/*                                                                            */
/* Input:                                                                     */
/*   PON_EVENTS_DTE- message_opcode                                           */
/*   parameters according to the opcode                                       */
/*                                                                            */
/* Output:                                                                    */
/*   PON_ERROR_DTE - Success or error returned code                           */
/*     PON_NO_ERROR - No errors or faults                                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE f_gpon_send_message_to_pon_task(PON_EVENTS_DTE xi_msg_opcode, ...)
{
    bdmf_error_t bdmf_error;
    va_list parameters;
    GPON_MESSAGE_DTE pon_message;

    /* Init the va database */
    va_start(parameters, xi_msg_opcode);

    /* For all messages - set opcode */
    pon_message.message_id = xi_msg_opcode;

    switch (xi_msg_opcode)
    {
    case PON_EVENT_CHANGE_POWER_LEVEL:
        pon_message.message_data.power_level = va_arg(parameters, GPON_POWER_LEVEL_PARAMETERS_DTE);
        break;

    case PON_EVENT_ASSIGN_CALLBACKS:
        pon_message.message_data.callback_routines = va_arg(parameters, GPON_CALLBACKS_DTE);
        break;

    case PON_EVENT_QUEUE_EVENT:
        pon_message.message_data.fifo_block = va_arg(parameters, PON_FIFO_ID_DTE);
        break;

    case PON_EVENT_KEEPALIVE_TIMEOUT:
        pon_message.message_data.keepalive_timeout = va_arg(parameters, uint32_t);
        break;

    case PON_EVENT_STATE_TRANSITION:
        pon_message.message_data.user_indication.indication_data.onu_state_transition.onu_old_sub_state = va_arg(parameters, LINK_SUB_STATE_DTE);
        pon_message.message_data.user_indication.indication_data.onu_state_transition.onu_new_sub_state = va_arg(parameters, LINK_SUB_STATE_DTE);
        break;

    case PON_EVENT_LINK_STATE_TRANSITION:
        pon_message.message_data.user_indication.indication_data.onu_link_state_transition.onu_old_link_state = va_arg(parameters, LINK_OPERATION_STATES_DTE);
        pon_message.message_data.user_indication.indication_data.onu_link_state_transition.onu_new_link_state = va_arg(parameters, LINK_OPERATION_STATES_DTE);
        change_pmd_tracking_state(
            pon_message.message_data.user_indication.indication_data.onu_link_state_transition.onu_old_link_state, 
            pon_message.message_data.user_indication.indication_data.onu_link_state_transition.onu_new_link_state);
        break;

    case PON_EVENT_OAM:
        pon_message.message_data.oam.oam_alarm_id = va_arg(parameters, GPON_OAM_ALARM_ID_DTE);
        pon_message.message_data.oam.oam_alarm_status = (bool)va_arg(parameters, int);

        switch (pon_message.message_data.oam.oam_alarm_id)
        {
            case GPON_INDICATION_MEM:
                pon_message.message_data.user_indication.indication_data.gpon_mem.unknown_msg = 
                    va_arg(parameters, GPON_DS_PLOAM);
                break;
        }
        break;

    case PON_EVENT_USER_INDICATION:
        pon_message.message_data.user_indication.indication_id = va_arg(parameters, PON_INDICATIONS_DTE);
        switch(pon_message.message_data.user_indication.indication_id)
        {
        case PON_INDICATION_RANGING_START:
            break;

        case PON_INDICATION_RANGING_STOP:
            pon_message.message_data.user_indication.indication_data.ranging_stop.ranging_stop_reason = va_arg(parameters, PON_RANGING_STOP_REASONS_INDICATION);
            break;

        case PON_INDICATION_ASSIGN_ALLOC_ID:
            pon_message.message_data.user_indication.indication_data.assign_alloc_id = va_arg(parameters, PON_ASSIGN_ALLOC_ID_INDICATION);
            break;

        case PON_INDICATION_ASSIGN_ONU_ID:
            pon_message.message_data.user_indication.indication_data.assign_onu_id = va_arg(parameters, PON_ASSIGN_ONU_ID_INDICATION);
            break;

        case PON_INDICATION_CONFIGURE_OMCI_PORT_ID_FILTER:
             pon_message.message_data.user_indication.indication_data.configure_omci_port = va_arg(parameters, PON_CONFIGURE_OMCI_PORT_ID_INDICATION);
             break;

        case PON_INDICATION_MODIFY_ENCRYPTION_PORT_ID_FILTER:
            pon_message.message_data.user_indication.indication_data.configure_port = va_arg(parameters, PON_CONFIGURE_FILTER_INDICATION);
            break;

        case PON_INDICATION_AES_ERROR:
        case PON_INDICATION_AES_KEY_SWITCHING_TIME_MESSAGE:
            pon_message.message_data.user_indication.indication_data.aes = va_arg(parameters, PON_AES_INDICATION);
            break;
        case PON_INDICATION_ERR:
            pon_message.message_data.user_indication.indication_data.bip_error = va_arg(parameters, PON_ERROR_INDICATION);
            break;
        case PON_INDICATION_ROGUE_ONU:
            pon_message.message_data.user_indication.indication_data.rogue_onu = va_arg(parameters, PON_ROGUE_ONU_INDICATION);
            break;
        case PON_INDICATION_PMD_ALARM:
            pon_message.message_data.user_indication.indication_data.pmd_alarm = va_arg(parameters, PMD_ALARM_INDICATION_PARAMETERS_DTE);
            break;
        }

        break;

    case PON_EVENT_CRITICAL_BER_TIMEOUT:
        break;

    case PON_EVENT_BER_TIMEOUT:
        break;

    case PON_EVENT_PEE_TIMEOUT:
        break;

    case PON_EVENT_CTRL_BER_TIMER:
        pon_message.message_data.timer_control.interval = va_arg(parameters, uint32_t);
        pon_message.message_data.timer_control.command = (bool)va_arg(parameters, int);
        break;

    case PON_EVENT_CTRL_PEE_TIMER:
        break;

    case PON_EVENT_PST_PLOAM:
        break;

    default:
        return PON_ERROR_INVALID_PARAMETER;
        break;
    }

    /* Since the parameter/buffer we send is not the message itself but a pointer to the */
    /* message, we have to send the pointer's address (& message_ptr ) */

    /* Send the message */
    bdmf_error = bdmf_queue_send(&gs_gpon_database.os_resources.pon_messages_queue_id,
        (char *)(&pon_message), sizeof(GPON_MESSAGE_DTE));
    if (bdmf_error != BDMF_ERR_OK)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Send PON message failed due to bdmf_error_t: %d, msg_opcode=%d ",
            bdmf_error, xi_msg_opcode);
        return PON_ERROR_QUEUE_SEND;
    }

    va_end(parameters);
    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_gpon_events_alarms_callback                                            */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Default Events and alarms user callback function                         */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function is called when a system event or alarm is turned on in     */
/*   a specific device                                                        */
/*                                                                            */
/* Input:                                                                     */
/*      - xi_event_opcode : An event opcode / id                              */
/* Output:                                                                    */
/*                                                                            */
/*   None                                                                      */
/*                                                                            */
/******************************************************************************/
static void p_default_event_and_alarm_callback(PON_API_MESSAGE_OPCODES_DTE xi_message_id,
    PON_USER_INDICATION_DTE xi_user_indication_message)
{
    /* empty function - all indications are through configuration handler only */
    return;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_gpon_dbr_runner_data_callback                                          */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Default DBR runner data callback function                                */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function is called when a system event or alarm is turned on in     */
/*   a specific device                                                        */
/*                                                                            */
/* Input:                                                                     */
/*       xi_runner_queue_id                                                   */
/*       xo_runner_queue_size_struct : Runner struct                          */
/* Output:                                                                    */
/*                                                                            */
/*   None                                                                      */
/*                                                                            */
/******************************************************************************/
static void p_gpon_dbr_runner_data_callback(uint32_t xi_runner_queue_id,
    uint32_t *xo_runner_queue_size)
{
    if (xo_runner_queue_size != NULL )
    {
        *xo_runner_queue_size = 0;
    }
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_gpon_sc_sc_runner_data_callback                                        */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   SC SC runner data callback function                                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function is called to get SC SC content                             */
/*                                                                            */
/* Input:                                                                     */
/*       xi_buffer                                                            */
/*       xi_flag                                                              */
/* Output:                                                                    */
/*                                                                            */
/*   None                                                                      */
/*                                                                            */
/******************************************************************************/
static void p_gpon_sc_sc_runner_data_callback(uint8_t *xo_buffer, uint8_t *xo_flag)
{
    /* empty function - all indications are through configuration handler only */
    return;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_gpon_flush_tcont_callback                                              */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Flush default callback function                                          */
/*                                                                            */
/******************************************************************************/
static int p_gpon_flush_tcont_callback(uint8_t tcont_id)
{
    return 0;
}

