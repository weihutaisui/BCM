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


#ifndef GPON_DB_H_INCLUDED
#define GPON_DB_H_INCLUDED

/*#include <linux/types.h>*/
#include "gpon_all.h"
#include "gpon_ind_task.h"
#include "gpon_sm.h"
#include "gpon_cfg.h"
#include "gpon_isr.h"

#define GPON_STACK_SIZE                         0x2000 
#define GPON_US_PLOAM_RATE                      2000 
#define GPON_DELIMITER_SIZE                     3 
#define GPON_DEFAULT_REI_SEQUENCE_NUMBER        0 
#define GPON_DEFAULT_BER_INTERVAL               1000 
#define GPON_DEFAULT_GUARD_BITS_NUMBER          0 
#define GPON_DEFAULT_DELIMITER_BYTE0            0xAA 
#define GPON_DEFAULT_DELIMITER_BYTE1            0x85 
#define GPON_DEFAULT_DELIMITER_BYTE2            0xB3 
#define GPON_DEFAULT_TYPE1_PREAMBLE_BITS        0 
#define GPON_DEFAULT_TYPE2_PREAMBLE_BITS        0 
#define GPON_DEFAULT_TYPE3_PREAMBLE_BITS        0xFFFFFFFF 
#define GPON_DEFAULT_PREASSIGNED_EQ_DELAY       0 
#define GPON_DEFAULT_POWER_LEVEL_MODE           0 
#define GPON_DEFAULT_EXTENDED_BURST_FLAG        BDMF_FALSE 
#define GPON_DEFAULT_PRE_RANGED_TYPE3_NUMBER    0 
#define GPON_DEFAULT_RANGED_TYPE3_NUMBER        0 
#define GPON_DEFAULT_EXTRA_SN_TRANSMISSIONS     0 
#define GPON_DEFAULT_PREEQUALIZATION_STATUS     BDMF_FALSE 
#define GPON_DEFAULT_AES_SWITCHING_TIME         0x3FFFFFFF 
#define GPON_DEFAULT_TX_THRESHOLD               0 
#define GPON_DEFAULT_TO1_TIMEOUT                20000 
#define GPON_DEFAULT_TO2_TIMEOUT                100 
#define GPON_DEFAULT_BER_INTERVAL               1000 
#define GPON_DEFAULT_SD_THRESHOLD               4 
#define GPON_DEFAULT_SF_THRESHOLD               3 
#define GPON_DEFAULT_LOF_ASSERTION              4 
#define GPON_DEFAULT_LOF_CLEAR                  1 
#define GPON_DEFAULT_MIN_RESPONSE_TIME          35 
#define GPON_DEFAULT_DV_HOLD_PATTERN            0x00FFF0 
#define GPON_DEFAULT_DV_POLARITY                1 
#define GPON_DEFAULT_DV_SETUP_PATTERN           0x000FFF 
#define GPON_DEFAULT_POWER_MODE                 BDMF_FALSE 
#define GPON_DEFAULT_POWER_CALIBRATION_PATTERN  0xBB44AA33 
#define GPON_DEFAULT_POWER_CALIBRATION_SIZE     120 
#define GPON_DEFAULT_DBR_VALID_PART1            0xFFFFFFFF 
#define GPON_DEFAULT_DBR_VALID_PART2            0xFF 
#define GPON_DEFAULT_DBR_STOP_INDEX             0 
#define GPON_DEFAULT_GEM_BLOCK_SIZE             GEM_BLOCK_SIZE_48_BYTES 
#define GPON_DEFAULT_DATA_PATTERN_ENABLE        BDMF_TRUE 
#define GPON_DEFAULT_PRBS_ENABLE                BDMF_TRUE 
#define GPON_DEFAULT_DATA_SETUP_LENGTH          3 
#define GPON_DEFAULT_DATA_HOLD_LENGTH           3 
#define GPON_DEFAULT_DATA_SETUP_PATTERN         0 
#define GPON_DEFAULT_DATA_HOLD_PATTERN          0 
#define GPON_DEFAULT_PON_TASK_PRIORITY          150 
#define GPON_SR_DBR_INVALID                     0xFF 
#define GPON_DBR_MAX_REPORT                     0xFE 
#define GPON_MIN_DBR_SR_DATA_BYTES_NUM          16 
#define GPON_MAX_DBR_SR_DATA_BLOCKS_NUM         8191 
#define GPON_INVALID_TIMER_HANDLE               NULL 
#define GPON_SR_DBA_POLLING_INTERVAL            2 
#define GPON_LCDG_TIMEOUT                       20
#define GPON_PON_PEE_TIMEOUT                    100 
#define ALLOC_ID_INDEX_MAX                      40
#define MAX_NUMBER_OF_TCONTS                    40
#define MAX_NUMBER_OF_FLOWS                     256 
#define CS_DEFAULT_TCONT_ID 39 
#define CS_TXPON_FIFO_UNIT_HIGH_FOR_GROUP_0 7 
#define GPON_TX_ONU_ID_DEF 0xff
#define GPON_RX_ONU_BROADCAST_ID_DEF 255

