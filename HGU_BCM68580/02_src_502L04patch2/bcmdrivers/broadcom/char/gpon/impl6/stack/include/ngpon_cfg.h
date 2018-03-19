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

#ifndef NGPON_CFG_H_INCLUDED
#define NGPON_CFG_H_INCLUDED



#include "ngpon_ploam.h"
#include "ngpon_general.h"
#include "ngpon_sm.h"
#include "pon_sm_common.h"



/******************************************************************************/
/* This type defines the software state                                       */
/******************************************************************************/
typedef uint32_t NGPON_SOFTWARE_STATE ;

/* The software is uninitialized */
#define NGPON_SOFTWARE_STATE_UNINITIALIZED   ( ( NGPON_SOFTWARE_STATE ) 0 )
/* The software is initialized */
#define NGPON_SOFTWARE_STATE_INITIALIZED     ( ( NGPON_SOFTWARE_STATE ) 1 )


/******************************************************************************/
/* This type defines the T-CONT id                                            */
/******************************************************************************/
#ifndef NGPON_TCONT_IDENTIFIER_DEFINED
#define NGPON_TCONT_IDENTIFIER_DEFINED

#define NGPON_TRAFFIC_TCONT_ID_LOW       ( ( PON_TCONT_ID_DTE ) 0 )
#define NGPON_TRAFFIC_TCONT_ID_HIGH      ( ( PON_TCONT_ID_DTE ) 39 )
#define NGPON_RX_TRFC_ALLOCID_NUMBER     ( 40 )
#define NGPON_RX_RNGNG_ALLOCID_NUMBER    ( 4 )           /* ??? */
#define NGPON_TRAFFIC_TCONT_ID_IN_RANGE(v)   ( ( (v) >= NGPON_TRAFFIC_TCONT_ID_LOW && (v) <= NGPON_TRAFFIC_TCONT_ID_HIGH )  || ( v )== NGPON_TCONT_ID_NOT_ASSIGNED)


#define NGPON_TCONT_ID_LOW               ( ( PON_TCONT_ID_DTE ) 0 )
#define NGPON_TCONT_ID_HIGH              ( ( PON_TCONT_ID_DTE ) NGPON_RX_TRFC_ALLOCID_NUMBER + NGPON_RX_RNGNG_ALLOCID_NUMBER - 1)
#define NGPON_TCONT_ID_NOT_ASSIGNED      ( ( PON_TCONT_ID_DTE ) 0xFF )
#define NGPON_TCONT_ID_IN_RANGE(v)       ( ( (v) >= NGPON_TCONT_ID_LOW && (v) <= NGPON_TCONT_ID_HIGH ) || ( v )== NGPON_TCONT_ID_NOT_ASSIGNED)


#endif /* NGPON_TCONT_IDENTIFIER_DEFINED */


/******************************************************************************/
/* PON- flow id value type.                                                   */
/******************************************************************************/
#ifndef NGPON_XGEM_FLOW_ID_DEFINED
#define NGPON_XGEM_FLOW_ID_DEFINED

#define XGPON_RX_FLOW_NUMBER          ( 255 )

#ifndef SIM_ENV
typedef uint32_t PON_FLOW_ID_DTE ;
#endif

/* Standard PON-FLOW-ID */
#define NGPON_XGEM_FLOW_ID_LOW        ( ( PON_FLOW_ID_DTE ) 0 )
#define NGPON_XGEM_FLOW_ID_HIGH       ( ( PON_FLOW_ID_DTE ) ( XGPON_RX_FLOW_NUMBER - 1 ) ) /* Value comes from RX registers file */
#define NGPON_XGEM_FLOW_ID_UNASSIGNED ( ( PON_FLOW_ID_DTE ) 0xFFFF )

#define NGPON_XGEM_FLOW_ID_IN_RANGE(v) ( (v) >= NGPON_XGEM_FLOW_ID_LOW && (v) <= NGPON_XGEM_FLOW_ID_HIGH )

#endif


/******************************************************************************/
/* PON- Port id value type.                                                   */
/******************************************************************************/
#ifndef NGPON_XGEM_PORT_ID_DEFINED
#define NGPON_XGEM_PORT_ID_DEFINED

typedef uint32_t NGPON_XGEM_PORT_ID ;

