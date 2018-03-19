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
#include "bdmf_shell.h"
#include "gpon_db.h"
#include "gpon_stat.h"
#include "gpon_general.h"
#include "gpon_logger.h"

#define BDMF_SESSION_DISABLE_FORMAT_CHECK
#define GPON_DBR_USER_RESOLUTION 64 

#define INTERRUPT_INACTIVE_LEVEL LOW 
#define INTERRUPT_ACTIVE_LEVEL HIGH

extern GPON_DATABASE_DTS gs_gpon_database;

#ifdef DBA_DEBUG_STATISTICS
extern GPON_DBA_DEBUG_STATISTICS_DTE *ge_dba_debug_statistics;
extern bool ge_dba_debug_statistics_collect;
extern bool ge_dba_debug_statistics_warning_not_all_tconts_were_collected;
extern uint32_t ge_dba_debug_statistics_max_number_of_cycles;
extern uint32_t ge_dba_debug_statistics_max_number_of_tconts;
extern uint32_t ge_dba_debug_statistics_cycle_counter;
extern uint32_t ge_dba_debug_statistics_number_of_cycles;
#endif 

#ifdef USE_BDMF_SHELL

static char *gpon_stack_gpon_error_code_to_string(PON_ERROR_DTE error_code);
int32_t f_translate_string_to_binary(char *xi_string, uint8_t *xo_buffer, uint32_t xi_max_length);

/******************************************************************************/
/*                                                                            */
/*  Static functions prototypes                                               */
/*                                                                            */
/******************************************************************************/

static struct bdmfmon_enum_val activation_state_names_enum_table[] = {
    {"INIT (O1)", 0},
    {"STANDBY (O2)", 1},
    {"SERIAL_NUMBER (O3)", 2},
    {"RANGING (O4)", 3},
    {"OPERATION (O5)", 4},
    {"POPUP (O6)", 5},
    {"EMERGENCY_STOP (O7)", 6}
};

static struct bdmfmon_enum_val link_sub_state_names_enum_table[] = {
    {"NONE", 0},
    {"STANDBY", 1},
    {"OPERATION", 2},
    {"STOP", 3}
};

static struct bdmfmon_enum_val link_state_names_enum_table[] = {
    {"READY", 0},
    {"INACTIVE", 1},
    {"ACTIVE", 2}
};

static char *gs_link_state_names[LINK_SUB_STATE_STOP + 1] =
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

static char *gs_activation_state_names[OPERATION_STATES_NUMBER] =
{
    "INIT (O1)",
    "STANDBY (O2)",
    "SERIAL_NUMBER (O3)",
    "RANGING (O4)",
    "OPERATION (O5)",
    "POPUP (O6)",
    "EMERGENCY_STOP (O7)"
};

