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

#include <bcm_ext_timer.h>
#include <boardparms.h>
#include "gpon_cfg.h"
#include "gpon_stat.h"
#include "gpon_ind_task.h"
#include "gpon_all.h"
#include "gpon_db.h"
#include "gpon_general.h"
#include "gpon_logger.h"
#include "rogue_drv.h"
#ifdef CONFIG_BCM_GPON_TODD
#include "gpon_tod.h"
#endif

extern bdmf_fastlock gs_fastlock;    
extern bdmf_fastlock gs_fastlock_irq;
extern GPON_DATABASE_DTS gs_gpon_database;
extern GPON_CALLBACKS_DTE gs_callbacks;

#define CS_125_MICROSEC 125 
#define CS_250_MICROSEC 250 
#define CS_500_MICROSEC 500 
/* Zero will represent 0.5 miliseconds interval since we can not configure fracation */
#define CS_0_5_MILISEC_SR_DBA_POLLING_INTERVAL 0 

#ifdef DBA_DEBUG_STATISTICS
#define CE_DBA_DEBUG_STATISTICS_DATABASE_SIZE_THREASHOLD_FOR_WARNING 1000000 
#define CE_DBA_DEBUG_STATISTICS_DONT_CARE_VALUE 0xFFFFFFFF 

/* Debug statistics - TCONT info */
typedef struct 
{
    uint32_t dba_sr_data;
    uint32_t tcont_occupancy;
    uint32_t runner_occupancy;
    uint8_t dbr_message;
}
GPON_DBA_DEBUG_STATISTICS_TCONT_INFO_DTE;

/* Debug statistics - cycle info */
typedef struct 
{
    GPON_DBA_DEBUG_STATISTICS_TCONT_INFO_DTE *statistics_tcont_info_array;
}
GPON_DBA_DEBUG_STATISTICS_CYCLE_INFO_DTE;

/* Debug statistics */
typedef struct 
{
    GPON_DBA_DEBUG_STATISTICS_CYCLE_INFO_DTE * cycle_info_array ;
    /* mapping of tcont id to index to the statistics_tcont_info_array array */
    uint32_t tcont_to_index[TXPON_FIFO_UNIT_NUMBER];
    uint32_t num_of_tconts_in_tcont_to_index_array;
}
GPON_DBA_DEBUG_STATISTICS_DTE;

/* debug statistics */
GPON_DBA_DEBUG_STATISTICS_DTE * ge_dba_debug_statistics = NULL;
/* this flag indicates whether to collect the debug statistics */
bool ge_dba_debug_statistics_collect =  BDMF_FALSE;
/* flag for warning that statistics were not collected for all TCONTs because
   the database is not big enough */
bool ge_dba_debug_statistics_warning_not_all_tconts_were_collected = BDMF_FALSE;
/* maximal number of cycles to collect statistics - this value is used
   for the database memory allocation */
uint32_t ge_dba_debug_statistics_max_number_of_cycles = 0;
/* maximal number of tconts to collect statistics - this value is used for the
   database memory allocation */
uint32_t ge_dba_debug_statistics_max_number_of_tconts = 0;
/* cycle counter */
uint32_t ge_dba_debug_statistics_cycle_counter = 0;
/* number of cycles to collect statistics */
uint32_t ge_dba_debug_statistics_number_of_cycles = 0;
#endif 


/* Internal functions declaration */
static PON_ERROR_DTE f_set_filter_configuration(PON_CONFIGURE_FILTER_INDICATION configure_filter_struct);
static PON_ERROR_DTE f_encryption_filter_configuration(PON_CONFIGURE_FILTER_INDICATION *configure_filter_struct);
static PON_ERROR_DTE f_set_filter_control(PON_CTRL_FILTER_INDICATION *control_filter_struct);
static PON_ERROR_DTE gpon_rx_set_flow_status(PON_FLOW_ID_DTE flow_id,
    bdmf_boolean flow_id_status);
static PON_ERROR_DTE misc_cfg(uint8_t prodc[16], uint8_t prcl, uint8_t brc, uint8_t prl,
    uint16_t msstart, uint16_t msstop, gpon_tx_gen_ctrl_ten tx_status);
static PON_ERROR_DTE gpon_tx_generate_misc_transmit(uint8_t prodc[16], uint8_t prcl,
    uint8_t brc, uint8_t prl, uint16_t msstart, uint16_t msstop, bdmf_boolean enable);
static void p_dba_sr_process_callback(unsigned long xi_user_params_ptr);
static PON_ERROR_DTE gpon_rx_set_flow_cfg(PON_FLOW_ID_DTE flow_id,
    PON_PORT_ID_DTE port_id, bdmf_boolean encrypt, PON_FLOW_TYPE_DTE type,
    PON_FLOW_PRIORITY_DTE priority);
static uint8_t p_encode_dbru(uint16_t tokens);
#ifdef DBA_DEBUG_STATISTICS
static void p_gpon_dba_debug_collect_debug_statistics_if_needed(PON_FIFO_ID_DTE xi_tcont_id,
    uint32_t xi_dba_sr_data, uint32_t xi_tcont_occupancy,
    uint32_t xi_runner_occupancy, uint8_t  xi_dbr_message);
