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

#ifndef NGPON_SM_H_INCLUDED
#define NGPON_SM_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif

#include "ngpon_ploam.h"
#include "pon_sm_common.h"


/******************************************************************************/
/*                                                                            */
/* ONU SM events                                                              */
/*                                                                            */
/******************************************************************************/

/* Define activation state machine events */
typedef enum
{
    /* 00 */ OPERATION_EVENT_TIMER_DS_SYNC_CHECK_EXPIRE = 0,
    /* 01 */ OPERATION_EVENT_DS_FRAME_SYNC,
    /* 02 */ OPERATION_EVENT_BURST_PROFILE_PLOAM,
    /* 03 */ OPERATION_EVENT_ASSIGN_ONU_ID_PLOAM,                                      
    /* 04 */ OPERATION_EVENT_RANGING_TIME_PLOAM,                                       
    /* 05 */ OPERATION_EVENT_TIMER_TO1_EXPIRE,                                         
    /* 06 */ OPERATION_EVENT_DEACTIVATE_ONU_ID_PLOAM,                                  
    /* 07 */ OPERATION_EVENT_DISABLE_SERIAL_NUMBER_PLOAM,                              
    /* 08 */ OPERATION_EVENT_DETECT_LODS,                                              
    /* 09 */ OPERATION_EVENT_TIMER_TO2_EXPIRE,                                         
    /* 10 */ OPERATION_EVENT_REQUEST_REGISTRATION_PLOAM,                               
    /* 11 */ OPERATION_EVENT_ASSIGN_ALLOC_ID_PLOAM,                                    
    /* 12 */ OPERATION_EVENT_KEY_CONTROL_PLOAM,                                        
    /* 13 */ OPERATION_EVENT_SLEEP_ALLOW_PLOAM,                                        
    /* 14 */ OPERATION_EVENT_CHANNEL_PROFILE_PLOAM,    
    /* 15 */ OPERATION_EVENT_PROTECTION_CONTROL_PLOAM,                                 
    /* 16 */ OPERATION_EVENT_SYSTEM_PROFILE_PLOAM,                                     
    /* 17 */ OPERATION_EVENT_CALIBRATION_REQUEST_PLOAM,                                
    /* 18 */ OPERATION_EVENT_ADJUST_TX_WAVELENGTH_PLOAM,                               
    /* 19 */ OPERATION_EVENT_TUNING_CONTROL_PLOAM,                                     
    /* 20 */ OPERATION_EVENT_TIMER_TOZ_EXPIRE,                                         
    /* 21 */ OPERATION_EVENT_TIMER_TO3_EXPIRE,                                         
    /* 22 */ OPERATION_EVENT_TIMER_TO4_EXPIRE,                                         
    /* 23 */ OPERATION_EVENT_TIMER_TO5_EXPIRE,  
    /* 24 */ OPERATION_EVENT_POWER_CONSUMPTION_INQUIRE,                                      
    /* 25 */ OPERATION_EVENT_SFC_MATCH,
    /* 26 */ OPERATION_NGPON2_EVENTS_NUMBER
}
ACTIVATION_EVENTS;



/******************************************************************************/
/*                                                                            */
/* ONU key exchange SM events                                                 */
/*                                                                            */
/******************************************************************************/

/* Define key exchange state machine events */
typedef enum
{
    /* 00 */ KEY_EVENT_NEW_KEY_REQUEST = 0,
    /* 01 */ KEY_EVENT_CONFIRM_KEY_REQUEST,
    /* 02 */ KEY_EVENT_TK4_EXPIRED,
    /* 03 */ KEY_EVENT_TK5_EXPIRED,
    /* 04 */ KEY_EVENTS_NUMBER,        
}
KEY_EVENTS ;

/******************************************************************************/
/* This type describes the sub-state of the active link                       */
/******************************************************************************/
#ifndef SIM_ENV
typedef uint32_t LINK_SUB_STATE_DTE ;
#endif

/* None */
#define CE_LINK_SUB_STATE_NONE                     ( ( LINK_SUB_STATE_DTE ) 0 )
/* Standby */
#define CE_LINK_SUB_STATE_STANDBY                  ( ( LINK_SUB_STATE_DTE ) 1 )
/* Operation */
#define CE_LINK_SUB_STATE_OPERATIONAL              ( ( LINK_SUB_STATE_DTE ) 2 )
/* Stop */
#define CE_LINK_SUB_STATE_STOP                     ( ( LINK_SUB_STATE_DTE ) 3 )