/******************************************************************************/
/* This type defines the states of the GPON software                          */
/******************************************************************************/
typedef uint32_t GPON_SW_STATUS_DTE; 

/* The GPON stack is being initialized */
#define GPON_STATE_INITIALIZED ((GPON_SW_STATUS_DTE)0)
/* The GPON stack is terminated */
#define GPON_STATE_TERMINATED  ((GPON_SW_STATUS_DTE)1)

/******************************************************************************/
/* This type specifies the ONU ID                                             */
/******************************************************************************/
typedef uint16_t ONU_ID_DTE;

/* Broadcast */
#define GPON_ID_BROADCAST ((ONU_ID_DTE)0xFF)
/* Dont care */
#define GPON_ID_DONT_CARE ((ONU_ID_DTE)0xFFFF)

/******************************************************************************/
/* Callbacks                                                                  */
/******************************************************************************/
typedef struct
{
    /* SN mask status */
    bool serial_number_mask_status;

    /* match status for SN mask */
    bool serial_number_mask_matched;
}
SERIAL_NUMBER_MASK_PARAMETERS;

/******************************************************************************/
/* Data Base                                                                  */
/******************************************************************************/

/* OS resources */
typedef struct
{
    /* Stack size */
    uint32_t stack_size;
    /* OS frequency */
    uint32_t os_frequency;
    /* Pon task ID*/
    bdmf_task pon_task_id;
    /* Pon task priority */
    uint32_t pon_task_priority;
    /* PON messages queue */
    bdmf_queue_t pon_messages_queue_id;
    /* TO1 timer */
    bdmf_timer_t to1_timer_id;
    /* TO2 timer */
    bdmf_timer_t to2_timer_id;
    /* BER interval timer */
    bdmf_timer_t ber_interval_timer_id;
    /* LCDG interval timer */
    bdmf_timer_t lcdg_timer_id;
    /* Keep alive timer */
    bdmf_timer_t keep_alive_timer_id;
    /* PEE timer ID */
    bdmf_timer_t pee_timer_id;
}
GPON_RESOURCES_DTE;

