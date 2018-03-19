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


#if defined(CONFIG_BCM96858)
#include "opticaldet.h"
#include <rogue_drv.h>
#endif
#include <boardparms.h>
#include "gpon_sm.h"
#include "gpon_stat.h"
#include "gpon_logger.h"
#include "gpon_all.h"
#include "gpon_db.h"
#include "gpon_pwm.h"
#include "wan_drv.h"
#ifdef CONFIG_BCM_GPON_TODD
#include "gpon_tod_common.h"
#endif


#define CE_US_RATE 1244160 
#define CE_US_BITS_PER_FRAME_RATE 155520 
#define PON_BITS_PER_BYTE 8 
#define GPON_OMCI_ALLOC_ID_INDEX 0 
#define NUMBER_OF_FIRST_TCONT_INDEX_FOR_DATA 0 
#define NUMBER_OF_LAST_TCONT_INDEX_FOR_DATA 38 
#define AES_FRAGMENTS_NUMBER 2 
#define AES_MESSAGES_NUMBER 3 

extern GPON_DATABASE_DTS gs_gpon_database;
extern bdmf_fastlock gs_fastlock;    
extern bdmf_fastlock gs_fastlock_irq;
extern GPON_CALLBACKS_DTE gs_callbacks;

bool pmdFirstBurstDone = false;

/* Define AES state machine states */
typedef enum
{
    /* 0 */ CS_AES_STATE_NOT_READY,
    /* 1 */ CS_AES_STATE_STANDBY,
    /* 2 */ CS_AES_STATE_SCHEDULING,

    /* 3 */ CS_AES_STATES_NUMBER
}
AES_STATES_DTS;

/* Define AES state machine events */
typedef enum
{
    /* 00 */ CS_AES_EVENT_REQUEST_KEY_MESSAGE_RECEIVED,
    /* 01 */ CS_AES_EVENT_SWITCH_TIME_MESSAGE_RECEIVED,

    /* 02 */ CS_AES_EVENTS_NUMBER
}
AES_EVENTS_DTS;

/* Define AES state machine events */
typedef enum
{
    /* 00 */ CS_AES_ERROR_KEY_SWITCHING_TIME_WITHOUT_KEY_REQUEST,
    /* 01 */ CS_AES_ERROR_KEY_SWITCHING_TIME_BEFORE_CURRENT_SET,
    /* 02 */ CS_AES_ERROR_KEY_SWITCHING_TIME_JUST_PASSED,

    /* 03 */ CS_AES_ERRORS_NUMBER
}
AES_ERROR_EVENTS_DTS;

#ifdef INCLUDE_LOGS
static char * gs_activation_state_names[OPERATION_STATES_NUMBER] =
{
    "INIT (O1)",
    "STANDBY (O2)",
    "SERIAL_NUMBER (O3)",
    "RANGING (O4)",
    "OPERATION (O5)",
    "POPUP (O6)",
    "EMERGENCY_STOP (O7)"
};

static char * gs_activation_event_names[CS_OPERATION_EVENTS_NUMBER] =
{
    "CLEAR_DS_LOS_OR_LOF",
    "US_OVERHEAD_MESSAGE",
    "SERIAL_NUMBER_REQUEST_MESSAGE",
    "ASSIGN_ONU_ID_MESSAGE",
    "RANGING_REQUEST_MESSAGE",
    "CHANGE_POWER_LEVEL_MESSAGE",
    "RANGING_TIME_MESSAGE",
    "TIMER_TO1_EXPIRE",
    "DEACTIVATE_ONU_ID_MESSAGE",
    "DETECT_DS_LOS_OR_LOF",
    "BROADCAST_POPUP_MESSAGE",
    "DIRECTED_POPUP_MESSAGE",
    "TIMER_TO2_EXPIRE",
    "DISABLE_SERIAL_NUMBER_DISABLE_MESSAGE",
    "DISABLE_SERIAL_NUMBER_ENABLE_MESSAGE",
    "EXTENDED_BURST_LENGTH_MESSAGE",
    "ENCRYPT_PORT_ID_MESSAGE",
    "ASSIGN_ALLOC_ID_MESSAGE",
    "CONFIGURE_PORT_ID_MESSAGE",
    "KEY_SWITCHING_TIME_MESSAGE",
    "REQUEST_KEY_MESSAGE",
    "PHYSICAL_EQUIPMENT_ERROR_MESSAGE",
    "VENDOR SPECIFIC_MESSAGE",
    "BER_INTERVAL_MESSAGE",
    "UNKNOWN_MESSAGE",
    "REQUEST_PASSWORD",
    "PST_MESSAGE",
    "SERIAL_NUMBER_MASK_MESSAGE",
    "SLEEP_ALLOW_MESSAGE",
    "PON_ID_MESSAGE",                  
    "SWIFT_POPUP_MESSAGE",             
    "RANGING_ADJUSTMENT_MESSAGE",      
};

static char * gs_aes_state_names[CS_AES_STATES_NUMBER] =
{
    "NOT READY",
    "STANDBY",
    "SCHEDULING"
};

static char * gs_aes_event_names[CS_AES_EVENTS_NUMBER] =
{
    "REQUEST_KEY_MESSAGE_RECEIVED",
    "SWITCH_TIME_MESSAGE_RECEIVED"
};
#endif 

typedef  struct
{
    union
    {
        /* ONU serial number: vendor_ID - variable form */
        uint32_t variable;
        /* ONU serial number: vendor_ID - byte array form */
        uint8_t byte[VENDOR_CODE_FIELD_SIZE];
    }
    vendor_code;

    union
    {
        /* ONU serial number: vendor specific serial number - variable form*/
        uint32_t variable;
        /* ONU serial number: vendor specific serial number - byte array form */
        uint8_t byte[VENDOR_SPECIFIC_FIELD_SIZE];
    }
    serial_number;
}
ONUB_STACK_SERIAL_NUMBER_MASK_DTE;


/* TED */
typedef struct
{
    /* number of bytes which define the delay between Rx and Tx frames. */
    uint16_t byte_delay; 
    /* number of frames which define the delay between Rx and Tx frames. */
    uint8_t frame_delay;
    /* number of bits which define the delay between Rx and Tx frames. */  
    uint8_t bit_delay;  
}
GPON_TX_EQUALIZATION_DELAY;

static GPON_TX_EQUALIZATION_DELAY gs_preassigned_ted ={};
static int32_t gs_preassigned_equalization_delay;

#define USE_NORMAL_PLAOM_ONLY_FOR_DYING_GASP

#ifdef USE_NORMAL_PLAOM_ONLY_FOR_DYING_GASP
#define USE_NORMAL_PLOAM_BUFFER
#endif

#define CE_RANDOM_DELAY_MAX 48 
#define CE_SEED_THRESHOLD 3 
#define CE_SN_REQUEST_THRESHOLD 10
#define GPON_DEF_ALLOC_ID 0
#define GPON_RX_ONU_ID_DEF 0
#define GPON_RX_RANGING_ALLOC_ID_MAX 0xfff
#define GPON_RX_RANGING_ALLOC_ID_DEF 254
#define GPON_TX_PREAMBLE_TYPE1_TYPE2_MAX_LENGTH 128
#define GPON_TX_PROD_TYPE_1_PATTERN 0xff
#define GPON_TX_PROD_TYPE_2_PATTERN 0 
#define PMD_NUMBER_OF_TYPE_3_PREAMBLE 127

extern const ru_block_rec *RU_GPON_BLOCKS[];
#define GPON_VIRTUAL_BASE ((long unsigned int) RU_GPON_BLOCKS[0]->addr[0])

#define GPON_WRITE_32(offset,r)     WRITE_32(GPON_VIRTUAL_BASE + offset,r)



typedef struct
{
    uint8_t bytes[PLOAM_MESSAGE_SIZE];
}
PLOAM_BUFFER_DTS;

typedef struct
{
    uint32_t times;
    PLOAM_BUFFER_DTS ploam;
}
PLOAM_RING_ENTRY_DTS;

typedef struct
{
    uint32_t size;
    uint32_t read;
    uint32_t write;
    PLOAM_RING_ENTRY_DTS *buffer;
}
RING_BUFFER_DTS;

#define URGENT_PLOAM_RING_BUFFER_LENGTH 16
#define NORMAL_PLOAM_RING_BUFFER_LENGTH 16
#define BASE_LEVEL_PLOAM_RING_BUFFER_LENGTH 16 
static PLOAM_RING_ENTRY_DTS gs_urgent_ploam_ring_buffer[URGENT_PLOAM_RING_BUFFER_LENGTH];
static RING_BUFFER_DTS gs_urgent_ploam_ring = {URGENT_PLOAM_RING_BUFFER_LENGTH, 0, 0, gs_urgent_ploam_ring_buffer};

#ifdef USE_NORMAL_PLOAM_BUFFER
static PLOAM_RING_ENTRY_DTS gs_normal_ploam_ring_buffer[NORMAL_PLOAM_RING_BUFFER_LENGTH];
static PLOAM_RING_ENTRY_DTS gs_base_level_ploam_ring_buffer[BASE_LEVEL_PLOAM_RING_BUFFER_LENGTH];
static RING_BUFFER_DTS gs_normal_ploam_ring = {NORMAL_PLOAM_RING_BUFFER_LENGTH, 0, 0, gs_normal_ploam_ring_buffer};
static RING_BUFFER_DTS gs_base_level_ploam_ring = {BASE_LEVEL_PLOAM_RING_BUFFER_LENGTH, 0, 0, gs_base_level_ploam_ring_buffer};
#endif

#define MS_PLOAM_READY_IN_RING(ring) (ring.read != ring.write)
#define MS_RING_IS_EMPTY(ring) (ring.read == ring.write)
#define MS_RING_IS_FULL(ring) (ring.read ==(ring.write + 1) % ring.size)
#define MS_RING_ADVANCE_READ(ring) { if (-- ring.buffer[ring.read].times == 0) ring.read = (ring.read + 1) % ring.size;}
#define MS_RING_ADVANCE_WRITE(ring) { ring.write =(ring.write + 1) % ring.size;}
#define MS_RING_READ_BUFFER(ring) (ring.buffer[ring.read])
#define MS_RING_WRITE_BUFFER(ring) (ring.buffer[ring.write])
#define MS_RING_FLUSH_BUFFER(ring) (ring.read = ring.write)

/* Define AES_key_switching_time_threshold */
#define AES_KEY_SWITCHING_TIME_MIN 3 /* 3 frames min before key-switch */
#define AES_KEY_SWITCHING_TIME_MAX (8000*60) /* 1 minute max for key-switch */
#define AES_MAX_KEY_SWITCHING_TIME 0x3fffffff 

/* Front End cfg */
#define TCONT_PKT_DESCRIPTORS_NUMBER(tcont_front_end_size) (tcont_front_end_size / 60) /* Worst case size w/min packet */
#define CS_TXPON_TCONT_GROUPS 5 
#define CS_NUMBER_OF_TCONT_IN_GROUP 8

/* TCONT front end buffer configuration(Bytes ) */
#define CS_TCONT_FRONT_END_SIZE_TCONT_0_7   960
#define CS_TCONT_FRONT_END_SIZE_TCONT_8_15  480
#define CS_TCONT_FRONT_END_SIZE_TCONT_16_23 480 
#define CS_TCONT_FRONT_END_SIZE_TCONT_24_31 480 
#define CS_TCONT_FRONT_END_SIZE_TCONT_32_39 0

static bool ls_state_was_o2_before = BDMF_FALSE;
static uint8_t gs_crc_table[256];

/* Internal function definitions */
static PON_ERROR_DTE gpon_tx_set_pon_overhead_and_length(GPON_DS_US_OVERHEAD_PLOAM
    pon_overhead_ploam, bdmf_boolean extended_flag, int number_of_type_3);
static bool f_serial_number_mask_match(uint8_t *xi_serial_number_mask_ptr, 
    uint8_t xi_number_of_valid_bits);
static void p_extended_length(uint32_t xi_number_of_type_3);
static PON_ERROR_DTE _f_gpon_txpon_send_ploam_message(GPON_US_PLOAM *const xi_message_ptr,
    PLOAM_PRIORITY_DTE xi_ploam_priority, GPON_PLOAM_TRANSMISSION_NUMBER xi_ploam_buffer_repetition);
static bool f_serial_number_match(PON_SERIAL_NUMBER_DTE xi_serial_number_1,
    PON_SERIAL_NUMBER_DTE xi_serial_number_2);
static void set_alloc_id_configuration(PON_ASSIGN_ALLOC_ID_INDICATION assign_alloc_id_struct);
#ifdef USE_NORMAL_PLOAM_BUFFER
static PON_ERROR_DTE f_send_normal_ploam(GPON_US_PLOAM *xi_ploam_message_ptr, 
    GPON_PLOAM_TRANSMISSION_NUMBER xi_normal_ploam_buffer_repetition);
static uint8_t gs_normal_ploam_valid_number = 0;
#define NORMAL_PLOAM_BUFFER_NUM 4
#endif
static PON_ERROR_DTE gpon_tx_set_sn_ranging_ploam_content(GPON_TX_PLOAM_MESSAGE *ranging_ploam_cont);
static PON_ERROR_DTE gpon_tx_get_sn_ranging_ploam_content(GPON_TX_PLOAM_MESSAGE *ranging_ploam_cont);
static PON_ERROR_DTE gpon_set_ranging_valid_bit(bdmf_boolean ranging_valid_bit);
static PON_ERROR_DTE gpon_tx_get_idle_ploam_content(GPON_TX_PLOAM_MESSAGE *idle_ploam_cont);
static PON_ERROR_DTE gpon_tx_set_idle_ploam_content(GPON_TX_PLOAM_MESSAGE *idle_ploam_cont);
static PON_ERROR_DTE gpon_tx_set_tx_ploam_bit(bdmf_boolean txploam_clear_bit);
static uint8_t gpon_tx_utils_calculate_crc(uint8_t *buff_ptr, uint8_t buff_length);
static PON_ERROR_DTE gpon_rx_set_ranging_alloc_id_configuration(uint16_t ranging_alloc_id,
    bdmf_boolean enable_ranging_alloc_id);
static PON_ERROR_DTE gpon_tx_set_group_txfifo_configuration(uint32_t xi_group_id,
    uint16_t xi_packet_descriptor_group_base_address, uint16_t xi_group_offset, 
    uint16_t xi_group_size);
static PON_ERROR_DTE gpon_rx_set_rx_status(bdmf_boolean rx_enable);
static PON_ERROR_DTE gpon_tx_set_tx_status(bdmf_boolean tx_status);
static bool f_serial_number_match(PON_SERIAL_NUMBER_DTE xi_serial_number_1,
    PON_SERIAL_NUMBER_DTE xi_serial_number_2);
static PON_ERROR_DTE f_clear_sw_allocation_table(void);
static PON_ERROR_DTE gpon_tx_set_pls_status(bdmf_boolean pls);
static PON_ERROR_DTE gpon_rx_set_alloc_id_cfg(uint32_t tcont_idx,
    uint16_t alloc_id, bdmf_boolean enable_alloc_id);
static uint32_t f_set_preassigned_ted(GPON_DS_US_OVERHEAD_PLOAM *xi_ploam_ptr,
    bool xi_random_flag);
static void p_set_ted(uint8_t *delay);
static void p_aes_state_machine(AES_EVENTS_DTS xi_event, GPON_DS_PLOAM *xi_ploam_ptr);
static void enable_functionalities_when_moving_to_from_operational_state(int enable);
static PON_ERROR_DTE p_send_ack(GPON_DS_PLOAM *xi_downstream_message_ptr);
static PON_ERROR_DTE gpon_tx_set_dba_sr_valid(uint8_t access_number, 
    bdmf_boolean dba_sr_valid_bit);

#ifdef USE_NORMAL_PLAOM_ONLY_FOR_DYING_GASP
#define f_gpon_txpon_send_ploam_message(xi_message_ptr, xx, xi_ploam_buffer_repetition) \
    _f_gpon_txpon_send_ploam_message(xi_message_ptr, URGENT_PLOAM_CELL, xi_ploam_buffer_repetition)
#else
#define f_gpon_txpon_send_ploam_message(xi_message_ptr, xi_ploam_priority, xi_ploam_buffer_repetition) \
    _f_gpon_txpon_send_ploam_message(xi_message_ptr, xi_ploam_priority, xi_ploam_buffer_repetition)
#endif

static PON_ERROR_DTE _f_gpon_txpon_send_base_level_ploam_message(GPON_US_PLOAM *const xi_message_ptr,
    GPON_PLOAM_TRANSMISSION_NUMBER xi_normal_ploam_buffer_repetition, int use_urgent);

static void p_operation_invalid_cross(GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, 
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);

/* State O1 */
static void p_sm_init_o1_state_downstream_clear_los_or_lof_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS * xi_operation_sm_params);

/* State O2 */
static void p_sm_standby_o2_state_upstream_overhead_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_standby_o2_state_downstream_detect_los_or_lof_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_standby_o2_state_disable_serial_number_message_with_disable_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);

/* State O3 */
static void p_sm_serial_number_o3_state_serial_number_request_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_serial_number_o3_state_assigned_onu_id_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_serial_number_o3_state_timer_to1_expire_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_serial_number_o3_state_detect_los_or_lof_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_serial_number_o3_state_disable_serial_number_message_with_disable_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_serial_number_o3_state_extended_burst_length_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_serial_number_o3_state_serial_number_mask_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);

/* State O4 */
static void p_sm_ranging_o4_state_ranging_request_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_ranging_o4_state_ranging_time_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_ranging_o4_state_timer_to1_expire_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_ranging_o4_state_deactivate_onu_id_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_ranging_o4_state_detect_los_or_lof_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_ranging_o4_state_disable_serial_number_message_with_disable_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_ranging_o4_state_change_power_level_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);

/* State O5 */
static void p_sm_operation_o5_state_ranging_request_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_change_power_level_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_ranging_time_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_deactivate_onu_id_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_detect_los_or_lof_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_disable_serial_number_message_with_disable_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_encrypt_port_id_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_assign_alloc_id_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_configure_port_id_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_request_aes_key_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_key_switching_time_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_physical_equipment_error_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_vendor_specific_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_ber_interval_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_request_password_message(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_serial_number_request_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_pst_message(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_sleep_allow_message(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params); 
static void p_sm_operation_o5_state_ranging_adjustment_message_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_operation_o5_state_pon_id_message_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);

/* State O6 */
static void p_sm_popup_o6_state_downstream_clear_los_or_lof_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_popup_o6_state_deactivate_onu_id_message_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_popup_o6_state_broadcast_popup_message_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_popup_o6_state_directed_popup_message_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_popup_o6_state_timer_to2_expire_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_popup_o6_state_detect_los_or_lof_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_popup_o6_state_disable_serial_number_message_with_disable_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_popup_o6_state_swift_popup_message_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);

/* State O7 */
static void p_sm_emergency_stop_o7_state_disable_serial_number_message_with_enable_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_emergency_stop_o7_state_downstream_clear_los_or_lof_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);
static void p_sm_emergency_stop_o7_state_downstream_detect_los_or_lof_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);

typedef void(* OPERATION_CROSS_FUNCTION_DTS)(GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);

static OPERATION_CROSS_FUNCTION_DTS gs_operation_state_machine[OPERATION_STATES_NUMBER][CS_OPERATION_EVENTS_NUMBER] =
{
    /* INIT (O1) */
    {
        /* CLEAR_DS_LOS_OR_LOF */
        p_sm_init_o1_state_downstream_clear_los_or_lof_event,
        /* US_OVERHEAD_MESSAGE */
        p_operation_invalid_cross,
        /* SERIAL_NUMBER_REQUEST_MESSAGE */
        p_operation_invalid_cross,
        /* ASSIGN_ONU_ID_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_REQUEST_MESSAGE */
        p_operation_invalid_cross,
        /* CHANGE_POWER_LEVEL_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_TIME_MESSAGE */
        p_operation_invalid_cross,
        /* TIMER_TO1_EXPIRE */
        p_operation_invalid_cross,
        /* DEACTIVATE_ONU_ID_MESSAGE */
        p_operation_invalid_cross,
        /* DETECT_DS_LOS_OR_LOF */
        p_operation_invalid_cross,
        /* BROADCAST_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* DIRECTED_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* TIMER_TO2_EXPIRE */
        p_operation_invalid_cross,
        /* DISABLE_SERIAL_NUMBER_DISABLE_MESSAGE */
        p_operation_invalid_cross,
        /* DISABLE_SERIAL_NUMBER_ENABLE_MESSAGE  */
        p_operation_invalid_cross,
        /* EXTENDED_BURST_LENGTH_MESSAGE */
        p_operation_invalid_cross,
        /* ENCRYPT_PORT_ID_MESSAGE */
        p_operation_invalid_cross,
        /* ASSIGN_ALLOC_ID_MESSAGE */
        p_operation_invalid_cross,
        /* CONFIGURE_PORT_ID_MESSAGE */
        p_operation_invalid_cross,
        /* KEY_SWITCHING_TIME_MESSAGE */
        p_operation_invalid_cross,
        /* REQUEST_KEY_MESSAGE */
        p_operation_invalid_cross,
        /* PHYSICAL_EQUIPMENT_ERROR_MESSAGE */
        p_operation_invalid_cross,
        /* VENDOR SPECIFIC_MESSAGE */
        p_operation_invalid_cross,
        /* BER_INTERVAL_MESSAGE */
        p_operation_invalid_cross,
        /* UNKNOWN_MESSAGE */
        p_operation_invalid_cross,
        /* REQUEST_PASSWORD */
        p_operation_invalid_cross,
        /* PST_message */
        p_operation_invalid_cross,
        /* SN number mask message */
        p_operation_invalid_cross,
        /* SLEEP_ALLOW_MESSAGE */
        p_operation_invalid_cross, 
        /* PON_ID_MESSAGE */
        p_operation_invalid_cross,
        /* SWIFT_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_ADJUSTMENT_MESSAGE */
        p_operation_invalid_cross
    },

    /* STANDBY (O2) */
    {
        /* CLEAR_DS_LOS_OR_LOF */
        p_operation_invalid_cross,
        /* US_OVERHEAD_MESSAGE */
        p_sm_standby_o2_state_upstream_overhead_message_event,
        /* SERIAL_NUMBER_REQUEST_MESSAGE */
        p_operation_invalid_cross,
        /* ASSIGN_ONU_ID_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_REQUEST_MESSAGE */
        p_operation_invalid_cross,
        /* CHANGE_POWER_LEVEL_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_TIME_MESSAGE */
        p_operation_invalid_cross,
        /* TIMER_TO1_EXPIRE */
        p_operation_invalid_cross,
        /* DEACTIVATE_ONU_ID_MESSAGE */
        p_operation_invalid_cross,
        /* DETECT_DS_LOS_OR_LOF */
        p_sm_standby_o2_state_downstream_detect_los_or_lof_event,
        /* BROADCAST_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* DIRECTED_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* TIMER_TO2_EXPIRE */
        p_operation_invalid_cross,
        /* DISABLE_SERIAL_NUMBER_DISABLE_MESSAGE */
        p_sm_standby_o2_state_disable_serial_number_message_with_disable_event,
        /* DISABLE_SERIAL_NUMBER_ENABLE_MESSAGE  */
        p_operation_invalid_cross,
        /* EXTENDED_BURST_LENGTH_MESSAGE */
        p_operation_invalid_cross,
        /* ENCRYPT_PORT_ID_MESSAGE */
        p_operation_invalid_cross,
        /* ASSIGN_ALLOC_ID_MESSAGE */
        p_operation_invalid_cross,
        /* CONFIGURE_PORT_ID_MESSAGE */
        p_operation_invalid_cross,
        /* KEY_SWITCHING_TIME_MESSAGE */
        p_operation_invalid_cross,
        /* REQUEST_KEY_MESSAGE */
        p_operation_invalid_cross,
        /* PHYSICAL_EQUIPMENT_ERROR_MESSAGE */
        p_operation_invalid_cross,
        /* VENDOR SPECIFIC_MESSAGE */
        p_operation_invalid_cross,
        /* BER_INTERVAL_MESSAGE */
        p_operation_invalid_cross,
        /* UNKNOWN_MESSAGE */
        p_operation_invalid_cross,
        /* REQUEST_PASSWORD */
        p_operation_invalid_cross,
        /* PST_message */
        p_operation_invalid_cross,
        /* SN number mask message */
        p_operation_invalid_cross,
		/* SLEEP_ALLOW_MESSAGE */
        p_operation_invalid_cross, 
        /* PON_ID_MESSAGE */
        p_operation_invalid_cross,
        /* SWIFT_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_ADJUSTMENT_MESSAGE */
        p_operation_invalid_cross
    },

    /* Serial Number (O3) */
    {
        /* CLEAR_DS_LOS_OR_LOF */
        p_operation_invalid_cross,
        /* US_OVERHEAD_MESSAGE */
        p_operation_invalid_cross,
        /* SERIAL_NUMBER_REQUEST_MESSAGE */
        p_sm_serial_number_o3_state_serial_number_request_event,
        /* ASSIGN_ONU_ID_MESSAGE */
        p_sm_serial_number_o3_state_assigned_onu_id_message_event,
        /* RANGING_REQUEST_MESSAGE */
        p_operation_invalid_cross,
        /* CHANGE_POWER_LEVEL_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_TIME_MESSAGE */
        p_operation_invalid_cross,
        /* TIMER_TO1_EXPIRE */
        p_sm_serial_number_o3_state_timer_to1_expire_event,
        /* DEACTIVATE_ONU_ID_MESSAGE */
        p_operation_invalid_cross,
        /* DETECT_DS_LOS_OR_LOF */
        p_sm_serial_number_o3_state_detect_los_or_lof_event,
        /* BROADCAST_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* DIRECTED_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* TIMER_TO2_EXPIRE */
        p_operation_invalid_cross,
        /* DISABLE_SERIAL_NUMBER_DISABLE_MESSAGE */
        p_sm_serial_number_o3_state_disable_serial_number_message_with_disable_event,
        /* DISABLE_SERIAL_NUMBER_ENABLE_MESSAGE  */
        p_operation_invalid_cross,
        /* EXTENDED_BURST_LENGTH_MESSAGE */
        p_sm_serial_number_o3_state_extended_burst_length_message_event,
        /* ENCRYPT_PORT_ID_MESSAGE */
        p_operation_invalid_cross,
        /* ASSIGN_ALLOC_ID_MESSAGE */
        p_operation_invalid_cross,
        /* CONFIGURE_PORT_ID_MESSAGE */
        p_operation_invalid_cross,
        /* KEY_SWITCHING_TIME_MESSAGE */
        p_operation_invalid_cross,
        /* REQUEST_KEY_MESSAGE */
        p_operation_invalid_cross,
        /* PHYSICAL_EQUIPMENT_ERROR_MESSAGE */
        p_operation_invalid_cross,
        /* VENDOR SPECIFIC_MESSAGE */
        p_operation_invalid_cross,
        /* BER_INTERVAL_MESSAGE */
        p_operation_invalid_cross,
        /* UNKNOWN_MESSAGE */
        p_operation_invalid_cross,
        /* REQUEST_PASSWORD */
        p_operation_invalid_cross,
        /* PST_message */
        p_operation_invalid_cross,
        /* SN number mask message */
        p_sm_serial_number_o3_state_serial_number_mask_message_event,
		/* SLEEP_ALLOW_MESSAGE */
        p_operation_invalid_cross, 
        /* PON_ID_MESSAGE */
        p_operation_invalid_cross,
        /* SWIFT_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_ADJUSTMENT_MESSAGE */
        p_operation_invalid_cross
    },
    /* Ranging (O4) */
    {
        /* CLEAR_DS_LOS_OR_LOF */
        p_operation_invalid_cross,
        /* US_OVERHEAD_MESSAGE */
        p_operation_invalid_cross,
        /* SERIAL_NUMBER_REQUEST_MESSAGE */
        p_operation_invalid_cross,
        /* ASSIGN_ONU_ID_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_REQUEST_MESSAGE */
        p_sm_ranging_o4_state_ranging_request_message_event,
        /* CHANGE_POWER_LEVEL_MESSAGE */
        p_sm_ranging_o4_state_change_power_level_message_event,
        /* RANGING_TIME_MESSAGE */
        p_sm_ranging_o4_state_ranging_time_message_event,
        /* TIMER_TO1_EXPIRE */
        p_sm_ranging_o4_state_timer_to1_expire_event,
        /* DEACTIVATE_ONU_ID_MESSAGE */
        p_sm_ranging_o4_state_deactivate_onu_id_message_event,
        /* DETECT_DS_LOS_OR_LOF */
        p_sm_ranging_o4_state_detect_los_or_lof_event,
        /* BROADCAST_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* DIRECTED_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* TIMER_TO2_EXPIRE */
        p_operation_invalid_cross,
        /* DISABLE_SERIAL_NUMBER_DISABLE_MESSAGE */
        p_sm_ranging_o4_state_disable_serial_number_message_with_disable_event,
        /* DISABLE_SERIAL_NUMBER_ENABLE_MESSAGE  */
        p_operation_invalid_cross,
        /* EXTENDED_BURST_LENGTH_MESSAGE */
        p_operation_invalid_cross,
        /* ENCRYPT_PORT_ID_MESSAGE */
        p_operation_invalid_cross,
        /* ASSIGN_ALLOC_ID_MESSAGE */
        p_operation_invalid_cross,
        /* CONFIGURE_PORT_ID_MESSAGE */
        p_operation_invalid_cross,
        /* KEY_SWITCHING_TIME_MESSAGE */
        p_operation_invalid_cross,
        /* REQUEST_KEY_MESSAGE */
        p_operation_invalid_cross,
        /* PHYSICAL_EQUIPMENT_ERROR_MESSAGE */
        p_operation_invalid_cross,
        /* VENDOR SPECIFIC_MESSAGE */
        p_operation_invalid_cross,
        /* BER_INTERVAL_MESSAGE */
        p_operation_invalid_cross,
        /* UNKNOWN_MESSAGE */
        p_operation_invalid_cross,
        /* REQUEST_PASSWORD */
        p_operation_invalid_cross,
        /* PST_message */
        p_operation_invalid_cross,
        /* SN number mask message */
        p_operation_invalid_cross,
		/* SLEEP_ALLOW_MESSAGE */
        p_operation_invalid_cross, 
        /* PON_ID_MESSAGE */
        p_operation_invalid_cross,
        /* SWIFT_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_ADJUSTMENT_MESSAGE */
        p_operation_invalid_cross
    },

    /* Operation (O5) */
    {
        /* CLEAR_DS_LOS_OR_LOF */
        p_operation_invalid_cross,
        /* US_OVERHEAD_MESSAGE */
        p_operation_invalid_cross,
        /* SERIAL_NUMBER_REQUEST_MESSAGE */
        p_sm_operation_o5_state_serial_number_request_event,
        /* ASSIGN_ONU_ID_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_REQUEST_MESSAGE */
        p_sm_operation_o5_state_ranging_request_message_event,
        /* CHANGE_POWER_LEVEL_MESSAGE */
        p_sm_operation_o5_state_change_power_level_message_event,
        /* RANGING_TIME_MESSAGE */
        p_sm_operation_o5_state_ranging_time_message_event,
        /* TIMER_TO1_EXPIRE */
        p_operation_invalid_cross,
        /* DEACTIVATE_ONU_ID_MESSAGE */
        p_sm_operation_o5_state_deactivate_onu_id_message_event,
        /* DETECT_DS_LOS_OR_LOF */
        p_sm_operation_o5_state_detect_los_or_lof_event,
        /* BROADCAST_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* DIRECTED_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* TIMER_TO2_EXPIRE */
        p_operation_invalid_cross,
        /* DISABLE_SERIAL_NUMBER_DISABLE_MESSAGE */
        p_sm_operation_o5_state_disable_serial_number_message_with_disable_event,
        /* DISABLE_SERIAL_NUMBER_ENABLE_MESSAGE  */
        p_operation_invalid_cross,
        /* EXTENDED_BURST_LENGTH_MESSAGE */
        p_operation_invalid_cross,
        /* ENCRYPT_PORT_ID_MESSAGE */
        p_sm_operation_o5_state_encrypt_port_id_message,
        /* ASSIGN_ALLOC_ID_MESSAGE */
        p_sm_operation_o5_state_assign_alloc_id_message,
        /* CONFIGURE_PORT_ID_MESSAGE */
        p_sm_operation_o5_state_configure_port_id_message,
        /* KEY_SWITCHING_TIME_MESSAGE */
        p_sm_operation_o5_state_key_switching_time_message,
        /* REQUEST_KEY_MESSAGE */
        p_sm_operation_o5_state_request_aes_key_message,
        /* PHYSICAL_EQUIPMENT_ERROR_MESSAGE */
        p_sm_operation_o5_state_physical_equipment_error_message,
        /* VENDOR SPECIFIC_MESSAGE */
        p_sm_operation_o5_state_vendor_specific_message,
        /* BER_INTERVAL_MESSAGE */
        p_sm_operation_o5_state_ber_interval_message,
        /* UNKNOWN_MESSAGE */
        p_operation_invalid_cross,
        /* REQUEST_PASSWORD */
        p_sm_operation_o5_state_request_password_message,
        /* PST_message */
        p_sm_operation_o5_state_pst_message,
        /* SN number mask message */
        p_operation_invalid_cross,
		/* SLEEP_ALLOW_MESSAGE */
        p_sm_operation_o5_state_sleep_allow_message, 
        /* PON_ID_MESSAGE */
        p_sm_operation_o5_state_pon_id_message_event,
        /* SWIFT_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_ADJUSTMENT_MESSAGE */
        p_sm_operation_o5_state_ranging_adjustment_message_event
    },
    /* Pop-Up (O6) */
    {
        /* CLEAR_DS_LOS_OR_LOF */
        p_sm_popup_o6_state_downstream_clear_los_or_lof_event,
        /* US_OVERHEAD_MESSAGE */
        p_operation_invalid_cross,
        /* SERIAL_NUMBER_REQUEST_MESSAGE */
        p_operation_invalid_cross,
        /* ASSIGN_ONU_ID_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_REQUEST_MESSAGE */
        p_operation_invalid_cross,
        /* CHANGE_POWER_LEVEL_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_TIME_MESSAGE */
        p_operation_invalid_cross,
        /* TIMER_TO1_EXPIRE */
        p_operation_invalid_cross,
        /* DEACTIVATE_ONU_ID_MESSAGE */
        p_sm_popup_o6_state_deactivate_onu_id_message_event,
        /* DETECT_DS_LOS_OR_LOF */
        p_sm_popup_o6_state_detect_los_or_lof_event,
        /* BROADCAST_POPUP_MESSAGE */
        p_sm_popup_o6_state_broadcast_popup_message_event,
        /* DIRECTED_POPUP_MESSAGE */
        p_sm_popup_o6_state_directed_popup_message_event,
        /* TIMER_TO2_EXPIRE */
        p_sm_popup_o6_state_timer_to2_expire_event,
        /* DISABLE_SERIAL_NUMBER_DISABLE_MESSAGE */
        p_sm_popup_o6_state_disable_serial_number_message_with_disable_event,
        /* DISABLE_SERIAL_NUMBER_ENABLE_MESSAGE  */
        p_operation_invalid_cross,
        /* EXTENDED_BURST_LENGTH_MESSAGE */
        p_operation_invalid_cross,
        /* ENCRYPT_PORT_ID_MESSAGE */
        p_operation_invalid_cross,
        /* ASSIGN_ALLOC_ID_MESSAGE */
        p_operation_invalid_cross,
        /* CONFIGURE_PORT_ID_MESSAGE */
        p_operation_invalid_cross,
        /* KEY_SWITCHING_TIME_MESSAGE */
        p_operation_invalid_cross,
        /* REQUEST_KEY_MESSAGE */
        p_operation_invalid_cross,
        /* PHYSICAL_EQUIPMENT_ERROR_MESSAGE */
        p_operation_invalid_cross,
        /* VENDOR SPECIFIC_MESSAGE */
        p_operation_invalid_cross,
        /* BER_INTERVAL_MESSAGE */
        p_operation_invalid_cross,
        /* UNKNOWN_MESSAGE */
        p_operation_invalid_cross,
        /* REQUEST_PASSWORD */
        p_operation_invalid_cross,
        /* PST_message */
        p_operation_invalid_cross,
        /* SN number mask message */
        p_operation_invalid_cross,
		/* SLEEP_ALLOW_MESSAGE */
        p_operation_invalid_cross,
        /* PON_ID_MESSAGE */
        p_operation_invalid_cross,
        /* SWIFT_POPUP_MESSAGE */
        p_sm_popup_o6_state_swift_popup_message_event,
        /* RANGING_ADJUSTMENT_MESSAGE */
        p_operation_invalid_cross
    },
    /* Emergency-Stop (O7) */
    {
        /* CLEAR_DS_LOS_OR_LOF */
        p_sm_emergency_stop_o7_state_downstream_clear_los_or_lof_event,
        /* US_OVERHEAD_MESSAGE */
        p_operation_invalid_cross,
        /* SERIAL_NUMBER_REQUEST_MESSAGE */
        p_operation_invalid_cross,
        /* ASSIGN_ONU_ID_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_REQUEST_MESSAGE */
        p_operation_invalid_cross,
        /* CHANGE_POWER_LEVEL_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_TIME_MESSAGE */
        p_operation_invalid_cross,
        /* TIMER_TO1_EXPIRE */
        p_operation_invalid_cross,
        /* DEACTIVATE_ONU_ID_MESSAGE */
        p_operation_invalid_cross,
        /* DETECT_DS_LOS_OR_LOF */
        p_sm_emergency_stop_o7_state_downstream_detect_los_or_lof_event,
        /* BROADCAST_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* DIRECTED_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* TIMER_TO2_EXPIRE */
        p_operation_invalid_cross,
        /* DISABLE_SERIAL_NUMBER_DISABLE_MESSAGE */
        p_operation_invalid_cross,
        /* DISABLE_SERIAL_NUMBER_ENABLE_MESSAGE  */
        p_sm_emergency_stop_o7_state_disable_serial_number_message_with_enable_event,
        /* EXTENDED_BURST_LENGTH_MESSAGE */
        p_operation_invalid_cross,
        /* ENCRYPT_PORT_ID_MESSAGE */
        p_operation_invalid_cross,
        /* ASSIGN_ALLOC_ID_MESSAGE */
        p_operation_invalid_cross,
        /* CONFIGURE_PORT_ID_MESSAGE */
        p_operation_invalid_cross,
        /* KEY_SWITCHING_TIME_MESSAGE */
        p_operation_invalid_cross,
        /* REQUEST_KEY_MESSAGE */
        p_operation_invalid_cross,
        /* PHYSICAL_EQUIPMENT_ERROR_MESSAGE */
        p_operation_invalid_cross,
        /* VENDOR SPECIFIC_MESSAGE */
        p_operation_invalid_cross,
        /* BER_INTERVAL_MESSAGE */
        p_operation_invalid_cross,
        /* UNKNOWN_MESSAGE */
        p_operation_invalid_cross,
        /* REQUEST_PASSWORD */
        p_operation_invalid_cross,
        /* PST_message */
        p_operation_invalid_cross,
        /* SN number mask message */
        p_operation_invalid_cross,
		/* SLEEP_ALLOW_MESSAGE */
        p_operation_invalid_cross, 
        /* PON_ID_MESSAGE */
        p_operation_invalid_cross,
        /* SWIFT_POPUP_MESSAGE */
        p_operation_invalid_cross,
        /* RANGING_ADJUSTMENT_MESSAGE */
        p_operation_invalid_cross
    }
};

