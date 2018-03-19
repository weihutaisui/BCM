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
#ifndef SIM_ENV
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/interrupt.h>
#include <bcm_ext_timer.h>
#else
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#endif
#include <bdmf_dev.h>

/* driver */
#include "ngpon_cfg.h"
#include "ngpon_db.h"
#ifdef CONFIG_BCM_GPON_TODD
#include "ngpon_tod.h"
#endif
#include "aes_cmac.h"

#if defined(USE_LOGGER)
#include "gpon_logger.h"
#endif

#include <wan_drv.h>

/******************************************************************************/
/*                                                                            */
/* Types and values definitions                                               */
/*                                                                            */
/******************************************************************************/
const uint8_t PLOAM_IK_DEFAULT[]={0X55, 0X55, 0X55, 0X55, 0X55, 0X55, 0X55, 0X55, 0X55, 0X55, 0X55, 0X55, 0X55, 0X55, 0X55, 0X55};


/******************************************************************************/
/*                                                                            */
/* Functions Definitions                                                      */
/*                                                                            */
/******************************************************************************/

void tx_int_handler(void);
PON_ERROR_DTE verify_ds_ploam_packet(NGPON_DS_PLOAM * packet_buffer);
void dump_ds_ploam_msg(uint8_t * xi_ploam_ptr);
void handle_rx_interrupt_lof_lcdg(ngpon_rx_int_ism * rx_int_ism);
void handle_sfc_indication (void);
extern void pon_serdes_lof_fixup_irq(int lof);


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   This function is the interrupt service routine(ISR) for the XGPON ONU   */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_mac_isr(void * xi_ploam_message_ptr, PON_INTERRUPTS_MASK_DTE xi_interrupt_vector)
{
    uint32_t            error_code;
    NGPON_DS_PLOAM * ploam_ptr =(NGPON_DS_PLOAM *)xi_ploam_message_ptr;

    /*
    p_log(ge_onu_logger.sections.isr.debug.general_id, "\n===> ngpon_mac_isr_tasklet() Stack is in %d state<===\n", g_software_state);
    */
    /* Check software state */
    if (g_software_state == NGPON_SOFTWARE_STATE_UNINITIALIZED)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Tasklet: the stack is not initialized");
        /* Return from the tasklet */
        goto clean_up_tasklet;
      
    }

    if (xi_interrupt_vector.rxpon_interrupt == BDMF_TRUE)
    {
        rx_int_handler();
    }

    if (xi_interrupt_vector.txpon_interrupt == BDMF_TRUE)
    {
        tx_int_handler();
    }

    if (xi_interrupt_vector.ploam_received_interrupt == BDMF_TRUE)
    {
        ds_ploam_hanlder(ploam_ptr);
    }

clean_up_tasklet:
    error_code = 0;

        return PON_NO_ERROR;
}


void rx_int_handler(void)
{
    PON_ERROR_DTE       device_error ;
    ngpon_rx_int_ism    rx_int;
    ngpon_rx_int_isr    unmasked_rx_int;

    /* Read the masked interrupts status */
    device_error = ag_drv_ngpon_rx_int_ism_get(&rx_int);
    if (device_error != PON_NO_ERROR)
    {
        /* Report the SW error */
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Failed to get Rx interrupts status(masked)");
        /* Return from the tasklet */
        return;
    }

    /* Clear the interrupts status of those interrupts that we are */
    /* about to take care of. In that way we would know if another */
    /* interrupt occurred while we took care of a previous         */
    /* interrupt from the same type                                */
    device_error = ag_drv_ngpon_rx_int_isr_get(&unmasked_rx_int);   
    if (device_error != PON_NO_ERROR)
    {
        /* Report the SW error */
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Failed to read Rx ISR");
        /* Return from the tasklet */
        return;
    }

    device_error = ag_drv_ngpon_rx_int_isr_set(&unmasked_rx_int);
    if (device_error != PON_NO_ERROR)
    {
        /* Report the SW error */
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Failed to clean Rx interrupts");
        /* Return from the tasklet */
        return;
    }
    /* Check the interrupt source and activate corresponding mechanism */

    if (rx_int.tm_fifo_ovf)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "TM FIFO overflow interrupt");
    }

    if (rx_int.bw_fifo_ovf)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Access FIFO overflow interrupt");
    }

    if (rx_int.aes_fail)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "no AES machines could be allocated to decrypt a fragment interrupt");
    }

    if (rx_int.fwi_state_change)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Assertion or negation of FWI in received BWMAPs interrupt");
    }

    if (unmasked_rx_int.lof_state_change || unmasked_rx_int.lcdg_state_change)
    {
        handle_rx_interrupt_lof_lcdg((ngpon_rx_int_ism *) &unmasked_rx_int);
    }

    if (rx_int.lb_fifo_ovf)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "LB FIFO overflow interrupt");
    }

#ifdef CONFIG_BCM_GPON_TODD
    if (unmasked_rx_int.tod_update)
    {
        p_log(ge_onu_logger.sections.isr.debug.rx_event_id, "ToD update interrupt");
        tod_handle_tod_update_interrupt();
    }
#endif 

    if (rx_int.bw_dis_tx)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Bandwidth allocation to a disabled transmitter interrupt");
    }

    if (rx_int.inv_key)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Incoming XGEM with a non-valid key index interrupt");
    }

    if (rx_int.bwmap_rec_done)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Bandwidth map recorder done interrupt");
    }

    if (rx_int.bwmap_rec_done)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Bandwidth map recorder done interrupt");
    }

    if (rx_int.ponid_inconsist)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "First PON-ID inconsistency interrupt");
    }

    if (rx_int.inv_portid_change)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Invalid Port-ID between Ethernet fragments interrupt");
    }

    if (rx_int.fec_fatal_error)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Fatal error in FEC block(check map) interrupt");
    }

    if (unmasked_rx_int.sfc_indication)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Indicates the start of the superframe set at gen. interrupt");
        handle_sfc_indication () ;
    }

    if (rx_int.amcc_grant)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Reserved for AMCC grant indication interrupt");
    }
}