#endif 

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
    FLUSH_TCONT_CALLBACK_DTE xi_flush_tcont_callback)
{
    PON_ERROR_DTE gpon_error;
    GPON_CALLBACKS_DTE callback_struct;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Check for initial pointer for callback function */
    if (xi_event_and_alarm_callback == 0 || xi_dbr_runner_data_callback == 0 ||
        xi_sc_sc_runner_callback == 0 || xi_flush_tcont_callback == 0) 
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Init the callback struct */
    callback_struct.event_and_alarm_callback = xi_event_and_alarm_callback;
    callback_struct.dbr_runner_data_callback = xi_dbr_runner_data_callback;
    callback_struct.sc_sc_runner_callback = xi_sc_sc_runner_callback;
    callback_struct.flush_tcont_callback = xi_flush_tcont_callback;

    /* Send indication to Pon task to assign callback functions */
    gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_ASSIGN_CALLBACKS,
        callback_struct);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to send oam indication: error %d !", gpon_error);
        return gpon_error;
    }

    return PON_NO_ERROR;
}


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
    LINK_OPERATION_STATES_DTE *const xo_operational_state)
{
    /* Varify that the stack is initialized */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Set output values */
    * xo_state = gs_gpon_database.link_parameters.operation_state_machine.link_state;
    * xo_sub_state = gs_gpon_database.link_parameters.operation_state_machine.link_sub_state;
    * xo_operational_state = gs_gpon_database.link_parameters.operation_state_machine.activation_state;

    return PON_NO_ERROR;
}


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
    PON_ALLOC_ID_DTE xi_alloc_id)
{
    PON_ERROR_DTE gpon_error = 0;
    uint32_t index = 0;
    uint32_t tcont_index = 0;
    uint32_t alloc_index = 0;
    uint32_t empty_index = 0;
    bool tcont_found_in_table = BDMF_FALSE;
    bool alloc_found_in_table = BDMF_FALSE;

    /* Varify that the stack is initialized */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Check the validity of the parameters together with the queue
       size parameter refering to the rest of the queue configureations */
    if (!PON_TCONT_ID_DTE_IN_RANGE(xi_tcont_queue_id) || 
        !PON_ALLOC_ID_DTE_IN_RANGE(xi_alloc_id))
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    /* Disable interrupt to prevent race between ISR and API */
    bdmf_fastlock_lock(&gs_fastlock);

    /* Search if this tcont-id and alloc-id are anywhere in the table */
    for (index = 0; index < MAX_NUMBER_OF_TCONTS; index++)
    {
        if (gs_gpon_database.tconts_table[index].assign_alloc_valid_flag == BDMF_TRUE ||
            gs_gpon_database.tconts_table[index].config_tcont_valid_flag == BDMF_TRUE)
        {
            if (gs_gpon_database.tconts_table[index].tcont_id == xi_tcont_queue_id)
            {
                tcont_found_in_table = BDMF_TRUE;
                tcont_index = index;
            }

            if (gs_gpon_database.tconts_table[index].alloc_id == xi_alloc_id )
            {
                alloc_found_in_table = BDMF_TRUE;
                alloc_index = index;
            }
        }
    }

    /* if the tcont is already apper in the table - return error */
    if ( tcont_found_in_table == BDMF_TRUE)
    {
        if (gs_gpon_database.tconts_table[tcont_index].config_tcont_valid_flag == BDMF_TRUE)
        {
            p_log(ge_onu_logger.sections.stack.debug.general_id, 
                "Error: The Tcont is already configured to alloc %d", 
                gs_gpon_database.tconts_table[tcont_index].alloc_id);
            bdmf_fastlock_unlock(&gs_fastlock);
            return PON_ERROR_TCONT_IS_ALREADY_CFG;
        }
        else
        {
            p_log(ge_onu_logger.sections.stack.debug.general_id, 
                "Error state tcont associate to anther alloc - We Should Not Get Here");
            bdmf_fastlock_unlock(&gs_fastlock);
            return PON_ERROR_INVALID_STATE;
        }

    }

    /* if the alloc apper in the table */
    if (alloc_found_in_table == BDMF_TRUE)
    {
        /* check if this alloc already associate to another tcont*/
        if (gs_gpon_database.tconts_table[alloc_index].config_tcont_valid_flag == BDMF_TRUE )
        {
            p_log(ge_onu_logger.sections.stack.debug.general_id,
                "Alloc-ID %d already assigned to Tcont %d",
                xi_alloc_id, gs_gpon_database.tconts_table[alloc_index].tcont_id);
            bdmf_fastlock_unlock(&gs_fastlock);
            return PON_ERROR_ALLOC_ALREADY_ASSIGN_TO_ANOTHER_TCONT;
        }
        else
        {
            /* if the alloc apper in the table and has no tcont associate and his assign alloc flag is false - wrong state */
            if (gs_gpon_database.tconts_table[alloc_index].assign_alloc_valid_flag == BDMF_FALSE)
            {
                bdmf_fastlock_unlock(&gs_fastlock);
                return PON_ERROR_INVALID_STATE;
            }
            else
            {
                gpon_error = gpon_rx_set_alloc_id_to_tcont_association(alloc_index, xi_tcont_queue_id);
                if (gpon_error != PON_NO_ERROR)
                {
                    p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                        "Unable to set tcont id %d to alloc index %d.", xi_tcont_queue_id, alloc_index);
                    return gpon_error;
                }

                gs_gpon_database.tconts_table[alloc_index].tcont_id = xi_tcont_queue_id;
                gs_gpon_database.tconts_table[alloc_index].config_tcont_valid_flag = BDMF_TRUE;
            }
        }
    }

    /* if the alloc and the tcont is not apper in the table */
    else
    {
        for (empty_index = 1; empty_index < MAX_NUMBER_OF_TCONTS; empty_index++)
        {
            if (gs_gpon_database.tconts_table[empty_index].assign_alloc_valid_flag == BDMF_FALSE &&
                gs_gpon_database.tconts_table[empty_index].config_tcont_valid_flag == BDMF_FALSE)
            {
                break;
            }
        }

        if (empty_index == MAX_NUMBER_OF_TCONTS)
        {
            bdmf_fastlock_unlock(&gs_fastlock);
            return PON_ERROR_TCONT_TABLE_IS_FULL;
        }

        /* add tcont to data base */
        gs_gpon_database.tconts_table[empty_index].alloc_id = xi_alloc_id;
        gs_gpon_database.tconts_table[empty_index].tcont_id = xi_tcont_queue_id;
        gs_gpon_database.tconts_table[empty_index].config_tcont_valid_flag = BDMF_TRUE;
    }

    /* Enable interrupt */
    bdmf_fastlock_unlock(&gs_fastlock);

    /* Check Error */
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to set Tcont configuration (%d)", gpon_error);
        return gpon_error;
    }

    return PON_NO_ERROR;
}

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
    PON_TCONT_DTE *const xo_tcont_cfg)
{
    uint32_t index;

    /* Varify that the stack is initialized */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    if (!PON_TCONT_ID_DTE_IN_RANGE(xi_tcont_id))
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    for (index = 0; index < MAX_NUMBER_OF_TCONTS; index++)
    {
        if (gs_gpon_database.tconts_table[index].config_tcont_valid_flag == BDMF_TRUE &&
            gs_gpon_database.tconts_table[index].tcont_id == xi_tcont_id)
        {
            xo_tcont_cfg->tcont_id = xi_tcont_id;
            xo_tcont_cfg->alloc_id = gs_gpon_database.tconts_table[index].alloc_id;
            xo_tcont_cfg->assign_alloc_valid_flag = gs_gpon_database.tconts_table[index].assign_alloc_valid_flag;
            xo_tcont_cfg->config_tcont_valid_flag = gs_gpon_database.tconts_table[index].config_tcont_valid_flag;
            xo_tcont_cfg->tcont_queue_size = gs_gpon_database.tconts_table[index].tcont_queue_size;
            break;
        }
    }
    if (index == MAX_NUMBER_OF_TCONTS)
    {
        return PON_ERROR_TCONT_NOT_ASSIGNED;
    }

    return PON_NO_ERROR;
}


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
    PON_ALLOC_ID_DTE xi_new_alloc_id)
{
    PON_ERROR_DTE gpon_error = 0;
    uint32_t index = 0;
    uint32_t tcont_index = 0;
    uint32_t alloc_index = 0;
    uint32_t empty_index = 0;
    bool tcont_found_in_table = BDMF_FALSE;
    bool alloc_found_in_table = BDMF_FALSE;       

    /* Varify that the stack is initialized */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Check the validity of the parameters together with the queue
       size parameter refering to the rest of the queue configureations */
    if (!PON_TCONT_ID_DTE_IN_RANGE(xi_tcont_id) ||
        !PON_ALLOC_ID_DTE_IN_RANGE(xi_new_alloc_id))
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    for (index = 0; index < MAX_NUMBER_OF_TCONTS; index++)
    {
        if (gs_gpon_database.tconts_table[index].assign_alloc_valid_flag == BDMF_TRUE ||
            gs_gpon_database.tconts_table[index].config_tcont_valid_flag == BDMF_TRUE)
        {
            if (gs_gpon_database.tconts_table[index].tcont_id == xi_tcont_id)
            {
                tcont_found_in_table = BDMF_TRUE;
                tcont_index = index;
            }

            if (gs_gpon_database.tconts_table[index].alloc_id == xi_new_alloc_id)
            {
                alloc_found_in_table = BDMF_TRUE;
                alloc_index = index;
            }
        }
    }

    if (tcont_found_in_table != BDMF_TRUE)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "ERROR: Tcont %d not found(as valid ) in tcont table", xi_tcont_id);
        return PON_GENERAL_ERROR;
    }
    else
    {
        /* Disable interrupt to prevent race between ISR and API */
        bdmf_fastlock_lock(&gs_fastlock);

        if (gs_gpon_database.tconts_table[tcont_index].config_tcont_valid_flag == BDMF_TRUE)
        {
            /* the old alloc id is not assign by the OLT */
            if (gs_gpon_database.tconts_table[tcont_index].assign_alloc_valid_flag == BDMF_FALSE)
            {
                /* the old alloc that was assoicate to this tcont is not assign yet and */
                if (alloc_found_in_table == BDMF_TRUE)
                {
                    if (gs_gpon_database.tconts_table[alloc_index].config_tcont_valid_flag == BDMF_TRUE)
                    {
                        /* the old alloc that was assoicate to this tcont is not assign yet and */
                        /* the new alloc that should be associate to this tcont is already assign to another tcont */

                        p_log(ge_onu_logger.sections.stack.debug.general_id, 
                            "Error: The Alloc id is already associate to another tcont ");
                        bdmf_fastlock_unlock(&gs_fastlock);
                        return( PON_ERROR_ALLOC_ALREADY_ASSIGN_TO_ANOTHER_TCONT);
                    }
                    else
                    {
                        /* the old alloc that was assoicate to this tcont is not assign yet and */
                        /* the new alloc that should be associate to this tcont is assign by the OLT */
                        gpon_error = gpon_rx_set_alloc_id_to_tcont_association(alloc_index, xi_tcont_id);
                        if (gpon_error != PON_NO_ERROR)
                        {
                            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                               "Unable to set tcont id %d to alloc index %d!", xi_tcont_id, alloc_index);
                           return gpon_error;
                        }

                        gs_gpon_database.tconts_table[alloc_index].config_tcont_valid_flag = BDMF_TRUE;
                        gs_gpon_database.tconts_table[alloc_index].tcont_id                = xi_tcont_id;
                        gs_gpon_database.tconts_table[tcont_index].tcont_id                = CS_DEFAULT_TCONT_ID;
                        gs_gpon_database.tconts_table[tcont_index].config_tcont_valid_flag = BDMF_FALSE;
                        gs_gpon_database.tconts_table[tcont_index].alloc_id                = PON_ALLOC_ID_DTE_NOT_ASSIGNED;

                    }
                }
                else
                {
                    /* the old alloc that was assoicate to this tcont is not assign yet and */
                    /* the new alloc that should be associate to this tcont is not assign yet */
                    for (empty_index = 1; empty_index < MAX_NUMBER_OF_TCONTS; empty_index++)
                    {
                        if (( gs_gpon_database.tconts_table[empty_index].assign_alloc_valid_flag == BDMF_FALSE) &&
                            (gs_gpon_database.tconts_table[empty_index].config_tcont_valid_flag == BDMF_FALSE))
                        {
                            break;
                        }
                    }

                    if (empty_index == MAX_NUMBER_OF_TCONTS)
                    {
                        bdmf_fastlock_unlock(& gs_fastlock);
                        return( PON_ERROR_TCONT_TABLE_IS_FULL);
                    }

                    gs_gpon_database.tconts_table[tcont_index].config_tcont_valid_flag = BDMF_FALSE;
                    gs_gpon_database.tconts_table[tcont_index].tcont_id                = CS_DEFAULT_TCONT_ID;
                    gs_gpon_database.tconts_table[empty_index].tcont_id                = xi_tcont_id;
                    gs_gpon_database.tconts_table[empty_index].config_tcont_valid_flag = BDMF_TRUE;
                    gs_gpon_database.tconts_table[empty_index].alloc_id                = xi_new_alloc_id;
                    gs_gpon_database.tconts_table[empty_index].assign_alloc_valid_flag = BDMF_FALSE;
                 }
            }

            /* The old alloc is already assign by the OLT */
            else
            {
                if (alloc_found_in_table == BDMF_TRUE)
                {
                    /* The new alloc is found in the table */
                    if (gs_gpon_database.tconts_table[alloc_index].config_tcont_valid_flag == BDMF_TRUE)
                    {
                        /* the new alloc that should be associate to this tcont is already assign to another tcont */
                        p_log(ge_onu_logger.sections.stack.debug.general_id, 
                            "Error: The Alloc id is already associate to another tcont ");
                        bdmf_fastlock_unlock(& gs_fastlock);
                        return( PON_ERROR_ALLOC_ALREADY_ASSIGN_TO_ANOTHER_TCONT);
                    }
                    else
                    {
                        /* the old alloc that was assoicate to this tcont is assign by the OLT and */
                        /* the new alloc that should be associate to this tcont is assign by the OLT */
                        gpon_error = gpon_rx_set_alloc_id_to_tcont_association(alloc_index, xi_tcont_id);
                        if (gpon_error != PON_NO_ERROR)
                        {
                            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                               "Unable to set tcont id %d to alloc index %d.", xi_tcont_id, alloc_index);
                           return gpon_error;
                        }

                        gpon_error = gpon_rx_set_alloc_id_to_tcont_association(tcont_index, CS_DEFAULT_TCONT_ID);
                        if (gpon_error != PON_NO_ERROR)
                        {
                            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                               "Unable to set tcont id %d to alloc index %d.", CS_DEFAULT_TCONT_ID, tcont_index);
                           return gpon_error;
                        }

                        gs_gpon_database.tconts_table[alloc_index].config_tcont_valid_flag = BDMF_TRUE;
                        gs_gpon_database.tconts_table[alloc_index].tcont_id                = xi_tcont_id;
                        gs_gpon_database.tconts_table[tcont_index].tcont_id                = CS_DEFAULT_TCONT_ID;
                        gs_gpon_database.tconts_table[tcont_index].config_tcont_valid_flag = BDMF_FALSE;

                    }
                }
                else
                {

                    for (empty_index = 1; empty_index < MAX_NUMBER_OF_TCONTS; empty_index++)
                    {
                        if (( gs_gpon_database.tconts_table[empty_index].assign_alloc_valid_flag == BDMF_FALSE) &&
                            (gs_gpon_database.tconts_table[empty_index].config_tcont_valid_flag == BDMF_FALSE))
                        {
                            break;
                        }
                    }

                    if (empty_index == MAX_NUMBER_OF_TCONTS)
                    {
                        bdmf_fastlock_unlock(& gs_fastlock);
                        return( PON_ERROR_TCONT_TABLE_IS_FULL);
                    }


                    gpon_error = gpon_rx_set_alloc_id_to_tcont_association(tcont_index, CS_DEFAULT_TCONT_ID);
                    if (gpon_error != PON_NO_ERROR)
                    {
                        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                           "Unable to set tcont id %d to alloc index %d.", CS_DEFAULT_TCONT_ID, tcont_index);
                       return gpon_error;
                    }

                    gs_gpon_database.tconts_table[tcont_index].config_tcont_valid_flag = BDMF_FALSE;
                    gs_gpon_database.tconts_table[tcont_index].tcont_id = CS_DEFAULT_TCONT_ID;
                    gs_gpon_database.tconts_table[empty_index].tcont_id = xi_tcont_id;
                    gs_gpon_database.tconts_table[empty_index].config_tcont_valid_flag = BDMF_TRUE;
                    gs_gpon_database.tconts_table[empty_index].alloc_id = xi_new_alloc_id;
                    gs_gpon_database.tconts_table[empty_index].assign_alloc_valid_flag = BDMF_FALSE;

                }

            }
        }
        else
        {
            /* ERROR - The tcont is not apper in the table */
            p_log(ge_onu_logger.sections.stack.debug.general_id, 
                "Error: The Tcont is not configured yet");
            bdmf_fastlock_unlock(&gs_fastlock);
            return PON_ERROR_TCONT_NOT_ASSIGNED;
        }

        /* Enable interrupt */
        bdmf_fastlock_unlock(&gs_fastlock);

        /* Check Error */
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Failed to set Modify Tcont configuration (%d)", gpon_error);
            return gpon_error;
        }
    }

    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_remove_tcont(PON_TCONT_ID_DTE xi_tcont_id)
{
    uint32_t index;
    PON_ERROR_DTE gpon_error = 0;
    uint32_t tcont_index = 0;
    bool tcont_found_in_table = BDMF_FALSE;

    /* Varify that the stack is initialized */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Check the validity of the parameters together with the queue
       size parameter refering to the rest of the queue configureations */
    if (!PON_TCONT_ID_DTE_IN_RANGE(xi_tcont_id))
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    p_log(ge_onu_logger.sections.stack.debug.general_id, "Remove Tcont Id %d!", xi_tcont_id);

    /* Search in the Tcont table */
    /* Search if this tcont-id and alloc-id are anywhere in the table */
    for (index = 0; index < MAX_NUMBER_OF_TCONTS; index++)
    {
        if (gs_gpon_database.tconts_table[index].config_tcont_valid_flag == BDMF_TRUE)
        {
            if (gs_gpon_database.tconts_table[index].tcont_id == xi_tcont_id)
            {
                tcont_found_in_table = BDMF_TRUE;
                tcont_index = index;
            }

        }
    }

    if (tcont_found_in_table != BDMF_TRUE)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "ERROR: Tcont %d not found(as valid) in tcont table", xi_tcont_id);
        return PON_GENERAL_ERROR;
    }
    else
    {
        /* Disable interrupt to prevent race between ISR and API */
        bdmf_fastlock_lock(&gs_fastlock);

        /* if the tcont is not already apper in the table - return error */
        if (tcont_found_in_table == BDMF_TRUE)
        {
            if (gs_gpon_database.tconts_table[tcont_index].config_tcont_valid_flag == BDMF_FALSE )
            {
                p_log(ge_onu_logger.sections.stack.debug.general_id, 
                    "Error: The Tcont is not configured yet");
                bdmf_fastlock_unlock(& gs_fastlock);
                return PON_ERROR_TCONT_NOT_ASSIGNED;
            }
            else
            {
               gs_gpon_database.tconts_table[tcont_index].tcont_id = CS_DEFAULT_TCONT_ID;
               gs_gpon_database.tconts_table[tcont_index].config_tcont_valid_flag = BDMF_FALSE;

               if (gs_gpon_database.tconts_table[tcont_index].assign_alloc_valid_flag == BDMF_TRUE)
               {
                    /* change the tcont id in the HW table to Default Tcont(0) */
                    gpon_error = gpon_rx_set_alloc_id_to_tcont_association(tcont_index, CS_DEFAULT_TCONT_ID);
                    if (gpon_error != PON_NO_ERROR)
                    {
                        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                           "Unable to set tcont id %d to alloc index %d.", CS_DEFAULT_TCONT_ID, tcont_index);
                       return gpon_error;
                    }
               }
               else
               {
                   gs_gpon_database.tconts_table[tcont_index].alloc_id = PON_ALLOC_ID_DTE_NOT_ASSIGNED;
               }
            }
        }

        /* Enable interrupt */
        bdmf_fastlock_unlock(&gs_fastlock);

        /* Check Error */
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Failed to set Tcont configuration (%d)", gpon_error);
            return gpon_error;
        }
    }

    return PON_NO_ERROR;
}


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
    PON_FLOW_PRIORITY_DTE xi_flow_priority, bool crcEnable)
{
    uint32_t flow_index;
    PON_CONFIGURE_FILTER_INDICATION filter_configuration;
    PON_ERROR_DTE gpon_error;

    /* Varify that the stack is initialized */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    if (!PON_FLOW_ID_DTE_IN_RANGE(xi_flow_id) || 
        xi_port_id_filter < GPON_PORT_ID_LOW || xi_port_id_filter > GPON_PORT_ID_HIGH)
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    /* Check if this port-id already configured in other flow instance */
    for (flow_index = 0; flow_index < MAX_NUMBER_OF_FLOWS; flow_index++)
    {
        if (gs_gpon_database.flows_table[flow_index].port_id == xi_port_id_filter)
        {
            if ((flow_index != xi_flow_id) &&(gs_gpon_database.flows_table[flow_index].configure_flag != BDMF_FALSE))
            {
                /* Found port id in other flow table instance */
                return PON_ERROR_PORT_ID_ALREADY_CFG;
            }
        }
    }

    /* Build the filter configuration struct */
    filter_configuration.flow_id = xi_flow_id;
    filter_configuration.encryption_mode = xi_encryption_mode;
    filter_configuration.flow_priority = xi_flow_priority;
    filter_configuration.flow_type = xi_flow_type;
    filter_configuration.port_id = xi_port_id_filter;
    filter_configuration.port_mask_id = xi_port_id_mask;

    gpon_error = f_set_filter_configuration(filter_configuration);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Cannot set filter configuration %d!", gpon_error);
        return gpon_error;
    }

    return PON_NO_ERROR;
}

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
    bool xi_encryption_mode)
{
    PON_CONFIGURE_FILTER_INDICATION filter_configuration;
    PON_ERROR_DTE gpon_error;

    /* Varify that the stack is initialized */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    if (!PON_FLOW_ID_DTE_IN_RANGE(xi_flow_id))
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    p_log(ge_onu_logger.sections.stack.debug.general_id, "Configure filter id %d!", xi_flow_id);

    /* Build the filter configuration struct */
    filter_configuration.flow_id = xi_flow_id;
    filter_configuration.encryption_mode = xi_encryption_mode;

    /* Modify encryption setting of the referenced Port-Id */
    gpon_error = f_encryption_filter_configuration(&filter_configuration);
    if (gpon_error != PON_NO_ERROR)
    {
        return gpon_error;
    }

    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_control_port_id_filter(PON_FLOW_ID_DTE xi_flow_id, bool xi_filter_status)
{
    PON_CTRL_FILTER_INDICATION filter_control;
    PON_ERROR_DTE gpon_error;

    /* Varify that the stack is initialized */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    if (!PON_FLOW_ID_DTE_IN_RANGE(xi_flow_id))
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    p_log(ge_onu_logger.sections.stack.debug.general_id, 
        "Control port id filter %d status %d!", xi_flow_id , xi_filter_status);

    /* Build the filter configuration struct */
    filter_control.flow_id = xi_flow_id;
    filter_control.configure_flag = xi_filter_status;

    /* Enable/Disable Port-Id filter entry */
    gpon_error = f_set_filter_control(& filter_control);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set port id filter entry : error %d !", gpon_error);
        return gpon_error;
    }

    return PON_NO_ERROR;
}

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
    bool *xo_crc_enable)
{
    /* Varify that the stack is initialized */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    * xo_filter_status = gs_gpon_database.flows_table[xi_flow_id].configure_flag;
    * xo_port_id_filter = gs_gpon_database.flows_table[xi_flow_id].port_id;
    * xo_port_id_mask = gs_gpon_database.flows_table[xi_flow_id].port_mask_id;
    * xo_encryption_mode = gs_gpon_database.flows_table[xi_flow_id].encryption_mode;
    * xo_flow_type = gs_gpon_database.flows_table[xi_flow_id].flow_type;
    * xo_flow_priority = gs_gpon_database.flows_table[xi_flow_id].flow_priority;
    * xo_crc_enable = BDMF_TRUE;

    return PON_NO_ERROR;
}

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
    uint16_t xi_tx_max_queue_size)
{
    int drv_error;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    if (!TXPON_FIFO_ID_DTE_IN_RANGE(xi_txfifo_id))
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

     p_log(ge_onu_logger.sections.stack.debug.general_id,
         "txfifo_id %d txfifo_id %d tx_bd_offset %d bd_number %d tx_queue_offset %d queue_size %d ",
         xi_packet_descriptor_id, xi_txfifo_id, xi_packet_descriptor_queue_base_address,
         xi_packet_descriptor_queue_size, xi_tx_queue_offset, xi_tx_max_queue_size);

     drv_error = ag_drv_gpon_tx_fifo_configuration_pdp_set(xi_packet_descriptor_id, 
         xi_packet_descriptor_queue_base_address, xi_packet_descriptor_queue_size);
     if (drv_error > 0)
     {
         p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
             "Unable to set packet descriptor configuration due to driver error %d", drv_error);
         return PON_ERROR_DRIVER_ERROR;
     }

     drv_error = ag_drv_gpon_tx_fifo_configuration_tqp_set(xi_txfifo_id, xi_tx_queue_offset, 
                                                              xi_tx_max_queue_size);
     if (drv_error > 0)
    {
         p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
             "Unable to set fifo configuration due to driver error %d", drv_error);
         return PON_ERROR_DRIVER_ERROR;
    }

     return PON_NO_ERROR;
}


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
    uint16_t *const xo_group_offset, uint16_t *const xo_group_size)
{
    int16_t group_size;
    int drv_error;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    if (!ME_GPON_TCONT_FRONT_END_BUFFER_GROUP_ID_DTE_IN_RANGE(xi_group_id))
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    drv_error = ag_drv_gpon_tx_fifo_configuration_pdpgrp_get((uint32_t)xi_group_id,
        xo_packet_descriptor_group_base_address);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read txfifo group packet descriptor configuration due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_fifo_configuration_tdqpgrp_get((uint32_t)xi_group_id, 
        xo_group_offset, &group_size);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read txfifo group configuration due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* TX FIFO group size(units of 4 bytes) */
    * xo_group_size = group_size * 4;

    return PON_NO_ERROR;
}

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
    uint16_t *const xo_tx_queue_size)
{
    int drv_error;
    uint16_t tx_queue_size;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Read queue configuration driver */
    if (xi_txfifo_id > CS_TXPON_FIFO_UNIT_HIGH_FOR_GROUP_0)
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    drv_error = ag_drv_gpon_tx_fifo_configuration_pdp_get(xi_packet_descriptor_id, 
        xo_packet_descriptor_queue_base_address, xo_packet_descriptor_queue_size);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read txfifo group configuration due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_fifo_configuration_tqp_get(xi_txfifo_id, 
        xo_tx_queue_offset, &tx_queue_size);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read txfifo group configuration due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* TX FIFO queue size(units of 4 bytes) */
    * xo_tx_queue_size = tx_queue_size * 4;

    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_flush_queue()
{
    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Flush queue !!! driver */

    /* flush tx base level normal,urgent queue of PLOAM messages.                              */
    /* it may occure that PLOAM messages are waiting on the queues for TX buffer to be freed. */
    if (f_gpon_txpon_flush_queue() != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Fail to flush queue");
    }

    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_set_loopback(bool xi_loopback_status, GPON_QUEUE_ID_DTE xi_loopback_queue)
{
    gpon_tx_gen_ctrl_ten tx_params; 
    int drv_error;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Link already activated? */
    if (gs_gpon_database.link_parameters.operation_state_machine.activation_state == LINK_STATE_ACTIVE)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Set loopback */
    drv_error = ag_drv_gpon_tx_fifo_configuration_lbp_set(xi_loopback_queue);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set Tx loopback due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_gen_ctrl_ten_get(&tx_params);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read tx paramters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    tx_params.loopben = xi_loopback_status;

    drv_error = ag_drv_gpon_tx_gen_ctrl_ten_set(&tx_params);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure tx paramters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* RX Disable */
    gs_gpon_database.rx_parameters.rcvr_config.rx_disable = BDMF_TRUE; /* Disable */
    gs_gpon_database.rx_parameters.rcvr_config.loopback_enable = xi_loopback_status;
    drv_error = ag_drv_gpon_rx_general_config_rcvr_config_set(&gs_gpon_database.rx_parameters.rcvr_config);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure Rx paramters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


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
    PON_ALLOC_ID_DTE xi_alloc_id_4)
{
    uint8_t alloc_index_array[GPON_RX_ALLOCS_IN_ASSOCIATION_NUM];
    uint16_t alloc_user_array[GPON_RX_ALLOCS_IN_ASSOCIATION_NUM];
    uint8_t  array_index;
    uint8_t  table_index;
    bool alloc_id_found = BDMF_FALSE;
    int drv_error;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Validate parameters */
    if (!PON_ALLOC_ID_DTE_IN_RANGE( xi_alloc_id_1) || !PON_ALLOC_ID_DTE_IN_RANGE( xi_alloc_id_2) ||
        !PON_ALLOC_ID_DTE_IN_RANGE( xi_alloc_id_3) || !PON_ALLOC_ID_DTE_IN_RANGE( xi_alloc_id_4) )
    {
       return PON_ERROR_ALLOC_NOT_IN_RANGE;
    }

    alloc_user_array[0]= xi_alloc_id_1;
    alloc_user_array[1]= xi_alloc_id_2;
    alloc_user_array[2]= xi_alloc_id_3;
    alloc_user_array[3]= xi_alloc_id_4;

    for(array_index = 0; array_index < GPON_RX_ALLOCS_IN_ASSOCIATION_NUM; array_index++ )
    {
        if (alloc_user_array[array_index]== PON_ALLOC_ID_DTE_NOT_ASSIGNED )
        {
            alloc_index_array[array_index]= ALLOC_ID_INDEX_MAX;
        }
        else
        {
            /* Search for the alloc id in the alloc table */
            for (table_index = 0; table_index < MAX_NUMBER_OF_TCONTS; table_index++)
            {
                if (gs_gpon_database.tconts_table[table_index].alloc_id == alloc_user_array[array_index])
                {
                    alloc_id_found = BDMF_TRUE;
                    alloc_index_array[array_index]= table_index;
                    break;
                }
            }

            if (!alloc_id_found)
            {
                return PON_ERROR_ALLOC_NOT_ASSIGNED;
            }
        }
    }

    drv_error = ag_drv_gpon_rx_alloc_id_bw_cnt_assoc_set(alloc_index_array[0], 
        alloc_index_array[1], alloc_index_array[2], alloc_index_array[3]);
    if (drv_error > 0)
    {
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;

}

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
    PON_ALLOC_ID_DTE *const xo_alloc_id_4)
{
    uint8_t alloc_index_array[GPON_RX_ALLOCS_IN_ASSOCIATION_NUM];
    uint16_t alloc_user_array[GPON_RX_ALLOCS_IN_ASSOCIATION_NUM];
    uint8_t  array_index;
    uint8_t  table_index;
    int drv_error;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    drv_error = ag_drv_gpon_rx_alloc_id_bw_cnt_assoc_get(&alloc_index_array[0], 
        &alloc_index_array[1], &alloc_index_array[2], &alloc_index_array[3]); 
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read bw cnt association bit due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    for (array_index = 0; array_index < GPON_RX_ALLOCS_IN_ASSOCIATION_NUM; array_index++)
    {
        if (alloc_index_array[array_index]== ALLOC_ID_INDEX_MAX)
        {
            alloc_user_array[array_index]= PON_ALLOC_ID_DTE_NOT_ASSIGNED;
        }
        else
        {
            table_index = alloc_index_array[array_index];
            alloc_user_array[array_index]= gs_gpon_database.tconts_table[table_index].alloc_id;
        }
    }

    * xo_alloc_id_1 = alloc_user_array[0];
    * xo_alloc_id_2 = alloc_user_array[1];
    * xo_alloc_id_3 = alloc_user_array[2];
    * xo_alloc_id_4 = alloc_user_array[3];

    return PON_NO_ERROR;
}


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
    uint32_t xi_update_interval)
{
    PON_ERROR_DTE gpon_error;
    int timer_error;
    PON_FIFO_ID_DTE tcont_id;

    /* verify that DBA SR is currently disabled*/
    if (gs_gpon_database.dbr_parameters.dbr_status != BDMF_FALSE)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
               "Error: DBA SR is already enabled");
        return PON_DBR_ALREADY_ENABLED;
    }

    /* verify that the RT HW timer has been initialized*/
    if (gs_gpon_database.dbr_parameters.dba_sr_timer_initiated != BDMF_TRUE)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
               "Error: RT HW timer has not been initialized yet");
        return PON_HW_TIMER_IS_NOT_INITIALIZED;
    }

    /* clear normal DBRu HW arrays */
    for (tcont_id = TXPON_FIFO_UNIT_LOW; tcont_id <= TXPON_FIFO_UNIT_HIGH; tcont_id++)
    {
        gpon_error = gpon_tx_set_normal_dbr_content(tcont_id, GPON_SR_DBR_INVALID);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Error: Unable to clear normal DBRu id %d due to driver error %d",
                tcont_id, gpon_error);
            return gpon_error;
        }
    }

    /* update dbr parameters and status */
    gs_gpon_database.dbr_parameters.dba_sr_reporting_block = xi_sr_reporting_block;
    gs_gpon_database.dbr_parameters.sr_dba_interval = xi_update_interval;

     /* since the units of the provided update interval are milliseconds */
     /* (1/2 msec and up ) and the resolution of the RT HW timer is in */
     /* microseconds we should convert the update interval to the units */
     /* resolution of the RT HW timer. */
    if (xi_update_interval > CS_0_5_MILISEC_SR_DBA_POLLING_INTERVAL && xi_update_interval < CS_125_MICROSEC) 
    {
        gs_gpon_database.dbr_parameters.sr_dba_interval_microsec = MS_MILISEC_TO_MICROSEC(xi_update_interval);
    }
    else
    {
        /* interval is 1/2 milisec */
        if (xi_update_interval == CS_0_5_MILISEC_SR_DBA_POLLING_INTERVAL)
        {
             gs_gpon_database.dbr_parameters.sr_dba_interval_microsec = CS_500_MICROSEC;
        }
        /* interval is 125 or 250 microSeconds */
        else
        {
            gs_gpon_database.dbr_parameters.sr_dba_interval_microsec = xi_update_interval;
        }
    }

    /* check whether DBA SR is currently allowed to run (it is allowed to run only if the ONU is in state O5) */
    /* if it is not allowed to run , turn on flag which will start the DBA-SR process, when it becomes        */
    /* allowed to run and return without error                                                                */
    if (gs_gpon_database.dbr_parameters.is_dba_sr_allowed == BDMF_FALSE)
    {
        gs_gpon_database.dbr_parameters.should_start_dba_sr = BDMF_TRUE;
        return PON_NO_ERROR;
    }   

    /* If the timer has not been registered yet, register the timer and start it */
    if (gs_gpon_database.dbr_parameters.timer_num < 0)
    {
        /* allocated timer. driver returns timer number */
        gs_gpon_database.dbr_parameters.timer_num = ext_timer_alloc(-1, 
            gs_gpon_database.dbr_parameters.sr_dba_interval_microsec, &p_dba_sr_process_callback, 
            0 /* do not deed callback params */);
        if (gs_gpon_database.dbr_parameters.timer_num < 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                    "Error: RT hw timer allocation failed with error = %d",
                    gs_gpon_database.dbr_parameters.timer_num);

            return PON_RT_HW_TIMER_ERROR;
        }
    }

    /* Start/restart the timer */
    timer_error = ext_timer_start(gs_gpon_database.dbr_parameters.timer_num);
    if (timer_error < 0 )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Error: Starting RT hw timer failed");
    
        return PON_RT_HW_TIMER_ERROR;
    }

    /* update the actual DBA-SR running status */
    gs_gpon_database.dbr_parameters.dbr_status = BDMF_TRUE;

    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_dba_sr_process_terminate(void)
{
    PON_ERROR_DTE gpon_error;
    int timer_error;
    PON_FIFO_ID_DTE tcont_id;
      
    /* check whether DBA SR is currently allowed to run (it is allowed to run only if the ONU is in state O5) */
    /* if it is not allowed to run , turn off flag to prevent  restarting the DBA SR process, when it becomes */
    /* allowed to run and return without error                                                                */
    if (gs_gpon_database.dbr_parameters.is_dba_sr_allowed == BDMF_FALSE)
    {
        gs_gpon_database.dbr_parameters.should_start_dba_sr = BDMF_FALSE;
        return PON_NO_ERROR;
    }

    /* verify that DBA SR is currently enabled  */
    if ( gs_gpon_database.dbr_parameters.dbr_status == BDMF_FALSE) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
               "Error: DBA SR is not enabled");
        return PON_DBR_NOT_ENABLED;
    }

    /* Stop and free counter if it is registered */
    if (gs_gpon_database.dbr_parameters.timer_num >= 0)
    {
        /* Stop the timer */
        timer_error = ext_timer_stop(gs_gpon_database.dbr_parameters.timer_num);
        if (timer_error < 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                    "Error: Stoping RT HW timer failed");
        
            return PON_RT_HW_TIMER_ERROR;
        }


        /* Free timer. driver returns timer number */
        timer_error = ext_timer_free(gs_gpon_database.dbr_parameters.timer_num);
        if (timer_error < 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                    "Error: Freeing RT HW timer failed");

            return PON_RT_HW_TIMER_ERROR;
        }
    }

    gs_gpon_database.dbr_parameters.timer_num = -1;

    /* Clear normal DBRu HW arrays */
    for (tcont_id = TXPON_FIFO_UNIT_LOW; tcont_id <= TXPON_FIFO_UNIT_HIGH; tcont_id++)
    {
        gpon_error = gpon_tx_set_normal_dbr_content(tcont_id, GPON_SR_DBR_INVALID);
        if (gpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Error: Unable to clear normal DBRu id %d due to driver error %d!",
                     tcont_id, gpon_error);
            return gpon_error;
        }
    }

    /* update the DBA SR status */
    gs_gpon_database.dbr_parameters.dbr_status = BDMF_FALSE;

    return PON_NO_ERROR;
}

