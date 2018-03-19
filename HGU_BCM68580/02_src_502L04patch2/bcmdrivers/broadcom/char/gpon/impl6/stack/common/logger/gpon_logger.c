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


/******************************************************************************/
/*                                                                            */
/* SW unit description:                                                       */
/*                                                                            */
/* This software unit implements the Lilac logger.                            */
/*                                                                            */
/******************************************************************************/

#ifdef USE_LOGGER

/* Infrastructure */
#include "bdmf_system.h"

/* Logger */
#include "gpon_logger.h"
#include "gpon_ploam.h"

/* Logger global */
LOGGER ge_onu_logger;
LOGGER_LOG_ID log_id;

static void bdmf_task_current_get_name(char *name)
{
    bdmf_task_get_name(bdmf_task_get_current(), name);
}

void gpon_logger_init(char *xi_base_folder)
{
    LOGGER_ERROR logger_error ;
    LOGGER_CALLBACKS callbacks =
    {
	.log_alloc = (void *(*)(long size))bdmf_alloc,
	.log_free = bdmf_free,
	.log_queue_create = (long (*)(bdmf_queue_t *, long, long))bdmf_queue_create,
        .log_queue_send = (long (*)(bdmf_queue_t *, char *, long))bdmf_queue_send,
	.log_queue_receive_not_blocking = (long (*)(bdmf_queue_t *, char *, long *))bdmf_queue_receive,
        .log_get_task_name = bdmf_task_current_get_name,
    };
    
    logger_error = fi_linux_logger_init("GPON", "/", 60, 0x4000, 1500, LOGGER_TIME_BASE_MICROSEC, & ge_onu_logger.log_id, &callbacks, 0) ;
    if(logger_error != LOGGER_OK ) 
    {
        bdmf_print("Error initializing logger (%d)\n", logger_error ) ;
        return ;
    }


    /* Set the stack section */
    logger_set_section(ge_onu_logger.log_id, "Stack", " [Stack,", BDMF_TRUE, & ge_onu_logger.sections.stack_id ) ;    
    logger_set_group(ge_onu_logger.sections.stack_id, "Operating", "Operating,", BDMF_TRUE, & ge_onu_logger.sections.stack.operating_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.operating_id, "Invoke_state_machine",        "Invoke state machine]",        BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_BOLD,   LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.operating.invoke_state_machine_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.operating_id, "State_change",                "State change]",                BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_BOLD,   LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.operating.state_change_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.operating_id, "Invalid_cross",               "Invalid cross]",               BDMF_FALSE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_BOLD,   LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.operating.invalid_cross ) ;
    logger_set_item(ge_onu_logger.sections.stack.operating_id, "General",                     "General]",                     BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.operating.general_id ) ;
    
    logger_set_group(ge_onu_logger.sections.stack_id, "Pon_task", "Pon task,", BDMF_TRUE, & ge_onu_logger.sections.stack.pon_task_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.pon_task_id, "Intertask_messages",         "Inter-task messages]",         BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.pon_task.inter_task_messages_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.pon_task_id, "Unknown_intertask_messages", "Unknown inter-task messages]", BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.pon_task.unknown_inter_task_messages_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.pon_task_id, "General", "General]", BDMF_FALSE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.pon_task.general_id ) ;
    
    logger_set_group(ge_onu_logger.sections.stack_id, "Downstream_PLOAM", "Downstream PLOAM, ", BDMF_TRUE, & ge_onu_logger.sections.stack.downstream_ploam_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "General",                     "General]",                     BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.general_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "NO_MESSAGE",                  "NO_MESSAGE]",                  BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.no_message_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "US_OVERHEAD",                 "US_OVERHEAD]",                 BDMF_FALSE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.upstream_overhead_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "SERIAL_NUMBER_MASK",          "SERIAL_NUMBER_MASK]",          BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.serial_number_mask_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "ASSIGN_ONU_ID",               "ASSIGN_ONU_ID]",               BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.assign_onu_id_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "RANGING_TIME",                "RANGING_TIME]",                BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "DEACTIVATE_ONU_ID",           "DEACTIVATE_ONU_ID]",           BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.deactivate_onu_id_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "DISABLE_SERIAL_NUMBER",       "DISABLE_SERIAL_NUMBER]",       BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.disable_serial_number_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "CFG_VP_VC",                   "CFG_VP_VC]",                   BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.configure_vp_vc_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "ENCRYPTED_VPI_PORT",          "ENCRYPTED_VPI_PORT]",          BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.encrypted_vpi_port_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "REQUEST_PASSWORD",            "REQUEST_PASSWORD]",            BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.request_password_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "ASSIGN_ALLOC_ID",             "ASSIGN_ALLOC_ID]",             BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.assign_alloc_id_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "POPUP",                       "POPUP]",                       BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.popup_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "REQUEST_KEY",                 "REQUEST_KEY]",                 BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.request_key_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "CFG_PORT_ID",                 "CFG_PORT_ID]",                 BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.configure_port_id_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "PEE",                         "PEE]",                         BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.pee_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "CHANGE_POWER_LEVEL",          "CHANGE_POWER_LEVEL]",          BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.cpl_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "PST",                         "PST]",                         BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.pst_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "BER_INTERVAL",                "BER_INTERVAL]",                BDMF_FALSE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.ber_interval_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "KEY_SWITCHING_TIME",          "KEY_SWITCHING_TIME]",          BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.key_switching_time_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "EXTENDED_BURST",              "EXTENDED_BURST]",              BDMF_FALSE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.extended_burst_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "SLEEP_ALLOW",                 "SLEEP_ALLOW]",                 BDMF_FALSE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.sleep_allow_message_id ) ; //__PwM
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "PON_ID",                      "PON_ID]",                      BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.pon_id_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "SWIFT_POPUP",                 "SWIFT_POPUP]",                 BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.swift_popup_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "RANGING_ADJUSTMENT",          "RANGING_ADJUSTMENT]",          BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.ranging_adjustment_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "KEY_CONTROL",                 "KEY_CONTROL]",                 BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "BURST_PROFILE",               "BURST_PROFILE]",               BDMF_FALSE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "REGISTRATION_REQUEST",        "REGISTRATION_REQUEST]",                 BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.request_registration_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "CALIBRATION_REQUEST",         "CALIBRATION_REQUEST]",         BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.ranging_adjustment_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "ADJUST_TX_WAVELENGTH",        "ADJUST_TX_WAVELENGTH]",        BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.ranging_adjustment_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "TUNING_CONTROL",              "TUNING_CONTROL]",              BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "SYSTEM_PROFILE",              "SYSTEM_PROFILE]",              BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.system_profile_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "CHANNEL_PROFILE",             "CHANNEL_PROFILE]",             BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.channel_profile_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "PROTECTION_CONTROL",          "PROTECTION_CONTROL]",          BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.protection_control_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "CHANGE_POWER_LEVEL",          "CHANGE_POWER_LEVEL]",          BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.change_power_level_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "POWER_CONSUMPTION_INQUIRE",   "POWER_CONSUMPTION_INQUIRE]",   BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.power_consumption_inquire_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "RATE_CONTROL",                "RATE_CONTROL]",                BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.rate_control_message_id ); 
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "UNKNOWN",                     "UNKNOWN]",                     BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.unknown_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.downstream_ploam_id, "Print_DS_Ploam",              "Print_DS_Ploam]",              BDMF_FALSE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.downstream_ploam.print_ploam ) ;

    logger_set_group(ge_onu_logger.sections.stack_id, "Upstream_PLOAM",                  "Upstream PLOAM,",    BDMF_TRUE, & ge_onu_logger.sections.stack.upstream_ploam_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "General",           "General]",           BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.general_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "SERIAL_NUMBER_ONU", "SERIAL_NUMBER_ONU]", BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.serial_number_onu_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "PASSWORD",          "PASSWORD]",          BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.password_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "NO_MESSAGE",        "NO_MESSAGE]",        BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.no_message_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "DYING_GASP",        "DYING_GASP]",        BDMF_FALSE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.dying_gasp_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "ENCRYPTION_KEY",    "ENCRYPTION_KEY]",    BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.encryption_key_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "PEE",               "PEE]",               BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.pee_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "PST",               "PST]",               BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.pst_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "REI",               "REI]",               BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.rei_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "ACK",               "ACK]",               BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.ack_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "REGISTRATION",      "REGISTRATION]",      BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.dying_gasp_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "KEY_REPORT",        "KEY_REPORT]",        BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.key_report_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "SLEEP_REQUEST",     "SLEEP_REQUEST]",     BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.sleep_request_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "TUNING_RESPONSE",   "TUNING_RESPONSE]",   BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.tuning_response_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "POWER_CONSUMPTION", "POWER_CONSUMPTION]", BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.rei_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "RATE_RESPONSE",     "RATE_RESPONSE]",     BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.pst_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "UNKNOWN",           "UNKNOWN]",           BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.unknown_message_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.upstream_ploam_id, "Print_US_Ploam",    "Print_US_Ploam]",    BDMF_FALSE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.upstream_ploam.print_ploam ) ;

    logger_set_group(ge_onu_logger.sections.stack_id, "Debug", "Debug messages,", BDMF_TRUE, & ge_onu_logger.sections.stack.debug_messages_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.debug_messages_id, "General",                 "General]",                 BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.debug.general_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.debug_messages_id, "SW_errors",               "SW errors]",               BDMF_TRUE,  BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.debug.sw_errors_id ) ;
    logger_set_item(ge_onu_logger.sections.stack.debug_messages_id, "Timers",                  "Timers]",                  BDMF_FALSE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.stack.debug.timers_id ) ;
    
    /* Set the ISR section */
    logger_set_section(ge_onu_logger.log_id, "ISR", " [ISR,", BDMF_TRUE, & ge_onu_logger.sections.isr_id ) ;
    
    logger_set_group(ge_onu_logger.sections.isr_id, "Debug", "Debug messages,", BDMF_TRUE, & ge_onu_logger.sections.isr.debug_id ) ;
    logger_set_item(ge_onu_logger.sections.isr.debug_id, "ISR_PON_message",     "ISR-PON message]",     BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.isr.debug.isr_pon_message_id ) ;
    logger_set_item(ge_onu_logger.sections.isr.debug_id, "RX_Event",     "RX Event]",     BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.isr.debug.rx_event_id ) ;
    logger_set_item(ge_onu_logger.sections.isr.debug_id, "TX_Event",     "TX Event]",     BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.isr.debug.tx_event_id ) ;
    logger_set_item(ge_onu_logger.sections.isr.debug_id, "AES",     "AES]",     BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.isr.debug.aes_id ) ;
    logger_set_item(ge_onu_logger.sections.isr.debug_id, "ranging",     "ranging]",     BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.isr.debug.ranging_id ) ;
    logger_set_item(ge_onu_logger.sections.isr.debug_id, "general",     "general]",     BDMF_TRUE, BDMF_FALSE, BDMF_FALSE, LOGGER_ATTRIBUTE_NORMAL, LOGGER_ITEM_TYPE_FORMAT, LOGGER_TRIGGER_ACTION_NONE, 0, 0, "", & ge_onu_logger.sections.isr.debug.general_id ) ;
}