static void p_aes_invalid_cross(GPON_AES_STATE_MACHINE_DTE *xi_aes_state_machine);
static void p_aes_send_new_key(GPON_AES_STATE_MACHINE_DTE *xi_aes_state_machine);
static void p_aes_setting_new_key_switching_time(GPON_AES_STATE_MACHINE_DTE *xi_aes_state_machine);

typedef void(* AES_CROSS_FUNCTION_DTS)(GPON_AES_STATE_MACHINE_DTE *xi_aes_state_machine);

static AES_CROSS_FUNCTION_DTS gs_aes_state_machine[CS_AES_STATES_NUMBER][CS_AES_EVENTS_NUMBER] =
{
    /* Not Ready */
    {
        /* Request key message */
        p_aes_invalid_cross,
        /* Key switching time message */
        p_aes_invalid_cross
    },
    /* STANDBY */
    {
        /* Request key message */
        p_aes_send_new_key,
        /* Key switching time message */
        p_aes_invalid_cross
    },
    /* SCHEDULING */
    {
        /* Request key message */
        p_aes_send_new_key,
        /* Key switching time message */
        p_aes_setting_new_key_switching_time
    }
};


/* State Machine functions */
static void p_operation_invalid_cross(GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, 
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    /* Invalid cross */
    p_log(ge_onu_logger.sections.stack.operating.invalid_cross, 
        "Activation state machine:Invalid cross");
}

/* State O1 */
static void p_sm_init_o1_state_downstream_clear_los_or_lof_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    int drv_error; 

    /* Set Broadcast ONU-ID to 255 (RX block) */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(1, 
        gs_gpon_database.onu_parameters.broadcast_default_value, BDMF_FALSE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to Enable Rx Broadcast ONU-ID due to driver error %d.", drv_error);
        return;
    }

    /* Send Alarm indication for appropriate LCDG/LOF/LOS indication(via PON task ) */
    if (xi_operation_sm_params -> lof_state == BDMF_FALSE)
    {
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM, PON_INDICATION_LOF,
            CE_OAM_ALARM_STATUS_OFF);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d", gpon_error);
        }
    }

    /* Report event "Link state transition" with state parameter [Standby] */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_STATE_TRANSITION, 
        xi_operation_state_machine->link_sub_state, LINK_SUB_STATE_STANDBY);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
        return;
    }

    /* Change link sub-state to STANDBY */
    xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_STANDBY;

    /* Change internal state to Standby (O2) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O2;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link State Transition: INIT_O1->STANDBY_O2");

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
        OPERATION_STATE_O1, OPERATION_STATE_O2);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
        return;
    }

    if (ls_state_was_o2_before == BDMF_FALSE)
    {
        ls_state_was_o2_before = BDMF_TRUE;
    }
}


static void p_sm_standby_o2_state_upstream_overhead_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine, 
    OPERATION_SM_PARAMS_DTS * xi_operation_sm_params)
{
    uint32_t i;
    uint16_t random_delay;
    uint32_t power_level;
    bdmf_error_t bdmf_error;
    PON_ERROR_DTE gpon_error;
    uint32_t preamble_length_bits;
    GPON_TX_PLOAM_MESSAGE sn_ploam_content;
    GPON_DS_US_OVERHEAD_PLOAM upstream_overhead_ploam;
    GPON_POWER_LEVEL_PARAMETERS_DTE power_level_param;
    bool extended_burst_length_valid_flag = BDMF_FALSE;
    uint32_t number_of_type_3 = 0;
    int drv_error;

    /* reset SN mask status */
    gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_matched = BDMF_FALSE;
    gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_status = BDMF_FALSE;

    /* Send Alarm indication for Ranging_start indication(via PON task ) */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION,
        PON_INDICATION_RANGING_START);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send oam indication: error %d !", gpon_error);
    }

    gs_gpon_database.onu_parameters.number_of_guard_bits = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.number_of_guard_bits;
    gs_gpon_database.onu_parameters.number_of_type_1_preamble_bits = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.number_of_type_1_preamble_bits;
    gs_gpon_database.onu_parameters.number_of_type_2_preamble_bits = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.number_of_type_2_preamble_bits;

    /* Set Type 3 preamble pattern */
    gs_gpon_database.onu_parameters.pattern_of_type_3_preamble_bits = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.pattern_of_type_3_preamble_bits;

    /* Setting the delimiter pattern */
    memcpy(gs_gpon_database.onu_parameters.delimiter, 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.delimiter, GPON_DELIMITER_SIZE);
    gs_gpon_database.onu_parameters.pre_equalization_status =
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.options.pre_equalization_status == 
        CE_PRE_EQUALIZATION_DELAY_DONT_USE ? BDMF_FALSE : BDMF_TRUE;
    gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_status = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.options.serial_number_mask_status;
    gs_gpon_database.onu_parameters.extra_serial_number_transmissions = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.options.extra_serial_number_transmissions + 1;
    power_level = xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.options.power_level_mode;

    switch (power_level)
    {
    case CE_POWER_LEVEL_MODE_0_NORMAL:
        gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_HIGH;
        break;
    case CE_POWER_LEVEL_MODE_1_NORMAL_3_DB:
        gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_NORMAL;
        break;
    case CE_POWER_LEVEL_MODE_2_NORMAL_6_DB:
        gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_LOW;
        break;
    default:
        gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_ILLEGAL;
        break;
    }

    /* Pre-assigned EqD : most significant Byte in 32 Bytes units */
    gs_gpon_database.onu_parameters.preassigned_equalization_delay = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.preassigned_equalization_delay[0] << 8;

    /* Pre-assigned EqD : least significant Byte in 32 Bytes units */
    gs_gpon_database.onu_parameters.preassigned_equalization_delay |= 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.preassigned_equalization_delay[1];

    gs_preassigned_equalization_delay = gs_gpon_database.onu_parameters.preassigned_equalization_delay;

     p_log(ge_onu_logger.sections.stack.operating.general_id, "Set PON parameters\n"
         " Number_of_guard_bits: %d\n"
         " Number_of_type_1_preamble_bits: %d [bits]\n"
         " Number_of_type_2_preamble_bits: %d [bits]\n",
         gs_gpon_database.onu_parameters.number_of_guard_bits,
         gs_gpon_database.onu_parameters.number_of_type_1_preamble_bits,
         gs_gpon_database.onu_parameters.number_of_type_2_preamble_bits);

    p_log(ge_onu_logger.sections.stack.debug.general_id, " Pattern_of_type_3_preamble_bits: 0x%02x\n"
        " Set serial number status to: %s\n" ,
        gs_gpon_database.onu_parameters.pattern_of_type_3_preamble_bits ,
        (gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_status == 
             CE_SERIAL_NUMBER_MASK_DISABLED ? "disabled" : "enabled" ));


    p_log(ge_onu_logger.sections.stack.debug.general_id, " Delimiter pattern[0]: 0x%02x \n"
        " Delimiter pattern[1]: 0x%02x \n"
        " Delimiter pattern[2]: 0x%02x \n",
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.delimiter[ 0 ],
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.delimiter[ 1 ],
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.delimiter[ 2 ]);


    p_log(ge_onu_logger.sections.stack.debug.general_id, " Extra_serial_number_transmissions: %d\n"
        " Set default power level mode to: %d\n"
        " Pre_equalization_status: %s\n"
        " Preassigned_equalization_delay: 0x%02x \n",
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.options.extra_serial_number_transmissions,
        gs_gpon_database.onu_parameters.default_power_level_mode,
        (xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.options.pre_equalization_status == 
        CE_PRE_EQUALIZATION_DELAY_DONT_USE ? "disabled" : "enabled"),
        gs_gpon_database.onu_parameters.preassigned_equalization_delay);

    /* Setting Upstream Overhead ploam to config the HW */
    upstream_overhead_ploam.number_of_guard_bits = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.number_of_guard_bits;
    upstream_overhead_ploam.number_of_type_1_preamble_bits = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.number_of_type_1_preamble_bits;
    upstream_overhead_ploam.number_of_type_2_preamble_bits = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.number_of_type_2_preamble_bits;
    upstream_overhead_ploam.pattern_of_type_3_preamble_bits = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.pattern_of_type_3_preamble_bits;
    memcpy(upstream_overhead_ploam.delimiter, 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.delimiter, GPON_DELIMITER_SIZE);
    upstream_overhead_ploam.options.pre_equalization_status =
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.options.pre_equalization_status == 
        CE_PRE_EQUALIZATION_DELAY_DONT_USE ? BDMF_FALSE : BDMF_TRUE;
    upstream_overhead_ploam.options.serial_number_mask_status = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.options.serial_number_mask_status;
    upstream_overhead_ploam.options.extra_serial_number_transmissions = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.options.extra_serial_number_transmissions;
    upstream_overhead_ploam.options.power_level_mode = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.options.power_level_mode;

    /* Pre-assigned EqD : most significant Byte in 32 Bytes units */
    upstream_overhead_ploam.preassigned_equalization_delay[0] = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.preassigned_equalization_delay[0];

    /* Pre-assigned EqD : least significant Byte in 32 Bytes units */
    upstream_overhead_ploam.preassigned_equalization_delay[1] = 
        xi_operation_sm_params->ploam_message_ptr->message.upstream_overhead.preassigned_equalization_delay[1];

    /* Check that T1 + T2 + GT <= 16 bytes and modulo test - else error */
    preamble_length_bits = upstream_overhead_ploam.number_of_type_1_preamble_bits +
        upstream_overhead_ploam.number_of_type_2_preamble_bits + upstream_overhead_ploam.number_of_guard_bits;

    if (((upstream_overhead_ploam.number_of_type_1_preamble_bits + upstream_overhead_ploam.number_of_type_2_preamble_bits) > GPON_TX_PREAMBLE_TYPE1_TYPE2_MAX_LENGTH)
         || (preamble_length_bits % 8) != 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Stack Error: The sum of T1 & T2 & GT overflow preamble length OR failed in ,modulo 8 criteria : T1 %d [bits], T2 %d [bits], GT %d [bits]",
            upstream_overhead_ploam.number_of_type_1_preamble_bits,
            upstream_overhead_ploam.number_of_type_2_preamble_bits,
            upstream_overhead_ploam.number_of_guard_bits);
        return;
    }

    /* reset extended burst to false */
    gs_gpon_database.onu_parameters.extended_burst_length_valid_flag = BDMF_FALSE;

    /* In PMD cold acquisition use longest possible transmission during O3 state */
    /*if (gs_gpon_database.OpticsType == BP_GPON_OPTICS_TYPE_PMD)
    {
        extended_burst_length_valid_flag = BDMF_TRUE;
        number_of_type_3 = PMD_NUMBER_OF_TYPE_3_PREAMBLE;
    }*/

    if (gs_gpon_database.OpticsType == BP_GPON_OPTICS_TYPE_PMD)
    {
        drv_error = ag_drv_gpon_tx_general_configuration_dvstp_set(
            gs_gpon_database.physical_parameters.transceiver_dv_setup_pattern);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to set tranceiver dv setup pattern due to driver error %d.", drv_error);
        }

        drv_error = ag_drv_gpon_tx_general_configuration_dvhld_set(
            gs_gpon_database.physical_parameters.transceiver_dv_hold_pattern);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to set tranceiver hold pattern due to driver error %d.", drv_error);
        }
    }

    /* HW - Set Pon upstream overhead - preamble & delimiter */
    gpon_error = gpon_tx_set_pon_overhead_and_length (upstream_overhead_ploam ,
        extended_burst_length_valid_flag, number_of_type_3);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Driver error %d !", gpon_error);
    }

    /* Set pre-equalization delay */
    random_delay = f_set_preassigned_ted (&upstream_overhead_ploam, BDMF_TRUE);

    /* Set default ONU transmit power level */
    power_level_param.power_level_mode = gs_gpon_database.onu_parameters.default_power_level_mode;
    power_level_param.reason = GPON_CHANGE_POWER_LEVEL_REASON_US_OVERHEAD_PLOAM;
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_CHANGE_POWER_LEVEL , power_level_param);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to change power level: error %d !", gpon_error);
    }
    
    /* Start TO1 timer */
    gs_gpon_database.link_parameters.received_serial_number_request = BDMF_FALSE;
    bdmf_error = bdmf_timer_start(& gs_gpon_database.os_resources.to1_timer_id, 
        GPON_MILISEC_TO_TICKS(gs_gpon_database.onu_parameters.to1_timer_timeout));
    if (bdmf_error != BDMF_ERR_OK )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to start TO1 timer");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start TO1 timer");
    }

    /* Set Ranging buffer for ONU s/n message with ONU-ID for Broadcast */
    /* Pon-id not assigned yet */
    sn_ploam_content.onu_id = 0xff;
    sn_ploam_content.message_id = CE_US_SERIAL_NUMBER_ONU_MESSAGE_ID;

    for (i = 0; i < VENDOR_CODE_FIELD_SIZE; i++)
    {
        sn_ploam_content.message_payload[i] = gs_gpon_database.physical_parameters.serial_number.vendor_code[i];
    }

    for (i = 0; i < VENDOR_SPECIFIC_FIELD_SIZE; i++)
    {
        sn_ploam_content.message_payload[VENDOR_CODE_FIELD_SIZE + i] = gs_gpon_database.physical_parameters.serial_number.serial_number[i];
    }

    sn_ploam_content.message_payload[8] = (random_delay & 0xff0) >> 4;
    sn_ploam_content.message_payload[9] = ((random_delay & 0xf) << 4) | 0x04 | gs_gpon_database.onu_parameters.default_power_level_mode;

    /* The crc field will be calculate from tx driver level */
    sn_ploam_content.payload_crc = 0;

    gpon_error = gpon_tx_set_sn_ranging_ploam_content(&sn_ploam_content);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to set SN ranging PLOAM!");
    }

    /* Reset the serial number request messages counter */
    gs_gpon_database.link_parameters.serial_number_request_messages_counter = 0;

    /* Set Rx Alloc Id (254) */
    if (gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_status == BDMF_TRUE)
    {
        gpon_error = gpon_rx_set_ranging_alloc_id_configuration(
            GPON_RX_RANGING_ALLOC_ID_DEF, BDMF_FALSE);
    }
    else
    {
        gpon_error = gpon_rx_set_ranging_alloc_id_configuration(
            GPON_RX_RANGING_ALLOC_ID_DEF, BDMF_TRUE);
    }

    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to set Rx ranging alloc id table and its valid bit due to Driver error %d!!", 
            gpon_error);
    }

    /* Disable and clear Alloc-Id in the 0th entry of the Alloc id table - disable OMCI channel */
    gpon_error = gpon_rx_set_alloc_id_cfg(GPON_OMCI_ALLOC_ID_INDEX,
        GPON_DEF_ALLOC_ID, BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear OMCI Rx alloc id and its valid bit due to Driver error %d alloc_id_index %d!", 
            gpon_error, GPON_OMCI_ALLOC_ID_INDEX);
        return;
    }

    /* Clear TX ONU ID */
    drv_error = ag_drv_gpon_tx_general_configuration_onuid_set(GPON_TX_ONU_ID_DEF);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Tx ONU-ID due to driver error %d.", drv_error);
       return;
    }

    /* Set / Clear the PLS bit according user configuration */
    if ((gs_gpon_database.OpticsType == BP_GPON_OPTICS_TYPE_PMD) && (!pmdFirstBurstDone))
    	drv_error = gpon_tx_set_pls_status(BDMF_TRUE);
    else
    	drv_error = gpon_tx_set_pls_status(gs_gpon_database.tx_parameters.tx_pls_enable);

    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Tx PLS status due to driver error %d", drv_error);
        return;
    }

    /* Set Ranging valid bit */
    gpon_error = gpon_set_ranging_valid_bit(BDMF_TRUE);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to Set and TX ranging valid bit due to driver error %d !", gpon_error);
        return;
    }

    /* Send user indication "DACT" - off */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM ,
        PON_INDICATION_DACT, BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send oam indication: error %d !", gpon_error);
    }

    /* Change internal state to SERIAL NUMBER (O3) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O3;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link State Transition: STANDBY_O2->SERIAL_NUMBER_O3");

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION ,
        OPERATION_STATE_O2, OPERATION_STATE_O3);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
        return;
    }
}

static void p_sm_standby_o2_state_downstream_detect_los_or_lof_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    int drv_error;

    /* Clear Broadcast Id and valid bit */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(1, 
        gs_gpon_database.onu_parameters.broadcast_default_value, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx Broadcast ONU-ID due to driver error %d.", drv_error);
    }

    if (xi_operation_sm_params->lof_state == BDMF_TRUE)
    {
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM ,
            PON_INDICATION_LOF, CE_OAM_ALARM_STATUS_ON);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d !", gpon_error);
            return;
        }
    }

    /* Change internal state to INIT (O1) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O1;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link State Transition: STANDBY_O2->INIT_O1");

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION ,
        OPERATION_STATE_O2, OPERATION_STATE_O1);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
        return;
    }
}

static void p_sm_standby_o2_state_disable_serial_number_message_with_disable_event( 
    GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS * xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    PON_SERIAL_NUMBER_DTE serial_number;

    /* Get the serial number */
    memcpy((char *)&serial_number, 
        xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.serial_number,
        sizeof(PON_SERIAL_NUMBER_DTE));

    /* Disabled unicast & Serial number match?  or Disabled multicast*/
    if (xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.control == CE_SERIAL_NUMBER_UNICAST_DISABLE &&
        (f_serial_number_match(serial_number, gs_gpon_database.physical_parameters.serial_number) == BDMF_TRUE))
    {
        /* Report event "Link sub state transition" with state parameter [Stop] */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_STATE_TRANSITION,
            xi_operation_state_machine->link_sub_state, LINK_SUB_STATE_STOP);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send user indication: error %d !", gpon_error);
            return;
        }
        
        /* Send user indication "DIS" - On */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM,
            PON_INDICATION_DIS, BDMF_TRUE);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d !", gpon_error);
        }
        
        /* Change internal state to EMERGENCY_STOP (O7) */
        xi_operation_state_machine->activation_state = OPERATION_STATE_O7;
        
        /* Change link sub-state to Stop */
        xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_STOP;
        
        p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
            "Link State Transition: STANDBY_O2->EMERGENCY_STOP_O7");
        
        /* Report event "Link state transition" with state parameter */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION ,
            OPERATION_STATE_O2, OPERATION_STATE_O7);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send user indication: error %d !", gpon_error);
            return;
        }
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.operating.general_id, "Serial number mismatch!");
    }
}

/* State O3 */
static void p_sm_serial_number_o3_state_serial_number_request_event( 
    GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS * xi_operation_sm_params)
{
    uint16_t random_delay;
    GPON_DS_US_OVERHEAD_PLOAM upstream_overhead_ploam;
    GPON_POWER_LEVEL_PARAMETERS_DTE power_level_param;
    GPON_TX_PLOAM_MESSAGE sn_ploam_content;
    PON_ERROR_DTE gpon_error;

    if (( gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_status == BDMF_TRUE ) &&
        (gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_matched == BDMF_FALSE ) )
    {
        /* Invalid cross */
        p_log(ge_onu_logger.sections.stack.operating.invoke_state_machine_id, "Activation state machine:Invalid cross");
        return;
    }

    /* Get preassigned equalization delay from the database */
    upstream_overhead_ploam.preassigned_equalization_delay [ 0 ] = 
        (gs_gpon_database.onu_parameters.preassigned_equalization_delay >> 8) & 0x000000FF;
    upstream_overhead_ploam.preassigned_equalization_delay [ 1 ] = 
        gs_gpon_database.onu_parameters.preassigned_equalization_delay & 0x000000FF;

    /* Set preassigned equalization delay */
    random_delay = f_set_preassigned_ted(&upstream_overhead_ploam, BDMF_TRUE);

    gpon_error = gpon_tx_get_sn_ranging_ploam_content(&sn_ploam_content);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    /* update random delay - in 32 byte units */
    sn_ploam_content.message_payload [ 8 ] = (random_delay & 0xff0) >> 4;
    sn_ploam_content.message_payload [ 9 ] = ((random_delay & 0xf) << 4) | 0x04 | gs_gpon_database.onu_parameters.default_power_level_mode;

    /* The crc field will be calculate from tx driver level */
    sn_ploam_content.payload_crc = 0;
    gpon_error = gpon_tx_set_sn_ranging_ploam_content(&sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    /* change the flag of serial number request */
    gs_gpon_database.link_parameters.received_serial_number_request = BDMF_TRUE;

    /* Increment the serial number request messages counter */
    gs_gpon_database.link_parameters.serial_number_request_messages_counter ++;
    p_log(ge_onu_logger.sections.stack.operating.general_id, 
        "Increment the serial number request messages counter %d ", 
        gs_gpon_database.link_parameters.serial_number_request_messages_counter);

    /* Check if SN erquest threshold is crossed to apply the SN_request threshold event */
    if (gs_gpon_database.link_parameters.serial_number_request_messages_counter >= CE_SN_REQUEST_THRESHOLD )
    {
        /* Reset the serial number request messages counter */
        p_log(ge_onu_logger.sections.stack.operating.general_id, 
            "Reset the serial number request messages counter");

        gs_gpon_database.link_parameters.serial_number_request_messages_counter = 0;

        /* ONU change it's power level -  via I2C (INCREASE) */
        switch (gs_gpon_database.onu_parameters.default_power_level_mode)
        {
        case GPON_POWER_LEVEL_LOW:
            gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_NORMAL;
            break;
        case GPON_POWER_LEVEL_NORMAL:
            gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_HIGH;
            break;
        case GPON_POWER_LEVEL_HIGH:
            gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_LOW;
            break;
        default:
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "power level is illegal");
            break;
        }

        power_level_param.power_level_mode = gs_gpon_database.onu_parameters.default_power_level_mode;
        power_level_param.reason = GPON_CHANGE_POWER_LEVEL_REASON_SN_REQUEST_THRESHOLD;
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_CHANGE_POWER_LEVEL, power_level_param);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to change power level: error %d !", gpon_error);
        }
   }
}

static void p_sm_serial_number_o3_state_assigned_onu_id_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine, 
    OPERATION_SM_PARAMS_DTS * xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    PON_SERIAL_NUMBER_DTE serial_number;
    GPON_TX_PLOAM_MESSAGE sn_ploam_content;
    GPON_DS_US_OVERHEAD_PLOAM upstream_overhead_ploam;
    PON_ALLOC_ID_DTE assign_alloc_id;
    uint32_t table_index = 0;
    uint32_t alloc_index = 0;
    bool alloc_id_found = BDMF_FALSE;
    int drv_error;

    /* restore pre_range_type_3_number Incase it was change for PMD cold acquisition */
    if (gs_gpon_database.onu_parameters.extended_burst_length_valid_flag == BDMF_TRUE)
    {
        p_extended_length(gs_gpon_database.onu_parameters.pre_range_type_3_number);
    }
    else
    {
        p_extended_length(0);
    }

    /* Get the serial number */
    memcpy((char *)&serial_number,
        xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.serial_number,
        sizeof(PON_SERIAL_NUMBER_DTE));

    /* Serial number match? */
    if (f_serial_number_match(serial_number,
        gs_gpon_database.physical_parameters.serial_number) == BDMF_TRUE)
    {
        /* Assign ONU-ID */
        p_log(ge_onu_logger.sections.stack.operating.general_id, "Assign ONU-ID (%d)",
            xi_operation_sm_params->ploam_message_ptr->message.assign_onu_id.assigned_onu_id);
        
        gs_gpon_database.onu_parameters.onu_id = 
            xi_operation_sm_params->ploam_message_ptr->message.assign_onu_id.assigned_onu_id;
        /* Clear Ranging valid bit */
        gpon_error = gpon_set_ranging_valid_bit (BDMF_FALSE);
        if (gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                    "Unable to Set and TX ranging valid bit due to driver error %d !",
                    gpon_error);
        }
        
        /* Set TX ONU-ID */
        drv_error = ag_drv_gpon_tx_general_configuration_onuid_set(gs_gpon_database.onu_parameters.onu_id);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to set Tx ONU-ID due to driver error %d.", drv_error);
            return;
        }
        
        /* Update the Ranging buffer with S/N message with assigned ONU-ID Set Tx ONU-ID */
        gpon_error = gpon_tx_get_sn_ranging_ploam_content (& sn_ploam_content);
        if (gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
        }
        
        sn_ploam_content.onu_id = gs_gpon_database.onu_parameters.onu_id;
        /* set random delay to 0*/
        sn_ploam_content.message_payload[8] = 0;
        sn_ploam_content.message_payload[9] = sn_ploam_content.message_payload[9] & 0x0f;
        
        /* Set SN ploam with new assigned ONU-ID */
        gpon_error = gpon_tx_set_sn_ranging_ploam_content(&sn_ploam_content);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Failed to set ranging ploam content");
        }
        
        /* Update the idle buffer with S/N message with assigned ONU-ID Set Tx ONU-ID */
        gpon_error = gpon_tx_get_idle_ploam_content (& sn_ploam_content);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Driver error %d!", gpon_error);
        }
        
        sn_ploam_content.onu_id = gs_gpon_database.onu_parameters.onu_id;
        sn_ploam_content.message_id = CE_US_NO_MESSAGE_MESSAGE_ID;
        
        /* Set idle buffer with new assigned ONU-iD */
        gpon_error = gpon_tx_set_idle_ploam_content(&sn_ploam_content);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Driver error %d!", gpon_error);
        }
        
        /* Set Rx ONU-ID */
        drv_error = ag_drv_gpon_rx_ploam_onu_id_set(0, 
            gs_gpon_database.onu_parameters.onu_id, BDMF_FALSE);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to set Rx ONU-ID due to driver error %d.", drv_error);
            return;
        }
        
        /* Get preassigned equalization delay from the database */
        upstream_overhead_ploam.preassigned_equalization_delay[0] = 
            gs_gpon_database.onu_parameters.preassigned_equalization_delay >> 8;
        upstream_overhead_ploam.preassigned_equalization_delay[1] = 
            gs_gpon_database.onu_parameters.preassigned_equalization_delay & 0x000000ff;
        
        /* Set preassigned equalization delay without random delay */
        f_set_preassigned_ted(& upstream_overhead_ploam , BDMF_FALSE);
        
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send ONU ID indication: error %d !", gpon_error);
        }
        
        /* Set Rx Alloc Id (ONU-ID) */
        gpon_error = gpon_rx_set_ranging_alloc_id_configuration(
            gs_gpon_database.onu_parameters.onu_id, BDMF_TRUE);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to set Rx ranging alloc id table and its valid bit due to Driver error %d!!", 
                gpon_error);
        }
        
        /* Set Ranging valid bit */
        gpon_error = gpon_set_ranging_valid_bit(BDMF_TRUE);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to Set and Tx ranging valid bit due to driver error %d !", gpon_error);
            return;
        }
        
        /* Change internal state to RANGING (O4) */
        xi_operation_state_machine->activation_state = OPERATION_STATE_O4;
        
        p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
            "Link State Transition: SERIAL_NUMBER_O3->RANGING_O4");
        
        /* Report event "Link state transition" with state parameter */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
            OPERATION_STATE_O3, OPERATION_STATE_O4);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send user indication: error %d !", gpon_error);
            return;
        }
        
        /* Associate onu id to its T-cont for omci flow after reactivation */
        assign_alloc_id = xi_operation_sm_params->ploam_message_ptr->message.assign_onu_id.assigned_onu_id;
        
        /* Search for this alloc id in the alloc table */
        for (table_index = 0; table_index < MAX_NUMBER_OF_TCONTS; table_index++)
        {
            if (gs_gpon_database.tconts_table[table_index].alloc_id == assign_alloc_id)
            {
                alloc_id_found = BDMF_TRUE;
                alloc_index = table_index;
                break;
            }
        }
        
        if (alloc_id_found == BDMF_TRUE)
        {
            if (alloc_index != GPON_OMCI_ALLOC_ID_INDEX)
            {
                gs_gpon_database.tconts_table[GPON_OMCI_ALLOC_ID_INDEX].tcont_id = 
                    gs_gpon_database.tconts_table[alloc_index].tcont_id;
                gs_gpon_database.tconts_table[GPON_OMCI_ALLOC_ID_INDEX].alloc_id =
                    gs_gpon_database.tconts_table[alloc_index].alloc_id;
                gs_gpon_database.tconts_table[GPON_OMCI_ALLOC_ID_INDEX].assign_alloc_valid_flag = BDMF_TRUE;
                gs_gpon_database.tconts_table[GPON_OMCI_ALLOC_ID_INDEX].tcont_queue_size = 
                    gs_gpon_database.tconts_table[alloc_index].tcont_queue_size;
                gs_gpon_database.tconts_table[GPON_OMCI_ALLOC_ID_INDEX].config_tcont_valid_flag =
                    gs_gpon_database.tconts_table[alloc_index].config_tcont_valid_flag;
        
                gs_gpon_database.tconts_table[alloc_index].tcont_id = CS_DEFAULT_TCONT_ID;
                gs_gpon_database.tconts_table[alloc_index].alloc_id = PON_ALLOC_ID_DTE_NOT_ASSIGNED;
                gs_gpon_database.tconts_table[alloc_index].tcont_queue_size = 0;
                gs_gpon_database.tconts_table[alloc_index].assign_alloc_valid_flag = BDMF_FALSE;
                gs_gpon_database.tconts_table[alloc_index].config_tcont_valid_flag = BDMF_FALSE;
            }
        
            gpon_error = gpon_rx_set_alloc_id_to_tcont_association(GPON_OMCI_ALLOC_ID_INDEX,
                gs_gpon_database.tconts_table[GPON_OMCI_ALLOC_ID_INDEX].tcont_id);
            if (gpon_error != PON_NO_ERROR)
            {
               p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                   "Unable to set tcont id %d to alloc index %d.", 
                   gs_gpon_database.tconts_table[GPON_OMCI_ALLOC_ID_INDEX].tcont_id,
                   GPON_OMCI_ALLOC_ID_INDEX);
               return;
            }
        }
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.operating.general_id, 
            "Serial number mismatch!");
    }
}

static void p_sm_serial_number_o3_state_timer_to1_expire_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    bdmf_error_t bdmf_error;

    if (gs_gpon_database.link_parameters.auto_restart_to1_mode == GPON_TO1_AUTO_RESTART_ON)
    {
        if (gs_gpon_database.link_parameters.received_serial_number_request == BDMF_TRUE)
        {
            /* Start TO1 timer */
            gs_gpon_database.link_parameters.received_serial_number_request = BDMF_FALSE;
            bdmf_error = bdmf_timer_start(&gs_gpon_database.os_resources.to1_timer_id, 
                GPON_MILISEC_TO_TICKS(gs_gpon_database.onu_parameters.to1_timer_timeout));
            if (bdmf_error != BDMF_ERR_OK)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to start TO1 timer");
            } 
            else
            {
                p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start TO1 timer");
            }
            return;
        }
    }

    /* Clear and disable RX - ranging alloc-id */
    gpon_error = gpon_rx_set_ranging_alloc_id_configuration(GPON_DEF_ALLOC_ID,
        BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear and ranging RX Alloc-ID due to driver error %d !", gpon_error);
        return;
    }

    /* Clear Tx Ranging valid bit */
    gpon_error = gpon_set_ranging_valid_bit(BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear and TX ranging valid bit due to driver error %d !", gpon_error);
        return;
    }

    /* Clear PLS bit */
    bdmf_error = gpon_tx_set_pls_status(BDMF_FALSE);
    if (bdmf_error < 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Tx PLS status due to driver error %d", bdmf_error);
    }

    /* Send user indication "Ranging Stop"  - reason [timeout] */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION,
        PON_INDICATION_RANGING_STOP, PON_RANGING_STOP_REASON_TIMEOUT_EXPIRED);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send oam indication: error %d !", gpon_error);
    }

    /* Change internal state to Standby (O2) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O2;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link State Transition: SERIAL_NUMBER_O3->STANDBY_O2");

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
        OPERATION_STATE_O3, OPERATION_STATE_O2);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
        return;
    }
}

static void p_sm_serial_number_o3_state_detect_los_or_lof_event( 
    GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS * xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    int drv_error;

    /* Stop TO1 timer*/
    bdmf_timer_stop(& gs_gpon_database.os_resources.to1_timer_id);
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop timer TO1");

    /* Clear and disable RX - ranging alloc-id */
    gpon_error = gpon_rx_set_ranging_alloc_id_configuration(GPON_DEF_ALLOC_ID,
        BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear and ranging RX Alloc-ID due to driver error %d !", gpon_error);
        return;
    }

    /* Clear PLS bit */
    drv_error = gpon_tx_set_pls_status(BDMF_FALSE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Tx PLS status due to driver error %d", drv_error);
        return;
    }

    /* Clear Tx Ranging valid bit */
    gpon_error = gpon_set_ranging_valid_bit(BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear and TX ranging valid bit due to driver error %d !", gpon_error);
        return;
    }

    /* Clear Broadcast Id and valid bit */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(1, 
        gs_gpon_database.onu_parameters.broadcast_default_value, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx Broadcast ONU-ID due to driver error %d.", drv_error);
       return;
    }

    /* Report event "Link sub state transition" with state parameter [Stop] */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_STATE_TRANSITION,
        xi_operation_state_machine->link_sub_state, LINK_SUB_STATE_STOP);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
        return;
    }

    /* Send Alarm indication for appropriate LCDG/LOF/LOS indication(via PON task ) */
    if (xi_operation_sm_params->lof_state == BDMF_TRUE)
    {
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM,
            PON_INDICATION_LOF, CE_OAM_ALARM_STATUS_ON);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d !", gpon_error);
            return;
        }
    }

    /* Change internal state to Initial (O1) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O1;

    /* Change link sub-state to Stop */
    xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_STOP;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link State Transition: SERIAL_NUMBER_O3->INIT_O1");

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
        OPERATION_STATE_O3, OPERATION_STATE_O1);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
        return;
    }
}

