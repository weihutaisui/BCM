
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

#include "gpon_db.h"
#include "gpon_stat.h"
#include "gpon_logger.h"
#include "gpon_pwm.h"
#include "rogue_drv.h"
#ifdef CONFIG_BCM_GPON_TODD
#include "gpon_tod.h"
#endif
#include <boardparms.h>
#include "bcm_map_part.h"
#include "opticaldet.h"

DEFINE_BDMF_FASTLOCK(gs_fastlock);
DEFINE_BDMF_FASTLOCK(gs_fastlock_irq);

/* Internak function definition */
static PON_ERROR_DTE f_create_os_resources(void);
static PON_ERROR_DTE f_release_os_resources(void);
static void p_init_globals(void);
#ifdef _BYTE_ORDER_LITTLE_ENDIAN_       /* To limit the definition to ARM-based systems */
extern void remap_ru_block_addrs(uint32_t block_index, const ru_block_rec *ru_blocks[]);
#endif
#define CS_PON_QUEUE_MESSAGES 100 

extern BIP8_COUNTERS gs_bip8_counters;
extern LOGGER_LOG_ID log_id;

GPON_DATABASE_DTS gs_gpon_database;

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_software_initialize                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Initialize GPON Stack Software                                     */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function activates the software and firmware                        */
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
PON_ERROR_DTE gpon_software_initialize(uint32_t xi_pon_task_priority)
{
    PON_ERROR_DTE gpon_error;

    p_init_globals();

    /* Create OS resources */
    gs_gpon_database.os_resources.pon_task_priority = xi_pon_task_priority;

    gpon_error = f_create_os_resources();
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "OS resources error!");
        return PON_ERROR_OS_ERROR;
    }

    /* system single RT HW timer, that is used by the DBA SR is initialized   */
    /* before initializing the GPON stack                                     */
     gs_gpon_database.dbr_parameters.dba_sr_timer_initiated = BDMF_TRUE;
    /* Mark active state */
    gs_gpon_database.gs_sw_state = GPON_STATE_INITIALIZED;

#ifdef CONFIG_BCM_GPON_TODD
    gpon_tod_register_funcs();

    if (tod_init_sw(GPON_MODE) != 0)
    {
        return PON_GENERAL_ERROR;
    }
#endif 

    pmd_dev_assign_pon_stack_callback(gpon_set_pmd_fb_done);

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_software_terminate                                                  */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Software Terminate                                                 */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function deactivates the software                                   */
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
PON_ERROR_DTE gpon_software_terminate(void)
{
    PON_ERROR_DTE gpon_error;
    BL_PWM_ERR_DTS err_pwm;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Release OS resources */
    gpon_error = f_release_os_resources();
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "OS resources error!");
        return PON_ERROR_OS_ERROR;
    }

    /* Mark deactivation state */
    gs_gpon_database.gs_sw_state = GPON_STATE_TERMINATED;

    /* Power Management: Free */
    err_pwm = api_pwm_free(); 
    if (err_pwm != CS_BL_PWM_ERR_OK )
    {
        return PON_GENERAL_ERROR;
    }


    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_software_status                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Get software state                                                 */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the current state of the software                  */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xo_state - The output state of the software                              */