/* Standard PON-PORT-ID */
#define NGPON_XGEM_PORT_ID_LOW             ( ( NGPON_XGEM_PORT_ID ) 0 )
#define NGPON_XGEM_PORT_ID_HIGH            ( ( NGPON_XGEM_PORT_ID ) 65535 ) /* 16 bits */
#define NGPON_XGEM_PORT_ID_UNASSIGNED      ( ( NGPON_XGEM_PORT_ID ) 0xFFFF )

#define NGPON_XGEM_PORT_ID_IN_RANGE(v)       ( ( (v) >= NGPON_XGEM_PORT_ID_LOW && (v) <= NGPON_XGEM_PORT_ID_HIGH ) || ( v ) == NGPON_XGEM_PORT_ID_UNASSIGNED )

/* 1023 is the lowest XGEM allowed according to the standard   */
#define NGPON_XGEM_DATA_PORT_ID_LOW             ( ( NGPON_XGEM_PORT_ID ) 1023 )
/* 0xFFFF=65535 is reserved fr Idle XGEM , 65534 is the highest port for Data  */
#define NGPON_XGEM_DATA_PORT_ID_HIGH            ( ( NGPON_XGEM_PORT_ID ) 65534 )

#define NGPON_XGEM_DATA_PORT_ID_IN_RANGE(v)       ( ( (v) >= NGPON_XGEM_DATA_PORT_ID_LOW && (v) <= NGPON_XGEM_DATA_PORT_ID_HIGH ) || ( v ) == NGPON_XGEM_PORT_ID_UNASSIGNED )


#endif

/******************************************************************************/
/* Encryption ring .                                                           */
/******************************************************************************/
#ifndef NGPON_ENCRYPTION_RING_DEFINED
#define NGPON_ENCRYPTION_RING_DEFINED

typedef uint32_t NGPON_ENCRYPTION_RING ;

#define NGPON_ENCRYPTION_RING_UNICAST_ONU_1     ( ( NGPON_ENCRYPTION_RING ) 1 )
#define NGPON_ENCRYPTION_RING_UNICAST_ONU_2     ( ( NGPON_ENCRYPTION_RING ) 1 )
#define NGPON_ENCRYPTION_RING_MULTICAST         ( ( NGPON_ENCRYPTION_RING ) 2 )
#define NGPON_ENCRYPTION_RING_PLAINTEXT         ( ( NGPON_ENCRYPTION_RING ) 3 )
#define NGPON_ENCRYPTION_RING_NONE              ( ( NGPON_ENCRYPTION_RING ) 0xFF )
#endif

/******************************************************************************/
/* This type defines the Alloc id values.                                     */
/******************************************************************************/
#ifndef NGPON_ALLOC_ID_DEFINED
#define NGPON_ALLOC_ID_DEFINED

#define NGPON_ALLOC_ID_LOW               ( ( PON_ALLOC_ID_DTE ) 0 )
#define NGPON_ALLOC_ID_HIGH              ( ( PON_ALLOC_ID_DTE ) 16383 )/* 14 bits */
#define NGPON_ALLOC_ID_NOT_ASSIGNED      ( ( PON_ALLOC_ID_DTE ) 0xFFFF )

#define NGPON_ALLOC_ID_IN_RANGE(v)       ( ( (v) >= NGPON_ALLOC_ID_LOW && (v) <= NGPON_ALLOC_ID_HIGH ) || ( v ) == NGPON_ALLOC_ID_NOT_ASSIGNED )

#define NGPON_DATA_ALLOC_ID_LOW          ( ( PON_ALLOC_ID_DTE ) 1024 )
#define NGPON_DATA_ALLOC_ID_HIGH         NGPON_ALLOC_ID_HIGH

#define NGPON_DATA_ALLOC_ID_IN_RANGE(v)       ( ( (v) >= NGPON_DATA_ALLOC_ID_LOW && (v) <= NGPON_DATA_ALLOC_ID_HIGH ) || ( v ) == NGPON_ALLOC_ID_NOT_ASSIGNED )

#define NGPON_BCAST_ALLOC_ID_2_5_US_IDX           0 
#define NGPON_BCAST_ALLOC_ID_10_US_IDX            1 
#define NGPON_BCAST_ALLOC_ID_BOTH_IDX             2 