PON_ERROR_DTE gpon_get_log_object_list(char* parent_name, char* name_list)
{
    PON_ERROR_DTE rc = PON_NO_ERROR;

    rc = logger_get_object_list(log_id, parent_name, name_list);

    return rc;
}

PON_ERROR_DTE gpon_set_log_object_state(char* object_name, bool state)
{
    PON_ERROR_DTE rc = PON_NO_ERROR;

    if (state)
    {
        rc = logger_on(0, log_id, object_name, CE_CHANGE_STATE);
    }
    else
    {
        rc = logger_off(0, log_id, object_name, CE_CHANGE_STATE);
    }

    return rc;
}

LOGGER_ERROR logger_init(void)
{
    LOGGER_DEVICE_ID device_id;
    LOGGER_ERROR logger_rc;
    char context[LOGGER_NAME_LENGTH];
    int rc;

    rc = logger_drv_init();
    if (rc < 0)
        return rc;
    gpon_logger_init("/");
    log_id = (LOGGER_LOG_ID)fi_get_log_ptr("/");

    bdmf_task_get_name(bdmf_task_get_current(), context);
    logger_rc = logger_set_device(log_id, context, "shell session", 0, 0, CE_TRUE, &device_id);

    if (logger_rc != LOGGER_OK)
    {
        printk(KERN_ERR "logger initialization error %d \n", logger_rc);
        logger_drv_exit();
        return -1;
    }

    return 0;
}

void logger_exit(void)
{
    LOGGER_ERROR logger_rc;

    /* Exit logger */
    log_id = (LOGGER_LOG_ID)fi_get_log_ptr("/");
    logger_rc = logger_terminate(log_id, 0, LOGGER_TERMINATION_MODE_GRACEFUL);
    if (logger_rc != LOGGER_OK)
        printk(KERN_ERR "logger terminate error %d \n", logger_rc);
    logger_drv_exit();
}

#endif