/*     GPON_STATE_TERMINATED - The GPON stack is inactive                     */
/*     GPON_STATE_INITIALIZED - The GPON stack is currently being activated   */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_software_status(GPON_SW_STATUS_DTE *const xo_state)
{
    *xo_state = gs_gpon_database.gs_sw_state;

    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_pon_id_parameters                                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Get PON ID parameters                                               */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   The function returns PON ID parameters that were read from pon-Id PLOAM. */
/*   [G.984.3 specification, Annex C]                                         */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*   xo_pon_param - pon_id_type[1 Byte], pon_identifier[7 bytes],             */
/*                  tx_optical_level[2 bytes]                                 */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_pon_id_parameters(PON_ID_PARAMETERS *xo_pon_param)
{
    memcpy(xo_pon_param, &gs_gpon_database.onu_parameters.pon_id_param, 
        sizeof(PON_ID_PARAMETERS));      

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_ploam_keep_alive_expired                                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Handles ploam keep alive timoout                                         */
/*                                                                            */
/* Abstract:                                                                  */
/*   This function sends correspondent user indication in case when           */
/*   ploam grants have not been detected during certain period of time        */
/*                                                                            */
/* Input:                                                                     */
/*    timer - unused                                                          */
/*    xi_parameter - unused                                                   */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/******************************************************************************/
static void p_ploam_keep_alive_expired(bdmf_timer_t *timer, unsigned long xi_parameter)
{
    bdmf_error_t bdmf_error;

    /* Link state Active? */
    if (gs_gpon_database.link_parameters.operation_state_machine.link_state != LINK_STATE_ACTIVE )
    {
        return;
    }

    /* Link sub-state Operational? */
    if (gs_gpon_database.link_parameters.operation_state_machine.link_sub_state != LINK_SUB_STATE_OPERATIONAL)
    {
        return;
    }

    /* Send appropriate user indication if no ploam grant was not received */
#ifdef TBD /* Get the ploam grant counter and check if clear then alarm for no ploam */
    if ((  ) == BDMF_FALSE )
#endif
    {
        bdmf_error = f_gpon_send_message_to_pon_task(PON_EVENT_KEEPALIVE_TIMEOUT, 
            gs_gpon_database.physical_parameters.upstream_ploam_rate);
        if (bdmf_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.timers_id, 
                "Fail to send message to pon task");
        }
    }

    /* Restart Keep Alive watchdog timer */
    bdmf_error = bdmf_timer_start(&gs_gpon_database.os_resources.keep_alive_timer_id, 
        GPON_MILISEC_TO_TICKS( gs_gpon_database.physical_parameters.upstream_ploam_rate));
    if (bdmf_error != BDMF_ERR_OK )
    {
        /* !!! report sw error for interrupt level */
    }
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_ber_interval_expired                                                   */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Handles ber interval timoout                                             */
/*                                                                            */
/* Abstract:                                                                  */
/*   This function sends correspondent user indication in case when           */
/*   ploam grants have not been detected during certain period of time        */
/*                                                                            */
/* Input:                                                                     */
/*    timer - unused                                                          */
/*    xi_parameter - unused                                                   */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/******************************************************************************/
static void p_ber_interval_expired(bdmf_timer_t *timer, unsigned long xi_parameter)
{
    bdmf_error_t bdmf_error;

    p_log(ge_onu_logger.sections.stack.debug.timers_id,  "BER interval timer has been expired.");

    /* Send message to Pon task throu ISR task since this routine is performed in Interrupt level */
    bdmf_error = f_gpon_send_message_to_pon_task(PON_EVENT_BER_TIMEOUT);
    if (bdmf_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Fail to send message to pon task");
    }

    /* Restart timer */
    bdmf_error = bdmf_timer_start(&gs_gpon_database.os_resources.ber_interval_timer_id, 
        GPON_MILISEC_TO_TICKS(gs_gpon_database.onu_parameters.ber_interval ));
    if (bdmf_error != BDMF_ERR_OK )
    {
        p_log(ge_onu_logger.sections.stack.debug.timers_id, "Fail to start watchdog");
    }
}

static void p_pon_to1_timer_callback(bdmf_timer_t *timer, unsigned long xi_parameter)
{
    OPERATION_SM_PARAMS_DTS operation_sm_params;

    p_log(ge_onu_logger.sections.stack.debug.timers_id, "TO1 timer expired ");

    p_operation_state_machine(CS_OPERATION_EVENT_TIMER_TO1_EXPIRE, &operation_sm_params);

    return;
}

static void p_pon_to2_timer_callback(bdmf_timer_t *timer, unsigned long xi_parameter)
{
    OPERATION_SM_PARAMS_DTS operation_sm_params;

    p_log(ge_onu_logger.sections.stack.debug.timers_id, "TO2 timer expired ");

    p_operation_state_machine(CS_OPERATION_EVENT_TIMER_TO2_EXPIRE, & operation_sm_params);

    return;
}

/* HW_WA_2 */
static void p_pon_lcdg_timer_callback(bdmf_timer_t *timer, unsigned long xi_parameter)
{
    /* result of GPON driver APIs */
    PON_ERROR_DTE gpon_error = PON_NO_ERROR;
    /* vector of Rx Pon enabled interrupts*/
    gpon_rx_irq_grxier gpon_rx_ier_vector = {};
    /* indicates whether all Rx Pon interrupts are currently masked */
    bool are_all_rx_int_masked = BDMF_FALSE;
    int drv_error;

    /* mark LCDG Rx interrupt as enabled interrupt, that should be handled by the stack*/
    gs_gpon_database.rx_ier_vector.lcdgchng = BDMF_TRUE;

    /*check whether RX Pon interrupts are currently masked*/
    gpon_error = gpon_are_rx_pon_interrupts_masked(&are_all_rx_int_masked);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to get gpon rx interrupt status, error =%d ", gpon_error);
        return;
    }
    /* if Rx Pon interrupts are currently not masked, unmask the LCDG interrupt as well */
    if (are_all_rx_int_masked == BDMF_FALSE)
    {    
        drv_error = ag_drv_gpon_rx_irq_grxier_get(&gpon_rx_ier_vector);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Failed to read Rx interrupts enable due to driver error %d.", drv_error);
          return;
        }

        gpon_rx_ier_vector.lcdgchng = BDMF_TRUE;
        gpon_error = ag_drv_gpon_rx_irq_grxier_set(&gpon_rx_ier_vector);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to set gpon rx interrupt enable register, error = %d", gpon_error);
            return;
        }
    }
    return;
}

