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
/* File Description:                                                          */
/*                                                                            */
/* This file contains the implementation of UT shell commands for the Lilac   */
/* TX PON driver.                                                             */
/*                                                                            */
/******************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Include files                                                             */
/*                                                                           */
/*****************************************************************************/
#include <bdmf_dev.h>

#include "bdmf_shell.h"
#include "ngpon_db.h"
#include "gpon_logger.h"
#include "bcmsfp_i2c.h"
#include "opticaldet.h"

#define BDMF_SESSION_DISABLE_FORMAT_CHECK

#ifdef USE_BDMF_SHELL

static char *gpon_stack_gpon_error_code_to_string(PON_ERROR_DTE error_code);
int32_t translate_string_to_binary(char *xi_string, uint8_t *xo_buffer, uint32_t xi_max_length);

/******************************************************************************/
/*                                                                            */
/*  Static functions prototypes                                               */
/*                                                                            */
/******************************************************************************/

static struct bdmfmon_enum_val activation_state_names_enum_table[] = {
  {"INIT(O1)", 0},
  {"SN(O2-3)PRE-PROFILE", 1},
  {"SN(O2-3)POST-PROFILE", 2},
  {"RANGING(O4)", 3},
  {"OPERATION(O5)", 4},
  {"INTERMITTENT_LODS(O6)", 5},
  {"EMERGENCY_STOP(O7)", 6},
  {"DS_TUNING(O8)", 7},
  {"US_TUNING(O9)", 8}
};

static struct bdmfmon_enum_val link_sub_state_names_enum_table[] = {
    {"NONE", 0},
    {"STANDBY", 1},
    {"OPERATION", 2},
    {"STOP", 3},
    {"O1_NO_SYNC", 4},
    {"O1_PROFILE_LEARN", 5},
    {"O5_ASSOCIATED", 6},
    {"O6_PENDING", 7},
    {"O8_NO_SYNC", 8},
    {"O8_PROFILE_LEARN", 9}     
};

static struct bdmfmon_enum_val link_state_names_enum_table[] = {
    {"READY", 0},
    {"INACTIVE", 1},
    {"ACTIVE", 2}
};

/*static char *gs_link_state_names[LINK_SUB_STATE_STOP + 1] =
{
    "READY",
    "INACTIVE",
    "ACTIVE",
};

static char *gs_link_sub_state_names[LINK_SUB_STATE_STOP + 1] =
{
    "NONE",
    "STANDBY",
    "OPERATION",
    "STOP"
};

static char *gs_activation_state_names[10] =
{
    "INIT(O1)",
    "SN(O2-3)PRE-PROFILE",
    "SN(O2-3)POST-PROFILE",
    "RANGING(O4)",
    "OPERATION(O5)",
    "INTERMITTENT_LODS(O6)",
    "EMERGENCY_STOP(O7)",
    "DS_TUNING(O8)",
    "US_TUNING(O9)"
};
*/
static int ngpon_software_initialize_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        PON_ERROR_DTE error ;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_software_initialize_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_software_init();
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_software_terminate_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        PON_ERROR_DTE error ;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_software_terminate_command' -- not implemented yet\n\r" );
        error = 0;
        /* -------------------------------------------------------------------- */
        /* error = ngpon_software_terminate (); */
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_configure_onu_serial_number_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_SERIAL_NUMBER_DTE serial_number ; /* input */
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error ;
        uint32_t i ;

        /* Define parameters handling variables */
        for (i = 0; i < VENDOR_CODE_FIELD_SIZE; ++i)
        {
                serial_number.vendor_code[i] = parm[parameter_index++].value.unumber;
        }
        for (i = 0; i < VENDOR_CODE_FIELD_SIZE; ++i)
        {
                serial_number.serial_number[i] = parm[parameter_index++].value.unumber;
        }

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_configure_onu_serial_number_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_configure_onu_serial_number ( serial_number );
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_configure_onu_password_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_PASSWORD_DTE  onu_password ;
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error ;
        uint32_t i ;

        /* Extract password (36 bytes) */
        i = translate_string_to_binary ( parm[parameter_index++].value.string, ( char * ) & onu_password, PON_PASSWORD_FIELD_SIZE );
        if ( i != PON_PASSWORD_FIELD_SIZE )
        {
                bdmf_session_print(session, "password should contain %d hex bytes (%d)\n\r", PON_PASSWORD_FIELD_SIZE, i );
                return BDMF_ERR_PARM;
        }

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_configure_onu_password_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_configure_onu_password ( onu_password );
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_link_reset_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        PON_ERROR_DTE error ;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_link_reset_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_link_reset ( );
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_link_activate_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        bool initial_state_disable = BDMF_FALSE ;
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error ;

        /* Define parameters handling variables */
        initial_state_disable = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_link_activate_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_link_activate ( initial_state_disable );
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_link_deactivate_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        PON_ERROR_DTE error ;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_link_deactivate_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_link_deactivate ( );
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_link_parameters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        rdpa_gpon_link_cfg_t user_link_parameters ; /* output */
        PON_ERROR_DTE error ;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_get_link_parameters_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_get_link_parameters ( & user_link_parameters );
        /* -------------------------------------------------------------------- */
        if ( error == PON_NO_ERROR )
        {
                bdmf_session_print(session, "transceiver_dv_setup_pattern           %d \n", user_link_parameters.transceiver_dv_setup_pattern );
                bdmf_session_print(session, "transceiver_dv_hold_pattern            %d \n", user_link_parameters.transceiver_dv_hold_pattern );
                bdmf_session_print(session, "transceiver_dv_polarity                %d \n", user_link_parameters.transceiver_dv_polarity );
                bdmf_session_print(session, "transceiver_power_calibration_mode     %s \n", ( user_link_parameters.transceiver_power_calibration_mode == BDMF_TRUE ? "enable" : "disable" ) );
                bdmf_session_print(session, "ber_interval                           %d\n", user_link_parameters.ber_interval );
                bdmf_session_print(session, "min_response_time                      %d\n", user_link_parameters.min_response_time );
                bdmf_session_print(session, "to1_timeout                            %d\n", user_link_parameters.to1_timeout );
                bdmf_session_print(session, "to2_timeout                            %d\n", user_link_parameters.to2_timeout );
                bdmf_session_print(session, "tx_data_polarity                       %s\n", ( user_link_parameters.tx_data_polarity == BDMF_TRUE ? "enable" : "disable" ) );
                bdmf_session_print(session, "rx_data_polarity                       %s\n", ( user_link_parameters.rx_din_polarity == BDMF_TRUE ? "enable" : "disable" ) );
                bdmf_session_print(session, "sd_threshold                           %d\n", user_link_parameters.ber_threshold_for_sd_assertion );
                bdmf_session_print(session, "sf_threshold                           %d\n", user_link_parameters.ber_threshold_for_sf_assertion );
                bdmf_session_print(session, "number_of_psyncs_for_lof_assertion     %d\n", user_link_parameters.number_of_psyncs_for_lof_assertion );
                bdmf_session_print(session, "number_of_psyncs_for_lof_clear         %d\n", user_link_parameters.number_of_psyncs_for_lof_clear );
        }

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_link_state_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    LINK_STATE_DTE state; /* output */
    LINK_SUB_STATE_DTE sub_state; /* output */
    LINK_OPERATION_STATES_DTE operational_state; /* output */
    PON_ERROR_DTE error;
    
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'ngpon_get_link_state_command'\n\r" );
    /* -------------------------------------------------------------------- */
    error = ngpon_get_link_status(&state, &sub_state, &operational_state);
    /* -------------------------------------------------------------------- */
    if (error == PON_NO_ERROR)
    {
        bdmf_session_print(session, "  State:                   %s\n", bdmfmon_enum_stringval(link_state_names_enum_table, state));
        bdmf_session_print(session, "  Sub-tate:                %s\n", bdmfmon_enum_stringval(link_sub_state_names_enum_table, sub_state));
        bdmf_session_print(session, "  Operation state machine: %s\n", bdmfmon_enum_stringval(activation_state_names_enum_table, operational_state));
    }
    
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string(error));
    
    return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_get_software_state_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        NGPON_SOFTWARE_STATE state ; /* output */
        PON_ERROR_DTE error = PON_NO_ERROR;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_get_software_state_command'\n\r" );
        /* -------------------------------------------------------------------- */
        /* error = ngpon_get_software_status ( & state );                       */
        /* -------------------------------------------------------------------- */
        state = BDMF_FALSE;
        if ( error == PON_NO_ERROR )
                bdmf_session_print(session, "  State: %s\n", (state == BDMF_FALSE ? "INITIALIZED" : "TERMINATED"));


        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_set_txfifo_queue_configuration_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        NGPON_TX_FIFO_CFG  tx_fifo_cfg;
        uint8_t parameter_index = 0;
        PON_ERROR_DTE error ;

        /* Define parameters handling variables */
        tx_fifo_cfg.packet_descriptor_id = parm[parameter_index++].value.unumber;
        tx_fifo_cfg.txfifo_id = parm[parameter_index++].value.unumber;
        tx_fifo_cfg.packet_descriptor_queue_base_address = parm[parameter_index++].value.unumber;
        tx_fifo_cfg.packet_descriptor_queue_size = parm[parameter_index++].value.unumber;
        tx_fifo_cfg.tx_queue_offset = parm[parameter_index++].value.unumber;
        tx_fifo_cfg.tx_max_queue_size = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_set_txfifo_queue_configuration_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_set_txfifo_queue_configuration (&tx_fifo_cfg);
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_txfifo_queue_configuration_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_FIFO_ID_DTE fifo_id; /* input */
        NGPON_TX_FIFO_CFG  tx_fifo_cfg;
        uint8_t parameter_index = 0;
        PON_ERROR_DTE error = PON_NO_ERROR;

        /* Define parameters handling variables */
        fifo_id = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_get_txfifo_queue_configuration_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_get_txfifo_queue_configuration( fifo_id, fifo_id, &tx_fifo_cfg);
        /* -------------------------------------------------------------------- */
        if ( error == PON_NO_ERROR )
        {
                bdmf_session_print(session, "packet_descriptor_id                 [%d]\n\r", fifo_id );
                bdmf_session_print(session, "fifo_id                              [%d]\n\r", fifo_id );
                bdmf_session_print(session, "packet_descriptor_queue_base_address [%d]\n\r", tx_fifo_cfg.packet_descriptor_queue_base_address );
                bdmf_session_print(session, "packet_descriptor_queue_size         [%d]\n\r", tx_fifo_cfg.packet_descriptor_queue_size );
                bdmf_session_print(session, "tx_queue_offset                      [%d]\n\r", tx_fifo_cfg.tx_queue_offset );
                bdmf_session_print(session, "tx_queue_size                        [%d]\n\r", tx_fifo_cfg.tx_max_queue_size );
        }

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_txfifo_queue_configuration_group_0_to_3_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        int  group_id; /* Input  */
#if 0
        uint16_t packet_descriptor_group_base_address ; /* Output */
        uint16_t group_offset ; /* Output */
        uint16_t group_size ; /* Output */
#endif
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error = PON_NO_ERROR;

        /* Define parameters handling variables */
        group_id = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_get_txfifo_queue_configuration_group_0_to_3_command -- not implemented yet'\n\r" );
        /* -------------------------------------------------------------------- */
        /* error = gpon_get_txfifo_queue_configuration_group_0_to_3( group_id,
           & packet_descriptor_group_base_address, & group_offset, & group_size ); */
        /* -------------------------------------------------------------------- */
        if ( error == PON_NO_ERROR )
        {
          /*
                bdmf_session_print(session, "group_id -  %d \n\r", group_id );
                bdmf_session_print(session, "packet_descriptor_group_base_address -  %d \n\r",
                                packet_descriptor_group_base_address );
                bdmf_session_print(session, "group_offset -  %d \n\r", group_offset );
                bdmf_session_print(session, "group_size -  %d \n\r", group_size );
          */
        }

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_rxpon_pm_counters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_RXPON_PM_COUNTERS rxpon_pm_counters;
        PON_ERROR_DTE error ;

        /* Call the under test routine */
        memset(&rxpon_pm_counters, 0, sizeof(rxpon_pm_counters));
        bdmf_session_print(session, "UT: Invoking function 'ngpon_get_rxpon_pm_counters_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_get_rxpon_pm_counters(&rxpon_pm_counters);

        /* -------------------------------------------------------------------- */
        if ( error == PON_NO_ERROR )
        {
                bdmf_session_print(session,  
                  "sync_lof_counter                          %d\n"
                  "bwmap_hec_err_counter                     %d\n"
                  "bwmap_invalid_burst_series_counter        %d\n" 
                  "hlend_hec_err_counter                     %d\n"
                  "del_lcdg_counter                          %d\n"
                  "sync_ponid_hec_err_counter                %d\n"
                  "del_pass_pkt_counter                      %d\n"
                  "bwmap_correct_counter                     %d\n"
                  "bwmap_bursts_counter                      %d\n"
                  "xhp_pass_pkt_counter                      %d\n",

                  rxpon_pm_counters.sync_lof_counter,
                  rxpon_pm_counters.bwmap_hec_err_counter,
                  rxpon_pm_counters.bwmap_invalid_burst_series_counter,
                  rxpon_pm_counters.hlend_hec_err_counter,
                  rxpon_pm_counters.del_lcdg_counter,
                  rxpon_pm_counters.sync_ponid_hec_err_counter,
                  rxpon_pm_counters.del_pass_pkt_counter,
                  rxpon_pm_counters.bwmap_correct_counter,
                  rxpon_pm_counters.bwmap_bursts_counter,
                  rxpon_pm_counters.xhp_pass_pkt_counter);

                bdmf_session_print(session,  
                  "dec_cant_allocate_counter                 %d\n"
                  "dec_invalid_key_idx_counter               %d\n"
                  "plp_valid_onuid_counter                   %d %d %d\n"
                  "plp_valid_bcst_counter                    %d\n"
                  "mic_err_counter                           %d\n"
                  "bwmap_hec_fix_counter                     %d\n"
                  "hlend_hec_fix_counter                     %d\n"
                  "xhp_hec_fix_counter                       %d\n"
                  "sync_sfc_hec_fix_counter                  %d\n"
                  "sync_ponid_hec_fix_counter                %d\n"
                  "xgem_overrun_counter                      %d\n"
                  "bwmap_discard_dis_tx_counter              %d\n"
                  "fec_bit_err_counter                       %d\n"
                  "fec_sym_err_counter                       %d\n"
                  "fec_cw_err_counter                        %d\n"
                  "fec_uc_cw_counter                         %d\n"
                  "bwmap_num_alloc_tcont_counter             %d %d %d\n"
                  "bwmap_tot_bw_tcont_counter                %d %d %d\n"
                  "fs_bip_err_counter                        %d\n",
 
                  rxpon_pm_counters.dec_cant_allocate_counter,
                  rxpon_pm_counters.dec_invalid_key_idx_counter,
                  rxpon_pm_counters.plp_valid_onuid_counter[0], rxpon_pm_counters.plp_valid_onuid_counter[1], rxpon_pm_counters.plp_valid_onuid_counter[2],
                  rxpon_pm_counters.plp_valid_bcst_counter,
                  rxpon_pm_counters.mic_err_counter,
                  rxpon_pm_counters.bwmap_hec_fix_counter,
                  rxpon_pm_counters.hlend_hec_fix_counter,
                  rxpon_pm_counters.xhp_hec_fix_counter,
                  rxpon_pm_counters.sync_sfc_hec_fix_counter,
                  rxpon_pm_counters.sync_ponid_hec_fix_counter,
                  rxpon_pm_counters.xgem_overrun_counter,
                  rxpon_pm_counters.bwmap_discard_dis_tx_counter,
                  rxpon_pm_counters.fec_bit_err_counter,
                  rxpon_pm_counters.fec_sym_err_counter,
                  rxpon_pm_counters.fec_cw_err_counter,
                  rxpon_pm_counters.fec_uc_cw_counter,
                  rxpon_pm_counters.bwmap_num_alloc_tcont_counter[0], rxpon_pm_counters.bwmap_num_alloc_tcont_counter[2], rxpon_pm_counters.bwmap_num_alloc_tcont_counter[2],
                  rxpon_pm_counters.bwmap_tot_bw_tcont_counter[0], rxpon_pm_counters.bwmap_tot_bw_tcont_counter[1], rxpon_pm_counters.bwmap_tot_bw_tcont_counter[2],
                  rxpon_pm_counters.fs_bip_err_counter);

        }

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_rxpon_ploam_counters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_RX_PLOAM_COUNTERS_DTE rxpon_ploam_counters;
        PON_ERROR_DTE error ;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_rxpon_ploam_counters_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error  = ngpon_get_rxpon_ploam_counters ( & rxpon_ploam_counters );
        /* -------------------------------------------------------------------- */
        if ( error == PON_NO_ERROR )
        {
                bdmf_session_print(session, "rxpon_pm_counters:\n"
                                "valid_onu_id_ploam_counter    %d\n"
                                "valid_broadcast_ploam_counter  %d\n",
                                rxpon_ploam_counters.valid_onu_id_ploam_counter,
                                rxpon_ploam_counters.valid_broadcast_ploam_counter);
        }

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}