void tx_int_handler(void)
{
    PON_ERROR_DTE        device_error, xgpon_error ;
    ngpon_tx_int_ism1   tx_int;
    ngpon_tx_int_isr1   unmasked_tx_int;

    uint32_t i;

    NGPON_ROGUE_INDICATION_PARAMS rogue_param; 

    device_error = ag_drv_ngpon_tx_int_ism1_get(&tx_int);
    if (device_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Failed to get Tx interrupts status(masked)");
        return;
    }

    device_error = ag_drv_ngpon_tx_int_isr1_get(&unmasked_tx_int);
    if (device_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Failed to read Tx interrupts");
        return;
    }

    device_error = ag_drv_ngpon_tx_int_isr1_set(&unmasked_tx_int);
    if (device_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Failed to clean Tx interrupts");
        return;
    }
    /*
     * Shut all DBRx interrupts reported in ISR0
     */
    for (i=0; i< BITS_PER_WORD; i++)
    {
       device_error = ag_drv_ngpon_tx_int_isr0_set(i, 1);
       if (device_error != PON_NO_ERROR)
       {
           p_log(ge_onu_logger.sections.isr.debug.general_id, "Failed to clean Tx interrupts (ISR0)");
           return;
       }
    }

    /* Tx PLOAM FIFO 0 */
    if (unmasked_tx_int.tx_plm_0)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "TX PLOAM FIFO 0 interrupt");
        /* Move PLOAM which waits at the SW buffer head to HW buffer */
        fetch_us_ploam();
    }

    if (unmasked_tx_int.tx_plm_1)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "TX PLOAM buffer 1 interrupt");
    }

    if (unmasked_tx_int.tx_plm_2)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "TX PLOAM buffer 2 interrupt");
    }

    if (unmasked_tx_int.fe_data_overun)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "An overrun occurred in the data forntend buffer interrupt");
    }

    if (unmasked_tx_int.pd_underun)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "A Packet Descriptor underrun occurred interrupt");
    }

    if (unmasked_tx_int.pd_overun)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "A Packet Descriptor overrun occurred interrupt");
    }

    if (unmasked_tx_int.af_err)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Access FIFO error interrupt");
    }
    if (unmasked_tx_int.rog_dif)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Rogue ONU detected - Difference between generated TXEN and examined TXEN for longer than expected interrupt");

        rogue_param.rogue_state = BDMF_TRUE;
        rogue_param.rogue_type = TX_FAULT_ROGUE_MODE ;

        xgpon_error = send_message_to_pon_task (PON_EVENT_USER_INDICATION, PON_INDICATION_ROGUE_ONU, rogue_param);

        if ( xgpon_error != BDMF_ERR_OK )
        {
            /* Log */
            p_log (ge_onu_logger.sections.isr.debug.general_id, "Unable to send user indication: error %d !", xgpon_error);
        }

        /* clear rogue interrupt */
        ngpon_cfg_rogue_onu_interrupts (ROGUE_ONU_TX_INT_CLEAR, 1, 0);

        /* disable the rogue interrupt */
        ngpon_cfg_rogue_onu_interrupts (ROGUE_ONU_TX_INT_MASK, 1, 0);

        /* Log */
        p_log ( ge_onu_logger.sections.isr.debug.tx_event_id, "Gpon TX: rogue_onu interrupt !!!" ) ;
    }

    if (unmasked_tx_int.rog_len)
    {
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Rogue ONU detected - transmission longer than predefined time window error interrupt");

        rogue_param.rogue_state = BDMF_TRUE;
        rogue_param.rogue_type = TX_MONITOR_ROGUE_MODE;

        xgpon_error = send_message_to_pon_task (PON_EVENT_USER_INDICATION, PON_INDICATION_ROGUE_ONU, rogue_param);

        if ( xgpon_error != BDMF_ERR_OK )
        {
            /* Log */
            p_log (ge_onu_logger.sections.isr.debug.general_id, "Unable to send user indication: error %d !", xgpon_error);
        }

        /* clear rogue interrupt */
        ngpon_cfg_rogue_onu_interrupts (ROGUE_ONU_TX_INT_CLEAR, 0, 1);

        /* disable the rogue interrupt*/
        ngpon_cfg_rogue_onu_interrupts (ROGUE_ONU_TX_INT_MASK, 0, 1);
    }
}

void handle_rx_interrupt_lof_lcdg(ngpon_rx_int_ism * rx_int_ism)
{
    PON_ERROR_DTE                   device_error, xgpon_error;
    bdmf_boolean                   frame_sync_state;        
    bdmf_boolean                   xgem_sync_state;         
    bdmf_boolean                   ponid_consist_state;     
    bdmf_boolean                   fwi_state; 
    OPERATION_SM_PARAMS            operation_sm_params;
    PON_OAM_ALARM_STATUS_DTE       alarm_status ;

    /* Check LOF & LCDG */
    device_error = ag_drv_ngpon_rx_gen_rcvrstat_get(&frame_sync_state, &xgem_sync_state, &ponid_consist_state, &fwi_state);
    if (device_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to get receiver_status due to:Driver error %d !", device_error);
    }

    if (rx_int_ism->lof_state_change)
    {
        /* Set the Operation SM according to HW */
        operation_sm_params.lods_state = frame_sync_state ? BDMF_FALSE : BDMF_TRUE;

        p_log ( ge_onu_logger.sections.isr.debug.rx_event_id, "pon LOF state change interrupt - LOF alarm %s", (operation_sm_params.lods_state)? "ON" : "Cleared");


        /* If LOF occurs while in operation state, consider it always as loss of frame on */
        if (operation_sm_params.lods_state == BDMF_TRUE)
        {
            /* LODS alarm set */
            p_operation_sm(OPERATION_EVENT_DETECT_LODS, &operation_sm_params);
        }
        else
        {
            /* LODS also cleared ? */
            p_operation_sm(OPERATION_EVENT_DS_FRAME_SYNC, &operation_sm_params);
        }

        pon_serdes_lof_fixup_irq(operation_sm_params.lods_state);
    }
    if (rx_int_ism->lcdg_state_change)
    {
        alarm_status = (xgem_sync_state == BDMF_TRUE) ? CE_OAM_ALARM_STATUS_ON : CE_OAM_ALARM_STATUS_OFF;
        xgpon_error = send_message_to_pon_task(PON_EVENT_OAM, PON_INDICATION_LCDG , alarm_status);
    }
}

void handle_sfc_indication ()
{
    OPERATION_SM_PARAMS            operation_sm_params;
    
    /*
     *  Tuning_Control handling - SFC match
     */
    g_xgpon_db.rx_ier_vector.sfc_indication_mask = BDMF_FALSE ;

    if (g_xgpon_db.link_params.sub_sm_state == o5_2_pending_sub_state)  /* handle SFC_MATCH only when Activation SM 
                                                                           is in 'pending for tuning' state */
    {
        p_operation_sm( OPERATION_EVENT_SFC_MATCH, &operation_sm_params);
    }
}


