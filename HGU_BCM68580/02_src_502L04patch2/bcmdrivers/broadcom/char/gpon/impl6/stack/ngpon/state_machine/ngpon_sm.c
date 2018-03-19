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
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <bcm_ext_timer.h>
#include <board.h>
#include <bdmf_dev.h>

/* driver */
#include "ngpon_db.h"
#include "ngpon_tune.h"
#include "aes_cmac.h"
#include "rogue_drv.h"
#include "pon_sm_common.h"
#include "bcm_map_part.h"
#include "NGPON_BLOCKS.h"
#ifdef CONFIG_BCM_GPON_TODD
#include "ngpon_tod.h"
#endif

#include <wan_drv.h>
#include <boardparms.h>


#if defined(USE_LOGGER)
#include "gpon_logger.h"
#endif
#ifdef _BYTE_ORDER_LITTLE_ENDIAN_       /* To prevent compilation for MIPS-based targets */
#if defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
void remap_ru_block_addrs(uint32_t block_index, const ru_block_rec *ru_blocks[])
{
    printk("need to build new ru map file!!!");
}
#else
extern void remap_ru_block_addrs(uint32_t block_index, const ru_block_rec *ru_blocks[]);
#endif
#endif
#ifdef USE_BDMF_SHELL
extern void ngpon_initialize_drv_pon_stack_shell(bdmfmon_handle_t driver_dir);
extern void ngpon_exit_drv_pon_stack_shell(void);
extern PON_ERROR_DTE ngpon_modify_tcont(PON_TCONT_ID_DTE xi_tcont_id,
    PON_ALLOC_ID_DTE xi_new_alloc_id);
#endif

char * g_activation_state_names [ OPERATION_NGPON2_STATES_NUMBER ] =
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
} ;

char * g_key_state_names [ CS_KEY_STATES_NUMBER ] =
{
    "INIT(KN0)            ",
    "WAIT_FOR_KEY_ACK(KN2)",
    "KEY_ACTIVE(KN4)      ",
    
} ;

char *g_activation_event_names[OPERATION_NGPON2_EVENTS_NUMBER] =
{
    "TIMER_DS_SYNC_CHECK_EXPIRE", 
    "DS_FRAME_SYNC",                  
    "BURST_PROFILE_PLOAM",        
    "ASSIGN_ONU_ID_PLOAM",        
    "RANGING_TIME_PLOAM",         
    "TIMER_TO1_EXPIRE",         
    "DEACTIVATE_ONU_ID_PLOAM",    
    "DISABLE_SERIAL_NUMBER_PLOAM",
    "DETECT_LODS" ,              
    "TIMER_TO2_EXPIRE",         
    "REQUEST_REGISTRATION_PLOAM" ,
    "ASSIGN_ALLOC_ID_PLOAM",      
    "KEY_CONTROL_PLOAM",          
    "SLEEP_ALLOW_PLOAM",
    "CHANNEL_PROFILE_PLOAM", 
    "PROTECTION_CONTROL_PLOAM",     
    "SYSTEM_PROFILE_PLOAM",         
    "CALIBRATION_REQUEST_PLOAM",    
    "ADJUST_TX_WAVELENGTH_PLOAM",   
    "TUNING_CONTROL_PLOAM",         
    "TIMER_TOZ_EXPIRE",             
    "TIMER_TO3_EXPIRE",             
    "TIMER_TO4_EXPIRE",             
    "TIMER_TO5_EXPIRE",     
    "POWER_CONSUMPTION_INQUIRE_PLOAM",
    "SFC_MATCH"
} ;

char * g_key_event_names [ KEY_EVENTS_NUMBER ] =
{
    "NEW_KEY_REQUEST", 
    "CONFIRM_KEY_REQUEST",                  
    "KEY_EVENT_TK4_EXPIRED",        
    "KEY_EVENT_TK5_EXPIRED",        
 
} ;
/******************************************************************************/

/* This type defines the rate constants structure                             */

/******************************************************************************/

typedef struct
{

    /* Rate string */
    char * rate_string ;

    /* Rate - bits per millisecond */
    uint32_t bit_rate ;

    /* Rate - bytes per millisecond */
    uint32_t byte_rate ;

    /* Words per frame */
    uint32_t words_per_frame ;

    /* Bytes per frame */
    uint32_t bytes_per_frame ;

    /* Bits per frame */
    uint32_t bits_per_frame ;
}
NGPON_US_RATE_CONSTANTS ;


/* Init the rate constants */
NGPON_US_RATE_CONSTANTS g_us_rate_constants [] =
{
    { "2.5Gb/s", 2488320, 311040, 9720, 38880, 311040 },      /* For 2.5Gb/s */
    { "5Gb/s",   0,       0,      0,    0,     0},            /* 5Gb/s - not used */
    { "10Gb/s",  9953280, 1244160, 38880, 155520, 1244160 },  /* For 10Gb/s */
} ;

/* US PLOAM waiting linked list */
#ifndef SIM_ENV
struct ploam_list_struct 
{
    struct list_head list;
    NGPON_US_PLOAM ploam;
    bdmf_boolean persistent;
    NGPON_PLOAM_TYPE type; 
    bdmf_boolean default_key;
};

struct list_head us_ploam_list;
spinlock_t lock_ploam_list;
#else

struct ploam_list_struct {} us_ploam_list;
int list_empty(struct ploam_list_struct* list) { return 1;} 

#endif 
int us_ploam_list_num = 0; /* Control issues counter */

static int attempt_to_sync = 0 ;


#define NGPON2_CPI_PSP_KEY   "Cpi" 
#define NGPON2_CPI_BUFLEN_2  2

#define NGPON2_NO_HUNT_PSP_KEY "noHunt"
#define NGPON2_NO_HUNT_BUFLEN_2  2

static bdmf_boolean g_in_rollback = BDMF_FALSE ;
static bdmf_boolean zero_eqd_after_tuning = BDMF_FALSE ;
static bdmf_boolean simplified_rollback_flag = BDMF_TRUE ;
static int sys_profiles_after_rollback_cnt = 0 ;
static bdmf_boolean ngpon2_no_hunt_mode = BDMF_FALSE;

/******************************************************************************/
/*                                                                            */
/* Global variables definitions                                               */
/*                                                                            */
/******************************************************************************/
const uint8_t  SessionK[] =       { 0x53, 0x65 ,0x73 , 0x73 , 0x69 , 0x6f , 0x6e ,0x4b };
const uint8_t  OMCI_IK_DATA[] =   { 0x4f, 0X4d ,0X43 ,0X49 ,0X49 ,0X6e ,0X74 ,0X65 ,0X67 ,0X72 ,0X69 ,0X74 ,0X79 ,0X4b ,0X65 ,0X79 };
const uint8_t  PLOAM_IK_DATA[] =  { 0x50, 0X4c ,0X4f ,0X41 ,0X4d ,0X49 ,0X6e ,0X74 ,0X65 ,0X67 ,0X72 ,0X74 ,0X79 ,0X4b ,0X65 ,0X79} ;
const uint8_t  KEK_DATA[]=        { 0x4b, 0X65 ,0X79 ,0X45 ,0X6e ,0X63 ,0X72 ,0X79 ,0X70 ,0X74 ,0X69 ,0X6f ,0X6e ,0X4b ,0X65 ,0X79 };
const uint8_t  MSK_INIT_KEY[]=    { 0X55, 0X55 ,0X55 ,0X55 ,0X55, 0X55 ,0X55 ,0X55 ,0X55, 0X55 ,0X55 ,0X55 ,0X55, 0X55 ,0X55 ,0X55 };
const uint8_t  KEY_NAME_DATA[] =  { 0x33, 0x31 ,0x34 ,0x31 ,0x35 ,0x39 ,0x32 ,0x36 ,0x35 ,0x33 ,0x35 ,0x38 ,0x39 ,0x37 ,0x39 ,0x33 };

/*****************************************************************************/
/* Macro to convert system tick to milisec and viseversa                     */
/*****************************************************************************/
#define NGPON_MILISEC_TO_TICKS(m) ((m * g_xgpon_db.os_resources.os_frequency) / 1000)
#define MS_TICKS_TO_MILISEC( t ) ( ( (t) * 1000 ) / ( g_xgpon_db.os_resources.os_frequency ) )
#define MS_DOWNSTREAM_FRAMES_TO_MILISEC( frames ) ( frames / CE_DOWNSTREAM_FRAMES_IN_MS )

#define MICROSEC_IN_MILISEC                    1000
#define MICROSEC_IN_SEC                       (1000 * (MICROSEC_IN_MILISEC))

XGPON_RXPON_PM_COUNTERS rx_counters;

bool pmdFirstBurstDone = false;

bdmf_error_t send_message_to_pon_task(PON_EVENTS_DTE xi_msg_opcode, ...) ;
uint32_t ngpon_get_pon_id (void) ;

static PON_ERROR_DTE generate_sn_ploam ( void ) ;
static uint32_t generate_random_delay ( void ) ;
static void generate_random_aes_key ( AES_KEY_DTE *  xi_key  ) ;
static bdmf_boolean onu_serial_number_match ( uint8_t* xi_serial_number_1, uint8_t* xi_serial_number_2 );
static PON_ERROR_DTE write_sn_ploam_to_hw ( uint32_t  random_delay ) ;
static PON_ERROR_DTE write_us_ploam(NGPON_US_PLOAM *ploam, bdmf_boolean persistent, 
    NGPON_PLOAM_TYPE type, bdmf_boolean default_key);
static PON_ERROR_DTE send_us_ploam(NGPON_US_PLOAM *ploam, bdmf_boolean persistent,
    NGPON_PLOAM_TYPE type, bdmf_boolean default_key);
static PON_ERROR_DTE ngpon_write_ploam_to_buf ( NGPON_PLOAM_TYPE  xi_ploam_type,
                                               uint8_t xi_ploam [ NGPON_PLOAM_LEN_IN_BYTES_NO_MIC ] ) ;
static PON_ERROR_DTE write_registration_ploam_to_hw(NGPON_ONU_ID onu_id, 
    uint32_t sequence_num, bdmf_boolean persistent, NGPON_PLOAM_TYPE type);
static PON_ERROR_DTE write_idle_ploam_to_hw(NGPON_ONU_ID onu_id);
static PON_ERROR_DTE write_ack_ploam_to_hw_comp_code(NGPON_ONU_ID onu_id, uint32_t sequence_num, uint8_t completion_code) ;
static PON_ERROR_DTE write_ack_ploam_to_hw(NGPON_ONU_ID onu_id, uint32_t sequence_num);
static PON_ERROR_DTE write_key_report_ploam_to_hw(NGPON_ONU_ID onu_id, 
    uint32_t sequence_num, AES_KEY_DTE *key_fragment, 
    NGPON_ENCRYP_DATA_KEY_INDEX key_index, uint32_t report_type);
static PON_ERROR_DTE  set_burst_profile_to_hw ( XGPON_BURST_PROFILE_INFO * xi_profile ) ;
static void save_burst_profile_to_db(NGPON_DS_BURST_PROFILE_PLOAM *profile);  
static PON_ERROR_DTE set_tx_ploam_key( NGPON_ENCRYP_PLOAM_KEY_TYPE ploam_key_type );
static PON_ERROR_DTE set_rx_ploam_key( NGPON_ENCRYP_PLOAM_KEY_TYPE ploam_key_type );
static PON_ERROR_DTE set_omci_keys( void ) ;
static PON_ERROR_DTE ngpon_generate_key_name_and_kek_encrypted_key ( uint8_t * xi_encryption_key , uint8_t * xi_kek , uint8_t * xo_key_name , uint8_t * xo_kek_encrypted_key );
static PON_ERROR_DTE ngpon_generate_session_keys ( uint8_t * xi_msk_key , uint8_t * xi_sn , uint8_t * xi_pon_tag ,
                                          uint8_t * xo_sk , uint8_t * xo_omci_ik , uint8_t * xo_ploam_ik ,
                                          uint8_t * xo_kek );
static PON_ERROR_DTE  ngpon_generate_master_session_key (uint8_t * xi_registration_id , uint8_t *xo_msk  );
static PON_ERROR_DTE ngpon_generate_ploam_cmac_k2_key (const uint8_t * xi_ploam_ik, uint8_t *xo_cmac_k2  );
static PON_ERROR_DTE ngpon_general_state_burst_profile_event(NGPON_DS_BURST_PROFILE_PLOAM *profile);
static void sm_general_deactivate_onu_id_ploam_event(LINK_OPERATION_STATES_DTE state);
void p_sm_general_state_lods_event( NGPON_OPERATION_SM  * xi_operation_sm,
                                       OPERATION_SM_PARAMS * xi_operation_sm_params,
                                    NGPON_ONU_LINK_STATE_TRANSITION_INDICATION_PARAMS  xi_link_states );
void p_sm_general_state_timer_ds_sync_check_expire_event( NGPON_OPERATION_SM  * xi_operation_sm,
                                                          OPERATION_SM_PARAMS * xi_operation_sm_params );
void p_sm_general_state_disable_serial_number_ploam_event(LINK_OPERATION_STATES_DTE *activation_state,
    NGPON_DS_DISABLE_SERIAL_NUMBER_PLOAM *disable_sn, LINK_OPERATION_STATES_DTE xi_current_state);
static void p_operation_invalid_cross ( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_operation_ignore_event ( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void sm_general_protection_control_ploam_message_received(NGPON_DS_PLOAM *protection);
static void sm_general_assign_sn_ploam_message_received(uint8_t seq_no,
    NGPON_DS_ASSIGN_ONU_ID_PLOAM *sn_ploam, LINK_OPERATION_STATES_DTE *state);
static void general_ranging_time_ploam_event(NGPON_DS_PLOAM *ploam);
static void o9_tuning_confirmation_event(void);
static void write_tuning_response(uint8_t operation_code, uint16_t response_code, 
    uint8_t sequence_number);

/* State O1 */
static void p_sm_init_o1_state_timer_ds_sync_check_expire_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_init_o1_state_ds_frame_sync_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_init_o1_state_channel_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_init_o1_state_protection_control_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_init_o1_state_system_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_init_o1_state_burst_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_init_o1_state_detect_lods_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_init_o1_state_disable_serial_number_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);

/* State O2 */
static void p_sm_sn_pre_profile_o2_state_burst_profile_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_sn_pre_profile_o2_state_deactivate_onu_id_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_sn_pre_profile_o2_state_disable_serial_number_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params) ;
static void p_sm_sn_pre_profile_o2_state_detect_lods_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;

/* State O3 */
static void p_sm_sn_post_profile_o3_state_burst_profile_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_sn_post_profile_o3_state_assigned_onu_id_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_sn_post_profile_o3_state_deactivate_onu_id_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_sn_post_profile_o3_state_disable_serial_number_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params) ;
static void p_sm_sn_post_profile_o3_state_detect_lods_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;

/* State O2-3 */
static void p_sm_sn_o23_state_calibration_request_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_sn_o23_state_system_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_sn_o23_state_channel_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_sn_o23_state_adjust_tx_wavelength_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_sn_o23_state_protection_control_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_sn_o23_state_timer_toz_expire_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);

/* State O4 */
static void p_sm_ranging_o4_state_burst_profile_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_ranging_o4_state_ranging_time_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_ranging_o4_state_timer_to1_expire_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_ranging_o4_state_deactivate_onu_id_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;                  
static void p_sm_ranging_o4_state_disable_serial_number_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params) ;  
static void p_sm_ranging_o4_state_detect_lods_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;                               
static void p_sm_ranging_o4_state_system_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ranging_o4_state_channel_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ranging_o4_state_adjust_tx_wavelength_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ranging_o4_state_protection_control_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ranging_o4_state_assigned_onu_id_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);

/* State O5 */
static void p_sm_operation_o5_state_burst_profile_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_operation_o5_state_ranging_time_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_operation_o5_state_deactivate_onu_id_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;                   
static void p_sm_operation_o5_state_disable_serial_number_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params) ;   
static void p_sm_operation_o5_state_detect_lods_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_operation_o5_state_request_registration_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_operation_o5_state_assign_alloc_id_ploam_event ( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_operation_o5_state_key_control_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_operation_o5_state_sleep_allow_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_operation_o5_state_system_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_operation_o5_state_channel_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_operation_o5_state_adjust_tx_wavelength_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_operation_o5_state_protection_control_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_operation_o5_state_tuning_control_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_operation_o5_state_assigned_onu_id_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_operation_o5_state_power_consumption_inquire_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_operation_o5_state_sfc_match_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);


/* State O6 */
static void p_intermittent_lods_o6_state_timer_ds_sync_check_expire_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_intermittent_lods_o6_state_ds_frame_sync_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_intermittent_lods_o6_state_timer_to2_expire_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_intermittent_lods_o6_state_timer_to3_expire_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);

/* State O7 */
static void p_sm_emergency_stop_o7_state_timer_ds_sync_check_expire_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_emergency_stop_o7_state_disable_serial_number_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;
static void p_sm_emergency_stop_o7_state_detect_lods_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;

/* State O8 */
static void p_sm_ds_tuning_o8_state_ds_frame_sync_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ds_tuning_o8_state_detect_lods_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ds_tuning_o8_state_channel_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ds_tuning_o8_state_system_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ds_tuning_o8_state_protection_control_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ds_tuning_o8_state_timer_to4_expire_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ds_tuning_o8_state_assigned_onu_id_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ds_tuning_o8_state_ranging_time_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ds_tuning_o8_state_deactivate_onu_id_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ds_tuning_o8_state_disable_serial_number_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_ds_tuning_o8_state_timer_ds_sync_check_expire_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ); 
static void p_sm_ds_tuning_o8_state_burst_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);

/* State O9 */
static void p_sm_us_tuning_o9_state_adjust_tx_wavelength_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_timer_to5_expire_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_assigned_onu_id_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_ranging_time_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_deactivate_onu_id_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_disable_serial_number_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_request_registration_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_assign_alloc_id_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_sleep_allow_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_key_control_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_tuning_control_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_protection_control_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_power_consumption_inquire_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_channel_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_system_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_protection_control_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);
static void p_sm_us_tuning_o9_state_burst_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params);

/* Sub State O1.1 */
static void sm_o1_offsync_sub_state_o1_1_ds_frame_sync_event(o1_sub_states *sub_state, sub_sm_params *params);
/* Sub State O1.2 */
static void sm_o1_profile_learning_sub_state_o1_2_system_profile_ploam_event(o1_sub_states *sub_state, sub_sm_params *params);
static void sm_o1_profile_learning_sub_state_o1_2_burst_profile_ploam_event(o1_sub_states *sub_state, sub_sm_params *params);
static void sm_o1_profile_learning_sub_state_o1_2_channel_profile_ploam_event(o1_sub_states *sub_state, sub_sm_params *params);
static void sm_o1_profile_learning_sub_state_o1_2_detect_lods_event(o1_sub_states *sub_state, sub_sm_params *params);
static void sm_o1_profile_learning_sub_state_o1_2_disable_serial_number_ploam_event(o1_sub_states *sub_state, sub_sm_params *params);
static void sm_o1_profile_learning_sub_state_o1_2_dwlch_ok_to_work(sub_sm_params *params);

static void sub_sm_o1_invalid_cross(o1_sub_states *sub_state, sub_sm_params *params);

static void sub_sm_o5_invalid_cross(o5_sub_states *sub_state, sub_sm_params *params); 

/* Sub State O5.1 */
static void sm_o5_associated_sub_state_o5_1_tuning_control_ploam_event(o5_sub_states *sub_state, sub_sm_params *params);
/* Sub State O5.2 */
static void sm_o5_pending_sub_state_o5_2_sfc_match_event(o5_sub_states *sub_state, sub_sm_params *params);

static void sub_sm_o5_invalid_cross(o5_sub_states *sub_state, sub_sm_params *params);

/* Sub State O8.1 */
static void sm_o8_offsync_sub_state_o8_1_detect_lods_event(o8_sub_states *sub_state, sub_sm_params *params);
static void sm_o8_offsync_sub_state_o8_1_ds_frame_sync_event(o8_sub_states *sub_state, sub_sm_params *params);
static void sm_o8_offsync_sub_state_o8_1_timer_to4_expire_event(o8_sub_states *sub_state, sub_sm_params *params);
/* Sub State O8.2 */
static void sm_o8_profile_learning_sub_state_o8_2_detect_lods_event(o8_sub_states *sub_state, sub_sm_params *params);
static void sm_o8_profile_learning_sub_state_o8_2_burst_profile_ploam_event(o8_sub_states *sub_state, sub_sm_params *params);
static void sm_o8_profile_learning_sub_state_o8_2_channel_profile_ploam_event(o8_sub_states *sub_state, sub_sm_params *params);
static void sm_o8_profile_learning_sub_state_o8_2_system_profile_ploam_event(o8_sub_states *sub_state, sub_sm_params *params);
static void sm_o8_profile_learning_sub_state_o8_2_protection_control_ploam_event(o8_sub_states *sub_state, sub_sm_params *params);
static void sm_o8_profile_learning_sub_state_o8_2_timer_to4_expire_event(o8_sub_states *sub_state, sub_sm_params *params);
static void sm_o8_profile_learning_sub_state_o8_2_assigned_onu_id_ploam_event(o8_sub_states *sub_state, sub_sm_params *params);
static void sm_o8_profile_learning_sub_state_o8_2_ranging_time_ploam_event(o8_sub_states *sub_state, sub_sm_params *params);
static void sm_o8_profile_learning_sub_state_o8_2_deactivate_onu_id_ploam_event(o8_sub_states *sub_state, sub_sm_params *params);
static void sm_o8_profile_learning_sub_state_o8_2_disable_serial_number_ploam_event(o8_sub_states *sub_state, sub_sm_params *params);
static void sub_sm_o8_invalid_cross(o8_sub_states *sub_state, sub_sm_params *params);
static void sm_o8_profile_learning_sub_state_o8_2_dwlch_ok_to_work(sub_sm_params *params);





typedef void ( * OPERATION_CROSS_FUNCTION_DTS ) ( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) ;

OPERATION_CROSS_FUNCTION_DTS g_operation_sm[OPERATION_NGPON2_STATES_NUMBER][OPERATION_NGPON2_EVENTS_NUMBER] =
{
    /* INIT (O1) */
    {
        /* TIMER_DS_SYNC_CHECK_EXPIRE  */
        p_sm_init_o1_state_timer_ds_sync_check_expire_event,

        /* DS_FRAME_SYNC               */  
        p_sm_init_o1_state_ds_frame_sync_event,

        /* BURST_PROFILE_PLOAM         */ 
        p_sm_init_o1_state_burst_profile_ploam_event,

        /* ASSIGN_ONU_ID_PLOAM         */ 
        p_operation_invalid_cross,

        /* RANGING_TIME_PLOAM          */ 
        p_operation_invalid_cross,

        /* TIMER_TO1_EXPIRE            */ 
        p_operation_invalid_cross,

        /* DEACTIVATE_ONU_ID_PLOAM     */ 
        p_operation_invalid_cross,

        /* DISABLE_SERIAL_NUMBER_PLOAM */ 
        p_sm_init_o1_state_disable_serial_number_ploam_event,

        /* DETECT_LODS                 */ 
        p_sm_init_o1_state_detect_lods_event,

        /* TIMER_TO2_EXPIRE            */ 
        p_operation_invalid_cross,

        /* REQUEST_REGISTRATION_PLOAM  */ 
        p_operation_invalid_cross,

        /* ASSIGN_ALLOC_ID_PLOAM       */ 
        p_operation_invalid_cross,

        /* KEY_CONTROL_PLOAM           */ 
        p_operation_invalid_cross,

        /* SLEEP_ALLOW_PLOAM           */ 
        p_operation_invalid_cross,

        /* CHANNEL_PROFILE_PLOAM */
        p_sm_init_o1_state_channel_profile_ploam_event,

        /* PROTECTION_CONTROL_PLOAM */
        p_sm_init_o1_state_protection_control_ploam_event,

        /* SYSTEM_PROFILE_PLOAM */
        p_sm_init_o1_state_system_profile_ploam_event,

        /* CALIBRATION_REQUEST_PLOAM */
        p_operation_invalid_cross,

        /* ADJUST_TX_WAVELENGTH_PLOAM */
        p_operation_invalid_cross,

        /* TUNING_CONTROL_PLOAM */
        p_operation_invalid_cross,

        /* TIMER_TOZ_EXPIRE */        
        p_operation_invalid_cross,

        /* TIMER_TO3_EXPIRE */        
        p_operation_invalid_cross,

        /* TIMER_TO4_EXPIRE */        
        p_operation_invalid_cross,

        /* TIMER_TO5_EXPIRE */        
        p_operation_invalid_cross,

        /* POWER_CONSUMPTION_INQUIRE */
        p_operation_invalid_cross,

        /* SFC_MATCH */
        p_operation_invalid_cross,

    },

    /* Serial Number (Pre-Profile) (O2) */
    {
        /* TIMER_DS_SYNC_CHECK_EXPIRE  */
         p_operation_invalid_cross,

        /* DS_FRAME_SYNC               */  
        p_operation_invalid_cross,

        /* BURST_PROFILE_PLOAM         */ 
        p_sm_sn_pre_profile_o2_state_burst_profile_ploam_event,

        /* ASSIGN_ONU_ID_PLOAM         */ 
        p_operation_invalid_cross,
      
        /* RANGING_TIME_PLOAM          */ 
        p_operation_invalid_cross,

        /* TIMER_TO1_EXPIRE            */ 
        p_operation_invalid_cross,

        /* DEACTIVATE_ONU_ID_PLOAM     */ 
        p_sm_sn_pre_profile_o2_state_deactivate_onu_id_ploam_event,

        /* DISABLE_SERIAL_NUMBER_PLOAM */ 
        p_sm_sn_pre_profile_o2_state_disable_serial_number_ploam_event,

        /* DETECT_LODS                 */ 
        p_sm_sn_pre_profile_o2_state_detect_lods_event,

        /* TIMER_TO2_EXPIRE            */ 
        p_operation_invalid_cross,

        /* REQUEST_REGISTRATION_PLOAM  */ 
        p_operation_invalid_cross,

        /* ASSIGN_ALLOC_ID_PLOAM       */ 
        p_operation_invalid_cross,

        /* KEY_CONTROL_PLOAM           */ 
        p_operation_invalid_cross,

        /* SLEEP_ALLOW_PLOAM           */ 
        p_operation_invalid_cross,

        /* CHANNEL_PROFILE_PLOAM */
        p_sm_sn_o23_state_channel_profile_ploam_event,

        /* PROTECTION_CONTROL_PLOAM */
        p_sm_sn_o23_state_protection_control_ploam_event,
        
        /* SYSTEM_PROFILE_PLOAM */
        p_sm_sn_o23_state_system_profile_ploam_event,

        /* CALIBRATION_REQUEST_PLOAM */
        p_sm_sn_o23_state_calibration_request_ploam_event,
        
        /* ADJUST_TX_WAVELENGTH_PLOAM */
        p_sm_sn_o23_state_adjust_tx_wavelength_ploam_event,

        /* TUNING_CONTROL_PLOAM */
        p_operation_invalid_cross,

        /* TIMER_TOZ_EXPIRE */        
        p_sm_sn_o23_state_timer_toz_expire_event,
    
        /* TIMER_TO3_EXPIRE */        
        p_operation_invalid_cross,
    
        /* TIMER_TO4_EXPIRE */        
        p_operation_invalid_cross,
    
        /* TIMER_TO5_EXPIRE */        
        p_operation_invalid_cross,

        /* POWER_CONSUMPTION_INQUIRE */
        p_operation_invalid_cross,

        /* SFC_MATCH */
        p_operation_invalid_cross,

    },

    /* Serial Number (POst-Profile) (O3) */
    {
        /* TIMER_DS_SYNC_CHECK_EXPIRE  */
        p_operation_invalid_cross,

        /* DS_FRAME_SYNC               */  
        p_operation_invalid_cross,

        /* BURST_PROFILE_PLOAM         */ 
        p_sm_sn_post_profile_o3_state_burst_profile_ploam_event,

        /* ASSIGN_ONU_ID_PLOAM         */ 
        p_sm_sn_post_profile_o3_state_assigned_onu_id_ploam_event,

        /* RANGING_TIME_PLOAM          */ 
        p_operation_invalid_cross,

        /* TIMER_TO1_EXPIRE            */ 
        p_operation_invalid_cross,

        /* DEACTIVATE_ONU_ID_PLOAM     */ 
        p_sm_sn_post_profile_o3_state_deactivate_onu_id_ploam_event,

        /* DISABLE_SERIAL_NUMBER_PLOAM */ 
        p_sm_sn_post_profile_o3_state_disable_serial_number_ploam_event,

        /* DETECT_LODS                 */ 
        p_sm_sn_post_profile_o3_state_detect_lods_event,

        /* TIMER_TO2_EXPIRE            */ 
        p_operation_invalid_cross,

        /* REQUEST_REGISTRATION_PLOAM  */ 
        p_operation_invalid_cross,

        /* ASSIGN_ALLOC_ID_PLOAM       */ 
        p_operation_invalid_cross,

        /* KEY_CONTROL_PLOAM           */ 
        p_operation_invalid_cross,

        /* SLEEP_ALLOW_PLOAM           */ 
        p_operation_invalid_cross,

        /* CHANNEL_PROFILE_PLOAM */
        p_sm_sn_o23_state_channel_profile_ploam_event,

        /* PROTECTION_CONTROL_PLOAM */
        p_sm_sn_o23_state_protection_control_ploam_event,

        /* SYSTEM_PROFILE_PLOAM */
        p_sm_sn_o23_state_system_profile_ploam_event,

        /* CALIBRATION_REQUEST_PLOAM */
        p_sm_sn_o23_state_calibration_request_ploam_event,

        /* ADJUST_TX_WAVELENGTH_PLOAM */
        p_sm_sn_o23_state_adjust_tx_wavelength_ploam_event,
        
        /* TUNING_CONTROL_PLOAM */
        p_operation_invalid_cross,

        /* TIMER_TOZ_EXPIRE */        
        p_sm_sn_o23_state_timer_toz_expire_event,
   
        /* TIMER_TO3_EXPIRE */        
        p_operation_invalid_cross,
   
        /* TIMER_TO4_EXPIRE */        
        p_operation_invalid_cross,
   
        /* TIMER_TO5_EXPIRE */        
        p_operation_invalid_cross,

        /* POWER_CONSUMPTION_INQUIRE */
        p_operation_invalid_cross,


        /* SFC_MATCH */
        p_operation_invalid_cross,

    },

    /* Ranging (O4) */
    {
        /* TIMER_DS_SYNC_CHECK_EXPIRE  */
        p_operation_invalid_cross,

        /* DS_FRAME_SYNC               */  
        p_operation_invalid_cross,

        /* BURST_PROFILE_PLOAM         */ 
        p_sm_ranging_o4_state_burst_profile_ploam_event,

        /* ASSIGN_ONU_ID_PLOAM         */ 
        p_sm_ranging_o4_state_assigned_onu_id_ploam_event,

        /* RANGING_TIME_PLOAM          */ 
        p_sm_ranging_o4_state_ranging_time_ploam_event,

        /* TIMER_TO1_EXPIRE            */ 
        p_sm_ranging_o4_state_timer_to1_expire_event,

        /* DEACTIVATE_ONU_ID_PLOAM     */ 
        p_sm_ranging_o4_state_deactivate_onu_id_ploam_event,

        /* DISABLE_SERIAL_NUMBER_PLOAM */ 
        p_sm_ranging_o4_state_disable_serial_number_ploam_event,

        /* DETECT_LODS                 */ 
        p_sm_ranging_o4_state_detect_lods_event,

        /* TIMER_TO2_EXPIRE            */ 
        p_operation_invalid_cross,

        /* REQUEST_REGISTRATION_PLOAM  */ 
        p_operation_invalid_cross,

        /* ASSIGN_ALLOC_ID_PLOAM       */ 
        p_operation_invalid_cross,

        /* KEY_CONTROL_PLOAM           */ 
        p_operation_invalid_cross,

        /* SLEEP_ALLOW_PLOAM           */ 
        p_operation_invalid_cross,

        /* CHANNEL_PROFILE_PLOAM */
        p_sm_ranging_o4_state_channel_profile_ploam_event,

        /* PROTECTION_CONTROL_PLOAM */
        p_sm_ranging_o4_state_protection_control_ploam_event,

        /* SYSTEM_PROFILE_PLOAM */
        p_sm_ranging_o4_state_system_profile_ploam_event,

        /* CALIBRATION_REQUEST_PLOAM */
        p_operation_invalid_cross,

        /* ADJUST_TX_WAVELENGTH_PLOAM */
        p_sm_ranging_o4_state_adjust_tx_wavelength_ploam_event,

        /* TUNING_CONTROL_PLOAM */
        p_operation_invalid_cross,

        /* TIMER_TOZ_EXPIRE */        
        p_operation_invalid_cross,
     
        /* TIMER_TO3_EXPIRE */        
        p_operation_invalid_cross,
     
        /* TIMER_TO4_EXPIRE */        
        p_operation_invalid_cross,
     
        /* TIMER_TO5_EXPIRE */        
        p_operation_invalid_cross,

        /* POWER_CONSUMPTION_INQUIRE */
        p_operation_invalid_cross,

        /* SFC_MATCH */
        p_operation_invalid_cross,

    },

    /* Operation (O5) */
    {
        /* TIMER_DS_SYNC_CHECK_EXPIRE  */
        p_operation_invalid_cross,

        /* DS_FRAME_SYNC               */  
        p_operation_ignore_event,     /* DS Sync already attained, event from stopped timer can still arrive */

        /* BURST_PROFILE_PLOAM         */ 
        p_sm_operation_o5_state_burst_profile_ploam_event,

        /* ASSIGN_ONU_ID_PLOAM         */ 
        p_sm_operation_o5_state_assigned_onu_id_ploam_event,

        /* RANGING_TIME_PLOAM          */ 
        p_sm_operation_o5_state_ranging_time_ploam_event,

        /* TIMER_TO1_EXPIRE            */ 
        p_operation_invalid_cross,

        /* DEACTIVATE_ONU_ID_PLOAM     */ 
        p_sm_operation_o5_state_deactivate_onu_id_ploam_event,

        /* DISABLE_SERIAL_NUMBER_PLOAM */
        p_sm_operation_o5_state_disable_serial_number_ploam_event,

        /* DETECT_LODS                 */ 
        p_sm_operation_o5_state_detect_lods_event,

        /* TIMER_TO2_EXPIRE            */ 
        p_operation_invalid_cross,

        /* REQUEST_REGISTRATION_PLOAM  */ 
        p_sm_operation_o5_state_request_registration_ploam_event,

        /* ASSIGN_ALLOC_ID_PLOAM       */ 
        p_sm_operation_o5_state_assign_alloc_id_ploam_event,

        /* KEY_CONTROL_PLOAM           */ 
        p_sm_operation_o5_state_key_control_ploam_event,

        /* SLEEP_ALLOW_PLOAM           */ 
        p_sm_operation_o5_state_sleep_allow_ploam_event,

        /* CHANNEL_PROFILE_PLOAM */
        p_sm_operation_o5_state_channel_profile_ploam_event,
        
        /* PROTECTION_CONTROL_PLOAM */
        p_sm_operation_o5_state_protection_control_ploam_event,

        /* SYSTEM_PROFILE_PLOAM */
        p_sm_operation_o5_state_system_profile_ploam_event,

        /* CALIBRATION_REQUEST_PLOAM */
        p_operation_invalid_cross,

        /* ADJUST_TX_WAVELENGTH_PLOAM */
        p_sm_operation_o5_state_adjust_tx_wavelength_ploam_event,

        /* TUNING_CONTROL_PLOAM */
        p_sm_operation_o5_state_tuning_control_ploam_event,

        /* TIMER_TOZ_EXPIRE */        
        p_operation_invalid_cross,
    
        /* TIMER_TO3_EXPIRE */        
        p_operation_invalid_cross,
    
        /* TIMER_TO4_EXPIRE */        
        p_operation_invalid_cross,
    
        /* TIMER_TO5_EXPIRE */        
        p_operation_invalid_cross,
 
        /* POWER_CONSUMPTION_INQUIRE */
        p_sm_operation_o5_state_power_consumption_inquire_ploam_event,

        /* SFC_MATCH */
        p_sm_operation_o5_state_sfc_match_event,
    },

    /* Intermittent_LODS (O6) */
    {
        /* TIMER_DS_SYNC_CHECK_EXPIRE  */
        p_intermittent_lods_o6_state_timer_ds_sync_check_expire_event,

        /* DS_FRAME_SYNC               */  
        p_intermittent_lods_o6_state_ds_frame_sync_event,

        /* BURST_PROFILE_PLOAM         */ 
        p_operation_invalid_cross,

        /* ASSIGN_ONU_ID_PLOAM         */ 
        p_operation_invalid_cross,

        /* RANGING_TIME_PLOAM          */ 
        p_operation_invalid_cross,

        /* TIMER_TO1_EXPIRE            */ 
        p_operation_invalid_cross,

        /* DEACTIVATE_ONU_ID_PLOAM     */ 
        p_operation_invalid_cross,

        /* DISABLE_SERIAL_NUMBER_PLOAM */ 
        p_operation_invalid_cross,

        /* DETECT_LODS                 */ 
        p_operation_invalid_cross,

        /* TIMER_TO2_EXPIRE            */ 
        p_intermittent_lods_o6_state_timer_to2_expire_event,

        /* REQUEST_REGISTRATION_PLOAM  */ 
        p_operation_invalid_cross,

        /* ASSIGN_ALLOC_ID_PLOAM       */ 
        p_operation_invalid_cross,

        /* KEY_CONTROL_PLOAM           */ 
        p_operation_invalid_cross,

        /* SLEEP_ALLOW_PLOAM           */ 
        p_operation_invalid_cross,

        /* CHANNEL_PROFILE_PLOAM */
        p_operation_invalid_cross,

        /* PROTECTION_CONTROL_PLOAM */
        p_operation_invalid_cross,

        /* SYSTEM_PROFILE_PLOAM */
        p_operation_invalid_cross,

        /* CALIBRATION_REQUEST_PLOAM */
        p_operation_invalid_cross,
        
        /* ADJUST_TX_WAVELENGTH_PLOAM */
        p_operation_invalid_cross,

        /* TUNING_CONTROL_PLOAM */
        p_operation_invalid_cross,

        /* TIMER_TOZ_EXPIRE */        
        p_operation_invalid_cross,
    
        /* TIMER_TO3_EXPIRE */        
        p_intermittent_lods_o6_state_timer_to3_expire_event,
    
        /* TIMER_TO4_EXPIRE */        
        p_operation_invalid_cross,
    
        /* TIMER_TO5_EXPIRE */        
        p_operation_invalid_cross,

        /* POWER_CONSUMPTION_INQUIRE */
        p_operation_invalid_cross,

        /* SFC_MATCH */
        p_operation_invalid_cross,
    },

    /* Emergency-Stop (O7) */
    {
        /* TIMER_DS_SYNC_CHECK_EXPIRE  */
        p_sm_emergency_stop_o7_state_timer_ds_sync_check_expire_event,

        /* DS_FRAME_SYNC               */  
        p_operation_invalid_cross,

        /* BURST_PROFILE_PLOAM         */ 
        p_operation_invalid_cross,

        /* ASSIGN_ONU_ID_PLOAM         */ 
        p_operation_invalid_cross,

        /* RANGING_TIME_PLOAM          */ 
        p_operation_invalid_cross,

        /* TIMER_TO1_EXPIRE            */ 
        p_operation_invalid_cross,

        /* DEACTIVATE_ONU_ID_PLOAM     */ 
        p_operation_invalid_cross,

        /* DISABLE_SERIAL_NUMBER_PLOAM */
        p_sm_emergency_stop_o7_state_disable_serial_number_ploam_event,

        /* DETECT_LODS                 */ 
        p_sm_emergency_stop_o7_state_detect_lods_event,

        /* TIMER_TO2_EXPIRE            */ 
        p_operation_invalid_cross,

        /* REQUEST_REGISTRATION_PLOAM  */ 
        p_operation_invalid_cross,

        /* ASSIGN_ALLOC_ID_PLOAM       */ 
        p_operation_invalid_cross,

        /* KEY_CONTROL_PLOAM           */ 
        p_operation_invalid_cross,

        /* SLEEP_ALLOW_PLOAM           */ 
        p_operation_invalid_cross,

        /* CHANNEL_PROFILE_PLOAM */
        p_operation_invalid_cross,

        /* PROTECTION_CONTROL_PLOAM */
        p_operation_invalid_cross,

        /* SYSTEM_PROFILE_PLOAM */
        p_operation_invalid_cross,

        /* CALIBRATION_REQUEST_PLOAM */
        p_operation_invalid_cross,

        /* ADJUST_TX_WAVELENGTH_PLOAM */
        p_operation_invalid_cross,

        /* TUNING_CONTROL_PLOAM */
        p_operation_invalid_cross,

        /* TIMER_TOZ_EXPIRE */        
        p_operation_invalid_cross,
  
        /* TIMER_TO3_EXPIRE */        
        p_operation_invalid_cross,
  
        /* TIMER_TO4_EXPIRE */        
        p_operation_invalid_cross,
  
        /* TIMER_TO5_EXPIRE */        
        p_operation_invalid_cross,

        /* POWER_CONSUMPTION_INQUIRE */
        p_operation_invalid_cross,

        /* SFC_MATCH */
        p_operation_invalid_cross,
    },

    /* DS Tuning (O8) */
    {
        /* TIMER_DS_SYNC_CHECK_EXPIRE */
        p_sm_ds_tuning_o8_state_timer_ds_sync_check_expire_event,
        
        /* DS_FRAME_SYNC */  
        p_sm_ds_tuning_o8_state_ds_frame_sync_event,
        
        /* BURST_PROFILE_PLOAM */ 
        p_sm_ds_tuning_o8_state_burst_profile_ploam_event,

        /* ASSIGN_ONU_ID_PLOAM */ 
        p_sm_ds_tuning_o8_state_assigned_onu_id_ploam_event,
        
        /* RANGING_TIME_PLOAM */ 
        p_sm_ds_tuning_o8_state_ranging_time_ploam_event,
        
        /* TIMER_TO1_EXPIRE */ 
        p_operation_invalid_cross,
        
        /* DEACTIVATE_ONU_ID_PLOAM */ 
        p_sm_ds_tuning_o8_state_deactivate_onu_id_ploam_event,
        
        /* DISABLE_SERIAL_NUMBER_PLOAM */
        p_sm_ds_tuning_o8_state_disable_serial_number_ploam_event,
        
        /* DETECT_LODS */ 
        p_sm_ds_tuning_o8_state_detect_lods_event,
        
        /* TIMER_TO2_EXPIRE */ 
        p_operation_invalid_cross,
        
        /* REQUEST_REGISTRATION_PLOAM */ 
        p_operation_invalid_cross,
        
        /* ASSIGN_ALLOC_ID_PLOAM */ 
        p_operation_invalid_cross,
        
        /* KEY_CONTROL_PLOAM */ 
        p_operation_invalid_cross,
        
        /* SLEEP_ALLOW_PLOAM */ 
        p_operation_invalid_cross,
        
        /* CHANNEL_PROFILE_PLOAM */
        p_sm_ds_tuning_o8_state_channel_profile_ploam_event,

        /* PROTECTION_CONTROL_PLOAM */
        p_sm_ds_tuning_o8_state_protection_control_ploam_event,
        
        /* SYSTEM_PROFILE_PLOAM */
        p_sm_ds_tuning_o8_state_system_profile_ploam_event,
        
        /* CALIBRATION_REQUEST_PLOAM */
        p_operation_invalid_cross,

        /* ADJUST_TX_WAVELENGTH_PLOAM */
        p_operation_invalid_cross,
        
        /* TUNING_CONTROL_PLOAM */
        p_operation_invalid_cross,
        
        /* TIMER_TOZ_EXPIRE */        
        p_operation_invalid_cross,
        
        /* TIMER_TO3_EXPIRE */        
        p_operation_invalid_cross,
        
        /* TIMER_TO4_EXPIRE */        
        p_sm_ds_tuning_o8_state_timer_to4_expire_event,
        
        /* TIMER_TO5_EXPIRE */        
        p_operation_invalid_cross,

        /* POWER_CONSUMPTION_INQUIRE */
        p_operation_invalid_cross,

        /* SFC_MATCH */
        p_operation_invalid_cross,
    },

    /* US Tuning (O9) */
    {
        /* TIMER_DS_SYNC_CHECK_EXPIRE  */
        p_operation_invalid_cross,

        /* DS_FRAME_SYNC */  
        p_operation_invalid_cross,

        /* BURST_PROFILE_PLOAM */ 
        p_sm_us_tuning_o9_state_burst_profile_ploam_event,

        /* ASSIGN_ONU_ID_PLOAM */ 
        p_sm_us_tuning_o9_state_assigned_onu_id_ploam_event,

        /* RANGING_TIME_PLOAM  */ 
        p_sm_us_tuning_o9_state_ranging_time_ploam_event,

        /* TIMER_TO1_EXPIRE */ 
        p_operation_invalid_cross,

        /* DEACTIVATE_ONU_ID_PLOAM */ 
        p_sm_us_tuning_o9_state_deactivate_onu_id_ploam_event,

        /* DISABLE_SERIAL_NUMBER_PLOAM */
        p_sm_us_tuning_o9_state_disable_serial_number_ploam_event,

        /* DETECT_LODS */ 
        p_operation_invalid_cross,

        /* TIMER_TO2_EXPIRE */ 
        p_operation_invalid_cross,

        /* REQUEST_REGISTRATION_PLOAM */ 
        p_sm_us_tuning_o9_state_request_registration_ploam_event,

        /* ASSIGN_ALLOC_ID_PLOAM */ 
        p_sm_us_tuning_o9_state_assign_alloc_id_ploam_event,

        /* KEY_CONTROL_PLOAM */ 
        p_sm_us_tuning_o9_state_key_control_ploam_event,

        /* SLEEP_ALLOW_PLOAM */ 
        p_sm_us_tuning_o9_state_sleep_allow_ploam_event,

        /* CHANNEL_PROFILE_PLOAM */
        p_sm_us_tuning_o9_state_channel_profile_ploam_event,

        /* PROTECTION_CONTROL_PLOAM */
        p_sm_us_tuning_o9_state_protection_control_ploam_event,

        /* SYSTEM_PROFILE_PLOAM */
        p_sm_us_tuning_o9_state_system_profile_ploam_event,

        /* CALIBRATION_REQUEST_PLOAM */
        p_operation_invalid_cross,

        /* ADJUST_TX_WAVELENGTH_PLOAM */
        p_sm_us_tuning_o9_state_adjust_tx_wavelength_ploam_event,

        /* TUNING_CONTROL_PLOAM */
        p_sm_us_tuning_o9_state_tuning_control_ploam_event,

        /* TIMER_TOZ_EXPIRE */        
        p_operation_invalid_cross,
  
        /* TIMER_TO3_EXPIRE */        
        p_operation_invalid_cross,
  
        /* TIMER_TO4_EXPIRE */        
        p_operation_invalid_cross,
  
        /* TIMER_TO5_EXPIRE */        
        p_sm_us_tuning_o9_state_timer_to5_expire_event,

        /* POWER_CONSUMPTION_INQUIRE */
        p_sm_us_tuning_o9_state_power_consumption_inquire_ploam_event,

        /* SFC_MATCH */
        p_operation_invalid_cross,
    },
};

