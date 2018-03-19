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

#ifndef NGPON_STACK_H_INCLUDED
#define NGPON_STACK_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif

#include "ngpon_ploam.h"
#include "ngpon_general.h"















/******************************************************************************/
/*                                                                            */
/* OAM alarm type                                                             */
/*                                                                            */
/******************************************************************************/
#ifndef XGPON_OAM_ALARMS_ID_DEFINED
#define XGPON_OAM_ALARMS_ID_DEFINED

typedef uint32_t NGPON_OAM_ALARM_ID_DTE ;

/* LCDG alarm id */
#define CE_OAM_ALARM_ID_LCDG   ( ( NGPON_OAM_ALARM_ID_DTE ) 0x1 )

/* LOF alarm id */
#define CE_OAM_ALARM_ID_LOF    ( ( NGPON_OAM_ALARM_ID_DTE ) 0x2 )

/* PEE from OLT alarm */
#define CE_OAM_ALARM_ID_PEE    ( ( NGPON_OAM_ALARM_ID_DTE ) 0x3 )

/* SUF alarm id */
#define CE_OAM_ALARM_ID_SUF    ( ( NGPON_OAM_ALARM_ID_DTE ) 0x4 )

/* PLOAM synchronization loss */
#define CE_OAM_ALARM_ID_OAML   ( ( NGPON_OAM_ALARM_ID_DTE ) 0x5 )

/* Frame loss */
#define CE_OAM_ALARM_ID_FRML   ( ( NGPON_OAM_ALARM_ID_DTE ) 0x6 )

/* Cell delineation loss */
#define CE_OAM_ALARM_ID_LCD    ( ( NGPON_OAM_ALARM_ID_DTE ) 0x7 )

/* BIP error */
/* #define CE_OAM_ALARM_ID_ERR    ( ( NGPON_OAM_ALARM_ID_DTE ) 0x8 ) */

/* Downstream BER value exceed 10exp-5 */
#define CE_OAM_ALARM_ID_SD     ( ( NGPON_OAM_ALARM_ID_DTE ) 0x9 )

/* Message error */
#define CE_OAM_ALARM_ID_MEM    ( ( NGPON_OAM_ALARM_ID_DTE ) 0xA )

/* Deactivate message with match PON_Id */
#define CE_OAM_ALARM_ID_DACT   ( ( NGPON_OAM_ALARM_ID_DTE ) 0xB )

/* Disable_Serial_Number message with match PON_Id */
#define CE_OAM_ALARM_ID_DIS                    ( ( NGPON_OAM_ALARM_ID_DTE ) 0xC )

/* LOF received in operational state */
#define CE_OAM_ALARM_ID_OPERATIONAL_LOF        ( ( NGPON_OAM_ALARM_ID_DTE ) 0xD )

#endif


/******************************************************************************/
/* This type defines the Alarm status struct                                  */
/******************************************************************************/
typedef struct
{
    /* Dis indication */
    bdmf_boolean dis ;

    /* Dact indication */
    bdmf_boolean dact ;

    /* Mem indication */
    bdmf_boolean mem ;

    /* Sd indication */
    bdmf_boolean sd ;

    /* Sf indication */
    bdmf_boolean sf ;

    /* Suf indication */
    bdmf_boolean suf ;

    /* Err indication */
    bdmf_boolean err ;

    /* Pee_Olt indication */
    bdmf_boolean pee_olt ;

    /* Tf indication */
    bdmf_boolean tf ;

    /* Lcd indication */
    bdmf_boolean lcd ;

    /* Oaml indication */
    bdmf_boolean oaml ;

    /* Los indication */
    bdmf_boolean los ;

    /* Onu state transition indication */
    bdmf_boolean onu_state_transition ;

    /* Bip Error indication */
    bdmf_boolean bip_error ;

    /* Keep alive indication */
    bdmf_boolean keep_alive ;

    /* Ranging Start indication */
    bdmf_boolean ranging_start ;

    /* Ranging Stop indication */
    bdmf_boolean ranging_stop ;

    /* Vendor Specific indication */
    bdmf_boolean vendor_specific ;

    /* Unknown Message indication */
    bdmf_boolean unknown_message ;
}
NGPON_ALARM_STATUS_DTE ;

/*************************************************************************************/
/*                                                                                   */   
/* This type defines the opcodes of the messages destinate to user callback function */
/*                                                                                   */   
/*************************************************************************************/
#ifndef XGPON_MESSAGES_TYPE_DEFINED
#define XGPON_MESSAGES_TYPE_DEFINED
                     
typedef uint32_t NGPON_API_MSG ;