static int ngpon_set_rxpon_bw_alloc_association_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_ALLOC_ID_DTE alloc_1 ;
        PON_ALLOC_ID_DTE alloc_2 ;
        PON_ALLOC_ID_DTE alloc_3 ;
        PON_ALLOC_ID_DTE alloc_4 ;
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error = 0;

        /* Define parameters handling variables */
        alloc_1 = parm[parameter_index++].value.unumber;
        alloc_2 = parm[parameter_index++].value.unumber;
        alloc_3 = parm[parameter_index++].value.unumber;
        alloc_4 = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_set_rxpon_bw_alloc_association_command -- not implemented yet'\n\r" );
        /* -------------------------------------------------------------------- */
        /* error  = gpon_set_rxpon_bw_alloc_association ( alloc_1, alloc_2, alloc_3, alloc_4 ); */
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_rxpon_bw_alloc_association_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
  /*
        PON_ALLOC_ID_DTE alloc_1 ;
    PON_ALLOC_ID_DTE alloc_2 ;
    PON_ALLOC_ID_DTE alloc_3 ;
    PON_ALLOC_ID_DTE alloc_4 ;
  */
        PON_ERROR_DTE error = 0;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_get_rxpon_bw_alloc_association_command - not implemented yet'\n\r" );
        /* -------------------------------------------------------------------- */
        /* error = gpon_get_rx_bw_alloc_association ( &alloc_1, &alloc_2, &alloc_3, &alloc_4 ); */
        /* -------------------------------------------------------------------- */
        if ( error == PON_NO_ERROR )
        {
          /*
                bdmf_session_print(session, "RXPON BW allocation counter association  \n"
                                "Counter 1 associated to alloc  %d \n"
                                "Counter 2 associated to alloc  %d \n"
                                "Counter 3 associated to alloc  %d \n"
                                "Counter 4 associated to alloc  %d \n",
                                alloc_1,
                                alloc_2,
                                alloc_3,
                                alloc_4 );
          */
        }

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_rxpon_bw_alloc_counter_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_ALLOC_ID_DTE alloc_id ;
  /*
        uint32_t bw_allocation_counter;
  */
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error = 0;

        /* Define parameters handling variables */
        alloc_id = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_get_rxpon_bw_alloc_counter_command - not implemented yet'\n\r" );
        /* -------------------------------------------------------------------- */
        /* error = gpon_get_bw_alloc_counter ( alloc_id, &bw_allocation_counter ); */
        /* -------------------------------------------------------------------- */
        if ( error == PON_NO_ERROR )
        {
          /*
                bdmf_session_print(session, "RXPON BW allocation counter for Alloc-id %d = %d  \n",
                                alloc_id,
                                bw_allocation_counter);
          */
        }

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_tcont_fe_queue_pd_counter_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_ALLOC_ID_DTE tcont_id ;
  /*
        uint16_t pd_counter;
  */
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error = 0;

        /* Define parameters handling variables */
        tcont_id = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_get_tcont_fe_queue_pd_counter_command - not implemented yet'\n\r" );
        /* -------------------------------------------------------------------- */
        /* error = gpon_get_tcont_fe_queue_pd_counter ( tcont_id, &pd_counter ); */
        /* -------------------------------------------------------------------- */
        if ( error == PON_NO_ERROR )
        {
          /*
                bdmf_session_print(session, "PD counter for Tcont id %d Front-End = %d  \n",
                                tcont_id,
                                pd_counter);
          */
        }

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_txpon_pm_counters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    uint16_t illegal_access_counter;
    uint32_t idle_ploam_counter;
    uint32_t normal_ploam_counter;
    PON_ERROR_DTE error;
    
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'ngpon_get_txpon_pm_counters_command'\n\r" );
    /* -------------------------------------------------------------------- */
    error = ngpon_get_txpon_pm_counters ( &illegal_access_counter, &idle_ploam_counter,
                &normal_ploam_counter);
    /* -------------------------------------------------------------------- */
    if (error == PON_NO_ERROR)
    {
        bdmf_session_print(session, "illegal_access_counter          %d \n"
           "idle_ploam_counter              %d \n"
           "normal_ploam_counter            %d \n",
           illegal_access_counter,
           idle_ploam_counter,
           normal_ploam_counter);
    }
    
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );
    
    return error ? BDMF_ERR_PARM : 0;
}
static int ngpon_counter_group_for_tcont_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    uint8_t parameter_index = 0;
    PON_ERROR_DTE error;
    uint8_t       tcont_0_id;
    uint8_t       tcont_1_id;
    uint8_t       tcont_2_id;
    uint8_t       tcont_3_id;

    tcont_0_id =  parm[parameter_index++].value.unumber;
    tcont_1_id =  parm[parameter_index++].value.unumber;
    tcont_2_id =  parm[parameter_index++].value.unumber;
    tcont_3_id =  parm[parameter_index++].value.unumber;

    bdmf_session_print(session, "UT: Invoking function 'ngpon_tx_set_tcont_to_counter_group_association'\n\r" );
    /* -------------------------------------------------------------------- */
    error = ngpon_tx_set_tcont_to_counter_group_association (tcont_0_id,
       tcont_1_id, tcont_2_id, tcont_3_id) ;
    /* -------------------------------------------------------------------- */
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );
    
    return error ? BDMF_ERR_PARM : 0;    
}


static int ngpon_get_tcont_pm_counters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    PON_FIFO_ID_DTE fifo_id;
    uint32_t transmitted_idle_counter;      
    uint32_t transmitted_gem_counter;      
    uint32_t transmitted_packet_counter;    
    uint16_t requested_dbr_counter;    
    uint16_t valid_access_counter;       
    
    uint8_t parameter_index = 0;
    PON_ERROR_DTE error;
    
    /* Define parameters handling variables */
    fifo_id = parm[parameter_index++].value.unumber;
    
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_tcont_pm_counters_command'\n\r" );
    /* -------------------------------------------------------------------- */
    error = ngpon_get_tcont_queue_pm_counters ( fifo_id, & transmitted_idle_counter, & transmitted_gem_counter,
                & transmitted_packet_counter, & requested_dbr_counter, & valid_access_counter );
    /* -------------------------------------------------------------------- */
    if ( error == PON_NO_ERROR )
    {
        bdmf_session_print(session, "fifo_id                    %d\n"
            "transmitted_gem_idle_counter   %d\n"
            "transmitted_gem_counter        %d\n"
            "transmitted_packet_counter     %d\n"
            "requested_dbr_counter          %d\n"
            "valid_access_counter           %d\n",
            fifo_id,
            transmitted_idle_counter,
            transmitted_gem_counter,
            transmitted_packet_counter,
            requested_dbr_counter,
            valid_access_counter );
    }
    
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );
    
    return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_control_port_id_filter_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_FLOW_ID_DTE flow_id ;
        bool filter_status = BDMF_FALSE ;
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error ;

        /* Define parameters handling variables */
        flow_id = parm[parameter_index++].value.unumber;
        filter_status = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'p_gpon_control_port_id_filter_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_control_port_id_filter ( flow_id, filter_status );
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_configure_port_id_filter_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    PON_FLOW_ID_DTE flow_id;
    PON_PORT_ID_DTE port_id_filter;
    PON_PORT_ID_DTE port_id_mask;
    bool encryption_mode;
    PON_FLOW_TYPE_DTE flow_type;
    PON_FLOW_PRIORITY_DTE flow_priority;
    uint8_t parameter_index = 0 ;
    PON_ERROR_DTE error;
    
    /* Define parameters handling variables */
    flow_id = parm[parameter_index++].value.unumber;
    port_id_filter = parm[parameter_index++].value.unumber;
    port_id_mask = parm[parameter_index++].value.unumber;
    encryption_mode = parm[parameter_index++].value.unumber;
    flow_type = parm[parameter_index++].value.unumber;
    flow_priority = parm[parameter_index++].value.unumber;
    
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'ngpon_configure_port_id_filter_command'\n\r" );
    /* -------------------------------------------------------------------- */
    error = ngpon_configure_port_id_filter(flow_id, port_id_filter,
        port_id_mask, encryption_mode, flow_type, flow_priority, true);
    /* -------------------------------------------------------------------- */
    
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string(error));
    
    return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_modify_encryption_port_id_filter_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_FLOW_ID_DTE flow_id ;
        bool encryption_mode = BDMF_FALSE ;
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error ;

        /* Define parameters handling variables */
        flow_id = parm[parameter_index++].value.unumber;
        encryption_mode = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_modify_encryption_port_id_filter_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_modify_encryption_port_id_filter ( flow_id,
            encryption_mode );
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_port_id_filter_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_FLOW_ID_DTE flow_id ;
        bool filter_status ;
        PON_PORT_ID_DTE  port_id_filter ;
        PON_PORT_ID_DTE  port_id_mask ;
        bool encryption_mode ;
        bool flow_configured ;
        PON_FLOW_PRIORITY_DTE flow_priority ;
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error ;
        bool crc;

        /* Define parameters handling variables */
        flow_id = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_get_port_id_filter_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_get_port_id_configuration ( flow_id, & filter_status,
            & port_id_filter,& port_id_mask, & encryption_mode, & flow_configured, & flow_priority, &crc );
        /* -------------------------------------------------------------------- */
        if ( error == PON_NO_ERROR )
        {
                bdmf_session_print(session, "flow_id:         %d \n"
                                "filter_status:   %d \n"
                                "port_id_filter:  %d \n"
                                "port_id_mask:    %d \n"
                                "encryption_mode: %d \n"
                                "configured?:     %d \n"
                                "flow_priority:   %d \n",
                                flow_id,
                                filter_status,
                                port_id_filter,
                                port_id_mask,
                                encryption_mode,
                                flow_configured,
                                flow_priority );
        }

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_configure_tcont_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_TCONT_ID_DTE       tcont_queue_id ;
        PON_ALLOC_ID_DTE       alloc_id ;
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error ;

        /* Define parameters handling variables */
        tcont_queue_id = parm[parameter_index++].value.unumber;
        alloc_id = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_configure_tcont_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_configure_tcont ( tcont_queue_id, alloc_id );
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_tcont_params_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_TCONT_ID_DTE tcont_queue_id ;
        PON_TCONT_DTE tcont_cfg;
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error ;

        /* Define parameters handling variables */
        tcont_queue_id = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_get_tcont_params_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_get_tcont_configuration ( tcont_queue_id, & tcont_cfg );
        /* -------------------------------------------------------------------- */
        if ( error == PON_NO_ERROR )
        {
                bdmf_session_print(session, "Tcont id %d ,alloc_id %d, assign_alloc_valid_flag %s, \n "
                                "config_tcont_valid_flag %s tcont_queue_size %d!",
                                tcont_queue_id,
                                tcont_cfg.alloc_id,
                                tcont_cfg.assign_alloc_valid_flag == BDMF_TRUE ? "true" : "false",
                                tcont_cfg.config_tcont_valid_flag == BDMF_TRUE ? "true" : "false",
                                tcont_cfg.tcont_queue_size);
        }

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_modify_tcont_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_TCONT_ID_DTE tcont_queue_id ;
        PON_ALLOC_ID_DTE new_alloc_id ;
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error ;

        /* Define parameters handling variables */
        tcont_queue_id = parm[parameter_index++].value.unumber;
        new_alloc_id = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_modify_tcont_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_modify_tcont ( tcont_queue_id, new_alloc_id );
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_remove_tcont_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_TCONT_ID_DTE tcont_queue_id ;
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error ;

        /* Define parameters handling variables */
        tcont_queue_id = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_remove_tcont_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_remove_tcont ( tcont_queue_id );
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_print_tcont_table_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        uint32_t tcont_index ;
        PON_ERROR_DTE error = PON_NO_ERROR ;

        bdmf_session_print(session, "\n|----------------------------|" );
        bdmf_session_print(session, "\n|         TCONT TABLE        |" );
        bdmf_session_print(session, "\n|----------------------------|");
        bdmf_session_print(session, "\n|-------------------------------------------------------------|" );
        bdmf_session_print(session, "\n| Tcont  | Alloc | Assign alloc | Configure Tcont |" );
        bdmf_session_print(session, "\n|  id    |  id   |  flag        |     flag        |" );
        bdmf_session_print(session, "\n|-------------------------------------------------------------|" );

        for ( tcont_index = 0 ; tcont_index <= NGPON_TRAFFIC_TCONT_ID_HIGH ; tcont_index++ )
        {
                if ( g_xgpon_db.tconts_table [ tcont_index ].tcont_id == NGPON_DUMMY_TCONT_ID_VALUE )
                {
                        bdmf_session_print(session,  "\n|NONE    |" );
                }
                else
                {
                        bdmf_session_print(session, "\n|%-8d|", g_xgpon_db.tconts_table [ tcont_index ].tcont_id );
                }

                if ( g_xgpon_db.tconts_table [ tcont_index ].alloc_id == PON_ALLOC_ID_DTE_NOT_ASSIGNED )
                {
                        bdmf_session_print(session,  "NONE   |" );
                }
                else
                {
                        bdmf_session_print(session, "%-7d|", g_xgpon_db.tconts_table [ tcont_index ].alloc_id );
                }
                bdmf_session_print(session, "%-14d|%-17d|" ,
                                g_xgpon_db.tconts_table [ tcont_index ].assign_alloc_valid_flag ,
                                g_xgpon_db.tconts_table [ tcont_index ].config_tcont_valid_flag );
        }

        bdmf_session_print(session, "\n|-----------------------------------------------------------------------------------------------------|\n" );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_init_dba_sr_process_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_ERROR_DTE error = 0;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_init_dba_sr_process_command - not implemented yet'\n\r" );
        /* -------------------------------------------------------------------- */
        /* error = gpon_dba_sr_process_initialize ( GPON_DBR_USER_RESOLUTION, 2000 ); */
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_terminate_dba_sr_process_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_ERROR_DTE error = 0;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_terminate_dba_sr_process_command - not implemented yet'\n\r" );
        /* -------------------------------------------------------------------- */
        /*error = gpon_dba_sr_process_terminate ( ); */
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_bip8_counters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
 
        uint32_t bip_count = 0;
 
        PON_ERROR_DTE error = 0;


        /* Call the under test routine */
        bdmf_session_print(session, "Invoking function 'nread_bip8_error_counters - not implemented yet'\n" );

        /* -------------------------------------------------------------------- */
        /* p_read_bip8_error_counters ( & bip_count, CS_USER_TYPE ); */
        /* -------------------------------------------------------------------- */

        bdmf_session_print(session, "bip8 errors = %d\n", bip_count );


        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_start_bw_recording_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        bool recocd_stop ;       /* input */
        uint8_t   record_type;        /* input */
        uint32_t  alloc_id ;          /* input */
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error ;

        /* Define parameters handling variables */
        recocd_stop = parm[parameter_index++].value.unumber;
        record_type = parm[parameter_index++].value.unumber;
        alloc_id = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_start_bw_recording_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_start_bw_recording ( recocd_stop, record_type, alloc_id );
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_stop_bw_recording_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    PON_ERROR_DTE error;
        
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'ngpon_stop_bw_recording_command'\n\r" );
    /* -------------------------------------------------------------------- */
    error = ngpon_stop_bw_recording();
    /* -------------------------------------------------------------------- */
    
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string(error));
    
    return error ? BDMF_ERR_PARM : 0;
}