typedef void (*o1_sub_sm_functions)(o1_sub_states *sub_state, sub_sm_params *params);

o1_sub_sm_functions o1_sub_sm[2][o1_sub_sm_event_num] =
{
    /* O1.1 Off Sync */
    {
        sm_o1_offsync_sub_state_o1_1_ds_frame_sync_event,
        sub_sm_o1_invalid_cross,
        sub_sm_o1_invalid_cross,
        sub_sm_o1_invalid_cross,
        sub_sm_o1_invalid_cross,
        sub_sm_o1_invalid_cross
    },

    /* O1.2 profile Learning */
    {
        sub_sm_o1_invalid_cross,
        sm_o1_profile_learning_sub_state_o1_2_system_profile_ploam_event,
        sm_o1_profile_learning_sub_state_o1_2_channel_profile_ploam_event,
        sm_o1_profile_learning_sub_state_o1_2_burst_profile_ploam_event,
        sm_o1_profile_learning_sub_state_o1_2_detect_lods_event,
        sm_o1_profile_learning_sub_state_o1_2_disable_serial_number_ploam_event
    }, 
};


typedef void (*o8_sub_sm_functions)(o8_sub_states *sub_state, sub_sm_params *params);

o8_sub_sm_functions o8_sub_sm[2][o8_sub_sm_event_num] =
{
    /* O8.1 Off Sync */
    {
        sm_o8_offsync_sub_state_o8_1_ds_frame_sync_event,
        sm_o8_offsync_sub_state_o8_1_timer_to4_expire_event,
        sm_o8_offsync_sub_state_o8_1_detect_lods_event,
        sub_sm_o8_invalid_cross,
        sub_sm_o8_invalid_cross,
        sub_sm_o8_invalid_cross,
        sub_sm_o8_invalid_cross,
        sub_sm_o8_invalid_cross,
        sub_sm_o8_invalid_cross,
        sub_sm_o8_invalid_cross,
        sub_sm_o8_invalid_cross
    },

    /* O8.2 profile Learning */
    {
        sub_sm_o8_invalid_cross,
        sm_o8_profile_learning_sub_state_o8_2_timer_to4_expire_event,
        sm_o8_profile_learning_sub_state_o8_2_detect_lods_event,
        sm_o8_profile_learning_sub_state_o8_2_channel_profile_ploam_event,
        sm_o8_profile_learning_sub_state_o8_2_protection_control_ploam_event,
        sm_o8_profile_learning_sub_state_o8_2_assigned_onu_id_ploam_event,
        sm_o8_profile_learning_sub_state_o8_2_ranging_time_ploam_event,
        sm_o8_profile_learning_sub_state_o8_2_deactivate_onu_id_ploam_event,
        sm_o8_profile_learning_sub_state_o8_2_disable_serial_number_ploam_event,
        sm_o8_profile_learning_sub_state_o8_2_system_profile_ploam_event,
        sm_o8_profile_learning_sub_state_o8_2_burst_profile_ploam_event
    },                                                                         
};

typedef void (*o5_sub_sm_functions)(o5_sub_states *sub_state, sub_sm_params *params);

o5_sub_sm_functions o5_sub_sm[2][o5_sub_sm_event_num] =
{
    /* O5.1 associated */
    {
        sm_o5_associated_sub_state_o5_1_tuning_control_ploam_event,
        sub_sm_o5_invalid_cross
    },

    /* O5.2 pending */
    {
        sub_sm_o5_invalid_cross,
        sm_o5_pending_sub_state_o5_2_sfc_match_event,
    }, 
} ;


void p_key_invalid_cross  ( NGPON_AES_SM * xi_key_sm ,NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index) ;
void p_key_reset_configuration  ( void ) ;


void p_key_sm_init_kn0_state_new_key_request_event  ( NGPON_AES_SM * xi_key_sm ,NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index ) ;

void p_key_sm_ack_state_new_key_request_event( NGPON_AES_SM * xi_key_sm ,NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index ) ;
void p_key_sm_ack_state_confirm_key_request  ( NGPON_AES_SM * xi_key_sm ,NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index ) ;
void p_key_sm_ack_state_tk4_expire_event  ( NGPON_AES_SM * xi_key_sm ,NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index) ;
void p_key_sm_ack_state_tk5_expire_event  ( NGPON_AES_SM * xi_key_sm ,NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index ) ;


void p_key_sm_active_state_new_key_request_event  ( NGPON_AES_SM * xi_key_sm ,NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index) ;
void p_key_sm_active_state_confirm_key_request  ( NGPON_AES_SM * xi_key_sm ,NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index) ;


typedef void ( * KEY_CROSS_FUNCTION_DTS ) ( NGPON_AES_SM * xi_key_sm , NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index ) ;

KEY_CROSS_FUNCTION_DTS g_key_sm [ CS_KEY_STATES_NUMBER ] [ KEY_EVENTS_NUMBER ] =
{
    /* INIT (KN0) */
    {
        /* NEW_KEY_REQUEST  */
        p_key_sm_init_kn0_state_new_key_request_event,

        /* CONFIRM_KEY_REQUEST               */  
        p_key_invalid_cross,

        /* KEY_EVENT_TK4_EXPIRED         */ 
        p_key_invalid_cross,

        /* KEY_EVENT_TK5_EXPIRED         */ 
        p_key_invalid_cross,

    },

    /* WAIT_FOR_KEY_ACK (KN1-2KN2) */
    {
        /* NEW_KEY_REQUEST  */
        p_key_sm_ack_state_new_key_request_event,

        /* CONFIRM_KEY_REQUEST         */  
        p_key_sm_ack_state_confirm_key_request,

        /* KEY_EVENT_TK4_EXPIRED        */ 
        p_key_sm_ack_state_tk4_expire_event,

        /* KEY_EVENT_TK5_EXPIRED        */ 
        p_key_sm_ack_state_tk5_expire_event,
    },

    /* ACTIVE_KEY (KN3-KN4) */
    {
        /* NEW_KEY_REQUEST  */
        p_key_sm_active_state_new_key_request_event,

        /* CONFIRM_KEY_REQUEST         */  
        p_key_sm_active_state_confirm_key_request,

        /* KEY_EVENT_TK4_EXPIRED        */ 
        p_key_invalid_cross,

        /* KEY_EVENT_TK5_EXPIRED        */ 
        p_key_invalid_cross,
 

    }
 
} ;

static void ngpon_2_store_cpi_handler (struct work_struct *dummy)
{
   int error ;
   char cpi_buff[4];

   sprintf (cpi_buff, "%02d", g_xgpon_db.link_params.ch_profile.current_cpi);

   error =  kerSysScratchPadSet(NGPON2_CPI_PSP_KEY, cpi_buff, NGPON2_CPI_BUFLEN_2);
   if (error != 0)
   {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
           "Failed to set CPI to non volatile memory");     
   }

}
/*
 * kerSysScratchPadSet uses mutexes to protect its critical section, therefore it cannot
 * be called from the tasklet context where the whole Activation SM runs
 * Therefore storing CPI in the scratchpad is offloaded to shared workqueue
 */
DECLARE_WORK(ngpon2_store_cpi, ngpon_2_store_cpi_handler);


/* Converts array of 4 bytes into word (incase memcpy is not good due to Big Little/Endian issues) */
static void bytes_to_word ( uint8_t * xi_byte_array, uint32_t* xo_word )
{

  uint32_t  * xi_word_p = (uint32_t *) xi_byte_array ;

  /* To write to the register : network order to ARM's Little Endian  */

#ifndef SIM_ENV
    *xo_word = be32_to_cpu ( *xi_word_p ) ;
#else
  */
    *xo_word = xi_byte_array[ 0 ] ;
    (*xo_word) <<= 8 ;
    *xo_word  |= xi_byte_array[ 1 ] ;
    (*xo_word) <<= 8 ;
    *xo_word  |= xi_byte_array[ 2 ] ;    
    (*xo_word) <<= 8 ;
    *xo_word  |= xi_byte_array[ 3 ] ;
#endif

}




void set_zero_eqd_after_tuning (bdmf_boolean val)
{
    zero_eqd_after_tuning = val ;
}

bdmf_boolean get_zero_eqd_after_tuning (void)
{
    return zero_eqd_after_tuning ;
}


void set_simplified_rollback (bdmf_boolean val)
{
    simplified_rollback_flag = val ;
}

bdmf_boolean get_simplified_rollback (void)
{
    return simplified_rollback_flag ;
}


char * p_get_operation_state_name (LINK_KEY_STATES_DTE  operation_state)
{
  return  g_activation_state_names [operation_state] ;
}


static PON_ERROR_DTE generate_sn_ploam ( void )
{
    PON_ERROR_DTE xgpon_error ;
    uint32_t      random_delay;

    /*Get Random Delay */
    random_delay = generate_random_delay();

    xgpon_error =  ag_drv_ngpon_tx_cfg_onu_id_to_plm_map_set ( NGPON_ONU_INDEX_ONU_2, NO_ONU_ID_ASSIGNED_YET, 
                                                               NGPON_PLOAM_TYPE_IDLE_OR_SN, NGPON_PLOAM_TYPE_REGISTRATION);
    if ( PON_NO_ERROR != xgpon_error ) 
    {
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.unknown_message_id,
                "Unable to set ONU to PLOAM. error %lu",xgpon_error);
            return ( xgpon_error ) ;
    }

    xgpon_error =  ag_drv_ngpon_tx_cfg_tcont_to_onu_id_map_set (NGPON_SN_TX_TCONT_ID_VALUE, NGPON_ONU_INDEX_ONU_2) ;
    if ( PON_NO_ERROR != xgpon_error ) 
    {
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.unknown_message_id,
                "Unable to set ONU to T-CONT mapping in Tx. error %lu",xgpon_error);
            return ( xgpon_error ) ;
    }

    /* Add min response time and update delay to HW and  */
    xgpon_error = ngpon_set_eqd ( random_delay , P_ABSOLUTE ,SIGN_POSITIVE  );
    if ( PON_NO_ERROR != xgpon_error ) 
    {
        p_log (ge_onu_logger.sections.stack.downstream_ploam.general_id, "Unable to write EQD to HW. error %lu",xgpon_error);
        return xgpon_error ;
    }
    /* Write SN PLOAM to TX PLOAM queue that is associated to ONU ID 0x3FF */
    xgpon_error = write_sn_ploam_to_hw( random_delay );
    if ( PON_NO_ERROR != xgpon_error ) 
    {
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.unknown_message_id,
                "Unable to write SN PLOAM to HW. error %lu",xgpon_error);

        return ( xgpon_error ) ;
    }


    xgpon_error =  ag_drv_ngpon_tx_cfg_tcont_to_cnt_grp_map_set (NGPON_DUMMY_TCONT_ID_VALUE, 0, 0, 0);
    if ( PON_NO_ERROR != xgpon_error ) 
    {
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.unknown_message_id,
                "Unable to set counter group to T-CONT mapping in Tx. error %lu",xgpon_error);
            return ( xgpon_error ) ;
    }

    return ( PON_NO_ERROR ) ;
}

static uint32_t ranging_delay_correction_upstream_rate(uint32_t ranging_delay)
{
   if ((g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_10G) ||
       (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGS))
   {
      return ranging_delay *= 4;
   }
   else
   {
      return ranging_delay;
   }
}


static PON_ERROR_DTE control_sn_alloc_id ( bdmf_boolean  xi_enable_sn_alloc_id )
{
    PON_ERROR_DTE xgpon_error ;
    uint8_t       enable_alloc_ids = 1;

    xgpon_error = ag_drv_ngpon_rx_bwmap_rngng_allocid_set (NGPON_BCAST_ALLOC_ID_2_5_US_IDX, NGPON_BCAST_ALLOC_ID_2_5_US_VAL);

    if (xgpon_error != PON_NO_ERROR)
        return xgpon_error;

    if (g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGPON) 
    {
        xgpon_error = ag_drv_ngpon_rx_bwmap_rngng_allocid_set (NGPON_BCAST_ALLOC_ID_10_US_IDX, NGPON_BCAST_ALLOC_ID_10_US_VAL);

        if (xgpon_error != PON_NO_ERROR)
           return  xgpon_error;

        xgpon_error = ag_drv_ngpon_rx_bwmap_rngng_allocid_set (NGPON_BCAST_ALLOC_ID_BOTH_IDX, NGPON_BCAST_ALLOC_ID_BOTH_VAL);

        if (xgpon_error != PON_NO_ERROR)
           return xgpon_error;
 
        enable_alloc_ids = 0x7;   /* enable all 3 Alloc-IDs */
    }

    xgpon_error = ag_drv_ngpon_rx_bwmap_rngng_allocid_en_set(xi_enable_sn_alloc_id? enable_alloc_ids : 0); 
    if (xgpon_error != PON_NO_ERROR)
    {
        return xgpon_error;
    }

     return PON_NO_ERROR;
}



static uint32_t generate_random_delay ( void )
{
    PON_ERROR_DTE xgpon_error ;
    uint32_t random_delay = 0 ;
    uint32_t random_seed ;
    uint32_t us_bit_rate_in_usec ;

    /* The random delay - to avoid conflict with other ONU's - in the range of 0 - 48 [usec]  */
    xgpon_error = ag_drv_ngpon_rx_gen_random_seed_get( & random_seed ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to Generate new random seed : error %d !", xgpon_error ) ;
    }
    else
    {
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id, "Generated a New Random Seed :%lu (0x%X) !", random_seed,random_seed ) ;
        bdmf_srand ( random_seed ) ;
    }

    us_bit_rate_in_usec = g_us_rate_constants[NGPON_US_LINE_RATE_2_5G].bit_rate / MICROSEC_IN_MILISEC ;   /* ??? Need to consider NGPON2 mode */

    /* With random delay in bits : Random delay [microSec] * bit rate [bits/microSec]*/
    random_delay =  bdmf_rand16 () % (  CE_RANDOM_DELAY_MAX * us_bit_rate_in_usec ) ;

    p_log ( ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id, "Random delay %lu (0x%X) [bits]", random_delay , random_delay ) ;


    /* return random delay in bits */
    return ( random_delay ) ;
}

void generate_random_aes_key ( AES_KEY_DTE *  xo_key  )
{
    PON_ERROR_DTE xgpon_error ;
    uint32_t random_seed ;
    uint32_t i ;
    uint8_t * key_byte_ptr = ( uint8_t * ) xo_key;

    /* The random delay - to avoid conflict with other ONU's - in the range of 0 - 48 [usec]  */
    xgpon_error = ag_drv_ngpon_rx_gen_random_seed_get( & random_seed ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to Generate AES new random seed : error %d !", xgpon_error ) ;
    }
    else
    {
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id, "Generate AES new random seed : %lu (0x%X) !",random_seed, random_seed ) ;
        bdmf_srand ( random_seed ) ;
    }

    for (i = 0 ; i < CE_XGPON_DEFAULT_AES_KEY_BYTES_SIZE ; i++)
    {
        /* Generate a new Key word */
        key_byte_ptr[i] = bdmf_rand16 () & 0xFF;
    }

}

bdmf_boolean onu_serial_number_match ( uint8_t* xi_serial_number_1, uint8_t* xi_serial_number_2 )
{
    uint32_t match_diff ;

    /* Check serial number */
    match_diff = memcmp ( xi_serial_number_1 , xi_serial_number_2 , SERIAL_NUMBER_FIELD_SIZE ) ;
    if ( match_diff != 0 )
    {
         p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id, "Serial number match: NOT MATCH!"  ) ;
         p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id, "Serial number A %02X %02X %02X %02X %02X %02X %02X %02X ",
                 xi_serial_number_1 [ 0 ], xi_serial_number_1 [ 1 ], xi_serial_number_1 [ 2 ], xi_serial_number_1 [ 3 ],
                 xi_serial_number_1 [ 4 ], xi_serial_number_1 [ 5 ], xi_serial_number_1 [ 6 ], xi_serial_number_1 [ 7 ]) ;
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id, "Serial number B %02X %02X %02X %02X %02X %02X %02X %02X ",
                xi_serial_number_2 [ 0 ], xi_serial_number_2 [ 1 ], xi_serial_number_2 [ 2 ], xi_serial_number_2 [ 3 ],
                xi_serial_number_2 [ 4 ], xi_serial_number_2 [ 5 ], xi_serial_number_2 [ 6 ], xi_serial_number_2 [ 7 ]) ;
        return ( BDMF_FALSE ) ;
    }
    p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id, "Serial number match: MATCH!"  ) ;
    return ( BDMF_TRUE ) ;
}



uint32_t ngpon_set_eqd ( uint32_t xi_delay, uint8_t absolute_or_relative, uint8_t sign  )
{
    uint32_t eqd = 0 ; 
    uint32_t min_response_time = 0 ; 
    PON_ERROR_DTE xgpon_error ;
    uint32_t  frame_delay ;
    uint32_t  word_delay ; 
    uint32_t  bit_delay ; 
    uint32_t  frame_length_in_bits;
    uint32_t  bits_per_usec;

    if ((g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGPON) || 
        (g_xgpon_db.onu_params.stack_mode ==  NGPON_MODE_NGPON2_2_5G))
    { 
       frame_length_in_bits = g_us_rate_constants[NGPON_US_LINE_RATE_2_5G].bits_per_frame ;                 /* Need to consider NGPON2 mode here */
       bits_per_usec = (g_us_rate_constants[NGPON_US_LINE_RATE_2_5G].bit_rate / MICROSEC_IN_MILISEC) ;
    }
    else
    {
       frame_length_in_bits = g_us_rate_constants[NGPON_US_LINE_RATE_10G].bits_per_frame ;                 
       bits_per_usec = (g_us_rate_constants[NGPON_US_LINE_RATE_10G].bit_rate / MICROSEC_IN_MILISEC) ;
    }

    if ( absolute_or_relative == P_ABSOLUTE )
    {
       /* ( Min respose time at micro seconds ) * ( bits per micro secs ) = value in bits */
             min_response_time = (g_xgpon_db.physic_params.min_response_time) * ( bits_per_usec ); 
       p_log ( ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
               "Min response time (%lu usec )      = %7lu (0x%X) [bits]", 
               g_xgpon_db.physic_params.min_response_time, min_response_time, min_response_time  ) ;
       p_log ( ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
               "Delay w/o Min response time       = %7lu (0x%X) [bits]", xi_delay , xi_delay ) ;
        eqd = xi_delay + min_response_time ;
    }
    /* Relative Ranging Time*/
    else
    {   /* Relative Ranging Time - Positive Sign */
        if ( SIGN_POSITIVE  == sign )
        {
             p_log ( ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
                "EqD before adding %lu Bits is %7lu (0x%X) Bits",xi_delay,
                      g_xgpon_db.physic_params.eqd_in_bits , g_xgpon_db.physic_params.eqd_in_bits ) ;
             if ( g_xgpon_db.physic_params.eqd_in_bits + xi_delay <= ULONG_MAX ) 
             {
                 eqd = g_xgpon_db.physic_params.eqd_in_bits + xi_delay ;
             }
             else
             {
                 p_log ( ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
                         "EqD is too large : Current EqD=%7lu [bits] + Ranging Time=%7lu [bits]",
                          g_xgpon_db.physic_params.eqd_in_bits ,xi_delay ) ;
             }
        }
        /* Relative Ranging Time - Negative Sign */
        else 
        {
            p_log ( ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
                    "EqD before reducing %7lu Bits is %7lu (0x%X) Bits", xi_delay ,
                     g_xgpon_db.physic_params.eqd_in_bits , g_xgpon_db.physic_params.eqd_in_bits ) ;
            if ( g_xgpon_db.physic_params.eqd_in_bits > xi_delay ) 
            {
                eqd = g_xgpon_db.physic_params.eqd_in_bits - xi_delay ;
            }
            else
            {
                p_log ( ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
                        "Illegal EqD operation (Reduce) : Current EqD=%lu [bits] < Ranging Time %7lu [bits]",
                         g_xgpon_db.physic_params.eqd_in_bits ,xi_delay ) ;
            }
        }
    }

    p_log ( ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
           "EqD (Including Min response time) = %7lu (0x%X) [bits]", eqd, eqd ) ;

    /* Store EqD in bits */
    g_xgpon_db.physic_params.eqd_in_bits = eqd; 

    frame_delay = eqd / frame_length_in_bits ;
    word_delay = ( eqd % frame_length_in_bits ) / BITS_PER_WORD ;
    bit_delay = ( eqd % frame_length_in_bits ) % BITS_PER_WORD ;

#ifdef SIM_ENV
    /*
     *   Constant and known EqD for simulation environment
     */
    frame_delay = 0 ;
    word_delay = 2526 ;
    bit_delay = 0 ;
#endif

    //    if (!eqd_set)
    //  {
    xgpon_error = ag_drv_ngpon_tx_cfg_tx_rng_dly_set( word_delay, frame_delay,  bit_delay) ;
    if ( PON_NO_ERROR == xgpon_error )
    {
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
                "EqD at TX Register:Frame=%lu(0x%X),Word=%lu(0x%X),Bit=%d(0x%X)",
                frame_delay, frame_delay, word_delay, word_delay, bit_delay, bit_delay);
    }
    else

    {      
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Fail to set EDQ to HW" ) ;
        return ( xgpon_error ) ;
    }
    //      }    
    return ( PON_NO_ERROR ) ;
}

/*
 * The fuction encapsulates weird auto generated code
 */
static PON_ERROR_DTE ngpon_write_ploam_word_to_buf_1_2 (uint32_t  buffer_index, uint32_t word_index,
   uint32_t  ploam_word)
{
  PON_ERROR_DTE rc;
  
   switch (word_index)
   {
     case 0:
       rc =  ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_0_set (buffer_index, ploam_word);
     break ;

     case 1:
       rc =  ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_1_set (buffer_index, ploam_word);
     break ;

     case 2:
       rc =  ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_2_set (buffer_index, ploam_word);
     break ;

     case 3:
       rc =  ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_3_set (buffer_index, ploam_word);
     break ;

     case 4:
       rc =  ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_4_set (buffer_index, ploam_word);
     break ;

     case 5:
       rc =  ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_5_set (buffer_index, ploam_word);
     break ;

     case 6:
       rc =  ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_6_set (buffer_index, ploam_word);
     break ;

     case 7:
       rc =  ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_7_set (buffer_index, ploam_word);
     break ;

     case 8:
       rc =  ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_8_set (buffer_index, ploam_word);
     break ;

     case 9:
       rc =  ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_9_set (buffer_index, ploam_word);
     break ;

     default:
       rc = PON_ERROR_INVALID_PARAMETER;
   }

   return rc;
}


static PON_ERROR_DTE ngpon_write_ploam_to_buf (NGPON_PLOAM_TYPE  ploam_type,
    uint8_t ploam [ NGPON_PLOAM_LEN_IN_BYTES_NO_MIC ]) 
{
    uint32_t        ploam_word ;
    uint32_t        byte_index ;
    PON_ERROR_DTE   stack_error;

    if (!NGPON_PLOAM_TYPE_IN_RANGE(ploam_type)) 
      return NGPON_ERROR_PLOAM_TYPE_OUT_OF_RANGE ;

    for (byte_index=0; byte_index<NGPON_PLOAM_LEN_IN_BYTES_NO_MIC; byte_index+=BYTES_PER_WORD) 
    {
        ploam_word = ploam[byte_index+0];
        ploam_word <<= 8 ;
        ploam_word |= ploam[byte_index+1];
        ploam_word <<= 8 ;
        ploam_word |= ploam[byte_index+2];
        ploam_word <<= 8 ;
        ploam_word |= ploam[byte_index+3];
        switch (ploam_type)
        {
           case NGPON_PLOAM_TYPE_NORMAL:
              stack_error = ag_drv_ngpon_tx_ploam_0_buf_tx_plm_0_buf_plm_set(byte_index >> 2, ploam_word);
           break;

           case NGPON_PLOAM_TYPE_REGISTRATION:
              stack_error = ngpon_write_ploam_word_to_buf_1_2 (0, byte_index >> 2, ploam_word);
           break;

           case NGPON_PLOAM_TYPE_IDLE_OR_SN:
              stack_error = ngpon_write_ploam_word_to_buf_1_2 (1, byte_index >> 2, ploam_word);
           break;

           default:
              p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id, "Illegal PLOAM buffer type in %s \n\r", __FUNCTION__ ) ;
              return stack_error ;
        }
        if ( PON_NO_ERROR != stack_error)
        { 
          p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id, "Failed to write PLOAM to the buffer in %s \n\r", __FUNCTION__ ) ;
              return stack_error ;
        }
    }

    return PON_NO_ERROR ;
}


static PON_ERROR_DTE write_us_ploam(NGPON_US_PLOAM *ploam, bdmf_boolean persistent, 
    NGPON_PLOAM_TYPE type, bdmf_boolean default_key)
{
    PON_ERROR_DTE rc;
    int i;

    rc = ngpon_write_ploam_to_buf(type, (uint8_t *)ploam);
    if (rc != PON_NO_ERROR) 
        return rc;

    rc = ag_drv_ngpon_tx_cfg_plm_cfg_set(type, persistent, BDMF_TRUE, 
        BDMF_TRUE, default_key);
    if (rc != PON_NO_ERROR) 
        return rc;

    g_pon_ploam_pm_counters.tx_ploam_total_counter++;

    p_log(ge_onu_logger.sections.stack.upstream_ploam.print_ploam, 
        "US Ploam Sent: "); 
    for(i = 0; i < NGPON_PLOAM_LEN_IN_BYTES_WITH_SPC_AND_MIC; i+=8)
    {
         p_log(ge_onu_logger.sections.stack.upstream_ploam.print_ploam,
             "%02X %02X %02X %02X %02X %02X %02X %02X",
             ((uint8_t *)ploam)[i], ((uint8_t *)ploam)[i + 1], ((uint8_t *)ploam)[i + 2], ((uint8_t *)ploam)[i + 3],
             ((uint8_t *)ploam)[i + 4], ((uint8_t *)ploam)[i + 5], ((uint8_t *)ploam)[i + 6], ((uint8_t *)ploam)[i + 7]);
    }

    return PON_NO_ERROR;
}