/*
 * Alloc IDs are set for simulation environment
 */
#define NGPON_BCAST_ALLOC_ID_2_5_US_VAL         1023
#define NGPON_BCAST_ALLOC_ID_10_US_VAL          1022
#define NGPON_BCAST_ALLOC_ID_BOTH_VAL           1021

#define NGPON_SN_TCONT_ID_VALUE                 38   /* special ranging tcont */
#define NGPON_RANGING_TCONT_ID_VALUE            38 

#endif



/******************************************************************************/
/* This type define the assign alloc id indication parameters                 */
/******************************************************************************/
typedef struct
{
    /* Alloc id */
    PON_ALLOC_ID_DTE alloc_id ;

    /* Assign/Deassign flag */
    bdmf_boolean assign_flag ;
}
NGPON_ASSIGN_ALLOC_ID_INDICATION_PARAMS ;




#define O1_SUB_STATE_STRINGS_IN_RDPA   4 /* offset of NGPON2 O1 state substate strings in 
                                            bdmf_attr_enum_table_t pon_sub_state_enum_table
                                            in rdpa_gpon.c 
                                          */ 

#define O5_SUB_STATE_STRINGS_IN_RDPA   6  /* offset of NGPON2 O5 state substate strings in 
                                            bdmf_attr_enum_table_t pon_sub_state_enum_table
                                            in rdpa_gpon.c 
                                          */

#define O8_SUB_STATE_STRINGS_IN_RDPA   8  /* offset of NGPON2 O8 state substate strings in 
                                            bdmf_attr_enum_table_t pon_sub_state_enum_table
                                            in rdpa_gpon.c 
                                          */


/******************************************************************************/
/* This type defines the Random delay struct                                  */
/******************************************************************************/
typedef struct
{
    /* Random delay MSB */
    uint32_t random_delay : 12 ;
}
NGPON_RANDOM_DELAY_DTE ;


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
/* This structure holds the global parameters for the GPON stack              */
/******************************************************************************/
typedef struct
{
     /* Base priority */
    uint32_t base_priority ;

    /* High priority */
    uint32_t high_priority ;

    /* Priority skip */
    uint32_t priority_skip ;

    /* Stack size */
    uint32_t stack_size ;

    /* Split ratio */
    uint32_t split_ratio ;

    /* Number of Alloc-IDs */
    uint32_t num_of_alloc_ids ;

    /* Number of port-IDs */
    uint32_t num_of_port_ids ;

    /* Commands queue size */
    uint32_t commands_queue_size ;

    /* Number of concurrent commands */
    uint32_t num_of_concurrent_commands ;
}
NGPON_PARAMETERS_DTE ;




/******************************************************************************/
/* This type defines the ONU serial number                                    */
/******************************************************************************/
typedef struct
{
    uint8_t vendor_id[4];
    uint8_t vssn[4];
}sn_struct;

typedef union
{
    sn_struct sn;
    uint8_t serial_number[SERIAL_NUMBER_FIELD_SIZE];
}
NGPON_SERIAL_NUMBER;


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
/* This type defines the Auto restart TO1 mode                                  */
/******************************************************************************/
typedef uint32_t NGPON_TO1_AUTO_RESTART_MODE_DTE ;
/* Auto restart TO1 enabled */
#define NGPON_TO1_AUTO_RESTART_ON          ( ( NGPON_TO1_AUTO_RESTART_MODE_DTE ) 0 )
/* Auto restart TO1 disabled */
#define NGPON_TO1_AUTO_RESTART_OFF         ( ( NGPON_TO1_AUTO_RESTART_MODE_DTE ) 1 )





/******************************************************************************/
/* This structure holds the PM counters of a link                           */
/******************************************************************************/
typedef struct
{
    /* TBD */
    signed char TBD ;
}
LINK_PM_DTE ;

/******************************************************************************/
/* This structure holds the Front End FIFO (TX) configuration                 */
/******************************************************************************/

typedef struct
{
    uint8_t          packet_descriptor_id;
    PON_FIFO_ID_DTE  txfifo_id;

    uint16_t         packet_descriptor_queue_base_address;
    uint16_t         packet_descriptor_queue_size;
    uint16_t         tx_queue_offset;
    uint16_t         tx_max_queue_size;
}
NGPON_TX_FIFO_CFG ;

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