static int p_gpon_software_initialize_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	PON_ERROR_DTE error ;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_software_initialize_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_software_initialize ( gs_gpon_database.os_resources.pon_task_priority );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_software_terminate_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	PON_ERROR_DTE error ;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_software_terminate_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_software_terminate ();
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_configure_onu_serial_number_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_configure_onu_serial_number_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_configure_onu_serial_number ( serial_number );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_configure_onu_password_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	PON_PASSWORD_DTE  onu_password ;
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;
	uint32_t i ;

	/* Extract password (10 bytes) */
	i = f_translate_string_to_binary ( parm[parameter_index++].value.string, ( char * ) & onu_password, PON_PASSWORD_FIELD_SIZE );
	if ( i != PON_PASSWORD_FIELD_SIZE )
	{
		bdmf_session_print(session, "password should contain %d hex bytes (%d)\n\r", PON_PASSWORD_FIELD_SIZE, i );
		return BDMF_ERR_PARM;
	}

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_configure_onu_password_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_configure_onu_password ( onu_password );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_link_reset_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	PON_ERROR_DTE error ;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_link_reset_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_link_reset ( );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_link_activate_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	bool initial_state_disable = BDMF_FALSE ;
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;

	/* Define parameters handling variables */
	initial_state_disable = parm[parameter_index++].value.unumber;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_link_activate_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_link_activate ( initial_state_disable );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_link_deactivate_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	PON_ERROR_DTE error ;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_link_deactivate_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_link_deactivate ( );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_get_link_parameters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	rdpa_gpon_link_cfg_t user_link_parameters ; /* output */
	PON_ERROR_DTE error ;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_link_parameters_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_get_link_parameters ( & user_link_parameters );
	/* -------------------------------------------------------------------- */
	if ( error == PON_NO_ERROR )
	{
		bdmf_session_print(session, "transceiver_dv_setup_pattern           %d \n", user_link_parameters.transceiver_dv_setup_pattern );
		bdmf_session_print(session, "transceiver_dv_hold_pattern            %d \n", user_link_parameters.transceiver_dv_hold_pattern );
		bdmf_session_print(session, "transceiver_dv_polarity                %d \n", user_link_parameters.transceiver_dv_polarity );
		bdmf_session_print(session, "transceiver_power_calibration_mode     %s \n", ( user_link_parameters.transceiver_power_calibration_mode == BDMF_TRUE ? "enable" : "disable" ) );
		bdmf_session_print(session, "transceiver_power_calibration_pattern  0x%08x \n",user_link_parameters.transceiver_power_calibration_pattern );
		bdmf_session_print(session, "transceiver_power_calibration_size     %d \n",user_link_parameters.transceiver_power_calibration_size );
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

static int p_gpon_get_link_state_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    LINK_STATE_DTE state; /* output */
    LINK_SUB_STATE_DTE sub_state; /* output */
    LINK_OPERATION_STATES_DTE operational_state; /* output */
    PON_ERROR_DTE error;
    
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_link_state_command'\n\r" );
    /* -------------------------------------------------------------------- */
    error = gpon_get_link_status(&state, &sub_state, &operational_state);
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

static int p_gpon_get_software_state_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	GPON_SW_STATUS_DTE state ; /* output */
	PON_ERROR_DTE error ;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_software_state_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_get_software_status ( & state );
	/* -------------------------------------------------------------------- */
	if ( error == PON_NO_ERROR )
		bdmf_session_print(session, "  State: %s\n", (state == BDMF_FALSE ? "INITIALIZED" : "TERMINATED"));


	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_set_txfifo_queue_configuration_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	uint8_t  packet_descriptor_id ;
	uint8_t  txfifo_id ;
	uint8_t  packet_descriptor_queue_base_address ;
	uint8_t  packet_descriptor_queue_size ;
	uint16_t tx_queue_offset ;
	uint16_t tx_max_queue_size ;
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;

	/* Define parameters handling variables */
	packet_descriptor_id = parm[parameter_index++].value.unumber;
	txfifo_id = parm[parameter_index++].value.unumber;
	packet_descriptor_queue_base_address = parm[parameter_index++].value.unumber;
	packet_descriptor_queue_size = parm[parameter_index++].value.unumber;
	tx_queue_offset = parm[parameter_index++].value.unumber;
	tx_max_queue_size = parm[parameter_index++].value.unumber;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_set_txfifo_queue_configuration_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_set_txfifo_queue_configuration ( packet_descriptor_id ,txfifo_id,
			packet_descriptor_queue_base_address, packet_descriptor_queue_size, tx_queue_offset, tx_max_queue_size );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_get_txfifo_queue_configuration_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	PON_FIFO_ID_DTE fifo_id; /* input */
	uint16_t packet_descriptor_queue_base_address = 0;
	uint16_t packet_descriptor_queue_size = 0;
	uint16_t tx_queue_offset = 0;
	uint16_t tx_queue_size = 0;
	uint8_t parameter_index = 0;
	PON_ERROR_DTE error;

        /* Define parameters handling variables */
	fifo_id = parm[parameter_index++].value.unumber;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_txfifo_queue_configuration_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_get_txfifo_queue_configuration( fifo_id, fifo_id, & packet_descriptor_queue_base_address,
			& packet_descriptor_queue_size, & tx_queue_offset, & tx_queue_size );
	/* -------------------------------------------------------------------- */
	if ( error == PON_NO_ERROR )
	{
		bdmf_session_print(session, "packet_descriptor_id                 [%d]\n\r", fifo_id );
		bdmf_session_print(session, "fifo_id                              [%d]\n\r", fifo_id );
		bdmf_session_print(session, "packet_descriptor_queue_base_address [%d]\n\r", packet_descriptor_queue_base_address );
		bdmf_session_print(session, "packet_descriptor_queue_size         [%d]\n\r", packet_descriptor_queue_size );
		bdmf_session_print(session, "tx_queue_offset                      [%d]\n\r", tx_queue_offset );
		bdmf_session_print(session, "tx_queue_size                        [%d]\n\r", tx_queue_size );
	}

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_get_txfifo_queue_configuration_group_0_to_3_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_DTE  group_id; /* Input  */
	uint16_t packet_descriptor_group_base_address ; /* Output */
	uint16_t group_offset ; /* Output */
	uint16_t group_size ; /* Output */
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;

	/* Define parameters handling variables */
	group_id = parm[parameter_index++].value.unumber;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_txfifo_queue_configuration_group_0_to_3_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_get_txfifo_queue_configuration_group_0_to_3( group_id,
			& packet_descriptor_group_base_address, & group_offset, & group_size );
	/* -------------------------------------------------------------------- */
	if ( error == PON_NO_ERROR )
	{
		bdmf_session_print(session, "group_id -  %d \n\r", group_id );
		bdmf_session_print(session, "packet_descriptor_group_base_address -  %d \n\r",
				packet_descriptor_group_base_address );
		bdmf_session_print(session, "group_offset -  %d \n\r", group_offset );
		bdmf_session_print(session, "group_size -  %d \n\r", group_size );
	}

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_get_rxpon_pm_counters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	GPON_RXPON_PM_COUNTERS_DTE rxpon_pm_counters;
	PON_ERROR_DTE error ;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_rxpon_pm_counters_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error  = gpon_get_rx_pm_counters ( & rxpon_pm_counters );
	/* -------------------------------------------------------------------- */
	if ( error == PON_NO_ERROR )
	{
		bdmf_session_print(session, "bw_map_error_counter         %d\n"
				"ploam_crc_error_counter      %d\n"
				"plend_crc_error_counter      %d\n"
				"superframe_mismatch_counter  %d\n"
				"bip_error_counter            %d\n"
				"gem_fragments_counter        %d\n"
				"eth_fragments_counter        %d\n"
				"fec_corrected_bytes_counter  %d\n"
				"fec_corrected_bits_counter   %d\n"
				"fec_corrected_codeword_counter %d\n",
				rxpon_pm_counters.bw_map_error_counter,
				rxpon_pm_counters.ploam_crc_error_counter,
				rxpon_pm_counters.plend_crc_error_counter,
				rxpon_pm_counters.superframe_mismatch_counter,
				rxpon_pm_counters.bip_error_counter,
				rxpon_pm_counters.gem_fragments_counter,
				rxpon_pm_counters.eth_fragments_counter,
				rxpon_pm_counters.fec_corrected_bytes_counter,
				rxpon_pm_counters.fec_corrected_bits_counter,
				rxpon_pm_counters.fec_corrected_codeword_counter );
	}

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_get_rxpon_ploam_counters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	PON_RX_PLOAM_COUNTERS_DTE rxpon_ploam_counters;
	PON_ERROR_DTE error ;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_rxpon_ploam_counters_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error  = gpon_get_rx_ploam_counters ( & rxpon_ploam_counters );
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


static int p_gpon_set_rxpon_bw_alloc_association_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	PON_ALLOC_ID_DTE alloc_1 ;
	PON_ALLOC_ID_DTE alloc_2 ;
	PON_ALLOC_ID_DTE alloc_3 ;
	PON_ALLOC_ID_DTE alloc_4 ;
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;

	/* Define parameters handling variables */
	alloc_1 = parm[parameter_index++].value.unumber;
	alloc_2 = parm[parameter_index++].value.unumber;
	alloc_3 = parm[parameter_index++].value.unumber;
	alloc_4 = parm[parameter_index++].value.unumber;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_set_rxpon_bw_alloc_association_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error  = gpon_set_rxpon_bw_alloc_association ( alloc_1, alloc_2, alloc_3, alloc_4 );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_get_rxpon_bw_alloc_association_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	PON_ALLOC_ID_DTE alloc_1 ;
    PON_ALLOC_ID_DTE alloc_2 ;
    PON_ALLOC_ID_DTE alloc_3 ;
    PON_ALLOC_ID_DTE alloc_4 ;
	PON_ERROR_DTE error ;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_rxpon_bw_alloc_association_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_get_rx_bw_alloc_association ( &alloc_1, &alloc_2, &alloc_3, &alloc_4 );
	/* -------------------------------------------------------------------- */
	if ( error == PON_NO_ERROR )
	{
		bdmf_session_print(session, "RXPON BW allocation counter association  \n"
				"Counter 1 associated to alloc  %d \n"
				"Counter 2 associated to alloc  %d \n"
				"Counter 3 associated to alloc  %d \n"
				"Counter 4 associated to alloc  %d \n",
				alloc_1,
				alloc_2,
				alloc_3,
				alloc_4 );
	}

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_get_rxpon_bw_alloc_counter_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	PON_ALLOC_ID_DTE alloc_id ;
	uint32_t bw_allocation_counter;
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;

	/* Define parameters handling variables */
	alloc_id = parm[parameter_index++].value.unumber;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_rxpon_bw_alloc_counter_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_get_bw_alloc_counter ( alloc_id, &bw_allocation_counter );
	/* -------------------------------------------------------------------- */
	if ( error == PON_NO_ERROR )
	{
		bdmf_session_print(session, "RXPON BW allocation counter for Alloc-id %d = %d  \n",
				alloc_id,
				bw_allocation_counter);
	}

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_get_tcont_fe_queue_pd_counter_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	PON_ALLOC_ID_DTE tcont_id ;
	uint16_t pd_counter;
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;

	/* Define parameters handling variables */
	tcont_id = parm[parameter_index++].value.unumber;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_tcont_fe_queue_pd_counter_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_get_tcont_fe_queue_pd_counter ( tcont_id, &pd_counter );
	/* -------------------------------------------------------------------- */
	if ( error == PON_NO_ERROR )
	{
		bdmf_session_print(session, "PD counter for Tcont id %d Front-End = %d  \n",
				tcont_id,
				pd_counter);
	}

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_get_txpon_pm_counters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    uint16_t illegal_access_counter;
    uint32_t idle_ploam_counter;
    uint32_t normal_ploam_counter;
    uint16_t requested_ranging_ploam_number;
    uint16_t requested_urgent_ploam_number;
    PON_ERROR_DTE error;
    
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_txpon_pm_counters_command'\n\r" );
    /* -------------------------------------------------------------------- */
    error = gpon_get_tx_pm_counters ( & illegal_access_counter, & idle_ploam_counter,
    		& normal_ploam_counter, & requested_ranging_ploam_number, & requested_urgent_ploam_number );
    /* -------------------------------------------------------------------- */
    if (error == PON_NO_ERROR)
    {
        bdmf_session_print(session, "illegal_access_counter          %d \n"
           "idle_ploam_counter              %d \n"
           "normal_ploam_counter            %d \n"
           "requested_ranging_ploam_number  %d \n"
           "requested_urgent_ploam_number   %d \n",
           illegal_access_counter,
           idle_ploam_counter,
           normal_ploam_counter,
           requested_ranging_ploam_number,
           requested_urgent_ploam_number );
    }
    
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );
    
    return error ? BDMF_ERR_PARM : 0;
}