static void p_pon_pee_timer_callback(bdmf_timer_t *timer, unsigned long xi_parameter)
{
    bdmf_error_t bdmf_error;

    /* Restart the PEE timer */
    bdmf_error = bdmf_timer_start(& gs_gpon_database.os_resources.pee_timer_id, 
        GPON_MILISEC_TO_TICKS(GPON_PON_PEE_TIMEOUT));
    if (bdmf_error != BDMF_ERR_OK)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "General error (PON_GENERAL_ERROR)");
    }

     /* send message to Pon task throu ISR task since
        this routine is performed in Interrupt level */
    bdmf_error = f_gpon_send_message_to_pon_task(PON_EVENT_PEE_TIMEOUT);
    if (bdmf_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Fail to send message to pon task");
    }
}


static PON_ERROR_DTE f_create_os_resources()
{
    bdmf_error_t bdmf_error;
    /* OS frequency */
    gs_gpon_database.os_resources.os_frequency = bdmf_get_cpu_frequency();

    /*  Allocate queue for messages */
    bdmf_error = bdmf_queue_create(&gs_gpon_database.os_resources.pon_messages_queue_id, 
        CS_PON_QUEUE_MESSAGES, sizeof(GPON_MESSAGE_DTE));
    if (bdmf_error != BDMF_ERR_OK)
    {
        return PON_ERROR_QUEUE_CREATE;
    }

    /* Pon task */
    bdmf_error = bdmf_task_create("Pon", gs_gpon_database.os_resources.pon_task_priority,
        gs_gpon_database.os_resources.stack_size, p_pon_task, NULL,
        &gs_gpon_database.os_resources.pon_task_id);
    if (bdmf_error != BDMF_ERR_OK)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "OS error while spawning 'Pon' task: %d", bdmf_error);
        return PON_ERROR_OS_ERROR;
    }

    /* Create TO1 timer */
    bdmf_timer_init(&gs_gpon_database.os_resources.to1_timer_id, p_pon_to1_timer_callback, 0);

    /* Create TO2 timer */
    bdmf_timer_init(&gs_gpon_database.os_resources.to2_timer_id, p_pon_to2_timer_callback, 0);

    /* Create LCDG timer */
    bdmf_timer_init(&gs_gpon_database.os_resources.lcdg_timer_id, p_pon_lcdg_timer_callback, 0);

    /* Create Keep Alive timer */
    bdmf_timer_init(&gs_gpon_database.os_resources.keep_alive_timer_id, p_ploam_keep_alive_expired, 0);

    /* Create PEE timer */
    bdmf_timer_init(&gs_gpon_database.os_resources.pee_timer_id, p_pon_pee_timer_callback, 0);

    /* Create BER timer */
    bdmf_timer_init(&gs_gpon_database.os_resources.ber_interval_timer_id, p_ber_interval_expired, 0);

    return PON_NO_ERROR;
}


