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


#ifndef GPON_CFG_H_INCLUDED
#define GPON_CFG_H_INCLUDED

#include <linux/types.h>
#include "pon_sm_common.h"

/******************************************************************************/
/* This type describes the various queues                                     */
/******************************************************************************/
typedef uint8_t GPON_QUEUE_ID_DTE;

/******************************************************************************/
/* This type defines the Auto restart TO1 mode                                  */
/******************************************************************************/
typedef uint32_t GPON_TO1_AUTO_RESTART_MODE_DTE;
/* Auto restart TO1 enabled */
#define GPON_TO1_AUTO_RESTART_ON  ((GPON_TO1_AUTO_RESTART_MODE_DTE)0)
/* Auto restart TO1 disabled */   
#define GPON_TO1_AUTO_RESTART_OFF ((GPON_TO1_AUTO_RESTART_MODE_DTE)1)

/*****************************************************************************/
/* This type defines the TCONT front end buffer configuration groups         */
/*****************************************************************************/
typedef uint32_t GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_DTE;

/* Tx-FIFO unit */
#define CE_TXPON_FRONT_END_BUFFER_GROUP_ID_0 ((GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_DTE)0)
#define CE_TXPON_FRONT_END_BUFFER_GROUP_ID_1 ((GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_DTE)1)
#define CE_TXPON_FRONT_END_BUFFER_GROUP_ID_2 ((GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_DTE)2)
#define CE_TXPON_FRONT_END_BUFFER_GROUP_ID_3 ((GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_DTE)3)

#define GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_RANGE_LOW  CE_TXPON_FRONT_END_BUFFER_GROUP_ID_0
#define GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_RANGE_HIGH CE_TXPON_FRONT_END_BUFFER_GROUP_ID_3

#define ME_GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_DTE_IN_RANGE(v) ((v) >= GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_RANGE_LOW && (v) <= GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_RANGE_HIGH)

/******************************************************************************/
/* status report mode                                                         */
/******************************************************************************/
typedef enum
{
    GPON_SR_MODE_MIPS     = 0,
    GPON_SR_MODE_RUNNER   = 1,
    GPON_SR_MODE_INVALID  = 2
}
GPON_SR_MODE_DTE;



/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_configure_link_params                                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON -  Set link parameters                                               */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function sets the parameters for the link                           */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*                                                                            */
/*   xi_link_default_configuration - The link parameters structure            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_configure_link_params(rdpa_gpon_link_cfg_t *const xi_link_default_configuration);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_link_status                                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON -  Get link status                                                   */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the current state of the link                      */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*                                                                            */
/*   xo_state - The output link state                                         */
/*     LINK_STATE_INACTIVE - Inactive                                         */
/*     LINK_STATE_ACTIVE - Active, please see sub-state                       */
/*     LINK_STATE_POPUP - Popup                                               */
/*     LINK_STATE_EMERGENCY_STOP - Emergency stop                             */
/*                                                                            */
/*   xo_sub_state - The output link sub-state                                 */
/*     LINK_SUB_STATE_NONE - None                                             */
/*     LINK_SUB_STATE_STANDBY - Standby                                       */
/*     LINK_SUB_STATE_POWER_SETUP - Power setup                               */
/*     LINK_SUB_STATE_SERIAL_NUMBER_ACQUSITION -                              */
/*       Serial number acqusition                                             */
/*     LINK_SUB_STATE_RANGING - Ranging                                       */
/*     LINK_SUB_STATE_AES_UPDATE - AES update                                 */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_link_status(LINK_STATE_DTE *const xo_state,
    LINK_SUB_STATE_DTE *const xo_sub_state,
    LINK_OPERATION_STATES_DTE *const xo_operational_state);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_configure_tcont                                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Configure Tcont                                                   */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function configure a T-Cont queue for upstream traffic.             */