static int p_gpon_get_tcont_pm_counters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    error = gpon_get_tcont_queue_pm_counters ( fifo_id, & transmitted_idle_counter, & transmitted_gem_counter,
    		& transmitted_packet_counter, & requested_dbr_counter, & valid_access_counter );
    /* -------------------------------------------------------------------- */
    if ( error == PON_NO_ERROR )
    {
        bdmf_session_print(session, "fifo_id                    %d\n"
    	    "transmitted_idle_counter   %d\n"
    	    "transmitted_gem_counter    %d\n"
    	    "transmitted_packet_counter %d\n"
    	    "requested_dbr_counter      %d\n"
    	    "valid_access_counter       %d\n",
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

static int p_gpon_control_port_id_filter_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
	error = gpon_control_port_id_filter ( flow_id, filter_status );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_configure_port_id_filter_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    bdmf_session_print(session, "UT: Invoking function 'p_gpon_configure_port_id_filter_command'\n\r" );
    /* -------------------------------------------------------------------- */
    error = gpon_configure_port_id_filter(flow_id, port_id_filter,
        port_id_mask, encryption_mode, flow_type, flow_priority, true);
    /* -------------------------------------------------------------------- */
    
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string(error));
    
    return error ? BDMF_ERR_PARM : 0;
}

static int p_gpon_modify_encryption_port_id_filter_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_modify_encryption_port_id_filter_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_modify_encryption_port_id_filter ( flow_id,
            encryption_mode );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_get_port_id_filter_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	PON_FLOW_ID_DTE flow_id ;
	bool filter_status ;
	PON_PORT_ID_DTE  port_id_filter ;
	PON_PORT_ID_DTE  port_id_mask ;
	bool encryption_mode ;
	PON_FLOW_TYPE_DTE flow_type ;
	PON_FLOW_PRIORITY_DTE flow_priority ;
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;
	bool crc;

	/* Define parameters handling variables */
	flow_id = parm[parameter_index++].value.unumber;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_port_id_filter_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_get_port_id_configuration ( flow_id, & filter_status,
            & port_id_filter,& port_id_mask, & encryption_mode, & flow_type, & flow_priority, &crc );
	/* -------------------------------------------------------------------- */
	if ( error == PON_NO_ERROR )
	{
		bdmf_session_print(session, "flow_id:         %d \n"
				"filter_status:   %d \n"
				"port_id_filter:  %d \n"
				"port_id_mask:    %d \n"
				"encryption_mode: %d \n"
				"flow_type:       %d \n"
				"flow_priority:   %d \n",
				flow_id,
				filter_status,
				port_id_filter,
				port_id_mask,
				encryption_mode,
				flow_type,
				flow_priority );
	}

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_configure_tcont_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_configure_tcont_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_configure_tcont ( tcont_queue_id, alloc_id );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_get_tcont_params_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	PON_TCONT_ID_DTE tcont_queue_id ;
	PON_TCONT_DTE tcont_cfg;
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;

	/* Define parameters handling variables */
	tcont_queue_id = parm[parameter_index++].value.unumber;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_tcont_params_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_get_tcont_configuration ( tcont_queue_id, & tcont_cfg );
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

static int p_gpon_modify_tcont_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_modify_tcont_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_modify_tcont ( tcont_queue_id, new_alloc_id );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_remove_tcont_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	PON_TCONT_ID_DTE tcont_queue_id ;
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;

	/* Define parameters handling variables */
	tcont_queue_id = parm[parameter_index++].value.unumber;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_remove_tcont_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_remove_tcont ( tcont_queue_id );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_print_tcont_table_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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

	for ( tcont_index = 0 ; tcont_index < MAX_NUMBER_OF_TCONTS ; tcont_index++ )
	{
		if ( gs_gpon_database.tconts_table [ tcont_index ].tcont_id == CS_DEFAULT_TCONT_ID )
		{
			bdmf_session_print(session,  "\n|NONE    |" );
		}
		else
		{
			bdmf_session_print(session, "\n|%-8d|", gs_gpon_database.tconts_table [ tcont_index ].tcont_id );
		}

		if ( gs_gpon_database.tconts_table [ tcont_index ].alloc_id == PON_ALLOC_ID_DTE_NOT_ASSIGNED )
		{
			bdmf_session_print(session,  "NONE   |" );
		}
		else
		{
			bdmf_session_print(session, "%-7d|", gs_gpon_database.tconts_table [ tcont_index ].alloc_id );
		}
		bdmf_session_print(session, "%-14d|%-17d|" ,
				gs_gpon_database.tconts_table [ tcont_index ].assign_alloc_valid_flag ,
				gs_gpon_database.tconts_table [ tcont_index ].config_tcont_valid_flag );
	}

	bdmf_session_print(session, "\n|-----------------------------------------------------------------------------------------------------|\n" );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_init_dba_sr_process_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	PON_ERROR_DTE error ;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_init_dba_sr_process_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_dba_sr_process_initialize ( GPON_DBR_USER_RESOLUTION, 2000 );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_terminate_dba_sr_process_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	PON_ERROR_DTE error ;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_terminate_dba_sr_process_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_dba_sr_process_terminate ( );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_get_bip8_counters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	uint32_t bip_count ;
	GPON_SW_STATUS_DTE state ;
	PON_ERROR_DTE error ;

	error = gpon_get_software_status(&state);
	if ( error == PON_NO_ERROR )
		goto end;

	/* Varify that the stack is active */
	if ( state != GPON_STATE_INITIALIZED )
	{
		bdmf_session_print(session, "software is not initialized\n" );
		return ( PON_ERROR_SW_NOT_INITIALIZED );
	}

	/* Call the under test routine */
	bdmf_session_print(session, "Invoking function 'p_read_bip8_error_counters'\n" );

	/* -------------------------------------------------------------------- */
	p_read_bip8_error_counters ( & bip_count, CS_USER_TYPE );
	/* -------------------------------------------------------------------- */

	bdmf_session_print(session, "bip8 errors = %d\n", bip_count );

end:
	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_start_bw_recording_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_start_bw_recording_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_start_bw_recording ( recocd_stop, record_type, alloc_id );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_stop_bw_recording_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    PON_ERROR_DTE error;
        
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'p_gpon_stop_bw_recording_command'\n\r" );
    /* -------------------------------------------------------------------- */
    error = gpon_stop_bw_recording();
    /* -------------------------------------------------------------------- */
    
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string(error));
    
    return error ? BDMF_ERR_PARM : 0;
}