static PON_ERROR_DTE f_release_os_resources(void)
{
    int ret;
    bdmf_error_t bdmf_error;

    /* Delete the Indication task resources */
    /* Pon task */
    ret = bdmf_task_destroy(gs_gpon_database.os_resources.pon_task_id);
    if (ret < 0 )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "OS error while deleting 'Pon' task: %d", ret);
        return PON_ERROR_OS_ERROR;
    }
    
    /*  Deallocate queue for messages */
    bdmf_error = bdmf_queue_delete(&gs_gpon_database.os_resources.pon_messages_queue_id);
    if (bdmf_error != BDMF_ERR_OK)
    {
        return PON_ERROR_QUEUE_CREATE;
    }

    /* Delete TO1 timer */
    bdmf_timer_delete(&gs_gpon_database.os_resources.to1_timer_id);

    /* Delete TO2 timer */
    bdmf_timer_delete(&gs_gpon_database.os_resources.to2_timer_id);

    /* HW_WA_2 */
    /* Delete LCDG timer */
    bdmf_timer_delete(&gs_gpon_database.os_resources.lcdg_timer_id);

    /* Delete Keep Alive timer */
    bdmf_timer_delete(&gs_gpon_database.os_resources.keep_alive_timer_id);

    /* Delete PEE timer */
    bdmf_timer_delete(&gs_gpon_database.os_resources.pee_timer_id);

    /* Delete BER timer */
    bdmf_timer_delete(&gs_gpon_database.os_resources.ber_interval_timer_id);
    
    return PON_NO_ERROR;
}