void ds_ploam_hanlder(NGPON_DS_PLOAM * ploam_ptr)
{
    OPERATION_SM_PARAMS  operation_sm_params;
    NGPON_DS_PLOAM tmp_ploam_p;
    PON_ERROR_DTE  stack_error;

    memcpy(&tmp_ploam_p, ploam_ptr, sizeof(NGPON_DS_PLOAM));

    dump_ds_ploam_msg((uint8_t *)(&tmp_ploam_p));
        /* Check ploam MIC and other */
    stack_error = verify_ds_ploam_packet(&tmp_ploam_p);
    if (stack_error != PON_NO_ERROR)
            return;

    tmp_ploam_p.onu_id = ntohs(tmp_ploam_p.onu_id);
    operation_sm_params.ploam_message_ptr = &tmp_ploam_p;

    switch (ploam_ptr->message_id)
    {
        case DS_BURST_PROFILE_MESSAGE_ID:
        {
            p_operation_sm(OPERATION_EVENT_BURST_PROFILE_PLOAM, & operation_sm_params);
            break;
        }
        case DS_ASSIGN_ONU_ID_MESSAGE_ID:
        {
            operation_sm_params.ploam_message_ptr->message.assign_onu_id.assigned_onu_id = 
                ntohs(operation_sm_params.ploam_message_ptr->message.assign_onu_id.assigned_onu_id);
            p_operation_sm(OPERATION_EVENT_ASSIGN_ONU_ID_PLOAM, & operation_sm_params);
            break;
        }
        case DS_RANGING_TIME_MESSAGE_ID:
        {
            p_operation_sm(OPERATION_EVENT_RANGING_TIME_PLOAM, & operation_sm_params);
            break;
        }
        case DS_DEACTIVATE_ONU_ID_MESSAGE_ID:
        {
            p_operation_sm(OPERATION_EVENT_DEACTIVATE_ONU_ID_PLOAM, & operation_sm_params);
            break;
        }
        case DS_DISABLE_SERIAL_NUMBER_MESSAGE_ID:
        {
            p_operation_sm(OPERATION_EVENT_DISABLE_SERIAL_NUMBER_PLOAM, & operation_sm_params);
            break;
        }
        case DS_REQUEST_REGISTRATION_MESSAGE_ID:
        {
            p_operation_sm(OPERATION_EVENT_REQUEST_REGISTRATION_PLOAM, & operation_sm_params);
            break;
        }
        case DS_ASSIGN_ALLOC_ID_MESSAGE_ID:
        {
            operation_sm_params.ploam_message_ptr->message.assign_alloc_id.alloc_16 = 
                ntohs(operation_sm_params.ploam_message_ptr->message.assign_alloc_id.alloc_16);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.assign_alloc_id_message_id, 
                "DS_ASSIGN_ALLOC_ID_MESSAGE_ID - Alloc ID %d", 
                operation_sm_params.ploam_message_ptr->message.assign_alloc_id.alloc_id.alloc_id);

            p_operation_sm(OPERATION_EVENT_ASSIGN_ALLOC_ID_PLOAM, &operation_sm_params);
            break;
        }
        case DS_KEY_CONTROL_MESSAGE_ID:
        {
            p_operation_sm(OPERATION_EVENT_KEY_CONTROL_PLOAM, &operation_sm_params);
            break;
        }
        case DS_CHANNEL_PROFILE_MESSAGE_ID:
        {
            p_operation_sm(OPERATION_EVENT_CHANNEL_PROFILE_PLOAM, &operation_sm_params);
            break;
        }
        case DS_SYSTEM_PROFILE_MESSAGE_ID:
        {
            p_operation_sm(OPERATION_EVENT_SYSTEM_PROFILE_PLOAM, &operation_sm_params);
            break;
        }
        case DS_TUNING_CONTROL_MESSAGE_ID:
        {
            p_operation_sm(OPERATION_EVENT_TUNING_CONTROL_PLOAM, &operation_sm_params);
            break;
        }
        case  DS_PROTECTION_CONTROL_MESSAGE_ID:
        {
            p_operation_sm(OPERATION_EVENT_PROTECTION_CONTROL_PLOAM, &operation_sm_params);
            break;
        }
        default:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.unknown_message_id,
                 "Message ID: Unknown: %d(0x%02X)", ploam_ptr->message_id, ploam_ptr->message_id);
         break;
        }
    }
}