/* This opcode indicates a state transition to one of the next states: STOP, STANDBY or OPERATIONAL. */
#define XGPON_API_MSG_GPON_SM_TRANSITION                                              ( ( XGPON_API_MSG ) 0x0 )
/* This opcode indicates a link state transition operation state changed */
#define XGPON_API_MSG_GPON_LINK_STATE_TRANSITION                                      ( ( XGPON_API_MSG ) 0x1 )
/* This opcode indicates that assign alloc id message has been sent by the OLT. */
#define XGPON_API_MSG_ASSIGN_ALLOC_ID_MESSAGE                                         ( ( XGPON_API_MSG ) 0x4 )
/* This opcode indicates that a divided slot grant allocation message has been sent by the OLT. */
#define XGPON_API_MSG_DIVIDED_SLOT_GRANT_ALLOCATION_MESSAGE                           ( ( XGPON_API_MSG ) 0x5 )
/* This opcode indicates that a vendor specific message has been sent by the OLT. */
#define XGPON_API_MSG_VENDOR_SPECIFIC_MESSAGE                                         ( ( XGPON_API_MSG ) 0x6 )
/* This opcode indicates that an unknown message has been sent by the OLT. */
#define XGPON_API_MSG_UNKNOWN_MESSAGE                                                 ( ( XGPON_API_MSG ) 0x7 )
/* This opcode indicates that a PST message has been sent by the OLT. */
#define XGPON_API_MSG_PST_MESSAGE                                                     ( ( XGPON_API_MSG ) 0x8 )
/* This opcode indicates a keep alive timeout, since a PLOAM message has not been sent for x seconds. */
#define XGPON_API_MSG_KEEPALIVE_TIMEOUT                                               ( ( XGPON_API_MSG ) 0xB )
/* This opcode indicates that FIFO full interrupt from the RX/TX FIFO was issued. */
#define XGPON_API_MSG_FIFO_FULL                                                       ( ( XGPON_API_MSG ) 0xE )
/* This opcode indicates queue event happened (one or more of the queue interrupts in the FIFO was active .) */
#define XGPON_API_MSG_QUEUE_EVENT                                                     ( ( XGPON_API_MSG ) 0x10 )
/* This opcode indicates an software error in the apon stack in base level. */
#define XGPON_API_MSG_BASE_SW_ERROR                                                   ( ( XGPON_API_MSG ) 0x11 )
/* This opcode indictaes an OAM event/alarm */                                       
#define XGPON_API_MSG_OAM_EVENT                                                       ( ( XGPON_API_MSG ) 0x13 )
/* This opcode indictaes Churn_VP message received. */                               
#define XGPON_API_MSG_CHURN_VP_MESSAGE                                                ( ( XGPON_API_MSG ) 0x16 )
/* This opcode indictaes BIP error in the downstream. */                             
#define XGPON_API_MSG_BIP_ERROR                                                       ( ( XGPON_API_MSG ) 0x17 ) 
/* This opcode indictaes a link ( internal ) error interrupt */
#define XGPON_API_MSG_LINK_ERROR                                                      ( ( XGPON_API_MSG ) 0x19 ) 
/* This opcode indictaes start of ranging. */                                        
#define XGPON_API_MSG_RANGING_START                                                   ( ( XGPON_API_MSG ) 0x1a ) 
/* This opcode indictaes stop of ranging */                                          
#define XGPON_API_MSG_RANGING_STOP                                                    ( ( XGPON_API_MSG ) 0x1b ) 
/* This opcode indictates encrypt port id */                                         
#define XGPON_API_MSG_ENCRYPT_PORT_ID                                                 ( ( XGPON_API_MSG ) 0x1c ) 
/* This opcode indictates configure OMCI port id */                                  
#define XGPON_API_MSG_CONFIGURE_OMCI_PORT_ID                                          ( ( XGPON_API_MSG ) 0x1d ) 
/* This opcode indictates AES message */                                             
#define XGPON_API_MSG_AES_KEY_SWITCHING_TIME                                          ( ( XGPON_API_MSG ) 0x1e ) 
/* This opcode indictates AES error */                                               
#define XGPON_API_MSG_AES_ERROR                                                       ( ( XGPON_API_MSG ) 0x1f ) 
/* This opcode indictates OPERATIONAL LOF indication */                              
#define XGPON_API_MSG_OPERATIONAL_LOF                                                 ( ( XGPON_API_MSG ) 0x20 ) 
                                                                                     
#if defined ( MAC_CLEANUP )                                                          
/* This opcode indictates Non-Critical BER indication */                             
#define XGPON_API_MSG_NON_CRITICAL_BER                                                ( ( XGPON_API_MSG ) 0x21 ) 
/* This opcode indictates Critical BER indication */                                 
#define XGPON_API_MSG_CRITICAL_BER                                                    ( ( XGPON_API_MSG ) 0x22 ) 
#endif /* MAC_CLEANUP */

/* This opcode indicates that assign ONU id message has been sent by the OLT. */
#define XGPON_API_MSG_ASSIGN_ONU_ID_MESSAGE                                           ( ( XGPON_API_MSG ) 0x23 )


#endif


/******************************************************************************/
/* GPON Vendor Specific message type                                          */
/******************************************************************************/
typedef uint32_t NGPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE ;