static void p_init_globals(void)
{
    uint32_t flow_index;
    uint32_t tcont_index;
#ifdef BCM_ROGUE_ONU_DETECT
    TRX_SIG_PRESENCE tx_sd_supported;
    int ret ;
    int bus = -1;
 
    opticaldet_get_xpon_i2c_bus_num(&bus);
#endif

    /* init OS resources */
    memset(& gs_gpon_database.os_resources.pon_messages_queue_id, 0, 
        sizeof(gs_gpon_database.os_resources.pon_messages_queue_id));
    gs_gpon_database.os_resources.pon_task_id = NULL;
    gs_gpon_database.os_resources.pon_task_priority = GPON_DEFAULT_PON_TASK_PRIORITY;
    gs_gpon_database.os_resources.stack_size = GPON_STACK_SIZE;
    memset(& gs_gpon_database.os_resources.to1_timer_id, 0, 
        sizeof(gs_gpon_database.os_resources.to1_timer_id ));
    memset(& gs_gpon_database.os_resources.to2_timer_id, 0, 
        sizeof(gs_gpon_database.os_resources.to2_timer_id));
    memset(& gs_gpon_database.os_resources.ber_interval_timer_id, 0, 
        sizeof(gs_gpon_database.os_resources.ber_interval_timer_id ));
    memset(& gs_gpon_database.os_resources.keep_alive_timer_id, 0, 
        sizeof(gs_gpon_database.os_resources.keep_alive_timer_id));
    memset(& gs_gpon_database.os_resources.pee_timer_id, 0, 
        sizeof(gs_gpon_database.os_resources.pee_timer_id));
    memset(& gs_gpon_database.os_resources.lcdg_timer_id, 0, 
        sizeof(gs_gpon_database.os_resources.lcdg_timer_id));
    gs_gpon_database.os_resources.os_frequency = 0;

    /* init Gpon interrupts that should be handled by the Gpon stack */
    /* Tx interrupts */
    memset(&gs_gpon_database.tx_ier_vector, BDMF_FALSE, sizeof(GPON_TX_ISR));
    gs_gpon_database.tx_ier_vector.normal_ploam_transmitted_irq = BDMF_TRUE;
    gs_gpon_database.tx_ier_vector.urgent_ploam_transmitted_irq = BDMF_TRUE;
    gs_gpon_database.tx_ier_vector.ranging_ploam_transmitted_irq = BDMF_TRUE;

#ifdef BCM_ROGUE_ONU_DETECT
    ret = trx_get_tx_sd_supported(bus, &tx_sd_supported) ;    
    if ((ret == 0) && (tx_sd_supported == TRX_SIGNAL_SUPPORTED))
    {
        gs_gpon_database.tx_ier_vector.rogue_onu_diff = BDMF_TRUE;
        gs_gpon_database.tx_ier_vector.rogue_onu_level = BDMF_TRUE;
    }
    else
    {
        gs_gpon_database.tx_ier_vector.rogue_onu_diff = BDMF_FALSE;
        gs_gpon_database.tx_ier_vector.rogue_onu_level = BDMF_FALSE;
    }
#else
    gs_gpon_database.tx_ier_vector.rogue_onu_diff = BDMF_FALSE;
    gs_gpon_database.tx_ier_vector.rogue_onu_level = BDMF_FALSE;
#endif
    /* Rx interrupts */
    memset(&gs_gpon_database.rx_ier_vector, BDMF_FALSE, sizeof(gpon_rx_irq_grxier));
    gs_gpon_database.rx_ier_vector.lcdgchng = BDMF_TRUE; /* lcdg_state_change_irq */
    gs_gpon_database.rx_ier_vector.lofchng = BDMF_TRUE;  /* lof_state_change_irq */
    gs_gpon_database.rx_ier_vector.tod = BDMF_TRUE;      /* tod_update_irq */

    /* Init alarm status */
    memset(& gs_gpon_database.last_alarm_status, 0, sizeof(PON_ALARM_STATUS_DTE));

    /* Set link default configuration */
    gs_gpon_database.link_parameters.rei_sequence_number = GPON_DEFAULT_REI_SEQUENCE_NUMBER;
    gs_gpon_database.link_parameters.serial_number_request_messages_counter = 0;
    gs_gpon_database.link_parameters.aes_key_switching_first_time = BDMF_TRUE;
    gs_gpon_database.link_parameters.operation_state_machine.link_state = LINK_STATE_NOT_READY;
    gs_gpon_database.link_parameters.operation_state_machine.link_sub_state = LINK_SUB_STATE_NONE;
    gs_gpon_database.link_parameters.operation_state_machine.activation_state = OPERATION_STATE_O1;
    gs_gpon_database.link_parameters.aes_state_machine.aes_state = CE_AES_STATE_NOT_READY;
    gs_gpon_database.link_parameters.aes_state_machine.key_index = 0;
    gs_gpon_database.link_parameters.aes_state_machine.seed_counter = 0;
    gs_gpon_database.link_parameters.aes_state_machine.aes_message_ptr = 0;
    gs_gpon_database.link_parameters.aes_state_machine.current_switching_time = GPON_DEFAULT_AES_SWITCHING_TIME;
    gs_gpon_database.link_parameters.aes_state_machine.request_key_arrived = BDMF_FALSE;
    gs_gpon_database.link_parameters.auto_restart_to1_mode = GPON_TO1_AUTO_RESTART_OFF;
    gs_gpon_database.link_parameters.received_serial_number_request = BDMF_FALSE;

    /* gs_gpon_database.link_parameters. */
    memset(gs_gpon_database.link_parameters.aes_state_machine.new_key, 0 ,
        (sizeof(AES_KEY_DTE)*GPON_DEFAULT_AES_KEY_SIZE));

    /* Init ONU parameters */
    gs_gpon_database.onu_parameters.onu_id = GPON_TX_ONU_ID_DEF;
    gs_gpon_database.onu_parameters.ber_interval = GPON_DEFAULT_BER_INTERVAL;
    gs_gpon_database.onu_parameters.ber_threshold_for_sd_assertion = GPON_DEFAULT_SD_THRESHOLD;
    gs_gpon_database.onu_parameters.ber_threshold_for_sf_assertion = GPON_DEFAULT_SF_THRESHOLD;

    memset(gs_gpon_database.onu_parameters.delimiter, 0 , GPON_DELIMITER_SIZE);

    gs_gpon_database.onu_parameters.extended_burst_length_valid_flag = GPON_DEFAULT_EXTENDED_BURST_FLAG;
    gs_gpon_database.onu_parameters.extra_serial_number_transmissions = GPON_DEFAULT_EXTRA_SN_TRANSMISSIONS;
    gs_gpon_database.onu_parameters.number_of_guard_bits = GPON_DEFAULT_GUARD_BITS_NUMBER;
    gs_gpon_database.onu_parameters.number_of_type_1_preamble_bits = GPON_DEFAULT_TYPE1_PREAMBLE_BITS;
    gs_gpon_database.onu_parameters.number_of_type_2_preamble_bits = GPON_DEFAULT_TYPE2_PREAMBLE_BITS;
    gs_gpon_database.onu_parameters.pattern_of_type_3_preamble_bits = GPON_DEFAULT_TYPE3_PREAMBLE_BITS;
    gs_gpon_database.onu_parameters.pre_equalization_status = GPON_DEFAULT_PREEQUALIZATION_STATUS;
    gs_gpon_database.onu_parameters.pre_range_type_3_number = GPON_DEFAULT_PRE_RANGED_TYPE3_NUMBER;
    gs_gpon_database.onu_parameters.range_type_3_number = GPON_DEFAULT_RANGED_TYPE3_NUMBER;
    gs_gpon_database.onu_parameters.preassigned_equalization_delay = GPON_DEFAULT_PREASSIGNED_EQ_DELAY;
    gs_gpon_database.onu_parameters.to1_timer_timeout = GPON_DEFAULT_TO1_TIMEOUT;
    gs_gpon_database.onu_parameters.to2_timer_timeout = GPON_DEFAULT_TO2_TIMEOUT;
    gs_gpon_database.onu_parameters.lcdg_timer_timeout = GPON_LCDG_TIMEOUT;
    gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_status = BDMF_FALSE;
    gs_gpon_database.onu_parameters.serial_number_mask_parameters.serial_number_mask_matched = BDMF_FALSE;
    gs_gpon_database.onu_parameters.default_power_level_mode = GPON_DEFAULT_POWER_LEVEL_MODE;
    gs_gpon_database.onu_parameters.broadcast_default_value = GPON_RX_ONU_BROADCAST_ID_DEF;
    gs_gpon_database.onu_parameters.delay = GPON_DEFAULT_PREASSIGNED_EQ_DELAY;

    /* Init ONU physical parameters */
    gs_gpon_database.physical_parameters.min_response_time = GPON_DEFAULT_MIN_RESPONSE_TIME;
    gs_gpon_database.physical_parameters.number_of_psyncs_for_lof_assertion = GPON_DEFAULT_LOF_ASSERTION;
    gs_gpon_database.physical_parameters.number_of_psyncs_for_lof_clear = GPON_DEFAULT_LOF_CLEAR;
    memset( & gs_gpon_database.physical_parameters.password, 0, PON_PASSWORD_FIELD_SIZE);
    memset( & gs_gpon_database.physical_parameters.serial_number, 0, sizeof(PON_SERIAL_NUMBER_DTE));

    gs_gpon_database.physical_parameters.transceiver_dv_hold_pattern           = GPON_DEFAULT_DV_HOLD_PATTERN;
    gs_gpon_database.physical_parameters.transceiver_dv_polarity               = GPON_DEFAULT_DV_POLARITY;
    gs_gpon_database.physical_parameters.transceiver_dv_setup_pattern          = GPON_DEFAULT_DV_SETUP_PATTERN;
    gs_gpon_database.physical_parameters.transceiver_power_calibration_mode    = GPON_DEFAULT_POWER_MODE;
    gs_gpon_database.physical_parameters.transceiver_power_calibration_pattern = GPON_DEFAULT_POWER_CALIBRATION_PATTERN;
    gs_gpon_database.physical_parameters.transceiver_power_calibration_size    = GPON_DEFAULT_POWER_CALIBRATION_SIZE;
    gs_gpon_database.physical_parameters.transceiver_data_pattern_enable       = GPON_DEFAULT_DATA_PATTERN_ENABLE;
    gs_gpon_database.physical_parameters.transceiver_prbs_enable               = GPON_DEFAULT_PRBS_ENABLE;
    gs_gpon_database.physical_parameters.transceiver_data_setup_length         = GPON_DEFAULT_DATA_SETUP_LENGTH;
    gs_gpon_database.physical_parameters.transceiver_data_hold_length          = GPON_DEFAULT_DATA_HOLD_LENGTH;
    gs_gpon_database.physical_parameters.transceiver_data_setup_pattern        = GPON_DEFAULT_DATA_SETUP_PATTERN;
    gs_gpon_database.physical_parameters.transceiver_data_hold_pattern         = GPON_DEFAULT_DATA_HOLD_PATTERN;

    gs_gpon_database.physical_parameters.upstream_ploam_rate = GPON_US_PLOAM_RATE;

    /* Init TX configuration */
    gs_gpon_database.tx_parameters.tx_bip_enable = BDMF_TRUE;
    gs_gpon_database.tx_parameters.tx_loopback_enable = BDMF_FALSE;
    gs_gpon_database.tx_parameters.tx_pls_enable = BDMF_FALSE;
    gs_gpon_database.tx_parameters.tx_misc_enable = BDMF_FALSE;
    gs_gpon_database.tx_parameters.tx_scrambler_enable = BDMF_TRUE;
    gs_gpon_database.tx_parameters.tx_fec_enable = BDMF_TRUE;
    gs_gpon_database.tx_parameters.tx_data_out_polarity = BDMF_FALSE;
    gs_gpon_database.tx_parameters.tx_phase_invert = BDMF_TRUE;
    gs_gpon_database.tx_parameters.rx_phase_invert = BDMF_FALSE;

    /* Init RX configuration */
    gs_gpon_database.rx_parameters.rcvr_config.des_disable = BDMF_FALSE;
    gs_gpon_database.rx_parameters.rcvr_config.fec_disable = BDMF_FALSE;
    gs_gpon_database.rx_parameters.rcvr_config.rx_disable = BDMF_TRUE;
    gs_gpon_database.rx_parameters.rcvr_config.loopback_enable = BDMF_FALSE;
    gs_gpon_database.rx_parameters.rcvr_config.fec_force = BDMF_FALSE;
    gs_gpon_database.rx_parameters.rcvr_config.fec_st_disc = 1;
    gs_gpon_database.rx_parameters.rcvr_config.squelch_dis = 1;
    gs_gpon_database.rx_parameters.rcvr_config.sop_reset = 0;
    gs_gpon_database.rx_parameters.rcvr_config.din_polarity = BDMF_FALSE;

    gs_gpon_database.rx_parameters.rx_crc_override = BDMF_FALSE;
    gs_gpon_database.rx_parameters.rx_idle_ploam_filltering_enable = BDMF_TRUE;
    gs_gpon_database.rx_parameters.rx_forward_all_ploam_messages = BDMF_FALSE;
    gs_gpon_database.rx_parameters.rx_congestion_into_threshold = 0x3F;
    gs_gpon_database.rx_parameters.rx_congestion_out_of_threshold = 0x0;
    gs_gpon_database.rx_parameters.rx_8khz_clock_lock = BDMF_TRUE;
    gs_gpon_database.rx_parameters.rx_8khz_clock_phase_diff = 0;

    gs_gpon_database.dbr_parameters.dbr_valid_part1            = GPON_DEFAULT_DBR_VALID_PART1;
    gs_gpon_database.dbr_parameters.dbr_valid_part2            = GPON_DEFAULT_DBR_VALID_PART2;
    gs_gpon_database.dbr_parameters.dbr_stop_index             = GPON_DEFAULT_DBR_STOP_INDEX;
    gs_gpon_database.dbr_parameters.dbr_sr_mode                = GPON_SR_MODE_MIPS ;
#ifdef XRDP
    gs_gpon_database.dbr_parameters.dbr_sr_mode                = GPON_SR_MODE_RUNNER ;
#endif
    gs_gpon_database.dbr_parameters.dba_sr_reporting_block     = GPON_DEFAULT_GEM_BLOCK_SIZE;
    gs_gpon_database.dbr_parameters.dbr_flush                  = BDMF_FALSE;
    gs_gpon_database.dbr_parameters.dba_sr_timer_initiated     = BDMF_FALSE;
    gs_gpon_database.dbr_parameters.dbr_status                 = BDMF_FALSE;
    gs_gpon_database.dbr_parameters.sr_dba_interval            = GPON_SR_DBA_POLLING_INTERVAL;
    gs_gpon_database.dbr_parameters.sr_dba_interval_microsec   =
        MS_MILISEC_TO_MICROSEC(gs_gpon_database.dbr_parameters.sr_dba_interval);
    gs_gpon_database.dbr_parameters.is_dba_sr_allowed           = BDMF_FALSE;                                                                
    gs_gpon_database.dbr_parameters.should_start_dba_sr       = BDMF_FALSE;
    gs_gpon_database.dbr_parameters.timer_num = -1;

    gs_bip8_counters.ber_interval_bip8_counter_mirror = 0;
    gs_bip8_counters.critical_ber_bip8_counter_mirror = 0;
    gs_bip8_counters.user_bip8_counter_mirror = 0;

    for (flow_index = 0; flow_index < MAX_NUMBER_OF_FLOWS; flow_index++ )
    {
        gs_gpon_database.flows_table[flow_index].configure_flag  = BDMF_FALSE;
        gs_gpon_database.flows_table[flow_index].encryption_mode = BDMF_FALSE;
        gs_gpon_database.flows_table[flow_index].flow_id         = FLOW_ID_UNASSIGNED;
        gs_gpon_database.flows_table[flow_index].flow_priority   = PON_FLOW_PRIORITY_UNASSIGNED;
        gs_gpon_database.flows_table[flow_index].flow_type       = PON_FLOW_TYPE_ETH;
        gs_gpon_database.flows_table[flow_index].port_id         = GPON_PORT_ID_UNASSIGNED;
        gs_gpon_database.flows_table[flow_index].port_mask_id    = GPON_PORT_ID_UNASSIGNED;
    }

    for (tcont_index = 0; tcont_index < MAX_NUMBER_OF_TCONTS; tcont_index++ )
    {
        gs_gpon_database.tconts_table[tcont_index].tcont_id                = CS_DEFAULT_TCONT_ID;
        gs_gpon_database.tconts_table[tcont_index].config_tcont_valid_flag = BDMF_FALSE;
        gs_gpon_database.tconts_table[tcont_index].assign_alloc_valid_flag = BDMF_FALSE;
        gs_gpon_database.tconts_table[tcont_index].alloc_id                = PON_ALLOC_ID_DTE_NOT_ASSIGNED;
    }
}

