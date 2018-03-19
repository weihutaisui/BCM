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

#ifndef BL_LOGGER_H_INCLUDED
#define BL_LOGGER_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/*                                                                            */
/* SW unit description:                                                       */
/*                                                                            */
/* This software unit implements the Lilac logger.                            */
/*                                                                            */
/******************************************************************************/

#ifdef INCLUDE_LOGS
#ifdef USE_LOGGER

#include "logger.h"
#include "pon_sm_common.h"

/* Use logger */
#ifndef SIM_ENV
#define p_log(log, message, parameters...) pi_logger_log(__FILE__, __LINE__, __FUNCTION__, log, message, ##parameters)
#else  
#define p_log(log, message, parameters...)  do {fprintf(stderr, "\n%s %d %s :", __FILE__, __LINE__, __FUNCTION__); \
    fprintf(stderr, message, ##parameters);} while (0);
#endif 

#else 
/* Use user-defined log routine. The user should implement pi_user_log */
static inline void pi_user_log(char * xi_file, uint32_t xi_line, char * xi_format, ...)
{
}

#define p_log(log, message, parameters...) pi_user_log(__FILE__, __LINE__, message, ##parameters)
#endif 
#else
/* Use empty log routine */
#define p_log(log, message, parameters...) 
#endif 

PON_ERROR_DTE gpon_get_log_object_list(char* parent_name, char* name_list);
PON_ERROR_DTE gpon_set_log_object_state(char* object_name, bool state);

#ifdef USE_LOGGER
void gpon_logger_init(char *xi_base_folder);
LOGGER_ERROR logger_init(void);
void logger_exit(void);

typedef struct
{
    /* Log */
    LOGGER_LOG_ID log_id;
    struct
    {
        /* GPON stack section */
        LOGGER_SECTION_ID stack_id;
        struct
        {
            /* Operating task group */
            LOGGER_GROUP_ID operating_id;
            struct
            {             
                /* Invoke state machine item */
                LOGGER_ITEM_ID invoke_state_machine_id;
                
                /* State change item */
                LOGGER_ITEM_ID state_change_id;

                /* Invalid Cross */
                LOGGER_ITEM_ID invalid_cross;

                /* General item */
                LOGGER_ITEM_ID general_id;
            }
            operating;
        
            /* Faults task group */
            LOGGER_GROUP_ID pon_task_id;
            struct
            {
                /* Inter-task messages item */
                LOGGER_ITEM_ID inter_task_messages_id;
                
                /* Unknown inter-task messages item */
                LOGGER_ITEM_ID unknown_inter_task_messages_id; 

                /* General item */
                LOGGER_ITEM_ID general_id;
                               
            }
            pon_task;
        
            /* Downstream PLOAM group */
            LOGGER_GROUP_ID downstream_ploam_id;
            struct
            {
                /* General item */                                                                           
                LOGGER_ITEM_ID general_id;                                                                     
                                                                                                                
                /* NO message item */                                                                
                LOGGER_ITEM_ID no_message_message_id;                                           
                                                                                                          
                /* US_OVERHEAD item */                                                               
                LOGGER_ITEM_ID upstream_overhead_message_id;                                     
                                                                                                                
                /* SERIAL_NUMBER_MASK item */                                                        
                LOGGER_ITEM_ID serial_number_mask_message_id;                                   
                                                                                                      
                /* ASSIGN_ONU_ID item */                                                               
                LOGGER_ITEM_ID assign_onu_id_message_id;                                                        
                                                                                                
                /* RANGING_TIME item */                                                             
                LOGGER_ITEM_ID ranging_time_message_id;                                                      
                                                                                                                       
                /* DEACTIVATE_ONU_ID item */                                                            
                LOGGER_ITEM_ID deactivate_onu_id_message_id;                                                    
                                                                                                     
                /* DISABLE_SERIAL_NUMBER item */                                                     
                LOGGER_ITEM_ID disable_serial_number_message_id;                                
                                                                                                     
                /* CFG_VP_VC item */                                                                 
                LOGGER_ITEM_ID configure_vp_vc_message_id;                                      
                                                                                                     
                /* ENCRYPTED_VPI/Port-ID item */                                                     
                LOGGER_ITEM_ID encrypted_vpi_port_message_id;                                   
                                                                                                     
                /* REQUEST_PASSWORD item */                                                          
                LOGGER_ITEM_ID request_password_message_id;                                     
                                                                                                     
                /* ALLOCATION item */                                                                
                LOGGER_ITEM_ID assign_alloc_id_message_id;                                      

                 /* POPUP item */
                LOGGER_ITEM_ID popup_message_id;

                /* REQUEST_KEY item */
                LOGGER_ITEM_ID request_key_message_id;

                /* CFG_PORT_ID item */
                LOGGER_ITEM_ID configure_port_id_message_id;

                /* PEE item */
                LOGGER_ITEM_ID pee_message_id;

                /* CHANGE_POWER_LEVEL item */
                LOGGER_ITEM_ID cpl_message_id;

                /* PST item */
                LOGGER_ITEM_ID pst_message_id;

                /* BER_INTERVAL item */
                LOGGER_ITEM_ID ber_interval_message_id;

                /* KEY_SWITCHING_TIME item */
                LOGGER_ITEM_ID key_switching_time_id;

                /* Extended burst item */
                LOGGER_ITEM_ID extended_burst_id;

                /* SLEEP_ALLOW item */
                LOGGER_ITEM_ID sleep_allow_message_id; 

                /* PON_ID item */
                LOGGER_ITEM_ID pon_id_message_id; 

                /* RANGING_ADJUSTMENT item */
                LOGGER_ITEM_ID ranging_adjustment_message_id; 

                /* SWIFT_POPUP item */
                LOGGER_ITEM_ID swift_popup_message_id;

                /* Key control message ID */
                LOGGER_ITEM_ID key_control_message_id;

                /* Profile (XGPON) */ 
                LOGGER_ITEM_ID burst_profile_message_id;

                /* Registration request (XGPON) */
                LOGGER_ITEM_ID request_registration_message_id;

                /* Calibration request (XGPON) */
                LOGGER_ITEM_ID calibration_request_message_id;   
                                                                     
                /* Tx wavelength (XGPON) */                         
                LOGGER_ITEM_ID adjust_tx_wavelength_message_id;        
                                                                          
                /* Tuning_control (XGPON) */                     
                LOGGER_ITEM_ID tuning_control_message_id;    

                /* System profile (XGPON) */
                LOGGER_ITEM_ID system_profile_message_id;

                /* Channel profile (XGPON) */
                LOGGER_ITEM_ID channel_profile_message_id;

                /* Protection control (XGPON) */
                LOGGER_ITEM_ID protection_control_message_id;

                /* Change power_level (XGPON) */
                LOGGER_ITEM_ID change_power_level_message_id;

                /* Change power_level (XGPON) */
                LOGGER_ITEM_ID rate_control_message_id;

                /* Change power_level (XGPON) */
                LOGGER_ITEM_ID power_consumption_inquire_message_id;

                /* UNKNOWN item */
                LOGGER_ITEM_ID unknown_message_id;

                /* Print PLOAM item */
                LOGGER_ITEM_ID print_ploam;
            }
            downstream_ploam;

            /* Upstream PLOAM group */
            LOGGER_GROUP_ID upstream_ploam_id;
            struct
            {
                /* General item */
                LOGGER_ITEM_ID general_id;                                       
                                                                            
                /* SERIAL_NUMBER_ONU item */                                 
                LOGGER_ITEM_ID serial_number_onu_message_id;               
                                                                              
                /* PASSWORD item */                                          
                LOGGER_ITEM_ID password_message_id;                    
                                                                            
                /* NO message item */                                       
                LOGGER_ITEM_ID no_message_message_id;                  
                                                                            
                /* Dying Gasp item */                                           
                LOGGER_ITEM_ID dying_gasp_message_id;                     
                                                                             
                /* Encryption key item */                                    
                LOGGER_ITEM_ID encryption_key_message_id;               

                /* PEE item */
                LOGGER_ITEM_ID pee_message_id;

                /* PST item */
                LOGGER_ITEM_ID pst_message_id;

                /* REI item */
                LOGGER_ITEM_ID rei_message_id;

                /* ACK item */
                LOGGER_ITEM_ID ack_message_id;

                /* Registration item */
                LOGGER_ITEM_ID registration_message_id;

                /* Key report item */
                LOGGER_ITEM_ID key_report_message_id;

                /* Sleep request item */
                LOGGER_ITEM_ID sleep_request_message_id;     

                /* Tuning response item */                     
                LOGGER_ITEM_ID tuning_response_message_id; 

                /* Power consumption report item */                  
                LOGGER_ITEM_ID power_consumption_report_message_id;

                /* Rate response item */                         
                LOGGER_ITEM_ID rate_response_message_id;

                /* UNKNOWN item */
                LOGGER_ITEM_ID unknown_message_id;

                /* Print PLOAM item */
                LOGGER_ITEM_ID print_ploam;
            }
            upstream_ploam;

            /* Debug messages */
            LOGGER_GROUP_ID debug_messages_id;
            struct
            {
                /* General item */
                LOGGER_ITEM_ID general_id;
                
                /* SW errors item */
                LOGGER_ITEM_ID sw_errors_id;
                
                /* Timers item */
                LOGGER_ITEM_ID timers_id;
                
            }
            debug;

          /* Key Exchange */
            LOGGER_GROUP_ID key_id;
            struct
            {             
                /* Invoke state machine item */
                LOGGER_ITEM_ID invoke_state_machine_id;
                
                /* State change item */
                LOGGER_ITEM_ID state_change_id;

                /* General item */
                LOGGER_ITEM_ID general_id;
            }
            key;


        }
        stack;
        
        /* ISR section */
        LOGGER_SECTION_ID isr_id;
        struct
        {
            /* debug group */
            LOGGER_GROUP_ID debug_id;
            struct
            {
                /* General item */
                LOGGER_ITEM_ID general_id;

                /* Message ISR-PON task */
                LOGGER_GROUP_ID isr_pon_message_id;
    
                /* Ploam TX item */
                LOGGER_GROUP_ID tx_event_id;
    
                /* Ploam RX item */
                LOGGER_GROUP_ID rx_event_id;

                LOGGER_GROUP_ID pmd_id;
    
                /* Ploam AES item */
                LOGGER_GROUP_ID aes_id;
    
                /* Ranging item */
                LOGGER_GROUP_ID ranging_id;
            }
            debug;

        }
        isr;
    }
    sections;
}
LOGGER;

extern LOGGER ge_onu_logger;

#endif /* USE_LOGGER */

#ifdef __cplusplus
}
#endif

#endif /* BL_LOGGER_H_INCLUDED */