static void p_sm_serial_number_o3_state_disable_serial_number_message_with_disable_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    PON_SERIAL_NUMBER_DTE serial_number;
    int drv_error;

    /* Get the serial number */
    memcpy((char *)&serial_number, 
        xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.serial_number, 
        sizeof(PON_SERIAL_NUMBER_DTE ));

    /* Disabled unicast & Serial number match?  or Disabled multicast*/
    if (xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.control == CE_SERIAL_NUMBER_UNICAST_DISABLE &&
       f_serial_number_match(serial_number, gs_gpon_database.physical_parameters.serial_number) == BDMF_TRUE)
    {
        /* Stop TO1 timer*/
        bdmf_timer_stop(& gs_gpon_database.os_resources.to1_timer_id);
            p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop timer TO1");

        /* Clear and disable RX - ranging alloc-id */
        gpon_error = gpon_rx_set_ranging_alloc_id_configuration (GPON_DEF_ALLOC_ID, BDMF_FALSE);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to clear and ranging RX Alloc-ID due to driver error %d !", gpon_error);
            return;
        }

        gpon_error = gpon_set_ranging_valid_bit(BDMF_FALSE);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to clear and TX ranging valid bit due to driver error %d !", gpon_error);
            return;
        }

        /* Clear PLS bit */
        drv_error = gpon_tx_set_pls_status(BDMF_FALSE);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to configure Tx PLS status due to driver error %d", drv_error);
            return;
        }

        /* Send user indication "DIS" - On */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM, 
            PON_INDICATION_DIS, BDMF_TRUE);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d !", gpon_error);
        }

        /* Report event "Link sub state transition" with state parameter [Stop] */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_STATE_TRANSITION,
            xi_operation_state_machine->link_sub_state, LINK_SUB_STATE_STOP);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send user indication: error %d !", gpon_error);
            return;
        }

        /* Send user indication "Ranging Stop" - reason [disable_sn] */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION, 
            PON_INDICATION_RANGING_STOP, PON_RANGING_STOP_REASON_DISABLE_SN);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d !", gpon_error);
        }

        /* Change internal state to EMERGENCY_STOP (O7) */
        xi_operation_state_machine->activation_state = OPERATION_STATE_O7;

        /* Change link sub-state to STOP */
        xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_STOP;

        p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
            "Link State Transition: SERIAL_NUMBER_O3->EMERGENCY_STOP_O7");

        /* Report event "Link state transition" with state parameter */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
            OPERATION_STATE_O3, OPERATION_STATE_O7);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send user indication: error %d !", gpon_error);
            return;
        }
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.operating.general_id, "Serial number mismatch!");
    }
}


static void p_sm_serial_number_o3_state_extended_burst_length_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    uint32_t preamble_length_bits;
    GPON_DS_US_OVERHEAD_PLOAM upstream_overhead_ploam;
    uint32_t number_of_type_3 = 0;

    /* Store "pre-range" type 3 preamble */
    gs_gpon_database.onu_parameters.pre_range_type_3_number = 
        xi_operation_sm_params->ploam_message_ptr->message.extended_burst_length.pre_range_type_3_number;

    /* Store "range" type 3 preamble */
    gs_gpon_database.onu_parameters.range_type_3_number = 
        xi_operation_sm_params->ploam_message_ptr->message.extended_burst_length.range_type_3_number;

    /* Set the Extended burst message as valid */
    gs_gpon_database.onu_parameters.extended_burst_length_valid_flag = BDMF_TRUE;

    /* Set Upstream Overhead params according to "pre-range" pattern */
    /* Set Type 3 preamble pattern according the possability of receiving Extended burst message */
    /* Setting Upstream Overhead ploam to config the HW */
    upstream_overhead_ploam.number_of_guard_bits = gs_gpon_database.onu_parameters.number_of_guard_bits;
    upstream_overhead_ploam.number_of_type_1_preamble_bits = 
        gs_gpon_database.onu_parameters.number_of_type_1_preamble_bits;
    upstream_overhead_ploam.number_of_type_2_preamble_bits = 
        gs_gpon_database.onu_parameters.number_of_type_2_preamble_bits;
    upstream_overhead_ploam.pattern_of_type_3_preamble_bits = 
        gs_gpon_database.onu_parameters.pattern_of_type_3_preamble_bits;
    memcpy(upstream_overhead_ploam.delimiter, 
        gs_gpon_database.onu_parameters.delimiter, GPON_DELIMITER_SIZE);
    upstream_overhead_ploam.options.pre_equalization_status =
        gs_gpon_database.onu_parameters.pre_equalization_status == CE_PRE_EQUALIZATION_DELAY_DONT_USE ? BDMF_FALSE : BDMF_TRUE;
    upstream_overhead_ploam.options.serial_number_mask_status = 
        gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_status;
    upstream_overhead_ploam.options.extra_serial_number_transmissions = 
        gs_gpon_database.onu_parameters.extra_serial_number_transmissions;
    upstream_overhead_ploam.options.power_level_mode = 
        gs_gpon_database.onu_parameters.default_power_level_mode;

    /* Get preassigned equalization delay from the database */
    upstream_overhead_ploam.preassigned_equalization_delay[0] = 
        (gs_gpon_database.onu_parameters.preassigned_equalization_delay >> 8) & 0x000000FF;
    upstream_overhead_ploam.preassigned_equalization_delay[1] = 
        gs_gpon_database.onu_parameters.preassigned_equalization_delay & 0x000000FF;

    /* Check that T1 + T2 + GT <= 16 bytes and modulo test - else error */
    preamble_length_bits = upstream_overhead_ploam.number_of_type_1_preamble_bits +
        upstream_overhead_ploam.number_of_type_2_preamble_bits + 
        upstream_overhead_ploam.number_of_guard_bits;

    if (preamble_length_bits > (PON_TX_PREAMBLE_OVERHEAD_SIZE_IN_BYTES << 3 /* Multiple by 8 */) ||
        (preamble_length_bits % 8) != 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Stack Error: The sum of T1 & T2 & GT overflow preamble length OR failed in,"
                "modulo 8 criteria : T1 %d [bits], T2 %d [bits], GT %d [bits]",
                upstream_overhead_ploam.number_of_type_1_preamble_bits,
                upstream_overhead_ploam.number_of_type_2_preamble_bits,
                upstream_overhead_ploam.number_of_guard_bits);
    }

    /* In PMD cold acquisition use longest possible transmission during O3 state */
    if (gs_gpon_database.OpticsType != BP_GPON_OPTICS_TYPE_PMD)
        number_of_type_3 = gs_gpon_database.onu_parameters.pre_range_type_3_number;
    else
        number_of_type_3 = PMD_NUMBER_OF_TYPE_3_PREAMBLE;

    /* HW - Set Pon upstream overhead - preamble & delimiter */
    gpon_error = gpon_tx_set_pon_overhead_and_length(upstream_overhead_ploam,
        gs_gpon_database.onu_parameters.extended_burst_length_valid_flag,
        gs_gpon_database.onu_parameters.pre_range_type_3_number);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Driver error %d !", gpon_error);
    }

    /* Keep pre-range parameters */
    gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.number_of_guard_bits =                                    
        gs_gpon_database.onu_parameters.number_of_guard_bits;
    gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.number_of_type_1_preamble_bits =                          
        gs_gpon_database.onu_parameters.number_of_type_1_preamble_bits;
    gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.number_of_type_2_preamble_bits =                          
        gs_gpon_database.onu_parameters.number_of_type_2_preamble_bits;
    gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.pattern_of_type_3_preamble_bits =                         
        gs_gpon_database.onu_parameters.pattern_of_type_3_preamble_bits;
    gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.pre_equalization_status  =                         
        gs_gpon_database.onu_parameters.pre_equalization_status ;
    gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.serial_number_mask_parameters.serial_number_mask_status = 
        gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_status;
    gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.extra_serial_number_transmissions =                       
        gs_gpon_database.onu_parameters.extra_serial_number_transmissions;
    gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.default_power_level_mode =                                
        gs_gpon_database.onu_parameters.default_power_level_mode;
    gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.delimiter[0] =                                         
        gs_gpon_database.onu_parameters.delimiter[0];
    gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.delimiter[1] =                                         
        gs_gpon_database.onu_parameters.delimiter[1];
    gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.delimiter[2] =                                         
        gs_gpon_database.onu_parameters.delimiter[2];
    gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.extended_burst_length_valid_flag =                        
        gs_gpon_database.onu_parameters.extended_burst_length_valid_flag;
    gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.number_of_type_3 =                                        
        gs_gpon_database.onu_parameters.pre_range_type_3_number;

}

static void p_sm_serial_number_o3_state_serial_number_mask_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, 
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;

    if (gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_status == BDMF_TRUE)
    {
        p_log(ge_onu_logger.sections.stack.operating.general_id, 
            "Received serial number mask message!");

        if (f_serial_number_mask_match(
            xi_operation_sm_params->ploam_message_ptr->message.serial_number_mask.serial_number, 
            xi_operation_sm_params->ploam_message_ptr->message.serial_number_mask.number_of_valid_bits))
        {
            gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_matched = BDMF_TRUE;
            p_log(ge_onu_logger.sections.stack.operating.general_id, 
                "Serial number match!");
            gpon_error = gpon_rx_set_ranging_alloc_id_configuration(
                GPON_RX_RANGING_ALLOC_ID_DEF, BDMF_TRUE);
            if (gpon_error != PON_NO_ERROR)
            {
               p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                   "Unable to set Rx ranging alloc id table and its valid bit due to Driver error %d!!", 
                   gpon_error);
            }
        }
        else
        {
            p_log(ge_onu_logger.sections.stack.operating.general_id, 
                "Serial number not matched!");
        }
    }
    else
    {
        /* invalid cross */
        p_log(ge_onu_logger.sections.stack.operating.invoke_state_machine_id, 
            "Activation state machine:Invalid cross");
    }

}

/* State O4 */
static void p_sm_ranging_o4_state_ranging_request_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    /* HW - Wait Minimum response time plus Preassigned delay */
    /* HW - Responding with S/N response message */

    gs_gpon_database.link_parameters.received_serial_number_request = BDMF_TRUE;

    p_log(ge_onu_logger.sections.stack.operating.general_id, 
        "Received ranging request message!");

}

static void p_sm_ranging_o4_state_ranging_time_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, 
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    uint32_t alloc_id_index;
    uint8_t delay[4];
    GPON_DS_US_OVERHEAD_PLOAM upstream_overhead_ploam;
    GPON_TX_PLOAM_MESSAGE idle_ploam_content;
    gpon_tx_gen_ctrl_ten tx_params; 
    bdmf_error_t bdmf_error;
    PON_ERROR_DTE gpon_error;
    GPON_RXPON_PM_COUNTERS_DTE rxpon_pm_counters;
    uint16_t illegal_access_counter;          
    uint32_t idle_ploam_counter;
    uint32_t normal_ploam_counter;
    uint16_t requested_ranging_ploam_number;  
    uint16_t requested_urgent_ploam_number;  
    uint32_t preamble_length_bits;
    uint8_t sc_sc_flag = 0;
    int drv_error;

    /* Keep Tranmission Configuration */
    bdmf_error = ag_drv_gpon_tx_gen_ctrl_ten_get(&tx_params);
    if (bdmf_error < 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to get Tx configuration due to driver error %d", bdmf_error);
        return;
    }

    /* Clear PLS bit */
    drv_error = gpon_tx_set_pls_status(BDMF_FALSE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Tx PLS status due to driver error %d", drv_error);
    }

    /* Reconfigure Tranmission */
    bdmf_error = ag_drv_gpon_tx_gen_ctrl_ten_set(&tx_params);
    if (bdmf_error < 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set Tx configuration due to driver error %d", bdmf_error);
        return;
    }

    /* Clear Rx Alloc Id enable bit (ONU-ID) */
    gpon_error = gpon_rx_set_ranging_alloc_id_configuration(GPON_DEF_ALLOC_ID,
        BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR)
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
           "Unable to clear Rx ranging alloc id enable bit due to Driver error %d!!", 
           gpon_error);
    }

    /* Enable OMCI channel */
    gs_gpon_database.tconts_table[GPON_OMCI_ALLOC_ID_INDEX].alloc_id = 
        gs_gpon_database.onu_parameters.onu_id;
    gs_gpon_database.tconts_table[GPON_OMCI_ALLOC_ID_INDEX].assign_alloc_valid_flag = BDMF_TRUE;


    /* Send indication to Pon task according assigned alloc - id just arrived */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION,
        PON_INDICATION_ASSIGN_ONU_ID, gs_gpon_database.onu_parameters.onu_id);

    gpon_error = gpon_rx_set_alloc_id_cfg(GPON_OMCI_ALLOC_ID_INDEX,
        gs_gpon_database.onu_parameters.onu_id, BDMF_TRUE);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to Set OMCI Rx alloc id and its valid bit due to Driver error %d alloc_id_index %d!", 
            gpon_error, GPON_OMCI_ALLOC_ID_INDEX);
       return;
    }

    /* Enable HW: Rx allocation table for data according to SW allocation table */
    for (alloc_id_index = NUMBER_OF_FIRST_TCONT_INDEX_FOR_DATA; alloc_id_index <= NUMBER_OF_LAST_TCONT_INDEX_FOR_DATA; alloc_id_index++)
    {
        if (gs_gpon_database.tconts_table[alloc_id_index].alloc_id != PON_ALLOC_ID_DTE_NOT_ASSIGNED)
        {
            gpon_error = gpon_rx_set_alloc_id_cfg(alloc_id_index,
                gs_gpon_database.tconts_table[alloc_id_index].alloc_id,
                gs_gpon_database.tconts_table[alloc_id_index].assign_alloc_valid_flag);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to set Rx alloc id table and its valid bit due to Driver error %d alloc_id_index %d!", 
                    gpon_error, alloc_id_index);
                return;
            }
        }
    }

    /* Set Equalization Delay plus min response time */
    upstream_overhead_ploam.number_of_guard_bits = 
        gs_gpon_database.onu_parameters.number_of_guard_bits;
    upstream_overhead_ploam.number_of_type_1_preamble_bits = 
        gs_gpon_database.onu_parameters.number_of_type_1_preamble_bits;
    upstream_overhead_ploam.number_of_type_2_preamble_bits = 
        gs_gpon_database.onu_parameters.number_of_type_2_preamble_bits;
    upstream_overhead_ploam.pattern_of_type_3_preamble_bits = 
        gs_gpon_database.onu_parameters.pattern_of_type_3_preamble_bits;
    memcpy(upstream_overhead_ploam.delimiter, 
        gs_gpon_database.onu_parameters.delimiter, GPON_DELIMITER_SIZE);
    upstream_overhead_ploam.options.pre_equalization_status =
        gs_gpon_database.onu_parameters.pre_equalization_status == CE_PRE_EQUALIZATION_DELAY_DONT_USE ? BDMF_FALSE : BDMF_TRUE;
    upstream_overhead_ploam.options.serial_number_mask_status = 
        gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_status;
    upstream_overhead_ploam.options.extra_serial_number_transmissions =
        gs_gpon_database.onu_parameters.extra_serial_number_transmissions;
    upstream_overhead_ploam.options.power_level_mode =
        gs_gpon_database.onu_parameters.default_power_level_mode;

    /* Get preassigned equalization delay from the database */
    upstream_overhead_ploam.preassigned_equalization_delay[0] =
        (gs_gpon_database.onu_parameters.preassigned_equalization_delay >> 8) & 0x000000FF;
    upstream_overhead_ploam.preassigned_equalization_delay[1] =
        gs_gpon_database.onu_parameters.preassigned_equalization_delay & 0x000000FF;

    /* Check that T1 + T2 + GT <= 16 bytes and modulo test - else error */
    preamble_length_bits = upstream_overhead_ploam.number_of_type_1_preamble_bits +
        upstream_overhead_ploam.number_of_type_2_preamble_bits + 
        upstream_overhead_ploam.number_of_guard_bits;
    if (preamble_length_bits > (PON_TX_PREAMBLE_OVERHEAD_SIZE_IN_BYTES << 3 /* Multiple by 8 */)
        || (preamble_length_bits % 8) != 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
             "Stack Error: The sum of T1 & T2 & GT overflow preamble length OR failed in,"
             "modulo 8 criteria : T1 %d [bits], T2 %d [bits], GT %d [bits]",
             upstream_overhead_ploam.number_of_type_1_preamble_bits,
             upstream_overhead_ploam.number_of_type_2_preamble_bits,
             upstream_overhead_ploam.number_of_guard_bits);
    }

    delay[0] = xi_operation_sm_params->ploam_message_ptr->message.ranging_time.delay[0];
    delay[1] = xi_operation_sm_params->ploam_message_ptr->message.ranging_time.delay[1];
    delay[2] = xi_operation_sm_params->ploam_message_ptr->message.ranging_time.delay[2];
    delay[3] = xi_operation_sm_params->ploam_message_ptr->message.ranging_time.delay[3];

    /* Set ted */
    p_set_ted(delay);

    /* Stop TO1 timer*/
    bdmf_timer_stop(& gs_gpon_database.os_resources.to1_timer_id);
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop timer TO1");

    /* Clear Tx Ranging valid bit */
    gpon_error = gpon_set_ranging_valid_bit(BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear and TX ranging valid bit due to driver error %d !", gpon_error);
        return;
    }

    /* Gathering the data from the runner per queue */
    gs_callbacks.sc_sc_runner_callback (idle_ploam_content.message_payload, &sc_sc_flag);
    if (idle_ploam_content.message_payload == NULL || sc_sc_flag == 0) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "sc_sc_runner_callback returns NULL");
    }

    idle_ploam_content.onu_id = gs_gpon_database.onu_parameters.onu_id;
    idle_ploam_content.message_id = CE_US_NO_MESSAGE_MESSAGE_ID;

    gpon_error = gpon_tx_set_idle_ploam_content(&idle_ploam_content);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Driver error %d!", gpon_error);
    }

    /* Set Upstream Overhead according to "Range" parameter (if extended message arrived - Ranged valid flag True) */
    /* Set Type 3 preamble pattern according the possability of receiving Extended burst message */
    /* Setting Upstream Overhead ploam to config the HW */
    if (gs_gpon_database.onu_parameters.extended_burst_length_valid_flag == BDMF_TRUE )
    {
        /* Check that T1 + T2 + GT <= 16 bytes and modulo test - else error */
        preamble_length_bits =(upstream_overhead_ploam.number_of_type_1_preamble_bits
                                 +
                                 upstream_overhead_ploam.number_of_type_2_preamble_bits
                                 +
                                 upstream_overhead_ploam.number_of_guard_bits);

        if ((preamble_length_bits > (PON_TX_PREAMBLE_OVERHEAD_SIZE_IN_BYTES << 3 /* Multiple by 8 */))
             ||
            (preamble_length_bits % 8 ) != 0 )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Stack Error: The sum of T1 & T2 & GT overflow preamble length OR failed in ,modulo 8 criteria : T1 %d [bits], T2 %d [bits], GT %d [bits]",
                    upstream_overhead_ploam.number_of_type_1_preamble_bits,
                    upstream_overhead_ploam.number_of_type_2_preamble_bits,
                    upstream_overhead_ploam.number_of_guard_bits);
        }

        /* HW - Set Pon upstream overhead - preamble & delimiter */
        gpon_error = gpon_tx_set_pon_overhead_and_length (upstream_overhead_ploam ,
            gs_gpon_database.onu_parameters.extended_burst_length_valid_flag,
            gs_gpon_database.onu_parameters.range_type_3_number);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d !", gpon_error);
        }
    }

    if (gs_gpon_database.OpticsType == BP_GPON_OPTICS_TYPE_PMD)
    {
        drv_error = ag_drv_gpon_tx_general_configuration_dvstp_set(0xffffff);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to set tranceiver dv setup pattern due to driver error %d.", drv_error);
        }
    }

    bdmf_error = bdmf_timer_start(& gs_gpon_database.os_resources.keep_alive_timer_id, 
        GPON_MILISEC_TO_TICKS( gs_gpon_database.physical_parameters.upstream_ploam_rate));
    if (bdmf_error != BDMF_ERR_OK )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to start Keep alive timer");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, 
            "Keep Alive timer start: %d [msec]", 
            gs_gpon_database.physical_parameters.upstream_ploam_rate);
    }

    /* Start BER interval timer */
    p_log(ge_onu_logger.sections.stack.debug.timers_id, 
        "BER interval timer start: %d [msec]", 
        gs_gpon_database.onu_parameters.ber_interval);
    p_pon_start_ber_interval( gs_gpon_database.onu_parameters.ber_interval);

    /* Clear TX PM counters */
    gpon_error = gpon_get_tx_pm_counters(&illegal_access_counter, &idle_ploam_counter,
        &normal_ploam_counter, &requested_ranging_ploam_number,
        &requested_urgent_ploam_number);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear Tx unit PM counters due to Api error %d !", gpon_error);
        return;
    }

    /* Clear RX PM counters */
    gpon_error = gpon_get_rx_pm_counters(&rxpon_pm_counters);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear Rx unit PM counters due to Api error %d !", gpon_error);
        return;
    }

    /* Send user indication "Ranging Stop"  - reason [ranging_end] */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION ,
        PON_INDICATION_RANGING_STOP, PON_RANGING_STOP_REASON_RANGING_END);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send oam indication: error %d !", gpon_error);
    }

    /* Report event "Link sub state transition" with state parameter [Operational] */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_STATE_TRANSITION ,
        xi_operation_state_machine->link_sub_state, LINK_SUB_STATE_OPERATIONAL);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
        return;
    }

    /* Allow DBA-SR process and start it if needed */
    gs_gpon_database.dbr_parameters.is_dba_sr_allowed = BDMF_TRUE;
    if (gs_gpon_database.dbr_parameters.should_start_dba_sr == BDMF_TRUE )
    {     
        gpon_error = gpon_dba_sr_process_initialize(
            gs_gpon_database.dbr_parameters.dba_sr_reporting_block,
            gs_gpon_database.dbr_parameters.sr_dba_interval);

        if (gpon_error != PON_NO_ERROR)
        {
           p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
               "Unable to start status report");
           return;
        }
        gs_gpon_database.dbr_parameters.should_start_dba_sr = BDMF_FALSE;
    }

    /* Init the AES state machine to standby state */
    gs_gpon_database.link_parameters.aes_state_machine.aes_state = CS_AES_STATE_STANDBY;

    /* Set key switching time boolean flag */
    gs_gpon_database.link_parameters.aes_key_switching_first_time = BDMF_TRUE;

    /* Change internal state to Operation (O5) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O5;

    /* Change link sub state to  Operational */
    xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_OPERATIONAL;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link State Transition: RANGING_O4->OPERATION_O5");

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION ,
        OPERATION_STATE_O4, OPERATION_STATE_O5);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
        return;
    }

#ifdef CONFIG_BCM_GPON_TODD
    /* pass gs_gpon_database.onu_parameters.preassigned_equalization_delay,
       it was updated by p_set_ted */
    tod_handle_ranging_time_ploam(
        gs_gpon_database.onu_parameters.preassigned_equalization_delay);
#endif 
}

static void p_sm_ranging_o4_state_timer_to1_expire_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params )
{
    PON_ERROR_DTE gpon_error;
    GPON_TX_PLOAM_MESSAGE sn_ploam_content;
    bdmf_error_t bdmf_error;

    if (gs_gpon_database.link_parameters.auto_restart_to1_mode == GPON_TO1_AUTO_RESTART_ON)
    {
        if (gs_gpon_database.link_parameters.received_serial_number_request == BDMF_TRUE)
        {
            /* Start TO1 timer */
            gs_gpon_database.link_parameters.received_serial_number_request = BDMF_FALSE;
            bdmf_error = bdmf_timer_start(& gs_gpon_database.os_resources.to1_timer_id, 
                GPON_MILISEC_TO_TICKS(gs_gpon_database.onu_parameters.to1_timer_timeout));
            if (bdmf_error != BDMF_ERR_OK)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to start TO1 timer");
            }
            else
            {
                p_log(ge_onu_logger.sections.stack.debug.timers_id, 
                    "Start TO1 timer");
            }
            return;
        }
    }

    /* Clear PLS bit */
    bdmf_error = gpon_tx_set_pls_status(BDMF_FALSE);
    if (bdmf_error < 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Tx PLS status due to driver error %d", bdmf_error);
    }

    /* Update the Ranging buffer with S/N message with default ONU-ID  */
    gpon_error = gpon_tx_get_sn_ranging_ploam_content (&sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to read ranging ploam content");
    }

    sn_ploam_content.onu_id = GPON_TX_ONU_ID_DEF;

    /* Set SN ploam with new assigned ONU-ID */
    gpon_error = gpon_tx_set_sn_ranging_ploam_content(&sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to set default ONU ID to ranging ploam content");
    }

    /* Clear the idle buffer assigned ONU-ID and set default Tx ONU-ID */
    gpon_error = gpon_tx_get_idle_ploam_content(&sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Failed to read Idle ploam content");
    }

    sn_ploam_content.onu_id = GPON_TX_ONU_ID_DEF;
    sn_ploam_content.message_id = CE_US_NO_MESSAGE_MESSAGE_ID;

    /* Clear idle buffer with new assigned ONU-iD */
    gpon_error = gpon_tx_set_idle_ploam_content (& sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to set default Tx ONU-ID to Idle ploam content");
    }

    /* Clear RX ONU ID */
    bdmf_error = ag_drv_gpon_rx_ploam_onu_id_set(0, GPON_RX_ONU_ID_DEF, BDMF_TRUE);
    if (bdmf_error < 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx ONU-ID due to driver error %d.", bdmf_error);
    }

    /* Clear TX ONU ID */
    bdmf_error = ag_drv_gpon_tx_general_configuration_onuid_set(GPON_TX_ONU_ID_DEF);
    if (bdmf_error < 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Tx ONU-ID due to driver error %d.", bdmf_error);
    }

    /* Clear and disable RX - ranging alloc-id */
    gpon_error = gpon_rx_set_ranging_alloc_id_configuration(GPON_DEF_ALLOC_ID, BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear and ranging RX Alloc-ID due to driver error %d !", gpon_error);
        return;
    }

    /* Clear Tx Ranging valid bit */
    gpon_error = gpon_set_ranging_valid_bit(BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear and TX ranging valid bit due to driver error %d !", gpon_error);
        return;
    }

    /* Send user indication "Ranging Stop"  - reason [timeout] */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION ,
                                                   PON_INDICATION_RANGING_STOP,
                                                   PON_RANGING_STOP_REASON_TIMEOUT_EXPIRED);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send oam indication: error %d !", gpon_error);
    }

    /* Change internal state to Standby (O2) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O2;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, "Link State Transition: RANGING_O4->STANDBY_O2");

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION ,
                                                   OPERATION_STATE_O4,
                                                   OPERATION_STATE_O2);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send user indication: error %d !", gpon_error);
        return;
    }
}

static void p_sm_ranging_o4_state_deactivate_onu_id_message_event( GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine,
                                                                   OPERATION_SM_PARAMS_DTS * xi_operation_sm_params )
{
    PON_ERROR_DTE gpon_error;
    GPON_TX_PLOAM_MESSAGE sn_ploam_content;
    int drv_error;

    /* Stop TO1 timer*/
    bdmf_timer_stop(& gs_gpon_database.os_resources.to1_timer_id);
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop timer TO1");

    /* Clear PLS bit */
    drv_error = gpon_tx_set_pls_status(BDMF_FALSE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Tx PLS status due to driver error %d", drv_error);
    }

    /* Clear and disable RX - ranging alloc-id */
    gpon_error = gpon_rx_set_ranging_alloc_id_configuration (GPON_DEF_ALLOC_ID,
                                                                        BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear and ranging RX Alloc-ID due to driver error %d !", gpon_error);
        return;
    }

    /* Clear Tx Ranging valid bit */
    gpon_error = gpon_set_ranging_valid_bit (BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear and TX ranging valid bit due to driver error %d !", gpon_error);
        return;
    }
    /* Update the Ranging buffer with S/N message with default ONU-ID  */
    gpon_error = gpon_tx_get_sn_ranging_ploam_content (& sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    sn_ploam_content.onu_id = GPON_TX_ONU_ID_DEF;

    /* Set SN ploam with new assigned ONU-iD */
    gpon_error = gpon_tx_set_sn_ranging_ploam_content (& sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    /* Clear the idle buffer assigned ONU-ID and set default Tx ONU-ID */
    gpon_error = gpon_tx_get_idle_ploam_content (& sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    sn_ploam_content.onu_id = GPON_TX_ONU_ID_DEF;
    sn_ploam_content.message_id = CE_US_NO_MESSAGE_MESSAGE_ID;

    /* Clear idle buffer with new assigned ONU-iD */
    gpon_error = gpon_tx_set_idle_ploam_content (& sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    /* Clear RX ONU ID */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(0, GPON_RX_ONU_ID_DEF, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx ONU-ID due to driver error %d.", drv_error);
    }

    /* Clear TX ONU ID */
    drv_error = ag_drv_gpon_tx_general_configuration_onuid_set(GPON_TX_ONU_ID_DEF);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Tx ONU-ID due to driver error %d.", drv_error);
    }

    /* Send user indication "Ranging Stop"  - reason [deactivation] */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION ,
                                                   PON_INDICATION_RANGING_STOP,
                                                   PON_RANGING_STOP_REASON_DEACTIVATE_PON_ID);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send oam indication: error %d !", gpon_error);
    }

    /* Change internal state to Standby (O2) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O2;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, "Link State Transition: RANGING_O4->STANDBY_O2");

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION ,
                                                   OPERATION_STATE_O4,
                                                   OPERATION_STATE_O2);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send user indication: error %d !", gpon_error);
        return;
    }
}

static void p_sm_ranging_o4_state_detect_los_or_lof_event( GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine,
                                                           OPERATION_SM_PARAMS_DTS * xi_operation_sm_params )
{
    PON_ERROR_DTE gpon_error;
    GPON_TX_PLOAM_MESSAGE sn_ploam_content;
    int drv_error;

    /* Stop TO1 timer*/
    bdmf_timer_stop(& gs_gpon_database.os_resources.to1_timer_id);
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop timer TO1");

    /* Clear PLS bit */
    drv_error = gpon_tx_set_pls_status(BDMF_FALSE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Tx PLS status due to driver error %d", drv_error);
    }

    /* Clear and disable RX - ranging alloc-id */
    gpon_error = gpon_rx_set_ranging_alloc_id_configuration (GPON_DEF_ALLOC_ID,
                                                                        BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear and ranging RX Alloc-ID due to driver error %d !", gpon_error);
        return;
    }

    /* Clear Tx Ranging valid bit */
    gpon_error = gpon_set_ranging_valid_bit (BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear and TX ranging valid bit due to driver error %d !", gpon_error);
        return;
    }

    /* Clear Broadcast Id and valid bit */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(1, gs_gpon_database.onu_parameters.broadcast_default_value, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx Broadcast ONU-ID due to driver error %d.", drv_error);
    }

    /* Update the Ranging buffer with S/N message with default ONU-ID  */
    gpon_error = gpon_tx_get_sn_ranging_ploam_content (& sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    sn_ploam_content.onu_id = GPON_TX_ONU_ID_DEF;

    /* Set SN ploam with new assigned ONU-iD */
    gpon_error = gpon_tx_set_sn_ranging_ploam_content (& sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    /* Clear the idle buffer assigned ONU-ID and set default Tx ONU-ID */
    gpon_error = gpon_tx_get_idle_ploam_content (& sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    sn_ploam_content.onu_id = GPON_TX_ONU_ID_DEF;
    sn_ploam_content.message_id = CE_US_NO_MESSAGE_MESSAGE_ID;

    /* Clear idle buffer with new assigned ONU-iD */
    gpon_error = gpon_tx_set_idle_ploam_content (& sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    /* Clear RX ONU ID */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(0, GPON_RX_ONU_ID_DEF, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx ONU-ID due to driver error %d.", drv_error);
    }

    /* Clear TX ONU ID */
    drv_error = ag_drv_gpon_tx_general_configuration_onuid_set(GPON_TX_ONU_ID_DEF);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Tx ONU-ID due to driver error %d.", drv_error);
    }

    /* Send user indication "Ranging Stop"  - reason [alarm_on] */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION ,
                                                   PON_INDICATION_RANGING_STOP,
                                                   PON_RANGING_STOP_REASON_ALARM_ON);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send oam indication: error %d !", gpon_error);
    }

    /* Send Alarm indication for appropriate LCDG/LOF/LOS indication(via PON task ) */
    if (xi_operation_sm_params->lof_state == BDMF_TRUE )
    {
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM ,
                                                       PON_INDICATION_LOF ,
                                                       CE_OAM_ALARM_STATUS_ON);
        if (gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send oam indication: error %d !", gpon_error);
        }
    }

    /* Change internal state to Initial (O1) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O1;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, "Link State Transition: RANGING_O4->INIT_O1");

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION ,
                                                   OPERATION_STATE_O4,
                                                   OPERATION_STATE_O1);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send user indication: error %d !", gpon_error);
        return;
    }
}

static void p_sm_ranging_o4_state_disable_serial_number_message_with_disable_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params )
{
    PON_ERROR_DTE gpon_error;
    PON_SERIAL_NUMBER_DTE serial_number;
    GPON_TX_PLOAM_MESSAGE sn_ploam_content;
    int drv_error;

    /* Get the serial number */
    memcpy((char *)&serial_number, 
        xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.serial_number, 
        sizeof(PON_SERIAL_NUMBER_DTE));

    /* Disabled unicast & Serial number match?  or Disabled multicast*/
    if ((xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.control == CE_SERIAL_NUMBER_UNICAST_DISABLE)
        && (( f_serial_number_match(serial_number, gs_gpon_database.physical_parameters.serial_number) == BDMF_TRUE)))
    {
        /* Stop TO1 timer*/
        bdmf_timer_stop(& gs_gpon_database.os_resources.to1_timer_id);
            p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop timer TO1");

        /* Clear and disable RX - ranging alloc-id */
        gpon_error = gpon_rx_set_ranging_alloc_id_configuration (GPON_DEF_ALLOC_ID,BDMF_FALSE);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to clear and ranging RX Alloc-ID due to driver error %d", 
                gpon_error);
            return;
        }

        /* Clear Tx Ranging valid bit */
        gpon_error = gpon_set_ranging_valid_bit(BDMF_FALSE);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to clear and TX ranging valid bit due to driver error %d", 
                gpon_error);
        }

        /* Clear PLS bit */
        drv_error = gpon_tx_set_pls_status(BDMF_FALSE);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to configure Tx PLS status due to driver error %d", drv_error);
        }

        /* Update the Ranging buffer with S/N message with default ONU-ID  */
        gpon_error = gpon_tx_get_sn_ranging_ploam_content(&sn_ploam_content);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Driver error %d!", gpon_error);
        }

        sn_ploam_content.onu_id = GPON_TX_ONU_ID_DEF;

        /* Set SN ploam with new assigned ONU-iD */
        gpon_error = gpon_tx_set_sn_ranging_ploam_content(&sn_ploam_content);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Driver error %d!", gpon_error);
        }

        /* Clear the idle buffer assigned ONU-ID and set default Tx ONU-ID */
        gpon_error = gpon_tx_get_idle_ploam_content(&sn_ploam_content);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Driver error %d!", gpon_error);
        }

        sn_ploam_content.onu_id = GPON_TX_ONU_ID_DEF;
        sn_ploam_content.message_id = CE_US_NO_MESSAGE_MESSAGE_ID;

        /* Clear idle buffer with new assigned ONU-ID */
        gpon_error = gpon_tx_set_idle_ploam_content(&sn_ploam_content);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Driver error %d!", gpon_error);
        }

        /* Clear RX ONU ID */
        drv_error = ag_drv_gpon_rx_ploam_onu_id_set(0, GPON_RX_ONU_ID_DEF, BDMF_TRUE);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to disable Rx ONU-ID due to driver error %d.", drv_error);
        }

        /* Clear TX ONU ID */
        drv_error = ag_drv_gpon_tx_general_configuration_onuid_set(GPON_TX_ONU_ID_DEF);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to disable Tx ONU-ID due to driver error %d.", drv_error);
        }

        /* Send user indication "Ranging Stop"  - reason [disable_sn] */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION,
            PON_INDICATION_RANGING_STOP, PON_RANGING_STOP_REASON_DISABLE_SN);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d !", gpon_error);
        }

        /* Send user indication "DIS" - On */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM, 
            PON_INDICATION_DIS, BDMF_TRUE);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d !", gpon_error);
        }

        /* Report event "Link sub state transition" with state parameter [Stop] */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_STATE_TRANSITION,
            xi_operation_state_machine->link_sub_state, LINK_SUB_STATE_STOP);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send user indication: error %d", gpon_error);
        }

        /* Change internal state to EMERGENCY_STOP (O7) */
        xi_operation_state_machine->activation_state = OPERATION_STATE_O7;

        /* Change link sub-state to STOP */
        xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_STOP;
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.operating.general_id, 
            "Serial number mismatch!");
    }

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link State Transition: RANGING_O4->EMERGENCY_STOP_O7");

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION, 
        OPERATION_STATE_O4, OPERATION_STATE_O7);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
        return;
    }
}