static void gpon_stack_rogue_onu_detect_init (void)
{
    gl_rogue_onu_detect_cb.get_rogue_onu_detection_params = gpon_tx_get_rogue_onu_detection_params;
    gl_rogue_onu_detect_cb.set_rogue_onu_detection_params = gpon_set_rogue_onu_detection_params;
    gl_rogue_onu_detect_cb.cfg_rogue_onu_interrupts = gpon_cfg_rogue_onu_interrupts;
}

int gpon_stack_init(void)
{
#ifdef USE_LOGGER
    LOGGER_ERROR rc;

    rc = logger_init();
    if (rc) 
        return rc;
#endif

#ifdef _BYTE_ORDER_LITTLE_ENDIAN_       /* To limit the function activation to ARM-based systems */
#if defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    remap_ru_block_addrs(XRDP_IDX, RU_GPON_BLOCKS);
#else
    remap_ru_block_addrs(GPON_IDX, RU_GPON_BLOCKS);
#endif
#endif

    rc = gpon_software_initialize(50); /* RDPA task priority */

    BpGetGponOpticsType(&gs_gpon_database.OpticsType);

    return rc;
}

void gpon_stack_exit(void)
{
#ifdef USE_LOGGER
    logger_exit();
#endif

    /* Exit SW */
    gpon_software_terminate();
}

int init_pon_module(PON_SM_CALLBACK_FUNC *p_pon_sm_cb)
{
    gpon_register_cb(p_pon_sm_cb);

    gpon_stack_rogue_onu_detect_init();

    gpon_init_crc32_table_for_omci();

    return 0;
}

EXPORT_SYMBOL(init_pon_module);

int __init gponStackModuleCreate(void)
{
    int  ret = 0;

    ret = gpon_stack_init();

    return ret;
}

void __exit gponStackModuleDelete(void)
{
    gpon_stack_exit();
    return;
}


MODULE_DESCRIPTION("Broadcom GPON Stack");
MODULE_LICENSE("Proprietary");
module_init(gponStackModuleCreate);
module_exit(gponStackModuleDelete);