/* vendor specific message type 0 */
#define CE_VENDOR_SPECIFIC_MESSAGE_TYPE_0 ( ( XGPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE ) 0 )
/* vendor specific message type 1 */
#define CE_VENDOR_SPECIFIC_MESSAGE_TYPE_1 ( ( XGPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE ) 1 )
/* vendor specific message type 2 */
#define CE_VENDOR_SPECIFIC_MESSAGE_TYPE_2 ( ( XGPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE ) 2 )
/* vendor specific message type 3 */
#define CE_VENDOR_SPECIFIC_MESSAGE_TYPE_3 ( ( XGPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE ) 3 )
/* vendor specific message type 4 */
#define CE_VENDOR_SPECIFIC_MESSAGE_TYPE_4 ( ( XGPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE ) 4 )
/* vendor specific message type 5 */
#define CE_VENDOR_SPECIFIC_MESSAGE_TYPE_5 ( ( XGPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE ) 5 )
/* vendor specific message type 6 */
#define CE_VENDOR_SPECIFIC_MESSAGE_TYPE_6 ( ( XGPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE ) 6 )
/* vendor specific message type 7 */
#define CE_VENDOR_SPECIFIC_MESSAGE_TYPE_7 ( ( XGPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE ) 7 )
/* vendor specific message mask */
#define CE_VENDOR_SPECIFIC_MESSAGE_MASK   ( ( XGPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE ) 7 )


/******************************************************************************/
/* GPON Vendor Specific message payload                                       */
/******************************************************************************/
#define CE_VENDOER_SPECIFIC_PLOAM_MESSAGE_PAYLOAD_FIELD_SIZE ( 10 )
typedef struct
{
    /* Vendoer Specific PLOAM message payload */
    uint8_t message_payload [ CE_VENDOER_SPECIFIC_PLOAM_MESSAGE_PAYLOAD_FIELD_SIZE ] ;
}
NGPON_VENDOR_SPECIFIC_MESSAGE_PAYLOAD_DTE ;
       



/******************************************************************************/
/* This type defines the states of the GPON software                          */
/******************************************************************************/
typedef uint32_t NGPON_SOFTWARE_STATUS_DTE ;

/* The GPON stack is unbound */
/* #define CE_XGPON_STATE_UNBOUND       ( ( XGPON_SOFTWARE_STATUS_DTE ) 0 ) */
/* The GPON stack is being initialized */
#define CE_XGPON_STATE_INITIALIZED   ( ( XGPON_SOFTWARE_STATUS_DTE ) 0 )
/* The GPON stack is terminated */
#define CE_XGPON_STATE_TERMINATED    ( ( XGPON_SOFTWARE_STATUS_DTE ) 1 )






/******************************************************************************/
/* This type defines the ONU Registration for PON network access              */
/******************************************************************************/
#ifndef NGPON_REGISTRATION_ID_DEFINED
#define NGPON_REGISTRATION_ID_DEFINED

