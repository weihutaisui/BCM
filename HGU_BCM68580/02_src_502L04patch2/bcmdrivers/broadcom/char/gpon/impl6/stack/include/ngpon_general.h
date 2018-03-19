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
#ifndef __NGPON_GENERAL_H
#define __NGPON_GENERAL_H


#ifdef __cplusplus
extern "C"
{
#endif

#ifdef SIM_ENV
typedef unsigned int bdmf_queue_t ;
#define BDMF_TRUE  ( 1 )
#define BDMF_FALSE ( 0 )
#endif



/****************************************************************************/
/**                                                                        **/
/** XGPON/NGPON ONU General definitions.                                         **/
/**                                                                        **/
/** Title:                                                                 **/ 
/**    XGPON/NGPON software general definitions.                                 **/
/**                                                                        **/
/** Abstract:                                                              **/
/**                                                                        **/
/**                                                                        **/
/****************************************************************************/


/****************************************************************************/
/**                                                                        **/
/** Include files                                                          **/
/**                                                                        **/
/****************************************************************************/
#include "ngpon_all.h"

/****************************************************************************/
/**                                                                        **/
/** Software definitions :                                                 **/
/**                                                                        **/
/****************************************************************************/

#ifdef SIM_ENV
#define WORD_UNMASK      ( 0 ) 

#define WORD_MASK        ( 0xFFFFFFFF ) 
#endif

#define BITS_PER_NIBBLE  ( 4 )
#define BYTES_PER_WORD   ( 4 )
#define BITS_PER_WORD    ( 32 )

#define CLEAR_INTERRUPT   ( 1 )

/******************************************************************************/
/* This type defines the general error codes returned by the various ONUXG    */
/* functions                                                                  */
/******************************************************************************/
#ifndef NGPON_RETURNED_STATUS_DEFINED
#define NGPON_RETURNED_STATUS_DEFINED


  typedef uint32_t  NGPON_ERROR ;

/* No errors or faults */
#define NGPON_NO_ERROR                                       ( ( NGPON_ERROR ) 0 )
/* Rx Module is not disabled */                                                              
#define NGPON_ERROR_RX_NOT_DISABLED                          ( ( NGPON_ERROR ) 1 )
/* Illegal XGEM Frame size */                                                   
#define NGPON_ERROR_ILLEGAL_XGEM_FRAME_SIZE                  ( ( NGPON_ERROR ) 2 )
/* ONU is not disabled */                                                   
#define NGPON_ERROR_ONU_NOT_DISABLED                         ( ( NGPON_ERROR ) 3 )
/* Illegal Alloc id index */                                                   
#define NGPON_ERROR_ILLEGAL_ALLOC_ID_INDEX                   ( ( NGPON_ERROR ) 4 )
/* xgem_flow_not_disabled */                                                   
#define NGPON_ERROR_XGEM_FLOW_NOT_DISABLED                   ( ( NGPON_ERROR ) 5 )
/* xgem_flow_id_out_of_range */                                             
#define NGPON_ERROR_XGEM_FLOW_ID_OUT_OF_RANGE                ( ( NGPON_ERROR ) 6 )
/* encryption_key_num_out_of_range */                                         
#define NGPON_ERROR_ENCRYPTION_KEY_NUM_OUT_OF_RANGE          ( ( NGPON_ERROR ) 7 )
/* encryption_key_type_out_of_range */                                         
#define NGPON_ERROR_ENCRYPTION_KEY_ONU_TYPE_OUT_OF_RANGE     ( ( NGPON_ERROR ) 8 )
/* encryption_key_still_valid */                                         
#define NGPON_ERROR_ENCRYPTION_KEY_STILL_VALID               ( ( NGPON_ERROR ) 9 )
/* onu_id_index_out_of_range */                                         
#define NGPON_ERROR_ONU_ID_INDEX_OUT_OF_RANGE                ( ( NGPON_ERROR ) 10 )
/* TCONT id_out_of_range */                                             
#define NGPON_ERROR_TCONT_ID_OUT_OF_RANGE                    ( ( NGPON_ERROR ) 11 )
/* bw_recorder_not_disabled */                                                   
#define NGPON_ERROR_BW_RECORDER_NOT_DISABLED                 ( ( NGPON_ERROR ) 12 )
/* TCONT has no counter associated */                        
#define NGPON_ERROR_TCONT_HAS_NO_COUNTER_ASSOCIATED          ( ( NGPON_ERROR ) 13 )
/* Illegal_data_setup_pattern_len  */                        
#define NGPON_ERROR_ILLEGAL_DATA_SETUP_PATTERN_LEN           ( ( NGPON_ERROR ) 14 )
/* Illegal_data_hold_pattern_len  */                         
#define NGPON_ERROR_ILLEGAL_DATA_HOLD_PATTERN_LEN            ( ( NGPON_ERROR ) 15 )
/* Illegal_queue id  */                                      
#define NGPON_ERROR_ILLEGAL_QUEUE_ID                         ( ( NGPON_ERROR ) 16 )
/* onu_id out_of_range */                                         
#define NGPON_ERROR_ONU_ID_OUT_OF_RANGE                      ( ( NGPON_ERROR ) 17 )
/* ploam_type out_of_range */                                         
#define NGPON_ERROR_PLOAM_TYPE_OUT_OF_RANGE                  ( ( NGPON_ERROR ) 18 )
/* tx_fifo_group_index out_of_range */                                         
#define NGPON_ERROR_TX_FIFO_GROUP_INDEX_OUT_OF_RANGE         ( ( NGPON_ERROR ) 19 )
/* tx_fifo_queue_index out_of_range */                                         
#define NGPON_ERROR_TX_FIFO_QUEUE_INDEX_OUT_OF_RANGE         ( ( NGPON_ERROR ) 20 )
/* queue_size_is_out_of_range */                                         
#define NGPON_ERROR_QUEUE_SIZE_IS_OUT_OF_RANGE               ( ( NGPON_ERROR ) 21 )
/* queue_base_address_is_not_aligned */                                         
#define NGPON_ERROR_QUEUE_BASE_ADDRESS_IS_NOT_ALIGNED        ( ( NGPON_ERROR ) 22 )
/* queue size_is_not_aligned */                                         
#define NGPON_ERROR_QUEUE_SIZE_IS_NOT_ALIGNED                ( ( NGPON_ERROR ) 23 )
/* total_queues_size_is_out_of_range */                                         
#define NGPON_ERROR_TOTAL_QUEUES_SIZE_IS_OUT_OF_RANGE        ( ( NGPON_ERROR ) 24 )
/* Tx Module is not disabled */                                                              
#define NGPON_ERROR_TX_NOT_DISABLED                          ( ( NGPON_ERROR ) 25 )
/* burst_profile_id_is_out_of_range */                                                              
#define NGPON_ERROR_BURST_PROFILE_ID_IS_OUT_OF_RANGE         ( ( NGPON_ERROR ) 26 )
/* bw_record_index  is_out_of_range */                                                              
#define NGPON_ERROR_BW_RECORD_INDEX_IS_OUT_OF_RANGE          ( ( NGPON_ERROR ) 27 )
/* The unit ID is not defined */                             
#define NGPON_ERROR_UNKNOWN_UNIT_ID                          ( ( NGPON_ERROR ) 28 )
/* the GPIO ID is out of range */                            
#define NGPON_ERROR_GPIO_ID_OUT_OF_RANGE                     ( ( NGPON_ERROR ) 29 )
/* the GPIO value is out of range */                         
#define NGPON_ERROR_GPIO_VALUE_OUT_OF_RANGE                  ( ( NGPON_ERROR ) 30 )
/* the ethernet interface type is unknown */                 
#define NGPON_ERROR_UNKNOWN_ETHER_INTERFACE_TYPE             ( ( NGPON_ERROR ) 31 )
/* the PON serdes type is unknown */                         
#define NGPON_ERROR_UNKNOWN_PON_SERDES_TYPE                  ( ( NGPON_ERROR ) 32 )
/* unknown_intterupt_register */                             
#define NGPON_ERROR_UNKNOWN_INTERRUPT_REGISTER               ( ( NGPON_ERROR ) 33 )
/* interrupt_register_is_read_only */                        
#define NGPON_ERROR_INTERRUPT_REGISTER_IS_READ_ONLY          ( ( NGPON_ERROR ) 34 )
/* clear_register_is_not_allowed */                          
#define NGPON_ERROR_CLEAR_REGISTER_IS_NOT_ALLOWED            ( ( NGPON_ERROR ) 35 )
/* the pointer passed is null */                             
#define NGPON_ERROR_NULL_POINTER                             ( ( NGPON_ERROR ) 36 )
/* the flow id is out of range */                            
#define NGPON_ERROR_FLOW_ID_OUT_OF_RANGE                     ( ( NGPON_ERROR ) 37 )
/* Write MDIO fail */
#define NGPON_ERROR_WRITE_MDIO_FAIL                          ( ( NGPON_ERROR ) 38 )
/* Read MDIO fail */                                         
#define NGPON_ERROR_READ_MDIO_FAIL                           ( ( NGPON_ERROR ) 39 )
/* Ranging_word_delay_is_out_of_range*/                      
#define NGPON_ERROR_TX_RANGING_WORD_DELAY_IS_OUT_OF_RANGE    ( ( NGPON_ERROR ) 40 )
/* Ranging_framw_delay_is_out_of_range*/                                         
#define NGPON_ERROR_TX_RANGING_FRAME_DELAY_IS_OUT_OF_RANGE   ( ( NGPON_ERROR ) 41 )
/* Ranging_bit_delay_is_out_of_range*/                                           
#define NGPON_ERROR_TX_RANGING_BIT_DELAY_IS_OUT_OF_RANGE     ( ( NGPON_ERROR ) 42 )
/* TX_timeline_offset_is_out_of_range*/                                          
#define NGPON_ERROR_TX_TIMELINE_OFFSET_IS_OUT_OF_RANGE       ( ( NGPON_ERROR ) 43 )
/* TX_preamble_length is_out_of_range*/                                          
#define NGPON_ERROR_TX_PREAMBLE_LEN_IS_OUT_OF_RANGE          ( ( NGPON_ERROR ) 44 )
/* TX_preamble_total length is_out_of_range*/                                    
#define NGPON_ERROR_TX_PREAMBLE_TOTLA_LEN_IS_OUT_OF_RANGE    ( ( NGPON_ERROR ) 45 )
/* TX_preamble_repeat is_out_of_range*/                                          
#define NGPON_ERROR_TX_PREAMBLE_REPEAT_IS_OUT_OF_RANGE       ( ( NGPON_ERROR ) 46 )
/* TX_Delimiter_length is_out_of_range*/                                         
#define NGPON_ERROR_TX_DELIMITER_LEN_IS_OUT_OF_RANGE         ( ( NGPON_ERROR ) 47 )
/* error originate by  the OS */                                                
#define NGPON_ERROR_OS_ERROR                                 ( ( NGPON_ERROR ) 48 )
/* MIC ERROR */                                         
#define NGPON_ERROR_MIC_ERROR                                ( ( NGPON_ERROR ) 49 )
/* ILLEGEL_PACKET_SIZE */                                         
#define NGPON_ERROR_ILLEGEL_PACKET_SIZE                      ( ( NGPON_ERROR ) 50 )
/* Decryption error */                                         
#define NGPON_ERROR_DECRYPTION_ERROR                         ( ( NGPON_ERROR ) 51 )
/* TX_PLOAM_BUFFER_IS_FULL */                                         
#define NGPON_ERROR_TX_PLOAM_BUFFER_IS_FULL                  ( ( NGPON_ERROR ) 52 )
/* TCONT_IS_ALREADY_CONFIGURED */                                         
#define NGPON_ERROR_TCONT_IS_ALREADY_CONFIGURED              ( ( NGPON_ERROR ) 53 )
/* ALLOC_ID_IS_ALREADY_CONFIGURED */                                         
#define NGPON_ERROR_ALLOC_ID_IS_ALREADY_CONFIGURED           ( ( NGPON_ERROR ) 54 )
/* XGEM_FLOW_ID_IS_ALREADY_CONFIGURED */                                         
#define NGPON_ERROR_XGEM_FLOW_ID_IS_ALREADY_CONFIGURED       ( ( NGPON_ERROR ) 55 )
/* XGEM_PORT_IS_ALREADY_CONFIGURED */                                         
#define NGPON_ERROR_XGEM_PORT_ID_IS_ALREADY_CONFIGURED       ( ( NGPON_ERROR ) 56 )
/* ALLOC_ALREADY_ASSIGN_TO_ANOTHER_TCONT */                                         
#define NGPON_ERROR_ALLOC_ALREADY_ASSIGN_TO_ANOTHER_TCONT    ( ( NGPON_ERROR ) 57 )
/* INVALID_STATE */                                                              
#define NGPON_ERROR_INVALID_STATE                            ( ( NGPON_ERROR ) 58 )
/* TCONT_TABLE_IS_FULL */                                                        
#define NGPON_ERROR_TX_TCONT_TABLE_IS_FULL                   ( ( NGPON_ERROR ) 59 )
/* alloc_id_out_of_range */                                             
#define NGPON_ERROR_ALLOC_ID_OUT_OF_RANGE                    ( ( NGPON_ERROR ) 60 )
/* XGEM_PORT_ID_out_of_range */                                             
#define NGPON_ERROR_XGEM_PORT_ID_OUT_OF_RANGE                ( ( NGPON_ERROR ) 61 )
/* FLOW_ID_IS_NOT_CONFIGURED */                                             
#define NGPON_ERROR_FLOW_ID_IS_NOT_CONFIGURED                ( ( NGPON_ERROR ) 62 )
/* TCONT_ID_IS_NOT_CONFIGURED */                                             
#define NGPON_ERROR_TCONT_ID_IS_NOT_CONFIGURED               ( ( NGPON_ERROR ) 63 )


/* the software is not initialized */
#define NGPON_ERROR_SOFTWARE_NOT_INITIALIZED                 ( ( NGPON_ERROR ) 100 )
/* the software was already initialized */
#define NGPON_ERROR_SOFTWARE_ALREADY_INITIALIZED             ( ( NGPON_ERROR ) 101 )
/* the device was already initialized */
#define NGPON_ERROR_DEVICE_ALREAD_INITIALIZED                ( ( NGPON_ERROR ) 102 )
/* the device version is not for ONU */
#define NGPON_ERROR_DEVICE_VERSION_IS_NOT_ONU                ( ( NGPON_ERROR ) 103 )
/* the PLL250 is not locked */
#define NGPON_ERROR_DEVICE_PLL_250_NOT_LOCKED                ( ( NGPON_ERROR ) 104 )
/* the PLL78 is not locked */
#define NGPON_ERROR_DEVICE_PLL_78_NOT_LOCKED                 ( ( NGPON_ERROR ) 105 )
/* the Link is not initialized */
#define NGPON_ERROR_LINK_NOT_INITIALIZED                     ( ( NGPON_ERROR ) 106 )
/* the Link is already active */
#define NGPON_ERROR_LINK_IS_ALREADY_ACTIVE                   ( ( NGPON_ERROR ) 107 )
/* the Link is not inactive */
#define NGPON_ERROR_LINK_IS_NOT_INACTIVE                     ( ( NGPON_ERROR ) 108 )
/* the Link is not active */
#define NGPON_ERROR_LINK_IS_NOT_ACTIVE                       ( ( NGPON_ERROR ) 109 )
/* led_index_is_illegal */
#define NGPON_ERROR_LED_INDEX_IS_ILLEGAL                     ( ( NGPON_ERROR ) 110 )


/* TM US TCONT descriptor is out of range */                      
#define NGPON_ERROR_TM_US_TCONT_DES_IS_OUT_OF_RANGE          ( ( NGPON_ERROR ) 200 )
/* TM US TCONT queue is out of range */                         
#define NGPON_ERROR_TM_US_QUEUE_DES_IS_OUT_OF_RANGE          ( ( NGPON_ERROR ) 201 )
/* TM US TCONT buffer descriptor is out of range */                          
#define NGPON_ERROR_TM_US_BD_IS_OUT_OF_RANGE                 ( ( NGPON_ERROR ) 202 )
/* the tm ds egress queue is out of range */                 
#define NGPON_ERROR_TM_DS_EGRESS_Q_IDX_OUT_OF_RANGE          ( ( NGPON_ERROR ) 203 )
/* the tm us buffer pointer is out of range */               
#define NGPON_ERROR_TM_US_DBUF_BUFFER_IS_OUT_OF_RANGE        ( ( NGPON_ERROR ) 204 )
/* the tm us dbuff word pointer is out of range */           
#define NGPON_ERROR_TM_US_DBUF_WORD_IS_OUT_OF_RANGE          ( ( NGPON_ERROR ) 205 )
/* the tm us lookup table pointer is out of range */         
#define NGPON_ERROR_TM_US_LOOKUP_TABLE_ID_IS_OUT_OF_RANGE    ( ( NGPON_ERROR ) 206 )
/* the tm DS queue can only be read by the HW */             
#define NGPON_ERROR_TM_DS_QUEUE_READ_NOT_ALLOWED             ( ( NGPON_ERROR ) 207 )
/* the tm DS queue is empty*/                                
#define NGPON_ERROR_TM_DS_QUEUE_IS_EMPTY                     ( ( NGPON_ERROR ) 208 )
/* There are no free DB in the TM US*/                       
#define NGPON_ERROR_TM_US_NO_FREE_BD                         ( ( NGPON_ERROR ) 209 )
/* the CPU should not write to this queue */                                         
#define NGPON_ERROR_TM_US_QUEUE_WRITE_NOT_ALLOWED            ( ( NGPON_ERROR ) 210 )
/* TM DS queue write not allowed */                                         
#define NGPON_ERROR_TM_US_TCONT_IS_NOT_DISABLED              ( ( NGPON_ERROR ) 211 )
/* TM congestion assert threshold is_out_of_range*/          
#define NGPON_ERROR_TM_CONGESTION_ASSERT_TH_IS_OUT_OF_RANGE  ( ( NGPON_ERROR ) 212 )
/* TM congestion negate threshold is_out_of_range*/          
#define NGPON_ERROR_TM_CONGESTION_NEGATE_TH_IS_OUT_OF_RANGE  ( ( NGPON_ERROR ) 213 )
/* TM DS queue is too full */                                         
#define NGPON_ERROR_TM_DS_QUEUE_NOT_ENOUGH_FREE_SPACE        ( ( NGPON_ERROR ) 214 )
/* TM DS queue write not allowed */                                         
#define NGPON_ERROR_TM_DS_QUEUE_WRITE_NOT_ALLOWED            ( ( NGPON_ERROR ) 215 )
/* TM US command register is not ready */                                         
#define NGPON_ERROR_TM_US_COMMAND_REGISTER_NOT_READY         ( ( NGPON_ERROR ) 216 )
/* tm_flow_is_already_configured */                                         
#define NGPON_ERROR_TM_FLOW_IS_ALREADY_CONFIGURED            ( ( NGPON_ERROR ) 217 )
/* tm_flow_is_not_configured*/                                         
#define NGPON_ERROR_TM_FLOW_IS_NOT_CONFIGURED                ( ( NGPON_ERROR ) 218 )
/* burst_profile_for_another_line_rate */                                                              
#define NGPON_ERROR_INCOMPATIBLE_BURST_PROFILE               ( ( NGPON_ERROR ) 219 )


#endif

/******************************************************************************/
/* General definitions                                                        */
/******************************************************************************/

/* HW Interrupts registers indexes */
typedef enum
{
    REG_INDEX_ISR,
    REG_INDEX_ISM,
    REG_INDEX_IER,
    REG_INDEX_ITR
} 
INTERRUPT_REG_INDEX ;



/******************************************************************************/
/* ONU id value type.                                                   */
/******************************************************************************/
#ifndef NGPON_UNICAST_ONU_ID_DEFINED
#define NGPON_UNICAST_ONU_ID_DEFINED

typedef uint32_t NGPON_ONU_ID ;

#define NGPON_UNICAST_ONU_ID_LOW          ( ( NGPON_ONU_ID ) 0 )
#define NGPON_UNICAST_ONU_ID_HIGH         ( ( NGPON_ONU_ID ) 1022 )
#define NGPON_UNICAST_ONU_ID_IN_RANGE(v) ( (v) >= NGPON_UNICAST_ONU_ID_LOW && (v) <= NGPON_UNICAST_ONU_ID_HIGH )

#define NGPON_ONU_ID_LOW          ( ( NGPON_ONU_ID ) 0 )
#define NGPON_ONU_ID_HIGH         ( ( NGPON_ONU_ID ) 1023 ) 
#define NGPON_ONU_ID_IN_RANGE(v) ( (v) >= NGPON_ONU_ID_LOW && (v) <= NGPON_ONU_ID_HIGH )

  //#define BROADCAST_ONU_ID         ( 1023 )

#endif




/******************************************************************************/
/* PLOAM buffer index.                                                                 */
/******************************************************************************/
#ifndef NGPON_PLOAM_TYPE_DEFINED
#define NGPON_PLOAM_TYPE_DEFINED

typedef uint32_t NGPON_PLOAM_TYPE ;

/* PLOAM_TYPE_NORMAL  */
#define NGPON_PLOAM_TYPE_NORMAL         ( ( NGPON_PLOAM_TYPE ) 0 )
/* PLOAM_TYPE_REGISTRATION  */
#define NGPON_PLOAM_TYPE_REGISTRATION   ( ( NGPON_PLOAM_TYPE ) 1 )
/* PLOAM_TYPE_IDLE     */
#define NGPON_PLOAM_TYPE_IDLE_OR_SN     ( ( NGPON_PLOAM_TYPE ) 2 )

#define NGPON_PLOAM_TYPE_LOW    ( NGPON_PLOAM_TYPE_NORMAL )
#define NGPON_PLOAM_TYPE_HIGH   ( NGPON_PLOAM_TYPE_IDLE_OR_SN )

#define NGPON_PLOAM_TYPE_IN_RANGE(v) ( (v) >= NGPON_PLOAM_TYPE_LOW && (v) <= NGPON_PLOAM_TYPE_HIGH )

#endif /* ngpon_PLOAM_TYPE_DEFINED */




/******************************************************************************/
/* TX fifo queue index.                                                                 */
/******************************************************************************/
#ifndef NGPON_TX_FIFO_QUEUE_INDEX_DEFINED
#define NGPON_TX_FIFO_QUEUE_INDEX_DEFINED

typedef uint32_t NGPON_TX_FIFO_QUEUE_INDEX ;

/* TX_FIFO_QUEUE_INDEX 0  */
#define NGPON_TX_FIFO_QUEUE_INDEX_0   ( ( NGPON_TX_FIFO_QUEUE_INDEX ) 0 )
/* TX_FIFO_QUEUE_INDEX 1  */
#define NGPON_TX_FIFO_QUEUE_INDEX_1   ( ( NGPON_TX_FIFO_QUEUE_INDEX ) 1 )
/* TX_FIFO_QUEUE_INDEX 2  */
#define NGPON_TX_FIFO_QUEUE_INDEX_2   ( ( NGPON_TX_FIFO_QUEUE_INDEX ) 2 )
/* TX_FIFO_QUEUE_INDEX 3  */
#define NGPON_TX_FIFO_QUEUE_INDEX_3   ( ( NGPON_TX_FIFO_QUEUE_INDEX ) 3 )
/* TX_FIFO_QUEUE_INDEX 4  */
#define NGPON_TX_FIFO_QUEUE_INDEX_4   ( ( NGPON_TX_FIFO_QUEUE_INDEX ) 4 )
/* TX_FIFO_QUEUE_INDEX 5  */
#define NGPON_TX_FIFO_QUEUE_INDEX_5   ( ( NGPON_TX_FIFO_QUEUE_INDEX ) 5 )
/* TX_FIFO_QUEUE_INDEX 6  */
#define NGPON_TX_FIFO_QUEUE_INDEX_6   ( ( NGPON_TX_FIFO_QUEUE_INDEX ) 6 )
/* TX_FIFO_QUEUE_INDEX 7  */
#define NGPON_TX_FIFO_QUEUE_INDEX_7   ( ( NGPON_TX_FIFO_QUEUE_INDEX ) 7 )


#define NGPON_TX_FIFO_QUEUE_INDEX_LOW          ( NGPON_TX_FIFO_QUEUE_INDEX_0 )
#define NGPON_TX_FIFO_QUEUE_INDEX_HIGH         ( NGPON_TX_FIFO_QUEUE_INDEX_7 )

#define NGPON_TX_FIFO_QUEUE_INDEX_IN_RANGE(v) ( (v) >= NGPON_TX_FIFO_QUEUE_INDEX_LOW && (v) <= NGPON_TX_FIFO_QUEUE_INDEX_HIGH )

#endif /* ngpon_TX_FIFO_QUEUE_INDEX_DEFINED */


/******************************************************************************/
/* DBR source.                                                                 */
/******************************************************************************/
#ifndef NGPON_DBR_SRC_DEFINED
#define NGPON_DBR_SRC_DEFINED

typedef uint32_t NGPON_DBR_SRC ;

/* DBR_SRC_HW   */
#define NGPON_DBR_SRC_HW   ( ( NGPON_DBR_SRC ) 0 )
/* ODBR_SRC_SW   */
#define NGPON_DBR_SRC_SW   ( ( NGPON_DBR_SRC ) 1 )

#define NGPON_DBR_SRC_LOW     ( NGPON_DBR_SRC_HW )
#define NGPON_DBR_SRC_HIGH    ( NGPON_DBR_SRC_SW )

#define NGPON_DBR_SRC_IN_RANGE(v) ( (v) >= NGPON_DBR_SRC_LOW && (v) <= NGPON_DBR_SRC_HIGH )

#endif /* NGPON_DBR_SRC_DEFINED */








/******************************************************************************/
/* This type defines the MIC key structure                                    */
/******************************************************************************/

#define NGPON_KEY_PARAMS     ( 4 )

/* Index 0 is for the LSB key bytes */
/* Index 3 is for the MSB key bytes */
typedef struct
{
    uint32_t key [ NGPON_KEY_PARAMS ] ;
}
NGPON_KEY ;

/******************************************************************************/
/* This type defines the MIC accelerator key types                            */
/******************************************************************************/

typedef enum
{
    TM_MIC_OMCI_IK,      
    TM_MIC_K1,              
    TM_MIC_K2              

} NGPON_TM_MIC_KEY_TYPE ;

/******************************************************************************/
/* Encryption key number .                                                           */
/******************************************************************************/
#ifndef NGPON_ENCRYP_DATA_KEY_NUM_DEFINED
#define NGPON_ENCRYP_DATA_KEY_NUM_DEFINED

typedef uint32_t NGPON_ENCRYP_DATA_KEY_NUM ;

#define NGPON_ENCRYP_DATA_KEY_NUM_KEY_1_UNICAST_ONU_1   ( ( NGPON_ENCRYP_DATA_KEY_NUM ) 0 )
#define NGPON_ENCRYP_DATA_KEY_NUM_KEY_2_UNICAST_ONU_1   ( ( NGPON_ENCRYP_DATA_KEY_NUM ) 1 )
#define NGPON_ENCRYP_DATA_KEY_NUM_KEY_1_UNICAST_ONU_2   ( ( NGPON_ENCRYP_DATA_KEY_NUM ) 2 )
#define NGPON_ENCRYP_DATA_KEY_NUM_KEY_2_UNICAST_ONU_2   ( ( NGPON_ENCRYP_DATA_KEY_NUM ) 3 )
#define NGPON_ENCRYP_DATA_KEY_NUM_KEY_1_MULTICAST       ( ( NGPON_ENCRYP_DATA_KEY_NUM ) 0 )
#define NGPON_ENCRYP_DATA_KEY_NUM_KEY_2_MULTICAST       ( ( NGPON_ENCRYP_DATA_KEY_NUM ) 1 )

#define NGPON_ENCRYP_DATA_KEY_NUM_LOW             ( NGPON_ENCRYP_DATA_KEY_NUM_KEY_1_UNICAST_ONU_1 )
#define NGPON_ENCRYP_DATA_KEY_NUM_HIGH            ( NGPON_ENCRYP_DATA_KEY_NUM_KEY_2_MULTICAST )
#define NGPON_ENCRYP_DATA_KEY_NUM_IN_RANGE(v)    ( (v) >= NGPON_ENCRYP_DATA_KEY_NUM_LOW && (v) <= NGPON_ENCRYP_DATA_KEY_NUM_HIGH )    


#define NGPON_UNICAST_ENCRYPTION_KEY_NUM_LOW             ( NGPON_ENCRYP_DATA_KEY_NUM_KEY_1_UNICAST_ONU_1 )
#define NGPON_UNICAAT_ENCRYPTION_KEY_NUM_HIGH            ( NGPON_ENCRYP_DATA_KEY_NUM_KEY_2_UNICAST_ONU_2 )
#define NGPON_UNICAST_ENCRYPTION_KEY_NUM_IN_RANGE(v)    ( (v) >= NGPON_UNICAST_ENCRYPTION_KEY_NUM_LOW && (v) <= NGPON_UNICAAT_ENCRYPTION_KEY_NUM_HIGH )    

#endif







/******************************************************************************/
/*  PLOAM key load status                                     */
/******************************************************************************/
#ifndef NGPON_PLOAM_KEY_LOAD_STATUS_DEFINED
#define NGPON_PLOAM_KEY_LOAD_STATUS_DEFINED

typedef uint32_t NGPON_PLOAM_KEY_LOAD_STATUS ;

#define NGPON_PLOAM_KEY_LOAD_STATUS_LOADED         ( ( NGPON_PLOAM_KEY_LOAD_STATUS ) 0 )
#define NGPON_PLOAM_KEY_LOAD_STATUS_NEED_TO_LOAD   ( ( NGPON_PLOAM_KEY_LOAD_STATUS ) 1 )

#endif /* NGPON_PLOAM_KEY_LOAD_STATUS_DEFINED */



/******************************************************************************/
/* This type defines the T-CONT queue                                         */
/******************************************************************************/
#ifndef NGPON_TCONT_QUEUE_ID_DEFINED
#define NGPON_TCONT_QUEUE_ID_DEFINED

typedef uint32_t NGPON_TCONT_QUEUE_ID ;

#define NGPON_TCONT_QUEUE_ID_LOW               ( ( NGPON_TCONT_QUEUE_ID ) 0 )
#define NGPON_TCONT_QUEUE_ID_HIGH              ( ( NGPON_TCONT_QUEUE_ID ) 7 )

#define NGPON_TCONT_QUEUE_ID_IN_RANGE(v)       ( (v) >= NGPON_TCONT_QUEUE_ID_LOW && (v) <= NGPON_TCONT_QUEUE_ID_HIGH )

#endif /* NGPON_TCONT_QUEUE_ID_DEFINED */






/******************************************************************************/
/* BW recording mode                                                          */
/******************************************************************************/
#ifndef NGPON_BW_RECORD_MODE_DEFINED
#define NGPON_BW_RECORD_MODE_DEFINED

typedef uint32_t NGPON_BW_RECORD_MODE;

#define NGPON_BW_RECORD_MODE_ALL             ((NGPON_BW_RECORD_MODE) 0)
#define NGPON_BW_RECORD_MODE_SINGLE_SPECIFIC ((NGPON_BW_RECORD_MODE) 2)

#define NGPON_BW_RECORD_MODE_IN_RANGE(v) (v == NGPON_BW_RECORD_MODE_ALL || v == NGPON_BW_RECORD_MODE_SINGLE_SPECIFIC)

#endif


/******************************************************************************/
/* BW record index .                                                           */
/******************************************************************************/
#ifndef NGPON_BW_RECORD_INDEX_DEFINED
#define NGPON_BW_RECORD_INDEX_DEFINED

typedef uint32_t NGPON_BW_RECORD_INDEX ;

#define NGPON_BW_RECORD_INDEX_LOW               ( ( NGPON_BW_RECORD_INDEX ) 0 )
#define NGPON_BW_RECORD_INDEX_HIGH              ( ( NGPON_BW_RECORD_INDEX ) 511 )

#define NGPON_BW_RECORD_INDEX_IN_RANGE(v)       ( (v) >= NGPON_BW_RECORD_INDEX_LOW && (v) <= NGPON_BW_RECORD_INDEX_HIGH )

#endif/* NGPON_BW_RECORD_INDEX_DEFINED */

/******************************************************************************/
/* DV setup pattern source                                                    */
/******************************************************************************/
#ifndef NGPON_DV_SETUP_PATTERN_SRC_DEFINED
#define NGPON_DV_SETUP_PATTERN_SRC_DEFINED

typedef uint32_t NGPON_DV_SETUP_PATTERN_SRC ;

#define NGPON_DV_SETUP_PATTERN_SRC_DV_CONIF          ( ( NGPON_DV_SETUP_PATTERN_SRC ) 0 )
#define NGPON_DV_SETUP_PATTERN_SRC_DV_SETUP_PAT_REG  ( ( NGPON_DV_SETUP_PATTERN_SRC ) 1 )

#endif/* NGPON_DV_SETUP_PATTERN_SRC_DEFINED */

/******************************************************************************/
/* DV hold pattern source                                                    */
/******************************************************************************/
#ifndef NGPON_DV_HOLD_PATTERN_SRC_DEFINED
#define NGPON_DV_HOLD_PATTERN_SRC_DEFINED

typedef uint32_t NGPON_DV_HOLD_PATTERN_SRC ;

#define NGPON_DV_HOLD_PATTERN_SRC_DV_CONIF         ( ( NGPON_DV_HOLD_PATTERN_SRC ) 0 )
#define NGPON_DV_HOLD_PATTERN_SRC_DV_HOLD_PAT_REG  ( ( NGPON_DV_HOLD_PATTERN_SRC ) 1 )

#endif/* NGPON_DV_HOLD_PATTERN_SRC_DEFINED */


/******************************************************************************/
/* Data pattern type                                                          */
/******************************************************************************/
#ifndef NGPON_DATA_PATTERN_TYPE_DEFINED
#define NGPON_DATA_PATTERN_TYPE_DEFINED

typedef uint32_t NGPON_DATA_PATTERN_TYPE ;

#define NGPON_DATA_PATTERN_TYPE_DATA_PATTERN_REG    ( ( NGPON_DATA_PATTERN_TYPE ) 0 )
#define NGPON_DATA_PATTERN_TYPE_PRBS_GENERATOR      ( ( NGPON_XG_DATA_PATTERN_TYPE ) 1 )

#endif/* NGPON_DATA_PATTERN_TYPE_DEFINED */








/******************************************************************************/
/* Default stack size                                                         */
/******************************************************************************/
#define DEFAULT_STACK_SIZE                  ( 0x4000 )








#define NGPON_XGPON_FRAME_LEN_OFFSET                ( 24 )
#define NGPON_XGPON_FRAME_LEN_BLOCK                 ( 248 )
#define NGPON_MAX_ALLOC_AT_ENABLE1_REGISTER         ( 32 )
#define NGPON_MAX_NUM_OF_RANGING_ALLOC_IDS          ( 4 )
#define NGPON_NUM_OF_TCONT_AT_ALLOC_INDEX_ARRAY     ( 4 )
#define NGPON_NUM_OF_CONFIGURABLE_COUNTERS          ( 4 )
#define NGPON_NUM_OF_TCONT_WITH_PREDIFEIND_COUNTERS ( 8 )
#define NGPON_NUM_OF_WORDS_AT_ENCRYPTION_KEY        ( 4 )
#define NGPON_PLOAM_LEN_FOR_MIC_CALACULATION        ( 41 ) /* PLOAM len for mic calculation */
#define NGPON_PLOAM_LEN_IN_BYTES_NO_MIC             ( 40 ) /* PLOAM len in bytes w/o MIC */
#define NGPON_PLOAM_LEN_IN_BYTES_WITH_MIC           ( 48 ) /* PLOAM len in bytes including MIC */
#define NGPON_PLOAM_LEN_IN_BYTES_WITH_SPC_AND_MIC   ( 56 ) /* PLOAM len in bytes including SPC and MIC */
#define NGPON_MAX_CPU_PACKET_SIZE                   ( 2048 )


#define OMCI_MIC_SIZE                               ( 4 )
#define OMCI_MAX_SIZE_WITH_NO_MIC                   ( NGPON_MAX_CPU_PACKET_SIZE - OMCI_MIC_SIZE )
#define OMCI_MIN_SIZE_WITH_NO_MIC                   ( BL_MIN_OMCI_PACKET_SIZE_WITHOUT_MIC )
#define NGPON_OMCI_DIR_DS                           1
#define NGPON_OMCI_DIR_US                           2



#define NGPON_DEFAULT_PLOAM_KEY_WORD_1               ( 0x55555555 )
#define NGPON_DEFAULT_PLOAM_KEY_WORD_2               ( 0x55555555 )
#define NGPON_DEFAULT_PLOAM_KEY_WORD_3               ( 0x55555555 )
#define NGPON_DEFAULT_PLOAM_KEY_WORD_4               ( 0x55555555 )

#define NGPON_DEFAULT_CMAC_K2_KEY_WORD_1             ( 0x212338C7 )
#define NGPON_DEFAULT_CMAC_K2_KEY_WORD_2             ( 0xB8CDF663 )
#define NGPON_DEFAULT_CMAC_K2_KEY_WORD_3             ( 0xF5EC0C22 )
#define NGPON_DEFAULT_CMAC_K2_KEY_WORD_4             ( 0x6F2B1673 )


#define NGPON_DEFAULT_KEY_SIZE                       ( 16 )
#define NGPON_CMAC_DATA_IN_MAX_SIZE                  ( 70 ) /* TBD */

#define NGPON_DEFAULT_ALLOC_ID_INDEX                39

#define NGPON_DEFAULT_XGEM_FLOW                     ( 0 )
#define NGPON_DEFAULT_TCONT_ID                      ( NGPON_TRAFFIC_TCONT_ID_HIGH )
#define NGPON_SN_ALLOC_ID_INDEX                     ( 0 )
#define NGPON_DUMMY_TCONT_ID_VALUE                  ( NGPON_TRAFFIC_TCONT_ID_HIGH ) 
#define NGPON_SN_TX_TCONT_ID_VALUE                  (38)
#define NGPON_SN_ALLOC_ID                           ( 0x3FF )
#define NGPON_DEFAULT_RX_ONU_ID_VALUE               ( 0 )
#define NGPON_DEFAULT_TX_ONU_INDEX_0_ONU_ID_VALUE   ( 0x60 )   /* For Sim */
#define NGPON_DEFAULT_TX_ONU_INDEX_1_ONU_ID_VALUE   ( 0x224 )  /* For Sim */
#define NGPON_DEFAULT_TX_ONU_INDEX_2_ONU_ID_VALUE   ( 0x14C )  /* For Sim */

#define NGPON_EXP_PSYNC_MS                          ( 0xc5e51840 )
#define NGPON_EXP_PSYNC_LS                          ( 0xfd59bb49 )
#define NGPON_PON_ID_CONSIST_MASK_MSB               ( 0x5EA1F )
#define NGPON_PON_ID_CONSIST_MASK_LSB               ( 0x59CC7FAC )
#define NGPON_DESIRED_PHASE_DIFF                    ( 0x61 )

#define NGPON_FEC_STRONG                            ( 2 )
#define NGPON_FEC_WEAK                              ( 1 )

#define NGPON_PSYNC_TOL_IN_SYNC                     ( 0x4 )
#define NGPON_PSYNC_TOL_OTHER                       ( 0x1 )


#if 1
/*
 *   For simulation only
 */
#define NGPON_SIM_ONU_1_ID                          ( 0x190 )
#define NGPON_SIM_ONU_2_ID                          ( 0x310 )
#define NGPON_SIM_ONU_3_ID                          ( 0x0 )

#define NGPON_PROP_ACCESS_ALLOC_ID                  ( 0x135B )
#define NGPON_LAST_FLOW_MASK                        ( 0xDA62 )

#define NGPON_ALLOC_ID_4_REC                        ( 0x1E6A )
#endif

/* TX params */
#define NGPON_MIN_RESPONSE_TIME                     ( 35 ) /* uSec */
#define NGPON_MAX_DAT_SETUP_PATTERN_LEN             ( 2 )  /* in 32 bits words*/   /* For Sim */
#define NGPON_MAX_DAT_HOLD_PATTERN_LEN              ( 2 )  /* in 32 bits words*/   /* For Sim */
#define NGPON_DEFAULT_DV_SETUP_LEN                  ( 1 )  /* For Sim */
#define NGPON_DEFAULT_DV_HOLD_LEN                   ( 1 )  /* For Sim */
#define NGPON_DEFAULT_DATA_PATTERN_REG              ( 0x0 ) /* For Sim */
#define NGPON_DEFAULT_DV_SETUP_PATTERN              ( 0x0 )
#define NGPON_DEFAULT_DV_HOLD_PATTERN               ( 0x0 )
#define NGPON_DEFAULT_DATA_SETUP_PATTERN            ( 0x0 )
#define NGPON_DEFAULT_DATA_HOLD_PATTERN             ( 0x0 )

#define NGPON_DEFAULT_DATA_SETUP_PATTERN_1          ( 0xFFFFFFFF )
#define NGPON_DEFAULT_DATA_SETUP_PATTERN_2          ( 0x3F78669F )
#define NGPON_DEFAULT_DATA_HOLD_PATTERN_1           ( 0x0 )
#define NGPON_DEFAULT_DATA_HOLD_PATTERN_2           ( 0xFFFFFFFF )
#define NGPON_DEFAULT_DATA_HOLD_PATTERN_3           ( 0x9E4E02C7 )


#define NGPON_TX_WORD_DELAY                         ( 0x60 )
#define NGPON_TX_FRAME_DELAY                        ( 0 )
#define NGPON_TX_BIT_DELAY                          ( 0 )

#define NGPON_MAX_TX_QUEUE_SIZE                     ( 1<<13 )  /* 2^13 = 8k */
#define NGPON_MAX_TOTAL_TX_QUEUES_SIZE              ( 20 * (1<<10) )/* 20k */
#define NGPON_TX_QUEUES_PER_GROUP                   ( 8 ) 
#define NGPON_TX_GROUP_COUNTERS                     ( 4 )
#define NGPON_TX_GROUP_TCONT_8_TO_TCONT_15          ( 0 )
#define NGPON_TX_GROUP_TCONT_16_TO_TCONT_23         ( 1 )
#define NGPON_TX_GROUP_TCONT_24_TO_TCONT_31        ( 2 )
#define NGPON_TX_GROUP_TCONT_32_TO_TCONT_39         ( 3 )

#define NGPON_TX_TCONT_PD_SIZE                                    ( 0x28 )
#define NGPON_TX_TCONT_FRONT_END_SIZE_TCONT_0_7                   ( 0x280 * 4 )  /* Sim setup*/
#define NGPON_TX_TCONT_FRONT_END_SIZE_TCONT_8_15                  ( 0x280 * 4 )
#define NGPON_TX_TCONT_FRONT_END_SIZE_TCONT_16_23                 ( 0x280 * 4 )
#define NGPON_TX_TCONT_FRONT_END_SIZE_TCONT_24_31                 ( 0x280 * 4 )
#define NGPON_TX_TCONT_FRONT_END_SIZE_TCONT_32_39                 ( 0x280 * 4 )

#define NGPON_TX_DEFAULT_TCONT_TO_GROUP_0            ( 0x6 )
#define NGPON_TX_DEFAULT_TCONT_TO_GROUP_1            ( 0x14 )
#define NGPON_TX_DEFAULT_TCONT_TO_GROUP_2            ( 0x23 )
#define NGPON_TX_DEFAULT_TCONT_TO_GROUP_3            ( 0x17 )

#define NGPON_RX_DEFAULT_TCONT_TO_GROUP_0            ( 1 )
#define NGPON_RX_DEFAULT_TCONT_TO_GROUP_1            ( 2 )
#define NGPON_RX_DEFAULT_TCONT_TO_GROUP_2            ( 3 )
#define NGPON_RX_DEFAULT_TCONT_TO_GROUP_3            ( NGPON_SN_TCONT_ID_VALUE )
#define NGPON_RX_CONG_ASSERT_THR                     ( 0x3f )


#define NGPON_TX_DEFAULT_XGEM_PYLD_MIN_LEN_VALUE     ( 0x4 )
#define NGPON_TX_DEFAULT_LOOPBACK_QUEUE              ( NGPON_DUMMY_TCONT_ID_VALUE - 1  )
#define NGPON_TX_DEFAULT_XGEM_HEADER_OPTIONS         ( 0x0 )
#define NGPON_TX_DEFAULT_US_LINE_RATE                ( NGPON_US_LINE_RATE_2_5G)
#define NGPON_TX_DEFAULT_TIMELINE_OFFSET_NGPON2      ( 0x2FF )  /* 1FF or XGPON 2FF for NGPON2 */
#define NGPON_TX_DEFAULT_TIMELINE_OFFSET_XGPON       ( 0x1FF )  /* 1FF or XGPON 2FF for NGPON2 */
#define NGPON_TX_DEFAULT_FRAME_LENGTH_MINUS_1_VALUE  ( 0x97df )
#define NGPON_TX_DEFAULT_IDLE_XGEM_PYLD_LEN_VALUE    ( 0x2 )
#define NGPON_TX_CFG_AND_CMD_PLM_0_STAT_OCCUPY_MAX_VALUE ( 0x4 )
#define NGPON_ROGUE_LEVEL_TIME_WINDOW                ( 0xC2B )
#define NGPON_ROGUE_DIFF_TIME_WINDOW                 ( 0x10A )
#define NGPON_LPBK_Q_NUM                             ( 5 )

#define NGPON_DFLT_XGEM_HDR_OPTIONS                  ( 0x0 )

#define NGPON_US_FRAME_LENGTH_2_5G                  ( 9719 )
#define NGPON_US_FRAME_LENGTH_10G                   ( 38879 )

#define NGPON_MAX_FLOW_ID_MASK_LEN                  ( ( 1<<16 ) - 1 )/* 16 bits of mask */
#define NGPON_MAX_TIMELINE_OFFSET                   ( ( 1<<7 ) - 1 ) /* 7 bits of offset */


#define NGPON_MAX_NUM_OF_TCONT_IDS                  ( NGPON_TCONT_ID_HIGH + 1 )
#define NGPON_MAX_NUM_OF_TRAFFIC_TCONT_IDS          ( NGPON_TRAFFIC_TCONT_ID_HIGH + 1 )
#define NGPON_MAX_NUM_OF_XGEM_FLOW_IDS              ( XGPON_RX_FLOW_NUMBER ) /* Value comes from RX registers file */



#if ( defined ( USE_SHELL ) )
void ngpon_print_error_code ( SHELL_SESSION_DTE * xi_session_ptr, ONUXG_ERROR xi_error );
#endif /* USE_SHELL */

#ifdef __cplusplus
}
#endif


#endif/* __NGPON_GENERAL_H */