/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*  Clears run-time configuration: profiles ets                               */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE clear_onu_config ( void ) ;


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*  Set Tx ON/OFF                                                             */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_enable_transmitter ( bdmf_boolean tx_state ) ;


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   This function initialize the XGPON ONU software. It allocates the memory */
/* and the RTOS resources, initializes the internal database and all state    */
/* machines.                                                                  */
/*                                                                            */
/******************************************************************************/
bdmf_error_t ngpon_software_init ( void  ) ;



/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   This function resets the NGPON link (disables and re-enables)            */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE  ngpon_link_reset ( void );

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   This function activates the NGPON link                                   */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE  ngpon_link_activate ( bdmf_boolean xi_initial_state_disable );

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Deactivate the NGPON link                                                */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE  ngpon_link_deactivate ( void );


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Compute MIC over DS PLOAM (for comparison with received)                 */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE  ngpon_generate_mic_for_ds_ploam (uint8_t * xi_ploam, uint8_t *xo_mic, const uint8_t * xi_key ) ;
/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*  Create TCONT ID - Alloc ID association                                    */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_assign_tcont ( PON_TCONT_ID_DTE   xi_tcont_id, PON_ALLOC_ID_DTE   xi_alloc_id ) ;

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*  Clears the tables                                                         */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE clear_default_alloc_id_and_default_xgem_flow_id ( void ) ;


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*  Alloc ID setup                                                            */
/*                                                                            */
/******************************************************************************/
void ngpon_config_alloc_id ( NGPON_ASSIGN_ALLOC_ID_INDICATION_PARAMS assign_alloc_id_struct );


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*  Enable Alloc ID and associate it with TCONT                               */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_enable_alloc_id_and_tcont ( PON_TCONT_ID_DTE xi_tcont_id ,uint32_t xi_alloc_index, PON_ALLOC_ID_DTE xi_alloc_id );

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*  Configure XGEM port                                                       */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_config_xgem_flow (PON_FLOW_ID_DTE  xi_flow_id, NGPON_XGEM_PORT_ID xi_port_id,  
                                    PON_FLOW_PRIORITY_DTE xi_priority, NGPON_ENCRYPTION_RING xi_encrypt,  
                                    bdmf_boolean xi_crc_enable);
/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Enable/Disable XGEM port                                                   */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_control_xgem_flow ( PON_FLOW_ID_DTE xi_flow_id , bdmf_boolean  xi_flow_enable );
/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Disable XGEM port and clean up in the table                                */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_remove_xgem_flow ( PON_FLOW_ID_DTE xi_flow_id ) ;  
/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Remove TCONT from the configuration                                        */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_remove_tcont ( PON_TCONT_ID_DTE xi_tcont_id ) ;  
/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Allocate resources (timers, queues, etc)                                   */
/*                                                                            */
/******************************************************************************/
bdmf_error_t  create_os_resources ( void ) ;
/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Release resources                                                          */
/*                                                                            */
/******************************************************************************/
bdmf_error_t  release_os_resources ( void ) ;

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Configure and activate BW recording                                        */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE  ngpon_start_bw_recording(bdmf_boolean rcd_stop,
    uint32_t record_type, uint32_t  alloc_id);

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Stop BW recording                                                          */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE  ngpon_stop_bw_recording(void) ;


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* read BW recording                                                          */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_read_bw_record(PON_ACCESS *record_data,
    int *const access_read_num);


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Get BW recording results                                                   */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_get_bw_recording_data(NGPON_ACCESS *xi_accssess_array,
    int *const xo_number_of_access_read);


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Get rogue ONU detection parameters                                         */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_get_rogue_onu_detection_params(PON_TX_ROGUE_ONU_PARAMETERS *rogue_onu_params);


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Set rogue ONU detection parameters                                         */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_set_rogue_onu_detection_params(const PON_TX_ROGUE_ONU_PARAMETERS *rogue_onu_parameters);


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Set stack mode (NGPON2 2.5G, NGPON2 10G, XGPON)                            */
/*                                                                            */
/******************************************************************************/
void ngpon_set_stack_mode (PON_MAC_MODE stack_mode) ;

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Set encryption key for multicast traffic                                   */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_set_multicast_encryption_key (uint32_t key_index, uint8_t* encryption_key);