static uint8_t p_encode_dbru(uint16_t tokens )
{
    uint16_t  code, shift;
    shift = 0;

    while ((tokens & 0xFF80) && (shift < 7 ))
    {
        tokens = (tokens >> 1);
        shift ++;
    }

    if (shift == 0)
    {
        code = tokens;
    }
    else
    {
        code = (((tokens & 0x003F) >> (shift-1)) | ((0xFF00) >> shift));
    }

    return ((uint8_t)(code & 0xFF));
}


static void p_dba_sr_process_callback(unsigned long xi_user_params_ptr)
{
    PON_ERROR_DTE gpon_error;
    PON_FIFO_ID_DTE tcont_id = TXPON_FIFO_UNIT_LOW;
    /* iterator over SW TCONTs table*/
    uint32_t tconts_iter;
    /* the total amount of occupied data per TCONT*/
    uint32_t dba_sr_data = 0;
    /* the total amount of occupied blocks per TCONT*/
    uint32_t dba_sr_data_blocks = 0;
    /* number of occupied bytes in the TxFIFO queue */
    uint16_t tcont_occupancy = 0;
    /* number of occupied bytes in the TCONT queue within the Runner */
    uint32_t runner_ddr_occupancy = 0;
    /* final number of reported occupied blocks */
    uint8_t dbr_message = 0;
    int drv_error;

    /* verify that DBA SR is still enabled, this is needed in case the callback */
    /* fires  in the middle of stopping DBA status reporting                  */
    if (gs_gpon_database.dbr_parameters.dbr_status == BDMF_FALSE )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
               "DBA SR is already disabled, ignoring timer callback");
        return;
    } 

    /* collect statistics for all TCONTs */
    for (tconts_iter = TXPON_FIFO_UNIT_LOW; tconts_iter <= TXPON_FIFO_UNIT_HIGH; tconts_iter++)
    {
        if (gs_gpon_database.tconts_table[ tconts_iter].assign_alloc_valid_flag != BDMF_FALSE)
        {
            if (gs_gpon_database.tconts_table[tconts_iter].config_tcont_valid_flag != BDMF_FALSE)
            {
                /* get current T-CONT queue Id*/
                tcont_id = gs_gpon_database.tconts_table[ tconts_iter].tcont_id;

                /* gather data from the runner    */
                gs_callbacks.dbr_runner_data_callback(tcont_id, &runner_ddr_occupancy);
                 /* get TxFIFO statistics*/
                drv_error = ag_drv_gpon_tx_fifo_statistics_tqcntr_get(tcont_id, &tcont_occupancy);
                if (drv_error > 0)
                {
                    p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                        "Error: Unable to get tcont id %u statistics due to error:%d",
                        tcont_id, drv_error);
                    tcont_occupancy = 0;
                }

                /* summarize the total number of occupied bytes */
                dba_sr_data = tcont_occupancy + runner_ddr_occupancy;

                /* if data occupancy is smaller than */
                /* GPON_MIN_DBR_SR_DATA_BYTES_NUM bytes set occupancy to zero */
                if (dba_sr_data < GPON_MIN_DBR_SR_DATA_BYTES_NUM)
                {
                    dba_sr_data_blocks = 0;
                }
                else
                {
                    /* compute the occupancy in terms of reporting blocks  */
                    dba_sr_data_blocks = 
                        ((dba_sr_data + gs_gpon_database.dbr_parameters.dba_sr_reporting_block - 1) / 
                        gs_gpon_database.dbr_parameters.dba_sr_reporting_block);
                }
            
                /* encode and build the data - MODE 0 method */
                if (dba_sr_data_blocks > GPON_MAX_DBR_SR_DATA_BLOCKS_NUM)
                {
                    dbr_message = GPON_DBR_MAX_REPORT;
                }
                else
                {
                    dbr_message = p_encode_dbru(dba_sr_data_blocks);
                }

                #ifdef DBA_DEBUG_STATISTICS
                p_gpon_dba_debug_collect_debug_statistics_if_needed(tcont_id,
                    dba_sr_data, tcont_occupancy, runner_ddr_occupancy,
                    dbr_message);
                #endif 
    
                /* set the dbr content according to computed occupancy*/
                gpon_error = gpon_tx_set_normal_dbr_content(tcont_id, dbr_message);
                /* log the error but do not return,                     */
                /* trying to gather occupancy of the rest of the TCONTs */      
                if (PON_NO_ERROR != gpon_error)
                {
                    p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                        "Error: Set normal DBR content failed with error: %d", gpon_error);
                }          

                /* clear local values */
                dba_sr_data = 0;
                dba_sr_data_blocks = 0;
                tcont_occupancy = 0;
                runner_ddr_occupancy = 0;
                dbr_message = 0;
            }
        }
    }