static    NGPON_ACCESS access_array[NGPON_NUMBER_OF_ACCESS_IN_MAP]; /* output */

static int ngpon_get_bw_recording_data_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    int access_read_num; /* output */
    uint32_t i;
    PON_ERROR_DTE error;
    
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'ngpon_get_bw_recording_data_command'\n\r");
    /* -------------------------------------------------------------------- */
    error = ngpon_get_bw_recording_data(access_array, &access_read_num);
    /* -------------------------------------------------------------------- */
    if (error == PON_NO_ERROR)
    {
        bdmf_session_print(session, "%d accesses Read. Recorded Data: \n\n", access_read_num);
        bdmf_session_print(session, "+----------------------------------------------------------------------------------------------+\n\r") ;
        bdmf_session_print(session, "| #  | Alloc ID | Start    | Grant Size| SF Cntr LSB | HEC OK | PloamU | FWI | DBRu | BPROFILE |\n\r") ;
        bdmf_session_print(session, "+----------------------------------------------------------------------------------------------+\n\r") ;
        for (i = 0; i < access_read_num; i++)
        {
           bdmf_session_print(session, "|%4d|%10d|%10d|%10d|%13d|%8d|%8d|%5d|%5d|%11d |\n\r", i,
                                access_array[i].allocid,
                                access_array[i].starttime,
                                access_array[i].grantsize,
                                access_array[i].sfc_ls,
                                access_array[i].hec_ok,
                                access_array[i].ploamu,
                                access_array[i].fwi,
                                access_array[i].dbru,
                                access_array[i].bprofile);
           bdmf_session_print(session, "+----------------------------------------------------------------------------------------------+\n\r") ;
        }
    }
    
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string(error));
    
    return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_set_rogue_onu_detection_params_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
    PON_TX_ROGUE_ONU_PARAMETERS rogue_onu_parameters; /* input */
    uint8_t parameter_index = 0 ;
    PON_ERROR_DTE error ;

    /* Define parameters handling variables */
    rogue_onu_parameters.rogue_onu_level_clear = parm[parameter_index++].value.unumber;
    rogue_onu_parameters.rogue_onu_diff_clear = parm[parameter_index++].value.unumber;
    rogue_onu_parameters.source_select = parm[parameter_index++].value.unumber;
    rogue_onu_parameters.rogue_onu_level_threshold = parm[parameter_index++].value.unumber;
    rogue_onu_parameters.rogue_onu_diff_threshold = parm[parameter_index++].value.unumber;

    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'ngpon_set_rogue_onu_detection_params_command'\n\r" );
    /* -------------------------------------------------------------------- */
    error = ngpon_set_rogue_onu_detection_params ( & rogue_onu_parameters );
    /* -------------------------------------------------------------------- */

    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

    return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_set_dbru_report_parameters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        bool  update_mode; /* input */
        uint8_t    gem_block_size; /* input */
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error = 0 ;

        /* Define parameters handling variables */
        update_mode = parm[parameter_index++].value.unumber;
        gem_block_size = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_set_dbru_report_parameters_command  - not implemented yet'\n\r" );
        /* -------------------------------------------------------------------- */
        /* error = ngpon_set_dbru_report_parameters ( update_mode, gem_block_size ); */
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_set_loopback_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        bool loopback_status ; /* input */
        int loopback_queue ; /* input */
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error = 0 ;

        /* Define parameters handling variables */
        loopback_status = parm[parameter_index++].value.unumber;
        loopback_queue = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_set_loopback_command  - not implemented yet'\n\r" );
        /* -------------------------------------------------------------------- */
        /* error = gpon_set_loopback ( loopback_status, loopback_queue ); */
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_flush_queue_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        PON_ERROR_DTE error = 0;
        uint8_t tcont_id;
        uint8_t parameter_index = 0 ;
 
        tcont_id = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_tx_set_flush_valid_bit'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_tx_set_flush_valid_bit(tcont_id, 1, 0);
        if (error == 0)
        {
             bdmf_usleep(250);
             error = ngpon_tx_set_flush_valid_bit(tcont_id, 0, 0);
        }
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_send_ploam_message_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        int repetition ; /* input */
        uint8_t priority ; /* input */
        uint8_t message_id ; /* input */
        uint8_t *ploam_buffer ; /* input */
        NGPON_US_PLOAM ploam_msg ; /* input */
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error = 0;
        uint32_t i ;

        /* Define parameters handling variables */
        repetition = parm[parameter_index++].value.unumber;
        message_id = parm[parameter_index++].value.unumber;

        /* Extract ploam data (10 bytes) */
        i = translate_string_to_binary ( parm[parameter_index++].value.string, (uint8_t *) &ploam_msg.message, PLOAM_DATA_SIZE );
        if ( i != PLOAM_DATA_SIZE )
        {
                bdmf_session_print(session, "Ploam message size ( %d ) doesn''t match actual buffer size ( %d )\n\r", PLOAM_DATA_SIZE, i );
                return BDMF_ERR_PARM;
        }

        priority = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_send_ploam_message_command  - not implemented yet'\n\r" );
        /* -------------------------------------------------------------------- */
        /* error = ngpon_send_ploam_message ( &ploam_msg, message_id, repetition, priority ); */
        /* -------------------------------------------------------------------- */

        if ( error == PON_NO_ERROR )
        {
                ploam_buffer = (uint8_t *)&ploam_msg.message;
                bdmf_session_print(session, "ploam_msg - : \n\r");
                for (i = 0; i < PLOAM_DATA_SIZE; ++i)
                        bdmf_session_print(session, "%02X ", ploam_buffer[i] );

                bdmf_session_print(session, "\n\r");

        }

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_get_onu_information_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */     
    uint32_t i;                   
    PON_SERIAL_NUMBER_DTE serial_number = {};
    PON_PASSWORD_DTE password = {};
    uint16_t onu_id = 0;
    uint32_t equalization_delay = 0;
    uint32_t downstream_ber_interval = 0;
    PON_TX_OVERHEAD_DTE pon_overhead = {};
    uint8_t pon_overhead_length = 0;
    uint8_t pon_overhead_repetition_length = 0;
    uint8_t pon_overhead_repetition = 0;
    uint8_t transceiver_power_level = 0;
    PON_ERROR_DTE error = 0;
    
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'ngpon_get_onu_information_command - not implemented yet'\n\r" );
    /* -------------------------------------------------------------------- */
    /*
    error = gpon_get_onu_information ( & serial_number, & password, & onu_id, & equalization_delay,
                & downstream_ber_interval, & pon_overhead, & pon_overhead_length,
                & pon_overhead_repetition_length, & pon_overhead_repetition, & transceiver_power_level );
    */
    /* -------------------------------------------------------------------- */
    
    if ( error == PON_NO_ERROR )
    {
        for ( i = 0 ; i < VENDOR_CODE_FIELD_SIZE ; i++ )
    {
        bdmf_session_print(session,"serial_number - vendor code [%d]: %02X \n",i,serial_number.vendor_code [ i ] );
    }
    
    for ( i = 0 ; i < VENDOR_SPECIFIC_FIELD_SIZE ; i++ )
    {
        bdmf_session_print(session,"serial_number [%d]: %02X \n",i,serial_number.serial_number [ i ] );
    }
    
    for ( i = 0 ; i < PON_PASSWORD_FIELD_SIZE ; i++ )
    {
        bdmf_session_print(session,"password [%d]: %02X \n",i , password.password [ i ] );
    }
    
    
    for ( i = 0; i < PON_TX_PREAMBLE_OVERHEAD_SIZE_IN_BYTES; i++ )
    {
        bdmf_session_print(session,"pon_overhead [%d]: %d \n",i ,pon_overhead.overhead [ i ] );
    }
        
    bdmf_session_print(session,
                "onu_id %d\nequalization_delay %d\n"
                "downstream_ber_interval %d\n"
                "pon_overhead_length %d\n"
                "pon_overhead_repetition_length %d\n"
                "pon_overhead_repetition %d\n"
                "transceiver_power_level %d \n",
                onu_id,
                equalization_delay,
                downstream_ber_interval,
                pon_overhead_length,
                pon_overhead_repetition_length,
                pon_overhead_repetition,
                transceiver_power_level );
    
    }    
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );
    
    return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_get_equalization_delay_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        uint32_t equalization_delay = 0 ;
        PON_ERROR_DTE error ;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_get_equalization_delay_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_get_equalization_delay ( & equalization_delay );
        /* -------------------------------------------------------------------- */
        if ( error == PON_NO_ERROR )
        {
                bdmf_session_print(session,
                                "equalization_delay = %d\n\r",
                                equalization_delay );
        }

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_set_equalization_delay_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */

        uint32_t delay = 0;

        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error = 0 ;


        delay = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function ngpon_set_equalization_delay_command\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_set_eqd (delay, 1, 0); 
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}
static char gs_onu_information_buffer[2048];

static void format_serial_number(uint8_t *xi_serial_number, uint8_t *xo_buffer)
{
    sprintf(xo_buffer, "%02X%02X%02X%02X-%02X%02X%02X%02X",
        xi_serial_number[0], xi_serial_number[1],
        xi_serial_number[2], xi_serial_number[3],
        xi_serial_number[4], xi_serial_number[5],
        xi_serial_number[6], xi_serial_number[7]);
}