PON_ERROR_DTE ngpon_get_multicast_encryption_key (uint32_t key_index, uint8_t* encryption_key, uint8_t* key_valid);

PON_ERROR_DTE _ngpon_link_activate(bdmf_boolean xi_initial_state_disable);
PON_ERROR_DTE _ngpon_link_deactivate(void);
PON_ERROR_DTE ngpon_configure_onu_serial_number ( PON_SERIAL_NUMBER_DTE xi_serial_number );
PON_ERROR_DTE ngpon_configure_onu_password ( PON_PASSWORD_DTE  xi_onu_password );
PON_ERROR_DTE ngpon_set_gem_block_size ( PON_GEM_BLOCK_SIZE_DTE  xi_gem_block_size );
PON_ERROR_DTE ngpon_dba_sr_process_initialize ( uint32_t xi_sr_reporting_block, uint32_t xi_update_interval );
PON_ERROR_DTE ngpon_dba_sr_process_terminate ( void );
PON_ERROR_DTE ngpon_configure_link_params ( rdpa_gpon_link_cfg_t * const xi_link_default_configuration );
PON_ERROR_DTE ngpon_control_port_id_filter ( PON_FLOW_ID_DTE xi_flow_id, bool  xi_filter_status );
PON_ERROR_DTE ngpon_modify_encryption_port_id_filter(PON_FLOW_ID_DTE xi_flow_id, bool xi_encryption_mode);
PON_ERROR_DTE ngpon_configure_port_id_filter(PON_FLOW_ID_DTE xi_flow_id, PON_PORT_ID_DTE xi_port_id_filter, PON_PORT_ID_DTE xi_port_id_mask,
                                             bool xi_encryption_mode, PON_FLOW_TYPE_DTE xi_flow_type, PON_FLOW_PRIORITY_DTE xi_flow_priority, bool crcEnable);
PON_ERROR_DTE ngpon_configure_tcont(PON_TCONT_ID_DTE xi_tcont_queue_id, PON_ALLOC_ID_DTE xi_alloc_id);
PON_ERROR_DTE _ngpon_remove_tcont ( PON_TCONT_ID_DTE  xi_tcont_id );
PON_ERROR_DTE _ngpon_link_reset ( void );
PON_ERROR_DTE ngpon_mask_rx_pon_interrupts ( void );
PON_ERROR_DTE ngpon_mask_tx_pon_interrupts ( void );
PON_ERROR_DTE ngpon_unmask_tx_pon_interrupts ( void );
PON_ERROR_DTE ngpon_unmask_rx_pon_interrupts ( void );
PON_ERROR_DTE ngpon_tx_set_tcont_to_counter_group_association(uint8_t tcont0, uint8_t tcont1, uint8_t tcont2, uint8_t tcont3);
PON_ERROR_DTE ngpon_tx_set_flush_valid_bit(uint8_t tcont_id, bdmf_boolean flush_enable, bdmf_boolean flush_immediate);
PON_ERROR_DTE ngpon_tx_generate_misc_transmit(uint8_t prodc[16], uint8_t prcl, uint8_t brc, uint8_t prl, uint16_t msstart, uint16_t msstop, bdmf_boolean enable);
PON_ERROR_DTE ngpon_get_link_status(LINK_STATE_DTE * const xo_state, LINK_SUB_STATE_DTE * const xo_sub_state, LINK_OPERATION_STATES_DTE * const xo_operational_state);
PON_ERROR_DTE ngpon_get_link_parameters ( rdpa_gpon_link_cfg_t * const xo_parameters );
PON_ERROR_DTE ngpon_get_onu_id ( uint16_t * const xo_onu_id );
PON_ERROR_DTE ngpon_get_burst_prof (rdpa_gpon_burst_prof_t * const xo_burst_prof, const int index);
PON_ERROR_DTE ngpon_get_overhead_and_length (PON_TX_OVERHEAD_DTE  * const xo_pon_overhead, uint8_t * const xo_pon_overhead_length,
                                             uint8_t * const xo_pon_overhead_repetition_length, uint8_t * const xo_pon_overhead_repetition);