/* Push PLOAM to SW buffer list */
static void push_us_ploam_to_list(NGPON_US_PLOAM *ploam, bdmf_boolean persistent,
    NGPON_PLOAM_TYPE type, bdmf_boolean default_key)
{
#ifndef SIM_ENV
    struct ploam_list_struct *entry;
    
    entry = kmalloc(sizeof(struct ploam_list_struct), GFP_KERNEL);
    if (!entry) 
    { 
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "ERROR: Cannot allocate list entry memory");
    }

    memcpy (&(entry->ploam), ploam, sizeof (NGPON_US_PLOAM));
    entry->persistent = persistent;
    entry->type = type;
    entry->default_key = default_key;
    spin_lock_bh(&lock_ploam_list);
    list_add_tail(&entry->list, &us_ploam_list);

    us_ploam_list_num++;
    spin_unlock_bh(&lock_ploam_list);

    /* Indicates problem sending ploams.
       Prevents the list to grow and grow and consuming resources. */
    if (unlikely(us_ploam_list_num > 1000))
    { 
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "ERROR: Problem with US PLOAM sending: US PLOAM SW list has reached 1000 entries!!!");
    }
#endif
}

/* Pop PLOAM from SW buffer list */
static int pop_us_ploam_from_list(NGPON_US_PLOAM *ploam, bdmf_boolean *persistent,
    NGPON_PLOAM_TYPE *type, bdmf_boolean *default_key)
{      
#ifndef SIM_ENV
    struct ploam_list_struct *node;

    spin_lock_bh(&lock_ploam_list);

    if (list_empty(&us_ploam_list))
    {
        spin_unlock_bh(&lock_ploam_list);
        return -1;
    }

    node = list_first_entry(&us_ploam_list, struct ploam_list_struct, list);
    list_del(&(node->list));
    us_ploam_list_num--;

    spin_unlock_bh(&lock_ploam_list);

    memcpy (ploam, &(node->ploam), sizeof(NGPON_US_PLOAM));
    *persistent = node->persistent;
    *type = node->type;
    *default_key = node->default_key;

    kfree(node);
#endif
    return 0;
}

/* Flush US PLOAMs - Remove all pending PLOAMs from SW buffer list */
static void free_us_ploam_list(void)
{      
#ifndef SIM_ENV
    struct ploam_list_struct *entry, *tmp_entry;

    list_for_each_entry_safe(entry, tmp_entry, &us_ploam_list, list)
    {
        spin_lock_bh(&lock_ploam_list);
        list_del(&entry->list);
        spin_unlock_bh(&lock_ploam_list);
        kfree(entry);

        us_ploam_list_num--;
    }

    if (unlikely(us_ploam_list_num != 0))
    {        
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "ERROR: Problem to free all pending PLOAMs from SW buffer list: us_ploam_list_num=%d", 
            us_ploam_list_num);
    }
#endif
}

static PON_ERROR_DTE send_us_ploam(NGPON_US_PLOAM *ploam, bdmf_boolean persistent,
    NGPON_PLOAM_TYPE type, bdmf_boolean default_key)
{
    PON_ERROR_DTE rc;
    ngpon_tx_cfg_plm_0_stat plm_0_stat;


    if (type != NGPON_PLOAM_TYPE_NORMAL)
    {
      /* direct write to the corresponding buffer */
      return write_us_ploam(ploam, persistent, type, default_key);
    }
    else
    /* If SW buffer is empty: Check PLOAM0 FIFO status Register */
    if (list_empty(&us_ploam_list))
    {

        rc = ag_drv_ngpon_tx_cfg_plm_0_stat_get(&plm_0_stat);
        if (rc != PON_NO_ERROR) 
        {
            p_log(ge_onu_logger.sections.stack.upstream_ploam.general_id, 
                "Cannot read PLOAM0 FIFO status Register: rc=%d", rc); 
            return rc;
        }
      
        /* If the number of valid PLOAMs entries in the PLOAM 0 FIFO is smaller 
           then 4 (OCCUPY<4), Write US PLOAM to HW buffer immediately */
        if (plm_0_stat.occupy < NGPON_TX_CFG_AND_CMD_PLM_0_STAT_OCCUPY_MAX_VALUE)
        {
            return write_us_ploam(ploam, persistent, type, default_key);
        }
        else /* HW fifo is full */
            push_us_ploam_to_list(ploam, persistent, type, default_key);
    }
    else
        push_us_ploam_to_list(ploam, persistent, type, default_key);

    return PON_NO_ERROR;
}
        
/* Function is called when an US PLOAM is transmitted:
   Tx interrrupt - tx_plm_0 is raised */
void fetch_us_ploam(void)
{
    NGPON_US_PLOAM ploam; 
    bdmf_boolean persistent;
    NGPON_PLOAM_TYPE type; 
    bdmf_boolean default_key;

    /* If US PLOAM waits at the SW buffer - read & remove from SW list */
    if (!pop_us_ploam_from_list(&ploam, &persistent, &type, &default_key))
    {

        /* If found PLOAM in SW list - Write US PLOAM to HW buffer */
        write_us_ploam(&ploam, persistent, type, default_key);
    }
}

void dump_key (uint8_t *key, uint32_t key_size_in_bytes)
{
    uint32_t i;

    for ( i = 0 ; i < key_size_in_bytes ; i+=16 )
    {
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id,
                "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
                key [ i + 0 ] , key [ i + 1 ], key [ i + 2 ], key [ i + 3 ] ,
                key [ i + 4 ] , key [ i + 5 ], key [ i + 6 ], key [ i + 7 ] ,
                key [ i + 8 ] , key [ i + 9 ] , key [ i + 10 ], key [ i + 11 ] , 
                key [ i + 12 ], key [ i + 13 ], key [ i + 14 ], key [ i + 15 ]  ) ;
    }
}

static PON_ERROR_DTE write_sn_ploam_to_hw(uint32_t random_delay) 
{
    NGPON_US_PLOAM ploam ;
    uint32_t pon_id ;
    uint32_t random_delay_net_order ;
    uint32_t pon_id_net_order ;
    int i ;

    pon_id = 0;
    pon_id_net_order = htonl(pon_id);


    memset(&ploam, 0, sizeof(NGPON_US_PLOAM));
    ploam.onu_id = htons(NO_ONU_ID_ASSIGNED_YET);    

    ploam.message_id = US_SERIAL_NUMBER_ONU_MESSAGE_ID; 
    ploam.sequence_number = DEFAULT_US_SN_ONU_PLOAM_SEQUENCE_NUM;
    memcpy(ploam.message.serial_number_onu.serial_number, 
        g_xgpon_db.onu_params.sn.serial_number, SERIAL_NUMBER_FIELD_SIZE);

    random_delay_net_order = htonl(random_delay);
    memcpy(ploam.message.serial_number_onu.random_delay, 
        &random_delay_net_order, RANDOM_DELAY_FIELD_SIZE);



    if ((g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_10G) ||
       (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_2_5G) ||
       (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGS))
    {

        pon_id = ngpon_get_pon_id() ;
        ploam.message.serial_number_onu.current_ds_pon_id = htonl(pon_id);
        ploam.message.serial_number_onu.current_us_pon_id = htonl(pon_id);


        for (i=0; i<8; i++)
        {
            /* Report Sufficient Calibration */
            ploam.message.serial_number_onu.calibration_record_status_twdm[i] = 
               (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGS)? 0x00 : 0x33;
        }
        
        /*
         * related to the upstream wavelength dithering mechanism (G.989.3 clause 17.4)
         */ 
        ploam.message.serial_number_onu.tuning_granularity = 0;
        ploam.message.serial_number_onu.step_tuning_time = 0;

        ploam.message.serial_number_onu.us_line_rate_capability.rate_2_5G_support = 
            ((g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_2_5G) || (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGS))? 1 : 0;
        ploam.message.serial_number_onu.us_line_rate_capability.rate_10G_support = 
            ((g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_10G) || (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGS))? 1 : 0;


        /*
         * Represents a requested attenuation level as a part of the power levelling instruction to an ONU
         */
        ploam.message.serial_number_onu.attenuation = 0;

        /*
         * Power levelling not supported
         */
        ploam.message.serial_number_onu.power_level_capabilty = 0;
    } 

    return send_us_ploam(&ploam, BDMF_TRUE, NGPON_PLOAM_TYPE_IDLE_OR_SN ,BDMF_TRUE);
}

static PON_ERROR_DTE write_registration_ploam_to_hw(NGPON_ONU_ID onu_id, 
    uint32_t sequence_num, bdmf_boolean persistent, NGPON_PLOAM_TYPE type) 
{
    NGPON_US_PLOAM ploam;
   
    memset(&ploam, 0, sizeof(NGPON_US_PLOAM));
    ploam.onu_id = htons(onu_id);
    ploam.message_id = US_REGISTRATION_MESSAGE_ID; 
    ploam.sequence_number = sequence_num;
    memcpy(ploam.message.registration.registration_id, 
        g_xgpon_db.onu_params.reg_id.reg_id, REGISTRATION_ID_FIELD_SIZE);

    g_pon_ploam_pm_counters.tx_ploam_regist_counter++;

    /* Persistent PLOAM , Use default PLOAM key for MIC calculation */ 

    return send_us_ploam(&ploam, persistent, type, BDMF_TRUE);
}

static PON_ERROR_DTE write_idle_ploam_to_hw(NGPON_ONU_ID onu_id) 
{
    NGPON_US_PLOAM ploam;

    memset(&ploam, 0, sizeof(NGPON_US_PLOAM));
    ploam.onu_id = htons(onu_id);    
    ploam.message_id = US_ACKNOWLEDGE_MESSAGE_ID ;
    ploam.sequence_number = 0;
    ploam.message.acknowledge.completion_code =  ACKNOWLEDGE_NO_MESSAGE;

    /* Persistent PLOAM , DO NOT Use default PLOAM key for MIC calculation */ 
    return send_us_ploam(&ploam, BDMF_TRUE, NGPON_PLOAM_TYPE_IDLE_OR_SN, BDMF_FALSE);
}


static PON_ERROR_DTE write_ack_ploam_to_hw_comp_code(NGPON_ONU_ID onu_id, uint32_t sequence_num, uint8_t completion_code) 
{
    NGPON_US_PLOAM ploam;
    bdmf_boolean default_key = BDMF_FALSE;

    memset(&ploam, 0, sizeof(NGPON_US_PLOAM));
    ploam.onu_id = htons(onu_id);    
    ploam.message_id = US_ACKNOWLEDGE_MESSAGE_ID;
    ploam.sequence_number = sequence_num;
    ploam.message.acknowledge.completion_code = completion_code;

    p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
          "====> Writing ACK PLOAM to HW");

    g_pon_ploam_pm_counters.tx_ploam_ack_counter++;

    /* Not Persistent PLOAM ,DO NOT Use default PLOAM key for MIC calculation */ 
    return send_us_ploam(&ploam, BDMF_FALSE, NGPON_PLOAM_TYPE_NORMAL, default_key);

}


static PON_ERROR_DTE write_ack_ploam_to_hw(NGPON_ONU_ID onu_id, uint32_t sequence_num) 
{
    return write_ack_ploam_to_hw_comp_code(onu_id, sequence_num, ACKNOWLEDGE_OK) ;
}


static PON_ERROR_DTE write_key_report_ploam_to_hw(NGPON_ONU_ID onu_id, 
    uint32_t sequence_num, AES_KEY_DTE *key_fragment, 
    NGPON_ENCRYP_DATA_KEY_INDEX key_index, uint32_t report_type) 
{
    NGPON_US_PLOAM ploam;
  
    memset(&ploam, 0, sizeof(NGPON_US_PLOAM));
    ploam.onu_id = htons(onu_id);    
    ploam.message_id = US_KEY_REPORT_MESSAGE_ID; 
    ploam.sequence_number = sequence_num;
    ploam.message.key_report.report_type.type = report_type;
    ploam.message.key_report.key_index.index = key_index;
    ploam.message.key_report.fragment_number.frag_num = 0;
    memset(ploam.message.key_report.key_fragment, 0, 
        KEY_FRAGMENT_FIELD_SIZE);
    memcpy(ploam.message.key_report.key_fragment, key_fragment, 
        CE_XGPON_DEFAULT_AES_KEY_BYTES_SIZE);

    p_log ( ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id,
        "write_key_report_ploam_to_hw: received: ");
    dump_key((uint8_t *)key_fragment, NGPON_DEFAULT_KEY_SIZE);
    p_log(ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id,
        "write_key_report_ploam_to_hw: send: ");
    dump_key(ploam.message.key_report.key_fragment, NGPON_DEFAULT_KEY_SIZE);

    g_pon_ploam_pm_counters.tx_ploam_key_report_counter++;

    /* Not Persistent PLOAM ,DO NOT Use default PLOAM key for MIC calculation */ 
    return send_us_ploam(&ploam, BDMF_FALSE, NGPON_PLOAM_TYPE_NORMAL, BDMF_FALSE);
}


static PON_ERROR_DTE set_burst_profile_to_hw (XGPON_BURST_PROFILE_INFO* profile)
{
   PON_ERROR_DTE xgpon_error    ;
   uint32_t msb_preamble ;
   uint32_t lsb_preamble ;
   uint32_t msb_delimiter ;
   uint32_t lsb_delimiter ;
   uint8_t fec_type ;

   ngpon_tx_profile_profile_ctrl   profile_ctrl ;

   xgpon_error =  ag_drv_ngpon_tx_profile_profile_ctrl_get (profile->profile_index, &profile_ctrl);
   if (PON_NO_ERROR != xgpon_error) 
       return xgpon_error ;
   profile_ctrl.prof_en = BDMF_FALSE ;

   xgpon_error =  ag_drv_ngpon_tx_profile_profile_ctrl_set (profile->profile_index, &profile_ctrl);
   if (PON_NO_ERROR != xgpon_error) 
       return xgpon_error ;

   if (BDMF_FALSE == g_xgpon_db.debug_params.swap_preamble_msb_lsb)
   {
       bytes_to_word (profile->preamble , &msb_preamble);
       bytes_to_word (&profile->preamble[4] , &lsb_preamble);
   }
   else
   {
       bytes_to_word (profile->preamble , &lsb_preamble);
       bytes_to_word (&profile->preamble[4] , &msb_preamble);
   }

   xgpon_error =  ag_drv_ngpon_tx_profile_preamble_0_set (profile->profile_index, msb_preamble);
   if ( PON_NO_ERROR != xgpon_error) 
       return xgpon_error ;
   xgpon_error =  ag_drv_ngpon_tx_profile_preamble_1_set (profile->profile_index, lsb_preamble);
   if ( PON_NO_ERROR != xgpon_error) 
       return xgpon_error ;

   if (BDMF_FALSE == g_xgpon_db.debug_params.swap_delimiter_msb_lsb)
   {
       bytes_to_word (profile->delimiter , &msb_delimiter);
       bytes_to_word (&profile->delimiter[4] , &lsb_delimiter);
   }
   else
   {
       bytes_to_word (profile->delimiter , &lsb_delimiter);
       bytes_to_word (&profile->delimiter[4] , &msb_delimiter);
   }


   xgpon_error = ag_drv_ngpon_tx_profile_delimiter_0_set (profile->profile_index, msb_delimiter);
   if ( PON_NO_ERROR != xgpon_error) 
       return xgpon_error ;
   xgpon_error = ag_drv_ngpon_tx_profile_delimiter_1_set (profile->profile_index, lsb_delimiter);
   if ( PON_NO_ERROR != xgpon_error) 
       return xgpon_error ;

   xgpon_error = ag_drv_ngpon_tx_profile_fec_type_set (profile->profile_index, profile->fec_type) ;
   if ( PON_NO_ERROR != xgpon_error) 
       return xgpon_error ;

   xgpon_error = ag_drv_ngpon_tx_profile_profile_ctrl_get (profile->profile_index, &profile_ctrl) ;
   if ( PON_NO_ERROR != xgpon_error) 
       return xgpon_error ;

   profile_ctrl.delimiter_len = profile->delimiter_len ;
   profile_ctrl.preamble_len  = profile->preamble_len ;
   profile_ctrl.fec_ind       = profile->fec_ind ;
   profile_ctrl.preamble_total_len = profile->preamble_len * profile->preamble_repeat_counter ;
   profile_ctrl.prof_en = BDMF_TRUE ;

   p_log(ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id, "set_burst_profile_to_hw(): index=%d, delimiter_len=%d preamble_len=%d preamble_total_len=%d prof_en=%d\n",
          profile->profile_index, 
          profile_ctrl.delimiter_len,
          profile_ctrl.preamble_len, profile_ctrl.preamble_total_len, profile_ctrl.prof_en);

   xgpon_error =  ag_drv_ngpon_tx_profile_profile_ctrl_set(profile->profile_index, &profile_ctrl) ;
   if ( PON_NO_ERROR != xgpon_error ) 
       return xgpon_error ;

   /*
    *  Correct FEC Type configuration for XGPON and NGPON2 (2.5G and 10G upstream)
    */
   switch (g_xgpon_db.onu_params.stack_mode)
   {
      case NGPON_MODE_NGPON2_10G:
      case NGPON_MODE_XGS:
         fec_type = 1;
         break;
      case NGPON_MODE_XGPON:
      case NGPON_MODE_NGPON2_2_5G:
         fec_type = 0;
         break;
      default:
        fec_type = 0;
   }

   xgpon_error = ag_drv_ngpon_tx_profile_fec_type_set(profile->profile_index,fec_type);

   return PON_NO_ERROR ;
}

static void save_burst_profile_to_db(NGPON_DS_BURST_PROFILE_PLOAM *profile)
{
   uint32_t profile_index ;

            p_log(ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id,
            "save_burst_profile_to_db()\nprofile_version: 0x%02X,profile_index: 0x%02X",
            profile->profile_index.version, profile->profile_index.index);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id,
            "fec_indiciation: 0x%02X, delimiter_length: 0x%02X",
            profile->fec_indiciation, profile->delimiter_length);

   profile_index = profile->profile_index.index;
    g_xgpon_db.link_params.burst_profiles.profiles[profile_index].profile_index = profile->profile_index.index ;
    g_xgpon_db.link_params.burst_profiles.profiles[profile_index].profile_version = profile->profile_index.version;
    g_xgpon_db.link_params.burst_profiles.profiles[profile_index].specific_line_rate = profile->profile_index.applicability_to_specific_line_rate;
    g_xgpon_db.link_params.burst_profiles.profiles[profile_index].fec_ind = profile->fec_indiciation.fec;
    g_xgpon_db.link_params.burst_profiles.profiles[profile_index].preamble_len = profile->preamble_length.length;
    g_xgpon_db.link_params.burst_profiles.profiles[profile_index].preamble_repeat_counter = profile->preamble_repeat_counter ;
    memcpy(g_xgpon_db.link_params.burst_profiles.profiles[profile_index].preamble, profile->preamble, PREAMBLE_MAX_LENGTH);
    g_xgpon_db.link_params.burst_profiles.profiles[profile_index].delimiter_len = profile->delimiter_length.length;
    /*
     *   Special treatment is required for delimiter_length equal or less than 4 bytes as explained in RDB:
     *=======>
     *   A register array of two registers that together holds 8 bytes of the delimiter.
     *   the first register (smaller address) holds the delimiters most significant bytes.
     *   Address offset | delimiterReg
     *   ----------------|---------------
     *   0 | D7_D6_D5_D4
     *   ----------------|---------------
     *   1 | D3_D2_D1_D0
     *   ----------------|---------------
     *
     *   Output to the PON
     *   First byte --------------> Last byte
     *   D7,D6,D5,D4,D3,D2,D1,D0 (if dlmg_delimiter_length=8)
     *   D3,D2,D1,D0 (if dlmg_delimiter_length=4)
     *   If the requested delimiter length is 0,1,2,3,5,6,7 then SW will write the end of the Preamble bytes into the delimiter registers in order to complete it to 4/8 bytes.
     *=======<
     * 
     */
     if (profile->delimiter_length.length > 4)
     {
         memcpy(g_xgpon_db.link_params.burst_profiles.profiles[profile_index].delimiter, profile->delimiter, profile->delimiter_length.length);
         if (profile->delimiter_length.length < DELIMITER_MAX_LENGTH)
         {
           /*
            * pad with preamble as required in RDB
            */
           memcpy(g_xgpon_db.link_params.burst_profiles.profiles[profile_index].delimiter+profile->delimiter_length.length,
                  g_xgpon_db.link_params.burst_profiles.profiles[profile_index].preamble,
                  DELIMITER_MAX_LENGTH - profile->delimiter_length.length);
         }
     }
     else /* profile->delimiter_length.length <= 4 bytes */
     {
         memset(g_xgpon_db.link_params.burst_profiles.profiles[profile_index].delimiter, 0, DELIMITER_MAX_LENGTH);
         memcpy(g_xgpon_db.link_params.burst_profiles.profiles[profile_index].delimiter+4, profile->delimiter, profile->delimiter_length.length);
         if (profile->delimiter_length.length < 4)
         {
           /*
            * pad with preamble as required in RDB
            */
            memcpy(g_xgpon_db.link_params.burst_profiles.profiles[profile_index].delimiter+profile->delimiter_length.length,
                   g_xgpon_db.link_params.burst_profiles.profiles[profile_index].preamble,
                   4 - profile->delimiter_length.length);
         }
     }
     memcpy(g_xgpon_db.link_params.burst_profiles.profiles[profile_index].pon_tag, profile->pon_tag ,PON_TAG_SIZE);
   /* Once the profile is written to HW, its state can be changed to ENABLE */
     g_xgpon_db.link_params.burst_profiles.profiles[profile_index].profile_enable = BDMF_FALSE;

     g_xgpon_db.link_params.burst_profiles.burst_profile_received = BDMF_TRUE;
}

void save_system_profile_to_db(NGPON_DS_SYSTEM_PROFILE_PLOAM *profile)
{
    memcpy(g_xgpon_db.link_params.system_profile.ng2sys_id, profile->ng2sys_id, 3);
    g_xgpon_db.link_params.system_profile.version = profile->version.version;                                
    g_xgpon_db.link_params.system_profile.us_operation_wavelength_bands_ptp_wdm = profile->us_operation_wave_band.ptp_wdm;                           
    g_xgpon_db.link_params.system_profile.us_operation_wavelength_bands_twdm = profile->us_operation_wave_band.twdm;       
    if (profile->twdm_channel_count.ch_profile_num)
    {
        g_xgpon_db.link_params.system_profile.twdm_channel_count = profile->twdm_channel_count.ch_profile_num ;
    }
    g_xgpon_db.link_params.system_profile.channel_spacing_twdm = profile->channel_spacing_twdm;                                                                           
    g_xgpon_db.link_params.system_profile.us_mse_twdm = profile->us_mse_twdm;                                                                                      
    memcpy(g_xgpon_db.link_params.system_profile.fsr_twdm, profile->fsr_twdm, 2);
    g_xgpon_db.link_params.system_profile.amcc_use_flag = profile->amcc_control.amcc_use_flag;             
    g_xgpon_db.link_params.system_profile.min_calibration_accuracy_for_activation_for_in_band = profile->amcc_control.min_calibration_accuracy_for_activation_for_in_band;  
    g_xgpon_db.link_params.system_profile.loose_calibration_bound_twdm = profile->loose_calibration_bound_twdm;                                 
    memcpy(g_xgpon_db.link_params.system_profile.ptp_wdm_channel_count, profile->ptp_wdm_channel_count, 2);
    g_xgpon_db.link_params.system_profile.channel_spacing_ptp_wdm = profile->channel_spacing_ptp_wdm;          
    g_xgpon_db.link_params.system_profile.us_mse_ptp_wdm = profile->us_mse_ptp_wdm;                   
    memcpy(g_xgpon_db.link_params.system_profile.fsr_ptp_wdm, profile->fsr_ptp_wdm, 2);                                                                      
    g_xgpon_db.link_params.system_profile.ptp_wdm_calibration_accuracy = profile->ptp_wdm_calibration.min_calibration_accuracy_for_activation;                                       
    g_xgpon_db.link_params.system_profile.loose_calibration_bound_ptp_wdm = profile->loose_calibration_bound_ptp_wdm;   
                                     
    g_xgpon_db.link_params.system_profile.system_profile_received = BDMF_TRUE;
}


void save_channel_profile_to_db(NGPON_DS_CHANNEL_PROFILE_PLOAM *profile)
{   
    int       index;

    /*
     *   According to G.989.3 channel profile index is arbitrary number; channel to channel_profile association is determined by PON ID.
     *   Therefore to find Channel_Profile for current channel we have to look up in Channel_Profile table for Channel_Profile with PON ID that matches PON ID in received
     *   Channel_Profile PLOAM  
     */

    if (g_xgpon_db.link_params.ch_profile.channel_profile_num > 0)
    {
        for (index=0; index < g_xgpon_db.link_params.ch_profile.channel_profile_num; index ++)
        {
            if (profile->twdm.pon_id == g_xgpon_db.link_params.ch_profile.channel[index].pon_id) 
            {          
                memcpy(&g_xgpon_db.link_params.ch_profile.channel[index], &(profile->twdm), sizeof(DS_CHANNEL_PROFILE_TWDM_PLOAM)); /* Ignore PtP case */
                g_xgpon_db.link_params.ch_profile.channel_profile_received = BDMF_TRUE;
                return;  
            }      
        }
    } 
    /*
     *  Profile not found, store it at free space
     */
    index = g_xgpon_db.link_params.ch_profile.channel_profile_num ++ ;

    if (index >= 16)
    {
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.channel_profile_message_id,
        "Too many different Channel_Profiles, not stored");
        return ; /* Too many profiles, ignore */
    }

    memcpy(&g_xgpon_db.link_params.ch_profile.channel[index], &(profile->twdm), sizeof(DS_CHANNEL_PROFILE_TWDM_PLOAM)); /* Ignore PtP case */

    g_xgpon_db.link_params.ch_profile.channel_profile_received = BDMF_TRUE;

        p_log ( ge_onu_logger.sections.stack.downstream_ploam.channel_profile_message_id,
                "Total num of profiles %d", g_xgpon_db.link_params.ch_profile.channel_profile_num);


}

static PON_ERROR_DTE set_tx_ploam_key( NGPON_ENCRYP_PLOAM_KEY_TYPE ploam_key_type )
{
    PON_ERROR_DTE xgpon_error ;
    uint32_t encryption_key[4]  ; 
    uint8_t tmp_cmac_k2[16] ; 
    uint32_t cmac_k2[4] ; 
    uint8_t  key_word;

    if (NGPON_ENCRYP_PLOAM_KEY_TYPE_DEFAULT == ploam_key_type) 
    {
       /*
        * Generate AES_CMAC for PLOAM MIC calculations from default PLOAM key
        */
       xgpon_error = ngpon_generate_ploam_cmac_k2_key((uint8_t *)g_xgpon_db.onu_params.ploam_default_key, tmp_cmac_k2);
       if (xgpon_error != PON_NO_ERROR)
          return xgpon_error;

       bytes_to_word (&tmp_cmac_k2[0], &cmac_k2[0]);
       bytes_to_word (&tmp_cmac_k2[4], &cmac_k2[1]);
       bytes_to_word (&tmp_cmac_k2[8], &cmac_k2[2]);
       bytes_to_word (&tmp_cmac_k2[12], &cmac_k2[3]);

       for (key_word = 0 ; key_word < 4 ; key_word ++ )
       {
          xgpon_error = ag_drv_ngpon_tx_enc_ploamik_dflt_set (key_word, g_xgpon_db.onu_params.ploam_default_key[key_word]);
          if (xgpon_error != PON_NO_ERROR)
             return xgpon_error;
       }

       xgpon_error = ag_drv_ngpon_tx_enc_cmac_k2_dflt_0_set (cmac_k2[0]) ;
       if (xgpon_error != PON_NO_ERROR)
          return xgpon_error;

       xgpon_error = ag_drv_ngpon_tx_enc_cmac_k2_dflt_1_set (cmac_k2[1]) ;
       if (xgpon_error != PON_NO_ERROR)
          return xgpon_error;

       xgpon_error = ag_drv_ngpon_tx_enc_cmac_k2_dflt_2_set (cmac_k2[2]) ;
       if (xgpon_error != PON_NO_ERROR)
          return xgpon_error;

       xgpon_error = ag_drv_ngpon_tx_enc_cmac_k2_dflt_3_set (cmac_k2[3]) ;
       if (xgpon_error != PON_NO_ERROR)
          return xgpon_error;

    }
    else
    {
       bytes_to_word (&g_xgpon_db.onu_params.ploam_ik_in_bytes[0], &encryption_key[0]);
       bytes_to_word (&g_xgpon_db.onu_params.ploam_ik_in_bytes[4], &encryption_key[1]);
       bytes_to_word (&g_xgpon_db.onu_params.ploam_ik_in_bytes[8], &encryption_key[2]);
       bytes_to_word (&g_xgpon_db.onu_params.ploam_ik_in_bytes[12], &encryption_key[3]);

       bytes_to_word (&g_xgpon_db.onu_params.ploam_cmac_k2_in_bytes[0], &cmac_k2[0]);
       bytes_to_word (&g_xgpon_db.onu_params.ploam_cmac_k2_in_bytes[4], &cmac_k2[1]);
       bytes_to_word (&g_xgpon_db.onu_params.ploam_cmac_k2_in_bytes[8], &cmac_k2[2]);
       bytes_to_word (&g_xgpon_db.onu_params.ploam_cmac_k2_in_bytes[12], &cmac_k2[3]);

       if (NGPON_ENCRYP_PLOAM_KEY_TYPE_ONU_1 == ploam_key_type)
       {
          for (key_word = 0 ; key_word < 4 ; key_word ++)
          {
             xgpon_error = ag_drv_ngpon_tx_enc_ploamik_onu1_set (key_word, encryption_key[key_word]);
             if (xgpon_error != PON_NO_ERROR)
                return ( xgpon_error ) ;
           }

          xgpon_error = ag_drv_ngpon_tx_enc_cmac_k2_onu1_0_set (cmac_k2[0]);
          if (xgpon_error != PON_NO_ERROR)
             return xgpon_error;

          xgpon_error = ag_drv_ngpon_tx_enc_cmac_k2_onu1_1_set (cmac_k2[1]);
          if (xgpon_error != PON_NO_ERROR)
             return xgpon_error;

          xgpon_error = ag_drv_ngpon_tx_enc_cmac_k2_onu1_2_set (cmac_k2[2]);
          if (xgpon_error != PON_NO_ERROR)
             return xgpon_error;

          xgpon_error = ag_drv_ngpon_tx_enc_cmac_k2_onu1_3_set (cmac_k2[3]);
          if (xgpon_error != PON_NO_ERROR)
             return xgpon_error;
       }
       else   /* NGPON_ENCRYP_PLOAM_KEY_TYPE_ONU_2 */
       {
          for (key_word = 0 ; key_word < 4 ; key_word ++ )
          {
             xgpon_error = ag_drv_ngpon_tx_enc_ploamik_onu2_set (key_word, encryption_key[key_word]);
             if (xgpon_error != PON_NO_ERROR)
                return xgpon_error;
          }
          xgpon_error = ag_drv_ngpon_tx_enc_cmac_k2_onu2_0_set (cmac_k2[0]);
          if (xgpon_error != PON_NO_ERROR)
             return xgpon_error;

          xgpon_error = ag_drv_ngpon_tx_enc_cmac_k2_onu2_1_set (cmac_k2[1]);
          if (xgpon_error != PON_NO_ERROR)
             return xgpon_error;

          xgpon_error = ag_drv_ngpon_tx_enc_cmac_k2_onu2_2_set (cmac_k2[2]);
          if (xgpon_error != PON_NO_ERROR)
             return xgpon_error;

          xgpon_error = ag_drv_ngpon_tx_enc_cmac_k2_onu2_3_set (cmac_k2[3]);
          if (xgpon_error != PON_NO_ERROR)
             return xgpon_error;

       } 
    }

    xgpon_error = ag_drv_ngpon_tx_enc_ploam_load_set ( (ploam_key_type == NGPON_ENCRYP_PLOAM_KEY_TYPE_ONU_1),
       (ploam_key_type == NGPON_ENCRYP_PLOAM_KEY_TYPE_ONU_2),
       (ploam_key_type == NGPON_ENCRYP_PLOAM_KEY_TYPE_DEFAULT)) ;
    if (xgpon_error != PON_NO_ERROR)
            return xgpon_error;

    return PON_NO_ERROR ;
}


PON_ERROR_DTE  set_rx_ploam_key( NGPON_ENCRYP_PLOAM_KEY_TYPE ploam_key_type )
{
    PON_ERROR_DTE  xgpon_error ;
    uint32_t decryption_key[4] ;
    uint8_t  tmp_cmac_k2[16] ; 
    uint32_t cmac_k2[4] ; 
    uint8_t  key_word;


    if (NGPON_ENCRYP_PLOAM_KEY_TYPE_DEFAULT == ploam_key_type) 
    {
      for (key_word = 0 ; key_word < 4 ; key_word ++)
      {
         xgpon_error = ag_drv_ngpon_rx_enc_ploamik_bcst_set (key_word, g_xgpon_db.onu_params.ploam_default_key[key_word]) ;
         if ( xgpon_error != PON_NO_ERROR )
             return ( xgpon_error ) ;

      }

      xgpon_error = ngpon_generate_ploam_cmac_k2_key ((uint8_t *)g_xgpon_db.onu_params.ploam_default_key, tmp_cmac_k2) ;
      if (xgpon_error != PON_NO_ERROR)
         return xgpon_error;

      bytes_to_word (&tmp_cmac_k2[0], &cmac_k2[0]) ;
      bytes_to_word (&tmp_cmac_k2[4], &cmac_k2[1]) ;
      bytes_to_word (&tmp_cmac_k2[8], &cmac_k2[2]) ;
      bytes_to_word (&tmp_cmac_k2[12], &cmac_k2[3]) ;

      xgpon_error = ag_drv_ngpon_rx_enc_cmack2_bcst_0_set (cmac_k2[0]) ;
      if (xgpon_error != PON_NO_ERROR)
          return xgpon_error ;

      xgpon_error = ag_drv_ngpon_rx_enc_cmack2_bcst_1_set (cmac_k2[1]) ;
      if (xgpon_error != PON_NO_ERROR)
          return xgpon_error ;

      xgpon_error = ag_drv_ngpon_rx_enc_cmack2_bcst_2_set (cmac_k2[2]) ;
      if (xgpon_error != PON_NO_ERROR)
          return xgpon_error ;

      xgpon_error = ag_drv_ngpon_rx_enc_cmack2_bcst_3_set (cmac_k2[3]) ;
      if (xgpon_error != PON_NO_ERROR)
          return xgpon_error ;
    }
    else
    {
        bytes_to_word ( & g_xgpon_db.onu_params.ploam_ik_in_bytes[0], &  decryption_key[0] );
        bytes_to_word ( & g_xgpon_db.onu_params.ploam_ik_in_bytes[4], &  decryption_key[1]  );
        bytes_to_word ( & g_xgpon_db.onu_params.ploam_ik_in_bytes[8], &  decryption_key[2] );
        bytes_to_word ( & g_xgpon_db.onu_params.ploam_ik_in_bytes[12], & decryption_key[3]  );

        bytes_to_word ( & g_xgpon_db.onu_params.ploam_cmac_k2_in_bytes[0], & cmac_k2[0] );
        bytes_to_word ( & g_xgpon_db.onu_params.ploam_cmac_k2_in_bytes[4], & cmac_k2[1]  );
        bytes_to_word ( & g_xgpon_db.onu_params.ploam_cmac_k2_in_bytes[8], & cmac_k2[2] );
        bytes_to_word ( & g_xgpon_db.onu_params.ploam_cmac_k2_in_bytes[12], & cmac_k2[3]  );

       if ( NGPON_ENCRYP_PLOAM_KEY_TYPE_ONU_1 == ploam_key_type )
       {
         for (key_word = 0 ; key_word < 4 ; key_word ++ )
         {
             xgpon_error = ag_drv_ngpon_rx_enc_ploamik_onu1_set (key_word, decryption_key[key_word]);
             if ( xgpon_error != PON_NO_ERROR )
               return ( xgpon_error ) ;

         }

         xgpon_error = ag_drv_ngpon_rx_enc_cmack2_onu1_0_set (cmac_k2[0]) ;
         if ( xgpon_error != PON_NO_ERROR )
           return ( xgpon_error ) ;

         xgpon_error = ag_drv_ngpon_rx_enc_cmack2_onu1_1_set (cmac_k2[1]) ;
         if ( xgpon_error != PON_NO_ERROR )
           return ( xgpon_error ) ;

         xgpon_error = ag_drv_ngpon_rx_enc_cmack2_onu1_2_set (cmac_k2[2]) ;
         if ( xgpon_error != PON_NO_ERROR )
           return ( xgpon_error ) ;

         xgpon_error = ag_drv_ngpon_rx_enc_cmack2_onu1_3_set (cmac_k2[3]) ;
         if ( xgpon_error != PON_NO_ERROR )
           return ( xgpon_error ) ;

       }
       else   /* NGPON_ENCRYP_PLOAM_KEY_TYPE_ONU_2 */
       {
         for (key_word = 0 ; key_word < 4 ; key_word ++ )
         {
             xgpon_error = ag_drv_ngpon_rx_enc_ploamik_onu2_set (key_word, decryption_key[key_word]) ;
             if ( xgpon_error != PON_NO_ERROR )
               return ( xgpon_error ) ;

         }
         xgpon_error = ag_drv_ngpon_rx_enc_cmack2_onu2_0_set (cmac_k2[0]) ;
         if (xgpon_error != PON_NO_ERROR)
           return xgpon_error ;

         xgpon_error = ag_drv_ngpon_rx_enc_cmack2_onu2_1_set (cmac_k2[1]) ;
         if (xgpon_error != PON_NO_ERROR)
           return xgpon_error ;

         xgpon_error = ag_drv_ngpon_rx_enc_cmack2_onu2_2_set (cmac_k2[2]) ;
         if (xgpon_error != PON_NO_ERROR)
           return xgpon_error ;

         xgpon_error = ag_drv_ngpon_rx_enc_cmack2_onu2_3_set (cmac_k2[3]) ;
         if (xgpon_error != PON_NO_ERROR)
           return xgpon_error ;

       } 

    }

    xgpon_error = ag_drv_ngpon_rx_enc_ploam_load_set ((ploam_key_type == NGPON_ENCRYP_PLOAM_KEY_TYPE_ONU_1),
        (ploam_key_type == NGPON_ENCRYP_PLOAM_KEY_TYPE_ONU_2),
        (ploam_key_type == NGPON_ENCRYP_PLOAM_KEY_TYPE_DEFAULT)) ;

    if (xgpon_error != PON_NO_ERROR)
            return xgpon_error ;

     return PON_NO_ERROR ;
}