static int p_gpon_get_bw_recording_data_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    GPON_ACCESS accesses_array[GPON_NUMBER_OF_ACCESS_IN_MAP]; /* output */
    int access_read_num; /* output */
    uint8_t i;
    PON_ERROR_DTE error;
    
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_bw_recording_data_command'\n\r");
    /* -------------------------------------------------------------------- */
    error = gpon_get_bw_recording_data(accesses_array, &access_read_num);
    /* -------------------------------------------------------------------- */
    if (error == PON_NO_ERROR)
    {
    	bdmf_session_print(session, "%d accesses Read. Recorded Data: \n\n", access_read_num);
        for (i = 0; i < access_read_num; i++)
        {
            bdmf_session_print(session, "BW recording Data index: %d\n\n\r", i) ;
            bdmf_session_print(session, "----------------------------\n\n\r") ;
            bdmf_session_print(session, "rcd_data [Hex]: %08x%08x   \n\n\r", 
                accesses_array[i].data.upper, accesses_array[i].data.lower);
            
            bdmf_session_print(session, "Alloc Id:       %d\n\r", accesses_array[i].map_fields.alloc_id);
            bdmf_session_print(session, "SSTART:         %d\n\r", accesses_array[i].map_fields.sstart);
            bdmf_session_print(session, "SSTOP:          %d\n\r", accesses_array[i].map_fields.sstop);
            bdmf_session_print(session, "SF counter LSB: %d\n\r", accesses_array[i].map_fields.sf_counter);
            bdmf_session_print(session, "Flags Fields:     \n\r");
            bdmf_session_print(session, "PLS:            %d\n\r", accesses_array[i].map_fields.flag_pls);
            bdmf_session_print(session, "Ploam:          %d\n\r", accesses_array[i].map_fields.flag_ploam);
            bdmf_session_print(session, "FEC:            %d\n\r", accesses_array[i].map_fields.flag_fec);
            bdmf_session_print(session, "DBRu:           %d\n\r", accesses_array[i].map_fields.flag_dbru);
            bdmf_session_print(session, "CRC:            %s\n\r", 
                (accesses_array[i].map_fields.crc == 1)? "CRC Valid" : "CRC Error");
        }
    }
    
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string(error));
    
    return error ? BDMF_ERR_PARM : 0;
}

static int p_gpon_set_rogue_onu_detection_params_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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

    /* Enable interrupts to get the indication */
    gs_gpon_database.tx_ier_vector.rogue_onu_level = BDMF_TRUE;
    gs_gpon_database.tx_ier_vector.rogue_onu_diff = BDMF_TRUE;
    gpon_cfg_rogue_onu_interrupts(ROGUE_ONU_TX_INT_UNMASK, 1, 1);
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'p_gpon_set_rogue_onu_detection_params_command'\n\r" );
    /* -------------------------------------------------------------------- */
    error = gpon_set_rogue_onu_detection_params ( & rogue_onu_parameters );
    /* -------------------------------------------------------------------- */

    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

    return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_set_dbru_report_parameters_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	bool  update_mode; /* input */
	uint8_t    gem_block_size; /* input */
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;

	/* Define parameters handling variables */
	update_mode = parm[parameter_index++].value.unumber;
	gem_block_size = parm[parameter_index++].value.unumber;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_set_dbru_report_parameters_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_set_dbru_report_parameters ( update_mode, gem_block_size );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_set_loopback_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	bool loopback_status ; /* input */
	GPON_QUEUE_ID_DTE loopback_queue ; /* input */
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;

	/* Define parameters handling variables */
	loopback_status = parm[parameter_index++].value.unumber;
	loopback_queue = parm[parameter_index++].value.unumber;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_set_loopback_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_set_loopback ( loopback_status, loopback_queue );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_flush_queue_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	PON_ERROR_DTE error ;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_flush_queue_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_flush_queue ( );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_send_ploam_message_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	GPON_PLOAM_TRANSMISSION_NUMBER repetition ; /* input */
	uint8_t priority ; /* input */
	uint8_t message_id ; /* input */
	uint8_t *ploam_buffer ; /* input */
	GPON_US_PLOAM ploam_msg ; /* input */
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;
	uint32_t i ;

	/* Define parameters handling variables */
	repetition = parm[parameter_index++].value.unumber;
	message_id = parm[parameter_index++].value.unumber;

	/* Extract ploam data (10 bytes) */
	i = f_translate_string_to_binary ( parm[parameter_index++].value.string, (uint8_t *) &ploam_msg.message, CE_PLOAM_DATA_SIZE );
	if ( i != CE_PLOAM_DATA_SIZE )
	{
		bdmf_session_print(session, "Ploam message size ( %d ) doesn''t match actual buffer size ( %d )\n\r", CE_PLOAM_DATA_SIZE, i );
		return BDMF_ERR_PARM;
	}

	priority = parm[parameter_index++].value.unumber;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_send_ploam_message_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_send_ploam_message ( &ploam_msg, message_id, repetition, priority );
	/* -------------------------------------------------------------------- */

	if ( error == PON_NO_ERROR )
	{
		ploam_buffer = (uint8_t *)&ploam_msg.message;
		bdmf_session_print(session, "ploam_msg - : \n\r");
		for (i = 0; i < CE_PLOAM_DATA_SIZE; ++i)
			bdmf_session_print(session, "%02X ", ploam_buffer[i] );

		bdmf_session_print(session, "\n\r");

	}

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_get_onu_information_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */     
    uint32_t i;                   
    PON_SERIAL_NUMBER_DTE serial_number;
    PON_PASSWORD_DTE password;
    uint16_t onu_id;
    uint32_t equalization_delay;
    uint32_t downstream_ber_interval;
    PON_TX_OVERHEAD_DTE pon_overhead;
    uint8_t pon_overhead_length;
    uint8_t pon_overhead_repetition_length;
    uint8_t pon_overhead_repetition;
    uint8_t transceiver_power_level;
    PON_ERROR_DTE error;
    
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_onu_information_command'\n\r" );
    /* -------------------------------------------------------------------- */
    error = gpon_get_onu_information ( & serial_number, & password, & onu_id, & equalization_delay,
    		& downstream_ber_interval, & pon_overhead, & pon_overhead_length,
    		& pon_overhead_repetition_length, & pon_overhead_repetition, & transceiver_power_level );
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

static int p_gpon_get_equalization_delay_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	uint32_t equalization_delay = 0 ;
	PON_ERROR_DTE error ;

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_equalization_delay_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_get_equalization_delay ( & equalization_delay );
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

static int p_gpon_isr_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	/* Define I/O variables */
	 GPON_DS_PLOAM ds_ploam ;
	 PON_INTERRUPTS_MASK_DTE interrupt_vector ;
	uint8_t parameter_index = 0 ;
	PON_ERROR_DTE error ;

	/* Get the parameters */
	switch ( parm[parameter_index++].value.unumber )
	{
	case 0 : /* rx */
		interrupt_vector.rxpon_interrupt = INTERRUPT_ACTIVE_LEVEL ;
		interrupt_vector.txpon_interrupt = INTERRUPT_INACTIVE_LEVEL ;
		break ;
	case 1 : /* tx */
		interrupt_vector.txpon_interrupt = INTERRUPT_ACTIVE_LEVEL ;
		interrupt_vector.rxpon_interrupt = INTERRUPT_INACTIVE_LEVEL ;
		break ;
	case 2 : /* both */
		interrupt_vector.rxpon_interrupt = INTERRUPT_ACTIVE_LEVEL ;
		interrupt_vector.txpon_interrupt = INTERRUPT_ACTIVE_LEVEL ;
		break ;
	}

	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_isr_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_isr ( & ds_ploam , interrupt_vector );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}
static char gs_onu_information_buffer[2048];

static void p_format_serial_number(uint8_t *xi_serial_number, uint8_t *xo_buffer)
{
    sprintf(xo_buffer, "%02X%02X%02X%02X-%02X%02X%02X%02X",
        xi_serial_number[0], xi_serial_number[1],
        xi_serial_number[2], xi_serial_number[3],
        xi_serial_number[4], xi_serial_number[5],
        xi_serial_number[6], xi_serial_number[7]);
}