/*                                                                            */
/* Input:                                                                     */
/*   xi_tcont_queue_id - T-Cont queue ID                                      */
/*                                                                            */
/*   xi_alloc_id - alloc id                                                   */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/* GPON_ALIGNMENT_ERROR - Parameter alignment error from the stack            */
/* PON_ERROR_INVALID_PARAMETER - Invalid parameter input to the stack level   */
/* PON_GENERAL_ERROR - General error from the stack level                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_configure_tcont(PON_TCONT_ID_DTE xi_tcont_queue_id,
    PON_ALLOC_ID_DTE xi_alloc_id);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_tcont_configuration                                             */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Get Tcont Configuration                                           */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function return a T-Cont configuration.                             */
/*                                                                            */
/* Input:                                                                     */
/*   xi_tcont_queue_id - T-Cont queue ID                                      */
/*                                                                            */
/*   xo_tcont_cfg - tcont cfg                                                 */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/* GPON_ALIGNMENT_ERROR - Parameter alignment error from the stack            */
/* PON_ERROR_INVALID_PARAMETER - Invalid parameter input to the stack level   */
/* PON_GENERAL_ERROR - General error from the stack level                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_tcont_configuration(PON_TCONT_ID_DTE xi_tcont_id,
   PON_TCONT_DTE *const xo_tcont_cfg);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_modify_tcont                                                        */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Modify Tcont                                                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function configure a TCONT queue for upstream traffic.              */
/*                                                                            */
/* Input:                                                                     */
/*   xi_tcont_id - TCONT queue ID                                             */
/*                                                                            */
/*   xi_alloc_id - alloc id                                                   */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/* GPON_ALIGNMENT_ERROR - Parameter alignment error from the stack            */
/* PON_ERROR_INVALID_PARAMETER - Invalid parameter input to the stack level   */
/* PON_GENERAL_ERROR - General error from the stack level                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_modify_tcont(PON_TCONT_ID_DTE xi_tcont_id,
    PON_ALLOC_ID_DTE xi_new_alloc_id);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_remove_tcont                                                        */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Remove Tcont                                                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function remove a T-Cont .                                          */
/*                                                                            */
/* Input:                                                                     */
/*   xi_tcont_id - T-Cont ID                                                  */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/* GPON_ALIGNMENT_ERROR - Parameter alignment error from the stack            */
/* PON_ERROR_INVALID_PARAMETER - Invalid parameter input to the stack level   */
/* PON_GENERAL_ERROR - General error from the stack level                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_remove_tcont(PON_TCONT_ID_DTE xi_tcont_id);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_configure_port_id_filter                                            */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - configure port id filter                                          */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function configure the port id filter .                             */
/*                                                                            */
/* Input:                                                                     */
/*   xi_tcont_queue_id - T-Cont queue ID                                      */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/* GPON_ALIGNMENT_ERROR - Parameter alignment error from the stack            */
/* PON_ERROR_INVALID_PARAMETER - Invalid parameter input to the stack level   */
/* PON_GENERAL_ERROR - General error from the stack level                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_configure_port_id_filter(PON_FLOW_ID_DTE xi_flow_id,
    PON_PORT_ID_DTE xi_port_id_filter, PON_PORT_ID_DTE xi_port_id_mask,
    bool xi_encryption_mode, PON_FLOW_TYPE_DTE xi_flow_type,
    PON_FLOW_PRIORITY_DTE xi_flow_priority, bool crcEnable);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_modify_encryption_port_id_filter                                    */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Modify encryption port id filter                                  */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function modify encryption the port id filter .                     */
/*                                                                            */
/* Input:                                                                     */
/*   xi_tcont_queue_id - T-Cont queue ID                                      */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*   GPON_ALIGNMENT_ERROR - Parameter alignment error from the stack          */
/*   PON_ERROR_INVALID_PARAMETER - Invalid parameter input                    */
/*   PON_GENERAL_ERROR - General error from the stack level .                 */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_modify_encryption_port_id_filter(PON_FLOW_ID_DTE xi_flow_id,
    bool xi_encryption_mode);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_control_port_id_filter                                              */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Control port id filter                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function control the port id filter .                               */
