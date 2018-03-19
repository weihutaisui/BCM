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


#ifndef GPON_SM_H_INCLUDED
#define GPON_SM_H_INCLUDED

#include "pon_sm_common.h"
#include "gpon_general.h"

/* Activation state machine control */
typedef struct
{
    LINK_STATE_DTE link_state;
    LINK_SUB_STATE_DTE link_sub_state;
    LINK_OPERATION_STATES_DTE activation_state;
}
GPON_OPERATION_STATE_MACHINE_DTE;

/* Define the Operation state machine struct */
typedef struct
{
    /* In case LOF / LCDG event */
    bool lof_state;
    bool lcdg_state;

    /* incase receiving Ploam message */
    GPON_DS_PLOAM *ploam_message_ptr;
}
OPERATION_SM_PARAMS_DTS;

/******************************************************************************/
/* This type describes the possible AES states                                */
/******************************************************************************/
typedef uint32_t AES_STATE_DTE;

/* Not ready */
#define CE_AES_STATE_NOT_READY  ((AES_STATE_DTE)0)
/* Standby */                   
#define CE_AES_STATE_STANDBY    ((AES_STATE_DTE)1)
/* Scheduling */                
#define CE_AES_STATE_SCHEDULING ((AES_STATE_DTE)2)

#define GPON_DEFAULT_AES_KEY_SIZE 4 

/* AES state machine control */
typedef struct
{
    AES_STATE_DTE aes_state;
    uint32_t key_index;
    uint32_t seed_counter;
    AES_KEY_DTE new_key[GPON_DEFAULT_AES_KEY_SIZE];
    GPON_DS_PLOAM *aes_message_ptr;
    uint32_t current_switching_time;
    bool request_key_arrived;
}
GPON_AES_STATE_MACHINE_DTE;

typedef enum
{
    DBR_DIVIDE_RATIO_32_BYTES = 0,
    DBR_DIVIDE_RATIO_48_BYTES = 1,                      
    DBR_DIVIDE_RATIO_64_BYTES = 2,
}dbr_ratio;