/******************************************************************************/
/* This type describes the max value for the random delay in us               */
/******************************************************************************/
#define  CE_RANDOM_DELAY_MAX ( 48 )  

/******************************************************************************/
/*  Encryption key  .                                                           */
/******************************************************************************/
#ifndef NGPON_ENCRYP_DATA_KEY_INDEX_DEFINED
#define NGPON_ENCRYP_DATA_KEY_INDEX_DEFINED

typedef uint32_t NGPON_ENCRYP_DATA_KEY_INDEX ;

#define NGPON_ENCRYP_DATA_KEY_INDEX_1       ( ( NGPON_ENCRYP_DATA_KEY_INDEX ) XGPON_TX_ENC_KEY_CFG_ONU1_KEY_SEL_KEY_1_VALUE )
#define NGPON_ENCRYP_DATA_KEY_INDEX_2       ( ( NGPON_ENCRYP_DATA_KEY_INDEX ) XGPON_TX_ENC_KEY_CFG_ONU1_KEY_SEL_KEY_2_VALUE )

#endif /* NGPON_ENCRYP_DATA_KEY_INDEX_DEFINED */

/******************************************************************************/
/* This type describes the key exchnage states                                */
/******************************************************************************/
/* Define activation state machine states */
typedef uint32_t LINK_KEY_STATES_DTE ;

#define CS_KEY_STATE_INIT_KN0                      ( ( LINK_KEY_STATES_DTE ) 0 )
#define CS_KEY_STATE_WAIT_ACK_KN2                  ( ( LINK_KEY_STATES_DTE ) 1 )
#define CS_KEY_STATE_ACTIVE_KN4                    ( ( LINK_KEY_STATES_DTE ) 2 )
#define CS_KEY_STATES_NUMBER                       ( ( LINK_KEY_STATES_DTE ) CS_KEY_STATE_ACTIVE_KN4 +1 )       



/******************************************************************************/
/* XGPON/NGPON US line rate                                                          */
/******************************************************************************/
#ifndef NGPON_US_LINE_RATE_DEFINED
#define NGPON_US_LINE_RATE_DEFINED

typedef uint32_t NGPON_US_LINE_RATE ;

#define NGPON_US_LINE_RATE_2_5G  ( ( NGPON_US_LINE_RATE ) 0 )
#define NGPON_US_LINE_RATE_5G    ( ( NGPON_US_LINE_RATE ) 1 )
#define NGPON_US_LINE_RATE_10G   ( ( NGPON_US_LINE_RATE ) 2 )

#endif/* NGPON_US_LINE_RATE_DEFINED */

/******************************************************************************/
/* ONU index.                                                                 */
/******************************************************************************/
#ifndef NGPON_ONU_INDEX_DEFINED
#define NGPON_ONU_INDEX_DEFINED

typedef uint32_t NGPON_ONU_INDEX ;

/* ONU Index of Uunicast ONU A */
#define NGPON_ONU_INDEX_ONU_0           ( ( NGPON_ONU_INDEX ) 0 )
/* ONU Index of Uunicast ONU B */
#define NGPON_ONU_INDEX_ONU_1           ( ( NGPON_ONU_INDEX ) 1 )
/* ONU Index of Broadcast ONU C */
#define NGPON_ONU_INDEX_ONU_BROADCAST   ( ( NGPON_ONU_INDEX ) 2 )
#define NGPON_ONU_INDEX_ONU_2           ( NGPON_ONU_INDEX_ONU_BROADCAST ) 

#define NGPON_ONU_INDEX_UNICAST_ONU_ID_LOW          ( NGPON_ONU_INDEX_ONU_0 )
#define NGPON_ONU_INDEX_UNICAST_ONU_ID_HIGH         ( NGPON_ONU_INDEX_ONU_1 )
#define NGPON_ONU_INDEX_UNICAST_ONU_ID_IN_RANGE(v) ( (v) >= NGPON_ONU_INDEX_UNICAST_ONU_ID_LOW && (v) <= NGPON_ONU_INDEX_UNICAST_ONU_ID_HIGH )

#define NGPON_ONU_INDEX_ONU_ID_LOW          ( NGPON_ONU_INDEX_ONU_0 )
#define NGPON_ONU_INDEX_ONU_ID_HIGH         ( NGPON_ONU_INDEX_ONU_BROADCAST )
#define NGPON_ONU_INDEX_ONU_ID_IN_RANGE(v) ( (v) >= NGPON_ONU_INDEX_ONU_ID_LOW && (v) <= NGPON_ONU_INDEX_ONU_ID_HIGH )