PON_ERROR_DTE verify_ds_ploam_packet(NGPON_DS_PLOAM *ploam_ptr)
{
    /* The buffer is twice than necessary to prevent over flow when calculating MIC using CMAC 128
     instead of CMAC 64(All our  CMAC fuctions are built for CMAC 128)       */
    uint8_t calculated_mic[PLOAM_MIC_SIZE * 2];


             
    /* Check MIC Error */
    /* Request Registration/Deactivate ONU /Disable ONU MIC  should be calculatesd using Default Key */
    if (DS_REQUEST_REGISTRATION_MESSAGE_ID != ploam_ptr->message_id &&
        DS_DEACTIVATE_ONU_ID_MESSAGE_ID != ploam_ptr->message_id &&
        DS_DISABLE_SERIAL_NUMBER_MESSAGE_ID != ploam_ptr->message_id &&
        DS_TUNING_CONTROL_MESSAGE_ID != ploam_ptr -> message_id &&
        /* The whole PLOAM is in Network order, we have to convert to Host */
        (!((DS_BURST_PROFILE_MESSAGE_ID == ploam_ptr->message_id) && (BROADCAST_ONU_ID == ntohs(ploam_ptr->onu_id)))) &&
        (!((DS_BURST_PROFILE_MESSAGE_ID == ploam_ptr->message_id) && (XGS_BROADCAST_ONU_ID == ntohs(ploam_ptr->onu_id)))) &&
        (! ((DS_ASSIGN_ONU_ID_MESSAGE_ID == ploam_ptr -> message_id) && (BROADCAST_ONU_ID == ntohs(ploam_ptr -> onu_id) ))) &&
        (! ((DS_CHANNEL_PROFILE_MESSAGE_ID == ploam_ptr -> message_id) && (BROADCAST_ONU_ID == ntohs(ploam_ptr -> onu_id) ))) &&
        (! ((DS_SYSTEM_PROFILE_MESSAGE_ID == ploam_ptr -> message_id) && (BROADCAST_ONU_ID == ntohs(ploam_ptr -> onu_id) ))) &&
        (! ((DS_PROTECTION_CONTROL_MESSAGE_ID == ploam_ptr -> message_id) && (BROADCAST_ONU_ID == ntohs(ploam_ptr -> onu_id) ))) &&
        (! ((DS_RANGING_TIME_MESSAGE_ID == ploam_ptr -> message_id) && (BROADCAST_ONU_ID == ntohs(ploam_ptr -> onu_id) ))) &&
        (! ((DS_RANGING_TIME_MESSAGE_ID == ploam_ptr->message_id) && (g_xgpon_db.link_params.operation_sm.activation_state == OPERATION_STATE_O9)))  
      )
    {

        ngpon_generate_mic_for_ds_ploam((uint8_t *) ploam_ptr, calculated_mic,
                g_xgpon_db.onu_params.ploam_ik_in_bytes);
        if (memcmp(ploam_ptr->mic, calculated_mic, PLOAM_MIC_SIZE) != 0)
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id,
                    "DS PLOAM %d had MIC error, ONU-specific key used:",
                    ploam_ptr->message_id);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id,
                    "OLT MIC: %02X %02X %02X %02X %02X %02X %02X %02X",
                    ploam_ptr->mic[0], ploam_ptr->mic[1], ploam_ptr->mic[2],
                    ploam_ptr->mic[3], ploam_ptr->mic[4], ploam_ptr->mic[5],
                    ploam_ptr->mic[6], ploam_ptr->mic[7]);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id,
                    "ONU MIC with default key: %02X %02X %02X %02X %02X %02X %02X %02X",
                    calculated_mic[0], calculated_mic[1], calculated_mic[2],
                    calculated_mic[3], calculated_mic[4], calculated_mic[5],
                    calculated_mic[6], calculated_mic[7]);
            return (NGPON_ERROR_MIC_ERROR);
        }
    }
    else
    {
        /* XXX: SWRDP-1337 */
        if (DS_DEACTIVATE_ONU_ID_MESSAGE_ID == ploam_ptr->message_id)
            return PON_NO_ERROR;



        /* Calculate MIC using Default PLOAM Key */
        ngpon_generate_mic_for_ds_ploam((uint8_t *) ploam_ptr, calculated_mic,
                PLOAM_IK_DEFAULT);
        if (memcmp(ploam_ptr->mic, calculated_mic, PLOAM_MIC_SIZE) != 0)
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id,
                    "DS PLOAM had MIC error:");
            p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id,
                    "OLT MIC: %02X %02X %02X %02X %02X %02X %02X %02X",
                    ploam_ptr->mic[0], ploam_ptr->mic[1], ploam_ptr->mic[2],
                    ploam_ptr->mic[3], ploam_ptr->mic[4], ploam_ptr->mic[5],
                    ploam_ptr->mic[6], ploam_ptr->mic[7]);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id,
                    "ONU MIC with default key: %02X %02X %02X %02X %02X %02X %02X %02X",
                    calculated_mic[0], calculated_mic[1], calculated_mic[2],
                    calculated_mic[3], calculated_mic[4], calculated_mic[5],
                    calculated_mic[6], calculated_mic[7]);
            return (NGPON_ERROR_MIC_ERROR);
        }

    }

    return PON_NO_ERROR;
}