PON_ERROR_DTE fi_format_onu_information(char **xo_onu_information)
{
    char buffer [ 32 ];
    char buffer1 [ 32 ];
    char buffer2 [ 32 ];

    p_format_serial_number(( char * ) & gs_gpon_database.physical_parameters.serial_number, buffer);

    if (gs_gpon_database.onu_parameters.onu_id == GPON_ID_DONT_CARE )
    {
        sprintf(buffer2, "DON'T CARE");
    }
    else if (gs_gpon_database.onu_parameters.onu_id == PON_ID_BROADCAST )
    {
        sprintf(buffer2, "BROADCAST");
    }
    else
    {
        sprintf(buffer2, "%d",(int16_t ) gs_gpon_database.onu_parameters.onu_id);
    }

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

    *xo_onu_information = gs_onu_information_buffer;

    return PON_NO_ERROR;
}

static int p_gpon_get_info_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    char *onu_information;
    PON_ERROR_DTE error;
    
    /* Call the under test routine */
    bdmf_session_print(session, "UT: Invoking function 'p_gpon_get_info_command'\n\r" );
    /* -------------------------------------------------------------------- */
    error = fi_format_onu_information(&onu_information);
    /* -------------------------------------------------------------------- */
    bdmf_session_print(session, "\n%s\n", onu_information);
    
    /* Translate and print the error code */
    bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );
    
    return error ? BDMF_ERR_PARM : 0;
}

static int p_gpon_set_min_response_time_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	PON_ERROR_DTE error = PON_NO_ERROR;
	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_set_min_response_time_command'\n\r" );
	/* Write to the DB */
	gs_gpon_database.physical_parameters.min_response_time = parm[1].value.unumber;
	return error ? BDMF_ERR_PARM : 0;
}

static int p_gpon_set_to1_timer_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	PON_ERROR_DTE error = PON_NO_ERROR;
	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_set_to1_timer_command'\n\r" );
	/* Write to the DB */
	gs_gpon_database.onu_parameters.to1_timer_timeout = parm[1].value.unumber;
	return error ? BDMF_ERR_PARM : 0;
}

static int p_gpon_set_to2_timer_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
	PON_ERROR_DTE error = PON_NO_ERROR;
	/* Call the under test routine */
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_set_to2_timer_command'\n\r" );
	/* Write to the DB */
	gs_gpon_database.onu_parameters.to2_timer_timeout = parm[1].value.unumber;
	return error ? BDMF_ERR_PARM : 0;
}

static int p_gpon_configure_link_params_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
	bdmf_session_print(session, "UT: Invoking function 'p_gpon_configure_link_params_command'\n\r" );
	/* -------------------------------------------------------------------- */
	error = gpon_configure_link_params ( & pon_parameters );
	/* -------------------------------------------------------------------- */

	/* Translate and print the error code */
	bdmf_session_print(session, "UT: Function returns '%s'\n\r", gpon_stack_gpon_error_code_to_string ( error ) );

	return error ? BDMF_ERR_PARM : 0;

}

static int p_gpon_dba_debug_statistics_allocate_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
#ifdef DBA_DEBUG_STATISTICS

    uint32_t memory_size;
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

    bdmf_session_print(session, "UT: Invoking function 'fi_gpon_dba_allocate_debug_statistics_db'\n\r" );
    /* --------------------------------------------------------------------------------- */
    memory_size = gpon_dba_allocate_debug_statistics_db(tconts_number, cycles_number);
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

    gpon_dba_debug_reset_statistics();

#else
    bdmf_session_print(session, "This operation is not supported without DBA_DEBUG_STATISTICS compilation flag\n\r");
#endif 

    return 0;
}

static int p_gpon_dba_debug_statistics_free_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
#ifdef DBA_DEBUG_STATISTICS

    if(ge_dba_debug_statistics == NULL)
    {
        bdmf_session_print(session, "Statistics database was not allocated\n\r");
        return BDMF_ERR_INTERNAL;
    }

    /* --------------------------------------------------------------------------------- */
    gpon_dba_free_debug_statistics_db(ge_dba_debug_statistics_max_number_of_cycles);
    /* --------------------------------------------------------------------------------- */

    bdmf_session_print(session, "Debug statistics database was freed\n\r");

#else
    bdmf_session_print(session, "This operation is not supported without DBA_DEBUG_STATISTICS compilation flag\n\r");
#endif 

    return 0;
}

static int p_gpon_dba_debug_statistics_collect_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
        ge_dba_debug_statistics_number_of_cycles = parm[0].value.unumber;
        ge_dba_debug_statistics_collect = 1;

        bdmf_session_print(session, "Starting to collect DBA debug statistics \n\r");
    }

#else
    bdmf_session_print(session, "This operation is not supported without DBA_DEBUG_STATISTICS compilation flag\n\r");
#endif 

    return 0;
}

static int p_gpon_dba_debug_statistics_reset_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
#ifdef DBA_DEBUG_STATISTICS

    if(ge_dba_debug_statistics == NULL)
    {
        bdmf_session_print(session, "statistics database must be allocated\n\r");
        return BDMF_ERR_INTERNAL;
    }

    /* -------------------------------- */
    pi_gpon_dba_debug_reset_statistics();
    /* -------------------------------- */

    bdmf_session_print(session, "DBA debug statistics were reset\n\r");

#else 
    bdmf_session_print(session, "This operation is not supported without DBA_DEBUG_STATISTICS compilation flag\n\r");
#endif 

    return 0;
}

static int p_gpon_dba_debug_dump_statistics_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
#ifdef DBA_DEBUG_STATISTICS
#define DBA_ALL_TCONTS 40

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
 
    return 0; 
}

static     char object_list[5000];
static int p_gpon_get_log_object_list_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    PON_ERROR_DTE error = PON_NO_ERROR ;

    char *parent_name = parm[0].value.string;

    strcpy(object_list, "\n");

    error = gpon_get_log_object_list(parent_name, object_list);

    if ( error != PON_NO_ERROR )
    {
        bdmf_session_print(session, "Gpon Logger Object %s not found\n", parent_name);
    }

    /* Translate and print the error code */
    bdmf_session_print(session, "%s\n\r",  object_list);

    return error ? BDMF_ERR_PARM : 0;
}

static int p_gpon_set_log_object_state_command(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    /* Define I/O variables */
    PON_ERROR_DTE error = PON_NO_ERROR ;

    char *object_name = parm[0].value.string;

    error = gpon_set_log_object_state(object_name, parm[1].value.unumber);

    if ( error != PON_NO_ERROR )
    {
        bdmf_session_print(session, "Gpon Logger Object %s not found\n", object_name);
    }
    else
    {
        strcpy(object_list, "\n");
        gpon_get_log_object_list(object_name, object_list);

        bdmf_session_print(session, "\n %s \n", object_list);
    }

    return error ? BDMF_ERR_PARM : 0;
}

static int p_gpon_set_broadcast_command ( bdmf_session_handle  session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms )
{
    gs_gpon_database.onu_parameters.broadcast_default_value = parm[0].value.unumber;
    bdmf_session_print(session, "BROADCAST ID = 0x%x\n", gs_gpon_database.onu_parameters.broadcast_default_value);

    return(0);
}

static bdmfmon_handle_t pon_stack_dir;
static bdmfmon_handle_t gpon_drv_dir;