PON_ERROR_DTE format_onu_information(char **xo_onu_information)
{
    char buffer [ 32 ];
    /*    char buffer1 [ 32 ]; */
    char buffer2 [ 32 ];

    format_serial_number(( char * ) & g_xgpon_db.onu_params.sn, buffer);

    if (g_xgpon_db.onu_params.onu_id == 0xFFFF )
    {
        sprintf(buffer2, "DON'T CARE");
    }
    else if (g_xgpon_db.onu_params.onu_id == PON_ID_BROADCAST )
    {
        sprintf(buffer2, "BROADCAST");
    }
    else
    {
        sprintf(buffer2, "%d",(int16_t ) g_xgpon_db.onu_params.onu_id);
    }
    /*
    sprintf(gs_onu_information_buffer,
              " Active:                              %s\n"
              " Serial number:                       %02X%02X%02X%02X%02X%02X%02X%02X\n"              " Preferred ONU-ID:                    %s\n"
              " ONU-ID:                              %s\n"
              " Pre-equalization status:             %s\n"
              " Extra serial number transmissions:   %d\n"
              " Number of guard bits:                %d\n"
              " Number of type 1 preamble bits:      %d\n"
              " Number of type 2 preamble bits:      %d\n"
              " Pattern of type 3 preamble bits:     0x%02X\n"
              " Delimiter:                           %02X%02X%02X\n"
              " Preassigned equalization delay:      %d\n"
              " Default power level mode:            %d\n"
              " Serial number mask status:           %s\n\n"
              " State:                               %s\n"
              " Sub-state:                           %s\n"
              " Internal state:                      %s\n"
              " Serial_number_request_messages_counter: %d\n",
             (gs_gpon_database.link_parameters.operation_state_machine.link_state == LINK_STATE_ACTIVE ? "ACTIVATED" : "DEACTIVATED" ),
              gs_gpon_database.physical_parameters.serial_number.vendor_code [ 0 ],
              gs_gpon_database.physical_parameters.serial_number.vendor_code [ 1 ],
              gs_gpon_database.physical_parameters.serial_number.vendor_code [ 2 ],
              gs_gpon_database.physical_parameters.serial_number.vendor_code [ 3 ],
              gs_gpon_database.physical_parameters.serial_number.serial_number [ 0 ],
              gs_gpon_database.physical_parameters.serial_number.serial_number [ 1 ],
              gs_gpon_database.physical_parameters.serial_number.serial_number [ 2 ],
              gs_gpon_database.physical_parameters.serial_number.serial_number [ 3 ],
              buffer1,
              buffer2,
             (gs_gpon_database.onu_parameters.pre_equalization_status == BDMF_FALSE ? "DONT-USE" : "USE" ),
             (uint16_t ) gs_gpon_database.onu_parameters.extra_serial_number_transmissions,
             (uint16_t ) gs_gpon_database.onu_parameters.number_of_guard_bits,
             (uint16_t ) gs_gpon_database.onu_parameters.number_of_type_1_preamble_bits,
             (uint16_t ) gs_gpon_database.onu_parameters.number_of_type_2_preamble_bits,
             (uint16_t ) gs_gpon_database.onu_parameters.pattern_of_type_3_preamble_bits,
             (uint16_t ) gs_gpon_database.onu_parameters.delimiter [ 0 ],
             (uint16_t ) gs_gpon_database.onu_parameters.delimiter [ 1 ],
             (uint16_t ) gs_gpon_database.onu_parameters.delimiter [ 2 ],
             (uint16_t ) gs_gpon_database.onu_parameters.preassigned_equalization_delay,
             (uint16_t ) gs_gpon_database.onu_parameters.default_power_level_mode,
             (gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_status == BDMF_FALSE ? "DISABLED" : "ENABLED" ),
              gs_link_state_names [ gs_gpon_database.link_parameters.operation_state_machine.link_state ],
              gs_link_sub_state_names [ gs_gpon_database.link_parameters.operation_state_machine.link_sub_state ],
              gs_activation_state_names [ gs_gpon_database.link_parameters.operation_state_machine.activation_state ],
             (uint16_t ) gs_gpon_database.link_parameters.serial_number_request_messages_counter);
    */
    *xo_onu_information = gs_onu_information_buffer;

    return PON_NO_ERROR;
}

static int ngpon_get_info_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    /* char *onu_information; */
    PON_ERROR_DTE error = 0;
    
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_info_command' - not implemented yet\n\r" );
    /* -------------------------------------------------------------------- */
    /* error = format_onu_information(&onu_information); */
    /* -------------------------------------------------------------------- */
    /* bdmf_session_print(session, "\n%s\n", onu_information); */
    
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );
    
    return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_set_min_response_time_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        PON_ERROR_DTE error = PON_NO_ERROR;
        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_set_min_response_time_command'\n\r" );
        /* Write to the DB */
        g_xgpon_db.physic_params.min_response_time = parm[0].value.unumber;
        return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_set_to1_timer_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        PON_ERROR_DTE error = PON_NO_ERROR;
        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_set_to1_timer_command'\n\r  timeout=%ld",parm[0].value.unumber);
        /* Write to the DB */
        g_xgpon_db.onu_params.to1_timer_timeout = parm[0].value.unumber;
        return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_set_to2_timer_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        PON_ERROR_DTE error = PON_NO_ERROR;
        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_set_to2_timer_command' timeout=%ld\n\r",parm[0].value.unumber);
        /* Write to the DB */
        g_xgpon_db.onu_params.to2_timer_timeout = parm[0].value.unumber;
        return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_set_to3_timer_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        PON_ERROR_DTE error = PON_NO_ERROR;
        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_set_to3_timer_command' timeout=%ld\n\r",parm[0].value.unumber);
        /* Write to the DB */
        g_xgpon_db.onu_params.to3_timer_timeout = parm[0].value.unumber;
        return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_set_to4_timer_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        PON_ERROR_DTE error = PON_NO_ERROR;
        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_set_to4_timer_command' timeout=%ld\n\r",parm[0].value.unumber);
        /* Write to the DB */
        g_xgpon_db.onu_params.to4_timer_timeout = parm[0].value.unumber;
        return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_set_to5_timer_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        PON_ERROR_DTE error = PON_NO_ERROR;
        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_set_to5_timer_command' timeout=%ld\n\r",parm[0].value.unumber);
        /* Write to the DB */
        g_xgpon_db.onu_params.to5_timer_timeout = parm[0].value.unumber;
        return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_set_toz_timer_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        PON_ERROR_DTE error = PON_NO_ERROR;
        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_set_toz_timer_command' timeout=%ld\n\r",parm[0].value.unumber);
        /* Write to the DB */
        g_xgpon_db.onu_params.toz_timer_timeout = parm[0].value.unumber;
        return error ? BDMF_ERR_PARM : 0;
}
#ifdef G989_3_AMD1
static int ngpon_set_tcpi_timer_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        PON_ERROR_DTE error = PON_NO_ERROR;
        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_set_tcpi_timer_command' timeout=%ld\n\r",parm[0].value.unumber);
        /* Write to the DB */
        g_xgpon_db.onu_params.tcpi_tout = parm[0].value.unumber ;
        return error ? BDMF_ERR_PARM : 0;
}
#endif

static int ngpon_send_dying_gasp_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        PON_ERROR_DTE error = PON_NO_ERROR;
        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_send_dying_gasp_command' state=%ld\n\r",parm[0].value.unumber);
        /* Write to the DB */
        ngpon_set_dying_gasp_state (parm[0].value.unumber);
        return error ? BDMF_ERR_PARM : 0;
}


static int ngpon_configure_link_params_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        rdpa_gpon_link_cfg_t pon_parameters ;
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error ;

        /* Define parameters handling variables */
        pon_parameters.transceiver_dv_setup_pattern          = parm[parameter_index++].value.unumber;
        pon_parameters.transceiver_dv_hold_pattern           = parm[parameter_index++].value.unumber;
        pon_parameters.transceiver_dv_polarity               = parm[parameter_index++].value.unumber;
        pon_parameters.transceiver_power_calibration_mode    = parm[parameter_index++].value.unumber;
        pon_parameters.transceiver_power_calibration_pattern = parm[parameter_index++].value.unumber;
        pon_parameters.transceiver_power_calibration_size    = parm[parameter_index++].value.unumber;
        pon_parameters.ber_interval                          = parm[parameter_index++].value.unumber;
        pon_parameters.min_response_time                     = parm[parameter_index++].value.unumber;
        pon_parameters.to1_timeout                           = parm[parameter_index++].value.unumber;
        pon_parameters.to2_timeout                           = parm[parameter_index++].value.unumber;
        pon_parameters.tx_data_polarity                      = parm[parameter_index++].value.unumber;
        pon_parameters.rx_din_polarity                       = parm[parameter_index++].value.unumber;
        pon_parameters.ber_threshold_for_sd_assertion        = parm[parameter_index++].value.unumber;
        pon_parameters.ber_threshold_for_sf_assertion        = parm[parameter_index++].value.unumber;
        pon_parameters.number_of_psyncs_for_lof_assertion    = parm[parameter_index++].value.unumber;
        pon_parameters.number_of_psyncs_for_lof_clear        = parm[parameter_index++].value.unumber;

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_configure_link_params_command'\n\r" );
        /* -------------------------------------------------------------------- */
        error = ngpon_configure_link_params ( & pon_parameters );
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

        return error ? BDMF_ERR_PARM : 0;

}

static int ngpon_dba_debug_statistics_allocate_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
#ifdef DBA_DEBUG_STATISTICS

    uint32_t memory_size = 0;
    uint32_t tconts_number;
    uint32_t cycles_number;
    uint8_t parameter_index = 0;

    tconts_number = parm[parameter_index++].value.unumber;
    cycles_number = parm[parameter_index++].value.unumber;

    if(ge_dba_debug_statistics != NULL)
    {
        bdmf_session_print(session, "Existing statistics database must be freed before allocating new one\n\r");
        return BDMF_ERR_INTERNAL;
    }

    bdmf_session_print(session, "UT: Invoking function 'ngpon_dba_allocate_debug_statistics_db - not implemented yet'\n\r" );
    /* --------------------------------------------------------------------------------- */
    /* memory_size = gpon_dba_allocate_debug_statistics_db(tconts_number, cycles_number); */
    /* --------------------------------------------------------------------------------- */
    if (memory_size == 0)                                  
    {
        bdmf_session_print(session, "Failed to allocate the debug statistics database!\n\r" );
        return BDMF_ERR_INTERNAL;
    }
    
    bdmf_session_print(session, "Debug statistics database was allocated, memory size is: %lu bytes\n\r", memory_size);

    if (memory_size > CE_DBA_DEBUG_STATISTICS_DATABASE_SIZE_THREASHOLD_FOR_WARNING)
    {
        bdmf_session_print(session, "WARNING: Debug statistics database size is above %lu bytes!\n\r", CE_DBA_DEBUG_STATISTICS_DATABASE_SIZE_THREASHOLD_FOR_WARNING);
    }

    /* gpon_dba_debug_reset_statistics(); */

#else
    bdmf_session_print(session, "This operation is not supported without DBA_DEBUG_STATISTICS compilation flag\n\r");
#endif 

    return 0;
}

static int ngpon_dba_debug_statistics_free_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
#ifdef DBA_DEBUG_STATISTICS

    if(ge_dba_debug_statistics == NULL)
    {
        bdmf_session_print(session, "Statistics database was not allocated\n\r");
        return BDMF_ERR_INTERNAL;
    }

    /* --------------------------------------------------------------------------------- */
    /* gpon_dba_free_debug_statistics_db(ge_dba_debug_statistics_max_number_of_cycles); */
    /* --------------------------------------------------------------------------------- */

    bdmf_session_print(session, "Debug statistics database was freed\n\r");

#else
    bdmf_session_print(session, "This operation is not supported without DBA_DEBUG_STATISTICS compilation flag\n\r");
#endif 

    return 0;
}

static int ngpon_dba_debug_statistics_collect_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
#ifdef DBA_DEBUG_STATISTICS

    if(ge_dba_debug_statistics == NULL)
    {
        bdmf_session_print(session, "statistics database must be allocated\n\r");
        return BDMF_ERR_INTERNAL;
    }

    /* start collecting statistics */
    if(ge_dba_debug_statistics_collect)
    {
        bdmf_session_print(session, "Previous statistics must be reset\n\r");
    }
    else
    {
        ge_dba_debug_statistics_number_of_cycles = parm[1].value.unumber;
        ge_dba_debug_statistics_collect = 1;

        bdmf_session_print(session, "Starting to collect DBA debug statistics \n\r");
    }

#else
    bdmf_session_print(session, "This operation is not supported without DBA_DEBUG_STATISTICS compilation flag\n\r");
#endif 

    return 0;
}

static int ngpon_dba_debug_statistics_reset_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
#ifdef DBA_DEBUG_STATISTICS

    if(ge_dba_debug_statistics == NULL)
    {
        bdmf_session_print(session, "statistics database must be allocated\n\r");
        return BDMF_ERR_INTERNAL;
    }

    /* -------------------------------- */
    /* ngpon_dba_debug_reset_statistics(); */
    /* -------------------------------- */

    bdmf_session_print(session, "DBA debug statistics were reset\n\r");

#else 
    bdmf_session_print(session, "This operation is not supported without DBA_DEBUG_STATISTICS compilation flag\n\r");
#endif 

    return 0;
}