void dump_ds_ploam_msg(uint8_t * xi_ploam_ptr)
{
    uint32_t i;
    NGPON_DS_PLOAM  *ds_ploam =(NGPON_DS_PLOAM *) xi_ploam_ptr;

    p_log(ge_onu_logger.sections.stack.downstream_ploam.print_ploam,"DS PLOAM received: ");
    for(i = 0; i < NGPON_PLOAM_LEN_IN_BYTES_WITH_SPC_AND_MIC; i+=8)
    {
        p_log(ge_onu_logger.sections.stack.downstream_ploam.print_ploam,
                "%02X %02X %02X %02X %02X %02X %02X %02X",
                xi_ploam_ptr[i + 0], xi_ploam_ptr[i + 1], xi_ploam_ptr[i + 2], xi_ploam_ptr[i + 3],
                xi_ploam_ptr[i + 4], xi_ploam_ptr[i + 5], xi_ploam_ptr[i + 6], xi_ploam_ptr[i + 7]);
    }

    p_log(ge_onu_logger.sections.stack.downstream_ploam.print_ploam,
            "ONU ID %u(0x%03X) SeqNo %u(0x%02X)"
          ,ntohs(ds_ploam->onu_id), ntohs(ds_ploam->onu_id),
             ds_ploam->sequence_number, ds_ploam->sequence_number);

    switch(ds_ploam->message_id)
    {
        case DS_BURST_PROFILE_MESSAGE_ID:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id,
            "Message ID: PROFILE(0x%02X)", ds_ploam->message_id);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id,
            "profile_version: 0x%02X,profile_index: 0x%02X",
            ds_ploam->message.burst_profile.profile_index.version, ds_ploam->message.burst_profile.profile_index.index);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id,
            "fec_indiciation: 0x%02X, delimiter_length: 0x%02X",
            ds_ploam->message.burst_profile.fec_indiciation, ds_ploam->message.burst_profile.delimiter_length);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id,"delimiter:"
            "%02X %02X %02X %02X %02X %02X %02X %02X",
            ds_ploam->message.burst_profile.delimiter[0], ds_ploam->message.burst_profile.delimiter[1],
            ds_ploam->message.burst_profile.delimiter[2], ds_ploam->message.burst_profile.delimiter[3],
            ds_ploam->message.burst_profile.delimiter[4], ds_ploam->message.burst_profile.delimiter[5],
            ds_ploam->message.burst_profile.delimiter[6], ds_ploam->message.burst_profile.delimiter[7]);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id,
                    "preamble_length: 0x%02X, "
                    "preamble_repeat_counter: 0x%02X ",
                    ds_ploam->message.burst_profile.preamble_length,
                    ds_ploam->message.burst_profile.preamble_repeat_counter);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id,"preamble:"
            "%02X %02X %02X %02X %02X %02X %02X %02X",
            ds_ploam->message.burst_profile.preamble[0], ds_ploam->message.burst_profile.preamble[1],
            ds_ploam->message.burst_profile.preamble[2], ds_ploam->message.burst_profile.preamble[3],
            ds_ploam->message.burst_profile.preamble[4], ds_ploam->message.burst_profile.preamble[5],
            ds_ploam->message.burst_profile.preamble[6], ds_ploam->message.burst_profile.preamble[7]);

            p_log(ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id,"pon_tag:"
            "%02X %02X %02X %02X %02X %02X %02X %02X",
            ds_ploam->message.burst_profile.pon_tag[0], ds_ploam->message.burst_profile.pon_tag[1],
            ds_ploam->message.burst_profile.pon_tag[2], ds_ploam->message.burst_profile.pon_tag[3],
            ds_ploam->message.burst_profile.pon_tag[4], ds_ploam->message.burst_profile.pon_tag[5],
            ds_ploam->message.burst_profile.pon_tag[6], ds_ploam->message.burst_profile.pon_tag[7]);
            break;
        }
        case DS_ASSIGN_ONU_ID_MESSAGE_ID:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.assign_onu_id_message_id,
                    "Message ID: ASSIGN_ONU_ID(0x%02X)",ds_ploam->message_id );
            p_log(ge_onu_logger.sections.stack.downstream_ploam.assign_onu_id_message_id,
                    "assigned_onu_id: %u(0x%03X)",
                    ntohs(ds_ploam->message.assign_onu_id.assigned_onu_id),
                  ntohs(ds_ploam->message.assign_onu_id.assigned_onu_id));
            p_log(ge_onu_logger.sections.stack.downstream_ploam.assign_onu_id_message_id,
                    "serial_number: %02X %02X %02X %02X %02X %02X %02X %02X",
                    ds_ploam->message.assign_onu_id.serial_number[0],
                    ds_ploam->message.assign_onu_id.serial_number[1],
                    ds_ploam->message.assign_onu_id.serial_number[2],
                    ds_ploam->message.assign_onu_id.serial_number[3],
                    ds_ploam->message.assign_onu_id.serial_number[4],
                    ds_ploam->message.assign_onu_id.serial_number[5],
                    ds_ploam->message.assign_onu_id.serial_number[6],
                    ds_ploam->message.assign_onu_id.serial_number[7]);
            break;
        }
        case DS_RANGING_TIME_MESSAGE_ID:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
                    "Message ID: RANGING_TIME(0x%02X)", ds_ploam->message_id);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
                    "Ranging Delay interpreted as %s(0x%X)",
                    ds_ploam->message.ranging_time.options.absolute_or_relative == 1 ?
                    "Absolute(Ignore Sign value)": "Relative", ds_ploam->message.ranging_time.options.absolute_or_relative);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
                    "Sign: %s the current EqD by the specified value ",
                    ds_ploam->message.ranging_time.options.absolute_or_relative == 1  ? "Not Relevant":
                   (ds_ploam->message.ranging_time.options.sign==0 ?"positive: increase":"negative: decrease"));
            p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id,
                   "Delay: %02X %02X %02X %02X",
                    ds_ploam->message.ranging_time.equalization_delay[0],
                    ds_ploam->message.ranging_time.equalization_delay[1],
                    ds_ploam->message.ranging_time.equalization_delay[2],
                    ds_ploam->message.ranging_time.equalization_delay[3]);
            break;
        }
        case DS_DEACTIVATE_ONU_ID_MESSAGE_ID:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.deactivate_onu_id_message_id,
                     "Message ID: DEACTIVATE_ONU_ID(0x%02X)", ds_ploam->message_id );
            break;
        }
        case DS_DISABLE_SERIAL_NUMBER_MESSAGE_ID:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.disable_serial_number_message_id,
                    "Message ID: DISABLE_SERIAL_NUMBER(0x%02X)\n"
                    "Control: 0x%02X(%s)",
                    ds_ploam->message_id,
                    ds_ploam->message.disable_serial_number.control,
                    ds_ploam->message.disable_serial_number.control == SERIAL_NUMBER_UNICAST_ENABLE ? "ENABLE ONU With the following SN":
                   (ds_ploam->message.disable_serial_number.control == SERIAL_NUMBER_BROADCAST_ENABLE ? "ENABLE ALL":
                     (ds_ploam->message.disable_serial_number.control == SERIAL_NUMBER_UNICAST_DISABLE ? "DISABLE ONU With the following SN" :
                       (ds_ploam->message.disable_serial_number.control == SERIAL_NUMBER_BROADCAST_DISABLE ? "DISABLE ALL": "UNKNOWN"))));
            p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id,
                    "Serial_number: %02X %02X %02X %02X %02X %02X %02X %02X",
                    ds_ploam->message.disable_serial_number.serial_number[0],
                    ds_ploam->message.disable_serial_number.serial_number[1],
                    ds_ploam->message.disable_serial_number.serial_number[2],
                    ds_ploam->message.disable_serial_number.serial_number[3],
                    ds_ploam->message.disable_serial_number.serial_number[4],
                    ds_ploam->message.disable_serial_number.serial_number[5],
                    ds_ploam->message.disable_serial_number.serial_number[6],
                    ds_ploam->message.disable_serial_number.serial_number[7]);
            break;
        }
        case DS_REQUEST_REGISTRATION_MESSAGE_ID:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.request_registration_message_id,
                      "Message ID: REQUEST_REGISTRATION(0x%02X)", ds_ploam->message_id );
            break;
        }
        case DS_ASSIGN_ALLOC_ID_MESSAGE_ID:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.assign_alloc_id_message_id,
                "Message ID: ASSIGN_ALLOC_ID(0x%02X)", ds_ploam->message_id);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.assign_alloc_id_message_id,
                "alloc_id = %u(0x%04X), alloc_type = %s",
                ntohs(ds_ploam->message.assign_alloc_id.alloc_16), 
                ntohs(ds_ploam->message.assign_alloc_id.alloc_16),
                ds_ploam->message.assign_alloc_id.alloc_id_type == ALLOC_TYPE_DEALLOC ?
                    "ALLOC_TYPE_DEALLOC" : "ALLOC_TYPE_XGEM");
            break;
        }
        case DS_KEY_CONTROL_MESSAGE_ID:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id,
                      "Message ID: KEY_CONTROL(0x%02X)",ds_ploam->message_id);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id,
                     " control = %s,"
                     " key_index = %s,"
                   , ds_ploam->message.key_control.control_flag.control == KEY_CONTROL_GENERATE_NEW_KEY ? "GENERATE_NEW_KEY" : "CONFIRM_EXISTING_KEY"
                   , ds_ploam->message.key_control.key_index.index == KEY_INDEX_FIRST_KEY ? " KEY_INDEX_FIRST_KEY":
                     (ds_ploam->message.key_control.key_index.index == KEY_INDEX_SECOND_KEY ? " KEY_INDEX_SECOND_KEY": "Unknown"));

            if (ds_ploam->message.key_control.key_length == 0)
            {
                 p_log(ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id," key_length = 256 bytes");
            }
            else
            {
                 p_log(ge_onu_logger.sections.stack.downstream_ploam.key_control_message_id,
                       " key_length = %u Bytes", ds_ploam->message.key_control.key_length);
            }
            break;
        }
        case DS_SLEEP_ALLOW_MESSAGE_ID:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.sleep_allow_message_id,
                      "Message ID: SLEEP_ALLOW(0x%02X)",ds_ploam->message_id );
            p_log(ge_onu_logger.sections.stack.downstream_ploam.sleep_allow_message_id,
                      " control = %s",ds_ploam->message.sleep_allow.control.sleep_allowed == SLEEP_ALLOW_ON ? "SLEEP_ALLOW_ON" : "SLEEP_ALLOW_OFF");
            break;
        }
        case DS_CHANNEL_PROFILE_MESSAGE_ID:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.sleep_allow_message_id,
                      "Message ID: CHANNEL_PROFILE(0x%02X)",ds_ploam->message_id );
           p_log(ge_onu_logger.sections.stack.downstream_ploam.channel_profile_message_id,"Control octet contents:");
           p_log(ge_onu_logger.sections.stack.downstream_ploam.channel_profile_message_id,"_This_ channel indicator         %d", ds_ploam->message.channel_profile.twdm.control.this_channel_indicator);
           p_log(ge_onu_logger.sections.stack.downstream_ploam.channel_profile_message_id,"DS void indicator                %d",  ds_ploam->message.channel_profile.twdm.control.ds_void_indicator);
           p_log(ge_onu_logger.sections.stack.downstream_ploam.channel_profile_message_id,"US void indicator                %d",  ds_ploam->message.channel_profile.twdm.control.us_void_indicator);
           p_log(ge_onu_logger.sections.stack.downstream_ploam.channel_profile_message_id,"Version                          %d\n",  ds_ploam->message.channel_profile.twdm.version.profile_version);

           p_log(ge_onu_logger.sections.stack.downstream_ploam.channel_profile_message_id,"PON ID                           0x%08x",ntohl(ds_ploam->message.channel_profile.twdm.pon_id));
           p_log(ge_onu_logger.sections.stack.downstream_ploam.channel_profile_message_id,"Channel Partition Indicator      %d", ds_ploam->message.channel_profile.twdm.channel_partition.index);

           p_log(ge_onu_logger.sections.stack.downstream_ploam.channel_profile_message_id,"Default Response Channel PON ID  0x%08x", ntohl(ds_ploam->message.channel_profile.twdm.default_response_channel));


           p_log(ge_onu_logger.sections.stack.downstream_ploam.channel_profile_message_id,"US channel index                 %d", ds_ploam->message.channel_profile.twdm.uwlch_id.channel_id);

           break ;
        }
        case DS_SYSTEM_PROFILE_MESSAGE_ID:
          {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.system_profile_message_id,
                      "Message ID: SYSTEM_PROFILE(0x%02X)",ds_ploam->message_id );
            p_log(ge_onu_logger.sections.stack.downstream_ploam.system_profile_message_id,"Version                             0x%02x", ds_ploam->message.system_profile.version.version);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.system_profile_message_id,"NGPON2 System ID                    (0x%02x 0x%02x 0x%02x)",
               ds_ploam->message.system_profile.ng2sys_id[0],
               ds_ploam->message.system_profile.ng2sys_id[1],
               ds_ploam->message.system_profile.ng2sys_id[2]
            );
            p_log(ge_onu_logger.sections.stack.downstream_ploam.system_profile_message_id,"Upstream Operation Wavelength bands 0x%02x", ds_ploam->message.system_profile.us_operation_wave_band.twdm);           
            p_log(ge_onu_logger.sections.stack.downstream_ploam.system_profile_message_id,"TWDM Channel Count                  0x%02x", ds_ploam->message.system_profile.twdm_channel_count.ch_profile_num);           
            p_log(ge_onu_logger.sections.stack.downstream_ploam.system_profile_message_id,"TWDM Channel Spacing                0x%02x", ds_ploam->message.system_profile.channel_spacing_twdm);           
            p_log(ge_onu_logger.sections.stack.downstream_ploam.system_profile_message_id,"TWDM Upstream MSE                   0x%02x", ds_ploam->message.system_profile.us_mse_twdm);           
            p_log(ge_onu_logger.sections.stack.downstream_ploam.system_profile_message_id,"FSR/TWDM                            0x%02x 0x%02x", 
                     ds_ploam->message.system_profile.fsr_twdm[0],
                     ds_ploam->message.system_profile.fsr_twdm[1]
            );           
            p_log(ge_onu_logger.sections.stack.downstream_ploam.system_profile_message_id,"TWDM AMCC Control (Use AMCC)        0x%02x", ds_ploam->message.system_profile.amcc_control.amcc_use_flag);           
            p_log(ge_onu_logger.sections.stack.downstream_ploam.system_profile_message_id,"TWDM AMCC Control (Min accuracy)    0x%02x", ds_ploam->message.system_profile.amcc_control.min_calibration_accuracy_for_activation_for_in_band); 
            p_log(ge_onu_logger.sections.stack.downstream_ploam.system_profile_message_id,"Loose calibration bound             0x%02x", ds_ploam->message.system_profile.ptp_wdm_calibration.min_calibration_accuracy_for_activation); 
            p_log(ge_onu_logger.sections.stack.downstream_ploam.system_profile_message_id,"PtP related fields skipped");
 
            break;
        }
        case DS_TUNING_CONTROL_MESSAGE_ID:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id,
                      "Message ID: TUNING_CONTROL(0x%02X)",ds_ploam->message_id );
            p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id,"Operation Code                      0x%02x", ds_ploam->message.tuning_control.operation_code);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id,"Scheduled SFC                       0x%04x", ntohs(ds_ploam->message.tuning_control.scheduled_sfc));
            p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id,"Rollback Flag                       0x%02x", ds_ploam->message.tuning_control.rollback_flag);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id,"Target DS PON ID                    0x%08x", ntohl(ds_ploam->message.tuning_control.target_ds_pon_id));
            p_log(ge_onu_logger.sections.stack.downstream_ploam.tuning_control_message_id,"Target US PON ID                    0x%08x", ntohl(ds_ploam->message.tuning_control.target_us_pon_id));

            break ;
        }

        case  DS_PROTECTION_CONTROL_MESSAGE_ID:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.protection_control_message_id,
                      "Message ID: PROTECTION_CONTROL(0x%02X)",ds_ploam->message_id );
            p_log(ge_onu_logger.sections.stack.downstream_ploam.protection_control_message_id,"Target DS PON ID                       0x%08x", ntohl(ds_ploam->message.protection_control.protect_ds_pon_id));
            p_log(ge_onu_logger.sections.stack.downstream_ploam.protection_control_message_id,"Target US PON ID                       0x%08x", ntohl(ds_ploam->message.protection_control.protect_us_pon_id));

            break ;
        }
        default:
        {
            p_log(ge_onu_logger.sections.stack.downstream_ploam.unknown_message_id,
                    "Message ID: Unknown: %d(0x%02X)",
                    ds_ploam->message_id, ds_ploam->message_id);
            break;
        }
    }

    p_log(ge_onu_logger.sections.stack.downstream_ploam.print_ploam,
           "MIC: %02X %02X %02X %02X %02X %02X %02X %02X",
           xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_NO_MIC + 0], xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_NO_MIC + 1],
           xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_NO_MIC + 2], xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_NO_MIC + 3],
           xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_NO_MIC + 4], xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_NO_MIC + 5],
           xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_NO_MIC + 6], xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_NO_MIC + 7]);
    p_log(ge_onu_logger.sections.stack.downstream_ploam.print_ploam,
           "SFC: %02X %02X %02X %02X %02X %02X %02X",
           xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_WITH_MIC + 0], xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_WITH_MIC + 1],
           xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_WITH_MIC + 2], xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_WITH_MIC + 3],
           xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_WITH_MIC + 4], xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_WITH_MIC + 5],
           xi_ploam_ptr[NGPON_PLOAM_LEN_IN_BYTES_WITH_MIC + 6]);
}