/*                                                                            */
/* Input:                                                                     */
/*   xi_tcont_queue_id - T-Cont queue ID                                      */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/* GPON_ALIGNMENT_ERROR - Parameter alignment error from the stack            */
/* PON_ERROR_INVALID_PARAMETER - Invalid parameter input to the stack level   */
/* PON_GENERAL_ERROR - General error from the stack level                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_control_port_id_filter(PON_FLOW_ID_DTE xi_flow_id, bool xi_filter_status);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_port_id_configuration                                           */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - return configuration of port id filter                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returnn configuration of the port id filter .              */
/*                                                                            */
/* Input:                                                                     */
/*   xi_tcont_queue_id - T-Cont queue ID                                      */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/* GPON_ALIGNMENT_ERROR - Parameter alignment error from the stack            */
/* PON_ERROR_INVALID_PARAMETER - Invalid parameter input to the stack level   */
/* PON_GENERAL_ERROR - General error from the stack level                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_port_id_configuration(PON_FLOW_ID_DTE xi_flow_id,
    bool *const xo_filter_status, PON_PORT_ID_DTE *const xo_port_id_filter,
    PON_PORT_ID_DTE *const xo_port_id_mask, bool *const xo_encryption_mode,
    PON_FLOW_TYPE_DTE *const xo_flow_type, PON_FLOW_PRIORITY_DTE *const xo_flow_priority,
    bool *xo_crc_enable);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_set_txfifo_queue_configuration                                      */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Set queue configuration                                        */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function configures the specified queue                             */
/*                                                                            */
/* This function is asynchronous and the caller should wait for completion    */
/* indication                                                                 */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_queue - The queue to act upon                                         */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_set_txfifo_queue_configuration(uint8_t xi_packet_descriptor_id,
    PON_FIFO_ID_DTE  xi_txfifo_id, uint8_t xi_packet_descriptor_queue_base_address,
    uint8_t xi_packet_descriptor_queue_size, uint16_t xi_tx_queue_offset,
    uint16_t xi_tx_max_queue_size);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_txfifo_queue_configuration_group_0_to_3                         */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Get TxFIFO Queue configuration for group 0-3                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns TCONT 8-39 front End buffer configuration group    */
/*                                                                            */
/* Registers :                                                                */
/*                                                                            */
/*   PDPGRP / TDQPGRP                                                         */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_group_id                                                              */
/*                                                                            */
/*   xo_packet_descriptor_queue_base_address - This field contains the        */
/*                                             offset of the packet           */
/*                                             descriptor group in the        */
/*                                             packet descriptor memory       */
/*                                                                            */
/*   xo_packet_descriptor_group_size - This field contains the size of the    */
/*                                     packet descriptor queue in the packet  */
/*                                     descriptor memory                      */
/*                                                                            */
/*   xo_group_offset - Offset of the group within the Tx FIFO                 */
/*                                                                            */
/*   xo_group_size - Size of group queue within the Tx FIFO                   */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/* GPON_ALIGNMENT_ERROR - Parameter alignment error from the stack            */
/* PON_ERROR_INVALID_PARAMETER - Invalid parameter input to the stack level   */
/* PON_GENERAL_ERROR - General error from the stack level                     */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_txfifo_queue_configuration_group_0_to_3(
    GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_DTE xi_group_id,
    uint16_t *const xo_packet_descriptor_group_base_address, 
    uint16_t *const xo_group_offset, uint16_t *const xo_group_size);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_txfifo_queue_configuration                                      */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Get txfifo queue configuration                                 */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns TCONT 0-7 front End buffer configuration           */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_queue - The queue to act upon                                         */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_txfifo_queue_configuration(uint8_t xi_packet_descriptor_id,
    PON_FIFO_ID_DTE xi_txfifo_id, uint16_t *const xo_packet_descriptor_queue_base_address,
    uint16_t *const xo_packet_descriptor_queue_size, uint16_t * const xo_tx_queue_offset,
    uint16_t *const xo_tx_queue_size);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_flush_queue                                                         */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON -  Flush queue                                                       */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function flushes the specified queue                                */