#endif

/******************************************************************************/
/* This type describes the possible AES states                                */
/******************************************************************************/
typedef uint32_t AES_STATE_DTE ;

/* Not ready */
#define CE_AES_STATE_NOT_READY       ( ( AES_STATE_DTE ) 0 )
/* Standby */
#define CE_AES_STATE_STANDBY         ( ( AES_STATE_DTE ) 1 )
/* Scheduling */
#define CE_AES_STATE_SCHEDULING      ( ( AES_STATE_DTE ) 2 )

#define CE_XGPON_DEFAULT_AES_KEY_WORDS_SIZE            ( 4 )
#define CE_XGPON_DEFAULT_AES_KEY_BYTES_SIZE            ( CE_XGPON_DEFAULT_AES_KEY_WORDS_SIZE * BYTES_PER_WORD )

/******************************************************************************/
/* This type defines BURST Profile queue                                         */
/******************************************************************************/
#ifndef NGPON_BURST_PROFILE_INDEX_ID_DEFINED
#define NGPON_BURST_PROFILE_INDEX_ID_DEFINED

typedef uint32_t NGPON_BURST_PROFILE_INDEX ;

#define NGPON_BURST_PROFILE_INDEX_0                ( ( NGPON_BURST_PROFILE_INDEX ) 0 )
#define NGPON_BURST_PROFILE_INDEX_1                ( ( NGPON_BURST_PROFILE_INDEX ) 1 )
#define NGPON_BURST_PROFILE_INDEX_2                ( ( NGPON_BURST_PROFILE_INDEX ) 2 )
#define NGPON_BURST_PROFILE_INDEX_3                ( ( NGPON_BURST_PROFILE_INDEX ) 3 )

#define NGPON_BURST_PROFILE_INDEX_LOW               ( ( NGPON_BURST_PROFILE_INDEX ) NGPON_BURST_PROFILE_INDEX_0 )
#define NGPON_BURST_PROFILE_INDEX_HIGH              ( ( NGPON_BURST_PROFILE_INDEX ) NGPON_BURST_PROFILE_INDEX_3 )

#define NGPON_BURST_PROFILE_INDEX_IN_RANGE(v)       ( (v) >= NGPON_BURST_PROFILE_INDEX_LOW && (v) <= NGPON_BURST_PROFILE_INDEX_HIGH )

#endif /* NGPON_BURST_PROFILE_INDEX_ID_DEFINED */

/* Define the Operation state machine struct */
typedef struct
{
    /* In case LODS event */
    bdmf_boolean lods_state;

    /* incoming received Ploam message */
    NGPON_DS_PLOAM *ploam_message_ptr;
}
OPERATION_SM_PARAMS;

/* Activation state machine control */
typedef struct
{
    /* Link state */
    LINK_STATE_DTE link_state ;

    /* Link sub-state */
    LINK_SUB_STATE_DTE link_sub_state ;

    LINK_OPERATION_STATES_DTE activation_state ;
}
NGPON_OPERATION_SM;

/* AES state machine control */
typedef struct
{
    AES_STATE_DTE key_state ;
    NGPON_ENCRYP_DATA_KEY_INDEX new_key_index ;
    NGPON_ENCRYP_DATA_KEY_INDEX active_key_index ;
    AES_KEY_DTE new_key [ CE_XGPON_DEFAULT_AES_KEY_WORDS_SIZE ] ;
    AES_KEY_DTE kek_encrypted_key  [ CE_XGPON_DEFAULT_AES_KEY_WORDS_SIZE ] ;
    AES_KEY_DTE key_name  [ CE_XGPON_DEFAULT_AES_KEY_WORDS_SIZE ] ;
    NGPON_DS_PLOAM  aes_message_ptr ;
}
NGPON_AES_SM ;

typedef struct
{
    NGPON_DS_PLOAM *p_ploam_message;
}sub_sm_params;

typedef enum
{
    o1_1_offsync_sub_state,
    o1_2_profile_learning_sub_state,
} o1_sub_states;

typedef enum
{
    o1_ds_frame_sync_event,
    o1_system_profile_ploam_event,
    o1_channel_profile_ploam_event,
    burst_profile_ploam_event,
    o1_detect_lods_event,
    o1_disable_serial_number_ploam_event,
    o1_sub_sm_event_num
} o1_sub_sm_events;