PON_ERROR_DTE ngpon_generate_mic_for_ds_ploam(uint8_t * xi_ploam, uint8_t *xo_mic, const uint8_t * xi_key)
{
   /* PLOAM-MIC = AES-CMAC(PLOAM_IK, Cdir | PLOAM_CONTENT, 64)
       Where Cdir is the direction code: Cdir = 0x01 for downstream and Cdir = 0x02 for upstream, and
       PLOAM_CONTENT denotes octets 1 to 40 of the PLOAM message. */
   uint8_t ploam_ds_for_mic[NGPON_PLOAM_LEN_FOR_MIC_CALACULATION];
   ploam_ds_for_mic[0]= DS_PLOAM_MIC_CDIR;
   memcpy(& ploam_ds_for_mic[1], xi_ploam, NGPON_PLOAM_LEN_IN_BYTES_NO_MIC);
   AES_CMAC(xi_key,ploam_ds_for_mic, xo_mic, NGPON_PLOAM_LEN_FOR_MIC_CALACULATION);
   return(PON_NO_ERROR);
}

PON_ERROR_DTE  ngpon_cfg_rogue_onu_interrupts (PON_ROGUE_ONU_TX_INT_CMD cmd, int rogue_onu_diff, int rogue_onu_level)
{
    ngpon_tx_int_ier1    tx_int_ier;
    ngpon_tx_int_isr1    tx_int_isr;
    PON_ERROR_DTE        device_error;
    uint8_t              rogue_level_clr;
    uint8_t              rogue_diff_clr;
    uint8_t              source_select;

    
    memset (&tx_int_isr, 0, sizeof (tx_int_isr)) ;
    device_error = ag_drv_ngpon_tx_int_ier1_get(&tx_int_ier) ;   
    if(device_error != PON_NO_ERROR)
    {
        /* Report the SW error */
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Failed to get IER for Rogue ONU Tx interrupts");

        return device_error;
    }


    switch (cmd) 
    {
     case ROGUE_ONU_TX_INT_CLEAR:
        /*
         * We have to clear also via ROGUE_ONU_CONTROL Register
         */
        device_error = ag_drv_ngpon_tx_cfg_rogue_onu_ctrl_get(&rogue_level_clr, &rogue_diff_clr, &source_select);
        if (device_error != PON_NO_ERROR) 
        {  
           p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
              "Failed to read rouge detect parameters due to driver error %d\n", device_error);
           return PON_ERROR_DRIVER_ERROR;
        }
        rogue_level_clr = 0;
        rogue_diff_clr = 0;

        if (rogue_onu_diff)
        {
            rogue_diff_clr = 1 ;
            tx_int_isr.rog_dif = 1;
        }
        if (rogue_onu_level)
        {
            rogue_level_clr = 1;
            tx_int_isr.rog_len = 1; 
        }
        device_error = ag_drv_ngpon_tx_int_isr1_set(&tx_int_isr) ;


        device_error = ag_drv_ngpon_tx_cfg_rogue_onu_ctrl_set(rogue_level_clr, rogue_diff_clr, source_select);
        if (device_error != PON_NO_ERROR) 
        {  
           p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
              "Failed to configure rouge detect parameters due to driver error %d\n", device_error);
           return PON_ERROR_DRIVER_ERROR;
        }
        udelay (1) ;

        rogue_level_clr = 0;
        rogue_diff_clr = 0;

        device_error = ag_drv_ngpon_tx_cfg_rogue_onu_ctrl_set(rogue_level_clr, rogue_diff_clr, source_select);
        if (device_error != PON_NO_ERROR) 
        {  
           p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
              "Failed to configure rouge detect parameters due to driver error %d\n", device_error);
           return PON_ERROR_DRIVER_ERROR;
        }
        break;

     case ROGUE_ONU_TX_INT_MASK:
     case ROGUE_ONU_TX_INT_UNMASK:
        /*
            'Clean' is done wy writing 1 in Interrupt Status Reg 1
            'Mask' - by writing '0' to Interrupt Enable Reg 1
            'Unmask' - by writing '1' to Interrupt Enable Reg 1
         */
         if (rogue_onu_diff)
         {
             tx_int_ier.rog_dif = (cmd == ROGUE_ONU_TX_INT_MASK)? 0 : 1;
         }
         if (rogue_onu_level)
         {
              tx_int_ier.rog_len = (cmd == ROGUE_ONU_TX_INT_MASK)? 0 : 1; 
         }
         device_error = ag_drv_ngpon_tx_int_ier1_set(&tx_int_ier) ;   
         break;

    default:
       p_log(ge_onu_logger.sections.isr.debug.general_id, 
          "Illegal Rogue ONU interrupts configuration operation cmd=%d", cmd);
       return PON_ERROR_DRIVER_ERROR;

    }
    
    if(device_error != PON_NO_ERROR)
    {
        /* Report the SW error */
        p_log(ge_onu_logger.sections.isr.debug.general_id, "Failed to set IER/ISR for Rogue ONU Tx interrupts");

        return device_error;
    }
    return PON_NO_ERROR ;
}