static void p_sm_ranging_o4_state_change_power_level_message_event( GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine,
                                                                    OPERATION_SM_PARAMS_DTS * xi_operation_sm_params )
{
    PON_ERROR_DTE gpon_error;
    GPON_POWER_LEVEL_PARAMETERS_DTE power_level_param;

    /* Change Power-Level - Prepare to increase / decrease power level by 3db on next SERIAL_NUMBER_REQUEST with PLSu=ENABLED */
    p_log(ge_onu_logger.sections.stack.downstream_ploam.cpl_message_id, "Prepare to [ increase / decrease / no_action ] power level by 3db on next SERIAL_NUMBER_REQUEST with PLSu=ENABLED");

    /* Set default ONU transmit power level */
    if (xi_operation_sm_params->ploam_message_ptr->message.change_power_level.indication ==  CE_CHANGE_POWER_LEVEL_INCREASE )
    {
        switch(gs_gpon_database.onu_parameters.default_power_level_mode )
        {
        case GPON_POWER_LEVEL_LOW :
            gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_NORMAL;
            break;
        case GPON_POWER_LEVEL_NORMAL :
            gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_HIGH;
            break;
        case GPON_POWER_LEVEL_HIGH :
            gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_LOW;
            break;
        default:
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "power level is illegal ");
            break;

        }
    }
    else
    {

        if (xi_operation_sm_params->ploam_message_ptr->message.change_power_level.indication == CE_CHANGE_POWER_LEVEL_DECREASE )
        {
            switch(gs_gpon_database.onu_parameters.default_power_level_mode )
            {
            case GPON_POWER_LEVEL_LOW :
                gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_HIGH;
                break;
            case GPON_POWER_LEVEL_NORMAL :
                gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_LOW;
                break;
            case GPON_POWER_LEVEL_HIGH :
                gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_NORMAL;
                break;
            default:
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "power level is illegal ");
                break;

            }
        }
    }
    power_level_param.power_level_mode = gs_gpon_database.onu_parameters.default_power_level_mode;
    power_level_param.reason = GPON_CHANGE_POWER_LEVEL_REASON_CHANGE_POWER_LEVEL_PLOAM;
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_CHANGE_POWER_LEVEL , power_level_param);

    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to change power level: error %d !", gpon_error);
    }


 }

/* State O5 */
static void p_sm_operation_o5_state_ranging_request_message_event( GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine,
                                                                   OPERATION_SM_PARAMS_DTS * xi_operation_sm_params )
{
    /* Respond with Ploam */
}

static void p_sm_operation_o5_state_change_power_level_message_event( GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine,
                                                                      OPERATION_SM_PARAMS_DTS * xi_operation_sm_params )
{
    PON_ERROR_DTE gpon_error;
    GPON_POWER_LEVEL_PARAMETERS_DTE power_level_param;

    /* Change Power-Level - Prepare to increase / decrease power level by 3db on next SERIAL_NUMBER_REQUEST with PLSu=ENABLED */
    p_log(ge_onu_logger.sections.stack.downstream_ploam.cpl_message_id, "Prepare to [ increase / decrease / no_action ] power level by 3db on next SERIAL_NUMBER_REQUEST with PLSu=ENABLED");

     /* Set default ONU transmit power level */
    if (xi_operation_sm_params->ploam_message_ptr->message.change_power_level.indication ==  CE_CHANGE_POWER_LEVEL_INCREASE )
         {
             switch(gs_gpon_database.onu_parameters.default_power_level_mode )
             {
             case GPON_POWER_LEVEL_LOW :
                 gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_NORMAL;
                 break;
             case GPON_POWER_LEVEL_NORMAL :
                 gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_HIGH;
                 break;
             case GPON_POWER_LEVEL_HIGH :
                 gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_LOW;
                 break;
             default:
                 p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "power level is illegal ");
                 break;

             }
         }

    else
    {

        if (xi_operation_sm_params->ploam_message_ptr->message.change_power_level.indication == CE_CHANGE_POWER_LEVEL_DECREASE )
        {
            switch(gs_gpon_database.onu_parameters.default_power_level_mode )
            {
            case GPON_POWER_LEVEL_LOW :
                gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_HIGH;
                break;
            case GPON_POWER_LEVEL_NORMAL :
                gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_LOW;
                break;
            case GPON_POWER_LEVEL_HIGH :
                gs_gpon_database.onu_parameters.default_power_level_mode = GPON_POWER_LEVEL_NORMAL;
                break;
            default:
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "power level is illegal ");
                break;

            }
        }
    }
    power_level_param.power_level_mode = gs_gpon_database.onu_parameters.default_power_level_mode;
    power_level_param.reason = GPON_CHANGE_POWER_LEVEL_REASON_CHANGE_POWER_LEVEL_PLOAM;
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_CHANGE_POWER_LEVEL , power_level_param);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to change power level: error %d !", gpon_error);
    }


}

static void p_sm_operation_o5_state_ranging_time_message_event( GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine,
                                                                OPERATION_SM_PARAMS_DTS * xi_operation_sm_params )
{
    uint8_t delay[4];
	BL_PWM_ERR_DTS err_pwm;
#ifdef CONFIG_BCM_TIME_SYNC_PON_UNSTABLE
    PON_ERROR_DTE err;
#endif 

     delay[0] = xi_operation_sm_params->ploam_message_ptr->message.ranging_time.delay[0];
     delay[1] = xi_operation_sm_params->ploam_message_ptr->message.ranging_time.delay[1];
     delay[2] = xi_operation_sm_params->ploam_message_ptr->message.ranging_time.delay[2];
     delay[3] = xi_operation_sm_params->ploam_message_ptr->message.ranging_time.delay[3];

     /* Set EqD */
     p_set_ted(delay);

#ifdef CONFIG_BCM_GPON_TODD
    /* pass gs_gpon_database.onu_parameters.preassigned_equalization_delay, it was updated by p_set_ted */
    tod_handle_ranging_time_ploam(
        gs_gpon_database.onu_parameters.preassigned_equalization_delay);
#endif 

#ifdef CONFIG_BCM_TIME_SYNC_PON_UNSTABLE
    /* Time Synchronization: PON Unstable - Set signal On */
    err = time_sync_pon_unstable_set(1);
    if (err != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Time Synchronizatioin; PON Unstable; Failed to set signal On: %u", err);
    }
#endif 

    /* Power Management: Activate - Must be located the LAST within the procedure */
    err_pwm = api_pwm_activate(); 
    if (err_pwm != CS_BL_PWM_ERR_OK )
    {    
        if (err_pwm == CS_BL_PWM_ERR_SERVICE_DISABLED )
            p_log( ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, "Power Management service is disabled");
        else
            p_log( ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, "Failed to activate Power Management; Error: %lu", err_pwm);
        return;
    }
}

static void p_sm_operation_o5_state_deactivate_onu_id_message_event( GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine,
                                                                     OPERATION_SM_PARAMS_DTS * xi_operation_sm_params )
{
    PON_ERROR_DTE gpon_error;
    uint32_t  alloc_id_index;
    GPON_TX_PLOAM_MESSAGE sn_ploam_content;
	BL_PWM_ERR_DTS err_pwm;
    int drv_error;

    /* Disable and clear Alloc-Id in the 0th entry of the Alloc id table - disable OMCI channel */
    gpon_error = gpon_rx_set_alloc_id_cfg (GPON_OMCI_ALLOC_ID_INDEX,
                                                                GPON_DEF_ALLOC_ID,
                                                                BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
       /* Log */
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear OMCI Rx alloc id and its valid bit due to Driver error %d alloc_id_index %d!", gpon_error, GPON_OMCI_ALLOC_ID_INDEX );
       return;
    }

    /* Disable HW:Rx allocation table for data according to SW allocation table */
    for (alloc_id_index = NUMBER_OF_FIRST_TCONT_INDEX_FOR_DATA;
        alloc_id_index <= NUMBER_OF_LAST_TCONT_INDEX_FOR_DATA; alloc_id_index++)
    {
        if (gs_gpon_database.tconts_table[alloc_id_index].alloc_id != PON_ALLOC_ID_DTE_NOT_ASSIGNED)
        {
            gpon_error = gpon_rx_set_alloc_id_cfg(alloc_id_index, 
                gs_gpon_database.tconts_table[alloc_id_index].alloc_id, BDMF_FALSE);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to clear Rx alloc id table and its valid bit due to Driver error %d alloc_id_index %d!", 
                    gpon_error, alloc_id_index);
                return;
            }
        }
    }

    /* Clear the SW allocation table - clear only the alloc row */
    gpon_error = f_clear_sw_allocation_table();
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear allocation table due to error:%d!", gpon_error);
        return;
    }


    /* Stop the DBA-SR process if it is running and don not allow starting it, */
    /* till the ONU returns to O5                                              */
    if (gs_gpon_database.dbr_parameters.dbr_status == BDMF_TRUE )
    {
        gpon_error = gpon_dba_sr_process_terminate ();

        if (gpon_error != PON_NO_ERROR )
        {
           /* Log */
           p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to stop status report");
           return;
        }
        gs_gpon_database.dbr_parameters.should_start_dba_sr = BDMF_TRUE;
    }
    gs_gpon_database.dbr_parameters.is_dba_sr_allowed = BDMF_FALSE;

    /* Update the Ranging buffer with S/N message with default ONU-ID  */
    gpon_error = gpon_tx_get_sn_ranging_ploam_content (& sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    sn_ploam_content.onu_id = GPON_TX_ONU_ID_DEF;

    /* Set SN ploam with new assigned ONU-ID */
    gpon_error = gpon_tx_set_sn_ranging_ploam_content (& sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    /* Clear the idle buffer assigned ONU-ID and set default Tx ONU-ID */
    gpon_error = gpon_tx_get_idle_ploam_content (& sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    sn_ploam_content.onu_id = GPON_TX_ONU_ID_DEF;
    sn_ploam_content.message_id = CE_US_NO_MESSAGE_MESSAGE_ID;

    /* Clear idle buffer with new assigned ONU-iD */
    gpon_error = gpon_tx_set_idle_ploam_content (& sn_ploam_content);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
    }

    /* Clear RX ONU ID */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(0, GPON_RX_ONU_ID_DEF, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx ONU-ID due to driver error %d.", drv_error);
    }

    /* Clear TX ONU ID */
    drv_error = ag_drv_gpon_tx_general_configuration_onuid_set(GPON_TX_ONU_ID_DEF);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Tx ONU-ID due to driver error %d.", drv_error);
    }

    /* Stop BER Interval timer */
    gpon_error =  f_pon_stop_ber_interval ();
    if (gpon_error != PON_NO_ERROR )
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to stop BER interval %d !", gpon_error);
    }

    /* Stop Keep Alive timer */
    bdmf_timer_stop(& gs_gpon_database.os_resources.keep_alive_timer_id);

    /* Flush normal and urgent ploam fifo */
    /* flush tx normal queue and urgent queue of PLOAM messages.                              */
    /* it may occure that PLOAM messages are waiting on the queues for TX buffer to be freed. */
    if (f_gpon_txpon_flush_queue() != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Fail to flush queue");
    }

    /* Clear Tx PLOAM valid bit */
    ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_TRUE);
    ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_FALSE);

    /* Send user indication "DACT" - On */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM ,
                                                   PON_INDICATION_DACT,
                                                   BDMF_TRUE);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send oam indication: error %d !", gpon_error);
    }

    /* Report event "Link sub state transition" with state parameter [Standby] */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_STATE_TRANSITION ,
                                                   xi_operation_state_machine->link_sub_state,
                                                   LINK_SUB_STATE_STANDBY);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send user indication: error %d !", gpon_error);
    }

    /* Change internal state to Standby (O2) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O2;

    /* Change link sub-state to Standby */
    xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_STANDBY;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, "Link State Transition: OPERATION_O5->STANDBY_O2");

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION ,
                                                   OPERATION_STATE_O5,
                                                   OPERATION_STATE_O2);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send user indication: error %d !", gpon_error);
        return;
    }

#ifdef CONFIG_BCM_TIME_SYNC_PON_UNSTABLE
    /* Time Synchronization: PON Unstable - Set signal Off */
    gpon_error = time_sync_pon_unstable_set(0);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Time Synchronizatioin; PON Unstable; Failed to set signal Off: %u", gpon_error);
    }
#endif 

    /* Power Management: Deactivate - Must be located the LAST within the procedure */
    err_pwm = api_pwm_deactivate(); //__PwM
    if (err_pwm != CS_BL_PWM_ERR_OK )
    {
        if (err_pwm == CS_BL_PWM_ERR_SERVICE_DISABLED )
            p_log( ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, "Power Management service is disabled");
        else
            p_log( ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, "Failed to deactivate Power Management; Error: %lu", err_pwm);
        return;
    }
}

static void p_sm_operation_o5_state_detect_los_or_lof_event( GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine,
                                                             OPERATION_SM_PARAMS_DTS * xi_operation_sm_params )
{
    PON_ERROR_DTE gpon_error;
    uint32_t alloc_id_index;
    bdmf_error_t bdmf_error;
	BL_PWM_ERR_DTS err_pwm;

    /* Stop BER Interval timer */
    gpon_error =  f_pon_stop_ber_interval ();
    if (gpon_error != PON_NO_ERROR )
    {
       /* Log */
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to stop BER interval %d !", gpon_error);

    }

    /* Stop Keep Alive timer */
    bdmf_timer_stop(& gs_gpon_database.os_resources.keep_alive_timer_id);

    /* Stop the DBA-SR process if it is running and don not allow starting it, */
    /* till the ONU returns to O5                                              */
    if (gs_gpon_database.dbr_parameters.dbr_status == BDMF_TRUE )
    {     
        gpon_error = gpon_dba_sr_process_terminate ();

        if (gpon_error != PON_NO_ERROR )
        {
           /* Log */
           p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to stop status report");
           return;
        }
        gs_gpon_database.dbr_parameters.should_start_dba_sr = BDMF_TRUE;
    }
    gs_gpon_database.dbr_parameters.is_dba_sr_allowed = BDMF_FALSE;

    /* Disable and clear Alloc-Id in the 0th entry of the Alloc id table - disable OMCI channel */
    gpon_error = gpon_rx_set_alloc_id_cfg (GPON_OMCI_ALLOC_ID_INDEX,
                                                                GPON_DEF_ALLOC_ID,
                                                                BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
       /* Log */
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear OMCI Rx alloc id and its valid bit due to Driver error %d alloc_id_index %d!", gpon_error, GPON_OMCI_ALLOC_ID_INDEX );
       return;
    }

    /* Disable HW:Rx allocation table for data according to SW allocation table */
    for (alloc_id_index = NUMBER_OF_FIRST_TCONT_INDEX_FOR_DATA; 
        alloc_id_index <= NUMBER_OF_LAST_TCONT_INDEX_FOR_DATA; alloc_id_index++)
    { 
        if (gs_gpon_database.tconts_table[alloc_id_index].alloc_id != PON_ALLOC_ID_DTE_NOT_ASSIGNED)
        {
            gpon_error = gpon_rx_set_alloc_id_cfg(alloc_id_index, 
                gs_gpon_database.tconts_table[alloc_id_index].alloc_id, BDMF_FALSE);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to clear Rx alloc id table and its valid bit due to Driver error %d alloc_id_index %d!", 
                    gpon_error, alloc_id_index);
                return;
            }
        }
    }

    /* Flush normal ploam fifo */
    /* flush tx normal queue and urgent queue of PLOAM messages.                              */
    /* it may occure that PLOAM messages are waiting on the queues for TX buffer to be freed. */
    if (f_gpon_txpon_flush_queue() != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Fail to flush queue");
    }

    /* Clear Tx PLOAM valid bit */
    gpon_tx_set_tx_ploam_bit(BDMF_TRUE);
    gpon_tx_set_tx_ploam_bit(BDMF_FALSE);

    /* Start TO2 timer */
    bdmf_error = bdmf_timer_start(& gs_gpon_database.os_resources.to2_timer_id, 
        GPON_MILISEC_TO_TICKS(gs_gpon_database.onu_parameters.to2_timer_timeout));
    if (bdmf_error != BDMF_ERR_OK )
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Unable to start TO2 timer");
    }
    else
    {
       p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start TO2 timer");
    }

    /* Send Alarm indication for appropriate LCDG/LOF/LOS indication(via PON task ) */
    if (xi_operation_sm_params->lof_state == BDMF_TRUE )
    {
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM ,
            PON_INDICATION_OPERATIONAL_LOF, CE_OAM_ALARM_STATUS_ON);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send perational lof indication: error %d !", gpon_error);
        }
    }

    /* Report event "Link sub state transition" with state parameter [Standby] */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_STATE_TRANSITION ,
        xi_operation_state_machine->link_sub_state, LINK_SUB_STATE_STANDBY);
    if (gpon_error != PON_NO_ERROR)
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Unable to send user indication: error %d !", gpon_error);
    }

    /* Change link sub-state to Standby */
    xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_STANDBY;

    /* Change internal state to POP_UP (O6) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O6;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link State Transition: OPERATION_O5->POPUP_O6");

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION ,
        OPERATION_STATE_O5, OPERATION_STATE_O6);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
        return;
    }

#ifdef CONFIG_BCM_TIME_SYNC_PON_UNSTABLE
    /* Time Synchronization: PON Unstable - Set signal Off */
    gpon_error = time_sync_pon_unstable_set(0);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log( ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Time Synchronization; PON Unstable; Failed to set signal Off: %u", gpon_error);
    }
#endif 
    
    /* Power Management: Deactivate - Must be located the LAST within the procedure */
    err_pwm = api_pwm_deactivate(); 
    if (err_pwm != CS_BL_PWM_ERR_OK )
    {
        if (err_pwm == CS_BL_PWM_ERR_SERVICE_DISABLED )
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, 
                "Power Management service is disabled");
        }
        else
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, 
                "Failed to deactivate Power Management; Error: %lu", err_pwm);
        }
        return;
     }
#if defined(CONFIG_BCM96848)
    wan_reset_rx_and_tx();
#endif

}

static void p_sm_operation_o5_state_disable_serial_number_message_with_disable_event( GPON_OPERATION_STATE_MACHINE_DTE * xi_operation_state_machine,
                                                                                      OPERATION_SM_PARAMS_DTS * xi_operation_sm_params )
{
    uint32_t alloc_id_index;
    PON_ERROR_DTE gpon_error;
    PON_SERIAL_NUMBER_DTE serial_number;
    GPON_TX_PLOAM_MESSAGE sn_ploam_content;
    bdmf_error_t bdmf_error;
	BL_PWM_ERR_DTS err_pwm;

    /* Get the serial number */
    memcpy(( char * ) & serial_number, xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.serial_number, sizeof(PON_SERIAL_NUMBER_DTE ));

    if (( xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.control == CE_SERIAL_NUMBER_UNICAST_DISABLE )
         &&
        (( f_serial_number_match(serial_number, gs_gpon_database.physical_parameters.serial_number ) == BDMF_TRUE ) ) )
    {
        /* Disable and clear Alloc-Id in the 0th entry of the Alloc id table - disable OMCI channel */
        gpon_error = gpon_rx_set_alloc_id_cfg (GPON_OMCI_ALLOC_ID_INDEX,
                                                                    GPON_DEF_ALLOC_ID,
                                                                    BDMF_FALSE);
        if (gpon_error != PON_NO_ERROR )
        {
           /* Log */
           p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear OMCI Rx alloc id and its valid bit due to Driver error %d alloc_id_index %d!", gpon_error, GPON_OMCI_ALLOC_ID_INDEX );
           return;
        }

        /* Disable HW:Rx allocation table for data according to SW allocation table */
        for (alloc_id_index = NUMBER_OF_FIRST_TCONT_INDEX_FOR_DATA;
            alloc_id_index <= NUMBER_OF_LAST_TCONT_INDEX_FOR_DATA; alloc_id_index++)
        {
            if (gs_gpon_database.tconts_table[alloc_id_index].alloc_id != PON_ALLOC_ID_DTE_NOT_ASSIGNED)
            {
                gpon_error = gpon_rx_set_alloc_id_cfg(alloc_id_index, 
                    gs_gpon_database.tconts_table[alloc_id_index].alloc_id, BDMF_FALSE);
                if (gpon_error != PON_NO_ERROR)
                {
                   p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                       "Unable to clear Rx alloc id table and its valid bit due to Driver error %d alloc_id_index %d!", 
                       gpon_error, alloc_id_index);
                   return;
                }
            }
        }

        /* Clear the SW allocation table - clear only the alloc row */
        gpon_error = f_clear_sw_allocation_table();
        if (gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear allocation table due to error:%d!", gpon_error);
            return;
        }

        /* Stop BER Interval timer */
        gpon_error =  f_pon_stop_ber_interval ();
        if (gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to stop BER interval %d !", gpon_error);

        }

        /* Stop Keep Alive timer */
        bdmf_timer_stop(& gs_gpon_database.os_resources.keep_alive_timer_id);

        /* Flush normal and urgent ploam fifo */
        /* flush tx normal queue and urgent queue of PLOAM messages.                              */
        /* it may occure that PLOAM messages are waiting on the queues for TX buffer to be freed. */
        if (f_gpon_txpon_flush_queue() != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Fail to flush queue");
        }

        /* Clear Tx PLOAM valid bit */
        gpon_tx_set_tx_ploam_bit (BDMF_TRUE);
        gpon_tx_set_tx_ploam_bit (BDMF_FALSE);

        /* Update the Ranging buffer with S/N message with default ONU-ID  */
        gpon_error = gpon_tx_get_sn_ranging_ploam_content (& sn_ploam_content);
        if (gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
        }

        sn_ploam_content.onu_id = GPON_TX_ONU_ID_DEF;

        /* Set SN ploam with new assigned ONU-iD */
        gpon_error = gpon_tx_set_sn_ranging_ploam_content (& sn_ploam_content);
        if (gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
        }

        /* Clear the idle buffer assigned ONU-ID and set default Tx ONU-ID */
        gpon_error = gpon_tx_get_idle_ploam_content (& sn_ploam_content);
        if (gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
        }

        sn_ploam_content.onu_id = GPON_TX_ONU_ID_DEF;
        sn_ploam_content.message_id = CE_US_NO_MESSAGE_MESSAGE_ID;

        /* Clear idle buffer with new assigned ONU-iD */
        gpon_error = gpon_tx_set_idle_ploam_content (& sn_ploam_content);
        if (gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d!", gpon_error);
        }

        /* Clear RX ONU ID */
        bdmf_error = ag_drv_gpon_rx_ploam_onu_id_set(0, GPON_RX_ONU_ID_DEF, BDMF_TRUE);
        if (bdmf_error < 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to disable Rx ONU-ID due to driver error %d.", bdmf_error);
        }

        /* Clear TX ONU ID */
        bdmf_error = ag_drv_gpon_tx_general_configuration_onuid_set(GPON_TX_ONU_ID_DEF);
        if (bdmf_error < 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to disable Tx ONU-ID due to driver error %d.", bdmf_error);
        }

        /* Send user indication "DIS" - On */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM ,
                                                       PON_INDICATION_DIS,
                                                       BDMF_TRUE);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send oam indication: error %d !", gpon_error);
        }

        /* Report event "Link sub state transition" with state parameter [Stop] */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_STATE_TRANSITION ,
                                                       xi_operation_state_machine->link_sub_state,
                                                       LINK_SUB_STATE_STOP);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send user indication: error %d !", gpon_error);
        }

        /* Change internal state to EMERGENCY_STOP (O7) */
        xi_operation_state_machine->activation_state = OPERATION_STATE_O7;

        /* Change link sub-state to Stop */
        xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_STOP;

        p_log(ge_onu_logger.sections.stack.operating.state_change_id, "Link State Transition: OPERATION_O5->EMERGENCY_STOP_O7");

        /* Report event "Link state transition" with state parameter */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION ,
                                                       OPERATION_STATE_O5,
                                                       OPERATION_STATE_O7);
        if (gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send user indication: error %d !", gpon_error);
            return;
        }

#ifdef CONFIG_BCM_TIME_SYNC_PON_UNSTABLE
    /* Time Synchronization: PON Unstable - Set signal Off */
    gpon_error = time_sync_pon_unstable_set(0);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Time Synchronizatioin; PON Unstable; Failed to set signal Off: %u", 
            gpon_error);
        return;
    }
#endif 

        /* Power Management: Deactivate - Must be located the LAST within the procedure */
        err_pwm = api_pwm_deactivate(); 
        if (err_pwm != CS_BL_PWM_ERR_OK )
        {
            if (err_pwm == CS_BL_PWM_ERR_SERVICE_DISABLED )
                p_log( ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, "Power Management service is disabled");
            else
                p_log( ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, "Failed to deactivate Power Management; Error: %lu", err_pwm);
            return;
        }
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.operating.general_id, "Serial number mismatch!");
    }
}

static void p_sm_operation_o5_state_encrypt_port_id_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    PON_CONFIGURE_FILTER_INDICATION configure_port;
    GPON_DS_PLOAM ds_ploam;

    /* Return Ack */
    memcpy(&ds_ploam, xi_operation_sm_params->ploam_message_ptr, sizeof(GPON_DS_PLOAM));
    ds_ploam.message.encrypted_port_id_or_vpi.port_id_16 = 
        htons(ds_ploam.message.encrypted_port_id_or_vpi.port_id_16);
    ds_ploam.message.encrypted_port_id_or_vpi.vpi_16 = 
        htons(ds_ploam.message.encrypted_port_id_or_vpi.vpi_16);
    p_send_ack(&ds_ploam);

    configure_port.configure_flag = 0;
    configure_port.encryption_mode = 
        xi_operation_sm_params->ploam_message_ptr->message.encrypted_port_id_or_vpi.options.encrypted;
    configure_port.flow_id = 0;
    configure_port.flow_priority = 0;
    configure_port.flow_type = 
        xi_operation_sm_params->ploam_message_ptr->message.encrypted_port_id_or_vpi.options.type;
    configure_port.port_id = 
        xi_operation_sm_params->ploam_message_ptr->message.encrypted_port_id_or_vpi.port_id.port_id;
    configure_port.port_mask_id = 0;

    p_log(ge_onu_logger.sections.stack.downstream_ploam.encrypted_vpi_port_message_id, 
        "Encrypt port id %d, encryption option: [%s]",
        configure_port.port_id,
        configure_port.encryption_mode == BDMF_FALSE ? "disable" : "enable");

    /* Send indication to Pon task according assigned alloc - id just arrived */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION,
        PON_INDICATION_MODIFY_ENCRYPTION_PORT_ID_FILTER, configure_port);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send oam indication: error %d !", gpon_error);
    }
}

static void p_sm_operation_o5_state_assign_alloc_id_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    PON_ASSIGN_ALLOC_ID_INDICATION assign_alloc_id;
    GPON_DS_PLOAM ds_ploam;

    /* Return Ack */
    memcpy(&ds_ploam, xi_operation_sm_params->ploam_message_ptr, sizeof(GPON_DS_PLOAM));
    ds_ploam.message.assign_alloc_id.alloc_16= 
        htons(ds_ploam.message.assign_alloc_id.alloc_16);
    p_send_ack(&ds_ploam);

    /* Build the assign alloc id struct */
    assign_alloc_id.alloc_id =
        ((PON_ALLOC_ID_DTE)(xi_operation_sm_params->ploam_message_ptr->message.assign_alloc_id.alloc_id.alloc_id));
    assign_alloc_id.alloc_id_type = xi_operation_sm_params->ploam_message_ptr->message.assign_alloc_id.alloc_id_type;
    assign_alloc_id.assign_flag = assign_alloc_id.alloc_id_type == CE_PAYLOAD_TYPE_DEALLOC ? BDMF_FALSE : BDMF_TRUE;

    p_log(ge_onu_logger.sections.stack.downstream_ploam.assign_alloc_id_message_id,
        "Assign alloc-id message arrived: assign alloc id %d , assign alloc-id type %d",
        assign_alloc_id.alloc_id, assign_alloc_id.alloc_id_type);

    /* Update HW and internal data staructure of Alloc-Id */
    set_alloc_id_configuration(assign_alloc_id);

    /* Send indication to Pon task according assigned alloc - id just arrived */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION,
        PON_INDICATION_ASSIGN_ALLOC_ID, assign_alloc_id);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send oam indication: error %d !", gpon_error);
    }
}

static void p_sm_operation_o5_state_configure_port_id_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    PON_CONFIGURE_OMCI_PORT_ID_INDICATION configure_omci_port_id;
    GPON_DS_PLOAM ds_ploam;

    /* Return Ack */
    memcpy(&ds_ploam, xi_operation_sm_params->ploam_message_ptr, sizeof(GPON_DS_PLOAM));
    ds_ploam.message.configure_port_id.port_id_16 = 
        htons(ds_ploam.message.configure_port_id.port_id_16);
    p_send_ack(&ds_ploam);

    /* Build the port-id struct */
    configure_omci_port_id.port_control = 
        xi_operation_sm_params->ploam_message_ptr->message.configure_port_id.options.activate == BDMF_TRUE ? BDMF_TRUE : BDMF_FALSE;
    configure_omci_port_id.port_id = 
        xi_operation_sm_params->ploam_message_ptr->message.configure_port_id.port_id.port_id;

    p_log(ge_onu_logger.sections.stack.downstream_ploam.configure_port_id_message_id, 
        "Configure OMCI port %d control %d", configure_omci_port_id.port_id, configure_omci_port_id.port_control);

    /* Send indication to Pon task according assigned alloc - id just arrived */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION,
        PON_INDICATION_CONFIGURE_OMCI_PORT_ID_FILTER, configure_omci_port_id);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send oam indication: error %d !", gpon_error);
    }
}

static void p_sm_operation_o5_state_request_aes_key_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    p_aes_state_machine(CS_AES_EVENT_REQUEST_KEY_MESSAGE_RECEIVED, 
        xi_operation_sm_params->ploam_message_ptr);
}

static void p_sm_operation_o5_state_key_switching_time_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    GPON_DS_PLOAM ploam_message;

    /* Return Ack */
    if (xi_operation_sm_params->ploam_message_ptr->onu_id == 
        gs_gpon_database.onu_parameters.broadcast_default_value)
    {
        memcpy(&ploam_message, xi_operation_sm_params->ploam_message_ptr, 
            sizeof(GPON_DS_PLOAM));
        ploam_message.onu_id = gs_gpon_database.onu_parameters.onu_id;
        p_send_ack(& ploam_message);
    }
    else
    {
        p_send_ack(xi_operation_sm_params->ploam_message_ptr);
    }

    p_aes_state_machine(CS_AES_EVENT_SWITCH_TIME_MESSAGE_RECEIVED, 
        xi_operation_sm_params->ploam_message_ptr);
}

static void p_sm_operation_o5_state_physical_equipment_error_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;

    p_log(ge_onu_logger.sections.stack.downstream_ploam.pee_message_id, 
        "Physical equipment error message!");

    /* Send user indication "PEE" */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM,
        PON_INDICATION_PEE_OLT, BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send oam indication: error %d !", gpon_error);
    }
}

static void p_sm_operation_o5_state_vendor_specific_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params )
{
    p_log(ge_onu_logger.sections.stack.debug.general_id, "vendor specific message !");
}

static void p_sm_operation_o5_state_ber_interval_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    uint32_t ber_interval_in_frames = 0;
    GPON_DS_PLOAM ploam_message;

    /* Return Ack */
    if (xi_operation_sm_params->ploam_message_ptr->onu_id == gs_gpon_database.onu_parameters.broadcast_default_value)
    {
        memcpy(&ploam_message, xi_operation_sm_params->ploam_message_ptr, sizeof(GPON_DS_PLOAM));
        ploam_message.onu_id = gs_gpon_database.onu_parameters.onu_id;
        p_send_ack(& ploam_message);
    }
    else
    {
        p_send_ack(xi_operation_sm_params->ploam_message_ptr);
    }

    /* Get the BER interval from the DS message */
    ber_interval_in_frames |= ((uint32_t)xi_operation_sm_params->ploam_message_ptr->message.ber_interval.interval[0]) << 24;
    ber_interval_in_frames |= ((uint32_t)xi_operation_sm_params->ploam_message_ptr->message.ber_interval.interval[1]) << 16;
    ber_interval_in_frames |= ((uint32_t)xi_operation_sm_params->ploam_message_ptr->message.ber_interval.interval[2]) << 8;
    ber_interval_in_frames |= ((uint32_t)xi_operation_sm_params->ploam_message_ptr->message.ber_interval.interval[3]);

    p_log(ge_onu_logger.sections.stack.downstream_ploam.ber_interval_message_id, 
        "BER interval message arrived , BER interval value %d [DS Frames]!", ber_interval_in_frames);

    /* Send user indication "BER Interval" */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_CTRL_BER_TIMER,
        MS_DS_FRAMES_TO_MILISEC(ber_interval_in_frames), BDMF_TRUE);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send BER interval indication: error %d !", gpon_error);
    }
}

static void p_sm_operation_o5_state_request_password_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    uint32_t i;
    PON_ERROR_DTE gpon_error;
    GPON_US_PLOAM ploam_message;

    ploam_message.onu_id = gs_gpon_database.onu_parameters.onu_id;
    ploam_message.message_id = CE_US_PASSWORD_MESSAGE_ID;

    for (i = 0; i < CE_PLOAM_MESSAGE_PAYLOAD_FIELD_SIZE; i++)
    {
        ploam_message.message.password.password[i] = gs_gpon_database.physical_parameters.password.password[i];
    }

    /* Send password via ploam buffer - 3 time */
    gpon_error = f_gpon_txpon_send_ploam_message(&ploam_message,
        NORMAL_PLOAM_CELL, GPON_PASSWORD_TRANSMISSION_NUMBER);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Error: unable to send password!");
        return;
    }
}

static void p_sm_operation_o5_state_serial_number_request_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    p_log(ge_onu_logger.sections.stack.operating.general_id, 
        "Serial number request message event arrived !");
}

static void p_sm_operation_o5_state_pst_message( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;

    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_PST_PLOAM);
     if (gpon_error != PON_NO_ERROR)
     {
         p_log(ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, 
             "Unable to send oam indication: error %d !", gpon_error);
     }
}


static void p_sm_operation_o5_state_sleep_allow_message(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    BL_PWM_ERR_DTS err_pwm;
    
    /* Sleep_Allow(On) */
    if (xi_operation_sm_params->ploam_message_ptr->message.sleep_allow.sleep_allow == CE_SLEEP_ALLOW_ON) 
         err_pwm = api_pwm_handle_sa_on();
    /* Sleep_Allow(Off) */
    else  
        err_pwm = api_pwm_handle_sa_off();
    
    if (err_pwm != CS_BL_PWM_ERR_OK)
    {
        if (err_pwm == CS_BL_PWM_ERR_SERVICE_DISABLED)
        {
            p_log( ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, 
                "Power Management service is disabled");
        }
        else
        {
            p_log( ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, 
                "Failed to handle Sleep Allow message; Error: %lu", err_pwm);
        }
        return;
    }
}

static void p_sm_operation_o5_state_ranging_adjustment_message_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, 
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    uint32_t new_eqd, delta;
    uint8_t delay[4];

    /* Calculate New EqD = Last EqD + Delay Delta */
    delta = (((uint32_t)xi_operation_sm_params->ploam_message_ptr->message.ranging_adjustment.eqd_delay[0])<<24 |
        ((uint32_t)xi_operation_sm_params->ploam_message_ptr->message.ranging_adjustment.eqd_delay[1])<<16 | 
        ((uint32_t)xi_operation_sm_params->ploam_message_ptr->message.ranging_adjustment.eqd_delay[2])<<8 |
        ((uint32_t)xi_operation_sm_params->ploam_message_ptr->message.ranging_adjustment.eqd_delay[3]));

    /* Read Equalization delay from DB, increase/decrease the current EqD by the specific value read from received Ranging Adjustment PLOAM. */
    if (xi_operation_sm_params->ploam_message_ptr->message.ranging_adjustment.decrease_flag == DS_RANGING_ADJUSTMENT_PLOAM_INCREASE_EQD) 
        new_eqd = gs_gpon_database.onu_parameters.delay + delta;
    else
        new_eqd = gs_gpon_database.onu_parameters.delay - delta;

    delay[0] = (uint8_t)((new_eqd>>24) & 0xff);
    delay[1] = (uint8_t)((new_eqd>>16) & 0xff);
    delay[2] = (uint8_t)((new_eqd>>8) & 0xff);
    delay[3] = (uint8_t)(new_eqd & 0xff);

    p_log (ge_onu_logger.sections.stack.downstream_ploam.ranging_adjustment_message_id,
        "Previous EqD: %d. New EqD before adding minimum response time: %d [%02X %02X %02X %02X]", 
        gs_gpon_database.onu_parameters.delay, new_eqd, delay[0], delay[1], delay[2], delay[3]);

    /* Add minimum response time and Set EqD in HW */
    p_set_ted(delay);