typedef enum
{
    o8_1_offsync_sub_state,
    o8_2_profile_learning_sub_state,
} o8_sub_states;

typedef enum
{
    o8_ds_frame_sync_event,
    timer_to4_expire_event,
    o8_detect_lods_event,
    o8_channel_profile_ploam_event,
    protection_control_ploam_event,
    assigned_onu_id_ploam_event,
    ranging_time_ploam_event,
    deactivate_onu_id_ploam_event,
    o8_disable_serial_number_ploam_event,
    o8_system_profile_ploam_event,
    o8_burst_profile_ploam_event,
    o8_sub_sm_event_num
} o8_sub_sm_events;

typedef enum
{
    o5_1_associated_sub_state,
    o5_2_pending_sub_state,
} o5_sub_states;

typedef enum
{
    tuning_control_ploam_event,
    sfc_match_event,
    o5_sub_sm_event_num
} o5_sub_sm_events;

/******************************************************************************/
/* PLOAM Encryption key onu id type .                                      */
/******************************************************************************/
#ifndef NGPON_ENCRYP_PLOAM_KEY_TYPE_DEFINED
#define NGPON_ENCRYP_PLOAM_KEY_TYPE_DEFINED

typedef uint32_t NGPON_ENCRYP_PLOAM_KEY_TYPE ;

#define NGPON_ENCRYP_PLOAM_KEY_TYPE_ONU_1       ( ( NGPON_ENCRYP_PLOAM_KEY_TYPE ) 0 )
#define NGPON_ENCRYP_PLOAM_KEY_TYPE_ONU_2       ( ( NGPON_ENCRYP_PLOAM_KEY_TYPE ) 1 )
#define NGPON_ENCRYP_PLOAM_KEY_TYPE_DEFAULT     ( ( NGPON_ENCRYP_PLOAM_KEY_TYPE ) 2 )

#define NGPON_ENCRYP_PLOAM_KEY_TYPE_LOW         ( NGPON_ENCRYP_PLOAM_KEY_TYPE_ONU_1 )
#define NGPON_ENCRYP_PLOAM_KEY_TYPE_HIGH        ( NGPON_ENCRYP_PLOAM_KEY_TYPE_DEFAULT )

#define NGPON_ENCRYP_PLOAM_KEY_TYPE_IN_RANGE(v)    ( (v) >= NGPON_ENCRYP_PLOAM_KEY_TYPE_LOW && (v) <= NGPON_ENCRYP_PLOAM_KEY_TYPE_DEFAULT )
#endif


void p_operation_sm ( ACTIVATION_EVENTS     xi_event,
                      OPERATION_SM_PARAMS * xi_operation_sm_params ) ;

void p_key_sm ( KEY_EVENTS   xi_event , NGPON_ENCRYP_DATA_KEY_INDEX xi_key_index ) ;


char * p_get_operation_state_name (LINK_KEY_STATES_DTE  operation_state) ;

void p_pon_to1_timer_callback ( uint32_t xi_parameter );
void p_pon_to2_timer_callback ( uint32_t xi_parameter );
void p_pon_tk4_timer_callback ( uint32_t xi_parameter );
void p_pon_tk5_timer_callback ( uint32_t xi_parameter );
void p_pon_ds_sync_check_timer_callback ( uint32_t xi_parameter );
void toz_timer_callback(uint32_t params);
void to3_timer_callback(uint32_t params);
void to4_timer_callback(uint32_t params);
void to5_timer_callback(uint32_t params);
void tcpi_timer_callback(uint32_t params);
void save_channel_profile_to_db(NGPON_DS_CHANNEL_PROFILE_PLOAM *profile);
void save_system_profile_to_db(NGPON_DS_SYSTEM_PROFILE_PLOAM *profile);

void fetch_us_ploam(void);
void dump_key (uint8_t *key, uint32_t key_size_in_bytes);

void set_zero_eqd_after_tuning (bdmf_boolean val) ;
bdmf_boolean get_zero_eqd_after_tuning (void) ;

void set_simplified_rollback (bdmf_boolean val) ;
bdmf_boolean get_simplified_rollback (void) ;

PON_ERROR_DTE ngpon_set_dying_gasp_state (uint8_t dying_gasp_state);

#ifdef __cplusplus
}
#endif


#endif /* NGPON_SM_H_INCLUDED */