static PON_ERROR_DTE  set_omci_keys( void )
{
    uint8_t omci_k1 [NGPON_DEFAULT_KEY_SIZE];
    uint8_t omci_k2 [NGPON_DEFAULT_KEY_SIZE];

    /* Generate K1 and K2 */
    AES_CMAC_generate_sub_keys(  g_xgpon_db.onu_params.omci_ik_in_bytes ,omci_k1 , omci_k2 );
    memcpy ( g_xgpon_db.onu_params.omci_k1, omci_k1, sizeof (omci_k1) ) ;
    memcpy ( g_xgpon_db.onu_params.omci_k2, omci_k2, sizeof (omci_k2) ) ;

    return PON_NO_ERROR ;
}


static PON_ERROR_DTE ngpon_generate_key_name_and_kek_encrypted_key ( uint8_t * xi_encryption_key , uint8_t * xi_kek ,
                                                            uint8_t * xo_key_name ,uint8_t * xo_kek_encrypted_key )
{
    AES_KEY e_engine_key ;
    uint8_t temp_data[100];

    /* encrypted key */
    AES_set_encrypt_key( xi_kek, 128, &e_engine_key) ;
    AES_encrypt(xi_encryption_key , xo_kek_encrypted_key, &e_engine_key) ;

    /* key name */
    memcpy (temp_data , xi_encryption_key , CE_XGPON_DEFAULT_AES_KEY_BYTES_SIZE ) ;
    memcpy (&temp_data[CE_XGPON_DEFAULT_AES_KEY_BYTES_SIZE] , KEY_NAME_DATA , sizeof(KEY_NAME_DATA)) ;
    AES_CMAC (xi_kek ,temp_data ,  xo_key_name , CE_XGPON_DEFAULT_AES_KEY_BYTES_SIZE + sizeof(KEY_NAME_DATA) );

    return ( PON_NO_ERROR ) ;
}

static PON_ERROR_DTE ngpon_generate_session_keys ( uint8_t * xi_msk_key , uint8_t * xi_sn ,uint8_t * xi_pon_tag ,
                                          uint8_t * xo_sk , uint8_t * xo_omci_ik , uint8_t * xo_ploam_ik ,
                                          uint8_t * xo_kek )
{
    uint8_t temp_data[100];
    memcpy (temp_data , xi_sn , SERIAL_NUMBER_FIELD_SIZE) ;
    memcpy (&temp_data[SERIAL_NUMBER_FIELD_SIZE] , xi_pon_tag , PON_TAG_SIZE) ;
    memcpy (&temp_data[ SERIAL_NUMBER_FIELD_SIZE + PON_TAG_SIZE ] , SessionK , strlen("SessionK") ) ;


    AES_CMAC (xi_msk_key ,temp_data ,  xo_sk , (SERIAL_NUMBER_FIELD_SIZE + PON_TAG_SIZE + strlen("SessionK")) );
    AES_CMAC (xo_sk ,OMCI_IK_DATA ,  xo_omci_ik , sizeof(OMCI_IK_DATA) );
    AES_CMAC (xo_sk ,PLOAM_IK_DATA , xo_ploam_ik , sizeof(PLOAM_IK_DATA) );
    AES_CMAC (xo_sk ,KEK_DATA , xo_kek , sizeof(KEK_DATA) );

    return ( PON_NO_ERROR ) ;
}

static PON_ERROR_DTE ngpon_generate_master_session_key (uint8_t * xi_registration_id , uint8_t *xo_msk  )
{
    AES_CMAC (MSK_INIT_KEY ,xi_registration_id ,  xo_msk , REGISTRATION_ID_FIELD_SIZE );
    return ( PON_NO_ERROR ) ;
}

static PON_ERROR_DTE ngpon_generate_ploam_cmac_k2_key (const uint8_t * xi_ploam_ik, uint8_t *xo_cmac_k2  )
{
    uint8_t k1 [NGPON_DEFAULT_KEY_SIZE];
    AES_CMAC_generate_sub_keys(  xi_ploam_ik ,k1 , xo_cmac_k2 );
    return ( PON_NO_ERROR ) ;
}


PON_ERROR_DTE ngpon_general_state_burst_profile_event(NGPON_DS_BURST_PROFILE_PLOAM *profile)
{ 
    PON_ERROR_DTE xgpon_error ;
    uint32_t profile_index = profile->profile_index.index;

    if (!NGPON_BURST_PROFILE_INDEX_IN_RANGE(profile_index)) 
    {
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                "Illegal profile: Profile Index: 0x%02X Profile Version: 0x%02X",
                profile->profile_index.index, profile->profile_index.version );
        return ( NGPON_ERROR_BURST_PROFILE_ID_IS_OUT_OF_RANGE ) ;
    }        

    if ((g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_10G) ||
        (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGS))
    {
       if (profile->profile_index.applicability_to_specific_line_rate == 0)
       {
           p_log (ge_onu_logger.sections.stack.downstream_ploam.general_id,
               "Profile is for 2.5Gbps upstream only - ignored : Profile Index: 0x%02X Profile Version: 0x%02X  F-flag: %d",
               profile->profile_index.index, profile->profile_index.version, profile->profile_index.applicability_to_specific_line_rate);
           return (NGPON_ERROR_INCOMPATIBLE_BURST_PROFILE) ;
       }
    }
    else  if (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGPON)
    {
       if (profile->profile_index.applicability_to_specific_line_rate == 1)
       {
           p_log (ge_onu_logger.sections.stack.downstream_ploam.general_id,
               "Profile is for 10Gbps upstream only - ignored : Profile Index: 0x%02X Profile Version: 0x%02X  F-flag: %d",
               profile->profile_index.index, profile->profile_index.version, profile->profile_index.applicability_to_specific_line_rate);
           return (NGPON_ERROR_INCOMPATIBLE_BURST_PROFILE) ;
       }
    }


    if ( ( g_xgpon_db.link_params.burst_profiles.profiles[profile_index].profile_enable == BDMF_TRUE ) &&
         ( g_xgpon_db.link_params.burst_profiles.profiles[profile_index].profile_index == profile->profile_index.index )  &&    
         ( g_xgpon_db.link_params.burst_profiles.profiles[profile_index].profile_version == profile->profile_index.version ) )                      
    {
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id,
        "Profile Index: 0x%02X Profile Version: 0x%02X is alredy configured",
         profile->profile_index.index , profile->profile_index.version );
    }
    else
    {
        /* Save TX profile in SW DB */
        save_burst_profile_to_db(profile);

        xgpon_error = set_burst_profile_to_hw ( & g_xgpon_db.link_params.burst_profiles.profiles[profile_index] );
        if ( PON_NO_ERROR != xgpon_error ) 
        {
            p_log ( ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id,
            "Unable to update profile:Error 0x%X, profile_version: 0x%02X ,profile_index: 0x%02X",
            xgpon_error , profile->profile_index.version, profile->profile_index.index );
            return ( xgpon_error ) ;
        }
        /* Update profile status at SW DB */
        g_xgpon_db.link_params.burst_profiles.profiles[profile_index].profile_enable = BDMF_TRUE ;
    }

    return PON_NO_ERROR;
}

void sm_general_set_initial_state(void)
{
    bdmf_boolean sync_state, fec_state, lcdg_state;
    PON_ERROR_DTE rc;
    sub_sm_params params;

    /* Change state */
    g_xgpon_db.link_params.operation_sm.activation_state = OPERATION_STATE_O1;
    g_xgpon_db.link_params.lcdg_state.curr_state = BDMF_FALSE;


    if ((g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGPON) &&
        (g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGS))
    {

          /* 
             Since NGPON stack does not support flexible channel allocation
             it is safe to set NGPON2 transceiver so that Tx channel matches Rx channel
 
             Seeing issue where optics are out of sync (e.g., rx channel = 1,
             tx channel = 2).  Still may have ds sync but will never range.
             Reset both to tx to match rx. 

             Usially the issues is caused by running "bs /d/o getchn" command in a loop while 
             ONU is hunting for valid channel
           */
        if (ngpon_tune_get_current_rx_channel() != ngpon_tune_get_current_tx_channel()) 
        {
            ngpon_tune_tx(ngpon_tune_get_current_rx_channel());
        }

    }


    rc = ngpon_rx_get_receiver_status(&sync_state, &fec_state, &lcdg_state);
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read receiver status, rc=%d", rc);
    }

    g_xgpon_db.link_params.sub_sm_state = o1_1_offsync_sub_state; 



    /* In case DS synchronization still atained */
    if (sync_state) 
    {   
        sm_o1_offsync_sub_state_o1_1_ds_frame_sync_event(
            (o1_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
    }

}


void sm_general_deactivate_onu_id_ploam_event(LINK_OPERATION_STATES_DTE state)
{
    PON_ERROR_DTE rc;
    PON_CONFIGURE_OMCI_PORT_ID_INDICATION omci_port_ind;

    /* Clear ONU HW Config  according to standart */
    switch (state) 
    {
    case OPERATION_STATE_O2:
    case OPERATION_STATE_O3:
        rc = clear_burst_profiles();
        break;
    case OPERATION_STATE_O4:
       rc = clear_onu_config_3();
       break;
    default:
       rc = clear_onu_config_5();
    }
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear configuration due to error %d", rc);
    }
    /*
     * Disable output in gearbox; NGPON MAC Tx will remain enabled
     */
    ngpon_wan_top_enable_transmitter(0) ;

    /* Change state */
    g_xgpon_db.link_params.operation_sm.activation_state = OPERATION_STATE_O1;

    
    sm_general_set_initial_state();

    /* Remove OMCI port */
    omci_port_ind.port_control = BDMF_FALSE;
    omci_port_ind.port_id = g_xgpon_db.onu_params.onu_id;
    rc = send_message_to_pon_task(PON_EVENT_USER_INDICATION,
       PON_INDICATION_CONFIGURE_OMCI_PORT_ID_FILTER, omci_port_ind);
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to send configure OMCI port indication to pon task: error %d", rc);
    }
    
#ifdef CONFIG_BCM_TIME_SYNC_PON_UNSTABLE
    /* Time Synchronization: PON Unstable - Set signal On/Off */
    rc = time_sync_pon_unstable_set(0);
    if (rc != PON_NO_ERROR)
    {
        p_log( ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Time Synchronizatioin PON Unstable: Failed to disable signal rc=%d", rc);
    }
#endif 

    /* Report event "Link state transition" with state parameter */
    rc = send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
        state, OPERATION_STATE_O1);
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to send Link state transition indication to pon task: error %d", rc);
    }
}

void p_sm_general_state_disable_serial_number_ploam_event(
    LINK_OPERATION_STATES_DTE *activation_state, 
    NGPON_DS_DISABLE_SERIAL_NUMBER_PLOAM *disable_sn, 
    LINK_OPERATION_STATES_DTE xi_current_state)
{
    PON_ERROR_DTE     xgpon_error ;
    bdmf_boolean      disable_discovery = BDMF_FALSE ;

    if (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_10G ||
        g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_2_5G)
    {
      if (disable_sn->control == SERIAL_NUMBER_DISCOVERY_DISABLE)
      {
        if ((xi_current_state == OPERATION_STATE_O1) ||
            (xi_current_state == OPERATION_STATE_O2) ||
            (xi_current_state == OPERATION_STATE_O3))
        {
          disable_discovery = BDMF_TRUE ;
        }
      }
    }
 
    if (disable_sn->control == SERIAL_NUMBER_BROADCAST_DISABLE || 
        (disable_sn->control == SERIAL_NUMBER_UNICAST_DISABLE && 
        onu_serial_number_match(g_xgpon_db.onu_params.sn.serial_number, 
            disable_sn->serial_number) == BDMF_TRUE)           ||
        disable_discovery)
    {
        /* At O4 - Stop TO1 */
        if (OPERATION_STATE_O4 == xi_current_state)
        {
            bdmf_timer_stop (&g_xgpon_db.os_resources.to1_timer_id ) ;
            p_log (ge_onu_logger.sections.stack.debug.timers_id, "Stopped to1 timer");
        }

        /* At O3 - Stop TOZ */
        if (OPERATION_STATE_O3 == xi_current_state)
        {
            /* Stop TOZ */
            bdmf_timer_stop(&g_xgpon_db.os_resources.toz_timer_id);
            p_log (ge_onu_logger.sections.stack.debug.timers_id, "Stopped toz timer");
        }


        /* At O9 - Stop TO5 */
        if (OPERATION_STATE_O9 == xi_current_state)
        {
            /* Stop TO5 */
            bdmf_timer_stop(&g_xgpon_db.os_resources.to5_timer_id);
            p_log (ge_onu_logger.sections.stack.debug.timers_id, "Stopped to5 timer");
        }


        /* AT O5 and O8 - Clean ONU config */
        if ((OPERATION_STATE_O5 == xi_current_state) ||
            (OPERATION_STATE_O8 == xi_current_state))
        {         
              /* Clear ONU Config   */
            xgpon_error = clear_onu_config_5 (); 
            if (xgpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to clear_onu_config due to Driver error %d !", xgpon_error);
            }
    
            /* Set Broadcast ONU Id */
            xgpon_error = ag_drv_ngpon_rx_ploam_bcst_set (BROADCAST_ONU_ID, BDMF_TRUE) ;
            if (xgpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to enable Rx Broadcast ONU-ID and its valid bit due to Driver error %d", 
                    xgpon_error);
            }
            if (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGS)
            {
                /* Disable SECONDARY RX ONU ID used for Broadcast ONU ID 0x3FE [1022] */
                xgpon_error = ag_drv_ngpon_rx_ploam_trfc_set (NGPON_ONU_INDEX_ONU_1,
                    XGS_BROADCAST_ONU_ID, BDMF_TRUE);
                if (xgpon_error !=  PON_NO_ERROR)
                {
                    p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to enable SECONDARY Rx ONU-ID and its valid bit (rx_ploam_trfc_set) %d !", xgpon_error) ;
                }
             }

        }

        /* AT O8 - Stop TO4 timer and clean config */
        if (OPERATION_STATE_O8 == xi_current_state)
        {         
            bdmf_timer_stop (&g_xgpon_db.os_resources.to4_timer_id) ;
            p_log ( ge_onu_logger.sections.stack.debug.timers_id, "Stopped to4 timer");
        }


        /* Disable output in gearbox; NGPON MAC Tx will remain enabled */
        ngpon_wan_top_enable_transmitter(0) ;
       /*
        * Disable channel hunting in O7 state
        */
        g_xgpon_db.link_params.init_tuning_done = BDMF_TRUE ;
        /* Change internal state to OPERATION_STATE_O7  */
        *activation_state = OPERATION_STATE_O7;

#ifdef CONFIG_BCM_TIME_SYNC_PON_UNSTABLE
        /* Time Synchronization: PON Unstable - Set signal Off */
        xgpon_error = time_sync_pon_unstable_set(0);
        if (xgpon_error != PON_NO_ERROR)
        {
            p_log( ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Time Synchronizatioin PON Unstable: Failed to disable signal rc=%d", 
                xgpon_error);
        }
#endif 

        /* Report event "Link state transition" with state parameter */
        xgpon_error = send_message_to_pon_task ( PON_EVENT_LINK_STATE_TRANSITION ,
            xi_current_state, OPERATION_STATE_O7);
    }
}

void p_sm_general_state_lods_event( NGPON_OPERATION_SM  * xi_operation_sm,
                                    OPERATION_SM_PARAMS * xi_operation_sm_params,
                                    NGPON_ONU_LINK_STATE_TRANSITION_INDICATION_PARAMS  xi_link_states )
{
    PON_ERROR_DTE     xgpon_error ;

    /* Change internal state to OPERATION_STATE_O1 or INTERMITTENT_LODS_O6  */
    xi_operation_sm -> activation_state = xi_link_states.onu_new_link_state ;

    /* Report event "Link state transition" with state parameter */
    xgpon_error = send_message_to_pon_task ( PON_EVENT_LINK_STATE_TRANSITION ,
        xi_link_states.onu_old_link_state, xi_link_states.onu_new_link_state);
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send_message_to_pon_task: error %d !", xgpon_error);
    }

    if ((OPERATION_STATE_O6 != xi_link_states.onu_new_link_state) &&
        (OPERATION_STATE_O5 != xi_link_states.onu_new_link_state))
    {        
        /* Clear ONU Config   */
        xgpon_error = clear_onu_config_5();
        if ( xgpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to clear_onu_config due to Driver error %d !", 
                xgpon_error);
        }

    }

    xgpon_error = send_message_to_pon_task(PON_EVENT_OAM, 
        PON_INDICATION_LOF, CE_OAM_ALARM_STATUS_ON);
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send oam indication: error %d !", xgpon_error);
    }
    ngpon_wan_top_enable_transmitter(0) ;

    if (xi_link_states.onu_new_link_state == OPERATION_STATE_O1) 
        g_xgpon_db.link_params.sub_sm_state = o1_1_offsync_sub_state;        

    /* Send OPERATION_EVENT_TIMER_DS_SYNC_CHECK_EXPIRE */
    p_operation_sm ( OPERATION_EVENT_TIMER_DS_SYNC_CHECK_EXPIRE, xi_operation_sm_params ) ;

#ifdef CONFIG_BCM_TIME_SYNC_PON_UNSTABLE
    /* Time Synchronization: PON Unstable - Set signal Off */
    xgpon_error = time_sync_pon_unstable_set(0);
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log( ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Time Synchronizatioin PON Unstable: Failed to disable signal rc=%d", 
            xgpon_error);
}
#endif 
}


void p_sm_general_state_timer_ds_sync_check_expire_event( NGPON_OPERATION_SM  * xi_operation_sm,
                                                          OPERATION_SM_PARAMS * xi_operation_sm_params )
{
    PON_ERROR_DTE       timer_start_error ;

    if ((g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGPON) &&
        (g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGS)   &&
        (!g_xgpon_db.link_params.init_tuning_done)               
       )
    {

       if (attempt_to_sync > g_xgpon_db.link_params.ticks_to_wait_before_chn_switch)    /* Time to try another channel */
       {
          /*
             If we are here - the channel is not good to work at it.
             It might be due to DS sync failure, or
             Due to issues with Channel_Profile, for example non compatible CPI value             
          */ 
          ngpon_tune_rx(0);  /* Go to next channel */
          ngpon_tune_tx(0);  /* Most chances are that the configuration will be symmetric */
          g_xgpon_db.link_params.ch_profile.current_ds_channel = ngpon_tune_get_current_rx_channel() ; 
          attempt_to_sync = 0 ;
       }
       else  /* the 'else' will not catch the case when sync_state==1, but in this case we are supposed not to return to this function - 
                the timer will be stopped by 'lof clear' interrupt handling */
       {
          attempt_to_sync ++ ;
       }
    }

    /* Start ds_sync_check timer */
    timer_start_error = bdmf_timer_start ( &g_xgpon_db.os_resources.ds_sync_check_timer_id, 
        NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.ds_sync_check_timer_timeout));
    if ( timer_start_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to start ds_sync_check timer");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, 
            "Start ds_sync_check timer ");
    }
}



static void sm_general_protection_control_ploam_message_received(NGPON_DS_PLOAM *protection)
{
    PON_ERROR_DTE rc; 

#ifdef G989_3_AMD1
    if (protection->message.protection_control.enable_flag == 0) /* enable (!) protection */
    {
       g_xgpon_db.onu_params.protect_ds_pon_id = ntohl(protection->message.protection_control.protect_ds_pon_id) ;
       g_xgpon_db.onu_params.protect_us_pon_id = ntohl(protection->message.protection_control.protect_us_pon_id) ;
       g_xgpon_db.link_params.WLCP = BDMF_TRUE ;
    }
    else 
    {
       g_xgpon_db.link_params.WLCP = BDMF_FALSE ;
    }
#else
    /* Store the pre-configured protection TWDM channel information */
    g_xgpon_db.onu_params.protect_ds_pon_id = ntohl(protection->message.protection_control.protect_ds_pon_id) ;
    g_xgpon_db.onu_params.protect_us_pon_id = ntohl(protection->message.protection_control.protect_us_pon_id) ;
    g_xgpon_db.link_params.WLCP = BDMF_TRUE ;
#endif

    /* Set an Acknowledgement PLOAM message */
    rc = write_ack_ploam_to_hw(g_xgpon_db.onu_params.onu_id,
        protection->sequence_number);
    if (rc == PON_NO_ERROR) 
    {
        p_log(ge_onu_logger.sections.stack.upstream_ploam.ack_message_id,
            "Set an Acknowledgement PLOAM message for protection ploam message (Ack Sequence Number = %lu)",
            protection->sequence_number);
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to Set an Acknowledgement PLOAM message to HW");
    }

}

static void sm_general_assign_sn_ploam_message_received(uint8_t seq_no,
    NGPON_DS_ASSIGN_ONU_ID_PLOAM *sn_ploam, LINK_OPERATION_STATES_DTE *state)
{
    PON_ERROR_DTE rc; 

    if ( BDMF_FALSE == onu_serial_number_match ( g_xgpon_db.onu_params.sn.serial_number , sn_ploam->serial_number ) )
        return;

    /* Check whether ONU ID is valid; if invalid send NACK to OLT*/
    if (sn_ploam->assigned_onu_id > MAX_VALID_ONU_ID)
    {

        /* Illegal ONU ID, send NACK to OLT */       
        rc = write_ack_ploam_to_hw_comp_code(g_xgpon_db.onu_params.onu_id, seq_no, ACKNOWLEDGE_PARAMETER_ERROR);
        if (PON_NO_ERROR == rc) 
        {
           p_log (ge_onu_logger.sections.stack.upstream_ploam.ack_message_id,
                  "Nack PLOAM for Assign ONU Id was written to HW sequence number = %lu completion_code = ACKNOWLEDGE_PARAMETER_ERROR)", seq_no);
        }
        else
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write_ack_ploam_to_hw_comp_code" );
        }
        return ; /* No further processing */

    } 
    /* If ONU-ID is not consistent */
    if (sn_ploam->assigned_onu_id != g_xgpon_db.onu_params.onu_id)
    {
        switch (*state) 
        {
        case OPERATION_STATE_O4:
           bdmf_timer_stop(&g_xgpon_db.os_resources.to1_timer_id);
           break;
        case OPERATION_STATE_O8:
           bdmf_timer_stop(&g_xgpon_db.os_resources.to4_timer_id);
           break;
        case OPERATION_STATE_O9:
           bdmf_timer_stop(&g_xgpon_db.os_resources.to5_timer_id);
           break;
        default:
           break;
        }

        /* State O4: Discard burst profiles parameters, ONU ID, 
           default Alloc-ID, default XGEM Port-ID (Paramter set 3). */
        if (*state == OPERATION_STATE_O4) 
            clear_onu_config_3();
        /* States O5/O8.2/O9: Discard burst profile parameters, ONU ID, 
           default Alloc-ID, default XGEM Port-ID, assigned Allocs, 
           Equalization delay, system profile and channel profile 
           (Paramter set 5). */
        else 
            clear_onu_config_5();

        /* Report event "Link state transition */
        rc = send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
            *state, OPERATION_STATE_O1);
        if (rc != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Failed to send PON task message");
        }

        /* Move to O1.1 sub state */
        *state = OPERATION_STATE_O1;
        g_xgpon_db.link_params.sub_sm_state = o1_1_offsync_sub_state;
    }
}

static void o9_tuning_confirmation_event(void)
{
    /* The receipt of ALLOC/KEY/Sleep US messages in Upstream tuning state defines an 
       US tuning confirmation event, causing to transition into operation state */ 

    PON_ERROR_DTE rc; 

    bdmf_timer_stop(&g_xgpon_db.os_resources.to5_timer_id);

    /* Report event "Link state transition */
    rc = send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
        OPERATION_STATE_O9, OPERATION_STATE_O5);
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to send PON task message");
    }
   
    write_idle_ploam_to_hw (g_xgpon_db.onu_params.onu_id);
    g_in_rollback = BDMF_FALSE ;    /* 
                                       Just to be on the safe side - indicate that rollback 
                                       has been completed and the state machine returns to O5
                                     */



    /* Move to O5.1 sub state */
    g_xgpon_db.link_params.operation_sm.activation_state = OPERATION_STATE_O5;
    g_xgpon_db.link_params.sub_sm_state = o5_1_associated_sub_state;
}

void p_pon_to1_timer_callback ( uint32_t xi_parameter )
{
    OPERATION_SM_PARAMS operation_sm_params ;

    p_log ( ge_onu_logger.sections.stack.debug.timers_id, "TO1 timer expired " ) ;

    p_operation_sm ( OPERATION_EVENT_TIMER_TO1_EXPIRE, & operation_sm_params ) ;

    return ;
}

void p_pon_to2_timer_callback ( uint32_t xi_parameter )
{
    OPERATION_SM_PARAMS operation_sm_params ;

    p_log ( ge_onu_logger.sections.stack.debug.timers_id, "TO2 timer expired\n " ) ;

    p_operation_sm ( OPERATION_EVENT_TIMER_TO2_EXPIRE, & operation_sm_params ) ;

    return ;
}


void p_pon_tk4_timer_callback ( uint32_t xi_parameter )
{

    p_log ( ge_onu_logger.sections.stack.debug.timers_id, "TK4 timer expired " ) ;

    p_key_sm ( KEY_EVENT_TK4_EXPIRED , g_xgpon_db.os_resources.tk4_key_index ) ;

    return ;
}

void p_pon_tk5_timer_callback ( uint32_t xi_parameter )
{

    p_log ( ge_onu_logger.sections.stack.debug.timers_id, "TK5 timer expired\n " ) ;

    p_key_sm ( KEY_EVENT_TK5_EXPIRED , g_xgpon_db.os_resources.tk5_key_index ) ;

    return ;
}


void p_pon_ds_sync_check_timer_callback ( uint32_t xi_parameter )
{
    OPERATION_SM_PARAMS operation_sm_params ;

    p_log ( ge_onu_logger.sections.stack.debug.timers_id, "ds_sync_check timer expired\n " ) ;

    p_operation_sm ( OPERATION_EVENT_TIMER_DS_SYNC_CHECK_EXPIRE, & operation_sm_params ) ;

    return ;
}

void toz_timer_callback(uint32_t params)
{
    OPERATION_SM_PARAMS operation_sm_params;

    p_log(ge_onu_logger.sections.stack.debug.timers_id, "TOZ timer expired");
    p_operation_sm(OPERATION_EVENT_TIMER_TOZ_EXPIRE, &operation_sm_params);
}

void to3_timer_callback(uint32_t params)
{
    OPERATION_SM_PARAMS operation_sm_params;

    p_log(ge_onu_logger.sections.stack.debug.timers_id, "TO3 timer expired");
    p_operation_sm(OPERATION_EVENT_TIMER_TO3_EXPIRE, &operation_sm_params);
}

void to4_timer_callback(uint32_t params)
{
    OPERATION_SM_PARAMS operation_sm_params;

    p_log(ge_onu_logger.sections.stack.debug.timers_id, "TO4 timer expired");
    p_operation_sm(OPERATION_EVENT_TIMER_TO4_EXPIRE, &operation_sm_params);
}


void to5_timer_callback(uint32_t params)
{
    OPERATION_SM_PARAMS operation_sm_params;

    p_log(ge_onu_logger.sections.stack.debug.timers_id, "TO5 timer expired");
    p_operation_sm(OPERATION_EVENT_TIMER_TO5_EXPIRE, &operation_sm_params);
}


#ifdef G989_3_AMD1
void tcpi_timer_callback(uint32_t params)
{
   g_xgpon_db.link_params.ch_profile.current_cpi = 0;
   /* Activate task in shared workqueue to store to scratchpad */
   schedule_work(&ngpon2_store_cpi);   
}
#endif


static void p_operation_invalid_cross ( NGPON_OPERATION_SM * xi_operation_state_machine, OPERATION_SM_PARAMS * xi_operation_sm_params )
{
    /* Invalid cross */
    p_log ( ge_onu_logger.sections.stack.operating.invoke_state_machine_id, "Activation state machine:Invalid cross in state %d", 
       xi_operation_state_machine->activation_state ) ;
}

static void p_operation_ignore_event ( NGPON_OPERATION_SM * xi_operation_state_machine, OPERATION_SM_PARAMS * xi_operation_sm_params )
{

}


static void sub_sm_o8_invalid_cross(o8_sub_states *sub_state, sub_sm_params *params)
{
    p_log(ge_onu_logger.sections.stack.operating.invoke_state_machine_id, "O8 Sub SM:Invalid cross");
}

static void sub_sm_o1_invalid_cross(o1_sub_states *sub_state, sub_sm_params *params)
{
    p_log(ge_onu_logger.sections.stack.operating.invoke_state_machine_id, "O1 Sub SM:Invalid cross");
}

static void sub_sm_o5_invalid_cross(o5_sub_states *sub_state, sub_sm_params *params)
{
    p_log(ge_onu_logger.sections.stack.operating.invoke_state_machine_id, "O5 Sub SM:Invalid cross");
}

static void p_sm_init_o1_state_timer_ds_sync_check_expire_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    p_sm_general_state_timer_ds_sync_check_expire_event (xi_operation_sm , xi_operation_sm_params );
}

static void p_sm_init_o1_state_ds_frame_sync_event( NGPON_OPERATION_SM * xi_operation_sm,
                                                    OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    sub_sm_params params;
    PON_ERROR_DTE xgpon_error;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    g_xgpon_db.link_params.sub_sm_state = o1_1_offsync_sub_state;
    o1_sub_sm[g_xgpon_db.link_params.sub_sm_state][o1_ds_frame_sync_event]((o1_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);

    xgpon_error = send_message_to_pon_task ( PON_EVENT_OAM , PON_INDICATION_LOF, CE_OAM_ALARM_STATUS_OFF ) ;
    if (xgpon_error != BDMF_ERR_OK)
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send oam indication: error %d !", xgpon_error ) ;

}

static void sm_o1_offsync_sub_state_o1_1_ds_frame_sync_event(o1_sub_states *sub_state, sub_sm_params *params)
{
    PON_ERROR_DTE xgpon_error;

    bdmf_timer_stop(&g_xgpon_db.os_resources.ds_sync_check_timer_id);

    /* Clear System Profiles and Channels profiles (Paramter Set 7) */
    clear_onu_config_7();

    /* Flush US PLOAM SW Fifo */
    free_us_ploam_list();

    /* Clear RX PM counters */
    xgpon_error = ngpon_get_all_rx_counters(&rx_counters);
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear Rx unit PM counters due to driver error %d", xgpon_error);
    }

    /* Set Broadcast ONU ID */
    xgpon_error = ag_drv_ngpon_rx_ploam_bcst_set(BROADCAST_ONU_ID, BDMF_TRUE);
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to enable Rx Broadcast ONU-ID and its valid bit due to driver error %d", xgpon_error);
    }

    if (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGS)
    {
        /* Disable SECONDARY RX ONU ID used for Broadcast ONU ID 0x3FE [1022] */
        xgpon_error = ag_drv_ngpon_rx_ploam_trfc_set (NGPON_ONU_INDEX_ONU_1,
            XGS_BROADCAST_ONU_ID, BDMF_TRUE);
        if (xgpon_error !=  PON_NO_ERROR)
        {
          p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to enable SECONDARY Rx ONU-ID and its valid bit (rx_ploam_trfc_set) %d !", xgpon_error) ;
        }
    }

    /* Enable SN Alloc ID - 0x3FF */
    xgpon_error = control_sn_alloc_id(BDMF_TRUE);
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set Ranging alloc id due to driver error %d", xgpon_error);
    }

    /* Associate SN Alloc ID 0x3FF (index 0 of the 4 "ranging" alloc ids) to TCONT 40 */
    xgpon_error = ag_drv_ngpon_rx_bwmap_rngng_tcnt_assoc_set(NGPON_SN_TCONT_ID_VALUE, 
        NGPON_SN_TCONT_ID_VALUE, NGPON_SN_TCONT_ID_VALUE, NGPON_SN_TCONT_ID_VALUE);
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set_ranging_alloc_id_to_tcont_association due to driver error %d", xgpon_error);
    }

    xgpon_error = set_tx_ploam_key(NGPON_ENCRYP_PLOAM_KEY_TYPE_DEFAULT);
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to set default Tx ploam_key: error %d", xgpon_error);
    }
    xgpon_error = set_rx_ploam_key(NGPON_ENCRYP_PLOAM_KEY_TYPE_DEFAULT);
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set default Rx ploam_key: error %d", xgpon_error);
    }
    ngpon_wan_post_init (0); 
    /* Move to O1.2 sub state */
    g_xgpon_db.link_params.sub_sm_state = o1_2_profile_learning_sub_state;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link Sub State transition: o1_1_offsync_sub_state -> o1_2_profile_learning_sub_state");
}