void gpon_initialize_drv_pon_stack_shell(bdmfmon_handle_t driver_dir)
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
			{"normal", NORMAL_PLOAM_CELL},
        {"urgent", URGENT_PLOAM_CELL},
        {NULL, 0},
	};

    static struct bdmfmon_enum_val gem_block_size_select_enum_table[] = 
    {
			{"32_Bytes", 32},
			{"48_Bytes", 48},
        {"64_Bytes", 64},
        {NULL, 0},
	};

    static struct bdmfmon_enum_val interrupt_vector_select_enum_table[] = 
    {
			{"rx", 0},
			{"tx", 1},
        {"both", 2},
        {NULL, 0},
	};

    static struct bdmfmon_enum_val record_type_enum_table[] = 
    {
			{"all_accesses", GPON_RX_BWMAP_RECORD_ALL_TYPE},
			{"onu_accesses", GPON_RX_BWMAP_RECORD_ALL_ONU_TYPE},
        {"specific_access", GPON_RX_BWMAP_RECORD_SPECIFIC_ALLOC_TYPE},
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

    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "i", "Software initialize", p_gpon_software_initialize_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "t", "Software terminate", p_gpon_software_terminate_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "cosn", "Configure onu serial number", p_gpon_configure_onu_serial_number_command,
        BDMFMON_MAKE_PARM_RANGE("vendor_code", "vendor_code", BDMFMON_PARM_HEX, 0, 0, 0xFFFFFFFF),
        BDMFMON_MAKE_PARM_RANGE("serial_number", "serial_number", BDMFMON_PARM_HEX, 0, 0, 0xFFFFFFFF));
    BDMFMON_MAKE_CMD(pon_stack_dir, "cop", "Configure onu password", p_gpon_configure_onu_password_command,
            BDMFMON_MAKE_PARM("onu_password", "onu_password", BDMFMON_PARM_STRING, 0));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "lr", "Link reset", p_gpon_link_reset_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "la", "Link activate", p_gpon_link_activate_command,
            BDMFMON_MAKE_PARM_ENUM("initial_state_disable", "initial_state_disable true or false", bdmfmon_enum_bool_table, 0));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "ld", "Link deactivate", p_gpon_link_deactivate_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "glnk", "Get link parameters", p_gpon_get_link_parameters_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gls", "Get link state", p_gpon_get_link_state_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gss", "Get software state", p_gpon_get_software_state_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "stxq",  "Set txfifo queue configuration", p_gpon_set_txfifo_queue_configuration_command,
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
    BDMFMON_MAKE_CMD(pon_stack_dir, "gtxq", "Get txfifo queue configuration", p_gpon_get_txfifo_queue_configuration_command,
        BDMFMON_MAKE_PARM_RANGE("fifo_id", "fifo_id - 0-31", BDMFMON_PARM_NUMBER, 0, 0, TXPON_FIFO_UNIT_HIGH));
    BDMFMON_MAKE_CMD(pon_stack_dir, "gtxg", "Get txfifo group configuration", p_gpon_get_txfifo_queue_configuration_group_0_to_3_command,
        BDMFMON_MAKE_PARM_RANGE("group_id", "fifo group ID", BDMFMON_PARM_NUMBER, 0,
            GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_RANGE_LOW, GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_RANGE_HIGH));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "grxpm", "Get rxpon pm counters", p_gpon_get_rxpon_pm_counters_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "grxplm", "Get rxpon ploam counters", p_gpon_get_rxpon_ploam_counters_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "srxbwa", "Set rxpon bw alloc association", p_gpon_set_rxpon_bw_alloc_association_command,
        BDMFMON_MAKE_PARM_RANGE("alloc_1", "alloc 1  - Decimal (0-1023)", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH),
        BDMFMON_MAKE_PARM_RANGE("alloc_2", "alloc 2  - Decimal (0-1023)", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH),
        BDMFMON_MAKE_PARM_RANGE("alloc_3", "alloc 3  - Decimal (0-1023)", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH),
        BDMFMON_MAKE_PARM_RANGE("alloc_4", "alloc 4  - Decimal (0-1023)", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "grxbwa", "Get rxpon bw alloc association", p_gpon_get_rxpon_bw_alloc_association_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "grxbwc", "Get rxpon bw alloc counter", p_gpon_get_rxpon_bw_alloc_counter_command,
        BDMFMON_MAKE_PARM_RANGE("alloc_id", "alloc id  - Decimal (0-1023)", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH));
    BDMFMON_MAKE_CMD(pon_stack_dir, "gtpdc", "Get tcont front end PD counter", p_gpon_get_tcont_fe_queue_pd_counter_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_id", "tcont_id : Decimal (0 - 39 )", BDMFMON_PARM_NUMBER, 0, 0, CS_DEFAULT_TCONT_ID));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gtxpm", "Get txpon pm counters", p_gpon_get_txpon_pm_counters_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "gtxfpm","Get txfifo pm counters", p_gpon_get_tcont_pm_counters_command,
        BDMFMON_MAKE_PARM_RANGE("fifo_id", "fifo_id : [0 - 3]", BDMFMON_PARM_NUMBER, 0, 0, 3));
    BDMFMON_MAKE_CMD(pon_stack_dir, "ctpif", "Control port id filter", p_gpon_control_port_id_filter_command,
        BDMFMON_MAKE_PARM_RANGE("flow_id", "flow id: [0 - 255]", BDMFMON_PARM_NUMBER, 0, 0, 255),
        BDMFMON_MAKE_PARM_ENUM("filter_status", "filter status: [true | false]", bdmfmon_enum_bool_table, 0));
    BDMFMON_MAKE_CMD(pon_stack_dir, "cpif", "Configure port id filter", p_gpon_configure_port_id_filter_command,
        BDMFMON_MAKE_PARM_RANGE("flow_id", "flow id: [0 - 255]", BDMFMON_PARM_NUMBER, 0, 0, 255),
        BDMFMON_MAKE_PARM_RANGE("port_id_filter", "port id: [0 - 4096]", BDMFMON_PARM_NUMBER, 0, 0, 4095),
        BDMFMON_MAKE_PARM_RANGE("port_id_mask", "Port id mask: [0 - 4096]", BDMFMON_PARM_NUMBER, 0, 0, 4095),
        BDMFMON_MAKE_PARM_ENUM("encryption_mode", "encryption mode: [on | off]", bdmfmon_enum_bool_table, 0),
        BDMFMON_MAKE_PARM_ENUM("flow_type", "flow_type mode: [eth | tdm]", flow_type_enum_table, 0),
        BDMFMON_MAKE_PARM_ENUM("flow_priority", "flow priority mode: [low | high | omci | unassigned]", flow_priority_enum_table, 0));
    BDMFMON_MAKE_CMD(pon_stack_dir, "mepif", "Modify encryption port id filter", p_gpon_modify_encryption_port_id_filter_command,
        BDMFMON_MAKE_PARM_RANGE("flow_id", "flow id: [0 - 255]", BDMFMON_PARM_NUMBER, 0, 0, 255),
        BDMFMON_MAKE_PARM_ENUM("encryption_mode", "encryption mode: [on | off]", bdmfmon_enum_bool_table, 0));
    BDMFMON_MAKE_CMD(pon_stack_dir, "gpif",  "Get port id filter configuration", p_gpon_get_port_id_filter_command,
        BDMFMON_MAKE_PARM_RANGE("flow_id", "flow id: [0 - 255]", BDMFMON_PARM_NUMBER, 0, 0, 255));
    BDMFMON_MAKE_CMD(pon_stack_dir, "ctcnt", "Configure tcont", p_gpon_configure_tcont_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_queue_id", "tcont_queue_id - [0..39]", BDMFMON_PARM_NUMBER, 0, 0, 39),
        BDMFMON_MAKE_PARM_RANGE("alloc_id", "alloc_id - [0..4095]", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH));
    BDMFMON_MAKE_CMD(pon_stack_dir, "gtcnt", "Get tcont params", p_gpon_get_tcont_params_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_queue_id", "tcont_queue_id - [0..39]", BDMFMON_PARM_NUMBER, 0, 0, 39));
    BDMFMON_MAKE_CMD(pon_stack_dir, "mtcnt", "Modify tcont", p_gpon_modify_tcont_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_queue_id", "tcont_queue_id - [1..39]", BDMFMON_PARM_NUMBER, 0, 1, 39),
        BDMFMON_MAKE_PARM_RANGE("new_alloc_id", "new alloc_id - [0..4095]", BDMFMON_PARM_NUMBER, 0,
            PON_ALLOC_ID_DTE_LOW, PON_ALLOC_ID_DTE_HIGH));
    BDMFMON_MAKE_CMD(pon_stack_dir, "rtcnt", "Remove tcont", p_gpon_remove_tcont_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_queue_id", "tcont_queue_id - [1..39]", BDMFMON_PARM_NUMBER, 0, 1, 39));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "ptcnt", "Print tcont table", p_gpon_print_tcont_table_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "idbr", "Init dba sr process ", p_gpon_init_dba_sr_process_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "tdbr", "Terminate dba sr process ", p_gpon_terminate_dba_sr_process_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gbip8c","Get bip8 counters ", p_gpon_get_bip8_counters_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "sbwrcd","start bw recording ", p_gpon_start_bw_recording_command,
        BDMFMON_MAKE_PARM_ENUM("rcd_stop", "recocd_stop - { false = Stop record on map end } | { true = Stop record on memory end }", bdmfmon_enum_bool_table, 0),
        BDMFMON_MAKE_PARM_ENUM("recocd_type", "recocd_stop - all_accesses | onu_accesses | specific_access ", record_type_enum_table, 0),
        BDMFMON_MAKE_PARM_RANGE("alloc_id", "Alloc id to be record (0-1023) in case record_type=specific_access", BDMFMON_PARM_NUMBER, 0, 0, 1023));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gbwrcd","get BW recording data ", p_gpon_get_bw_recording_data_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "sbwr","Stop BW recording", p_gpon_stop_bw_recording_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "sdbrp" ,"Set rogue onu detection parameters ", p_gpon_set_rogue_onu_detection_params_command,
        BDMFMON_MAKE_PARM_ENUM("rogue_onu_level_clear", "rogue_onu_level_clear", bdmfmon_enum_bool_table, 0),
        BDMFMON_MAKE_PARM_ENUM("rogue_onu_diff_clear", "rogue_onu_diff_clear", bdmfmon_enum_bool_table, 0),
        BDMFMON_MAKE_PARM_ENUM("source_select", "external (from IO) | internal (serDes)", rogue_source_select_enum_table, 0),
        BDMFMON_MAKE_PARM_RANGE("rogue_onu_level_threshold", "rogue_onu_level_threshold", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFF),
        BDMFMON_MAKE_PARM_RANGE("rogue_onu_diff_threshold", "rogue_onu_diff_threshold", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFF));
    BDMFMON_MAKE_CMD(pon_stack_dir, "srop"  ,"Set dbru report parameters ", p_gpon_set_dbru_report_parameters_command,
        BDMFMON_MAKE_PARM_ENUM("update_mode", "mode - mips | runner", update_mode_select_enum_table, 0),
        BDMFMON_MAKE_PARM_ENUM("gem_block_size", "block size  - Decimal 32 | 48 | 64 Bytes", gem_block_size_select_enum_table, 0));
    BDMFMON_MAKE_CMD(pon_stack_dir, "slb", "Set loopback", p_gpon_set_loopback_command,
        BDMFMON_MAKE_PARM_ENUM("loopback_status", "loopback_status", bdmfmon_enum_bool_table, 0),
        BDMFMON_MAKE_PARM_RANGE("loopback_queue", "loopback_queue - 0...6", BDMFMON_PARM_NUMBER, 0, 0, 6));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "fqueue", "flush queue", p_gpon_flush_queue_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "spm", "Send ploam message", p_gpon_send_ploam_message_command,
        BDMFMON_MAKE_PARM_RANGE("repetition", "repetition", BDMFMON_PARM_NUMBER, 0, 1, 3),
        BDMFMON_MAKE_PARM_RANGE("message_id", "ploam message id in HEX format", BDMFMON_PARM_HEX, 0, 0, 0xFF),
        BDMFMON_MAKE_PARM("ploam_buffer", "string of 10 bytes in HEX format", BDMFMON_PARM_STRING, 0),
        BDMFMON_MAKE_PARM_ENUM("priority", "priority - normal | urgent ( only 1 time to send )", ploam_priority_select_enum_table, 0));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "goi", "Get onu information", p_gpon_get_onu_information_command);
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "ged", "Get equalization delay", p_gpon_get_equalization_delay_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "isr", "isr", p_gpon_isr_command,
        BDMFMON_MAKE_PARM_ENUM("interrupt_vector", "interrupt vector - rx/tx/both", interrupt_vector_select_enum_table, 0));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "gi", "Get info", p_gpon_get_info_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "smrt", "set min response time", p_gpon_set_min_response_time_command,
        BDMFMON_MAKE_PARM_RANGE("min_response_time", "min_response_time uint 32", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFFF));
    BDMFMON_MAKE_CMD(pon_stack_dir, "sto1", "Set to1 timer", p_gpon_set_to1_timer_command,
       BDMFMON_MAKE_PARM_RANGE("to1_timer", "to1_timer uint 32", BDMFMON_PARM_NUMBER, 0, 0, 2147483647));
    BDMFMON_MAKE_CMD(pon_stack_dir, "sto2", "Set to2 timer", p_gpon_set_to2_timer_command,
        BDMFMON_MAKE_PARM_RANGE("to2_timer", "to2_timer uint 32", BDMFMON_PARM_NUMBER, 0, 0, 2147483647));
    BDMFMON_MAKE_CMD(pon_stack_dir, "cflnk", "Configure link parameters", p_gpon_configure_link_params_command,
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
    BDMFMON_MAKE_CMD(pon_stack_dir, "broadcast", "Set broadcst Id", p_gpon_set_broadcast_command, BDMFMON_MAKE_PARM_RANGE("set_broadcast_id", "set_broadcast_id", BDMFMON_PARM_NUMBER, 0, 0, 0xFFFFFFF));
   
    BDMFMON_MAKE_CMD(pon_stack_dir, "dsa", "Debug Statistics Allocate", p_gpon_dba_debug_statistics_allocate_command,
        BDMFMON_MAKE_PARM_RANGE("tconts_number", "Maximal number of TCONT IDs", BDMFMON_PARM_NUMBER, 0, 0, 32),
        BDMFMON_MAKE_PARM_RANGE("cycles_number", "Maximal number of cycles", BDMFMON_PARM_NUMBER, 0, 0, 100));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "dsf", "Debug Statistics Free", p_gpon_dba_debug_statistics_free_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "dsc", "Debug Statistics Collect", p_gpon_dba_debug_statistics_collect_command,
        BDMFMON_MAKE_PARM_RANGE("number_of_cycles", "Maximal number of cycles", BDMFMON_PARM_NUMBER, 0, 0, 100));
    BDMFMON_MAKE_CMD_NOPARM(pon_stack_dir, "dsr", "Debug Statistics Reset", p_gpon_dba_debug_statistics_reset_command);
    BDMFMON_MAKE_CMD(pon_stack_dir, "dds", "Debug Statistics Dump", p_gpon_dba_debug_dump_statistics_command,
        BDMFMON_MAKE_PARM_RANGE("tcont_id", "TCONT ID or '40' for all tconts", BDMFMON_PARM_NUMBER, 0, 0, 40));
    BDMFMON_MAKE_CMD(pon_stack_dir, "glol", "Get logger object list by parent", p_gpon_get_log_object_list_command,
        BDMFMON_MAKE_PARM("parent_object", "parent_object", BDMFMON_PARM_STRING, 0));
    BDMFMON_MAKE_CMD(pon_stack_dir, "slos", "Logger object set state", p_gpon_set_log_object_state_command,
        BDMFMON_MAKE_PARM("parent_object", "parent_object", BDMFMON_PARM_STRING, 0),
        BDMFMON_MAKE_PARM_RANGE("state", "0 or 1", BDMFMON_PARM_UNUMBER, 0, 0, 1));


    /* Initialize driver shell */
    gpon_drv_dir = bdmfmon_dir_add(driver_dir, "gpon_drv", "GPON drivers", BDMF_ACCESS_GUEST, NULL);
    if (!gpon_drv_dir)
    {
        bdmf_session_print(NULL, "Can't create gpon_drv_dir directory\n");
        return;
    }

}