/* Physical paramters */
typedef struct
{
    /* Serial number */
    PON_SERIAL_NUMBER_DTE serial_number;
    /* Password */
    PON_PASSWORD_DTE password;
    /* Upstream ploam rate */
    uint32_t upstream_ploam_rate;
    /* Min response time */
    uint32_t min_response_time;
    /* Data Pattern Enable */
    bool transceiver_data_pattern_enable;
    /* PRBS Enable */
    bool transceiver_prbs_enable;
    /* Data Setup Length */
    uint8_t transceiver_data_setup_length;
    /* Data Hold Length */
    uint8_t transceiver_data_hold_length;
    /* Data Setup Pattern */
    uint32_t transceiver_data_setup_pattern;
    /* Data Hold Pattern */
    uint32_t transceiver_data_hold_pattern;
    /* Transceiver DV setup pattern */
    uint32_t transceiver_dv_setup_pattern;
    /* Transceiver DV hold pattern */
    uint32_t transceiver_dv_hold_pattern;
    /* Transceiver DV polarity */
    uint32_t transceiver_dv_polarity;
    /* Transceiver DV extension enable */
    uint32_t transceiver_dv_extension_enable;
    /* Transceiver power calibration mode */
    bool transceiver_power_calibration_mode;
    /* Transceiver power calibration pattern */
    uint32_t transceiver_power_calibration_pattern;
    /* Transceiver power calibration size */
    uint32_t transceiver_power_calibration_size;
     /* Number of PSYNCs for LOF assertion */
    uint32_t number_of_psyncs_for_lof_assertion;
    /* Number of PSYNCs for LOF clear */
    uint32_t number_of_psyncs_for_lof_clear;
}
GPON_PHYSICAL_PARAMETERS_DTE;

typedef struct
{
    /* TX - Tx bip enable */
    bool tx_bip_enable;
    /* TX - Tx loopback enable */
    bool tx_loopback_enable;
    /* TX - Tx pls enable */
    bool tx_pls_enable;
    /* TX - Tx misc enable */
    bool tx_misc_enable;
    /* TX Scrambler mode*/
    bool tx_scrambler_enable;
    /* TX Fec mode*/
    bool tx_fec_enable;
    /* Tx data polarity*/
    bool tx_data_out_polarity;
    /* Tx Phase*/
    bool tx_phase_invert;
    /* Rx Phase*/
    bool rx_phase_invert;
}
GPON_TX_PARAMETERS;

typedef struct
{
    /* RX - Rx enable */
    bool rx_enable;
    /* Rx Paramters */
    gpon_rx_general_config_rcvr_config rcvr_config;
    /* TX CRC Override */
    bool rx_crc_override;
    /* TX Idle Filltering Enable */
    bool rx_idle_ploam_filltering_enable;
    /* TX Forward all ploam messages */
    bool rx_forward_all_ploam_messages;
    /* RX 8 KHZ clock lock */
    bool rx_8khz_clock_lock;
    /* RX 8KHZ clock phase diff */
    uint32_t rx_8khz_clock_phase_diff;
    /* Congestion threshold - into */
    uint8_t rx_congestion_into_threshold;
    /* Congestion threshold - out-of */
    uint8_t rx_congestion_out_of_threshold;
}
GPON_RX_PARAMETERS;

typedef struct
{
    /* Pre-equalization status */
    bool pre_equalization_status;
    /* Extra serial number transmissions */
    uint32_t extra_serial_number_transmissions;
    /* Number of guard bits */
    uint32_t number_of_guard_bits;
    /* Number of type 1 preamble bits */
    uint32_t number_of_type_1_preamble_bits;
    /* Number of type 2 preamble bits */
    uint32_t number_of_type_2_preamble_bits;
    /* Pattern of type 3 preamble bits */
    uint32_t pattern_of_type_3_preamble_bits;
    /* Delimiter */
    uint8_t delimiter[3];
    /* Preassigned equalization delay */
    uint32_t preassigned_equalization_delay;
    /* Serial number mask parameters */
    SERIAL_NUMBER_MASK_PARAMETERS serial_number_mask_parameters;
    /* Default power level mode */
    GPON_POWER_LEVEL_MODE_DTE default_power_level_mode;
    /* Extended burst length - valid flag - indicate if this message already arrived */
    bool extended_burst_length_valid_flag;
    /* Extended burst length - pre-range type_3_number */
    uint32_t number_of_type_3;
}
DS_PRE_RANGING_OVERHEAD_AND_LENGTH_DTE;