#ifdef CONFIG_BCM_GPON_TODD
    /* Configure ToDD after EqD was updated by p_set_ted() */
    tod_handle_ranging_time_ploam(
        gs_gpon_database.onu_parameters.preassigned_equalization_delay);
#endif 
}

static void p_sm_operation_o5_state_pon_id_message_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, 
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    p_log(ge_onu_logger.sections.stack.downstream_ploam.pon_id_message_id, 
        "PON ID message arrived");

    memcpy(&gs_gpon_database.onu_parameters.pon_id_param, 
        &xi_operation_sm_params->ploam_message_ptr->message.pon_id, 
        sizeof(PON_ID_PARAMETERS));  
}

/* State O6 */
static void p_sm_popup_o6_state_downstream_clear_los_or_lof_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params )
{
    PON_ERROR_DTE gpon_error;

    /* Send Alarm indication for appropriate LCDG/LOF/LOS indication(via PON task ) */
    if (xi_operation_sm_params->lof_state == BDMF_FALSE)
    {
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM ,
            PON_INDICATION_LOF, CE_OAM_ALARM_STATUS_OFF);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d !", gpon_error);
        }
    }
}

static void p_sm_popup_o6_state_deactivate_onu_id_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    uint32_t alloc_id_index;

    /* HW responsibility */
    p_log(ge_onu_logger.sections.stack.downstream_ploam.deactivate_onu_id_message_id, 
        "The following actions should be taken by HW:");

    /* Stop TO2 timer */
    bdmf_timer_stop(& gs_gpon_database.os_resources.to2_timer_id);
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop timer TO2");

    /* Disable and clear Alloc-Id in the 0th entry of the Alloc id table - disable OMCI channel */
    gpon_error = gpon_rx_set_alloc_id_cfg (GPON_OMCI_ALLOC_ID_INDEX,
        GPON_DEF_ALLOC_ID, BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR)
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Unable to clear OMCI Rx alloc id and its valid bit due to Driver error %d alloc_id_index %d!", 
           gpon_error, GPON_OMCI_ALLOC_ID_INDEX);
       return;
    }

    /* Disable HW:Rx allocation table for data according to SW allocation table */
    for (alloc_id_index = NUMBER_OF_FIRST_TCONT_INDEX_FOR_DATA;
        alloc_id_index <= NUMBER_OF_LAST_TCONT_INDEX_FOR_DATA; alloc_id_index++)
    {
        if (gs_gpon_database.tconts_table[alloc_id_index].alloc_id != PON_ALLOC_ID_DTE_NOT_ASSIGNED)
        {
            gpon_error = gpon_rx_set_alloc_id_cfg(alloc_id_index,
                gs_gpon_database.tconts_table[alloc_id_index].alloc_id, BDMF_FALSE);
            if (gpon_error != PON_NO_ERROR)
            {
               p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                   "Unable to clear Rx alloc id table and its valid bit due to Driver error %d alloc_id_index %d!", 
                   gpon_error, alloc_id_index);
               return;
            }
        }
    }

    /* Clear the SW allocation table - clear only the alloc row */
    gpon_error = f_clear_sw_allocation_table();
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear allocation table due to error:%d!", gpon_error);
        return;
    }

    /* Change internal state to Standby (O2) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O2;

    /* Change link sub-state to Standby */
    xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_STANDBY;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link State Transition: POPUP_O6->STANDBY_O2");

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
        OPERATION_STATE_O6, OPERATION_STATE_O2);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
        return;
    }
}

static void p_sm_popup_o6_state_broadcast_popup_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    uint32_t preamble_length_bits;
    PON_ERROR_DTE gpon_error;
    bdmf_error_t bdmf_error;
    GPON_DS_US_OVERHEAD_PLOAM upstream_overhead_ploam;

    /* Stop TO2 timer */
    bdmf_timer_stop(& gs_gpon_database.os_resources.to2_timer_id);
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop TO2 timer");

    /* Start TO1 timer */
    gs_gpon_database.link_parameters.received_serial_number_request = BDMF_FALSE;
    bdmf_error = bdmf_timer_start(& gs_gpon_database.os_resources.to1_timer_id, 
        GPON_MILISEC_TO_TICKS(gs_gpon_database.onu_parameters.to1_timer_timeout));
    if (bdmf_error != BDMF_ERR_OK)
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Unable to start TO1 timer");
    }
    else
    {
       p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start TO1 timer");
    }

    /* Get preassigned equalization delay from the database */
    upstream_overhead_ploam.preassigned_equalization_delay[0] = 
        gs_preassigned_equalization_delay >> 8;
    upstream_overhead_ploam.preassigned_equalization_delay[1] = 
        gs_preassigned_equalization_delay & 0x000000ff;

    /*Set preassigned equalization delay without random delay*/
    f_set_preassigned_ted(&upstream_overhead_ploam, BDMF_FALSE);

    /* Set Upstream Overhead params according to "pre-range" pattern */
    /*  Set Type 3 preamble pattern according the possability of receiving Extended burst message  */
    /*  Setting Upstream Overhead ploam to config the HW  */
    upstream_overhead_ploam.number_of_guard_bits = 
        gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.number_of_guard_bits;
    upstream_overhead_ploam.number_of_type_1_preamble_bits = 
        gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.number_of_type_1_preamble_bits;
    upstream_overhead_ploam.number_of_type_2_preamble_bits = 
        gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.number_of_type_2_preamble_bits;
    upstream_overhead_ploam.pattern_of_type_3_preamble_bits = 
        gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.pattern_of_type_3_preamble_bits;
    upstream_overhead_ploam.options.pre_equalization_status =
        gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.pre_equalization_status == 
        CE_PRE_EQUALIZATION_DELAY_DONT_USE ? BDMF_FALSE : BDMF_TRUE;
    upstream_overhead_ploam.options.serial_number_mask_status = 
        gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.serial_number_mask_parameters.serial_number_mask_status;
    upstream_overhead_ploam.options.extra_serial_number_transmissions = 
        gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.extra_serial_number_transmissions;
    upstream_overhead_ploam.options.power_level_mode = 
        gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.default_power_level_mode;
    memcpy(upstream_overhead_ploam.delimiter, 
        gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.delimiter, GPON_DELIMITER_SIZE);

    /* Check that T1 + T2 + GT <= 16 bytes and modulo test - else error */
    preamble_length_bits = upstream_overhead_ploam.number_of_type_1_preamble_bits +
        upstream_overhead_ploam.number_of_type_2_preamble_bits + upstream_overhead_ploam.number_of_guard_bits;

    if (preamble_length_bits > (PON_TX_PREAMBLE_OVERHEAD_SIZE_IN_BYTES << 3 /* Multiple by 8 */) ||
        (preamble_length_bits % 8) != 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Stack Error: The sum of T1 & T2 & GT overflow preamble length OR failed in,"
            "modulo 8 criteria : T1 %d [bits], T2 %d [bits], GT %d [bits]",
            upstream_overhead_ploam.number_of_type_1_preamble_bits,
            upstream_overhead_ploam.number_of_type_2_preamble_bits,
            upstream_overhead_ploam.number_of_guard_bits);
    }

    /* HW - Set Pon upstream overhead - preamble & delimiter */
    if (gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.extended_burst_length_valid_flag == BDMF_TRUE)
    {
        gpon_error = gpon_tx_set_pon_overhead_and_length(upstream_overhead_ploam,
            gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.extended_burst_length_valid_flag,
            gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.number_of_type_3);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d !", gpon_error);
        }
    }
    else
    {
        /*HW - Set Pon upstream overhead - preamble & delimiter */
        gpon_error = gpon_tx_set_pon_overhead_and_length (upstream_overhead_ploam ,
            gs_gpon_database.onu_parameters.pre_ranging_overhead_and_length_parameters.extended_burst_length_valid_flag, 0);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d !", gpon_error);
        }
    }

    /* Set Ranging valid bit */
    gpon_error = gpon_set_ranging_valid_bit(BDMF_TRUE);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to Set and TX ranging valid bit due to driver error %d !", gpon_error);
        return;
    }

    /* Set Rx Alloc Id (ONU-ID)*/
    gpon_error = gpon_rx_set_ranging_alloc_id_configuration(
        gs_gpon_database.onu_parameters.onu_id, BDMF_TRUE);
    if (gpon_error != PON_NO_ERROR)
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
           "Unable to set Rx ranging alloc id table and its valid bit due to Driver error %d!!", 
           gpon_error);
    }

    /* Change internal state to Ranging (O4) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O4;

    /* Change link sub-state to Standby */
    xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_STANDBY;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link State Transition: POPUP_O6->RANGING_O4");

#if defined(CONFIG_BCM96848)
    wan_reset_rx_and_tx();
#endif

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION ,
        OPERATION_STATE_O6, OPERATION_STATE_O4);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d !", gpon_error);
        return;
    }
}

static void p_sm_popup_o6_state_directed_popup_message_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;

    /* Stop TO2 timer */
    bdmf_timer_stop(& gs_gpon_database.os_resources.to2_timer_id);
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop TO2 timer");

    enable_functionalities_when_moving_to_from_operational_state(BDMF_TRUE);

    /* Change internal state to Operation (O5) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O5;

    /* Change link sub-state to Standby */
    xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_OPERATIONAL;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link State Transition: POPUP_O6->OPERATION_O5");

#if defined(CONFIG_BCM96848)
    wan_reset_rx_and_tx();
#endif

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION ,
        OPERATION_STATE_O6, OPERATION_STATE_O5);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d", gpon_error);
        return;
    }
}

static void p_sm_popup_o6_state_timer_to2_expire_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params )
{
    bdmf_boolean lof_state;
    bdmf_boolean fec_state;
    bdmf_boolean lcdg_state;
    uint8_t bit_align;
    bdmf_error_t bdmf_error;
    uint32_t alloc_id_index;
    PON_ERROR_DTE gpon_error;
    int drv_error;

    /* Flush normal and urgent ploam fifo */
    /* flush tx normal queue and urgent queue of PLOAM messages.                              */
    /* it may occure that PLOAM messages are waiting on the queues for TX buffer to be freed. */
    if (f_gpon_txpon_flush_queue() != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Fail to flush queue");
    }

    /* Clear Tx PLOAM valid bit */
    gpon_tx_set_tx_ploam_bit(BDMF_TRUE);
    gpon_tx_set_tx_ploam_bit(BDMF_FALSE);

    /* Clear Broadcast Id and valid bit */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(1, 
        gs_gpon_database.onu_parameters.broadcast_default_value, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx Broadcast ONU-ID due to driver error %d.", drv_error);
    }

    /* Clear TX ONU ID */
    drv_error = ag_drv_gpon_tx_general_configuration_onuid_set(GPON_TX_ONU_ID_DEF);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Tx ONU-ID due to driver error %d.", drv_error);
    }

    /* Disable and clear Alloc-Id in the 0th entry of the Alloc id table - disable OMCI channel */
    gpon_error = gpon_rx_set_alloc_id_cfg(GPON_OMCI_ALLOC_ID_INDEX,
        GPON_DEF_ALLOC_ID, BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear OMCI Rx alloc id and its valid bit due to Driver error %d alloc_id_index %d!", 
            gpon_error, GPON_OMCI_ALLOC_ID_INDEX);
       return;
    }

    /* Disable HW:Rx allocation table for data according to SW allocation table */
    for (alloc_id_index = NUMBER_OF_FIRST_TCONT_INDEX_FOR_DATA;
        alloc_id_index <= NUMBER_OF_LAST_TCONT_INDEX_FOR_DATA; alloc_id_index++)
    {
        if (gs_gpon_database.tconts_table[alloc_id_index].alloc_id != PON_ALLOC_ID_DTE_NOT_ASSIGNED)
        {
            gpon_error = gpon_rx_set_alloc_id_cfg(alloc_id_index,
                gs_gpon_database.tconts_table[alloc_id_index].alloc_id, BDMF_FALSE);
            if (gpon_error != PON_NO_ERROR)
            {
               p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                   "Unable to clear Rx alloc id table and its valid bit due to Driver error %d alloc_id_index %d!", 
                   gpon_error, alloc_id_index);
               return;
            }
        }
    }

    /* Clear the SW allocation table - clear only the alloc row */
    gpon_error = f_clear_sw_allocation_table();
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear allocation table due to error:%d!", gpon_error);
        return;
    }

    /* Accorsing to receiver status cross Operation SM to O1 or to O2 */
    bdmf_error = ag_drv_gpon_rx_general_config_rcvr_status_get(&lof_state, &fec_state,
       &lcdg_state, &bit_align);
    if (bdmf_error < 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to get LCDG & LOF status due to Driver error %d", bdmf_error);
    }

    if (( lcdg_state == BDMF_FALSE )
         &&
        (lof_state == BDMF_FALSE ) )
    {
        /* Set Broadcast ONU-ID to 255 (RX block) */
        drv_error = ag_drv_gpon_rx_ploam_onu_id_set(1, 
            gs_gpon_database.onu_parameters.broadcast_default_value, BDMF_FALSE);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to enable Rx Broadcast ONU-ID due to driver error %d.", drv_error);
        }

        /* Change internal state to Standby (O2) */
        xi_operation_state_machine->activation_state = OPERATION_STATE_O2;

        p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
            "Link State Transition: POPUP_O6->STANDBY_O2");

        /* Report event "Link state transition" with state parameter */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
            OPERATION_STATE_O6, OPERATION_STATE_O2);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send user indication: error %d !", gpon_error);
            return;
        }
    }
    else
    {
        /* Change internal state to Init (O1) */
        xi_operation_state_machine->activation_state = OPERATION_STATE_O1;

        p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
            "Link State Transition: POPUP_O6->INIT_O1");

        /* Report event "Link state transition" with state parameter */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
            OPERATION_STATE_O6, OPERATION_STATE_O1);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send user indication: error %d !", gpon_error);
            return;
        }
    }

    /* Change link sub-state to Standby */
    xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_STANDBY;
}

static void p_sm_popup_o6_state_detect_los_or_lof_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;

    /* Send Alarm indication for appropriate LCDG/LOF/LOS indication(via PON task ) */
    if (xi_operation_sm_params->lof_state == BDMF_TRUE)
    {
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM,
            PON_INDICATION_LOF, CE_OAM_ALARM_STATUS_ON);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d !", gpon_error);
        }
    }
}

static void p_sm_popup_o6_state_disable_serial_number_message_with_disable_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    PON_SERIAL_NUMBER_DTE serial_number;

    /* Get the serial number */
    memcpy((char *)&serial_number, 
        xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.serial_number, 
        sizeof(PON_SERIAL_NUMBER_DTE));

    if (xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.control == CE_SERIAL_NUMBER_UNICAST_DISABLE &&
        f_serial_number_match(serial_number, gs_gpon_database.physical_parameters.serial_number) == BDMF_TRUE)
    {
        /* Stop TO2 timer */
        bdmf_timer_stop(& gs_gpon_database.os_resources.to2_timer_id);
            p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop TO2 timer");
        
        /* Send user indication "DIS" - On */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM,
            PON_INDICATION_DIS, BDMF_TRUE);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d !", gpon_error);
        }
        
        /* Report event "Link sub state transition" with state parameter [Stop] */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_STATE_TRANSITION,
            xi_operation_state_machine->link_sub_state, LINK_SUB_STATE_STOP);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send user indication: error %d !", gpon_error);
        }
        
        /* Clear the SW allocation table - clear only the alloc row */
        gpon_error = f_clear_sw_allocation_table();
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to clear allocation table due to error:%d!", gpon_error);
            return;
        }
        
        /* Change internal state to EMERGENCY_STOP (O7) */
        xi_operation_state_machine->activation_state = OPERATION_STATE_O7;
        
        /* Change link sub state to STOP */
        xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_STOP;
        
        p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
            "Link State Transition: POPUP_O6->EMERGENCY_STOP_O7");
        
        /* Report event "Link state transition" with state parameter */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
            OPERATION_STATE_O6, OPERATION_STATE_O7);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send user indication: error %d !", gpon_error);
            return;
        }
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.operating.general_id, 
            "Serial number mismatch!");
    }
}

static void p_sm_popup_o6_state_swift_popup_message_event(
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine, 
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;

    /* Stop TO2 timer */
    bdmf_timer_stop(&gs_gpon_database.os_resources.to2_timer_id);
    p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop TO2 timer");

    enable_functionalities_when_moving_to_from_operational_state(BDMF_TRUE);

    /* Keep Alive timer start */
    bdmf_timer_start(&gs_gpon_database.os_resources.keep_alive_timer_id, 
        GPON_MILISEC_TO_TICKS(gs_gpon_database.physical_parameters.upstream_ploam_rate));
    p_log(ge_onu_logger.sections.stack.debug.timers_id, "Keep Alive timer start: %d [msec]", 
        gs_gpon_database.physical_parameters.upstream_ploam_rate);
 
    /* BER interval timer start */
    p_pon_start_ber_interval(gs_gpon_database.onu_parameters.ber_interval);

    /* Change internal state to Operation (O5) */
    xi_operation_state_machine->activation_state = OPERATION_STATE_O5;

    /* Change link sub state to  Operational */
    xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_OPERATIONAL;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link State Transition: POPUP_O6->OPERATION_O5");

#if defined(CONFIG_BCM96848)
    wan_reset_rx_and_tx();
#endif

    /* Report event "Link state transition" with state parameter */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION, 
        OPERATION_STATE_O6, OPERATION_STATE_O5);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d", gpon_error);
        return;
    }
}

/* State O7 */
static void p_sm_emergency_stop_o7_state_downstream_clear_los_or_lof_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    int drv_error;

    /* Send Alarm indication for appropriate LCDG/LOF/LOS indication(via PON task ) */
    if (xi_operation_sm_params->lof_state == BDMF_TRUE)
    {
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM ,
            PON_INDICATION_LOF, CE_OAM_ALARM_STATUS_ON);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d !", gpon_error);
            return;
        }
    }

    /* Set Broadcast ONU-ID to default (RX block) and enable the filter (waits foe enable) */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(1, 
        gs_gpon_database.onu_parameters.broadcast_default_value, BDMF_FALSE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx Broadcast ONU-ID due to driver error %d.", drv_error);
        return;
    }
}

static void p_sm_emergency_stop_o7_state_downstream_detect_los_or_lof_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    int drv_error;

    /* Clear Broadcast Id and valid bit */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(1, 
        gs_gpon_database.onu_parameters.broadcast_default_value, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx Broadcast ONU-ID due to driver error %d.", drv_error);
    }

    /* Clear TX ONU ID */
    drv_error = ag_drv_gpon_tx_general_configuration_onuid_set(GPON_TX_ONU_ID_DEF);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Tx ONU-ID due to driver error %d.", drv_error);
    }

    /* Send Alarm indication for appropriate LCDG/LOF/LOS indication(via PON task ) */
    if (xi_operation_sm_params->lof_state == BDMF_TRUE)
    {
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM,
            PON_INDICATION_LOF, CE_OAM_ALARM_STATUS_ON);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d !", gpon_error);
            return;
        }
    }
}


static void p_sm_emergency_stop_o7_state_disable_serial_number_message_with_enable_event( 
    GPON_OPERATION_STATE_MACHINE_DTE *xi_operation_state_machine,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    PON_ERROR_DTE gpon_error;
    PON_SERIAL_NUMBER_DTE serial_number;

    /* Get the serial number */
    memcpy((char *)&serial_number, 
        xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.serial_number, 
        sizeof(PON_SERIAL_NUMBER_DTE));

    /* Unicast Enable ? & Serial number match? OR Multicast enable */
    if ((xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.control == CE_SERIAL_NUMBER_UNICAST_ENABLE &&
        f_serial_number_match(serial_number, gs_gpon_database.physical_parameters.serial_number) == BDMF_TRUE) ||
        (xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number.control == CE_SERIAL_NUMBER_MULTICAST_ENABLE))
    {
        /* Flush normal and urgent ploam fifo flush tx normal queue and urgent queue of PLOAM messages. */
        /* it may occure that PLOAM messages are waiting on the queues for TX buffer to be freed. */
        if (f_gpon_txpon_flush_queue() != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Fail to flush queue");
        }
        
        /* Clear Tx PLOAM valid bit */
        gpon_tx_set_tx_ploam_bit(BDMF_TRUE);
        gpon_tx_set_tx_ploam_bit(BDMF_FALSE);
        
        /* Send user indication "DIS" - Off */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM, 
            PON_INDICATION_DIS, BDMF_FALSE);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send oam indication: error %d !", gpon_error);
        }
        
        /* Report event "Link sub state transition" with state parameter [Standby] */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_STATE_TRANSITION ,
            xi_operation_state_machine->link_sub_state, LINK_SUB_STATE_STANDBY);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send user indication: error %d !", gpon_error);
        }
        
        /* Change internal state to Standby (O2) */
        xi_operation_state_machine->activation_state = OPERATION_STATE_O2;
        
        /* Change link sub state to STOP */
        xi_operation_state_machine->link_sub_state = LINK_SUB_STATE_STANDBY;
        
        p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
            "Link State Transition: EMERGENCY_STOP_O7->STANDBY_O2");
        
        /* Report event "Link state transition" with state parameter */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
            OPERATION_STATE_O7, OPERATION_STATE_O2);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send user indication: error %d !", gpon_error);
            return;
        }
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.operating.general_id, 
            "Serial number mismatch!");
    }

    if (ls_state_was_o2_before == BDMF_FALSE)
    {
        ls_state_was_o2_before = BDMF_TRUE;
    }
}


static void enable_functionalities_when_moving_to_from_operational_state(int enable)
{    
    PON_ERROR_DTE gpon_error;
    uint32_t alloc_id_index;
    BL_PWM_ERR_DTS err_pwm;

    /* Enable HW: Rx allocation table according to SW allocation table */
    for (alloc_id_index = 0; alloc_id_index < ALLOC_ID_INDEX_MAX; alloc_id_index++)
    {
        if (gs_gpon_database.tconts_table[alloc_id_index].alloc_id != PON_ALLOC_ID_DTE_NOT_ASSIGNED)
        {
            gpon_error = gpon_rx_set_alloc_id_cfg(alloc_id_index, 
                gs_gpon_database.tconts_table[alloc_id_index].alloc_id, 
                enable ? gs_gpon_database.tconts_table[alloc_id_index].assign_alloc_valid_flag : BDMF_FALSE);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to %s Rx alloc id table and its valid bit due to driver error %d alloc_id_index %d.", 
                    enable ? "set" : "clear", gpon_error, alloc_id_index);
               return;
            }
        }
    }

    /* Start/stop DBA-SR process */
    gs_gpon_database.dbr_parameters.is_dba_sr_allowed = enable;
    if (enable) 
    {
        if (gs_gpon_database.dbr_parameters.should_start_dba_sr == BDMF_TRUE) 
        {
            gpon_error = gpon_dba_sr_process_initialize(
                gs_gpon_database.dbr_parameters.dba_sr_reporting_block,
                gs_gpon_database.dbr_parameters.sr_dba_interval);
        }
    }
    else if (gs_gpon_database.dbr_parameters.dbr_status == BDMF_TRUE)
        gpon_error = gpon_dba_sr_process_terminate();

    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to %s SR-DBA report", enable ? "start" : "stop");
        return;
    }

    gs_gpon_database.dbr_parameters.should_start_dba_sr = enable ? BDMF_FALSE : BDMF_TRUE;

    /* Set Tx Ranging valid bit */
    gpon_error = gpon_set_ranging_valid_bit(enable ? BDMF_FALSE : BDMF_TRUE);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set TX ranging valid bit due to driver error %d", gpon_error);
        return;
    }

#ifdef CONFIG_BCM_TIME_SYNC_PON_UNSTABLE
    /* Time Synchronization: PON Unstable - Set signal On/Off */
    gpon_error = time_sync_pon_unstable_set(enable);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log( ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Time Synchronizatioin PON Unstable: Failed to set signal %s: %u", 
            enable ? "On" : "Off", gpon_error);
    }
#endif 

    /* Power Management: Activate/Deactivate */
    err_pwm = enable ? api_pwm_activate() : api_pwm_deactivate();
    if (err_pwm != CS_BL_PWM_ERR_OK)
    {
        if (err_pwm == CS_BL_PWM_ERR_SERVICE_DISABLED)
        {
            p_log( ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, 
                "Power Management service is disabled");
        }
        else
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, 
                "Failed to %s Power Management; Error: %lu", 
                enable ? "activate" : "deactivate", err_pwm);
        }
    }
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_send_ack                                                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Send Ack                                                                 */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function constructs ACK ploam message and sends it.                 */
/*                                                                            */
/* Input:                                                                     */
/*   xi_downstream_message_ptr - request message                              */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE p_send_ack(GPON_DS_PLOAM *xi_downstream_message_ptr)
{
    uint32_t i;
    GPON_US_PLOAM ack_message;
    PON_ERROR_DTE gpon_error;

    ack_message.onu_id = xi_downstream_message_ptr->onu_id;
    ack_message.message_id = CE_US_ACKNOWLEDGE_MESSAGE_ID;
    ack_message.message.acknowledge.ds_message_id = xi_downstream_message_ptr->message_id;
    ack_message.message.acknowledge.ds_message[0] = xi_downstream_message_ptr->onu_id;
    ack_message.message.acknowledge.ds_message[1] = xi_downstream_message_ptr->message_id;

    p_log(ge_onu_logger.sections.stack.upstream_ploam.general_id, 
        "Send ACK as urgent PLOAM message, Ack for message id %d", 
        ack_message.message.acknowledge.ds_message_id);

    for (i = 2; i < CE_DS_PLOAM_MESSAGE_ACK_FIELD_SIZE; i++)
    {
        ack_message.message.acknowledge.ds_message[i] = 
            xi_downstream_message_ptr->message.no_message.unspecified[i - 2];
    }

    p_log(ge_onu_logger.sections.stack.upstream_ploam.print_ploam, 
        "p_send_ack DS ploam: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
        xi_downstream_message_ptr->message.no_message.unspecified[0], xi_downstream_message_ptr->message.no_message.unspecified[1],
        xi_downstream_message_ptr->message.no_message.unspecified[2], xi_downstream_message_ptr->message.no_message.unspecified[3],
        xi_downstream_message_ptr->message.no_message.unspecified[4], xi_downstream_message_ptr->message.no_message.unspecified[5],
        xi_downstream_message_ptr->message.no_message.unspecified[6], xi_downstream_message_ptr->message.no_message.unspecified[7],
        xi_downstream_message_ptr->message.no_message.unspecified[8], xi_downstream_message_ptr->message.no_message.unspecified[9]);                 
                                       
    /* Send the acknowledge message to the Tx PLOAM handler, Send the message as an normal PLOAM message */
    gpon_error = f_gpon_txpon_send_ploam_message(&ack_message,
        NORMAL_PLOAM_CELL, GPON_ACK_TRANSMISSION_NUMBER);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Error: unable to send ack message!");
    }

    return gpon_error;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_send_urgent_ploam                                                      */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Send Ploam                                                               */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function send ploam message .                                       */
/*                                                                            */
/* Input:                                                                     */
/*   xi_ploam_message_ptr - request message                                   */
/*   xi_repetition - time to send ploam message                               */
/* Output:                                                                    */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE f_send_urgent_ploam(GPON_US_PLOAM *xi_ploam_message_ptr)
{
    int i;
    int drv_error;
    uint8_t uplmc;
    GPON_TX_ISR txisr;
    bdmf_boolean upvld;
    uint8_t npvld;
 
    for (i = 0; i <= PLOAM_MESSAGE_SIZE; i++) 
    {
        if (i < PLOAM_MESSAGE_SIZE) 
            uplmc = ((uint8_t *)(xi_ploam_message_ptr))[i];
        else
            uplmc = gpon_tx_utils_calculate_crc((uint8_t *)(xi_ploam_message_ptr), PLOAM_MESSAGE_SIZE);
 
        drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_uplm_set(i, uplmc);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Failed to set SN ranging ploam content due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }

    /* Set the urgent ploam valid bit */
    drv_error = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_set(0, 1);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tx valid bit due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
 
    /* Verify that the valid bit was set correctly */
    for (i = 0; i <= 4; i++) 
    {
        /* Read the valid bits */
        drv_error = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get(&npvld, &upvld);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read tx valid bit due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        /* Read ISR register */
        get_tx_pon_pending_interrupts(&txisr);

        if (!upvld && !txisr.urgent_ploam_transmitted_irq) 
        {
            /* set valid bit once again */
            drv_error = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_set(npvld, 1);
            if (drv_error > 0)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set tx valid bit due to driver error %d.", drv_error);
                return PON_ERROR_DRIVER_ERROR;
            }
        } 
        else 
            break;
    }

    if (i > 4) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set tx valid bit after 5 tries.");
        return PON_ERROR_DRIVER_ERROR;
    }

    p_log(ge_onu_logger.sections.stack.upstream_ploam.print_ploam, 
        "Write urgent Ploam: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
        ((uint8_t *)(xi_ploam_message_ptr))[0], ((uint8_t *)(xi_ploam_message_ptr))[1],
        ((uint8_t *)(xi_ploam_message_ptr))[2], ((uint8_t *)(xi_ploam_message_ptr))[3],
        ((uint8_t *)(xi_ploam_message_ptr))[4], ((uint8_t *)(xi_ploam_message_ptr))[5],
        ((uint8_t *)(xi_ploam_message_ptr))[6], ((uint8_t *)(xi_ploam_message_ptr))[7],
        ((uint8_t *)(xi_ploam_message_ptr))[8], ((uint8_t *)(xi_ploam_message_ptr))[9],
        ((uint8_t *)(xi_ploam_message_ptr))[10], ((uint8_t *)(xi_ploam_message_ptr))[11]);                 
 
    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_send_normal_ploam                                                      */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Send Ploam                                                               */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function send ploam message .                                       */
/*                                                                            */
/* Input:                                                                     */
/*   xi_ploam_message_ptr - request message                                   */
/*   xi_repetition - time to send ploam message                               */
/* Output:                                                                    */
/*                                                                            */
/******************************************************************************/
#ifdef USE_NORMAL_PLOAM_BUFFER
static PON_ERROR_DTE f_send_normal_ploam(GPON_US_PLOAM *xi_ploam_message_ptr,
    GPON_PLOAM_TRANSMISSION_NUMBER xi_normal_ploam_buffer_repetition)
{
#define TRANS_NUM_TO_REPETITION_NUM(r) ((r)- 1)

    int i;
    int drv_error;
    bdmf_boolean upvld;
    uint8_t npvld, nplmc;
    uint8_t prep[4];

    /* Read the valid bits */
    drv_error = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get(&npvld, &upvld);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read tx valid bit due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    if (npvld & (1 << gs_normal_ploam_valid_number))
    {
        return PON_ERROR_GENERAL_ERROR;
    }
    
    for (i = 0; i <= PLOAM_MESSAGE_SIZE; i++) 
    {
        if (i < PLOAM_MESSAGE_SIZE) 
            nplmc = ((uint8_t *)(xi_ploam_message_ptr))[i];
        else
            nplmc = gpon_tx_utils_calculate_crc((uint8_t *)(xi_ploam_message_ptr), PLOAM_MESSAGE_SIZE);
    

        switch (gs_normal_ploam_valid_number) 
        {
        case 0:   
            drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_set(i, nplmc);
            break;
        case 1:        
            drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_set(i, nplmc);
            break;
        case 2:        
            drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_set(i, nplmc);
            break;
        case 3:        
            drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_set(i, nplmc);
            break;
        default:
            return PON_ERROR_INVALID_PARAMETER;
        }

        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Failed to set SN ranging ploam content due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }


    /* Set normal ploam buffer repetition */
    drv_error = ag_drv_gpon_tx_gen_ctrl_nprep_get(&prep[0], &prep[1], &prep[2], &prep[3]);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read normal ploam buffer repetition due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    prep[gs_normal_ploam_valid_number] = TRANS_NUM_TO_REPETITION_NUM(xi_normal_ploam_buffer_repetition);

    drv_error = ag_drv_gpon_tx_gen_ctrl_nprep_set(prep[0], prep[1], prep[2], prep[3]);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read normal ploam buffer repetition due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Enable valid bit */
    drv_error = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_set(1<<gs_normal_ploam_valid_number, 0);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set tx valid bit due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
}

    /* Forward to the next bit (0 - 3) - looking forward to set the next normal valid bit */
    gs_normal_ploam_valid_number = (gs_normal_ploam_valid_number + 1)%NORMAL_PLOAM_BUFFER_NUM;

    p_log(ge_onu_logger.sections.stack.upstream_ploam.print_ploam, 
        "Write Normal Ploam: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
        ((uint8_t *)(xi_ploam_message_ptr))[0], ((uint8_t *)(xi_ploam_message_ptr))[1],
        ((uint8_t *)(xi_ploam_message_ptr))[2], ((uint8_t *)(xi_ploam_message_ptr))[3],
        ((uint8_t *)(xi_ploam_message_ptr))[4], ((uint8_t *)(xi_ploam_message_ptr))[5],
        ((uint8_t *)(xi_ploam_message_ptr))[6], ((uint8_t *)(xi_ploam_message_ptr))[7],
        ((uint8_t *)(xi_ploam_message_ptr))[8], ((uint8_t *)(xi_ploam_message_ptr))[9],
        ((uint8_t *)(xi_ploam_message_ptr))[10], ((uint8_t *)(xi_ploam_message_ptr))[11]);                 
 

    return PON_NO_ERROR;