/*   This function is asynchronous and the caller should wait for completion  */
/*   indication                                                               */
/*                                                                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_flush_queue(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_set_loopback                                                        */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON -  Set loopback                                                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function sets the specific type of loopback for the link            */
/*   This function is asynchronous and the caller should wait for completion  */
/*   indication                                                               */
/*                                                                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*                                                                            */
/*   xi_loopback - The loopback to set                                        */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                       */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_set_loopback(bool xi_loopback_status, GPON_QUEUE_ID_DTE xi_loopback_queue);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_set_rxpon_bw_alloc_association                                      */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Set BW allocation association                                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*  This function  sets 4 alloc ids, on which user will have the ability to   */
/*  read their BW allocation counter.                                         */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*       xi_alloc_id_1 - Alloc ID(0 - 4095 , 65535 for invalid)               */
/*       xi_alloc_id_2 - Alloc ID(0 - 4095 , 65535 for invalid)               */
/*       xi_alloc_id_3 - Alloc ID(0 - 4095 , 65535 for invalid)               */
/*       xi_alloc_id_4 - Alloc ID(0 - 4095 , 65535 for invalid)               */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_ERROR_INVALID_STATE - Invalid state                                */
/*     PON_ERROR_ALLOC_NOT_IN_RANGE  - Alloc id not in range                  */
/*     PON_ERROR_ALLOC_NOT_ASSIGNED -alloc-id not assigned to ONU             */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_set_rxpon_bw_alloc_association(PON_ALLOC_ID_DTE xi_alloc_id_1,
    PON_ALLOC_ID_DTE xi_alloc_id_2, PON_ALLOC_ID_DTE xi_alloc_id_3,
    PON_ALLOC_ID_DTE xi_alloc_id_4);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_rx_bw_alloc_association                                         */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON -  Get BW allocation association                                     */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*  This function returns 4 alloc ids, on which user will have the ability to */
/*  read their BW allocation counter.                                         */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*       xo_alloc_id_1 - Alloc ID(0 - 1023)                                   */
/*       xo_alloc_id_2 - Alloc ID(0 - 1023)                                   */
/*       xo_alloc_id_3 - Alloc ID(0 - 1023)                                   */
/*       xo_alloc_id_4 - Alloc ID(0 - 1023)                                   */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_ERROR_INVALID_STATE - Invalid state                                */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_rx_bw_alloc_association(PON_ALLOC_ID_DTE *const xo_alloc_id_1,
    PON_ALLOC_ID_DTE *const xo_alloc_id_2, PON_ALLOC_ID_DTE *const xo_alloc_id_3, 
    PON_ALLOC_ID_DTE *const xo_alloc_id_4);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_dba_sr_process_initialize                                           */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Initialize DBA status report process                               */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function initializes the DBA sr process.                            */
/*   This function is synchronous.                                            */
/*                                                                            */
/* Input:                                                                     */
/*   xi_sr_reporting_block - Size of reporting block in bytes.                */                                           
/*   xi_update_interval - Interval to update DBRu arrays in the HW            */
/*   interval values:                                                         */
/*             0 for 500 microsecs                                            */
/*             125 for 125 microsecs                                          */
/*             250 for 250 microsecs                                          */
/*             1-10 milisecs                                                  */
/*                                                                            */
/* Output:                                                                    */                                                                  
/*    PON_NO_ERROR  on success or                                             */
/*    specific error code of type PON_ERROR_DTE on failure                    */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_dba_sr_process_initialize(uint32_t xi_sr_reporting_block,
    uint32_t xi_update_interval);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_dba_sr_process_terminate                                            */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Terminate DBA status report process                                */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function terminates the DBA sr process.                             */