typedef struct
{
    /* ONU ID */
    ONU_ID_DTE onu_id;
    /* Ber Interval */
    uint32_t ber_interval;
    /* TO1 - Timer timeout */
    uint32_t to1_timer_timeout;
    /* TO2 - Timer timeout */
    uint32_t to2_timer_timeout;
    /* LCDG - Timer timeout */
    uint32_t lcdg_timer_timeout;
     /* BER threshold for SF assertion */
    uint32_t ber_threshold_for_sf_assertion;
    /* BER threshold for SD assertion */
    uint32_t ber_threshold_for_sd_assertion;
    /* Pre-equalization status */
    bool pre_equalization_status;
    /* Extra serial number transmissions */
    uint32_t extra_serial_number_transmissions;
    /* Number of guard bits */
    uint32_t number_of_guard_bits;
    /* Number of type 1 preamble bits */
    uint32_t number_of_type_1_preamble_bits;
    /* Number of type 2 preamble bits */
    uint32_t number_of_type_2_preamble_bits;
    /* Pattern of type 3 preamble bits */
    uint32_t pattern_of_type_3_preamble_bits;
    /* Delimiter */
    uint8_t delimiter [ 3 ];
    /* Preassigned equalization delay */
    uint32_t preassigned_equalization_delay;
    /* Equalization dealy as recieved from OLT */
    uint32_t delay;
    /* Serial number mask parameters */
    SERIAL_NUMBER_MASK_PARAMETERS serial_number_mask_parameters;
    /* Default power level mode */
    GPON_POWER_LEVEL_MODE_DTE default_power_level_mode;
    /* Extended burst length - valid flag - indicate if this message already arrived */
    bool extended_burst_length_valid_flag;
    /* Extended burst length - pre-range type_3_number */
    uint8_t pre_range_type_3_number;
    /* Extended burst length - range type_3_number*/
    uint8_t range_type_3_number;
    /* pre-ranging parameters */
    DS_PRE_RANGING_OVERHEAD_AND_LENGTH_DTE pre_ranging_overhead_and_length_parameters;
    /* Broadcast */
    ONU_ID_DTE broadcast_default_value;
    /* PON ID */
    PON_ID_PARAMETERS pon_id_param;
}
GPON_ONU_PARAMETERS;

/* Link */
typedef struct
{
    /* Activation state machine control */
    GPON_OPERATION_STATE_MACHINE_DTE operation_state_machine;
    /* AES state machine control */
    GPON_AES_STATE_MACHINE_DTE aes_state_machine;
    /* Serial number mask message counter */
    uint32_t serial_number_request_messages_counter;
    /* REI sequence number */
    uint8_t rei_sequence_number;
    /* AES switching key time flag */
    bool aes_key_switching_first_time;
    /*Auto restart TO1 mode*/
    GPON_TO1_AUTO_RESTART_MODE_DTE auto_restart_to1_mode;
    /*Flag for serial number request*/
    bool received_serial_number_request;
}
GPON_LINK_PARAMETERS_DTE;

typedef struct
{
    bool dba_sr_timer_initiated;
    /* Reporting block in units of bytes */
    PON_GEM_BLOCK_SIZE_DTE dba_sr_reporting_block;
    /* Reporting polling interval as configured by user */
    uint32_t sr_dba_interval;
    /* Value of polling interval in microseconds which are the units   
       resolution of the RT HW timer */
    uint32_t sr_dba_interval_microsec;
    /*DBR valid to tcont 0 -31 */
    uint32_t dbr_valid_part1;
    /* DBR valid to tcont 32-39 */
    uint8_t dbr_valid_part2;
    /* DPR stop index */
    uint8_t dbr_stop_index;
    /* DBA status report mode */
    GPON_SR_MODE_DTE dbr_sr_mode;
    /* DBR flush */
    bool dbr_flush;
    /* DBR Status - Enable / Disable */
    bool dbr_status;
    /* indicates whether DBA-SR process is allowed to run   
       it is allowed to run only if the ONU is in state O5. */
    bool is_dba_sr_allowed;
    /* indicates whether DBA-SR process should be started when the ONU  
       moves from state O4 or state O6 to state O5.  */
    bool should_start_dba_sr;
    /* Allocated rttmr timer */
    int timer_num;
}
GPON_DBA_SR_PARAMETERS_DTS;