#undef TRANS_NUM_TO_REPETITION_NUM
}
#endif


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_rx_set_alloc_id_cfg                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Rx - Set Alloc Id configuration.                                    */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function sets the alloc-id and the enables alloc-id bit value.      */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_rx_set_alloc_id_cfg(uint32_t tcont_idx,
    uint16_t alloc_id, bdmf_boolean enable_alloc_id) 
{
    int drv_error;
    uint8_t alloc_en;

    if (tcont_idx >= ALLOC_ID_INDEX_MAX) 
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    /* Configure alloc id table */
    drv_error = ag_drv_gpon_rx_alloc_id_alc_id_cfg_set(tcont_idx, alloc_id);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set alloc-ID configuration due to driver error %d alloc_id %d", drv_error, alloc_id);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Configure alloc id enable table */
    drv_error = ag_drv_gpon_rx_alloc_id_alc_id_en_get(tcont_idx/8, &alloc_en);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read alloc-ID enable table due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    if (enable_alloc_id) 
        alloc_en |= (1 << (tcont_idx%8)); 
    else
        alloc_en &= ~(1 << (tcont_idx%8)); 

    drv_error = ag_drv_gpon_rx_alloc_id_alc_id_en_set(tcont_idx/8, alloc_en);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set alloc-ID enable table due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


/* AES state machine functions */
static void p_aes_invalid_cross(GPON_AES_STATE_MACHINE_DTE *xi_aes_state_machine)
{
    /* Invalid cross */
    p_log(ge_onu_logger.sections.stack.operating.invoke_state_machine_id, 
        "AES state machine: Invalid cross");
}

static void p_aes_send_new_key(GPON_AES_STATE_MACHINE_DTE * xi_aes_state_machine)
{
    uint32_t i;
    GPON_US_PLOAM aes_key;
    uint32_t random_seed;
    int drv_error;

    /* Set the request_key flag */
    xi_aes_state_machine->request_key_arrived = BDMF_TRUE;

    /* Generate new random seed  */
    if (( ++xi_aes_state_machine->seed_counter ) > CE_SEED_THRESHOLD )
    {
        /* Reset seed counter */
        xi_aes_state_machine->seed_counter = 0;

        drv_error = ag_drv_gpon_rx_general_config_randomsd_get(&random_seed);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to Generate AES new random seed : error %d !", drv_error);
        }
        else
        {
            p_log(ge_onu_logger.sections.isr.debug.aes_id, 
                "Generate AES new random seed : %u !", random_seed);
            /* Set the new random seed */
            bdmf_srand(random_seed);
        }
    }

    /* Increment the key index */
    xi_aes_state_machine->key_index++;

    /* Generate new AES key */
    for (i = 0; i < 4; i++ )
    {
        /* Generate random key - rand () returned 16 bits random number */
        xi_aes_state_machine->new_key [ i ] = bdmf_rand16() +(bdmf_rand16() << 16);
        p_log(ge_onu_logger.sections.isr.debug.aes_id,
                "AES state machine: Generate new key: key part [%d] key %d key_index %d ",
                i,
                xi_aes_state_machine->new_key [ i ],
                xi_aes_state_machine->key_index);
    }

    /* Generate the new AES key */
    for (i = 0; i < AES_FRAGMENTS_NUMBER; i ++ )
    {
        aes_key.onu_id = xi_aes_state_machine->aes_message_ptr->onu_id;
        aes_key.message_id = CE_US_ENCRYPTION_KEY_MESSAGE_ID;
        aes_key.message.encryption_key.key_index = xi_aes_state_machine->key_index;
        aes_key.message.encryption_key.fragment_index = i;
        aes_key.message.encryption_key.key[0] = (xi_aes_state_machine->new_key[((int) i ) * 2] & 0xff000000 ) >> 24;
        aes_key.message.encryption_key.key[1] = (xi_aes_state_machine->new_key[((int) i ) * 2] & 0x00ff0000 ) >> 16;
        aes_key.message.encryption_key.key[2] = (xi_aes_state_machine->new_key[((int) i ) * 2] & 0x0000ff00 ) >> 8;
        aes_key.message.encryption_key.key[3] = (xi_aes_state_machine->new_key[((int) i ) * 2] & 0x000000ff);
        aes_key.message.encryption_key.key[4] = (xi_aes_state_machine->new_key[((int) i ) * 2 + 1] & 0xff000000 ) >> 24;
        aes_key.message.encryption_key.key[5] = (xi_aes_state_machine->new_key[((int) i ) * 2 + 1] & 0x00ff0000 ) >> 16;
        aes_key.message.encryption_key.key[6] = (xi_aes_state_machine->new_key[((int) i ) * 2 + 1] & 0x0000ff00 ) >> 8;
        aes_key.message.encryption_key.key[7] = (xi_aes_state_machine->new_key[((int) i ) * 2 + 1] & 0x000000ff);

        p_log(ge_onu_logger.sections.isr.debug.aes_id, "AES: Send new key %d times", i);

        /* Send the message as an normal PLOAM message */
        if (f_gpon_txpon_send_ploam_message(
            &aes_key, NORMAL_PLOAM_CELL, AES_MESSAGES_NUMBER) != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Fail to send normal PLOAM message");
        }
    }

    /* Move AES state machine to wait for "key switching time" */
    xi_aes_state_machine->aes_state = CS_AES_STATE_SCHEDULING;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_aes_encryption_key                                              */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   returns the aes encryption key                                           */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns aes encryption key.                                */
/*                                                                            */
/* Input:                                                                     */
/* Output:                                                                    */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_aes_encryption_key(AES_KEY_DTE *xi_aes_encryption_key, uint8_t xi_fragment_index)
{
    if (gs_gpon_database.link_parameters.operation_state_machine.activation_state != OPERATION_STATE_O5 &&
        gs_gpon_database.link_parameters.operation_state_machine.activation_state != OPERATION_STATE_O7 )
    {
        return PON_ERROR_INVALID_STATE;
    }

    *xi_aes_encryption_key = htonl(gs_gpon_database.link_parameters.aes_state_machine.new_key[xi_fragment_index]);

    return PON_NO_ERROR;
}

static void p_aes_setting_new_key_switching_time(GPON_AES_STATE_MACHINE_DTE * xi_aes_state_machine )
{
    uint32_t super_frame;
    PON_ERROR_DTE gpon_error;
    bdmf_boolean decryption_arm;
    uint32_t new_frame_counter = 0;
    uint32_t sf_gap;
    uint32_t decryption_switch_time;
    PON_AES_INDICATION aes_error;
    uint16_t switching_time_message_counter;
    int drv_error;
    int i;

    p_log(ge_onu_logger.sections.stack.downstream_ploam.key_switching_time_id, "Message record data: %02X %02X %02X %02X %02X %02X",
            xi_aes_state_machine->aes_message_ptr->message.key_switching_time.frame_counter[ 0 ],
            xi_aes_state_machine->aes_message_ptr->message.key_switching_time.frame_counter[ 1 ],
            xi_aes_state_machine->aes_message_ptr->message.key_switching_time.frame_counter[ 2 ],
            xi_aes_state_machine->aes_message_ptr->message.key_switching_time.frame_counter[ 3 ],
            xi_aes_state_machine->aes_message_ptr->message.key_switching_time.unspecified[ 0 ] ,
            xi_aes_state_machine->aes_message_ptr->message.key_switching_time.unspecified[ 1 ]);

    /* Build the new frame counter */
    new_frame_counter |=(( uint32_t ) xi_aes_state_machine->aes_message_ptr->message.key_switching_time.frame_counter[ 0 ] ) << 24;
    new_frame_counter |=(( uint32_t ) xi_aes_state_machine->aes_message_ptr->message.key_switching_time.frame_counter[ 1 ] ) << 16;
    new_frame_counter |=(( uint32_t ) xi_aes_state_machine->aes_message_ptr->message.key_switching_time.frame_counter[ 2 ] ) << 8;
    new_frame_counter |=(( uint32_t ) xi_aes_state_machine->aes_message_ptr->message.key_switching_time.frame_counter[ 3 ] ) ;

    switching_time_message_counter =(( xi_aes_state_machine->aes_message_ptr->message.key_switching_time.unspecified[ 0 ] ) & 0x00FF ) << 8;
    switching_time_message_counter +=(xi_aes_state_machine->aes_message_ptr->message.key_switching_time.unspecified[ 1 ] & 0x00FF ) ;

    p_log(ge_onu_logger.sections.stack.debug.general_id, "Switching time message counter: %u", switching_time_message_counter);

    /* Key switch out of sync - scenario A : key_switching before request key */
    if (xi_aes_state_machine->request_key_arrived == BDMF_FALSE )
    {
        p_log(ge_onu_logger.sections.isr.debug.aes_id, "AES: Key switching out of sync - scenario A");

        /* Setting AES error struct */
        aes_error.error_id = CS_AES_ERROR_KEY_SWITCHING_TIME_WITHOUT_KEY_REQUEST;
        aes_error.key_switching_time = new_frame_counter;

        /* Send indication to pon task according AES error */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION,
                                                       PON_INDICATION_AES_ERROR,
                                                       aes_error);
        if (gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send oam indication: error %d !", gpon_error);
        }

        return;
    }

    drv_error = ag_drv_gpon_rx_encryption_sf_cntr_get(&super_frame);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.isr.debug.aes_id,
            "AES: unable to get super frame counter: error %d ", drv_error);
    }

    /* If key switching time arrived with new_frame_counter AND
   (HW_frame_counter - new_frame_counter ) < AES_key_switching_time_threshold
    then declare for Scenario C */

       if (super_frame > new_frame_counter)
        sf_gap = (AES_MAX_KEY_SWITCHING_TIME - super_frame) + new_frame_counter;
    else
        sf_gap = new_frame_counter - super_frame;

    if ((sf_gap < AES_KEY_SWITCHING_TIME_MIN) || (sf_gap > AES_KEY_SWITCHING_TIME_MAX))
    {
        /* ILLegal super frame counter arrived */
        p_log(ge_onu_logger.sections.isr.debug.aes_id,
                "AES: Illegal super frame counter(under threshold ), super_frame %d new_frame_counter %d \n" ,
                super_frame,
                new_frame_counter);

        /* Key switch out of sync - scenario C : new key_switching time received but the switching time elapsed  */
        p_log(ge_onu_logger.sections.isr.debug.aes_id,
                "AES: Key switching out of sync - scenario C "
                "HW super frame %d, new_frame_counter %d",
                super_frame,
                new_frame_counter);

        /* Setting AES error struct */
        aes_error.error_id = CS_AES_ERROR_KEY_SWITCHING_TIME_JUST_PASSED;
        aes_error.key_switching_time = new_frame_counter;
        aes_error.current_superframe = super_frame;

        /* Send indication to Pon task according AES error */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION,
                                                       PON_INDICATION_AES_ERROR,
                                                       aes_error);
        if (gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send oam indication: error %d !", gpon_error);
        }

        /* Set switching time to immediate value - actually useless */
        new_frame_counter = (3 + super_frame) & AES_MAX_KEY_SWITCHING_TIME;
    }
    else
    {
        /* Legal super frame counter arrived */
        p_log(ge_onu_logger.sections.isr.debug.aes_id,
                "AES: Legal super frame counter(under threshold ), super_frame %d new_frame_counter %d \n" ,
                super_frame,
                new_frame_counter);
    }

    drv_error = ag_drv_gpon_rx_encryption_switch_time_get(&decryption_switch_time, &decryption_arm);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "AES: unable to get decryption switch time configuration : error %d ", drv_error);
    }

    /* Key switch out of sync - scenario B : new key_switching_time received before current switching time exchanged */
    /* Check if current(waiting ) switching time already exchanged - except for the first time */
    if (( gs_gpon_database.link_parameters.aes_key_switching_first_time == BDMF_FALSE ) &&(decryption_arm == 1 ) )
    {
        p_log(ge_onu_logger.sections.isr.debug.aes_id,
                "AES: Key switching out of sync - scenario B "
                "AES: Arm Bit is already ON ,HW super frame counter %d , current switching time %d",
                super_frame,
                xi_aes_state_machine->current_switching_time);

        /* Setting AES error struct */
        aes_error.error_id = CS_AES_ERROR_KEY_SWITCHING_TIME_BEFORE_CURRENT_SET;
        aes_error.key_switching_time = new_frame_counter;
        aes_error.current_superframe = super_frame;

        /* Send indication to Pon task according AES error */
        gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION,
                                                       PON_INDICATION_AES_ERROR,
                                                       aes_error);
        if (gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send user indication: error %d !", gpon_error);
        }
    }

    p_log(ge_onu_logger.sections.isr.debug.aes_id, "AES: Setting new key %d ", new_frame_counter);

    for (i=0; i < GPON_DEFAULT_AES_KEY_SIZE; i++)
    { 
        drv_error = ag_drv_gpon_rx_encryption_key_set(i, xi_aes_state_machine->new_key[i]);
        if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to set new key 0x%02X%02X%02X%02X, due to driver error %d",
                xi_aes_state_machine->new_key[0], xi_aes_state_machine->new_key[1],
                xi_aes_state_machine->new_key[2], xi_aes_state_machine->new_key[3], drv_error);
            break;
        }
    }

    p_log (ge_onu_logger.sections.isr.debug.aes_id, "AES: Setting new key switching time");

    drv_error = ag_drv_gpon_rx_encryption_switch_time_set(new_frame_counter, 1);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set new key switching time %d, due to driver error %d.",
            new_frame_counter, drv_error);
    }

    /* Setting AES error struct */
    aes_error.key_switching_time = new_frame_counter;
    aes_error.current_superframe = super_frame;

    /* Send indication to Pon task according AES error */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION,
                                                   PON_INDICATION_AES_KEY_SWITCHING_TIME_MESSAGE,
                                                   aes_error);
    if (gpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send user indication: error %d !", gpon_error);
    }

    /* Set key switching time boolean flag */
    gs_gpon_database.link_parameters.aes_key_switching_first_time = BDMF_FALSE;

    /* Save the current switching time */
    xi_aes_state_machine->current_switching_time = new_frame_counter;

    /* Clear the request key arrived flag - to mark that correct request key message arrived */
    xi_aes_state_machine->request_key_arrived = BDMF_FALSE;

    /* Move AES state machine to wait for "key switching time" */
    xi_aes_state_machine->aes_state = CS_AES_STATE_STANDBY;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   f_set_preassigned_ted                                                    */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Extract and set preassign delay from the message                         */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This procedure extracts overhead data from the PLOAM message and sets    */
/*   it in appropriate registers                                              */
/*                                                                            */
/* Input:                                                                     */
/*   xi_ploam_ptr - pointer to the Upstream overhead PLOAM message            */
/*   xi_random_flag - define if random deleay will add to the preassigned ted */
/* Output:                                                                    */
/*                                                                            */
/******************************************************************************/
static uint32_t f_set_preassigned_ted(GPON_DS_US_OVERHEAD_PLOAM *xi_ploam_ptr,
    bool xi_random_flag)
{
    uint32_t preassigned_equalization_delay = 0;
    uint32_t frame_length = 0;
    uint16_t random_delay = 0;
    uint32_t random_seed;
    int drv_error;

    /* Set preassigned ted in appropriate register and save its SW shadow */
    frame_length = CE_US_BITS_PER_FRAME_RATE;

    preassigned_equalization_delay = gs_gpon_database.physical_parameters.min_response_time * CE_US_RATE / 1000;
    p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id, "Min response time = %d bit (%d usec) ", preassigned_equalization_delay, gs_gpon_database.physical_parameters.min_response_time);

    /* If preassigned equalization delay status is set then add the preassigned equalization delay - multiple 8*32 = 256(since the preassigned is in 32 byte units ) */
    if (gs_gpon_database.onu_parameters.pre_equalization_status == BDMF_TRUE )
    {
        preassigned_equalization_delay += 256 *(((( uint32_t ) xi_ploam_ptr->preassigned_equalization_delay [ 0 ] ) << 8 )
                                                  |
                                                 (( uint32_t ) xi_ploam_ptr->preassigned_equalization_delay [ 1 ] ));

        p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id, "Preassigned equalization delay after adding the preassigned equalization delay = %d bit (%d usec) ", preassigned_equalization_delay, gs_gpon_database.physical_parameters.min_response_time);
    }

    /* Adding the random delay - to avoid conflict with other ONU's - in the range of 0 - 48 [usec]  */
    if (xi_random_flag == BDMF_TRUE )
    {
        drv_error = ag_drv_gpon_rx_general_config_randomsd_get(&random_seed);
        if (drv_error > 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to Generate AES new random seed : error %d !", drv_error);
        }
        else
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id, "Generate AES new random seed : %u !", random_seed);
            /* Set the new random seed */
            bdmf_srand(random_seed);
        }

        /* With random delay in bits */
        random_delay = bdmf_rand16 () %(( CE_RANDOM_DELAY_MAX * CE_US_RATE ) / 1000);
    }
    else
    {
        /* Without random delay */
        random_delay = 0;
    }

    /* random delay in 32-byte units */
    random_delay = random_delay /(PON_BITS_PER_BYTE * 32);

    p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id, "Random delay %u [bits]",(random_delay * PON_BITS_PER_BYTE * 32 ));
    preassigned_equalization_delay +=(random_delay * PON_BITS_PER_BYTE * 32);
    p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id, "Preassigned equalization delay after adding the random delay = %d bit ", preassigned_equalization_delay);

    /* Extract the number of frames from the ranging delay, which is structured of bytes. */
    gs_preassigned_ted.frame_delay = preassigned_equalization_delay / frame_length;

    /* Extract the number of bytes from the ranging delay, after extracting the frames. */
    gs_preassigned_ted.byte_delay =(preassigned_equalization_delay % frame_length ) / PON_BITS_PER_BYTE;

    /* Extract the remaining bits from the ranging delay, after extracting frames and bytes. */
    gs_preassigned_ted.bit_delay =(preassigned_equalization_delay % frame_length ) % PON_BITS_PER_BYTE;


    if (ag_drv_gpon_tx_gen_ctrl_tdly_set(gs_preassigned_ted.byte_delay, gs_preassigned_ted.frame_delay, 
        gs_preassigned_ted.bit_delay) != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Fail to set ted");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
                     "Set preassigned ted. frame=%d, byte=%d, bit=%d",
                     gs_preassigned_ted.frame_delay,
                     gs_preassigned_ted.byte_delay,
                     gs_preassigned_ted.bit_delay);
    }

    /* return random delay in 32-byte units */
    return(random_delay);
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_set_ted                                                                */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Extract and set equalization delay from the message                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This procedure extracts equaliztion delay from the PLOAM message and sets*/
/*   it in appropriate registers                                              */
/*                                                                            */
/* Input:                                                                     */
/*   delay - pointer to the Ranging time PLOAM message delay fields           */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/******************************************************************************/
static void p_set_ted(uint8_t *delay)
{
    uint32_t equalization_delay = 0;
    uint32_t frame_length = 0;
    uint32_t min_response_time = 0;

    frame_length = CE_US_BITS_PER_FRAME_RATE;

    min_response_time = gs_gpon_database.physical_parameters.min_response_time * CE_US_RATE / 1000;
    p_log(ge_onu_logger.sections.stack.debug.general_id, "Min response time = %d bit (%d usec) ", min_response_time, gs_gpon_database.physical_parameters.min_response_time);

    /* Adding the preassigned equalization delay */
    equalization_delay = (((uint32_t)delay[0])<<24) | (((uint32_t)delay[1])<<16) |
        (((uint32_t)delay[2])<<8) | ((uint32_t)delay[3]);

    gs_gpon_database.onu_parameters.delay = equalization_delay;
    equalization_delay += min_response_time;

    p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id, "Equalization delay after adding the minimum response delay = %d bit (%d usec) ", equalization_delay, gs_gpon_database.physical_parameters.min_response_time);

    gs_gpon_database.onu_parameters.preassigned_equalization_delay = equalization_delay;

    /* Extract the number of frames from the ranging delay, which is structured of bytes. */
    gs_preassigned_ted.frame_delay = equalization_delay / frame_length;

    /* Extract the number of bytes from the ranging delay, after extracting the frames. */
    gs_preassigned_ted.byte_delay =(equalization_delay % frame_length ) / PON_BITS_PER_BYTE;

    /* Extract the remaining bits from the ranging delay, after extracting frames and bytes. */
    gs_preassigned_ted.bit_delay =(equalization_delay % frame_length ) % PON_BITS_PER_BYTE;

    if (ag_drv_gpon_tx_gen_ctrl_tdly_set(gs_preassigned_ted.byte_delay, gs_preassigned_ted.frame_delay, 
        gs_preassigned_ted.bit_delay) != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Fail to set ted");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
                     "Set preassigned ted. frame=%d, byte=%d, bit=%d",
                     gs_preassigned_ted.frame_delay,
                     gs_preassigned_ted.byte_delay,
                     gs_preassigned_ted.bit_delay);
    }
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_link_activate                                                       */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON -  Activate link                                                    */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   Activates gpon link.                                                     */
/*   The function is asynchronous and the caller should wait for completion   */
/*   indication.                                                              */
/*                                                                            */
/* Input:                                                                     */
/*    xi_initial_state_disable -                                              */
/*      defines if the ONU previous state was O1 or O7                        */
/*                                                                            */
/* Output:                                                                    */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_link_activate(bdmf_boolean xi_initial_state_disable)
{
    PON_ERROR_DTE  gpon_error;
    bdmf_boolean lof_state;
    bdmf_boolean fec_state;
    bdmf_boolean lcdg_state;
    uint8_t bit_align;
    bdmf_error_t bdmf_error;
    OPERATION_SM_PARAMS_DTS operation_sm_params;
    int drv_error;
#if defined(CONFIG_BCM96858)
    TRX_SIG_ACTIVE_POLARITY  tx_sd_polarity ;
    int                      rc ;
    uint8_t                  lbe_invert_bit_val ;
    int bus = -1;
 
    opticaldet_get_xpon_i2c_bus_num(&bus);
#endif

    /* Varify that ONU initiliazed */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Varify that Link is inactive */
    if (gs_gpon_database.link_parameters.operation_state_machine.link_state != LINK_STATE_INACTIVE )
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Configure Rx & Tx */
    /* Disable and clear Alloc-Id in the 0th entry of the Alloc id table - disable OMCI channel */
    gpon_error = gpon_rx_set_alloc_id_cfg(GPON_OMCI_ALLOC_ID_INDEX,
        GPON_DEF_ALLOC_ID, BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Unable to clear OMCI Rx alloc id and its valid bit: alloc_id_index %d.", 
           GPON_OMCI_ALLOC_ID_INDEX);
       return gpon_error;
    }

    /* Set Rx ranging alloc id and disable it */
    gpon_error = gpon_rx_set_ranging_alloc_id_configuration(
        GPON_RX_RANGING_ALLOC_ID_DEF, BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR)
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Unable to clear Rx Ranging alloc id and its valid bit.");
       return gpon_error;
    }

    /* Clear Rx ONU ID */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(0, GPON_RX_ONU_ID_DEF, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx ONU-ID due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Clear Tx - Normal ploam repetition reg */
    drv_error = ag_drv_gpon_tx_gen_ctrl_nprep_set(0, 0, 0 ,0);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear Tx normal ploam repetition due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Clear all TX buffers and valid bits */
    /* flush tx normal queue and urgent queue of PLOAM messages. */
    /* it may occure that PLOAM messages are waiting on the queues for TX buffer to be freed. */
    gpon_error = f_gpon_txpon_flush_queue();
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Fail to flush queue");
        return(gpon_error);
    }

    /* Clear Normal ploam repetition normal valid bits */
    drv_error = ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tx ploam bit due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_FALSE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tx ploam bit due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }


    /* Clear TX ONU ID */
    drv_error = ag_drv_gpon_tx_general_configuration_onuid_set(GPON_TX_ONU_ID_DEF);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Tx ONU-ID due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Initialization */

    /* Activation state machine control */
    gs_gpon_database.link_parameters.operation_state_machine.link_state = LINK_STATE_ACTIVE;
    gs_gpon_database.link_parameters.operation_state_machine.link_sub_state = LINK_SUB_STATE_STANDBY;

    if (xi_initial_state_disable == BDMF_TRUE)
    {
        gs_gpon_database.link_parameters.operation_state_machine.link_sub_state = LINK_SUB_STATE_STOP;
        gs_gpon_database.link_parameters.operation_state_machine.activation_state = OPERATION_STATE_O7;
    
        /* Set Broadcast Id and set valid bit */
        drv_error = ag_drv_gpon_rx_ploam_onu_id_set(1, gs_gpon_database.onu_parameters.broadcast_default_value, BDMF_FALSE);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to enable Rx Broadcast ONU-ID due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }
    else
    {
        /* According to RX link sub state will be */
        
        /* Check LOF & LCDG */
        bdmf_error = ag_drv_gpon_rx_general_config_rcvr_status_get(&lof_state, &fec_state,
            &lcdg_state, &bit_align);
        if (bdmf_error < 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to get LCDG & LOF status due to Driver error %d", bdmf_error);
            return PON_ERROR_DRIVER_ERROR;
        }
        
        /* Set the Operation SM according to HW */
        operation_sm_params.lof_state = lof_state;
        operation_sm_params.lcdg_state = lcdg_state;
        
        if (lof_state == BDMF_TRUE || lcdg_state == BDMF_TRUE) 
        {
            gs_gpon_database.link_parameters.operation_state_machine.activation_state = OPERATION_STATE_O1;
        
            /* LOF or LCDG alarms set */
            p_operation_state_machine(CS_OPERATION_EVENT_DETECT_DS_LOS_OR_LOF, &operation_sm_params);
        }
        else
        {
            gs_gpon_database.link_parameters.operation_state_machine.activation_state = OPERATION_STATE_O2;
        
            /* LOF and LCDG alarms cleared */
            p_operation_state_machine(CS_OPERATION_EVENT_CLEAR_DS_LOS_OR_LOF, &operation_sm_params);
        
            /* Enable Broadcast Id and set valid bit */
            drv_error = ag_drv_gpon_rx_ploam_onu_id_set(1, gs_gpon_database.onu_parameters.broadcast_default_value, BDMF_FALSE);
            if (drv_error > 0)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to Enable Rx Broadcast ONU-ID due to driver error %d.", drv_error);
                return PON_ERROR_DRIVER_ERROR;
            }
        }
        pon_serdes_lof_fixup_irq(lof_state);
    }
    
    gs_gpon_database.link_parameters.serial_number_request_messages_counter = 0;
#if defined(CONFIG_BCM96858)     
    rc = trx_get_tx_sd_polarity(bus, &tx_sd_polarity) ;
    if (rc == 0)
    {
        gs_gpon_database.physical_parameters.transceiver_dv_polarity = (tx_sd_polarity == TRX_ACTIVE_LOW)? 0 : 1 ;
        lbe_invert_bit_val = gpon_ngpon_get_lbe_invert_bit_val () ;
        gpon_ngpon_wan_top_set_lbe_invert(lbe_invert_bit_val) ;
    }
    /* Otherwise use default value already set in driver's database */
#endif
    /* Set DV Setup and hold */
    drv_error = ag_drv_gpon_tx_general_configuration_dvslp_set(
        gs_gpon_database.physical_parameters.transceiver_dv_polarity,
        gs_gpon_database.OpticsType == BP_GPON_OPTICS_TYPE_PMD ? BDMF_TRUE : BDMF_FALSE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tranceiver polarity due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_general_configuration_dvstp_set(
        gs_gpon_database.physical_parameters.transceiver_dv_setup_pattern);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tranceiver dv setup pattern due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_general_configuration_dvhld_set(
        gs_gpon_database.physical_parameters.transceiver_dv_hold_pattern);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tranceiver hold pattern due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Clear Normal ploam repetition normal valid bits (TPCLR) */
    drv_error = ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tx ploam bit due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_FALSE);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tx ploam bit due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }


    /* Enable Rx unit */
    gs_gpon_database.rx_parameters.rcvr_config.rx_disable = BDMF_FALSE; /* Enable */
    drv_error = ag_drv_gpon_rx_general_config_rcvr_config_set(&gs_gpon_database.rx_parameters.rcvr_config);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Rx paramters due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Enable interrupts - drivers */

    /* Enable Tx interrupts */
    gpon_error = gpon_unmask_tx_pon_interrupts();

    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d !", gpon_error);
        return gpon_error;
    }

    /* Enable Rx interrupts */
    gpon_error = gpon_unmask_rx_pon_interrupts();
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d !", gpon_error);
        return gpon_error;
    }

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_link_deactivate                                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON -  Deactivate link                                                   */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function deactivates the specified link                             */
/*                                                                            */
/* This function is asynchronous and the caller should wait for completion    */
/* indication                                                                 */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_link_deactivate(void)
{
    PON_ERROR_DTE  gpon_error;
    uint32_t  alloc_id_index;
    int drv_error;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Link already deactivated? */
    if (gs_gpon_database.link_parameters.operation_state_machine.link_state != LINK_STATE_ACTIVE)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Stop TO1 timer*/
    bdmf_timer_stop(& gs_gpon_database.os_resources.to1_timer_id);
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop timer TO1");

    /* Stop TO2 timer*/
    bdmf_timer_stop(& gs_gpon_database.os_resources.to2_timer_id);
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop timer TO2");

    /* HW_WA_2 */
    /* Stop LCDG timer*/
    bdmf_timer_stop(& gs_gpon_database.os_resources.lcdg_timer_id);
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop timer LCDG");

    /* Stop all timers: Pee , Ber and Keep alive  */
    bdmf_timer_stop(& gs_gpon_database.os_resources.keep_alive_timer_id);

    /* Stop PEE interval */
    gpon_error = f_pon_stop_pee_interval();
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to stop PEE interval %d !", gpon_error);
        return gpon_error;
    }

    /* Stop BER interval */
    gpon_error = f_pon_stop_ber_interval ();
    if (gpon_error != PON_NO_ERROR )
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Unable to stop BER interval %d !", gpon_error);
       return gpon_error;
    }

    /* Disable interrupts - driver */

    /* Disable Tx interrupts */
    gpon_error = gpon_mask_tx_pon_interrupts ();
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Tx %d !", gpon_error);
        return gpon_error;
    }

    /* Disable Rx interrupts */
    gpon_error = gpon_mask_rx_pon_interrupts();
    if (gpon_error != PON_NO_ERROR)
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Unable to disable Rx %d !", gpon_error);
       return gpon_error;
    }

    /* Disable units - driver */

    /* RX Clear Broadcast Id and valid bit */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(1, 
        gs_gpon_database.onu_parameters.broadcast_default_value, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx Broadcast ONU-ID due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Disable ONU ID */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(0, GPON_RX_ONU_ID_DEF, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx ONU-ID due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* TX Clear Normal ploam repetition normal valid bits(TPCLR )*/
    drv_error = ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tx ploam bit due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_FALSE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tx ploam bit due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Disable and clear Alloc-Id in the 0th entry of the Alloc id table - disable OMCI channel */
    gpon_error = gpon_rx_set_alloc_id_cfg (GPON_OMCI_ALLOC_ID_INDEX,
        GPON_DEF_ALLOC_ID, BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear OMCI Rx alloc id and its valid bit due to Driver error %d alloc_id_index %d!", 
            gpon_error, GPON_OMCI_ALLOC_ID_INDEX);
        return(PON_ERROR_DRIVER_ERROR);
    }

    /* Disable HW:Rx allocation table for data according to SW allocation table */
    for (alloc_id_index = NUMBER_OF_FIRST_TCONT_INDEX_FOR_DATA;
        alloc_id_index <= NUMBER_OF_LAST_TCONT_INDEX_FOR_DATA; alloc_id_index++)
    {
        if (gs_gpon_database.tconts_table[alloc_id_index].alloc_id != PON_ALLOC_ID_DTE_NOT_ASSIGNED)
        {
            gpon_error = gpon_rx_set_alloc_id_cfg(alloc_id_index,
                gs_gpon_database.tconts_table[alloc_id_index].alloc_id, BDMF_FALSE);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to clear Rx alloc id table and its valid bit due to Driver error %d alloc_id_index %d!", 
                    gpon_error, alloc_id_index);
                return PON_ERROR_DRIVER_ERROR;
            }
        }
    }

    /* Clear the SW allocation table - clear only the alloc row */
    gpon_error = f_clear_sw_allocation_table();
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear allocation table due to error:%d!", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* flush tx normal queue and urgent queue of PLOAM messages. */
    /* it may occure that PLOAM messages are waiting on the queues for TX buffer to be freed. */
    gpon_error = f_gpon_txpon_flush_queue();
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Fail to flush queue");
        return(gpon_error);
    }

    /* Clear Tx PLOAM valid bit */
    ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_TRUE);
    ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_FALSE);

    /* Diasble Rx */
    gpon_error = gpon_rx_set_rx_status(BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR)
    {
    	p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to diasble Rx due to Driver error %d", gpon_error);
    	return PON_ERROR_DRIVER_ERROR;
    }

    /* Cleanup - Activation state machine control */    
    gs_gpon_database.link_parameters.operation_state_machine.link_state = LINK_STATE_INACTIVE;
    gs_gpon_database.link_parameters.operation_state_machine.link_sub_state = LINK_SUB_STATE_NONE;
    gs_gpon_database.link_parameters.operation_state_machine.activation_state = OPERATION_STATE_O1;

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_link_reset                                                          */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Link Reset                                                        */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function resets the hardware link                                   */
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
PON_ERROR_DTE gpon_link_reset(void)
{
    uint32_t txfifo_id;
    uint32_t tx_queue_offset = 0;
    uint32_t tx_queue_size = 0;
    uint32_t tx_queue_size_group_0 = 0;
    uint32_t tx_queue_size_group_1 = 0;
    uint32_t tx_queue_size_group_2 = 0;
    uint32_t tx_queue_size_group_3 = 0;
    uint32_t tx_bd_offset = 0;
    GPON_TX_ISR tx_ier_register = {};
    uint16_t rx_ier_register = 0;
    uint32_t index;
    GPON_RXPON_PM_COUNTERS_DTE rxpon_pm_counters;
    uint16_t illegal_access_counter;        
    uint32_t idle_ploam_counter;
    uint32_t normal_ploam_counter;
    uint16_t requested_ranging_ploam_number;  
    uint16_t requested_urgent_ploam_number;  
    gpon_tx_gen_ctrl_ten tx_params; 
    bdmf_boolean urgent_valid_bit;
    uint8_t normal_valid_bit;
    PON_ERROR_DTE gpon_error;
    int drv_error;
    uint8_t dbr_divide_ratio;
    PON_FIFO_ID_DTE tcont_id;
    uint32_t spec_func_reg_val;
#ifdef CONFIG_BCM_GPON_TODD
    bdmf_boolean tod_arm, tod_en;
#if !(defined(CONFIG_BCM96838) || defined(CONFIG_BCM96848))
    uint8_t rdarm;
#endif
#endif

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Link already activated? */
    if (gs_gpon_database.link_parameters.operation_state_machine.link_state == LINK_STATE_ACTIVE)
    {
        return PON_ERROR_INVALID_STATE;
    }

    drv_error = ag_drv_gpon_rx_general_config_rcvr_config_set(&gs_gpon_database.rx_parameters.rcvr_config);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Rx paramters due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_rx_general_config_lof_params_set(
        gs_gpon_database.physical_parameters.number_of_psyncs_for_lof_assertion,
        gs_gpon_database.physical_parameters.number_of_psyncs_for_lof_clear);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure LOF paramters due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_rx_ploam_params_set(gs_gpon_database.rx_parameters.rx_crc_override, 
        gs_gpon_database.rx_parameters.rx_idle_ploam_filltering_enable, 
        gs_gpon_database.rx_parameters.rx_forward_all_ploam_messages);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure PLOAM paramters due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_rx_clk_8khz_clk_8khz_conf_set(
        gs_gpon_database.rx_parameters.rx_8khz_clock_phase_diff,
        gs_gpon_database.rx_parameters.rx_8khz_clock_lock ? BDMF_TRUE : BDMF_FALSE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure 8KHZ clock due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_rx_congestion_thresh_set(gs_gpon_database.rx_parameters.rx_congestion_into_threshold,
        gs_gpon_database.rx_parameters.rx_congestion_out_of_threshold);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Rx congestion thresholds due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }


    drv_error = ag_drv_gpon_tx_gen_ctrl_ten_get(&tx_params);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read tx paramters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    tx_params.scren = gs_gpon_database.tx_parameters.tx_scrambler_enable;     
    tx_params.bipen = gs_gpon_database.tx_parameters.tx_bip_enable;     
    tx_params.fecen = gs_gpon_database.tx_parameters.tx_fec_enable;
    tx_params.loopben = gs_gpon_database.tx_parameters.tx_loopback_enable;
    tx_params.plsen = gs_gpon_database.tx_parameters.tx_misc_enable;

    drv_error = ag_drv_gpon_tx_gen_ctrl_ten_set(&tx_params);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure tx paramters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = gpon_tx_set_pls_status(gs_gpon_database.tx_parameters.tx_pls_enable);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Tx PLS status due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Configure tx data polarity */
    drv_error = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_set(
        gs_gpon_database.tx_parameters.tx_data_out_polarity);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure tx data polarity due to driver error %d !", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Disable short allocation */
    drv_error = ag_drv_gpon_tx_general_configuration_esall_set(BDMF_FALSE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure tx short allocation enable bit due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Set PLS Specification - pattern and length */
    drv_error = ag_drv_gpon_tx_general_configuration_plss_set(
        gs_gpon_database.physical_parameters.transceiver_power_calibration_pattern);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure PLS Tx due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_general_configuration_plsrs_set(
        gs_gpon_database.physical_parameters.transceiver_power_calibration_size/sizeof(uint32_t));
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure PLS Tx due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_general_configuration_dpen_set(
        gs_gpon_database.physical_parameters.transceiver_data_pattern_enable, 
        gs_gpon_database.physical_parameters.transceiver_prbs_enable, 
        gs_gpon_database.physical_parameters.transceiver_data_setup_length, 
        gs_gpon_database.physical_parameters.transceiver_data_hold_length);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure transceiver data pattern due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_general_configuration_dstp_set(
        gs_gpon_database.physical_parameters.transceiver_data_setup_pattern);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure transceiver data pattern due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_general_configuration_dhld_set(
        gs_gpon_database.physical_parameters.transceiver_data_hold_pattern);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure transceiver data pattern due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* TCONT Front End buffer configuration - based groups */
    /* 1st Group - TCONT 0-7 */
    for (txfifo_id = TXPON_FIFO_UNIT_LOW; txfifo_id <= CS_TXPON_FIFO_UNIT_HIGH_FOR_GROUP_0; txfifo_id ++)
    {
        /* TX FIFO size should be in units of 4 bytes */
        tx_queue_size = CS_TCONT_FRONT_END_SIZE_TCONT_0_7 / 4;

        drv_error = ag_drv_gpon_tx_fifo_configuration_pdp_set(txfifo_id, tx_bd_offset, TCONT_PKT_DESCRIPTORS_NUMBER(CS_TCONT_FRONT_END_SIZE_TCONT_0_7));
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to set packet descriptor configuration due to driver error %d", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        drv_error = ag_drv_gpon_tx_fifo_configuration_tqp_set(txfifo_id, tx_queue_offset, tx_queue_size);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to set fifo configuration due to driver error %d", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        /* Update the buffer descriptor offset */
        tx_bd_offset += TCONT_PKT_DESCRIPTORS_NUMBER(CS_TCONT_FRONT_END_SIZE_TCONT_0_7);

        /* Update the TX FIFO offset. It points to word of 4 bytes */
        tx_queue_offset += tx_queue_size;
    }

    /*  TCONT Front End buffer configuration for TCONT 8 and up */

    /* Group 0 - TCONT 8-15 */
    /* TX FIFO group size(units of 4 bytes ) */
    tx_queue_size_group_0 = CS_TCONT_FRONT_END_SIZE_TCONT_8_15  / 4;

    gpon_error = gpon_tx_set_group_txfifo_configuration(CE_TXPON_FRONT_END_BUFFER_GROUP_ID_0,
        tx_bd_offset,
        tx_queue_offset, /* tx_queue_size_group_0 * CS_NUMBER_OF_TCONT_IN_GROUP * (CE_TXPON_FRONT_END_BUFFER_GROUP_ID_0 + 1) */
        tx_queue_size_group_0);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to cofigure tx fifo group 0 - TCONT 8-15");
        return gpon_error;
    }

    tx_bd_offset += TCONT_PKT_DESCRIPTORS_NUMBER(CS_TCONT_FRONT_END_SIZE_TCONT_8_15) * CS_NUMBER_OF_TCONT_IN_GROUP;
    tx_queue_offset += tx_queue_size_group_0 * CS_NUMBER_OF_TCONT_IN_GROUP;

    /* Group 1 - TCONT 16-23 */
    /* TX FIFO group size(units of 4 bytes ) */
    tx_queue_size_group_1 = CS_TCONT_FRONT_END_SIZE_TCONT_16_23 / 4;

    /* Configure queue driver */
    gpon_error = gpon_tx_set_group_txfifo_configuration(CE_TXPON_FRONT_END_BUFFER_GROUP_ID_1,
        tx_bd_offset,
        tx_queue_offset, /* tx_queue_size_group_1 * CS_NUMBER_OF_TCONT_IN_GROUP * (CE_TXPON_FRONT_END_BUFFER_GROUP_ID_1 + 1) */                                                                                       
        tx_queue_size_group_1);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to cofigure tx fifo group 1 - TCONT 16-23");
        return gpon_error;
    }

    tx_bd_offset += TCONT_PKT_DESCRIPTORS_NUMBER(CS_TCONT_FRONT_END_SIZE_TCONT_16_23) * CS_NUMBER_OF_TCONT_IN_GROUP;
    tx_queue_offset += tx_queue_size_group_1 * CS_NUMBER_OF_TCONT_IN_GROUP;

    /* Group 2 - TCONT 24-31 */
    /* TX FIFO group size(units of 4 bytes ) */
    tx_queue_size_group_2 = CS_TCONT_FRONT_END_SIZE_TCONT_24_31 / 4;

    /* Configure queue driver */
    gpon_error = gpon_tx_set_group_txfifo_configuration(CE_TXPON_FRONT_END_BUFFER_GROUP_ID_2,
        tx_bd_offset,
        tx_queue_offset, /*tx_queue_size_group_2 * CS_NUMBER_OF_TCONT_IN_GROUP * (CE_TXPON_FRONT_END_BUFFER_GROUP_ID_2 + 1) */
        tx_queue_size_group_2);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to cofigure tx fifo group 2 - TCONT 24-31");
        return gpon_error;
    }

    tx_bd_offset += TCONT_PKT_DESCRIPTORS_NUMBER(CS_TCONT_FRONT_END_SIZE_TCONT_24_31) * CS_NUMBER_OF_TCONT_IN_GROUP;
    tx_queue_offset += tx_queue_size_group_2 * CS_NUMBER_OF_TCONT_IN_GROUP;

    /* Group 3 - TCONT 32-39 */
    /* TX FIFO group size(units of 4 bytes ) */
    tx_queue_size_group_3 = CS_TCONT_FRONT_END_SIZE_TCONT_32_39 / 4;

    /* Configure queue driver */
    gpon_error = gpon_tx_set_group_txfifo_configuration(CE_TXPON_FRONT_END_BUFFER_GROUP_ID_3,
        TCONT_PKT_DESCRIPTORS_NUMBER(CS_TCONT_FRONT_END_SIZE_TCONT_32_39) * CS_NUMBER_OF_TCONT_IN_GROUP *(CE_TXPON_FRONT_END_BUFFER_GROUP_ID_3 + 1),
        tx_queue_offset, /*tx_queue_size_group_3 * CS_NUMBER_OF_TCONT_IN_GROUP * (CE_TXPON_FRONT_END_BUFFER_GROUP_ID_3 + 1) */
        tx_queue_size_group_3);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to cofigure tx fifo group 3 - TCONT 32-39");
        return gpon_error;
    }