PON_ERROR_DTE ngpon_get_transceiver_power_level(uint8_t * const xo_transceiver_power_level);
PON_ERROR_DTE ngpon_get_equalization_delay ( uint32_t * const xo_equalization_delay );
PON_ERROR_DTE ngpon_get_aes_encryption_key ( AES_KEY_DTE * xi_aes_encryption_key, uint8_t xi_fragment_index );
PON_ERROR_DTE ngpon_get_bip_error_counter( uint32_t * const xo_bip_errors);
PON_ERROR_DTE ngpon_get_rxpon_ploam_counters ( PON_RX_PLOAM_COUNTERS_DTE * const xo_rxpon_ploam_counters );
PON_ERROR_DTE ngpon_get_rx_unkonw_ploam_counter ( uint32_t * const  xo_rx_unknown_counters );
PON_ERROR_DTE ngpon_get_txpon_pm_counters(uint16_t * const xo_illegal_access_counter, uint32_t * const xo_idle_ploam_counter,
                                          uint32_t * const xo_ploam_counter);
PON_ERROR_DTE ngpon_get_port_id_configuration(PON_FLOW_ID_DTE xi_flow_id, bool * const xo_filter_status, PON_PORT_ID_DTE * const xo_port_id_filter,
                                              PON_PORT_ID_DTE * const xo_port_id_mask, bool * const xo_encryption_mode, PON_FLOW_TYPE_DTE * const xo_flow_type,
                                              PON_FLOW_PRIORITY_DTE * const xo_flow_priority, bool * xo_crc_enable);
PON_ERROR_DTE ngpon_get_encryption_error_counter(uint32_t *const xo_rx_encryption_error);
PON_ERROR_DTE ngpon_get_tcont_configuration(PON_TCONT_ID_DTE xi_tcont_id, PON_TCONT_DTE * const xo_tcont_cfg);
PON_ERROR_DTE ngpon_get_tcont_queue_pm_counters(uint32_t xi_fifo_id, uint32_t * const xo_transmitted_idle_counter,
                                                uint32_t * const xo_transmitted_gem_counter, uint32_t * const xo_transmitted_packet_counter,
                                                uint16_t *const xo_requested_dbr_counter, uint16_t * const xo_valid_access_counter);
PON_ERROR_DTE ngpon_get_ploam_crc_error_counter(PON_RXPON_REQUEST_DTE xi_request_type, uint32_t *const xo_ploam_crc_errors);
PON_ERROR_DTE ngpon_rx_get_receiver_status(bdmf_boolean *lof_state, bdmf_boolean *fec_state, bdmf_boolean *lcdg_state);
PON_ERROR_DTE ngpon_rx_get_fec_counters(uint32_t *fec_corrected_bytes, 
    uint32_t *fec_corrected_codewords, uint32_t *fec_uncorrectable_codewords);
PON_ERROR_DTE ngpon_rx_get_hec_counters(PON_RX_HEC_COUNTERS_DTE * const rxpon_hec_counter);
PON_ERROR_DTE ngpon_tx_get_flush_valid_bit(uint8_t *tcont_id, 
    bdmf_boolean *flush_enable, bdmf_boolean *flush_immediate, bdmf_boolean *flush_done);
PON_ERROR_DTE ngpon_get_pon_id_parameters(PON_ID_PARAMETERS *xo_pon_param);
void ngpon_set_pmd_fb_done(uint8_t state);


PON_ERROR_DTE clear_burst_profiles(void);
void clear_onu_config_7(void);
PON_ERROR_DTE clear_all_rx_onu_ids(void);
PON_ERROR_DTE clear_all_tx_onu_ids(void);
PON_ERROR_DTE clear_onu_config_5(void);
PON_ERROR_DTE clear_onu_config_3(void);

PON_ERROR_DTE ngpon_set_txfifo_queue_configuration(NGPON_TX_FIFO_CFG *txfifo_queue_cfg);

PON_ERROR_DTE ngpon_get_txfifo_queue_configuration(uint8_t xi_packet_descriptor_id,
    PON_FIFO_ID_DTE xi_txfifo_id, NGPON_TX_FIFO_CFG *txfifo_queue_cfg);


PON_ERROR_DTE ngpon_get_rxpon_pm_counters(PON_RXPON_PM_COUNTERS * const rxpon_pm_counters);