static int ngpon_dba_debug_dump_statistics_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
#ifdef DBA_DEBUG_STATISTICS
#define DBA_ALL_TCONTS 40
#if 0

    PON_FIFO_ID_DTE tcont_id;
    uint32_t dba_cycle_number;
    uint32_t index;
    uint32_t dba_sr_data_average = 0;
    uint32_t tcont_occupancy_average = 0;
    uint32_t runner_occupancy_average = 0;
    uint32_t dbr_message_average = 0;
    uint32_t one_tcont_only = 0;
    uint32_t input_tcont_id;
    uint8_t parameter_index = 0;

    input_tcont_id = parm[parameter_index++].value.unumber;

    /* if specific tcont id */
    if(input_tcont_id != DBA_ALL_TCONTS)
    {
        if((input_tcont_id == -1) || ((input_tcont_id > TXPON_FIFO_UNIT_HIGH) || (input_tcont_id < TXPON_FIFO_UNIT_LOW)))
        {
            bdmf_session_print(session, "input_tcont_id - Wrong parameters\n\r" );
            return BDMF_ERR_INTERNAL;
        }

        one_tcont_only = 1;
    }

    if(ge_dba_debug_statistics == NULL)
    {
        bdmf_session_print(session, "statistics database must be allocated\n\r");
        return BDMF_ERR_INTERNAL;
    }

    if(ge_dba_debug_statistics_cycle_counter != ge_dba_debug_statistics_number_of_cycles)
    {
        bdmf_session_print(session, "Collecting statistics is not finished (cycle_counter = %lu, number_of_cycles = %lu)\n\r",
            ge_dba_debug_statistics_cycle_counter, ge_dba_debug_statistics_number_of_cycles);
        return BDMF_ERR_INTERNAL;
    }

    bdmf_session_print(session, "Dumping DBA SR debug statistics \n\n\r");

    for(tcont_id = CE_TXPON_FIFO_UNIT_LOW; tcont_id < CE_TXPON_FIFO_UNIT_NUMBER; ++tcont_id)
    {
        if (one_tcont_only && (tcont_id != input_tcont_id))
        {
            continue;
        }

        index = ge_dba_debug_statistics->tcont_to_index[tcont_id];

        if(index != CE_DBA_DEBUG_STATISTICS_DONT_CARE_VALUE)
        {
            dba_sr_data_average = 0;
            tcont_occupancy_average = 0;
            runner_occupancy_average = 0;
            dbr_message_average = 0;

            bdmf_session_print(session, "--- TCONT %lu: ---\n\r" , tcont_id);

            bdmf_session_print(session, "Reported Data: \n\r" );
            for(dba_cycle_number = 0; dba_cycle_number < ge_dba_debug_statistics_number_of_cycles; ++dba_cycle_number)
            {
                bdmf_session_print(session, "%10lu ", ge_dba_debug_statistics ->cycle_info_array[dba_cycle_number].statistics_tcont_info_array [ index ].dba_sr_data );
                dba_sr_data_average +=  ge_dba_debug_statistics ->cycle_info_array[dba_cycle_number].statistics_tcont_info_array [ index ].dba_sr_data;
            }

            dba_sr_data_average /= ge_dba_debug_statistics_number_of_cycles;
            bdmf_session_print(session, "\n\r");

            bdmf_session_print(session, "TCONT Occupancy: \n\r");
            for(dba_cycle_number = 0 ; dba_cycle_number < ge_dba_debug_statistics_number_of_cycles ; ++ dba_cycle_number)
            {
                bdmf_session_print(session, "%10lu ", ge_dba_debug_statistics->cycle_info_array[dba_cycle_number].statistics_tcont_info_array[index].tcont_occupancy );
                tcont_occupancy_average += ge_dba_debug_statistics->cycle_info_array[dba_cycle_number].statistics_tcont_info_array[index].tcont_occupancy;
            }
            tcont_occupancy_average /= ge_dba_debug_statistics_number_of_cycles;
            bdmf_session_print(session, "\n\r" );

            bdmf_session_print(session, "Runner Occupancy: \n\r");
            for ( dba_cycle_number = 0 ; dba_cycle_number < ge_dba_debug_statistics_number_of_cycles ; ++ dba_cycle_number )
            {
                bdmf_session_print(session, "%10lu ", ge_dba_debug_statistics ->cycle_info_array [ dba_cycle_number ].statistics_tcont_info_array [ index ].runner_occupancy );
                runner_occupancy_average += ge_dba_debug_statistics ->cycle_info_array [ dba_cycle_number ].statistics_tcont_info_array [ index ].runner_occupancy ;
            }
            runner_occupancy_average /= ge_dba_debug_statistics_number_of_cycles ;
            bdmf_session_print(session, "\n\r" );
            bdmf_session_print(session, "Encoded Message: \n\r" );
            for(dba_cycle_number = 0; dba_cycle_number < ge_dba_debug_statistics_number_of_cycles; ++ dba_cycle_number)
            {
                bdmf_session_print(session, "%8s%02x ", "0x", ge_dba_debug_statistics->cycle_info_array[dba_cycle_number].statistics_tcont_info_array[index].dbr_message);
                dbr_message_average += ge_dba_debug_statistics ->cycle_info_array [ dba_cycle_number ].statistics_tcont_info_array[index].dbr_message;
            }

            dbr_message_average /= ge_dba_debug_statistics_number_of_cycles;
            bdmf_session_print(session, "\n\n\r");
            bdmf_session_print(session, "Reported Data average:     %lu \n\r", dba_sr_data_average);
            bdmf_session_print(session, "TCONT Occupancy average:   %lu \n\r", tcont_occupancy_average);
            bdmf_session_print(session, "Runner Occupancy average:  %lu \n\r", runner_occupancy_average);
            bdmf_session_print(session, "Encoded Message average:   0x%02x\n\r", dbr_message_average);
            bdmf_session_print(session, "\n\r" );

            if(one_tcont_only && (tcont_id == input_tcont_id ))
            {
                break ;
            }
        }
    }

    if (ge_dba_debug_statistics_warning_not_all_tconts_were_collected)
    {
        bdmf_session_print(session, "\nWARNING! Statistics were collected for only part of the TCONT IDs.\nYou should allocate database for more than %lu TCONT IDs\n\r",
            ge_dba_debug_statistics_max_number_of_tconts );
    }

    bdmf_session_print(session, "\nEnd of debug statistics\n\r");

#undef DBA_ALL_TCONTS
#else 
    bdmf_session_print(session, "This operation is not supported without DBA_DEBUG_STATISTICS compilation flag\n\r");
#endif
 #endif
    return 0; 
}

static     char object_list[5000];
static int  ngpon_get_log_object_list_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    PON_ERROR_DTE error = PON_NO_ERROR ;
    char *object_list_p =  object_list ;
    char *curr_str_p = NULL ;

    char *parent_name = parm[0].value.string;

    memset (object_list, 0, sizeof(object_list)); 
    strcpy(object_list, "\n");

    error = gpon_get_log_object_list(parent_name, object_list);

    if ( error != PON_NO_ERROR )
    {
        bdmf_session_print(session, "Gpon Logger Object %s not found\n", parent_name);
    }

    /* Print line-by-line */
    while ((curr_str_p = strsep(&object_list_p,"\n")) != NULL) 
    {
       bdmf_session_print(session, "%s\n\r",  curr_str_p);
    }
    

    return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_set_log_object_state_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    PON_ERROR_DTE error = PON_NO_ERROR ;
    char *object_list_p =  object_list ;
    char *curr_str_p = NULL ;

    char *object_name = parm[0].value.string;

    error = gpon_set_log_object_state(object_name, parm[1].value.unumber);

    if ( error != PON_NO_ERROR )
    {
        bdmf_session_print(session, "Gpon Logger Object %s not found\n", object_name);
    }
    else
    {

        memset(object_list,0, sizeof(object_list));
        gpon_get_log_object_list(object_name, object_list);

        /* Print line-by-line */
        while ((curr_str_p = strsep(&object_list_p,"\n")) != NULL) 
        {
           bdmf_session_print(session, "%s\n\r",  curr_str_p);
        }
    }

    return error ? BDMF_ERR_PARM : 0;
}

static int ngpon_set_broadcast_command ( bdmf_session_handle  session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms )
{
  /*
    gs_gpon_database.onu_parameters.broadcast_default_value = parm[0].value.unumber;
    bdmf_session_print(session, "BROADCAST ID = 0x%x\n", gs_gpon_database.onu_parameters.broadcast_default_value);
  */
    return(0);
}

static int ngpon_print_sys_prof ( bdmf_session_handle  session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms )
{
    
   if ((g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_10G) ||
       (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_2_5G))
   {
     bdmf_session_print(session,"NG-PON2 System Profile\n");
     bdmf_session_print(session,"Profile is %s\n", (g_xgpon_db.link_params.system_profile.system_profile_received)? "valid" : "not valid");
     bdmf_session_print(session,"Version                             0x%02x\n", g_xgpon_db.link_params.system_profile.version);
     bdmf_session_print(session,"NGPON2 System ID                    (0x%02x 0x%02x 0x%02x)\n",
        g_xgpon_db.link_params.system_profile.ng2sys_id[0],
        g_xgpon_db.link_params.system_profile.ng2sys_id[1],
        g_xgpon_db.link_params.system_profile.ng2sys_id[2]
     );
     bdmf_session_print(session,"Upstream Operation Wavelength bands 0x%02x\n", g_xgpon_db.link_params.system_profile.us_operation_wavelength_bands_twdm);           
     bdmf_session_print(session,"TWDM Channel Count                  0x%02x\n", g_xgpon_db.link_params.system_profile.twdm_channel_count);           
     bdmf_session_print(session,"TWDM Channel Spacing                0x%02x\n", g_xgpon_db.link_params.system_profile.channel_spacing_twdm);           
     bdmf_session_print(session,"TWDM Upstream MSE                   0x%02x\n", g_xgpon_db.link_params.system_profile.us_mse_twdm);           
     bdmf_session_print(session,"FSR/TWDM                            0x%02x 0x%02x\n", 
        g_xgpon_db.link_params.system_profile.fsr_twdm[0],
        g_xgpon_db.link_params.system_profile.fsr_twdm[1]
     );           
     bdmf_session_print(session,"TWDM AMCC Control (Use AMCC)        0x%02x\n", g_xgpon_db.link_params.system_profile.amcc_use_flag);           
     bdmf_session_print(session,"TWDM AMCC Control (Min accuracy)    0x%02x\n", g_xgpon_db.link_params.system_profile.min_calibration_accuracy_for_activation_for_in_band); 
     bdmf_session_print(session,"Loose calibration bound             0x%02x\n", g_xgpon_db.link_params.system_profile.loose_calibration_bound_twdm); 
     bdmf_session_print(session,"PtP related fields skipped\n");
   }
   else
     bdmf_session_print(session,"NG-PON2 System Profile not supported\n");

    return(0);
}

static int ngpon_print_burst_profs ( bdmf_session_handle  session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms )
{
  int i,j;

  bdmf_session_print(session,"Burst profiles - %s\n", (g_xgpon_db.link_params.burst_profiles.burst_profile_received)? "configured" : "not configured");
  for (i=0; i<4; i++)
  {
       bdmf_session_print(session,"\nProfile #%d\n", i);
       bdmf_session_print(session,"Configured index         %d\n", g_xgpon_db.link_params.burst_profiles.profiles[i].profile_index);
       bdmf_session_print(session,"Version                  %d\n", g_xgpon_db.link_params.burst_profiles.profiles[i].profile_version);
       bdmf_session_print(session,"US Line Rate             %d\n",  g_xgpon_db.link_params.burst_profiles.profiles[i].specific_line_rate);
       bdmf_session_print(session,"FEC Indication           %d\n",  g_xgpon_db.link_params.burst_profiles.profiles[i].fec_ind);
       bdmf_session_print(session,"Preamble Length          %d\n",  g_xgpon_db.link_params.burst_profiles.profiles[i].preamble_len);
       bdmf_session_print(session,"Preamble Repeat Counter  %d\n",  g_xgpon_db.link_params.burst_profiles.profiles[i].preamble_repeat_counter);
       bdmf_session_print(session,"Preamble                 ");
       for (j=0; j<g_xgpon_db.link_params.burst_profiles.profiles[i].preamble_len; j++)
       {
         bdmf_session_print(session,"0x%02x ", g_xgpon_db.link_params.burst_profiles.profiles[i].preamble[j]);
       }
       bdmf_session_print(session,"\n");
       bdmf_session_print(session,"Delimiter Length         %d\n", g_xgpon_db.link_params.burst_profiles.profiles[i].delimiter_len);

       bdmf_session_print(session,"Delimiter                ");
       for (j=g_xgpon_db.link_params.burst_profiles.profiles[i].delimiter_len-1; j>=0; j--)
       {
         bdmf_session_print(session,"0x%02x ", g_xgpon_db.link_params.burst_profiles.profiles[i].delimiter[j]);
       }
       bdmf_session_print(session,"\n");
       bdmf_session_print(session,"PON TAG                  ");
       for (j=0; j<PON_TAG_SIZE; j++)
       {
         bdmf_session_print(session,"0x%02x ", g_xgpon_db.link_params.burst_profiles.profiles[i].pon_tag[j]);
       }
       bdmf_session_print(session,"\n");
  }
  return (0);
}
    

static int ngpon_print_chan_profs ( bdmf_session_handle  session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms )
{
  int i;

   if ((g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_10G) ||
       (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_2_5G))
   {
     bdmf_session_print(session,"NG-PON2 Channel Profiles\n");
     bdmf_session_print(session,"Channel profiles   - %s\n", (g_xgpon_db.link_params.ch_profile.channel_profile_received)? "configured" : "not configured");
     bdmf_session_print(session,"Current CPI        - %d\n", g_xgpon_db.link_params.ch_profile.current_cpi);
     bdmf_session_print(session,"Current DS channel - %d\n", g_xgpon_db.link_params.ch_profile.current_ds_channel);
     bdmf_session_print(session,"Current US channel - %d\n", g_xgpon_db.link_params.ch_profile.current_us_channel);

  

     for (i=0; i<16; i++)
     {
       bdmf_session_print(session,"\nChannel #%d\n", i);
       bdmf_session_print(session,"Configured index                 %d\n", g_xgpon_db.link_params.ch_profile.channel[i].control.index);
       bdmf_session_print(session,"Control octet contents:\n");
       bdmf_session_print(session,"_This_ channel indicator         %d\n", g_xgpon_db.link_params.ch_profile.channel[i].control.this_channel_indicator);
       bdmf_session_print(session,"DS void indicator                %d\n",  g_xgpon_db.link_params.ch_profile.channel[i].control.ds_void_indicator);
       bdmf_session_print(session,"US void indicator                %d\n",  g_xgpon_db.link_params.ch_profile.channel[i].control.us_void_indicator);
       bdmf_session_print(session,"Version                          %d\n",  g_xgpon_db.link_params.ch_profile.channel[i].version.profile_version);
       bdmf_session_print(session,"PON ID                           0x%08x\n", ntohl(g_xgpon_db.link_params.ch_profile.channel[i].pon_id));
       bdmf_session_print(session,"DS Frequency Offset              0x%02x\n", g_xgpon_db.link_params.ch_profile.channel[i].ds_freq_offset);
       bdmf_session_print(session,"DS Rate octet contents:\n");
       bdmf_session_print(session," DS Line Rate                    %d\n", g_xgpon_db.link_params.ch_profile.channel[i].ds_rate.ds_tc_layer_line_rate);
       bdmf_session_print(session," DS FEC                          %d\n", g_xgpon_db.link_params.ch_profile.channel[i].ds_rate.ds_fec);
       bdmf_session_print(session," DS Line Code                    0x%02x\n", g_xgpon_db.link_params.ch_profile.channel[i].ds_rate.ds_line_code);
       bdmf_session_print(session,"Channel Partition Indicator      %d\n", g_xgpon_db.link_params.ch_profile.channel[i].channel_partition.index);

       bdmf_session_print(session,"Default Responce Channel PON ID  0x%08x\n", ntohl(g_xgpon_db.link_params.ch_profile.channel[i].default_response_channel));

       bdmf_session_print(session,"SN grant indication contents:\n");
       bdmf_session_print(session," In band SN grants type          %d\n", g_xgpon_db.link_params.ch_profile.channel[i].sn_grant_type.in_band_sn_grants);
       bdmf_session_print(session," AMCC SN grants type             %d\n", g_xgpon_db.link_params.ch_profile.channel[i].sn_grant_type.amcc_sn_grants);

       bdmf_session_print(session,"AMCC window spec                 0x%08x\n", g_xgpon_db.link_params.ch_profile.channel[i].amcc_win_spec);

       bdmf_session_print(session,"US channel index                 %d\n", g_xgpon_db.link_params.ch_profile.channel[i].uwlch_id.channel_id);

       bdmf_session_print(session,"US Frequency                     0x%08x\n", ntohl(g_xgpon_db.link_params.ch_profile.channel[i].us_freq));

       bdmf_session_print(session,"Optical Link Type A supported    %d\n", g_xgpon_db.link_params.ch_profile.channel[i].optical_link_type.type_a_support);
       bdmf_session_print(session,"Optical Link Type B supported    %d\n", g_xgpon_db.link_params.ch_profile.channel[i].optical_link_type.type_b_support);

       bdmf_session_print(session,"US line rate 10Gbps supported    %d\n", g_xgpon_db.link_params.ch_profile.channel[i].us_rate.rate_10G_support);
       bdmf_session_print(session,"US line rate 2.5Gbps supported   %d\n", g_xgpon_db.link_params.ch_profile.channel[i].us_rate.rate_2_5G_support);

       bdmf_session_print(session,"Default ONU Attenuation          %d\n", g_xgpon_db.link_params.ch_profile.channel[i].default_onu_attenuation.level);
       bdmf_session_print(session,"TWDM Response Threshold          %d\n", g_xgpon_db.link_params.ch_profile.channel[i].response_threshold);
     }
   }
   else
   {
     bdmf_session_print(session,"NG-PON2 Channel Profile not supported\n");
   }
    return (0) ;
}