PON_ERROR_DTE clear_interrupts (void)
{
    ngpon_tx_int_isr1        tx_interrupts ;
    uint8_t                  tcont_id ;
    ngpon_rx_int_isr         rx_interrupts ;
    PON_ERROR_DTE             stack_error ;

    for (tcont_id = NGPON_TRAFFIC_TCONT_ID_LOW;  tcont_id <= NGPON_TRAFFIC_TCONT_ID_HIGH; tcont_id++)
    {
      stack_error = ag_drv_ngpon_tx_int_isr0_set ( tcont_id, BDMF_TRUE ) ;   /* Writing "1" clears the interrupt source */
       if ( stack_error != PON_NO_ERROR )
       {
          return ( stack_error ) ;
       }
    }


    tx_interrupts.tx_plm_0         = BDMF_TRUE ;   /* Writing "1" clears the interrupt source */
    tx_interrupts.tx_plm_1         = BDMF_TRUE ;
    tx_interrupts.tx_plm_2         = BDMF_TRUE ;
    tx_interrupts.fe_data_overun   = BDMF_TRUE ;
    tx_interrupts.pd_underun       = BDMF_TRUE ;
    tx_interrupts.pd_overun        = BDMF_TRUE ;
    tx_interrupts.af_err           = BDMF_TRUE ;
    tx_interrupts.rog_dif          = BDMF_TRUE ;
    tx_interrupts.rog_len          = BDMF_TRUE ;
    tx_interrupts.tx_tcont_32_dbr  = BDMF_TRUE ;
    tx_interrupts.tx_tcont_33_dbr  = BDMF_TRUE ;
    tx_interrupts. tx_tcont_34_dbr = BDMF_TRUE ;
    tx_interrupts.tx_tcont_35_dbr  = BDMF_TRUE ;
    tx_interrupts.tx_tcont_36_dbr  = BDMF_TRUE ;
    tx_interrupts.tx_tcont_37_dbr  = BDMF_TRUE ;
    tx_interrupts.tx_tcont_38_dbr  = BDMF_TRUE ;
    tx_interrupts.tx_tcont_39_dbr  = BDMF_TRUE ;


    stack_error = ag_drv_ngpon_tx_int_isr1_set ( &tx_interrupts ) ;
    if ( stack_error != PON_NO_ERROR )
    {
        return ( stack_error ) ;
    }


    /* Clear RX Interrupts */

    rx_interrupts.tm_fifo_ovf                  = BDMF_TRUE ;
    rx_interrupts.bw_fifo_ovf                  = BDMF_TRUE ;
    rx_interrupts.aes_fail                     = BDMF_TRUE ;
    rx_interrupts.fwi_state_change             = BDMF_TRUE ;
    rx_interrupts.lof_state_change             = BDMF_TRUE ;
    rx_interrupts.lcdg_state_change            = BDMF_TRUE ;
    rx_interrupts.lb_fifo_ovf                  = BDMF_TRUE ;
    rx_interrupts.tod_update                   = BDMF_TRUE ;
    rx_interrupts.bw_dis_tx                    = BDMF_TRUE ;
    rx_interrupts.inv_key                      = BDMF_TRUE ;
    rx_interrupts.bwmap_rec_done               = BDMF_TRUE ;
    rx_interrupts.ponid_inconsist              = BDMF_TRUE ;
    rx_interrupts.inv_portid_change            = BDMF_TRUE ;
    rx_interrupts.fec_fatal_error              = BDMF_TRUE ;
    rx_interrupts.sfc_indication               = BDMF_TRUE ;
    rx_interrupts.amcc_grant                   = BDMF_TRUE ;
    rx_interrupts.dwba                         = 0 ;

    stack_error = ag_drv_ngpon_rx_int_isr_set ( & rx_interrupts ) ;
    if (stack_error != PON_NO_ERROR)
    {
        return ( stack_error ) ;
    }

    return (PON_NO_ERROR);
}