/*   This function is synchronous.                                            */
/*                                                                            */
/* Input:                                                                     */
/*   N/A                                                                      */
/*                                                                            */
/* Output:                                                                    */
/*   PON_NO_ERROR on success or                                               */
/*   specific error code of type PON_ERROR_DTE on failure.                    */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_dba_sr_process_terminate(void);
/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_set_auto_restart_to1_mode                                           */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON -  Set Auto restart TO1 mode                                         */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function configures the auto restart TO1 mode                       */
/*                                                                            */
/* Input:                                                                     */
/*   xi_to1_auto_restart - Auto restart TO1 mode                              */
/*                                                                            */
/* Output:                                                                    */
/*   PON_ERROR_INVALID_STATE  -                                               */
/*       Try to intiate an action in wrong state                              */
/*   PON_NO_ERROR - No Error                                                  */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_set_TO1_auto_restart_mode(GPON_TO1_AUTO_RESTART_MODE_DTE xi_to1_auto_restart);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_set_gem_block_size                                                  */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Set GEM block Size                                                 */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function modifies the GEM block size                                */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_gem_block_size - GEM block size                                       */
/*                                                                            */
/* Output:                                                                    */
/*   PON_ERROR_INVALID_STATE Try to intiate an action in wrong state          */
/*   PON_NO_ERROR - No Error                                                  */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_set_gem_block_size(PON_GEM_BLOCK_SIZE_DTE  xi_gem_block_size);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_to1_auto_restart_mode                                           */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON -  Get TO1 Auto restart mode                                        */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function return the mode of the TO! auto restart                    */
/*                                                                            */
/* Input:                                                                     */
/*   xo_to1_auto_restart - Auto restart TO1 mode                              */
/*                                                                            */
/* Output:                                                                    */
/*   PON_ERROR_INVALID_STATE  -                                               */
/*       Try to intiate an action in wrong state                              */
/*     PON_NO_ERROR - No Error                                                */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_to1_auto_restart_mode(GPON_TO1_AUTO_RESTART_MODE_DTE *const xo_to1_auto_restart);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_set_dbru_report_parameters                                          */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Set DBRu Report Parameters                                        */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/* This function sets the DBRu report mode and the gem block size             */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_update_mode - Boolean: mips (SW) /runner (HW)                         */
/*                                                                            */
/*   xi_gem_block_size - 32/48/64 Bytes                                       */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Success or error returned code                           */
/*     PON_NO_ERROR - No errors or faults                                     */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_set_dbru_report_parameters(GPON_SR_MODE_DTE xi_update_mode,
    PON_GEM_BLOCK_SIZE_DTE xi_gem_block_size);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_set_rogue_onu_detection_params                                      */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Set Rogue ONU Detection Parameters                             */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function Set the Rouge ONU detection parameters such as source      */
/*   detection thresholds, and clear counters                                 */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_rogue_onu_parameters - Rogue ONU paramteres struct                    */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Success or error returned code                           */
/*     PON_NO_ERROR - No errors or faults                                     */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_set_rogue_onu_detection_params(const PON_TX_ROGUE_ONU_PARAMETERS *xi_rogue_onu_parameters);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_get_rogue_onu_detection_params                                   */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Get Rogue ONU Detection Parameters                             */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_tx_get_rogue_onu_detection_params(PON_TX_ROGUE_ONU_PARAMETERS *rogue_onu_params);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_control_8khz_clock                                                  */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL GPON - Set phase lock                                                 */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function sets the phase lock                                        */
/*                                                                            */
/* Input:                                                                     */
/*    xi_enable_phase_difference - enable/disable the phase lock              */
/*    xi_phase_difference - the phase lock difference                         */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   ONUG_ERROR_DTE - Return code                                             */
/*     CE_ONUG_NO_ERROR - Success                                             */
/*     CE_ONUG_ERROR_INVALID_STATE - Invalid state error                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_control_8khz_clock(bdmf_boolean xi_enable_phase_difference,
    uint16_t xi_phase_difference);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_set_flush_valid_bit                                              */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Set flush valid bit                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   The function configures the flush mode and waits until flush done.       */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_tx_set_flush_valid_bit(uint8_t tcont_id,
    bdmf_boolean flush_enable, bdmf_boolean flush_immediate);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_get_flush_valid_bit                                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON_Tx - Get flush valid bit                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   The function returns the flush tcont queue, the flush mode, the flush    */