typedef struct
{
    /* Registration ID */
    uint8_t reg_id [ REGISTRATION_ID_FIELD_SIZE ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
__PACKING_ATTRIBUTE_STRUCT_END__

NGPON_REGISTRATION_ID ;
#endif

/******************************************************************************/
/* This type defines the message conveys the Vendor_Specific downstream       */
/* PLOAM message's parameters                                                 */
/******************************************************************************/


typedef struct
{
    /* Vendor Specifc Id */
    NGPON_VENDOR_SPECIFIC_MESSAGE_TYPE_DTE type ;

    /* Vendor Specific payload */
    NGPON_VENDOR_SPECIFIC_MESSAGE_PAYLOAD_DTE payload ;
}
NGPON_VENDOR_SPECIFIC_MSG_DTE ;



/******************************************************************************/
/* This type defines the Link specifier                                     */
/******************************************************************************/
typedef uint32_t XGPON_LINK_DTE ;

/* Dont care */
#define CE_NGPON_LINK_DONT_CARE ( ( NGPON_LINK_DTE ) 0xFFFF )








/******************************************************************************/
/* This type describes The period of the device timer in ms                   */
/******************************************************************************/
#define CE_DEVICE_TIMER_PERIOD  ( 100 )      

/******************************************************************************/
/* This type describes The period of Keep Alive timer in ms                   */
/******************************************************************************/
#define CE_DEVICE_KEEP_ALIVE_TIMER_PERIOD  ( 500 )      

/******************************************************************************/
/* This type describes The period of Ber Interval timer in ms                 */
/******************************************************************************/
#define CE_DEVICE_BER_INTERVAL_TIMER_PERIOD  ( 100 )     
 

/******************************************************************************/
/* This type describes the threshold of the SN messages requests              */
/******************************************************************************/
#define CE_SN_REQUEST_THRESHOLD ( 10 )

/******************************************************************************/
/* This type describes the power level range to use for tranceiver power level*/
/******************************************************************************/
#define CE_POWER_LEVEL_RANGE ( 3 ) 

/******************************************************************************/
/* This type describes the seed threshold to use in seed generation for AES   */
/* process .                                                                  */
/******************************************************************************/
#define CE_SEED_THRESHOLD ( 3 )

/******************************************************************************/
/* This type defines number of downstream frames in milli second              */
/*                                                                            */
/******************************************************************************/
#define CE_DOWNSTREAM_FRAMES_IN_MS ( 8 )




/*****************************************************************************************/
/* This type define the enable/disable and configuration of filter indication parameters */
/*****************************************************************************************/
typedef struct
{
    /* Flow / filter id */
    PON_FLOW_ID_DTE flow_id ;
    
    /* Enable / Disable filter */
    bdmf_boolean enable_flag ;

    /* port id */
    NGPON_XGEM_PORT_ID xgem_port_id ;

    /* port mask id */
    NGPON_XGEM_PORT_ID xgem_port_mask_id ;

    /* Flow priority - (Low/High/Omci) */
    PON_FLOW_PRIORITY_DTE flow_priority ;

    /* UNICAST_ONU_1 , UNICAST_ONU_2 , MULTICAST  , PLAIN TEXT  ,NONE */
    NGPON_ENCRYPTION_RING    encryption_ring ;
    bdmf_boolean             crc_enable ;

    bdmf_boolean             flow_configured ;

    bdmf_boolean             encryption ;  /* True - encryption configured */
}
NGPON_DS_XGEM_INDICATION_PARAMS ;


/******************************************************************************/
/* This type define the LCDG indication parameters                             */
/******************************************************************************/
typedef struct
{
    bdmf_boolean alarm ;
}
NGPON_LCDG_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the LOF indication parameters                             */
/******************************************************************************/
typedef struct
{
    bdmf_boolean alarm ;
}
NGPON_LOF_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the DIS indication parameters                             */
/******************************************************************************/
typedef struct
{
    bdmf_boolean alarm ;
}
NGPON_DIS_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the Dact indication parameters                            */
/******************************************************************************/
typedef struct
{
    bdmf_boolean alarm ;
}
NGPON_DACT_INDICATION_PARAMS ;


/******************************************************************************/
/* This type define the MEM indication parameters                             */
/******************************************************************************/
typedef struct
{
    bdmf_boolean alarm ;
    NGPON_DS_PLOAM unknown_msg ;
}
NGPON_MEM_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the OAML indication parameters                           */
/******************************************************************************/
typedef struct
{
    NGPON_ALARM_STATUS_DTE oaml_alarm_status ;
}
NGPON_OAML_INDICATION_PARAMS ;


/******************************************************************************/
/* This type define the SD indication parameters                              */
/******************************************************************************/
typedef struct
{
    /* user bip rate */
    uint32_t user_bip_rate ;
    
    bdmf_boolean alarm ;
}
NGPON_SD_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the SF indication parameters                              */
/******************************************************************************/
typedef struct
{
    /* user bip rate */
    uint32_t user_bip_rate ;
    
    bdmf_boolean alarm ;
}
NGPON_SF_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the SUF  indication parameters                            */
/******************************************************************************/
typedef struct
{
    bdmf_boolean alarm ;
}
NGPON_SUF_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the BER  indication parameters                            */
/******************************************************************************/
typedef struct
{
    bdmf_boolean alarm ;
}
NGPON_BER_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the PEE_OLT  indication parameters                        */
/******************************************************************************/
typedef struct
{
    bdmf_boolean alarm ;
}
NGPON_PEE_OLT_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the TF  indication parameters                             */
/******************************************************************************/
typedef struct
{
    bdmf_boolean alarm ;
}
NGPON_TF_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the LCD  indication parameters                            */
/******************************************************************************/
typedef struct
{
    bdmf_boolean alarm ;
}
NGPON_LCD_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the ONU state transition indication parameters            */
/******************************************************************************/
typedef struct
{
    /* ONU Old States */
    LINK_SUB_STATE_DTE onu_old_sub_state ;

    /* ONU New States */
    LINK_SUB_STATE_DTE onu_new_sub_state ;
}
NGPON_ONU_STATE_TRANSITION_INDICATION_PARAMS ;


/******************************************************************************/
/* This type define the ONU Link state transition indication parameters       */
/******************************************************************************/
typedef struct
{
    /* ONU Old States */
    LINK_OPERATION_STATES_DTE onu_old_link_state ;

    /* ONU New States */
    LINK_OPERATION_STATES_DTE onu_new_link_state ;
}
NGPON_ONU_LINK_STATE_TRANSITION_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the PST indication parameters                             */
/******************************************************************************/
typedef struct
{
    /* line_number */
    bdmf_boolean line_number ;

    /* k1 */
    uint8_t k1 ;

    /* k2 */
    uint8_t k2 ;

    /* PST message */
    NGPON_DS_PLOAM pst_msg ;
}
NGPON_PST_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the AES indication parameters                             */
/******************************************************************************/
typedef struct
{
    /* Key switching time */
    uint32_t current_superframe ;

    /* Key switching time */
    uint32_t key_switching_time ;

    /* Error id */
    uint32_t error_id ;
}
NGPON_AES_INDICATION_PARAMS ;



/******************************************************************************/
/* This type define the bip error  indication parameters                      */
/******************************************************************************/
typedef struct
{
    /* Bip Error */
    uint32_t bip_error ;
    
    /* Bod error alarm status */
    bdmf_boolean alarm ;
}
NGPON_ERROR_INDICATION_PARAMS ;

/******************************************************************************/
/* This type define the keep alive  indication parameters                     */
/******************************************************************************/
typedef struct
{
    uint32_t keepalive_time_value ;
}
NGPON_KEEP_ALIVE_INDICATION_PARAMS ;



/******************************************************************************/
/* This type define the ranging start indication parameters                   */
/******************************************************************************/
typedef struct
{
}
NGPON_RANGING_START_INDICATION_PARAMS ;


/*****************************************************************************************/
/* This type define the Rogue ONU detection indication parameters                        */
/*****************************************************************************************/
typedef enum
{
    TX_MONITOR_ROGUE_MODE,
    TX_FAULT_ROGUE_MODE,
    TX_INVALID_ROGUE_MODE
}
NGPON_ROGUE_ONU_MODE ;

typedef struct
{
    /* In case rogue onu event */
    bdmf_boolean rogue_state ;

    NGPON_ROGUE_ONU_MODE rogue_type ;
}
NGPON_ROGUE_INDICATION_PARAMS ;

/******************************************************************************/
/* This type defines the reasons for ranging stop types                       */
/******************************************************************************/
typedef uint32_t NGPON_RANGING_STOP_REASONS_INDICATIONS ;

/* Device deactivate */
#define CE_XGPON_RANGING_STOP_REASON_LINK_DEACTIVATE   ( ( NGPON_RANGING_STOP_REASONS_INDICATIONS ) 0 )
/* Alarm On */
#define CE_XGPON_RANGING_STOP_REASON_ALARM_ON          ( ( NGPON_RANGING_STOP_REASONS_INDICATIONS ) 1 )
/* SN Unmatch */
#define CE_XGPON_RANGING_STOP_REASON_SN_UNMATCH        ( ( NGPON_RANGING_STOP_REASONS_INDICATIONS ) 2 )
/* Deactivate Pon Id */
#define CE_XGPON_RANGING_STOP_REASON_DEACTIVATE_PON_ID ( ( NGPON_RANGING_STOP_REASONS_INDICATIONS ) 3 )
/* Disable SN */
#define CE_XGPON_RANGING_STOP_REASON_DISABLE_SN        ( ( NGPON_RANGING_STOP_REASONS_INDICATIONS ) 4 )
/* Ranging End */
#define CE_XGPON_RANGING_STOP_REASON_RANGING_END       ( ( NGPON_RANGING_STOP_REASONS_INDICATIONS ) 5 )
/* Timeout Expired */
#define CE_XGPON_RANGING_STOP_REASON_TIMEOUT_EXPIRED   ( ( NGPON_RANGING_STOP_REASONS_INDICATIONS ) 6 )
/* Number of ranging stop reasos */
#define CE_XGPON_RANGING_STOP_REASONS_NUMBER            ( ( NGPON_RANGING_STOP_REASONS_INDICATIONS ) 7 )

/******************************************************************************/
/* This type define the ranging stop  indication parameters                   */
/******************************************************************************/
typedef struct
{
    /* Ranging stop reason */
    NGPON_RANGING_STOP_REASONS_INDICATIONS ranging_stop_reason ;
}
NGPON_RANGING_STOP_INDICATION_PARAMS ;




/******************************************************************************/
/* This type define the assign ONU id indication parameters                   */
/******************************************************************************/
typedef struct
{
    /* ONU id */
    NGPON_ONU_ID onu_id ;
}
NGPON_ASSIGN_ONU_ID_INDICATION_PARAMS ;


/******************************************************************************/
/* This type define the configure OMCI port id indication parameters               */
/******************************************************************************/
typedef struct
{
    /* Activate/Deactivate port-id flag */
    bdmf_boolean port_control ;
 
    /* Downstream port id */
    NGPON_XGEM_PORT_ID   port_id ;
}
NGPON_CONFIGURE_OMCI_PORT_ID_INDICATION_PARAMS ;




/******************************************************************************/
/* This type define the enable/disable filter indication parameters           */
/******************************************************************************/
typedef struct
{
    /* Flow / filter id */
    PON_FLOW_ID_DTE flow_id ;
    
    /* Enable / Disable filter */
    bdmf_boolean configure_flag ;
}
NGPON_CONTROL_FILTER_INDICATION_PARAMS ;

/********************************************************************/
/* This type define the TDM control indication parameters           */
/********************************************************************/
typedef struct
{
    /* TDM Control */
    bdmf_boolean control_flag ;
}
XGPON_CONTROL_TDM_INDICATION_PARAMS ;

/********************************************************************/
/* This type define the TDM configuration indication parameters     */
/********************************************************************/
typedef struct
{
    /* Tcont queue id */
    NGPON_TCONT_QUEUE_ID tcont_queue_id ;

    /* Port id */
    NGPON_XGEM_PORT_ID port_id ;
}
XGPON_CONFIGURE_TDM_INDICATION_PARAMS ;

/********************************************************************/
/* This type define the OPERATIONAL LOF indication parameters       */
/********************************************************************/
typedef struct
{
}
NGPON_OPERATIONAL_LOF_INDICATION_PARAMS ;


/******************************************************************************/
/* This type define the parameters recieves by all indications                */
/******************************************************************************/
typedef union
{
    /* Lcdg indication */
    NGPON_LCDG_INDICATION_PARAMS lcdg ;

    /* Lof indication */
    NGPON_LOF_INDICATION_PARAMS lof ;
    
    /* Dis indication */
    NGPON_DIS_INDICATION_PARAMS dis ;

    /* Dact indication */
    NGPON_DACT_INDICATION_PARAMS dact ;

    /* Mem indication */
    NGPON_MEM_INDICATION_PARAMS mem ;

    /* Oaml indication */
    NGPON_OAML_INDICATION_PARAMS oaml ;

    /* Sd indication */
    NGPON_SD_INDICATION_PARAMS sd ;

    /* Sf indication */
    NGPON_SD_INDICATION_PARAMS sf ;
   
    /* Onu state transition indication */
    NGPON_ONU_STATE_TRANSITION_INDICATION_PARAMS onu_state_transition ;

    /* Onu state transition indication */
    NGPON_ONU_LINK_STATE_TRANSITION_INDICATION_PARAMS onu_link_state_transition ;

    /* Bip Error indication */
    NGPON_ERROR_INDICATION_PARAMS bip_error ;

    /* SUF Error */
    NGPON_SUF_INDICATION_PARAMS suf ;

    /* Keep alive indication */
    NGPON_KEEP_ALIVE_INDICATION_PARAMS keep_alive ;

    /* Ranging Start indication */
    NGPON_RANGING_START_INDICATION_PARAMS ranging_start ;

    /* Ranging Stop indication */
    NGPON_RANGING_STOP_INDICATION_PARAMS ranging_stop ;
    
    /* Assign Alloc Id indication */
    NGPON_ASSIGN_ALLOC_ID_INDICATION_PARAMS assign_alloc_id ;

    /* Assign ONU ID indication */
    NGPON_ASSIGN_ONU_ID_INDICATION_PARAMS assign_onu_id ;

    /* Control port indiction */
    NGPON_CONTROL_FILTER_INDICATION_PARAMS control_port ;

    /* Configure port indication */
    NGPON_DS_XGEM_INDICATION_PARAMS configure_port ;

    /* Configure OMCI port indication */
    NGPON_CONFIGURE_OMCI_PORT_ID_INDICATION_PARAMS configure_omci_port ;
      
    /* Pee message from OLT */
    NGPON_PEE_OLT_INDICATION_PARAMS pee_olt ;

    /* PST message from OLT */
    NGPON_PST_INDICATION_PARAMS pst ;
   
    /* AES messages from OLT */
    NGPON_AES_INDICATION_PARAMS aes ;

    /* OPERATIONAL LOF indication */
    NGPON_OPERATIONAL_LOF_INDICATION_PARAMS operational_lof ;

    /* Rogue ONU parameters */
    NGPON_ROGUE_INDICATION_PARAMS rogue_onu;
}
NGPON_INDICATIONS_PARAMETERS ;

/******************************************************************************************/
/*                                                                                        */
/* structure of Timer control ( pee & ber ) : actins like start stop  + interval forber   */
/*                                                                                        */
/******************************************************************************************/
typedef struct
{
    bdmf_boolean      command ;
    uint32_t      interval ;
}
TIMER_CONTROL ;


/******************************************************************************/
/*                                                                            */
/* Structure of device error stored in correspondent pon task message         */
/*                                                                            */
/******************************************************************************/

typedef struct
{
    NGPON_OAM_ALARM_ID_DTE oam_alarm_id  ;

    bdmf_boolean oam_alarm_status  ;

    union 
    {
      NGPON_DS_PLOAM ploam_message ;
    } oam_payload  ;
}
NGPON_OAM ;

typedef struct
{
    uint32_t indication_id ;
    NGPON_INDICATIONS_PARAMETERS indication_data ;
}
NGPON_USER_INDICATION ;


/******************************************************************************/
/*  Power level parameters structure                                          */
/******************************************************************************/


typedef  uint8_t NGPON_CHANGE_POWER_LEVEL_REASON ;


/* Change power level ploam */
#define NGPON_CHANGE_POWER_LEVEL_REASON_CHANGE_POWER_LEVEL_PLOAM                ((NGPON_CHANGE_POWER_LEVEL_REASON) 0)
/* Serial number requests threshold crossed */
#define NGPON_CHANGE_POWER_LEVEL_REASON_SN_REQUEST_THRESHOLD                    ((NGPON_CHANGE_POWER_LEVEL_REASON) 1)
/* Upstream overhead ploam*/
#define NGPON_CHANGE_POWER_LEVEL_REASON_UPSTREAM_OVERHEAD_PLOAM                 ((NGPON_CHANGE_POWER_LEVEL_REASON_DTE) 2)

typedef enum
{
    NGPON_POWER_LEVEL_LOW = 0 ,
    NGPON_POWER_LEVEL_NORMAL ,
    NGPON_POWER_LEVEL_HIGH ,
    NGPON_MAX_NUM_OF_POWER_LEVEL ,
    NGPON_POWER_LEVEL_ILLEGAL
}
NGPON_POWER_LEVEL_MODE ;

typedef struct
{
    /* power level mode */
    NGPON_POWER_LEVEL_MODE power_level_mode ;

    /* change power level reason */
    NGPON_CHANGE_POWER_LEVEL_REASON reason ;
}
NGPON_POWER_LEVEL_PARAMETERS ;

#define MS_INCREASE_POWER_LEVEL( m ) { ( m + 1 ) % CE_XGPON_MAX_NUM_OF_POWER_LEVEL }
#define MS_DECREASE_POWER_LEVEL( m ) { ( m + 2 ) % CE_XGPON_MAX_NUM_OF_POWER_LEVEL }

/******************************************************************************/
/* This type defines the panic function callback                              */
/******************************************************************************/
typedef void ( * PANIC_CALLBACK_DTE ) ( void ) ;

/******************************************************************************/
/* This type defines the event and alarms function callback                  */
/******************************************************************************/
//typedef void (*EVENT_AND_ALARM_CALLBACK_DTE) (NGPON_API_MSG message_id , NGPON_USER_INDICATION user_indication_message);


typedef void (*CHANGE_POWER_LEVEL_CALLBACK) (NGPON_POWER_LEVEL_PARAMETERS param) ;

/* Callbacks */
typedef struct
{
    /* Panic */ 
    PANIC_CALLBACK_DTE panic_callback ;

    /* Events and Alarms */
    EVENT_AND_ALARM_CALLBACK_DTE event_and_alarm_callback ;

    /* DBR runner data callback */
  /*    Not sure whether we need it ???
    DBR_PROCESS_RUNNER_DATA_CALLBACK_DTE dbr_runner_data_callback ;
  */
    /* Change power level callback */
    CHANGE_POWER_LEVEL_CALLBACK change_power_level_callback ;

    /* Sc SC callback */
    SC_SC_RUNNER_DATA_CALLBACK_DTE sc_sc_runner_callback ;

    /* Flush TCONT callback */
    FLUSH_TCONT_CALLBACK_DTE flush_tcont_callback ;
}
NGPON_CALLBACKS ;


/******************************************************************************/
/*                                                                            */
/* structure of pon task message                                              */
/*                                                                            */
/******************************************************************************/
typedef struct
{
    
    PON_EVENTS_DTE  message_id ;
    
    union
    {
        NGPON_OAM                           oam ;
        uint32_t                            fifo_block ;
        uint32_t                            keepalive_timeout ;
        NGPON_DS_PLOAM                      ploam_message ;
        PON_USER_INDICATION_DTE             user_indication;
        TIMER_CONTROL                       timer_control ;
        NGPON_CALLBACKS                     callback_routines ;
        NGPON_POWER_LEVEL_PARAMETERS        power_level ;
    } message_data ;
}
NGPON_MESSAGE ;    



/******************************************************************************/
/* This type defines the alarm calls to the callback function                 */
/******************************************************************************/
typedef uint32_t ALARM_CALL_DTE ;


/******************************************************************************/
/* This type defines the information calls to the callback function           */
/******************************************************************************/
typedef uint32_t INFORMATION_CALL_DTE ;

/* Asynchronous command completed */
#define CE_INFORMATION_CALL_ASYNCHRONOUS_COMMAND_COMPLETED ( ( INFORMATION_CALL_DTE ) 0 )

/* TBD */
#define CE_INFORMATION_CALL_TBD                            ( ( INFORMATION_CALL_DTE ) 100 )


/******************************************************************************/
/* This type defines the information function callback                        */
/******************************************************************************/
/*typedef XGPON_ERROR_DTE ( * INFORMATION_CALLBACK_DTE ) ( INFORMATION_CALL_DTE xi_information, uint32_t xi_data ) ;*/



/******************************************************************************/
/* This type defines the packet available callback                            */
/******************************************************************************/
/*typedef XGPON_ERROR_DTE ( * PACKET_AVAILABLE_CALLBACK_DTE ) ( void  ) ;*/


/*###############################################################################*/



/* TX PON counters  */
typedef struct
{    
    uint16_t      received_access [NGPON_NUM_OF_CONFIGURABLE_COUNTERS];
    uint16_t      requested_dbr_counter [NGPON_NUM_OF_CONFIGURABLE_COUNTERS];
    uint32_t      transmitted_packet_counter [NGPON_NUM_OF_CONFIGURABLE_COUNTERS];
    uint32_t      transmitted_xgem_frames_counter [NGPON_NUM_OF_CONFIGURABLE_COUNTERS];
    uint32_t      transmitted_xgem_idle_frames_counter [NGPON_NUM_OF_CONFIGURABLE_COUNTERS];
    uint32_t      requested_ploam_counter [NGPON_PLOAM_TYPE_HIGH + 1];
}
XGPON_TXPON_PM_COUNTERS  ;


/* RX PON counters  */
typedef struct
{
    uint16_t       lof_num_counter ;
    uint16_t       hec_err_counter ;
    uint16_t       bwmap_invalid_burst_series_counter ;
    uint16_t       hlend_hec_err_counter ;
    uint32_t       lcdg_counter ;
    uint16_t       sync_ponid_hec_err_counter ;
    uint32_t       del_pass_pkt_counter ;
    uint32_t       bwmap_correct_counter ;
    uint16_t       bwmap_bursts_counter ;
    uint32_t       pass_xgem_filter_pkt_counter ;
    uint16_t       cipher_cant_allocate_counter ;
    uint16_t       xgem_frame_invalid_key_idx_counter ;
    uint16_t       valid_unicast_ploams_counter [ NGPON_ONU_INDEX_UNICAST_ONU_ID_HIGH + 1 ] ;
    uint16_t       valid_broadcast_ploams_counter ;
    uint16_t       ploam_mic_err_counter ;
    uint16_t       bwmap_hec_fix_counter ;
    uint16_t       hlend_hec_fix_counter ;
    uint16_t       xgem_header_hec_fix_counter ;
    uint16_t       superframe_hec_fix_counter ;
    uint16_t       pon_id_hec_fix_counter ;
    uint16_t       xgem_overrun_counter ;
    uint16_t       bwmap_discard_dis_tx_counter ;
    uint32_t       corrected_fec_bits_counter ;
    uint32_t       corrected_fec_symbols_counter ;
    uint32_t       corrected_fec_words_counter ;
    uint32_t       uncorrected_fec_words_counter ;
    uint16_t       bwmap_alloc_per_tcont_counter [ NGPON_TRAFFIC_TCONT_ID_HIGH + 1] ;
    uint32_t       total_bwmap_alloc_per_tcont_counter [ NGPON_TRAFFIC_TCONT_ID_HIGH + 1] ;
    uint32_t       xgem_frames_per_flow_counter [ NGPON_XGEM_FLOW_ID_HIGH + 1 ] ;
    uint32_t       received_bytes_per_xgem_flow_counter [ NGPON_XGEM_FLOW_ID_HIGH + 1 ] ;
}
XGPON_RXPON_PM_COUNTERS;


/* NGPON TX PLOAM counters  */
typedef struct
{
    uint32_t       tx_ploam_total_counter;
    uint32_t       tx_ploam_regist_counter;
    uint32_t       tx_ploam_ack_counter;
    uint32_t       tx_ploam_key_report_counter;
}
NGPON_PLOAM_MSG_PM_COUNTERS;


/* NGPON tuning request counters  */
typedef struct
{
    uint32_t       requests_for_rx_or_rxtx;
    uint32_t       requests_for_tx_counter;
    uint32_t       requests_failed_to4_exp;
    uint32_t       requests_failed_to5_exp;
}
NGPON_TUNING_REQ_PM_COUNTERS;


/* NGPON PLODS counters  */
typedef struct
{
    uint32_t       total_lods_event_counter;
    uint32_t       lods_restored_operation_twdm_counter;
    uint32_t       lods_restored_protection_twdm_counter;
    uint32_t       lods_handshake_fail_protection_counter;
    uint32_t       lods_reactivations;
}
NGPON_PLODS_PM_COUNTERS;

bdmf_error_t send_message_to_pon_task ( PON_EVENTS_DTE xi_msg_opcode, ...);


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Get NGPON Rx counters                                                    */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_get_all_rx_counters (XGPON_RXPON_PM_COUNTERS * xi_pm_rx);
/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Get NGPON Tx counters                                                    */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_get_all_tx_counters (XGPON_TXPON_PM_COUNTERS * xi_pm_tx);
/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Get Link State and Activation State Machine state                        */
/*                                                                            */
/******************************************************************************/

PON_ERROR_DTE ngpon_assign_user_callback(EVENT_AND_ALARM_CALLBACK_DTE          xi_event_and_alarm_callback,
                                         DBR_PROCESS_RUNNER_DATA_CALLBACK_DTE  xi_dbr_runner_data_callback,
                                         SC_SC_RUNNER_DATA_CALLBACK_DTE        xi_sc_sc_runner_callback,
                                         FLUSH_TCONT_CALLBACK_DTE              xi_flush_tcont_callback);

extern char *g_link_state_names[LINK_STATE_ACTIVE + 1];
extern char *g_link_sub_state_names[CE_LINK_SUB_STATE_STOP + 1] ;
extern char *g_activation_state_names[OPERATION_NGPON2_STATES_NUMBER];
extern char *g_activation_event_names[OPERATION_NGPON2_EVENTS_NUMBER];

#ifdef USE_SHELL
void ngpon_stack_initialize_unit_test_shell ( SHELL_SESSION_DTE * xi_session_ptr, char * xi_base_folder ) ;
#endif

#ifdef __cplusplus
}
#endif


#endif /* NGPON_STACK_H_INCLUDED */