PON_ERROR_DTE disable_interrupts (bool rx, bool tx)
{
    ngpon_tx_int_ier1        tx_interrupts = {} ;
    uint8_t                  tcont_id ;
    ngpon_rx_int_ier         rx_interrupts = {} ;
    PON_ERROR_DTE            stack_error ;

    if (tx)
    {
        for (tcont_id = NGPON_TRAFFIC_TCONT_ID_LOW;  tcont_id <= NGPON_TRAFFIC_TCONT_ID_HIGH; tcont_id++)
        {
            stack_error = ag_drv_ngpon_tx_int_ier0_set ( tcont_id, BDMF_FALSE ) ;
            if ( stack_error != PON_NO_ERROR )
            {
                return ( stack_error ) ;
            }
        }

        stack_error = ag_drv_ngpon_tx_int_ier1_set ( &tx_interrupts ) ;
        if ( stack_error != PON_NO_ERROR )
        {
            return ( stack_error ) ;
        }
    }

    if (rx)
    {
        stack_error = ag_drv_ngpon_rx_int_ier_set ( & rx_interrupts ) ;
        if ( stack_error != PON_NO_ERROR )
        {
            return ( stack_error ) ;
        }
    }

    return (PON_NO_ERROR);
}

PON_ERROR_DTE enable_interrupts (bool rx, bool tx)
{
    PON_ERROR_DTE stack_error;

    if (tx)
    {
        stack_error = ag_drv_ngpon_tx_int_ier1_set (&g_xgpon_db.tx_ier_vector);
        if (stack_error != PON_NO_ERROR)
        {
            return stack_error ;
        }
    }

    if (rx)
    {
        stack_error = ag_drv_ngpon_rx_int_ier_set (&g_xgpon_db.rx_ier_vector);
        if (stack_error != PON_NO_ERROR)
        {
            return stack_error ;
        }
    }

    return PON_NO_ERROR ;
}

static uint8_t local_omci_buf[ OMCI_MAX_SIZE_WITH_NO_MIC+4];

static uint32_t ngpon_calc_omci_mic(uint8_t direction, uint8_t *buffer, uint32_t length)
{
  uint32_t mic[4] = {0, 0, 0, 0};

  memcpy (&local_omci_buf[1], buffer, length);

  local_omci_buf[0] = direction;

  AES_CMAC (g_xgpon_db.onu_params.omci_ik_in_bytes, local_omci_buf, (uint8_t *) &mic[0], length + 1);
  return htonl(mic[0]);
}


uint32_t ngpon_calc_ds_omci_mic(uint8_t *buffer, uint32_t length)
{
  return  ngpon_calc_omci_mic(NGPON_OMCI_DIR_DS, buffer, length);
}

uint32_t ngpon_calc_us_omci_mic(uint8_t *buffer, uint32_t length)
{
  return  ngpon_calc_omci_mic(NGPON_OMCI_DIR_US, buffer, length);
}