#ifdef DBA_DEBUG_STATISTICS
   if ((ge_dba_debug_statistics_collect == BDMF_TRUE) &&
       (ge_dba_debug_statistics_cycle_counter < ge_dba_debug_statistics_number_of_cycles))
   {
       ++ ge_dba_debug_statistics_cycle_counter;
   }
#endif 

   return;
}


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
PON_ERROR_DTE gpon_set_TO1_auto_restart_mode(GPON_TO1_AUTO_RESTART_MODE_DTE xi_to1_auto_restart)
{
    /* Save to database */
    gs_gpon_database.link_parameters.auto_restart_to1_mode = xi_to1_auto_restart;

    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_set_gem_block_size(PON_GEM_BLOCK_SIZE_DTE xi_gem_block_size)
{
    /* verify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }
 
    /* update the database */
    gs_gpon_database.dbr_parameters.dba_sr_reporting_block = xi_gem_block_size;

    return  PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_get_to1_auto_restart_mode(GPON_TO1_AUTO_RESTART_MODE_DTE *const xo_to1_auto_restart)
{
    /* Check SW stste state */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    * xo_to1_auto_restart = gs_gpon_database.link_parameters.auto_restart_to1_mode;

    return PON_NO_ERROR;
}

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
    PON_GEM_BLOCK_SIZE_DTE xi_gem_block_size)
{
    uint32_t dba_sr_valid_bit1;
    uint8_t dba_sr_valid_bit2;
    uint8_t dbr_divide_rate;
    bdmf_boolean dbr_report_mode;
    bdmf_boolean dbr_flush;
    int drv_error;
    PON_ERROR_DTE gpon_error;

    /* Check GPON stack state */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* read current DBA-SR parameters*/
    drv_error = ag_drv_gpon_tx_gen_ctrl_tdbvld_get(&dba_sr_valid_bit1);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read dbr sr param due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_gen_ctrl_tdbconf_get(&dba_sr_valid_bit2, &dbr_report_mode, 
        &dbr_divide_rate, &dbr_flush);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read dbr sr param due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* if DBA_SR mode is Runner the GEM block should be 32/48/64 bytes*/
    if (xi_update_mode == GPON_SR_MODE_RUNNER)
    {
        gpon_error = f_get_dbr_divide_ratio(gs_gpon_database.dbr_parameters.dba_sr_reporting_block, 
            &dbr_divide_rate);
        if (gpon_error != PON_NO_ERROR)
        {   
            return gpon_error;
        }
        dbr_report_mode = GPON_SR_MODE_RUNNER;
    }
    else
    {
        dbr_report_mode = GPON_SR_MODE_MIPS;
    }

    /* Set the DBA-SR parameters - unchanged parameters are set with their existing values, read before */
    drv_error = ag_drv_gpon_tx_gen_ctrl_tdbvld_set(dba_sr_valid_bit1);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set dbr sr param due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_gen_ctrl_tdbconf_set(dba_sr_valid_bit2, dbr_report_mode, 
        dbr_divide_rate, dbr_flush);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set dbr sr param due to driver error %d.", gpon_error);
        return PON_ERROR_DRIVER_ERROR ;
    }

    /* Save to database */
    gs_gpon_database.dbr_parameters.dbr_sr_mode = xi_update_mode;

    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_set_rogue_onu_detection_params(const PON_TX_ROGUE_ONU_PARAMETERS *xi_rogue_onu_parameters)
{
    int drv_error;

    /* Check GPON stack state */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Set the rogue onu parameters*/
    drv_error = ag_drv_gpon_tx_general_configuration_rogue_onu_en_set(xi_rogue_onu_parameters->rogue_onu_level_clear, 
        xi_rogue_onu_parameters->rogue_onu_diff_clear, xi_rogue_onu_parameters->source_select);
    if (drv_error > 0) 
    { 
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Failed to configure rouge detect parameters due to driver error %d\n", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_general_configuration_rogue_level_time_set(
        xi_rogue_onu_parameters->rogue_onu_level_threshold);
    if (drv_error > 0) 
    { 
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Failed to configure rouge detect parameters due to driver error %d\n", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_general_configuration_rogue_diff_time_set(
        xi_rogue_onu_parameters->rogue_onu_diff_threshold);
    if (drv_error > 0) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Failed to configure rouge detect parameters due to driver error %d\n", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Save to database */
    gs_gpon_database.rogue_onu_detection_parameters.source_indication = xi_rogue_onu_parameters->source_select;
    gs_gpon_database.rogue_onu_detection_parameters.rogue_onu_level_threshold = xi_rogue_onu_parameters->rogue_onu_level_threshold;
    gs_gpon_database.rogue_onu_detection_parameters.rogue_onu_diff_threshold  = xi_rogue_onu_parameters->rogue_onu_diff_threshold;

    return PON_NO_ERROR;

}


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
PON_ERROR_DTE gpon_tx_get_rogue_onu_detection_params(PON_TX_ROGUE_ONU_PARAMETERS *rogue_onu_params)
{
    int drv_error;

    drv_error = ag_drv_gpon_tx_general_configuration_rogue_onu_en_get(
        &rogue_onu_params->rogue_onu_level_clear, &rogue_onu_params->rogue_onu_diff_clear, 
        &rogue_onu_params->source_select);
    if (drv_error > 0) 
    { 
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Failed to read rouge ONU parameters due to driver error %d\n", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
    
    drv_error = ag_drv_gpon_tx_general_configuration_rogue_level_time_get(
        &rogue_onu_params->rogue_onu_level_threshold);        
    if (drv_error > 0) 
    { 
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Failed to read rouge ONU level time due to driver error %d\n", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
    
    drv_error = ag_drv_gpon_tx_general_configuration_rogue_diff_time_get(
        &rogue_onu_params->rogue_onu_diff_threshold);        
    if (drv_error > 0) 
    { 
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Failed to read rouge ONU level time due to driver error %d\n", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


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
    uint16_t xi_phase_difference)
{
    int drv_error;

    /* Check GPON stack state */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Set phase lock */
    drv_error = ag_drv_gpon_rx_clk_8khz_clk_8khz_conf_set(xi_phase_difference, ~xi_enable_phase_difference);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure 8KHZ clock due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Update DB */
    gs_gpon_database.rx_parameters.rx_8khz_clock_lock = xi_enable_phase_difference;
    gs_gpon_database.rx_parameters.rx_8khz_clock_phase_diff = xi_phase_difference;

    return PON_NO_ERROR;
}

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
    bdmf_boolean flush_enable, bdmf_boolean flush_immediate) 
{
    int drv_error; 
    int i;
    bdmf_boolean flush_done;

    drv_error = ag_drv_gpon_tx_general_configuration_flush_set(tcont_id, flush_enable, flush_immediate);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Failed to configure flush parameters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    if (flush_enable) 
    {
        for (i=0; i <= 10000; i++) 
        {
            drv_error = ag_drv_gpon_tx_general_configuration_flshdn_get(&flush_done);
            if (drv_error > 0)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                    "Failed to read flush status due to driver error %d", drv_error);
                return PON_ERROR_DRIVER_ERROR;
            }

            if (flush_done)
                break;

        }

        if (i == 10000) 
            return PON_ERROR_TIME_OUT_FLUSH_IS_NOT_DONE;
    }

    return PON_NO_ERROR;
}


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
    bdmf_boolean *flush_immediate, bdmf_boolean *flush_done)
{
    int drv_error; 

    drv_error = ag_drv_gpon_tx_general_configuration_flush_get(tcont_id, flush_enable, flush_immediate);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Failed to read flush parameters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_general_configuration_flshdn_get(flush_done);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Failed to read flush status due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


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
    bdmf_boolean *fec_state, bdmf_boolean *lcdg_state) 
{
    int drv_error; 
    uint8_t bit_align;

    drv_error = ag_drv_gpon_rx_general_config_rcvr_status_get(lof_state, 
        fec_state, lcdg_state, &bit_align);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read recevier status due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_ber_enable (void)
{
	PON_ERROR_DTE err_gpon;
    int drv_error;
    uint32_t bip;

	/* Clear BIP counter */
    drv_error = ag_drv_gpon_rx_pm_counter_bip_get(&bip); /* Clear-On-Read */
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to read BIP status due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

	err_gpon = p_pon_start_ber_interval( gs_gpon_database.onu_parameters.ber_interval);
	if (err_gpon != PON_NO_ERROR)
	{
    	return PON_GENERAL_ERROR;
	}

    return PON_NO_ERROR;
}


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
PON_ERROR_DTE f_pon_stop_pee_interval(void)
{
    /* stop timer with referenced timeout*/
    bdmf_timer_stop(& gs_gpon_database.os_resources.pee_timer_id);
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop PEE timer");

    /* Turn off the indication */
#ifdef TBD
    link.physical_parameters.pee_status = BDMF_FALSE;
#endif

    return PON_NO_ERROR;
}

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
PON_ERROR_DTE p_pon_start_ber_interval(uint32_t xi_ber_interval)
{
    bdmf_error_t bdmf_error;

    /* validate ber interval */
    if (GPON_MILISEC_TO_TICKS(xi_ber_interval) == 0 )
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, 
            "Error. Can't start timer of %d ticks", GPON_MILISEC_TO_TICKS(xi_ber_interval));
        return PON_ERROR_INVALID_PARAMETER;
    }

    /* save current BER interval. It is not accurate since BER calculation may use new value for actually old timeout ! */
    gs_gpon_database.onu_parameters.ber_interval = xi_ber_interval;

    /* start timer with referenced timeout*/
    bdmf_error = bdmf_timer_start(&gs_gpon_database.os_resources.ber_interval_timer_id, 
        GPON_MILISEC_TO_TICKS(xi_ber_interval));
    if (bdmf_error != BDMF_ERR_OK)
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Fail to Start BER timer");
        return PON_ERROR_WATCHDOG_START;
    }
    else
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start BER timer");
    }


    return PON_NO_ERROR;
}

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
PON_ERROR_DTE f_pon_stop_ber_interval(void)
{
    /* stop timer with referenced timeout*/
    bdmf_timer_stop(& gs_gpon_database.os_resources.ber_interval_timer_id);
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Stop BER timer");

    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_ber_disable(void)
{
	PON_ERROR_DTE err_gpon;

	err_gpon = f_pon_stop_ber_interval();
	if (err_gpon != PON_NO_ERROR )
	{
    	return PON_GENERAL_ERROR;
	}

    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_get_onu_id(uint16_t *const xo_onu_id)
{
    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* ONU ID */
    * xo_onu_id = gs_gpon_database.onu_parameters.onu_id;

    return PON_NO_ERROR;
}


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
    uint8_t *const xo_pon_overhead_repetition)
{
    int drv_error;
    uint32_t word_idx;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    for (word_idx = 0; word_idx < PON_TX_PREAMBLE_OVERHEAD_SIZE_IN_BYTES; word_idx++)
    {
        drv_error = ag_drv_gpon_tx_general_configuration_prod_get(word_idx, &xo_pon_overhead->overhead[word_idx]);
        if (drv_error > 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to read pon overhead due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }

    drv_error = ag_drv_gpon_tx_general_configuration_prods_get(
        xo_pon_overhead_length, xo_pon_overhead_repetition_length);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read pon overhead length and repetition length due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_general_configuration_prrb_get(xo_pon_overhead_repetition);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read pon overhead repetition due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


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
    uint8_t *const xo_transceiver_power_level)
{
    uint32_t i = 0;
    PON_ERROR_DTE gpon_error;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Read ONU information */

    /* Serial number */
    for (i = 0; i < VENDOR_CODE_FIELD_SIZE; i++)
    {
        xo_serial_number->vendor_code[i] = gs_gpon_database.physical_parameters.serial_number.vendor_code[i];
    }

    for (i = 0; i < VENDOR_SPECIFIC_FIELD_SIZE; i++)
    {
        xo_serial_number->serial_number[i] = gs_gpon_database.physical_parameters.serial_number.serial_number[i];
    }

    /* Password */
    for (i = 0; i < PON_PASSWORD_FIELD_SIZE; i++)
    {
        xo_password->password[i] = gs_gpon_database.physical_parameters.password.password[i];
    }

    /* ONU ID */
    *xo_onu_id = gs_gpon_database.onu_parameters.onu_id;

    /* Equalization delay */
    *xo_equalization_delay = gs_gpon_database.onu_parameters.preassigned_equalization_delay;

    /* Downstream BER interval */
    *xo_downstream_ber_interval = gs_gpon_database.onu_parameters.ber_interval;

    /* Pon Overhead */
    gpon_error = gpon_get_overhead_and_length(xo_pon_overhead, xo_pon_overhead_length,
        xo_pon_overhead_repetition_length, xo_pon_overhead_repetition);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read pon patterns");
        return gpon_error;
    }

    /* Current power-level of the optical transceiver */
    *xo_transceiver_power_level = gs_gpon_database.onu_parameters.default_power_level_mode;

    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_get_link_parameters(rdpa_gpon_link_cfg_t *const xo_parameters)
{
    rdpa_gpon_link_cfg_t link_parameters;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Get the Link physical parameters */
    link_parameters.rx_din_polarity = gs_gpon_database.rx_parameters.rcvr_config.din_polarity;
    link_parameters.transceiver_power_calibration_mode = gs_gpon_database.tx_parameters.tx_pls_enable;
    link_parameters.tx_data_polarity = gs_gpon_database.tx_parameters.tx_data_out_polarity;

    link_parameters.transceiver_dv_hold_pattern = gs_gpon_database.physical_parameters.transceiver_dv_hold_pattern;
    link_parameters.transceiver_dv_polarity = gs_gpon_database.physical_parameters.transceiver_dv_polarity;
    link_parameters.transceiver_dv_setup_pattern = gs_gpon_database.physical_parameters.transceiver_dv_setup_pattern;
    link_parameters.transceiver_power_calibration_pattern = gs_gpon_database.physical_parameters.transceiver_power_calibration_pattern;
    link_parameters.transceiver_power_calibration_size = gs_gpon_database.physical_parameters.transceiver_power_calibration_size;
    link_parameters.min_response_time = gs_gpon_database.physical_parameters.min_response_time;
    link_parameters.number_of_psyncs_for_lof_assertion = gs_gpon_database.physical_parameters.number_of_psyncs_for_lof_assertion;
    link_parameters.number_of_psyncs_for_lof_clear = gs_gpon_database.physical_parameters.number_of_psyncs_for_lof_clear;

    link_parameters.ber_interval = gs_gpon_database.onu_parameters.ber_interval;
    link_parameters.to1_timeout = gs_gpon_database.onu_parameters.to1_timer_timeout;
    link_parameters.to2_timeout = gs_gpon_database.onu_parameters.to2_timer_timeout;
    link_parameters.ber_threshold_for_sd_assertion = gs_gpon_database.onu_parameters.ber_threshold_for_sd_assertion;
    link_parameters.ber_threshold_for_sf_assertion = gs_gpon_database.onu_parameters.ber_threshold_for_sf_assertion;

    link_parameters.number_of_psyncs_for_lof_assertion = gs_gpon_database.physical_parameters.number_of_psyncs_for_lof_assertion;
    link_parameters.number_of_psyncs_for_lof_clear = gs_gpon_database.physical_parameters.number_of_psyncs_for_lof_clear;

    /* Set output values */
    *xo_parameters = link_parameters;

    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_get_equalization_delay(uint32_t *const xo_equalization_delay)
{
    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    * xo_equalization_delay = gs_gpon_database.onu_parameters.preassigned_equalization_delay;
    
    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_get_transceiver_power_level(uint8_t *const xo_transceiver_power_level)
{
    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Current power-level of the optical transceiver */
    * xo_transceiver_power_level = gs_gpon_database.onu_parameters.default_power_level_mode;

    return PON_NO_ERROR;
}






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
PON_ERROR_DTE gpon_configure_link_params(rdpa_gpon_link_cfg_t *const xi_link_default_configuration)
{
    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Set the Link physical parameters */
    gs_gpon_database.rx_parameters.rcvr_config.din_polarity      = xi_link_default_configuration->rx_din_polarity;
    gs_gpon_database.tx_parameters.tx_pls_enable        = xi_link_default_configuration->transceiver_power_calibration_mode;
    gs_gpon_database.tx_parameters.tx_data_out_polarity = xi_link_default_configuration->tx_data_polarity;

    gs_gpon_database.physical_parameters.transceiver_dv_hold_pattern           = xi_link_default_configuration->transceiver_dv_hold_pattern;
    gs_gpon_database.physical_parameters.transceiver_dv_polarity               = xi_link_default_configuration->transceiver_dv_polarity;
    gs_gpon_database.physical_parameters.transceiver_dv_setup_pattern          = xi_link_default_configuration->transceiver_dv_setup_pattern;
    gs_gpon_database.physical_parameters.transceiver_power_calibration_pattern = xi_link_default_configuration->transceiver_power_calibration_pattern;
    gs_gpon_database.physical_parameters.transceiver_power_calibration_size    = xi_link_default_configuration->transceiver_power_calibration_size;
    gs_gpon_database.physical_parameters.min_response_time                     = xi_link_default_configuration->min_response_time;
    gs_gpon_database.physical_parameters.number_of_psyncs_for_lof_assertion    = xi_link_default_configuration->number_of_psyncs_for_lof_assertion;
    gs_gpon_database.physical_parameters.number_of_psyncs_for_lof_clear        = xi_link_default_configuration->number_of_psyncs_for_lof_clear;

    gs_gpon_database.onu_parameters.ber_interval                   = xi_link_default_configuration->ber_interval;
    gs_gpon_database.onu_parameters.to1_timer_timeout              = xi_link_default_configuration->to1_timeout;
    gs_gpon_database.onu_parameters.to2_timer_timeout              = xi_link_default_configuration->to2_timeout;
    gs_gpon_database.onu_parameters.ber_threshold_for_sd_assertion = xi_link_default_configuration->ber_threshold_for_sd_assertion;
    gs_gpon_database.onu_parameters.ber_threshold_for_sf_assertion = xi_link_default_configuration->ber_threshold_for_sf_assertion;


    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_configure_onu_serial_number(PON_SERIAL_NUMBER_DTE xi_serial_number)
{
    uint32_t i;

    /* Link already activated? */
    if (gs_gpon_database.link_parameters.operation_state_machine.link_state == LINK_STATE_ACTIVE)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Copy serial number */
      for (i = 0; i < VENDOR_CODE_FIELD_SIZE; i++)
    {
        gs_gpon_database.physical_parameters.serial_number.vendor_code[i] = xi_serial_number.vendor_code[i];
    }

      for (i = 0; i < VENDOR_SPECIFIC_FIELD_SIZE; i++ )
    {
        gs_gpon_database.physical_parameters.serial_number.serial_number[i] = xi_serial_number.serial_number[i];
    }

    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_configure_onu_password(PON_PASSWORD_DTE xi_onu_password)
{
    uint32_t i;

    /* Link already activated? */
    if (gs_gpon_database.link_parameters.operation_state_machine.link_state == LINK_STATE_ACTIVE)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Copy password */
    for (i = 0; i < PON_PASSWORD_FIELD_SIZE; i++)
    {
        gs_gpon_database.physical_parameters.password.password[i]= xi_onu_password.password[i];
    }

    return PON_NO_ERROR;
}

/*
 Two empty callbacks for compatability with NGPON2 stack
*/
PON_ERROR_DTE gpon_set_multicast_encryption_key(uint32_t key_index, uint8_t* encryption_key)
{
   return 0;
}

void gpon_set_stack_mode (PON_MAC_MODE stack_mode)
{

}


void gpon_register_cb(PON_SM_CALLBACK_FUNC *p_pon_sm_cb)
{
    p_pon_sm_cb->pon_assign_user_callback = gpon_assign_user_callback;
    p_pon_sm_cb->pon_configure_onu_serial_number = gpon_configure_onu_serial_number;
    p_pon_sm_cb->pon_configure_onu_password = gpon_configure_onu_password;
    p_pon_sm_cb->pon_link_activate = gpon_link_activate;
    p_pon_sm_cb->pon_set_gem_block_size = gpon_set_gem_block_size;
    p_pon_sm_cb->pon_dba_sr_process_initialize = gpon_dba_sr_process_initialize;
    p_pon_sm_cb->pon_link_deactivate = gpon_link_deactivate;
    p_pon_sm_cb->pon_configure_link_params = gpon_configure_link_params;
    p_pon_sm_cb->pon_send_dying_gasp_message = gpon_send_dying_gasp_message;
    p_pon_sm_cb->pon_send_pee_message = gpon_send_pee_message;
    p_pon_sm_cb->pon_send_pst_message = gpon_send_pst_message;
    p_pon_sm_cb->pon_dba_sr_process_terminate = gpon_dba_sr_process_terminate;
    p_pon_sm_cb->pon_control_port_id_filter = gpon_control_port_id_filter;
    p_pon_sm_cb->pon_modify_encryption_port_id_filter = gpon_modify_encryption_port_id_filter;
    p_pon_sm_cb->pon_configure_port_id_filter = gpon_configure_port_id_filter;
    p_pon_sm_cb->pon_configure_tcont = gpon_configure_tcont;
    p_pon_sm_cb->pon_remove_tcont = gpon_remove_tcont;
    p_pon_sm_cb->pon_link_reset = gpon_link_reset;
    p_pon_sm_cb->pon_mask_rx_pon_interrupts = gpon_mask_rx_pon_interrupts;
    p_pon_sm_cb->pon_mask_tx_pon_interrupts = gpon_mask_tx_pon_interrupts;
    p_pon_sm_cb->pon_isr_handler = gpon_isr;
    p_pon_sm_cb->pon_unmask_tx_pon_interrupts = gpon_unmask_tx_pon_interrupts;
    p_pon_sm_cb->pon_unmask_rx_pon_interrupts = gpon_unmask_rx_pon_interrupts;
    p_pon_sm_cb->pon_tx_set_tcont_to_counter_group_association = gpon_tx_set_tcont_to_counter_group_association;
    p_pon_sm_cb->pon_tx_set_flush_valid_bit = gpon_tx_set_flush_valid_bit;
    p_pon_sm_cb->pon_tx_generate_misc_transmit = gpon_tx_generate_misc_transmit;
    p_pon_sm_cb->pon_get_link_status = gpon_get_link_status;
    p_pon_sm_cb->pon_get_link_parameters = gpon_get_link_parameters;
    p_pon_sm_cb->pon_get_onu_id = gpon_get_onu_id;
    p_pon_sm_cb->pon_get_overhead_and_length = gpon_get_overhead_and_length;
    p_pon_sm_cb->pon_get_transceiver_power_level = gpon_get_transceiver_power_level;
    p_pon_sm_cb->pon_get_equalization_delay = gpon_get_equalization_delay;
    p_pon_sm_cb->pon_get_aes_encryption_key = gpon_get_aes_encryption_key;
    p_pon_sm_cb->pon_get_bip_error_counter = gpon_get_bip_error_counter;
    p_pon_sm_cb->pon_get_rxpon_ploam_counters = gpon_get_rx_ploam_counters;
    p_pon_sm_cb->pon_get_rx_unkonw_ploam_counter = gpon_get_rx_unkonw_ploam_counter;
    p_pon_sm_cb->pon_get_txpon_pm_counters = _gpon_get_tx_pm_counters;
    p_pon_sm_cb->pon_get_port_id_configuration = gpon_get_port_id_configuration;
    p_pon_sm_cb->pon_get_encryption_error_counter = gpon_get_encryption_error_counter;
    p_pon_sm_cb->pon_get_tcont_configuration = gpon_get_tcont_configuration;
    p_pon_sm_cb->pon_get_tcont_queue_pm_counters = gpon_get_tcont_queue_pm_counters;
    p_pon_sm_cb->pon_get_ploam_crc_error_counter = gpon_get_ploam_crc_error_counter;
    p_pon_sm_cb->pon_rx_get_receiver_status = gpon_rx_get_receiver_status;
    p_pon_sm_cb->pon_rx_get_fec_counters = gpon_rx_get_fec_counters;
    p_pon_sm_cb->pon_rx_get_hec_counters = gpon_rx_get_hec_counters;
    p_pon_sm_cb->pon_tx_get_flush_valid_bit = gpon_tx_get_flush_valid_bit;
    p_pon_sm_cb->pon_get_pon_id_parameters = gpon_get_pon_id_parameters;
    p_pon_sm_cb->pon_start_bw_recording = gpon_start_bw_recording;
    p_pon_sm_cb->pon_stop_bw_recording = gpon_stop_bw_recording;
    p_pon_sm_cb->pon_get_bw_recording_data = gpon_read_bw_record;
    p_pon_sm_cb->pon_get_burst_prof = gpon_get_burst_prof;
    p_pon_sm_cb->rogue_onu_detect_cfg = rogue_onu_detect_cfg;
    p_pon_sm_cb->set_multicast_encryption_key = gpon_set_multicast_encryption_key;
    p_pon_sm_cb->set_stack_mode = gpon_set_stack_mode;
    p_pon_sm_cb->calc_ds_omci_crc_or_mic = gpon_calc_omci_crc;
    p_pon_sm_cb->calc_us_omci_crc_or_mic = gpon_calc_omci_crc;

#ifdef USE_BDMF_SHELL
    p_pon_sm_cb->pon_modify_tcont = gpon_modify_tcont;
    p_pon_sm_cb->initialize_drv_pon_stack_shell = gpon_initialize_drv_pon_stack_shell;
    p_pon_sm_cb->exit_drv_pon_stack_shell = gpon_exit_drv_pon_stack_shell;
#endif

#ifdef CONFIG_BCM_GPON_TODD
    p_pon_sm_cb->pon_todd_set_tod_info = pon_tod_set_tod_info;
    p_pon_sm_cb->pon_todd_get_tod_info = pon_tod_get_tod_info;
    p_pon_sm_cb->pon_todd_reg_1pps_start_cb = pon_tod_reg_1pps_start_cb;
    p_pon_sm_cb->pon_todd_get_tod = pon_tod_get_tod;   
    p_pon_sm_cb->pon_tod_get_sfc = pon_tod_get_sfc;   
#endif
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_set_filter_configuration                                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE f_set_filter_configuration(PON_CONFIGURE_FILTER_INDICATION configure_filter_struct )
{
    PON_ERROR_DTE gpon_error;
    int drv_error;

    /* Update Rx Flows table */
    gs_gpon_database.flows_table[configure_filter_struct.flow_id].encryption_mode = configure_filter_struct.encryption_mode;
    gs_gpon_database.flows_table[configure_filter_struct.flow_id].flow_id = configure_filter_struct.flow_id;
    gs_gpon_database.flows_table[configure_filter_struct.flow_id].port_id = configure_filter_struct.port_id;
    gs_gpon_database.flows_table[configure_filter_struct.flow_id].port_mask_id = configure_filter_struct.port_mask_id;

    if (configure_filter_struct.flow_id > FLOW_ID_HIGH)
    {
        if (configure_filter_struct.flow_priority != PON_FLOW_PRIORITY_LOW || 
            configure_filter_struct.flow_type != PON_FLOW_TYPE_ETH) 
        {
            return PON_ERROR_INVALID_PARAMETER;
        }
    }

    gs_gpon_database.flows_table[configure_filter_struct.flow_id].flow_priority = configure_filter_struct.flow_priority;
    gs_gpon_database.flows_table[configure_filter_struct.flow_id].flow_type = configure_filter_struct.flow_type;

    gpon_error = gpon_rx_set_flow_cfg(gs_gpon_database.flows_table[configure_filter_struct.flow_id].flow_id,
        gs_gpon_database.flows_table[configure_filter_struct.flow_id].port_id,
        gs_gpon_database.flows_table[configure_filter_struct.flow_id].encryption_mode,
        gs_gpon_database.flows_table[configure_filter_struct.flow_id].flow_type,
        gs_gpon_database.flows_table[configure_filter_struct.flow_id].flow_priority);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set flow id configuration : error %d !", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Only if the flow id to configure is 31 then configure its mask
       (this flow can be configured to couple of ports) */
    if (configure_filter_struct.flow_id == 31)
    {
        /*
         *   RDPA layer sets .port_mask_id unconditionally to
         *   GPON_PORT_ID_UNASSIGNED ((PON_PORT_ID_DTE) 0xffff) - this is most common case
         *   the driver rejects values above _12BITS_MAX_VAL_  
         *
         *   Actually port mask can be updated only when the port is disabled
         */
        if (!gs_gpon_database.flows_table[configure_filter_struct.flow_id].configure_flag &&
            (gs_gpon_database.flows_table[configure_filter_struct.flow_id].flow_id == FLOW_ID_UNASSIGNED) &&
            (gs_gpon_database.flows_table[configure_filter_struct.flow_id].port_mask_id != GPON_PORT_ID_UNASSIGNED))
        {
            drv_error = ag_drv_gpon_rx_port_id_mask_set(
               (uint16_t)gs_gpon_database.flows_table[configure_filter_struct.flow_id].port_mask_id);
            if (drv_error > 0)
            {
               p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                   "Unable to set flow id mask : error %d !", gpon_error);
               return PON_ERROR_DRIVER_ERROR;
            }
        }
    }

    gpon_error = gpon_rx_set_flow_status(gs_gpon_database.flows_table[configure_filter_struct.flow_id].flow_id,
        gs_gpon_database.flows_table[configure_filter_struct.flow_id].configure_flag);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set flow id status: error %d", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}
 
static PON_ERROR_DTE misc_cfg(uint8_t prodc[16], uint8_t prcl, uint8_t brc, uint8_t prl,
    uint16_t msstart, uint16_t msstop, gpon_tx_gen_ctrl_ten tx_status)
{
    int drv_error; 
    int i;

    /* Set preamble overhead */
    for (i = 0; i < PON_TX_PREAMBLE_OVERHEAD_SIZE_IN_BYTES; i++)
    {
        drv_error = ag_drv_gpon_tx_general_configuration_prod_set(i, prodc[i]);
        if (drv_error > 0) 
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to set preamble patteren due to driver error %d", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }
    
    /* Configure Preamble Overhead Specification */
    drv_error = ag_drv_gpon_tx_general_configuration_prods_set(prcl, prl);
    if (drv_error > 0) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to configure preamble overhaed due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
    
    /* Set repetition byte content */
    drv_error = ag_drv_gpon_tx_general_configuration_prrb_set(brc);
    if (drv_error > 0) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver return Error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_general_configuration_miscac1_set(msstop, msstart);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set misc configuration due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
        
    /* Configure Tx parameters */
    drv_error = ag_drv_gpon_tx_gen_ctrl_ten_set(&tx_status);
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
/*   gpon_tx_generate_misc_transmit                                           */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON_Tx - Generate MISC transmit                                         */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   The function configure preamble overhead and generates MISC transmit.    */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_tx_generate_misc_transmit(uint8_t prodc[16], uint8_t prcl,
    uint8_t brc, uint8_t prl, uint16_t msstart, uint16_t msstop, bdmf_boolean enable)
{ 
    int drv_error; 
    gpon_tx_gen_ctrl_ten tx_status = {};
    static gpon_tx_gen_ctrl_ten def_tx_status;
    static uint8_t def_prodc[16];
    static uint8_t def_prcl;
    static uint8_t def_prl;
    static uint8_t def_brc;
    static uint16_t def_sstart;
    static uint16_t def_sstop;
    uint16_t transceiver;
    int i;
    int rc;

    BpGetGponOpticsType(&transceiver);

    if (enable)
    {
        /* Save old configuration */
        for (i = 0; i < PON_TX_PREAMBLE_OVERHEAD_SIZE_IN_BYTES; i++)
        {
            drv_error = ag_drv_gpon_tx_general_configuration_prod_get(i, &def_prodc[i]);
            if (drv_error > 0)
            {
                p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to read pon overhead due to driver error %d.", drv_error);
                return PON_ERROR_DRIVER_ERROR;
            }
        }

        drv_error = ag_drv_gpon_tx_general_configuration_prods_get(&def_prcl, &def_prl);
        if (drv_error > 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to read pon overhead length and repetition length due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        drv_error = ag_drv_gpon_tx_general_configuration_prrb_get(&def_brc);
        if (drv_error > 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to read pon overhead repetition due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        drv_error = ag_drv_gpon_tx_general_configuration_miscac1_get(&def_sstop, &def_sstart);
        if (drv_error > 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to read misc configuration due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        drv_error = ag_drv_gpon_tx_gen_ctrl_ten_get(&def_tx_status);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Cannot read transmit enable register due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        tx_status.misctxen = 1;
        tx_status.txen = 1;
        tx_status.scren = 1;

        rc = misc_cfg(prodc, prcl, brc, prl, msstart, msstop, tx_status);
        if (rc) 
            return rc;
    }
    else
    {
        /* Restore default */
        rc = misc_cfg(def_prodc, def_prcl, def_brc, def_prl, 
            def_sstart, def_sstop, def_tx_status);
        if (rc) 
            return rc;
    }

    if (transceiver == BP_GPON_OPTICS_TYPE_PMD)
    {
        pmd_dev_enable_prbs_or_misc_mode(enable, 0);
    }

    return PON_NO_ERROR;
}



/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_encryption_filter_configuration                                        */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE f_encryption_filter_configuration(PON_CONFIGURE_FILTER_INDICATION *configure_filter_struct )
{
    PON_ERROR_DTE gpon_error;

    /* Update Rx Flows table with the encryption field only */
    gs_gpon_database.flows_table[configure_filter_struct->flow_id].encryption_mode = configure_filter_struct->encryption_mode;

    gpon_error = gpon_rx_set_flow_cfg(gs_gpon_database.flows_table[configure_filter_struct->flow_id].flow_id,
        gs_gpon_database.flows_table[configure_filter_struct->flow_id].port_id,
        gs_gpon_database.flows_table[configure_filter_struct->flow_id].encryption_mode,
        gs_gpon_database.flows_table[configure_filter_struct->flow_id].flow_type,
        gs_gpon_database.flows_table[configure_filter_struct->flow_id].flow_priority);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set flow id configuration : error %d !", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_set_filter_control                                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE f_set_filter_control(PON_CTRL_FILTER_INDICATION *control_filter_struct)
{
    PON_ERROR_DTE gpon_error;

    /* Update Rx Flows table */
    gs_gpon_database.flows_table[control_filter_struct->flow_id].flow_id = control_filter_struct->flow_id;
    gs_gpon_database.flows_table[control_filter_struct->flow_id].configure_flag = control_filter_struct->configure_flag;

    gpon_error = gpon_rx_set_flow_status(gs_gpon_database.flows_table[control_filter_struct->flow_id].flow_id,
        gs_gpon_database.flows_table[control_filter_struct->flow_id].configure_flag);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set flow id status : error %d", gpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_rx_set_flow_cfg                                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Rx - Set Flow Configuration                                         */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   The function configures gem flow paramters.                              */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_rx_set_flow_cfg(PON_FLOW_ID_DTE flow_id,
    PON_PORT_ID_DTE port_id, bdmf_boolean encrypt, PON_FLOW_TYPE_DTE type,
    PON_FLOW_PRIORITY_DTE priority)
{
    int drv_error; 

    if (!PON_FLOW_ID_DTE_IN_RANGE(flow_id) || port_id > GPON_PORT_ID_HIGH)
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    if (flow_id < 32)
    {
        drv_error = ag_drv_gpon_rx_port_id_fullcfg_set(flow_id, port_id, encrypt, type, 
            (uint8_t)priority);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Cannot configures gem flow paramters due to driver error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }
    else
    {
        if (priority != PON_FLOW_PRIORITY_LOW || type != PON_FLOW_TYPE_ETH)
        {
            return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
        }
        else
        {
            drv_error = ag_drv_gpon_rx_port_id_cfg_set(flow_id - 32, (uint16_t)port_id, encrypt);
            if (drv_error > 0)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Cannot configures gem flow paramters due to driver error %d.", drv_error);
                return PON_ERROR_DRIVER_ERROR;
            }
        }
    }

    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_rx_set_flow_status                                                  */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Rx - Set Flow status                                                */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   The function enable/dusable the flow.                                    */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_rx_set_flow_status(PON_FLOW_ID_DTE flow_id,
    bdmf_boolean flow_id_status) 
{
    int drv_error; 
    uint32_t sf_cntr1, sf_cntr2;
    bdmf_boolean lof, fec, lcdg;
    uint8_t bit_align;
    
    if (!PON_FLOW_ID_DTE_IN_RANGE(flow_id))
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    drv_error = ag_drv_gpon_rx_port_id_disable_set(flow_id, !flow_id_status);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to disable/enable port due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* If Disable flow- wait until the frame end */
    if (!flow_id_status)
    {
        /* Read super frame counter */
        drv_error = ag_drv_gpon_rx_encryption_sf_cntr_get(&sf_cntr1);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to read super frame counter due to driver error %d ", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        /* Check for LOF alarm */
        drv_error = ag_drv_gpon_rx_general_config_rcvr_status_get(&lof, &fec, &lcdg, &bit_align);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to read recevier status due to driver error %d ", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        if (!lof)
        {
            drv_error = ag_drv_gpon_rx_encryption_sf_cntr_get(&sf_cntr2);
            if (drv_error > 0)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                    "Unable to read super frame counter due to driver error %d ", drv_error);
                return PON_ERROR_DRIVER_ERROR;
            }
        }

        /* wait until the frame end*/
        while (sf_cntr1 == sf_cntr2 && !lof)
        {
            drv_error = ag_drv_gpon_rx_encryption_sf_cntr_get(&sf_cntr2);
            if (drv_error > 0)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                    "Unable to read super frame counter due to driver error %d", drv_error);
                return PON_ERROR_DRIVER_ERROR;
            }

            /* check lof alarm */
            drv_error = ag_drv_gpon_rx_general_config_rcvr_status_get(&lof, &fec, &lcdg, &bit_align);
            if (drv_error > 0)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                    "Unable to read recevier status due to driver error %d", drv_error);
                return PON_ERROR_DRIVER_ERROR;
            }
        }
    }

    return PON_NO_ERROR;
}


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
    uint16_t tcont_id) 
{   
    int drv_error;
    uint8_t tnum[4];

    if (alloc_id_idx >= 40) 
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    drv_error = ag_drv_gpon_rx_alloc_id_alc_tcnt_assoc_get(alloc_id_idx/4, &tnum[0], &tnum[1], &tnum[2], &tnum[3]);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read alloc ID to tcont association due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    tnum[alloc_id_idx%4] = tcont_id;

    drv_error = ag_drv_gpon_rx_alloc_id_alc_tcnt_assoc_set(alloc_id_idx/4, tnum[0], tnum[1], tnum[2], tnum[3]);
    if (drv_error > 0)
        {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to associate alloc ID to tcont due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
   
    return PON_NO_ERROR;
}


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
PON_ERROR_DTE f_get_dbr_divide_ratio(uint32_t xi_gem_block_size, uint8_t *xo_dbr_divide_ratio)
{ 
    switch(xi_gem_block_size )
    {
    case GEM_BLOCK_SIZE_32_BYTES :
        *xo_dbr_divide_ratio = DBR_DIVIDE_RATIO_32_BYTES;
        break;
    case GEM_BLOCK_SIZE_48_BYTES :
        *xo_dbr_divide_ratio = DBR_DIVIDE_RATIO_48_BYTES;
        break;
    case GEM_BLOCK_SIZE_64_BYTES :
        *xo_dbr_divide_ratio = DBR_DIVIDE_RATIO_64_BYTES;
        break;
    default:
        return  PON_ERROR_INVALID_PARAMETER;
    }
    
    return PON_NO_ERROR;
}


/* DBA Statistic Debug Functions */
#ifdef DBA_DEBUG_STATISTICS

/* allocates debug statistics database. returns the size of allocated memory, */
/* or 0 on failure                                                            */
uint32_t gpon_dba_allocate_debug_statistics_db(uint32_t xi_number_of_tconts, uint32_t xi_number_of_cycles)
{
    uint32_t memory_size = 0;
    uint32_t total_memory_size = 0;
    uint32_t index = 0;

    /* allocate the main database */
    memory_size = sizeof(GPON_DBA_DEBUG_STATISTICS_DTE);

    ge_dba_debug_statistics =(GPON_DBA_DEBUG_STATISTICS_DTE *)bdmf_alloc(memory_size);
    if (ge_dba_debug_statistics == 0)
    {
        return 0;
    }

    total_memory_size += memory_size;

    /* allocate the cycles array */
    memory_size = xi_number_of_cycles * sizeof(GPON_DBA_DEBUG_STATISTICS_CYCLE_INFO_DTE);

    ge_dba_debug_statistics ->cycle_info_array = 
        (GPON_DBA_DEBUG_STATISTICS_CYCLE_INFO_DTE *)bdmf_alloc(memory_size);
    if (ge_dba_debug_statistics ->cycle_info_array == 0)
    {
        bdmf_free(ge_dba_debug_statistics);
        ge_dba_debug_statistics = 0;

        return 0;
    }

    total_memory_size += memory_size;

    /* allocate the tconts arrays */
    memory_size = xi_number_of_tconts * sizeof(GPON_DBA_DEBUG_STATISTICS_TCONT_INFO_DTE);

    for (index = 0; index < xi_number_of_cycles; ++ index)
    {
        ge_dba_debug_statistics ->cycle_info_array[index].statistics_tcont_info_array = 
            (GPON_DBA_DEBUG_STATISTICS_TCONT_INFO_DTE *)bdmf_alloc(memory_size);
        if (ge_dba_debug_statistics ->cycle_info_array[index].statistics_tcont_info_array == 0)
        {
            /* free all allocated memory */
            while(index != 0)
            {
                -- index;
                bdmf_free(ge_dba_debug_statistics ->cycle_info_array[index].statistics_tcont_info_array);
            }

            bdmf_free(ge_dba_debug_statistics ->cycle_info_array);
            bdmf_free(ge_dba_debug_statistics);
            ge_dba_debug_statistics = 0;

            return 0;
        }

        total_memory_size += memory_size;
    }

    ge_dba_debug_statistics_max_number_of_tconts = xi_number_of_tconts;
    ge_dba_debug_statistics_max_number_of_cycles = xi_number_of_cycles;

    return total_memory_size; 
}

/* free debug statistics database */
void gpon_dba_free_debug_statistics_db(uint32_t xi_number_of_cycles)
{
    uint32_t index;

    if (ge_dba_debug_statistics != 0)
    {
        for(index = 0; index < xi_number_of_cycles; ++ index)
        {
            bdmf_free(ge_dba_debug_statistics ->cycle_info_array[index].statistics_tcont_info_array);
        }

        bdmf_free(ge_dba_debug_statistics ->cycle_info_array);
        bdmf_free(ge_dba_debug_statistics);
        ge_dba_debug_statistics = 0;
        ge_dba_debug_statistics_max_number_of_tconts = 0;
        ge_dba_debug_statistics_max_number_of_cycles = 0;
    }
}

/* tcont id->debug statistics index mapping */
uint32_t gpon_get_debug_statistics_index(PON_FIFO_ID_DTE xi_tcont_id)
{
    uint32_t index;

    index = ge_dba_debug_statistics ->tcont_to_index[xi_tcont_id];

    /* tcont already exists */
    if (index != CE_DBA_DEBUG_STATISTICS_DONT_CARE_VALUE)
    {
        return index;
    }

    /* doesn't exist - try to add it */
    if (ge_dba_debug_statistics->num_of_tconts_in_tcont_to_index_array < ge_dba_debug_statistics_max_number_of_tconts)
    {
        ge_dba_debug_statistics->tcont_to_index[xi_tcont_id] = ge_dba_debug_statistics->num_of_tconts_in_tcont_to_index_array;

        ++ge_dba_debug_statistics->num_of_tconts_in_tcont_to_index_array;

        return ge_dba_debug_statistics->tcont_to_index[xi_tcont_id];
    }
    /* cannot add new alloc */
    else
    {
        ge_dba_debug_statistics_warning_not_all_tconts_were_collected = BDMF_TRUE;

        return CE_DBA_DEBUG_STATISTICS_DONT_CARE_VALUE;
    }
}

void gpon_dba_debug_reset_statistics(void)
{
    uint32_t tcont_index;
    uint32_t cycle_index;

    ge_dba_debug_statistics_collect = BDMF_FALSE;
    ge_dba_debug_statistics_cycle_counter = 0;
    ge_dba_debug_statistics_warning_not_all_tconts_were_collected = BDMF_FALSE;
    ge_dba_debug_statistics_number_of_cycles = 0;

    /* reset the "tcont to index" mapping */
    for(tcont_index = 0; tcont_index < TXPON_FIFO_UNIT_NUMBER; ++ tcont_index)
    {
        ge_dba_debug_statistics->tcont_to_index[tcont_index] = CE_DBA_DEBUG_STATISTICS_DONT_CARE_VALUE;
    }

    ge_dba_debug_statistics ->num_of_tconts_in_tcont_to_index_array = 0;

    for(cycle_index = 0; cycle_index < ge_dba_debug_statistics_max_number_of_cycles; ++ cycle_index )
    {
        /* reset the tconts info. if tcont becomes active after some cycles of
           collecting statitiscs, we will se in the first cycles the 0xff
           values. i use 0xff and not 0 to not be confused since 0 is legal
           possible value*/
        memset(ge_dba_debug_statistics ->cycle_info_array[cycle_index].statistics_tcont_info_array,
            0xff, ge_dba_debug_statistics_max_number_of_tconts * sizeof(GPON_DBA_DEBUG_STATISTICS_TCONT_INFO_DTE));
    }
}

/* function to be called from the dba_sr_process_callback */
static void p_gpon_dba_debug_collect_debug_statistics_if_needed(PON_FIFO_ID_DTE xi_tcont_id,
    uint32_t xi_dba_sr_data, uint32_t xi_tcont_occupancy,
    uint32_t xi_runner_occupancy, uint8_t  xi_dbr_message)
{
    uint32_t tcont_index  = 0;

  if (ge_dba_debug_statistics_collect == BDMF_TRUE &&
      (ge_dba_debug_statistics_cycle_counter < ge_dba_debug_statistics_number_of_cycles))
   {
        tcont_index = gpon_get_debug_statistics_index(xi_tcont_id);
        if (tcont_index != CE_DBA_DEBUG_STATISTICS_DONT_CARE_VALUE)
        {
            ge_dba_debug_statistics->cycle_info_array[ge_dba_debug_statistics_cycle_counter].statistics_tcont_info_array[tcont_index].dba_sr_data = xi_dba_sr_data;
            ge_dba_debug_statistics->cycle_info_array[ge_dba_debug_statistics_cycle_counter].statistics_tcont_info_array[tcont_index].tcont_occupancy = xi_tcont_occupancy;
            ge_dba_debug_statistics->cycle_info_array[ge_dba_debug_statistics_cycle_counter].statistics_tcont_info_array[tcont_index].runner_occupancy = xi_runner_occupancy;
            ge_dba_debug_statistics->cycle_info_array[ge_dba_debug_statistics_cycle_counter].statistics_tcont_info_array[tcont_index].dbr_message = xi_dbr_message;
        }
    }
}

#endif 