static void p_sm_init_o1_state_channel_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
    {
    sub_sm_params params;
    
    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o1_sub_sm[g_xgpon_db.link_params.sub_sm_state][o1_channel_profile_ploam_event]((o1_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
    }

static void p_sm_init_o1_state_system_profile_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
    {
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o1_sub_sm[g_xgpon_db.link_params.sub_sm_state][o1_system_profile_ploam_event]((o1_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
}

static void p_sm_init_o1_state_burst_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o1_sub_sm[g_xgpon_db.link_params.sub_sm_state][burst_profile_ploam_event]((o1_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
}
    
static void p_sm_init_o1_state_detect_lods_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
    {
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o1_sub_sm[g_xgpon_db.link_params.sub_sm_state][o1_detect_lods_event]((o1_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
    }

static void p_sm_init_o1_state_disable_serial_number_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
    {
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o1_sub_sm[g_xgpon_db.link_params.sub_sm_state][o1_disable_serial_number_ploam_event]((o1_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
    }

static void sm_sn_pre_profile_move_to_o3(void) 
    {
    PON_ERROR_DTE rc;
    int i;
    uint8_t msk[NGPON_DEFAULT_KEY_SIZE];  /* MSK (Master session Key) */

    rc = ngpon_generate_master_session_key(g_xgpon_db.onu_params.reg_id.reg_id, msk);
    if (rc != PON_NO_ERROR) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
        "Unable to generate_master_session_key");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.general_id, "Master Session Key: ");
         dump_key( msk , NGPON_DEFAULT_KEY_SIZE);
    }

    /* Load previously PON TAG from burst profile */
    for (i = 0; i <= NGPON_BURST_PROFILE_INDEX_HIGH; i++)
    {
        if (g_xgpon_db.link_params.burst_profiles.profiles[i].profile_enable == BDMF_TRUE)
            break;
    }

    rc = ngpon_generate_session_keys(msk, g_xgpon_db.onu_params.sn.serial_number,
        g_xgpon_db.link_params.burst_profiles.profiles[i].pon_tag, g_xgpon_db.onu_params.sk_in_bytes, 
        g_xgpon_db.onu_params.omci_ik_in_bytes, g_xgpon_db.onu_params.ploam_ik_in_bytes, 
                                                g_xgpon_db.onu_params.kek_in_bytes );
    if (rc != PON_NO_ERROR) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to generate session_key");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.general_id, "Ploam IK: ");
         dump_key( g_xgpon_db.onu_params.ploam_ik_in_bytes , NGPON_DEFAULT_KEY_SIZE );
    }

    rc = ngpon_generate_ploam_cmac_k2_key(g_xgpon_db.onu_params.ploam_ik_in_bytes,
                                                g_xgpon_db.onu_params.ploam_cmac_k2_in_bytes );
    if (rc != PON_NO_ERROR) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
        "Unable to generate PLOAM cmac_k2_key");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.general_id, "PLOAM CMAC_K2 Key: ");
         dump_key( g_xgpon_db.onu_params.ploam_cmac_k2_in_bytes , NGPON_DEFAULT_KEY_SIZE );
    }
#ifdef G989_3_AMD1
    g_xgpon_db.os_resources.tcpi_timer_is_running = BDMF_FALSE;
    bdmf_timer_stop(&g_xgpon_db.os_resources.tcpi_timer_id);
#endif


    /* Change internal state to SN_POST_PROFILE_O3  */
    g_xgpon_db.link_params.operation_sm.activation_state = OPERATION_STATE_O3;
   
    /* Report event "Link state transition" with state parameter */
    rc = send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
        OPERATION_STATE_O2, OPERATION_STATE_O3);
}

uint32_t ngpon_get_pon_id(void)
{
    uint16_t admin_label_msb ;
    uint16_t tol ;
    uint8_t  dwlch_id ;
    uint32_t admin_label_lsb;
    int rc ;
    uint32_t admin_label ;
    uint32_t pon_id ;

    bdmf_boolean pit_re;
    uint8_t pit_rsvd, pit_odn_class;

    rc = ag_drv_ngpon_rx_gen_sync_ponid_ms_get(&admin_label_msb, &pit_rsvd, &pit_odn_class, &pit_re);
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id, "Cannot read PONID MS from HW");     
            return 0xFFFFFFFF ;
    }

    rc = ag_drv_ngpon_rx_gen_sync_ponid_ls_get(&tol, &dwlch_id, &admin_label_lsb);
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id, "Cannot read PONID LS from HW");     
        return 0xFFFFFFFF;
    }

    admin_label = (admin_label_msb << 17) | admin_label_lsb ;
    pon_id = (admin_label << 4) | (dwlch_id & 0x0F) ;
    
    return pon_id;
}

static PON_ERROR_DTE ngpon_find_channel_profile_by_pon_id (uint32_t pon_id, DS_CHANNEL_PROFILE_TWDM_PLOAM **channel_profile_p)
{
   uint8_t  current_channel_idx ;
   uint8_t  chan_count =  g_xgpon_db.link_params.ch_profile.channel_profile_num ;

   *channel_profile_p = NULL;


   for (current_channel_idx = 0; current_channel_idx < chan_count; current_channel_idx++)
   {
       /*
        * compare PON ID from Channel_Profile PLOAM with that from Rx MAC (read from PSBd) or from Tuning_Control PLOAM
        */
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
           "In Channel Profile: Looking up  PON ID 0x%08X PON ID from Channel_Profile=0x%08X", pon_id, ntohl(g_xgpon_db.link_params.ch_profile.channel[current_channel_idx].pon_id));     

       if (pon_id == ntohl(g_xgpon_db.link_params.ch_profile.channel[current_channel_idx].pon_id))
       {
           *channel_profile_p = &g_xgpon_db.link_params.ch_profile.channel[current_channel_idx] ;
           /*
            * Channel profile found, exiting
            */        
           return PON_NO_ERROR ;
       }
   }
   return PON_GENERAL_ERROR;
}

/*
 * If both current Channel Partition Index and CPI received in Channel_Profile PLOAM
 * are non zero we have to compare it to decide whether ONU can tune to the channel 
 * referenced by Channel_Profile
 */ 
static PON_ERROR_DTE ngpon_is_cpi_ok (uint8_t channel_profile_cpi)
{

#ifdef G989_3_AMD1
    /*
     * An ONU with a specific CPI may activate only on channels whose Channel_Profile CPI matches the CPI of the ONU. 
     */
    if (g_xgpon_db.link_params.ch_profile.current_cpi)
#else
    if (g_xgpon_db.link_params.ch_profile.current_cpi && channel_profile_cpi)
#endif
    {


        if (g_xgpon_db.link_params.ch_profile.current_cpi != channel_profile_cpi)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                  "Checking - incorrect CPI: current CPI %d channel profile CPI %d", g_xgpon_db.link_params.ch_profile.current_cpi, channel_profile_cpi);     

            return PON_GENERAL_ERROR ;
        }
    }

    return PON_NO_ERROR ;
}

static PON_ERROR_DTE ngpon_general_chan_profile_ds_check (DS_CHANNEL_PROFILE_TWDM_PLOAM *channel_profile_p)
{

    if (channel_profile_p == NULL)  /* To be on the safe side */
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
           "Checking - invalid pointer");     

       return PON_GENERAL_ERROR ;
    }
   /*
    *  Profile is not void? Marked as 'this' channel profile? Then we can use it
    */
   if (channel_profile_p->control.ds_void_indicator)
   {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
           "Checking - DS Void");     
       return PON_GENERAL_ERROR ;
   }
   /*
    *  Downstream rate shall be 10Gbps
    */
   if (channel_profile_p->ds_rate.ds_tc_layer_line_rate != 0)
   {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
           "Checking - invalid DS rate");     
       return PON_GENERAL_ERROR ;
   } 
#if 0  /* Not implemented by Maple yet */
   /*
    *  In Band S/N grants shall be supported
    */
   if (!channel_profile_p->sn_grant_type.in_band_sn_grants)
   {
       return PON_GENERAL_ERROR ;
   } 
#endif
   if (PON_NO_ERROR != ngpon_is_cpi_ok(channel_profile_p->channel_partition.index))
   {
       return PON_GENERAL_ERROR ;
   } 
   /*
    *   Everything OK, channel can be used
    */
   return PON_NO_ERROR ;
}

static PON_ERROR_DTE ngpon_tuning_control_chan_profile_ds_check(DS_CHANNEL_PROFILE_TWDM_PLOAM *channel_profile_p)
{
    PON_ERROR_DTE rc ;
    uint32_t      dwlch ;

    rc = ngpon_general_chan_profile_ds_check (channel_profile_p) ;
    if (rc != PON_NO_ERROR)
    {
        return rc;
    }

    dwlch = ntohl(channel_profile_p->pon_id) & 0x0F ;
    if (dwlch > g_xgpon_db.link_params.system_profile.twdm_channel_count)
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
             "Checking - illegal channel count - from profile %d", dwlch);     

        return PON_GENERAL_ERROR ;
    }

    return PON_NO_ERROR ;
}

static PON_ERROR_DTE ngpon_tuning_control_chan_profile_us_check(DS_CHANNEL_PROFILE_TWDM_PLOAM *channel_profile_p)
{
    if (channel_profile_p == NULL)  /* To be on the safe side */
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
           "Channel Profile verification - invalid pointer to US profile");     
       return PON_GENERAL_ERROR ;
    }
   /*
    *  Profile is not void? Marked as 'this' channel profile? Then we can use it
    */
   if (channel_profile_p->control.us_void_indicator)
   {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
           "Channel Profile verification - US void");     

       return PON_GENERAL_ERROR ;
   }

   /*
    *  Upstream rate 10Gbps shall be supported
    */
   if (!channel_profile_p->us_rate.rate_10G_support)
   {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
           "Channel Profile verification - US rate 10Gbps not supported");     
       return PON_GENERAL_ERROR ;
   } 

   if (channel_profile_p->uwlch_id.channel_id > g_xgpon_db.link_params.system_profile.twdm_channel_count)
   {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
             "Channel Profile verification - invalid US channel %d", channel_profile_p->uwlch_id.channel_id);     
       return PON_GENERAL_ERROR ;
   } 
   /*
    *   Everything OK, channel can be used
    */
   return PON_NO_ERROR ;
}


static PON_ERROR_DTE ngpon_init_analyze_sys_chan_profiles(void)
{
   uint8_t  chan_count ;
   uint32_t rx_hw_pon_id ;
   DS_CHANNEL_PROFILE_TWDM_PLOAM *channel_profile_p = NULL; 
   PON_ERROR_DTE rc ;

  /*
   * Analyze System Profile fields
   */
   chan_count = g_xgpon_db.link_params.system_profile.twdm_channel_count ;

   ngpon_tune_set_max_chn (4 /* chan_count */) ;  /* Meanwhile hardcoded */

   g_xgpon_db.link_params.ch_profile.current_ds_channel = ngpon_tune_get_current_rx_channel() ;
 
   /*
    *   Get PON ID from Rx MAC
    */
   rx_hw_pon_id = ngpon_get_pon_id () ; 

   /*
    * Analyze Channel Profile fields
    */
   rc = ngpon_find_channel_profile_by_pon_id(rx_hw_pon_id, &channel_profile_p) ;
   /*
    *   Corresponding Channel_Profile found ?
    */
   if (rc != PON_NO_ERROR)
   {
       return rc;
   }
   /*
    * Check the profile
    */
   rc = ngpon_general_chan_profile_ds_check(channel_profile_p) ;
   if (rc != PON_NO_ERROR)
   {
       return rc;
   }
   /*
    * Something gone wrong - profile with correct PON ID has no 'this_profile' indicator set
    */
   if (!channel_profile_p->control.this_channel_indicator)
   {
       return PON_GENERAL_ERROR ;
   }
 

   g_xgpon_db.link_params.ch_profile.current_us_channel = channel_profile_p->uwlch_id.channel_id + 1 ;   /* Store required upstream channel */


   p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id,
               "ONU CPI is  %d  CPI indicated in Channel_Profile PLOAM is %d", g_xgpon_db.link_params.ch_profile.current_cpi, channel_profile_p->channel_partition.index);

   if ((channel_profile_p->channel_partition.index) && 
       (g_xgpon_db.link_params.ch_profile.current_cpi != channel_profile_p->channel_partition.index))
   {
      /* Store Channel Partition Indicator */
      g_xgpon_db.link_params.ch_profile.current_cpi = channel_profile_p->channel_partition.index ;      
      /* Activate task in shared workqueue to store to scratchpad */
      schedule_work(&ngpon2_store_cpi);
   }
   
   p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
       "Current US Channel - %d", g_xgpon_db.link_params.ch_profile.current_us_channel);     

   return  PON_NO_ERROR;
   
}
 

static void sm_o1_profile_learning_sub_state_o1_2_dwlch_ok_to_work(sub_sm_params *params)
{
    PON_ERROR_DTE rc; 
    PON_ERROR_DTE timer_start_error;

    if (g_xgpon_db.link_params.burst_profiles.burst_profile_received == BDMF_FALSE ||
        g_xgpon_db.link_params.system_profile.system_profile_received == BDMF_FALSE ||
        g_xgpon_db.link_params.ch_profile.channel_profile_received == BDMF_FALSE)
    {
         p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id,
               "DWLCH is not OK to work burst_profiles %d system profiles %d channel profiles %d: Not all profiles received",
               g_xgpon_db.link_params.burst_profiles.burst_profile_received,
               g_xgpon_db.link_params.system_profile.system_profile_received,
               g_xgpon_db.link_params.ch_profile.channel_profile_received);     
         return;
    }


    rc = ngpon_enable_transmitter(BDMF_TRUE);
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to Set Tx state due to error %d", rc);
    }
    g_xgpon_db.tx_params.lbe_invert_bit = gpon_ngpon_get_lbe_invert_bit_val () ; /* pluggable TRX might be replaced */
    /*
     * Enable output in gearbox; NGPON MAC is fully initialized and receives rx clock
     */
    ngpon_wan_top_enable_transmitter(1);
    gpon_ngpon_wan_top_set_lbe_invert(g_xgpon_db.tx_params.lbe_invert_bit) ;
    config_transceiver_dv () ;

    rc = generate_sn_ploam();
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to generate SN PLAOM, error %d", rc);
    }

#ifdef G989_3_AMD1
        if (!g_xgpon_db.os_resources.tcpi_timer_is_running)
        {
             timer_start_error = bdmf_timer_start (&g_xgpon_db.os_resources.tcpi_timer_id, 
                  NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.tcpi_tout));
             g_xgpon_db.os_resources.tcpi_timer_is_running = BDMF_TRUE;
        }
#endif

    /* Start Discovery timer TOZ */
    if ((g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGPON) &&
        (g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGS))
    {
      uint8_t us_channel ;

      rc = ngpon_init_analyze_sys_chan_profiles() ;
      if (rc != PON_NO_ERROR)
      {
          p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
              "No valid Channel Profile received, error %d", rc);

          /*
           * IF all channel profiles were received AND channel profile scan in 
           * ngpon_init_analyze_sys_chan_profiles() reported that there is no
           * valid Channel_Profile for current channel
           * THEN - activate channel hunting
           */ 
          if (g_xgpon_db.link_params.ch_profile.channel_profile_num >= 
             g_xgpon_db.link_params.system_profile.twdm_channel_count)
          { 
              /* Go back to No Sync substate to look for another channel */
              g_xgpon_db.link_params.sub_sm_state = o1_1_offsync_sub_state;  

              /*
               *  Start ds_sync_check_timer - channel hunting mechanism is propagated by ticks of this timer
               */
              timer_start_error = bdmf_timer_start ( &g_xgpon_db.os_resources.ds_sync_check_timer_id, 
                 NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.ds_sync_check_timer_timeout));
              if (timer_start_error != PON_NO_ERROR)
              {
                  p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                     "Unable to start ds_sync_check timer");
              }
          }
          return;
      }
      if (!g_xgpon_db.link_params.init_tuning_done)
      {
         us_channel = g_xgpon_db.link_params.ch_profile.current_us_channel? g_xgpon_db.link_params.ch_profile.current_us_channel :
           g_xgpon_db.link_params.ch_profile.current_ds_channel ;

         p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "TRX will be tuned to Tx (US) channel %d", us_channel);


         if (us_channel != g_xgpon_db.link_params.ch_profile.current_ds_channel) /* If equal - we're already tuned */
         {
           /*
            * Assymetric channel: ds_channel != us_channel ... let's try
            */
            rc = ngpon_tune_tx (us_channel) ;      
            if (rc != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                      "Failed to tune to channel %d, error %d", us_channel, rc);
             }
         }
         g_xgpon_db.link_params.init_tuning_done = BDMF_TRUE ;
      }

      rc = bdmf_timer_start(&g_xgpon_db.os_resources.toz_timer_id, 
         NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.toz_timer_timeout));
      if (rc != BDMF_ERR_OK)
      {
         p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to start TOZ timer");
      }
      else
      {
         p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start TOZ timer");
      }
    }
    /* Change link state to Standby (O2) */
    g_xgpon_db.link_params.operation_sm.activation_state = OPERATION_STATE_O2;

    /* Report event "Link state transition" with state parameter */
    rc = send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
        OPERATION_STATE_O1, OPERATION_STATE_O2);
    if (rc != BDMF_ERR_OK)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d", rc);
    }

    /* Move to O3 */
    sm_sn_pre_profile_move_to_o3();
}

static void sm_o1_profile_learning_sub_state_o1_2_system_profile_ploam_event(o1_sub_states *sub_state, sub_sm_params *params)
{
    save_system_profile_to_db(&params->p_ploam_message->message.system_profile);
    sm_o1_profile_learning_sub_state_o1_2_dwlch_ok_to_work(params);
}

static void sm_o1_profile_learning_sub_state_o1_2_burst_profile_ploam_event(o1_sub_states *sub_state, sub_sm_params *params)
{
    if (ngpon_general_state_burst_profile_event(&params->p_ploam_message->message.burst_profile))
        return;

    sm_o1_profile_learning_sub_state_o1_2_dwlch_ok_to_work(params);
}

static void sm_o1_profile_learning_sub_state_o1_2_channel_profile_ploam_event(o1_sub_states *sub_state, sub_sm_params *params)
{
    save_channel_profile_to_db(&params->p_ploam_message->message.channel_profile);
    sm_o1_profile_learning_sub_state_o1_2_dwlch_ok_to_work(params);
}

static void sm_o1_profile_learning_sub_state_o1_2_detect_lods_event(o1_sub_states *sub_state, sub_sm_params *params)
{
    int rc;
    PON_ERROR_DTE timer_start_error;

    /* Discard burst profile parameters (Set 1) */
    rc = clear_burst_profiles();
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to discard busrt profiles, error %d", rc);
    }
    
    /* Move to O1.1 sub state */
    g_xgpon_db.link_params.sub_sm_state = o1_1_offsync_sub_state;

    /* Start ds_sync_check timer */
    timer_start_error = bdmf_timer_start ( &g_xgpon_db.os_resources.ds_sync_check_timer_id, 
        NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.ds_sync_check_timer_timeout));
    if ( timer_start_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to start ds_sync_check timer");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, 
            "Start ds_sync_check timer ");
    }


    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link Sub State transition: o1_2_profile_learning_sub_state -> o1_1_offsync_sub_state");
}

static void sm_o1_profile_learning_sub_state_o1_2_disable_serial_number_ploam_event(o1_sub_states *sub_state, sub_sm_params *params)
{
    p_sm_general_state_disable_serial_number_ploam_event(&g_xgpon_db.link_params.operation_sm.activation_state, 
        &params->p_ploam_message->message.disable_serial_number, OPERATION_STATE_O1);
}

static void p_sm_sn_pre_profile_o2_state_burst_profile_ploam_event(NGPON_OPERATION_SM  *xi_operation_sm,
    OPERATION_SM_PARAMS *xi_operation_sm_params) 
{
    PON_ERROR_DTE rc;

    /*
     * Re-generate S/N PLOAM with new random_delay
     */
    rc = generate_sn_ploam () ;
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to regenerate SN PLOAM in O2 state, error %d", rc);
    }

    rc = ngpon_general_state_burst_profile_event(&xi_operation_sm_params->ploam_message_ptr->message.burst_profile);
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to handle busrt profile, error %d", rc);
    }

    if (g_xgpon_db.optics_type == BP_GPON_OPTICS_TYPE_PMD)
    {
            /*rc = ag_drv_ngpon_tx_cfg_dv_setup_pat_set ( g_xgpon_db.physic_params.transceiver_dv_setup_pattern  );
            if (rc != PON_NO_ERROR)
            {
                p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_dv_setup_pattern ,  error %d !", rc ) ;
            }
            rc = ag_drv_ngpon_tx_cfg_dv_hold_pat_set ( g_xgpon_db.physic_params.transceiver_dv_hold_pattern  );
            if (rc != PON_NO_ERROR)
            {
                p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_dv_hold_pattern ,  error %d !", rc ) ;
            }*/
        //rc =  ag_drv_ngpon_tx_cfg_dv_cfg_set ( &tx_dv_cfg ) ;
    }
}

static void p_sm_sn_pre_profile_o2_state_deactivate_onu_id_ploam_event(NGPON_OPERATION_SM *xi_operation_sm, 
    OPERATION_SM_PARAMS *xi_operation_sm_params) 
{
    bdmf_timer_stop(&g_xgpon_db.os_resources.toz_timer_id);
    sm_general_deactivate_onu_id_ploam_event(OPERATION_STATE_O2);
}


static void p_sm_sn_pre_profile_o2_state_disable_serial_number_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params) 
{
    /* Stop TOZ */
    bdmf_timer_stop(&g_xgpon_db.os_resources.toz_timer_id);
    
    p_sm_general_state_disable_serial_number_ploam_event(&xi_operation_sm->activation_state, 
        &xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number,
                                                          OPERATION_STATE_O2 );
}


static void p_sm_sn_pre_profile_o2_state_detect_lods_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    NGPON_ONU_LINK_STATE_TRANSITION_INDICATION_PARAMS  link_states;
    link_states.onu_old_link_state = OPERATION_STATE_O2 ;
    link_states.onu_new_link_state = OPERATION_STATE_O1;
    p_sm_general_state_lods_event( xi_operation_sm, xi_operation_sm_params ,link_states );
}



static void p_sm_sn_post_profile_o3_state_burst_profile_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    PON_ERROR_DTE  xgpon_error ;
 
    /*
     * Re-generate S/N PLOAM with new random_delay
     */
    xgpon_error = generate_sn_ploam () ;
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to regenerate SN PLOAM in O2 state, error %d", xgpon_error);
    }
   
    xgpon_error = ngpon_general_state_burst_profile_event(&xi_operation_sm_params->ploam_message_ptr->message.burst_profile);
    if ( PON_NO_ERROR != xgpon_error  )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                 "Unable to handle busrt profile, error %d !", xgpon_error ) ;
    }

    if (g_xgpon_db.optics_type == BP_GPON_OPTICS_TYPE_PMD)
    {
        xgpon_error = ag_drv_ngpon_tx_cfg_dv_setup_pat_set ( g_xgpon_db.physic_params.transceiver_dv_setup_pattern  );
            if (xgpon_error != PON_NO_ERROR)
            {
                /* Log */
                p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_dv_setup_pattern ,  error %d !", xgpon_error ) ;
            }
            xgpon_error = ag_drv_ngpon_tx_cfg_dv_hold_pat_set ( g_xgpon_db.physic_params.transceiver_dv_hold_pattern  );
            if (xgpon_error != PON_NO_ERROR)
            {
                /* Log */
                p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_dv_hold_pattern ,  error %d !", xgpon_error ) ;
            }
    }
}

static void p_sm_sn_post_profile_o3_state_assigned_onu_id_ploam_event( NGPON_OPERATION_SM * xi_operation_sm,
                                                                       OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    PON_ERROR_DTE    xgpon_error ;
    bdmf_error_t    timer_start_error ;
#if 0 
    ONUXG_DRV_TM_DS_FLOW_LUT_ENTRY  tm_ds_flow_entry ;
#endif
    NGPON_DS_ASSIGN_ONU_ID_PLOAM* assign_onu_ploam ; 
    ngpon_rx_assign_flow   assign_flow;


    assign_onu_ploam = (NGPON_DS_ASSIGN_ONU_ID_PLOAM* ) & xi_operation_sm_params->ploam_message_ptr->message.assign_onu_id ;


    if ( BDMF_TRUE == onu_serial_number_match ( g_xgpon_db.onu_params.sn.serial_number , assign_onu_ploam->serial_number )
       && assign_onu_ploam->assigned_onu_id <= MAX_VALID_ONU_ID && assign_onu_ploam->assigned_onu_id >= 0)
    {
        /* Stop TOZ */
        bdmf_timer_stop(&g_xgpon_db.os_resources.toz_timer_id);
        //#ifndef AKIVAS_DBG
        /* Disable the SN Alloc ID (0x3FF)*/
        xgpon_error =  control_sn_alloc_id ( BDMF_FALSE );
        if ( xgpon_error != PON_NO_ERROR )
        {
           p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                    "Unable to Disable SN alloc id  due to Driver error %d !", xgpon_error ) ;
        }
        //#endif
         /* Store ONU ID at the DB */
        g_xgpon_db.onu_params.onu_id = assign_onu_ploam->assigned_onu_id ;

        /* Enable Default ALLOC ID and associate it with ONU ID */
        xgpon_error = ngpon_enable_alloc_id_and_tcont ( NGPON_DEFAULT_TCONT_ID, NGPON_DEFAULT_TCONT_ID,
                                                        g_xgpon_db.onu_params.onu_id );
        if ( PON_NO_ERROR != xgpon_error ) 
        {
              p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                     "Unable to set Default ALLOc at HW. error 0x%X", xgpon_error );
        }
        else
        {          
              p_log ( ge_onu_logger.sections.stack.debug.general_id,
                    "Added Alloc id %lu at TCONT Table Index %d for TCONT ID  %d ",
                    g_xgpon_db.onu_params.onu_id, NGPON_DEFAULT_TCONT_ID , NGPON_DEFAULT_TCONT_ID );
        }

        g_xgpon_db.tconts_table [ NGPON_DEFAULT_ALLOC_ID_INDEX ].assign_alloc_valid_flag = BDMF_TRUE ;
        g_xgpon_db.tconts_table [ NGPON_DEFAULT_ALLOC_ID_INDEX ].config_tcont_valid_flag = BDMF_FALSE ;
        g_xgpon_db.tconts_table [ NGPON_DEFAULT_ALLOC_ID_INDEX ].alloc_id = g_xgpon_db.onu_params.onu_id ;
        g_xgpon_db.tconts_table [ NGPON_DEFAULT_ALLOC_ID_INDEX ].tcont_id = NGPON_DEFAULT_TCONT_ID ;

#if 0    /* no need in 6858 */
        /* Enable Default TCONT at the TM US */
        xgpon_error = ngpon_drv_set_tm_us_tcont_control   ( NGPON_DEFAULT_TCONT_ID , BL_ENABLE ) ;
        if ( PON_NO_ERROR != xgpon_error ) 
        {
              p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                     "Unable to set Default TCONT at HW. error 0x%X", xgpon_error );
        }
        
        /* Map Default XGEM to TM DS queue 10 (CPU queue) */
        xgpon_error = ngpon_drv_tm_ds_get_flow_lut_entry( NGPON_DEFAULT_XGEM_FLOW , & tm_ds_flow_entry );
        if ( PON_NO_ERROR != xgpon_error ) 
        {
             p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                     "Unable to tm_ds_get_flow_lut_entry. error %lu",xgpon_error);
        }

        tm_ds_flow_entry.destination_queue_number = TM_DS_CPU_QUEUE_ID ;
        xgpon_error = ngpon_drv_tm_ds_set_flow_lut_entry( NGPON_DEFAULT_XGEM_FLOW , & tm_ds_flow_entry );
        if ( PON_NO_ERROR != xgpon_error ) 
        {
             p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                     "Unable to tm_ds_set_flow_lut_entry. error %lu",xgpon_error);
        }

        g_xgpon_db.tm_ds_flows_config_table [ NGPON_DEFAULT_XGEM_FLOW ] = BL_TRUE ;
#endif

        /* Configure Default XGEM */
        g_xgpon_db.ds_xgem_flow_table [ NGPON_DEFAULT_XGEM_FLOW ].flow_id         = NGPON_DEFAULT_XGEM_FLOW;                             
        g_xgpon_db.ds_xgem_flow_table [ NGPON_DEFAULT_XGEM_FLOW ].flow_priority   = PON_FLOW_PRIORITY_EXCLUSIVE;
        g_xgpon_db.ds_xgem_flow_table [ NGPON_DEFAULT_XGEM_FLOW ].xgem_port_id     = assign_onu_ploam->assigned_onu_id; 
        g_xgpon_db.ds_xgem_flow_table [ NGPON_DEFAULT_XGEM_FLOW ].encryption_ring = NGPON_ENCRYPTION_RING_UNICAST_ONU_1;                           
        g_xgpon_db.ds_xgem_flow_table [ NGPON_DEFAULT_XGEM_FLOW ].crc_enable      = BDMF_FALSE ;
       

        /* Status is set to enable once enable XGEM is called */
        g_xgpon_db.ds_xgem_flow_table [ NGPON_DEFAULT_XGEM_FLOW ].enable_flag  = BDMF_FALSE ;


        assign_flow.portid =    g_xgpon_db.ds_xgem_flow_table [ NGPON_DEFAULT_XGEM_FLOW ].xgem_port_id ;
        assign_flow.priority =  g_xgpon_db.ds_xgem_flow_table [ NGPON_DEFAULT_XGEM_FLOW ].flow_priority ;
        assign_flow.enc_ring =  g_xgpon_db.ds_xgem_flow_table [ NGPON_DEFAULT_XGEM_FLOW ].encryption_ring ;
        assign_flow.crc_en =    g_xgpon_db.ds_xgem_flow_table [ NGPON_DEFAULT_XGEM_FLOW ].crc_enable ;
        assign_flow.en =        BDMF_TRUE ;

        xgpon_error = ag_drv_ngpon_rx_assign_flow_set ( g_xgpon_db.ds_xgem_flow_table [ NGPON_DEFAULT_XGEM_FLOW ].flow_id, &assign_flow ) ;
        if ( PON_NO_ERROR != xgpon_error ) 
        {
             p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                     "Unable to set_xgem_flow_config. error %lu",xgpon_error);
        }

#if 0   /* done by ag_drv_ngpon_rx_assign_flow_set */

        /* Enable Default XGEM */
        xgpon_error =  ngpon_control_xgem_flow ( ONUXG_DEFAULT_XGEM_FLOW, BL_ENABLE );
        if ( PON_NO_ERROR != xgpon_error ) 
        {
             p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                     "Unable to set_xgem_flow_config. error %lu",xgpon_error);
        }
        else
        {        
             g_xgpon_db.ds_xgem_flow_table [ ONUXG_DEFAULT_XGEM_FLOW ].flow_configured = BL_TRUE ;
              p_log ( ge_onu_logger.sections.stack.debug.general_id,
                    "Added XGEM Port %lu at XGEM Flow Table Index %d",
                    g_xgpon_db.onu_params.onu_id, ONUXG_DEFAULT_XGEM_FLOW  );
        }
#endif
        /* Start using RX Unicast PLOAM Key TYPE instead of Default PLOAM Key */
        xgpon_error = set_rx_ploam_key ( NGPON_ENCRYP_PLOAM_KEY_TYPE_ONU_1 );
        if ( xgpon_error != PON_NO_ERROR )
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set_rx_ploam_key: error %d !", xgpon_error ) ;
        }
        else
        {
            p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
            "Start using RX Unicast Ploam IK:");
             dump_key( g_xgpon_db.onu_params.ploam_ik_in_bytes , NGPON_DEFAULT_KEY_SIZE );
        }

        xgpon_error = write_registration_ploam_to_hw (assign_onu_ploam->assigned_onu_id ,
           DEFAULT_US_SN_ONU_PLOAM_SEQUENCE_NUM, BDMF_TRUE, NGPON_PLOAM_TYPE_REGISTRATION) ;  
        if ( PON_NO_ERROR != xgpon_error ) 
        {
             p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                     "Unable to write_registration_ploam_to_hw. error %lu",xgpon_error);
        }

        xgpon_error = ag_drv_ngpon_tx_cfg_plm_cfg_set(NGPON_PLOAM_TYPE_IDLE_OR_SN, BDMF_FALSE, BDMF_FALSE, 
            BDMF_FALSE, BDMF_FALSE);
        if ( PON_NO_ERROR != xgpon_error ) 
        {
           p_log ( ge_onu_logger.sections.stack.downstream_ploam.unknown_message_id,
                    "Unable to disable PLOAM buffer 2 (NGPON_PLOAM_TYPE_IDLE_OR_SN). error %lu",xgpon_error);
        }
 
       /*
         *   Check PLOAM types in accessor activation 
         */
        xgpon_error =  ag_drv_ngpon_tx_cfg_onu_id_to_plm_map_set ( NGPON_ONU_INDEX_ONU_0, assign_onu_ploam->assigned_onu_id, 
                                                                   NGPON_PLOAM_TYPE_REGISTRATION, NGPON_PLOAM_TYPE_IDLE_OR_SN );     
        if ( PON_NO_ERROR != xgpon_error ) 
        {
             p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                     "Unable to set onu_id_to_ploam_mapping. error %lu",xgpon_error);
        }
        /* Update delay to be only min response time and update delay to HW and  */
        xgpon_error = ngpon_set_eqd ( 0, P_ABSOLUTE, SIGN_POSITIVE  );
        if ( PON_NO_ERROR != xgpon_error ) 
        {
            p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                   "Unable to write EQD to HW. error %lu",xgpon_error);
        }

        /* Set ONU Id so PLOAM can arrive */
        xgpon_error = ag_drv_ngpon_rx_ploam_trfc_set ( NGPON_ONU_INDEX_ONU_0,
                                                      assign_onu_ploam->assigned_onu_id  ,
                                                      BDMF_TRUE );

        if ( xgpon_error != PON_NO_ERROR )
        {
           p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to enable Rx ONU ID and its valid bit due to Driver error %d !", xgpon_error ) ;
        }
        else
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                   "ONU ID %lu (0x%X) was updated at HW table   ",
                    assign_onu_ploam->assigned_onu_id, assign_onu_ploam->assigned_onu_id) ;
        }
        /* Report event "Link state transition" with state parameter */
        xgpon_error = send_message_to_pon_task ( PON_EVENT_LINK_STATE_TRANSITION ,
                                                       OPERATION_STATE_O3,
                                                       OPERATION_STATE_O4 ) ;

        xi_operation_sm -> activation_state = OPERATION_STATE_O4 ;

        /* Start to1 timer */
        timer_start_error = bdmf_timer_start (  & g_xgpon_db.os_resources.to1_timer_id, 
            NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.to1_timer_timeout));
        if ( timer_start_error != BDMF_ERR_OK )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to start to1 timer");
        }
        else
        {
            p_log(ge_onu_logger.sections.stack.debug.timers_id, 
                "Start TO1 timer");
        }
    }
}

static void p_sm_sn_post_profile_o3_state_deactivate_onu_id_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    bdmf_timer_stop(&g_xgpon_db.os_resources.toz_timer_id);
    sm_general_deactivate_onu_id_ploam_event(OPERATION_STATE_O3);
}

static void p_sm_sn_post_profile_o3_state_disable_serial_number_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params)
{

    p_sm_general_state_disable_serial_number_ploam_event(&xi_operation_sm->activation_state, 
        &xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number,
                                                          OPERATION_STATE_O3 );
}

static void p_sm_sn_post_profile_o3_state_detect_lods_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    NGPON_ONU_LINK_STATE_TRANSITION_INDICATION_PARAMS  link_states;
    link_states.onu_old_link_state = OPERATION_STATE_O2 ;
    link_states.onu_new_link_state = OPERATION_STATE_O1;
    p_sm_general_state_lods_event( xi_operation_sm, xi_operation_sm_params ,link_states );
}