PON_ERROR_DTE ngpon_get_txpon_pm_counters(uint16_t *const illegal_access_counter,
                                          uint32_t * const idle_ploam_counter, uint32_t * const tx_ploam_counter);

PON_ERROR_DTE ngpon_get_port_id_configuration(PON_FLOW_ID_DTE xi_flow_id, bool * const xo_filter_status, PON_PORT_ID_DTE * const xo_port_id_filter,
                                              PON_PORT_ID_DTE * const xo_port_id_mask, bool * const xo_encryption_mode, PON_FLOW_TYPE_DTE * const xo_flow_type,
                                              PON_FLOW_PRIORITY_DTE * const xo_flow_priority, bool * xo_crc_enable);

PON_ERROR_DTE ngpon_get_encryption_error_counter(uint32_t *const xo_rx_encryption_error);

PON_ERROR_DTE ngpon_get_tcont_configuration(PON_TCONT_ID_DTE xi_tcont_id, PON_TCONT_DTE * const xo_tcont_cfg );

PON_ERROR_DTE ngpon_get_tcont_queue_pm_counters(uint32_t xi_fifo_id, uint32_t * const xo_transmitted_idle_counter,
                                                uint32_t * const xo_transmitted_gem_counter, uint32_t * const xo_transmitted_packet_counter,
                                                uint16_t *const xo_requested_dbr_counter, uint16_t * const xo_valid_access_counter);

PON_ERROR_DTE ngpon_get_ploam_crc_error_counter(PON_RXPON_REQUEST_DTE xi_request_type, uint32_t *const xo_ploam_crc_errors);

PON_ERROR_DTE ngpon_rx_get_receiver_status(bdmf_boolean *lof_state, bdmf_boolean *fec_state, bdmf_boolean *lcdg_state);

PON_ERROR_DTE ngpon_rx_get_fec_counters(uint32_t *fec_corrected_bytes, 
                                        uint32_t *fec_corrected_codewords, uint32_t *fec_uncorrectable_codewords);


PON_ERROR_DTE ngpon_tx_get_flush_valid_bit(uint8_t *tcont_id, bdmf_boolean *flush_enable,
                                           bdmf_boolean *flush_immediate, bdmf_boolean *flush_done);

PON_ERROR_DTE ngpon_get_pon_id_parameters(PON_ID_PARAMETERS *xo_pon_param);

uint32_t ngpon_set_eqd ( uint32_t xi_delay, uint8_t absolute_or_relative, uint8_t sign );

void ngpon_configure_omci_key (uint8_t *omci_ik_p);

PON_ERROR_DTE config_transceiver_dv ( void );

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Check, whether the SFC_INDICATION is enabled - means that                */
/* somebody is expected to handle this interrupt                              */
/*                                                                            */
/******************************************************************************/
bdmf_boolean ngpon_is_sfc_indication_int_set(void) ;

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Configure SYNC_SFC_IND Register and enable corresponding interrupt       */
/* somebody is expected to handle this interrupt                              */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_set_sfc_indication_int(uint32_t sfc_msb, uint32_t sfc_lsb) ;



/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Clean Enable bit in SYNC_SFC_IND Register and disable corresponding      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_clear_sfc_indication_int (void) ;


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Execute NGPON MAC FIFO flush sequence for all active TCONTs              */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE  ngpon_flush_traffic_alloc_table (void);

#ifdef USE_BDMF_SHELL

void ngpon_exit_drv_pon_stack_shell(void);
#endif

PON_ERROR_DTE ngpon_modify_tcont(PON_TCONT_ID_DTE xi_tcont_id,
    PON_ALLOC_ID_DTE xi_new_alloc_id);
extern char * g_link_state_names [ LINK_STATE_ACTIVE + 1 ];
extern char * g_link_sub_state_names [ CE_LINK_SUB_STATE_STOP + 1 ] ;
extern char *g_activation_state_names[OPERATION_NGPON2_STATES_NUMBER];
extern char *g_activation_event_names[OPERATION_NGPON2_EVENTS_NUMBER];

#ifdef USE_SHELL
void ngpon_stack_initialize_unit_test_shell ( SHELL_SESSION_DTE * xi_session_ptr, char * xi_base_folder ) ;
#endif



#endif /* NGPON_CFG_H_INCLUDED */