/*    valid bit and the flush done bit.                                       */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_tx_get_flush_valid_bit(uint8_t *tcont_id, bdmf_boolean *flush_enable,
    bdmf_boolean *flush_immediate, bdmf_boolean *flush_done);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_pon_stop_pee_interval                                                  */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Stop BER interval timer                                                  */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   Stop watchdog of PEE interval.                                           */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*    PON_ERROR_DTE                                                          */
/******************************************************************************/
PON_ERROR_DTE f_pon_stop_pee_interval(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_ber_enable                                                          */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Enable BER                                                        */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function enables mechanism: BER                                     */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*    None                                                                    */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_ber_enable (void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_pon_start_ber_interval                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Start BER interval timer                                                 */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   Starts watchdog of BER interval. If already active restarts with new     */
/*   timeout value.                                                           */
/* Input:                                                                     */
/*   uint32_t - ber interval in msec                                          */
/*                                                                            */
/* Output:                                                                    */
/*    PON_ERROR_DTE                                                           */
/******************************************************************************/
PON_ERROR_DTE p_pon_start_ber_interval(uint32_t xi_ber_interval);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_pon_stop_ber_interval                                                  */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Stop BER interval timer                                                  */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   Stop watchdog of BER interval.                                           */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*    PON_ERROR_DTE                                                          */
/******************************************************************************/
PON_ERROR_DTE f_pon_stop_ber_interval(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_ber_disable                                                         */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL GPON - Disable BER                                                    */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function disables mechanism: BER                                    */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*    None                                                                    */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_ber_disable(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_onu_id                                                          */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Get onu id                                                         */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the onu id                                         */
/*                                                                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   xo_equalization_delay                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_onu_id(uint16_t *const xo_onu_id);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_overhead_and_length                                             */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Get Overhead and length                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the overhead and length                            */
/*                                                                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   xo_equalization_delay                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_overhead_and_length (PON_TX_OVERHEAD_DTE  *const xo_pon_overhead,
    uint8_t *const xo_pon_overhead_length, uint8_t *const xo_pon_overhead_repetition_length,
    uint8_t *const xo_pon_overhead_repetition);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_onu_information                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Get ONU information                                                */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the current information of the ONU                 */
/*   This function is asynchronous and the caller should wait for completion  */
/*   indication                                                               */
/*                                                                            */
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
PON_ERROR_DTE gpon_get_onu_information(PON_SERIAL_NUMBER_DTE *const xo_serial_number,
    PON_PASSWORD_DTE *const xo_password, uint16_t *const xo_onu_id,
    uint32_t *const xo_equalization_delay, uint32_t *const xo_downstream_ber_interval,
    PON_TX_OVERHEAD_DTE *const xo_pon_overhead, uint8_t *const xo_pon_overhead_length,
    uint8_t *const xo_pon_overhead_repetition_length, uint8_t *const xo_pon_overhead_repetition,
    uint8_t *const xo_transceiver_power_level);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_equalization_delay                                              */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Get equalization delay                                             */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the equalization delay of the ONU                  */
/*                                                                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   xo_equalization_delay                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_equalization_delay(uint32_t *const xo_equalization_delay);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_transceiver_power_level                                         */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Get transceiver power level                                        */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the transceiver power level                        */
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
PON_ERROR_DTE gpon_get_transceiver_power_level(uint8_t *const xo_transceiver_power_level);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_configure_onu_serial_number                                         */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Configure ONU Serial Number                                        */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function configures the ONU's serial number according to the        */
/*   format defined in ITU-T/G984.3.                                          */
/*   The function can be used only when link is Inactive                      */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_serial_number -  ONU serial number struct                             */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - The function's returned fail/success status              */
/*     PON_NO_ERROR - No errors or faults                                     */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_configure_onu_serial_number(PON_SERIAL_NUMBER_DTE xi_serial_number);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_configure_onu_password                                              */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Configure ONU password                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function configure the GPON password, can be used                   */
/*   only when the GR-BPON Link is in 'Inactive' state.                       */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_onu_password - ONU Password                                           */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/* GPON_ALIGNMENT_ERROR - Parameter alignment error from the stack            */
/* PON_ERROR_INVALID_PARAMETER - Invalid parameter input                      */
/* PON_GENERAL_ERROR - General error from the stack level                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_configure_onu_password(PON_PASSWORD_DTE xi_onu_password);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_assign_callback                                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Assign callback functions                                          */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function assigns callback routine to references callback type.      */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*                                                                            */
/*   xi_event_and_alarm_callback - Callback routine                           */
/*                                                                            */
/*   xi_dbr_runner_data_callback - Callback routine                           */
/*                                                                            */
/*   xi_sc_sc_runner_callback - Callback routine                              */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_assign_user_callback(EVENT_AND_ALARM_CALLBACK_DTE xi_event_and_alarm_callback,
    DBR_PROCESS_RUNNER_DATA_CALLBACK_DTE xi_dbr_runner_data_callback,
    SC_SC_RUNNER_DATA_CALLBACK_DTE xi_sc_sc_runner_callback, 
    FLUSH_TCONT_CALLBACK_DTE xi_flush_tcont_callback);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   f_get_dbr_divide_ratio                                                   */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Returns the divide ratio of the DBR.                                     */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the divide ratio of the DBR according to the given */
/*   value of gem block size.                                                 */
/*                                                                            */
/* Input:                                                                     */
/*   xi_gem_block_size - gem block size.                                      */
/*                                                                            */
/* Output:                                                                    */
/*   xo_dbr_divide_ratio - the divide ratio, determined by the block size.    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                          */
/******************************************************************************/
PON_ERROR_DTE f_get_dbr_divide_ratio(uint32_t xi_gem_block_size, uint8_t *xo_dbr_divide_ratio);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_rx_set_alloc_id_to_tcont_association                                */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Rx - Associate Alloc Id index to Tcont.                             */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   The function associate Alloc Id index to Tcont.                          */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_rx_set_alloc_id_to_tcont_association(uint32_t alloc_id_idx,
    uint16_t tcont_id);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_rx_get_receiver_status                                              */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Rx - Get Receiver Status                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   The function returns receiver status: LOF, FEC, LCDG                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_rx_get_receiver_status(bdmf_boolean *lof_state,
    bdmf_boolean *fec_state, bdmf_boolean *lcdg_state);


void gpon_register_cb(PON_SM_CALLBACK_FUNC *p_pon_sm_cb);

/* DBA Statistic Debug Functions */
#ifdef DBA_DEBUG_STATISTICS
/* Allocates debug statistics database. returns the size of allocated memory */
uint32_t gpon_dba_allocate_debug_statistics_db(uint32_t xi_number_of_tconts , uint32_t xi_number_of_cycles);
/* Free debug statistics database */
void gpon_dba_free_debug_statistics_db(uint32_t xi_number_of_cycles);
/* tcont id -> debug statistics index mapping */
uint32_t gpon_get_debug_statistics_index(GPON_FIFO_ID_DTE xi_tcont_id);
void gpon_dba_debug_reset_statistics(void);
#endif 

#ifdef USE_BDMF_SHELL
void gpon_initialize_drv_pon_stack_shell(bdmfmon_handle_t driver_dir);
void gpon_exit_drv_pon_stack_shell(void);
#endif 

#endif