static void p_sm_sn_o23_state_calibration_request_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    PON_ERROR_DTE rc;
    uint16_t tol;
    uint8_t dwlch_id;
    uint8_t current_dwlch;
    uint32_t admin_label_lsb;
    
    /* Stop TOZ */
    bdmf_timer_stop(&g_xgpon_db.os_resources.toz_timer_id);

    rc = ag_drv_ngpon_rx_gen_sync_ponid_ls_get(&tol, &dwlch_id, &admin_label_lsb);
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to read PON ID operational control. driver error = %d" );
    }
        
    current_dwlch = (xi_operation_sm_params->ploam_message_ptr->message.calibration_request.current_pon_id[3]>>4)&0xf;
    /* Calibration request for current DS wavelength channel */
    if (dwlch_id == current_dwlch)
    {
        /* Make calibration record for the confirmed upstream wavelength channel [DB]. */
        memcpy(g_xgpon_db.link_params.calibration_record.vendor_id, 
            xi_operation_sm_params->ploam_message_ptr->message.calibration_request.vendor_id, 4);
        memcpy(g_xgpon_db.link_params.calibration_record.vssn, 
            xi_operation_sm_params->ploam_message_ptr->message.calibration_request.vssn, 4);
        memcpy(g_xgpon_db.link_params.calibration_record.current_pon_id, 
            xi_operation_sm_params->ploam_message_ptr->message.calibration_request.current_pon_id, 4);
        memcpy(g_xgpon_db.link_params.calibration_record.correlation_tag, 
            xi_operation_sm_params->ploam_message_ptr->message.calibration_request.correlation_tag, 2);
        memcpy(g_xgpon_db.link_params.calibration_record.target_ds_pon_id, 
            xi_operation_sm_params->ploam_message_ptr->message.calibration_request.target_ds_pon_id, 4);
        memcpy(g_xgpon_db.link_params.calibration_record.target_us_pon_id, 
            xi_operation_sm_params->ploam_message_ptr->message.calibration_request.target_us_pon_id, 4);
        
        /* Tune to the specified pair of downstream and upstream wavelength channels... TBD */ 
    }
    else
    { 
        /* Clear system profile, channel profile and burst profiles parameters. */
        clear_burst_profiles(); /* Paramter set 1: burst */
        clear_onu_config_7();   /* Paramter set 7: channel, system */

        /* Re-Start Discovery timer TOZ. looking for new channels.*/
        rc = bdmf_timer_start(&g_xgpon_db.os_resources.toz_timer_id, 
            NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.toz_timer_timeout));
        if (rc != BDMF_ERR_OK)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to start TOZ timer");
        }
        else
        {
            p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start TOZ timer");
        }

        /* Move to O1.1 */
        xi_operation_sm->activation_state = OPERATION_STATE_O1;
        g_xgpon_db.link_params.sub_sm_state = o1_1_offsync_sub_state;

        /* Report event "Link state transition" with state parameter */
        rc = send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
            OPERATION_STATE_O3, OPERATION_STATE_O1);
        if (rc != BDMF_ERR_OK)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to send user indication: error %d", rc);
        }
    }
}

static void p_sm_sn_o23_state_timer_toz_expire_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    bdmf_error_t rc;

    /* Clear burst profiles (Paramter set 1) */
    clear_burst_profiles(); 


    /* Clear System Profiles and Channels profiles (Paramter Set 7) */
    clear_onu_config_7();
    g_xgpon_db.link_params.init_tuning_done = BDMF_FALSE ;

    /* Wait for new set of ploams to arrive. */
    g_xgpon_db.link_params.burst_profiles.burst_profile_received = BDMF_FALSE;
    g_xgpon_db.link_params.system_profile.system_profile_received = BDMF_FALSE;
    g_xgpon_db.link_params.ch_profile.channel_profile_received = BDMF_FALSE;

    /* Abandon current channel and try the next. */
    ngpon_tune_rx(0);  /* Go to next channel */
    ngpon_tune_tx(0);  /* Most chances are that the configuration will be symmetric */


    /* Move to O1.1 according to sync state */
    sm_general_set_initial_state();


    /* Report event "Link state transition" with state parameter */
    rc = send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
        OPERATION_STATE_O3, OPERATION_STATE_O1);
    if (rc != BDMF_ERR_OK)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d", rc);
    }
}

static void p_sm_ranging_o4_state_burst_profile_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    PON_ERROR_DTE  xgpon_error ;
    
    xgpon_error =  ngpon_general_state_burst_profile_event(&xi_operation_sm_params->ploam_message_ptr->message.burst_profile);
    if ( PON_NO_ERROR != xgpon_error  )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                 "Unable to handle busrt profile, error %d !", xgpon_error ) ;
    }

    if (g_xgpon_db.optics_type == BP_GPON_OPTICS_TYPE_PMD)
    {
        xgpon_error = ag_drv_ngpon_tx_cfg_dv_setup_pat_set ( g_xgpon_db.physic_params.transceiver_dv_setup_pattern  );
            if (xgpon_error != PON_NO_ERROR)
            {
                /* Log */
                p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_dv_setup_pattern ,  error %d !", xgpon_error ) ;
            }
            xgpon_error = ag_drv_ngpon_tx_cfg_dv_hold_pat_set ( g_xgpon_db.physic_params.transceiver_dv_hold_pattern  );
            if (xgpon_error != PON_NO_ERROR)
            {
                /* Log */
                p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_dv_hold_pattern ,  error %d !", xgpon_error ) ;
            }
    }
}
static void p_sm_ranging_o4_state_ranging_time_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    PON_ERROR_DTE xgpon_error ;
    uint32_t   ranging_delay ;
    uint32_t   flow_id ;
    NGPON_DS_RANGING_TIME_PLOAM* ranging_time_ploam ;
    ngpon_rx_assign_flow assign_flow ;
    PON_CONFIGURE_OMCI_PORT_ID_INDICATION configure_omci_port_id;

    ranging_time_ploam = (NGPON_DS_RANGING_TIME_PLOAM* ) & xi_operation_sm_params->ploam_message_ptr->message.ranging_time;
  
    /* Only if it is Unicast PLOAM and absolute ranging */
    if ( ( xi_operation_sm_params->ploam_message_ptr->onu_id == g_xgpon_db.onu_params.onu_id ) &&
         ( ranging_time_ploam ->options.absolute_or_relative == P_ABSOLUTE ) )
    {
        /* Stop to1 timer */
        bdmf_timer_stop ( &g_xgpon_db.os_resources.to1_timer_id ) ;

        /* Do NOT add min response time before writing the delay to HW */
        bytes_to_word ( ranging_time_ploam ->equalization_delay, & ranging_delay );

        ranging_delay = ranging_delay_correction_upstream_rate (ranging_delay);

        xgpon_error = ngpon_set_eqd ( ranging_delay , P_ABSOLUTE ,SIGN_POSITIVE );
        if ( PON_NO_ERROR != xgpon_error ) 
        {
            p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id, "Unable to write EQD to HW. error %lu",xgpon_error);
        }

        /* Start using Unicast PLOAM Key TYPE instead of Default PLOAM Key */
        xgpon_error = set_tx_ploam_key ( NGPON_ENCRYP_PLOAM_KEY_TYPE_ONU_1 );
        if ( xgpon_error != PON_NO_ERROR )
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set_tx_ploam_key: error %d !", xgpon_error ) ;
        }
        else
        {
            p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
            "Start using TX Unicast Ploam IK:");
             dump_key( g_xgpon_db.onu_params.ploam_ik_in_bytes , NGPON_DEFAULT_KEY_SIZE );
        }

        xgpon_error = write_idle_ploam_to_hw (g_xgpon_db.onu_params.onu_id);
        if ( PON_NO_ERROR != xgpon_error ) 
        {
             p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                     "Unable to write_idle_ploam_to_hw. error %lu",xgpon_error);
        }

        /* Update OMCI Keys */
        xgpon_error = set_omci_keys() ;
        if ( xgpon_error != PON_NO_ERROR )
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set_omci_key: error %d !", xgpon_error ) ;
        }
        else
        {
            p_log ( ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id,
            "Start using OMCI_IK:");
             dump_key( g_xgpon_db.onu_params.omci_ik_in_bytes , NGPON_DEFAULT_KEY_SIZE );
        }

        xgpon_error =  ag_drv_ngpon_tx_cfg_onu_id_to_plm_map_set ( NGPON_ONU_INDEX_ONU_0, g_xgpon_db.onu_params.onu_id, 
                                                                   NGPON_PLOAM_TYPE_NORMAL, NGPON_PLOAM_TYPE_IDLE_OR_SN );
        if ( PON_NO_ERROR != xgpon_error ) 
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set_onu_id_to_ploam_mapping, xgpon_error =%u",xgpon_error );
        }


        /* Disable PLOAM buffer 1 that contained the registration ID */
        xgpon_error = ag_drv_ngpon_tx_cfg_plm_cfg_set(NGPON_PLOAM_TYPE_REGISTRATION, BDMF_FALSE, BDMF_FALSE, 
            BDMF_FALSE, BDMF_FALSE);
        if ( PON_NO_ERROR != xgpon_error ) 
        {
           p_log ( ge_onu_logger.sections.stack.downstream_ploam.unknown_message_id,
                    "Unable to disable PLOAM buffer 1 (NGPON_PLOAM_TYPE_REGISTRATION). error %lu",xgpon_error);
        }

        xgpon_error =  ag_drv_ngpon_tx_cfg_tcont_to_onu_id_map_set (NGPON_DEFAULT_TCONT_ID, NGPON_ONU_INDEX_ONU_0) ;
        if ( PON_NO_ERROR != xgpon_error ) 
        {
           p_log ( ge_onu_logger.sections.stack.downstream_ploam.unknown_message_id,
                    "Unable to set ONU to T-CONT mapping in Tx. error %lu",xgpon_error);
        }

        if (g_xgpon_db.optics_type == BP_GPON_OPTICS_TYPE_PMD)
        {
                xgpon_error = ag_drv_ngpon_tx_cfg_dv_setup_pat_set (0xffffff);
            if (xgpon_error != PON_NO_ERROR)
            {
                /* Log */
                p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_dv_setup_pattern ,  error %d !", xgpon_error ) ;
            }
            xgpon_error = ag_drv_ngpon_tx_cfg_dv_hold_pat_set (0);
            if (xgpon_error != PON_NO_ERROR)
            {
                /* Log */
                p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_dv_hold_pattern ,  error %d !", xgpon_error ) ;
            }
        }

        xgpon_error =  write_ack_ploam_to_hw (g_xgpon_db.onu_params.onu_id ,
           xi_operation_sm_params->ploam_message_ptr->sequence_number) ;
        if ( PON_NO_ERROR == xgpon_error ) 
        {

            p_log ( ge_onu_logger.sections.stack.upstream_ploam.ack_message_id,
             "Ack PLOAM for Ranging Time was written to HW (Ack Sequence Number = %lu)",
            xi_operation_sm_params->ploam_message_ptr->sequence_number );
        }
        else
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                     "Unable to write_ack_ploam_to_hw, xgpon_error =%u",xgpon_error );
        }
        /*  reset key exchange configuration */
        p_key_reset_configuration  (  ) ;

        /* Change internal state to OPERATION_STATE_O5  */
        xi_operation_sm->activation_state = OPERATION_STATE_O5 ;
        g_xgpon_db.link_params.sub_sm_state = o5_1_associated_sub_state; 

        /* Report event "Link state transition" with state parameter */
        xgpon_error = send_message_to_pon_task ( PON_EVENT_LINK_STATE_TRANSITION ,
                                                       OPERATION_STATE_O4,
                                                       OPERATION_STATE_O5 ) ;
        if ( xgpon_error != BDMF_ERR_OK )
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send_message_to_pon_task: error %d !", xgpon_error ) ;
        }        
        /* Send indication to Pon task according assigned alloc - id just arrived */
        xgpon_error = send_message_to_pon_task ( PON_EVENT_USER_INDICATION,
                                                    PON_INDICATION_ASSIGN_ONU_ID,
                                                    g_xgpon_db.onu_params.onu_id ) ;

        /* Restore XGEM Port ID configuration and enable them , starting from flow 1 */
        for ( flow_id = 1 ; flow_id < NGPON_MAX_NUM_OF_XGEM_FLOW_IDS ; flow_id ++ )
        {
            if ( ( NGPON_XGEM_PORT_ID_UNASSIGNED != g_xgpon_db.ds_xgem_flow_table [ flow_id ].xgem_port_id ) &&
                 ( NGPON_XGEM_FLOW_ID_UNASSIGNED != g_xgpon_db.ds_xgem_flow_table [ flow_id ].flow_id ) ) 
            {
                /* Configure  XGEM */
                assign_flow.portid   = g_xgpon_db.ds_xgem_flow_table [ flow_id ].xgem_port_id ;
                assign_flow.priority =  g_xgpon_db.ds_xgem_flow_table [ flow_id ].flow_priority ;
                assign_flow.enc_ring =  g_xgpon_db.ds_xgem_flow_table [ flow_id ].encryption_ring ;
                assign_flow.crc_en   =  g_xgpon_db.ds_xgem_flow_table [ flow_id ].crc_enable ;
                assign_flow.en       =  BDMF_TRUE ;

                xgpon_error = ag_drv_ngpon_rx_assign_flow_set ( g_xgpon_db.ds_xgem_flow_table [ flow_id ].flow_id, &assign_flow ) ;
                if ( PON_NO_ERROR != xgpon_error ) 
                {
                     p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                             "Unable to set_xgem_flow_config. error %lu",xgpon_error);
                }
            }
        }

        /* workaround: Tear down OMCI port before creating a new one;
           This is done becuase we want the RDPA TCONT to be created once again if already exeists (otherwise skipped by indication callback)
           because when RDPA TCONT is being created, it reuses the default alloc id, and otherwise default allocid will not be set to OMCI TCONT */

        configure_omci_port_id.port_control = BDMF_FALSE;
        configure_omci_port_id.port_id = g_xgpon_db.onu_params.onu_id;
        xgpon_error = send_message_to_pon_task(PON_EVENT_USER_INDICATION,
           PON_INDICATION_CONFIGURE_OMCI_PORT_ID_FILTER, configure_omci_port_id);
        if (xgpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                 "Unable to send OMCI port creation request: error %d !", xgpon_error);
        }

        configure_omci_port_id.port_control = BDMF_TRUE ;
        configure_omci_port_id.port_id = g_xgpon_db.onu_params.onu_id ;    /* OMCI Port ID is equal to ONU ID */

        xgpon_error = send_message_to_pon_task(PON_EVENT_USER_INDICATION,
           PON_INDICATION_CONFIGURE_OMCI_PORT_ID_FILTER, configure_omci_port_id);
        if (xgpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                 "Unable to send OMCI port creation request: error %d !", xgpon_error);
        }

#ifdef CONFIG_BCM_GPON_TODD
        /* g_xgpon_db.physic_params.eqd_in_bits was updated in ngpon_set_eqd() */
        tod_handle_ranging_time_ploam(g_xgpon_db.physic_params.eqd_in_bits);
#endif 

#ifdef CONFIG_BCM_TIME_SYNC_PON_UNSTABLE
        /* Time Synchronization: PON Unstable - Set signal On */
        xgpon_error = time_sync_pon_unstable_set(1);
        if (xgpon_error != PON_NO_ERROR)
        {
            p_log( ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Time Synchronizatioin PON Unstable: Failed to set signal On, rc=%d", 
                xgpon_error);
    }
#endif 
    }
    else
    {
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
                "ONU is at O4 - it can get only Unicast+Absolute Ranging Time (PLOAM ONU Id %lu, Stored ONU Id %lu, absolute flag =%d)  ",
                xi_operation_sm_params->ploam_message_ptr->onu_id, g_xgpon_db.onu_params.onu_id,
                ranging_time_ploam ->options.absolute_or_relative );
    }
}

static void p_sm_ranging_o4_state_timer_to1_expire_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{

    PON_ERROR_DTE xgpon_error, rc;

    xgpon_error = clear_default_alloc_id_and_default_xgem_flow_id ( );
    if ( xgpon_error != PON_NO_ERROR )
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear_default_alloc_id_and_default_xgem_flow_id  due to Driver error %d !", xgpon_error ) ;
    }

    xgpon_error = generate_sn_ploam ();
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                 "Unable to Generate SN PLAOM, error %d !", xgpon_error ) ;
    }

    /* Enable the SN Alloc ID (0x3FF)*/
    xgpon_error =  control_sn_alloc_id ( BDMF_TRUE );
    if ( xgpon_error != PON_NO_ERROR )
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to Enable SN alloc id  due to Driver error %d !", xgpon_error ) ;
    }

    /* Start Discovery timer TOZ */
    if ((g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGPON) &&
        (g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGS))
    {
       rc = bdmf_timer_start(&g_xgpon_db.os_resources.toz_timer_id, 
           NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.toz_timer_timeout));
       if (rc != BDMF_ERR_OK)
       {
           p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to start TOZ timer");
       }
       else
       {
           p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start TOZ timer");
       }
    }
    /* Report event "Link state transition" with state parameter */
    xgpon_error = send_message_to_pon_task ( PON_EVENT_LINK_STATE_TRANSITION ,
                                               OPERATION_STATE_O4,
                                               OPERATION_STATE_O3 ) ;

    /* Change internal state to OPERATION_STATE_O3  */
    xi_operation_sm -> activation_state = OPERATION_STATE_O3 ;
}


static void p_sm_ranging_o4_state_deactivate_onu_id_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{

    bdmf_timer_stop ( &g_xgpon_db.os_resources.to1_timer_id  ) ;
    sm_general_deactivate_onu_id_ploam_event(OPERATION_STATE_O4);

}
static void p_sm_ranging_o4_state_disable_serial_number_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params)
{
    p_sm_general_state_disable_serial_number_ploam_event(&xi_operation_sm->activation_state, 
        &xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number,
                                                          OPERATION_STATE_O4 );
}
static void p_sm_ranging_o4_state_detect_lods_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    NGPON_ONU_LINK_STATE_TRANSITION_INDICATION_PARAMS  link_states;

    /* Stop to1 timer */
    bdmf_timer_stop ( &g_xgpon_db.os_resources.to1_timer_id  ) ;

    link_states.onu_old_link_state = OPERATION_STATE_O4 ;
    link_states.onu_new_link_state = OPERATION_STATE_O1;
    p_sm_general_state_lods_event( xi_operation_sm, xi_operation_sm_params ,link_states );
}


static void p_sm_operation_o5_state_burst_profile_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{

    PON_ERROR_DTE    xgpon_error ;
    
    xgpon_error = ngpon_general_state_burst_profile_event(&xi_operation_sm_params->ploam_message_ptr->message.burst_profile);
    if ( PON_NO_ERROR != xgpon_error  )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                 "Unable to handle busrt profile, error %d !", xgpon_error ) ;
    }
   
    /* If Unicast PLOAM - Send Ack */
    if ( xi_operation_sm_params->ploam_message_ptr->onu_id == g_xgpon_db.onu_params.onu_id ) 
    {
        xgpon_error =  write_ack_ploam_to_hw (g_xgpon_db.onu_params.onu_id ,
          xi_operation_sm_params->ploam_message_ptr->sequence_number) ;
        if ( PON_NO_ERROR == xgpon_error ) 
        {
            p_log ( ge_onu_logger.sections.stack.upstream_ploam.ack_message_id,
           "Ack PLOAM for Profile was written to HW (Ack Sequence Number = %lu)",
           xi_operation_sm_params->ploam_message_ptr->sequence_number );
        }
        else
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write_ack_ploam_to_hw" );
        }
    }
}

static void general_ranging_time_ploam_event(NGPON_DS_PLOAM *ploam)
{
    PON_ERROR_DTE  xgpon_error ;
    uint32_t   ranging_delay ;
    NGPON_DS_RANGING_TIME_PLOAM* ranging_time_ploam ;
    ranging_time_ploam = (NGPON_DS_RANGING_TIME_PLOAM*)&ploam->message.ranging_time;

    /* Unicast Ranging Time PLOAM during o5 can be Absolute or Relative */
    if (ploam->onu_id == g_xgpon_db.onu_params.onu_id) 
    {
        /* Absolute/Relative and sign value according to the incoming Ploam */
        bytes_to_word ( ranging_time_ploam ->equalization_delay, & ranging_delay );
        ranging_delay = ranging_delay_correction_upstream_rate (ranging_delay);

        xgpon_error = ngpon_set_eqd ( ranging_delay, ranging_time_ploam ->options.absolute_or_relative,
                                      ranging_time_ploam ->options.sign  );
        if ( PON_NO_ERROR == xgpon_error ) 
        {
            /* EqD was successfully written to HW - since it was unicast message , Send Ack to the OLT */
            xgpon_error =  write_ack_ploam_to_hw (g_xgpon_db.onu_params.onu_id ,
                ploam->sequence_number);
            if ( PON_NO_ERROR == xgpon_error ) 
            {
                 p_log ( ge_onu_logger.sections.stack.upstream_ploam.ack_message_id,
                "Ack PLOAM for Ranging Time was written to HW (Ack Sequence Number = %lu)",
                     ploam->sequence_number);
                return;
            }
            else 
            {
                p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                         "Unable to write_ack_ploam_to_hw  ,Error %lu",xgpon_error  );
                return;
            }
        }
        else
        {
            p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                     "Unable to write EQD to HW. Error %lu",xgpon_error);
            return;
        }
    }   
    /* Broadcast PLOAM is relevant only if it is Relative Ranging time */
    else if (ploam->onu_id == BROADCAST_ONU_ID && 
        ranging_time_ploam->options.absolute_or_relative == P_RELATIVE)              
        {
            /* Absolute/Relative and sign value according to the incoming Ploam */
            bytes_to_word ( ranging_time_ploam ->equalization_delay, & ranging_delay );
            ranging_delay = ranging_delay_correction_upstream_rate (ranging_delay);

            xgpon_error = ngpon_set_eqd ( ranging_delay, ranging_time_ploam ->options.absolute_or_relative,
                                          ranging_time_ploam ->options.sign  );
            if ( PON_NO_ERROR != xgpon_error ) 
            {
                p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                         "Unable to write EQD to HW. Error %lu",xgpon_error);
                return;
            }
        }
    /* Illegal Ranging time message */
    else
    {
           p_log ( ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
           "Illegal Rangign Time Message : ONU is at O5 - it can get only "
           "(Unicast+Absulote/Relative) Ranging Time or (Broadcast+Relative) Ranging Time" );
           return;
    }

#ifdef CONFIG_BCM_GPON_TODD
    /* g_xgpon_db.physic_params.eqd_in_bits was updated in ngpon_set_eqd() */
    tod_handle_ranging_time_ploam(g_xgpon_db.physic_params.eqd_in_bits);
#endif 
}

static void p_sm_operation_o5_state_ranging_time_ploam_event(NGPON_OPERATION_SM * xi_operation_sm,
    OPERATION_SM_PARAMS *xi_operation_sm_params) 
{
    general_ranging_time_ploam_event(xi_operation_sm_params->ploam_message_ptr);
}

static void p_sm_operation_o5_state_deactivate_onu_id_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params )
{
      sm_general_deactivate_onu_id_ploam_event(OPERATION_STATE_O5);
}

static void p_sm_operation_o5_state_disable_serial_number_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params)
{
    p_sm_general_state_disable_serial_number_ploam_event(&xi_operation_sm->activation_state, 
        &xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number,
                                                          OPERATION_STATE_O5 );
}

static void p_sm_operation_o5_state_detect_lods_event( 
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params) 
{
    bdmf_error_t rc;
    NGPON_ONU_LINK_STATE_TRANSITION_INDICATION_PARAMS link_states;

    /* Start TO3 if the wavelength channel protection (WLCP) is ON, 
       or timer TO2 if WLCP is off */
    if (g_xgpon_db.link_params.WLCP)
    {
        /* Start LODS Protection timer TO3 */
        rc = bdmf_timer_start(&g_xgpon_db.os_resources.to3_timer_id, 
            NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.to3_timer_timeout));
    }
    else
    {
        rc = bdmf_timer_start(&g_xgpon_db.os_resources.to2_timer_id,
            NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.to2_timer_timeout));
    }

    if (rc != BDMF_ERR_OK)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to start timer");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start TO3/TO2 timer");
    }
    link_states.onu_old_link_state = OPERATION_STATE_O5;
    link_states.onu_new_link_state = OPERATION_STATE_O6;
    g_pon_lods_pm_counters.total_lods_event_counter++;
    p_sm_general_state_lods_event(xi_operation_sm, xi_operation_sm_params, link_states);
}

static void p_sm_operation_o5_state_request_registration_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    PON_ERROR_DTE     xgpon_error ;
    /* Write registration PLOAM to Normal PLOAM queue (Buf 0)*/
    xgpon_error = write_registration_ploam_to_hw (  g_xgpon_db.onu_params.onu_id ,
                                                    xi_operation_sm_params->ploam_message_ptr->sequence_number ,
                                                    BDMF_FALSE , NGPON_PLOAM_TYPE_NORMAL ) ;
    if ( PON_NO_ERROR != xgpon_error ) 
    {
         p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                 "Unable to write_registration_ploam_to_hw. error %lu",xgpon_error);
    }
}

static void general_assigned_alloc_id_ploam_event(NGPON_DS_PLOAM *ploam) 
{
    PON_ERROR_DTE xgpon_error ;
    NGPON_ASSIGN_ALLOC_ID_INDICATION_PARAMS assign_alloc_id ;
    NGPON_DS_ASSIGN_ALLOC_ID_PLOAM* assign_alloc_id_ploam ;

    assign_alloc_id_ploam = (NGPON_DS_ASSIGN_ALLOC_ID_PLOAM* ) & ploam->message.assign_alloc_id;


    /* Build the assign alloc id struct */
    assign_alloc_id.alloc_id = ((PON_ALLOC_ID_DTE)(ploam->message.assign_alloc_id.alloc_id.alloc_id));
    assign_alloc_id.assign_flag = 
       (ploam->message.assign_alloc_id.alloc_id_type == ALLOC_TYPE_DEALLOC) ? BDMF_FALSE : BDMF_TRUE ;


    /* Check Alloc Id validity */

    if (assign_alloc_id.alloc_id >= ALLOC_ID_MIN_ASSIGN)
    {
        /* Return Ack */
        xgpon_error = write_ack_ploam_to_hw(g_xgpon_db.onu_params.onu_id, ploam->sequence_number);
        if (PON_NO_ERROR == xgpon_error) 
        {
           p_log (ge_onu_logger.sections.stack.upstream_ploam.ack_message_id,
               "Ack PLOAM for Assign Alloc Id was written to HW (Ack Sequence Number = %lu)",
                ploam->sequence_number);
        }
        else
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write_ack_ploam_to_hw" );
         }
    }
    else 
    {
        /* Illegal Alloc_ID, send NACK to OLT */       
        xgpon_error = write_ack_ploam_to_hw_comp_code(g_xgpon_db.onu_params.onu_id, ploam->sequence_number, ACKNOWLEDGE_PARAMETER_ERROR);
        if (PON_NO_ERROR == xgpon_error) 
        {
           p_log (ge_onu_logger.sections.stack.upstream_ploam.ack_message_id,
               "Nack PLOAM for Assign Alloc Id was written to HW (Ack Sequence Number = %lu, completion_code = ACKNOWLEDGE_PARAMETER_ERROR)",
                ploam->sequence_number);
        }
        else
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write_ack_ploam_to_hw_comp_code" );
        }
        return ; /* No further processing */
    }

    p_log ( ge_onu_logger.sections.stack.downstream_ploam.assign_alloc_id_message_id,
            "Assign alloc-id message arrived: , Type=%d (%s)",
            ploam->message.assign_alloc_id.alloc_id_type,
            assign_alloc_id.assign_flag == BDMF_FALSE ? "Deallocate this Alloc-ID" : "Assign this Alloc-ID"  ) ;

    /* Update HW and internal data staructure of Alloc-Id */
    ngpon_config_alloc_id ( assign_alloc_id ) ;

    /* Send indication to Pon task according assigned alloc - id just arrived */
    xgpon_error = send_message_to_pon_task ( PON_EVENT_USER_INDICATION, PON_INDICATION_ASSIGN_ALLOC_ID,
                                               assign_alloc_id ) ;
    if ( BDMF_ERR_OK != xgpon_error ) 
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send oam indication: error %d !", xgpon_error ) ;
}

static void p_sm_operation_o5_state_assign_alloc_id_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params) 
{
    general_assigned_alloc_id_ploam_event(xi_operation_sm_params->ploam_message_ptr);
}

static void p_sm_operation_o5_state_key_control_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    memcpy(&g_xgpon_db.link_params.key_sm.aes_message_ptr, 
        xi_operation_sm_params->ploam_message_ptr, sizeof(NGPON_DS_PLOAM));
    if (xi_operation_sm_params->ploam_message_ptr->message.key_control.control_flag.control ==  KEY_CONTROL_GENERATE_NEW_KEY)
    {
        p_key_sm(KEY_EVENT_NEW_KEY_REQUEST, 
            xi_operation_sm_params->ploam_message_ptr->message.key_control.key_index.index);
    }
    else
    {
        p_key_sm(KEY_EVENT_CONFIRM_KEY_REQUEST, 
            xi_operation_sm_params->ploam_message_ptr->message.key_control.key_index.index);
    }
}

static void p_sm_operation_o5_state_sleep_allow_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    /* TBD*/
    p_log ( ge_onu_logger.sections.stack.downstream_ploam.sleep_allow_message_id,
            "%s is not supported yet...",__FUNCTION__);
}

static void p_sm_operation_o5_state_tuning_control_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o5_sub_sm[g_xgpon_db.link_params.sub_sm_state][tuning_control_ploam_event]((o5_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
}

static void p_sm_operation_o5_state_sfc_match_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sub_sm_params params;
    bdmf_error_t  msg_snd_failure ;

    o5_sub_sm[g_xgpon_db.link_params.sub_sm_state][sfc_match_event]((o5_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);

    /* Report event "Link state transition: O5->O8*/
    msg_snd_failure = send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
        OPERATION_STATE_O5, OPERATION_STATE_O8);
    if (msg_snd_failure != BDMF_ERR_OK)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to send PON task message");
    }    

    /* Move to O8.1 sub state */
    xi_operation_sm->activation_state = OPERATION_STATE_O8;
    g_xgpon_db.link_params.sub_sm_state = o8_1_offsync_sub_state;
   
    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link Sub State transition: o5_2_pending_sub_state -> o8_1_offsync_sub_state");
}


static void write_tuning_response(uint8_t operation_code, uint16_t response_code, 
    uint8_t sequence_number)
{
    NGPON_US_PLOAM ploam;
    PON_ERROR_DTE rc;
    uint32_t pon_id;
    uint16_t rand;
    int i;

    
    memset(&ploam, 0, sizeof(NGPON_US_PLOAM));
    ploam.onu_id = htons(g_xgpon_db.onu_params.onu_id);    
    ploam.message_id = US_TUNING_RESPONSE_MESSAGE_ID; 
    ploam.sequence_number = sequence_number;
    
    ploam.message.tuning_response.operation_code = operation_code;
    ploam.message.tuning_response.response_code = htons(response_code);
    memcpy(ploam.message.tuning_response.vendor_id, g_xgpon_db.onu_params.sn.sn.vendor_id, 4);
    memcpy(ploam.message.tuning_response.vssn, g_xgpon_db.onu_params.sn.sn.vssn, 4);
    pon_id = ngpon_get_pon_id();
    ploam.message.tuning_response.pon_id = htonl(pon_id);
    /* In an upstream PLOAM message, the correlation tag is an ONU-generated 
    non-zero 16-bit field, which should take a different value each time the 
    transmitter optical power or frequency are changed. */
    rand = bdmf_rand16();
    ploam.message.tuning_response.correlation_tag = rand; 
    /* channel ID of the US wavelength channel in which the ONU is transmitting - TBD */
    ploam.message.tuning_response.uwlch_id.channel_id = 
        g_xgpon_db.link_params.ch_profile.current_us_channel - 1; 
    /* Report that all channels have sufficient calibration (0b0011) */
    for (i=0; i<8; i++) 
    {
        ploam.message.tuning_response.sn_digest_calibration_record.calibration_record_status_twdm[i] = 0x33;
    }
    /* 0x0 Indicates that the ONU does not support fine tuning/dithering. */
    ploam.message.tuning_response.tuning_granularity = 0; 
    /* 0x0 Indicates that the ONU does not support fine tuning/dithering. */
    ploam.message.tuning_response.one_step_tuning_time = 0; 
    ploam.message.tuning_response.us_line_rate.twdm.us_nominal_line_rate_10G = 1;
    ploam.message.tuning_response.us_line_rate.twdm.us_nominal_line_rate_2_5G = 0;
    /* Unattenuated transmission */
    ploam.message.tuning_response.attenuation = 0; 
    ploam.message.tuning_response.power_leveling_capability = 0;
    
    rc = send_us_ploam(&ploam, BDMF_FALSE, NGPON_PLOAM_TYPE_NORMAL, BDMF_TRUE);
    if (rc == PON_NO_ERROR) 
    {
        p_log(ge_onu_logger.sections.stack.upstream_ploam.tuning_response_message_id,
            "Sends Tuning Response PLOAM message for protection ploam message (Sequence Number = %lu)",
            ploam.sequence_number);
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to write Tuning Response PLOAM message to HW");
    }

    if (operation_code_complete_u == operation_code)
    {
        /*
         * ONU is switched to destination channel and tries to signal to OLT that it got DS Sync
         * Put couple of PLOAMs to Tx buffer in case first PLOAM(s) will arrive garbled
         */

         /* Persistent PLOAM , DO NOT Use default PLOAM key for MIC calculation */ 
         send_us_ploam(&ploam, BDMF_TRUE, NGPON_PLOAM_TYPE_IDLE_OR_SN, BDMF_TRUE);

    }

}


static void sm_o5_associated_sub_state_o5_1_tuning_control_ploam_event(
    o5_sub_states *sub_state, sub_sm_params *params)
{
    DS_CHANNEL_PROFILE_TWDM_PLOAM *ds_channel_profile_p ;
    DS_CHANNEL_PROFILE_TWDM_PLOAM *us_channel_profile_p ;

    uint32_t ds_pon_id = ntohl(params->p_ploam_message->message.tuning_control.target_ds_pon_id) ;
    uint32_t us_pon_id = ntohl(params->p_ploam_message->message.tuning_control.target_us_pon_id) ;
    PON_ERROR_DTE stack_error ; 
    uint32_t ls_value ;
    uint32_t ms_value ;   
    uint8_t  valid ;
    uint16_t response_code ;
    uint16_t scheduled_sfc ;
    uint16_t current_sfc_low_16 ;

    /* Expect Operation code: Request */
    if (params->p_ploam_message->message.tuning_control.operation_code != operation_code_request) 
    { 
        p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id, 
              "Tuining control with OpCode=%d received", params->p_ploam_message->message.tuning_control.operation_code) ;

        stack_error = write_ack_ploam_to_hw(g_xgpon_db.onu_params.onu_id, params->p_ploam_message->sequence_number);
        return ;
    }

    /* Stats the tuning pm counter */
    if ((ngpon_get_pon_id() != ds_pon_id) && (ngpon_get_pon_id() != us_pon_id))
    {
        // request for tuning to different DS channel - request PON ID differs from current 
        g_pon_tuning_req_pm_counters.requests_for_rx_or_rxtx++;
    }
    else if (ngpon_get_pon_id() != us_pon_id)
    {
        // request for tuning to different US channel - request PON ID differs from current 
        g_pon_tuning_req_pm_counters.requests_for_tx_counter++;
    }

    /* Sends Tuning_Response PLOAM message with an appropriate operation code:
       If the ONU intends to execute the command - Send ACK
       If the ONU is unable to execute the command - Send NACK */

    if ((PON_NO_ERROR == ngpon_find_channel_profile_by_pon_id (ds_pon_id, &ds_channel_profile_p)) &&
        (PON_NO_ERROR == ngpon_find_channel_profile_by_pon_id (us_pon_id, &us_channel_profile_p)))
    {
        if ((PON_NO_ERROR == ngpon_tuning_control_chan_profile_ds_check(ds_channel_profile_p)) &&
            (PON_NO_ERROR == ngpon_tuning_control_chan_profile_us_check(us_channel_profile_p)))
        {


            if (BDMF_FALSE == ngpon_is_sfc_indication_int_set())
            {
                /*
                 * Configure SFC_IND interrupt
                 */
                stack_error = ag_drv_ngpon_rx_gen_sync_sfc_get (&ms_value, &valid) ;
                if (stack_error != PON_NO_ERROR)
                {
                     p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id, 
                        "Failed to read SFC_MSB value") ;
                     return ;
                }

                stack_error = ag_drv_ngpon_rx_gen_sync_sfc_ls_get (&ls_value) ;
                if (stack_error != PON_NO_ERROR)
                {
                     p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id, 
                        "Failed to read SFC_LSB value") ;
                     return ;
                }
                scheduled_sfc = ntohs(params->p_ploam_message->message.tuning_control.scheduled_sfc) ;
                current_sfc_low_16 = ls_value & 0xFFFF ;

                if (scheduled_sfc != current_sfc_low_16)  /* sanity check */
                {
                    uint32_t sfc_ms, sfc_ls;
                    uint8_t  sfc_valid;
                    ag_drv_ngpon_rx_gen_sync_sfc_get(&sfc_ms, &sfc_valid);
                    ag_drv_ngpon_rx_gen_sync_sfc_ls_get(&sfc_ls);

                    /*
                     *   If the value in lower 16 bits of current SFC is less then required 
                     *   we have to increment bit 17 
                     */ 
                    if (current_sfc_low_16 < scheduled_sfc) 
                    {
                        ls_value = (ls_value & 0xFFFF0000) | scheduled_sfc ;
                    }
                    else
                    {
                        ls_value &= 0xFFFF0000 ;
                        ls_value += 0x10000 ;
                        ls_value |= scheduled_sfc ;
                    }

                    p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id, 
                          "Setting SFC Indication to MS=0x%08x LS=0x%08x (scheduled SFC 0x%04X) while SFC counter is MS=0x%08x LS=0x%08x valid=%d",
                          ms_value, ls_value, scheduled_sfc, sfc_ms, sfc_ls, valid) ;


                    if (BDMF_TRUE == ngpon_is_sfc_indication_int_set())
                    {
                        ngpon_clear_sfc_indication_int() ;
                    }
                  
                    stack_error = ngpon_set_sfc_indication_int (ms_value, ls_value) ;
                    if (stack_error != PON_NO_ERROR)
                    {
                        p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id, 
                           "Failed to read SFC_MSB value") ;
                        return ;
                    }
  
                    write_tuning_response(operation_code_ack, response_ack, 
                        params->p_ploam_message->sequence_number);


                    g_xgpon_db.link_params.ch_profile.target_ds_channel = (ntohl(ds_channel_profile_p->pon_id) & 0xF) + 1 ;
                    g_xgpon_db.link_params.ch_profile.target_us_channel = us_channel_profile_p->uwlch_id.channel_id + 1 ;
                    g_xgpon_db.link_params.ch_profile.rollback_available = 
                        params->p_ploam_message->message.tuning_control.rollback_flag.rollback_flag ;

                    g_xgpon_db.link_params.ch_profile.tuning_resp_seqno = params->p_ploam_message->sequence_number;

                    /* Move to O5.2 sub state */
                    g_xgpon_db.link_params.sub_sm_state = o5_2_pending_sub_state;

                    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
                        "Link Sub State transition: o5_1_associated_sub_state -> o5_2_pending_sub_state");
                    return ;
                 
                }
                else
                   response_code = response_nack_int_sfc ;
            }
            else
               response_code = response_nack_ds_void ;
        }
        else
           response_code = response_nack_ds_part ; 
    }  
    else
    {
    	response_code = 0;
    	if (ds_channel_profile_p == NULL)
    	{
    	    response_code |= response_nack_ds_albl;
    	}
    	if (us_channel_profile_p == NULL)
    	{
    	    response_code |= response_nack_us_albl ;
    	}
    }

    p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
        "Tuning_Control request rejected, reason %d", response_code);

    /* Reject the request and remain in the same state */
    write_tuning_response(operation_code_nack, response_code, 
       params->p_ploam_message->sequence_number);

}