static int ngpon_print_timers ( bdmf_session_handle  session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms )
{
  bdmf_session_print(session,"XGPON/NGPON2/XGS Stack timer periods (in mks)\n");
  
  bdmf_session_print(session,"TO1  -- %d\n", g_xgpon_db.onu_params.to1_timer_timeout);
  bdmf_session_print(session,"TO2  -- %d\n", g_xgpon_db.onu_params.to2_timer_timeout);
  bdmf_session_print(session,"TO3  -- %d\n", g_xgpon_db.onu_params.to3_timer_timeout);
  bdmf_session_print(session,"TO4  -- %d\n", g_xgpon_db.onu_params.to4_timer_timeout);
  bdmf_session_print(session,"TO5  -- %d\n", g_xgpon_db.onu_params.to5_timer_timeout);
  bdmf_session_print(session,"TOZ  -- %d\n", g_xgpon_db.onu_params.toz_timer_timeout);

  return (0) ;

}

static int ngpon_print_wlcp ( bdmf_session_handle  session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms )
{
  bdmf_session_print(session,"NGPON2 Stack WLCP state\n");

  bdmf_session_print(session,"WLCP configured %s\n", (g_xgpon_db.link_params.WLCP)? "yes" : "no");

  bdmf_session_print(session,"WLCP DS PON ID  0x%08x\n", g_xgpon_db.onu_params.protect_ds_pon_id);
  bdmf_session_print(session,"WLCP US PON ID  0x%08x\n", g_xgpon_db.onu_params.protect_us_pon_id);

  return (0) ;

}


static int ngpon_configure_omci_key_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
        /* Define I/O variables */
        uint8_t omci_ik[NGPON_DEFAULT_KEY_SIZE];
        uint8_t parameter_index = 0 ;
        PON_ERROR_DTE error = PON_NO_ERROR;
        uint32_t i ;

        /* Extract password (36 bytes) */
        i = translate_string_to_binary (parm[parameter_index++].value.string, ( char * ) omci_ik, NGPON_DEFAULT_KEY_SIZE);
        if (i != NGPON_DEFAULT_KEY_SIZE)
        {
                bdmf_session_print(session, "OMCI key should contain %d bytes , actually contain %d bytes \n\r", NGPON_DEFAULT_KEY_SIZE, i );
                return BDMF_ERR_PARM;
        }

        /* Call the under test routine */
        bdmf_session_print(session, "UT: Invoking function 'ngpon_configure_onu_password_command'\n\r" );
        /* -------------------------------------------------------------------- */
        ngpon_configure_omci_key (omci_ik) ;
        /* -------------------------------------------------------------------- */

        /* Translate and print the error code */
        bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string (error));

        return error ? BDMF_ERR_PARM : 0;

}


static int ngpon_set_zero_eqd_flag (bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
   uint8_t parameter_index = 0;   
   bdmf_boolean zero_eqd_flag = parm[parameter_index].value.unumber ;

   set_zero_eqd_after_tuning (zero_eqd_flag) ;    
   return 0;
}

static int ngpon_print_zeqd (bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_session_print(session, "Zeroing EqD after NGPON2 channel tuning %s\n\r", get_zero_eqd_after_tuning()? "enabled" : "disabled") ;
   return 0;
}


static int ngpon_set_simplified_rollback_flag (bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
   uint8_t parameter_index = 0;   
   bdmf_boolean simplified_rollback_flag = parm[parameter_index].value.unumber ;

   set_simplified_rollback (simplified_rollback_flag) ;    
   return 0;
}

static int ngpon_print_simplified_rollback_flag (bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_session_print(session, "Simplified Rollback %s\n\r", get_simplified_rollback()? "enabled" : "disabled") ;
   return 0;
}

static int ngpon_get_ploam_pm_counters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
      bdmf_session_print(session,  
          "tx_ploam_total_counter                       %d\n"
          "tx_ploam_regist_counter                      %d\n"
          "tx_ploam_ack_counter                         %d\n" 
          "tx_ploam_key_report_counter                  %d\n",

          g_pon_ploam_pm_counters.tx_ploam_total_counter,
          g_pon_ploam_pm_counters.tx_ploam_regist_counter,
          g_pon_ploam_pm_counters.tx_ploam_ack_counter,
          g_pon_ploam_pm_counters.tx_ploam_key_report_counter);

      return 0;
}

static int ngpon_get_tuning_pm_counters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
      bdmf_session_print(session,  
          "requests_for_rx_or_rxtx                      %d\n"
          "requests_for_tx_counter                      %d\n"
          "requests_failed_to4_exp                      %d\n" 
          "requests_failed_to5_exp                      %d\n",

          g_pon_tuning_req_pm_counters.requests_for_rx_or_rxtx,
          g_pon_tuning_req_pm_counters.requests_for_tx_counter,
          g_pon_tuning_req_pm_counters.requests_failed_to4_exp,
          g_pon_tuning_req_pm_counters.requests_failed_to5_exp);

      return 0;
}

static int ngpon_get_lods_pm_counters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
      bdmf_session_print(session,  
          "total_lods_event_counter                     %d\n"
          "lods_restored_operation_twdm_counter         %d\n"
          "lods_restored_protection_twdm_counter        %d\n" 
          "lods_handshake_fail_protection_counter       %d\n"
          "lods_reactivations                           %d\n",

          g_pon_lods_pm_counters.total_lods_event_counter,
          g_pon_lods_pm_counters.lods_restored_operation_twdm_counter,
          g_pon_lods_pm_counters.lods_restored_protection_twdm_counter,
          g_pon_lods_pm_counters.lods_handshake_fail_protection_counter,
          g_pon_lods_pm_counters.lods_reactivations);

      return 0;
}

#define TRX_TX_CHN_CFG    0x70

#define TRX_TX_CHN_CFG    0x70
#define TRX_RX_CHN_CFG    0x71
#define TRX_PASS_BASE     0x7B


static int ngpon_set_chn_ng_pon2_trx(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
   int rc = 0;
   uint8_t parameter_index = 0;   
   uint8_t rx_channel = parm[parameter_index++].value.unumber - 1;
   uint8_t tx_channel = parm[parameter_index++].value.unumber - 1;
   int bus = -1;
 
   opticaldet_get_xpon_i2c_bus_num(&bus);
   if ((rx_channel > 3) || (tx_channel > 3))
   {
     bdmf_session_print(session, "Illegal channel RX %d TX %d (valid range 1..4)", rx_channel+1, tx_channel+1) ;
   }
   else
   {
      rc = bcmsfp_write_byte(bus, 0, TRX_RX_CHN_CFG, rx_channel);
      if (rc < 0)
      {
         bdmf_session_print(session, "Failed to write value 0x%02x to addr 0x%02x\n\r", rx_channel, TRX_RX_CHN_CFG) ;
      }
      rc = bcmsfp_write_byte(bus, 0, TRX_TX_CHN_CFG, tx_channel);
      if (rc < 0)
      {
         bdmf_session_print(session, "Failed to write value 0x%02x to addr 0x%02x\n\r", tx_channel, TRX_RX_CHN_CFG) ;
      }
   }
   bdmf_session_print(session, "UT: Done...\n\r");
   return (rc < 0) ? BDMF_ERR_PARM : 0;
}



static int ngpon_get_chn_ng_pon2_trx(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
   uint8_t i2c_byte ;
   int bus = -1;
 
   opticaldet_get_xpon_i2c_bus_num(&bus);


   bcmsfp_read_byte(bus, 0, TRX_RX_CHN_CFG, &i2c_byte);
   bdmf_session_print(session, "RX Channel %d\n\r", i2c_byte+1) ;

   bcmsfp_read_byte(bus, 0, TRX_TX_CHN_CFG, &i2c_byte);
   bdmf_session_print(session, "TX Channel %d\n\r", i2c_byte+1) ;

   bdmf_session_print(session, "UT: Done...\n\r");
   return 0;
}

static bdmfmon_handle_t pon_stack_dir;
static bdmfmon_handle_t gpon_drv_dir;