void gpon_exit_drv_pon_stack_shell(void)
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
        return "GPON_NO_ERROR";
        break;
    case PON_GENERAL_ERROR:
        return "GPON_GENERAL_ERROR";
        break;
    case PON_ERROR_INVALID_PARAMETER:
        return "GPON_ERROR_INVALID_PARAMETER";
        break;
    case PON_ERROR_SW_NOT_INITIALIZED:
        return "GPON_ERROR_SW_NOT_INITIALIZED";
        break;
    case PON_ERROR_SW_ALREADY_INITIALIZED:
        return "GPON_ERROR_SW_ALREADY_INITIALIZED";
        break;
    case PON_ERROR_SW_ALREADY_TERMINATED:
        return "GPON_ERROR_SW_ALREADY_TERMINATED";
        break;
    case PON_ERROR_GENERAL_ERROR:
        return "GPON_ERROR_GENERAL_ERROR";
        break;
    case PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR:
        return "GPON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR";
        break;
    case PON_ERROR_INTERNAL_DB_ERROR:
        return "GPON_ERROR_INTERNAL_DB_ERROR";
        break;
    case PON_ERROR_TIME_OUT_FLUSH_IS_NOT_DONE:
        return "GPON_ERROR_TIME_OUT_FLUSH_IS_NOT_DONE";
        break;
    case PON_HW_TIMER_IS_NOT_INITIALIZED:
        return "GPON_HW_TIMER_IS_NOT_INITIALIZED";
        break;
    case PON_DBR_ALREADY_ENABLED:
        return "GPON_DBR_ALREADY_ENABLED";
        break;
    case PON_DBR_NOT_ENABLED:
        return "GPON_DBR_NOT_ENABLED";
        break;
    case PON_ERROR_NOT_ENOUGH_MEMORY:
        return "GPON_ERROR_NOT_ENOUGH_MEMORY";
        break;
    case PON_ERROR_INVALID_OBJECT:
        return "GPON_ERROR_INVALID_OBJECT";
        break;
    case PON_ERROR_DRIVER_ERROR:
        return "GPON_ERROR_DRIVER_ERROR";
        break;
    case PON_ERROR_OS_ERROR:
        return "GPON_ERROR_OS_ERROR";
        break;
    case PON_ERROR_INVALID_STATE:
        return "GPON_ERROR_INVALID_STATE";
        break;
    case PON_ERROR_LINK_NOT_CREATED:
        return "GPON_ERROR_LINK_NOT_CREATED";
        break;
    case PON_ERROR_INVALID_INFO:
        return "GPON_ERROR_INVALID_INFO";
        break;
    case PON_ERROR_INVALID_SYSCLK_TIMER_ID:
        return "GPON_ERROR_INVALID_SYSCLK_TIMER_ID";
        break;
    case PON_ERROR_LINK_ALREADY_ACTIVE:
        return "GPON_ERROR_LINK_ALREADY_ACTIVE";
        break;
    case PON_ERROR_LINK_ALREADY_INACTIVE:
    	return "GPON_ERROR_LINK_ALREADY_INACTIVE";
    	break;
    case PON_ERROR_LINK_NOT_ACTIVE:
    	return "GPON_ERROR_LINK_NOT_ACTIVE";
    	break;
    case PON_ERROR_LINK_NOT_INACTIVE:
    	return "GPON_ERROR_LINK_NOT_INACTIVE";
    	break;
    case PON_ERROR_OUT_OF_RESOURCES:
    	return "GPON_ERROR_OUT_OF_RESOURCES";
    	break;
    case PON_ERROR_QUEUE_ILLEGAL_ID:
    	return "GPON_ERROR_QUEUE_ILLEGAL_ID";
    	break;
    case PON_ERROR_QUEUE_DISABLED:
    	return "GPON_ERROR_QUEUE_DISABLED";
    	break;
    case PON_ERROR_QUEUE_FULL:
    	return "GPON_ERROR_QUEUE_FULL";
    	break;
    case PON_ERROR_QUEUE_EMPTY:
    	return "GPON_ERROR_QUEUE_EMPTY";
    	break;
    case PON_ERROR_TCONT_NOT_ASSIGNED:
    	return "GPON_ERROR_TCONT_NOT_ASSIGNED";
    	break;
    case PON_ERROR_ALLOC_ALREADY_ASSIGN_TO_ANOTHER_TCONT:
    	return "GPON_ERROR_ALLOC_ALREADY_ASSIGN_TO_ANOTHER_TCONT";
    	break;
    case PON_ERROR_TCONT_TABLE_IS_FULL:
    	return "GPON_ERROR_TCONT_TABLE_IS_FULL";
    	break;
    case PON_ERROR_ALLOC_ALREADY_ASSIGN:
    	return "GPON_ERROR_ALLOC_ALREADY_ASSIGN";
    	break;
    case PON_ERROR_ALLOC_NOT_ASSIGNED:
    	return "GPON_ERROR_ALLOC_NOT_ASSIGNED";
    	break;
    case PON_ERROR_ALLOC_NOT_IN_RANGE:
    	return "GPON_ERROR_ALLOC_NOT_IN_RANGE";
    	break;
    case PON_ERROR_GRANT_ILLEGAL_ID:
    	return "GPON_ERROR_GRANT_ILLEGAL_ID";
    	break;
    case PON_ERROR_GRANT_INVALID_TYPE:
    	return "GPON_ERROR_GRANT_INVALID_TYPE";
    	break;
    case PON_ERROR_PORT_ID_ALREADY_CFG:
    	return "GPON_ERROR_PORT_ID_ALREADY_CFG";
    	break;
    case PON_ERROR_TCONT_IS_ALREADY_CFG:
    	return "GPON_ERROR_TCONT_IS_ALREADY_CFG";
    	break;
    case PON_ERROR_OPERATION_TIMEOUT:
    	return "GPON_ERROR_OPERATION_TIMEOUT";
    	break;
    case PON_ERROR_QUEUE_CREATE:
    	return "GPON_ERROR_QUEUE_CREATE";
    	break;
    case PON_ERROR_QUEUE_SEND:
    	return "GPON_ERROR_QUEUE_SEND";
    	break;
    case PON_ERROR_WATCHDOG_START:
    	return "GPON_ERROR_WATCHDOG_START";
    	break;
    case PON_IC_ERROR:
    	return "GPON_IC_ERROR";
    	break;
    case PON_FE_ERROR:
    	return "GPON_FE_ERROR";
    	break;
    case PON_RT_HW_TIMER_ERROR:
    	return "RT_HW_TIMER_ERROR";
    	break;
    default:
        return "unknown";
        break;
    }
}

int32_t f_translate_string_to_binary(char *xi_string, uint8_t *xo_buffer, uint32_t xi_max_length )
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