static void sm_o5_pending_sub_state_o5_2_sfc_match_event(
    o5_sub_states *sub_state, sub_sm_params *params)
{
    PON_ERROR_DTE   rc ;
    bdmf_error_t    timer_failure ;

       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
             "Current DS Channel - %d", g_xgpon_db.link_params.ch_profile.current_ds_channel);     

    /*
     *  Tune Rx
     */
    p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id, 
          "Tuning started ... %d", g_xgpon_db.link_params.ch_profile.target_ds_channel) ; 

    rc = ngpon_tune_rx (g_xgpon_db.link_params.ch_profile.target_ds_channel) ;
    if (rc !=  PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id, 
              "Failed to tune Rx to channel %d", g_xgpon_db.link_params.ch_profile.target_ds_channel) ;

        /*
         *  At least try to return back
         */

        ngpon_tune_rx (g_xgpon_db.link_params.ch_profile.current_ds_channel) ; 
        g_xgpon_db.link_params.sub_sm_state = o5_1_associated_sub_state ;
        return ;
    }
    /*
     *  Tune Tx in advance
     */
    rc = ngpon_tune_tx (g_xgpon_db.link_params.ch_profile.target_us_channel) ;
    if (rc !=  PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id, 
              "Failed to tune Tx to channel %d", g_xgpon_db.link_params.ch_profile.target_us_channel) ;

        /*
         *  At least try to return back
         */
        ngpon_tune_rx (g_xgpon_db.link_params.ch_profile.current_ds_channel) ; 
        ngpon_tune_tx (g_xgpon_db.link_params.ch_profile.current_us_channel) ; 
        g_xgpon_db.link_params.sub_sm_state = o5_1_associated_sub_state ;
        return ;
    }
    /*
     * Start T04
     */
    timer_failure = bdmf_timer_start(&g_xgpon_db.os_resources.to4_timer_id,
        NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.to4_timer_timeout));
    if (timer_failure != BDMF_ERR_OK)
    {
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to start TO4 timer");
    }
    /*
     * Start DSYNC detection timer
     */
    timer_failure = bdmf_timer_start (&g_xgpon_db.os_resources.ds_sync_check_timer_id, 
        NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.ds_sync_check_timer_timeout));
    if (timer_failure != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to start ds_sync_check timer");
    }
    /*
     * Clear interrupt enable bit
     */
    ngpon_clear_sfc_indication_int () ;
}

static void p_intermittent_lods_o6_state_timer_ds_sync_check_expire_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    p_sm_general_state_timer_ds_sync_check_expire_event (xi_operation_sm , xi_operation_sm_params );
}

static void p_intermittent_lods_o6_state_ds_frame_sync_event( 
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params) 
{
    bdmf_error_t     xgpon_error ;

    bdmf_timer_stop ( &g_xgpon_db.os_resources.to2_timer_id ) ;
    bdmf_timer_stop(&g_xgpon_db.os_resources.to3_timer_id);
    bdmf_timer_stop ( &g_xgpon_db.os_resources.ds_sync_check_timer_id ) ;

    xgpon_error = send_message_to_pon_task(PON_EVENT_OAM, PON_INDICATION_LOF, 
        CE_OAM_ALARM_STATUS_OFF);
    if ( xgpon_error != BDMF_ERR_OK )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send oam indication: error %d !", xgpon_error);
    }

    udelay( 2 ) ;

    ngpon_wan_post_init (0); 
    /* Enable output in gearbox */
    ngpon_wan_top_enable_transmitter(1);
    gpon_ngpon_wan_top_set_lbe_invert(g_xgpon_db.tx_params.lbe_invert_bit) ;

    /* Change internal state to Operational (O5) */
    xi_operation_sm -> activation_state = OPERATION_STATE_O5 ;
    g_xgpon_db.link_params.sub_sm_state = o5_1_associated_sub_state;

    /* Only support counter operating channel */
    g_pon_lods_pm_counters.lods_restored_operation_twdm_counter++;

    /* Report event "Link state transition" with state parameter */
    xgpon_error = send_message_to_pon_task ( PON_EVENT_LINK_STATE_TRANSITION ,
        OPERATION_STATE_O6, OPERATION_STATE_O5);
    if ( xgpon_error != BDMF_ERR_OK )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send link state transition indication: error %d", xgpon_error);
    }

#ifdef CONFIG_BCM_TIME_SYNC_PON_UNSTABLE
        /* Time Synchronization: PON Unstable - Set signal On */
        xgpon_error = time_sync_pon_unstable_set(1);
        if (xgpon_error != PON_NO_ERROR)
        {
            p_log( ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Time Synchronizatioin PON Unstable: Failed to set signal On, rc=%d", 
                xgpon_error);
        }
#endif 

        if (g_in_rollback)
        {
             /*
              * If rollback after unsuccessful tuning is in progress - transmit "rolling back" response 
              * at every opportunity
              */
             write_tuning_response (operation_code_rollback, response_rollback_com_ds,
                g_xgpon_db.link_params.ch_profile.tuning_resp_seqno); 

        }
}

static void go_to_o1_from_oper_state (NGPON_OPERATION_SM *xi_operation_sm, 
   OPERATION_SM_PARAMS *xi_operation_sm_params, LINK_OPERATION_STATES_DTE old_state)
{
    NGPON_ONU_LINK_STATE_TRANSITION_INDICATION_PARAMS link_states;

#ifdef G989_3_AMD1
    PON_ERROR_DTE timer_start_error;
#endif
    /*
     * To be able to hunt for another NGPON2 channel
     */ 
    if (!ngpon2_no_hunt_mode) 
    {
    g_xgpon_db.link_params.init_tuning_done = BDMF_FALSE ;
    }
    clear_onu_config_7() ;

    link_states.onu_old_link_state = old_state;
    link_states.onu_new_link_state = OPERATION_STATE_O1;

#ifdef G989_3_AMD1

    if ((g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGPON) &&
        (g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGS))
    {

        if (!g_xgpon_db.os_resources.tcpi_timer_is_running)
        {   
            timer_start_error = bdmf_timer_start (&g_xgpon_db.os_resources.tcpi_timer_id, 
                 NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.tcpi_tout));
            g_xgpon_db.os_resources.tcpi_timer_is_running = BDMF_TRUE;
        } 

    }
#endif

    p_sm_general_state_lods_event(xi_operation_sm, xi_operation_sm_params ,link_states);
}

static void p_intermittent_lods_o6_state_timer_to2_expire_event( 
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params ) 
{
    PON_ERROR_DTE xgpon_error;
    PON_CONFIGURE_OMCI_PORT_ID_INDICATION omci_port_ind;

    bdmf_timer_stop(&g_xgpon_db.os_resources.ds_sync_check_timer_id);


    /*
     * Disable output in gearbox; NGPON MAC Tx will remain enabled
     */
    ngpon_wan_top_enable_transmitter(0);

    /* Remove OMCI port */
    omci_port_ind.port_control = BDMF_FALSE;
    omci_port_ind.port_id = g_xgpon_db.onu_params.onu_id;
    xgpon_error = send_message_to_pon_task(PON_EVENT_USER_INDICATION,
       PON_INDICATION_CONFIGURE_OMCI_PORT_ID_FILTER, omci_port_ind);
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to send configure OMCI port indication to pon task: error %d !", 
            xgpon_error);
    }

    xgpon_error = send_message_to_pon_task ( PON_EVENT_OAM , PON_INDICATION_LOF, CE_OAM_ALARM_STATUS_ON ) ;
    if (xgpon_error != BDMF_ERR_OK)
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send oam indication: error %d !", xgpon_error ) ;

    g_pon_lods_pm_counters.lods_reactivations++;

    go_to_o1_from_oper_state (xi_operation_sm, xi_operation_sm_params, OPERATION_STATE_O6) ;
}


static void p_intermittent_lods_o6_state_timer_to3_expire_event(NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    bdmf_error_t rc;

   
    /* Discard burst profile parameters (Set 1) */
    rc = clear_burst_profiles();
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to discard busrt profiles, error %d", rc);
    }

    /* Start TO4 - Downstream Tuning timer */
    rc = bdmf_timer_start(&g_xgpon_db.os_resources.to4_timer_id, 
        NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.to4_timer_timeout));
    if (rc != BDMF_ERR_OK)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to start TO4 timer");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start TO4 timer");
    } 


    /*
     * Start DSYNC detection timer 
     */
    if ( bdmf_timer_start (&g_xgpon_db.os_resources.ds_sync_check_timer_id,
        NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.ds_sync_check_timer_timeout)) != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to start ds_sync_check timer");
    }

    /*
     * Protection_Control PLOAM has configured protection channel
     */
    g_xgpon_db.link_params.ch_profile.target_ds_channel = (g_xgpon_db.onu_params.protect_ds_pon_id & 0xF) + 1;
    g_xgpon_db.link_params.ch_profile.target_us_channel = (g_xgpon_db.onu_params.protect_us_pon_id & 0xF) + 1;
    g_xgpon_db.link_params.ch_profile.rollback_available = BDMF_FALSE ;



    ngpon_wan_post_init (0); 
    /* Enable output in gearbox */
    ngpon_wan_top_enable_transmitter(1);   /* Tx was disabled on entry to O6; enable it before switching to Protection Channel */
    gpon_ngpon_wan_top_set_lbe_invert(g_xgpon_db.tx_params.lbe_invert_bit) ;
    /*
     *  Tune Rx
     */
    rc = ngpon_tune_rx (g_xgpon_db.link_params.ch_profile.target_ds_channel) ;
    if (rc !=  PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id, 
              "Failed to tune Rx to channel %d", g_xgpon_db.link_params.ch_profile.target_ds_channel) ;

        /*
         *  At least try to return back
         */
        ngpon_tune_rx (g_xgpon_db.link_params.ch_profile.current_ds_channel) ;

        go_to_o1_from_oper_state (xi_operation_sm, xi_operation_sm_params, OPERATION_STATE_O6) ;
        return ;
    }
    /*
     *  Tune Tx in advance
     */
    rc = ngpon_tune_tx (g_xgpon_db.link_params.ch_profile.target_us_channel) ;
    if (rc !=  PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id, 
              "Failed to tune Tx to channel %d", g_xgpon_db.link_params.ch_profile.target_us_channel) ;

        /*
         *  At least try to return back
         */
        ngpon_tune_rx (g_xgpon_db.link_params.ch_profile.current_ds_channel) ; 
        ngpon_tune_tx (g_xgpon_db.link_params.ch_profile.current_us_channel) ; 

        go_to_o1_from_oper_state  (xi_operation_sm, xi_operation_sm_params, OPERATION_STATE_O6) ;
        return ;
    }
  
    /* Report event "Link state transition: O6->O8*/
    rc = send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
        OPERATION_STATE_O6, OPERATION_STATE_O8);
    if (rc != BDMF_ERR_OK)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to send PON task message");
    }

    /* Move to O8.1 sub state */
    xi_operation_sm->activation_state = OPERATION_STATE_O8;
    g_xgpon_db.link_params.sub_sm_state = o8_1_offsync_sub_state;
   
    g_pon_lods_pm_counters.lods_reactivations++;
    g_pon_lods_pm_counters.lods_restored_protection_twdm_counter++;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link Sub State transition: OPERATION_STATE_O6 -> o8_1_offsync_sub_state");
}

static void p_sm_emergency_stop_o7_state_timer_ds_sync_check_expire_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    p_sm_general_state_timer_ds_sync_check_expire_event (xi_operation_sm , xi_operation_sm_params );
}

static void p_sm_emergency_stop_o7_state_disable_serial_number_ploam_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    bdmf_error_t     xgpon_error ;
    NGPON_DS_DISABLE_SERIAL_NUMBER_PLOAM* disable_sn ; 
    disable_sn = (NGPON_DS_DISABLE_SERIAL_NUMBER_PLOAM* ) & xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number ;

    if ( ( disable_sn->control == SERIAL_NUMBER_BROADCAST_ENABLE ) || 
         ( ( disable_sn->control == SERIAL_NUMBER_UNICAST_ENABLE ) && 
         ( BDMF_TRUE == onu_serial_number_match ( g_xgpon_db.onu_params.sn.serial_number ,
                                              disable_sn->serial_number ) ) ) )

    {
        /* Report event "Link state transition" with state parameter */
        xgpon_error = send_message_to_pon_task ( PON_EVENT_LINK_STATE_TRANSITION ,
                                                   OPERATION_STATE_O7,
                                                   OPERATION_STATE_O1 ) ;

        /* Change internal state to OPERATION_STATE_O1 and enable channel hunting for NGPON2 */
        g_xgpon_db.link_params.init_tuning_done = BDMF_FALSE ;
        xi_operation_sm -> activation_state = OPERATION_STATE_O1 ;
        g_xgpon_db.link_params.sub_sm_state = o1_1_offsync_sub_state;

        /* Set LODS to ON */
        xi_operation_sm_params->lods_state = BDMF_TRUE ;
        p_operation_sm ( OPERATION_EVENT_DS_FRAME_SYNC, xi_operation_sm_params ) ;
    }
}

static void p_sm_emergency_stop_o7_state_detect_lods_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
   bdmf_error_t     xgpon_error ;
   xgpon_error = send_message_to_pon_task ( PON_EVENT_OAM , PON_INDICATION_LOF, CE_OAM_ALARM_STATUS_ON ) ;
   if ( xgpon_error != BDMF_ERR_OK )
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send oam indication: error %d !", xgpon_error ) ;

   p_sm_general_state_timer_ds_sync_check_expire_event (xi_operation_sm , xi_operation_sm_params );
}

static void p_sm_ds_tuning_o8_state_timer_ds_sync_check_expire_event( NGPON_OPERATION_SM * xi_operation_sm, OPERATION_SM_PARAMS * xi_operation_sm_params ) 
{
    p_sm_general_state_timer_ds_sync_check_expire_event (xi_operation_sm , xi_operation_sm_params);
}

static void p_sm_ds_tuning_o8_state_ds_frame_sync_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o8_sub_sm[g_xgpon_db.link_params.sub_sm_state][o8_ds_frame_sync_event]((o8_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
}

static void p_sm_ds_tuning_o8_state_timer_to4_expire_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sub_sm_params params;

    g_pon_tuning_req_pm_counters.requests_failed_to4_exp++;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o8_sub_sm[g_xgpon_db.link_params.sub_sm_state][timer_to4_expire_event]((o8_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
}

static void p_sm_ds_tuning_o8_state_detect_lods_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o8_sub_sm[g_xgpon_db.link_params.sub_sm_state][o8_detect_lods_event]((o8_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
    g_pon_lods_pm_counters.lods_reactivations++;
}

static void p_sm_ds_tuning_o8_state_channel_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o8_sub_sm[g_xgpon_db.link_params.sub_sm_state][o8_channel_profile_ploam_event]((o8_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
}

static void p_sm_ds_tuning_o8_state_burst_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    PON_ERROR_DTE    xgpon_error ;
    
    xgpon_error = ngpon_general_state_burst_profile_event(&xi_operation_sm_params->ploam_message_ptr->message.burst_profile);
    if ( PON_NO_ERROR != xgpon_error  )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                 "Unable to handle busrt profile, error %d !", xgpon_error ) ;
    }
}

static void p_sm_ds_tuning_o8_state_system_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o8_sub_sm[g_xgpon_db.link_params.sub_sm_state][o8_system_profile_ploam_event]((o8_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
}

static void p_sm_ds_tuning_o8_state_protection_control_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o8_sub_sm[g_xgpon_db.link_params.sub_sm_state][protection_control_ploam_event]((o8_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
}

static void p_sm_ds_tuning_o8_state_assigned_onu_id_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o8_sub_sm[g_xgpon_db.link_params.sub_sm_state][assigned_onu_id_ploam_event]((o8_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
}

static void p_sm_ds_tuning_o8_state_ranging_time_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o8_sub_sm[g_xgpon_db.link_params.sub_sm_state][ranging_time_ploam_event]((o8_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
}

static void p_sm_ds_tuning_o8_state_deactivate_onu_id_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o8_sub_sm[g_xgpon_db.link_params.sub_sm_state][deactivate_onu_id_ploam_event]((o8_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
}

static void p_sm_ds_tuning_o8_state_disable_serial_number_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sub_sm_params params;

    params.p_ploam_message = xi_operation_sm_params->ploam_message_ptr;
    o8_sub_sm[g_xgpon_db.link_params.sub_sm_state][o8_disable_serial_number_ploam_event]((o8_sub_states *)&g_xgpon_db.link_params.sub_sm_state, &params);
}

static void p_sm_init_o1_state_protection_control_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sm_general_protection_control_ploam_message_received(xi_operation_sm_params->ploam_message_ptr);
}

static void p_sm_sn_o23_state_protection_control_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sm_general_protection_control_ploam_message_received(xi_operation_sm_params->ploam_message_ptr);
}

static void p_sm_ranging_o4_state_protection_control_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sm_general_protection_control_ploam_message_received(xi_operation_sm_params->ploam_message_ptr);
}

static void p_sm_operation_o5_state_protection_control_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sm_general_protection_control_ploam_message_received(xi_operation_sm_params->ploam_message_ptr);
}

static void sm_o8_profile_learning_sub_state_o8_2_protection_control_ploam_event(
    o8_sub_states *sub_state, sub_sm_params *params)
{
    sm_general_protection_control_ploam_message_received(params->p_ploam_message);
}

static void p_sm_sn_o23_state_system_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{ 
    save_system_profile_to_db(&xi_operation_sm_params->ploam_message_ptr->message.system_profile);
}

static void p_sm_ranging_o4_state_system_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{ 
    save_system_profile_to_db(&xi_operation_sm_params->ploam_message_ptr->message.system_profile);
}

static void p_sm_operation_o5_state_system_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    /*
     * If ONU is doing rollback after unsuccessfull tuning to another channel - transmit rollback response several times 
     * once the channel DS is stable
     * System_Profile PLOAM reception (in the channel where System_Profile is transmitted periodically)
     * is good indication that the channel is OK to transmit upstream
     *
     * Transmit Tuning_Response(Rollback) several times to maximize OLT chances to catch it
     */   
    if (g_in_rollback)
    {
       write_tuning_response (operation_code_rollback, response_rollback_com_ds,
          g_xgpon_db.link_params.ch_profile.tuning_resp_seqno); 
       sys_profiles_after_rollback_cnt ++ ;       
    }
    if (sys_profiles_after_rollback_cnt == 10)
    {
        sys_profiles_after_rollback_cnt = 0 ;
        g_in_rollback = BDMF_FALSE ;
    }

    save_system_profile_to_db(&xi_operation_sm_params->ploam_message_ptr->message.system_profile);
}

static void sm_o8_profile_learning_sub_state_o8_2_system_profile_ploam_event(
    o8_sub_states *sub_state, sub_sm_params *params)
{
    save_system_profile_to_db(&params->p_ploam_message->message.system_profile);
    sm_o8_profile_learning_sub_state_o8_2_dwlch_ok_to_work(params);
}

static void sm_o8_profile_learning_sub_state_o8_2_burst_profile_ploam_event(
    o8_sub_states *sub_state, sub_sm_params *params)
{
    PON_ERROR_DTE    xgpon_error ;
    
    xgpon_error = ngpon_general_state_burst_profile_event(&params->p_ploam_message->message.burst_profile);
    if ( PON_NO_ERROR != xgpon_error  )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                 "Unable to handle busrt profile, error %d !", xgpon_error ) ;
    }
    sm_o8_profile_learning_sub_state_o8_2_dwlch_ok_to_work(params);
}

static void p_sm_sn_o23_state_channel_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    save_channel_profile_to_db(&xi_operation_sm_params->ploam_message_ptr->message.channel_profile);
}

static void p_sm_ranging_o4_state_channel_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    save_channel_profile_to_db(&xi_operation_sm_params->ploam_message_ptr->message.channel_profile);
}

static void p_sm_operation_o5_state_channel_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    save_channel_profile_to_db(&xi_operation_sm_params->ploam_message_ptr->message.channel_profile);
}

static void sm_o8_profile_learning_sub_state_o8_2_channel_profile_ploam_event(
    o8_sub_states *sub_state, sub_sm_params *params)
{
    save_channel_profile_to_db(&params->p_ploam_message->message.channel_profile);
    sm_o8_profile_learning_sub_state_o8_2_dwlch_ok_to_work(params);
}

static void p_sm_sn_o23_state_adjust_tx_wavelength_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    /* Adjust transmitter wavelength */
}

static void p_sm_operation_o5_state_adjust_tx_wavelength_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    /* Adjust transmitter wavelength */
}

static void p_sm_ranging_o4_state_adjust_tx_wavelength_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    /* Adjust transmitter wavelength */
}

static void p_sm_us_tuning_o9_state_adjust_tx_wavelength_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
     /* Adjust transmitter wavelength */
}

static void p_sm_us_tuning_o9_state_assigned_onu_id_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sm_general_assign_sn_ploam_message_received(xi_operation_sm_params->ploam_message_ptr->sequence_number,
        &xi_operation_sm_params->ploam_message_ptr->message.assign_onu_id,
        &xi_operation_sm->activation_state);
}

static void p_sm_operation_o5_state_power_consumption_inquire_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    /* TBD */
}

static void p_sm_operation_o5_state_assigned_onu_id_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sm_general_assign_sn_ploam_message_received(xi_operation_sm_params->ploam_message_ptr->sequence_number,
        &xi_operation_sm_params->ploam_message_ptr->message.assign_onu_id,
        &xi_operation_sm->activation_state);
}

static void p_sm_ranging_o4_state_assigned_onu_id_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sm_general_assign_sn_ploam_message_received(xi_operation_sm_params->ploam_message_ptr->sequence_number,
        &xi_operation_sm_params->ploam_message_ptr->message.assign_onu_id,
        &xi_operation_sm->activation_state);
}

static void sm_o8_offsync_sub_state_o8_1_ds_frame_sync_event(
    o8_sub_states *sub_state, sub_sm_params *params)
{
    /* Clear System Profiles and Channels profiles (Paramter Set 7) */
    clear_onu_config_7();

    if (!simplified_rollback_flag)
    {
       /* This code was added to reset profiles.  The sm_o8_profile_learning_sub_state_o8_2_dwlch_ok_to_work
          function was checking these and it always succeeded the first time because they
          were already set during the previous 01.2 learning state.  Also found that the
          transmitter had to be enabled for protection switching to work as it was
          disabled in the lods logic. */
       clear_burst_profiles();
       /* Wait for new set of ploams to arrive. */
       g_xgpon_db.link_params.burst_profiles.burst_profile_received = BDMF_FALSE;
       g_xgpon_db.link_params.system_profile.system_profile_received = BDMF_FALSE;
       g_xgpon_db.link_params.ch_profile.channel_profile_received = BDMF_FALSE;
    }

    ngpon_wan_post_init (0);
    /* Enable output in gearbox */
    ngpon_wan_top_enable_transmitter(1);
    gpon_ngpon_wan_top_set_lbe_invert(g_xgpon_db.tx_params.lbe_invert_bit) ;



    /* Move to O8.2 sub state */
    g_xgpon_db.link_params.sub_sm_state = o8_2_profile_learning_sub_state;

    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link Sub State transition: o8_1_offsync_sub_state -> o8_2_profile_learning_sub_state");

    /* Check if moving to O9 [temp solution - TBD] */
    sm_o8_profile_learning_sub_state_o8_2_dwlch_ok_to_work(params);
}

static void sm_o8_offsync_sub_state_o8_1_detect_lods_event(
    o8_sub_states *sub_state, sub_sm_params *params)
{
    /* Do nothing here - wait until DS sync attained or TO4 expired */
}

static void sm_general_timer_to4_expire_event (o8_sub_states *sub_state, sub_sm_params *params)
{
    PON_ERROR_DTE rc;
    static char  *old_sub_state_str[] = {
        "o8_1_offsync_sub_state",
        "o8_2_profile_learning_sub_state"
    } ;

    int sub_state_indx = (sub_state == o8_1_offsync_sub_state)? 0 : 1 ;

    /* Timer TO4 is used to abort an unsuccessful wavelength channel tuning 
       operation in state O8, when no suitable downstream wavelength channel is found. */

    if (!g_xgpon_db.link_params.ch_profile.rollback_available)
    {

        /* Clear bursts profiles, ONU-ID, default Alloc-ID, default port ID, 
           assigend Alloc-IDs and EqD (Paramter Set 5) */
        rc = clear_onu_config_5();
        if (rc != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                  "Unable to clear_onu_config due to error %d", rc);
        }

        /* Report event "Link state transition: O8->O1*/
        rc = send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
            OPERATION_STATE_O8, OPERATION_STATE_O1);
        if (rc != BDMF_ERR_OK)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Failed to send PON task message");
        }
        /*
                 Enable hunting if we are returning to O1
         */

        g_xgpon_db.link_params.init_tuning_done = BDMF_FALSE ;

        /* Move to O1.1 sub state */
        g_xgpon_db.link_params.operation_sm.activation_state = OPERATION_STATE_O1;
        g_xgpon_db.link_params.sub_sm_state = o1_1_offsync_sub_state;
       
        p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
              "Link Sub State transition: %s -> o1_1_offsync_sub_state", old_sub_state_str[sub_state_indx]);
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id, 
            "Failed to attain DS Sync, Rollback permitted, going back");

        bdmf_timer_stop(&g_xgpon_db.os_resources.ds_sync_check_timer_id);

       /*
        * Tune back DS and US
        */
       ngpon_tune_rx (g_xgpon_db.link_params.ch_profile.current_ds_channel) ; 

       g_xgpon_db.link_params.ch_profile.current_us_channel = g_xgpon_db.link_params.ch_profile.current_ds_channel ;
       ngpon_tune_tx (g_xgpon_db.link_params.ch_profile.current_us_channel) ;

       g_xgpon_db.link_params.ch_profile.target_ds_channel = 0 ;
       g_xgpon_db.link_params.ch_profile.target_us_channel = 0 ;

       if (simplified_rollback_flag)
       {
           /* Report event "Link state transition: O8->O5*/
           rc = send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
                OPERATION_STATE_O8, OPERATION_STATE_O5);
           if (rc != PON_NO_ERROR)
           {
               p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                   "Failed to send PON task message");
           }
 
           g_xgpon_db.link_params.sub_sm_state = o5_1_associated_sub_state ;
           g_xgpon_db.link_params.operation_sm.activation_state = OPERATION_STATE_O5;

           p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
                 "Link Sub State transition: %s -> o5_1_associated_sub_state", old_sub_state_str[sub_state_indx]);

       }
       else
       {
           rc = send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
               OPERATION_STATE_O8, OPERATION_STATE_O9);
           if (rc != PON_NO_ERROR)
           {
               p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                  "Failed to send PON task message");
           }

           g_xgpon_db.link_params.operation_sm.activation_state = OPERATION_STATE_O9;

           p_log(ge_onu_logger.sections.stack.operating.state_change_id,
                 "Link Sub State transition: %s -> OPERATION_STATE_O9", old_sub_state_str[sub_state_indx]);

           /* Start US tuning timer TO5 */
           rc = bdmf_timer_start(&g_xgpon_db.os_resources.to5_timer_id, 
              NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.to5_timer_timeout));
           if (rc != BDMF_ERR_OK)
           {
               p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to start TO5 timer");
           }
           else
           {
              p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start TO5 timer");
           }
       }

       g_in_rollback = BDMF_TRUE ;
       write_tuning_response (operation_code_rollback, response_rollback_com_ds,
          g_xgpon_db.link_params.ch_profile.tuning_resp_seqno); 

    }

    ngpon_flush_traffic_alloc_table ();
}

static void sm_o8_offsync_sub_state_o8_1_timer_to4_expire_event(
    o8_sub_states *sub_state, sub_sm_params *params)
{
    sm_general_timer_to4_expire_event (sub_state, params) ;
}

static void sm_o8_profile_learning_sub_state_o8_2_detect_lods_event(
    o8_sub_states *sub_state, sub_sm_params *params)
{
    PON_ERROR_DTE rc;

    if (!simplified_rollback_flag)
    {
       /* Discard burst profile parameters. */
       rc = clear_burst_profiles();
       if (rc != PON_NO_ERROR)
       {
           p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
               "Unable to discard burst profiles due to error %d", rc);
       }
    }

    /* Move to O8.1 (Off sync) */
    g_xgpon_db.link_params.sub_sm_state = o8_1_offsync_sub_state;
 
    p_log(ge_onu_logger.sections.stack.operating.state_change_id, 
        "Link Sub State transition: o8_2_profile_learning_sub_state -> o8_1_offsync_sub_state");
}


static void sm_o8_profile_learning_sub_state_o8_2_timer_to4_expire_event(
    o8_sub_states *sub_state, sub_sm_params *params)
{
    sm_general_timer_to4_expire_event (sub_state, params) ;
}

static void sm_o8_profile_learning_sub_state_o8_2_assigned_onu_id_ploam_event(
    o8_sub_states *sub_state, sub_sm_params *params)
{
    sm_general_assign_sn_ploam_message_received(params->p_ploam_message->sequence_number,
        &params->p_ploam_message->message.assign_onu_id,
        &g_xgpon_db.link_params.operation_sm.activation_state);
}

static void sm_o8_profile_learning_sub_state_o8_2_ranging_time_ploam_event(
    o8_sub_states *sub_state, sub_sm_params *params)
{     
    bdmf_timer_stop(&g_xgpon_db.os_resources.to4_timer_id);
    general_ranging_time_ploam_event(params->p_ploam_message);
}

static void sm_o8_profile_learning_sub_state_o8_2_dwlch_ok_to_work(sub_sm_params *params)
{
    bdmf_error_t rc; 
    PON_ERROR_DTE xgpon_error ;

    if (g_xgpon_db.link_params.burst_profiles.burst_profile_received == BDMF_FALSE ||
        g_xgpon_db.link_params.system_profile.system_profile_received == BDMF_FALSE ||
        g_xgpon_db.link_params.ch_profile.channel_profile_received == BDMF_FALSE)
    {
         p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id,
             "DWLCH is not OK to work: Not all profiles received");     
         return;
    }

    /* Check reciever: DS wavelength is OK to work? */


    /*
     *     Either we had issues receiving all of the profiles from the OLT or it
     *  would send down a system profile, single channel profile, then burst profile so
     *  we'd get here but the received channel profile didn't contain the 'this' indicator
     *  so we bailed and went back to state 01.1 and ignored subsequent channel profile
     *  ploams.  Added check here for a usable channel profile. If not, we'd stay in
     *  state 8.2 and wait for valid channel profile to arrive or for O4 expiration.    
     */
    rc = ngpon_init_analyze_sys_chan_profiles() ;
    if (rc != PON_NO_ERROR)
    {
  	  p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id,
  	    "No valid Channel Profile received, error %d", rc);

        return;
    }


    /* Prepare tuning response PLOAM for O9 */
    write_tuning_response(operation_code_complete_u, response_complete_u,
        g_xgpon_db.link_params.ch_profile.tuning_resp_seqno); 
    /* -> Not sure about sequence_number, since the function can be
       called not as an answer for tuning request, but from other reasons [Hila] */


    /* Stop TO4 */
    bdmf_timer_stop(&g_xgpon_db.os_resources.to4_timer_id);

    /* Start US tuning timer TO5 */
    rc = bdmf_timer_start(&g_xgpon_db.os_resources.to5_timer_id, 
        NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.to5_timer_timeout));
    if (rc != BDMF_ERR_OK)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to start TO5 timer");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start TO5 timer");
    }
    if (zero_eqd_after_tuning)
    {
        /* 
         * Zero EqD - once tuned to target channel ONU has to set EqD to 0; OLT will do re-ranging
         */
         xgpon_error = ngpon_set_eqd ( 0, P_ABSOLUTE, SIGN_POSITIVE  );
         if (PON_NO_ERROR != xgpon_error) 
         {
            p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                "Unable to write EQD to HW. error %lu",xgpon_error);
         }
    }

    bdmf_timer_stop(&g_xgpon_db.os_resources.ds_sync_check_timer_id);

    /* Change link state to O9 */
    g_xgpon_db.link_params.operation_sm.activation_state = OPERATION_STATE_O9;

    /* Report event "Link state transition" with state parameter */
    rc = send_message_to_pon_task(PON_EVENT_LINK_STATE_TRANSITION,
        OPERATION_STATE_O8, OPERATION_STATE_O9);
    if (rc != BDMF_ERR_OK)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send user indication: error %d", rc);
    }


}

static void p_sm_us_tuning_o9_state_ranging_time_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{     
    bdmf_timer_stop(&g_xgpon_db.os_resources.to5_timer_id);

    general_ranging_time_ploam_event(xi_operation_sm_params->ploam_message_ptr);
}

static void sm_o8_profile_learning_sub_state_o8_2_deactivate_onu_id_ploam_event(
    o8_sub_states *sub_state, sub_sm_params *params)
{
    bdmf_timer_stop(&g_xgpon_db.os_resources.to4_timer_id);
    sm_general_deactivate_onu_id_ploam_event(OPERATION_STATE_O8);
}

static void p_sm_us_tuning_o9_state_deactivate_onu_id_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    bdmf_timer_stop(&g_xgpon_db.os_resources.to5_timer_id);
    sm_general_deactivate_onu_id_ploam_event(OPERATION_STATE_O9);
}

static void sm_o8_profile_learning_sub_state_o8_2_disable_serial_number_ploam_event(
    o8_sub_states *sub_state, sub_sm_params *params)
{
    p_sm_general_state_disable_serial_number_ploam_event(
        &g_xgpon_db.link_params.operation_sm.activation_state, 
        &params->p_ploam_message->message.disable_serial_number, OPERATION_STATE_O8);
}

static void p_sm_us_tuning_o9_state_disable_serial_number_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    p_sm_general_state_disable_serial_number_ploam_event(
        &g_xgpon_db.link_params.operation_sm.activation_state, 
        &xi_operation_sm_params->ploam_message_ptr->message.disable_serial_number, OPERATION_STATE_O9);
}