#ifdef CONFIG_BCM_GPON_TODD
    /* Enable ToD update */
#if defined(CONFIG_BCM96838) || defined(CONFIG_BCM96848)
    drv_error = ag_drv_gpon_rx_amd2_tod_arm_get(&tod_arm, &tod_en);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read AMD2 tod due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_rx_amd2_tod_arm_set(tod_arm, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to enable AMD2 tod due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
#else
    drv_error = ag_drv_gpon_rx_amd2_up_arm_get(&tod_arm, &tod_en, &rdarm);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read AMD2 tod due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_rx_amd2_up_arm_set(tod_arm, BDMF_TRUE, rdarm);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to enable AMD2 tod due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
#endif
#endif 

    /* Disable Tx unit */
    gpon_error = gpon_tx_set_tx_status(BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to Disable Tx");
        return gpon_error;
    }

    /* RX Disable */
    gpon_error = gpon_rx_set_rx_status(BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to disable Rx unit");
        return gpon_error;
    }

    /* Mask TX Interrupts */
    gpon_error = gpon_mask_tx_pon_interrupts();
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Tx interrupt unit due to Driver error %d !", gpon_error);
        return(gpon_error);
    }

    /* Clear ITR */
    gpon_error = gpon_tx_generate_interrupt_test_register(tx_ier_register);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Tx interrupt unit due to Driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Clear TX-ISR */
    gpon_error = gpon_tx_clear_interrupt_status_register();
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear Tx-ISR unit due to Driver error %d.", gpon_error);
        return gpon_error;
    }
   
    /* Mask RX Interrupts */
    gpon_error = gpon_mask_rx_pon_interrupts();
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx interrupt unit due to Driver error %d !", gpon_error);
        return(gpon_error);
    }

    /* Clear RX-ITR */
    drv_error = ag_drv_gpon_rx_irq_grxitr_set((uint16_t)rx_ier_register);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to set Rx interrupts test due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Clear RX-ISR */
    gpon_error = gpon_rx_clear_interrupt_status_reg();
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear Rx-ISR unit");
        return gpon_error;
    }

    /* Configure RX & TX */

    /* Clear Rx ranging alloc id and disable it */
    gpon_error = gpon_rx_set_ranging_alloc_id_configuration(GPON_DEF_ALLOC_ID, BDMF_FALSE);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear Rx Ranging alloc id and its valid bit due to Driver error %d !", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Clear Rx allocation table */
    for (index = 0; index < MAX_NUMBER_OF_TCONTS; index++ )
    {
        gpon_error = gpon_rx_set_alloc_id_cfg(index, GPON_DEF_ALLOC_ID, BDMF_FALSE);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to clear Rx alloc id table and its valid bit index %d.", index);
            return gpon_error;
        }

        gpon_error = gpon_rx_set_alloc_id_to_tcont_association(index, CS_DEFAULT_TCONT_ID);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to clear Rx alloc id table and its tcont id due to Driver error %d index %d.", gpon_error, index);
            return gpon_error;
        }
    }

    /* Clear RX ONU ID */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(0, GPON_RX_ONU_ID_DEF, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx ONU-ID due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Clear Broadcast Id and valid bit */
    drv_error = ag_drv_gpon_rx_ploam_onu_id_set(1, gs_gpon_database.onu_parameters.broadcast_default_value, BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Rx Broadcast ONU-ID due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Clear TX ONU ID */
    drv_error = ag_drv_gpon_tx_general_configuration_onuid_set(GPON_TX_ONU_ID_DEF);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Tx ONU-ID due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Clear Tx - Normal ploam repetition reg */
    drv_error = ag_drv_gpon_tx_gen_ctrl_nprep_set(0, 0, 0 ,0);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear Tx normal ploam repetition due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Read the valid bits */
    drv_error = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get(&normal_valid_bit, &urgent_valid_bit);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read tx valid bit due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Set the urgent ploam valid bit */
    drv_error = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_set(normal_valid_bit, 1);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tx valid bit due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Determine the divide ratio of the DBR */
    gpon_error = f_get_dbr_divide_ratio(gs_gpon_database.dbr_parameters.dba_sr_reporting_block, 
        &dbr_divide_ratio);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to get the DBR divide ratio, error %d ", gpon_error);
        return gpon_error;
    }

    /* Configure DBA SR parameters */
    drv_error = ag_drv_gpon_tx_gen_ctrl_tdbvld_set(gs_gpon_database.dbr_parameters.dbr_valid_part1);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set dbr sr param due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_gen_ctrl_tdbconf_set(gs_gpon_database.dbr_parameters.dbr_valid_part2, 
        gs_gpon_database.dbr_parameters.dbr_sr_mode, dbr_divide_ratio, 
        gs_gpon_database.dbr_parameters.dbr_flush);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set dbr sr param due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* clear normal DBRu HW arrays */
    for (tcont_id = TXPON_FIFO_UNIT_LOW; tcont_id <= TXPON_FIFO_UNIT_HIGH; tcont_id++ )
    {
        gpon_error = gpon_tx_set_normal_dbr_content(tcont_id, GPON_SR_DBR_INVALID);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Error: Unable to clear normal DBRu id %d", tcont_id);
            return gpon_error;
        }
    }

    /* Clear all TX buffers and valid bits */
    /* flush tx normal queue and urgent queue of PLOAM messages. */
    /* It may occure that PLOAM messages are waiting on the queues for TX buffer to be freed. */
    gpon_error = f_gpon_txpon_flush_queue();
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Fail to flush queue");
        return(gpon_error);
    }

    /* Clear Normal ploam repetition normal valid bits */
    drv_error = ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_TRUE);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tx ploam bit due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_FALSE);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tx ploam bit due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Clear TX PM counters */
    gpon_error = gpon_get_tx_pm_counters(&illegal_access_counter,
        &idle_ploam_counter, &normal_ploam_counter, &requested_ranging_ploam_number,
        &requested_urgent_ploam_number);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear Tx unit PM counters due to Api error %d", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Clear Rx PM counters */
    gpon_error = gpon_get_rx_pm_counters(& rxpon_pm_counters);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear Rx unit PM counters due to Api error %d !", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* DS configuration - max treshold */
    drv_error = ag_drv_gpon_rx_congestion_thresh_set(0x3f, 0x2);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Rx congestion thresholds due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Enable Tx unit */
    gpon_error = gpon_tx_set_tx_status(BDMF_TRUE);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to Enable Tx unit driver error %d", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    spec_func_reg_val = GPON_TX_LAST_ALLOC_HANDLING;
    GPON_WRITE_32 ((GPON_TX_SPECIAL_FUNCTIONS_OFFSET), spec_func_reg_val);

    /* Activation state machine control */
    gs_gpon_database.link_parameters.operation_state_machine.link_state = LINK_STATE_INACTIVE;
    gs_gpon_database.link_parameters.operation_state_machine.link_sub_state = LINK_SUB_STATE_STANDBY;
    gs_gpon_database.link_parameters.operation_state_machine.activation_state = OPERATION_STATE_O1;

    gs_gpon_database.link_parameters.serial_number_request_messages_counter = 0;

    return PON_NO_ERROR;
}


static bool f_serial_number_mask_match(uint8_t *xi_serial_number_mask_ptr, uint8_t xi_number_of_valid_bits)
{
    ONUB_STACK_SERIAL_NUMBER_MASK_DTE mask;
    ONUB_STACK_SERIAL_NUMBER_MASK_DTE serial_number;
    uint32_t vendor_bits_mask = 0xffffffff;
    uint32_t serial_bits_mask = 0xffffffff;

    if (xi_number_of_valid_bits < 64)
    {
        if (xi_number_of_valid_bits < 32)
        {
            vendor_bits_mask = 0x0;
            serial_bits_mask = xi_number_of_valid_bits == 0 ? 0x0 : (serial_bits_mask >> (32 - xi_number_of_valid_bits));
        }
        else
        {
            vendor_bits_mask = xi_number_of_valid_bits == 32 ? 0x0 : (vendor_bits_mask >> (32 -(xi_number_of_valid_bits - 32)));
        }
    }

    /* Fill the mask and serial number  */
    serial_number.vendor_code.byte[0] = gs_gpon_database.physical_parameters.serial_number.vendor_code[0];
    serial_number.vendor_code.byte[1] = gs_gpon_database.physical_parameters.serial_number.vendor_code[1];
    serial_number.vendor_code.byte[2] = gs_gpon_database.physical_parameters.serial_number.vendor_code[2];
    serial_number.vendor_code.byte[3] = gs_gpon_database.physical_parameters.serial_number.vendor_code[3];
    serial_number.serial_number.byte[0] = gs_gpon_database.physical_parameters.serial_number.serial_number[0];
    serial_number.serial_number.byte[1] = gs_gpon_database.physical_parameters.serial_number.serial_number[1];
    serial_number.serial_number.byte[2] = gs_gpon_database.physical_parameters.serial_number.serial_number[2];
    serial_number.serial_number.byte[3] = gs_gpon_database.physical_parameters.serial_number.serial_number[3];

    mask.vendor_code.byte[0] = xi_serial_number_mask_ptr[0];
    mask.vendor_code.byte[1] = xi_serial_number_mask_ptr[1];
    mask.vendor_code.byte[2] = xi_serial_number_mask_ptr[2];
    mask.vendor_code.byte[3] = xi_serial_number_mask_ptr[3];
    mask.serial_number.byte[0] = xi_serial_number_mask_ptr[4];
    mask.serial_number.byte[1] = xi_serial_number_mask_ptr[5];
    mask.serial_number.byte[2] = xi_serial_number_mask_ptr[6];
    mask.serial_number.byte[3] = xi_serial_number_mask_ptr[7];


    if (((serial_number.vendor_code.variable & vendor_bits_mask) == (mask.vendor_code.variable & vendor_bits_mask)) &&
        ((serial_number.serial_number.variable & serial_bits_mask) == (mask.serial_number.variable & serial_bits_mask)))
    {
        p_log(ge_onu_logger.sections.stack.debug.general_id, "Serial number match: MATCH!" );
        return(BDMF_TRUE);
    }

    p_log(ge_onu_logger.sections.stack.debug.general_id, "Serial number match: NOT MATCH!" );
    p_log(ge_onu_logger.sections.stack.debug.general_id, "Serial number A vendor ID: %x ", serial_number.vendor_code.variable);
    p_log(ge_onu_logger.sections.stack.debug.general_id, "Serial number A Serial: %x ", serial_number.serial_number.variable);
    p_log(ge_onu_logger.sections.stack.debug.general_id, "Serial number B vendor ID: %x ", mask.vendor_code.variable);
    p_log(ge_onu_logger.sections.stack.debug.general_id, "Serial number B Serial: %x ", mask.serial_number.variable);

    return BDMF_FALSE;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_set_pon_overhead_and_length                                      */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Set Pon overhead and Length                                              */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   The function sets the PON pattern and the PON overhead length during     */
/*   the premable and delimiter transmission.                                 */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_tx_set_pon_overhead_and_length(GPON_DS_US_OVERHEAD_PLOAM
    pon_overhead_ploam, bdmf_boolean extended_flag, int number_of_type_3)
    {
    uint32_t preamble_overhead_bytes; 
    int overall_overhead_bits = 96; /* Rate 1244M overall overhead time bits */
    int delimiter_bits = 24; /* Tx rate delimiter time bits */
    int preamble_type3_bytes_number;
    uint8_t preamble_type1_bytes_number;
    uint8_t preamble_type1_fractional_bits_number;
    uint8_t preamble_type2_bytes_number;
    uint8_t preamble_type2_fractional_bits_number;
    uint32_t delimiter_pattern = 0;
    bool preamble_type1_type2_fractional_bits = BDMF_FALSE;
    int drv_error;

    preamble_type1_bytes_number = pon_overhead_ploam.number_of_type_1_preamble_bits/PON_BITS_PER_BYTE;
    preamble_type1_fractional_bits_number = pon_overhead_ploam.number_of_type_1_preamble_bits%PON_BITS_PER_BYTE;
    preamble_type2_bytes_number = pon_overhead_ploam.number_of_type_2_preamble_bits/PON_BITS_PER_BYTE;
    preamble_type2_fractional_bits_number = pon_overhead_ploam.number_of_type_2_preamble_bits%PON_BITS_PER_BYTE;

    /* TypeI + TypeII can not exceed preamble overhead array */
    if (pon_overhead_ploam.number_of_type_1_preamble_bits + pon_overhead_ploam.number_of_type_2_preamble_bits > 128)  
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Set PON Overhead and length: TypeI + TypeII can not exceed preamble overhead array");
    }

    /* TypeI and TypeII are non integer number of bytes */
    if (preamble_type1_fractional_bits_number > 0 && preamble_type2_fractional_bits_number > 0)
    {
        preamble_type1_type2_fractional_bits = BDMF_TRUE;
    }

    if (!extended_flag) 
    {
        if (pon_overhead_ploam.number_of_guard_bits + pon_overhead_ploam.number_of_type_1_preamble_bits + 
            pon_overhead_ploam.number_of_type_2_preamble_bits + delimiter_bits > overall_overhead_bits) 
        {
            preamble_type3_bytes_number = 0;
        } 
        else
        {
            preamble_type3_bytes_number = (overall_overhead_bits - (pon_overhead_ploam.number_of_guard_bits + 
                pon_overhead_ploam.number_of_type_1_preamble_bits + pon_overhead_ploam.number_of_type_2_preamble_bits + 
                delimiter_bits))/PON_BITS_PER_BYTE;
        }
    }
    else 
        preamble_type3_bytes_number = number_of_type_3;

    /* Setting delimiter content [1] */
    delimiter_pattern = (delimiter_pattern | pon_overhead_ploam.delimiter[2])<<PON_BITS_PER_BYTE;
    /* Setting delimiter content [2] */
    delimiter_pattern = (delimiter_pattern | pon_overhead_ploam.delimiter[1])<<PON_BITS_PER_BYTE;
    /* Setting delimiter content [3] */
    delimiter_pattern = (delimiter_pattern | pon_overhead_ploam.delimiter[0])<<PON_BITS_PER_BYTE;

    if (preamble_type3_bytes_number > 0) 
    {
        /* Pad delimiter LSB with typeIII preamble pattern */
        delimiter_pattern = (delimiter_pattern | ((0xff)&pon_overhead_ploam.pattern_of_type_3_preamble_bits));
        preamble_type3_bytes_number--;
    } 
    else if (preamble_type2_bytes_number > 0) 
    {
        /* Pad delimiter LSB with typeII preamble pattern */
        delimiter_pattern = (delimiter_pattern | GPON_TX_PROD_TYPE_2_PATTERN);
        preamble_type2_bytes_number--;
    } 
    else if (preamble_type1_type2_fractional_bits) 
    {
        /* Pad delimiter LSB with fractional bits of preamble TYPEI + TYPEII */
        delimiter_pattern = (delimiter_pattern | ((0xff)&((GPON_TX_PROD_TYPE_1_PATTERN<<preamble_type2_fractional_bits_number) | 
            (GPON_TX_PROD_TYPE_2_PATTERN>>preamble_type1_fractional_bits_number))));
        preamble_type1_type2_fractional_bits = BDMF_FALSE;
    } 
    else if (preamble_type1_bytes_number > 0) 
    {
        /* Pad delimiter LSB with typeI preamble pattern */
        delimiter_pattern = (delimiter_pattern | GPON_TX_PROD_TYPE_1_PATTERN);
        preamble_type1_bytes_number--;
    }

    /* Set the delimiter */
    drv_error = ag_drv_gpon_tx_general_configuration_delc_set(delimiter_pattern);
    if (drv_error > 0) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver return Error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Set repetition byte content */
    drv_error = ag_drv_gpon_tx_general_configuration_prrb_set(pon_overhead_ploam.pattern_of_type_3_preamble_bits);
    if (drv_error > 0) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver return Error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Allocate TypeI preamble */
    for (preamble_overhead_bytes = 0; preamble_overhead_bytes < preamble_type1_bytes_number; preamble_overhead_bytes++) 
    {
        drv_error = ag_drv_gpon_tx_general_configuration_prod_set(preamble_overhead_bytes, GPON_TX_PROD_TYPE_1_PATTERN);
        if (drv_error > 0) 
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver return Error %d", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }

    /* TYPEI&II are non integer number of bytes */
    if (preamble_type1_type2_fractional_bits) 
    {
        drv_error = ag_drv_gpon_tx_general_configuration_prod_set(preamble_overhead_bytes, 
            ((0xff)&((GPON_TX_PROD_TYPE_1_PATTERN << preamble_type2_fractional_bits_number)|
            (GPON_TX_PROD_TYPE_2_PATTERN >> preamble_type1_fractional_bits_number))));
        if (drv_error > 0) 
    {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver return Error %d", drv_error);
            return PON_ERROR_DRIVER_ERROR;
    }

        preamble_overhead_bytes++;
    }

    /* allocate TYPEII preamble */
    for (;preamble_overhead_bytes < preamble_type1_bytes_number + preamble_type2_bytes_number; preamble_overhead_bytes++) 
    {
        drv_error = ag_drv_gpon_tx_general_configuration_prod_set(preamble_overhead_bytes, GPON_TX_PROD_TYPE_2_PATTERN);
        if (drv_error > 0) 
    {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver return Error %d", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }

    /* Configure Preamble Overhead Specification */
    drv_error = ag_drv_gpon_tx_general_configuration_prods_set(preamble_overhead_bytes, preamble_type3_bytes_number);
    if (drv_error > 0) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver return Error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


static void p_aes_state_machine(AES_EVENTS_DTS xi_event, GPON_DS_PLOAM *xi_ploam_ptr)
{
    p_log(ge_onu_logger.sections.stack.operating.state_change_id,
        "Invoking AES state machine: state %s event %s !",
        gs_aes_state_names[gs_gpon_database.link_parameters.aes_state_machine.aes_state],
        gs_aes_event_names[xi_event]);

    gs_gpon_database.link_parameters.aes_state_machine.aes_message_ptr = xi_ploam_ptr;

    p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id, 
        "Read ploam record data: %02X %02X %02X %02X",
        xi_ploam_ptr->message.key_switching_time.frame_counter[0],
        xi_ploam_ptr->message.key_switching_time.frame_counter[1],
        xi_ploam_ptr->message.key_switching_time.frame_counter[2],
        xi_ploam_ptr->message.key_switching_time.frame_counter[3]);

    gs_aes_state_machine[gs_gpon_database.link_parameters.aes_state_machine.aes_state][xi_event](& gs_gpon_database.link_parameters.aes_state_machine);

}


void p_operation_state_machine(ACTIVATION_EVENTS_DTS xi_event,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params)
{
    LINK_OPERATION_STATES_DTE previous_state;

    /* Save previous state */
    previous_state = gs_gpon_database.link_parameters.operation_state_machine.activation_state;

    /* Log(Only messages that are relavent for ONU ) */
    if (gs_operation_state_machine[previous_state][xi_event]!= p_operation_invalid_cross) 
    {
        p_log(ge_onu_logger.sections.stack.operating.state_change_id,
            "Activation state machine: Invoke: state=%s, event=%s", 
            gs_activation_state_names[previous_state], gs_activation_event_names[xi_event]);
    }

    /* Invoke the state machine */
    gs_operation_state_machine[previous_state][xi_event](
        &gs_gpon_database.link_parameters.operation_state_machine, xi_operation_sm_params);

}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   f_clear_sw_allocation_table                                              */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE f_clear_sw_allocation_table(void)
{
    PON_ERROR_DTE gpon_error;
    uint32_t index;

    /* Clear SW Rx allocation table */
    for (index = 0; index < MAX_NUMBER_OF_TCONTS; index++)
    {

        if (gs_gpon_database.tconts_table[index].assign_alloc_valid_flag == BDMF_TRUE)
        { 
            /* FLUSH tconts HERE before the memory about it will be cleared */
            gs_callbacks.flush_tcont_callback(gs_gpon_database.tconts_table[index].tcont_id);
            gpon_error = gpon_rx_set_alloc_id_cfg(index, GPON_DEF_ALLOC_ID, BDMF_FALSE);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to clear Rx alloc id table and its valid bit due to Driver error %d index %d!", 
                    gpon_error, index);

                return PON_ERROR_DRIVER_ERROR;
            }

            gpon_error = gpon_rx_set_alloc_id_to_tcont_association(index, CS_DEFAULT_TCONT_ID);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to remove associate alloc id index - %d to the default Tcont id", index);
                return gpon_error;
            }

            gs_gpon_database.tconts_table[index].assign_alloc_valid_flag = BDMF_FALSE;

            if (gs_gpon_database.tconts_table[index].config_tcont_valid_flag == BDMF_FALSE)
            {
                gs_gpon_database.tconts_table[index].alloc_id = PON_ALLOC_ID_DTE_NOT_ASSIGNED;
            }
        }
    }

    return PON_NO_ERROR;
}

/*****************************************************************************/
/*                                                                           */
/* Name:                                                                     */
/*                                                                           */
/*   _f_gpon_txpon_send_ploam_message                                        */
/*                                                                           */
/* Title:                                                                    */
/*                                                                           */
/*   GPON - Send PLOAM message from ISR context                              */
/*                                                                           */
/* Abstract:                                                                 */
/*                                                                           */
/*   This function sends a PLOAM message to tx ploam buffer or sw waiting    */
/*  (NOTE: Currently only single device is supported: )                      */
/*                                                                           */
/* Input:                                                                    */
/*                                                                           */
/*   xi_message_ptr - The pointer to the PLOAM message to be sent.           */
/*                                                                           */
/*   xi_ploam_priority - specifies normal or urgent queue                    */
/*                                                                           */
/* Output:                                                                   */
/*                                                                           */
/*   PON_ERROR_DTE - Success or error returned code                          */
/*   PON_NO_ERROR - No errors or faults                                      */
/*   PON_GENERAL_ERROR - General error                                       */
/*                                                                           */
/*****************************************************************************/
static PON_ERROR_DTE _f_gpon_txpon_send_ploam_message(GPON_US_PLOAM *const xi_message_ptr,
    PLOAM_PRIORITY_DTE xi_ploam_priority, GPON_PLOAM_TRANSMISSION_NUMBER xi_ploam_buffer_repetition)
{
    bdmf_boolean urgent_valid_bit;
    uint8_t normal_valid_bit;
    PON_ERROR_DTE gpon_error;
    int drv_error;
    uint32_t i;

#ifdef USE_NORMAL_PLOAM_BUFFER
    /* If urgent message */
    if (xi_ploam_priority == URGENT_PLOAM_CELL)
#endif
    {
        /* Get the valid bit */
        drv_error = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get(
            &normal_valid_bit, &urgent_valid_bit);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to read tx valid bit due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        /* Notify that urgent buffer don't have any repetition capability */
        /* If urgent ring buffer is empty AND urgent TX buffer is empty A
           ND one urgent message need to send than write the ploam to urgent TX buffer */
        if (MS_RING_IS_EMPTY(gs_urgent_ploam_ring) && urgent_valid_bit == 0)
        {
            gpon_error = f_send_urgent_ploam(xi_message_ptr);
            if (gpon_error != PON_NO_ERROR)
            {
                return gpon_error;
            }

            /* Decrease single ploam from the repetiotion number */
            xi_ploam_buffer_repetition--;
        }

        /* Write the rest of ploams to urgent ring buffer according to the repetition */
        for (i = 0; i < xi_ploam_buffer_repetition; i++)
        {
            if (MS_RING_IS_FULL(gs_urgent_ploam_ring))
                break;
            else
            {
                memcpy(MS_RING_WRITE_BUFFER(gs_urgent_ploam_ring).ploam.bytes,
                    (char *)xi_message_ptr, PLOAM_MESSAGE_SIZE);
                MS_RING_WRITE_BUFFER(gs_urgent_ploam_ring).times = GPON_ONE_TRANSMISSION_NUMBER;
                MS_RING_ADVANCE_WRITE(gs_urgent_ploam_ring);
            }
        }
    }
#ifdef USE_NORMAL_PLOAM_BUFFER
    else
    {
        /* All normal ploam arrving to this function are called from
          ISR, and therefore are not protected by disable interrupts */
        drv_error = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get(
            &normal_valid_bit, &urgent_valid_bit);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to read tx valid bit due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        /* If normal ring buffer is empty and normal TX buffer is empty than  and Check if the access */
        /* to txpon buffer is not capture by task level write the ploam to normal TX buffer */
        if (MS_RING_IS_EMPTY(gs_normal_ploam_ring) && (((normal_valid_bit>> 0)&1) == BDMF_FALSE ||
            ((normal_valid_bit>> 1)&1) == BDMF_FALSE || ((normal_valid_bit>> 2)&1) == BDMF_FALSE ||
            ((normal_valid_bit>> 3)&1) == BDMF_FALSE))
        {
            gpon_error = f_send_normal_ploam(xi_message_ptr, xi_ploam_buffer_repetition);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to send Normal ploam %d ", gpon_error);
                return gpon_error;
            }

            p_log(ge_onu_logger.sections.stack.upstream_ploam.general_id, 
                "Set PLOAM in Normal TX buffer. message=%d", xi_message_ptr->message_id);
        }
        else
        {
            /* write the ploam to normal ring buffer */
            memcpy(MS_RING_WRITE_BUFFER(gs_normal_ploam_ring).ploam.bytes, (char *)xi_message_ptr,
                PLOAM_MESSAGE_SIZE);
            MS_RING_WRITE_BUFFER(gs_normal_ploam_ring ).times = xi_ploam_buffer_repetition;
            MS_RING_ADVANCE_WRITE(gs_normal_ploam_ring);

            p_log(ge_onu_logger.sections.stack.upstream_ploam.general_id, 
                "Set PLOAM in Normal waiting queue. message=%d", xi_message_ptr->message_id);
        }
    }
#endif

    return PON_NO_ERROR;
}

/*****************************************************************************/
/*                                                                           */
/* Name:                                                                     */
/*                                                                           */
/*   _f_gpon_txpon_send_base_level_ploam_message                             */
/*                                                                           */
/* Title:                                                                    */
/*                                                                           */
/*   GPON - send Base level message                                          */
/*                                                                           */
/* Abstract:                                                                 */
/*   This function sends a PLOAM message to tx ploam buffer or sw waiting    */
/*  (NOTE: Currently only single device is supported: )                      */
/*                                                                           */
/* Input:                                                                    */
/*                                                                           */
/* Output:                                                                   */
/*                                                                           */
/*   PON_ERROR_DTE - Success or error returned code                          */
/*     PON_NO_ERROR - No errors or faults                                    */
/*     PON_GENERAL_ERROR - General error                                     */
/*                                                                           */
/*****************************************************************************/
static PON_ERROR_DTE _f_gpon_txpon_send_base_level_ploam_message(GPON_US_PLOAM *const xi_message_ptr,
    GPON_PLOAM_TRANSMISSION_NUMBER xi_normal_ploam_buffer_repetition, int use_urgent)
{

    PON_ERROR_DTE gpon_error;
    unsigned long flags;
    PLOAM_PRIORITY_DTE xi_ploam_priority =
#ifdef USE_NORMAL_PLAOM_ONLY_FOR_DYING_GASP
        use_urgent ? URGENT_PLOAM_CELL :
#endif
        NORMAL_PLOAM_CELL;

    /* All base level ploam are sent using the normal ploam, and
      are called from the PON task or User tasks. Therefore they
      are protected by disable interrupts from ISR */
    /* Disable interrupt to prevent race between ISR and API */
    bdmf_fastlock_lock_irq(&gs_fastlock_irq, flags);

    /* Send ploam message */
    gpon_error = _f_gpon_txpon_send_ploam_message(xi_message_ptr, xi_ploam_priority,
        xi_normal_ploam_buffer_repetition);
     /* Enable interrupt */
     bdmf_fastlock_unlock_irq(&gs_fastlock_irq, flags);
     if (gpon_error != PON_NO_ERROR )
     {
         p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
             "Error = %d: unable to send from base level", gpon_error);
         return gpon_error;
     }

     return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   f_gpon_txpon_fetch_ploam_message                                         */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - fetch PLOAM message                                               */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   fetch PLOAM message from the sw waiting queue and set it                 */
/*   in appropriate TX PLOAM buffer                                           */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_ploam_priority - specifies normal or urgent queue                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Success or error returned code                           */
/*     PON_NO_ERROR - No errors or faults                                     */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE f_gpon_txpon_fetch_ploam_message(PLOAM_PRIORITY_DTE xi_ploam_priority)
{
    uint32_t i;
    GPON_US_PLOAM ploam_message;
    PON_ERROR_DTE gpon_error;
    bdmf_boolean urgent_valid_bit;
    uint8_t normal_valid_bit;
    int drv_error;

    /* Check validity of Urgent TX PLOAM buffer */
    drv_error = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get(
        &normal_valid_bit, &urgent_valid_bit);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read tx valid bit due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

#ifdef USE_NORMAL_PLOAM_BUFFER
    drv_error = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get(
        &normal_valid_bit, &urgent_valid_bit);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read tx valid bit due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
#endif

    p_log(ge_onu_logger.sections.stack.upstream_ploam.general_id, 
        "Fetch PLOAM: valid-bit:%d, RING-status:%d", urgent_valid_bit, 
        MS_PLOAM_READY_IN_RING(gs_urgent_ploam_ring));

    /* Is there urgent message waiting and urgent buffer is empty ? */
    if (MS_PLOAM_READY_IN_RING(gs_urgent_ploam_ring) && urgent_valid_bit == BDMF_FALSE)
    {
        /* Update message fields(according to 'no_message' PLOAM type ) */
        ploam_message.onu_id = ((GPON_US_PLOAM *)(&(MS_RING_READ_BUFFER(gs_urgent_ploam_ring).ploam)))->onu_id;
        ploam_message.message_id =((GPON_US_PLOAM *)(&(MS_RING_READ_BUFFER(gs_urgent_ploam_ring).ploam)))->message_id;

        for (i = 0; i < CE_PLOAM_MESSAGE_PAYLOAD_FIELD_SIZE; i++)
        {
            ploam_message.message.no_message.unspecified[i] = 
                ((GPON_US_PLOAM *)(&(MS_RING_READ_BUFFER(gs_urgent_ploam_ring).ploam)))->message.no_message.unspecified[i];
        }

        /* Move urgent message from the queue to TX buffer. */
        gpon_error = f_send_urgent_ploam(&ploam_message);
        if (gpon_error != PON_NO_ERROR )
        {
            return gpon_error;
        }

        MS_RING_ADVANCE_READ(gs_urgent_ploam_ring);
    }

#ifdef USE_NORMAL_PLOAM_BUFFER
    /* Is there normal message waiting */
    if (MS_PLOAM_READY_IN_RING(gs_normal_ploam_ring))
    {
        if (((((normal_valid_bit>> 0)&1) == BDMF_FALSE) || (((normal_valid_bit>> 1)&1) == BDMF_FALSE) ||
            (((normal_valid_bit>> 2)&1) == BDMF_FALSE) || (((normal_valid_bit>> 3)&1) == BDMF_FALSE)))
        {
            /* Update message fields(according to 'no_message' PLOAM type ) */
            ploam_message.onu_id = ((GPON_US_PLOAM *)(&(MS_RING_READ_BUFFER(gs_normal_ploam_ring).ploam)))->onu_id;
            ploam_message.message_id = ((GPON_US_PLOAM *)(&(MS_RING_READ_BUFFER(gs_normal_ploam_ring).ploam)))->message_id;

            for (i = 0; i < CE_PLOAM_MESSAGE_PAYLOAD_FIELD_SIZE; i++)
            {
                ploam_message.message.no_message.unspecified[i] = 
                    ((GPON_US_PLOAM *)(&(MS_RING_READ_BUFFER( gs_normal_ploam_ring ).ploam ) ) )->message.no_message.unspecified[i];
            }

            /* Move normal message from the queue to TX buffer. */
            gpon_error = f_send_normal_ploam(& ploam_message, MS_RING_READ_BUFFER(gs_normal_ploam_ring ).times);
            if (gpon_error != PON_NO_ERROR )
            {
                return gpon_error;
            }

            MS_RING_ADVANCE_READ(gs_normal_ploam_ring);

            p_log(ge_onu_logger.sections.stack.upstream_ploam.general_id, 
                "Fetch PLOAM from Normal waiting queue to Normal TX buffer");
        }
    }
    else
    {
        /* Is there base level message waiting and normal buffer is empty ? */
        if ((MS_PLOAM_READY_IN_RING(gs_base_level_ploam_ring)) &&
            ((((normal_valid_bit>> 0)&1) == BDMF_FALSE) || (((normal_valid_bit>> 1)&1) == BDMF_FALSE) ||
            (((normal_valid_bit>> 2)&1) == BDMF_FALSE) || (((normal_valid_bit>> 3)&1) == BDMF_FALSE)))
        {
            /* Update message fields(according to 'no_message' PLOAM type ) */
            ploam_message.onu_id = ((GPON_US_PLOAM *)(&(MS_RING_READ_BUFFER(gs_base_level_ploam_ring).ploam)))->onu_id;
            ploam_message.message_id = ((GPON_US_PLOAM *)(&(MS_RING_READ_BUFFER( gs_base_level_ploam_ring ).ploam)))->message_id;

            for (i = 0; i < CE_PLOAM_MESSAGE_PAYLOAD_FIELD_SIZE; i++)
            {
                ploam_message.message.no_message.unspecified[i]= 
                    ((GPON_US_PLOAM *)(&(MS_RING_READ_BUFFER(gs_base_level_ploam_ring).ploam)))->message.no_message.unspecified[i];
            }

            /* Move base level message from the queue to TX buffer. */
            gpon_error =  f_send_normal_ploam(& ploam_message, MS_RING_READ_BUFFER(gs_base_level_ploam_ring ).times);
            if (gpon_error != PON_NO_ERROR )
            {
                return gpon_error;
            }

            MS_RING_ADVANCE_READ(gs_base_level_ploam_ring);

            p_log(ge_onu_logger.sections.stack.upstream_ploam.general_id, 
                "Fetch PLOAM from Base level waiting queue to Normal TX buffer");
        }
    }
#endif

    return PON_NO_ERROR;
}