typedef struct
{
    uint8_t onu_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t message_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t message_payload[CE_PLOAM_DATA_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t payload_crc __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
GPON_TX_PLOAM_MESSAGE;

/******************************************************************************/
/* PLOAM Message Priority                                                     */
/******************************************************************************/
typedef uint32_t PLOAM_PRIORITY_DTE;

/* The normal PLOAM cell memory. */
#define NORMAL_PLOAM_CELL ((PLOAM_PRIORITY_DTE)0x0)
/* The urgent PLOAM cell memory. */
#define URGENT_PLOAM_CELL ((PLOAM_PRIORITY_DTE)0x1)
/* The idle PLOAM cell memory.   */
#define IDLE_PLOAM_CELL   ((PLOAM_PRIORITY_DTE)0x2)

/******************************************************************************/
/* GPON Vendor Specific message type                                          */
/******************************************************************************/
typedef uint32_t GPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE;

/******************************************************************************/
/* GPON Vendor Specific message payload                                       */
/******************************************************************************/
#define CE_VENDOER_SPECIFIC_PLOAM_MESSAGE_PAYLOAD_FIELD_SIZE 10 
typedef struct
{
    /* Vendoer Specific PLOAM message payload */
    uint8_t message_payload[CE_VENDOER_SPECIFIC_PLOAM_MESSAGE_PAYLOAD_FIELD_SIZE];
}
GPON_VENDOR_SPECIFIC_MESSAGE_PAYLOAD_DTE;

/******************************************************************************/
/* This type defines the message conveys the Vendor_Specific downstream       */
/* PLOAM message's parameters                                                 */
/******************************************************************************/
typedef struct
{
    /* Vendor Specifc Id */
    GPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE type;
    /* Vendor Specific payload */
    GPON_VENDOR_SPECIFIC_MESSAGE_PAYLOAD_DTE payload;
}
GPON_VENDOR_SPECIFIC_MSG_DTE;

/* Define activation state machine events */
typedef enum
{
    /* 01 */ CS_OPERATION_EVENT_CLEAR_DS_LOS_OR_LOF = 0,
    /* 02 */ CS_OPERATION_EVENT_US_OVERHEAD_MESSAGE,
    /* 03 */ CS_OPERATION_EVENT_SERIAL_NUMBER_REQUEST,
    /* 04 */ CS_OPERATION_EVENT_ASSIGN_ONU_ID_MESSAGE,
    /* 05 */ CS_OPERATION_EVENT_RANGING_REQUEST,
    /* 06 */ CS_OPERATION_EVENT_CHANGE_POWER_LEVEL_MESSAGE,
    /* 07 */ CS_OPERATION_EVENT_RANGING_TIME_MESSAGE,
    /* 08 */ CS_OPERATION_EVENT_TIMER_TO1_EXPIRE,
    /* 09 */ CS_OPERATION_EVENT_DEACTIVATE_ONU_ID_MESSAGE,
    /* 10 */ CS_OPERATION_EVENT_DETECT_DS_LOS_OR_LOF,
    /* 11 */ CS_OPERATION_EVENT_BROADCAST_POPUP_REQUEST,
    /* 12 */ CS_OPERATION_EVENT_DIRECTED_POPUP_REQUEST,
    /* 13 */ CS_OPERATION_EVENT_TIMER_TO2_EXPIRE,
    /* 14 */ CS_OPERATION_EVENT_DISABLE_SERIAL_NUMBER_DISABLE_MESSAGE,
    /* 15 */ CS_OPERATION_EVENT_DISABLE_SERIAL_NUMBER_ENABLE_MESSAGE,
    /* 16 */ CS_OPERATION_EVENT_EXTENDED_BURST_LENGTH_MESSAGE,
    /* 17 */ CS_OPERATION_EVENT_ENCRYPT_PORT_ID_MESSAGE,
    /* 18 */ CS_OPERATION_EVENT_ASSIGN_ALLOC_ID_MESSAGE,
    /* 19 */ CS_OPERATION_EVENT_CONFIGURE_PORT_ID_MESSAGE,
    /* 20 */ CS_OPERATION_EVENT_KEY_SWITCHING_TIME_MESSAGE,
    /* 21 */ CS_OPERATION_EVENT_REQUEST_KEY_MESSAGE,
    /* 22 */ CS_OPERATION_EVENT_PHYSICAL_EQUIPMENT_ERROR_MESSAGE,
    /* 23 */ CS_OPERATION_EVENT_VENDOR_SPECIFIC_MESSAGE,
    /* 24 */ CS_OPERATION_EVENT_BER_INTERVAL_MESSAGE,
    /* 25 */ CS_OPERATION_EVENT_UNKNOWN_MESSAGE,
    /* 26 */ CS_OPERATION_EVENT_REQUEST_PASSWORD,
    /* 27 */ CS_OPERATION_EVENT_PST_MESSAGE,
    /* 28 */ CS_OPERATION_EVENT_SERIAL_NUMBER_MASK_MESSAGE,
    /* 29 */ CS_OPERATION_EVENT_SLEEP_ALLOW_MESSAGE, 
    /* 30 */ CS_OPERATION_EVENT_PON_ID_MESSAGE,
    /* 31 */ CS_OPERATION_EVENT_SWIFT_POPUP_MESSAGE,
    /* 32 */ CS_OPERATION_EVENT_RANGING_ADJUSTMENT_MESSAGE,

    /* 30 */ CS_OPERATION_EVENTS_NUMBER 
}
ACTIVATION_EVENTS_DTS;




/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_operation_state_machine                                                */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Operation State Machine                                           */
/*                                                                            */
/******************************************************************************/
void p_operation_state_machine(ACTIVATION_EVENTS_DTS xi_event,
    OPERATION_SM_PARAMS_DTS *xi_operation_sm_params);


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
PON_ERROR_DTE gpon_link_reset(void);


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
PON_ERROR_DTE gpon_link_deactivate(void);


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
PON_ERROR_DTE gpon_link_activate(bdmf_boolean xi_initial_state_disable);


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
    uint8_t ndbr_content);


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
PON_ERROR_DTE gpon_send_dying_gasp_message(void);


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
    GPON_VENDOR_SPECIFIC_MSG_DTE *const xi_message_ptr);


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
PON_ERROR_DTE gpon_send_pee_message(void);


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
PON_ERROR_DTE gpon_send_pst_message(PON_PST_INDICATION_PARAMETERS_DTE xi_pst_configuration_struct);


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
    uint8_t xi_message_id, uint8_t xi_repetition, uint8_t xi_priority);


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
PON_ERROR_DTE f_gpon_txpon_fetch_ploam_message(PLOAM_PRIORITY_DTE xi_ploam_priority);


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
PON_ERROR_DTE f_gpon_txpon_flush_queue(void);


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
PON_ERROR_DTE gpon_get_aes_encryption_key(AES_KEY_DTE *xi_aes_encryption_key, uint8_t xi_fragment_index);


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
    GPON_PLOAM_TRANSMISSION_NUMBER xi_normal_ploam_buffer_repetition);

#endif