static void p_sm_us_tuning_o9_state_assign_alloc_id_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    general_assigned_alloc_id_ploam_event(xi_operation_sm_params->ploam_message_ptr);
    o9_tuning_confirmation_event();
}

static void p_sm_us_tuning_o9_state_sleep_allow_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    p_log(ge_onu_logger.sections.stack.downstream_ploam.sleep_allow_message_id,
        "Not supported");
    o9_tuning_confirmation_event();
}

static void p_sm_us_tuning_o9_state_key_control_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{ 
    memcpy(&g_xgpon_db.link_params.key_sm.aes_message_ptr, 
        xi_operation_sm_params->ploam_message_ptr, sizeof(NGPON_DS_PLOAM));
    if (xi_operation_sm_params->ploam_message_ptr->message.key_control.control_flag.control == KEY_CONTROL_GENERATE_NEW_KEY)
    {
        p_key_sm(KEY_EVENT_NEW_KEY_REQUEST, 
            xi_operation_sm_params->ploam_message_ptr->message.key_control.key_index.index);
    }
    else
    {
        p_key_sm(KEY_EVENT_CONFIRM_KEY_REQUEST, 
            xi_operation_sm_params->ploam_message_ptr->message.key_control.key_index.index);
    }

    o9_tuning_confirmation_event();
}

static void p_sm_us_tuning_o9_state_protection_control_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    sm_general_protection_control_ploam_message_received(xi_operation_sm_params->ploam_message_ptr);
    o9_tuning_confirmation_event();
}

static void p_sm_us_tuning_o9_state_request_registration_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    PON_ERROR_DTE rc;

   /* Write registration PLOAM to Normal PLOAM queue (Buf 0) */
   rc = write_registration_ploam_to_hw(g_xgpon_db.onu_params.onu_id,
       xi_operation_sm_params->ploam_message_ptr->sequence_number,
       BDMF_FALSE, NGPON_PLOAM_TYPE_NORMAL);
   if (rc != PON_NO_ERROR) 
   {
        p_log(ge_onu_logger.sections.stack.downstream_ploam.request_registration_message_id,
            "Unable to write registration ploam, error %d", rc);
   }

   o9_tuning_confirmation_event();
}

static void p_sm_us_tuning_o9_state_timer_to5_expire_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    PON_ERROR_DTE rc;
    
    /* Discard burst profile parameters, ONU ID, default Alloc-ID, default XGEM Port-ID, 
       assigned Allocs, Equalization delay, system profile, channel profile. */
    rc = clear_onu_config_5();
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear configuration due to error %d", rc);
    }
    
    /*
     * Failed to receive acknowledge from OLT
     * remain at destination channel and move to O1 state
     */
    g_pon_tuning_req_pm_counters.requests_failed_to5_exp++;
    g_pon_lods_pm_counters.lods_handshake_fail_protection_counter++;

    ngpon_tune_rx(g_xgpon_db.link_params.ch_profile.target_ds_channel);
    ngpon_tune_tx(g_xgpon_db.link_params.ch_profile.target_us_channel);

    g_xgpon_db.link_params.ch_profile.current_ds_channel = g_xgpon_db.link_params.ch_profile.target_ds_channel;
    g_xgpon_db.link_params.ch_profile.current_us_channel = g_xgpon_db.link_params.ch_profile.target_us_channel;
    write_idle_ploam_to_hw (g_xgpon_db.onu_params.onu_id);


    go_to_o1_from_oper_state  (xi_operation_sm, xi_operation_sm_params, OPERATION_STATE_O9) ;
       
    ngpon_flush_traffic_alloc_table ();


}


static void p_sm_us_tuning_o9_state_tuning_control_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    PON_ERROR_DTE rc;

    /* Expect Operation code: Complete_d */
    if (xi_operation_sm_params->ploam_message_ptr->message.tuning_control.operation_code != operation_code_complete_d) 
    { 
        p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id, 
            "Unexpected operation control for tuining control request");
        return;
    }
    /* Send an Acknowledgement PLOAM message,
       to indicate successful completion of the wavelength channel handover. */
    rc = write_ack_ploam_to_hw(g_xgpon_db.onu_params.onu_id,
        xi_operation_sm_params->ploam_message_ptr->sequence_number);
    if (rc == PON_NO_ERROR) 
    {
         p_log(ge_onu_logger.sections.stack.upstream_ploam.ack_message_id,
             "Acknowledgement PLOAM message was written to HW (Ack Sequence Number = %d),",
             "to indicate successful completion of the wavelength channel handover.",
             xi_operation_sm_params->ploam_message_ptr->sequence_number);
    }
    else 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to write ACK ploam, Error %d", rc);
    }
    rc = write_idle_ploam_to_hw (g_xgpon_db.onu_params.onu_id);   /* return back idle PLOAM sending */
    if (PON_NO_ERROR != rc) 
    {
      p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                     "Unable to write idle PLOAM to HW. error %lu",rc);
    }

    ngpon_flush_traffic_alloc_table ();

    o9_tuning_confirmation_event();
}

static void p_sm_us_tuning_o9_state_power_consumption_inquire_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    /* HANDLING PLOAM - TBD */

    o9_tuning_confirmation_event();
}

static void p_sm_us_tuning_o9_state_system_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{

    write_tuning_response(operation_code_complete_u, response_complete_u,
        g_xgpon_db.link_params.ch_profile.tuning_resp_seqno); 

    save_system_profile_to_db(&xi_operation_sm_params->ploam_message_ptr->message.system_profile);
}

static void p_sm_us_tuning_o9_state_burst_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    PON_ERROR_DTE    xgpon_error ;
    
    xgpon_error = ngpon_general_state_burst_profile_event(&xi_operation_sm_params->ploam_message_ptr->message.burst_profile);
    if ( PON_NO_ERROR != xgpon_error  )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                 "Unable to handle busrt profile, error %d !", xgpon_error ) ;
    }
}

static void p_sm_us_tuning_o9_state_channel_profile_ploam_event(
    NGPON_OPERATION_SM *xi_operation_sm, OPERATION_SM_PARAMS *xi_operation_sm_params)
{
    write_tuning_response(operation_code_complete_u, response_complete_u,
        g_xgpon_db.link_params.ch_profile.tuning_resp_seqno); 

    save_channel_profile_to_db(&xi_operation_sm_params->ploam_message_ptr->message.channel_profile);
}


/******************************************************************************/
/*                                                                            */
/* key exchange state machine functions                                       */
/*                                                                            */
/*                                                                            */
/******************************************************************************/


void p_key_invalid_cross  ( NGPON_AES_SM * xi_key_sm ,NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index ) 
{
    /* Invalid cross */
    p_log ( ge_onu_logger.sections.stack.key.invoke_state_machine_id, "key state machine:Invalid cross" ) ;
}

void p_key_start_key_exchange  ( NGPON_AES_SM * xi_key_sm ,NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index) 
{

    bdmf_error_t timer_error ;
    PON_ERROR_DTE ngpon_error ;
    NGPON_ENCRYP_DATA_KEY_NUM key_number ;
    uint8_t i ;
    ngpon_rx_enc_key_valid  key_valid ;


    /* Generate the new key */
    if ( g_xgpon_db.debug_params.use_predefined_key )
    {
        memcpy ( xi_key_sm->new_key, g_xgpon_db.debug_params.predefined_key ,
                         NGPON_DEFAULT_KEY_SIZE );
    }
    else
        generate_random_aes_key ( xi_key_sm->new_key );

   ngpon_error = ag_drv_ngpon_rx_enc_key_valid_get ( &key_valid ) ;
   if ( ngpon_error != PON_NO_ERROR )
   {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to get key valid (rx) : error %d !", ngpon_error ) ;
   }

    /*  Write new key to RX and set as valid  */
    if (xi_key_index == KEY_INDEX_FIRST_KEY) 
    {
        key_number = NGPON_ENCRYP_DATA_KEY_NUM_KEY_1_UNICAST_ONU_1 ;

        for ( i=0 ; i < 4 ; i ++ )
        {
           ngpon_error = ag_drv_ngpon_rx_enc_key1_onu1_set ( i, htonl(xi_key_sm->new_key[i]) ) ;
           if ( ngpon_error != PON_NO_ERROR )
           {
             p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write word %d of the key to rx: error %d !", i , ngpon_error ) ;
           }
        }
        key_valid.key1_onu1 = BDMF_TRUE ;
    }
    else
    {
        key_number = NGPON_ENCRYP_DATA_KEY_NUM_KEY_2_UNICAST_ONU_1 ;
        for ( i=0 ; i < 4 ; i ++ )
        {
           ngpon_error = ag_drv_ngpon_rx_enc_key2_onu1_set ( i, htonl(xi_key_sm->new_key[i]) ) ;
           if ( ngpon_error != PON_NO_ERROR )
           {
             p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write word %d of the key to rx: error %d !", i , ngpon_error ) ;
           }
        }
        key_valid.key2_onu1 = BDMF_TRUE ;
    }

    if (ngpon_error == PON_NO_ERROR)
    {
        /* Dump New key */
        p_log(ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id,
            "New Key has been written to ONU RX:");
        dump_key((uint8_t *)xi_key_sm->new_key, CE_XGPON_DEFAULT_AES_KEY_BYTES_SIZE);
    }

    /* Update DB */
     memcpy(g_xgpon_db.link_params.key_sm.new_key,
         (uint8_t *)xi_key_sm->new_key, CE_XGPON_DEFAULT_AES_KEY_BYTES_SIZE);
    
   ngpon_error = ag_drv_ngpon_rx_enc_key_valid_set ( &key_valid ) ;
   if ( ngpon_error != PON_NO_ERROR )
   {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to set key valid to rx : error %d !", ngpon_error ) ;
   }

    /* Generate the  kek_encrypted_key and key name*/
    ngpon_error = ngpon_generate_key_name_and_kek_encrypted_key((uint8_t * )xi_key_sm->new_key ,
                                                                (uint8_t * ) g_xgpon_db.onu_params.kek_in_bytes ,
                                                                (uint8_t * )xi_key_sm->key_name ,
                                                                (uint8_t * )xi_key_sm->kek_encrypted_key );
    if ( ngpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
              "Unable to ngpon_generate_key_name_and_kek_encrypted_key: error %d !", ngpon_error ) ;
    }
    else
    {
        /* Dump KEK key */
        p_log(ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id, "KEK key:");
        dump_key (  (uint8_t *)xi_key_sm->kek_encrypted_key , CE_XGPON_DEFAULT_AES_KEY_BYTES_SIZE );
    }


    p_log(ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id,
             "Sending New Key. Seq Num=0x%X ,Key_Index=%d , Key_Type=%d ",
            xi_key_sm->aes_message_ptr.sequence_number,xi_key_index ,KEY_REPROT_TYPE_NEW_KEY ) ;

    /* Send key report ploam (new) */
    ngpon_error = write_key_report_ploam_to_hw (g_xgpon_db.onu_params.onu_id,
                                                xi_key_sm->aes_message_ptr.sequence_number,
                                                xi_key_sm->kek_encrypted_key,
                                                xi_key_index ,KEY_REPROT_TYPE_NEW_KEY);
    if ( ngpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                 "Unable to write key report ploam: error %d !", ngpon_error ) ;
    }
    
    /*Start TK5 timer*/
    g_xgpon_db.os_resources.tk5_key_index = xi_key_index ;
    timer_error = bdmf_timer_start ( &g_xgpon_db.os_resources.tk5_timer_id ,
        NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.tk5_timer_timeout)) ;
    if ( timer_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to start tk5 timer");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, 
            "Start tk5 timer");
    }
    
    /* set handled key index */
    g_xgpon_db.link_params.key_sm.new_key_index = xi_key_index ;
    
    /* set key state machine */
    g_xgpon_db.link_params.key_sm.key_state = CS_KEY_STATE_WAIT_ACK_KN2 ;


}
void p_key_sm_init_kn0_state_new_key_request_event
    (NGPON_AES_SM *xi_key_sm, NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index) 
{
    bdmf_error_t timer_error;

    p_log(ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id, 
        "key_sm: Init (kn0 state) new key request event (key index: %d)", xi_key_index);

    if (g_xgpon_db.link_params.key_sm.active_key_index == xi_key_index ) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "request for key exchnage on the active key %d", xi_key_index);
        return;
    }

    /* start TK4 timer */
    g_xgpon_db.os_resources.tk4_key_index = xi_key_index ;
    timer_error = bdmf_timer_start ( &g_xgpon_db.os_resources.tk4_timer_id ,
        NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.tk4_timer_timeout)) ;
    if ( timer_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to start tk4 timer");
    }
    else
    {
        p_log ( ge_onu_logger.sections.stack.debug.timers_id, "Start tk4 timer" );
    }

    p_key_start_key_exchange ( xi_key_sm ,xi_key_index );
}

void p_key_sm_ack_state_new_key_request_event(NGPON_AES_SM *xi_key_sm, 
    NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index) 
{
    bdmf_error_t timer_error ;
    PON_ERROR_DTE ngpon_error ;

    /* Send key report ploam (new) */

    if ( g_xgpon_db.link_params.key_sm.new_key_index != xi_key_index )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "received event for key index %d while handling key index %d", 
            xi_key_index, g_xgpon_db.link_params.key_sm.new_key_index);

           return;

    }

    ngpon_error = write_key_report_ploam_to_hw (g_xgpon_db.onu_params.onu_id ,xi_key_sm->aes_message_ptr.sequence_number, xi_key_sm->kek_encrypted_key ,xi_key_index ,KEY_CONTROL_GENERATE_NEW_KEY );
    if ( ngpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write key report ploam: error %d !", ngpon_error ) ;
    }

    /*Start TK5 timer*/
    g_xgpon_db.os_resources.tk5_key_index = xi_key_index ;
    timer_error = bdmf_timer_start ( &g_xgpon_db.os_resources.tk5_timer_id ,
         NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.tk5_timer_timeout));
    if ( timer_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to start tk5 timer");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, 
            "Start tk5 timer");
    }
}
void p_key_sm_ack_state_confirm_key_request  ( NGPON_AES_SM * xi_key_sm , NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index ) 
{

    PON_ERROR_DTE ngpon_error ;
    uint8_t      i ;
    bdmf_boolean first_key_valid ,second_key_valid  ;
    NGPON_ENCRYP_DATA_KEY_NUM old_key_number, new_key_number ;
    bdmf_boolean tx_key_index ;
    ngpon_rx_enc_key_valid  key_valid ;   
   
    if ( g_xgpon_db.link_params.key_sm.new_key_index != xi_key_index )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "received event for key index %d while handling key index %d", 
            xi_key_index, g_xgpon_db.link_params.key_sm.new_key_index);
        return;
    }
    /*stop TK5 timer*/
    bdmf_timer_stop ( &g_xgpon_db.os_resources.tk5_timer_id ) ;
    p_log ( ge_onu_logger.sections.stack.debug.timers_id, "tk5 timer stopped" );

    /*stop TK4 timer*/
    bdmf_timer_stop ( &g_xgpon_db.os_resources.tk4_timer_id ) ;
    p_log ( ge_onu_logger.sections.stack.debug.timers_id, "tk4 timer stopped" );

    /*  Write new key to TX and set as valid  */
    if (xi_key_index == KEY_INDEX_FIRST_KEY) 
    {
        new_key_number = NGPON_ENCRYP_DATA_KEY_NUM_KEY_1_UNICAST_ONU_1 ;
        old_key_number = NGPON_ENCRYP_DATA_KEY_NUM_KEY_2_UNICAST_ONU_1 ;
        first_key_valid = BDMF_TRUE ;
        second_key_valid = BDMF_FALSE ;
        tx_key_index = 0 ;

        for ( i=0 ; i < 4 ; i ++ )
        {
           ngpon_error = ag_drv_ngpon_tx_enc_pyld_enc_key1_onu1_set ( i, htonl(xi_key_sm->new_key[i]) ) ;
           if ( ngpon_error != PON_NO_ERROR )
           {
             p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write word %d of the key to tx: error %d !", i , ngpon_error ) ;
           }
        }


    }
    else
    {
        new_key_number = NGPON_ENCRYP_DATA_KEY_NUM_KEY_2_UNICAST_ONU_1 ;
        old_key_number = NGPON_ENCRYP_DATA_KEY_NUM_KEY_1_UNICAST_ONU_1 ;
        first_key_valid = BDMF_FALSE  ;
        second_key_valid = BDMF_TRUE ;
        tx_key_index = 1 ;

        for ( i=0 ; i < 4 ; i ++ )
        {
           ngpon_error = ag_drv_ngpon_tx_enc_pyld_enc_key2_onu1_set ( i, htonl(xi_key_sm->new_key[i]) ) ;
           if ( ngpon_error != PON_NO_ERROR )
           {
             p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write word %d of the key to tx: error %d !", i , ngpon_error ) ;
           }
        }

    }
    ngpon_error = ag_drv_ngpon_tx_enc_key_cfg_onu1_set ( first_key_valid,  second_key_valid, tx_key_index ) ;
    if ( ngpon_error != PON_NO_ERROR )
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set key invalid to tx: error %d !", ngpon_error ) ;
    }


    ngpon_error = ag_drv_ngpon_rx_enc_key_valid_get ( &key_valid ) ;
    if ( ngpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to get key valid (rx) : error %d !", ngpon_error ) ;
    }
    key_valid.key1_onu1 = first_key_valid;
    key_valid.key2_onu1 = second_key_valid;
    ngpon_error = ag_drv_ngpon_rx_enc_key_valid_set ( &key_valid ) ;
    if ( ngpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to set key valid to rx : error %d !", ngpon_error ) ;
    }


     /* Send key report ploam (confirm) */
     ngpon_error = write_key_report_ploam_to_hw (g_xgpon_db.onu_params.onu_id,
                                                 xi_key_sm->aes_message_ptr.sequence_number,
                                                 xi_key_sm->key_name,
                                                 xi_key_index ,KEY_CONTROL_CONFIRM_EXISTING_KEY);
     if ( ngpon_error != PON_NO_ERROR )
     {
         p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write key report ploam: error %d !", ngpon_error ) ;
     }
     else
     {
         p_log(ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id,
             "Sending Key Report with Key Name:");
         dump_key (  (uint8_t *)xi_key_sm->key_name , CE_XGPON_DEFAULT_AES_KEY_BYTES_SIZE );
     }


     /* set new key as active */
     g_xgpon_db.link_params.key_sm.active_key_index = xi_key_index ;
     /* set handled key index to none */
     g_xgpon_db.link_params.key_sm.new_key_index = 0 ;
     /* set key state machine */
     g_xgpon_db.link_params.key_sm.key_state = CS_KEY_STATE_ACTIVE_KN4 ;


}


void p_key_reset_configuration  ( void ) 
{

    PON_ERROR_DTE ngpon_error ;
    ngpon_rx_enc_key_valid  key_valid; 



    /*set all keys as invalid */
    key_valid.key1_onu1 = BDMF_FALSE;
    key_valid.key2_onu1 = BDMF_FALSE;
    key_valid.key1_onu2 = BDMF_FALSE;
    key_valid.key2_onu2 = BDMF_FALSE;
    key_valid.key1_mcst = BDMF_FALSE;
    key_valid.key2_mcst = BDMF_FALSE;


    ngpon_error = ag_drv_ngpon_rx_enc_key_valid_set( &key_valid ) ;
    
    if ( ngpon_error != PON_NO_ERROR )
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set key invalid to rx: error %d !", ngpon_error ) ;
    }

    ngpon_error = ag_drv_ngpon_tx_enc_key_cfg_onu1_set( BDMF_FALSE, BDMF_FALSE, 0 );
    if ( ngpon_error != PON_NO_ERROR )
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set key invalid to tx: error %d !", ngpon_error ) ;
    }


    /* set new key index to none */
    g_xgpon_db.link_params.key_sm.active_key_index = 0 ;
    /* set handled key index to none */
    g_xgpon_db.link_params.key_sm.new_key_index = 0 ;
      /*   move to KN0     */
    g_xgpon_db.link_params.key_sm.key_state = CS_KEY_STATE_INIT_KN0 ;

  
}

void p_key_sm_ack_state_tk4_expire_event  ( NGPON_AES_SM * xi_key_sm , NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index ) 
{

    /* check if this happens when there is already an active key in the system*/
    if( g_xgpon_db.link_params.key_sm.active_key_index  != 0)
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "there is already an active key %d TK4 should not be set " ,g_xgpon_db.link_params.key_sm.active_key_index ) ;
        return;
    }

    /*stop TK5 timer*/
    bdmf_timer_stop ( &g_xgpon_db.os_resources.tk5_timer_id ) ;
    p_log ( ge_onu_logger.sections.stack.debug.timers_id, "Timer tk5 stopped" );

    p_key_reset_configuration  ( ) ;
  
}

void p_key_sm_ack_state_tk5_expire_event  ( NGPON_AES_SM * xi_key_sm , NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index) 
{
    bdmf_error_t  timer_error ;
    PON_ERROR_DTE  ngpon_error ;

    /* Send key report ploam (new) */
    
    if ( g_xgpon_db.link_params.key_sm.new_key_index != xi_key_index )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "received event for key index %d while handling key index %d", 
            xi_key_index, g_xgpon_db.link_params.key_sm.new_key_index);
         return;
    }
    ngpon_error = write_key_report_ploam_to_hw (g_xgpon_db.onu_params.onu_id , xi_key_sm->aes_message_ptr.sequence_number, xi_key_sm->kek_encrypted_key ,xi_key_index ,KEY_CONTROL_GENERATE_NEW_KEY );
    if ( ngpon_error != PON_NO_ERROR )
    {
     p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write key report ploam: error %d !", ngpon_error ) ;
    }
    
    /*Start TK5 timer*/
    g_xgpon_db.os_resources.tk5_key_index = xi_key_index ;
    timer_error = bdmf_timer_start ( &g_xgpon_db.os_resources.tk5_timer_id ,
        NGPON_MILISEC_TO_TICKS(g_xgpon_db.onu_params.tk5_timer_timeout));
    if ( timer_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to start tk5 timer");
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, 
            "Start tk5 timer");
    }
}


void p_key_sm_active_state_new_key_request_event  ( NGPON_AES_SM * xi_key_sm , NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index ) 
{
  PON_ERROR_DTE  ngpon_error ;

    p_log(ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id, 
        "key_sm: Active state - new key request event (key index: %d)", xi_key_index);

    if (g_xgpon_db.link_params.key_sm.active_key_index == xi_key_index ) 
    {

         p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "request for key exchnage on the active key %d" ,xi_key_index);

         ngpon_error = write_key_report_ploam_to_hw (g_xgpon_db.onu_params.onu_id ,xi_key_sm->aes_message_ptr.sequence_number, xi_key_sm->kek_encrypted_key ,xi_key_index ,KEY_CONTROL_GENERATE_NEW_KEY );
         if ( ngpon_error != PON_NO_ERROR )
         {
              p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write key report ploam: error %d !", ngpon_error ) ;
         }
         else
         {
             p_log(ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id,
                   "Sending Key Report with Key Name:");
             dump_key (  (uint8_t *)xi_key_sm->key_name , CE_XGPON_DEFAULT_AES_KEY_BYTES_SIZE );
         }

         return;
    }

    p_key_start_key_exchange (xi_key_sm ,xi_key_index);
}

void p_key_sm_active_state_confirm_key_request  ( NGPON_AES_SM * xi_key_sm ,NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index ) 
{
  PON_ERROR_DTE  ngpon_error ;

    if ( g_xgpon_db.link_params.key_sm.active_key_index != xi_key_index) 
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "report is requested for key index %d but active key is %d" ,xi_key_index , g_xgpon_db.link_params.key_sm.active_key_index);
        return;

    }
    /* Send key report ploam (confirm) */
    ngpon_error = write_key_report_ploam_to_hw (g_xgpon_db.onu_params.onu_id , xi_key_sm->aes_message_ptr.sequence_number, xi_key_sm->key_name ,xi_key_index ,KEY_CONTROL_CONFIRM_EXISTING_KEY );
    if ( ngpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write key report ploam: error %d !", ngpon_error ) ;
    }
}

void p_operation_sm ( ACTIVATION_EVENTS     xi_event,
                      OPERATION_SM_PARAMS * xi_operation_sm_params )
{
    LINK_OPERATION_STATES_DTE previous_state ;


    if ( g_xgpon_db.debug_params.disable_onu_sm )
        return;

    /* Save previous state */
    previous_state = g_xgpon_db.link_params.operation_sm.activation_state ;

    /* In order to avoid the continues logs when ds_sync_check expires */
    if (( OPERATION_EVENT_TIMER_DS_SYNC_CHECK_EXPIRE != xi_event ) &&
        ( OPERATION_EVENT_BURST_PROFILE_PLOAM != xi_event))
    {
        p_log ( ge_onu_logger.sections.stack.operating.state_change_id,
               "Invoke SM: state=%s, event=%s" , g_activation_state_names [ previous_state ], g_activation_event_names [ xi_event ] ) ;
    }

    /* Invoke the state machine */
    g_operation_sm [ previous_state ] [ xi_event ] ( & g_xgpon_db.link_params.operation_sm,  xi_operation_sm_params ) ;
}

void p_key_sm ( KEY_EVENTS   xi_event , NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index )
{
    LINK_OPERATION_STATES_DTE onu_state ;
    LINK_KEY_STATES_DTE key_state ;

    /* Save the ONU state */
   onu_state = g_xgpon_db.link_params.operation_sm.activation_state ;
   key_state = g_xgpon_db.link_params.key_sm.key_state;

    /* run state machine only if the ONU is active  */
    if  ( OPERATION_STATE_O5 == onu_state  ) 
    {
        p_log ( ge_onu_logger.sections.stack.key.invoke_state_machine_id,
               "Invoke Key SM: state=%s, event=%s" , g_key_state_names [ key_state ], g_key_event_names [ xi_event ] ) ;

        /* Invoke the state machine */
        g_key_sm [ key_state ] [ xi_event ] ( & g_xgpon_db.link_params.key_sm , xi_key_index) ;

        p_log ( ge_onu_logger.sections.stack.key.state_change_id,
               "Key SM New State=%s" , g_key_state_names [ g_xgpon_db.link_params.key_sm.key_state ] ) ;
    }
    else
    {

        p_log ( ge_onu_logger.sections.stack.key.invoke_state_machine_id,
               "Invoke SM:but ONU is inactive state=%s, event=%s" , g_key_state_names [ key_state ], g_activation_event_names [ xi_event ] ) ;

    }

}


void ngpon_set_pmd_fb_done(uint8_t state)
{
    printk("\n**** ngpon_set_pmd_fb_done to %d****\n", state);

    if (state == 1)
    {
        pmdFirstBurstDone = true;
    }
    else
    {
        pmdFirstBurstDone = false;
    }
}

int ngpon_stack_init(void)
{
#ifdef USE_LOGGER
    LOGGER_ERROR rc;
    int          count ;
    char         cpi_buff[16];
    char         no_hunt_buff[16];
    int          cpi ;

    rc = logger_init();
    if (rc) 
        return rc;
#endif
#ifdef _BYTE_ORDER_LITTLE_ENDIAN_       /* To prevent compilation for MIPS-based targets */
    remap_ru_block_addrs(NGPON2_IDX, RU_NGPON_BLOCKS);
#endif
    ngpon_software_init();

    BpGetGponOpticsType(&g_xgpon_db.optics_type);
    /*
     *    Get Channel Partition Indicator value from scratchpad
     */
    g_xgpon_db.link_params.ch_profile.current_cpi = 0;
    count = kerSysScratchPadGet(NGPON2_CPI_PSP_KEY, cpi_buff, NGPON2_CPI_BUFLEN_2);
    if (count > 0)
    {
        sscanf(cpi_buff,"%d", &cpi) ;
        if ((cpi >= 0) && (cpi < 16))
        {
            g_xgpon_db.link_params.ch_profile.current_cpi = cpi ;
        }
    }

    count = kerSysScratchPadGet(NGPON2_NO_HUNT_PSP_KEY, no_hunt_buff, NGPON2_NO_HUNT_BUFLEN_2);
    if (count > 0)
    {
        if ((no_hunt_buff[0] == 'y') || (no_hunt_buff[0] == 'Y'))
        {
            g_xgpon_db.link_params.init_tuning_done = BDMF_TRUE ;
            ngpon2_no_hunt_mode = BDMF_TRUE ;
        }
    }


    return 0;
}

void ngpon_stack_exit(void)
{
#ifdef USE_LOGGER
    logger_exit();
#endif

    /* Exit SW */
    //TODO
}
/*
 * Function to be used by the driver's shell
 */
PON_ERROR_DTE ngpon_set_dying_gasp_state (uint8_t dying_gasp_state)
{
    ag_drv_ngpon_tx_cfg_dying_gasp_set(dying_gasp_state);

    return PON_NO_ERROR;
}

/*
 * Function to be used by RDPA
 */
static PON_ERROR_DTE ngpon_set_dying_gasp(void)
{
  return ngpon_set_dying_gasp_state (1);
}


PON_ERROR_DTE ngpon_send_pee_message(void)
{
    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_send_pst_message(PON_PST_INDICATION_PARAMETERS_DTE xi_pst_configuration_struct)
{
    return PON_NO_ERROR;
}
#ifndef SIM_ENV

void ngpon_register_cb(PON_SM_CALLBACK_FUNC * p_pon_sm_cb)
{
    p_pon_sm_cb->pon_assign_user_callback = ngpon_assign_user_callback;
    p_pon_sm_cb->pon_configure_onu_serial_number = ngpon_configure_onu_serial_number;
    p_pon_sm_cb->pon_configure_onu_password = ngpon_configure_onu_password;
    p_pon_sm_cb->pon_link_activate = _ngpon_link_activate;
    p_pon_sm_cb->pon_set_gem_block_size = ngpon_set_gem_block_size;
    p_pon_sm_cb->pon_dba_sr_process_initialize = ngpon_dba_sr_process_initialize;
    p_pon_sm_cb->pon_link_deactivate = _ngpon_link_deactivate;
    p_pon_sm_cb->pon_configure_link_params = ngpon_configure_link_params;
    p_pon_sm_cb->pon_send_dying_gasp_message = ngpon_set_dying_gasp;
    p_pon_sm_cb->pon_send_pee_message = ngpon_send_pee_message;
    p_pon_sm_cb->pon_send_pst_message = ngpon_send_pst_message;
    p_pon_sm_cb->pon_dba_sr_process_terminate = ngpon_dba_sr_process_terminate;
    p_pon_sm_cb->pon_control_port_id_filter = ngpon_control_port_id_filter;
    p_pon_sm_cb->pon_modify_encryption_port_id_filter = ngpon_modify_encryption_port_id_filter;
    p_pon_sm_cb->pon_configure_port_id_filter = ngpon_configure_port_id_filter;
    p_pon_sm_cb->pon_configure_tcont = ngpon_configure_tcont;
    p_pon_sm_cb->pon_remove_tcont = _ngpon_remove_tcont;
    p_pon_sm_cb->pon_link_reset = _ngpon_link_reset;
    p_pon_sm_cb->pon_mask_rx_pon_interrupts = ngpon_mask_rx_pon_interrupts;
    p_pon_sm_cb->pon_mask_tx_pon_interrupts = ngpon_mask_tx_pon_interrupts;
    p_pon_sm_cb->pon_isr_handler = ngpon_mac_isr;
    p_pon_sm_cb->pon_unmask_tx_pon_interrupts = ngpon_unmask_tx_pon_interrupts;
    p_pon_sm_cb->pon_unmask_rx_pon_interrupts = ngpon_unmask_rx_pon_interrupts;
    p_pon_sm_cb->pon_tx_set_tcont_to_counter_group_association = ngpon_tx_set_tcont_to_counter_group_association;
    p_pon_sm_cb->pon_tx_set_flush_valid_bit = ngpon_tx_set_flush_valid_bit;
    p_pon_sm_cb->pon_tx_generate_misc_transmit = ngpon_tx_generate_misc_transmit;

    p_pon_sm_cb->pon_get_link_status = ngpon_get_link_status;
    p_pon_sm_cb->pon_get_link_parameters = ngpon_get_link_parameters;
    p_pon_sm_cb->pon_get_onu_id = ngpon_get_onu_id;
    p_pon_sm_cb->pon_get_overhead_and_length = ngpon_get_overhead_and_length;
    p_pon_sm_cb->pon_get_transceiver_power_level = ngpon_get_transceiver_power_level;
    p_pon_sm_cb->pon_get_equalization_delay = ngpon_get_equalization_delay;
    p_pon_sm_cb->pon_get_aes_encryption_key = ngpon_get_aes_encryption_key;
    p_pon_sm_cb->pon_get_bip_error_counter = ngpon_get_bip_error_counter;
    p_pon_sm_cb->pon_get_rxpon_ploam_counters = ngpon_get_rxpon_ploam_counters;
    p_pon_sm_cb->pon_get_rx_unkonw_ploam_counter = ngpon_get_rx_unkonw_ploam_counter;
    p_pon_sm_cb->pon_get_txpon_pm_counters = ngpon_get_txpon_pm_counters;
    p_pon_sm_cb->pon_get_port_id_configuration = ngpon_get_port_id_configuration;
    p_pon_sm_cb->pon_get_encryption_error_counter = ngpon_get_encryption_error_counter;
    p_pon_sm_cb->pon_get_tcont_configuration = ngpon_get_tcont_configuration;
    p_pon_sm_cb->pon_get_tcont_queue_pm_counters = ngpon_get_tcont_queue_pm_counters;
    p_pon_sm_cb->pon_get_ploam_crc_error_counter = ngpon_get_ploam_crc_error_counter;
    p_pon_sm_cb->pon_rx_get_receiver_status = ngpon_rx_get_receiver_status;
    p_pon_sm_cb->pon_rx_get_fec_counters = ngpon_rx_get_fec_counters;
    p_pon_sm_cb->pon_rx_get_hec_counters = ngpon_rx_get_hec_counters;    
    p_pon_sm_cb->pon_tx_get_flush_valid_bit = ngpon_tx_get_flush_valid_bit;
    p_pon_sm_cb->pon_get_pon_id_parameters = ngpon_get_pon_id_parameters;
    p_pon_sm_cb->rogue_onu_detect_cfg = rogue_onu_detect_cfg;
    p_pon_sm_cb->set_multicast_encryption_key = ngpon_set_multicast_encryption_key;
    p_pon_sm_cb->set_stack_mode = ngpon_set_stack_mode ;
    p_pon_sm_cb->pon_start_bw_recording = ngpon_start_bw_recording;
    p_pon_sm_cb->pon_stop_bw_recording = ngpon_stop_bw_recording;
    p_pon_sm_cb->pon_get_bw_recording_data = ngpon_read_bw_record;
    p_pon_sm_cb->pon_get_burst_prof = ngpon_get_burst_prof;
    p_pon_sm_cb->calc_ds_omci_crc_or_mic = ngpon_calc_ds_omci_mic;
    p_pon_sm_cb->calc_us_omci_crc_or_mic = ngpon_calc_us_omci_mic;

#ifdef USE_BDMF_SHELL
    p_pon_sm_cb->pon_modify_tcont = ngpon_modify_tcont;
    p_pon_sm_cb->initialize_drv_pon_stack_shell = ngpon_initialize_drv_pon_stack_shell;
    p_pon_sm_cb->exit_drv_pon_stack_shell = ngpon_exit_drv_pon_stack_shell;
#endif

#ifdef CONFIG_BCM_GPON_TODD
    p_pon_sm_cb->pon_todd_set_tod_info = pon_tod_set_tod_info;
    p_pon_sm_cb->pon_todd_get_tod_info = pon_tod_get_tod_info;
    p_pon_sm_cb->pon_todd_reg_1pps_start_cb = pon_tod_reg_1pps_start_cb;
    p_pon_sm_cb->pon_todd_get_tod = pon_tod_get_tod;
    p_pon_sm_cb->pon_tod_get_sfc = pon_tod_get_sfc;
#endif
}
#endif

int init_pon_module(PON_SM_CALLBACK_FUNC * p_pon_sm_cb)
{
    ngpon_register_cb(p_pon_sm_cb);

    return 0;
}

EXPORT_SYMBOL(init_pon_module);

int __init ngponStackModuleCreate(void)
{
    int  ret = 0;

    ret = ngpon_stack_init();

    return ret;
}

void __exit ngponStackModuleDelete(void)
{
    ngpon_stack_exit();
    return;
}


MODULE_DESCRIPTION("Broadcom NGPON Stack");
MODULE_LICENSE("Proprietary");
module_init(ngponStackModuleCreate);
module_exit(ngponStackModuleDelete);