void ngpon_initialize_drv_pon_stack_shell(bdmfmon_handle_t driver_dir)
{

    static struct bdmfmon_enum_val flow_type_enum_table[] = 
    {
                        {"eth", BDMF_FALSE},
        {"tdm", BDMF_TRUE},
        {NULL, 0},
        };

    static struct bdmfmon_enum_val rogue_source_select_enum_table[] = 
    {
                        {"external", BDMF_FALSE},
        {"internal", BDMF_TRUE},
        {NULL, 0},
        };
 
    static struct bdmfmon_enum_val update_mode_select_enum_table[] = 
    {
                        {"mips", BDMF_FALSE},
        {"runner", BDMF_TRUE},
        {NULL, 0},
        };

    static struct bdmfmon_enum_val ploam_priority_select_enum_table[] = 
    {
                        {"normal", 0},
        {"urgent", 1},
        {NULL, 0},
        };

    static struct bdmfmon_enum_val gem_block_size_select_enum_table[] = 
    {
                        {"32_Bytes", 32},
                        {"48_Bytes", 48},
        {"64_Bytes", 64},
        {NULL, 0},
        };

    static struct bdmfmon_enum_val record_type_enum_table[] = 
    {
                        {"all_accesses", NGPON_BW_RECORD_MODE_ALL},
                        {"onu_accesses", NGPON_BW_RECORD_MODE_SINGLE_SPECIFIC},
        {"specific_access", NGPON_BW_RECORD_MODE_SINGLE_SPECIFIC},
        {NULL, 0},
        };

    static struct bdmfmon_enum_val flow_priority_enum_table[] = 
    {
                        {"high", PON_FLOW_PRIORITY_HIGH},
                        {"ploam", PON_FLOW_PRIORITY_PLOAM},
                        {"omci", PON_FLOW_PRIORITY_OMCI},
                        {"unassigned", PON_FLOW_PRIORITY_UNASSIGNED},
        {NULL, 0},
        };

        pon_stack_dir = bdmfmon_dir_add(driver_dir, "pon_stack", "PON stack Driver", BDMF_ACCESS_GUEST, NULL );
        if (!pon_stack_dir)
        {
                bdmf_session_print(NULL, "Can't create pon_stack directory\n");
                return;
        }
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "i", "Software initialize", ngpon_software_initialize_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "t", "Software terminate", ngpon_software_terminate_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "cosn", "Configure onu serial number", ngpon_configure_onu_serial_number_command,
        BDMFMON_MAKE_PARM_RANGE("vendor_code", "vendor_code", BDMFMON_PARM_HEX, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_RANGE("serial_number", "serial_number", BDMFMON_PARM_HEX, 0, 0, 0xFFFFFFFF));
    BDMFMON_MAKE_CMD(pon_stack_dir, "cop", "Configure onu password", ngpon_configure_onu_password_command,
            BDMFMON_MAKE_PARM("onu_password", "onu_password", BDMFMON_PARM_STRING, 0));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "lr", "Link reset", ngpon_link_reset_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "la", "Link activate", ngpon_link_activate_command,
            BDMFMON_MAKE_PARM_ENUM("initial_state_disable", "initial_state_disable true or false", bdmfmon_enum_bool_table, 0));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "ld", "Link deactivate", ngpon_link_deactivate_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "glnk", "Get link parameters", ngpon_get_link_parameters_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gls", "Get link state", ngpon_get_link_state_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gss", "Get software state", ngpon_get_software_state_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "stxq",  "Set txfifo queue configuration", ngpon_set_txfifo_queue_configuration_command,
        BDMFMON_MAKE_PARM_RANGE("packet descriptor id", "packet descriptor id", BDMFMON_PARM_NUMBER, 0, 0, 128),
        BDMFMON_MAKE_PARM_RANGE("txfifo_id", "txfifo_id", BDMFMON_PARM_NUMBER, 0, 0, TXPON_FIFO_UNIT_HIGH),
        BDMFMON_MAKE_PARM_RANGE("packet_descriptor_queue_base_address", "packet_descriptor_queue_base_address - decimal: 0 - 8192 [Bytes]",
            BDMFMON_PARM_NUMBER, 0, 0, 8192),
        BDMFMON_MAKE_PARM_RANGE("packet_descriptor_queue_size", "packet_descriptor_queue_size - \n"
            "decimal: 0 - 128 [Buffer Descriptors] ( Out of total 128 )\n",
            BDMFMON_PARM_NUMBER, 0, 0, 128),
        BDMFMON_MAKE_PARM_RANGE("tx_queue_offset", "tx_queue_offset - decimal: 0 - 8192 [Bytes]",
            BDMFMON_PARM_NUMBER, 0, 0, 8192),
        BDMFMON_MAKE_PARM_RANGE("tx_max_queue_size", "tx_max_queue_size - decimal: 0 - 8192 [Bytes]",
            BDMFMON_PARM_NUMBER, 0, 0, 8192));
    BDMFMON_MAKE_CMD(pon_stack_dir, "gtxq", "Get txfifo queue configuration", ngpon_get_txfifo_queue_configuration_command,
        BDMFMON_MAKE_PARM_RANGE("fifo_id", "fifo_id - 0-31", BDMFMON_PARM_NUMBER, 0, 0, TXPON_FIFO_UNIT_HIGH));
    BDMFMON_MAKE_CMD(pon_stack_dir, "gtxg", "Get txfifo group configuration", ngpon_get_txfifo_queue_configuration_group_0_to_3_command,
        BDMFMON_MAKE_PARM_RANGE("group_id", "fifo group ID", BDMFMON_PARM_NUMBER, 0,
            NGPON_TX_FIFO_QUEUE_INDEX_LOW, NGPON_TX_FIFO_QUEUE_INDEX_HIGH));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "grxpm", "Get rxpon pm counters", ngpon_get_rxpon_pm_counters_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "grxplm", "Get rxpon ploam counters", ngpon_get_rxpon_ploam_counters_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "srxbwa", "Set rxpon bw alloc association", ngpon_set_rxpon_bw_alloc_association_command,
        BDMFMON_MAKE_PARM_RANGE("alloc_1", "alloc 1  - Decimal (0-1023)", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH),
        BDMFMON_MAKE_PARM_RANGE("alloc_2", "alloc 2  - Decimal (0-1023)", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH),
        BDMFMON_MAKE_PARM_RANGE("alloc_3", "alloc 3  - Decimal (0-1023)", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH),
        BDMFMON_MAKE_PARM_RANGE("alloc_4", "alloc 4  - Decimal (0-1023)", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "grxbwa", "Get rxpon bw alloc association", ngpon_get_rxpon_bw_alloc_association_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "grxbwc", "Get rxpon bw alloc counter", ngpon_get_rxpon_bw_alloc_counter_command,
        BDMFMON_MAKE_PARM_RANGE("alloc_id", "alloc id  - Decimal (0-1023)", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH));
    BDMFMON_MAKE_CMD(pon_stack_dir, "gtpdc", "Get tcont front end PD counter", ngpon_get_tcont_fe_queue_pd_counter_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_id", "tcont_id : Decimal (0 - 39 )", BDMFMON_PARM_NUMBER, 0, 0, NGPON_DUMMY_TCONT_ID_VALUE));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gtxpm", "Get txpon pm counters", ngpon_get_txpon_pm_counters_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "gtxfpm","Get txfifo pm counters", ngpon_get_tcont_pm_counters_command,
        BDMFMON_MAKE_PARM_RANGE("fifo_id", "fifo_id : [0 - 3]", BDMFMON_PARM_NUMBER, 0, 0, 3));
    BDMFMON_MAKE_CMD(pon_stack_dir, "ctpif", "Control port id filter", ngpon_control_port_id_filter_command,
        BDMFMON_MAKE_PARM_RANGE("flow_id", "flow id: [0 - 255]", BDMFMON_PARM_NUMBER, 0, 0, 255),
        BDMFMON_MAKE_PARM_ENUM("filter_status", "filter status: [true | false]", bdmfmon_enum_bool_table, 0));
    BDMFMON_MAKE_CMD(pon_stack_dir, "cpif", "Configure port id filter", ngpon_configure_port_id_filter_command,
        BDMFMON_MAKE_PARM_RANGE("flow_id", "flow id: [0 - 255]", BDMFMON_PARM_NUMBER, 0, 0, 255),
        BDMFMON_MAKE_PARM_RANGE("port_id_filter", "port id: [0 - 4096]", BDMFMON_PARM_NUMBER, 0, 0, 4095),
        BDMFMON_MAKE_PARM_RANGE("port_id_mask", "Port id mask: [0 - 4096]", BDMFMON_PARM_NUMBER, 0, 0, 4095),
        BDMFMON_MAKE_PARM_ENUM("encryption_mode", "encryption mode: [on | off]", bdmfmon_enum_bool_table, 0),
        BDMFMON_MAKE_PARM_ENUM("flow_type", "flow_type mode: [eth | tdm]", flow_type_enum_table, 0),
        BDMFMON_MAKE_PARM_ENUM("flow_priority", "flow priority mode: [low | high | omci | unassigned]", flow_priority_enum_table, 0));
    BDMFMON_MAKE_CMD(pon_stack_dir, "mepif", "Modify encryption port id filter", ngpon_modify_encryption_port_id_filter_command,
        BDMFMON_MAKE_PARM_RANGE("flow_id", "flow id: [0 - 255]", BDMFMON_PARM_NUMBER, 0, 0, 255),
        BDMFMON_MAKE_PARM_ENUM("encryption_mode", "encryption mode: [on | off]", bdmfmon_enum_bool_table, 0));
    BDMFMON_MAKE_CMD(pon_stack_dir, "gpif",  "Get port id filter configuration", ngpon_get_port_id_filter_command,
        BDMFMON_MAKE_PARM_RANGE("flow_id", "flow id: [0 - 255]", BDMFMON_PARM_NUMBER, 0, 0, 255));
    BDMFMON_MAKE_CMD(pon_stack_dir, "ctcnt", "Configure tcont", ngpon_configure_tcont_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_queue_id", "tcont_queue_id - [0..39]", BDMFMON_PARM_NUMBER, 0, 0, 39),
        BDMFMON_MAKE_PARM_RANGE("alloc_id", "alloc_id - [0..4095]", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH));
    BDMFMON_MAKE_CMD(pon_stack_dir, "gtcnt", "Get tcont params", ngpon_get_tcont_params_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_queue_id", "tcont_queue_id - [0..39]", BDMFMON_PARM_NUMBER, 0, 0, 39));
    BDMFMON_MAKE_CMD(pon_stack_dir, "mtcnt", "Modify tcont", ngpon_modify_tcont_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_queue_id", "tcont_queue_id - [1..39]", BDMFMON_PARM_NUMBER, 0, 1, 39),
        BDMFMON_MAKE_PARM_RANGE("new_alloc_id", "new alloc_id - [0..4095]", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH));
    BDMFMON_MAKE_CMD(pon_stack_dir, "rtcnt", "Remove tcont", ngpon_remove_tcont_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_queue_id", "tcont_queue_id - [1..39]", BDMFMON_PARM_NUMBER, 0, 1, 39));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "ptcnt", "Print tcont table", ngpon_print_tcont_table_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "idbr", "Init dba sr process ", ngpon_init_dba_sr_process_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "tdbr", "Terminate dba sr process ", ngpon_terminate_dba_sr_process_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gbip8c","Get bip8 counters ", ngpon_get_bip8_counters_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "sbwrcd","start bw recording ", ngpon_start_bw_recording_command,
        BDMFMON_MAKE_PARM_ENUM("rcd_stop", "recocd_stop - { false = Stop record on map end } | { true = Stop record on memory end }", bdmfmon_enum_bool_table, 0),
        BDMFMON_MAKE_PARM_ENUM("recocd_type", "recocd_stop - all_accesses | onu_accesses | specific_access ", record_type_enum_table, 0),
        BDMFMON_MAKE_PARM_RANGE("alloc_id", "Alloc id to be record (0-1023) in case record_type=specific_access", BDMFMON_PARM_NUMBER, 0, 0, 1023));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gbwrcd","get BW recording data ", ngpon_get_bw_recording_data_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "sbwr","Stop BW recording", ngpon_stop_bw_recording_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "sdbrp" ,"Set rogue onu detection parameters ", ngpon_set_rogue_onu_detection_params_command,
        BDMFMON_MAKE_PARM_ENUM("rogue_onu_level_clear", "rogue_onu_level_clear", bdmfmon_enum_bool_table, 0),
        BDMFMON_MAKE_PARM_ENUM("rogue_onu_diff_clear", "rogue_onu_diff_clear", bdmfmon_enum_bool_table, 0),
        BDMFMON_MAKE_PARM_ENUM("source_select", "external (from IO) | internal (serDes)", rogue_source_select_enum_table, 0),
        BDMFMON_MAKE_PARM_RANGE("rogue_onu_level_threshold", "rogue_onu_level_threshold", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFF),
        BDMFMON_MAKE_PARM_RANGE("rogue_onu_diff_threshold", "rogue_onu_diff_threshold", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFF));
    BDMFMON_MAKE_CMD(pon_stack_dir, "srop"  ,"Set dbru report parameters ", ngpon_set_dbru_report_parameters_command,
        BDMFMON_MAKE_PARM_ENUM("update_mode", "mode - mips | runner", update_mode_select_enum_table, 0),
        BDMFMON_MAKE_PARM_ENUM("gem_block_size", "block size  - Decimal 32 | 48 | 64 Bytes", gem_block_size_select_enum_table, 0));
    BDMFMON_MAKE_CMD(pon_stack_dir, "slb", "Set loopback", ngpon_set_loopback_command,
        BDMFMON_MAKE_PARM_ENUM("loopback_status", "loopback_status", bdmfmon_enum_bool_table, 0),
        BDMFMON_MAKE_PARM_RANGE("loopback_queue", "loopback_queue - 0...6", BDMFMON_PARM_NUMBER, 0, 0, 6));
    BDMFMON_MAKE_CMD(pon_stack_dir, "fqueue", "flush queue", ngpon_flush_queue_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_id", "tcont_id - [0..39]", BDMFMON_PARM_NUMBER, 0, 0, 39));
    BDMFMON_MAKE_CMD(pon_stack_dir, "spm", "Send ploam message", ngpon_send_ploam_message_command,
        BDMFMON_MAKE_PARM_RANGE("repetition", "repetition", BDMFMON_PARM_NUMBER, 0, 1, 3),
        BDMFMON_MAKE_PARM_RANGE("message_id", "ploam message id in HEX format", BDMFMON_PARM_HEX, 0, 0, 0xFF),
        BDMFMON_MAKE_PARM("ploam_buffer", "string of 10 bytes in HEX format", BDMFMON_PARM_STRING, 0),
        BDMFMON_MAKE_PARM_ENUM("priority", "priority - normal | urgent ( only 1 time to send )", ploam_priority_select_enum_table, 0));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "goi", "Get onu information", ngpon_get_onu_information_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "ged", "Get equalization delay", ngpon_get_equalization_delay_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "seqd", "seqd", ngpon_set_equalization_delay_command,
        BDMFMON_MAKE_PARM_RANGE("delay", "delay", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gi", "Get info", ngpon_get_info_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "smrt", "set min response time", ngpon_set_min_response_time_command,
        BDMFMON_MAKE_PARM_RANGE("min_response_time", "min_response_time uint 32", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF));
    BDMFMON_MAKE_CMD(pon_stack_dir, "sto1", "Set to1 timer", ngpon_set_to1_timer_command,
       BDMFMON_MAKE_PARM_RANGE("to1_timer", "to1_timer uint 32", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF));
    BDMFMON_MAKE_CMD(pon_stack_dir, "sto2", "Set to2 timer", ngpon_set_to2_timer_command,
        BDMFMON_MAKE_PARM_RANGE("to2_timer", "to2_timer uint 32", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF));
    BDMFMON_MAKE_CMD(pon_stack_dir, "sto3", "Set to3 timer", ngpon_set_to3_timer_command,
        BDMFMON_MAKE_PARM_RANGE("to3_timer", "to3_timer uint 32", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF));
    BDMFMON_MAKE_CMD(pon_stack_dir, "sto4", "Set to4 timer", ngpon_set_to4_timer_command,
        BDMFMON_MAKE_PARM_RANGE("to4_timer", "to4_timer uint 32", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF));
    BDMFMON_MAKE_CMD(pon_stack_dir, "sto5", "Set to5 timer", ngpon_set_to5_timer_command,
        BDMFMON_MAKE_PARM_RANGE("to5_timer", "to5_timer uint 32", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF));
    BDMFMON_MAKE_CMD(pon_stack_dir, "stoZ", "Set toZ timer", ngpon_set_toz_timer_command,
        BDMFMON_MAKE_PARM_RANGE("toz_timer", "toz_timer uint 32", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF));
    BDMFMON_MAKE_CMD(pon_stack_dir, "sdg", "Activate dying gasp sending", ngpon_send_dying_gasp_command,
        BDMFMON_MAKE_PARM_RANGE("dying_gasp_state", "dying gasp state", BDMFMON_PARM_NUMBER, 0, 0, 0x1));
#ifdef G989_3_AMD1
    BDMFMON_MAKE_CMD(pon_stack_dir, "stcpi", "Set Tcpi timer", ngpon_set_tcpi_timer_command,
        BDMFMON_MAKE_PARM_RANGE("tcpi_timer", "tcpi_timer uint 32", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF));
#endif

    BDMFMON_MAKE_CMD(pon_stack_dir, "cflnk", "Configure link parameters", ngpon_configure_link_params_command,
        BDMFMON_MAKE_PARM_RANGE("transceiver_dv_setup_pattern", "transceiver_dv_setup_pattern", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_RANGE("transceiver_dv_hold_pattern", "transceiver_dv_hold_pattern", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_RANGE("transceiver_dv_polarity", "transceiver_dv_polarity", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_ENUM("transceiver_power_calibration_mode", "transceiver_power_calibration_mode", bdmfmon_enum_bool_table, 0),
        BDMFMON_MAKE_PARM_RANGE("transceiver_power_calibration_pattern", "transceiver_power_calibration_pattern", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_RANGE("transceiver_power_calibration_size", "transceiver_power_calibration_size", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_RANGE("ber_interval", "ber_interval", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_RANGE("min_response_time", "min_response_time", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_RANGE("to1_timeout", "to1_timeout", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_RANGE("to2_timeout", "to2_timeout", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_ENUM("tx_data_polarity", "tx_data_polarity", bdmfmon_enum_bool_table, 0),
        BDMFMON_MAKE_PARM_ENUM("rx_din_polarity", "rx_din_polarity", bdmfmon_enum_bool_table, 0),
        BDMFMON_MAKE_PARM_RANGE("ber_threshold_for_sd_assertion", "ber_threshold_for_sd_assertion", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_RANGE("ber_threshold_for_sf_assertion", "ber_threshold_for_sf_assertion", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_RANGE("number_of_psyncs_for_lof_assertion", "number_of_psyncs_for_lof_assertion", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_RANGE("number_of_psyncs_for_lof_clear", "number_of_psyncs_for_lof_clear", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF));
    BDMFMON_MAKE_CMD(pon_stack_dir, "broadcast", "Set broadcst Id", ngpon_set_broadcast_command, BDMFMON_MAKE_PARM_RANGE("set_broadcast_id", "set_broadcast_id", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFF));
   
    BDMFMON_MAKE_CMD(pon_stack_dir, "dsa", "Debug Statistics Allocate", ngpon_dba_debug_statistics_allocate_command,
        BDMFMON_MAKE_PARM_RANGE("tconts_number", "Maximal number of TCONT IDs", BDMFMON_PARM_NUMBER, 0, 0, 32),
        BDMFMON_MAKE_PARM_RANGE("cycles_number", "Maximal number of cycles", BDMFMON_PARM_NUMBER, 0, 0, 100));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "dsf", "Debug Statistics Free", ngpon_dba_debug_statistics_free_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "dsc", "Debug Statistics Collect", ngpon_dba_debug_statistics_collect_command,
        BDMFMON_MAKE_PARM_RANGE("number_of_cycles", "Maximal number of cycles", BDMFMON_PARM_NUMBER, 0, 0, 100));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "dsr", "Debug Statistics Reset", ngpon_dba_debug_statistics_reset_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "dds", "Debug Statistics Dump", ngpon_dba_debug_dump_statistics_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_id", "TCONT ID or '40' for all tconts", BDMFMON_PARM_NUMBER, 0, 0, 40));
    BDMFMON_MAKE_CMD(pon_stack_dir, "glol", "Get logger object list by parent", ngpon_get_log_object_list_command,
        BDMFMON_MAKE_PARM("parent_object", "parent_object", BDMFMON_PARM_STRING, 0));
    BDMFMON_MAKE_CMD(pon_stack_dir, "slos", "Logger object set state", ngpon_set_log_object_state_command,
        BDMFMON_MAKE_PARM("parent_object", "parent_object", BDMFMON_PARM_STRING, 0),
        BDMFMON_MAKE_PARM_RANGE("state", "0 or 1", BDMFMON_PARM_UNUMBER, 0, 0, 1));
    BDMFMON_MAKE_CMD(pon_stack_dir, "cgrt", "Set counter group for tcont", ngpon_counter_group_for_tcont_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_0_id", "tcont_id for group 0: Decimal (0 - 39 )", BDMFMON_PARM_NUMBER, 0, 0, NGPON_DUMMY_TCONT_ID_VALUE),
        BDMFMON_MAKE_PARM_RANGE("tcont_1_id", "tcont_id for group 1: Decimal (0 - 39 )", BDMFMON_PARM_NUMBER, 0, 0, NGPON_DUMMY_TCONT_ID_VALUE),
        BDMFMON_MAKE_PARM_RANGE("tcont_2_id", "tcont_id for group 2: Decimal (0 - 39 )", BDMFMON_PARM_NUMBER, 0, 0, NGPON_DUMMY_TCONT_ID_VALUE),
        BDMFMON_MAKE_PARM_RANGE("tcont_3_id", "tcont_id for group 3: Decimal (0 - 39 )", BDMFMON_PARM_NUMBER, 0, 0, NGPON_DUMMY_TCONT_ID_VALUE));
    BDMFMON_MAKE_CMD(pon_stack_dir, "comcik", "Configure omci key", ngpon_configure_omci_key_command,
        BDMFMON_MAKE_PARM("omci_key", "omci key", BDMFMON_PARM_STRING, 0));
    BDMFMON_MAKE_CMD(pon_stack_dir, "setchn", "Set NG-PON2 transceiver RX and TX channels ", ngpon_set_chn_ng_pon2_trx,
        BDMFMON_MAKE_PARM_RANGE("rx_chn_id", "Channel ID (1..4)", BDMFMON_PARM_UNUMBER, 1, 1, 4),
        BDMFMON_MAKE_PARM_RANGE("rx_chn_id", "Channel ID (1..4)", BDMFMON_PARM_UNUMBER, 1, 1, 4));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "getchn", "Get RX and TX channel IDs from NG-PON2 transceiver", ngpon_get_chn_ng_pon2_trx);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "psyspr", "Print System Profile fields", ngpon_print_sys_prof);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "pchnpr", "Print Channel Profile fields", ngpon_print_chan_profs);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "pbpr", "Print Burst Profile fields", ngpon_print_burst_profs);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "ptmr", "Print timer periods (in mks)", ngpon_print_timers);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "pwlcp", "Print Wavelength Channel Protection configuration", ngpon_print_wlcp);
    BDMFMON_MAKE_CMD(pon_stack_dir, "szeqd", "Zero EqD on NGPON2 ONU Tuning", ngpon_set_zero_eqd_flag,
        BDMFMON_MAKE_PARM_RANGE("zero_eqd_flag", "0 - migrate with same EqD 1 - zero EqD on migration", BDMFMON_PARM_UNUMBER, 0, 0, 1));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "pzeqd", "Print 'Zero EqD on migration' flag", ngpon_print_zeqd);
    BDMFMON_MAKE_CMD(pon_stack_dir, "ssr", "Set Simplified Rollback flag for NGPON2 tuning", ngpon_set_simplified_rollback_flag,
        BDMFMON_MAKE_PARM_RANGE("simplified_rollback_flag", "0 - go to O9 in rollback 1 - go to O9 in rollback", BDMFMON_PARM_UNUMBER, 0, 0, 1));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "pssr", "Print 'Simplified Rollback' flag", ngpon_print_simplified_rollback_flag);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gppc", "Get PLOAM pm counters", ngpon_get_ploam_pm_counters_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gtpc", "Get tuning pm counters", ngpon_get_tuning_pm_counters_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "glpc", "Get LODS pm counters", ngpon_get_lods_pm_counters_command);

    /* Initialize driver shell */
    gpon_drv_dir = bdmfmon_dir_add(driver_dir, "gpon_drv", "GPON drivers", BDMF_ACCESS_GUEST, NULL);
    if (!gpon_drv_dir)
    {
        bdmf_session_print(NULL, "Can't create gpon_drv_dir directory\n");
        return;
    }

}

void ngpon_exit_drv_pon_stack_shell(void)
{
    if (pon_stack_dir)
    {
        bdmfmon_token_destroy(pon_stack_dir);
        pon_stack_dir = NULL;
    }

    if (gpon_drv_dir)
    {
        bdmfmon_token_destroy(gpon_drv_dir);
        gpon_drv_dir = NULL;
    }
}


static char *gpon_stack_gpon_error_code_to_string(PON_ERROR_DTE error_code)
{
    switch (error_code)
    {
    case PON_NO_ERROR:
        return "PON_NO_ERROR";
        break;
    case PON_GENERAL_ERROR:
        return "PON_GENERAL_ERROR";
        break;
    case PON_ERROR_INVALID_PARAMETER:
        return "PON_ERROR_INVALID_PARAMETER";
        break;
    case PON_ERROR_SW_NOT_INITIALIZED:
        return "PON_ERROR_SW_NOT_INITIALIZED";
        break;
    case PON_ERROR_SW_ALREADY_INITIALIZED:
        return "PON_ERROR_SW_ALREADY_INITIALIZED";
        break;
    case PON_ERROR_SW_ALREADY_TERMINATED:
        return "PON_ERROR_SW_ALREADY_TERMINATED";
        break;
    case PON_ERROR_GENERAL_ERROR:
        return "PON_ERROR_GENERAL_ERROR";
        break;
    case PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR:
        return "PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR";
        break;
    case PON_ERROR_INTERNAL_DB_ERROR:
        return "PON_ERROR_INTERNAL_DB_ERROR";
        break;
    case PON_ERROR_TIME_OUT_FLUSH_IS_NOT_DONE:
        return "PON_ERROR_TIME_OUT_FLUSH_IS_NOT_DONE";
        break;
    case PON_HW_TIMER_IS_NOT_INITIALIZED:
        return "PON_HW_TIMER_IS_NOT_INITIALIZED";
        break;
    case PON_DBR_ALREADY_ENABLED:
        return "PON_DBR_ALREADY_ENABLED";
        break;
    case PON_DBR_NOT_ENABLED:
        return "PON_DBR_NOT_ENABLED";
        break;
    case PON_ERROR_NOT_ENOUGH_MEMORY:
        return "PON_ERROR_NOT_ENOUGH_MEMORY";
        break;
    case PON_ERROR_INVALID_OBJECT:
        return "PON_ERROR_INVALID_OBJECT";
        break;
    case PON_ERROR_DRIVER_ERROR:
        return "PON_ERROR_DRIVER_ERROR";
        break;
    case PON_ERROR_OS_ERROR:
        return "PON_ERROR_OS_ERROR";
        break;
    case PON_ERROR_INVALID_STATE:
        return "PON_ERROR_INVALID_STATE";
        break;
    case PON_ERROR_LINK_NOT_CREATED:
        return "PON_ERROR_LINK_NOT_CREATED";
        break;
    case PON_ERROR_INVALID_INFO:
        return "PON_ERROR_INVALID_INFO";
        break;
    case PON_ERROR_INVALID_SYSCLK_TIMER_ID:
        return "PON_ERROR_INVALID_SYSCLK_TIMER_ID";
        break;
    case PON_ERROR_LINK_ALREADY_ACTIVE:
        return "PON_ERROR_LINK_ALREADY_ACTIVE";
        break;
    case PON_ERROR_LINK_ALREADY_INACTIVE:
        return "PON_ERROR_LINK_ALREADY_INACTIVE";
        break;
    case PON_ERROR_LINK_NOT_ACTIVE:
        return "PON_ERROR_LINK_NOT_ACTIVE";
        break;
    case PON_ERROR_LINK_NOT_INACTIVE:
        return "PON_ERROR_LINK_NOT_INACTIVE";
        break;
    case PON_ERROR_OUT_OF_RESOURCES:
        return "PON_ERROR_OUT_OF_RESOURCES";
        break;
    case PON_ERROR_QUEUE_ILLEGAL_ID:
        return "PON_ERROR_QUEUE_ILLEGAL_ID";
        break;
    case PON_ERROR_QUEUE_DISABLED:
        return "PON_ERROR_QUEUE_DISABLED";
        break;
    case PON_ERROR_QUEUE_FULL:
        return "PON_ERROR_QUEUE_FULL";
        break;
    case PON_ERROR_QUEUE_EMPTY:
        return "PON_ERROR_QUEUE_EMPTY";
        break;
    case PON_ERROR_TCONT_NOT_ASSIGNED:
        return "PON_ERROR_TCONT_NOT_ASSIGNED";
        break;
    case PON_ERROR_ALLOC_ALREADY_ASSIGN_TO_ANOTHER_TCONT:
        return "PON_ERROR_ALLOC_ALREADY_ASSIGN_TO_ANOTHER_TCONT";
        break;
    case PON_ERROR_TCONT_TABLE_IS_FULL:
        return "PON_ERROR_TCONT_TABLE_IS_FULL";
        break;
    case PON_ERROR_ALLOC_ALREADY_ASSIGN:
        return "PON_ERROR_ALLOC_ALREADY_ASSIGN";
        break;
    case PON_ERROR_ALLOC_NOT_ASSIGNED:
        return "PON_ERROR_ALLOC_NOT_ASSIGNED";
        break;
    case PON_ERROR_ALLOC_NOT_IN_RANGE:
        return "PON_ERROR_ALLOC_NOT_IN_RANGE";
        break;
    case PON_ERROR_GRANT_ILLEGAL_ID:
        return "PON_ERROR_GRANT_ILLEGAL_ID";
        break;
    case PON_ERROR_GRANT_INVALID_TYPE:
        return "PON_ERROR_GRANT_INVALID_TYPE";
        break;
    case PON_ERROR_PORT_ID_ALREADY_CFG:
        return "PON_ERROR_PORT_ID_ALREADY_CFG";
        break;
    case PON_ERROR_TCONT_IS_ALREADY_CFG:
        return "PON_ERROR_TCONT_IS_ALREADY_CFG";
        break;
    case PON_ERROR_OPERATION_TIMEOUT:
        return "PON_ERROR_OPERATION_TIMEOUT";
        break;
    case PON_ERROR_QUEUE_CREATE:
        return "PON_ERROR_QUEUE_CREATE";
        break;
    case PON_ERROR_QUEUE_SEND:
        return "PON_ERROR_QUEUE_SEND";
        break;
    case PON_ERROR_WATCHDOG_START:
        return "PON_ERROR_WATCHDOG_START";
        break;
    case PON_IC_ERROR:
        return "PON_IC_ERROR";
        break;
    case PON_FE_ERROR:
        return "PON_FE_ERROR";
        break;
    case PON_RT_HW_TIMER_ERROR:
        return "RT_HW_TIMER_ERROR";
        break;
    default:
        return "unknown";
        break;
    }
}

int32_t translate_string_to_binary(char *xi_string, uint8_t *xo_buffer, uint32_t xi_max_length )
{
    char * read;
    uint8_t * write;
    uint8_t byte;

    /* Check string length */
    if (strlen(xi_string ) < xi_max_length * 2 )
    {
        /* String is too short */
        return(-1);
    }

    /* Check string values */
    read = xi_string;
    while(( * read ) &&(read - xi_string < strlen(xi_string ) ) )
    {
        /* Convert to uppercase and make sure this is a hex digit */
        if (! isxdigit((int)( * read ) ) )
        {
            /* Invalid string */
            return(-2);
        }
        * read = toupper(* read);
        read ++;
    }

    /* Convert string */
    read = xi_string;
    write = xo_buffer;
    while(* read &&(write - xo_buffer < xi_max_length ) )
    {
        byte =(isdigit((int)( * read ) ) ? * read - '0' : * read - 'A' + 0x0A);
        byte <<= 4;
        read ++;
        byte +=(isdigit((int)( * read ) ) ? * read - '0' : * read - 'A' + 0x0A);
        read ++;
        * write ++ = byte;
    }

    /* Return calculated length */
    return(write - xo_buffer);
}

#endif