static void p_extended_length(uint32_t xi_number_of_type_3)
{
    PON_ERROR_DTE gpon_error;
    uint32_t preamble_length_bits;
    GPON_DS_US_OVERHEAD_PLOAM upstream_ploam;

    /* Set Upstream Overhead params according to "pre-range" pattern */
    /* Set Type 3 preamble pattern according the possability of receiving Extended burst message */
    /* Setting Upstream Overhead ploam to config the HW */
    upstream_ploam.number_of_guard_bits = gs_gpon_database.onu_parameters.number_of_guard_bits;
    upstream_ploam.number_of_type_1_preamble_bits = gs_gpon_database.onu_parameters.number_of_type_1_preamble_bits;
    upstream_ploam.number_of_type_2_preamble_bits = gs_gpon_database.onu_parameters.number_of_type_2_preamble_bits;
    upstream_ploam.pattern_of_type_3_preamble_bits = gs_gpon_database.onu_parameters.pattern_of_type_3_preamble_bits;
    memcpy(upstream_ploam.delimiter, gs_gpon_database.onu_parameters.delimiter, GPON_DELIMITER_SIZE);
    upstream_ploam.options.pre_equalization_status = gs_gpon_database.onu_parameters.pre_equalization_status ==   
        CE_PRE_EQUALIZATION_DELAY_DONT_USE ? BDMF_FALSE : BDMF_TRUE;
    upstream_ploam.options.serial_number_mask_status = 
        gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_status;
    upstream_ploam.options.extra_serial_number_transmissions = 
        gs_gpon_database.onu_parameters.extra_serial_number_transmissions;
    upstream_ploam.options.power_level_mode = gs_gpon_database.onu_parameters.default_power_level_mode;

    /* Get preassigned equalization delay from the database */
    upstream_ploam.preassigned_equalization_delay[0] =
        (( gs_gpon_database.onu_parameters.preassigned_equalization_delay >> 8 ) & 0x000000FF);
    upstream_ploam.preassigned_equalization_delay[1] =
        (gs_gpon_database.onu_parameters.preassigned_equalization_delay & 0x000000FF);

    /* Check that T1 + T2 + GT <= 16 bytes and modulo test - else error */
    preamble_length_bits = upstream_ploam.number_of_type_1_preamble_bits +
       upstream_ploam.number_of_type_2_preamble_bits + upstream_ploam.number_of_guard_bits;

    if (preamble_length_bits > (PON_TX_PREAMBLE_OVERHEAD_SIZE_IN_BYTES << 3 /* Multiple by 8 */) ||
        (preamble_length_bits % 8 ) != 0 )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Stack Error: The sum of T1 & T2 & GT overflow preamble length OR failed in, "
            "modulo 8 criteria : T1 %d [bits], T2 %d [bits], GT %d [bits]",
            upstream_ploam.number_of_type_1_preamble_bits, upstream_ploam.number_of_type_2_preamble_bits,
            upstream_ploam.number_of_guard_bits);
    }

    /* HW - Set Pon upstream overhead - preamble & delimiter */
    gpon_error = gpon_tx_set_pon_overhead_and_length(upstream_ploam, BDMF_TRUE,
        xi_number_of_type_3);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d !", gpon_error);
    }

    p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
        "Set number of type 3  %d", xi_number_of_type_3);
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   set_alloc_id_configuration                                             */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/* Input:                                                                     */
/*   xi_ploam_ptr - alloc id configuration struct                             */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/******************************************************************************/
static void set_alloc_id_configuration(PON_ASSIGN_ALLOC_ID_INDICATION assign_alloc_id_struct)
{
    uint32_t table_index = 0;
    uint32_t alloc_index = 0;
    uint32_t empty_index = 0;
    PON_ERROR_DTE gpon_error;
    bool alloc_id_found = BDMF_FALSE;

    /* Check range of alloc-id */
    if (!PON_ALLOC_ID_DTE_IN_RANGE(assign_alloc_id_struct.alloc_id))
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Alloc id out of range: %d !", assign_alloc_id_struct.alloc_id);
        return;
    }

   /* Disable interrupt to prevent race between ISR and API */
    bdmf_fastlock_lock(& gs_fastlock);
    /* Search for this alloc id in the alloc table */
    for (table_index = 0; table_index < MAX_NUMBER_OF_TCONTS; table_index++)
    {
        if (gs_gpon_database.tconts_table[table_index].alloc_id == assign_alloc_id_struct.alloc_id)
        {
            alloc_id_found = BDMF_TRUE;
            alloc_index = table_index;
            p_log(ge_onu_logger.sections.stack.debug.general_id, 
                "found alloc %d in table index = %d\n", assign_alloc_id_struct.alloc_id ,table_index);
            break;
        }
    }

    /* Assign */
    if (assign_alloc_id_struct.assign_flag == BDMF_TRUE)
    {
        /* if the alloc id already apper in the table */
        if (alloc_id_found == BDMF_TRUE)
        {
            /* if this alloc already assign by the OLT */
            if (gs_gpon_database.tconts_table[alloc_index].assign_alloc_valid_flag == BDMF_TRUE)
            {
                p_log(ge_onu_logger.sections.stack.debug.general_id,
                    "Alloc-ID %d already assigned\n", gs_gpon_database.tconts_table[alloc_index].alloc_id);
                bdmf_fastlock_unlock(&gs_fastlock);
                return;
            }
            else
            {
                /* if tcont already associate to this alloc, update the HW table */
                if (gs_gpon_database.tconts_table[alloc_index].config_tcont_valid_flag == BDMF_TRUE)
                {
                    p_log(ge_onu_logger.sections.stack.debug.general_id, 
                        "Write and enable alloc id %d in HW table index %d \n", 
                        assign_alloc_id_struct.alloc_id, alloc_index);
                    /* write and enable this alloc in tne  HW table */
                    gpon_error = gpon_rx_set_alloc_id_cfg(alloc_index, assign_alloc_id_struct.alloc_id, 
                        BDMF_TRUE);
                    if (gpon_error != PON_NO_ERROR)
                    {
                        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                            "Unable to write alloc id- %d in table and its valid bit due to Driver error !", 
                            assign_alloc_id_struct.alloc_id);
                        bdmf_fastlock_unlock(&gs_fastlock);
                        return;
                    }

                    p_log(ge_onu_logger.sections.stack.debug.general_id, 
                        "associate alloc id in index %d to Tcont %d \n", alloc_index, 
                        gs_gpon_database.tconts_table[alloc_index].tcont_id );

                    /* change the tcont id in the HW table to Default Tcont (0) */
                    gpon_error = gpon_rx_set_alloc_id_to_tcont_association(alloc_index, 
                        gs_gpon_database.tconts_table[alloc_index].tcont_id);
                    if (gpon_error != PON_NO_ERROR)
                    {
                       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                           "Unable to set tcont id %d to alloc index %d.", 
                           gs_gpon_database.tconts_table[alloc_index].tcont_id, alloc_index);
                        return;
                    }

                    gs_gpon_database.tconts_table[alloc_index].assign_alloc_valid_flag = BDMF_TRUE;

                }
                /* error  - the alloc apper in the table but not associate to any tcont */
                else
                {
                    bdmf_fastlock_unlock(& gs_fastlock);
                    return;
                }
            }
        }

        /* The alloc does not apper in the table */
        else
        {
            /* Find empty entry in the tcont table */
            for (empty_index = 1; empty_index < MAX_NUMBER_OF_TCONTS; empty_index++)
            {
                if (gs_gpon_database.tconts_table[empty_index].assign_alloc_valid_flag == BDMF_FALSE &&
                    gs_gpon_database.tconts_table[empty_index].config_tcont_valid_flag == BDMF_FALSE) 
                {
                    break;
                }
            }

            p_log(ge_onu_logger.sections.stack.debug.general_id, 
                "Write and enable alloc id %d in table index %d \n", 
                assign_alloc_id_struct.alloc_id, empty_index);
            /* write and enable this alloc in tne  HW table */
            gpon_error = gpon_rx_set_alloc_id_cfg(empty_index, assign_alloc_id_struct.alloc_id,
                BDMF_TRUE);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to write alloc id- %d in table and its valid bit due to Driver error !", 
                    assign_alloc_id_struct.alloc_id);
                bdmf_fastlock_unlock(&gs_fastlock);
                return;
            }

            gpon_error = gpon_rx_set_alloc_id_to_tcont_association(empty_index, CS_DEFAULT_TCONT_ID);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to associate alloc id- %d to the default Tcont id", assign_alloc_id_struct.alloc_id);
                bdmf_fastlock_unlock(&gs_fastlock);
                return;
            }

            gs_gpon_database.tconts_table[empty_index].alloc_id = assign_alloc_id_struct.alloc_id;
            gs_gpon_database.tconts_table[empty_index].assign_alloc_valid_flag = BDMF_TRUE;
            gs_gpon_database.tconts_table[empty_index].tcont_id = CS_DEFAULT_TCONT_ID;
            gs_gpon_database.tconts_table[empty_index].config_tcont_valid_flag = BDMF_FALSE;
        }
    }
    /* Deassign */
    else
    {
        /* if the alloc id apper in the table */
        if (alloc_id_found == BDMF_TRUE && 
            gs_gpon_database.tconts_table[alloc_index].assign_alloc_valid_flag == BDMF_TRUE) 
        {
            /* remove the alloc from the HW table and disable it */
            gpon_error = gpon_rx_set_alloc_id_cfg(alloc_index, PON_ALLOC_ID_DTE_NOT_ASSIGNED,
                BDMF_FALSE);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to write alloc id- %d in table and its valid bit due to Driver error !", 
                    gs_gpon_database.tconts_table[alloc_index].alloc_id);
                bdmf_fastlock_unlock(&gs_fastlock);
                return;
            }

            /* if there is tcont that configure to this alloc dont remove the alloc just update his flag */
            if (gs_gpon_database.tconts_table[alloc_index].config_tcont_valid_flag == BDMF_FALSE)
            {
                gs_gpon_database.tconts_table[alloc_index].alloc_id = PON_ALLOC_ID_DTE_NOT_ASSIGNED;
            }
            gs_gpon_database.tconts_table[alloc_index].assign_alloc_valid_flag = BDMF_FALSE;
        }
        /* if the alloc not assign yet */
        else
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to deallocate alloc id- %d , the alloc was not assign yet!", gs_gpon_database.tconts_table[alloc_index].alloc_id);
            bdmf_fastlock_unlock(&gs_fastlock);
            return;
        }
    }

     /* Enable interrupt */
    bdmf_fastlock_unlock(&gs_fastlock);
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_set_sn_ranging_ploam_content                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Set Ranging ploam content                                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function set the SN/Ranging ploam content                           */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_tx_set_sn_ranging_ploam_content(GPON_TX_PLOAM_MESSAGE *ranging_ploam_cont)
{
    int i;
    int drv_error;
    uint8_t rplmc;

    for (i = 0; i <= PLOAM_MESSAGE_SIZE; i++) 
    {
        if (i < PLOAM_MESSAGE_SIZE) 
            rplmc = ((uint8_t *)(ranging_ploam_cont))[i];
        else
            rplmc = gpon_tx_utils_calculate_crc((uint8_t *)(ranging_ploam_cont), PLOAM_MESSAGE_SIZE);

        drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_rplm_set(i, rplmc);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Failed to set SN ranging ploam content due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
        }

    /* Enable valid bit */
    gpon_set_ranging_valid_bit(BDMF_TRUE);

    p_log(ge_onu_logger.sections.stack.upstream_ploam.print_ploam, 
        "Write Ranging or SN Ploam: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
        ((uint8_t *)(ranging_ploam_cont))[0], ((uint8_t *)(ranging_ploam_cont))[1],
        ((uint8_t *)(ranging_ploam_cont))[2], ((uint8_t *)(ranging_ploam_cont))[3],
        ((uint8_t *)(ranging_ploam_cont))[4], ((uint8_t *)(ranging_ploam_cont))[5],
        ((uint8_t *)(ranging_ploam_cont))[6], ((uint8_t *)(ranging_ploam_cont))[7],
        ((uint8_t *)(ranging_ploam_cont))[8], ((uint8_t *)(ranging_ploam_cont))[9],
        ((uint8_t *)(ranging_ploam_cont))[10], ((uint8_t *)(ranging_ploam_cont))[11],
        ((uint8_t *)(ranging_ploam_cont))[12]);                 

    return PON_NO_ERROR;
    }

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_get_sn_ranging_ploam_content                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Get Ranging ploam content                                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function get the SN/Ranging ploam content                           */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_tx_get_sn_ranging_ploam_content(GPON_TX_PLOAM_MESSAGE *ranging_ploam_cont)
{
    int i;
    int drv_error;

    for (i = 0; i <= PLOAM_MESSAGE_SIZE; i++) 
    {
        drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_rplm_get(i, &(((uint8_t *)(ranging_ploam_cont))[i]));
        if (drv_error > 0)
    {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Failed to set SN ranging ploam content due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }

    return PON_NO_ERROR;
}

         
/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_set_ranging_valid_bit                                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx -  Set Ranging ploam valid                                       */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function set the ranging valid bit .                                */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_set_ranging_valid_bit(bdmf_boolean ranging_valid_bit)
{
    int drv_error;
    gpon_tx_gen_ctrl_ten ten;

    drv_error = ag_drv_gpon_tx_gen_ctrl_ten_get(&ten);
    if (drv_error > 0)
	{
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Cannot read transmit enable register due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
	}

    ten.rngen = ranging_valid_bit;

    drv_error = ag_drv_gpon_tx_gen_ctrl_ten_set(&ten);
    if (drv_error > 0)
	{
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Cannot set ranging valid bit due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
	}

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_get_idle_ploam_content                                           */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx -  Get Idle ploam content                                        */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function get the Idle ploam content                                 */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_tx_get_idle_ploam_content(GPON_TX_PLOAM_MESSAGE *idle_ploam_cont)
{
    int i;
    int drv_error;
   
    for (i = 0; i <= PLOAM_MESSAGE_SIZE; i++) 
    {
        drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_iplm_get(i, &(((uint8_t *)(idle_ploam_cont))[i]));
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Failed to set SN ranging ploam content due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }

    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_set_idle_ploam_content                                           */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx -  Set Idle ploam content                                        */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function set the Idle ploam content                                 */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_tx_set_idle_ploam_content(GPON_TX_PLOAM_MESSAGE *idle_ploam_cont)
{
    int i;
    int drv_error;
    uint8_t iplc;
  
    for (i = 0; i <= PLOAM_MESSAGE_SIZE; i++) 
{
        if (i < PLOAM_MESSAGE_SIZE) 
            iplc = ((uint8_t *)(idle_ploam_cont))[i];
        else
            iplc = gpon_tx_utils_calculate_crc((uint8_t *)(idle_ploam_cont), PLOAM_MESSAGE_SIZE);

        drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_iplm_set(i, iplc);
        if (drv_error > 0)
    {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Failed to set SN ranging ploam content due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }
  
    p_log(ge_onu_logger.sections.stack.upstream_ploam.print_ploam, 
        "Write Idle Ploam: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
        ((uint8_t *)(idle_ploam_cont))[0], ((uint8_t *)(idle_ploam_cont))[1],
        ((uint8_t *)(idle_ploam_cont))[2], ((uint8_t *)(idle_ploam_cont))[3],
        ((uint8_t *)(idle_ploam_cont))[4], ((uint8_t *)(idle_ploam_cont))[5],
        ((uint8_t *)(idle_ploam_cont))[6], ((uint8_t *)(idle_ploam_cont))[7],
        ((uint8_t *)(idle_ploam_cont))[8], ((uint8_t *)(idle_ploam_cont))[9],
        ((uint8_t *)(idle_ploam_cont))[10], ((uint8_t *)(idle_ploam_cont))[11],
        ((uint8_t *)(idle_ploam_cont))[12]);                 

    return PON_NO_ERROR;
    }


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_set_tx_ploam_bit                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Set Tx ploam valid bit                                         */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function set the TX PLOAM clear valid bit.                          */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_tx_set_tx_ploam_bit(bdmf_boolean txploam_clear_bit)
{
    int drv_error;

#ifdef USE_NORMAL_PLOAM_BUFFER
    if (txploam_clear_bit)
        gs_normal_ploam_valid_number = 0; 
#endif

    drv_error = ag_drv_gpon_tx_gen_ctrl_tpclr_set(txploam_clear_bit);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Cannot set Tx ploam bit due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}           

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_rx_set_ranging_alloc_id_configuration                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Rx - Set Ranging Alloc Id configuration.                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   The function sets the Ranging alloc-id and the enable alloc-id bit value.*/
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_rx_set_ranging_alloc_id_configuration(uint16_t ranging_alloc_id,
    bdmf_boolean enable_ranging_alloc_id) 
{
    int drv_error;

    if (ranging_alloc_id >= GPON_RX_RANGING_ALLOC_ID_MAX) 
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    if (enable_ranging_alloc_id)
    {
        drv_error = ag_drv_gpon_rx_alloc_id_rang_alc_id_set(ranging_alloc_id);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to configure ranging alloc-ID due to driver error %d", drv_error);
            return PON_ERROR_DRIVER_ERROR;
}

        drv_error = ag_drv_gpon_rx_alloc_id_rang_alc_id_en_set(BDMF_TRUE);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to enable ranging alloc-ID due to driver error %d", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }
    else
    {
        drv_error = ag_drv_gpon_rx_alloc_id_rang_alc_id_en_set(BDMF_FALSE);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to disable ranging alloc-ID due to driver error %d", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        drv_error = ag_drv_gpon_rx_alloc_id_rang_alc_id_set(ranging_alloc_id);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to configure ranging alloc-ID due to driver error %d", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }

    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_set_group_txfifo_configuration                                   */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Set TxFIFO group configuration                                 */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function set the TxFIFO group configuration                         */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_tx_set_group_txfifo_configuration(uint32_t xi_group_id,
    uint16_t xi_packet_descriptor_group_base_address, uint16_t xi_group_offset, 
    uint16_t xi_group_size) 
{
    int drv_error;

    drv_error = ag_drv_gpon_tx_fifo_configuration_pdpgrp_set(
        xi_group_id, xi_packet_descriptor_group_base_address);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set group tx fifo configuration due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_fifo_configuration_tdqpgrp_set(xi_group_id, 
         xi_group_offset, xi_group_size);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set group tx fifo configuration due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;    
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_set_tx_status                                                    */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Set Tx status                                                  */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function configures the Tx mode.                                    */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_tx_set_tx_status(bdmf_boolean tx_status)
{
    gpon_tx_gen_ctrl_ten tx_params; 
    int drv_error;
    bdmf_boolean set, rdy;
    int i;

    drv_error = ag_drv_gpon_tx_gen_ctrl_ten_get(&tx_params);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read tx paramters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    tx_params.txen = tx_status;

    drv_error = ag_drv_gpon_tx_gen_ctrl_ten_set(&tx_params);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure tx paramters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error =  ag_drv_gpon_tx_general_configuration_rdyind_get(&set, &rdy);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read tx buffer indication bit due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error =  ag_drv_gpon_tx_general_configuration_rdyind_set(BDMF_TRUE, rdy);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tx buffer indication bit due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    for (i=0; i <= 10000; i++) 
    {
        ag_drv_gpon_tx_general_configuration_rdyind_get(&set, &rdy);
        if (rdy)
            break;
    }

    if (i == 10000)
        return PON_ERROR_TIME_OUT_FLUSH_IS_NOT_DONE;

    drv_error = ag_drv_gpon_tx_general_configuration_rdyind_set(BDMF_FALSE, rdy);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set tx buffer indication bit due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}           

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_rx_set_rx_status                                                    */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Set RX Status                                                     */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function sets the Rx status                                         */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_rx_set_rx_status(bdmf_boolean rx_enable)
{
    int drv_error;
    gpon_rx_general_config_rcvr_config rcvr_config;

    drv_error = ag_drv_gpon_rx_general_config_rcvr_config_get(&rcvr_config);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read Rx paramters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    rcvr_config.rx_disable = ~rx_enable;

    drv_error = ag_drv_gpon_rx_general_config_rcvr_config_set(&gs_gpon_database.rx_parameters.rcvr_config);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Rx paramters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    gs_gpon_database.rx_parameters.rcvr_config.rx_disable = ~rx_enable;

    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   f_gpon_txpon_flush_queue                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - flush queue                                                       */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   Flush all messages from the queue and release buffer to correspondent    */
/*   Ring Buffer                                                              */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Success or error returned code                           */
/*     PON_NO_ERROR - No errors or faults                                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE f_gpon_txpon_flush_queue(void)
{
    /* Flush all the Ring Buffers */
    MS_RING_FLUSH_BUFFER(gs_urgent_ploam_ring);
#ifdef USE_NORMAL_PLOAM_BUFFER
    MS_RING_FLUSH_BUFFER(gs_normal_ploam_ring);
    MS_RING_FLUSH_BUFFER(gs_base_level_ploam_ring);
#endif

    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_set_pls_status                                                    */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Set PLS status                                                  */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function configures the Tx mode.                                    */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_tx_set_pls_status(bdmf_boolean pls)
{
    gpon_tx_gen_ctrl_ten tx_params; 
    int drv_error;

    drv_error = ag_drv_gpon_tx_gen_ctrl_ten_get(&tx_params);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read tx paramters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    tx_params.plsen = pls;
   
    drv_error = ag_drv_gpon_tx_gen_ctrl_ten_set(&tx_params);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure tx paramters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_set_normal_dbr_content                                           */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Set Normal DBR content                                         */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function configure the normal dbr content                           */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_tx_set_normal_dbr_content(uint8_t ndbr_buff_num,
    uint8_t ndbr_content) 
{
    uint8_t ndbr_content_crc;
    PON_ERROR_DTE gpon_error;
    int drv_error;

    /* Set Idle report. It will be sent if request arrives when Normal report is updated */
    ndbr_content_crc = gpon_tx_utils_calculate_crc(&ndbr_content, 1);

    drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_idbr_set(ndbr_content | ndbr_content_crc<<8);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable configure the normal dbr content due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Clear DBA SR valid bit */
    gpon_error = gpon_tx_set_dba_sr_valid(ndbr_buff_num, 0);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Failed to set DBA SR valid bit");
        return gpon_error;
    }

    /* Set normal content */
    if (ndbr_buff_num < 8)
    {
         switch (ndbr_buff_num)
         {
         case 0:        
            drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_set(ndbr_content);             
            break;
         case 1:        
            drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_set(ndbr_content);             
            break;
         case 2:        
            drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_set(ndbr_content);             
            break;
         case 3:        
            drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_set(ndbr_content);             
            break;
         case 4:        
            drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_set(ndbr_content);             
            break;
         case 5:        
            drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_set(ndbr_content);             
            break;
         case 6:        
            drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_set(ndbr_content);             
            break;
        case 7:        
            drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_set(ndbr_content);             
        break;
        default:
             return PON_ERROR_INVALID_PARAMETER;
        }
    }
    else
    {
        drv_error = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_set(ndbr_buff_num - 8, ndbr_content);
    }

    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable configure the normal dbr content due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Set DBA SR valid bit */
    gpon_error = gpon_tx_set_dba_sr_valid(ndbr_buff_num, 1);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Failed to set DBA SR valid bit");
        return gpon_error;
    }

    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_set_dba_sr_valid                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Set DBA SR valid                                               */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function enable the DBA SR.                                         */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_tx_set_dba_sr_valid(uint8_t access_number, bdmf_boolean dba_sr_valid_bit)
{
    uint32_t dba_sr_valid_0_to_31;
    uint8_t dbav; 
    bdmf_boolean dbr_gen_en;
    uint8_t divrate;
    bdmf_boolean dbflush;
    int drv_error;

    if (access_number < 32)
    {
        drv_error = ag_drv_gpon_tx_gen_ctrl_tdbvld_get(&dba_sr_valid_0_to_31);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to read dbr sr param due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        if (dba_sr_valid_bit == 1)
        {
            dba_sr_valid_0_to_31 = dba_sr_valid_0_to_31 | (1 << access_number);
        }
        else
        {
            dba_sr_valid_0_to_31 = dba_sr_valid_0_to_31 & (~(1 << access_number));
        }

        drv_error = ag_drv_gpon_tx_gen_ctrl_tdbvld_set(dba_sr_valid_0_to_31);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to set dbr sr param due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }
    else if (access_number > 31 && access_number < 40)
    {
        drv_error = ag_drv_gpon_tx_gen_ctrl_tdbconf_get(&dbav, &dbr_gen_en, &divrate, &dbflush);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to read dbr sr param due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        if (dba_sr_valid_bit == 1)
        {
            dbav = dbav | (1<<(access_number - 32));
        }
        else
        {
            dbav = dbav & (~((1<<(access_number - 32))));
        }

        drv_error = ag_drv_gpon_tx_gen_ctrl_tdbconf_set(dbav, dbr_gen_en, divrate, dbflush);
        if (drv_error > 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to set dbr sr param due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }
    
    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_send_vendor_specific_message                                        */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Send vendor specific message                                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function send 'vendor-specific' message to the OLT.                 */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_message_type - Vendor specific message type (0..7)                    */
/*                                                                            */
/*   xi_message_ptr - The message payload(byte 4-13 as defined in             */
/*                    ITU-T/G983.1 spec , table 12)                           */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     GPON_ALIGNMENT_ERROR - Parameter alignment error from the stack        */
/*     PON_ERROR_INVALID_PARAMETER - Invalid parameter input                  */
/*     PON_GENERAL_ERROR - General error from the stack level                 */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_send_vendor_specific_message(GPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE xi_message_type,
    GPON_VENDOR_SPECIFIC_MSG_DTE *const xi_message_ptr)
{
    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_send_dying_gasp_message                                             */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - send dying gasp message                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function send Dying gasp PLOAM message 3 times.                     */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_send_dying_gasp_message(void)
{
    uint32_t i;
    PON_ERROR_DTE gpon_error;
    GPON_US_PLOAM ploam_message;
  
    /* Clear any pending ploams */
    ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_TRUE);
    ag_drv_gpon_tx_gen_ctrl_tpclr_set(BDMF_FALSE);
  
    ploam_message.onu_id = gs_gpon_database.onu_parameters.onu_id;
    ploam_message.message_id = CE_US_DYING_GASP_MESSAGE_ID;
  
    for (i = 0; i < CE_PLOAM_MESSAGE_PAYLOAD_FIELD_SIZE; i++)
    {
        ploam_message.message.dying_gasp.unspecified[i] = 0;
    }
  
    /* Send Dying gasp ploam buffer - 1 time */
    gpon_error = _f_gpon_txpon_send_base_level_ploam_message(&ploam_message, 
        GPON_DYING_GASP_TRANSMISSION_NUMBER, 0);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Error: unable to send Dying gasp message!" ) ;
        return gpon_error;
    }
  
    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_send_pee_message                                                    */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Send pee message                                                   */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function send single PEE message                                    */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_send_pee_message(void)
{
    uint32_t i;
    PON_ERROR_DTE gpon_error;
    GPON_US_PLOAM ploam_message;
    
    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }
    
    if (gs_gpon_database.link_parameters.operation_state_machine.activation_state != OPERATION_STATE_O5)
    {
         p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
             "Error , Link is'nt Operational !" );
         return PON_ERROR_LINK_NOT_ACTIVE;
    }
    
    ploam_message.onu_id = gs_gpon_database.onu_parameters.onu_id;
    ploam_message.message_id = CE_US_PHYSICAL_EQUIPMENT_ERROR_MESSAGE_ID;
    
    for (i = 0; i < US_PLOAM_PEE_UNSPECIFIED_FIELD_SIZE; i++)
    {
        ploam_message.message.physical_equipment_error.unspecified[i] = 0;
    }
    
    /* Send PEE via ploam buffer - 1 time */
    gpon_error = f_gpon_txpon_send_base_level_ploam_message( 
        &ploam_message, GPON_PEE_ALARM_TRANSMISSION_NUMBER);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Error: unable to send PEE message!");
        return gpon_error;
    }
    
    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_send_pst_message                                                    */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Send pst message                                                   */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function send single PST message                                    */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_send_pst_message(PON_PST_INDICATION_PARAMETERS_DTE xi_pst_configuration_struct)
{
    PON_ERROR_DTE gpon_error;
    GPON_US_PLOAM ploam_message;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    if (gs_gpon_database.link_parameters.operation_state_machine.activation_state != OPERATION_STATE_O5)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Error , Link is'nt Operational !");
        return PON_ERROR_LINK_NOT_ACTIVE;
    }

    ploam_message.onu_id = gs_gpon_database.onu_parameters.onu_id;
    ploam_message.message_id = CE_US_PST_MESSAGE_ID;
    ploam_message.message.pst.k1 = xi_pst_configuration_struct.k1;
    ploam_message.message.pst.k2 = xi_pst_configuration_struct.k2;
    ploam_message.message.pst.line_number = xi_pst_configuration_struct.line_number;

    memset(&ploam_message.message.pst.unspecified, 0, 
        US_PLOAM_PEE_UNSPECIFIED_FIELD_SIZE);

    /* Send PST via ploam buffer - 1 time */
    gpon_error = f_gpon_txpon_send_base_level_ploam_message(
        &ploam_message, GPON_PST_ALARM_TRANSMISSION_NUMBER);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Error: unable to send PST message!");
        return gpon_error;
    }

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_send_ploam_message                                                  */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Send ploam message                                                 */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function send ploam message                                         */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_send_ploam_message(GPON_US_PLOAM *xi_ploam_message,
    uint8_t xi_message_id, uint8_t xi_repetition, uint8_t xi_priority)
{
    PON_ERROR_DTE gpon_error;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    if (gs_gpon_database.link_parameters.operation_state_machine.activation_state != OPERATION_STATE_O5)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Error , Link is'nt Operational !");
        return PON_ERROR_LINK_NOT_ACTIVE;
    }

    xi_ploam_message->onu_id = gs_gpon_database.onu_parameters.onu_id;
    xi_ploam_message->message_id = xi_message_id;

    /* Send ploam buffer */
    gpon_error = _f_gpon_txpon_send_ploam_message(xi_ploam_message,
        xi_priority, xi_repetition);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Error: unable to send Dying gasp message!");
        return gpon_error;
    }

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   f_gpon_txpon_send_base_level_ploam_message                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Send Base Leve PLOAM                                                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE f_gpon_txpon_send_base_level_ploam_message(GPON_US_PLOAM *const xi_message_ptr,
    GPON_PLOAM_TRANSMISSION_NUMBER xi_normal_ploam_buffer_repetition)
{
    return _f_gpon_txpon_send_base_level_ploam_message(xi_message_ptr, xi_normal_ploam_buffer_repetition, 1);
}

/*****************************************************************************/
/*                                                                           */
/* Name:                                                                     */
/*                                                                           */
/*   gpon_tx_utils_generate_crc_table                                        */
/*                                                                           */
/* Title:                                                                    */
/*                                                                           */
/*   GPON Utils - generate CRC table according the ATM HEC polynomial        */
/*                                                                           */
/* Abstract:                                                                 */
/*                                                                           */
/*   This function generates a 256 entries table (bytes) for the CRC         */
/* calculation.                                                              */
/*                                                                           */
/*****************************************************************************/
static void gpon_tx_utils_generate_crc_table(void)
{
#define POLYNOMIAL 0x00000107 
    uint32_t i;
    uint32_t j;
    uint8_t crc_accum;

    for (i = 0; i < 256; i ++) 
    {
        crc_accum = i;
        for (j = 0; j < 8; j ++) 
        {
            if (crc_accum & 0x80) 
                crc_accum = (crc_accum << 1)^POLYNOMIAL;
            else 
                crc_accum = (crc_accum << 1);
        }
        gs_crc_table[i] = crc_accum;
    }

    return;
#undef POLYNOMIAL
}

/*****************************************************************************/
/*                                                                           */
/* Name:                                                                     */
/*                                                                           */
/*   gpon_tx_utils_calculate_crc                                             */
/*                                                                           */
/* Title:                                                                    */
/*                                                                           */
/*   Drivers Utils - calculate HEC of a message                              */
/*                                                                           */
/* Abstract:                                                                 */
/*                                                                           */
/*   This function generates the HEC of message by polynomial division       */
/*                                                                           */
/*****************************************************************************/
static uint8_t gpon_tx_utils_calculate_crc(uint8_t *buff_ptr, uint8_t buff_length)
{
    uint8_t crc = 0;
    uint8_t *p;

    /* First the CRC table should be initialized. */
    if (!gs_crc_table[1]) 
        gpon_tx_utils_generate_crc_table();

    for (p = buff_ptr; buff_length > 0; ++p, --buff_length) 
    {
        crc = gs_crc_table[crc^(*p)];
    }

    return crc;
}


static bool f_serial_number_match(PON_SERIAL_NUMBER_DTE xi_serial_number_1,
    PON_SERIAL_NUMBER_DTE xi_serial_number_2)
{
    uint32_t i,j;

    /* Check serial number */
    i = memcmp((char *)xi_serial_number_1.serial_number,
        (char *)xi_serial_number_2.serial_number, VENDOR_SPECIFIC_FIELD_SIZE);

    j = memcmp((char *)xi_serial_number_1.vendor_code,
        (char *)xi_serial_number_2.vendor_code, VENDOR_CODE_FIELD_SIZE);

    if ((i != 0) || (j != 0))
    {
         p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id, 
             "Serial number match: NOT MATCH!");
         p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id, 
             "Serial number A %02X %02X %02X %02X %02X %02X %02X %02X ",
             xi_serial_number_1.vendor_code[0], xi_serial_number_1.vendor_code[1],
             xi_serial_number_1.vendor_code[2], xi_serial_number_1.vendor_code[3],
             xi_serial_number_1.serial_number[0], xi_serial_number_1.serial_number[1],
             xi_serial_number_1.serial_number[2], xi_serial_number_1.serial_number[3]);
         p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id, 
             "Serial number B %02X %02X %02X %02X %02X %02X %02X %02X ",
             xi_serial_number_2.vendor_code[0], xi_serial_number_2.vendor_code[1],
             xi_serial_number_2.vendor_code[2], xi_serial_number_2.vendor_code[3],
             xi_serial_number_2.serial_number[0], xi_serial_number_2.serial_number[1],
             xi_serial_number_2.serial_number[2], xi_serial_number_2.serial_number[3]);
             
         return BDMF_FALSE;
    }

    p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id, 
        "Serial number match: MATCH!");

    return BDMF_TRUE;
}

void gpon_set_pmd_fb_done(uint8_t state)
{
    PON_ERROR_DTE    gpon_error ;

    printk("\n**** gpon_set_pmd_fb_done to %d****\n", state);

    if (state == 1)
    {
        gpon_error = gpon_tx_set_pls_status(BDMF_FALSE);
        if ( gpon_error != PON_NO_ERROR )
        {
            /* Log */
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure PLS due to driver error %d !", gpon_error ) ;
        }
        pmdFirstBurstDone = true;
    }
    else
    {
        //Should pls_status set to 'true' or leave it to stack 02?
        pmdFirstBurstDone = false;
    }
}