typedef struct
{
    uint32_t non_standard_rx_msg;
}
GPON_PLOAM_UNKNOWN_COUTNERS_DTS;

typedef struct
{
    /* Physical parameters */
    GPON_PHYSICAL_PARAMETERS_DTE physical_parameters;
    /* Tx param */
    GPON_TX_PARAMETERS tx_parameters;
    /* Rx param */
    GPON_RX_PARAMETERS rx_parameters;
    /* vector of Tx Pon interrupts that should be handled by the stack, 
       if not masked */
    GPON_TX_ISR tx_ier_vector;
    /* vector of Rx Pon interrupts that should be handled by the stack,
       if not masked */
    gpon_rx_irq_grxier rx_ier_vector;
    /* ONU param */
    GPON_ONU_PARAMETERS onu_parameters;
    /* link param */
    GPON_LINK_PARAMETERS_DTE link_parameters;
    /* OS resources*/
    GPON_RESOURCES_DTE os_resources;
    /* Alarm status*/
    PON_ALARM_STATUS_DTE last_alarm_status;
    /* DBR parameters */
    GPON_DBA_SR_PARAMETERS_DTS dbr_parameters;
    /* Rogue ONU Detection parameters */
    ROGUE_ONU_DETECTION_PARAMETERS rogue_onu_detection_parameters;
    /* SW Tcont table */
    PON_TCONT_DTE tconts_table[MAX_NUMBER_OF_TCONTS];
    /* SW Port table */
    PON_CONFIGURE_FILTER_INDICATION flows_table[MAX_NUMBER_OF_FLOWS];
    /* unknown ploam counter */
    GPON_PLOAM_UNKNOWN_COUTNERS_DTS unknown_ploam;
    /* Rx counters */
    PON_COUNTERS_DTE rxpon_gpon_conters;
    /* Optics Type */
    uint16_t OpticsType;
    /* SW Status */
    GPON_SW_STATUS_DTE gs_sw_state;
}
GPON_DATABASE_DTS;


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_software_initialize                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Initialize GPON Stack Software                                     */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function activates the software and firmware                        */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_software_initialize(uint32_t xi_pon_task_priority);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_software_terminate                                                  */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Software Terminate                                                 */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function deactivates the software                                   */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_software_terminate(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_software_status                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Get software state                                                 */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the current state of the software                  */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xo_state - The output state of the software                              */
/*     GPON_STATE_TERMINATED - The GPON stack is inactive                     */
/*     GPON_STATE_INITIALIZED - The GPON stack is currently being activated   */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_software_status(GPON_SW_STATUS_DTE *const xo_state);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_pon_id_parameters                                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Get PON ID parameters                                               */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   The function returns PON ID parameters that were read from pon-Id PLOAM. */
/*   [G.984.3 specification, Annex C]                                         */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*   xo_pon_param - pon_id_type[1 Byte], pon_identifier[7 bytes],             */
/*                  tx_optical_level[2 bytes]                                 */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_pon_id_parameters(PON_ID_PARAMETERS *xo_pon_param);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_link_parameters                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Get link parameters                                                */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the current parameters                             */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xo_parameters - The current link parameters                              */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_link_parameters(rdpa_gpon_link_cfg_t *const xo_parameters);

void gpon_set_pmd_fb_done(uint8_t state);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_stack_init                                                          */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Init                                                               */
/*                                                                            */
/******************************************************************************/
int gpon_stack_init(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_stack_exit                                                          */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Terminate                                                          */
/*                                                                            */
/******************************************************************************/
void gpon_stack_exit(void);

#endif
